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
#include "Unit.h"
using namespace std;

//#define DEG2RAD (M_PI/180.0)
#ifndef M_PI
	#define M_PI		3.14159265358979323846
#endif
#define M_H_PI		1.57079632679489661923f
#define M_Q_PI		0.785398163397448309615f

Object::Object() : m_position(0,0,0,0) 
{
	m_mapId = 0;
//	m_zoneId = 0;

	m_uint32Values = 0;
	m_objectUpdated = false;


	m_valuesCount = 0;

//	m_movementFlags = 0;

	m_mapMgr = 0;
	m_mapCell = 0;

	mSemaphoreTeleport = false;


	m_faction = NULL;
	m_factionDBC = NULL;

	m_instanceId = WORLD_INSTANCE;
	Active = false;
	m_inQueue = false;
	m_loadedFromDB = false;
	static_object = false;

	m_faction = dbcFactionTemplate.LookupRow( 0 );
	m_factionDBC = dbcFaction.LookupRow( 0 );

	m_objectsInRange.clear();
	m_inRangePlayers.clear();
//	m_oppFactsInRange.clear();
//	m_sameFactsInRange.clear();
	deleted = OBJ_AVAILABLE;
	m_phase = 0x01;
	next_inrage_update_stamp = 0;
//	m_ClientFinishedLoading = true;	//this is valid for any object. But if you spam packets to players that still have loading screen they will quickly crash
#ifndef USE_STATIC_SET
	m_UseQueueInrangeSetChange = 0;
#endif
}

Object::~Object( )
{
	Virtual_Destructor();
//	ASSERT( deleted == OBJ_DELETED ); //garbage collector will set this. Else it will have value OBJ_POOLED_FOR_DELETE or something else
	deleted = OBJ_DELETED;	//pointless but might help debugging
#ifdef DEBUG_
	ASSERT( m_UseQueueInrangeSetChange == 0 );
#endif
}

void Object::Virtual_Destructor( )
{
	if(m_objectTypeId != TYPEID_ITEM)
		ASSERT(!m_inQueue);

	if( this->IsInWorld() && IsItem() == false )
		this->RemoveFromWorld(false);

	ClearInRangeSet();
	// for linux
	m_instanceId = WORLD_INSTANCE;
	
	//avoid mem leaks
	ExtensionSet::iterator itr;
	for( itr=m_extensions.begin(); itr!=m_extensions.end(); itr++)
		if( itr->second )
			delete itr->second;
	m_extensions.clear();

	//avoid leaving traces in eventmanager. Have to work on the speed. Not all objects ever had events so list iteration can be skipped
	sEventMgr.RemoveEvents( this );

	m_objectTypeId = TYPEID_UNUSED;
	internal_object_type = INTERNAL_OBJECT_TYPE_NONE;
	deleted = OBJ_POOLED_FOR_DELETE;	//pointless but might help debugging
	EventableObject::Virtual_Destructor();	//mark object as unable to store new events
}

ARCEMU_INLINE void Object::InrageConsistencyCheck(bool forced)
{
	//removing objects from ingame parses visibility list to remove himself from other. 
	//If A is getting removed, A does not see B, but B sees A then a corrupted pointer is created. 
	//Example : player logs out near invisible player then invisible player casts a spell on him
	//Example : NPC A despawns near a stealthed NPC B, NPC B will still think NPC A exists
	if( next_inrage_update_stamp < getMSTime() || forced == true )
	{
		next_inrage_update_stamp = getMSTime() + MAX(1000,GARBAGE_DELETE_DELAY / 2);
		AquireInrangeLock(); //make sure to release lock before exit function !
		InRangeSetRecProt::iterator iter,itr2;
		InrangeLoopExitAutoCallback AutoLock;
		for (iter = GetInRangeSetBegin( AutoLock ); iter != GetInRangeSetEnd();)
		{
			itr2 = iter;
			iter++;
			if( 
//				(*itr2) == NULL || 
				(*itr2)->deleted != OBJ_AVAILABLE 
//				|| (*itr2)->GetMapMgr() != GetMapMgr() 
//				|| (*itr2)->m_instanceId != m_instanceId 
//				|| objmgr.IsActiveObject( *itr2 ) == false
//				|| sGarbageCollection.HasObject( *itr2 ) == true
//				|| ((*itr2)->IsPlayer() && objmgr.IsActivePlayer( *itr2 ) == false )
				|| ( GetMapMgr() != NULL && GetMapMgr()->IsActiveObject( *itr2 ) == false )
				)
			{
				m_objectsInRange.erase( itr2 );
			}
		}
		InRangePlayerSetRecProt::iterator iter3,iter4;
		for (iter3 = GetInRangePlayerSetBegin( AutoLock ); iter3 != GetInRangePlayerSetEnd();)
		{
			iter4 = iter3;
			iter3++;
			if( 
//				(*iter4) == NULL || 
				(*iter4)->deleted != OBJ_AVAILABLE 
//				|| (*iter4)->GetMapMgr() != GetMapMgr() 
//`				|| (*iter4)->m_instanceId != m_instanceId 
//				|| objmgr.IsActiveObject( *iter4 ) == false
//				|| sGarbageCollection.HasObject( *iter4 ) == true
//				|| objmgr.IsActivePlayer( *iter4 ) == false
				|| ( GetMapMgr() != NULL && GetMapMgr()->IsActiveObject( *iter4 ) == false )
				)
				m_inRangePlayers.erase( iter4 );
		}
		ReleaseInrangeLock();
	}
}

ARCEMU_INLINE void Object::Update( uint32 p_time )
{
	//should we process this before or after object update ? 
	InrageConsistencyCheck();
}

void Object::_Create( uint32 mapid, float x, float y, float z, float ang )
{
	m_mapId = mapid;
	m_position.ChangeCoords(x, y, z, ang);
	if( IsCreature() )
		SafeCreatureCast( this )->SetSpawnPosition( x, y, z, ang );
	m_lastMapUpdatePosition.ChangeCoords(x,y,z,ang);
}

uint32 Object::BuildCreateUpdateBlockForPlayer(ByteBuffer *data, Player *target)
{
#ifdef _DEBUG
/*	if( target && *target->GetCreateIn64Extension( EXTENSION_ID_SIMULATE_FAULTY_SPAWNING ) == 0 )
	{
		if( Rand( 50 ) )
			return 0;
	}*/
#endif
	uint16 flags = 0;
	uint32 flags2 = 0;
	uint8 temp_objectTypeId = m_objectTypeId; //omg this can be owerwriten by objects that can be destroyed ?

	uint8 updatetype = UPDATETYPE_CREATE_OBJECT;

	// any other case
	switch(m_objectTypeId)
	{
		case TYPEID_ITEM:
		case TYPEID_CONTAINER:
	//		flags = CREATE_FLAG_FOR_SELF;		//note that both 0x8 and 0x10 flags are not know fields to be sent. Might cause issues
	//		updatetype = UPDATETYPE_CREATE_OBJECT_SELF;
			break;
		case TYPEID_UNIT:
		case TYPEID_PLAYER:
			flags = 0x60;
	//		if( SafeUnitCast( this )->GetVehicle() )
	//			flags |= 0x80;	//send this only if he is not facing where vehicle is facing
			break;
			// gameobject/dynamicobject
		case TYPEID_GAMEOBJECT:
/*
14480
01 - UPDATETYPE_CREATE_OBJECT
F3 0A 13 92 51 12 F1 
05 
40 02 
57 61 0B C6 B1 2A 50 C3 9F 52 B2 42 36 C2 BC 40 6E F1 1C 00 00 00 00 00 01 73 C4 09 00 0A 13 00 00 92 51 12 F1 21 00 00 00 92 51 02 00 00 00 80 3F 5B 02 00 00 1B 00 3F BE DF 81 7B 3F 00 00 FF FF 01 07 00 FF
144333
02 - UPDATETYPE_CREATE_OBJECT_SELF
F3 90 08 AA 1E 13 F1 
05 
40 03 
C3 67 05 C0	-2.0844581127167 
1F 
92 28 7F C4	-1020.6339111328 
5B B3 17 45	2427.2097167969 
96 3D EE 42	119.12028503418 
24 B9 56 42	53.680801391602 
2B D9 0B C0	-2.1851298809052 
DC 46 27 41	10.4547996521 
77 D8 FB 3F	1.9675434827805 
00 00 00 00 
00 00 00 00 00 00 00 00 
01 
73 84 09 00 90 08 00 00 AA 1E 13 F1 21 00 00 00 AA 1E 03 00 00 00 00 40 43 25 00 00 00 00 80 3F 00 00 FF FF 01 08 00 FF
*/
			flags = 0x0240;
//			flags = 0x0340;
/*			switch( GetByte(GAMEOBJECT_BYTES_1,GAMEOBJECT_BYTES_TYPEID) )
			{
				case GAMEOBJECT_TYPE_MO_TRANSPORT:  
					{
						if(GetTypeFromGUID() != HIGHGUID_TYPE_TRANSPORTER)
							return 0;   // bad transporter
						else
							flags = 0x0242;	//if we need to link this object to something else then send 352
					}break;

				case GAMEOBJECT_TYPE_TRANSPORT:
					{
						// deeprun tram, etc
						flags = 0x242;
					}break;

				case GAMEOBJECT_TYPE_DUEL_ARBITER:
					{
						// duel flags have to stay as updatetype 3, otherwise
						// it won't animate
						updatetype = UPDATETYPE_CREATE_OBJECT;
					}break;
				case GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING:
					{
						//EoE platform where people can stand on. Also a lot of towers use this. I bet they can be destroyed
						//also building like things use this
						//flags = 0x0F0;
						//temp_objectTypeId = TYPEID_UNIT;
					}break;
			}*/
			break; 
		case TYPEID_DYNAMICOBJECT:
		case TYPEID_CORPSE:
			flags = 0x0040;	//confirmed in 14333
			break;
	}

	if( target == this )
	{
		// player creating self
		flags |= CREATE_FLAG_FOR_SELF;
		updatetype = UPDATETYPE_CREATE_OBJECT_SELF;
	}

	// build our actual update
	*data << updatetype;

	// we shouldn't be here, under any cercumstances, unless we have a wowguid..
	ASSERT(m_wowGuid.GetNewGuidLen());
	*data << m_wowGuid;
	
	*data << temp_objectTypeId;

	_BuildMovementUpdate(data, flags, flags2, target);

	// we have dirty data, or are creating for ourself.
	UpdateMask updateMask;
	updateMask.SetCount( m_valuesCount );
	_SetCreateBits( &updateMask, target );

	// this will cache automatically if needed
	_BuildValuesUpdate( data, &updateMask, target );

	// update count: 1 ;)
	return 1;
}


//That is dirty fix it actually creates update of 1 field with
//the given value ignoring existing changes in fields and so on
//usefull if we want update this field for certain players
//NOTE: it does not change fields. This is also very fast method
WorldPacket *Object::BuildFieldUpdatePacket( uint32 index,uint32 value)
{
   // uint64 guidfields = GetGUID();
   // uint8 guidmask = 0;
	WorldPacket * packet=new WorldPacket(1500);
	packet->SetOpcode( SMSG_UPDATE_OBJECT );
	
	*packet << (uint32)1;//number of update/create blocks

	*packet << (uint8) UPDATETYPE_VALUES;		// update type == update
	*packet << GetNewGUID();

	uint32 mBlocks = index/32+1;
	*packet << (uint8)mBlocks;
	
	for(uint32 dword_n=mBlocks-1;dword_n;dword_n--)
		*packet <<(uint32)0;

	*packet <<(((uint32)(1))<<(index%32));
	*packet << value;
	
	return packet;
}

void Object::BuildFieldUpdatePacket(Player* Target, uint32 Index, uint32 Value)
{
	ByteBuffer buf(500);
	buf << uint8(UPDATETYPE_VALUES);
	buf << GetNewGUID();

	uint32 mBlocks = Index/32+1;
	buf << (uint8)mBlocks;

	for(uint32 dword_n=mBlocks-1;dword_n;dword_n--)
		buf <<(uint32)0;

	buf <<(((uint32)(1))<<(Index%32));
	buf << Value;

	Target->PushUpdateData(&buf, 1);
}

void Object::BuildFieldUpdatePacket(ByteBuffer * buf, uint32 Index, uint32 Value)
{
	*buf << uint8(UPDATETYPE_VALUES);
	*buf << GetNewGUID();

	uint32 mBlocks = Index/32+1;
	*buf << (uint8)mBlocks;

	for(uint32 dword_n=mBlocks-1;dword_n;dword_n--)
		*buf <<(uint32)0;

	*buf <<(((uint32)(1))<<(Index%32));
	*buf << Value;
}

uint32 Object::BuildValuesUpdateBlockForPlayer(ByteBuffer *data, Player *target)
{
	//might fail if sending to others due to visibility mask = no updates
	if( m_updateMask.GetBlockCount() )
	{
		*data << (uint8) UPDATETYPE_VALUES;		// update type == update
		ASSERT(m_wowGuid.GetNewGuidLen());
		*data << m_wowGuid;
		_BuildValuesUpdate( data, &m_updateMask, target );
		return 1;
	}
	return 0;
}

uint32 Object::BuildValuesUpdateBlockForPlayer(ByteBuffer * buf, UpdateMask * mask )
{
	// returns: update count
	*buf << (uint8) UPDATETYPE_VALUES;		// update type == update

	ASSERT(m_wowGuid.GetNewGuidLen());
	*buf << m_wowGuid;

	_BuildValuesUpdate( buf, mask, 0 );

	// 1 update.
	return 1;
}

void Object::DestroyForPlayer(Player *target, uint8 anim) const
{
	if(target->GetSession() == 0) 
	{ 
		return;
	}

	ASSERT(target);

	sStackWorldPacket( data, SMSG_DESTROY_OBJECT, 20 );
	data << GetGUID();
	data << uint8(anim);
	target->GetSession()->SendPacket( &data );
}


///////////////////////////////////////////////////////////////
/// Build the Movement Data portion of the update packet
/// Fills the data with this object's movement/speed info
/// TODO: rewrite this stuff, document unknown fields and flags
uint32 TimeStamp();

void Object::_BuildMovementUpdate(ByteBuffer * data, uint16 flags, uint32 flags2, Player* target )
{
	/* ByteBuffer *splinebuf = (m_objectTypeId == TYPEID_UNIT) ? target->GetAndRemoveSplinePacket(GetGUID()) : 0; */
//	uint16 flag16 = 0;	// some other flag
	*data << (uint16)flags;

	Player * pThis = NULL;
	MovementInfo* moveinfo = NULL;
	if(GetTypeId() == TYPEID_PLAYER)
	{
		pThis = SafePlayerCast( this );
		if(pThis->GetSession())
			moveinfo = pThis->GetSession()->GetMovementInfo();
		if(target == this)
		{
			// Updating our last speeds.
			pThis->UpdateLastSpeeds();
		}
	}
	Creature * uThis = NULL;
	if (GetTypeId() == TYPEID_UNIT)
		uThis = SafeCreatureCast(this);

	if( flags & 0x0100 )
	{
		*data << uint8( 0 ); //some compressed guid
	} 

	if (flags & 0x20)
	{
//		if(pThis && pThis->m_TransporterGUID != 0)
//			m_movementFlags |= 0x200;
//		else if(uThis != NULL && uThis->m_transportGuid != 0 && uThis->m_transportPosition != NULL)
//			m_movementFlags |= 0x200;
//		else if( IsUnit() && SafeUnitCast( this )->GetVehicle() )
//			m_movementFlags |= 0x200;

		/* if(splinebuf)
		{
			flags2 |= 0x08000001;	   //1=move forward
			if(uThis != NULL)
			{
				if(uThis->GetAIInterface()->m_moveRun == false)
					flags2 |= 0x100;	//100=walk
			}			
		}
		*/

		if(uThis != NULL)
		{
			//		 Don't know what this is, but I've only seen it applied to spirit healers.
			//		 maybe some sort of invisibility flag? :/

/*			switch(GetEntry())
			{
				case 6491:  // Spirit Healer
				case 13116: // Alliance Spirit Guide
				case 13117: // Horde Spirit Guide
					{
						flags2 |= 0x10000000; //in 420 client this makes them stride forever
					}break;
			} */
		
			if( uThis->GetAIInterface()->IsFlying() )
				flags2 |= A9_FLAGS2_NOGRAVITY | A9_FLAGS2_FLYING; //in 420

//(1 << 9) -> if put on player client will spam server with packet 0x08EA after loading screan finish containing guid. Client will not finish login
//(1 << 12) -> if put on player client will spam server with packet 2848 after loading screan finish containing guid. Client will not finish login
//(1 << 13) -> if put on player client will spam server with packet 2848 after loading screan finish containing guid. Client will not finish login
//(1 << 21) -> if put on player client will spam server with packet 0x08EA after loading screan finish containing guid. Client will not finish login
//(1 << 25) -> if put on player client will spam server with packet 0x08EA after loading screan finish containing guid. Client will not finish login
//(1 << 26) -> if put on player client will spam server with packet 0x08EA after loading screan finish containing guid. Client will not finish login
			if(uThis->GetProto() && uThis->GetProto()->extra_a9_flags)
			{
//do not send shit we can't honor
#define UNKNOWN_FLAGS2 ( 0x0200 | 0x00002000 | 0x00001000 | 0x00200000 | 0x04000000 | 0x08000000 )
//				uint32 inherit = uThis->GetProto()->extra_a9_flags;	//In case we want to send special flags. Might crash the client !
//				uint32 inherit = uThis->GetProto()->extra_a9_flags & UNKNOWN_FLAGS2;
				uint32 inherit = uThis->GetProto()->extra_a9_flags & (~UNKNOWN_FLAGS2);
				flags2 |= inherit;
			}
/*			if(GetGUIDHigh() == HIGHGUID_WAYPOINT)
			{
				if(GetUInt32Value(UNIT_FIELD_STAT0) == 768)		// flying waypoint
					flags2 |= 0x800;
			}*/
		}

		*data << (uint32)flags2;

//		*data << (uint16)m_movementFlags;
		*data << (uint16)0;

		*data << GetGUID();	//added in 14333 client

		*data << getMSTime(); // this appears to be time in ms but can be any thing. Maybe packet serializer ?

		// this stuff:
		//   0x01 -> Enable Swimming?
		//   0x04 -> ??
		//   0x10 -> disables movement compensation and causes players to jump around all the place
		//   0x40 -> disables movement compensation and causes players to jump around all the place
	}

	if( ( flags & 0x0100 ) )
	{
		*data << (float)m_position.x;
		*data << (float)m_position.y;
		*data << (float)m_position.z;
		*data << (float)m_position.x;
		*data << (float)m_position.y;
		*data << (float)m_position.z;
		*data << (float)m_position.o;
		if( IsCorpse() )
			*data << (float)m_position.o; 
		else
			*data << (float)0; 
	}
	else if (flags & 0x40)
	{
		*data << (float)m_position.x;
		*data << (float)m_position.y;
		*data << (float)m_position.z;
		*data << (float)m_position.o;

		if(flags2 & 0x0200)
		{
			Vehicle *pVehicle;
			if(uThis && uThis->GetParentSpawnID())
			{
				// Find Pointer To Parent Vehicle
				ParentInfo * info = uThis->GetParentInfo();
				pVehicle = GetMapMgr()->GetSqlIdVehicle( info->parentspawnid );
				if(pVehicle != NULL)
				{
					*data << pVehicle->GetNewGUID();
					*data << info->offsetX;
					*data << info->offsetY;
					*data << info->offsetZ;
					*data << info->orientation;
					*data << (uint32)(0);
					*data << uint8(info->seatid);
				}
				else
					goto EMPTYTRANSPORTDATA;
			}
			else if( IsUnit() && (pVehicle = SafeUnitCast(this)->GetVehicle()) != NULL ) // we are on a vehicle no matter wich seat we're in
			{
				Unit* pUnit = SafeUnitCast(this);
				VehicleEntry* v = dbcVehicleEntry.LookupEntry(pVehicle->GetVehicleID());
				if (v == NULL)
					goto EMPTYTRANSPORTDATA;
				int8 seat = pUnit->GetSeatID();
				VehicleSeatEntry * seatentry = dbcVehicleSeatEntry.LookupEntry(pVehicle->GetVehicleEntry()->m_seatID[seat]);
				if (seatentry == NULL)
					goto EMPTYTRANSPORTDATA;
				*data << pUnit->GetVehicle()->GetNewGUID();
				*data << seatentry->m_attachmentOffsetX;
				*data << seatentry->m_attachmentOffsetY;
				*data << seatentry->m_attachmentOffsetZ;
				*data << float(0.0f);
				*data << uint32(0);
				*data << uint8(pUnit->GetSeatID());
			}
			else if(pThis)
			{
				WoWGuid wowguid(pThis->m_TransporterGUID);
				*data << wowguid;
				*data << pThis->m_TransporterX << pThis->m_TransporterY << pThis->m_TransporterZ << pThis->m_TransporterO;
				*data << pThis->m_TransporterTime;
				*data << (uint8)0;
			}
			else if(uThis != NULL && uThis->m_transportPosition != NULL)
			{
				uint64 tguid = ((uint64)HIGHGUID_TYPE_TRANSPORTER << 32) | uThis->m_transportGuid;
				WoWGuid wowguid( tguid );
				*data << wowguid;
				*data << uThis->m_transportPosition->x << uThis->m_transportPosition->y << 
					uThis->m_transportPosition->z << uThis->m_transportPosition->o;
				*data << uint32(0);
				*data << uint8(0);
			}
			else
			{
EMPTYTRANSPORTDATA:
				*data << uint8(0);
				*data << LocationVector(0, 0, 0);
				*data << (float)0;
				*data << uint32(0);
				*data << uint8(0);
			}
		}
	}

	if (flags & 0x20)
	{
/*
		//removed in 14333 ?
		if( flags2 & 0x0200000 ) //flying/swimming, && unk sth to do with vehicles?
		{
			if(pThis && moveinfo)
				*data << moveinfo->pitch;
			else 
				*data << (float)0; //pitch
		} /**/

//		if(pThis && moveinfo)
//			*data << moveinfo->unklast;
//		else
//			*data << (uint32)0; //last fall time

		if( ( flags2 & 0x00002000 ) || ( flags2 & 0x00001000 ) )
		{
/*			if(pThis && moveinfo)
			{
				*data << moveinfo->FallTime;
				*data << moveinfo->jump_sinAngle;
				*data << moveinfo->jump_cosAngle;
				*data << moveinfo->jump_xySpeed;
			}
			else */
			{
				*data << (float)0;
				*data << (float)1.0;
				*data << (float)0;
				*data << (float)0;
			}
		}
		if( flags2 & 0x02000000 )
			*data << (uint32)0; //?
		if( flags2 & 0x00200000 )
			*data << (uint32)0; //?
		if( flags2 & 0x04000000 )
			*data << (uint32)0; //?

		if( IsUnit() )
		{
			*data << SafeUnitCast( this )->m_walkSpeed;
			*data << SafeUnitCast( this )->m_runSpeed;
			*data << SafeUnitCast( this )->m_backWalkSpeed;
			*data << SafeUnitCast( this )->m_swimSpeed;
			*data << SafeUnitCast( this )->m_backSwimSpeed;
			*data << SafeUnitCast( this )->m_flySpeed;
			*data << SafeUnitCast( this )->m_backFlySpeed;
			*data << SafeUnitCast( this )->m_turnRate;
			*data << SafeUnitCast( this )->m_pitchRate;
		}
		else
		{
			*data << 7.0f;
			*data << 9.0f;
			*data << 5.0f;
			*data << 7.0f;
			*data << 5.0f;
			*data << 15.0f;
			*data << 10.0f;
			*data << 3.14f;
			*data << 3.14f;
		}
	}

#ifdef NOT_VALID_SINCE_13287_CLIENT
	if( ( flags & 0x08 ) && ( flags & 0x10 ) )
	{
		//!!! this is defenetly not guid. Seems to be some huge number that increases by every item sent to client
		//note that any object can have 0x18 flags
		/*
		02 - create packet for item
		9F 98 46 DD 43 02 41 01 - item guid
		18 00 - flags
		AC 0F 49 D2 98 46 DD 43 - this 8 byte number, what is it ?

		02 9F B0 89 CD 37 02 41 01 18 00 AC 0F 49 D2 B0 89 CD 37
		02 9F 1F 89 CD 37 02 41 01 18 00 AC 0F 49 D2 1F 89 CD 37
		02 9F 29 25 22 39 02 41 01 18 00 AC 0F 49 D2 29 25 22 39
		02 9F 2D 89 CD 37 02 41 01 18 00 AC 0F 49 D2 2D 89 CD 37
		02 9F 09 CC 1C 3B 02 41 01 18 00 AC 0F 49 D2 09 CC 1C 3B
		02 9F 2E 89 CD 37 02 41 01 18 00 AC 0F 49 D2 2E 89 CD 37
		02 9F 1D 89 CD 37 02 41 01 18 00 AC 0F 49 D2 1D 89 CD 37
		02 9F 20 89 CD 37 02 41 01 18 00 AC 0F 49 D2 20 89 CD 37
		02 9F 1E 89 CD 37 02 41 01 18 00 AC 0F 49 D2 1E 89 CD 37
		02 9F 2A 89 CD 37 02 41 01 18 00 AC 0F 49 D2 2A 89 CD 37
		02 9F 6B 05 79 42 02 41 01 18 00 AC 0F 49 D2 6B 05 79 42
		02 9F 23 89 CD 37 02 41 01 18 00 AC 0F 49 D2 23 89 CD 37
		02 9F 2B 89 CD 37 02 41 01 18 00 AC 0F 49 D2 2B 89 CD 37
		02 9F 72 70 A1 3F 02 41 01 18 00 AC 0F 49 D2 72 70 A1 3F
		02 9F 66 AE F5 39 02 41 01 18 00 AC 0F 49 D2 66 AE F5 39
		02 9F 33 89 CD 37 02 41 01 18 00 AC 0F 49 D2 33 89 CD 37
		02 9F 32 89 CD 37 02 41 02 18 00 AC 0F 49 D2 32 89 CD 37

		02 9F BD 89 CD 37 02 41 01 18 00 B9 18 18 FA BD 89 CD 37
		02 9F 13 89 CD 37 02 41 01 10 00 13 89 CD 37
		02 9F 16 89 CD 37 02 41 01 10 00 16 89 CD 37
		02 8F AB BF 38 03 01 04 78 00 00 00 00 00 00 00 D5 08 64 AF A5 43 F0 44 85 66 91 C5 43 79 E7 41 34 0A 0B 40 BD 02 00 00 00 00 20 40 CD CC 00 41 00 00 90 40 72 1C 97 40 00 00 20 40 00 00 E0 40 00 00 90 40 DB 0F 49 40 DB 0F 49 40 80 E1 2F D2 78 E1 2F D2 2A
		*/

		//AC 0F 49 D2 - seems to be changing slower then the next 4 bytes.
		*data << GetUInt32Value(OBJECT_FIELD_GUID_01);	//defenetly not HIGHguid
//		*data << uint32( 0x0xD2490FAC );	//seems to be a static number / block ?
		*data << GetUInt32Value(OBJECT_FIELD_GUID);			//confirmed to be lowguid only for item !
	}
	else if(flags & 0x10)
	{
        switch(GetTypeId())
        {
            case TYPEID_OBJECT:
            case TYPEID_ITEM:
            case TYPEID_CONTAINER:
            case TYPEID_GAMEOBJECT:
            case TYPEID_DYNAMICOBJECT:
            case TYPEID_CORPSE:
                *data << GetUInt32Value(OBJECT_FIELD_GUID);		//confirmed
                break;
            case TYPEID_UNIT:
                *data << uint32(0x0000000B);                // unk, can be 0xB or 0xC
                break;
            case TYPEID_PLAYER:
                if(flags & 0x1)	//self
                    *data << uint32(0x0000002F);            // unk, can be 0x15 or 0x22
                else
                    *data << uint32(0x00000008);            // unk, can be 0x7 or 0x8
                break;
            default:
                *data << uint32(0x00000000);                // unk
                break;
        }
	}
#endif

	if( flags & 0x04 )
		*data << (uint8)0;	//some compressed GUID -> some say it is target guid

	if( flags & 0x80 )
	{
		if( IsUnit() )
			*data << (uint32)(SafeUnitCast(this)->GetCurrentVehicleID()); //vehicle ID
		else
			*data << (uint32)0; 
		*data << (float)0; //facing adjustments or simply facing ? (used in client for camera handling)
	}

	//no idea of content and place
	if (flags & 0x800)
	{
		*data << uint16(0) << uint16(0) << uint16(0); //unk
	} 

	if(flags & 0x2)
	{
		if(target)
		{
			/*int32 m_time = TimeStamp() - target->GetSession()->m_clientTimeDelay;
			*data << m_time;*/
			*data << getMSTime();
		}
		else
            *data << getMSTime();
	}
	if( flags & 0x0200 )
	{
		if( IsGameObject() )
			*data << SafeGOCast( this )->m_rotationX;
		else
			*data << (uint64)0; //?
	}
	
	//no idea of content and place
	if (flags & 0x1000)
	{
		uint8 bytes = 0;
		*data << bytes;
		for(uint8 i = 0; i < bytes; i++) //example :P
		{
			*data << uint32(0);
		}
	} 
}


//=======================================================================================
//  Creates an update block with the values of this object as
//  determined by the updateMask.
//=======================================================================================
void Object::_BuildValuesUpdate(ByteBuffer * data, UpdateMask *updateMask, Player* target)
{
	bool activate_quest_object = false;
	bool resetUnitFlags = false,resetGOFlags = false;
	uint32 oldflags;
	uint32 oldUnitflags; //gms should be able to delete even not selectable mobs, or talk to them
	uint32 oldNPCFlags;

	if( updateMask->GetBit(OBJECT_FIELD_GUID) && target )	   // We're creating.
	{
		if( IsCreature() )
		{
			oldUnitflags = m_uint32Values[UNIT_FIELD_FLAGS];
			oldNPCFlags = m_uint32Values[UNIT_NPC_FLAGS];
			oldflags = m_uint32Values[UNIT_DYNAMIC_FLAGS];
			resetUnitFlags = true;
			Creature * pThis = SafeCreatureCast(this);
			if( ( m_uint32Values[UNIT_FIELD_FLAGS] & UNIT_FLAG_NOT_SELECTABLE ) && target->GetSession() && target->GetSession()->HasGMPermissions() )
			{
	//			m_uint32Values[UNIT_FIELD_FLAGS] = oldunitflags & ~(UNIT_FLAG_NOT_ATTACKABLE_2|UNIT_FLAG_NOT_ATTACKABLE_9|UNIT_FLAG_NOT_SELECTABLE);
				m_uint32Values[UNIT_FIELD_FLAGS] = oldUnitflags & ~(UNIT_FLAG_NOT_SELECTABLE);
			}
			//remove trainer flags if we cannot train at this NPC
			if( target && ( m_uint32Values[UNIT_NPC_FLAGS] & UNIT_NPC_FLAG_TRAINER_CLASS ) && CanTrainAt( target, pThis->GetTrainer() ) == false )
			{
				m_uint32Values[UNIT_NPC_FLAGS] = oldNPCFlags & ~(UNIT_NPC_FLAG_TRAINER_CLASS|UNIT_NPC_FLAG_TRAINER);
			}
			if( pThis->TaggerGuid && (pThis->loot.gold || pThis->loot.items.size()))
			{
				// Let's see if we're the tagger or not.
				uint32 Flags = m_uint32Values[UNIT_DYNAMIC_FLAGS];

				//if player group killed it or tagger was not in a group when started attacking
				if( pThis->TaggerGroupId == target->GetGroup() || ( pThis->TaggerGroupId == NULL && pThis->TaggerGuid == target->GetGUID() ) )
				{
					// Our target is our tagger.
					Flags &= ~U_DYN_FLAG_TAGGED_BY_OTHER;
					Flags |= U_DYN_FLAG_LOOTABLE;
				}
				else
				{
					// Target is not the tagger.
					Flags |= U_DYN_FLAG_TAGGED_BY_OTHER;
					Flags &= ~U_DYN_FLAG_LOOTABLE;
				}

				m_uint32Values[UNIT_DYNAMIC_FLAGS] = Flags;

				updateMask->SetBit(UNIT_DYNAMIC_FLAGS);
			}
		}
		else if( IsGameObject() )
		{
			GameObject *go = SafeGOCast(this);
			QuestLogEntry *qle;
			GameObjectInfo *info;

			if ( go )
			{
				if( go->HasQuests() )
				{
					activate_quest_object = true;
				}
				else
				{
					info = go->GetInfo();
					if( info && ( info->goMap.size() || info->itemMap.size() ) )
					{
						for( GameObjectGOMap::iterator itr = go->GetInfo()->goMap.begin(); itr != go->GetInfo()->goMap.end(); ++itr )
						{
							qle = target->GetQuestLogForEntry( itr->first->id );
							if( qle != NULL )
							{
								if( qle->GetQuest()->count_required_mob == 0 )
									continue;
								for( uint32 i = 0; i < 4; ++i )
								{
									if( qle->GetQuest()->required_mob[i] == (int32)go->GetEntry() && (int32)qle->GetMobCount(i) < qle->GetQuest()->required_mobcount[i])
									{
										activate_quest_object = true;
										break;
									}
								}
								if(activate_quest_object)
									break;
							}
						}

						if(!activate_quest_object)
						{
							for(GameObjectItemMap::iterator itr = go->GetInfo()->itemMap.begin();
								itr != go->GetInfo()->itemMap.end();
								++itr)
							{
								for(std::map<uint32, uint32>::iterator it2 = itr->second.begin();
									it2 != itr->second.end(); 
									++it2)
								{
									if((qle = target->GetQuestLogForEntry(itr->first->id)) != 0)
									{
										if(target->GetItemInterface()->GetItemCount(it2->first) < it2->second)
										{
											activate_quest_object = true;
											break;
										}
									}
								}
								if(activate_quest_object)
									break;
							}
						}
					}
				}
			}
		}
	}


	if(activate_quest_object)
	{
		oldflags = m_uint32Values[GAMEOBJECT_DYNAMIC];
		if(!updateMask->GetBit(GAMEOBJECT_DYNAMIC))
			updateMask->SetBit(GAMEOBJECT_DYNAMIC);
		m_uint32Values[GAMEOBJECT_DYNAMIC] = GO_DYNFLAG_LO_ACTIVATE | GO_DYNFLAG_LO_SPARKLE; // 8 to show sparkling
		resetGOFlags = true;
	}

	ASSERT( updateMask && updateMask->GetCount() == m_valuesCount );
	uint32 bc;
	uint32 values_count;
	uint8 *m = NULL;
	if( updateMask->GetBit(OBJECT_FIELD_GUID) == 0 )//it is an update not a create
	{
		if( 
//			1 == 1 ||
			target == this 
			|| !IsUnit() 
			|| GetUInt32Value( UNIT_FIELD_CHARMEDBY ) == m_uint32Values[ OBJECT_FIELD_GUID ]
			|| GetUInt32Value( UNIT_FIELD_SUMMONEDBY ) == m_uint32Values[ OBJECT_FIELD_GUID ]
			|| GetUInt32Value( UNIT_FIELD_CREATEDBY ) == m_uint32Values[ OBJECT_FIELD_GUID ]
			)	//updating self or stuff that is ours
		{
			bc = updateMask->GetUpdateBlockCount();
			values_count = min<uint32>( bc * 32, m_valuesCount );
		}
		else
		{
			m = (uint8*)sWorld.create_update_for_other_masks[ GetTypeId() ]->GetMask();
			bc = updateMask->GetUpdateBlockCount( (uint32*)m );
			values_count = min<uint32>( bc * 32, m_valuesCount );
		}
	}
	else 
	{
		bc=updateMask->GetBlockCount();
		values_count=m_valuesCount;
	}

	/*
//#define DEBUG_A9_CONTENT 1	//if enabled it should produce same output as parser does. Comparison should be simple
#ifdef DEBUG_A9_CONTENT
	#include "A9_human/human_form.cpp"
	sLog.outDebug("Mask is : ");
	const uint8 *mask = updateMask->GetMask();
	for( unsigned int i=0; i<bc*4; i++ )
		sLog.outDebug("%02X ",mask[i]);
	sLog.outDebug("\n");
#endif
	for( uint32 index = 0; index < values_count; index ++ )
		if( updateMask->GetBit( index ) )
		{
#ifdef DEBUG_A9_CONTENT
			if( player_field_lookup_table[ index ][ 0 ].type == 1 )
				sLog.outDebug(" %s = %u , index = %u\n",player_field_lookup_table[ index ][ 0 ].name, m_uint32Values[ index ], index );		
			else if( player_field_lookup_table[ index ][ 0 ].type == 2 )
				sLog.outDebug(" %s = %f , index = %u\n",player_field_lookup_table[ index ][ 0 ].name, *(float*)&m_uint32Values[ index ], index );		
#endif
			*data << m_uint32Values[ index ];
		}*/

	//we can messup the packet if we do not send any mask and content i think
	if( bc == 0 )
	{
		//just send an empty mask. Shit happens i guess a lot of times( really lot of times ) on live servers
		*data << (uint8)1;	
		*data << (uint32)0;
	}
	else
	{
		*data << (uint8)bc;
		//this is probably create for self or just create for others ( in this case we made a temp update mask already )
		if( m == NULL )
		{
			data->append( updateMask->GetMask(), bc*4 );
			for( uint32 index = 0; index < values_count; index ++ )
				if( updateMask->GetBit( index ) )
					*data << m_uint32Values[ index ];
		}
		//this is update
		else
		{
			//add a mask that filters visibility bits only
			uint32 *m32 = (uint32 *)m;
			for( uint32 index = 0; index < bc; index ++ )
				*data << (updateMask->GetBlock( index ) & m32[ index ]);
			//only add values filtered by visibility bits
			for( uint32 index = 0; index < values_count; index ++ )
				if( updateMask->GetBit( index, m ) )
					*data << m_uint32Values[ index ];
		}
	}

	if( resetGOFlags )
	{
		m_uint32Values[GAMEOBJECT_DYNAMIC] = oldflags;
	}
	else if( resetUnitFlags )
	{
		m_uint32Values[UNIT_FIELD_FLAGS] = oldUnitflags;
		m_uint32Values[UNIT_NPC_FLAGS] = oldNPCFlags;
		m_uint32Values[UNIT_DYNAMIC_FLAGS] = oldflags;
	}
}
/*
void Object::BuildHeartBeatMsg(WorldPacket *data) const
{
	data->Initialize(MSG_MOVE_HEARTBEAT);

	*data << GetNewGUID();

	*data << uint32(0); // flags
	*data << uint16(0); // flags2
	*data << uint32( getMSTime() ); // mysterious value #1

	*data << m_position;
	*data << m_position.o;

	*data << uint32(0); //unk1 - one check showed orientation / 2
	*data << uint32(0); //unk2
}*/

void Object::BuildTeleportAckMsg(const LocationVector & v)
{
	if( !IsPlayer() )
		return;
	///////////////////////////////////////
	//Update player on the client with TELEPORT_ACK
/*
{SERVER} Packet: (0x00C7) SMSG_MOVE_TELEPORT_ACK PacketSize = 40 TimeStamp = 4491377
8F BB B1 82 02 07 
00 00 00 00 
00 00 00 00
00 00 
F2 50 4B 9D 
58 E9 0C 44 
21 3F B8 42 
A0 9A C5 43
DC 0F C9 3F
00 00 00 00
	WorldPacket * data = new WorldPacket(SMSG_MOVE_TELEPORT_ACK, 80);
	*data << GetNewGUID();
	*data << uint32( 0 ); // 
	*data << uint32( 0 ); // flags -> this 0x0C is from a packet when we blinked. Might be something else !
	*data << uint16( 0 );
	*data << uint32( getMSTime() ); // mysterious value #1 -> this can be just 0 ?
	*data << v;
	*data << v.o;
	*data << uint32(0);	//falltime ?
	return data;

{SERVER} Packet: (0x9303) UNKNOWN PacketSize = 28 TimeStamp = 1663766
CE 80 11001110 10000000 gmask 
02 g[3] - 1
00 g[5] - 1 ..maybe go[7]
A9 g[2] + 1
0C 00 00 00 flags
81 g[4] + 1
3D B5 0B C6 x -8941.3095703125 
91 2D 39 C3 y -185.17799377441 
A2 D4 9F 42 z 79.915298461914 
D3 g[0] + 1
EC D1 32 40 o 2.7940626144409 
E8 g[1] - 1
{SERVER} Packet: (0x9303) SMSG_MOVE_TELEPORT_ACK PacketSize = 27 TimeStamp = 9619286
CC 80 11001100 10000000 mask
05 go3 04
03 go7 02
67 go2 66
C8 00 00 00 f
3D 24 0A C6 x
F8 D3 F4 43 y
C9 36 DB 42 z
9F go0 9E
4D 9C 1E 3F o
E7 go1 E6
//mage blink
{SERVER} Packet: (0x9303) SMSG_MOVE_TELEPORT_ACK PacketSize = 27 TimeStamp = 65002824
CC 80 
02 00 FB 
0B 00 00 00 
73 92 B2 45 
40 84 61 44 
55 D1 1E 43 
D2 
B3 77 8E 40 
28 
*/
//	uint8	g_len = GetNewGUID().GetNewGuidLen()+1;
	uint8	gbytes[8];
	*(uint64*)gbytes = GetGUID();
	sStackWorldPacket( data, SMSG_MOVE_TELEPORT_ACK, 80);
	data << uint16( 0x80CC );	//mask
//	if( gbytes[3] )
		data << ObfuscateByte( gbytes[3] );
//	if( gbytes[7] )	//mask bit 1
		data << ObfuscateByte( gbytes[7] );
//	if( gbytes[2] )
		data << ObfuscateByte( gbytes[2] );
	if( IsPlayer() )
		data << uint32( SafePlayerCast( this )->m_PositionUpdateCounter++ );
	else
		data << uint32( getMSTime() );	// seems to be packet serializer. Increases with any other movement serializer
//	if( gbytes[4] )	//mask bit 2
//		*data << ObfuscateByte( gbytes[4] );
	data << v.x;
	data << v.y;
	data << v.z;
//	if( gbytes[0] )	//in fact this is always true for a GUID
		data << ObfuscateByte( gbytes[0] );
	data << v.o;
//	if( gbytes[1] )	
		data << ObfuscateByte( gbytes[1] );
	if( SafePlayerCast( this )->GetSession() )
		SafePlayerCast( this )->GetSession()->SendPacket( &data );
	SafePlayerCast( this )->SetPlayerStatus( TRANSFER_PENDING );
	SafePlayerCast( this )->m_sentTeleportPosition = v;
}

bool Object::SetPosition(const LocationVector & v, bool allowPorting /* = false */)
{
	bool updateMap = false, result = true;

	if (m_position.x != v.x || m_position.y != v.y)
		updateMap = true;

	m_position = const_cast<LocationVector&>(v);

	if (!allowPorting && v.z < -2000)
	{
		m_position.z = 500;
		sLog.outError( "setPosition: fell through map; height ported" );

		result = false;
	}

	if (IsInWorld() && updateMap)
	{
		m_mapMgr->ChangeObjectLocation(this);
	}

	return result;
}

bool Object::SetPosition( float newX, float newY, float newZ, float newOrientation, bool allowPorting )
{
	bool updateMap = false, result = true;

	//It's a good idea to push through EVERY transport position change, no matter how small they are. By: VLack aka. VLsoft
	if( GetTypeId() == TYPEID_GAMEOBJECT && SafeGOCast(this)->GetInfo()->Type == GAMEOBJECT_TYPE_TRANSPORT )
		updateMap = true;

	//if (m_position.x != newX || m_position.y != newY)
		//updateMap = true;
	if( IsPlayer() && SafePlayerCast( this )->IsStealth() && m_lastMapUpdatePosition.Distance2DSq(newX, newY) > 1.0f )
		updateMap = true;
	else if(m_lastMapUpdatePosition.Distance2DSq(newX, newY) > (7.0f*7.0f))
		updateMap = true;

	m_position.ChangeCoords(newX, newY, newZ, newOrientation);

	if (!allowPorting && newZ < -2000)
	{
		m_position.z = 500;
		sLog.outError( "setPosition: fell through map; height ported" );

		result = false;
	}

	if( IsInWorld() && updateMap == true )
	{
		m_lastMapUpdatePosition.ChangeCoords(newX,newY,newZ,newOrientation);
		m_mapMgr->ChangeObjectLocation(this);

		if( m_objectTypeId == TYPEID_PLAYER && SafePlayerCast( this )->GetGroup() && SafePlayerCast( this )->m_last_group_position.Distance2DSq(m_position) > 25.0f ) // distance of 5.0
            SafePlayerCast( this )->GetGroup()->HandlePartialChange( GROUP_UPDATE_FLAG_POSITION, SafePlayerCast( this ) );
	}

	return result;
}

void Object::SetRotation( uint64 guid )
{
	sStackWorldPacket(data,SMSG_AI_REACTION, 15);
	data << guid;
	data << uint32(2);
	SendMessageToSet(&data, false);
}

void Object::OutPacketToSet(uint16 Opcode, uint16 Len, const void * Data, bool self, bool myteam_only, bool InWorldTargets)
{
	if(self && IsPlayer() && SafePlayerCast( this )->GetSession() )
		SafePlayerCast( this )->GetSession()->OutPacket(Opcode, Len, Data);

	if(!IsInWorld())
	{ 
		return;
	}

	InrangeLoopExitAutoCallback AutoLock;
	InRangePlayerSetRecProt::iterator itr = GetInRangePlayerSetBegin( AutoLock );
	InRangePlayerSetRecProt::iterator it_end = GetInRangePlayerSetEnd();
	int gm = ( IsPlayer() ? SafePlayerCast( this )->IsGMInvisible() : 0 );
	AquireInrangeLock();
	if( gm )	//very rare
	{
		for(; itr != it_end; ++itr)
			if( (*itr)->GetSession()->GetPermissionCount() > 0 
				&& ( myteam_only == false || (*itr)->GetTeam() == GetTeam() )
//				&& ( InWorldTargets == false || (*itr)->m_ClientFinishedLoading == CLIENT_FINISHED_LOADING ) 
				)
				(*itr)->GetSession()->OutPacket(Opcode, Len, Data);
	}
	else if( myteam_only == true )	//rare
	{
		for(; itr != it_end; ++itr)
			if( (*itr)->GetTeam() == GetTeam() 
//				&& ( InWorldTargets == false || (*itr)->m_ClientFinishedLoading == CLIENT_FINISHED_LOADING ) 
				)
				(*itr)->GetSession()->OutPacket(Opcode, Len, Data);
	}
//	else if( InWorldTargets == true ) //almost all the case
	{
		for(; itr != it_end; ++itr)
//			if( (*itr)->m_ClientFinishedLoading == CLIENT_FINISHED_LOADING )
			if( (*itr)->GetSession() )
				(*itr)->GetSession()->OutPacket(Opcode, Len, Data);
	}
/*	else
	{
		for(; itr != it_end; ++itr)
			(*itr)->GetSession()->OutPacket(Opcode, Len, Data);
	} */
	ReleaseInrangeLock();
}


////////////////////////////////////////////////////////////////////////////
/// Fill the object's Update Values from a space deliminated list of values.
void Object::LoadValues(const char* data)
{
	// thread-safe ;) strtok is not.
	std::string ndata = data;
	std::string::size_type last_pos = 0, pos = 0;
	uint32 index = 0;
	uint32 val;
	do 
	{
		// prevent overflow
		if(index >= m_valuesCount)
		{
			break;
		}
		pos = ndata.find(" ", last_pos);
		val = atol(ndata.substr(last_pos, (pos-last_pos)).c_str());
		if(m_uint32Values[index] == 0)
			m_uint32Values[index] = val;
		last_pos = pos+1;
		++index;
	} while(pos != std::string::npos);
} 

void Object::_SetUpdateBits(UpdateMask *updateMask, Player *target) const
{
	*updateMask = m_updateMask;
}


void Object::_SetCreateBits(UpdateMask *updateMask, Player *target) const
{
	/*for( uint16 index = 0; index < m_valuesCount; index++ )
	{
		if(GetUInt32Value(index) != 0)
			updateMask->SetBit(index);
	}*/
	for(uint32 i = 0; i < m_valuesCount; ++i)
		if(m_uint32Values[i] != 0)
			updateMask->SetBit(i);
}

void Object::AddToWorld()
{
	MapMgr *mapMgr = sInstanceMgr.GetInstance(this);
	if(!mapMgr)
	{ 
		return;
	}

	if(this->IsPlayer())
	{
		Player *plr = SafePlayerCast( this );

		//this should not happen, before adding to some world we should removed from other
		//however due to teleport event on teleport exist it is possible that this gets bugged
		if( IsInWorld() )
			RemoveFromWorld( false );

		if(mapMgr->pInstance != NULL && !plr->bGMTagOn)
		{
			// Player limit?
			if(mapMgr->GetMapInfo()->playerlimit && mapMgr->GetPlayerCount() >= mapMgr->GetMapInfo()->playerlimit)
			{ 
				return;
			}
			Group* group = plr->GetGroup();
			// Player in group?
			if(group == NULL && mapMgr->pInstance->m_creatorGuid == 0)
			{ 
				return;
			}
			// If set: Owns player the instance?
			if(mapMgr->pInstance->m_creatorGuid != 0 && mapMgr->pInstance->m_creatorGuid != plr->GetLowGUID())
			{ 
				return;
			}
			// Is instance empty or owns our group the instance?
			if(mapMgr->pInstance->m_creatorGroup != 0 && group != NULL && mapMgr->pInstance->m_creatorGroup != group->GetID())
			{
				// Player not in group or another group is already playing this instance.
				sChatHandler.SystemMessageToPlr(plr, "Another group is already inside this instance of the dungeon.");
				if(plr->GetSession()->GetPermissionCount() > 0)
					sChatHandler.BlueSystemMessageToPlr(plr, "Enable your GameMaster flag to ignore this rule.");
				return; 
			}
			else if(group != NULL && mapMgr->pInstance->m_creatorGroup == 0)
				// Players group now "owns" the instance.
				mapMgr->pInstance->m_creatorGroup = group->GetID(); 
		}
	}

	m_mapMgr = mapMgr;
	m_inQueue = true;

	mapMgr->AddObject(this);

	// correct incorrect instance id's
	m_instanceId = m_mapMgr->GetInstanceID();
//	m_mapId = m_mapMgr->GetMapId();
	event_Relocate(); //changed map ? then we need to port events to this map

	mSemaphoreTeleport = false;
}

void Object::AddToWorld(MapMgr * pMapMgr)
{
	if( !pMapMgr|| (pMapMgr->GetMapInfo()->playerlimit && this->IsPlayer() && pMapMgr->GetPlayerCount() >= pMapMgr->GetMapInfo()->playerlimit) )
		return; //instance add failed

	//this should not happen, before adding to some world we should removed from other
	//however due to teleport event on teleport exist it is possible that this gets bugged
	if( IsInWorld() )
		RemoveFromWorld( false );

	m_mapMgr = pMapMgr;
	m_inQueue = true;

	pMapMgr->AddObject(this);

	// correct incorrect instance id's
	m_instanceId = pMapMgr->GetInstanceID();
//	m_mapId = m_mapMgr->GetMapId();
	event_Relocate();

	mSemaphoreTeleport = false;
}

//Unlike addtoworld it pushes it directly ignoring add pool
//this can only be called from the thread of mapmgr!!!
void Object::PushToWorld(MapMgr*mgr)
{
	if(!mgr || deleted/* || (m_mapMgr != NULL && m_mapCell != NULL) */)
		return; //instance add failed

	m_mapId=mgr->GetMapId();
	m_instanceId = mgr->GetInstanceID();

	m_mapMgr = mgr;
	OnPrePushToWorld();

	mgr->PushObject(this);

	// correct incorrect instance id's
	mSemaphoreTeleport = false;
	m_inQueue = false;
   
	event_Relocate();
	
	// call virtual function to handle stuff.. :P
	OnPushToWorld();
}

void Object::RemoveFromWorld(bool free_guid)
{
//	ASSERT(m_mapMgr);
	MapMgr * m = m_mapMgr;
	m_mapMgr = NULL;

	mSemaphoreTeleport = true;

	if( m ) 
		m->RemoveObject(this, free_guid);
	
	// update our event holder
	event_Relocate();

//	m_instanceId = 0;
//	m_mapId = 0;
	//m_inQueue is set to true when AddToWorld() is called. AddToWorld() queues the Object to be pushed, but if it's not pushed and RemoveFromWorld()
	//is called, m_inQueue will still be true even if the Object is no more inworld, nor queued.
	m_inQueue = false;
}

//! Set uint32 property
void Object::SetUInt32Value( const uint32 index, const uint32 value )
{
	ASSERT( index < m_valuesCount );
	// save updating when val isn't changing.
	if(m_uint32Values[index] == value)
	{ 
		return;
	}

	m_uint32Values[ index ] = value;

	if(IsInWorld())
	{
		m_updateMask.SetBit( index );

		if(!m_objectUpdated)
		{
			m_mapMgr->ObjectUpdated(this);
			m_objectUpdated = true;
		}
	}

	// Group update handling
	if(m_objectTypeId == TYPEID_PLAYER)
	{
		if(IsInWorld())
		{
			Group* pGroup = SafePlayerCast( this )->GetGroup();
			if( pGroup != NULL )
				pGroup->HandleUpdateFieldChange( index, SafePlayerCast( this ) );
		}
	}
}

//! Set uint32 property
void Object::SetInt32Value( const uint32 index, const int32 value )
{
	ASSERT( index < m_valuesCount );
	// save updating when val isn't changing.
	if(m_int32Values[index] == value)
	{ 
		return;
	}

	m_int32Values[ index ] = value;

	if(IsInWorld())
	{
		m_updateMask.SetBit( index );

		if(!m_objectUpdated)
		{
			m_mapMgr->ObjectUpdated(this);
			m_objectUpdated = true;
		}
	}

	// Group update handling
	if(m_objectTypeId == TYPEID_PLAYER)
	{
		if(IsInWorld())
		{
			Group* pGroup = SafePlayerCast( this )->GetGroup();
			if( pGroup != NULL )
				pGroup->HandleUpdateFieldChange( index, SafePlayerCast( this ) );
		}
	}
}

/*
//must be in %
void Object::ModPUInt32Value(const uint32 index, const int32 value, bool apply )
{
	ASSERT( index < m_valuesCount );
	int32 basevalue = (int32)m_uint32Values[ index ];
	if(apply)
		m_uint32Values[ index ] += ((basevalue*value)/100);
	else
		m_uint32Values[ index ] = (basevalue*100)/(100+value);

	if(IsInWorld())
	{
		m_updateMask.SetBit( index );

		if(!m_objectUpdated )
		{
			m_mapMgr->ObjectUpdated(this);
			m_objectUpdated = true;
		}
	}
}
*/
uint32 Object::GetModPUInt32Value(const uint32 index, const int32 value)
{
	ASSERT( index < m_valuesCount );
	int32 basevalue = (int32)m_uint32Values[ index ];
	return ((basevalue*value)/100);
}

void Object::ModUnsigned32Value(uint32 index, int32 mod)
{
	ASSERT( index < m_valuesCount );
	if(mod == 0)
	{ 
		return;
	}

	m_int32Values[ index ] += mod;
	if( m_int32Values[index] < 0 )
		m_int32Values[index] = 0;

	if(IsInWorld())
	{
		m_updateMask.SetBit( index );

		if(!m_objectUpdated)
		{
			m_mapMgr->ObjectUpdated(this);
			m_objectUpdated = true;
		}
	}
}

void Object::ModSignedInt32Value(uint32 index, int32 value )
{
	ASSERT( index < m_valuesCount );
	if(value == 0)
	{ 
		return;
	}

	m_uint32Values[ index ] += value;
	if(IsInWorld())
	{
		m_updateMask.SetBit( index );

		if(!m_objectUpdated)
		{
			m_mapMgr->ObjectUpdated(this);
			m_objectUpdated = true;
		}
	}
}

void Object::ModFloatValue(const uint32 index, const float value )
{
	ASSERT( index < m_valuesCount );
	m_floatValues[ index ] += value;

	if(IsInWorld())
	{
		m_updateMask.SetBit( index );

		if(!m_objectUpdated)
		{
			m_mapMgr->ObjectUpdated(this);
			m_objectUpdated = true;
		}
	}
}
void Object::ModFloatValueByPCT( const uint32 index, int32 byPct )
{
	ASSERT( index < m_valuesCount );
	if( byPct > 0 )
		m_floatValues[ index ] *= 1.0f + float( byPct ) / 100.0f;
	else
		m_floatValues[ index ] /= 1.0f + float( -byPct ) / 100.0f;


	if( IsInWorld() )
	{
		m_updateMask.SetBit( index );

		if( !m_objectUpdated )
		{
			m_mapMgr->ObjectUpdated( this );
			m_objectUpdated = true;
		}
	}
}

//! Set uint64 property
void Object::SetUInt64Value( const uint32 index, const uint64 value )
{
	assert( index + 1 < m_valuesCount );
#ifndef USING_BIG_ENDIAN
	if(m_uint32Values[index] == GUID_LOPART(value) && m_uint32Values[index+1] == GUID_HIPART(value))
	{ 
		return;
	}

	m_uint32Values[ index ] = *((uint32*)&value);
	m_uint32Values[ index + 1 ] = *(((uint32*)&value) + 1);
#else
	m_uint32Values[index] = value & 0xffffffff;
	m_uint32Values[index+1] = (value >> 32) & 0xffffffff;
#endif

	if(IsInWorld())
	{
		m_updateMask.SetBit( index );
		m_updateMask.SetBit( index + 1 );

		if(!m_objectUpdated)
		{
			m_mapMgr->ObjectUpdated(this);
			m_objectUpdated = true;
		}
	}
}

//! Set float property
void Object::SetFloatValue( const uint32 index, const float value )
{
	ASSERT( index < m_valuesCount );
	if(m_floatValues[index] == value)
	{ 
		return;
	}

	m_floatValues[ index ] = value;
	
	if(IsInWorld())
	{
		m_updateMask.SetBit( index );

		if(!m_objectUpdated)
		{
			m_mapMgr->ObjectUpdated(this);
			m_objectUpdated = true;
		}
	}
}


void Object::SetFlag( const uint32 index, uint32 newFlag )
{
	ASSERT( index < m_valuesCount );

	//no change -> no update
	if((m_uint32Values[ index ] & newFlag)==newFlag)
	{ 
		return;
	}

	m_uint32Values[ index ] |= newFlag;

	if(IsInWorld())
	{
		m_updateMask.SetBit( index );

		if(!m_objectUpdated)
		{
			m_mapMgr->ObjectUpdated(this);
			m_objectUpdated = true;
		}
	}
}


void Object::RemoveFlag( const uint32 index, uint32 oldFlag )
{
	ASSERT( index < m_valuesCount );

	//no change -> no update
	if((m_uint32Values[ index ] & oldFlag)==0)
	{ 
		return;
	}

	m_uint32Values[ index ] &= ~oldFlag;

	if(IsInWorld())
	{
		m_updateMask.SetBit( index );

		if(!m_objectUpdated)
		{
			m_mapMgr->ObjectUpdated(this);
			m_objectUpdated = true;
		}
	}
}

void Object::RemoveFlagForceUpdate( uint32 index, uint32 oldFlag )
{
	ASSERT( index < m_valuesCount );

	m_uint32Values[ index ] &= ~oldFlag;

	if(IsInWorld())
	{
		m_updateMask.SetBit( index );

		if(!m_objectUpdated)
		{
			m_mapMgr->ObjectUpdated(this);
			m_objectUpdated = true;
		}
	}
}

void Object::SetUpdateMask( const uint32 index )
{
	ASSERT( index < m_valuesCount );
	if( IsInWorld() )
	{
		m_updateMask.SetBit( index );
		if( !m_objectUpdated )
		{
			m_mapMgr->ObjectUpdated( this );
			m_objectUpdated = true;
		}
	}
}

void Object::ToggleFlagForceUpdate( uint32 index, uint32 oldFlag )
{
	ASSERT( index < m_valuesCount );

	if( HasFlag( index, oldFlag ) )
		RemoveFlag( index, oldFlag );
	else
		SetFlag( index, oldFlag );
	m_updateMask.SetBit( index );
	if( IsInWorld() && !m_objectUpdated )
		m_mapMgr->ObjectUpdated(this);
	m_objectUpdated = true;
}

void Object::SetByteFlag(const uint32 index, const uint32 flag, uint8 newFlag)
{
   if( HasByteFlag(index,flag,newFlag))
   { 
      return;
   }

   SetByte(index, flag, GetByte(index,flag)|newFlag);

   if(IsInWorld())
   {
      m_updateMask.SetBit( index );

      if(!m_objectUpdated)
      {
         m_mapMgr->ObjectUpdated( this );
         m_objectUpdated = true;
      }
   }
}

void Object::RemoveByteFlag(const uint32 index, const uint32 flag, uint8 checkFlag)
{
   if( !HasByteFlag(index,flag,checkFlag))
   { 
      return;
   }

   SetByte(index,flag, GetByte(index,flag) & ~checkFlag );

   if(IsInWorld())
   {
      m_updateMask.SetBit( index );

      if(!m_objectUpdated)
      {
         m_mapMgr->ObjectUpdated( this );
         m_objectUpdated = true;
      }
   }
}

bool Object::HasByteFlag(const uint32 index, const uint32 flag, uint8 checkFlag)
{
   if( GetByte(index,flag) & checkFlag )
   { 
      return true;
   }
   else
      return false;
}
////////////////////////////////////////////////////////////

float Object::CalcDistance(Object *Ob)
{
	ASSERT(Ob != NULL);
	return CalcDistance(this->GetPositionX(), this->GetPositionY(), this->GetPositionZ(), Ob->GetPositionX(), Ob->GetPositionY(), Ob->GetPositionZ());
}
float Object::CalcDistance(float ObX, float ObY, float ObZ)
{
	return CalcDistance(this->GetPositionX(), this->GetPositionY(), this->GetPositionZ(), ObX, ObY, ObZ);
}
float Object::CalcDistance(Object *Oa, Object *Ob)
{
	ASSERT(Oa != NULL);
	ASSERT(Ob != NULL);
	return CalcDistance(Oa->GetPositionX(), Oa->GetPositionY(), Oa->GetPositionZ(), Ob->GetPositionX(), Ob->GetPositionY(), Ob->GetPositionZ());
}
float Object::CalcDistance(Object *Oa, float ObX, float ObY, float ObZ)
{
	ASSERT(Oa != NULL);
	return CalcDistance(Oa->GetPositionX(), Oa->GetPositionY(), Oa->GetPositionZ(), ObX, ObY, ObZ);
}

float Object::CalcDistance(float OaX, float OaY, float OaZ, float ObX, float ObY, float ObZ)
{
	float xdest = OaX - ObX;
	float ydest = OaY - ObY;
	float zdest = OaZ - ObZ;
	return sqrtf(zdest*zdest + ydest*ydest + xdest*xdest);
}

bool Object::IsWithinDistInMap(Object* obj, const float dist2compare) const
{
	ASSERT(obj != NULL);
	float xdest = this->GetPositionX() - obj->GetPositionX();
	float ydest = this->GetPositionY() - obj->GetPositionY();
	float zdest = this->GetPositionZ() - obj->GetPositionZ();
	return sqrtf(zdest*zdest + ydest*ydest + xdest*xdest) <= dist2compare;
}

bool Object::IsWithinLOSInMap(Object* obj)
{
	ASSERT(obj != NULL);
    if (!IsInMap(obj)) 
    { 
    	return false;
    }
	LocationVector location;
    location = obj->GetPosition();
    return IsWithinLOS(location );
}

bool Object::IsWithinLOS( LocationVector location )
{
    LocationVector location2;
    location2 = GetPosition();

	if (sWorld.Collision) {
	{ 
		return CollideInterface.CheckLOS(GetMapId(), location2.x, location2.y, location2.z+2.0f, location.x, location.y, location.z+2.0f);
	}
	} else {
		return true;
	}
}


float Object::calcAngle( float Position1X, float Position1Y, float Position2X, float Position2Y )
{
	float dx = Position2X-Position1X;
	float dy = Position2Y-Position1Y;
	double angle=0.0f;

	// Calculate angle
	if (dx == 0.0)
	{
		if (dy == 0.0)
			angle = 0.0;
		else if (dy > 0.0)
			angle = M_PI * 0.5 /* / 2 */;
		else
			angle = M_PI * 3.0 * 0.5/* / 2 */;
	}
	else if (dy == 0.0)
	{
		if (dx > 0.0)
			angle = 0.0;
		else
			angle = M_PI;
	}
	else
	{
		if (dx < 0.0)
			angle = atanf(dy/dx) + M_PI;
		else if (dy < 0.0)
			angle = atanf(dy/dx) + (2*M_PI);
		else
			angle = atanf(dy/dx);
	}

	// Convert to degrees
	angle = angle * float(180 / M_PI);

	// Return
	return float(angle);
}

float Object::calcRadAngle( float Position1X, float Position1Y, float Position2X, float Position2Y )
{
	double dx = double(Position2X-Position1X);
	double dy = double(Position2Y-Position1Y);
	double angle=0.0;

	// Calculate angle
	if (dx == 0.0)
	{
		if (dy == 0.0)
			angle = 0.0;
		else if (dy > 0.0)
			angle = M_PI * 0.5/*/ 2.0*/;
		else
			angle = M_PI * 3.0 * 0.5/*/ 2.0*/;
	}
	else if (dy == 0.0)
	{
		if (dx > 0.0)
			angle = 0.0;
		else
			angle = M_PI;
	}
	else
	{
		if (dx < 0.0)
			angle = atan(dy/dx) + M_PI;
		else if (dy < 0.0)
			angle = atan(dy/dx) + (2*M_PI);
		else
			angle = atan(dy/dx);
	}

	// Return
	return float(angle);
}

float Object::getEasyAngle( float angle )
{
	int div_res = float2int32( angle / 360 );
	angle -= div_res*360;
	return angle;
}

bool Object::inArc(float Position1X, float Position1Y, float FOV, float Orientation, float Position2X, float Position2Y )
{
	float angle = calcAngle( Position1X, Position1Y, Position2X, Position2Y );
	float lborder = getEasyAngle( ( Orientation - (FOV*0.5f/*/2*/) ) );
	float rborder = getEasyAngle( ( Orientation + (FOV*0.5f/*/2*/) ) );
	//sLog.outDebug("Orientation: %f Angle: %f LeftBorder: %f RightBorder %f",Orientation,angle,lborder,rborder);
	if(((angle >= lborder) && (angle <= rborder)) || ((lborder > rborder) && ((angle < rborder) || (angle > lborder))))
	{
		return true;
	}
	else
	{
		return false;
	}
} 

//is the target in front of object ?
bool Object::isInFront(Object* target)
{
	// check if we facing something ( is the object within a 180 degree slice of our positive y axis )

    float x = target->GetPositionX() - m_position.x;
    float y = target->GetPositionY() - m_position.y;

    float angle = atan2( y, x );
    angle = ( angle >= 0.0 ) ? angle : 2.0f * M_PI + angle;
	angle -= m_position.o;

	int div_res;	//must be int to get the modulo
	div_res = float2int32( angle / ( 2.0f * M_PI ) );
	angle -= div_res * (2.0f * M_PI);

	// replace M_PI in the two lines below to reduce or increase angle

    float left = -1.0f * ( M_PI / 2.0f );
    float right = ( M_PI / 2.0f );

    return( ( angle >= left ) && ( angle <= right ) );
}

bool Object::isInBack(Object* target)
{
	// check if we are behind something ( is the object within a 180 degree slice of our negative y axis )

    float x = m_position.x - target->GetPositionX();
    float y = m_position.y - target->GetPositionY();

    float angle = atan2( y, x );
    angle = ( angle >= 0.0f ) ? angle : 2.0f * M_PI + angle;

	// if we are a unit and have a UNIT_FIELD_TARGET then we are always facing them
	if( m_objectTypeId == TYPEID_UNIT && m_uint32Values[UNIT_FIELD_TARGET] != 0 && SafeUnitCast( this )->GetAIInterface()->GetNextTarget() )
	{
		Unit* pTarget = SafeUnitCast( this )->GetAIInterface()->GetNextTarget();
		angle -= float( Object::calcRadAngle( target->m_position.x, target->m_position.y, pTarget->m_position.x, pTarget->m_position.y ) );
	}
	else
		angle -= target->GetOrientation();

	int div_res;	//must be int to get the modulo
	div_res = float2int32( angle / ( 2.0f * M_PI ) );
	angle -= div_res * (2.0f * M_PI);

	// replace M_H_PI in the two lines below to reduce or increase angle
    return( ( angle <= -M_Q_PI ) && ( angle >= M_Q_PI ) );
}
bool Object::isInArc(Object* target , float angle) // angle in degrees
{
    return inArc( GetPositionX() , GetPositionY() , angle , GetOrientation() , target->GetPositionX() , target->GetPositionY() );
}

bool Object::HasInArc( float degrees, Object* target )
{
	return isInArc(target, degrees);
}

bool Object::isInRange(Object* target, float range)
{
	float dist = CalcDistance( target );
	return( dist <= range );
}

bool Object::IsPet()
{
	if( IsUnit() == false )
	{ 
		return false;
	}

	if( internal_object_type & INTERNAL_OBJECT_TYPE_PET )
	{
		return true;
	}

/*	if( m_uint32Values[UNIT_FIELD_CREATEDBY] != 0 && m_uint32Values[UNIT_FIELD_SUMMONEDBY] != 0 )
	{ 
		return true;
	}*/

	return false;
}

void Object::_setFaction()
{
	FactionTemplateDBC* factT = NULL;

	if(GetTypeId() == TYPEID_UNIT || GetTypeId() == TYPEID_PLAYER)
	{
		factT = dbcFactionTemplate.LookupEntry(GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE));
		if( !factT )
			sLog.outDetail("Unit does not have a valid faction. It will make him act stupid in world. Don't blame us, blame yourself for not checking :P, faction %u set to entry %u",GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE),GetUInt32Value(OBJECT_FIELD_ENTRY) );
	}
	else
	if(GetTypeId() == TYPEID_GAMEOBJECT)
	{
		factT = dbcFactionTemplate.LookupEntry(GetUInt32Value(GAMEOBJECT_FACTION));
		if( !factT )
			sLog.outDetail("Game Object does not have a valid faction. It will make him act stupid in world. Don't blame us, blame yourself for not checking :P, faction %u set to entry %u",GetUInt32Value(GAMEOBJECT_FACTION),GetUInt32Value(OBJECT_FIELD_ENTRY) );
	}

	if(!factT)
	{
		factT = dbcFactionTemplate.LookupRow( 0 );
		//this is causeing a lot of crashes cause people have shitty dbs
//		return;
	}
	m_faction = factT;
	m_factionDBC = dbcFaction.LookupEntry(factT->Faction);
	if( !m_factionDBC )
		m_factionDBC = dbcFaction.LookupRow( 0 );
}

/*
void Object::UpdateOppFactionSet()
{
	m_oppFactsInRange.clear();
	this->AquireInrangeLock(); //make sure to release lock before exit function !
	for(InRangeSet::iterator i = GetInRangeSetBegin(); i != GetInRangeSetEnd(); ++i)
	{
		if (((*i)->GetTypeId() == TYPEID_UNIT) || ((*i)->GetTypeId() == TYPEID_PLAYER) || ((*i)->GetTypeId() == TYPEID_GAMEOBJECT))
		{
			if (isHostile(this, (*i)))
			{
				if(!(*i)->IsInRangeOppFactSet(this))
					(*i)->m_oppFactsInRange.insert(this);
				if (!IsInRangeOppFactSet((*i)))
					m_oppFactsInRange.insert((*i));
				
			}
			else
			{
				if((*i)->IsInRangeOppFactSet(this))
					(*i)->m_oppFactsInRange.erase(this);
				if (IsInRangeOppFactSet((*i)))
					m_oppFactsInRange.erase((*i));
			}
		}
	}
	this->ReleaseInrangeLock();
}*/

/*
void Object::UpdateSameFactionSet()
{
	m_sameFactsInRange.clear();
	this->AquireInrangeLock(); //make sure to release lock before exit function !
	for(InRangeSet::iterator i = GetInRangeSetBegin(); i != GetInRangeSetEnd(); ++i)
	{
		if (((*i)->GetTypeId() == TYPEID_UNIT) || ((*i)->GetTypeId() == TYPEID_PLAYER) || ((*i)->GetTypeId() == TYPEID_GAMEOBJECT))
		{
			if (isFriendly(this, (*i)))
			{
				if(!(*i)->IsInRangeSameFactSet(this))
					(*i)->m_sameFactsInRange.insert(this);
				if (!IsInRangeOppFactSet((*i)))
					m_sameFactsInRange.insert((*i));
				
			}
			else
			{
				if((*i)->IsInRangeSameFactSet(this))
					(*i)->m_sameFactsInRange.erase(this);
				if (IsInRangeSameFactSet((*i)))
					m_sameFactsInRange.erase((*i));
			}
		}
	}
	this->ReleaseInrangeLock();
}*/

#ifdef BATTLEGRUND_REALM_BUILD
void CalcAndGiveGoldToBGPlayer( Player *p, int32 is_winner )
{
	int64 *last_gold_reward_stamp = p->GetCreateIn64Extension( EXTENSION_ID_BG_REALM_GOLD_STAMP );
	int32 player_level = p->getLevel();
	//should not happen
	if( *last_gold_reward_stamp == 0 ) 
		*last_gold_reward_stamp = getMSTime();
	// we get gold as GPS ( gold per second )
	// right now avg item price is 85 gold. We intend to be able to buy 1 item / day at level 85
	int32 avg_item_price = 400000;
	int32 item_buy_day = 1;
	int32 expected_playhours_day = 4;
//	int32 item_price_kill_pct_bonus_10x = 5;
	int32 GPS = avg_item_price / ( item_buy_day * expected_playhours_day * 60 * 60 );
	int64 time_diff = (int64)getMSTime() - (*last_gold_reward_stamp);
//	int64 gold_reward = ( time_diff * GPS / 1000 ) / ( 1 + (PLAYER_LEVEL_CAP_BLIZZLIKE - player_level ) / 4 );
	int64 gold_reward = ( time_diff * GPS / 1000 ) ;

	//we made the killer blow ? Extra gold then
//	if( is_winner == 1 )
//		gold_reward += avg_item_price * item_price_kill_pct_bonus_10x / 1000;

	//looser gets only half of the gold
//	if( is_winner == 0 )
//		gold_reward = gold_reward * 3 / 2;
	gold_reward += RandomUInt() % ( 1 + gold_reward * 10 / 100 ); //let's give a bit of luck
	gold_reward = int64( gold_reward * sWorld.getRate(RATE_MONEY) );
	if( p->GetGroup() == NULL )
		gold_reward = gold_reward / 2;

	//award the reward
	sStackWorldPacket( pkt, SMSG_LOOT_MONEY_NOTIFY, 4 + 10 );
	pkt << (uint32)( gold_reward );
	p->GetSession()->SendPacket( &pkt );
	p->ModGold( gold_reward);
	//mark the stamp when we last cached in the rewards
	*last_gold_reward_stamp = getMSTime();
}
#endif

void Object::EventSetUInt32Value(uint32 index, uint32 value)
{
	SetUInt32Value(index,value);
}

//This is created with statitics data based on bugs found inside emu! Needs to be update periodically
//attacker is index[0] and victim is index [1] => table[a][v] = chance to win the fight / 4
//X% of extra win chance of attacker is converted into victim defense
//Y% of extra loose chance of victim is converted into victim dmg
//X+Y can differ from 100% because more defense mod is used then DMG
//!!!values are relative to previous forced balancing !!
float PVP_forced_balancing[12][12] = 
{
//   none     warr     pala     hunter   rogue    priest   DK       shaman   mage     warlock  none     druid    
    {0.0000f, 0.0000f, 0.0000f, 0.0000f, 0.0000f, 0.0000f, 0.0000f, 0.0000f, 0.0000f, 0.0000f, 0.0000f, 0.0000f}, // none
    {0.0000f, 0.0000f, 1.0380f, 0.5635f, 0.4665f, 0.5635f, 1.3246f, 0.8376f, 0.9355f, 0.4772f, 0.0000f, 0.4690f}, // warr
    {0.0000f, 0.9619f, 0.0000f, 0.9825f, 0.7474f, 0.7858f, 1.2724f, 0.8174f, 0.9744f, 0.4853f, 0.0000f, 1.0681f}, // pala
    {0.0000f, 1.0364f, 1.0175f, 0.0000f, 0.9649f, 1.0324f, 1.1403f, 0.9690f, 0.8486f, 0.9583f, 0.0000f, 1.1547f}, // hunter
    {0.0000f, 1.1334f, 1.2525f, 1.0350f, 0.0000f, 0.4744f, 1.1867f, 1.3142f, 0.9040f, 0.5222f, 0.0000f, 1.0911f}, // rogue
    {0.0000f, 1.0364f, 1.2141f, 0.9675f, 1.1255f, 0.0000f, 1.2546f, 1.1298f, 0.9003f, 0.9655f, 0.0000f, 0.6985f}, // priest
    {0.0000f, 0.6753f, 0.7276f, 0.8596f, 0.4132f, 0.7453f, 0.0000f, 0.6755f, 0.8246f, 0.7331f, 0.0000f, 0.8211f}, // DK
    {0.0000f, 1.1623f, 1.1825f, 1.0309f, 0.6857f, 0.8701f, 1.3244f, 0.0000f, 0.9352f, 0.8939f, 0.0000f, 0.8438f}, // shaman
    {0.0000f, 1.0644f, 1.0255f, 1.1513f, 1.0959f, 1.0996f, 1.1753f, 1.0647f, 0.0000f, 0.8410f, 0.0000f, 0.7677f}, // mage
    {0.0000f, 1.1227f, 1.1146f, 1.0416f, 1.0777f, 1.0344f, 1.2668f, 1.1061f, 1.1589f, 0.0000f, 0.0000f, 1.1898f}, // warlock
    {0.0000f, 0.0000f, 0.0000f, 0.0000f, 0.0000f, 0.0000f, 0.0000f, 0.0000f, 0.0000f, 0.0000f, 0.0000f, 0.0000f}, // none
    {0.0000f, 1.1310f, 0.9318f, 0.8452f, 0.9088f, 1.3014f, 1.1788f, 1.1561f, 1.2322f, 0.8101f, 0.0000f, 0.0000f}, // druid
};

void Object::DealDamage(Unit *pVictim, uint32 damage, uint32 spellId, bool no_remove_auras)
{
	Player* plr = 0;

#ifdef FORCED_GM_TRAINEE_MODE
	if( this->IsPlayer() && SafePlayerCast( this )->GetSession() && SafePlayerCast( this )->GetSession()->HasPermissions() && !SafePlayerCast( this )->GetSession()->HasGMPermissions() )
		return; //no DMG by trainees
#endif
	if( this->IsPlayer() && SafePlayerCast( this )->GetSession() && !SafePlayerCast( this )->GetSession()->HasPermissions() && !SafePlayerCast( this )->GetSession()->HasGMPermissions() )
	{
		//wtf big dmg ?
		if( damage > 200000 )
		{
			SafePlayerCast( this )->BroadcastMessage( "Damage %d ignored until bug is fixed. Report how to reproduce this to get it fixed : spell id %d",damage,spellId);
			return;
		}
		if( SafePlayerCast( this )->IsSpectator() )
		{
			return;
		}
	}

	if( !pVictim || !pVictim->isAlive() || !pVictim->IsInWorld() || !IsInWorld() )
	{ 
		return;
	}
	if( pVictim->bInvincible )
	{
		return;
	}
	if( pVictim->IsSpiritHealer() )
	{ 
		return;
	}

	//this is emu crap balancer - convert win / loose chance into a DMG mod
/*	if( this->IsUnit() && PVP_forced_balancing[ SafeUnitCast(this)->getClass() ][ pVictim->getClass() ] != 0.0f )
	{
		float dmg_mod = PVP_forced_balancing[ SafeUnitCast(this)->getClass() ][ pVictim->getClass() ];
		uint32 new_dmg = float2int32( damage * dmg_mod );
		//sanity check to make sure nothing went uber wrong
		if( new_dmg < damage / 2 )
			new_dmg = damage / 2;
		else if( new_dmg < damage * 2 ) 
			new_dmg = damage * 2;
		damage = new_dmg;
	}/**/

	if( this->IsUnit() && pVictim->IsUnit() && pVictim != this )
	{
		// Set our attack target to the victim.
		if( sWorld.AnyKillerBlowReduction && sWorld.AnyKillerBlowBelow > damage && pVictim->IsPlayer() )
		{
			uint32 max_dmg = (int32)( pVictim->GetUInt32Value( UNIT_FIELD_MAXHEALTH ) / sWorld.AnyKillerBlowReduction ) ;
			if( damage > max_dmg )
				damage = max_dmg;
		}
/*		//patch for beez : weapon X does Y phisical DMG all the time. Strongly hardcoded shit
		if( spellId == 0					//needs phizical weapon
			&& pVictim->GetEntry() == 1715	//change mob entry here !
			&& this->IsPlayer()										//it's a must to have a weapon
			&& SafePlayerCast( this )->GetItemInterface()	//can never happen
			&& SafePlayerCast( this )->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_MAINHAND )	//probably true if we get here
			&& SafePlayerCast( this )->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_MAINHAND )->GetEntry() == 25 //change item entry here !
			)
			damage = 50000;	// change dmg here !!
			*/
	}

	//watched video and DOTS did not break standing. 
	if( pVictim->GetStandState() )//not standing-> standup
	{
		if( spellId == 0 || dbcSpell.LookupEntryForced( spellId )->quick_tickcount <= 1 )
			pVictim->SetStandState( STANDSTATE_STAND );//probably mobs also must standup
	}

	// This one is easy. If we're attacking a hostile target, and we're not flagged, flag us.
	// Also, you WONT get flagged if you are dueling that person - FiShBaIt
	if( pVictim->IsPlayer() )
	{
		if( IsPlayer() )
		{	
			if( isHostile( this, pVictim ) && SafePlayerCast( pVictim )->DuelingWith != SafePlayerCast( this ) )
				SafePlayerCast( this )->SetPvPFlag();
		}

		//If our pet attacks  - flag us.
		if( IsPet() )
		{
			Player* owner = SafePetCast( this )->GetPetOwner() ;
			if( owner != NULL )
			{  
				if( owner->isAlive() && SafePlayerCast( pVictim )->DuelingWith != owner )
					owner->SetPvPFlag();
			}
		}
		else if( IsCreature() &&  SafeCreatureCast( this )->IsTotem() )
		{
			Player* owner = SafeCreatureCast( this )->GetTotemOwner() ;
			if( owner != NULL && owner->isAlive() && SafePlayerCast( pVictim )->DuelingWith != owner )
				owner->SetPvPFlag();		
		}
	}

	if(!no_remove_auras)
	{
		//zack 2007 04 24 : root should not remove self (and also other unknown spells)
		if(spellId)
		{
			pVictim->RemoveAurasByInterruptFlagButSkip(AURA_INTERRUPT_ON_ANY_DAMAGE_TAKEN,spellId,0,damage);
//			if(Rand(35))
//				pVictim->RemoveAurasByInterruptFlagButSkip(AURA_INTERRUPT_ON_ANY_DAMAGE_TAKEN,spellId);
		}
		else
		{
			pVictim->RemoveAurasByInterruptFlagButSkip(AURA_INTERRUPT_ON_ANY_DAMAGE_TAKEN,0,0,damage);
//			if(Rand(35))
//				pVictim->RemoveAurasByInterruptFlag(AURA_INTERRUPT_ON_ANY_DAMAGE_TAKEN);
		}
	}

	if(this->IsUnit())
	{
		if( pVictim->IsPlayer() )
		{
			if( pVictim->CombatStatus.IsInCombat() )
				sHookInterface.OnEnterCombat( SafePlayerCast( pVictim ), SafeUnitCast( this ) );
			SafePlayerCast( pVictim )->m_dmg_received_since_login += damage;
		}

		if(IsPlayer() && ! SafePlayerCast( this )->CombatStatus.IsInCombat() == true )
			sHookInterface.OnEnterCombat( SafePlayerCast( this ), SafePlayerCast( this ) );

		//the black sheep , no actually it is paladin : Ardent Defender
//		if(SafeUnitCast(this)->DamageTakenPctModOnHP35 && SafeUnitCast(this)->HasAuraStateFlag( AURASTATE_HEALTH_BELOW_35_PCT) )
//			damage = damage - float2int32(damage * SafeUnitCast(this)->DamageTakenPctModOnHP35) / 100 ;
			
		plr = 0;
		if(IsPet())
			plr = SafePetCast(this)->GetPetOwner();
		else if(IsPlayer())
		{
			plr = SafePlayerCast( this );
			plr->m_dmg_made_since_login += damage;
			plr->m_dmg_made_last_time = damage;
		}

		if(pVictim->GetTypeId()==TYPEID_UNIT && plr && plr->GetTypeId() == TYPEID_PLAYER) // Units can't tag..
		{
			// Tagging
			Creature *victim = SafeCreatureCast(pVictim);
			bool taggable;
			if(victim->GetCreatureInfo() && (victim->GetCreatureInfo()->Type == CRITTER || victim->IsPet()))
				taggable = false;
			else 
				taggable = true;

			if(taggable && victim->TaggerGroupId == NULL && victim->TaggerGuid == 0 )
			{
//				victim->Tagged = true;
				//try to tag it as group + as person
				victim->TaggerGroupId = plr->GetGroup();
				victim->TaggerGuid = plr->GetGUID();

				/* set loot method */
				if( plr->GetGroup() != NULL )
					victim->m_lootMethod = plr->GetGroup()->GetMethod();

				// For new players who get a create object
				uint32 Flags = pVictim->m_uint32Values[UNIT_DYNAMIC_FLAGS];
				Flags |= U_DYN_FLAG_TAPPED_BY_PLAYER;

				pVictim->m_uint32Values[UNIT_DYNAMIC_FLAGS] |= U_DYN_FLAG_TAGGED_BY_OTHER;

				// Update existing players.
				ByteBuffer buf(500);
				ByteBuffer buf1(500);

				pVictim->BuildFieldUpdatePacket(&buf1, UNIT_DYNAMIC_FLAGS, Flags);
				pVictim->BuildFieldUpdatePacket(&buf, UNIT_DYNAMIC_FLAGS, pVictim->m_uint32Values[UNIT_DYNAMIC_FLAGS]);

				// Loop inrange set, append to their update data.
				AquireInrangeLock();
				InrangeLoopExitAutoCallback AutoLock;
				for(InRangePlayerSetRecProt::iterator itr = GetInRangePlayerSetBegin( AutoLock ); itr != GetInRangePlayerSetEnd(); ++itr)
				{
					if (SafePlayerCast(plr)->InGroup())
					{
						if (SafePlayerCast(*itr)->GetGroup() && SafePlayerCast(plr)->GetGroup()->GetID() == SafePlayerCast(*itr)->GetGroup()->GetID())
						{
							(*itr)->PushUpdateData(&buf1, 1);
						} 
						else
						{
							(*itr)->PushUpdateData(&buf, 1);
						}
						
					} 
					else
					{
						(*itr)->PushUpdateData(&buf, 1);
					}
				}
				ReleaseInrangeLock();

				// Update ourselves
				plr->PushUpdateData(&buf1, 1);

			}
		}		
	}

    //Rage : only for real damage. The function is split in 3 parts because some script do not use "strike" or "spellnonmeleedmglog" to deal dmg
	if( pVictim->GetPowerType() == POWER_TYPE_RAGE && pVictim != this && this->IsUnit() )
		pVictim->RageEventDamageTaken( damage, SafeUnitCast( this ) );

/*	if( pVictim->IsPlayer() )
	{
		Player *pThis = SafePlayerCast(pVictim);
		if(pThis->cannibalize)
		{
			sEventMgr.RemoveEvents(pVictim, EVENT_CANNIBALIZE);
			pThis->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
			pThis->cannibalize = false;
		}
	} */

	//* BATTLEGROUND DAMAGE COUNTER *//
	if( pVictim != this )
	{
		if( IsPlayer() )
		{
			plr = SafePlayerCast( this );
		}
		else if( IsPet() )
		{
			plr = SafePetCast( this )->GetPetOwner();
			if (plr)
				if( plr != NULL && plr->GetMapMgr() == GetMapMgr() )
					plr = NULL;
		}

		if( plr != NULL && plr->m_bg != NULL && plr->GetMapMgr() == GetMapMgr() )
		{
			plr->m_bgScore.DamageDone += damage;
			plr->Event_AchiementCriteria_Received( ACHIEVEMENT_CRITERIA_TYPE_DAMAGE_DONE,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,damage,ACHIEVEMENT_EVENT_ACTION_ADD);
			//strange huh ? 2 objects on same map but one has fucked up BG pointer
//			if( BattlegroundManager.IsValidBGPointer( plr->m_bg ) )
//				plr->m_bg->UpdatePvPData();
//			else
//				plr->m_bg = NULL;
		}
	}
   
	uint32 health = pVictim->GetUInt32Value( UNIT_FIELD_HEALTH );

	/*------------------------------------ DUEL HANDLERS --------------------------*/
	//Both Players. Victim is receiving dmg from dueling player
	if( health <= damage && pVictim->IsPlayer() == true && SafePlayerCast(pVictim)->DuelingWith != NULL && this->IsUnit() == true )
	{
		Unit *attacker = SafeUnitCast( this )->GetTopOwner(); //can be pet or totem...
		if( attacker->IsPlayer() == true && SafePlayerCast(pVictim)->DuelingWith == SafePlayerCast( attacker ) ) 
//		if( SafePlayerCast( attacker )->DuelingWith != NULL)
		{
			Player *pattacker = SafePlayerCast( attacker );
			//Player in Duel and Player Victim has lost
			uint32 NewHP = pVictim->GetUInt32Value(UNIT_FIELD_MAXHEALTH)/100;
			if(NewHP < 5) 
				NewHP = 5;
			//Set there health to 1% or 5 if 1% is lower then 5
			pVictim->SetUInt32Value(UNIT_FIELD_HEALTH, NewHP);
//			if( attacker )
				pattacker->EndDuel(DUEL_WINNER_KNOCKOUT);
//			else
//				SafePlayerCast( pVictim )->EndDuel( DUEL_WINNER_RETREAT );
			// surrender emote
			pVictim->Emote(EMOTE_ONESHOT_BEG);			// Animation
			// cast godmode to avoid accidental death due to queued events
			pVictim->CastSpell( pVictim, 1235, true );
			attacker->CastSpell( attacker, 1235, true );
			return;		  
		}
	}
	/*------------------------------------ DUEL HANDLERS END--------------------------*/

/*	bool isCritter = false;
	if(pVictim->IsCreature() && SafeCreatureCast(pVictim)->GetCreatureInfo())
	{
			if(SafeCreatureCast(pVictim)->GetCreatureInfo()->Type == CRITTER)
				isCritter = true;
	}*/
	/* -------------------------- HIT THAT CAUSES VICTIM TO DIE ---------------------------*/
//	if ((isCritter || health <= damage) )
	if ( health <= damage )
	{
		//general hook for die
		sHookInterface.OnPreUnitDie( SafeUnitCast( this ), pVictim);
		//warlock - seed of corruption
		if( IsUnit() )
		{
			SpellEntry *killerspell;
			if( spellId )
				killerspell = dbcSpell.LookupEntry( spellId );
			else killerspell = NULL;
			if( this != pVictim )
			{
				pVictim->HandleProc( PROC_ON_DIE_VICTIM, SafeUnitCast( this ), killerspell );
				SafeUnitCast( this )->HandleProc( PROC_ON_TARGET_DIE, pVictim, killerspell );
			}
			else
				pVictim->HandleProc( PROC_ON_DIE_VICTIM | PROC_ON_TARGET_DIE, SafeUnitCast( this ), killerspell );
		}
		// check if pets owner is combat participant
		bool owner_participe = false;
		if( IsPet() )
		{
			Player* owner = SafePetCast( this )->GetPetOwner();
			if( owner != NULL && pVictim->GetAIInterface()->getThreatByPtr( owner ) > 0 )
				owner_participe = true;
		}
#ifdef BATTLEGRUND_REALM_BUILD
		//when players take part of a fight they are actively playing and should be rewarded
		if( this->IsPlayer() && pVictim->IsPlayer() )
		{
			SafePlayerCast( this )->BGRealmPVPFightSupporters[ pVictim->GetGUID() ] = getMSTime();
			SafePlayerCast( pVictim )->BGRealmPVPFightSupporters[ this->GetGUID() ] = getMSTime();
		}
#endif
		/* -------------------------------- HONOR + BATTLEGROUND CHECKS ------------------------ */
		//Zack : this event should ocure before setting death state !
		plr = NULL;
		if( IsPlayer() )
			plr = SafePlayerCast( this );
		else if(IsPet())
			plr = SafePetCast( this )->GetPetOwner();

		if( plr != NULL)
		{
			//crap, we should check flags and map for this. But we do not have the support for it atm
			if( pVictim->IsPlayer() )
			{
#ifdef GENERATE_CLASS_BALANCING_STATISTICS
				if( plr->getLevel() == pVictim->getLevel() 
					&& plr->getClass() != pVictim->getClass() 
					&& plr->GetHealthPct() < 75
					)
				{
					Player *pvictim = SafePlayerCast( pVictim );
					//players should have similar gear - 100 means they have very similar gear score
					int32 plr1_str = plr->item_level_sum / ( plr->item_count_sum + 1 );
					int32 plr2_str = pvictim->item_level_sum / ( pvictim->item_count_sum + 1 );
					float difference_pct_score1 = ( (float)abs(plr1_str - plr2_str) / (float)( plr1_str + plr2_str + 1 ) );
					//no naked player testing dmg on other
					if( difference_pct_score1 <= 0.25 
						&& plr->CombatStatus.HealSize() < 2 && plr->CombatStatus.AttackersSize() < 2			//avoid imba group fights
						&& pvictim->CombatStatus.HealSize() < 2 && pvictim->CombatStatus.AttackersSize() < 2 )	//avoid imba group fights
						CharacterDatabase.Execute("Update war_balancer set c=c+1 where A=%d and v=%d",plr->getClass(),pVictim->getClass());
				}
#endif
				if( plr->m_bg != 0 )
					plr->m_bg->HookOnPlayerKill( plr, SafePlayerCast( pVictim ) );

				sHookInterface.OnKillPlayer( plr, SafePlayerCast( pVictim ) );
				bool setAurastateFlag = false;
				if(plr->getLevel() > pVictim->getLevel())
				{
					unsigned int diff = plr->getLevel() - pVictim->getLevel();
					if( diff <= 8 )
					{
						HonorHandler::OnPlayerKilledUnit(plr, pVictim);
						setAurastateFlag = true;
					}
				}
				else
				{
					HonorHandler::OnPlayerKilledUnit( plr, pVictim );
					setAurastateFlag = true;
				}

				if (setAurastateFlag)
				{
					if( sEventMgr.ModifyEventTimeLeft(this,EVENT_LASTKILLWITHHONOR_FLAG_EXPIRE,20000) == false)
						sEventMgr.AddEvent(SafeUnitCast(this),&Unit::EventAurastateExpire,(uint32)AURASTATE_LASTKILLWITHHONOR,EVENT_LASTKILLWITHHONOR_FLAG_EXPIRE,20000,1,0);
					if( SafeUnitCast(this)->HasAuraStateFlag(AURASTATE_LASTKILLWITHHONOR) == false )
						SafeUnitCast(this)->SetAuraStateFlag(AURASTATE_LASTKILLWITHHONOR); //got used up by a spell before expire
				}
			}
			else
			{
//				if (!isCritter) // REPUTATION
				{
					plr->Reputation_OnKilledUnit( pVictim, false );
				}
			}
			plr->Event_AchiementCriteria_Received( ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_HIT_DEALT,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,damage,ACHIEVEMENT_EVENT_ACTION_SET_MAX);
		}
		/* -------------------------------- END : HONOR + BATTLEGROUND CHECKS END------------------------ */

		/* victim died! */
		if( pVictim->IsPlayer() )
		{
			SafePlayerCast( pVictim )->KillPlayer();
			SafePlayerCast( pVictim )->Event_AchiementCriteria_Received( ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_HIT_RECEIVED,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,damage,ACHIEVEMENT_EVENT_ACTION_SET_MAX);
			SafePlayerCast( pVictim )->Event_AchiementCriteria_Received( ACHIEVEMENT_CRITERIA_TYPE_TOTAL_DAMAGE_RECEIVED,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,damage,ACHIEVEMENT_EVENT_ACTION_ADD);
			if( this->IsPlayer() )
			{
				if( pVictim != this )
				{
					SafePlayerCast( pVictim )->Event_AchiementCriteria_Received( ACHIEVEMENT_CRITERIA_TYPE_KILLED_BY_PLAYER,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
					plr->Event_AchiementCriteria_Received( ACHIEVEMENT_CRITERIA_TYPE_EARN_HONORABLE_KILL,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
					if( plr->m_bg )
						plr->Event_AchiementCriteria_Received( ACHIEVEMENT_CRITERIA_TYPE_GET_KILLING_BLOWS,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
				}
			}
			else
			{
				SafePlayerCast( pVictim )->Event_AchiementCriteria_Received( ACHIEVEMENT_CRITERIA_TYPE_KILLED_BY_CREATURE,GetUInt32Value( OBJECT_FIELD_ENTRY),ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
			}
		}
		else
		{
			pVictim->setDeathState( JUST_DIED );	//and add respawn event for corpse
			if( IsUnit() )
			{
				pVictim->GetAIInterface()->HandleEvent( EVENT_LEAVECOMBAT, SafeUnitCast( this ), 0);
				//Zack:but victim already died. We should try to get the attacker out of combat not the victim ? Or both ?
//				SafeUnitCast( this )->GetAIInterface()->HandleEvent( EVENT_LEAVECOMBAT, SafeUnitCast( this ), 0);
			}
			if( plr )
			{
				if( plr->GetGroup() )
				{
					Group * group = plr->GetGroup(); 
					for( uint8 k = 0; k < group->GetSubGroupCount(); k++ )
					{
						SubGroup * subgroup = group->GetSubGroup( k );
						if( subgroup )
							for( GroupMembersSet::iterator itr = subgroup->GetGroupMembersBegin(); itr != subgroup->GetGroupMembersEnd(); ++itr )
								if( (*itr)->m_loggedInPlayer )
									(*itr)->m_loggedInPlayer->Event_AchiementCriteria_Received( ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE,pVictim->GetUInt32Value( OBJECT_FIELD_ENTRY),ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
					}
				}
				else
					plr->Event_AchiementCriteria_Received( ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE,pVictim->GetUInt32Value( OBJECT_FIELD_ENTRY),ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
			}
		}

		if( pVictim->IsPlayer() && !IsPlayer())
		{
			SafePlayerCast( pVictim )->DeathDurabilityLoss(0.10);
		}

		/* Zone Under Attack */
        MapInfo * pMapInfo = WorldMapInfoStorage.LookupEntry(GetMapId());
        if( pMapInfo && IS_STATIC_MAP( pMapInfo ) && !pVictim->IsPlayer() && !pVictim->IsPet() && ( IsPlayer() || IsPet() ) )
		{
			// Only NPCs that bear the PvP flag can be truly representing their faction.
			if( SafeCreatureCast(pVictim)->HasFlag( UNIT_FIELD_FLAGS, UNIT_FLAG_PVP ) )
			{
				Player * pAttacker = NULL;
				if( IsPet() )
					pAttacker = SafePetCast( this )->GetPetOwner();
				else if(IsPlayer())
					pAttacker = SafePlayerCast( this );

				if( pAttacker != NULL)
                {
				    uint8 teamId = (uint8)pAttacker->GetTeam();
				    if(teamId == 0) // Swap it.
					    teamId = 1;
				    else
					    teamId = 0;
//				    uint32 AreaID = pVictim->GetMapMgr()->GetAreaID(pVictim->GetPositionX(), pVictim->GetPositionY());
				    uint32 AreaID = sTerrainMgr.GetAreaID( pVictim->GetMapId(), pVictim->GetPositionX(), pVictim->GetPositionY(), pVictim->GetPositionZ() );
				    if(!AreaID)
					    AreaID = pAttacker->GetZoneId(); // Failsafe for a shitty TerrainMgr

				    if(AreaID)
				    {
					    WorldPacket data(SMSG_ZONE_UNDER_ATTACK, 4);
					    data << AreaID;
					    sWorld.SendFactionMessage(&data, teamId);
				    }
                }
			}
		}
		
		if(pVictim->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT) > 0)
		{
			Spell *spl = pVictim->GetCurrentSpell();
			if(spl != NULL)
			{
				for(int i = 0; i < 3; i++)
				{
					if(spl->GetProto()->eff[i].Effect == SPELL_EFFECT_PERSISTENT_AREA_AURA)
					{
						DynamicObject *dObj = GetMapMgr()->GetDynamicObject( pVictim->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT) );
						if(!dObj)
						{ 
							return;
						}
						WorldPacket data(SMSG_GAMEOBJECT_DESPAWN_ANIM, 8);
						data << dObj->GetGUID();
						dObj->SendMessageToSet(&data, false);
						dObj->Remove();
					}
				}
				if(spl->GetProto()->ChannelInterruptFlags == 48140) 
					spl->safe_cancel();
			}
		}
		
		//check if we need to set a self res spell. Before we drop our ouras !
		if(pVictim->IsPlayer())
		{
			uint32 self_res_spell = 0;
			if (SafePlayerCast( pVictim )->m_bg == NULL || (SafePlayerCast( pVictim )->m_bg != NULL && SafePlayerCast( pVictim )->m_bg->GetType() != BATTLEGROUND_ARENA_5V5 && SafePlayerCast( pVictim )->m_bg->GetType() != BATTLEGROUND_ARENA_3V3 && SafePlayerCast( pVictim )->m_bg->GetType() != BATTLEGROUND_ARENA_2V2))
			{
				Aura *a = pVictim->HasAuraWithNameHash( SPELL_HASH_SOULSTONE_RESURRECTION, 0, AURA_SEARCH_POSITIVE );
				if( a )
					self_res_spell = a->GetSpellProto()->eff[0].EffectTriggerSpell;
				else
				{
					a = pVictim->HasAuraWithNameHash( SPELL_HASH_REINCARNATION, 0, AURA_SEARCH_PASSIVE );
					if( a && SafePlayerCast( pVictim )->Cooldown_CanCast( a->GetSpellProto() ) )
					{
						uint32 ankh_count = SafePlayerCast( pVictim )->GetItemInterface()->GetItemCount( 17030 );
						if( ankh_count )
							self_res_spell = 21169;
						else
						{
							SpellEntry *sp = dbcSpell.LookupEntryForced( 21169 );
							if( sp->SpellGroupType[0] & SafePlayerCast( pVictim )->GetUInt32Value(PLAYER_NO_REAGENT_COST_1 + 0) 
								|| sp->SpellGroupType[1] & SafePlayerCast( pVictim )->GetUInt32Value(PLAYER_NO_REAGENT_COST_1+1) 
								|| sp->SpellGroupType[2] & SafePlayerCast( pVictim )->GetUInt32Value(PLAYER_NO_REAGENT_COST_1+2) )
								self_res_spell = 21169;
						}
					}
				}
			}
			pVictim->SetUInt32Value( PLAYER_SELF_RES_SPELL, self_res_spell );
			pVictim->SetUInt32Value( UNIT_FIELD_MOUNTDISPLAYID , 0 );
			//pVictim->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_MOUNTED_TAXI);
		}

		/* Remove all Auras */
		pVictim->DropAurasOnDeath();

		/* Stop players from casting */
		InRangePlayerSetRecProt::iterator itr;
		InrangeLoopExitAutoCallback AutoLock;
		for( itr = pVictim->GetInRangePlayerSetBegin( AutoLock ) ; itr != pVictim->GetInRangePlayerSetEnd() ; itr ++ )
		{
			//if player has selection on us
			if( (*itr)->GetSelection()==pVictim->GetGUID())							
			{
				if( (*itr)->isCasting() )
					(*itr)->InterruptSpell( );
			}
		}
		/* Stop victim from attacking */
		if( this->IsUnit() )
			pVictim->smsg_AttackStop( SafeUnitCast( this ) );

		if( pVictim->IsPlayer() )
		{
			SafePlayerCast( pVictim )->EventAttackStop();
			CALL_INSTANCE_SCRIPT_EVENT( m_mapMgr, OnPlayerDeath )( SafePlayerCast(pVictim) , SafeUnitCast(this) );
		}

		/* Set victim health to 0 */
		pVictim->SetUInt32Value(UNIT_FIELD_HEALTH, 0);

		// Wipe our attacker set on death
		pVictim->CombatStatus.ClearAttackers();

		//		 sent to set. don't send it to the party, becuase if they're out of
		//		 range they won't know this guid exists -> possible 132.

		/*if (this->IsPlayer())
			if( SafePlayerCast( this )->InGroup() )
				SafePlayerCast( this )->GetGroup()->SendPartyKillLog( this, pVictim );*/

		/* Stop Unit from attacking */
		if( this->IsPlayer() && (SafePlayerCast(this)->GetSelection() == pVictim->GetGUID()) )
			SafePlayerCast( this )->EventAttackStop();
	   
		if( this->IsUnit() )
		{
			CALL_SCRIPT_EVENT( this, OnTargetDied )( pVictim );

			//stop attack if he was our target
			if( ( IsPlayer() && SafePlayerCast( this )->GetSelection() == pVictim->GetGUID() )
				|| ( SafeUnitCast( this )->GetAIInterface() && SafeUnitCast( this )->GetAIInterface()->GetNextTarget() == pVictim ) )
				SafeUnitCast( this )->smsg_AttackStop( pVictim );
		
			/* Tell Unit that it's target has Died */
			SafeUnitCast( this )->addStateFlag( UF_TARGET_DIED );

			// We will no longer be attacking this target, as it's dead.
			//SafeUnitCast(this)->setAttackTarget(NULL);
		}
		//so now we are completely dead
		//lets see if we have spirit of redemption
		if( pVictim->IsPlayer() )
		{
			if( SafePlayerCast( pVictim)->HasSpell( 20711 ) ) //check for spirit of Redemption
			{
				SafePlayerCast( pVictim)->CastSpellDelayed( pVictim->GetGUID(), 27827, 1, true );
			}
		}
		uint64 victimGuid = pVictim->GetGUID();

		if( IsPlayer() )
		{
			WorldPacket data( SMSG_PARTYKILLLOG, 20 );
			data << GetGUID() << pVictim->GetGUID();
			if( SafePlayerCast( this )->GetGroup() )
				SafePlayerCast( this )->GetGroup()->SendPacketToAll( &data );
			else if( SafePlayerCast( this )->GetSession() )
				SafePlayerCast( this )->GetSession()->SendPacket( &data );
		}			

		if( pVictim->IsCreature() )
		{
			pVictim->GetAIInterface()->OnDeath(this);

			/* Tell Unit that it's target has Died */
			SafeUnitCast( pVictim )->SetFlag( UNIT_FIELD_FLAGS, UNIT_FLAG_DEAD );
			SafeUnitCast( pVictim )->RemoveFlag( UNIT_FIELD_FLAGS_2, UNIT_FLAG_ALIVE );

			// it Seems that pets some how dont get a name and cause a crash here
			//bool isCritter = (pVictim->GetCreatureInfo() != NULL)? pVictim->GetCreatureInfo()->Type : 0;

			//-----------------------------------LOOOT--------------------------------------------
			Creature * victim = SafeCreatureCast(pVictim);
			// fill loot vector.
			bool has_loot = victim->generateLoot();
			if( has_loot == true )
			{

				Player *owner = NULL;
				if(victim->TaggerGuid)
					owner = GetMapMgr()->GetPlayer( victim->TaggerGuid );

				if(owner == NULL)  // no owner
				{
					// donno why this would happen, but anyway.. anyone can loot ;p
					// no owner no loot
					//victim->SetFlag(UNIT_DYNAMIC_FLAGS, U_DYN_FLAG_LOOTABLE);
					//Zack : reassign to killer if owner left
					if( plr )
					{
						victim->TaggerGuid = plr->GetGUID();	//probably player DCed
						victim->TaggerGroupId = plr->GetGroup();	//probably player DCed
						owner = plr;
					}
				}
				if( owner != NULL )
				{
					// Build the actual update.
					ByteBuffer buf( 500 );

					uint32 Flags = victim->m_uint32Values[ UNIT_DYNAMIC_FLAGS ];
					Flags |= U_DYN_FLAG_LOOTABLE;
					Flags |= U_DYN_FLAG_TAPPED_BY_PLAYER;

					victim->BuildFieldUpdatePacket( &buf, UNIT_DYNAMIC_FLAGS, Flags );

					// Check for owner's group.
					Group * pGroup = owner->GetGroup();
					if( pGroup != NULL )
					{
						// Owner was in a party.
						// Check loot method.
						victim->m_lootMethod = pGroup->GetMethod();
						switch( victim->m_lootMethod )
						{
						case PARTY_LOOT_RR:
/*						//this commented code is not used because it was never tested and finished !
						{
								//get new tagger for creature
								Player *tp = pGroup->GetnextRRlooter();
								if(tp)
								{
									//we force on creature a new tagger
									victim->TaggerGuid = tp->GetGUID();
									victim->Tagged = true;
									if(tp->IsVisible(victim))  // Save updates for non-existant creatures
										tp->PushUpdateData(&buf, 1);
								}
							}break;*/
						case PARTY_LOOT_FFA:
						case PARTY_LOOT_GROUP:
						case PARTY_LOOT_NBG:
							{
								// Loop party players and push update data.
								GroupMembersSet::iterator itr;
								SubGroup * sGrp;
								pGroup->Lock();
								for( uint32 Index = 0; Index < pGroup->GetSubGroupCount(); ++Index )
								{
									sGrp = pGroup->GetSubGroup( Index );
									itr = sGrp->GetGroupMembersBegin();
									for( ; itr != sGrp->GetGroupMembersEnd(); ++itr )
									{
										if( (*itr)->m_loggedInPlayer && (*itr)->m_loggedInPlayer->IsVisible( victim ) )	   // Save updates for non-existant creatures
											(*itr)->m_loggedInPlayer->PushUpdateData( &buf, 1 );
									}
								}
								pGroup->Unlock();
							}break;
						case PARTY_LOOT_MASTER:
							{
								GroupMembersSet::iterator itr;
								SubGroup * sGrp;
								pGroup->Lock();
								for( uint32 Index = 0; Index < pGroup->GetSubGroupCount(); ++Index )
								{
									sGrp = pGroup->GetSubGroup( Index );
									itr = sGrp->GetGroupMembersBegin();
									for( ; itr != sGrp->GetGroupMembersEnd(); ++itr )
									{
										if( (*itr)->m_loggedInPlayer && (*itr)->m_loggedInPlayer->IsVisible( victim ) )	   // Save updates for non-existant creatures
											(*itr)->m_loggedInPlayer->PushUpdateData( &buf, 1 );
									}
								}
								pGroup->Unlock();

								Player * pLooter = pGroup->GetLooter() ? pGroup->GetLooter()->m_loggedInPlayer : NULL;
								if( pLooter == NULL )
									pLooter = pGroup->GetLeader()->m_loggedInPlayer;

								if( pLooter->IsVisible( victim ) )  // Save updates for non-existant creatures
									pLooter->PushUpdateData( &buf, 1 );
							}break;
						}
					}
					else
					{
						// Owner killed the mob solo.
						if( owner->IsVisible( victim ) )
						{
							owner->PushUpdateData( &buf, 1 );
						}
					}
				}
			}
			//---------------------------------loot-----------------------------------------  

			// ----------------------------- XP --------------
			if ( pVictim->GetUInt64Value( UNIT_FIELD_CREATEDBY ) == 0 && 
				pVictim->GetUInt64Value( OBJECT_FIELD_CREATED_BY ) == 0 &&
				pVictim->GetMapMgr() &&
				!pVictim->IsPet() && SafeCreatureCast(pVictim)->TaggerGuid )
			{
				Unit *uTagger = pVictim->GetMapMgr()->GetUnit(SafeCreatureCast(pVictim)->TaggerGuid);
				if (uTagger != NULL)
				{
					if (uTagger->IsPlayer())
					{
						Player *pTagger = SafePlayerCast(uTagger);
						if (pTagger)
						{
							if (pTagger->InGroup())
							{
#ifndef BATTLEGRUND_REALM_BUILD
								pTagger->GiveGroupXP( pVictim, pTagger);
#endif
							}
							else if( IsUnit() ) 
							{
								uint32 xp = CalculateXpToGive( pVictim, uTagger );
								if( xp > 0 )
								{
#ifndef BATTLEGRUND_REALM_BUILD
									pTagger->GiveXP( xp, victimGuid, true );
#endif

									// This is crashing sometimes, commented by now
									if( sEventMgr.ModifyEventTimeLeft(this,EVENT_LASTKILLWITHHONOR_FLAG_EXPIRE,20000) == false )
										sEventMgr.AddEvent(SafeUnitCast(this),&Unit::EventAurastateExpire,(uint32)AURASTATE_LASTKILLWITHHONOR,EVENT_LASTKILLWITHHONOR_FLAG_EXPIRE,20000,1,0);
									if( SafeUnitCast(this)->HasAuraStateFlag(AURASTATE_LASTKILLWITHHONOR) == false )
										SafeUnitCast(this)->SetAuraStateFlag(AURASTATE_LASTKILLWITHHONOR); //got used up by a spell before expire
#ifndef BATTLEGRUND_REALM_BUILD
									if( pTagger->GetSummon() && pTagger->GetSummon()->CanGainXP() )
									{
										xp = CalculateXpToGive( pVictim, pTagger->GetSummon() );
										if( xp > 0 )
											pTagger->GetSummon()->GiveXP( xp );
									}
#endif
								}
							}
							if( !pVictim->IsPlayer() )
								sQuestMgr.OnPlayerKill( pTagger, SafeCreatureCast( pVictim ), true );
						}
					}
					else if (uTagger->IsPet())
					{
						Pet* petTagger = SafePetCast(uTagger);
						if (petTagger != NULL)
						{
							Player* petOwner = petTagger->GetPetOwner();
							if( petOwner != NULL)
							{
								if( petOwner->InGroup() )
								{
									//Calc Group XP
#ifndef BATTLEGRUND_REALM_BUILD
									petOwner->GiveGroupXP( pVictim, petOwner );
#endif
								}
								else if( IsUnit() )
								{
									uint32 xp = CalculateXpToGive( pVictim, petOwner );
									if( xp > 0 )
									{
#ifndef BATTLEGRUND_REALM_BUILD
										petOwner->GiveXP( xp, victimGuid, true );
#endif
										// This is crashing sometimes, commented by now
										if( sEventMgr.ModifyEventTimeLeft(this,EVENT_LASTKILLWITHHONOR_FLAG_EXPIRE,20000) == false )
											sEventMgr.AddEvent(SafeUnitCast(this),&Unit::EventAurastateExpire,(uint32)AURASTATE_LASTKILLWITHHONOR,EVENT_LASTKILLWITHHONOR_FLAG_EXPIRE,20000,1,0);
										if( SafeUnitCast(this)->HasAuraStateFlag(AURASTATE_LASTKILLWITHHONOR) == false )
											SafeUnitCast(this)->SetAuraStateFlag(AURASTATE_LASTKILLWITHHONOR); //got used up by a spell before expire
#ifndef BATTLEGRUND_REALM_BUILD
										if( petTagger->CanGainXP() )
										{
											xp = CalculateXpToGive( pVictim, petTagger );
											if( xp > 0 )
												petTagger->GiveXP( xp );
										}
#endif
									}
								}
								if(pVictim->GetTypeId() != TYPEID_PLAYER)
									sQuestMgr.OnPlayerKill( petOwner, SafeCreatureCast( pVictim ), true );
							}
						}
					}
				}
				// ----------------------------- XP --------------
			/* ----------------------------- PET XP HANDLING END-------------- */

			/* ----------------------------- PET DEATH HANDLING -------------- */
				if( pVictim->IsPet() )
				{
					Pet* pPet = SafePetCast( pVictim );
					Player* owner = pPet->GetPetOwner();

					// dying pet looses 1 happiness level (not in BG)
/*					if( !pPet->IsSummon() && !pPet->IsInBg() )
					{
						uint32 hap = pPet->GetPower( POWER_TYPE_HAPPINESS );
						hap = hap - PET_HAPPINESS_UPDATE_VALUE > 0 ? hap - PET_HAPPINESS_UPDATE_VALUE : 0;
						pPet->SetPower( POWER_TYPE_HAPPINESS, hap );
					}/**/
					
					pPet->DelayedRemove( false, true );
					
					//remove owner warlock soul link from caster
					if( owner != NULL )
						owner->EventDismissPet();
				}
				/* ----------------------------- PET DEATH HANDLING END -------------- */
				else if( pVictim->GetUInt64Value( UNIT_FIELD_CHARMEDBY ) )
				{
					//remove owner warlock soul link from caster
					Unit *owner=pVictim->GetMapMgr()->GetUnit( pVictim->GetUInt64Value( UNIT_FIELD_CHARMEDBY ) );
					if( owner != NULL && owner->IsPlayer())
						SafePlayerCast( owner )->EventDismissPet();
				}
			}
		}
		else if( pVictim->IsPlayer() )
		{
#ifdef BATTLEGRUND_REALM_BUILD
			Player *owner = NULL;
			if( IsPlayer() )
				owner = SafePlayerCast( this );
			else if( IsUnit() )
			{
				Unit *tow = SafeUnitCast( this )->GetTopOwner();
				if( tow->IsPlayer() )
					owner = SafePlayerCast( tow );
			}
			if( owner && owner->m_bg )
			{
				Player *ppVictim = SafePlayerCast( pVictim );
				//anti feeder protection
				{
					//killer get a bonus
					CalcAndGiveGoldToBGPlayer( owner, 1 );
					//everyone that contributed in the kill also gets partial kill bonus
					std::map<uint64,uint32>::iterator itr;
					for( itr = ppVictim->BGRealmPVPFightSupporters.begin(); itr != ppVictim->BGRealmPVPFightSupporters.end(); itr++ )
						if( itr->second + 60 * 1000 > getMSTime() )
						{
							Player *helper = owner->GetMapMgr()->GetPlayer( itr->first );
							if( helper )
								CalcAndGiveGoldToBGPlayer( helper, 2 );
						}
				}
				int64 *last_anti_feed_stamp = pVictim->GetCreateIn64Extension( EXTENSION_ID_BG_REALM_ANTI_FEED_STAMP );
				if( *last_anti_feed_stamp + 1 * 60 * 1000 < getMSTime() )
				{
					*last_anti_feed_stamp = getMSTime() + (RandomUInt() % 30000);
					//victim also get a small reward since he is a victim here
					CalcAndGiveGoldToBGPlayer( SafePlayerCast( pVictim ), 0 );
					if( owner->InGroup() )
						owner->GiveGroupXP( pVictim, owner );
					else
					{
						uint32 xp = CalculateXpToGive( pVictim, owner );
						owner->GiveXP( xp, pVictim->GetGUID(), true );
					}

					if( ppVictim->InGroup() )
						ppVictim->GiveGroupXP( owner, ppVictim );
					else
					{
						uint32 xp = CalculateXpToGive( owner, ppVictim  );
						ppVictim->GiveXP( xp, owner->GetGUID(), true );
					}
				}
			}
#endif
			//for a specific duration do not allow players to resurrect. Instead ban them for a day or so
			if( sWorld.getIntRate( INTRATE_RESPAWN_COOLDOWN ) && SafePlayerCast( pVictim )->m_bg == NULL && SafePlayerCast( pVictim )->GetSession() )
			{
				string sReason;
				sReason = "Resurrecting";
				uint32 uBanTime = (uint32)UNIXTIME + sWorld.getIntRate( INTRATE_RESPAWN_COOLDOWN ); 
				SafePlayerCast( pVictim )->SetBanned( uBanTime, sReason );
				sEventMgr.AddEvent( SafePlayerCast( pVictim ), &Player::_Kick, EVENT_PLAYER_KICK, 7000, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
			}
			SafePlayerCast( pVictim )->m_UnderwaterTime = 0;
			SafePlayerCast( pVictim )->m_UnderwaterState = 0;
			SafePlayerCast( pVictim )->m_BreathDamageTimer = 0;
			SafePlayerCast( pVictim )->m_SwimmingTime = 0;

			/* -------------------- KILL PET WHEN PLAYER DIES ---------------*/
			if( SafePlayerCast( pVictim )->GetSummon() != NULL )
			{
				SafePlayerCast( pVictim )->GetSummon()->Dismiss( true, false, true );
			}
			//also get rid of other spawns
			pVictim->DismissAllGuardians();
			/* -------------------- KILL PET WHEN PLAYER DIES END---------------*/
		}
		else sLog.outError("DealDamage for Unknown Object.");
	}
	else /* ---------- NOT DEAD YET --------- */
	{
		if(pVictim != this /* && updateskill */)
		{
			// Send AI Reaction UNIT vs UNIT
			/* Weird: why should WE react on OUR damage?
			If meaning of this is to get reaction of victim, then its already handled few rows below... 
			if( GetTypeId() ==TYPEID_UNIT )
			{
				SafeUnitCast( this )->GetAIInterface()->AttackReaction( pVictim, damage, spellId );
			}*/
			
			// Send AI Victim Reaction
			if( this->IsPlayer() || this->IsCreature() )
			{
				if( !pVictim->IsPlayer() )
				{
					SafeCreatureCast( pVictim )->GetAIInterface()->AttackReaction( SafeUnitCast( this ), damage, spellId );
				}
				else
				{
					// Defensive pet
					Pet* pPet = SafePlayerCast( pVictim )->GetSummon();
					if( pPet != NULL && pPet->GetPetState() != PET_STATE_PASSIVE && pPet->GetAIInterface())
					{
						pPet->GetAIInterface()->AttackReaction( SafeUnitCast( this ), 1, 0 );
						pPet->HandleAutoCastEvent( AUTOCAST_EVENT_OWNER_ATTACKED );
					}
				}
			}
		}
		
		// TODO: Mark victim as a HK
		/*if( SafePlayerCast( pVictim )->GetCurrentBattleground() != NULL && SafePlayerCast( this )->GetCurrentBattleground() != NULL)
		{
			
		}*/	

		pVictim->SetUInt32Value( UNIT_FIELD_HEALTH, health - damage );

		if ( IsCreature() 
//			&& !IsPet()	//removed by zack : why don't pets get agro from target ?
			)
			SafeUnitCast(this)->GetAIInterface()->HandleEvent(EVENT_DAMAGEDEALT, pVictim, damage);
	}
}

float	Object::GetSpellCritChance( Unit *pVictim, SpellEntry *spellInfo, uint32 DOTticks, int32 sp_eff_index )
{

	if( ( sp_eff_index < MAX_SPELL_EFFECT_COUNT && spellInfo->EffectCanCrit[ sp_eff_index ] == false ) || ( sp_eff_index >= MAX_SPELL_EFFECT_COUNT && spellInfo->EffectCanCrit[ 0 ] == false ) )
		return 0.0f;

	float CritChance = 0.0f;

	if( IsUnit() == false )
		return 0.0f;

	uint32 school = spellInfo->School;
	Unit *caster = SafeUnitCast( this );

		float spell_crit_mod = 0.0f;
		if( Need_SM_Apply(spellInfo) )
		{
			if( caster->SM_Mods )
				SM_FFValue(caster->SM_Mods->SM_FCriticalChance, &spell_crit_mod, spellInfo->GetSpellGroupType());
			if( pVictim->SM_Mods )
				SM_FFValue(pVictim->SM_Mods->SM_FCriticalChanceTarget, &spell_crit_mod, spellInfo->GetSpellGroupType());
#ifdef COLLECTION_OF_UNTESTED_STUFF_AND_TESTERS
			float spell_flat_modifers=0;
			SM_FFValue(caster->SM_FCriticalChance,&spell_flat_modifers,spellInfo->GetSpellGroupType());
			if(spell_flat_modifers!=0)
				printf("!!!!spell critchance mod flat %f ,spell group %u\n",spell_flat_modifers,spellInfo->GetSpellGroupType());
#endif
		}

//		if( sp_eff_index < MAX_SPELL_EFFECT_COUNT && spellInfo->EffectCanCrit[ sp_eff_index ] == true )
		{
//------------------------------critical strike chance--------------------------------------	
			// lol ranged spells were using spell crit chance
			if( spellInfo->GetSpellDMGType() == SPELL_DMG_TYPE_RANGED )
			{

				if( IsPlayer() )
				{
					CritChance = GetFloatValue( PLAYER_RANGED_CRIT_PERCENTAGE );
					if( pVictim->IsPlayer() )
						CritChance += SafePlayerCast(pVictim)->res_R_crit_get();
				}
				else
				{
					CritChance = 5.0f; // static value for mobs.. not blizzlike, but an unfinished formula is not fatal :)
				}
				CritChance += (float)(pVictim->AttackerCritChanceMod[spellInfo->School]);
//				if( pVictim->IsPlayer() )
//					CritChance -= SafePlayerCast(pVictim)->CalcRating( PLAYER_RATING_MODIFIER_RANGED_CRIT_RESILIENCE ) * 2;
			}
			else if( spellInfo->GetSpellDMGType() == SPELL_DMG_TYPE_MELEE || caster->getClass() == ROGUE )
			{
				// Same shit with the melee spells, such as Judgement/Seal of Command
				if( IsPlayer() )
					CritChance = GetFloatValue(PLAYER_CRIT_PERCENTAGE);
				if( pVictim->IsPlayer() )
					CritChance += SafePlayerCast(pVictim)->res_R_crit_get(); //this could be ability but in that case we overwrite the value
				// Resilience
//				CritChance -= pVictim->IsPlayer() ? SafePlayerCast(pVictim)->CalcRating( PLAYER_RATING_MODIFIER_MELEE_CRIT_RESILIENCE ) * 2 : 0.0f;
				// Victim's (!) crit chance mod for physical attacks?
				CritChance += (float)(pVictim->AttackerCritChanceMod[0]);
			}
			else
			{
				CritChance = caster->spellcritperc + caster->SpellCritChanceSchool[school] + pVictim->AttackerCritChanceMod[school];

//				if( pVictim->IsPlayer() )
//					CritChance -= SafePlayerCast(pVictim)->CalcRating( PLAYER_RATING_MODIFIER_SPELL_CRIT_RESILIENCE ) * 2.2f;
			}

			CritChance += spell_crit_mod;

			//cause blizz is full of special cases
			if( spellInfo->CritHandler != NULL )
			{
				ProcHandlerContextShare context;
				memset( &context, 0, sizeof( ProcHandlerContextShare ) );
				context.in_Caller = caster;
				context.in_Victim = pVictim;
				context.in_CastingSpell = spellInfo;
				context.out_dmg_overwrite[0] = context.in_dmg = float2int32( CritChance );
				spellInfo->CritHandler( &context );
				CritChance = (float)context.out_dmg_overwrite[0];
			}

			if( caster->IsPlayer() && SafePlayerCast( caster )->m_FrozenCritChanceMultiplier > 0 && pVictim->HasAuraStateFlag( AURASTATE_FROZEN ) )
				CritChance = CritChance * SafePlayerCast( caster )->m_FrozenCritChanceMultiplier;

			if( CritChance < 0 ) 
				CritChance = 0;
			if( CritChance > 95 ) 
				CritChance = 95;
		}

	return CritChance;
}

void UpdateHealAbsorbAuras( Unit *pVictim, int32 absorb );

void Object::SpellHeal(Unit *pVictim, SpellEntry *spellInfo, uint32 damage, bool allowProc, bool static_damage, bool no_remove_auras, uint32 DOTticks, int32 pct_mod, int32 sp_eff_index, int8 CanCrit)
{
	if(!pVictim || !pVictim->isAlive())
	{ 
		return;
	}

	if(!spellInfo)
	{ 
        return;
	}

	if (this->IsPlayer() && !SafePlayerCast( this )->canCast(spellInfo))
	{ 
		return;
	}

	//cannot get healed
	if( pVictim->HealTakenPctMod <= -100 )
		return;

	int32 amount = damage; //store base_amount for later use

	Unit *u_caster;
	Player *p_caster;
	if( IsUnit() )
		u_caster = SafeUnitCast( this );
	else
		u_caster = NULL;

	if( IsPlayer() )
	{
		p_caster = SafePlayerCast( this );
		p_caster->last_heal_spell = spellInfo;

		//self healing shouldn't flag himself
		if( pVictim->IsPlayer() && this != pVictim )
		{
			// Healing a flagged target will flag you.
			if( pVictim->IsPvPFlagged() )
				u_caster->SetPvPFlag();
			//cannot heal ones that are Dueling(and not self)
			if( pVictim->IsPlayer() && SafePlayerCast( pVictim )->DuelingWith )
				return;
		}
	}
	else
	{
		p_caster = NULL;
	}

	uint32 a_proc = PROC_ON_HEAL_EVENT | PROC_ON_NO_CRIT;
	if( DOTticks > 1 )
		a_proc |= PROC_ON_DOT;
	else
		a_proc |= PROC_ON_NO_DOT;

	bool critical = false;
	if( u_caster != NULL && spellInfo->Spell_value_is_scripted == false && static_damage == 0 )
	{
		int32 critchance = 0;
		int32 bonus = 0;
		float SummaryModCoeff;

		//item spells do not get spell power bonus
		bonus += u_caster->GetSpellHealBonus( pVictim, spellInfo, damage, sp_eff_index, &SummaryModCoeff );
		SummaryModCoeff = SummaryModCoeff * ( pct_mod / 100.0f );

		if( CanCrit == 16 )
		{
//			critchance = float2int32(u_caster->spellcritperc + u_caster->SpellCritChanceSchool[GetProto()->School] );
//			critchance += forced_heal_crit_mod[ eff_index ]);
			critchance = GetSpellCritChance( pVictim, spellInfo, DOTticks, sp_eff_index );
			//one of my worst hackfixes since a while 
			{
				void *CritMod = u_caster->GetExtension( EXTENSION_ID_ONETIME_TARGETSPECIFIC_CRIT_MOD_TARGET ); //stupid spell "Last Word"
				if( CritMod && *(uint64*)(CritMod) == pVictim->GetGUID() && *(u_caster->GetCreateIn64Extension(EXTENSION_ID_ONETIME_TARGETSPECIFIC_CRIT_MOD_SPELL)) == spellInfo->Id )
				{
					critchance += *(u_caster->GetCreateIn64Extension(EXTENSION_ID_ONETIME_TARGETSPECIFIC_CRIT_MOD_VAL));
					*(uint64*)(CritMod) = 0;
				}
			}

			if( critchance > 0.0f )
			{
				float EffectiveCritchance = 100.0f * ( (float)u_caster->EventsSpellsCrit / (float)u_caster->EventsSpellsOverall );
				critchance = critchance * ( critchance / EffectiveCritchance ) ; // cause 50 already gives the feeling it is proccing all the time
				u_caster->EventsSpellsOverall++;
			}
			critical = RandChance( critchance );
			if( critical )
				u_caster->EventsSpellsCrit++;

			if( critical == true )
			{
				int32 critical_bonus = 200;
				if( Need_SM_Apply( spellInfo ) && u_caster->SM_Mods )
					SM_FIValue( u_caster->SM_Mods->SM_PCriticalDamage, &critical_bonus, spellInfo->GetSpellGroupType() );

				critical_bonus += u_caster->HealCritDonePctMod;
				SummaryModCoeff = SummaryModCoeff * ( critical_bonus / 100.0f );

				a_proc |= PROC_ON_CRIT;
				a_proc &= ~PROC_ON_NO_CRIT;
			}
		}

		amount = float2int32( ( amount + bonus ) * SummaryModCoeff ); 

		if( amount < 0 )
			amount = 0; //!! the mods i tell you !
	}
	else
	{
		amount = amount * pct_mod / 100;
	}

	int32 absorb = 0;
	if( pVictim->HealAbsorb <= amount )
	{
		absorb = pVictim->HealAbsorb;
		amount -= absorb;
		pVictim->HealAbsorb = 0;
	}
	else // if( unitTarget->HealAbsorb > amount )
	{
		absorb = amount;
		pVictim->HealAbsorb -= amount;
		amount = 0;
	}
	if( absorb )
		UpdateHealAbsorbAuras( pVictim, absorb );

	if(amount < 0)
		amount = 0;

	//maybe consider pets and totems here ?
	if( p_caster != NULL )
	{
		p_caster->m_bgScore.HealingDone += amount;
		if( p_caster->m_bg != NULL )
		{
			//you might wonder how this gets invalid. Well i agree with you. This should not get invalid ever. Sadly some players get stuck in teleporting state until they fuck up pointer ..
//			if( BattlegroundManager.IsValidBGPointer( p_caster->m_bg ) )
//				p_caster->m_bg->UpdatePvPData();
//			else
//				p_caster->m_bg = NULL;
			if( pVictim->IsPlayer() ) 
				SafePlayerCast( pVictim )->Event_AchiementCriteria_Received( ACHIEVEMENT_CRITERIA_TYPE_TOTAL_HEALING_RECEIVED,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,amount,ACHIEVEMENT_EVENT_ACTION_ADD);
			p_caster->Event_AchiementCriteria_Received( ACHIEVEMENT_CRITERIA_TYPE_HEALING_DONE,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,amount,ACHIEVEMENT_EVENT_ACTION_ADD);
		}
		p_caster->Event_AchiementCriteria_Received( ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_HEAL_CASTED,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,amount,ACHIEVEMENT_EVENT_ACTION_SET_MAX);

	}
	if(	( allowProc == true || (spellInfo->c_is_flags & SPELL_FLAG_IS_PROC_TRIGGER_PROC) ) && (spellInfo->c_is_flags3 & SPELL_FLAG3_IS_DISABLE_OTHER_SPELL_CPROC) == 0 )
	{
		if( pVictim != this )
		{
			//also contains overheal value and gives a chance to detect missing health
			if( u_caster )
				amount += u_caster->HandleProc( ( a_proc | PROC_ON_NOT_VICTIM ), pVictim, spellInfo, &amount, &absorb );
			amount += pVictim->HandleProc( ( a_proc | PROC_ON_VICTIM ), u_caster, spellInfo, &amount, &absorb );
		}
		else
			amount += pVictim->HandleProc( ( a_proc | PROC_ON_VICTIM | PROC_ON_NOT_VICTIM ), u_caster, spellInfo, &amount, &absorb );
	}

	//send heal log
//	if( unitTarget->IsPlayer() )
//		SendHealSpellOnPlayer( m_caster, SafePlayerCast( unitTarget ), amount, critical );
	if( DOTticks <= 1 )
		this->SendCombatLogMultiple( pVictim, amount, 0, absorb, 0, spellInfo->spell_id_client, spellInfo->SchoolMask, COMBAT_LOG_HEAL, critical );
	else
		this->SendCombatLogMultiple( pVictim, amount, 0, absorb, 0, spellInfo->spell_id_client, spellInfo->SchoolMask, COMBAT_LOG_PERIODIC_HEAL, critical, COMBAT_LOG_SUBFLAG_PERIODIC_HEAL );


	uint32 curHealth = pVictim->GetUInt32Value(UNIT_FIELD_HEALTH);
	uint32 maxHealth = pVictim->GetUInt32Value(UNIT_FIELD_MAXHEALTH);
	if((curHealth + amount) >= maxHealth)
	{
		amount = maxHealth - curHealth;
		pVictim->SetUInt32Value(UNIT_FIELD_HEALTH, maxHealth);
	}
	else
		pVictim->ModUnsigned32Value(UNIT_FIELD_HEALTH, amount);

	//only consider used up healing and not total overheal too
	if( p_caster != NULL )
		p_caster->m_heal_made_since_login += amount;
	if( pVictim->IsPlayer() )
		SafePlayerCast( pVictim )->m_heal_received_since_login += amount;

	pVictim->RemoveAurasByHeal();

	// add threat for spells that cause threath
	if( u_caster != NULL && spellInfo->ThreatForSpellCoef != 0 && allowProc == true )
	{
		std::vector<Unit*> target_threat;
		int count = 0;
		u_caster->AquireInrangeLock();
		InrangeLoopExitAutoCallback AutoLock;
		for(InRangeSetRecProt::iterator itr = u_caster->GetInRangeSetBegin( AutoLock ); itr != u_caster->GetInRangeSetEnd(); ++itr)
		{
			if((*itr)->GetTypeId() != TYPEID_UNIT || !SafeUnitCast(*itr)->CombatStatus.IsInCombat() || (SafeUnitCast(*itr)->GetAIInterface()->getThreatByPtr(u_caster) == 0 && SafeUnitCast(*itr)->GetAIInterface()->getThreatByPtr(pVictim) == 0))
				continue;

			target_threat.push_back(SafeUnitCast(*itr));
			count++;
		}
		u_caster->ReleaseInrangeLock();
		if (count == 0)
		{ 
			return;
		}

		amount = amount / count;

		for(std::vector<Unit*>::iterator itr = target_threat.begin(); itr != target_threat.end(); ++itr)
			SafeUnitCast(*itr)->GetAIInterface()->HealReaction(u_caster, pVictim, spellInfo, amount);

		// remember that we healed (for combat status)
//		if(unitTarget->IsInWorld() && u_caster->IsInWorld())
//			u_caster->CombatStatus.WeHealed(unitTarget);
	}
}

void Object::SpellNonMeleeDamageCalc(Unit *pVictim, SpellEntry *spellInfo, dealdamage *dmg, uint32 DOTticks, int32 sp_eff_index )
{
	uint32 school = spellInfo->School;
	uint32 spellID = spellInfo->Id;
	int32 spell_dmg_bonus;
	float SummaryModCoeff = 1.0f;
	
	//A school damage is not necessarily magic
	bool critical = false;
//==========================================================================================
//==============================+Spell Damage Bonus Calculations============================
//==========================================================================================
//------------------------------by stats----------------------------------------------------
//	dealdamage dmg;
//	memset( &dmg, 0, sizeof( dmg ) );
	dmg->full_damage = dmg->base_dmg;
	//hmm, scripted spells should be able to crit or not ? We do have Crit deny spell attribute
//	if( spellInfo->Spell_value_is_scripted == false )
	{
		if( this->IsUnit() )
		{
			Unit* u_caster = SafeUnitCast( this );
			if( !dmg->StaticDamage && spellInfo->Spell_value_is_scripted == false )
			{
				spell_dmg_bonus = u_caster->GetSpellDmgBonus( pVictim, spellInfo, dmg->full_damage, sp_eff_index, &SummaryModCoeff );
				dmg->full_damage += spell_dmg_bonus;
			}

			//rogue talent Sanguinary Vein
			if( u_caster->getClass() == ROGUE )
			{
				int64 *ext = (int64*)GetExtension( EXTENSION_ID_SANGUINARY_VEIN_VAL );
				if( ext )
				{
//					bool is_bleeding = (pVictim->HasAuraWithMechanic( 0, AURA_SEARCH_NEGATIVE, ( 1 << MECHANIC_BLEEDING ) )!=NULL);
//					if( is_bleeding )
					uint32 BleedCount = 0;
					for(uint32 x=MAX_POSITIVE_AURAS;x<MAX_NEGATIVE_AURAS1( pVictim );x++)
						if( pVictim->m_auras[x] && ( pVictim->m_auras[x]->GetSpellProto()->MechanicsTypeFlags & ( 1 << MECHANIC_BLEEDING ) ) )
							BleedCount++;
					if( BleedCount >= 1 )
						dmg->pct_mod_final_dmg = dmg->pct_mod_final_dmg * ( 100 + (int32)(*ext) ) / 100;
				}
			}
	//==========================================================================================
	//==============================Post +SpellDamage Bonus Modifications=======================
	//==========================================================================================
			float CritChance = GetSpellCritChance( pVictim, spellInfo, DOTticks, sp_eff_index );

//if( IsPlayer() ) SafePlayerCast( this )->BroadcastMessage( "Crit chance for spell %s is %f ", spellInfo->Name, CritChance );

			if( CritChance > 0.0f )
			{
				float EffectiveCritchance = 100.0f * ( (float)u_caster->EventsSpellsCrit / (float)u_caster->EventsSpellsOverall );
				CritChance = CritChance * ( CritChance / EffectiveCritchance ) ; // cause 50 already gives the feeling it is proccing all the time
				u_caster->EventsSpellsOverall++;
			}
			critical = RandChance( CritChance );
			if( critical )
				u_caster->EventsSpellsCrit++;
	//sLog.outString( "SpellNonMeleeDamageLog: Crit Chance %f%%, WasCrit = %s" , CritChance , critical ? "Yes" : "No" );
	//==========================================================================================
	//==============================Spell Critical Hit==========================================
	//==========================================================================================
			if (critical)
			{		
				if( spellInfo->School == SCHOOL_NORMAL || spellInfo->is_melee_spell || spellInfo->is_ranged_spell )		// physical || hackfix SoCommand/JoCommand
					dmg->pct_mod_crit += 100;
				else
				{
					if(( u_caster->getClass() == PRIEST && spellInfo->School == SCHOOL_HOLY ) || u_caster->getClass() == MAGE || u_caster->getClass() == WARLOCK )
						dmg->pct_mod_crit += 100;
					else if( u_caster->getClass() == DEATHKNIGHT )
						dmg->pct_mod_crit += 100; //spell Runic Focus adds another 100% instead of hardscripting
					else
						dmg->pct_mod_crit += 50;
				}
			
				if( Need_SM_Apply(spellInfo) && u_caster->SM_Mods )
					SM_FIValue( u_caster->SM_Mods->SM_PCriticalDamage, &dmg->pct_mod_crit, spellInfo->GetSpellGroupType() );

				if( IsPlayer() )
					dmg->pct_mod_crit += SafePlayerCast( this )->m_modSpellCritdmgPCT;

				SummaryModCoeff *= ( 1.0f + dmg->pct_mod_crit / 100.0f );
			}
		}
		SummaryModCoeff = SummaryModCoeff * dmg->pct_mod_final_dmg / 100.0f;
		SummaryModCoeff = SummaryModCoeff * pVictim->DamageTakenPctMod[ school ] / 100.0f;
		dmg->full_damage = float2int32( dmg->full_damage * SummaryModCoeff );	//this comes from spells with chained targetting that loose PCT as they jump
	}
	dmg->school_type = school;

	if( pVictim->SchoolImmunityAntiEnemy[ school ] || pVictim->SchoolImmunityAntiFriend[ school ] )
	{
		dmg->resisted_damage = dmg->full_damage;
		dmg->full_damage = 0;
	}
	else
	{
		float ResilianceReduceCoef = pVictim->ResilianceAbsorbPCT( this );
		float ResistReduceCoef;
		if( this->IsUnit() && ( spellInfo->c_is_flags2 & SPELL_FLAG2_IS_AVOIDING_ALL_RESIST ) == 0 )
			ResistReduceCoef = SafeUnitCast(this)->CalculateResistanceReductionPCT( pVictim, dmg, spellInfo, sp_eff_index );
		else
			ResistReduceCoef = 0.0f;
		float TotalResistCoef = 1.0f - MAX( 0.0f, MIN( 1.0f, ( 1.0f - ResilianceReduceCoef ) * ( 1.0f - ResistReduceCoef ) ));	
		dmg->resisted_damage += MAX( 0, ( dmg->full_damage - dmg->absorbed_damage ) ) * TotalResistCoef;

		// according to wowhead 2.1 client power word shield is AFTER resistance absorb
		if( ( spellInfo->c_is_flags2 & SPELL_FLAG2_IS_AVOIDING_ALL_RESIST ) == 0 )
		{
			dmg->absorbed_damage += pVictim->AbsorbDamage( school, dmg->full_damage - dmg->resisted_damage - dmg->absorbed_damage );
			dmg->absorbed_damage += pVictim->ManaShieldAbsorb( dmg->full_damage - dmg->resisted_damage - dmg->absorbed_damage );
		}
	}
	// Paladin: Blessing of Sacrifice, and Warlock: Soul Link
	if( pVictim->m_damageSplitTarget 
		&& this != pVictim //not sure this is ok. Warlock health funnel should not split caster self dmg with pet
		)
	{
		int32 dmg_reamining = pVictim->DoDamageSplitTarget( dmg->full_damage, school, false);
		dmg->absorbed_damage += ( dmg->full_damage - dmg_reamining );
	}
}

void Object::SpellNonMeleeDamageLog(Unit *pVictim, SpellEntry *spellInfo, dealdamage *dmg, uint32 DOTticks, int32 sp_eff_index )
{
	if(!pVictim || !pVictim->isAlive())
	{ 
		return;
	}

	if(!spellInfo)
	{ 
        return;
	}

	if (this->IsPlayer() && !SafePlayerCast( this )->canCast(spellInfo))
	{ 
		return;
	}

	//should never happen
	if( dmg == NULL )
	{
		return;
	}

	uint32 school = spellInfo->School;
	uint32 spellID = spellInfo->Id;
	uint32 aproc = PROC_ON_NO_CRIT;

	//need this to be able to script all spells. Some are melee, some are ranged, some are bugged.
	aproc |= PROC_ON_SPELL_HIT_EVENT;
	
	//A school damage is not necessarily magic
	if( DOTticks <= 1 )	//avid poison ticks, bleed ticks to trigger talents as melee swing
	{
		//Disabled Ranged or melle tipe of spell damge event cause random spells behave randomly. This might cause some overprocs or 0 procs in the future. Please script spells that require specific dmg type procs
		switch( spellInfo->GetSpellDMGType() )
		{
			case SPELL_DMG_TYPE_RANGED:	
					aproc |= PROC_ON_RANGED_ATTACK_EVENT;
			break;
			case SPELL_DMG_TYPE_MELEE:
					aproc |= PROC_ON_MELEE_ATTACK_EVENT;
			break;
//			case SPELL_DMG_TYPE_MAGIC: 
//			default:
//				aproc = PROC_ON_SPELL_HIT_EVENT;
//			break;
		}
	}
	if( DOTticks > 1 )
	{
		aproc |= PROC_ON_DOT;
		if( dmg->FirstTick == true )
			aproc |= PROC_ON_FIRST_TICK;
	}
	else
		aproc |= PROC_ON_NO_DOT;

	SpellNonMeleeDamageCalc( pVictim, spellInfo, dmg, DOTticks, sp_eff_index );

	if( this->IsUnit() )
	{
		Unit* u_caster = SafeUnitCast( this );
		u_caster->RemoveAurasByInterruptFlagButSkip( AURA_INTERRUPT_ON_START_ATTACK , spellID );
	}

	if( dmg->pct_mod_crit != 0 )
	{
		if( pVictim->GetTypeId() == TYPEID_UNIT && SafeCreatureCast(pVictim)->GetCreatureInfo() && SafeCreatureCast(pVictim)->GetCreatureInfo()->Rank != ELITE_WORLDBOSS)
			pVictim->Emote( EMOTE_ONESHOT_WOUNDCRITICAL );
		aproc |= PROC_ON_CRIT;
		aproc &= ~PROC_ON_NO_CRIT;
	}

	if( dmg->absorbed_damage )
		aproc |= PROC_ON_ABSORB_EVENT;
	
//==========================================================================================
//==============================Data Sending ProcHandling===================================
//==========================================================================================
	if( this->IsUnit() && dmg->DisableProc == false )
	{
//		int32 tdmg = dmg->full_damage - dmg->resisted_damage - dmg->absorbed_damage;
		int32 param_abs = (dmg->resisted_damage + dmg->absorbed_damage);
		int32 last_minute_resist;
		if( pVictim != SafeUnitCast( this ) ) //can be self dot
		{
			last_minute_resist = pVictim->HandleProc( ( aproc | PROC_ON_VICTIM ), SafeUnitCast( this ), spellInfo, &dmg->full_damage, &param_abs );
			SafeUnitCast( this )->HandleProc( ( aproc | PROC_ON_NOT_VICTIM ), pVictim, spellInfo, &dmg->full_damage, &param_abs );
		}
		else
			last_minute_resist = pVictim->HandleProc( ( aproc | PROC_ON_VICTIM | PROC_ON_NOT_VICTIM ), SafeUnitCast( this ), spellInfo, &dmg->full_damage, &param_abs );

		//due to some procs dmg may change
		dmg->absorbed_damage += ( last_minute_resist + ( param_abs - (dmg->resisted_damage + dmg->absorbed_damage) ) );
	}

	int32 ress = dmg->full_damage - dmg->resisted_damage - dmg->absorbed_damage;
	if( ress < 0 )
		ress = 0;

	//Rage for remaining dmg will be added in "dealdamage" We only add rage here for the resisted part
	if( dmg->full_damage && pVictim->IsPlayer() && pVictim->GetPowerType() == POWER_TYPE_RAGE && pVictim->CombatStatus.IsInCombat() && IsUnit() )
		pVictim->RageEventDamageTaken( MIN( dmg->full_damage, dmg->resisted_damage + dmg->absorbed_damage ), SafeUnitCast( this ) );

	uint32 ID_for_the_logs ;
	if( spellInfo->spell_id_client )
		ID_for_the_logs = spellInfo->spell_id_client;
	else
		ID_for_the_logs = spellInfo->Id;
	if( DOTticks > 1 )
//		SendSpellPeriodicAuraLog(this, pVictim, ID_for_the_logs, school, float2int32(res), abs_dmg, dmg->resisted_damage, FLAG_PERIODIC_DAMAGE, critical);
		SendCombatLogMultiple( pVictim, ress, dmg->resisted_damage, dmg->absorbed_damage, 0, ID_for_the_logs, spellInfo->SchoolMask, COMBAT_LOG_PERIODIC_DAMAGE, ( dmg->pct_mod_crit != 0 ), COMBAT_LOG_SUBFLAG_PERIODIC_DAMAGE );
	else
	{
//		SendSpellNonMeleeDamageLog(this, pVictim, ID_for_the_logs, float2int32(res), school, abs_dmg, dmg->resisted_damage, false, 0, critical, IsPlayer());
		SendCombatLogMultiple( pVictim, ress, dmg->resisted_damage, dmg->absorbed_damage, 0, ID_for_the_logs, spellInfo->SchoolMask, COMBAT_LOG_SPELL_DAMAGE, ( dmg->pct_mod_crit != 0 ) );
	}
	DealDamage( pVictim, ress, spellID );

	if( ress > 0 && DOTticks <= 1 && pVictim != this )
	{
		//dots do not keep people in combat. This is needed for rogues/druid who need to stealth after 5 seconds even with dots
		//no idea if this should be valid for other class. Warriors would loose rage
		if( this->IsUnit() )
		{
			SafeUnitCast( this )->CombatStatus.OnDamageDealt( pVictim );
		}
		//Only pushback the victim current spell if it's not fully absorbed
		if( pVictim->GetCurrentSpell() )
			pVictim->GetCurrentSpell()->AddTime( school );
	}
	//victim cannot get out of combat. 
	if( pVictim != this )
		pVictim->CombatStatusHandler_ResetPvPTimeout();
	//we handled this with the dmg part
	//Attacker can get out of combat if not using direct dmg( running away from fight )
//	if( IsUnit() )
//		SafeUnitCast(this)->CombatStatusHandler_ResetPvPTimeout();
}

/*
void Object::SpellNonMeleeDamageLog(Unit *pVictim, uint32 SpellId, uint32 damage, bool allowProc, bool static_damage, bool no_remove_auras,uint32 DOTticks)
{
	SpellEntry *spellInfo = dbcSpell.LookupEntryForced( SpellId );
	if( spellInfo )
		SpellNonMeleeDamageLog( pVictim, spellInfo, damage, allowProc, static_damage, no_remove_auras, DOTticks );
}*/

//*****************************************************************************************
//* SpellLog packets just to keep the code cleaner and better to read
//*****************************************************************************************

void Object::SendSpellLog(Object *Caster, Object *Target,uint32 Ability, uint8 SpellLogType)
{
	if ( ( ( !Caster || !Target	) && Ability ) )
		return;

/*
1C A7 00 00 
68 00 00 99 5D 00 30 F1 
01 
01 00 00 00 
69 00 00 99 5D 00 30 F1 
03 - dodge
70 1C DE 40 
00 00 00 00

3B D0 00 00 
01 6F 54 01 00 00 00 00 
00 - it's 0 and not 1 as we suspect it to be. Maybe it signals packet extension ?
01 00 00 00
49 0F 00 F1 76 00 30 F1 
02 - resist
*/
	sStackWorldPacket( data, SMSG_SPELLLOGMISS, 60 );
	data << Ability;										// spellid
	data << Caster->GetGUID();							  // caster / player
	if( SpellLogType == SPELL_LOG_RESIST )
		data << (uint8)0;									   // unknown but I think they are const
	else
		data << (uint8)1;									   // unknown but I think they are const
	data << (uint32)1;									  // unknown but I think they are const
	data << Target->GetGUID();							  // target
	data << SpellLogType;								   // spelllogtype
#ifdef MINIMIZE_NETWORK_TRAFIC
	SendMessageToDuel( &data );
#else
	Caster->SendMessageToSet(&data, true);
#endif
}
/*
void Object::SendSpellPeriodicAuraLog(Object* Caster, Unit * Target, uint32 SpellID, uint32 School, uint32 Amount, uint32 abs_dmg, uint32 resisted_damage, uint32 Flags, uint8 crit)
{
	if ((!Caster || !Target) && SpellID)
	{ 
		return;
	}

	sStackWorldPacket( data, SMSG_PERIODICAURALOG, 150);
	data << Target->GetNewGUID();		   // target guid
	data << Caster->GetNewGUID();
	data << SpellID;						// spellid
	data << (uint32)1;						// 1 simple cast, 2 channeled
	data << uint32(Flags | 0x1);			// aura = 1, type of action, 15 for mana regen ?
	data << Amount;							// amount of done to target / heal / damage
	data << uint32(Target->GetOverkill(Amount));
	data << g_spellSchoolConversionTable[School];
	data << uint32(abs_dmg);
	data << uint32(resisted_damage);
	data << crit;

#ifdef MINIMIZE_NETWORK_TRAFIC
	SendMessageToDuel( &data );
#else
	Caster->SendMessageToSet(&data, true);
#endif
}
*/
/*
void Object::SendSpellNonMeleeDamageLog( Object* Caster, Object* Target, uint32 SpellID, uint32 Damage, uint8 School, uint32 AbsorbedDamage, uint32 ResistedDamage, bool PhysicalDamage, uint32 BlockedDamage, bool CriticalHit, bool bToset )
{
	if ((!Caster || !Target) && SpellID)
	{ 
		return;
	}

	sStackWorldPacket( data, SMSG_SPELLNONMELEEDAMAGELOG, 150);
	data << Target->GetNewGUID();
	data << Caster->GetNewGUID();
	data << SpellID;                    // SpellID / AbilityID
	data << Damage;                     // All Damage
	if( Target->IsUnit() )
		data << uint32(SafeUnitCast(Target)->GetOverkill(Damage));
	else
		data << uint32(0);
	data << uint8(g_spellSchoolConversionTable[School]);                     // School
	data << AbsorbedDamage;             // Absorbed Damage
	data << ResistedDamage;             // Resisted Damage
	data << uint8(PhysicalDamage);      // Physical Damage (true/false)
	data << uint8(0);                   // unknown or it binds with Physical Damage
	data << BlockedDamage;		     // Physical Damage (true/false)
	data << uint8(CriticalHit ? 7 : 5);                   // unknown const
	data << uint32(0);

#ifdef MINIMIZE_NETWORK_TRAFIC
	SendMessageToDuel( &data );
#else
	Caster->SendMessageToSet( &data, bToset );
#endif
}*/

void Object::SendCombatLogMultiple(Unit *target, int32 base_dmg, int32 resisted, int32 absorbed, int32 blocked, uint32 spell_id, uint32 School_mask, uint32 log_type, uint8 is_critical, uint8 is_periodic )
{
/*
13329
- for fireball
01 00 00 00 - block counter ?
36 94 C6 9C - time
36 94 C6 9C - time for diff
7D 9D 00 00 - magic numbers hardcoded in client
F7 D1 CB 43 B9 A7 30 F5 - target c guid
8F D0 4B 74 03 04 - attacker c guid
85 00 00 00 - spell id
1E 00 00 00 - full dmg
FF FF FF FF - maybe overkill
04 
00 00 00 00 - school mask 
00 
00 00 00 00 - abs dmg ?
00 
00 00 00 00 
05 
00 00 00 00 
- flash heal log
01 00 00 00 - block count
EF D9 36 9D - time
EF D9 36 9D - time
5D 8C 00 00 - magic constant
8F 9D 4A 74 03 04 - target
8F 9D 4A 74 03 04 - attacker
0D 08 00 00 - spell id
A9 00 00 00 - full heal
8E 00 00 00 - overheal
00 00 00 00 - school mask ?
00 
00 critical ?
-evocation mana regen with glyph that gives 40% health
01 00 00 00 
C2 4A 4A 10 
C2 4A 4A 10 
57 F5 00 00 magic constant
8F E9 D7 38 03 01 target 
8F E9 D7 38 03 01 attacker
13 2F 00 00 spell
02 00 00 00  block count ?
15 00 00 00 mana type block ?
00 00 00 00 - type mana
63 34 00 00 - mana received
14 00 00 00 - heal type block ?
50 29 00 00 - 10576 over heal amount
50 29 00 00 - 10576 abs heal amount 
00 00 00 00 - real amount
00 critical ?

//mana gain
01 00 00 00 
60 35 5E 10 
60 35 5E 10 
56 3C 00 00 
8F E9 D7 38 03 01 
8F E9 D7 38 03 01 
95 71 00 00 
00 00 00 00 
44 03 00 00 
14333
//gain solar energy
{SERVER} Packet: (0xEE77) SMSG_COMBAT_LOG_MULTIPLE PacketSize = 42 TimeStamp = 38660651
01 00 00 00 
FF 8D FE 7C time 1 ?
68 8C FE 7C time 2
2A 7A 00 00 logtype
CF D7 74 3E 04 80 01 target
CF D7 74 3E 04 80 01 caster
B1 5C 01 00 - spell
08 00 00 00 - 8 type
14 00 00 00 - 20 amt
//starfire - dmg
{SERVER} Packet: (0xEE77) SMSG_COMBAT_LOG_MULTIPLE PacketSize = 63 TimeStamp = 38660432
01 00 00 00 
68 8C FE 7C 
68 8C FE 7C 
AE B2 00 00 
F7 53 CE 02 CD 8F 30 F1 
CF D7 74 3E 04 80 01 
60 0B 00 00 spell
47 00 00 00 base dmg
FF FF FF FF overkill ?
40 school mask arcane
00 00 00 00 abs
00 00 00 00 res
00 00 00 00 ?
00 00 ?
05 normal hit
00 00 00 00 blocked
//Rejuvenation( tick heal )
{SERVER} Packet: (0xEE77) SMSG_COMBAT_LOG_MULTIPLE PacketSize = 55 TimeStamp = 41463256
01 00 00 00 0C 52 29 7D 0C 52 29 7D A3 A2 00 00 CF D7 74 3E 04 80 01 CF D7 74 3E 04 80 01 06 03 00 00 
01 00 00 00 
08 00 00 00 
29 00 00 00 
29 00 00 00 
00 00 00 00 
00 
//Nourish( direct heal )
{SERVER} Packet: (0xEE77) SMSG_COMBAT_LOG_MULTIPLE PacketSize = 48 TimeStamp = 41476173
01 00 00 00 BC 84 29 7D BC 84 29 7D 73 F6 00 00 CF D7 74 3E 04 80 01 CF D7 74 3E 04 80 01 20 C5 00 00 
52 00 00 00 
52 00 00 00 
00 00 00 00 
00 
00 
//moonfire dot
{SERVER} Packet: (0xEE77) SMSG_COMBAT_LOG_MULTIPLE PacketSize = 63 TimeStamp = 42181391
01 00 00 00 
CF 46 34 7D 
CF 46 34 7D 
AE B2 00 00 
F7 70 D4 02 CD 8F 30 F1 
CF D7 74 3E 04 80 01 
D9 22 00 00 - spell
0A 00 00 00 
FF FF FF FF 
40 
00 00 00 00 
00 
00 00 00 00 
00 
00 00 00 00 
05 
00 00 00 00 

04 00 00 00 
CF 0C 3D C9 6C 0C 3D C9 AE B2 00 00 CF 23 A4 0F 01 80 01 CF 98 BC 20 02 80 01 
D9 22 00 00 
00 00 00 00 
FF FF FF FF 
40 
AB 04 00 00 
00 
00 00 00 00 
00 
00 00 00 00 
05 
00 00 00 00 
- block 2
6C 0C 3D C9 
73 F6 00 00 CF 80 EA 41 04 80 01 CF 80 EA 41 04 80 01 
7B 78 01 00 
28 00 00 00 
00 00 00 00 
00 00 00 00 00 00 6C 0C 3D C9 
AE B2 00 00 F9 0A 1D EC B4 40 F1 F9 0B C1 96 B4 40 F1 21 D3 00 00 58 08 00 00 FF FF FF FF 20 00 00 00 00 90 05 00 00 00 00 00 00 00 00 05 00 00 00 00 CF 0C 3D C9 A3 A2 00 00 F9 0B C1 96 B4 40 F1 CF 98 BC 20 02 80 01 45 E1 00 00 01 00 00 00 18 00 00 00 00 00 00 00 38 00 00 00 



//Judgements of the Wise - periodic energise
{SERVER} Packet: (0xEE77) SMSG_COMBAT_LOG_MULTIPLE PacketSize = 50 TimeStamp = 14475691
01 00 00 00 C4 CB 38 20 C4 CB 38 20 A3 A2 00 00 CF B2 03 E3 04 80 03 CF B2 03 E3 04 80 03 BA 7C 00 00 
01 00 00 00 
18 00 00 00 flags ?
00 00 00 00 mana
0C 00 00 00 amt ?
*/

	sStackWorldPacket( data, SMSG_COMBAT_LOG_MULTIPLE, 500 );
	data << uint32( 1 ); //block count. 
	data << uint32( getMSTime() ); //time diff will be used for no idea what client side
//	for(uint32 i=0;i<BlockCount;i++)
	{
		data << uint32( getMSTime() );
		data << uint32( log_type );
		data << target->GetNewGUID();
		data << this->GetNewGUID();
		data << spell_id;

		if( log_type == COMBAT_LOG_HEAL )
		{
			data << base_dmg;
			data << uint32( target->GetOverHeal( base_dmg ) );	
			data << uint32( absorbed );		//can be the same as overheal
			data << uint8( is_critical );
			data << uint8( 0 );	//?
		}
		else if( log_type == COMBAT_LOG_PERIODIC_HEAL || log_type == COMBAT_LOG_PERIODIC_ENERGIZE || log_type == COMBAT_LOG_PERIODIC_DAMAGE )
		{
			if( is_periodic == COMBAT_LOG_SUBFLAG_PERIODIC_HEAL )
			{
				data << uint32( 1 );	//?
		//		data << uint32( School_mask );
				data << uint32( COMBAT_LOG_SUBFLAG_PERIODIC_HEAL );
				data << base_dmg;
				data << uint32( target->GetOverHeal( base_dmg ) );	
				data << uint32( absorbed );		//can be the same as overheal
				data << uint8( is_critical );
			}
			else if( is_periodic == COMBAT_LOG_SUBFLAG_PERIODIC_ENERGIZE )
			{
				data << uint32( 1 );	//?
				data << uint32( COMBAT_LOG_SUBFLAG_PERIODIC_ENERGIZE );
				uint32 power_type = resisted; //variable name is wrong, reusing it for power type
				data << uint32( power_type );	
				data << base_dmg;
			}
			else
			{
				/*
				3A 15 3D C9 
				A3 A2 00 00 
				CF 23 A4 0F 01 80 01 
				CF 98 BC 20 02 80 01 
				D9 22 00 00 Moonfire DOT
				01 00 00 00 
				03 00 00 00 flag
				00 00 00 00 dmg
				FF FF FF FF overkill
				40 00 00 00 mask
				2E 03 00 00 absorbed
				CC 00 00 00 resisted
				00 crit

		*/
				data << uint32( 1 );	//?
				data << uint32( COMBAT_LOG_SUBFLAG_PERIODIC_DAMAGE );
				data << base_dmg;
				data << (uint32)( target->GetOverkill( base_dmg ) );	

				data << (uint32)(School_mask);

				data << uint32( absorbed );	
				data << uint32( resisted ); 

				if( is_critical == 0 )
					data << uint8( 0 );	
				else
					data << uint8( 7 );	

				data << uint32( blocked );	
			}
		}
		else if( log_type == COMBAT_LOG_ENERGIZE )	
		{
			uint32 power_type = resisted; //variable name is wrong, reusing it for power type
			data << uint32( power_type );	
			data << base_dmg;
		}
		else //dmg log
		{
/*
	01 00 00 00 7B A4 05 91 7B A4 05 91 AE B2 00 00 8F 9E FC 65 06 03 8F F3 D6 64 06 03 
E4 21 00 00 5D 00 00 00 FF FF FF FF 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 25 00 00 00 00 
	01 00 00 00 12 A6 05 91 86 A4 05 91 AE B2 00 00 8F 9E FC 65 06 03 8F 0A 15 60 06 03 
E4 21 00 00 6B 00 00 00 56 00 00 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 25 00 00 00 00 
	01 00 00 00 82 CD 05 91 82 CD 05 91 AE B2 00 00 CF 60 DE C9 04 80 06 8F E1 27 A0 04 01 
89 AD 00 00 92 00 00 00 FF FF FF FF 40 00 00 00 00 12 00 00 00 00 00 00 00 00 00 07 00 00 00 00 
	01 00 00 00 C1 D5 05 91 40 D4 05 91 AE B2 00 00 8F 33 F7 9A 05 06 8F 70 22 65 06 03 
49 36 00 00 28 00 00 00 FF FF FF FF 04 00 00 00 00 00 00 00 00 00 01 00 00 00 00 05 00 00 00 00 
	01 00 00 00 3B FA 06 91 3B FA 06 91 AE B2 00 00 CF A6 79 B9 04 80 06 8F D9 E4 9A 04 01 
E8 21 00 00 13 00 00 00 FF FF FF FF 08 00 00 00 00 02 00 00 00 00 01 00 00 00 00 05 00 00 00 00 
	01 00 00 00 53 BC 05 91 53 BC 05 91 AE B2 00 00 8F E1 27 A0 04 01 CF 60 DE C9 04 80 06 
E8 21 00 00 0F 00 00 00 FF FF FF FF 08 00 00 00 00 00 00 00 00 00 01 00 00 00 00 04 00 00 00 00 
	02 00 00 00 77 70 07 91 DD 6F 07 91 AE B2 00 00 8F 33 F7 9A 05 06 8F D9 E4 9A 04 01 
32 08 00 00 22 00 00 00 02 00 00 00 01 23 00 00 00 00 00 00 00 00 00 00 00 00 00 25 00 00 00 00


01 00 00 00 3B FA 06 91 3B FA 06 91 AE B2 00 00 CF A6 79 B9 04 80 06 8F D9 E4 9A 04 01 
E8 21 00 00 
13 00 00 00 
FF FF FF FF 
08 - school
00 00 00 00 abs 
02 00 00 00 res
00 ?
01 ?
00 00 00 00 
05 - non crit
00 00 00 00 - blocked
*/
			data << base_dmg;
			data << (uint32)( target->GetOverkill( base_dmg ) );	

			data << (uint8)(School_mask);

			data << uint32( absorbed );	
			data << uint32( resisted ); 
			data << uint8( 0 );		//  ?
			data << uint32( 0 );	// this spell has it as "1" 13897
			data << uint8( 0 );		// - some sub block counter. Client will try to read some aditional data if this is present

			//seen 4,5,7(0x02 when crit),25(0x20 when physical school?)
			uint8 SubFlags = 0;
			SubFlags |= 0x04; //no idea
			if( 1 ) //does not have it : Instant Poison
				SubFlags |= 0x01;
			if( School_mask == SCHOOL_MASK_NORMAL )
				SubFlags |= 0x20; //not sure what it is good for :(
			if( is_critical != 0 )
				SubFlags |= 0x02;

			data << uint8( SubFlags );	

			data << uint32( blocked );	//at some point i broke this and can't get it shown client side ?
		}
	}

	SendMessageToSet(&data, true );
}

void Object::SendAttackerStateUpdate( Object* Caster, Object* Target, dealdamage *Dmg, uint32 Damage, uint32 Abs, uint32 BlockedDamage, uint32 HitStatus, uint32 VState )
{
	if (!Caster || !Target || !Dmg)
	{ 
		return;
	}

/*
02 hitanimation
1C 00 00 8F 49 4B 74 03 attacker
04 D7 6E 9A 06 26 30 F1 victim
08 00 00 00 dmg
FF FF FF FF abs
01 - block count
01 00 00 00 school
00 00 00 41 float
08 00 00 00 dmg
01 victim state
E8 03 00 00 - 1000 
00 00 00 00 ?
//13393
02 04 80 00 
F7 5A 0C 0A 1B B5 30 F1 attacker
8F FF 3B 79 03 04 victim
04 00 00 00 damage
FF FF FF FF overkill ?
01 dmg type or block counter
01 00 00 00
00 00 80 40
04 00 00 00 
01 vstate
E8 03 00 00 
00 00 00 00 
41 00 00 00 

{SERVER} Packet: (0xBF56) SMSG_ATTACKERSTATEUPDATE PacketSize = 56 TimeStamp = 105596328
42 04 80 00 attacker state
F7 16 4E 5C E6 06 30 F1 attacker
8F 5F 23 91 02 03 victim
06 00 00 00 real recv dmg
FF FF FF FF overkill
01 ?
01 00 00 00 physical
00 00 98 41 full dmg as float
13 00 00 00 full dmg
0D 00 00 00 absorbed
01 victim state
FF FF FF FF ?
00 00 00 00 ?
7C 00 00 00 

02 3C C0 00 = HITSTATUS_HITANIMATION | 0x1000 | 0x2000 | 0x800 | 0x400 | 0x800000 | 0x400000
F7 4E 82 03 25 9D 30 F1 
8F E3 5D CF 02 04 
C6 00 00 00 
FF FF FF FF 
01 
01 00 00 00 
66 66 46 43 
C6 00 00 00 
01 
E8 03 00 00 
00 00 00 00 
2A 01 00 00 blocked dmg
82 00 00 00 
*/

	sStackWorldPacket( data ,SMSG_ATTACKERSTATEUPDATE, 250 );
	//0x4--dualwield,0x10 miss,0x20 absorbed,0x80 crit,0x4000 -glancing,0x8000-crushing
	//only for melee!

	data << (uint32)HitStatus;   
	data << Caster->GetNewGUID();
	data << Target->GetNewGUID();
		
	data << (uint32)Damage;				// Realdamage;
	if( this->IsUnit() )
		data << (uint32)(SafeUnitCast(this)->GetOverkill(Damage));	// Damage resisted
	else
		data << uint32(0);
	data << (uint8)1;					// Damage type counter / swing type

	data << (uint32)g_spellSchoolConversionTable[Dmg->school_type];				  // Damage school
	data << (float)Dmg->full_damage;	// Damage float
	data << (uint32)Dmg->full_damage;	// Damage amount

	if( HitStatus & HITSTATUS_ABSORBED )
		data << uint32( Abs );				// Damage absorbed

	if( HitStatus & HITSTATUS_RESIST )
		data << uint32( Dmg->resisted_damage );	// Damage resisted

	data << uint8( VState );
	data << uint32( 0 );					// can be 0,1000 or -1
	data << uint32( Dmg->SpellId );			// melee spell ID ?

	if( (HitStatus & HITSTATUS_BLOCK ) || (HitStatus & HITSTATUS_CRITICAL_BLOCK) )
		data << uint32( BlockedDamage );		// Damage amount blocked

	if ( HitStatus & HITSTATUS_UNK2 )
		data << uint32( 0 );				// unknown

	if( HitStatus & HITSTATUS_UNK )
	{
		data << uint32( 0 );
		data << float( 0 );
		data << float( 0 );
		data << float( 0 );
		data << float( 0 );
		data << float( 0 );
		data << float( 0 );
		data << float( 0 );
		data << float( 0 );

		data << float( 0 ); // Found in loop
		data << float( 0 );	// Found in loop
		data << uint32( 0 );
	}

	SendMessageToSet(&data, Caster->IsPlayer());
}

int32 Object::event_GetInstanceID()
{
	// return -1 for non-inworld.. so we get our shit moved to the right thread
	if(!IsInWorld())
	{ 
		return WORLD_INSTANCE;
	}
	else
		return m_instanceId;
}

void Object::EventSpellDamage(uint64 Victim, SpellEntry *sp, uint32 Damage, int32 pct_mod, int32 eff_index)
{
	if(!IsInWorld())
	{ 
		return;
	}

	Unit * pUnit = GetMapMgr()->GetUnit(Victim);
	if(pUnit == 0) 
	{ 
		return;
	}

	dealdamage tdmg;
	tdmg.base_dmg = Damage;
	tdmg.pct_mod_final_dmg = pct_mod;
	SpellNonMeleeDamageLog( pUnit, sp, &tdmg, 1, eff_index );
}

void Object::EventSpellDamageToLocation(uint64 Victim, SpellEntry *sp, uint32 Damage, int32 pct_mod, int32 eff_index, LocationVector src, LocationVector dst )
{
	if(!IsInWorld())
	{ 
		return;
	}

	Unit * pUnit = GetMapMgr()->GetUnit(Victim);
	if(pUnit == 0) 
	{ 
		return;
	}

	//check how far is the projectile
	float tagetx = pUnit->GetPositionX();
	float tagety = pUnit->GetPositionY();
	float tagetz = pUnit->GetPositionZ();

//	float distancesq = Distance3DSq( dst.x, dst.y, dst.z, tagetx, tagety, tagetz );
	//line distance to see if he is indeed dodging
	float PreciseDistance = abs( ( dst.x - src.x ) * ( src.y - tagety ) - ( src.x - tagetx ) * ( dst.y - src.y ) ) / sqrt ( ( dst.x - src.x ) * ( dst.x - src.x ) + ( dst.y - src.y ) * ( dst.y - src.y ) );

//	if( distancesq > 5.0f * 5.0f )
	//run speed 8 yards / sec ? 
	if( PreciseDistance > 2.2f )
		SendSpellLog(this, pUnit, sp->Id, SPELL_LOG_EVADE );
	else
	{
		sLog.outDebug("Spell missle distance from destination is %f", PreciseDistance );
		dealdamage tdmg;
		tdmg.base_dmg = Damage;
		tdmg.pct_mod_final_dmg = pct_mod;
		SpellNonMeleeDamageLog(pUnit, sp, &tdmg, 1, eff_index );
	}
}

void Object::EventWeaponDamageToLocation(uint64 Victim, SpellEntry *sp, uint32 Damage, int32 pct_mod, int32 eff_index, LocationVector src, LocationVector dst )
{
	if(!IsInWorld() || IsUnit() == false )
	{ 
		return;
	}

	Unit * pUnit = GetMapMgr()->GetUnit(Victim);
	if(pUnit == 0) 
	{ 
		return;
	}

	//check how far is the projectile
	float tagetx = pUnit->GetPositionX();
	float tagety = pUnit->GetPositionY();
	float tagetz = pUnit->GetPositionZ();

//	float distancesq = Distance3DSq( dst.x, dst.y, dst.z, tagetx, tagety, tagetz );
	//line distance to see if he is indeed dodging
	float PreciseDistance = abs( ( dst.x - src.x ) * ( src.y - tagety ) - ( src.x - tagetx ) * ( dst.y - src.y ) ) / sqrt ( ( dst.x - src.x ) * ( dst.x - src.x ) + ( dst.y - src.y ) * ( dst.y - src.y ) );

//	if( distancesq > 5.0f * 5.0f )
	//run speed 8 yards / sec ? 
	if( PreciseDistance > 2.2f )
	{
		SendSpellLog(this, pUnit, sp->Id, SPELL_LOG_EVADE );
//		this->SendCombatLogMultiple( pVictim, realdamage, dmg.resisted_damage, dmg.absorbed_damage, blocked_damage, ability->spell_id_client, school_mask, COMBAT_LOG_SPELL_DAMAGE, ( ( hit_status & HITSTATUS_CRICTICAL ) != 0 ) );
	}
	else
	{
		sLog.outDebug("Spell missle distance from destination is %f", PreciseDistance );
		SafeUnitCast( this )->Strike( pUnit, RANGED, sp, Damage, pct_mod, 0, false, false );
	}
}

bool Object::CanActivate()
{
	switch(m_objectTypeId)
	{
	case TYPEID_UNIT:
		{
			if(!IsPet())
			{ 
				return true;
			}
		}break;

	case TYPEID_GAMEOBJECT:
		{
			if(SafeGOCast(this)->HasAI() 
				//Tudi : why on earth traps can't have a default update tick ?
//				&& GetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_TYPEID) != GAMEOBJECT_TYPE_TRAP
				)
			{ 
				return true;
			}
		}break;
	}

	return false;
}

void Object::Activate(MapMgr * mgr)
{
	switch(m_objectTypeId)
	{
	case TYPEID_UNIT:
		mgr->activeCreatures.insert(SafeCreatureCast(this));
		break;

	case TYPEID_GAMEOBJECT:
		mgr->activeGameObjects.insert(SafeGOCast(this));
		break;
	}

	Active = true;

	InrageConsistencyCheck( true );
}

void Object::Deactivate(MapMgr * mgr)
{
	//very experimental -> static objects set by scripts that always get updated even if the cell beneath them is not active anymore !

	switch(m_objectTypeId)
	{
	case TYPEID_UNIT:
		//might crah like hell
		if( static_object == true && SafeUnitCast( this )->isAlive() )
		{
			return;
		}
		// check iterator
		if(mgr->activeCreaturesItr != mgr->activeCreatures.end() && (*mgr->activeCreaturesItr) == SafeCreatureCast(this))
			++mgr->activeCreaturesItr;
		mgr->activeCreatures.erase(SafeCreatureCast(this));
		break;

	case TYPEID_GAMEOBJECT:
		//might crah like hell
		if( static_object == true )
		{
			return;
		}
		mgr->activeGameObjects.erase(SafeGOCast(this));
		break;
	}
	Active = false;
}

void Object::SetByte(uint32 index, uint32 index1,uint8 value)
{
	ASSERT( index < m_valuesCount );
	// save updating when val isn't changing.
	#ifndef USING_BIG_ENDIAN
	uint8 * v =&((uint8*)m_uint32Values)[index*4+index1];
	#else
	uint8 * v = &((uint8*)m_uint32Values)[index*4+(3-index1)];
	#endif
	if(*v == value)
	{ 
		return;
	}

	*v = value;

	if(IsInWorld())
	{
		m_updateMask.SetBit( index );

		if(!m_objectUpdated)
		{
			m_mapMgr->ObjectUpdated(this);
			m_objectUpdated = true;
		}
	}

}

void Object::PlaySoundToSet(uint32 sound_entry)
{
	WorldPacket data(SMSG_PLAY_SOUND, 4);
	data << sound_entry;
	SendMessageToSet(&data, true);
}

void Object::PlaySoundTarget(uint32 sound_entry, uint64 guid)
{
	WorldPacket data(SMSG_PLAY_OBJECT_SOUND, 16);
	data << sound_entry;
	if( guid == 0 )
		data << GetGUID();
	else
		data << guid;
	SendMessageToSet(&data, true);
}

bool Object::IsInBg()
{
	MapInfo *pMapinfo = WorldMapInfoStorage.LookupEntry(this->GetMapId());
	if(pMapinfo)
	{
		return (pMapinfo->type_flags == INSTANCE_FLAG_PVP);
	}

	return false;
}

uint32 Object::GetTeam()
{
	if (IsPlayer())
	{
		return SafePlayerCast( this )->GetTeam();
	}
	if (IsPet())
	{
		if (SafePetCast( this )->GetPetOwner() != NULL)
		{
			return SafePetCast( this )->GetPetOwner()->GetTeam();
		}
	}
	if ( IsCreature() && SafeCreatureCast( this )->IsTotem() )
	{
		if (SafeCreatureCast( this )->GetTotemOwner() != NULL)
		{
			return SafeCreatureCast( this )->GetTotemOwner()->GetTeam();
		}
	}

	return (uint32)(-1);
}

//Manipulates the phase value, see "enum PHASECOMMANDS" in Object.h for a longer explanation!
void Object::Phase(uint8 command, uint32 newphase)
{
	switch( command )
	{
	case PHASE_SET:
		m_phase = newphase;
		break;
	case PHASE_ADD:
		m_phase |= newphase;
		break;
	case PHASE_DEL:
		m_phase &= ~newphase;
		break;
	case PHASE_RESET:
		m_phase = 1;
		break;
	default:
		return;
	}

/*	if ( IsPlayer() ) 
	{
		Player * p_player=SafePlayerCast( this );
		std::list<Pet*> summons = p_player->GetSummons();
		for(std::list<Pet*>::iterator itr = summons.begin(); itr != summons.end(); ++itr)
		{
			(*itr)->Phase(command, newphase);
		}
		//We should phase other, non-combat "pets" too...
	}*/

	AquireInrangeLock();
	for( InRangeSetRecProt::iterator itr=m_objectsInRange.begin(); itr!=m_objectsInRange.end(); ++itr )
	{
		if ( (*itr)->IsUnit() )
		{
			if( (*itr)->GetUInt64Value( UNIT_FIELD_CREATEDBY ) == this->GetUInt64Value( OBJECT_FIELD_GUID ) )
				(*itr)->Phase(command, newphase);
			SafeUnitCast( *itr )->UpdateVisibility();
		}
	}
	ReleaseInrangeLock();

	if ( IsUnit() )
		SafeUnitCast( this )->UpdateVisibility();

	return;
}

ARCEMU_INLINE uint32 Object::GetAreaID()
{
	return sTerrainMgr.GetAreaID( GetMapId(), GetPositionX(), GetPositionY(), GetPositionZ() );
}


#ifndef USE_STATIC_SET
ARCEMU_INLINE void Object::_AutoReleaseInrangeLock()
{ 
	m_UseQueueInrangeSetChange--;
	if( m_UseQueueInrangeSetChange == 0 )
		QueueInrangeProcessPending();
}

ARCEMU_INLINE void Object::_AutoAquireInrangeLock( InrangeLoopExitAutoCallback &Tracker )
{ 
	if( Tracker.o == NULL )
	{
		if( m_UseQueueInrangeSetChange == 0 )
			QueueInrangeProcessPending();
		Tracker.o = this;
		m_UseQueueInrangeSetChange++;
	}
}

InrangeLoopExitAutoCallback::~InrangeLoopExitAutoCallback()
{
	if( o )
		o->_AutoReleaseInrangeLock();
}
#else
ARCEMU_INLINE void Object::_AutoReleaseInrangeLock()
{ 
	m_objectsInRange.RecursionAutoLockRelease();
	m_inRangePlayers.RecursionAutoLockRelease();
}

ARCEMU_INLINE void Object::_AutoAquireInrangeLock( InrangeLoopExitAutoCallback &Tracker )
{ 
	if( Tracker.o == NULL )
	{
		Tracker.o = this;
		m_objectsInRange.RecursionAutoLockAquire();
		m_inRangePlayers.RecursionAutoLockAquire();
#ifdef _DEBUG
		Tracker.UnrollOrderCheck = IsInrangeObjectSetLocked();
#endif
	}
}

InrangeLoopExitAutoCallback::~InrangeLoopExitAutoCallback()
{
	if( o )
	{
#ifdef _DEBUG
		assert( UnrollOrderCheck == o->IsInrangeObjectSetLocked() );
#endif
		o->_AutoReleaseInrangeLock();
	}
}
#endif