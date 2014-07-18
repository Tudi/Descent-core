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

//safe way to handle arrays in debug mode
#include <array>

#ifndef __BATTLEGROUNDMGR_H
#define __BATTLEGROUNDMGR_H

#define ANTI_CHEAT

class CBattleground;
class MapMgr;
class Player;
class Map;
class Group;
class Corpse;

enum BattlegroundDbcIndex
{
	BGDBC_ALTERAC_VALLEY		= 1,
	BGDBC_WARSONG_GULCH			= 2,
	BGDBC_ARATHI_BASIN			= 3,
	BGDBC_ARENA_NAGRAND			= 4,
	BGDBC_ARENA_BLADES_EDGE		= 5,
	BGDBC_ARENA_ALLMAPS			= 6,
	BGDBC_EYE_OF_THE_STORM		= 7,
	BGDBC_RUINS_OF_LORDAERON	= 8,
	BGDBC_STRAND_OF_THE_ANCIENT	= 9,
	BGDBC_DALARAN_SEWERS		= 10,
	BGDBC_RING_OF_VALOR			= 11,
	BGDBC_RANDOM_BG				= 32,
	BGDBC_TWIN_PEAKS			= 108,
	BGDBC_BATTLE_FOR_GILNEAS	= 120,
	BGDBC_ROWS					= 11,
};

enum BattleGroundTypes
{
	BATTLEGROUND_ALTERAC_VALLEY			= 1,
	BATTLEGROUND_WARSONG_GULCH			= 2,
	BATTLEGROUND_ARATHI_BASIN			= 3,
	BATTLEGROUND_ARENA_2V2				= 4,
	BATTLEGROUND_ARENA_3V3				= 5,
	BATTLEGROUND_ARENA_5V5				= 6,
	BATTLEGROUND_EYE_OF_THE_STORM		= 7,
	BATTLEGROUND_STRAND_OF_THE_ANCIENT	= 9,
	BATTLEGROUND_ALL_BG					= 32,	//random BG
	BATTLEGROUND_TWIN_PEAKS				= 108,
	BATTLEGROUND_GILNEAS				= 120,
	BATTLEGROUND_NUM_TYPES				= 121,
};

enum BattlegroundSpells
{
    SPELL_WAITING_FOR_RESURRECT     = 2584,                 // Waiting to Resurrect
    SPELL_SPIRIT_HEAL_CHANNEL       = 22011,                // Spirit Heal Channel
    SPELL_SPIRIT_HEAL               = 22012,                // Spirit Heal
    SPELL_RESURRECTION_VISUAL       = 24171,                // Resurrection Impact Visual
    SPELL_ARENA_PREPARATION         = 32727,                // use this one, 32728 not correct
    SPELL_ALLIANCE_GOLD_FLAG        = 32724,
    SPELL_ALLIANCE_GREEN_FLAG       = 32725,
    SPELL_HORDE_GOLD_FLAG           = 35774,
    SPELL_HORDE_GREEN_FLAG          = 35775,
    SPELL_PREPARATION               = 44521,                // Preparation
    SPELL_SPIRIT_HEAL_MANA          = 44535,                // Spirit Heal
    SPELL_RECENTLY_DROPPED_FLAG     = 42792,                // Recently Dropped Flag
    SPELL_AURA_PLAYER_INACTIVE      = 43681,                // Inactive
    SPELL_HONORABLE_DEFENDER_25Y    = 68652,                // +50% honor when standing at a capture point that you control, 25yards radius (added in 3.2)
    SPELL_HONORABLE_DEFENDER_60Y    = 66157,                // +50% honor when standing at a capture point that you control, 60yards radius (added in 3.2), probably for 40+ player battlegrounds
    SPELL_THE_LAST_STANDING         = 26549                 // Arena achievement related
};

class BGScore
{
public:
	BGScore()
	{
		KillingBlows = HonorableKills = Deaths = BonusHonor = DamageDone = HealingDone = 0;
		for( uint32 i=0;i<5;i++)
			MiscData[i] = 0;
	}
	uint32 KillingBlows;
	uint32 HonorableKills;
	uint32 Deaths;
	uint32 BonusHonor;
	int32	RatingChange;
	uint32 DamageDone;
	uint32 HealingDone;
	//uint32 MiscData[5];
	tr1::array<uint32,5> MiscData;
};

#define BG_SCORE_AB_BASES_ASSAULTED 0
#define BG_SCORE_AB_BASES_CAPTURED 1
#define BG_SCORE_AV_GRAVEYARDS_ASSAULTED 0
#define BG_SCORE_AV_GRAVEYARDS_DEFENDED 1
#define BG_SCORE_AV_TOWERS_ASSAULTED 2
#define BG_SCORE_AV_TOWERS_DEFENDED 3
#define BG_SCORE_AV_MINES_CAPTURES 4
#define BG_SCORE_EOTS_FLAGS_CAPTURED 0
#define BG_SCORE_WSG_FLAGS_CAPTURED 0
#define BG_SCORE_WSG_FLAGS_RETURNED 1
#define BG_SCORE_SOTA_DEMOLISHERS_DESTROYED 0
#define BG_SCORE_SOTA_GATES_DESTROYED 1
#define BG_SCORE_BFG_BASES_ASSAULTED 0
#define BG_SCORE_BFG_BASES_CAPTURED 1

#define SOUND_BATTLEGROUND_BEGIN			0xD6F
#define SOUND_HORDE_SCORES					8213
#define SOUND_ALLIANCE_SCORES				8173
#define SOUND_ALLIANCE_CAPTURE				8174
#define SOUND_HORDE_CAPTURE					8212
#define SOUND_ALLIANCE_RETURNED				8192
#define SOUND_HORDE_RETURNED				8192	// huh?
#define SOUND_HORDEWINS						8454
#define SOUND_ALLIANCEWINS					8455
#define SOUND_HORDE_BGALMOSTEND				0x2108
#define SOUND_ALLIANCE_BGALMOSTEND			0x2109

#define AB_ALLIANCE_RESOURCES				0x6F0
#define AB_HORDE_RESOURCES					0x6F1
#define AB_HORDE_CAPTUREBASE				0x6F2
#define AB_ALLIANCE_CAPTUREBASE				0x6F3
#define AB_MAX_SCORE						0x6F4

#define BFG_ALLIANCE_RESOURCES				0x6F0
#define BFG_HORDE_RESOURCES					0x6F1
#define BFG_HORDE_CAPTUREBASE				0x6F2
#define BFG_ALLIANCE_CAPTUREBASE			0x6F3
#define BFG_MAX_SCORE						0x6F4

#define BG_PREPARATION						44521
#define BG_REVIVE_PREPARATION				44535
#define RESURRECT_SPELL						21074   // Spirit Healer Res
#define BG_DESERTER							26013

// WSG define's
#define WSG_ALLIANCE_FLAG_CAPTURED			0x922
#define WSG_HORDE_FLAG_CAPTURED				0x923
#define WSG_CURRENT_HORDE_SCORE				0x62E
#define WSG_CURRENT_ALLIANCE_SCORE			0x62D
#define WSG_MAX_SCORE						0x641
#define WSG_WS_TIME_REMAINING				0x1098

#define DEFAULT_BG_CLOSE_TIME			( 25*60*1000 )
// AV define's
#define AV_UNCONTROLED_SNOWFALL_GRAVE	   0x7AE //1 -> show uncontrolled

#define AV_CONTROLED_ICEBLOOD_TOWER_HORDE   0x569 //1 -> horde controled
#define AV_CONTROLED_TOWER_POINT_HORDE	  0x568 //1 -> horde controled
#define AV_CONTROLED_FROSTWOLF_RELIFHUNT_HORDE 0x532 //1 -> horde controled
#define AV_CONTROLED_EAST_FROSTWOLF_TOWER_HORDE 0x567 //1 -> horde controled
#define AV_CONTROLED_WEST_FROSTWOLF_TOWER_HORDE 0x566 //1 -> horde controled
#define AV_CONTROLED_ICEBLOOD_GRAVE_HORDE   0x543 //1 -> horde controled
#define AV_CONTROLED_FROSTWOLF_GRAVE_HORDE   0x53A //1 -> horde controled

#define AV_CONTROLED_IRONDEEP_MINE_TROGG	0x550 //1 -> trogg controled
#define AV_CONTROLED_COLDTHOOT_MINE_KOBOLT  0x54D //1 -> kobolt controled

#define AV_CONTROLED_STORMPIKE_GRAVE_ALLIANCE  0x535 //1 -> alliance controled
#define AV_CONTROLED_STONEHEART_BUNKER_ALLIANCE  0x554 //1 -> alliance controled
#define AV_CONTROLED_ICEWING_BUNKER_ALLIANCE  0x553 //1 -> alliance controled
#define AV_CONTROLED_DUBALDER_NORTH_BUNKER_ALLIANCE  0x552 //1 -> alliance controled
#define AV_CONTROLED_DUBALDER_SOUTH_BUNKER_ALLIANCE  0x551 //1 -> alliance controled
#define AV_CONTROLED_STORMPIKE_AID_STATION_ALLIANCE  0x52D //1 -> alliance controled
#define AV_CONTROLED_STONEHEART_GRAVE_ALLIANCE  0x516 //1 -> alliance controled

static inline uint32 GetFieldCount(uint32 BGType)
{
	switch( BGType )
	{
	case BATTLEGROUND_ALTERAC_VALLEY:
		return 5;
	case BATTLEGROUND_ARATHI_BASIN:
	case BATTLEGROUND_WARSONG_GULCH:
	case BATTLEGROUND_TWIN_PEAKS:
	case BATTLEGROUND_STRAND_OF_THE_ANCIENT:
	case BATTLEGROUND_GILNEAS:
		return 2;
	case BATTLEGROUND_EYE_OF_THE_STORM:
		return 1;
	default:
		return 0;
	}
}

#define MAX_LEVEL_GROUP (2+85/5)
#define MINIMUM_PLAYERS_ON_EACH_SIDE_FOR_BG 1
#define MAXIMUM_BATTLEGROUNDS_PER_LEVEL_GROUP 50
#define LEVEL_GROUP_70 (MAX_LEVEL_GROUP-1)
// get level grouping for player 
static inline uint32 GetLevelGrouping(uint32 level)
{
#ifdef BATTLEGRUND_REALM_BUILD
	return MIN( level / 5, MAX_LEVEL_GROUP );
#endif
	if(level < 10)
		return 0;
	else if(level < 20)
		return 1;
	else if(level < 30)
		return 2;
	else if(level < 40)
		return 3;
	else if(level < 50)
		return 4;
	else if(level < 60)
		return 5;
	else if(level < 70)
		return 6;
	else if(level < 80)
		return 7;
	else if(level < 90)
		return 8;
	else
		return 9;
}

enum BFStatus
{
	BF_STATUS_PACKET_REMOVE_PLAYER			= 0,
	BF_STATUS_PACKET_QUEUE_PLAYER				= 1,
	BF_STATUS_PACKET_ALLOW_ENTER_PLAYER		= 2,
	BF_STATUS_PACKET_TEL_PLAYER				= 3,
	BF_STATUS_PACKET_PLAYER_JOINED			= 4,
};

#define IS_ARENA(x) ( (x) >= BATTLEGROUND_ARENA_2V2 && (x) <= BATTLEGROUND_ARENA_5V5 )
#define IS_ARENADBC(x) ( x == BGDBC_ARENA_NAGRAND || x == BGDBC_ARENA_BLADES_EDGE || x == BGDBC_ARENA_ALLMAPS || x == BGDBC_DALARAN_SEWERS || x == BGDBC_RING_OF_VALOR )

//#define WARMUP_WAITTIME		120000	
#ifdef _DEBUG
	#define WARMUP_WAITTIME		20000	
#else
	#define WARMUP_WAITTIME		60000	
#endif
//#define WARMUP_WAITTIME	10000
class Arena;

class CBattlegroundManager : public Singleton<CBattlegroundManager>, public EventableObject
{
	/* Battleground Instance Map */
	map<uint32, CBattleground*> m_instances[BATTLEGROUND_NUM_TYPES];
	Mutex m_instanceLock;

	/* Max Id */
	uint32 m_maxBattlegroundId;
	
	/* Queue System */
	// Instance Id -> list<Player guid> [ BattlegroundType ] (instance 0 - first available)
//	list<uint32> m_queuedPlayers[BATTLEGROUND_NUM_TYPES][MAX_LEVEL_GROUP];
	tr1::array< tr1::array<set<uint32>,MAX_LEVEL_GROUP + 1 >,BATTLEGROUND_NUM_TYPES + 1> m_queuedPlayers;

	// Instance Id -> list<Group id> [BattlegroundType][LevelGroup]
//	list<uint32> m_queuedGroups[BATTLEGROUND_NUM_TYPES];
	tr1::array<set<uint32>,BATTLEGROUND_NUM_TYPES > m_queuedGroups;

	Mutex m_queueLock;

public:
	CBattlegroundManager();
	~CBattlegroundManager();

	/* Get the Config */
	void LoadBGSetFromConfig();

	/* Packet Handlers */
	void HandleBattlegroundListPacket(WorldSession * m_session, uint32 BattlegroundType, bool battlemaster=true);
	void HandleArenaJoin(WorldSession * m_session, uint32 BattlegroundType, uint8 as_group, uint8 rated_match);

	/* Player Logout Handler */
	void OnPlayerLogout(Player * plr);

	/* Handler On Update Event */
	void EventQueueUpdate();
	void EventQueueUpdate(bool forceStart);

	/* Handle GetBattlegroundQueue Command */
	void HandleGetBattlegroundQueueCommand(WorldSession * m_session);

	/* Handle Battleground Join */
	void HandleBattlegroundJoin(WorldSession * m_session, WorldPacket & pck);

	/* Remove Player From All Queues */
	void RemovePlayerFromQueues(Player * plr);
	void RemoveGroupFromQueues(Group * grp);

	/* Create a battleground instance of type x */
	SERVER_DECL CBattleground * CreateInstance( uint32 Type, uint32 LevelGroup, uint32 MapID = 0 );

	/* Can we create a new instance of type x level group y? (NO LOCK!) */
	bool CanCreateInstance(uint32 Type, uint32 LevelGroup);

	/* Deletes a battleground (called from MapMgr) */
	void DeleteBattleground(CBattleground * bg);

	/* Build SMSG_BATTLEFIELD_STATUS */
	void SendBattlefieldStatus(Player * plr, BFStatus Status, uint32 Type, uint32 InstanceID, uint32 Time, uint32 MapId, uint8 RatedMatch);

	/* Gets ArenaTeam info from group */
	uint32 GetArenaGroupQInfo(Group * group, int type, uint32 *avgRating);

	/* Creates an arena with groups group1 and group2 */
	Arena *CreateArenaType(int type, Group * group1, Group * group2);

	/* Add player to bg team */
	void AddPlayerToBgTeam(CBattleground * bg, deque<uint32> *playerVec, uint32 i, uint32 j, int Team);

	/* Add player to bg */
	void AddPlayerToBg(CBattleground * bg, deque<uint32> *playerVec, uint32 i, uint32 j);

	/* Add a group to an arena */
	void AddGroupToArena(CBattleground * bg, Group * group, int nteam);

	/* Returns a mapid for the battleground */
//	uint32 GetMap(uint32 bg_index);

	/* Returns the minimum number of players (Only valid for battlegrounds) */
	uint32 GetMinimumPlayers(uint32 dbcIndex);

	/* Returns the maximum number of players (Only valid for battlegrounds) */
	uint32 GetMaximumPlayers(uint32 dbcIndex);
	uint32 GetMaximumPlayersDBC(uint32 dbcIndex);

	//let players know the status of the queue
	void NotifyPlayersAboutStatus( Player *plr_change );

	//sniff, fucking pointer coruptions.!!!!this is a hack until i find the reason
	bool IsValidBGPointer( CBattleground *checkme );
};

class CBattleground : public EventableObject
{
	friend class AVNode;
protected:
	/* Groups */
///	Group * m_groups[2];
	tr1::array<Group *,2> m_groups;

	time_t					m_nextPvPUpdateTime;
	MapMgr					*m_mapMgr;
	uint32					m_id;
	uint32					m_type;
	uint32					m_levelGroup;
//	uint32 m_deltaRating[2];
	tr1::array<int32,2>		m_deltaRating;
//	uint32					m_invisGMs;
	uint32					m_honorPerKill;

public:
	/* Team->Player Map */
//	std::set<Player *> m_players[2];
	tr1::array< std::set<Player *>, 2 > m_players;
	uint32					m_ScriptCPReward;
	void Lock() { m_mainLock.Acquire(); }
	void Unlock() { m_mainLock.Release(); }
//	void AddInvisGM() {Lock(); m_invisGMs++; Unlock();}
//	void RemoveInvisGM() {Lock(); m_invisGMs--; Unlock();}
	void AddGroupToTeam( Group *group, int8 Team, int32 RemainingAdds = 255 );
	Group *GetGroup(uint32 team )
	{
		if( team < 2 )
			return m_groups[ team ];
		return NULL;
	}
protected:
	/* PvP Log Data Map */
	map<uint32, BGScore>	m_pvpData;

	/* Lock for all player data */
	Mutex					m_mainLock;

	/* Player count per team */
	uint32					m_playerCountPerTeam;

	/* "pending" players */
	//set<uint32> m_pendPlayers[2];
	tr1::array< set<uint32>, 2> m_pendPlayers;

	/* starting time */
	uint32					m_startTime;
	bool					m_started;

	/* countdown stuff */
//	uint32					m_countdownStage;
	uint32					m_WarmupExpireStamp;

	/* winner stuff */
	bool					m_ended;
	uint8					m_winningteam;

	/* resurrect queue */
	map<Creature*, set<uint32> > m_resurrectMap;
	uint32					m_lastResurrect;

	bool					m_isWeekend;
	bool					m_isRandomBG;	//awayrds conquest point

public:
	bool					m_SkipRewards; //wargames
	int32					m_PlayerResurrects;
	int32					m_TeamResurrects,m_TeamResurrectsDone;
	bool					m_SuddenDeath;
	bool					m_DenyNewJoins;
	char					*m_ArenaTeamNames[2];

	void SendChatMessage(uint32 Type, uint64 Guid, const char * Format, ...);

	/* Hook Functions */
	virtual void HookOnPlayerDeath(Player * plr) = 0;

	/* Repopping - different battlegrounds have different ways of handling this */
	virtual bool HookHandleRepop(Player * plr) = 0;

	/* In CTF battlegrounds mounting will cause you to lose your flag. */
	virtual void HookOnMount(Player * plr) = 0;

	/* Only used in CTF (as far as I know) */
	virtual void HookFlagDrop(Player * plr, GameObject * obj) = 0;
	virtual void HookFlagStand(Player * plr, GameObject * obj) = 0;
	virtual void HookOnFlagDrop(Player *plr) = 0;

	/* Used when a player kills a player */
	virtual void HookOnPlayerKill(Player * plr, Player * pVictim) = 0;
	virtual void HookOnHK(Player * plr) = 0;

	/* On Area Trigger */
	virtual void HookOnAreaTrigger(Player * plr, uint32 id) = 0;

	/* On Shadow Sight */
	virtual void HookOnShadowSight() = 0;

	/* On Loot Generating */
	virtual void HookGenerateLoot(Player *plr, Object *pCorpse) = 0;

	/* On Unit Killing */
	virtual void HookOnUnitKill(Player * plr, Unit * pVictim) = 0;

	/* Retreival Functions */
	ARCEMU_INLINE uint32 GetId() { return m_id; }
	ARCEMU_INLINE uint32 GetLevelGroup() { return m_levelGroup; }
	ARCEMU_INLINE MapMgr* GetMapMgr() { return m_mapMgr; }
	
	/* Creating a battleground requires a pre-existing map manager */
	CBattleground(MapMgr * mgr, uint32 id, uint32 levelgroup, uint32 type);
	virtual ~CBattleground();

	/* Has it ended? */
	ARCEMU_INLINE bool HasEnded() { return m_ended; }
	/* Has it started? */
	ARCEMU_INLINE bool HasStarted() { return m_started; }

	/* Send our current world states to a player . */
	void SendWorldStates(Player * plr); 
	void SetWorldState(uint32 Index, uint32 Value); 

	/* Send the pvp log data of all players to this player. */
	void SendPVPData(Player * plr);

	/* Get the starting position for this team. */
	virtual LocationVector GetStartingCoords(uint32 Team) = 0;

	/* Send a packet to the entire battleground */
	void DistributePacketToAll(WorldPacket * packet);

	/* send a packet to only this team */
	void DistributePacketToTeam(WorldPacket * packet, uint32 Team);

	/* play sound to this team only */
	void PlaySoundToTeam(uint32 Team, uint32 Sound);

	/* play sound to all */
	void PlaySoundToAll(uint32 Sound);

	/* Full? */
	ARCEMU_INLINE bool IsFull() { return !(HasFreeSlots(0,m_type) || HasFreeSlots(1,m_type)); }

	/* Are we full? */
	bool HasFreeSlots(uint32 Team, uint32 type);
	//only gives back a valid number if it would make teams have same number of players
	uint32 GetBalanceFreeSlots(uint32 Team, uint32 type);

	/* Add Player */
	SERVER_DECL void AddPlayer(Player * plr, uint32 team, bool WithInit = false );
	virtual void OnAddPlayer(Player * plr) = 0;

	/* Remove Player */
	void RemovePlayer(Player * plr, bool logout);
	virtual void OnRemovePlayer(Player * plr) = 0;

	/* Port Player */
	void PortPlayer(Player * plr, bool skip_teleport = false);
	virtual void OnCreate() = 0;

	/* Remove pending player */
	void EventRemovePendingPlayer(uint32 lowguid);
	void RemovePendingPlayer(Player * plr);

	/* Gets the number of free slots */
	uint32 GetFreeSlots(uint32 t, uint32 type);

	GameObject * SpawnGameObject(uint32 entry,uint32 MapId , float x, float y, float z, float o, uint32 flags, uint32 faction, float scale);
	Creature * SpawnCreature(uint32 entry,float x, float y, float z, float o);
	void UpdatePvPData();

	ARCEMU_INLINE uint32 GetStartTime() { return m_startTime; }
	ARCEMU_INLINE uint32 GetType() { return m_type; }

	// events should execute in the correct context
	int32 event_GetInstanceID();
	void EventCreate();

	virtual uint32 GetNameID() { return 34;}
	void EventCountdown();

	virtual void Start();
	virtual void OnStart() {}
	void Close();
	virtual void OnClose() {}

	Creature * SpawnSpiritGuide(float x, float y, float z, float o, uint32 horde);

	ARCEMU_INLINE uint32 GetLastResurrect() { return m_lastResurrect; }
	void AddSpiritGuide(Creature * pCreature);
	void RemoveSpiritGuide(Creature * pCreature);
	void QueuePlayerForResurrect(Player * plr, Creature * spirit_healer);
	void RemovePlayerFromResurrect(Player * plr, Creature * spirit_healer);
	void EventResurrectPlayers();
	virtual bool CanPlayerJoin(Player * plr,uint32 type);
	virtual bool CreateCorpse(Player * plr) { return true; }
	virtual bool HookSlowLockOpen(GameObject * pGo, Player * pPlayer, Spell * pSpell) { return false; }

	void BuildPvPUpdateDataPacket(WorldPacket * data);
	virtual uint8 Rated() { return 0; }
	virtual uint8 GetArenaTeamType() { return 0; }
	void OnPlayerPushed(Player* plr);

	virtual void SetIsWeekend(bool isweekend) {}
	virtual void SetIsRandomBG(bool israndom) { m_isRandomBG = israndom; }
	void QueueAtNearestSpiritGuide(Player *plr, Creature *old);
	void	EventOnEndedPreCleanup();
};

#define BattlegroundManager CBattlegroundManager::getSingleton( )

	
#endif
