/*
 * ArcEmu MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 * Copyright (C) 2008-2009 <http://www.ArcEmu.org/>
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

Vehicle::Vehicle(Unit *owner)
{
	owner->DestoryVehicle();	//in case we have a vehicle then we will destroy it
	m_owner = owner;
	m_vehicledata = NULL;
	m_vehicleEntry = NULL;
	m_seatdata = NULL;
	IsMount = false;
	memset(&m_passengers, 0, sizeof(Unit*) * MAX_PASSENGERS);
	m_controller = NULL;
	m_passengerCount = 0;
	owner->SetUInt32Value(UNIT_DYNAMIC_FLAGS, 0);
//	owner->SetFlag(UNIT_FIELD_FLAGS , UNIT_FLAG_MOUNTABLE | UNIT_FLAG_PLAYER_CONTROLLED );
//	owner->SetFlag(UNIT_FIELD_FLAGS , UNIT_FLAG_MOUNTABLE );
	if( m_owner->IsPlayer() )
		owner->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_VEHICLE);
	else
		owner->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
}

Vehicle::~Vehicle()
{
	m_vehicledata = NULL;
	m_vehicleEntry = NULL;
	m_seatdata = NULL;
}

void Vehicle::LoadFromDB()
{
	uint32 VehicleID = GetVehicleID();
	m_vehicleEntry = dbcVehicleEntry.LookupEntryForced(VehicleID);
	m_vehicledata = VehicleDataStorage.LookupEntry(VehicleID);
	m_seatdata = SeatDataStorage.LookupEntry(VehicleID);
	if(m_vehicledata == NULL || m_vehicleEntry == NULL || m_seatdata == NULL) // No database support for this vehicle
	{
		printf("No or invalid vehicle data was found for vehicleid %u, removing vehicle from world.\n", VehicleID);
		return;
	}
//	m_movementFlags = static_cast<uint16>(m_vehicledata->movementflags);
//	m_turnRate = m_vehicleEntry->m_turnSpeed;
//	m_pitchRate = m_vehicleEntry->m_pitchSpeed;

	if( (m_vehicledata->flags & VEHICLE_FLAG_DISABLE_AI_ON_SPAWN) && m_controller != NULL )
		m_controller->DisableAI();
}

void Vehicle::LoadStaticPassengers()
{
/*	ASSERT( m_owner && m_owner->IsInWorld() );
	if( m_owner->IsCreature() == false )
		return;
	if( SafeCreatureCast( m_owner )->m_spawn == NULL )
		return;
	uint32 spawnid = SafeCreatureCast( m_owner )->m_spawn->id;
	QueryResult * qresult = WorldDatabase.Query("SELECT parentspawnid, seatid, orientation, flags FROM vehicle_staticpassengers WHERE spawnid = '%u'", spawnid);
	if(qresult)
	{
		Field * r = qresult->Fetch();

		ParentInfo * inf = new ParentInfo();
		inf->spawnid = spawnid;
		inf->parentspawnid = r[0].GetUInt32();
		inf->seatid = r[1].GetUInt8();
		if(inf->seatid >= MAX_PASSENGERS)
		{
//			SetParentInfo(NULL);
			delete inf;
			delete qresult;
			return;
		}
		LocationVector offset = m_seatdata->seat[inf->seatid].offset;
		inf->offsetX = offset.x;
		inf->offsetY = offset.y;
		inf->offsetZ = offset.z;
		inf->orientation = r[2].GetFloat();
		inf->flags = r[3].GetUInt32();
		
//		SetParentInfo(inf);
		delete qresult;
	}*/
}

bool Vehicle::HasAllProtoAvailable()
{
	return (m_vehicledata != NULL && m_vehicleEntry != NULL && m_seatdata != NULL);
}

void Vehicle::AddPassenger(Unit * passenger, int8 seat)
{
	if( HasAllProtoAvailable() == false )
		return;

	ASSERT( passenger );
	if( !passenger->IsInWorld() )
		return;

//	if(GetPassengerCount() + 1 > GetOccupancyLimit()) // Check if there is place left for us
	//passanger 0 is the controller and does not require a seat
	if( GetPassengerCount() > GetOccupancyLimit() ) // Check if there is place left for us
	{
		return;
	}

	// Check if we're a vehicle trying to mount a non-player seat position
	if( seat == VEHICLE_PASSENGER_SLOT_INVALID || seat >= MAX_PASSENGERS || seat > GetOccupancyLimit() || m_passengers[seat] != NULL)
	{
		return;
	}
	//right now mount owners are using seat 0 with is not actually a seat. In case there are crash reports we should check if picked seat is also valid in DBC
//	if( m_vehicleEntry->m_seatID[ seat ] ==  0 ) return;

	m_passengers[seat] = passenger;
	// Set this vehicle and seat to player
	passenger->SetVehicle(this, seat);
	//set controler
	if( m_controller == NULL )
	{
		m_controller = passenger;
		if( IsMount == false )
		{
			//should not posses self ?
			if( m_controller != m_owner && passenger->IsPlayer() )
				SafePlayerCast( passenger )->Possess( m_owner );
//			passenger->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);
		}
	}
	if( passenger->IsPlayer() )
	{
		Player * player = SafePlayerCast(passenger);
		if( passenger != m_owner )
		{
			if( IsMount == false )
				SendFarsightPacket(player, true);	//focus on vehicle instead us ? But why ?
			player->GetSession()->SetActiveMover( GetNewGUID() );	//loose movement control ?
		}
		if( IsMount == false )
			player->m_noFallDamage = true;

		// Show the vehicle action bar. Is this to owner or to seat ?
		SendVehicleActionBar( player );

		if( passenger != m_owner )
			SendRidePacket( passenger, GetSeatData()->seat[seat] );

		//seems like this is a mount
		if( passenger == m_owner )
		{
			// Send others that we now have a vehicle
			sStackWorldPacket( data, SMSG_CONVERT_VEHICLE, 9+4);
			data << passenger->GetNewGUID() ;
			data << uint32( proto->vehicleId );
			player->SendMessageToSet(&data,true);
			//is this for all players or only the real vehicle controller
			data.Initialize(SMSG_CONTROL_VECHICLE);
			player->GetSession()->SendPacket(&data); 
		}
	}

	// Remove shapeshift auras
	passenger->RemoveAllAuraType(SPELL_AURA_MOD_SHAPESHIFT);
	passenger->RemoveAllAuraType(SPELL_AURA_TRANSFORM);

	if( passenger != m_owner )
	{
		passenger->Root();
		passenger->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
	}
	else if( passenger == m_owner )
		m_owner->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_VEHICLE);

	if(m_vehicledata->flags & VEHICLE_FLAG_DISABLE_AI_ON_ENTER)
		m_owner->DisableAI();

	// Check if we exceeded maximum passenger capacity
//	if( GetPassengerCount() >= GetOccupancyLimit() )
//		m_owner->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);

	m_passengerCount++; // Count the passenger as inside the vehicle
}

void Vehicle::RemovePassenger(Unit * passenger)
{
	if( HasAllProtoAvailable() == false )
		return;
	ASSERT(this && passenger);
	if (passenger == NULL ||										// Are we passing NULL pointers?
		passenger->GetVehicle() != this ||							// Are we trying to dismount from something invalid?
		passenger->GetSeatID() >= MAX_PASSENGERS ||					// Are we trying to dismount someone in an inexcisting seat?
		m_passengers[passenger->GetSeatID()] != passenger)			// Are we trying to dismount someone who's seat we do not own?
	{
//		ASSERT( false );	//wow, we are not expecting this situation at all
		return;
	}

	if(passenger->IsPlayer()) // The passenger is a player
	{
		Player * player = static_cast<Player*>(passenger);

		//remove pet action bar in case we had any
		sStackWorldPacket( data, SMSG_PET_SPELLS, 9 );
		data << uint64(0);
		player->GetSession()->SendPacket(&data);

		if( passenger != m_owner )
		{
//			SendHeartbeatPacket(player);
//			RelocateToVehicle(player);
			if( IsMount == false )
				SendFarsightPacket(player, false);

//			player->SetUInt64Value(UNIT_FIELD_TARGET, 0);	//loose targetting, but why ?
			if( IsMount == false && player == m_controller) // Are we the driver of this vehicle?
			{
				m_controller = NULL;
				player->UnPossess();
//				passenger->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);
			}

			player->GetSession()->SetActiveMover( player->GetNewGUID() );
			passenger->Unroot();

			WorldPacket data;
			data.Initialize(SMSG_MONSTER_MOVE);
			data << player->GetNewGUID();		// PlayerGUID
			data << uint8(0x40);				// Unk - blizz uses 0x40
			data << player->GetPosition();		// Player Position xyz
			data << getMSTime();				// Timestamp
			data << uint8(0x4);					// Flags
			data << player->GetOrientation();	// Orientation
			data << uint32(0x01000000);			// MovementFlags
			data << uint32(0);					// MovementTime
			data << uint32(1);					// Pointcount
			data << m_owner->GetPosition();		// Vehicle Position xyz
			player->SendMessageToSet(&data, true);
		}
		if( passenger == m_owner )
		{
			// Send others that we are no longer a vehicle
			sStackWorldPacket( data, SMSG_CONVERT_VEHICLE, 9+4);
			data << passenger->GetNewGUID() ;
			data << uint32( 0 );
			player->SendMessageToSet(&data,true);
		}
		player->m_noFallDamage = false;
	}
	else if(passenger->IsCreature())
	{
		static_cast<Creature*>(passenger)->ResetParentInfo();
		passenger->SetPosition( m_owner->GetPositionX(), m_owner->GetPositionY(), m_owner->GetPositionZ(), m_owner->GetOrientation(), true);
	}
	else
	{
		sLog.outDebug("Error while removing passenger.");
	}

	//clear refs
	uint8 seat = passenger->GetSeatID();
	passenger->m_vehicle_data = NULL;
	passenger->m_CurrentVehicleSeat = VEHICLE_PASSENGER_SLOT_INVALID;
	m_passengers[seat] = NULL;

	passenger->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_MOUNTABLE);
	passenger->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_VEHICLE);
//	if( GetPassengerCount() < GetOccupancyLimit() && m_owner->IsPlayer() == false && m_owner != passenger )
//		m_owner->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);

	if(m_vehicledata->flags & VEHICLE_FLAG_DISABLE_AI_ON_ENTER)
		m_owner->EnableAI();

	--m_passengerCount;
	if( m_passengerCount <= 0 && IsMount )
	{
		m_owner->DestoryVehicle();
		return;	//we just killed ourself, do not do anything after that function call
	}
}

void Vehicle::EjectAllPassengers()
{
	for (uint8 seat = 0; seat < MAX_PASSENGERS; seat++)
		if(m_passengers[seat] != NULL && m_passengers[seat] != m_owner )
			m_passengers[seat]->ExitVehicle();
	m_owner->ExitVehicle();
}

void Vehicle::MoveVehicle(float x, float y, float z, float o)
{
	m_owner->SetPosition(x, y, z, o);

	int8 passengercount = GetPassengerCount();
	for (int8 i=0; i < passengercount; i++)
		if( m_passengers[i] != NULL && m_passengers[i] != m_owner )	//there is a chance that owner is also present in vehicle. Is there a double setposition there ?
			m_passengers[i]->SetPosition(x, y, z, o);
}

void Vehicle::SetDeathState(DeathState s)
{
	m_owner->setDeathState( s );
	EjectAllPassengers();
}

///////////////////////////////////////////////////////////////////////////////
void Unit::ConvertToVehicle(CreatureProto *cp, bool skip_non_vehicle, bool is_mount)
{
	if( cp == NULL || ( skip_non_vehicle == true && cp->vehicleId==0 ) )
		return;
	m_vehicle_data = new Vehicle( this );
	m_vehicle_data->proto = cp;				//the main thing
	m_vehicle_data->LoadFromDB();			//set local variables based on vehicle type
	m_vehicle_data->IsMount = is_mount;		//unmounting controller kicks our the rest of the passangers
	if( is_mount )
		EnterVehicle( m_vehicle_data, 0 );
	m_vehicle_data->LoadStaticPassengers();	//attach units to it in case he has. Crap, what if we are not added to world yet ? Well you should convert a vehicle after adding to world :P
}

//!!!! watch from where you call this function. You probably want to make sure that vehicle pointer is not stored in some local variable !
void Unit::DestoryVehicle()
{
	if( m_vehicle_data && m_vehicle_data->GetOwner() == this )
	{
		m_vehicle_data->EjectAllPassengers();
		delete m_vehicle_data;
		m_vehicle_data = NULL;
	}
}


ARCEMU_INLINE void Unit::SetVehicle(Vehicle *v, int8 seat)
{
	m_vehicle_data = v;
	m_CurrentVehicleSeat = seat;
}

bool Unit::EnterVehicle(Vehicle * vehicle, int8 preferedseat)
{
	if( vehicle == NULL || vehicle->HasAllProtoAvailable() == false )
		return false;

	RemoveAurasByInterruptFlag(AURA_INTERRUPT_ON_MOUNT | AURA_INTERRUPT_ON_INVINCIBLE);
	if( preferedseat == VEHICLE_PASSENGER_SLOT_INVALID ) // Lovely, freedom of choice
	{
		for (int8 i=0; i <= vehicle->GetOccupancyLimit(); i++)
			if( vehicle->m_vehicleEntry->m_seatID[i] != 0 && vehicle->GetPassenger(i) == NULL )
			{
				preferedseat = i;
				break;
			}
	}

	if(preferedseat < MAX_PASSENGERS)
	{
		if( preferedseat > vehicle->GetOccupancyLimit())
		{
				return false;
		}
	}
	else
	{
		return false;
	}

	ExitVehicle();

	if( vehicle->GetPassenger(preferedseat) == NULL ) // There's some room available here
	{
		vehicle->AddPassenger(this, preferedseat);
		return true;
	}
	return false;
}

void Unit::ExitVehicle()
{
	Vehicle *m_vehicle = GetVehicle();
	if(m_vehicle == NULL)
		return;
	m_vehicle->RemovePassenger(this);
	RemoveAurasByInterruptFlag( AURA_INTERRUPT_ON_DISMOUNT );
	//GetAIInterface()->SendMoveToPacket(GetPositionX() - 1.0f, GetPositionY() - 1.0f, GetPositionZ() - 1.0f, GetOrientation(), 1000, 0x1000);
}

bool Unit::IsVehicleSeatHealthShielded()
{
//	if( m_vehicle_data )
//		return true;
	return false;	//later we will check if seat is protected by vehicle.mostly they are
}

uint32	Unit::GetCurrentSeatEntry()
{
	if( m_vehicle_data == NULL )
		return 0;
	VehicleEntry * v = dbcVehicleEntry.LookupEntryForced( GetCurrentVehicleID() );
	if(v) 
		return v->m_seatID[ GetSeatID() ];
	return 0;
}

ARCEMU_INLINE uint32 Unit::GetCurrentVehicleID() 
{ 
	if( m_vehicle_data )
		return m_vehicle_data->proto->vehicleId;
	return 0; 
}

/************************************************************************/
/* Vehicle Mount Opcode Handlers                                        */
/************************************************************************/

void WorldSession::HandleEnterVehicleOpcode(WorldPacket & recv_data)
{
	CHECK_INWORLD_RETURN;

	uint64 vehicleGuid;
	recv_data >> (uint64)vehicleGuid;

	Player * pl = GetPlayer();
	Player * owner = pl->GetMapMgr()->GetPlayer( vehicleGuid );
	if( owner == NULL || owner->GetVehicle() == NULL || GetPlayer()->GetGroup() == NULL || GetPlayer()->GetGroup() != owner->GetGroup() )
	{
		return;
	}

	pl->EnterVehicle( owner->GetVehicle(), VEHICLE_PASSENGER_SLOT_INVALID );
}

/************************************************************************/
/* Vehicle Opcodes Handlers                                             */
/************************************************************************/

 void WorldSession::HandleVehicleDismiss(WorldPacket & recv_data)
 {
	if (GetPlayer() == NULL || GetPlayer()->GetVehicle() == NULL)
		return;

	GetPlayer()->ExitVehicle();
 }
 
void WorldSession::HandleVehicleSwitchSeat(WorldPacket & recv_data)
{
	CHECK_INWORLD_RETURN;

	WoWGuid Vehicleguid;
	uint8 RequestedSeat;
	if(recv_data.GetOpcode() == CMSG_REQUEST_VEHICLE_SWITCH_SEAT)
	{
		recv_data >> Vehicleguid;
		recv_data >> RequestedSeat;
	}
	else
	{
		HandleMovementOpcodes(recv_data);
		recv_data >> Vehicleguid;
		recv_data >> RequestedSeat;
	}
	if(RequestedSeat == GetPlayer()->GetSeatID())
		return;
	uint64 guid = Vehicleguid.GetOldGuid();
	Vehicle * v = GetPlayer()->GetVehicle( );
	if( v )
		GetPlayer()->EnterVehicle(v, RequestedSeat);
}

void WorldSession::HandleVehicleEjectPassenger(WorldPacket & recv_data)
{
	CHECK_INWORLD_RETURN;
	uint64 passengerguid;
	recv_data >> passengerguid;

	Vehicle * v = GetPlayer()->GetVehicle();
	if(v)
	{
		for (uint8 i = 0; i < MAX_PASSENGERS; i++)
		{
			Unit * p = v->GetPassenger(i);
			if(p && p->GetGUID() == passengerguid)
			{
				p->ExitVehicle();
				break;
			}
		}
	}
}

/************************************************************************/
/* Vehicle Mount Packets                                                */
/************************************************************************/
/*
void Mount::SendRidePacket(SeatInfo info, Unit * passenger)
{
	WorldPacket data(SMSG_MONSTER_MOVE_TRANSPORT, 100);
	data << passenger->GetNewGUID();						// Passengerguid
	data << m_mountowner->GetNewGUID();						// Transporterguid (vehicleguid)
	data << info.seatid;									// Vehicle Seat ID
	data << uint8(0);										// Unknown
	data << info.offset.x;									// OffsetTransporterX
	data << info.offset.y;									// OffsetTransporterY
	data << info.offset.z;									// OffsetTransporterZ
	data << getMSTime();									// Timestamp
	data << uint8(0x04);									// Flags
	data << float(0.0f);									// Orientation
	data << uint32(0x800000);								// MovementFlags
	data << uint32(0);										// MoveTime
	data << uint32(1);										// Points
	data << float(0);										// OffsetSeatX - unused for mounts
	data << float(0);										// OffsetSeatY - unused for mounts
	data << float(0);										// OffsetSeatZ - unused for mounts
	m_mountowner->SendMessageToSet(&data, true);
}

void Mount::SendHeartbeatPacket(Unit * passenger, bool MonsterMoveDebug)
{
	WorldPacket data;
	if(MonsterMoveDebug)
	{
		data.Initialize(SMSG_MONSTER_MOVE);
		data << passenger->GetNewGUID();		// PlayerGUID
		data << uint8(0);						// Unk - blizz uses 0x40
		data << passenger->GetPosition();		// Player Position xyz
		data << getMSTime();					// Timestamp
		data << uint8(0x4);						// Flags
		data << passenger->GetOrientation();	// Orientation
		data << uint32(0x1000000);				// MovementFlags
		data << uint32(0);						// MovementTime
		data << uint32(1);						// Pointcount
		data << m_mountowner->GetPosition();	// Vehicle Position xyz
		m_mountowner->SendMessageToSet(&data, false);
	}
}

void Mount::ConvertToVehicle(uint32 vehicleid)
{
	if(m_mountowner == NULL)
		return;

	WorldPacket pkt(SMSG_CONVERT_VEHICLE, 84);
	pkt << m_mountowner->GetNewGUID();
	pkt << vehicleid;
	m_mountowner->SendMessageToSet(&pkt, true);

	if(vehicleid)
	{
		m_mountowner->SetUInt32Value(UNIT_DYNAMIC_FLAGS, 0);
		m_mountowner->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_VEHICLE);
		m_mountowner->SetFlag(UNIT_FIELD_FLAGS , UNIT_FLAG_MOUNTABLE);
	}
	else
	{
		m_mountowner->SetUInt32Value(UNIT_DYNAMIC_FLAGS, 0);
		m_mountowner->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_VEHICLE);
		m_mountowner->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_MOUNTABLE);
	}
}*/

/************************************************************************/
/* Vehicle Packets                                                      */
/************************************************************************/
/*
void Vehicle::SendHeartbeatPacket(Unit * passenger)
{
	WorldPacket data;
	data.Initialize(SMSG_MONSTER_MOVE);
	data << passenger->GetNewGUID();		// PlayerGUID
	data << uint8(0);						// Unk - blizz uses 0x40
	data << passenger->GetPosition();		// Player Position xyz
	data << getMSTime();					// Timestamp
	data << uint8(0x4);						// Flags
	data << passenger->GetOrientation();	// Orientation
	data << uint32(0x1000000);				// MovementFlags
	data << uint32(0);						// MovementTime
	data << uint32(1);						// Pointcount
	data << m_owner->GetPosition();			// Vehicle Position xyz
	m_owner->SendMessageToSet(&data, false);

	// Clear update block info
//	if(passenger->IsCreature())
//	{
//		Creature * c = static_cast<Creature*>(passenger);
//		ParentInfo * info = c->GetParentInfo();
//		if(info) delete info;
//		c->SetParentInfo(NULL);
//	}
}
*/
void Vehicle::SendRidePacket(Unit * passenger, SeatInfo seat)
{
/*
13329
{SERVER} Packet: (0x777C) SMSG_MONSTER_MOVE_TRANSPORT PacketSize = 59 TimeStamp = 9723625
8F 43 E7 38 03 01 
8F F4 CB 38 03 01 
01 
00 
00 08 C5 3F 
00 A8 D8 3E 
80 A9 20 3F 0.62758636474609 
1E 39 5C 02 
04 
00 00 00 00 
00 00 80 00 
00 00 00 00 
01 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
143333
{SERVER} Packet: (0xB2BA) SMSG_MONSTER_MOVE_TRANSPORT PacketSize = 59 TimeStamp = 14638931
8F E9 D7 38 03 01 
8F 07 E2 91 03 01
00 
00 
00 80 28 3F 
00 FE 03 C0 
00 C0 08 BD 
64 0E C9 00 
04 
00 00 00 00 
00 00 10 00 
00 00 00 00
01 00 00 00
00 00 00 00 
00 00 00 00
00 00 00 00 
*/
	sStackWorldPacket( data, SMSG_MONSTER_MOVE_TRANSPORT, 100 + 100);
	data << passenger->GetNewGUID();								// Passengerguid
	data << GetNewGUID();											// Transporterguid (vehicleguid)
	data << uint8(passenger->GetSeatID());							// Vehicle Seat ID
	data << uint8(0);												// Unknown
	data << passenger->GetPositionX() - m_owner->GetPositionX();	// OffsetTransporterX
	data << passenger->GetPositionY() - m_owner->GetPositionY();	// OffsetTransporterY
	data << passenger->GetPositionZ() - m_owner->GetPositionZ();	// OffsetTransporterZ
	data << getMSTime();											// Timestamp
	data << uint8(0x04);											// Flags
	data << float(0.0f);											// Orientation
	data << uint32(0x00800000);										// MovementFlags
	data << uint32(0);												// MoveTime
	data << uint32(1);												// Points
	data << seat.offset.x;											// OffsetSeatX
	data << seat.offset.y;											// OffsetSeatY
	data << seat.offset.z;											// OffsetSeatZ
	m_owner->SendMessageToSet(&data, true);

	// Initialize update block info
/*	if(passenger->IsCreature())
	{
		Creature * c = static_cast<Creature*>(passenger);
		ParentInfo * info = c->GetParentInfo();
		if(info) 
			delete info;
		else 
			info = new ParentInfo();
		info->parentspawnid = spawnid;
		info->spawnid = c->spawnid;
		info->offsetX = seat.offset.x;
		info->offsetY = seat.offset.y;
		info->offsetZ = seat.offset.z;
		info->orientation = GetOrientation();
		info->seatid = c->GetSeatID();
		c->SetParentInfo(info); 

		c->GetAIInterface()->SendMoveToPacket(GetPositionX(), GetPositionY(), GetPositionZ(), GetOrientation(), 0, MONSTER_MOVE_TELEPORT_VEHICLE);
		c->SetPosition(GetPositionX(), GetPositionY(), GetPositionZ(), GetOrientation(), true);
	}*/
}

/*
void Vehicle::RelocateToVehicle(Player * player)
{
	//player->SetPlayerStatus(TRANSFER_PENDING);
	//player->m_sentTeleportPosition.ChangeCoords(GetPositionX(), GetPositionY(), GetPositionZ());
}*/

void Vehicle::SendFarsightPacket(Player * player, bool enabled)
{
	if (enabled)
	{
		player->SetUInt64Value(PLAYER_FARSIGHT, GetGUID());
		WorldPacket ack(SMSG_CONTROL_VECHICLE, 0);
		if (player->GetSession() != NULL)
			player->GetSession()->SendPacket(&ack);
	}
	else
	{
		player->SetUInt64Value(PLAYER_FARSIGHT, 0);
		player->SetUInt64Value(UNIT_FIELD_CHARM, 0);
	}
}

void Vehicle::SendVehicleActionBar(Player * player,bool if_has_any,int8 seat)
{
	sStackWorldPacket( data, SMSG_PET_SPELLS, 63+5);
	data << GetGUID();
	data << uint16(0x0000);//pet family
	data << uint32(0x00000000);//unk
	data << uint32(0x00000101);//bar type
	uint32 found_spells = 0;
	for (int i = 0; i < 10; i++)
	{
		if( m_vehicledata->spells[i] )
			found_spells++;
		if(i < 5)
			data << uint16(m_vehicledata->spells[i]) << uint8(0) << uint8(i+8);
		else
			data << uint16(0) << uint8(0) << uint8(i+8);
	}

	data << uint16(0)
		 << uint16(DEFAULT_SPELL_STATE);
	
	data << uint8(0);

	if( if_has_any == false || found_spells )
		player->GetSession()->SendPacket(&data);

	/*data.Initialize(SMSG_PET_GUIDS);
	data << uint32(1);
	data << GetGUID();
	player->GetSession()->SendPacket(&data);*/
}

bool Vehicle::CanCastSpell( uint32 spellID )
{
	if( m_vehicledata == NULL )
		return false;
	for( uint32 slot = 0; slot < MAX_VEHICLE_SPELLS; slot++ )
		if( m_vehicledata->spells[ slot ] == spellID )
			return true;
	//cooldown will be stored inside player ?
	return false;
}

void WorldSession::HandleDismissVehicle(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: Recvd CMSG_DISMISS_CONTROLLED_VEHICLE");

	if( !_player->IsInWorld() )
		return;

	Vehicle *vehicle = _player->GetVehicle();
    if( vehicle == NULL )
        return;

    // using charm guid, because we don't have vehicle guid...
	vehicle->RemovePassenger( _player );
}


void WorldSession::HandleSpellClick(WorldPacket& recvPacket)
{
	sLog.outDetail("WORLD: got CMSG_SPELLCLICK packet, data length = %i",recvPacket.size());

	if(!_player->IsInWorld()) 
	{ 
		sLog.outDebug("CMSG_SPELLCLICK : player not in world\n");
		return;
	}
	if(_player->getDeathState()==CORPSE)
	{ 
		sLog.outDebug("CMSG_SPELLCLICK : player dead\n");
		return;
	}

	uint64 target_guid; // this will store the guid of the object we are going to use it's spell. There must be a dbc that indicates what spells a unit has

	recvPacket >> target_guid;

	//we have only 1 example atm for entry : 28605
	Unit *target_unit = _player->GetMapMgr()->GetUnit( target_guid );

	if( !target_unit )
	{ 
		sLog.outDebug("CMSG_SPELLCLICK : cannot find targer\n");
		return;
	}

	if( !target_unit->IsCreature() )
	{ 
		sLog.outDebug("CMSG_SPELLCLICK : target is not creature\n");
		return;
	}

	Creature *target_creature = SafeCreatureCast( target_unit ); 
	if( ! target_creature->GetProto() )
	{
		sLog.outDebug("CMSG_SPELLCLICK : target has no proto \n");
		return; //wtf ?
	}

	//is this condicioned by some quest ?
	if( target_creature->GetProto()->vehicle_use_required_quest != 0 )
	{
		if ( _player->GetQuestStatus( target_creature->GetProto()->vehicle_use_required_quest ) != QUEST_STATUS_INCOMPLETE )
		{
			sLog.outDebug("CMSG_SPELLCLICK : Player does not have active quest %u \n",target_creature->GetProto()->vehicle_use_required_quest);
			_player->BroadcastMessage("You need an active quest to use this");
			return;
		}
	}

	uint32 cast_spell_id = target_creature->GetProto()->on_click_spell;
	if( cast_spell_id != 0 )
	{
		SpellEntry *spellInfo = dbcSpell.LookupEntryForced( cast_spell_id );
		if( !spellInfo )
		{
			sLog.outDetail("Invalid clickspell %u for entry %u",cast_spell_id,target_creature->GetProto()->Id);
			return;
		}
		Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		if( target_creature->GetProto()->on_click_spell_caster_is_player )
		{
			spell->Init(_player, spellInfo, false, NULL);
			if( target_creature->GetProto()->on_click_spell_target_is_clicker )
			{
				SpellCastTargets targets( _player->GetGUID() );
				spell->prepare(&targets);
			}
			else
			{
				SpellCastTargets targets( target_guid );
				spell->prepare(&targets);
			}
		}
		else
		{
			spell->Init(target_creature, spellInfo, false, NULL);
			if( target_creature->GetProto()->on_click_spell_target_is_clicker )
			{
				SpellCastTargets targets( _player->GetGUID() );
				spell->prepare(&targets);
			}
			else
			{
				SpellCastTargets targets( target_guid );
				spell->prepare(&targets);
			}
		}
	}
	else if( target_unit->GetVehicle() )
	{
		Vehicle *pVehicle = target_unit->GetVehicle(); 

		// just in case.
//		if( sEventMgr.HasEvent( pVehicle, EVENT_VEHICLE_SAFE_DELETE ) )
//		{
//			sLog.outDetail("WORLD: vehicle is getting deleted, cannot add passanger" );
//			return;
//		}
		_player->EnterVehicle( pVehicle, VEHICLE_PASSENGER_SLOT_INVALID );
	}
	else
	{
		sLog.outDetail("WORLD: creature with entry = %u should have onclick spell but it is missing",target_unit->GetEntry() );
		return;
	}

	//lightwell is just special
	if( target_unit->HasAura(59907) )
	{
		target_unit->RemoveAura(59907);
		if( !target_unit->HasAura(59907) )
			if(target_unit->IsCreature())
				SafeCreatureCast(target_unit)->Despawn(0,0);
	}

}
