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

//
// WorldCreator.cpp
//

#include "StdAfx.h"

SERVER_DECL InstanceMgr sInstanceMgr;
initialiseSingleton( FormationMgr );

InstanceMgr::InstanceMgr()
{
	memset(m_maps, 0, sizeof(Map*)* NUM_MAPS);
	memset(m_instances, 0, sizeof(InstanceMap*) * NUM_MAPS);
	memset(m_singleMaps,0, sizeof(MapMgr*) * NUM_MAPS);
	memset(&m_nextInstanceReset, 0, sizeof(time_t) * NUM_MAPS);
}

void InstanceMgr::Load(TaskList * l)
{
	// Create all non-instance type maps.
	QueryResult *result = CharacterDatabase.Query( "SELECT MAX(id) FROM instances" );
	if( result )
	{
		m_InstanceHigh = result->Fetch()[0].GetUInt32()+1;
		delete result;
		result = NULL;
	}
	else
		m_InstanceHigh = 1;

	// load each map we have in the database.
	result = WorldDatabase.Query("SELECT DISTINCT Map FROM creature_spawns");
	if(result)
	{
		do 
		{
			if(WorldMapInfoStorage.LookupEntry(result->Fetch()[0].GetUInt32()) == NULL)
				continue;

			if( result->Fetch()[0].GetUInt32() >= NUM_MAPS )
			{
				Log.Warning("InstanceMgr", "One or more of your creature_spawns rows specifies an invalid map: %u", result->Fetch()[0].GetUInt32() );
				continue;
			}

			//_CreateMap(result->Fetch()[0].GetUInt32());
			l->AddTask(new Task(new CallbackP1<InstanceMgr,uint32>(this, &InstanceMgr::_CreateMap, result->Fetch()[0].GetUInt32())));
		} while(result->NextRow());
		delete result;
		result = NULL;
	}

	l->wait();

	// create maps for any we don't have yet.
	StorageContainerIterator<MapInfo> * itr = WorldMapInfoStorage.MakeIterator();
	while(!itr->AtEnd())
	{
		if( itr->Get()->mapid >= NUM_MAPS )
		{
			Log.Warning("InstanceMgr", "One or more of your worldmap_info rows specifies an invalid map: %u", itr->Get()->mapid );
			itr->Inc();
			continue;
		}

		if(m_maps[itr->Get()->mapid] == NULL)
		{
			l->AddTask(new Task(new CallbackP1<InstanceMgr,uint32>(this, &InstanceMgr::_CreateMap, itr->Get()->mapid)));
		}
		//_CreateMap(itr->Get()->mapid);

		itr->Inc();
	}
	itr->Destruct();
	l->wait();

	// load reset times
	result = CharacterDatabase.Query("SELECT `setting_id`, `setting_value` FROM `server_settings` WHERE `setting_id` LIKE 'next_instance_reset_%%'");
	if(result)
	{
		do
		{
			const char *id = result->Fetch()[0].GetString();
			uint32 value = result->Fetch()[1].GetUInt32();
			if(strlen(id) <= 20)
				continue;
			uint32 mapId = atoi(id + 20);
			if(mapId >= NUM_MAPS)
				continue;
			m_nextInstanceReset[mapId] = value;
		}while(result->NextRow());
		delete result;
		result = NULL;
	}

	// load saved instances
	_LoadInstances();
}

InstanceMgr::~InstanceMgr()
{

}

void InstanceMgr::Shutdown()
{
	uint32 i;
	InstanceMap::iterator itr;
	for(i = 0; i < NUM_MAPS; ++i)
	{
		if(m_instances[i] != NULL)
		{
			for(itr = m_instances[i]->begin(); itr != m_instances[i]->end(); ++itr)
			{
				if(itr->second->m_mapMgr)
					itr->second->m_mapMgr->KillThread();

				delete itr->second;
				itr->second = NULL;
			}
			m_instances[i]->clear();
			delete m_instances[i];
			m_instances[i]=NULL;
		}

		if(m_singleMaps[i] != NULL)
		{
			MapMgr * ptr = m_singleMaps[i];
			ptr->KillThread();
			delete ptr;
			m_singleMaps[i]=NULL;
		}

		if(m_maps[i] != NULL)
		{
			delete m_maps[i];
			m_maps[i]=NULL;
		}
	}

	delete FormationMgr::getSingletonPtr();
}

uint32 InstanceMgr::PreTeleport(uint32 mapid, Player * plr, uint32 instanceid)
{
	// preteleport is where all the magic happens :P instance creation, etc.
	MapInfo * inf = WorldMapInfoStorage.LookupEntry(mapid);
	Group * pGroup;
	InstanceMap * instancemap;
	Instance * in;

	if(inf == NULL || mapid>=NUM_MAPS)
		return INSTANCE_ABORT_NOT_FOUND;

	// main continent check.
	if( IS_STATIC_MAP( inf ) )
	{
		// this will be useful when clustering comes into play.
		// we can check if the destination world server is online or not and then cancel them before they load.
		return (m_singleMaps[mapid] != NULL) ? INSTANCE_OK : INSTANCE_ABORT_NOT_FOUND;
	}

	// shouldn't happen
	if( IS_BG_OR_ARENA( inf ) )
		return INSTANCE_ABORT_NOT_FOUND;

	//check if mode is available
	uint32 difficulty_mask = 1 << (plr->GetInstanceDifficulty()+2);
	if( (inf->type_flags & difficulty_mask)==0)
	{
		plr->BroadcastMessage("could not find instance difficulty mode %s. Please choose another mode",GetDifficultyString( plr->GetInstanceDifficulty() ) );
		return INSTANCE_ABORT_NOT_FOUND;
	}
	pGroup = plr->GetGroup();

	if( !plr->TriggerpassCheat )
	{
		if( pGroup == NULL )
		{
			// players without groups cannot enter raids and heroic instances
			if( IS_MAP_RAID( inf ) && IS_MAP_DUNGEON( inf ) == false )
			{
				sLog.outDebug(" Pretellport : trying to enter a raid instance without raid group : exiting\n");
				plr->BroadcastMessage("Cannot enter a dungeon with a raid group");
				return INSTANCE_ABORT_NOT_IN_RAID_GROUP;
			}
		}
		else
		{
			// players without raid groups cannot enter raid instances
			if( pGroup->GetGroupType() != GROUP_TYPE_RAID )
			{
				if( IS_MAP_RAID( inf ) && IS_MAP_DUNGEON( inf ) == false  )
				{
					sLog.outDebug(" Pretellport : trying to enter a raid instance without raid group : exiting\n");
					plr->BroadcastMessage("You need a raid group to enter in this instance");
					return INSTANCE_ABORT_NOT_IN_RAID_GROUP;
				}
			}

			// if this is a dungeon and player tries to cheat bringing in extra players then we forbid entrance
			if( pGroup->GetGroupType() == GROUP_TYPE_RAID )
			{
				if( IS_MAP_DUNGEON( inf )&& IS_MAP_RAID( inf ) == false )
				{
					sLog.outDebug(" Pretellport : trying to enter a dungeon instance with raid group : exiting\n");
					plr->BroadcastMessage("Cannot enter a dungeon with a raid group");
					return INSTANCE_ABORT_NOT_IN_RAID_GROUP;
				}
			}
		}
	}

	// if we are here, it means:
	// 1) we're a non-raid instance
	// 2) we're a raid instance, and the person is in a group.
	// so, first we have to check if they have an instance on this map already, if so, allow them to teleport to that.
	// otherwise, we can create them a new one.
	m_mapLock.Acquire();
	instancemap = m_instances[mapid];

	if(instancemap == NULL)
	{
		if(instanceid != 0)
		{
			m_mapLock.Release();
			return INSTANCE_ABORT_NOT_FOUND;
		}

		// gotta create the hashmap.
		m_instances[mapid] = new InstanceMap;
		instancemap = m_instances[mapid];
	}
	else
	{
		InstanceMap::iterator itr;

		if(instanceid != 0)
		{
			itr = instancemap->find(instanceid);
			if(itr != instancemap->end())
			{
				in = itr->second;

				if( plr->TriggerpassCheat == false )
				{
					if(!CHECK_INSTANCE_GROUP(in, pGroup))
					{
						// Another group is already playing in this instance of the dungeon...
						m_mapLock.Release();
						sChatHandler.SystemMessageToPlr(plr, "Another group is already inside this instance of the dungeon.");
						return INSTANCE_ABORT_NOT_IN_RAID_GROUP;
					}

					uint32 tplayerlimit = MAX( in->GetCurrentPlayerLimit(), inf->playerlimit);
					if( in->m_mapMgr->GetPlayerCountPending() >= tplayerlimit )
					{
						m_mapLock.Release();
						return INSTANCE_ABORT_FULL;
					}
				}

				// Try to add instance ID to player and to group
				plr->SetPersistentInstanceId(in);

				// Set current group
				if(pGroup)
					in->m_creatorGroup = pGroup->GetID();

				m_mapLock.Release();
				return INSTANCE_OK;
			} 
			else 
			{
				m_mapLock.Release();
				return INSTANCE_ABORT_NOT_FOUND;
			}
		}
		else
		{
			in = NULL;
			uint32 Difficulty;
			if( pGroup )
				Difficulty = pGroup->GetInstanceDifficulty();
			else 
				Difficulty = plr->GetInstanceDifficulty();
			//order is : try to get player instance. 
			uint32 PlayerInstanceID = plr->GetPersistentInstanceId( mapid, Difficulty );
			if( PlayerInstanceID != 0 )
			{
				in = sInstanceMgr.GetInstanceByIds( mapid, PlayerInstanceID );
				if( in )
					plr->BroadcastMessage( "You have a personal saved instance. Resuming it" );
			}
			if( in == NULL && pGroup != NULL )
			{
				uint32 group_saved_instance_id = pGroup->GetSavedInstance( mapid, Difficulty );
				if( sWorld.instance_TakeGroupLeaderID )
				{
					PlayerInfo *pLeaderInfo = pGroup->GetLeader();
					if(pLeaderInfo)
					{
						pLeaderInfo->savedInstanceIdsLock.Acquire();
						PlayerInstanceMap::iterator itrLeader = pLeaderInfo->savedInstanceIds[Difficulty].find(mapid);
						if(itrLeader != pLeaderInfo->savedInstanceIds[Difficulty].end())
						{
							in = sInstanceMgr.GetInstanceByIds(mapid, (*itrLeader).second);
							if( in )
								plr->BroadcastMessage( "Group leader has a saved instance. Resuming it" );
						}
						pLeaderInfo->savedInstanceIdsLock.Release();
					}
				}
				if( in == NULL && group_saved_instance_id != GROUP_SAVED_INSTANCE_MISSING )
				{
					in = sInstanceMgr.GetInstanceByIds(mapid, group_saved_instance_id );
					if( in )
						plr->BroadcastMessage( "Group has a saved instance. Joining it" );
				}
			}

			if(in == NULL)
			{
				// search the instance and see if we have one here.
				for(itr = instancemap->begin(); itr != instancemap->end();)
				{
					in = itr->second;
					++itr;
					if( in->instance_difficulty == Difficulty && PlayerOwnsInstance(in, plr) )
						break;						
					in = NULL;
				}
				if( in )
					plr->BroadcastMessage( "Found saved instance. Resuming it" );
			}

			if(in != NULL)
			{
				m_mapLock.Release();

				// check the player count and in combat status.
				if( plr->TriggerpassCheat == false )
				{
					if(in->m_mapMgr )
					{
						if(in->m_mapMgr->IsCombatInProgress())
						{
							plr->BroadcastMessage("Cannot enter now, there is a combat going on inside right now. Wait until they finish");
							return INSTANCE_ABORT_ENCOUNTER;
						}
				
						uint32 tplayerlimit = MAX( in->GetCurrentPlayerLimit(), inf->playerlimit);
						if( in->m_mapMgr->GetPlayerCountPending() >= tplayerlimit )
						{
							plr->BroadcastMessage("Instance in current setup is full. Player limit now is %u",tplayerlimit);
							return INSTANCE_ABORT_FULL;
						}
					}

					if(!CHECK_INSTANCE_GROUP(in, pGroup))
					{
						// Another group is already playing in this instance of the dungeon...
						sChatHandler.SystemMessageToPlr(plr, "Another group is already inside this instance of the dungeon.");
						return INSTANCE_ABORT_NOT_IN_RAID_GROUP;
					}
				}
	
				// Try to add instance ID to player
				plr->SetPersistentInstanceId(in);
	
				// Set current group
				if(pGroup)
					in->m_creatorGroup = pGroup->GetID();

				plr->SetInstanceID(in->m_instanceId);

				// found our instance, allow him in.
				return INSTANCE_OK;
			}
		}
	}

	// if we're here, it means we need to create a new instance.
	in = new Instance;
	in->m_creation = UNIXTIME;
	if( pGroup )
		in->instance_difficulty = pGroup->GetInstanceDifficulty();
	else
		in->instance_difficulty = plr->GetInstanceDifficulty();
	in->m_instanceId = GenerateInstanceID();
	in->m_mapId = mapid;
	in->m_mapInfo = inf;
	in->m_mapMgr = NULL;		// always start off without a map manager, it is created in GetInstance()
	in->m_isBattleground = false;
	in->m_persistent = IS_PERSISTENT_INSTANCE(in) && objmgr.m_InstanceBossInfoMap[mapid] == NULL;
	in->m_creatorGuid = pGroup ? 0 : plr->GetLowGUID();		// creator guid is 0 if its owned by a group.
	in->m_creatorGroup = pGroup ? pGroup->GetID() : 0;

	if(sWorld.instance_SlidingExpiration)
	{
		if( IS_MAP_RAID( inf ) && IS_HEROIC_INSTANCE_DIFFICULTIE( in->instance_difficulty ))
			in->m_expiration = UNIXTIME + TIME_DAY;
		else
			in->m_expiration = ( IS_MAP_DUNGEON( inf ) || ( IS_MAP_RAID( inf ) && IS_NORMAL_INSTANCE_DIFFICULTIE( in->instance_difficulty ))) ? 0 : UNIXTIME + inf->cooldown;
	}
	else
	{
		if( IS_MAP_RAID( inf ) && IS_HEROIC_INSTANCE_DIFFICULTIE( in->instance_difficulty ))
		{
			in->m_expiration = UNIXTIME - (UNIXTIME % TIME_DAY) + ( (UNIXTIME % TIME_DAY) > (sWorld.instance_DailyHeroicInstanceResetHour * TIME_HOUR) ? 82800 : -3600 ) + ((sWorld.instance_DailyHeroicInstanceResetHour - sWorld.GMTTimeZone) * TIME_HOUR);
		}
		else if(IS_PERSISTENT_INSTANCE(in))
		{
			if(m_nextInstanceReset[in->m_mapId] == 0)
			{
				m_nextInstanceReset[in->m_mapId] = UNIXTIME - (UNIXTIME % TIME_DAY) - ((sWorld.GMTTimeZone + 1) * TIME_HOUR) + (in->m_mapInfo->cooldown == 0 ? TIME_DAY : in->m_mapInfo->cooldown);
				CharacterDatabase.Execute("REPLACE INTO `server_settings` (`setting_id`, `setting_value`) VALUES ('next_instance_reset_%u', '%u')", in->m_mapId, m_nextInstanceReset[in->m_mapId]);
			}
			if(m_nextInstanceReset[in->m_mapId] + (TIME_MINUTE * 15) < UNIXTIME)
			{
				do
				{
					time_t tmp = m_nextInstanceReset[in->m_mapId];
					if(tmp + (TIME_MINUTE * 15) < UNIXTIME)
						m_nextInstanceReset[in->m_mapId] = tmp + (in->m_mapInfo->cooldown == 0 ? TIME_DAY : in->m_mapInfo->cooldown);
				} while(m_nextInstanceReset[in->m_mapId] + (TIME_MINUTE * 15) < UNIXTIME);
				CharacterDatabase.Execute("REPLACE INTO `server_settings` (`setting_id`, `setting_value`) VALUES ('next_instance_reset_%u', '%u')", in->m_mapId, m_nextInstanceReset[in->m_mapId]);
			}
			in->m_expiration = m_nextInstanceReset[in->m_mapId];
		}
		else
		{
			in->m_expiration = ( IS_MAP_DUNGEON( inf ) || ( IS_MAP_RAID( inf ) && IS_NORMAL_INSTANCE_DIFFICULTIE( in->instance_difficulty ))) ? 0 : UNIXTIME + inf->cooldown;
		}
	}

	//this hack is required to not time out instance while loading screen is still in progress :(
	if( in->m_expiration )
		in->m_expiration = MAX( in->m_expiration, UNIXTIME + TIME_MINUTE * 2 );

	plr->SetInstanceID(in->m_instanceId);
	Log.Debug("InstanceMgr", "Creating instance for player %u and group %u on map %u. (%u)", in->m_creatorGuid, in->m_creatorGroup, in->m_mapId, in->m_instanceId);
	
	// save our new instance to the database.
	in->SaveToDB();
    
	// apply it in the instance map
	instancemap->insert( InstanceMap::value_type( in->m_instanceId, in ) );

	// Try to add instance ID to player
	plr->SetPersistentInstanceId(in);

	if( pGroup )
		plr->BroadcastMessage( "Creating new instance for group" );
	else
		plr->BroadcastMessage( "Creating new personal instance" );

	// instance created ok, i guess? return the ok for him to transport.
	m_mapLock.Release();
	return INSTANCE_OK;
}

MapMgr* InstanceMgr::GetMapMgr(uint32 mapId)
{
	return m_singleMaps[mapId];
}

MapMgr * InstanceMgr::GetInstance(Object* obj)
{
	Player * plr;
	Instance * in;
	InstanceMap::iterator itr;
	InstanceMap * instancemap;
	MapInfo * inf = WorldMapInfoStorage.LookupEntry(obj->GetMapId());

	// we can *never* teleport to maps without a mapinfo.
	if( inf == NULL || obj->GetMapId() >= NUM_MAPS )
		return NULL;

	if( obj->IsPlayer() )
	{
		// players can join instances based on their groups/solo status.
		plr = SafePlayerCast( obj );

		// single-instance maps never go into the instance set.
		if( IS_STATIC_MAP( inf ) )
			return m_singleMaps[obj->GetMapId()];

		m_mapLock.Acquire();
		instancemap = m_instances[obj->GetMapId()];
		if(instancemap != NULL)
		{
			// check our saved instance id. see if its valid, and if we can join before trying to find one.
			itr = instancemap->find(obj->GetInstanceID());
			if(itr != instancemap->end())
			{
				if(itr->second->m_mapMgr == NULL)
				{
					itr->second->m_mapMgr = _CreateInstance(itr->second);
				}
				if(itr->second->m_mapMgr)
				{
					m_mapLock.Release();
					return itr->second->m_mapMgr;
				}
			}

			// iterate over our instances, and see if any of them are owned/joinable by him.
			for(itr = instancemap->begin(); itr != instancemap->end();)
			{
				in = itr->second;
				++itr;
				if( in->instance_difficulty == plr->GetInstanceDifficulty() && PlayerOwnsInstance(in, plr))
				{
					// this is our instance.
					if(in->m_mapMgr == NULL)
					{
						/*if(plr->m_TeleportState == 1)
						{
							// the player is loading. boot him out to the entry point, we don't want to spam useless instances on startup.
							m_mapLock.Release();
							return NULL;
						}*/

						// create the actual instance.
						in->m_mapMgr = _CreateInstance(in);
						m_mapLock.Release();
						return in->m_mapMgr;
					}
					else
					{
						// instance is already created.
						m_mapLock.Release();
						return in->m_mapMgr;
					}
				}
			}
		}

		// if we're here, it means there are no instances on that map, or none of the instances on that map are joinable
		// by this player.
		m_mapLock.Release();
		return NULL;
	}
	else
	{
		// units are *always* limited to their set instance ids.
		if( IS_STATIC_MAP( inf ) )
			return m_singleMaps[obj->GetMapId()];

		m_mapLock.Acquire();
		instancemap = m_instances[obj->GetMapId()];
		if(instancemap)
		{
			itr = instancemap->find(obj->GetInstanceID());
			if(itr != instancemap->end())
			{
				// we never create instances just for units.
				m_mapLock.Release();
				return itr->second->m_mapMgr;
			}
		}

		// instance is non-existant (shouldn't really happen for units...)
		m_mapLock.Release();
		return NULL;
	}
}

MapMgr * InstanceMgr::_CreateInstance(uint32 mapid, uint32 instanceid)
{
	MapInfo * inf = WorldMapInfoStorage.LookupEntry(mapid);
	MapMgr * ret;

	ASSERT(inf && IS_STATIC_MAP( inf ) );
	ASSERT(mapid < NUM_MAPS && m_maps[mapid] != NULL);

	Log.Notice("InstanceMgr", "Creating continent %s.", m_maps[mapid]->GetName());

	ret = new MapMgr(m_maps[mapid], mapid, instanceid);
	ASSERT(ret);

	// start its thread
	ThreadPool.ExecuteTask(ret);
    
	// assign pointer
	m_singleMaps[mapid] = ret;
	return ret;
}

MapMgr * InstanceMgr::_CreateInstance(Instance * in)
{
	Log.Notice("InstanceMgr", "Creating saved instance %u (%s)", in->m_instanceId, m_maps[in->m_mapId]->GetName());
	ASSERT(in->m_mapMgr==NULL);

	// we don't have to check for world map info here, since the instance wouldn't have been saved if it didn't have any.
	in->m_mapMgr = new MapMgr(m_maps[in->m_mapId], in->m_mapId, in->m_instanceId);
	in->m_mapMgr->pInstance = in;
	in->m_mapMgr->instance_difficulty = in->instance_difficulty;
	in->m_mapMgr->InactiveMoveTime = 60+UNIXTIME;
	ThreadPool.ExecuteTask(in->m_mapMgr);
	return in->m_mapMgr;
}

void InstanceMgr::_CreateMap(uint32 mapid)
{
	if( mapid >= NUM_MAPS )
		return;

	MapInfo * inf;

	inf = WorldMapInfoStorage.LookupEntry(mapid);
	if(inf==NULL)
		return;
	if(m_maps[mapid]!=NULL)
		return;

	m_maps[mapid] = new Map(mapid, inf);
	if( IS_STATIC_MAP( inf ) )
	{
		// we're a continent, create the instance.
		_CreateInstance(mapid, GenerateInstanceID());
	}
}

uint32 InstanceMgr::GenerateInstanceID()
{
	uint32 iid;
	m_mapLock.Acquire();
	iid = m_InstanceHigh++;
	m_mapLock.Release();
	return iid;
}

void BuildStats(MapMgr * mgr, char * m_file, Instance * inst, MapInfo * inf)
{
	char tmp[200];
	strcpy(tmp, "");
#define pushline strcat(m_file, tmp)

	snprintf(tmp, 200, "	<instance>\n");																												pushline;
	snprintf(tmp, 200, "		<map>%u</map>\n", mgr->GetMapId());																						pushline;
	snprintf(tmp, 200, "		<maptype>%u</maptype>\n", inf->type_flags);																						pushline;
	snprintf(tmp, 200, "		<players>%u</players>\n", mgr->GetPlayerCount());																			pushline;
	snprintf(tmp, 200, "		<maxplayers>%u</maxplayers>\n", inf->playerlimit);																		pushline;

	//<creationtime>
	if (inst)
	{
		tm *ttime = localtime( &inst->m_creation );
		snprintf(tmp, 200, "		<creationtime>%02u:%02u:%02u %02u/%02u/%u</creationtime>\n",ttime->tm_hour, ttime->tm_min, ttime->tm_sec, ttime->tm_mday, ttime->tm_mon, uint32( ttime->tm_year + 1900 ));
		pushline;
	}
	else
	{
		snprintf(tmp, 200, "		<creationtime>N/A</creationtime>\n");
		pushline;
	}

	//<expirytime>
	if (inst && inst->m_expiration)
	{
		tm *ttime = localtime( &inst->m_expiration );
		snprintf(tmp, 200, "		<expirytime>%02u:%02u:%02u %02u/%02u/%u</expirytime>\n",ttime->tm_hour, ttime->tm_min, ttime->tm_sec, ttime->tm_mday, ttime->tm_mon, uint32( ttime->tm_year + 1900 ));
		pushline;
	}
	else
	{
		snprintf(tmp, 200, "		<expirytime>N/A</expirytime>\n");
		pushline;

	}
	//<idletime>
	if (mgr->InactiveMoveTime)
	{
		tm *ttime = localtime( &mgr->InactiveMoveTime );
		snprintf(tmp, 200, "		<idletime>%02u:%02u:%02u %02u/%02u/%u</idletime>\n",ttime->tm_hour, ttime->tm_min, ttime->tm_sec, ttime->tm_mday, ttime->tm_mon, uint32( ttime->tm_year + 1900 ));
		pushline;
	}
	else
	{
		snprintf(tmp, 200, "		<idletime>N/A</idletime>\n");
		pushline;
	}

	snprintf(tmp, 200, "	</instance>\n");																											pushline;
#undef pushline
}

void InstanceMgr::BuildXMLStats(char * m_file)
{
	uint32 i;
	InstanceMap::iterator itr;
	InstanceMap * instancemap;
	Instance * in;
	
	m_mapLock.Acquire();
	for(i = 0; i < NUM_MAPS; ++i)
	{
		if(m_singleMaps[i] != NULL)
			BuildStats(m_singleMaps[i], m_file, NULL, m_singleMaps[i]->GetMapInfo());
		else
		{
			instancemap = m_instances[i];
			if(instancemap != NULL)
			{
				for(itr = instancemap->begin(); itr != instancemap->end();)
				{
					in = itr->second;
					++itr;

					if(in->m_mapMgr==NULL)
						continue;

					BuildStats(in->m_mapMgr, m_file, in, in->m_mapInfo);
				}
			}
		}
	}
	m_mapLock.Release();
}

void InstanceMgr::_LoadInstances()
{
	MapInfo * inf;
	Instance * in;
	QueryResult * result;

	// clear any instances that have expired.
	Log.Notice("InstanceMgr", "Deleting Expired Instances...");
	CharacterDatabase.WaitExecute("DELETE FROM `instances` WHERE `expiration` > 0 AND `expiration` <= %u", UNIXTIME);
	CharacterDatabase.Execute("DELETE FROM `instanceids` WHERE `instanceid` NOT IN (SELECT `id` FROM `instances`)");
	
	// load saved instances
	result = CharacterDatabase.Query("SELECT `id`, `mapid`, `creation`, `expiration`, `killed_npc_guids`, `difficulty`, `creator_group`, `creator_guid`, `persistent` FROM instances");
	Log.Notice("InstanceMgr", "Loading %u saved instances." , result ? result->GetRowCount() : 0);

	if(result)
	{
		do 
		{
			inf = WorldMapInfoStorage.LookupEntry(result->Fetch()[1].GetUInt32());
			if(inf == NULL || result->Fetch()[1].GetUInt32() >= NUM_MAPS)
			{
				CharacterDatabase.Execute("DELETE FROM `instances` WHERE `mapid` = %u", result->Fetch()[1].GetUInt32());
				continue;
			}

			in = new Instance();
			in->m_mapInfo = inf;
			in->LoadFromDB(result->Fetch());

			// this assumes that groups are already loaded at this point.
			if(!in->m_persistent && in->m_creatorGroup && objmgr.GetGroupById(in->m_creatorGroup) == NULL)
			{
				CharacterDatabase.Execute("DELETE FROM `instances` WHERE `id` = %u", in->m_instanceId);
				delete in;
				in = NULL;
				continue;
			}

			if(m_instances[in->m_mapId] == NULL)
				m_instances[in->m_mapId] = new InstanceMap;

			(*m_instances[in->m_mapId])[in->m_instanceId] = in;

		} while(result->NextRow());
		delete result;
		result = NULL;
	}
}

void Instance::LoadFromDB(Field * fields)
{
	char * p, *q;
	char * m_npcstring = strdup_local(fields[4].GetString());

	m_instanceId = fields[0].GetUInt32();
	m_mapId = fields[1].GetUInt32();
	m_creation = fields[2].GetUInt32();
	m_expiration = fields[3].GetUInt32();
	instance_difficulty = fields[5].GetUInt32();
	m_creatorGroup = fields[6].GetUInt32();
	m_creatorGuid = fields[7].GetUInt32();
	m_persistent = fields[8].GetBool();
	m_mapMgr=NULL;
	m_isBattleground = false;

	// Reset group binding if it's a persistent instance
	if(m_persistent)
		m_creatorGroup = 0;

	// process saved npc's
	q = m_npcstring;
	p = strchr(m_npcstring, ' ');
	while(p)
	{
		*p = 0;
		uint32 val = atol(q);
		if (val)
			m_killedNpcs.insert( val );
		q = p+1;
		p = strchr(q, ' ');
	}

	free(m_npcstring);
}

void InstanceMgr::ResetSavedInstances(Player * plr)
{
//	WorldPacket *pData;
	Instance * in;
	InstanceMap::iterator itr;
	InstanceMap * instancemap;
	uint32 i;

	if( !plr->IsInWorld() || IS_STATIC_MAP( plr->GetMapMgr()->GetMapInfo() ) == false )
		return;

	m_mapLock.Acquire();
	Group *group = plr->GetGroup();
	for(i = 0; i < NUM_MAPS; ++i)
	{
		if(m_instances[i] != NULL)
		{
			instancemap = m_instances[i];
			for(itr = instancemap->begin(); itr != instancemap->end();)
			{
				in = itr->second;
				++itr;

				if(
					//zack : reset any instance we own
//					IS_RESETABLE_INSTANCE(in) && 
					(CHECK_INSTANCE_GROUP(in, plr->GetGroup()) || plr->GetLowGUID() == in->m_creatorGuid )
					)
				{
					if(in->m_mapMgr && in->m_mapMgr->HasPlayers())
					{
/*						pData = new WorldPacket(SMSG_RESET_INSTANCE_FAILED, 8);
						*pData << uint32(INSTANCE_RESET_ERROR_PLAYERS_INSIDE);
						*pData << uint32(in->m_mapId);
						plr->GetSession()->SendPacket(pData);
						delete pData;*/
						continue;
					}

					if(group)
					{
						group->RemoveSavedInstance( in->m_mapId, in->instance_difficulty );
					}
					if( plr->IsGroupLeader() && group )
						group->ResetInstanceForPlayers(in->m_mapId, in->instance_difficulty);
					else
						//if instance was persistent then we better remove it. Else it has no effect
						plr->SetPersistentInstanceId( in->m_mapId, in->instance_difficulty, 0 );

					// <mapid> has been reset.
/*					pData = new WorldPacket(SMSG_RESET_INSTANCE, 4);
					*pData << uint32(in->m_mapId);
					plr->GetSession()->SendPacket(pData);
					delete pData;*/

					// destroy the instance
					_DeleteInstance(in, true);
				}
			}
		}
	}
    m_mapLock.Release();	
}

void InstanceMgr::OnGroupDestruction(Group * pGroup)
{
	// this means a group has been deleted, so lets clear out all instances that they owned.
	// (instances don't transfer to the group leader, or anything)
	Instance * in;
	InstanceMap::iterator itr;
	InstanceMap * instancemap;
	uint32 i;

	m_mapLock.Acquire();
	for(i = 0; i < NUM_MAPS; ++i)
	{
		instancemap = m_instances[i];
		if(instancemap)
		{
			for(itr = instancemap->begin(); itr != instancemap->end();)
			{
				in = itr->second;
				++itr;

				if(in->m_mapMgr && in->m_creatorGroup && in->m_creatorGroup == pGroup->GetID())
				{
					if(IS_RESETABLE_INSTANCE(in))
					{
						_DeleteInstance(in, false);
					}
					else if(in->m_mapMgr->HasPlayers())
					{
						WorldPacket data(SMSG_RAID_GROUP_ONLY, 8);
						data << uint32(60000) << uint32(1);

						for(PlayerStorageMap::iterator mitr = in->m_mapMgr->m_PlayerStorage.begin(); mitr != in->m_mapMgr->m_PlayerStorage.end(); ++mitr)
						{
							if((*mitr).second->IsInWorld() && !(*mitr).second->raidgrouponlysent && (*mitr).second->GetInstanceID() == (int32)in->m_instanceId)
							{
								(*mitr).second->GetSession()->SendPacket(&data);
								(*mitr).second->raidgrouponlysent=true;

								sEventMgr.AddEvent((*mitr).second, &Player::EjectFromInstance, EVENT_PLAYER_EJECT_FROM_INSTANCE, 60000, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
							}	
						}
					}
				}
			}
		}
	}
	m_mapLock.Release();
}

bool InstanceMgr::_DeleteInstance(Instance * in, bool ForcePlayersOut,bool skip_db_delete)
{
	m_mapLock.Acquire();
	InstanceMap * instancemap;
	InstanceMap::iterator itr;

	//due to mem coruption this turn out to trugger sometimes :(
	//omg will this leave a deleted pointer in instances list ? Yes it will ! But this issue is comming from some other corruption problem that needs to be fixed first
	if( in->m_mapId > NUM_MAPS )
		return false;

	// remove the instance from the large map.
	instancemap = m_instances[in->m_mapId];
	if(instancemap)
	{
		itr = instancemap->find(in->m_instanceId);
		if(itr != instancemap->end())
			instancemap->erase(itr);
		else
		{
			m_mapLock.Release();
			return false;	//invalid instance to delete
		}
	}
	else
	{
		m_mapLock.Release();
		return false; //invalid instance to delete
	}

	if(in->m_mapMgr)
	{
		// "ForcePlayersOut" will teleport the players in this instance to their entry point/hearthstone.
		// otherwise, they will get a 60 second timeout telling them they are not in this instance's group.
		if(in->m_mapMgr->HasPlayers())
		{
			if(ForcePlayersOut)
				in->m_mapMgr->InstanceShutdown();
			else
			{
				in->m_mapMgr->BeginInstanceExpireCountdown();
				in->m_mapMgr->pInstance = NULL;
			}
		}
		else
			in->m_mapMgr->InstanceShutdown();
	}
    
	// cleanup corpses, database references
	if( skip_db_delete == false )
		in->DeleteFromDB();

	// delete the instance pointer.
	delete in;
	in = NULL;
	m_mapLock.Release();
	
	return true;
}

void Instance::DeleteFromDB()
{
	// cleanup all the corpses on this instance
	CharacterDatabase.Execute("DELETE FROM `corpses` WHERE `instanceid` = %u", m_instanceId);

	// delete from the database
	CharacterDatabase.Execute("DELETE FROM `instances` WHERE `id` = %u", m_instanceId);

	// Delete all instance assignments
	CharacterDatabase.Execute("DELETE FROM `instanceids` WHERE `mapId` = %u AND `instanceId` = %u AND `mode` = %u", m_mapId, m_instanceId, instance_difficulty);
}

void InstanceMgr::CheckForExpiredInstances()
{
	// checking for any expired instances.
	Instance * in;
	InstanceMap::iterator itr;
	InstanceMap * instancemap;
	uint32 i;

	m_mapLock.Acquire();
	for(i = 0; i < NUM_MAPS; ++i)
	{
		instancemap = m_instances[i];
		if(instancemap)
		{
			for(itr = instancemap->begin(); itr != instancemap->end();)
			{
				in = itr->second;
				++itr;

				// use a "soft" delete here.
				if( 
					( IS_RESETABLE_INSTANCE( in ) && HasInstanceExpired(in) )
//					this is handled also in mapmanager code :P
//					|| ( in->m_mapMgr && in->m_mapMgr->InactiveMoveTime && in->m_mapMgr->InactiveMoveTime < UNIXTIME )	//if any istance is inactive over a long period then shut it down
					)
					_DeleteInstance(in, false, true );
			}

		}
	}
	m_mapLock.Release();
}

void InstanceMgr::BuildSavedInstancesForPlayer(Player * plr)
{
	WorldPacket data(4);
	Instance * in;
	InstanceMap::iterator itr;
	InstanceMap * instancemap;
	uint32 i;

	if(!plr->IsInWorld() || IS_STATIC_MAP( plr->GetMapMgr()->GetMapInfo() ) == false )
	{
		m_mapLock.Acquire();
		for(i = 0; i < NUM_MAPS; ++i)
		{
			if(m_instances[i] != NULL)
			{
				instancemap = m_instances[i];
				for(itr = instancemap->begin(); itr != instancemap->end();)
				{
					in = itr->second;
					++itr;

					if( PlayerOwnsInstance(in, plr) && !IS_PERSISTENT_INSTANCE(in) )
					{
						m_mapLock.Release();

/*						data.SetOpcode(SMSG_INSTANCE_SAVE);
						data << uint32(in->m_mapId);
						plr->GetSession()->SendPacket(&data);

						data.Initialize(SMSG_INSTANCE_RESET_ACTIVATE);
						data << uint32(0x01);
						plr->GetSession()->SendPacket(&data);*/
					
						return;
					}
				}
			}
		}
		m_mapLock.Release();
	}

/*	data.SetOpcode(SMSG_INSTANCE_RESET_ACTIVATE);
	data << uint32(0x00);
	plr->GetSession()->SendPacket(&data);*/
}

void InstanceMgr::BuildRaidSavedInstancesForPlayer(Player * plr)
{
	WorldPacket data(SMSG_RAID_INSTANCE_INFO, 200);
/*
//3.3.3
0B 00 00 00 

70 02 00 00 WintergraspRaid
01 00 00 00 difficulty
15 0E ED 01 ID
00 00 41 1F 
01 00		- started instance
E7 91 07 00 - time passed since started
77 02 00 00 IcecrownCitadel
00 00 00 00 diff
33 67 ED 01 id
00 00 41 1F 
01 00 
E7 91 07 00 
53 02 00 00 StratholmeCOT
01 00 00 00 
81 4C C4 01 ID
00 00 41 1F 
00 00 
00 00 00 00 
67 02 00 00 ChamberOfAspectsBlack
00 00 00 00 
32 48 BE 01 ID
00 00 41 1F 
00 00 
00 00 00 00 
8A 02 00 00 ArgentTournamentDungeon
01 00 00 00 
01 5C BE 01 
00 00 41 1F
00 00 
00 00 00 00 
3F 02 00 00 01 00 00 00 A6 C3 C5 01 00 00 41 1F 00 00 00 00 00 00 89 02 00 00 03 00 00 00 71 A5 E8 01 00 00 41 1F 00 00 00 00 00 00 77 02 00 00 01 00 00 00 0A 36 E2 01 00 00 41 1F 00 00 00 00 00 00 5B 02 00 00 00 00 00 00 DC 47 C6 01 00 00 41 1F 00 00 00 00 00 00 89 02 00 00 02 00 00 00 1A D3 C6 01 00 00 41 1F 00 00 00 00 00 00 89 02 00 00 00 00 00 00 3A 3C BC 01 00 00 41 1F 00 00 00 00 00 00 

0B 00 00 00 - 11 instances
70 02 00 00
01 00 00 00
15 0E ED 01
00 00 41 1F 
01 00 
9B 92 07 00 - time since opened -> compare with prev log 
77 02 00 00 00 00 00 00 33 67 ED 01 00 00 41 1F 01 00 9B 92 07 00 53 02 00 00 01 00 00 00 81 4C C4 01 00 00 41 1F 00 00 00 00 00 00 67 02 00 00 00 00 00 00 32 48 BE 01 00 00 41 1F 00 00 00 00 00 00 8A 02 00 00 01 00 00 00 01 5C BE 01 00 00 41 1F 00 00 00 00 00 00 3F 02 00 00 01 00 00 00 A6 C3 C5 01 00 00 41 1F 00 00 00 00 00 00 89 02 00 00 03 00 00 00 71 A5 E8 01 00 00 41 1F 00 00 00 00 00 00 77 02 00 00 01 00 00 00 0A 36 E2 01 00 00 41 1F 00 00 00 00 00 00 5B 02 00 00 00 00 00 00 DC 47 C6 01 00 00 41 1F 00 00 00 00 00 00 89 02 00 00 02 00 00 00 1A D3 C6 01 00 00 41 1F 00 00 00 00 00 00 89 02 00 00 00 00 00 00 3A 3C BC 01 00 00 41 1F 00 00 00 00 00 00 

14333
{SERVER} Packet: (0x66E2) UNKNOWN PacketSize = 64 TimeStamp = 15027357
02 00 00 00 
9D 02 00 00 00 00 00 00 00 00 00 00 8C F9 4B 05 00 00 41 1F 01 00 6E 61 06 00 2C 00 00 00 
85 02 00 00 BlackRockSpire_4_0
01 00 00 00 difficulty
00 00 00 00 
38 57 4D 05 ID
00 00 41 1F maybe high guid of instance ?
01 00 started
6E 1B 01 00 
1F 00 00 00 

*/
	Instance * in;
	InstanceMap::iterator itr;
	InstanceMap * instancemap;
	uint32 i;
	uint32 counter = 0;

	data << counter;
	m_mapLock.Acquire();
	for(i = 0; i < NUM_MAPS; ++i)
	{
		if(m_instances[i] != NULL)
		{
			instancemap = m_instances[i];
			for(itr = instancemap->begin(); itr != instancemap->end();)
			{
				in = itr->second;
				++itr;

				if(in->m_persistent && PlayerOwnsInstance(in, plr))
				{
					data << in->m_mapId;
					if( in->instance_difficulty >= INSTANCE_MODE_RAID_10_MAN )
						data << (in->instance_difficulty - INSTANCE_MODE_RAID_10_MAN);	//raid difficulty if it is raid
					else
						data << in->instance_difficulty;	//dungeon difficulty if it is dungeon
					data << uint32( 0 );	//new in 420
					data << in->m_instanceId;
					data << uint32( 0 );							//seems to be a constant ? Maybe high guid of instance ?
					data << uint16( 1 );							//can reset ? ( 0 = no mobs killed, 1 = started instance and next number will say how much you played in it) 
					data << uint32( UNIXTIME - in->m_creation );	//time passsed since created( created as killed first mob)
					data << uint32( 0 );							//no idea 
					counter++;
				}
			}
		}
	}
	m_mapLock.Release();

	*(uint32*)&data.contents()[0] = counter;
	plr->GetSession()->SendPacket(&data);
}

void Instance::SaveToDB()
{
	// don't save non-raid instances.
	if( IS_MAP_DUNGEON( m_mapInfo ) || m_isBattleground )
		return;

	//avoid instance save spam
	if( instance_next_save_stamp > getMSTime() )
		return;

	instance_next_save_stamp = getMSTime() + sWorld.getIntRate(INTRATE_SAVE);

	std::stringstream ss;
	set<uint32>::iterator itr;

	ss << "REPLACE INTO `instances` (`id`, `mapid`, `creation`, `expiration`, `killed_npc_guids`, `difficulty`, `creator_group`, `creator_guid`, `persistent`) VALUES("
		<< m_instanceId << ","
		<< m_mapId << ","
		<< (uint32)m_creation << ","
		<< (uint32)m_expiration << ",'";

	for(itr = m_killedNpcs.begin(); itr != m_killedNpcs.end(); ++itr)
		ss << (*itr) << " ";

	ss << "',"
		<< instance_difficulty << ","
		<< m_creatorGroup << ","
		<< m_creatorGuid << ","
		<< m_persistent << ")";

	CharacterDatabase.Execute(ss.str().c_str());
}

void InstanceMgr::PlayerLeftGroup(Group * pGroup, Player * pPlayer)
{
	// does this group own any instances? we have to kick the player out of those instances.
	Instance * in;
	InstanceMap::iterator itr;
	InstanceMap * instancemap;
/*
14333
{SERVER} Packet: (0x22AE) SMSG_RAID_GROUP_ONLY PacketSize = 8 TimeStamp = 10379791
60 EA 00 00 
02 00 00 00 - maybe group type ?
*/
	WorldPacket data(SMSG_RAID_GROUP_ONLY, 8);
	uint32 i;

	m_mapLock.Acquire();
	for(i = 0; i < NUM_MAPS; ++i)
	{
		instancemap = m_instances[i];
		if(instancemap)
		{
			for(itr = instancemap->begin(); itr != instancemap->end();)
			{
				in = itr->second;
				++itr;

				if(in->m_creatorGroup && in->m_creatorGroup == pGroup->GetID())
				{
					// better make sure we're actually in that instance.. :P
					if(!pPlayer->raidgrouponlysent && pPlayer->GetInstanceID() == (int32)in->m_instanceId)
					{
						data << uint32(60000) << uint32(2);
						pPlayer->GetSession()->SendPacket(&data);
						pPlayer->raidgrouponlysent=true;
	
						sEventMgr.AddEvent(pPlayer, &Player::EjectFromInstance, EVENT_PLAYER_EJECT_FROM_INSTANCE, 60000, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);

						m_mapLock.Release();
						return;
					}
				}
			}
		}
	}
	m_mapLock.Release();
}

MapMgr * InstanceMgr::CreateBattlegroundInstance(uint32 mapid)
{
	// shouldn't happen
	if( mapid >= NUM_MAPS )
		return NULL;

	if(!m_maps[mapid])
	{
		_CreateMap(mapid);
		if(!m_maps[mapid])
			return NULL;
	}

	MapMgr * ret = new MapMgr(m_maps[mapid],mapid,GenerateInstanceID());
	//early exit should not happen
	if( ret == NULL )
		return NULL;
	Instance * pInstance = new Instance();
	if( pInstance == NULL )
	{
		delete ret;
		return NULL;
	}
	pInstance->m_creation = UNIXTIME;
	pInstance->m_creatorGroup = 0;
	pInstance->m_creatorGuid = 0;
	pInstance->instance_difficulty = 0;
	pInstance->m_expiration = 0;
	pInstance->m_instanceId = ret->GetInstanceID();
	pInstance->m_isBattleground = true;
	pInstance->m_persistent = false;
	pInstance->m_mapId = mapid;
	pInstance->m_mapInfo = WorldMapInfoStorage.LookupEntry( mapid );
	pInstance->m_mapMgr = ret;
	m_mapLock.Acquire();
	if( m_instances[mapid] == NULL )
		m_instances[mapid] = new InstanceMap;

	(*m_instances[mapid])[pInstance->m_instanceId]= pInstance;
	m_mapLock.Release();
	ThreadPool.ExecuteTask(ret);
	return ret;
}

void InstanceMgr::DeleteBattlegroundInstance(uint32 mapid, uint32 instanceid)
{
	m_mapLock.Acquire();
	InstanceMap::iterator itr = m_instances[mapid]->find( instanceid );
	if( itr == m_instances[mapid]->end() )
	{
		printf("Could not delete battleground instance %u - %u !\n",mapid,instanceid);
		m_mapLock.Release();
		return;
	}
	delete itr->second;
	itr->second = NULL;
	m_instances[mapid]->erase( itr );
	m_mapLock.Release();
}

void InstanceMgr::GetServerStats( float &AvgLatency, int &ActiveMapCount, int &ActiveCreatureCount, int &ActiveGamobjects, int &ActiveCorpses )
{
	float server_latency = 1;
	float server_latency_counter = 1;	//hmmm, sleeping maps will have insanely small latency and might hide bigger latencies
	ActiveMapCount = 0;
	ActiveCreatureCount = 0;
	ActiveGamobjects = 0;
	ActiveCorpses = 0;
	for(int i=0; i<NUM_MAPS; i++)
		if( m_singleMaps[i] && m_singleMaps[i]->last_updated != 0 )
		{
			float cur_latency = ((float)m_singleMaps[i]->timer_exec_time / (float)m_singleMaps[i]->timer_exec_count);
			if( cur_latency > 0.1f )
			{
				server_latency += cur_latency;
				server_latency_counter++;
				ActiveMapCount++;
				ActiveCreatureCount += (int)m_singleMaps[i]->activeCreatures.size();
				ActiveGamobjects += (int)m_singleMaps[i]->activeGameObjects.size();
				ActiveCorpses += (int)m_singleMaps[i]->m_corpses.size();
			}
		}
	m_mapLock.Acquire();
	for(int i=0; i<NUM_MAPS; i++)
		if(m_instances[i] != NULL)
		{
			for( InstanceMap::iterator itr = m_instances[i]->begin(); itr != m_instances[i]->end(); itr++ )
				if( itr->second->m_mapMgr && itr->second->m_mapMgr->last_updated != 0 )
				{
					MapMgr *mgr = itr->second->m_mapMgr;
					float cur_latency = ((float)mgr->timer_exec_time / (float)mgr->timer_exec_count);
					if( cur_latency > 0.1f )
					{
//						server_latency += cur_latency;
//						server_latency_counter++;
						ActiveMapCount++;
						ActiveCreatureCount += (int)mgr->activeCreatures.size();
						ActiveGamobjects += (int)mgr->activeGameObjects.size();
						ActiveCorpses += (int)mgr->m_corpses.size();
					}
				}
		}
	m_mapLock.Release();
	AvgLatency = server_latency / server_latency_counter;
}

FormationMgr::FormationMgr()
{
	QueryResult * res = WorldDatabase.Query("SELECT * FROM creature_formations");
	if(res)
	{
		Formation *f ;
		do 
		{
			f = new Formation;
			f->fol = res->Fetch()[1].GetUInt32();
			f->ang = res->Fetch()[2].GetFloat();
			f->dist = res->Fetch()[3].GetFloat();
			m_formations[res->Fetch()[0].GetUInt32()] = f;
		} while(res->NextRow());
		delete res;
		res = NULL;
	}
}

FormationMgr::~FormationMgr()
{
	FormationMap::iterator itr;
	for(itr = m_formations.begin(); itr != m_formations.end(); ++itr)
	{
		delete itr->second;
		itr->second = NULL;
	}
	m_formations.clear();
}
