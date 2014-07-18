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

#ifndef WOWSERVER_GAMEOBJECT_H
#define WOWSERVER_GAMEOBJECT_H

class Player;
class GameObjectAIScript;
class GameObjectTemplate;

#define GAMEOBJECT_BYTES_STATE 0
#define GAMEOBJECT_BYTES_TYPEID 1
#define GAMEOBJECT_BYTES_ARTKIT 2
#define GAMEOBJECT_BYTES_ANIMPROGRESS 3

struct GOQuestItem
{
	uint32 itemid;
	uint32 requiredcount;
};

struct GOQuestGameObject
{
	uint32 goid;
	uint32 requiredcount;
};

//!! depends on GO state !
// elevators have flags 8 + 32 to not let players fall through them
enum GAMEOBJECT_FLAG_BIT
{
	GAMEOBJECT_UNCLICKABLE = 0x01,
	GAMEOBJECT_CLICKABLE = 0x20,
};

// client side GO show states
enum GOState
{
	//anim progress finished and cannot click on it
    GO_STATE_ACTIVE             = 0,                        // show in world as used and not reset (closed door open)
	//gold mouse icon so you may activate it
    GO_STATE_READY              = 1,                        // show in world as ready (closed door close)
    GO_STATE_ACTIVE_ALTERNATIVE = 2                         // show in world as used in alt way and not reset (closed door open by cannon fire)
};

enum LockType
{
	GO_LOCKTYPE_NOLOCK					= 0,
	GO_LOCKTYPE_ITEM_REQ				= 1,
	GO_LOCKTYPE_SKILL_REQ				= 2,
};

#if ENABLE_SHITTY_STL_HACKS == 1
typedef HM_NAMESPACE::hash_map<Quest*, uint32 > GameObjectGOMap;
#else
namespace HM_NAMESPACE
{
	template <>
	struct hash<Quest*>
	{
		union __vp
		{
			size_t s;
			Quest* p;
		};

		size_t operator()(Quest* __x) const
		{
			__vp vp;
			vp.p = __x;
			return vp.s;
		}
	};
};

typedef HM_NAMESPACE::hash_map<Quest*, uint32, HM_NAMESPACE::hash<Quest*> > GameObjectGOMap;
#endif

typedef HM_NAMESPACE::hash_map<Quest*, std::map<uint32, uint32> > GameObjectItemMap;

#pragma pack(push,1)
struct GameObjectInfo
{
	GameObjectInfo() 
	{
		gossip_script = NULL;
	}
    union                                                   // different GO types have different data field
    {
		struct
		{
			uint32 ID;
			uint32 Type;
			uint32 DisplayID;
			char * Name;
			char * info_str;
			char * castbarcaption;
			uint32 SpellFocus;
			uint32 sound1;
			uint32 sound2;
			uint32 sound3;
			uint32 sound4;
			uint32 sound5;
			uint32 sound6;
			uint32 sound7;
			uint32 sound8;
			uint32 sound9;
			uint32 Unknown1;
			uint32 Unknown2;
			uint32 Unknown3;
			uint32 Unknown4;
			uint32 Unknown5;
			uint32 Unknown6;
			uint32 Unknown7;
			uint32 Unknown8;
			uint32 Unknown9;
			uint32 Unknown10;
			uint32 Unknown11;
			uint32 Unknown12;
			uint32 Unknown13;
			uint32 Unknown14;
			float  Scale;
			uint32 RelatedItemId1;
			uint32 RelatedItemId2;
			uint32 RelatedItemId3;
			uint32 RelatedItemId4;
			uint32 RelatedItemId5;
			uint32 RelatedItemId6;
			uint32 respawn_time;
		};
        //0 GAMEOBJECT_TYPE_DOOR
        struct
        {
            uint32 startOpen;                               //0 used client side to determine GO_ACTIVATED means open/closed
            uint32 lockId;                                  //1 -> Lock.dbc
            uint32 autoCloseTime;                           //2 secs till autoclose = autoCloseTime / 0x10000
            uint32 noDamageImmune;                          //3 break opening whenever you recieve damage?
            uint32 openTextID;                              //4 can be used to replace castBarCaption?
            uint32 closeTextID;                             //5
            uint32 ignoredByPathing;                        //6
        } door;
        //1 GAMEOBJECT_TYPE_BUTTON
        struct
        {
            uint32 startOpen;                               //0
            uint32 lockId;                                  //1 -> Lock.dbc
            uint32 autoCloseTime;                           //2 secs till autoclose = autoCloseTime / 0x10000
            uint32 linkedTrap;                              //3
            uint32 noDamageImmune;                          //4 isBattlegroundObject
            uint32 large;                                   //5
            uint32 openTextID;                              //6 can be used to replace castBarCaption?
            uint32 closeTextID;                             //7
            uint32 losOK;                                   //8
        } button;
        //2 GAMEOBJECT_TYPE_QUESTGIVER
        struct
        {
            uint32 lockId;                                  //0 -> Lock.dbc
            uint32 questList;                               //1
            uint32 pageMaterial;                            //2
            uint32 gossipID;                                //3
            uint32 customAnim;                              //4
            uint32 noDamageImmune;                          //5
            uint32 openTextID;                              //6 can be used to replace castBarCaption?
            uint32 losOK;                                   //7
            uint32 allowMounted;                            //8
            uint32 large;                                   //9
        } questgiver;
        //3 GAMEOBJECT_TYPE_CHEST
        struct
        {
            uint32 lockId;                                  //0 -> Lock.dbc
            uint32 lootId;                                  //1
            uint32 chestRestockTime;                        //2
            uint32 consumable;                              //3
            uint32 minSuccessOpens;                         //4
            uint32 maxSuccessOpens;                         //5
            uint32 eventId;                                 //6 lootedEvent
            uint32 linkedTrapId;                            //7
            uint32 questId;                                 //8 not used currently but store quest required for GO activation for player
            uint32 level;                                   //9
            uint32 losOK;                                   //10
            uint32 leaveLoot;                               //11
            uint32 notInCombat;                             //12
            uint32 logLoot;                                 //13
            uint32 openTextID;                              //14 can be used to replace castBarCaption?
            uint32 groupLootRules;                          //15
            uint32 floatingTooltip;                         //16
        } chest;
        //4 GAMEOBJECT_TYPE_BINDER - empty
        //5 GAMEOBJECT_TYPE_GENERIC
        struct
        {
            uint32 floatingTooltip;                         //0
            uint32 highlight;                               //1
            uint32 serverOnly;                              //2
            uint32 large;                                   //3
            uint32 floatOnWater;                            //4
            uint32 questID;                                 //5
        } _generic;
        //6 GAMEOBJECT_TYPE_TRAP
        struct
        {
            uint32 lockId;                                  //0 -> Lock.dbc
            uint32 level;                                   //1
            uint32 radius;                                  //2 radius for trap activation
            uint32 spellId;                                 //3
            uint32 charges;                                 //4 need respawn (if > 0)
            uint32 cooldown;                                //5 time in secs
            uint32 autoCloseTime;                           //6
            uint32 startDelay;                              //7
            uint32 serverOnly;                              //8
            uint32 stealthed;                               //9
            uint32 large;                                   //10
            uint32 stealthAffected;                         //11
            uint32 openTextID;                              //12 can be used to replace castBarCaption?
            uint32 closeTextID;                             //13
            uint32 ignoreTotems;                            //14
        } trap;
        //7 GAMEOBJECT_TYPE_CHAIR
        struct
        {
            uint32 slots;                                   //0
            uint32 height;                                  //1
            uint32 onlyCreatorUse;                          //2
            uint32 triggeredEvent;                          //3
        } chair;
        //8 GAMEOBJECT_TYPE_SPELL_FOCUS
        struct
        {
            uint32 focusId;                                 //0
            uint32 dist;                                    //1
            uint32 linkedTrapId;                            //2
            uint32 serverOnly;                              //3
            uint32 questID;                                 //4
            uint32 large;                                   //5
            uint32 floatingTooltip;                         //6
        } spellFocus;
        //9 GAMEOBJECT_TYPE_TEXT
        struct
        {
            uint32 pageID;                                  //0
            uint32 language;                                //1
            uint32 pageMaterial;                            //2
            uint32 allowMounted;                            //3
        } text;
        //10 GAMEOBJECT_TYPE_GOOBER
        struct
        {
            uint32 lockId;                                  //0 -> Lock.dbc
            uint32 questId;                                 //1
            uint32 eventId;                                 //2
            uint32 autoCloseTime;                           //3
            uint32 customAnim;                              //4
            uint32 consumable;                              //5
            uint32 cooldown;                                //6
            uint32 pageId;                                  //7
            uint32 language;                                //8
            uint32 pageMaterial;                            //9
            uint32 spellId;                                 //10
            uint32 noDamageImmune;                          //11
            uint32 linkedTrapId;                            //12
            uint32 large;                                   //13
            uint32 openTextID;                              //14 can be used to replace castBarCaption?
            uint32 closeTextID;                             //15
            uint32 losOK;                                   //16 isBattlegroundObject
            uint32 allowMounted;                            //17
            uint32 floatingTooltip;                         //18
            uint32 gossipID;                                //19
            uint32 WorldStateSetsState;                     //20
        } goober;
        //11 GAMEOBJECT_TYPE_TRANSPORT
        struct
        {
            uint32 pause;                                   //0
            uint32 startOpen;                               //1
            uint32 autoCloseTime;                           //2 secs till autoclose = autoCloseTime / 0x10000
            uint32 pause1EventID;                           //3
            uint32 pause2EventID;                           //4
        } transport;
        //12 GAMEOBJECT_TYPE_AREADAMAGE
        struct
        {
            uint32 lockId;                                  //0
            uint32 radius;                                  //1
            uint32 damageMin;                               //2
            uint32 damageMax;                               //3
            uint32 damageSchool;                            //4
            uint32 autoCloseTime;                           //5 secs till autoclose = autoCloseTime / 0x10000
            uint32 openTextID;                              //6
            uint32 closeTextID;                             //7
        } areadamage;
        //13 GAMEOBJECT_TYPE_CAMERA
        struct
        {
            uint32 lockId;                                  //0 -> Lock.dbc
            uint32 cinematicId;                             //1
            uint32 eventID;                                 //2
            uint32 openTextID;                              //3 can be used to replace castBarCaption?
        } camera;
        //14 GAMEOBJECT_TYPE_MAPOBJECT - empty
        //15 GAMEOBJECT_TYPE_MO_TRANSPORT
        struct
        {
            uint32 taxiPathId;                              //0
            uint32 moveSpeed;                               //1
            uint32 accelRate;                               //2
            uint32 startEventID;                            //3
            uint32 stopEventID;                             //4
            uint32 transportPhysics;                        //5
            uint32 mapID;                                   //6
            uint32 worldState1;                             //7
        } moTransport;
        //16 GAMEOBJECT_TYPE_DUELFLAG - empty
        //17 GAMEOBJECT_TYPE_FISHINGNODE - empty
        //18 GAMEOBJECT_TYPE_SUMMONING_RITUAL
        struct
        {
            uint32 reqParticipants;                         //0
            uint32 spellId;                                 //1
            uint32 animSpell;                               //2
            uint32 ritualPersistent;                        //3
            uint32 casterTargetSpell;                       //4
            uint32 casterTargetSpellTargets;                //5
            uint32 castersGrouped;                          //6
            uint32 ritualNoTargetCheck;                     //7
        } summoningRitual;
        //19 GAMEOBJECT_TYPE_MAILBOX - empty
        //20 GAMEOBJECT_TYPE_DONOTUSE - empty
        //21 GAMEOBJECT_TYPE_GUARDPOST
        struct
        {
            uint32 creatureID;                              //0
            uint32 charges;                                 //1
        } guardpost;
        //22 GAMEOBJECT_TYPE_SPELLCASTER
        struct
        {
            uint32 spellId;                                 //0
            uint32 charges;                                 //1
            uint32 partyOnly;                               //2
            uint32 allowMounted;                            //3
            uint32 large;                                   //4
        } spellcaster;
        //23 GAMEOBJECT_TYPE_MEETINGSTONE
        struct
        {
            uint32 minLevel;                                //0
            uint32 maxLevel;                                //1
            uint32 areaID;                                  //2
        } meetingstone;
        //24 GAMEOBJECT_TYPE_FLAGSTAND
        struct
        {
            uint32 lockId;                                  //0
            uint32 pickupSpell;                             //1
            uint32 radius;                                  //2
            uint32 returnAura;                              //3
            uint32 returnSpell;                             //4
            uint32 noDamageImmune;                          //5
            uint32 openTextID;                              //6
            uint32 losOK;                                   //7
        } flagstand;
        //25 GAMEOBJECT_TYPE_FISHINGHOLE                    // not implemented yet
        struct
        {
            uint32 radius;                                  //0 how close bobber must land for sending loot
            uint32 lootId;                                  //1
            uint32 minSuccessOpens;                         //2
            uint32 maxSuccessOpens;                         //3
            uint32 lockId;                                  //4 -> Lock.dbc; possibly 1628 for all?
        } fishinghole;
        //26 GAMEOBJECT_TYPE_FLAGDROP
        struct
        {
            uint32 lockId;                                  //0
            uint32 eventID;                                 //1
            uint32 pickupSpell;                             //2
            uint32 noDamageImmune;                          //3
            uint32 openTextID;                              //4
        } flagdrop;
        //27 GAMEOBJECT_TYPE_MINI_GAME
        struct
        {
            uint32 gameType;                                //0
        } miniGame;
        //29 GAMEOBJECT_TYPE_CAPTURE_POINT
        struct
        {
            uint32 radius;                                  //0
            uint32 spell;                                   //1
            uint32 worldState1;                             //2
            uint32 worldstate2;                             //3
            uint32 winEventID1;                             //4
            uint32 winEventID2;                             //5
            uint32 contestedEventID1;                       //6
            uint32 contestedEventID2;                       //7
            uint32 progressEventID1;                        //8
            uint32 progressEventID2;                        //9
            uint32 neutralEventID1;                         //10
            uint32 neutralEventID2;                         //11
            uint32 neutralPercent;                          //12
            uint32 worldstate3;                             //13
            uint32 minSuperiority;                          //14
            uint32 maxSuperiority;                          //15
            uint32 minTime;                                 //16
            uint32 maxTime;                                 //17
            uint32 large;                                   //18
            uint32 highlight;                               //19
            uint32 startingValue;                           //20
            uint32 unidirectional;                          //21
        } capturePoint;
        //30 GAMEOBJECT_TYPE_AURA_GENERATOR
        struct
        {
            uint32 startOpen;                               //0
            uint32 radius;                                  //1
            uint32 auraID1;                                 //2
            uint32 conditionID1;                            //3
            uint32 auraID2;                                 //4
            uint32 conditionID2;                            //5
            uint32 serverOnly;                              //6
        } auraGenerator;
        //31 GAMEOBJECT_TYPE_DUNGEON_DIFFICULTY
        struct
        {
            uint32 mapID;                                   //0
            uint32 difficulty;                              //1
        } dungeonDifficulty;
        //32 GAMEOBJECT_TYPE_BARBER_CHAIR
        struct
        {
            uint32 chairheight;                             //0
            uint32 heightOffset;                            //1
        } barberChair;
        //33 GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING
        struct
        {
            uint32 intactNumHits;                           //0
            uint32 creditProxyCreature;                     //1
            uint32 empty1;                                  //2
            uint32 intactEvent;                             //3
            uint32 empty2;                                  //4
            uint32 damagedNumHits;                          //5
            uint32 empty3;                                  //6
            uint32 empty4;                                  //7
            uint32 empty5;                                  //8
            uint32 damagedEvent;                            //9
            uint32 empty6;                                  //10
            uint32 empty7;                                  //11
            uint32 empty8;                                  //12
            uint32 empty9;                                  //13
            uint32 destroyedEvent;                          //14
            uint32 empty10;                                 //15
            uint32 debuildingTimeSecs;                      //16
            uint32 empty11;                                 //17
            uint32 destructibleData;                        //18
            uint32 rebuildingEvent;                         //19
            uint32 empty12;                                 //20
            uint32 empty13;                                 //21
            uint32 damageEvent;                             //22
            uint32 empty14;                                 //23
        } destructibleBuilding;
        //34 GAMEOBJECT_TYPE_GUILDBANK - empty
        //35 GAMEOBJECT_TYPE_TRAPDOOR
        struct
        {
            uint32 whenToPause;                             // 0
            uint32 startOpen;                               // 1
            uint32 autoClose;                               // 2
        } trapDoor;

        // not use for specific field access (only for output with loop by all filed), also this determinate max union size
        struct
        {
            uint32 data[24];
        } raw;
    };

	// Quests
	GameObjectGOMap goMap;
	GameObjectItemMap itemMap;
    GossipScript * gossip_script;
};
#pragma pack(pop)

enum GAMEOBJECT_TYPES
{
	GAMEOBJECT_TYPE_DOOR			   = 0,
	GAMEOBJECT_TYPE_BUTTON			 = 1,
	GAMEOBJECT_TYPE_QUESTGIVER		 = 2,
	GAMEOBJECT_TYPE_CHEST			  = 3,
	GAMEOBJECT_TYPE_BINDER			 = 4,
	GAMEOBJECT_TYPE_GENERIC			= 5,
	GAMEOBJECT_TYPE_TRAP			   = 6,
	GAMEOBJECT_TYPE_CHAIR			  = 7,
	GAMEOBJECT_TYPE_SPELL_FOCUS		= 8,
	GAMEOBJECT_TYPE_TEXT			   = 9,
	GAMEOBJECT_TYPE_GOOBER			 = 10,
	GAMEOBJECT_TYPE_TRANSPORT		  = 11,
	GAMEOBJECT_TYPE_AREADAMAGE		 = 12,
	GAMEOBJECT_TYPE_CAMERA			 = 13,
	GAMEOBJECT_TYPE_MAP_OBJECT		 = 14,
	GAMEOBJECT_TYPE_MO_TRANSPORT	   = 15,
	GAMEOBJECT_TYPE_DUEL_ARBITER	   = 16,
	GAMEOBJECT_TYPE_FISHINGNODE			= 17,
	GAMEOBJECT_TYPE_RITUAL			 = 18,
	GAMEOBJECT_TYPE_MAILBOX				= 19,
	GAMEOBJECT_TYPE_AUCTIONHOUSE	   = 20,
	GAMEOBJECT_TYPE_GUARDPOST		  = 21,
	GAMEOBJECT_TYPE_SPELLCASTER			= 22,
	GAMEOBJECT_TYPE_MEETINGSTONE	   = 23,
	GAMEOBJECT_TYPE_FLAGSTAND		  = 24,
	GAMEOBJECT_TYPE_FISHINGHOLE			= 25,
	GAMEOBJECT_TYPE_FLAGDROP				= 26,
	GAMEOBJECT_TYPE_MINI_GAME				= 27,
	GAMEOBJECT_TYPE_CAPTURE_POINT			= 29,
	GAMEOBJECT_TYPE_AURA_GENERATOR			= 30,
	GAMEOBJECT_TYPE_DUNGEON_DIFFICULTY		= 31,
	GAMEOBJECT_TYPE_BARBERCHAIR				 = 32,
	GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING	   = 33,	// !! this is invisible if used as platform !
	GAMEOBJECT_TYPE_GUILDBANK				= 34,
	GAMEOBJECT_TYPE_TRAPDOOR				= 35,
};

enum GameObjectFlagsy
{
	GO_FLAG_IN_USE			= 0x001,
	GO_FLAG_LOCKED			= 0x002,
	GO_FLAG_INTERACT_COND	= 0x004,
	GO_FLAG_TRANSPORT		= 0x008,
	GO_FLAG_UNK1			= 0x010,
	GO_FLAG_NODESPAWN		= 0x020,
	GO_FLAG_TRIGGERED		= 0x040,
	GO_FLAG_DAMAGED			= 0x200,
	GO_FLAG_DESTROYED		= 0x400,
};

enum GO_FLAGSY
{
	GO_FLAGS_NONE					= 0x0,
	GO_FLAGS_BUILDING_INTACT		= 0x20,
	GO_FLAGS_BUILDING_UNK			= 0x100,
	GO_FLAGS_BUILDING_DAMAGED		= 0x200,
	GO_FLAGS_BUILDING_BROKEN		= 0x400,
};

enum GOBYTES1_FLAGSY
{
	GOBYTES1_FLAG_CLICKABLE		= 0x1,
	GOBYTES1_FLAG_CAN_HIGHLIGHT	= 0x100,
	GOBYTES1_FLAG_SHOW_INFO		= 0x2000,

	// Destructible Buildings
	GOBYTES1_FULL_HEALTH		= 0xFF000000, // 100
	GOBYTES1_DMG1_HEALTH		= 0xE7000000, // 75
	GOBYTES1_DMG2_HEALTH		= 0xC7000000, // 60

	GOBYTES1_DEFAULT			= GOBYTES1_FLAG_SHOW_INFO | GOBYTES1_FLAG_CAN_HIGHLIGHT,
	GOBYTES1_DEFAULT_CLICKABLE	= GOBYTES1_DEFAULT | GOBYTES1_FLAG_CLICKABLE,
};

enum GameObjectDynamicLowFlags
{
    GO_DYNFLAG_LO_ACTIVATE          = 0x01,                 // enables interaction with GO
    GO_DYNFLAG_LO_ANIMATE           = 0x02,                 // possibly more distinct animation of GO
    GO_DYNFLAG_LO_NO_INTERACT       = 0x04,                 // appears to disable interaction (not fully verified)
    GO_DYNFLAG_LO_SPARKLE           = 0x08,                 // makes GO sparkle
};

enum GameObjectDestructibleState
{
    GO_DESTRUCTIBLE_INTACT      = 0,
    GO_DESTRUCTIBLE_DAMAGED     = 1,
    GO_DESTRUCTIBLE_DESTROYED   = 2,
    GO_DESTRUCTIBLE_REBUILDING  = 3,
}; 

#define CALL_GO_SCRIPT_EVENT(obj, func) if(obj->GetTypeId() == TYPEID_GAMEOBJECT && SafeGOCast(obj)->GetScript() != NULL) SafeGOCast(obj)->GetScript()->func

class SERVER_DECL GameObject : public Object
{
public:
	/************************************************************************/
	/* LUA Stuff                                                            */
	/************************************************************************/
/*	typedef struct { const char *name; int(*mfunc)(lua_State*,GameObject*); } RegType;
	static const char className[];
	static RegType methods[];

	// a lua script cannot create a unit.
	GameObject(lua_State * L) { ASSERT(false); }*/

	GameObject(uint64 guid);
	~GameObject( );
	virtual void Virtual_Destructor();

	ARCEMU_INLINE GameObjectInfo* GetInfo() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
		return pInfo; 
	}
	ARCEMU_INLINE void SetInfo(GameObjectInfo * goi) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
		pInfo = goi; 
	}

	//void Create ( uint32 display_id, uint8 state, uint32 obj_field_entry, float scale, uint16 type, uint16 faction, uint32 mapid, float x, float y, float z, float ang );
   // void Create ( uint32 mapid, float x, float y, float z, float ang);
	bool CreateFromProto(uint32 entry,uint32 mapid, float x, float y, float z, float ang, float r0= 0.0f, float r1= 0.0f, float r2= 0.0f, float r3= 0.0f);
   
	bool Load(GOSpawn *spawn);

	virtual void Update(uint32 p_time);

	void Spawn(MapMgr * m);
	void Despawn(uint32 respawntime);
	void Despawn(uint32 delay, uint32 respawntime);
	Loot loot;
	//void _EnvironmentalDamageUpdate();
	void UpdateTrapState();
	// Serialization
	void SaveToDB(const char *to_file_name = NULL);
	//bool LoadFromDB(uint32 guid);
	//void LoadFromDB(GameObjectTemplate *t);
	void DeleteFromDB();
	void EventCloseDoor();

	//Fishing stuff
	void UseFishingNode(Player *player);
	void EndFishingEvent(uint64 guid,bool abort);
	void EndFishing(Player *player,bool abort);
	void FishHooked(uint64 guid);
	
	// Quests
	void _LoadQuests();
	bool HasQuests() { return m_quests != NULL; };
	void AddQuest(QuestRelation *Q);
	void DeleteQuest(QuestRelation *Q);
	Quest *FindQuest(uint32 quest_id, uint8 quest_relation);
	uint16 GetQuestRelation(uint32 quest_id);
	uint32 NumOfQuests();
	std::list<QuestRelation *>::iterator QuestsBegin() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
		return m_quests->begin(); 
	}
	std::list<QuestRelation *>::iterator QuestsEnd() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
		return m_quests->end(); 
	}
	void SetQuestList(std::list<QuestRelation *>* qst_lst) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
		m_quests = qst_lst; 
	}

	void SetSummoned(Unit *mob)
	{
		INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
//		m_summoner = mob;
		SetUInt64Value( OBJECT_FIELD_CREATED_BY, mob->GetGUID() );
//		m_summonedGo = true;
	}
	void _Expire();
	
	void ExpireAndDelete();

	void Deactivate();

	ARCEMU_INLINE bool isQuestGiver()
	{
		INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
		if(GetByte(GAMEOBJECT_BYTES_1,GAMEOBJECT_BYTES_TYPEID) == 2)
			return true;
		else
			return false;
	}

	/// Quest data
	std::list<QuestRelation *>* m_quests;
   
	uint64 *m_ritualmembers;
	uint64 m_ritualcaster,m_ritualtarget;
	uint16 m_ritualspell;

	void InitAI();
	void ConvertToCaster( uint32 SpellId, float Radius=-1, uint32 CheckRate=10,int32 Charges=-1, int32 RespawnTime=-1);
	SpellEntry* spell;
	
	float	rangeSQ;
	uint8	checkrate;
	uint16	counter;
	int32	charges,ChargesInitial;//used for type==22,to limit number of usages.
	int32	RespawnCaster;
	bool	invisible;//invisible
	uint8	invisibilityFlag;
//	Unit*	m_summoner;// object can deactivate and this will become invalid pointer
	int8	bannerslot;
	int8	bannerauraslot;
	CBattleground * m_battleground;

	void CallScriptUpdate();
   

	ARCEMU_INLINE GameObjectAIScript* GetScript() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
		return myScript; 
	}

	void TrapSearchTarget();	// Traps need to find targets faster :P

	ARCEMU_INLINE bool HasAI() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
		return spell != 0; 
	}
	GOSpawn * m_spawn;
	void OnPushToWorld();
	void OnRemoveInRangeObject(Object* pObj);
	void RemoveFromWorld(bool free_guid);

/*	ARCEMU_INLINE bool CanMine()
	{
		INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
		return (usage_remaining > 0);
	}
	ARCEMU_INLINE void UseMine()
	{ 
		INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
		if(usage_remaining) usage_remaining--;
	}
	void CalcMineRemaining(bool force)
	{
		INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
//		if(force || !usage_remaining)
//			usage_remaining = GetInfo()->sound4 + RandomUInt(GetInfo()->sound5 - GetInfo()->sound4) - 1;
		usage_remaining = 0;
	}
	ARCEMU_INLINE bool CanFish() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
		return ( usage_remaining > 0 ); 
	}
	ARCEMU_INLINE void CatchFish() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
		if ( usage_remaining ) usage_remaining--; 
	}
	void CalcFishRemaining( bool force )
	{
		INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
		if ( force || !usage_remaining )
			usage_remaining = GetInfo()->sound2 + RandomUInt( GetInfo()->sound3 - GetInfo()->sound2 ) - 1;
	} */
	ARCEMU_INLINE void CalcMineRemaining(bool force){}
	ARCEMU_INLINE bool CanFish() { 	return true; 	}
	ARCEMU_INLINE void CatchFish() { }
	ARCEMU_INLINE void CalcFishRemaining( bool force ){} 
	bool	HasLoot();
	uint32	GetGOReqSkill();
	//if we get deleted by gm while we are not on map. Then make sure we delete the respawn event also 
	//!! cell can get deleted while creature is still sleeping to get spawned
	MapCell *m_respawnCell;			
	bool	m_noRespawn;				//it means some mapcell has a reference to us
	void	OnPrePushToWorld();
	void	UpdateGoRotationXFromOrientation();
	uint64	m_rotationX;	//actually this is not around X, it is making X facing
//	void GameObject::SetRotation(float rad);
//	void GameObject::UpdateRotation();

	void SetParentRotation( uint8 rot, float value ) { SetFloatValue(GAMEOBJECT_PARENTROTATION+rot, value); }
	float GetParentRotation( uint8 rot ) { return GetFloatValue(GAMEOBJECT_PARENTROTATION+rot); }

	ARCEMU_INLINE void SetFaction( uint32 id ) 
	{ 
		SetUInt32Value(GAMEOBJECT_FACTION, id); 
		_setFaction();
	}
	ARCEMU_INLINE uint32 GetFaction() { return GetUInt32Value(GAMEOBJECT_FACTION); }

	//Destructable Building
	uint32 Health;
	void TakeDamage(uint32 ammount, Object* mcaster, Object* pcaster, uint32 spellid = 0);
	void Rebuild();
	void SetTrapArmDuration( uint32 Dur ) { m_Arm_At_Stamp = getMSTime() + Dur; }

	void TriggerScriptEventStr(string func);
protected:

	uint32 m_Arm_At_Stamp;
//	bool m_summonedGo;
	bool m_deleted;
	GameObjectInfo *pInfo;
	GameObjectAIScript * myScript;
	uint32 _fields[GAMEOBJECT_END];
//	uint32 usage_remaining; //used for mining to mark times it can be mined
};

#endif



