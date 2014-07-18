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

#ifndef WOWSERVER_CREATURE_H
#define WOWSERVER_CREATURE_H

class CreatureTemplate;
class GossipScript;

#define MAX_CREATURE_ITEMS 128
#define MAX_CREATURE_LOOT 8
#define MAX_PET_SPELL 4
#define VENDOR_ITEMS_UPDATE_TIME 3600000
#include "Map.h"

#define VENDOR_SLOT_TYPE_ITEM		1
#define VENDOR_SLOT_TYPE_CURRENCY	2

struct CreatureItem
{
	uint32	itemid;
	uint32	buy_amount; //!!!!! stack amount.
	uint32	available_amount;
	int32	max_amount;
	uint32	incrtime;
	ItemExtendedCostEntry * extended_cost;
	uint32	item_vendor_slot; //required to be sent back on buy 
	uint8	item_or_currency;	//1 item 2 currency for client
};

enum CreatureAISpellFlags
{
	CREATURE_AI_FLAG_NONE				= 0x00,
	CREATURE_AI_FLAG_RANDOMCAST			= 0x01,
	CREATURE_AI_FLAG_CASTOUTOFCOMBAT	= 0x02,
	CREATURE_AI_FLAG_PLAYERGCD			= 0x04
};

#pragma pack(push,1)
struct CreatureInfo
{
	uint32 Id;
	char * Name;
	char * SubName;
	char * info_str;
	uint32 Flags1;
	uint32 unk420;	//very rare, can see it 3 times
	uint32 Type;
	uint32 Family;
	uint32 Rank;
	uint32 killcredit[2];
	uint32 Male_DisplayID;
	uint32 Female_DisplayID;
	uint32 Male_DisplayID2;
	uint32 Female_DisplayID2;
	float base_HP_mod;		
	float base_MP_mod;
	uint8  Leader;
	tr1::array< uint32,6> quest_item;
	uint32 creature_movetype;
	uint32 unk403;	//very rare, can see it 2 times
//	uint8  Civilian;

	std::string lowercase_name;
	GossipScript * gossip_script;
	uint32 GenerateModelId(uint32 * des)
	{
		uint32 models[] = { Male_DisplayID, Male_DisplayID2, Female_DisplayID, Female_DisplayID2 };
		if(!models[0] && !models[1] && !models[2] && !models[3])
		{
			*des = 0;
			// All models are invalid.
			Log.Notice("CreatureSpawn", "All model IDs are invalid for creature %u", Id);
			return 0;
		}

		while(true)
		{
			uint32 res = RandomUInt(3);
			if(models[res])
			{
				*des = models[res];
				return res < 2 ? 0 : 1;
			}
		}
	}
	bool IsExotic()
	{
		//worms : 42
		//rhino : 43
		//Shale Spider : 55
		if( Family == 38 || Family == 39 || Family == 41 || Family == 42 || Family == 43 || Family == 45 || Family == 46 || Family == 55 || Family == 59 )
			return true;
		return false;
	}
};

struct LootStoreStruct;
struct LootCurencyStoreStruct;

class CreatureProto
{
public:
	CreatureProto() { on_click_spell = 0; vehicleId = 0; vehicle_use_required_quest = 0; loot_currency = NULL; }
	uint32 Id;
	uint32 MinLevel;
	uint32 MaxLevel;
	uint32 Faction;
	uint32 MinHealth;
	uint32 MaxHealth;
	uint32 Mana;
	float  Scale;
	uint32	NPCFLags;
	uint32 AttackTime;
	uint32 AttackType;
	float MinDamage;
	float MaxDamage;
	uint32 RangedAttackTime;
	float RangedMinDamage;
	float RangedMaxDamage;
	uint32 RespawnTime;
	tr1::array<uint32,7>Resistances;
	float CombatReach;
	float BoundingRadius;
	char * aura_string;
	uint32 boss;
	uint32 money;
	uint32 invisibility_type;
	uint32 death_state;
	float	walk_speed;//base movement
	float	run_speed;//most of the time mobs use this
	float	fly_speed;
	uint32	extra_a9_flags;
	float	hoverheight;	//this is so rare that i'm not sure if this is spawn based or proto based. I guess it is proto. Like valkirie is floating with +2 compared to default. 
	uint32	modImmunities;
	uint32	loot_kill_id;
	uint32	loot_pickpocket_id;
	uint32	loot_skin_id;
	uint32	heroic_mode_proto_redirects[3];	//atm only 3 redirects
	uint32	vehicleId;						//row ID of vehocle DBC

	/* AI Stuff */
	bool m_canRangedAttack;
	bool m_canFlee;
	float m_fleeHealth;
	uint32 m_fleeDuration;
	bool m_canCallForHelp;
	float m_callForHelpHealth;

	set<uint32> start_auras;
	list<AI_Spell*> spells;
	uint32	on_click_spell;
	uint8   on_click_spell_caster_is_player;
	uint8   on_click_spell_target_is_clicker;
	uint32	vehicle_use_required_quest;			//in order to use this vehicle you need this active quest

	SimpleVect< SimpleVect<LootStoreStruct*>* >	*loot_kill;						//loot group list ID when he died
	SimpleVect< SimpleVect<LootStoreStruct*>* >	*loot_skin;						//loot group list ID when he is profession skinned
	SimpleVect< SimpleVect<LootStoreStruct*>* >	*loot_pickpocket;				//loot group list ID when NPC is pickpocketed
	SimpleVect< LootCurencyStoreStruct* >		*loot_currency;					//simple list of possible currency loots
};

struct ParentInfo
{
	uint32 spawnid;
	uint32 parentspawnid;
	float offsetX;
	float offsetY;
	float offsetZ;
	float orientation;
	uint32 flags;
	uint8 seatid;
};

#pragma pack(pop)


struct Formation{
	uint32 fol;
	float ang;
	float dist;
};
 
   
enum UNIT_TYPE
{
	NOUNITTYPE	  = 0,
	BEAST		   = 1,
	DRAGONSKIN	  = 2,
	DEMON		   = 3,
	ELEMENTAL	   = 4,
	GIANT		   = 5,
	UNDEAD		  = 6,
	HUMANOID		= 7,
	CRITTER		 = 8,
	MECHANICAL	  = 9,
	UNIT_TYPE_MISC  = 10,
	TOTEM  = 11,
	NON_COMBAT_PET  = 12,
	GAS_CLOUD  = 13,
	//found even type 14 in 3.1.2
};

enum CreatureFlag1
{
	CREATURE_FLAG1_TAMEABLE   = 0x0001,
	CREATURE_FLAG1_HERBLOOT   = 0x0100,
	CREATURE_FLAG1_MININGLOOT = 0x0200,
	CREATURE_FLAG1_FIGHT_MOUNTED = 0x0800,
	CREATURE_FLAG1_ENGINEERLOOT = 0x08000,
};

enum FAMILY
{
	FAMILY_WOLF = 1,
	FAMILY_CAT,
	FAMILY_SPIDER,
	FAMILY_BEAR,
	FAMILY_BOAR,
	FAMILY_CROCILISK,
	FAMILY_CARRION_BIRD,
	FAMILY_CRAB,
	FAMILY_GORILLA,
	FAMILY_SPECIAL_MOUNT,
	FAMILY_RAPTOR = 11,
	FAMILY_TALLSTRIDER ,
	FAMILY_FELHUNTER = 15,
	FAMILY_VOIDWALKER,
	FAMILY_SUCCUBUS,
	FAMILY_DOOMGUARD = 19,
	FAMILY_SCORPID,
	FAMILY_TURTLE, 
	FAMILY_IMP = 23,
	FAMILY_BAT,
	FAMILY_HYENA,
	FAMILY_OWL,
	FAMILY_WIND_SERPENT,
	FAMILY_REMOTE_CONTROL,
	FAMILY_FELGUARD,
	FAMILY_DRAGONHAWK,
	FAMILY_RAVAGER,
	FAMILY_WARP_STALKER,
	FAMILY_SPOREBAT,
	FAMILY_NETHER_RAY,
	FAMILY_SERPENT,
	FAMILY_MOTH = 37,
	FAMILY_CHIMERA,
	FAMILY_DEVILSAUR,
	FAMILY_GHOUL,
	FAMILY_RAVAGER2,
	FAMILY_WORM,
	FAMILY_RHINO,
	FAMILY_WASP,
	FAMILY_CORE_HOUND,
	FAMILY_SPIRIT_BEAST,
    FAMILY_WATER_ELEMENTAL = 49,
    FAMILY_FOX             = 50,
    FAMILY_MONKEY          = 51,
    FAMILY_DOG             = 52,
    FAMILY_BEETLE          = 53,
    FAMILY_SHALE_SPIDER    = 55,
    FAMILY_ZOMBIE          = 56,
    FAMILY_BEETLE_ZZOLD    = 57,
    FAMILY_SILITHID2       = 59,
    FAMILY_WASP2           = 66,
    FAMILY_HYDRA           = 68,
};

enum ELITE
{
	ELITE_NORMAL = 0,
	ELITE_ELITE,
	ELITE_RAREELITE,
	ELITE_WORLDBOSS,
	ELITE_RARE
};
enum TIME_REMOVE_CORPSE
{
	TIME_CREATURE_REMOVE_CORPSE = 180000,
	TIME_CREATURE_REMOVE_RARECORPSE = 180000*3,
	TIME_CREATURE_REMOVE_BOSSCORPSE = 180000*5,
};

struct PetSpellCooldown
{
	uint32 spellId;
	int32 cooldown;
};

class CreatureAIScript;
class GossipScript;
class AuctionHouse;
struct Trainer;
#define CALL_SCRIPT_EVENT(obj, func) if(obj->IsCreature() && SafeCreatureCast(obj)->GetScript() != NULL) SafeCreatureCast(obj)->GetScript()->func

///////////////////
/// Creature object

class SERVER_DECL Creature : public Unit
{
public:

	Creature(uint64 guid);
	virtual ~Creature();
	virtual void Virtual_Destructor();
    virtual bool Load(CreatureSpawn *spawn, uint32 mode, MapInfo *info);
	virtual void Load(CreatureProto * proto_, float x, float y, float z, float o=0.0f);

	void AddToWorld();
	void AddToWorld(MapMgr * pMapMgr);
	void RemoveFromWorld(bool addrespawnevent, bool free_guid);
	void RemoveFromWorld(bool free_guid)	//leave this like this so we may override virtual function call
	{
		INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
		RemoveFromWorld( true, free_guid );	//respawn event should be handled automatically
	}

	/// Creation
	void Create ( const char* creature_name, uint32 mapid, float x, float y, float z, float ang);	
	void CreateWayPoint ( uint32 WayPointID, uint32 mapid, float x, float y, float z, float ang);
	

	/// Updates
	virtual void Update( uint32 time );

	/// Creature inventory
	ARCEMU_INLINE uint32 GetItemIdBySlot(uint32 slot) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
		return m_SellItems->at(slot).itemid; 
	}
	ARCEMU_INLINE uint32 GetItemBuyAmountBySlot(uint32 slot) 
	{
		INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
		return m_SellItems->at(slot).buy_amount; 
	}

	ARCEMU_INLINE bool HasItems() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
		return ((m_SellItems != NULL) ? true : false); 
	}
	ARCEMU_INLINE CreatureProto* GetProto() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
		return proto; 
	}

	int32 GetSlotByItemId(uint32 itemid)
	{
		INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
		uint32 slot = 0;
		for(std::vector<CreatureItem>::iterator itr = m_SellItems->begin(); itr != m_SellItems->end(); ++itr)
		{
			if(itr->itemid == itemid)
				return slot;
			else
				++slot;			
		}
		return -1;
	}

	uint32 GetItemBuyAmountByItemId(uint32 itemid)
	{
		INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
		for(std::vector<CreatureItem>::iterator itr = m_SellItems->begin(); itr != m_SellItems->end(); ++itr)
		{
			if(itr->itemid == itemid)
				return ((itr->buy_amount < 1) ? 1 : itr->buy_amount);
		}
		return 0;
	}

	ARCEMU_INLINE void GetSellItemBySlot(uint32 slot, CreatureItem &ci)
	{
		INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
		ci = m_SellItems->at(slot);
	}

	void GetSellItemByItemId(uint32 itemid, CreatureItem &ci)
	{
		INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
		for(std::vector<CreatureItem>::iterator itr = m_SellItems->begin(); itr != m_SellItems->end(); ++itr)
		{
			if(itr->itemid == itemid)
			{
				ci = (*itr);   
				return;
			}
		}
		ci.buy_amount = 0;
		ci.max_amount=0;
		ci.available_amount =0;
		ci.incrtime=0;
		ci.itemid = 0;
	}

	ItemExtendedCostEntry * GetItemExtendedCostByItemId( uint32 itemid )
	{
		INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
		for( std::vector<CreatureItem>::iterator itr = m_SellItems->begin(); itr != m_SellItems->end(); ++itr )
		{
			if ( itr->itemid == itemid )
				return itr->extended_cost;
		}
		return NULL;
	}

	ARCEMU_INLINE std::vector<CreatureItem>::iterator GetSellItemBegin() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
		return m_SellItems->begin(); 
	}
	ARCEMU_INLINE std::vector<CreatureItem>::iterator GetSellItemEnd()   { return m_SellItems->end(); }
	ARCEMU_INLINE size_t GetSellItemCount() { return m_SellItems->size(); }
	void RemoveVendorItem(uint32 itemid)
	{
		INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
		for(std::vector<CreatureItem>::iterator itr = m_SellItems->begin(); itr != m_SellItems->end(); ++itr)
		{
			if(itr->itemid == itemid)
			{
				m_SellItems->erase(itr);
				return;
			}
		}
	}
	void AddVendorItem( uint32 itemid, uint32 amount, ItemExtendedCostEntry * ec );
	void ModAvItemAmount(uint32 itemid, uint32 value);
	void UpdateItemAmount(uint32 itemid);
	/// Quests
	void _LoadQuests();
	bool HasQuests() { return m_quests != NULL; };
	bool HasQuest(uint32 id, uint32 type)
	{
		INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
		if(!m_quests) return false;
		for(std::list<QuestRelation*>::iterator itr = m_quests->begin(); itr != m_quests->end(); ++itr)
		{
			if((*itr)->qst->id == id && (*itr)->type & type)
				return true;
		}
		return false;
	}
	void AddQuest(QuestRelation *Q);
	void DeleteQuest(QuestRelation *Q);
	Quest *FindQuest(uint32 quest_id, uint8 quest_relation);
	uint16 GetQuestRelation(uint32 quest_id);
	uint32 NumOfQuests();
	list<QuestRelation *>::iterator QuestsBegin() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
		return m_quests->begin(); 
	}
	list<QuestRelation *>::iterator QuestsEnd() { return m_quests->end(); };
	void SetQuestList(std::list<QuestRelation *>* qst_lst) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
		m_quests = qst_lst; 
	}

	ARCEMU_INLINE bool isVendor()		const { return HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_VENDOR ); }
	ARCEMU_INLINE bool isTrainer()		const { return HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_TRAINER ); }
	ARCEMU_INLINE bool isProf()			const { return HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_TRAINER_PROF ); }
	ARCEMU_INLINE bool isQuestGiver()	const { return HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER ); }
	ARCEMU_INLINE bool isGossip()		const { return HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP ); }
	ARCEMU_INLINE bool isTaxi()			const { return HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_TAXIVENDOR ); }
	ARCEMU_INLINE bool isCharterGiver()	const { return HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_ARENACHARTER ); }
	ARCEMU_INLINE bool isGuildBank()	const { return HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GUILD_BANK ); }
	ARCEMU_INLINE bool isBattleMaster()	const { return HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_BATTLEFIELDPERSON ); }
	ARCEMU_INLINE bool isBanker()		const { return HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_BANKER ); }
	ARCEMU_INLINE bool isInnkeeper()	const { return HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_INNKEEPER ); }
	ARCEMU_INLINE bool isSpiritHealer()	const { return HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPIRITHEALER ); }
	ARCEMU_INLINE bool isTabardDesigner() const { return HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_TABARDCHANGER ); }
	ARCEMU_INLINE bool isAuctioner()	const { return HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_AUCTIONEER ); }
	ARCEMU_INLINE bool isStableMaster()	const { return HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_STABLEMASTER ); }
	ARCEMU_INLINE bool isArmorer()		const { return HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_ARMORER ); }
    ARCEMU_INLINE bool isServiceProvider() const
        {
            return HasFlag( UNIT_NPC_FLAGS,
                UNIT_NPC_FLAG_VENDOR | UNIT_NPC_FLAG_TRAINER | UNIT_NPC_FLAG_FLIGHTMASTER |
                UNIT_NPC_FLAG_PETITIONER | UNIT_NPC_FLAG_BATTLEMASTER | UNIT_NPC_FLAG_BANKER |
                UNIT_NPC_FLAG_INNKEEPER | UNIT_NPC_FLAG_GUARD | UNIT_NPC_FLAG_SPIRITHEALER |
                UNIT_NPC_FLAG_SPIRITGUIDE | UNIT_NPC_FLAG_TABARDDESIGNER | UNIT_NPC_FLAG_AUCTIONEER );
        }
    ARCEMU_INLINE bool isSpiritService() const { return HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPIRITHEALER | UNIT_NPC_FLAG_SPIRITGUIDE ); }
	
	ARCEMU_INLINE uint32 GetHealthFromSpell() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
		return m_healthfromspell; 
	}
	void SetHealthFromSpell(uint32 value) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
		m_healthfromspell = value;
	}
	int32 m_speedFromHaste;

	//int32 FlatResistanceMod[7];
	tr1::array<int32,7> FlatResistanceMod;
	//int32 BaseResistanceModPct[7];
	tr1::array<int32,7> BaseResistanceModPct;
	//int32 ResistanceModPct[7];
	tr1::array<int32,7> ResistanceModPct;
	
	//int32 FlatStatMod[5];
	tr1::array<int32,5> FlatStatMod;
	//int32 StatModPct[5];
	tr1::array<int32,5> StatModPct;
	//int32 TotalStatModPct[5];
	tr1::array<int32,5> TotalStatModPct;
	
	//int32 ModDamageDone[7];
	tr1::array<int32,7> ModDamageDone;
	//float ModDamageDonePct[7];
	tr1::array<int32,7> ModDamageDonePct;
	void CalcResistance(uint32 type);
	void CalcStat(uint32 type);
	
	bool m_canRegenerateHP;
	void RegenerateHealth();
	void RegenerateMana();
	int BaseAttackType;

	bool CanSee(Unit* obj); // * Invisibility & Stealth Detection - Partha *

	//Make this unit face another unit
	bool setInFront(Unit* target);

	/// Looting
	bool generateLoot();
	
	bool Skinned;

//	bool Tagged;
	void	*TaggerGroupId;	//use this as value and not as pointer !
	uint64	TaggerGuid;

	/// Misc
	ARCEMU_INLINE void setEmoteState(uint8 emote) { m_emoteState = emote; };
	ARCEMU_INLINE uint32 GetSQL_id() { return spawnid; };

	virtual void setDeathState(DeathState s);

	uint32 GetOldEmote() { return m_oldEmote; }

	// Serialization
	void SaveToDB();
	void SaveToFile(std::stringstream & name);
	//bool LoadFromDB(uint32 guid);
	//bool LoadFromDB(CreatureTemplate *t);
	void DeleteFromDB();

	void OnJustDied();
	void OnRemoveCorpse();
	void OnRespawn(MapMgr * m);
	void SafeDelete();
	//void Despawn();
	void SummonExpire(); // this is used for guardians. They are non respawnable creatures linked to a player


	// In Range
//	void AddInRangeObject(Object* pObj);
	void QueueRemoveInRangeObject(Object* pObj);
//	void ClearInRangeSet();

	// Demon
	void EnslaveExpire();

	// Pet
	void UpdatePet();
	uint32 GetEnslaveCount() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
		return m_enslaveCount; 
	}
	void SetEnslaveCount(uint32 count) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
		m_enslaveCount = count; 
	}
	uint32 GetEnslaveSpell() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
		return m_enslaveSpell; 
	}
	void SetEnslaveSpell(uint32 spellId) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
		m_enslaveSpell = spellId; 
	}
	int32 RemoveEnslave();

	ARCEMU_INLINE Player *GetTotemOwner() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
		return totemOwner; 
	}
	ARCEMU_INLINE void SetTotemOwner(Player *owner) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
		totemOwner = owner; 
	}
	ARCEMU_INLINE uint32 GetTotemSlot() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
		return totemSlot; 
	}
	ARCEMU_INLINE void SetTotemSlot(uint32 slot) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
		totemSlot = slot; 
	}

	ARCEMU_INLINE bool IsPickPocketed() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
		return m_PickPocketed; 
	}
	ARCEMU_INLINE void SetPickPocketed(bool val = true) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
		m_PickPocketed = val; 
	}

	ARCEMU_INLINE CreatureAIScript * GetScript() 
	{ 
		return _myScriptClass;
	}
	ARCEMU_INLINE void SetScript(CreatureAIScript *new_script) 
	{ 
		_myScriptClass = new_script;
	}
	void LoadScript();

	void CallScriptUpdate();

	uint32 m_TaxiNode;
	
	ARCEMU_INLINE CreatureInfo *GetCreatureInfo() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
		return creature_info; 
	}
	ARCEMU_INLINE void SetCreatureInfo(CreatureInfo *ci) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
		creature_info = ci; 
	}
	ARCEMU_INLINE Trainer* GetTrainer() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
		return mTrainer; 
	}
	void RegenerateFocus();
	void RegenerateEnergy();

	CreatureFamilyEntry * myFamily;
	ARCEMU_INLINE bool IsTotem() 
	{ 
		return totemOwner != 0 && totemSlot != -1; 
	}
	void TotemExpire();
	void FormationLinkUp(uint32 SqlId);
//	void ChannelLinkUpGO(uint32 SqlId);
//	void ChannelLinkUpCreature(uint32 SqlId);
	bool haslinkupevent;
	WayPoint * CreateWaypointStruct();
	uint32 spawnid;
	uint32 original_emotestate;
	
	CreatureSpawn * m_spawn;

//	void AISpellUpdate();

	void OnPrePushToWorld();
	void OnPushToWorld();
	void Despawn(uint32 delay, uint32 respawntime);
	void RecastProtoAuras();

	void TriggerScriptEventStr(string func);
	void TriggerScriptEvent(int);

	AuctionHouse * auctionHouse;
	bool has_waypoint_text;
	bool has_combat_text;
	int8 m_lootMethod;

	void DeleteMe();
	bool CanAddToWorld();

	// scriptdev2
	uint32 GetNpcTextId();

	WayPointMap * m_custom_waypoint_map; //for backward compatibility
	Player * m_escorter;
	void DestroyCustomWaypointMap();
	bool IsInLimboState() { return m_limbostate; }
	uint32 GetLineByFamily(CreatureFamilyEntry * family)
	{
		INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE
		return family->skilline ? family->skilline : 0;
	}
	void RemoveLimboState(Unit * healer);
	void SetGuardWaypoints();
	bool m_corpseEvent;
	//if for some reason the creature gets deleted while it is respawn pending. It would spawn a deleted object
	//!! cell can get deleted while creature is still sleeping to get spawned
	MapCell *m_respawnCell;		
	bool	m_noRespawn;				//it means some mapcell has a reference to us
	uint32	m_respawnTimeOverride;
	LocationVector * m_transportPosition;
	uint32 m_transportGuid;

	float GetBaseParry();
	/************************************************************************/
	/* Vehicles                                                             */
	/************************************************************************/

	// Initialize parent info
	ARCEMU_INLINE void SetParentInfo(ParentInfo * info) { m_parentInfo = info; }
	// Resets parent info
	ARCEMU_INLINE void ResetParentInfo() { memset(&m_parentInfo, 0, sizeof(ParentInfo)); }
	// Retrieve parent information
	ARCEMU_INLINE ParentInfo * GetParentInfo() { return m_parentInfo; }
	// Retrieve parent guid
	ARCEMU_INLINE const uint32 GetParentSpawnID() const { return m_parentInfo ? m_parentInfo->parentspawnid : 0; }
protected:
	CreatureAIScript *_myScriptClass;
	bool m_limbostate;
	Trainer* mTrainer;

	void _LoadGoods();
	void _LoadGoods(std::list<CreatureItem*>* lst);
	void _LoadMovement();

	/// Vendor data
	std::vector<CreatureItem>* m_SellItems;

	/// Taxi data
	uint32 mTaxiNode;

	/// Quest data
	std::list<QuestRelation *>* m_quests;
   
	/// Pet
	uint32 m_enslaveCount;
	uint32 m_enslaveSpell;

	Player * totemOwner;
	int32 totemSlot;

	bool m_PickPocketed;
	uint32 _fields[UNIT_END];
	uint32 m_healthfromspell;

	CreatureInfo *creature_info;
	CreatureProto *proto;

	LocationVector m_spawnLocation;
public:
	CreatureProto	*DificultyProto;
	float			crit_chance_mod;	//not pct !
	ParentInfo		*m_parentInfo;
	//threath related
	uint32	m_HighestThreath;			//we will monitor this to know when to resend the treath list
	uint32	m_NextThreathUpdateStamp;	//once a mob is attacked threath will change all the time. Let's not spam too much

	ARCEMU_INLINE const float& GetSpawnX( ) const { return m_spawnLocation.x; }
	ARCEMU_INLINE const float& GetSpawnY( ) const { return m_spawnLocation.y; }
	ARCEMU_INLINE const float& GetSpawnZ( ) const { return m_spawnLocation.z; }
	ARCEMU_INLINE const float& GetSpawnO( ) const { return m_spawnLocation.o; }
	void SetSpawnPosition(float newX, float newY, float newZ, float newOrientation);
	void SetSpawnPosition(const LocationVector & v);
};

//definition is in NPCHandler.cpp
bool CanTrainAt(Player * plr, Trainer * trn);

#endif
