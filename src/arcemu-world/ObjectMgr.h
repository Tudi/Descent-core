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

#ifndef _OBJECTMGR_H
#define _OBJECTMGR_H

//safe way to handle arrays in debug mode
#include <array>

ARCEMU_INLINE bool FindXinYString(std::string& x, std::string& y)
{
	return y.find(x) != std::string::npos;
}

typedef std::set<uint32> InstanceBossTrashList;
struct InstanceBossInfo
{
	uint32 mapid;
	uint32 creatureid;
	InstanceBossTrashList trash;
	uint32 trashRespawnOverride;
};

struct GM_Ticket
{
	uint64 guid;
	uint64 playerGuid;
	std::string name;
	uint32 level;
	uint32 map;
	float posX;
	float posY;
	float posZ;
	std::string message;
	uint32 timestamp;
	uint64 deleted_by;
	uint64 assignedToPlayer;
	std::string comment;
};

enum
{
	GM_TICKET_CHAT_OPCODE_NEWTICKET     = 1,
	GM_TICKET_CHAT_OPCODE_LISTSTART     = 2,
	GM_TICKET_CHAT_OPCODE_LISTENTRY     = 3,
	GM_TICKET_CHAT_OPCODE_CONTENT       = 4,
	GM_TICKET_CHAT_OPCODE_APPENDCONTENT = 5,
	GM_TICKET_CHAT_OPCODE_REMOVED       = 6,
	GM_TICKET_CHAT_OPCODE_UPDATED       = 7, 
	GM_TICKET_CHAT_OPCODE_ASSIGNED      = 8,
	GM_TICKET_CHAT_OPCODE_RELEASED      = 9, 
	GM_TICKET_CHAT_OPCODE_COMMENT       = 10,
	GM_TICKET_CHAT_OPCODE_ONLINESTATE   = 11
};

#pragma pack(push,1)
struct FishingZoneEntry
{
	uint32 ZoneID;
	uint32 MinSkill;
	uint32 MaxSkill;
};

struct ZoneGuardEntry
{
	uint32 ZoneID;
	uint32 HordeEntry;
	uint32 AllianceEntry;
};

struct UnitModelSizeEntry
{
	uint32	DisplayId;
	float	HalfSize;
};

struct ProfessionDiscovery
{
	uint32 SpellId;
	uint32 SpellToDiscover;
	uint32 SkillValue;
	float Chance;
};

struct ItemPage
{
    uint32 id;
	char * text;
	uint32 next_page;
};

struct WorldStringTable
{
	uint32 id;
	char * text;
};

struct WorldBroadCast
{
	uint32 id;
	char * text;
	uint32 percent;
};

#pragma pack(pop)

struct SpellReplacement
{
	uint32 count;
	uint32 *spells;
};

class Group;

struct GossipMenuItem
{
	uint32		Id;
	uint32		IntId;
	uint8		Icon;
	uint8		Extra;//requires passw to open it
	string		Text;
	uint32      m_gSender;
	uint32      m_gAction;
	string		m_gBoxMessage;
    uint32      m_gBoxMoney;
};
struct SpellEntry;
struct TrainerSpell
{
	SpellEntry * pCastSpell;
	SpellEntry * pLearnSpell;
	SpellEntry * pCastRealSpell;
	uint32	DeleteSpell;
	uint32	RequiredSpell;
	uint32	RequiredSkillLine;
	uint32	RequiredSkillLineValue;
	bool	AdsProfession;
	uint32	Cost;
	uint32	RequiredLevel;
};

struct Trainer
{
	uint32 SpellCount;
	SimpleVect<TrainerSpell*> Spells;
	char*	UIMessage;
    uint32 RequiredSkill;
	uint32 RequiredSkillLine;
	uint32 RequiredClass;
	uint32 TrainerType;
	uint32 Can_Train_Gossip_TextId;
	uint32 Cannot_Train_GossipTextId;
};

struct ReputationMod
{
	uint32 faction[2];
	int32 value;
	uint32 replimit;
};

struct InstanceReputationMod
{
	uint32 mapid;
	uint32 mob_rep_reward;
	uint32 mob_rep_limit;
	uint32 boss_rep_reward;
	uint32 boss_rep_limit;
	uint32 faction[2];
};

struct ReputationModifier
{
	uint32 entry;
	vector<ReputationMod> mods;
};

struct InstanceReputationModifier
{
	uint32 mapid;
	vector<InstanceReputationMod> mods;
};

struct ReputationToReputationMod
{
	uint32	source_rep;
	uint32	target_rep;				// mod rep a -> b
	float	mod_factor;				// probably a negative number like -1
	int32	mod_flat;				// add a simple value and not a PCT one
	int32	target_rep_limit;		// do not change target rep more then this
};

struct NpcMonsterSay
{
	float Chance;
	uint32 Language;
	uint32 Type;
	char * MonsterName;

	uint32 TextCount;
	char ** Texts;
};

enum MONSTER_SAY_EVENTS
{
	MONSTER_SAY_EVENT_ENTER_COMBAT		= 0,
	MONSTER_SAY_EVENT_RANDOM_WAYPOINT	= 1,
	MONSTER_SAY_EVENT_CALL_HELP			= 2,
	MONSTER_SAY_EVENT_ON_COMBAT_STOP	= 3,
	MONSTER_SAY_EVENT_ON_DAMAGE_TAKEN	= 4,
	MONSTER_SAY_EVENT_ON_DIED			= 5,
	NUM_MONSTER_SAY_EVENTS,
};
/*
enum AREATABLE_FLAGS
{
    AREA_CANNOT_DISABLE_PVP = 0x01, //not dead sure
    AREA_CITY_AREA          = 0x20,
    AREA_NEUTRAL_AREA       = 0x40,
    AREA_PVP_ARENA          = 0x80,
    AREA_CITY               = 0x200,
    AREA_SANCTUARY          = 0x800,	// 311 sanctuary areas ? now way
    AREA_ISLAND             = 0x1000,
    UNK1		            = 0x4000,	//seems like expansion content
    AREA_PVP_OBJECTIVE_AREA = 0x8000,
};

enum AREATABLE_CATEGORY
{
    AREAC_CONTESTED          = 0,
    AREAC_ALLIANCE_TERRITORY = 2,
    AREAC_HORDE_TERRITORY    = 4,
    AREAC_SANCTUARY          = 6,
};
*/
struct TotemSpells;

#define MAX_PREDEFINED_NEXTLEVELXP PLAYER_LEVEL_CAP_BLIZZLIKE
static const uint32 NextLevelXp[MAX_PREDEFINED_NEXTLEVELXP]= 
{
	400,	900,	1400,	2100,	2800,	3600,	4500,	5400,	6500,	7600,
	8700,	9800,	11000,	12300,	13600,	15000,	16400,	17800,	19300,	20800,
	22400,	24000,	25500,	27200,	28900,	30500,	32200,	33900,	36300,	38800,
	41600,	44600,	48000,	51400,	55000,	58700,	62400,	66200,	70200,	74300,
	78500,	82800,	87100,	91600,	95300,	101000,	105800,	110700,	115700,	120900,
	126100,	131500,	137000,	142500,	148200,	154000,	159900,	165800,	172000,	494000,
	317000, 349000, 386000, 428000, 475000, 527000, 585000, 648000, 717000, 1523800,
	1539600, 1555700, 1571800, 1587900, 1604200, 1620700, 1637400, 1653900, 1670800, 1700000,
	1686300 ,2121500 ,4004000 ,5203400 ,8548500
};

struct QuestPOIPoint
{
    int32 x;
    int32 y;
    QuestPOIPoint() : x(0), y(0) {}
    QuestPOIPoint(int32 _x, int32 _y) : x(_x), y(_y) {}
};

struct QuestPOI
{
    int32 ObjectiveIndex;
    uint32 MapId;
    uint32 WorldMapAreaDBCIndex;
    uint32 FloorLevel;
    uint32 Unk3;
    uint32 Unk4;
    std::vector<QuestPOIPoint> points;

    QuestPOI() : ObjectiveIndex(0), MapId(0), WorldMapAreaDBCIndex(0), FloorLevel(0), Unk3(0), Unk4(0) {}
    QuestPOI(int32 objIndex, uint32 mapId, uint32 unk1, uint32 unk2, uint32 unk3, uint32 unk4) : ObjectiveIndex(objIndex), MapId(mapId), WorldMapAreaDBCIndex(unk1), FloorLevel(unk2), Unk3(unk3), Unk4(unk4) {}
};

typedef std::vector<QuestPOI> QuestPOIVector;
typedef std::map<uint32, QuestPOIVector> QuestPOIMap;

struct LootStoreStruct
{
	uint32	entry;			//this is entry of the loot list -> same as found in creature_proto
	uint32	ItemEntry;		//this will be looted if drop filter is passed
	float	DropChance;		//first filter
	int32	MinCountOrRef;	//
	uint32	MaxCount;		//randomize this and add to min count
	uint8	GroupID;		//uint8 since client can take max 16
	uint8	ConditionType;	//
	uint32	Cond[2];		//if loot is conditional then these are parameters
	uint32	difficulty_mask;//only drop in certain difficulty of instance
	SimpleVect< SimpleVect<LootStoreStruct*>* > *ref_list;	//loot will redirect loot picking to this list maybe more then 1 count
	_LootItem	item;
};

struct LootCurencyStoreStruct
{
	uint32	entry;				//the NPC
	uint32	currency_type;
	uint32	currency_amt;
	uint32	max_level;			//if attacker has higher level then this then he will not receive points
	uint32	difficulty_mask;	// same mob might be present in multiple instance types but has to award different loot type
};

class SERVER_DECL GossipMenu
{
public:
	GossipMenu(uint64 Creature_Guid, uint32 Text_Id);
	void AddItem(GossipMenuItem* GossipItem);
	void AddItem(uint8 Icon, const char* Text, int32 Id = -1, int8 Extra = 0);
	void AddMenuItem(uint8 Icon, std::string Message, uint32 dtSender, uint32 dtAction, std::string BoxMessage, uint32 BoxMoney, bool Coded = false, uint32 ID=0);
	void BuildPacket(WorldPacket& Packet);
	void SendTo(Player* Plr);
	void SendGossipMenu( uint32 TitleTextId, uint64 npcGUID );
	GossipMenuItem GetItem(uint32 Id);
	ARCEMU_INLINE void SetTextID(uint32 TID) { TextId = TID; }

	uint32 unk240;
protected:
	uint32 TextId;
	uint64 CreatureGuid;
	std::vector<GossipMenuItem> Menu;
};

#define GUILD_CHARTER_SIGNATURES_REQ	4
class Charter
{
public:
	uint32 GetNumberOfSlotsByType()
	{
		switch(CharterType)
		{
		case CHARTER_TYPE_GUILD:
			return GUILD_CHARTER_SIGNATURES_REQ;

/*		case CHARTER_TYPE_ARENA_2V2:
			return 1;

		case CHARTER_TYPE_ARENA_3V3:
			return 2;

		case CHARTER_TYPE_ARENA_5V5:
			return 4; */

		default:
			return GUILD_CHARTER_SIGNATURES_REQ;
		}
	}

	uint32 SignatureCount;
	uint32 * Signatures;
	uint32 CharterType;
	uint32 Slots;
	uint64 LeaderGuid;
	uint64 ItemGuid;
	uint32 CharterId;
	string GuildName;

	Charter(Field * fields);
	Charter(uint32 id, uint32 leader, uint32 type) : CharterType(type), LeaderGuid(leader), CharterId(id)
	{
		SignatureCount = 0;
		ItemGuid = 0;
		Slots = GetNumberOfSlotsByType();
		Signatures = new uint32[Slots];
		memset(Signatures, 0, sizeof(uint32)*Slots);
	}

	~Charter()
	{
		delete [] Signatures;
		Signatures = NULL;
	}
	
	void SaveToDB();
	void Destroy();		 // When item is deleted.

	void AddSignature(uint32 PlayerGuid);
	void RemoveSignature(uint32 PlayerGuid);

	ARCEMU_INLINE uint64 GetLeader() { return LeaderGuid; }
	ARCEMU_INLINE uint32 GetID() { return CharterId; }

	ARCEMU_INLINE bool IsFull() { return (SignatureCount == Slots); }
};

typedef std::map<uint32, std::list<SpellEntry*>* >                  OverrideIdMap;
typedef HM_NAMESPACE::hash_map<uint32, Player*>                     PlayerStorageMap;
typedef std::list<GM_Ticket*>                                       GmTicketList;
typedef std::map<uint32, InstanceBossInfo*>                         InstanceBossInfoMap;

#if (!defined( WIN32 ) && !defined( WIN64 ) )
#define arcemu_USE_MAP_PLAYER_INDEX
#ifdef arcemu_USE_MAP_PLAYER_INDEX

// you can use the string map (slower)
typedef map<string, PlayerInfo*> PlayerNameStringIndexMap;


#else			// or

// gcc has no default hash for string type,
// so we have to make an explicit hash template here
template<>
struct __gnu_cxx::hash<string>
{
	size_t operator()(string& tbh) const
	{
		// simple crc32 hash for now, we may need to change this later however
		return size_t( crc32( (const unsigned char*)tbh.c_str(), tbh.length() ) );
	}
}

typedef HM_NAMESPACE::hash_map<string, PlayerInfo*> PlayerNameStringIndexMap;

#endif
#else

// vc++ has the type for a string hash already, so we don't need to do anything special
typedef HM_NAMESPACE::hash_map<string, PlayerInfo*>					PlayerNameStringIndexMap;
typedef HM_NAMESPACE::hash_map<uint32, Guild*>                      GuildMap;

#endif

class SERVER_DECL ObjectMgr : public Singleton < ObjectMgr >, public EventableObject
{
public:
	ObjectMgr();
	~ObjectMgr();
	void LoadCreatureWaypoints();

	void LoadCreatureTimedEmotes();
	TimedEmoteList * GetTimedEmoteList(uint32 spawnid);
	void LoadCreatureClickSpells();

	// other objects
    
    // Set typedef's
	typedef HM_NAMESPACE::hash_map<uint32, Group*>						GroupMap;
	
    // HashMap typedef's
    typedef HM_NAMESPACE::hash_map<uint64, Item*>                       ItemMap;
	typedef HM_NAMESPACE::hash_map<uint32, CorpseData*>                 CorpseCollectorMap;
	typedef HM_NAMESPACE::hash_map<uint32, PlayerInfo*>                 PlayerNameMap;
	typedef HM_NAMESPACE::hash_map<uint32, PlayerCreateInfo*>           PlayerCreateInfoMap;
	typedef HM_NAMESPACE::hash_map<uint32, skilllinespell*>             SLMap;
	typedef HM_NAMESPACE::hash_map<uint32, std::vector<CreatureItem>*>  VendorMap;
    typedef HM_NAMESPACE::hash_map<uint32, Transporter*>                TransportMap;
	typedef HM_NAMESPACE::hash_map<uint32, Trainer*>                    TrainerMap;
	typedef HM_NAMESPACE::hash_map<uint32, std::vector<TrainerSpell*> > TrainerSpellMap;
    typedef HM_NAMESPACE::hash_map<uint32, ReputationModifier*>         ReputationModMap;
    typedef HM_NAMESPACE::hash_map<uint32, Corpse*>                     CorpseMap;
    
    // Map typedef's
//    typedef std::map<uint32, LevelInfo*>                                LevelMap;
//	typedef std::map<pair<uint32, uint32>, LevelMap* >                  LevelInfoMap;
    typedef std::map<uint32, std::list<ItemPrototype*>* >               ItemSetContentMap;
	typedef std::map<uint32, uint32>                                    NpcToGossipTextMap;
	typedef std::map<uint32, set<SpellEntry*> >                         PetDefaultSpellMap;
	typedef std::map<uint32, uint32>                                    PetSpellCooldownMap;
	typedef std::map<uint32, TotemSpells*>                              TotemSpellMap;
	typedef std::multimap <uint32,uint32>                               BCEntryStorage;

    // object holders
	GmTicketList         GM_TicketList;
	TotemSpellMap        m_totemSpells;
	OverrideIdMap        mOverrideIdMap;
	//InstanceBossInfoMap* m_InstanceBossInfoMap[NUM_MAPS];
	tr1::array<InstanceBossInfoMap*,NUM_MAPS> m_InstanceBossInfoMap;

	Player* GetPlayer(const char* name, bool caseSensitive = true);
	Player* GetPlayer(uint64 guid);
	
	CorpseMap m_corpses;
	Mutex _corpseslock;
	Mutex m_corpseguidlock;
    Mutex _TransportLock;
	uint32 m_hiCorpseGuid;
	uint32 m_hiGuildId;
	
	Item * CreateItem(uint32 entry,Player * owner);
	Item * LoadItem(const uint64 &guid);
	Item * LoadExternalItem(const uint64 &guid);
  
	// Groups
	Group * GetGroupByLeader(Player *pPlayer);
	Group * GetGroupById(uint32 id);
	uint32 GenerateGroupId()
	{
		uint32 r;
		m_guidGenMutex.Acquire();
		r = ++m_hiGroupId;
		m_guidGenMutex.Release();
		return r;
	}

	uint32 GenerateGuildId()
	{
		uint32 r;
		m_guidGenMutex.Acquire();
		r = ++m_hiGuildId;
		m_guidGenMutex.Release();
		return r;
	}

	void AddGroup(Group* group)
	{
		m_groupLock.AcquireWriteLock();
		m_groups[ group->GetID() ] = group;
		m_groupLock.ReleaseWriteLock();
	}

	void RemoveGroup(Group* group)
	{
		m_groupLock.AcquireWriteLock();
		m_groups.erase(group->GetID());
		m_groupLock.ReleaseWriteLock();
	}

	void GroupVoiceReconnected();

	void LoadGroups();

	// player names
	void AddPlayerInfo(PlayerInfo *pn);
	PlayerInfo *GetPlayerInfo(const uint64 &guid );
	PlayerInfo *GetPlayerInfoByName(const char * name);
	void RenamePlayerInfo(PlayerInfo * pn, const char * oldname, const char * newname);
	void DeletePlayerInfo(uint32 guid);
	PlayerCreateInfo* GetPlayerCreateInfo(uint8 race, uint8 class_) const;

	// Guild
	void AddGuild(Guild *pGuild);
	uint32 GetTotalGuildCount();
	bool RemoveGuild(uint32 guildId);
	Guild* GetGuild(uint32 guildId);  
	Guild* GetGuildByLeaderGuid(const uint64 &leaderGuid);  
	Guild* GetGuildByGuildName(std::string guildName);

	//Corpse Stuff
	Corpse *GetCorpseByOwner(uint32 ownerguid);
	Corpse * CreateCorpse();
	void AddCorpse(Corpse*);
	void RemoveCorpse(Corpse*);
	Corpse * GetCorpse(uint32 corpseguid);

	uint32 GetGossipTextForNpc(uint32 ID);

	// Gm Tickets
	void AddGMTicket(GM_Ticket *ticket, bool startup = false);
	void UpdateGMTicket(GM_Ticket *ticket);
	void RemoveGMTicketByPlayer(const uint64 &playerGuid);
	void RemoveGMTicket(const uint64 &ticketGuid,const uint64 &GMGuid);
	void DeleteGMTicketPermanently(const uint64 &ticketGuid);
	void DeleteAllRemovedGMTickets();
	GM_Ticket* GetGMTicket(const uint64 &ticketGuid);
	GM_Ticket* GetGMTicketByPlayer(const uint64 &playerGuid);
	//std::list<GM_Ticket*>* GetGMTicketsByPlayer(uint64 playerGuid);

	//Vendors
	std::vector<CreatureItem> *GetVendorList(uint32 entry);
	void SetVendorList(uint32 Entry, std::vector<CreatureItem>* list_);

	//Totem
	TotemSpells* GetTotemSpell(uint32 spellId);

	std::list<ItemPrototype*>* GetListForItemSet(uint32 setid);

	Pet * CreatePet( uint32 entry, uint8 pet_number );
	uint32 m_hiPetGuid;
	uint32 m_hiArenaTeamId;
	uint32 GenerateArenaTeamId()
	{
		uint32 ret;
		m_arenaTeamLock.Acquire();
		ret = ++m_hiArenaTeamId;
		m_arenaTeamLock.Release();
		return ret;
	}

	Mutex m_petlock;

	Player * CreatePlayer();
	 Mutex m_playerguidlock;
	PlayerStorageMap _players;
	RWLock _playerslock;
	uint32 m_hiPlayerGuid;
	
	void AddPlayer(Player * p);//add it to global storage
	void RemovePlayer(Player *p);


	// Serialization

	void LoadQuests();
	void LoadPlayersInfo();
	void LoadPlayerCreateInfo();
	void LoadGuilds();
	Corpse* LoadCorpse(uint32 guid);
	void LoadCorpses(MapMgr * mgr);
	void LoadGMTickets();
	void SaveGMTicket(GM_Ticket* ticket, QueryBuffer * buf);
	void LoadInstanceBossInfos();
	void LoadAuctions();
	void LoadAuctionItems();
	void LoadSpellSkills();
	void LoadVendors();
	void ReloadVendors();
	void LoadTotemSpells();
	void LoadAIThreatToSpellId();
//	void LoadSpellFixes();
	void LoadSpellProcs();
	void LoadSpellEffectsOverride();

	void LoadReputationModifierTable(const char * tablename, ReputationModMap * dmap);
	void LoadReputationModifiers();
	ReputationModifier * GetReputationModifier(uint32 entry_id, uint32 faction_id);
	SimpleVect<ReputationToReputationMod *>* GetRepChainedModList( uint32 Rep )
	{
		HM_NAMESPACE::hash_map<uint32, SimpleVect<ReputationToReputationMod *> *>::iterator itr;
		itr = m_reputation_chains.find( Rep );
		if( itr == m_reputation_chains.end() )
			return NULL;
		return itr->second;
	}

	void SetHighestGuids();
	uint32 GenerateLowGuid(uint32 guidhigh);
	uint32 GenerateMailID();
	uint64 GenerateTicketID();
	
	void LoadTransporters();
	void ProcessGameobjectQuests();
    void AddTransport(Transporter * pTransporter);
   
	void LoadTrainers();
	Trainer* GetTrainer(uint32 Entry);

	void LoadExtraItemStuff();
	void LoadExtraCreatureProtoStuff();
	void LoadProfessionDiscoveries();

	void StoreBroadCastGroupKey();

	void CreateGossipMenuForPlayer(GossipMenu** Location,uint64 Guid, uint32 TextID, Player* Plr); 

//	LevelInfo * GetLevelInfo(uint32 Race, uint32 Class, uint32 Level);
//	void GenerateLevelUpInfo();
	void LoadDefaultPetSpells();
	set<SpellEntry*>* GetDefaultPetSpells(uint32 Entry);
	uint32 GetPetSpellCooldown(uint32 SpellId);
	WayPointVect * GetWayPointMap(uint32 spawnid);
	void LoadSpellOverride();

	void ResetDailies();
	void ResetWeeklies();

	uint32 GenerateCreatureSpawnID()
	{
		m_CreatureSpawnIdMutex.Acquire();
		uint32 r = ++m_hiCreatureSpawnId;
		m_CreatureSpawnIdMutex.Release();
		return r;
	}

	uint32 GenerateGameObjectSpawnID()
	{
		m_GOSpawnIdMutex.Acquire();
		uint32 r = ++m_hiGameObjectSpawnId;
		m_GOSpawnIdMutex.Release();
		return r;
	}
	uint64	GenerateEquipmentSetGuid()
	{
		m_EquipmentSetIdMutex.Acquire();
		uint64 r = ++m_hiEquipmentSetId;
		m_EquipmentSetIdMutex.Release();
		return r;
	}

//	uint32		m_hiVehicleGuid;
	uint32		m_hiTransportGuid;
	Transporter * GetTransporter(uint32 guid);
	Transporter * GetTransporterByEntry(uint32 entry);

	Charter * CreateCharter(uint32 LeaderGuid, CharterTypes Type);
	Charter * GetCharter(uint32 CharterId, CharterTypes Type);
	void RemoveCharter(Charter *);
	void LoadGuildCharters();
	Charter * GetCharterByName(string &charter_name, CharterTypes Type);
	Charter * GetCharterByItemGuid(const uint64 &guid);
	Charter * GetCharterByGuid(const uint64 &playerguid, CharterTypes type);

	ArenaTeam * GetArenaTeamByName(string & name, uint32 Type);
	ArenaTeam * GetArenaTeamById(uint32 id);
	ArenaTeam * GetArenaTeamByGuid(uint32 guid, uint32 Type);
	void UpdateArenaTeamRankings();
	void UpdateArenaTeamWeekly();
	void ResetArenaTeamRatings();
	void LoadArenaTeams();
	//HM_NAMESPACE::hash_map<uint32, ArenaTeam*> m_arenaTeamMap[3];
	tr1::array< HM_NAMESPACE::hash_map<uint32, ArenaTeam*>, 3> m_arenaTeamMap;
	HM_NAMESPACE::hash_map<uint32, ArenaTeam*> m_arenaTeams;
	void RemoveArenaTeam(ArenaTeam * team);
	void AddArenaTeam(ArenaTeam * team);
	Mutex m_arenaTeamLock;

	typedef HM_NAMESPACE::hash_map<uint32, NpcMonsterSay*> MonsterSayMap;
	//MonsterSayMap mMonsterSays[NUM_MONSTER_SAY_EVENTS];
	tr1::array< MonsterSayMap, NUM_MONSTER_SAY_EVENTS> mMonsterSays;

	void HandleMonsterSayEvent(Creature * pCreature, MONSTER_SAY_EVENTS Event);
	bool HasMonsterSay(uint32 Entry, MONSTER_SAY_EVENTS Event);
	void LoadMonsterSay();

	bool HandleInstanceReputationModifiers(Player * pPlayer, Unit * pVictim);
	void LoadInstanceReputationModifiers();
	void LoadChainedReputationModifiers();

	void LoadDisabledSpells();
	void ReloadDisabledSpells();
	ARCEMU_INLINE GuildMap::iterator GetGuildsBegin() { return mGuild.begin(); }
	ARCEMU_INLINE GuildMap::iterator GetGuildsEnd() { return mGuild.end(); }

	std::set<ProfessionDiscovery*> ProfessionDiscoveryTable;

	// cebernic: This is an perfect Broadcast system,multi-lang supported also.
	ARCEMU_INLINE uint32 GetBCGroupCountByKey(uint32 Key) { return (uint32)m_BCEntryStorage.count(Key); }
	ARCEMU_INLINE uint32 CalcCurrentBCEntry() 
	// func sync at MAKE_TASK(ObjectMgr, StoreBroadCastGroupKey)[world.cpp]
	{
		if ( m_BCEntryStorage.empty() ) return 0;
		vector<uint32> Entries;
		BCEntryStorage::iterator it = m_BCEntryStorage.upper_bound( RandomUInt(98)+1 );
		while( it!=m_BCEntryStorage.end() )
		{
			Entries.push_back(it->second);
			++it;
		}
		if ( Entries.empty() ) return 0;
		uint32 n = ( Entries.size()>1 ? RandomUInt(  (uint32)(Entries.size())-1  ) : 0);
		return Entries[n];
	}

	uint32  GetGuildCount() { return (uint32)mGuild.size(); }
	uint32  GetGroupCount() { return (uint32)m_groups.size(); }

    QuestPOIVector const* GetQuestPOIVector(uint32 questId)
    {
        QuestPOIMap::const_iterator itr = mQuestPOIMap.find(questId);
        if(itr != mQuestPOIMap.end())
            return &itr->second;
        return NULL;
    }
	void LoadQuestPOI();

	std::map<uint32,uint32>	VelumEnchantToScrollEntry;

//#ifdef USE_NEW_LOOT_SYSTEM
	//Need to load these before creature_proto so we can link them directly, or not ?
	//lists to store groups so we can destroy the lists at shutdown. Care ?
	std::map<uint32, SimpleVect< SimpleVect<LootStoreStruct*>* >* > m_reference_loot;
	std::map<uint32, SimpleVect< SimpleVect<LootStoreStruct*>* >* > m_creature_loot_kill;
	std::map<uint32, SimpleVect< SimpleVect<LootStoreStruct*>* >* > m_creature_loot_skin;
	std::map<uint32, SimpleVect< SimpleVect<LootStoreStruct*>* >* > m_creature_loot_pickpocket;
	void LoadLootsAny(const char *table_name,std::map<uint32, SimpleVect< SimpleVect<LootStoreStruct*>* >* > *cur_global_store);
	void ProcessReferenceLootTable();
	void LoadReferenceLoot(){			LoadLootsAny( "loot_reference", &m_reference_loot );				}
	void LoadLootsCreatureKill(){		LoadLootsAny( "loot_creature", &m_creature_loot_kill );				}
	void LoadLootsCreatureSkin(){		LoadLootsAny( "loot_skinning", &m_creature_loot_skin );				}
	void LoadLootsCreaturePickPocket(){	LoadLootsAny( "loot_pickpocketing", &m_creature_loot_pickpocket );	}
	void DestroyLootList( std::map<uint32, SimpleVect< SimpleVect<LootStoreStruct*>* >* > *cur_global_store );
//#endif

	std::map<uint32,uint32>	IP_mute_expires;		//only for some players this will work to protect GMs getting harassed by players
	void					AddIPMute( const char *IP, uint32 duration_seconds );
	void					RemoveIPMute( const char *IP );
	uint32					GetIPMuteExpire( const char *IP );
	void					LoadIPMute();

	SimpleVect< SimpleVect<LootCurencyStoreStruct*>* >	m_currency_loot; //store it here for the destructor
	void LoadLootsCurrency();

	//this is because we have tools to clear auras on players. They loose deserter buff and can rejoin
	ARCEMU_INLINE uint32	PlayerHasDeserterCooldown( const uint64 &GUID );
	ARCEMU_INLINE void		PlayerAddDeserterCooldown( const uint64 &GUID );
	//fucked up way to check for a deleted object. An active object is active on one of the existing maps
	ARCEMU_INLINE bool		IsActiveObject( Object *obj )  { return ( m_ActiveObjects.find( obj ) != m_ActiveObjects.end() ); }
	ARCEMU_INLINE void		AddActiveObject( Object *obj ) { m_ActiveObjects.insert( obj ); }
	ARCEMU_INLINE void		RemActiveObject( Object *obj ) { m_ActiveObjects.erase( obj ); }
	ARCEMU_INLINE bool		IsActivePlayer( Object *obj )  { return ( m_ActivePlayers.find( obj ) != m_ActivePlayers.end() ); }
	ARCEMU_INLINE void		AddActivePlayer( Object *obj ) { m_ActivePlayers.insert( obj ); }
	ARCEMU_INLINE void		RemActivePlayer( Object *obj ) { m_ActivePlayers.erase( obj ); }

	void OnArenaTeamJoinedQueue( ArenaTeam *team, Group *group );
	void ListArenaMatches( Player *p, uint32 TypeFilter, ArenaTeam	**buffer, uint32 BufferSlots );

#ifdef PVP_AREANA_REALM
	//custom addition to diminish PVP honor farming. It's player to player based
	uint32 GetIncreasePVPDiminishing( uint64 A1, uint64 A2, uint8 Increase );
	uint32 GetPVPGoldDiminishing( uint32 A1 );
	uint32 ModPVPGoldDiminishing( uint32 A1, uint32 GoldGain );
	void	ResetDailyPVPArenaTeamFarm();
	std::map<uint64,uint32>	m_PVPKillsTodayArenaTeamPair;
	std::map<uint32,uint32>	m_PVPGoldGainPlayer;
#endif

protected:
	std::set<Object*>		m_ActiveObjects;
	std::set<Object*>		m_ActivePlayers;
	QuestPOIMap				mQuestPOIMap;
	BCEntryStorage m_BCEntryStorage; // broadcast system.
	RWLock playernamelock;
	uint32 m_mailid;
	uint64 m_ticketid;
	// highest GUIDs, used for creating new objects
	Mutex m_guidGenMutex;
    union
    {
	    uint32 m_hiItemGuid;
	    uint32 m_hiContainerGuid;
    };
	uint32 m_hiGroupId;
	uint32 m_hiCharterId;
	RWLock m_charterLock;

	ReputationModMap m_reputation_faction;
	ReputationModMap m_reputation_creature;
	HM_NAMESPACE::hash_map<uint32, InstanceReputationModifier*> m_reputation_instance;
	HM_NAMESPACE::hash_map<uint32, SimpleVect<ReputationToReputationMod *>* > m_reputation_chains;

	//HM_NAMESPACE::hash_map<uint32, Charter*> m_charters[NUM_CHARTER_TYPES];
	tr1::array< HM_NAMESPACE::hash_map<uint32, Charter*>, NUM_CHARTER_TYPES> m_charters;
	
	HM_NAMESPACE::hash_map<uint32,PlayerInfo*> m_playersinfo;
	PlayerNameStringIndexMap m_playersInfoByName;
	
//	HM_NAMESPACE::hash_map<uint32,WayPointMap*> m_waypoints;//stored by spawnid
	HM_NAMESPACE::hash_map<uint32,WayPointVect*> m_waypoints;//stored by spawnid
	HM_NAMESPACE::hash_map<uint32,TimedEmoteList*> m_timedemotes;//stored by spawnid
	uint32 m_hiCreatureSpawnId;
	
	Mutex m_CreatureSpawnIdMutex;
	Mutex m_GOSpawnIdMutex;

	uint32 m_hiGameObjectSpawnId;
	
	///// Object Tables ////
	// These tables are modified as creatures are created and destroyed in the world

	// Group List
	RWLock m_groupLock;
	GroupMap m_groups;

	// Map of all starting infos needed for player creation
	PlayerCreateInfoMap mPlayerCreateInfo;

	// DK: Map of all Guild's
	GuildMap mGuild;

	// Map of all vendor goods
	VendorMap mVendors;

	// Maps for Gossip stuff
	NpcToGossipTextMap  mNpcToGossipText;

//	SLMap				mSpellSkills;

    TransportMap mTransports;

	ItemSetContentMap mItemSets;

	TrainerMap mTrainers;
//	LevelInfoMap mLevelInfo;
	PetDefaultSpellMap mDefaultPetSpells;

	Mutex	m_EquipmentSetIdMutex;
	uint32	m_hiEquipmentSetId;

	std::map<uint32,uint32>	m_PlayerArenaDeserterCooldown;
};


#define objmgr ObjectMgr::getSingleton()

//void SetProgressBar(int, int, const char*);
//void ClearProgressBar();

#endif
