/*
 * ArcEmu MMORPG Server
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

#ifndef __SPELLSTORE_H
#define __SPELLSTORE_H

#include "Common.h"
#include "DataStore.h"
#include "Timer.h"

enum SPELL_EFFECT_OVERRIDE_RETURNS;
class Player;

#pragma pack(push,1)

struct WorldSafeLocsEntry
{
	uint32	ID;
	uint32	map;
	float	x,y,z;
	char	*name;
};

struct LFGDungeonEntry
{
    uint32  ID;                                             // 0
    //char*   name[16];                                     // 1-17 Name lang
	//uint32	name_flags;
    uint32  minlevel;                                       // 18
    uint32  maxlevel;                                       // 19
    uint32  reclevel;                                       // 20
    uint32  recminlevel;                                    // 21
    uint32  recmaxlevel;                                    // 22
    int32	map_id;                                         // 23
    uint32  difficulty;                                     // 24 - heroic or not
    //uint32  unk;                                          // 25
    uint32  type;                                           // 26
    //uint32  unk2;                                         // 27
    //char*   unk3;                                         // 28
    uint32  expansion;                                      // 29
    //uint32  unk4;                                         // 30
    uint32  grouptype;                                      // 31
	//uint32 unk5;
};

//does not seem to be usable. More like give info what is allowed to be leanrned
struct SkillRaceClassInfo
{
	uint32	Id;
	uint32	skill_ID;
	uint32	race_mask;
	uint32	class_mask;
	uint32	unk_flags;	
	uint32	col_5;		//mostly depracated skills	
	uint32	col_6;		//seems like grouping. Maybe flag based ? Ex : language, riding, profession
	uint32  col_7;		//seems like unavailable skills
};

struct WorldMapAreaEntry
{
	uint32	Id;				//row ID for this dbc
	uint32	map_id;			//map for which this this area is assigned
	uint32	zone_id;		//name of the zone we are delimiting
	char	*name;			//name of the zone
	float	max_y,min_y;
	float	max_x,min_x;
//	int32	unk1,unk2;		//first one seems to be always -1 the other one always 0
};

struct VehicleEntry
{
	uint32 m_ID;
	uint32 m_flags;
	float m_turnSpeed;
	float m_pitchSpeed;
	float m_pitchMin;
	float m_pitchMax;
	uint32 m_seatID[8];
	float m_mouseLookOffsetPitch;
	float m_cameraFadeDistScalarMin;
	float m_cameraFadeDistScalarMax;
	float m_cameraPitchOffset;
	float m_facingLimitRight;
	float m_facingLimitLeft;
	float m_msslTrgtTurnLingering;
	float m_msslTrgtPitchLingering;
	float m_msslTrgtMouseLingering;
	float m_msslTrgtEndOpacity;
	float m_msslTrgtArcSpeed;
	float m_msslTrgtArcRepeat;
	float m_msslTrgtArcWidth;
	float m_msslTrgtImpactRadius[2];
	char* m_msslTrgtArcTexture;
	char* m_msslTrgtImpactTexture;
	char* m_msslTrgtImpactModel[2];
	float m_cameraYawOffset;
	uint32 m_uiLocomotionType;
	float m_msslTrgtImpactTexRadius;
	uint32 m_uiSeatIndicatorType;
	uint32  m_powerType;
};

struct VehicleSeatEntry
{
    uint32  m_ID;                                           // 0
    uint32  m_flags;                                        // 1
    int32   m_attachmentID;                                 // 2
    float   m_attachmentOffsetX;                            // 3
    float   m_attachmentOffsetY;                            // 4
    float   m_attachmentOffsetZ;                            // 5
    float   m_enterPreDelay;                                // 6
    float   m_enterSpeed;                                   // 7
    float   m_enterGravity;                                 // 8
    float   m_enterMinDuration;                             // 9
    float   m_enterMaxDuration;                             // 10
    float   m_enterMinArcHeight;                            // 11
    float   m_enterMaxArcHeight;                            // 12
    int32   m_enterAnimStart;                               // 13
    int32   m_enterAnimLoop;                                // 14
    int32   m_rideAnimStart;                                // 15
    int32   m_rideAnimLoop;                                 // 16
    int32   m_rideUpperAnimStart;                           // 17
    int32   m_rideUpperAnimLoop;                            // 18
    float   m_exitPreDelay;                                 // 19
    float   m_exitSpeed;                                    // 20
    float   m_exitGravity;                                  // 21
    float   m_exitMinDuration;                              // 22
    float   m_exitMaxDuration;                              // 23
    float   m_exitMinArcHeight;                             // 24
    float   m_exitMaxArcHeight;                             // 25
    int32   m_exitAnimStart;                                // 26
    int32   m_exitAnimLoop;                                 // 27
    int32   m_exitAnimEnd;                                  // 28
    float   m_passengerYaw;                                 // 29
    float   m_passengerPitch;                               // 30
    float   m_passengerRoll;                                // 31
    int32   m_passengerAttachmentID;                        // 32
    int32   m_vehicleEnterAnim;                             // 33
    int32   m_vehicleExitAnim;                              // 34
    int32   m_vehicleRideAnimLoop;                          // 35
    int32   m_vehicleEnterAnimBone;                         // 36
    int32   m_vehicleExitAnimBone;                          // 37
    int32   m_vehicleRideAnimLoopBone;                      // 38
    float   m_vehicleEnterAnimDelay;                        // 39
    float   m_vehicleExitAnimDelay;                         // 40
    uint32  m_vehicleAbilityDisplay;                        // 41
    uint32  m_enterUISoundID;                               // 42
    uint32  m_exitUISoundID;                                // 43
    int32   m_uiSkin;                                       // 44
    uint32  m_flagsB;                                       // 45
    //uint32 unk[6];                                        // 58-63
    //uint32 unk2;                                          // 64 4.0.0
    //uint32 unk3;                                          // 65 4.0.1
	bool IsUsable() const { return ((m_flags & 0x2000000)!=0); } 
};

struct CurrencyTypesEntry
{
    uint32    ID;                                         // 0 not used
    uint32    ItemId;                                     // 1 used as real index
    uint32    Category;                                   // 2 may be category
    uint32    BitIndex;                                   // 3 bit index in PLAYER_FIELD_KNOWN_CURRENCIES (1 << (index-1))
};

struct WorldMapOverlay
{
	uint32 ID;           // 0
//	uint32 worldMapID;   // 1
	uint32 areaID;       // 2 - index to AreaTable
	uint32 areaID_2;     // 3 - index to AreaTable
	uint32 areaID_3;     // 4 - index to AreaTable
	uint32 areaID_4;     // 5 - index to AreaTable
// any of the four above indexes is enough to uncover the fragment
};


struct BattlemasterListEntry
{
	uint32	bg_index;
	int32	maps[8];
	uint32	instance_type;
	//uint32 unk_0_or_1;
//	char	*name;
	uint32	min_player_count;		//seems to be valid for 3.3.3 but changed a lot
	uint32  area_or_trigger_id;		//not sure
	uint32	minimum_player_level; // ??? Is it, 3.0.8 doesn't match 3.0.3 numbers
								 // Do not use for now (NAQUADA)
	uint32	maximum_player_level;	//just 85
	uint32	start_match_at_player_queue;	//just seems to be between min and max player count
	uint32	max_player_count;	
	uint32	is_rated_bg;	//has val 2
};

struct AreaGroup
{
	uint32 AreaGroupId;
	uint32 AreaId[ 7 ];
};

struct SpellRuneCostEntry
{
	uint32 Id;
	uint32 costs[ 3 ]; //death rune is optional
	uint32 runic_power_conversion;
};

struct GlyphPropertiesEntry
{
    uint32  Id;
    uint32  SpellId;
    uint32  TypeFlags;
    uint32  Unk1;
};

struct GlyphSlotEntry
{
    uint32  Id;
    uint32  TypeFlags;
    uint32  Order;
};

struct BarberShopStyleEntry
{
    uint32 Id; // 0
    //uint32 type; // 1 value 0 -> hair, value 2 -> facialhair
    //char* name[16]; // 2-17 name of hair style
    //uint32 name_flags; // 18
    //uint32 unk_name[16]; // 19-34, all empty
    //uint32 unk_flags; // 35
    //float unk3; // 36 values 1 and 0,75
    //uint32 race; // 37 race
    //uint32 gender; // 38 0 -> male, 1 -> female
    uint32 hair_id; // 39 real ID to hair/facial hair
};

struct GtBarberShopCostBaseEntry
{
    float   cost;
};

/*
struct AchievementEntry
{
    uint32    ID;                                           // 0
    uint32    factionFlag;                                  // 1 -1=all, 0=horde, 1=alliance
    uint32    mapID;                                        // 2 -1=none
    //uint32 parentAchievement;                             // 3 its Achievement parent (can`t start while parent uncomplete, use its Criteria if don`t have own, use its progress on begin)
    char *name;                                          // 4-19
//    char *name[15+1];                                         // 4-19
    //uint32 name_flags;                                    // 20
    //char *description[16];                                // 21-36
    //uint32 desc_flags;                                    // 37
    uint32    categoryId;                                   // 38
    uint32    points;                                       // 39 reward points
    //uint32 OrderInCategory;                               // 40
    uint32    flags;                                        // 41
    //uint32    icon;                                       // 42 icon (from SpellIcon.dbc)
    //char *titleReward[16];                                // 43-58
    //uint32 titleReward_flags;                             // 59
    uint32 count;                                           // 60 - need this count of completed criterias (own or referenced achievement criterias)
    uint32 refAchievement;                                  // 61 - referenced achievement (counting of all completed criterias)
}; */
enum AchievementCriteriaCompletionFlags
{
   ACHIEVEMENT_CRITERIA_FLAG_PROGRESS_BAR      = 0x00000001,         // Show progress as bar
   ACHIEVEMENT_CRITERIA_FLAG_HIDEN             = 0x00000002,         // Not show criteria in client
   ACHIEVEMENT_CRITERIA_FLAG_FAIL_ACHIEVEMENT  = 0x00000004,         // BG related??
   ACHIEVEMENT_CRITERIA_FLAG_RESET_ON_START    = 0x00000008,         //
   ACHIEVEMENT_CRITERIA_FLAG_IS_DATE           = 0x00000010,         // not used
   ACHIEVEMENT_CRITERIA_FLAG_IS_MONEY          = 0x00000020,         // Displays counter as money
   ACHIEVEMENT_CRITERIA_FLAG_IS_ACHIEVEMENT_ID = 0x00000040,
};

enum AchievementCriteriaMoreReqType
{
    ACHIEVEMENT_CRITERIA_MORE_REQ_TYPE_ITEM_LEVEL             = 3,
    ACHIEVEMENT_CRITERIA_MORE_REQ_TYPE_CREATURE_ID            = 4,
    ACHIEVEMENT_CRITERIA_MORE_REQ_TYPE_SPELL                  = 8,
    ACHIEVEMENT_CRITERIA_MORE_REQ_TYPE_SPELL_ON_TARGET        = 10,
    ACHIEVEMENT_CRITERIA_MORE_REQ_TYPE_ITEM_QUALITY_EQUIPPED  = 14,
    ACHIEVEMENT_CRITERIA_MORE_REQ_TYPE_ITEM_QUALITY_LOOTED    = 15,
    ACHIEVEMENT_CRITERIA_MORE_REQ_TYPE_AREA_ID                = 17,
    ACHIEVEMENT_CRITERIA_MORE_REQ_TYPE_AREA_ID2               = 18,
    ACHIEVEMENT_CRITERIA_MORE_REQ_TYPE_RAID_DIFFICULTY        = 20,
    ACHIEVEMENT_CRITERIA_MORE_REQ_TYPE_ARENA_TYPE             = 24,
    ACHIEVEMENT_CRITERIA_MORE_REQ_TYPE_PLAYER_CLASS           = 26,
    ACHIEVEMENT_CRITERIA_MORE_REQ_TYPE_PLAYER_RACE            = 27,
    ACHIEVEMENT_CRITERIA_MORE_REQ_TYPE_PLAYER_CLASS2          = 28,
    ACHIEVEMENT_CRITERIA_MORE_REQ_TYPE_CREATURE_TYPE          = 30,
    ACHIEVEMENT_CRITERIA_MORE_REQ_TYPE_MAP_ID                 = 32,
    ACHIEVEMENT_CRITERIA_MORE_REQ_TYPE_TIMED_QUEST            = 35,
    ACHIEVEMENT_CRITERIA_MORE_REQ_TYPE_PLAYER_TITLE           = 38,
    ACHIEVEMENT_CRITERIA_MORE_REQ_TYPE_PLAYER_LEVEL           = 39,
    ACHIEVEMENT_CRITERIA_MORE_REQ_TYPE_PLAYER_LEVEL2          = 40,
    ACHIEVEMENT_CRITERIA_MORE_REQ_TYPE_AREA_ID3               = 41,
    ACHIEVEMENT_CRITERIA_MORE_REQ_TYPE_GUILD_REP              = 62,
};

struct AchievementEntry
{
	uint32      ID;                                           // 0
	int32       factionFlag;                                  // 1 -1=all, 0=horde, 1=alliance
	int32       mapID;                                        // 2 -1=none
	uint32      unknown1;                                     // 20	- prev achievement required ?
	const char* name;                                         // 3-18
//	uint32      name_flags;                                   // 19
	const char* description;                                  // 21-36
//	uint32      desc_flags;                                   // 37
	uint32      categoryId;                                   // 38 - like exploration achievements
	uint32      points;                                       // 39 reward points
	uint32      orderInCategory;                              // 40
	uint32      flags;                                        // 41
	uint32      unknown2;                                     // 42
	const char* rewardName;                                   // 43-58 title/item reward name
//	uint32      rewardName_flags;                             // 59
	uint32      count;                                        // 60
	uint32      refAchievement;                               // 61
};


#define	 ACHIEVEMENT_CRITERIA_COMPLETE_FLAG_ALL  2 //complete one and completes all. ex 689
#define  ACHIEVEMENT_CRITERIA_GROUP_NOT_IN_GROUP 2 //only if not in group

enum AchievementCriteriaTypes
{
	ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE					=   0, // 
	ACHIEVEMENT_CRITERIA_TYPE_WIN_BG						=   1, // 
	ACHIEVEMENT_CRITERIA_TYPE_ACHEOLOGY_LOOT_OR_RESEARCH	=   3, // 
	ACHIEVEMENT_CRITERIA_TYPE_REACH_LEVEL					=   5, // 
	ACHIEVEMENT_CRITERIA_TYPE_REACH_SKILL_LEVEL			    =   7, // 
	ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_ARCHIEVEMENT		    =   8, // 
	ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_QUEST_COUNT			=   9, // 
	ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_DAILY_QUEST_DAILY	=  10, // 
	ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_QUESTS_IN_ZONE		=  11, // 
	ACHIEVEMENT_CRITERIA_TYPE_EARN_CURRENCY				    =  12,	// +++ Added TODO: Add the call when a currency is earned
	ACHIEVEMENT_CRITERIA_TYPE_DAMAGE_DONE					=  13, // 
	ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_DAILY_QUEST			=  14, // 
	ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_BATTLEGROUND		    =  15, // 
	ACHIEVEMENT_CRITERIA_TYPE_DEATH_AT_MAP					=  16, // 
	ACHIEVEMENT_CRITERIA_TYPE_DEATH						    =  17, // 
	ACHIEVEMENT_CRITERIA_TYPE_DEATH_IN_DUNGEON				=  18, // 
	ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_RAID				    =  19, // 
	ACHIEVEMENT_CRITERIA_TYPE_KILLED_BY_CREATURE			=  20, // 
	ACHIEVEMENT_CRITERIA_TYPE_KILLED_BY_PLAYER				=  23, // 
	ACHIEVEMENT_CRITERIA_TYPE_FALL_WITHOUT_DYING			=  24, // 
	ACHIEVEMENT_CRITERIA_TYPE_DEATHS_FROM					=  26, // 
	ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_QUEST				=  27, // 
	ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET				=  28, // TODO: check_1 is not spell_id???
	ACHIEVEMENT_CRITERIA_TYPE_CAST_SPELL					=  29, // 
	ACHIEVEMENT_CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE			=  30, // 
	ACHIEVEMENT_CRITERIA_TYPE_HONORABLE_KILL_AT_AREA		=  31, // 
	ACHIEVEMENT_CRITERIA_TYPE_WIN_ARENA					    =  32, // 
	ACHIEVEMENT_CRITERIA_TYPE_PLAY_ARENA					=  33, //
	ACHIEVEMENT_CRITERIA_TYPE_LEARN_SPELL					=  34, // 
	ACHIEVEMENT_CRITERIA_TYPE_HONORABLE_KILL				=  35, //
	ACHIEVEMENT_CRITERIA_TYPE_OWN_ITEM						=  36, // TODO: check_2 is quality of item
	ACHIEVEMENT_CRITERIA_TYPE_WIN_RATED_ARENA				=  37, //
	ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_TEAM_RATING			=  38, // 
	ACHIEVEMENT_CRITERIA_TYPE_REACH_TEAM_RATING			    =  39, // 
	ACHIEVEMENT_CRITERIA_TYPE_LEARN_SKILL_LEVEL			    =  40, // 
	ACHIEVEMENT_CRITERIA_TYPE_USE_ITEM						=  41, //
	ACHIEVEMENT_CRITERIA_TYPE_LOOT_ITEM					    =  42, // TODO: check_2 is quality of item
	ACHIEVEMENT_CRITERIA_TYPE_EXPLORE_AREA					=  43, //
	ACHIEVEMENT_CRITERIA_TYPE_OWN_PVP_RANK					=  44, //
	ACHIEVEMENT_CRITERIA_TYPE_BUY_BANK_SLOT				    =  45, //
	ACHIEVEMENT_CRITERIA_TYPE_GAIN_REPUTATION				=  46, //
	ACHIEVEMENT_CRITERIA_TYPE_GAIN_EXALTED_REPUTATION		=  47, //
	ACHIEVEMENT_CRITERIA_TYPE_VISIT_BARBER_SHOP             =  48, //
	ACHIEVEMENT_CRITERIA_TYPE_EQUIP_EPIC_ITEM               =  49, // TODO: Not called, check_1 slotid, check_2 item_level, check_3 item_quality => Add check_3 to the header of the function
	ACHIEVEMENT_CRITERIA_TYPE_ROLL_NEED_ON_LOOT             =  50, // TODO: Pass as check_2 the level of the item
	ACHIEVEMENT_CRITERIA_TYPE_ROLL_GREED_ON_LOOT			=  51, // TODO: Pass as check_2 the level of the item
	ACHIEVEMENT_CRITERIA_TYPE_HK_CLASS						=  52, // TODO: Not called, pass class of the kill as check_1
	ACHIEVEMENT_CRITERIA_TYPE_HK_RACE						=  53, // TODO: Not called, pass race of the kill as check_1
	ACHIEVEMENT_CRITERIA_TYPE_DO_EMOTE						=  54, // TODO: Not called, pass the emote id as check_1, target npc ID not coded, spell ID cast  not coded, spell ID of the spell that the target has cast  (eg. elegant dress) not codeds
	ACHIEVEMENT_CRITERIA_TYPE_HEALING_DONE                  =  55, // 
	ACHIEVEMENT_CRITERIA_TYPE_GET_KILLING_BLOWS			    =  56, // TODO: spell effect is not passed
	ACHIEVEMENT_CRITERIA_TYPE_EQUIP_ITEM					=  57, // 	
	ACHIEVEMENT_CRITERIA_TYPE_GOLD_FROM_VENDORS			    =  59, // 
	ACHIEVEMENT_CRITERIA_TYPE_GOLD_SPENT_FOR_TALENTS		=  60, // 
	ACHIEVEMENT_CRITERIA_TYPE_NUMBER_OF_TALENT_RESETS       =  61, // 
	ACHIEVEMENT_CRITERIA_TYPE_MONEY_FROM_QUEST_REWARD       =  62, // 
	ACHIEVEMENT_CRITERIA_TYPE_GOLD_SPENT_FOR_TRAVELLING	    =  63, // 
	ACHIEVEMENT_CRITERIA_TYPE_GOLD_SPENT_AT_BARBER			=  65, // 
	ACHIEVEMENT_CRITERIA_TYPE_GOLD_SPENT_FOR_MAIL			=  66, // 
	ACHIEVEMENT_CRITERIA_TYPE_LOOT_MONEY					=  67, // 
	ACHIEVEMENT_CRITERIA_TYPE_USE_GAMEOBJECT				=  68, //
	ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET2				=  69, // TODO: check_1 is not spell_id???
	ACHIEVEMENT_CRITERIA_TYPE_SPECIAL_PVP_KILL				=  70, // TODO: Not called
	ACHIEVEMENT_CRITERIA_TYPE_FISH_IN_GAMEOBJECT			=  72, // TODO: ID is not in gameobject ID
	ACHIEVEMENT_CRITERIA_TYPE_UNKNOWN_0				        =  73, // TODO: Unknown
	ACHIEVEMENT_CRITERIA_TYPE_EARNED_PVP_TITLE              =  74, // TODO: Not called
	ACHIEVEMENT_CRITERIA_TYPE_LEARN_SKILLLINE_SPELLS        =  75, // 
	ACHIEVEMENT_CRITERIA_TYPE_WIN_DUEL                      =  76, // 
	ACHIEVEMENT_CRITERIA_TYPE_LOSE_DUEL                     =  77, // 
	ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE_TYPE            =  78, // TODO: get the npc tyoe (deamon, dragonkin etc.)
	ACHIEVEMENT_CRITERIA_TYPE_GOLD_EARNED_BY_AUCTIONS       =  80, // 
	ACHIEVEMENT_CRITERIA_TYPE_CREATE_AUCTION                =  82, // 
	ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_AUCTION_BID           =  83, // 
	ACHIEVEMENT_CRITERIA_TYPE_WON_AUCTIONS                  =  84, // 
	ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_AUCTION_SOLD          =  85, // TODO: Not called
	ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_GOLD_VALUE_OWNED      =  86, // 
	ACHIEVEMENT_CRITERIA_TYPE_GAIN_REVERED_REPUTATION       =  87, // 
	ACHIEVEMENT_CRITERIA_TYPE_GAIN_HONORED_REPUTATION       =  88, // TODO: Not called
	ACHIEVEMENT_CRITERIA_TYPE_KNOWN_FACTIONS                =  89, // 
	ACHIEVEMENT_CRITERIA_TYPE_LOOT_EPIC_ITEM                =  90, // 
	ACHIEVEMENT_CRITERIA_TYPE_RECEIVE_EPIC_ITEM             =  91, // 
	ACHIEVEMENT_CRITERIA_TYPE_ROLL_NEED                     =  93, // 
	ACHIEVEMENT_CRITERIA_TYPE_ROLL_GREED                    =  94, // 
	ACHIEVEMENT_CRITERIA_TYPE_REACH_PERSONAL_RATING		    =  95, //
	ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_HIT_DEALT             = 101, // 
	ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_HIT_RECEIVED          = 102, // 
	ACHIEVEMENT_CRITERIA_TYPE_TOTAL_DAMAGE_RECEIVED         = 103, // 
	ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_HEAL_CASTED           = 104, // 
	ACHIEVEMENT_CRITERIA_TYPE_TOTAL_HEALING_RECEIVED        = 105, // 
	ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_HEALING_RECEIVED      = 106, // 
	ACHIEVEMENT_CRITERIA_TYPE_QUEST_ABANDONED               = 107, // 
	ACHIEVEMENT_CRITERIA_TYPE_FLIGHT_PATHS_TAKEN            = 108, // 
	ACHIEVEMENT_CRITERIA_TYPE_LOOT_TYPE                     = 109, // 
	ACHIEVEMENT_CRITERIA_TYPE_CAST_SPELL2                   = 110, // 
	ACHIEVEMENT_CRITERIA_TYPE_LEARN_SKILL_LINE_SPELL2       = 112, // 
	ACHIEVEMENT_CRITERIA_TYPE_EARN_HONORABLE_KILL           = 113, // 
	ACHIEVEMENT_CRITERIA_TYPE_ACCEPTED_SUMMONINGS           = 114, // TODO: Not called
	ACHIEVEMENT_CRITERIA_TYPE_EARN_ACHIEVEMENT_POINTS       = 115, // TODO: Not called. Get achievement points for player.
	ACHIEVEMENT_CRITERIA_TYPE_ROLL_DISENCHANT               = 117, // TODO: Not present in criteria DBC, remove the call
	ACHIEVEMENT_CRITERIA_TYPE_REACH_GUILD_LEVEL             = 125, // TODO: Not called. Guild level
	ACHIEVEMENT_CRITERIA_TYPE_GUILD_ACHIEVEMENT_CRAFT       = 126, // TODO: Not called, and criterias
	ACHIEVEMENT_CRITERIA_TYPE_GUILD_ACHIEVEMENT_FISH        = 127, // TODO: Not called, and criterias
	ACHIEVEMENT_CRITERIA_TYPE_WIN_RATED_BATTLEGROUND        = 130, // TODO: Not called, and criterias
	ACHIEVEMENT_CRITERIA_TYPE_EARN_BATTLEGROUND_RATING      = 132, // TODO: Not called, and criterias
	ACHIEVEMENT_CRITERIA_TYPE_GUILD_CREST                   = 133, // TODO: Not called, and criterias
	ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_QUEST_NUMBER         = 134, // TODO: Not called, and criterias
	ACHIEVEMENT_CRITERIA_TYPE_HONORABLE_KILLS_COUNT         = 135, // TODO: Not called, and criterias
	ACHIEVEMENT_CRITERIA_TYPE_GUILD_MAP_CHALLANGE_COUNT     = 138, // TODO: Not called, and criterias
	ACHIEVEMENT_CRITERIA_TYPE_GUILD_CHALLANGE_COUNT			= 139, // TODO: Not called, and criterias
    ACHIEVEMENT_CRITERIA_TYPE_TOTAL                         = 151,

    // ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE = 0,
    // ACHIEVEMENT_CRITERIA_TYPE_WIN_BG = 1,
    // ACHIEVEMENT_CRITERIA_TYPE_ACHEOLOGY_LOOT_OR_RESEARCH = 3,	//also research ? how do i know which is which ?
    // ACHIEVEMENT_CRITERIA_TYPE_REACH_LEVEL = 5,
    // ACHIEVEMENT_CRITERIA_TYPE_REACH_SKILL_LEVEL = 7,
    // ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_ARCHIEVEMENT = 8,
    // ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_QUEST_COUNT = 9,
    // // you have to complete a daily quest x times in a row
    // ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_DAILY_QUEST_DAILY = 10,
    // ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_QUESTS_IN_ZONE = 11,
	// ACHIEVEMENT_CRITERIA_TYPE_DAMAGE_DONE            = 13,
    // ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_DAILY_QUEST = 14,
    // ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_BATTLEGROUND= 15,
    // ACHIEVEMENT_CRITERIA_TYPE_DEATH_AT_MAP= 16,
    // // TODO: this can be both arena and total deaths. Where is this difference in the dbc?
    // ACHIEVEMENT_CRITERIA_TYPE_DEATH= 17,
    // ACHIEVEMENT_CRITERIA_TYPE_DEATH_IN_DUNGEON = 18,
    // ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_RAID = 19,
    // ACHIEVEMENT_CRITERIA_TYPE_KILLED_BY_CREATURE = 20,
    // ACHIEVEMENT_CRITERIA_TYPE_KILLED_BY_PLAYER = 23,
    // ACHIEVEMENT_CRITERIA_TYPE_FALL_WITHOUT_DYING = 24,
	// ACHIEVEMENT_CRITERIA_TYPE_DEATHS_FROM = 26,
    // ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_QUEST = 27,
    // ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET = 28,
    // ACHIEVEMENT_CRITERIA_TYPE_CAST_SPELL= 29,
    // ACHIEVEMENT_CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE = 30,
    // ACHIEVEMENT_CRITERIA_TYPE_HONORABLE_KILL_AT_AREA = 31,
    // ACHIEVEMENT_CRITERIA_TYPE_WIN_ARENA = 32,
    // ACHIEVEMENT_CRITERIA_TYPE_PLAY_ARENA = 33,
    // ACHIEVEMENT_CRITERIA_TYPE_LEARN_SPELL = 34,
    // // TODO: this criteria has additional conditions which can not be found in the dbcs
    // ACHIEVEMENT_CRITERIA_TYPE_HONORABLE_KILL = 35,
    // ACHIEVEMENT_CRITERIA_TYPE_OWN_ITEM = 36,
    // // TODO: the archievements 1162 and 1163 requires a special rating which can't be found in the dbc
    // ACHIEVEMENT_CRITERIA_TYPE_WIN_RATED_ARENA = 37,
    // ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_TEAM_RATING = 38,
    // ACHIEVEMENT_CRITERIA_TYPE_REACH_TEAM_RATING = 39,
    // ACHIEVEMENT_CRITERIA_TYPE_LEARN_SKILL_LEVEL = 40,
    // ACHIEVEMENT_CRITERIA_TYPE_USE_ITEM = 41,
    // ACHIEVEMENT_CRITERIA_TYPE_LOOT_ITEM= 42,
    // ACHIEVEMENT_CRITERIA_TYPE_EXPLORE_AREA = 43,
    // ACHIEVEMENT_CRITERIA_TYPE_OWN_PVP_RANK= 44,
    // ACHIEVEMENT_CRITERIA_TYPE_BUY_BANK_SLOT= 45,
    // ACHIEVEMENT_CRITERIA_TYPE_GAIN_REPUTATION= 46,
    // ACHIEVEMENT_CRITERIA_TYPE_GAIN_EXALTED_REPUTATION= 47,
    // // noted: rewarded as soon as the player payed, not at taking place at the seat
    // ACHIEVEMENT_CRITERIA_TYPE_VISIT_BARBER_SHOP= 48,
    // ACHIEVEMENT_CRITERIA_TYPE_EQUIP_EPIC_ITEM = 49,
    // // TODO: itemlevel is mentioned in text but not present in dbc
    // ACHIEVEMENT_CRITERIA_TYPE_ROLL_NEED_ON_LOOT = 50,
	// ACHIEVEMENT_CRITERIA_TYPE_ROLL_GREED_ON_LOOT      = 51,
    // ACHIEVEMENT_CRITERIA_TYPE_HK_CLASS = 52,
    // ACHIEVEMENT_CRITERIA_TYPE_HK_RACE = 53,
    // ACHIEVEMENT_CRITERIA_TYPE_DO_EMOTE = 54,
    // ACHIEVEMENT_CRITERIA_TYPE_HEALING_DONE = 55,
	// ACHIEVEMENT_CRITERIA_TYPE_GET_KILLING_BLOWS      = 56,
    // ACHIEVEMENT_CRITERIA_TYPE_EQUIP_ITEM = 57,
    // ACHIEVEMENT_CRITERIA_TYPE_GOLD_FROM_VENDORS = 59,
    // ACHIEVEMENT_CRITERIA_TYPE_GOLD_SPENT_FOR_TALENTS = 60,
    // ACHIEVEMENT_CRITERIA_TYPE_NUMBER_OF_TALENT_RESETS = 61,
	// ACHIEVEMENT_CRITERIA_TYPE_MONEY_FROM_QUEST_REWARD= 62,
    // ACHIEVEMENT_CRITERIA_TYPE_GOLD_SPENT_FOR_TRAVELLING = 63,
    // ACHIEVEMENT_CRITERIA_TYPE_GOLD_SPENT_AT_BARBER = 65,
    // ACHIEVEMENT_CRITERIA_TYPE_GOLD_SPENT_FOR_MAIL = 66,
    // ACHIEVEMENT_CRITERIA_TYPE_LOOT_MONEY = 67,
    // ACHIEVEMENT_CRITERIA_TYPE_USE_GAMEOBJECT = 68,
    // ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET2= 69,
    // ACHIEVEMENT_CRITERIA_TYPE_SPECIAL_PVP_KILL= 70,
    // ACHIEVEMENT_CRITERIA_TYPE_FISH_IN_GAMEOBJECT = 72,
    // // TODO: title id is not mentioned in dbc
    // ACHIEVEMENT_CRITERIA_TYPE_EARNED_PVP_TITLE = 74,
    // ACHIEVEMENT_CRITERIA_TYPE_LEARN_SKILLLINE_SPELLS= 75,
    // ACHIEVEMENT_CRITERIA_TYPE_WIN_DUEL = 76,
    // ACHIEVEMENT_CRITERIA_TYPE_LOSE_DUEL = 77,
    // // TODO: creature type (demon, undead etc.) is not stored in dbc
    // ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE_TYPE = 78,
    // ACHIEVEMENT_CRITERIA_TYPE_GOLD_EARNED_BY_AUCTIONS= 80,
    // ACHIEVEMENT_CRITERIA_TYPE_CREATE_AUCTION= 82,
    // ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_AUCTION_BID= 83,
    // ACHIEVEMENT_CRITERIA_TYPE_WON_AUCTIONS= 84,
    // ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_AUCTION_SOLD = 85,
    // ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_GOLD_VALUE_OWNED = 86,
    // ACHIEVEMENT_CRITERIA_TYPE_GAIN_REVERED_REPUTATION = 87,
    // ACHIEVEMENT_CRITERIA_TYPE_GAIN_HONORED_REPUTATION = 88,
    // ACHIEVEMENT_CRITERIA_TYPE_KNOWN_FACTIONS = 89,
    // ACHIEVEMENT_CRITERIA_TYPE_LOOT_EPIC_ITEM = 90,
    // ACHIEVEMENT_CRITERIA_TYPE_RECEIVE_EPIC_ITEM = 91,
    // ACHIEVEMENT_CRITERIA_TYPE_ROLL_NEED = 93,
    // ACHIEVEMENT_CRITERIA_TYPE_ROLL_GREED = 94,
// //    ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_HEALTH = 95,
// //    ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_POWER = 96,
// //    ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_STAT = 97,
// //    ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_SPELLPOWER = 98,
// //    ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_ARMOR = 99,
// //    ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_RATING = 100,
    // ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_HIT_DEALT = 101,
    // ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_HIT_RECEIVED = 102,
    // ACHIEVEMENT_CRITERIA_TYPE_TOTAL_DAMAGE_RECEIVED = 103,
    // ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_HEAL_CASTED = 104,
    // ACHIEVEMENT_CRITERIA_TYPE_TOTAL_HEALING_RECEIVED = 105,
    // ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_HEALING_RECEIVED = 106,
    // ACHIEVEMENT_CRITERIA_TYPE_QUEST_ABANDONED = 107,
    // ACHIEVEMENT_CRITERIA_TYPE_FLIGHT_PATHS_TAKEN = 108,
    // ACHIEVEMENT_CRITERIA_TYPE_LOOT_TYPE = 109,
    // // TODO: target entry is missing
    // ACHIEVEMENT_CRITERIA_TYPE_CAST_SPELL2 = 110,
    // ACHIEVEMENT_CRITERIA_TYPE_LEARN_SKILL_LINE_SPELL2 = 112,
    // ACHIEVEMENT_CRITERIA_TYPE_EARN_HONORABLE_KILL = 113,
    // ACHIEVEMENT_CRITERIA_TYPE_ACCEPTED_SUMMONINGS = 114,
    // ACHIEVEMENT_CRITERIA_TYPE_ROLL_DISENCHANT = 117,
    // // 0..114 => 115 criteria types total
    // ACHIEVEMENT_CRITERIA_TYPE_TOTAL = 136,
};
 
//should have named this a simple achievemnt entry
struct AchievementCriteriaEntry
{
    uint32  ID;                                             // 0
    uint32  referredAchievement;                            // 1
    uint32  requiredType;                                   // 2
    union
    {
        // ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE          = 0
        // TODO: also used for player deaths..
        struct
        {
            uint32  creatureID;                             // 3
            uint64  creatureCount;                          // 4
        } kill_creature;

        // ACHIEVEMENT_CRITERIA_TYPE_WIN_BG                 = 1
        struct
        {
            uint32  bgMapID;                                // 3
            uint64  winCount;                               // 4
            uint32  additionalRequirement1_type;            // 5 additional requirement 1 type
            uint32  additionalRequirement1_value;           // 6 additional requirement 1 value
            uint32  additionalRequirement2_type;            // 7 additional requirement 2 type
            uint32  additionalRequirement2_value;           // 8 additional requirement 1 value
        } win_bg;

        // ACHIEVEMENT_CRITERIA_TYPE_REACH_LEVEL            = 5
        struct
        {
            uint32  unused;                                 // 3
            uint64  level;                                  // 4
        } reach_level;

        // ACHIEVEMENT_CRITERIA_TYPE_REACH_SKILL_LEVEL      = 7
        struct
        {
            uint32  skillID;                                // 3
            uint64  skillLevel;                             // 4
        } reach_skill_level;

        // ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_ACHIEVEMENT   = 8
        struct
        {
            uint32  linkedAchievement;                      // 3
        } complete_achievement;

        // ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_QUEST_COUNT   = 9
        struct
        {
            uint32  unused;                                 // 3
            uint64  totalQuestCount;                        // 4
        } complete_quest_count;

        // ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_DAILY_QUEST_DAILY = 10
        struct
        {
            uint32  unused;                                 // 3
            uint64  numberOfDays;                           // 4
        } complete_daily_quest_daily;

        // ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_QUESTS_IN_ZONE = 11
        struct
        {
            uint32  zoneID;                                 // 3
            uint64  questCount;                             // 4
        } complete_quests_in_zone;

        // ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_DAILY_QUEST   = 14
        struct
        {
            uint32  unused;                                 // 3
            uint64  questCount;                             // 4
        } complete_daily_quest;

        // ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_BATTLEGROUND  = 15
        struct
        {
            uint32  mapID;                                  // 3
        } complete_battleground;

        // ACHIEVEMENT_CRITERIA_TYPE_DEATH_AT_MAP           = 16
        struct
        {
            uint32  mapID;                                  // 3
        } death_at_map;

        // ACHIEVEMENT_CRITERIA_TYPE_DEATH_IN_DUNGEON       = 18
        struct
        {
            uint32  manLimit;                               // 3
        } death_in_dungeon;

        // ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_RAID          = 19
        struct
        {
            uint32  groupSize;                              // 3 can be 5, 10 or 25
        } complete_raid;

        // ACHIEVEMENT_CRITERIA_TYPE_KILLED_BY_CREATURE     = 20
		// ACHIEVEMENT_CRITERIA_TYPE_KILLED_BY_PLAYER		= 23
        struct
        {
            uint32  creatureEntry;                          // 3
        } killed_by_creature;

        // ACHIEVEMENT_CRITERIA_TYPE_FALL_WITHOUT_DYING     = 24
        struct
        {
            uint32  unused;                                 // 3
            uint64  fallHeight;                             // 4
        } fall_without_dying;

        // ACHIEVEMENT_CRITERIA_TYPE_DEATHS_FROM            = 26
        struct
        {
            uint32 type;                                    // 3, see enum EnviromentalDamage
        } death_from;

        // ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_QUEST         = 27
        struct
        {
            uint32  questID;                                // 3
            uint64  questCount;                             // 4
        } complete_quest;

        // ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET        = 28
        // ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET2       = 69
        struct
        {
            uint32  spellID;                                // 3
            uint64  spellCount;                             // 4
        } be_spell_target;

        // ACHIEVEMENT_CRITERIA_TYPE_CAST_SPELL             = 29
        // ACHIEVEMENT_CRITERIA_TYPE_CAST_SPELL2            = 110
        struct
        {
            uint32  spellID;                                // 3
            uint64  castCount;                              // 4
        } cast_spell;

        // ACHIEVEMENT_CRITERIA_TYPE_HONORABLE_KILL_AT_AREA = 31
        struct
        {
            uint32  areaID;                                 // 3 Reference to AreaTable.dbc
            uint64  killCount;                              // 4
        } honorable_kill_at_area;

        // ACHIEVEMENT_CRITERIA_TYPE_WIN_ARENA              = 32
        struct
        {
            uint32  mapID;                                  // 3 Reference to Map.dbc
        } win_arena;

        // ACHIEVEMENT_CRITERIA_TYPE_PLAY_ARENA             = 33
        struct
        {
            uint32  mapID;                                  // 3 Reference to Map.dbc
        } play_arena;

        // ACHIEVEMENT_CRITERIA_TYPE_LEARN_SPELL            = 34
        struct
        {
            uint32  spellID;                                // 3 
        } learn_spell;

        // ACHIEVEMENT_CRITERIA_TYPE_OWN_ITEM               = 36
        struct
        {
            uint32  itemID;                                 // 3
            uint64  itemCount;                              // 4
        } own_item;

        // ACHIEVEMENT_CRITERIA_TYPE_WIN_RATED_ARENA        = 37
        struct
        {
            uint32  unused;                                 // 3
            uint64  count;                                  // 4
            uint32  flag;                                   // 5 4=in a row
        } win_rated_arena;

        // ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_TEAM_RATING    = 38
        struct
        {
            uint32  teamtype;                               // 3 {2,3,5}
        } highest_team_rating;

        // ACHIEVEMENT_CRITERIA_TYPE_REACH_TEAM_RATING      = 39
        struct
        {
            uint32  teamtype;                               // 3 {2,3,5}
            uint64  teamrating;                             // 4
        } reach_team_rating;

        // ACHIEVEMENT_CRITERIA_TYPE_LEARN_SKILL_LEVEL      = 40
        struct
        {
            uint32  skillID;                                // 3
            uint64  skillLevel;                             // 4 apprentice=1, journeyman=2, expert=3, artisan=4, master=5, grand master=6
        } learn_skill_level;

        // ACHIEVEMENT_CRITERIA_TYPE_USE_ITEM               = 41
        struct
        {
            uint32  itemID;                                 // 3
            uint64  itemCount;                              // 4
        } use_item;

        // ACHIEVEMENT_CRITERIA_TYPE_LOOT_ITEM              = 42
        struct
        {
            uint32  itemID;                                 // 3
            uint64  itemCount;                              // 4
        } loot_item;

        // ACHIEVEMENT_CRITERIA_TYPE_EXPLORE_AREA           = 43
        struct
        {
            // TODO: This rank is _NOT_ the index from AreaTable.dbc
            uint32  areaReference;                          // 3
        } explore_area;

        // ACHIEVEMENT_CRITERIA_TYPE_OWN_PVP_RANK               = 44
        struct
        {
            // TODO: This rank is _NOT_ the index from CharTitles.dbc
            uint32  rank;                                   // 3
        } own_rank;

        // ACHIEVEMENT_CRITERIA_TYPE_BUY_BANK_SLOT          = 45
        struct
        {
            uint32  unused;                                 // 3
            uint64  numberOfSlots;                          // 4
        } buy_bank_slot;

        // ACHIEVEMENT_CRITERIA_TYPE_GAIN_REPUTATION        = 46
        struct
        {
            uint32  factionID;                              // 3
            uint64  reputationAmount;                       // 4 Total reputation amount, so 42000 = exalted
        } gain_reputation;

        // ACHIEVEMENT_CRITERIA_TYPE_GAIN_EXALTED_REPUTATION= 47
        struct
        {
            uint32  unused;                                 // 3
            uint64  numberOfExaltedFactions;                // 4
        } gain_exalted_reputation;

        // ACHIEVEMENT_CRITERIA_TYPE_VISIT_BARBER_SHOP      = 48
        struct
        {
            uint32 unused;                                  // 3
            uint64 numberOfVisits;                          // 4
        } visit_barber;

        // ACHIEVEMENT_CRITERIA_TYPE_EQUIP_EPIC_ITEM        = 49
        // TODO: where is the required itemlevel stored?
        struct
        {
            uint32  itemSlot;                               // 3
            uint64  count;                                  // 4
        } equip_epic_item;

        // ACHIEVEMENT_CRITERIA_TYPE_ROLL_NEED_ON_LOOT      = 50
        struct
        {
            uint32  rollValue;                              // 3
            uint64  count;                                  // 4
        } roll_need_on_loot;
       // ACHIEVEMENT_CRITERIA_TYPE_ROLL_GREED_ON_LOOT      = 51
        struct
        {
            uint32  rollValue;                              // 3
            uint64  count;                                  // 4
        } roll_greed_on_loot;

        // ACHIEVEMENT_CRITERIA_TYPE_HK_CLASS               = 52
        struct
        {
            uint32  classID;                                // 3
            uint64  count;                                  // 4
        } hk_class;

        // ACHIEVEMENT_CRITERIA_TYPE_HK_RACE                = 53
        struct
        {
            uint32  raceID;                                 // 3
            uint64  count;                                  // 4
        } hk_race;

        // ACHIEVEMENT_CRITERIA_TYPE_DO_EMOTE               = 54
        // TODO: where is the information about the target stored?
        struct
        {
            uint32  emoteID;                                // 3 enum TextEmotes
            uint64  count;                                  // 4 count of emotes, always required special target or requirements
        } do_emote;
        // ACHIEVEMENT_CRITERIA_TYPE_DAMAGE_DONE            = 13
        // ACHIEVEMENT_CRITERIA_TYPE_HEALING_DONE           = 55
        // ACHIEVEMENT_CRITERIA_TYPE_GET_KILLING_BLOWS      = 56
        struct
        {
            uint32  unused;                                 // 3
            uint64  count;                                  // 4
            uint32  flag;                                   // 5 =3 for battleground healing
            uint32  mapid;                                  // 6
        } healing_done;

        // ACHIEVEMENT_CRITERIA_TYPE_EQUIP_ITEM             = 57
        struct
        {
            uint32  itemID;                                 // 3
            uint64  count;                                  // 4
        } equip_item;

        // ACHIEVEMENT_CRITERIA_TYPE_MONEY_FROM_QUEST_REWARD= 62
        struct
        {
            uint32  unused;                                 // 3
            uint64  goldInCopper;                           // 4
        } quest_reward_money;


        // ACHIEVEMENT_CRITERIA_TYPE_LOOT_MONEY             = 67
        struct
        {
            uint32  unused;                                 // 3
            uint64  goldInCopper;                           // 4
        } loot_money;

        // ACHIEVEMENT_CRITERIA_TYPE_USE_GAMEOBJECT         = 68
        struct
        {
            uint32  goEntry;                                // 3
            uint64  useCount;                               // 4
        } use_gameobject;

        // ACHIEVEMENT_CRITERIA_TYPE_SPECIAL_PVP_KILL       = 70
        // TODO: are those special criteria stored in the dbc or do we have to add another sql table?
        struct
        {
            uint32  unused;                                 // 3
            uint64  killCount;                              // 4
        } special_pvp_kill;

        // ACHIEVEMENT_CRITERIA_TYPE_FISH_IN_GAMEOBJECT     = 72
        struct
        {
            uint32  goEntry;                                // 3
            uint64  lootCount;                              // 4
        } fish_in_gameobject;

        // ACHIEVEMENT_CRITERIA_TYPE_LEARN_SKILLLINE_SPELLS = 75
        struct
        {
            uint32  skillLine;                              // 3
            uint64  spellCount;                             // 4
        } learn_skillline_spell;

        // ACHIEVEMENT_CRITERIA_TYPE_WIN_DUEL               = 76
        struct
        {
            uint32  unused;                                 // 3
            uint64  duelCount;                              // 4
        } win_duel;

        // ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_POWER          = 96
        struct
        {
            uint32  powerType;                              // 3 mana=0, 1=rage, 3=energy, 6=runic power
        } highest_power;

        // ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_STAT           = 97
        struct
        {
            uint32  statType;                               // 3 4=spirit, 3=int, 2=stamina, 1=agi, 0=strength
        } highest_stat;

        // ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_SPELLPOWER     = 98
        struct
        {
            uint32  spellSchool;                            // 3
        } highest_spellpower;

        // ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_RATING         = 100
        struct
        {
            uint32  ratingType;                             // 3
        } highest_rating;

        // ACHIEVEMENT_CRITERIA_TYPE_LOOT_TYPE              = 109
        struct
        {
            uint32  lootType;                               // 3 3=fishing, 2=pickpocket, 4=disentchant
            uint64  lootTypeCount;                          // 4
        } loot_type;

        // ACHIEVEMENT_CRITERIA_TYPE_LEARN_SKILL_LINE       = 112
        struct
        {
            uint32  skillLine;                              // 3
            uint64  spellCount;                             // 4
        } learn_skill_line;

        // ACHIEVEMENT_CRITERIA_TYPE_EARN_HONORABLE_KILL    = 113
        struct
        {
            uint32  unused;                                 // 3
            uint64  killCount;                              // 4
        } honorable_kill;

        struct
        {
            uint32  requiredAchievementRelatedEntry;        // 3 main requirement
            uint64  requiredAchievementRelatedCount;        // 4 main requirement count
            uint32  additionalRequirement1_type;            // 5 additional requirement 1 type
            uint32  additionalRequirement1_value;           // 6 additional requirement 1 value
            uint32  additionalRequirement2_type;            // 7 additional requirement 2 type
            uint32  additionalRequirement2_value;           // 8 additional requirement 1 value
        } raw;
    };
    char*  name;                                            // 9
    uint32  completionFlag;                                 // 10
    //uint32  timedCriteriaStartType;                       // 11 Only appears with timed achievements, seems to be the type of starting a timed Achievement, only type 1 and some of type 6 need manual starting
                                                            // 1: ByEventId(?) (serverside IDs),    2: ByQuestId,   5: ByCastSpellId(?)
                                                            // 6: BySpellIdTarget(some of these are unknown spells, some not, some maybe spells)
                                                            // 7: ByKillNpcId,  9: ByUseItemId
    uint32  timedCriteriaMiscId;                            // 12 Alway appears with timed events, used internally to start the achievement, store 
    uint32  timeLimit;                                      // 13 time limit in seconds
    uint32  showOrder;                                      // 14 show order
    //uint32 unk1;                                          // 15 only one value, still unknown
    //uint32 unk2;                                          // 16 all zeros
    //uint32 moreRequirement[3];                            // 17-19
    //uint32 moreRequirementValue[3];                       // 20-22
	///!!!!!!!!customs !!!!!
	uint32	requires_heroic_instance;						// creature kill type requires instances to have specific mode
	uint32	requires_player_count;							// creature kill type requires instances to have specific mode
	uint32	requires_arenta_team_type_check;				// this seems to available only in the criteria description
	SPELL_EFFECT_OVERRIDE_RETURNS		(*ConditionHandler)(Player *p,AchievementCriteriaEntry *ac,uint32 &check_1,uint32 &check_2,int64 &pvalue,uint32 &action );
};

#define ACHIEVEMENT_UNUSED_FIELD_VALUE	0xFFFFFFFF

struct BankSlotPrice
{
	uint32 Id;
	uint32 Price;
};

struct ItemSetEntry
{
    uint32 id;                  //1
    char* name;                //2
    //uint32 unused_shit[15];      //3 - 9
//    uint32 flag;                //10 constant
    uint32 itemid[8];           //11 - 18
    //uint32 more_unused_names[9]; //19 - 27
    uint32 SpellID[8];          //28 - 35
    uint32 itemscount[8];       //36 - 43
    uint32 RequiredSkillID;     //44
    uint32 RequiredSkillAmt;    //45
};

struct Lock
{
    uint32 Id;
    uint32 locktype[5]; // 0 - no lock, 1 - item needed for lock, 2 - min lockping skill needed
    //uint32 unk1[3];
    uint32 lockmisc[5]; // if type is 1 here is a item to unlock, else is unknow for now
    //uint32 unk2[3];
    uint32 minlockskill[5]; // min skill in lockpiking to unlock.
    //uint32 unk3[11];

	/* Actually it is:
	uint32 Id;
    uint32 locktype[8];
    uint32 lockmisc[8];
    uint32 minlockskill[8];
    uint32 action[8];
	*/
};

struct emoteentry
{
    uint32 Id;
    //uint32 name;
    uint32 textid;
    uint32 textid2;
    uint32 textid3;
    uint32 textid4;
    //uint32 unk1;
    uint32 textid5;
    //uint32 unk2;
    uint32 textid6;
    //uint32 unk3;
    //uint32 unk4;
    //uint32 unk5;
    //uint32 unk6;
    //uint32 unk7;
    //uint32 unk8;
    //uint32 unk9;
    //uint32 unk10;
    //uint32 unk11;
};

struct TalentEntry
{
    uint32    TalentID;                                     // 0
    uint32    TalentTab;                                    // 1 index in TalentTab.dbc (TalentTabEntry)
    uint32    Row;                                          // 2
    uint32    Col;                                          // 3
    uint32    RankID[5];					                // 4-8
//	uint32	  unk403;
    uint32    DependsOn;                                    // 9 index in Talent.dbc (TalentEntry)
//	uint32	  unk[4];										// 10-11 not used
    uint32    DependsOnRank;                                // 12
                                                            // 13-14 not used
    //uint32  needAddInSpellBook;                           // 15  also need disable higest ranks on reset talent tree
    //uint32  unk2;                                         // 16, all 0
    //uint64  allowForPet;                                  // 17 its a 64 bit mask for pet 1<<m_categoryEnumID in CreatureFamily.dbc
};

struct TalentPrimarySpellsEntry
{
	uint32 ID;
	uint32 tabID;
	uint32 SpellID;
};

struct skilllinespell //SkillLineAbility.dbc
{
    uint32    Id;                                           // 0        m_ID
    uint32    skillId;                                      // 1        m_skillLine
    uint32    spellId;                                      // 2        m_spell
    uint32    raceMask;                                     // 3        m_raceMask
    uint32    classMask;                                    // 4        m_classMask
    uint32    excludeRace;                                // 5        m_excludeRace
    uint32    excludeClass;                               // 6        m_excludeClass
    uint32    req_skill_value;                              // 7        m_minSkillLineRank
    uint32    forward_spellid;                              // 8        m_supercededBySpell
    uint32    learnOnGetSkill;                              // 9        m_acquireMethod
    uint32    max_value;                                    // 10       m_trivialSkillLineRankHigh
    uint32    min_value;                                    // 11       m_trivialSkillLineRankLow
    uint32    skill_point_advance;                          // 12		number of skill points you may receive for casting this spell
    //uint32                                                // 13       4.0.0
	//customs 
	uint32 prev;	//customs
};

struct EnchantEntry
{
    uint32 Id;
//	uint32 unk1;
    uint32 type[3];
    int32 min[3];//for compat, in practice min==max
    int32 max[3];
    uint32 spell[3];
    char* Name;
    //uint32 NameAlt1;
    //uint32 NameAlt2;
    //uint32 NameAlt3;
    //uint32 NameAlt4;
    //uint32 NameAlt5;
    //uint32 NameAlt6;
    //uint32 NameAlt7;
    //uint32 NameAlt8;
    //uint32 NameAlt9;
    //uint32 NameAlt10;
    //uint32 NameAlt11;
    //uint32 NameAlt12;
    //uint32 NameAlt13;
    //uint32 NameAlt14;
    //uint32 NameAlt15;
    //uint32 NameFlags;
    uint32 visual;
    uint32 EnchantGroups;
    uint32 GemEntry;
    uint32 unk7;//Gem Related
	uint32 required_skill;
	uint32 required_skill_rank;
	uint32 unk10;
	uint32 req_itemlevel;	//not dead sure though more like item level+1 
	//								//39
	uint32 custom_enchant_group; //this comes from example gem types.There is a high chance this value already exists somewhere
	uint32 custom_enchant_maxstack; //this comes from example gem types
	uint32 custom_GearScore;		//gem level or spell level
	uint32 custom_ScriptCreated;	//do not send visual details to client
};

struct GemPropertyEntry{
    uint32 Entry;
    uint32 EnchantmentID;
    uint32 SocketMaskMustMatch;//bool - maybe sellable
    uint32 unk2;//bool - maybe sellable
    uint32 SocketMask;
	uint32 unk3;//seems to be 285 or 0
};

struct ItemLimitCategory{
	uint32 CategoryId;
//	uint32 Name1;
//	uint32 NameAlt2;
//	uint32 NameAlt3;
//	uint32 NameAlt4;
//	uint32 NameAlt5;
//	uint32 NameAlt6;
//	uint32 NameAlt7;
//	uint32 NameAlt8;
//	uint32 NameAlt9;
//	uint32 NameAlt10;
//	uint32 NameAlt11;
//	uint32 NameAlt12;
//	uint32 NameAlt13;
//	uint32 NameAlt14;
//	uint32 NameAlt15;
//	uint32 NameAlt16;
//	uint32 NameAlt17;
//	uint32 NameFlags; //not sure they are actually nameflags
	uint32 MaxStack;
//	uint32 unkbool;
};

struct skilllineentry //SkillLine.dbc
{
    uint32 id;
    uint32 type;
    char* Name;
    //int32 NameAlt1;
    //uint32 NameAlt2;
    //uint32 NameAlt3;
    //uint32 NameAlt4;
    //uint32 NameAlt5;
    //uint32 NameAlt6;
    //uint32 NameAlt7;
    //uint32 NameAlt8;
    //uint32 NameAlt9;
    //uint32 NameAlt10;
    //uint32 NameAlt11;
    //uint32 NameAlt12;
    //uint32 NameAlt13;
    //uint32 NameAlt14;
    //uint32 NameAlt15;
    //uint32 NameFlags;
    //uint32 Description;
    //uint32 DescriptionAlt1;
    //uint32 DescriptionAlt2;
    //uint32 DescriptionAlt3;
    //uint32 DescriptionAlt4;
    //uint32 DescriptionAlt5;
    //uint32 DescriptionAlt6;
    //uint32 DescriptionAlt7;
    //uint32 DescriptionAlt8;
    //uint32 DescriptionAlt9;
    //uint32 DescriptionAlt10;
    //uint32 DescriptionAlt11;
    //uint32 DescriptionAlt12;
    //uint32 DescriptionAlt13;
    //uint32 DescriptionAlt14;
    //uint32 DescriptionAlt15;
    //uint32 DescriptionFlags;
    //uint32 unk2;
    //uint32 TypeText;
    //uint32 TypeTextAlt1;
    //uint32 TypeTextAlt2;
    //uint32 TypeTextAlt3;
    //uint32 TypeTextAlt4;
    //uint32 TypeTextAlt5;
    //uint32 TypeTextAlt6;
    //uint32 TypeTextAlt7;
    //uint32 TypeTextAlt8;
    //uint32 TypeTextAlt9;
    //uint32 TypeTextAlt10;
    //uint32 TypeTextAlt11;
    //uint32 TypeTextAlt12;
    //uint32 TypeTextAlt13;
    //uint32 TypeTextAlt14;
    //uint32 TypeTextAlt15;
    //uint32 TypeTextFlags;
	//uint32 unk3;
};

class Spell;
class Unit;
class Aura;
struct ProcHandlerContextShare;

enum SpellRequiredTargetTypes
{
	SPELL_TARGET_TYPE_REQUIRE_NO_CHECK			= 0,
	SPELL_TARGET_TYPE_REQUIRE_FACTION_CHECK		= 1,
	SPELL_TARGET_TYPE_REQUIRE_ENTRY_CHECK		= 2,
};

#define MAX_SPELL_REAGENTS 8
#define MAX_SPELL_TOTEMS 2
#define MAX_SPELL_TOTEM_CATEGORIES 2

// SpellAuraOptions.dbc
struct SpellAuraOptionsEntry
{
    uint32    Id;                                           // 0        m_ID
    uint32    StackAmount;                                  // 51       m_cumulativeAura
    uint32    procChance;                                   // 38       m_procChance
    uint32    procCharges;                                  // 39       m_procCharges
    uint32    procFlags;                                    // 37       m_procTypeMask
};

// SpellAuraRestrictions.dbc
struct SpellAuraRestrictionsEntry
{
    uint32    Id;                                           // 0        m_ID
    uint32    CasterAuraState;                              // 21       m_casterAuraState
    uint32    TargetAuraState;                              // 22       m_targetAuraState
    uint32    CasterAuraStateNot;                           // 23       m_excludeCasterAuraState
    uint32    TargetAuraStateNot;                           // 24       m_excludeTargetAuraState
    uint32    casterAuraSpell;                              // 25       m_casterAuraSpell
    uint32    targetAuraSpell;                              // 26       m_targetAuraSpell
    uint32    excludeCasterAuraSpell;                       // 27       m_excludeCasterAuraSpell
    uint32    excludeTargetAuraSpell;                       // 28       m_excludeTargetAuraSpell
};

// SpellCastingRequirements.dbc
struct SpellCastingRequirementsEntry
{
    uint32    Id;                                           // 0        m_ID
    uint32    FacingCasterFlags;                            // 20       m_facingCasterFlags
    //uint32    MinFactionId;                               // 159      m_minFactionID not used
    //uint32    MinReputation;                              // 160      m_minReputation not used
    int32     AreaGroupId;                                  // 164      m_requiredAreaGroupId
    //uint32    RequiredAuraVision;                         // 161      m_requiredAuraVision not used
    uint32    RequiresSpellFocus;                           // 19       m_requiresSpellFocus
};

// SpellCategories.dbc
struct SpellCategoriesEntry
{
    uint32    Id;                                           // 0        m_ID
    uint32    Category;                                     // 1        m_category
    uint32    DmgClass;                                     // 153      m_defenseType
    uint32    Dispel;                                       // 2        m_dispelType
    uint32    Mechanic;                                     // 3        m_mechanic
    uint32    PreventionType;                               // 154      m_preventionType
    uint32    StartRecoveryCategory;                        // 145      m_startRecoveryCategory
};

// SpellClassOptions.dbc
struct SpellClassOptionsEntry
{
    uint32    Id;                                         // 0        m_ID
    //uint32    modalNextSpell;                             // 50       m_modalNextSpell not used
    uint32    SpellFamilyFlags[3];                          // 149-150  m_spellClassMask NOTE: size is 12 bytes!!!
    uint32    SpellFamilyName;                              // 148      m_spellClassSet
    //char*   Description;                                  // 6 4.0.0
};

// SpellCooldowns.dbc
struct SpellCooldownsEntry
{
    uint32    Id;                                          // 0        m_ID
    int32    CategoryRecoveryTime;                         // 31       m_categoryRecoveryTime
    int32    RecoveryTime;                                 // 30       m_recoveryTime
    int32    StartRecoveryTime;                            // 146      m_startRecoveryTime
};

// SpellEffect.dbc
#define SPELL_EFFECT_MAGIC_UNDECIDED_SPELL_POWER_COEFF	0.0000001f
#define ITEM_SPELL_DEFAULT_SP_BENEFIT_COEF				0.001f

struct SpellEffectEntry
{
    //uint32    Id;                                           // 0        m_ID
    uint32    Effect;                                       // 73-75    m_effect
    float     EffectConversionCoef;                         // 106-108  m_effectAmplitude
    uint32    EffectApplyAuraName;                          // 100-102  m_effectAura
    uint32    EffectAmplitude;                              // 103-105  m_effectAuraPeriod
    int32     EffectBasePoints;                             // 82-84    m_effectBasePoints (don't must be used in spell/auras explicitly, must be used cached Spell::m_currentBasePoints)
    float	  EffectSpellPowerCoef;                         // 169-171  
    float     EffectChainMultiplier;                         // 156-158  m_effectChainAmplitude
	int32     EffectChainTarget;                            // 109-111  m_effectChainTargets
    int32     EffectDieSides;                               // 76-78    m_effectDieSides
    uint32    EffectItemType;                               // 112-114  m_effectItemType
    uint32    EffectMechanic;                               // 85-87    m_effectMechanic
    int32     EffectMiscValue;                              // 115-117  m_effectMiscValue
    int32     EffectMiscValueB;                             // 118-120  m_effectMiscValueB
    float     EffectPointsPerComboPoint;                    // 124-126  m_effectPointsPerCombo
    uint32    EffectRadiusIndex;                            // 94-96    m_effectRadiusIndex - spellradius.dbc
    uint32    EffectRadiusMaxIndex;                        // 97-99    4.0.0
    float     EffectRealPointsPerLevel;                     // 79-81    m_effectRealPointsPerLevel
    uint32    EffectSpellGroupRelation[3];                  // 127-129  m_effectSpellClassMaskA, effect 0
    uint32    EffectTriggerSpell;                           // 121-123  m_effectTriggerSpell
    uint32    EffectImplicitTargetA;                        // 88-90    m_implicitTargetA
    uint32    EffectImplicitTargetB;                        // 91-93    m_implicitTargetB
    uint32    EffectSpellId;                                // new 4.0.0 - most important, we will attach effects to spells based on this :P
    uint32    EffectIndex;                                  // new 4.0.0
};

// SpellEquippedItems.dbc
struct SpellEquippedItemsEntry
{
    uint32    Id;                                           // 0        m_ID
    int32     EquippedItemClass;                            // 70       m_equippedItemClass (value)
    int32     EquippedItemInventoryTypeMask;                // 72       m_equippedItemInvTypes (mask)
    int32     EquippedItemSubClassMask;                     // 71       m_equippedItemSubclass (mask)
};

// SpellInterrupts.dbc
struct SpellInterruptsEntry
{
    uint32    Id;                                           // 0        m_ID
    uint32    AuraInterruptFlags;                           // 33       m_auraInterruptFlags
    //uint32                                                // 34       4.0.0
    uint32    ChannelInterruptFlags;                        // 35       m_channelInterruptFlags
    //uint32                                                // 36       4.0.0
    uint32    InterruptFlags;                               // 32       m_interruptFlags
};

// SpellLevels.dbc
struct SpellLevelsEntry
{
    uint32    Id;                                           // 0        m_ID
    uint32    baseLevel;                                    // 41       m_baseLevel
    uint32    maxLevel;                                     // 40       m_maxLevel
    uint32    spellLevel;                                   // 42       m_spellLevel
};

// SpellPower.dbc
struct SpellPowerEntry
{
    uint32    Id;                                           // 0        m_ID
    uint32    manaCost;                                     // 45       m_manaCost
    uint32    manaCostPerlevel;                             // 46       m_manaCostPerLevel
    uint32    ManaCostPercentage;                           // 144      m_manaCostPct
    uint32    manaPerSecond;                                // 47       m_manaPerSecond
    //uint32  PowerDisplayId;                               // 168      PowerDisplay.dbc, new in 3.1
    //uint32  unk1;                                         // 6        4.0.0
};

// SpellReagents.dbc
struct SpellReagentsEntry
{
    uint32    Id;                                           // 0        m_ID
    int32     Reagent[MAX_SPELL_REAGENTS];                  // 54-61    m_reagent
    uint32    ReagentCount[MAX_SPELL_REAGENTS];             // 62-69    m_reagentCount
};

// SpellScaling.dbc
struct SpellScalingEntry
{
    uint32    Id;                                           // 0        m_ID
    int32     castTimeMin;                                  // 1
    int32     castTimeMax;                                  // 2
    int32     castScalingMaxLevel;                          // 3
    int32     playerClass;                                  // 4        (index * 100) + charLevel => gtSpellScaling.dbc
    float     coeff_points_gtscale[3];                      // 5-7
    float     coeff_randompoints_gtscale[3];                // 5-7
    float     coeff_combopoint_extra_gtscale[3];            // 5-7		//formula what this will multiply is still custom. Like weapon dmg, or base value or duration ...
    float     effLevelDiffCoeff;                            // 14        some coefficient, mostly 1.0f
    int32     effScalingMinLevel;                           // 15        some level
};

// SpellShapeshift.dbc
struct SpellShapeshiftEntry
{
    uint32    Id;                                           // 0        m_ID
    uint32    Stances;                                      // 13       m_shapeshiftMask
    // uint32 unk_320_2;                                    // 14       3.2.0
    uint32    StancesNot;                                   // 15       m_shapeshiftExclude
    // uint32 unk_320_3;                                    // 16       3.2.0
    // uint32    StanceBarOrder;                            // 155      m_stanceBarOrder not used
};

// SpellTargetRestrictions.dbc
struct SpellTargetRestrictionsEntry
{
    uint32    Id;                                           // 0        m_ID
    uint32    MaxAffectedTargets;                           // 152      m_maxTargets
    uint32    MaxTargetLevel;                               // 147      m_maxTargetLevel
    uint32    TargetCreatureType;                           // 18       m_targetCreatureType
    uint32    Targets;                                      // 17       m_targets
};

// SpellTotems.dbc
struct SpellTotemsEntry
{
    uint32    Id;                                           // 0        m_ID
    uint32    TotemCategory[MAX_SPELL_TOTEM_CATEGORIES];    // 162-163  m_requiredTotemCategoryID
    uint32    Totem[MAX_SPELL_TOTEMS];                      // 52-53    m_totem
};

struct SpellShapeshiftFormEntry
{
    uint32 ID;                                              // 0
    //uint32 buttonPosition;                                // 1 unused
    //char*  Name;                                          // 2 unused
    uint32 flags1;                                          // 3
    int32  creatureType;                                    // 4 <=0 humanoid, other normal creature types
    //uint32 unk1;                                          // 5 unused, related to next field
    uint32 attackSpeed;                                     // 6
    uint32 modelID_A;                                       // 7 alliance modelid (0 means no model)
    uint32 modelID_H;                                       // 8 horde modelid (but only for one form)
    //uint32 unk3;                                          // 9 unused always 0
    //uint32 unk4;                                          // 10 unused always 0
    uint32 spellId[8];                                      // 11-18 spells which appear in the bar after shapeshifting
	//uint32 unk5;
	//uint32 unk6;
};

#define MAX_DIFFICULTY 4	
struct SpellDifficultyEntry
{
    uint32 ID;                                              // 0        m_ID
    uint32 spellId[MAX_DIFFICULTY];                         // 1-4      m_spellId[4]
};

struct SpellFocusObjectEntry
{
    uint32    ID;                                           // 0
    //char*     Name;                                       // 1
};

class SpellEffectOverrideScript;
class SpellCanCastScript;
class SpellCanTargetedScript;
class Object;

#define TEMP_DISABLE_SPELL_COEFS		//seems like 403 client is not using this anymore according to GUI
#define MAX_SPELL_EFFECT_COUNT		3	//don't be an idiot and put a value that is not initialized by DBC, you will have random spells poping up
// Struct for the entry in Spell.dbc
struct SpellEntry
{
    uint32    Id;                                           // 0        m_ID
    uint32    Attributes;                                   // 1        m_attribute
    uint32    AttributesEx;                                 // 2        m_attributesEx
    uint32    AttributesExB;                                // 3        m_attributesExB
    uint32    AttributesExC;                                // 4        m_attributesExC
    uint32    AttributesExD;                                // 5        m_attributesExD
    uint32    AttributesExE;                                // 6        m_attributesExE
    uint32    AttributesExF;                                // 7        m_attributesExF
    uint32    unk_320_1;                                    // 8        3.2.0 (0x20 - totems, 0x4 - paladin auras, etc...)
    uint32    someFlags;                                    // 9        4.0.0
    uint32    unk_400_1;                                    // 10       4.0.0
    uint32    CastingTimeIndex;                             // 11       m_castingTimeIndex
    uint32    DurationIndex;                                // 12       m_durationIndex
    uint32    powerType;                                    // 13       m_powerType
    uint32    rangeIndex;                                   // 14       m_rangeIndex
    float     speed;                                        // 15       m_speed
    uint32    SpellVisual[2];                               // 16-17    m_spellVisualID
    uint32    SpellIconID;                                  // 18       m_spellIconID
    uint32    activeIconID;                                 // 19       m_activeIconID
    char*	  Name;											// 20       m_name_lang
    char*	  Rank;                                         // 21       m_nameSubtext_lang
    char*	  Description;                                  // 22       m_description_lang not used
    char*	  ToolTip;                                      // 23       m_auraDescription_lang not used
    uint32    SchoolMask;                                   // 24       m_schoolMask
    uint32    RuneCostID;                                   // 25       m_runeCostID
    uint32    spellMissileID;                               // 26       m_spellMissileID not used
    uint32	  spellDescriptionVariableID;                   // 27       3.2.0
    uint32    SpellDifficultyId;                              // 28       m_spellDifficultyID - id from SpellDifficulty.dbc
    float     AttackPowerToSpellDamageCoeff;                   // 29			//attack power to dmg contribution
    uint32    SpellScalingId;                                  // 30       SpellScaling.dbc
    uint32    SpellAuraOptionsId;                              // 31       SpellAuraOptions.dbc
    uint32    SpellAuraRestrictionsId;                         // 32       SpellAuraRestrictions.dbc
    uint32    SpellCastingRequirementsId;                      // 33       SpellCastingRequirements.dbc
    uint32    SpellCategoriesId;                               // 34       SpellCategories.dbc
    uint32    SpellClassOptionsId;                             // 35       SpellClassOptions.dbc
    uint32    SpellCooldownsId;                                // 36       SpellCooldowns.dbc
    uint32    unkIndex7;                                       // 37       all zeros...
    uint32    SpellEquippedItemsId;                            // 38       SpellEquippedItems.dbc
    uint32    SpellInterruptsId;                               // 39       SpellInterrupts.dbc
    uint32    SpellLevelsId;                                   // 40       SpellLevels.dbc
    uint32    SpellPowerId;                                    // 41       SpellPower.dbc
    uint32    SpellReagentsId;                                 // 42       SpellReagents.dbc
    uint32    SpellShapeshiftId;                               // 43       SpellShapeshift.dbc
    uint32    SpellTargetRestrictionsId;                       // 44       SpellTargetRestrictions.dbc
    uint32    SpellTotemsId;                                   // 45       SpellTotems.dbc
    uint32    ResearchProject;                                 // 46  

    /// CUSTOM: these fields are used for the modifications made in the world.cpp
//	uint32									eff_count;		//will attach N effects here
	//why more then 3 ? to be able to add custom effects. Some of us are not jerks about 1 MB extra memory
	SpellEffectEntry						eff[MAX_SPELL_EFFECT_COUNT];				//max 3 effects
	SpellScalingEntry						ss;	//this one is used all the time
	SpellAuraRestrictionsEntry				sar;	
	int32									maxstack;		//used for hackfixes
	uint32									procCharges;
	//this is accessed like 20 time at every spell cast. Let's not complicate it with linked lookups
	uint32									SpellGroupType[3];							//96 bits
	uint32									School;
	uint32									AuraInterruptFlags;
	uint32									InterruptFlags;
	uint32									ChannelInterruptFlags;
	int32									RecoveryTime;
	uint32									StartRecoveryCategory;
	int32									StartRecoveryTime;
	uint32									Category;
	int32									CategoryRecoveryTime;
	uint32									MechanicsType;
	uint32									MechanicsTypeFlags;	//combined all mechanics for removal like "imparring"
	uint32									PreventionType;
	uint32									RequiredShapeShift;
	bool									RemoveOnShapeShift; // vengeance / starfall
//	uint32									spellLevel;
	uint32									FacingCasterFlags;
	uint32									RequiredAreaId;
    uint32 DiminishStatus;                  //
	uint32 DiminishStatusMaxPVPDur;			// i wonder if this is set by blizz somewhere
    uint32 proc_interval;                   //!!! CUSTOM, <Fill description for variable>
	uint32 procChance;						//we will mod proc chance, keeping a backup mostly for debugging
	uint32 procFlags;
	uint32 procFlags2;						//since old flag list is too small
	uint32 procFlagsRemove;					//charge disipates if these proc flags are met. This is implemented wrongly. We should test the spell that will proc for flags. But we already register procs based on owner spell and not the proccing spell
	//Buff Groupin Rule -> caster can cast this spell only on 1 target. Value represents the group spell is part of. Can be part of only 1 group
	//caster can only cast on 1 target these spells
    uint32	BGR_one_buff_from_caster_on_1target;	 //!!! CUSTOM, <Fill description for variable>
	//target can have only buff of this type on self. Value represents the group spell is part of. Can be part of only 1 group
    uint32	BGR_one_buff_on_target;					 //!!! CUSTOM, these are related to creating a item through a spell
	//all positive buffs should have this true and negative buffs as false
	bool	BGR_one_buff_on_target_skip_caster_check;	//addition for previous. 90% of the time check who casted this spell on target. Like blessings should stack
	//caster can have only 1 Aura per spell group, ex pal auras
    uint32	BGR_one_buff_from_caster_on_self;        //!!! CUSTOM, these are related to creating a item through a spell
	bool	HasStackGroupFlags;
#define MAX_SPELL_STACK_GROUPS	3
	union
	{
		uint64	StackGroupMasks[MAX_SPELL_STACK_GROUPS];		//spells from same group need to apply stacking rule. Imported from trinity. bit0 from mask 1 is set to 1 if there are any values
		uint32	StackGroupMasks32[MAX_SPELL_STACK_GROUPS*2];	//spells from same group need to apply stacking rule. Imported from trinity. bit0 from mask 1 is set to 1 if there are any values
	};
//    uint32 buffIndexType;					//!!! CUSTOM, <Fill description for variable>
    uint32 c_is_flags;						//!!! CUSTOM, store spell checks in a static way : isdamageind,ishealing
    uint32 c_is_flags2;						//!!! CUSTOM, store spell checks in a static way : isdamageind,ishealing
    uint32 c_is_flags3;						//!!! CUSTOM, store spell checks in a static way : isdamageind,ishealing
//    float AP_spell_bonus_base[3];			//!!! CUSTOM, mod GetAP() include it as base value so other talents can scale it 
    float AP_spell_bonus_total[3];			//!!! CUSTOM, mod GetAP() include it as a final value without talents scaling it
//    uint32 buffType;                      //!!! CUSTOM, these are related to creating a item through a spell
    uint32 RankNumber;                      //!!! CUSTOM, this protects players from having >1 rank of a spell
    uint32 NameHash;                        //!!! CUSTOM, related to custom spells, summon spell quest related spells
	//this is the group the spell will affect : each effect has it's own mask
//    uint32 EffectSpellGroupRelation[3][3];     //!!! this is not contained in client dbc but server must have it
	uint32 ThreatForSpell;					//some basic calculation for spell threath
	float ThreatForSpellCoef;				//theoretically setting this to 0 would make a spell not do threath
	uint32 ProcOnNameHash[3];

	float base_range_or_radius_sqr;         //!!! CUSTOM, needed for aoe spells most of the time
	// love me or hate me, all "In a cone in front of the caster" spells don't necessarily mean "in front"
	float cone_width;
	uint32 dmg_type;
	//Spell Coefficient
	float SpellPowerCoefAvg;
	bool Spell_value_is_scripted;							// for fixed spell value calculations this is required.
	int	ai_target_type;
	//!!!!!!!!!!! 1 spell may be present in multiple talent entries ! Ex : pet shared talents = cobra reflexes
#define SPELL_SKILL_LINK_LIMIT 5
	uint32	spell_skilline[SPELL_SKILL_LINK_LIMIT];			//skilline represents the icon in client spellbook
	uint32	spell_skilline_assoc_counter;					//spell may be present in multiple skillines
	TalentEntry *talent_entry[SPELL_SKILL_LINK_LIMIT];		//if this spell is obtained from a talent then we are able to look it up
	uint32	talent_entry_rank[SPELL_SKILL_LINK_LIMIT];		//if this spell is obtained from a talent then we are able to look it up
	uint32	talent_entry_assoc_counter;						//count this spell in how many talent trees is present
	uint32  talent_tree[SPELL_SKILL_LINK_LIMIT];			//multiple talents have multiple trees
	uint32	talent_remove_auras[SPELL_SKILL_LINK_LIMIT];	//if script is adding or casting auras that should get removed on talent change, add them here
	bool	spell_learn_is_linked_to_talent;				//when changing talent spec we need to know which spells to unlearn. If spell was learned by a talent then we need to remove it.
	uint8	belongs_to_player_class;						//this was added for talent specializations that mod all fire dmg. But should not mod fire dmg procced from items
	uint32	spell_learn_next_level_spell;					//used for iterating through all levels of a spell : kinda like selecting spells with same namehash but this is based on SkillLineDBC
	skilllinespell	*spell_skill;							//in case there is one spell would be part of some skillspell
	uint32	spell_id_client;								//use this ID fro client isntead real ID
	uint32	quick_duration_min;
	uint32	quick_tickcount;
	uint32	quick_maxAmplitude;								//so that spells can tick last effect
	uint32	quick_ExtendDuration;
	SpellEntry	*chained_cast;								// cast this spell also when casting the original spell. Used for recently bandaged, forbearance and other spells that require more then 3 effects
	SpellEntry	*aura_remove_cast;							// if spell is dispelled, expires, canceled, this spell is casted by original caster on target(if possible). Used very rarely
	uint32	RequiredPlayerClass;							// this was added for aura 102 and 103 to not be exploitable by other classes
	uint32	MaxAffectedTargets;
	uint32	ForcedAuraVisibleFlags;							//should tell client to enable the casting of this spell
	uint32	ReplacingSpell;									//some spells temp replace others while specific aura is active. Needs manual scripting

	bool self_cast_only;
	bool apply_on_shapeshift_change;
	bool is_melee_spell;					//!!! CUSTOM,
	bool is_ranged_spell;					//!!! CUSTOM,
//	bool spell_can_crit;					//!!! CUSTOM,
	bool EffectCanCrit[MAX_SPELL_EFFECT_COUNT];
//	bool noproc;

	SPELL_EFFECT_OVERRIDE_RETURNS		(*EffectHandler)(Spell *sp,Unit *target, int32 &value, int32 i);	//handle special effect value calculations
	void								(*ProcHandler)(ProcHandlerContextShare *context);	//i hope 1 handler / spell is enough
	void								(*ChargeHandler)(ProcHandlerContextShare *context);	//i hope 1 handler / spell is enough
	uint32								(*CanCastHandler)(Spell *sp);	
	uint32								(*CanCastFilterHandler)(Spell *sp, SpellCanCastScript *ccs );			//caster can have filters to cast new spells
	uint32								(*CanTargetedFilterHandler)(Spell *sp, Object *Caster, Object *FilterOwner, SpellCanTargetedScript *cts );	//can we get targeted by some spells ? immunity handlers
	void								(*TargettingOverride[3])(Spell *sp,uint8 i);				//can script each effect how to gather their targettings
	SPELL_EFFECT_OVERRIDE_RETURNS		(*AuraHandleHook)( Aura *aur, bool apply, uint8 i);
	bool								(*AuraPeriodicDummyTickHook)(uint32 i, Aura * pAura, bool apply);
	bool								(*SpellDummyEffectScript)( uint32 i, Spell* pSpell);
	void								(*CritHandler)(ProcHandlerContextShare *context);	//i hope 1 handler / spell is enough
	void								(*EffRedirectHandler)( Spell *sp );	
	bool								(*AuraInterruptHandler)(Unit *owner, Aura *a,uint32 flag, uint32 skip_casted, uint32 skiphash_casted, uint32 dmg);
	//note this does not support more then 1 effects
	SPELL_EFFECT_OVERRIDE_RETURNS		(*ClassOverideApplyHandler)(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i,SpellEffectOverrideScript *so);
	SPELL_EFFECT_OVERRIDE_RETURNS		(*ClassOverideRemoveHandler)(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i,SpellEffectOverrideScript *so);
	int32								required_target_type;		//player - 1, mob = 2, ... !!! db has value -1 to be mangos compatible
	SimpleVect<uint32>					required_target_value;		//faction for 1 and entry for 2
#define MAX_AURA_STATES_TO_SET			3
	uint32								SetAuraStatesCount,SetAuraStates[MAX_AURA_STATES_TO_SET],RemAuraStates[MAX_AURA_STATES_TO_SET];
	int32								RemoveCasterAuraState;	//when casting a spell and we require to have X aurastate, it does not always mean we need to remove it
//methods ( ofc we all know that structures do not have methods. Class does )
	uint32 *GetSpellGroupType();
//	uint32 GetChannelInterruptFlags();
//	uint32 GetRecoveryTime();
//	uint32 GetInterruptFlags();
//	uint32 GetCategoryRecoveryTime();
//	uint32 GetRequiredShapeShift();
	uint32 GetCasterAuraState(){ return sar.CasterAuraState; }
	uint32 GetTargetAuraState(){ return sar.TargetAuraState; }
	uint32 GetTargetAuraStateNot(){ return sar.TargetAuraStateNot; }
	uint32 GetCasterAuraStateNot(){ return sar.CasterAuraStateNot; }
	uint32 GetCasterAuraSpell(){ return sar.casterAuraSpell; }
	uint32 GetCasterAuraSpellNot(){ return sar.excludeCasterAuraSpell; }
	uint32 GetTargetAuraSpell(){ return sar.targetAuraSpell; }
	uint32 GetTargetAuraSpellNot(){ return sar.excludeTargetAuraSpell; }
	uint32 GetRequiresSpellFocus();
//	uint32 GetRequiresAreaId();
	uint32 GetRequiresAreaId(){ return RequiredAreaId; } 
	uint32 *GetTotem();
	int32  *GetReagent();
	uint32 *GetReagentCount();
	int32 GetEquippedItemClass();
	int32 GetEquippedItemSubClass();
	uint32 GetRequiredItemFlags();
	uint32 GetSpellDMGType(){ return dmg_type; }
#define NO_MAX_TARGETS_DEFINED 65535 
	uint32 GetMaxTargets(){ return MaxAffectedTargets; }
	uint32 GetDispelType();
	SpellPowerEntry PowerEntry;
//	uint32 GetManaCost();
//	uint32 GetManaCostPCT();
	SpellLevelsEntry	SpellLevel;
//	uint32 GetBaseLevel();
//	uint32 GetMaxLevel();
//	uint32 GetLevel();
};

struct GuildPerkSpellEntry
{
	uint32 row_id;
	uint32 req_level;
	uint32 spell_id;
};

struct TeamContributionPoints
{
    uint32    Entry;                                      // 0
    float     Value;                                        // 1 (???)
};

struct ItemExtendedCostEntry
{
    uint32 costid;
    uint32 just_0_1;
    uint32 just_0_2;
	uint32 new_320;
    uint32 item[5];
    uint32 count[5];
	uint32 personalrating;	//can be both BG or arena rating
	uint32 arena;			//found all 0, i'm guessing
    uint32 currency[5];
    uint32 currency_count[5];
	uint32 unk;
	//uint32 sometextmaybe; //all pointed to null string
};

struct TalentTabEntry
{
    uint32  TalentTabID;                                    // 0
    //char* name;                                           // 1, unused
    //unit32  spellicon;                                    // 2
    uint32  ClassMask;                                      // 3
    uint32  petTalentMask;                                  // 4
    uint32  tabpage;                                        // 5
    //char* internalname;                                   // 6
    //char* description;                                    // 7
    //uint32 rolesMask;                                     // 8 4.0.0
    uint32  MasterySpellIds[2];                             // 9-10 passive mastery bonus spells?
};

struct SpellCastTime
{
    uint32 ID;
    uint32 CastTime;
    //uint32 unk1;
    //uint32 unk2;
};

struct SpellRadius
{
    uint32 ID;
    float Radius;
    //float unk1;
    float Radius2;
};

struct SpellRange
{
    uint32 ID;
    float minRange;
    float minRangeFriendly;
    float maxRange;
    float maxRangeFriendly;
    //uint32 unks[35];
};

struct SpellDuration
{
    uint32 ID;
    uint32 Duration1;
    uint32 Duration2;
    uint32 Duration3;
};

#define RANDOM_PROPERTY_ENCHANT_COUNT 3
struct RandomProps
{
    uint32 ID;
    //uint32 name1;
    uint32 enchantments[RANDOM_PROPERTY_ENCHANT_COUNT];
    //uint32 unk1;
    //uint32 unk2;
    //uint32 name2;
    //uint32 RankAlt1;
    //uint32 RankAlt2;
    //uint32 RankAlt3;
    //uint32 RankAlt4;
    //uint32 RankAlt5;
    //uint32 RankAlt6;
    //uint32 RankAlt7;
    //uint32 RankAlt8;
    //uint32 RankAlt9;
    //uint32 RankAlt10;
    //uint32 RankAlt11;
    //uint32 RankAlt12;
    //uint32 RankAlt13;
    //uint32 RankAlt14;
    //uint32 RankAlt15;
    //uint32 RankFlags;

};

enum AreaFlags
{
    AREA_FLAG_SNOW                  = 0x00000001,           // snow (only Dun Morogh, Naxxramas, Razorfen Downs and Winterspring)
    AREA_FLAG_UNK1                  = 0x00000002,           // may be necropolis?
    AREA_FLAG_UNK2                  = 0x00000004,           // Only used for areas on map 571 (development before)
    AREA_FLAG_SLAVE_CAPITAL         = 0x00000008,           // city and city subsones
    AREA_FLAG_UNK3                  = 0x00000010,           // can't find common meaning
    AREA_FLAG_SLAVE_CAPITAL2        = 0x00000020,           // slave capital city flag?
    AREA_FLAG_UNK4                  = 0x00000040,           // many zones have this flag
    AREA_FLAG_ARENA                 = 0x00000080,           // arena, both instanced and world arenas
    AREA_FLAG_CAPITAL               = 0x00000100,           // main capital city flag
    AREA_FLAG_CITY                  = 0x00000200,           // only for one zone named "City" (where it located?)
    AREA_FLAG_OUTLAND               = 0x00000400,           // expansion zones? (only Eye of the Storm not have this flag, but have 0x00004000 flag)
    AREA_FLAG_SANCTUARY             = 0x00000800,           // sanctuary area (PvP disabled)
    AREA_FLAG_NEED_FLY              = 0x00001000,           // only Netherwing Ledge, Socrethar's Seat, Tempest Keep, The Arcatraz, The Botanica, The Mechanar, Sorrow Wing Point, Dragonspine Ridge, Netherwing Mines, Dragonmaw Base Camp, Dragonmaw Skyway
    AREA_FLAG_UNUSED1               = 0x00002000,           // not used now (no area/zones with this flag set in 3.0.3)
    AREA_FLAG_OUTLAND2              = 0x00004000,           // expansion zones? (only Circle of Blood Arena not have this flag, but have 0x00000400 flag)
    AREA_FLAG_PVP                   = 0x00008000,           // pvp objective area? (Death's Door also has this flag although it's no pvp object area)
    AREA_FLAG_ARENA_INSTANCE        = 0x00010000,           // used by instanced arenas only
    AREA_FLAG_UNUSED2               = 0x00020000,           // not used now (no area/zones with this flag set in 3.0.3)
    AREA_FLAG_UNK5                  = 0x00040000,           // only used for Amani Pass, Hatchet Hills
    AREA_FLAG_UNK6                  = 0x00080000,           // Valgarde and Acherus: The Ebon Hold
    AREA_FLAG_LOWLEVEL              = 0x00100000,           // used for some starting areas with area_level <=15
    AREA_FLAG_TOWN                  = 0x00200000,           // small towns with Inn
    AREA_FLAG_UNK7                  = 0x00400000,           // Warsong Hold, Acherus: The Ebon Hold, New Agamand Inn, Vengeance Landing Inn
    AREA_FLAG_UNK8                  = 0x00800000,           // Westguard Inn, Acherus: The Ebon Hold, Valgarde
    AREA_FLAG_OUTDOOR_PVP           = 0x01000000,           // Wintergrasp and it's subzones
    AREA_FLAG_INSIDE                = 0x02000000,           // used for determinating spell related inside/outside questions in Map::IsOutdoors
    AREA_FLAG_OUTSIDE               = 0x04000000,           // used for determinating spell related inside/outside questions in Map::IsOutdoors
    AREA_FLAG_CAN_HEARTH_AND_RES    = 0x08000000,           // Wintergrasp and it's subzones
    AREA_FLAG_CANNOT_FLY            = 0x20000000            // not allowed to fly, only used in Dalaran areas (zone 4395)
};

enum AreaTeams
{
    AREATEAM_NONE  = 0,
    AREATEAM_ALLY  = 2,
    AREATEAM_HORDE = 4
};

struct AreaTable
{
    uint32  AreaId;                                         // 0
    uint32  mapId;                                          // 1
    uint32  ZoneId;                                         // 2 if 0 then it's zone, else it's zone id of this area
    uint32  explorationFlag;                                // 3, main index
    uint32  AreaFlags;                                      // 4, unknown value but 312 for all cities
                                                            // 5-9 unused
    int32   level;                                          // 10
    char    *name;                                          // 11
    uint32  teamflags;                                           // 12
                                                            // 13-19 unknown
    //uint32 unk20;                                         // 20 4.0.0
    //uint32 unk21;                                         // 21 4.0.0
    //uint32 unk22;                                         // 22 4.0.0
    //uint32 unk23;                                         // 23 4.0.0
    //uint32 unk24;                                         // 24 4.0.1, may be worldStateId
	uint32 GetTeam()
	{
		if( teamflags == AREATEAM_ALLY )
			return 0;
		if( teamflags == AREATEAM_HORDE )
			return 1;
		return 2;
	}
};

struct FactionTemplateDBC
{
    uint32 ID;
    uint32 Faction;
    uint32 FactionGroup;
    uint32 Mask;
    uint32 FriendlyMask;
    uint32 HostileMask;
    uint32 EnemyFactions[4];
    uint32 FriendlyFactions[4];
};

struct AuctionHouseDBC
{
    uint32 id;
    uint32 unk;
    uint32 fee;
    uint32 tax;
    //char* name;
    //char* nameAlt1;
    //char* nameAlt2;
    //char* nameAlt3;
    //char* nameAlt4;
    //char* nameAlt5;
    //char* nameAlt6;
    //char* nameAlt7;
    //char* nameAlt8;
    //char* nameAlt9;
    //char* nameAlt10;
    //char* nameAlt11;
    //char* nameAlt12;
    //char* nameAlt13;
    //char* nameAlt14;
    //char* nameAlt15;
    //char* nameFlags;
};

struct FactionDBC
{
    uint32 ID;
    int32 RepListId;
    uint32 RaceMask[4];
    uint32 ClassMask[4];
    int32 baseRepValue[4];
    uint32 repFlags[4];
    uint32 parentFaction;
    char* Name;
    //uint32 shit[16];
    //uint32 Description;
    //uint32 shit2[16];
};

struct DBCTaxiNode
{
    uint32 id;
    uint32 mapid;
    float x;
    float y;
    float z;
    //uint32 name;
    //uint32 namealt1;
    //uint32 namealt2;
    //uint32 namealt3;
    //uint32 namealt4;
    //uint32 namealt5;
    //uint32 namealt6;
    //uint32 namealt7;
    //uint32 namealt8;
    //uint32 namealt9;
    //uint32 namealt10;
    //uint32 namealt11;
    //uint32 namealt12;
    //uint32 namealt13;
    //uint32 namealt14;
    //uint32 namealt15;
    //uint32 nameflags;
    uint32 horde_mount;
    uint32 alliance_mount;
	uint32 unk1;
	float  unk2;
	float  unk3;
};

struct DBCTaxiPath
{
    uint32 id;
    uint32 from;
    uint32 to;
    uint32 price;
};

struct DBCTaxiPathNode
{
    uint32 id;
    uint32 path;
    uint32 seq;
    uint32 mapid;
    float x;
    float y;
    float z;
    uint32 unk1;
    uint32 waittime;
    //uint32 unk2;
    //uint32 unk3;
};

struct CreatureSpellDataEntry
{
    uint32 id;
    uint32 Spells[3];
    uint32 PHSpell;
    uint32 Cooldowns[3];
    uint32 PH;
};

struct CharRaceEntry
{
   uint32      RaceID;                                     // 0
                                                            // 1 unused
    uint32      FactionID;                                  // 2 faction template id
                                                            // 3 unused
    uint32      model_m;                                    // 4
    uint32      model_f;                                    // 5
                                                            // 6 unused
    uint32      TeamID;                                     // 7 (7-Alliance 1-Horde)
                                                            // 8-11 unused
    uint32      CinematicSequence;                          // 12 id from CinematicSequences.dbc
    //uint32    unk_322;                                    // 13 faction (0 alliance, 1 horde, 2 not available?)
    char		*name;                                         // 14 used for DBC language detection/selection
    //char*       nameFemale;                               // 15, if different from base (male) case
    //char*       nameNeutralGender;                        // 16, if different from base (male) case
                                                            // 17-19 unused
    uint32      expansion;                                  // 20 (0 - original race, 1 - tbc addon, ...)
    //uint32                                                // 21 (23 for worgens)
    //uint32                                                // 22 4.0.0
    //uint32                                                // 23 4.0.0
};

struct CharClassEntry
{
    uint32 class_id;
    //uint32 unk1;
    uint32 power_type;
    //uint32 unk2;
    char* name;
    //uint32 namealt1;
    //uint32 namealt2;
    //uint32 namealt3;
    //uint32 namealt4;
    //uint32 namealt5;
    //uint32 namealt6;
    //uint32 namealt7;
    //uint32 namealt8;
    //uint32 namealt9;
    //uint32 namealt10;
    //uint32 namealt11;
    //uint32 namealt12;
    //uint32 namealt13;
    //uint32 namealt14;
    //uint32 namealt15;
    //uint32 nameflags;
    //uint32 unk3;
    //uint32 unk4;
    //uint32 unk5;
	//uint32 expansion;
};

struct CreatureFamilyEntry
{
    uint32 ID;
    float minsize;
    uint32 minlevel;
    float maxsize;
    uint32 maxlevel;
    uint32 skilline;
    uint32 tameable;		//second skill line - 270 Generic
    uint32 petdietflags;
	uint32 PetType;		//ferocity, cunning..
	uint32 unk2; //maybe some flags
    char* name;
    //uint32 namealt1;
    //uint32 namealt2;
    //uint32 namealt3;
    //uint32 namealt4;
    //uint32 namealt5;
    //uint32 namealt6;
    //uint32 namealt7;
    //uint32 namealt8;
    //uint32 namealt9;
    //uint32 namealt10;
    //uint32 namealt11;
    //uint32 namealt12;
    //uint32 namealt13;
    //uint32 namealt14;
    //uint32 namealt15;
    //uint32 nameflags;
    //uint32 iconFile;
};

struct MapEntry
{
    uint32  id;                                          // 0
    //char*       internalname;                             // 1 unused
    uint32  map_type;                                       // 2
    //uint32 mapFlags;                                      // 3 some kind of flags (0x100 - CAN_CHANGE_PLAYER_DIFFICULTY)
    //uint32 unk4;                                          // 4 4.0.1
    //uint32 isPvP;                                         // 5 0 or 1 for battlegrounds (not arenas)
    char*	real_name;                                      // 6
    uint32  linked_zone;                                    // 7 common zone for instance and continent map
    //char*     hordeIntro;                                 // 8 text for PvP Zones
    //char*     allianceIntro;                              // 9 text for PvP Zones
    uint32  multimap_id;                                    // 10 index in  LoadingScreens.dbc
    //float   BattlefieldMapIconScale;                      // 11 BattlefieldMapIconScale
    int32   ghost_entrance_map;                             // 12 map_id of entrance map in ghost mode (continent always and in most cases = normal entrance)
    float   ghost_entrance_x;                               // 13 entrance x coordinate in ghost mode  (in most cases = normal entrance)
    float   ghost_entrance_y;                               // 14 entrance y coordinate in ghost mode  (in most cases = normal entrance)
    //uint32  timeOfDayOverride;                            // 15 time of day override
    uint32  addon;                                          // 16 expansion
    //uint32 unkTime;                                       // 17 some kind of time?
    //uint32 maxPlayers;                                    // 18 max players
    //uint32 unk400;                                        // 19 new 4.0.0, mapid, related to phasing
};

#define SUFFIX_ENCHANT_COUNT	5
struct ItemRandomSuffixEntry
{
	uint32 id;
//	uint32 name;
//	uint32 namealt1;
//	uint32 namealt2;
//	uint32 namealt3;
//	uint32 namealt4;
//	uint32 namealt5;
//	uint32 namealt6;
//	uint32 namealt7;
//	uint32 namealt8;
//	uint32 namealt9;
//	uint32 namealt10;
//	uint32 namealt11;
//	uint32 namealt12;
//	uint32 namealt13;
//	uint32 namealt14;
//	uint32 namealt15;
//	uint32 namealtflags;
//	uint32 desc;
	uint32 enchantments[SUFFIX_ENCHANT_COUNT];
	uint32 prefixes[SUFFIX_ENCHANT_COUNT];
};

struct RandomPropertiesPointsEntry
{
    uint32	  Id;                                           // 0 hidden key
    uint32    itemLevel;                                    // 1
    uint32    EpicPropertiesPoints[5];                      // 2-6
    uint32    RarePropertiesPoints[5];                      // 7-11
    uint32    UncommonPropertiesPoints[5];                  // 12-16
}; 

struct ItemReforgeEntry
{
	uint32	ID;	//row identifier
	uint32	Stat_Source;
	float	conversion_factor;
	uint32	Stat_Dest;
	uint32	unk;	//seems to be always 1
};

struct gtFloat
{
	float val;
};

struct gtClassLevelFloat
{
	uint32 Class100Level;
	float val;
};

struct ChatChannelDBC
{
	uint32 id;
	uint32 flags;
//	char * NULLString;
	char * name_pattern[16];
};

struct DurabilityQualityEntry
{
    uint32 id;
    float quality_modifier;
};

struct DurabilityCostsEntry
{
    uint32 itemlevel;
    uint32 modifier[29];
};

struct AreaTriggerEntry
{
    uint32    id;				// 0
    uint32    mapid;			// 1
    float     x;				// 2
    float     y;				// 3
    float     z;				// 4
//    float     unk1;				// 5
//    float     unk2;				// 5
//    float     unk3;				// 5
//    float     unk4;				// 5
//    float     box_x;			// 6 extent x edge
//    float     box_y;			// 7 extent y edge
//    float     box_z;			// 8 extent z edge
//    float     box_o;			// 9 extent rotation by about z axis
};

struct QuestXPEntry
{
    uint32    level;				// 0
    uint32    rewXP1;				// 2
	uint32    rewXP2;				// 3
	uint32    rewXP3;				// 4
	uint32    rewXP4;				// 5
	uint32    rewXP5;				// 6
	uint32    rewXP6;				// 7
	uint32    rewXP7;				// 8
	uint32    rewXP8;				// 9
};

struct QuestRewRepEntry
{
    uint32   entry;					// 0
    int32    rewRep1;				// 2
	int32    rewRep2;				// 3
	int32    rewRep3;				// 4
	int32    rewRep4;				// 5
	int32    rewRep5;				// 6
	int32    rewRep6;				// 7
	int32    rewRep7;				// 8
	int32    rewRep8;				// 9
	int32    rewRep9;				// 9
};

struct CharTitlesEntry
{
	uint32		ID;
	char*		name;										// Field [2], Name
	uint32		bit_index;									// Field [36], used in PLAYER_CHOSEN_TITLE and (uint32(1) << bit_index) in PLAYER_FIELD_KNOWN_TITLES and PLAYER_FIELD_KNOWN_TITLES_X
};

#define MAX_SSDE_COLUMN	10
struct ScalingStatDistributionEntry
{
	uint32 id;
	int32 stat[MAX_SSDE_COLUMN];				//these are item bonus types, ex : spell_power
	uint32 statmodifier[MAX_SSDE_COLUMN];
	//uint32 unk1;
	uint32 maxlevel;
};

struct ScalingStatValuesEntry
{
    uint32  Id;                                             // 0
    uint32  Level;                                          // 1
    uint32  dpsMod[6];                                      // 2-7 DPS mod for level
    uint32  spellBonus;                                     // 8 spell power for level
    uint32  StatMultiplier[5];                              // 9-13 Multiplier for ScalingStatDistribution
    uint32  Armor[8][4];							        // 14-46 Armor for level
    uint32  CloakArmor;                                    // 46 unk, probably also Armor for level (flag 0x80000?)
};

struct ArmorLocationEntry
{
  uint32    InventoryType;                                  // 0
  float     Value[5];                                       // 1-5 multiplier for armor types (cloth...plate, no armor?)
};

struct ItemArmorQualityEntry
{
  uint32    Id;                                             // 0 item level
  float     Value[7];                                       // 1-7 multiplier for item quality
  uint32    Id2;                                            // 8 item level
};

struct ItemArmorShieldEntry
{
  uint32    Id;                                             // 0 item level
  uint32    Id2;                                            // 1 item level
  float     Value[7];                                       // 2-8 multiplier for item quality
};

struct ItemArmorTotalEntry
{
  uint32    Id;                                             // 0 item level
  uint32    Id2;                                            // 1 item level
  float     Value[4];                                       // 2-5 multiplier for armor types (cloth...plate)
};

struct ItemDamageEntry
{
  uint32    Id;                                             // 0 item level
  float     Value[7];                                       // 1-7 multiplier for item quality
  uint32    Id2;                                            // 8 item level
};

struct ResearchBranchEntry
{
	uint32	RowId;
	char	*Name;
	uint32	just1;
	uint32	ReqCurrency;				//not sure, too small to be spell, not valid skill
//	char	*icon;
	uint32	ScrollItemId;	//it gives you completion points if you use this type of scroll
};

struct ResearchProjectEntry
{
	uint32	RowId;
	char	*Name;
//	char	*description;
//	uint32	unk;					//almost all the time 0, 2 times "1"
	uint32	BranchId;				//pick only 1 project / branch at once
	uint32	CompletionSpellCast;	//after you gather suficient parts, you can cast this spell "solve"
	uint32	Complexity;				//not sure, seems to change witht he number of required fragments
//	char	*Icon;
	uint32	req_fragments;	//maybe
};

struct ResearchSiteEntry
{
	uint32	RowId;
	uint32	MapId;
	uint32	unk;	//i would say this needs to be poi point entry or something, maybe area/zone ID ?
	char	*Name;
	uint32	justB1;	//const
};

struct MountCapabilityEntry
{
    uint32  id;                                             // 0 index
    uint32  flag;                                           // 1 some flag
    uint32  reqSkillLevel;                                  // 2 skill level of riding required
    //uint32 unk;                                           // 3 unk
    //uint32 empty;                                         // 4 empty
    uint32  reqSpell;                                       // 5 spell that has to be known to you
    uint32  spell;                                          // 6 spell to cast to apply mount speed effects
    uint32  map;                                            // 7 map where this is applicable
};

#define MAX_MOUNT_TYPE_COLUMN 17
struct MountTypeEntry
{
    uint32  id;                                             // 0 index
    uint32  capabilities[MAX_MOUNT_TYPE_COLUMN];            // 1-17 capability ids from MountCapability.dbc
    //uint32  empty[7];                                     // 18-24 empty. maybe continues capabilities
};

struct WMOAreaTableEntry
{
	uint32 id; // 0
	int32 rootId; // 1
	int32 adtId; // 2
	int32 groupId; // 3
	//uint32 field4;
	//uint32 field5;
	//uint32 field6;
	//uint32 field7;
	//uint32 field8;
	uint32 flags; // 9
	uint32 areaId; // 10  ref -> AreaTableEntry
	//char Name[16];
	//uint32 nameflags;
};

struct CharSectionsEntry
{
//	uint32	id;
	uint32	Race;
	uint32	Gender;
	uint32	TextureType;
	char	*Texture0;
//	char	*Texture1;
//	char	*Texture2;
//	uint32	Flags;
//	uint32	Type;
	uint32	VariationId;
};

struct CharacterFacialHairStylesEntry
{
//	uint32	id;
	uint32	Race;
	uint32	Gender;
	uint32	VariationId;
//	uint32	Unk1;
//	uint32	Unk2;
//	uint32	Unk3;
//	uint32	Unk4;
//	uint32	Unk5;
};

#pragma pack(pop)

ARCEMU_INLINE float GetRadius(SpellRadius *radius)
{
    return radius->Radius;
}
ARCEMU_INLINE uint32 GetCastTime(SpellCastTime *time)
{
    return time->CastTime;
}
ARCEMU_INLINE float GetMaxRange(SpellRange *range)
{
	if( range == NULL )
		return 30.0f;	//missing range ?
    return range->maxRange;
}
ARCEMU_INLINE float GetMinRange(SpellRange *range)
{
    return range->minRange;
}
ARCEMU_INLINE uint32 GetDuration(SpellDuration *dur)
{
    return dur->Duration1;
}

//#define SAFE_DBC_CODE_RETURNS			/* undefine this to make out of range/nulls return null. */

template<class T>
class SERVER_DECL DBCStorage
{
	T * m_heapBlock;
	T * m_firstEntry;

	uint32 m_firstEntryRow;
	uint32 m_LastReadRow;	//only in case we wish to extend DBC size from a script

	T ** m_entries;
	uint32 m_max;
	uint32 m_numrows;
	uint32 m_stringlength;
	char * m_stringData;

	uint32 rows;
	uint32 cols;
	uint32 RecordSize;
	uint32 header;
	bool	ContentIsEntryOrdered;	//row[0].entry<row[1].entry 

public:
	
	DBCStorage()
	{
		m_heapBlock = NULL;
		m_entries = NULL;
		m_firstEntry = NULL;
		m_firstEntryRow = 0;
		m_max = 0;
		m_numrows = 0;
		m_stringlength=0;
		m_stringData = NULL;
		ContentIsEntryOrdered = true;
		m_LastReadRow = 0;
	}

	~DBCStorage()
	{
		if(m_heapBlock)
			free(m_heapBlock);
		if(m_entries)
			free(m_entries);
		if( m_stringData != NULL )
			free(m_stringData);
	}

	bool Load(const char * filename, const char * format, bool load_indexed, bool load_strings)
	{
		uint32 i;
		uint32 string_length;
		long pos;

		FILE * f = fopen(filename, "rb");
		if(f == NULL)
			return false;

		/* read the number of rows, and allocate our block on the heap */
		fread(&header,4,1,f);
		fread(&rows, 4, 1, f);
		fread(&cols, 4, 1, f);
		fread(&RecordSize, 4, 1, f);
		fread(&string_length, 4, 1, f);

#define DB2FmtSig 0x32424457          // WDB2
#define ADBFmtSig 0x32484357          // WCH2
		if( header == DB2FmtSig || header == ADBFmtSig )
		{
			//db2 contains a lot of crap, we need to smart skip those
			//!!! warning, only tested this for itemextendedcost in 14333 client
			fseek( f, -((int32)string_length + (int32)RecordSize * (int32)rows), SEEK_END);
		}
		pos = ftell(f);

#ifdef USING_BIG_ENDIAN
		swap32(&rows); swap32(&cols); swap32(&useless_shit); swap32(&string_length);
#endif

		if( load_strings )
		{
			fseek( f, 20 + ( rows * RecordSize ), SEEK_SET );
			m_stringData = (char*)malloc(string_length);
			m_stringlength = string_length;
			if (m_stringData)
				fread( m_stringData, string_length, 1, f );
		}

		fseek(f, pos, SEEK_SET);

		m_heapBlock = (T*)malloc( ( rows + 1 ) * sizeof(T));
		m_LastReadRow = rows;
		ASSERT(m_heapBlock);

		/* read the data for each row */
		if( strlen( format ) != cols )
		{
			printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!)\n");
			printf("!!! possible invalid format in file %s (us: %d, them: %u)\n", filename, strlen( format ), cols);
			printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!)\n");
			int temp;
			scanf("%d",&temp);
		}
		uint32 prev_entry = 0;	
		for(i = 0; i < rows; ++i)
		{
			memset(&m_heapBlock[i], 0, sizeof(T));
			ReadEntry(f, &m_heapBlock[i], format, cols, filename);

			if(load_indexed)
			{
				/* all the time the first field in the dbc is our unique entry */
				if(*(uint32*)&m_heapBlock[i] > m_max)
					m_max = *(uint32*)&m_heapBlock[i];
			}
			else if( *(uint32*)&m_heapBlock[i] < prev_entry && ContentIsEntryOrdered == true )
			{
				ContentIsEntryOrdered = false;
			}
			//rule of the thumb, rows are ordered by entry in ascending order !Need this for binary search
			//!!!! this is presumed to be never ever be run :P. Blizz already orders entry in DBC
/*			else if( *(uint32*)&m_heapBlock[i] < prev_entry )
			{
				//omg we need to percolate this row to it's correct place
				uint32 should_be_at = 0;
				for( int32 j=i;j>=0;j--)
					if( *(uint32*)&m_heapBlock[j] <= *(uint32*)&m_heapBlock[i] )
					{
						should_be_at = j;
						break;
					}
				//store temp row
				T temp_row;
				memcpy( &temp_row, &m_heapBlock[i], sizeof(T));
				//need to slide all j->i rows by 1 to the end of the memblock
				memcpy( &m_heapBlock[should_be_at+1], &m_heapBlock[should_be_at], sizeof(T)*(i-should_be_at-1));
				//now insert row to it's place
				memcpy( &m_heapBlock[should_be_at], &temp_row, sizeof(T));
			}*/
			prev_entry = *(uint32*)&m_heapBlock[i];
		}

		if(load_indexed)
		{
			m_entries = (T**)malloc(sizeof(T*) * (m_max+1));
			ASSERT(m_entries);

			memset(m_entries, 0, (sizeof(T*) * (m_max+1)));
			for(i = 0; i < rows; ++i)
			{
				if(m_firstEntry == NULL)
				{
					m_firstEntry = &m_heapBlock[i];
					m_firstEntryRow = i;
				}
				uint32 CurEntry = *(uint32*)&m_heapBlock[i];
				ASSERT( CurEntry <= m_max );
				m_entries[ CurEntry ] = &m_heapBlock[i];
			}
		}

		m_numrows = rows;

		fclose(f);
		return true;
	}

	void ReadEntry(FILE * f, T * dest, const char * format, uint32 cols, const char * filename)
	{
		const char * t = format;
		uint32 * dest_ptr = (uint32*)dest;
		uint32 c = 0;
		uint32 val;
//		size_t len = strlen(format);
//		if(len!= cols)
//			printf("!!! possible invalid format in file %s (us: %d, them: %u)\n", filename, len, cols);

		while(*t != 0)
		{
			if((++c) > cols)
			{
				++t;
//				printf("!!! Read buffer overflow in DBC reading of file %s\n", filename);
				continue;
			}

			fread(&val, 4, 1, f);
			if(*t == 'x')
			{
				++t;
				continue;		// skip!
			}
#ifdef USING_BIG_ENDIAN
			swap32(&val);
#endif
			else if(*t == 's')
			{
				char ** new_ptr = (char**)dest_ptr;
				static const char * null_str = "";
				char * ptr;
				if( val < m_stringlength )
					ptr = m_stringData + val;
				else
					ptr = (char*)null_str;

				*new_ptr = ptr;
				new_ptr++;
				dest_ptr = (uint32*)new_ptr;
			}
			else if(*t == 'L')	//uint64
			{
				*dest_ptr = val;
				dest_ptr++;
				fread(&val, 4, 1, f);
				*dest_ptr = val;
				dest_ptr++;
			}
			else
			{
				*dest_ptr = val;
				dest_ptr++;
			}

			++t;
		}
	}

	ARCEMU_INLINE uint32 GetNumRows()
	{
		return m_numrows;
	}

	T * LookupEntryForced(uint32 i)
	{
		if(m_entries)
		{
			if(i > m_max || m_entries[i] == NULL)
			{
#if 0
				printf("LookupEntryForced failed for entry %u\n", i);
#endif
				return NULL;
			}
			else
				return m_entries[i];
		}
		else
		{
			T *temp = LookupEntry( i );
			if( temp && *(uint32*)temp != i )
				return NULL;
			return temp;
		}
	}

	T * CreateCopy(T * obj)
	{
		T * oCopy = (T*)malloc(sizeof(T));
		ASSERT(oCopy);
		memcpy(oCopy,obj,sizeof(T));
		return oCopy;
	}
	void SetRow(uint32 i, T * t)
	{
		if(i < m_max && m_entries)
			m_entries[i] = t;
		if( m_LastReadRow < rows )
		{
			memcpy( &m_heapBlock[ m_LastReadRow ], t, sizeof( T ) );
			m_LastReadRow++;
		}
	}
	void ExtendRows( uint32 ExtraRowCount )
	{
		uint32 NewRows = rows + ExtraRowCount;
		uint32 Newm_max = m_max + 100 + ExtraRowCount;

		T * Newm_heapBlock = (T*)malloc( NewRows * sizeof(T) );
		ASSERT( Newm_heapBlock );
		memset( Newm_heapBlock, 0, NewRows * sizeof( T ) );
		memcpy( Newm_heapBlock, m_heapBlock, rows * sizeof( T ) );
		m_firstEntry = &Newm_heapBlock[ m_firstEntryRow ];
//		free( m_heapBlock ); //this crashes memory manager and i can't figure out why :(
		m_heapBlock = Newm_heapBlock;

		T ** Newm_entries = (T**)malloc( sizeof( T* ) * ( Newm_max ) );
		ASSERT( Newm_entries );
		memset( Newm_entries, 0, ( Newm_max ) * sizeof( T* ) );
		memcpy( Newm_entries, m_entries, m_max * sizeof( T* ) );
//		free( m_entries );//this crashes memory manager and i can't figure out why :(
		m_entries = Newm_entries;

		m_numrows = rows = NewRows;
		m_max = Newm_max;
	}

	T * LookupEntry(uint32 entry)
	{
		if(m_entries)
		{
			if(entry > m_max || m_entries[entry] == NULL)
				return m_firstEntry;
			else
				return m_entries[entry];
		}
		else
		{
			// removed. Old and absolutly bad. This is not entry but row lookup
			/*{
				if(i >= m_numrows)
					return NULL;
				else
					return &m_heapBlock[i];
			}*/
			//if DBC was entry based oredered : row[0].entry<row[1].entry
			if( ContentIsEntryOrdered )
			{
				//we can use logarithmic search. That is good
				uint32 start = 0;
				uint32 end = m_numrows;
				uint32 length;
				uint32 middle_index;
				do
				{
					length = end - start;
					length = length >> 1;
					middle_index = start + length;
					if( *(uint32*)&m_heapBlock[middle_index] < entry )
						start += length;
					else if( *(uint32*)&m_heapBlock[middle_index] > entry )
						end -= length;
					else 
						return &m_heapBlock[middle_index];
				}while( length != 0 );
				if( *(uint32*)&m_heapBlock[ middle_index ] == entry )
					return &m_heapBlock[middle_index];
				//return NULL;
				return m_firstEntry;	//plain wrong and insane !
			}
			else
			{
				//flat search :(
				for(uint32 i=0;i<m_numrows;i++)
					if( *(uint32*)&m_heapBlock[ i ] == entry )
						return &m_heapBlock[ i ];	
//				return NULL;
				return m_firstEntry;	//plain wrong and insane !
			}
		}
	}

	T * LookupRow(uint32 i)
	{
		if(i >= m_numrows)
			return NULL;
		else
			return &m_heapBlock[i];
	}
};

extern SERVER_DECL DBCStorage <CharSectionsEntry>			dbcCharSections;
extern SERVER_DECL DBCStorage <CharacterFacialHairStylesEntry>dbcCharacterFacialHairStyles;

extern SERVER_DECL DBCStorage <MountCapabilityEntry>		dbcMountCapabilityStore;
extern SERVER_DECL DBCStorage <MountTypeEntry>				dbcMountTypeStore;

extern SERVER_DECL DBCStorage <ArmorLocationEntry>			dbcArmorLocationStore;
extern SERVER_DECL DBCStorage <ItemArmorQualityEntry>		dbcItemArmorQualityStore;
extern SERVER_DECL DBCStorage <ItemArmorShieldEntry>		dbcItemArmorShieldStore;
extern SERVER_DECL DBCStorage <ItemArmorTotalEntry>			dbcItemArmorTotalStore;
extern SERVER_DECL DBCStorage <ItemDamageEntry>				dbcItemDamageAmmoStore;
extern SERVER_DECL DBCStorage <ItemDamageEntry>				dbcItemDamageOneHandStore;
extern SERVER_DECL DBCStorage <ItemDamageEntry>				dbcItemDamageOneHandCasterStore;
extern SERVER_DECL DBCStorage <ItemDamageEntry>				dbcItemDamageRangedStore;
extern SERVER_DECL DBCStorage <ItemDamageEntry>				dbcItemDamageThrownStore;
extern SERVER_DECL DBCStorage <ItemDamageEntry>				dbcItemDamageTwoHandStore;
extern SERVER_DECL DBCStorage <ItemDamageEntry>				dbcItemDamageTwoHandCasterStore;
extern SERVER_DECL DBCStorage <ItemDamageEntry>				dbcItemDamageWandStore;

extern SERVER_DECL DBCStorage <LFGDungeonEntry>				dbcLFGDungeonStore;
extern SERVER_DECL DBCStorage <WorldMapAreaEntry>			dbcWorldMapZoneStore;
extern SERVER_DECL DBCStorage <VehicleSeatEntry>			dbcVehicleSeatEntry;
extern SERVER_DECL DBCStorage <VehicleEntry>				dbcVehicleEntry;
//extern SERVER_DECL DBCStorage <CurrencyTypesEntry>		dbcCurrencyTypesStore;
extern SERVER_DECL DBCStorage <ScalingStatDistributionEntry> dbcScalingStatDistribution;
extern SERVER_DECL DBCStorage <ScalingStatValuesEntry>		dbcScalingStatValues;
extern SERVER_DECL DBCStorage <WorldMapOverlay>				dbcWorldMapOverlayStore;
extern SERVER_DECL DBCStorage <BattlemasterListEntry>		dbcBattlemasterListStore;
extern SERVER_DECL DBCStorage <ItemLimitCategory>			dbcItemLimitCategory; //used for prismatic gems
extern SERVER_DECL DBCStorage <AreaGroup>					dbcAreaGroup;
extern SERVER_DECL DBCStorage <GlyphPropertiesEntry>        dbcGlyphPropertiesStore;
extern SERVER_DECL DBCStorage <GlyphSlotEntry>              dbcGlyphSlotStore;
extern SERVER_DECL std::list<AchievementCriteriaEntry*>		dbcAchievementCriteriaTypeLookup[ACHIEVEMENT_CRITERIA_TYPE_TOTAL];
extern SERVER_DECL DBCStorage<AchievementEntry>				dbcAchievementStore;
extern SERVER_DECL DBCStorage<AchievementCriteriaEntry>		dbcAchievementCriteriaStore;
extern SERVER_DECL DBCStorage<BarberShopStyleEntry>			dbcBarberShopStyleStore;
extern SERVER_DECL DBCStorage<GtBarberShopCostBaseEntry>	dbcBarberShopCostStore;
extern SERVER_DECL DBCStorage<GemPropertyEntry>				dbcGemProperty;
extern SERVER_DECL DBCStorage<ItemSetEntry>					dbcItemSet;
extern SERVER_DECL DBCStorage<Lock>							dbcLock;

extern SERVER_DECL DBCStorage<SpellEntry>					dbcSpell;
extern SERVER_DECL DBCStorage<SpellDuration>				dbcSpellDuration;
extern SERVER_DECL DBCStorage<SpellRange>					dbcSpellRange;
extern SERVER_DECL DBCStorage<SpellRadius>					dbcSpellRadius;
extern SERVER_DECL DBCStorage<SpellCastTime>				dbcSpellCastTime;
extern SERVER_DECL DBCStorage<SpellRuneCostEntry>			dbcSpellRuneCostEntry;

extern SERVER_DECL DBCStorage<SpellEffectEntry>						dbcSpellEffect;
//extern SERVER_DECL DBCStorage<SpellDifficultyEntry>					dbcSpellDifficulty;
//extern SERVER_DECL DBCStorage<SpellFocusObjectEntry>				dbcSpellFocusObject;
//extern SERVER_DECL DBCStorage<SpellItemEnchantmentConditionEntry>	dbcSpellItemEnchantmentCondition;
//extern SERVER_DECL SpellCategoryStore								dbcSpellCategory;
extern SERVER_DECL DBCStorage<SpellShapeshiftFormEntry>				dbcSpellShapeshiftForm;
extern SERVER_DECL DBCStorage<SpellShapeshiftEntry>					dbcSpellShapeshift;
extern SERVER_DECL DBCStorage<SpellAuraOptionsEntry>				dbcSpellAuraOptions;
extern SERVER_DECL DBCStorage<SpellAuraRestrictionsEntry>			dbcSpellAuraRestrictions;
extern SERVER_DECL DBCStorage<SpellCastingRequirementsEntry>		dbcSpellCastingRequirements;
extern SERVER_DECL DBCStorage<SpellCategoriesEntry>					dbcSpellCategories;
extern SERVER_DECL DBCStorage<SpellClassOptionsEntry>				dbcSpellClassOptions;
extern SERVER_DECL DBCStorage<SpellCooldownsEntry>					dbcSpellCooldowns;
extern SERVER_DECL DBCStorage<SpellEquippedItemsEntry>				dbcSpellEquippedItems;
extern SERVER_DECL DBCStorage<SpellInterruptsEntry>					dbcSpellInterrupts;
extern SERVER_DECL DBCStorage<SpellLevelsEntry>						dbcSpellLevels;
extern SERVER_DECL DBCStorage<SpellPowerEntry>						dbcSpellPower;
extern SERVER_DECL DBCStorage<SpellReagentsEntry>					dbcSpellReagents;
extern SERVER_DECL DBCStorage<SpellScalingEntry>					dbcSpellScaling;
extern SERVER_DECL DBCStorage<SpellTargetRestrictionsEntry>			dbcSpellTargetRestrictions;
extern SERVER_DECL DBCStorage<SpellTotemsEntry>						dbcSpellTotems;
extern SERVER_DECL DBCStorage<gtClassLevelFloat>					dbcGTSpellScale;

extern SERVER_DECL DBCStorage<ResearchBranchEntry>			dbcResearchBranch;
extern SERVER_DECL DBCStorage<ResearchProjectEntry>			dbcResearchProject;
extern SERVER_DECL DBCStorage<ResearchSiteEntry>			dbcResearchSite;
extern SERVER_DECL DBCStorage<GuildPerkSpellEntry>			dbcGuildPerks;
extern SERVER_DECL DBCStorage<emoteentry>					dbcEmoteEntry;
extern SERVER_DECL DBCStorage<AreaTable>					dbcArea;
extern SERVER_DECL DBCStorage<FactionTemplateDBC>			dbcFactionTemplate;
extern SERVER_DECL DBCStorage<FactionDBC>					dbcFaction;
extern SERVER_DECL DBCStorage<EnchantEntry>					dbcEnchant;
extern SERVER_DECL DBCStorage<skilllinespell>				dbcSkillLineSpell;
extern SERVER_DECL DBCStorage<skilllineentry>				dbcSkillLine;
extern SERVER_DECL DBCStorage<DBCTaxiNode>					dbcTaxiNode;
extern SERVER_DECL DBCStorage<DBCTaxiPath>					dbcTaxiPath;
extern SERVER_DECL DBCStorage<DBCTaxiPathNode>				dbcTaxiPathNode;
extern SERVER_DECL DBCStorage<AuctionHouseDBC>				dbcAuctionHouse;
extern SERVER_DECL DBCStorage<TalentEntry>					dbcTalent;
extern SERVER_DECL DBCStorage<TalentTabEntry>				dbcTalentTab;
extern SERVER_DECL DBCStorage<TalentPrimarySpellsEntry>		dbcTalentPrimarySpells;
extern SERVER_DECL DBCStorage<CreatureSpellDataEntry>		dbcCreatureSpellData;
extern SERVER_DECL DBCStorage<CreatureFamilyEntry>			dbcCreatureFamily;
extern SERVER_DECL DBCStorage<CharClassEntry>				dbcCharClass;
extern SERVER_DECL DBCStorage<CharRaceEntry>				dbcCharRace;
extern SERVER_DECL DBCStorage<MapEntry>						dbcMap;
extern SERVER_DECL DBCStorage<ItemExtendedCostEntry>		dbcItemExtendedCost;
extern SERVER_DECL DBCStorage<RandomProps>					dbcRandomProps;
extern SERVER_DECL DBCStorage<ItemRandomSuffixEntry>		dbcItemRandomSuffix;
extern SERVER_DECL DBCStorage<RandomPropertiesPointsEntry>	dbcRandomPropertyPoints;
extern SERVER_DECL DBCStorage<ItemReforgeEntry>				dbcItemReforge;
extern SERVER_DECL DBCStorage<gtClassLevelFloat>			dbcCombatRating;
extern SERVER_DECL DBCStorage<ChatChannelDBC>				dbcChatChannels;
extern SERVER_DECL DBCStorage<DurabilityCostsEntry>			dbcDurabilityCosts;
extern SERVER_DECL DBCStorage<DurabilityQualityEntry>		dbcDurabilityQuality;
extern SERVER_DECL DBCStorage<BankSlotPrice>				dbcBankSlotPrices;
//extern SERVER_DECL DBCStorage<BankSlotPrice>				dbcStableSlotPrices; //uses same structure as Bank
extern SERVER_DECL DBCStorage<gtClassLevelFloat>			dbcMeleeCrit;
extern SERVER_DECL DBCStorage<gtClassLevelFloat>			dbcMeleeCritBase;
extern SERVER_DECL DBCStorage<gtClassLevelFloat>			dbcSpellCrit;
extern SERVER_DECL DBCStorage<gtClassLevelFloat>			dbcSpellCritBase;
extern SERVER_DECL DBCStorage<gtClassLevelFloat>			dbcMPRegen;
extern SERVER_DECL DBCStorage<gtClassLevelFloat>			dbcMPRegenBase;
//extern SERVER_DECL DBCStorage<gtFloat>						dbcHPRegen;
//extern SERVER_DECL DBCStorage<gtFloat>						dbcHPRegenBase;
extern SERVER_DECL DBCStorage<AreaTriggerEntry>				dbcAreaTrigger;
extern SERVER_DECL DBCStorage<QuestXPEntry>					dbcQuestXP;
extern SERVER_DECL DBCStorage<QuestRewRepEntry>				dbcQuestRewRep;
extern SERVER_DECL DBCStorage<CharTitlesEntry>				dbcCharTitle; // not used yet
extern SERVER_DECL DBCStorage<WorldSafeLocsEntry>			dbcWorldSafeLocs; 
extern SERVER_DECL DBCStorage<WMOAreaTableEntry>			dbcWMOAreaTable;

bool LoadDBCs();

#endif
