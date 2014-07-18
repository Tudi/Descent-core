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
#ifndef _SPELL_AURAS_H_
#define _SPELL_AURAS_H_
#include <array>

enum AURA_INTERNAL_USAGE_FLAGS
{
	//if all 3 mods are resisted then we can send client as a fully resisted spell.
	//don't change the value of these !
	MOD_0_RESISTED							= 1,
	MOD_1_RESISTED							= 2,
	MOD_2_RESISTED							= 4,
	NEED_STATEIGNORE_RESEND_ON_MAPCHANGE	= 8,	// could be static spell flag
	NEED_RUNETYPE_RESEND_ON_MAPCHANGE		= 16,	// could be static spell flag
	WAS_REMOVED_ON_PLAYER_REQUEST			= 32,	// right now used only by 1 spell for boss scripting
	WAS_REMOVED_ON_SCRIPT_REQUEST			= 64,	// right now used only by 1 spell for boss scripting
	WAS_REMOVED_ON_DISPEL					= 128,	// right now used only by 1 spell for boss scripting
	WAS_MERGED_INTO_OTHER_ON_APPLY			= 256,	// to not spam combat log we merge DOTS and HOTS on APPLY
	WAS_MERGED_INTO_THIS_ON_APPLY			= 512,	// to not spam combat log we merge DOTS and HOTS on APPLY
	MODS_ARE_APPLIED						= 1024,	// debug if values got applied / removed on remove
	MODS_APPLY_WAS_CALLED					= 2048,	// debug if values got applied / removed on remove
	AURAFLAG_AREA_AURA						= 4096,
	AURAFLAG_CASTED_IN_DUEL					= 8192,
	AURAFLAG_CASTED_IN_BG					= 16384,
	AURAFLAG_CANT_BE_DISPELLED				= 32768,
	FIRST_TICK_ENDED						= 0x10000,
};

//needs to be remade. Some of them can be stacked. On remove we need to check stack
enum AURA_STAE_FLAGS
{
	AURASTATE_DODGE_BLOCK			    = 1,   
    AURASTATE_HEALTH_BELOW_20_PCT		= 2,   
    AURASTATE_BERSERK					= 3,   
    AURASTATE_FROZEN					= 4,   
    AURASTATE_JUDGEMENT					= 5,   
    AURASTATE_PARRY						= 6,   
    AURASTATE_LASTKILLWITHHONOR			= 10,  
    AURASTATE_CRITICAL					= 11,  
    AURASTATE_HEALTH_BELOW_35_PCT		= 13,  
    AURASTATE_IMMOLATE					= 14,  
    AURASTATE_REJUVENATE				= 15,  
    AURASTATE_POISON					= 16,  
    AURASTATE_ENRAGE					= 17,  
    AURASTATE_BLEED						= 18,  
    AURASTATE_HYPOTHERMIA               = 19,  
    AURASTATE_HEALTH_ABOVE_75_PCT		= 23,  

/*
	AURASTATE_FLAG_DODGE_BLOCK			= (1 << (AURASTATE_DODGE_BLOCK)),        //1
    AURASTATE_FLAG_HEALTH_BELOW_20_PCT  = (1 << (AURASTATE_HEALTH_BELOW_20_PCT)),        //2
	AURASTATE_FLAG_BERSERK              = (1 << (AURASTATE_BERSERK)),        //3
    AURASTATE_FLAG_FROZEN               = (1 << (AURASTATE_FROZEN)),        //4
    AURASTATE_FLAG_JUDGEMENT            = (1 << (AURASTATE_JUDGEMENT)),       //5
    AURASTATE_FLAG_PARRY                = (1 << (AURASTATE_PARRY)),       //7
    AURASTATE_FLAG_LASTKILLWITHHONOR    = (1 << (AURASTATE_LASTKILLWITHHONOR-1)),      //10
    AURASTATE_FLAG_CRITICAL             = (1 << (AURASTATE_CRITICAL-1)),     //11
    AURASTATE_FLAG_HEALTH_BELOW_35_PCT  = (1 << (AURASTATE_HEALTH_BELOW_35_PCT)),     //13
    AURASTATE_FLAG_IMMOLATE             = (1 << (AURASTATE_IMMOLATE)),     //14
    AURASTATE_FLAG_REJUVENATE           = (1 << (AURASTATE_REJUVENATE)),    //15 //where do i use this ?
    AURASTATE_FLAG_POISON               = (1 << (AURASTATE_POISON)),    //16
    AURASTATE_FLAG_ENRAGE               = (1 << (AURASTATE_ENRAGE)),    //17
    AURASTATE_FLAG_BLEED                = (1 << (AURASTATE_BLEED)),   //18
    AURASTATE_FLAG_HYPOTHERMIA          = (1 << (AURASTATE_HYPOTHERMIA)),   //19
    AURASTATE_FLAG_HEALTH_ABOVE_75_PCT	= (1 << (AURASTATE_HEALTH_ABOVE_75_PCT)),  //23
	*/
};

//!!there is a duplicate in unit.h
#define	AURASTATE_MAX_INDEX				24		//at least this is the max we know of

enum MOD_TYPES
{
    SPELL_AURA_NONE = 0,                                // None
    SPELL_AURA_BIND_SIGHT = 1,                          // Bind Sight
    SPELL_AURA_MOD_POSSESS = 2,                         // Mod Possess
    SPELL_AURA_PERIODIC_DAMAGE = 3,                     // Periodic Damage
    SPELL_AURA_DUMMY = 4,                               // Script Aura
    SPELL_AURA_MOD_CONFUSE = 5,                         // Mod Confuse
    SPELL_AURA_MOD_CHARM = 6,                           // Mod Charm
    SPELL_AURA_MOD_FEAR = 7,                            // Mod Fear
    SPELL_AURA_PERIODIC_HEAL = 8,                       // Periodic Heal
    SPELL_AURA_MOD_ATTACKSPEED = 9,                     // Mod Attack Speed
    SPELL_AURA_MOD_THREAT = 10,                         // Mod Threat
    SPELL_AURA_MOD_TAUNT = 11,                          // Taunt
    SPELL_AURA_MOD_STUN = 12,                           // Stun
    SPELL_AURA_MOD_DAMAGE_DONE = 13,                    // Mod Damage Done
    SPELL_AURA_MOD_DAMAGE_TAKEN = 14,                   // Mod Damage Taken
    SPELL_AURA_DAMAGE_SHIELD = 15,                      // Damage Shield
    SPELL_AURA_MOD_STEALTH = 16,                        // Mod Stealth
    SPELL_AURA_MOD_DETECT = 17,                         // Mod Detect
    SPELL_AURA_MOD_INVISIBILITY = 18,                   // Mod Invisibility
    SPELL_AURA_MOD_INVISIBILITY_DETECTION = 19,         // Mod Invisibility Detection
    SPELL_AURA_MOD_TOTAL_HEALTH_REGEN_PCT = 20,
    SPELL_AURA_MOD_TOTAL_MANA_REGEN_PCT = 21,
    SPELL_AURA_MOD_RESISTANCE = 22,                     // Mod Resistance
    SPELL_AURA_PERIODIC_TRIGGER_SPELL = 23,             // Periodic Trigger
    SPELL_AURA_PERIODIC_ENERGIZE = 24,                  // Periodic Energize
    SPELL_AURA_MOD_PACIFY = 25,                         // Pacify
    SPELL_AURA_MOD_ROOT = 26,                           // Root
    SPELL_AURA_MOD_SILENCE = 27,                        // Silence
    SPELL_AURA_REFLECT_SPELLS = 28,                     // Reflect Spells %
    SPELL_AURA_MOD_STAT = 29,                           // Mod Stat
    SPELL_AURA_MOD_SKILL = 30,                          // Mod Skill
    SPELL_AURA_MOD_INCREASE_SPEED = 31,                 // Mod Speed
    SPELL_AURA_MOD_INCREASE_MOUNTED_SPEED = 32,         // Mod Speed Mounted
    SPELL_AURA_MOD_DECREASE_SPEED = 33,                 // Mod Speed Slow
    SPELL_AURA_MOD_INCREASE_HEALTH = 34,                // Mod Increase Health
    SPELL_AURA_MOD_INCREASE_ENERGY = 35,                // Mod Increase Energy
    SPELL_AURA_MOD_SHAPESHIFT = 36,                     // Shapeshift
    SPELL_AURA_EFFECT_IMMUNITY = 37,                    // Immune Effect
    SPELL_AURA_STATE_IMMUNITY = 38,                     // Immune State
    SPELL_AURA_SCHOOL_IMMUNITY = 39,                    // Immune School    
    SPELL_AURA_DAMAGE_IMMUNITY = 40,                    // Immune Damage
    SPELL_AURA_DISPEL_IMMUNITY = 41,                    // Immune Dispel Type
    SPELL_AURA_PROC_TRIGGER_SPELL = 42,                 // Proc Trigger Spell
    SPELL_AURA_PROC_TRIGGER_DAMAGE = 43,                // Proc Trigger Damage
    SPELL_AURA_TRACK_CREATURES = 44,                    // Track Creatures
    SPELL_AURA_TRACK_RESOURCES = 45,                    // Track Resources
    SPELL_AURA_MOD_PARRY_SKILL = 46,                    // Mod Parry Skill
    SPELL_AURA_MOD_PARRY_PERCENT = 47,                  // Mod Parry Percent
    SPELL_AURA_MOD_DODGE_SKILL = 48,                    // Mod Dodge Skill
    SPELL_AURA_MOD_DODGE_PERCENT = 49,                  // Mod Dodge Percent  
    SPELL_AURA_MOD_CRIT_HEAL_DONE_PCT = 50,             // Mod Crit Heal Done
    SPELL_AURA_MOD_BLOCK_PERCENT = 51,                  // Mod Block Percent
    SPELL_AURA_MOD_CRIT_PERCENT = 52,                   // Mod Crit Percent
    SPELL_AURA_PERIODIC_LEECH = 53,                     // Periodic Leech
    SPELL_AURA_MOD_HIT_CHANCE = 54,                     // Mod Hit Chance
    SPELL_AURA_MOD_SPELL_HIT_CHANCE = 55,               // Mod Spell Hit Chance
    SPELL_AURA_TRANSFORM = 56,                          // Transform
    SPELL_AURA_MOD_SPELL_CRIT_CHANCE = 57,              // Mod Spell Crit Chance
    SPELL_AURA_MOD_INCREASE_SWIM_SPEED = 58,            // Mod Speed Swim
    SPELL_AURA_MOD_DAMAGE_DONE_CREATURE = 59,           // Mod Creature Dmg Done   
    SPELL_AURA_MOD_PACIFY_SILENCE = 60,                 // Pacify & Silence
    SPELL_AURA_MOD_SCALE = 61,                          // Mod Scale
    SPELL_AURA_PERIODIC_HEALTH_FUNNEL = 62,             // Periodic Health Funnel
    SPELL_AURA_PERIODIC_MANA_FUNNEL = 63,               // Periodic Mana Funnel
    SPELL_AURA_PERIODIC_MANA_LEECH = 64,                // Periodic Mana Leech
    SPELL_AURA_MOD_CASTING_SPEED = 65,                  // Haste - Spells
    SPELL_AURA_FEIGN_DEATH = 66,                        // Feign Death
    SPELL_AURA_MOD_DISARM = 67,                         // Disarm
    SPELL_AURA_MOD_STALKED = 68,                        // Mod Stalked
    SPELL_AURA_SCHOOL_ABSORB = 69,                      // School Absorb    
    SPELL_AURA_EXTRA_ATTACKS = 70,                      // Extra Attacks
    SPELL_AURA_MOD_SPELL_CRIT_CHANCE_SCHOOL = 71,       // Mod School Spell Crit Chance
    SPELL_AURA_MOD_POWER_COST = 72,                     // Mod Power Cost
    SPELL_AURA_MOD_POWER_COST_SCHOOL = 73,              // Mod School Power Cost
    SPELL_AURA_REFLECT_SPELLS_SCHOOL = 74,              // Reflect School Spells %
    SPELL_AURA_MOD_LANGUAGE = 75,                       // Mod Language
    SPELL_AURA_FAR_SIGHT = 76,                          // Far Sight
    SPELL_AURA_MECHANIC_IMMUNITY = 77,                  // Immune Mechanic
    SPELL_AURA_MOUNTED = 78,                            // Mounted
    SPELL_AURA_MOD_DAMAGE_PERCENT_DONE = 79,            // Mod Dmg %   
    SPELL_AURA_MOD_PERCENT_STAT = 80,                   // Mod Stat %
    SPELL_AURA_SPLIT_DAMAGE = 81,                       // Split Damage
    SPELL_AURA_WATER_BREATHING = 82,                    // Water Breathing
    SPELL_AURA_MOD_BASE_RESISTANCE = 83,                // Mod Base Resistance
    SPELL_AURA_MOD_REGEN = 84,                          // Mod Health Regen
    SPELL_AURA_MOD_POWER_REGEN = 85,                    // Mod Power Regen
    SPELL_AURA_CHANNEL_DEATH_ITEM = 86,                 // Create Death Item
    SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN = 87,           // Mod Dmg % Taken
    SPELL_AURA_MOD_PERCENT_REGEN = 88,                  // Mod Health Regen Percent
    SPELL_AURA_PERIODIC_DAMAGE_PERCENT = 89,            // Periodic Damage Percent   
    SPELL_AURA_MOD_RESIST_CHANCE = 90,                  // Mod Resist Chance
    SPELL_AURA_MOD_DETECT_RANGE = 91,                   // Mod Detect Range
    SPELL_AURA_PREVENTS_FLEEING = 92,                   // Prevent Fleeing
    SPELL_AURA_MOD_UNATTACKABLE = 93,                   // Mod Uninteractible
    SPELL_AURA_INTERRUPT_REGEN = 94,                    // Interrupt Regen
    SPELL_AURA_GHOST = 95,                              // Ghost
    SPELL_AURA_SPELL_MAGNET = 96,                       // Spell Magnet
    SPELL_AURA_MANA_SHIELD = 97,                        // Mana Shield
    SPELL_AURA_MOD_SKILL_TALENT = 98,                   // Mod Skill Talent
    SPELL_AURA_MOD_ATTACK_POWER = 99,                   // Mod Attack Power
    SPELL_AURA_AURAS_VISIBLE = 100,                     // Auras Visible
    SPELL_AURA_MOD_RESISTANCE_PCT = 101,                // Mod Resistance %
    SPELL_AURA_MOD_CREATURE_ATTACK_POWER = 102,         // Mod Creature Attack Power
    SPELL_AURA_MOD_TOTAL_THREAT = 103,                  // Mod Total Threat (Fade)
    SPELL_AURA_WATER_WALK = 104,                        // Water Walk
    SPELL_AURA_FEATHER_FALL = 105,                      // Feather Fall
    SPELL_AURA_HOVER = 106,                             // Hover
    SPELL_AURA_ADD_FLAT_MODIFIER = 107,                 // Add Flat Modifier
    SPELL_AURA_ADD_PCT_MODIFIER = 108,                  // Add % Modifier
    SPELL_AURA_ADD_TARGET_TRIGGER = 109,                // Add Class Target Trigger    
    SPELL_AURA_MOD_POWER_REGEN_PERCENT = 110,           // Mod Power Regen %
    SPELL_AURA_ADD_CASTER_HIT_TRIGGER = 111,            // Add Class Caster Hit Trigger
    SPELL_AURA_OVERRIDE_CLASS_SCRIPTS = 112,            // Override Class Scripts
    SPELL_AURA_MOD_RANGED_DAMAGE_TAKEN = 113,           // Mod Ranged Dmg Taken
    SPELL_AURA_MOD_RANGED_DAMAGE_TAKEN_PCT = 114,       // Mod Ranged % Dmg Taken
    SPELL_AURA_MOD_HEALING = 115,                       // Mod Healing
    SPELL_AURA_IGNORE_REGEN_INTERRUPT = 116,            // Regen During Combat
    SPELL_AURA_MOD_MECHANIC_RESISTANCE = 117,           // Mod Mechanic Resistance
    SPELL_AURA_MOD_HEALING_PCT = 118,                   // Mod Healing %
    SPELL_AURA_SHARE_PET_TRACKING = 119,                // Share Pet Tracking    
    SPELL_AURA_UNTRACKABLE = 120,                       // Untrackable
    SPELL_AURA_EMPATHY = 121,                           // Empathy (Lore, whatever)
    SPELL_AURA_MOD_OFFHAND_DAMAGE_PCT = 122,            // Mod Offhand Dmg %
    SPELL_AURA_MOD_POWER_COST_PCT = 123,                // Mod Power Cost % --> armor penetration & spell penetration
    SPELL_AURA_MOD_RANGED_ATTACK_POWER = 124,           // Mod Ranged Attack Power
    SPELL_AURA_MOD_MELEE_DAMAGE_TAKEN = 125,            // Mod Melee Dmg Taken
    SPELL_AURA_MOD_MELEE_DAMAGE_TAKEN_PCT = 126,        // Mod Melee % Dmg Taken
    SPELL_AURA_RANGED_ATTACK_POWER_ATTACKER_BONUS = 127,// Rngd Atk Pwr Attckr Bonus
    SPELL_AURA_MOD_POSSESS_PET = 128,                   // Mod Possess Pet
    SPELL_AURA_MOD_INCREASE_SPEED_ALWAYS = 129,         // Mod Speed Always   
    SPELL_AURA_MOD_MOUNTED_SPEED_ALWAYS = 130,          // Mod Mounted Speed Always
    SPELL_AURA_MOD_CREATURE_RANGED_ATTACK_POWER = 131,  // Mod Creature Ranged Attack Power
    SPELL_AURA_MOD_INCREASE_ENERGY_PERCENT = 132,       // Mod Increase Energy %
    SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT = 133,       // Mod Max Health %
    SPELL_AURA_MOD_MANA_REGEN_INTERRUPT = 134,          // Mod Interrupted Mana Regen
    SPELL_AURA_MOD_HEALING_DONE = 135,                  // Mod Healing Done
    SPELL_AURA_MOD_HEALING_DONE_PERCENT = 136,          // Mod Healing Done %
    SPELL_AURA_MOD_TOTAL_STAT_PERCENTAGE = 137,         // Mod Total Stat %
    SPELL_AURA_MOD_HASTE = 138,                         // Haste - Melee
    SPELL_AURA_FORCE_REACTION = 139,                    // Force Reaction    
    SPELL_AURA_MOD_RANGED_HASTE = 140,                  // Haste - Ranged
    SPELL_AURA_MOD_RANGED_AMMO_HASTE = 141,             // Haste - Ranged (Ammo Only)
    SPELL_AURA_MOD_BASE_RESISTANCE_PCT = 142,           // Mod Base Resistance %
    SPELL_AURA_MOD_RESISTANCE_EXCLUSIVE = 143,          // Mod Resistance Exclusive
    SPELL_AURA_SAFE_FALL = 144,                         // Safe Fall
    SPELL_AURA_CHARISMA = 145,                          // Charisma
    SPELL_AURA_PERSUADED = 146,                         // Persuaded
    SPELL_AURA_ADD_CONTROL_IMMUNITY = 147,              // Add Crowd Control Immunity
    SPELL_AURA_RETAIN_COMBO_POINTS = 148,               // Retain Combo Points
    SPELL_AURA_RESIST_PUSHBACK = 149,                   // Resist Pushback
    SPELL_AURA_MOD_SHIELD_BLOCK_PCT = 150,              // Mod Shield Block %
    SPELL_AURA_TRACK_STEALTHED = 151,                   // Track Stealthed
    SPELL_AURA_MOD_DETECTED_RANGE = 152,                // Mod Detected Range
    SPELL_AURA_SPLIT_DAMAGE_FLAT = 153,                 // Split Damage Flat
    SPELL_AURA_MOD_STEALTH_LEVEL = 154,                 // Stealth Level Modifier
    SPELL_AURA_MOD_WATER_BREATHING = 155,               // Mod Water Breathing
    SPELL_AURA_MOD_REPUTATION_ADJUST = 156,             // Mod Reputation Gain
    SPELL_AURA_PET_DAMAGE_MULTI = 157,                  // Mod Pet Damage
    SPELL_AURA_MOD_SHIELD_BLOCK = 158,                  // Mod Shield Block
    SPELL_AURA_NO_PVP_CREDIT = 159,                     // No PVP Credit 
    SPELL_AURA_MOD_SIDE_REAR_PDAE_DAMAGE_TAKEN = 160,   // Mod Side/Rear PBAE Damage Taken 
    SPELL_AURA_MOD_HEALTH_REGEN_IN_COMBAT = 161,        // Mod Health Regen In Combat
    SPELL_AURA_POWER_BURN = 162,                        // Power Burn 
    SPELL_AURA_MOD_CRIT_DAMAGE_BONUS_MELEE = 163,       // Mod Critical Damage Bonus (Physical)
    SPELL_AURA_MELEE_ATTACK_POWER_ATTACKER_BONUS = 165, // Melee AP Attacker Bonus
    SPELL_AURA_MOD_ATTACK_POWER_PCT = 166,              // Mod Attack Power
    SPELL_AURA_MOD_RANGED_ATTACK_POWER_PCT = 167,       // Mod Ranged Attack Power %
    SPELL_AURA_INCREASE_DAMAGE = 168,                   // Increase Damage Type
    SPELL_AURA_INCREASE_CRITICAL = 169,                 // Increase Critical Type
    SPELL_AURA_DETECT_AMORE = 170,                      // Detect Amore
    SPELL_AURA_INCREASE_MOVEMENT_AND_MOUNTED_SPEED = 172,// Increase Movement and Mounted Speed (Non-Stacking)
    SPELL_AURA_INCREASE_SPELL_DAMAGE_FROM_STAT_PCT = 174,         // Increase Spell Damage by % status
    SPELL_AURA_INCREASE_SPELL_HEALING_ATTRIBUTE = 175,        // Increase Spell Healing by % status
    SPELL_AURA_SPIRIT_OF_REDEMPTION = 176,              // Spirit of Redemption Auras
    SPELL_AURA_AREA_CHARM = 177,                        // Area Charm 
    SPELL_AURA_INCREASE_ATTACKER_SPELL_CRIT = 179,      // Increase Attacker Spell Crit Type
    SPELL_AURA_INCREASE_SPELL_DAMAGE_VS_TYPE = 180,     // Increase Spell Damage Type
    SPELL_AURA_INCREASE_ARMOR_BASED_ON_INTELLECT_PCT = 182, // Increase Armor based on Intellect
    SPELL_AURA_DECREASE_CRIT_THREAT = 183,              // Decrease Critical Threat by
    SPELL_AURA_DECREASE_ATTACKER_CHANCE_TO_HIT_MELEE = 184,//Reduces Attacker Chance to Hit with Melee
    SPELL_AURA_DECREASE_ATTACKER_CHANGE_TO_HIT_RANGED = 185,// Reduces Attacker Chance to Hit with Ranged 
    SPELL_AURA_DECREASE_ATTACKER_CHANGE_TO_HIT_SPELLS = 186,// Reduces Attacker Chance to Hit with Spells
    SPELL_AURA_DECREASE_ATTACKER_CHANGE_TO_CRIT_MELEE = 187,// Reduces Attacker Chance to Crit with Melee (Ranged?)
    SPELL_AURA_DECREASE_ATTACKER_CHANGE_TO_CRIT_RANGED = 188,// Reduces Attacker Chance to Crit with Ranged (Melee?)
    SPELL_AURA_INCREASE_REPUTATION = 190,               // Increases reputation from killed creatures
    SPELL_AURA_SPEED_LIMIT = 191,                       // speed limit
    SPELL_AURA_MELEE_SLOW_PCT = 192,
    SPELL_AURA_INCREASE_TIME_BETWEEN_ATTACKS = 193,
    SPELL_AURA_INREASE_SPELL_DAMAGE_PCT_OF_INTELLECT = 194,  // NOT USED ANYMORE - 174 used instead
    SPELL_AURA_MOD_IGNORE_ABSORB_FOR_SPELL = 195,		
    SPELL_AURA_MOD_ALL_WEAPON_SKILLS = 196,
    SPELL_AURA_REDUCE_ATTACKER_CRICTICAL_HIT_CHANCE_PCT = 197,
    SPELL_AURA_INCREASE_SPELL_HIT_PCT = 199,
    SPELL_AURA_MOD_XP_AND_REP_PCT = 200,
    SPELL_AURA_CANNOT_BE_DODGED = 201,
	SPELL_AURA_FINISHING_MOVES_CANNOT_BE_DODGED = 202,
    SPELL_AURA_REDUCE_ATTACKER_CRICTICAL_HIT_DAMAGE_MELEE_PCT = 203,
    SPELL_AURA_REDUCE_ATTACKER_CRICTICAL_HIT_DAMAGE_RANGED_PCT = 204,
    SPELL_AURA_ENABLE_FLIGHT1 = 206,
    SPELL_AURA_ENABLE_FLIGHT2 = 207,
    SPELL_AURA_ENABLE_FLIGHT_NO_SPEED = 208,
    SPELL_AURA_MOD_MOUNTED_SPEED_PCT = 209,
    SPELL_AURA_MOD_FLIGHT_SPEED_PCT = 210,
    SPELL_AURA_INCREASE_RANGED_ATTACK_POWER_PCT_OF_INTELLECT = 212,
    SPELL_AURA_INCREASE_RAGE_FROM_DAMAGE_DEALT_PCT = 213,
    SPELL_AURA_INCREASE_CASTING_TIME_PCT = 216,
    SPELL_AURA_REGEN_MANA_STAT_PCT=219,
    SPELL_AURA_RATING_FROM_STAT=220,
    SPELL_AURA_DRINK_NEW=226,
    SPELL_AURA_PERIODIC_TRIGGER_SPELL_WITH_VALUE=227,
    SPELL_AURA_MOD_AOE_DMG_TAKEN_PCT=229,
    SPELL_AURA_INCREASE_MAX_HEALTH=230,//Used by Commanding Shout
	SPELL_AURA_PROC_TRIGGER_SPELL2=231,
	SPELL_AURA_MOD_MECHANIC_DURATION_PCT=232,
	SPELL_AURA_MOD_REDUCE_MECHANIC_DURATION=234,
    SPELL_AURA_INCREASE_HEALING_POWER_FROM_ATTACKPOWER_PCT=237,
    SPELL_AURA_INCREASE_SPELL_POWER_FROM_ATTACKPOWER_PCT=238,
	SPELL_AURA_MODIFY_AXE_SKILL=240,
	SPELL_AURA_FORCE_MOVE_FORWARD = 241,
    SPELL_AURA_MOD_SPELL_DAMAGE_FROM_HEALING = 242,
    SPELL_AURA_MOD_FACTION = 243,
    SPELL_AURA_COMPREHEND_LANGUAGE = 244,
    SPELL_AURA_MOD_DURATION_OF_MAGIC_EFFECTS = 245,
    SPELL_AURA_MOD_AURA_DURATION_DISPELTYPE = 246,
    APELL_AURA_CLONE_CASTER = 247,
    SPELL_AURA_MOD_COMBAT_RESULT_CHANCE = 248,
    SPELL_AURA_CONVERT_RUNE = 249,
    SPELL_AURA_MOD_INCREASE_HEALTH_2 = 250,
    SPELL_AURA_MOD_ENEMY_DODGE = 251,
    SPELL_AURA_MOD_ATTACK_AND_CAST_SPEED_PCT = 252,
    SPELL_AURA_MOD_CRIT_BLOCK_CHANCE_PCT = 253,
	SPELL_AURA_MOD_DISARM_OFFHAND = 254,
    SPELL_AURA_MOD_DMG_TAKEN_MECHANIC_PCT = 255,
	SPELL_AURA_MOD_REAGENT_COST2 = 256,
	SPELL_AURA_SCREEN_EFFECT = 260,
	SPELL_AURA_PHASE = 261,
	SPELL_AURA_REMOVE_CAST_CONDITION = 262,
	SPELL_AURA_MOD_NOCAST = 263,
	SPELL_AURA_MOD_CANCAST_SCRIPT_ADD = 264,
	SPELL_AURA_MOD_DEBUFF_IMMUNITY = 267,
	SPELL_AURA_MOD_AP_BY_STAT = 268,
	SPELL_AURA_MOD_DAMAGE_TAKEN_PCT = 271,
    SPELL_AURA_MOD_IGNORE_SHAPESHIFT = 275,
	SPELL_AURA_MOD_DISARM_RANGED = 278,
    SPELL_AURA_MOD_IGNOREARMOR_FOR_WEAPON = 280,
    SPELL_AURA_MOD_HONOR_GAIN_PCT = 281,
    SPELL_AURA_MOD_HEALTHPCT = 282,
    SPELL_AURA_MOD_ATTACK_POWER_BY_ARMOR = 285,
    SPELL_AURA_MOD_SPELL_DEFLECT = 285,
    SPELL_AURA_MOD_DOT_CRITCHANCE = 286,
    SPELL_AURA_MOD_ACTION_HIT_CHANCE = 287,
    SPELL_AURA_MOD_CRIT_CHANCE_ALL = 290,
    SPELL_AURA_MOD_MIRROR_DMG = 300,
    SPELL_AURA_MOD_HEAL_ABSORB = 301,
    SPELL_AURA_MOD_CASTER_CRIT_CHANCE_ON_TARGET_OF_SPELLS = 308,	
    SPELL_AURA_MOD_SPELL_POWER_PCT = 317,
    SPELL_AURA_MOD_MASTERY = 318,
    SPELL_AURA_MOD_HASTE_NOREGEN = 319,
    SPELL_AURA_MOD_RANGED_TARGETTING = 322,		//NOT SURE ! only 2 examples of spells
    SPELL_AURA_MOD_RUNIC_POWER_GAIN_PCT = 330,
    SPELL_AURA_MOD_IGNORE_INTERRUPT = 330,
    SPELL_AURA_MOD_REPLACE_SPELL = 332,
    SPELL_AURA_MOD_ADD_TEMPORARY_SPELL = 333,
    SPELL_AURA_MOD_AUTO_ATTACK_DMG_PCT = 344,
    SPELL_AURA_MOD_DMG_PENETRATE = 345,
    SPELL_AURA_MOD_AP_TO_SP_EXCLUSIVE = 366,	//convert x% attack power to spell power. Only used by Mental Quickness
    TOTAL_SPELL_AURAS = 367
};

enum AuraTickFlags
{
    FLAG_PERIODIC_DAMAGE            = 2,
    FLAG_PERIODIC_TRIGGER_SPELL     = 4,
    FLAG_PERIODIC_HEAL              = 8,
    FLAG_PERIODIC_LEECH             = 16,
    FLAG_PERIODIC_ENERGIZE          = 32,
};

enum DisarmTypes
{
    DISARM_TYPE_FLAG_MAINHAND     = 1,
    DISARM_TYPE_FLAG_OFFHAND      = 2,
    DISARM_TYPE_FLAG_RANGED       = 4,
};

struct Modifier
{
    uint32	m_AuraName;			// Aura type -> ex mod_hp
    int32	m_amount;			// By how much does it mod?
	int32	m_pct_mod;			// dmg can be scaled by PCT value and flat value
    int32	m_miscValue;		// Misc Value -> same as getspellproto()->miscvalue[i]
    uint32	i;					// aura belongs from spell effect i
	uint32	list_ind;			// since spell can have max 3 auras min 0 we will not store this aura in same slot as i
//	uint32	total_mods;			// represents count of mods added by this spell. Only proc PROC_ON_AURA_REMOVE event if this is last

    //need this to store % values or they cannot be reverted corectly (i think :D )
    int32	fixed_amount[3];	//
	void	*extension;
};


struct ProcTriggerSpellOnSpell
{
    uint32 origId;
    uint32 spellId;
    uint64 caster;
    uint32 procChance;
    uint32 procFlags;
    uint32 RemainingCharges;
    uint32 LastTrigger;
    void  *owner;//mark the owner of this proc to know which one to delete
};

struct DamageProc
{
    uint32 m_spellId;
    uint32 m_damage;
  //  uint64 m_caster;//log is: some reflects x arcane/nature damage to 'attacker' no matter who casted
    uint32 m_school;
    uint32 m_flags;
    void  *owner;//mark the owner of this proc to know which one to delete
};

struct DamageSplitTarget
{
	uint64 m_target; // we store them
	uint32 m_spellId;
	float m_pctDamageSplit; // % of taken damage to transfer (i.e. Soul Link)
	uint32 m_flatDamageSplit; // flat damage to transfer (i.e. Blessing of Sacrifice)
//	uint8 damage_type; // bitwise 0-127 thingy
	void * creator;
};

struct ProcTriggerSpell
{
	ProcTriggerSpell( SpellEntry *sp, void *p_owner ) 
	{ 
		LastTrigger = 0; 
		deleted = false; 
		origId = sp->Id;
		spellId = sp->Id;
		procFlags = sp->procFlags;
		procChance = MAX( 1, sp->procChance );
/*		SpellAuraOptionsEntry *sao = dbcSpellAuraOptions.LookupEntryForced( sp->SpellAuraOptionsId );
		if( sao )
		{
			procChance = sao->procChance;
			procFlags = sao->procFlags;
		}
		else
			sLog.outDebug("Could not determine procchance and procflags for spell %u",sp->Id );
		/**/
		procFlags2 = sp->procFlags2;
		procInterval = sp->proc_interval;
		created_with_value = 0;
		custom_holder = NULL;
		owner = p_owner;
		procSuccessCounter = procEventCounter = 1;
	}
	//  ProcTriggerSpell() : origId(0), trigger(0), spellId(0), caster(0), procChance(0), procFlags(0), procCharges(0) { }
	uint32 origId;
	// uint32 trigger;
	uint32 spellId;
	uint64 caster;
	uint32 procChance;
	uint32 procSuccessCounter,procEventCounter; // Every time we proc, this gets reseted to 0. Every time we fail to proc this gets increased
	uint32 procInterval;	//needed for ppm. Some change due to talents !
	uint32 procFlags;
	uint32 procFlags2;		//custom extension
	uint32 procCharges;
	//    SpellEntry *ospinfo;
	//    SpellEntry *spinfo;
	uint32 LastTrigger;
	int64 created_with_value;	//contain the amount the spell used when we casted it 
	void  *custom_holder;
	void  *owner; //so each aura will remove it's proc instead some other proc
//	uint32 ProcType; //0=triggerspell/1=triggerclassspell
	bool deleted;
};

struct SpellCharge
{
	SpellCharge() 
	{
//		created_at_stamp = getMSTime() + 200; //should provide 1 update cycle safety
		GotCreatedInThisLoop = true;
		SkipFirstEvents = 0;
	}
//    uint32		spellId;
	SpellEntry	*spe;
    int32		count;
    uint32		ProcFlag;	//this helps data locality = improves speed when iterating through list
	//this approach is not good because players are using macroes to chain cast spells and exploit multi usage of for example "clearcasting" "soulburn"
//	uint32		created_at_stamp;	//somehow forbid spells to proc and get removed on same cycles. Not sure what bugs this will cause. But it should fix quite a few
	bool		GotCreatedInThisLoop;
	int8		SkipFirstEvents;	//ex : mod next cast time, but we are already casting the spell
};

typedef set<uint64> AreaAuraList;

#define MAX_AURA_MOD_COUNT 3

class SERVER_DECL Aura : public EventableObject
{
public:
    Aura( );
	void Init( SpellEntry *proto, int32 duration,Object* caster, Unit *target, Item* i_caster = NULL );
	void Virtual_Constructor();		//when using object pool contructor is not good to be called again sometimes. Use this instead
	~Aura();
	void Virtual_Destructor();		//this makes sure we do not leave events on objects that are supposed to be deleted
	int32 m_bufferPoolId;
#ifdef BUFFER_POOL_REMEMBER_HISTORY
	int	  m_bufferPoolPrevAllocLine;
	char  *m_bufferPoolPrevAllocFILE;
	int	  m_bufferPoolPrevDeAllocLine;
	char  *m_bufferPoolPrevDeAllocFILE;
#endif

    void Remove();
    void RemoveDelayed();	//used for scripts to not kill the object before control is given back to core

    void AddMod(uint32 t, int32 a,uint32 miscValue,uint32 i, int32 pct_mod);

    ARCEMU_INLINE SpellEntry* GetSpellProto() const { return m_spellProto; }
    ARCEMU_INLINE uint32 GetSpellId() const {return m_spellProto->Id; }
    ARCEMU_INLINE bool IsPassive()
	{ 
		if(!m_spellProto) 
			return false; 
		return ( (m_spellProto->Attributes & ATTRIBUTES_PASSIVE) && IsAreaAura() == false );
	}
    ARCEMU_INLINE bool IsVisible()
	{ 
		if(!m_spellProto) 
			return false; 		
		return ((IsPassive() == false || (m_spellProto->AttributesEx & ATTRIBUTESEX_VISIBLE)) && ( m_spellProto->SpellIconID != 1 || m_spellProto->activeIconID != 0 ));
	}

    ARCEMU_INLINE int32 GetDuration() const { return m_duration; }
    void SetDuration(int32 duration) { m_duration = duration; }
	void ResetDuration(uint32 InheritedDuration);	//do all the stuff needed to reset duration, client side also
	void ResetDuration();	//do all the stuff needed to reset duration, client side also

//  ARCEMU_INLINE uint16 GetAuraSlot() const { return m_auraSlot; }
//	void SetAuraSlot( uint16 slot ) { m_auraSlot = slot; }

	ARCEMU_INLINE bool IsPositive() 
	{ 
		if( m_positive == 0 )
			return !IsDamagingSpell(m_spellProto); //this hardly covers negative buffs. It is better then nothing though
		return m_positive>0; 
	}

	void SetNegative(signed char value=1) 
	{ 
		m_positive -= value; 
	}
	void SetPositive(signed char value=1) 
	{ 
		m_positive += value; 
	}

	Object* GetCaster();
	ARCEMU_INLINE uint64 GetCasterGUID(){return m_casterGuid;}
	Unit* GetUnitCaster();
	ARCEMU_INLINE Unit* GetTarget() { return m_target; }

	Aura* StrongerThat(Aura *aur);
	void ApplyModifiers(bool apply);
	void UpdateModifiers();
//	void AddAuraVisual();
//	void RemoveAuraVisual();
	void EventUpdateAA(float r);
	void RemoveAA();
		
	ARCEMU_INLINE uint32 GetTimeLeft()//in sec
	{
		if(m_duration==-1)return (uint32)-1;
		int32 n=int32((UNIXTIME-time_t(timeleft))*1000);
		if(n>=m_duration) return 0;
		return (m_duration-n);
	}
	ARCEMU_INLINE uint32 GetTimePassed()
	{
		return (UNIXTIME-time_t(timeleft))*1000;
	}
	ARCEMU_INLINE void ResetTimeLeft()
	{
		timeleft = ( uint32 )UNIXTIME;
	}

		// Aura Handlers
	void SpellAuraNULL(bool apply);
	void SpellAuraBindSight(bool apply);
	void SpellAuraModPossess(bool apply);
	void SpellAuraPeriodicDamage(bool apply);
	void SpellAuraDummy(bool apply);
	void SpellAuraModConfuse(bool apply);
	void SpellAuraModCharm(bool apply);
	void SpellAuraModFear(bool apply);
	void SpellAuraPeriodicHeal(bool apply);
	void SpellAuraModAttackSpeed(bool apply);
	void SpellAuraModThreatGeneratedSchoolPCT(bool apply);
	void SpellAuraModTaunt(bool apply);
	void SpellAuraModStun(bool apply);
	void SpellAuraModDamageDone(bool apply);
	void SpellAuraModDamageTaken(bool apply);
	void SpellAuraDamageShield(bool apply);
	void SpellAuraModStealth(bool apply);
	void SpellAuraModDetect(bool apply);
	void SpellAuraModInvisibility(bool apply);
	void SpellAuraModInvisibilityDetection(bool apply);
	void SpellAuraModTotalHealthRegenPct(bool apply);
	void SpellAuraModTotalManaRegenPct(bool apply);
	void SpellAuraModResistance(bool apply);
	void SpellAuraPeriodicTriggerSpell(bool apply);
	void SpellAuraPeriodicEnergize(bool apply);
	void SpellAuraModPacify(bool apply);
	void SpellAuraModRoot(bool apply);
	void SpellAuraModSilence(bool apply);
	void SpellAuraReflectSpells(bool apply);
	void SpellAuraModStat(bool apply);
	void SpellAuraModSkill(bool apply);
	void SpellAuraModIncreaseSpeed(bool apply);
	void SpellAuraModDecreaseSpeed(bool apply);
	void SpellAuraModIncreaseHealth(bool apply);
	void SpellAuraModIncreaseEnergy(bool apply);
	void SpellAuraModShapeshift(bool apply);
	void SpellAuraModEffectImmunity(bool apply);
	void SpellAuraModStateImmunity(bool apply);
	void SpellAuraModSchoolImmunity(bool apply);
	void SpellAuraModIgnoreState(bool apply);
	void SpellAuraModNoCast(bool apply);
	void SpellAuraModAPBasedOnStat(bool apply);
	void SpellAuraModDmgImmunity(bool apply);
	void SpellAuraModDispelImmunity(bool apply);
	void SpellAuraProcTriggerSpell(bool apply);
	void SpellAuraProcTriggerDamage(bool apply);
	void SpellAuraTrackCreatures(bool apply);
	void SpellAuraTrackResources(bool apply);	
	void SpellAuraModParryPerc(bool apply);	
	void SpellAuraModDodgePerc(bool apply);	
	void SpellAuraModBlockPerc(bool apply);
	void SpellAuraModCritPerc(bool apply);
	void SpellAuraPeriodicLeech(bool apply);
	void SpellAuraModHitChance(bool apply);
	void SpellAuraModSpellHitChance(bool apply);
	void SpellAuraTransform(bool apply);
	void SpellAuraModSpellCritChance(bool apply);
	void SpellAuraIncreaseSwimSpeed(bool apply);	
	void SpellAuraModCratureDmgDone(bool apply);
	void SpellAuraPacifySilence(bool apply);
	void SpellAuraModScale(bool apply);
	void SpellAuraPeriodicHealthFunnel(bool apply);	
	void SpellAuraPeriodicManaLeech(bool apply);
	void SpellAuraModCastingSpeed(bool apply);
	void SpellAuraFeignDeath(bool apply);
	void SpellAuraModDisarm(bool apply);
	void SpellAuraModStalked(bool apply);
	void SpellAuraSchoolAbsorb(bool apply);	
	void SpellAuraModSpellCritChanceSchool(bool apply);
	void SpellAuraModPowerCost(bool apply);
	void SpellAuraModPowerCostSchool(bool apply);
	void SpellAuraReflectSpellsSchool(bool apply);
	void SpellAuraModLanguage(bool apply);
	void SpellAuraAddFarSight(bool apply);
	void SpellAuraMechanicImmunity(bool apply);
	void SpellAuraMounted(bool apply);
	void SpellAuraModDamagePercDone(bool apply);
	void SpellAuraModPercStat(bool apply);
	void SpellAuraSplitDamage(bool apply);
	void SpellAuraWaterBreathing(bool apply);
	void SpellAuraModBaseResistance(bool apply);
	void SpellAuraModRegen(bool apply);
	void SpellAuraModPowerRegen(bool apply);
	void SpellAuraModHealingPct(bool apply);
	void SpellAuraChannelDeathItem(bool apply);
	void SpellAuraModDamagePercTaken(bool apply);
	void SpellAuraModRegenPercent(bool apply);
	void SpellAuraPeriodicDamagePercent(bool apply);
	void SpellAuraModResistChance(bool apply);
	void SpellAuraModDetectRange(bool apply);
	void SpellAuraPreventsFleeing(bool apply);
	void SpellAuraModUnattackable(bool apply);
	void SpellAuraInterruptRegen(bool apply);
	void SpellAuraGhost(bool apply);
	void SpellAuraMagnet(bool apply);
	void SpellAuraManaShield(bool apply);
	void SpellAuraSkillTalent(bool apply);
	void SpellAuraModAttackPower(bool apply);
	void SpellAuraVisible(bool apply);
	void SpellAuraModResistancePCT(bool apply);
	void SpellAuraModCreatureAttackPower(bool apply);
	void SpellAuraModTotalThreat(bool apply);
	void SpellAuraWaterWalk(bool apply);
	void SpellAuraFeatherFall(bool apply);
	void SpellAuraHover(bool apply);
	void SpellAuraAddFlatModifier(bool apply);
	void SpellAuraAddPctMod(bool apply);
//	void SpellAuraAddTargetTrigger(bool apply);
	void SpellAuraModPowerRegPerc(bool apply);	
	void SpellAuraOverrideClassScripts(bool apply);
	void SpellAuraModRangedDamageTaken(bool apply);	
	void SpellAuraModRangedDamageTakenPCT(bool apply);	
	void SpellAuraModHealing(bool apply);
	void SpellAuraIgnoreRegenInterrupt(bool apply);
	void SpellAuraModMechanicResistance(bool apply);
	void SpellAuraModHealingPCT(bool apply);   
	void SpellAuraUntrackable(bool apply);
	void SpellAuraEmphaty(bool apply);
	void SpellAuraModOffhandDamagePCT(bool apply);
	void SpellAuraModPenetration(bool apply);
	void SpellAuraModRangedAttackPower(bool apply);
	void SpellAuraModMeleeDamageTaken(bool apply);
	void SpellAuraModMeleeDamageTakenPct(bool apply);
	void SpellAuraRAPAttackerBonus(bool apply);	
	void SpellAuraModIncreaseSpeedAlways(bool apply);
	void SpellAuraModIncreaseMountedSpeed(bool apply);
	void SpellAuraModCreatureRangedAttackPower(bool apply);
	void SpellAuraModIncreaseEnergyPerc(bool apply);
	void SpellAuraModIgnoreArmorForWeapon(bool apply);
	void SpellAuraModIncreaseMaxHealthPerc(bool apply);
	void SpellAuraModManaRegInterrupt(bool apply);
	void SpellAuraModHealingDone(bool apply);
	void SpellAuraModHealingDonePct(bool apply);
	void SpellAuraModTotalStatPerc(bool apply);
	void SpellAuraModHaste(bool apply);
	void SpellAuraForceReaction(bool apply);
	void SpellAuraModRangedHaste(bool apply);
	void SpellAuraModRangedAmmoHaste(bool apply);
	void SpellAuraModBaseResistancePerc(bool apply);
	void SpellAuraModResistanceExclusive(bool apply);
	void SpellAuraSafeFall(bool apply);
	void SpellAuraCrowdControlImmunity(bool apply);
	void SpellAuraRetainComboPoints(bool apply);
	void SpellAuraResistPushback(bool apply);
	void SpellAuraModShieldBlockPCT(bool apply);
	void SpellAuraTrackStealthed(bool apply);
	void SpellAuraModDetectedRange(bool apply);
	void SpellAuraSplitDamageFlat(bool apply);
	void SpellAuraModStealthLevel(bool apply);
	void SpellAuraModUnderwaterBreathing(bool apply);
	void SpellAuraModReputationAdjust(bool apply);
	void SpellAuraNoPVPCredit(bool apply);
	void SpellAuraModHealthRegInCombat(bool apply);
	void SpellAuraPowerBurn(bool apply);
	void SpellAuraModCritDmgSchool(bool apply);
	void SpellAuraModAttackPowerPct(bool apply);
	void SpellAuraModRangedAttackPowerPct(bool apply);
	void SpellAuraAPAttackerBonus(bool apply);
	void SpellAuraIncreaseDamageTypePCT(bool apply);
	void SpellAuraIncreaseCricticalTypePCT(bool apply);
	void SpellAuraIncreasePartySpeed(bool apply);
	void SpellAuraIncreaseSpellDamageByAttribute(bool apply);
	void SpellAuraIncreaseHealingByAttribute(bool apply);
	void SpellAuraIncreaseArmorByPctInt(bool apply);
	void SpellAuraReduceAttackerMHitChance(bool apply);
	void SpellAuraReduceAttackerRHitChance(bool apply);
	void SpellAuraReduceAttackerSHitChance(bool apply);
	void SpellAuraReduceEnemyMCritChance(bool apply);
	void SpellAuraReduceEnemyRCritChance(bool apply);
	void SpellAuraLimitSpeed(bool apply);
	void SpellAuraIncreaseTimeBetweenAttacksPCT(bool apply);
//	void SpellAuraIncreaseSpellDamageByInt(bool apply);
//	void SpellAuraIncreaseHealingByInt(bool apply);
	void SpellAuraIncreaseAllWeaponSkill(bool apply);
	void SpellAuraModAttackerCritChance(bool apply);
	void SpellAuraIncreaseHitRate(bool apply);
	void SpellAuraReduceCritMeleeAttackDmg(bool apply);
	void SpellAuraReduceCritRangedAttackDmg(bool apply);
	void SpellAuraEnableFlight(bool apply);
	void SpellAuraEnableFlightWithUnmountedSpeed(bool apply);
	void SpellAuraIncreaseRageFromDamageDealtPCT(bool apply);
	void SpellAuraIncreaseFlightSpeed(bool apply);
	void SpellAuraIncreaseMovementAndMountedSpeed(bool apply);
	void SpellAuraIncreaseRating(bool apply);
	void SpellAuraIncreaseCastTimePCT(bool apply);
	void SpellAuraRegenManaStatPCT(bool apply);
	void SpellAuraRatingBasedOnStat(bool apply);
	void SpellAuraModStealthDetection(bool apply);
	void SpellAuraIncreaseMaxHealth(bool apply);
	void SpellAuraSpiritOfRedemption(bool apply);
	void SpellAuraIncreaseAttackerSpellCrit(bool apply);
	void SpellAuraIncreaseRepGainPct(bool apply);
	void SpellAuraIncreaseExpGainPct(bool apply);
	void SpellAuraIncreaseRangedAPStatPCT(bool apply);
	//void SpellAuraModRangedDamageTakenPCT(bool apply);
	void SpellAuraModBlockValue(bool apply);
	void SpellAuraFinishingMovesCannotBeDodged(bool apply);
	void SpellAuraExpertieseModifier(bool apply);
	void SpellAuraPeriodicDummy(bool apply);
	void SpellAuraPeriodicTriggerSpellWithValue(bool apply);
	void SpellAuraModPossessPet(bool apply);
	void SpellAuraModHealingByAP(bool apply);
	void SpellAuraModSpellDamageByAP(bool apply);
	void SpellAuraMeleeHaste(bool apply);
	void SpellAuraReduceEffectDuration(bool apply);
	void HandleAuraControlVehicle(bool apply);
	void EventPeriodicDrink(uint32 amount);
	void SpellAuraConvertRune(bool apply);
	void SpellAuraAddHealth(bool apply);
	void SpellAuraModDMGTakenMechanic(bool apply);
	void SpellAuraRemoveReagentCost(bool apply);
	void SpellAuraIgnoreShapeshift(bool apply);
	void SpellAuraIncreaseAPBasedOnArmor(bool apply);
	void SpellAuraModHasteAndCastSpeed(bool apply);
	void SpellAuraModFaction(bool apply);
	void SpellAuraModXPAndRepGainPCT(bool apply);
	void SpellAuraModHonorGainPCT(bool apply);
	void SpellAuraModFlyghtSpeedPCT(bool apply);
	void SpellAuraModMountedSpeedPCT(bool apply);
	void SpellAuraModOffensiveMagicalDurationPCT(bool apply);
	void SpellAuraPeriodicDummyTrigger(bool apply, uint32 mod_i);
	void SpellAuraAuraSchoolImmunity(bool apply);
	void SpellAuraCritHealDoneModPct(bool apply);
	void SpellAuraModDeflectChance(bool apply);
	void SpellAuraOpenStableUI(bool apply);
	void SpellAuraPhase(bool apply);
	void SpellAuraMirrorDMG(bool apply);
	void SpellAuraModHealAbsorb(bool apply);
	void SpellAuraModIgnoreInterrupt(bool apply);
	void SpellAuraModSpellPowerPCT(bool apply);
	void SpellAuraModMastery(bool apply);
	void SpellAuraModAutoAttackDmg(bool apply);
	void SpellAuraModCritBlockChance(bool apply);
	void SpellAuraModDmgPenetrate(bool apply);
	void SpellAuraModRunicPowerRegenPCT(bool apply);
	void SpellAuraModDisarmOffhand(bool apply);
	void SpellAuraModDisarmRanged(bool apply);
	void SpellAuraModCritPercAll(bool apply);
	void SpellAuraModForceMoveForward(bool apply);
	void SpellAuraModComprehendLanguage(bool apply);
	void SpellAuraModAOEDmgTakenPct(bool apply);
	void SpellAuraModAOEDmgTakenPctFromCreature(bool apply);
	void SpellAuraReduceEffectDurationDispelType(bool apply);
	void SpellAuraModReplaceSpell(bool apply);
	void SpellAuraModAddTemporarySpell(bool apply);
	void SpellAuraModRangedTargetting(bool apply);
	void SpellAuraModCasterCritChanceOnTargetOfSpells(bool apply);
	void SpellAuraModAPToSPExclusive(bool apply);
	void SpellAuraModAttackSpeedNoRegen(bool apply);
	void SpellAuraModRegisterCanCastScript(bool apply);
#define AURA_SpellAuraIncreaseAPBasedOnArmor_REFRESH_INTERVAL 2*60*1000
	void Refresh_SpellAuraIncreaseAPBasedOnArmor(uint32 mod_i);
	//frikkin hackfix, one day i will figure out why i need this this way
	void	SendIgnoreStateAura();
	void	RemoveIgnoreStateAura();

	void UpdateAuraModDecreaseSpeed();

	void SendModifierLog(int32 **ObjectSMValues,int32 newval,uint32 *AffectsSMMask,uint8 type,uint8 pct = 0);

	// Events
	void EventPeriodicDamage( uint32 ticks, uint32 mod_index );
	void EventPeriodicDamage(uint32 amount, int32 pct_mod, uint32 ticks, uint32 eff_index);
	void EventPeriodicDamagePercent(uint32, uint32 eff_index, int32 PCT);
	void EventPeriodicHeal( int32 period, int32 mod_index );
	void EventPeriodicHeal2( uint32 amount, uint32 mod_index, int32 period, int32 eff_index );
	void EventPeriodicTriggerSpell(SpellEntry* spellInfo,int32 value_overwrite, uint64 periodic_target);
	void EventPeriodicEnergize(uint32,uint32);
	void EventPeriodicEnergizeVariable(uint32,uint32);
	void EventPeriodicHealNoBonus(uint32);
//	void EventPeriodicLeech(uint32,int32 period,int32 effect_index);
	void EventPeriodicLeech(uint32 aura_list_ind,int32 period);
	void EventPeriodicBurn(uint32,uint32);
	void EventPeriodicHealthFunnel(uint32);
	void EventPeriodicManaLeech(uint32,uint32 effect_index);
	void EventPeriodicHealPct(float RegenPct,int32 period, int32 eff_index);
	void EventPeriodicManaPct(float);
	void EventPeriodicRegenManaStatPct(uint32 perc,uint32 stat);
	void EventParalelManaRegen()	//used when druid shapeshifts to regen mana in the background
	{
		if( m_target && m_target->IsPlayer() )
			SafePlayerCast(m_target)->RegenerateMana( false );
	}
	int32 event_GetInstanceID();
/*
	ARCEMU_INLINE void SendPeriodicHealAuraLog(uint32 amt)
	{
{SERVER} Packet: (0xF557) UNKNOWN PacketSize = 46 TimeStamp = 16945811
8F F8 CA 03 05 06 
CF 35 31 17 04 80 06 
72 1F 00 00 - flame shock
01 00 00 00 1 struct
03 00 00 00 dmg
1B 00 00 00 total
07 00 00 00 overkill
04 00 00 00 absorbed
00 00 00 00 
00 00 00 00 
00 
		WorldPacket data(48);
		data.SetOpcode(SMSG_PERIODICAURALOG);
		data << m_target->GetNewGUID();
		FastGUIDPack(data, m_casterGuid);
		data << GetSpellProto()->Id;
		data << uint32(1);
		data << uint32(FLAG_PERIODIC_HEAL);	//some say this is auraname
		data << uint32(amt);
		data << uint32(m_target->GetOverHeal(amt));
		data << uint32(0);//absorb ?
		data << uint32(0);//critical ?
		data << uint32(0);
		m_target->SendMessageToSet(&data,true);
	}

	ARCEMU_INLINE void SendPeriodicHealAuraLog(Object *caster, Object *target,uint32 amt)
	{
		WorldPacket data(48);
		data.SetOpcode(SMSG_PERIODICAURALOG);
		data << target->GetNewGUID();
		data << caster->GetNewGUID();
		data << GetSpellProto()->Id;
		data << uint32(1);
		data << uint32(FLAG_PERIODIC_HEAL);
		data << uint32(amt);
		data << uint32(m_target->GetOverHeal(amt));
		data << uint32(0);
		data << uint32(0);
		data << uint32(0);
		m_target->SendMessageToSet(&data,true);
	}*/

	ARCEMU_INLINE bool WasCastInDuel() { return ( ( m_flags & AURAFLAG_CASTED_IN_DUEL ) != 0 ); }
	ARCEMU_INLINE void SetCastInDuel() { m_flags |= AURAFLAG_CASTED_IN_DUEL; }
	ARCEMU_INLINE bool IsAreaAura() { return ( ( m_flags & AURAFLAG_AREA_AURA ) != 0 ); }
	ARCEMU_INLINE void SetIsAreaAura() { m_flags |= AURAFLAG_AREA_AURA; }
	ARCEMU_INLINE bool WasCastInBG() { return ( ( m_flags & AURAFLAG_CASTED_IN_BG ) != 0 ); }
	ARCEMU_INLINE void SetWasCastInBG() { m_flags |= AURAFLAG_CASTED_IN_BG; }

	SpellEntry		* m_spellProto;
	Modifier		* mod;
	AreaAuraList	targets;//this is only used for AA
	uint64			m_casterGuid;
//	uint64			m_CasterDyn;	//dynamic objects will use the unit caster as caster. But we sometimes need to know the location of the Dyn

	uint32			m_castedItemId;
	uint16			m_auraSlot;		//slot on the unit
	//!!!!this seems to failt to properly count aura stack sometimes. Need to debug when
	uint16			GetStackCount()
	{
		if( m_target && m_visualSlot < MAX_VISIBLE_AURAS )
			return m_target->m_auraStackCount[m_visualSlot];
		return 0;
	}
	uint16			m_visualSlot;	//visual slot will also tell us the stack amount
	uint16			m_visualSlot2;	//used for ignorestate aura when only an enabler aura is sent
	uint32			pSpellId; //this represents the triggering spell id

	// this stuff can be cached in spellproto.
/*	ARCEMU_INLINE bool IsCombatStateAffecting()
	{
		for(uint32 x = 0; x < 3; ++x)
		{
			if(m_spellProto->EffectApplyAuraName[x] == SPELL_AURA_PERIODIC_DAMAGE || 
				m_spellProto->EffectApplyAuraName[x] == SPELL_AURA_PERIODIC_DAMAGE_PERCENT ||
				m_spellProto->EffectApplyAuraName[x] == SPELL_AURA_PERIODIC_TRIGGER_SPELL ||
				m_spellProto->EffectApplyAuraName[x] == SPELL_AURA_PERIODIC_LEECH ||
				m_spellProto->EffectApplyAuraName[x] == SPELL_AURA_PERIODIC_MANA_LEECH)
			{
				return true;
			}
		}

		return false;
	}*/

	inline bool TargetWasImuneToMods()
	{ 
		return (m_modcount && (((( m_flags & MOD_0_RESISTED)!=0)+(( m_flags & MOD_1_RESISTED)!=0)+(( m_flags & MOD_2_RESISTED)!=0)) == m_modcount) );
	}

	int32		GetModAmount( uint32 i ) { if(i<MAX_AURA_MOD_COUNT) return m_modList[i].m_amount; return 0; }
	int32		GetRealTickCount();
	bool		IsFirstTick(){ return ( ( m_flags & FIRST_TICK_ENDED ) == 0) ;	} //dot DOT and HOT, Some procs should only proc on first tick

	uint8			m_modcount;
//	int32			m_DamageDoneLastTick;
	//Modifier m_modList[3];
private:
	Unit			*m_target;
//	Player			*p_target;
	uint32			timeleft;
	int32			m_duration; // in msecs
	signed char		m_positive;

protected:

	void SendInterrupted(uint8 result, Object * m_caster);
	void SendChannelUpdate(uint32 time, Object * m_caster);
public:
	void		*temp_custom_structure_holder;
	uint32		m_flags;
	uint8		m_deleted;
//	int8		m_interrupted;
	uint32		m_spellId;		 // need this because something is corrupting pointers and i cannot see what aura is messed up
	tr1::array< Modifier, MAX_AURA_MOD_COUNT> m_modList; //put at the end so crashlog can be inspected

//	ARCEMU_INLINE bool IsInterrupted() { return ( m_interrupted >= 0 ); }
	//extensions
	typedef std::map<uint32, void*> ExtensionSet;
	ARCEMU_INLINE void* GetExtension(uint32 ID)
	{
		ExtensionSet::iterator itr = m_extensions.find( ID );
		if( itr == m_extensions.end() )
			return (NULL);
		else
			return (itr->second);
	}
	ARCEMU_INLINE void SetExtension(uint32 ID, void* ptr)
	{
		void *cur_value = GetExtension( ID );
		if( ptr == NULL )
		{
			if( cur_value != NULL )
			{
//				*cur_value = 0;	//just mark value as destroyed for debugging
				delete cur_value;
			}
			m_extensions.erase( ID );
		}
		else 
		{
			//find old one and delete it
			if( cur_value != NULL && ptr != cur_value )
			{
//				*cur_value = 0;	//just mark value as destroyed for debugging
				delete cur_value;
				m_extensions.erase( ID );
				m_extensions[ID] = ptr;
			}
			else if( cur_value == NULL )
				m_extensions[ID] = ptr;
		}
	}
	ARCEMU_INLINE int64* GetCreateIn64Extension(uint32 ID)
	{
		ExtensionSet::iterator itr = m_extensions.find( ID );
		if( itr == m_extensions.end() )
		{
			int64 *p = new int64;
			*p = 0;
			SetExtension( ID, p );
			return p;
		}
		else
			return (int64 *)(itr->second);
	}
	ExtensionSet m_extensions;
};

typedef void(Aura::*pSpellAura)(bool apply);

#endif