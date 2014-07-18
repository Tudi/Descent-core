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

Creature::Creature(uint64 guid) : m_spawnLocation(0,0,0,0)
{
	m_valuesCount = UNIT_END;
	m_objectTypeId = TYPEID_UNIT;
	internal_object_type = INTERNAL_OBJECT_TYPE_CREATURE;
	m_uint32Values = _fields;
	memset(m_uint32Values, 0,(UNIT_END)*sizeof(uint32));
	m_updateMask.SetCount(UNIT_END);
	SetUInt32Value( OBJECT_FIELD_TYPE,TYPE_UNIT|TYPE_OBJECT);
	SetUInt64Value( OBJECT_FIELD_GUID,guid);
	SetFloatValue( UNIT_FIELD_HOVERHEIGHT, 1 );
	m_wowGuid.Init(GetGUID());

	SetUInt32Value( UNIT_FIELD_BASEATTACKTIME, 2000 ); //avoid division by 0
	SetUInt32Value( UNIT_FIELD_BASEATTACKTIME_1, 2000 ); //avoid division by 0
	SetUInt32Value( UNIT_FIELD_RANGEDATTACKTIME, 2000 ); //avoid division by 0

	m_quests = NULL;
	DificultyProto = proto = NULL;
	spawnid=0;
 
	creature_info=NULL;
	m_H_regenTimer=0;
	m_P_regenTimer=0;
	m_useAI = true;
	mTaxiNode = 0;

//	Tagged = false;
	TaggerGroupId = 0;
	TaggerGuid = 0;

	Skinned = false;
	
	m_enslaveCount = 0;
	m_enslaveSpell = 0;
	
	for(uint32 x=0;x<7;x++)
	{
		FlatResistanceMod[x]=0;
		BaseResistanceModPct[x]=0;
		ResistanceModPct[x]=0;
		ModDamageDone[x]=0;
		ModDamageDonePct[x]=100;
	}

	for(uint32 x=0;x<5;x++)
	{
		TotalStatModPct[x]=0;
		StatModPct[x]=0;
		FlatStatMod[x]=0;
	}

	totemOwner = NULL;
	totemSlot = -1;

	m_PickPocketed = false;
	m_SellItems = NULL;
	_myScriptClass = NULL;
	m_TaxiNode = 0;
	myFamily = 0;

	loot.gold = 0;
	haslinkupevent = false;
	original_emotestate = 0;
	mTrainer = 0;
	m_spawn = 0;
	spawnid = 0;
	auctionHouse = 0;
	has_waypoint_text = has_combat_text = false;
	SetFloatValue(UNIT_FIELD_ATTACK_POWER_MULTIPLIER,0.0f);
	SetFloatValue(UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER,0.0f);
	m_custom_waypoint_map = 0;
	m_escorter = 0;
	m_limbostate = false;
	m_corpseEvent=false;
	m_respawnCell=NULL;
	m_walkSpeed = 2.5f;
	m_runSpeed = MONSTER_NORMAL_RUN_SPEED;
	m_base_runSpeed = m_runSpeed;
	m_base_walkSpeed = m_walkSpeed;
	m_noRespawn = true;	//all creatures except real spawns
	m_respawnTimeOverride=0;
    m_canRegenerateHP = true;
	m_transportGuid = 0;
	m_transportPosition = NULL;
	BaseAttackType = SCHOOL_NORMAL;
	m_lootMethod = -1;
	m_healthfromspell = 0;
	m_speedFromHaste = 0;
	crit_chance_mod = 0;
	m_parentInfo = NULL;
	m_HighestThreath = 0;
	m_NextThreathUpdateStamp = 0;
}

Creature::~Creature()
{
	Virtual_Destructor();
}

void Creature::Virtual_Destructor()
{
	ObjectLock.Acquire();
	ASSERT( m_noRespawn == true );
	//i wonder which ones of these need to be deleted
	sEventMgr.RemoveEvents( this );
	//fallthrough destructor(Creature -> Unit) would make acces higher level structures that got freed
	RemoveAllAuras();

	if (GetScript() != NULL )
	{
		GetScript()->Destroy(); //deletes script !
		_myScriptClass = NULL;
	}
	mTrainer = NULL;
	m_SellItems = NULL;
	m_quests = NULL;
	creature_info = NULL;
	proto = NULL;
	m_transportPosition = NULL;
	m_escorter = NULL;
	m_spawn = NULL;
	myFamily = NULL;

	if( IsTotem() && totemOwner )
	{
		totemOwner->m_TotemSlots[totemSlot] = 0;
		totemOwner = NULL;
		totemSlot = -1;
	}
	if(m_parentInfo)
	{
		delete m_parentInfo;
		m_parentInfo = NULL;
	}
	DestroyCustomWaypointMap();
	//omg it seems we are getting deleted from somewhere else then cell destructor. Hard to imagine why we would have m_respawnCell set
	if( m_respawnCell != NULL && m_noRespawn == false )
	{
		m_respawnCell->_respawnObjects.erase(this);
		m_respawnCell = NULL;
	}/**/
	ObjectLock.Release();
	Unit::Virtual_Destructor();
	//avoid Unit destructor referencing to this object ass creature after creature destructor call
	//if unit destructor will acces creature fields after destructor it could create memcorruptions
	m_objectTypeId = TYPEID_UNUSED;
	internal_object_type = INTERNAL_OBJECT_TYPE_UNIT;
}

void Creature::Update( uint32 p_time )
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	Unit::Update( p_time );
	if( IsTotem() && IsDead() )
	{
		RemoveFromWorld(false, true);
		sGarbageCollection.AddObject( this );
		return;
	}

	if(m_corpseEvent)
	{
		sEventMgr.RemoveEvents(this);
		if(this->GetProto()==NULL)
			sEventMgr.AddEvent(this, &Creature::OnRemoveCorpse, EVENT_CREATURE_REMOVE_CORPSE, 1000, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
		else if (this->creature_info->Rank == ELITE_WORLDBOSS)
			sEventMgr.AddEvent(this, &Creature::OnRemoveCorpse, EVENT_CREATURE_REMOVE_CORPSE, TIME_CREATURE_REMOVE_BOSSCORPSE, 1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
		else if ( this->creature_info->Rank == ELITE_RAREELITE || this->creature_info->Rank == ELITE_RARE)
			sEventMgr.AddEvent(this, &Creature::OnRemoveCorpse, EVENT_CREATURE_REMOVE_CORPSE, TIME_CREATURE_REMOVE_RARECORPSE, 1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
		else
			sEventMgr.AddEvent(this, &Creature::OnRemoveCorpse, EVENT_CREATURE_REMOVE_CORPSE, TIME_CREATURE_REMOVE_CORPSE, 1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);

		m_corpseEvent=false;
	}
}

void Creature::SafeDelete()
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	if( !IsCreature() && !IsPet() )
	{
		sLog.outDebug(" Creature::SafeDelete : !!! omg invalid call on wrong object !\n");
		ASSERT(false);
		return;
	}
	//cause some might use this directly
	if( IsInWorld() )
		RemoveFromWorld( false, true );
	//this does not save us from having this event registered more then once
	sEventMgr.RemoveEvents(this);
	//do not delete map spawns
	if(	m_noRespawn == false )
		return;
	//sEventMgr.AddEvent(World::getSingletonPtr(), &World::DeleteObject, ((Object*)this), EVENT_CREATURE_SAFE_DELETE, 1000, 1);
//	sEventMgr.AddEvent(this, &Creature::DeleteMe, EVENT_CREATURE_SAFE_DELETE, 1000, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
	sEventMgr.AddEvent(this, &Creature::DeleteMe, EVENT_CREATURE_SAFE_DELETE, 1000, 1, EVENT_FLAG_DELETES_OBJECT | EVENT_FLAG_UNIQUE_EVENT_DELETE_NEWER);
}

void Creature::DeleteMe()
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	if(IsInWorld())
		RemoveFromWorld(false, true);
	//do not delete map spawns
	if(	m_noRespawn == false )
		return;

	sGarbageCollection.AddObject( this );
}

void Creature::OnRemoveCorpse()
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	// time to respawn!
	if (IsInWorld() && (int32)m_mapMgr->GetInstanceID() == m_instanceId)
	{

		sLog.outDetail("Removing corpse of "I64FMT"...", GetGUID());
	   
		if((GetMapMgr()->GetMapInfo() && IS_MAP_RAID( GetMapMgr()->GetMapInfo() ) && this->GetProto() && this->GetProto()->boss) || m_noRespawn)
		{
			RemoveFromWorld(false, true);
		}
		else
		{
			if((proto && proto->RespawnTime) || m_respawnTimeOverride)
			{
				RemoveFromWorld(true, false);
			}
			else
			{
				RemoveFromWorld(false, true);
			}
		}
		
	   
		setDeathState(DEAD);

		m_position = m_spawnLocation;
	}
	else
	{
		// if we got here it's pretty bad
	}
}

void Creature::OnRespawn(MapMgr * m)
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	if( m_noRespawn == true )
	{ 
		return;
	}

	InstanceBossInfoMap *bossInfoMap = objmgr.m_InstanceBossInfoMap[m->GetMapId()];
	if(bossInfoMap != NULL)
	{
		bool skip = false;
		Instance *pInstance = m->pInstance;
		for(std::set<uint32>::iterator killedNpc = pInstance->m_killedNpcs.begin(); killedNpc != pInstance->m_killedNpcs.end(); ++killedNpc)
		{
			// Is killed boss?
			if((*killedNpc) == this->creature_info->Id)
			{
				skip = true;
				break;
			}
			// Is add from killed boss?
			InstanceBossInfoMap::const_iterator bossInfo = bossInfoMap->find((*killedNpc));
			if(bossInfo != bossInfoMap->end() && bossInfo->second->trash.find(this->spawnid) != bossInfo->second->trash.end())
			{
				skip = true;
				break;
			}
		}
		if(skip)
		{
			this->m_noRespawn = true;
			this->SafeDelete();
			return;
		}
	}

	sLog.outDetail("Respawning "I64FMT"...", GetGUID());
	SetUInt32Value(UNIT_FIELD_HEALTH, GetUInt32Value(UNIT_FIELD_MAXHEALTH));
	SetUInt32Value(UNIT_DYNAMIC_FLAGS, 0); // not tagging shiat
	if(proto && m_spawn)
	{
		SetUInt32Value(UNIT_NPC_FLAGS, proto->NPCFLags);
		SetUInt32Value(UNIT_NPC_EMOTESTATE, m_spawn->emote_state);
	}

	RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);
	Skinned = false;
//	Tagged = false;
	TaggerGroupId = 0;
	TaggerGuid = 0;
	m_lootMethod = -1;
	loot.items.clear(); // make sure we did not keep previous loot. I think this was cleared already
	loot.currencies.clear();

	/* creature death state */
	if(proto && proto->death_state == 1)
	{
/*		uint32 newhealth = m_uint32Values[UNIT_FIELD_HEALTH] / 100;
		if(!newhealth)
			newhealth = 1;*/
		SetUInt32Value(UNIT_FIELD_HEALTH, 1);
		m_limbostate = true;
		bInvincible = true;
		setDeathState( CORPSE );
		SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_DEAD);
	}
	else
		setDeathState( ALIVE );

	GetAIInterface()->StopMovement(0); // after respawn monster can move
	m_PickPocketed = false;
	PushToWorld(m);
}

void Creature::Create (const char* name, uint32 mapid, float x, float y, float z, float ang)
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	Object::_Create( mapid, x, y, z, ang );
}

void Creature::CreateWayPoint (uint32 WayPointID, uint32 mapid, float x, float y, float z, float ang)
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	Object::_Create( mapid, x, y, z, ang);
}

///////////
/// Looting

bool Creature::generateLoot()
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	if (!loot.items.empty()) 
	{ 
		return false;
	}
	//do not generate loot for pets. Exploit for taming bosses in cataclysm
	if( IsPet() || GetUInt32Value( UNIT_FIELD_CREATEDBY ) )
	{
		return false;
	}
	//critters do not have loot ?
	if( GetCreatureInfo() && GetCreatureInfo()->Type == CRITTER )
	{
		return false;
	}
//	lootmgr.FillCreatureLoot(&loot,GetEntry(), m_mapMgr ? (m_mapMgr->instance_difficulty) : 0);
	if( DificultyProto )
	{
		if( DificultyProto->loot_kill )
			lootmgr.FillObjectLoot(this,DificultyProto->loot_kill);
		if( GetMapMgr() )
		{
			SimpleVect< LootCurencyStoreStruct* >		*loot_currency = NULL;
			if( DificultyProto->loot_currency )
				loot_currency = DificultyProto->loot_currency;
			else if( proto->loot_currency )
				loot_currency = proto->loot_currency;
			if( loot_currency )
			{
				uint32 difficulty_mask = 1 << (GetMapMgr()->instance_difficulty + 2 );
				uint32 max_size = loot_currency->GetMaxSize();
				for(uint32 it=0;it<max_size;it++)
				{
					LootCurencyStoreStruct *ls = loot_currency->GetValue( it );
					if( (ls->difficulty_mask & difficulty_mask) != 0 )
					{
						__LootCurrency cl;
						cl.CurrencyCount = ls->currency_amt;
						cl.CurrencyType = ls->currency_type;
						loot.currencies.push_back( cl );
					}
				}
			}
		}
	}
	
	loot.gold = DificultyProto ? DificultyProto->money : 0;
#ifdef BATTLEGRUND_REALM_BUILD
	loot.gold = 1;
#endif 

	// Master Looting Ninja Checker
	if(sWorld.antiMasterLootNinja && this->m_lootMethod == PARTY_LOOT_MASTER)
	{
		Player *looter = objmgr.GetPlayer((uint32)this->TaggerGuid);
		if(looter && looter->GetGroup())
		{
			uint16 lootThreshold = looter->GetGroup()->GetThreshold();

			for(vector<__LootItem>::iterator itr = loot.items.begin(); itr != loot.items.end(); itr++)
			{
				if(itr->item.itemproto->Quality < lootThreshold)
					continue;

				// Master Loot Stuff - Let the rest of the raid know what dropped..
				//TODO: Shouldn't we move this array to a global position? Or maybe it allready exists^^ (VirtualAngel) --- I can see (dead) talking pigs...^^
				char* itemColours[ITEM_MAX_KNOWN_ITEM_QUALITY+1] = { "9d9d9d", "ffffff", "1eff00", "0070dd", "a335ee", "ff8000", "e6cc80", "e6cc80" };
				char buffer[500];
				if( itr->item.itemproto->Quality < ITEM_MAX_KNOWN_ITEM_QUALITY )
					sprintf(buffer, "\174cff%s\174Hitem:%u:0:0:0:0:0:0:0\174h[%s]\174h\174r", itemColours[itr->item.itemproto->Quality], itr->item.itemproto->ItemId, itr->item.itemproto->Name1);
				else 
					sprintf(buffer, "\174cff%s\174Hitem:%u:0:0:0:0:0:0:0\174h[%s]\174h\174r", itemColours[ ITEM_QUALITY_POOR_GREY ], itr->item.itemproto->ItemId, itr->item.itemproto->Name1);
				this->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, buffer);
			}
		}
	}

	/*
	 * If there's an amount given, take it as an expected value and
	 * generated a corresponding random value. The random value is
	 * something similar to a normal distribution.
	 *
	 * You'd get a ``better'' distribution if you called `rand()' for each
	 * copper individually. However, if the loot was 1G we'd call `rand()'
	 * 15000 times, which is not ideal. So we use one call to `rand()' to
	 * (hopefully) get 24 random bits, which is then used to create a
	 * normal distribution over 1/24th of the difference.
	 */
	if ((loot.gold > 0) && (loot.gold < 12))
	{
		/* Don't use the below formula for very small cash - rouding
		 * errors will be too bad.. */
	}
	else if ( loot.gold >= 12 && DificultyProto->money < 0xEFFFFFFF )
	{
		uint32 random_bits;
		double chunk_size;
		double gold_fp;

		/* Split up the difference into 12 chunks.. */
		chunk_size = ((double) loot.gold) / 12.0;

		/* Get 24 random bits. We use the low order bits, because we're
		 * too lazy to check how many random bits the system actually
		 * returned. */
		random_bits = rand () & 0x00ffffff;

		gold_fp = 0.0;
		while (random_bits != 0)
		{
			/* If last bit is one .. */
			if ((random_bits & 0x01) == 1)
				/* .. increase loot by 1/12th of expected value */
				gold_fp += chunk_size;

			/* Shift away the LSB */
			random_bits >>= 1;
		}

		/* To hide your discrete values a bit, add another random
		 * amount between -(chunk_size/2) and +(chunk_size/2). */
		gold_fp += chunk_size
			* ((((double) rand ()) / (((double) RAND_MAX) + 1.0)) - .5);

		/*
		 * In theory we can end up with a negative amount. Give at
		 * least one chunk_size here to prevent this from happening. In
		 * case you're interested, the probability is around 2.98e-8.
		 */
		if (gold_fp < chunk_size)
			gold_fp = chunk_size;

		/* Convert the floating point gold value to an integer again
		 * and we're done. */
		loot.gold = (uint32) (.5 + gold_fp);
	}
	else if ( DificultyProto == NULL || DificultyProto->money == 0 )
		/* if(!loot.gold) */
	{
		CreatureInfo *info=GetCreatureInfo();
		if (info && info->Type != BEAST)
		{
			if(m_uint32Values[UNIT_FIELD_MAXHEALTH] <= 1667)
				loot.gold = (uint32)((info->Rank+1)*getLevel()*(rand()%5 + 1)); //generate copper
			else
				loot.gold = (uint32)((info->Rank+1)*getLevel()*(rand()%5 + 1)*(this->GetUInt32Value(UNIT_FIELD_MAXHEALTH)*0.0006)); //generate copper
		}
	}
	
	if(loot.gold)
		loot.gold = int32(float(loot.gold) * sWorld.getRate(RATE_MONEY));
#ifdef BATTLEGRUND_REALM_BUILD
	loot.gold = 1;
#endif 
	return ( loot.gold != 0 || loot.items.empty() == false );
}

void Creature::SaveToDB()
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	if(!spawnid)
		spawnid = objmgr.GenerateCreatureSpawnID();
	 
	std::stringstream ss;
	ss << "REPLACE INTO creature_spawns VALUES("
		<< spawnid << ","
		<< GetEntry() << ","
		<< GetMapId() << ","
		<< m_position.x << ","
		<< m_position.y << ","
		<< m_position.z << ","
		<< m_position.o << ","
		<< m_aiInterface->getMoveType() << ","
		<< m_uint32Values[UNIT_FIELD_DISPLAYID] << ","
		<< m_uint32Values[UNIT_FIELD_FACTIONTEMPLATE] << ","
		<< m_uint32Values[UNIT_FIELD_FLAGS] << ","
		<< m_uint32Values[UNIT_FIELD_BYTES_0] << ","
		<< m_uint32Values[UNIT_FIELD_BYTES_1] << ","
		<< m_uint32Values[UNIT_FIELD_BYTES_2] << ","
		<< m_uint32Values[UNIT_NPC_EMOTESTATE] << ",";
//		<< "0,";
		/*<< ((this->m_spawn ? m_spawn->respawnNpcLink : uint32(0))) << ",";*/

//	if(m_spawn)
//		ss << m_spawn->channel_spell << "," << m_spawn->channel_target_go << "," << m_spawn->channel_target_creature << ",";
//	else
//		ss << "0,0,0,";

	ss << uint32(GetStandState()) ;
	ss << "," << m_uint32Values[ UNIT_FIELD_MOUNTDISPLAYID ];
	ss << "," << m_uint32Values[ UNIT_VIRTUAL_ITEM_SLOT_ID ];
	ss << "," << m_uint32Values[ UNIT_VIRTUAL_ITEM_SLOT_ID_1 ];
	ss << "," << m_uint32Values[ UNIT_VIRTUAL_ITEM_SLOT_ID_2 ];
	if( m_spawn )
		ss << "," << m_spawn->difficulty_spawnmask;
	else
		ss << ",65535";
	if( m_spawn )
		ss << "," << m_spawn->phase;
	else
		ss << ",2147483647";
	ss << ")";
	WorldDatabase.Execute(ss.str().c_str());
}

void Creature::SaveToFile(std::stringstream & name)
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
/*	FILE * OutFile;

	OutFile = fopen(name.str().c_str(), "wb");
	if (!OutFile) return;
	
	uint32 creatureEntry = GetUInt32Value(OBJECT_FIELD_ENTRY);
	if (!m_sqlid)
		m_sqlid = objmgr.GenerateLowGuid(HIGHGUID_UNIT);

	std::stringstream ss;
	ss << "DELETE FROM creatures WHERE id=" << m_sqlid;
	fwrite(ss.str().c_str(),1,ss.str().size(),OutFile);

	ss.rdbuf()->str("");
	ss << "\nINSERT INTO creatures (id, mapId, zoneId, name_id, positionX, positionY, positionZ, orientation, moverandom, running, data) VALUES ( "
		<< m_sqlid << ", "
		<< GetMapId() << ", "
		<< GetZoneId() << ", "
		<< GetUInt32Value(OBJECT_FIELD_ENTRY) << ", "
		<< m_position.x << ", "
		<< m_position.y << ", "
		<< m_position.z << ", "
		<< m_position.o << ", "
		<< GetAIInterface()->getMoveType() << ", "
		<< GetAIInterface()->getMoveRunFlag() << ", '";
	for( uint16 index = 0; index < m_valuesCount; index ++ )
		ss << GetUInt32Value(index) << " ";

	ss << "' )";
	fwrite(ss.str().c_str(),1,ss.str().size(),OutFile);
	fclose(OutFile);*/
}


void Creature::LoadScript()
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	if( IsPet() == false )
		_myScriptClass = sScriptMgr.CreateAIScriptClassForEntry( this );
	else
		_myScriptClass = sScriptMgr.CreatePetAIScriptClassForEntry( this );
}

void Creature::DeleteFromDB()
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	if(!GetSQL_id())
	{ 
		return;
	}
	WorldDatabase.Execute("DELETE FROM creature_spawns WHERE id=%u", GetSQL_id());
	WorldDatabase.Execute("DELETE FROM creature_waypoints WHERE spawnid=%u",GetSQL_id());
	WorldDatabase.Execute("DELETE FROM creature_formations WHERE spawn_id=%u or target_spawn_id=%u ",GetSQL_id());
}


/////////////
/// Quests

void Creature::AddQuest(QuestRelation *Q)
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	m_quests->push_back(Q);
}

void Creature::DeleteQuest(QuestRelation *Q)
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	list<QuestRelation *>::iterator it;
	for ( it = m_quests->begin(); it != m_quests->end(); ++it )
	{
		if (((*it)->type == Q->type) && ((*it)->qst == Q->qst ))
		{
			delete (*it);
			(*it) = NULL;
			m_quests->erase(it);
			break;
		}
	}
}

Quest* Creature::FindQuest(uint32 quest_id, uint8 quest_relation)
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
   
	list<QuestRelation *>::iterator it;
	for (it = m_quests->begin(); it != m_quests->end(); ++it)
	{
		QuestRelation *ptr = (*it);

		if ((ptr->qst->id == quest_id) && (ptr->type & quest_relation))
		{
			return ptr->qst;
		}
	}
	return NULL;
}

uint16 Creature::GetQuestRelation(uint32 quest_id)
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	uint16 quest_relation = 0;
	list<QuestRelation *>::iterator it;

	for (it = m_quests->begin(); it != m_quests->end(); ++it)
	{
		if ((*it)->qst->id == quest_id)
		{
			quest_relation |= (*it)->type;
		}
	}
	return quest_relation;
}

uint32 Creature::NumOfQuests()
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	return (uint32)m_quests->size();
}

void Creature::_LoadQuests()
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	sQuestMgr.LoadNPCQuests(this);
}

void Creature::setDeathState(DeathState s) 
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	if(s == ALIVE)
	{
		RemoveFlag( UNIT_FIELD_FLAGS, UNIT_FLAG_DEAD );
		SetFlag( UNIT_FIELD_FLAGS_2, UNIT_FLAG_ALIVE );
	}

	if(s == JUST_DIED) 
	{

		GetAIInterface()->SetUnitToFollow(NULL);
		m_deathState = CORPSE;
		m_corpseEvent=true;
		
		/*sEventMgr.AddEvent(this, &Creature::OnRemoveCorpse, EVENT_CREATURE_REMOVE_CORPSE, 180000, 1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);*/
		if(m_enslaveSpell)
			RemoveEnslave();

		if(m_currentSpell)
			m_currentSpell->safe_cancel();

	}
	else 
		m_deathState = s;
}

void Creature::AddToWorld()
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	// force set faction
	if(m_faction == 0 || m_factionDBC == 0)
		_setFaction();

	if(creature_info == 0)
		creature_info = CreatureNameStorage.LookupEntry(GetEntry());

	if(creature_info == 0) 
	{ 
		return;
	}
	
	if(m_faction == 0 || m_factionDBC == 0)
	{ 
		return;
	}

	Object::AddToWorld();
}

void Creature::AddToWorld(MapMgr * pMapMgr)
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	// force set faction
	if(m_faction == 0 || m_factionDBC == 0)
		_setFaction();

	if(creature_info == 0)
		creature_info = CreatureNameStorage.LookupEntry(GetEntry());

	if(creature_info == 0) 
	{ 
		return;
	}

	if(m_faction == 0 || m_factionDBC == 0)
	{ 
		return;
	}

	Object::AddToWorld(pMapMgr);
}

bool Creature::CanAddToWorld()
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	if(m_factionDBC == 0 || m_faction == 0)
		_setFaction();

	if(creature_info == 0 || m_faction == 0 || m_factionDBC == 0 || proto == 0)
	{ 
		return false;
	}
	
	return true;
}

void Creature::RemoveFromWorld(bool addrespawnevent, bool free_guid)
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	//remove ai stuff
	sEventMgr.RemoveEvents(this, EVENT_CREATURE_AISPELL);

	RemoveAllAuras();
	
	if(IsInWorld())
	{
		if( IsPet() )
		{
			Unit::RemoveFromWorld( true );
			return;
		}
		
		uint32 delay = 0;
		if( addrespawnevent && ( m_respawnTimeOverride > 0 || ( DificultyProto && DificultyProto->RespawnTime > 0 ) ) )
			delay = m_respawnTimeOverride > 0 ? m_respawnTimeOverride : DificultyProto->RespawnTime;

		Despawn( 0, delay );
	}
}

void Creature::EnslaveExpire()
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	
	m_enslaveCount++;
	Player *caster = objmgr.GetPlayer(GetUInt32Value(UNIT_FIELD_CHARMEDBY));
	if(caster)
	{
		caster->SetUInt64Value(UNIT_FIELD_CHARM, 0);
		caster->SetUInt64Value(UNIT_FIELD_SUMMON, 0);
		sStackWorldPacket( data, SMSG_PET_SPELLS, 50 );
		data << uint64(0);
		data << uint32(0x00000000);
		caster->GetSession()->SendPacket(&data);
	}
	SetUInt64Value(UNIT_FIELD_CHARMEDBY, 0);
	SetUInt64Value(UNIT_FIELD_SUMMONEDBY, 0);

	m_walkSpeed = m_base_walkSpeed;
	m_runSpeed = m_base_runSpeed;

	switch(GetCreatureInfo()->Type)
	{
	case DEMON:
		SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, 90);
		break;
	default:
		SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, 954);
		break;
	};
	_setFaction();

	GetAIInterface()->Init(SafeUnitCast(this), AITYPE_AGRO, MOVEMENTTYPE_NONE);

	// Update InRangeSet
//	UpdateOppFactionSet();
//	UpdateSameFactionSet();
}

int32 Creature::RemoveEnslave()
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	return RemoveAura(m_enslaveSpell);
}
/*
void Creature::AddInRangeObject(Object* pObj)
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	Unit::AddInRangeObject(pObj);
}*/

void Creature::QueueRemoveInRangeObject(Object* pObj)
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	if(totemOwner == pObj)		// player gone out of range of the totem
	{
		// Expire next loop.
		event_ModifyTimeLeft(EVENT_TOTEM_EXPIRE, 1);
	}

	if(m_escorter == pObj)
	{
		// we lost our escorter, return to the spawn.
		m_aiInterface->StopMovement(10000);
		DestroyCustomWaypointMap();
		Despawn(1000, 1000);
	}

	Unit::QueueRemoveInRangeObject(pObj);
}
/*
void Creature::ClearInRangeSet()
{
	Unit::ClearInRangeSet();
}*/

void Creature::CalcResistance(uint32 type)
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	int32 pos = 0;
	int32 neg = 0;

	if( BaseResistanceModPct[ type ] < 0 )
		neg = ( BaseResistance[ type ] * abs(BaseResistanceModPct[ type ]) / 100 );
	else
		pos = ( BaseResistance[ type ] * BaseResistanceModPct[ type ] ) / 100;
	
	if( ResistanceModPct[ type ] < 0 )
		neg += ( BaseResistance[ type ] + pos - neg ) * abs(ResistanceModPct[ type ]) / 100;
	else
		pos += ( BaseResistance[ type ] + pos - neg ) * ResistanceModPct[ type ] / 100;
	
	if( FlatResistanceMod[ type ] < 0 )
		neg += abs(FlatResistanceMod[ type ]);
	else
		pos += FlatResistanceMod[ type ];
		
	SetUInt32Value( UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE + type, pos );
	SetUInt32Value( UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE + type, neg );

    int32 tot = BaseResistance[ type ] + pos - neg;
	
	SetUInt32Value( UNIT_FIELD_RESISTANCES + type, tot > 0 ? tot : 0);
}

void Creature::CalcStat(uint32 type)
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	int32 pos = 0;
	int32 neg = 0;

	if( StatModPct[ type ] < 0 )
		neg = ( BaseStats[ type ] * abs(StatModPct[ type ]) / 100 );
	else
		pos = ( BaseStats[ type ] * StatModPct[ type ] ) / 100;
	
	if( IsPet() )
	{
		Player* owner = SafePetCast( this )->GetPetOwner();
		if( type == STAT_STAMINA && owner )
//			pos += int32( 0.30f * owner->GetUInt32Value( UNIT_FIELD_STAT2 ) );	//says tooltip
			pos += int32( SafePetCast( this )->OwnerStamContribPCT * owner->GetUInt32Value( UNIT_FIELD_STAMINA ) / 100 );	//says wowwiki
		else if( type == STAT_INTELLECT && owner && GetUInt32Value( UNIT_CREATED_BY_SPELL ) )
			pos += int32( 0.30f * owner->GetUInt32Value( UNIT_FIELD_INTELLECT ) );
	}

	if( TotalStatModPct[ type ] < 0 )
		neg += ( BaseStats[ type ] + pos - neg ) * abs(TotalStatModPct[ type ]) / 100;
	else
		pos += ( BaseStats[ type ] + pos - neg ) * TotalStatModPct[ type ] / 100;
	
	if( FlatStatMod[ type ] < 0 )
		neg += abs(FlatStatMod[ type ]);
	else
		pos += FlatStatMod[ type ];
	
	SetUInt32Value( UNIT_FIELD_POSSTAT0 + type, pos );
	SetUInt32Value( UNIT_FIELD_NEGSTAT0 + type, neg );

    int32 tot = BaseStats[ type ] + pos - neg;
//	SetUInt32Value( UNIT_FIELD_STAT0 + type, tot > 0 ? tot : 0);
	SetUInt32Value( UNIT_FIELD_STAT0 + type, tot > 1 ? tot : 1);	//crashes client in 3.3.3 ?

	switch( type )
	{
	case STAT_STRENGTH:
		{
			//Attack Power
			if( !IsPet() )//We calculate pet's later
			{
				uint32 str = GetUInt32Value( UNIT_FIELD_STRENGTH );
				int32 AP = ( str * 2 - 20 );
				if( AP < 0 ) 
					AP = 0;
				SetUInt32Value( UNIT_FIELD_ATTACK_POWER, AP );
			}
			CalcDamage();
		}break;
	case STAT_AGILITY:
		{
			//Ranged Attack Power (Does any creature use this?)
			int32 RAP = GetUInt32Value( UNIT_FIELD_LEVEL ) + GetUInt32Value( UNIT_FIELD_AGILITY ) - 10;
			if( RAP < 0 ) 
				RAP = 0;
			SetUInt32Value( UNIT_FIELD_RANGED_ATTACK_POWER, RAP );
		}break;
	case STAT_STAMINA:
		{
			//Health
			uint32 hp = GetUInt32Value( UNIT_FIELD_BASE_HEALTH );
			uint32 stat_bonus = GetUInt32Value( UNIT_FIELD_POSSTAT2 ) - GetUInt32Value( UNIT_FIELD_NEGSTAT2 );
			if ( stat_bonus < 0 ) 
				stat_bonus = 0;
	
			uint32 bonus = stat_bonus * 10 + m_healthfromspell;
			uint32 res = hp + bonus;
	
			if( res < hp ) 
				res = hp;
			SetUInt32Value( UNIT_FIELD_MAXHEALTH, res );
			if( GetUInt32Value( UNIT_FIELD_HEALTH ) > GetUInt32Value( UNIT_FIELD_MAXHEALTH ) )
				SetUInt32Value( UNIT_FIELD_HEALTH, GetUInt32Value( UNIT_FIELD_MAXHEALTH ) );
		}break;
	case STAT_INTELLECT:
		{
			if( GetPowerType() == POWER_TYPE_MANA )
			{
				uint32 mana = GetUInt32Value( UNIT_FIELD_BASE_MANA );
				uint32 stat_bonus = ( GetUInt32Value( UNIT_FIELD_POSSTAT3 ) - GetUInt32Value( UNIT_FIELD_NEGSTAT3 ) );
				if( stat_bonus < 0 ) stat_bonus = 0;

				uint32 bonus = stat_bonus * 15;
				uint32 res = mana + bonus;

				if( res < mana ) res = mana;
				SetMaxPower( POWER_TYPE_MANA, res );
			}
		}break;
	}
}

void Creature::RegenerateHealth()
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	if(m_limbostate || !m_canRegenerateHP)
	{ 
		return;
	}

	uint32 cur=GetUInt32Value(UNIT_FIELD_HEALTH);
	uint32 mh=GetUInt32Value(UNIT_FIELD_MAXHEALTH);

	//bug happens on script initializations and some procs
	if(cur>mh)
		SetUInt32Value( UNIT_FIELD_HEALTH, mh );

	if(cur==mh)
	{ 
		return;
	}
	
	//though creatures have their stats we use some wierd formula for amt
	float amt = 0.0f;
	uint32 lvl = getLevel();

	amt = lvl*2.0f + RegenModifier;
	amt = ( amt * PctRegenModifier ) / 100;

	if (GetCreatureInfo() && GetCreatureInfo()->Rank == 3)
		amt *= 10000.0f;
	//Apply shit from conf file
	amt *= sWorld.getRate(RATE_HEALTH);
	
	if(amt<=1.0f)//this fixes regen like 0.98
		cur++;
	else
		cur+=(uint32)amt;
	SetUInt32Value(UNIT_FIELD_HEALTH,(cur>=mh)?mh:cur);
}

void Creature::RegenerateMana()
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	float amt;
	if (m_interruptRegen)
	{ 
		return;
	}
   
	uint32 cur=GetPower( POWER_TYPE_MANA );
	uint32 mm=GetMaxPower( POWER_TYPE_MANA );
	if(cur>=mm)
	{ 
		return;
	}
	amt=(getLevel()+10)*PctPowerRegenModifier[POWER_TYPE_MANA];
	
	//Apply shit from conf file
	amt*=sWorld.getRate(RATE_POWER1);
	if(amt<=1.0f && amt>0.0f)//this fixes regen like 0.98
		cur++;
	else
		cur+=(uint32)amt;
	SetPower( POWER_TYPE_MANA,(cur>=mm)?mm:cur);
}

void Creature::RegenerateFocus()
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	if (m_interruptRegen)
	{ 
		return;
	}

	uint32 cur=GetPower(POWER_TYPE_FOCUS);
	uint32 mm=GetMaxPower(POWER_TYPE_FOCUS);
	if(cur>=mm)
	{ 
		return;
	}
	//called every 2 seconds
	//client thinks 5 focus / sec
	float amt = 10.0f * ( 100.0f + PctPowerRegenModifier[POWER_TYPE_FOCUS] ) / 100.0f;
	cur+=(uint32)amt;
	SetPower(POWER_TYPE_FOCUS,(cur>=mm)?mm:cur);
}

void Creature::RegenerateEnergy()
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	uint32 cur = GetPower( POWER_TYPE_ENERGY );
	uint32 mh = GetMaxPower( POWER_TYPE_ENERGY );
	if(cur >= mh)
	{ 
		return;
	}

	float amt = 10.0f;

	cur += float2int32(amt);
	SetPower( POWER_TYPE_ENERGY, cur);
}

void Creature::CallScriptUpdate()
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	ASSERT(_myScriptClass);
	if(!IsInWorld())
	{ 
		return;
	}

	//not sure this is correct. But if we do not have MapCell unload enabled once a creature gets spawned he will keep spamming he's AIUpdate all the time forever. And yes it does cause random crashes
	if( Active == false )
	{
		return;
	}

	if( _myScriptClass )
		_myScriptClass->AIUpdate();
}

void Creature::AddVendorItem(uint32 itemid, uint32 amount, ItemExtendedCostEntry * ec)
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	CreatureItem ci;
	ci.buy_amount = amount;
	ci.itemid = itemid;
	ci.available_amount = 0;
	ci.max_amount = 0;
	ci.incrtime = 0;
	ci.extended_cost = ec;
	if(!m_SellItems)
	{
		m_SellItems = new vector<CreatureItem>;
		objmgr.SetVendorList(GetEntry(), m_SellItems);
	}
	m_SellItems->push_back(ci);
}
void Creature::ModAvItemAmount(uint32 itemid, uint32 value)
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	for(std::vector<CreatureItem>::iterator itr = m_SellItems->begin(); itr != m_SellItems->end(); ++itr)
	{
		if(itr->itemid == itemid)
		{
			if(itr->available_amount)
			{
				if(value > itr->available_amount)	// shouldnt happen
				{
					itr->available_amount=0;
					return;
				}
				else
					itr->available_amount -= value;
                
				if(!event_HasEvent(EVENT_ITEM_UPDATE))
					sEventMgr.AddEvent(this, &Creature::UpdateItemAmount, itr->itemid, EVENT_ITEM_UPDATE, itr->incrtime, 1, 0);
			}
			return;
		}
	}
}
void Creature::UpdateItemAmount(uint32 itemid)
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	for(std::vector<CreatureItem>::iterator itr = m_SellItems->begin(); itr != m_SellItems->end(); ++itr)
	{
		if(itr->itemid == itemid)
		{
			if (itr->max_amount==0)		// shouldnt happen
				itr->available_amount=0;
			else
			{
				itr->available_amount = itr->max_amount;
			}
			return;
		}
	}
}

void Creature::TotemExpire()
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	if( totemOwner != NULL )
	{
		totemOwner->m_TotemSlots[totemSlot] = 0;
	}
	
	totemSlot = -1;
	totemOwner = NULL;

//	if(pOwner != NULL)
//		DestroyForPlayer(pOwner); //make sure the client knows it's gone...

	if( IsInWorld() )
		RemoveFromWorld( false, true );
//	else 
		SafeDelete();
}

void Creature::FormationLinkUp(uint32 SqlId)
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	if(!m_mapMgr)		// shouldnt happen
	{ 
		return;
	}

	Creature * creature = m_mapMgr->GetSqlIdCreature(SqlId);
	if(creature != 0)
	{
		m_aiInterface->m_formationLinkTarget = creature;
		haslinkupevent = false;
		event_RemoveEvents(EVENT_CREATURE_FORMATION_LINKUP);
	}
}
/*
void Creature::ChannelLinkUpGO(uint32 SqlId)
{
	if(!m_mapMgr)		// shouldnt happen
	{ 
		return;
	}

	GameObject * go = m_mapMgr->GetSqlIdGameObject(SqlId);
	if(go != 0)
	{
		event_RemoveEvents(EVENT_CREATURE_CHANNEL_LINKUP);
		SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, go->GetGUID());
		SetUInt32Value(UNIT_CHANNEL_SPELL, m_spawn->channel_spell);
	}
}

void Creature::ChannelLinkUpCreature(uint32 SqlId)
{
	if(!m_mapMgr)		// shouldnt happen
	{ 
		return;
	}

	Creature * go = m_mapMgr->GetSqlIdCreature(SqlId);
	if(go != 0)
	{
		event_RemoveEvents(EVENT_CREATURE_CHANNEL_LINKUP);
		SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, go->GetGUID());
		SetUInt32Value(UNIT_CHANNEL_SPELL, m_spawn->channel_spell);
	}
}*/

WayPoint * Creature::CreateWaypointStruct()
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	return new WayPoint();
}
//#define SAFE_FACTIONS

//GetDifficultyString( mode );
bool Creature::Load(CreatureSpawn *spawn, uint32 mode, MapInfo *info)
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	m_spawn = spawn;
	proto = CreatureProtoStorage.LookupEntry(spawn->entry);
	if(!proto)
	{ 
		return false;
	}
	creature_info = CreatureNameStorage.LookupEntry(spawn->entry);
	if(!creature_info)
	{ 
		return false;
	}
	
//	entry - normal entry (normal dungeon, normal creature, 10 man normal)
//	heroic_entry1 - heroic dungeon | 25 normal
//	heroic_entry2 - 10 heroic
//	heroic_entry3 - 25 heroic
	if( ( mode == INSTANCE_MODE_DUNGEON_HEROIC || mode == INSTANCE_MODE_RAID_25_MAN) && proto->heroic_mode_proto_redirects[0] )
		DificultyProto = CreatureProtoStorage.LookupEntry( proto->heroic_mode_proto_redirects[0] );
	else if( mode == INSTANCE_MODE_RAID_10_MAN_HEROIC && ( proto->heroic_mode_proto_redirects[1] || proto->heroic_mode_proto_redirects[0] ) )
	{
		if( proto->heroic_mode_proto_redirects[1] )
			DificultyProto = CreatureProtoStorage.LookupEntry( proto->heroic_mode_proto_redirects[1] );
		if( ( DificultyProto == NULL || DificultyProto->Id != proto->heroic_mode_proto_redirects[1] ) && proto->heroic_mode_proto_redirects[0] )
			DificultyProto = CreatureProtoStorage.LookupEntry( proto->heroic_mode_proto_redirects[0] );
	}
	else if( mode == INSTANCE_MODE_RAID_25_MAN_HEROIC && ( proto->heroic_mode_proto_redirects[2] || proto->heroic_mode_proto_redirects[1] || proto->heroic_mode_proto_redirects[0] )  )
	{
		if( proto->heroic_mode_proto_redirects[2] )
			DificultyProto = CreatureProtoStorage.LookupEntry( proto->heroic_mode_proto_redirects[2] );
		if( ( DificultyProto == NULL || DificultyProto->Id != proto->heroic_mode_proto_redirects[2] ) && proto->heroic_mode_proto_redirects[1] )
			DificultyProto = CreatureProtoStorage.LookupEntry( proto->heroic_mode_proto_redirects[1] );
		if( ( DificultyProto == NULL || ( DificultyProto->Id != proto->heroic_mode_proto_redirects[2] && DificultyProto->Id != proto->heroic_mode_proto_redirects[1] ) ) && proto->heroic_mode_proto_redirects[0] )
			DificultyProto = CreatureProtoStorage.LookupEntry( proto->heroic_mode_proto_redirects[0] );
	}
	if( DificultyProto == NULL || DificultyProto->Id == 0 || DificultyProto->Id == 1 )
		DificultyProto = proto;
	
	m_phase = spawn->phase;
	spawnid = spawn->id;

	m_walkSpeed = m_base_walkSpeed = DificultyProto->walk_speed; //set speeds
	m_runSpeed = m_base_runSpeed = DificultyProto->run_speed; //set speeds
//	m_maxSpeed = m_runSpeed;
	m_flySpeed = DificultyProto->fly_speed;

	//Set fields
	SetUInt32Value(OBJECT_FIELD_ENTRY,proto->Id);

	SetFloatValue(OBJECT_FIELD_SCALE_X,DificultyProto->Scale);
	
	SetMaxPower( POWER_TYPE_MANA,DificultyProto->Mana);
	SetPower( POWER_TYPE_MANA,DificultyProto->Mana);
	SetUInt32Value(UNIT_FIELD_BASE_MANA,DificultyProto->Mana);
	
	uint32 model;
	uint32 gender = creature_info->GenerateModelId(&model);

#ifdef _DEBUG
	SetUInt32Value(UNIT_FIELD_DISPLAYID,model); //debug mode we always have model
	setGender(gender);	//might block people from talking to NPC if gender is not set right
#else
	//release mode we hide trigger spawns
	if( spawn->displayid != 0 )
		SetUInt32Value(UNIT_FIELD_DISPLAYID, spawn->displayid ); //WANT TO HIDE SPAWN ?
	else
	{
		SetUInt32Value(UNIT_FIELD_DISPLAYID,model);
		setGender(gender);//might block people from talking to NPC if gender is not set right
	}
#endif

	SetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID,model);
	SetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID,spawn->MountedDisplayID);
	EventModelChange();

    //SetUInt32Value(UNIT_FIELD_LEVEL, (mode ? proto->Level + (info ? info->lvl_mod_a : 0) : proto->Level));
	SetUInt32Value(UNIT_FIELD_LEVEL, DificultyProto->MinLevel + (RandomUInt(DificultyProto->MaxLevel - DificultyProto->MinLevel)));
	if( 
//		DificultyProto != proto && 
		mode && info)
	{
		uint32 exrta_level = info->lvl_mod_a;
		if( mode == INSTANCE_MODE_DUNGEON_HEROIC )
			exrta_level = MAX( 2, info->lvl_mod_a / 5 );
		else if( mode == INSTANCE_MODE_RAID_10_MAN_HEROIC )
			exrta_level = MAX( 4, info->lvl_mod_a / 2 );
		else if( mode == INSTANCE_MODE_RAID_25_MAN_HEROIC )
			exrta_level = MAX( 6, info->lvl_mod_a  );
		ModUnsigned32Value(UNIT_FIELD_LEVEL, MIN( MAX( 0, sWorld.m_levelCap + 3 - GetUInt32Value(UNIT_FIELD_LEVEL) ), exrta_level));
	}

	uint32 health = DificultyProto->MinHealth + RandomUInt(DificultyProto->MaxHealth - DificultyProto->MinHealth);
	if( 
//		DificultyProto != proto && 
		IS_HEROIC_INSTANCE_DIFFICULTIE( mode ) )
		health = (uint32)( health * pow( sWorld.mob_HeroicHPEXPPerLVL,(int)getLevel() ) );
	SetUInt32Value(UNIT_FIELD_HEALTH, health);
	SetUInt32Value(UNIT_FIELD_MAXHEALTH, health);
	SetUInt32Value(UNIT_FIELD_BASE_HEALTH, health);

	for(uint32 i = 0; i < 7; ++i)
		SetUInt32Value(UNIT_FIELD_RESISTANCES+i,DificultyProto->Resistances[i]);

	if( 
//		DificultyProto != proto && 
		IS_HEROIC_INSTANCE_DIFFICULTIE( mode ) )
	{
		uint32 res = GetUInt32Value(UNIT_FIELD_RESISTANCES);
		res = (uint32)( res * pow(sWorld.mob_HeroicArmorEXPPerLVL,(int)getLevel() ) );
		res = (uint32)( res + sWorld.mob_HeroicArmorFlatPerLVL * getLevel() );
		ModUnsigned32Value(UNIT_FIELD_RESISTANCES, res);
		for(uint32 i = 1; i < 7; ++i)
		{
			res = GetUInt32Value(UNIT_FIELD_RESISTANCES + i);
			res = (uint32)( res * pow(sWorld.mob_HeroicSpellResEXPPerLVL,(int)getLevel() ) );
			res = (uint32)( res + sWorld.mob_HeroicResFlatPerLVL * getLevel() );
			ModUnsigned32Value(UNIT_FIELD_RESISTANCES+i,res);
		}
	}

	SetUInt32Value(UNIT_FIELD_BASEATTACKTIME,DificultyProto->AttackTime);
	if( 
//		DificultyProto != proto && 
		IS_HEROIC_INSTANCE_DIFFICULTIE( mode ) )
	{
		int32 AS = GetUInt32Value(UNIT_FIELD_BASEATTACKTIME);
		AS = (int32)( AS - sWorld.mob_HeroicAttackFlatSpeedPerLVL * getLevel() );
		if( AS < 500 )
			AS = 500;
		SetUInt32Value(UNIT_FIELD_BASEATTACKTIME, AS );
	}
	SetUInt32Value( UNIT_FIELD_BASEATTACKTIME_1, GetUInt32Value( UNIT_FIELD_BASEATTACKTIME ) ); //avoid division by 0
	SetUInt32Value( UNIT_FIELD_RANGEDATTACKTIME, GetUInt32Value( UNIT_FIELD_BASEATTACKTIME ) ); //avoid division by 0

	SetFloatValue(UNIT_FIELD_MINDAMAGE, DificultyProto->MinDamage);
	SetFloatValue(UNIT_FIELD_MAXDAMAGE, DificultyProto->MaxDamage);

	if( 
//		DificultyProto != proto && 
		IS_HEROIC_INSTANCE_DIFFICULTIE( mode ) )
	{
		float dmg;
		dmg = GetFloatValue(UNIT_FIELD_MINDAMAGE);
		dmg = dmg + pow(sWorld.mob_HeroicDMGEXPPerLVL,(int)getLevel() );
		SetFloatValue(UNIT_FIELD_MINDAMAGE, dmg );

		dmg = GetFloatValue(UNIT_FIELD_MAXDAMAGE);
		dmg = dmg  + pow(sWorld.mob_HeroicDMGEXPPerLVL,(int)getLevel() );
		SetFloatValue(UNIT_FIELD_MAXDAMAGE, dmg );
	}

	SetUInt32Value(UNIT_FIELD_RANGEDATTACKTIME,DificultyProto->RangedAttackTime);
	SetFloatValue(UNIT_FIELD_MINRANGEDDAMAGE,DificultyProto->RangedMinDamage);
	SetFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE,DificultyProto->RangedMaxDamage);

	SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, spawn->Item1SlotDisplay);
	SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID_1, spawn->Item2SlotDisplay);
	SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID_2, spawn->Item3SlotDisplay);

	SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, spawn->factionid);
	SetUInt32Value(UNIT_FIELD_FLAGS, spawn->flags);
	SetUInt32Value(UNIT_NPC_EMOTESTATE, spawn->emote_state);
	SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, DificultyProto->BoundingRadius);
	SetFloatValue(UNIT_FIELD_COMBATREACH, DificultyProto->CombatReach);
	original_emotestate = spawn->emote_state;
	// set position
	m_position.ChangeCoords( spawn->x, spawn->y, spawn->z, spawn->o );
	m_spawnLocation.ChangeCoords(spawn->x, spawn->y, spawn->z, spawn->o);
	m_aiInterface->setMoveType(spawn->movetype);	

	m_aiInterface->timed_emotes = objmgr.GetTimedEmoteList(spawn->id);

	m_faction = dbcFactionTemplate.LookupEntry(spawn->factionid);
	if(m_faction)
	{
		m_factionDBC = dbcFaction.LookupEntry(m_faction->Faction);
		// not a neutral creature
		if(!(m_factionDBC->RepListId == -1 && m_faction->HostileMask == 0 && m_faction->FriendlyMask == 0))
		{
			GetAIInterface()->m_canCallForHelp = true;
		}
	}


//SETUP NPC FLAGS
	SetUInt32Value(UNIT_NPC_FLAGS,DificultyProto->NPCFLags);

	if ( HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_VENDOR ) )
		m_SellItems = objmgr.GetVendorList(GetEntry());

	if ( HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER ) )
		_LoadQuests();

	if ( HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_TAXIVENDOR) )
		m_TaxiNode = sTaxiMgr.GetNearestTaxiNode( m_position.x, m_position.y, m_position.z, GetMapId() );

	if ( HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_TRAINER) || HasFlag(UNIT_NPC_FLAGS,UNIT_NPC_FLAG_TRAINER_PROF))
		mTrainer = objmgr.GetTrainer(GetEntry());

	if ( HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_AUCTIONEER ) )
		auctionHouse = sAuctionMgr.GetAuctionHouse(GetEntry());

	//enable unit collision for active NPCs. This is to avoid people sitting on the NPC and not letting others click it.
	//did not test this feature yet 
//	if( GetUInt32Value( UNIT_NPC_FLAGS ) != 0 )
//		SetFlag( UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_9 | UNIT_FLAG_PASSIVE );

//NPC FLAGS
	 m_aiInterface->SetWaypointMap( objmgr.GetWayPointMap(spawn->id) );
 
	//load resistances
	for(uint32 x=0;x<7;x++)
		BaseResistance[x]=GetUInt32Value(UNIT_FIELD_RESISTANCES+x);
	for(uint32 x=0;x<5;x++)
		BaseStats[x]=GetUInt32Value(UNIT_FIELD_STAT0+x);

	BaseDamage[0]=GetFloatValue(UNIT_FIELD_MINDAMAGE);
	BaseDamage[1]=GetFloatValue(UNIT_FIELD_MAXDAMAGE);
	BaseOffhandDamage[0]=GetFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE);
	BaseOffhandDamage[1]=GetFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE);
	BaseRangedDamage[0]=GetFloatValue(UNIT_FIELD_MINRANGEDDAMAGE);
	BaseRangedDamage[1]=GetFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE);
	BaseAttackType=DificultyProto->AttackType;

	SetFloatValue(UNIT_MOD_CAST_SPEED, 1.0f);   // better set this one
	SetUInt32Value(UNIT_FIELD_BYTES_0, spawn->bytes0);
	{
		//this seems to happen only in 403 realm.Might be related that byte 2 needs a value for byte 3 ?
		setGender(0);//wtf random bug that female NPC refuse to reply to me if this is set to "1", or maybe value needs to be inversed ?
	}
	SetUInt32Value(UNIT_FIELD_BYTES_1, spawn->bytes1);
	SetUInt32Value(UNIT_FIELD_BYTES_2, spawn->bytes2);

////////////AI
	
	// kek	
//	for(list<AI_Spell*>::iterator itr = proto->spells.begin(); itr != proto->spells.end(); ++itr)
//		m_aiInterface->addSpellToList(*itr);
	if( DificultyProto->spells.size() )
		for(list<AI_Spell*>::iterator itr = DificultyProto->spells.begin(); itr != DificultyProto->spells.end(); ++itr)
			m_aiInterface->addSpellToList(*itr);
	else if( proto->spells.size() )
		for(list<AI_Spell*>::iterator itr = proto->spells.begin(); itr != proto->spells.end(); ++itr)
			m_aiInterface->addSpellToList(*itr);
	//m_aiInterface->m_canCallForHelp = proto->m_canCallForHelp;
	//m_aiInterface->m_CallForHelpHealth = proto->m_callForHelpHealth;
	m_aiInterface->m_canFlee = DificultyProto->m_canFlee;
	m_aiInterface->m_FleeHealth = DificultyProto->m_fleeHealth;
	m_aiInterface->m_FleeDuration = DificultyProto->m_fleeDuration;
	m_aiInterface->m_moveFly = ( m_aiInterface->m_moveFly == true) | (( DificultyProto->extra_a9_flags & A9_FLAGS2_NOGRAVITY ) != 0);

	//these fields are always 0 in db
	GetAIInterface()->setMoveType(0);
	GetAIInterface()->setMoveRunFlag(0);
	
	// load formation data
	if( spawn->form != NULL )
	{
		m_aiInterface->m_formationLinkSqlId = spawn->form->fol;
		m_aiInterface->m_formationFollowDistance = spawn->form->dist;
		m_aiInterface->m_formationFollowAngle = spawn->form->ang;
	}
	else
	{
		m_aiInterface->m_formationLinkSqlId = 0;
		m_aiInterface->m_formationFollowDistance = 0;
		m_aiInterface->m_formationFollowAngle = 0;
	}

//////////////AI

	myFamily = dbcCreatureFamily.LookupEntryForced(creature_info->Family);

	
// PLACE FOR DIRTY FIX BASTARDS
	// HACK! set call for help on civ health @ 100%
//	if(creature_info->Civilian >= 1)
//		m_aiInterface->m_CallForHelpHealth = 100;
 
 //HACK!
	if(m_uint32Values[UNIT_FIELD_DISPLAYID] == 17743 ||
		m_uint32Values[UNIT_FIELD_DISPLAYID] == 20242 ||
		m_uint32Values[UNIT_FIELD_DISPLAYID] == 15435 ||
		(creature_info->Family == UNIT_TYPE_MISC))
	{
		m_useAI = false;
	}

	/* more hacks! */
	if( DificultyProto->Mana != 0 )
		SetPowerType(POWER_TYPE_MANA );
	else
		SetPowerType( POWER_TYPE_MANA );
	PowerFields[ POWER_TYPE_MANA ] = 0;	//mana is stored in slot 0

	has_combat_text = objmgr.HasMonsterSay(GetEntry(), MONSTER_SAY_EVENT_ENTER_COMBAT);
	has_waypoint_text = objmgr.HasMonsterSay(GetEntry(), MONSTER_SAY_EVENT_RANDOM_WAYPOINT);
	m_aiInterface->m_isGuard = isGuard(GetEntry());
	m_aiInterface->m_isNeutralGuard = isNeutralGuard(GetEntry());
	m_aiInterface->getMoveFlags();

	//if Spirit Healer don't move
	if( isSpiritService() ) 
	{ 
		bInvincible = true;
		m_aiInterface->disable_melee = true;
		m_aiInterface->disable_combat = true;
		m_aiInterface->disable_targeting = true;
	}

	/* creature death state */
	if(DificultyProto->death_state == 1)
	{
/*		uint32 newhealth = m_uint32Values[UNIT_FIELD_HEALTH] / 100;
		if(!newhealth)
			newhealth = 1;*/
		SetUInt32Value(UNIT_FIELD_HEALTH, 1);
		m_limbostate = true;
		bInvincible = true;
		setDeathState( CORPSE );
		SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_DEAD);
	}
	else
		setDeathState( ALIVE );
	m_invisFlag |= ( (uint64)1 << DificultyProto->invisibility_type );
	if( spawn->stand_state )
		SetStandState( (uint8)spawn->stand_state );

	if( IS_HEROIC_INSTANCE_DIFFICULTIE( mode ) )
		for(int i=0;i<7;i++)
			SetFloatValue(UNIT_FIELD_POWER_COST_MULTIPLIER,1.0f / ( sWorld.mob_HeroicSpellCostPerLVL * getLevel() ) );

	ConvertToVehicle( proto );

	m_aiInterface->EventAiInterfaceParamsetFinish();

	SetFloatValue( UNIT_FIELD_HOVERHEIGHT, proto->hoverheight );
	
	return true;
}


void Creature::Load(CreatureProto * proto_, float x, float y, float z, float o)
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	DificultyProto = proto = proto_;

	creature_info = CreatureNameStorage.LookupEntry(proto->Id);
	if(!creature_info)
	{ 
		return;
	}

	m_walkSpeed = m_base_walkSpeed = proto->walk_speed; //set speeds
	m_runSpeed = m_base_runSpeed = proto->run_speed; //set speeds

	//Set fields
	SetUInt32Value(OBJECT_FIELD_ENTRY,proto->Id);
	SetFloatValue(OBJECT_FIELD_SCALE_X,proto->Scale);

	//SetUInt32Value(UNIT_FIELD_HEALTH, (mode ? long2int32(proto->Health * 1.5)  : proto->Health));
	//SetUInt32Value(UNIT_FIELD_BASE_HEALTH, (mode ? long2int32(proto->Health * 1.5)  : proto->Health));
	//SetUInt32Value(UNIT_FIELD_MAXHEALTH, (mode ? long2int32(proto->Health * 1.5)  : proto->Health));
	uint32 health = proto->MinHealth + RandomUInt(proto->MaxHealth - proto->MinHealth);

	SetUInt32Value(UNIT_FIELD_HEALTH, health);
	SetUInt32Value(UNIT_FIELD_MAXHEALTH, health);
	SetUInt32Value(UNIT_FIELD_BASE_HEALTH, health);

	SetPowerType( POWER_TYPE_MANA );
	PowerFields[ POWER_TYPE_MANA ] = 0;	//mana is stored in slot 0
	SetMaxPower( POWER_TYPE_MANA,proto->Mana);
	SetPower( POWER_TYPE_MANA,proto->Mana);
	SetUInt32Value(UNIT_FIELD_BASE_MANA,proto->Mana);

	uint32 model;
	uint32 gender = creature_info->GenerateModelId(&model);
	setGender(gender);

	SetUInt32Value(UNIT_FIELD_DISPLAYID,model);
	SetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID,model);
//	SetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID,proto->MountedDisplayID);
	EventModelChange();

	//SetUInt32Value(UNIT_FIELD_LEVEL, (mode ? proto->Level + (info ? info->lvl_mod_a : 0) : proto->Level));
	SetUInt32Value(UNIT_FIELD_LEVEL, proto->MinLevel + (RandomUInt(proto->MaxLevel - proto->MinLevel)));

	if( GetUInt32Value(UNIT_FIELD_LEVEL ) > PLAYER_LEVEL_CAP + 3)
		SetUInt32Value( UNIT_FIELD_LEVEL, PLAYER_LEVEL_CAP + 3 );

	for(uint32 i = 0; i < 7; ++i)
		SetUInt32Value(UNIT_FIELD_RESISTANCES+i,proto->Resistances[i]);

	SetUInt32Value(UNIT_FIELD_BASEATTACKTIME,proto->AttackTime);
	SetFloatValue(UNIT_FIELD_MINDAMAGE, proto->MinDamage);
	SetFloatValue(UNIT_FIELD_MAXDAMAGE, proto->MaxDamage);

	SetUInt32Value(UNIT_FIELD_RANGEDATTACKTIME,proto->RangedAttackTime);
	SetFloatValue(UNIT_FIELD_MINRANGEDDAMAGE,proto->RangedMinDamage);
	SetFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE,proto->RangedMaxDamage);

//	SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, proto->Item1SlotID);
//	SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY_01, proto->Item2SlotID);
//	SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY_02, proto->Item3SlotID);

	SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, proto->Faction);
	SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, proto->BoundingRadius);
	SetFloatValue(UNIT_FIELD_COMBATREACH, proto->CombatReach);
	original_emotestate = 0;
	// set position

	//lift flying creatures above the ground. Non Flying ones will fall to ground
	if( m_spawn == NULL )
		z += proto->hoverheight;

	m_position.ChangeCoords( x, y, z, o );
	m_spawnLocation.ChangeCoords(x, y, z, o);
	m_faction = dbcFactionTemplate.LookupEntry(proto->Faction);

	if(m_faction)
	{
		m_factionDBC = dbcFaction.LookupEntry(m_faction->Faction);
		// not a neutral creature
		if(!(m_factionDBC->RepListId == -1 && m_faction->HostileMask == 0 && m_faction->FriendlyMask == 0))
		{
			GetAIInterface()->m_canCallForHelp = true;
		}
	}

	//SETUP NPC FLAGS
	SetUInt32Value(UNIT_NPC_FLAGS,proto->NPCFLags);

	if ( HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_VENDOR ) )
		m_SellItems = objmgr.GetVendorList(GetEntry());

	if ( HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER ) )
		_LoadQuests();

	if ( HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_TAXIVENDOR) )
		m_TaxiNode = sTaxiMgr.GetNearestTaxiNode( m_position.x, m_position.y, m_position.z, GetMapId() );

	if ( HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_TRAINER) || HasFlag(UNIT_NPC_FLAGS,UNIT_NPC_FLAG_TRAINER_PROF))
		mTrainer = objmgr.GetTrainer(GetEntry());

	if ( HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_AUCTIONEER ) )
		auctionHouse = sAuctionMgr.GetAuctionHouse(GetEntry());

	//load resistances
	for(uint32 x=0;x<7;x++)
		BaseResistance[x]=GetUInt32Value(UNIT_FIELD_RESISTANCES+x);
	for(uint32 x=0;x<5;x++)
		BaseStats[x]=GetUInt32Value(UNIT_FIELD_STAT0+x);

	BaseDamage[0]=GetFloatValue(UNIT_FIELD_MINDAMAGE);
	BaseDamage[1]=GetFloatValue(UNIT_FIELD_MAXDAMAGE);
	BaseOffhandDamage[0]=GetFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE);
	BaseOffhandDamage[1]=GetFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE);
	BaseRangedDamage[0]=GetFloatValue(UNIT_FIELD_MINRANGEDDAMAGE);
	BaseRangedDamage[1]=GetFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE);
	BaseAttackType=proto->AttackType;

	SetFloatValue(UNIT_MOD_CAST_SPEED, 1.0f);   // better set this one

	////////////AI

	// kek
	for(list<AI_Spell*>::iterator itr = proto->spells.begin(); itr != proto->spells.end(); ++itr)
	{
		m_aiInterface->addSpellToList(*itr);
	}
	m_aiInterface->m_canCallForHelp = proto->m_canCallForHelp;
	m_aiInterface->m_CallForHelpHealth = proto->m_callForHelpHealth;
	m_aiInterface->m_canFlee = proto->m_canFlee;
	m_aiInterface->m_FleeHealth = proto->m_fleeHealth;
	m_aiInterface->m_FleeDuration = proto->m_fleeDuration;
	m_aiInterface->m_moveFly = ( m_aiInterface->m_moveFly == true) | (( proto->extra_a9_flags & A9_FLAGS2_NOGRAVITY ) != 0);

	//these fields are always 0 in db
	GetAIInterface()->setMoveType(0);
	GetAIInterface()->setMoveRunFlag(0);

	// load formation data
	m_aiInterface->m_formationLinkSqlId = 0;
	m_aiInterface->m_formationFollowDistance = 0;
	m_aiInterface->m_formationFollowAngle = 0;

	//critters sacrifice and die
	if( creature_info->Type == CRITTER )
	{
		m_aiInterface->disable_combat = true;
		m_aiInterface->disable_melee = true;
		m_aiInterface->disable_ranged = true;
		m_aiInterface->disable_spell = true;
	}

	//////////////AI

	myFamily = dbcCreatureFamily.LookupEntryForced(creature_info->Family);


	// PLACE FOR DIRTY FIX BASTARDS
	// HACK! set call for help on civ health @ 100%
//	if(creature_info->Civilian >= 1)
//		m_aiInterface->m_CallForHelpHealth = 100;

	//HACK!
	if(m_uint32Values[UNIT_FIELD_DISPLAYID] == 17743 ||
		m_uint32Values[UNIT_FIELD_DISPLAYID] == 20242 ||
		m_uint32Values[UNIT_FIELD_DISPLAYID] == 15435 ||
		(creature_info->Family == UNIT_TYPE_MISC))
	{
		m_useAI = false;
	}

	/* more hacks! */
	if(proto->Mana != 0)
		SetPowerType(POWER_TYPE_MANA );
	else
		SetPowerType( POWER_TYPE_MANA );

	has_combat_text = objmgr.HasMonsterSay(GetEntry(), MONSTER_SAY_EVENT_ENTER_COMBAT);
	has_waypoint_text = objmgr.HasMonsterSay(GetEntry(), MONSTER_SAY_EVENT_RANDOM_WAYPOINT);
	m_aiInterface->m_isGuard = isGuard(GetEntry());
	m_aiInterface->m_isNeutralGuard = isNeutralGuard(GetEntry());

	m_aiInterface->getMoveFlags();

	/* creature death state */
	if(proto->death_state == 1)
	{
/*		uint32 newhealth = m_uint32Values[UNIT_FIELD_HEALTH] / 100;
		if(!newhealth)
			newhealth = 1;*/
		SetUInt32Value(UNIT_FIELD_HEALTH, 1);
		m_limbostate = true;
		bInvincible = true;
		setDeathState( CORPSE );
		SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_DEAD);
	}
	else
		setDeathState( ALIVE );
	m_invisFlag |= ( (uint64)1 << proto->invisibility_type );
}

void Creature::RecastProtoAuras()
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	if(DificultyProto)
	{
		set<uint32>::iterator itr = DificultyProto->start_auras.begin();
		SpellEntry * sp;
		for(; itr != DificultyProto->start_auras.end(); ++itr)
		{
			sp = dbcSpell.LookupEntryForced((*itr));
			if(sp == 0) continue;

#ifdef FORCE_DISABLE_CREATURE_PROTO_PERIODIC_AURAS
			if( sp->eff[0].Effect == SPELL_EFFECT_PERSISTENT_AREA_AURA 
				|| sp->eff[1].Effect == SPELL_EFFECT_PERSISTENT_AREA_AURA 
				|| sp->eff[2].Effect == SPELL_EFFECT_PERSISTENT_AREA_AURA 
				|| sp->eff[0].Effect == SPELL_EFFECT_APPLY_AREA_AURA 
				|| sp->eff[1].Effect == SPELL_EFFECT_APPLY_AREA_AURA 
				|| sp->eff[2].Effect == SPELL_EFFECT_APPLY_AREA_AURA 
				|| sp->eff[0].EffectApplyAuraName == SPELL_AURA_PERIODIC_TRIGGER_SPELL 
				|| sp->eff[1].EffectApplyAuraName == SPELL_AURA_PERIODIC_TRIGGER_SPELL 
				|| sp->eff[2].EffectApplyAuraName == SPELL_AURA_PERIODIC_TRIGGER_SPELL 
				|| sp->eff[0].EffectApplyAuraName == SPELL_AURA_PERIODIC_TRIGGER_SPELL_WITH_VALUE 
				|| sp->eff[1].EffectApplyAuraName == SPELL_AURA_PERIODIC_TRIGGER_SPELL_WITH_VALUE 
				|| sp->eff[2].EffectApplyAuraName == SPELL_AURA_PERIODIC_TRIGGER_SPELL_WITH_VALUE )
				continue;
#endif
			CastSpell(this, sp, 0);
		}
		//generic ai stuff
//		if ( this->GetProto()->AISpells[0] != 0 )
//			sEventMgr.AddEvent(this, &Creature::AISpellUpdate, EVENT_CREATURE_AISPELL, 500, 0, 0);
	}
}

void Creature::OnPrePushToWorld()
{
	if(_myScriptClass)
		_myScriptClass->OnPrePushToWorld();
}

void Creature::OnPushToWorld()
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	sEventMgr.AddEvent(this, &Creature::RecastProtoAuras, EVENT_SEND_AURAS_TO_PLAYER, 250, 1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT); 

	LoadScript();
	Unit::OnPushToWorld();

//	if( GetParentSpawnID() )
//		EnterVehicle( GetMapMgr()->GetSqlIdVehicle( GetParentSpawnID() ), GetParentInfo()->seatid);

	if(_myScriptClass)
		_myScriptClass->OnLoad();

	if(m_spawn)
	{
		if(m_aiInterface->m_formationLinkSqlId)
		{
			// add event
			sEventMgr.AddEvent(this, &Creature::FormationLinkUp, m_aiInterface->m_formationLinkSqlId,
				EVENT_CREATURE_FORMATION_LINKUP, 1000, 0,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
			haslinkupevent = true;
		}

/*		if(m_spawn->channel_target_creature)
		{
			sEventMgr.AddEvent(this, &Creature::ChannelLinkUpCreature, m_spawn->channel_target_creature, EVENT_CREATURE_CHANNEL_LINKUP, 1000, 5, 0);	// only 5 attempts
		}
		
		if(m_spawn->channel_target_go)
		{
			sEventMgr.AddEvent(this, &Creature::ChannelLinkUpGO, m_spawn->channel_target_go, EVENT_CREATURE_CHANNEL_LINKUP, 1000, 5, 0);	// only 5 attempts
		}*/
	}

	if (this->HasItems())
	{
		for(std::vector<CreatureItem>::iterator itr = m_SellItems->begin(); itr != m_SellItems->end(); ++itr)
		{
				if (itr->max_amount == 0)
					itr->available_amount=0;
				else if (itr->available_amount<(uint32)itr->max_amount)				
					sEventMgr.AddEvent(this, &Creature::UpdateItemAmount, itr->itemid, EVENT_ITEM_UPDATE, VENDOR_ITEMS_UPDATE_TIME, 1,0);
		}

	}
	CALL_INSTANCE_SCRIPT_EVENT( m_mapMgr, OnCreaturePushToWorld )( this );
}

// this is used for guardians. They are non respawnable creatures linked to a player
void Creature::SummonExpire()
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	if( IsInWorld() )
		RemoveFromWorld(false, true);
//	else
		SafeDelete();
}

void Creature::Despawn(uint32 delay, uint32 respawntime)
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	if( !IsCreature() && !IsPet() )
	{
		sLog.outDebug(" Creature::Despawn : !!! omg invalid call on wrong object !\n");
		ASSERT(false);
		return;
	}

	//do not loop add / readd events if map is already shutting down
//	if( GetMapMgr()->_shutdown == true )
//		return;

	if( delay )
	{
		//tough luck, we cannot register more then 1 despawn event
		if( sEventMgr.HasEvent(this,EVENT_CREATURE_RESPAWN) == false )
		{
			//this is useless in case called from creature event update list. Might deadlock
//			sEventMgr.RemoveEvents(this, EVENT_CREATURE_RESPAWN);
			sEventMgr.AddEvent(this, &Creature::Despawn, (uint32)0, respawntime, EVENT_CREATURE_RESPAWN, delay, 1,0);
		}
		return;
	}

	//we are 99% not in world at this point
/*	if(!IsInWorld())
	{ 
		return;
	}*/

	if( GetMapMgr() && GetMapMgr()->GetMapInfo() && IS_MAP_RAID( GetMapMgr()->GetMapInfo() ) )
		if(GetCreatureInfo() && GetCreatureInfo()->Rank == 3)
			GetMapMgr()->RemoveCombatInProgress(GetGUID());

	if( _myScriptClass )
		_myScriptClass->OnRemoveFromWorld();

	if(	respawntime 
		&& GetMapMgr() != NULL //can't add respawn event to a non existing map
		&& m_noRespawn == false 
		&& IsPet() == false	//this should not happen but better be sure
		&& GetSQL_id() //only mobs with "static" spawn will respawn. This is to avoid temporal spawns to respawn
		&& m_spawn
		&& GetMapMgr()->_shutdown == false //map is shutting down. Do not create corrupted pointers of deleted creatures
		)
	{
		//we always respawn on same spot
		m_respawnCell = m_mapMgr->GetCellByCoords( m_spawnLocation.x, m_spawnLocation.y);

		//when cell gets activated again he will reload spawn table
		if( !m_respawnCell )
		{ 
			return; 
		}
	
		//cell is active so we insert ourself in respawn list
		m_respawnCell->_respawnObjects.insert( this );

		//make sure we do not have events regarding dead mob
		sEventMgr.RemoveEvents(this);

		//even if we are standing on some other cell we do respawn in respawn location
		sEventMgr.AddEvent(m_mapMgr, &MapMgr::EventRespawnCreature, this, m_spawnLocation.x, m_spawnLocation.y, EVENT_CREATURE_RESPAWN, respawntime, 1, 0);

		//all our events will go global. Mob will respawn even if there is no more cell for him
		Unit::RemoveFromWorld(false);

		//at this point only event manager may have reference to this object
	}
	else
	{
		//i hope we did not move from 1 cell to another without updating mapmanager or we might leave a pointer reference in some cell
/*		MapCell * curCell = GetMapCell();  //debug
		MapCell * standCell = GetMapMgr()->GetCellByCoords( GetPositionX(), GetPositionY() );  //debug
		if( standCell != curCell ) //debug
		{
			sLog.outDebug("Creature is not in he's own cell when died. Eighter corrupted pointer or not correctly moved object");
			isvalidcellpointer = GetMapMgr()->IsValidCellPointer( GetMapCell() ); //debug
			//this is not correct 
			if( !isvalidcellpointer ) //debug
			{
				sLog.outDebug("Creature is standing in an invalid (already deleted) cell.");
				SetMapCell( NULL );
			}
		}*/
		Unit::RemoveFromWorld(true);
		SafeDelete();
	}
}

void Creature::TriggerScriptEventStr(string func)
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	if( _myScriptClass )
		_myScriptClass->StringFunctionCallStr( func.c_str() );
}

void Creature::TriggerScriptEvent(int fRef)
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	if( _myScriptClass )
		_myScriptClass->StringFunctionCall(fRef);
}

void Creature::DestroyCustomWaypointMap()
{
	if(m_custom_waypoint_map)
	{
/*		
		//some script is already deleting this list and not clearing content. Right now leads to double delete and crash. Need to find the damn script
		for(WayPointMap::iterator itr = m_custom_waypoint_map->begin(); itr != m_custom_waypoint_map->end(); ++itr)
		{
			delete (*itr);
			(*itr) = NULL;
		}
		m_custom_waypoint_map->clear();
		delete m_custom_waypoint_map;*/
		m_custom_waypoint_map = NULL;
		m_aiInterface->SetWaypointMap( (WayPointVect*) NULL);
	}
}

void Creature::RemoveLimboState(Unit * healer)
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	if(!m_limbostate != true)
	{ 
		return;
	}

	m_limbostate = false;
	SetUInt32Value(UNIT_NPC_EMOTESTATE, m_spawn ? m_spawn->emote_state : 0);
	SetUInt32Value(UNIT_FIELD_HEALTH, GetUInt32Value(UNIT_FIELD_MAXHEALTH));
	bInvincible = false;
}

// Generates 3 random waypoints around the NPC
void Creature::SetGuardWaypoints()
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	if(!GetMapMgr()) 
	{ 
		return;
	}
	{ 
	if(!GetCreatureInfo()) 
	{ 
		return;
	}
	}

	GetAIInterface()->setMoveType(1);
	for(int i = 1; i <= 4; i++)
	{
		float ang = rand()/100.0f;
		float ran = (rand()%(100))/10.0f;
		while(ran < 1)
			ran = (rand()%(100))/10.0f;

		WayPoint * wp = new WayPoint;
		wp->id = i;
		wp->flags = 0;
		wp->waittime = 800;  /* these guards are antsy :P */
		wp->x = GetSpawnX()+ran*sin(ang);
		wp->y = GetSpawnY()+ran*cos(ang);

/*		if (sWorld.Collision)
		{
			wp->z = CollideInterface.GetHeight(m_mapId, wp->x, wp->y, m_spawnLocation.z + 2.0f);
			if( wp->z == NO_WMO_HEIGHT )
				wp->z = m_mapMgr->GetLandHeight(wp->x, wp->y);

			if( fabs( wp->z - m_spawnLocation.z ) > 10.0f )
				wp->z = m_spawnLocation.z;
		} 
		else */
		{
			wp->z = GetMapMgr()->GetLandHeight(wp->x, wp->y);
		}

		wp->o = 0;
		wp->backwardemoteid = 0;
		wp->backwardemoteoneshot = 0;
		wp->forwardemoteid = 0;
		wp->forwardemoteoneshot = 0;
		wp->backwardskinid = m_uint32Values[UNIT_FIELD_NATIVEDISPLAYID];
		wp->forwardskinid = m_uint32Values[UNIT_FIELD_NATIVEDISPLAYID];
		GetAIInterface()->addWayPoint(wp);
	}
}

uint32 Creature::GetNpcTextId()
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	return objmgr.GetGossipTextForNpc(this->GetEntry());
}

float Creature::GetBaseParry()
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	// TODO what are the parry rates for mobs?
	// FACT: bosses have varying parry rates (used to tune the difficulty of boss fights)

	// for now return a base of 5%, later get from dbase?
	return 5.0f;
}

bool Creature::CanSee(Unit* obj) // * Invisibility & Stealth Detection - Partha *
{
	INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
	if(!obj)
		return false;

	if( deleted != OBJ_AVAILABLE || obj->deleted != OBJ_AVAILABLE )
		return false;	//noway to see a deleted object :P

	if (obj == this)
	   return true;

	if( obj->GetUInt64Value( UNIT_FIELD_CREATEDBY ) == GetGUID() )
		return true; //we can see our own creations always

	if(obj->IsInvisible()) // Invisibility - Detection of Players and Units
	{
		if(obj->getDeathState() == CORPSE) // can't see dead players' spirits
			return false;

//		if(m_invisDetect[obj->m_invisFlag] < 1) // can't see invisible without proper detection
		if( (m_invisDetect & obj->m_invisFlag) != obj->m_invisFlag )
			return false;
	}

	float detectRange = 0.0f;
	if(obj->IsStealth()) // Stealth Detection (  I Hate Rogues :P  )
	{
		if(isInFront(obj)) // stealthed player is in front of creature
		{
			// Detection Range = 5yds + (Detection Skill - Stealth Skill)/5
//			if(getLevel() < 70)
//				detectRange = 5.0f + getLevel() + 0.2f * (float)(GetStealthDetectBonus() - obj->GetStealthLevel());
//			else
//				detectRange = 75.0f + 0.2f * (float)(GetStealthDetectBonus() - obj->GetStealthLevel());
			detectRange = 50.0f; 
		}
		detectRange += (float)(GetStealthDetectBonus() - obj->GetStealthLevel());

		if(detectRange < 1.0f) 
			detectRange = 1.0f; // Minimum Detection Range = 1yd

		detectRange += GetFloatValue(UNIT_FIELD_BOUNDINGRADIUS); // adjust range for size of creature
		detectRange += obj->GetFloatValue(UNIT_FIELD_BOUNDINGRADIUS); // adjust range for size of stealthed player

		if(GetDistanceSq(obj) > detectRange * detectRange)
		{
			//not dead sure about this. It was reported that if owner can see it then pet can see it also. Ex : Track Stealthed
			if( IsPet() && SafePetCast( this )->GetPetOwner() )
				return SafePetCast( this )->GetPetOwner()->CanSee( obj );

			return false;
		}
	}

	return true;
}

void Creature::SetSpawnPosition(const LocationVector & v)
{
	SetSpawnPosition(v.x, v.y, v.z, v.o);
}
 
void Creature::SetSpawnPosition(float newX, float newY, float newZ, float newOrientation)
{
	m_spawnLocation.x=newX;
	m_spawnLocation.y=newY;
	m_spawnLocation.z=newZ;
}
