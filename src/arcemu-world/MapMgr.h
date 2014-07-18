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
// MapMgr.h
//

#ifndef __MAPMGR_H
#define __MAPMGR_H

//#define IS_PERSISTENT_INSTANCE(p) ( ((p)->m_mapInfo->type == INSTANCE_MULTIMODE && IS_HEROIC_INSTANCE_DIFFICULTIE((p)->instance_difficulty)) || (p)->m_mapInfo->type == INSTANCE_RAID )
#define IS_PERSISTENT_INSTANCE(p)	( IS_MAP_RAID((p)->m_mapInfo ) )
#define IS_PERSISTENT_MAP(p)		(( p->type_flags & (INSTANCE_FLAG_DUNGEON_5_MEN_HEROIC|INSTANCE_FLAG_RAID_10_MEN|INSTANCE_FLAG_RAID_10_MEN_HEROIC|INSTANCE_FLAG_RAID_25_MEN|INSTANCE_FLAG_RAID_25_MEN_HEROIC) )!=0)
#define IS_RESETABLE_INSTANCE(p)	( !(p)->m_persistent && ( IS_MAP_DUNGEON( (p)->m_mapInfo ) || ( IS_MAP_RAID( (p)->m_mapInfo ) && IS_NORMAL_INSTANCE_DIFFICULTIE( (p)->instance_difficulty ) )))
#define CHECK_INSTANCE_GROUP(p,g)	( (p)->m_creatorGroup == 0 || ((g) && (p)->m_creatorGroup == (g)->GetID()) )
#define IS_STATIC_MAP(p)			(( p->type_flags & INSTANCE_FLAG_STATIC_MAP )!=0)
#define IS_BG_OR_ARENA(p)			(( p->type_flags & INSTANCE_FLAG_PVP )!=0)
#define IS_DUEL_ENABLED_MAP(p)		(( p->type_flags & INSTANCE_FLAG_STATIC_MAP )!=0)
#define HAS_GRAVEYARDS_MAP(p)		(( p->type_flags & (INSTANCE_FLAG_STATIC_MAP|INSTANCE_FLAG_PVP) )!=0)
#define DISABLE_PVP_TOGGLE_MAP(p)	(( p->type_flags & INSTANCE_FLAG_PVP )!=0)
#define CAN_USE_FLYINGMOUNT(p)		( ( p->GetMapInfo()->type_flags & INSTANCE_FLAG_DISABLE_FLIGHT ) == 0 )
#define GO_GUID_RECYCLE_INTERVAL	2048	//client will cache GO positions. Using same guid for same client will make GO appear at wrong possition so we try to avoid assigning same guid

class MapCell;
class Map;
class Object;
class MapScriptInterface;
class WorldSession;
class GameObject;
class Creature;
class Player;
class Pet;
class Transporter;
class Corpse;
class CBattleground;
class Instance;
class InstanceScript;
class Vehicle;
class MultiBoxDetector;

enum MapMgrTimers
{
	MMUPDATE_OBJECTS = 0,
	MMUPDATE_SESSIONS = 1,
	MMUPDATE_FIELDS = 2,
	MMUPDATE_IDLE_OBJECTS = 3,
	MMUPDATE_ACTIVE_OBJECTS = 4,
	MMUPDATE_COUNT = 5
};

enum ObjectActiveState
{
	OBJECT_STATE_NONE	 = 0,
	OBJECT_STATE_INACTIVE = 1,
	OBJECT_STATE_ACTIVE   = 2,
};

typedef std::set<Object *> ObjectSet;
typedef std::set<Object *> UpdateQueueSet;	//quick check for duplicats
typedef std::list<Object *> UpdateQueueList;	//make sure update order is respected for A9 packets
typedef std::set<Player *> PUpdateQueue;
//typedef std::set<Player *> PlayerSet;
typedef HM_NAMESPACE::hash_map<uint32, Object*> StorageMap;
typedef set<uint64> CombatProgressMap;
typedef set<Creature*> CreatureSet;
typedef set<GameObject*> GameObjectSet;
typedef HM_NAMESPACE::hash_map<uint32, Creature*> CreatureSqlIdMap;
typedef HM_NAMESPACE::hash_map<uint32, GameObject*> GameObjectSqlIdMap;

#define CALL_INSTANCE_SCRIPT_EVENT( Mgr, Func ) if ( Mgr != NULL && Mgr->GetScript() != NULL ) Mgr->GetScript()->Func 

#define MAX_TRANSPORTERS_PER_MAP 25

class Transporter;
#define RESERVE_EXPAND_SIZE 1024

SERVER_DECL uint32 GetProbableZoneIdForLocation(uint32 map_id, float x, float y, bool UnknownArea );

class SERVER_DECL MapMgr : public CellHandler <MapCell>, public EventableObject,public CThread
{
	friend class UpdateObjectThread;
	friend class ObjectUpdaterThread;
	friend class MapCell;
	friend class MapScriptInterface;
public:
		
	//This will be done in regular way soon
	std::set< MapCell * > m_forcedcells;

	void AddForcedCell( MapCell * c );
	void RemoveForcedCell( MapCell * c );

	Mutex m_objectinsertlock;
	ObjectSet m_objectinsertpool;
	void AddObject(Object *);

#define LIMIT_MAX_SPAWN_COUNT_ON_1_MAP 500000 //just to debug broken input parameter

////////////////////////////////////////////////////////
// Local (mapmgr) storage/generation of GameObjects
/////////////////////////////////////////////
	uint32 m_GOArraySize;
	uint32 m_GOHighGuid;
	GameObject ** m_GOStorage;
	GameObject * CreateGameObject(uint32 entry);
	GameObject * CreateAndSpawnGameObject(uint32 entryID, float x, float y, float z, float o, float scale, bool TempSpawn = false);

	ARCEMU_INLINE uint32 ExtendGOStorage( uint32 NewGUID );
	ARCEMU_INLINE uint32 GenerateGameobjectGuid()
	{ 
		uint32 ret = m_GOHighGuid;
		m_GOHighGuid++;
		return ret; 
	}
	ARCEMU_INLINE GameObject * GetGameObject(const uint64 guid)
	{
		uint32 tguid = (uint32)(guid & LOWGUID_ENTRY_MASK);
		if(tguid > m_GOHighGuid)
			return 0;
		GameObject *ret = m_GOStorage[tguid];
#ifdef _DEBUG
		if( ret ) 
			ASSERT( ret->IsGameObject() );
#endif
		return ret;
	}
	ARCEMU_INLINE GameObject * GetGameObjectByEntry(const uint32 entry)
	{
		for(uint32 i=0;i<m_GOHighGuid;i++)
			if( m_GOStorage[i] && m_GOStorage[i]->deleted == false && m_GOStorage[i]->GetEntry() == entry )
				return m_GOStorage[i];
		return NULL;
	}

/////////////////////////////////////////////////////////
// Local (mapmgr) storage/generation of Creatures
/////////////////////////////////////////////
	uint32 m_CreatureArraySize;
	uint32 m_CreatureHighGuid;
	Creature ** m_CreatureStorage;
	Creature * CreateCreature(uint32 entry,bool FullInit=false,float x=0.0f,float y=0.0f,float z=0.0f,float o=0.0f);

	ARCEMU_INLINE uint32 ExtendCreatureStorage( uint32 NewGUID );
	ARCEMU_INLINE Creature * GetCreature(const uint64 guid)
	{
		uint32 tguid = (uint32)(guid & LOWGUID_ENTRY_MASK);
		if(tguid > m_CreatureHighGuid)
			return NULL;
		Creature *ret = m_CreatureStorage[tguid];
#ifdef _DEBUG
		if( ret ) 
			ASSERT( ret->IsCreature() );
#endif
		return ret;
	}

	ARCEMU_INLINE Creature * GetCreatureByEntry(const uint32 entry)
	{
		for(uint32 i=0;i<m_CreatureHighGuid;i++)
			if( m_CreatureStorage[i] && m_CreatureStorage[i]->deleted == false && m_CreatureStorage[i]->GetEntry() == entry )
				return m_CreatureStorage[i];
		return NULL;
	}
	Vehicle * GetSqlIdVehicle(uint32 sqlid);
//////////////////////////////////////////////////////////
// Local (mapmgr) storage/generation of DynamicObjects
////////////////////////////////////////////
	uint32 m_DynamicObjectHighGuid;
	typedef HM_NAMESPACE::hash_map<uint32, DynamicObject*> DynamicObjectStorageMap;
	DynamicObjectStorageMap m_DynamicObjectStorage;
	DynamicObject * CreateDynamicObject();
	
	ARCEMU_INLINE DynamicObject * GetDynamicObject(const uint64 guid)
	{
		uint32 tguid = guid & LOWGUID_ENTRY_MASK;
		DynamicObjectStorageMap::iterator itr = m_DynamicObjectStorage.find( tguid );
		DynamicObject *ret = (itr != m_DynamicObjectStorage.end()) ? itr->second : 0;
#ifdef _DEBUG
		if( ret ) 
			ASSERT( ret->IsDynObj() );
#endif
		return ret;
	}

//////////////////////////////////////////////////////////
// Local (mapmgr) storage of pets
///////////////////////////////////////////
	typedef HM_NAMESPACE::hash_map<uint32, Pet*> PetStorageMap;
	PetStorageMap m_PetStorage;
	ARCEMU_INLINE Pet * GetPet(const uint64 guid)
	{
		uint32 tguid = (uint32)(guid & LOWGUID_ENTRY_MASK);
		PetStorageMap::iterator itr = m_PetStorage.find( tguid );
		Pet *ret = (itr != m_PetStorage.end()) ? itr->second : 0;
#ifdef _DEBUG
		if( ret ) 
			ASSERT( ret->IsPet() );
#endif
		return ret;
	}

//////////////////////////////////////////////////////////
// Local (mapmgr) storage of players for faster lookup
////////////////////////////////
    
    // double typedef lolz// a compile breaker..
	typedef HM_NAMESPACE::hash_map<uint32, Player*>                     PlayerStorageMap;
	PlayerStorageMap m_PlayerStorage;
	ARCEMU_INLINE Player * GetPlayer(const uint64 guid)
	{
		uint32 tguid = (uint32)(guid & LOWGUID_ENTRY_MASK);
		PlayerStorageMap::iterator itr = m_PlayerStorage.find( tguid );
		Player *ret = (itr != m_PlayerStorage.end()) ? itr->second : 0;
#ifdef _DEBUG
		if( ret ) 
			ASSERT( ret->IsPlayer() );
#endif
		return ret;
	}

//////////////////////////////////////////////////////////
// Local (mapmgr) storage of combats in progress
////////////////////////////////
	CombatProgressMap _combatProgress;
	void AddCombatInProgress(uint64 guid)
	{
		_combatProgress.insert(guid);
	}
	void RemoveCombatInProgress(uint64 guid)
	{
		_combatProgress.erase(guid);
	}

//////////////////////////////////////////////////////////
// Lookup Wrappers
///////////////////////////////////
	Unit * GetUnit(const uint64 guid);
//	Vehicle * GetVehicle(const uint64 guid);
	Object * _GetObject(const uint64 guid);

	bool run();
	bool Do();

	MapMgr(Map *map, uint32 mapid, uint32 instanceid);
	~MapMgr();

	void PushObject(Object *obj);
	void PushStaticObject(Object * obj, bool StaticOnly = false);
	void RemoveStaticObject(Object * obj, bool StaticOnly = false);
	void RemoveObject(Object *obj, bool free_guid);
	void ChangeObjectLocation(Object *obj); // update inrange lists
	void ChangeFarsightLocation(Player *plr, DynamicObject *farsight);

	//! Mark object as updated
	void ObjectUpdated(Object *obj);
	void UpdateCellActivity(int32 x, int32 y, int32 radius, int8 ForceCreateStatic=0);

	// Terrain Functions
	float  GetLandHeight(float x, float y,float z=-10000);
//	ARCEMU_INLINE float  GetWaterHeight(float x, float y) { return GetBaseMap()->GetWaterHeight(x, y); }
//	ARCEMU_INLINE uint8  GetWaterType(float x, float y) { return GetBaseMap()->GetWaterType(x, y); }
//	ARCEMU_INLINE uint8  GetWalkableState(float x, float y) { return GetBaseMap()->GetWalkableState(x, y); }
	uint16 GetAreaID(float x, float y);

	ARCEMU_INLINE uint32 GetMapId() { return _mapId; }

	void PushToProcessed(Player* plr);

	ARCEMU_INLINE bool HasPlayers() { return (m_PlayerStorage.size() > 0); }
	ARCEMU_INLINE bool IsCombatInProgress() { return (_combatProgress.empty() == false); }
	void TeleportPlayers();

	ARCEMU_INLINE uint32 GetInstanceID() { return m_instanceID; }
	ARCEMU_INLINE MapInfo *GetMapInfo() { return pMapInfo; }

	bool _shutdown;

	ARCEMU_INLINE MapScriptInterface * GetInterface() { return ScriptInterface; }
	virtual int32 event_GetInstanceID() { return m_instanceID; }

	void LoadAllCells();
	ARCEMU_INLINE size_t GetPlayerCount() { return m_PlayerStorage.size(); }
	ARCEMU_INLINE size_t GetPlayerCountPending() { return (m_PlayerStorage.size()+m_objectinsertpool.size()); }
	uint32 GetTeamPlayersCount(uint32 teamId);

	void _PerformObjectDuties();
	uint32 mLoopCounter;
	uint32 lastGameobjectUpdate;
	uint32 lastUnitUpdate;

	time_t InactiveMoveTime;
    uint32 instance_difficulty;

	void UnloadCell(uint32 x,uint32 y);
//	void EventRespawnCreature(Creature * c, MapCell * p);
	void EventRespawnCreature(Creature * c, float pos_x,float pos_y);
//	void EventRespawnGameObject(GameObject * o, MapCell * c);
	void EventRespawnGameObject(GameObject * o, float pos_x,float pos_y);
	void SendMessageToCellPlayers(Object * obj, WorldPacket * packet, uint32 cell_radius = 2);
	void SendChatMessageToCellPlayers(Object * obj, WorldPacket * packet, uint32 cell_radius, uint32 langpos, int32 lang, WorldSession * originator);

	Instance * pInstance;
	void BeginInstanceExpireCountdown();
	void HookOnAreaTrigger(Player * plr, uint32 id);

	ARCEMU_INLINE void SetWorldState(uint32 state, uint32 value);
	ARCEMU_INLINE uint32 GetWorldState(uint32 state);
	void		SendInitialWorldState(Player *plr, bool delayed = true);
	
	// better hope to clear any references to us when calling this :P
	void InstanceShutdown()
	{
		pInstance = NULL;
		SetThreadState(THREADSTATE_TERMINATE);
	}

	// kill the worker thread only
	void KillThread()
	{
		pInstance=NULL;
		thread_kill_only = true;
		SetThreadState(THREADSTATE_TERMINATE);
		while(thread_running)
		{
			Sleep(100);
		}
	}

#ifdef FORCED_SERVER_KEEPALIVE
	void	KillThreadWithCleanup();
	void	TeleportCorruptedPlayers();	//we have to be prepared something is corrupted here
#endif

protected:

	//! Collect and send updates to clients
	void _UpdateObjects();

private:
	//! Objects that exist on map
 
	uint32 _mapId;
	std::set<Object *> _mapWideStaticObjects;

	std::map<uint32,uint32> _worldStateSet;

	bool _CellActive(uint32 x, uint32 y);
	void UpdateInRangeSet( Object *obj, Player *plObj, MapCell* cell );

public:
	// Distance a Player can "see" other objects and receive updates from them (!! ALREADY dist*dist !!)
	float m_UpdateDistance;

	// Script related
	InstanceScript* GetScript() { return mInstanceScript; };
	void LoadInstanceScript();
	void CallScriptUpdate();

	typedef std::map<uint32, void*> ExtensionSet;
	ExtensionSet m_extensions;

	void SetExtension(const char *name, void* ptr)
	{
		uint32 ID = crc32((const unsigned char*)name, (unsigned int)strlen(name));
		SetExtension( ID, ptr );
	}
	ARCEMU_INLINE void SetExtension(uint32 ID, void* ptr)
	{
		void *cur_value = GetExtension( ID );
		if( ptr == NULL )
		{
			if( cur_value != NULL )
			{
//				*cur_value = 0;	//just mark value as destroyed for debugging
				delete cur_value;
			}
			m_extensions.erase( ID );
		}
		else 
		{
			//find old one and delete it
			if( cur_value != NULL && ptr != cur_value )
			{
//				*cur_value = 0;	//just mark value as destroyed for debugging
				delete cur_value;
				m_extensions.erase( ID );
				m_extensions[ID] = ptr;
			}
			else if( cur_value == NULL )
				m_extensions[ID] = ptr;
		}
	}

	void* GetExtension(const char *name)
	{
		uint32 ID = crc32((const unsigned char*)name, (unsigned int)strlen(name));
		return GetExtension( ID );
	}
	ARCEMU_INLINE int64* GetCreateIn64Extension(uint32 ID)
	{
		ExtensionSet::iterator itr = m_extensions.find( ID );
		if( itr == m_extensions.end() )
		{
			int64 *p = new int64;
			*p = 0;
			SetExtension( ID, p );
			return p;
		}
		else
			return (int64 *)(itr->second);
	}
	int64* GetCreateIn64Extension(const char *name)
	{
		uint32 ID = crc32((const unsigned char*)name, (unsigned int)strlen(name));
		return GetCreateIn64Extension( ID );
	}
	ARCEMU_INLINE void* GetExtension(uint32 ID)
	{
		ExtensionSet::iterator itr = m_extensions.find( ID );
		if( itr == m_extensions.end() )
			return (NULL);
		else
			return (itr->second);
	}

	//debugging a crash, this is probably useless later. The idea is : Object A seems object B, object B does not see A. Object B logs out => object A will see a deleted object
	ARCEMU_INLINE bool		IsActiveObject( Object *obj )  
	{ 
		return ( m_ActiveObjects.find( obj ) != m_ActiveObjects.end() ); 
		//! object can be a bad pointer, can't ask for guid
	}
	ARCEMU_INLINE void		AddActiveObject( Object *obj ) 
	{ 
		m_ActiveObjects.insert( obj ); 
	}
	ARCEMU_INLINE void		RemActiveObject( Object *obj ) 
	{ 
		m_ActiveObjects.erase( obj ); 
	}
	ARCEMU_INLINE bool		IsActivePlayer( Object *obj )  { return ( m_ActivePlayers.find( obj ) != m_ActivePlayers.end() ); }
	ARCEMU_INLINE void		AddActivePlayer( Object *obj ) { m_ActivePlayers.insert( obj ); }
	ARCEMU_INLINE void		RemActivePlayer( Object *obj ) { m_ActivePlayers.erase( obj ); }
	//just for the stats
	uint32	timer_exec_time,timer_exec_count;
private:
	std::set<Object*>		m_ActiveObjects;
	std::set<Object*>		m_ActivePlayers;
	InstanceScript* mInstanceScript;

	/* Update System */
	Mutex			m_updateMutex;		// use a user-mode mutex for extra speed
	UpdateQueueSet  _updates;			// make it a list to respect order
	PUpdateQueue	_processQueue;

	/* Sessions */
	//note that session update changes the list while sessions get updated. We create a duplicate list and remove sessions from original list
	ARCEMU_INLINE void RemoveSession( WorldSession * session )
	{
		SessionSet::iterator itr = Sessions.find( session );
		if( itr != Sessions.end() )
			Sessions.erase( itr );
	}
	SessionSet Sessions;

	/* Map Information */
	MapInfo *pMapInfo;
	uint32 m_instanceID;

	MapScriptInterface * ScriptInterface;

public:
#if (defined( WIN32 ) || defined( WIN64 ) )
	DWORD threadid;
#endif

	GameObjectSet activeGameObjects;
	CreatureSet activeCreatures;
	CreatureSet::iterator activeCreaturesItr;		//removing objects from list while iterating through it is bad
	EventableObjectHolder eventHolder;
	CBattleground * m_battleground;
	set<Corpse*> m_corpses;
	CreatureSqlIdMap _sqlids_creatures;
	GameObjectSqlIdMap _sqlids_gameobjects;

	Creature * GetSqlIdCreature(uint32 sqlid);
	GameObject * GetSqlIdGameObject(uint32 sqlid);
#ifndef DISABLE_GUID_RECYCLING
	deque<uint32> _reusable_guids_gameobject;
	deque<uint32> _reusable_guids_creature;
#endif
	bool		forced_expire;
	bool		thread_kill_only;
	bool		thread_running;
	ByteBuffer	temp_buff;	//used to create createblocks or updateblocks temporally
#ifdef USE_MULTIBOXING_DETECTOR
	MultiBoxDetector *MultiboxingDetector;
#endif
};

#endif
