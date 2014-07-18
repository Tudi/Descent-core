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
//safe way to handle arrays in debug mode

enum LfgType
{
    LFG_TYPE_DUNGEON = 1,
    LFG_TYPE_RAID    = 2,
    LFG_TYPE_QUEST   = 3,
    LFG_TYPE_ZONE    = 4,
    LFG_TYPE_HEROIC  = 5,
    LFG_TYPE_RANDOM  = 6,
};

enum LfgStatusCode
{
    LFG_LOCKSTATUS_OK                        = 0,           // Internal use only
    LFG_LOCKSTATUS_INSUFFICIENT_EXPANSION    = 1,
    LFG_LOCKSTATUS_TOO_LOW_LEVEL             = 2,
    LFG_LOCKSTATUS_TOO_HIGH_LEVEL            = 3,
    LFG_LOCKSTATUS_TOO_LOW_GEAR_SCORE        = 4,
    LFG_LOCKSTATUS_TOO_HIGH_GEAR_SCORE       = 5,
    LFG_LOCKSTATUS_RAID_LOCKED               = 6,
    LFG_LOCKSTATUS_ATTUNEMENT_TOO_LOW_LEVEL  = 1001,
    LFG_LOCKSTATUS_ATTUNEMENT_TOO_HIGH_LEVEL = 1002,
    LFG_LOCKSTATUS_QUEST_NOT_COMPLETED       = 1022,
    LFG_LOCKSTATUS_MISSING_ITEM              = 1025,
    LFG_LOCKSTATUS_NOT_IN_SEASON             = 1031,
};

enum LfgUpdateType
{
    LFG_UPDATETYPE_LEADER               = 1,
    LFG_UPDATETYPE_ROLECHECK_ABORTED    = 4,
    LFG_UPDATETYPE_JOIN_PROPOSAL        = 5,
    LFG_UPDATETYPE_ROLECHECK_FAILED     = 6,
    LFG_UPDATETYPE_REMOVED_FROM_QUEUE   = 7,
    LFG_UPDATETYPE_PROPOSAL_FAILED      = 8,
    LFG_UPDATETYPE_PROPOSAL_DECLINED    = 9,
    LFG_UPDATETYPE_GROUP_FOUND          = 10,
    LFG_UPDATETYPE_ADDED_TO_QUEUE       = 12,
    LFG_UPDATETYPE_PROPOSAL_BEGIN       = 13,
    LFG_UPDATETYPE_CLEAR_LOCK_LIST      = 14,
    LFG_UPDATETYPE_GROUP_MEMBER_OFFLINE = 15,
    LFG_UPDATETYPE_GROUP_DISBAND        = 16,
}; 

enum LfgJoinResult
{
    LFG_JOIN_OK                    = 0,                     // Joined (no client msg)
    LFG_JOIN_FAILED                = 1,                     // RoleCheck Failed
    LFG_JOIN_GROUPFULL             = 2,                     // Your group is full
    LFG_JOIN_UNK3                  = 3,                     // No client reaction
    LFG_JOIN_INTERNAL_ERROR        = 4,                     // Internal LFG Error
    LFG_JOIN_NOT_MEET_REQS         = 5,                     // You do not meet the requirements for the chosen dungeons
    LFG_JOIN_PARTY_NOT_MEET_REQS   = 6,                     // One or more party members do not meet the requirements for the chosen dungeons
    LFG_JOIN_MIXED_RAID_DUNGEON    = 7,                     // You cannot mix dungeons, raids, and random when picking dungeons
    LFG_JOIN_MULTI_REALM           = 8,                     // The dungeon you chose does not support players from multiple realms
    LFG_JOIN_DISCONNECTED          = 9,                     // One or more party members are pending invites or disconnected
    LFG_JOIN_PARTY_INFO_FAILED     = 10,                    // Could not retrieve information about some party members
    LFG_JOIN_DUNGEON_INVALID       = 11,                    // One or more dungeons was not valid
    LFG_JOIN_DESERTER              = 12,                    // You can not queue for dungeons until your deserter debuff wears off
    LFG_JOIN_PARTY_DESERTER        = 13,                    // One or more party members has a deserter debuff
    LFG_JOIN_RANDOM_COOLDOWN       = 14,                    // You can not queue for random dungeons while on random dungeon cooldown
    LFG_JOIN_PARTY_RANDOM_COOLDOWN = 15,                    // One or more party members are on random dungeon cooldown
    LFG_JOIN_TOO_MUCH_MEMBERS      = 16,                    // You can not enter dungeons with more that 5 party members
    LFG_JOIN_USING_BG_SYSTEM       = 17,                    // You can not use the dungeon system while in BG or arenas
    LFG_JOIN_FAILED2               = 18,                    // RoleCheck Failed
}; 

enum LfgRoles
{
    ROLE_NONE   = 0x00,
    ROLE_LEADER = 0x01,
    ROLE_TANK   = 0x02,
    ROLE_HEALER = 0x04,
    ROLE_DAMAGE = 0x08,
}; 

//used player side to store the list of maps he dreams to join ( succker )
struct LFG_store
{
	uint8	type;	//type of the map player is looking for
	uint32	map_id;	
};

#define RB_LIST_REFRESH_INTERVAL	30000	//client refresh button seems to be broken. we register an event

class LfgMatch;
class LfgMgr : public Singleton < LfgMgr >, EventableObject
{
public:	
	
	LfgMgr();
	~LfgMgr();

	int32 event_GetInstanceId() { return -1; }

	//start advertising ourself as a player that wishes to join a group
	void LFGDungeonJoin( Player *plr );
	//stop advertising ourself. We should not receive any more messages from others
	void LFGDungeonLeave( Player *plr );
	//we cannot accept invitations unless we are advertising ourself
	bool LFGIsPlayerLFG( uint64 PlayerGUID )
	{
		return ( m_lookingForDungeonPlayerGUIDs.find( PlayerGUID ) != m_lookingForDungeonPlayerGUIDs.end() );
	}
	void RBSendLookingPlayerList( Player *plr );
	
protected:
	void				RB_RefreshClientLists();			//check which players are still online and looking for a group
	//all details are stored player side (i hope) 
	std::set<uint64>	m_lookingForDungeonPlayerGUIDs;	//actually the player can look for multiple dungeons and multiple types of dungeons
	uint32				m_last_updated;	//we update on request without spams
	Mutex				list_lock;
};

#define sLfgMgr LfgMgr::getSingleton()
