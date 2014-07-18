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
// MapMgr.cpp
//

#include "StdAfx.h"
#define MAPMGR_INACTIVE_MOVE_TIME 10
extern bool bServerShutdown;

MapMgr::MapMgr(Map *map, uint32 mapId, uint32 instanceid) : CellHandler<MapCell>(map), _mapId(mapId), eventHolder(instanceid)
{
	_shutdown = false;
	m_instanceID = instanceid;
	pMapInfo = WorldMapInfoStorage.LookupEntry(mapId);
	m_UpdateDistance = pMapInfo->update_distance * pMapInfo->update_distance;
	instance_difficulty = 0;

	// Create script interface
	ScriptInterface = new MapScriptInterface(*this);

	// Set up storage arrays
	m_CreatureArraySize = map->CreatureSpawnCount + 5;
	m_GOArraySize = map->GameObjectSpawnCount + 5;

	//m_CreatureStorage = new Creature*[m_CreatureArraySize];
	m_CreatureStorage = (Creature**)malloc(sizeof(Creature*) * m_CreatureArraySize);
	memset(m_CreatureStorage,0,sizeof(Creature*)*m_CreatureArraySize);

	//m_GOStorage = new GameObject*[m_GOArraySize];
	m_GOStorage = (GameObject**)malloc(sizeof(GameObject*) * m_GOArraySize);
	memset(m_GOStorage,0,sizeof(GameObject*)*m_GOArraySize);

	m_DynamicObjectHighGuid = m_GOHighGuid = m_CreatureHighGuid = 1;

	lastUnitUpdate = getMSTime();
	lastGameobjectUpdate = getMSTime();
	m_battleground = 0;

	m_holder = &eventHolder;
	forced_expire = false;
	InactiveMoveTime = 0;
	mLoopCounter=0;
	pInstance = NULL;
	thread_kill_only = false;
	thread_running = false;

	m_forcedcells.clear();
	m_PlayerStorage.clear();
	m_PetStorage.clear();
	m_DynamicObjectStorage.clear();

	_combatProgress.clear();
	_mapWideStaticObjects.clear();
	_worldStateSet.clear();
	_updates.clear();
	_processQueue.clear();
	Sessions.clear();

	activeGameObjects.clear();
	activeCreatures.clear();
	m_corpses.clear();
	_sqlids_creatures.clear();
	_sqlids_gameobjects.clear();
#ifndef DISABLE_GUID_RECYCLING
	_reusable_guids_gameobject.clear();
	_reusable_guids_creature.clear();
#endif
	//see WorldStateUI.dbc for some values ?
	//these are so it will enable Arena charters in PVP client menu
	//if( mapId == 0 || mapId == 1 )
	{
		SetWorldState(3191,10); //arena season 10 ?
	}
	mInstanceScript = NULL;
	temp_buff.reserve( 65000 );
	activeCreaturesItr = activeCreatures.end();
	timer_exec_time = 50;
	timer_exec_count = 1;	//avoid division by 0 check, this is for the stats anyway
	last_updated = 0;
#ifdef USE_MULTIBOXING_DETECTOR
	MultiboxingDetector = new MultiBoxDetector;
#endif 
}


MapMgr::~MapMgr()
{
	_shutdown=true;
	sEventMgr.RemoveEvents(this);
	sEventMgr.RemoveEventHolder( m_instanceID );

	if ( mInstanceScript != NULL ) 
	{
		mInstanceScript->Destroy();  //deletes script !
		mInstanceScript = NULL;
	}

	if( ScriptInterface )
	{
		delete ScriptInterface;
		ScriptInterface = NULL;
	}
	
	// Remove objects
	if(_cells)
		for (uint32 i = 0; i < _sizeX; i++)
			if(_cells[i] != 0)
				for (uint32 j = 0; j < _sizeY; j++)
					if(_cells[i][j] != 0)
					{
						_cells[i][j]->_unloadpending=false;
						_cells[i][j]->RemoveObjects();
					}

	for(InRangeSet::iterator itr = _mapWideStaticObjects.begin(); itr != _mapWideStaticObjects.end(); ++itr)
	{
		if((*itr)->IsInWorld())
			(*itr)->RemoveFromWorld(false);
		delete (*itr);
//		(*itr) = NULL;
	}
	_mapWideStaticObjects.clear();


	if( m_GOStorage )
	{
		free(m_GOStorage);
		m_GOStorage = NULL;
	}
	if( m_CreatureStorage )
	{
		free(m_CreatureStorage);
		m_CreatureStorage = NULL;
	}

	Corpse * pCorpse;
	for(set<Corpse*>::iterator itr = m_corpses.begin(); itr != m_corpses.end();)
	{
		pCorpse = *itr;
		++itr;

		pCorpse->Despawn(); //remove from world + delete
	}
	m_corpses.clear();

	//Clear our remaining containers
	ObjectSet::iterator i;
	for(ObjectSet::iterator i2=m_objectinsertpool.begin();i2!=m_objectinsertpool.end();)
	{
		i = i2; //removefromworld changes list !
		i2++;
		if( (*i)->IsInWorld() )
		{
			//hehe you think an active map cannot be destroyed ? you thought well! though something goes wrong
			//fucking shit, you cannot be in insert pool and inworld too. If you are then it is a bug
			//if you are in this map insert pool then you should not exit other map
			//if you are in this map and also in insert pool then there is an issue again
			if( (*i)->IsInWorld() && (*i)->GetMapMgr() == this )
				(*i)->RemoveFromWorld( true );
		}
		//do not add deleted objects to map
		if( (*i)->deleted )
			continue;
		//do not add deleted objects to map
/*		if( (*i)->m_ClientFinishedLoading != CLIENT_FINISHED_LOADING )
		{
			m_objectinsertpool.insert( (*i) );
			continue;
		} */
	}
	m_objectinsertpool.clear();

	m_PlayerStorage.clear();
	m_PetStorage.clear();
	m_DynamicObjectStorage.clear();

	_combatProgress.clear();
	_updates.clear();
	_processQueue.clear();

	//kick sessions from map so they will not fuckup online player count
	for(SessionSet::iterator itr = Sessions.begin(); itr != Sessions.end(); itr++)
		(*itr)->SetInstance( WORLD_INSTANCE );
	Sessions.clear();

	activeGameObjects.clear();
	activeCreatures.clear();
	_sqlids_creatures.clear();
	_sqlids_gameobjects.clear();
#ifndef DISABLE_GUID_RECYCLING
	_reusable_guids_creature.clear();
	_reusable_guids_gameobject.clear();
#endif

	//this part should not make a difference. We already cleaned up things. If that is true then this whole part is skipped
	{
		// Clear the instance's reference to us.
		if(m_battleground)
		{
			BattlegroundManager.DeleteBattleground( m_battleground );
			sInstanceMgr.DeleteBattlegroundInstance( GetMapId(), GetInstanceID() );
			m_battleground = NULL;
		}

		if(pInstance)
		{
			// check for a non-raid instance, these expire after 10 minutes.
			if( IS_MAP_DUNGEON( GetMapInfo() ) || pInstance->m_isBattleground )
			{
				pInstance->m_mapMgr = NULL;
				sInstanceMgr._DeleteInstance(pInstance, true);
				pInstance = NULL; //DO NOT USE THIS pointer since it is deleted now
			}
			else
			{
				// just null out the pointer
				pInstance->m_mapMgr=NULL;
			}
		}
		else if( IS_STATIC_MAP( GetMapInfo() ) )
			sInstanceMgr.m_singleMaps[GetMapId()] = NULL;
		pInstance = NULL;
	}

	//avoid mem leaks
	ExtensionSet::iterator itr;
	for( itr=m_extensions.begin(); itr!=m_extensions.end(); itr++)
		if( itr->second )
			delete itr->second;
	m_extensions.clear();

#ifdef USE_MULTIBOXING_DETECTOR
	delete MultiboxingDetector;
	MultiboxingDetector = NULL;
#endif 
	Log.Notice("MapMgr", "Instance %u shut down. (%s)" , m_instanceID, GetBaseMap()->GetName());
}


void MapMgr::SendInitialWorldState(Player *plr, bool delayed )
{
	if( _worldStateSet.size()== 0 )
	{ 
		return;
	}
	WorldPacket data(SMSG_INIT_WORLD_STATES, _worldStateSet.size()*8 + 16 );
	data << GetMapId();
	data << (uint32)plr->GetZoneId(); // zone id?
	data << (uint32)plr->GetAreaID(); // area id?
	data << (uint16)_worldStateSet.size();
	std::map<uint32,uint32>::iterator itr = _worldStateSet.begin();
	for(; itr != _worldStateSet.end(); itr++)
	{
		data << (uint32)itr->first;
		data << (uint32)itr->second;
	}

	if( delayed == true )
		plr->CopyAndSendDelayedPacket( &data );
	else if( plr->GetSession() )
		plr->GetSession()->SendPacket( &data );
}

void MapMgr::SetWorldState(uint32 state, uint32 value)
{
	if(_worldStateSet.find(state) == _worldStateSet.end())
	{
		_worldStateSet.insert( make_pair(state, value) );
	}
	else
	{
		_worldStateSet[state] = value;
	}

	// Distribute this update
	
	SessionSet::iterator itr = Sessions.begin();
	for(; itr != Sessions.end(); itr++)
	{
		WorldSession * pSession = (*itr);
		if(!pSession)
			continue;

		sStackWorldPacket( data, SMSG_UPDATE_WORLD_STATE, 9 + 10 );
		data << state;
		data << value;
		data << uint8( 0 );
		pSession->SendPacket(&data);
	}
}

uint32 MapMgr::GetWorldState(uint32 state)
{
	if(_worldStateSet.find(state) == _worldStateSet.end())
	{ 
		return 0;
	}

	return _worldStateSet[state];
}

uint32 MapMgr::GetTeamPlayersCount(uint32 teamId)
{
	uint32 result = 0;
	PlayerStorageMap::iterator itr = m_PlayerStorage.begin();
	for(; itr != m_PlayerStorage.end(); itr++)
	{
		Player * pPlayer = (itr->second);
		if(pPlayer->GetTeam() == teamId)
			result++;
	}
	return result;
}


void MapMgr::PushObject(Object *obj)
{
	/////////////
	// Assertions
	/////////////
	ASSERT(obj);

	//do not add objects to a map that is getting shut down
	if( _shutdown == true )
	{ 
		return;
	}
	if( obj->deleted != OBJ_AVAILABLE )
		return;
	// That object types are not map objects. TODO: add AI groups here?
	if( obj->IsItem() )
	{
		// mark object as updatable and exit
		return;
	}

	if( obj->IsCorpse() )
	{
		m_corpses.insert(SafeCorpseCast(obj));
	}	
	
	obj->ClearInRangeSet();
	ASSERT(obj->GetMapId() == _mapId);
	//somehow player is out of max map boundaries
	if(!(obj->GetPositionX() < _maxX && obj->GetPositionX() > _minX) || 
	   !(obj->GetPositionY() < _maxY && obj->GetPositionY() > _minY))
	{
		if(obj->IsPlayer())
		{
			Player * plr = SafePlayerCast( obj );
			if(plr->GetBindMapId() != GetMapId())
			{
				plr->SafeTeleport(plr->GetBindMapId(),0,plr->GetBindPositionX(),plr->GetBindPositionY(),plr->GetBindPositionZ(),0);
				plr->GetSession()->SystemMessage("Teleported you to your hearthstone location as you were out of the map boundaries.");
				return;
			}
			else
			{
				obj->GetPositionV()->ChangeCoords(plr->GetBindPositionX(),plr->GetBindPositionY(),plr->GetBindPositionZ(),0);
				plr->GetSession()->SystemMessage("Teleported you to your hearthstone location as you were out of the map boundaries.");
				plr->BuildTeleportAckMsg(plr->GetPosition());
			}
		}
		else
		{
			obj->GetPositionV()->ChangeCoords(0,0,0,0); //teleport him to 0 0 0 ?
		}
	}

	ASSERT(obj->GetPositionY() < _maxY && obj->GetPositionY() > _minY);
	ASSERT(_cells);

	///////////////////////
	// Get cell coordinates
	///////////////////////

	uint32 x = GetPosX(obj->GetPositionX());
	uint32 y = GetPosY(obj->GetPositionY());

	if(x >= _sizeX || y >= _sizeY)
	{
		if(obj->IsPlayer())
		{
			Player * plr = SafePlayerCast( obj );
			if(plr->GetBindMapId() != GetMapId())
			{
				plr->SafeTeleport(plr->GetBindMapId(),0,plr->GetBindPositionX(),plr->GetBindPositionY(),plr->GetBindPositionZ(),0);
				plr->GetSession()->SystemMessage("Teleported you to your hearthstone location as you were out of the map boundaries.");
				return;
			}
			else
			{
				obj->GetPositionV()->ChangeCoords(plr->GetBindPositionX(),plr->GetBindPositionY(),plr->GetBindPositionZ(),0);
				plr->GetSession()->SystemMessage("Teleported you to your hearthstone location as you were out of the map boundaries.");
				plr->BuildTeleportAckMsg(plr->GetPosition());
			}
			x = GetPosX(obj->GetPositionX());
			y = GetPosY(obj->GetPositionY());
			//fucked up bindpoint after fucked up cord ?
			if(x >= _sizeX || y >= _sizeY)
			{
				plr->SafeTeleport(0,0,0,0,0,0); //oh yeah, the poor bastard
				return;
			}
		}
		else
		{
			obj->GetPositionV()->ChangeCoords(0,0,0,0);
			x = GetPosX(obj->GetPositionX());
			y = GetPosY(obj->GetPositionY());
		}
		if(x >= _sizeX || y >= _sizeY)
			return; //something is very wrong
	}

	//this object is used on some map
//	objmgr.AddActiveObject( obj );
	AddActiveObject( obj );
	if( obj->IsPlayer() ) 
	{
//		objmgr.AddActivePlayer( obj );
//		AddActivePlayer( obj );
	}

	MapCell *objCell = GetCell(x,y);
	if ( !objCell 
		|| objCell->GetPositionX() != x || objCell->GetPositionY() != y ) //wtf ? there is a cell but it's data are corrupted ? We leak memory here or object is already deleted ?
	{
		objCell = Create(x,y);
		objCell->Init(x, y, _mapId, this);
	}

	uint32 endX = (x < _sizeX - 1) ? x + 1 : (_sizeX-1);
	uint32 endY = (y < _sizeY - 1) ? y + 1 : (_sizeY-1);
	uint32 startX = x > 0 ? x - 1 : 0;
	uint32 startY = y > 0 ? y - 1 : 0;
	uint32 posX, posY;
	MapCell *cell;

	uint32 count;
	Player *plObj;

	if( obj->IsPlayer() )
		plObj = SafePlayerCast( obj );
	else
		plObj = NULL;

	//////////////////////
	// Build in-range data
	//////////////////////

	for (posX = startX; posX <= endX; posX++ )
		for (posY = startY; posY <= endY; posY++ )
		{
			cell = GetCell(posX, posY);
			if( !cell || cell->GetPositionX() != posX || cell->GetPositionY() != posY )
				continue; //wtf ? corrupted pointer or object not initialized yet ?
			UpdateInRangeSet(obj, plObj, cell);
		}

	//add ourself as last one so as soon as we enter world everything else is spawned
	if(plObj)
	{
		/* Add the map wide objects */
		if(_mapWideStaticObjects.size())
		{
			for(InRangeSet::iterator itr = _mapWideStaticObjects.begin(); itr != _mapWideStaticObjects.end(); ++itr)
			{
				temp_buff.clear();
				count = (*itr)->BuildCreateUpdateBlockForPlayer(&temp_buff, plObj);
				plObj->PushCreationData( &temp_buff, count, (*itr) );
			}
		}

		sLog.outDetail("Creating player "I64FMT" for himself.", obj->GetGUID());
		temp_buff.clear();
		count = plObj->BuildCreateUpdateBlockForPlayer(&temp_buff, plObj);
		plObj->PushCreationData( &temp_buff, count, plObj );
		plObj->ProcessPendingUpdates();
	}

	//Add to the cell's object list
	objCell->AddObject(obj);

	obj->SetMapCell(objCell);
	 //Add to the mapmanager's object list
	if(plObj)
	{
	   m_PlayerStorage[plObj->GetLowGUID()] = plObj;
	   UpdateCellActivity(x, y, 2);
	}
	else
	{
		switch(obj->GetTypeFromGUID())
		{
		case HIGHGUID_TYPE_PET:
			m_PetStorage[obj->GetUIdFromGUID()] = SafePetCast( obj );
			break;

		case HIGHGUID_TYPE_UNIT:
//		case HIGHGUID_TYPE_VEHICLE:
			{
#ifdef _DEBUG
				ASSERT( obj->IsCreature() );
#endif
				uint32 uid = obj->GetUIdFromGUID();	//avoid mem corruption due to scripts
				if( ExtendCreatureStorage( uid ) )
					return;
				if( uid <= m_CreatureHighGuid )
					m_CreatureStorage[ uid ] = SafeCreatureCast(obj);
				if(SafeCreatureCast(obj)->m_spawn != NULL)
				{
					_sqlids_creatures.insert(make_pair( SafeCreatureCast(obj)->m_spawn->id, SafeCreatureCast(obj) ) );
				}
			}break;

		case HIGHGUID_TYPE_GAMEOBJECT:
			{
				uint32 uid = obj->GetUIdFromGUID();	//avoid mem corruption due to scripts
				if( ExtendGOStorage( uid ) )
					return;
				if( uid < m_GOHighGuid )
					m_GOStorage[ uid ] = SafeGOCast(obj);
				if(SafeGOCast(obj)->m_spawn != NULL)
				{
					_sqlids_gameobjects.insert(make_pair( SafeGOCast(obj)->m_spawn->id, SafeGOCast(obj) ) );
				}
			}break;

		case HIGHGUID_TYPE_DYNAMICOBJECT:
			uint32 uid = obj->GetLowGUID();
			if( uid <= m_DynamicObjectHighGuid )
				m_DynamicObjectStorage[ uid ] = SafeDynOCast(obj);
			break;
		}
	}

	// Handle activation of that object.
	if(objCell->IsActive() && obj->CanActivate())
		obj->Activate(this);

	// Add the session to our set if it is a player.
	if(plObj)
	{
		Sessions.insert(plObj->GetSession());

		// Change the instance ID, this will cause it to be removed from the world thread (return value 1)
		plObj->GetSession()->SetInstance(GetInstanceID());
	}

	if(plObj && !forced_expire)
		InactiveMoveTime = 0;
}


void MapMgr::PushStaticObject(Object *obj, bool StaticOnly)
{
	_mapWideStaticObjects.insert(obj);
	obj->Activate( this );

	float VisibilityRange = 0.0f;
	if( StaticOnly == false )
	{
		switch(obj->GetTypeFromGUID())
		{
			case HIGHGUID_TYPE_UNIT:
	//		case HIGHGUID_TYPE_VEHICLE:
			{
				uint32 GUID = obj->GetUIdFromGUID();
				if( ExtendCreatureStorage( GUID ) )
					return;
				if( GUID < m_CreatureHighGuid )
					m_CreatureStorage[ GUID ] = SafeCreatureCast(obj);
				if( obj->IsUnit() && SafeUnitCast( obj )->GetAIInterface() )
					VisibilityRange = sqrt( (float)SafeUnitCast( obj )->GetAIInterface()->getOutOfCombatRange() );
			}break;

			case HIGHGUID_TYPE_GAMEOBJECT:
			{
				uint32 GUID = obj->GetUIdFromGUID();
				if( ExtendGOStorage( GUID ) )
					return;
				if( GUID < m_GOHighGuid )
					m_GOStorage[ GUID ] = SafeGOCast(obj);
			}break;

			default:
				// maybe add a warning, shouldnt be needed
				break;
		}
	}
	//let players see it
	for(hash_map<uint32, Player*>::iterator itr = m_PlayerStorage.begin(); itr != m_PlayerStorage.end(); ++itr)
	{
		temp_buff.clear();
		Player *plObj = itr->second;
		int32 count = obj->BuildCreateUpdateBlockForPlayer(&temp_buff, plObj);
		plObj->PushCreationData( &temp_buff, count, obj );
		if( obj->isInRange( plObj, VisibilityRange ) )
			obj->QueueAddInRangeObject( plObj );
		if( plObj->CanSee( obj ) )
			plObj->AddVisibleObject( obj );
	}
}

void MapMgr::RemoveStaticObject(Object *obj, bool StaticOnly)
{
	_mapWideStaticObjects.erase(obj);

	if( StaticOnly == false )
	{
		switch(obj->GetTypeFromGUID())
		{
			case HIGHGUID_TYPE_UNIT:
	//		case HIGHGUID_TYPE_VEHICLE:
				m_CreatureStorage[obj->GetUIdFromGUID()] = NULL;
				break;

			case HIGHGUID_TYPE_GAMEOBJECT:
				m_GOStorage[obj->GetUIdFromGUID()] = NULL;
				break;

			default:
				// maybe add a warning, shouldnt be needed
				break;
		}
	}
}

#define OPTIONAL_IN_RANGE_SETS

void MapMgr::RemoveObject(Object *obj, bool free_guid)
{
	/////////////
	// Assertions
	/////////////

	ASSERT(obj);
	ASSERT(obj->GetMapId() == _mapId);
	//ASSERT(obj->GetPositionX() > _minX && obj->GetPositionX() < _maxX);
	//ASSERT(obj->GetPositionY() > _minY && obj->GetPositionY() < _maxY);
	ASSERT(_cells);

	if(obj->Active)
		obj->Deactivate(this);

	_mapWideStaticObjects.erase( obj );

	//this object is used on some map
//	objmgr.RemActiveObject( obj );
	RemActiveObject( obj );
	if( obj->IsPlayer() ) 
	{
//		objmgr.RemActivePlayer( obj );
//		RemActivePlayer( obj );
	}

	//there is a very small chance that on double player ports on same update player is added to multiple insertpools but not removed
	//one clear example was the double port proc when exploiting double resurect 
	//!!! Zack : this could lead to list corruption in case object is removed while added to the map
	// we will empty this list after processing the content of it so there is no need for it
//	m_objectinsertlock.Acquire();
//	m_objectinsertpool.erase( obj );
//	m_objectinsertlock.Release();

	bool weird_object_status_deleted = (obj->deleted==1);	//debugging
//	_updates.remove( obj );
	_updates.erase( obj );
	if( weird_object_status_deleted == false )
		obj->ClearUpdateMask();
	Player* plObj = (obj->GetTypeId() == TYPEID_PLAYER) ? SafePlayerCast( obj ) : 0;

	///////////////////////////////////////
	// Remove object from all needed places
	///////////////////////////////////////
 
	switch(obj->GetTypeFromGUID())
	{
		case HIGHGUID_TYPE_UNIT:
//		case HIGHGUID_TYPE_VEHICLE:
		{
			uint32 GUID = obj->GetUIdFromGUID();
			ASSERT( GUID <= m_CreatureHighGuid );
			if( ExtendCreatureStorage( GUID ) )
				return;
			m_CreatureStorage[ GUID ] = 0;
			if(SafeCreatureCast(obj)->m_spawn != NULL)
			{
				_sqlids_creatures.erase(SafeCreatureCast(obj)->m_spawn->id);
			}

#ifndef DISABLE_GUID_RECYCLING
			if(free_guid)
				_reusable_guids_creature.push_back(obj->GetUIdFromGUID());
#endif
		}break;

		case HIGHGUID_TYPE_PET:
			m_PetStorage.erase(obj->GetUIdFromGUID());
			break;

		case HIGHGUID_TYPE_DYNAMICOBJECT:
			m_DynamicObjectStorage.erase(obj->GetLowGUID());
			break;

		case HIGHGUID_TYPE_GAMEOBJECT:
		{
			uint32 GUID = obj->GetUIdFromGUID();
			ASSERT( GUID <= m_GOHighGuid );
			if( ExtendGOStorage( GUID ) )
				return;
			m_GOStorage[ GUID ] = NULL;
			if(SafeGOCast(obj)->m_spawn != NULL)
			{
				_sqlids_gameobjects.erase(SafeGOCast(obj)->m_spawn->id);
			}

#ifndef DISABLE_GUID_RECYCLING
			if(free_guid)
				_reusable_guids_gameobject.push_back(obj->GetUIdFromGUID());
#endif

		}break;
	}

	// That object types are not map objects. TODO: add AI groups here?
	if( obj->IsItem() 
		|| obj->GetTypeId() == TYPEID_UNUSED	//wtf hacky crashfix ?
		)
	{
		_processQueue.erase( SafePlayerCast( obj ) );
		return;
	}

	if( obj->IsCorpse() )
	{
		m_corpses.erase( SafeCorpseCast( obj ) );
	}

	if(!obj->GetMapCell())
	{
		/* set the map cell correctly */
		if(obj->GetPositionX() >= _maxX || obj->GetPositionX() <= _minY ||
			obj->GetPositionY() >= _maxY || obj->GetPositionY() <= _minY)
		{
			// do nothing
		}
		else
		{
			obj->SetMapCell(this->GetCellByCoords(obj->GetPositionX(), obj->GetPositionY()));
		}		
	}

	MapCell *curcell = obj->GetMapCell();
	if(curcell)
	{
		ASSERT( curcell );
	
		// Remove object from cell
		curcell->RemoveObject( obj );
	
		// Unset object's cell
		if( weird_object_status_deleted == false )
			obj->SetMapCell(NULL);
	}

	// Clear any updates pending
	if( obj->IsPlayer() )
	{
		_processQueue.erase( SafePlayerCast( obj ) );
		if( weird_object_status_deleted == false )
			SafePlayerCast( obj )->ClearAllPendingUpdates();
	}
	
	// Remove object from all objects 'seeing' him
	//there is a small chance for deadlock here. On the a situation i never understood how it can happen and corrupt the list too.
//	if( weird_object_status_deleted == false )
	if( !_shutdown ) //if it is shutdown then in theory there are no players on it. Do creatures really need to get their queue filled up before "delete" ?
	{
		obj->InrageConsistencyCheck( true );
		obj->AquireInrangeLock(); //make sure to release lock before exit function !
		if( obj->HasInRangeObjects() == true )
		{
			InRangeSetRecProt::iterator iter;
//			InRangeSet tset( obj->m_objectsInRange ); //some idiot concurency issue when player somehow gets deleted while logging out
			// Clear object's in-range set
//			obj->ClearInRangeSet();
//			for (InRangeSet::iterator iter2 = tset.begin();	iter2 != tset.end(); )
			InrangeLoopExitAutoCallback AutoLock;
			for (InRangeSetRecProt::iterator iter2 = obj->GetInRangeSetBegin( AutoLock ); iter2 != obj->GetInRangeSetEnd(); )
			{
				iter = iter2;
				iter2++;
				if( (*iter) 
					&& (*iter)->deleted == false 
					&& (*iter)->GetTypeId() < TYPEID_UNUSED
					)
				{
					if( (*iter)->IsPlayer() )
					{
						if( SafePlayerCast( *iter )->IsVisible( obj ) 
							&& SafePlayerCast( *iter )->m_TransporterGUID != obj->GetGUID() 
							&& weird_object_status_deleted == false
							)
							SafePlayerCast( *iter )->PushOutOfRange( obj->GetNewGUID() );
					}
					(*iter)->QueueRemoveInRangeObject( obj );
				}
			}
			// Clear object's in-range set
			obj->ClearInRangeSet();
		}
		obj->ReleaseInrangeLock();
	}


	// If it's a player - update his nearby cells
	if( !_shutdown && obj->IsPlayer() )
	{
		// get x/y
		if(obj->GetPositionX() >= _maxX || obj->GetPositionX() <= _minY ||
			obj->GetPositionY() >= _maxY || obj->GetPositionY() <= _minY)
		{
			// do nothing
		}
		else
		{
			uint32 x = GetPosX(obj->GetPositionX());
			uint32 y = GetPosY(obj->GetPositionY());
			UpdateCellActivity(x, y, 2);
		}
		m_PlayerStorage.erase( obj->GetLowGUID() );
	}

	if( weird_object_status_deleted == false )
		for(InRangeSet::iterator itr = _mapWideStaticObjects.begin(); itr != _mapWideStaticObjects.end(); ++itr)
		{
			(*itr)->QueueRemoveInRangeObject( obj );
			if( plObj )
				plObj->PushOutOfRange((*itr)->GetNewGUID());
		}
	// Remove the session from our set if it is a player.
	if(plObj)
	{
		// Setting an instance ID here will trigger the session to be removed
		// by MapMgr::run(). :)
		if( plObj->GetSession() )
		{
			plObj->GetSession()->SetInstance( WORLD_INSTANCE );

			// Add it to the global session set.
			// Don't "re-add" to session if it is being deleted.
			if(!plObj->GetSession()->bDeleted)
				sWorld.AddGlobalSession(plObj->GetSession());
		}
	}

	if(!HasPlayers())
	{
		if(this->pInstance != NULL && this->pInstance->m_persistent)
			this->pInstance->m_creatorGroup = 0;
		if(!InactiveMoveTime && !forced_expire && IS_MAP_INSTANCE( GetMapInfo() ) )
		{
			InactiveMoveTime = UNIXTIME + (MAPMGR_INACTIVE_MOVE_TIME * 60);	 
			Log.Debug("MapMgr", "Instance %u is now idle. (%s)", m_instanceID, GetBaseMap()->GetName());
		}
	}
}

void MapMgr::ChangeObjectLocation( Object *obj )
{
	if ( !obj ) return; // crashfix

	// Items and containers are of no interest for us
	if( obj->IsItem() || obj->GetMapMgr() != this )
	{ 
		return;
	}

	Player* plObj;

	if( obj->IsPlayer() )
		plObj = SafePlayerCast( obj );
	else
		plObj = NULL;

	Object* curObj;
	float fRange;

	///////////////////////////////////////
	// Update in-range data for old objects
	///////////////////////////////////////

	/** let's duplicate some code here :P Less branching is always good.
	 * - Burlex
	 */
/*#define IN_RANGE_LOOP \
	for (InRangeSet::iterator iter = obj->GetInRangeSetBegin(), iter2; \
		iter != obj->GetInRangeSetEnd();) \
	{ \
		curObj = *iter; \
		iter2 = iter; \
		++iter; \
		if(curObj->IsPlayer() && obj->IsPlayer() && plObj->m_TransporterGUID && plObj->m_TransporterGUID == SafePlayerCast( curObj )->m_TransporterGUID ) \
			fRange = 0.0f;		\
		else if((curObj->GetGUIDHigh() == HIGHGUID_TRANSPORTER || obj->GetGUIDHigh() == HIGHGUID_TRANSPORTER)) \
			fRange = 0.0f;		\
		else if((curObj->GetGUIDHigh() == HIGHGUID_GAMEOBJECT && curObj->GetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_TYPEID) == GAMEOBJECT_TYPE_TRANSPORT || obj->GetGUIDHigh() == HIGHGUID_GAMEOBJECT && obj->GetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_TYPEID) == GAMEOBJECT_TYPE_TRANSPORT)) \
			fRange = 0.0f;		\
		else \
			fRange = m_UpdateDistance;	\
		if (curObj->GetDistance2dSq(obj) > fRange && fRange > 0) \

#define END_IN_RANGE_LOOP } \

	if(plObj)
	{
		IN_RANGE_LOOP
		{
			plObj->RemoveIfVisible(curObj);
			plObj->RemoveInRangeObject(iter2);

			if(curObj->NeedsInRangeSet())
				curObj->RemoveInRangeObject(obj);

			if(curObj->IsPlayer())
				SafePlayerCast( curObj )->RemoveIfVisible(obj);
		}
		END_IN_RANGE_LOOP
	}
	else if(obj->NeedsInRangeSet())
	{
		IN_RANGE_LOOP
		{
			if(curObj->NeedsInRangeSet())
				curObj->RemoveInRangeObject(obj);

			if(curObj->IsPlayer())
				SafePlayerCast( curObj )->RemoveIfVisible(obj);

			obj->RemoveInRangeObject(iter2);
		}
		END_IN_RANGE_LOOP
	}
	else
	{
		IN_RANGE_LOOP
		{
			if(curObj->NeedsInRangeSet())
				curObj->RemoveInRangeObject(obj);

			if(curObj->IsPlayer())
			{
				SafePlayerCast( curObj )->RemoveIfVisible(obj);
				obj->RemoveInRangePlayer(curObj);
			}
		}
		END_IN_RANGE_LOOP
	}

#undef IN_RANGE_LOOP
#undef END_IN_RANGE_LOOP*/

	if(obj->HasInRangeObjects()) 
	{
		obj->AquireInrangeLock(); //make sure to release lock before exit function !
		InrangeLoopExitAutoCallback AutoLock;
		for (InRangeSetRecProt::iterator iter = obj->GetInRangeSetBegin( AutoLock ), iter2; iter != obj->GetInRangeSetEnd();)
		{
			curObj = *iter;
			iter2 = iter++;
			if( curObj->IsPlayer() && obj->IsPlayer() && plObj->m_TransporterGUID && plObj->m_TransporterGUID == SafePlayerCast( curObj )->m_TransporterGUID )
				fRange = 0.0f; // unlimited distance for people on same boat
			else if( curObj->GetTypeFromGUID() == HIGHGUID_TYPE_TRANSPORTER )
				fRange = 0.0f; // unlimited distance for transporters (only up to 2 cells +/- anyway.)
			else if( curObj->IsCorpse() && curObj->GetUInt32Value( CORPSE_FIELD_OWNER ) == obj->GetLowGUID() )
				fRange = 0.0f; // need to see this all the time
			else if( curObj->static_object )
			{
				if( curObj->IsUnit() && SafeUnitCast( curObj )->GetAIInterface() )
					fRange = ( (float)SafeUnitCast( curObj )->GetAIInterface()->getOutOfCombatRange() );
				else
					fRange = 0.0f; //for scripts...
			}
			else
				fRange = m_UpdateDistance; // normal distance

			if( fRange > 0.0f && curObj->GetDistanceSq(obj) > fRange )
			{
				if( plObj )
					plObj->RemoveIfVisible(curObj);

				if( curObj->IsPlayer() )
					SafePlayerCast( curObj )->RemoveIfVisible(obj);

				curObj->QueueRemoveInRangeObject( obj );

				//we got kicked out of this map. we should try to clean our visibility of objects not on same map with us
				if( obj->GetMapMgr() != this )
				{
					obj->ReleaseInrangeLock();
					return;
				} 
				obj->QueueRemoveInRangeObject( curObj );
			}
		}
		obj->ReleaseInrangeLock();
	}

	///////////////////////////
	// Get new cell coordinates
	///////////////////////////
	if(obj->GetMapMgr() != this)
	{
		/* Something removed us. */
		return;
	}

	if(obj->GetPositionX() >= _maxX || obj->GetPositionX() <= _minX ||
		obj->GetPositionY() >= _maxY || obj->GetPositionY() <= _minY)
	{
		if(obj->IsPlayer())
		{
			Player* plr = SafePlayerCast( obj );
			if(plr->GetBindMapId() != GetMapId())
			{
				plr->SafeTeleport(plr->GetBindMapId(),0,plr->GetBindPositionX(),plr->GetBindPositionY(),plr->GetBindPositionZ(),0);
				plr->GetSession()->SystemMessage("Teleported you to your hearthstone location as you were out of the map boundaries.");
				return;
			}
			else
			{
				obj->GetPositionV()->ChangeCoords(plr->GetBindPositionX(),plr->GetBindPositionY(),plr->GetBindPositionZ(),0);
				plr->GetSession()->SystemMessage("Teleported you to your hearthstone location as you were out of the map boundaries.");
				plr->BuildTeleportAckMsg(plr->GetPosition());
			}
		}
		else
		{
			obj->GetPositionV()->ChangeCoords(0,0,0,0);
		}
	}

	uint32 cellX = GetPosX(obj->GetPositionX());
	uint32 cellY = GetPosY(obj->GetPositionY());

	if(cellX >= _sizeX || cellY >= _sizeY)
	{
		return;
	}

	MapCell *objCell = GetCell(cellX, cellY);
	MapCell * pOldCell = obj->GetMapCell();
	if (!objCell)
	{
		objCell = Create(cellX,cellY);
		objCell->Init(cellX, cellY, _mapId, this);
	}

	// If object moved cell
	if ( objCell != pOldCell )
	{
		// THIS IS A HACK!
		// Current code, if a creature on a long waypoint path moves from an active
		// cell into an inactive one, it will disable itself and will never return.
		// This is to prevent cpu leaks. I will think of a better solution very soon :P

		if(!objCell->IsActive() && !plObj && obj->Active)
			obj->Deactivate(this);

		if( pOldCell )
			pOldCell->RemoveObject(obj);
	
		objCell->AddObject(obj);
		obj->SetMapCell(objCell);

		// if player we need to update cell activity
		// radius = 2 is used in order to update both
		// old and new cells
		if(obj->GetTypeId() == TYPEID_PLAYER)
		{
			// have to unlock/lock here to avoid a deadlock situation.
			UpdateCellActivity(cellX, cellY, 2);
			if( pOldCell != NULL )
				// only do the second check if theres -/+ 2 difference
				if( abs( (int)cellX - (int)pOldCell->_x ) > 2 || abs( (int)cellY - (int)pOldCell->_y ) > 2 ) 
					UpdateCellActivity( pOldCell->_x, pOldCell->_y, 2 );
		}
	}


	//////////////////////////////////////
	// Update in-range set for new objects
	//////////////////////////////////////

	uint32 endX = cellX < ( _sizeX - 1) ? cellX + 1 : (_sizeX-1);
	uint32 endY = cellY < ( _sizeY - 1) ? cellY + 1 : (_sizeY-1);
	uint32 startX = cellX > 0 ? cellX - 1 : 0;
	uint32 startY = cellY > 0 ? cellY - 1 : 0;
	uint32 posX, posY;
	MapCell *cell;

	for (posX = startX; posX <= endX; ++posX )
		for (posY = startY; posY <= endY; ++posY )
		{
			cell = GetCell(posX, posY);
			if ( !cell || cell->GetPositionX() != posX || cell->GetPositionY() != posY )
				continue; //wtf ? cell is not yet inited or invalid pointer
			UpdateInRangeSet(obj, plObj, cell );
		}
}

void MapMgr::UpdateInRangeSet( Object *obj, Player *plObj, MapCell* cell )
{
	if( cell == NULL )
	{ 
		return;
	}

	Object* curObj;
	Player* plObj2;
	int count;
	ObjectSet::iterator iter = cell->Begin();
	float fRange;
	bool cansee, isvisible;

	//had to add this as some GM managed to spawn an event with 15k objects in 1 cell....
	uint32 MaxObjectsToAdd = cell->ObjectCountToReserveOnEnter;
	obj->ReserveObjectListSize( MaxObjectsToAdd );

	while( iter != cell->End() )
	{
		curObj = *iter;
		++iter;

		if( curObj == NULL )
			continue;

		if( curObj->IsPlayer() && obj->IsPlayer() && plObj->m_TransporterGUID && plObj->m_TransporterGUID == SafePlayerCast( curObj )->m_TransporterGUID )
			fRange = 0.0f; // unlimited distance for people on same boat
		else if( curObj->GetTypeFromGUID() == HIGHGUID_TYPE_TRANSPORTER )
			fRange = 0.0f; // unlimited distance for transporters (only up to 2 cells +/- anyway.)
		else if( curObj->static_object )
		{
			if( curObj->IsUnit() && SafeUnitCast( curObj )->GetAIInterface() )
				fRange = ( (float)SafeUnitCast( curObj )->GetAIInterface()->getOutOfCombatRange() );
			else
				fRange = 0.0f; //for scripts...
		}
		else
			fRange = m_UpdateDistance; // normal distance
			

		if ( curObj != obj 
			&& ( fRange == 0.0f || curObj->GetDistanceSq( obj ) <= fRange )
			&& ( curObj->m_phase & obj->m_phase )
			)
		{
			if( !obj->IsInRangeSet( curObj ) )
			{
				// Object in range, add to set
				obj->QueueAddInRangeObject( curObj );
				curObj->QueueAddInRangeObject( obj );

				if( curObj->IsPlayer() )
				{
					plObj2 = SafePlayerCast( curObj );

					if( plObj2->CanSee( obj ) && !plObj2->IsVisible( obj ) )
					{
						temp_buff.clear();
						count = obj->BuildCreateUpdateBlockForPlayer(&temp_buff, plObj2);
						plObj2->PushCreationData( &temp_buff, count, obj );
						plObj2->AddVisibleObject(obj);
						if( obj->IsUnit() && obj != plObj2 )
//							SafeUnitCast( obj )->SendAurasToPlayer( plObj2 );
							sEventMgr.AddEvent(SafeUnitCast( obj ),&Unit::SendAllAurasToPlayer,plObj2->GetLowGUID(),EVENT_SEND_AURAS_TO_PLAYER,AURA_SEND_DELAY,1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT); 
					}
				}

				if( plObj != NULL )
				{
					if( plObj->CanSee( curObj ) && !plObj->IsVisible( curObj ) )
					{
						temp_buff.clear();
						count = curObj->BuildCreateUpdateBlockForPlayer( &temp_buff, plObj );
						plObj->PushCreationData( &temp_buff, count, curObj );
						plObj->AddVisibleObject( curObj );
						if( curObj->IsUnit() && curObj != plObj )
//							SafeUnitCast( curObj )->SendAurasToPlayer( plObj );
							sEventMgr.AddEvent(SafeUnitCast( curObj ),&Unit::SendAllAurasToPlayer,plObj->GetLowGUID(),EVENT_SEND_AURAS_TO_PLAYER,AURA_SEND_DELAY,1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
					}
				}
			}
			//we are in visibility box but not in visibility circle
			else
			{
				// Check visiblility
				if( curObj->IsPlayer() )
				{
					plObj2 = SafePlayerCast( curObj );
					cansee = plObj2->CanSee(obj);
					isvisible = plObj2->GetVisibility(obj);
					if(!cansee && isvisible)
					{
						plObj2->PushOutOfRange(obj->GetNewGUID());
						plObj2->RemoveVisibleObject(obj);
					}
					else if(cansee && !isvisible)
					{
						temp_buff.clear();
						count = obj->BuildCreateUpdateBlockForPlayer(&temp_buff, plObj2);
						plObj2->PushCreationData( &temp_buff, count, obj );
						plObj2->AddVisibleObject(obj);
					}
				}

				if( plObj )
				{
					cansee = plObj->CanSee( curObj );
					isvisible = plObj->GetVisibility( curObj );
					if(!cansee && isvisible)
					{
						plObj->PushOutOfRange( curObj->GetNewGUID() );
						plObj->RemoveVisibleObject( curObj );
					}
					else if(cansee && !isvisible)
					{
						temp_buff.clear();
						count = curObj->BuildCreateUpdateBlockForPlayer( &temp_buff, plObj );
						plObj->PushCreationData( &temp_buff, count, curObj );
						plObj->AddVisibleObject( curObj );
					}
				}
			}
		}
	}
/*
#define IN_RANGE_LOOP_P1 \
	while(iter != cell->End()) \
	{ \
		curObj = *iter; \
		++iter; \
		if(curObj->IsPlayer() && obj->IsPlayer() && plObj && plObj->m_TransporterGUID && plObj->m_TransporterGUID == SafePlayerCast( curObj )->m_TransporterGUID) \
			fRange = 0.0f; \
		else if((curObj->GetGUIDHigh() == HIGHGUID_TRANSPORTER ||obj->GetGUIDHigh() == HIGHGUID_TRANSPORTER)) \
			fRange = 0.0f; \
		else if((curObj->GetGUIDHigh() == HIGHGUID_GAMEOBJECT && curObj->GetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_TYPEID) == GAMEOBJECT_TYPE_TRANSPORT || obj->GetGUIDHigh() == HIGHGUID_GAMEOBJECT && obj->GetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_TYPEID) == GAMEOBJECT_TYPE_TRANSPORT)) \
			fRange = 0.0f; \
		else \
			fRange = m_UpdateDistance; \
		if ( curObj != obj && (fRange == 0.0f || curObj->GetDistance2dSq(obj) < fRange ) ) \
		{ \
			if(!obj->IsInRangeSet(curObj)) \
			{ \
				if(curObj->NeedsInRangeSet()) \
				{ \
					curObj->AddInRangeObject(obj); \
				} else if(obj->IsPlayer()) \
				{ \
					curObj->AddInRangePlayer(obj); \
				} \
				if(curObj->IsPlayer()) \
				{ \
					plObj2 = SafePlayerCast( curObj ); \
					if (plObj2->CanSee(obj) && !plObj2->IsVisible(obj))  \
					{ \
						CHECK_BUF; \
						count = obj->BuildCreateUpdateBlockForPlayer(*buf, plObj2); \
						plObj2->PushCreationData(*buf, count); \
						plObj2->AddVisibleObject(obj); \
						(*buf)->clear(); \
					} \
				} 

#define IN_RANGE_LOOP_P2 \
			} \
			else \
			{ \
				if(curObj->IsPlayer()) \
				{ \
					plObj2 = SafePlayerCast( curObj ); \
					cansee = plObj2->CanSee(obj); \
					isvisible = plObj2->GetVisibility(obj, &itr); \
					if(!cansee && isvisible) \
					{ \
						plObj2->RemoveVisibleObject(itr); \
						plObj2->PushOutOfRange(obj->GetNewGUID()); \
					} \
					else if(cansee && !isvisible) \
					{ \
						CHECK_BUF; \
						count = obj->BuildCreateUpdateBlockForPlayer(*buf, plObj2); \
						plObj2->PushCreationData(*buf, count); \
						plObj2->AddVisibleObject(obj); \
						(*buf)->clear(); \
					} \
				} \

#define IN_RANGE_LOOP_P3 \
			} \
		} \
	} \


	if(plObj)
	{
		IN_RANGE_LOOP_P1

			obj->AddInRangeObject(curObj);
			if(plObj->CanSee(curObj) && !plObj->IsVisible(curObj))
			{
				CHECK_BUF;
				count = curObj->BuildCreateUpdateBlockForPlayer(*buf, plObj);
				plObj->PushCreationData(*buf, count);
				plObj->AddVisibleObject(curObj);
				(*buf)->clear();
			}

		IN_RANGE_LOOP_P2

			if(plObj)
			{
				cansee = plObj->CanSee(curObj);
				isvisible = plObj->GetVisibility(curObj, &itr);
				if(!cansee && isvisible)
				{
					plObj->PushOutOfRange(curObj->GetNewGUID());
					plObj->RemoveVisibleObject(itr);
				}
				else if(cansee && !isvisible)
				{
					CHECK_BUF;
					count = curObj->BuildCreateUpdateBlockForPlayer(*buf, plObj);
					plObj->PushCreationData(*buf, count);
					plObj->AddVisibleObject(curObj);
					(*buf)->clear();
				}
			}

		IN_RANGE_LOOP_P3
	} else if(obj->NeedsInRangeSet())
	{
		IN_RANGE_LOOP_P1
			obj->AddInRangeObject(curObj);
		IN_RANGE_LOOP_P2
		IN_RANGE_LOOP_P3
	}
	else
	{
		IN_RANGE_LOOP_P1
			if(curObj->IsPlayer())
				obj->AddInRangePlayer(obj);

		IN_RANGE_LOOP_P2
		IN_RANGE_LOOP_P3
	}

#undef IN_RANGE_LOOP_P1
#undef IN_RANGE_LOOP_P2
#undef IN_RANGE_LOOP_P3*/
}

void MapMgr::_UpdateObjects()
{
	if( _updates.empty() == true && _processQueue.empty() == true )
	{ 
		return;
	}

	Object *pObj;
	Player *pOwner;
	//InRangeSet::iterator it_start, it_end, itr;
	InRangePlayerSetRecProt::iterator it_start, it_end, itr;
	Player * lplr;
	uint32 count = 0;
	
	m_updateMutex.Acquire();
	UpdateQueueSet::iterator iter = _updates.begin();
	PUpdateQueue::iterator it, eit;

	for(; iter != _updates.end();)
	{
		pObj = *iter;
		++iter;

		//somehow bad pointers get into this list. Just debugging for now. If it will still crash then the issue is after this function
		if( pObj->QueryObjectIntegrityIsBad() == true )
			continue;

		if( 
//			!pObj || 
//			pObj->m_ClientFinishedLoading != CLIENT_FINISHED_LOADING ||
			pObj->IsInWorld() == false ) 
		{
			pObj->m_objectUpdated = false; //we are delaying the update
			continue;
		}

		if( pObj->IsItem() )
		{
			// our update is only sent to the owner here.
			pOwner = SafeItemCast(pObj)->GetOwner();
			if( pOwner != NULL && pOwner->deleted == 0 && pOwner->IsInWorld() == true && pOwner->GetMapMgr() == pObj->GetMapMgr() ) //something gets really fucked up with some items loosing their owner or something
			{
				temp_buff.clear();
				count = SafeItemCast( pObj )->BuildValuesUpdateBlockForPlayer( &temp_buff, pOwner );
				// send update to owner
				if( count )
					pOwner->PushUpdateData( &temp_buff, count );
			}
		}
		else
		{
			// players have to receive their own updates ;)
			if( pObj->IsPlayer() )
			{
				// need to be different! ;)
				temp_buff.clear();
				count = pObj->BuildValuesUpdateBlockForPlayer( &temp_buff, SafePlayerCast( pObj ) );
				if( count )
					SafePlayerCast( pObj )->PushUpdateData( &temp_buff, count );
			}

			// build the update
			if( pObj->HasInRangePlayers() )
			{
				temp_buff.clear();
				count = pObj->BuildValuesUpdateBlockForPlayer( &temp_buff, (Player*)NULL );

				if( count )
				{
					InrangeLoopExitAutoCallback AutoLock;
					it_start = pObj->GetInRangePlayerSetBegin( AutoLock );
					it_end = pObj->GetInRangePlayerSetEnd();
					for(itr = it_start; itr != it_end;)
					{
						lplr = *itr;
						++itr;
						// Make sure that the target player can see us.
						if( lplr->IsPlayer() 
							&& lplr->IsVisible( pObj ) 
//							&& lplr->m_ClientFinishedLoading == true 
							)
							lplr->PushUpdateData( &temp_buff, count );
					}
				}
			}
		}
		pObj->ClearUpdateMask();
	}
	_updates.clear();
	m_updateMutex.Release();
	
	// generate pending a9packets and send to clients.
	Player *plyr;
	for(it = _processQueue.begin(); it != _processQueue.end();)
	{
		plyr = *it;
		eit = it;
		++it;
//		_processQueue.erase(eit);
		if(plyr->GetMapMgr() == this)
			plyr->ProcessPendingUpdates();
	}
	_processQueue.clear();
}
void MapMgr::LoadAllCells()
{
	// eek
	MapCell * cellInfo;
	CellSpawnsVect * spawns;

	for( uint32 x = 0 ; x < _sizeX ; x ++ )
	{
		for( uint32 y = 0 ; y < _sizeY ; y ++ )
		{
			cellInfo = GetCell( x , y );
			
			if( !cellInfo )
			{
				// Cell doesn't exist, create it.
				// There is no spoon. Err... cell.
				cellInfo = Create( x , y );
				cellInfo->Init( x , y , _mapId , this );
				sLog.outDetail( "Created cell [%u,%u] on map %d (instance %d)." , x , y , _mapId , m_instanceID );
				cellInfo->SetActivity( true );
//				_map->CellGoneActive( x , y );
				ASSERT( !cellInfo->IsLoaded() );

				spawns = _map->GetSpawnsList( x , y );
				if( spawns )
					cellInfo->LoadObjects( spawns );
			}
			else
			{
				// Cell exists, but is inactive
				if ( !cellInfo->IsActive() )
				{
					sLog.outDetail("Activated cell [%u,%u] on map %d (instance %d).", x, y, _mapId, m_instanceID );
//					_map->CellGoneActive( x , y );
					cellInfo->SetActivity( true );

					if (!cellInfo->IsLoaded())
					{
						//sLog.outDetail("Loading objects for Cell [%d][%d] on map %d (instance %d)...", 
						//	posX, posY, this->_mapId, m_instanceID);
						spawns = _map->GetSpawnsList( x , y );
						if( spawns )
							cellInfo->LoadObjects( spawns );
					}
				}
			}
		}
	}
}

void MapMgr::UpdateCellActivity(int32 x, int32 y, int32 radius, int8 ForceCreateStatic)
{
	CellSpawnsVect * sp;
	uint32 endX = (x + radius) < _sizeX ? x + radius : (_sizeX-1);
	uint32 endY = (y + radius) < _sizeY ? y + radius : (_sizeY-1);
	uint32 startX = x - radius > 0 ? x - radius : 0;
	uint32 startY = y - radius > 0 ? y - radius : 0;
	uint32 posX, posY;

	MapCell *objCell;

	for (posX = startX; posX <= endX; posX++ )
	{
		for (posY = startY; posY <= endY; posY++ )
		{
			objCell = GetCell(posX, posY);

			if (!objCell)
			{
				if( ForceCreateStatic != 0 || _CellActive(posX, posY) )
				{
					objCell = Create(posX, posY);
					objCell->Init(posX, posY, _mapId, this);

					sLog.outDetail("Cell [%d,%d] on map %d (instance %d) is now active.", posX, posY, this->_mapId, m_instanceID);
					objCell->SetActivity(true);
//					_map->CellGoneActive(posX, posY);

					ASSERT(!objCell->IsLoaded());

					sLog.outDetail("Loading objects for Cell [%d][%d] on map %d (instance %d)...", posX, posY, this->_mapId, m_instanceID);

					sp = _map->GetSpawnsList(posX, posY);
					if(sp) 
						objCell->LoadObjects(sp);
					//do not unload it in the future. Required for scripts
					if( ForceCreateStatic == 2 )
						m_forcedcells.insert( objCell );
				}
			}
			else
			{
				//Cell is now active
				if (_CellActive(posX, posY) && !objCell->IsActive())
				{
					sLog.outDetail("Cell [%d,%d] on map %d (instance %d) is now active.", posX, posY, this->_mapId, m_instanceID);
//					_map->CellGoneActive(posX, posY);
					objCell->SetActivity(true);

					if (!objCell->IsLoaded())
					{
						sLog.outDetail("Loading objects for Cell [%d][%d] on map %d (instance %d)...", posX, posY, this->_mapId, m_instanceID);
						sp = _map->GetSpawnsList(posX, posY);
						if(sp) objCell->LoadObjects(sp);
					}
				}
				//Cell is no longer active
				else if (!_CellActive(posX, posY) && objCell->IsActive())
				{
					sLog.outDetail("Cell [%d,%d] on map %d (instance %d) is now idle.", posX, posY, this->_mapId, m_instanceID);
//					_map->CellGoneIdle(posX, posY);
					objCell->SetActivity(false);
				}
			}
		}
	}

}

bool MapMgr::_CellActive(uint32 x, uint32 y)
{
	uint32 endX = ((x+1) < _sizeX) ? x + 1 : (_sizeX-1);
	uint32 endY = ((y+1) < _sizeY) ? y + 1 : (_sizeY-1);
	uint32 startX = x > 0 ? x - 1 : 0;
	uint32 startY = y > 0 ? y - 1 : 0;
	uint32 posX, posY;

	MapCell *objCell;

	for (posX = startX; posX <= endX; posX++ )
		for (posY = startY; posY <= endY; posY++ )
		{
			objCell = GetCell(posX, posY);

			if (objCell)
				if ( objCell->HasPlayers() || m_forcedcells.find( objCell ) != m_forcedcells.end() )
				{ 
					return true;
				}
		}

	return false;
}

void MapMgr::ObjectUpdated(Object *obj)
{
	// set our fields to dirty
	// stupid fucked up code in places.. i hate doing this but i've got to :<
	// - burlex
	m_updateMutex.Acquire();

/*	{
		bool AlreadyPending = false;	
		for(UpdateQueueList::iterator iter = _updates.begin(); iter != _updates.end(); ++iter)
		{
			if( (*iter) == obj )
			{
				AlreadyPending = true;
				break;
			}
		}
		
		if( AlreadyPending == false )
			_updates.push_back(obj);
	}/**/
	_updates.insert( obj );

	m_updateMutex.Release();
}

void MapMgr::PushToProcessed(Player* plr)
{
	_processQueue.insert(plr);
}


void MapMgr::ChangeFarsightLocation(Player *plr, DynamicObject *farsight)
{
	if(farsight == 0)
	{
		// We're clearing.
		for(ObjectSet::iterator itr = plr->m_visibleFarsightObjects.begin(); itr != plr->m_visibleFarsightObjects.end();
			++itr)
		{
			if(plr->IsVisible((*itr)) && !plr->CanSee((*itr)))
			{
				// Send destroy
				plr->PushOutOfRange((*itr)->GetNewGUID());
			}
		}
		plr->m_visibleFarsightObjects.clear();
	}
	else
	{
		uint32 cellX = GetPosX(farsight->GetPositionX());
		uint32 cellY = GetPosY(farsight->GetPositionY());
		uint32 endX = (cellX < _sizeX - 1 ) ? cellX + 1 : (_sizeX-1);
		uint32 endY = (cellY < _sizeY - 1 ) ? cellY + 1 : (_sizeY-1);
		uint32 startX = cellX > 0 ? cellX - 1 : 0;
		uint32 startY = cellY > 0 ? cellY - 1 : 0;
		uint32 posX, posY;
		MapCell *cell;
		Object *obj;
		MapCell::ObjectSet::iterator iter, iend;
		uint32 count;
		for (posX = startX; posX <= endX; ++posX )
		{
			for (posY = startY; posY <= endY; ++posY )
			{
				cell = GetCell(posX, posY);
				if (cell)
				{
					iter = cell->Begin();
					iend = cell->End();
					for(; iter != iend; ++iter)
					{
						obj = (*iter);
						if(!plr->IsVisible(obj) && plr->CanSee(obj) && farsight->GetDistanceSq(obj) <= m_UpdateDistance)
						{
							temp_buff.clear();
							count = obj->BuildCreateUpdateBlockForPlayer(&temp_buff, plr);
							plr->PushCreationData( &temp_buff, count, obj );
							plr->m_visibleFarsightObjects.insert(obj);
						}
					}
					
				}
			}
		}
	}
}

#ifdef FORCED_SERVER_KEEPALIVE

//there is a chance at least one of the players got corrupted
void MapMgr::TeleportCorruptedPlayers()
{
	PlayerStorageMap::iterator itr =  m_PlayerStorage.begin();
	for(; itr !=  m_PlayerStorage.end();)
	{
		Player *p = itr->second;
		++itr;
		try	
		{ 
//			p->EjectFromInstance();
			if( p->GetSession() )
			{
				p->GetSession()->LogoutPlayer(false);
			}
			else
			{
				delete p;
				p = NULL;
			}
		}catch(int er)	{ er = 1; }//lols for the warning
	}
}
//an exception ocured somewhere. We try to cleanup if there is anything to clean up and then kill this thread.
//We should not save data in case some memory corruption ocured
//we should be prepared that during cleanup process we meet the same exeption and we have to skip that (or handle it)
//there might be cases when we should restart the mapmanager made for the the map (like always loaded maps)
void MapMgr::KillThreadWithCleanup()
{
	try
	{
		// remove all events regarding to this map
		sEventMgr.RemoveEvents( this );

		// Teleport any left-over players out.
		TeleportCorruptedPlayers();	

		if(m_battleground)
		{
			sInstanceMgr.DeleteBattlegroundInstance( GetMapId(), GetInstanceID() );
			BattlegroundManager.DeleteBattleground(m_battleground);
			m_battleground = NULL;
		}

		if(pInstance)
		{
			// check for a non-raid instance, these expire after 10 minutes.
			if(GetMapInfo()->type == INSTANCE_DUNGEON || pInstance->m_isBattleground)
			{
				pInstance->m_mapMgr = NULL;
				sInstanceMgr._DeleteInstance(pInstance, true);
				pInstance = NULL; //DO NOT USE THIS pointer since it is deleted now
			}
			else
			{
				// just null out the pointer
				pInstance->m_mapMgr=NULL;
			}
		}
		else if(GetMapInfo()->type == INSTANCE_NULL)
		{
			uint32 this_mapid = GetMapId();
			sInstanceMgr.m_singleMaps[ this_mapid ] = NULL;
			//if we are really cocky we try to create a new map instead of the crashed one
			sInstanceMgr._CreateInstance( this_mapid, sInstanceMgr.GenerateInstanceID() );
		}

		thread_running = false;

		// delete ourselves
//		delete this; //threadpool will delete us ?
	}
	catch(int error)
	{
		error = 1;//deal with this later
		//we are just avoiding the process to catch this exeption. We know it was comming
	}
//	KillThread();
}
#endif

bool MapMgr::run()
{
	bool rv=true;
	THREAD_TRY_EXECUTION_MapMgr
		rv = Do();
	THREAD_HANDLE_CRASH_MapMgr
	return rv;
}

bool MapMgr::Do()
{
#if (defined( WIN32 ) || defined( WIN64 ) )
	threadid=GetCurrentThreadId();
#endif
	thread_running = true;
	ThreadState =THREADSTATE_BUSY;
	SetThreadName("Map mgr - M%u|I%u",this->_mapId ,this->m_instanceID);
	uint32 last_exec=getMSTime();
	// Create Instance script 
	LoadInstanceScript(); 

	/* create static objects */
	for(GOSpawnList::iterator itr = _map->staticSpawns.GOSpawns.begin(); itr != _map->staticSpawns.GOSpawns.end(); ++itr)
	{
		GameObject * obj = CreateGameObject((*itr)->entry);
		obj->Load((*itr));
		_mapWideStaticObjects.insert(obj);
	}

	// Call script OnLoad virtual procedure 
 	CALL_INSTANCE_SCRIPT_EVENT( this, OnLoad )(); 

	for(CreatureSpawnList::iterator itr = _map->staticSpawns.CreatureSpawns.begin(); itr != _map->staticSpawns.CreatureSpawns.end(); ++itr)
	{
		Creature * obj = CreateCreature((*itr)->entry);
		obj->Load(*itr, 0, pMapInfo);
		_mapWideStaticObjects.insert(obj);
	}

	/* add static objects */
	for(InRangeSet::iterator itr = _mapWideStaticObjects.begin(); itr != _mapWideStaticObjects.end(); ++itr)
		PushStaticObject(*itr);

	/* load corpses */
	objmgr.LoadCorpses(this);

	// always declare local variables outside of the loop!
	// otherwise theres a lot of sub esp; going on.

	uint32 exec_time, exec_start;
#if (defined( WIN32 ) || defined( WIN64 ) )
	HANDLE hThread = GetCurrentThread();
#endif
	while((ThreadState != THREADSTATE_TERMINATE) && !_shutdown)
	{
		exec_start=getMSTime();
		//first push to world new objects
		m_objectinsertlock.Acquire();//<<<<<<<<<<<<<<<<
		if( m_objectinsertpool.empty() == false )
		{
			//during pushing objects this list might change. If we clear content some objects might never get to be pushed in world
			ObjectSet m_objectinsertpool_local( m_objectinsertpool );
			//clear now, it might have items after the loop
			m_objectinsertpool.clear();

			ObjectSet::iterator i;
			for(i=m_objectinsertpool_local.begin();i!=m_objectinsertpool_local.end();i++)
			{
				// fighting multi threading bug. Players is ported in player port event thus getting added to multiple insert queues.
				// it is bad if we do that but even worse if we delete the object due to client crash and leave player in one of the queues to crash the server
				if( (*i)->IsInWorld() && (*i)->GetMapMgr() != this )
				{
					sLog.outDebug("Player is already in world. Not adding it\n");
					continue;
				}
				//PushObject(*i);
				(*i)->PushToWorld(this);
			}
		}
		m_objectinsertlock.Release();//>>>>>>>>>>>>>>>>
		//-------------------------------------------------------
				
		//Now update sessions of this map + objects
		_PerformObjectDuties();

		//update global MS precision time. We do not need more precise then update loop one
		global_MS_time = GetTickCount();

		last_exec = getMSTime();
		last_updated = last_exec;
		exec_time = last_exec - exec_start ;

		//only monitor active maps. Avoid breaking statistics on idleing maps :P
		if( m_PlayerStorage.size() )
		{
			timer_exec_time += exec_time;
			timer_exec_count++;
		}

		if(exec_time<MAP_MGR_UPDATE_PERIOD)
		{
			/*
				The common place I see this is waiting for a Win32 thread to exit. I used to come up with all sorts of goofy,
				elaborate event-based systems to do this myself until I discovered that thread handles are waitable. Just use
				WaitForSingleObject() on the thread handle and you're done. No risking race conditions with the thread exit code.
				I think pthreads has pthread_join() for this too.

				- http://www.virtualdub.org/blog/pivot/entry.php?id=62
			*/

#if (defined( WIN32 ) || defined( WIN64 ) )
			WaitForSingleObject(hThread, MAP_MGR_UPDATE_PERIOD-exec_time);
#else
			Sleep(MAP_MGR_UPDATE_PERIOD-exec_time);
#endif
		}

		//////////////////////////////////////////////////////////////////////////
		// Check if we have to die :P
		//////////////////////////////////////////////////////////////////////////
		if(InactiveMoveTime && UNIXTIME >= InactiveMoveTime)
			break;
	}
	last_updated = 0; //not active thread from our point of view

	// Teleport any left-over players out.
	// Zack: stupid thread collision. It would crash the shutdown process if maps that we are going to port are getting deleted.
	TeleportPlayers();	

	// Clear the instance's reference to us.
	if(m_battleground)
	{
		BattlegroundManager.DeleteBattleground(m_battleground);
		sInstanceMgr.DeleteBattlegroundInstance( GetMapId(), GetInstanceID() );
		m_battleground = NULL;
	}

	if(pInstance)
	{
		// check for a non-raid instance, these expire after 10 minutes.
		if( IS_MAP_DUNGEON( GetMapInfo() ) || pInstance->m_isBattleground)
		{
			pInstance->m_mapMgr = NULL;
			sInstanceMgr._DeleteInstance(pInstance, true);
			pInstance = NULL; //DO NOT USE THIS pointer since it is deleted now
		}
		else
		{
			// just null out the pointer
			pInstance->m_mapMgr=NULL;
		}
	}
	else if( IS_STATIC_MAP( GetMapInfo() ) )
		sInstanceMgr.m_singleMaps[GetMapId()] = NULL;

	thread_running = false;
	if(thread_kill_only)
	{ 
		return false;
	}

	// delete ourselves
	delete this;

	// already deleted, so the threadpool doesn't have to.
	return false;
}

void MapMgr::BeginInstanceExpireCountdown()
{
	WorldPacket data(SMSG_RAID_GROUP_ONLY, 8);
	PlayerStorageMap::iterator itr;

	// so players getting removed don't overwrite us
	forced_expire = true;

	// send our sexy packet
    data << uint32(60000) << uint32(1);
	for(itr = m_PlayerStorage.begin(); itr != m_PlayerStorage.end(); ++itr)
	{
		if(!itr->second->raidgrouponlysent)
			itr->second->GetSession()->SendPacket(&data);
	}

	// set our expire time to 60 seconds.
	InactiveMoveTime = UNIXTIME + 60;
}

void MapMgr::AddObject(Object *obj)
{
	m_objectinsertlock.Acquire();//<<<<<<<<<<<<
	m_objectinsertpool.insert(obj);
	m_objectinsertlock.Release();//>>>>>>>>>>>>
}


Unit* MapMgr::GetUnit(const uint64 guid)
{
	if ( guid == 0 )
	{ 
		sLog.outDebug("MapMgr: asking for unit without GUID\n");
		return NULL;
	}
	switch(GET_TYPE_FROM_GUID(guid))
	{
	case HIGHGUID_TYPE_UNIT:
//	case HIGHGUID_TYPE_VEHICLE:
		return GetCreature( guid );
		break;

	case HIGHGUID_TYPE_PLAYER:
		return GetPlayer( guid );
		break;

	case HIGHGUID_TYPE_PET:
		return GetPet( guid );
		break;
	}

	return NULL;
}

/*
ARCEMU_INLINE Vehicle* MapMgr::GetVehicle(const uint64 guid)
{
	Unit *pu = GetCreature( guid );
	if( !pu || !pu->IsVehicle() )
		return NULL;
	return static_cast< Vehicle* >(pu);
}*/

Object* MapMgr::_GetObject(const uint64 guid)
{
	if (!guid)
	{ 
		return NULL;
	}

	switch(GET_TYPE_FROM_GUID(guid))
	{
	case	HIGHGUID_TYPE_GAMEOBJECT:
		return GetGameObject( guid );
		break;
	case	HIGHGUID_TYPE_UNIT:
//	case	HIGHGUID_TYPE_VEHICLE:
		return GetCreature( guid );
		break;
	case	HIGHGUID_TYPE_DYNAMICOBJECT:
		return GetDynamicObject( guid );
		break;
	case	HIGHGUID_TYPE_TRANSPORTER:
		return objmgr.GetTransporter(GUID_LOPART(guid));
		break;
	default:
		return GetUnit(guid);
		break;
	}
}

void MapMgr::_PerformObjectDuties()
{
	++mLoopCounter;
	uint32 mstime = getMSTime();
	uint32 difftime = mstime - lastUnitUpdate;
	//wtf ?
//	if(difftime > 500)
//		difftime = 500;
	// Update any events.
	// we make update of events before objects so in case there are 0 timediff events they do not get deleted after update but on next server update loop
	eventHolder.Update(difftime);
#ifdef _CRTDBG_MAP_ALLOC_TEST_MEM
	_CrtCheckMemory();
#endif

	// Update creatures.
	{
//		CreatureSet creatures(activeCreatures);
//		for(CreatureSet::iterator itr = activeCreatures.begin(); activeCreaturesItr != activeCreatures.end();)
		for(activeCreaturesItr = activeCreatures.begin(); activeCreaturesItr != activeCreatures.end();)
		{
			Creature *ptr = *activeCreaturesItr;
			 ++activeCreaturesItr;	//increment before update to not get double increment due to chained NPC remove
			if( ptr->deleted != OBJ_AVAILABLE )
			{
				activeCreatures.erase( ptr );	//aaa, same as deactivate ?
				RemoveObject( ptr, false );
				continue;
			}
			ptr->Update( difftime );
#ifdef _CRTDBG_MAP_ALLOC_TEST_MEM
			_CrtCheckMemory();
#endif
		}
	}
	{
		PetStorageMap::iterator it2 = m_PetStorage.begin();
		for(; it2 != m_PetStorage.end();)
		{
			Pet *ptr = it2->second;
			++it2;
			if( ptr->deleted != OBJ_AVAILABLE )
			{
				m_PetStorage.erase( ptr->GetLowGUID() );	//same as in removeobject ?
				RemoveObject( ptr, false );
				continue;
			}
			ptr->Update( difftime );
#ifdef _CRTDBG_MAP_ALLOC_TEST_MEM
			_CrtCheckMemory();
#endif
		}		
	}

	// Update players.
	{
		PlayerStorageMap::iterator itr = m_PlayerStorage.begin();
		for(; itr != m_PlayerStorage.end(); )
		{
			Player* ptr = SafePlayerCast( (itr->second) );
			++itr;
			if( ptr->deleted != OBJ_AVAILABLE )
			{
				m_PlayerStorage.erase( ptr->GetLowGUID() );	//same as in removeobject ?
				RemoveObject( ptr, false );
				continue;
			}
			// wholy crap player is added to multiple maps ? Or just not removed from ours ?
			// this happens because someone implemented the damn teleport on telleport thing
			// this never crashed but it is better to be safe about this since multiple map adding happens
			if( ptr->GetMapMgr() != this )
			{
				m_PlayerStorage.erase( ptr->GetLowGUID() ); 	//same as in removeobject ?
				RemoveObject( ptr, false );
				sLog.outDebug("Player is already present in some other mapmanager : removing it from this one\n");
				continue;
			}
			ptr->Update( difftime );
#ifdef _CRTDBG_MAP_ALLOC_TEST_MEM
			_CrtCheckMemory();
#endif
		}

		lastUnitUpdate = mstime;
	}

	// Update gameobjects (not on every loop, however)
	if( mLoopCounter % 2 )
	{
		difftime = mstime - lastGameobjectUpdate;

		GameObjectSet::iterator itr = activeGameObjects.begin();
		GameObject * ptr;
		for(; itr != activeGameObjects.end(); )
		{
			ptr = *itr;
			++itr;
			ptr->Update( difftime );
#ifdef _CRTDBG_MAP_ALLOC_TEST_MEM
			_CrtCheckMemory();
#endif
		}

		lastGameobjectUpdate = mstime;
	}	

	//static objects - these can be creatures or gamobjects that always get updated...do not spam these on the map
	//might need to work on the part when they get killed...
/*	for(InRangeSet::iterator itr = _mapWideStaticObjects.begin(); itr != _mapWideStaticObjects.end(); ++itr)
	{
		if( (*itr)->IsCreature() )
		{
			Creature *ptr = SafeCreatureCast( *itr );
			if( ptr->deleted != OBJ_AVAILABLE )
				continue;
			ptr->Update(difftime);
		}
		if( (*itr)->IsGameObject() )
		{
			GameObject *ptr = SafeGOCast( *itr );
			if( ptr->deleted != OBJ_AVAILABLE )
				continue;
			ptr->Update(difftime);
		}
	}/**/

	// Sessions are updated every loop.
	{
		SessionSet::iterator itr,itr2;
		SessionSet new_list;	//updatesession changes the list. Mutex would deadlock
		new_list = Sessions;
		//players that teleport to other maps are removed from session list. Other threads may insert new sessions also
		//!!!!this is randomly creating stack corruption. I cannot find the reason till this day
		for(itr2 = new_list.begin(); itr2 != new_list.end(); )
		{
			itr = itr2;
			itr2++;
			WorldSession * session;
			session = (*itr);

			//idiot threading issue. Double check if this was deleted somewhere else and for some reason left in this list.
			//probably some global session update will remove the bad pointer. Not touching it due to parallel references
			if( sWorld.FindSession( session->GetAccountId() ) != session || session->bDeleted == true )
			{
				RemoveSession( session );
				continue;
			}

			if(session->GetInstance() != m_instanceID)
			{
				RemoveSession( session );
				continue;
			}

			// Don't update players not on our map.
			// If we abort in the handler, it means we will "lose" packets, or not process this.
			// .. and that could be diasterous to our client :P
			if(session->GetPlayer() && (session->GetPlayer()->GetMapMgr() != this && session->GetPlayer()->GetMapMgr() != 0))
			{
				continue;
			}

			//if while processing packets player changed the map then we are supposed to remove him from our list
			if( session->Update(m_instanceID) != 0 )
				RemoveSession( session );
		}
	}

	// Finally, A9 Building/Distribution
	_UpdateObjects();

#ifdef USE_MULTIBOXING_DETECTOR
	PlayerStorageMap::iterator itr = m_PlayerStorage.begin();
	MultiboxingDetector->StartCycle();
	for(; itr != m_PlayerStorage.end(); ++itr)
	{
		Player* ptr = SafePlayerCast( (itr->second) );
		MultiboxingDetector->AddPlayer( ptr );
	}
	MultiboxingDetector->EndCycle();
#endif

#ifdef _CRTDBG_MAP_ALLOC_TEST_MEM
	_CrtCheckMemory();
#endif
}

void MapMgr::TeleportPlayers()
{
	PlayerStorageMap::iterator itr =  m_PlayerStorage.begin();
	if(!bServerShutdown)
	{
		for(; itr !=  m_PlayerStorage.end();)
		{
			Player *p = itr->second;
			++itr;
			p->EjectFromInstance();
		}
	}
	else
	{
		for(; itr !=  m_PlayerStorage.end();)
		{
			Player *p = itr->second;
			++itr;
			if(p->GetSession())
				p->GetSession()->LogoutPlayer(false);
			else
			{
				sGarbageCollection.AddObject( p );
				p = NULL;
			}
		}
	}
}

void MapMgr::UnloadCell(uint32 x,uint32 y)
{
	MapCell * c = GetCell(x,y);
	if( c == NULL || _CellActive(x,y) || !c->IsUnloadPending()) 
	{ 
		return;
	}

	if( c->CanUnload() == false )
	{
		c->CancelPendingUnload(); //set state to a not unloaded one
		c->QueueUnloadPending(); //queue it for a new unload. Maybe objects will move out from it meantime
	}

	sLog.outDetail("Unloading Cell [%d][%d] on map %d (instance %d)...", x,y,_mapId,m_instanceID);

//	c->Unload();
	//there is no point keeping it in memory as empty cell
	Remove( x, y );
}

void MapMgr::EventRespawnCreature(Creature *c, float pos_x,float pos_y)
{
	//wtf crash ? This is a hackfix !
	if( pos_x == 0 && pos_y == 0 )
	{ 
		return;
	}

	//wtf should never happen
	if( c->deleted != 0 || c->m_noRespawn == true )
	{
		return;
	}

	MapCell *cell = GetCellByCoords( pos_x, pos_y );
	if( !cell 
//		|| !cell->IsActive()
//		|| cell->IsUnloadPending() //Zack: Unload pending might get canceled so we still respawn the mob in an idle form 
		)
		return; //seems like cell got deleted since we died
	//check if creature indeed needs to be respawned
	ObjectSet::iterator itr = cell->_respawnObjects.find( c );
	if(itr != cell->_respawnObjects.end())
	{
//		c->m_respawnCell=NULL;
		cell->_respawnObjects.erase(itr);
		c->OnRespawn(this);
		if( !cell->IsActive() && c->Active )
			c->Deactivate( this );
	}
}

void MapMgr::EventRespawnGameObject(GameObject * o, float pos_x,float pos_y)
{
	//wtf should never happen
	if( o->deleted != 0 || o->m_noRespawn == true )
	{
		return;
	}

	MapCell *cell = GetCellByCoords( pos_x, pos_y );
	if( !cell 
//		|| !cell->IsActive() || cell->IsUnloadPending() 
		)
	{ 
		return;
	}
	ObjectSet::iterator itr = cell->_respawnObjects.find( o );
	if(itr != cell->_respawnObjects.end())
	{
//		o->m_respawnCell=NULL;
		cell->_respawnObjects.erase(itr);
		o->Spawn(this);
		if( !cell->IsActive() && o->Active )
			o->Deactivate();
	}
}

void MapMgr::SendMessageToCellPlayers(Object * obj, WorldPacket * packet, uint32 cell_radius /* = 2 */)
{
	uint32 cellX = GetPosX(obj->GetPositionX());
	uint32 cellY = GetPosY(obj->GetPositionY());
	uint32 endX = ((cellX+cell_radius) < _sizeX) ? cellX + cell_radius : (_sizeX-1);
	uint32 endY = ((cellY+cell_radius) < _sizeY) ? cellY + cell_radius : (_sizeY-1);
	uint32 startX = (cellX-cell_radius) > 0 ? cellX - cell_radius : 0;
	uint32 startY = (cellY-cell_radius) > 0 ? cellY - cell_radius : 0;

	uint32 posX, posY;
	MapCell *cell;
	MapCell::ObjectSet::iterator iter, iend;
	for (posX = startX; posX <= endX; ++posX )
		for (posY = startY; posY <= endY; ++posY )
		{
			cell = GetCell(posX, posY);
			if (cell && cell->HasPlayers() )
			{
				iter = cell->Begin();
				iend = cell->End();
				for(; iter != iend; ++iter)
					if((*iter)->IsPlayer())
						SafePlayerCast(*iter)->GetSession()->SendPacket(packet);
			}
		}
}

void MapMgr::SendChatMessageToCellPlayers(Object * obj, WorldPacket * packet, uint32 cell_radius, uint32 langpos, int32 lang, WorldSession * originator)
{
	uint32 cellX = GetPosX(obj->GetPositionX());
	uint32 cellY = GetPosY(obj->GetPositionY());
	uint32 endX = ((cellX+cell_radius) < _sizeX) ? cellX + cell_radius : (_sizeX-1);
	uint32 endY = ((cellY+cell_radius) < _sizeY) ? cellY + cell_radius : (_sizeY-1);
	uint32 startX = (cellX-cell_radius) > 0 ? cellX - cell_radius : 0;
	uint32 startY = (cellY-cell_radius) > 0 ? cellY - cell_radius : 0;

	uint32 posX, posY;
	MapCell *cell;
	MapCell::ObjectSet::iterator iter, iend;
	for (posX = startX; posX <= endX; ++posX )
		for (posY = startY; posY <= endY; ++posY )
		{
			cell = GetCell(posX, posY);
			if (cell && cell->HasPlayers() )
			{
				iter = cell->Begin();
				iend = cell->End();
				for(; iter != iend; ++iter)
					if((*iter)->IsPlayer())
						//SafePlayerCast(*iter)->GetSession()->SendPacket(packet);
						SafePlayerCast(*iter)->GetSession()->SendChatPacket(packet, langpos, lang, originator);
			}
		}
}

Creature * MapMgr::GetSqlIdCreature(uint32 sqlid)
{
	CreatureSqlIdMap::iterator itr = _sqlids_creatures.find(sqlid);
	return (itr == _sqlids_creatures.end()) ? NULL : itr->second;
}

GameObject * MapMgr::GetSqlIdGameObject(uint32 sqlid)
{
	GameObjectSqlIdMap::iterator itr = _sqlids_gameobjects.find(sqlid);
	return (itr == _sqlids_gameobjects.end()) ? NULL : itr->second;
}

void MapMgr::HookOnAreaTrigger(Player * plr, uint32 id)
{
	switch (id)
	{
	case 4591:
		//Only opens when the first one steps in, if 669 if you find a way, put it in :P (else was used to increase the time the door stays opened when another one steps on it)
		GameObject *door = GetInterface()->GetGameObjectNearestCoords(803.827f, 6869.38f, -38.5434f, 184212);
		if (door && (door->GetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE) == 1))
		{
			door->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, 0);
			//sEventMgr.AddEvent(door, &GameObject::SetUInt32Value, GAMEOBJECT_STATE, 1, EVENT_SCRIPT_UPDATE_EVENT, 10000, 1, 0);
		}
		//else
		//{
			//sEventMgr.RemoveEvents(door);
			//sEventMgr.AddEvent(door, &GameObject::SetUInt32Value,GAMEOBJECT_STATE, 0, EVENT_SCRIPT_UPDATE_EVENT, 10000, 1, 0);
		//}
		break;
	}
}

Creature * MapMgr::CreateCreature(uint32 entry,bool FullInit,float x,float y,float z,float o)
{
//	uint64 newguid = ( (uint64)HIGHGUID_TYPE_UNIT << 32 ) | ( (uint64)petnumber << 24 );
	uint64 newguid = ( (uint64)HIGHGUID_TYPE_UNIT << 32 );

#ifndef DISABLE_GUID_RECYCLING
	if(_reusable_guids_creature.size())
	{
		uint32 guid = _reusable_guids_creature.front();
		_reusable_guids_creature.pop_front();

		newguid |= guid;
		return new Creature(newguid);
	}
#endif

	uint64 NewLowGUID = m_CreatureHighGuid;

	m_CreatureHighGuid++;
	if( ExtendCreatureStorage( m_CreatureHighGuid ) )
		return 0;

	newguid |= NewLowGUID;
	Creature *p = new Creature(newguid);
	if( FullInit && entry )
	{
		CreatureProto * proto = CreatureProtoStorage.LookupEntry(entry);
		CreatureInfo * info = CreatureNameStorage.LookupEntry(entry);
		if(proto && info)
		{
			p->Load(proto, x, y, z, o);
			p->m_faction = dbcFactionTemplate.LookupEntry( proto->Faction );
			if(p->m_faction)
				p->m_factionDBC = dbcFaction.LookupEntry( p->m_faction->Faction );
			p->PushToWorld( this );
		}
	}
	return p;
}

ARCEMU_INLINE uint32 MapMgr::ExtendGOStorage( uint32 NewGUID )
{
	if( NewGUID > 500000 )
		return 1; //error, too many spawns on this map
	if( NewGUID >= m_GOArraySize )
	{
		uint32 diff = NewGUID - m_GOArraySize;
		diff = MAX( diff, RESERVE_EXPAND_SIZE );
		// Reallocate array with larger size.
		uint32 New_m_GOArraySize = m_GOArraySize + diff;
		m_GOStorage = (GameObject**)realloc( m_GOStorage, sizeof(GameObject*) * New_m_GOArraySize );
		memset( &m_GOStorage[m_GOArraySize], 0, diff * sizeof(GameObject*));
		m_GOArraySize = New_m_GOArraySize;
	}
	return 0;
}

ARCEMU_INLINE uint32 MapMgr::ExtendCreatureStorage( uint32 NewGUID )
{
	if( NewGUID > LIMIT_MAX_SPAWN_COUNT_ON_1_MAP )
		return 1; //error, too many spawns on this map
	if( NewGUID >= m_CreatureArraySize )
	{
		uint32 diff = NewGUID - m_CreatureArraySize;
		diff = MAX( diff, RESERVE_EXPAND_SIZE );
		// Reallocate array with larger size.
		uint32 New_m_CreatureArraySize = m_CreatureArraySize + diff;
		m_CreatureStorage = (Creature**)realloc( m_CreatureStorage, sizeof(Creature*) * New_m_CreatureArraySize );
		memset( &m_CreatureStorage[ m_CreatureArraySize ], 0, diff * sizeof(Creature*) );
		m_CreatureArraySize = New_m_CreatureArraySize;
	}
	return 0;
}

GameObject * MapMgr::CreateGameObject(uint32 entry)
{
#ifndef DISABLE_GUID_RECYCLING
	if( _reusable_guids_gameobject.size() > GO_GUID_RECYCLE_INTERVAL )
	{
		uint32 reguid = _reusable_guids_gameobject.front();
		_reusable_guids_gameobject.pop_front();
//		uint64 new_guid = ( (uint64)HIGHGUID_TYPE_GAMEOBJECT << 32 ) | ( (uint64)entry << 24 ) | reguid;
		uint64 new_guid = ( (uint64)HIGHGUID_TYPE_GAMEOBJECT << 32 ) | reguid;
		return new GameObject( new_guid );
	}
#endif

	uint64 UseLowGUID = m_GOHighGuid;
	m_GOHighGuid++;
	if( ExtendGOStorage( m_GOHighGuid ) )
		return 0;
//	uint64 new_guid = ( (uint64)HIGHGUID_TYPE_GAMEOBJECT << 32 ) | ( (uint64)entry << 24 ) | m_GOHighGuid;
	uint64 new_guid = ( (uint64)HIGHGUID_TYPE_GAMEOBJECT << 32 ) | UseLowGUID;
	return new GameObject( new_guid );
}

// Spawns the object too, without which you can not interact with the object
GameObject * MapMgr::CreateAndSpawnGameObject(uint32 entryID, float x, float y, float z, float o, float scale, bool TempSpawn)
{
	GameObjectInfo* goi = GameObjectNameStorage.LookupEntry(entryID);
	if(!goi)
	{
		sLog.outDebug("Error looking up entry in CreateAndSpawnGameObject");
		return NULL;
	}

	sLog.outDebug("CreateAndSpawnGameObject: By Entry '%u'", entryID);

	GameObject *go = CreateGameObject(entryID);

	//Player *chr = m_session->GetPlayer();
	uint32 mapid = GetMapId();
	// Setup game object
	go->SetInstanceID(GetInstanceID());
	go->CreateFromProto(entryID,mapid,x,y,z,o);
	go->SetFloatValue(OBJECT_FIELD_SCALE_X, scale);
	go->InitAI();
	go->PushToWorld(this);

	// Create spawn instance
	if( TempSpawn == false )
	{
		GOSpawn * gs = new GOSpawn;
		gs->entry = go->GetEntry();
		gs->o = go->GetOrientation();
		gs->faction = go->GetUInt32Value(GAMEOBJECT_FACTION);
		gs->flags = go->GetUInt32Value(GAMEOBJECT_FLAGS);
		gs->id = objmgr.GenerateGameObjectSpawnID();
		gs->parent_rot1 = go->GetFloatValue(GAMEOBJECT_PARENTROTATION);
		gs->parent_rot2 = go->GetFloatValue(GAMEOBJECT_PARENTROTATION);
		gs->parent_rot3 = go->GetFloatValue(GAMEOBJECT_PARENTROTATION_2);
		gs->parent_rot4 = go->GetFloatValue(GAMEOBJECT_PARENTROTATION_3);
		gs->scale = go->GetFloatValue(OBJECT_FIELD_SCALE_X);
		gs->x = go->GetPositionX();
		gs->y = go->GetPositionY();
		gs->z = go->GetPositionZ();
		gs->state = go->GetByte(GAMEOBJECT_BYTES_1, 0);
		gs->precise_facing = 0;

		uint32 cx = GetPosX(x);
		uint32 cy = GetPosY(y);

		GetBaseMap()->GetSpawnsListAndCreate(cx,cy)->GOSpawns.push_back(gs);
		go->m_spawn = gs;
	}

	MapCell * mCell = GetCell( x, y );

	if( mCell != NULL )
		mCell->_loaded = true; //lies !

	return go;
}

DynamicObject * MapMgr::CreateDynamicObject()
{
	return new DynamicObject(HIGHGUID_TYPE_DYNAMICOBJECT,(++m_DynamicObjectHighGuid));
}

void MapMgr::AddForcedCell( MapCell * c )
{
	m_forcedcells.insert( c );
	UpdateCellActivity( c->GetPositionX(), c->GetPositionY(), 1 );
}
void MapMgr::RemoveForcedCell(MapCell* c)
{
	m_forcedcells.erase( c );
	UpdateCellActivity( c->GetPositionX(), c->GetPositionY(), 1 );
}

float  MapMgr::GetLandHeight(float x, float y,float z)
{ 
#ifdef ENABLE_STATISTICS_GENERATED_HEIGHTMAP
	// based on statistical values as players walk the map. The higher ground is stored in case there are multiple levels
	//these values are stored to be lifted 2 yards from the start
	float SVMap_z = sSVMaps.GetHeight( GetMapId(), x, y, z );
	if( SVMap_z != VMAP_VALUE_NOT_INITIALIZED )
	{ 
		return SVMap_z + 1.0f;	//if you manage to guess the value with less then 1 yard precission then wow will do the collision test and make a smooth movement. Else "fall" animation will be triggered
	}
	//because there are tunnels and stuffs where the other height manager will generate "good" values and makes players run through walls
	if( IS_BG_OR_ARENA( GetMapInfo() ) )
		return VMAP_VALUE_NOT_INITIALIZED;
#endif
	// this is pretty accurate, hax added 2.5 yards (rarely only extra 1.5 is used) because on porting client does 
	// weak collision and might let ppl fall underground
	// !! does not take into count objects like a city spawned on the map
	// has only 1 elevation level inside
	float TM2_z = sTerrainMgr.GetHeight( GetMapId(), x, y, z );
	if( TM2_z != VMAP_VALUE_NOT_INITIALIZED )
	{ 
		return TM2_z + 1.0f;
	}
	if( z != -10000 )
	{
		return z + 1.0f;
	}

	return VMAP_VALUE_NOT_INITIALIZED;
}

uint16 MapMgr::GetAreaID(float x, float y)
{
	return sTerrainMgr.GetAreaID( GetMapId(), x, y, 0.0f); 
}

void MapMgr::LoadInstanceScript() 
{ 
	mInstanceScript = sScriptMgr.CreateScriptClassForInstance( _mapId, this ); 
}

void MapMgr::CallScriptUpdate() 
{ 
	ASSERT( mInstanceScript ); 
	mInstanceScript->UpdateEvent(); 
}

Vehicle * MapMgr::GetSqlIdVehicle(uint32 sqlid)
{
	CreatureSqlIdMap::iterator itr = _sqlids_creatures.find(sqlid);
	if( itr == _sqlids_creatures.end() )
		return NULL;
	return itr->second->GetVehicle();
}

//!!!!!!! due to area overlays this can fail badly
//we still use it in case we have no map files extracted for a specific location
uint32 GetProbableZoneIdForLocation(uint32 map_id, float x, float y, bool UnknownArea)
{
	if( UnknownArea == true )
	{
		//!! we can also get area id from maps (of we have maps) and areas always belong to a zone
		//speed over precission #care
		int x1 = (int)x;
		int y1 = (int)y;
		uint32 best_distance = 0x0FFFFFFF;
		uint32 best_zone = 0;
		for(uint32 i=0; i < dbcWorldMapZoneStore.GetNumRows(); i++)
		{
			WorldMapAreaEntry *wma = dbcWorldMapZoneStore.LookupRow(i);

			//make sure to set arena flags 
			if( wma->zone_id != 0			//we have whole continent sizes too ;)
				&& wma->map_id == map_id
				&& wma->min_x <= x 
				&& wma->max_x >= x
				&& wma->min_y <= y
				&& wma->max_y >= y
				)
			{
				int dist_x = x1 - ( (int)( wma->min_x + wma->max_x ) / 2 );
				int dist_y = y1 - ( (int)( wma->min_y + wma->max_y ) / 2 );
				uint32 sq_dist = dist_x*dist_x+dist_y*dist_y;
				if( sq_dist <= best_distance )
				{
					best_distance = sq_dist;
					best_zone = wma->zone_id;
				}
			}
		}
		return best_zone;
	}
	else
	{
		uint16 new_area_id = sTerrainMgr.GetAreaID( map_id, x, y, 0.0f );
		//exploration needs area, rest needs the zone id
		AreaTable * at = dbcArea.LookupEntryForced( new_area_id );
		if(at == NULL)
			return GetProbableZoneIdForLocation( map_id, x, y, true );
		return at->ZoneId; 
	}
	return 0;
}