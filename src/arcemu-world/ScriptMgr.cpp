/*
 * ArcEmu MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 * Copyright (C) 2008 <http://www.ArcEmu.org/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "StdAfx.h"
#if (!defined( WIN32 ) && !defined( WIN64 ) )
    #include <dlfcn.h>
    #include <unistd.h>
    #include <dirent.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <cstdlib>
    #include <cstring>
#endif

#include <svn_revision.h>
#define SCRIPTLIB_HIPART(x) ((x >> 16))
#define SCRIPTLIB_LOPART(x) ((x & 0x0000ffff))
#define SCRIPTLIB_VERSION_MINOR (BUILD_REVISION % 1000)
#define SCRIPTLIB_VERSION_MAJOR (BUILD_REVISION / 1000)

initialiseSingleton(ScriptMgr);
initialiseSingleton(HookInterface);

ScriptMgr::ScriptMgr()
{
	DefaultGossipScript = new GossipScript();
}

ScriptMgr::~ScriptMgr()
{

}

struct ScriptingEngine
{
#if (defined( WIN32 ) || defined( WIN64 ) )
	HMODULE Handle;
#else
	void* Handle;
#endif
	exp_script_register InitializeCall;
	uint32 Type;
};

void ScriptMgr::LoadScripts()
{
	if(!HookInterface::getSingletonPtr())
		new HookInterface;

	Log.Notice("Server","Loading External Script Libraries...");
	sLog.outString("");

	string start_path = Config.MainConfig.GetStringDefault( "Script", "BinaryLocation", "script_bin" ) + "\\";
	string search_path = start_path + "*.";

	vector< ScriptingEngine > ScriptEngines;

	/* Loading system for win32 */
#if (defined( WIN32 ) || defined( WIN64 ) )
	search_path += "dll";

	WIN32_FIND_DATA data;
	uint32 count = 0;
	HANDLE find_handle = FindFirstFile( search_path.c_str(), &data );
	if(find_handle == INVALID_HANDLE_VALUE)
		sLog.outError( "  No external scripts found! Server will continue to function with limited functionality." );
	else
	{
		do
		{
			string full_path = start_path + data.cFileName;
			HMODULE mod = LoadLibrary( full_path.c_str() );
			printf( "  %s : 0x%p : ", data.cFileName, reinterpret_cast< uint32* >( mod ));
			if( mod == 0 )
			{
				printf( "error!\n" );
			}
			else
			{
				// find version import
				exp_get_version vcall = (exp_get_version)GetProcAddress(mod, "_exp_get_version");
				exp_script_register rcall = (exp_script_register)GetProcAddress(mod, "_exp_script_register");
				exp_get_script_type scall = (exp_get_script_type)GetProcAddress(mod, "_exp_get_script_type");
				if(vcall == 0 || rcall == 0 || scall == 0)
				{
					printf("version functions not found!\n");
					FreeLibrary(mod);
				}
				else
				{
					uint32 version = vcall();
					uint32 stype = scall();
					if(SCRIPTLIB_LOPART(version) == SCRIPTLIB_VERSION_MINOR && SCRIPTLIB_HIPART(version) == SCRIPTLIB_VERSION_MAJOR)
					{
						if( stype & SCRIPT_TYPE_SCRIPT_ENGINE )
						{
							printf("v%u.%u : ", SCRIPTLIB_HIPART(version), SCRIPTLIB_LOPART(version));
							printf("delayed load.\n");

							ScriptingEngine se;
							se.Handle = mod;
							se.InitializeCall = rcall;
							se.Type = stype;

							ScriptEngines.push_back( se );
						}
						else
						{
							_handles.push_back(((SCRIPT_MODULE)mod));
							printf("v%u.%u : ", SCRIPTLIB_HIPART(version), SCRIPTLIB_LOPART(version));
							rcall(this);
							printf("loaded.\n");						
						}

						++count;
					}
					else
					{
						FreeLibrary(mod);
						printf("version mismatch!\n");						
					}
				}
			}
		}
		while(FindNextFile(find_handle, &data));
		FindClose(find_handle);
		sLog.outString("");
		Log.Notice("Server","Loaded %u external libraries.", count);
		sLog.outString("");

		Log.Notice("Server","Loading optional scripting engines...");
		for(vector<ScriptingEngine>::iterator itr = ScriptEngines.begin(); itr != ScriptEngines.end(); ++itr)
		{
			if( itr->Type & SCRIPT_TYPE_SCRIPT_ENGINE_LUA )
			{
				// lua :O
				if( Config.MainConfig.GetBoolDefault("ScriptBackends", "LUA", false) )
				{
					Log.Notice("Server","Initializing LUA script engine...");
					itr->InitializeCall(this);
					_handles.push_back( (SCRIPT_MODULE)itr->Handle );
				}
				else
				{
					FreeLibrary( itr->Handle );
				}
			}
			else if( itr->Type & SCRIPT_TYPE_SCRIPT_ENGINE_AS )
			{
				if( Config.MainConfig.GetBoolDefault("ScriptBackends", "AS", false) )
				{
					Log.Notice("Server","Initializing AngelScript script engine...");
					itr->InitializeCall(this);
					_handles.push_back( (SCRIPT_MODULE)itr->Handle );
				}
				else
				{
					FreeLibrary( (*itr).Handle );
				}
			}
			else
			{
				Log.Notice("Server","Unknown script engine type: 0x%.2X, please contact developers.", (*itr).Type );
				FreeLibrary( itr->Handle );
			}
		}
		Log.Notice("Server","Done loading script engines...");
	}
#else
	/* Loading system for *nix */
	struct dirent ** list = NULL;
	int filecount = scandir(PREFIX "/lib/", &list, 0, 0);
	uint32 count = 0;

	if(!filecount || !list || filecount < 0)
		sLog.outError("  No external scripts found! Server will continue to function with limited functionality.");
	else
	{
char *ext;
		while(filecount--)
		{
			ext = strrchr(list[filecount]->d_name, '.');
#ifdef HAVE_DARWIN
			if (ext != NULL && strstr(list[filecount]->d_name, ".0.dylib") == NULL && !strcmp(ext, ".dylib")) {
#else
                        if (ext != NULL && !strcmp(ext, ".so")) {
#endif
				string full_path = "../lib/" + string(list[filecount]->d_name);
				SCRIPT_MODULE mod = dlopen(full_path.c_str(), RTLD_NOW);
				printf("  %s : 0x%p : ", list[filecount]->d_name, mod);
				if(mod == 0)
					printf("error! [%s]\n", dlerror());
				else
				{
					// find version import
					exp_get_version vcall = (exp_get_version)dlsym(mod, "_exp_get_version");
					exp_script_register rcall = (exp_script_register)dlsym(mod, "_exp_script_register");
					exp_get_script_type scall = (exp_get_script_type)dlsym(mod, "_exp_get_script_type");
					if(vcall == 0 || rcall == 0 || scall == 0)
					{
						printf("version functions not found!\n");
						dlclose(mod);
					}
					else
					{
						int32 version = vcall();
						uint32 stype = scall();
						if(SCRIPTLIB_LOPART(version) == SCRIPTLIB_VERSION_MINOR && SCRIPTLIB_HIPART(version) == SCRIPTLIB_VERSION_MAJOR)
						{
							if( stype & SCRIPT_TYPE_SCRIPT_ENGINE )
							{
								printf("v%u.%u : ", SCRIPTLIB_HIPART(version), SCRIPTLIB_LOPART(version));
								printf("delayed load.\n");

								ScriptingEngine se;
								se.Handle = mod;
								se.InitializeCall = rcall;
								se.Type = stype;

								ScriptEngines.push_back( se );
							}
							else
							{
								_handles.push_back(((SCRIPT_MODULE)mod));
								printf("v%u.%u : ", SCRIPTLIB_HIPART(version), SCRIPTLIB_LOPART(version));
								rcall(this);
								printf("loaded.\n");						
							}

							++count;
						}
						else
						{
							dlclose(mod);
							printf("version mismatch!\n");						
						}
					}
				}
			}
			free(list[filecount]);
		}
		free(list);
		sLog.outString("");
		sLog.outString("Loaded %u external libraries.", count);
		sLog.outString("");

		sLog.outString("Loading optional scripting engines...");
		for(vector<ScriptingEngine>::iterator itr = ScriptEngines.begin(); itr != ScriptEngines.end(); ++itr)
		{
			if( itr->Type & SCRIPT_TYPE_SCRIPT_ENGINE_LUA )
			{
				// lua :O
				if( Config.MainConfig.GetBoolDefault("ScriptBackends", "LUA", false) )
				{
					sLog.outString("   Initializing LUA script engine...");
					itr->InitializeCall(this);
					_handles.push_back( (SCRIPT_MODULE)itr->Handle );
				}
				else
				{
					dlclose( itr->Handle );
				}
			}
			else if( itr->Type & SCRIPT_TYPE_SCRIPT_ENGINE_AS )
			{
				if( Config.MainConfig.GetBoolDefault("ScriptBackends", "AS", false) )
				{
					sLog.outString("   Initializing AngelScript script engine...");
					itr->InitializeCall(this);
					_handles.push_back( (SCRIPT_MODULE)itr->Handle );
				}
				else
				{
					dlclose( (*itr).Handle );
				}
			}
			else
			{
				sLog.outString("  Unknown script engine type: 0x%.2X, please contact developers.", (*itr).Type );
				dlclose( itr->Handle );
			}
		}
		sLog.outString("Done loading script engines...");
	}
#endif
}

void ScriptMgr::UnloadScripts()
{
	if(HookInterface::getSingletonPtr())
		delete HookInterface::getSingletonPtr();

	for(CustomGossipScripts::iterator itr = _customgossipscripts.begin(); itr != _customgossipscripts.end(); ++itr)
		(*itr)->Destroy();	 //deletes script !
	_customgossipscripts.clear();
	delete DefaultGossipScript;
	DefaultGossipScript=NULL;

	LibraryHandleMap::iterator itr = _handles.begin();
	for(; itr != _handles.end(); ++itr)
	{
#if (defined( WIN32 ) || defined( WIN64 ) )
		FreeLibrary(((HMODULE)*itr));
#else
		dlclose(*itr);
#endif
	}
	_handles.clear();
}

void ScriptMgr::register_creature_script(uint32 entry, exp_create_creature_ai callback)
{
	if(_creatures.find(entry) != _creatures.end())
		sLog.outError("ScriptMgr is trying to register a script for Creature ID: %u even if there's already one for that Creature. Remove one of those scripts.\n", entry);
	_creatures.insert( CreatureCreateMap::value_type( entry, callback ) );
}

void ScriptMgr::register_pet_script(uint32 entry, exp_create_creature_ai callback)
{
	if(_pets.find(entry) != _pets.end())
		sLog.outError("ScriptMgr is trying to register a script for Creature ID: %u even if there's already one for that Creature. Remove one of those scripts.\n", entry);
	_pets.insert( CreatureCreateMap::value_type( entry, callback ) );
}

void ScriptMgr::register_gameobject_script(uint32 entry, exp_create_gameobject_ai callback)
{
	if(_gameobjects.find(entry) != _gameobjects.end())
		sLog.outError("ScriptMgr is trying to register a script for GameObject ID: %u even if there's already one for that GameObject. Remove one of those scripts.\n", entry);
	_gameobjects.insert( GameObjectCreateMap::value_type( entry, callback ) );
}

void ScriptMgr::register_dummy_aura(uint32 entry, exp_handle_dummy_aura callback)
{
/*
	if(_auras.find(entry) != _auras.end())
		sLog.outError("ScriptMgr is trying to register a script for Aura ID: %u even if there's already one for that Aura. Remove one of those scripts.\n", entry);
	_auras.insert( HandleDummyAuraMap::value_type( entry, callback ) );
	*/
	SpellEntry *sp = dbcSpell.LookupEntryForced( entry );
	if( sp == NULL )
		return;
	if( sp->AuraPeriodicDummyTickHook  )
		sLog.outError("ScriptMgr is trying to register a script for Aura ID: %u even if there's already one for that Aura. Remove one of those scripts.\n", entry);
	sp->AuraPeriodicDummyTickHook = callback;
}

void ScriptMgr::register_dummy_spell(uint32 entry, exp_handle_dummy_spell callback)
{
//	if(_spells.find(entry) != _spells.end())
//		sLog.outError("ScriptMgr is trying to register a script for Spell ID: %u even if there's already one for that Spell. Remove one of those scripts.\n", entry);
//	_spells.insert( HandleDummySpellMap::value_type( entry, callback ) );
	SpellEntry *sp = dbcSpell.LookupEntryForced( entry );
	if( sp == NULL )
		return;
	if( sp->SpellDummyEffectScript  )
		sLog.outError("ScriptMgr is trying to register a script for Spell ID: %u even if there's already one for that Spell. Remove one of those scripts.\n", entry);
	sp->SpellDummyEffectScript = callback;
}

void ScriptMgr::register_gossip_script(uint32 entry, GossipScript * gs)
{
	CreatureInfo * ci = CreatureNameStorage.LookupEntry(entry);
	if(ci)
	{
		if(ci->gossip_script != DefaultGossipScript)
			sLog.outError("ScriptMgr is trying to register a gossip for Creature ID: %u even if there's already one for that Creature. Remove one of those gossips.\n", entry);
		ci->gossip_script = gs;
	}
	CreatureProto * cp = CreatureProtoStorage.LookupEntry(entry);
	if( cp )
		cp->NPCFLags |= UNIT_NPC_FLAG_GOSSIP;

	_customgossipscripts.insert(gs);
}

void ScriptMgr::register_go_gossip_script(uint32 entry, GossipScript * gs)
{
	GameObjectInfo * gi = GameObjectNameStorage.LookupEntry(entry);
	if(gi)
	{
		if(gi->gossip_script != NULL)
			sLog.outError("ScriptMgr is trying to register a gossip for GameObject ID: %u even if there's already one for that GameObject. Remove one of those gossips.\n", entry);
		gi->gossip_script = gs;
	}

	_customgossipscripts.insert(gs);
}

void ScriptMgr::register_quest_script(uint32 entry, QuestScript * qs)
{
	Quest * q = QuestStorage.LookupEntry( entry );
	if( q != NULL )
	{
		if(q->pQuestScript != NULL)
		sLog.outError("ScriptMgr is trying to register a script for Quest ID: %u even if there's already one for that Quest. Remove one of those scripts.\n", entry);
		q->pQuestScript = qs;
	}

	_questscripts.insert( qs );
}

void ScriptMgr::register_instance_script( uint32 pMapId, exp_create_instance_ai pCallback ) 
{ 
	if(mInstances.find(pMapId) != mInstances.end())
		sLog.outError("ScriptMgr is trying to register a script for Instance ID: %u even if there's already one for that Instance. Remove one of those scripts.\n", pMapId);
	mInstances.insert( InstanceCreateMap::value_type( pMapId, pCallback ) ); 
}; 

CreatureAIScript* ScriptMgr::CreatePetAIScriptClassForEntry(Creature* pCreature)
{
	CreatureCreateMap::iterator itr = _pets.find(pCreature->GetEntry());
	if(itr == _pets.end())
	{ 
		return NULL;
	}

	exp_create_creature_ai function_ptr = itr->second;
	return (function_ptr)(pCreature);
}

CreatureAIScript* ScriptMgr::CreateAIScriptClassForEntry(Creature* pCreature)
{
	CreatureCreateMap::iterator itr = _creatures.find(pCreature->GetEntry());
	if(itr == _creatures.end())
	{ 
		return NULL;
	}

	exp_create_creature_ai function_ptr = itr->second;
	return (function_ptr)(pCreature);
}

InstanceScript* ScriptMgr::CreateScriptClassForInstance( uint32 pMapId, MapMgr* pMapMgr ) 
{ 
	InstanceCreateMap::iterator Iter = mInstances.find( pMapMgr->GetMapId() ); 
		if ( Iter == mInstances.end() ) 
			return NULL; 
		exp_create_instance_ai function_ptr = Iter->second; 
			return ( function_ptr )( pMapMgr ); 
}; 


GameObjectAIScript * ScriptMgr::CreateAIScriptClassForGameObject(uint32 uEntryId, GameObject* pGameObject)
{
	GameObjectCreateMap::iterator itr = _gameobjects.find(pGameObject->GetEntry());
	if(itr == _gameobjects.end())
	{ 
		return NULL;
	}

	exp_create_gameobject_ai function_ptr = itr->second;
	return (function_ptr)(pGameObject);
}

#if 0
bool ScriptMgr::CallScriptedDummySpell(uint32 uSpellId, uint32 i, Spell* pSpell)
{
/*	HandleDummySpellMap::iterator itr = _spells.find(uSpellId);
	if(itr == _spells.end())
	{ 
		return false;
	}

	exp_handle_dummy_spell function_ptr = itr->second;
	return (function_ptr)(i, pSpell); */
	SpellEntry *sp = dbcSpell.LookupEntryForced( uSpellId );
	if( sp && sp->SpellDummyEffectScript && sp->SpellDummyEffectScript( pSpell, i ) == SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION )
		return true;
	return false;
}

bool ScriptMgr::CallScriptedDummyAura(uint32 uSpellId, uint32 i, Aura* pAura, bool apply)
{
/*	HandleDummyAuraMap::iterator itr = _auras.find(uSpellId);
	if(itr == _auras.end())
	{ 
		return false;
	}

	exp_handle_dummy_aura function_ptr = itr->second;
	return (function_ptr)(i, pAura, apply); */
	SpellEntry *sp = dbcSpell.LookupEntryForced( uSpellId );
	if( sp && sp->AuraPeriodicDummyTickHook && sp->AuraPeriodicDummyTickHook( pAura, apply, i ) == SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION )
		return true;
	return false;
}
#endif

bool ScriptMgr::CallScriptedDummySpell(SpellEntry *sp, uint32 i, Spell* pSpell)
{
	if( sp->SpellDummyEffectScript && sp->SpellDummyEffectScript( i, pSpell ) == true )
		return true;
	return false;
}

bool ScriptMgr::CallScriptedDummyAura(SpellEntry *sp, uint32 i, Aura* pAura, bool apply)
{
	if( sp->AuraPeriodicDummyTickHook && sp->AuraPeriodicDummyTickHook( i, pAura, apply ) == true )
		return true;
	return false;
}

bool ScriptMgr::CallScriptedItem(Item * pItem, Player * pPlayer)
{
	if(pItem->GetProto() && pItem->GetProto()->gossip_script)
	{
		pItem->GetProto()->gossip_script->GossipHello(pItem,pPlayer,true);
		return true;
	}
	
	return false;
}

void ScriptMgr::register_item_gossip_script(uint32 entry, GossipScript * gs)
{
	ItemPrototype * proto = ItemPrototypeStorage.LookupEntry(entry);
	if(proto)
	{
		if(proto->gossip_script != NULL)
		sLog.outError("ScriptMgr is trying to register a gossip for Item ID: %u even if there's already one for that Item. Remove one of those gossips.\n", entry);

		proto->gossip_script = gs;
	}

	_customgossipscripts.insert(gs);
}

void ScriptMgr::CreateEmtryCreatureNameAndProto( uint32 Entry )
{
	CreatureProto *protoNew = CreatureProtoStorage.AllocateEntry( Entry );
	CreatureInfo *infoNew = CreatureNameStorage.AllocateEntry( Entry );
}

/* CreatureAI Stuff */
CreatureAIScript::CreatureAIScript(Creature* creature) : _unit(creature)
{
	ScriptTypeID = 0;
}

void CreatureAIScript::RegisterAIUpdateEvent(uint32 frequency)
{
	//sEventMgr.AddEvent(_unit, &Creature::CallScriptUpdate, EVENT_SCRIPT_UPDATE_EVENT, frequency, 0,0);
	sEventMgr.AddEvent(_unit, &Creature::CallScriptUpdate, EVENT_SCRIPT_UPDATE_EVENT, frequency, 0,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
}

void CreatureAIScript::ModifyAIUpdateEvent(uint32 newfrequency)
{
	sEventMgr.ModifyEventRepeatInterval(_unit, EVENT_SCRIPT_UPDATE_EVENT, newfrequency);
}

void CreatureAIScript::RemoveAIUpdateEvent()
{
	sEventMgr.RemoveEvents(_unit, EVENT_SCRIPT_UPDATE_EVENT);
}

/* GameObjectAI Stuff */

GameObjectAIScript::GameObjectAIScript(GameObject* goinstance) : _gameobject(goinstance)
{

}

void GameObjectAIScript::ModifyAIUpdateEvent(uint32 newfrequency)
{
	sEventMgr.ModifyEventRepeatInterval(_gameobject, EVENT_SCRIPT_UPDATE_EVENT, newfrequency);
}
 
void GameObjectAIScript::RemoveAIUpdateEvent()
{
	sEventMgr.RemoveEvents(_gameobject, EVENT_SCRIPT_UPDATE_EVENT);
}

void GameObjectAIScript::RegisterAIUpdateEvent(uint32 frequency)
{
	sEventMgr.AddEvent(_gameobject, &GameObject::CallScriptUpdate, EVENT_SCRIPT_UPDATE_EVENT, frequency, 0,0);
}

/* QuestScript Stuff */

/* Gossip Stuff*/

GossipScript::GossipScript()
{
	
}

void GossipScript::GossipEnd(Object* pObject, Player* Plr)
{
	Plr->CleanupGossipMenu();
}

bool CanTrainAt(Player * plr, Trainer * trn);
void GossipScript::GossipHello(Object* pObject, Player* Plr, bool AutoSend)
{
	GossipMenu *Menu;
	uint32 TextID = 2;
	Creature * pCreature = (pObject->GetTypeId()==TYPEID_UNIT)?SafeCreatureCast( pObject ):NULL;
	if(!pCreature)
	{ 
		return;
	}

	Trainer *pTrainer = pCreature->GetTrainer();
	uint32 Text = objmgr.GetGossipTextForNpc(pCreature->GetEntry());
	if(Text != 0)
	{
		GossipText * text = NpcTextStorage.LookupEntry(Text);
		if(text != 0)
			TextID = Text;
	}

	objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), TextID, Plr);
	
	uint32 flags = pCreature->GetUInt32Value(UNIT_NPC_FLAGS);

	if(flags & (UNIT_NPC_FLAG_VENDOR|UNIT_NPC_FLAG_VENDOR_AMMO|UNIT_NPC_FLAG_VENDOR_FOOD|UNIT_NPC_FLAG_VENDOR_POISON|UNIT_NPC_FLAG_VENDOR_REAGENT|UNIT_NPC_FLAG_ARMORER|UNIT_NPC_FLAG_REPAIR))
		Menu->AddItem(1, "I would like to browse your goods", 1);
	
	if((flags & (UNIT_NPC_FLAG_TRAINER | UNIT_NPC_FLAG_TRAINER_PROF | UNIT_NPC_FLAG_TRAINER_CLASS)) && pTrainer != 0)
	{
		string name = pCreature->GetCreatureInfo()->Name;
		string::size_type pos = name.find(" ");	  // only take first name
		if(pos != string::npos)
			name = name.substr(0, pos);

		if(CanTrainAt(Plr, pTrainer))
			Menu->SetTextID(pTrainer->Can_Train_Gossip_TextId);
		else
			Menu->SetTextID(pTrainer->Cannot_Train_GossipTextId);
        if(pTrainer->TrainerType != TRAINER_TYPE_PET)
		{
			string msg = "I seek ";
			if(pTrainer->RequiredClass)
			{
				switch(Plr->getClass())
				{
				case MAGE:
					msg += "mage";
					break;
				case SHAMAN:
					msg += "shaman";
					break;
				case WARRIOR:
					msg += "warrior";
					break;
				case PALADIN:
					msg += "paladin";
					break;
				case WARLOCK:
					msg += "warlock";
					break;
				case HUNTER:
					msg += "hunter";
					break;
				case ROGUE:
					msg += "rogue";
					break;
				case DRUID:
					msg += "druid";
					break;
				case PRIEST:
					msg += "priest";
					break;
				}
				msg += " training, ";
				msg += name;
				msg += ".";

				Menu->AddItem(3, msg.c_str(), 2);

			}
			else
			{
				msg += "training, ";
				msg += name;
				msg += ".";

				Menu->AddItem(3, msg.c_str(), 2);
			}
		}
		else
		{
			
			Menu->AddItem(3, "Train me in the ways of the beast.", 2);
		}
	}

	if(flags & UNIT_NPC_FLAG_TAXIVENDOR)
		Menu->AddItem(2, "Give me a ride.", 3);

	if(flags & UNIT_NPC_FLAG_AUCTIONEER)
		Menu->AddItem(0, "I would like to make a bid.", 4);

	if(flags & UNIT_NPC_FLAG_INNKEEPER)
		Menu->AddItem(5, "Make this inn your home.", 5);

	if(flags & UNIT_NPC_FLAG_BANKER)
		Menu->AddItem(0, "I would like to check my deposit box.", 6);

	if(flags & UNIT_NPC_FLAG_SPIRITHEALER)
		Menu->AddItem(0, "Bring me back to life.", 7);

	if(flags & UNIT_NPC_FLAG_ARENACHARTER)
		Menu->AddItem(0, "How do I create a guild/arena team?", 8);

	if(flags & UNIT_NPC_FLAG_TABARDCHANGER)
		Menu->AddItem(0, "I want to create a guild crest.", 9);

	if(flags & UNIT_NPC_FLAG_BATTLEFIELDPERSON)
		Menu->AddItem(0, "I would like to go to the battleground.", 10);

	if( 
		( ( pTrainer &&
		pTrainer->RequiredClass &&					  // class trainer
		pTrainer->RequiredClass == Plr->getClass() &&   // correct class
		pTrainer->TrainerType != TRAINER_TYPE_PET &&  // Pet Trainers do not respec hunter talents
		pCreature->getLevel() > 10				   // creature level
		) || ( pTrainer == NULL && ( flags & (UNIT_NPC_FLAG_TRAINER | UNIT_NPC_FLAG_TRAINER_CLASS ) ) ) )
		&& Plr->getLevel() > 10 )						  // player level
	{
		Menu->AddItem(0, "I would like to reset my talents.", 11);
		//add dual spec learn menu
		if( Plr->getLevel() >= 40 && ( Plr->m_talentSpecsCount == 1 || Plr->HasSpell( 63645 ) == false ) )
		{
			Menu->AddMenuItem(0, "Purchase a Dual Talent Specialization.",0,0,"Are you sure you would like to purchase your second talent specialization?",1000000,0,15);
		}
	}
	
	if( pTrainer &&
		pTrainer->TrainerType == TRAINER_TYPE_PET &&	// pet trainer type
		Plr->getClass() == HUNTER &&					// hunter class
		Plr->GetSummon() != NULL )						// have pet
	{
		Menu->AddItem(0, "I wish to untrain my pet.", 13);
	}

	if(flags & UNIT_NPC_FLAG_REFORGER)
		Menu->AddItem(0, "Reforge an item.", 16);

	if( pCreature->isStableMaster() )
		Menu->AddItem(0, "I'd like to stable my pet here.", 17);

	//not sure this is correct to enable
//	if(flags & (UNIT_NPC_FLAG_FLIGHTMASTER))
//		Menu->AddItem(0, "I would like to travel.", 18);

	if(AutoSend)
		Menu->SendTo(Plr);
}

void GossipScript::GossipSelectOption(Object* pObject, Player* Plr, uint32 Id, uint32 IntId, const char * EnteredCode)
{
	Creature* pCreature = SafeCreatureCast( pObject );
	if( pObject->GetTypeId() != TYPEID_UNIT )
	{ 
		return;
	}

	switch( IntId )
	{
	case 1:
		// vendor
		Plr->GetSession()->SendInventoryList(pCreature);
		break;
	case 2:
		// trainer
		Plr->GetSession()->SendTrainerList(pCreature);
		break;
	case 3:
		// taxi
		Plr->GetSession()->SendTaxiList(pCreature);
		break;
	case 4:
		// auction
		Plr->GetSession()->SendAuctionList(pCreature);
		break;
	case 5:
		// innkeeper
		Plr->GetSession()->SendInnkeeperBind(pCreature);
		break;
	case 6:
		// banker
		Plr->GetSession()->SendBankerList(pCreature);
		break;
	case 7:
		// spirit
		Plr->GetSession()->SendSpiritHealerRequest(pCreature);
		break;
	case 8:
		// petition
		Plr->GetSession()->SendCharterRequest(pCreature);
		break;
	case 9:
		// guild crest
		Plr->GetSession()->SendTabardHelp(pCreature);
		break;
	case 10:
		// battlefield
		Plr->GetSession()->SendBattlegroundList(pCreature, 0);
		break;
	case 11:
		// switch to talent reset message
		{
			GossipMenu *Menu;
			objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 5674, Plr);
			Menu->AddItem(0, "I understand, continue.", 12);
			Menu->SendTo(Plr);
		}break;
	case 12:
		// talent reset
		{
			Plr->Gossip_Complete();
			Plr->SendTalentResetConfirm();
		}break;
	case 13:
		// switch to untrain message
		{
			GossipMenu *Menu;
			objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 7722, Plr);
			Menu->AddItem(0, "Yes, please do.", 14);
			Menu->SendTo(Plr);
		}break;
	case 14:
		// untrain pet
		{
			Plr->Gossip_Complete();
//			Plr->SendPetUntrainConfirm();
			//in 420 client there is no more confirm dialog box
			if(  Plr->GetSummon() )
			{
				Pet *pPet = Plr->GetSummon();
				int32 cost = pPet->GetUntrainCost();
				Plr->ModGold( -cost );
				pPet->WipeTalents();
				pPet->SetTalentPoints( pPet->GetTalentPointsForLevel( pPet->getLevel() ) );
			}
		}break;
	case 15:
		// learn Dual Spec 
		{
			//everything costs
			if( Plr->GetGold( ) < 1000000 )
			{
				//no way to get to this menu unless you had the gold
				return;
			}
			Plr->ModGold( -1000000);
			Plr->m_talentSpecsCount = 2;

			//reset spells cause old spell list is not compatible with multi talent speccing
			//remove this in case you are using emu on a new realm
			//Plr->Reset_Spells();
			//this is required so that spells will get marked to be removed/deleted when switching specs
			Plr->Reset_Talents();

			//trainer teachies us the 2 spells
			SpellEntry *spInfo = dbcSpell.LookupEntry(63680); 
			Spell *sp=SpellPool.PooledNew( __FILE__, __LINE__ );
			sp->Init(Plr,spInfo,true,NULL);
			SpellCastTargets tgt(Plr->GetGUID());	//hmm spell targets self
			sp->prepare(&tgt);
		}break;
	case 16:	//reforge an item, open reforge menu
		{
			Plr->GetSession()->SendReforgeWindowOpen( pCreature->GetGUID() );
		}break;
	case 17:	//open stable master list
		{
			Plr->GetSession()->SendStabledPetList( pCreature->GetGUID() );
		}break;
	case 18:	//taxi list
		{
			Plr->GetSession()->SendTaxiList( pCreature );
		}break;

	default:
		sLog.outError("Unknown IntId %u on entry %u", IntId, pCreature->GetEntry());
		break;
	}	
}

void ScriptMgr::register_hook(ServerHookEvents event, void * function_pointer)
{
	ASSERT(event < NUM_SERVER_HOOKS);
	_hooks[event].push_back(function_pointer);
}

/* Hook Implementations */
#define OUTER_LOOP_BEGIN(type, fptr_type) if(!sScriptMgr._hooks[type].size()) { \
	return; } \
	fptr_type call; \
	for(ServerHookList::iterator itr = sScriptMgr._hooks[type].begin(); itr != sScriptMgr._hooks[type].end(); ++itr) { \
	call = ((fptr_type)*itr);

#define OUTER_LOOP_END }

#define OUTER_LOOP_BEGIN_COND(type, fptr_type) if(!sScriptMgr._hooks[type].size()) { \
	return true; } \
	fptr_type call; \
	bool ret_val = true; \
	for(ServerHookList::iterator itr = sScriptMgr._hooks[type].begin(); itr != sScriptMgr._hooks[type].end(); ++itr) { \
		call = ((fptr_type)*itr);

#define OUTER_LOOP_END_COND } return ret_val;

bool HookInterface::OnNewCharacter(uint32 Race, uint32 Class, WorldSession * Session, const char * Name)
{
	OUTER_LOOP_BEGIN_COND(SERVER_HOOK_EVENT_ON_NEW_CHARACTER, tOnNewCharacter)
		ret_val = (call)(Race, Class, Session, Name);
	OUTER_LOOP_END_COND
}

void HookInterface::OnKillPlayer(Player * pPlayer, Player * pVictim)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_KILL_PLAYER, tOnKillPlayer)
		(call)(pPlayer, pVictim);
	OUTER_LOOP_END
}

void HookInterface::OnFirstEnterWorld(Player * pPlayer)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_FIRST_ENTER_WORLD, tOnFirstEnterWorld)
		(call)(pPlayer);
	OUTER_LOOP_END
}

void HookInterface::OnCharacterCreate(Player * pPlayer)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_CHARACTER_CREATE, tOCharacterCreate)
		(call)(pPlayer);
	OUTER_LOOP_END
}

void HookInterface::OnEnterWorld(Player * pPlayer)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_ENTER_WORLD, tOnEnterWorld)
		(call)(pPlayer);
	OUTER_LOOP_END
}

void HookInterface::OnRemoveFromWorld(Unit *pUnit)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_REMOVE_FROM_WORLD, tOnRemoveFromWorld)
		(call)(pUnit);
	OUTER_LOOP_END
}

void HookInterface::OnDuelEnded(Player * pWinner, Player * pVictim)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_DUEL_ENDED, tOnDuelEnded)
		(call)(pWinner,pVictim);
	OUTER_LOOP_END
}

void HookInterface::OnGuildCreate(Player * pLeader, Guild * pGuild)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_GUILD_CREATE, tOnGuildCreate)
		(call)(pLeader, pGuild);
	OUTER_LOOP_END
}

void HookInterface::OnGuildJoin(Player * pPlayer, Guild * pGuild)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_GUILD_JOIN, tOnGuildJoin)
		(call)(pPlayer, pGuild);
	OUTER_LOOP_END
}

void HookInterface::OnDeath(Player * pPlayer)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_DEATH, tOnDeath)
		(call)(pPlayer);
	OUTER_LOOP_END
}

bool HookInterface::OnRepop(Player * pPlayer)
{
	OUTER_LOOP_BEGIN_COND(SERVER_HOOK_EVENT_ON_REPOP, tOnRepop)
		ret_val = (call)(pPlayer);
	OUTER_LOOP_END_COND
}

void HookInterface::OnEmote(Player * pPlayer, uint32 Emote, Unit * pUnit)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_EMOTE, tOnEmote)
		(call)(pPlayer, Emote, pUnit);
	OUTER_LOOP_END
}

void HookInterface::OnEnterCombat(Player * pPlayer, Unit * pTarget)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_ENTER_COMBAT, tOnEnterCombat)
		(call)(pPlayer, pTarget);
	OUTER_LOOP_END
}

bool HookInterface::OnCastSpell(Player * pPlayer, SpellEntry* pSpell)
{
	OUTER_LOOP_BEGIN_COND(SERVER_HOOK_EVENT_ON_CAST_SPELL, tOnCastSpell)
		ret_val = (call)(pPlayer, pSpell);
	OUTER_LOOP_END_COND
}

bool HookInterface::OnLogoutRequest(Player * pPlayer)
{
	OUTER_LOOP_BEGIN_COND(SERVER_HOOK_EVENT_ON_LOGOUT_REQUEST, tOnLogoutRequest)
		ret_val = (call)(pPlayer);
	OUTER_LOOP_END_COND
}

void HookInterface::OnLogout(Player * pPlayer)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_LOGOUT, tOnLogout)
		(call)(pPlayer);
	OUTER_LOOP_END
}

void HookInterface::OnQuestAccept(Player * pPlayer, Quest * pQuest, Object * pQuestGiver)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_QUEST_ACCEPT, tOnQuestAccept)
		(call)(pPlayer, pQuest, pQuestGiver);
	OUTER_LOOP_END
}

void HookInterface::OnZone(Player * pPlayer, uint32 Zone)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_ZONE, tOnZone)
		(call)(pPlayer, Zone);
	OUTER_LOOP_END
}

bool HookInterface::OnChat(Player * pPlayer, uint32 Type, uint32 Lang, const char * Message, const char * Misc)
{
	OUTER_LOOP_BEGIN_COND(SERVER_HOOK_EVENT_ON_CHAT, tOnChat)
		ret_val = (call)(pPlayer, Type, Lang, Message, Misc);
	OUTER_LOOP_END_COND
}

void HookInterface::OnLoot(Player * pPlayer, Unit * pTarget, uint32 Money, uint32 ItemId)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_LOOT, tOnLoot)
		(call)(pPlayer, pTarget, Money, ItemId);
	OUTER_LOOP_END
}

void HookInterface::OnObjectLoot(Player * pPlayer, Object * pTarget, uint32 Money, uint32 ItemId)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_OBJECTLOOT, tOnObjectLoot)
		(call)(pPlayer, pTarget, Money, ItemId);
	OUTER_LOOP_END
}

void HookInterface::OnEnterWorld2(Player * pPlayer)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_ENTER_WORLD_2, tOnEnterWorld)
		(call)(pPlayer);
	OUTER_LOOP_END
}

void HookInterface::OnQuestCancelled(Player * pPlayer, Quest * pQuest)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_QUEST_CANCELLED, tOnQuestCancel)
		(call)(pPlayer, pQuest);
	OUTER_LOOP_END
}

void HookInterface::OnQuestFinished(Player * pPlayer, Quest * pQuest, Object * pQuestGiver)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_QUEST_FINISHED, tOnQuestFinished)
		(call)(pPlayer, pQuest, pQuestGiver);
	OUTER_LOOP_END
}

void HookInterface::OnHonorableKill(Player * pPlayer, Player * pKilled)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_HONORABLE_KILL, tOnHonorableKill)
		(call)(pPlayer, pKilled);
	OUTER_LOOP_END
}

void HookInterface::OnArenaFinish(Player * pPlayer, ArenaTeam* pTeam, bool victory, bool rated)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_ARENA_FINISH, tOnArenaFinish)
		(call)(pPlayer, pTeam, victory, rated);
	OUTER_LOOP_END
}

void HookInterface::OnAreaTrigger(Player * pPlayer, uint32 areaTrigger)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_AREATRIGGER, tOnAreaTrigger)
		(call)(pPlayer, areaTrigger);
	OUTER_LOOP_END
}

void HookInterface::OnPostLevelUp(Player * pPlayer)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_POST_LEVELUP, tOnPostLevelUp)
		(call)(pPlayer);
	OUTER_LOOP_END
}

void HookInterface::OnPreUnitDie(Unit *Killer, Unit *Victim)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_PRE_DIE, tOnPreUnitDie)
		(call)(Killer, Victim);
	OUTER_LOOP_END
}

void HookInterface::OnAdvanceSkillLine(Player * pPlayer, uint32 SkillLine, uint32 Current)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_ADVANCE_SKILLLINE, tOnAdvanceSkillLine)
		(call)(pPlayer, SkillLine, Current);
	OUTER_LOOP_END
}

void HookInterface::OnAddRemovePlayerItem(Player * pPlayer, Item *item, bool Add)
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_ADD_REM_ITEM, tOnAddRemItem)
		(call)(pPlayer, item, Add);
	OUTER_LOOP_END
}

void HookInterface::OnBGEnded( CBattleground *bat )
{
	OUTER_LOOP_BEGIN(SERVER_HOOK_EVENT_ON_BG_ENDED, tOnBGEnded)
		(call)( bat );
	OUTER_LOOP_END
}

/* InstanceAI Stuff */ 

InstanceScript::InstanceScript( MapMgr* pMapMgr ) : mInstance( pMapMgr ) 
{
};

void InstanceScript::RegisterUpdateEvent( uint32 pFrequency ) 
{
	sEventMgr.AddEvent( mInstance, &MapMgr::CallScriptUpdate, EVENT_SCRIPT_UPDATE_EVENT, pFrequency, 0, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT ); 
};
void InstanceScript::ModifyUpdateEvent( uint32 pNewFrequency ) 
{
	sEventMgr.ModifyEventRepeatInterval( mInstance, EVENT_SCRIPT_UPDATE_EVENT, pNewFrequency );
};
void InstanceScript::RemoveUpdateEvent()
{
	sEventMgr.RemoveEvents( mInstance, EVENT_SCRIPT_UPDATE_EVENT ); 
};
