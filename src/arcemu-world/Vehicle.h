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

#ifndef _VEHICLE_H_
#define _VEHICLE_H_

class creature;
class CreatureProto;

#define MAX_PASSENGERS 8
#define MAX_VEHICLE_SPELLS 5

#define VEHICLE_PASSENGER_SLOT_INVALID ((int8)(-1))

//#define MAX_MOUNT_PASSENGERS 4

#pragma pack(push, 1)
struct VehicleData
{
	uint32 Id;
	uint32 flags;
	uint32 movementflags;
	uint32 spells[MAX_VEHICLE_SPELLS];
};

struct SeatInfo
{
	uint8			seatid;
	uint32			entryid;
	LocationVector	offset;
	bool			is_dmg_protected;	//i think all are, not implemented atm
};

struct SeatData
{
	uint32			Id;
	uint8			capacity;
	SeatInfo		seat[MAX_PASSENGERS];
};

#pragma pack(pop)

enum VehicleFlags
{
	VEHICLE_FLAG_NONE,
	VEHICLE_FLAG_DISABLE_AI_ON_SPAWN,
	VEHICLE_FLAG_DISABLE_AI_ON_ENTER,
};

enum VehiclePowerType
{
    POWER_TYPE_PYRITE		= 41,
	POWER_TYPE_STEAM		= 61,
};

//class SERVER_DECL Vehicle : public Creature
class SERVER_DECL Vehicle
{
//	friend class Unit;
//	friend class Player;
	// Vehicle specific data
	Unit			*m_passengers[MAX_PASSENGERS];
	//mounts have a passanger always. MOB vehicles do not self mount
	Unit			*m_owner;		//sucks if this will lead to a corrupted pointer. Don't fuck with us kk ?
	Unit			*m_controller;	//sucks if this will lead to a corrupted pointer. Don't fuck with us kk ?
	int8			m_passengerCount;
	VehicleData		*m_vehicledata;
public:
	Vehicle(Unit *owner);
	~Vehicle();
	void LoadFromDB();
	void LoadStaticPassengers();

//	void OnPrePushToWorld();
//	void OnPushToWorld();
//	void SafeDelete();

	// Set the vehicle's death state
	void SetDeathState(DeathState s);

	/************************************************************************/
	/* Vehicle Information Functions                                        */
	/************************************************************************/
	uint32 GetVehicleID() { return proto->vehicleId; }
	ARCEMU_INLINE void SetVehicleID(uint32 vehicleId) { proto->vehicleId = vehicleId; }

	// Get the maximum amount of passengers possible for this vehicle
	ARCEMU_INLINE int8 GetOccupancyLimit(){ 	return m_seatdata->capacity; }

	// Passengers inside vehicle
	int8 GetPassengerCount() { ASSERT(m_passengerCount <= MAX_PASSENGERS); return m_passengerCount; }
	// Get vehicle controller
	Unit* GetVehicleController() { return m_controller; }
	// Find a passenger inside a particular seat
	Unit* GetPassenger(int8 seat)
	{
		ASSERT(seat < MAX_PASSENGERS);
		return m_passengers[seat];
	}

	const VehicleEntry	*GetVehicleEntry() const { return m_vehicleEntry; }
	const VehicleData	*GetVehicleData()	const { return m_vehicledata; }
	const SeatData		*GetSeatData()	const { return m_seatdata; }
	ARCEMU_INLINE const uint64& GetGUID(){ return m_owner->GetGUID(); }
	ARCEMU_INLINE const WoWGuid& GetNewGUID(){ return m_owner->GetNewGUID(); }
	ARCEMU_INLINE const Unit* GetOwner(){ return m_owner; }

	/************************************************************************/
	/* Vehicle Movement Handler                                             */
	/************************************************************************/
	void MoveVehicle(float x, float y, float z, float o);

	/************************************************************************/
	/* Vehicle Packet Functions                                             */
	/************************************************************************/
//	void RelocateToVehicle(Player * player);
	void SendFarsightPacket(Player * player, bool enabled);
	void SendVehicleActionBar(Player * player,bool if_has_any=true,int8 seat=0);
	void SendRidePacket(Unit * passenger, SeatInfo seat);
//	void SendHeartbeatPacket(Unit *passenger);

	/************************************************************************/
	/* Passenger Control Functions                                          */
	/************************************************************************/
	void EjectAllPassengers();

	// EnterVehicle(Vehicle * vehicle) removes the passenger from its current vehicle and adds it to another vehicle
	void AddPassenger(Unit * passenger, int8 seat = -1);

	// ExitVehicle() removes the passenger from the vehicle
	void RemovePassenger(Unit * passenger);

	// Clears the passenger
//	void ClearPassengers();
	void RemoveFromWorld();

//	Player * m_lastcaster;
	// Is the object being removed from the world
//	bool IsDeleted;
	//GM function !!
	ARCEMU_INLINE void SetSpell(uint32 slot,uint32 spell ) 
	{ 
		if( slot >= MAX_VEHICLE_SPELLS )
			return;
		m_vehicledata->spells[ slot ] = spell;
	}
	bool CanCastSpell( uint32 spellID );
	bool HasAllProtoAvailable();
	bool IsMount;
	CreatureProto	*proto;	//vehicle ID and other stuff that might be usefull for us
	// dbc info
	VehicleEntry * m_vehicleEntry;
	// seat info
	SeatData * m_seatdata;

};

#endif

