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

#ifndef _PLAYER_H
#define _PLAYER_H

//safe way to handle arrays in debug mode
#include <array>
#include "FeatureDefinitions.h"

class BGScore;
class Channel;
class Creature;
class Battleground;
class TaxiPath;
class GameObject;
class Transporter;
class Corpse;
class Guild;
class GuildRank;
class Pet;
class Charter;
class LFGMatch;
struct LevelInfo;
class SpeedDetector;
class Vehicle;
class SpellEffectOverrideScript;
class Instance;
class MapMgr;
struct LFG_store;

#define PLAYER_NORMAL_RUN_SPEED			7.00167f	//why not round ? to avoid memory hackers to find it :)
#define PLAYER_NORMAL_SWIM_SPEED		7.00167f
//#define PLAYER_NORMAL_FLIGHT_SPEED		9.332f
#define PLAYER_NORMAL_FLIGHT_SPEED		PLAYER_NORMAL_RUN_SPEED //will be auto scaled by spells
#define PLAYER_HONORLESS_TARGET_SPELL	2479
#define MONSTER_NORMAL_RUN_SPEED		(PLAYER_NORMAL_RUN_SPEED * 1.3f)	//player runs faster ? Not good
/* action button defines */
#define PLAYER_ACTION_BUTTON_COUNT		145
#define PLAYER_ACTION_BUTTON_SIZE PLAYER_ACTION_BUTTON_COUNT * sizeof(uint32)
#define MAX_SPEC_COUNT					2
#define GLYPHS_COUNT (PLAYER_GLYPHS_ENABLED - PLAYER_FIELD_GLYPHS_1)
#define PLAYER_HEIGHT					2.0f	//in yards, aproximation of the height of a player model

#define PLAYER_LEVEL_CAP_BLIZZLIKE					85
#define PLAYER_LEVEL_CAP							137
#define PLAYER_LEVEL_CAP_DBC						100
#define PLAYER_ARENA_MIN_LEVEL						70
#define PLAYER_SKILL_CAP							525
#define REQUIRED_LEVEL_TO_MAKE_DK					55
#define PLAYERS_RECEIVE_TALENTPOINTS_WITH_LEVEL		(90-9)	//1 point per level from level 9

#define PLAYER_DEFAULT_MELEE_RANGE				3

#define ACHIEVEMENT_SEND_DELAY 1000 //we have this delay of sending auras to other players so client will have time to create object first
#define LOGIN_CIENT_SEND_DELAY 1000 //we have this delay of sending auras to other players so client will have time to create object first

//this does not fix random client crash issue :(
//#define MAX_SEND_BUFFER_SIZE			32767
//#define MAX_SEND_BUFFER_SIZE_TO_SEND	32000	//maybe some network cards will freak out of large packets ?
#define MAX_SEND_BUFFER_SIZE			131078
#define MAX_SEND_BUFFER_SIZE_TO_SEND	120000	//maybe some network cards will freak out of large packets ?

#define PLAYER_GUILD_FACTION_ID			1168	
//====================================================================
//  Inventory
//  Holds the display id and item type id for objects in
//  a character's inventory
//====================================================================

#define TOTAL_NORMAL_RUNE_TYPES 3
#define TOTAL_USED_RUNES (TOTAL_NORMAL_RUNE_TYPES*2)
#define TOTAL_RUNE_TYPES 4
//#define TOTAL_RUNE_AVAIL 8

//#define TAXIMASK_SIZE		(78/4)	//client crashes on improper size ?
#define TAXIMASK_SIZE		86		//client crashes on improper size ?
#define MAX_QUEST_LOG_SIZE	25

enum Classes
{
	WARRIOR = 1,
	PALADIN = 2,
	HUNTER = 3,
	ROGUE = 4,
	PRIEST = 5,
	DEATHKNIGHT = 6,
	SHAMAN = 7,
	MAGE = 8,
	WARLOCK = 9,
	DRUID = 11,
	PLAYER_CLASS_TYPE_COUNT
};

enum Races
{
	RACE_HUMAN = 1,
	RACE_ORC = 2,
	RACE_DWARF = 3,
	RACE_NIGHTELF = 4,
	RACE_UNDEAD = 5,
	RACE_TAUREN = 6,
	RACE_GNOME = 7,
	RACE_TROLL = 8,
	RACE_GOBLIN = 9,
	RACE_BLOODELF = 10,
	RACE_DRAENEI = 11,
	RACE_WORGEN = 22, //or maybe it is 12 ? why do i have char gear for race 22 in dbc ? :)
	RACE_PANDA = 26, 
	RACE_COUNT = 27
};

enum ShapeshiftForm
{
   FORM_NORMAL              = 0,
   FORM_CAT                 = 1,
   FORM_TREE                = 2,
   FORM_TRAVEL              = 3,
   FORM_AQUA                = 4,
   FORM_BEAR                = 5,
   FORM_AMBIENT             = 6,
   FORM_GHOUL               = 7,
   FORM_DIREBEAR            = 8,
   FORM_CUSTOM_NORMAL		= 9,		// added manually to be able to detect non transformed mode
   FORM_THARONJA	        = 10,
   FORM_BUILDING	        = 11,
   FORM_PLAYER		        = 12,
   FORM_SHADOW_DANCE        = 13,
   FORM_CREATUREBEAR        = 14,
   FORM_CREATURECAT         = 15,
   FORM_GHOSTWOLF           = 16,
   FORM_BATTLESTANCE        = 17,
   FORM_DEFENSIVESTANCE     = 18,
   FORM_BERSERKERSTANCE     = 19,
   FORM_SHAPESHIFTED	    = 20,
   FORM_ZOMBIE              = 21,
   FORM_DEMON               = 22,
   FORM_LICHBORN            = 25,
   FORM_SWIFT               = 27,		//normal form 1 ? for druids
   FORM_SHADOW              = 28,		//normal form 2 ? 
   FORM_FLIGHT              = 29,
   FORM_STEALTH             = 30,
   FORM_MOONKIN             = 31,
   FORM_SPIRITOFREDEMPTION  = 32,
   FORM_RESURRECT_AS_GHOUL	= 127,
   FORM_NORMAL_FORM			= 134217728,	//having this means you are not transformed = form 28 ?
   FORM_NORMAL_FORM2		= 268435456,	//having this means you are not transformed = form 29 ?
   FORM_TOTAL_COUNT
};

enum PlayerStatus
{
	NONE			 = 0,
	TRANSFER_PENDING = 1,
};

enum DrunkenState
{
	DRUNKEN_SOBER	= 0,
	DRUNKEN_TIPSY	= 1,
	DRUNKEN_DRUNK	= 2,
	DRUNKEN_SMASHED	= 3
};

enum ECPLIPSE_BAR_STATE
{
	ECLIPSE_BAR_NOT_DECIDED_STATE	= 1,
	ECLIPSE_BAR_MOVING_TO_LUNAR		= 2,
	ECLIPSE_BAR_MOVING_TO_SOLAR		= 4,
};

enum RankTitles
{
	PVPTITLE_NONE							= 0,
	PVPTITLE_PRIVATE						= 1,
	PVPTITLE_CORPORAL						= 2,
	PVPTITLE_SERGEANT						= 3,
	PVPTITLE_MASTER_SERGEANT				= 4,
	PVPTITLE_SERGEANT_MAJOR					= 5,
	PVPTITLE_KNIGHT							= 6,
	PVPTITLE_KNIGHT_LIEUTENANT				= 7,
	PVPTITLE_KNIGHT_CAPTAIN					= 8,
	PVPTITLE_KNIGHT_CHAMPION				= 9,
	PVPTITLE_LIEUTENANT_COMMANDER			= 10,
	PVPTITLE_COMMANDER						= 11,
	PVPTITLE_MARSHAL						= 12,
	PVPTITLE_FIELD_MARSHAL					= 13,
	PVPTITLE_GRAND_MARSHAL					= 14,
	PVPTITLE_SCOUT							= 15,
	PVPTITLE_GRUNT							= 16,
	PVPTITLE_SERGEANT_2						= 17,
	PVPTITLE_SENIOR_SERGEANT				= 18,
	PVPTITLE_FIRST_SERGEANT					= 19,
	PVPTITLE_STONE_GUARD					= 20,
	PVPTITLE_BLOOD_GUARD					= 21,
	PVPTITLE_LEGIONNAIRE					= 22,
	PVPTITLE_CENTURION						= 23,
	PVPTITLE_CHAMPION						= 24,
	PVPTITLE_LIEUTENANT_GENERAL				= 25,
	PVPTITLE_GENERAL						= 26,
	PVPTITLE_WARLORD						= 27,
	PVPTITLE_HIGH_WARLORD					= 28,
	PVPTITLE_GLADIATOR						= 29,
	PVPTITLE_DUELIST						= 30,
	PVPTITLE_RIVAL							= 31,
	PVPTITLE_CHALLENGER						= 32,
	PVPTITLE_SCARAB_LORD					= 33,
	PVPTITLE_CONQUEROR						= 34,
	PVPTITLE_JUSTICAR						= 35,
	PVPTITLE_CHAMPION_OF_THE_NAARU			= 36,
	PVPTITLE_MERCILESS_GLADIATOR			= 37,
	PVPTITLE_OF_THE_SHATTERED_SUN			= 38,
	PVPTITLE_HAND_OF_A_DAL					= 39,
	PVPTITLE_VENGEFUL_GLADIATOR				= 40,
	PVPTITLE_BATTLEMASTER					= 41,
	PVPTITLE_THE_SEEKER						= 42,
	PVPTITLE_ELDER							= 43,
	PVPTITLE_FLAME_WARDEN					= 44,
	PVPTITLE_FLAME_KEEPER					= 45,
	PVPTITLE_THE_EXALTED					= 46,
	PVPTITLE_THE_EXPLORER					= 47,
	PVPTITLE_THE_DIPLOMAT					= 48,
	PVPTITLE_BRUTAL_GLADIATOR				= 49,
	PVPTITLE_ARENA_MASTER					= 50,
	PVPTITLE_SALTY							= 51,
	PVPTITLE_CHEF							= 52,
	PVPTITLE_THE_SUPREME					= 53,
	PVPTITLE_OF_THE_TEN_STORMS				= 54,
	PVPTITLE_OF_THE_EMERALD_DREAM			= 55,
	PVPTITLE_DEADLY_GLADIATOR				= 56,
	PVPTITLE_PROPHET						= 57,
	PVPTITLE_THE_MALEFIC					= 58,
	PVPTITLE_STALKER						= 59,
	PVPTITLE_OF_THE_EBON_BLADE				= 60,
	PVPTITLE_ARCHMAGE						= 61,
	PVPTITLE_WARBRINGER						= 62,
	PVPTITLE_ASSASSIN						= 63,
	PVPTITLE_GRAND_MASTER_ALCHEMIST			= 64,
	PVPTITLE_GRAND_MASTER_BLACKSMITH		= 65,
	PVPTITLE_IRON_CHEF						= 66,
	PVPTITLE_GRAND_MASTER_ENCHANTER			= 67,
	PVPTITLE_GRAND_MASTER_ENGINEER			= 68,
	PVPTITLE_DOCTOR							= 69,
	PVPTITLE_GRAND_MASTER_ANGLER			= 70,
	PVPTITLE_GRAND_MASTER_HERBALIST			= 71,
	PVPTITLE_GRAND_MASTER_SCRIBE			= 72,
	PVPTITLE_GRAND_MASTER_JEWELCRAFTER		= 73,
	PVPTITLE_GRAND_MASTER_LEATHERWORKER		= 74,
	PVPTITLE_GRAND_MASTER_MINER				= 75,
	PVPTITLE_GRAND_MASTER_SKINNER			= 76,
	PVPTITLE_GRAND_MASTER_TAILOR			= 77,
	PVPTITLE_OF_QUEL_THALAS					= 78,
	PVPTITLE_OF_ARGUS						= 79,
	PVPTITLE_OF_KHAZ_MODAN					= 80,
	PVPTITLE_OF_GNOMEREGAN					= 81,
	PVPTITLE_THE_LION_HEARTED				= 82,
	PVPTITLE_CHAMPION_OF_ELUNE				= 83,
	PVPTITLE_HERO_OF_ORGRIMMAR				= 84,
	PVPTITLE_PLAINSRUNNER					= 85,
	PVPTITLE_OF_THE_DARKSPEAR				= 86,
	PVPTITLE_THE_FORSAKEN					= 87,
	PVPTITLE_THE_MAGIC_SEEKER				= 88,
	PVPTITLE_TWILIGHT_VANQUISHER			= 89,
	PVPTITLE_CONQUEROR_OF_NAXXRAMAS			= 90,
	PVPTITLE_HERO_OF_NORTHREND				= 91,
	PVPTITLE_THE_HALLOWED					= 92,
	PVPTITLE_LOREMASTER						= 93,
	PVPTITLE_OF_THE_ALLIANCE				= 94,
	PVPTITLE_OF_THE_HORDE					= 95,
	PVPTITLE_THE_FLAWLESS_VICTOR			= 96,
	PVPTITLE_CHAMPION_OF_THE_FROZEN_WASTES	= 97,
	PVPTITLE_AMBASSADOR						= 98,
	PVPTITLE_THE_ARGENT_CHAMPION			= 99,
	PVPTITLE_GUARDIAN_OF_CENARIUS			= 100,
	PVPTITLE_BREWMASTER						= 101,
	PVPTITLE_MERRYMAKER						= 102,
	PVPTITLE_THE_LOVE_FOOL					= 103,
	PVPTITLE_MATRON							= 104,
	PVPTITLE_PATRON							= 105,
	PVPTITLE_OBSIDIAN_SLAYER				= 106,
	PVPTITLE_OF_THE_NIGHTFALL				= 107,
	PVPTITLE_THE_IMMORTAL					= 108,
	PVPTITLE_THE_UNDYING					= 109,
	PVPTITLE_JENKINS						= 110,
	PVPTITLE_BLOODSAIL_ADMIRAL				= 111,
	PVPTITLE_THE_INSANE						= 112,
	PVPTITLE_OF_THE_EXODAR					= 113,
	PVPTITLE_OF_DARNASSUS					= 114,
	PVPTITLE_OF_IRONFORGE					= 115,
	PVPTITLE_OF_STORMWIND					= 116,
	PVPTITLE_OF_ORGRIMMAR					= 117,
	PVPTITLE_OF_SEN_JIN						= 118,
	PVPTITLE_OF_SILVERMOON					= 119,
	PVPTITLE_OF_THUNDER_BLUFF				= 120,
	PVPTITLE_OF_THE_UNDERCITY				= 121,
	PVPTITLE_THE_NOBLE						= 122,
	PVPTITLE_CRUSADER						= 123,
	PVPTITLE_DEATH_S_DEMISE					= 124,
	PVPTITLE_THE_CELESTIAL_DEFENDER			= 125,
	PVPTITLE_CONQUEROR_OF_ULDUAR			= 126,
	PVPTITLE_CHAMPION_OF_ULDUAR				= 127,
	PVPTITLE_VANQUISHER						= 128,
	PVPTITLE_STARCALLER						= 129,
	PVPTITLE_THE_ASTRAL_WALKER				= 130,
	PVPTITLE_HERALD_OF_THE_TITANS			= 131,
	PVPTITLE_FURIOUS_GLADIATOR				= 132,
	PVPTITLE_THE_PILGRIM					= 133,
	PVPTITLE_RELENTLESS_GLADIATOR			= 134,
	PVPTITLE_GRAND_CRUSADER					= 135,
	PVPTITLE_THE_ARGENT_DEFENDER			= 136,
	PVPTITLE_THE_PATIENT					= 137,
	PVPTITLE_THE_LIGHT_OF_DAWN				= 138,
	PVPTITLE_BANE_OF_THE_FALLEN_KING		= 139,
	PVPTITLE_THE_KINGSLAYER					= 140,
	PVPTITLE_OF_THE_ASHEN_VERDICT			= 141,
	PVPTITLE_WRATHFUL_GLADIATOR				= 142,
	PVPTITLE_THE_CAMEL_HOARDER				= 143,
	PVPTITLE_WARBRINGER_2					= 144,
	PVPTITLE_WARBOUND						= 145,
	PVPTITLE_BLESSED_DEFENDER_OF_NORDRASSIL	= 146,
	PVPTITLE_ASSISTANT_PROFESSOR			= 148,
	PVPTITLE_ASSOCIATE_PROFESSOR			= 149,
	PVPTITLE_PROFESSOR						= 150,
	PVPTITLE_OF_THE_FOUR_WINDS				= 151,
	PVPTITLE_VETERAN_OF_THE_ALLIANCE		= 152,
	PVPTITLE_VETERAN_OF_THE_HORDE			= 153,
	PVPTITLE_PRIVATE_2						= 154,
	PVPTITLE_CORPORAL_2						= 155,
	PVPTITLE_SERGEANT_3						= 156,
	PVPTITLE_MASTER_SERGEANT_2				= 157,
	PVPTITLE_SERGEANT_MAJOR_2				= 158,
	PVPTITLE_KNIGHT_2						= 159,
	PVPTITLE_KNIGHT_LIEUTENANT_2			= 160,
	PVPTITLE_KNIGHT_CAPTAIN_2				= 161,
	PVPTITLE_KNIGHT_CHAMPION_2				= 162,
	PVPTITLE_LIEUTENANT_COMMANDER_2			= 163,
	PVPTITLE_COMMANDER_2					= 164,
	PVPTITLE_MARSHAL_2						= 165,
	PVPTITLE_FIELD_MARSHAL_2				= 166,
	PVPTITLE_GRAND_MARSHAL_2				= 167,
	PVPTITLE_SCOUT_2						= 168,
	PVPTITLE_GRUNT_2						= 169,
	PVPTITLE_SERGEANT_4						= 170,
	PVPTITLE_SENIOR_SERGEANT_2				= 171,
	PVPTITLE_FIRST_SERGEANT_2				= 172,
	PVPTITLE_STONE_GUARD_2					= 173,
	PVPTITLE_BLOOD_GUARD_2					= 174,
	PVPTITLE_LEGIONNAIRE_2					= 175,
	PVPTITLE_CENTURION_2					= 176,
	PVPTITLE_CHAMPION_2						= 177,
	PVPTITLE_LIEUTENANT_GENERAL_2			= 178,
	PVPTITLE_GENERAL_2						= 179,
	PVPTITLE_WARLORD_2						= 180,
	PVPTITLE_HIGH_WARLORD_2					= 181,
	PVPTITLE_HERO_OF_THE_ALLIANCE			= 182,
	PVPTITLE_HERO_OF_THE_HORDE				= 183,
	PVPTITLE_THE_BLOODTHIRSTY				= 184,
	PVPTITLE_DEFENDER_OF_A_SHATTERED_WORLD	= 185,
	PVPTITLE_DRAGONSLAYER					= 186,
	PVPTITLE_BLACKWING_S_BANE				= 187,
	PVPTITLE_AVENGER_OF_HYJAL				= 188,
	PVPTITLE_THE_FLAMEBREAKER				= 189,
	PVPTITLE_FIRELORD						= 190,
	PVPTITLE_VICIOUS_GLADIATOR				= 191,
	PVPTITLE_RUTHLESS_GLADIATOR				= 192,
	PVPTITLE_CATACLYSMIC_GLADIATOR			= 193,
	PVPTITLE_SAVIOR_OF_AZEROTH				= 194,
	PVPTITLE_FARMER							= 195,
	PVPTITLE_DESTROYER_S_END				= 196,
	PVPTITLE_THE_BELOVED					= 197,
	PVPTITLE_THE_FEARLESS					= 198,
	PVPTITLE_ZOOKEEPER						= 199,
	PVPTITLE_THE_RELIC_HUNTER				= 200,
	PVPTITLE_THE_UNDAUNTED					= 201,
	PVPTITLE_MASTER_OF_THE_WAYS				= 202,
	PVPTITLE_THE_TRANQUIL_MASTER			= 203,
	PVPTITLE_DELVER_OF_THE_VAULTS			= 204,
	PVPTITLE_SHADO_MASTER					= 205,
	PVPTITLE_THE_SCENATURDIST				= 206,
	PVPTITLE_TAMER							= 207,
	PVPTITLE_SEEKER_OF_KNOWLEDGE			= 208,
	PVPTITLE_BRAWLER						= 209,
	PVPTITLE_CONQUEROR_OF_ORGRIMMAR			= 211,
	PVPTITLE_LIBERATOR_OF_ORGRIMMAR			= 212,
	PVPTITLE_HELLSCREAM_S_DOWNFALL			= 214,
	PVPTITLE_THE_PROVEN_ASSAILANT			= 215,
	PVPTITLE_THE_PROVEN_DEFENDER			= 216,
	PVPTITLE_THE_WAKENER					= 218,
	PVPTITLE_THE_PROVEN_HEALER				= 219,
	PVPTITLE_KHAN							= 220,
	PVPTITLE_THE_STORMBREAKER				= 221,
	PVPTITLE_STORM_S_END					= 222,
	PVPTITLE_MALEVOLENT_GLADIATOR			= 223,
	PVPTITLE_GOB_SQUAD_RECRUIT				= 224,
	PVPTITLE_TYRANNICAL_GLADIATOR			= 225,
	PVPTITLE_GOB_SQUAD_COMMANDO				= 226,
	PVPTITLE_TRAINER						= 227,
	PVPTITLE_DARKSPEAR_REVOLUTIONARY		= 228,
	PVPTITLE_GORGEOUS						= 229,
	PVPTITLE_THE_HORDEBREAKER				= 230,
	PVPTITLE_THE_POISONED_MIND				= 231,
	PVPTITLE_THE_BLOODSEEKER				= 232,
	PVPTITLE_THE_LOCUST						= 233,
	PVPTITLE_THE_SWARMKEEPER				= 234,
	PVPTITLE_THE_PRIME						= 235,
	PVPTITLE_THE_MANIPULATOR				= 236,
	PVPTITLE_THE_DISSECTOR					= 237,
	PVPTITLE_THE_LUCID						= 238,
	PVPTITLE_THE_WIND_REAVER				= 239,
	PVPTITLE_THE_CRAZY_CAT_LADY				= 240,
	PVPTITLE_DEFENDER_OF_THE_WALL			= 241,
	PVPTITLE_MOGU_SLAYER					= 242,
	PVPTITLE_FLAMEWEAVER					= 243,
	PVPTITLE_SCARLET_COMMANDER				= 244,
	PVPTITLE_DARKMASTER						= 245,
	PVPTITLE_PURIFIED_DEFENDER				= 246,
	PVPTITLE_SIEGEBREAKER					= 247,
	PVPTITLE_STORMBREWER					= 248,
	PVPTITLE_JADE_PROTECTOR					= 249,
	PVPTITLE_FIRE_WATCHER					= 251,
	PVPTITLE_THE_CRAZY_CAT_MAN				= 252,
	PVPTITLE_GRIEVOUS_GLADIATOR				= 253,
	PVPTITLE_PRIDEFUL_GLADIATOR				= 254,
	PVPTITLE_END							= 255,
};

enum Player_Custom_Flags
{
//	PLAYER_CUSTOM_FLAG_IGNORES_SPELL_STATE_REQUIREMENTS			= 1,
};

enum PvPAreaStatus
{
    AREA_ALLIANCE = 1,
    AREA_HORDE = 2,
    AREA_CONTESTED = 3,
    AREA_PVPARENA = 4,
};

enum PlayerMovementType
{
    MOVE_ROOT			= 1,
    MOVE_UNROOT			= 2,
    MOVE_WATER_WALK		= 3,
    MOVE_LAND_WALK		= 4,
    MOVE_FEATHER_FALL	= 5,
    MOVE_NORMAL_FALL	= 6,
    MOVE_HOVER			= 7,
    MOVE_NO_HOVER		= 8,
};

enum PlayerSpeedType
{
    RUN	            = 1,
    RUNBACK         = 2,
    SWIM	        = 3,
    SWIMBACK        = 4,
    WALK	        = 5,
    FLY	            = 6,
};

enum Standing
{
    STANDING_HATED,
    STANDING_HOSTILE,
    STANDING_UNFRIENDLY,
    STANDING_NEUTRAL,
    STANDING_FRIENDLY,
    STANDING_HONORED,
    STANDING_REVERED,
    STANDING_EXALTED
};

enum FactionFlags
{
	FACTION_FLAG_VISIBLE            = 0x01,
	FACTION_FLAG_AT_WAR             = 0x02,
	FACTION_FLAG_HIDDEN             = 0x04,
	FACTION_FLAG_FORCED_INVISIBLE   = 0x08,// if both ACTION_FLAG_VISIBLE and FACTION_FLAG_FORCED_INVISIBLE are set, client crashes!
	FACTION_FLAG_DISABLE_ATWAR      = 0x10,// disables AtWar button for client, but you can be in war with the faction
	FACTION_FLAG_INACTIVE           = 0x20,
	FACTION_FLAG_RIVAL              = 0x40 // only Scryers and Aldor have this flag
};

enum PlayerFlags
{
    PLAYER_FLAG_PARTY_LEADER			= 0x00000001,
    PLAYER_FLAG_AFK						= 0x00000002,
    PLAYER_FLAG_DND						= 0x00000004,
    PLAYER_FLAG_GM						= 0x00000008,
    PLAYER_FLAG_DEATH_WORLD_ENABLE		= 0x00000010,
    PLAYER_FLAG_RESTING					= 0x00000020,
    PLAYER_FLAG_UNKNOWN1				= 0x00000040,
    PLAYER_FLAG_FREE_FOR_ALL_PVP		= 0x00000080,
    PLAYER_FLAGS_CONTESTED_PVP			= 0x00000100,
    PLAYER_FLAG_PVP_TOGGLE				= 0x00000200,
    PLAYER_FLAG_NOHELM					= 0x00000400,
    PLAYER_FLAG_NOCLOAK					= 0x00000800,
    PLAYER_FLAG_NEED_REST_3_HOURS		= 0x00001000,
    PLAYER_FLAG_NEED_REST_5_HOURS		= 0x00002000,
	PLAYER_FLAGS_IS_OUT_OF_BOUNDS		= 0x00004000,       // Lua_IsOutOfBounds
	PLAYER_FLAGS_DEVELOPER				= 0x00008000,
	PLAYER_FLAGS_ENABLE_LOW_LEVEL_RAID  = 0x00010000,       // triggers lua event EVENT_ENABLE_LOW_LEVEL_RAID
	PLAYER_FLAGS_TAXI_BENCHMARK         = 0x00020000,       // taxi benchmark mode (on/off) (2.0.1)
	PLAYER_FLAGS_PVP_STATUS_COOLDOWN	= 0x00040000,
	PLAYER_FLAGS_COMMENTATOR            = 0x00080000,
	PLAYER_FLAGS_COMMENTATOR_UBER       = 0x00400000,       // something like COMMENTATOR_CAN_USE_INSTANCE_COMMAND
	PLAYER_ALLOW_ONLY_SPELLS_MELEE		= 0x00800000,       // used by bladestorm and killing spree - EVENT_SPELL_UPDATE_USABLE and EVENT_UPDATE_SHAPESHIFT_USABLE, disabled all abilitys on tab except autoattack
	PLAYER_ALLOW_ONLY_SPELL				= 0x01000000,       // EVENT_SPELL_UPDATE_USABLE and EVENT_UPDATE_SHAPESHIFT_USABLE, disabled all melee ability on tab include autoattack
	PLAYER_FLAGS_NO_XP_GAIN				= 0x02000000,
	PLAYER_FLAGS_AUTO_DECLINE_GUILD     = 0x08000000,       // Automatically declines guild invites
	PLAYER_FLAGS_IS_IN_GUILD			= 0x10000000,
	PLAYER_FLAGS_VOID_UNLOCKED          = 0x20000000,       // void storage
	PLAYER_FLAGS_CUSTOM_FORCE_TALENTRESET = 0x20000000,	//update characters set player_flags = player_flags | 0x20000000;
	PLAYER_FLAGS_CUSTOM_FORCE_SPELLRESET  = 0x40000000,
};

enum CharacterFlags
{
    CHARACTER_FLAG_NONE                 = 0x00000000,
    CHARACTER_FLAG_UNK1                 = 0x00000001,
    CHARACTER_FLAG_UNK2                 = 0x00000002,
    CHARACTER_LOCKED_FOR_TRANSFER       = 0x00000004,
    CHARACTER_FLAG_UNK4                 = 0x00000008,
    CHARACTER_FLAG_UNK5                 = 0x00000010,
    CHARACTER_FLAG_UNK6                 = 0x00000020,
    CHARACTER_FLAG_UNK7                 = 0x00000040,
    CHARACTER_FLAG_UNK8                 = 0x00000080,
    CHARACTER_FLAG_UNK9                 = 0x00000100,
    CHARACTER_FLAG_UNK10                = 0x00000200,
    CHARACTER_FLAG_HIDE_HELM            = 0x00000400,
    CHARACTER_FLAG_HIDE_CLOAK           = 0x00000800,
    CHARACTER_FLAG_UNK13                = 0x00001000,
    CHARACTER_FLAG_GHOST                = 0x00002000,
    CHARACTER_FLAG_RENAME               = 0x00004000,
    CHARACTER_FLAG_UNK16                = 0x00008000,
    CHARACTER_FLAG_UNK17                = 0x00010000,
    CHARACTER_FLAG_UNK18                = 0x00020000,
    CHARACTER_FLAG_UNK19                = 0x00040000,
    CHARACTER_FLAG_UNK20                = 0x00080000,
    CHARACTER_FLAG_UNK21                = 0x00100000,
    CHARACTER_FLAG_UNK22                = 0x00200000,
    CHARACTER_FLAG_UNK23                = 0x00400000,
    CHARACTER_FLAG_UNK24                = 0x00800000,
    CHARACTER_FLAG_LOCKED_BY_BILLING    = 0x01000000,
    CHARACTER_FLAG_DECLINED             = 0x02000000,
    CHARACTER_FLAG_UNK27                = 0x04000000,
    CHARACTER_FLAG_UNK28                = 0x08000000,
    CHARACTER_FLAG_UNK29                = 0x10000000,
    CHARACTER_FLAG_UNK30                = 0x20000000,
    CHARACTER_FLAG_UNK31                = 0x40000000,
    CHARACTER_FLAG_UNK32                = 0x80000000
};

enum CharacterCustomizeFlags
{
    CHAR_CUSTOMIZE_FLAG_NONE            = 0x00000000,
    CHAR_CUSTOMIZE_FLAG_CUSTOMIZE       = 0x00000001,       // name, gender, etc...
    CHAR_CUSTOMIZE_FLAG_FACTION         = 0x00010000,       // name, gender, faction, etc...
    CHAR_CUSTOMIZE_FLAG_RACE            = 0x00100000,       // name, gender, race, etc...
    CHAR_CUSTOMIZE_FLAG_DISPLAY_ORDER   = 0x04000000,       // the way they are show in char enum,! not sure !
};

enum PlayerFieldByteFlags
{
	//seems like having pet can also make us have flag 4 ? need more research
	PLAYER_FIELD_BYTES_FLAG_HAS_PET				= 0x00000001, //client does not let you tame another pet
    PLAYER_FIELD_BYTES_TRACK_STEALTHED			= 0x00000002,
//	PLAYER_FIELD_BYTES_FLAG_NO_HUMAN_CONTROL	= 0x00000004, //not sure, just guessing since it was there on logout
//	PLAYER_FIELD_BYTES_FLAG_HUMAN_CONTROL		= 0x00000008,
    PLAYER_FIELD_BYTES_RELEASE_TIMER			= 0x00000008,       // Display time till auto release spirit
    PLAYER_FIELD_BYTES_NO_RELEASE_WINDOW		= 0x00000010        // Display no "release spirit" window at all
};

//byte 0 = cast bar ID
//byte 1 = cast bar id / state animations
//byte 2 = states used for vehicles ?
//byte 3 = stealth / invis .. ?
enum PlayerFieldByte2Flags
{
    PLAYER_FIELD_BYTES2_NONE				= 0x00000000,
    PLAYER_FIELD_BYTES2_DETECT_AMORE_0		= 0x00020000,   // SPELL_AURA_DETECT_AMORE, not used as value and maybe not relcted to, but used in code as base for mask apply
    PLAYER_FIELD_BYTES2_DETECT_AMORE_1		= 0x00040000,   // SPELL_AURA_DETECT_AMORE value 1
    PLAYER_FIELD_BYTES2_DETECT_AMORE_2		= 0x00080000,   // SPELL_AURA_DETECT_AMORE value 2
    PLAYER_FIELD_BYTES2_DETECT_AMORE_3		= 0x00100000,   // SPELL_AURA_DETECT_AMORE value 3
    PLAYER_FIELD_BYTES2_CAMOUFLAGE			= 0x20000000,	// if you login close(3 yards) behing a player with this flag, client will start spamming packet 0x08EA and i have no idea what i need to reply to that. Probably remove stealth ?
    PLAYER_FIELD_BYTES2_INVISIBILITY_GLOW	= 0x40000000,	
};

//#define PLAYER_IS_PVP_CLIENT_SIDE_FLAGS ( PLAYER_FLAG_PVP_TOGGLE | PLAYER_FLAGS_PVP_STATUS )
#define PLAYER_IS_PVP_CLIENT_SIDE_FLAGS ( PLAYER_FLAG_PVP_TOGGLE )

enum CharterTypes
{
	CHARTER_TYPE_GUILD			= 0,
//	CHARTER_TYPE_ARENA_2V2		= 1,
//	CHARTER_TYPE_ARENA_3V3		= 2,
//	CHARTER_TYPE_ARENA_5V5		= 3,
	NUM_CHARTER_TYPES			= 4,
};

enum ArenaTeamTypes
{
	ARENA_TEAM_TYPE_2V2			= 0,
	ARENA_TEAM_TYPE_3V3			= 1,
	ARENA_TEAM_TYPE_5V5			= 2,
	NUM_ARENA_TEAM_TYPES		= 3,
};

enum CooldownTypes
{
	COOLDOWN_TYPE_SPELL			= 0,
	COOLDOWN_TYPE_CATEGORY		= 1,
	NUM_COOLDOWN_TYPES,
};

enum LootType
{
    LOOT_CORPSE                 = 1,
    LOOT_SKINNING               = 2,
    LOOT_FISHING                = 3,
    LOOT_PICKPOCKETING          = 2,                        // 4 unsupported by client, sending LOOT_SKINNING instead
    LOOT_DISENCHANTING          = 2,                        // 5 unsupported by client, sending LOOT_SKINNING instead
    LOOT_PROSPECTING            = 2,                        // 6 unsupported by client, sending LOOT_SKINNING instead
	LOOT_MILLING				= 2,
    LOOT_INSIGNIA               = 2                         // 7 unsupported by client, sending LOOT_SKINNING instead
};

enum HasteModType
{	
	MOD_MELEE		= 0,
	MOD_RANGED		= 1,
	MOD_SPELL		= 2,
	MOD_TYPE_COUNT	= 3,
};

#pragma pack(push,1)
#define GetActionButtonSpell( x ) (x & 0x00FFFFFF)	//spell is on 24 bits
#define GetActionButtonType( x ) (x >>24)	//type is the upper 8 bits
#define CreateActionButton( spell, type ) ( ( type << 24 ) | ( spell & 0x00FFFFFF ) )

#pragma pack(pop)

enum ActionButtonType
{
    ACTION_BUTTON_SPELL				= 0x00,
    ACTION_BUTTON_C					= 0x01,                         // click?
    ACTION_BUTTON_COMPOSITE_SPELL   = 0x10 | 0x20,                         // client know that when you click on this he will expand it so you can select from sub spells
    ACTION_BUTTON_EQSET				= 0x20,
    ACTION_BUTTON_MACRO				= 0x40,
    ACTION_BUTTON_CMACRO			= ACTION_BUTTON_C | ACTION_BUTTON_MACRO,
    ACTION_BUTTON_ITEM				= 0x80
};

struct WargameStore
{
	uint64	OtherPartyLeader;		// either the guid that invited us or the guid we invited to join the BG
	uint8	BGType;					// BattlegroundDbcIndex
	uint8	ArenaType;				// 2vs2, 3vs3 4vs4
	uint8	SkipPartySizeCheck;		// if we wish to create for example 6 vs 6 arena
	uint8	SuddenDeath;			// health drops as time passes
	int32	PlayerLives;			// each player can die N times max
	int32	TeamLives;				// Team members will be resurrected N times max
};

struct AchievementVal
{
	AchievementVal(){ cur_value = completed_at_stamp = 0; }
	uint32 cur_value;
	uint32 completed_at_stamp;
};

#define ACHIEVEMENT_EVENT_ACTION_SET		0
#define ACHIEVEMENT_EVENT_ACTION_ADD		1
#define ACHIEVEMENT_EVENT_ACTION_SET_MAX	2

struct CreateInfo_ItemStruct
{
	uint32  protoid;
	uint8   slot;
	uint32  amount;
};

#if GM_STATISTICS_UPDATE_INTERVAL > 0 
struct GM_statistics
{
	GM_statistics() 
	{
		wisper_chat_messages = global_chat_messages = tickets_deleted = time_active = commands_executed = mails_sent = summons_made = appear_made = 0;
		walk_tel_distance_sum = 0.0f;
	}
	uint32 time_active;				//sum of seconds when the GM was not afk. Note that there are anti AFK modules
	uint32 commands_executed;		//all possible commands
	uint32 mails_sent;				//number of tickets he replied (probably)
	uint32 tickets_deleted;			//number of tickets deleted (not all replied?)
	float  walk_tel_distance_sum;	//this is a rough estimation how much he covered from world. If this is small then it is kinda bad, If it is large then he simply moves a lot
	uint32 global_chat_messages;	//got to love GMs that talk a lot
	uint32 wisper_chat_messages;	//maybe he is shy and talks a lot in private only
	uint32 summons_made;			//we all love GMs that talk to people face to face
	uint32 appear_made;				//we love even more GMs that simply appear at others and not harass them while they play
	uint32 next_afk_update;			//just so we do not spam DB. This should not make a bi difference anyway
	float  last_pos_x,last_pos_y;	//so we can create the distance diffs
};
#endif

struct CreateInfo_SkillStruct
{
	uint32  skillid;
	uint32  currentval;
	uint32  maxval;
};

struct CreateInfo_ActionBarStruct
{
	uint32  button;
	uint32  action;
	uint32  type;
	uint32  misc;
};

struct CreateInfo_levelBaseStats
{
	uint32  HP;
	uint32  Mana;
	uint32	Stat[5];
	uint32	XPToNextLevel;
};

struct PlayerCreateInfo{
	uint8   index;
	uint8   race;
	uint32  factiontemplate;
	uint8   class_;
	uint32  mapId;
//	uint32  zoneId;
	float   positionX;
	float   positionY;
	float   positionZ;
	uint32  displayId;
	CreateInfo_levelBaseStats				stats[PLAYER_LEVEL_CAP];
	std::list<CreateInfo_ItemStruct>		items;
	std::list<CreateInfo_SkillStruct>		skills;
	std::list<CreateInfo_ActionBarStruct>	actionbars;
	std::set<uint32>						spell_list;
};

struct DamageSplit
{
	Player* caster;
	Aura*   aura;
	uint32  miscVal;
	union
	{
		uint32 damage;
		float damagePCT;
	};
};

struct LoginAura
{
	uint32	id;
	uint32	dur;
	bool	positive;
	uint32	charges;
	int32	mod_ammount;	//in case caster was someone else then keep mods
};

/*
Exalted	        1,000	 Access to racial mounts. Capped at 999.7
Revered	        21,000	 Heroic mode keys for Outland dungeons
Honored	        12,000	 10% discount from faction vendors
Friendly	    6,000
Neutral	        3,000
Unfriendly	    3,000	 Cannot buy, sell or interact.
Hostile	        3,000	 You will always be attacked on sight
Hated	        36,000 
*/
enum FactionRating
{
	HATED,
	HOSTILE,
	UNFRIENDLY,
	NEUTRAL,
	FRIENDLY,
	HONORED,
	REVERED,
	EXALTED
};
struct FactionReputation
{
	int32 standing;
	uint8 flag;
	int32 baseStanding;
	ARCEMU_INLINE int32 CalcStanding() { return standing - baseStanding; }
	ARCEMU_INLINE bool Positive() { return standing >= 0; }
};

typedef HM_NAMESPACE::hash_map<uint32, uint32> PlayerInstanceMap;
struct PlayerInfo
{
	~PlayerInfo();
	uint32 guid;
	uint32 acct;
	char * name;
	uint32 race;
	uint32 gender;
	uint32 _class;
	uint32 team;
	
	time_t lastOnline;
	uint32 lastZone;
	uint32 lastLevel;
	Group * m_Group;
	int8 subGroup;
	Mutex savedInstanceIdsLock;
	//PlayerInstanceMap savedInstanceIds[NUM_INSTANCE_MODES];
	tr1::array< PlayerInstanceMap, NUM_INSTANCE_MODES > savedInstanceIds;
#ifdef VOICE_CHAT
	int8 groupVoiceId;
#endif

	Player * m_loggedInPlayer;
	Guild * guild;
	GuildRank * guildRank;
	GuildMember * guildMember;
};
struct PlayerPet
{
	string	name;
	uint32	entry;
//	string	fields;
	uint32	xp;
//	bool	active;		//auto resummon event can use this
	int8	StableSlot;	//from 0 to 25 ? First 5 are active, rest are stabled
	uint32	number;
	uint32	level;
//	uint32	loyaltyxp;
//	uint32	happinessupdate;
	string	actionbar;
	bool	summon;
//	uint32	loyaltypts;
//	uint32	loyaltyupdate;
//	char	loyaltylvl;
	time_t	reset_time;
	uint32	reset_cost;
	uint32	spellid;
	uint8	State; //passive, agressive, defensive
	uint8	HPPctOnDismiss;
	std::map<uint32,uint32>	Cooldowns;
};
enum MeetingStoneQueueStatus
{
	MEETINGSTONE_STATUS_NONE								= 0,
	MEETINGSTONE_STATUS_JOINED_MEETINGSTONE_QUEUE_FOR	   = 1,
	MEETINGSTONE_STATUS_PARTY_MEMBER_LEFT_LFG			   = 2,
	MEETINGSTONE_STATUS_PARTY_MEMBER_REMOVED_PARTY_REMOVED  = 3,
	MEETINGSTONE_STATUS_LOOKING_FOR_NEW_PARTY_IN_QUEUE	  = 4,
	MEETINGSTONE_STATUS_NONE_UNK							= 5,
};
enum ItemPushResultTypes
{
	ITEM_PUSH_TYPE_LOOT			 = 0x00000000,
	ITEM_PUSH_TYPE_RECEIVE		  = 0x00000001,
	ITEM_PUSH_TYPE_CREATE		   = 0x00000002,
};
struct WeaponModifier
{
	int32		wclass;
	int32		subclass;
	float		value;
	SpellEntry	*spellentry;
};
struct PetActionBar
{
	uint32 spell[10];
};

class Spell;
class Item;
class Container;
class WorldSession;
class ItemInterface;
class GossipMenu;
class SpeedCheatDetector;
struct TaxiPathNode;

#define RESTSTATE_RESTED			 1
#define RESTSTATE_NORMAL			 2
#define RESTSTATE_TIRED100		     3
#define RESTSTATE_TIRED50			 4
#define RESTSTATE_EXHAUSTED		     5
#define UNDERWATERSTATE_NONE		 0
#define UNDERWATERSTATE_SWIMMING	 1
#define UNDERWATERSTATE_UNDERWATER   2
#define UNDERWATERSTATE_RECOVERING   4
#define UNDERWATERSTATE_TAKINGDAMAGE 8
#define UNDERWATERSTATE_FATIGUE	     16
#define UNDERWATERSTATE_LAVA		 32
#define UNDERWATERSTATE_SLIME		 64

enum TRADE_STATUS
{
	TRADE_STATUS_PLAYER_BUSY	    = 0x00,
	TRADE_STATUS_PROPOSED		    = 0x01,
	TRADE_STATUS_INITIATED		    = 0x02,
	TRADE_STATUS_CANCELLED		    = 0x03,
	TRADE_STATUS_ACCEPTED		    = 0x04,
	TRADE_STATUS_ALREADY_TRADING    = 0x05,
	TRADE_STATUS_PLAYER_NOT_FOUND   = 0x06,
	TRADE_STATUS_STATE_CHANGED	    = 0x07,
	TRADE_STATUS_COMPLETE		    = 0x08,
	TRADE_STATUS_UNACCEPTED		    = 0x09,
	TRADE_STATUS_TOO_FAR_AWAY	    = 0x0A,
	TRADE_STATUS_WRONG_FACTION	    = 0x0B,
	TRADE_STATUS_FAILED			    = 0x0C,
	TRADE_STATUS_DEAD			    = 0x0D,
	TRADE_STATUS_PETITION		    = 0x0E,
	TRADE_STATUS_PLAYER_IGNORED	    = 0x0F,
    TRADE_STATUS_TARGET_STUNNED		= 0x10,
    TRADE_STATUS_YOU_DEAD			= 0x11,
    TRADE_STATUS_TARGET_DEAD		= 0x12,
    TRADE_STATUS_YOU_LOGOUT			= 0x13,
    TRADE_STATUS_TARGET_LOGOUT		= 0x14,
    TRADE_STATUS_TRIAL_ACCOUNT		= 0x15,                       // Trial accounts can not perform that action
    TRADE_STATUS_ONLY_CONJURED		= 0x16                        // You can only trade conjured items... (cross realm BG related).
};

#define TRADE_ID_CONST	0x00002AAB

enum TRADE_DATA
{
	TRADE_GIVE		= 0x00,
	TRADE_RECEIVE	 = 0x01,
};

#define TRADE_MAX_TRADABLE_ITEMS	6	//this does not include the enchant slot
#define TRADE_MAX_ENCHANTABLE_ITEMS	1
#define TRADE_TOTAL_TRADE_SLOTS		( TRADE_MAX_TRADABLE_ITEMS + TRADE_MAX_ENCHANTABLE_ITEMS )

#define DUEL_COUNTDOWN_MS		3000
enum DUEL_STATUS
{
	DUEL_STATUS_OUTOFBOUNDS,
	DUEL_STATUS_INBOUNDS
};
enum DUEL_STATE
{
	DUEL_STATE_REQUESTED,
	DUEL_STATE_COUNTDOWN,
	DUEL_STATE_STARTED,
	DUEL_STATE_FINISHED
};
enum DUEL_WINNER
{
	DUEL_WINNER_KNOCKOUT,
	DUEL_WINNER_RETREAT,
};
#define PLAYER_ATTACK_TIMEOUT_INTERVAL	5000
#define PLAYER_FORCED_RESURECT_INTERVAL	360000 // 1000*60*6= 6 minutes 

struct PlayerSkill
{
	skilllineentry * Skill;
	uint16 CurrentValue;
	uint16 MaximumValue;
	uint16 BonusValue;
	float GetSkillUpChance();
	void Reset(uint32 Id);
};

enum SPELL_INDEX
{
	SPELL_TYPE_INDEX_MARK			= 1,
	SPELL_TYPE_INDEX_POLYMORPH		= 2,
	SPELL_TYPE_INDEX_FEAR			= 3,
	SPELL_TYPE_INDEX_SAP			= 4,
	SPELL_TYPE_INDEX_SCARE_BEAST	= 5,
	SPELL_TYPE_INDEX_HIBERNATE		= 6,
	SPELL_TYPE_INDEX_EARTH_SHIELD	= 7,
	SPELL_TYPE_INDEX_CYCLONE		= 8,
	SPELL_TYPE_INDEX_BANISH			= 9,
	SPELL_TYPE_INDEX_SHACKLE_UNDEAD	= 10,	
	SPELL_TYPE_INDEX_FOCUS_MAGIC	= 11,	
	SPELL_TYPE_INDEX_BEACON_OF_LIGHT	= 12,	
	SPELL_TYPE_INDEX_SACRED_SHIELD		= 13,	
	SPELL_TYPE_INDEX_DEVOURING_PLAGUE	= 14,	
	SPELL_TYPE_INDEX_DARK_INTENT		= 15,	
//	SPELL_TYPE_INDEX_GRACE				= 16,	
	SPELL_TYPE_INDEX_LIFEBLOOM			= 17,	
	SPELL_TYPE_INDEX_BANE				= 18,	
	SPELL_TYPE_INDEX_SOULSTONE			= 19,	
	NUM_SPELL_TYPE_INDEX,
};

enum SPELL_INDEX2
{
	SPELL_TYPE2_PALADIN_AURA			= 1,
};

#define PLAYER_RATING_MODIFIER_RANGED_SKILL						(PLAYER_FIELD_COMBAT_RATING_1 + 0)
#define PLAYER_RATING_MODIFIER_DEFENCE							(PLAYER_FIELD_COMBAT_RATING_1 + 1)
#define PLAYER_RATING_MODIFIER_DODGE							(PLAYER_FIELD_COMBAT_RATING_1 + 2)
#define PLAYER_RATING_MODIFIER_PARRY							(PLAYER_FIELD_COMBAT_RATING_1 + 3)
#define PLAYER_RATING_MODIFIER_BLOCK							(PLAYER_FIELD_COMBAT_RATING_1 + 4)
#define PLAYER_RATING_MODIFIER_MELEE_HIT						(PLAYER_FIELD_COMBAT_RATING_1 + 5)
#define PLAYER_RATING_MODIFIER_RANGED_HIT						(PLAYER_FIELD_COMBAT_RATING_1 + 6)
#define PLAYER_RATING_MODIFIER_SPELL_HIT						(PLAYER_FIELD_COMBAT_RATING_1 + 7)
#define PLAYER_RATING_MODIFIER_MELEE_CRIT						(PLAYER_FIELD_COMBAT_RATING_1 + 8)
#define PLAYER_RATING_MODIFIER_RANGED_CRIT						(PLAYER_FIELD_COMBAT_RATING_1 + 9)
#define PLAYER_RATING_MODIFIER_SPELL_CRIT						(PLAYER_FIELD_COMBAT_RATING_1 + 10)
#define PLAYER_RATING_MODIFIER_MELEE_HIT_AVOIDANCE				(PLAYER_FIELD_COMBAT_RATING_1 + 11) // Not 100% sure but the numbers line up
#define PLAYER_RATING_MODIFIER_RANGED_HIT_AVOIDANCE				(PLAYER_FIELD_COMBAT_RATING_1 + 12) // GUESSED
#define PLAYER_RATING_MODIFIER_SPELL_HIT_AVOIDANCE				(PLAYER_FIELD_COMBAT_RATING_1 + 13) // GUESSED
//#define PLAYER_RATING_MODIFIER_MELEE_CRIT_RESILIENCE			(PLAYER_FIELD_COMBAT_RATING_1 + 14)
#define PLAYER_RATING_MODIFIER_RANGED_CRIT_RESILIENCE			(PLAYER_FIELD_COMBAT_RATING_1 + 15)	//almost 3 times weaker the spell resiliance. Is it still used ?
//#define PLAYER_RATING_MODIFIER_SPELL_CRIT_RESILIENCE			(PLAYER_FIELD_COMBAT_RATING_1 + 16)
//	#define PLAYER_RATING_MODIFIER_MELEE_CRIT_RESILIENCE			PLAYER_RATING_MODIFIER_SPELL_CRIT_RESILIENCE	//seems to be removed in 403 client
#define PLAYER_RATING_MODIFIER_MELEE_HASTE						(PLAYER_FIELD_COMBAT_RATING_1 + 17)
#define PLAYER_RATING_MODIFIER_RANGED_HASTE						(PLAYER_FIELD_COMBAT_RATING_1 + 18)
#define PLAYER_RATING_MODIFIER_SPELL_HASTE						(PLAYER_FIELD_COMBAT_RATING_1 + 19)
#define PLAYER_RATING_MODIFIER_MELEE_MAIN_HAND_SKILL			(PLAYER_FIELD_COMBAT_RATING_1 + 20)
#define PLAYER_RATING_MODIFIER_MELEE_OFF_HAND_SKILL				(PLAYER_FIELD_COMBAT_RATING_1 + 21)
#define PLAYER_RATING_MODIFIER_HIT_AVOIDANCE_RATING             (PLAYER_FIELD_COMBAT_RATING_1 + 22)
#define PLAYER_RATING_MODIFIER_EXPERTISE						(PLAYER_FIELD_COMBAT_RATING_1 + 23)
#define PLAYER_RATING_MODIFIER_ARMOR_PENETRATION_RATING         (PLAYER_FIELD_COMBAT_RATING_1 + 24)
#define PLAYER_RATING_MODIFIER_MASTERY					        (PLAYER_FIELD_COMBAT_RATING_1 + 25)

#define CURRENCY_ARENA_POINT									103
#define CURRENCY_HONOR_POINT									392
#define CURRENCY_CONQUEST_POINT									390
#define CURRENCY_JUSTICE_POINT									395
#define CURRENCY_VALOR_POINT									396
#define CURRENCY_CHEF_S_AWARD									402

#define CURRENCY_LIMIT_HONOR_POINT								4000
#define CURRENCY_LIMIT_ARENA_POINT								5000

class ArenaTeam;
struct PlayerCooldown
{
	uint32	ExpireTime;
	uint32	ItemId;
	uint32	SpellId;
	uint32	ExpiretimeAfterCombat;	//potions will start cooldown after out of combat
};
//hmm and what if time is larger then X bits ? Doomed. It will insta clear server wide cooldown. Lucky client still has value
#define PLAYER_SPELL_COOLDOWN_NOT_STARTED ( 0xFFFFFFFF ) 

#define MAX_EQUIPMENT_SET_INDEX			3                          // client limit is 10 ? FFS how can you handle the complexity ?
#define MAX_EQUIPMENT_SET_INDEX_CLIENT	10                         // 

struct EquipmentSet
{
    EquipmentSet()
    {
        for(int i = 0; i < EQUIPMENT_SLOT_END; ++i)
            ItemGUIDS[i] = 0;
		need_save = false;
    }
    uint64		Guid;		//this is set GUID only used to be able to delete a set, not sure if it has a point in case name is unique
    std::string Name;
    std::string IconName;
    uint64		ItemGUIDS[EQUIPMENT_SLOT_END];
	bool		need_save;
};

//====================================================================
//  Player
//  Class that holds every created character on the server.
//
//  TODO:  Attach characters to user accounts
//====================================================================
typedef std::set<uint32>	                        SpellSet;
//typedef std::list<classScriptOverride*>             ScriptOverrideList;
typedef std::set<uint32>                            SaveSet;
typedef std::map<uint64, ByteBuffer*>               SplineMap;
//typedef std::map<uint32, ScriptOverrideList* >      SpellOverrideMap;
typedef std::map<uint32, uint32>                    SpellOverrideExtraAuraMap;
typedef std::map<uint32, FactionReputation*>        ReputationMap;
typedef std::map<uint32, uint64>                    SoloSpells;
typedef std::map<SpellEntry*, pair<uint32, uint32> >StrikeSpellMap;
typedef std::map<uint32, OnHitSpell >               StrikeSpellDmgMap;
//typedef std::map<uint32, PlayerSkill>				SkillMap;
typedef std::set<Player**>							ReferenceSet;
typedef std::map<uint32, PlayerCooldown>			PlayerCooldownMap;

#define UNIT_POSITIONING_UPDATE_INTERVAL		5000	//recon to not spam this since it is only visual thing
//just for visual beauty -> attackers and followers to circle around a unit instead standing at the same spot
class UnitToUnitPossitionAdjuster
{
public:
	// allocate a new position for this unit and adjust other units to stay in circle around us
	float				GetUnitAngle(const uint64 &guid);	//our main function
	//initializations here
	void				SetCenterUnit(Unit *owner)
	{
		CenterUnit = owner;
	}
private:
	Unit				*CenterUnit;				//circle around this
	std::list<uint64>	CircleingUnits;				//these must be all units that will circle around owner
//	uint32				NextUpdateStamp;			//do not spam updates
//	uint32				CircleType;					//attackers or guardians 
	void				UpdateCircle();				//check if units are standing nice and secsy
};

struct GOSummon
{
	uint64	GUID;
	uint32	entry;
};

class SERVER_DECL Player : public Unit
{
	friend class WorldSession;
	friend class Pet;
//	friend class SkillIterator;

public:

	Player ( uint32 guid );
	~Player ( );
	void Virtual_Destructor( );

	ARCEMU_INLINE Guild * GetGuild() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_playerInfo->guild; 
	}
	ARCEMU_INLINE GuildMember * GetGuildMember() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_playerInfo->guildMember; 
	}
	ARCEMU_INLINE GuildRank * GetGuildRankS() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_playerInfo->guildRank; 
	}

	void EventGroupFullUpdate();
	void GroupUninvite(Player *player, PlayerInfo *info);

	/************************************************************************/
	/* Skill System															*/
	/************************************************************************/

	void _AdvanceSkillLine(uint32 SkillLine, uint32 Count = 1);
	void _AddSkillLine(uint32 SkillLine, uint32 Current, uint32 Max);
	uint32 _GetSkillLineMax(uint32 SkillLine);
	uint32 _GetSkillLineCurrent(uint32 SkillLine, bool IncludeBonus = true);
	void _RemoveSkillLine( uint32 SkillLine, bool SpellsAlso = false );
	void _UpdateMaxSkillCounts();
	void _ModifySkillBonus(uint32 SkillLine, int32 Delta);
//	void _ModifySkillBonusByType(uint32 SkillType, int32 Delta);
	bool _HasSkillLine(uint32 SkillLine);
	void LearnAllSpellsFromSkillLine(uint32 skill_line,bool skip_talents, bool class_specific);
	void RemoveSpellsFromLine(uint32 skill_line);
	void _RemoveAllSkills();
	void _RemoveLanguages();
	void _AddLanguages(bool All);
	void _AdvanceAllSkills(uint32 count);
	void _ModifySkillMaximum(uint32 SkillLine, uint32 NewMax);
	void AutoLearnSkillRankSpells( uint32 SkillLine, uint32 curr_sk);

	void RecalculateHonor();

protected:

	void _UpdateSkillFields();

//	uint32 object_separator_shield[200];
	
public:
//	SkillMap m_skills;
	CommitPointerList<PlayerSkill> m_skills;	//maybe proc on proc corrupts this list ? Expected size 127 structs
	CommitPointerListNode<PlayerSkill> *SkillFind( uint32 SkillId );
	void SkillAddNonDuplicate( PlayerSkill *New );
	// COOLDOWNS
	//PlayerCooldownMap m_cooldownMap[NUM_COOLDOWN_TYPES];
//	uint32 var_col_shield1[50];
	tr1::array< PlayerCooldownMap, NUM_COOLDOWN_TYPES> m_cooldownMap;
//	uint32 var_col_shield2[50];
//	uint32 m_globalCooldown;
	void Cooldown_AddStart( SpellEntry * pSpell );
	void Cooldown_Add( SpellEntry * pSpell, uint32 ItemOrPetID );
	void Cooldown_AddItem(ItemPrototype * pProto, uint32 x, uint32 after_combat_cooldown);
	int32 Cooldown_Getremaining(SpellEntry * pSpell, uint32 ItemOrPetID = 0 );
	bool Cooldown_CanCast(SpellEntry * pSpell) { return ( Cooldown_Getremaining( pSpell ) == 0 ); }
	void StartOutOfCombatCooldowns();
	void ClearCooldownsOnLine(uint32 skill_line, uint32 called_from);
	void ResetAllCooldowns(bool IncludingProcs = true );
	void ClearCooldownForSpell(uint32 spell_id);
	//not client side, just server side for rapid casting 
	void ClearCategoryCooldownForSpell(uint32 spell_id, bool OnlyStartCategory);
	void StartClientCooldown( uint32 spell_id );
	void UpdateClientCooldown( uint32 spell_id, uint32 cooldown );
	//another blizz great invention
	void ModCooldown( uint32 spell_id, int32 time_mod, bool StartNew, Object *ChildObj = NULL );
	void _Cooldown_Add(uint32 Type, uint32 Misc, uint32 Time, uint32 SpellId, uint32 ItemId, uint32 after_combat_cooldown = 0);

protected:
	void _LoadPlayerCooldowns(QueryResult * result);
	void _SavePlayerCooldowns(QueryBuffer * buf);

	// END COOLDOWNS

public:

	bool ok_to_remove;
	//uint64 m_spellIndexTypeTargets[NUM_SPELL_TYPE_INDEX];
	tr1::array< uint64, NUM_SPELL_TYPE_INDEX> m_spellIndexTypeTargets;
	Spell * m_currentSpellAny;	//any spell, can be proc spell also. Note that spells that create chained cast will delete previously set value of this

	void OnLogin();//custom stuff on player login.
	void RemoveSpellTargets(uint32 Type, Unit* target);
	void RemoveSpellIndexReferences(uint32 Type);
	void SetSpellTargetType(uint32 Type, Unit* target);
	void SendMeetingStoneQueue(uint32 DungeonId, uint8 Status);
	void SendDungeonDifficulty();
	void SendRaidDifficulty();

	void AddToWorld();
	void AddToWorld(MapMgr * pMapMgr);
	void RemoveFromWorld();
	void RemoveFromWorld(bool free_guid)	// to overwrite virtual function call
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		RemoveFromWorld();
	}
	void EventClientFinishedLoadingScreen();	//send packets now ? In 420 client netwrok queue gets saturated quickly and crashes client if not carefull
	bool Create ( WorldPacket &data );
	
	void Update( uint32 time );
    void BuildFlagUpdateForNonGroupSet(uint32 index, uint32 flag);
	std::string m_afk_reason;
	void SetAFKReason(std::string reason) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_afk_reason = reason; 
	}
	ARCEMU_INLINE const char* GetName() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_name.c_str(); 
	}
	ARCEMU_INLINE std::string* GetNameString() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return &m_name; 
	}
	void Die();
	//void KilledMonster(uint32 entry, const uint64 &guid);
	void GiveXP(uint32 xp, const uint64 &guid, bool allowbonus);   // to stop rest xp being given
	void ModifyBonuses( uint32 type, int32 val, bool apply );
	std::map<uint32, uint32> m_wratings;

	//ArenaTeam * m_arenaTeams[NUM_ARENA_TEAM_TYPES];
	tr1::array<ArenaTeam *, NUM_ARENA_TEAM_TYPES> m_arenaTeams;
	
    /************************************************************************/
    /* Taxi                                                                 */
    /************************************************************************/
    ARCEMU_INLINE TaxiPath*    GetTaxiPath() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_CurrentTaxiPath; 
	}
    ARCEMU_INLINE bool         GetTaxiState() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_onTaxi; 
	}
    uint32		        GetTaximask( uint8 index )  
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_taximask[index]; 
	}
    void                LoadTaxiMask(const char* data);
    void                TaxiStart(TaxiPath* path, uint32 modelid, uint32 start_node);
    void                JumpToEndTaxiNode(TaxiPath * path);
    void                EventDismount(uint32 money, float x, float y, float z);
    void                EventTaxiInterpolate();

    ARCEMU_INLINE void         SetTaxiState    (bool state) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_onTaxi = state; 
	}
    ARCEMU_INLINE void         SetTaximask     (uint8 index, uint32 value ) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		if( index < TAXIMASK_SIZE ) m_taximask[index] = value; 
	}
    ARCEMU_INLINE void         SetTaxiPath     (TaxiPath *path) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_CurrentTaxiPath = path; 
	}
    ARCEMU_INLINE void         SetTaxiPos()	
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_taxi_pos_x = m_position.x; m_taxi_pos_y = m_position.y; m_taxi_pos_z = m_position.z;
	}
    ARCEMU_INLINE void         UnSetTaxiPos()	
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_taxi_pos_x = 0; m_taxi_pos_y = 0; m_taxi_pos_z = 0; 
	}
 
	// Taxi related variables
	vector<TaxiPath*>   m_taxiPaths;
    TaxiPath*           m_CurrentTaxiPath;
    uint32              taxi_model_id;
	uint32              lastNode;
    uint32              m_taxi_ride_time;
    //uint32              m_taximask[TAXIMASK_SIZE];
	tr1::array<uint32,TAXIMASK_SIZE> m_taximask;
    float               m_taxi_pos_x;
    float               m_taxi_pos_y;
    float               m_taxi_pos_z;
    bool                m_onTaxi;
	uint32				m_taxiMapChangeNode;

    /************************************************************************/
    /* Quests                                                               */
    /************************************************************************/
	bool HasQuests() 
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		for(int i = 0; i < MAX_QUEST_LOG_SIZE; ++i)
		{
			if(m_questlog[i] != 0)
				return true;
		}
		return false;
	}

	int32                GetOpenQuestSlot();
	QuestLogEntry*       GetQuestLogForEntry(uint32 quest);
	ARCEMU_INLINE QuestLogEntry*GetQuestLogInSlot(uint32 slot)  
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		if( slot >= MAX_QUEST_LOG_SIZE )
			return NULL;
		return m_questlog[slot]; 
	}
    ARCEMU_INLINE uint32        GetQuestSharer()                
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_questSharer; 
	}
    
    ARCEMU_INLINE void         SetQuestSharer(uint32 guid)     
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_questSharer = guid; 
	}
    void                SetQuestLogSlot(QuestLogEntry *entry, uint32 slot);
    
    ARCEMU_INLINE void         PushToRemovedQuests(uint32 questid)	
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_removequests.insert(questid);
	}
	ARCEMU_INLINE void			PushToFinishedDailies(uint32 questid) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		DailyMutex.Acquire(); m_finishedDailies.insert(questid); DailyMutex.Release();
	}
	ARCEMU_INLINE bool		HasFinishedDaily(uint32 questid) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return (m_finishedDailies.find(questid) == m_finishedDailies.end() ? false : true); 
	}
	ARCEMU_INLINE bool		HasFinishedWeekly(uint32 questid) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return (m_finishedWeeklies.find(questid) == m_finishedWeeklies.end() ? false : true); 
	}
	ARCEMU_INLINE void			PushToFinishedWeeklies(uint32 questid) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		DailyMutex.Acquire(); m_finishedWeeklies.insert(questid); DailyMutex.Release();
	}
    void                AddToFinishedQuests(uint32 quest_id);
    void                EventTimedQuestExpire(Quest *qst, QuestLogEntry *qle, uint32 log_slot);
	void                RemoveQuestsFromLine(int skill_line);
	void				AreaExploredOrEventHappens( uint32 questId ); // scriptdev2

	bool                HasFinishedQuest(uint32 quest_id);
	bool				GetQuestRewardStatus(uint32 quest_id);
	bool                HasQuestForItem(uint32 itemid);
    bool                CanFinishQuest(Quest* qst);
	bool                HasQuestSpell(uint32 spellid);
	void                RemoveQuestSpell(uint32 spellid);
	bool                HasQuestMob(uint32 entry);
	bool                HasQuest(uint32 entry);
	void                RemoveQuestMob(uint32 entry);

    //Quest related variables
	//uint32 m_questbarrier1[25];
//	tr1::array<uint32, 25> m_questbarrier1;
    //QuestLogEntry*      m_questlog[25];
	tr1::array< QuestLogEntry*, MAX_QUEST_LOG_SIZE> m_questlog;
	//uint32 m_questbarrier2[25];
//	tr1::array< uint32, 25> m_questbarrier2;
    std::set<uint32>    m_QuestGOInProgress;
    std::set<uint32>    m_removequests;
    std::set<uint32>    m_finishedQuests;
	Mutex				DailyMutex;
	std::set<uint32>	m_finishedDailies;
	std::set<uint32>	m_finishedWeeklies;
    uint32              m_questSharer;
//    uint32              timed_quest_slot;
	std::set<uint32>    quest_spells;
	std::set<uint32>    quest_mobs;

    void EventPortToGM(Unit *p);
	ARCEMU_INLINE uint32 GetTeam() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_team; 
	}
	ARCEMU_INLINE uint32 GetTeamInitial() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return myRace->TeamID==7 ? 0 : 1; //7 is horde
	}
	ARCEMU_INLINE void SetTeam(uint32 t) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_team = t; m_bgTeam=t; 
	}
	ARCEMU_INLINE void ResetTeam() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_team = myRace->TeamID==7 ? 0 : 1; m_bgTeam=m_team; 
	}

	ARCEMU_INLINE bool IsInFeralForm()
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		int s = GetShapeShift();
		if( s <= 0 )
			return false;

		//Fight forms that do not use player's weapon
		return ( s == FORM_BEAR || s == FORM_DIREBEAR || s == FORM_CAT );
		//Shady: actually ghostwolf form doesn't use weapon too.
	}
	void CalcDamage();
	uint32 GetMainMeleeDamage(uint32 AP_owerride); //i need this for windfury

    ARCEMU_INLINE const uint64& GetSelection( ) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_curSelection; 
	}
	ARCEMU_INLINE const uint64& GetTarget( ) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_curTarget; 
	}
	void SetSelection(const uint64 &guid) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_curSelection = guid; 
	}
	void SetTarget(const uint64 &guid) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_curTarget = guid; 
	}
	
    /************************************************************************/
    /* Spells                                                               */
    /************************************************************************/
	bool HasSpell(uint32 spell);
	bool HasSpellwithNameHash(uint32 hash);
	bool HasDeletedSpell(uint32 spell);
	void smsg_InitialSpells();
	void addSpell(uint32 spell_idy,bool skip_skill_add_due_to_load=false,bool auto_add_actionbar=false);
	void removeSpellByHashName(uint32 hash, bool skip_glyphs = true, bool OnlyTalents = false );
	bool removeSpell(uint32 SpellID, bool MoveToDeleted, bool SupercededSpell, uint32 SupercededSpellID);
	bool removeDeletedSpell( uint32 SpellID );

    // PLEASE DO NOT INLINE!
/*    void AddOnStrikeSpell(SpellEntry* sp, uint32 delay)
    {
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
        m_onStrikeSpells.insert( map< SpellEntry*, pair<uint32, uint32> >::value_type( sp, make_pair( delay, 0 ) ) );
    }
    void RemoveOnStrikeSpell(SpellEntry *sp)
    {
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
        m_onStrikeSpells.erase(sp);
    }
    void AddOnStrikeSpellDamage(uint32 spellid, uint32 mindmg, uint32 maxdmg)
    {
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
        OnHitSpell sp;
        sp.spellid = spellid;
        sp.mindmg = mindmg;
        sp.maxdmg = maxdmg;
        m_onStrikeSpellDmg[spellid] = sp;
    }
    void RemoveOnStrikeSpellDamage(uint32 spellid)
    {
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
        m_onStrikeSpellDmg.erase(spellid);
    }

    //Spells variables
    StrikeSpellMap      m_onStrikeSpells;
    StrikeSpellDmgMap   m_onStrikeSpellDmg;
	*/
	SimplePointerList<SpellCanCastScript>			mSpellCanCastOverrideMap;		//one spell denies the cast of other. Right now only Deterrence
	SimplePointerList<SpellCanTargetedScript>		mSpellCanTargetedOverrideMap;	//Immunity list. Could add generic handlers here
    SpellSet            mSpells;
    SpellSet            mTempSpells;			//spells that are gained temporary from auras and scripts. Ex : Fire!,Dark Simulacrum 
	std::map<uint32,uint32>	mSpellReplaces;		//more idiot things. example neather ward will have an aura mod that will apply spell book swaps
    SpellSet            mDeletedSpells;
	SpellSet			mShapeShiftSpells;

	void AddShapeShiftSpell(uint32 id);
	void RemoveShapeShiftSpell(uint32 id);


    /************************************************************************/
    /* Actionbar                                                            */
    /************************************************************************/
	void                setAction(uint8 button, uint32 action, uint8 type);
	uint32				GetAction(uint8 button)
	{ 
		if( button >= PLAYER_ACTION_BUTTON_COUNT )
			return 0; 
		return GetActionButtonSpell( m_specs[ m_talentActiveSpec ].ActionButtons[ button ] );
	}
	void                SendInitialActions();
	void				SwapActionButtonSpell(uint32 from, uint32 to, bool add_new, bool remove_old, bool learn_send = true );	//this is used by new blizz spell mechanics fro insta cast no cooldown spells
	
    /************************************************************************/
    /* Reputation                                                           */
    /************************************************************************/
	void                ModStanding(uint32 Faction, int32 Value, int32 inloop=0);
	void				ChainedModStanding( uint32 Faction, int32 Value);
	int32               GetStanding(uint32 Faction);
	int32               GetBaseStanding(uint32 Faction);
	void                SetStanding(uint32 Faction, int32 Value, int32 inloop=0);
	void                SetAtWar(uint32 Faction, bool Set);
	bool                IsAtWar(uint32 Faction);
	Standing            GetStandingRank(uint32 Faction);
	bool                IsHostileBasedOnReputation(FactionDBC * dbc);
//	void                UpdateInrangeSetsBasedOnReputation();
	void                Reputation_OnKilledUnit(Unit * pUnit, bool InnerLoop);
	void                Reputation_OnTalk(FactionDBC * dbc);
	static Standing     GetReputationRankFromStanding(int32 Standing_);
	void				SetFactionInactive( uint32 faction, bool set );
	bool				AddNewFaction( FactionDBC * dbc, int32 standing, bool base );
	void				OnModStanding( FactionDBC * dbc, FactionReputation * rep, int32 value );
	
    /************************************************************************/
    /* Factions                                                             */
    /************************************************************************/
	void smsg_InitialFactions();
    // factions variables
    int32	pctReputationMod;
	uint32	dungeon_factiongain_redirect;

    /************************************************************************/
    /* PVP                                                                  */
    /************************************************************************/
	ARCEMU_INLINE uint8 GetPVPRank()
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return (uint8)((GetUInt32Value(PLAYER_BYTES_3) >> 24) & 0xFF);
	}
	ARCEMU_INLINE void SetPVPRank(int newrank)
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		SetUInt32Value(PLAYER_BYTES_3, ((GetUInt32Value(PLAYER_BYTES_3) & 0x00FFFFFF) | (uint8(newrank) << 24)));
	}
	uint32 GetMaxPersonalRating();
	
	ARCEMU_INLINE bool HasKnownTitle( RankTitles title )
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		uint32	int_index = title / 32;
		uint32	bitmask = 1 << ( title % 32 );
		uint32  current = GetUInt32Value( PLAYER_FIELD_KNOWN_TITLES + int_index );
		return ( current & bitmask ) != 0;
	}

	void SetKnownTitle( RankTitles title, bool set );
    /************************************************************************/
    /* Groups                                                               */
    /************************************************************************/
	void                SetInviter(uint32 pInviter) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_GroupInviter = pInviter; 
	}
	uint32 GetInviter();
	ARCEMU_INLINE uint32 HasBeenInvited() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return GetInviter() != 0; 
	}
	ARCEMU_INLINE bool         InGroup() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return (m_playerInfo->m_Group != NULL && !m_GroupInviter); 
	}
	bool                IsGroupLeader()
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		if(m_playerInfo->m_Group != NULL)
		{
			if(m_playerInfo->m_Group->GetLeader() == m_playerInfo)
				return true;
		}
		return false;
	}

	ARCEMU_INLINE Group*       GetGroup() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_playerInfo ? m_playerInfo->m_Group : NULL; 
	}
	ARCEMU_INLINE int8		   GetSubGroup() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_playerInfo->subGroup; 
	}
    bool                IsGroupMember(Player *plyr);
	ARCEMU_INLINE bool         IsBanned()
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		if(m_banned)
		{
			if(m_banned < 100 || (uint32)UNIXTIME < m_banned)
				return true;
		}
		return false;
	}
    ARCEMU_INLINE void         SetBanned() { m_banned = 4;}
	ARCEMU_INLINE void         SetBanned(string Reason) { m_banned = 4; m_banreason = Reason;}
	ARCEMU_INLINE void         SetBanned(uint32 timestamp, string& Reason) { m_banned = timestamp; m_banreason = Reason; }
	ARCEMU_INLINE void         UnSetBanned() { m_banned = 0; }
	ARCEMU_INLINE string       GetBanReason() {return m_banreason;}

    /************************************************************************/
    /* Guilds                                                               */
    /************************************************************************/
	uint32 m_GuildId;
	ARCEMU_INLINE  bool        IsInGuild() 
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return (m_GuildId!=0);
	}
	ARCEMU_INLINE uint32       GetGuildId() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_GuildId;
	}
	void                SetGuildId(uint32 guildId);
	ARCEMU_INLINE uint32       GetGuildRank() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_uint32Values[PLAYER_GUILDRANK]; 
	}
	void                SetGuildRank(uint32 guildRank);
	uint32              GetGuildInvitersGuid() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_invitersGuid; 
	}
	void                SetGuildInvitersGuid( uint32 guid ) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_invitersGuid = guid; 
	}
	void                UnSetGuildInvitersGuid() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_invitersGuid = 0; 
	}
  
    /************************************************************************/
    /* Duel                                                                 */
    /************************************************************************/
    void                RequestDuel(Player *pTarget);
	void                DuelBoundaryTest();
	void                EndDuel(uint8 WinCondition);
	void                DuelCountdown();
	void                SetDuelStatus(uint8 status) 
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_duelStatus = status; 
	}
	ARCEMU_INLINE uint8        GetDuelStatus() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_duelStatus; 
	}
	void                SetDuelState(uint8 state) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_duelState = state; 
	}
	ARCEMU_INLINE uint8        GetDuelState() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_duelState; 
	}
    // duel variables
    Player*             DuelingWith;

    /************************************************************************/
    /* Trade                                                                */
    /************************************************************************/
	void                SendTradeUpdate(uint8 is_update=1);
	void				ResetTradeVariables()
	{
		mTradeGold = 0;
		for( uint32 i=0;i<TRADE_TOTAL_TRADE_SLOTS;i++)
			mTradeItems[i]=0;
		mTradeStatus = 0;
		mTradeTarget = 0;
		m_tradeSequence = 2;
	}
	
    /************************************************************************/
    /* Pets                                                                 */
    /************************************************************************/
	ARCEMU_INLINE void			SetSummon(Pet *pet,uint32 index=0) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_Summon[index] = pet; 
	}
	ARCEMU_INLINE Pet*			GetSummon(uint32 index=0) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_Summon[index]; 
	}
	uint32						GeneratePetNumber(uint32 CreatureEntry = -1);
	void						RemovePlayerPet( uint32 pet_number, uint32 SlotNumber );
	ARCEMU_INLINE void			AddPlayerPet(PlayerPet* pet, uint32 pet_number) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		std::list<PlayerPet*>::iterator itr;
		for( itr = m_Pets.begin();itr!=m_Pets.end(); itr++)
			if( (*itr) == pet || (*itr)->number == pet->number )
				return;
		m_Pets.push_front( pet ); 
		UpdateClientCallPetSpellIcons();
	}
	ARCEMU_INLINE PlayerPet*	GetPlayerPet( uint32 pPetNumber, uint32 CreatureEntry = -1, uint32 pStableSlot = -1 )
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		std::list<PlayerPet*>::iterator itr;
		for( itr = m_Pets.begin();itr!=m_Pets.end(); itr++)
			if( (*itr)->number == pPetNumber || (*itr)->StableSlot == pStableSlot || (*itr)->entry == CreatureEntry )
				return (*itr);
		return NULL;
	}
	void						SpawnPet(uint32 pet_number, uint32 SlotNumber);
//	void						SpawnActivePet();
	void						AutoResummonLastPet();
	void						SetAutoSummonPetNR( uint32 PetNr ) { m_AutoRessummonPetNr = PetNr; }

	ARCEMU_INLINE uint8         GetPetCount(void) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return (uint8)m_Pets.size(); 
	}
	ARCEMU_INLINE void			SetStableSlotCount(uint8 count) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_StableSlotCount = count; 
	}
	ARCEMU_INLINE uint8			GetStableSlotCount(void) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_StableSlotCount; 
	}
#define MAX_ALLOWED_UNSTABLED_PETS 5
#define MAX_ALLOWED_STABLED_PETS 20
#define MAX_ALLOWED_TOTAL_PETS (MAX_ALLOWED_UNSTABLED_PETS+MAX_ALLOWED_STABLED_PETS)
#define IsPetActive( x ) ( x->StableSlot < MAX_ALLOWED_UNSTABLED_PETS )
	uint32						GetUnstabledPetNumber(void)
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		if( m_Pets.empty() ) 
			return 0;
		uint32 ret = 0;
		std::list<PlayerPet*>::iterator itr;
		for(itr = m_Pets.begin();itr != m_Pets.end();itr++)
			if( IsPetActive( (*itr ) ) )
				ret++;
		return ret;
	} 
	void						EventSummonPet(Pet *new_pet);		//if we charmed or simply summoned a pet, this function should get called
	void						EventSummonPetCastSpell( Pet *new_pet, uint32 SpellID );	//talents that need recasting on pet summon
	void						EventDismissPet();					//if pet/charm died or whatever happned we should call this function
	void						ForceAllPetTalentAndSpellReset();	//this is for hunter talent when we need to reset pet talents even if they are offline
	void						UpdateClientCallPetSpellIcons();

    /************************************************************************/
    /* Item Interface                                                       */
    /************************************************************************/
	ARCEMU_INLINE ItemInterface* GetItemInterface() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_ItemInterface; 
	} // Player Inventory Item storage
	ARCEMU_INLINE void			ApplyItemMods(Item *item, int16 slot, bool apply,bool justdrokedown=false) 
	{  
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		_ApplyItemMods(item, slot, apply,justdrokedown); 
	}
	ARCEMU_INLINE bool			HasItemCount( uint32 item, uint32 count, bool inBankAlso = false );
    // item interface variables
    ItemInterface *     m_ItemInterface;
	ARCEMU_INLINE float			GetWeaponNormalizationCoeffForAP( WeaponDamageType Type );
	
    /************************************************************************/
    /* Loot                                                                 */
    /************************************************************************/
	ARCEMU_INLINE const uint64& GetLootGUID() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_lootGuid; 
	}
	ARCEMU_INLINE void         SetLootGUID(const uint64 &guid) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_lootGuid = guid; 
	}
	void                SendLoot(uint64 guid,uint8 loot_type);
    // loot variables
    uint64              m_lootGuid;
    uint64              m_currentLoot;
    bool                bShouldHaveLootableOnCorpse;

    /************************************************************************/
    /* World Session                                                        */
    /************************************************************************/
	ARCEMU_INLINE WorldSession* GetSession() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_session; 
	}
	void SetSession(WorldSession *s) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_session = s; 
	}
	void SetBindPoint(float x, float y, float z, uint32 m, uint32 v) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_bind_pos_x = x; m_bind_pos_y = y; m_bind_pos_z = z; m_bind_mapid = m; m_bind_zoneid = v;
	}
	void SendDelayedPacket(WorldPacket *data, bool bDeleteOnSend)
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		if(data == NULL) 
			return;
		if(GetSession() != NULL) 
			GetSession()->SendPacket(data);
		if(bDeleteOnSend)
			delete data;
	}
	float offhand_dmg_mod;
	float GetSpellTimeMod(uint32 id);
	int GetSpellDamageMod(uint32 id);
	int32 GetSpellManaMod(uint32 id);
	
	// These functions build a specific type of A9 packet
	uint32 __fastcall BuildCreateUpdateBlockForPlayer( ByteBuffer *data, Player *target );
//	void DestroyForPlayer( Player *target, uint8 anim = 0);
	
	std::list<LoginAura> loginauras;
//    std::set<uint32> OnMeleeAuras;

    /************************************************************************/
    /* Player loading and savings                                           */
    /* Serialize character to db                                            */
    /************************************************************************/
	void SaveToDB(bool bNewCharacter);
	void SaveAuras(stringstream&);
	bool LoadFromDB(uint32 guid);
	void LoadFromDBProc(QueryResultVector & results);

	void LoadNamesFromDB(uint32 guid);
	bool m_FirstLogin;

    /************************************************************************/
    /* Death system                                                         */
    /************************************************************************/
	void SpawnCorpseBody();
	void SpawnCorpseBones();
	void CreateCorpse();
	void KillPlayer();
	void SetDeathStateCorpse();
	void ResurrectPlayer();
	void DeathDurabilityLoss(double percent);
	void RepopAtGraveyard(float ox, float oy, float oz, uint32 mapid);
	void RemoteRevive()
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		ResurrectPlayer();
		SetMovement( MOVE_UNROOT );
		SetPlayerSpeed( RUN, PLAYER_NORMAL_RUN_SPEED );
		SetPlayerSpeed( SWIM, PLAYER_NORMAL_SWIM_SPEED );
		SetMovement( MOVE_LAND_WALK );
		SetUInt32Value( UNIT_FIELD_HEALTH, GetUInt32Value( UNIT_FIELD_MAXHEALTH ) );
	}
	uint64			m_resurrecter;
	bool			bCorpseCreateable;	//when player gets skinned, he can't have a corpse
	uint32			m_resurrectHealth, m_resurrectMana;
	uint32			m_resurrectInstanceID, m_resurrectMapId;
	LocationVector	m_resurrectPosition;
	uint32			m_myCorpseLowGuid;
	
    /************************************************************************/
    /* Movement system                                                      */
    /************************************************************************/
	void SetMovement(uint8 pType);
	void SetPlayerSpeed(uint8 SpeedType, float value);
	float GetPlayerSpeed(){return m_runSpeed;}
	uint8 m_currentMovement;
	bool m_isMoving;
	bool moving;
	uint32 last_moved;
	bool strafing;
	bool jumping;
	//Invisibility stuff
//	bool m_isGmInvisible;
//	bool m_deathVision;
	bool IsGMInvisible() { return ((INVIS_FLAG_CUSTOM_GM & m_invisFlag) != 0 ); }
	void SetGMInvisible() { m_invisFlag |= INVIS_FLAG_CUSTOM_GM; }
	void RemGMInvisible() { m_invisFlag &= ~INVIS_FLAG_CUSTOM_GM; }
	bool IsDeathVision() { return ((INVIS_FLAG_GHOST & m_invisDetect) != 0 ); }
	void SetDeathVision() { m_invisDetect |= INVIS_FLAG_GHOST; }
	void RemDeathVision() { m_invisDetect &= ~INVIS_FLAG_GHOST; }
	
    /************************************************************************/
    /* Channel stuff                                                        */
    /************************************************************************/
	void JoinedChannel(Channel *c);
	void LeftChannel(Channel *c);
	void CleanupChannels();

	/************************************************************************/
	/* Attack stuff															*/
    /************************************************************************/
	void EventAttackStart();
	void EventAttackStop();
	void EventAttackUpdateSpeed() { }
	void EventDeath();
	//Note:ModSkillLine -> value+=amt;ModSkillMax -->value=amt; --wierd
	float GetSkillUpChance(uint32 id);
	//ARCEMU_INLINE std::list<struct skilllines>getSkillLines() { return m_skilllines; }
	float SpellCrtiticalStrikeRatingBonus;
	void ModAttackSpeed( int32 mod, HasteModType type );
	void UpdateAttackSpeed();
	float GetDefenseChance(uint32 opLevel);
	float GetDodgeChance();
	float GetBlockChance();
	float GetParryChance();
	void UpdateChances();
	void UpdateStats();
	void UpdatePowerRegen();	//depends on haste and stats
	void UpdateHit(int32 hit);
   
	bool canCast(SpellEntry *m_spellInfo);
	ARCEMU_INLINE float GetSpellCritFromSpell() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_spellcritfromspell; 
	}
	ARCEMU_INLINE float GetHitFromSpell() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_hitfromspell; 
	}
	void SetSpellCritFromSpell(float value) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_spellcritfromspell = value; 
	}
	void SetHitFromSpell(float value) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_hitfromspell = value; 
		SetFloatValue( PLAYER_FIELD_UI_SPELL_HIT_MODIFIER, m_hitfromspell );
	}
	ARCEMU_INLINE uint32 GetHealthFromSpell() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_healthfromspell; 
	}
	ARCEMU_INLINE uint32 GetManaFromSpell() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_manafromspell; 
	}
	void SetHealthFromSpell(uint32 value) 
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_healthfromspell = value;
	}
	void SetManaFromSpell(uint32 value) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_manafromspell = value;
	}
	uint32 CalcTalentResetCost(uint32 resetnum);
	void SendTalentResetConfirm();
//	void SendPetUntrainConfirm();
	uint32 GetTalentResetTimes() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_talentresettimes; 
	}
	ARCEMU_INLINE void SetTalentResetTimes(uint32 value) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_talentresettimes = value; 
	}
	//////////////////////////////
	//multi talent specing
	//////////////////////////////
	uint32 GetFirstTalentSpecTree();
	uint32 CalcTalentPointsShouldHaveMax();
	uint32 CalcTalentPointsHaveSpent(uint32 spec);
	void fill_talent_info_in_packet( WorldPacket &data );
	uint8 m_talentSpecsCount;
	uint8 m_talentActiveSpec;
	struct PlayerSpec
	{
		uint32													tree_lock;	//tree is the spec ID where you put first talent point
		std::map<uint32, uint8>									talents;	// map of <talentId, talentRank>
		tr1::array< uint16, GLYPHS_COUNT >						glyphs;
		SpellSet												SpecSpecificSpells;
		tr1::array< uint32, PLAYER_ACTION_BUTTON_COUNT >		ActionButtons;
	};

	//PlayerSpec m_specs[MAX_SPEC_COUNT];
	tr1::array< PlayerSpec, MAX_SPEC_COUNT > m_specs;
				
	void	Event_Learn_Talent( uint32 newTalentId, uint8 newTalentRank, uint8 point_consume = 1, bool no_client_update = true );
	void	Event_UnLearn_Talent( uint32 newTalentId, uint32 SpellId, uint32 Rank );
	void	Switch_Talent_Spec(); //remove old talents active on us and apply new ones
    void	LearnTalent( uint32 talentid, uint32 rank, bool isPreviewed = false ); 
	void	Reset_Talent(SpellEntry *spellInfo,uint32 loop=0, bool unlearn = true, bool OnlyTalents = true);
	void	Reset_Talent(TalentEntry *tmpTalent);
	void	Reset_Talents();

	// end multi talent specing
	void SetPlayerStatus(uint8 pStatus) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_status = pStatus; 
	}
	ARCEMU_INLINE uint8 GetPlayerStatus() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_status; 
	}
	ARCEMU_INLINE const float& GetBindPositionX( ) 
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_bind_pos_x; 
	}
	ARCEMU_INLINE const float& GetBindPositionY( ) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_bind_pos_y; 
	}
	ARCEMU_INLINE const float& GetBindPositionZ( ) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_bind_pos_z; 
	}
	ARCEMU_INLINE const uint32& GetBindMapId( ) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_bind_mapid; 
	}
	ARCEMU_INLINE const uint32& GetBindZoneId( )
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_bind_zoneid; 
	}
	ARCEMU_INLINE uint8 GetShapeShift()
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return GetByte(UNIT_FIELD_BYTES_2,3);
	}
	ARCEMU_INLINE uint32 GetShapeShiftMask()
	{
		uint32 ss = GetShapeShift();
		if( ss == 0 )
			ss = (1 << (FORM_CUSTOM_NORMAL-1)) | FORM_NORMAL_FORM | FORM_NORMAL_FORM2;
		else
			ss = 1 << ( ss - 1 );
		return ss;
	}

	
	void delayAttackTimer(int32 delay)
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		if(!delay)
			return;

		m_attackTimer += delay;
		m_attackTimer_1 += delay;
	}

	void SetShapeShift(uint8 ss);

    //Showing Units WayPoints
	AIInterface* waypointunit;
	
	uint32 m_nextSave;
	//Tutorials
	uint32 GetTutorialInt(uint32 intId );
	void SetTutorialInt(uint32 intId, uint32 value);
	//Base stats calculations
	//void CalcBaseStats();
	// Rest
	uint32 SubtractRestXP(uint32 amount);
	void AddCalculatedRestXP(uint32 seconds);
	void ApplyPlayerRestState(bool apply);
	void UpdateRestState();
	bool m_noFallDamage;
	float z_axisposition;
	int32 m_safeFall;
	// Gossip
	GossipMenu* CurrentGossipMenu;
	void CleanupGossipMenu();
	void Gossip_Complete();
//	bool m_requiresNoAmmo;
	
	// scriptdev2
	GossipMenu* PlayerTalkClass;
	void PrepareQuestMenu( uint64 guid );
	void SendGossipMenu( uint32 TitleTextId, uint64 npcGUID );
	void CloseGossip();
	QuestStatus GetQuestStatus( uint32 quest_id );

	// Water level related stuff
	void SetNoseLevel();
	// Water level related stuff (they are public because they need to be accessed fast)
	// Nose level of the character (needed for proper breathing)
	float m_noseLevel;
	bool m_waterwalk;
	bool m_setwaterwalk;	
	bool m_bUnlimitedBreath;
	uint32 m_UnderwaterTime;
	uint32 m_UnderwaterState;
	uint32 m_UnderwaterMaxTime;
	uint32 m_UnderwaterLastDmg;
	uint32 m_SwimmingTime;
	uint32 m_BreathDamageTimer;

	// Visible objects
	bool CanSee(Object* obj);
	ARCEMU_INLINE bool IsVisible(Object* pObj) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return !(m_visibleObjects.find(pObj) == m_visibleObjects.end()); 
	}
	void QueueAddInRangeObject(Object* pObj);
	void QueueRemoveInRangeObject(Object* pObj);
	void ClearInRangeSet();
	ARCEMU_INLINE void AddVisibleObject(Object* pObj) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_visibleObjects.insert(pObj); 
	}
	ARCEMU_INLINE void RemoveVisibleObject(Object* pObj) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_visibleObjects.erase(pObj); 
	}
	ARCEMU_INLINE void RemoveVisibleObject(InRangeSet::iterator itr) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_visibleObjects.erase(itr); 
	}
	ARCEMU_INLINE InRangeSet::iterator FindVisible(Object * obj) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_visibleObjects.find(obj); 
	}
	ARCEMU_INLINE void RemoveIfVisible(Object * obj)
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		InRangeSet::iterator itr = m_visibleObjects.find(obj);
		if(itr == m_visibleObjects.end())
			return;

		m_visibleObjects.erase(obj);
		PushOutOfRange(obj->GetNewGUID());
	}

	ARCEMU_INLINE bool GetVisibility(Object * obj)
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return ( m_visibleObjects.find(obj) != m_visibleObjects.end());
	}

	ARCEMU_INLINE InRangeSet::iterator GetVisibleSetBegin() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_visibleObjects.begin(); 
	}
	ARCEMU_INLINE InRangeSet::iterator GetVisibleSetEnd() { return m_visibleObjects.end(); }
	int16	m_SeeRange,m_SeeBlockRange;	//see range should be equal to map visibility range, but it could be moded by scripts

	//Transporters
	bool m_lockTransportVariables;
	uint64 m_TransporterGUID;
	float m_TransporterX;
	float m_TransporterY;
	float m_TransporterZ;
	float m_TransporterO;
	float m_TransporterTime;
	// Misc
//	void EventCannibalize(uint32 amount);
	void EventReduceDrunk(bool full);
	bool m_AllowAreaTriggerPort;
	void EventAllowTiggerPort(bool enable);

	uint32 m_modblockabsorbvaluePCT;
	uint32 m_modblockvaluefromspells;
	void SendInitialLogonPackets();
	void Reset_Spells();
	void Reset_ToLevel1();
	// Battlegrounds 
	CBattleground * m_bg;
	CBattleground * m_pendingBattleground;
	void RemovePlayerFromBGResurrect(); //this is called as safety to avoid multi queue
	void DelayedBGResurrect( uint64 SpiritGUID ); //this is called as safety to avoid multi queue
	uint32 m_bgEntryPointMap;
	float m_bgEntryPointX;	
	float m_bgEntryPointY;
	float m_bgEntryPointZ;
	float m_bgEntryPointO;
	int32 m_bgEntryPointInstance;
	bool m_bgHasFlag;
	bool m_bgIsQueued;
	uint32 m_bgQueueType;
	uint32 m_bgQueueInstanceId;
	void EventRepeatSpell();
	void EventCastRepeatedSpell(uint32 spellid, Unit *target);
	int32 CanShootRangedWeapon(uint32 spellid, Unit *target, bool autoshot);
	int32 m_AutoShotDuration;
	int32 m_AutoShotAttackTimer;
	bool m_onAutoShot;
//	uint64 m_AutoShotTarget;
	SpellEntry *m_AutoShotSpell;
	void _InitialReputation();
	void EventActivateGameObject(GameObject* obj);
	void EventDeActivateGameObject(GameObject* obj);
	void UpdateNearbyGameObjects();
	
	void CalcResistance(uint32 type);
	ARCEMU_INLINE float res_M_crit_get()
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_resist_critical[0];
	}
	ARCEMU_INLINE void res_M_crit_set(float newvalue)
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_resist_critical[0]=newvalue;
	}
	ARCEMU_INLINE float res_R_crit_get()
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_resist_critical[1];
	}
	ARCEMU_INLINE void res_R_crit_set(float newvalue)
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_resist_critical[1]=newvalue;
	}
	//uint32 FlatResistanceModifierPos[7];
	tr1::array< uint32, 7 > FlatResistanceModifierPos;
	//uint32 FlatResistanceModifierNeg[7];
	tr1::array< uint32, 7 > FlatResistanceModifierNeg;
	//uint32 BaseResistanceModPctPos[7];
	tr1::array< uint32, 7 > BaseResistanceModPctPos;
	//uint32 BaseResistanceModPctNeg[7];
	tr1::array< uint32, 7 > BaseResistanceModPctNeg;
	//uint32 ResistanceModPctPos[7];
	tr1::array< uint32, 7 > ResistanceModPctPos;
	//uint32 ResistanceModPctNeg[7];
	tr1::array< uint32, 7 > ResistanceModPctNeg;
	//float m_resist_critical[2];//when we are a victim we can have talents to decrease chance for critical hit. This is a negative value and it's added to critchances
	tr1::array< float, 2 > m_resist_critical;
	//float m_resist_hit[3]; // 0 = melee; 1= ranged; 2=spells; 3=deflect(not conditional
	tr1::array< float, 3 > m_resist_hit;
	float					m_deflect;	//unconditional chance to resist melee, ranged, spell
	//float m_attack_speed_mod[3];
	tr1::array< float, 3 > m_attack_speed_mod;
	float	m_AuraCastSpeedMods;
	//float SpellHealDoneByAttribute[5][7];
//	tr1::array< tr1::array<float, 7>,5 > SpellHealDoneByAttribute;
	tr1::array<float, 5> SpellHealDoneByAttribute;
//	tr1::array<int32, 5> DamageDoneByStatPCT;
	uint16	m_modphyscritdmgPCT,m_modSpellCritdmgPCT;
	uint32	m_FrozenCritChanceMultiplier; // Class Script Override: Shatter
	uint8	m_ForceTargetFrozen;
	int32	m_APToSPExclusivePCT;	//convert Ap to SP, but you cannot use normal SP if you have a value in this

	uint32 m_ModInterrMRegenPCT;
	int32 m_ModInterrMRegen;
	int32 m_ModMPRegen;
//	float m_RegenManaOnSpellResist;
	
	//uint32 FlatStatModPos[5];
	tr1::array<uint32, 5> FlatStatModPos;
	//uint32 FlatStatModNeg[5];
	tr1::array<uint32, 5> FlatStatModNeg;
	//uint32 StatModPctPos[5];
	tr1::array<uint32, 5> StatModPctPos;
	//uint32 StatModPctNeg[5];
	tr1::array<uint32, 5> StatModPctNeg;
	//uint32 TotalStatModPctPos[5];
	tr1::array<uint32, 5> TotalStatModPctPos;
	//uint32 TotalStatModPctNeg[5];
	tr1::array<uint32, 5> TotalStatModPctNeg;
	//int32 IncreaseDamageByType[12]; //mod dmg by creature type
	tr1::array<int32, CREATURE_TYPES> IncreaseDamageByType;
	//float IncreaseDamageByTypePCT[12];
	tr1::array<float, CREATURE_TYPES> IncreaseDamageByTypePCT;
	//float IncreaseCricticalByTypePCT[12];
	tr1::array<float, CREATURE_TYPES> IncreaseCricticalByTypePCT;
	int32 DetectedRange;
	float PctIgnoreRegenModifier;
//	uint32 m_retainedrage;
/*	
	union {
		float mRatingToPct[37];
		uint32 mRatingToPoint[37]; //block, skill.. cant be decimal values
	};
*/
	ARCEMU_INLINE uint32* GetPlayedtime() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return &m_playedtime[0]; 
	}
	ARCEMU_INLINE uint32 GetPlayedTimeSinceLogin() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return ((uint32)UNIXTIME - m_playedtime[2]); 
	}
	void CalcStat(uint32 t);
	float CalcRating(uint32 index,uint32 FlatMod=0);
	void RecalcAllRatings();
	void RegenerateMana(bool is_interrupted);
	void RegenerateHealth(bool inCombat);
	void RegenerateEnergy();
	void RegenerateRunes();
	void RegenerateRunicPower();
	void RegenerateFocus(bool is_interrupted);
	void RegenerateHolyPower();
	void RegenerateSoulPower();
	void RegenerateEclipsePower();
	void LooseRage(int32 value);
	
	uint64 misdirectionTarget;

	ARCEMU_INLINE uint64 GetMisdirectionTarget()
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return misdirectionTarget;
	}
	void SetMisdirectionTarget(uint64 PlayerGUID)
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		misdirectionTarget = PlayerGUID;
	}

//	bool removeReagentCost;

	SimplePointerList<WeaponModifier>	damagedone;
	SimplePointerList<WeaponModifier>	tocritchance;
	SimplePointerList<WeaponModifier>	ignore_armor_pct;
	float								weapon_target_armor_pct_ignore; //value is already converted into float ! It applies only to melee
//	bool cannibalize;
//	uint8 cannibalizeCount;
	int32 rageFromDamageDealt;
	int32 rageFromDamageTaken;
	// GameObject commands
	GameObject* GetSelectedGo();
	uint64 m_GM_SelectedGO;
	
#ifndef CLUSTERING
	void _Relocate(uint32 mapid,const LocationVector & v, bool sendpending, bool force_new_world, uint32 instance_id);
#else
	void RelocateCallback(uint32 instance_were_going_to);
#endif
	void AddItemsToWorld();
	void RemoveItemsFromWorld();
	
	uint32 m_ShapeShifted;
	uint32 m_MountSpellId;
    
	ARCEMU_INLINE bool IsMounted() 
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return (m_MountSpellId!=0 ? true : false); 
	}
	
    bool bHasBindDialogOpen;
	bool bGMTagOn;
//	uint32 TrackingSpell;		//you can track more then 1 things in cataclysm
	void _EventCharmAttack();
	void _Kick();
	void Kick(uint32 delay = 0);
	void SoftDisconnect();
	uint64 m_CurrentCharm;
	Transporter * m_CurrentTransporter;
	
	Object * GetSummonedObject () 
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_SummonedObject;
	}
	void SetSummonedObject (Object * t_SummonedObject) 
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_SummonedObject = t_SummonedObject;
	}
	uint32 roll;

	bool bProcessPending;
	Mutex _bufferS;
	void PushUpdateData(ByteBuffer *data, uint32 updatecount);
    void PushCreationData(ByteBuffer *data, uint32 updatecount,Object *pObj);
	void PushOutOfRange(const WoWGuid & guid);
	void ProcessPendingUpdates(bool skip_lock=false);
	bool __fastcall CompressAndSendUpdateBuffer(uint32 size, const uint8* update_buffer);
	void ClearAllPendingUpdates();
	
	uint32 GetArmorProficiency() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return armor_proficiency; 
	}
	uint32 GetWeaponProficiency() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return weapon_proficiency; 
	}

	void SpeedCheatDelay(uint32 ms_delay);
	void SpeedCheatReset();

	void AddSplinePacket(uint64 guid, ByteBuffer* packet);
	ByteBuffer* GetAndRemoveSplinePacket(uint64 guid);
	void ClearSplinePackets();
	bool ExitInstance();
	void SaveEntryPoint(uint32 mapId);
	bool CooldownCheat;
	bool CastTimeCheat;
	bool PowerCheat;
	bool FlyCheat;
	void EventZoneChange(uint32 ZoneId);
	ARCEMU_INLINE uint32 GetAreaID() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_AreaID; 
	}
	void SetAreaID(uint32 area) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_AreaID = area; 
	}
	uint32      m_AreaID;
	
	// Instance IDs

	ARCEMU_INLINE uint32 GetPersistentInstanceId(uint32 mapId, uint32 difficulty)
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		if(mapId >= NUM_MAPS || difficulty >= NUM_INSTANCE_MODES || m_playerInfo == NULL)
			return 0;
		m_playerInfo->savedInstanceIdsLock.Acquire();
		PlayerInstanceMap::iterator itr = m_playerInfo->savedInstanceIds[difficulty].find(mapId);
		if(itr == m_playerInfo->savedInstanceIds[difficulty].end())
		{
			m_playerInfo->savedInstanceIdsLock.Release();
			return 0;
		}
		m_playerInfo->savedInstanceIdsLock.Release();
		return (*itr).second;
	}

	void SetPersistentInstanceId(Instance *pInstance);
	//Use this method carefully.. 
	void SetPersistentInstanceId(uint32 mapId, uint32 difficulty, uint32 instanceId);
	PlayerInfo *getPlayerInfo() { return m_playerInfo; }

public:
	ARCEMU_INLINE void StopMirrorTimer(uint32 Type);
	ARCEMU_INLINE void ClientControlUpdate( uint8 has_control );
	BGScore m_bgScore;
	uint32 m_bgTeam;
	void UpdateChanceFields();
	//Honor Variables
	time_t m_fallDisabledUntil;
	uint32 m_honorToday;
	uint32 m_honorYesterday;
	
	void RolloverHonor();
//	uint32 m_honorPoints;
	uint32 m_honorRolloverTime;
	uint32 m_killsToday;
	uint32 m_killsYesterday;
	uint32 m_killsLifetime;
//	uint32 m_arenaPoints;
	uint32 m_honorless;
	uint32 m_lastSeenWeather;
	InRangeSet m_visibleFarsightObjects;
	void EventTeleport(uint32 mapid, float x, float y, float z);
	void EventTeleportTaxi(uint32 mapid, float x, float y, float z);
	void ApplyLevelInfo( uint32 Level );
	void BroadcastMessage(const char* Format, ...);
//	map<uint32, set<uint32> > SummonSpells;
	map<uint32, PetSpellMap*> PetSpells;
//	void AddSummonSpell(uint32 Entry, uint32 SpellID);
//	void RemoveSummonSpell(uint32 Entry, uint32 SpellID);
//	set<uint32>* GetSummonSpells(uint32 Entry);
	LockedQueue<WorldPacket*> delayedPackets;
	set<Player *> gmTargets;
//	ARCEMU_INLINE void setMyCorpse(Corpse * corpse) { myCorpse = corpse; }
//	ARCEMU_INLINE Corpse * getMyCorpse() { return myCorpse; }
	bool blinked;
	uint32 m_explorationTimer;
	// DBC stuff
	CharRaceEntry * myRace;
	CharClassEntry * myClass;
	Unit * linkTarget;
	bool AuraStackCheat;
	bool TriggerpassCheat;
	bool SafeTeleport(uint32 MapID, uint32 InstanceID, float X, float Y, float Z, float O);
	bool SafeTeleport(uint32 MapID, uint32 InstanceID, const LocationVector & vec);
	void SafeTeleport(MapMgr * mgr, const LocationVector & vec);
	//We need to delay teleport a bit from object create / update or some updates get messed up
	void SafeTeleportDelayed(uint32 MapID, uint32 InstanceID, const LocationVector &vec, uint32 delay=1000);
	void EjectFromInstance();
	
	void EventSafeTeleport(uint32 MapID, uint32 InstanceID, LocationVector vec)
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		SafeTeleport(MapID, InstanceID, vec);
	}

	// Hack fix here!
//	void ForceZoneUpdate();

	/*****************
	  PVP Stuff
	******************/
	uint32 m_pvpTimer;
//	uint32 m_pvpFFATimer;	//someone said this has a 30 second timer. Only valid when not triggered in arenas ?
	
	void SetPvPFlag();
	//! Removal
	void RemovePvPFlag();
	//! Do this on /pvp off
	ARCEMU_INLINE void ResetPvPTimer();
	//! Stop the timer for pvp off
	ARCEMU_INLINE void StopPvPTimer() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		RemoveFlag(PLAYER_FLAGS, PLAYER_FLAGS_PVP_STATUS_COOLDOWN);
		m_pvpTimer = 0; 
//		m_pvpFFATimer = 0;
	}
	void SetFFAPvPFlag();
	void RemoveFFAPvPFlag();
	
	//! Called at login to add the honorless buff, etc.
//	void LoginPvPSetup();
	//! Update our pvp area (called when zone changes)
	void UpdatePvPArea(bool just_teleported);
	//! PvP Toggle (called on /pvp)
	void PvPToggle();

	ARCEMU_INLINE uint32 LastHonorResetTime() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return m_lastHonorResetTime; 
	}
	ARCEMU_INLINE void LastHonorResetTime(uint32 val) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_lastHonorResetTime = val; 
	}
	uint32 OnlineTime;
	bool tutorialsDirty;
	LevelInfo * lvlinfo;
	void CalculateBaseStats();
//	uint32 load_health;
//	uint32 load_mana;
	void CompleteLoading();
//	set<SpellEntry *> castSpellAtLogin;
	void OnPushToWorld();
	void OnPrePushToWorld();
	void OnWorldPortAck();
	uint32 m_TeleportState;
	set<Unit*> visiblityChangableSet;
	bool m_beingPushed;
	bool CanSignCharter(Charter * charter, Player * requester);
	//Charter * m_charters[NUM_CHARTER_TYPES];
	tr1::array< Charter *, NUM_CHARTER_TYPES> m_charters;
	uint32 flying_aura;
//	stringstream LoadAuras;
	bool resend_speed;
	bool rename_pending;
	// Groups/Raids
	uint32 m_GroupInviter;
	bool	raidgrouponlysent;
	uint8	GroupPlayerRole;			//Tank,Healer,DPS
//	uint32	instance_difficulty;		//this is the instance type the player is using right now
	//if player is not in group or normal group then use dungeon settings
	uint32	dungeon_difficulty;
	//if player is in raid group then use raid settings
	uint32	raid_difficulty;
	//instance difficulty contains both raid and dungeon values !
	uint32 GetInstanceDifficulty()
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		uint32 ret;
		if( GetGroup() )
			ret = GetGroup()->GetInstanceDifficulty();
		else
			ret = dungeon_difficulty;
//		ASSERT( ret < NUM_INSTANCE_MODES );
		return ret;
	}
	ARCEMU_INLINE void SetName(string& name) { m_name = name; }
	ARCEMU_INLINE void SetNameSuffix(const char *name) { m_name_suffix = name; }
	// spell to (delay, last time)
	
	//FactionReputation * reputationByListId[128];
	tr1::array<FactionReputation *,128> reputationByListId;
	
	uint64 m_comboTarget;
	int8 m_comboPoints;
	int8 m_spellcomboPoints; // rogue talent Ruthlessness will change combopoints while consuming them. solutions 1) add post cast prochandling, 2) delay adding the CP
	int8 m_tempComboPoints;	//used by auras that add temp points, if these get consumed they do not need to get removed
	void UpdateComboPoints();

	ARCEMU_INLINE void AddComboPoints(uint64 target, uint8 count)
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
        if(m_comboTarget == target)
			m_comboPoints += count;
		else
		{
			m_comboTarget = target;
			m_comboPoints = count;
		}
		UpdateComboPoints();
	}
	ARCEMU_INLINE void RemoveComboPoints( uint8 count )
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		if( m_comboPoints >= count )
			m_comboPoints -= count;
		else
			m_comboPoints = 0;
		UpdateComboPoints();
	}

	ARCEMU_INLINE void NullComboPoints() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_comboTarget = 0; m_comboPoints = 0; m_spellcomboPoints=0; m_tempComboPoints = 0;
		UpdateComboPoints(); 
	}
	void SendAreaTriggerMessage(const char * message, ...);
        
	// Trade Target
	//Player *getTradeTarget() {return mTradeTarget;};

	ARCEMU_INLINE Player * GetTradeTarget();

	Item *getTradeItem(uint32 slot) 
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		if( slot >= TRADE_TOTAL_TRADE_SLOTS )
			return NULL;
		return mTradeItems[slot];
	}
        
	/* Mind Control */
	void Possess(Unit * pTarget);
	void UnPossess();

	/* Last Speeds */
	ARCEMU_INLINE void UpdateLastSpeeds()
	{
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		m_lastRunSpeed = m_runSpeed;
		m_lastRunBackSpeed = m_backWalkSpeed;
		m_lastSwimSpeed = m_swimSpeed;
		m_lastRunBackSpeed = m_backSwimSpeed;
		m_lastFlySpeed = m_flySpeed;
	}

	LocationVector m_last_group_position;
	void SummonRequest(uint32 Requestor, uint32 ZoneID, uint32 MapID, uint32 InstanceID, const LocationVector & Position);

#ifdef CLUSTERING
	void EventRemoveAndDelete();
	void PackPlayerData(ByteBuffer & data);
	bool UnpackPlayerData(ByteBuffer & data);
#endif

	SpellEntry * last_heal_spell;
	LocationVector m_sentTeleportPosition;

//	void RemoveFromBattlegroundQueue();
	void FullHPMP();
	uint32 m_arenateaminviteguid;

    /************************************************************************/
    /* Spell Packet wrapper Please keep this separated                      */
    /************************************************************************/
    void SendLevelupInfo(uint32 level, uint32 Hp, uint32 Mana, uint32 Stat0, uint32 Stat1, uint32 Stat2, uint32 Stat3, uint32 Stat4);
    void SendLogXPGain(uint64 guid, uint32 NormalXP, uint32 RestedXP, bool type);
    void SendEnvironmentalDamageLog(const uint64 & guid, uint8 type, uint32 damage);
	void SendWorldStateUpdate(uint32 WorldState, uint32 Value);
	void SendCastResult(uint32 SpellId, uint8 ErrorMessage, uint8 MultiCast, uint32 Extra);
	void Gossip_SendPOI(float X, float Y, uint32 Icon, uint32 Flags, uint32 Data, const char* Name);
    /************************************************************************/
    /* End of SpellPacket wrapper                                           */
    /************************************************************************/

	Mailbox m_mailBox;

	bool m_safe_fall;
	bool m_setflycheat;
	uint64 m_areaSpiritHealer_guid;
	bool m_finishingmovesdodge;

	ARCEMU_INLINE bool IsAttacking() {return ( m_attacking || m_onAutoShot ); }

//	static void InitVisibleUpdateBits();
//	static UpdateMask m_visibleUpdateMask;

	void CopyAndSendDelayedPacket(WorldPacket * data);
	void CopyAndSendDelayedPacket(StackWorldPacket * data);
	void PartLFGChannel();
	SpeedCheatDetector	*SDetector;
	PlayerCreateInfo	*GetCreateInfo(){return info;}
protected:
	LocationVector m_summonPos;
	uint32 m_summonInstanceId;
	uint32 m_summonMapId;
	uint32 m_summoner;

	void _SetCreateBits(UpdateMask *updateMask, Player *target) const;
//	void _SetUpdateBits(UpdateMask *updateMask, Player *target) const;

	/* Update system components */
	ByteBuffer			bUpdateBuffer;
	uint32				mUpdateCount;
    ByteBuffer			bCreationBuffer;
    uint32				mCreationCount;
	ByteBuffer			mOutOfRangeIds;
	uint32				mOutOfRangeIdCount;
	std::set<uint64>	mCreateGUIDS;	//if you create then destroy something in the same packet then client will show the object forever
	std::set<uint64>	mDestroyGUIDS;  //if you destroy then create the same object in 1 packet there is a chance client will bug out
//	bool				CreateDestroyConflictDetected;	//this leaves a bad non interactable object on the map client side
//	bool				DestroyCreateConflictDetected;	//this can be handled by the client. No biggie here
	SplineMap			_splineMap;
	/* End update system */

	void _LoadTutorials(QueryResult * result);
	void _SaveTutorials(QueryBuffer * buf);
	void _SaveInventory(bool firstsave);
	void _SaveQuestLogEntry(QueryBuffer * buf);
	void _LoadQuestLogEntry(QueryResult * result);

	void _LoadPet(QueryResult * result);
//	void _LoadPetSpells(QueryResult * result);
	void _SavePet(QueryBuffer * buf);
//	void _SavePetSpells(QueryBuffer * buf);
	void _ApplyItemMods( Item* item, int16 slot, bool apply, bool justdrokedown = false, bool skip_stat_apply = false );
	void _EventAttack( bool offhand );
	void _EventExploration();

	/************************************************************************/
	/* Trade																*/
	/************************************************************************/
	//Item* mTradeItems[TRADE_TOTAL_TRADE_SLOTS];
	tr1::array< Item *, TRADE_TOTAL_TRADE_SLOTS> mTradeItems;
	uint64 mTradeGold;
	uint64 mTradeTarget;
	uint32 mTradeStatus;

    /************************************************************************/
    /* Player Class systems, info and misc things                           */
    /************************************************************************/
    PlayerCreateInfo *info;
	uint32      m_AttackMsgTimer;	// "too far away" and "wrong facing" timer
	bool        m_attacking;
	std::string m_name;	// max 21 character name
	std::string m_name_suffix;		// used by blizz to signal original realm 
	//uint32      m_Tutorials[8];
	tr1::array< uint32, 8 > m_Tutorials; 

    // Character Ban
	uint32      m_banned;
	string      m_banreason;
#define MAX_ACTIVE_PET_COUNT 2
	Pet*        m_Summon[MAX_ACTIVE_PET_COUNT];		//feral sprits would require more then 1 active pet, i bet there are more
//	uint32      m_PetNumberMax;
	std::list< PlayerPet*> m_Pets;	//pets from this list seem to get corrupted sometimes. Need to figure out when and why
	uint8		m_AutoRessummonPetNr;
	
    uint32      m_invitersGuid; // It is guild inviters guid ,0 when its not used
    

    // bind
	float m_bind_pos_x;
	float m_bind_pos_y;
	float m_bind_pos_z;
	uint32 m_bind_mapid;
	uint32 m_bind_zoneid;
	SimplePointerList<ItemSet> m_itemsets;
	//Duel
	int32 m_duelCountdownTimer;
	uint8 m_duelStatus;
	uint8 m_duelState;
	// Rested State Stuff
	uint32 m_timeLogoff;
	// Played time
	//uint32 m_playedtime[3];
	tr1::array< uint32, 3> m_playedtime;
	uint8 m_isResting;
	uint8 m_restState;
	uint32 m_restAmount;
	//combat mods
	float m_blockfromspellPCT;
	float m_critfromspell;
	float m_spellcritfromspell;
	float m_hitfromspell;
	//stats mods
	uint32 m_healthfromspell;
	uint32 m_manafromspell;
	uint32 m_healthfromitems;
	uint32 m_manafromitems;

	uint32 m_SpellPowerFromIntelect;
	
	uint32 armor_proficiency;
	uint32 weapon_proficiency;
	// Talents
	uint32 m_talentresettimes;
	// STATUS
	uint8 m_status;
	// guid of current target
	uint64 m_curTarget;
	// guid of current selection
	uint64 m_curSelection;
	// Raid
	uint8 m_targetIcon;
	//Player Action Bar
	//ActionButton mActions[PLAYER_ACTION_BUTTON_SIZE];
//	tr1::array< uint32, PLAYER_ACTION_BUTTON_COUNT> mActions;
	// Player Reputation
	ReputationMap m_reputation;
	// Pointer to this char's game client
	WorldSession *m_session;
	// Channels
	std::set<Channel*> m_channels;
	// Visible objects
	InRangeSet m_visibleObjects;
	uint8 m_StableSlotCount;

    // Fishing related
	Object *m_SummonedObject;

	uint32      m_lastHonorResetTime;
	uint32      _fields[PLAYER_END];

	uint32 m_team;
	float       m_lastRunSpeed;
	float       m_lastRunBackSpeed;
	float       m_lastSwimSpeed;
	float       m_lastBackSwimSpeed;
	float       m_lastFlySpeed;

	uint32 m_mountCheckTimer;
	void RemovePendingPlayer();
public:
	void ClearReputations() { m_reputation.clear(); }	//you sure you want to do this ?

	void addDeletedSpell(uint32 id) { mDeletedSpells.insert( id ); }

	map<uint32, uint32> m_forcedReactions;

	uint32 m_flyhackCheckTimer;
	void _FlyhackCheck();

	bool m_passOnLoot;
	uint32 m_tradeSequence;
	bool m_changingMaps;

	/************************************************************************/
	/* SOCIAL                                                               */
	/************************************************************************/
private:
	/* we may have multiple threads on this(chat) - burlex */
	Mutex m_socialLock;
	map<uint32, char*> m_friends;
	set<uint32> m_ignores;
	set<uint32> m_hasFriendList;

	void Social_SendFriendList(uint32 flag);
	
	void Social_AddFriend(const char * name, const char * note);
	void Social_RemoveFriend(uint32 guid);
	
	void Social_AddIgnore(const char * name);
	void Social_RemoveIgnore(uint32 guid);

	void Social_SetNote(uint32 guid, const char * note);

public:
	bool Social_IsIgnoring(PlayerInfo * m_info);
	bool Social_IsIgnoring(uint32 guid);

	void Social_TellFriendsOnline();
	void Social_TellFriendsOffline();

	/************************************************************************/
	/* end social                                                           */
	/************************************************************************/

	PlayerInfo * m_playerInfo;
/*	uint32 m_outStealthDamageBonusPct;
	uint32 m_outStealthDamageBonusPeriod;
	uint32 m_outStealthDamageBonusTimer; */

    /************************************************************************/
    /* Player Archievements						                            */
    /************************************************************************/
public:
	//this may change in time, We will call it each time a new monitored position is reached
	//it is up to this function to decide if we actually made an archievement in that type or not
	bool Achiement_Finished_Add(uint32 AchievementID); //so we can use it with GM commands
	bool Achiement_Has_Finished(uint32 AchievementID); //so we can use it with GM commands
	void Achiement_Remove(uint32 AchievementID); //so we can use it with GM commands
	void Event_AchiementCriteria_Received(uint32 achievementtype,uint32 check_1,uint32 check_2,int64 mod_val,uint32 action); //entry is not always used
	void GiveAchievementReward(AchievementEntry const* entry);
	void SendAchievmentStatus( uint32 criteriaid, uint64 new_value, uint32 at_stamp=0 );
	void SendAchievmentEarned( uint32 archiId, uint32 at_stamp=0, bool to_others_too = true );
#ifdef USE_HACK_TO_ENABLE_ACHIEVEMENTS
	void SendAllAchievementEarned();	//temp until i figure out how to send it at login
	void SendAllAchievementStatus();	//temp until i figure out how to send it at login
#endif
	void SendAllAchievementData(WorldSession *target); //used on login
	//id and the status
	//a criteria is a group of achievements. When all achievements are completed then a criteria is completed
	//when a criteria is completed then achievement points are awarded
	std::map<uint32,uint32>			 m_achievement_achievements;	//these are grouping small achievement criterias
	std::map<uint32,AchievementVal*> m_sub_achievement_criterias;	//achievement that belongs to a criteria, there are a lot of these
	uint32 m_achievement_points; // for quick check in case it is used as currency
private:
    /************************************************************************/
    /* Player Archievements - end				                            */
    /************************************************************************/
    /************************************************************************/
    /* Player Glyphs							                            */
    /************************************************************************/
	void UpdateGlyphsAvail();
    /************************************************************************/
    /* Player DK Rune sistem					                            */
    /************************************************************************/
public:
	//float m_runes[TOTAL_USED_RUNES]; //runes regen over 10 seconds and they deplete based on spells. Spellcasts replenish runic power
//	tr1::array< float, TOTAL_USED_RUNES> m_runes;
	uint16 m_runes[TOTAL_USED_RUNES];	//do not convert it to other list type, we are using pointers to it
	//uint8 m_rune_types[TOTAL_USED_RUNES];
	tr1::array< uint8, TOTAL_USED_RUNES> m_rune_types;
	uint8 GetFullRuneCount(uint8 type);
	uint8 TakeFullRuneCount(uint8 type, uint8 count);
	void ActivateRuneType(uint8 type);	//regen to max a type
	void UpdateRuneIcon(int8 rune_index);
	void ConvertRuneIcon(uint8 index,uint8 dst_rune_type);
	void UpdateRuneCooldowns();
	void ClearRuneCooldown(uint8 index);

//	uint32	m_custom_flags;

	void	RemoveAndMailItemFromSlot(uint8 slot);
	void	RemoveAndMailItem(Item **it);

	int32	m_Talent_point_mods;			// DK will receive talent points from quests
	int32	m_dmg_made_since_login;			// some spells use the amount of dmg we did in a period to calculate the effect of the spell
	int32	m_heal_made_since_login;		// used mostly fro statistics.
	int32	m_dmg_received_since_login;		// some spells use the amount of dmg we did in a period to calculate the effect of the spell
	int32	m_heal_received_since_login;	// used mostly fro statistics.
	int32	m_dmg_made_last_time;			// needed for spells
	int32	last_received_move_opcode;		// used to see if we can supress move packets
	int8	ignoreShapeShiftChecks;

	SimpleVect<uint32>	ignoreMoveCastInterrupt;
	void				AddSpellIgnoringMoveInterrupt( uint32 SpellNameHash );
	void				RemSpellIgnoringMoveInterrupt( uint32 SpellNameHash );
	bool				IsSpellIgnoringMoveInterrupt( uint32 SpellNameHash );

	// update (A9) related stuff. Making update packets represents 57% of memory allocations on a live server
	uint8	update_buffer[MAX_SEND_BUFFER_SIZE];			//instead of all those allocations. 1000 players mean 65 Mb ram !
	uint8	zlib_update_buffer[MAX_SEND_BUFFER_SIZE];		//instead of all those allocations. 1000 players mean 65 Mb ram !
	z_stream zlib_stream;

	uint8	has_mixology_talent;			//	only affects potions
	uint8	has_alchemist_stone_effect;		//	only affects potions
	uint8	pet_extra_talent_points;		//  hunters have a talent like this

	//these are used by blizz to generate some mob strengths in instances. Ex : The eye : dragonlings health and dmg
	uint32	item_level_sum;					// used in some formulas to be able to compare player strength 
	uint32	item_enchant_level_sum;			// used in some formulas to be able to compare player strength 
	uint32	item_count_sum;					// to to be able to make avg player strength also
	/************************************************************************/
	/* Drunk system                                                         */
	/************************************************************************/
	void SetDrunkValue( uint16 newDrunkValue, uint32 itemid = 0 );
	uint16 GetDrunkValue() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE
		return ( GetUInt32Value(PLAYER_BYTES_3) & 0xFFFE); 
	}
	DrunkenState GetDrunkenstateByValue( uint16 value );
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// void HandleSpellLoot( uint32 itemid )
	//  Generates loot for the spell loot item (clams for example) , then adds the generated loot to the Player
	//
	// Parameters:
	//  uint32 itemid   -  unique numerical identifier of the item the Player is looting
	//
	// Return Value:
	//  None.
	///////////////////////////////////////////////////////////////////////////////
	void HandleSpellLoot( uint32 itemid );
    ////////////////////////////////////////////////////////////////////////////////////////////////
    //  void LearnTalent( uint32 talentid, uint32 rank )
    //    Teaches a talentspell to the Player and decreases the available talent points
    //
    //
    //  Parameters:
    //   uint32 talentid     -   unique numeric identifier of the talent (index of talent.dbc )
    //   uint32 rank         -   rank of the talent
	//   bool isPreviewed	 -   true if called from the preview system
    //
    //  Return Value:
    //   None.
    //  
    ////////////////////////////////////////////////////////////////////////////////////////////////
	SpellEntry *last_casted_enemy_spell;
	int32		LastSpellCost; //this can be anything : mana, energy, holy power, shards...

	uint32 mount_look_override;
//	uint32	mana_burn_target,mana_burn_target_cast;

	uint32 last_mana_regen_stamp;		//cause we need mana regen precision

	std::string only_player_gm;
	int32	XPModBonusPCT;
	int32	HonorModBonusPCT;
//	float	ModAbillityAPBonusCoef;		//the amount of DK abilities will benefit this bonus
	int32	IgnoreSpellSpecialCasterStateRequirement;

	UnitToUnitPossitionAdjuster	SummonPosAdjuster;
//	UnitToUnitPossitionAdjuster	*AttackerPosAdjuster;
	float GetSummonAngle(const uint64 &guid){ return SummonPosAdjuster.GetUnitAngle( guid ); }
	//maybe later remake this with triggers. Right now a lot of glyphs have no common effect so it is not worth implementing triggers
	bool  HasGlyphWithID(uint32 glyphID );
	uint32	m_time_sync_send_counter;
	void	SendTimeSyncRequest();
	SimplePointerList<GOSummon>	PossibleGoSummons;	//this does not mean they are all active. This is the list to avoid spawn spamming
	void SendItemPushResult( bool created, bool recieved, bool sendtoset, bool newitem,  uint8 destbagslot, uint32 destslot, uint32 count, uint32 entry, uint32 suffix, uint32 randomprop, uint32 stack );
	uint32 GetInitialFactionId();
	ARCEMU_INLINE void SendTriggerMovie(uint32 movieId);
	//anti gold bug protection. Gold resets after 214k
	ARCEMU_INLINE uint64 Player::GetGold()
	{ 
		return GetUInt64Value(PLAYER_FIELD_COINAGE); 
	}
	ARCEMU_INLINE void Player::SetGold(uint64 new_val)
	{ 
		SetUInt64Value(PLAYER_FIELD_COINAGE,new_val);
	}
	void ModGold(int64 mod);
	ARCEMU_INLINE bool HasGold( uint64 coins ){	return (GetUInt64Value(PLAYER_FIELD_COINAGE) >= coins);	}

	//lfg system
	std::string						LFG_comment;
	uint32							LFG_roles; //(32 bits!)				//he bends over if you drop the soap
	CommitPointerList<LFG_store>	LFG_dungeons;
	uint32							LFG_joined_queue;		//used to aproximate avg wait time based on roles
	uint32							RB_map_id;				//filter server response list : raid browser map id
	uint32							RB_type;				//filter server response list : 10 man, dungeon ....
	void							LFG_UpdateClientUI(uint8 UpdateType);
	void							LFG_PartyUpdateClientUI(uint8 UpdateType);
	void							LFG_JoinResponse(uint8 checkResult, uint8 checkValue);
	bool							RB_IsLookingFor( uint32 map_id, uint32 type );
	uint32							LFG_GetGearScore( ) { return (item_level_sum+item_enchant_level_sum); }	//might not be what you expect it to be. But better then nothing
//	uint32							LFG_selected_MAP_ID;
//	uint8							LFG_selected_Type;
//	bool							LFG_Autojoin;
//	bool							LFG_AutoAddMem;
//	LfgMatch * m_lfgMatch;
//	uint32 m_lfgInviterGuid;
	void EventTimeoutLfgInviter();

	//equipment manager - another aproach coul dbe to simply attach a guid/index to items ?
	CommitPointerList<EquipmentSet> m_EquipmentSets;
	void							SendEquipmentSetList();
	void							DeleteEquipmentSet( uint64 GUID );
	void							_SaveEquipmentSets(QueryBuffer * buf);
	void							_LoadEquipmentSets(QueryResult * result);

	//gm statistic related stuff
#if GM_STATISTICS_UPDATE_INTERVAL > 0 
	GM_statistics					*m_GM_statistics;
#endif

	//currency handling, These got removed as player fields. Now they are sent probably with a packet
	struct PlayerCurrencyStore
	{
		uint32	type;
		int32	count;
		int32	CountSinceLockReset;		//will need to implement properly later : limit on interval
		uint32	next_unlock_stamp;			//will need to implement properly later : interval expire
	};
	CommitPointerList<PlayerCurrencyStore> m_Currency;
	uint32 GetCurrencyCount( uint32 type );
	uint32 GetCurrencyCanStore( uint32 type );
	void SetCurrencyCount( uint32 type, int32 count ) { ModCurrencyCount(type,count,true); }
	void ModCurrencyCount( uint32 type, int32 count, bool is_assign = false );
	void SendCurrencyStatus();	//send full list at player login
	//profession related 
	bool CanLearnProfession( uint32 skill )
	{
		if( skill && GetUInt32Value( PLAYER_PROFESSION_SKILL_LINE_1 ) == skill )
			return false;
		if( skill && GetUInt32Value( PLAYER_PROFESSION_SKILL_LINE_2 ) == skill )
			return false;
		uint32 prof_count = 0;
		if( GetUInt32Value( PLAYER_PROFESSION_SKILL_LINE_1 ) != 0 )
			prof_count++;
		if( GetUInt32Value( PLAYER_PROFESSION_SKILL_LINE_2 ) != 0 )
			prof_count++;
		return ( prof_count < 2 );
	}
	void UnlearnProfession( uint32 skill )
	{
		if( skill && GetUInt32Value( PLAYER_PROFESSION_SKILL_LINE_1 ) == skill )
			SetUInt32Value( PLAYER_PROFESSION_SKILL_LINE_1 , 0 );
		if( skill && GetUInt32Value( PLAYER_PROFESSION_SKILL_LINE_2 ) == skill )
			SetUInt32Value( PLAYER_PROFESSION_SKILL_LINE_2 , 0 );
	}
	void LearnProfession( uint32 skill )
	{
		//safety first
		if( skill == 0 )
			return;
		if( GetUInt32Value( PLAYER_PROFESSION_SKILL_LINE_1 ) == skill )
			return;
		if( GetUInt32Value( PLAYER_PROFESSION_SKILL_LINE_2 ) == skill )
			return;

		if( GetUInt32Value( PLAYER_PROFESSION_SKILL_LINE_1 ) == 0 )
		{
			SetUInt32Value( PLAYER_PROFESSION_SKILL_LINE_1 , skill );
//			AutoLearnSkillRankSpells( skill, 1 );
		}
		else if( GetUInt32Value( PLAYER_PROFESSION_SKILL_LINE_2 ) == 0 )
		{
			SetUInt32Value( PLAYER_PROFESSION_SKILL_LINE_2 , skill );
//			AutoLearnSkillRankSpells( skill, 1 );
		}
	}
	int32	m_crit_block_chance;
//	int32	m_dmg_pct_penetrate;	//attacker will ignore X% of our armor. default is 0
	int32	m_mod_runic_power_gain_pct;
	float	m_mod_rune_power_gain_coeff[RUNE_TYPE_COUNT];	//by default 2 runes of same type regen in 10 seconds, 2.0 means it will half rune regen time
	int8	m_eclipsebar_state;		//will define what kind of balance energy we can gain
	//research ( archaeology )
	//due to not enough data we initialize dig site once player will cast the "survey" spell
	//if he will cast survey we will find a radom nearby spot and mark it as goal
	//once he loots the GO, we will reinitialize our digsites and he has a chance that he will need to move to some other site
#define MAX_RESEARCH_SITES ((PLAYER_FIELD_RESERACH_SITE_1_7-PLAYER_FIELD_RESERACH_SITE_1+1 ) * 2 )
	struct ResearchDigSiteStatus
	{
		float	x,y,z;			//loot will be found here
		float	last_cast_dist;	//if this increases a lot we will presume he moved to a new site
		uint32	loot_GO_entry;	//we can generate different type of loots, depending on loot type we will have different conatiner looks
		uint32	loot_currency_type;
		uint32	loot_currency_count;	//only found NE :( wich topped to 5
	};
	void	GenerateResearchDigSites();	//on learn,login and after loot survey site. (add or replace existing sites)
	void	GenerateResearchProjects(uint32 max=0xFFFF);	//on learn, loot survey site, (adds new projects)
	void	EventSolveProject(SpellEntry *sp);	//after we solve a project we randomly pick a new one instead of it
	uint32	GetSurveyBotEntry(float &x,float &y,float &z,float &orientation, int32 &duration);	//depending on the distance from our designated crapdump we will swap the Survey entry
	void	LootResearchChest(GameObject *chest);	//this will be called when the player clicks on the research digsite loot chest
	ResearchDigSiteStatus	m_digsite;	//we only memorize one. The one you will try to loot atm
	std::map<uint32,uint32>	m_temp_completed_projects;
#ifdef SOCIAL_BONUSES_ADDON
	uint32	social_next_update_stamp;
	void	EventUpdateSocial();
#endif
	//guild finder begin
	void GuildFinderSendPendingRequests();
	void GuildFinderAddRequest( GuildFinderRequest *r );
	void GuildFinderDelRequest( uint64 guild_guid );
	bool GuildFinderHasPendingRequest( uint64 guild_guid );
	void GuildFinderWipeRequests();
	void GuildFinderCleanupRejectedRequests();
	std::list< GuildFinderRequest *> m_guild_finder_requests;
	//guild finder end

//	float	AP_to_SP_coeff;	
	float	AP_to_HP_coeff;
//	std::map<uint32,uint8>	m_item_querys;	//first query is display and class, second query is for additional info. Just reproducing as is on blizz
	uint32	m_sec_power_regen_stamp;		//atm only holy power is using this, ticks every 30 sec
	uint32	GetItemAvgLevelBlizzlike();		//blizz takes the most powerfull items and calculates level based on that
	WargameStore	*m_Wargames;
	void	WargamesInitialize();			//create store struct. Only a fraction of players will use this feature. No need to waste emmory for everyone

	void ModRating(const uint32 index, const int32 value)
	{
		ModUnsigned32Value( index, value );
		if( index == PLAYER_RATING_MODIFIER_MELEE_HASTE )
			ModAttackSpeed( 0, MOD_MELEE );
		else if( index == PLAYER_RATING_MODIFIER_RANGED_HASTE )
			ModAttackSpeed( 0, MOD_RANGED );
		else if( index == PLAYER_RATING_MODIFIER_SPELL_HASTE )
			ModAttackSpeed( 0, MOD_SPELL );
	}
	float m_attack_speed_mod_noRegen;
	std::map<uint32,uint8>	m_SpellEffectImmunities;
#ifdef BATTLEGRUND_REALM_BUILD
	std::map<uint64,uint32>	BGRealmPVPFightSupporters;
#endif
#ifdef CLEAR_DUEL_COOLDOWNS
	std::set<uint32> m_DuelSpells;
	uint32 PreDuelHP,PreDuelMana;
#endif
	int16	m_manafromspellPCT;
	float	m_mastery;		//from spells and mabe other mods. Will be merged with mastery rating
	int16	m_ExpertiseMod;	// from auras. note this is not rating
	void	ModStatToRating( uint32 Stat, uint32 Rating, int32 ConversionPCT );
	void	UpdateStatToRating( uint32 Stat );
	void	ModStatToSpellPower( uint32 Stat, int32 ConversionPCT );
	void	UpdateStatToSpellPower( uint32 Stat );
	void	ModAttackPowerToSpellPower( int32 ConversionPCT );
	void	UpdateAttackPowerToSpellPower( );
	ARCEMU_INLINE bool	IsSpectator() { return ( ( m_invisFlag & INVIS_FLAG_CUSTOM_SPECTATE ) || ( HasFlag( PLAYER_FLAGS, PLAYER_FLAGS_COMMENTATOR ) ) ); }
	void	SpectatorApply();
	void	SpectatorRemove( bool NoTeleport = false );
	//! Zone id.
	ARCEMU_INLINE const uint32& GetZoneId( ) const { return m_zoneId; }
	ARCEMU_INLINE void SetZoneId(uint32 newZone);
	uint32 m_zoneId;
	void	BackupRestoreOnceRaceGenderStyle(bool restore=false);
};

#endif
