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
#include <array>

#ifndef _GUILD_H
#define _GUILD_H

struct PlayerInfo;

enum PETITION_TURNIN_ERRORS
{
	ERR_PETITION_OK,
	ERR_PETITION_ALREADY_SIGNED,
	ERR_PETITION_IN_GUILD,
	ERR_PETITION_CREATOR,
	ERR_PETITION_NOT_ENOUGH_SIGNATURES,

	//PETITION_YOU_ALREADY_IN_A_GUILD = 0x02,
	//PETITION_YOU_NEED_MORE_SIGNS	= 0x04,
	//ERR_PET_SPELL_DEAD
	//ERR_PETITION_DECLINED_S
	//ERR_PETITION_SIGNED_S
	//ERR_PETITION_SIGNED
	//ERR_PETITION_OFFERED

};

enum GUILDEMBLEM_ERRORS
{
	ERR_GUILDEMBLEM_SUCCESS,
	ERR_GUILDEMBLEM_INVALID_TABARD_COLORS,
	ERR_GUILDEMBLEM_NOGUILD,
	ERR_GUILDEMBLEM_NOTGUILDMASTER,
	ERR_GUILDEMBLEM_NOTENOUGHMONEY,
	ERR_GUILDEMBLEM_INVALIDVENDOR,
};

enum GuildMessageTypes
{
	G_MSGTYPE_ALL,
	G_MSGTYPE_ALLBUTONE,
	G_MSGTYPE_PUBLICCHAT,
	G_MSGTYPE_OFFICERCHAT,
};

enum MONEY
{
	MONEY_ONE_COPPER			= 1,
	MONEY_ONE_SILVER			= MONEY_ONE_COPPER * 100,
	MONEY_ONE_GOLD				= MONEY_ONE_SILVER * 100,
};

enum CommandErrors
{
	GUILD_U_HAVE_INVITED		= 0x00,
	GUILD_INTERNAL				= 0x01,
	GUILD_ALREADY_IN_GUILD		= 0x02,
	ALREADY_IN_GUILD			= 0x03,
	INVITED_TO_GUILD			= 0x04,
	ALREADY_INVITED_TO_GUILD	= 0x05,
	GUILD_NAME_INVALID			= 0x06,
	GUILD_NAME_EXISTS			= 0x07,
	GUILD_LEADER_LEAVE			= 0x08,
	GUILD_PERMISSIONS			= 0x08,
	GUILD_PLAYER_NOT_IN_GUILD	= 0x09,
	GUILD_PLAYER_NOT_IN_GUILD_S = 0x0A,
	GUILD_PLAYER_NOT_FOUND		= 0x0B,
	GUILD_NOT_ALLIED			= 0x0C,
};

//for uint32(0)<<name<<code
enum GUILD_COMMAND_RESULTS
{
	C_R_CREATED								= 1,
	C_R_ALREADY_IN_GUILD					= 2,
	C_R_TARGET_ALREADY_IN_GUILD				= 3,
	C_R_ALREADY_INVITED_TO_GUILD			= 4,
	C_R_TARGET_ALREADY_INVITED_TO_GUILD		= 5,
	C_R_GUILDNAME_HAS_INVALID_CHARACTERS	= 6,
	C_R_GUILD_NAME_EXISTS					= 7,
	C_R_DONT_HAVE_PERMISSION				= 8,
	C_R_NOT_IN_GUILD						= 9,
	C_R_TARGET_IS_NOT_IN_YOUR_GUILD			= 10,
	C_R_NAME_NOT_FOUND						= 11,
	C_R_CANT_INVITE_PLYRS_FROM_OPP_ALLIANCE = 12,
	C_R_NAME_RANK_TOO_HIGH					= 13,
	C_R_NAME_RANK_AT_LOWEST_RANK			= 14,
	/*
	ERR_NO_GUILD_CHARTER

	ERR_GUILD_NAME_NAME_CONSECUTIVE_SPACES
	ERR_GUILD_NAME_INVALID_SPACE
	ERR_GUILD_NAME_RESERVED
	ERR_GUILD_NAME_PROFANE
	ERR_GUILD_NAME_MIXED_LANGUAGES
	ERR_GUILD_NAME_TOO_SHORT

	ERR_GUILD_ENTER_NAME

	ERR_GUILD_NAME_EXISTS_S
	ERR_GUILD_NAME_INVALID

	ERR_GUILD_RANK_TOO_LOW_S
	ERR_GUILD_RANK_TOO_HIGH_S
	ERR_GUILD_RANK_IN_USE
	ERR_GUILD_RANKS_LOCKED
	ERR_GUILD_LEADER_LEAVE
	ERR_GUILD_NOT_ALLIED
	ERR_GUILD_DISBANDED
	ERR_GUILD_LEADER_CHANGED_SS
	ERR_GUILD_LEADER_IS_S
	ERR_GUILD_INTERNAL
	ERR_GUILD_NOT_IN_A_GUILD
	ERR_GUILD_CANT_DEMOTE_S
	ERR_GUILD_CANT_PROMOTE_S
	ERR_GUILD_PLAYER_NOT_IN_GUILD
	ERR_GUILD_PLAYER_NOT_IN_GUILD_S
	ERR_GUILD_PLAYER_NOT_FOUND_S
	ERR_GUILD_LEADER_SELF
	ERR_GUILD_LEADER_S
	ERR_GUILD_DISBAND_SELF
	ERR_GUILD_DISBAND_S
	ERR_GUILD_REMOVE_SELF
	ERR_GUILD_REMOVE_SS
	ERR_GUILD_LEAVE_S
	ERR_GUILD_QUIT_S
	ERR_GUILD_DEMOTE_SSS
	ERR_GUILD_PROMOTE_SSS
	ERR_GUILD_FOUNDER_S
	ERR_GUILD_JOIN_S
	ERR_GUILD_PERMISSIONS
	ERR_GUILD_DECLINE_S
	ERR_GUILD_ACCEPT
	ERR_ALREADY_IN_GUILD
	ERR_INVITED_TO_GUILD
	ERR_ALREADY_INVITED_TO_GUILD_S
	ERR_ALREADY_IN_GUILD_S
	ERR_INVITED_TO_GUILD_SS
	ERR_GUILD_INVITE_S
	ERR_GUILD_CREATE_S
	*/
};

enum typecommand
{
	GUILD_CREATE_S					= 0x00,
	GUILD_INVITE_S					= 0x01,
	GUILD_QUIT_S					= 0x02,
	GUILD_PROMOTE_S					= 0x03,
	GUILD_FOUNDER_S					= 0x0C,
	GUILD_MEMBER_S					= 0x0D,
	GUILD_PUBLIC_NOTE_CHANGED_S		= 0x13,
	GUILD_OFFICER_NOTE_CHANGED_S	= 0x14,
};

enum GuildRankRights
{
	GR_RIGHT_EMPTY			            = 0x00000040,
	GR_RIGHT_GCHATLISTEN				= 0x00000001,
	GR_RIGHT_GCHATSPEAK					= 0x00000002,
	GR_RIGHT_OFFCHATLISTEN				= 0x00000004,
	GR_RIGHT_OFFCHATSPEAK				= 0x00000008,
	GR_RIGHT_INVITE						= 0x00000010,
	GR_RIGHT_REMOVE						= 0x00000020,
	GR_RIGHT_PROMOTE					= 0x00000080,
	GR_RIGHT_DEMOTE						= 0x00000100,
	GR_RIGHT_SETMOTD					= 0x00001000,
	GR_RIGHT_EPNOTE						= 0x00002000,
	GR_RIGHT_VIEWOFFNOTE				= 0x00004000,
	GR_RIGHT_EOFFNOTE					= 0x00008000,
	GR_RIGHT_EGUILDINFO					= 0x00010000,
	GR_RIGHT_GUILD_BANK_REPAIR			= 0x00040000,
	GR_RIGHT_GUILD_BANK_WITHDRAW_MONEY	= 0x00080000,
	GR_RIGHT_CREATE_GUILD_EVENT			= 0x00100000,
	GR_RIGHT_REQ_AUTHENTIFICATOR		= 0x00200000,
	GR_RIGHT_EDIT_TABS					= 0x00400000,
	GR_RIGHT_ALL						= 0xFDF1FF,
	GR_RIGHT_DEFAULT					= GR_RIGHT_GCHATLISTEN | GR_RIGHT_GCHATSPEAK | GR_RIGHT_EMPTY,

	GR_RIGHT_GUILD_BANK_VIEW_TAB		= 0x01,
	GR_RIGHT_GUILD_BANK_DEPOSIT_ITEMS	= 0x02,
	GR_RIGHT_GUILD_BANK_CHANGE_TABTXT	= 0x04,
	GR_RIGHT_GUILD_BANK_ALL	= GR_RIGHT_GUILD_BANK_VIEW_TAB | GR_RIGHT_GUILD_BANK_DEPOSIT_ITEMS | GR_RIGHT_GUILD_BANK_CHANGE_TABTXT,
};

enum GuildRankTabRights
{
	GRT_RIGHT_VIEW_CONTENT		= 0x01,
	GRT_RIGHT_DEPOSIT_ITEMS		= 0x02,
	GRT_RIGHT_UPDATE_NAME		= 0x04,
};

//#define GUILDRANK_INVITE GUILDRANK_VETERAN
//#define GUILDRANK_KICK GUILDRANK_OFFICER
//#define GUILDRANK_PROMOTE GUILDRANK_VETERAN
//#define GUILDRANK_DEMOTE GUILDRANK_VETERAN
//#define GUILDRANK_MOTD GUILDRANK_OFFICER

enum GuildEvent
{
//	GUILD_EVENT_PROMOTION			= 0x0,
//	GUILD_EVENT_DEMOTION			= 0x0,
	GUILD_EVENT_MOTD				= 0x3,
	GUILD_EVENT_JOINED				= 0x4,
	GUILD_EVENT_LEFT				= 0x5,
	GUILD_EVENT_REMOVED				= 0x6,	//!!!just guessing
	GUILD_EVENT_LEADER_IS			= 0x7,
	GUILD_EVENT_LEADER_CHANGED		= 0x8,	//!!!just guessing
	GUILD_EVENT_DISBANDED			= 0x9,
	GUILD_EVENT_TABARDCHANGE		= 0xA,	//!!!just guessing
	GUILD_EVENT_RANK_EDITED			= 0xB,	
	GUILD_EVENT_RANK_ADDED			= 0xC,	
	GUILD_EVENT_RANK_DELETED		= 0xD,	
	GUILD_EVENT_RANK_PROMOTE_DEMOTE	= 0xE,	
	GUILD_EVENT_HASCOMEONLINE		= 0x10,
	GUILD_EVENT_HASGONEOFFLINE		= 0x11,
	GUILD_EVENT_BANKTABBOUGHT		= 0x12,	//!!!just guessing
	GUILD_EVENT_SETNEWBALANCE		= 0x15,
	GUILD_EVENT_TABINFO				= 0x17,	//!!!just guessing
/*
	self:RegisterEvent("GUILD_ROSTER_UPDATE");
	self:RegisterEvent("PLAYER_GUILD_UPDATE");
	self:RegisterEvent("GUILD_XP_UPDATE");
	self:RegisterEvent("GUILD_PERK_UPDATE");
	self:RegisterEvent("PLAYER_ENTERING_WORLD");
	self:RegisterEvent("UPDATE_FACTION"); */
};

enum GuildLogEventE
{
	GUILD_LOG_EVENT_END_LIST	= 0,
	GUILD_LOG_EVENT_INVITE		= 1,
	GUILD_LOG_EVENT_JOIN		= 2,
	GUILD_LOG_EVENT_PROMOTION	= 3,
	GUILD_LOG_EVENT_DEMOTION	= 4,
	GUILD_LOG_EVENT_REMOVAL		= 5,
	GUILD_LOG_EVENT_LEFT		= 6,
};

enum GuildBankLogEvents
{
	GUILD_BANK_LOG_EVENT_DEPOSIT_ITEM	= 1,
	GUILD_BANK_LOG_EVENT_WITHDRAW_ITEM	= 2,
	GUILD_BANK_LOG_EVENT_DEPOSIT_MONEY	= 4,
	GUILD_BANK_LOG_EVENT_WITHDRAW_MONEY	= 5,
};

enum GuildNewsLogEvents
{
	GUILD_NEWS_LOG_GUILD_MOTD			= -1,		//no category
	GUILD_NEWS_LOG_GUILD_ACHIEVEMENT	= 0,
	GUILD_NEWS_LOG_PLAYER_ACHIEVEMENT	= 1,
	GUILD_NEWS_LOG_DUNGEON_ENCOUNTER	= 2,		//col_0 from DungeonEncounter.dbc
	GUILD_NEWS_LOG_ITEM_LOOTED			= 3,
	GUILD_NEWS_LOG_ITEM_CRAFTED			= 4,
	GUILD_NEWS_LOG_ITEM_PURCHASED		= 5,
	GUILD_NEWS_LOG_LEVELED				= 6,		//not tested, value was 5 and rest was 0
	GUILD_NEWS_LOG_GUILD_CREATED		= 7,
};

enum GuildMemberFlags
{
    GUILD_MEMBER_FLAG_OFFLINE           = 0x00,
    GUILD_MEMBER_FLAG_ONLINE            = 0x01,
    GUILD_MEMBER_FLAG_AFK               = 0x02,
    GUILD_MEMBER_FLAG_DND               = 0x04,
    GUILD_MEMBER_FLAG_MOBILE            = 0x08, // Not used; On retail it's set if the plr is logged on with Mobile Armory
};

enum GuildFinderCharFilterActivityFlags
{
	GF_CFAF_QUESTING		= 1,
	GF_CFAF_DUNGEONS		= 2,
	GF_CFAF_RAIDS			= 4,
	GF_CFAF_PVP				= 8,
	GF_CFAF_PVE				= 16,
};

enum GuildFinderCharFilterAvailabilityFlags
{
	GF_CFAF_WEEKDAYS		= 1,
	GF_CFAF_WEEKENDS		= 2,
};

enum GuildFinderCharFilterRoleFlags
{
	GF_CFRF_TANK			= 1,
	GF_CFRF_HEALER			= 2,
	GF_CFRF_DPS				= 4,
};

enum GuildFinderGuildFilterLevelFlags
{
	GF_GFLF_ANY_LEVEL		= 1,
	GF_GFLF_MAX_LEVEL		= 2,
};

#define ITEM_ENTRY_GUILD_CHARTER 5863
//#define ARENA_TEAM_CHARTER_2v2      23560
//#define ARENA_TEAM_CHARTER_2v2_COST 800000  // 80 G
//#define ARENA_TEAM_CHARTER_3v3      23561
//#define ARENA_TEAM_CHARTER_3v3_COST 1200000 // 120 G
//#define ARENA_TEAM_CHARTER_5v5      23562
//#define ARENA_TEAM_CHARTER_5v5_COST 2000000 // 200 G

#define MAX_GUILD_BANK_SLOTS	98
#define MAX_GUILD_BANK_TABS		8
#define MAX_GUILD_RANKS			10
#define MAX_GUILD_NEWS			200	//bliz sent up to 400 values here

#define MAX_GUILD_LEVEL			25
extern uint64					GUILD_LEVEL_XP_REQ[MAX_GUILD_LEVEL+2];
#define GUILD_DAILY_XP_LIMIT	6246000

struct SERVER_DECL GuildRankTabPermissions
{
	uint32 iFlags;
	int32 iStacksPerDay;
};

#define MAX_GUILD_FINDER_PLAYER_REQUESTS	10
#define MAX_GUILD_FINDER_GUILD_REQUESTS		10
#define MAX_GUILD_FINDER_PENDING_SECONDS	(28*24*60*60)

struct GuildFinderRequest
{
	GuildFinderRequest() { ref_count = 0; };
	void AddRef() { ref_count++; };
	//!!has a chance to delete the object !
	void DecRef() { ref_count--; if( ref_count == 0 ) delete this; }
	string	player_message;
	string	guild_name;
	string	player_name;
	uint8	ref_count;	//gets added to player and guild, if ref count reaches 0 then it gets deleted
	uint32	guild_id;
	uint32	player_guid;
	uint32	stamp_unix;
};

class SERVER_DECL GuildRank
{
public:
	GuildRank()
	{
		iId = iRights = iGoldLimitPerDay = 0;
	}
	uint32 iId;
	uint32 iRights;
	int32 iGoldLimitPerDay;
	//GuildRankTabPermissions iTabPermissions[MAX_GUILD_BANK_TABS];
	tr1::array< GuildRankTabPermissions, MAX_GUILD_BANK_TABS> iTabPermissions;
	char * szRankName;
	bool CanPerformCommand(uint32 t);
	bool CanPerformBankCommand(uint32 t, uint32 tab);
};

struct SERVER_DECL GuildMember
{
	PlayerInfo	* pPlayer;
	const char	* szPublicNote;
	const char	* szOfficerNote;
	GuildRank	* pRank;
	uint32		uLastWithdrawReset;
	uint32		uWithdrawlsSinceLastReset;
	uint32		xp_contrib;
	//uint32 uLastItemWithdrawReset[MAX_GUILD_BANK_TABS];
	tr1::array< uint32, MAX_GUILD_BANK_TABS > uLastItemWithdrawReset;
	//uint32 uItemWithdrawlsSinceLastReset[MAX_GUILD_BANK_TABS];
	tr1::array<uint32, MAX_GUILD_BANK_TABS> uItemWithdrawlsSinceLastReset;

	uint32 CalculateAllowedItemWithdraws(uint32 tab);
	void OnItemWithdraw(uint32 tabid);
	
	uint32 CalculateAvailableAmount();
	void OnMoneyWithdraw(uint64 amt);
};

struct SERVER_DECL GuildLogEvent
{
	uint32 iLogId;
	uint8 iEvent;
	uint32 iTimeStamp;
	//uint32 iEventData[3];
	tr1::array< uint32, 3> iEventData;
};

struct SERVER_DECL GuildBankEvent
{
	uint32 iLogId;
	uint8 iAction;
	uint32 uPlayer;
	uint32 uEntry;
	uint8 iStack;
	uint32 uTimeStamp;
};

enum GuildNewsEventFlags
{
	GUILD_NEWS_FLAG_STICKY	= 1,
};

struct SERVER_DECL GuildNewsEvent
{
	uint32				Id;		// will be used to delete/sticky...
	uint8				Type;	// see enum GuildNewsLogEvents
	uint64				OwnerGuid;	// the one who performed the action( player / guild )
	uint64				Value;		// achievement / items entry
	uint32				TimeStamp;	// UNIXSTAMP ( will be converted to gametime )
	uint8				Flags;		// see enum GuildNewsEventFlags
};

struct SERVER_DECL GuildBankTab
{
	uint8 iTabId;
	char * szTabName;
	char * szTabIcon;
	char * szTabInfo;
	//Item * pSlots[MAX_GUILD_BANK_SLOTS];
	tr1::array< Item *, MAX_GUILD_BANK_SLOTS> pSlots;
	list<GuildBankEvent*> lLog;
};

class Charter;

typedef map<PlayerInfo*, GuildMember*> GuildMemberMap;

class SERVER_DECL Guild
{
public:
	Guild();
	~Guild( );
	static Guild* Create();
	bool LoadFromDB(Field * f);

	/** Log entry processing
	 */
protected:
	uint32 m_hiLogId;
public:
	uint32 GenerateGuildLogEventId();
	
	/* guild bank logging calls
	 */
	void LogGuildBankActionMoney(uint8 iAction, uint32 uGuid, int64 uAmount);
	void LogGuildBankAction(uint8 iAction, uint32 uGuid, uint32 uEntry, uint8 iStack, GuildBankTab * pTab);
	static void ClearOutOfDateLogEntries();

	/* only call at first create/save */
	void CreateInDB();

	/** Sets new MOTD, and updates in database
	 */
	void SetMOTD(const char * szNewMotd, WorldSession * pClient);

	/** Gets MOTD
	 */
	ARCEMU_INLINE const char * GetMOTD() const { return m_motd; }

	/** Sets guild information, updates in database
	 */
	void SetGuildInformation(const char * szGuildInformation, WorldSession * pClient);

	/** Gets guild information
	 */
	ARCEMU_INLINE const char * GetGuildInformation() const { return m_guildInfo; }

	/** Sends the guild roster to this client.
	 */
	void SendGuildRoster(WorldSession * pClient);

	/** Sends the guild query response to this client.
	 */
	void SendGuildQuery(WorldSession * pClient);

	/** Sends the guild query response to this client.
	 */
	void SendGuildRankQuery(WorldSession * pClient);

	//need to decode content. 
	void SendGuildRankChangeNotify(WorldSession * pClient);

	/** Adds a member to the guild, saves him into the database.
	 * A provided rank of -1 means the lowest rank.
	 */
	void AddGuildMember(PlayerInfo * pMember, WorldSession * pClient, int32 ForcedRank = -1);

	/** Removes a member from the guild.
	 * If this member is the guild master, the guild will be automatically handed down to the next
	 * highest member.
	 */
	void RemoveGuildMember(PlayerInfo * pMember, WorldSession * pClient);

	/** Check if a given ID is a valid rank ID
	 * Used when client is sending the ID
	 */
	bool IsValidRank( uint32 Rank );

	/** Promotes a member of a guild.
	 * Do not use for changing guild master. Use ChangeGuildMaster() for that instead.
	 */
	void PromoteGuildMember(PlayerInfo * pMember, WorldSession * pClient,uint32 new_rank);

	/** Demotes a member of a guild.
	 * Do not use for changing guild master. Use ChangeGuildMaster() for that instead.
	 */
	void DemoteGuildMember(PlayerInfo * pMember, WorldSession * pClient, uint32 new_rank);

	/** Changes the guild master of the guild.
	 */
	void ChangeGuildMaster(PlayerInfo * pNewMaster, WorldSession * pClient);

	/** Sends a guild command packet to the client.
	 */
	static void SendGuildCommandResult(WorldSession * pClient, uint32 iCmd, const char * szMsg, uint32 iType);

	/** Sends a turn in petition result to the client.
	 */
	static void SendTurnInPetitionResult( WorldSession * pClient, uint32 result );

	/** Logs a guild event and sends it to all online players.
	 */
	void LogGuildEvent(uint8 iEvent, uint64 TargetGUID, uint8 iStringCount, ...);
	
	/** Guild event logging.
	 */
	void AddGuildLogEntry(uint8 iEvent, uint8 iParamCount, ...);

	/** Creates a guild from a charter.
	 */
	void CreateFromCharter(Charter * pCharter, WorldSession * pTurnIn);

	/** Sends a packet to all online players.
	 */
	void SendPacket(WorldPacket * data);

	/** Sends a guild chat message.
	 */
	void GuildChat(const char * szMessage, WorldSession * pClient, uint32 iType);

	/** Sends an officer chat message.
	 */
	void OfficerChat(const char * szMessage, WorldSession * pClient, uint32 iType);

	/** Sends the guild log to a player.
	 */
	void SendGuildLog(WorldSession * pClient);
	void SendGuildBankLog(WorldSession * pClient, uint8 iSlot);
	void SendGuildStatusUnk(WorldSession * pClient);

	/** Sets the public note for a player.
	 */
	void SetPublicNote(PlayerInfo * pMember, const char * szNewNote, WorldSession * pClient);

	/** Sets the officer note for a player.
	 */
	void SetOfficerNote(PlayerInfo * pMember, const char * szNewNote, WorldSession * pClient);

	/** Disbands a guild.
	 */
	void Disband();

	/** creation time stuff
	 */
	uint32 creationDay;
	uint32 creationMonth;
	uint32 creationYear;

	/** Getters :P
	 */
	ARCEMU_INLINE const char * GetGuildName() const { return m_guildName; }
	ARCEMU_INLINE const uint64 GetGuildLeader() const { return m_guildLeader; }
	ARCEMU_INLINE const uint32 GetGuildId() const { return m_guildId; }
	ARCEMU_INLINE const uint32 GetBankTabCount() const { return m_bankTabCount; }
	ARCEMU_INLINE const uint64 GetBankBalance() const { return m_bankBalance; }
	ARCEMU_INLINE const size_t GetNumMembers() const { return m_members.size(); }
	/** Creates a guild rank with the specified permissions.
	 */
	GuildRank * CreateGuildRank(const char * szRankName, uint32 iPermissions, bool bFullGuildBankPermissions);

	/** "Pops" or removes the bottom guild rank.
	 */
	void RemoveGuildRank(GuildRank * rank,WorldSession * pClient);

	/** Buys a new guild bank tab, usable only by guild master 
	 */
	void BuyBankTab(WorldSession * pClient);

	/** Deposits money into the guild bank, usable by any member.
	 */
	void DepositMoney(WorldSession * pClient, int64 uAmount);

	/** Withdraws money from the guild bank, usable by members with that permission.
	 */
	void WithdrawMoney(WorldSession * pClient, int64 uAmount);

	/** Retrieves a guild rank for editing
	 */
	ARCEMU_INLINE GuildRank * GetGuildRank(uint32 Id)
	{ 
		if(Id >= MAX_GUILD_RANKS)
			return NULL;

		return m_ranks[Id];
	}
	//used in 403 client to update a rank
	ARCEMU_INLINE void SwapGuildRank(uint32 RId1,uint32 RId2);

	/** Gets a guild bank tab for editing/viewing
	 */

	ARCEMU_INLINE GuildBankTab * GetBankTab(uint32 Id)
	{
		if(Id >= m_bankTabCount)
			return NULL;

		return m_bankTabs[Id];
	}

	/** Gets a guild member struct
	 */
	ARCEMU_INLINE GuildMember * GetGuildMember(PlayerInfo * pInfo)
	{
		GuildMemberMap::iterator itr;
		GuildMember * ret;
		m_lock.Acquire();
		itr = m_members.find(pInfo);
		ret = (itr!=m_members.end()) ? itr->second : NULL;
		m_lock.Release();
		return ret;
	}

	/* Get iterators */
	ARCEMU_INLINE GuildMemberMap::iterator GetGuildMembersBegin() { return m_members.begin(); }
	ARCEMU_INLINE GuildMemberMap::iterator GetGuildMembersEnd() { return m_members.end(); }

	/* Get, Lock, Unlock Mutex */
        ARCEMU_INLINE Mutex& getLock() { return m_lock; }
	ARCEMU_INLINE void Lock() { m_lock.Acquire(); }
	ARCEMU_INLINE void Unlock() { return m_lock.Release(); }

	/** Sends the guild bank to this client.
	 */
	void SendGuildBank(WorldSession * pClient, GuildBankTab * pTab, int8 updated_slot1 = -1, int8 updated_slot2 = -1);
//	void SendGuildBankInfo(WorldSession * pClient);

	/** Changes the tabard info.
	 */
	void SetTabardInfo(uint32 EmblemStyle, uint32 EmblemColor, uint32 BorderStyle, uint32 BorderColor, uint32 BackgroundColor);
	
	/** Sends the guild information packet to the specified client.
	 */
	void SendGuildInfo(WorldSession * pClient);

	void SaveGuildState();
	ARCEMU_INLINE uint32 GetPropBgrColor() { return m_backgroundColor; }
	ARCEMU_INLINE uint32 GetPropBrdColor() { return m_borderColor; }
	ARCEMU_INLINE uint32 GetPropEmblemColor() { return m_emblemColor; }
	ARCEMU_INLINE uint32 GetPropBrdStyle() { return m_borderStyle; }
	ARCEMU_INLINE uint32 GetPropEmblemStyle() { return m_emblemStyle; }
	ARCEMU_INLINE uint32 GetLevel() { return m_level; }
	ARCEMU_INLINE uint64 GetXPTotalThisLevel() { return m_level_XP; }
	ARCEMU_INLINE uint64 GetXPTodayRemaining() { return (GUILD_DAILY_XP_LIMIT - m_level_XP_limit_today); }
	ARCEMU_INLINE uint64 GetXPNextLevel() { return GUILD_LEVEL_XP_REQ[ m_level ]; }
	void GiveXP(uint64 amt);
	void SendGuildXPStatus(WorldSession * pClient);
	int64 EventPlayerLootedGold( Player *p, int64 gold );
	//guild finder begin
	uint32	m_GF_Filter;		//this is combined filter
	string	m_GF_Msg;
	bool	m_GF_is_public;
	void	GuildFinderSetFilter( bool is_public,uint32 FilterInterest, uint32 FilterActivity, uint32 FilterRole, uint32 FilterLevelFlags, const char *msg )
	{
		m_GF_is_public = is_public;
		m_GF_Filter = 0;
		m_GF_Filter = m_GF_Filter | ((FilterInterest & 0xFF));
		m_GF_Filter = m_GF_Filter | ((FilterActivity & 0xFF) << 8);
		m_GF_Filter = m_GF_Filter | ((FilterRole & 0xFF) << 16);
		m_GF_Filter = m_GF_Filter | ((FilterLevelFlags & 0xFF) << 24);
		if( msg )
			m_GF_Msg = msg;
		else
			m_GF_Msg = "";
	}
	uint32	GetGuildFinderFilter() { return m_GF_Filter; }
	uint32	GetGuildFinderFilterInterest() { return ((m_GF_Filter >> 0) & 0xFF ); }
	uint32	GetGuildFinderFilterActivity() { return ((m_GF_Filter >> 8) & 0xFF ); }
	uint32	GetGuildFinderFilterRole() { return ((m_GF_Filter >> 16) & 0xFF ); }
	uint32	GetGuildFinderFilterLevel() { return ((m_GF_Filter >> 24) & 0xFF ); }
	const char	*GetGuildFinderDesc() { return m_GF_Msg.c_str(); }
	void	GuildFinderAddRequest( GuildFinderRequest *r );
	void	GuildFinderDelRequest( uint64 player_guid );
	bool	GuildFinderHasPendingRequest( uint64 player_guid );
	void	GuildFinderCleanOfflines();
	std::list< GuildFinderRequest *> m_guild_finder_requests;
	//guild finder end

	void	GuildNewsAdd( uint8 Type, uint64 GUID, uint32 Value );
	void	GuildNewsSendAll( WorldSession * pClient );
	void	GuildNewsToggleSticky( uint32 EventId, WorldSession * pClient );
protected:
	
	/** Enables/disables command logging.
	 * Use when performing mass events such as guild creation or destruction.
	 */
	bool m_commandLogging;
	
	/** Internal variables
	 */
	uint32 m_guildId;
	uint32 m_emblemStyle;
	uint32 m_emblemColor;
	uint32 m_borderStyle;
	uint32 m_borderColor;
	uint32 m_backgroundColor;
	uint64 m_guildLeader;
	uint32 m_creationTimeStamp;
	uint32 m_bankTabCount;
	uint64 m_bankBalance;
	uint32 m_level;
	uint64 m_level_XP;
	uint64 m_level_XP_limit_today;
	uint32 m_level_XP_limit_today_stamp;
	uint32 m_last_XP_save_stamp;

	typedef vector<GuildBankTab*> GuildBankTabVector;
	GuildBankTabVector m_bankTabs;

	char * m_guildName;
	char * m_guildInfo;
	char * m_motd;
	
	/** Guild Member Map.
	 */
	//typedef map<PlayerInfo*, GuildMember*> GuildMemberMap;
	GuildMemberMap m_members;

	/** Guild Rank Information.
	 */
	//GuildRank * m_ranks[MAX_GUILD_RANKS];
	tr1::array<GuildRank *,  MAX_GUILD_RANKS> m_ranks;

	/** Guild log. Ordered in first event -> last event.
	 */
	list<GuildLogEvent*>  m_log;
	list<GuildBankEvent*> m_moneyLog;
	list<GuildNewsEvent*> m_NewsLog;

	/** Guild lock.
	 */
	Mutex m_lock;

	/** finds the lowest rank
	 */
	GuildRank * FindLowestRank();
	GuildRank * FindHighestRank();
};


#endif

/*0,1->guild created
2->you are already in guild
3->selection is already in guild
4->you have been already invited to guild
5->selection is already invited to guild
6->guildname contains invalid characters pls rename
7->there is an already guild named "name"
8->you dont have permission to do that
9->you are not in guild
10->selection isnot in your guild
11->"name" not found
12->you cannot invite players from opposite alliance
13->"name"'s rank is too high
14->"name" is already at lowest rank*/
