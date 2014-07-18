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
#ifndef _GROUP_H_
#define _GROUP_H_

//safe way to handle arrays in debug mode
#include <array>

enum PartyErrors
{
	ERR_PARTY_NO_ERROR					= 0,
	ERR_PARTY_CANNOT_FIND				= 1,
	ERR_PARTY_IS_NOT_IN_YOUR_PARTY		= 2,
	ERR_PARTY_UNK						= 3,
	ERR_PARTY_IS_FULL					= 4,
	ERR_PARTY_ALREADY_IN_GROUP			= 5,
	ERR_PARTY_YOU_ARENT_IN_A_PARTY		= 6,
	ERR_PARTY_YOU_ARE_NOT_LEADER		= 7,
	ERR_PARTY_WRONG_FACTION				= 8,
	ERR_PARTY_IS_IGNORING_YOU			= 9,
};


enum MaxGroupCount
{
	MAX_GROUP_SIZE_PARTY				= 5,
	MAX_GROUP_SIZE_RAID					= 40,
};

enum GroupTypes
{
	GROUP_TYPE_PARTY					= 0,
	GROUP_TYPE_BG						= 1,	//you can't leave this group type
	GROUP_TYPE_RAID						= 2,
	GROUP_TYPE_BGRAID					= GROUP_TYPE_BG | GROUP_TYPE_RAID,
	GROUP_TYPE_LFD						= 0x08,
};

enum PartyUpdateFlags
{
	GROUP_UPDATE_FLAG_NONE						= 0,		// 0x00000000
	GROUP_UPDATE_FLAG_ONLINE					= 0x00000001,		// uint8
	GROUP_UPDATE_FLAG_HEALTH					= 0x00000002,		// uint32 - 14333
	GROUP_UPDATE_FLAG_MAXHEALTH					= 0x00000004,		// uint32 - 14333
	GROUP_UPDATE_FLAG_POWER_TYPE				= 0x00000008,		// uint8 - 14333
	GROUP_UPDATE_FLAG_POWER						= 0x00000010,		// uint16 - 14333
	GROUP_UPDATE_FLAG_MAXPOWER					= 0x00000020,		// uint16 - 14333
	GROUP_UPDATE_FLAG_LEVEL						= 0x00000040,		// uint16 - 14333
	GROUP_UPDATE_FLAG_ZONEID					= 0x00000080,		// uint16 - 14333
	GROUP_UPDATE_FLAG_UNK						= 0x00000100,		// uint16 - 14333
	GROUP_UPDATE_FLAG_POSITION					= 0x00000200,		// uint16, uint16, uint16 - 14333
	GROUP_UPDATE_FLAG_PLAYER_AURAS				= 0x00000400,		// struct - 14333
	GROUP_UPDATE_FLAG_PET_GUID					= 0x00000800,		// uint64
	GROUP_UPDATE_FLAG_PET_NAME					= 0x00001000,		// string - 14333
	GROUP_UPDATE_FLAG_PET_DISPLAYID				= 0x00002000,		// uint16
	GROUP_UPDATE_FLAG_PET_HEALTH				= 0x00004000,		// uint32 - 14333
	GROUP_UPDATE_FLAG_PET_MAXHEALTH				= 0x00008000,		// uint32
	GROUP_UPDATE_FLAG_PET_POWER_TYPE			= 0x00010000,		// uint8
	GROUP_UPDATE_FLAG_PET_POWER					= 0x00020000,		// uint16 - 14333
	GROUP_UPDATE_FLAG_PET_MAXPOWER				= 0x00040000,		// uint16
	GROUP_UPDATE_FLAG_PET_AURAS					= 0x00080000,		// struct
    GROUP_UPDATE_FLAG_VEHICLE_SEAT				= 0x00100000,       // uint32 vehicle_seat_id (index from VehicleSeat.dbc)
};

enum PartyUpdateFlagGroups
{
	GROUP_UPDATE_TYPE_FULL_CREATE				=	GROUP_UPDATE_FLAG_ONLINE | GROUP_UPDATE_FLAG_HEALTH | GROUP_UPDATE_FLAG_MAXHEALTH |
													GROUP_UPDATE_FLAG_POWER_TYPE | GROUP_UPDATE_FLAG_POWER | GROUP_UPDATE_FLAG_MAXPOWER | 
													GROUP_UPDATE_FLAG_LEVEL | GROUP_UPDATE_FLAG_ZONEID | GROUP_UPDATE_FLAG_POSITION |
													GROUP_UPDATE_FLAG_PLAYER_AURAS |
													GROUP_UPDATE_FLAG_VEHICLE_SEAT, 
//	GROUP_UPDATE_TYPE_FULL_REQUEST_REPLY		=   0x7FFC0BFF,
};

//set_role 
enum PlayerGroupRoleFlag
{
	PLAYER_ROLE_FLAG_NOT_SET	= 0,
	PLAYER_ROLE_FLAG_TANK		= 2,
	PLAYER_ROLE_FLAG_HEALER		= 4,
	PLAYER_ROLE_FLAG_DPS		= 8,
};

enum GroupMemberFlags
{
    MEMBER_FLAG_ASSISTANT   = 0x01,
    MEMBER_FLAG_MAINTANK    = 0x02,
    MEMBER_FLAG_MAINASSIST  = 0x04,
}; 

struct PlayerInfo;
typedef struct
{
	PlayerInfo * player_info;
	Player * player;
}GroupMember;

class Group;
class Player;

typedef std::set<PlayerInfo*> GroupMembersSet;

class SERVER_DECL SubGroup	  // Most stuff will be done through here, not through the "Group" class.
{
public:
	friend class Group;

	SubGroup(Group* parent, uint32 id):m_Parent(parent),m_Id(id)
	{
	}

	~SubGroup();

	ARCEMU_INLINE GroupMembersSet::iterator GetGroupMembersBegin(void) { return m_GroupMembers.begin(); }
	ARCEMU_INLINE GroupMembersSet::iterator GetGroupMembersEnd(void)   { return m_GroupMembers.end();   }

	bool AddPlayer(PlayerInfo * info);
	void RemovePlayer(PlayerInfo * info);
	
	ARCEMU_INLINE bool IsFull(void)				{ return m_GroupMembers.size() >= MAX_GROUP_SIZE_PARTY; }
	ARCEMU_INLINE size_t GetMemberCount(void)		{ return m_GroupMembers.size(); }
	
	ARCEMU_INLINE uint32 GetID(void)			   { return m_Id; }
	ARCEMU_INLINE void SetID(uint32 newid)		 { m_Id = newid; }

	ARCEMU_INLINE void   SetParent(Group* parent)  { m_Parent = parent; }
	ARCEMU_INLINE Group* GetParent(void)		   { return m_Parent; }

	void   Disband();
	bool HasMember(uint32 guid);

protected:

	GroupMembersSet	 m_GroupMembers;
	Group*			  m_Parent;
	uint32			  m_Id;

};

#define GROUP_SAVED_INSTANCE_MISSING (0xFFFFFFFF)
//group has a list of these
struct active_instance
{
	uint32 map_id;
	uint32 difficulty;
	uint32 instance_id;
};

#define MAX_RAID_MARKER_COUNT 5
struct RaidMarkerStore
{
	uint32 map_id;
	uint32 instance_id;
	uint64 object_guid;
};

class Arena;
class SERVER_DECL Group
{
public:
	friend class SubGroup;

	static Group* Create();

	Group(bool Assign);
	~Group();

	// Adding/Removal Management
	bool AddMember(PlayerInfo * info, int32 subgroupid=-1);
	void RemovePlayer(PlayerInfo * info, bool DoNotKickFromBG = false );

	// Leaders and Looting
	void SetLeader(Player* pPlayer,bool silent);
	void SetLooter(Player *pPlayer, uint8 method, uint16 threshold);

	// Transferring data to clients
	void Update();

	ARCEMU_INLINE void SendPacketToAll(WorldPacket *packet) { SendPacketToAllButOne(packet, NULL); }
	void SendPacketToAllButOne(WorldPacket *packet, Player *pSkipTarget);

	ARCEMU_INLINE void OutPacketToAll(uint16 op, uint16 len, const void* data) { OutPacketToAllButOne(op, len, data, NULL); }
	void OutPacketToAllButOne(uint16 op, uint16 len, const void* data, Player *pSkipTarget);

	void SendNullUpdate(Player *pPlayer);

	// Group Combat
	void SendPartyKillLog(Object * player, Object * Unit);

	// Destroying/Converting
	void Disband();
	Player* FindFirstPlayer();
	
	// Accessing functions
	ARCEMU_INLINE SubGroup* GetSubGroup(uint32 Id)
	{
		if(Id >= 8)
			return 0;

		return m_SubGroups[Id];
	}

	ARCEMU_INLINE uint32 GetSubGroupCount(void) { return m_SubGroupCount; }

	ARCEMU_INLINE uint8 GetMethod(void) { return m_LootMethod; }
	ARCEMU_INLINE uint16 GetThreshold(void) { return m_LootThreshold; }
	ARCEMU_INLINE PlayerInfo* GetLeader(void) { return m_Leader; }
	ARCEMU_INLINE PlayerInfo* GetLooter(void) { return m_Looter; }

	void MovePlayer(PlayerInfo* info, uint8 subgroup);

	bool HasMember(Player *pPlayer);
	bool HasMember(PlayerInfo * info);
	ARCEMU_INLINE uint32 MemberCount(void) { return m_MemberCount; }
	ARCEMU_INLINE uint32 MemberCountOnline(void);
	ARCEMU_INLINE bool IsFull() { return ((m_GroupType == GROUP_TYPE_PARTY && m_MemberCount >= MAX_GROUP_SIZE_PARTY) || (m_GroupType == GROUP_TYPE_RAID && m_MemberCount >= MAX_GROUP_SIZE_RAID)); }

	SubGroup* FindFreeSubGroup();

	void ExpandToRaid( bool IsBGRaid );
	void ShrinkToParty();
	void ConvertToNormalFromBG( )
	{
		m_GroupType &= ~GROUP_TYPE_BG;
		m_dirty=true;
		Update();
	}

	void SaveToDB();
	void LoadFromDB(Field *fields);

	ARCEMU_INLINE uint8 GetGroupType() { return m_GroupType; }
	ARCEMU_INLINE uint32 GetID() { return m_Id; }

	void UpdateOutOfRangePlayer(Player * pPlayer, uint32 Flags, bool Distribute, WorldPacket * Packet,bool full_update=false);
	void UpdateAllOutOfRangePlayersFor(Player * pPlayer);
	void HandleUpdateFieldChange(uint32 Index, Player * pPlayer);
	void HandlePartialChange(uint32 Type, Player * pPlayer);

	//uint64 m_targetIcons[8];
	tr1::array< uint64,8> m_targetIcons;
	bool m_disbandOnNoMembers;
	ARCEMU_INLINE Mutex& getLock() { return m_groupLock; }
	ARCEMU_INLINE void Lock() { m_groupLock.Acquire(); }
	ARCEMU_INLINE void Unlock() { return m_groupLock.Release(); }
	bool m_isqueued;

	void SetAssistantLeader(PlayerInfo * pMember);
	void SetMainTank(PlayerInfo * pMember);
	void SetMainAssist(PlayerInfo * pMember);

	ARCEMU_INLINE PlayerInfo * GetAssistantLeader() { return m_assistantLeader; }
	ARCEMU_INLINE PlayerInfo * GetMainTank() { return m_mainTank; }
	ARCEMU_INLINE PlayerInfo * GetMainAssist() { return m_mainAssist; }

	//Zack : 90% of the time this list has 1-2 values and not 8400
	//uint32 m_instanceIds[NUM_MAPS][NUM_INSTANCE_MODES];
//	tr1::array< tr1::array< uint32, NUM_INSTANCE_MODES>, NUM_MAPS> m_instanceIds;
	SimplePointerList<active_instance>		m_active_instances;
	uint32	GetSavedInstance( uint32 map, uint32 difficulty );
	void	RemoveSavedInstance( uint32 map, uint32 difficulty );
	void	RemoveSavedInstances();
	void	AddSavedInstance( uint32 map, uint32 difficulty, uint32 instance_id, uint8 check_exist = 1 );

	void ResetInstanceForPlayers(uint32 map, uint32 difficulty);
	bool CanCast( Player *p, uint32 SpellID );
	void SpawnRaidMarker( uint32 SpellID, DynamicObject *NewMarker );
	void ClearRaidMarkers( uint8 pslot, bool silent = false );
	void SendRaidMarkerStatus();
	RaidMarkerStore m_markers[ MAX_RAID_MARKER_COUNT ];

	bool DisbandOnBGEnd() { return m_DisbandOnBGEnd; };
	void DisbandOnBGEndDisable() { m_DisbandOnBGEnd = false; };

	/************************************************************************/
	/* Voicechat                                                            */
	/************************************************************************/
#ifdef VOICE_CHAT
	void AddVoiceMember(PlayerInfo * pPlayer);
	void RemoveVoiceMember(PlayerInfo * pPlayer);
	void SendVoiceUpdate();
	void CreateVoiceSession();
	void VoiceChannelCreated(uint16 id);
	void VoiceSessionDropped();
	void VoiceSessionReconnected();

public:
	bool m_voiceChannelRequested;
	int16 m_voiceChannelId;
	uint32 m_voiceMemberCount;
protected:
	PlayerInfo* m_voiceMembersList[41];
#endif	// VOICE_CHAT

protected:
	PlayerInfo * m_Leader;
	PlayerInfo * m_Looter;
	PlayerInfo * m_assistantLeader;
	PlayerInfo * m_mainTank;
	PlayerInfo * m_mainAssist;

	uint8 m_LootMethod;
	uint16 m_LootThreshold;
	uint8 m_GroupType;
	uint32 m_Id;
	uint64 m_GUID;

	//SubGroup* m_SubGroups[8];
	tr1::array< SubGroup*, 8> m_SubGroups;
	uint8 m_SubGroupCount;
	uint32 m_MemberCount;
	Mutex m_groupLock;
	bool m_dirty;
	bool m_updateblock;
	bool m_DisbandOnBGEnd;
public:
//	uint8 instance_difficulty;
	uint8 dungeon_difficulty;
	uint8 raid_difficulty;
//	void SetDifficulty(uint8 difficulty);
	uint32 GetInstanceDifficulty(MapInfo *new_instance=NULL);

//	bool LFG_joined;		//if true then leader queued the whole team and not player joined
};

#endif  // _GROUP_H_
