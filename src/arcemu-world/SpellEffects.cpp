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

#include "StdAfx.h"

pSpellEffect SpellEffectsHandler[TOTAL_SPELL_EFFECTS]={
		&Spell::SpellEffectNULL,					//SPELL_EFFECT_NULL - 0
		&Spell::SpellEffectInstantKill,				//SPELL_EFFECT_INSTAKILL - 1
		&Spell::SpellEffectSchoolDMG,				//SPELL_EFFECT_SCHOOL_DAMAGE - 2
		&Spell::SpellEffectDummy,					//SPELL_EFFECT_DUMMY - 3
		&Spell::SpellEffectNULL,					//SPELL_EFFECT_PORTAL_TELEPORT - 4
		&Spell::SpellEffectTeleportUnits,			//SPELL_EFFECT_TELEPORT_UNITS - 5
		&Spell::SpellEffectApplyAura,				//SPELL_EFFECT_APPLY_AURA - 6
		&Spell::SpellEffectEnvironmentalDamage,		//SPELL_EFFECT_ENVIRONMENTAL_DAMAGE - 7
		&Spell::SpellEffectPowerDrain,				//SPELL_EFFECT_POWER_DRAIN - 8
		&Spell::SpellEffectHealthLeech,				//SPELL_EFFECT_HEALTH_LEECH - 9
		&Spell::SpellEffectHeal,					//SPELL_EFFECT_HEAL - 10
		&Spell::SpellEffectBind,					//SPELL_EFFECT_BIND - 11
		&Spell::SpellEffectNULL,					//SPELL_EFFECT_PORTAL - 12
		&Spell::SpellEffectNULL,					//SPELL_EFFECT_RITUAL_BASE - 13
		&Spell::SpellEffectNULL,					//SPELL_EFFECT_RITUAL_SPECIALIZE - 14
		&Spell::SpellEffectNULL,					//SPELL_EFFECT_RITUAL_ACTIVATE_PORTAL - 15
		&Spell::SpellEffectQuestComplete,			//SPELL_EFFECT_QUEST_COMPLETE - 16
		&Spell::SpellEffectWeapondamageNoschool,	//SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL - 17
		&Spell::SpellEffectResurrect,				//SPELL_EFFECT_RESURRECT - 18
		&Spell::SpellEffectAddExtraAttacks,			//SPELL_EFFECT_ADD_EXTRA_ATTACKS - 19
		&Spell::SpellEffectDodge,					//SPELL_EFFECT_DODGE - 20
		&Spell::SpellEffectNULL,					//SPELL_EFFECT_EVADE - 21
		&Spell::SpellEffectParry,					//SPELL_EFFECT_PARRY - 22
		&Spell::SpellEffectBlock,					//SPELL_EFFECT_BLOCK - 23
		&Spell::SpellEffectCreateItem,				//SPELL_EFFECT_CREATE_ITEM - 24
		&Spell::SpellEffectWeapon,					//SPELL_EFFECT_WEAPON - 25
		&Spell::SpellEffectDefense,					//SPELL_EFFECT_DEFENSE - 26
		&Spell::SpellEffectPersistentAA,			//SPELL_EFFECT_PERSISTENT_AREA_AURA - 27
		&Spell::SpellEffectSummon,					//SPELL_EFFECT_SUMMON - 28
		&Spell::SpellEffectLeap,					//SPELL_EFFECT_LEAP - 29
		&Spell::SpellEffectEnergize,				//SPELL_EFFECT_ENERGIZE - 30
		&Spell::SpellEffectWeaponDmgPerc,			//SPELL_EFFECT_WEAPON_PERCENT_DAMAGE - 31
		&Spell::SpellEffectTriggerMissile,			//SPELL_EFFECT_TRIGGER_MISSILE - 32
		&Spell::SpellEffectOpenLock,				//SPELL_EFFECT_OPEN_LOCK - 33
		&Spell::SpellEffectTranformItem,			//SPELL_EFFECT_TRANSFORM_ITEM - 34
		&Spell::SpellEffectApplyAA,					//SPELL_EFFECT_APPLY_AREA_AURA - 35
//		&Spell::SpellEffectNULL,					// seems to be a scripted effect in 303 - 35 Increases stuff but seems to everything. 147 spells use it
		&Spell::SpellEffectLearnSpell,				//SPELL_EFFECT_LEARN_SPELL - 36
		&Spell::SpellEffectSpellDefense,			//SPELL_EFFECT_SPELL_DEFENSE - 37
		&Spell::SpellEffectDispel,					//SPELL_EFFECT_DISPEL - 38
		&Spell::SpellEffectLanguage,				//SPELL_EFFECT_LANGUAGE - 39
		&Spell::SpellEffectDualWield,				//SPELL_EFFECT_DUAL_WIELD - 40
//		&Spell::SpellEffectSummonWild,				//SPELL_EFFECT_SUMMON_WILD - 41
		&Spell::SpellEffectNULL,					//this is some move related effect now. Ex: leap, jump, tel - 41
//		&Spell::SpellEffectSummonGuardian,			//SPELL_EFFECT_SUMMON_GUARDIAN - 42
		&Spell::SpellEffectJumpBehindTarget,		//SPELL_EFFECT_JUMP_BEHIND_TARGET this is some move related effect now. Ex: pull, knock, tel - 42
		&Spell::SpellEffectNULL,					//SPELL_EFFECT_TELEPORT_UNITS_FACE_CASTER - 43
		&Spell::SpellEffectSkillStep,				//SPELL_EFFECT_SKILL_STEP - 44
		&Spell::SpellEffectAddHonor,				//SPELL_ADD_HONOR - 45
		&Spell::SpellEffectSpawn,					//SPELL_EFFECT_SPAWN - 46
		&Spell::SpellEffectNULL,					//SPELL_EFFECT_TRADE_SKILL - 47
		&Spell::SpellEffectNULL,					//SPELL_EFFECT_STEALTH - 48
		&Spell::SpellEffectNULL,					//SPELL_EFFECT_DETECT - 49
		&Spell::SpellEffectSummonObject,			//SPELL_EFFECT_SUMMON_OBJECT - 50
		&Spell::SpellEffectNULL,					//SPELL_EFFECT_FORCE_CRITICAL_HIT - 51 NA
		&Spell::SpellEffectNULL,					//SPELL_EFFECT_GUARANTEE_HIT - 52 NA
		&Spell::SpellEffectEnchantItem,				//SPELL_EFFECT_ENCHANT_ITEM - 53
		&Spell::SpellEffectEnchantItemTemporary,	//SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY - 54
		&Spell::SpellEffectTameCreature,			//SPELL_EFFECT_TAMECREATURE - 55
		&Spell::SpellEffectSummonPet,				//SPELL_EFFECT_SUMMON_PET	- 56
		&Spell::SpellEffectLearnPetSpell,			//SPELL_EFFECT_LEARN_PET_SPELL - 57
		&Spell::SpellEffectWeapondamage,			//SPELL_EFFECT_WEAPON_DAMAGE - 58
		&Spell::SpellEffectOpenLockItem,			//SPELL_EFFECT_OPEN_LOCK_ITEM - 59
		&Spell::SpellEffectProficiency,				//SPELL_EFFECT_PROFICIENCY - 60
		&Spell::SpellEffectSendEvent,				//SPELL_EFFECT_SEND_EVENT - 61
		&Spell::SpellEffectPowerBurn,				//SPELL_EFFECT_POWER_BURN - 62
		&Spell::SpellEffectThreat,					//SPELL_EFFECT_THREAT - 63
		&Spell::SpellEffectTriggerSpell,			//SPELL_EFFECT_TRIGGER_SPELL - 64
//		&Spell::SpellEffectHealthFunnel,			//SPELL_EFFECT_HEALTH_FUNNEL - 65
		&Spell::SpellEffectApplyAA,					//SPELL_EFFECT_APPLY_AREA_AURA - 65
		&Spell::SpellEffectPowerFunnel,				//SPELL_EFFECT_POWER_FUNNEL - 66
		&Spell::SpellEffectHealMaxHealth,			//SPELL_EFFECT_HEAL_MAX_HEALTH - 67
		&Spell::SpellEffectInterruptCast,			//SPELL_EFFECT_INTERRUPT_CAST - 68
		&Spell::SpellEffectDistract,				//SPELL_EFFECT_DISTRACT - 69
		&Spell::SpellEffectNULL,					//SPELL_EFFECT_PULL - 70
		&Spell::SpellEffectPickpocket,				//SPELL_EFFECT_PICKPOCKET - 71
		&Spell::SpellEffectAddFarsight,				//SPELL_EFFECT_ADD_FARSIGHT - 72
		&Spell::SpellEffectSummonPossessed,			//SPELL_EFFECT_SUMMON_POSSESSED - 73
//		&Spell::SpellEffectCreateSummonTotem,		//SPELL_EFFECT_SUMMON_TOTEM - 74
		&Spell::SpellEffectEquipGlyph,				//SPELL_EFFECT_APPLY_GLYPH - 74
		&Spell::SpellEffectHealMechanical,			//SPELL_EFFECT_HEAL_MECHANICAL - 75
		&Spell::SpellEffectSummonObjectWild,		//SPELL_EFFECT_SUMMON_OBJECT_WILD - 76
		&Spell::SpellEffectScriptEffect,			//SPELL_EFFECT_SCRIPT_EFFECT - 77
		&Spell::SpellEffectNULL,					//SPELL_EFFECT_ATTACK - 78
		&Spell::SpellEffectSanctuary,				//SPELL_EFFECT_SANCTUARY - 79
		&Spell::SpellEffectAddComboPoints,			//SPELL_EFFECT_ADD_COMBO_POINTS - 80
		&Spell::SpellEffectCreateHouse,				//SPELL_EFFECT_CREATE_HOUSE - 81
		&Spell::SpellEffectNULL,					//SPELL_EFFECT_BIND_SIGHT - 82
		&Spell::SpellEffectDuel,					//SPELL_EFFECT_DUEL - 83
		&Spell::SpellEffectStuck,					//SPELL_EFFECT_STUCK - 84
		&Spell::SpellEffectSummonPlayer,			//SPELL_EFFECT_SUMMON_PLAYER - 85
		&Spell::SpellEffectActivateObject,			//SPELL_EFFECT_ACTIVATE_OBJECT - 86
		&Spell::SpellEffectWMODamage,				//SPELL_EFFECT_WMO_DAMAGE - 87
		&Spell::SpellEffectWMORepair,				//SPELL_EFFECT_WMO_REPAIR - 88
		&Spell::SpellEffectSummonTotem,				//SPELL_EFFECT_SUMMON_TOTEM_SLOT3 - 89
		&Spell::SpellEffectSummonTotem,				//SPELL_EFFECT_SUMMON_TOTEM_SLOT4 - 90
		&Spell::SpellEffectNULL,					//SPELL_EFFECT_THREAT_ALL - 91 UNUSED
		&Spell::SpellEffectEnchantHeldItem,			//SPELL_EFFECT_ENCHANT_HELD_ITEM - 92
		&Spell::SpellEffectNULL,					//SPELL_EFFECT_SUMMON_PHANTASM - 93 OLD
		&Spell::SpellEffectSelfResurrect,			//SPELL_EFFECT_SELF_RESURRECT - 94
		&Spell::SpellEffectSkinning,				//SPELL_EFFECT_SKINNING - 95
		&Spell::SpellEffectCharge,					//SPELL_EFFECT_CHARGE - 96
		&Spell::SpellEffectSummonCritter,			//SPELL_EFFECT_SUMMON_CRITTER - 97
		&Spell::SpellEffectKnockBack,				//SPELL_EFFECT_KNOCK_BACK - 98
		&Spell::SpellEffectDisenchant,				//SPELL_EFFECT_DISENCHANT - 99
		&Spell::SpellEffectInebriate,				//SPELL_EFFECT_INEBRIATE - 100
		&Spell::SpellEffectFeedPet,					//SPELL_EFFECT_FEED_PET - 101
		&Spell::SpellEffectDismissPet,				//SPELL_EFFECT_DISMISS_PET - 102
		&Spell::SpellEffectReputation,				//SPELL_EFFECT_REPUTATION - 103
		&Spell::SpellEffectSummonObjectSlot,		//SPELL_EFFECT_SUMMON_OBJECT_SLOT1 - 104
		&Spell::SpellEffectSummonObjectSlot,		//SPELL_EFFECT_SUMMON_OBJECT_SLOT2 - 105
		&Spell::SpellEffectSummonObjectSlot,		//SPELL_EFFECT_SUMMON_OBJECT_SLOT3 - 106
		&Spell::SpellEffectSummonObjectSlot,		//SPELL_EFFECT_SUMMON_OBJECT_SLOT4 - 107
		&Spell::SpellEffectDispelMechanic,			//SPELL_EFFECT_DISPEL_MECHANIC - 108
		&Spell::SpellEffectSummonDeadPet,			//SPELL_EFFECT_SUMMON_DEAD_PET - 109
		&Spell::SpellEffectDestroyAllTotems,		//SPELL_EFFECT_DESTROY_ALL_TOTEMS - 110
		&Spell::SpellEffectNULL,					//SPELL_EFFECT_DURABILITY_DAMAGE - 111
		&Spell::SpellEffectSummonDemon,				//SPELL_EFFECT_SUMMON_DEMON - 112
		&Spell::SpellEffectResurrectNew,			//SPELL_EFFECT_RESURRECT_NEW - 113
		&Spell::SpellEffectAttackMe,				//SPELL_EFFECT_ATTACK_ME - 114
		&Spell::SpellEffectNULL,					//SPELL_EFFECT_DURABILITY_DAMAGE_PCT - 115
		&Spell::SpellEffectSkinPlayerCorpse,		//SPELL_EFFECT_SKIN_PLAYER_CORPSE - 116
		&Spell::SpellEffectNULL,					//SPELL_EFFECT_SPIRIT_HEAL - 117//Not used
		&Spell::SpellEffectSkill,					//SPELL_EFFECT_SKILL - 118
		&Spell::SpellEffectApplyPetAura,			//SPELL_EFFECT_APPLY_PET_AURA - 119
		&Spell::SpellEffectNULL,					//SPELL_EFFECT_TELEPORT_GRAVEYARD - 120//Not used
		&Spell::SpellEffectDummyMelee,				//SPELL_EFFECT_DUMMYMELEE	- 121
		&Spell::SpellEffectNULL,					// unknown - 122 //not used
		&Spell::SpellEffectFilming,					//SPELL_EFFECT_FILMING - 123 // http://www.thottbot.com/?sp=27998: flightpath 
		&Spell::SpellEffectPlayerPull,				//SPELL_EFFECT_PLAYER_PULL - 124 - http://thottbot.com/e2312
		&Spell::SpellEffectReduceThreatPercent,		//SPELL_EFFECT_REDUCE_THREAT_PERCENT - 125 // Reduce Threat by % //http://www.thottbot.com/?sp=32835
		&Spell::SpellEffectSpellSteal,				//SPELL_EFFECT_SPELL_STEAL - 126 // Steal Beneficial Buff (Magic) //http://www.thottbot.com/?sp=30449
		&Spell::SpellEffectProspecting,				// unknown - 127 // Search 5 ore of a base metal for precious gems.  This will destroy the ore in the process.
		&Spell::SpellEffectApplyAura128,			// unknown - 128 // Adjust a stats by %: Mod Stat // ITS FLAT
		&Spell::SpellEffectApplyAura,				// unknown - 129 // Mod Dmg % (Spells)
		&Spell::SpellEffectRedirectThreat,// unknown - 130 // http://www.thottbot.com/s34477
		&Spell::SpellEffectPlayMusic,				// unknown - 131 // test spell quite a few are music plays
		&Spell::SpellEffectPlayMusic,				// 132 SPELL_EFFECT_PLAY_MUSIC               sound id in misc value (SoundEntries.dbc)
		&Spell::SpellEffectForgetSpecialization,	//SPELL_EFFECT_FORGET_SPECIALIZATION - 133 // http://www.thottbot.com/s36441 // I think this is a gm/npc spell
		&Spell::SpellEffectKillCredit,				// SPELL_EFFECT_KILL_CREDIT - 134 // related to summoning objects and removing them, http://www.thottbot.com/s39161
		&Spell::SpellEffectNULL,					// unknown - 135 // no spells
		&Spell::SpellEffectHealMaxHealthPCT,		// SPELL_EFFECT_HEAL_MAXHEALTH_PCT - 136 // http://www.thottbot.com/s41542 and http://www.thottbot.com/s39703
		&Spell::SpellEffectEnergizeMaxEnergyPct,	// SPELL_EFFECT_ENERGIZE_MAXPOWER_PCT - 137 // http://www.thottbot.com/s41542 - restore max mana pct
		&Spell::SpellEffectKnockBack,				// SPELL_EFFECT_KNOCK_BACK2 - 138 // related to superjump or even "*jump" spells http://www.thottbot.com/?e=Unknown%20138
		&Spell::SpellEffectNULL,					// unknown - 139 // no spells
		&Spell::SpellEffectTriggerSpell,			// - 140 - triggers a spell from target back to caster - used at Malacrass f.e.
		&Spell::SpellEffectNULL,					// unknown - 141 // triggers spell, magic one,  (Mother spell) http://www.thottbot.com/s41065
		&Spell::SpellEffectTriggerSpellWithValue,	//SPELL_EFFECT_TRIGGER_SPELL_WITH_VALUE - 142 // triggers some kind of "Put spell on target" thing... (dono for sure) http://www.thottbot.com/s40872 and http://www.thottbot.com/s33076
		&Spell::SpellEffectApplyAura,				// unknown - 143 // Master -> deamon effecting spell, http://www.thottbot.com/s25228 and http://www.thottbot.com/s35696
		&Spell::SpellEffectKnockBack, // SPELL_EFFECT_KNOCK_BACK3 - 144 // maybe not only knockabck ?
		&Spell::SpellEffectJumpBehindTargetHighArc, // SPELL_EFFECT_LEAP2 jump effect - 145 
		&Spell::SpellEffectRuneActivate, // SPELL_EFFECT_RUNE_ACTIVATE - 146
		&Spell::SpellEffectNULL, // SPELL_EFFECT_QUEST_FAIL - 147 // http://thottbot.com/s46070
		&Spell::SpellEffectNULL, // school dmg - 148 
		&Spell::SpellEffectCharge, // SPELL_EFFECT_CHARGE2 charge - 149
		&Spell::SpellEffectNULL, // - 150 
		&Spell::SpellEffectSummonTarget, // SPELL_EFFECT_SUMMON_TARGET - 151 // trigger ritual of summoning
		&Spell::SpellEffectNULL, // summon refered friend (acc setting not used for emus) - 152
		&Spell::SpellEffectCreatePet, // SPELL_EFFECT_TAME_CREATURE - 153 // http://thottbot.com/s46686 , http://thottbot.com/s46716 , http://thottbot.com/s46717, http://thottbot.com/s46718 ...  // spelleffect summon pet ??
		&Spell::SpellEffectTeachTaxiPath, //  - 154
		&Spell::SpellEffectNULL, // remove 2 hand requirement for items - 155
		&Spell::SpellEffectEnchantItem, // SPELL_EFFECT_ADD_SOCKET - add socket to item - 156
		&Spell::SpellEffectCreateItem, // also creates a lot if gems - discover recipes - 157
		&Spell::SpellEffectMilling, // Milling - 158
		&Spell::SpellEffectRenamePet, // Ability to rename pet. Seems to be client sided - 159
		&Spell::SpellEffectNULL, //160
		&Spell::SpellEffectSetTalentSpecsCount, //161 Sets number of talent specs available to the player
		&Spell::SpellEffectActivateTalentSpec, //162 Activates one of talent specs
};

char* SpellEffectNames[TOTAL_SPELL_EFFECTS] = {
	"NULL",
	"INSTANT_KILL",              //    1
	"SCHOOL_DAMAGE",             //    2
	"DUMMY",                     //    3
	"PORTAL_TELEPORT",           //    4
	"TELEPORT_UNITS",            //    5
	"APPLY_AURA",                //    6
	"ENVIRONMENTAL_DAMAGE",      //    7
	"POWER_DRAIN",               //    8
	"HEALTH_LEECH",              //    9
	"HEAL",                      //    10
	"BIND",                      //    11
	"PORTAL",                    //    12
	"RITUAL_BASE",               //    13
	"RITUAL_SPECIALIZE",         //    14
	"RITUAL_ACTIVATE_PORTAL",    //    15
	"QUEST_COMPLETE",            //    16
	"WEAPON_DAMAGE_NOSCHOOL",    //    17
	"RESURRECT",                 //    18
	"ADD_EXTRA_ATTACKS",         //    19
	"DODGE",                     //    20
	"EVADE",                     //    21
	"PARRY",                     //    22
	"BLOCK",                     //    23
	"CREATE_ITEM",               //    24
	"WEAPON",                    //    25
	"DEFENSE",                   //    26
	"PERSISTENT_AREA_AURA",      //    27
	"SUMMON",                    //    28
	"LEAP",                      //    29
	"ENERGIZE",                  //    30
	"WEAPON_PERCENT_DAMAGE",     //    31
	"TRIGGER_MISSILE",           //    32
	"OPEN_LOCK",                 //    33
	"TRANSFORM_ITEM",            //    34
	"APPLY_AREA_AURA",           //    35
	"LEARN_SPELL",               //    36
	"SPELL_DEFENSE",             //    37
	"DISPEL",                    //    38
	"LANGUAGE",                  //    39
	"DUAL_WIELD",                //    40
	"SUMMON_WILD",               //    41
	"SPELL_EFFECT_JUMP_BEHIND_TARGET", //    42
	"TELEPORT_UNITS_FACE_CASTER",//    43
	"SKILL_STEP",                //    44
	"SPELL_EFFECT_ADD_HONOR",    //    45
	"SPAWN",                     //    46
	"TRADE_SKILL",               //    47
	"STEALTH",                   //    48
	"DETECT",                    //    49
	"SUMMON_OBJECT",             //    50
	"FORCE_CRITICAL_HIT",        //    51
	"GUARANTEE_HIT",             //    52
	"ENCHANT_ITEM",              //    53
	"ENCHANT_ITEM_TEMPORARY",    //    54
	"TAMECREATURE",              //    55
	"SUMMON_PET",                //    56
	"LEARN_PET_SPELL",           //    57
	"WEAPON_DAMAGE",             //    58
	"OPEN_LOCK_ITEM",            //    59
	"PROFICIENCY",               //    60
	"SEND_EVENT",                //    61
	"POWER_BURN",                //    62
	"THREAT",                    //    63
	"TRIGGER_SPELL",             //    64
//	"HEALTH_FUNNEL",             //    65
	"APPLY_AREA_AURA",           //    65
	"CREATE_MANA_GEM",           //    66
	"HEAL_MAX_HEALTH",           //    67
	"INTERRUPT_CAST",            //    68
	"DISTRACT",                  //    69
	"PULL",                      //    70
	"PICKPOCKET",                //    71
	"ADD_FARSIGHT",              //    72
	"UNTRAIN_TALENTS",           //    73
	"SPELL_EFFECT_APPLY_GLYPH",  //    74
	"HEAL_MECHANICAL",           //    75
	"SUMMON_OBJECT_WILD",        //    76
	"SCRIPT_EFFECT",             //    77
	"ATTACK",                    //    78
	"SANCTUARY",                 //    79
	"ADD_COMBO_POINTS",          //    80
	"CREATE_HOUSE",              //    81
	"BIND_SIGHT",                //    82
	"DUEL",                      //    83
	"STUCK",                     //    84
	"SUMMON_PLAYER",             //    85
	"ACTIVATE_OBJECT",           //    86
	"WMO_DAMAGE",				 //    87
	"WMO_REPAIR",				 //    88
	"WMO_CHANGE",				 //    89
	"KILL_CREDIT",				 //    90
	"THREAT_ALL",                //    91
	"ENCHANT_HELD_ITEM",         //    92
	"SUMMON_PHANTASM",           //    93
	"SELF_RESURRECT",            //    94
	"SKINNING",                  //    95
	"CHARGE",                    //    96
	"SUMMON_CRITTER",            //    97
	"KNOCK_BACK",                //    98
	"DISENCHANT",                //    99
	"INEBRIATE",                 //    100
	"FEED_PET",                  //    101
	"DISMISS_PET",               //    102
	"REPUTATION",                //    103
	"SUMMON_OBJECT_SLOT1",       //    104
	"SUMMON_OBJECT_SLOT2",       //    105
	"SUMMON_OBJECT_SLOT3",       //    106
	"SUMMON_OBJECT_SLOT4",       //    107
	"DISPEL_MECHANIC",           //    108
	"SUMMON_DEAD_PET",           //    109
	"DESTROY_ALL_TOTEMS",        //    110
	"DURABILITY_DAMAGE",         //    111
	"SUMMON_DEMON",              //    112
	"RESURRECT_FLAT",            //    113
	"ATTACK_ME",                 //    114
	"DURABILITY_DAMAGE_PCT",     //    115
	"SKIN_PLAYER_CORPSE",        //    116
	"SPIRIT_HEAL",               //    117
	"SKILL",                     //    118
	"APPLY_PET_AURA",            //    119
	"TELEPORT_GRAVEYARD",        //    120
	"DUMMYMELEE",                //    121
	"UNKNOWN1",                  //    122
	"FILMING",                   //    123
	"PLAYER_PULL",               //    124
	"UNKNOWN4",                  //    125
	"UNKNOWN5",                  //    126
	"PROSPECTING",               //    127
	"UNKNOWN7",                  //    128
	"APPLY_AURA",                //    129
	"UNKNOWN10",                 //    130
	"UNKNOWN11",                 //    131
	"UNKNOWN12",                 //    132
	"FORGET_SPECIALIZATION",     //    133
	"UNKNOWN14",                 //    134
	"UNKNOWN15",                 //    135
	"SPELL_EFFECT_HEAL_MAXHEALTH_PCT",                 //    136
	"SPELL_EFFECT_ENERGIZE_MAXPOWER_PCT",                 //    137
	"SPELL_EFFECT_KNOCK_BACK2",  //    138
	"UNKNOWN19",                 //    139
	"SPELL_EFFECT_FORCE_CAST",   //    140
	"UNKNOWN21",                 //    141
	"TRIGGER_SPELL_WITH_VALUE",  //    142
	"PET-MASTER AURA",           //    143
	"SPELL_EFFECT_KNOCK_BACK3",   //    144
	"SPELL_EFFECT_LEAP2",        //    145
	"SPELL_EFFECT_RUNE_ACTIVATE",//    146
	"QUEST_FAIL",                //    147
	"UNKNOWN26",                 //    148
	"UNKNOWN27",                 //    149
	"UNKNOWN28",                 //    150
	"SUMMON_TARGET",             //    151
	"UNKNOWN29",                 //    152
	"TAME_CREATURE",             //    153
	"?",						 //    154
	"?",						 //    155
	"ADD_SOCKET",				 //    156
	"CREATE_ITEM_2",             //    157
	"MILLING",				     //    158
	"ALLOW_RENAME_PET",          //    159
	"UNKNOWN160",		         //    160
	"SET_SPEC_COUNT",            //    161
	"SET_ACTIVE_SPEC",           //    162
};

void Spell::SpellEffectNULL(uint32 i)
{
	sLog.outDebug("Unhandled spell effect %u in spell %u.\n",GetProto()->eff[i].Effect,GetProto()->Id);
}

void Spell::SpellEffectInstantKill(uint32 i)
{
	if(!unitTarget || !unitTarget->isAlive())
	{ 
		return;
	}

	//Sacrifice: if spell caster has "void walker" pet, pet dies and spell caster gets a 
	/*Sacrifices the Voidwalker, giving its owner a shield that will absorb 
	305 damage for 30 sec. While the shield holds, spellcasting will not be \
	interrupted by damage.*/

	/*
	Demonic Sacrifice

	When activated, sacrifices your summoned demon to grant you an effect that lasts 
	30 minutes. The effect is canceled if any Demon is summoned. 
	Imp: Increases your Fire damage by 15%. 
	Voidwalker: Restores 3% of total Health every 4 sec. 
	Succubus: Increases your Shadow damage by 15%. 
	Felhunter: Restores 2% of total Mana every 4 sec.

	When activated, sacrifices your summoned demon to grant you an effect that lasts $18789d.  The effect is canceled if any Demon is summoned.

	Imp: Increases your Fire damage by $18789s1%.

	Voidwalker: Restores $18790s1% of total Health every $18790t1 sec.

	Succubus: Increases your Shadow damage by $18791s1%.

	Felhunter: Restores $18792s1% of total Mana every $18792t1 sec.

	*/
	uint32 spellId = GetProto()->Id;

	switch(spellId)
	{
	case 3617://Goblin Bomb Suicide
		{
			if(m_caster->GetTypeId() != TYPEID_UNIT)
				break;
			Unit *caster = m_caster->GetMapMgr()->GetPlayer(m_caster->GetUInt32Value(UNIT_FIELD_SUMMONEDBY));
			caster->summonPet->RemoveFromWorld(false,true);
			sGarbageCollection.AddObject( caster->summonPet );
			caster->summonPet = NULL;
		}break;
	case 7814:
	case 7815:
	case 7816:
	case 7876:
	case 7877:
	case 7878:
	case 11778:
	case 11779:
	case 11780:
	case 15968:
	case 15969:
	case 18128:
	case 18129:
	case 20398:
	case 20399:
	case 20400:
	case 20401:
	case 20402:
		{
		}break;

	case 29364:	// encapsulate voidwalker
		{
			return;
		}break;
	case 18788: //Demonic Sacrifice (508745)
		uint32 spellid1 = 0;
		switch(unitTarget->GetEntry())
		{
			case 416: //Imp
			{   
				spellid1 = 18789;
		
			}break;
			case 417: //Felhunter
			{
				spellid1 = 18792;
		
			}break;
			case 1860: //VoidWalker
			{
				spellid1 = 18790;
			}break;
			case 1863: //Succubus
			{
				spellid1 = 18791;
			}break;
			case 17252: //felguard
			{
				spellid1 = 35701;
			}break;
		}
		//now caster gains this buff
		if (spellid1)
		{
			SpellEntry *sp = dbcSpell.LookupEntry(spellid1);
			if (sp) u_caster->CastSpell(u_caster, sp, true);
		}
	}

	switch( GetProto()->NameHash )
	{
	case SPELL_HASH_DEATH_PACT:
		{}break;	//allow the kill on our minions, special test is used to avoid people exploiting isnta kill spells
	case SPELL_HASH_SACRIFICE:
		{
			if( !u_caster->IsPet() )
			{ 
				return;
			}

			SafePetCast(u_caster)->Dismiss( );
			return;
		}break;
	case SPELL_HASH_DEMONIC_SACRIFICE:
		{
			if( !p_caster->IsPlayer() )
			{ 
				return;
			}

			SafePetCast(unitTarget)->Dismiss( );
			return;
		}break;

	default:
		{
			// moar cheaters
			if( p_caster == NULL || (u_caster != NULL && u_caster->IsPet() ) )
			{ 
				return;
			}

			if( p_caster->GetSession()->GetPermissionCount() == 0 )
			{ 
				return;
			}
		}
	}
	//instant kill effects don't have a log
	//m_caster->SpellNonMeleeDamageLog(unitTarget, GetProto()->Id, unitTarget->GetUInt32Value(UNIT_FIELD_HEALTH), true);
//	m_caster->DealDamage(unitTarget, unitTarget->GetUInt32Value(UNIT_FIELD_HEALTH)*100, 0);
	unitTarget->SetUInt32Value( UNIT_FIELD_HEALTH, 1 );
	m_caster->DealDamage(unitTarget, 666, 0);	//muhahaha, doomed !
	WorldPacket data(SMSG_SPELLINSTAKILLLOG, 100);
	data << m_caster->GetGUID() << unitTarget->GetGUID() << spellId;
	m_caster->SendMessageToSet(&data, true);
}

void Spell::SpellEffectSchoolDMG(uint32 i) // dmg school
{
	if(!unitTarget || !unitTarget->isAlive())
	{ 
		return;
	}

	if( unitTarget->SchoolImmunityAntiEnemy[GetProto()->School] || unitTarget->SchoolImmunityAntiFriend[GetProto()->School] )
	{
		SendCastResult(SPELL_FAILED_IMMUNE);
		return;
	}
   
	int32 dmg;
	bool static_damage=false;

	{
		dmg = damage;
		switch(GetProto()->NameHash)
		{
			case SPELL_HASH_MOLTEN_ARMOR:		// fire armor, is static damage
			case SPELL_HASH_FIRE_STRIKE:
			case SPELL_HASH_LIGHTNING_STRIKE:
			case SPELL_HASH_LIGHTNING_CONDUIT: //hagara boss encounter scripted spell
				static_damage=true;
			break;
			//Shockwave Damage
			//Concussion Blow
			case SPELL_HASH_SHOCKWAVE:
			case SPELL_HASH_CONCUSSION_BLOW:
			{
				dmg = float2int32( float( u_caster->GetAP() ) * 0.75f );
			}break;
		}
	}

	// check for no more damage left (chains)
	if(!dmg) 
		return;

	if( dmg < 0 )
		return;

	//make sure there are no value overflows anywhere
	if( dmg > 1000000 )
		return;

	if(GetProto()->speed > 0 && pSpellId == 0 )
	{
		//FIXME:Use this one and check player movement and update distance
		//It now only checks the first distance and hits the player after time expires.
		//sEventMgr.AddEvent(this, &Spell::_DamageRangeUpdate, (uint32)100, EVENT_SPELL_DAMAGE_HIT, 100, 0);
		// Zack : some spells might rely on the delay effect of the speed factor ( chained casts )
		float dist = m_caster->CalcDistance( unitTarget );
		float time = ((dist*1000.0f)/GetProto()->speed);
/*		if(time <= 500)
		{
			SpellEntry *ID_for_the_logs ;
			if( GetProto()->spell_id_client )
				ID_for_the_logs = dbcSpell.LookupEntry( GetProto()->spell_id_client );	//this might get to be null. Dirty client has more spells then us. CHEATER !
			else
				ID_for_the_logs = GetProto();
			m_caster->SpellNonMeleeDamageLog(unitTarget, ID_for_the_logs, dmg, pSpellId==0, static_dmg[i] != 0, false, 1, chaincast_jump_pct_mod + forced_pct_mod_cur - 100 , i);
		}
		else */
		{
#ifndef USE_SPELL_MISSILE_DODGE
			/*sEventMgr.AddEvent(m_caster, &Object::SpellNonMeleeDamageLog,
				unitTarget,GetProto()->Id,dmg, EVENT_SPELL_DAMAGE_HIT, uint32(time), 1);*/
			sEventMgr.AddEvent(m_caster, &Object::EventSpellDamage, unitTarget->GetGUID(), GetProto(), (uint32)dmg, (int32)forced_pct_mod_cur, (int32)i, EVENT_SPELL_DAMAGE_HIT, uint32(time), 1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
#else
			LocationVector src,dst;
			src.x = m_caster->GetPositionX();
			src.y = m_caster->GetPositionY();
			src.z = m_caster->GetPositionZ();
			dst.x = unitTarget->GetPositionX();
			dst.y = unitTarget->GetPositionY();
			dst.z = unitTarget->GetPositionZ();
			sEventMgr.AddEvent(m_caster, &Object::EventSpellDamageToLocation, unitTarget->GetGUID(), GetProto(), (uint32)dmg, (int32)forced_pct_mod_cur, (int32)i, src, dst, EVENT_SPELL_DAMAGE_HIT, uint32(time), 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
#endif
		}
	}
	else
	{
		if( GetType() == SPELL_DMG_TYPE_MAGIC )		
		{
			static_damage = static_damage || (static_dmg[i] != 0);
			dealdamage tdmg;
			tdmg.base_dmg = dmg;
			tdmg.pct_mod_final_dmg = chaincast_jump_pct_mod + forced_pct_mod_cur - 100;
			tdmg.StaticDamage = static_damage;
			tdmg.DisableProc = (pSpellId!=0);
			m_caster->SpellNonMeleeDamageLog( unitTarget, GetProto(), &tdmg, 1, i);
		}
		else 
		{
			if( u_caster != NULL )
			{
				uint32 _type;
				if( GetType() == SPELL_DMG_TYPE_RANGED )
					_type = RANGED;
				else
				{
					if (GetProto()->AttributesExC & 0x1000000)
						_type =  OFFHAND;
					else
						_type = MELEE;
				}
				u_caster->Strike( unitTarget, _type, GetProto(), 0, forced_pct_mod_cur-100, dmg, pSpellId != 0, false );
			}
		}
	}   
	if(GetProto()->eff[i].EffectChainTarget)//chain
	{
		int32 reduce_reduce = 0;	
		if((GetProto()->c_is_flags & SPELL_FLAG_IS_REQUIRING_SM_MOD) && u_caster && u_caster->SM_Mods )
			SM_FIValue(u_caster->SM_Mods->SM_PJumpReduce,&reduce_reduce,GetProto()->GetSpellGroupType());
		chaincast_jump_pct_mod = float2int32( chaincast_jump_pct_mod * GetProto()->eff[i].EffectChainMultiplier ) + reduce_reduce;
	}
}

void Spell::SpellEffectDummy(uint32 i) // Dummy(Scripted events)
{
	uint32 spellId = GetProto()->Id;
	
	// Try a dummy SpellHandler
	if(sScriptMgr.CallScriptedDummySpell(GetProto(), i, this))
	{ 
		return;
	}

	if( i_caster && i_caster->GetProto() )
	{
		uint32 item_entry = i_caster->GetProto()->ItemId;

		switch(item_entry)
		{
#include "../arcemu-customs/custom_item_handlers.cpp"
			default:break;
		};
	}

	switch( GetProto()->NameHash )
	{
		case SPELL_HASH_COLD_SNAP:
		{
			if( p_caster )
				p_caster->ClearCooldownsOnLine( SKILL_FROST, GetProto()->Id );
		}break;
		case SPELL_HASH_SAVAGE_ROAR:
		{
			if( p_caster && p_caster->m_comboPoints )
			{
				forced_duration = 9000 + p_caster->m_comboPoints * 5000;	//this will be used for effect 1 = aura
				Dur = forced_duration;
			}
		}break;
		case SPELL_HASH_VANISH:
		{
			if( u_caster )
				u_caster->RemoveAurasMovementImpairing();
		}break;
		case SPELL_HASH_BLADED_ARMOR://Increases your attack power by $s1 for every $s2 armor value you have.
			{
				if( u_caster )
					forced_basepoints[1] = (GetProto()->eff[1].EffectBasePoints + 1)*u_caster->GetUInt32Value( UNIT_FIELD_RESISTANCES ) / damage;
			}break;
		case SPELL_HASH_KILLING_SPREE:
			{
				if( u_caster == NULL || unitTarget == NULL )
					return;
				//push the other events back with 1 trigger period
				sEventMgr.DelayEventTrigger( u_caster, EVENT_KILLING_SPREE_PROC, GetProto()->eff[i].EffectAmplitude );
				//register a new trigger
				uint64 guid = unitTarget->GetGUID();
				sEventMgr.AddEvent(u_caster, &Unit::EventCastSpell2, guid, GetProto()->eff[i].EffectTriggerSpell, true,
					EVENT_KILLING_SPREE_PROC, GetProto()->eff[i].EffectAmplitude, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
				if( p_caster )
					p_caster->SpeedCheatDelay( GetProto()->eff[i].EffectAmplitude * GetProto()->quick_tickcount + MIN( 5000, p_caster->GetSession()->GetLatency() ) + 4000 );
			}break;
		case SPELL_HASH_CALL_OF_THE_ELEMENTS:
		case SPELL_HASH_CALL_OF_THE_ANCESTORS:
		case SPELL_HASH_CALL_OF_THE_SPIRITS:
			{
				if( p_caster )
				{
					uint32 totem;
					int32 from,to;
					if( GetProto()->NameHash == SPELL_HASH_CALL_OF_THE_ELEMENTS )
					{
						from = 135;
						to = 132;
					}
					else if( GetProto()->NameHash == SPELL_HASH_CALL_OF_THE_ANCESTORS )
					{
						from = 139;
						to = 136;
					}
					else if( GetProto()->NameHash == SPELL_HASH_CALL_OF_THE_SPIRITS )
					{
						from = 143;
						to = 140;
					}
					else 
					{
						return;
					}
					//reverse order so stoneclaw totem gets casted last 
					uint32 order[]={0,2,3,1};
//					for( int32 ab=from;ab>=to;ab--)
					for( int32 ab=0;ab<4;ab++)
					{
						totem = p_caster->GetAction( to + order[ab] );
						if( totem )
						{
							SpellEntry * spellInfo = dbcSpell.LookupEntryForced( totem );
							if( !p_caster->HasSpell(totem) 
								|| spellInfo == NULL
								|| ( spellInfo->unk_320_1 & SPELL_ATTR7_SUMMON_PLAYER_TOTEM )==0 
								|| ( spellInfo->Attributes & ATTRIBUTES_PASSIVE ) )
								break;
							p_caster->ClearCategoryCooldownForSpell( totem, true );
							p_caster->CastSpell( p_caster, totem, false );
//							p_caster->CastSpellDelayed( p_caster->GetGUID(), totem, ab * 1500, false );
						}
					}
				}
			}break;
	};

	switch(spellId)
	{
#include "../arcemu-customs/custom_spell_effects.cpp"
        case 59640:                                     // Underbelly Elixir
            if( u_caster )
            {
//				if( u_caster->HasAura( 59645 ) == NULL && u_caster->HasAura( 59831 ) == NULL && u_caster->HasAura( 59843 ) == NULL )
				if( u_caster->HasAura( 59645 ) == NULL && u_caster->HasAura( 59831 ) == NULL )
				{
					static uint32 PossibleForms[3] = { 59645, 59831 };
		            u_caster->CastSpell( unitTarget, PossibleForms[ RandomUInt() % 2 ], true ); 
				}
            }
            break;
		case 46354:                                     // Blood Elf Illusion
            if( u_caster )
            {
				if( u_caster->getGender() )
                    u_caster->CastSpell( unitTarget, 46356, true ); //female
				else
					u_caster->CastSpell( unitTarget, 46355, true );
            }
            break;
		case 42339: //Bucket Lands
			{
				if( unitTarget && unitTarget->IsCreature() && SafeCreatureCast( unitTarget )->GetScript() )
					SafeCreatureCast( unitTarget )->GetScript()->SetValue( 0, 42339 );
			}break;
		case 55342: //Mirror Image - loose targetting
			{
				if( u_caster )
				{
					InRangeSetRecProt::iterator itr2;
					m_caster->AquireInrangeLock(); //make sure to release lock before exit function !
					InrangeLoopExitAutoCallback AutoLock;
					for( itr2 = m_caster->GetInRangeSetBegin( AutoLock ); itr2 != m_caster->GetInRangeSetEnd();)
					{
						Object *ot = *itr2;
						itr2++;
						if( ot->IsUnit() == false
							|| SafeUnitCast((ot))->GetTopOwner() == m_caster 
							|| SafeUnitCast((ot))->GetTargetGUID() != m_caster->GetGUID()
							|| isAttackable( ot, m_caster ) == false
							)
							continue;
						SafeUnitCast((ot))->ClearTargetGUID();
					}
					m_caster->ReleaseInrangeLock();
				}
			}break;
		case 89792: //Flee
			{
				if( unitTarget )
					unitTarget->RemoveAurasMovementImpairing();
			}break;
		case 101184: //Leyara's Locket
			{
				if( unitTarget->getGender() == 0 )
					unitTarget->CastSpell( unitTarget, 101185, true ); 
				else
					unitTarget->CastSpell( unitTarget, 101186, true ); 
			}break;
		case 51640: //Taunt Flag Targeting
			{
				Player *Target = p_caster->GetMapMgr()->GetPlayer( p_caster->GetSelection() );
				if( Target != NULL )
					p_caster->CastSpell( Target, 52605, true ); // Taunt Flag
			}break;
		case 8232: //Windfury Weapon
			{
				if( u_caster )
				{
					//dummy effects sets the actual bonus the procs should use
					ProcTriggerSpell	*itr2 = u_caster->HasProcSpell( 33757 );
					if( itr2 )
						itr2->created_with_value = damage;
				}
			}break;
		case 55004: //Nitro Boosts
			{
				if( unitTarget )
				{
					//small chance to Rocket Fuel Leak
					if( RandChance( 10 ) )
						unitTarget->CastSpell( unitTarget, 94794, true );
					else if( RandChance( 10 ) )
						unitTarget->CastSpell( unitTarget, 48030, true ); //rocket jump
					else
						unitTarget->CastSpell( unitTarget, 54861, true );
				}
			}break;
		case 51582: //Rocket Boots Engaged
			{
				if( unitTarget )
					unitTarget->CastSpell( unitTarget, 30452, true );
			}break;
		case 96429:	//Disentanglement
			{
				if( playerTarget )
					playerTarget->RemoveAllAuraType( SPELL_AURA_MOD_ROOT );
			}break;
		case 50315: //disco
			{
				if( playerTarget ) //making some NPCs do emotes they do not have might crash the client
				{
//					uint32 value_list[] = {EMOTE_ONESHOT_CHEER,EMOTE_STATE_DANCE,EMOTE_ONESHOT_CRY,EMOTE_ONESHOT_KISS,EMOTE_ONESHOT_TRAIN,EMOTE_ONESHOT_CHICKEN,EMOTE_STATE_APPLAUD,EMOTE_STATE_ROAR,EMOTE_STATE_DANCESPECIAL,EMOTE_ONESHOT_DANCESPECIAL};
					uint32 value_list[] = {EMOTE_ONESHOT_CHEER,EMOTE_STATE_DANCE,EMOTE_ONESHOT_CRY,EMOTE_ONESHOT_KISS,EMOTE_ONESHOT_TRAIN,EMOTE_ONESHOT_CHICKEN,EMOTE_STATE_APPLAUD,EMOTE_STATE_ROAR};
					uint32 list_size = sizeof(value_list) / sizeof( uint32 );
					uint32 randval = value_list[ RandomUInt() % list_size ];
					playerTarget->SetUInt32Value( UNIT_NPC_EMOTESTATE, randval );
					//give a small chance to get shamshed :P
					if( RandChance( 10 ) && playerTarget->GetDrunkValue() == 0 )
					{
						damage = 23000;
						SpellEffectInebriate(i);
					}
				}
			}break;
		case 86121: //Soul Swap
			{
				if( p_caster == NULL )
					return;
				bool NeedRemove = (p_caster->HasGlyphWithID( GLYPH_WARLOCK_SOUL_SWAP ) == false);
				//we are limited to exhale the DOTS on the same target
				uint64 *target = (uint64*)p_caster->GetCreateIn64Extension( EXTENSION_ID_SOUL_SWAP_TARGET );
				*target = unitTarget->GetGUID();
				//let's make a list of the IDs we are inhaling
				int64 *counter = p_caster->GetCreateIn64Extension( EXTENSION_ID_SOUL_SWAP_AURA_COUNTER );
				*counter = 0;
				for( uint32 i = NEG_AURAS_START; i < MAX_NEGATIVE_AURAS1( unitTarget ); i++ )
					if( unitTarget->m_auras[i] 
						&& unitTarget->m_auras[i]->GetCasterGUID() == p_caster->GetGUID()
						&& (unitTarget->m_auras[i]->GetSpellProto()->SchoolMask & SCHOOL_MASK_SHADOW )
						&& unitTarget->m_auras[i]->GetSpellProto()->quick_tickcount > 1 )
						{
							int64 *holder = p_caster->GetCreateIn64Extension( EXTENSION_ID_SOUL_SWAP_AURAS_START + (uint32)*counter );
							*holder = unitTarget->m_auras[i]->GetSpellId();
							if( NeedRemove == true )
								unitTarget->m_auras[i]->Remove();
							*counter = *counter + 1;
						}
				if( *counter > 0 )
				{
					//swap the icon client side to exhale
					p_caster->CastSpell( p_caster, 86211, true );				
					//trigger client side cooldown ( don't ask, idiot blizz )
					p_caster->CastSpell( p_caster, 94229, true );
					p_caster->_Cooldown_Add( COOLDOWN_TYPE_SPELL, GetProto()->Id, getMSTime() + 30000, GetProto()->Id, 0, 0 );
				}

			}break;
		case 86213: //Soul Swap Exhale
			{
				if( p_caster == NULL )
					return;
				//same target ? Sorry, we do not allow that
				uint64 *target = (uint64*)p_caster->GetCreateIn64Extension( EXTENSION_ID_SOUL_SWAP_TARGET );
				if( *target == unitTarget->GetGUID() )
					return;
				//get the number of spells to exhale
				int64 *counter = p_caster->GetCreateIn64Extension( EXTENSION_ID_SOUL_SWAP_AURA_COUNTER );
				if( *counter == 0 )
					return;
				*counter = MIN( MAX_AURAS, *counter ); //sanity check
				//we can only exhale while we have the buff on us
				Aura *a = p_caster->HasAura( 86211 ); // the exhale marker
				if( a == NULL )
				{
					*counter = 0;
					return;
				}
				a->Remove();
				for( uint32 i = 0; i < *counter; i++ )
				{
					int64 *spellID = p_caster->GetCreateIn64Extension( EXTENSION_ID_SOUL_SWAP_AURAS_START + i );
					p_caster->CastSpell( unitTarget, (uint32)*spellID, true );
				}
/*				if( p_caster->HasGlyphWithID( GLYPH_WARLOCK_SOUL_SWAP ) == true )
				{
					//trigger client side cooldown ( don't ask, idiot blizz )
					p_caster->CastSpell( p_caster, 94229, true );
//					p_caster->Cooldown_Add( 86121, 
				} */
				//we should not be able to exhale unless we inhaled something
				*counter = 0;
			}break;
		case 98020:	//Spirit Link - Spirit Link Totem - redistribute health in the party 
			{
				Unit *owner = u_caster->GetTopOwner();
				if( owner->IsPlayer() == false )
					break;
				Player *powner = SafePlayerCast( owner );
				if( powner->GetGroup() == NULL )
					break;
				Group * group = powner->GetGroup(); 
				float range=GetRadius(i);
				range*=range;
				group->Lock();
				int32 sum_health_pct = 0;
				int32 member_count = 0;
				uint32 count = group->GetSubGroupCount();
				// Loop through each raid group.
				for( uint8 k = 0; k < count; k++ )
				{
					SubGroup * subgroup = group->GetSubGroup( k );
					if( subgroup )
					{
						for( GroupMembersSet::iterator itr = subgroup->GetGroupMembersBegin(); itr != subgroup->GetGroupMembersEnd(); ++itr )
						{
							if( (*itr)->m_loggedInPlayer 
								&& (*itr)->m_loggedInPlayer->isAlive() 
								&& IsInrange( (*itr)->m_loggedInPlayer, u_caster, range ) )
							{
								member_count++;
								sum_health_pct += (*itr)->m_loggedInPlayer->GetHealthPct();
							}
						}
					}
				}
				//nobody is in range of the totem ? sucks for them
				if( member_count == 0 )
				{
					group->Unlock();
					break;
				}
				int32 new_pct_health = sum_health_pct / member_count;
				for( uint8 k = 0; k < count; k++ )
				{
					SubGroup * subgroup = group->GetSubGroup( k );
					if( subgroup )
					{
						for( GroupMembersSet::iterator itr = subgroup->GetGroupMembersBegin(); itr != subgroup->GetGroupMembersEnd(); ++itr )
						{
							if( (*itr)->m_loggedInPlayer 
								&& (*itr)->m_loggedInPlayer->isAlive() 
								&& IsInrange( (*itr)->m_loggedInPlayer, u_caster, range ) )
							{
								int32 healt_pct_now = (*itr)->m_loggedInPlayer->GetHealthPct();
								int32 deal_dmg = 1;
								int32 heal_amt = 1;
								//damage him
								if( healt_pct_now > new_pct_health )
								{
									int32 loose_hp_pct = healt_pct_now - new_pct_health;
									deal_dmg = (*itr)->m_loggedInPlayer->GetMaxHealth() * loose_hp_pct / 100;
								}
								//heal him
								else if( healt_pct_now < new_pct_health )
								{
									int32 gain_hp_pct = new_pct_health - healt_pct_now;
									heal_amt = (*itr)->m_loggedInPlayer->GetMaxHealth() * gain_hp_pct / 100;
								}
								//everyone at 100% ?
								if( deal_dmg == 1 && heal_amt == 1 )
									continue;
								SpellCastTargets targets( (*itr)->m_loggedInPlayer->GetGUID() );
								SpellEntry *spellInfo = dbcSpell.LookupEntry( 98021 );	//Spirit Link - heall / dmg
								Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
								spell->Init( u_caster, spellInfo ,true, NULL );
								if( deal_dmg == 1)
									spell->redirected_effect[0] = SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_ALL_TARGETS;
								spell->forced_basepoints[0] = deal_dmg; 
								if( heal_amt == 1)
									spell->redirected_effect[1] = SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_ALL_TARGETS;
								spell->forced_basepoints[1] = heal_amt;
								spell->ProcedOnSpell = GetProto();
								spell->pSpellId=GetProto()->Id;
								spell->prepare(&targets);
							}
						}
					}
				}
				group->Unlock();
			}break;
		case 88751:	//Wild Mushroom: Detonate
			{
				if( u_caster )
				{
					InRangeSetRecProt::iterator itr2;
					m_caster->AquireInrangeLock(); //make sure to release lock before exit function !
					InrangeLoopExitAutoCallback AutoLock;
					for( itr2 = m_caster->GetInRangeSetBegin( AutoLock ); itr2 != m_caster->GetInRangeSetEnd();)
					{
						Object *ot = *itr2;
						itr2++;
						if(!((ot)->IsCreature()) || !SafeUnitCast((ot))->isAlive() 
							|| ot->GetEntry() != 47649 
							|| SafeUnitCast((ot))->GetTopOwner() != m_caster )
							continue;
						Unit *ut = SafeUnitCast((ot));
						u_caster->CastSpell( ut, 78777, true);	//Wild Mushroom - dmg
						ut->CastSpell( ut, 92853, true);	//Wild Mushroom: Detonate Suicide
						//some spell targetting bug ?
						if( ut->isAlive() )
						{
							ut->SetUInt32Value( UNIT_FIELD_HEALTH, 1 );
							m_caster->DealDamage(ut, 666, 0);	//muhahaha, doomed !
						}
						SafeCreatureCast( ut )->Despawn( 1500, 0 );	//just in case the above code did not execute
					}
					m_caster->ReleaseInrangeLock();
				}
			}break;
		case 82174:	//Synapse Springs
			{
				if( p_caster )
				{
					if( p_caster->GetStat( STAT_INTELLECT ) > p_caster->GetStat( STAT_AGILITY ) && p_caster->GetStat( STAT_INTELLECT ) > p_caster->GetStat( STAT_STRENGTH ) )
						p_caster->CastSpell( p_caster, 96230, true );
					else if( p_caster->GetStat( STAT_AGILITY ) > p_caster->GetStat( STAT_STRENGTH ) )
						p_caster->CastSpell( p_caster, 96228, true );
					else
						p_caster->CastSpell( p_caster, 96229, true );
				}
			}break;
		case 97127:	//Blessing of Khaz'goroth
		case 96934:	//Blessing of Khaz'goroth
			{
				if( p_caster )
				{
					uint32 stacks = p_caster->CountAuraNameHash( SPELL_HASH_TITANIC_POWER, AURA_SEARCH_POSITIVE );
					if( stacks )
					{
						p_caster->RemoveAuraByNameHash( SPELL_HASH_TITANIC_POWER, 0, AURA_SEARCH_POSITIVE, MAX_PASSIVE_AURAS );
						uint32 SpellID = 0;
						if( p_caster->CalcRating( PLAYER_RATING_MODIFIER_MELEE_CRIT ) > p_caster->CalcRating( PLAYER_RATING_MODIFIER_MELEE_HASTE )
							&& p_caster->CalcRating( PLAYER_RATING_MODIFIER_MELEE_CRIT ) > p_caster->CalcRating( PLAYER_RATING_MODIFIER_MASTERY ) )
							SpellID = 96928;	//crit
						else if( p_caster->CalcRating( PLAYER_RATING_MODIFIER_MELEE_HASTE ) > p_caster->CalcRating( PLAYER_RATING_MODIFIER_MASTERY ) )
							SpellID = 96927;	//haste
						else
							SpellID = 96929;	//mastery

						SpellCastTargets targets( p_caster->GetGUID() );
						Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
						SpellEntry *sp = dbcSpell.LookupEntryForced( SpellID );
						spell->Init(p_caster, sp ,true, NULL);
						spell->forced_basepoints[0] = stacks * GetProto()->eff[0].EffectBasePoints;
						spell->pSpellId=SpellID;
						spell->prepare(&targets);
					}
				}
			}break;
		case 1535:	//Fire nova
			{
				if( u_caster )
				{
					InRangeSetRecProt::iterator itr2;
					m_caster->AquireInrangeLock(); //make sure to release lock before exit function !
					InrangeLoopExitAutoCallback AutoLock;
					for( itr2 = m_caster->GetInRangeSetBegin( AutoLock ); itr2 != m_caster->GetInRangeSetEnd();)
					{
						Object *ot = *itr2;
						itr2++;
						if(!((ot)->IsUnit()) || !SafeUnitCast((ot))->isAlive())
							continue;
						Unit *ut = SafeUnitCast((ot));
						Aura *a = ut->HasAuraWithNameHash( SPELL_HASH_FLAME_SHOCK, 0, AURA_SEARCH_NEGATIVE );
						if( a == NULL || a->GetCasterGUID() != m_caster->GetGUID() )
							continue;
						u_caster->CastSpell( ut, 8349, true);	//dmg. Ut will not receive dmg, only people around him
						ut->CastSpell( ut, 19823, true);	//visual ?
					}
					m_caster->ReleaseInrangeLock();
				}
			}break;
		case 77762:	//Lava Surge!
			{
				if( p_caster )
					p_caster->ClearCooldownForSpell( 51505 ); // lava burst
			}break;
		case 7812: //Sacrifice - voidwalker spell
			{
				if( u_caster )
				{
					int32 pct_health = u_caster->GetHealthPct();
					if( pct_health < damage )
						damage = MAX( 0, pct_health - 1);
					dealdamage tdmg;
					tdmg.base_dmg = damage * u_caster->GetMaxHealth() / 100;
					tdmg.pct_mod_final_dmg = 100;
					tdmg.DisableProc = true;
					tdmg.StaticDamage = true;
					u_caster->SpellNonMeleeDamageLog( u_caster, GetProto(), &tdmg, 1, i );
				}
			}break;
		case 1464:	//slam (warrior)
			{
				//slam 1
				if( u_caster )
					u_caster->CastSpell( unitTarget, 50782, true );
				//talent makes it do strike with offhand weapon
				if( p_caster && p_caster->HasAuraWithNameHash( SPELL_HASH_SINGLE_MINDED_FURY, 0, AURA_SEARCH_PASSIVE ) )
				{
					//only if both weapons are 1handed
					bool cancast = true;
					const uint32 not_good_mask = (1<<ITEM_SUBCLASS_WEAPON_TWOHAND_AXE)|(1<<ITEM_SUBCLASS_WEAPON_TWOHAND_MACE)|(1<<ITEM_SUBCLASS_WEAPON_POLEARM)|(1<<ITEM_SUBCLASS_WEAPON_TWOHAND_SWORD)|(1<<ITEM_SUBCLASS_WEAPON_STAFF);
					Item *it = p_caster->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_MAINHAND );
					if( it && ( ( 1 << it->GetProto()->SubClass) & not_good_mask ) )
						cancast = false;
					else
					{
						Item *it = p_caster->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_OFFHAND );
						if( it && ( ( 1 << it->GetProto()->SubClass) & not_good_mask ) )
							cancast = false;
					}
					if( cancast )
						u_caster->CastSpell( unitTarget, 50783, true );	//this should be the offhand version
				}
			}break;
		case 43987://Ritual of Refreshment
			{
				if( p_caster )
					p_caster->CastSpell( p_caster, 43985, true ); //spawns a portal that can be channeled. This is not needed if you have Glyph
			}break;
		case 43988://Ritual of Refreshment - one of the first tables
			{
				if( p_caster )
//					p_caster->CastSpell( p_caster, 43985, true );
//					p_caster->CastSpell( p_caster, 58661, true );
//					p_caster->CastSpell( p_caster, 92823, true );
					p_caster->CastSpell( p_caster, 92830, true );
			}break;
        case 42955: // Conjure Refreshment
        {
			if( p_caster )
			{
				uint32 entry;
				if( p_caster->getLevel() < 44 )
					entry = 65500;
//					entry = 92739;
				else if( p_caster->getLevel() < 54 )
					entry = 65515;
//					entry = 92799;
				else if( p_caster->getLevel() < 64 )
					entry = 65516;
//					entry = 92802;
				else if( p_caster->getLevel() < 74 )
					entry = 65517;
//					entry = 92805;
				else if( p_caster->getLevel() < 80 )
					entry = 43518;
//					entry = 74625;
				else if( p_caster->getLevel() < 85 )
					entry = 43523;
//					entry = 92822;
				else
					entry = 65499;
//					entry = 92727;
				Item *newItem = p_caster->GetItemInterface()->AddItemById( entry, 20, 0 );
				if( newItem )
					newItem->SetUInt64Value(ITEM_FIELD_CREATOR,p_caster->GetGUID());
			}
		}break;
        case 68996: // Worgen Transform Spell - two forms
        {
/*			if( m_caster->HasFlag( UNIT_FIELD_FLAGS_2, UNIT_FLAG2_WORGEN_TRANSFORM ) )
				m_caster->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_WORGEN_TRANSFORM);           
			else
				m_caster->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_WORGEN_TRANSFORM);           
			/**/
			if( m_caster->IsUnit() )
			{
				if( SafeUnitCast( m_caster )->HasAuraWithNameHash( SPELL_HASH_ALTERED_FORM, 0, AURA_SEARCH_POSITIVE ) )
					SafeUnitCast( m_caster )->RemoveAura( 97709, 0, AURA_SEARCH_ALL, MAX_PASSIVE_AURAS );	//this removes passive auras also
				else
					SafeUnitCast( m_caster )->CastSpell( SafeUnitCast( m_caster ), 97709, true );
			}
        }break; 
		//Readiness
		case 23989:
			{
				if( p_caster == NULL )
					return;
				uint32 insiginificant_cooldown = getMSTime() + 500;
				PlayerCooldownMap::iterator itr, itr2;
				for(uint32 i = 0; i < NUM_COOLDOWN_TYPES; ++i)
				{
					for( itr = p_caster->m_cooldownMap[i].begin(); itr != p_caster->m_cooldownMap[i].end(); )
					{
						itr2 = itr++;
						//clear all category cooldowns
						if( i == COOLDOWN_TYPE_CATEGORY )
						{
							p_caster->m_cooldownMap[i].erase( itr2 );
						}
						else 
						{
							SpellEntry * spe = dbcSpell.LookupEntry( itr2->first );
							if( spe->NameHash == SPELL_HASH_BESTIAL_WRATH || spe->NameHash == SPELL_HASH_READINESS )
								continue;
							//if we send too many packets the client chokes on it and does a 3 second lag :(
							if( itr2->second.ExpireTime > insiginificant_cooldown )
							{
								sStackWorldPacket( data, SMSG_CLEAR_COOLDOWN, 16);
								data << itr2->first << p_caster->GetGUID();
								p_caster->GetSession()->SendPacket(&data);
							}

							p_caster->m_cooldownMap[i].erase( itr2 );
						}
					}
				}
			}break;
		//some quest spell - Throw Ice
		case 56099:
			{
				InRangeSetRecProt::iterator itr,itr2;
				m_caster->AquireInrangeLock(); //make sure to release lock before exit function !
				InrangeLoopExitAutoCallback AutoLock;
				for( itr2 = m_caster->GetInRangeSetBegin( AutoLock ); itr2 != m_caster->GetInRangeSetEnd(); )
				{
					itr = itr2;
					itr2++;
					if( (*itr)->IsGameObject() == false || (*itr)->GetEntry() != 192124 )
						continue;
					
					GameObject *go = u_caster->GetMapMgr()->CreateGameObject(192127);
					if( go == NULL )
					{
						sLog.outDebug("Missing GO proto 192127");
						return;
					}
//					go->CreateFromProto( 192127, m_caster->GetMapId(), m_caster->GetPositionX() + RandomUInt( 3 ), m_caster->GetPositionY() + RandomUInt( 3 ), m_caster->GetPositionZ(), m_caster->GetOrientation() );
					go->CreateFromProto( 192127, (*itr)->GetMapId(), (*itr)->GetPositionX(), (*itr)->GetPositionY(), (*itr)->GetPositionZ(), (*itr)->GetOrientation() );
					go->SetUInt64Value( OBJECT_FIELD_CREATED_BY, m_caster->GetGUID() );
					go->SetInstanceID( m_caster->GetInstanceID() );
					go->PushToWorld( m_caster->GetMapMgr() );
					sEventMgr.AddEvent(go, &GameObject::ExpireAndDelete, EVENT_GAMEOBJECT_EXPIRE, 5*60*1000, 1,0);
					break;
				}
				m_caster->ReleaseInrangeLock();
			}break;
		//Well of Souls - boss spell
		case 68820:
			{
				//show jumping anymation ?
				u_caster->SendKnockbackPacket( cosf(m_caster->GetOrientation()), sinf(m_caster->GetOrientation()), sqrt( m_caster->GetDistance2dSq( unitTarget ) ), - 10 );
				//mobs insta teleport :P
				u_caster->SetPosition(unitTarget->GetPosition(), true);
				u_caster->CastSpellDelayed( unitTarget->GetGUID(), 68853, 2000, true );
			}break;
//		case SPELL_HASH_RESTORE_SOUL:	//revive players that we see
		case 72595:
		{
			InRangeSetRecProt::iterator itr,itr2;
			m_caster->AquireInrangeLock(); //make sure to release lock before exit function !
			InrangeLoopExitAutoCallback AutoLock;
			for( itr2 = m_caster->GetInRangeSetBegin( AutoLock ); itr2 != m_caster->GetInRangeSetEnd(); )
			{
				itr = itr2;
				itr2++;
				if( !((*itr)->IsPlayer()) || SafeUnitCast((*itr))->isAlive() == true )
					continue;
				Player *pt = SafePlayerCast((*itr));
				pt->SetMovement(MOVE_UNROOT);
				pt->ResurrectPlayer();
				pt->SetUInt32Value(UNIT_FIELD_HEALTH, pt->GetUInt32Value(UNIT_FIELD_MAXHEALTH) );
			}
			m_caster->ReleaseInrangeLock();
		}break;
	/*****************************************
	 *	Class Spells
	 *****************************************/	
		case 59566: //Earthen Power
		{
			if( unitTarget )
			{
				for(uint32 x=MAX_POSITIVE_AURAS;x<unitTarget->m_auras_neg_size;x++)
					if( unitTarget->m_auras[x] && unitTarget->m_auras[x]->GetSpellProto()->MechanicsType == MECHANIC_ENSNARED )
						unitTarget->m_auras[x]->Remove();
			}
		}break;
		case 8146: //Tremor Totem
		{
			if( unitTarget )
			{
//				for(uint32 x=MAX_POSITIVE_AURAS;x<unitTarget->m_auras_neg_size;x++)
				//some charm effects get to be positiv. We need to remove those too
				for(uint32 x=0;x<unitTarget->m_auras_neg_size;x++)
					if( unitTarget->m_auras[x] && 
						( unitTarget->m_auras[x]->GetSpellProto()->MechanicsType == MECHANIC_CHARMED 
						|| unitTarget->m_auras[x]->GetSpellProto()->MechanicsType == MECHANIC_ASLEEP 
						|| unitTarget->m_auras[x]->GetSpellProto()->MechanicsType == MECHANIC_FLEEING 
//						|| unitTarget->m_auras[x]->GetSpellProto()->MechanicsType == MECHANIC_HORRIFIED 
						)
						)
					{ 
							unitTarget->m_auras[x]->Remove();
					}
			}
		}break;
		case 64981: //Summon Random Vanquished Tentacle
		{
			//summon one of these tentacles
			uint32 tentaclespells[3]={64982,64984,64983};
            SpellCastTargets targets( m_caster->GetGUID() );
			SpellEntry *spellInfo = dbcSpell.LookupEntry( tentaclespells[ RandomUInt() % 3 ] );
			if (spellInfo)
			{
				Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
				spell->Init( m_caster, spellInfo ,true, NULL);
				spell->prepare(&targets);
			}
		}break;
		case 47948: //Pain and Suffering
		{
			// refresh the Shadow Word: pain ticks on target
			if( unitTarget )
			{
				Aura *taur = unitTarget->HasAuraWithNameHash( SPELL_HASH_SHADOW_WORD__PAIN, m_caster->GetGUID(), AURA_SEARCH_NEGATIVE );
				if( taur )
					taur->ResetDuration();
			}
		}break;
		case 46584: //Raise Dead
		{
			if( p_caster && i == 0 )
			{
				if( p_caster->HasAuraWithNameHash( SPELL_HASH_MASTER_OF_GHOULS, 0, AURA_SEARCH_PASSIVE ) )
					p_caster->CastSpell( p_caster, 52150, true );
				else
					p_caster->CastSpell( p_caster, 46585, true );
			}
/*			if(p_caster	!= NULL)
			{
				float x = p_caster->GetPositionX()+rand()%25;
				float y = p_caster->GetPositionY()+rand()%25;
				float z = p_caster->GetPositionZ();
				//let's try to find a nearby humanoid corpse target


				CreatureInfo * ci = CreatureNameStorage.LookupEntry(26125);
				CreatureProto * cp = CreatureProtoStorage.LookupEntry(26125);
				if( !ci || !cp )
					return;
					
				LocationVector *vec = new LocationVector(x,y,z);
				Pet *summon = objmgr.CreatePet(26125,p_caster->GeneratePetNumber() );
				summon->SetPower( POWER_TYPE_ENERGY, (uint32)100 );
				summon->SetPower( POWER_TYPE_ENERGY, (uint32)100 );
				if(p_caster->HasSpell(52143))
				{	
					summon->CreateAsSummon(26125, ci, NULL, p_caster, GetProto(), 6, 0, vec); // considered pet
					summon->AddSpell(dbcSpell.LookupEntry(47481), true); // Gnaw
					summon->AddSpell(dbcSpell.LookupEntry(47482), true); // Leap 
					summon->AddSpell(dbcSpell.LookupEntry(47484), true); // Huddle
					summon->AddSpell(dbcSpell.LookupEntry(47468), true); // Claw
				}
				else
					summon->CreateAsSummon(26125, ci, NULL, p_caster, GetProto(), 6, 120000, vec); // 2 min duration
					
				summon->CastSpell(summon,50142,true);
				delete vec;
				vec = NULL;
			}/**/
		}break;/**/
	case 49560:	//DK : death grip -> pulls target close to DK
	case 73325:	//Priest : Leap of Faith -> pulls target close to DK
		{
			if( !u_caster || !unitTarget || unitTarget == u_caster )
				break; //no conditions to make do our stuff

			//interrupt spell cast of the target
			unitTarget->InterruptSpell();

			float distt = sqrt( unitTarget->GetDistance2dSq( u_caster ) );

			if( distt + 2.0f <= unitTarget->GetModelHalfSize() )
				break;//he is close enough to us. Np need to pull

			float speed = 137.0f / 15.0f; //well this is just 1 example from blizz :p
			uint32 timetofly = (uint32)(distt * speed);
			float raise_mob_to_height = unitTarget->GetPositionZ() + distt * 250.f / 9.0f; //seems a lot 
//			float raise_mob_to_height = distt * 250.f / 9.0f; //seems a lot 
			float dist;
			if( unitTarget->GetModelHalfSize() > u_caster->GetModelHalfSize() )
				dist = unitTarget->GetModelHalfSize(); 
			else 
				dist = u_caster->GetModelHalfSize();
			float angle = (unitTarget->calcAngle(unitTarget->GetPositionX(), unitTarget->GetPositionY(), u_caster->GetPositionX(), m_caster->GetPositionY()) * float(M_PI) ) / 180.0f;
			angle += (float)(M_PI); // is it me or the fromula is putting mob behind me all the time ? 
//printf("!!!raise mob height is %f and angle is %f\n",raise_mob_to_height,angle);
			float x = u_caster->GetPositionX()+dist * cosf(angle);
			float y = u_caster->GetPositionY()+dist * sinf(angle);
			float z = u_caster->GetPositionZ()+2.0f;
			unitTarget->SetPosition(x,y,z,unitTarget->GetOrientation(), true );

/*
{SERVER} Packet: (0xA65D) SMSG_MONSTER_MOVE PacketSize = 58 TimeStamp = 31093198
F7 65 44 10 C1 6F 30 F1 
00 
6D 67 08 45 
D3 E8 B2 C5 
A0 E1 D3 42 
F0 DF 3F 01 
00 
00 18 00 A0 
60 01 00 00 
D9 21 01 42 
00 00 00 00 
01 00 00 00 
95 41 09 45 
B0 44 B2 C5 
44 46 DF 42 
14333 - death grip
{SERVER} Packet: (0xAAE6) SMSG_MONSTER_MOVE PacketSize = 58 TimeStamp = 538250
F7 C6 01 0B EC 0E 30 F7
00 
49 96 06 45 2153.392
B1 AF 5C C5 -3530.98
92 56 40 42 
F7 2E 99 03 
00 
00 03 00 10 - flags
62 01 00 00 - time
BD 5A FF 41 - somefloat maybe xyz speed ? 31.919305801392 
00 00 00 00 
01 00 00 00 
69 3E 06 45 2147.90
D0 BB 5D C5 -3547.73
15 9C 3B 42 
*/
			WorldPacket data(SMSG_MONSTER_MOVE, 60);
			data << unitTarget->GetNewGUID();
			data << uint8(0);
			data << unitTarget->GetPositionX() << unitTarget->GetPositionY() << unitTarget->GetPositionZ();
			data << getMSTime();
			data << uint8(0);
			data << (uint32)( MONSTER_MOVE_FLAG_DEATHGRIP ); //make mob seem to jump into air and fall infront of us
			data << timetofly; 
			data << raise_mob_to_height; //not really sure about this one
			data << uint32( 0 );
			data << uint32(1);	  // 1 waypoint
			data << x << y << z;
			unitTarget->SendMessageToSet( &data, true );
			if( playerTarget )
			{
				playerTarget->SpeedCheatDelay( distt * 1000 / speed + MIN( 5000, playerTarget->GetSession()->GetLatency() ) + 1000 );
				//somone said death grip does not remove mount
//				if( playerTarget->m_MountSpellId )
//					playerTarget->RemoveAura(playerTarget->m_MountSpellId);
			}
		}break;
	/*************************
	 * MAGE SPELLS
	 *************************
	 * IDs:
	 *	11189 Frost Warding   -	RANK 1,		STATUS: DONE
	 *  28332 Frost Warding   -	RANK 2,		STATUS: DONE
	 *  11094 Molten Shields  -	RANK 1,		STATUS: DONE
	 *  13043 Molten Shields  -	RANK 2,		STATUS: DONE

	 *  --------------------------------------------
	 *************************/
	
	/*
		Frost Warding
		Increases the armor and resistances given by your Frost Armor and Ice Armor spells by X%.  In addition, gives your Frost Ward a X% chance to reflect Frost spells and effects while active.
		
		Effect #1	Apply Aura: Add % Modifier (8)

		Effect #2	Apply Aura: Dummy
		11189, 28332
	*/
	case 11189:
	case 28332:
		{
			if(!unitTarget)
				break;
			CommitPointerListNode<struct ReflectSpellSchool> *itr;
			unitTarget->m_reflectSpellSchool.BeginLoop();
			for(itr = unitTarget->m_reflectSpellSchool.begin(); itr != unitTarget->m_reflectSpellSchool.end();itr = itr->Next() )
				if(GetProto()->Id == itr->data->spellId)
					unitTarget->m_reflectSpellSchool.SafeRemove(itr,1);
			unitTarget->m_reflectSpellSchool.EndLoopAndCommit();

			ReflectSpellSchool *rss = new ReflectSpellSchool;
			rss->chance = GetProto()->procChance;
			rss->spellId = GetProto()->Id;
			rss->require_aura_hash = SPELL_HASH_FROST_WARD; 
			rss->school = SCHOOL_FROST;				

			unitTarget->m_reflectSpellSchool.push_front(rss);
		}break;
	/*
		Molten Shields	Rank 2
		Causes your Fire Ward to have a 20% chance to reflect Fire spells while active. In addition, your Molten Armor has a 100% chance to affect ranged and spell attacks.
		Effect #1	Apply Aura: Dummy
		11904,13043
	*/
	case 11904:
	case 13043:
		{
			if(!unitTarget)
				break;
			CommitPointerListNode<struct ReflectSpellSchool> *itr;
			unitTarget->m_reflectSpellSchool.BeginLoop();
			for(itr = unitTarget->m_reflectSpellSchool.begin(); itr != unitTarget->m_reflectSpellSchool.end();itr = itr->Next() )
				if(GetProto()->Id == itr->data->spellId)
					unitTarget->m_reflectSpellSchool.SafeRemove(itr,1);
			unitTarget->m_reflectSpellSchool.EndLoopAndCommit();
 
			ReflectSpellSchool *rss = new ReflectSpellSchool;
			rss->chance = GetProto()->eff[0].EffectBasePoints;
			rss->spellId = GetProto()->Id;
			rss->require_aura_hash = SPELL_HASH_FIRE_WARD; 
			rss->school = SCHOOL_FIRE;		

			unitTarget->m_reflectSpellSchool.push_front(rss);
		}break;
	//heroic fury clear intercept cooldown
	case 60970:
		{
			if(p_caster && p_caster->HasSpell(20252))
				p_caster->ClearCooldownForSpell(20252);
			if( u_caster )
				u_caster->RemoveAurasMovementImpairing();
		}break;

	/*************************
	 * ROGUE SPELLS
	 *************************
	 * IDs:
	 * 14185 Preparation		STATUS: DONE
	 *  --------------------------------------------
	 * 35729 Cloak of Shadows	STATUS: DONE
	 *  --------------------------------------------
	 *************************/

	case 5938: //shiv
	{
		if( p_caster == NULL || unitTarget == NULL )
			return;

		//ofhand shiv attack
		p_caster->CastSpell(unitTarget->GetGUID(),5940,true);	

		//apply offhand poison on target
		if( p_caster->GetItemInterface() )
		{
			Item *it = p_caster->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_OFFHAND );
			if( it == NULL )
				return;
		
			EnchantmentInstance * ench = it->GetEnchantment( 1 ); // temp enchantment slot
			if(ench)
			{
				EnchantEntry* Entry = ench->Enchantment;
				for( uint32 c = 0; c < 3; c++ )
				{
					if( Entry->type[c] && Entry->spell[c] )
					{
						SpellEntry *sp = dbcSpell.LookupEntry( Entry->spell[c] );
						if( sp == NULL )
							return;
					
						if( sp->c_is_flags & SPELL_FLAG_IS_POISON )
						{
							p_caster->CastSpell(unitTarget->GetGUID(),Entry->spell[c], true);
						}
					}
				}
			}
		}
		//remove 1 enrage effect from target
		//can this be a boss encounter ?
		if( unitTarget->IsPlayer() )
			unitTarget->RemoveAllAurasByMechanic( MECHANIC_ENRAGED, 1, true );

	} break;
	//Sword and Board
	case 50227:
		{
			if( !p_caster )
				return;

			uint32 ClearSpellId[8] = { 23922,23923,23924,23925,25258,30356,47487,47488 };

			for(i = 0; i < 8; ++i)
				if( p_caster->HasSpell( ClearSpellId[i] ) )
					p_caster->ClearCooldownForSpell( ClearSpellId[i] );
		}break;
	/*
		Preparation
		When activated, this ability immediately finishes the cooldown on your Evasion, Sprint, Vanish, Cold Blood, Shadowstep and Premeditation abilities.		
		
		Effect	Dummy
	*/
	case 14185:
		{
			if( !p_caster )
				return;

			uint32 ClearSpellId[11] =
			{
			5277,  /* Evasion - Rank 1 */
			26669, /* Evasion - Rank 2 */
			2983,  /* Sprint  - Rank 1 */
			8696,  /* Sprint  - Rank 2 */
			11305, /* Sprint  - Rank 3 */
			1856,  /* Vanish  - Rank 1 */
			1857,  /* Vanish  - Rank 2 */
			26889, /* Vanish  - Rank 3 */
			14177, /* Cold Blood       */
			14183, /* Premeditation    */
			36554  /* Shadowstep       */
			};

			for(i = 0; i < 11; ++i)
			{
				if( p_caster->HasSpell( ClearSpellId[i] ) )
					p_caster->ClearCooldownForSpell( ClearSpellId[i] );
			}
		}break;
	/*
		Cloak of Shadows
		Instantly removes all existing harmful spell effects and increases your chance to resist all spells by 90% for 5 sec.  Does not remove effects that prevent you from using Cloak of Shadows.
		
		Effect #1	Apply Aura: Mod Attacker Spell Hit Chance (126)
			Value: -90
		Effect #2	Trigger Spell
			Spell #35729 <--- THIS SPELL
	*/
	case 35729:
	case 31224:	//changed by zack since trigger spell is not present anymore
		{
			if( !unitTarget || !unitTarget->isAlive())
				return;

			Aura * pAura;
			for(uint32 i = MAX_POSITIVE_AURAS; i < MAX_NEGATIVE_AURAS1(unitTarget); ++i)
			{
				pAura = unitTarget->m_auras[i];
				if( pAura != NULL && !pAura->IsPassive() && !pAura->IsPositive() && !(pAura->GetSpellProto()->Attributes & ATTRIBUTES_IGNORE_INVULNERABILITY) 
					&& pAura->GetSpellProto()->School != 0	//bleed effects are fizical
					)
				{
					pAura->Remove();
				}
			}
		}break;

	/*************************
	 * PALADIN SPELLS
	 *************************
	 * IDs:
	 * 31789 Righteous Defense		STATUS: DONE
	 *  --------------------------------------------
	 * 18350 illumination			STATUS: DONE
	 *  --------------------------------------------
	 *************************/

	/*
		Righteous Defense
		Come to the defense of a friendly target, commanding up to 3 enemies attacking the target to attack the Paladin instead.
		
		Effect #1	Dummy
			Radius: 5 yards

		Effect #2	Trigger Spell
			Spell #31980
	*/
	case 31789:
		{
			//we will try to lure 3 enemies from our target
			if(!unitTarget || !u_caster)
				break;
			Unit *targets[3];
			int targets_got=0;
			unitTarget->AquireInrangeLock();
			InrangeLoopExitAutoCallback AutoLock;
			for(InRangeSetRecProt::iterator itr = unitTarget->GetInRangeSetBegin( AutoLock ), i2; itr != unitTarget->GetInRangeSetEnd(); )
			{
				i2 = itr++;
				// don't add objects that are not units and that are dead
				if((*i2)->GetTypeId()!= TYPEID_UNIT || !((Unit*)(*i2))->isAlive())
					continue;
		        
				Creature *cr=SafeCreatureCast(*i2);
				if(cr->GetAIInterface()->GetNextTarget()==unitTarget)
					targets[targets_got++]=cr;
				if(targets_got==3)
					break;
			}
			unitTarget->ReleaseInrangeLock();
			for(int i=0;i<targets_got;i++)
			{
				//set threat to this target so we are the msot hated
				uint32 threat_to_him = targets[i]->GetAIInterface()->getThreatByPtr( unitTarget );
				uint32 threat_to_us = targets[i]->GetAIInterface()->getThreatByPtr(u_caster);
				int threat_dif = threat_to_him - threat_to_us;
				if(threat_dif>0)//should not happen
					targets[i]->GetAIInterface()->modThreatByPtr(u_caster,threat_dif);
				targets[i]->GetAIInterface()->AttackReaction(u_caster,1,0);
				targets[i]->GetAIInterface()->SetNextTarget(u_caster);
			}
		}break;
	/*
		Illumination
		After getting a critical effect from your Flash of Light, Holy Light, or Holy Shock heal spell, gives you a X% chance to gain mana equal to 60% of the base cost of the spell.
		
		Effect #1	Apply Aura: Proc Trigger Spell
			Proc chance: 20%
			Spell #18350 <-- THIS SPELL	
		Effect #2	Apply Aura: Override Class Scripts (2689)
			Value: 60
	*/
	case 18350:
		{
			switch( m_triggeredByAura==NULL ? pSpellId : m_triggeredByAura->GetSpellId() )
			{
/*			case 20210:
			case 20212:
			case 20213:
			case 20214:
			case 20215:
				{
					if(!p_caster) 
						return;
					SpellEntry * sp = p_caster->last_heal_spell ? p_caster->last_heal_spell : GetProto();
					p_caster->Energize( p_caster, 20272, 60 * u_caster->GetUInt32Value( UNIT_FIELD_BASE_MANA ) * sp->PowerEntry.ManaCostPercentage / 10000, POWER_TYPE_MANA );
				}break; */
			case 38443:
				{
					// Shaman - Skyshatter Regalia - Two Piece Bonus
					// it checks for earth, air, water, fire totems and triggers Totemic Mastery spell 38437.
					if(!p_caster) return;
					if(p_caster->m_TotemSlots[0] && p_caster->m_TotemSlots[1] && p_caster->m_TotemSlots[2] && p_caster->m_TotemSlots[3])
					{
						Aura *aur = AuraPool.PooledNew( __FILE__, __LINE__ );
						aur->Init(dbcSpell.LookupEntry(38437), 5000, p_caster, p_caster);
						for( uint32 i=0; i<3; i++ ) 
							aur->AddMod( aur->GetSpellProto()->eff[i].EffectApplyAuraName, aur->GetSpellProto()->eff[i].EffectBasePoints+1, aur->GetSpellProto()->eff[i].EffectMiscValue, i, forced_pct_mod_cur );
						p_caster->AddAura(aur);
					}
				}break;
			default:
				return;
			}
		}break;
	/*************************
	 * PRIEST SPELLS
	 *************************
	 * IDs:
	 * 34433 Shadowfiend	STATUS: ToDo
	 *  --------------------------------------------
	 *************************/
	/*
		Creates a shadowy fiend to attack the target.  Caster receives mana when the Shadowfiend deals damage.  Lasts 15 sec.
		Effect #1	Summon (19668)
		Effect #2	Trigger Spell
					Spell #41967 <--- THIS SPELL
	*/
	/*case 41967:
		{
		}break;
	*/

	/*************************
	 * SHAMAN SPELLS
	 *************************
	 * IDs:
	 * NO SPELLS
	 *  --------------------------------------------
	 *************************/


	case 25228:
		{
			if(!u_caster || !u_caster->isAlive() || !unitTarget || !unitTarget->isAlive())
				return;
			uint32 pet_dmg = this->forced_basepoints[0]*20/100;
			if( pet_dmg )
			{
				unitTarget->ModUnsigned32Value(UNIT_FIELD_HEALTH,pet_dmg);
				unitTarget->DealDamage(u_caster,pet_dmg,25228,false);
			}
		}break;
	//Demonic Circle : teleport
	case 48020:
		{
			if( !p_caster )
				return;
		
			GameObject *DemonicCircle = p_caster->GetMapMgr()->GetGameObject( p_caster->m_ObjectSlots[0] );
			if( DemonicCircle )
			{
				if( DemonicCircle->GetDistance2dSq( p_caster ) <= 40*40 )
				{
					p_caster->SafeTeleport( DemonicCircle->GetMapId(), DemonicCircle->GetInstanceID(), DemonicCircle->GetPosition());
					p_caster->RemoveAllAurasByMechanic( MECHANIC_ENSNARED, 0xFFFF, false );
				}
				else
					SendCastResult(SPELL_FAILED_OUT_OF_RANGE);
			}
			else
			{
				p_caster->m_ObjectSlots[0] = 0;
				p_caster->RemoveAuraByNameHash( SPELL_HASH_DEMONIC_CIRCLE__SUMMON, 0, AURA_SEARCH_POSITIVE );
				sStackWorldPacket(data, SMSG_AURA_UPDATE, 20 );
				data << p_caster->GetNewGUID();
				data << uint8( 99 );	//slot, it is fictional !
				data << uint32( 0 );	//spell id = clear
				p_caster->GetSession()->SendPacket( &data );
				SendCastResult(SPELL_FAILED_BAD_TARGETS);
			}
		}break;

	/*************************
		Non-Class spells
		- Done
	 *************************/
	/*
		Poultryizer
		Turns the target into a chicken for 15 sec.   Well, that is assuming the transmogrification polarity has not been reversed...
	*/
	case 30507:
		{
			if( !unitTarget || !unitTarget->isAlive())
				return;

			u_caster->CastSpell(unitTarget->GetGUID(),30501,true);
		}break;
	/*
		Six Demon Bag
		Blasts enemies in front of you with the power of wind, fire, all that kind of thing!
	*/
	case 14537:
		{
			if( !unitTarget || !unitTarget->isAlive())
				return;

			uint32 ClearSpellId[6] = { 8401,8408,930,118,1680,10159 };
			int min = 1;
			uint32 effect = min + int( ((6-min)+1) * rand() / (RAND_MAX + 1.0) );
			
			u_caster->CastSpell(unitTarget, ClearSpellId[effect] ,true);
		}break;

	case 30427: // Extract Gas
		{
			bool check = false;
			uint32 cloudtype = 0;
			Creature *creature = 0;

			if(!p_caster)
				return;

			p_caster->AquireInrangeLock();
			InrangeLoopExitAutoCallback AutoLock;
			for(InRangeSetRecProt::iterator i = p_caster->GetInRangeSetBegin( AutoLock ); i != p_caster->GetInRangeSetEnd(); ++i)
			{
				if((*i)->GetTypeId() == TYPEID_UNIT)
				{
					creature=SafeCreatureCast((*i));
					cloudtype=creature->GetEntry();
					if(cloudtype == 24222 || cloudtype == 17408 || cloudtype == 17407 || cloudtype == 17378)
					{
						if(p_caster->GetDistance2dSq((*i)) < 400)
						{
							p_caster->SetSelection(creature->GetGUID());
							check = true;
							break;
						}
					}
				}
			}
			p_caster->ReleaseInrangeLock();
			if(check)
			{
				uint32 item, count = 0;
				item = count = 3+(rand()%3);
			
				if (cloudtype==24222) item=22572;//-air
				if (cloudtype==17408) item=22576;//-mana
				if (cloudtype==17407) item=22577;//-shadow
				if (cloudtype==17378) item=22578;//-water

				Item *add = p_caster->GetItemInterface()->FindItemLessMax(item, count, false);
				if (!add)
				{
					ItemPrototype* proto = ItemPrototypeStorage.LookupEntry(item);
					SlotResult slotresult;

					slotresult = p_caster->GetItemInterface()->FindFreeInventorySlot(proto);
					if(!slotresult.Result)
					{
						p_caster->GetItemInterface()->BuildInventoryChangeError(NULL, NULL, INV_ERR_INVENTORY_FULL);
						return;
					}
					Item * it=objmgr.CreateItem(item,p_caster);  
					it->SetUInt32Value( ITEM_FIELD_STACK_COUNT, count);
					p_caster->GetItemInterface()->SafeAddItem(it,slotresult.ContainerSlot, slotresult.Slot);
					creature->Despawn(3500,creature->GetProto()->RespawnTime);
				}
				else
				{
					add->SetCount(add->GetUInt32Value(ITEM_FIELD_STACK_COUNT) + count);
					add->m_isDirty = true;
					creature->Despawn(3500,creature->GetProto()->RespawnTime);
				}
			}
		}break;

	//curse of agony(18230) = periodic damage increased in 
	//flag 2031678
/*	case 34120:
		{//steady shot
		if(unitTarget)
		if(unitTarget->IsDazed())
		{
//			u_caster->SpellNonMeleeDamageLog(unitTarget,spellId,damage,false);
			u_caster->SpellNonMeleeDamageLog(unitTarget,spellId,damage,pSpellId==0);
		}
		
		}break;*/
	case 1454://life tap
	case 1455:
	case 1456:
	case 11687:
	case 11688:
	case 11689:
	case 27222:
	case 57946:
		{//converts base+1 points of health into mana
		if(!p_caster || !playerTarget)
			return;

//		uint32 spirit_pct = GetProto()->RankNumber * 100;
//		if( spirit_pct > 300 ) 
//			spirit_pct = 300;
//		uint32 damage = ( ( ( GetProto()->eff[i].EffectBasePoints + 1 ) * ( 100 + playerTarget->m_lifetapbonus ) ) / 100 ) + spirit_pct * playerTarget->GetUInt32Value( UNIT_FIELD_SPIRIRT ) / 100;
		int32 damage = GetProto()->eff[2].EffectBasePoints;
		damage = damage * playerTarget->GetUInt32Value( UNIT_FIELD_MAXHEALTH ) / 100;
		if( damage >= (int32)playerTarget->GetUInt32Value( UNIT_FIELD_HEALTH ) )
			damage = MAX((int32)playerTarget->GetInt32Value( UNIT_FIELD_HEALTH ) - 5, 0 );
		if( damage == 0 )
			return;
		//this is probably not blizzlike. But noob helper for sure. You can spam lifetap without killing yourself without a reason
		int32 can_store_mana = playerTarget->GetMaxPower(  POWER_TYPE_MANA ) - playerTarget->GetPower(  POWER_TYPE_MANA );
		int32 m_lifetapbonus = 0;
		int64 *p = (int64*)p_caster->GetExtension( EXTENSION_ID_LIFETAP_BONUS );
		if( p )
			m_lifetapbonus = (int32)(*p);
		int32 ManaConversionPCT = GetProto()->eff[1].EffectBasePoints + m_lifetapbonus;
		int32 requires_health = can_store_mana * 100 / ManaConversionPCT;
		damage = MIN( requires_health, damage );
		int32 restored_mana = damage * ManaConversionPCT / 100;
		int32 abs = playerTarget->ResilianceAbsorb( damage, GetProto(), m_caster );
		damage -= abs;
		//!!!! eeek, instead abs dmg we are passing mana restored :(
		p_caster->HandleProc( PROC_ON_SPELL_HIT_DOT, p_caster, GetProto(), &damage, &restored_mana ); 
		p_caster->DealDamage( playerTarget, damage, spellId );
		p_caster->Energize( playerTarget, spellId, restored_mana, POWER_TYPE_MANA, 0 );
		}break;
	case 974:
	case 32593:
	case 32594:
		{
			if(!pSpellId) return;
			SpellEntry *spellInfo = dbcSpell.LookupEntry(pSpellId);
			if(!spellInfo) return;
			uint32 heal32 = CalculateEffect(i,u_caster);
			unitTarget=u_caster; // Should heal caster :p
			if(heal32)
			{
//				Heal(heal32);
				u_caster->SpellHeal( unitTarget, GetProto(), damage, pSpellId==0, static_dmg[i] != 0, false, 1, forced_pct_mod_cur, i );
			}
		}break;
	case 28730: //Arcane Torrent (Mana + interrupt)
		{
			// for each mana tap, gives you (2.17*level+9.136) mana
			if( !unitTarget || !p_caster) 
				return;

			//only non player targets get their spells canceled
			if( unitTarget->isCasting() )
				unitTarget->InterruptSpell();

			if(!unitTarget->isAlive() || unitTarget->getClass()==WARRIOR || unitTarget->getClass() == ROGUE)
				return;

			uint32 count = 0;
			for(uint32 x = 0; x < MAX_NEGATIVE_AURAS1(unitTarget); ++x)
			{
				if(unitTarget->m_auras[x] && unitTarget->m_auras[x]->GetSpellId() == 28734)
				{
					unitTarget->m_auras[x]->Remove();
					++count;
				}
			}

			uint32 gain = (uint32)(count * (2.17*p_caster->getLevel()+9.136));
			p_caster->Energize( unitTarget, 28730, gain, POWER_TYPE_MANA, 0 );
		}break;
	case 39610://Mana Tide
		{
			if(unitTarget == NULL || unitTarget->IsDead() || unitTarget->getClass() == WARRIOR || unitTarget->getClass() == ROGUE)
 				return;
 			uint32 gain = (uint32) (unitTarget->GetMaxPower( POWER_TYPE_MANA )*0.06);
			unitTarget->Energize( unitTarget, 16191, gain, POWER_TYPE_MANA, 0 );
		}break;
	case 20577:// Cannibalize
		{
/*			if(!p_caster)
				return;
			bool check = false;
			float rad = GetRadius(i);
			rad *= rad;
			p_caster->AquireInrangeLock();
			InrangeLoopExitAutoCallback AutoLock;
			for(InRangeSet::iterator i = p_caster->GetInRangeSetBegin( AutoLock ); i != p_caster->GetInRangeSetEnd(); ++i)
			{
				if( (*i)->IsUnit() )
				{
					if(SafeCreatureCast((*i))->getDeathState() == CORPSE)
					{
						CreatureInfo *cn = SafeCreatureCast((*i))->GetCreatureInfo();
							if(cn && (cn->Type == HUMANOID || cn->Type == UNDEAD) )
							{
								if(p_caster->GetDistance2dSq((*i)) < rad)
								{
									check = true;
									break;
								}
							}
						
					}
				}
			}
			p_caster->ReleaseInrangeLock();
			if(check)
			{
				p_caster->cannibalize = true;
				p_caster->cannibalizeCount = 0;
				sEventMgr.AddEvent(p_caster, &Player::EventCannibalize, uint32(7), EVENT_CANNIBALIZE, 2000, 5,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
				p_caster->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_CANNIBALIZE);
			} */
//			if( unitTarget->getDeathState() == CORPSE ) //this is correct, but it will be reported countless times by noobs
			if( unitTarget->isAlive() == false )
				u_caster->CastSpell( unitTarget, 20578, false );
		}break;
	case 23074:// Arcanite Dragonling
	case 23075:// Mithril Mechanical Dragonling
	case 23076:// Mechanical Dragonling
	case 23133:// Gnomish Battle Chicken
		{
			uint32 spell_id;
			switch(GetProto()->Id)
			{
                case 23074: spell_id = 19804; break;
                case 23075: spell_id = 12749; break;
                case 23076: spell_id =  4073; break;
                case 23133: spell_id = 13166; break;
				default: 
					return;
			}
			u_caster->CastSpell(u_caster,spell_id,true);
		}break;
	case 23725:// Gift of Life
		{
			if(!playerTarget)
				break;

			SpellCastTargets tgt;
			tgt.m_unitTarget = playerTarget->GetGUID();
			SpellEntry * inf =dbcSpell.LookupEntry(23782);
			Spell * spe = SpellPool.PooledNew( __FILE__, __LINE__ );
			spe->Init(u_caster,inf,true,NULL);
			spe->prepare(&tgt);

		}break;
	case 12975:// Last Stand
		{
			if(!playerTarget)
				break;
			SpellCastTargets tgt;
			tgt.m_unitTarget = playerTarget->GetGUID();
			SpellEntry * inf =dbcSpell.LookupEntry(12976);
			Spell * spe = SpellPool.PooledNew( __FILE__, __LINE__ );
			spe->Init(u_caster,inf,true,NULL);
			spe->prepare(&tgt);
		}break;
	/*************************
		Non-Class spells
		- ToDo
	 *************************/
	case 6668:// Red Firework
		{
			// Shoots a firework into the air that bursts into a thousand red stars
		}break;
	case 8344:// Universal Remote
		{
			//FIXME:Allows control of a mechanical target for a short time.  It may not always work and may just root the machine or make it very very angry.  Gnomish engineering at its finest.
		}break;
	case 9976:// Polly Eats the E.C.A.C.
		{
			//FIXME: Don't know what this does
		}break;
	case 10137:// Fizzule's Whistle
		{
			//FIXME:Blow on the whistle to let Fizzule know you're an ally
			//This item comes after a finish of quest at venture co.
			//You must whistle this every time you reach there to make Fizzule
			//ally to you.
		}break;
	case 11540:// Blue Firework
		{
			//Shoots a firework into the air that bursts into a thousand blue stars
		}break;
	case 11541:// Green Firework
		{
			//Shoots a firework into the air that bursts into a thousand green stars
		}break;
	case 11542:// Red Streaks Firework
		{
			//Shoots a firework into the air that bursts into a thousand red streaks
		}break;
	case 11543:// Red, White and Blue Firework
		{
			//Shoots a firework into the air that bursts into red, white and blue stars
		}break;
	case 11544:// Yellow Rose Firework
		{
			//Shoots a firework into the air that bursts in a yellow pattern
		}break;
	case 12151:// Summon Atal'ai Skeleton
		{
			//8324	Atal'ai Skeleton

			//FIXME:Add here remove in time event
		}break;
	case 13006:// Shrink Ray
		{
			//FIXME:Schematic is learned from the gnomish engineering trainer. The gnomish/gobblin engineering decision is made when you are lvl40+ and your engineering is 200+. Interestingly, however, when this item fails to shrink the target, it can do a variety of things, such as...
			//-Make you bigger (attack power +250)
			//-Make you smaller (attack power -250)
			//-Make them bigger (same effect as above)
			//-Make your entire party bigger
			//-Make your entire party smaller
			//-Make every attacking enemy bigger
			//-Make ever attacking enemy smaller
			//Works to your advantage for the most part (about 70% of the time), but don't use in high-pressure situations, unless you're going to die if you don't. Could tip the scales the wrong way.
			//Search for spells of this


			//13004 - grow <- this one
			//13010 - shrink <-this one
			//
		}break;
	case 13180:// Gnomish Mind Control Cap
		{
			// FIXME:Take control of humanoid target by chance(it can be player)
		}break;
	case 13278:// Gnomish Death Ray
		{
			// FIXME:The devices charges over time using your life force and then directs a burst of energy at your opponent
			//Drops life			
		}break;
	case 13280:// Gnomish Death Ray
		{
			//FIXME: Drop life
		}break;
	case 17816:// Sharp Dresser
		{
			//Impress others with your fashion sense
		}break;
	case 21343:// Snowball
		{
		}break;
	case 23645:// Hourglass Sand
		{
			//Indeed used at the Chromo fight in BWL. Chromo has a stunning debuff, uncleansable, unless you have hourglass sand. This debuff will stun you every 4 seconds, for 4 seconds. It is resisted a lot though. Mage's and other casters usually have to do this fight with the debuff on, healers, tanks and hunters will get some to cure themselves from the debuff
		}break;
	case 24325:// Pagle's Point Cast - Create Mudskunk Lure
		{
			//FIXME:Load with 5 Zulian Mudskunks, and then cast from Pagle's Point in Zul'Gurub
		}
	case 24392:// Frosty Zap
		{
			//FIXME:Your Frostbolt spells have a 6% chance to restore 50 mana when cast.
			//damage == 50
		}break;
	case 25822:// Firecrackers
		{
			//FIXME:Find firecrackers
		}break;
	case 26373:// Lunar Invititation
		{
			//FIXME: Teleports the caster from within Greater Moonlight
		}break;
	case 26374:// Elune's Candle
		{
			//FIXME:Shoots a firework at target
		}break;
	case 26889:// Give Friendship Bracelet
		{
			//Give to a Heartbroken player to cheer them up
			//laugh emote
		}break;
	case 27662:// Throw Cupid's Dart
		{
			//FIXME:Shoot a player, and Kwee Q. Peddlefeet will find them! (Only works on players with no current critter pets.)
		}break;
	case 28414:// Call Ashbringer
		{
			//http://www.thottbot.com/?i=53974
		}break;
	case 28806:// Toss Fuel on Bonfire
		{
			//FIXME:Dont know what this dummy does
		}break;
	case 7669:// Bethor's Potion
		{
			// related to Hex of Ravenclaw,
			// its a dispell spell.
			//FIXME:Dont know whats the usage of this dummy
		}break;
	case 8283:// Snufflenose Command
		{
			//FIXME:Quest Blueleaf Tubers
			//For use on a Snufflenose Gopher
		}break;
	case 8913:// Sacred Cleansing
		{
			//FIXME:Removes the protective enchantments around Morbent Fel
			//Quest Morbent Fel
		}break;
	case 9962://Capture Treant
		{
			//Quest Treant Muisek 
		}break;
	case 10113:// Flare Gun's flare
		{
			//FIXME:Quest Deep Cover
			//1543 may need to cast this
			//2 flares and the /salute
		}break;
	case 10617:// Release Rageclaw
		{
			//Quest Druid of the Claw
			//Use on the fallen body of Rageclaw
		}break;
	case 11402:// Shay's Bell
		{
			//FIXME:Quest Wandering Shay
			//Ring to call Shay back to you
		}break;
	case 11548:// Summon Spider God
		{
			//FIXME:Quest Summoning Shadra  (Elite)
			//Use at the Shadra'Alor Altar to summon the spider god
		}break;
	case 11610:// Gammerita Turtle Camera
		{
			//Quest The Super Snapper FX 
		}break;
	case 11886:// Capture Wildkin
		{
			//Quest Testing the Vessel
			//Shrink and Capture a Fallen Wildkin
		}break;
	case 11887:// Capture Hippogryph
		{
			//FIXME:Same with 11888
			//Quest Hippogryph Muisek 
		}break;
	case 11888:// Capture Faerie Dragon
		{
			//FIXME:Check Faerie Dragon Muisek is killed or not if its killed update quest
			//And allow create of fearie Dragon which is effect 1
			//Quest: Faerie Dragon Muisek
		}break;
	case 11889:// Capture Mountain Giant
		{
			//FIXME:Same with 11888
			//Quest: Mountain Giant Muisek
		}break;
	case 12189:// Summon Echeyakee
		{
			//3475	Echeyakee

			//FIXME:Quest Echeyakee
		}break;
	case 12283:// Xiggs Signal Flare
		{
			//Quest Signal for Pickup
			//To be used at the makeshift helipad in Azshara. It will summon Pilot Xiggs Fuselighter to pick up the tablet rubbings
		}break;
	case 12938:// Fel Curse
		{
			//FIXME:Makes near target killable(servants of Razelikh the Defiler)
		}break;
	case 14247:// Blazerunner Dispel
		{
			//FIXME:Quest Aquementas and some more
		}break;
	case 14250:// Capture Grark
		{
			//Quest Precarious Predicament
		}break;
	case 14813:// Rocknot's Ale
		{
			//you throw the mug
			//and the guy gets pissed well everyone gets pissed and he crushes the door so you can get past
			//maybe after like 30 seconds so you can get past.  but lke I said I have never done it myself 
			//so i am not 100% sure what happens.
		}break;
	case 15991://Revive Ringo
		{
			//Quest A Little Help From My Friends 
			//Revive Ringo with water
		}break;
	case 15998:// Capture Worg Pup
		{
			//FIXME:Ends Kibler's Exotic Pets  (Dungeon) quest
		}break;
	case 16031:// Releasing Corrupt Ooze
		{
			//FIXME:Released ooze moves to master ooze and "Merged Ooze Sample"
			//occurs after some time.This item helps to finish quest
		}break;
	case 16378:// Temperature Reading
		{
			//FIXME:Quest Finding the Source
			//Take a reading of the temperature at a hot spot.
		}break;
	case 17166:// Release Umi's Yeti
		{
			//Quest Are We There, Yeti?
			//Select Umi's friend and click to release the Mechanical Yeti
		}break;
	case 17271:// Test Fetid Skull
		{
			//FIXME:Marauders of Darrowshire
			//Wave over a Fetid skull to test its resonance
		}break;
	case 18153:// Kodo Kombobulator
		{
			//FIXME:Kodo Roundup Quest
			//Kodo Kombobulator on any Ancient, Aged, or Dying Kodo to lure the Kodo to follow (one at a time)
		}break;
	case 19250:// Placing Smokey's Explosives
		{
			//This is something related to quest i think
		}break;
	case 19512:// Apply Salve
		{
			//FIXME:Cure a sickly animal afflicted by the taint of poisoning
		}break;
	case 20804:// Triage 
		{
			//Quest Triage
			//Use on Injured, Badly Injured, and Critically Injured Soldiers
		}break;
	case 21050:// Melodious Rapture
		{
			//Quest Deeprun Rat Roundup 
		}break;
	case 21332:// Aspect of Neptulon
		{
			//FIXME:Used on plagued water elementals in Eastern Plaguelands
			//Quest:Poisoned Water
		}break;
	case 21960:// Manifest Spirit
		{
			//FIXME:Forces the spirits of the first centaur Kahns to manifest in the physical world
			//thats a quest
			//its for maraudon i think
			//u use that on the spirit mobs
			//to release them
		}break;
	case 23359:// Transmogrify!
		{
			//Quest Zapped Giants 
			//Zap a Feralas giant into a more manageable form
		}break;
	case 27184:// Summon Mor Grayhoof
		{
			//16044	Mor Grayhoof Trigger
			//16080	Mor Grayhoof

			//Related to quests The Left Piece of Lord Valthalak's Amulet  (Dungeon)
			//and The Right Piece of Lord Valthalak's Amulet  (Dungeon)
		}break;
	case 27190:// Summon Isalien
		{
			//16045	Isalien Trigger
			//16097	Isalien

			//Related to quests The Left Piece of Lord Valthalak's Amulet  (Dungeon)
			//and The Right Piece of Lord Valthalak's Amulet  (Dungeon)
		}break;
	case 27191:// Summon the remains of Jarien and Sothos
		{
			/*
			16046	Jarien and Sothos Trigger
			16101	Jarien
			16103	Spirit of Jarien
			
			16102	Sothos
			16104	Spirit of Sothos
			*/

			//Related to quests The Left Piece of Lord Valthalak's Amulet  (Dungeon)
			//and The Right Piece of Lord Valthalak's Amulet  (Dungeon)
		}break;
	case 27201:// Summon the spirit of Kormok
		{
			/*16047	Kormok Trigger
			16118	Kormok
			*/
			//Related to quests The Left Piece of Lord Valthalak's Amulet  (Dungeon)
			//and The Right Piece of Lord Valthalak's Amulet  (Dungeon)
		}break;
	case 27202:// Summon Lord Valthalak
		{
			/*
			16042	Lord Valthalak
			16048	Lord Valthalak Trigger
			16073	Spirit of Lord Valthalak

			*/
			//Related to quests The Left Piece of Lord Valthalak's Amulet  (Dungeon)
			//and The Right Piece of Lord Valthalak's Amulet  (Dungeon)
		}break;
	case 27203:// Summon the spirits of the dead at haunted locations
		{
			//Related to quests The Left Piece of Lord Valthalak's Amulet  (Dungeon)
			//and The Right Piece of Lord Valthalak's Amulet  (Dungeon)
		}break;
	case 27517:// Use this banner at the Arena in Blackrock Depths to challenge Theldren
		{
			//This is used to make Theldren spawn at the place where it used
			//I couldnt find theldrin, and his men in creature names database
			//Someone has to write this and this is related to The Challange quest
			/*By moving to the center grate, you trigger the arena event. 
			A random group of mobs (spiders, worms, bats, raptors) spawns, 
			and you have to kill them. After the last one dies, and a small 
			break, a boss mob spawns. Successfully completing this event 
			turns the arena spectators from red to yellow*/
		}break;
	case 29858: //Soulshatter
		{
			if( !u_caster || !u_caster->isAlive() || !unitTarget || !unitTarget->isAlive() )
				return;

			u_caster->CastSpell(unitTarget, 32835, false);
		}break;
	}										 
}

void Spell::SpellEffectTeleportUnits( uint32 i )  // Teleport Units
{
	uint32 spellId = GetProto()->Id;

	if( unitTarget == NULL )
	{ 
		return;
	}

	// Try a dummy SpellHandler
	if( sScriptMgr.CallScriptedDummySpell( GetProto(), i, this ) )
	{ 
		return;
	}

	// Shadowstep
	if( ( GetProto()->NameHash == SPELL_HASH_SHADOWSTEP 
			|| GetProto()->NameHash == SPELL_HASH_KILLING_SPREE 
			|| GetProto()->NameHash == SPELL_HASH_FERAL_CHARGE___CAT )
		&& p_caster && p_caster->IsInWorld() )
	{
		// this is rather tricky actually. we have to calculate the orientation of the creature/player, and then calculate a little bit of distance behind that. */
		float ang;

		if( unitTarget == m_caster )
		{
			/* try to get a selection */
 			unitTarget = m_caster->GetMapMgr()->GetUnit(p_caster->GetSelection());
			if( (unitTarget == NULL ) || !isAttackable(p_caster, unitTarget, !(GetProto()->c_is_flags & SPELL_FLAG_IS_TARGETINGSTEALTHED) ) || (unitTarget->CalcDistance(p_caster) > 28.0f))
			{ 
				return;
			}
		}

		if( unitTarget->GetTypeId() == TYPEID_UNIT )
		{
			if( unitTarget->GetUInt64Value( UNIT_FIELD_TARGET ) != 0 )
			{
				// We're chasing a target. We have to calculate the angle to this target, this is our orientation.
				ang = m_caster->calcAngle(m_caster->GetPositionX(), m_caster->GetPositionY(), unitTarget->GetPositionX(), unitTarget->GetPositionY());

				// convert degree angle to radians 
				ang = ang * float(M_PI) / 180.0f;
			}
			else
			{
				// Our orientation has already been set.
				ang = unitTarget->GetOrientation();
			}
		}
		else
		{
			// Players orientation is sent in movement packets
			ang = unitTarget->GetOrientation();
		}

		// avoid teleporting into the model on scaled models
		float shadowstep_distance = 1.6f * unitTarget->GetFloatValue(OBJECT_FIELD_SCALE_X);
		float new_x = unitTarget->GetPositionX() - (shadowstep_distance * cosf(ang));
		float new_y = unitTarget->GetPositionY() - (shadowstep_distance * sinf(ang));
		float new_z = p_caster->GetMapMgr()->GetLandHeight( new_x, new_y, unitTarget->GetPositionZ() );
		if( new_z == VMAP_VALUE_NOT_INITIALIZED )
			new_z = unitTarget->GetPositionZ() + 1.0f;
		new_z = MAX( new_z, unitTarget->GetPositionZ() + 1.0f );

		/* Send a movement packet to "charge" at this target. Similar to warrior charge. */
		p_caster->z_axisposition = 0.0f;
		p_caster->SafeTeleport(p_caster->GetMapId(), p_caster->GetInstanceID(), LocationVector(new_x, new_y, new_z, unitTarget->GetOrientation()));
		p_caster->SpeedCheatDelay( float2int32( shadowstep_distance * 1000.0f ) + MIN( 5000, p_caster->GetSession()->GetLatency() ) + 1000 );
		
		return;
	}

	/* TODO: Remove Player From bg */

	if(unitTarget->GetTypeId() == TYPEID_PLAYER)
		HandleTeleport(spellId, unitTarget);
}

void Spell::SpellEffectApplyAura(uint32 i)  // Apply Aura
{
	if(!unitTarget)
	{ 
		return;
	}
	// can't apply stuns/fear/polymorph/root etc on boss
	if ( !playerTarget )
	{
		if (u_caster && (u_caster != unitTarget))
		{
			Creature * c = SafeCreatureCast( unitTarget );
			if (c)
			{

				/*
				Charm (Mind Control, enslave demon): 1
				Confuse (Blind etc): 2
				Fear: 4
				Root: 8
				Silence : 16
				Stun: 32
				Sheep: 64
				Banish: 128
				Sap: 256
				Frozen : 512
				Ensnared 1024
				Sleep 2048
				Taunt (aura): 4096
				Decrease Speed (Hamstring) (aura): 8192
				Spell Haste (Curse of Tongues) (aura): 16384
				Interrupt Cast: 32768
				Mod Healing % (Mortal Strike) (aura): 65536
				Total Stats % (Vindication) (aura): 131072
				*/

				//Spells with Mechanic also add other ugly auras, but if the main aura is the effect --> immune to whole spell
				if (c->GetProto() && c->GetProto()->modImmunities)
				{
					bool immune = false;
					if (m_spellInfo->MechanicsType)
					{
						switch(m_spellInfo->MechanicsType)
						{
						case MECHANIC_CHARMED:
							if (c->GetProto()->modImmunities & 1)
								immune = true;
							break;
						case MECHANIC_DISORIENTED:
							if (c->GetProto()->modImmunities & 2)
								immune = true;
							break;
						case MECHANIC_FLEEING:
							if (c->GetProto()->modImmunities & 4)
								immune = true;
							break;
						case MECHANIC_ROOTED:
							if (c->GetProto()->modImmunities & 8)
								immune = true;
							break;
						case MECHANIC_SILENCED:
							if ( c->GetProto()->modImmunities & 16)
								immune = true;
							break;
						case MECHANIC_STUNNED:
							if (c->GetProto()->modImmunities & 32)
								immune = true;
							break;
						case MECHANIC_POLYMORPHED:
							if (c->GetProto()->modImmunities & 64)
								immune = true;
							break;
						case MECHANIC_BANISHED:
							if (c->GetProto()->modImmunities & 128)
								immune = true;
							break;
						case MECHANIC_SAPPED:
							if (c->GetProto()->modImmunities & 256)
								immune = true;
							break;
						case MECHANIC_FROZEN:
							if (c->GetProto()->modImmunities & 512)
								immune = true;
							break;
						case MECHANIC_ENSNARED:
							if (c->GetProto()->modImmunities & 1024)
								immune = true;
							break;
						case MECHANIC_ASLEEP:
							if (c->GetProto()->modImmunities & 2048)
								immune = true;
							break;
						}
					}
					if (!immune)
					{
						// Spells wich do more than just one thing (damage and the effect) dont have a mechanic and we should only cancel the aura to be placed
						switch (m_spellInfo->eff[i].EffectApplyAuraName)
						{
						case SPELL_AURA_MOD_CONFUSE:
							if (c->GetProto()->modImmunities & 2)
								immune = true;
							break;
						case SPELL_AURA_MOD_FEAR:
							if (c->GetProto()->modImmunities & 4)
								immune = true;
							break;
						case SPELL_AURA_MOD_TAUNT:
							if (c->GetProto()->modImmunities & 4096)
								immune = true;
							break;
						case SPELL_AURA_MOD_STUN:
							if (c->GetProto()->modImmunities & 32)
								immune = true;
							break;
						case SPELL_AURA_MOD_SILENCE:
							if ((c->GetProto()->modImmunities & 32768) || (c->GetProto()->modImmunities & 16))
								immune = true;
							break;
						case SPELL_AURA_MOD_DECREASE_SPEED:
							if (c->GetProto()->modImmunities & 8192)
								immune = true;
							break;
						case SPELL_AURA_INCREASE_CASTING_TIME_PCT:
							if (c->GetProto()->modImmunities & 16384)
								immune = true;
							break;
						case SPELL_AURA_MOD_LANGUAGE: //hacky way to prefer that the COT icon is set to mob
							if (c->GetProto()->modImmunities & 16384)
								immune = true;
							break;
						case SPELL_AURA_MOD_HEALING_DONE_PERCENT:
							if (c->GetProto()->modImmunities & 65536)
								immune = true;
							break;
						case SPELL_AURA_MOD_TOTAL_STAT_PERCENTAGE:
							if (c->GetProto()->modImmunities & 131072)
								immune = true;
							break;
						}
					}
					if (immune)
					{ 
						return;
					}
				}
			}
		}
	}
	
	// avoid map corruption.
	if( unitTarget->GetInstanceID() != m_caster->GetInstanceID() )
	{ 
		return;
	}
	//aura immunity used only by magic shell atm
	if( unitTarget->AuraSchoolImmunityList[ GetProto()->School ] && isAttackable( m_caster, unitTarget ) )
	{
		return;
	}
	//check if we already have stronger aura
	Aura *pAura = unitTarget->FindQueuedAura( GetProto()->Id );

//	std::map<uint32,Aura*>::iterator itr=unitTarget->tmpAura.find(GetProto()->Id);
	//if we do not make a check to see if the aura owner is the same as the caster then we will stack the 2 auras and they will not be visible client sided
//	if(itr==unitTarget->tmpAura.end())
	if( pAura == NULL )
	{
		uint32 Duration = this->GetDuration();
		if( u_caster && GetProto()->eff[i].EffectAmplitude > 0 
//			&& (GetProto()->c_is_flags & (SPELL_FLAG_IS_HEALING|SPELL_FLAG_IS_DAMAGING)) -> spells like arcane missiles should also "expire" and cast more quickly
			)
		{
//			Duration += GetProto()->EffectAmplitude[i];	//frikkin hackfix to make periodic triggers do the last tick
			//removed since cataclysm ? duration is not moded, only tick count
//			Duration = (uint32)(Duration * u_caster->GetSpellHasteDots());
		}
		
		// Handle diminishing returns, if it should be resisted, it'll make duration 0 here.
		if(!(GetProto()->Attributes & ATTRIBUTES_PASSIVE) && Duration != (-1) ) // Passive
		{
			GetPVPDuration( &Duration, unitTarget, GetProto() );
			//would be cool to know if aura is negative before we apply it
			int32 DurationMod = 0;
			if( unitTarget->OffensiveMagicalDurationModPCT != 0 && ( ( GetProto()->SchoolMask & SCHOOL_MASK_NORMAL ) == 0 ) && isAttackable( m_caster, unitTarget ) == true 
				&& GetProto()->GetDispelType() == DISPEL_MAGIC //do not reduce cyclone
				)
				DurationMod += (int32)Duration * unitTarget->OffensiveMagicalDurationModPCT / 100;
			if( GetProto()->MechanicsType > 0 && GetProto()->MechanicsType < MECHANIC_TOTAL )
				DurationMod += (int32)Duration * unitTarget->MechanicDurationPctMod[ GetProto()->MechanicsType ] / 100;
			else if( GetProto()->eff[i].EffectMechanic > 0 && GetProto()->eff[i].EffectMechanic < MECHANIC_TOTAL )
				DurationMod += (int32)Duration * unitTarget->MechanicDurationPctMod[ GetProto()->eff[i].EffectMechanic ] / 100;
			//disarm needs to have duration mod pre diminish timer
			//make sure we can extract negative number from positive
			if( DurationMod < 0 && uint32(-DurationMod) > Duration )
				Duration = 0;
			else if( Duration != 0 )	//0 means probably diminishing return told us we should be immune to it
				Duration = Duration + DurationMod;

			if( unitTarget != m_caster )
				::ApplyDiminishingReturnTimer(&Duration, unitTarget, GetProto());
#ifdef _DEBUG
			if( p_caster )
			{
				int64 *AddDur = p_caster->GetCreateIn64Extension( EXTENSION_ID_MOD_AUR_DURATION );
				Duration = MAX( Duration, *AddDur );
			}
			if( u_caster && u_caster->IsPet() )
			{
				int64 *AddDur = SafePetCast( u_caster )->GetPetOwner()->GetCreateIn64Extension( EXTENSION_ID_MOD_AUR_DURATION );
				Duration = MAX( Duration, *AddDur );
			}
#endif		
		}

		if( Duration <= 0 )
		{
			//maybe add some resist messege to client here ?
			sLog.outDebug("Spell apply aura error : Spell %u - %s has no duration \n",GetProto()->Id,GetProto()->Name);
			return;
		}

		pAura=AuraPool.PooledNew( __FILE__, __LINE__ );
		if( m_caster->IsGameObject() && m_caster->GetUInt32Value(OBJECT_FIELD_CREATED_BY) )
		{
			Unit *GoCreater = m_caster->GetMapMgr()->GetUnit( m_caster->GetUInt64Value( OBJECT_FIELD_CREATED_BY ) );
			if( GoCreater )
				pAura->Init(GetProto(), Duration, GoCreater, unitTarget, i_caster);
			else
				pAura->Init(GetProto(), Duration, m_caster, unitTarget, i_caster);
		}
		else
			pAura->Init(GetProto(), Duration, m_caster, unitTarget, i_caster);

		//player casts this item spell
		if( castedItemId && pAura->m_castedItemId == 0 )
			pAura->m_castedItemId = castedItemId;

		pAura->pSpellId = pSpellId; //this is required for triggered spells
		
//		unitTarget->tmpAura[GetProto()->Id] = pAura;		
		unitTarget->QueueAuraAdd.push_front( pAura );
	}

	if( forced_miscvalues[i] )
		pAura->AddMod(GetProto()->eff[i].EffectApplyAuraName,damage,forced_miscvalues[i],i, forced_pct_mod_cur);
	else
		pAura->AddMod(GetProto()->eff[i].EffectApplyAuraName,damage,GetProto()->eff[i].EffectMiscValue,i, forced_pct_mod_cur);

}

void Spell::SpellEffectPowerDrain(uint32 i)  // Power Drain
{
	if(!unitTarget || !unitTarget->isAlive())
	{ 
		return;
	}

	uint32 power_type = GetProto()->eff[i].EffectMiscValue;
	uint32 curPower = unitTarget->GetPower(power_type);
	if( power_type == POWER_TYPE_MANA && unitTarget->IsPlayer() )
	{
		Player* mPlayer = SafePlayerCast( unitTarget );
		if( mPlayer->IsInFeralForm() )
		{ 
			return;
		}

		// Resilience - reduces the effect of mana drains by (CalcRating*2)%.
//		damage *= float2int32( 1 - ( ( SafePlayerCast(unitTarget)->CalcRating( PLAYER_RATING_MODIFIER_SPELL_CRIT_RESILIENCE ) * 2 ) / 100.0f ) );
	}
	uint32 amt = damage + ( ( u_caster->GetDamageDoneMod( GetProto()->School ) * 80 ) / 100 );
	if(amt>curPower)
		amt=curPower;
	unitTarget->SetPower(power_type,curPower-amt);
	u_caster->Energize( u_caster, GetProto()->Id, amt, power_type, 0 );
}

void Spell::SpellEffectHealthLeech(uint32 i) // Health Leech
{
	if(!unitTarget || !unitTarget->isAlive())
	{ 
		return;
	}

	uint32 curHealth = unitTarget->GetUInt32Value(UNIT_FIELD_HEALTH);
	uint32 heal_amt = float2int32( damage * GetProto()->eff[i].EffectConversionCoef );
	if(heal_amt > curHealth)
		heal_amt = curHealth;

	int32 abs = unitTarget->ResilianceAbsorb( damage, GetProto(), m_caster );
	damage -= abs;
	m_caster->DealDamage(unitTarget, damage, GetProto()->Id);
	//there is a leach combat log, should use that. Right now it's not updated
	m_caster->SendCombatLogMultiple( unitTarget, damage, 0, abs, 0, GetProto()->Id, GetProto()->SchoolMask, COMBAT_LOG_SPELL_DAMAGE, 0, COMBAT_LOG_SUBFLAG_PERIODIC_DAMAGE );

	if( m_caster->IsUnit() )
		m_caster->SendCombatLogMultiple( SafeUnitCast( m_caster ), heal_amt, 0, 0, 0, GetProto()->Id, GetProto()->SchoolMask, COMBAT_LOG_HEAL, 0, COMBAT_LOG_SUBFLAG_PERIODIC_HEAL );

	uint32 playerCurHealth = m_caster->GetUInt32Value(UNIT_FIELD_HEALTH);
	uint32 playerMaxHealth = m_caster->GetUInt32Value(UNIT_FIELD_MAXHEALTH);
	if(playerCurHealth + heal_amt > playerMaxHealth)
		m_caster->SetUInt32Value(UNIT_FIELD_HEALTH, playerMaxHealth);
	else
		m_caster->SetUInt32Value(UNIT_FIELD_HEALTH, playerCurHealth + heal_amt);		   
}

void Spell::SpellEffectHeal(uint32 i) // Heal
{
	{
		//yep, the usual special case. This one is shaman talent : Nature's guardian
		//health is below 30%, we have a mother spell to get value from
		switch (GetProto()->Id)
		{
		case 31616:
			{
				if(unitTarget && unitTarget->IsPlayer() && pSpellId && unitTarget->GetHealthPct()<30)
				{
					//check for that 10 second cooldown
					SpellEntry *spellInfo = dbcSpell.LookupEntry(pSpellId );
					if(spellInfo)
					{
						//heal value is receivad by the level of current active talent :s
						//maybe we should use CalculateEffect(uint32 i) to gain SM benefits
						int32 value = 0;
						int32 basePoints = spellInfo->eff[i].EffectBasePoints+1;//+(m_caster->getLevel()*basePointsPerLevel);
						int32 randomPoints = spellInfo->eff[i].EffectDieSides;
						if(randomPoints <= 1)
							value = basePoints;
						else
							value = basePoints + rand() % randomPoints;
						//the value is in percent. Until now it's a fixed 10%
						value = unitTarget->GetUInt32Value(UNIT_FIELD_MAXHEALTH)*value/100;
//						Heal( value, false, i);
						u_caster->SpellHeal( unitTarget, GetProto(), value, pSpellId==0, static_dmg[i] != 0, false, 1, forced_pct_mod_cur, i, 0 );
					}
				}
			}break; 
/*		case 18562: //druid - swiftmend
			{
				if( unitTarget )
				{
					uint32 ticks_for_insta_heal = 18000;
					//consume rejuvenetaion and regrowth
					Aura * taura = unitTarget->HasAuraWithNameHash( SPELL_HASH_REGROWTH, 0, AURA_SEARCH_POSITIVE ); //Regrowth
					if( taura == NULL )
					{
						taura = unitTarget->HasAuraWithNameHash( SPELL_HASH_REJUVENATION, 0, AURA_SEARCH_POSITIVE );//Rejuvenation
						ticks_for_insta_heal = 12000;
					}
					if( taura != NULL 
//						&& taura->m_modcount == 1 //should have 1 aura, else ...wtf ?
						)
					{
						Modifier *mod = &taura->m_modList[0];
						uint32 heal_ticks = mod->m_amount;
						uint32 bonus = u_caster->GetSpellHealBonus( unitTarget, taura->GetSpellProto(), mod->m_amount, 0 );
						heal_ticks += bonus;
						uint32 tick_period = mod->fixed_amount[0];
						if( tick_period < 500 )	//should be 2000 ?
							tick_period = 500;

						uint32 new_dmg = heal_ticks * ticks_for_insta_heal / mod->fixed_amount[0];	//fixed amount is supposed to be the tick period here
						
						if( p_caster == NULL || p_caster->HasGlyphWithID( GLYPH_DRUID_SWIFTMEND ) == false )
							unitTarget->RemoveAura( taura );	//consume aura
						else
						{
							//should we set the aurastate here again ? Cause swiftmend eat it up
						}

						Heal((int32)new_dmg,false, i);
					}

				}
			}break; */
/*			//Bloodthirst
			case 23880:
			{
				if(unitTarget)
					Heal(unitTarget->GetUInt32Value(UNIT_FIELD_MAXHEALTH)/100);
			}break;*/
			//Death pact
/*			case 48743:
			{
//				if(unitTarget && unitTarget->GetUInt64Value( UNIT_FIELD_CREATEDBY ) == u_caster->GetGUID() )
//				if( p_caster && p_caster->GetSummon() )
				{
//					Unit *ttarget = unitTarget;
//					unitTarget = u_caster; //we always heal the caster
//					Heal(ttarget->GetUInt32Value(UNIT_FIELD_BASE_HEALTH)*damage/100);
					Heal(u_caster->GetUInt32Value(UNIT_FIELD_MAXHEALTH)*damage/100);
//					unitTarget = ttarget;
					if( p_caster && p_caster->GetSummon() )
						p_caster->GetSummon()->Dismiss();
					else if( unitTarget->IsCreature() )
						SafeCreatureCast( unitTarget )->Despawn( 1, 0 );
				} 
			}break; */
		default:
//			Heal((int32)damage,false, i);
			u_caster->SpellHeal( unitTarget, GetProto(), damage, pSpellId==0, static_dmg[i] != 0, false, 1, forced_pct_mod_cur * chaincast_jump_pct_mod / 100, i );
			break;
		}
	}
	if(GetProto()->eff[i].EffectChainTarget)//chain
	{
		int32 reduce_reduce = 0;	
		if((GetProto()->c_is_flags & SPELL_FLAG_IS_REQUIRING_SM_MOD) && u_caster && u_caster->SM_Mods )
			SM_FIValue(u_caster->SM_Mods->SM_PJumpReduce,&reduce_reduce,GetProto()->GetSpellGroupType());
		chaincast_jump_pct_mod = float2int32( chaincast_jump_pct_mod * GetProto()->eff[i].EffectChainMultiplier ) + reduce_reduce;
	}
}

void Spell::SpellEffectQuestComplete(uint32 i) // Quest Complete
{
	if ( p_caster == NULL ) 
	{ 
		return;
	}
	QuestLogEntry * en = p_caster->GetQuestLogForEntry( GetProto()->eff[i].EffectMiscValue );
	if ( en != NULL && !en->CanBeFinished() )
	{
		en->HackComplete();
		en->SendQuestComplete();
	}
}

//wand->
void Spell::SpellEffectWeapondamageNoschool(uint32 i) // Weapon damage + (no School)
{
	if(!unitTarget ||!u_caster)
	{ 
		return;
	}

	//avoid dmg overflows
	if( damage < 0 || damage > 100000 )
	{ 
		return;
	}

	if( GetType() == SPELL_DMG_TYPE_RANGED && GetProto()->speed > 0.0f )
	{
#ifndef USE_SPELL_MISSILE_DODGE
		float time = (m_caster->CalcDistance(unitTarget) * 1000.0f) / GetProto()->speed;
		if(time <= 100.0f)
			u_caster->Strike( unitTarget, RANGED, GetProto(), 0, forced_pct_mod_cur-100, 0, false, false );
		else
		{
			damage = damage * forced_pct_mod_cur / 100;
			sEventMgr.AddEvent(u_caster,&Unit::EventStrikeWithAbility,unitTarget->GetGUID(),
				GetProto(), (uint32)damage, EVENT_SPELL_DAMAGE_HIT, float2int32(time), 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
		} 
#else
		float dist = m_caster->CalcDistance( unitTarget );
		float time = ((dist*1000.0f)/GetProto()->speed);
		LocationVector src,dst;
		src.x = m_caster->GetPositionX();
		src.y = m_caster->GetPositionY();
		src.z = m_caster->GetPositionZ();
		dst.x = unitTarget->GetPositionX();
		dst.y = unitTarget->GetPositionY();
		dst.z = unitTarget->GetPositionZ();
		sEventMgr.AddEvent(m_caster, &Object::EventWeaponDamageToLocation, unitTarget->GetGUID(), GetProto(), (uint32)damage, (int32)forced_pct_mod_cur-100, (int32)i, src, dst, EVENT_SPELL_DAMAGE_HIT, uint32(time), 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
#endif
	}
	else
		u_caster->Strike( unitTarget, ( GetType() == SPELL_DMG_TYPE_RANGED ? RANGED : MELEE ), GetProto(), damage, forced_pct_mod_cur-100, 0, false, false );
}

void Spell::SpellEffectAddExtraAttacks(uint32 i) // Add Extra Attacks
{
	if(!u_caster)
	{ 
		return;
	}

	ExtraAttack *nea = new ExtraAttack;
	nea->sp = GetProto();
	nea->count = damage;
	//maybe set more details here ?

	u_caster->AddExtraAttack( nea );
}

void Spell::SpellEffectDodge(uint32 i)
{
	//i think this actually enbles the skill to be able to dodge melee+ranged attacks
	//value is static and sets value directly which will be modified by other factors
	//this is only basic value and will be overwiten elsewhere !!!
//	if(unitTarget->IsPlayer())
//		unitTarget->SetFloatValue(PLAYER_DODGE_PERCENTAGE,damage);
}

void Spell::SpellEffectParry(uint32 i)
{
	if(unitTarget)
		unitTarget->setcanperry(true);
}

void Spell::SpellEffectBlock(uint32 i)
{
	//i think this actually enbles the skill to be able to block melee+ranged attacks
	//value is static and sets value directly which will be modified by other factors
//	if(unitTarget->IsPlayer())
//		unitTarget->SetFloatValue(PLAYER_BLOCK_PERCENTAGE,damage);
}

void Spell::SpellEffectCreateItem(uint32 i) // Create item 
{
	if(!p_caster || unitTarget == NULL || unitTarget->IsPlayer() == false )
	{ 
		return;
	}

	Item* newItem;
	Item *add;
	uint8 slot;
	SlotResult slotresult;

	skilllinespell* skill = GetProto()->spell_skill;

	//take away the currencies + generate a new project instead of this
	if( skill && skill->skillId == SKILL_ARCHAEOLOGY && p_caster )
		p_caster->EventSolveProject( GetProto() );

	for(int j=0; j<3; j++) // now create the Items
	{
		ItemPrototype *m_itemProto;
		uint32 entry = GetProto()->eff[j].EffectItemType;
		//Darkmoon Card of the North -> will create a card from one of the many, stats say you need to create around 90 cards to get 1 set
		if( entry == 44318 ) 
		{
			uint32 randomentry[]={44260,44261,44262,44263,44264,44265,44266,44267,44268,44269,44270,44271,44272,44273,44274,44275,44286,44287,44288,44289,44290,44291,44292,44293,44277,44278,44279,44280,44281,44282,44284,44285};
			uint32 randomentrycount = sizeof(randomentry) / sizeof(uint32);
			entry = randomentry[ RandomUInt() % randomentrycount ];
		}
		//Darkmoon Card of the Destruction -> will create a card from one of the many, stats say you need to create around 90 cards to get 1 set
		else if( entry == 61987 ) 
		{
			uint32 randomentry[]={61996,61995,61994,61993,61992,61991,61990,61988,61989,61997,61998,61999,62000,62001,62002,62003,62004,62013,62014,62015,62016,62017,62018,62019,62005,62006,62007,62008,62009,62010,62011,62012};
			uint32 randomentrycount = sizeof(randomentry) / sizeof(uint32);
			entry = randomentry[ RandomUInt() % randomentrycount ];
		}
		m_itemProto = ItemPrototypeStorage.LookupEntry( entry );
		if (!m_itemProto)
			continue;

		if(GetProto()->eff[j].EffectItemType == 0)
			continue;

		uint32 item_count = MAX(1,damage);

		if( p_caster && p_caster->GetItemInterface() )
		{
			uint8 ret = p_caster->GetItemInterface()->CanReceiveItem( m_itemProto, item_count );
			if( ret != 0 )
			{
				SendCastResult(SPELL_FAILED_TOO_MANY_OF_ITEM);
				return;
			}
		}

		// tailoring specializations get +1 cloth bonus
		switch(GetProto()->Id)
		{
		case 36686: //Shadowcloth
			if(p_caster->HasSpell(26801)) item_count += 1;
			break;
		case 26751: // Primal Mooncloth
			if(p_caster->HasSpell(26798)) item_count += 1;
			break;
		case 31373: //Spellcloth
			if(p_caster->HasSpell(26797)) item_count += 1;
			break;
		}

		// item count cannot be more than allowed in a single stack
		if (item_count > m_itemProto->MaxCount)
			item_count = m_itemProto->MaxCount;

		// item count cannot be more than item unique value
		if (m_itemProto->Unique && item_count > m_itemProto->Unique)
			item_count = m_itemProto->Unique;

		Player *pTarget = SafePlayerCast( unitTarget );	//we checked unittarget to be player on casting the spell

		if(pTarget->GetItemInterface()->CanReceiveItem(m_itemProto, item_count)) //reversed since it sends >1 as invalid and 0 as valid
		{
			SendCastResult(SPELL_FAILED_TOO_MANY_OF_ITEM);
			return;
		}

		slot = 0;
		add = pTarget->GetItemInterface()->FindItemLessMax( entry, item_count, false);
		if (!add)
		{
			slotresult = pTarget->GetItemInterface()->FindFreeInventorySlot(m_itemProto);
			if(!slotresult.Result)
			{
				  SendCastResult(SPELL_FAILED_TOO_MANY_OF_ITEM);
				  return;
			}
			
			newItem =objmgr.CreateItem( entry ,pTarget);
			newItem->SetUInt64Value(ITEM_FIELD_CREATOR,m_caster->GetGUID());
			newItem->SetUInt32Value(ITEM_FIELD_STACK_COUNT, item_count);
			newItem->Randomize();

			AddItemResult res = pTarget->GetItemInterface()->SafeAddItem(newItem,slotresult.ContainerSlot, slotresult.Slot);
			if( res == ADD_ITEM_RESULT_OK )
			{
				/*WorldPacket data(45);
				pTarget->GetSession()->BuildItemPushResult(&data, pTarget->GetGUID(), 1, item_count, GetProto()->EffectItemType[i] ,0,0xFF,1,0xFFFFFFFF);
				pTarget->SendMessageToSet(&data, true);*/
				pTarget->GetSession()->SendItemPushResult(newItem,true,false,true,true,slotresult.ContainerSlot,slotresult.Slot,item_count);
			} 
			else if( res == ADD_ITEM_RESULT_ERROR )
			{
				newItem->DeleteMe();
				newItem = NULL;
			}
		} 
		else 
		{
			//scale item_count down if total stack will be more than 20
			if(add->GetUInt32Value(ITEM_FIELD_STACK_COUNT) + item_count > m_itemProto->MaxCount )
			{
				uint32 item_count_filled;
				item_count_filled = m_itemProto->MaxCount - add->GetUInt32Value(ITEM_FIELD_STACK_COUNT);
				add->SetCount(m_itemProto->MaxCount);
				add->m_isDirty = true;

				slotresult = pTarget->GetItemInterface()->FindFreeInventorySlot(m_itemProto);
				if(!slotresult.Result)
					item_count = item_count_filled;
				else
				{
					newItem =objmgr.CreateItem( entry,pTarget);
					newItem->SetUInt64Value(ITEM_FIELD_CREATOR,m_caster->GetGUID());
					newItem->SetUInt32Value(ITEM_FIELD_STACK_COUNT, item_count - item_count_filled);
					if(pTarget->GetItemInterface()->SafeAddItem(newItem,slotresult.ContainerSlot, slotresult.Slot) == ADD_ITEM_RESULT_ERROR )
					{
						newItem->DeleteMe();
						newItem = NULL;
						item_count = item_count_filled;
					}
					else
						pTarget->GetSession()->SendItemPushResult(newItem, true, false, true, true, slotresult.ContainerSlot, slotresult.Slot, item_count);
                }
			}
			else
			{
				add->SetCount(add->GetUInt32Value(ITEM_FIELD_STACK_COUNT) + item_count);
				add->m_isDirty = true;
				pTarget->GetSession()->SendItemPushResult(add, true,false,true,false,pTarget->GetItemInterface()->GetBagSlotByGuid(add->GetGUID()),0xFFFFFFFF,item_count);
			}

			/*WorldPacket data(45);
			pTarget->GetSession()->BuildItemPushResult(&data, pTarget->GetGUID(), 1, item_count, GetProto()->EffectItemType[i] ,0,0xFF,1,0xFFFFFFFF);
			pTarget->SendMessageToSet(&data, true);*/
		}
		//add guild log if should 
		if( pTarget->GetGuild() )
		{
			Item *it = pTarget->GetItemInterface()->FindItemLessMax( entry, 1, false );
			if( it != NULL && it->GetProto()->Quality >= ITEM_QUALITY_EPIC_PURPLE && it->GetProto()->ItemLevel * 110 / 100 >= pTarget->GetItemAvgLevelBlizzlike() )
				pTarget->GetGuild()->GuildNewsAdd( GUILD_NEWS_LOG_ITEM_CRAFTED, pTarget->GetGUID(), entry );
		}
		// skill up
		if ( skill != NULL )
			DetermineSkillUp( skill->skillId, skill->skill_point_advance );
	}
	if ( skill != NULL )
	{
		// profession discoveries
		uint32 discovered_recipe = 0;
		std::set<ProfessionDiscovery*>::iterator itr = objmgr.ProfessionDiscoveryTable.begin();
		for ( ; itr != objmgr.ProfessionDiscoveryTable.end(); itr++ )
		{
			ProfessionDiscovery * pf = ( *itr );
			if ( pf != NULL && GetProto()->Id == pf->SpellId && p_caster->_GetSkillLineCurrent( skill->skillId ) >= pf->SkillValue && !p_caster->HasSpell( pf->SpellToDiscover ) && RandChance( pf->Chance ) )
			{
				discovered_recipe = pf->SpellToDiscover;
				break;
			}
		}
		// if something discovered learn p_caster that recipe and broadcast message
		if ( discovered_recipe != 0 )
		{
			SpellEntry * se = dbcSpell.LookupEntryForced( discovered_recipe );
			if ( se != NULL )
			{
				p_caster->addSpell( discovered_recipe );
				p_caster->BroadcastMessage( "DISCOVERY! %s has discovered how to create %s",p_caster->GetName(), se->Name );
//				char msg[256];
//				sprintf( msg, "%sDISCOVERY! %s has discovered how to create %s.|r", MSG_COLOR_GOLD, p_caster->GetName(), se->Name );
//				WorldPacket * data;
//				data = sChatHandler.FillMessageData( CHAT_MSG_SYSTEM, LANG_UNIVERSAL,  msg, p_caster->GetGUID(), 0 );
//				p_caster->GetMapMgr()->SendChatMessageToCellPlayers( p_caster, data, 2, 1, LANG_UNIVERSAL, p_caster->GetSession() );
//				delete data;
//				data = NULL;
			}
		}
	}
}

void Spell::SpellEffectWeapon(uint32 i)
{
	if( playerTarget == NULL )
	{ 
		return;
	}

	uint32 skill = 0;
	uint32 spell = 0;

	switch( this->GetProto()->Id )
	{
	case 201:    // one-handed swords
		{
			skill = SKILL_SWORDS;
		}break;
	case 202:   // two-handed swords
		{
			skill = SKILL_2H_SWORDS;
		}break;
	case 203:   // Unarmed
		{
			skill = SKILL_UNARMED;
		}break;
	case 199:   // two-handed maces
		{
			skill = SKILL_2H_MACES;
		}break;
	case 198:   // one-handed maces
		{
			skill = SKILL_MACES;
		}break;
	case 197:   // two-handed axes
		{
			skill = SKILL_2H_AXES;
		}break;
	case 196:   // one-handed axes
		{
			skill = SKILL_AXES;
		}break;
	case 5011: // crossbows
		{
			skill = SKILL_CROSSBOWS;
			spell = SPELL_RANGED_GENERAL;
		}break;
	case 227:   // staves
		{
			skill = SKILL_STAVES;
		}break;
	case 1180:  // daggers
		{
			skill = SKILL_DAGGERS;
		}break;
	case 200:   // polearms
		{
			skill = SKILL_POLEARMS;
		}break;
	case 15590: // fist weapons
		{
			skill = SKILL_UNARMED;
		}break;
	case 264:   // bows
		{
			skill = SKILL_BOWS;
			spell = SPELL_RANGED_GENERAL;
		}break;
	case 266: // guns
		{
			skill = SKILL_GUNS;
			spell = SPELL_RANGED_GENERAL;
		}break;
	case 2567:  // thrown
		{
			skill = SKILL_THROWN;
		}break;
	case 5009:  // wands
		{
			skill = SKILL_WANDS;
//			spell = SPELL_RANGED_GENERAL;
			spell = SPELL_RANGED_WAND;
		}break;
	//case 3386:  // spears
	//	skill = 0;   // ??!!
	//	break;
	default:
		{
			skill = 0;
			sLog.outDebug("WARNING: Could not determine skill for spell id %d (SPELL_EFFECT_WEAPON)", this->GetProto()->Id);
		}break;
	}

	// Don't add skills to players logging in.
	/*if((GetProto()->Attributes & ATTRIBUTES_PASSIVE) && playerTarget->m_TeleportState == 1)
		return;*/

	if(skill)
	{
		if(spell)
			playerTarget->addSpell(spell);
		
		// if we do not have the skill line
		if(!playerTarget->_HasSkillLine(skill))
		{
			playerTarget->_AddSkillLine(skill, 1, playerTarget->getLevel()*5);
		}
		else // unhandled.... if we have the skill line
		{
		}
	}
}

void Spell::SpellEffectDefense(uint32 i)
{
	//i think this actually enbles the skill to be able to use defense
	//value is static and sets value directly which will be modified by other factors
	//this is only basic value and will be overwiten elsewhere !!!
//	if(unitTarget->IsPlayer())
//		unitTarget->SetFloatValue(UNIT_FIELD_RESISTANCES,damage);
}

void Spell::SpellEffectPersistentAA(uint32 i) // Persistent Area Aura
{
	if(m_AreaAura == true || !m_caster->IsInWorld())
	{ 
		return;
	}
	//this has a 0.01% chance to be called. Using it to be able to cast the AA at specific target location. Scripting it
	if( GetProto()->TargettingOverride[i] )
	{
		GetProto()->TargettingOverride[i]( this, i );
	}
	Unit *GoCreater = u_caster;
	if( m_caster->IsGameObject() )
	{
		GoCreater = m_caster->GetMapMgr()->GetUnit( m_caster->GetUInt64Value( OBJECT_FIELD_CREATED_BY ) );
		//owner is on different map. Exiting
		if( GoCreater == NULL )
			return;
	}

	//create only 1 dyn object
	uint32 dur = GetDuration();
	float r = GetRadius(i);

	//Note: this code seems to be useless
	//this must be only source point or dest point
	//this AREA aura it's apllied on area
	//it can'be on unit or self or item or object
	//uncomment it if i'm wrong
	//We are thinking in general so it might be useful later DK
	
	// grep: this is a hack!
	// our shitty dynobj system doesnt support GO casters, so we gotta
	// kinda have 2 summoners for traps that apply AA.
	DynamicObject * dynObj = m_caster->GetMapMgr()->CreateDynamicObject();
//	int32 update_interval = MAX( DYN_OBJ_UPDATE_INTERVAL, GetSpellAmplitude( GetProto(), u_caster, i ));//this gets imba sometimes ticking 3-4 times as they should
	int32 update_interval = MAX( DYN_OBJ_UPDATE_INTERVAL, GetProto()->eff[i].EffectAmplitude ); 

	if( m_caster->IsGameObject() && !unitTarget)
	{
		dynObj->Create( GoCreater, this, m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ(), dur, r, damage, update_interval );
		m_AreaAura = true;
		return;
	}
	
//	if( GetProto()->c_is_flags3 & SPELL_FLAG3_IS_HUNTER_TRAP )
//		dynObj->SetTrapArmDuration( 2000 );

	//i need this to come before TARGET_FLAG_UNIT because AA always targets caster
	if (( m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION ) && m_targets.m_destX != 0.0f )
	{
		dynObj->SetInstanceID(m_caster->GetInstanceID());
		dynObj->Create( GoCreater, this,m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ,dur,r, damage, update_interval);
	}
	else if (m_targets.m_targetMask & TARGET_FLAG_SELF )
		dynObj->Create(u_caster, this,	m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ(), dur,r, damage, update_interval);		 
	else if (( m_targets.m_targetMask & TARGET_FLAG_UNIT ) && unitTarget != NULL && unitTarget->isAlive() )
		dynObj->Create( u_caster, this, unitTarget->GetPositionX(), unitTarget->GetPositionY(), unitTarget->GetPositionZ(),	dur, r, damage, update_interval);
	else if (( m_targets.m_targetMask & TARGET_FLAG_OBJECT ) && unitTarget != NULL && unitTarget->isAlive() )
		dynObj->Create(u_caster, this, unitTarget->GetPositionX(), unitTarget->GetPositionY(), unitTarget->GetPositionZ(),dur, r, damage, update_interval);
	else if (( m_targets.m_targetMask & TARGET_FLAG_SOURCE_LOCATION ) && m_targets.m_srcX != 0.0f )
	{
		dynObj->SetInstanceID(m_caster->GetInstanceID());
		dynObj->Create(u_caster, this, m_targets.m_srcX,m_targets.m_srcY, m_targets.m_srcZ, dur,r, damage, update_interval);
	}
	else if( m_caster == unitTarget )
	{
		dynObj->Create(u_caster, this,	m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ(), dur,r, damage, update_interval);		 
	}
	else
	{
		return;
	}   
	
	if(u_caster && GetProto()->ChannelInterruptFlags > 0)
	{
		u_caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT,dynObj->GetGUID());
		u_caster->SetUInt32Value(UNIT_CHANNEL_SPELL,GetProto()->Id);
	}
	m_AreaAura = true;	

	if( GetProto()->c_is_flags2 & SPELL_FLAG2_IS_TICKING_IMEDIATLY )
		dynObj->Update( 0 );
}

void Spell::SpellEffectSummon(uint32 i) // Summon
{
	damage += 1;	//in 403 sometimes basepoints get +1 added sometimes not
	switch(m_spellInfo->eff[i].EffectMiscValueB)
	{
		case 63:
		case 81:
		case 82:
		case 83:
		case 121:
			{
				SpellEffectSummonTotem(i);
				return;
			}
		case 41:
		case 307:
		case 407:
		case 1341:
		case 1841:
			{
				SpellEffectSummonCritter(i);
				return;
			}
		case 3097: //force of nature
			damage -= 1;	//aaaargh, i need the flag for this
		case 61:	//water elemental (depracated)
		case 881:
		case 669:
		case 208: //dancing blade is summon or just visual aura ?
//		case 1161: //feral spirit
		case 1021: //mage mirror image
		case 2909: //mage mirror image
		case 1964: //force of nature
		case 1562: //force of nature
		case 2301: //snake trap
		case 713: //blood worms
		case 3062:	//Shadowy Apparition - so they are counted
		case 3092:	//bane of doom
		case 829: //raise dead - the version  when you are not specced unholy
			{
				SpellEffectSummonGuardian(i);
				return;
			}
		case 64:	//water elemental (depracated)
		case 1141:	//lightwell
		case 3069:	//wild mushroom
			{
				SpellEffectSummonWild(i);
				return;
			}
		case 65:
		case 428:
			{
				SpellEffectSummonPossessed(i);
				return;
			}
		case 66:
			{
				SpellEffectSummonDemon(i);
				return;
			}
	}	

	if( !u_caster )
	{ 
		sLog.outDebug("Summoning non standard way is not allowed by mobs. Exiting\n");
		return;
	}

	// This is for summon water elemenal, etc 
	uint32 creature_proto;

	if( forced_miscvalues[i] != 0 )
		creature_proto = forced_miscvalues[i];
	else
		creature_proto = GetProto()->eff[i].EffectMiscValue;

	if( p_caster )
	{
		if( GetProto()->NameHash == SPELL_HASH_FLAME_ORB && p_caster->HasAuraWithNameHash( SPELL_HASH_FROSTFIRE_ORB, 0, AURA_SEARCH_PASSIVE ) )
			creature_proto = 45322;
		else if( GetProto()->NameHash == SPELL_HASH_GUARDIAN_OF_ANCIENT_KINGS )
		{
			if( p_caster->GetFirstTalentSpecTree() == 831 ) // paladin holy spec 
				creature_proto = 46490;
			else if( p_caster->GetFirstTalentSpecTree() == 839 ) // paladin protection spec 
				creature_proto = 46499;
			else if( p_caster->GetFirstTalentSpecTree() == 855 ) // paladin retribution spec 
				creature_proto = 46506;
			u_caster->create_guardian( creature_proto, GetDuration(), 0, NULL, 0, GetProto() );
			return;
		}
	}

	CreatureInfo * ci = CreatureNameStorage.LookupEntry( creature_proto );
	CreatureProto * cp = CreatureProtoStorage.LookupEntry( creature_proto );
	if( !ci || !cp )
	{ 
		sLog.outDebug("Cannot summon mobs without existing proto %u\n",creature_proto);
		return;
	}

	if( creature_proto == 510 && p_caster)	// Water Elemental
	{
		PlayerPet *pet = p_caster->GetPlayerPet( -1, creature_proto );
		Pet *summon;
		if( pet )
		{
			p_caster->SpawnPet( pet->number, pet->StableSlot );
			summon = p_caster->GetSummon();
			summon->SetExpire( GetDuration() );
		}
		else
		{
			summon = objmgr.CreatePet( creature_proto, p_caster->GeneratePetNumber( creature_proto ) );
			summon->SetInstanceID(u_caster->GetInstanceID());
			summon->CreateAsSummon(creature_proto, ci, NULL, p_caster, GetProto(), 16 | 1,  GetDuration() );
			summon->SetUInt32Value(UNIT_FIELD_LEVEL, u_caster->getLevel());
			summon->AddSpell(dbcSpell.LookupEntryForced(31707), true);
			summon->AddSpell(dbcSpell.LookupEntryForced(33395), true);
			summon->SendSpellsToOwner(); // some said it does not have castbar
			summon->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, u_caster->GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE));
			summon->_setFaction();
		}

		u_caster->SetUInt64Value(UNIT_FIELD_SUMMON, summon->GetGUID());
	}
	else if( creature_proto == 19668 && p_caster) //shadowfiend
	{
		Pet *summon = objmgr.CreatePet( creature_proto,p_caster->GeneratePetNumber( creature_proto ) );
		summon->SetInstanceID(u_caster->GetInstanceID());
		summon->CreateAsSummon( creature_proto, ci, NULL, p_caster, GetProto(), 1,  GetDuration() );
		summon->SetUInt32Value(UNIT_FIELD_LEVEL, u_caster->getLevel());
		summon->AddSpell(dbcSpell.LookupEntryForced(63619), true);
		summon->SendSpellsToOwner(); // some said it does not have castbar
		summon->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, u_caster->GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE));
		summon->_setFaction();

		u_caster->SetUInt64Value(UNIT_FIELD_SUMMON, summon->GetGUID());
	}
	else if( creature_proto == 29264 && p_caster) //Feral Spirit
	{
		Pet *secondwolf;
		{
			secondwolf = objmgr.CreatePet( creature_proto, p_caster->GeneratePetNumber( creature_proto ) + 1 );
			secondwolf->GetAIInterface()->SetUnitToFollowAngle( M_PI * 3 / 2 );
			secondwolf->SetInstanceID(u_caster->GetInstanceID());
			secondwolf->CreateAsSummon( creature_proto, ci, NULL, p_caster, GetProto(), 1,  GetDuration() );
			secondwolf->SetUInt32Value(UNIT_FIELD_LEVEL, u_caster->getLevel());
			SpellEntry * sp;
			sp = dbcSpell.LookupEntryForced(58857);
			secondwolf->AddSpell( sp, true);	//Twin Howl
			secondwolf->SetSpellState( sp, DEFAULT_SPELL_STATE ); //no autocast
			sp = dbcSpell.LookupEntryForced(58875);
			secondwolf->AddSpell( sp, true);	//Spirit Walk
			secondwolf->SetSpellState( sp, DEFAULT_SPELL_STATE ); //no autocast
			sp = dbcSpell.LookupEntryForced(58861);
			secondwolf->AddSpell( sp, true);	//Bash
			secondwolf->SetSpellState( sp, DEFAULT_SPELL_STATE ); //no autocast
//			secondwolf->SendSpellsToOwner(); // some said it does not have castbar
			secondwolf->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, u_caster->GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE));
			secondwolf->_setFaction();
			secondwolf->CastSpell( secondwolf, 58877, true );	//Spirit Hunt
			p_caster->SetSummon( NULL, 0 ); //restore real summon 
			p_caster->SetSummon( secondwolf, 1 );
		}/**/

		PlayerPet *pet = p_caster->GetPlayerPet( -1, creature_proto );
		Pet *summon;
		if( pet )
		{
			p_caster->SpawnPet( pet->number, pet->StableSlot );
			summon = p_caster->GetSummon();
			summon->SetExpire( GetDuration() );
			summon->CastSpell( secondwolf, 58877, true );	//Spirit Hunt
		}
		else
		{
			summon = objmgr.CreatePet( creature_proto, p_caster->GeneratePetNumber( creature_proto ) );
			summon->SetInstanceID(u_caster->GetInstanceID());
			summon->CreateAsSummon( creature_proto, ci, NULL, p_caster, GetProto(), 16 + 1,  GetDuration() );
			summon->SetUInt32Value(UNIT_FIELD_LEVEL, u_caster->getLevel());
			summon->AddSpell(dbcSpell.LookupEntryForced(58857), true);	//Twin Howl
			summon->AddSpell(dbcSpell.LookupEntryForced(58875), true);	//Spirit Walk
			summon->AddSpell(dbcSpell.LookupEntryForced(58861), true);	//Bash
			summon->SendSpellsToOwner(); // some said it does not have castbar
			summon->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, u_caster->GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE));
			summon->_setFaction();
			summon->CastSpell( summon, 58877, true );	//Spirit Hunt
		}
		u_caster->SetUInt64Value(UNIT_FIELD_SUMMON, summon->GetGUID());
		//hacks on hacks, since we do not support pet groups we will add a pet to our pet
		//the sucky part is that casting spell on 1 pet should also cast spell on the other one :(
/*		Unit *secondwolf;
		{
			secondwolf = summon->create_guardian( creature_proto, GetDuration() );
			if( secondwolf )
			{
				secondwolf->CastSpell( secondwolf, 58877, true );	//Spirit Hunt
				secondwolf->SetUInt32Value( UNIT_FIELD_MINDAMAGE, summon->GetUInt32Value( UNIT_FIELD_MINDAMAGE ) );
				secondwolf->SetUInt32Value( UNIT_FIELD_MAXDAMAGE, summon->GetUInt32Value( UNIT_FIELD_MAXDAMAGE ) );
				secondwolf->BaseDamage[0] = summon->BaseDamage[0];
			}
			//add a proc that will make first wolf ( pet ) spell casts get replicated by second wolf
			{
				SpellEntry *sp = dbcSpell.LookupEntryForced( 51533 );
				ProcTriggerSpell pts( sp, NULL );
				pts.caster = summon->GetGUID();
				pts.spellId = sp->Id;
				pts.procFlags = PROC_ON_CAST_SPELL;
				pts.created_with_value = 0;
				pts.procCharges = 0;
				pts.procChance = 100;
				pts.procInterval = 0;
				summon->RegisterScriptProcStruct( pts );
				uint64 *OtherWolfGUID = (uint64*)summon->GetCreateIn64Extension( EXTENSION_ID_FERAL_SPIRIT_COPY );
				*OtherWolfGUID = secondwolf->GetGUID();
			}
		}/**/
		if( u_caster->HasAuraWithNameHash( SPELL_HASH_ITEM___SHAMAN_T13_ENHANCEMENT_4P_BONUS__FERAL_SPIRITS_, 0, AURA_SEARCH_PASSIVE ) )
		{
			SpellEntry *sp = dbcSpell.LookupEntryForced( 53817 );	//Maelstrom Weapon
			ProcTriggerSpell pts( sp, NULL );
			pts.caster = summon->GetGUID();
			pts.spellId = sp->Id;
			pts.procFlags = PROC_ON_MELEE_ATTACK;
			pts.created_with_value = sp->eff[0].EffectBasePoints + 1;
			pts.procCharges = 0;
			pts.procChance = 45;
			summon->RegisterScriptProcStruct( pts );
			pts.caster = secondwolf->GetGUID();
			secondwolf->RegisterScriptProcStruct( pts );
		}
	}
	else	//just for cases that we did not inspect before. Try to use scripts if possible
	{
		//only one summon at a time
		uint32 summon_duration = GetDuration();
		if( summon_duration < 2000 )
			summon_duration = 10000;	
		LocationVector v=u_caster->GetPosition();
		if( ( GetProto()->c_is_flags2 & SPELL_FLAG2_IS_TRIGGERED_ON_TARGET ) && unitTarget )
			v = unitTarget->GetPosition();
		//i hope this will not mess up mobs casting summon spells :(
		else if( p_caster && m_targets.m_destX != 0.0f && ( m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION ) )
		{
			v.x = m_targets.m_destX; 
			v.y = m_targets.m_destY; 
			v.z = m_targets.m_destZ; 
		}
		Unit *new_sum = u_caster->create_summon( creature_proto, summon_duration, 0, NULL, &v );
	}
}

void Spell::SpellEffectLeap(uint32 i) // Leap
{
	float radius = GetRadius(i);

	// remove movement impeding auras
	u_caster->RemoveAurasByInterruptFlag(AURA_INTERRUPT_ON_ANY_DAMAGE_TAKEN);

/*	if (sWorld.Collision) 
	{
		float ori = m_caster->GetOrientation();				
		float posX = m_caster->GetPositionX()+(radius*(cosf(ori)));
		float posY = m_caster->GetPositionY()+(radius*(sinf(ori)));
		float z = CollideInterface.GetHeight(m_caster->GetMapId(), posX, posY, m_caster->GetPositionZ() + 2.0f);
		if(z == NO_WMO_HEIGHT)		// not found height, or on adt
			z = m_caster->GetMapMgr()->GetLandHeight(posX,posY);

		if( fabs( z - m_caster->GetPositionZ() ) >= 10.0f )
		{ 
			return;
		}

		LocationVector dest(posX, posY, z + 2.0f, ori);
		LocationVector destest(posX, posY, dest.z, ori);
		LocationVector src(m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ() + 2.0f);

		if(CollideInterface.GetFirstPoint(m_caster->GetMapId(), src, destest, dest, -1.5f))
		{
			// hit an object new point is in dest.
			// is this necessary?
			dest.z = CollideInterface.GetHeight(m_caster->GetMapId(), dest.x, dest.y, dest.z + 2.0f);
		}
		else
			dest.z = z;

		dest.o = u_caster->GetOrientation();
		if(p_caster)
		{
			p_caster->blinked = true;
			p_caster->SafeTeleport( p_caster->GetMapId(), p_caster->GetInstanceID(), dest );
		}
		else
		{
			u_caster->SetPosition(dest, true);
		}
	} 
	else  */
	{
		if(!p_caster) 
		{ 
			return;
		}
		p_caster->SpeedCheatDelay( radius * 1000 / p_caster->m_runSpeed + MIN( 5000, p_caster->GetSession()->GetLatency() ) + 2000 );	//should start sampling again after we land
		if( GetProto()->NameHash == SPELL_HASH_BLINK )
		{
			//should we add fear and the rest ?
			if( p_caster->m_special_state & ( UNIT_STATE_STUN | UNIT_STATE_ROOT ) )
				return;
			//check if we could use teleport. Use only statistics map and check if we could walk forward on the jump path
			bool can_tele = true;
			float ori = m_caster->GetOrientation();				
			for( float r=1;r<radius;r+=2.0f )
			{
				float posX = m_caster->GetPositionX()+(r*(cosf(ori)));
				float posY = m_caster->GetPositionY()+(r*(sinf(ori)));
				float z = sSVMaps.GetHeight( m_caster->GetMapId(), posX, posY, m_caster->GetPositionZ() );
				if( z == NO_WMO_HEIGHT || fabs( z - m_caster->GetPositionZ() ) >= 10.0f)
				{ 
					can_tele = false;
					break;
				}
			}
			if( can_tele == true )
			{
				float ori = m_caster->GetOrientation();				
				float posX = m_caster->GetPositionX()+(radius*(cosf(ori)));
				float posY = m_caster->GetPositionY()+(radius*(sinf(ori)));
				float z = sSVMaps.GetHeight( m_caster->GetMapId(), posX, posY, m_caster->GetPositionZ() );
				p_caster->blinked = true;
				p_caster->SafeTeleport( p_caster->GetMapId(), p_caster->GetInstanceID(), posX, posY, z + 3.0f, ori );
				return;
			}
		}
		float verticalSpeed = float(GetProto()->eff[i].EffectBasePoints)/10.0f;
		if( verticalSpeed >= 0.0f )	//positiv puts you underground
			verticalSpeed = -12.0f;
		p_caster->SendKnockbackPacket( cosf(m_caster->GetOrientation()), sinf(m_caster->GetOrientation()), radius, verticalSpeed );
		//m_caster->SendMessageToSet(&data, true);
	}
}

void Spell::SpellEffectJumpBehindTarget(uint32 i) // Leap
{
	if( p_caster == NULL )
		return;

	float radius = GetRadius(i);

	if( m_targets.m_destX != 0 )
		radius = sqrt( Distance2DSq( unitTarget->GetPositionX(), unitTarget->GetPositionY(), m_targets.m_destX, m_targets.m_destY ) );
	else
	{
		Unit *ttarget = unitTarget;
		if( ( ttarget == NULL || ttarget == m_caster ) && p_caster != NULL )
			ttarget = m_caster->GetMapMgr()->GetUnit( p_caster->GetSelection() );
		if( ttarget != NULL && ttarget != m_caster )
		{
			radius = MIN( radius, sqrt( m_caster->GetDistance2dSq( ttarget ) ) );
			m_targets.m_destX = ttarget->GetPositionX();
			m_targets.m_destY = ttarget->GetPositionY();
			m_targets.m_destZ = ttarget->GetPositionZ();
		}
		else
		{
//			SendCastResult( SPELL_FAILED_BAD_TARGETS );
			p_caster->SendKnockbackPacket( cosf(m_caster->GetOrientation()), sinf(m_caster->GetOrientation()), radius, -10 );
			return;
		}
	}


//	float speed = 137.0f / 15.0f; //well this is just 1 example from blizz :p
	float speed = 3.0f; //well this is just 1 example from blizz :p
	uint32 timetofly = (uint32)(radius * speed);
//	float raise_mob_to_height = m_targets.m_destZ + distt * 250.f / 9.0f; //seems a lot 
	float raise_mob_to_height = radius * 2.5f; //no idea about this value, should be parabolic parameter some sort

	p_caster->GetAIInterface()->SendMoveToPacket( m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ, 0.0f, timetofly, MONSTER_MOVE_FLAG_HEROIC_LEAP, raise_mob_to_height );
	p_caster->SpeedCheatDelay( radius * 1000 / speed  + MIN( 5000, p_caster->GetSession()->GetLatency() ) + 2000 );	//should start sampling again after we land
}

void Spell::SpellEffectJumpBehindTargetHighArc(uint32 i) // Leap
{
	if( p_caster == NULL )
		return;

	float radius = GetRadius(i);

	if( m_targets.m_destX != 0 )
		radius = sqrt( Distance2DSq( unitTarget->GetPositionX(), unitTarget->GetPositionY(), m_targets.m_destX, m_targets.m_destY ) );
	else
	{
		Unit *ttarget = unitTarget;
		if( ( ttarget == NULL || ttarget == m_caster ) && p_caster != NULL )
			ttarget = m_caster->GetMapMgr()->GetUnit( p_caster->GetSelection() );
		if( ttarget != NULL && ttarget != m_caster )
		{
			radius = MIN( radius, sqrt( m_caster->GetDistance2dSq( ttarget ) ) );
			m_targets.m_destX = ttarget->GetPositionX();
			m_targets.m_destY = ttarget->GetPositionY();
			m_targets.m_destZ = ttarget->GetPositionZ();
		}
		else
		{
//			SendCastResult( SPELL_FAILED_BAD_TARGETS );
			p_caster->SendKnockbackPacket( cosf(m_caster->GetOrientation()), sinf(m_caster->GetOrientation()), radius, -10 );
			return;
		}
	}


//	float speed = 137.0f / 15.0f; //well this is just 1 example from blizz :p
	float speed = 237.0f / 15.0f; //if not large enough, it will make the jump quick. If too large it raises people to the skies
//	float raise_mob_to_height = m_targets.m_destZ + distt * 250.f / 9.0f; //seems a lot 
	float raise_mob_to_height = radius; //no idea about this value, should be parabolic parameter some sort
	uint32 timetofly = (uint32)( radius * M_PI * speed );

	p_caster->GetAIInterface()->SendMoveToPacket( m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ, 0.0f, timetofly, MONSTER_MOVE_FLAG_HEROIC_LEAP, raise_mob_to_height );
	p_caster->SpeedCheatDelay( radius * 1000 / speed  + MIN( 5000, p_caster->GetSession()->GetLatency() ) + 2000 );	//should start sampling again after we land
}

void Spell::SpellEffectEnergize(uint32 i) // Energize
{
	if(!unitTarget || !unitTarget->isAlive())
	{ 
		return;
	}

	uint32 modEnergy = 0;
	//paladin - Spiritual Attunement 
	if(GetProto()->Id==31786 && ProcedOnSpell)
	{
		SpellEntry *motherspell=dbcSpell.LookupEntry(pSpellId);
		if(motherspell)
		{
			//heal amount from procspell (we only proced on a heal spell)
			uint32 healamt=0;
			for(uint32 i=0;i<MAX_SPELL_EFFECT_COUNT;i++)
				if(ProcedOnSpell->eff[i].Effect==SPELL_EFFECT_HEAL || ProcedOnSpell->eff[i].Effect==SPELL_EFFECT_SCRIPT_EFFECT)
				{
					healamt=ProcedOnSpell->eff[i].EffectBasePoints+1;
					break;
				}
			modEnergy = (motherspell->eff[0].EffectBasePoints+1)*(healamt)/100;
		}
	}
	else  
        modEnergy = damage * forced_pct_mod_cur / 100;

	uint32 ID_for_the_logs;
	if( GetProto()->spell_id_client )
		ID_for_the_logs = GetProto()->spell_id_client;
	else
		ID_for_the_logs = GetProto()->Id;
	uint32 PowerType;
	if( forced_miscvalues[i] != 0 )
		PowerType = forced_miscvalues[i];
	else
		PowerType = GetProto()->eff[i].EffectMiscValue;

	if( u_caster )
		u_caster->Energize( unitTarget, ID_for_the_logs, modEnergy, PowerType, 0 );
	else
		unitTarget->Energize( unitTarget, ID_for_the_logs, modEnergy, PowerType, 0 );
}

void Spell::SpellEffectEnergizeMaxEnergyPct(uint32 i)
{
	if(!unitTarget || !unitTarget->isAlive())
	{ 
		return;
	}

	uint32 PowerType;
	if( forced_miscvalues[i] != 0 )
		PowerType = forced_miscvalues[i];
	else
		PowerType = GetProto()->eff[i].EffectMiscValue;

	if( PowerType > 5 )
	{ 
		return;
	}

	uint32 maxEnergy = unitTarget->GetMaxPower( PowerType );
	u_caster->Energize( unitTarget, GetProto()->Id, maxEnergy * damage / 100, PowerType, 0 );
}

void Spell::SpellEffectWeaponDmgPerc(uint32 i) // Weapon Percent damage
{
	if(!unitTarget  || !u_caster)
	{ 
		return;
	}

	if( GetType() == SPELL_DMG_TYPE_MAGIC )
	{
		dmg_storage_struct tdmg;
		float fdmg;
		CalculateDamage( u_caster, unitTarget, MELEE, 0, GetProto(), tdmg );
		int32 dmg_diff = tdmg.base_max - tdmg.base_min;
		dmg_diff = MAX( 1, dmg_diff );
		fdmg = (float)(tdmg.base_min + ( RandomUInt() % dmg_diff ));
		fdmg = (float)(fdmg * tdmg.pct_mod_add / 100);
		fdmg += (float)(tdmg.flat_mod_add);

		uint32 dmg = float2int32(fdmg*(float(damage/100.0f)));

		//avoid dmg overflows
		if( dmg < 0 || dmg > 100000 )
		{ 
			return;
		}

		dealdamage ttdmg;
		ttdmg.base_dmg = dmg;
		ttdmg.pct_mod_final_dmg = forced_pct_mod_cur;
		ttdmg.StaticDamage = ( static_dmg[i] != 0 );
		u_caster->SpellNonMeleeDamageLog( unitTarget, GetProto(), &ttdmg, 1, i );
	}
	else
	{
		uint32 _type;
		if( GetType() == SPELL_DMG_TYPE_RANGED )
			_type = RANGED;
		else
		{
			if (GetProto()->AttributesExC & 0x1000000)
				_type =  OFFHAND;
			else
				_type = MELEE;
		}

		//avoid dmg overflows
		if( damage < 0 || damage > 200000 || add_damage < 0 || add_damage > 200000 )
		{ 
			return;
		}

//		if( GetProto()->NameHash == SPELL_HASH_SEAL_OF_BLOOD || GetProto()->NameHash == SPELL_HASH_SEAL_OF_THE_MARTYR )
//			forced_basepoints[1] = damage * (GetProto()->eff[1].EffectBasePoints + 1 ) / 100;

//		int32 add_pct_dmg = damage - 100 + forced_pct_mod_cur - 100; // !!! function parameter ads the dmg pct to normal dmg !!! 
		int32 add_pct_dmg = ( damage * forced_pct_mod_cur / 100 ) - 100; // !!! function parameter ads the dmg pct to normal dmg !!! 

#ifdef USE_SPELL_MISSILE_DODGE
		if(GetProto()->speed > 0 && _type == RANGED )
		{
			float dist = m_caster->CalcDistance( unitTarget );
			float time = ((dist*1000.0f)/GetProto()->speed);
			LocationVector src,dst;
			src.x = m_caster->GetPositionX();
			src.y = m_caster->GetPositionY();
			src.z = m_caster->GetPositionZ();
			dst.x = unitTarget->GetPositionX();
			dst.y = unitTarget->GetPositionY();
			dst.z = unitTarget->GetPositionZ();
			sEventMgr.AddEvent(m_caster, &Object::EventWeaponDamageToLocation, unitTarget->GetGUID(), GetProto(), (uint32)add_damage, (int32)add_pct_dmg, (int32)i, src, dst, EVENT_SPELL_DAMAGE_HIT, uint32(time), 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
		}
		else
			u_caster->Strike( unitTarget, _type, GetProto(), add_damage, add_pct_dmg, 0, false, false );
#else
		u_caster->Strike( unitTarget, _type, GetProto(), add_damage, add_pct_dmg, 0, false, false );
#endif
	}
}

void Spell::SpellEffectTriggerMissile(uint32 i) // Trigger Missile
{
	//Used by mortar team
	//Triggers area affect spell at destinatiom
	if(!m_caster)
	{ 
		return;
	}

	uint32 spellid = GetProto()->eff[i].EffectTriggerSpell;
	if(spellid == 0)
	{ 
		return;
	}

	SpellEntry *spInfo = dbcSpell.LookupEntryForced(spellid);
	if(!spInfo)
	{ 
		return;
	}

	SpellRadius *sr = dbcSpellRadius.LookupEntryForced( spInfo->eff[0].EffectRadiusIndex );	//should try to guess the best radius for this spell :(
	float spellRadius;
	if( sr )
		spellRadius = sr->Radius;
	else
		spellRadius = GetRadius( i );

	m_caster->AquireInrangeLock();
	InrangeLoopExitAutoCallback AutoLock;
	for(InRangeSetRecProt::iterator itr = m_caster->GetInRangeSetBegin( AutoLock ); itr != m_caster->GetInRangeSetEnd(); itr++ )
	{
		if(!((*itr)->IsUnit()) || !SafeUnitCast((*itr))->isAlive())
			continue;
		Unit *t=SafeUnitCast(*itr);
	
		float r;
		float d=m_targets.m_destX-t->GetPositionX();
		r=d*d;
		d=m_targets.m_destY-t->GetPositionY();
		r+=d*d;
		d=m_targets.m_destZ-t->GetPositionZ();
		r+=d*d;
		if(sqrt(r) - SafeUnitCast(*itr)->GetModelHalfSize() > spellRadius)
			continue;
		
		if(!isAttackable(m_caster, SafeUnitCast(*itr)))//Fixme only enemy targets?
			continue;

		Spell*sp=SpellPool.PooledNew( __FILE__, __LINE__ );
		sp->Init(m_caster,spInfo,true,NULL);
		SpellCastTargets tgt;
		tgt.m_unitTarget=(*itr)->GetGUID();
		tgt.m_targetMask |= TARGET_FLAG_DEST_LOCATION;
		tgt.m_destX = m_targets.m_destX;
		tgt.m_destY = m_targets.m_destY;
		tgt.m_destZ = m_targets.m_destZ;
		sp->prepare(&tgt);
	}
	m_caster->ReleaseInrangeLock();
}

void Spell::SpellEffectOpenLock(uint32 i) // Open Lock
{
	if(!p_caster)
	{ 
		return;
	}
		
	uint8 loottype = 0;

//  Zack : wtf ? we are using the same spell to open locks, we cannot use same lock type
	uint32 locktype=GetProto()->eff[i].EffectMiscValue;
	//Zack : probably this is the same for items also. Lock type is obtained from proto lock field
	if( gameObjTarget && ( gameObjTarget->GetInfo()->Type == GAMEOBJECT_TYPE_CHEST || gameObjTarget->GetInfo()->Type == GAMEOBJECT_TYPE_GOOBER ) )
	{
        Lock *pLock = dbcLock.LookupEntry( gameObjTarget->GetInfo()->SpellFocus );
        if(pLock)
            for(uint32 i=0; i < 5; i++)
               if( pLock->locktype[i] == GO_LOCKTYPE_SKILL_REQ && pLock->lockmisc[i] != 0 )
			   {
				   locktype = pLock->lockmisc[i];
				   break;
			   }
	}

	//zack: o fuck me hard, another hack. Archaeology loot until currency loot is properly implemented
	if( p_caster && gameObjTarget && p_caster->m_digsite.loot_GO_entry == gameObjTarget->GetEntry() )
	{
		p_caster->LootResearchChest( gameObjTarget );
		loottype = LOOT_CORPSE;
	}

	switch(locktype)
	{
		case LOCKTYPE_DISARM_TRAP:
		{
			if(gameObjTarget && gameObjTarget->GetInfo()->Type == GAMEOBJECT_TYPE_TRAP )
				gameObjTarget->Despawn(0);
		}break;
		case LOCKTYPE_PICKLOCK:
		{
			uint32 v = 0;
			uint32 lockskill = p_caster->_GetSkillLineCurrent(SKILL_LOCKPICKING);

			if(itemTarget)
			{	
				if(!itemTarget->locked)
				{ 
					SendCastResult( SPELL_FAILED_BAD_TARGETS );
					return;
				}
						
				Lock *lock = dbcLock.LookupEntry( itemTarget->GetProto()->LockId );
				if(!lock) 
				{ 
					SendCastResult( SPELL_FAILED_BAD_TARGETS );
					return;
				}
				for(int i=0;i<5;i++)
					if(lock->locktype[i] == GO_LOCKTYPE_SKILL_REQ && lock->minlockskill[i] && lockskill >= lock->minlockskill[i])
					{
						v = lock->minlockskill[i];
						itemTarget->locked = false;
						itemTarget->SetFlag(ITEM_FIELD_FLAGS, ITEM_FLAG_LOOTABLE ); // unlock
						DetermineSkillUp(SKILL_LOCKPICKING, 1, v/5);
						break;
					}
				if( v == 0 )
					SendCastResult( SPELL_FAILED_MIN_SKILL );
			}
			else if(gameObjTarget)
			{
				GameObjectInfo *info = GameObjectNameStorage.LookupEntry(gameObjTarget->GetEntry());
				if(!info || gameObjTarget->GetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE) == 0) 
				{ 
					return;
				}
				Lock *lock = dbcLock.LookupEntry( info->SpellFocus );
				if(lock == 0)
				{ 
					return;
				}

				for(int i=0;i<5;i++)
				{
					if(lock->locktype[i] == GO_LOCKTYPE_SKILL_REQ && lock->minlockskill[i] && lockskill >= lock->minlockskill[i])
					{
						v = lock->minlockskill[i];
						gameObjTarget->SetUInt32Value(GAMEOBJECT_FLAGS, 0);
						gameObjTarget->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, 1);
						//Add Fill GO loot here
						if(gameObjTarget->loot.items.empty() == true)
						{
							//lootmgr.FillGOLoot(&gameObjTarget->loot,gameObjTarget->GetEntry(), gameObjTarget->GetMapMgr() ? (gameObjTarget->GetMapMgr()->iInstanceMode ? true : false) : false);
							// zack : blizz recycles gameobject entry. We use something that is same for all entry variations
							lootmgr.FillGOLoot(&gameObjTarget->loot,gameObjTarget->GetInfo()->sound1, gameObjTarget->GetMapMgr() ? (gameObjTarget->GetMapMgr()->instance_difficulty ? true : false) : false);
							DetermineSkillUp(SKILL_LOCKPICKING,1, v/5); //to prevent free skill up
						}
						loottype = LOOT_CORPSE;
						//End of it
						break;
					}
				}
			}
		}
		break;
		case LOCKTYPE_HERBALISM:
		{
			if(!gameObjTarget) 
			{ 
				return;	  
			}
			
			uint32 v = gameObjTarget->GetGOReqSkill();
			bool bAlreadyUsed = false;
		 
//			if(RandChance(100)) // 3% chance to fail//why?
			{
				if( SafePlayerCast( m_caster )->_GetSkillLineMax( SKILL_HERBALISM ) == 0 )
				{
					SendCastResult( SPELL_FAILED_MIN_SKILL, SKILL_HERBALISM );
					return;
				}
				if( SafePlayerCast( m_caster )->_GetSkillLineCurrent( SKILL_HERBALISM ) < v )
				{
					SendCastResult( SPELL_FAILED_MIN_SKILL, SKILL_HERBALISM );
					return;
				}
				else
				{
					if( gameObjTarget->loot.items.empty() == true )
					{
//						lootmgr.FillGOLoot(&gameObjTarget->loot,gameObjTarget->GetEntry(), gameObjTarget->GetMapMgr() ? (gameObjTarget->GetMapMgr()->iInstanceMode ? true : false) : false);
						// zack : blizz recycles gameobject entry. We use something that is same for all entry variations
						lootmgr.FillGOLoot(&gameObjTarget->loot,gameObjTarget->GetInfo()->sound1, gameObjTarget->GetMapMgr() ? (gameObjTarget->GetMapMgr()->instance_difficulty) : 0);
					}
					else
						bAlreadyUsed = true;
				}
				loottype = LOOT_SKINNING;
			}
			//Skill up
			if(!bAlreadyUsed) //Avoid cheats with opening/closing without taking the loot
				DetermineSkillUp(SKILL_HERBALISM,1, v/5); 
		}
		break;
		case LOCKTYPE_MINING:
		{
			if(!gameObjTarget) 
			{ 
				return;
			}

			uint32 v = gameObjTarget->GetGOReqSkill();
			bool bAlreadyUsed = false;

//			if( Rand( 100.0f ) ) // 3% chance to fail//why?
			{
				//check if we have a pickaxe
				if( p_caster->GetItemInterface()->HasItemWithTotemCategory( TOTEM_CATEGORY_MINING_PICK1 ) == false
					&& p_caster->GetItemInterface()->HasItemWithTotemCategory( TOTEM_CATEGORY_MINING_PICK2 ) == false
					&& p_caster->GetItemInterface()->HasItemWithTotemCategory( TOTEM_CATEGORY_MINING_PICK3 ) == false
					&& p_caster->GetItemInterface()->HasItemWithTotemCategory( TOTEM_CATEGORY_MINING_PICK4 ) == false )
				{
					SendCastResult( SPELL_FAILED_TOTEMS, TOTEM_CATEGORY_MINING_PICK1 );
					return;
				}
				if( SafePlayerCast( m_caster )->_GetSkillLineMax( SKILL_MINING ) == 0 )
				{
					SendCastResult( SPELL_FAILED_MIN_SKILL, SKILL_MINING );
					return;
				}
				if( SafePlayerCast( m_caster )->_GetSkillLineCurrent( SKILL_MINING ) < v )
				{
					SendCastResult( SPELL_FAILED_MIN_SKILL, SKILL_MINING );
					return;
				}
				else if( gameObjTarget->loot.items.empty() == true )
				{
//					lootmgr.FillGOLoot(&gameObjTarget->loot,gameObjTarget->GetEntry(), gameObjTarget->GetMapMgr() ? (gameObjTarget->GetMapMgr()->iInstanceMode ? true : false) : false);
					// zack : blizz recycles gameobject entry. We use something that is same for all entry variations
					lootmgr.FillGOLoot(&gameObjTarget->loot,gameObjTarget->GetInfo()->sound1, gameObjTarget->GetMapMgr() ? (gameObjTarget->GetMapMgr()->instance_difficulty) : 0);
				}	
				else
					bAlreadyUsed = true;

				loottype = LOOT_SKINNING;
			}
//			else
			{
//				SendCastResult(SPELL_FAILED_TRY_AGAIN);
			}
			//Skill up
			if(!bAlreadyUsed) //Avoid cheats with opening/closing without taking the loot
				DetermineSkillUp(SKILL_MINING,1,v/5);
		}
		break;
		case LOCKTYPE_SLOW_OPEN: // used for BG go's
		{
			if(!gameObjTarget ) 
			{ 
				return;
			}
			if(p_caster && p_caster->m_bg)
				if(p_caster->m_bg->HookSlowLockOpen(gameObjTarget,p_caster,this))
				{ 
					return;
				}

			uint32 spellid = !gameObjTarget->GetInfo()->Unknown1 ? 23932 : gameObjTarget->GetInfo()->Unknown1;
			SpellEntry*en=dbcSpell.LookupEntry(spellid);
			Spell *sp=SpellPool.PooledNew( __FILE__, __LINE__ );
			sp->Init(p_caster,en,true,NULL);
			SpellCastTargets tgt;
			tgt.m_unitTarget=gameObjTarget->GetGUID();
			sp->prepare(&tgt);
			return;
		}	
		break;
		case LOCKTYPE_QUICK_CLOSE:
			{
				if(!gameObjTarget ) 
				{ 
					return;
				}
				gameObjTarget->EventCloseDoor();
			}
		break;
		default://not profession
		{
			if(!gameObjTarget ) 
			{ 
				return;
			}

			if( gameObjTarget->GetByte( GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_TYPEID ) == GAMEOBJECT_TYPE_GOOBER)
					CALL_GO_SCRIPT_EVENT(gameObjTarget, OnActivate)(SafePlayerCast(p_caster));
			CALL_INSTANCE_SCRIPT_EVENT( gameObjTarget->GetMapMgr(), OnGameObjectActivate )( gameObjTarget, p_caster ); 
			
			if(sQuestMgr.OnActivateQuestGiver(gameObjTarget, p_caster))
			{ 
				return;
			}

			if(sQuestMgr.OnGameObjectActivate(p_caster, gameObjTarget))
			{
				p_caster->UpdateNearbyGameObjects();
				return;
			}

			if(gameObjTarget->loot.items.empty() == true)
			{
				//lootmgr.FillGOLoot(&gameObjTarget->loot,gameObjTarget->GetEntry(), gameObjTarget->GetMapMgr() ? (gameObjTarget->GetMapMgr()->iInstanceMode ? true : false) : false);
				// zack : blizz recycles gameobject entry. We use something that is same for all entry variations
				lootmgr.FillGOLoot(&gameObjTarget->loot,gameObjTarget->GetInfo()->sound1, gameObjTarget->GetMapMgr() ? (gameObjTarget->GetMapMgr()->instance_difficulty) : 0);
			}
			loottype= LOOT_CORPSE ;
		}
		break;
	};
	if( gameObjTarget != NULL && gameObjTarget->GetByte( GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_TYPEID ) == GAMEOBJECT_TYPE_CHEST)
		SafePlayerCast( m_caster )->SendLoot( gameObjTarget->GetGUID(), loottype );
}

void Spell::SpellEffectOpenLockItem(uint32 i)
{
	Unit* caster = u_caster;
	if(!caster && i_caster)
		caster = i_caster->GetOwner();

	if( p_caster != NULL && i_caster != NULL )
	{
		ItemPrototype *ip = i_caster->GetProto();
		if( ip == NULL )
			return;
		else
			p_caster->HandleSpellLoot( ip->ItemId );
	}

	if(!gameObjTarget || !gameObjTarget->IsInWorld()) 
	{ 
		return;
	}
	
	if( caster && caster->IsPlayer() && sQuestMgr.OnGameObjectActivate( (SafePlayerCast(caster)), gameObjTarget ) )
		SafePlayerCast(caster)->UpdateNearbyGameObjects();

	if( p_caster )
	{
		CALL_GO_SCRIPT_EVENT(gameObjTarget, OnActivate)(SafePlayerCast(caster));
		CALL_INSTANCE_SCRIPT_EVENT( gameObjTarget->GetMapMgr(), OnGameObjectActivate )( gameObjTarget, p_caster ); 
	}
	gameObjTarget->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, 0);	

	if( gameObjTarget->GetEntry() == 183146)
	{
		gameObjTarget->Despawn(1);
		return;
	}

	if( gameObjTarget->GetByte( GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_TYPEID ) == GAMEOBJECT_TYPE_CHEST)
	{
//		lootmgr.FillGOLoot(&gameObjTarget->loot,gameObjTarget->GetEntry(), gameObjTarget->GetMapMgr() ? (gameObjTarget->GetMapMgr()->iInstanceMode ? true : false) : false);
		// zack : blizz recycles gameobject entry. We use something that is same for all entry variations
		lootmgr.FillGOLoot(&gameObjTarget->loot,gameObjTarget->GetInfo()->sound1, gameObjTarget->GetMapMgr() ? (gameObjTarget->GetMapMgr()->instance_difficulty) : 0);
		if(gameObjTarget->loot.items.size() > 0)
		{
			SafePlayerCast(caster)->SendLoot(gameObjTarget->GetGUID(),LOOT_CORPSE);
		}
	}

	if( gameObjTarget->GetByte( GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_TYPEID ) == GAMEOBJECT_TYPE_DOOR)
		gameObjTarget->SetUInt32Value(GAMEOBJECT_FLAGS, 33);

	if( IS_STATIC_MAP( gameObjTarget->GetMapMgr()->GetMapInfo() ) )//dont close doors for instances
		sEventMgr.AddEvent(gameObjTarget,&GameObject::EventCloseDoor, EVENT_GAMEOBJECT_DOOR_CLOSE,10000,1,0);
	
	sEventMgr.AddEvent(gameObjTarget, &GameObject::Despawn, (uint32)1, EVENT_GAMEOBJECT_ITEM_SPAWN, 6*60*1000, 1, 0);
}

void Spell::SpellEffectProficiency(uint32 i)
{
//	uint32 skill;
//	skilllinespell* skillability = dbcSkillLineSpell.LookupEntryForced( GetProto()->spell_skilline[0] );
//	if (skillability)
//		skill = skillability->skilline;
	if( GetProto()->spell_skilline_assoc_counter == 0 )
		sLog.outDebug("Spell Proficiency that has no skill to teach ! %u \n",GetProto()->Id);

	for(uint32 i=0;i<GetProto()->spell_skilline_assoc_counter;i++)
	{
		skilllineentry* sk = dbcSkillLine.LookupEntry( GetProto()->spell_skilline[i] );
		uint32 skill = sk->id;
		if(skill)
		{
			if(playerTarget)
			{
				//stop spamming ourself with self teach stuff in case of playercreate spells
	//			playerTarget->removeSpell( GetProto()->Id, false, false, 0 );
				if(playerTarget->_HasSkillLine(skill))
				{
					// Increase it by one
				   // playerTarget->AdvanceSkillLine(skill);
				}
				else
				{
					// Don't add skills to players logging in.
					/*if((GetProto()->Attributes & 64) && playerTarget->m_TeleportState == 1)
						return;*/

					if(sk && sk->type == SKILL_TYPE_WEAPON)
						playerTarget->_AddSkillLine(skill, 1, 5*playerTarget->getLevel());
					else
						playerTarget->_AddSkillLine(skill, 1, 1);				
				}
			}
		}
		else
			sLog.outDebug("Spell Proficiency that has no skill to teach ! %u \n",GetProto()->Id);
	}
}

void Spell::SpellEffectSendEvent(uint32 i) //Send Event
{
	//This is mostly used to trigger events on quests or some places

	uint32 spellId = GetProto()->Id;

	// Try a dummy SpellHandler
	if(sScriptMgr.CallScriptedDummySpell(GetProto(), i, this))
	{ 
		return;
	}

	switch(spellId)
	{

	// WSG Flags
	case 23333:
	case 23335:
		{
			if( !p_caster )
				return;
			/* set the flag holder */
			p_caster->m_bgHasFlag = true;

			if( p_caster->GetTeam() == 1 )
				p_caster->m_bg->SendChatMessage( CHAT_MSG_BG_EVENT_HORDE, p_caster->GetGUID(), "The Alliance flag was picked up by %s!", p_caster->GetName() );
			else
				p_caster->m_bg->SendChatMessage( CHAT_MSG_BG_EVENT_ALLIANCE, p_caster->GetGUID(), "The Horde flag was picked up by %s!", p_caster->GetName() );
		}break;

	// Place Loot
	case 25720: // Places the Bag of Gold at the designated Drop-Off Point.
		{

		}break;

	// Item - Cleansing Vial DND
	case 29297: // Empty the vial near the Bones of Aggonar to cleanse the waters of their demonic taint.
		{
			if( p_caster )
			{
				QuestLogEntry *en=p_caster->GetQuestLogForEntry(9427);
				if(!en)
				{ 
					return;
				}
				en->SendQuestComplete();
			}
		}break;

	//Warlock: Summon Succubus Quest
	case 8674:
	case 9223:
	case 9224:
		{
			if( !p_caster )
				return;
			CreatureInfo * ci = CreatureNameStorage.LookupEntry(5677);
			CreatureProto * cp = CreatureProtoStorage.LookupEntry(5677);
			if( !ci || !cp )
			{ 
				return;
			}

		   Creature * pCreature = p_caster->GetMapMgr()->CreateCreature(cp->Id);
		   pCreature->Load(cp, p_caster->GetPositionX(), p_caster->GetPositionY(), p_caster->GetPositionZ());
		   pCreature->_setFaction();
		   pCreature->GetAIInterface()->Init(pCreature,AITYPE_AGRO,MOVEMENTTYPE_NONE);
		   pCreature->GetAIInterface()->taunt(p_caster,true);
		   pCreature->_setFaction();
//		   pCreature->m_noRespawn = true;
		   pCreature->PushToWorld(p_caster->GetMapMgr());
		   sEventMgr.AddEvent(pCreature, &Creature::SafeDelete, EVENT_CREATURE_REMOVE_CORPSE,60000, 1, 0);
		}break;

	//Warlock: Summon Voidwalker Quest
	case 30208:
	case 9221:
	case 9222:
	case 7728:
		{
			if( !p_caster )
				return;
			CreatureInfo * ci = CreatureNameStorage.LookupEntry(5676);
			CreatureProto * cp = CreatureProtoStorage.LookupEntry(5676);
			if( !ci || !cp )
			{ 
				return;
			}

		   Creature * pCreature = p_caster->GetMapMgr()->CreateCreature(cp->Id);
		   pCreature->Load(cp, p_caster->GetPositionX(), p_caster->GetPositionY(), p_caster->GetPositionZ());
		   pCreature->_setFaction();
		   pCreature->GetAIInterface()->Init(pCreature,AITYPE_AGRO,MOVEMENTTYPE_NONE);
		   pCreature->GetAIInterface()->taunt(p_caster,true);
		   pCreature->_setFaction();
//		   pCreature->m_noRespawn = true;
		   pCreature->PushToWorld(p_caster->GetMapMgr());
		   sEventMgr.AddEvent(pCreature, &Creature::SafeDelete, EVENT_CREATURE_REMOVE_CORPSE,60000, 1, 0);
		}break;

	//Warlock: Summon Felhunter Quest
	case 8712:
		{
			if( !p_caster )
				return;
			CreatureInfo * ci = CreatureNameStorage.LookupEntry(6268);
			CreatureProto * cp = CreatureProtoStorage.LookupEntry(6268);
			if( !ci || !cp )
			{ 
				return;
			}

		   Creature * pCreature = p_caster->GetMapMgr()->CreateCreature(cp->Id);
		   pCreature->Load(cp, p_caster->GetPositionX(), p_caster->GetPositionY(), p_caster->GetPositionZ());
		   pCreature->_setFaction();
		   pCreature->GetAIInterface()->Init(pCreature,AITYPE_AGRO,MOVEMENTTYPE_NONE);
		   pCreature->GetAIInterface()->taunt(p_caster,true);
		   pCreature->_setFaction();
//		   pCreature->m_noRespawn = true;
		   pCreature->PushToWorld(p_caster->GetMapMgr());
		   sEventMgr.AddEvent(pCreature, &Creature::SafeDelete, EVENT_CREATURE_REMOVE_CORPSE,60000, 1, 0);
		}break;

	}
}

void Spell::SpellEffectApplyAA(uint32 i) // Apply Area Aura
{
	if(!unitTarget || !unitTarget->isAlive())
	{ 
		return;
	}
	if(u_caster!=unitTarget)
	{ 
		return;
	}

	//remove already existing area auras with same namehash to avoid stacking them on login. Ex : Abomination's Might with different / same ranks
	unitTarget->RemoveAuraByNameHash( GetProto()->NameHash );

	Aura *pAura = unitTarget->FindQueuedAura( GetProto()->Id );
//	std::map<uint32,Aura*>::iterator itr=unitTarget->tmpAura.find(GetProto()->Id);
//	if(itr==unitTarget->tmpAura.end())
	if( pAura == NULL )
	{
		pAura=AuraPool.PooledNew( __FILE__, __LINE__ );
		pAura->Init(GetProto(),GetDuration(),m_caster,unitTarget);
//		unitTarget->tmpAura[ GetProto()->Id ]= pAura;
		unitTarget->QueueAuraAdd.push_front( pAura );
		//hmm, if we do not set this it will stack with other auras from other players. Ex : Abomination's Might with different / same ranks
//		pAura->SetIsAreaAura();
	}

	if(!sEventMgr.HasEvent(pAura, EVENT_AREAAURA_UPDATE))		/* only add it once */
	{
		float r=GetRadius(i);
		sEventMgr.AddEvent(pAura, &Aura::EventUpdateAA,r*r, EVENT_AREAAURA_UPDATE, 1000, 0,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);	
	}

	pAura->AddMod(GetProto()->eff[i].EffectApplyAuraName,damage,GetProto()->eff[i].EffectMiscValue,i,forced_pct_mod_cur);
}

void Spell::SpellEffectLearnSpell(uint32 i) // Learn Spell
{
	if(playerTarget == 0 && unitTarget && unitTarget->IsPet())
	{
		// bug in target map fill?
		//playerTarget = m_caster->GetMapMgr()->GetPlayer((uint32)m_targets.m_unitTarget);
		SpellEffectLearnPetSpell(i);
		return;
	}

	if( GetProto()->Id == 483 || GetProto()->Id == 55884 )		// "Learning"
	{
		if( !i_caster || !p_caster )
		{ 
			return;
		}

		uint32 spellid = 0;
		for(int i = 0; i < 5; ++i)
		{
			if( i_caster->GetProto()->Spells.Trigger[i] == LEARNING && i_caster->GetProto()->Spells.Id[i] != 0 )
			{
				spellid = i_caster->GetProto()->Spells.Id[i];
				break;
			}
		}

		if( !spellid || !dbcSpell.LookupEntryForced(spellid) )
		{ 
			return;
		}

		// learn me!
		p_caster->addSpell( spellid );

		// no normal handler
		return;
	}

	if(playerTarget)
	{
		uint32 spellToLearn = GetProto()->eff[i].EffectTriggerSpell;
		playerTarget->addSpell(spellToLearn);
		//smth is wrong here, first we add this spell to player then we may cast it on player...
		SpellEntry *spellinfo = dbcSpell.LookupEntry(spellToLearn);
 		//remove specializations
		switch(spellinfo->Id)
 		{
		case 26801: //Shadoweave Tailoring
			playerTarget->removeSpell(26798,false,false,0); //Mooncloth Tailoring
			playerTarget->removeSpell(26797,false,false,0); //Spellfire Tailoring
 			break;
		case 26798: // Mooncloth Tailoring
			playerTarget->removeSpell(26801,false,false,0); //Shadoweave Tailoring
			playerTarget->removeSpell(26797,false,false,0); //Spellfire Tailoring
 			break;
		case 26797: //Spellfire Tailoring
			playerTarget->removeSpell(26801,false,false,0); //Shadoweave Tailoring
			playerTarget->removeSpell(26798,false,false,0); //Mooncloth Tailoring
 			break;
 		case 10656: //Dragonscale Leatherworking
			playerTarget->removeSpell(10658,false,false,0); //Elemental Leatherworking
			playerTarget->removeSpell(10660,false,false,0); //Tribal Leatherworking
 			break;
 		case 10658: //Elemental Leatherworking
			playerTarget->removeSpell(10656,false,false,0); //Dragonscale Leatherworking
			playerTarget->removeSpell(10660,false,false,0); //Tribal Leatherworking
 			break;
 		case 10660: //Tribal Leatherworking
			playerTarget->removeSpell(10656,false,false,0); //Dragonscale Leatherworking
			playerTarget->removeSpell(10658,false,false,0); //Elemental Leatherworking
 			break;
 		case 28677: //Elixir Master
			playerTarget->removeSpell(28675,false,false,0); //Potion Master
			playerTarget->removeSpell(28672,false,false,0); //Transmutation Maste
 			break;
 		case 28675: //Potion Master
			playerTarget->removeSpell(28677,false,false,0); //Elixir Master
			playerTarget->removeSpell(28672,false,false,0); //Transmutation Maste
 			break;
 		case 28672: //Transmutation Master
			playerTarget->removeSpell(28675,false,false,0); //Potion Master
			playerTarget->removeSpell(28677,false,false,0); //Elixir Master
 			break;
 		case 20219: //Gnomish Engineer
			playerTarget->removeSpell(20222,false,false,0); //Goblin Engineer
 			break;
 		case 20222: //Goblin Engineer
			playerTarget->removeSpell(20219,false,false,0); //Gnomish Engineer
 			break;
 		case 9788: //Armorsmith
			playerTarget->removeSpell(9787,false,false,0); //Weaponsmith
			playerTarget->removeSpell(17039,false,false,0); //Master Swordsmith
			playerTarget->removeSpell(17040,false,false,0); //Master Hammersmith
			playerTarget->removeSpell(17041,false,false,0); //Master Axesmith
 			break;
 		case 9787: //Weaponsmith
			playerTarget->removeSpell(9788,false,false,0); //Armorsmith
 			break;
 		case 17041: //Master Axesmith
			playerTarget->removeSpell(9788,false,false,0); //Armorsmith
			playerTarget->removeSpell(17040,false,false,0); //Master Hammersmith
			playerTarget->removeSpell(17039,false,false,0); //Master Swordsmith
 			break;
 		case 17040: //Master Hammersmith
			playerTarget->removeSpell(9788,false,false,0); //Armorsmith
			playerTarget->removeSpell(17039,false,false,0); //Master Swordsmith
			playerTarget->removeSpell(17041,false,false,0); //Master Axesmith
 			break;
 		case 17039: //Master Swordsmith
			playerTarget->removeSpell(9788,false,false,0); //Armorsmith
			playerTarget->removeSpell(17040,false,false,0); //Master Hammersmith
			playerTarget->removeSpell(17041,false,false,0); //Master Axesmith
 			break;
 		}
		for(uint32 i=0;i<3;i++)
			if(spellinfo->eff[i].Effect == SPELL_EFFECT_WEAPON ||
			   spellinfo->eff[i].Effect == SPELL_EFFECT_PROFICIENCY ||
			   spellinfo->eff[i].Effect == SPELL_EFFECT_DUAL_WIELD )
			{
				Spell *sp = SpellPool.PooledNew( __FILE__, __LINE__ );
				sp->Init(unitTarget, spellinfo, true, NULL);
				SpellCastTargets targets;
				targets.m_unitTarget = unitTarget->GetGUID();
				targets.m_targetMask = 0x02;
				sp->prepare(&targets);
				break;
			}	  
		return;
	}

	// if we got here... try via pet spells..
	SpellEffectLearnPetSpell(i);
}

void Spell::SpellEffectSpellDefense(uint32 i)
{
	//used to enable this ability. We use it all the time ...
}

void Spell::SpellEffectLearnPetSpell(uint32 i)
{
	/*if(unitTarget && m_caster->GetTypeId() == TYPEID_PLAYER)
	{
		if(unitTarget->IsPet() && unitTarget->GetTypeId() == TYPEID_UNIT)
		{
			SafePlayerCast(m_caster)->AddPetSpell(GetProto()->eff[i].EffectTriggerSpell, unitTarget->GetEntry());
		}
	}*/

	if(unitTarget && unitTarget->IsPet() && p_caster)
	{
		Pet * pPet = SafePetCast( unitTarget );
//		if(pPet->IsSummon())
//			p_caster->AddSummonSpell(unitTarget->GetEntry(), GetProto()->eff[i].EffectTriggerSpell);
		
		pPet->AddSpell( dbcSpell.LookupEntry( GetProto()->eff[i].EffectTriggerSpell ), true );
	}
}

void Spell::SpellEffectDispel(uint32 i) // Dispel
{
	if(!u_caster || !unitTarget)
	{ 
		return;
	}

	//spell that are waiting for SMT mods
	if( damage <= 0 )
	{
		return;
	}

	Aura *aur;
	uint32 start,end;
	bool CanDispelImmunity = ( GetProto()->NameHash == SPELL_HASH_MASS_DISPEL );
	if(isAttackable(u_caster,unitTarget))
	{
		//do not dispell boss encoutner auras. Sorry
		if( unitTarget->IsCreature() )
			CanDispelImmunity = false;
		start=0;
		end=MAX_POSITIVE_AURAS1(unitTarget);
		if( unitTarget->SchoolImmunityAntiEnemy[GetProto()->School] && CanDispelImmunity == false )
		{ 
			return;
		}
	}
	else
	{
		start=MAX_POSITIVE_AURAS;
		end=MAX_NEGATIVE_AURAS1(unitTarget);
	}

	uint32 our_spell_dispel_type = GetProto()->GetDispelType();
	uint32 our_eff_dispel_type;
	if( forced_miscvalues[i] != 0 )
		our_eff_dispel_type = forced_miscvalues[i];
	else
		our_eff_dispel_type = GetProto()->eff[i].EffectMiscValue;
	WorldPacket data(SMSG_SPELLDISPELLOG, 16);

	//this hack is required because some talents/procs add new auras on dispel event. And those get dispelled as well
	Aura *old_list[MAX_AURAS + MAX_PASSIVE_AURAS];
	for(uint32 x=start;x<end;x++)
		old_list[x] = unitTarget->m_auras[x];
	for(uint32 i=start;i<end;i++)
	{
		//dispel a random buff instead a fixed one
		uint32 AvailableIndexesToSteal[MAX_AURAS];
		uint32 UsedIndexes = 0;
		for(uint32 x=start;x<end;x++)
		{
			if( unitTarget->m_auras[x] && old_list[x] == unitTarget->m_auras[x] )//this hack is required because some talents/procs add new auras on dispel event. And those get dispelled as well
			{
				aur = unitTarget->m_auras[x];
				uint32 aur_dispel_type = aur->GetSpellProto()->GetDispelType();
				//Nothing can dispel resurrection sickness;
				if( !aur->IsPassive() 
					&& ( !(aur->GetSpellProto()->Attributes & ATTRIBUTES_IGNORE_INVULNERABILITY) || CanDispelImmunity == true )
					&& ( our_spell_dispel_type == DISPEL_ALL || aur_dispel_type == our_eff_dispel_type )
					&& ( ( aur->m_flags & AURAFLAG_CANT_BE_DISPELLED ) == 0 )	//Unholy Blight only ?
					)
				{
					AvailableIndexesToSteal[ UsedIndexes ] = x;
					UsedIndexes++;
				}
			}
		}
		if( UsedIndexes == 0 )
			break;
		
		uint32 x = AvailableIndexesToSteal[ RandomUInt() % UsedIndexes ];
		aur = unitTarget->m_auras[x];

	//			uint32 UARank = 0;
				data.clear();
				data << m_caster->GetNewGUID();
				data << unitTarget->GetNewGUID();
				data << (uint32)1;//probably dispel type
				data << aur->GetSpellId();
				m_caster->SendMessageToSet(&data,true);

				//this is only used by scripted spells right now
				aur->m_flags |= WAS_REMOVED_ON_DISPEL;

				int32 spell_id = aur->GetSpellId();
				uint32 NameHash = aur->GetSpellProto()->NameHash;
				Unit *aur_caster = aur->GetUnitCaster();
				if( unitTarget != u_caster )
				{
					unitTarget->HandleProc( PROC_ON_PRE_DISPELL_AURA_CASTER_EVENT | PROC_ON_VICTIM, u_caster, GetProto(), &spell_id );
					u_caster->HandleProc( PROC_ON_PRE_DISPELL_AURA_CASTER_EVENT | PROC_ON_NOT_VICTIM, unitTarget, GetProto(), &spell_id );
				}
				else
					u_caster->HandleProc( PROC_ON_PRE_DISPELL_AURA_CASTER_EVENT | PROC_ON_VICTIM | PROC_ON_NOT_VICTIM, unitTarget, GetProto(), &spell_id );
				if( aur_caster && unitTarget != aur_caster && aur_caster != u_caster )
					aur_caster->HandleProc( PROC_ON_PRE_DISPELL_AURA_CASTER_EVENT | PROC_ON_NOT_VICTIM , unitTarget , GetProto(), &spell_id );

				//handleproc might remove us !				
//				if( unitTarget->m_auras[x] == aur )
				{
//					unitTarget->RemoveAura(aur);
					//remove all stacks not just 1. Tested on retail on earth shield with purge
					for(uint32 x=start;x<end;x++)
						if( unitTarget->m_auras[x] 
							&& old_list[x] == unitTarget->m_auras[x] //this hack is required because some talents/procs add new auras on dispel event. And those get dispelled as well
							&& unitTarget->m_auras[x]->GetSpellProto()->NameHash == NameHash )
							{
								unitTarget->m_auras[x]->Remove();
								forced_basepoints[i]++; //count the number of auras removed so far. Should this count stacks too ?
							}
				}
				--damage;
				//inherit target PVP state
				if( unitTarget->IsPlayer() && p_caster && p_caster != SafePlayerCast(unitTarget) && SafePlayerCast(unitTarget)->IsPvPFlagged() == true && p_caster->IsPvPFlagged() == false )
					p_caster->SetPvPFlag();
				//end of charges means to exit now
				if( damage <= 0 )
				{ 
					return;
				}
		}   
}

void Spell::SpellEffectDualWield(uint32 i)
{
	if(m_caster->GetTypeId() != TYPEID_PLAYER) 
	{ 
		return;
	}

	Player *pPlayer = SafePlayerCast( m_caster );

	if( !pPlayer->_HasSkillLine( SKILL_DUAL_WIELD ) )
		 pPlayer->_AddSkillLine( SKILL_DUAL_WIELD, 1, 1 );
	
		// Increase it by one
		//dual wield is 1/1 , it never increases it's not even displayed in skills tab

	//note: probably here must be not caster but unitVictim
}

void Spell::SpellEffectSummonWild(uint32 i)  // Summon Wild
{
	//these are some cretures that have your faction and do not respawn
	//number of creatures is actualy dmg (the usual formula), sometimes =3 sometimes =1
	if( !u_caster || !u_caster->IsInWorld() )
	{ 
		return;
	}
	
	uint32 cr_entry;
	if( forced_miscvalues[i] != 0 )
		cr_entry = forced_miscvalues[i];
	else
		cr_entry = GetProto()->eff[i].EffectMiscValue;

	CreatureProto * proto = CreatureProtoStorage.LookupEntry(cr_entry);
	CreatureInfo * info = CreatureNameStorage.LookupEntry(cr_entry);
	if(!proto || !info)
	{
		sLog.outDetail("Warning : Missing summon creature template %u used by spell %u!",cr_entry,GetProto()->Id);
		return;
	}
	float x, y, z;
	if( m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION && m_targets.m_destX && m_targets.m_destY && m_targets.m_destZ )
	{
		x = m_targets.m_destX;
		y = m_targets.m_destY;
		z = m_targets.m_destZ;
	}
	else
	{
		x = u_caster->GetPositionX();
		y = u_caster->GetPositionY();
		z = u_caster->GetPositionZ();
	}
	for(int i=0;i<damage;i++)
	{
		float m_followAngle=-(float(M_PI)/2*i);
		x += (GetRadius(GetProto()->eff[i].EffectRadiusIndex)*(cosf(m_followAngle+u_caster->GetOrientation())));
		y += (GetRadius(GetProto()->eff[i].EffectRadiusIndex)*(sinf(m_followAngle+u_caster->GetOrientation())));
		Creature * p = u_caster->GetMapMgr()->CreateCreature(cr_entry);
		if( ( GetProto()->c_is_flags2 & SPELL_FLAG2_IS_TRIGGERED_ON_TARGET ) && unitTarget )
		{
			x = unitTarget->GetPositionX();
			y = unitTarget->GetPositionY();
			z = unitTarget->GetPositionZ();
		}
		//ASSERT(p);
		p->Load(proto, x, y, z);
//		p->SetZoneId( m_caster->GetZoneId() );

		//some summons need to know about their owner !
		p->SetUInt64Value( UNIT_FIELD_SUMMONEDBY, m_caster->GetGUID() );
		p->SetUInt64Value( UNIT_FIELD_CREATEDBY, m_caster->GetGUID() );

		if ( p->GetProto() && p->GetProto()->Faction == 35 )
		{
			p->SetUInt32Value( UNIT_FIELD_FACTIONTEMPLATE, u_caster->GetUInt32Value( UNIT_FIELD_FACTIONTEMPLATE ) );
		}
		else
		{
			p->SetUInt32Value( UNIT_FIELD_FACTIONTEMPLATE, proto->Faction );
		}

		p->m_faction = dbcFactionTemplate.LookupEntry(proto->Faction);
		if(p->m_faction)
			p->m_factionDBC = dbcFaction.LookupEntry(p->m_faction->Faction);
//		p->m_noRespawn = true;
		p->PushToWorld(u_caster->GetMapMgr());
		//make sure they will be desumonized (roxor)
		sEventMgr.AddEvent(p, &Creature::SummonExpire, EVENT_SUMMON_EXPIRE, GetDuration(), 1,0);
		if( p_caster )
			SendSummonLog( p_caster, GetProto()->Id, p, 1 );
	}
}

void Spell::SpellEffectSummonGuardian(uint32 i) // Summon Guardian
{
	GameObject * obj = NULL; //Snake trap part 1
	LocationVector * vec = NULL;
	
	if ( m_caster->IsGameObject() ) 
	{
		Unit *GoCreater = m_caster->GetMapMgr()->GetUnit( m_caster->GetUInt64Value( OBJECT_FIELD_CREATED_BY ) );
		if ( GoCreater )
		{
			u_caster = GoCreater; //set the caster to the summoner unit
			obj = SafeGOCast( m_caster ); //and keep the trap info
		}
		else
		{
			return;
		}
	}
	else if ( !u_caster )
	{ 
		return;
	}

	uint32 cr_entry;
	if( forced_miscvalues[i] != 0 )
		cr_entry = forced_miscvalues[i];
	else
		cr_entry = GetProto()->eff[i].EffectMiscValue;
	uint32 level = 0;

	if( GetProto()->c_is_flags & SPELL_FLAG_IS_INHERITING_LEVEL )
		level = u_caster->getLevel();
	/*if ( u_caster->GetTypeId()==TYPEID_PLAYER && itemTarget )
	{
		if (itemTarget->GetProto() && itemTarget->GetProto()->RequiredSkill == SKILL_ENGINEERING)
		{
			uint32 skill202 = SafePlayerCast( m_caster )->_GetSkillLineCurrent(SKILL_ENGINEERING);
			if (skill202>0)
			{
				level = skill202/5;
			}
		}
	}*/
	if( m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION && m_targets.m_destX && m_targets.m_destY && m_targets.m_destZ )
	{
		vec = new LocationVector(m_targets.m_destX,m_targets.m_destY,m_targets.m_destZ);
	}

//	float angle_for_each_spawn = -float(M_PI) * 2 / damage;
	for( int i = 0; i < damage; i++ )
	{
//		float m_followAngle = angle_for_each_spawn * i;
		Unit *tu = u_caster->create_guardian(cr_entry,GetDuration(),level,obj,vec, GetProto() );
		//snake trap, try to aggro the unit that triggerred the trap and not hunter selection
		if( tu && unitTarget && GetProto()->NameHash == SPELL_HASH_SNAKE_TRAP && isAttackable( u_caster, unitTarget ) )
		{
			tu->GetAIInterface()->taunt( unitTarget, true );
		}
	}
	if (vec) 
	{
		delete vec;
		vec = NULL;
	}
}

void Spell::SpellEffectSkillStep(uint32 i) // Skill Step
{
	Player*target;
	if(m_caster->GetTypeId() != TYPEID_PLAYER)
	{
		// Check targets
		if( m_targets.m_unitTarget )
		{
			target = objmgr.GetPlayer((uint32)m_targets.m_unitTarget);
			if( target == NULL ) 
			{ 
				return;
			}
		}
		else
			return;
	}
	else
	{
		target = SafePlayerCast( m_caster );
	}
	
	uint32 skill = GetProto()->eff[i].EffectMiscValue;
	if( skill == 242 )
		skill = SKILL_LOCKPICKING; // somehow for lockpicking misc is different than the skill :s

	skilllineentry* sk = dbcSkillLine.LookupEntry( skill );

	if( sk == NULL )
	{ 
		return;
	}

	uint32 max = 1;
//	damage = MAX(damage-1,1);
	switch( sk->type )
	{
		case SKILL_TYPE_PROFESSION:
		case SKILL_TYPE_SECONDARY:
			max = damage * 75;
			break;
		case SKILL_TYPE_WEAPON:
			max = 5 * target->getLevel();
			break;
		case SKILL_TYPE_CLASS:
		case SKILL_TYPE_ARMOR:
			if( skill == SKILL_LOCKPICKING )
				max = damage * 75;
			else
				max = 1;
			break;
		default: //u cant learn other types in game
			return;
	};

	if( target->_HasSkillLine( skill ) )
	{
		target->_ModifySkillMaximum( skill, max );
	}		
	else
	{
		if( sk->type == SKILL_TYPE_PROFESSION )
			target->LearnProfession( skill );
	  
		target->_AddSkillLine( skill, 1, max );
	}

}

void Spell::SpellEffectSummonObject(uint32 i)
{
	if( !u_caster || u_caster->GetMapMgr() == NULL )
	{ 
		return;
	}

	uint32 entry;
	if( forced_miscvalues[i] != 0 )
		entry = forced_miscvalues[i];
	else
		entry = GetProto()->eff[i].EffectMiscValue;

	uint32 mapid = u_caster->GetMapId();
	float px = u_caster->GetPositionX();
	float py = u_caster->GetPositionY();
	float pz = u_caster->GetPositionZ();
	float orient = m_caster->GetOrientation();
	float posx = 0,posy = 0,posz = 0;
	
	if( entry == GO_FISHING_BOBBER && p_caster )
	{
		posx = m_targets.m_destX;
		posy = m_targets.m_destY;
		posz = m_targets.m_destZ;

		// Todo / Fix me: This should be loaded / cached
		uint32 zone = p_caster->GetAreaID();
		if( zone == 0 ) // If the player's area ID is 0, use the zone ID instead
			zone = p_caster->GetZoneId();

//		uint32 minskill;
		FishingZoneEntry *fishentry = FishingZoneStorage.LookupEntry( zone );
		if( !fishentry ) // Check database if there is fishing area / zone information, if not, return
		{ 
			p_caster->BroadcastMessage("This place is not marked for fishing. Report it to devs if it is a bug for area %d",zone);
			return;
		}
		
		// Todo / Fix me: Add fishskill to the calculations
//		minskill = fishentry->MinSkill;
#define MAX_FISHING_DURATION	17000 //needs to be larger then 4 seconds
		SendChannelStart( MAX_FISHING_DURATION ); // 30 seconds
		/*spell->SendSpellStart();
		spell->SendCastResult(SPELL_CANCAST_OK);
		spell->SendSpellGo ();*/

		GameObject *go = u_caster->GetMapMgr()->CreateGameObject(GO_FISHING_BOBBER);

		go->CreateFromProto( GO_FISHING_BOBBER, mapid, posx, posy, posz, orient );
		go->SetUInt32Value( GAMEOBJECT_FLAGS, 0 );
		go->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, 0 );
		go->SetUInt64Value( OBJECT_FIELD_CREATED_BY, m_caster->GetGUID() );
		u_caster->SetUInt64Value( UNIT_FIELD_CHANNEL_OBJECT, go->GetGUID() );
			 
		go->SetInstanceID( m_caster->GetInstanceID() );
		go->PushToWorld( m_caster->GetMapMgr() );

		uint64 CasterGuid = m_caster->GetGUID();
		if( lootmgr.IsFishable( zone ) ) // Only set a 'splash' if there is any loot in this area / zone
		{
			uint32 seconds = RandomUInt( MAX_FISHING_DURATION - 4000 ) + 3000;
			sEventMgr.AddEvent( go, &GameObject::FishHooked, CasterGuid , EVENT_GAMEOBJECT_FISH_HOOKED, seconds, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
		}
		sEventMgr.AddEvent( go, &GameObject::EndFishingEvent, CasterGuid, false, EVENT_GAMEOBJECT_END_FISHING, MAX_FISHING_DURATION, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
		p_caster->SetSummonedObject( go );
	}
	else
	{
		posx=px;
		posy=py;		
		GameObjectInfo * goI = GameObjectNameStorage.LookupEntry(entry);
		if(!goI)
		{
			if( p_caster != NULL )
			{
				sChatHandler.BlueSystemMessage(p_caster->GetSession(), "non-existant gameobject %u tried to be created by SpellEffectSummonObject. Report to devs!", entry);
			}
			return;
		}
		if( m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION && m_targets.m_destX && m_targets.m_destY && m_targets.m_destZ )
		{
			posx = m_targets.m_destX;
			posy = m_targets.m_destY;
			pz = m_targets.m_destZ;
		}
		GameObject *go=m_caster->GetMapMgr()->CreateGameObject(entry);
		
		go->SetInstanceID(m_caster->GetInstanceID());
		go->CreateFromProto(entry,mapid,posx,posy,pz,orient);
		go->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, 1);
		go->SetUInt64Value(OBJECT_FIELD_CREATED_BY,m_caster->GetGUID());
		go->PushToWorld(m_caster->GetMapMgr());

		//avoid infinit spawns ...could get to be an exploit
		uint32 duration = GetDuration();
		if( duration == (uint32)-1 || duration > 60*60*1000 )
			duration = 10*60*1000;

		sEventMgr.AddEvent(go, &GameObject::ExpireAndDelete, EVENT_GAMEOBJECT_EXPIRE, duration, 1,0);
		if ( entry == 17032 && p_caster ) // this is a portal
		{
			// enable it for party only
			go->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, 0 );
			//disable by default
			WorldPacket * pkt = go->BuildFieldUpdatePacket( GAMEOBJECT_BYTES_1, 1 );
			SubGroup * pGroup = p_caster->GetGroup() ? p_caster->GetGroup()->GetSubGroup(p_caster->GetSubGroup()) : NULL;

			if ( pGroup != NULL )
			{
				p_caster->GetGroup()->Lock();
				for ( GroupMembersSet::iterator itr = pGroup->GetGroupMembersBegin(); itr != pGroup->GetGroupMembersEnd(); ++itr )
				{
					if( (*itr)->m_loggedInPlayer && m_caster != (*itr)->m_loggedInPlayer )
						(*itr)->m_loggedInPlayer->GetSession()->SendPacket( pkt );
				}
				p_caster->GetGroup()->Unlock();
			}
			delete pkt;
			pkt = NULL;
		}
		else if (( 
				entry == 36727 || // Summoning Portal
				entry == 177193 || // Doom Portal
				entry == 194108 || // Ritual of Summoning
				entry == 181622 // Ritual of Souls
			) && p_caster )
		{
			//Player * pTarget = p_caster->GetMapMgr()->GetPlayer( p_caster->GetSelection() );
			Player * pTarget = objmgr.GetPlayer( (uint32)p_caster->GetSelection() );	//selection can be on other maps too
			if( pTarget == NULL || !pTarget->IsInWorld() || pTarget->GetGroup() != p_caster->GetGroup() || p_caster->GetGroup() == NULL )
			{
				p_caster->BroadcastMessage("You need to select a party or raid member first");
				SendCastResult( SPELL_FAILED_TARGET_NOT_IN_PARTY );
				if( p_caster )
					p_caster->ClearCooldownForSpell( GetProto()->Id );
				safe_cancel();
				return;
			}
			if( go->m_ritualmembers == NULL )
			{
				go->m_ritualmembers = new uint64[go->GetInfo()->SpellFocus];
				memset(go->m_ritualmembers,0,sizeof(uint64) * go->GetInfo()->SpellFocus );
			}
			go->m_ritualmembers[0] = p_caster->GetLowGUID();
			go->m_ritualcaster = p_caster->GetLowGUID();
			go->m_ritualtarget = pTarget->GetLowGUID();
			go->m_ritualspell = GetProto()->Id;
		}
		else if (( entry == 186811 || entry == 181622 
			 || entry == 193062	//Refreshment Portal
			 || GetProto()->ChannelInterruptFlags !=0 
			) && p_caster ) // ritual of refreshment, ritual of souls
		{
			if( go->m_ritualmembers == NULL )
			{
				go->m_ritualmembers = new uint64[go->GetInfo()->SpellFocus];
				memset(go->m_ritualmembers,0,sizeof(uint64) * go->GetInfo()->SpellFocus );
			}
			go->m_ritualmembers[0] = p_caster->GetLowGUID();
			go->m_ritualcaster = p_caster->GetLowGUID();
			go->m_ritualtarget = 0;
			go->m_ritualspell = GetProto()->Id;
		}
		else if ( entry == 186812 || entry == 181621 ) // Refreshment Table, Soulwell
		{
			go->charges = goI->sound1;
		}
		else//Lightwell,if there is some other type -- add it
		{
			go->charges = 5;//Max 5 charges
		}
		if( p_caster )
			p_caster->SetSummonedObject(go);
	}
}

void Spell::SpellEffectEnchantItem(uint32 i) // Enchant Item Permanent
{
	if(!itemTarget || !p_caster) 
	{ 
		return;
	}

	//special handling for velums
	if( itemTarget->GetProto()->Class == ITEM_CLASS_TRADEGOODS && itemTarget->GetProto()->SubClass == ITEM_SUBCLASS_VELUM )
	{
		//create the scroll
		uint32 scroll_entry = objmgr.VelumEnchantToScrollEntry[ GetProto()->Id ];
		if( scroll_entry <= 0 )
		{
			p_caster->BroadcastMessage( "Velum is missing transformation item. Report this to devs : Velum spell %d missing", GetProto()->Id );
			return;
		}
		//remove the velum first to give the space for the new item(if it helps at all )
		p_caster->GetItemInterface()->RemoveItemAmt_ProtectPointer( itemTarget->GetProto()->ItemId, 1, &itemTarget);
		ItemPrototype *m_itemProto;
		m_itemProto = ItemPrototypeStorage.LookupEntry( scroll_entry );
		if (!m_itemProto)
			return;
		if(p_caster->GetItemInterface()->CanReceiveItem(m_itemProto, 1))
		{
			SendCastResult(SPELL_FAILED_TOO_MANY_OF_ITEM);
			return;
		}
		Item *add = p_caster->GetItemInterface()->FindItemLessMax(scroll_entry, 1, false);
		if( add )  
		{
			add->SetCount(add->GetUInt32Value(ITEM_FIELD_STACK_COUNT) + 1);
			add->m_isDirty = true;
			p_caster->GetSession()->SendItemPushResult(add, true,false,true,false,p_caster->GetItemInterface()->GetBagSlotByGuid(add->GetGUID()),0xFFFFFFFF,1);
		}
		else
		{
			SlotResult slotresult = p_caster->GetItemInterface()->FindFreeInventorySlot(m_itemProto);
			if(!slotresult.Result)
			{
				  SendCastResult(SPELL_FAILED_TOO_MANY_OF_ITEM);
				  return;
			}
			
			Item *newItem =objmgr.CreateItem(scroll_entry,p_caster);
			newItem->SetUInt64Value(ITEM_FIELD_CREATOR,m_caster->GetGUID());
			newItem->SetUInt32Value(ITEM_FIELD_STACK_COUNT, 1);

			AddItemResult res = p_caster->GetItemInterface()->SafeAddItem(newItem,slotresult.ContainerSlot, slotresult.Slot);
			if( res == ADD_ITEM_RESULT_OK )
				p_caster->GetSession()->SendItemPushResult(newItem,true,false,true,true,slotresult.ContainerSlot,slotresult.Slot,1);
			else if( res == ADD_ITEM_RESULT_ERROR )
			{
				newItem->DeleteMe();
				newItem = NULL;
			}
		}
		return;
	}

	EnchantEntry * Enchantment = dbcEnchant.LookupEntry(GetProto()->eff[i].EffectMiscValue);
	if(!Enchantment) 
	{ 
		return;
	}

	if(p_caster->GetSession()->GetPermissionCount() > 0)
		sGMLog.writefromsession(p_caster->GetSession(), "enchanted item for %s", itemTarget->GetOwner()->GetName());

	//remove other perm enchantment that was enchanted by profession
	int32 Slot;
//	if( GetProto()->eff[i].Effect == SPELL_EFFECT_ADD_SOCKET )
	if( Enchantment->type[0] == SPELL_ENCHANT_TYPE_ADD_SOCKET || Enchantment->type[1] == SPELL_ENCHANT_TYPE_ADD_SOCKET || Enchantment->type[2] == SPELL_ENCHANT_TYPE_ADD_SOCKET )
	{
		//check if we already added a slot via enchantments
		if ( itemTarget->HasEnchantment( Enchantment->Id ) != -1 )
		{ 
			return;
		}
		itemTarget->AddEnchantment(Enchantment, 0, true, true, false, PRISMATIC_ENCHANTMENT_SLOT);
	}
	//some say tinkers stack with other enchants ? I bet there is a flag which goes in which slot
	else if( Enchantment->type[0] == SPELL_ENCHANT_TYPE_ADD_SPELL || Enchantment->type[1] == SPELL_ENCHANT_TYPE_ADD_SPELL || Enchantment->type[2] == SPELL_ENCHANT_TYPE_ADD_SPELL )
	{
		itemTarget->RemoveProfessionEnchant( ITEM_ENCHANT_SLOT_TEMPORARY2 );
		Slot = itemTarget->AddEnchantment(Enchantment, 0, true, true, false, ITEM_ENCHANT_SLOT_TEMPORARY2);
	}
	else
	{
		itemTarget->RemoveProfessionEnchant( ITEM_ENCHANT_SLOT_TEMPORARY1 );
		Slot = itemTarget->AddEnchantment(Enchantment, 0, true, true, false, ITEM_ENCHANT_SLOT_TEMPORARY1);
	}

	if( i_caster == NULL )
		for(uint32 i=0;i<GetProto()->spell_skilline_assoc_counter;i++)
		{
			if( GetProto()->spell_skill )
				DetermineSkillUp( GetProto()->spell_skilline[i], GetProto()->spell_skill->skill_point_advance );
			else
				DetermineSkillUp( GetProto()->spell_skilline[i], 1 );
		}
}

void Spell::SpellEffectEnchantItemTemporary(uint32 i)  // Enchant Item Temporary
{
	if( !itemTarget || !p_caster )
	{
		sLog.outDebug(" Cannot apply temporary enchant %u for spell %u without target or caster",GetProto()->Id,GetProto()->eff[i].EffectMiscValue);
		return;
	}
	uint32 Duration = damage > 1 ? damage : 1800;

	// dont allow temporary enchants unless we're the owner of the item
	if( itemTarget->GetOwner() != p_caster )
	{
		sLog.outDebug(" Cannot apply temporary enchant %u for spell %u on not owned item",GetProto()->Id,GetProto()->eff[i].EffectMiscValue);
		return;
	}

	EnchantEntry * Enchantment = dbcEnchant.LookupEntry(GetProto()->eff[i].EffectMiscValue);
	if(!Enchantment)
	{
		sLog.outDebug(" Enchantment %u for spell %u does not exist",GetProto()->Id,GetProto()->eff[i].EffectMiscValue);
		return;
	}

	itemTarget->RemoveEnchantment(ITEM_ENCHANT_SLOT_TEMPORARY2);
	int32 Slot = itemTarget->AddEnchantment(Enchantment, Duration, false, true, false, ITEM_ENCHANT_SLOT_TEMPORARY2);
	if(Slot < 0)
		return; // Apply failed

//	skilllinespell* skill = objmgr.GetSpellSkill(GetProto()->Id);
	if( GetProto()->spell_skilline[0] )
	{
		if( GetProto()->spell_skill->skill_point_advance )
			DetermineSkillUp( GetProto()->spell_skilline[0], GetProto()->spell_skill->skill_point_advance, itemTarget->GetProto()->ItemLevel);
		else
			DetermineSkillUp( GetProto()->spell_skilline[0], 1, itemTarget->GetProto()->ItemLevel);
	}
}

void Spell::SpellEffectTameCreature(uint32 i)
{
	if( !p_caster )
	{ 
		return;
	}

    Creature* tame = ( ( unitTarget->GetTypeId() == TYPEID_UNIT ) ? SafeCreatureCast( unitTarget ) : NULL );

	//the target might get removed from world before we manage to tame it
    if( tame == NULL || !tame->IsInWorld() )
    { 
		return;
    }

	// Remove target
	tame->GetAIInterface()->HandleEvent(EVENT_LEAVECOMBAT, p_caster, 0);
	Pet *pPet = objmgr.CreatePet( tame->GetEntry(), p_caster->GeneratePetNumber() );
	pPet->SetInstanceID(p_caster->GetInstanceID());
	//302 - tamed pets are close to tamer level
//	if( tame->getLevel() + 5 < p_caster->getLevel() )
//		tame->SetUInt32Value( UNIT_FIELD_LEVEL, p_caster->getLevel() ); //this will never loop into negative ;)
	pPet->CreateAsSummon(tame->GetEntry(), tame->GetCreatureInfo(), tame, p_caster, NULL, 0x10 | 2, 0);
//	p_caster->SetFlag(PLAYER_FIELD_BYTES,PLAYER_BYTES_FLAG_HAS_PET);
	//tame->SafeDelete();
	//delete tame;
	tame->Despawn(0,tame->GetProto()? tame->GetProto()->RespawnTime:0);
}

void Spell::SpellEffectSummonPet(uint32 i) //summon - pet
{
	if( GetProto()->Id == 883 
		|| GetProto()->Id == 83242
		|| GetProto()->Id == 83243
		|| GetProto()->Id == 83244
		|| GetProto()->Id == 83245
		)
	{
		// "Call Pet" spell
		if(!p_caster)
		{ 
			return;
		}

		if(p_caster->GetSummon() != 0)
		{
			p_caster->GetSession()->SendNotification("You already have a pet summoned.");
			return;
		}

//		uint32 petno = p_caster->GetUnstabledPetNumber();
		uint32 SlotNo = damage;

		if( damage >= 0 )
		{
			p_caster->SpawnPet( -1, SlotNo );
		}
		else
		{
			WorldPacket data(SMSG_AREA_TRIGGER_MESSAGE, 50);
			data << uint32(0) << "You do not have any pets to call." << uint8(0);
			p_caster->GetSession()->SendPacket(&data);
		}
		return;
	}
	
	//uint32 entryId = GetProto()->eff[i].EffectMiscValue;

	//VoidWalker:torment, sacrifice, suffering, consume shadows
	//Succubus:lash of pain, soothing kiss, seduce , lesser invisibility
	//felhunter:	 Devour Magic,Paranoia,Spell Lock,	Tainted Blood
 
	if(!p_caster)
	{ 
		return;
	}
	
	// remove old pet
	Pet *old = SafePlayerCast(m_caster)->GetSummon();
	if(old)
		old->Dismiss();		

	uint32 CreatureEntry;
	if( forced_miscvalues[i] != 0 )
		CreatureEntry = forced_miscvalues[i];
	else
		CreatureEntry = GetProto()->eff[i].EffectMiscValue;

	if(p_caster->getClass() == WARLOCK)
	{
		//if demonic sacrifice auras are still active, remove them
		//uint32 spids[] = { 18789, 18790, 18791, 18792, 35701, 0 };
		//p_caster->RemoveAuras(spids);
		p_caster->RemoveAura(18789);
		p_caster->RemoveAura(18790);
		p_caster->RemoveAura(18791);
		p_caster->RemoveAura(18792);
		p_caster->RemoveAura(35701);
	}

	PlayerPet *pet = p_caster->GetPlayerPet( -1, CreatureEntry );
	uint32 pet_no;
	if( pet )
	{
		p_caster->SpawnPet( pet->number, pet->StableSlot );
	}
	else
	{
		pet_no = p_caster->GeneratePetNumber( CreatureEntry );

		CreatureInfo *ci = CreatureNameStorage.LookupEntry( CreatureEntry );
		if(ci)
		{
			Pet *summon = objmgr.CreatePet( CreatureEntry, pet_no );
			summon->SetInstanceID(m_caster->GetInstanceID());
			summon->CreateAsSummon( CreatureEntry, ci, NULL, p_caster, GetProto(), 0x10 | 1, 0);
		}
	}
}

void Spell::SpellEffectWeapondamage( uint32 i ) // Weapon damage +
{
	if( unitTarget == NULL || u_caster == NULL )
	{ 
		return;
	}

	// Hacky fix for druid spells where it would "double attack".
	if( GetProto()->eff[2].Effect == SPELL_EFFECT_WEAPON_PERCENT_DAMAGE || GetProto()->eff[1].Effect == SPELL_EFFECT_WEAPON_PERCENT_DAMAGE )
	{
		add_damage += damage;
		return;
	}

	uint32 _type;
	if( GetType() == SPELL_DMG_TYPE_RANGED )
		_type = RANGED;
	else
	{
		if (GetProto()->AttributesExC & 0x1000000)
			_type =  OFFHAND;
		else
			_type = MELEE;
	}

	//avoid dmg overflows
	if( damage < 0 || damage > 100000 )
	{ 
		return;
	}

#ifdef USE_SPELL_MISSILE_DODGE
	if(GetProto()->speed > 0 && _type == RANGED )
	{
		float dist = m_caster->CalcDistance( unitTarget );
		float time = ((dist*1000.0f)/GetProto()->speed);
		LocationVector src,dst;
		src.x = m_caster->GetPositionX();
		src.y = m_caster->GetPositionY();
		src.z = m_caster->GetPositionZ();
		dst.x = unitTarget->GetPositionX();
		dst.y = unitTarget->GetPositionY();
		dst.z = unitTarget->GetPositionZ();
		sEventMgr.AddEvent(m_caster, &Object::EventWeaponDamageToLocation, unitTarget->GetGUID(), GetProto(), (uint32)damage, (int32)forced_pct_mod_cur-100, (int32)i, src, dst, EVENT_SPELL_DAMAGE_HIT, uint32(time), 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
	}
	else
		u_caster->Strike( unitTarget, _type, GetProto(), damage, forced_pct_mod_cur-100, 0, false, false );
#else
		u_caster->Strike( unitTarget, _type, GetProto(), damage, forced_pct_mod_cur-100, 0, false, false );
#endif
}

void Spell::SpellEffectPowerBurn(uint32 i) // power burn
{
	if(!unitTarget)
	{ 
		return;
	}
	if(!unitTarget->isAlive())
	{ 
		return;
	}
	if (unitTarget->GetPowerType() != POWER_TYPE_MANA )
	{ 
		return;
	}

	//there must be a general flag that sets if value is PCT or direct. This one is pct
	if( GetProto()->NameHash == SPELL_HASH_MANA_BURN )
	{
		uint32 new_dmg = unitTarget->GetMaxPower( POWER_TYPE_MANA ) * damage / 100;
		uint32 dmg_limit;
		if( u_caster )
			dmg_limit = u_caster->GetMaxPower( POWER_TYPE_MANA ) * GetProto()->eff[i].EffectBasePoints * 2 / 100;
		else
			dmg_limit = 0x00FFFFFF;
		if( new_dmg >= dmg_limit )
			new_dmg = dmg_limit;
/*		else // dmg_limit > new_dmg
		{
			uint32 extra_dmg = dmg_limit - new_dmg ;
			new_dmg = new_dmg + RandomUInt() % extra_dmg;
		}*/
		damage = new_dmg;
		//cannot cast more then twice on same target
/*		if( p_caster )
		{
			if( p_caster->mana_burn_target == unitTarget->GetLowGUID() )
				p_caster->mana_burn_target_cast++;
			else
			{
				p_caster->mana_burn_target_cast = 0;
				p_caster->mana_burn_target = unitTarget->GetLowGUID();
			}
			if( p_caster->mana_burn_target_cast >= 2 )
			{
				SendCastResult( SPELL_FAILED_BAD_TARGETS );
				return;
			}
		}*/
	}

	if( unitTarget->IsPlayer() )
	{
		Player* mPlayer = SafePlayerCast( unitTarget );
		if( mPlayer->IsInFeralForm() )
		{ 
			return;
		}

		// Resilience - reduces the effect of mana drains by (CalcRating*2)%.
//		damage *= float2int32( 1 - ( ( SafePlayerCast(unitTarget)->CalcRating( PLAYER_RATING_MODIFIER_SPELL_CRIT_RESILIENCE ) * 2 ) / 100.0f ) );
	}

	int32 mana = (int32)min( (int32)unitTarget->GetPower( POWER_TYPE_MANA ), damage );
//	unitTarget->ModPower( POWER_TYPE_MANA, -mana );
	u_caster->Energize( unitTarget, GetProto()->Id, -mana, POWER_TYPE_MANA, 0 );
	
	dealdamage tdmg;
	tdmg.base_dmg = (uint32)(mana * GetProto()->eff[i].EffectConversionCoef);
	tdmg.pct_mod_final_dmg = forced_pct_mod_cur;
	tdmg.StaticDamage = true;
	tdmg.DisableProc = (pSpellId!=0);
	m_caster->SpellNonMeleeDamageLog( unitTarget, GetProto(), &tdmg, 1, i);   
}

void Spell::SpellEffectThreat(uint32 i) // Threat
{
	if(!unitTarget || !unitTarget->isAlive() )
	{ 
		return;
	}
	//some say taunt should not be working in PVP ?
	if( unitTarget->IsPlayer() == true )
	{
		return;
	}

	bool chck = unitTarget->GetAIInterface()->modThreatByPtr(u_caster,damage);
	if( chck == false )
		unitTarget->GetAIInterface()->AttackReaction(u_caster,damage,0);	
}

void Spell::SpellEffectTriggerSpell(uint32 i) // Trigger Spell
{
	SpellEntry *entry = dbcSpell.LookupEntry(GetProto()->eff[i].EffectTriggerSpell);
	if (entry == NULL || entry->Id == GetProto()->Id ) 
	{ 
		return;
	}

	//fucking hackfixes to target selected target in case of multiple targets
	//isn't this supposed to be default behavior ? (Well no, there are some spells that might affect only friends ..)
	if( ( GetProto()->c_is_flags2 & SPELL_FLAG2_IS_TRIGGERED_ON_TARGET ) )
	{
		SpellCastTargets targets( m_targets );
		if( unitTarget )
			targets.m_unitTarget = unitTarget->GetGUID();
		Spell *sp = SpellPool.PooledNew( __FILE__, __LINE__ );
		if( unitTarget && (( GetProto()->c_is_flags & SPELL_FLAG_IS_TRIGGERED_BY_TARGET ) || GetProto()->eff[i].Effect == SPELL_EFFECT_FORCE_CAST ) )
			sp->Init(unitTarget,entry,true,NULL);
		else
			sp->Init(m_caster,entry,true,NULL);
		sp->prepare(&targets);
		return;
	}
	bool DelayedCast = false;
	float Speed = MAX( entry->speed, GetProto()->speed );
	if( Speed > 0 && u_caster )
	{
		float dist;
		if (m_targets.m_targetMask & TARGET_FLAG_SELF )
			dist = 0.0f; 
		else if (( m_targets.m_targetMask & TARGET_FLAG_UNIT ) && unitTarget != NULL && unitTarget->isAlive() )
			dist = Distance2DSq( m_caster, unitTarget );
		else if (( m_targets.m_targetMask & TARGET_FLAG_OBJECT ) && unitTarget != NULL && unitTarget->isAlive() )
			dist = Distance2DSq( m_caster, unitTarget );
		else if (( m_targets.m_targetMask & TARGET_FLAG_SOURCE_LOCATION ) && m_targets.m_srcX != 0.0f )
			dist = Distance2DSq( m_caster->GetPositionX(), m_caster->GetPositionY(), m_targets.m_srcX, m_targets.m_srcY );
		else if (( m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION ) && m_targets.m_destX != 0.0f )
			dist = Distance2DSq( m_caster->GetPositionX(), m_caster->GetPositionY(), m_targets.m_destX, m_targets.m_destY );
		dist = sqrt( dist );
		float time = ((dist*1000.0f)/Speed);
		if( time > 500 )
		{
			DelayedCast = true;
			sEventMgr.AddEvent( u_caster, &Unit::EventCastSpell3, m_targets, entry, EVENT_SPELL_DAMAGE_HIT, uint32(time), 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
			return;
		}
	}
	if( DelayedCast == false )
	{
		SpellCastTargets targets = m_targets;
		Spell *sp = SpellPool.PooledNew( __FILE__, __LINE__ );
		if( unitTarget && (( GetProto()->c_is_flags & SPELL_FLAG_IS_TRIGGERED_BY_TARGET ) || GetProto()->eff[i].Effect == SPELL_EFFECT_FORCE_CAST ) )
			sp->Init(unitTarget,entry,true,NULL);
		else
			sp->Init(m_caster,entry,true,NULL);
		sp->prepare(&targets);
	}
}

void Spell::SpellEffectHealthFunnel(uint32 i) // Health Funnel
{
	if(!unitTarget)
	{ 
		return;		
	}
	if(!unitTarget->isAlive() || !unitTarget->IsPet())
	{ 
		return;
	}

	//does not exist
}

void Spell::SpellEffectPowerFunnel(uint32 i) // Power Funnel
{
	if(!unitTarget)
	{ 
		return;		
	}
	if(!unitTarget->isAlive() || !unitTarget->IsPet())
	{ 
		return;
	}

	//does not exist
}

void Spell::SpellEffectKillCredit( uint32 i )
{
	uint32 CreatureEntry;
	if( forced_miscvalues[i] != 0 )
		CreatureEntry = forced_miscvalues[i];
	else
		CreatureEntry = GetProto()->eff[i].EffectMiscValue;
	if ( playerTarget != NULL )
	{
		CreatureProto * cp = CreatureProtoStorage.LookupEntry( CreatureEntry );
		if ( cp != NULL )
			sQuestMgr.OnPlayerKill( playerTarget, CreatureEntry, false );
	}
}

void Spell::SpellEffectHealMaxHealthPCT(uint32 i)   // Heal Max Health
{
	if( unitTarget == NULL || !unitTarget->isAlive() )
	{ 
		return;
	}

	int32 heal = (int32)unitTarget->GetUInt32Value( UNIT_FIELD_MAXHEALTH ) * damage / 100;
//	Heal( heal );
	u_caster->SpellHeal( unitTarget, GetProto(), heal, pSpellId==0, static_dmg[i] != 0, false, 1, forced_pct_mod_cur, i, 0 );
/*	uint32 combat_log_heal = heal;

	int32 absorb = 0;
	if( unitTarget->HealAbsorb <= heal )
	{
		absorb = unitTarget->HealAbsorb;
		heal -= absorb;
		unitTarget->HealAbsorb = 0;
	}
	else // if( unitTarget->HealAbsorb > amount )
	{
		absorb = heal;
		unitTarget->HealAbsorb -= heal;
		heal = 0;
	}

	int32 dif = unitTarget->GetUInt32Value( UNIT_FIELD_MAXHEALTH ) - unitTarget->GetUInt32Value( UNIT_FIELD_HEALTH );
	if( heal > dif )
		heal = dif;

//	if( unitTarget->GetTypeId() == TYPEID_PLAYER)
//		 SendHealSpellOnPlayer( SafePlayerCast( m_caster ), SafePlayerCast( unitTarget ), heal, false );
	m_caster->SendCombatLogMultiple( unitTarget, combat_log_heal, 0, absorb, 0, GetProto()->spell_id_client, GetProto()->SchoolMask, COMBAT_LOG_HEAL, 0 );
	unitTarget->ModUnsigned32Value( UNIT_FIELD_HEALTH, heal );
	*/
}

void Spell::SpellEffectHealMaxHealth(uint32 i)   // Heal Max Health
{
	if( unitTarget == NULL || !unitTarget->isAlive() )
	{ 
		return;
	}

	uint32 dif = unitTarget->GetUInt32Value( UNIT_FIELD_MAXHEALTH ) - unitTarget->GetUInt32Value( UNIT_FIELD_HEALTH );
	if( !dif )
	{
		SendCastResult( SPELL_FAILED_ALREADY_AT_FULL_HEALTH );
		return;
	}

//	if( unitTarget->GetTypeId() == TYPEID_PLAYER)
//		 SendHealSpellOnPlayer( SafePlayerCast( m_caster ), SafePlayerCast( unitTarget ), dif, false );
	m_caster->SendCombatLogMultiple( unitTarget, dif, 0, 0, 0, GetProto()->spell_id_client, GetProto()->SchoolMask, COMBAT_LOG_HEAL, 0 );

	unitTarget->ModUnsigned32Value( UNIT_FIELD_HEALTH, dif );
}

void Spell::SpellEffectInterruptCast(uint32 i) // Interrupt Cast
{
	if(!unitTarget || !unitTarget->isAlive())
	{ 
		return;
	}

	if(!playerTarget)
	{
		if (u_caster && (u_caster != unitTarget))
		{
			unitTarget->GetAIInterface()->AttackReaction(u_caster, 1, m_spellInfo->Id);
			Creature *c = SafeCreatureCast( unitTarget );
			if (c && c->GetProto() && c->GetProto()->modImmunities)
			{
				if (c->GetProto()->modImmunities & 32768)
				{ 
					return;
				}
			}
		}
	}
	//immune to dispel mechanic ?
	if( unitTarget->MechanicsDispels[ MECHANIC_INTERRUPTED ] != 0 || ( GetProto()->eff[i].EffectMechanic < MECHANIC_TOTAL && unitTarget->MechanicsDispels[ GetProto()->eff[i].EffectMechanic ] ) )
	{
		return;
	}

	// FIXME:This thing prevent target from spell casting too but cant find.
	uint32 SchoolMask = 0;
	int32 spell_id = 0;
	if( unitTarget->GetCurrentSpell() )
	{
		SpellEntry *sp = unitTarget->GetCurrentSpell()->GetProto();
		SchoolMask = sp->SchoolMask;
		//do not interrupt physical spells ( abilities )
		if( ( SchoolMask & ~SCHOOL_MASK_NORMAL ) == 0 )
			return;
		spell_id = sp->Id;
	}

	//Barkskin. Not dead sure this comes here. Remove flags we are immune to
	if( p_caster )
	{
		for(uint32 i=0;i<SCHOOL_COUNT;i++)
			if( p_caster->SpellDelayResist[ i ] > 0 && ( SchoolMask & ( 1<<i ) ) && RandChance( p_caster->SpellDelayResist[ i ] ) )
				SchoolMask &= ~(1<<i);
	}

	if( SchoolMask == 0 )
		return;

	if( spell_id )
	{
		//right now i'm out of flags :(
		if( u_caster )
		{
			if( u_caster != unitTarget )
			{
				unitTarget->HandleProc( PROC_ON_INTERRUPT_EVENT | PROC_ON_VICTIM, u_caster, GetProto(), &spell_id );
				u_caster->HandleProc( PROC_ON_INTERRUPT_EVENT | PROC_ON_NOT_VICTIM, unitTarget, GetProto(), &spell_id );
			}
			else
				u_caster->HandleProc( PROC_ON_INTERRUPT_EVENT | PROC_ON_VICTIM | PROC_ON_NOT_VICTIM, unitTarget, GetProto(), &spell_id );
		}
//		SendInterruptLog( p_caster, GetProto()->Id, unitTarget, spell_id );
	}
	//interrupt it after the prochandler so we may catch the casted spell
	unitTarget->InterruptSpell();

	if(SchoolMask)//prevent from casts in this school
	{
		int32 duration = GetDuration();
		// Check for interruption reducing talents
		int32 DurationModifier = unitTarget->MechanicDurationPctMod[MECHANIC_INTERRUPTED];
		if( DurationModifier >= - 100 )
			duration = ( duration*(100+DurationModifier))/100;
		if( duration > 0 )
		{
	//		duration = MAX( 5000, duration );
			for(uint32 i=0;i<SCHOOL_COUNT;i++)
				if( SchoolMask & ( 1<<i) )
					unitTarget->SchoolCastPrevent[i]=duration+getMSTime();

			//iterate through all player spells and put them on cooldown
			if( unitTarget->IsPlayer() )
			{
				Player *p = SafePlayerCast( unitTarget );
				int32 NeedToSend = 0;
				WorldPacket data(SMSG_SPELL_COOLDOWN, 8+1+p->mSpells.size()*8);
				data << uint64( p->GetGUID() );
				data << uint8(0x0);                                     // flags (0x1, 0x2)
				SpellSet::iterator spellItr = p->mSpells.begin();
				for(; spellItr != p->mSpells.end(); ++spellItr)
				{
					SpellEntry *sp = dbcSpell.LookupEntryForced( *spellItr );
					if( sp == NULL || sp->PreventionType != SPELL_PREVENTION_TYPE_SILENCE)
						continue;
					if ( sp->Attributes & ATTRIBUTES_TRIGGER_COOLDOWN )
						continue;
					if( sp->c_is_flags2 & ( SPELL_FLAG2_IS_CASTABLE_SCHOOL_SILANCE | SPELL_FLAG2_IS_CASTABLE_SILANCED | SPELL_FLAG2_IS_CASTABLE_PACIFIED ) )
						continue;
					if( ( sp->SchoolMask & SchoolMask ) != 0 
							&& ( sp->SchoolMask & ~SchoolMask ) == 0	//silences ALL schools of this spell and not just 1
							&& ( sp->Attributes & ATTRIBUTES_PASSIVE ) == 0 )
					{
						int32 RemainingCooldown = p->Cooldown_Getremaining( sp );
						//1500 because some spells will use global cooldown as generic cooldown. Ex interrupt "heal" and it will put a cooldown on "mind blast"
	//					if( RemainingCooldown > 0 && RemainingCooldown < duration )
	//						p->ModCooldown( *spellItr, duration - RemainingCooldown, false );
	//					else
						if( duration > RemainingCooldown )
						{
							p->_Cooldown_Add( COOLDOWN_TYPE_SPELL, sp->Id, getMSTime() + duration, sp->Id, 0 );
							data << sp->Id;
							data << duration;
							NeedToSend++;
						}
					}
				}
				if( NeedToSend && p->GetSession() )
					p->GetSession()->SendPacket( &data );
			}
		}
	}
}

void Spell::SpellEffectDistract(uint32 i) // Distract
{
	//spellId 1725 Distract:Throws a distraction attracting the all monsters for ten sec's
	if(!unitTarget)
	{ 
		return;
	}
	if(!unitTarget->isAlive())
	{ 
		return;
	}

	if(m_targets.m_destX != 0.0f || m_targets.m_destY != 0.0f || m_targets.m_destZ != 0.0f)
	{
//		unitTarget->GetAIInterface()->MoveTo(m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ, 0);
		uint32 Stare_duration=GetDuration();
		if(Stare_duration>30*60*1000)
			Stare_duration=10000;//if we try to stare for more then a half an hour then better not stare at all :P (bug)
		float newo=unitTarget->calcRadAngle(unitTarget->GetPositionX(),unitTarget->GetPositionY(),m_targets.m_destX,m_targets.m_destY);
		unitTarget->GetAIInterface()->StopMovement(Stare_duration);
		unitTarget->SetFacing(newo);
	}

	//Smoke Emitter 164870
	//Smoke Emitter Big 179066
	//Unit Field Target of 
}

void Spell::SpellEffectPickpocket(uint32 i) // pickpocket
{
	//Show random loot based on roll,	
	if(!unitTarget)
		return; // impossible..
	if(!p_caster)
	{ 
		return;
	}

	if(unitTarget->GetTypeId() != TYPEID_UNIT)
	{ 
		return;
	}

	Creature *target = SafeCreatureCast( unitTarget );
	if(target->IsPickPocketed() || target->GetCreatureInfo() == NULL || 
		( target->GetCreatureInfo()->Type != HUMANOID && target->GetCreatureInfo()->Type != UNDEAD )
		)
	{
		SendCastResult(SPELL_FAILED_TARGET_NO_POCKETS);
		return;
	}
			
 // lootmgr.FillPickpocketingLoot(&(SafeCreatureCast(unitTarget))->loot,unitTarget->GetEntry());
	if( target->DificultyProto && target->DificultyProto->loot_pickpocket )
	{
		lootmgr.FillObjectLoot(unitTarget,SafeCreatureCast(unitTarget)->DificultyProto->loot_pickpocket);
		p_caster->Event_AchiementCriteria_Received( ACHIEVEMENT_CRITERIA_TYPE_LOOT_TYPE,2,ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
	}

	uint32 _rank = target->GetCreatureInfo()->Rank;
	unitTarget->loot.gold = float2int32((_rank+1) * unitTarget->getLevel() * (RandomUInt(5) + 1) * sWorld.getRate(RATE_MONEY));
#ifdef BATTLEGRUND_REALM_BUILD
	unitTarget->loot.gold = 1;
#endif 
	p_caster->SendLoot(unitTarget->GetGUID(), LOOT_PICKPOCKETING );
	target->SetPickPocketed(true);
}

void Spell::SpellEffectAddFarsight(uint32 i) // Add Farsight
{
	if(!p_caster)
	{ 
		return;
	}

	float x = m_targets.m_destX;
	float y = m_targets.m_destY;
	float z = m_targets.m_destZ;
	if(x == 0)
		x = m_targets.m_srcX;
	if(y == 0)
		y = m_targets.m_srcY;
	if(z == 0)
		z = m_targets.m_srcZ;

	DynamicObject * dynObj = p_caster->GetMapMgr()->CreateDynamicObject();
	dynObj->Create(u_caster, this, x, y, z, GetDuration(), GetRadius(i), damage, GetProto()->eff[0].EffectAmplitude );

	ASSERT(dynObj);
/*
	if( dynObj == NULL ) //i <3 burlex :P
	{
		delete dynObj;
		return;
	}
	*/
    dynObj->SetUInt32Value(OBJECT_FIELD_TYPE, 65);
    dynObj->SetUInt32Value(DYNAMICOBJECT_BYTES, 0x80000002);
	dynObj->SetInstanceID(p_caster->GetInstanceID());	
	p_caster->SetUInt64Value(PLAYER_FARSIGHT, dynObj->GetGUID());

	p_caster->GetMapMgr()->ChangeFarsightLocation(p_caster, dynObj);
}

void Spell::SpellEffectSummonPossessed(uint32 i) // eye of kilrog
{
	/*
	m_target->DisableAI();
	pCaster->SetUInt64Value(UNIT_FIELD_SUMMON, 0);
	m_target->SetUInt64Value(UNIT_FIELD_SUMMONEDBY, 0);
	pCaster->SetUInt64Value(UNIT_FIELD_CHARM, m_target->GetGUID());
	m_target->SetUInt64Value(UNIT_FIELD_CHARMEDBY, pCaster->GetGUID());
	pCaster->SetUInt64Value(PLAYER_FARSIGHT, m_target->GetGUID());
	pCaster->m_CurrentCharm = ((Creature*)m_target);
	m_target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED_CREATURE);
	pCaster->m_noInterrupt = 1;
	pCaster->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_LOCK_PLAYER);

	WorldPacket data(SMSG_DEATH_NOTIFY_OBSOLETE);
	data << m_target->GetNewGUID() << uint8(1);
	pCaster->GetSession()->SendPacket(&data);
	*/

	uint32 CreatureEntry;
	if( forced_miscvalues[i] != 0 )
		CreatureEntry = forced_miscvalues[i];
	else
		CreatureEntry = GetProto()->eff[i].EffectMiscValue;
	CreatureInfo *ci = CreatureNameStorage.LookupEntry(CreatureEntry);
	if( ci && p_caster )
	{
		Creature* NewSummon = m_caster->GetMapMgr()->CreateCreature(ci->Id);
		// Create
		NewSummon->SetInstanceID(m_caster->GetInstanceID());
		NewSummon->Create( ci->Name, m_caster->GetMapId(), 
			m_caster->GetPositionX()+(3*(cos((float(M_PI)/2)+m_caster->GetOrientation()))), m_caster->GetPositionY()+(3*(cos((float(M_PI)/2)+m_caster->GetOrientation()))), m_caster->GetPositionZ() + 2, m_caster->GetOrientation());

		// Fields
		NewSummon->SetUInt32Value(UNIT_FIELD_LEVEL,m_caster->GetUInt32Value(UNIT_FIELD_LEVEL));
		NewSummon->SetUInt32Value(UNIT_FIELD_DISPLAYID,  ci->Male_DisplayID);
		NewSummon->SetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID, ci->Male_DisplayID);
		NewSummon->SetUInt64Value(UNIT_FIELD_SUMMONEDBY, m_caster->GetGUID());
		NewSummon->SetUInt64Value(UNIT_FIELD_CREATEDBY, m_caster->GetGUID());
		NewSummon->SetUInt32Value(UNIT_FIELD_HEALTH , 100);
		NewSummon->SetUInt32Value(UNIT_FIELD_MAXHEALTH , 100);
		NewSummon->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, p_caster->GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE));
		NewSummon->SetFloatValue(OBJECT_FIELD_SCALE_X,1.0f);
//		NewSummon->SetUInt32Value(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_9 | UNIT_FLAG_PLAYER_CONTROLLED_CREATURE);

		if(p_caster->IsPvPFlagged())
			NewSummon->SetPvPFlag();
		if(p_caster->IsFFAPvPFlagged())
			NewSummon->SetFFAPvPFlag();

		//Setting faction
		NewSummon->_setFaction();
		NewSummon->m_temp_summon=true;

		// Add To World
		NewSummon->PushToWorld(m_caster->GetMapMgr());
		
		// Force an update on the player to create this guid.
		p_caster->ProcessPendingUpdates();

		//p_caster->SetUInt64Value(UNIT_FIELD_SUMMON, NewSummon->GetGUID());
		p_caster->SetUInt64Value(PLAYER_FARSIGHT, NewSummon->GetGUID());	//not always ? Posssesd should handle this ?
		//p_caster->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_LOCK_PLAYER);
		p_caster->Possess(NewSummon);
	}/**/
}

//void Spell::SpellEffectCreateSummonTotem(uint32 i)
void Spell::SpellEffectEquipGlyph(uint32 i)
{
	if( !p_caster )
	{ 
		return;
	}

	//check if slot is valid
	if( GLYPHS_COUNT <= GlyphSlot )
	{ 
		return;
	}

	//check if we can even equip this
	uint32 newGlyph = GetProto()->eff[i].EffectMiscValue;

	//this is clear glyph spell 
	if( newGlyph == 0 )
	{
		//remove old glyp if there is one equipped
		uint32 old_glyph = p_caster->GetUInt32Value( PLAYER_FIELD_GLYPHS_1 + GlyphSlot );
		GlyphPropertiesEntry *old_glyphprops = dbcGlyphPropertiesStore.LookupEntry( old_glyph );
		if( old_glyphprops )
			p_caster->RemoveAura( old_glyphprops->SpellId );

		//the new glyph. 
		p_caster->SetUInt32Value( PLAYER_FIELD_GLYPHS_1 + GlyphSlot, newGlyph );
		p_caster->m_specs[ p_caster->m_talentActiveSpec ].glyphs[ GlyphSlot ] = newGlyph;
		p_caster->smsg_TalentsInfo();
		return;	//no other action is needed
	}

	//check if we already have this glyph equipped
	for(int i=0;i<GLYPHS_COUNT;i++)
		if( p_caster->GetUInt32Value( PLAYER_FIELD_GLYPHS_1 + i ) == newGlyph )
		{
			SendCastResult(SPELL_FAILED_UNIQUE_GLYPH);
			return;
		}

	GlyphPropertiesEntry *glyphprops = dbcGlyphPropertiesStore.LookupEntry( newGlyph );
	GlyphSlotEntry	*glypslot = dbcGlyphSlotStore.LookupEntry( p_caster->GetUInt32Value( PLAYER_FIELD_GLYPH_SLOTS_1 + GlyphSlot ) );
	//should not happen
	if( !glyphprops || !glypslot || glyphprops->TypeFlags != glypslot->TypeFlags )
	{
		sLog.outDebug("We are trying to insert a glyph into slot %u where we have slottype %u glyph is %u we have props %u and slot info %u\n",	GlyphSlot,p_caster->GetUInt32Value( PLAYER_FIELD_GLYPH_SLOTS_1 + GlyphSlot ),newGlyph,glyphprops!=NULL,glypslot!=NULL);
		if( glyphprops && glypslot )
			sLog.outDebug(" Glyph property flags = %u while slot property flags = %u\n",glyphprops->TypeFlags,glypslot->TypeFlags);
		SendCastResult(SPELL_FAILED_INVALID_GLYPH);
		return;
	}

	//remove old glyp if there is one equipped
	uint32 old_glyph = p_caster->GetUInt32Value( PLAYER_FIELD_GLYPHS_1 + GlyphSlot );
	GlyphPropertiesEntry *old_glyphprops = dbcGlyphPropertiesStore.LookupEntry( old_glyph );
	if( old_glyphprops )
		p_caster->RemoveAura( old_glyphprops->SpellId );

	//the new glyph. 
	p_caster->SetUInt32Value( PLAYER_FIELD_GLYPHS_1 + GlyphSlot, newGlyph );
	p_caster->m_specs[ p_caster->m_talentActiveSpec ].glyphs[ GlyphSlot ] = newGlyph;

	//blizz uses this. Not sure we need it or not
//	sStackWorldPacket( learnspell, SMSG_LEARNED_SPELL, 8);
//	learnspell << spell_id;
//	learnspell << uint16( 0 );	//from 3.2
//	p_caster->m_session->SendPacket( &learnspell );
	p_caster->smsg_TalentsInfo();

	//the effect of the glyph
	SpellEntry *spellInfo = dbcSpell.LookupEntry( glyphprops->SpellId ); //we already modified this spell on server loading so it must exist
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init( p_caster, spellInfo ,true, NULL );
	SpellCastTargets targets( p_caster->GetGUID() );
	spell->prepare(&targets);

	//isn't this auto removed ?
    if( i_caster && p_caster->GetItemInterface() )
		p_caster->GetItemInterface()->SafeFullRemoveItemByGuid( i_caster->GetGUID() );
}

void Spell::SpellEffectHealMechanical(uint32 i)
{
	if(!unitTarget)
	{ 
		return;
	}
	if(unitTarget->GetTypeId() != TYPEID_UNIT)
	{ 
		return;
	}
	if(SafeCreatureCast(unitTarget)->GetCreatureInfo()->Type != MECHANICAL)
	{ 
		return;
	}

//	Heal((int32)damage,false, i);
	u_caster->SpellHeal( unitTarget, GetProto(), damage, pSpellId==0, static_dmg[i] != 0, false, 1, forced_pct_mod_cur, i );
}

void Spell::SpellEffectSummonObjectWild(uint32 i)
{
	if(!u_caster)
	{ 
		return;
	}


	uint32 GOEntry;
	if( forced_miscvalues[i] != 0 )
		GOEntry = forced_miscvalues[i];
	else
		GOEntry = GetProto()->eff[i].EffectMiscValue;
	//let's check if paleyr already has this summoned. No idea what should be max but atm this is a server crashing exploit
	if( p_caster )
	{
		SimplePointerListNode<GOSummon>	*itr;
		for( itr = p_caster->PossibleGoSummons.begin(); itr != p_caster->PossibleGoSummons.end(); itr = itr->Next() )
			if( itr->data->entry == GOEntry )
			{
				//try to remove this go from world
				GameObject *go = p_caster->GetMapMgr()->GetGameObject( itr->data->GUID );
				if( go )
					go->Despawn( 0 );
				p_caster->PossibleGoSummons.remove( itr, 1 );
				break;
			}
	}

	// spawn a new one
	GameObject *GoSummon = u_caster->GetMapMgr()->CreateGameObject( GOEntry );
	float x,y,z;
	if( m_targets.m_destX != 0 && ( m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION ) )
	{
		x = m_targets.m_destX;
		y = m_targets.m_destY;
		z = m_targets.m_destZ+1.0f;
	}
	else
	{
		x = m_caster->GetPositionX();
		y = m_caster->GetPositionY();
		z = m_caster->GetPositionZ()+1.0f;
	}
	if(!GoSummon->CreateFromProto( GOEntry,	m_caster->GetMapId(), x, y, z, m_caster->GetOrientation() ))
	{
		sLog.outDebug("Could not create GameObject %u from proto\n",GOEntry );
		sGarbageCollection.AddObject( GoSummon );
		GoSummon = NULL;
		return;
	}
	
	GoSummon->SetInstanceID(m_caster->GetInstanceID());
	GoSummon->SetUInt32Value(GAMEOBJECT_LEVEL, u_caster->getLevel());
	GoSummon->SetUInt64Value(OBJECT_FIELD_CREATED_BY, m_caster->GetGUID());
	GoSummon->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, 0);
	GoSummon->PushToWorld(u_caster->GetMapMgr());
	GoSummon->SetSummoned(u_caster);
	
	//avoid infinit spawns ...could get to be an exploit
	uint32 duration = GetDuration();
	if( duration == (uint32)-1 || duration > 60*60*1000 )
		duration = 10*60*1000;

	sEventMgr.AddEvent(GoSummon, &GameObject::ExpireAndDelete, EVENT_GAMEOBJECT_EXPIRE, duration, 1,0);

	//antisummonspam
	if( p_caster )
	{
		GOSummon *PCasterAntiSummonSpam = new GOSummon;
		PCasterAntiSummonSpam->entry = GOEntry;
		PCasterAntiSummonSpam->GUID = GoSummon->GetGUID();
		p_caster->PossibleGoSummons.push_front( PCasterAntiSummonSpam );
	}
}

void Spell::SpellEffectScriptEffect(uint32 i) // Script Effect
{
	uint32 spellId = GetProto()->Id;

	// Try a dummy SpellHandler
	if(sScriptMgr.CallScriptedDummySpell(GetProto(), i, this))
	{ 
		return;
	}

	switch(spellId)
	{
		case 62525: //Attuned to Nature 10 Dose Reduction
		{
			Unit* ut = GetUnitTarget();
			if( ut )
				ut->RemoveAura( 62519, 0, AURA_SEARCH_POSITIVE, 10 );
		}break;
		case 97985: //Feral Swiftness Clear
		{
			Unit* ut = GetUnitTarget();
			if( ut )
			{
				ut->RemoveAllAurasByMechanicMulti( (1<<MECHANIC_ROOTED)|(1<<MECHANIC_ENSNARED)|(1<<MECHANIC_SLOWED)|(1<<MECHANIC_DAZED), -1, true );
			}
		}break;
		case 12355:	//Impact
			{
				//spreads any Fire damage over time effects to nearby enemy targets within $12355a2 yards
				Unit* ut = GetUnitTarget();
				if( ut && u_caster )
				{
					bool has_targets = false;
					for(uint32 i=NEG_AURAS_START;i<MAX_NEGATIVE_AURAS1(ut);i++)
					{
						Aura *a = ut->m_auras[i];
						if(  a != NULL
							&& a->GetCasterGUID() == m_caster->GetGUID() 
							&& (a->GetSpellProto()->c_is_flags & SPELL_FLAG_IS_DAMAGING) 
							&& ((a->GetSpellProto()->SchoolMask & SCHOOL_MASK_FIRE) == SCHOOL_MASK_FIRE ) //skip frostfire cause it bugs out badly
							&& (a->GetSpellProto()->quick_tickcount > 1 ) )
							{
								uint32 spell_id = a->GetSpellProto()->Id;
								//cast this spell on everyone attackable within x yards of our target
								if( has_targets == false )
								{
//									FillTargetMap( 2 );
									FillAllTargetsInArea( ut->GetPosition(), 2 );
									has_targets = true;
								}
								bool EffectFilter[MAX_SPELL_EFFECT_COUNT];
								//deny all effects
								for(uint32 i=0;i<MAX_SPELL_EFFECT_COUNT;i++)
									EffectFilter[i]=true;
								//alow aura effects
								for(uint32 i=0;i<MIN((uint32)MAX_SPELL_EFFECT_COUNT,a->m_modcount);i++)
									EffectFilter[ MIN(MAX_SPELL_EFFECT_COUNT-1,a->m_modList[i].i) ]=false;
								SpellEntry *sp = a->GetSpellProto();
								//cast this filtered spell on others
								std::vector<uint64>::iterator i, i2;
								for(i= m_targetUnits[2].begin();i != m_targetUnits[2].end();)
								{
									i2 = i++;
									//do not refresh dots on original target
									if( (*i2) == ut->GetGUID() )
										continue;
//									u_caster->CastSpell( (*i2), spell_id, true );
									SpellCastTargets targets((*i2));
									Spell *newSpell = SpellPool.PooledNew( __FILE__, __LINE__ );
									newSpell->Init(u_caster, sp, true, 0);
									newSpell->ProcedOnSpell = GetProto();
									for(int32 i=0;i<MAX_SPELL_EFFECT_COUNT;i++)
										newSpell->redirected_effect[i]=EffectFilter[i];
									newSpell->prepare(&targets);
								}
							}
					}
				}
			}break;
		case 77801:	//Demon Soul
			{
				uint32 spellid1 = 0;
				switch(unitTarget->GetEntry())
				{
					case 416: //Imp
					{   
						spellid1 = 79459;
				
					}break;
					case 417: //Felhunter
					{
						spellid1 = 79460;
				
					}break;
					case 1860: //VoidWalker
					{
						spellid1 = 79464;
					}break;
					case 1863: //Succubus
					{
						spellid1 = 79463;
					}break;
					case 17252: //felguard
					{
						spellid1 = 79462;
					}break;
				}
				//now caster gains this buff
				if (spellid1)
				{
					SpellEntry *sp = dbcSpell.LookupEntry(spellid1);
					if (sp) 
						u_caster->CastSpell( u_caster, sp, true );
				}
			}break;
		case 87151: //Archangel
			{
				Unit* ut = GetUnitTarget();
				if( ut )
				{
					uint16 stacks;
					stacks = ut->RemoveAura( 81661, 0, AURA_SEARCH_POSITIVE, MAX_AURAS );	//watch to not remove the passsive evangelism talent
					if( stacks == 0 )
						stacks = ut->RemoveAura( 81660, 0, AURA_SEARCH_POSITIVE, MAX_AURAS );	//watch to not remove the passsive evangelism talent

//					if( stacks >= 5 )
					if( stacks >= 1 )
					{
						ut->RemoveAura( 94709 );	//archangel enabler
						ut->CastSpell( ut, 87152, true );	//restore mana
						SpellEntry *se = dbcSpell.LookupEntryForced( 81700 );
						Spell*sp=SpellPool.PooledNew( __FILE__, __LINE__ );
						sp->Init(ut,se,true,NULL);
						SpellCastTargets tgt( ut->GetGUID() );
						sp->forced_basepoints[0] = se->eff[0].EffectBasePoints * stacks;
						sp->forced_basepoints[1] = se->eff[1].EffectBasePoints * stacks;
						sp->forced_basepoints[2] = se->eff[2].EffectBasePoints * stacks;
						sp->prepare(&tgt);
					}
					else
					{
						stacks = ut->RemoveAuraByNameHash( SPELL_HASH_DARK_EVANGELISM, 0, AURA_SEARCH_POSITIVE, MAX_PASSIVE_AURAS );	//watch to not remove the passsive evangelism talent
//						if( stacks >= 5 )
						if( stacks >= 1 )
						{
							ut->RemoveAura( 94709 );	//archangel enabler
//							ut->CastSpell( ut, 87153, true );	//restore mana
							SpellEntry *se = dbcSpell.LookupEntryForced( 87153 );
							Spell*sp=SpellPool.PooledNew( __FILE__, __LINE__ );
							sp->Init(ut,se,true,NULL);
							SpellCastTargets tgt( ut->GetGUID() );
							sp->forced_basepoints[0] = se->eff[0].EffectBasePoints * stacks;
							sp->forced_basepoints[1] = se->eff[1].EffectBasePoints * stacks;
							sp->forced_basepoints[2] = se->eff[2].EffectBasePoints * stacks;
							sp->prepare(&tgt);
						}
					}
				}
			}break;
		case 64323:	//Book of Glyph Mastery
			{
				if( p_caster )
				{
					const uint32 spell_list[] = {63862,89927,64256,64268,64246,64247,64248,64249,64250,64251,64252,64253,64254,64255,64257,64270,64271,64273,64274,64275,64276,64277,64278,64279,64280,64281,64282,64283,64284,64285,64286,64287,64288,64289,64291,64294,64295,64296,64297,64298,64300,64302,64303,64304,64305,64307,64308,64309,64310,64311,64313,64314,64315,64316,64317,64318,0};
					uint32 list_size = sizeof( spell_list ) / sizeof(uint32);
					for(uint32 i=0;i<list_size;i++)
					{
//						uint32 random_spell_to_learn_ind = RandomUInt( ) % list_size;
						uint32 random_spell_to_learn_ind = i;
						if( p_caster->HasSpell( spell_list[ random_spell_to_learn_ind ] ) == false )
						{
							p_caster->addSpell( spell_list[ random_spell_to_learn_ind ], true );
							break;
						}
					}
				}
			}break;
		//Make Player Destroy Totems - note that some versions were made using "reagents" to remove totems. This is only double check
		case 66744:
		{
			Player *p_target = GetPlayerTarget();
			if( p_target && p_target->GetItemInterface() )
			{
				p_target->GetItemInterface()->RemoveItemAmt(5177 ,1);	//water
				p_target->GetItemInterface()->RemoveItemAmt(5175 ,1);	//earth
				p_target->GetItemInterface()->RemoveItemAmt(5176 ,1);	//fire
				p_target->GetItemInterface()->RemoveItemAmt(5178 ,1);	//air
			}
		}break;
		//haunt
		case 48181:
		case 59161:
		case 59163:
		case 59164:
			{
				if( p_caster )
				{
					//effect1 should have made a dmg, unless some proc messed it up we should be able to get that
					uint32 eff1_dmg = p_caster->m_dmg_made_last_time;
					ProcTriggerSpell *pts= new ProcTriggerSpell( GetProto(), NULL );
					pts->procFlags = PROC_ON_AURA_REMOVE;
					pts->caster = p_caster->GetGUID();	//we will deal dmg on this guy
					pts->procChance = 100;
					pts->procCharges = 1;
					pts->created_with_value = eff1_dmg;
					//pts.spellId = 48210;		//we will use this for all spells since we need to force deal dmg to friendly
					pts->spellId = 50091;		//we will use this for all spells since we need to force deal dmg to friendly
					unitTarget->RegisterProcStruct(pts);
				}
			}break;
		//Everlasting Affliction
		case 47422:
			{
				Aura *a = unitTarget->HasAuraWithNameHash( SPELL_HASH_CORRUPTION, u_caster->GetGUID(), AURA_SEARCH_NEGATIVE );
				if( a )
					a->ResetDuration();
			}break;
		//master's call
		case 53271:
			{
				if( p_caster )
				{
					//caster is player
					Unit *target1 = p_caster->GetSummon(); //this spell always targets the pet and someone else
					Unit *target2;
					if( unitTarget == target1 )  //no point double casting it on pet. Let's help player cast it on self
						target2 = p_caster; 
					else
						target2 = unitTarget;
					if( target1 )
					{
						target1->RemoveAurasMovementImpairing();
						p_caster->CastSpell( target1, 54216, true );
					}
					if( target2 )
					{
						target2->RemoveAurasMovementImpairing();
						p_caster->CastSpell( target2, 54216, true );
					}
				}
			}break;
		//there is no break here !!! we remove auras from target too !
		//Escape Artist
		case 20589:
			{
				if( unitTarget )
					unitTarget->RemoveAurasMovementImpairing();				
			}break;
		//chimera shot
/*		case 53209:
			{
				if( u_caster &&  unitTarget )
				{
					Aura *a;
					a = unitTarget->HasAuraWithNameHash( SPELL_HASH_VIPER_STING, 0, AURA_SEARCH_NEGATIVE );
					if( a )
					{
						a->ResetTimeLeft();
						sEventMgr.ModifyEventTimeLeft(a, EVENT_AURA_REMOVE, a->GetDuration());
						unitTarget->ModVisualAuraStackCount(a, 0);
						u_caster->Energize( u_caster, 53358, unitTarget->GetMaxPower( POWER_TYPE_MANA ) * 4 / 100 * 60 / 100, POWER_TYPE_MANA );
					}
					else
					{
						a = unitTarget->HasAuraWithNameHash( SPELL_HASH_SCORPID_STING, 0, AURA_SEARCH_NEGATIVE );
						if( a )
						{
							a->ResetTimeLeft();
							sEventMgr.ModifyEventTimeLeft(a, EVENT_AURA_REMOVE, a->GetDuration());
							unitTarget->ModVisualAuraStackCount(a, 0);
							u_caster->CastSpell( unitTarget, 53359, true );
						}
						else
						{
							a = unitTarget->HasAuraWithNameHash( SPELL_HASH_SERPENT_STING );
							if( a )
							{
								a->ResetTimeLeft();
								sEventMgr.ModifyEventTimeLeft(a, EVENT_AURA_REMOVE, a->GetDuration());
								unitTarget->ModVisualAuraStackCount(a, 0);
								u_caster->SpellNonMeleeDamageLog( unitTarget, 53353, a->m_modList[0].m_amount * 5 * 40 / 100, true );
							}
						}
					}
				}
			}break; */
		//Pestilence
		case 50842: //take diseases from target and spread to nearby targets
		{
			Unit *attacktarget = m_caster->GetMapMgr()->GetUnit( m_targets.m_unitTarget );
			if( attacktarget == NULL && p_caster )
				attacktarget = m_caster->GetMapMgr()->GetUnit( p_caster->GetSelection() );
			if( attacktarget && u_caster )
			{
				Aura *frost,*bp;
				frost = attacktarget->HasAuraWithNameHash( SPELL_HASH_FROST_FEVER, 0, AURA_SEARCH_NEGATIVE ); //frost feaver
				bp = attacktarget->HasAuraWithNameHash( SPELL_HASH_BLOOD_PLAGUE, 0, AURA_SEARCH_NEGATIVE ); //blood plague
				float r = 10.0f * 10.0f;
				if( frost || bp )
				{
					SpellEntry *spf,  *bpf;
					if( frost )
						spf = frost->GetSpellProto();
					if( bp )
						bpf = bp->GetSpellProto();
					m_caster->AquireInrangeLock(); //make sure to release lock before exit function !
					InrangeLoopExitAutoCallback AutoLock;
					for(InRangeSetRecProt::iterator itr = m_caster->GetInRangeSetBegin( AutoLock ); itr != m_caster->GetInRangeSetEnd(); itr++ )
					{
						// don't add objects that are not units and that are dead
						if( !( (*itr)->IsUnit() ) || ! SafeUnitCast( *itr )->isAlive())
							continue;

						Unit *target = SafeUnitCast( ( *itr ) );

						//do not refresh on the original target
						if( target == attacktarget )
							continue;

						if(IsInrange(m_caster,target,r))
						{
							if( isAttackable( u_caster, target, !( GetProto()->c_is_flags & SPELL_FLAG_IS_TARGETINGSTEALTHED ) ) )
							{
								uint8 did_hit_result = DidHit(i, target );
								if( did_hit_result == SPELL_DID_HIT_SUCCESS )
								{
									if( frost )
									{
//										Aura *aur;
//										aur = AuraPool.PooledNew( __FILE__, __LINE__ );
//										aur->Init( frost->GetSpellProto(), frost->GetDuration(), u_caster, target );
//										target->AddAura(aur);
//										u_caster->CastSpell( target, frost->GetSpellProto(), true );
										Spell*sp=SpellPool.PooledNew( __FILE__, __LINE__ );
										sp->Init(u_caster, spf, true, NULL);
										SpellCastTargets tgt( target->GetGUID() );
										sp->ProcedOnSpell = GetProto();	//need this for "Contagion"
										sp->forced_pct_mod[0]=50;
										sp->forced_pct_mod[1]=50;
										sp->forced_pct_mod[2]=50;
										sp->prepare(&tgt);
									}
									if( bp )
									{
//										Aura *aur;
//										aur = AuraPool.PooledNew( __FILE__, __LINE__ );
//										aur->Init( bp->GetSpellProto(), bp->GetDuration(), u_caster, target );
//										target->AddAura(aur);
//										u_caster->CastSpell( target, bp->GetSpellProto(), true );
										Spell*sp=SpellPool.PooledNew( __FILE__, __LINE__ );
										sp->Init(u_caster, bpf, true, NULL);
										SpellCastTargets tgt( target->GetGUID() );
										sp->ProcedOnSpell = GetProto();	//need this for "Contagion"
										sp->forced_pct_mod[0]=50;
										sp->forced_pct_mod[1]=50;
										sp->forced_pct_mod[2]=50;
										sp->prepare(&tgt);
									}
								}
							}
						}
					}
					m_caster->ReleaseInrangeLock();
				}
			}
		}break;
		//improved sprint
		case 30918:
			{
				if( !u_caster )
					return;
				u_caster->RemoveAurasMovementImpairing();
			}break;
	// Arcane Missiles
	/*
	case 5143://Rank 1
	case 5144://Rank 2
	case 5145://Rank 3
	case 8416://Rank 4
	case 8417://Rank 5
	case 10211://Rank 6
	case 10212://Rank 7
	case 25345://Rank 8
	{
		if(m_tmpAffect == 0)
		{
			Affect* aff = new Affect(GetProto(),GetDuration(sSpellDuration.LookupEntry(GetProto()->DurationIndex)),m_GetGUID(),m_caster);
			m_tmpAffect = aff;
		}
		if(GetProto()->eff[0].EffectBasePoints < 0)
			m_tmpAffect->SetNegative();

		m_tmpAffect->SetPeriodicTriggerSpell(GetProto()->eff[0].EffectTriggerSpell,GetProto()->eff[0].EffectAmplitude,damage);
	}break;
	*/

//	case SPELL_HASH_DEMONIC_EMPOWERMENT:	//no namehash to avoid cyclic cast !
	case 47193:	//demonic empowerment
	{
		if ( unitTarget->GetUInt32Value( OBJECT_FIELD_ENTRY ) == 416 ) //in case it is imp
			unitTarget->CastSpell( unitTarget, 54444, true );
		else if ( unitTarget->GetUInt32Value( OBJECT_FIELD_ENTRY ) == 1860 ) //VoidWalker
			unitTarget->CastSpell( unitTarget, 54443, true );
		else if ( unitTarget->GetUInt32Value( OBJECT_FIELD_ENTRY ) == 1863 ) //Succubus
			unitTarget->CastSpell( unitTarget, 54436, true );
		else if ( unitTarget->GetUInt32Value( OBJECT_FIELD_ENTRY ) == 417 ) //Felhunter
			unitTarget->CastSpell( unitTarget, 54509, true );
		else if ( unitTarget->GetUInt32Value( OBJECT_FIELD_ENTRY ) == 17252 ) //Felguard
			unitTarget->CastSpell( unitTarget, 54508, true );
	}break;
	// Warlock Healthstones, just how much health does a lock need?
	case 6201:		// Minor Healthstone
		if( !p_caster )
			return;
		if (p_caster->HasSpell(18692))
		{
			CreateItem(19004);
			break;
		}
		if (p_caster->HasSpell(18693))
		{
			CreateItem(19005);
			break;
		}
		CreateItem(5512);
		break;
	case 6202:		// Lesser Healthstone
		if( !p_caster )
			return;
		if (p_caster->HasSpell(18693))	// Improved Healthstone (2)
		{
			CreateItem(19007);
			break;
		}
		if (p_caster->HasSpell(18692))	// Improved Healthstone (1)
		{
			CreateItem(19006);
			break;
		}
		CreateItem(5511);
		break;
	case 5699:		// Healthstone
		if( !p_caster )
			return;
		if (p_caster->HasSpell(18693))	// Improved Healthstone (2)
		{
			CreateItem(19009);
			break;
		}
		if (p_caster->HasSpell(18692))	// Improved Healthstone (1)
		{
			CreateItem(19008);
			break;
		}
		CreateItem(5509);
		break;
	case 11729:		// Greater Healthstone
		if( !p_caster )
			return;
		if (p_caster->HasSpell(18693))	// Improved Healthstone (2)
		{
			CreateItem(19011);
			break;
		}
		if (p_caster->HasSpell(18692))	// Improved Healthstone (1)
		{
			CreateItem(19010);
			break;
		}
		CreateItem(5510);
		break;
	case 11730:		// Major Healthstone
		if( !p_caster )
			return;
		if (p_caster->HasSpell(18693))	// Improved Healthstone (2)
		{
			CreateItem(19013);
			break;
		}
		if (p_caster->HasSpell(18692))	// Improved Healthstone (1)
		{
			CreateItem(19012);
			break;
		}
		CreateItem(9421);
		break;
	case 27230:		// Master Healthstone
		if( !p_caster )
			return;
		if (p_caster->HasSpell(18693))	// Improved Healthstone (2)
		{
			CreateItem(22105);
			break;
		}
		if (p_caster->HasSpell(18692))	// Improved Healthstone (1)
		{
			CreateItem(22104);
			break;
		}
		CreateItem(22103);
		break;
	case 47871:		// Demonic Healthstone
		if( !p_caster )
			return;
		if (p_caster->HasSpell(18693))	// Improved Healthstone (2)
		{
			CreateItem(36891);
			break;
		}
		if (p_caster->HasSpell(18692))	// Improved Healthstone (1)
		{
			CreateItem(36890);
			break;
		}
		CreateItem(36889);
		break;
	case 47878:		// Fel Healthstone
		if( !p_caster )
			return;
		if (p_caster->HasSpell(18693))	// Improved Healthstone (2)
		{
			CreateItem(36894);
			break;
		}
		if (p_caster->HasSpell(18692))	// Improved Healthstone (1)
		{
			CreateItem(36893);
			break;
		}
		CreateItem(36892);
		break;

	// Holy Light
	case 635:
	case 639:
	case 647:
	case 1026:
	case 1042:
	case 3472:
	case 10328:
	case 10329:
	case 10348:
	case 25292:
	case 27135:
	case 27136:
	case 48781:
	case 48782:
	//Flash of light
	case 19750:
	case 19939:
	case 19940:
	case 19941:
	case 19942:
	case 19943:
	case 27137:
	case 48784:
	case 48785:
//		if( u_caster->HasAura( 20216 ) )
//			Heal((int32)damage, true);
//		else
//			Heal((int32)damage);
		u_caster->SpellHeal( unitTarget, GetProto(), damage, pSpellId==0, static_dmg[i] != 0, false, 1, forced_pct_mod_cur, i );
	break;

	// Judgement
/*	case 53407:	//Judgement of Justice - preventing them from fleeing and limiting their movement speed
		{
			if( unitTarget )
				for(uint32 x= MAX_POSITIVE_AURAS; x < MAX_NEGATIVE_AURAS1(unitTarget); ++x)
					if(unitTarget->m_auras[x])
						for(uint32 y = 0; y < 3; ++y)
							switch(unitTarget->m_auras[x]->GetSpellProto()->eff[y].EffectApplyAuraName)
							{
								case SPELL_AURA_MOD_INCREASE_SPEED:
								case SPELL_AURA_MOD_INCREASE_MOUNTED_SPEED:
									unitTarget->m_auras[x]->Remove();
									y=100;	//removed aura, do not use it anymore !
							}	
		} //no beak, continue execution !
	case 53408: //Judgement of Wisdom - giving each attack a chance to restore 2% of the attacker's base mana. 
	case 20271: //Judgement of Light - granting attacks made against the judged enemy a chance of healing the attacker for [0.10 * SPH + 0.10 * AP]. */
	case 20271: //Judgement - in 403 there is THE judgement spell :P
		{
			if(!unitTarget || !p_caster)
			{ 
				return;
			}

			//get my seal from target
			uint32 judgespell = 0;
			SpellEntry	*sp = NULL;
			for(uint32 buffitr=FIRST_AURA_SLOT;buffitr<MAX_POSITIVE_AURAS1(p_caster);buffitr++)
				if( p_caster->m_auras[ buffitr ] 
					&& ( p_caster->m_auras[ buffitr ]->GetSpellProto()->BGR_one_buff_on_target & SPELL_TYPE_SEAL ) )
				{
					sp = p_caster->m_auras[ buffitr ]->GetSpellProto();
					judgespell = sp->NameHash;
					//on retail judgement eats seal ?
//					p_caster->m_auras[ buffitr ]->Remove();
					break;
				}
			
			if( judgespell == SPELL_HASH_SEAL_OF_BLOOD 
//				|| judgespell == SPELL_HASH_SEAL_OF_THE_MARTYR
				)
			{
				//instantly causing ${0.26*$mw+0.11*$AP+0.18*$SPH} to ${0.26*$MW+0.11*$AP+0.18*$SPH} Holy damage at the cost of health equal to 33% of the damage caused.
				Item *it;
				float MVS = 0;
				if(p_caster->GetItemInterface())
				{
					it = p_caster->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_MAINHAND);
					if(it)
						MVS = ( it->GetProto()->Delay + 75 ) / 1000.0f; //yeah that 75 is really strange but at this time it produces blizzlike value :S
				}
				else MVS = p_caster->GetUInt32Value( UNIT_FIELD_BASEATTACKTIME ) / 1000.0f;
				uint32 dmg_to_be_done = (uint32)(0.26f*MVS + p_caster->GetAP()*0.11f + 0.18f*p_caster->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SCHOOL_HOLY));
				int32 HP_before = unitTarget->GetUInt32Value( UNIT_FIELD_HEALTH );

				dealdamage tdmg;
				tdmg.base_dmg = dmg_to_be_done;
				tdmg.pct_mod_final_dmg = forced_pct_mod_cur;
				tdmg.StaticDamage = false;

				p_caster->SpellNonMeleeDamageLog( unitTarget, GetProto(), &tdmg, 1, 0 );
				int32 HP_after = unitTarget->GetUInt32Value( UNIT_FIELD_HEALTH );
				uint32 self_dmg = abs(HP_after-HP_before) * 33 / 100;

				tdmg.base_dmg = self_dmg;
				p_caster->SpellNonMeleeDamageLog( p_caster, GetProto(), &tdmg, 1, 0 );
			}
			else  if( 
//				judgespell == SPELL_HASH_SEAL_OF_CORRUPTION || 
				judgespell == SPELL_HASH_SEAL_OF_TRUTH	//${1+0.223*$SPH+0.142*$AP}
				)
			{
				//will deal ${1+0.22*$SPH+0.14*$AP} Holy damage to an enemy, increased by 10% for each application of Blood Corruption on the target.
//				uint32 dmg_to_be_done = 1 + uint32(p_caster->GetAP()*0.14f + 0.22f*p_caster->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SCHOOL_HOLY));
				float SPH = p_caster->GetDamageDoneMod( SCHOOL_HOLY ) * p_caster->GetDamageDonePctMod( SCHOOL_HOLY );
				uint32 dmg_to_be_done = 1 + uint32(p_caster->GetAP()*0.142f + 0.223f*SPH);
				uint32 aura_count = unitTarget->CountAura( 31803, AURA_SEARCH_NEGATIVE ); //CENSURE
				dmg_to_be_done = dmg_to_be_done * (100 + 20 * aura_count) / 100;

				dealdamage tdmg;
				tdmg.base_dmg = dmg_to_be_done;
				tdmg.pct_mod_final_dmg = forced_pct_mod_cur;
				tdmg.StaticDamage = true;

				p_caster->SpellNonMeleeDamageLog (unitTarget, GetProto(), &tdmg, 1, 0 );
			}
			else if( judgespell == SPELL_HASH_SEAL_OF_JUSTICE 
				|| judgespell == SPELL_HASH_SEAL_OF_INSIGHT		//Unleashing this Seal's energy will deal ${1+0.25*$SPH+0.16*$AP} Holy damage to an enemy and restore $s1% of the Paladin's base mana.
//				|| judgespell == SPELL_HASH_SEAL_OF_WISDOM 
				)
			{
				//will deal ${1+0.25*$SPH+0.16*$AP} Holy damage to an enemy.
				//Unleashing this Seal's energy will deal ${1+0.25*$SPH+0.16*$AP} Holy damage to an enemy and restore $s1% of the Paladin's base mana.
//				uint32 dmg_to_be_done = 1 + (uint32)(p_caster->GetAP()*0.16f + 0.25f*p_caster->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SCHOOL_HOLY));
				//1 + 0.25 * holy power.16 * AP
//				uint32 dmg_to_be_done = 1 + float2int32((0.25f*p_caster->GetPower(POWER_TYPE_HOLY)+p_caster->GetAP()*0.16f));
				float SPH = p_caster->GetDamageDoneMod( SCHOOL_HOLY ) * p_caster->GetDamageDonePctMod( SCHOOL_HOLY );
				uint32 dmg_to_be_done = 1 + float2int32((0.25f*SPH+p_caster->GetAP()*0.16f));

				dealdamage tdmg;
				tdmg.base_dmg = dmg_to_be_done;
				tdmg.pct_mod_final_dmg = forced_pct_mod_cur;
				tdmg.StaticDamage = true;
				p_caster->SpellNonMeleeDamageLog( unitTarget, GetProto(), &tdmg, 1, 0 );
				if( judgespell == SPELL_HASH_SEAL_OF_INSIGHT )
					p_caster->Energize( p_caster, sp->Id, p_caster->GetUInt32Value( UNIT_FIELD_BASE_MANA ) * 15 / 100, POWER_TYPE_MANA, 0 );
			}
			else if( judgespell == SPELL_HASH_SEAL_OF_RIGHTEOUSNESS )
			{
				//will cause ${1+0.2*$AP+0.32*$SPH} Holy damage to an enemy.
				float SPH = p_caster->GetDamageDoneMod( SCHOOL_HOLY ) * p_caster->GetDamageDonePctMod( SCHOOL_HOLY );
				uint32 dmg_to_be_done = 1 + float2int32(p_caster->GetAP()*0.2f + 0.32f*SPH);
				dealdamage tdmg;
				tdmg.base_dmg = dmg_to_be_done;
				tdmg.pct_mod_final_dmg = forced_pct_mod_cur;
				tdmg.StaticDamage = true;
				p_caster->SpellNonMeleeDamageLog( unitTarget, GetProto(), &tdmg, 1, 0 );
			}
		}break; 
		/**/
	//warlock - Master Demonologist
	case 23784:
		{
			if( p_caster == NULL || unitTarget == NULL)
				return; //can't imagine how this talent got to anybody else then a player casted on pet
			uint32 casted_spell_id = 0 ;
			uint32 inc_resist_by_level = 0 ;
			uint32 inc_resist_by_level_spell = 0 ;
			if ( unitTarget->GetUInt32Value( OBJECT_FIELD_ENTRY ) == 416 ) //in case it is imp
				casted_spell_id = 23759 ;
			else if ( unitTarget->GetUInt32Value( OBJECT_FIELD_ENTRY ) == 1860 ) //VoidWalker
				casted_spell_id = 23760 ;
			else if ( unitTarget->GetUInt32Value( OBJECT_FIELD_ENTRY ) == 1863 ) //Succubus
				casted_spell_id = 23761 ;
			else if ( unitTarget->GetUInt32Value( OBJECT_FIELD_ENTRY ) == 417 ) //Felhunter
			{
				casted_spell_id = 0 ;
				inc_resist_by_level_spell = 23762 ;
				inc_resist_by_level = 20 ;
			}
			else if ( unitTarget->GetUInt32Value( OBJECT_FIELD_ENTRY ) == 17252 ) //Felguard
			{
				casted_spell_id = 35702 ;
				inc_resist_by_level_spell = 23762 ;
				inc_resist_by_level = 10 ;
			}
			if( casted_spell_id )
			{
				//for self
				Spell *sp = SpellPool.PooledNew( __FILE__, __LINE__ );
				sp->Init( p_caster, dbcSpell.LookupEntry( casted_spell_id ), true, NULL );
				SpellCastTargets tgt( p_caster->GetGUID() );
				sp->prepare( &tgt );
				//for pet
				sp = SpellPool.PooledNew( __FILE__, __LINE__ );
				sp->Init(  unitTarget, dbcSpell.LookupEntry( casted_spell_id ), true, NULL );
				SpellCastTargets tgt1( unitTarget->GetGUID() );
				sp->prepare( &tgt1 );
			}
			if( inc_resist_by_level_spell )
			{
				//for self
				Spell *sp = SpellPool.PooledNew( __FILE__, __LINE__ );
				sp->Init( p_caster, dbcSpell.LookupEntry( inc_resist_by_level_spell ), true, NULL );
				SpellCastTargets tgt( p_caster->GetGUID() );
				sp->prepare( &tgt );
				//for pet
				sp = SpellPool.PooledNew( __FILE__, __LINE__ );
				sp->Init( unitTarget, dbcSpell.LookupEntry( inc_resist_by_level_spell ), true, NULL );
				SpellCastTargets tgt1( unitTarget->GetGUID() );
				sp->prepare( &tgt1 );
			}
		}break;
	case 23830:
		{
			if( p_caster == NULL || unitTarget == NULL)
				return; //can't imagine how this talent got to anybody else then a player casted on pet
			uint32 casted_spell_id = 0 ;
			uint32 inc_resist_by_level = 0 ;
			uint32 inc_resist_by_level_spell = 0 ;
			if ( unitTarget->GetUInt32Value( OBJECT_FIELD_ENTRY ) == 416 ) //in case it is imp
				casted_spell_id = 23826 ;
			else if ( unitTarget->GetUInt32Value( OBJECT_FIELD_ENTRY ) == 1860 ) //VoidWalker
				casted_spell_id = 23841 ;
			else if ( unitTarget->GetUInt32Value( OBJECT_FIELD_ENTRY ) == 1863 ) //Succubus
				casted_spell_id = 23833 ;
			else if ( unitTarget->GetUInt32Value( OBJECT_FIELD_ENTRY ) == 417 ) //Felhunter
			{
				casted_spell_id = 1 ;
				inc_resist_by_level_spell = 23837 ;
				inc_resist_by_level = 40 ;
			}
			else if ( unitTarget->GetUInt32Value( OBJECT_FIELD_ENTRY ) == 17252 ) //Felguard
			{
				casted_spell_id = 35703 ;
				inc_resist_by_level_spell = 23837 ;
				inc_resist_by_level = 20 ;
			}
			if( casted_spell_id )
			{
				//for self
				Spell *sp = SpellPool.PooledNew( __FILE__, __LINE__ );
				sp->Init( p_caster, dbcSpell.LookupEntry( casted_spell_id ), true, NULL );
				SpellCastTargets tgt( p_caster->GetGUID() );
				sp->prepare( &tgt );
				//for pet
				sp = SpellPool.PooledNew( __FILE__, __LINE__ );
				sp->Init(  unitTarget, dbcSpell.LookupEntry( casted_spell_id ), true, NULL );
				SpellCastTargets tgt1( unitTarget->GetGUID() );
				sp->prepare( &tgt1 );
			}
			if( inc_resist_by_level_spell )
			{
				//for self
				Spell *sp = SpellPool.PooledNew( __FILE__, __LINE__ );
				sp->Init( p_caster, dbcSpell.LookupEntry( inc_resist_by_level_spell ), true, NULL );
				SpellCastTargets tgt( p_caster->GetGUID() );
				sp->prepare( &tgt );
				//for pet
				sp = SpellPool.PooledNew( __FILE__, __LINE__ );
				sp->Init( unitTarget, dbcSpell.LookupEntry( inc_resist_by_level_spell ), true, NULL );
				SpellCastTargets tgt1( unitTarget->GetGUID() );
				sp->prepare( &tgt1 );
			}
		}break;
	case 23831:
		{
			if( p_caster == NULL || unitTarget == NULL)
				return; //can't imagine how this talent got to anybody else then a player casted on pet
			uint32 casted_spell_id = 0 ;
			uint32 inc_resist_by_level = 0 ;
			uint32 inc_resist_by_level_spell = 0 ;
			if ( unitTarget->GetUInt32Value( OBJECT_FIELD_ENTRY ) == 416 ) //in case it is imp
				casted_spell_id = 23827 ;
			else if ( unitTarget->GetUInt32Value( OBJECT_FIELD_ENTRY ) == 1860 ) //VoidWalker
				casted_spell_id = 23842 ;
			else if ( unitTarget->GetUInt32Value( OBJECT_FIELD_ENTRY ) == 1863 ) //Succubus
				casted_spell_id = 23834 ;
			else if ( unitTarget->GetUInt32Value( OBJECT_FIELD_ENTRY ) == 417 ) //Felhunter
			{
				casted_spell_id = 0 ;
				inc_resist_by_level_spell = 23838 ;
				inc_resist_by_level = 60 ;
			}
			else if ( unitTarget->GetUInt32Value( OBJECT_FIELD_ENTRY ) == 17252 ) //Felguard
			{
				casted_spell_id = 35704 ;
				inc_resist_by_level_spell = 23838 ;
				inc_resist_by_level = 30 ;
			}
			if( casted_spell_id )
			{
				//for self
				Spell *sp = SpellPool.PooledNew( __FILE__, __LINE__ );
				sp->Init( p_caster, dbcSpell.LookupEntry( casted_spell_id ), true, NULL );
				SpellCastTargets tgt( p_caster->GetGUID() );
				sp->prepare( &tgt );
				//for pet
				sp = SpellPool.PooledNew( __FILE__, __LINE__ );
				sp->Init(  unitTarget, dbcSpell.LookupEntry( casted_spell_id ), true, NULL );
				SpellCastTargets tgt1( unitTarget->GetGUID() );
				sp->prepare( &tgt1 );
			}
			if( inc_resist_by_level_spell )
			{
				//for self
				Spell *sp = SpellPool.PooledNew( __FILE__, __LINE__ );
				sp->Init( p_caster, dbcSpell.LookupEntry( inc_resist_by_level_spell ), true, NULL );
				SpellCastTargets tgt( p_caster->GetGUID() );
				sp->prepare( &tgt );
				//for pet
				sp = SpellPool.PooledNew( __FILE__, __LINE__ );
				sp->Init( unitTarget, dbcSpell.LookupEntry( inc_resist_by_level_spell ), true, NULL );
				SpellCastTargets tgt1( unitTarget->GetGUID() );
				sp->prepare( &tgt1 );
			}
		}break;
	case 23832:
		{
			if( p_caster == NULL || unitTarget == NULL)
				return; //can't imagine how this talent got to anybody else then a player casted on pet
			uint32 casted_spell_id = 0 ;
			uint32 inc_resist_by_level = 0 ;
			uint32 inc_resist_by_level_spell = 0 ;
			if ( unitTarget->GetUInt32Value( OBJECT_FIELD_ENTRY ) == 416 ) //in case it is imp
				casted_spell_id = 23828 ;
			else if ( unitTarget->GetUInt32Value( OBJECT_FIELD_ENTRY ) == 1860 ) //VoidWalker
				casted_spell_id = 23843 ;
			else if ( unitTarget->GetUInt32Value( OBJECT_FIELD_ENTRY ) == 1863 ) //Succubus
				casted_spell_id = 23835 ;
			else if ( unitTarget->GetUInt32Value( OBJECT_FIELD_ENTRY ) == 417 ) //Felhunter
			{
				casted_spell_id = 0 ;
				inc_resist_by_level_spell = 23839 ;
				inc_resist_by_level = 80 ;
			}
			else if ( unitTarget->GetUInt32Value( OBJECT_FIELD_ENTRY ) == 17252 ) //Felguard
			{
				casted_spell_id = 35705 ;
				inc_resist_by_level_spell = 23839 ;
				inc_resist_by_level = 40 ;
			}
			if( casted_spell_id )
			{
				//for self
				Spell *sp = SpellPool.PooledNew( __FILE__, __LINE__ );
				sp->Init( p_caster, dbcSpell.LookupEntry( casted_spell_id ), true, NULL );
				SpellCastTargets tgt( p_caster->GetGUID() );
				sp->prepare( &tgt );
				//for pet
				sp = SpellPool.PooledNew( __FILE__, __LINE__ );
				sp->Init(  unitTarget, dbcSpell.LookupEntry( casted_spell_id ), true, NULL );
				SpellCastTargets tgt1( unitTarget->GetGUID() );
				sp->prepare( &tgt1 );
			}
			if( inc_resist_by_level_spell )
			{
				//for self
				Spell *sp = SpellPool.PooledNew( __FILE__, __LINE__ );
				sp->Init( p_caster, dbcSpell.LookupEntry( inc_resist_by_level_spell ), true, NULL );
				SpellCastTargets tgt( p_caster->GetGUID() );
				sp->prepare( &tgt );
				//for pet
				sp = SpellPool.PooledNew( __FILE__, __LINE__ );
				sp->Init( unitTarget, dbcSpell.LookupEntry( inc_resist_by_level_spell ), true, NULL );
				SpellCastTargets tgt1( unitTarget->GetGUID() );
				sp->prepare( &tgt1 );
			}
		}break;
/*		case 34026: //Hunter: Kill Command
		{
			// in this case unitTarget == pet
			if( p_caster == NULL || unitTarget == NULL || unitTarget->getDeathState() != ALIVE )
				return;

			AI_Spell* Sp = SafePetCast( unitTarget )->CreateAISpell( dbcSpell.LookupEntry( 34027 ) );

			Unit* tgt = p_caster->GetMapMgr()->GetUnit( p_caster->GetSelection() );

			if( tgt != NULL && isAttackable( p_caster, tgt ) )
			{
				unitTarget->GetAIInterface()->AttackReaction( tgt, 1 );
				unitTarget->GetAIInterface()->SetNextTarget( tgt );
			}
			else if( unitTarget->CombatStatus.GetPrimaryAttackTarget() == 0 )
				return;				
			unitTarget->GetAIInterface()->SetNextSpell( Sp );
			//TODO: Kill Command - return to previous pet target after cast

         }break; */
		case 35708:
		{
			if( p_caster == NULL || unitTarget == NULL)
				return; //can't imagine how this talent got to anybody else then a player casted on pet
			uint32 casted_spell_id = 0 ;
			uint32 inc_resist_by_level = 0 ;
			uint32 inc_resist_by_level_spell = 0 ;
			if ( unitTarget->GetUInt32Value( OBJECT_FIELD_ENTRY ) == 416 ) //in case it is imp
				casted_spell_id = 23829 ;
			else if ( unitTarget->GetUInt32Value( OBJECT_FIELD_ENTRY ) == 1860 ) //VoidWalker
				casted_spell_id = 23844 ;
			else if ( unitTarget->GetUInt32Value( OBJECT_FIELD_ENTRY ) == 1863 ) //Succubus
				casted_spell_id = 23836 ;
			else if ( unitTarget->GetUInt32Value( OBJECT_FIELD_ENTRY ) == 417 ) //Felhunter
			{
				casted_spell_id = 0 ;
				inc_resist_by_level_spell = 23840 ;
				inc_resist_by_level = 100 ;
			}
			else if ( unitTarget->GetUInt32Value( OBJECT_FIELD_ENTRY ) == 17252 ) //Felguard
			{
				casted_spell_id = 35706 ;
				inc_resist_by_level_spell = 23840 ;
				inc_resist_by_level = 50 ;
			}
			if( casted_spell_id )
			{
				//for self
				Spell *sp = SpellPool.PooledNew( __FILE__, __LINE__ );
				sp->Init( p_caster, dbcSpell.LookupEntry( casted_spell_id ), true, NULL );
				SpellCastTargets tgt( p_caster->GetGUID() );
				sp->prepare( &tgt );
				//for pet
				sp = SpellPool.PooledNew( __FILE__, __LINE__ );
				sp->Init(  unitTarget, dbcSpell.LookupEntry( casted_spell_id ), true, NULL );
				SpellCastTargets tgt1( unitTarget->GetGUID() );
				sp->prepare( &tgt1 );
			}
			if( inc_resist_by_level_spell )
			{
				//for self
				Spell *sp = SpellPool.PooledNew( __FILE__, __LINE__ );
				sp->Init( p_caster, dbcSpell.LookupEntry( inc_resist_by_level_spell ), true, NULL );
				SpellCastTargets tgt( p_caster->GetGUID() );
				sp->prepare( &tgt );
				//for pet
				sp = SpellPool.PooledNew( __FILE__, __LINE__ );
				sp->Init( unitTarget, dbcSpell.LookupEntry( inc_resist_by_level_spell ), true, NULL );
				SpellCastTargets tgt1( unitTarget->GetGUID() );
				sp->prepare( &tgt1 );
			}
		}break;
	}
}

void Spell::SpellEffectSanctuary(uint32 i) // Stop all attacks made to you
{
	if(!u_caster)
	{ 
		return;
	}

	//warning this causes crashes !
//	InRangeSet::iterator itr = u_caster->GetInRangeOppFactsSetBegin();
//	InRangeSet::iterator itr_end = u_caster->GetInRangeOppFactsSetEnd();
	//use these instead
	InrangeLoopExitAutoCallback AutoLock;
	InRangeSetRecProt::iterator itr = u_caster->GetInRangeSetBegin( AutoLock );
	InRangeSetRecProt::iterator itr_end = u_caster->GetInRangeSetEnd();
	Unit * pUnit;

	if(u_caster->IsPlayer())
		SafePlayerCast(u_caster)->RemoveAllAuraType(SPELL_AURA_MOD_ROOT);

	u_caster->AquireInrangeLock();
	for( ; itr != itr_end; ++itr )
		if( (*itr)->IsUnit() )
		{
			pUnit = SafeUnitCast(*itr);

			if( pUnit && pUnit->GetTypeId() == TYPEID_UNIT )
				pUnit->GetAIInterface()->RemoveThreatByPtr( unitTarget );
		}
	u_caster->ReleaseInrangeLock();
}

void Spell::SpellEffectAddComboPoints(uint32 i) // Add Combo Points
{
	if(!p_caster)
	{ 
		return;
	}
  
	damage = MAX(1,damage);	// 403 : or maybe just add 1 ? random spell use the +1 or not at effect calc
	//if this is a procspell Ruthlessness (maybe others later)
	if(pSpellId && GetProto()->Id==14157)
	{
		//it seems this combo adding procspell is going to change combopoint count before they will get reseted. We add it after the reset
		/* burlex - this is wrong, and exploitable.. :/ if someone uses this they will have unlimited combo points */
		//re-enabled this by Zack. Explained why it is used + recheked to make sure initialization is good ...
		// while casting a spell talent will trigger uppon the spell prepare faze
		// the effect of the talent is to add 1 combo point but when triggering spell finishes it will clear the extra combo point
		p_caster->m_spellcomboPoints += damage;
		return;
	}
	p_caster->AddComboPoints(p_caster->GetSelection(), damage);
}

void Spell::SpellEffectCreateHouse(uint32 i) // Create House
{


}

void Spell::SpellEffectDuel(uint32 i) // Duel
{
	if(!p_caster)
	{ 
		return;
	}
	if(!p_caster->isAlive())
	{ 
		return;
	}

	if (p_caster->IsStealth())
	{
		SendCastResult(SPELL_FAILED_CANT_DUEL_WHILE_STEALTHED);
		return; // Player is stealth
	}
	if(!playerTarget)
	{ 
		return;
	}
	if(playerTarget == p_caster)
	{ 
		return;
	}

	/* not implemented yet
	TODO: dueling zones ? ( SPELL_FAILED_NO_DUELING )
	if (player->IsInvisible())
	{
		SendCastResult(SPELL_FAILED_CANT_DUEL_WHILE_INVISIBLE);
		return;
	}
	*/

	//Player *pTarget = sObjHolder.GetObject<Player>(player->GetSelection());	   //  hacky.. and will screw up if plr is deselected..
	if (!playerTarget)
	{
		SendCastResult(SPELL_FAILED_BAD_TARGETS);
		return; // invalid Target
	}
	if (!playerTarget->isAlive())
	{
		SendCastResult(SPELL_FAILED_TARGETS_DEAD);
		return; // Target not alive
	}
	if (playerTarget->hasStateFlag(UF_ATTACKING))
	{
		SendCastResult(SPELL_FAILED_TARGET_IN_COMBAT);
		return; // Target in combat with another unit
	}
	if (playerTarget->DuelingWith != NULL)
	{
		SendCastResult(SPELL_FAILED_TARGET_DUELING);
		return; // Already Dueling
	}
	if (playerTarget->m_bg != NULL)
	{
		SendCastResult(SPELL_FAILED_NOT_HERE);
		return; // Already Dueling
	}

	p_caster->RequestDuel(playerTarget);
}

void Spell::SpellEffectStuck(uint32 i)
{
    if(!playerTarget || playerTarget != p_caster)
    { 
        return;
    }

	//remove all auras from him
	for(uint32 buffitr=0;buffitr<MAX_AURAS;buffitr++)
		if( playerTarget->m_auras[ buffitr ] )
			playerTarget->m_auras[ buffitr ]->Remove();

	//revive player to avoid all those tickets about being stuck
	playerTarget->ResurrectPlayer();

	//some noobs miss bind location so we force them in main city
	if( playerTarget->GetBindPositionX() == 0 ) 
	{
		if( playerTarget->GetTeam() == 0 )
			playerTarget->SetBindPoint( -9100,406,93,0,0);
		else
			playerTarget->SetBindPoint( 1371,-4370,27,1,0);
	}

	sEventMgr.AddEvent(playerTarget,&Player::EventTeleport,playerTarget->GetBindMapId(),playerTarget->GetBindPositionX(),playerTarget->GetBindPositionY(),
		playerTarget->GetBindPositionZ(),EVENT_PLAYER_TELEPORT,50,1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
	/*
	playerTarget->SafeTeleport(playerTarget->GetBindMapId(), 0, playerTarget->GetBindPositionX(), playerTarget->GetBindPositionY(), playerTarget->GetBindPositionZ(), 3.14f);*/
}

void Spell::SpellEffectSummonPlayer(uint32 i)
{
	if( playerTarget == NULL )
	{ 
		return;
	}

	// vojta: from 2.4 players can be summoned on another map
	//if( m_caster->GetMapMgr()->GetMapInfo() && m_caster->GetMapMgr()->GetMapInfo()->type != INSTANCE_NULL && m_caster->GetMapId() != playerTarget->GetMapId())
	//	return;
	if ( m_caster->GetMapMgr()->GetMapInfo() && playerTarget->getLevel() < m_caster->GetMapMgr()->GetMapInfo()->minlevel ) // we need some blizzlike message that player needs level xx - feel free to add it ;)
	{ 
		return;
	}

	uint32 ZoneId = 0;
	if( p_caster )
		ZoneId = p_caster->GetZoneId();
	playerTarget->SummonRequest( m_caster->GetLowGUID(), ZoneId, m_caster->GetMapId(), m_caster->GetInstanceID(), m_caster->GetPosition() );
}

void Spell::SpellEffectActivateObject(uint32 i) // Activate Object
{
	/*if(!p_caster)
		return;

	if(!gameObjTarget)
		return;

	gameObjTarget->SetUInt32Value(GAMEOBJECT_DYNAMIC, 1);

	sEventMgr.AddEvent(gameObjTarget, &GameObject::Deactivate, EVENT_GAMEOBJECT_DEACTIVATE, GetDuration(), 1);*/
}

void Spell::SpellEffectSummonTotem(uint32 i) // Summon Totem
{
	if( !p_caster || p_caster->GetMapMgr() == NULL ) 
	{ 
		return;
	}

	float x = p_caster->GetPositionX();
	float y = p_caster->GetPositionY();
	uint32 slot = m_spellInfo->eff[i].EffectImplicitTargetA - EFF_TARGET_TOTEM_EARTH;
	if(slot < 0 || slot > 3)
	{
		if( p_caster->getClass() == SHAMAN )
		{
			sLog.outDebug("Totem slot is : %u and max shoud be 3, i = %u , target = %u \n",slot,i,m_spellInfo->eff[i].EffectImplicitTargetA);
			return; // Just 4 totems
		}
		else
			slot = 0;	//who said anything that totem slot is based on target type ?
	}

	switch(m_spellInfo->eff[i].EffectMiscValueB)
	{
	case 63: 
		x -= 1.5f;
		y -= 1.5f;
		break;
	case 81: 
		x -= 1.5f;
		y += 1.5f;
		break;
	case 82:  
		x += 1.5f;
		y -= 1.5f;
		break;
	case 83: 
		x += 1.5f;
		y += 1.5f;
		break;
	default:
		break;
	}

	if(p_caster->m_TotemSlots[slot] != 0)
		p_caster->m_TotemSlots[slot]->TotemExpire();

	uint32 entry;
	if( forced_miscvalues[i] != 0 )
		entry = forced_miscvalues[i];
	else
		entry = GetProto()->eff[i].EffectMiscValue;

	CreatureInfo* ci = CreatureNameStorage.LookupEntry(entry);
	if(!ci)
	{
		sLog.outDebug("Missing totem creature entry : %u \n",entry);
		return;
	}

	// Obtain the spell we will be casting.
	TotemSpells *TotemSpell = ObjectMgr::getSingleton().GetTotemSpell(GetProto()->Id);
	if(TotemSpell == NULL) 
	{
		sLog.outDebug("Totem %u does not have any spells to cast !!!\n",entry);
//		return;
	}

	Creature * pTotem = p_caster->GetMapMgr()->CreateCreature(entry);

	p_caster->m_TotemSlots[slot] = pTotem;
	pTotem->SetTotemOwner(p_caster);
	pTotem->SetTotemSlot(slot);

	float landh = p_caster->GetMapMgr()->GetLandHeight(x,y);
	float landdiff = landh - p_caster->GetPositionZ();

	if ( landh == VMAP_VALUE_NOT_INITIALIZED || fabs(landdiff) > 4 )
		pTotem->Create(ci->Name, p_caster->GetMapId(), x, y, p_caster->GetPositionZ()+1, p_caster->GetOrientation());
	else
		pTotem->Create(ci->Name, p_caster->GetMapId(), x, y, landh, p_caster->GetOrientation());

	uint32 displayID = 0;

	if( p_caster->GetTeamInitial() == 0 )
	{
		if ( ci->Female_DisplayID != 0 )
		{
			displayID = ci->Female_DisplayID; //this is the nice solution provided by emsy
		}
		//this is the case when you are using a blizzlike db
		else if( p_caster->getRace() == RACE_DRAENEI ) 
		{
			if( ci->Male_DisplayID == 4587 )
				displayID = 19075;
			else if( ci->Male_DisplayID == 4588 )
				displayID = 19073;
			else if( ci->Male_DisplayID == 4589 )
				displayID = 19074;
			else if( ci->Male_DisplayID == 4590 )
				displayID = 19071;
			else if( ci->Male_DisplayID == 4683 )
				displayID = 19074;
			else
				displayID = ci->Male_DisplayID;
		}
		else if( p_caster->getRace() == RACE_DWARF ) 
		{
			if( ci->Male_DisplayID == 4587 )	//mana = blue
				displayID = 30755;
			else if( ci->Male_DisplayID == 4588 ) //earth = yellow, should be green
				displayID = 30753;
			else if( ci->Male_DisplayID == 4589 ) //searing = red
				displayID = 30754;
			else if( ci->Male_DisplayID == 4590 ) //windfury = white
				displayID = 30755;		//not confirmed
			else if( ci->Male_DisplayID == 4683 ) //fire nowa = red
				displayID = 30754;
			else
				displayID = ci->Male_DisplayID;
		}
		else
			displayID = ci->Male_DisplayID;
	}
	else
	{
		//orc special looks
		if( p_caster->getRace() == RACE_ORC )
		{
			if( ci->Male_DisplayID == 4587 )
				displayID = 30759;
			else if( ci->Male_DisplayID == 4588 )
				displayID = 30757;
			else if( ci->Male_DisplayID == 4589 )
				displayID = 30758;
			else if( ci->Male_DisplayID == 4590 )
				displayID = 30756;
			else
				displayID = ci->Male_DisplayID;
		}
		//troll special looks
		else if( p_caster->getRace() == RACE_TROLL )
		{
			if( ci->Male_DisplayID == 4587 )
				displayID = 30763;
			else if( ci->Male_DisplayID == 4588 )
				displayID = 30761;
			else if( ci->Male_DisplayID == 4589 )
				displayID = 30762;
			else if( ci->Male_DisplayID == 4590 )
				displayID = 30760;
			else
				displayID = ci->Male_DisplayID;
		}
		else if( p_caster->getRace() == RACE_GOBLIN )
		{
			if( ci->Male_DisplayID == 4587 )	//healing,mana light blue
				displayID = 30784;			
			else if( ci->Male_DisplayID == 4588 ) //earth = green
				displayID = 30782;
			else if( ci->Male_DisplayID == 4589 ) //searing = red
				displayID = 30783;
			else if( ci->Male_DisplayID == 4590 ) //windfury = blue
				displayID = 30781;
			else
				displayID = ci->Male_DisplayID;
		}		//rest
		else
			displayID = ci->Male_DisplayID;
	}

	// Set up the creature.
	pTotem->SetUInt32Value(OBJECT_FIELD_ENTRY, entry);
	pTotem->SetFloatValue(OBJECT_FIELD_SCALE_X, 1.0f);
	pTotem->SetUInt64Value(UNIT_FIELD_CREATEDBY, p_caster->GetGUID());
	pTotem->SetUInt32Value(UNIT_FIELD_HEALTH, damage);
	pTotem->SetUInt32Value(UNIT_FIELD_MAXHEALTH, damage);
	pTotem->SetPower( POWER_TYPE_FOCUS, p_caster->getLevel() * 30);
	pTotem->SetMaxPower(POWER_TYPE_FOCUS, p_caster->getLevel() * 30);
	pTotem->SetUInt32Value(UNIT_FIELD_LEVEL, p_caster->getLevel());
	pTotem->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, p_caster->GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE));
//	pTotem->SetUInt32Value(UNIT_FIELD_BYTES_0, (1 << 8) | (2 << 16) | (1 << 24)); //race + class...
	pTotem->SetUInt32Value(UNIT_FIELD_BYTES_0, (p_caster->GetUInt32Value( UNIT_FIELD_BYTES_0 ) & 0x00FFFFFF) | (1 << 24));
//	pTotem->SetUInt32Value(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED | UNIT_FLAG_SELF_RES);
	pTotem->SetUInt32Value(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED );
	pTotem->SetUInt32Value(UNIT_FIELD_BASEATTACKTIME, 2000);
	pTotem->SetUInt32Value(UNIT_FIELD_BASEATTACKTIME_1, 2000);
	pTotem->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 1.0f);
	pTotem->SetFloatValue(UNIT_FIELD_COMBATREACH, 1.0f);
	pTotem->SetUInt32Value(UNIT_FIELD_DISPLAYID, displayID);
	pTotem->SetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID, ci->Male_DisplayID); //blizzlike :P
	pTotem->SetFloatValue(UNIT_MOD_CAST_SPEED, 1.0f);
	pTotem->SetUInt32Value(UNIT_CREATED_BY_SPELL, GetProto()->Id);
	pTotem->SetUInt32Value(UNIT_FIELD_BYTES_2, 1 ); 
	//inherit pvp setting from owner
	if( p_caster && p_caster->IsPvPFlagged() )
		pTotem->SetPvPFlag();

	// Initialize faction stuff.
	pTotem->_setFaction();

	//added by Zack : Some shaman talents are casted on player but it should be inherited or something by totems
	pTotem->InheritSMMods(p_caster);

	// Totems get spell damage and healing bonus from the Shaman
	for(int school=0;school<7;school++)
	{
//		pTotem->ModDamageDone[school] = (int32)(p_caster->GetDamageDoneMod( PLAYER_FIELD_MOD_DAMAGE_DONE_POS + school ) - (int32)p_caster->GetUInt32Value( PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + school ));
//		pTotem->HealDoneMod[school] = p_caster->HealDoneMod[school];
		pTotem->ModDamageDone[school] = p_caster->GetDamageDoneMod( school );
	}
	if( p_caster && p_caster->m_APToSPExclusivePCT )
		pTotem->HealDoneMod = ( SPELL_POWER_TO_HEALING_POWER_CONVERSION_FACTOR + 0.20f ) * p_caster->GetAP() * p_caster->m_APToSPExclusivePCT / 100;
	else
		pTotem->HealDoneMod = p_caster->HealDoneMod;
	for(uint32 a = 0; a < 5; a++)
		pTotem->HealDoneMod += float2int32(p_caster->SpellHealDoneByAttribute[a] * (float)p_caster->GetUInt32Value(UNIT_FIELD_STAT0 + a));
	for( uint32 i=0;i<SCHOOL_COUNT;i++)
		pTotem->AoeDamageTakenPctMod[i] = 0;
//	float HasteModDmg = 1.0f / p_caster->GetSpellHaste();
//	pTotem->HealDoneMod = pTotem->HealDoneMod * HasteModDmg;
	pTotem->HealDonePctMod = p_caster->HealDonePctMod;
	// Setup complete. Add us to the world.
	pTotem->PushToWorld(m_caster->GetMapMgr());

	//maybe it is scripted ?
	if( TotemSpell )
	{
		// Set up AI, depending on our spells.
		uint32 j;
		for( j = 0; j < 3; ++j )
			if( TotemSpell->spells[0]->eff[j].Effect == SPELL_EFFECT_APPLY_AREA_AURA || TotemSpell->spells[0]->eff[j].Effect == SPELL_EFFECT_PERSISTENT_AREA_AURA || TotemSpell->spells[0]->eff[j].EffectApplyAuraName == SPELL_AURA_PERIODIC_TRIGGER_SPELL )
				break;
		if(j != 3)
		{
			// We're an area aura. Simple. Disable AI and cast the spell.
			pTotem->DisableAI();
			pTotem->GetAIInterface()->totemspell = TotemSpell;

			Spell * pSpell = SpellPool.PooledNew( __FILE__, __LINE__ );
			pSpell->Init(pTotem, TotemSpell->spells[0], true, 0);

			SpellCastTargets targets;
			targets.m_destX = pTotem->GetPositionX();
			targets.m_destY = pTotem->GetPositionY();
			targets.m_destZ = pTotem->GetPositionZ();
			targets.m_targetMask = TARGET_FLAG_DEST_LOCATION;

			pSpell->prepare(&targets);
		}
		else
		{
			// We're a casting totem. Switch AI on, and tell it to cast this spell.
			pTotem->EnableAI();
			pTotem->GetAIInterface()->Init(pTotem, AITYPE_TOTEM, MOVEMENTTYPE_NONE, p_caster);
			pTotem->GetAIInterface()->totemspell = TotemSpell;
//			int32 totemspelltimer = 3000, totemspelltime = 3000;	// need a proper resource for this.
			int32 totemspelltimer = 0, totemspelltime = 3000;	// need a proper resource for this.

			switch(TotemSpell->spells[0]->Id)
			{
			case 3606:
				totemspelltime = 1500;
				break;
			case 8167: //Poison Cleansing Totem
			case 8172: //Disease Cleansing Totem
			{
				//Zack : wtf we are overwriting values from DB ?
	//			if(TotemSpell->spells[0]->Id == 8167)
	//				TotemSpell = dbcSpell.LookupEntry( 8168 );	// Better to use this spell
	//			else
	//				TotemSpell = dbcSpell.LookupEntry( 8171 );
//				pTotem->GetAIInterface()->totemspell = TotemSpell;
				totemspelltime =  5000;
				totemspelltimer = 0; //First tick done immediately
				break;
			}
			case 8146: //Tremor Totem
			{
				totemspelltimer = 0; //First tick done immediately
				break;
			}
			case 8178: //Grounding Totem
			case 3600: //Earthbind Totem
			{
				totemspelltimer = 0; //First tick done immediately
				break;
			}
			case 8349: //Fire Nova Totem 1
			case 8502: //Fire Nova Totem 2
			case 8503: //Fire Nova Totem 3
			case 11306: //Fire Nova Totem 4
			case 11307: //Fire Nova Totem 5
			case 25535: //Fire Nova Totem 6
			case 25537: //Fire Nova Totem 7
			{
				totemspelltimer =  4000;
				// Improved Fire Totems
				if( p_caster->SM_Mods )
					SM_FIValue(p_caster->SM_Mods->SM_FDur, &totemspelltimer, TotemSpell->spells[0]->GetSpellGroupType());
				totemspelltime = totemspelltimer;
				break;
			}
			case 32982: // fire elemental totem
			case 33663: // earth elemental totem
				{
					totemspelltimer =  120000 * 10;
					totemspelltime = 0x7FFFFFFF;
				}break;
			case 98007: //Spirit Link Totem
				{
					totemspelltime =  1000; //every 1 second
					totemspelltimer = 0;
				}break;
			default:break;
			}

			pTotem->GetAIInterface()->spell_global_cooldown_stamp = getMSTime() + totemspelltimer;
			pTotem->GetAIInterface()->m_totemspelltime = totemspelltime;
		}

		//in case these are our elemental totems then we should set them up
//		if(GetProto()->Id==2062)
//			pTotem->GetAIInterface()->Event_Summon_EE_totem(GetDuration());
//		else if(GetProto()->Id==2894)
//			pTotem->GetAIInterface()->Event_Summon_FE_totem(GetDuration());

	//	pTotem->m_noRespawn = true;
	}

	// Set up the deletion event. The totem needs to expire after a certain time, or upon its death.
	sEventMgr.AddEvent(pTotem, &Creature::TotemExpire, EVENT_TOTEM_EXPIRE, GetDuration()+100, 1,0);
}

void Spell::SpellEffectSelfResurrect(uint32 i)
{
	if(!p_caster || !playerTarget)
	{ 
		return;
	}
	if(playerTarget->isAlive())
	{ 
		return;
	}
	uint32 mana;
	uint32 health;
	uint32 class_=unitTarget->getClass();
	
	switch(GetProto()->Id)
	{
	case 21169: //Reincarnation. Ressurect with 20% health and mana
		{
			int32 amt = 20;
			if( (GetProto()->c_is_flags & SPELL_FLAG_IS_REQUIRING_SM_MOD) && unitTarget->SM_Mods )
			{
				SM_FIValue(unitTarget->SM_Mods->SM_FMiscEffect,&amt,GetProto()->GetSpellGroupType());
				SM_PIValue(unitTarget->SM_Mods->SM_PMiscEffect,&amt,GetProto()->GetSpellGroupType());
			}
			health = uint32((unitTarget->GetUInt32Value(UNIT_FIELD_MAXHEALTH)*amt)/100);
			mana = uint32((unitTarget->GetMaxPower( POWER_TYPE_MANA )*amt)/100);
		}
		break;
	default:
		{
			if(damage < 0) 
			{ 
				health = -damage;
				mana = GetProto()->eff[i].EffectMiscValue;
			}
			else
			{ 
				health = uint32(unitTarget->GetUInt32Value(UNIT_FIELD_MAXHEALTH)*damage/100);
				mana = uint32(unitTarget->GetMaxPower( POWER_TYPE_MANA )*damage/100);
			}
		}break;
        }

	if(class_==WARRIOR||class_==ROGUE)
		mana=0;
	
	playerTarget->m_resurrectHealth = health;
	playerTarget->m_resurrectMana = mana;

	playerTarget->ResurrectPlayer();
	playerTarget->SetMovement(MOVE_UNROOT);

	playerTarget->SetUInt32Value(PLAYER_SELF_RES_SPELL, 0);

	if(GetProto()->Id==21169)
		AddCooldown();
}

void Spell::SpellEffectSkinning(uint32 i)
{
	if( unitTarget == NULL || unitTarget->IsPlayer() )
	{ 
		return;
	}

	uint32 sk = SafePlayerCast( m_caster )->_GetSkillLineCurrent( SKILL_SKINNING );
	uint32 lvl = unitTarget->getLevel();

	if( ( sk >= lvl * 5 ) || ( ( sk + 100 ) >= lvl * 10 ) )
	{
		//Fill loot for Skinning
//		lootmgr.FillSkinningLoot(&(SafeCreatureCast(unitTarget))->loot,unitTarget->GetEntry());
		if( (SafeCreatureCast(unitTarget))->DificultyProto && (SafeCreatureCast(unitTarget))->DificultyProto->loot_skin )
		{
			//dump normal loot
			SafeCreatureCast(unitTarget)->loot.items.clear();	
			SafeCreatureCast(unitTarget)->loot.gold = 0;
			//fill creature loot
			lootmgr.FillObjectLoot(unitTarget,SafeCreatureCast(unitTarget)->DificultyProto->loot_skin);
		}
		SafePlayerCast( m_caster )->SendLoot( unitTarget->GetGUID(), LOOT_SKINNING );
		
		//Not skinable again
		unitTarget->BuildFieldUpdatePacket( p_caster, UNIT_FIELD_FLAGS, 0 );

		//still lootable
		//pkt=unitTarget->BuildFieldUpdatePacket(UNIT_DYNAMIC_FLAGS,U_DYN_FLAG_LOOTABLE);
		//SafePlayerCast( m_caster )->GetSession()->SendPacket(pkt);
		//delete pkt;
		if (!SafeCreatureCast(unitTarget)->Skinned)
			DetermineSkillUp(SKILL_SKINNING,1,sk<lvl*5?sk/5:lvl);

		SafeCreatureCast(unitTarget)->Skinned = true;
	}
	else
	{
		SendCastResult(SPELL_FAILED_TARGET_UNSKINNABLE);
	}   
			
//	DetermineSkillUp(SKILL_SKINNING,unitTarget->getLevel());
}

void Spell::SpellEffectCharge(uint32 i)
{
	if(!unitTarget || !u_caster)
	{ 
		return;
	}
	//if(!p_caster) who said units can't charge? :P
	//	return;
	if(!unitTarget->isAlive())
	{ 
		return;
	}
	if (u_caster->IsStunned() || u_caster->m_rooted || u_caster->IsPacified() || u_caster->IsFeared())
	{ 
		return;
	}

	float x, y, z;
	float dx,dy;

	//if(unitTarget->GetTypeId() == TYPEID_UNIT)
	//	if(unitTarget->GetAIInterface())
	//		unitTarget->GetAIInterface()->StopMovement(5000);
	if(unitTarget->GetPositionX() == 0.0f || unitTarget->GetPositionY() == 0.0f)
	{ 
		return;
	}
	
	dx=unitTarget->GetPositionX()-m_caster->GetPositionX();
	dy=unitTarget->GetPositionY()-m_caster->GetPositionY();
	if(dx == 0.0f || dy == 0.0f)
	{ 
		return;
	}

	float d = sqrt(dx*dx+dy*dy)-unitTarget->GetFloatValue(UNIT_FIELD_BOUNDINGRADIUS)-m_caster->GetFloatValue(UNIT_FIELD_BOUNDINGRADIUS);
	float alpha = atanf(dy/dx);
	if(dx<0)
		alpha += float(M_PI);

	x = d*cosf(alpha)+m_caster->GetPositionX();
	y = d*sinf(alpha)+m_caster->GetPositionY();
	z = unitTarget->GetPositionZ();

//	uint32 time = uint32( (u_caster->CalcDistance(unitTarget) / ((u_caster->m_runSpeed * 3.5) * 0.001f)) + 0.5);
	uint32 time = uint32( (u_caster->CalcDistance(unitTarget) / ((u_caster->m_base_runSpeed * 3.5) * 0.001f)) + 0.5);
	time = time / 2; //on blizz charge is really fast. Maybe spell has some speed coeff hidden somewhere ?

	if( u_caster && u_caster->GetAIInterface() )
		u_caster->GetAIInterface()->SendMoveToPacket(x, y, z, alpha, time, MONSTER_MOVE_FLAG_RUN);
/*	WorldPacket data(SMSG_MONSTER_MOVE, 50);
	data << m_caster->GetNewGUID();
	data << uint8(0);
	data << m_caster->GetPositionX();
	data << m_caster->GetPositionY();
	data << m_caster->GetPositionZ();
	data << getMSTime();
	data << uint8(0x00);
	data << uint32(MONSTER_MOVE_FLAG_RUN);
	data << time;
	data << uint32(1);
	data << x << y << z;*/
	if(unitTarget->GetTypeId() == TYPEID_UNIT)
		unitTarget->GetAIInterface()->StopMovement(2000);

//	u_caster->SendMessageToSet(&data, true);   
	
	u_caster->SetPosition(x,y,z,alpha,true);
	u_caster->addStateFlag(UF_ATTACKING);
	u_caster ->smsg_AttackStart( unitTarget );
	u_caster->setAttackTimer(time, false);
	u_caster->setAttackTimer(time, true);

	// trigger an event to reset speedhack detection
	if( p_caster )
	{
		p_caster->EventAttackStart();
		p_caster->SpeedCheatDelay( time * 1000 + MIN( 5000, p_caster->GetSession()->GetLatency() ) + 2000 );
		p_caster->z_axisposition = 0.0f;
	}
}

void Spell::SpellEffectPlayerPull( uint32 i )
{
	if( unitTarget == NULL || !unitTarget->isAlive() || !unitTarget->IsPlayer() )
	{ 
		return;
	}

	Player* p_target = SafePlayerCast( unitTarget );

	// calculate destination
	float pullD = p_target->CalcDistance( m_caster ) - p_target->GetFloatValue( UNIT_FIELD_BOUNDINGRADIUS ) - m_caster->GetFloatValue( UNIT_FIELD_BOUNDINGRADIUS ) - 1.0f;
	float pullO = p_target->calcRadAngle( p_target->GetPositionX(), p_target->GetPositionY(), m_caster->GetPositionX(), m_caster->GetPositionY() );
	float pullX = p_target->GetPositionX() + pullD * cosf( pullO );
	float pullY = p_target->GetPositionY() + pullD * sinf( pullO );
	float pullZ = m_caster->GetPositionZ() + 0.3f;
	uint32 time = uint32( pullD * 42.0f );

	p_target->SetOrientation( pullO );

	WorldPacket data( SMSG_MONSTER_MOVE, 60 );
	data << p_target->GetNewGUID();
	data << uint8(0);
	data << p_target->GetPositionX() << p_target->GetPositionY() << p_target->GetPositionZ();
	data << getMSTime();
	data << uint8( 4 );
	data << pullO;
//	data << uint32( MONSTER_MOVE_FLAG_TELEPORT );
	data << uint32( 0 );
	data << time;
	data << uint32( 1 );
	data << pullX << pullY << pullZ;

	p_target->SendMessageToSet( &data, true );   
}

void Spell::SpellEffectSummonCritter(uint32 i)
{
	if(!u_caster || u_caster->IsInWorld() == false)
	{ 
		return;
	}

	uint32 SummonCritterID;
	if( forced_miscvalues[i] != 0 )
		SummonCritterID = forced_miscvalues[i];
	else
		SummonCritterID = GetProto()->eff[i].EffectMiscValue;

	// GetProto()->EffectDieSides[i] has something to do with dismissing our critter
	// when it is 1, it means to just dismiss it if we already have it
	// when it is 0, it could mean to always summon a new critter, but there seems to be exceptions

	if(u_caster->critterPet)
	{
		// if we already have this critter, we will just dismiss it and return
		if(u_caster->critterPet->GetCreatureInfo() && u_caster->critterPet->GetCreatureInfo()->Id == SummonCritterID)
		{
			u_caster->critterPet->RemoveFromWorld(false,true);
			sGarbageCollection.AddObject( u_caster->critterPet );
			u_caster->critterPet = NULL;
			return;
		}
		// this is a different critter, so we will dismiss our current critter and then go on to summon the new one
		else
		{
			u_caster->critterPet->RemoveFromWorld(false,true);
			sGarbageCollection.AddObject( u_caster->critterPet );
			u_caster->critterPet = NULL;
		}
	}

	if(!SummonCritterID) 
	{ 
		return;
	}

	CreatureInfo * ci = CreatureNameStorage.LookupEntry(SummonCritterID);
	CreatureProto * cp = CreatureProtoStorage.LookupEntry(SummonCritterID);

	if(!ci || !cp) 
	{ 
		return;
	}

	Creature * pCreature = u_caster->GetMapMgr()->CreateCreature(SummonCritterID);
	pCreature->SetInstanceID(u_caster->GetMapMgr()->GetInstanceID());
	pCreature->Load(cp, m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ());
	pCreature->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, 35);
	pCreature->_setFaction();
	pCreature->SetUInt32Value(UNIT_FIELD_LEVEL, 1);
	pCreature->GetAIInterface()->Init(pCreature,AITYPE_PET,MOVEMENTTYPE_NONE,u_caster);
	pCreature->SetFlag( UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED );
	pCreature->GetAIInterface()->SetUnitToFollow(u_caster);
	pCreature->GetAIInterface()->SetUnitToFollowAngle( float(-(M_PI/2)) );
//	pCreature->GetAIInterface()->SetFollowDistance( GetRadius(GetProto()->eff[i].EffectRadiusIndex) );
	pCreature->GetAIInterface()->disable_melee = true;
	pCreature->bInvincible = true;
//	pCreature->m_noRespawn = true;
	pCreature->PushToWorld(u_caster->GetMapMgr());
	u_caster->critterPet = pCreature;

	if(u_caster->IsPvPFlagged())
		pCreature->SetPvPFlag();
	if(u_caster->IsFFAPvPFlagged())
		pCreature->SetFFAPvPFlag();
}

void Spell::SpellEffectKnockBack(uint32 i)
{
	if(unitTarget == NULL || !unitTarget->isAlive() || !m_caster)
	{ 
		return;
	}

	//interrupt spell cast of the target
	unitTarget->InterruptSpell();

	//check target effect immunity
	float dx, dy;
	float verticalSpeed = float(GetProto()->eff[i].EffectBasePoints+1)/10.0f;
	float radius = float(GetProto()->eff[i].EffectMiscValue)/10.0f;

	float orientation;
	if( m_caster == unitTarget )
		orientation = m_caster->GetOrientation()+ 3.14f; //jump backward ?
	else
	{
		float spx = m_caster->GetPositionX();
		float spy = m_caster->GetPositionY();
		if( p_caster || m_caster->IsPet() )
		{
			if( m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION )
			{
				spx = m_targets.m_destX;
				spy = m_targets.m_destY;
			}
			else if( m_targets.m_targetMask & TARGET_FLAG_SOURCE_LOCATION )
			{
				spx = m_targets.m_srcX;
				spy = m_targets.m_srcY;
			}
		}
		orientation = m_caster->calcRadAngle( spx, spy, unitTarget->GetPositionX(),unitTarget->GetPositionY() );
	}

	dx = sinf(orientation);
	dy = cosf(orientation);

	//there must be a flag to signal if it is a forward or backward jump :(
	if( GetProto()->NameHash == SPELL_HASH_ROCKET_JUMP )
	{
		dx = -dx;
		dy = -dy;
	}

	if( unitTarget->IsPlayer() == true )
	{
		playerTarget->SendKnockbackPacket( dy, dx, radius, -verticalSpeed );
//		playerTarget->SendKnockbackPacket( dy, dx, -verticalSpeed, radius );
		playerTarget->blinked = true;
		playerTarget->SpeedCheatDelay( MAX(float2int32( radius * 1000 ),5000) + MIN( 5000, playerTarget->GetSession()->GetLatency() ) + 2000 );
	}
	else
	{
		float aprox_dist = radius / 10.0f;
		float new_x = unitTarget->GetPositionX()+ aprox_dist*dx;
		float new_y = unitTarget->GetPositionY()+ aprox_dist*dy;
		float new_z = unitTarget->GetPositionZ()+2.0f;
		float land_z = unitTarget->GetMapMgr()->GetLandHeight( new_x, new_y, new_z );
		if( land_z == VMAP_VALUE_NOT_INITIALIZED )
			new_z = land_z;
//		unitTarget->SendMessageToSet( &data, true );
		unitTarget->SetPosition( new_x, new_y, new_z, unitTarget->GetOrientation() );
		unitTarget->GetAIInterface()->StopMovement(0);	//aaah crap, how do i do the kickback for mobs ?
	}
}

void Spell::SpellEffectDisenchant( uint32 i )
{
	if( p_caster == NULL )
	{ 
		return;
	}

	Item* it = p_caster->GetItemInterface()->GetItemByGUID( m_targets.m_itemTarget );
	if( it == NULL )
	{
		SendCastResult( SPELL_FAILED_CANT_BE_DISENCHANTED );
		return;
	}

	//Fill disenchanting loot
	p_caster->SetLootGUID( it->GetGUID() );
	if( !it->loot )
	{
		it->loot = new Loot;
		lootmgr.FillItemLoot( it->loot, it->GetEntry() );
		p_caster->Event_AchiementCriteria_Received( ACHIEVEMENT_CRITERIA_TYPE_LOOT_TYPE,4,ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
	}

	Log.Debug( "SpellEffect", "Succesfully disenchanted item %d", uint32( itemTarget->GetEntry() ) );
	p_caster->SendLoot( it->GetGUID(), LOOT_DISENCHANTING );

	//We can increase Enchanting skill up to 60 
	int32 skill = p_caster->_GetSkillLineCurrent( SKILL_ENCHANTING );
	if( skill && skill < 60 )
	{
		if( RandChance( 100 - skill * 75 / 100 ) )
		{
			uint32 base_points = 1;
			if( GetProto()->spell_skill )
				base_points = GetProto()->spell_skill->skill_point_advance;
			uint32 SkillUp = float2int32( base_points * sWorld.getRate( RATE_SKILLRATE ) );
			if( skill + SkillUp > 60 )
				SkillUp = 60 - skill;

			p_caster->_AdvanceSkillLine( SKILL_ENCHANTING, SkillUp );
		}
	}
	if( it == i_caster )
		i_caster = NULL;
}

void Spell::SpellEffectInebriate(uint32 i) // lets get drunk!
{
	if(!playerTarget)
	{ 
		return;
	}

	uint16 currentDrunk = playerTarget->GetDrunkValue();
	uint16 drunkMod = (uint16)(damage) * 256;
	if( currentDrunk + drunkMod > 0xFFFE )
		currentDrunk = 0xFFFE;
	else
		currentDrunk += drunkMod;
	playerTarget->SetDrunkValue( currentDrunk, i_caster ? i_caster->GetEntry() : 0 );

	sEventMgr.RemoveEvents(playerTarget, EVENT_PLAYER_REDUCEDRUNK);
	sEventMgr.AddEvent(playerTarget, &Player::EventReduceDrunk, false, EVENT_PLAYER_REDUCEDRUNK, 10000, 0,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
}

void Spell::SpellEffectFeedPet(uint32 i)  // Feed Pet
{
	// food flags and food level are checked in Spell::CanCast()
	if(!itemTarget || !p_caster)
	{ 
		return;
	}
	
	Pet *pPet = p_caster->GetSummon();
	if(!pPet)
	{ 
		return;
	}

	/**	Cast feed pet effect
	- effect is item level and pet level dependent, aura ticks are 35, 17, 8 (*1000) happiness
	- http://petopia.brashendeavors.net/html/articles/basics_feeding.shtml */
	int8 deltaLvl = (int8)(pPet->getLevel() - itemTarget->GetProto()->ItemLevel);
	damage /= 1000; //damage of Feed pet spell is 35000
	if(deltaLvl > 10) damage = damage >> 1;//divide by 2
	if(deltaLvl > 20) damage = damage >> 1;
	damage *= 1000;

	SpellEntry *spellInfo = dbcSpell.LookupEntry(GetProto()->eff[i].EffectTriggerSpell);
	Spell *sp= SpellPool.PooledNew( __FILE__, __LINE__ );
	sp->Init(p_caster,spellInfo,true,NULL);
	sp->forced_basepoints[0] = damage;
	SpellCastTargets tgt;
	tgt.m_unitTarget=pPet->GetGUID();
	sp->prepare(&tgt);

	if(itemTarget->GetUInt32Value(ITEM_FIELD_STACK_COUNT)>1)
	{
		itemTarget->ModUnsigned32Value(ITEM_FIELD_STACK_COUNT, -1);
		itemTarget->m_isDirty=true;
	}
	else
	{
		p_caster->GetItemInterface()->SafeFullRemoveItemByGuid(itemTarget->GetGUID());
		itemTarget=NULL;
	}
}

void Spell::SpellEffectRedirectThreat(uint32 i)
{
	if (!p_caster || !unitTarget)
	{ 
		return;
	}

	if ((unitTarget->IsPlayer() && p_caster->GetGroup() != SafePlayerCast(unitTarget)->GetGroup()) || (unitTarget->IsCreature() && !unitTarget->IsPet()))
	{ 
		return;
	}

	p_caster->SetMisdirectionTarget(unitTarget->GetGUID());
	uint32 maxduration = GetDuration();
	//there was an exploit for people setting this to dead player then mobs would get stuck
	if( maxduration > 20000 )
		maxduration = 20000;
	if( maxduration < 4000 )
		maxduration = 4000;	//misdirect next attack, at least try
	sEventMgr.AddEvent(p_caster,&Player::SetMisdirectionTarget,(uint64)0,EVENT_RESET_MISSDIRECTIONTARGET,maxduration,1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
	
}

void Spell::SpellEffectReduceThreatPercent(uint32 i)
{
	if (!unitTarget || !unitTarget->IsCreature() || !u_caster || unitTarget->GetAIInterface()->getThreatByPtr(u_caster) == 0)
	{ 
		return;
	}
	unitTarget->GetAIInterface()->modThreatByPtr(u_caster, (int32)unitTarget->GetAIInterface()->getThreatByPtr(u_caster) * damage / 100);
}

void Spell::SpellEffectReputation(uint32 i)
{
	if(!playerTarget)
	{ 
		return;
	}

	//playerTarget->modReputation(GetProto()->eff[i].EffectMiscValue, damage, true);
	playerTarget->ModStanding(GetProto()->eff[i].EffectMiscValue, damage);
}

void Spell::SpellEffectSummonObjectSlot(uint32 i)
{
	if(!u_caster || !u_caster->IsInWorld())
	{ 
		return;
	}

	GameObject *GoSummon = NULL;

	uint32 slot=GetProto()->eff[i].Effect - SPELL_EFFECT_SUMMON_OBJECT_SLOT1;
	GoSummon = u_caster->m_ObjectSlots[slot] ? u_caster->GetMapMgr()->GetGameObject(u_caster->m_ObjectSlots[slot]) : 0;
	u_caster->m_ObjectSlots[slot] = 0;
	
	if( GoSummon )
	{
		if(GoSummon->GetInstanceID() != u_caster->GetInstanceID())
			GoSummon->ExpireAndDelete();
		else
		{
			if( GoSummon->IsInWorld() )
				GoSummon->RemoveFromWorld(true);
			sGarbageCollection.AddObject( GoSummon );
			GoSummon = NULL;
		}
	}


	// spawn a new one
	float spx,spy,spz,spo;
	uint32 entry;
	if( forced_miscvalues[i] != 0 )
		entry = forced_miscvalues[i];
	else
		entry = GetProto()->eff[i].EffectMiscValue;

	if( m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION )
	{
		spx = m_targets.m_destX;
		spy = m_targets.m_destY;
		spz = m_targets.m_destZ;
		spo = m_caster->GetOrientation();
	}
	else if( m_targets.m_targetMask & TARGET_FLAG_SOURCE_LOCATION )
	{
		spx = m_targets.m_srcX;
		spy = m_targets.m_srcY;
		spz = m_targets.m_srcZ;
		spo = m_caster->GetOrientation();
	}
	else if( unitTarget != NULL && ( GetProto()->c_is_flags & SPELL_FLAG_IS_SUMMONING_AT_TARGET ) )
	{
		spx = unitTarget->GetPositionX();
		spy = unitTarget->GetPositionY();
		spz = unitTarget->GetPositionZ();
		spo = unitTarget->GetOrientation();
	}
	else
	{
		spx = m_caster->GetPositionX();
		spy = m_caster->GetPositionY();
		spz = m_caster->GetPositionZ();
		spo = m_caster->GetOrientation();
	}
	int32 duration = GetDuration();
	if( duration <= 0 || duration > 0x00FFFFFF )
		duration = 30000;	//30 seconds for imba durations ?

	//overwrite the entry and the orientation
	if( GetProto()->NameHash == SPELL_HASH_SURVEY && p_caster )
		entry = p_caster->GetSurveyBotEntry( spx,spy,spz,spo, duration );

	GoSummon = u_caster->GetMapMgr()->CreateGameObject( entry );
	if( !GoSummon->CreateFromProto( entry, m_caster->GetMapId(), spx, spy, spz, spo ) )
	{
		sGarbageCollection.AddObject( GoSummon );
		GoSummon = NULL;
		return;
	}
	
	GoSummon->SetUInt32Value(GAMEOBJECT_LEVEL, u_caster->getLevel());
	GoSummon->SetUInt64Value(OBJECT_FIELD_CREATED_BY, m_caster->GetGUID()); 
	GoSummon->SetInstanceID(m_caster->GetInstanceID());

	if(GoSummon->GetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_TYPEID) == GAMEOBJECT_TYPE_TRAP)
	{
		GoSummon->SetUInt64Value(OBJECT_FIELD_CREATED_BY, u_caster->GetGUID());
		GoSummon->invisible = true;
		GoSummon->invisibilityFlag = INVIS_FLAG_TRAP;
		GoSummon->charges = 1;
		GoSummon->checkrate = 5;	//this is in update intervals. avg 50 ms, * 2 = 1 second
		int32 Radius = GoSummon->GetInfo()->sound2;
		if( Radius <= 0 )
			Radius = 10;
		GoSummon->rangeSQ = float( Radius * Radius );
		if( u_caster && u_caster->CombatStatus.IsInCombat() )
			GoSummon->SetTrapArmDuration( 1000 );
//		sEventMgr.AddEvent(GoSummon, &GameObject::TrapSearchTarget, EVENT_GAMEOBJECT_TRAP_SEARCH_TARGET, 500, 0,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
	}
	else
	{
		sEventMgr.AddEvent(GoSummon, &GameObject::ExpireAndDelete, EVENT_GAMEOBJECT_EXPIRE, duration, 1,0);
	}
	GoSummon->PushToWorld(m_caster->GetMapMgr());
	GoSummon->SetSummoned(u_caster);
	u_caster->m_ObjectSlots[slot] = GoSummon->GetGUID();
	
	if( p_caster && p_caster->GetSession() )
	{
/*
14333
{SERVER} Packet: (0x7237) UNKNOWN PacketSize = 17 TimeStamp = 7873916
01 0F AA 03 00 F2 16 30 F1 E0 93 04 00 8B 1F 00 00 
*/
		//no idea where is used. Maybe it triggers some Addon functionality
		sStackWorldPacket(data,SMSG_TOTEM_CREATED,40);
		data << uint8(slot);
		data << GoSummon->GetGUID();
		data << uint32( duration );
		data << uint32( GetProto()->Id );
		p_caster->GetSession()->SendPacket( &data );
	}
}

void Spell::SpellEffectDispelMechanic(uint32 i)
{
	if( !unitTarget || !unitTarget->isAlive() )
	{ 
		return;
	}
	/* this was already working before...	
	uint32 sMisc = GetProto()->eff[i].EffectMiscValue;

	for( uint32 x = 0 ; x<MAX_AURAS ; x++ )
	{
		if( unitTarget->m_auras[x] && !unitTarget->m_auras[x]->IsPositive())
		{
			if( unitTarget->m_auras[x]->GetSpellProto()->MechanicsType == sMisc )
				unitTarget->m_auras[x]->Remove();
		}
	}
	*/
	uint32 our_eff_dispel_type;
	if( forced_miscvalues[i] != 0 )
		our_eff_dispel_type = forced_miscvalues[i];
	else
		our_eff_dispel_type = GetProto()->eff[i].EffectMiscValue;
//	unitTarget->RemoveAllAurasByMechanic( our_eff_dispel_type, GetProto()->eff[i].EffectBasePoints , false );
	unitTarget->RemoveAllAurasByMechanic( our_eff_dispel_type, damage , false );	//for SMT mod cases

	/*Shady: if it's about Daze spell - dismount should be done by RemoveAllAurasByMechanic.
	We don't need useless code or hackfixes here, so commented.*/
	//if( playerTarget && GetProto()->NameHash == SPELL_HASH_DAZED && playerTarget->IsMounted() )
	//	playerTarget->RemoveAura(playerTarget->m_MountSpellId);
}

void Spell::SpellEffectSummonDeadPet(uint32 i)
{//this is pet resurrect
	Pet *pPet;
	if( p_caster != NULL ) 
		pPet = p_caster->GetSummon();
	else if( u_caster && u_caster->IsPet() )
		pPet = SafePetCast( u_caster );
	if(pPet)
	{
		if( (GetProto()->c_is_flags & SPELL_FLAG_IS_REQUIRING_SM_MOD) && p_caster->SM_Mods )
		{
			SM_FIValue(p_caster->SM_Mods->SM_FMiscEffect,&damage,GetProto()->GetSpellGroupType());
			SM_PIValue(p_caster->SM_Mods->SM_PMiscEffect,&damage,GetProto()->GetSpellGroupType());
		}

		pPet->SetUInt32Value( UNIT_DYNAMIC_FLAGS, 0 );
		pPet->SetUInt32Value( UNIT_FIELD_HEALTH, ( uint32 )( ( pPet->GetUInt32Value(UNIT_FIELD_MAXHEALTH) * damage ) / 100 ));
		pPet->setDeathState( ALIVE );
		pPet->GetAIInterface()->HandleEvent( EVENT_FOLLOWOWNER, pPet, 0 );
		sEventMgr.RemoveEvents( pPet, EVENT_PET_DELAYED_REMOVE );
		pPet->SendSpellsToOwner();
 	}
}

/* This effect has 2 functions
 * 1. It delete's all current totems from the player
 * 2. It returns a percentage of the mana back to the player
 *
 * Bur kick my ass if this is not safe:P
*/

void Spell::SpellEffectDestroyAllTotems(uint32 i)
{
	if(!p_caster || !p_caster->IsInWorld()) 
	{ 
		return;
	}

	float RetreivedMana = 0.0f;
	for(uint32 x=0;x<4;x++)
	{
		// atm totems are considert creature's
		if(p_caster->m_TotemSlots[x])
		{
			uint32 SpellID = p_caster->m_TotemSlots[x]->GetUInt32Value(UNIT_CREATED_BY_SPELL);
			SpellEntry * sp = dbcSpell.LookupEntry(SpellID);
			if (!sp)
				continue;

			float pts = float(GetProto()->eff[i].EffectBasePoints+1) / 100.0f;
			RetreivedMana += float(sp->PowerEntry.manaCost) * pts;
			RetreivedMana += float(sp->PowerEntry.ManaCostPercentage) * pts * p_caster->GetBaseMana() / 100.0f;

			p_caster->m_TotemSlots[x]->TotemExpire();
		}

		if(p_caster->m_ObjectSlots[x])
		{
			GameObject * obj = p_caster->GetMapMgr()->GetGameObject(p_caster->m_ObjectSlots[x]);
			if(obj)
			{
				obj->ExpireAndDelete();
			}
			p_caster->m_ObjectSlots[x] = 0;
		}
	}

	p_caster->Energize( p_caster, GetProto()->Id, uint32( RetreivedMana ), POWER_TYPE_MANA, 0 );
}

void Spell::SpellEffectSummonDemon(uint32 i)
{
	if(!p_caster/* ||  p_caster->getClass() != WARLOCK */) //summoning a demon shouldn't be warlock only, see spells 25005, 24934, 24810 etc etc
	{ 
		return;
	}
	Pet *pPet = p_caster->GetSummon();
	if(pPet)
	{
		pPet->Dismiss();
	}

	uint32 CreatureEntry;
	if( forced_miscvalues[i] != 0 )
		CreatureEntry = forced_miscvalues[i];
	else
		CreatureEntry = GetProto()->eff[i].EffectMiscValue;
	CreatureInfo *ci = CreatureNameStorage.LookupEntry(CreatureEntry);
	if(ci)
	{
		LocationVector *vec = NULL;
		if( m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION && m_targets.m_destX && m_targets.m_destY && m_targets.m_destZ )
		{
			vec = new LocationVector(m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ);
		}
		pPet = objmgr.CreatePet( CreatureEntry, p_caster->GeneratePetNumber( CreatureEntry ) );
		pPet->SetInstanceID(p_caster->GetInstanceID());
		pPet->CreateAsSummon( CreatureEntry, ci, NULL, p_caster, GetProto(), 1, 300000, vec);
		if (vec) 
		{
			delete vec;
			vec = NULL;
		}
	}
	//Create Enslave Aura if its inferno spell
	if(GetProto()->Id == 1122 && pPet )
	{
		SpellEntry *spellInfo = dbcSpell.LookupEntry(11726);
		
		Spell *sp=SpellPool.PooledNew( __FILE__, __LINE__ );
		sp->Init(pPet,spellInfo,true,NULL);
		SpellCastTargets tgt;
		tgt.m_unitTarget=pPet->GetGUID();
		sp->prepare(&tgt);
	}
}

void Spell::SpellEffectResurrect(uint32 i) // Resurrect (Flat)
{
	if(!playerTarget)
	{
		if(!corpseTarget)
		{
			// unit resurrection handler
			if(unitTarget)
			{
				if(unitTarget->GetTypeId()==TYPEID_UNIT && unitTarget->IsPet() && unitTarget->IsDead())
				{
					uint32 hlth = ((uint32)GetProto()->eff[i].EffectBasePoints > unitTarget->GetUInt32Value(UNIT_FIELD_MAXHEALTH)) ? unitTarget->GetUInt32Value(UNIT_FIELD_MAXHEALTH) : (uint32)GetProto()->eff[i].EffectBasePoints;
					uint32 mana = ((uint32)GetProto()->eff[i].EffectBasePoints > unitTarget->GetMaxPower( POWER_TYPE_MANA )) ? unitTarget->GetMaxPower( POWER_TYPE_MANA ) : (uint32)GetProto()->eff[i].EffectBasePoints;

					if(!unitTarget->IsPet())
					{
						sEventMgr.RemoveEvents(unitTarget, EVENT_CREATURE_REMOVE_CORPSE);
					}
					else
					{
						sEventMgr.RemoveEvents(unitTarget, EVENT_PET_DELAYED_REMOVE);
						sEventMgr.RemoveEvents(unitTarget, EVENT_CREATURE_REMOVE_CORPSE);
					}
					unitTarget->SetUInt32Value(UNIT_FIELD_HEALTH, hlth);
					unitTarget->SetPower( POWER_TYPE_MANA, mana);
					unitTarget->SetUInt32Value(UNIT_DYNAMIC_FLAGS, 0);
					unitTarget->setDeathState(ALIVE);
//					SafeCreatureCast(unitTarget)->Tagged=false;
					SafeCreatureCast(unitTarget)->TaggerGroupId=0;
					SafeCreatureCast(unitTarget)->TaggerGuid=0;
					SafeCreatureCast(unitTarget)->loot.gold=0;
					SafeCreatureCast(unitTarget)->loot.looters.clear();
					//clear roll info for items
					SafeCreatureCast(unitTarget)->loot.items.clear();
					SafeCreatureCast(unitTarget)->loot.currencies.clear();
				}
			}

			return;
		}
		playerTarget = objmgr.GetPlayer(corpseTarget->GetUInt32Value(CORPSE_FIELD_OWNER));
		if(!playerTarget) 
		{ 
			return;
		}
	}

	if(playerTarget->isAlive() || !playerTarget->IsInWorld())
	{ 
		return;
	}

	uint32 health = damage * playerTarget->GetBaseHealth();
	uint32 mana;
	if( GetProto()->eff[i].EffectMiscValue == 0 )
		mana = damage * playerTarget->GetBaseMana();
	else
		mana = GetProto()->eff[i].EffectMiscValue * playerTarget->GetBaseMana();
	
	playerTarget->m_resurrectHealth = health;
	playerTarget->m_resurrectMana = mana;

	SendResurrectRequest(playerTarget);   
	playerTarget->SetMovement(MOVE_UNROOT);
}

void Spell::SpellEffectAttackMe(uint32 i)
{
	if( unitTarget == NULL || unitTarget->isAlive() == false || unitTarget->IsPlayer() == true )
	{ 
		return;
	}

	int32 threat_to_most_hated = (int32)unitTarget->GetAIInterface()->getThreatByPtr( unitTarget->GetAIInterface()->GetMostHated() );
	int32 threat_to_us = (int32)unitTarget->GetAIInterface()->getThreatByPtr( u_caster );
	int32 threat_dif = threat_to_most_hated - threat_to_us;
	if(threat_dif>0)//maybe we are already the most hated ?
		unitTarget->GetAIInterface()->modThreatByPtr( u_caster, threat_dif + 1000 );

	unitTarget->GetAIInterface()->AttackReaction( u_caster, damage, 0 );	
}

void Spell::SpellEffectSkinPlayerCorpse(uint32 i)
{
	Corpse * corpse = 0;
	if(!playerTarget)
	{
		// means we're "skinning" a corpse
		corpse = objmgr.GetCorpse((uint32)m_targets.m_unitTarget);  // hacky
	}
	else if(playerTarget->getDeathState() == CORPSE)	// repopped while we were casting 
	{
		corpse = objmgr.GetCorpse(playerTarget->GetLowGUID());
	}

	if(!m_caster->IsPlayer()) 
	{ 
		return;
	}
 
	if(playerTarget && !corpse)
	{
		if(!playerTarget->m_bg || !playerTarget->IsDead())
		{ 
			return;
		}

		// Set all the lootable stuff on the player. If he repops before we've looted, we'll set the flags
		// on corpse then :p

		playerTarget->bShouldHaveLootableOnCorpse = false;
		playerTarget->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);
		playerTarget->SetFlag(UNIT_DYNAMIC_FLAGS, U_DYN_FLAG_LOOTABLE);

		// Send the loot.
		p_caster->SendLoot(playerTarget->GetGUID(), LOOT_SKINNING);

		// Send a message to the died player, telling him he has to resurrect at the graveyard.
		// Send an empty corpse location too, :P
		
		playerTarget->GetSession()->OutPacket(SMSG_PLAYER_SKINNED, 1, "\x00");
		playerTarget->GetSession()->OutPacket(MSG_CORPSE_QUERY, 1, "\x00");

		// don't allow him to spawn a corpse
		playerTarget->bCorpseCreateable = false;

		// and.. force him to the graveyard and repop him.
		playerTarget->SetDeathStateCorpse();

	}else if(corpse)
	{
		// find the corpses' owner
		Player * owner = objmgr.GetPlayer(corpse->GetUInt32Value(CORPSE_FIELD_OWNER));
		if(owner)
		{
			if(owner->m_bg == NULL)
			{ 
				return;
			}

			owner->GetSession()->OutPacket(SMSG_PLAYER_SKINNED, 1, "\x00");
			owner->GetSession()->OutPacket(MSG_CORPSE_QUERY, 1, "\x00");
		}

		if(corpse->GetUInt32Value(CORPSE_FIELD_DYNAMIC_FLAGS) != 1)
			corpse->SetUInt32Value(CORPSE_FIELD_DYNAMIC_FLAGS, 1); // sets it so you can loot the plyr
		
		// remove skinnable flag
		corpse->SetUInt32Value(CORPSE_FIELD_FLAGS, 5);

		// remove owner association
		corpse->SpawnBones();

		// send loot
		p_caster->SendLoot(corpse->GetGUID(), LOOT_SKINNING);
	}
}

void Spell::SpellEffectSkill(uint32 i)
{
	// Used by professions only
	// Effect should be renamed in RequireSkill

	if ( !p_caster || p_caster->_GetSkillLineMax(GetProto()->eff[i].EffectMiscValue) >= uint32( damage * 75 ) )
	{ 
		return;
	}
	
	//no multi casts in case of playercreate spell
	//zack:maybe some spells are required by client. Ex : language
//	p_caster->removeSpell( GetProto()->Id, false, false, 0 );

	if ( p_caster->_HasSkillLine( GetProto()->eff[i].EffectMiscValue) )
		p_caster->_ModifySkillMaximum( GetProto()->eff[i].EffectMiscValue, uint32( damage * 75 ) );
	else
		p_caster->_AddSkillLine( GetProto()->eff[i].EffectMiscValue, 1, uint32( damage * 75 ) );
}

void Spell::SpellEffectApplyPetAura(uint32 i)
{
	SpellEffectApplyAura(i);
}

void Spell::SpellEffectDummyMelee( uint32 i ) // Normalized Weapon damage +
{

	if( unitTarget == NULL || u_caster == NULL )
	{ 
		return;
	}

	if( GetProto()->NameHash == SPELL_HASH_OVERPOWER && p_caster != NULL ) //warrior : overpower - let us clear the event and the combopoint count
	{
		p_caster->NullComboPoints(); //some say that we should only remove 1 point per dodge. Due to cooldown you can't cast it twice anyway..
		sEventMgr.RemoveEvents( p_caster, EVENT_COMBO_POINT_CLEAR_FOR_TARGET );
	}
	else if( GetProto()->NameHash == SPELL_HASH_DEVASTATE)
	{
		//count the number of sunder armors on target
		uint32 sunder_count=0;
		SpellEntry *spellInfo=dbcSpell.LookupEntry(58567);	//sunder armor rank one will be casted in case we will not find a better one on the target
		for(uint32 x = MAX_POSITIVE_AURAS; x < MAX_NEGATIVE_AURAS1(unitTarget); ++x)
		{
			if(unitTarget->m_auras[x] && unitTarget->m_auras[x]->GetSpellProto()->NameHash==SPELL_HASH_SUNDER_ARMOR)
			{
				sunder_count++;
				spellInfo=unitTarget->m_auras[x]->GetSpellProto();
			}
		}
		if(!spellInfo)
			return; //omg how did this happen ?
		//we should also cast sunder armor effect on target with or without dmg
		Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		spell->Init( u_caster, spellInfo, true, NULL );
		spell->ProcedOnSpell = GetProto();
		spell->pSpellId=GetProto()->Id;
		SpellCastTargets targets(unitTarget->GetGUID());
		spell->prepare(&targets);
		if( p_caster && p_caster->HasGlyphWithID( GLYPH_WARRIOR_DEVASTATE ) )
		{
			Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
			spell->Init(u_caster, spellInfo ,true, NULL);
			spell->ProcedOnSpell = GetProto();
			spell->pSpellId=GetProto()->Id;
			SpellCastTargets targets(unitTarget->GetGUID());
			spell->prepare(&targets);
		}
		if(!sunder_count)
			return; //no damage = no joy
		damage = damage*sunder_count;
	}
/*	else if( GetProto()->NameHash == SPELL_HASH_CRUSADER_STRIKE ) // Crusader Strike - refreshes *all* judgements, not just your own
	{
		for( uint32 x = MAX_POSITIVE_AURAS1(unitTarget) ; x <= MAX_NEGATIVE_AURAS1(unitTarget) ; x ++ ) // there are only debuff judgements anyway :P
		{
			if( unitTarget->m_auras[x] && unitTarget->m_auras[x]->GetSpellProto()->BGR_one_buff_from_caster_on_1target == SPELL_TYPE_INDEX_JUDGEMENT )
			{
				// Refresh it!
				// oh noes, they don't stack...
				Aura * aur = unitTarget->m_auras[x];
				SpellEntry * spinfo = aur->GetSpellProto();
				aur->Remove();
				Spell * sp = SpellPool.PooledNew( __FILE__, __LINE__ );
				sp->Init( this->u_caster , spinfo , true , NULL );
				
				SpellCastTargets tgt;
				tgt.m_unitTarget = unitTarget->GetGUID();
				sp->prepare( &tgt );
			}
		}
	}*/


	uint32 _type;
	if( GetType() == SPELL_DMG_TYPE_RANGED )
		_type = RANGED;
	else
	{
		if (GetProto()->AttributesExC & 0x1000000)
			_type =  OFFHAND;
		else
			_type = MELEE;
	}

	//!!!not sure !!!
/*	if( p_caster 
//		&& _type != RANGED	//hunters might make insane dmg with this
		&& GetProto()->AttackPowerToSpellDamageCoeff == 0.0f
		)
	{
		float WeaponNormalizationCoeff = p_caster->GetWeaponNormalizationCoeff( (WeaponDamageType)_type );
		damage = float2int32( damage * WeaponNormalizationCoeff );
	} */

	// rogue ambush etc
	for (uint32 x =i+1;x<3;x++)
		if(GetProto()->eff[x].Effect == SPELL_EFFECT_WEAPON_PERCENT_DAMAGE)
		{
//			add_damage = damage * (GetProto()->eff[x].EffectBasePoints+1) /100;
			add_damage = damage;	//115% weapon damage + 
			return;
		}

	//rogue - mutilate ads dmg if target is poisoned
	uint32 add_pct_dmg_mod = forced_pct_mod_cur - 100;
#ifdef USE_SPELL_MISSILE_DODGE
	if(GetProto()->speed > 0 && _type == RANGED )
	{
		float dist = m_caster->CalcDistance( unitTarget );
		float time = ((dist*1000.0f)/GetProto()->speed);
		LocationVector src,dst;
		src.x = m_caster->GetPositionX();
		src.y = m_caster->GetPositionY();
		src.z = m_caster->GetPositionZ();
		dst.x = unitTarget->GetPositionX();
		dst.y = unitTarget->GetPositionY();
		dst.z = unitTarget->GetPositionZ();
		sEventMgr.AddEvent(m_caster, &Object::EventWeaponDamageToLocation, unitTarget->GetGUID(), GetProto(), (uint32)damage, (int32)add_pct_dmg_mod, (int32)i, src, dst, EVENT_SPELL_DAMAGE_HIT, uint32(time), 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
	}
	else
		u_caster->Strike( unitTarget, _type, GetProto(), damage, add_pct_dmg_mod, 0, false, false );
#else
	u_caster->Strike( unitTarget, _type, GetProto(), damage, add_pct_dmg_mod, 0, false, false );
#endif
}

void Spell::SpellEffectFilming( uint32 i )
{
	if (!playerTarget || !playerTarget->isAlive() || !u_caster)
	{ 
		return;
	}

	if(playerTarget->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_LOCK_PLAYER))
	{ 
		return;
	}

	TaxiPath* taxipath = sTaxiMgr.GetTaxiPath(GetProto()->eff[0].EffectMiscValue);

	if( !taxipath )
	{ 
		return;
	}

	TaxiNode* taxinode = sTaxiMgr.GetTaxiNode( taxipath->GetSourceNode() );

	if( !taxinode )
	{ 
		return;
	}

	uint32 modelid =0;

	if( playerTarget->GetTeam() )
	{
		CreatureInfo* ci = CreatureNameStorage.LookupEntry( taxinode->horde_mount );
		if(!ci) 
		{ 
			return;
		}
		modelid = ci->Male_DisplayID;
		if(!modelid) 
		{ 
			return;
		}
	}
	else
	{
		CreatureInfo* ci = CreatureNameStorage.LookupEntry( taxinode->alliance_mount );
		if(!ci) 
		{ 
			return;
		}
		modelid = ci->Male_DisplayID;
		if(!modelid) 
		{ 
			return;
		}
	}

	if(playerTarget->GetSummon() != NULL)
	{
		playerTarget->GetSummon()->Dismiss( true, false );					  // hunter pet -> just remove for later re-call
	}

	playerTarget->TaxiStart(taxipath, modelid, 0);
}

void Spell::SpellEffectSpellSteal( uint32 i )
{
	if (!unitTarget || !u_caster || !unitTarget->isAlive())
	{ 
		return;
	}
	if(unitTarget->IsPlayer() && p_caster && p_caster != SafePlayerCast(unitTarget))
	{
		if(SafePlayerCast(unitTarget)->IsPvPFlagged())
			p_caster->SetPvPFlag();
	}

	Aura *aur;
	uint32 start, end;
	if(isAttackable(u_caster,unitTarget))
	{
		start=0;
		end=MAX_POSITIVE_AURAS1(unitTarget);
	}
	else
		return;
	
	//SpellSteal should steal random buff, not the newest/oldest ones. This way enemy can spam a dummy spell to avoid spell steal to work
	uint32 AvailableIndexesToSteal[MAX_AURAS];
	uint32 UsedIndexes = 0;
	for(uint32 x=start;x<end;x++)
		if(unitTarget->m_auras[x])
		{
			aur = unitTarget->m_auras[x];
			if( ( aur->GetSpellId() != 15007 && !aur->IsPassive() 
					&& ( aur->GetSpellProto()->c_is_flags & (SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET|SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER))==0
					&& ( aur->GetSpellProto()->talent_entry_assoc_counter == 0 )
	//				&& aur->IsPositive()	// Zack : We are only checking positiv auras. There is no meaning to check again
					&& ( aur->GetSpellProto()->AttributesExD & SPELL_ATTR4_NOT_STEALABLE ) == 0
					&& aur->GetSpellProto()->GetDispelType() == DISPEL_MAGIC
					&& aur->GetTimeLeft() > 2000	//this will bug out client due to fast add / remove and an icon will remain there forever
				) //Nothing can dispel resurrection sickness
				|| ( aur->GetSpellProto()->c_is_flags2 & SPELL_FLAG2_IS_SPELL_STEAL_SAFE ) //manually set these flags and they are safe to steal
				)
			{
				AvailableIndexesToSteal[ UsedIndexes ] = x;
				UsedIndexes++;
			}
		}

	WorldPacket data(SMSG_SPELLDISPELLOG, 16);

	for(uint32 i=0;i<UsedIndexes;i++)
	{
		uint32 x = AvailableIndexesToSteal[ RandomUInt() % UsedIndexes ];
		aur = unitTarget->m_auras[x];
		if( aur )
		{
				data.clear();
				data << m_caster->GetNewGUID();
				data << unitTarget->GetNewGUID();
				data << (uint32)1;
				data << aur->GetSpellId();
				m_caster->SendMessageToSet(&data,true);

				uint32 OriAuraTimeLeft = aur->GetTimeLeft();
				uint32 aurdur = ( OriAuraTimeLeft>120000 ? 120000 : OriAuraTimeLeft );
				Aura *aura = AuraPool.PooledNew( __FILE__, __LINE__ );
				aura->Init(aur->GetSpellProto(), aurdur, u_caster, u_caster );
				uint32 aur_removed = unitTarget->RemoveAuraByNameHash( aura->GetSpellProto()->NameHash, 0, AURA_SEARCH_POSITIVE, MAX( 1, aura->GetSpellProto()->procCharges ) );
				for ( uint32 i = 0; i < 3; i++ )
				{
					if ( aura->GetSpellProto()->eff[i].Effect )
					{
						aura->AddMod( aura->GetSpellProto()->eff[i].EffectApplyAuraName, aura->GetSpellProto()->eff[i].EffectBasePoints+1, aura->GetSpellProto()->eff[i].EffectMiscValue, i, forced_pct_mod_cur );
					}
				}
				if( aura->GetSpellProto()->procCharges>0 )
				{
					Aura *aur = NULL;
					for(uint32 i = 0; i<aur_removed-1; i++)
					{
						aur = AuraPool.PooledNew( __FILE__, __LINE__ );
						aur->Init( aura->GetSpellProto(), aurdur, u_caster, u_caster );
						aur->SetDuration( aurdur ); // double checking
						u_caster->AddAura(aur);
						aur = NULL;
					}
					if(!(aura->GetSpellProto()->procFlags2 & PROC2_REMOVEONUSE))
					{
						SpellCharge charge;
						charge.count=aur_removed;
//						charge.spellId=aura->GetSpellId();
						charge.spe=aura->GetSpellProto();
						if( aura->GetSpellProto()->procFlagsRemove )
							charge.ProcFlag=aura->GetSpellProto()->procFlagsRemove;
						else
							charge.ProcFlag=aura->GetSpellProto()->procFlags;
//						u_caster->m_chargeSpells.insert(make_pair(aura->GetSpellId(),charge));
						u_caster->RegisterNewChargeStruct( charge );
					}
				}
				u_caster->AddAura(aura);
				aura->SetDuration( aurdur ); // double checking
				break;
		}
	}   
}

void Spell::SpellEffectProspecting(uint32 i)
{
	if(!p_caster) 
	{ 
		return;
	}

	if(!itemTarget) // this should never happen
	{
		SendCastResult(SPELL_FAILED_CANT_BE_PROSPECTED);
		return;
	}

	//Fill Prospecting loot
	p_caster->SetLootGUID(itemTarget->GetGUID());
	if( !itemTarget->loot )
		{
			itemTarget->loot = new Loot;
			lootmgr.FillItemLoot( itemTarget->loot , itemTarget->GetEntry());
		}

	if ( itemTarget->loot->items.size() > 0 )
	{
		Log.Debug("SpellEffect","Succesfully prospected item %d", uint32(itemTarget->GetEntry()));
		p_caster->SendLoot( itemTarget->GetGUID(), LOOT_PROSPECTING );
	} 
	else // this should never happen either
	{
		Log.Debug("SpellEffect","Prospecting failed, item %d has no loot", uint32(itemTarget->GetEntry()));
		SendCastResult(SPELL_FAILED_CANT_BE_PROSPECTED);
	}
}

void Spell::SpellEffectResurrectNew(uint32 i)
{
	//base p =hp,misc mana
	if(!playerTarget)
	{
		if(!corpseTarget)
		{
			// unit resurrection handler
			if(unitTarget)
			{
				if(unitTarget->GetTypeId()==TYPEID_UNIT && unitTarget->IsPet() && unitTarget->IsDead())
				{
					uint32 hlth = ((uint32)GetProto()->eff[i].EffectBasePoints > unitTarget->GetUInt32Value(UNIT_FIELD_MAXHEALTH)) ? unitTarget->GetUInt32Value(UNIT_FIELD_MAXHEALTH) : (uint32)GetProto()->eff[i].EffectBasePoints;
					uint32 mana = ((uint32)GetProto()->eff[i].EffectBasePoints > unitTarget->GetMaxPower( POWER_TYPE_MANA )) ? unitTarget->GetMaxPower( POWER_TYPE_MANA ) : (uint32)GetProto()->eff[i].EffectBasePoints;

					if(!unitTarget->IsPet())
					{
						sEventMgr.RemoveEvents(unitTarget, EVENT_CREATURE_REMOVE_CORPSE);
					}
					else
					{
						sEventMgr.RemoveEvents(unitTarget, EVENT_PET_DELAYED_REMOVE);
						sEventMgr.RemoveEvents(unitTarget, EVENT_CREATURE_REMOVE_CORPSE);
					}
					unitTarget->SetUInt32Value(UNIT_FIELD_HEALTH, hlth);
					unitTarget->SetPower( POWER_TYPE_MANA, mana);
					unitTarget->SetUInt32Value(UNIT_DYNAMIC_FLAGS, 0);
					unitTarget->setDeathState(ALIVE);
//					SafeCreatureCast(unitTarget)->Tagged=false;
					SafeCreatureCast(unitTarget)->TaggerGroupId=0;
					SafeCreatureCast(unitTarget)->TaggerGuid=0;
					SafeCreatureCast(unitTarget)->loot.gold=0;
					SafeCreatureCast(unitTarget)->loot.looters.clear();
					//clear roll info for items
					SafeCreatureCast(unitTarget)->loot.items.clear();
					SafeCreatureCast(unitTarget)->loot.currencies.clear();
				}
			}

			return;
		}
		playerTarget = objmgr.GetPlayer(corpseTarget->GetUInt32Value(CORPSE_FIELD_OWNER));
		if(!playerTarget) 
		{ 
			return;
		}
	}

	if(playerTarget->isAlive() || !playerTarget->IsInWorld())
	{ 
		return;
	}
	//resurr
	//playerTarget->resurrector = p_caster->GetLowGUID();
	playerTarget->m_resurrectMapId = m_caster->GetMapId();
	playerTarget->m_resurrectInstanceID = m_caster->GetInstanceID();
	playerTarget->m_resurrectPosition = m_caster->GetPosition();
	playerTarget->m_resurrectHealth = damage;
	playerTarget->m_resurrectMana = GetProto()->eff[i].EffectMiscValue;

	SendResurrectRequest( playerTarget );
}

void Spell::SpellEffectTranformItem(uint32 i)
{
	bool result;
	AddItemResult result2;

	if(!i_caster)
	{ 
		return;
	}
	uint32 itemid=GetProto()->eff[i].EffectItemType;
	if(!itemid)
	{ 
		return;
	}

	//Save durability of the old item
	Player * owner=i_caster->GetOwner();
	uint32 dur= i_caster->GetUInt32Value(ITEM_FIELD_DURABILITY);
	//	int8 slot=owner->GetItemInterface()->GetInventorySlotByGuid(i_caster->GetGUID());
	//	uint32 invt=i_caster->GetProto()->InventoryType;

	   result  = owner->GetItemInterface()->SafeFullRemoveItemByGuid(i_caster->GetGUID());
	if(!result)
	{
		//something went wrong if this happen, item doesnt exist, so it wasnt destroyed.
		return;
	}

	i_caster=NULL;

	Item *it=objmgr.CreateItem(itemid,owner);
	it->SetDurability(dur);
	//additem
	
	   //additem
	result2 = owner->GetItemInterface()->AddItemToFreeSlot(&it);
	if(!result2) //should never get here
	{ 
		owner->GetItemInterface()->BuildInventoryChangeError(NULL,NULL,INV_ERR_BAG_FULL);
		it->DeleteMe();
	}
}

void Spell::SpellEffectEnvironmentalDamage(uint32 i)
{
	if(!playerTarget)
	{ 
		return;
	}

	if( playerTarget->SchoolImmunityAntiEnemy[GetProto()->School] || playerTarget->SchoolImmunityAntiFriend[GetProto()->School] )
	{
		SendCastResult(SPELL_FAILED_IMMUNE);
		return;
	}
	//this is GO, not unit	
	dealdamage tdmg;
	tdmg.base_dmg = damage;
	tdmg.pct_mod_final_dmg = forced_pct_mod_cur;
	tdmg.StaticDamage = true;
	tdmg.DisableProc = (pSpellId!=0);
	m_caster->SpellNonMeleeDamageLog( unitTarget, GetProto(), &tdmg, 1, i );
  
	WorldPacket data(SMSG_ENVIRONMENTALDAMAGELOG, 13);
	data << unitTarget->GetGUID();
	data << uint8(DAMAGE_FIRE);
	data << uint32(damage);
	data << uint32(0);
	data << uint32(0);
	unitTarget->SendMessageToSet( &data, true );
}

void Spell::SpellEffectDismissPet(uint32 i)
{
	// remove pet.. but don't delete so it can be called later
	if(!p_caster)
	{ 
		return;
	}

	Pet *pPet = p_caster->GetSummon();
	if(!pPet || pPet->IsInWorld() == false || pPet->deleted != OBJ_AVAILABLE )
	{ 
		return;
	}
	pPet->Dismiss(true, true);
	p_caster->SetSummon(NULL);	//just in case of broken chain
}

void Spell::SpellEffectEnchantHeldItem( uint32 i )
{
	if( playerTarget == NULL )
	{ 
		return;
	}

	Item * item = playerTarget->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_MAINHAND );
	if( item == NULL )
	{ 
		return;
	}

	uint32 Duration = 1800; // Needs to be found in dbc.. I guess?

	EnchantEntry * Enchantment = dbcEnchant.LookupEntry( GetProto()->eff[i].EffectMiscValue );
	
	if( Enchantment == NULL )
	{ 
		return;
	}

	item->RemoveEnchantment( ITEM_ENCHANT_SLOT_TEMPORARY2 );
	item->AddEnchantment( Enchantment, Duration, false, true, false, ITEM_ENCHANT_SLOT_TEMPORARY2 );
}

void Spell::SpellEffectAddHonor(uint32 i)
{
	if( playerTarget == NULL )
	{ 
		return;
	}
	uint32 val = GetProto()->eff[i].EffectBasePoints;

	if( GetProto()->AttributesExB & FLAGS3_UNK4 )val /= 10;

	val += 1;

	HonorHandler::AddHonorPointsToPlayer( playerTarget, val );

	WorldPacket data(SMSG_PVP_CREDIT, 16);
	data << val;
	data << uint64(0);
	data << uint32(5);
	playerTarget->GetSession()->SendPacket(&data);
}

void Spell::SpellEffectSpawn(uint32 i)
{
	// this effect is mostly for custom teleporting
	switch(GetProto()->Id)
	{
	  case 10418: // Arugal spawn-in spell , teleports it to 3 locations randomly sneeking players (bastard ;P)   
	  { 
		if(!u_caster || u_caster->IsPlayer())
		{ 
			return;
		}
		 
		static float coord[3][3]= {{-108.9034f,2129.5678f,144.9210f},{-108.9034f,2155.5678f,155.678f},{-77.9034f,2155.5678f,155.678f}};
		
		int i = (int)(rand()%3);
		u_caster->GetAIInterface()->SendMoveToPacket(coord[i][0],coord[i][1],coord[i][2],0.0f,0,u_caster->GetAIInterface()->getMoveFlags());
	  }
	}
}

void Spell::SpellEffectApplyAura128(uint32 i)
{
	if(GetProto()->eff[i].EffectApplyAuraName != 0)
		SpellEffectApplyAura(i);
}

void Spell::SpellEffectRuneActivate(uint32 i)
{
	if( !p_caster )
		return; //atm only players can have runes
	uint32 RuneToActivate;
	if( forced_miscvalues[i] != 0 )
		RuneToActivate = forced_miscvalues[i];
	else
		RuneToActivate = GetProto()->eff[i].EffectMiscValue;
	if( RuneToActivate >= TOTAL_USED_RUNES )
	{
		sLog.outDebug("Spell is trying to convert unsupported runetype %u",RuneToActivate);
		return; 
	}

//	p_caster->TakeFullRuneCount( GetProto()->eff[i].EffectMiscValue, damage ); //seems dmg is not 1-2
	p_caster->ActivateRuneType( RuneToActivate );
	if( damage > 1 )
		p_caster->ActivateRuneType( RuneToActivate );
	//it's enough to send this only once / spell cast, Sending spell GO after spell effects would also eliminate the need for this
	//right now activate already sends this
//	p_caster->UpdateRuneCooldowns();	
}

void Spell::SpellEffectTriggerSpellWithValue(uint32 i)
{
	if( unitTarget == NULL )
	{ 
		return;
	}

	SpellEntry* TriggeredSpell = dbcSpell.LookupEntryForced(GetProto()->eff[i].EffectTriggerSpell);
	if( TriggeredSpell == NULL )
	{ 
		return;
	}

	Spell *sp=SpellPool.PooledNew( __FILE__, __LINE__ );
	sp->Init(m_caster,TriggeredSpell,true,NULL);

	for(uint32 x=0;x<MAX_SPELL_EFFECT_COUNT;x++)
	{
//		if( i==x )
			sp->forced_basepoints[x] = damage;	//prayer of mending should inherit heal bonus ?
//		else
//			sp->forced_basepoints[x] = TriggeredSpell->eff[i].EffectBasePoints;
	}

	SpellCastTargets tgt(unitTarget->GetGUID());
	sp->prepare(&tgt);
}

void Spell::SpellEffectSummonTarget(uint32 i) // ritual of summoning
{
	if ( unitTarget == NULL )
	{ 
		return;
	}

	SpellEffectTriggerSpell( i );
}

void Spell::SpellEffectForgetSpecialization(uint32 i)
{
	if (!playerTarget)
	{ 
		return;
	}

	uint32 spellid = GetProto()->eff[i].EffectTriggerSpell;
	playerTarget->removeSpell( spellid, false, false, 0);

	sLog.outDebug("Player %u have forgot spell %u from spell %u ( caster: %u)", playerTarget->GetLowGUID(), spellid, GetProto()->Id, m_caster->GetLowGUID());
}

void Spell::SpellEffectPlayMusic(uint32 i)
{
    if( !unitTarget || unitTarget->IsPlayer() == false )
        return;

    uint32 soundid = GetProto()->eff[i].EffectMiscValue;

//    if (!sSoundEntriesStore.LookupEntry(soundid))
//    {
//        sLog.outError("EffectPlayMusic: Sound (Id: %u) not exist in spell %u.",soundid,m_spellInfo->Id);
//        return;
//    }

//	sStackWorldPacket( data, SMSG_PLAY_MUSIC, 4 + 10 );
	sStackWorldPacket( data, SMSG_PLAY_SOUND, 4 + 10 );
    data << uint32( soundid );
    SafePlayerCast(unitTarget)->GetSession()->SendPacket(&data);
}

void Spell::SpellEffectMilling(uint32 i)
{
	if(!p_caster) 
	{ 
		return;
	}

	if(!itemTarget) // this should never happen
	{
		SendCastResult(SPELL_FAILED_CANT_BE_PROSPECTED);
		return;
	}

	//Fill Prospecting loot
	p_caster->SetLootGUID(itemTarget->GetGUID());
	if( !itemTarget->loot )
		{
			itemTarget->loot = new Loot;
			lootmgr.FillItemLoot( itemTarget->loot , itemTarget->GetEntry());
		}

	if ( itemTarget->loot->items.size() > 0 )
	{
		Log.Debug("SpellEffect","Succesfully milled item %d", uint32(itemTarget->GetEntry()));
		p_caster->SendLoot( itemTarget->GetGUID(), LOOT_MILLING );
	} 
	else // this should never happen either
	{
		Log.Debug("SpellEffect","Milling failed, item %d has no loot", uint32(itemTarget->GetEntry()));
		SendCastResult(SPELL_FAILED_CANT_BE_PROSPECTED);
	}
}

void Spell::SpellEffectSetTalentSpecsCount(uint32 i)
{
	if(!p_caster)
	{ 
		return;
	}

	p_caster->m_talentSpecsCount = damage % MAX_SPEC_COUNT;

	// Send update
//	p_caster->smsg_TalentsInfo(false, 0, 0);
}

void Spell::SpellEffectActivateTalentSpec(uint32 i)
{
	if(!p_caster)
	{ 
		return;
	}

	//remove offhand whatever item and mail it to the owner
	if( p_caster->HasSpellwithNameHash( SPELL_HASH_TITAN_S_GRIP ) && p_caster->GetItemInterface() && p_caster->getClass() == WARRIOR ) 
	{
		Item * it = p_caster->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_OFFHAND );
		if( it )
			p_caster->RemoveAndMailItemFromSlot( EQUIPMENT_SLOT_OFFHAND );
	}

	//spamming same spell ? Nothing to switch ?
	uint32 new_spec = ( GetProto()->eff[i].EffectBasePoints + 1 ) % p_caster->m_talentSpecsCount;
	if( p_caster->m_talentActiveSpec == new_spec )
	{
		sLog.outDebug("Something is wrong, we are activating our already active spec");
		return;
	}
	
	//cannot switch in conditions
	if(p_caster->CombatStatus.IsInCombat())
	{
		SendCastResult(SPELL_FAILED_AFFECTING_COMBAT);
		return;
	}
	else if(p_caster->m_bg)
	{
		uint32 Type = p_caster->m_bg->GetType();
		if(Type >= BATTLEGROUND_ARENA_2V2 && Type <= BATTLEGROUND_ARENA_5V5)
		{
			SendCastResult(SPELL_FAILED_AFFECTING_COMBAT); // does the job
			return;
		}
		else
		{
			if(p_caster->m_bg->HasStarted())
			{
				SendCastResult(SPELL_FAILED_AFFECTING_COMBAT); // does the job
			}
		}
	}

	// 1 = primary, 2 = secondary
//	p_caster->m_talentActiveSpec = damage % p_caster->m_talentSpecsCount;
	// Send update
//	p_caster->smsg_TalentsInfo(false, 0, 0);
	p_caster->Switch_Talent_Spec();

	//reset powers
	p_caster->SetPower(p_caster->GetPowerType(), 0);
//	p_caster->UpdatePowerAmm( );
}

void Spell::SpellEffectRenamePet( uint32 i )
{
	if( !unitTarget || !unitTarget->IsPet() || 
		!SafePetCast(unitTarget)->GetPetOwner() || SafePetCast(unitTarget)->GetPetOwner()->getClass() != HUNTER )
		return;

	unitTarget->SetByte( UNIT_FIELD_BYTES_2, 2, UNIT_BYTE2_FLAG_RENAME|UNIT_BYTE2_FLAG_PET_DETAILS );
}

void Spell::SpellEffectBind(uint32 i)
{
	if(!playerTarget || !playerTarget->isAlive() || !m_caster)
		return;

	WorldPacket data(45);
	uint32 zoneid = playerTarget->GetZoneId();
	uint32 mapid = playerTarget->GetMapId();
	if(GetProto()->eff[i].EffectMiscValue)
	{
		zoneid = GetProto()->eff[i].EffectMiscValue;
		AreaTable * at = dbcArea.LookupEntry(zoneid);
		if(!at)
			return;
		mapid = at->mapId;
	}

	playerTarget->SetBindPoint(playerTarget->GetPositionX(), playerTarget->GetPositionY(), playerTarget->GetPositionZ(), mapid, zoneid);

	data.Initialize(SMSG_BINDPOINTUPDATE);
	data << playerTarget->GetBindPositionX() << playerTarget->GetBindPositionY() << playerTarget->GetBindPositionZ() << playerTarget->GetBindMapId() << playerTarget->GetBindZoneId();
	playerTarget->GetSession()->SendPacket( &data );

	data.Initialize(SMSG_PLAYERBOUND);
	data << m_caster->GetGUID() << playerTarget->GetBindZoneId();
	playerTarget->GetSession()->SendPacket(&data);
}

void Spell::SpellEffectLanguage(uint32 i)
{
/*	if(!playerTarget || !GetProto()->eff[i].EffectMiscValue)
		return;

	if( GetProto()->eff[i].EffectMiscValue >= 15 )
	{
		sLog.outDebug(" Learn Language index out of range %u\n",GetProto()->eff[i].EffectMiscValue);
	}

	uint32 skills[15][2] = {
	{ 0, 0 },
	{ SKILL_LANG_ORCISH, 669 },
	{ SKILL_LANG_DARNASSIAN, 671 },
	{ SKILL_LANG_TAURAHE, 670 },
	{ SKILL_LANG_DWARVEN, 672 },
	{ SKILL_LANG_COMMON, 668 },
	{ SKILL_LANG_DEMON_TONGUE, 815 },
	{ SKILL_LANG_TITAN, 816 },
	{ SKILL_LANG_THALASSIAN, 813 },
	{ SKILL_LANG_DRACONIC, 814 },
	{ 0, 0 },
	{ SKILL_LANG_GNOMISH, 7430 },
	{ SKILL_LANG_TROLL, 7431 },
	{ SKILL_LANG_GUTTERSPEAK, 17737 },
	{ SKILL_LANG_DRAENEI, 29932 },
	};

	if(skills[GetProto()->eff[i].EffectMiscValue][0])
	{
		playerTarget->_AddSkillLine(skills[GetProto()->eff[i].EffectMiscValue][0], 300, 300);   
		playerTarget->addSpell(skills[GetProto()->eff[i].EffectMiscValue][1]);
	}

	//no multi casts in case of playercreate spell
	playerTarget->removeSpell( GetProto()->Id, false, false, 0 );
	*/
}

void Spell::SpellEffectCreatePet(uint32 i)
{
	if( !playerTarget )
		return;

	if(playerTarget->GetSummon())
	{
		playerTarget->GetSummon()->Dismiss( true, true );
		playerTarget->SetSummon( NULL );
	}
	uint32 Entry;
	if( forced_miscvalues[i] != 0 )
		Entry = forced_miscvalues[i];
	else
		Entry = GetProto()->eff[i].EffectMiscValue;
	CreatureInfo *ci = CreatureNameStorage.LookupEntry( Entry );
	if( ci )
	{
		if( ci->IsExotic() == true && playerTarget->HasAuraWithNameHash( SPELL_HASH_BEAST_MASTERY, 0, AURA_SEARCH_PASSIVE ) == false )
		{
			p_caster->BroadcastMessage("You need to spec beast mastery talent tree to tame or use exotic pets" );
			SendTameFailure( PETTAME_CANTCONTROLEXOTIC );
			return;
		}
		Pet *pPet = objmgr.CreatePet( Entry, playerTarget->GeneratePetNumber( Entry ) );

		pPet->CreateAsSummon( Entry, ci, NULL, playerTarget, GetProto(), 1, 0 );
	}
}

void Spell::SpellEffectTeachTaxiPath( uint32 i )
{
	if( !playerTarget || !GetProto()->eff[i].EffectTriggerSpell )
		return;

	playerTarget->removeSpell( GetProto()->Id, false, false, 0 );
	uint8 field = (uint8)((GetProto()->eff[i].EffectTriggerSpell - 1) / 32);
	uint32 submask = 1<<((GetProto()->eff[i].EffectTriggerSpell-1)%32);

	// Check for known nodes
	if (!(playerTarget->GetTaximask(field) & submask))
	{
		playerTarget->SetTaximask(field, (submask | playerTarget->GetTaximask(field)) );

		playerTarget->GetSession()->OutPacket(SMSG_NEW_TAXI_PATH);

		//Send packet
		WorldPacket update(SMSG_TAXINODE_STATUS, 9);
		update << uint64( 0 ) << uint8( 1 );
		playerTarget->GetSession()->SendPacket( &update );
	}
}

void Spell::SpellEffectWMODamage(uint32 i)
{
	if(gameObjTarget && gameObjTarget->GetInfo() && gameObjTarget->GetInfo()->Type == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING)
		gameObjTarget->TakeDamage(uint32(damage), m_caster, p_caster, m_spellInfo->Id);
}

void Spell::SpellEffectWMORepair(uint32 i)
{
	if(gameObjTarget && gameObjTarget->GetInfo()->Type == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING)
		gameObjTarget->Rebuild();
}
