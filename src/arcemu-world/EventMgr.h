//#define USE_OLD_EVENT_MANAGER
#ifdef USE_OLD_EVENT_MANAGER
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

#ifndef __EVENTMGR_H
#define __EVENTMGR_H

enum EventTypes
{
	EVENT_UNK = 0,
	EVENT_MAPMGR_UPDATEOBJECTS,
	EVENT_MAPCELL_UNLOAD,
	EVENT_WORLD_UPDATEAUCTIONS,
	EVENT_WORLD_UPDATESESSIONS,
	EVENT_CREATURE_UPDATE,
	EVENT_PLAYER_UPDATE,
	EVENT_PLAYER_UPDATEEXPLORATION,
	EVENT_GAMEOBJECT_UPDATE,
	EVENT_DYNAMICOBJECT_UPDATE,
	EVENT_GAMEOBJECT_ENV_DAMAGE,
	EVENT_PLAYER_STOPPVP,
	EVENT_CREATURE_JUSTDIED,
	EVENT_CREATURE_REMOVE_CORPSE,
	EVENT_CREATURE_RESPAWN,
	EVENT_MAPMGR_UNLOAD_CELL,
	EVENT_PLAYER_REST,
	EVENT_PLAYER_SEND_PACKET,
	EVENT_UNIT_EMOTE,
	EVENT_WORLD_UPDATEQUEUES,
	EVENT_PET_UPDATE,
	EVENT_PLAYER_REDUCEDRUNK,
	EVENT_WEATHER_UPDATE,
	EVENT_TOTEM_EXPIRE,
	EVENT_TOTEM_SEEK_TARGET,
	EVENT_TOTEM_SEEK_TARGET_AREA_AURA,
	EVENT_ENSLAVE_EXPIRE,
	EVENT_PLAYER_TAXI_DISMOUNT,
	EVENT_GAMEOBJECT_DOOR_CLOSE,
	EVENT_PLAYER_ALLOW_TRIGGERPORT,
	EVENT_SPELL_DAMAGE_HIT,
	EVENT_PET_SPELL_COOLDOWN,
	EVENT_GAMEOBJECT_UNHOOK,
	EVENT_GAMEOBJECT_FISH_HOOKED,
	EVENT_GAMEOBJECT_FISH_NOT_HOOKED,
	EVENT_GAMEOBJECT_END_FISHING,
	EVENT_PLAYER_REPEAT_SHOT,
	EVENT_PLAYER_ACTIVATE_GAMEOBJECT,
	EVENT_UNIT_REPEAT_MSG,
	EVENT_UNIT_CHAT_MSG,
	EVENT_PLAYER_MOTD,
	EVENT_TIMED_QUEST_EXPIRE,
	EVENT_UNIT_MANUAL_REPEAT_MSG,
	EVENT_PLAYER_TAXI_POSITION_UPDATE,
	EVENT_PLAYER_TAXI_INTERPOLATE,
	EVENT_PLAYER_CHECKFORCHEATS,
	EVENT_AREAAURA_UPDATE,
	EVENT_AURA_REMOVE,
	EVENT_AURA_PERIODIC_DAMAGE,
	EVENT_AURA_PERIODIC_DAMAGE_PERCENT,
	EVENT_AURA_PERIODIC_HEAL,
	EVENT_AURA_PERIODIC_TRIGGERSPELL,
	EVENT_AURA_PERIODIC_ENERGIZE,
	EVENT_AURA_PERIODIC_LEECH,
	EVENT_AURA_PERIODIC_BURN,
	EVENT_AURA_PERIOCIC_MANA,
	EVENT_AURA_PERIODIC_HEALTH_FUNNEL,
	EVENT_AURA_PERIODIC_UPDATE,
	EVENT_AURA_MANA_FUNNEL,
	EVENT_CANNIBALIZE,
	EVENT_GROUP_POSITION_UPDATE,
	EVNET_RAID_POSITION_UPDATE,
	EVENT_DELETE_TIMER,
	EVENT_UNIT_SENDMOVE,
	EVENT_GAMEOBJECT_ITEM_SPAWN,
	EVENT_GAMEOBJECT_ITEM_DESPAWN,
	EVENT_GAMEOBJECT_EXPIRE,
	EVENT_CREATURE_SAFE_DELETE,
	EVENT_PET_DELAYED_REMOVE,
	EVENT_REMOVE_ENCHANTMENT,
	EVENT_REMOVE_ENCHANTMENT1,
	EVENT_REMOVE_ENCHANTMENT2,
	EVENT_REMOVE_ENCHANTMENT3,
	EVENT_REMOVE_ENCHANTMENT4,
	EVENT_REMOVE_ENCHANTMENT5,
	EVENT_REMOVE_ENCHANTMENT6,
	EVENT_REMOVE_ENCHANTMENT7,
	EVENT_REMOVE_ENCHANTMENT8,
	EVENT_REMOVE_ENCHANTMENT9,
	EVENT_REMOVE_ENCHANTMENT10,
	EVENT_REMOVE_ENCHANTMENT11,
	EVENT_REMOVE_ENCHANTMENT12,
	EVENT_REMOVE_ENCHANTMENT13,
	EVENT_REMOVE_ENCHANTMENT14,
	EVENT_REMOVE_ENCHANTMENT15,
	EVENT_REMOVE_ENCHANTMENT16,
	EVENT_REMOVE_ENCHANTMENT17,
	EVENT_REMOVE_ENCHANTMENT18,
	EVENT_REMOVE_ENCHANTMENT19,
	EVENT_REMOVE_ENCHANTMENT20,
	EVENT_REMOVE_ENCHANTMENT21,
	EVENT_PLAYER_CHARM_ATTACK,
	EVENT_PLAYER_KICK,
	EVENT_TRANSPORTER_NEXT_WAYPOINT,
	EVENT_LOOT_ROLL_FINALIZE,
	EVENT_FIELD_UPDATE_EXPIRE,
	EVENT_SUMMON_PET_EXPIRE,
	EVENT_CORPSE_DESPAWN,
	EVENT_SCRIPT_UPDATE_EVENT,
	EVENT_INSTANCE_SOFT_RESET,
	EVENT_INSTANCE_HARD_RESET,
	EVENT_INSTANCE_LIMIT_COUNTER,
	EVENT_MAP_SPAWN_EVENTS,
	EVENT_PLAYER_DUEL_COUNTDOWN,
	EVENT_PLAYER_DUEL_BOUNDARY_CHECK,
	EVENT_GAMEOBJECT_TRAP_SEARCH_TARGET,
	EVENT_PLAYER_TELEPORT,
	EVENT_UNIT_DIMINISHING_RETURN,
	EVENT_UNIT_UNROOT,
	EVENT_MAILSYSTEM_RELOAD,
	EVENT_CREATURE_FORMATION_LINKUP,
	EVENT_CREATURE_CHANNEL_LINKUP,
	EVENT_AURA_REGEN_MANA_STAT_PCT,
	EVENT_AURA_PERIODIC_HEALINCOMB,
	EVENT_AURA_REGEN,
	EVENT_AURA_PERIODIC_REGEN,
	EVENT_AURA_PERIODIC_HEALPERC,
	EVENT_ATTACK_TIMEOUT,		//Zack 2007 05 05: if no action is taken then combat should be exited after 5 seconds.
	EVENT_SUMMON_EXPIRE,		//Zack 2007 05 28: similar to pet expire but we can have multiple guardians
	EVENT_MUTE_PLAYER,			//Zack 2007 06 05: player gains his voice back
	EVENT_PLAYER_FORECED_RESURECT,		//Zack 2007 06 08: After player not pushing release spirit for 6 minutes while dead
	EVENT_PLAYER_SOFT_DISCONNECT,		//Zack 2007 06 12: Kick AFK players to not eat resources
	EVENT_BATTLEGROUND_WSG_AUTO_RETURN_FLAG,
	EVENT_BATTLEGROUND_WSG_AUTO_RETURN_FLAG_1,
	EVENT_CORPSE_SPAWN_BONES,
	EVENT_DODGE_BLOCK_FLAG_EXPIRE,	//yeah, there are more then 1 flags
	EVENT_REJUVENATION_FLAG_EXPIRE,
	EVENT_PARRY_FLAG_EXPIRE,		
	EVENT_CRIT_FLAG_EXPIRE,		
	EVENT_LASTKILLWITHHONOR_FLAG_EXPIRE,
	EVENT_GMSCRIPT_EVENT,
	EVENT_RELOCATE,
	EVENT_BATTLEGROUND_QUEUE_UPDATE,
	EVENT_BATTLEGROUND_SPAWN_BUFF,
	EVENT_BATTLEGROUND_COUNTDOWN,
	EVENT_BATTLEGROUND_CLOSE,
	EVENT_BATTLEGROUND_RESOURCEUPDATE,
	EVENT_PLAYER_EJECT_FROM_INSTANCE,
	EVENT_AB_RESOURCES_UPDATE_TEAM_0,
	EVENT_AB_RESOURCES_UPDATE_TEAM_1,
	EVENT_AB_CAPTURE_CP_1,
	EVENT_AB_CAPTURE_CP_2,
	EVENT_AB_CAPTURE_CP_3,
	EVENT_AB_CAPTURE_CP_4,
	EVENT_AB_CAPTURE_CP_5,
	EVENT_AB_RESPAWN_BUFF,
	EVENT_AV_ADD_REINFORCEMENTS_COLDTOOTH,
	EVENT_AV_ADD_REINFORCEMENTS_IRONDEEP,
	EVENT_AV_CAPTURE_CP_0,
	EVENT_AV_CAPTURE_CP_1,
	EVENT_AV_CAPTURE_CP_2,
	EVENT_AV_CAPTURE_CP_3,
	EVENT_AV_CAPTURE_CP_4,
	EVENT_AV_CAPTURE_CP_5,
	EVENT_AV_CAPTURE_CP_6,
	EVENT_AV_CAPTURE_CP_7,
	EVENT_AV_CAPTURE_CP_8,
	EVENT_AV_CAPTURE_CP_9,
	EVENT_AV_CAPTURE_CP_10,
	EVENT_AV_CAPTURE_CP_11,
	EVENT_AV_CAPTURE_CP_12,
	EVENT_AV_CAPTURE_CP_13,
	EVENT_AV_CAPTURE_CP_14,
	EVENT_AV_CAPTURE_CP_15,
	EVENT_AV_CAPTURE_CP_16,
	EVENT_COMBO_POINT_CLEAR_FOR_TARGET,
	EVENT_ITEM_UPDATE,
	EVENT_PLAYER_JOIN_INSTANCE,
	EVENT_PLAYER_FLUSH_MOVEMENT,
	EVENT_EOTS_GIVE_POINTS,
	EVENT_EOTS_CHECK_CAPTURE_POINT_STATUS,
	EVENT_EOTS_RESET_FLAG,
	EVENT_PLAYER_RESET_HEARTBEAT,
	EVENT_COMBAT_TIMER,
	EVENT_CREATURE_AISPELL,
	EVENT_UNIT_DELAYED_SPELL_CANCEL,
	EVENT_AURA_PERIODIC_ENERGIZE_VARIABLE, // Aspect of the Viper
	EVENT_ARENA_SHADOW_SIGHT,
	EVENT_SEND_AURAS_TO_PLAYER,
	EVENT_SEND_ACHIEVEMNTS_TO_PLAYER,
	EVENT_SEND_PACKET_TO_PLAYER_AFTER_LOGIN,
	EVENT_SEND_PACKET_TO_PLAYER_DELAYED,
	EVENT_RESET_MISSDIRECTIONTARGET,
	EVENT_DELETE_SESSION,
	EVENT_KILLING_SPREE_PROC,
	EVENT_PERIODIC_DUMMY_AURA_TRIGGER,
	EVENT_DELAYED_SPELLCAST,
};

enum EventFlags
{
	EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT = 0x1,
	EVENT_FLAG_DELETES_OBJECT				   = 0x2,
};

struct SERVER_DECL TimedEvent
{
public:
	TimedEvent(void* object, CallbackBase* callback, uint32 type, time_t time, uint32 repeat, uint32 flags) : 
		obj(object), cb(callback), eventType(type), eventFlag(flags), msTime(time), currTime(time), repeats(repeat), deleted(false),ref(0) {}
		
	void *obj;
	CallbackBase *cb;
	uint32 eventType;
	uint16 eventFlag;
	time_t msTime;
	time_t currTime;
	uint16 repeats;
	bool deleted;
	int instanceId;
	volatile long ref;
//	Mutex	LockedDelete;

	static TimedEvent * Allocate(void* object, CallbackBase* callback, uint32 flags, time_t time, uint32 repeat);
//private:
#if (defined( WIN32 ) || defined( WIN64 ) )
	// theoretically ref should never get to be 2 times 0 even if there is thread concurency. What if interlocked decrement fails ?
	void DecRef()
	{
//		LockedDelete.Acquire();
		InterlockedDecrement(&ref); 
		if(ref == 0 && cb) //there are some chances that we actually might miss this "if"
		{
			delete cb;
			cb = NULL;
//			LockedDelete.Release();
			delete this;
			return;
		}
//		LockedDelete.Release();
	}

	void IncRef() { InterlockedIncrement(&ref); }
#else

	/* burlex: if anyone knows how to do the equivilent of InterlockedIncrement/Decrement on linux feel free
	   to change this, I couldn't find the atomic functions anymore though :*( */

	void IncRef() { ++ref; }
    
	void DecRef()
	{
		--ref;
		if(ref <= 0)
		{
			 delete cb;
			 delete this;
		}
	}
#endif

};

class EventMgr;
class EventableObjectHolder;
typedef map<int32, EventableObjectHolder*> HolderMap;

class SERVER_DECL EventMgr : public Singleton < EventMgr >
{
	friend class MiniEventMgr;
public:
	template <class Class>
		void AddEvent(Class *obj, void (Class::*method)(), uint32 type, uint32 time, uint32 repeats, uint32 flags)
	{
		// create a timed event
		TimedEvent * event = new TimedEvent(obj, new CallbackP0<Class>(obj, method), type, time, repeats, flags);

		// add this to the object's list, updating will all be done later on...
		obj->event_AddEvent(event);
	}

	template <class Class, typename P1>
		void AddEvent(Class *obj, void (Class::*method)(P1), P1 p1, uint32 type, uint32 time, uint32 repeats, uint32 flags)
	{
		// create a timed event
		TimedEvent * event = new TimedEvent(obj, new CallbackP1<Class, P1>(obj, method, p1), type, time, repeats, flags);

		// add this to the object's list, updating will all be done later on...
		obj->event_AddEvent(event);
	}

	template <class Class, typename P1, typename P2>
		void AddEvent(Class *obj, void (Class::*method)(P1,P2), P1 p1, P2 p2, uint32 type, uint32 time, uint32 repeats, uint32 flags)
	{
		// create a timed event
		TimedEvent * event = new TimedEvent(obj, new CallbackP2<Class, P1, P2>(obj, method, p1, p2), type, time, repeats, flags);

		// add this to the object's list, updating will all be done later on...
		obj->event_AddEvent(event);
	}

	template <class Class, typename P1, typename P2, typename P3>
		void AddEvent(Class *obj,void (Class::*method)(P1,P2,P3), P1 p1, P2 p2, P3 p3, uint32 type, uint32 time, uint32 repeats, uint32 flags)
	{
		// create a timed event
		TimedEvent * event = new TimedEvent(obj, new CallbackP3<Class, P1, P2, P3>(obj, method, p1, p2, p3), type, time, repeats, flags);

		// add this to the object's list, updating will all be done later on...
		obj->event_AddEvent(event);
	}

	template <class Class, typename P1, typename P2, typename P3, typename P4>
		void AddEvent(Class *obj, void (Class::*method)(P1,P2,P3,P4), P1 p1, P2 p2, P3 p3, P4 p4, uint32 type, uint32 time, uint32 repeats, uint32 flags)
	{
		// create a timed event
		TimedEvent * event = new TimedEvent(obj, new CallbackP4<Class, P1, P2, P3, P4>(obj, method, p1, p2, p3, p4), type, time, repeats, flags);

		// add this to the object's list, updating will all be done later on...
		obj->event_AddEvent(event);
	}

	template <class Class> void RemoveEvents(Class *obj) { obj->event_RemoveEvents((uint32)(-1)); }
	template <class Class> void RemoveEvents(Class *obj, int32 type)
	{
		obj->event_RemoveEvents(type);
	}

	template <class Class> void ModifyEventTimeLeft(Class *obj, uint32 type, uint32 time,bool unconditioned=true)
	{
		obj->event_ModifyTimeLeft(type, time,unconditioned);
	}

	template <class Class> void ModifyEventRepeatInterval(Class *obj, uint32 type, uint32 time)
	{
		obj->event_ModifyRepeatInterval(type, time);
	}

	template <class Class> void ModifyEventTime(Class *obj, uint32 type, uint32 time)
	{
		obj->event_ModifyTime(type, time);
	}

	template <class Class> void DelayEventTrigger(Class *obj, uint32 type, uint32 time)
	{
		obj->event_DelayTrigger(type, time);
	}

	template <class Class> bool HasEvent(Class *obj, uint32 type)
	{
		return obj->event_HasEvent(type);
	}

	EventableObjectHolder * GetEventHolder(int32 InstanceId)
	{
		HolderMap::iterator itr = mHolders.find(InstanceId);
		if(itr == mHolders.end()) return 0;
		return itr->second;
	}

	void AddEventHolder(EventableObjectHolder * holder, int32 InstanceId)
	{
		holderLock.Acquire();
		mHolders.insert( HolderMap::value_type( InstanceId, holder) );
		holderLock.Release();
	}

	void RemoveEventHolder(int32 InstanceId)
	{
		holderLock.Acquire();
		mHolders.erase(InstanceId);
		holderLock.Release();
	}

	void RemoveEventHolder(EventableObjectHolder * holder)
	{
		holderLock.Acquire();
		HolderMap::iterator itr = mHolders.begin();
		for(; itr != mHolders.end(); ++itr)
		{
			if(itr->second == holder)
			{
				mHolders.erase(itr);
				holderLock.Release();
				return;
			}
		}
		holderLock.Release();
	}

protected:
	HolderMap mHolders;
	Mutex holderLock;
};

#define sEventMgr EventMgr::getSingleton()

#endif
#endif

#ifndef USE_OLD_EVENT_MANAGER
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

#ifndef __EVENTMGR_H
#define __EVENTMGR_H

enum EventTypes
{
	EVENT_UNK = 0,
	EVENT_WORLD_UPDATEAUCTIONS,
	EVENT_PLAYER_UPDATE,
	EVENT_DYNAMICOBJECT_UPDATE,
	EVENT_CREATURE_REMOVE_CORPSE,
	EVENT_CREATURE_RESPAWN,
	EVENT_PLAYER_REST,
	EVENT_UNIT_EMOTE,
	EVENT_PLAYER_REDUCEDRUNK,
	EVENT_WEATHER_UPDATE,
	EVENT_TOTEM_EXPIRE,
	EVENT_PLAYER_TAXI_DISMOUNT,
	EVENT_GAMEOBJECT_DOOR_CLOSE,
	EVENT_SPELL_DAMAGE_HIT,
	EVENT_GAMEOBJECT_FISH_HOOKED,
	EVENT_GAMEOBJECT_END_FISHING,
	EVENT_TIMED_QUEST_EXPIRE,
	EVENT_PLAYER_TAXI_INTERPOLATE,
	EVENT_PLAYER_CHECKFORCHEATS,
	EVENT_AREAAURA_UPDATE,
	EVENT_AURA_REMOVE,
	EVENT_AURA_PERIODIC_DAMAGE,
	EVENT_AURA_PERIODIC_DAMAGE_PERCENT,
	EVENT_AURA_PERIODIC_HEAL,
	EVENT_AURA_PERIODIC_TRIGGERSPELL,
	EVENT_AURA_PERIODIC_ENERGIZE,
	EVENT_AURA_PERIODIC_LEECH,
	EVENT_AURA_PERIODIC_BURN,
	EVENT_AURA_PERIOCIC_MANA,
	EVENT_AURA_PERIODIC_HEALTH_FUNNEL,
	EVENT_AURA_PERIODIC_UPDATE,
	EVENT_CANNIBALIZE,
	EVENT_DELETE_TIMER,
	EVENT_UNIT_SENDMOVE,
	EVENT_GAMEOBJECT_ITEM_SPAWN,
	EVENT_GAMEOBJECT_EXPIRE,
	EVENT_CREATURE_SAFE_DELETE,
	EVENT_PET_DELAYED_REMOVE,
	EVENT_REMOVE_ENCHANTMENT,
	EVENT_REMOVE_ENCHANTMENT1,
	EVENT_REMOVE_ENCHANTMENT2,
	EVENT_REMOVE_ENCHANTMENT3,
	EVENT_REMOVE_ENCHANTMENT4,
	EVENT_REMOVE_ENCHANTMENT5,
	EVENT_REMOVE_ENCHANTMENT6,
	EVENT_REMOVE_ENCHANTMENT7,
	EVENT_REMOVE_ENCHANTMENT8,
	EVENT_REMOVE_ENCHANTMENT9,
	EVENT_REMOVE_ENCHANTMENT10,
	EVENT_REMOVE_ENCHANTMENT11,
	EVENT_REMOVE_ENCHANTMENT12,
	EVENT_REMOVE_ENCHANTMENT13,
	EVENT_REMOVE_ENCHANTMENT14,
	EVENT_REMOVE_ENCHANTMENT15,
	EVENT_REMOVE_ENCHANTMENT16,
	EVENT_REMOVE_ENCHANTMENT17,
	EVENT_REMOVE_ENCHANTMENT18,
	EVENT_REMOVE_ENCHANTMENT19,
	EVENT_REMOVE_ENCHANTMENT20,
	EVENT_REMOVE_ENCHANTMENT21,
	EVENT_REMOVE_ENCHANTMENT22,
	EVENT_REMOVE_ENCHANTMENT23,
	EVENT_REMOVE_ENCHANTMENT24,
	EVENT_REMOVE_ENCHANTMENT25,
	EVENT_PLAYER_CHARM_ATTACK,
	EVENT_PLAYER_KICK,
	EVENT_TRANSPORTER_NEXT_WAYPOINT,
	EVENT_LOOT_ROLL_FINALIZE,
	EVENT_CORPSE_DESPAWN,
	EVENT_SCRIPT_UPDATE_EVENT,
	EVENT_PLAYER_DUEL_COUNTDOWN,
	EVENT_PLAYER_DUEL_BOUNDARY_CHECK,
	EVENT_GAMEOBJECT_TRAP_SEARCH_TARGET,
	EVENT_PLAYER_TELEPORT,
	EVENT_CREATURE_FORMATION_LINKUP,
	EVENT_CREATURE_CHANNEL_LINKUP,
	EVENT_AURA_REGEN_MANA_STAT_PCT,
	EVENT_AURA_PERIODIC_HEALINCOMB,
	EVENT_AURA_REGEN,
	EVENT_AURA_PERIODIC_REGEN,
	EVENT_AURA_PERIODIC_HEALPERC,
	EVENT_ATTACK_TIMEOUT,		//Zack 2007 05 05: if no action is taken then combat should be exited after 5 seconds.
	EVENT_SUMMON_EXPIRE,		//Zack 2007 05 28: similar to pet expire but we can have multiple guardians
	EVENT_PLAYER_FORECED_RESURECT,		//Zack 2007 06 08: After player not pushing release spirit for 6 minutes while dead
	EVENT_PLAYER_SOFT_DISCONNECT,		//Zack 2007 06 12: Kick AFK players to not eat resources
	EVENT_BATTLEGROUND_WSG_AUTO_RETURN_FLAG,
	EVENT_BATTLEGROUND_WSG_AUTO_RETURN_FLAG_1,
	EVENT_CORPSE_SPAWN_BONES,
	EVENT_DODGE_BLOCK_FLAG_EXPIRE,	//yeah, there are more then 1 flags
	EVENT_REJUVENATION_FLAG_EXPIRE,
	EVENT_PARRY_FLAG_EXPIRE,		
	EVENT_CRIT_FLAG_EXPIRE,		
	EVENT_LASTKILLWITHHONOR_FLAG_EXPIRE,
	EVENT_GMSCRIPT_EVENT,
	EVENT_RELOCATE,
	EVENT_BATTLEGROUND_QUEUE_UPDATE,
	EVENT_BATTLEGROUND_SPAWN_BUFF,
	EVENT_BATTLEGROUND_COUNTDOWN,
	EVENT_BATTLEGROUND_REMAINING_TIME,
	EVENT_BATTLEGROUND_CLOSE,
	EVENT_BATTLEGROUND_RESOURCEUPDATE,
	EVENT_PLAYER_EJECT_FROM_INSTANCE,
	EVENT_AB_RESOURCES_UPDATE_TEAM_0,
	EVENT_AB_RESOURCES_UPDATE_TEAM_1,
	EVENT_AB_CAPTURE_CP_1,
	EVENT_AB_CAPTURE_CP_2,
	EVENT_AB_CAPTURE_CP_3,
	EVENT_AB_CAPTURE_CP_4,
	EVENT_AB_CAPTURE_CP_5,
	EVENT_AB_RESPAWN_BUFF,
	EVENT_AV_ADD_REINFORCEMENTS_COLDTOOTH,
	EVENT_AV_ADD_REINFORCEMENTS_IRONDEEP,
	EVENT_AV_CAPTURE_CP_0,
	EVENT_AV_CAPTURE_CP_1,
	EVENT_AV_CAPTURE_CP_2,
	EVENT_AV_CAPTURE_CP_3,
	EVENT_AV_CAPTURE_CP_4,
	EVENT_AV_CAPTURE_CP_5,
	EVENT_AV_CAPTURE_CP_6,
	EVENT_AV_CAPTURE_CP_7,
	EVENT_AV_CAPTURE_CP_8,
	EVENT_AV_CAPTURE_CP_9,
	EVENT_AV_CAPTURE_CP_10,
	EVENT_AV_CAPTURE_CP_11,
	EVENT_AV_CAPTURE_CP_12,
	EVENT_AV_CAPTURE_CP_13,
	EVENT_AV_CAPTURE_CP_14,
	EVENT_AV_CAPTURE_CP_15,
	EVENT_AV_CAPTURE_CP_16,
	EVENT_COMBO_POINT_CLEAR_FOR_TARGET,
	EVENT_ITEM_UPDATE,
	EVENT_PLAYER_JOIN_INSTANCE,
	EVENT_PLAYER_FLUSH_MOVEMENT,
	EVENT_EOTS_GIVE_POINTS,
	EVENT_EOTS_CHECK_CAPTURE_POINT_STATUS,
	EVENT_EOTS_RESET_FLAG,
	EVENT_PLAYER_RESET_HEARTBEAT,
	EVENT_COMBAT_TIMER,
	EVENT_CREATURE_AISPELL,
	EVENT_UNIT_DELAYED_SPELL_CANCEL,
	EVENT_AURA_PERIODIC_ENERGIZE_VARIABLE, // Aspect of the Viper
	EVENT_ARENA_SHADOW_SIGHT,
	EVENT_SEND_AURAS_TO_PLAYER,
	EVENT_SEND_ACHIEVEMNTS_TO_PLAYER,
	EVENT_SEND_PACKET_TO_PLAYER_AFTER_LOGIN,
	EVENT_SEND_PACKET_TO_PLAYER_DELAYED,
	EVENT_RESET_MISSDIRECTIONTARGET,
	EVENT_DELETE_SESSION,
	EVENT_KILLING_SPREE_PROC,
	EVENT_UNIT_CHAT_MSG,
	EVENT_GAMEOBJECT_UPDATE,
	EVENT_CREATURE_UPDATE,
	EVENT_SUMMON_PET_EXPIRE,
	EVENT_PLAYER_SEND_PACKET,
	EVENT_PERIODIC_DUMMY_AURA_TRIGGER,
	EVENT_DELAYED_SPELLCAST,
	EVENT_RB_REFRESH_LIST,
	EVENT_PLAYER_DELAYED_UPDATE,		//required in 4.3 since client looses some updates. Used to resync importnat values
	EVENT_PLAYER_DELAYED_BG_RES_QUEUE,	//stupid update packets getting out of order and making player get in half way dead state
	EVENT_CLIENT_FINISHED_LOADING_MAP,
	EVENT_BFG_RESOURCES_UPDATE_TEAM_0,
	EVENT_BFG_RESOURCES_UPDATE_TEAM_1,
	EVENT_BFG_CAPTURE_CP_1,
	EVENT_BFG_CAPTURE_CP_2,
	EVENT_BFG_CAPTURE_CP_3,
	EVENT_BFG_CAPTURE_CP_4,
	EVENT_BFG_CAPTURE_CP_5,
	EVENT_BFG_RESPAWN_BUFF,
	EVENT_PLAYER_DELAYED_CAST0,
	EVENT_PLAYER_DELAYED_CAST1,
	EVENT_PLAYER_DELAYED_CAST2,
	EVENT_PLAYER_DELAYED_CAST3,
	EVENT_PLAYER_DELAYED_CAST4,
	EVENT_COOLDOWN_UPDATE,
	NUM_EVENT_TYPES,
};

enum EventFlags
{
	EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT = 0x1,
	EVENT_FLAG_DELETES_OBJECT				   = 0x2,	//make sure to have trigger ammount 1 for this
	EVENT_FLAG_TICK_ON_NEXT_UPDATE			   = 0x4,	//instead waiting for trigger interval, tick instantly on next update unless it is a one time tick
	EVENT_FLAG_UNIQUE_EVENT_DELETE_NEWER	   = 0x8,	// do not queue multiple descrtructor / delete calls
};

struct SERVER_DECL TimedEvent
{
public:
	TimedEvent(void* object, CallbackBase* callback, uint32 type, uint32 TriggerDelay, uint32 repeat, uint32 flags);
		
	void *obj;
	CallbackBase *cb;
	uint32 eventType;
	uint16 eventFlag;
//	time_t msTime;
//	time_t currTime;
	uint32 TriggerAtStamp;
	uint32 TriggerTimeInterval;
	uint16 repeats;
	uint8  TE_deleted;
	int instanceId;
	volatile long ref;

	static TimedEvent * Allocate(void* object, CallbackBase* callback, uint32 flags, uint32 time, uint32 repeat);
//private:
#if (defined( WIN32 ) || defined( WIN64 ) )
	//you can get this fucked up if you first remove from a list then add it to another (dec=delete the add=incref)
	//but we lock the lists before add so this should not happen ?
	void DecRef()
	{
		if(InterlockedDecrement(&ref) == 0 && cb) //there are some chances that we actually might miss this "if"
		{
			delete cb;
			cb = NULL;
			delete this;
			return;
		}
	}

	void IncRef() { InterlockedIncrement(&ref); }
#else

	/* burlex: if anyone knows how to do the equivilent of InterlockedIncrement/Decrement on linux feel free
	   to change this, I couldn't find the atomic functions anymore though :*( */

	void IncRef() { ++ref; }
    
	void DecRef()
	{
		--ref;
		if(ref <= 0)
		{
			 delete cb;
			 delete this;
		}
	}
#endif

};

class EventMgr;
class EventableObjectHolder;
typedef map<int32, EventableObjectHolder*> HolderMap;

class SERVER_DECL EventMgr : public Singleton < EventMgr >
{
	friend class MiniEventMgr;
public:
	template <class Class>
		void AddEvent(Class *obj, void (Class::*method)(), uint32 type, uint32 time, uint32 repeats, uint32 flags)
	{
		// create a timed event
		TimedEvent * event = new TimedEvent(obj, new CallbackP0<Class>(obj, method), type, time, repeats, flags);

		// add this to the object's list, updating will all be done later on...
		obj->event_AddEvent(event);
	}

	template <class Class, typename P1>
		void AddEvent(Class *obj, void (Class::*method)(P1), P1 p1, uint32 type, uint32 time, uint32 repeats, uint32 flags)
	{
		// create a timed event
		TimedEvent * event = new TimedEvent(obj, new CallbackP1<Class, P1>(obj, method, p1), type, time, repeats, flags);

		// add this to the object's list, updating will all be done later on...
		obj->event_AddEvent(event);
	}

	template <class Class, typename P1, typename P2>
		void AddEvent(Class *obj, void (Class::*method)(P1,P2), P1 p1, P2 p2, uint32 type, uint32 time, uint32 repeats, uint32 flags)
	{
		// create a timed event
		TimedEvent * event = new TimedEvent(obj, new CallbackP2<Class, P1, P2>(obj, method, p1, p2), type, time, repeats, flags);

		// add this to the object's list, updating will all be done later on...
		obj->event_AddEvent(event);
	}

	template <class Class, typename P1, typename P2, typename P3>
		void AddEvent(Class *obj,void (Class::*method)(P1,P2,P3), P1 p1, P2 p2, P3 p3, uint32 type, uint32 time, uint32 repeats, uint32 flags)
	{
		// create a timed event
		TimedEvent * event = new TimedEvent(obj, new CallbackP3<Class, P1, P2, P3>(obj, method, p1, p2, p3), type, time, repeats, flags);

		// add this to the object's list, updating will all be done later on...
		obj->event_AddEvent(event);
	}

	template <class Class, typename P1, typename P2, typename P3, typename P4>
		void AddEvent(Class *obj, void (Class::*method)(P1,P2,P3,P4), P1 p1, P2 p2, P3 p3, P4 p4, uint32 type, uint32 time, uint32 repeats, uint32 flags)
	{
		// create a timed event
		TimedEvent * event = new TimedEvent(obj, new CallbackP4<Class, P1, P2, P3, P4>(obj, method, p1, p2, p3, p4), type, time, repeats, flags);

		// add this to the object's list, updating will all be done later on...
		obj->event_AddEvent(event);
	}

	template <class Class, typename P1, typename P2, typename P3, typename P4, typename P5>
		void AddEvent(Class *obj, void (Class::*method)(P1,P2,P3,P4,P5), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, uint32 type, uint32 time, uint32 repeats, uint32 flags)
	{
		// create a timed event
		TimedEvent * event = new TimedEvent(obj, new CallbackP5<Class, P1, P2, P3, P4, P5>(obj, method, p1, p2, p3, p4, p5), type, time, repeats, flags);

		// add this to the object's list, updating will all be done later on...
		obj->event_AddEvent(event);
	}

	template <class Class, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
		void AddEvent(Class *obj, void (Class::*method)(P1,P2,P3,P4,P5,P6), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, uint32 type, uint32 time, uint32 repeats, uint32 flags)
	{
		// create a timed event
		TimedEvent * event = new TimedEvent(obj, new CallbackP6<Class, P1, P2, P3, P4, P5, P6>(obj, method, p1, p2, p3, p4, p5, p6), type, time, repeats, flags);

		// add this to the object's list, updating will all be done later on...
		obj->event_AddEvent(event);
	}

	template <class Class, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
		void AddEvent(Class *obj, void (Class::*method)(P1,P2,P3,P4,P5,P6,P7), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, uint32 type, uint32 time, uint32 repeats, uint32 flags)
	{
		// create a timed event
		TimedEvent * event = new TimedEvent(obj, new CallbackP7<Class, P1, P2, P3, P4, P5, P6, P7>(obj, method, p1, p2, p3, p4, p5, p6, p7), type, time, repeats, flags);

		// add this to the object's list, updating will all be done later on...
		obj->event_AddEvent(event);
	}

	template <class Class> void RemoveEvents(Class *obj) { obj->event_RemoveEvents(); }
	template <class Class> void RemoveEvents(Class *obj, int32 type)
	{
		obj->event_RemoveEvents(type);
	}

	//Can only increse time when it will get triggered(Due to multi event lists). In case you want to reduce it you should remove and readd the events
	template <class Class> bool ModifyEventTimeLeft(Class *obj, uint32 type, uint32 TimeUntilNextTrigger)
	{
		return obj->event_ModifyTimeLeft(type, TimeUntilNextTrigger);
	}

	template <class Class> void ModifyEventRepeatInterval(Class *obj, uint32 type, uint32 time)
	{
		obj->event_ModifyRepeatInterval(type, time);
	}

	template <class Class> void DelayEventTrigger(Class *obj, uint32 type, uint32 time)
	{
		obj->event_DelayTrigger(type, time);
	}

	template <class Class> bool HasEvent(Class *obj, uint32 type)
	{
		return obj->event_HasEvent(type);
	}
	template <class Class> uint32 GetTimeLeft(Class *obj, uint32 type)
	{
		return obj->event_GetTimeLeft(type);
	}

	EventableObjectHolder * GetEventHolder(int32 InstanceId)
	{
		HolderMap::iterator itr = mHolders.find(InstanceId);
		if(itr == mHolders.end()) return 0;
		return itr->second;
	}

	void AddEventHolder(EventableObjectHolder * holder, int32 InstanceId)
	{
		holderLock.Acquire();
		mHolders.insert( HolderMap::value_type( InstanceId, holder) );
		holderLock.Release();
	}

	void RemoveEventHolder(int32 InstanceId)
	{
		holderLock.Acquire();
		mHolders.erase(InstanceId);
		holderLock.Release();
	}

	void RemoveEventHolder(EventableObjectHolder * holder)
	{
		holderLock.Acquire();
		HolderMap::iterator itr = mHolders.begin();
		for(; itr != mHolders.end(); ++itr)
		{
			if(itr->second == holder)
			{
				mHolders.erase(itr);
				holderLock.Release();
				return;
			}
		}
		holderLock.Release();
	}

protected:
	HolderMap mHolders;
	Mutex holderLock;
};

#define sEventMgr EventMgr::getSingleton()

#endif
#endif