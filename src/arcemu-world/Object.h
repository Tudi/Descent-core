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

#ifndef _OBJECT_H
#define _OBJECT_H

class Unit;
//!!! bytes 0,1,2 are global counters. 
//!!! bytes 3,4,5 are some personalization bytes. Like internal counter. Example pet number.
//!!! bytes 6,7 these are object type definitions. Myabe just byte 7 or maybe just last 1-2 bits
enum HIGHGUID_TYPE
{
	HIGHGUID_TYPE_ITEM				= 0x44000000, // do not seem to have entry in guid
	HIGHGUID_TYPE_CONTAINER			= 0x44000000, // it is same as item in 3.1.1 and probably should be the same in the future also cause we are not saving highguid in db		
	HIGHGUID_TYPE_UNIT				= 0xF1300000,
	HIGHGUID_TYPE_PET				= 0xF1400000, // found in 3.1.3 TBC, note that the lower 2 bytes is pet number !(also lowguid highest byte)
//	HIGHGUID_TYPE_VEHICLE			= 0xF1500000,
	HIGHGUID_TYPE_GAMEOBJECT		= 0xF1120000, // 
	HIGHGUID_TYPE_GAMEOBJECT_NO_COL	= 0xF1100000, // do not calculate collision for this gameobject
	HIGHGUID_TYPE_PLAYER			= 0x00000000,	//though i seen blizz put 1 for highguid emu has all over the place nerfing this to 32 bit value
	HIGHGUID_TYPE_DYNAMICOBJECT		= 0xF1000000,
	HIGHGUID_TYPE_TRANSPORTER		= 0xF1200000,
	HIGHGUID_TYPE_WAYPOINT			= 0x10000000,
	HIGHGUID_TYPE_CORPSE			= 0xF1010000,
//	HIGHGUID_TYPE_GUILD				= 0x1FF10000,	//true, but do we really need to complicate it ?
	HIGHGUID_GROUP					= 0x1F570000,
//===============================================
	HIGHGUID_TYPE_MASK				= 0xFFFF0000,
//	LOWGUID_ENTRY_MASK				= 0x00FFFFFF,		// this loops like once a month on my test realm thus object collisions and other crap
	LOWGUID_ENTRY_MASK				= 0xFFFFFFFF,		// some say guid is just 32 bit long number. Need this to identify pets ?
};

// !!! entry is inserted from high to low. Highest entry byte is on pos 5, then pos 4 ...
//#define REMOVE_ENTRY_FROM_GUID_MASK ((uint64)(0xFFFF000000FFFFFF))
#define GET_TYPE_FROM_GUID(x) (GUID_HIPART((x)) & HIGHGUID_TYPE_MASK)
#define GET_LOWGUID_PART(x) (GUID_LOPART((x)) & LOWGUID_ENTRY_MASK)
//for safety we could check to avoid converting 0 - novalue to a guid. Also note that due to some messup container might be different then simple item
#define CONVERT_LOWGUID_TO_FULLGUID_ITEM( x ) ((uint64)x | ( (uint64)HIGHGUID_TYPE_ITEM << 32 ) )
//#define GET_GUILD_GUID( GuidId ) ((uint64)((((uint64)HIGHGUID_TYPE_GUILD)<<32) | GuidId ))
#define GET_GUILD_GUID( GuidId ) ((uint64)GuidId)
#define GET_GUILD_ID( GuidGUID ) ( (uint32)(GuidGUID) )
#define GET_PLAYER_GUID( GuidId ) ((uint64)((((uint64)HIGHGUID_TYPE_PLAYER)<<32) | GuidId ))
// TODO: fix that type mess

enum TYPE {
	TYPE_OBJECT		 = 1,
	TYPE_ITEM		   = 2,
	TYPE_CONTAINER	  = 4,
	TYPE_UNIT		   = 8,
	TYPE_PLAYER		 = 16,
	TYPE_GAMEOBJECT	 = 32,
	TYPE_DYNAMICOBJECT  = 64,
	TYPE_CORPSE		 = 128,
	TYPE_AIGROUP		= 256,
	TYPE_AREATRIGGER	= 512,
	TYPE_IN_GUILD			= 0x10000,	//found it in 4.x client, DK has it from the start = not guild related 
};

//these are also used by client
enum TYPEID {
	TYPEID_OBJECT		= 0,
	TYPEID_ITEM		  = 1,
	TYPEID_CONTAINER	 = 2,
	TYPEID_UNIT		  = 3,
	TYPEID_PLAYER		= 4,
	TYPEID_GAMEOBJECT	= 5,
	TYPEID_DYNAMICOBJECT = 6,
	TYPEID_CORPSE		= 7,
	TYPEID_AIGROUP	   = 8,
	TYPEID_AREATRIGGER   = 9,
	TYPEID_UNUSED			= 10,//used to signal invalid reference (object dealocated but someone is still using it)
};

//almost same as previous object types. just not sent to client
enum Internal_emu_object_types
{
	INTERNAL_OBJECT_TYPE_NONE		= 0,
	INTERNAL_OBJECT_TYPE_ITEM		= 1,
	INTERNAL_OBJECT_TYPE_CONTAINER	= 2,
	INTERNAL_OBJECT_TYPE_CREATURE	= 4,
	INTERNAL_OBJECT_TYPE_PLAYER		= 8,
	INTERNAL_OBJECT_TYPE_VEHICLE	= 16,
	INTERNAL_OBJECT_TYPE_GAMEOBJECT	= 32,
	INTERNAL_OBJECT_TYPE_DYNOBJECT	= 64,
	INTERNAL_OBJECT_TYPE_CORPSE		= 128,
	INTERNAL_OBJECT_TYPE_PET		= 256,
	INTERNAL_OBJECT_TYPE_UNIT		= 512,		//used only for destructor of player, creature, pet
	INTERNAL_OBJECT_TYPE_ANY_VALID	= ( INTERNAL_OBJECT_TYPE_ITEM | INTERNAL_OBJECT_TYPE_CONTAINER | INTERNAL_OBJECT_TYPE_CREATURE | INTERNAL_OBJECT_TYPE_PLAYER | INTERNAL_OBJECT_TYPE_VEHICLE | INTERNAL_OBJECT_TYPE_GAMEOBJECT | INTERNAL_OBJECT_TYPE_DYNOBJECT | INTERNAL_OBJECT_TYPE_CORPSE | INTERNAL_OBJECT_TYPE_PET | INTERNAL_OBJECT_TYPE_UNIT ),
	INTERNAL_OBJECT_TYPE_NONE2		= 1024,		//to be able to check if we messed up something
};

enum OBJECT_UPDATE_TYPE {
	UPDATETYPE_VALUES = 0,
	UPDATETYPE_CREATE_OBJECT = 1, // can be same as create object, blizz simply does not have movement block for these object types
	UPDATETYPE_CREATE_OBJECT_SELF = 2,
	//  8 bytes - GUID
	//  1 byte - Object Type (*)
	//  Goto Position Update
	//  Goto Update Block
	UPDATETYPE_OUT_OF_RANGE_OBJECTS = 3 // <- this is correct, not sure about 3
	//  4 bytes - Count
	//  Loop Count Times:
	//  8 bytes - GUID

};

/*
#define CLIENT_FINISHED_CREATING_PLAYER		1
#define CLIENT_FINISHED_LOADING_MAP			2
#define CLIENT_FINISHED_CREATING_OBJECTS	4
#define CLIENT_FINISHED_LOADING ( CLIENT_FINISHED_CREATING_PLAYER | CLIENT_FINISHED_LOADING_MAP | CLIENT_FINISHED_CREATING_OBJECTS )
*/
#define CREATE_FLAG_FOR_SELF	0x01

enum PHASECOMMANDS 
{
	PHASE_SET = 0, //overwrites the phase value with the supplied one
	PHASE_ADD = 1, //adds the new bits to the current phase value
	PHASE_DEL = 2, //removes the given bits from the current phase value
	PHASE_RESET = 3 //sets the default phase of 1, same as PHASE_SET with 1 as the new value
};

//keep the order of this structure to show how final dmg is calculated
struct dealdamage
{ 
	dealdamage() 
	{ 
		memset( this, 0, sizeof( dealdamage ) ); 
		pct_mod_final_dmg = 100;	// 100% dmg = no mod
	}
	int32	base_dmg;			// base as weapon / spell dmg
	int32	flat_mod_base_dmg;	// SP or AP ...
	int32	pct_mod_base_dmg;	// % weapon dmg done. 
	int32	flat_mod_final_dmg;	// for combo spells like : and additional x dmg
	int32	pct_mod_final_dmg;	// final scale of the dmg like deal double dmg on demons. Mostly talents
	int32	pct_mod_crit;		// default is 0. Scales everything below
	uint32	school_type;		// required for external functions. Will be auto filled sometimes
	int32	full_damage;		// resulted after calculations
	int32	resisted_damage;	// resulted after calculations
	int32   absorbed_damage;	// resulted after calculations
	int32	SpellId;			// might not be set all the time !
	bool	StaticDamage;		// do not crit / mod this dmg. Just subtract resistances
	bool	DisableProc;		// procs do not proc other procs unless specified
	bool	FirstTick;			// valid for DOT dmg to be able to filter some of the procs that should be called only on first tick
};

class SharedObjectDeleteNotify
{
	SharedObjectDeleteNotify() 
	{ 
		ReferenceCounter = 2; 
	}
	void DecRef( ) 
	{ 
		ReferenceCounter--; 
		if( ReferenceCounter == 0 ) 
			delete this; 
	}
	void		(*NotifyObj1Obj2)( void *Obj1 ); //if A gets deleted then call notify of B
	void		(*NotifyObj2Obj1)( void *Obj2 ); //if B gets deleted then call notify of A
	signed char	ReferenceCounter; //volatile ? so it's thread safe ?
};

class WorldPacket;
class ByteBuffer;
class WorldSession;
class Player;
class MapCell;
class MapMgr;

#define A9_FLAGS2_FLYING			(1<<2)	//enable flight = no gravity
#define A9_FLAGS2_NOGRAVITY			0x80000
#define A9_FLAGS2_WALK_BACK			(1<<10)	//Michael Jackson in action
#define A9_FLAGS2_STRAFE_LEFT		(1<<11)	//players are actually running not just anim
#define A9_FLAGS2_WALK_MODE			(1<<18)	//you are walking as default and start running if enabled
#define A9_FLAGS2_DISABLE_JUMP_FALL	(1<<19)	//i was not able to jump or fall unless coliding with something
#define A9_FLAGS2_ROOTED			(1<<20)	//i can only rotate
#define A9_FLAGS2_FALLING			(1<<22)	//anim ?
#define A9_FLAGS2_WALK_BACK2		(1<<27)	//
#define A9_FLAGS2_STRAFE_LEFT2		(1<<28)	//
#define A9_FLAGS2_STRAFE_RIGHT		(1<<29)	//
#define A9_FLAGS2_ROTATE_LEFT		(1<<30)	//
#define A9_FLAGS2_ROTATE_RIGHT		(1<<31)	//

//this is defined so first 4 bytes will always represent the type of the extension no matter what that would be
class BaseObjectExtension
{
public:
	uint32 type;
};

class Object;

class InrangeLoopExitAutoCallback
{
public:
	InrangeLoopExitAutoCallback()
	{
		o = NULL;
	}
	SERVER_DECL ~InrangeLoopExitAutoCallback();
	Object *o;
#ifdef _DEBUG
	uint32	UnrollOrderCheck;
#endif
};

//====================================================================
//  Object
//  Base object for every item, unit, player, corpse, container, etc
//====================================================================
class SERVER_DECL Object : public EventableObject
{
public:
	typedef std::map<uint32, void*> ExtensionSet;

	Object (  );
	virtual ~Object ( );
	virtual void Virtual_Destructor( );

	ARCEMU_INLINE virtual void Update ( uint32 time );
  //! True if object exists in world
	
	ARCEMU_INLINE bool IsInWorld() { return m_mapMgr != NULL; }
	virtual void AddToWorld();
	virtual void AddToWorld(MapMgr * pMapMgr);
	void PushToWorld(MapMgr*);
	virtual void OnPushToWorld() { }
	virtual void OnPrePushToWorld() { }
	virtual void RemoveFromWorld(bool free_guid);
	uint8 GetType( ) { return ( ( GetUInt32Value(DYNAMICOBJECT_BYTES) >> ( 24 + 4 )) & 0x0F ); }
	uint16 GetSpellVisual( ) { return GetUInt32Value(DYNAMICOBJECT_BYTES); }

	// guid always comes first
	ARCEMU_INLINE const uint64& GetGUID() const { return *((uint64*)&m_uint32Values[ OBJECT_FIELD_GUID ] ); }
//	ARCEMU_INLINE const uint32 GetGUIDLow() const { return GET_LOWGUID_PART( m_uint32Values[ OBJECT_FIELD_GUID ] ); }

	ARCEMU_INLINE const WoWGuid& GetNewGUID() const { return m_wowGuid; }
	ARCEMU_INLINE uint32 GetEntry(){return m_uint32Values[ OBJECT_FIELD_ENTRY ];}
	ARCEMU_INLINE float GetScale(){return m_floatValues[ OBJECT_FIELD_SCALE_X ];}
	ARCEMU_INLINE void SetScale(float scale){ SetFloatValue(OBJECT_FIELD_SCALE_X , scale );}
	
//	ARCEMU_INLINE const uint32 GetEntryFromGUID() const
//	{
/*		uint64 entry = *(uint64*)m_uint32Values;
		entry >>= 24;
		return (uint32)(entry & 0xFFFFFFFF);*/

//		return uint32( (*(uint64*)m_uint32Values >> 24) & 0xFFFFFFFF );
//	}

	ARCEMU_INLINE const uint32 GetTypeFromGUID() const { return (m_uint32Values[ OBJECT_FIELD_GUID + 1 ] & HIGHGUID_TYPE_MASK); }
	ARCEMU_INLINE const uint32 GetUIdFromGUID() const { return (m_uint32Values[ OBJECT_FIELD_GUID ] & LOWGUID_ENTRY_MASK); }
//	ARCEMU_INLINE const uint32 GetLowGUID() const { return (m_uint32Values[ OBJECT_FIELD_GUID ] & LOWGUID_ENTRY_MASK); }
	ARCEMU_INLINE const uint32 GetLowGUID() const { return (m_uint32Values[ OBJECT_FIELD_GUID ]); }

	// type
	ARCEMU_INLINE const uint8& GetTypeId() const { return m_objectTypeId; }
	ARCEMU_INLINE bool IsUnit()	{ return 0!=( internal_object_type & ( INTERNAL_OBJECT_TYPE_UNIT | INTERNAL_OBJECT_TYPE_CREATURE | INTERNAL_OBJECT_TYPE_VEHICLE | INTERNAL_OBJECT_TYPE_PET | INTERNAL_OBJECT_TYPE_PLAYER ) ); }
	ARCEMU_INLINE bool IsPlayer() { return 0!=(internal_object_type & INTERNAL_OBJECT_TYPE_PLAYER); }
	ARCEMU_INLINE bool IsCreature() { return 0!=(internal_object_type & ( INTERNAL_OBJECT_TYPE_CREATURE | INTERNAL_OBJECT_TYPE_VEHICLE | INTERNAL_OBJECT_TYPE_PET ) ); }
//	ARCEMU_INLINE bool IsVehicle() { return 0!=(internal_object_type & INTERNAL_OBJECT_TYPE_VEHICLE); }
	ARCEMU_INLINE bool IsGameObject() { return 0!=(internal_object_type & INTERNAL_OBJECT_TYPE_GAMEOBJECT); }
	ARCEMU_INLINE bool IsCorpse() { return 0!=(internal_object_type & INTERNAL_OBJECT_TYPE_CORPSE); }
	ARCEMU_INLINE bool IsItem() { return 0 != (internal_object_type & ( INTERNAL_OBJECT_TYPE_ITEM | INTERNAL_OBJECT_TYPE_CONTAINER ) ); }
	ARCEMU_INLINE bool IsContainer() { return 0 != (internal_object_type & INTERNAL_OBJECT_TYPE_CONTAINER ); }
	ARCEMU_INLINE bool IsDynObj() { return 0 != (internal_object_type & INTERNAL_OBJECT_TYPE_DYNOBJECT ); }
	bool IsPet();

	//! This includes any nested objects we have, inventory for example.
	virtual uint32 __fastcall BuildCreateUpdateBlockForPlayer( ByteBuffer *data, Player *target );
	uint32 __fastcall BuildValuesUpdateBlockForPlayer( ByteBuffer *buf, Player *target );
	uint32 __fastcall BuildValuesUpdateBlockForPlayer( ByteBuffer * buf, UpdateMask * mask );
	uint32 __fastcall BuildOutOfRangeUpdateBlock( ByteBuffer *buf );

	WorldPacket* BuildFieldUpdatePacket(uint32 index,uint32 value);
	void BuildFieldUpdatePacket(Player* Target, uint32 Index, uint32 Value);
	void BuildFieldUpdatePacket(ByteBuffer * buf, uint32 Index, uint32 Value);

	void DealDamage(Unit *pVictim, uint32 damage, uint32 spellId, bool no_remove_auras = false);

	virtual void DestroyForPlayer( Player *target, uint8 anim = 0 ) const;

//	void BuildHeartBeatMsg( WorldPacket *data ) const;
	void BuildTeleportAckMsg( const LocationVector & v);
	bool IsBeingTeleported() { return mSemaphoreTeleport; }
	void SetSemaphoreTeleport(bool semphsetting) { mSemaphoreTeleport = semphsetting; }

	bool SetPosition( float newX, float newY, float newZ, float newOrientation, bool allowPorting = false );
	bool SetPosition( const LocationVector & v, bool allowPorting = false);
	void SetRotation( uint64 guid );

	ARCEMU_INLINE const float& GetPositionX( ) const { return m_position.x; }
	ARCEMU_INLINE const float& GetPositionY( ) const { return m_position.y; }
	ARCEMU_INLINE const float& GetPositionZ( ) const { return m_position.z; }
	ARCEMU_INLINE const float& GetOrientation( ) const { return m_position.o; }
	ARCEMU_INLINE void SetOrientation( float o ) { m_position.o = o; }
	ARCEMU_INLINE void SetPositionX( float x ) { m_position.z = x; }	//right now this does not require map changes
	ARCEMU_INLINE void SetPositionY( float y ) { m_position.z = y; }	//right now this does not require map changes
	ARCEMU_INLINE void SetPositionZ( float z ) { m_position.z = z; }	//right now this does not require map changes

	ARCEMU_INLINE const LocationVector & GetPosition() { return m_position; }
	ARCEMU_INLINE LocationVector & GetPositionNC() { return m_position; }
	ARCEMU_INLINE LocationVector * GetPositionV() { return &m_position; }

	//Distance Calculation
	float CalcDistance(Object* Ob);
	float CalcDistance(float ObX, float ObY, float ObZ);
	float CalcDistance(Object *Oa, Object *Ob);
	float CalcDistance(Object *Oa, float ObX, float ObY, float ObZ);
	float CalcDistance(float OaX, float OaY, float OaZ, float ObX, float ObY, float ObZ);
	// scriptdev2
	bool IsInMap( Object* obj) { return GetMapId()==obj->GetMapId() && GetInstanceID()==obj->GetInstanceID(); }
 	bool IsWithinDistInMap( Object* obj, const float dist2compare) const;
	bool IsWithinLOSInMap( Object* obj);
	bool IsWithinLOS( LocationVector location  );

	//! Only for MapMgr use
	ARCEMU_INLINE MapCell* GetMapCell() const { return m_mapCell; }
	//! Only for MapMgr use
	ARCEMU_INLINE void SetMapCell(MapCell* cell) { m_mapCell = cell; }
	//! Only for MapMgr use
	ARCEMU_INLINE MapMgr* GetMapMgr() const { return m_mapMgr; }

	ARCEMU_INLINE void SetMapId(uint32 newMap) { m_mapId = newMap; }
	void SetZoneId(uint32 newZone);

	ARCEMU_INLINE const uint32 GetMapId( ) const { return m_mapId; }

	//! Get uint32 property
	ARCEMU_INLINE const uint32& GetUInt32Value( uint32 index ) const
	{
		ASSERT( index < m_valuesCount );

		return m_uint32Values[ index ];
	}
	ARCEMU_INLINE const int32& GetInt32Value( uint32 index ) const
	{
		ASSERT( index < m_valuesCount );

		return m_int32Values[ index ];
	}

	//! Get uint64 property
	ARCEMU_INLINE const uint64& GetUInt64Value( uint32 index ) const
	{
		ASSERT( index + uint32(1) < m_valuesCount );
		return *((uint64*)&(m_uint32Values[ index ]));
	}

	//! Get float property
	ARCEMU_INLINE const float& GetFloatValue( uint32 index ) const
	{
		ASSERT( index < m_valuesCount );
		return m_floatValues[ index ];
	}

	void __fastcall ModFloatValue(const uint32 index, const float value );
	void ModFloatValueByPCT(const uint32 index, int32 byPct );
	void ModSignedInt32Value(uint32 index, int32 value);
	void ModUnsigned32Value(uint32 index, int32 mod);
	uint32 GetModPUInt32Value(const uint32 index, const int32 value);

	//! Set uint32 property
	void SetByte(uint32 index, uint32 index1,uint8 value);

	ARCEMU_INLINE uint8 GetByte(uint32 i,uint32 i1)
	{
		ASSERT( i < m_valuesCount);
		ASSERT(i1 < 4);
		return ((uint8*)m_uint32Values)[i*4+i1];
	}
	
	ARCEMU_INLINE void SetNewGuid(uint32 Guid)
	{
		SetUInt32Value(OBJECT_FIELD_GUID, Guid);
		m_wowGuid.Init(GetGUID());
	}

	void EventSetUInt32Value(uint32 index, uint32 value);
	void __fastcall SetUInt32Value( const uint32 index, const uint32 value );
	void __fastcall SetInt32Value( const uint32 index, const int32 value );

	//! Set uint64 property
	void __fastcall SetUInt64Value( const uint32 index, const uint64 value );

	//! Set float property
	void __fastcall SetFloatValue( const uint32 index, const float value );

	void __fastcall SetByteFlag( const uint32 index, const uint32 byte, uint8 newFlag);
	bool __fastcall HasByteFlag( const uint32 index, const uint32 byte, uint8 checkFlag);
	void __fastcall RemoveByteFlag(const uint32 index, const uint32 byte, uint8 checkFlag);
	void __fastcall SetFlag( const uint32 index, uint32 newFlag );

	void __fastcall RemoveFlag( const uint32 index, uint32 oldFlag );
	void RemoveFlagForceUpdate( uint32 index, uint32 oldFlag );
	void ToggleFlagForceUpdate( uint32 index, uint32 oldFlag );

	ARCEMU_INLINE bool HasFlag( const uint32 index, uint32 flag ) const
	{
		ASSERT( index < m_valuesCount );
		return (m_uint32Values[ index ] & flag) != 0;
	}
	
	////////////////////////////////////////
	void ClearUpdateMask( )
	{
		m_updateMask.Clear();
		m_objectUpdated = false;
	}

	uint8 HasUpdateField(uint32 index)
	{
		ASSERT( index < m_valuesCount);
		return m_updateMask.GetBit(index);
	}
	void SetUpdateMask( uint32 index );

	//use it to check if a object is in range of another
	bool isInRange(Object* target, float range);

	// Use it to Check if a object is in front of another one
	bool isInFront(Object* target);
	bool isInBack(Object* target);
	// Check to see if an object is in front of a target in a specified arc (in degrees)
	bool isInArc(Object* target , float degrees); 
	bool HasInArc( float degrees, Object* target );  // scriptdev2
	/* Calculates the angle between two Positions */
	float calcAngle( float Position1X, float Position1Y, float Position2X, float Position2Y );
	float calcRadAngle( float Position1X, float Position1Y, float Position2X, float Position2Y );

	/* converts to 360 > x > 0 */
	float getEasyAngle( float angle );

	ARCEMU_INLINE const float GetDistanceSq(Object* obj)
	{
		if(obj->GetMapId() != m_mapId) return 40000.0f; //enough for out of range
		return m_position.DistanceSq(obj->GetPosition());
	}

	ARCEMU_INLINE float GetDistanceSq(LocationVector & comp)
	{
		return comp.DistanceSq(m_position);
	}

	ARCEMU_INLINE float CalcDistance(LocationVector & comp)
	{
		return comp.Distance(m_position);
	}

	ARCEMU_INLINE const float GetDistanceSq(float x, float y, float z)
	{
		return m_position.DistanceSq(x, y, z);
	}

	ARCEMU_INLINE const float GetDistance2dSq( Object* obj )
	{
		if( obj->GetMapId() != m_mapId )
			return 40000.0f; //enough for out of range
		return m_position.Distance2DSq( obj->m_position );
	}

#ifndef USE_STATIC_SET
	// In-range object management, not sure if we need it
	ARCEMU_INLINE bool IsInRangeSet( Object* pObj )
	{
		return !( m_objectsInRange.find( pObj ) == m_objectsInRange.end() );
	}
	
	virtual void QueueAddInRangeObject(Object* pObj)
	{
		if( pObj == NULL )
			return;

//		if( pObj == this )
//			printf("!!!!we are in range of self !\n");

		if( m_UseQueueInrangeSetChange )
		{
			m_QueueAddObjectsInRange.insert( pObj );
			m_QueueRemoveObjectsInRange.erase( pObj );
		}
		else
		{
			//Zack: as far as i know list inserts do not corrupt iterators
			m_objectsInRange.insert( pObj );

			// NOTES: Since someone will come along and try and change it.
			// Don't reinrepret_cast has to be used static_cast will not work when we are
			// inside the class we are casting from if we want to cast up the inheritance
			// chain, as Object has no concept of Player.

			if( pObj->IsPlayer() )
				m_inRangePlayers.insert( reinterpret_cast< Player* >( pObj ) );
		}
		
	}

//	Mutex m_inrangechangelock;
	ARCEMU_INLINE void AquireInrangeLock()
	{ 
//		m_inrangechangelock.Acquire(); 
//		if( m_UseQueueInrangeSetChange == 0 )
//			QueueInrangeProcessPending();
//		m_UseQueueInrangeSetChange++;
	}
	ARCEMU_INLINE void ReleaseInrangeLock()
	{ 
//		m_inrangechangelock.Release(); 
//		m_UseQueueInrangeSetChange--;
//		if( m_UseQueueInrangeSetChange == 0 )
//			QueueInrangeProcessPending();
	}

/*	ARCEMU_INLINE void RemoveInRangeObject( Object* pObj )
	{
		if( pObj == NULL )
			return;

		AquireInrangeLock();
		OnRemoveInRangeObject( pObj );
		m_objectsInRange.erase( pObj );
		ReleaseInrangeLock();
	}/**/
	virtual void QueueRemoveInRangeObject( Object* pObj )
	{
//		if( pObj == NULL )
//			return;

//		AquireInrangeLock();
//		OnRemoveInRangeObject( pObj );
//		m_objectsInRange.erase( pObj );
//		ReleaseInrangeLock();
		if( m_UseQueueInrangeSetChange )
		{
			m_QueueRemoveObjectsInRange.insert( pObj );
			m_QueueAddObjectsInRange.erase( pObj );
		}
		else
		{
			if( pObj->IsPlayer() )
				m_inRangePlayers.erase( reinterpret_cast< Player* >( pObj ) );
			m_objectsInRange.erase( pObj );
		}
	}

	ARCEMU_INLINE bool HasInRangeObjects()
	{
		return ( m_objectsInRange.empty() == false );
	}
	ARCEMU_INLINE bool HasInRangePlayers()
	{
		return ( m_inRangePlayers.empty() == false );
	}

/*	virtual void OnRemoveInRangeObject( Object * pObj )
	{
		if( pObj->IsPlayer() )
//			ASSERT( m_inRangePlayers.erase( reinterpret_cast< Player* >( pObj ) ) == 1 );
			m_inRangePlayers.erase( reinterpret_cast< Player* >( pObj ) );
	} */

	//!! only use this when you are removing object from world or before pushing it in world
	virtual void ClearInRangeSet()
	{
		m_objectsInRange.clear();
		m_inRangePlayers.clear();
		//issues with current create / delete : rogue stealths and gets invisible, mage casts detection and rogue gets pushed into create object. 1 packet will contain destroy / create. Client gets bugged
//		m_QueueAddObjectsInRange.clear();
//		m_QueueRemoveObjectsInRange.clear();
//		m_oppFactsInRange.clear();
//		m_sameFactsInRange.clear();
	} 
	ARCEMU_INLINE void _AutoReleaseInrangeLock();
	ARCEMU_INLINE void _AutoAquireInrangeLock( InrangeLoopExitAutoCallback &Tracker );

	ARCEMU_INLINE size_t GetInRangeCount() { return m_objectsInRange.size(); }
	ARCEMU_INLINE size_t GetInRangePlayersCount() { return m_inRangePlayers.size();}
	ARCEMU_INLINE InRangeSetRecProt::iterator GetInRangeSetBegin( InrangeLoopExitAutoCallback &Tracker ) 
	{ 
		_AutoAquireInrangeLock( Tracker );
		return m_objectsInRange.begin(); 
	}
	ARCEMU_INLINE InRangeSetRecProt::iterator GetInRangeSetEnd() { return m_objectsInRange.end(); }
//	ARCEMU_INLINE InRangeSetRecProt::iterator FindInRangeSet(Object * obj) { return m_objectsInRange.find(obj); }

/*	void QueueRemoveInRangeObject(InRangeSetRecProt::iterator itr)
	{ 
		AquireInrangeLock();
		OnRemoveInRangeObject(*itr);
		m_objectsInRange.erase(itr);
		ReleaseInrangeLock();
	}*/

/*	ARCEMU_INLINE bool QueueRemoveIfInRange( Object * obj )
	{
		InRangeSetRecProt::iterator itr = m_objectsInRange.find(obj);
		if( obj->GetTypeId() == TYPEID_PLAYER )
			m_inRangePlayers.erase( reinterpret_cast< Player* >( obj ) );

		if( itr == m_objectsInRange.end() )
			return false;
		
		AquireInrangeLock();
		m_objectsInRange.erase( itr );
		ReleaseInrangeLock();
		return true;
	}*/

/*	ARCEMU_INLINE void QueueAddInRangePlayer( Object * obj )
	{
		m_inRangePlayers.insert( reinterpret_cast< Player* >( obj ) );
	}

	ARCEMU_INLINE void QueueRemoveInRangePlayer( Object * obj )
	{
		m_inRangePlayers.erase( reinterpret_cast< Player* >( obj ) );
	} */

//	bool IsInRangeSameFactSet(Object* pObj) { return (m_sameFactsInRange.count(pObj) > 0); }
//	void UpdateSameFactionSet();
//	ARCEMU_INLINE InRangeSetRecProt::iterator GetInRangeSameFactsSetBegin() { return m_sameFactsInRange.begin(); }
//	ARCEMU_INLINE InRangeSetRecProt::iterator GetInRangeSameFactsSetEnd() { return m_sameFactsInRange.end(); }

//	bool IsInRangeOppFactSet(Object* pObj) { return (m_oppFactsInRange.count(pObj) > 0); }
//	void UpdateOppFactionSet();
//	ARCEMU_INLINE size_t GetInRangeOppFactsSize(){ return m_oppFactsInRange.size(); }
//	ARCEMU_INLINE InRangeSetRecProt::iterator GetInRangeOppFactsSetBegin() { return m_oppFactsInRange.begin(); }
//	ARCEMU_INLINE InRangeSetRecProt::iterator GetInRangeOppFactsSetEnd() { return m_oppFactsInRange.end(); }
	ARCEMU_INLINE InRangePlayerSetRecProt::iterator GetInRangePlayerSetBegin( InrangeLoopExitAutoCallback &Tracker ) 
	{ 
		_AutoAquireInrangeLock( Tracker );
		return m_inRangePlayers.begin(); 
	}
	ARCEMU_INLINE InRangePlayerSetRecProt::iterator GetInRangePlayerSetEnd() { return m_inRangePlayers.end(); }
	ARCEMU_INLINE InRangePlayerSetRecProt * GetInRangePlayerSet() { return &m_inRangePlayers; };

	//after object update finished. Only in 1 place. Process the pending changes to not mess up iterators
	ARCEMU_INLINE void QueueInrangeProcessPending()
	{
		if( m_QueueAddObjectsInRange.empty() && m_QueueRemoveObjectsInRange.empty() )
			return;
		for( InRangeSetRecProt::iterator itr = m_QueueAddObjectsInRange.begin(); itr != m_QueueAddObjectsInRange.end(); itr++ )
		{
			if( (*itr)->IsPlayer() )
				m_inRangePlayers.insert( reinterpret_cast< Player* >(*itr) );
			m_objectsInRange.insert( (*itr) );
		}
		m_QueueAddObjectsInRange.clear();
		for( InRangeSetRecProt::iterator itr = m_QueueRemoveObjectsInRange.begin(); itr != m_QueueRemoveObjectsInRange.end(); itr++ )
		{
			if( (*itr)->IsPlayer() )
				m_inRangePlayers.erase( reinterpret_cast< Player* >(*itr) );
			m_objectsInRange.erase( (*itr) );
		}
		m_QueueRemoveObjectsInRange.clear();
	}
#else
	// In-range object management, not sure if we need it
	ARCEMU_INLINE bool IsInRangeSet( Object* pObj )
	{
		return !( m_objectsInRange.find( pObj ) == m_objectsInRange.end() );
	}
	
	ARCEMU_INLINE void ReserveObjectListSize( uint32 Size )
	{
		m_objectsInRange.EnsureSize( Size );
	}
	virtual void QueueAddInRangeObject(Object* pObj)
	{
		if( pObj == NULL )
			return;

//		if( pObj == this )
//			printf("!!!!we are in range of self !\n");

		//Zack: as far as i know list inserts do not corrupt iterators
		m_objectsInRange.insert( pObj );

		// NOTES: Since someone will come along and try and change it.
		// Don't reinrepret_cast has to be used static_cast will not work when we are
		// inside the class we are casting from if we want to cast up the inheritance
		// chain, as Object has no concept of Player.

		if( pObj->IsPlayer() )
			m_inRangePlayers.insert( reinterpret_cast< Player* >( pObj ) );
	}

	ARCEMU_INLINE void AquireInrangeLock()	
	{ 	
//		m_objectsInRange.RecursionAutoLockAquire();
//		m_inRangePlayers.RecursionAutoLockAquire();
	}
	ARCEMU_INLINE void ReleaseInrangeLock()	
	{ 	
//		m_objectsInRange.RecursionAutoLockRelease();
//		m_inRangePlayers.RecursionAutoLockRelease();
	}

	virtual void QueueRemoveInRangeObject( Object* pObj )
	{
		if( pObj->IsPlayer() )
			m_inRangePlayers.erase( reinterpret_cast< Player* >( pObj ) );
		m_objectsInRange.erase( pObj );
	}

	ARCEMU_INLINE bool HasInRangeObjects()
	{
		return ( m_objectsInRange.empty() == false );
	}
	ARCEMU_INLINE bool HasInRangePlayers()
	{
		return ( m_inRangePlayers.empty() == false );
	}
	//!! only use this when you are removing object from world or before pushing it in world
	virtual void ClearInRangeSet()
	{
		m_objectsInRange.clear();
		m_inRangePlayers.clear();
	} 
	ARCEMU_INLINE void _AutoReleaseInrangeLock();
	ARCEMU_INLINE void _AutoAquireInrangeLock( InrangeLoopExitAutoCallback &Tracker );

	ARCEMU_INLINE size_t GetInRangeCount() { return m_objectsInRange.size(); }
	ARCEMU_INLINE size_t GetInRangePlayersCount() { return m_inRangePlayers.size();}
	ARCEMU_INLINE InRangeSetRecProt::iterator GetInRangeSetBegin( InrangeLoopExitAutoCallback &Tracker ) 
	{ 
		_AutoAquireInrangeLock( Tracker );
		return m_objectsInRange.begin(); 
	}
	ARCEMU_INLINE InRangeSetRecProt::iterator GetInRangeSetEnd() { return m_objectsInRange.end(); }
	ARCEMU_INLINE InRangePlayerSetRecProt::iterator GetInRangePlayerSetBegin( InrangeLoopExitAutoCallback &Tracker ) 
	{ 
		_AutoAquireInrangeLock( Tracker );
		return m_inRangePlayers.begin(); 
	}
	ARCEMU_INLINE InRangePlayerSetRecProt::iterator GetInRangePlayerSetEnd() { return m_inRangePlayers.end(); }
	ARCEMU_INLINE InRangePlayerSetRecProt * GetInRangePlayerSet() { return &m_inRangePlayers; };
	ARCEMU_INLINE void QueueInrangeProcessPending()	{	}
	ARCEMU_INLINE int IsInrangeObjectSetLocked()	{ return m_objectsInRange.IsLockedForWrite();	}
#endif
	ARCEMU_INLINE void InrageConsistencyCheck(bool forced = false);
//	void __fastcall SendMessageToSet(WorldPacket *data, bool self,bool myteam_only=false);
//	ARCEMU_INLINE void SendMessageToSet(StackWorldPacket * data, bool self_also) { OutPacketToSet(data->GetOpcode(), data->GetSize(), data->GetBufferPointer(), self_also); }
	ARCEMU_INLINE void SendMessageToSet(WorldPacket * data, bool self_also=true, bool myteam_only=false, bool InWorldTargets = true) 
	{ 
		OutPacketToSet(data->GetOpcode(), data->wpos(), data->contents(), self_also, myteam_only, InWorldTargets); 
	}
	ARCEMU_INLINE void SendMessageToSet(StackWorldPacket * data, bool self_also=true, bool myteam_only=false, bool InWorldTargets = true) 
	{ 
		OutPacketToSet(data->GetOpcode(), data->GetSize(), data->GetBufferPointer(), self_also, myteam_only, InWorldTargets); 
	}
	void OutPacketToSet(uint16 Opcode, uint16 Len, const void * Data, bool self = true, bool myteam_only = false, bool InWorldTargets = true);

	//! Fill values with data from a space seperated string of uint32s.
	void LoadValues(const char* data);

	ARCEMU_INLINE uint16 GetValuesCount() const { return m_valuesCount; }

//	uint16 m_movementFlags;

	void EventSpellDamage(uint64 Victim, SpellEntry *sp, uint32 Damage, int32 pct_mod, int32 eff_index);
	void EventSpellDamageToLocation(uint64 Victim, SpellEntry *sp, uint32 Damage, int32 pct_mod, int32 eff_index, LocationVector src, LocationVector dst );
	void EventWeaponDamageToLocation(uint64 Victim, SpellEntry *sp, uint32 Damage, int32 pct_mod, int32 eff_index, LocationVector src, LocationVector dst );
	float	GetSpellCritChance( Unit *pVictim, SpellEntry *spellInfo,uint32 DOTticks=1, int32 sp_eff_index = 0xFF);
	void SpellNonMeleeDamageCalc(Unit *pVictim, SpellEntry *spellInfo, dealdamage *dmg, uint32 DOTticks=1, int32 sp_eff_index = 0xFF);
	void SpellNonMeleeDamageLog(Unit *pVictim, SpellEntry *spellInfo, dealdamage *dmg, uint32 DOTticks=1, int32 sp_eff_index = 0xFF);
//	void SpellNonMeleeDamageLog(Unit *pVictim, uint32 SpellId, dealdamage *dmg, bool allowProc, bool no_remove_auras = false,uint32 DOTticks=1);
	void SpellHeal(Unit *pVictim, SpellEntry *spellInfo, uint32 damage, bool allowProc, bool static_damage = false, bool no_remove_auras = false,uint32 DOTticks=1,int32 pct_mod=100, int32 sp_eff_index = 0xFF, int8 CanCrit=16);
	
	//*****************************************************************************************
	//* SpellLog packets just to keep the code cleaner and better to read
	//*****************************************************************************************
	void SendSpellLog(Object *Caster, Object *Target,uint32 Ability, uint8 SpellLogType);
//	void SendSpellNonMeleeDamageLog( Object* Caster, Object* Target, uint32 SpellID, uint32 Damage, uint8 School, uint32 AbsorbedDamage, uint32 ResistedDamage, bool PhysicalDamage, uint32 BlockedDamage, bool CriticalHit, bool bToSet );
//	void SendSpellPeriodicAuraLog(Object* Caster, Unit * Target, uint32 SpellID, uint32 School, uint32 Amount, uint32 abs_dmg, uint32 resisted_damage, uint32 Flags, uint8 crit);
	void SendAttackerStateUpdate( Object* Caster, Object* Target, dealdamage *Dmg, uint32 Damage, uint32 Abs, uint32 BlockedDamage, uint32 HitStatus, uint32 VState );
	void SendCombatLogMultiple(Unit *target, int32 base_dmg, int32 resisted, int32 absorbed, int32 blocked, uint32 spell_id, uint32 School_mask, uint32 log_type, uint8 is_critical, uint8 is_periodic = 0 );

	//object faction - only units and GOs have a faction ? Items do not ? Save 12 bytes for a lot of objects ? Screw it
	void _setFaction();
	uint32 _getFaction(){return m_faction->Faction;}
	FactionTemplateDBC *m_faction;
	FactionDBC *m_factionDBC;

	ARCEMU_INLINE void SetInstanceID(int32 instance) { m_instanceId = instance; }
	ARCEMU_INLINE int32 GetInstanceID() { return m_instanceId; }

	int32 event_GetInstanceID();

	uint32 m_phase; //This stores the phase, if two objects have the same bit set, then they can see each other. The default phase is 0x1.
	const uint32 GetPhase() { return m_phase; }
	void Phase(uint8 command=PHASE_SET, uint32 newphase=1);

	bool CanActivate();
	void Activate(MapMgr * mgr);
	void Deactivate(MapMgr * mgr);
//	uint8	m_ClientFinishedLoading;	//maybe this can be removed later. Right now using it to debug why client crashes on receiving A9 packets
	uint8	deleted;	//as idiot this looks like it marks that the object is marked to be deleted in the near future
	ARCEMU_INLINE bool QueryObjectIntegrityIsBad()
	{
		return (deleted != OBJ_AVAILABLE || internal_object_type > INTERNAL_OBJECT_TYPE_NONE2 || m_objectTypeId >= TYPEID_UNUSED );
	}
	ARCEMU_INLINE void SetMapMgr(MapMgr * mgr) { m_mapMgr = mgr; }

	void GMScriptEvent(void * function, uint32 argc, uint32 * argv, uint32 * argt);
//	ARCEMU_INLINE size_t GetInRangeOppFactCount() { return m_oppFactsInRange.size(); }
	void PlaySoundToSet(uint32 sound_entry);
	void PlaySoundTarget(uint32 sound_entry, uint64 guid = 0);
	bool IsInBg();
	uint32 GetTeam();
	virtual bool IsFFAPvPFlagged(){ return false; }	//items are never FFA flagged but units have this function overwritten

protected:

	//void _Create (uint32 guidlow, uint32 guidhigh);
	void _Create( uint32 mapid, float x, float y, float z, float ang);

	//! Mark values that need updating for specified player.
	virtual void _SetUpdateBits(UpdateMask *updateMask, Player *target) const;
	//! Mark values that player should get when he/she/it sees object for first time.
	virtual void _SetCreateBits(UpdateMask *updateMask, Player *target) const;

	void _BuildMovementUpdate( ByteBuffer *data, uint16 flags, uint32 flags2, Player* target );
	void _BuildValuesUpdate( ByteBuffer *data, UpdateMask *updateMask, Player* target );


	//! WoWGuid class
	WoWGuid m_wowGuid;

	 //! Type id.
	uint8 m_objectTypeId;

	//! Continent/map id.
	uint32 m_mapId;
	//! Map manager
	MapMgr *m_mapMgr;
	//! Current map cell
	MapCell *m_mapCell;

	/* Main Function called by isInFront(); */
	bool inArc(float Position1X, float Position1Y, float FOV, float Orientation, float Position2X, float Position2Y );
	
	LocationVector m_position;
	LocationVector m_lastMapUpdatePosition;

	//! Object properties.
	union {
		uint32* m_uint32Values;
		int32*	m_int32Values;
		float*	m_floatValues;
	};

	//! Number of properties
	uint16 m_valuesCount;

	//! List of object properties that need updating.
	UpdateMask m_updateMask;

	//! Set of Objects in range.
	//! TODO: that functionality should be moved into WorldServer.
#ifndef USE_STATIC_SET
	InRangePlayerSetRecProt	m_inRangePlayers;
//	InRangeSet				m_oppFactsInRange;
//	InRangeSet				m_sameFactsInRange;
  	InRangeSet				m_objectsInRange;
  	InRangeSetRecProt		m_QueueAddObjectsInRange;		//do not add and delete objects in the same cycle to not mess up iterators
	InRangeSet				m_QueueRemoveObjectsInRange;
	uint8					m_UseQueueInrangeSetChange;
#else
  	InRangeSetRecProt		m_objectsInRange;
	InRangePlayerSetRecProt	m_inRangePlayers;
#endif

	//! Remove object from map
	void _RemoveFromMap();

	int32		m_instanceId;
	uint32		internal_object_type;		//there are more then 1 type of creatures and we do not want to mess up pointers

	ExtensionSet m_extensions;
	//removing objects from ingame parses visibility list to remove himself from other. If A is getting removed, A does not see B, but B sees A then a corrupted pointer is created. Example player logs out near invisible player then invisible player casts a spell on him
	uint32		next_inrage_update_stamp;

public:

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
	ARCEMU_INLINE void* GetExtension(uint32 ID)
	{
#ifdef _DEBUG
		if( ID >= EXTENSION_ID_LAST_UNUSED )
		{
			//add this to the list of extensions instead using random numbers
//			assert( false );
		}
#endif
		ExtensionSet::iterator itr = m_extensions.find( ID );
		if( itr == m_extensions.end() )
			return (NULL);
		else
			return (itr->second);
	}
	ARCEMU_INLINE int64* GetCreateIn64Extension(uint32 ID)
	{
		void *Ext = GetExtension( ID );
		if( Ext == NULL )
		{
			int64 *p = new int64;
			*p = 0;
			SetExtension( ID, p );
			return p;
		}
		else
			return (int64 *)(Ext);
	}
	int64* GetCreateIn64Extension(const char *name)
	{
		uint32 ID = crc32((const unsigned char*)name, (unsigned int)strlen(name));
		return GetCreateIn64Extension( ID );
	}
	ARCEMU_INLINE uint32 GetAreaID();

	bool m_loadedFromDB;
	bool static_object;		//always gets update even if cell gets unloaded. Experimental !!!
	bool m_objectUpdated;	//! True if object was updated since last MapMgr::_UpdateObjects()
	// Semaphores - needed to forbid two operations on the same object at the same very time (may cause crashing\lack of data)
	bool mSemaphoreTeleport;
	bool Active;
	bool m_inQueue;
	Mutex ObjectLock;		//general usage mutex for list clearing and other stuff that delete pointers
};

#endif



