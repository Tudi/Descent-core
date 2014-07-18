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
// MapCell.cpp
//
#include "StdAfx.h"

MapCell::~MapCell()
{
	RemoveObjects();
}

void MapCell::Init(uint32 x, uint32 y, uint32 mapid, MapMgr *mapmgr)
{
	_mapmgr = mapmgr;
	_active = false;
	_loaded = false;
	_playerCount = 0;
	_x=x;
	_y=y;
	_unloadpending=false;
	ObjectCountToReserveOnEnter = 0;
}

void MapCell::AddObject(Object *obj)
{
	//we intend to delete this cell, do not corrupt list / itr
	if( _mapmgr == NULL || _mapmgr->_shutdown == true )
		return;

	if(obj->IsPlayer())
		++_playerCount;

	_objects.insert(obj);
}

void MapCell::RemoveObject(Object *obj)
{
	if(obj->IsPlayer())
		--_playerCount;

	//do not recursively change the object list or it might get corrupted. Ex : hunter and he's pet are removeg together.
	if( _mapmgr->_shutdown == true )
	{ 
		return; 
	}

	_objects.erase(obj);
}

void MapCell::SetActivity(bool NewStateActive)
{	
	if( _active == false && NewStateActive == true )
	{
		// Move all objects to active set.
		for(ObjectSet::iterator itr = _objects.begin(); itr != _objects.end(); ++itr)
		{
			if(!(*itr)->Active && (*itr)->CanActivate())
				(*itr)->Activate(_mapmgr);
		}

		if(_unloadpending)
			CancelPendingUnload();

		if (sWorld.Collision) 
		{
			CollideInterface.ActivateTile(_mapmgr->GetMapId(), _x/8, _y/8);
		}
	} 
	else if( _active == true && NewStateActive == false )
	{
		// Move all objects from active set.
		for(ObjectSet::iterator itr = _objects.begin(); itr != _objects.end(); ++itr)
		{
			if((*itr)->Active)
				(*itr)->Deactivate(_mapmgr);
		}

		if( sWorld.map_unload_time && !_unloadpending )
			QueueUnloadPending();

		if (sWorld.Collision) 
		{
			CollideInterface.DeactivateTile(_mapmgr->GetMapId(), _x/8, _y/8);
		}
	}

	_active = NewStateActive; 

}
void MapCell::RemoveObjects()
{
	ObjectSet::iterator itr;
	uint32 count = 0;
	//uint32 ltime = getMSTime();

	//This time it's simpler! We just remove everything :)
	if( _objects.size() ) //whooly crap. double check to see if list is corrupted. No idea why this even exists :(
	for(itr = _objects.begin(); itr != _objects.end(); )
	{
		count++;

		Object *obj = (*itr);

		itr++;

		//this object is not even on this map ?
		if(!obj || obj->GetMapMgr() != _mapmgr )
			continue;

		if( obj->IsGameObject() )
		{
			SafeGOCast( obj )->m_noRespawn = true;
			SafeGOCast( obj )->m_respawnCell = NULL;
		}
		else if( obj->IsCreature() )
		{
			SafeCreatureCast( obj )->m_noRespawn = true;
			SafeCreatureCast( obj )->m_respawnCell = NULL;
		}

		//remove from world even corpses in case map is getting destroyed to avoid invalid pointer references ?
		if( obj->Active )
			obj->Deactivate( _mapmgr );

		obj->InrageConsistencyCheck( true );

		if( obj->IsInWorld() )
		{
			//this should remove spell spawns and stuff
			obj->RemoveFromWorld( true );
		}
		
		//!!! if we do not delete these then they will create mem corruption on moving them to other cell or removing from world
		//if( _unloadpending )
		{
			//do not delete pets and summons 
			//!!! this is a mem leak for spell created stuff !
			if( !obj->m_loadedFromDB )
			{
				obj->SetMapCell(NULL);
				sLog.outDebug("Object is loosing it's object manager. Some cleanup should delete it or it will leak memory");
				continue;
			}
			if(obj->GetTypeFromGUID() == HIGHGUID_TYPE_TRANSPORTER)
			{
				obj->SetMapCell(NULL);
				continue;
			}
			//object manager should cleanup these
			else if(obj->IsCorpse()	)
			{
				obj->SetMapCell(NULL);
				continue;
			}
			// wholy fucking crap. How did we get here ? Leaking him ? What can we do ? DC him ? But he is probably already DC-ed
			// this happens when a BG closes and players are still in it. Need to solve that actually
			else if( obj->IsPlayer() )
			{
				obj->SetMapCell(NULL);
				continue;
			}
		}

		//creatures, gameobjects and Dynobjects ?
//		delete obj;	//leave corpse deletion to Object manager maybe ? Not sure how many pointer refenreces remain uncleared  
		sGarbageCollection.AddObject( obj );
		obj = NULL;
	}
	_objects.clear();

	/* delete objects in pending respawn state */
	if( _respawnObjects.size() ) //whooly crap. double check to see if list is corrupted. No idea why this even exists :(
	for(itr = _respawnObjects.begin(); itr != _respawnObjects.end(); ++itr)
	{
		switch((*itr)->GetTypeId())
		{
		case TYPEID_UNIT: {
				if( !(*itr)->IsPet() )
				{
					//he is not in world so thoretically there is no guid collision
#ifndef DISABLE_GUID_RECYCLING
					_mapmgr->_reusable_guids_creature.push_back( (*itr)->GetUIdFromGUID() );
#endif
//					SafeCreatureCast( *itr )->m_respawnCell=NULL;	//break double chain
					SafeCreatureCast( *itr )->m_noRespawn = true;
					sGarbageCollection.AddObject( *itr );
				}
			}break;

		case TYPEID_GAMEOBJECT: {
#ifndef DISABLE_GUID_RECYCLING
			_mapmgr->_reusable_guids_gameobject.push_back( (*itr)->GetUIdFromGUID() );
#endif
//			SafeGOCast( *itr )->m_respawnCell=NULL;
			SafeGOCast( *itr )->m_noRespawn = true;
			sGarbageCollection.AddObject( *itr );
			}break;
		default:
			sLog.outError("Unsupported object in cell respawn list.Might create crash on mapremove. Typeid %u",(*itr)->GetTypeId());
		}
	}
	//!!important to clear this. removing object from world might insert it into respawn list, but we already force deleted everything !
	// why would an alive mod be added to the respawn list ? because lootable mobs exist (dead and not dead)
	_respawnObjects.clear();

	_playerCount = 0;
	_loaded = false;
}


void MapCell::LoadObjects(CellSpawnsVect * sp)
{
	_loaded = true;
	Instance * pInstance = _mapmgr->pInstance;
	InstanceBossInfoMap *bossInfoMap = objmgr.m_InstanceBossInfoMap[_mapmgr->GetMapId()];

	uint32 instance_difficulty_mask = 1 << ( this->_mapmgr->instance_difficulty + 2 );
	if( IS_BG_OR_ARENA( this->_mapmgr->GetMapInfo() ) )
		instance_difficulty_mask |= INSTANCE_FLAG_PVP;
	if( IS_STATIC_MAP( this->_mapmgr->GetMapInfo() ) )
		instance_difficulty_mask |= INSTANCE_FLAG_STATIC_MAP;

	//when an object enters this cell, we will reserve this amount of "inrange slots"
	ObjectCountToReserveOnEnter = sp->CreatureSpawns.GetMaxSize() + sp->GOSpawns.GetMaxSize();

	uint32 max_size = sp->CreatureSpawns.GetMaxSize();
	for(uint32 ti=0;ti<max_size;ti++)
	{
		CreatureSpawn *ts = sp->CreatureSpawns.GetValue( ti ); //!! this might get to be NULL !

		//not all spawns are spawned in all difficulty modes
		if( ( ts->difficulty_spawnmask & instance_difficulty_mask ) == 0 )
			continue;

		uint32 respawnTimeOverride = 0;
		if( _mapmgr->pInstance )	//wtf ? map is getting unloaded while we are adding objects to it ?
		{
			if(bossInfoMap != NULL && IS_PERSISTENT_INSTANCE(pInstance))
			{
				bool skip = false;
				for(std::set<uint32>::iterator killedNpc = pInstance->m_killedNpcs.begin(); killedNpc != pInstance->m_killedNpcs.end(); ++killedNpc)
				{
					// Do not spawn the killed boss.
					if((*killedNpc) == ts->entry)
					{
						skip = true;
						break;
					}
					// Do not spawn the killed boss' trash.
					InstanceBossInfoMap::const_iterator bossInfo = bossInfoMap->find((*killedNpc));
					if (bossInfo != bossInfoMap->end() && bossInfo->second->trash.find(ts->id) != bossInfo->second->trash.end())
					{
						skip = true;
						break;
					}
				}
				if(skip)
					continue;

				for(InstanceBossInfoMap::iterator bossInfo = bossInfoMap->begin(); bossInfo != bossInfoMap->end(); ++bossInfo)
					if(pInstance->m_killedNpcs.find(bossInfo->second->creatureid) == pInstance->m_killedNpcs.end() && bossInfo->second->trash.find(ts->id) != bossInfo->second->trash.end())
						respawnTimeOverride = bossInfo->second->trashRespawnOverride;
			}
			else
			{
				// No boss information available ... fallback ...
				if(pInstance->m_killedNpcs.find(ts->id) != pInstance->m_killedNpcs.end())
					continue;
			}
		}

		Creature * c;
		CreatureProto *p = CreatureProtoStorage.LookupEntry( ts->entry );
		if( p == NULL )
			continue;	//happens somehow on map shutdown / cell reactivate. Really rare and related to GM spawn play
		c =_mapmgr->CreateCreature(ts->entry);

		c->SetMapId(_mapmgr->GetMapId());
		c->SetInstanceID(_mapmgr->GetInstanceID());
		c->m_loadedFromDB = true;
		if(respawnTimeOverride > 0)
			c->m_respawnTimeOverride = respawnTimeOverride;

        if(c->Load(ts, _mapmgr->instance_difficulty, _mapmgr->GetMapInfo()))
		{
			c->m_noRespawn = false;	//only map spawns respawn, all rest get deleted
			if(!c->CanAddToWorld())
			{
				delete c;
				c = NULL;
			}
			else 
				c->PushToWorld(_mapmgr);
		}
		else
		{
			delete c;//missing proto or smth of that kind
			c = NULL;
		}
	}

	max_size = sp->GOSpawns.GetMaxSize();
	for(uint32 ti=0;ti<max_size;ti++)
	{
		GOSpawn *ts = sp->GOSpawns.GetValue( ti );//!! this might get to be NULL !
		GameObject *go=_mapmgr->CreateGameObject(ts->entry);
		go->SetInstanceID(_mapmgr->GetInstanceID());
		if(go->Load(ts))
		{
			//uint32 state = go->GetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE);

			// FIXME - burlex
			/*
			if(pInstance && pInstance->FindObject((*i)->stateNpcLink))
			{
				go->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, (state ? 0 : 1));
			}*/			   

			go->m_noRespawn = false;	//only map spawns respawn, all rest get deleted
			go->m_loadedFromDB = true;
			go->PushToWorld(_mapmgr);
		}
		else
		{
			delete go;//missing proto or smth of that kind
			go = NULL;
		}
	}
}


void MapCell::QueueUnloadPending()
{
	if(_unloadpending)
	{ 
		return;
	}

	if( _mapmgr->_shutdown == true )
	{ 
		return;
	}

	_unloadpending = true;
	//Log.Debug("MapCell", "Queueing pending unload of cell %u %u", _x, _y);
	sEventMgr.AddEvent(_mapmgr, &MapMgr::UnloadCell,(uint32)_x,(uint32)_y,MAKE_CELL_EVENT(_x,_y),sWorld.map_unload_time * 1000,1,0);
}

void MapCell::CancelPendingUnload()
{
	//Log.Debug("MapCell", "Cancelling pending unload of cell %u %u", _x, _y);
	if(!_unloadpending)
	{ 
		return;
	}

	sEventMgr.RemoveEvents(_mapmgr,MAKE_CELL_EVENT(_x,_y));
	_unloadpending = false;
}

void MapCell::Unload()
{
	//Log.Debug("MapCell", "Unloading cell %u %u", _x, _y);
	ASSERT(_unloadpending);
	_unloadpending=false;
	if(_active)
	{ 
		return;
	}
	RemoveObjects();
}

//this function is only called when we have mapcell unloading enabled 
bool MapCell::CanUnload()
{
	if( _mapmgr->_shutdown == true )
	{ 
		return false;
	}

	if( _playerCount )
	{ 
		return false;
	}

	ObjectSet::iterator itr;
	//This time it's simpler! We just remove everything :)
	for(itr = _objects.begin(); itr != _objects.end(); )
	{
		Object *obj = (*itr);
		itr++;
		if(!obj)
			continue;

		if(obj->GetTypeFromGUID() == HIGHGUID_TYPE_TRANSPORTER)
		{ 
			return false;
		}
		if(obj->GetTypeId()==TYPEID_CORPSE && obj->GetUInt32Value(CORPSE_FIELD_OWNER) != 0)
		{ 
			return false;
		}
		if(!obj->m_loadedFromDB) //all kind of summons
		{ 
			return false;
		}
	}

	if( _mapmgr->m_forcedcells.find( this ) != _mapmgr->m_forcedcells.end() )
		return false;

	return true;
}
