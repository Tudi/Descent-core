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

/** Table formats converted to strings
 */
const char * gItemNameFormat							= "usu";
//const char * gItemPrototypeFormat						= "uiiissssuiiuiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiuuffiffiiiiiiiiiifiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiisiiiiiiiiiiiiiiiiiiiiiiiiiiuuuu";
const char * gItemPrototypeFormat						= "uuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuusssssuuuuuuuuuuuuuuuuuuuuuuufuuufuuuuu";
const char * gCreatureNameFormat						= "usssuuuuuuuuuuuffcuuuuuuuu";
const char * gGameObjectNameFormat						= "uuusssuuuuuuuuuuuuuuuuuuuuuuuufuuuuuuu";
const char * gCreatureProtoFormat						= "uuuuuuufuuuffuffuuuuuuuuffsuuuufffufuuuuuuu";
const char * gAreaTriggerFormat							= "ucuusffffuu";
const char * gItemPageFormat							= "usu";
const char * gNpcTextFormat								= "ufssuuuuuuufssuuuuuuufssuuuuuuufssuuuuuuufssuuuuuuufssuuuuuuufssuuuuuuufssuuuuuuu";
//const char * gQuestFormat								= "iiiiiiiiiiiiiiiiiifiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiffisssssiiiiiiiiiiiiiiiiiiiiiiiiiiiissssuuuuuuuuuuuuuuuuuuuuuuiiiiuuuuussi";
const char * gQuestFormat								= "iiiiiiiiiiiiiiiiiifiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiffisssssiiiiiiiiiiiiiiiiiiiiiiiiiiiiissssuuuuuuuuuuuuuuuussssuuuuuuuuuuuuuuuuuuuuuuuuiiiiuuuuussi";
const char * gSpellExtraFormat							= "uuuu";
const char * gGraveyardFormat							= "uffffuuuux";
const char * gTeleportCoordFormat						= "uxufffx";
const char * gPvPAreaFormat								= "ush";
const char * gFishingFormat								= "uuu";
const char * gWorldMapInfoFormat						= "uuuuufffusuuuuuuufuuu";
const char * gZoneGuardsFormat							= "uuu";
const char * gUnitModelSizeFormat						= "uf";
// cebernic extended
const char * gWorldStringTableFormat					= "us"; // p2wow added [for worldserver common message storage]
const char * gWorldBroadCastFormat						= "usu";// announce message

const char * gvehicledataFormat							= "uuuuuuuu";
const char * gseatdataFormat							= "uuuufffuufffuufffuufffuufffuufffuufffuufff";

/** SQLStorage symbols
 */
SERVER_DECL SQLStorage<ItemName, ArrayStorageContainer<ItemName> >							ItemNameStorage;
SERVER_DECL SQLStorage<ItemPrototype, ArrayStorageContainer<ItemPrototype> >				ItemPrototypeStorage;
SERVER_DECL SQLStorage<CreatureInfo, HashMapStorageContainer<CreatureInfo> >				CreatureNameStorage;
SERVER_DECL SQLStorage<GameObjectInfo, HashMapStorageContainer<GameObjectInfo> >			GameObjectNameStorage;
SERVER_DECL SQLStorage<CreatureProto, HashMapStorageContainer<CreatureProto> >				CreatureProtoStorage;
SERVER_DECL SQLStorage<AreaTrigger, HashMapStorageContainer<AreaTrigger> >					AreaTriggerStorage;
SERVER_DECL SQLStorage<ItemPage, HashMapStorageContainer<ItemPage> >						ItemPageStorage;
SERVER_DECL SQLStorage<Quest, HashMapStorageContainer<Quest> >								QuestStorage;
SERVER_DECL SQLStorage<GossipText, HashMapStorageContainer<GossipText> >					NpcTextStorage;
SERVER_DECL SQLStorage<GraveyardTeleport, HashMapStorageContainer<GraveyardTeleport> >		GraveyardStorage;
SERVER_DECL SQLStorage<TeleportCoords, HashMapStorageContainer<TeleportCoords> >			TeleportCoordStorage;
SERVER_DECL SQLStorage<FishingZoneEntry, HashMapStorageContainer<FishingZoneEntry> >		FishingZoneStorage;
SERVER_DECL SQLStorage<MapInfo, ArrayStorageContainer<MapInfo> >							WorldMapInfoStorage;
SERVER_DECL SQLStorage<ZoneGuardEntry, HashMapStorageContainer<ZoneGuardEntry> >			ZoneGuardStorage;
SERVER_DECL SQLStorage<UnitModelSizeEntry, HashMapStorageContainer<UnitModelSizeEntry> >	UnitModelSizeStorage;
SERVER_DECL SQLStorage<WorldStringTable, HashMapStorageContainer<WorldStringTable> >		WorldStringTableStorage;
SERVER_DECL SQLStorage<WorldBroadCast, HashMapStorageContainer<WorldBroadCast> >			WorldBroadCastStorage;
SERVER_DECL SQLStorage<VehicleData, HashMapStorageContainer<VehicleData> >					VehicleDataStorage;
SERVER_DECL SQLStorage<SeatData, HashMapStorageContainer<SeatData> >						SeatDataStorage;

SERVER_DECL set<string> ExtraMapCreatureTables;
SERVER_DECL set<string> ExtraMapGameObjectTables;

void ObjectMgr::LoadProfessionDiscoveries()
{
	QueryResult * result = WorldDatabase.Query( "SELECT * from professiondiscoveries" );
	if ( result != NULL )
	{
		do
		{
			Field *f = result->Fetch();
			ProfessionDiscovery * pf = new ProfessionDiscovery;
			pf->SpellId = f[0].GetUInt32();
			if( dbcSpell.LookupEntryForced( pf->SpellId ) == NULL ) 
			{
				sLog.outDebug("Profession discovery spell is using invalid spell %d. Skipping load ",pf->SpellId);
				delete pf;
				continue;
			}
			pf->SpellToDiscover = f[1].GetUInt32();
			pf->SkillValue = f[2].GetUInt32();
			pf->Chance = f[3].GetFloat();
			ProfessionDiscoveryTable.insert( pf );
		}
		while( result->NextRow() );
	}
	delete result;
	result = NULL;
}

void ObjectMgr::LoadExtraCreatureProtoStuff()
{
	{
		StorageContainerIterator<CreatureProto> * itr = CreatureProtoStorage.MakeIterator();
		CreatureProto * cn;
		while(!itr->AtEnd())
		{
			cn = itr->Get();
			if(itr->Get()->aura_string)
			{
				string auras = string(itr->Get()->aura_string);
				vector<string> aurs = StrSplit(auras, " ");
				for(vector<string>::iterator it = aurs.begin(); it != aurs.end(); ++it)
				{
					uint32 id = atol((*it).c_str());
					if(id)
						itr->Get()->start_auras.insert( id );
				}
			}

			if(!itr->Get()->MinHealth)
				itr->Get()->MinHealth = 1;
			if(!itr->Get()->MaxHealth)
				itr->Get()->MaxHealth = 1;
			if (itr->Get()->AttackType > SCHOOL_ARCANE)
				itr->Get()->AttackType = SCHOOL_NORMAL;

			cn->m_canFlee = cn->m_canRangedAttack = cn->m_canCallForHelp = false;
			cn->m_fleeHealth = 0.0f;
			// please.... m_fleeDuration is a uint32...
			//cn->m_fleeDuration = 0.0f;
			cn->m_fleeDuration = 0;

			//attach a loot list if there is one
			if( cn->loot_kill_id == 0 )
				cn->loot_kill_id = cn->Id;
			if( cn->loot_skin_id == 0 )
				cn->loot_skin_id = cn->Id;
			if( cn->loot_pickpocket_id == 0 )
				cn->loot_pickpocket_id = cn->Id;
			cn->loot_kill = m_creature_loot_kill[ cn->loot_kill_id ];
			cn->loot_skin = m_creature_loot_skin[ cn->loot_skin_id ];
			cn->loot_pickpocket = m_creature_loot_pickpocket[ cn->loot_pickpocket_id ];

			if(!itr->Inc())
				break;
		}

		itr->Destruct();
	}

	{
		StorageContainerIterator<CreatureInfo> * itr = CreatureNameStorage.MakeIterator();
		CreatureInfo * ci;
		while(!itr->AtEnd())
		{
			ci = itr->Get();

			ci->lowercase_name = string(ci->Name);
			for(uint32 j = 0; j < ci->lowercase_name.length(); ++j)
				ci->lowercase_name[j] = tolower(ci->lowercase_name[j]); // Darvaleo 2008/08/15 - Copied lowercase conversion logic from ItemPrototype task

			ci->gossip_script = sScriptMgr.GetDefaultGossipScript();

			if(!itr->Inc())
				break;
		}
		itr->Destruct();
	}

	{
		StorageContainerIterator<Quest> * itr = QuestStorage.MakeIterator();
		Quest * qst;
		while(!itr->AtEnd())
		{
			qst = itr->Get();
			qst->pQuestScript = NULL;

			//if src item is required item then do not give it to player
			if( qst->srcitem )
			{
				bool skipadd = false;
				for(uint32 i=0;i<6;i++)
					if( qst->required_item[i] == qst->srcitem )
						skipadd = true;
				for(uint32 i=0;i<4;i++)
					if( qst->receive_items[i] == qst->srcitem )
						skipadd = true;
				if( skipadd == true )
					qst->srcitem = 0;
			}

			if( !itr->Inc() )
				break;
		}
		itr->Destruct();
	}

	// Load AI Agents
	if(Config.MainConfig.GetBoolDefault("Server", "LoadAIAgents", true))
	{
		QueryResult * result = WorldDatabase.Query( "SELECT * FROM ai_agents" );
		CreatureProto * cn;

		if( result != NULL )
		{
			AI_Spell *sp;
			SpellEntry * spe;
			uint32 entry;


			do
			{
				Field *fields = result->Fetch();
				entry = fields[0].GetUInt32();
				cn = CreatureProtoStorage.LookupEntry(entry);
				if( !cn )
					continue;

				uint32 spell_ID = fields[5].GetUInt32();
				spe = dbcSpell.LookupEntryForced( spell_ID );
				if( spe == NULL )
				{
					Log.Warning("AIAgent", "For %u has nonexistant spell %u.", spell_ID, fields[5].GetUInt32());
					if( spell_ID != 1 )	//might be used to make combat events
					{
						QueryResult *result = WorldDatabase.Query( "Delete from ai_agents where spell=%d", spell_ID );
						if( result )
							delete result;
					}
					continue;
				}
				if(!cn)
					continue;

				sp = new AI_Spell;
				memset( sp, 0, sizeof( AI_Spell ) );
				sp->entryId = fields[0].GetUInt32();
				sp->agent = fields[1].GetUInt16();
				sp->minrange = (float)fields[2].GetUInt32();
				sp->procChance = fields[3].GetUInt32();
				sp->procCount = fields[4].GetUInt32();
				if( sp->procCount == 0 )
					sp->procCount = MAX_INT;
				sp->spell = spe;
				sp->maxrange = (float)fields[6].GetUInt32();

				int32  targettype = fields[7].GetInt32();
				if( targettype == -1 )
					sp->spelltargetType = GetAiTargetType( spe );
				else sp->spelltargetType = targettype;

				sp->cooldown = fields[8].GetInt32();
				sp->min_hp_pct = fields[9].GetInt32();
				sp->autocast_type=AUTOCAST_EVENT_NONE;
				sp->max_hp_pct = fields[10].GetUInt32();
				sp->initial_cooldown = fields[11].GetUInt32();
				sp->global_cooldown = fields[12].GetUInt32(); //have this value in milliseconds
				sp->difficulty_mask = fields[14].GetUInt32(); //prev field is shout
				if( sp->difficulty_mask == 0 )
					sp->difficulty_mask = 0xFFFF;	//morron protection

				if(sp->agent == AGENT_SPELL)
				{
					if(!sp->spell)
					{
						sLog.outDebug("SpellId %u in ai_agent for %u is invalid.\n", (unsigned int)fields[5].GetUInt32(), (unsigned int)sp->entryId);
						delete sp;
						sp = NULL;
						continue;
					}
					
					if(sp->spell->eff[0].Effect == SPELL_EFFECT_LEARN_SPELL || sp->spell->eff[1].Effect == SPELL_EFFECT_LEARN_SPELL ||
						sp->spell->eff[2].Effect == SPELL_EFFECT_LEARN_SPELL)
					{
						sLog.outDebug("Teaching spell %u in ai_agent for %u\n", (unsigned int)fields[5].GetUInt32(), (unsigned int)sp->entryId);
						delete sp;
						sp = NULL;
						continue;
					}

					float t;
					t = GetMinRange(dbcSpellRange.LookupEntry(sp->spell->rangeIndex));
					if( t > sp->maxrange )
						sp->maxrange = t;
					t = GetMaxRange(dbcSpellRange.LookupEntry(sp->spell->rangeIndex));
					if( t < sp->minrange )
						sp->minrange = t;

					//omg the poor darling has no clue about making ai_agents
					if(sp->cooldown == (uint32)-1)
					{
						//now this will not be exact cooldown but maybe a bigger one to not make him spam spells to often
						int cooldown;
						SpellDuration *sd=dbcSpellDuration.LookupEntry(sp->spell->DurationIndex);
						int Dur=0;
						int Casttime=0;//most of the time 0
						int RecoveryTime=sp->spell->RecoveryTime;
						if(sp->spell->DurationIndex)
							Dur =::GetDuration(sd);
						Casttime=GetCastTime(dbcSpellCastTime.LookupEntry(sp->spell->CastingTimeIndex));
						cooldown=Dur+Casttime+RecoveryTime;
						if(cooldown<0)
							sp->cooldown=2000;//huge value that should not loop while adding some timestamp to it
						else sp->cooldown=cooldown;
					}
				}

				if(sp->agent == AGENT_RANGED)
				{
					cn->m_canRangedAttack = true;
					delete sp;
					sp = NULL;
				}
				else if(sp->agent == AGENT_FLEE)
				{
					cn->m_canFlee = true;
					cn->m_fleeHealth = 0.2f;
					cn->m_fleeDuration = 10000;

					delete sp;
					sp = NULL;
				}
				else if(sp->agent == AGENT_CALLFORHELP)
				{
					cn->m_canCallForHelp = true;
					cn->m_callForHelpHealth = 0.2f;
					delete sp;
					sp = NULL;
				}
				else
				{
					sp->shout_before_cast = strdup_local( fields[13].GetString() );
					cn->spells.push_back(sp);
				}
			} while( result->NextRow() );

			delete result;
			result = NULL;
		}
	}
}

void ObjectMgr::LoadExtraItemStuff()
{
	map<uint32,uint32> foodItems;
	QueryResult * result = WorldDatabase.Query("SELECT * FROM itempetfood ORDER BY entry");
	if(result)
	{
		Field *f = result->Fetch();
		do
		{		
			foodItems.insert( make_pair( f[0].GetUInt32(), f[1].GetUInt32() ) );
		}
		while(result->NextRow());
		delete result;
		result = NULL;
	}

	std::map<uint32,uint32> item_name_customcategory;
	StorageContainerIterator<ItemPrototype> * itr = ItemPrototypeStorage.MakeIterator();
	ItemPrototype * pItemPrototype;
	while(!itr->AtEnd())
	{
		pItemPrototype = itr->Get();

		//we need to group some items to not stack : heroic - non heroic versions
		pItemPrototype->NameHash = crc32((const unsigned char*)pItemPrototype->Name1, (unsigned int)strlen(pItemPrototype->Name1));
		pItemPrototype->CustomItemSet = pItemPrototype->ItemSet;
		// These season pvp itemsets are interchangeable and each set group has the same
		// bonuses if you have a full set made up of parts from any of the 3 similar sets
		// you will get the highest sets bonus
		// TODO: make a config for server so they can configure which season is active season
		// * Gladiator's Battlegear
		if( pItemPrototype->CustomItemSet == 701 || pItemPrototype->CustomItemSet == 736 || pItemPrototype->CustomItemSet == 567 )
			pItemPrototype->CustomItemSet = 736;
		// * Gladiator's Dreadgear
		else if( pItemPrototype->CustomItemSet == 702 || pItemPrototype->CustomItemSet == 734 || pItemPrototype->CustomItemSet == 568 )
			pItemPrototype->CustomItemSet = 734;
		// * Gladiator's Earthshaker
		else if( pItemPrototype->CustomItemSet == 703 || pItemPrototype->CustomItemSet == 732 || pItemPrototype->CustomItemSet == 578 )
			pItemPrototype->CustomItemSet= 732;
		// * Gladiator's Felshroud
		else if( pItemPrototype->CustomItemSet == 704 || pItemPrototype->CustomItemSet == 735 || pItemPrototype->CustomItemSet == 615 )
			pItemPrototype->CustomItemSet = 735;
		// * Gladiator's Investiture
		else if( pItemPrototype->CustomItemSet == 705 || pItemPrototype->CustomItemSet == 728 || pItemPrototype->CustomItemSet == 687 )
			pItemPrototype->CustomItemSet = 728;
		// * Gladiator's Pursuit
		else if( pItemPrototype->CustomItemSet == 706 || pItemPrototype->CustomItemSet == 723 || pItemPrototype->CustomItemSet == 586 )
			pItemPrototype->CustomItemSet = 723;
		// * Gladiator's Raiment
		else if( pItemPrototype->CustomItemSet == 707 || pItemPrototype->CustomItemSet == 729 || pItemPrototype->CustomItemSet == 581 )
			pItemPrototype->CustomItemSet = 729;
		// * Gladiator's Redemption
		else if( pItemPrototype->CustomItemSet == 708 || pItemPrototype->CustomItemSet == 725 || pItemPrototype->CustomItemSet == 690 )
			pItemPrototype->CustomItemSet = 725;
		// * Gladiator's Refuge
		else if( pItemPrototype->CustomItemSet == 709 || pItemPrototype->CustomItemSet == 720 || pItemPrototype->CustomItemSet == 685 )
			pItemPrototype->CustomItemSet = 720;
		// * Gladiator's Regalia
		else if( pItemPrototype->CustomItemSet == 710 || pItemPrototype->CustomItemSet == 724 || pItemPrototype->CustomItemSet == 579 )
			pItemPrototype->CustomItemSet = 724;
		// * Gladiator's Sanctuary
		else if( pItemPrototype->CustomItemSet == 711 || pItemPrototype->CustomItemSet == 721 || pItemPrototype->CustomItemSet == 584 )
			pItemPrototype->CustomItemSet = 721;
		// * Gladiator's Thunderfist
		else if( pItemPrototype->CustomItemSet == 712 || pItemPrototype->CustomItemSet == 733 || pItemPrototype->CustomItemSet == 580 )
			pItemPrototype->CustomItemSet = 733;
		// * Gladiator's Vestments
		else if( pItemPrototype->CustomItemSet == 713 || pItemPrototype->CustomItemSet == 730 || pItemPrototype->CustomItemSet == 577 )
			pItemPrototype->CustomItemSet = 730;
		// * Gladiator's Vindication
		else if( pItemPrototype->CustomItemSet == 714 || pItemPrototype->CustomItemSet == 726 || pItemPrototype->CustomItemSet == 583 )
			pItemPrototype->CustomItemSet = 726;
		// * Gladiator's Wartide
		else if( pItemPrototype->CustomItemSet == 715 || pItemPrototype->CustomItemSet == 731 || pItemPrototype->CustomItemSet == 686 )
			pItemPrototype->CustomItemSet = 731;
		// * Gladiator's Wildhide
		else if( pItemPrototype->CustomItemSet == 716 || pItemPrototype->CustomItemSet == 722 || pItemPrototype->CustomItemSet == 585 )
			pItemPrototype->CustomItemSet = 722;


		if( item_name_customcategory[ pItemPrototype->NameHash ] != 0 )
		{
			ItemPrototype *otheritem = ItemPrototypeStorage.LookupEntry( item_name_customcategory[ pItemPrototype->NameHash ] ); 
			if( otheritem->InventoryType == pItemPrototype->InventoryType 
				&& otheritem->Class == pItemPrototype->Class
				&& otheritem->SubClass == pItemPrototype->SubClass	
				&& ( otheritem->InventoryType == INVTYPE_FINGER || otheritem->InventoryType == INVTYPE_TRINKET )
				)	//same slot type. required for paired items with same name
				pItemPrototype->CustomCategoryId = item_name_customcategory[ pItemPrototype->NameHash ];
			else
			{
				pItemPrototype->CustomCategoryId = pItemPrototype->ItemId;
			}
		}
		else
		{
			item_name_customcategory[ pItemPrototype->NameHash ] = pItemPrototype->ItemId;
			pItemPrototype->CustomCategoryId = pItemPrototype->ItemId;
		}

		if( pItemPrototype->ContainerSlots > (CONTAINER_FIELD_SLOT_1_71-CONTAINER_FIELD_SLOT_1)/2)
			pItemPrototype->ContainerSlots = (CONTAINER_FIELD_SLOT_1_71-CONTAINER_FIELD_SLOT_1)/2;

		if(pItemPrototype->ItemSet > 0)
		{
			ItemSetContentMap::iterator itr = mItemSets.find(pItemPrototype->ItemSet);
			std::list<ItemPrototype*>* l;
			if(itr == mItemSets.end())
			{
				l = new std::list<ItemPrototype*>;				
				mItemSets.insert( ItemSetContentMap::value_type( pItemPrototype->ItemSet, l) );
			} 
			else 
				l = itr->second;
			l->push_back(pItemPrototype);
		}


		// lowercase name, used for searches
		pItemPrototype->lowercase_name = pItemPrototype->Name1;
		for(uint32 j = 0; j < pItemPrototype->lowercase_name.length(); ++j)
			pItemPrototype->lowercase_name[j] = tolower(pItemPrototype->lowercase_name[j]);

		//load item_pet_food_type from extra table
		uint32 ft = 0;
		map<uint32,uint32>::iterator iter = foodItems.find(pItemPrototype->ItemId);
		if(iter != foodItems.end())
			ft = iter->second;
		pItemPrototype->FoodType = ft ;
	
		pItemPrototype->gossip_script=NULL;

		//force item spells to not proc too often 
		for( int k = 0; k < 5; k++ )
			if( pItemPrototype->Spells.Id[k] != 0 )
			{
				SpellEntry *sp = dbcSpell.LookupEntryForced( pItemPrototype->Spells.Id[k] );
				if( sp )
				{
					//adjust spell power benefit
					SetItemSpellSPCoeff( sp );
					SetItemSpellProcInterval( sp, pItemPrototype->InventoryType == INVTYPE_TRINKET );
					//check to see if this is a velumn enchanting result(scroll)
					if( ( pItemPrototype->Flags & ITEM_FLAGS_ENCHANT_SCROLL )
						&& sp->spell_skilline[0] == SKILL_ENCHANTING 
						&& pItemPrototype->SellPrice != 0 //so we can trade these babies ? Else there will be duplicats and overwrites
						&& pItemPrototype->ItemId != 72070 //this is not a velumn, later handle these cases properly. Right now i'm drunk :(
						//these items got removed by blizz, we still have them. Need to detect which ones are outdated
//						&& pItemPrototype->ItemId != 35400 //2.4 client version of 37603 item
//						&& pItemPrototype->ItemId != 35420 //2.4 client version of 38897 item
//						&& pItemPrototype->ItemId != 35426 //2.4 client version of 38898 item
						)
					{
//						ASSERT( VelumEnchantToScrollEntry[ sp->Id ] <= 0 );
						VelumEnchantToScrollEntry[ sp->Id ] = pItemPrototype->ItemId;
					}

				}
			}
		pItemPrototype->NormalizationCoeff = pItemPrototype->Delay / ( 1000.0f * 14.0f );
/*		switch( pItemPrototype->SubClass )
		{
			case ITEM_SUBCLASS_WEAPON_DAGGER:
				{
//					pItemPrototype->NormalizationCoeff = 1.79f;
					pItemPrototype->NormalizationCoeff = pItemPrototype->Delay / 1000.0f / 14.0f;
				}break;
			case ITEM_SUBCLASS_WEAPON_AXE:
			case ITEM_SUBCLASS_WEAPON_MACE:
			case ITEM_SUBCLASS_WEAPON_SWORD:
			case ITEM_SUBCLASS_WEAPON_FIST_WEAPON:
				{
//					pItemPrototype->NormalizationCoeff = 2.43f;
					pItemPrototype->NormalizationCoeff = pItemPrototype->Delay / 1000.0f / 14.0f;
				}break;
			case ITEM_SUBCLASS_WEAPON_TWOHAND_AXE:
			case ITEM_SUBCLASS_WEAPON_TWOHAND_MACE:
			case ITEM_SUBCLASS_WEAPON_TWOHAND_SWORD:
			case ITEM_SUBCLASS_WEAPON_POLEARM:
			case ITEM_SUBCLASS_WEAPON_STAFF:
			case ITEM_SUBCLASS_WEAPON_FISHING_POLE:
				{
//					pItemPrototype->NormalizationCoeff = 3.3f;
					pItemPrototype->NormalizationCoeff = pItemPrototype->Delay / 1000.0f / 14.0f;
				}break;
			default:
				break;
		};*/
        if(!itr->Inc())
			break;
	}

	itr->Destruct();
	foodItems.clear();
}

#define make_task(storage, itype, storagetype, tablename, format) tl.AddTask( new Task( \
	new CallbackP2< SQLStorage< itype, storagetype< itype > >, const char *, const char *> \
    (&storage, &SQLStorage< itype, storagetype< itype > >::Load, tablename, format) ) )

void Storage_FillTaskList(TaskList & tl)
{
	make_task(ItemNameStorage, ItemName, ArrayStorageContainer, "itemnames", gItemNameFormat);
	make_task(ItemPrototypeStorage, ItemPrototype, ArrayStorageContainer, "items", gItemPrototypeFormat);
	make_task(CreatureNameStorage, CreatureInfo, HashMapStorageContainer, "creature_names", gCreatureNameFormat);
	make_task(GameObjectNameStorage, GameObjectInfo, HashMapStorageContainer, "gameobject_names", gGameObjectNameFormat);
	make_task(CreatureProtoStorage, CreatureProto, HashMapStorageContainer, "creature_proto", gCreatureProtoFormat);
	make_task(AreaTriggerStorage, AreaTrigger, HashMapStorageContainer, "areatriggers", gAreaTriggerFormat);
	make_task(ItemPageStorage, ItemPage, HashMapStorageContainer, "itempages", gItemPageFormat);
	make_task(QuestStorage, Quest, HashMapStorageContainer, "quests", gQuestFormat);
	make_task(GraveyardStorage, GraveyardTeleport, HashMapStorageContainer, "graveyards", gGraveyardFormat);
	make_task(TeleportCoordStorage, TeleportCoords, HashMapStorageContainer, "teleport_coords", gTeleportCoordFormat);
	make_task(FishingZoneStorage, FishingZoneEntry, HashMapStorageContainer, "fishing", gFishingFormat);
	make_task(NpcTextStorage, GossipText, HashMapStorageContainer, "npc_text", gNpcTextFormat);
	make_task(WorldMapInfoStorage, MapInfo, ArrayStorageContainer, "worldmap_info", gWorldMapInfoFormat);
	make_task(ZoneGuardStorage, ZoneGuardEntry, HashMapStorageContainer, "zoneguards", gZoneGuardsFormat);
	make_task(UnitModelSizeStorage, UnitModelSizeEntry, HashMapStorageContainer, "unit_display_sizes", gUnitModelSizeFormat);
	make_task(WorldStringTableStorage, WorldStringTable, HashMapStorageContainer, "worldstring_tables", gWorldStringTableFormat);
	make_task(WorldBroadCastStorage, WorldBroadCast, HashMapStorageContainer, "worldbroadcast", gWorldBroadCastFormat);
	make_task(VehicleDataStorage, VehicleData, HashMapStorageContainer, "vehicle_data", gvehicledataFormat);
	make_task(SeatDataStorage, SeatData, HashMapStorageContainer, "vehicle_seat_data", gseatdataFormat);	
}

void Storage_Cleanup()
{
	{
		StorageContainerIterator<CreatureProto> * itr = CreatureProtoStorage.MakeIterator();
		CreatureProto * p;
		while(!itr->AtEnd())
		{
			p = itr->Get();
			for(list<AI_Spell*>::iterator it = p->spells.begin(); it != p->spells.end(); ++it)
			{
				if( (*it)->shout_before_cast )
					free( (*it)->shout_before_cast );
				delete (*it);
				(*it) = NULL;
			}
			p->spells.clear();
			p->start_auras.clear();
			if(!itr->Inc())
				break;
		}
		itr->Destruct();
	}
	ItemNameStorage.Cleanup();
	ItemPrototypeStorage.Cleanup();
	CreatureNameStorage.Cleanup();
	GameObjectNameStorage.Cleanup();
	CreatureProtoStorage.Cleanup();
	AreaTriggerStorage.Cleanup();
	ItemPageStorage.Cleanup();
	QuestStorage.Cleanup();
	GraveyardStorage.Cleanup();
	TeleportCoordStorage.Cleanup();
	FishingZoneStorage.Cleanup();
	NpcTextStorage.Cleanup();
	WorldMapInfoStorage.Cleanup();
	ZoneGuardStorage.Cleanup();
	UnitModelSizeStorage.Cleanup();
	WorldStringTableStorage.Cleanup();
	WorldBroadCastStorage.Cleanup();
	VehicleDataStorage.Cleanup();
	SeatDataStorage.Cleanup();
}

vector<pair<string,string> > additionalTables;

bool LoadAdditionalTable(const char * TableName, const char * SecondName)
{
	if(!stricmp(TableName, "creature_spawns"))
	{
		ExtraMapCreatureTables.insert(string(SecondName));
		return false;
	}
	else if(!stricmp(TableName, "gameobject_spawns"))
	{
		ExtraMapGameObjectTables.insert(string(SecondName));
		return false;
	}
	else if(!stricmp(TableName, "items"))					// Items
		ItemPrototypeStorage.LoadAdditionalData(SecondName, gItemPrototypeFormat);
	else if(!stricmp(TableName, "creature_proto"))		// Creature Proto
		CreatureProtoStorage.LoadAdditionalData(SecondName, gCreatureProtoFormat);
	else if(!stricmp(TableName, "creature_names"))		// Creature Names
		CreatureNameStorage.LoadAdditionalData(SecondName, gCreatureNameFormat);
	else if(!stricmp(TableName, "gameobject_names"))	// GO Names
		GameObjectNameStorage.LoadAdditionalData(SecondName, gGameObjectNameFormat);
	else if(!stricmp(TableName, "areatriggers"))		// Areatriggers
		AreaTriggerStorage.LoadAdditionalData(SecondName, gAreaTriggerFormat);
	else if(!stricmp(TableName, "itempages"))			// Item Pages
		ItemPrototypeStorage.LoadAdditionalData(SecondName, gItemPageFormat);
	else if(!stricmp(TableName, "worldstring_tables"))			// WorldString
		WorldStringTableStorage.LoadAdditionalData(SecondName, gWorldStringTableFormat);
	else if(!stricmp(TableName, "worldbroadcast"))			// Worldbroadcast
		WorldBroadCastStorage.LoadAdditionalData(SecondName, gWorldBroadCastFormat);
	else if(!stricmp(TableName, "quests"))				// Quests
		QuestStorage.LoadAdditionalData(SecondName, gQuestFormat);
	else if(!stricmp(TableName, "npc_text"))			// NPC Text Storage
		NpcTextStorage.LoadAdditionalData(SecondName, gNpcTextFormat);
	else if(!stricmp(TableName, "fishing"))				// Fishing Zones
		FishingZoneStorage.LoadAdditionalData(SecondName, gFishingFormat);
	else if(!stricmp(TableName, "teleport_coords"))		// Teleport coords
		TeleportCoordStorage.LoadAdditionalData(SecondName, gTeleportCoordFormat);
	else if(!stricmp(TableName, "graveyards"))			// Graveyards
		GraveyardStorage.LoadAdditionalData(SecondName, gGraveyardFormat);
	else if(!stricmp(TableName, "worldmap_info"))		// WorldMapInfo
		WorldMapInfoStorage.LoadAdditionalData(SecondName, gWorldMapInfoFormat);
	else if(!stricmp(TableName, "zoneguards"))
		ZoneGuardStorage.LoadAdditionalData(SecondName, gZoneGuardsFormat);
	else if(!stricmp(TableName, "unit_display_sizes"))
		UnitModelSizeStorage.LoadAdditionalData(SecondName, gUnitModelSizeFormat);
	else if(!stricmp(TableName, "vehicle_data"))
		VehicleDataStorage.Reload();	
	else if(!stricmp(TableName, "vehicle_seat_data"))
		VehicleDataStorage.Reload();	
	else
		return false;

	return true;
}

bool Storage_ReloadTable(const char * TableName)
{
	// bur: mah god this is ugly :P
	if(!stricmp(TableName, "items"))					// Items
		ItemPrototypeStorage.Reload();
	else if(!stricmp(TableName, "creature_proto"))		// Creature Proto
		CreatureProtoStorage.Reload();
	else if(!stricmp(TableName, "creature_names"))		// Creature Names
		CreatureNameStorage.Reload();
	else if(!stricmp(TableName, "gameobject_names"))	// GO Names
		GameObjectNameStorage.Reload();
	else if(!stricmp(TableName, "areatriggers"))		// Areatriggers
		AreaTriggerStorage.Reload();
	else if(!stricmp(TableName, "itempages"))			// Item Pages
		ItemPageStorage.Reload();
	else if(!stricmp(TableName, "worldstring_tables"))			// wst
		WorldStringTableStorage.Reload();
	else if(!stricmp(TableName, "worldbroadcast"))			// wb
		WorldBroadCastStorage.Reload();
	else if(!stricmp(TableName, "quests"))				// Quests
		QuestStorage.Reload();
	else if(!stricmp(TableName, "npc_text"))			// NPC Text Storage
		NpcTextStorage.Reload();
	else if(!stricmp(TableName, "fishing"))				// Fishing Zones
		FishingZoneStorage.Reload();
	else if(!stricmp(TableName, "teleport_coords"))		// Teleport coords
		TeleportCoordStorage.Reload();
	else if(!stricmp(TableName, "graveyards"))			// Graveyards
		GraveyardStorage.Reload();
	else if(!stricmp(TableName, "worldmap_info"))		// WorldMapInfo
		WorldMapInfoStorage.Reload();
	else if(!stricmp(TableName, "zoneguards"))
		ZoneGuardStorage.Reload();
	else if(!stricmp(TableName, "unit_display_sizes"))
		UnitModelSizeStorage.Reload();
	else if(!stricmp(TableName, "command_overrides"))	// Command Overrides
	{
		CommandTableStorage::getSingleton().Dealloc();
		CommandTableStorage::getSingleton().Init();
		CommandTableStorage::getSingleton().Load();
	}
	else
		return false;
	
	uint32 len = (uint32)strlen(TableName);
	uint32 len2;
	for(vector<pair<string,string> >::iterator itr = additionalTables.begin(); itr != additionalTables.end(); ++itr)
	{
		len2=(uint32)itr->second.length();
		if(!strnicmp(TableName, itr->second.c_str(), min(len,len2)))
			LoadAdditionalTable(TableName, itr->first.c_str());
	}
	return true;
}

void Storage_LoadAdditionalTables()
{
	ExtraMapCreatureTables.insert(string("creature_spawns"));
	ExtraMapGameObjectTables.insert(string("gameobject_spawns"));

	string strData = Config.MainConfig.GetStringDefault("Startup", "LoadAdditionalTables", "");
	if(strData.empty())
	{ 
		return;
	}

	vector<string> strs = StrSplit(strData, ",");
	if(strs.empty())
	{ 
		return;
	}

	for(vector<string>::iterator itr = strs.begin(); itr != strs.end(); ++itr)
	{
		char s1[200];
		char s2[200];
		if(sscanf((*itr).c_str(), "%s %s", s1, s2) != 2)
			continue;

		if(LoadAdditionalTable(s2, s1)) {
			pair<string,string> tmppair;
			tmppair.first = string(s1);
			tmppair.second = string(s2);
			additionalTables.push_back(tmppair);
		}
	}
}

void ObjectMgr::StoreBroadCastGroupKey() 
// cebernic: plz feedback
{
	vector<string> keyGroup;
	QueryResult * result = WorldDatabase.Query( "SELECT DISTINCT percent FROM `worldbroadcast` ORDER BY percent DESC" );
	// result->GetRowCount();
	if ( result != NULL )
	{
		do
		{
			Field *f = result->Fetch();
			keyGroup.push_back( string(f[0].GetString()) );
		}
		while( result->NextRow() );
		delete result;
		result = NULL;
	}
	
	if ( keyGroup.empty() )
	{
		Log.Notice("ObjectMgr", "These values from field `percent` in worldbroadcast table was empty!");
		return;
	}

	for(vector<string>::iterator itr = keyGroup.begin(); itr != keyGroup.end(); ++itr)
	{
		string curKey = (*itr);
		char szSQL[512];
		memset(szSQL,0,sizeof(szSQL));
		sprintf(szSQL,"SELECT entry,percent FROM `worldbroadcast` WHERE percent='%s' ",curKey.c_str());
		result = WorldDatabase.Query( szSQL );
		if ( result != NULL )
		{
			do
			{
				Field *f = result->Fetch();
				m_BCEntryStorage.insert(pair<uint32,uint32>( uint32(atoi(curKey.c_str())), f[0].GetUInt32()  ));
			}
			while( result->NextRow() );
			delete result;
			result = NULL;
		}
	}
	// cebernic for test
	/*for (;;)
	{
		uint32 n= CalcCurrentBCEntry();
		if ( n==0 ) {
			printf("table empty!\n");
			continue;
		}
		printf("current : %u\n",n);
		Sleep(1000);
	}*/

}
