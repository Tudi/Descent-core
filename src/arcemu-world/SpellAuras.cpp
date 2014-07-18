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

pSpellAura SpellAuraHandler[TOTAL_SPELL_AURAS]={
		&Aura::SpellAuraNULL,//SPELL_AURA_NONE = 0
		&Aura::SpellAuraBindSight,//SPELL_AURA_BIND_SIGHT = 1
		&Aura::SpellAuraModPossess,//SPELL_AURA_MOD_POSSESS = 2,
		&Aura::SpellAuraPeriodicDamage,//SPELL_AURA_PERIODIC_DAMAGE = 3,
		&Aura::SpellAuraDummy,//SPELL_AURA_DUMMY = 4,
		&Aura::SpellAuraModConfuse,//SPELL_AURA_MOD_CONFUSE = 5,
		&Aura::SpellAuraModCharm,//SPELL_AURA_MOD_CHARM = 6,
		&Aura::SpellAuraModFear,//SPELL_AURA_MOD_FEAR = 7,
		&Aura::SpellAuraPeriodicHeal,//SPELL_AURA_PERIODIC_HEAL = 8,
		&Aura::SpellAuraModAttackSpeed,//SPELL_AURA_MOD_ATTACKSPEED = 9,
		&Aura::SpellAuraModThreatGeneratedSchoolPCT,//SPELL_AURA_MOD_THREAT = 10,
		&Aura::SpellAuraModTaunt,//SPELL_AURA_MOD_TAUNT = 11,
		&Aura::SpellAuraModStun,//SPELL_AURA_MOD_STUN = 12,
		&Aura::SpellAuraModDamageDone,//SPELL_AURA_MOD_DAMAGE_DONE = 13,
		&Aura::SpellAuraModDamageTaken,//SPELL_AURA_MOD_DAMAGE_TAKEN = 14,
		&Aura::SpellAuraDamageShield,//SPELL_AURA_DAMAGE_SHIELD = 15,
		&Aura::SpellAuraModStealth,//SPELL_AURA_MOD_STEALTH = 16,
		&Aura::SpellAuraModDetect,//SPELL_AURA_MOD_DETECT = 17,
		&Aura::SpellAuraModInvisibility,//SPELL_AURA_MOD_INVISIBILITY = 18,
		&Aura::SpellAuraModInvisibilityDetection,//SPELL_AURA_MOD_INVISIBILITY_DETECTION = 19,
		&Aura::SpellAuraModTotalHealthRegenPct,// SPELL_AURA_MOD_TOTAL_HEALTH_REGEN_PCT = 20
		&Aura::SpellAuraModTotalManaRegenPct,// SPELL_AURA_MOD_TOTAL_MANA_REGEN_PCT = 21
		&Aura::SpellAuraModResistance,//SPELL_AURA_MOD_RESISTANCE = 22,
		&Aura::SpellAuraPeriodicTriggerSpell,//SPELL_AURA_PERIODIC_TRIGGER_SPELL = 23,
		&Aura::SpellAuraPeriodicEnergize,//SPELL_AURA_PERIODIC_ENERGIZE = 24,
		&Aura::SpellAuraModPacify,//SPELL_AURA_MOD_PACIFY = 25,
		&Aura::SpellAuraModRoot,//SPELL_AURA_MOD_ROOT = 26,
		&Aura::SpellAuraModSilence,//SPELL_AURA_MOD_SILENCE = 27,
		&Aura::SpellAuraReflectSpells,//SPELL_AURA_REFLECT_SPELLS = 28,
		&Aura::SpellAuraModStat,//SPELL_AURA_MOD_STAT = 29,
		&Aura::SpellAuraModSkill,//SPELL_AURA_MOD_SKILL = 30,
		&Aura::SpellAuraModIncreaseSpeed,//SPELL_AURA_MOD_INCREASE_SPEED = 31,
		&Aura::SpellAuraModIncreaseMountedSpeed,//SPELL_AURA_MOD_INCREASE_MOUNTED_SPEED = 32,
		&Aura::SpellAuraModDecreaseSpeed,//SPELL_AURA_MOD_DECREASE_SPEED = 33,
//		&Aura::SpellAuraModIncreaseHealth,//SPELL_AURA_MOD_INCREASE_HEALTH = 34,
		&Aura::SpellAuraAddHealth,//SPELL_AURA_MOD_INCREASE_HEALTH = 34,
		&Aura::SpellAuraModIncreaseEnergy,//SPELL_AURA_MOD_INCREASE_ENERGY = 35,
		&Aura::SpellAuraModShapeshift,//SPELL_AURA_MOD_SHAPESHIFT = 36,
		&Aura::SpellAuraModEffectImmunity,//SPELL_AURA_EFFECT_IMMUNITY = 37,
		&Aura::SpellAuraModStateImmunity,//SPELL_AURA_STATE_IMMUNITY = 38,
		&Aura::SpellAuraModSchoolImmunity,//SPELL_AURA_SCHOOL_IMMUNITY = 39,
		&Aura::SpellAuraModDmgImmunity,//SPELL_AURA_DAMAGE_IMMUNITY = 40,
		&Aura::SpellAuraModDispelImmunity,//SPELL_AURA_DISPEL_IMMUNITY = 41,
		&Aura::SpellAuraProcTriggerSpell,//SPELL_AURA_PROC_TRIGGER_SPELL = 42,
		&Aura::SpellAuraProcTriggerDamage,//SPELL_AURA_PROC_TRIGGER_DAMAGE = 43,
		&Aura::SpellAuraTrackCreatures,//SPELL_AURA_TRACK_CREATURES = 44,
		&Aura::SpellAuraTrackResources,//SPELL_AURA_TRACK_RESOURCES = 45,
		&Aura::SpellAuraNULL,//SPELL_AURA_MOD_PARRY_SKILL = 46, obsolete? not used in 1.12.1 spell.dbc
		&Aura::SpellAuraModParryPerc,//SPELL_AURA_MOD_PARRY_PERCENT = 47,
		&Aura::SpellAuraNULL,//SPELL_AURA_MOD_DODGE_SKILL = 48, obsolete?
		&Aura::SpellAuraModDodgePerc,//SPELL_AURA_MOD_DODGE_PERCENT = 49,
		&Aura::SpellAuraCritHealDoneModPct,//SPELL_AURA_MOD_CRIT_HEAL_DONE_PCT = 50,
		&Aura::SpellAuraModBlockPerc,//SPELL_AURA_MOD_BLOCK_PERCENT = 51,
		&Aura::SpellAuraModCritPerc,//SPELL_AURA_MOD_CRIT_PERCENT = 52,
		&Aura::SpellAuraPeriodicLeech,//SPELL_AURA_PERIODIC_LEECH = 53,
		&Aura::SpellAuraModHitChance,//SPELL_AURA_MOD_HIT_CHANCE = 54,
		&Aura::SpellAuraModSpellHitChance,//SPELL_AURA_MOD_SPELL_HIT_CHANCE = 55,
		&Aura::SpellAuraTransform,//SPELL_AURA_TRANSFORM = 56,
		&Aura::SpellAuraModSpellCritChance,//SPELL_AURA_MOD_SPELL_CRIT_CHANCE = 57,
		&Aura::SpellAuraIncreaseSwimSpeed,//SPELL_AURA_MOD_INCREASE_SWIM_SPEED = 58,
		&Aura::SpellAuraModCratureDmgDone,//SPELL_AURA_MOD_DAMAGE_DONE_CREATURE = 59,
		&Aura::SpellAuraPacifySilence,//SPELL_AURA_MOD_PACIFY_SILENCE = 60,
		&Aura::SpellAuraModScale,//SPELL_AURA_MOD_SCALE = 61,
		&Aura::SpellAuraPeriodicHealthFunnel,//SPELL_AURA_PERIODIC_HEALTH_FUNNEL = 62,
		&Aura::SpellAuraNULL,//SPELL_AURA_PERIODIC_MANA_FUNNEL = 63,//obselete?
		&Aura::SpellAuraPeriodicManaLeech,//SPELL_AURA_PERIODIC_MANA_LEECH = 64,
		&Aura::SpellAuraModCastingSpeed,//SPELL_AURA_MOD_CASTING_SPEED = 65,
		&Aura::SpellAuraFeignDeath,//SPELL_AURA_FEIGN_DEATH = 66,
		&Aura::SpellAuraModDisarm,//SPELL_AURA_MOD_DISARM = 67,
		&Aura::SpellAuraModStalked,//SPELL_AURA_MOD_STALKED = 68,
		&Aura::SpellAuraSchoolAbsorb,//SPELL_AURA_SCHOOL_ABSORB = 69,
		&Aura::SpellAuraNULL,//SPELL_AURA_EXTRA_ATTACKS = 70,//obselete?
		&Aura::SpellAuraModSpellCritChanceSchool,//SPELL_AURA_MOD_SPELL_CRIT_CHANCE_SCHOOL = 71,
		&Aura::SpellAuraModPowerCost,//SPELL_AURA_MOD_POWER_COST = 72,
		&Aura::SpellAuraModPowerCostSchool,//SPELL_AURA_MOD_POWER_COST_SCHOOL = 73,
		&Aura::SpellAuraReflectSpellsSchool,//SPELL_AURA_REFLECT_SPELLS_SCHOOL = 74,
		&Aura::SpellAuraModLanguage,//SPELL_AURA_MOD_LANGUAGE = 75,
		&Aura::SpellAuraAddFarSight,//SPELL_AURA_FAR_SIGHT = 76,
		&Aura::SpellAuraMechanicImmunity,//SPELL_AURA_MECHANIC_IMMUNITY = 77,
		&Aura::SpellAuraMounted,//SPELL_AURA_MOUNTED = 78,
		&Aura::SpellAuraModDamagePercDone,//SPELL_AURA_MOD_DAMAGE_PERCENT_DONE = 79,
		&Aura::SpellAuraModPercStat,//SPELL_AURA_MOD_PERCENT_STAT = 80,
		&Aura::SpellAuraSplitDamage,//SPELL_AURA_SPLIT_DAMAGE = 81,
		&Aura::SpellAuraWaterBreathing,//SPELL_AURA_WATER_BREATHING = 82,
		&Aura::SpellAuraModBaseResistance,//SPELL_AURA_MOD_BASE_RESISTANCE = 83,
		&Aura::SpellAuraModRegen,//SPELL_AURA_MOD_REGEN = 84,
		&Aura::SpellAuraModPowerRegen,//SPELL_AURA_MOD_POWER_REGEN = 85,
		&Aura::SpellAuraChannelDeathItem,//SPELL_AURA_CHANNEL_DEATH_ITEM = 86,
		&Aura::SpellAuraModDamagePercTaken,//SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN = 87,
		&Aura::SpellAuraModRegenPercent,//SPELL_AURA_MOD_PERCENT_REGEN = 88,
		&Aura::SpellAuraPeriodicDamagePercent,//SPELL_AURA_PERIODIC_DAMAGE_PERCENT = 89,
		&Aura::SpellAuraModResistChance,//SPELL_AURA_MOD_RESIST_CHANCE = 90,
		&Aura::SpellAuraModDetectRange,//SPELL_AURA_MOD_DETECT_RANGE = 91,
		&Aura::SpellAuraPreventsFleeing,//SPELL_AURA_PREVENTS_FLEEING = 92,
		&Aura::SpellAuraModUnattackable,//SPELL_AURA_MOD_UNATTACKABLE = 93,
		&Aura::SpellAuraInterruptRegen,//SPELL_AURA_INTERRUPT_REGEN = 94,
		&Aura::SpellAuraGhost,//SPELL_AURA_GHOST = 95,
		&Aura::SpellAuraMagnet,//SPELL_AURA_SPELL_MAGNET = 96,
		&Aura::SpellAuraManaShield,//SPELL_AURA_MANA_SHIELD = 97,
		&Aura::SpellAuraSkillTalent,//SPELL_AURA_MOD_SKILL_TALENT = 98,
		&Aura::SpellAuraModAttackPower,//SPELL_AURA_MOD_ATTACK_POWER = 99,
		&Aura::SpellAuraVisible,//SPELL_AURA_AURAS_VISIBLE = 100,
		&Aura::SpellAuraModResistancePCT,//SPELL_AURA_MOD_RESISTANCE_PCT = 101,
		&Aura::SpellAuraModCreatureAttackPower,//SPELL_AURA_MOD_CREATURE_ATTACK_POWER = 102,
		&Aura::SpellAuraModTotalThreat,//SPELL_AURA_MOD_TOTAL_THREAT = 103,
		&Aura::SpellAuraWaterWalk,//SPELL_AURA_WATER_WALK = 104,
		&Aura::SpellAuraFeatherFall,//SPELL_AURA_FEATHER_FALL = 105,
		&Aura::SpellAuraHover,//SPELL_AURA_HOVER = 106,
		&Aura::SpellAuraAddFlatModifier,//SPELL_AURA_ADD_FLAT_MODIFIER = 107,
		&Aura::SpellAuraAddPctMod,//SPELL_AURA_ADD_PCT_MODIFIER = 108,
		&Aura::SpellAuraProcTriggerSpell,//SPELL_AURA_ADD_TARGET_TRIGGER = 109,
		&Aura::SpellAuraModPowerRegPerc,//SPELL_AURA_MOD_POWER_REGEN_PERCENT = 110,
		&Aura::SpellAuraNULL,//SPELL_AURA_ADD_CASTER_HIT_TRIGGER = 111,
		&Aura::SpellAuraOverrideClassScripts,//SPELL_AURA_OVERRIDE_CLASS_SCRIPTS = 112,
		&Aura::SpellAuraModRangedDamageTaken,//SPELL_AURA_MOD_RANGED_DAMAGE_TAKEN = 113,
		&Aura::SpellAuraModRangedDamageTakenPCT,//SPELL_AURA_MOD_RANGED_DAMAGE_TAKEN_PCT = 114,
		&Aura::SpellAuraModHealing,//SPELL_AURA_MOD_HEALING = 115,
		&Aura::SpellAuraIgnoreRegenInterrupt,//SPELL_AURA_IGNORE_REGEN_INTERRUPT = 116,
		&Aura::SpellAuraModMechanicResistance,//SPELL_AURA_MOD_MECHANIC_RESISTANCE = 117,
		&Aura::SpellAuraModHealingPCT,//SPELL_AURA_MOD_HEALING_PCT = 118,
		&Aura::SpellAuraNULL,//SPELL_AURA_SHARE_PET_TRACKING = 119,//obselete
		&Aura::SpellAuraUntrackable,//SPELL_AURA_UNTRACKABLE = 120,
		&Aura::SpellAuraEmphaty,//SPELL_AURA_EMPATHY = 121,
		&Aura::SpellAuraModOffhandDamagePCT,//SPELL_AURA_MOD_OFFHAND_DAMAGE_PCT = 122,
		&Aura::SpellAuraModPenetration,//SPELL_AURA_MOD_POWER_COST_PCT = 123, --> armor penetration & spell penetration, NOT POWER COST!
		&Aura::SpellAuraModRangedAttackPower,//SPELL_AURA_MOD_RANGED_ATTACK_POWER = 124,
		&Aura::SpellAuraModMeleeDamageTaken,//SPELL_AURA_MOD_MELEE_DAMAGE_TAKEN = 125,
		&Aura::SpellAuraModMeleeDamageTakenPct,//SPELL_AURA_MOD_MELEE_DAMAGE_TAKEN_PCT = 126,
		&Aura::SpellAuraRAPAttackerBonus,//SPELL_AURA_RANGED_ATTACK_POWER_ATTACKER_BONUS = 127,
		&Aura::SpellAuraModPossessPet,//SPELL_AURA_MOD_POSSESS_PET = 128,
		&Aura::SpellAuraModIncreaseSpeedAlways,//SPELL_AURA_MOD_INCREASE_SPEED_ALWAYS = 129,
		&Aura::SpellAuraModIncreaseMountedSpeed,//SPELL_AURA_MOD_MOUNTED_SPEED_ALWAYS = 130,
		&Aura::SpellAuraModCreatureRangedAttackPower,//SPELL_AURA_MOD_CREATURE_RANGED_ATTACK_POWER = 131,
		&Aura::SpellAuraModIncreaseEnergyPerc,//SPELL_AURA_MOD_INCREASE_ENERGY_PERCENT = 132,
		&Aura::SpellAuraModIncreaseMaxHealthPerc,//SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT = 133,
		&Aura::SpellAuraModManaRegInterrupt,//SPELL_AURA_MOD_MANA_REGEN_INTERRUPT = 134,
		&Aura::SpellAuraModHealingDone,//SPELL_AURA_MOD_HEALING_DONE = 135,
		&Aura::SpellAuraModHealingDonePct,//SPELL_AURA_MOD_HEALING_DONE_PERCENT = 136,
		&Aura::SpellAuraModTotalStatPerc,//SPELL_AURA_MOD_TOTAL_STAT_PERCENTAGE = 137,
		&Aura::SpellAuraModHaste,//SPELL_AURA_MOD_HASTE = 138,
		&Aura::SpellAuraForceReaction,//SPELL_AURA_FORCE_REACTION = 139,
		&Aura::SpellAuraModRangedHaste,//SPELL_AURA_MOD_RANGED_HASTE = 140,
		&Aura::SpellAuraModRangedAmmoHaste,//SPELL_AURA_MOD_RANGED_AMMO_HASTE = 141,
		&Aura::SpellAuraModBaseResistancePerc,//SPELL_AURA_MOD_BASE_RESISTANCE_PCT = 142, also called as resistance from items
		&Aura::SpellAuraModResistanceExclusive,//SPELL_AURA_MOD_RESISTANCE_EXCLUSIVE = 143,
		&Aura::SpellAuraSafeFall,//SPELL_AURA_SAFE_FALL = 144,
		&Aura::SpellAuraNULL,//SPELL_AURA_CHARISMA = 145,//obselete?
		&Aura::SpellAuraNULL,//SPELL_AURA_PERSUADED = 146,//obselete
		&Aura::SpellAuraCrowdControlImmunity,//SPELL_AURA_ADD_CONTROL_IMMUNITY = 147,//http://wow.allakhazam.com/db/spell.html?wspell=36798
		&Aura::SpellAuraRetainComboPoints,//SPELL_AURA_RETAIN_COMBO_POINTS = 148,
		&Aura::SpellAuraResistPushback,//SPELL_AURA_RESIST_PUSHBACK = 149,//	Resist Pushback //Simply resist spell casting delay
		&Aura::SpellAuraModShieldBlockPCT,//SPELL_AURA_MOD_SHIELD_BLOCK_PCT = 150,//	Mod Shield Absorbed dmg %
		&Aura::SpellAuraTrackStealthed,//SPELL_AURA_TRACK_STEALTHED = 151,//	Track Stealthed
		&Aura::SpellAuraModDetectedRange,//SPELL_AURA_MOD_DETECTED_RANGE = 152,//	Mod Detected Range
		&Aura::SpellAuraSplitDamageFlat,//SPELL_AURA_SPLIT_DAMAGE_FLAT= 153,//	Split Damage Flat
		&Aura::SpellAuraModStealthLevel,//SPELL_AURA_MOD_STEALTH_LEVEL = 154,//	Stealth Level Modifier
		&Aura::SpellAuraModUnderwaterBreathing,//SPELL_AURA_MOD_WATER_BREATHING = 155,//	Mod Water Breathing
		&Aura::SpellAuraModReputationAdjust,//SPELL_AURA_MOD_REPUTATION_ADJUST = 156,//	Mod Reputation Gain
		&Aura::SpellAuraNULL,//SPELL_AURA_PET_DAMAGE_MULTI = 157,//	Mod Pet Damage
		&Aura::SpellAuraModBlockValue,//SPELL_AURA_MOD_SHIELD_BLOCKVALUE = 158//used Apply Aura: Mod Shield Block //http://www.thottbot.com/?sp=25036
		&Aura::SpellAuraNoPVPCredit,//missing = 159 //used Apply Aura: No PVP Credit http://www.thottbot.com/?sp=2479
		&Aura::SpellAuraNULL,//missing = 160 //Apply Aura: Mod Side/Rear PBAE Damage Taken %//used http://www.thottbot.com/?sp=23198
		&Aura::SpellAuraModHealthRegInCombat,//SPELL_AURA_MOD_HEALTH_REGEN_IN_COMBAT = 161,
		&Aura::SpellAuraPowerBurn,//missing = 162 //used //Apply Aura: Power Burn (Mana) //http://www.thottbot.com/?sp=19659
		&Aura::SpellAuraModCritDmgSchool,//missing = 163 //Apply Aura: Mod Crit Damage Bonus (Physical)
		&Aura::SpellAuraNULL,//missing = 164 //used //test spell
		&Aura::SpellAuraAPAttackerBonus,//SPELL_AURA_MELEE_ATTACK_POWER_ATTACKER_BONUS = 165,	// Melee AP Attacker Bonus
		&Aura::SpellAuraModAttackPowerPct,//SPELL_AURA_MOD_ATTACK_POWER_PCT = 166 //used //Apply Aura: Mod Attack Power % // http://www.thottbot.com/?sp=30803
		&Aura::SpellAuraModRangedAttackPowerPct,//missing = 167 //http://www.thottbot.com/s34485
		&Aura::SpellAuraIncreaseDamageTypePCT,//missing = 168 //used //Apply Aura: Increase Damage % *type* //http://www.thottbot.com/?sp=24991
		&Aura::SpellAuraIncreaseCricticalTypePCT,//missing = 169 //used //Apply Aura: Increase Critical % *type* //http://www.thottbot.com/?sp=24293
		&Aura::SpellAuraNULL,//missing = 170 //used //Apply Aura: Detect Amore //http://www.thottbot.com/?sp=26802
		&Aura::SpellAuraIncreasePartySpeed,//missing = 171
		&Aura::SpellAuraIncreaseMovementAndMountedSpeed,//missing = 172 //used //Apply Aura: Increase Movement and Mounted Speed (Non-Stacking) //http://www.thottbot.com/?sp=26022 2e effect
		&Aura::SpellAuraNULL,//missing = 173 // Apply Aura: Allow Chperiodion Spells
		&Aura::SpellAuraIncreaseSpellDamageByAttribute,//SPELL_AURA_INCREASE_SPELL_DAMAGE_FROM_STAT_PCT = 174 //used //Apply Aura: Increase Spell Damage by % Spirit (Spells) //http://www.thottbot.com/?sp=15031
		&Aura::SpellAuraIncreaseHealingByAttribute,//SPELL_AURA_INCREASE_SPELL_HEALING_ATTRIBUTE = 175 //used //Apply Aura: Increase Spell Healing by % Spirit //http://www.thottbot.com/?sp=15031
		&Aura::SpellAuraSpiritOfRedemption,//missing = 176 //used // Apply Aura: Spirit of Redemption
		&Aura::SpellAuraModCharm,//missing = 177 //used //Apply Aura: Area Charm // http://www.thottbot.com/?sp=26740
		&Aura::SpellAuraNULL,//missing = 178 //Apply Aura: Increase Debuff Resistance
		&Aura::SpellAuraIncreaseAttackerSpellCrit,//SPELL_AURA_INCREASE_ATTACKER_SPELL_CRIT//Apply Aura: Increase Attacker Spell Crit % *type* //http://www.thottbot.com/?sp=12579
		&Aura::SpellAuraModCratureDmgDone,//missing = 180 //used //Apply Aura: Increase Spell Damage *type* //http://www.thottbot.com/?sp=29113
		&Aura::SpellAuraNULL,//missing = 181
		&Aura::SpellAuraIncreaseArmorByPctInt,//missing = 182 //used //Apply Aura: Increase Armor by % of Intellect //http://www.thottbot.com/?sp=28574  SPELL_AURA_INC_ARMOR_BY_PCT_INT
		&Aura::SpellAuraNULL,//missing = 183 //used //Apply Aura: Decrease Critical Threat by % (Spells) //http://www.thottbot.com/?sp=28746
		&Aura::SpellAuraReduceAttackerMHitChance,//SPELL_AURA_DECREASE_ATTACKER_CHANCE_TO_HIT_MELEE 184//Apply Aura: Reduces Attacker Chance to Hit with Melee //http://www.thottbot.com/s31678
		&Aura::SpellAuraReduceAttackerRHitChance,//SPELL_AURA_DECREASE_ATTACKER_CHANGE_TO_HIT_RANGED 185//Apply Aura: Reduces Attacker Chance to Hit with Ranged //http://www.thottbot.com/?sp=30895
		&Aura::SpellAuraReduceAttackerSHitChance,//SPELL_AURA_DECREASE_ATTACKER_CHANGE_TO_HIT_SPELLS 186//Apply Aura: Reduces Attacker Chance to Hit with Spells (Spells) //http://www.thottbot.com/?sp=30895
		&Aura::SpellAuraReduceEnemyMCritChance,//SPELL_AURA_DECREASE_ATTACKER_CHANGE_TO_CRIT_MELEE = 187 //used //Apply Aura: Reduces Attacker Chance to Crit with Melee (Ranged?) //http://www.thottbot.com/?sp=30893
		&Aura::SpellAuraReduceEnemyRCritChance,//SPELL_AURA_DECREASE_ATTACKER_CHANGE_TO_CRIT_RANGED = 188 //used //Apply Aura: Reduces Attacker Chance to Crit with Ranged (Melee?) //http://www.thottbot.com/?sp=30893
		&Aura::SpellAuraIncreaseRating,//missing = 189 //Apply Aura: Increases Rating
		&Aura::SpellAuraIncreaseRepGainPct,//SPELL_AURA_MOD_FACTION_REPUTATION_GAIN //used // Apply Aura: Increases Reputation Gained by % //http://www.thottbot.com/?sp=30754
		&Aura::SpellAuraLimitSpeed,//missing = 191 //used // noname //http://www.thottbot.com/?sp=29894
		&Aura::SpellAuraMeleeHaste,//192 SPELL_AURA_MELEE_SLOW_PCT %
//		&Aura::SpellAuraIncreaseTimeBetweenAttacksPCT,//193 Apply Aura: Increase Time Between Attacks (Melee, Ranged and Spell) by %
		&Aura::SpellAuraModHasteAndCastSpeed,//193 Apply Aura: attack and cast speed
		&Aura::SpellAuraNULL,//194 //&Aura::SpellAuraIncreaseSpellDamageByInt,//194 Apply Aura: Increase Spell Damage by % of Intellect (All)
		&Aura::SpellAuraNULL,//195 //&Aura::SpellAuraIncreaseHealingByInt,//195 Apply Aura: Increase Healing by % of Intellect
		&Aura::SpellAuraNULL,//196 Apply Aura: Mod All Weapon Skills (6)
		&Aura::SpellAuraModAttackerCritChance,//SPELL_AURA_REDUCE_ATTACKER_CRICTICAL_HIT_CHANCE_PCT 197 Apply Aura: Reduce Attacker Critical Hit Chance by %
		&Aura::SpellAuraIncreaseAllWeaponSkill,//198
		&Aura::SpellAuraIncreaseHitRate,//199 Apply Aura: Increases Spell % To Hit (Fire, Nature, Frost)
		&Aura::SpellAuraIncreaseExpGainPct,//200 // Increases experience earned by $s1%.  Lasts $d.
		&Aura::SpellAuraNULL,//201 - root or unroot or add flyight abillity. More like reduce gravity
		&Aura::SpellAuraFinishingMovesCannotBeDodged,//202 // Finishing moves cannot be dodged - 32601, 44452
		&Aura::SpellAuraReduceCritMeleeAttackDmg,//203 Apply Aura: Reduces Attacker Critical Hit Damage with Melee by %
		&Aura::SpellAuraReduceCritRangedAttackDmg,//204 Apply Aura: Reduces Attacker Critical Hit Damage with Ranged by %
		&Aura::SpellAuraNULL,//205 // SPELL_AURA_MOD_ATTACKER_SPELL_CRIT_DAMAGE
		&Aura::SpellAuraEnableFlight,//SPELL_AURA_ENABLE_FLIGHT1 206 // Take flight on a worn old carpet. - Spell 43343
		&Aura::SpellAuraEnableFlight,//SPELL_AURA_ENABLE_FLIGHT2 207 set fly
		&Aura::SpellAuraEnableFlightWithUnmountedSpeed,//SPELL_AURA_ENABLE_FLIGHT_NO_SPEED 208
		&Aura::SpellAuraModMountedSpeedPCT,//209  // 
		&Aura::SpellAuraModFlyghtSpeedPCT,//210	// 
		&Aura::SpellAuraIncreaseFlightSpeed,//211
		&Aura::SpellAuraIncreaseRangedAPStatPCT,//SPELL_AURA_MOD_RANGED_ATTACK_POWER_OF_INTELLECT //212 Apply Aura: Increase Ranged Atk Power by % of Intellect
		&Aura::SpellAuraIncreaseRageFromDamageDealtPCT, //213 Apply Aura: Increase Rage from Damage Dealt by %
		&Aura::SpellAuraNULL,//214 // Tamed Pet Passive (DND)
		&Aura::SpellAuraRemoveReagentCost,//215 // arena preparation buff - cancel soul shard requirement?
		&Aura::SpellAuraModCastingSpeed,//216 Increases casting time %, reuse existing handler...
		&Aura::SpellAuraNULL,//217 // not used
		&Aura::SpellAuraNULL,//218 // increases time between ranged attacks
		&Aura::SpellAuraRegenManaStatPCT,//219 SPELL_AURA_REGEN_MANA_STAT_PCT Regenerate mana equal to $s1% of your Intellect every 5 sec, even while casting
		&Aura::SpellAuraRatingBasedOnStat,//SPELL_AURA_RATING_FROM_STAT=220 Increases your rating by %of stat
		&Aura::SpellAuraNULL,//221 Detaunt "Ignores an enemy, forcing the caster to not attack it unless there is no other target nearby. When the effect wears off, the creature will attack the most threatening target."
		&Aura::SpellAuraNULL,//222 // not used
		&Aura::SpellAuraNULL,//223 // used in one spell, cold stare 43593
		&Aura::SpellAuraNULL,//224 // not used
		&Aura::SpellAuraNULL,//225 // Prayer of Mending "Places a spell on the target that heals them for $s1 the next time they take damage.  When the heal occurs, Prayer of Mending jumps to a raid member within $a1 yards.  Jumps up to $n times and lasts $d after each jump.  This spell can only be placed on one target at a time."
		&Aura::SpellAuraPeriodicDummy,//SPELL_AURA_DRINK_NEW 226 // used in brewfest spells, headless hoerseman, Aspect of the Viper
		&Aura::SpellAuraPeriodicTriggerSpellWithValue,//227 Inflicts [SPELL DAMAGE] damage to enemies in a cone in front of the caster. (based on combat range) http://www.thottbot.com/s40938. Mind flay also does periodic damage to target
		&Aura::SpellAuraModStealthDetection,//228 Stealth Detection. http://www.thottbot.com/s34709
		&Aura::SpellAuraModAOEDmgTakenPct,//229 SPELL_AURA_MOD_AOE_DMG_PCT Apply Aura:Reduces the damage taken from area of effect attacks http://www.thottbot.com/s35694
		&Aura::SpellAuraIncreaseMaxHealth,//230 SPELL_AURA_INCREASE_MAX_HEALTH Increase Max Health (commanding shout);
        &Aura::SpellAuraProcTriggerSpell,//231 - proc some action by chance
        &Aura::SpellAuraReduceEffectDuration,//232 SPELL_AURA_MOD_MECHANIC_DURATION_PCT // Reduces duration of Magic effects by $s2%.
        &Aura::SpellAuraDummy,//233 // Beer Goggles - see every unmodeled humanoid with display id X
        &Aura::SpellAuraReduceEffectDuration,//234 SPELL_AURA_MOD_REDUCE_MECHANIC_DURATION Apply Aura: Reduces Silence or Interrupt effects, Item spell magic http://www.thottbot.com/s42184
		&Aura::SpellAuraNULL,//235 33206 Instantly reduces a friendly target's threat by $44416s1%, reduces all damage taken by $s1% and increases resistance to Dispel mechanics by $s2% for $d.
		&Aura::HandleAuraControlVehicle,                        //236 SPELL_AURA_CONTROL_VEHICLE
		&Aura::SpellAuraModHealingByAP,//237 SPELL_AURA_INCREASE_HEALING_POWER_FROM_ATTACKPOWER_PCT	//increase spell healing by X pct from attack power
		&Aura::SpellAuraModSpellDamageByAP,//238 SPELL_AURA_INCREASE_SPELL_POWER_FROM_ATTACKPOWER_PCT	//increase spell dmg by X pct from attack power
		&Aura::SpellAuraModScale,//239 - mod scale pct
		&Aura::SpellAuraExpertieseModifier,//240 Increase Expertiese X
		&Aura::SpellAuraModForceMoveForward,//241 SPELL_AURA_FORCE_MOVE_FORWARD - camera control
		&Aura::SpellAuraNULL,//242 - spell damage + healing from intelect
		&Aura::SpellAuraNULL,//243 - mod faction standing
		&Aura::SpellAuraModComprehendLanguage,//244 SPELL_AURA_COMPREHEND_LANGUAGE - language ?
		&Aura::SpellAuraModOffensiveMagicalDurationPCT,//245 - reduce duration of spell mechanics (magical harmfull effects)
		&Aura::SpellAuraReduceEffectDurationDispelType,//246 - SPELL_AURA_MOD_AURA_DURATION_DISPELTYPE
		&Aura::SpellAuraDummy,//247 - target clone or share health the caster
		&Aura::SpellAuraNULL,//248 - reduce chance for target to dodge
		&Aura::SpellAuraConvertRune,//SPELL_AURA_CONVERT_RUNE 249 - seems to be for some power conversion but it does not seem to be required anywhere 
		&Aura::SpellAuraAddHealth,//250 SPELL_AURA_MOD_INCREASE_HEALTH_2 - increase max health and health by flat value
		&Aura::SpellAuraNULL,//251
		&Aura::SpellAuraNULL,//252 - SPELL_AURA_MOD_ATTACK_CAST_SPEED_PCT
		&Aura::SpellAuraModCritBlockChance,//253 - SPELL_AURA_MOD_CRIT_BLOCK_CHANCE_PCT
		&Aura::SpellAuraModDisarmOffhand,//254 - SPELL_AURA_MOD_DISARM_OFFHAND
		&Aura::SpellAuraModDMGTakenMechanic,//255 - SPELL_AURA_MOD_DMG_TAKEN_MECHANIC_PCT	- mod spell value if mechanics match and sometimes spell name
		&Aura::SpellAuraRemoveReagentCost,//256 - mod reagent cost
		&Aura::SpellAuraNULL,//257
		&Aura::SpellAuraNULL,//258
		&Aura::SpellAuraNULL,//259 - add spell effect
		&Aura::SpellAuraDummy,//260 - visual or scripted
		&Aura::SpellAuraDummy,//261 - realm shift (like ghost world)
		&Aura::SpellAuraModIgnoreState,//262 - SPELL_AURA_REMOVE_CAST_CONDITION ignore / set caster / target aura state/stance
		&Aura::SpellAuraModNoCast,//263 - SPELL_AURA_MOD_NOCAST
		&Aura::SpellAuraModRegisterCanCastScript,//264 SPELL_AURA_MOD_CANCAST_SCRIPT_ADD = 264
		&Aura::SpellAuraNULL,//265
		&Aura::SpellAuraNULL,//266 
		&Aura::SpellAuraAuraSchoolImmunity,//267 - SPELL_AURA_MOD_DEBUFF_IMMUNITY - !! only 1 example so far
		&Aura::SpellAuraModAPBasedOnStat,//268 - AP pct based on stat x
		&Aura::SpellAuraNULL,//269	//damage reduction for school
		&Aura::SpellAuraNULL,//270
		&Aura::SpellAuraModDamagePercTaken,//271 target inc school dmg taken from caster by x% / SMT_EFFECT_TARGET
		&Aura::SpellAuraNULL,//272 
		&Aura::SpellAuraNULL,//273 - scripted
		&Aura::SpellAuraNULL,//274
		&Aura::SpellAuraIgnoreShapeshift,//275 - ignore casteraura requirements
		&Aura::SpellAuraNULL,//276
		&Aura::SpellAuraNULL,//277 - SM target count
		&Aura::SpellAuraModDisarmRanged,//278 - SPELL_AURA_MOD_DISARM_RANGED
		&Aura::SpellAuraNULL,//279 - clone caster by target
		&Aura::SpellAuraModIgnoreArmorForWeapon,//280 attack with item type ignore armor
		&Aura::SpellAuraModHonorGainPCT,//281 honor gained from faction
		&Aura::SpellAuraModIncreaseMaxHealthPerc,//282 SPELL_AURA_MOD_HEALTHPCT base health by x%
		&Aura::SpellAuraNULL,//283
		&Aura::SpellAuraNULL,//284 - seems to be scripted, sometimes it is proc,negate ability,summon
		&Aura::SpellAuraIncreaseAPBasedOnArmor,//285		
		&Aura::SpellAuraNULL,//286 - DOT ability to crit
		&Aura::SpellAuraModDeflectChance,//287
		&Aura::SpellAuraNULL,//288
		&Aura::SpellAuraNULL,//289
		&Aura::SpellAuraModCritPercAll,//290 SPELL_AURA_MOD_CRIT_CHANCE_ALL - mod crit chance all
		&Aura::SpellAuraNULL,//291
		&Aura::SpellAuraOpenStableUI,//292 - items that let you use stable pet interface from anywhere
		&Aura::SpellAuraNULL,//293
		&Aura::SpellAuraNULL,//294
		&Aura::SpellAuraNULL,//295
		&Aura::SpellAuraNULL,//296
		&Aura::SpellAuraNULL,//297
		&Aura::SpellAuraNULL,//298
		&Aura::SpellAuraNULL,//299
		&Aura::SpellAuraMirrorDMG,//300
		&Aura::SpellAuraModHealAbsorb,//301	SPELL_AURA_MOD_HEAL_ABSORB 5 spells
		&Aura::SpellAuraNULL,//302
		&Aura::SpellAuraNULL,//303 - mod damage based on condition
		&Aura::SpellAuraNULL,//304
		&Aura::SpellAuraNULL,//305
		&Aura::SpellAuraNULL,//306
		&Aura::SpellAuraNULL,//307 mod spell crit chance based if target has this aura. Needs scripting
		&Aura::SpellAuraModCasterCritChanceOnTargetOfSpells,//SPELL_AURA_MOD_CASTER_CRIT_CHANCE_ON_TARGET_OF_SPELLS = 308
		&Aura::SpellAuraNULL,//309
		&Aura::SpellAuraModAOEDmgTakenPctFromCreature,//310	SPELL_AURA_MOD_PET_AOE_DAMAGE_AVOIDANCE
		&Aura::SpellAuraNULL,//311
		&Aura::SpellAuraNULL,//312
		&Aura::SpellAuraNULL,//313
		&Aura::SpellAuraNULL,//314
		&Aura::SpellAuraNULL,//315
		&Aura::SpellAuraNULL,//316
		&Aura::SpellAuraModSpellPowerPCT,//317 SPELL_AURA_MOD_SPELL_POWER_PCT
		&Aura::SpellAuraModMastery,//SPELL_AURA_MOD_MASTERY = 318
		&Aura::SpellAuraModAttackSpeedNoRegen,//SPELL_AURA_MOD_HASTE_NOREGEN = 319 - seems to be haste again - This one should not mod energy regen !
		&Aura::SpellAuraModRangedHaste,//320 SPELL_AURA_MOD_RANGED_HASTE - seems to be ranged haste again
		&Aura::SpellAuraNULL,//321
		&Aura::SpellAuraModRangedTargetting,//322 SPELL_AURA_MOD_RANGED_TARGETTING ! not sure !	- targeting related, cannot target in specific situations
		&Aura::SpellAuraNULL,//323
		&Aura::SpellAuraNULL,//324
		&Aura::SpellAuraNULL,//325
		&Aura::SpellAuraNULL,//326
		&Aura::SpellAuraNULL,//327
		&Aura::SpellAuraNULL,//328
		&Aura::SpellAuraModRunicPowerRegenPCT,//SPELL_AURA_MOD_RUNIC_POWER_GAIN_PCT = 329. Only 1 example found
		&Aura::SpellAuraModIgnoreInterrupt,//330	//SPELL_AURA_MOD_IGNORE_INTERRUPT
		&Aura::SpellAuraNULL,//331
//		&Aura::SpellAuraModReplaceSpell,//332	SPELL_AURA_MOD_REPLACE_SPELL	
		&Aura::SpellAuraModAddTemporarySpell,//332	SPELL_AURA_MOD_REPLACE_SPELL	
		&Aura::SpellAuraModAddTemporarySpell,//333	SPELL_AURA_MOD_ADD_TEMPORARY_SPELL //enable spell casting client side
		&Aura::SpellAuraNULL,//334
		&Aura::SpellAuraNULL,//335
		&Aura::SpellAuraNULL,//336
		&Aura::SpellAuraNULL,//337
		&Aura::SpellAuraNULL,//338
		&Aura::SpellAuraNULL,//339
		&Aura::SpellAuraNULL,//340
		&Aura::SpellAuraNULL,//341
		&Aura::SpellAuraModAttackSpeedNoRegen,//SPELL_AURA_MOD_HASTE - seems to be haste again
		&Aura::SpellAuraNULL,//343
		&Aura::SpellAuraModAutoAttackDmg,//SPELL_AURA_MOD_AUTO_ATTACK_DMG_PCT
		&Aura::SpellAuraModDmgPenetrate,//SPELL_AURA_MOD_DMG_PENETRATE - 345 x% of the dmg ignores any armor
		&Aura::SpellAuraNULL,//346
		&Aura::SpellAuraDummy,//347 - Some kinda haste effect Have 2 spells for it : Haste effects lower the cooldown of your Crusader Strike and Divine Storm abilities.
		&Aura::SpellAuraNULL,//348
		&Aura::SpellAuraNULL,//349
		&Aura::SpellAuraNULL,//350
		&Aura::SpellAuraNULL,//351
		&Aura::SpellAuraNULL,//352
		&Aura::SpellAuraNULL,//353
		&Aura::SpellAuraNULL,//354
		&Aura::SpellAuraNULL,//355
		&Aura::SpellAuraNULL,//356
		&Aura::SpellAuraNULL,//357
		&Aura::SpellAuraNULL,//358
		&Aura::SpellAuraNULL,//359
		&Aura::SpellAuraNULL,//360
		&Aura::SpellAuraProcTriggerSpell,// 361 SPELL_AURA_PROC_TRIGGER_SPELL = 42,  - Melee strikes deal significant Fire damage.
		&Aura::SpellAuraNULL,//362
		&Aura::SpellAuraNULL,//363
		&Aura::SpellAuraNULL,//364
		&Aura::SpellAuraNULL,//365
		&Aura::SpellAuraModAPToSPExclusive,//SPELL_AURA_MOD_AP_TO_SP_EXCLUSIVE=366
};

char* SpellAuraNames[TOTAL_SPELL_AURAS] = {
    "NONE",												//   0 None
    "BIND_SIGHT",										//   1 Bind Sight
    "MOD_POSSESS",										//   2 Mod Possess
    "PERIODIC_DAMAGE",									//   3 Periodic Damage
    "DUMMY",											//   4 Script Aura
    "MOD_CONFUSE",										//   5 Mod Confuse
    "MOD_CHARM",										//   6 Mod Charm
    "MOD_FEAR",											//   7 Mod Fear
    "PERIODIC_HEAL",									//   8 Periodic Heal
    "MOD_ATTACKSPEED",									//   9 Mod Attack Speed
    "MOD_THREAT_SCHOOL_PCT",							//  10 Mod Threat
    "MOD_TAUNT",										//  11 Taunt
    "MOD_STUN",											//  12 Stun
    "MOD_DAMAGE_DONE",									//  13 Mod Damage Done
    "MOD_DAMAGE_TAKEN",									//  14 Mod Damage Taken
    "DAMAGE_SHIELD",									//  15 Damage Shield
    "MOD_STEALTH",										//  16 Mod Stealth
    "MOD_DETECT",										//  17 Mod Detect
    "MOD_INVISIBILITY",									//  18 Mod Invisibility
    "MOD_INVISIBILITY_DETECTION",						//  19 Mod Invisibility Detection
    "MOD_TOTAL_HEALTH_REGEN_PCT",						//  20
    "MOD_TOTAL_MANA_REGEN_PCT",							//  21
    "MOD_RESISTANCE",									//  22 Mod Resistance
    "PERIODIC_TRIGGER_SPELL",							//  23 Periodic Trigger
    "PERIODIC_ENERGIZE",								//  24 Periodic Energize
    "MOD_PACIFY",										//  25 Pacify
    "MOD_ROOT",											//  26 Root
    "MOD_SILENCE",										//  27 Silence
    "REFLECT_SPELLS",									//  28 Reflect Spells %
    "MOD_STAT",											//  29 Mod Stat
    "MOD_SKILL",										//  30 Mod Skill
    "MOD_INCREASE_SPEED",								//  31 Mod Speed
    "MOD_INCREASE_MOUNTED_SPEED",						//  32 Mod Speed Mounted
    "MOD_DECREASE_SPEED",								//  33 Mod Speed Slow
    "MOD_INCREASE_HEALTH",								//  34 Mod Increase Health
    "MOD_INCREASE_ENERGY",								//  35 Mod Increase Energy
    "MOD_SHAPESHIFT",									//  36 Shapeshift
    "EFFECT_IMMUNITY",									//  37 Immune Effect
    "STATE_IMMUNITY",									//  38 Immune State
    "SCHOOL_IMMUNITY",									//  39 Immune School    
    "DAMAGE_IMMUNITY",									//  40 Immune Damage
    "DISPEL_IMMUNITY",									//  41 Immune Dispel Type
    "PROC_TRIGGER_SPELL",								//  42 Proc Trigger Spell
    "PROC_TRIGGER_DAMAGE",								//  43 Proc Trigger Damage
    "TRACK_CREATURES",									//  44 Track Creatures
    "TRACK_RESOURCES",									//  45 Track Resources
    "MOD_PARRY_SKILL",									//  46 Mod Parry Skill
    "MOD_PARRY_PERCENT",								//  47 Mod Parry Percent
    "MOD_DODGE_SKILL",									//  48 Mod Dodge Skill
    "MOD_DODGE_PERCENT",								//  49 Mod Dodge Percent  
    "MOD_BLOCK_SKILL",									//  50 Mod Block Skill
    "MOD_BLOCK_PERCENT",								//  51 Mod Block Percent
    "MOD_CRIT_PERCENT",									//  52 Mod Crit Percent
    "PERIODIC_LEECH",									//  53 Periodic Leech
    "MOD_HIT_CHANCE",									//  54 Mod Hit Chance
    "MOD_SPELL_HIT_CHANCE",								//  55 Mod Spell Hit Chance
    "TRANSFORM",										//  56 Transform
    "MOD_SPELL_CRIT_CHANCE",							//  57 Mod Spell Crit Chance
    "MOD_INCREASE_SWIM_SPEED",							//  58 Mod Speed Swim
    "MOD_DAMAGE_DONE_CREATURE",							//  59 Mod Creature Dmg Done   
    "MOD_PACIFY_SILENCE",								//  60 Pacify & Silence
    "MOD_SCALE",										//  61 Mod Scale
    "PERIODIC_HEALTH_FUNNEL",							//  62 Periodic Health Funnel
    "PERIODIC_MANA_FUNNEL",								//  63 Periodic Mana Funnel
    "PERIODIC_MANA_LEECH",								//  64 Periodic Mana Leech
    "MOD_CASTING_SPEED",								//  65 Haste - Spells
    "FEIGN_DEATH",										//  66 Feign Death
    "MOD_DISARM",										//  67 Disarm
    "MOD_STALKED",										//  68 Mod Stalked
    "SCHOOL_ABSORB",									//  69 School Absorb    
    "EXTRA_ATTACKS",									//  70 Extra Attacks
    "MOD_SPELL_CRIT_CHANCE_SCHOOL",						//  71 Mod School Spell Crit Chance
    "MOD_POWER_COST",									//  72 Mod Power Cost
    "MOD_POWER_COST_SCHOOL",							//  73 Mod School Power Cost
    "REFLECT_SPELLS_SCHOOL",							//  74 Reflect School Spells %
    "MOD_LANGUAGE",										//  75 Mod Language
    "FAR_SIGHT",										//  76 Far Sight
    "MECHANIC_IMMUNITY",								//  77 Immune Mechanic
    "MOUNTED",											//  78 Mounted
    "MOD_DAMAGE_PERCENT_DONE",							//  79 Mod Dmg %   
    "MOD_PERCENT_STAT",									//  80 Mod Stat %
    "SPLIT_DAMAGE",										//  81 Split Damage
    "WATER_BREATHING",									//  82 Water Breathing
    "MOD_BASE_RESISTANCE",								//  83 Mod Base Resistance
    "MOD_REGEN",										//  84 Mod Health Regen
    "MOD_POWER_REGEN",									//  85 Mod Power Regen
    "CHANNEL_DEATH_ITEM",								//  86 Create Death Item
    "MOD_DAMAGE_PERCENT_TAKEN",							//  87 Mod Dmg % Taken
    "MOD_PERCENT_REGEN",								//  88 Mod Health Regen Percent
    "PERIODIC_DAMAGE_PERCENT",							//  89 Periodic Damage Percent   
    "MOD_RESIST_CHANCE",								//  90 Mod Resist Chance
    "MOD_DETECT_RANGE",									//  91 Mod Detect Range
    "PREVENTS_FLEEING",									//  92 Prevent Fleeing
    "MOD_UNATTACKABLE",									//  93 Mod Uninteractible
    "INTERRUPT_REGEN",									//  94 Interrupt Regen
    "GHOST",											//  95 Ghost
    "SPELL_MAGNET",										//  96 Spell Magnet
    "MANA_SHIELD",										//  97 Mana Shield
    "MOD_SKILL_TALENT",									//  98 Mod Skill Talent
    "MOD_ATTACK_POWER",									//  99 Mod Attack Power
    "AURAS_VISIBLE",									// 100 Auras Visible
    "MOD_RESISTANCE_PCT",								// 101 Mod Resistance %
    "MOD_CREATURE_ATTACK_POWER",						// 102 Mod Creature Attack Power
    "MOD_TOTAL_THREAT",									// 103 Mod Total Threat (Fade)
    "WATER_WALK",										// 104 Water Walk
    "FEATHER_FALL",										// 105 Feather Fall
    "HOVER",											// 106 Hover
    "ADD_FLAT_MODIFIER",								// 107 Add Flat Modifier
    "ADD_PCT_MODIFIER",									// 108 Add % Modifier
    "ADD_TARGET_TRIGGER",								// 109 Add Class Target Trigger    
    "MOD_POWER_REGEN_PERCENT",							// 110 Mod Power Regen %
    "ADD_CASTER_HIT_TRIGGER",							// 111 Add Class Caster Hit Trigger
    "OVERRIDE_CLASS_SCRIPTS",							// 112 Override Class Scripts
    "MOD_RANGED_DAMAGE_TAKEN",							// 113 Mod Ranged Dmg Taken
    "MOD_RANGED_DAMAGE_TAKEN_PCT",						// 114 Mod Ranged % Dmg Taken
    "MOD_HEALING",										// 115 Mod Healing
    "IGNORE_REGEN_INTERRUPT",							// 116 Regen During Combat
    "MOD_MECHANIC_RESISTANCE",							// 117 Mod Mechanic Resistance
    "MOD_HEALING_PCT",									// 118 Mod Healing %
    "SHARE_PET_TRACKING",								// 119 Share Pet Tracking    
    "UNTRACKABLE",										// 120 Untrackable
    "EMPATHY",											// 121 Empathy (Lore, whatever)
    "MOD_OFFHAND_DAMAGE_PCT",							// 122 Mod Offhand Dmg %
    "MOD_POWER_COST_PCT",								// 123 Mod Power Cost % --> armor penetration & spell penetration
    "MOD_RANGED_ATTACK_POWER",							// 124 Mod Ranged Attack Power
    "MOD_MELEE_DAMAGE_TAKEN",							// 125 Mod Melee Dmg Taken
    "MOD_MELEE_DAMAGE_TAKEN_PCT",						// 126 Mod Melee % Dmg Taken
    "RANGED_ATTACK_POWER_ATTACKER_BONUS",				// 127 Rngd Atk Pwr Attckr Bonus
    "MOD_POSSESS_PET",									// 128 Mod Possess Pet
    "MOD_INCREASE_SPEED_ALWAYS",						// 129 Mod Speed Always   
    "MOD_MOUNTED_SPEED_ALWAYS",							// 130 Mod Mounted Speed Always
    "MOD_CREATURE_RANGED_ATTACK_POWER",					// 131 Mod Creature Ranged Attack Power
    "MOD_INCREASE_ENERGY_PERCENT",						// 132 Mod Increase Energy %
    "MOD_INCREASE_HEALTH_PERCENT",						// 133 Mod Max Health %
    "MOD_MANA_REGEN_INTERRUPT",							// 134 Mod Interrupted Mana Regen
    "MOD_HEALING_DONE",									// 135 Mod Healing Done
    "MOD_HEALING_DONE_PERCENT",							// 136 Mod Healing Done %
    "MOD_TOTAL_STAT_PERCENTAGE",						// 137 Mod Total Stat %
    "MOD_HASTE",										// 138 Haste - Melee
    "FORCE_REACTION",									// 139 Force Reaction    
    "MOD_RANGED_HASTE",									// 140 Haste - Ranged
    "MOD_RANGED_AMMO_HASTE",							// 141 Haste - Ranged (Ammo Only)
    "MOD_BASE_RESISTANCE_PCT",							// 142 Mod Base Resistance %
    "MOD_RESISTANCE_EXCLUSIVE",							// 143 Mod Resistance Exclusive
    "SAFE_FALL",										// 144 Safe Fall
    "CHARISMA",											// 145 Charisma
    "PERSUADED",										// 146 Persuaded
    "SPELL_AURA_ADD_CONTROL_IMMUNITY",					// 147 Add Creature Immunity
    "RETAIN_COMBO_POINTS",								// 148 Retain Combo Points
    "RESIST_PUSHBACK",									// 149 Resist Pushback
    "MOD_SHIELD_BLOCK_PCT",								// 150 Mod Shield Block %
    "TRACK_STEALTHED",									// 151 Track Stealthed
    "MOD_DETECTED_RANGE",								// 152 Mod Detected Range
    "SPLIT_DAMAGE_FLAT",								// 153 Split Damage Flat
    "MOD_STEALTH_LEVEL",								// 154 Stealth Level Modifier
    "MOD_WATER_BREATHING",								// 155 Mod Water Breathing
    "MOD_REPUTATION_ADJUST",							// 156 Mod Reputation Gain
    "PET_DAMAGE_MULTI",									// 157 Mod Pet Damage
    "MOD_SHIELD_BLOCK",									// 158 Mod Shield Block
    "NO_PVP_CREDIT",									// 159 No PVP Credit 
    "MOD_SIDE_REAR_PDAE_DAMAGE_TAKEN",					// 160 Mod Side/Rear PBAE Damage Taken 
    "MOD_HEALTH_REGEN_IN_COMBAT",						// 161 Mod Health Regen In Combat
    "POWER_BURN",										// 162 Power Burn 
    "MOD_CRIT_DAMAGE_BONUS_MELEE",						// 163 Mod Critical Damage Bonus (Physical)
	"",													// 164
    "MELEE_ATTACK_POWER_ATTACKER_BONUS",				// 165 Melee AP Attacker Bonus
    "MOD_ATTACK_POWER_PCT",								// 166 Mod Attack Power
    "MOD_RANGED_ATTACK_POWER_PCT",						// 167 Mod Ranged Attack Power %
    "INCREASE_DAMAGE",									// 168 Increase Damage Type
    "INCREASE_CRITICAL",								// 169 Increase Critical Type
    "DETECT_AMORE",										// 170 Detect Amore
	"",													// 171
    "INCREASE_MOVEMENT_AND_MOUNTED_SPEED",				// 172 Increase Movement and Mounted Speed (Non-Stacking)
	"",													// 173
    "INCREASE_SPELL_DAMAGE_BY_ATTRIBUTE",				// 174 Increase Spell Damage by % status
    "INCREASE_SPELL_HEALING_BY_ATTRIBUTE",				// 175 Increase Spell Healing by % status
    "SPIRIT_OF_REDEMPTION",								// 176 Spirit of Redemption Auras
    "AREA_CHARM",										// 177 Area Charm 
	"",													// 178
    "INCREASE_ATTACKER_SPELL_CRIT",						// 179 Increase Attacker Spell Crit Type
    "INCREASE_SPELL_DAMAGE_VS_TYPE",					// 180 Increase Spell Damage Type
	"",													// 181
    "INCREASE_ARMOR_BASED_ON_INTELLECT_PCT",			// 182 Increase Armor based on Intellect
    "DECREASE_CRIT_THREAT",								// 183 Decrease Critical Threat by
    "DECREASE_ATTACKER_CHANCE_TO_HIT_MELEE",			// 184 Reduces Attacker Chance to Hit with Melee
    "DECREASE_ATTACKER_CHANGE_TO_HIT_RANGED",			// 185 Reduces Attacker Chance to Hit with Ranged 
    "DECREASE_ATTACKER_CHANGE_TO_HIT_SPELLS",			// 186 Reduces Attacker Chance to Hit with Spells
    "DECREASE_ATTACKER_CHANGE_TO_CRIT_MELEE",			// 187 Reduces Attacker Chance to Crit with Melee (Ranged?)
    "DECREASE_ATTACKER_CHANGE_TO_CRIT_RANGED",			// 188 Reduces Attacker Chance to Crit with Ranged (Melee?)
	"INCREASE_RATING",									// 189
    "INCREASE_REPUTATION",								// 190 Increases reputation from killed creatures
    "SPEED_LIMIT",										// 191 speed limit
    "MELEE_SLOW_PCT",									// 192
    "INCREASE_ATTACK_AND_CAST_SPEED",					// 193
    "INREASE_SPELL_DAMAGE_PCT_OF_INTELLECT",			// 194 NOT USED ANYMORE - 174 used instead
    "IGNORE_ABSORB_FOR_SPELL",							// 195 
    "MOD_ALL_WEAPON_SKILLS",							// 196
    "REDUCE_ATTACKER_CRICTICAL_HIT_CHANCE_PCT",			// 197
	"",													// 198
    "INCREASE_SPELL_HIT_PCT",							// 199
	"SPELL_AURA_MOD_XP_AND_REP_PCT",					// 200
    "CANNOT_BE_DODGED",									// 201
	"FINISHING_MOVES_CANNOT_BE_DODGED",					// 202
    "REDUCE_ATTACKER_CRICTICAL_HIT_DAMAGE_MELEE_PCT",	// 203
    "REDUCE_ATTACKER_CRICTICAL_HIT_DAMAGE_RANGED_PCT",	// 204
	"",													// 205
	"SPELL_AURA_ENABLE_FLIGHT",							// 206
	"SPELL_AURA_ENABLE_FLIGHT",							// 207
	"",													// 208
	"SPELL_AURA_MOD_MOUNTED_SPEED_PCT",					// 209
	"SPELL_AURA_MOD_FLIGHT_SPEED_PCT",					// 210
	"",													// 211
    "INCREASE_RANGED_ATTACK_POWER_PCT_OF_INTELLECT",	// 212
    "INCREASE_RAGE_FROM_DAMAGE_DEALT_PCT",				// 213
	"",													// 214
	"",													// 215
    "INCREASE_CASTING_TIME_PCT",						// 216
	"",													// 217
	"",													// 218
    "REGEN_MANA_STAT_PCT",								// 219
    "SPELL_AURA_RATING_FROM_STAT",						// 220
	"",													// 221
	"",													// 222
	"",													// 223
	"",													// 224
	"",													// 225
	"PERIODIC_DUMMY",									// 226
	"SPELL_AURA_PERIODIC_TRIGGER_SPELL_WITH_VALUE",		// 227
	"",													// 228
	"SPELL_AURA_MOD_AOE_DMG_TAKEN_PCT",					// 229
    "INCREASE_MAX_HEALTH",								// 230 Used by Commanding Shout
	"SPELL_AURA_PROC_TRIGGER_SPELL2",					// 231
	"SPELL_AURA_MOD_MECHANIC_DURATION_PCT",				// 232
	"",													// 233
	"SPELL_AURA_MOD_REDUCE_MECHANIC_DURATION",			// 234
	"",													// 235
	"CONTROL_VEHICLE",									// 236
	"SPELL_AURA_INCREASE_HEALING_POWER_FROM_ATTACKPOWER_PCT",	// 237
	"SPELL_AURA_INCREASE_SPELL_POWER_FROM_ATTACKPOWER_PCT",		// 238
	"MOD_SCALE_PCT",									// 239
	"MODIFY_RATING",									// 240
	"SPELL_AURA_FORCE_MOVE_FORWARD",					// 241
    "MOD_SPELL_DAMAGE_FROM_HEALING",					// 242
    "SPELL_AURA_MOD_FACTION",							// 243
    "SPELL_AURA_COMPREHEND_LANGUAGE",					// 244
    "MOD_DURATION_OF_MAGIC_EFFECTS",					// 245
    "SPELL_AURA_MOD_AURA_DURATION_DISPELTYPE",			// 246
    "APELL_AURA_CLONE_CASTER",							// 247
    "MOD_COMBAT_RESULT_CHANCE",							// 248
    "SPELL_AURA_CONVERT_RUNE",							// 249
    "MOD_INCREASE_HEALTH_2",							// 250
    "MOD_ENEMY_DODGE",									// 251
    "SPELL_AURA_MOD_ATTACK_CAST_SPEED_PCT",				// 252
    "SPELL_AURA_MOD_CRIT_BLOCK_CHANCE_PCT",				// 253
    "SPELL_AURA_MOD_DISARM_OFFHAND",					// 254
    "SPELL_AURA_MOD_DMG_TAKEN_MECHANIC_PCT",			// 255
    "SPELL_AURA_MOD_REAGENT_COST2",						// 256
    "257",												// 257
    "258",												// 258
    "259",												// 259
	"SPELL_AURA_SCREEN_EFFECT",							// 260
	"SPELL_AURA_PHASE ",								// 261
	"SPELL_AURA_REMOVE_CAST_CONDITION",					// 262
	"SPELL_AURA_MOD_NOCAST",							// 263
	"SPELL_AURA_MOD_CANCAST_SCRIPT_ADD",				// 264
	"265",												// 265
	"266",												// 266
	"SPELL_AURA_MOD_SCHOOL_IMMUNITY",					// 267
	"SPELL_AURA_MOD_AP_BY_STAT",						// 268
	"269",												// 269
	"270",												// 270
	"SPELL_AURA_MOD_DAMAGE_TAKEN_PCT",					// 271
	"272",												// 272
	"273",												// 273
	"274",												// 274
	"SPELL_AURA_MOD_IGNORE_SHAPESHIFT",					// 275
	"276",												// 276
	"277",												// 277
	"SPELL_AURA_MOD_DISARM_RANGED",						// 278
	"279",												// 279
	"SPELL_AURA_MOD_IGNOREARMOR_FOR_WEAPON",			// 280
	"SPELL_AURA_MOD_HONOR_GAIN_PCT",					// 281
	"SPELL_AURA_MOD_HEALTHPCT",							// 282
	"283",												// 283
	"284",												// 284
	"SPELL_AURA_MOD_ATTACK_POWER_BY_ARMOR",				// 285
	"286",												// 286
	"SPELL_AURA_MOD_DEFLECT",							// 287
	"288",												// 288
	"289",												// 289
	"SPELL_AURA_MOD_CRIT_CHANCE_ALL",					// 290
	"291",												// 291
	"292",												// 292
	"293",												// 293
	"294",												// 294
	"295",												// 295
	"296",												// 296
	"297",												// 297
	"298",												// 298
	"299",												// 299
	"SPELL_AURA_MOD_MIRROR_DMG",						// 300
	"SPELL_AURA_MOD_HEAL_ABSORB",						// 301
	"302",												// 302
	"303",												// 303
	"304",												// 304
	"305",												// 305
	"306",												// 306
	"307",												// 307
	"SPELL_AURA_MOD_CASTER_CRIT_CHANCE_ON_TARGET_OF_SPELLS", // 308
	"309",												// 309
	"310",												// 310
	"311",												// 311
	"312",												// 312
	"313",												// 313
	"314",												// 314
	"315",												// 315
	"316",												// 316
	"SPELL_AURA_MOD_SPELL_POWER_PCT",					// 317
	"SPELL_AURA_MOD_MASTERY",							// 318
	"SPELL_AURA_MOD_HASTE",								// 319
	"SPELL_AURA_MOD_RANGED_HASTE",						// 320
	"321",												// 321
	"SPELL_AURA_MOD_RANGED_TARGETTING",					// 322	!not sure !
	"323",												// 323
	"324",												// 324
	"325",												// 325
	"326",												// 326
	"327",												// 327
	"328",												// 328
	"SPELL_AURA_MOD_RUNIC_POWER_GAIN_PCT",				// 329
	"SPELL_AURA_MOD_IGNORE_INTERRUPT",					// 330
	"331",												// 331
	"SPELL_AURA_MOD_REPLACE_SPELL",						// 332
	"SPELL_AURA_MOD_ADD_TEMPORARY_SPELL",				// 333
	"334",												// 334
	"335",												// 335
	"336",												// 336
	"337",												// 337
	"338",												// 338
	"339",												// 339
	"340",												// 340
	"341",												// 341
	"SPELL_AURA_MOD_HASTE",								// 342
	"343",												// 343
	"SPELL_AURA_MOD_AUTO_ATTACK_DMG_PCT",				// 344
	"SPELL_AURA_MOD_DMG_PENETRATE",						// 345
	"346",												// 346
	"347",												// 347
	"348",												// 348
	"349",												// 349
	"350",												// 350
	"351",												// 351
	"352",												// 352
	"353",												// 353
	"354",												// 354
	"355",												// 355
	"356",												// 356
	"357",												// 357
	"358",												// 358
	"359",												// 359
	"360",												// 360
	"SPELL_AURA_PROC_TRIGGER_SPELL",					// 361
	"362",												// 362
	"363",												// 363
	"364",												// 364
	"365",												// 365
	"SPELL_AURA_MOD_AP_TO_SP_EXCLUSIVE",				// 366
};


/*
ARCEMU_INLINE void ApplyFloatSM(float ** m,float v,uint32 mask, float def)
{
	if(*m == 0)
	{
		*m = new float[SPELL_GROUPS];

		for(uint32 x=0;x<SPELL_GROUPS;x++)
		{
			(*m)[x] = def;
			if((1<<x) & mask)
				(*m)[x]+=v;
		}
	}
	else
	{
		for(uint32 x=0;x<SPELL_GROUPS;x++)
		{
			if((1<<x) & mask)
				(*m)[x]+=v;
		}
	}
}*/
/*
ARCEMU_INLINE void ApplyFloatPSM(float ** m,int32 v,uint32 mask, float def)
{
	if(*m == 0)
	{
		*m = new float[SPELL_GROUPS];

		for(uint32 x=0;x<SPELL_GROUPS;x++)
		{
			(*m)[x] = def;
			if((1<<x) & mask)
				(*m)[x]+=((float)v)/100.0f;
		}
	}
	else
	{
		for(uint32 x=0;x<SPELL_GROUPS;x++)
		{
			if((1<<x) & mask)
				(*m)[x]+=((float)v)/100.0f;
		}
	}
}*/

Unit* Aura::GetUnitCaster()
{
	//!!!! AMG fix this. We are removed while we are handling aura !!! This is mem corruption
	if( !m_target )
		return NULL;

	if( m_casterGuid == m_target->GetGUID() )
		return m_target;
	if( m_target->GetMapMgr() )
		return m_target->GetMapMgr()->GetUnit( m_casterGuid );
	else
		return NULL;
}

Object* Aura::GetCaster()
{
	if( m_casterGuid == m_target->GetGUID() )
	{ 
		return m_target;
	}
	if( m_target->GetMapMgr() )
	{ 
		return m_target->GetMapMgr()->_GetObject( m_casterGuid );
	}
	else
		return NULL;
}

Aura::Aura()
{
	m_bufferPoolId = OBJECT_WAS_ALLOCATED_STANDARD_WAY;
}

void Aura::Init( SpellEntry* proto, int32 duration, Object* caster, Unit* target, Item* i_caster )
{
	m_spellId = proto->Id;
	m_spellProto = proto;
	m_duration = duration;
	m_positive = 0; //we suppose spell will have positive impact on target
	m_deleted = 0;
	m_casterGuid = caster->GetGUID();
//	m_DamageDoneLastTick = 0;
//	m_CasterDyn = 0;
	m_target = target;

	if( !IsPassive() )
		timeleft = ( uint32 )UNIXTIME;

	m_visualSlot = AURA_INVALID_VISUAL_SLOT;
	pSpellId = 0;
//	periodic_target = 0;
	//sLog.outDetail("Aura::Constructor %u (%s) from %u.", m_spellProto->Id, m_spellProto->Name, m_target->GetLowGUID());
	m_auraSlot = AURA_IN_PROCESS_TO_ADD;
//	m_interrupted = -1;
	m_flags = 0;
	//fixed_amount = 0;//used only for percent values to be able to recover value correctly.No need to init this if we are not using it


//	if( m_target && m_target->IsPlayer() )
//		p_target = SafePlayerCast( m_target );
//	else
//		p_target = NULL;

	if ( i_caster != NULL && i_caster->GetProto() )
		m_castedItemId = i_caster->GetProto()->ItemId;
	else
		m_castedItemId = 0;

	// Modifies current aura duration based on its mechanic type
	// we already applied these when we calculated duration. In case you need this for custom auras then we need to add a new API to get target specific dutation
/*	if( m_target )
	{
		int32 DurationModifier = 0;
		if( proto->MechanicsType < MECHANIC_TOTAL )
			DurationModifier = m_target->MechanicDurationPctMod[ proto->MechanicsType ];
		if( proto->GetDispelType() < DISPEL_COUNT_NON_BENEFICIAL )
			DurationModifier += m_target->DispelDurationPctMod[ proto->GetDispelType() ];
		if(DurationModifier < - 100)
			DurationModifier = -100; // Can't reduce by more than 100%
		SetDuration((GetDuration()*(100+DurationModifier))/100);
	} */

	//SetCasterFaction(caster->_getFaction());

	//m_auraSlot = 0;
	mod = NULL;	//do not keep refenerence to previous usage of this aura. Modlist should be static but it seems something is corrupting it
	m_modcount = 0;
	//we need this to be able to count the number of ticks passed for some mods
	for(uint32 i=0;i<MAX_SPELL_EFFECT_COUNT;i++)
		m_modList[i].fixed_amount[0] = 0;

	if( m_spellProto->c_is_flags & SPELL_FLAG_IS_FORCEDDEBUFF )
		SetNegative( 100 );
	else if( m_spellProto->c_is_flags & SPELL_FLAG_IS_FORCEDBUFF )
		SetPositive( 100 );
	if( caster->IsUnit() )
	{
		if( m_spellProto->BGR_one_buff_from_caster_on_1target > 0 && caster->IsPlayer() )
		{
			( SafePlayerCast(caster) )->RemoveSpellTargets( m_spellProto->BGR_one_buff_from_caster_on_1target, target);
			( SafePlayerCast(caster) )->SetSpellTargetType( m_spellProto->BGR_one_buff_from_caster_on_1target, target);
		}
		if( m_spellProto->BGR_one_buff_from_caster_on_self != 0 )
			SafeUnitCast(caster)->RemoveAllAuraFromSelfType2( m_spellProto->BGR_one_buff_from_caster_on_self, m_spellProto->NameHash );

		if( isAttackable( caster, target ) )
		{
			SetNegative();
			/*if(caster->IsPlayer())
			{
				SafePlayerCast( caster )->CombatModeDelay = COMBAT_DECAY_TIME;
			}
			else if(caster->IsPet())
			{
				Player* p = SafePetCast( caster )->GetPetOwner();
				if(p)
					p->CombatModeDelay = COMBAT_DECAY_TIME;
			}*/
		}
		else
			SetPositive();

		Unit *TopOwnerTarget = target->GetTopOwner();
		if( TopOwnerTarget->IsPlayer() )
		{
			if( SafePlayerCast( TopOwnerTarget )->DuelingWith )
			{
				if( SafePlayerCast( TopOwnerTarget )->DuelingWith == caster )
					SetCastInDuel();
				else if( caster->IsUnit() && SafeUnitCast( caster )->GetTopOwner() == SafePlayerCast( TopOwnerTarget )->DuelingWith )
					SetCastInDuel();
			}
			if( SafePlayerCast( TopOwnerTarget )->m_bg != NULL )
				SetWasCastInBG();
		}
	}
}

void Aura::Virtual_Constructor()
{
	EventableObject::Virtual_Constructor();
	sEventMgr.RemoveEvents( this ); //this will do nothing
	m_holder = NULL;	//clear out event holder. There is a bug that if player logoud out in an instance and item was attached to it then item get invalid pointer to a holder
}

Aura::~Aura()
{
	Virtual_Destructor();
	if( m_bufferPoolId != OBJECT_WAS_ALLOCATED_STANDARD_WAY )
		ASSERT( false ); //we are deleting a pooled object. This leads to mem corruptions
	m_bufferPoolId = OBJECT_WAS_DEALLOCATED;
}

void Aura::Virtual_Destructor()
{
	ExtensionSet::iterator itr;
	for( itr=m_extensions.begin(); itr!=m_extensions.end(); itr++)
		if( itr->second )
			delete itr->second;
	m_extensions.clear();
	//forget about context and events to avoid memory leaks
	EventableObject::Virtual_Destructor();
	//this should do nothing now
	sEventMgr.RemoveEvents( this );
}

void Aura::RemoveDelayed()
{
	sEventMgr.AddEvent(this, &Aura::Remove, EVENT_AURA_PERIODIC_UPDATE, 1, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
}

void Aura::Remove()
{
	//required to remove events always since there is a chance that mod 1 removes aura while mod 2 applies a periodic spell on the deleted aura
	sEventMgr.RemoveEvents(  this );

	//just lol, this should be properly solved ? 
	//Zack : changing from bool to numeric to see if there is a thread concurency issue here
	{
		if( m_deleted != 0 )
		{ 
			return;
		}
		m_deleted++;
	}

	// no idea how this happens, if for some reason the aura gets deleted while it is still pending to get added to the unit, then don't add it to the unit
	// so far found aura : 13218
	CommitPointerListNode<Aura> *itr;
	m_target->QueueAuraAdd.BeginLoop();
	for( itr = m_target->QueueAuraAdd.begin(); itr != m_target->QueueAuraAdd.end(); itr = itr->Next() )
		if( itr->data == this )
			m_target->QueueAuraAdd.SafeRemove( itr, 0 );
	m_target->QueueAuraAdd.EndLoopAndCommit();

	//in case i forget to remove this then it is only for temp debug :P
//	CStackWalker sw; 
//	if( GetSpellProto()->Id == 49005 )
//		sw.ShowCallstack(); 

	//we already deleted all events so this should do nothing in theory
	sEventMgr.RemoveEvents( this );

	//first thing to do is to remove ourself from unit check list
	// this increases the chance that on removing aura we do not trigger event to double delete us
	//just a wtf check : object does not have the aura but aura thinks he still belongs to the object. This should never happen
	if( m_auraSlot != AURA_IN_PROCESS_TO_ADD && m_target->m_auras[ m_auraSlot ] == this )
		m_target->m_auras[ m_auraSlot ] = NULL;
	else
	{
		bool found_on_target = false;
		uint32 i;
		for(i=FIRST_AURA_SLOT;i<MAX_AURAS + MAX_PASSIVE_AURAS;i++)
			if( m_target->m_auras[ i ] == this )
			{
				m_target->m_auras[ i ] = NULL;
				found_on_target = true;
			}
		if( found_on_target == false )
		{
//				ASSERT( false );
			if( m_auraSlot != AURA_IN_PROCESS_TO_ADD ) //when we try to add the aura but fail for some reason. In this case the effects were already applied and should be removed
			{
				printf("Removing aura from target but target does not have the aura ! %d - %s - %d - mods applied %d-%d\n",m_spellProto->Id,m_spellProto->Name,m_auraSlot,(m_flags&MODS_ARE_APPLIED)!=0,(m_flags&MODS_APPLY_WAS_CALLED)!=0);
				return;
			}
		}
		else
		{
			printf("Removing aura from target from incorrect slot ! %d - %s - %d - %d\n",m_spellProto->Id,m_spellProto->Name,m_auraSlot,i);
		}
	}
	m_auraSlot = AURA_IN_PROCESS_TO_ADD;

	if( !IsPassive() || ( IsPassive() && m_spellProto->AttributesEx & 1024 ) )
//		RemoveAuraVisual();
		m_target->ModVisualAuraStackCount( this, -1 );

	ApplyModifiers( false );

	for( uint32 x = 0; x < MAX_SPELL_EFFECT_COUNT; x++ )
	{
		if( GetSpellProto()->eff[x].EffectTriggerSpell 
			&& ( m_spellProto->eff[x].Effect == SPELL_EFFECT_TRIGGER_SPELL || m_spellProto->eff[x].Effect == SPELL_EFFECT_TRIGGER_SPELL_WITH_VALUE )	//this is required to avoid removing auras that get applied after N seconds of having another buff : ex : well fed 
			&& (GetSpellProto()->c_is_flags2 & SPELL_FLAG2_IS_ALLOWED_TO_LEAVE_CHAINED_AURAS) == 0 
			)
		{
			m_target->RemoveAura( GetSpellProto()->eff[x].EffectTriggerSpell, m_casterGuid, AURA_SEARCH_BENEFIC );
		}
		else if( (m_spellProto->eff[x].Effect == SPELL_EFFECT_APPLY_AREA_AURA || m_spellProto->eff[x].Effect == SPELL_EFFECT_APPLY_AREA_AURA2) && m_casterGuid == m_target->GetGUID())
		{
			RemoveAA();	//need to call this after marking this aura in process of being removed to avoid loop calls !
		}
	}

	if( m_spellProto->procCharges > 0 
//		&& m_spellProto->proc_interval == 0 
		)
	{
		std::map< uint32, struct SpellCharge >::iterator iter;
		iter = m_target->m_chargeSpells.find( GetSpellId() );
		if( iter != m_target->m_chargeSpells.end() )
			if( iter->second.count >= 1 )
				--iter->second.count;	//deletion will be made only in handleproc function
	}

	// reset diminishing return timer if needed
	//::UnapplyDiminishingReturnTimer( m_target, m_spellProto );

	// remove attacker
	Unit* caster = GetUnitCaster();
	if( caster != NULL)
	{
/*		if( caster != m_target )
		{
			caster->CombatStatus.RemoveAttackTarget( m_target );
			m_target->CombatStatus.RemoveAttacker( caster, caster->GetGUID() );
		}*/

		//only works for auras with 1 stack !
		if( m_spellProto->BGR_one_buff_from_caster_on_1target != 0 && m_target->IsPlayer() && m_spellProto->maxstack <= 1 )
			SafePlayerCast( m_target )->RemoveSpellIndexReferences( m_spellProto->BGR_one_buff_from_caster_on_1target );

		/**********************Cooldown**************************
		* this is only needed for some spells
		* for now only spells that have:
		* (m_spellInfo->Attributes == 0x2050000) && !(m_spellInfo->AttributesEx) ||
		* m_spellProto->Attributes == 0x2040100
		* are handled. Its possible there are more spells like this
		*************************************************************/

		if( caster->IsPlayer() && caster->IsInWorld() )
		{
			//shadow form
			if( m_spellProto->c_is_flags & SPELL_FLAG_IS_REQUIRECOOLDOWNUPDATE )
			{
				if( caster->IsPlayer() && SafePlayerCast( caster )->GetSession() )
					SafePlayerCast( caster )->StartClientCooldown( m_spellProto->Id );
			}

			//check for farsight removal
			if( HasEffectType( m_spellProto, SPELL_EFFECT_ADD_FARSIGHT ) )
				caster->SetUInt64Value(PLAYER_FARSIGHT, 0);
		}

		//warning !!! this might create a lag or loop. Casting 60 auras a second by player is very much possible and it might lag
		//only proc after we removed all auras of this spell
		if( m_modcount
//			&& mod->i == mod->total_mods 
			)
		{
			if( caster != m_target )
			{
				m_target->HandleProc( PROC_ON_AURA_REMOVE_EVENT | PROC_ON_VICTIM, caster, m_spellProto, &m_modList[0].m_amount );
				caster->HandleProc( PROC_ON_AURA_REMOVE_EVENT | PROC_ON_NOT_VICTIM, m_target, m_spellProto, &m_modList[0].m_amount );
			}
			else
				caster->HandleProc( PROC_ON_AURA_REMOVE_EVENT | PROC_ON_NOT_VICTIM | PROC_ON_VICTIM, caster, m_spellProto, &m_modList[0].m_amount );
		}
		//only used by wyvern sting ?
		if( m_spellProto->aura_remove_cast )
			caster->CastSpell( m_target, m_spellProto->aura_remove_cast, true );
	}
//	else
//		m_target->CombatStatus.RemoveAttacker( NULL, m_casterGuid );

	//if this spell had a linked cast then also remove that one :P
	if( GetSpellProto()->chained_cast )
		m_target->RemoveAura( GetSpellProto()->chained_cast->Id, m_casterGuid, AURA_SEARCH_BENEFIC, GetSpellProto()->chained_cast->maxstack + 1 );
//		m_target->RemoveAura( GetSpellProto()->chained_cast->Id, m_casterGuid );	//there are some buffs that leave a negative buff on us. Don't remove that one

	mod = NULL;	//this might cause crashes. X spell has aura that casts spell Y that removes X. When X returns control he has invalid pointers
	m_target = NULL;
//	p_target = NULL;
	m_spellProto = NULL;
//	m_casterGuid = 0; //i will keep this to track hackers who manage to bug out auras

	//maybe some idiot coder added new events in the process of remove ? Purge the evil !
	sEventMgr.RemoveEvents( this );

	AuraPool.PooledDelete( this, __FILE__, __LINE__ ); // suicide xD	leaking this shit out
}

void Aura::AddMod( uint32 AuraName, int32 a, uint32 miscValue, uint32 i, int32 pct_mod )
{
	//this is fix, when u have the same unit in target list several times
	//for(uint32 x=0;x<m_modcount;x++)
	//	if(m_modList[x].i==i)return;

	/*if(m_modList[0].m_type == t ||
		m_modList[1].m_type == t ||
		m_modList[2].m_type == t)
		return; // dont duplicate mods // some spells apply duplicate mods, like some seals*/
	if( m_flags & MODS_APPLY_WAS_CALLED )
	{
		printf("Aura is receiving a new mod but aura is already applied on target. Strange\n");
	}

	if( m_modcount >= 3 )
	{
		//this might happen when mob is dead and we keep adding auras to temp list
		sLog.outString("Tried to add >3 (%u) mods to spellid %u [%u:%u, %u:%u, %u:%u]", m_modcount+1, this->m_spellProto->Id, m_modList[0].m_AuraName, m_modList[0].m_amount, m_modList[1].m_AuraName, m_modList[1].m_amount, m_modList[2].m_AuraName, m_modList[2].m_amount);
		return;
	}
	m_modList[m_modcount].m_AuraName = AuraName;
	m_modList[m_modcount].m_amount = a;
	m_modList[m_modcount].m_pct_mod = pct_mod;
	m_modList[m_modcount].m_miscValue = miscValue;
	m_modList[m_modcount].i = i;
	m_modList[m_modcount].list_ind = m_modcount;
	m_modcount++;
	//ASSERT(m_modcount<=3);
}

void Aura::ApplyModifiers( bool apply )
{
//	if( apply == true && m_target )
//	{
//	    if( m_spellProto->BGR_one_buff_on_target & SPELL_TYPE_SEAL )
//			m_target->SetAuraStateFlag( AURASTATE_JUDGEMENT );
//	}
	if( apply == false && ( m_flags & MODS_ARE_APPLIED ) == 0 )
	{
//		printf("Trying to unapply aura mods, but the mods were never applied\n"); //this will happen a lot for druids who shapeshift
		return;
	}
	if( apply == true && ( m_flags & MODS_ARE_APPLIED ) != 0 )
	{
//		printf("Trying to apply aura mods, but the mods were already applied\n"); 
		return;
	}

	m_flags |= MODS_APPLY_WAS_CALLED;

	if( apply )
	{
		m_flags |= MODS_ARE_APPLIED;
		for( uint32 i=0;i<GetSpellProto()->SetAuraStatesCount;i++)
			m_target->SetAuraStateFlag( GetSpellProto()->SetAuraStates[ i ] );
	}
	else
	{
		m_flags &= ~MODS_ARE_APPLIED;
		for( uint32 i=0;i<GetSpellProto()->SetAuraStatesCount;i++)
			m_target->RemoveAuraStateFlag( GetSpellProto()->RemAuraStates[ i ] );

	}
	for( uint32 x = 0; x < m_modcount; x++ )
	{
		mod = &m_modList[x];
//		mod->total_mods = m_modcount-1;
		if(mod->m_AuraName<TOTAL_SPELL_AURAS)
		{
			sLog.outDebug( "WORLD: target = %u , Spell Aura id = %u (%s), SpellId  = %u, i = %u, apply = %s, duration = %u, damage = %d, period = %u",m_target->GetLowGUID(),mod->m_AuraName, SpellAuraNames[mod->m_AuraName], m_spellProto->Id, mod->i, apply ? "true" : "false",GetDuration(),mod->m_amount,m_spellProto->eff[ mod->i ].EffectAmplitude);
			if( GetSpellProto()->AuraHandleHook )
			{
				uint32 ret = GetSpellProto()->AuraHandleHook( this, apply, mod->i );
				//this handler fully overwritten the old one ?
				if( ret == SPELL_EFFECT_OVERRIDE_BREAK_EXECUTION )
					return;
			}
			(*this.*SpellAuraHandler[mod->m_AuraName])(apply);
		}
		else
			sLog.outError("Unknown Aura id %d", (uint32)mod->m_AuraName);
	}
}

void Aura::UpdateModifiers( )
{
	for( uint32 x = 0; x < m_modcount; x++ )
	{
		mod = &m_modList[x];

		if(mod->m_AuraName<TOTAL_SPELL_AURAS)
		{
			sLog.outDebug( "WORLD: target = %u , Spell Aura id = %u (%s), SpellId  = %u, i = %u, duration = %u, damage = %d",m_target->GetLowGUID(),mod->m_AuraName, SpellAuraNames[mod->m_AuraName], m_spellProto->Id, mod->i, GetDuration(),mod->m_amount);
			switch (mod->m_AuraName)
			{
				case SPELL_AURA_MOD_DECREASE_SPEED: UpdateAuraModDecreaseSpeed(); break;

			}
		}
		else
			sLog.outError("Unknown Aura id %d", (uint32)mod->m_AuraName);
	}
}

//these might bug out if original stack gets removed then rest will have no effect at all
bool TryToStackAura( Unit *m_target, Aura *NewA, Modifier *mod )
{
	if( NewA->IsPassive() )
		return false;
	//find the other stack and mod the value of that to not spam combat log
	//this has a chance to bug out if we remove original stack and rest will do nothing since they have no event registered
	SpellEntry *sp = NewA->GetSpellProto();
	if( sp->maxstack > 1 && sp->procCharges == 0 )
	{
		uint32 start,end;
		if( NewA->IsPositive() )
		{
			start = POS_AURAS_START;
			end = MAX_POSITIVE_AURAS1( m_target );
		}
		else
		{
			start = NEG_AURAS_START;
			end = MAX_NEGATIVE_AURAS1( m_target );
		}
		//let search debuffs to see if we can find out match
		for( uint32 i=start;i<end; i++ )
		{
			Aura *a = m_target->m_auras[i];
			if( a && a->GetSpellId() == sp->Id && a != NewA && (a->m_flags & WAS_MERGED_INTO_OTHER_ON_APPLY) == 0 )
			{
				for( uint32 j=0;j<a->m_modcount;j++)
					if( a->m_modList[j].list_ind == mod->list_ind )
					{
						{
//							mod->m_amount = mod->m_amount * mod->m_pct_mod / 100;
//							a->m_modList[j].m_amount += mod->m_amount;
						}
						{
							a->m_modList[j].m_amount = MAX( a->m_modList[j].m_amount, mod->m_amount );	//this is required or it might get replaced by the next aura refresh and the PCT is lost
							a->m_modList[j].m_pct_mod += mod->m_pct_mod;
						}
						{
//							a->m_modList[j].m_amount = a->m_modList[j].m_amount * a->m_modList[j].m_pct_mod / 100;
//							a->m_modList[j].m_pct_mod = 100;
//							mod->m_amount = mod->m_amount * mod->m_pct_mod / 100;
//							a->m_modList[j].m_amount += mod->m_amount;
						}
						mod->fixed_amount[2] = (int32)(a);
						NewA->m_flags |= WAS_MERGED_INTO_OTHER_ON_APPLY;
						a->m_flags |= WAS_MERGED_INTO_THIS_ON_APPLY;
						return true;
					}
			}
		}
	}
	return false;
}

bool TryToUnStackAura( Unit *m_target, Aura *NewA, Modifier *mod )
{
	//we should move all stacks of this aura from target or else he will become bugged
	if( ( NewA->m_flags & WAS_MERGED_INTO_OTHER_ON_APPLY ) != 0 )
	{
		uint32 start,end;
		if( NewA->IsPositive() )
		{
			start = POS_AURAS_START;
			end = MAX_POSITIVE_AURAS1( m_target );
		}
		else
		{
			start = NEG_AURAS_START;
			end = MAX_NEGATIVE_AURAS1( m_target );
		}
		for( uint32 i=start;i<end; i++ )
		{
			Aura *a = m_target->m_auras[i];
			if( a && a->GetSpellId() == NewA->GetSpellId() && a != NewA && (a->m_flags & WAS_MERGED_INTO_OTHER_ON_APPLY) != 0 )
				a->RemoveDelayed();
		}
	}
	//if we get replaced by a stronger version
	if( ( NewA->m_flags & WAS_MERGED_INTO_OTHER_ON_APPLY ) != 0 )
	{
		uint32 start,end;
		if( NewA->IsPositive() )
		{
			start = POS_AURAS_START;
			end = MAX_POSITIVE_AURAS1( m_target );
		}
		else
		{
			start = NEG_AURAS_START;
			end = MAX_NEGATIVE_AURAS1( m_target );
		}
		for( uint32 i=start;i<end; i++ )
		{
			Aura *a = m_target->m_auras[i];
			if( (int32)a == mod->fixed_amount[2] )
			{
				for( uint32 j=0;j<a->m_modcount;j++)
					if( a->m_modList[j].list_ind == mod->list_ind )
					{
//						a->m_modList[j].m_amount -= mod->m_amount;
						a->m_modList[j].m_pct_mod -= mod->m_pct_mod;
						return true;
					}
			}
		}
	}
	return false;
}

void Aura::EventUpdateAA(float r)
{
	uint32 i;
	vector<uint64> NewTargets;
	vector<uint64>::iterator NewTargetsitr;
	Group * group = NULL;

	// if the caster is no longer valid->remove the aura
	if( m_target == NULL || m_target->deleted != OBJ_AVAILABLE )
	{
		Remove();
		//since we lost the caster we cannot do anything more
		return;
	}

	bool skip_group_check = true;

	//if we are not added to a map then there is no point adding auras to anyone atm, just remove from old targets
	if( m_target->GetMapMgr() )
	{
		group = m_target->GetGroup();
		if( group )
		{
			skip_group_check = false;
			group->Lock();
			uint32 count = group->GetSubGroupCount();
			// Loop through each raid group.
			for( uint8 k = 0; k < count; k++ )
			{
				SubGroup * sgroup = group->GetSubGroup( k );
				if(sgroup && sgroup->GetMemberCount() > 0 )
				{
					GroupMembersSet::iterator itr;
					for(itr = sgroup->GetGroupMembersBegin(); itr != sgroup->GetGroupMembersEnd(); ++itr)
						if( (*itr) 
							&& (*itr)->m_loggedInPlayer 
							&& (*itr)->m_loggedInPlayer->GetInstanceID() == m_target->GetInstanceID()
							&& (*itr)->m_loggedInPlayer->GetDistanceSq(m_target) <= r 
							&& (*itr)->m_loggedInPlayer->isAlive()
							)
						{
							if( (*itr)->m_loggedInPlayer->HasAuraWithNameHash( m_spellProto->NameHash ) == false )
								NewTargets.push_back( (*itr)->m_loggedInPlayer->GetGUID() );
							//summons are also inside the group ?
							if( (*itr)->m_loggedInPlayer->GetSummon() && (*itr)->m_loggedInPlayer->GetSummon()->HasAuraWithNameHash( m_spellProto->NameHash ) == false )
								NewTargets.push_back( (*itr)->m_loggedInPlayer->GetSummon()->GetGUID() );
						}
				}
			}
			group->Unlock();
		}
		else
		{
			if( m_target->isAlive() && m_target->HasAuraWithNameHash(m_spellProto->NameHash) == false )
				NewTargets.push_back( m_target->GetLowGUID() );	//no frikkin way
			else if( m_target->IsPlayer() && SafePlayerCast( m_target )->GetSummon() && SafePlayerCast( m_target )->GetSummon()->HasAuraWithNameHash( m_spellProto->NameHash ) == false  )
				NewTargets.push_back( SafePlayerCast( m_target )->GetSummon()->GetGUID() );
			else if( m_target->IsPet() && SafePetCast( m_target )->GetPetOwner() && SafePetCast( m_target )->GetPetOwner()->HasAuraWithNameHash( m_spellProto->NameHash ) == false  )
				NewTargets.push_back( SafePetCast( m_target )->GetPetOwner()->GetGUID() );
		}
		//stupid talents that have same name as the area aura. Ex : Totemic Wrath is a proc that will have the same name as the talent
		{
			Unit *topA = m_target->GetTopOwner();	//totem owner ;)
			if( topA != m_target && topA->HasAura( m_spellProto->Id ) == false )
				NewTargets.push_back( topA->GetLowGUID() );
		}

		//add aura to the new targets
		for( NewTargetsitr=NewTargets.begin();NewTargetsitr!=NewTargets.end();NewTargetsitr++)
		{
			Unit *target = m_target->GetMapMgr()->GetUnit( *NewTargetsitr );	//maybe one cast removes spawns ?
			if( target 
				&& target->GetDistanceSq( m_target ) <= r )	//do not spam the addaura packet just to remoe it a few lines below
			{
				Aura * aura = NULL;
				aura = AuraPool.PooledNew( __FILE__, __LINE__ );
				aura->Init(m_spellProto, -1, m_target, target);
				for(i = 0; i < m_modcount; ++i)
					if( m_spellProto->eff[m_modList[i].i].Effect == SPELL_EFFECT_APPLY_AREA_AURA
						|| m_spellProto->eff[m_modList[i].i].Effect == SPELL_EFFECT_APPLY_AREA_AURA2
						)
						aura->AddMod(m_modList[i].m_AuraName, m_modList[i].m_amount,m_modList[i].m_miscValue, m_modList[i].i, m_modList[i].m_pct_mod );
				aura->SetIsAreaAura();	//! so other players may remove aura if clicked on it
				target->AddAura(aura);
			}
		}
	}

	// Update the existing players in the target set.
	AreaAuraList::iterator itr, it2;
	for(itr = targets.begin(); itr != targets.end(); )
	{
		it2 = itr;
		++itr;

		if( m_target == NULL )
			return;

		// Check if the target is 'valid'.
		Unit *target;
		if(m_target->IsInWorld())
			target = m_target->GetMapMgr()->GetUnit(*it2);
		else
			target = NULL;

		if( target == NULL )
			target = objmgr.GetPlayer(*it2);

		if( target == NULL )
		{
			targets.erase(it2);
			continue;
		}

		//caster does not loose aura, ever
		if( m_target == target ) 
			continue;

		if( target->GetDistanceSq( m_target ) > r || target->GetInstanceID() != m_target->GetInstanceID() || target->deleted != OBJ_AVAILABLE )
		{
			targets.erase(it2);
			target->RemoveAura(m_spellProto->Id);
			continue;
		}

		if(skip_group_check == false && ( !group || target->GetGroup() != group ) )
		{
			// execute in the correct context
			target->RemoveAura( m_spellProto->Id );
			targets.erase(it2);
		}
	}

	// Push new targets into the set.
	for(NewTargetsitr = NewTargets.begin(); NewTargetsitr != NewTargets.end(); ++NewTargetsitr)
		targets.insert( (*NewTargetsitr) );

	NewTargets.clear();
}

//!! this removes auras only from targets. We are not included here ! We are not supposed to be included !
void Aura::RemoveAA()
{
	AreaAuraList::iterator itr;

	for(itr = targets.begin(); itr != targets.end(); ++itr)
	{
		if( m_target == NULL )
			break;

		// Check if the target is 'valid'.
		Unit *aatarget;
		if( m_target->IsInWorld() )
			aatarget = m_target->GetMapMgr()->GetUnit( *itr );
		else
			aatarget = objmgr.GetPlayer(*itr);

		if( !aatarget )
			continue;

		aatarget->RemoveAura( m_spellProto->Id );
	}
	targets.clear();
}

void Aura::ResetDuration(uint32 InheritedDuration)
{
	//when a short duration aura refreshes a long duration aura, make sure to not extend the duration too much
	//this happens with auras that have dynamic duration based on combopoints
	uint32 tl = GetTimeLeft();
	uint32 max_avail_dur = tl + InheritedDuration;
	uint32 max_dur = MAX( (uint32)GetDuration(), InheritedDuration );
	SetDuration( MIN( max_avail_dur, max_dur )  );
	ResetDuration();
}

void Aura::ResetDuration()
{
	//update duration,the same aura (update the whole stack whenever we cast a new one)
	ResetTimeLeft();
	//important to use new duration because due to diminishing returns refreshmight get smaller!
	if( sEventMgr.HasEvent( this, EVENT_AURA_REMOVE ) )
	{
		//mod event time will not work !!!!
//		sEventMgr.ModifyEventTimeLeft( this, EVENT_AURA_REMOVE, GetDuration());
		sEventMgr.RemoveEvents( this, EVENT_AURA_REMOVE);
		sEventMgr.AddEvent( this, &Aura::Remove, EVENT_AURA_REMOVE, GetDuration(), 1, (EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT | EVENT_FLAG_DELETES_OBJECT));
	}
//	else if( GetDuration() > 0 )
//		sEventMgr.AddEvent(this, &Aura::Remove, EVENT_AURA_REMOVE, GetDuration() + 500, 1,	EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT | EVENT_FLAG_DELETES_OBJECT);

	if( m_modcount > 0 ) //chage spells will have 0 mods. Refresh only the placeholder one that has values
		m_target->ModVisualAuraStackCount( this, 0);
}

//------------------------- Aura Effects -----------------------------

void Aura::SpellAuraModBaseResistance(bool apply)
{
	SpellAuraModResistance(apply);
	//both add/decrease some resistance difference is unknown
}

void Aura::SpellAuraModBaseResistancePerc(bool apply)
{
	uint32 Flag = mod->m_miscValue;
	int32 amt;
	if(apply)
	{
		amt = mod->m_amount;
		if(amt > 0)
			SetPositive();
		else
			SetNegative();
	}
	else
		amt =- mod->m_amount;

	for(uint32 x=0;x<SCHOOL_COUNT;x++)
	{
		if(Flag & (((uint32)1)<< x))
		{
			if( m_target->IsPlayer() )
			{
				if(mod->m_amount>0)
					SafePlayerCast( m_target )->BaseResistanceModPctPos[x]+=amt;
				else
					SafePlayerCast( m_target )->BaseResistanceModPctNeg[x]-=amt;
				SafePlayerCast( m_target )->CalcResistance(x);

			}
			else if( m_target->IsCreature() )
			{
				SafeCreatureCast(m_target)->BaseResistanceModPct[x]+=amt;
				SafeCreatureCast(m_target)->CalcResistance(x);
			}
		}
	}
}

void Aura::SpellAuraNULL(bool apply)
{
	 sLog.outDebug("Unknown Aura id %d", (uint32)mod->m_AuraName);
}

void Aura::SpellAuraBindSight(bool apply)
{
	SetPositive();
	// MindVision
	if(apply)
	{
		Unit *caster = GetUnitCaster();
		if(!caster || !caster->IsPlayer())
		{ 
			return;
		}
		caster->SetUInt64Value(PLAYER_FARSIGHT, m_target->GetGUID());
	}
	else
	{
		Unit *caster = GetUnitCaster();
		if(!caster || !caster->IsPlayer())
		{ 
			return;
		}
		caster->SetUInt64Value(PLAYER_FARSIGHT, 0 );
	}
}

void Aura::SpellAuraModPossess(bool apply)
{
	Unit *caster = GetUnitCaster();

	if(apply)
	{
		if( caster != NULL && caster->IsInWorld() && caster->IsPlayer() )
		{
			SafePlayerCast(caster)->Possess( m_target );
			SafePlayerCast(caster)->SpeedCheatDelay( GetDuration() );
		}
	}
	else
	{
		if( caster != NULL && caster->IsInWorld() && caster->IsPlayer() )
		{
			SafePlayerCast(caster)->UnPossess();
//			SafePlayerCast(caster)->SpeedCheatReset();
		}

		// make sure Player::UnPossess() didn't fail, if it did we will just free the target here
		if( m_target->GetUInt64Value( UNIT_FIELD_CHARMEDBY ) != 0 )
		{
			if( m_target->IsCreature() )
			{
				m_target->setAItoUse( true );
				m_target->m_redirectSpellPackets = 0;
			}

			m_target->SetUInt64Value( UNIT_FIELD_CHARMEDBY, 0 );
			m_target->RemoveFlag( UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED_CREATURE );
			m_target->SetUInt32Value( UNIT_FIELD_FACTIONTEMPLATE, m_target->GetCharmTempVal() );
			m_target->_setFaction();
//			m_target->UpdateOppFactionSet();
		}
		else
		{
			//mob woke up and realized he was controlled. He will turn to controller and also notify the other mobs he is fighting that they should attack the caster
			//sadly i got only 3 test cases about this so i might be wrong :(
			//zack : disabled until tested
			m_target->GetAIInterface()->EventChangeFaction( caster );
		}
	}
}

void Aura::SpellAuraPeriodicDamage(bool apply)
{
	if(apply)
	{
		if( m_target )
		{
			if( m_spellProto->MechanicsType == MECHANIC_BLEEDING && m_target->MechanicsDispels[MECHANIC_BLEEDING] )
			{
				m_flags |= 1 << mod->i;
				return;
			}
		}
		int32 dmg = mod->m_amount;
		Unit *c = GetUnitCaster();

		if(dmg<=0)
			return; //who would want a neagtive dmg here ?

		//hasting affects ticks too
		Unit *Unitc = GetUnitCaster();
		int32 period = GetSpellAmplitude( GetSpellProto(), Unitc, mod->i, m_castedItemId );

		int32 ticks = GetDuration() / period;
		if( ticks < 1 )
			ticks = 1;
//		sLog.outDebug("Registering periodic ticks. Total dmg %u, period %u \n",dmg,period);
//		mod->m_amount = dmg * mod->m_pct_mod / 100;	//used by improved serpent sting to get the dot value
		mod->m_amount = dmg;

		/*SafePlayerCast( c )->GetSession()->SystemMessage("dot will do %u damage every %u seconds (total of %u)", dmg,m_spellProto->eff[mod->i].EffectAmplitude,(GetDuration()/m_spellProto->eff[mod->i].EffectAmplitude)*dmg);
		printf("dot will do %u damage every %u seconds (total of %u)\n", dmg,m_spellProto->eff[mod->i].EffectAmplitude,(GetDuration()/m_spellProto->eff[mod->i].EffectAmplitude)*dmg);*/
		SetNegative();
		uint32 Flags = EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT;
		if( GetSpellProto()->c_is_flags2 & SPELL_FLAG2_IS_TICKING_IMEDIATLY )
			Flags |= EVENT_FLAG_TICK_ON_NEXT_UPDATE;

		//find the other stack and mod the value of that to not spam combat log
		//this has a chance to bug out if we remove original stack and rest will do nothing since they have no event registered
		if( TryToStackAura( m_target, this, mod ) == false )
			sEventMgr.AddEvent(this, &Aura::EventPeriodicDamage, (uint32)ticks, (uint32)mod->list_ind, EVENT_AURA_PERIODIC_DAMAGE, period , 0, Flags );
	}
	else if( (m_flags & (1 << mod->i)) == 0 ) //add these checks to mods where imunity can cancel only 1 mod and not whole spell
	{
	}
	//if we get replaced by a stronger version
	if( apply == false )
		TryToUnStackAura( m_target, this, mod );
}

//this is used if we want to dynamically change the tick amount while the spell is ongoing. We can change the aura modlist variables
void Aura::EventPeriodicDamage( uint32 ticks, uint32 mod_index )
{
	EventPeriodicDamage( m_modList[ mod_index ].m_amount, m_modList[ mod_index ].m_pct_mod, ticks, m_modList[ mod_index ].i );
	m_flags |= FIRST_TICK_ENDED;
}

void Aura::EventPeriodicDamage(uint32 amount, int32 pct_mod, uint32 ticks, uint32 eff_index)
{
	//DOT
	if(!m_target || !m_target->isAlive())
	{ 
		return;
	}
	if( m_target->SchoolImmunityAntiEnemy[GetSpellProto()->School] || m_target->SchoolImmunityAntiFriend[GetSpellProto()->School] )
	{ 
		return;
	}
	Unit * c = GetUnitCaster();
	if( m_target->GetGUID() != m_casterGuid )//don't use resist when cast on self-- this is some internal stuff
	{
		if(c)
		{
			dealdamage dmg;
			dmg.base_dmg = amount;
			dmg.pct_mod_final_dmg = pct_mod;
			dmg.FirstTick = IsFirstTick();
			c->SpellNonMeleeDamageLog( m_target, GetSpellProto(), &dmg, ticks, eff_index);
//			m_DamageDoneLastTick = dmg.full_damage * 100 / ( dmg.pct_mod_crit + 100 );
			return; //!! aura can get deleted while doing dmg !!
		}
	}
}

void Aura::SpellAuraDummy(bool apply)
{
	// Try a dummy SpellHandler
	if(sScriptMgr.CallScriptedDummyAura(GetSpellProto(), mod->i, this, apply))
	{ 
		return;
	}

	uint32 TamingSpellid = 0;

	Player * _ptarget;

	if( m_target->IsPlayer() )
		_ptarget = SafePlayerCast( m_target );
	else 
		_ptarget = NULL;

	switch(GetSpellId())
	{
#include "../arcemu-customs/custom_spell_auras.cpp"
		case 56817: //rune strike enabler. Yes, this does not make any sense. Why enable aurastate on caster instead target. Stupid asian developers
			{
				if( apply )
					m_target->SetAuraStateFlag( AURASTATE_DODGE_BLOCK );
				else
					m_target->RemoveAuraStateFlag( AURASTATE_DODGE_BLOCK );
			}break;
		case 80265: // Potion of Illusion
		case 80396:
		{
			if( _ptarget == NULL )
				return;
			if( apply == true )
			{
				//backup looks
				_ptarget->BackupRestoreOnceRaceGenderStyle();
				for( uint32 i=PLAYER_VISIBLE_ITEM_1_ENTRYID;i<PLAYER_VISIBLE_ITEM_19_ENTRYID;i+=2 )
				{
					int64 *b = _ptarget->GetCreateIn64Extension( EXTENSION_ID_POTION_OF_ILLUSION_ITEM_BACKUP_START + i );
					*b = _ptarget->GetUInt32Value( i );
				}
				mod->fixed_amount[1] = _ptarget->GetUInt32Value( UNIT_FIELD_NATIVEDISPLAYID );

				//get a random char we will copy
				uint32 ValidCharCount = 0;
				InrangeLoopExitAutoCallback AutoLock;
				for(InRangePlayerSetRecProt::iterator it2 = _ptarget->GetInRangePlayerSetBegin( AutoLock ); it2 != _ptarget->GetInRangePlayerSetEnd(); ++it2)
				{
					if( (*it2)->isAlive() == false )
						continue;
					ValidCharCount++;
				}
				if( ValidCharCount == 0 )
					return;
				uint32 SelectedChar = RandomUInt() % ValidCharCount;
				ValidCharCount = 0;
				Player *pSelectedChar = NULL;
				for(InRangePlayerSetRecProt::iterator it2 = _ptarget->GetInRangePlayerSetBegin( AutoLock ); it2 != _ptarget->GetInRangePlayerSetEnd(); ++it2)
				{
					if( (*it2)->isAlive() == false )
						continue;
					if( ValidCharCount == SelectedChar )
						pSelectedChar = (*it2);
					ValidCharCount++;
				}
				//now copy looks and items
				_ptarget->SetUInt32Value( PLAYER_BYTES, pSelectedChar->GetUInt32Value( PLAYER_BYTES ) );
				_ptarget->SetByte( PLAYER_BYTES_2, 0, pSelectedChar->GetByte( PLAYER_BYTES_2, 0 ) );
				_ptarget->setGender( pSelectedChar->getGender() );
				_ptarget->setRace( pSelectedChar->getRace() );
				_ptarget->SetUInt32Value(UNIT_FIELD_DISPLAYID, pSelectedChar->GetUInt32Value(UNIT_FIELD_DISPLAYID ) );
				_ptarget->SetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID, _ptarget->GetUInt32Value( UNIT_FIELD_DISPLAYID ) ); //or else items will not display
				for( uint32 i=PLAYER_VISIBLE_ITEM_1_ENTRYID;i<PLAYER_VISIBLE_ITEM_19_ENTRYID;i+=2 )
					_ptarget->SetUInt32Value( i, pSelectedChar->GetUInt32Value( i ) );
				//mark that we made changes
				mod->fixed_amount[0] = 1;
			}
			else if( mod->fixed_amount[0] == 1 )
			{
				_ptarget->BackupRestoreOnceRaceGenderStyle( true );
				for( uint32 i=PLAYER_VISIBLE_ITEM_1_ENTRYID;i<PLAYER_VISIBLE_ITEM_19_ENTRYID;i+=2 )
				{
					int64 *b = _ptarget->GetCreateIn64Extension( EXTENSION_ID_POTION_OF_ILLUSION_ITEM_BACKUP_START + i );
					_ptarget->SetUInt32Value( i, *b );
				}
				_ptarget->SetUInt32Value(UNIT_FIELD_DISPLAYID, mod->fixed_amount[1] ); //or else items will not display
				_ptarget->SetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID, mod->fixed_amount[1] ); //or else items will not display
			}
		}break;
		case 50536:	//Unholy Blight
			{
				//find diseases on target and mark them as undispellable
				int32 val = apply?1:-1;
				for(uint32 x=MAX_POSITIVE_AURAS;x<m_target->m_auras_neg_size;x++)
				{
					Aura *a = m_target->m_auras[x];
					if( a && 
						( a->GetSpellId() == 55095 || a->GetSpellId() == 55078 || a->GetSpellId() == 65142 ) // frost feaver blood plague Ebon Plague
						)
						{ 
							if( apply )
								a->m_flags |= AURAFLAG_CANT_BE_DISPELLED;
							else
								a->m_flags &= ~AURAFLAG_CANT_BE_DISPELLED;
						}
				}
			}break;
		case 91565: // Faerie Fire
			{
				if( apply == false )
					m_target->m_stealth_dissalowed--;
				else
					m_target->m_stealth_dissalowed++;
			}break;
		case 63560: //Dark Transformation
			{
				Unit *caster = GetUnitCaster();
				if( caster == NULL || caster->IsPlayer() == false ) 
					return;
				Pet *summon = SafePlayerCast( caster )->GetSummon();
				if( summon )
				{
					//update our spellbook
					if( apply )
					{

						//might have these as double spells. Remove these as client will not show icons for these ?
						summon->RemoveSpell( dbcSpell.LookupEntryForced(47468) );	//Claws
						summon->RemoveSpell( dbcSpell.LookupEntryForced(47481) );	//Gnaw
						summon->RemoveSpell( dbcSpell.LookupEntryForced(47482) );	//Leap 
						summon->RemoveSpell( dbcSpell.LookupEntryForced(47484) );	//Huddle 

						summon->RemoveSpell( dbcSpell.LookupEntryForced(91776) );	//Claws
						summon->RemoveSpell( dbcSpell.LookupEntryForced(91800) );	//Gnaw
						summon->RemoveSpell( dbcSpell.LookupEntryForced(91809) );	//Leap 
						summon->RemoveSpell( dbcSpell.LookupEntryForced(91838) );	//Huddle 

						summon->AddSpell( dbcSpell.LookupEntryForced(91778), true, false );	//Sweeping Claws
						summon->AddSpell( dbcSpell.LookupEntryForced(91797), true, false );	//Monstrous Blow
						summon->AddSpell( dbcSpell.LookupEntryForced(91802), true, false );	//Shambling Rush
						summon->AddSpell( dbcSpell.LookupEntryForced(91837), true, false );	//Putrid Bulwark

						summon->RemoveAura( 91342, 0, AURA_SEARCH_POSITIVE, MAX_AURAS ); //Shadow Infusion
					}
					else
					{
						summon->RemoveSpell( dbcSpell.LookupEntryForced(91778) );	//Sweeping Claws
						summon->RemoveSpell( dbcSpell.LookupEntryForced(91797) );	//Monstrous Blow
						summon->RemoveSpell( dbcSpell.LookupEntryForced(91802) );	//Shambling Rush
						summon->RemoveSpell( dbcSpell.LookupEntryForced(91837) );	//Putrid Bulwark

						summon->AddSpell( dbcSpell.LookupEntryForced(91776), true, false );	//Claws
						summon->AddSpell( dbcSpell.LookupEntryForced(91800), true, false );	//Gnaw
						summon->AddSpell( dbcSpell.LookupEntryForced(91809), true, false );	//Leap 
						summon->AddSpell( dbcSpell.LookupEntryForced(91838), true, false );	//Huddle 
					}
					summon->SendSpellsToOwner(); 
				}
			}break;
		case 17927:	// Improved Searing Pain
		case 17929:
			{
				if( _ptarget == NULL )
					return;
				if( apply )
				{
					int64 *Critmod = _ptarget->GetCreateIn64Extension( EXTENSION_ID_SEARING_PAIN_CRITMOD );
					*Critmod = mod->m_amount;
					int64 *HealthLimit = _ptarget->GetCreateIn64Extension( EXTENSION_ID_SEARING_PAIN_HP_LIMIT );
					*HealthLimit = dbcSpell.LookupEntryForced( 17927 )->eff[1].EffectBasePoints;
				}
				else
				{
					_ptarget->SetExtension( EXTENSION_ID_SEARING_PAIN_CRITMOD, NULL );
					_ptarget->SetExtension( EXTENSION_ID_SEARING_PAIN_HP_LIMIT, NULL );
				}
			}break;
		case 60069: //Dispersion - immune to mevemnt imparing
		case 1044: //Hand of Freedom - immune to mevemnt imparing
			{
				static uint32 mechanics[5] = { MECHANIC_ROOTED, MECHANIC_ENSNARED, MECHANIC_SHACKLED, MECHANIC_DAZED, MECHANIC_SLOWED };
				
				uint32 MaskToRemove = 0;
				for( uint32 x = 0; x < 5; x++ )
					if( apply )
					{
						m_target->MechanicsDispels[ mechanics[x] ]++; //they say this should NOT break stuns and not make it immune
						MaskToRemove |= ( 1 << mechanics[x] );
					}
					else 
					{
						if( m_target->MechanicsDispels[ mechanics[x] ] > 0 )
							m_target->MechanicsDispels[ mechanics[x] ]--;
					}
				if( MaskToRemove )
				{
					m_target->RemoveAurasMovementImpairing();
					m_target->RemoveAllAurasByMechanicMulti( MaskToRemove, (uint32)(-1), false );
				}
			}break;
		case 76577: //smoke bomb
			{
//				if( _ptarget )
				{
					/* ver 1
					if( apply )
					{
						m_target->m_invisFlag |= INVIS_FLAG_CUSTOM_SMOKEBOMB;
						m_target->m_invisFlag &= ~INVIS_FLAG_NORMAL;
						m_target->m_invisDetect |= INVIS_FLAG_CUSTOM_SMOKEBOMB;
						m_target->m_invisDetect &= ~INVIS_FLAG_NORMAL;
						mod->fixed_amount[0] = getMSTime();
						//wait until Dynamic object finishes targetting everybody to avoid bugging out visibility
						sEventMgr.AddEvent(m_target, &Unit::UpdateVisibility, EVENT_AURA_PERIODIC_UPDATE, 1, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
					}
					else
					{
						m_target->m_invisFlag &= ~INVIS_FLAG_CUSTOM_SMOKEBOMB;
						m_target->m_invisFlag |= INVIS_FLAG_NORMAL;
						m_target->m_invisDetect &= ~INVIS_FLAG_CUSTOM_SMOKEBOMB;
						m_target->m_invisDetect |= INVIS_FLAG_NORMAL;
						//no need for double update
						if( sEventMgr.HasEvent( m_target, EVENT_AURA_PERIODIC_UPDATE ) == false )
						{
							//if we stealth / unstealth players too quickly they might bug out and remain invisible ?
							if( getMSTime() - mod->fixed_amount[0] < 1000 )
								sEventMgr.AddEvent(m_target, &Unit::UpdateVisibility, EVENT_AURA_PERIODIC_UPDATE, 1000, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
							else
								sEventMgr.AddEvent(m_target, &Unit::UpdateVisibility, EVENT_AURA_PERIODIC_UPDATE, 1, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
						}
					} */
					/* ver 2
					if( apply )
					{
						m_target->m_invisFlag |= INVIS_FLAG_CUSTOM_SMOKEBOMB;	//outsiders can't see us
//						m_target->m_invisFlag &= ~INVIS_FLAG_NORMAL;			//we can see outside. 
						m_target->m_invisDetect |= INVIS_FLAG_CUSTOM_SMOKEBOMB;	//we can see inside
//						m_target->m_invisDetect &= ~INVIS_FLAG_NORMAL;
						mod->fixed_amount[0] = getMSTime();
						//wait until Dynamic object finishes targetting everybody to avoid bugging out visibility
						sEventMgr.AddEvent(m_target, &Unit::UpdateVisibility, EVENT_AURA_PERIODIC_UPDATE, 1, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
					}
					else
					{
						m_target->m_invisFlag &= ~INVIS_FLAG_CUSTOM_SMOKEBOMB;
//						m_target->m_invisFlag |= INVIS_FLAG_NORMAL;
						m_target->m_invisDetect &= ~INVIS_FLAG_CUSTOM_SMOKEBOMB;
//						m_target->m_invisDetect |= INVIS_FLAG_NORMAL;
						//no need for double update
						if( sEventMgr.HasEvent( m_target, EVENT_AURA_PERIODIC_UPDATE ) == false )
						{
							//if we stealth / unstealth players too quickly they might bug out and remain invisible ?
							if( getMSTime() - mod->fixed_amount[0] < 1000 )
								sEventMgr.AddEvent(m_target, &Unit::UpdateVisibility, EVENT_AURA_PERIODIC_UPDATE, 1000, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
							else
								sEventMgr.AddEvent(m_target, &Unit::UpdateVisibility, EVENT_AURA_PERIODIC_UPDATE, 1, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
						}
					} */
//					m_target->UpdateVisibility();
					if( _ptarget )
					{
						if( apply == true )
						{
							//break stealth of enemy players
							//there was a 2 page long debate about this
							if( _ptarget->IsStealth() )
							{
								Unit *u_caster = GetUnitCaster();
								if( u_caster && isAttackable( u_caster, _ptarget, false ) )
									_ptarget->RemoveAura( _ptarget->m_stealth, 0, AURA_SEARCH_POSITIVE );
							}/**/
							if( GetSpellProto()->CanCastFilterHandler != NULL )
							{
								SpellCanTargetedScript *so = new SpellCanTargetedScript;
								so->CanTargetedFilterHandler = GetSpellProto()->CanTargetedFilterHandler;
								so->OwnerSpell = GetSpellProto();
								so->CasterGUID = GetCasterGUID();
								Unit *u_caster = GetUnitCaster();
								if( u_caster && u_caster->GetGroup() )
									so->CasterGroupId = u_caster->GetGroup()->GetID();
								_ptarget->mSpellCanTargetedOverrideMap.push_front( so );
								//mod->extension = (void*)so;	//!!! can't be the same as for SpellAuraModRegisterCanCastScript
								mod->fixed_amount[0] = (uint32)so;	//!!! can't be the same as for SpellAuraModRegisterCanCastScript

								//should run this script on everyone that is already targetting us with a spell
								_ptarget->AquireInrangeLock(); //make sure to release lock before exit function !
								InrangeLoopExitAutoCallback AutoLock;
								for(InRangeSetRecProt::iterator itr = _ptarget->GetInRangeSetBegin( AutoLock ); itr != _ptarget->GetInRangeSetEnd(); itr++ )
								{
									// don't add objects that are not units and that are dead
									if( !( (*itr)->IsUnit() ) )
										continue;

									if( SafeUnitCast( *itr )->isCasting() == false )
										continue;
									if( SafeUnitCast( *itr )->GetCurrentSpell()->m_targets.m_unitTarget != _ptarget->GetGUID() )
										continue;
									uint32 NewError = GetSpellProto()->CanTargetedFilterHandler( SafeUnitCast( *itr )->GetCurrentSpell(), *itr, _ptarget, so );
									if( NewError != SPELL_DID_HIT_SUCCESS )
										SafeUnitCast( *itr )->GetCurrentSpell()->safe_cancel();
								}
								_ptarget->ReleaseInrangeLock(); //make sure to release lock before exit function !
							}
						}
						else
						{
							SimplePointerListNode<SpellCanTargetedScript> *itr,*itr2;
							for(itr = _ptarget->mSpellCanTargetedOverrideMap.begin(); itr != _ptarget->mSpellCanTargetedOverrideMap.end();)
							{
								itr2 = itr;
								itr = itr->Next();
//								if( (void*)(itr2->data) == (void*)mod->extension )	//!!! can't be the same as for SpellAuraModRegisterCanCastScript
								if( (uint32)(itr2->data) == (uint32)mod->fixed_amount[0] )
								{
									_ptarget->mSpellCanTargetedOverrideMap.remove( itr2, 1 );//no leaks pls = delete on remove
//									mod->extension = NULL; //!!! can't be the same as for SpellAuraModRegisterCanCastScript
								}
							}
						}
						//also limit casters inside the smoke to cast outside
						SpellAuraModRegisterCanCastScript( apply );
					}
				}
			}break;
		case 44445: // Hot Streak - disable Arcane missle proc
		case 44446:
		case 44448:
		case 44546: // Brain freez - disable Arcane missle proc
		case 44548:
		case 44549:
			{
				if( _ptarget )
				{
					if( apply )
					{
						*(_ptarget->GetCreateIn64Extension( EXTENSION_ID_DISABLE_HOT_STREAK ) ) = 1; //mark that we have this tallent instead searching through 200 passive spells all the time
						//!! this should be executed AFTER loading spell 79684
						_ptarget->UnRegisterProcStruct( _ptarget, 79684, 1, 1 );
					}
					else
					{
						_ptarget->SetExtension( EXTENSION_ID_DISABLE_HOT_STREAK, NULL );
						_ptarget->UnRegisterProcStruct( _ptarget, 79684, 1, 2 );
					}
				}
			}break;
		case 16929: //Thick Hide
		case 16930: 
		case 16931: 
			{
				if( _ptarget == NULL )
					return;
				int32 ss = _ptarget->GetShapeShift();
				if( apply )
				{
					// Increases your Armor contribution from cloth and leather items by $s1%, 
					// increases armor while in Bear Form by an additional $s2%, 
					if( ss == FORM_BEAR )
						mod->m_amount += GetSpellProto()->eff[1].EffectBasePoints;
					_ptarget->BaseResistanceModPctPos[ SCHOOL_NORMAL ] += mod->m_amount;
				}
				else
				{
					_ptarget->BaseResistanceModPctPos[ SCHOOL_NORMAL ] -= mod->m_amount;
				}
				_ptarget->CalcResistance( SCHOOL_NORMAL );
			}break;
		case 51271: //pilar of frost. Effect immunity
			{
				if( _ptarget == NULL )
					return;
				if(apply)
				{
					_ptarget->m_SpellEffectImmunities[ SPELL_EFFECT_KNOCK_BACK ]++;
					_ptarget->m_SpellEffectImmunities[ SPELL_EFFECT_KNOCK_BACK2 ]++;
					_ptarget->m_SpellEffectImmunities[ SPELL_EFFECT_KNOCK_BACK3 ]++;
				}
				else 
				{
					if( _ptarget->m_SpellEffectImmunities[ SPELL_EFFECT_KNOCK_BACK ] > 0 )
						_ptarget->m_SpellEffectImmunities[ SPELL_EFFECT_KNOCK_BACK ]--;
					if( _ptarget->m_SpellEffectImmunities[ SPELL_EFFECT_KNOCK_BACK2 ] > 0 )
						_ptarget->m_SpellEffectImmunities[ SPELL_EFFECT_KNOCK_BACK2 ]--;
					if( _ptarget->m_SpellEffectImmunities[ SPELL_EFFECT_KNOCK_BACK3 ] > 0 )
						_ptarget->m_SpellEffectImmunities[ SPELL_EFFECT_KNOCK_BACK3 ]--;
				}
			}break;
		case 96219: //Holy Walk - Ruthless Gladiator's Mooncloth Leggings
			{
				float val;
				if(apply)
				{
					mod->fixed_amount[0] = float2int32( m_target->m_base_runSpeed * mod->m_amount );
					val = mod->fixed_amount[0] / 100.0f;
				}
				else
					val = 0;
				m_target->m_minSpeed = val;
				m_target->UpdateSpeed();
			}break;
		case 96268: //Death's Advance
			{
				float val;
				if(apply)
				{
					mod->fixed_amount[0] = float2int32( m_target->m_base_runSpeed * mod->m_amount );
					val = mod->fixed_amount[0] / 100.0f;
				}
				else
					val = 0;
				m_target->m_minSpeed = val;
				m_target->UpdateSpeed();
			}break;

		case 63611:	//Improved Blood Presence - this is frost and unholy part
			{
				if( _ptarget == NULL )
					return;
/*				float val = MAX( 0.0001f, 1.0f - ((float)(mod->m_amount))/100.0f ); //postive reduces dmg taken, negative increases dmg taken
				if( apply == false )
					val = 1.0f / val;
				for(uint32 i=0;i<SCHOOL_COUNT;i++)
					_ptarget->DamageTakenPctMod[i] *= val;	//reduce dmg not increase
				*/
				int32 val;
				if( apply )
					val = -mod->m_amount;
				else
					val = +mod->m_amount;
				for(uint32 i=0;i<SCHOOL_COUNT;i++)
					m_target->DamageTakenPctMod[i] += val;	//reduce dmg not increase
			}break;
		case 50365:	//Improved Blood Presence
		case 50371:	
			{
				if( _ptarget == NULL )
					return;
				if( mod->i == 2 )	//just hope this is the effect that mods rune regen
				{
					float val;
					if(apply)
						val = ((float)(mod->m_amount))/100.0f;	//value is negative, for us it is menth to increase the regen value
					else
						val = -((float)(mod->m_amount))/100.0f;
					for( uint32 i=0;i<RUNE_TYPE_COUNT;i++)
						_ptarget->m_mod_rune_power_gain_coeff[ i ] += val;
					_ptarget->UpdateStats();
				}
				if( mod->i == 1 )
				{
					_ptarget->AttackerCritChanceMod[0] += (apply?mod->m_amount:-mod->m_amount);
				}
			}break;
		case 51460:	//Runic Corruption
			{
				if( _ptarget == NULL )
					return;
				float val;
				if(apply)
					val = ((float)(mod->m_amount))/100.0f;	//value is negative, for us it is menth to increase the regen value
				else
					val = -((float)(mod->m_amount))/100.0f;
				for( uint32 i=0;i<RUNE_TYPE_COUNT;i++)
					_ptarget->m_mod_rune_power_gain_coeff[ i ] += val;
				_ptarget->UpdateStats();

				//disable / enable the proc of Runic Empowerment
				if( apply )
					_ptarget->UnRegisterProcStruct( _ptarget, 81229, 1, 1 );
				else
					_ptarget->UnRegisterProcStruct( _ptarget, 81229, 1, 2 );

			}break;
		case 44544:	//Fingers of Frost
			{
				if( _ptarget == NULL )
					return;
				if( apply )
					_ptarget->m_ForceTargetFrozen++;
				else
					_ptarget->m_ForceTargetFrozen--;
			}break;
/*		case 19263:	//Deterrence
			{
				if( _ptarget == NULL )
					return;
				if( apply )
				{
					//note that these will be overwritten because we do not have a shield. Temp hackfix to avoid diminishing returns on miss chance
					_ptarget->ModFloatValue(PLAYER_BLOCK_PERCENTAGE, 101.0f);
					_ptarget->ModFloatValue(PLAYER_DODGE_PERCENTAGE, 101.0f);
					_ptarget->ModFloatValue(PLAYER_PARRY_PERCENTAGE, 101.0f);
				}
				else
				{
					//note that these will be overwritten because we do not have a shield. Temp hackfix to avoid diminishing returns on miss chance
					if( _ptarget->GetFloatValue(PLAYER_BLOCK_PERCENTAGE) >= 100.0f )
						_ptarget->ModFloatValue(PLAYER_BLOCK_PERCENTAGE, -101.0f);
					if( _ptarget->GetFloatValue(PLAYER_DODGE_PERCENTAGE) >= 100.0f )
						_ptarget->ModFloatValue(PLAYER_DODGE_PERCENTAGE, -101.0f);
					if( _ptarget->GetFloatValue(PLAYER_PARRY_PERCENTAGE) >= 100.0f )
						_ptarget->ModFloatValue(PLAYER_PARRY_PERCENTAGE, -101.0f);
				}
			}break; */
		case 17003: //Heart of the Wild
		case 17004: 
		case 17005: 
			{
				if( _ptarget == NULL )
					return;
				//remove previous application
				int32 ss = _ptarget->GetShapeShift();
				if( apply )
				{
					mod->fixed_amount[0] = 0;
					mod->m_amount = 0;
					if( ss == FORM_BEAR )
					{
						mod->fixed_amount[0] = 1;
						mod->m_amount = GetSpellProto()->eff[2].EffectBasePoints;
					}
					if( ss == FORM_CAT )
					{
						mod->fixed_amount[0] = 2;
						mod->m_amount = GetSpellProto()->eff[1].EffectBasePoints;
					}
				}
				if( mod->fixed_amount[0] == 1 )
					SpellAuraModTotalStatPerc( apply );
				else if( mod->fixed_amount[0] == 2 )
					SpellAuraModAttackPowerPct( apply );
			}break;
		case 79146:	//Sanguinary Vein
		case 79147:
			{
				if( _ptarget && mod->i == 0 )
				{
					if( apply )
					{
						uint64 *p = (uint64 *)_ptarget->GetCreateIn64Extension( EXTENSION_ID_SANGUINARY_VEIN_VAL );
						*p = mod->m_amount;
						uint64 *p2 = (uint64 *)_ptarget->GetCreateIn64Extension( EXTENSION_ID_SANGUINARY_VEIN_CHANCE);
						*p2 = GetSpellProto()->eff[1].EffectBasePoints; //chance for bleeds to not remove gauge
					}
					else
					{
						_ptarget->SetExtension( EXTENSION_ID_SANGUINARY_VEIN_VAL, NULL );
						_ptarget->SetExtension( EXTENSION_ID_SANGUINARY_VEIN_CHANCE, NULL );
					}
				}
			}break;
        case 68996: // Worgen Transform Spell two forms
        case 97709: // Worgen Transform Spell
        {
			//since 144380 no actions are required
/*			if( apply )
				m_target->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_WORGEN_TRANSFORM);           
			else
				m_target->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_WORGEN_TRANSFORM);          
				*/
        }break;
/*		case 91713:	//Nether Ward
			{
				if( m_target->IsPlayer() )
				{
					if( apply )
					{
						SafePlayerCast( m_target )->mSpellReplaces[ 687 ] = 91711;		//for demon armor send this value
						SafePlayerCast( m_target )->mSpellReplaces[ 28176 ] = 91711;	//for fel armor send this value
					}
					else
					{
						SafePlayerCast( m_target )->mSpellReplaces[ 687 ] = 0;
						SafePlayerCast( m_target )->mSpellReplaces[ 28176 ] = 0;
					}
				}
			}break; */
		case 80240:	//Bane of Havoc
			{
				Unit *caster = GetUnitCaster();
				if( caster == NULL )
					return; //nothing we can do here
				if( apply )
				{
					ProcTriggerSpell *pts = new ProcTriggerSpell(GetSpellProto(), this);
					pts->procFlags = PROC_ON_ANY_DAMAGE_DONE;
					pts->procChance = 100;
					pts->spellId = 85455;
					pts->procInterval = 0; //every damage
					pts->caster = m_target->GetGUID();		//orginally blizz made it so beacon registers this cast
					caster->RegisterProcStruct( pts );
				}
				else
				{
					caster->UnRegisterProcStruct( this );
				}
			}break;
/*		case 82368:	//Victorious
			{
				if( apply )
					m_target->SetAuraStateFlag( AURASTATE_LASTKILLWITHHONOR );
				else
					m_target->RemoveAuraStateFlag( AURASTATE_LASTKILLWITHHONOR );
			}break; */
		case 34482:	//Careful Aim
		case 34483:
			{
				if( _ptarget && mod->i == 0 )
				{
					if( apply )
					{
						uint64 *p = (uint64 *)_ptarget->GetCreateIn64Extension( EXTENSION_ID_CAREFUL_AIM_CRITCHANCE );
						*p = mod->m_amount;
					}
					else
					{
						_ptarget->SetExtension( EXTENSION_ID_CAREFUL_AIM_CRITCHANCE, NULL );
					}
				}
			}break;
		case 61336:	//Survival Instincts
			{
/*				float val = MAX( 0.0001f, 1.0f - ((float)(mod->m_amount))/100.0f ); //postive reduces dmg taken, negative increases dmg taken
				if( apply == false )
					val = 1.0f / val;
				for(uint32 i=0;i<SCHOOL_COUNT;i++)
					m_target->DamageTakenPctMod[i] *= val;	//reduce dmg not increase */
				int32 val;
				if( apply )
					val = -mod->m_amount;
				else
					val = +mod->m_amount;
				for(uint32 i=0;i<SCHOOL_COUNT;i++)
					m_target->DamageTakenPctMod[i] += val;	//reduce dmg not increase
			}break;
		case 57819: //Argent Champion
		case 57820: //Ebon Champion
		case 57822: //Wyrmrest Champion
		case 93795: //Stormwind Champion
		case 93805: //Ironforge Champion
		case 93806: //Darnassus Champion
		case 93811: //Exodar Champion
		case 93816: //Gilneas Champion
		case 93821: //Gnomeregan Champion
		case 93825: //Orgrimmar Champion
		case 93827: //Darkspear Champion
		case 93828: //Silvermoon Champion
		case 93830: //Bilgewater Champion
		case 94462: //Undercity Champion
		case 94463: //Thunder Bluff Champion
			{
				if( _ptarget )
				{
					if( apply )
						_ptarget->dungeon_factiongain_redirect = mod->m_miscValue;
					else
						_ptarget->dungeon_factiongain_redirect = 0;
				}
			}break;
		case 87840: //running wild
			{
				if( apply )
				{
					Unit *target = GetTarget();
//					if( target->IsPlayer() )
//						SafePlayerCast( target )->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_WORGEN_TRANSFORM3);
					m_target->SetUInt32Value( UNIT_FIELD_MOUNTDISPLAYID, m_target->GetUInt32Value( UNIT_FIELD_NATIVEDISPLAYID ) );
				}
				else
				{
					Unit *target = GetTarget();
//					if( target->IsPlayer() )
//						SafePlayerCast( target )->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_WORGEN_TRANSFORM3);
					m_target->SetUInt32Value( UNIT_FIELD_MOUNTDISPLAYID, 0 );
				}
			}break;
		case 43827:
			{
				if( apply )
					m_target->SetUInt32Value( UNIT_FIELD_DISPLAYID, 30721 );
				else
					m_target->SetUInt32Value( UNIT_FIELD_DISPLAYID, m_target->GetUInt32Value( UNIT_FIELD_NATIVEDISPLAYID ) );
			}break;
		//druid Master Shapeshifter
		case 48411:
//		case 48412:
			{
				if( _ptarget )
				{
					if( apply )
					{
						uint32 spell_to_cast = 0;
						int32 ss = _ptarget->GetShapeShift();
						if( ss == FORM_BEAR )
							spell_to_cast = 48418;
						else if( ss == FORM_CAT )
							spell_to_cast = 48420;
						else if( ss == FORM_MOONKIN )
							spell_to_cast = 48421;
//						else if( ss == FORM_TREE )
//							spell_to_cast = 48411; //self cast ! in tree form we simply allow casting effect 2
						if( spell_to_cast )
						{
		                    SpellCastTargets targets2( _ptarget->GetGUID() );
							SpellEntry *spellInfo = dbcSpell.LookupEntry( spell_to_cast );
							Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
							spell->Init(_ptarget, spellInfo ,true, NULL);
							spell->forced_basepoints[0] = GetSpellProto()->eff[0].EffectBasePoints+1;
							spell->prepare(&targets2);
						}
						mod->fixed_amount[ 0 ] = spell_to_cast;
					}
					else if( mod->fixed_amount[ 0 ] )
					{
						_ptarget->RemoveAura( mod->fixed_amount[ 0 ] );
					}
				}
			}break;
		//Corrupt Soul - boss spell
		case 68839:
			{
				if( apply == false && m_target )
				{
					Unit *u_caster = GetUnitCaster();
					if( u_caster )
						u_caster->create_guardian( 36535, 10*60*1000, 0, NULL, m_target->GetPositionV() );
				}
			}break;
		case 85767:	//Dark Intent
			{
				Unit *caster = GetUnitCaster();
				//no self casts ! this should be available to target only!
				if( caster == m_target )
				{
					if( apply == false )
					{
						//we are the caster and we want our target to loose the buff also
						if( caster && caster->IsPlayer() )
							SafePlayerCast( caster )->RemoveSpellTargets( SPELL_TYPE_INDEX_DARK_INTENT, NULL );
					}
					return;
				}
				if( apply )
				{
					if( caster == NULL )
						return; //nothing we can do here
					if( caster->IsPlayer() )
						SafePlayerCast( caster )->SetSpellTargetType( SPELL_TYPE_INDEX_DARK_INTENT, m_target );	//because we are casting the spell on 2 targets

					//add a proc to original caster
					ProcTriggerSpell *pts = new ProcTriggerSpell(GetSpellProto(),this);
					pts->caster = caster->GetGUID();
					pts->spellId = GetSpellProto()->eff[mod->i].EffectTriggerSpell;
					pts->created_with_value = m_target->GetGUID() >> 32;
					uint32 low_guid = m_target->GetLowGUID();
					pts->custom_holder = (void*)low_guid;		//when we proc our target is the other guy
					caster->RegisterProcStruct(pts);	

					//add a proc to caster's target
					pts = new ProcTriggerSpell(GetSpellProto(), this );
					pts->caster = caster->GetGUID();
					pts->spellId = GetSpellProto()->eff[mod->i].EffectTriggerSpell;
					pts->created_with_value = caster->GetGUID() >> 32;
					low_guid = caster->GetLowGUID();
					pts->custom_holder = (void*)low_guid;		//when we proc our target is the other guy
					caster->RegisterProcStruct(pts);	
				}
				else
				{
					//remove proc from original caster
					if( caster != NULL )
					{
						//we are not the caster, but we want the caster to loose the aura also
						caster->RemoveAura( GetSpellId() );
						caster->UnRegisterProcStruct( this, 0, MAX_AURAS );
					}
				}
			}break;
		case 53563:	//beacon of light
			{
				Unit *caster = GetUnitCaster();
				if( caster == NULL )
					return; //nothing we can do here
				if( apply )
				{
					//make sure the caster does not have any other beacon proc. This might happen if the caster exits map and the aura is nto removed from him
					caster->UnRegisterProcStruct( NULL, GetSpellId(), MAX_AURAS );
					//add a new proc
					ProcTriggerSpell *pts = new ProcTriggerSpell( GetSpellProto(), this );
//					pts->procChance = GetSpellProto()->procChance;
//					pts->procCharges = GetSpellProto()->procCharges;
					pts->spellId = GetSpellProto()->eff[mod->i].EffectTriggerSpell;
					pts->caster = m_target->GetGUID();		//orginally blizz made it so beacon registers this cast
					caster->RegisterProcStruct(pts);	//if caster casts a healing spell
				}
				else
				{
					caster->UnRegisterProcStruct( this );
				}
			}break;
		case 48018:
			{
				//Demonic Circle: Summon -> periodic trigger ? we do not really need it, one cast is enough
				if( _ptarget && _ptarget->GetSession() )
				{
					if( apply )
					{
/*
14480
{SERVER} Packet: (0x2A37) SMSG_AURA_UPDATE PacketSize = 23 TimeStamp = 748102
8F 8F DC 38 03 01 
00 slot
92 BB 00 00 spell
39 00 - 57 = 32 + 16 + 8 + 1 = VISUALAURA_FLAG_HAS_DURATION + VISUALAURA_FLAG_REMOVABLE + VISUALAURA_FLAG_SELF_CAST + VISUALAURA_FLAG_VISIBLE_1
55 
00 
40 7E 05 00 
40 7E 05 00 
{SERVER} Packet: (0x2A37) SMSG_AURA_UPDATE PacketSize = 15 TimeStamp = 749304
8F 8F DC 38 03 01 
02 
B4 F3 00 00 - inexistent spell
19 00 = 16 + 8 + 1 = VISUALAURA_FLAG_REMOVABLE + VISUALAURA_FLAG_VISIBLE_2 + VISUALAURA_FLAG_VISIBLE_1
55 
00
*/
						//!! this spell does not exist in our DBC, client however knows about it !
						sStackWorldPacket(data, SMSG_AURA_UPDATE, 20 );
						data << _ptarget->GetNewGUID();
						data << uint8( 99 );	//slot, it is fictional !
						data << uint32( 62388 );	//spell id
						data << uint16( VISUALAURA_FLAG_REMOVABLE | VISUALAURA_FLAG_SELF_CAST | VISUALAURA_FLAG_VISIBLE_1 );	//flags, cause i say so
						data << uint8( 85 );	//lvl 80 rules
						data << uint8( 0 );		//stack, ofc it does not stack
						_ptarget->GetSession()->SendPacket( &data );
					}
					else
					{
						sStackWorldPacket(data, SMSG_AURA_UPDATE, 20 );
						data << _ptarget->GetNewGUID();
						data << uint8( 99 );	//slot, it is fictional !
						data << uint32( 0 );	//spell id = clear
						_ptarget->GetSession()->SendPacket( &data );
					}
				}
			}break;
		//Impurity
/*		case 49220:	
		case 49633:
		case 49635:
		case 49636:
		case 49638:	
			{
				if( _ptarget )
				{
					if( apply )
						_ptarget->ModAbillityAPBonusCoef += mod->m_amount / 100.0f;
					else
						_ptarget->ModAbillityAPBonusCoef -= mod->m_amount / 100.0f;
				}
			}break; */
		case 25860:	//Reindeer Transformation
		case 62061:	//Festive Holiday Mount
			{
				if( _ptarget )
				{
					if( apply )
						_ptarget->mount_look_override = 22724;
					else
						_ptarget->mount_look_override = 0;
				}
			}break;
		case 62062: //Brewfestive Holiday Mount
			{
				if( _ptarget )
				{
					if( apply )
						_ptarget->mount_look_override = 24757;
					else
						_ptarget->mount_look_override = 0;
				}
			}break;
		case 53270: //hunter : beast mastery
			{
				if( _ptarget )
				{
					if( apply )
					{
						_ptarget->pet_extra_talent_points += mod->m_amount;
						if( _ptarget->GetSummon() )
							_ptarget->GetSummon()->smsg_TalentsInfo();
					}
					else
					{
						_ptarget->pet_extra_talent_points -= mod->m_amount;
						//only take these actions on talent reset 
						//_ptarget->ForceAllPetTalentAndSpellReset();
					}
				}
			}break;
		case 53042: //mixology
			{
				if( _ptarget )
				{
					if( apply )
						_ptarget->has_mixology_talent++;
					else if( _ptarget->has_mixology_talent )
						_ptarget->has_mixology_talent--;
				}
			}break;
		case 17619: //alchemist stone
			{
				if( _ptarget )
				{
					if( apply )
						_ptarget->has_alchemist_stone_effect++;
					else if( _ptarget->has_alchemist_stone_effect )
						_ptarget->has_alchemist_stone_effect--;
				}
			}break;
	// Deadly Throw Interrupt
	case 32748:
	{
		if ( m_target == NULL )
		{ 
			return;
		}

		uint32 school = 0;
		if(m_target->GetCurrentSpell())
		{
			school = m_target->GetCurrentSpell()->GetProto()->School;
		}
		m_target->InterruptSpell();
		m_target->SchoolCastPrevent[school]=3000+getMSTime();
	}break;
	//improved sprint effect
	case 30918:
	{
		m_target->RemoveAurasMovementImpairing();
	}break;
/*	//Requires No Ammo
	case 46699:
		{
			if( m_target->IsPlayer() )
				SafePlayerCast( m_target )->m_requiresNoAmmo = apply;

		}break; */
	//Hunter - Bestial Wrath & The Beast Within
	//Both should provide immunity for all CC effect, but the dbc specifies only stuns. Imba!
	case 19574:
	case 34471:
	case 54508: // Demonic Empowerment - Felguard - provide immunity also
		{
			static uint32 mechanics[16] = { MECHANIC_CHARMED, MECHANIC_DISORIENTED,	MECHANIC_DISTRACED, MECHANIC_FLEEING, 
											MECHANIC_ROOTED, MECHANIC_ASLEEP, MECHANIC_ENSNARED, MECHANIC_STUNNED,
											MECHANIC_FROZEN, MECHANIC_INCAPACIPATED, MECHANIC_POLYMORPHED, MECHANIC_BANISHED,
											MECHANIC_INFECTED, MECHANIC_HORRIFIED, MECHANIC_TURNED, MECHANIC_SAPPED };
			
			bool ProvideImmunity = false;
			if( GetSpellId() == 54508 )
				ProvideImmunity = true;
			uint32 MaskToRemove = 0;
			for( uint32 x = 0; x < 16; x++ )
				if( apply )
				{
					if( ProvideImmunity ) 
						m_target->MechanicsDispels[ mechanics[x] ]++; //they say this should only break stuns and not make it immune
					MaskToRemove |= ( 1 << mechanics[x] );
				}
				else 
				{
					if( ProvideImmunity && m_target->MechanicsDispels[ mechanics[x] ] > 0 )
						m_target->MechanicsDispels[ mechanics[x] ]--;
				}
			if( MaskToRemove )
				m_target->RemoveAllAurasByMechanicMulti( MaskToRemove, (uint32)(-1), false );
			
		}break;
	//Warlock - Demonic Knowledge
	case 35696:
		{
			if ( m_target->IsPet() )
			{
				Unit* PetOwner;
				if ( SafePetCast( m_target )->GetPetOwner() )
				{
					PetOwner = SafePetCast( m_target )->GetPetOwner();
					if( apply == true )
					{
						uint32 val1 = m_target->GetUInt32Value( UNIT_FIELD_STAT2 ); // stamina
						uint32 val2 = m_target->GetUInt32Value( UNIT_FIELD_STAT3 ); // intelect
						uint32 val0 = val1+val2;
						mod->m_amount = val0*mod->m_amount/100;
					}
					Unit *old_target = m_target;
					m_target = PetOwner;
					SpellAuraModDamageDone( apply );	//avoid code redundancy, let same function handle us
					if( m_target = PetOwner )			//maybe aura got removed ...call chains are evil with shared memory ;)
						m_target = old_target;
				}
			}
		}break;
	//paladin - Blessing of Light.
/*	case 19977:
	case 19978:
	case 19979:
	case 27144:
	case 32770:
	case 27145:
	case 25890:
		{
			if( mod->i == 0 )
				SMTMod_On_target( apply, false, 0x9B56A8F5, mod->m_amount ); //holy light
			if( mod->i == 1 )
				SMTMod_On_target( apply, false, 0x333C4740, mod->m_amount ); //flash of light
		}break;*/
	//shaman - Healing Way - effect
/*	case 29203:
		{
			SMTMod_On_target( apply, true, 0x08F1A7EF, mod->m_amount ); // Healing Wave
		}break;*/
	//druid - mangle
/*	case 33878:
	case 33986:
	case 33987:
	case 33876:
	case 33982:
	case 33983:
		{
			int32 val = (apply) ? 30 : -30;
			m_target->ModDamageTakenByMechPCT[MECHANIC_BLEEDING] += float( val ) / 100.0f;
		}break;*/
	//warrior - berserker rage (Forcing a dummy aura, so we can add the missing 4th effect).
	case 18499:
		{
			if( !m_target->IsPlayer() )
			{ 
				return;
			}

			Player * p = SafePlayerCast( m_target );

			if( apply )
				p->rageFromDamageTaken += 100;
			else
				p->rageFromDamageTaken -= 100;

			if( apply )
			{
//				p->MechanicsDispels[ GetSpellProto()->eff[i].EffectMiscValue ]++;
//				p->RemoveAllAurasByMechanic( GetSpellProto()->eff[i].EffectMiscValue , (uint32)(-1) , false );
//				p->MechanicsDispels[ MECHANIC_STUNNED ]++;
//				p->RemoveAllAurasByMechanic( MECHANIC_STUNNED , (uint32)(-1) , false );
				p->MechanicsDispels[ MECHANIC_SAPPED ]++;
				p->MechanicsDispels[ MECHANIC_INCAPACIPATED ]++;
				p->MechanicsDispels[ MECHANIC_FLEEING ]++;
				p->RemoveAllAurasByMechanicMulti( (1<<MECHANIC_SAPPED)|(1<<MECHANIC_INCAPACIPATED)|(1<<MECHANIC_FLEEING) , (uint32)(-1) , false );
			}
			else
			{
//				if( p->MechanicsDispels[ GetSpellProto()->eff[i].EffectMiscValue ] > 0 )
//					p->MechanicsDispels[ GetSpellProto()->eff[i].EffectMiscValue ]--;
//				if( p->MechanicsDispels[ MECHANIC_STUNNED ] > 0 )
//					p->MechanicsDispels[ MECHANIC_STUNNED ]--;
				if( p->MechanicsDispels[ MECHANIC_SAPPED ] > 0 )
					p->MechanicsDispels[ MECHANIC_SAPPED ]--;
				if( p->MechanicsDispels[ MECHANIC_INCAPACIPATED ] > 0 )
					p->MechanicsDispels[ MECHANIC_INCAPACIPATED ]--;
				if( p->MechanicsDispels[ MECHANIC_FLEEING ] > 0 )
					p->MechanicsDispels[ MECHANIC_FLEEING ]--;
			}
		}break;
	//rogue - Blade Flurry
	case 13877:
		{
			if(apply)
				m_target->AddExtraStrikeTarget(GetSpellProto(), 0x00FFFFFF);
			else
				m_target->RemoveExtraStrikeTarget(GetSpellProto());
		}break;
	//warrior - sweeping strikes
	case 12328:
		{
			if(apply)
				m_target->AddExtraStrikeTarget(GetSpellProto(), 10);
			else
				m_target->RemoveExtraStrikeTarget(GetSpellProto());
		}break;
	//taming rod spells
	case 19548:	{                 //invoke damage to trigger attack
		if (apply)
		{
			m_target->GetAIInterface()->AttackReaction( GetUnitCaster(), 10, 0);
			break;
		}
		else
			TamingSpellid=19597; //uses Spelleffect3 #19614
	}break;
	case 19674:	{
		if (apply)
		{
			m_target->GetAIInterface()->AttackReaction( GetUnitCaster(), 10, 0);
			break;
		}
		else
			TamingSpellid=19677;
	}break;
	case 19687:	{
		if (apply)
		{
			m_target->GetAIInterface()->AttackReaction( GetUnitCaster(), 10, 0);
			break;
		}
		else
			TamingSpellid=19676;
	}break;
	case 19688:	{
		if (apply)
		{
			m_target->GetAIInterface()->AttackReaction( GetUnitCaster(), 10, 0);
			break;
		}
		else
			TamingSpellid=19678;
	}break;
	case 19689:	{
		if (apply)
		{
			m_target->GetAIInterface()->AttackReaction( GetUnitCaster(), 10, 0);
			break;
		}
		else
			TamingSpellid=19679;
	}break;
	case 19692:	{
		if (apply)
		{
			m_target->GetAIInterface()->AttackReaction( GetUnitCaster(), 10, 0);
			break;
		}
		else
			TamingSpellid=19680;
	}break;
	case 19693:	{
		if (apply)
		{
			m_target->GetAIInterface()->AttackReaction( GetUnitCaster(), 10, 0);
			break;
		}
		else
			TamingSpellid=19684;
	}break;
	case 19694:	{
		if (apply)
		{
			m_target->GetAIInterface()->AttackReaction( GetUnitCaster(), 10, 0);
			break;
		}
		else
			TamingSpellid=19681;
	}break;
	case 19696:	{
		if (apply)
		{
			m_target->GetAIInterface()->AttackReaction( GetUnitCaster(), 10, 0);
			break;
		}
		else
			TamingSpellid=19682;
	}break;
	case 19697:	{
		if (apply)
		{
			m_target->GetAIInterface()->AttackReaction( GetUnitCaster(), 10, 0);
			break;
		}
		else
			TamingSpellid=19683;
	}break;
	case 19699:	{
		if (apply)
		{
			m_target->GetAIInterface()->AttackReaction( GetUnitCaster(), 10, 0);
			break;
		}
		else
			TamingSpellid=19685;
	}break;
	case 19700:	{
		if (apply)
		{
			m_target->GetAIInterface()->AttackReaction( GetUnitCaster(), 10, 0);
			break;
		}
		else
			TamingSpellid=19686;
	}break;
	case 30099:	{
		if (apply)
		{
			m_target->GetAIInterface()->AttackReaction( GetUnitCaster(), 10, 0);
			break;
		}
		else
			TamingSpellid=30100;
	}break;
	case 30102:	{
		if (apply)
		{
			m_target->GetAIInterface()->AttackReaction( GetUnitCaster(), 10, 0);
			break;
		}
		else
			TamingSpellid=30103;
	}break;
	case 30105:	{
		if (apply)
		{
			m_target->GetAIInterface()->AttackReaction( GetUnitCaster(), 10, 0);
			break;
		}
		else
			TamingSpellid=30104;
	}break;
	case 30646:	{
		if (apply)
		{
			m_target->GetAIInterface()->AttackReaction( GetUnitCaster(), 10, 0);
			break;
		}
		else
			TamingSpellid=30647;
	}break;
	case 30653:	{
		if (apply)
		{
			m_target->GetAIInterface()->AttackReaction( GetUnitCaster(), 10, 0);
			break;
		}
		else
			TamingSpellid=30648;
	}break;
	case 30654:	{
		if (apply)
		{
			m_target->GetAIInterface()->AttackReaction( GetUnitCaster(), 10, 0);
			break;
		}
		else
			TamingSpellid=30648;
	}break;
/*	case 16972://Predatory Strikes
	case 16974:
	case 16975:
	{
		if(apply)
		{
			SetPositive();
			mod->fixed_amount[0] = (mod->m_amount * m_target->getLevel())/100;
			m_target->ModUnsigned32Value(UNIT_FIELD_ATTACK_POWER_MOD_POS,mod->fixed_amount[0]);
		}else
			m_target->ModUnsigned32Value(UNIT_FIELD_ATTACK_POWER_MOD_POS, -mod->fixed_amount[0]);
		m_target->CalcDamage();
	}break;*/
	case 974:	//Earth Shield
	case 32593:
	case 32594:
		{
			if(apply)
			{
				ProcTriggerSpell *pts = new ProcTriggerSpell(GetSpellProto(),this);
				pts->caster = m_casterGuid;
				pts->procChance = GetSpellProto()->procChance;
				int charges = GetSpellProto()->procCharges;
				Unit *uc = GetUnitCaster();
				if( (GetSpellProto()->c_is_flags & SPELL_FLAG_IS_REQUIRING_SM_MOD) && uc != NULL && uc->SM_Mods )
				{
					SM_FIValue( uc->SM_Mods->SM_FCharges, &charges, GetSpellProto()->GetSpellGroupType() );
					SM_PIValue( uc->SM_Mods->SM_PCharges, &charges, GetSpellProto()->GetSpellGroupType() );
	#ifdef COLLECTION_OF_UNTESTED_STUFF_AND_TESTERS
					float spell_flat_modifers=0;
					float spell_pct_modifers=0;
					SM_FIValue(GetUnitCaster()->SM_FCharges,&spell_flat_modifers,GetSpellProto()->GetSpellGroupType());
					SM_FIValue(GetUnitCaster()->SM_PCharges,&spell_pct_modifers,GetSpellProto()->GetSpellGroupType());
					if(spell_flat_modifers!=0 || spell_pct_modifers!=0)
						printf("!!!!!spell charge bonus mod flat %f , spell range bonus pct %f , spell range %f, spell group %u\n",spell_flat_modifers,spell_pct_modifers,maxRange,m_spellInfo->GetSpellGroupType());
	#endif
				}
				pts->procCharges = charges;
				m_target->RegisterProcStruct(pts);
			}
			else
			{
				m_target->UnRegisterProcStruct( this );
			}
		}break;
	case 126: //Eye of Kilrogg
		{
			/*if(m_target->IsInWorld() == false)
				return;

			if(!apply)
			{
				m_target->SetUInt64Value(PLAYER_FARSIGHT,0);
				Creature *summon = m_target->GetMapMgr()->GetCreature(m_target->GetUInt32Value(UNIT_FIELD_SUMMON));
				m_target->SetUInt64Value(UNIT_FIELD_SUMMON, 0);
				m_target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_LOCK_PLAYER);

				if(summon)
				{
					summon->RemoveFromWorld(false,true);
					delete summon;
				}
				m_target->m_noInterrupt--;
				if(m_target->m_noInterrupt < 0)
					m_target->m_noInterrupt = 0;
			}*/
		}break;
	case 17056://Furor
	case 17058:
	case 17059:
	case 17060:
	case 17061:
		{
			if( !_ptarget )
				return;
			int64 *p;
			if( apply ) 
			{
				if( m_target->getClass() == DRUID )
				{
					uint32 ss = _ptarget->GetShapeShift();
					mod->fixed_amount[ 2 ] = ss;
					if( ss == FORM_CAT )
					{
						//allows to keep energy
						if( (int32)m_target->GetPower( POWER_TYPE_ENERGY ) < mod->m_amount )
						{
							p = _ptarget->GetCreateIn64Extension( EXTENSION_ID_FUROR_ENERGY );
//							m_target->SetPower( POWER_TYPE_ENERGY, mod->m_amount );
							m_target->SetPower( POWER_TYPE_ENERGY, *p );
							m_target->UpdatePowerAmm( true, POWER_TYPE_ENERGY );
						}
					}
					else if( ss == FORM_BEAR && RandChance( mod->m_amount ) )
					{
						//chance to add power
						m_target->ModPower( POWER_TYPE_RAGE, 10 * 10 );
						m_target->UpdatePowerAmm( true, POWER_TYPE_RAGE );
					}
				}
			}
			else
			{
//					uint32 ss = _ptarget->GetShapeShift();
					uint32 ss = mod->fixed_amount[ 2 ];
					if( ss == FORM_CAT )
					{
						//remember energy amt
						p = _ptarget->GetCreateIn64Extension( EXTENSION_ID_FUROR_ENERGY );
						*p = MIN( m_target->GetPower( POWER_TYPE_ENERGY ), mod->m_amount );
					}
			}
		}break;
	case 12295:
	case 12676:	//Tactical Mastery
	case 12677:
	case 12678:
		{
			if( !_ptarget )
				return;
			int64 *p = _ptarget->GetCreateIn64Extension( EXTENSION_ID_FUROR_RAGE );
			if(apply)
				*p += mod->m_amount*10; //don't really know if value is all value or needs to be multiplyed with 10
			else
				*p -= mod->m_amount*10;
		}break;
	case 2096://MindVision
		{
		}break;
	case 6196://FarSight
		{
			if(apply)
			{
			}
			else
			{
				// disabled this due to unstableness :S
#if 0
				Creature *summon = m_target->GetMapMgr()->GetCreature(m_target->GetUInt32Value(PLAYER_FARSIGHT));
				if(summon)
				{
					summon->RemoveFromWorld(false,true);
					delete summon;
				}
				m_target->SetUInt64Value(PLAYER_FARSIGHT,0);
#endif
			}
		}break;
	case 18182:
	case 18183:
		{//improved life tap give amt% bonus for convers
			if( _ptarget == NULL )
				return;
			if(apply)
			{
				int64 *p = _ptarget->GetCreateIn64Extension( EXTENSION_ID_LIFETAP_BONUS );
				*p=mod->m_amount;
			}
			else
			{
				_ptarget->SetExtension( EXTENSION_ID_LIFETAP_BONUS, NULL );
			}
		}break;
	case 570:   // far sight
	case 1345:
	case 6197:
	case 6198:  // eagle eye
	case 24125:
	case 21171:
		{
			/*if(!apply && m_target->IsPlayer() && m_target->IsInWorld())
			{
				// reset players vision
				Player * plr = SafePlayerCast( m_target );
				plr->GetMapMgr()->ChangeFarsightLocation(plr, NULL);

				Creature * farsight = plr->GetMapMgr()->GetCreature(plr->GetUInt32Value(PLAYER_FARSIGHT));
				plr->SetUInt64Value(PLAYER_FARSIGHT, 0);
				if(farsight)
				{
					farsight->RemoveFromWorld(false,true);
					delete farsight;
				}
			}*/
		}break;

	case 23701://Darkmoon Card: Twisting Nether give 10% chance to self resurrect ->cast 23700
		{
			//if(!apply)

		}break;

	//Second Wind - triggers only on stun and Immobilize
	case 29834:
		{
			Unit *caster = GetUnitCaster();
			if(caster && caster->IsPlayer())
				SafePlayerCast(caster)->SetTriggerStunOrImmobilize(29841,100, true);//fixed 100% chance
		}break;
	case 29838:
		{
			Unit *caster = GetUnitCaster();
			if(caster && caster->IsPlayer())
				SafePlayerCast(caster)->SetTriggerStunOrImmobilize(29842,100, true);//fixed 100% chance
		}break;
/*	//mage Magic Absorption
	case 29441:
	case 29444:
	case 29445:
	case 29446:
	case 29447:
		{
			if (m_target->IsPlayer())
			{
				SafePlayerCast( m_target )->m_RegenManaOnSpellResist += ((apply) ? 1:-1)*(float)mod->m_amount/100;
			}
		}break;*/
/*	//warlock - seed of corruption
	case 27243:
	case 32863:
	case 36123:
	case 38252:
	case 39367:
		{
			//register a cast on death of the player
			if(apply)
			{
				ProcTriggerSpell pts;
				pts.origId = GetSpellProto()->Id;
				pts.caster = m_casterGuid;
//					pts.spellId=GetSpellProto()->Id;
				pts.spellId=32865;
				if(!pts.spellId)
					return;
				pts.procChance = GetSpellProto()->procChance;
//					pts.procFlags = GetSpellProto()->procFlags;
				pts.procFlags = PROC_ON_DIE_VICTIM;
				pts.procCharges = GetSpellProto()->procCharges;
				pts.LastTrigger = 0;
				pts.deleted = false;
				m_target->m_procSpells.push_front(pts);
			}
			else
			{
				for(std::list<struct ProcTriggerSpell>::iterator itr = m_target->m_procSpells.begin();itr != m_target->m_procSpells.end();itr++)
				{
					if(itr->origId == GetSpellId() && itr->caster == m_casterGuid)
					{
						//m_target->m_procSpells.erase(itr);
						itr->deleted = true;
						break;
					}
				}
			}
		}break;*/

	case 740:
	case 8918:
	case 9862:
	case 9863:
	case 21791:
	case 25817:
	case 26983:
	case 34550:
	case 48446:
	case 48447:		// Tranquility 6+7 Fix by MtR
		{
			//uint32 duration = GetDuration();
			//printf("moo\n");
			if(apply)
				sEventMgr.AddEvent(this, &Aura::EventPeriodicHealNoBonus, (uint32)mod->m_amount, EVENT_AURA_PERIODIC_HEAL, 2000, 0, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
			else
				sEventMgr.RemoveEvents(this, EVENT_AURA_PERIODIC_HEAL);

		}break;

	case 16914:
	case 17401:
	case 17402:
	case 27012:		// hurricane
		{
			if(apply)
			{
				uint32 ticks = GetDuration() / 1000;
				sEventMgr.AddEvent(this, &Aura::EventPeriodicDamage, (uint32)mod->m_amount / ticks, mod->m_pct_mod ,ticks, mod->i, EVENT_AURA_PERIODIC_DAMAGE, 1000, 0, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
			}
			else
				sEventMgr.RemoveEvents(this, EVENT_AURA_PERIODIC_DAMAGE);
		}break;
	case 2584:			// Area spirit healer aura for BG's
		{
			if( !m_target->IsPlayer() || apply )		// already applied in opcode handler
			{ 
				return;
			}

			Player* pTarget = SafePlayerCast( m_target );
			uint64 crtguid = pTarget->m_areaSpiritHealer_guid;
			Creature* pCreature = pTarget->IsInWorld() ? pTarget->GetMapMgr()->GetCreature( crtguid ) : NULL;
			if(pCreature==NULL || pTarget->m_bg==NULL)
			{ 
				return;
			}

			pTarget->m_bg->RemovePlayerFromResurrect(pTarget,pCreature);
		}break;

	case 34477: // Misdirection
		{
			if (GetUnitCaster()->GetTypeId() != TYPEID_PLAYER)
			{ 
				return;
			}

			if (!apply)
			{
				sEventMgr.AddEvent(SafePlayerCast(GetUnitCaster()), &Player::SetMisdirectionTarget,(uint64)0, EVENT_UNK, 250, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
				//SafePlayerCast(GetUnitCaster())->SetMisdirectionTarget(0);
			}
		}break;

	case 17007: //Druid:Leader of the Pack
		{
			if( !m_target->IsPlayer() )
			{ 
				return;
			}

			Player * pTarget = SafePlayerCast(m_target);
			if( apply )
				pTarget->AddShapeShiftSpell( 24932 );
			else
				pTarget->RemoveShapeShiftSpell( 24932 );
		}break;
	case 48384: //Druid:Improved Moonkin Form
		{
			if( !m_target->IsPlayer() )
			{ 
				return;
			}

			Player * pTarget = SafePlayerCast(m_target);
			if( apply )
				pTarget->AddShapeShiftSpell( 50170 );
			else
				pTarget->RemoveShapeShiftSpell( 50170 );
		}break;
	case 48385: //Druid:Improved Moonkin Form
		{
			if( !m_target->IsPlayer() )
			{ 
				return;
			}

			Player * pTarget = SafePlayerCast(m_target);
			if( apply )
				pTarget->AddShapeShiftSpell( 50171 );
			else
				pTarget->RemoveShapeShiftSpell( 50171 );
		}break;
	case 48396: //Druid:Improved Moonkin Form
		{
			if( !m_target->IsPlayer() )
			{ 
				return;
			}

			Player * pTarget = SafePlayerCast(m_target);
			if( apply )
				pTarget->AddShapeShiftSpell( 50172 );
			else
				pTarget->RemoveShapeShiftSpell( 50172 );
		}break;
/*	case 31223:
	case 31222:
	case 31221:		// Rogue : Master of Subtlety
		{
			if( !m_target->IsPlayer() )
			{ 
				return;
			}

			Player * pTarget = SafePlayerCast(m_target);
			if( apply )
			{
				pTarget->m_outStealthDamageBonusPct += mod->m_amount;
				pTarget->m_outStealthDamageBonusPeriod = 6;			// 6 seconds
				pTarget->m_outStealthDamageBonusTimer = 0;			// reset it
			}
			else
			{
				pTarget->m_outStealthDamageBonusPct -= mod->m_amount;
				pTarget->m_outStealthDamageBonusPeriod = 6;			// 6 seconds
				pTarget->m_outStealthDamageBonusTimer = 0;			// reset it
			}
		}break; */
/*	case 17804: // Warlock: Soul Siphon
	case 17805:
		{
			Unit *caster = GetUnitCaster();
			if(caster) {
				if( apply )
					caster->m_soulSiphon.amt+= mod->m_amount;
				else
					caster->m_soulSiphon.amt-= mod->m_amount;
			}
		}break;*/
	}
	if ( TamingSpellid && ! GetTimeLeft() )
	{
		// Creates a 15 minute pet, if player has the quest that goes with the spell and if target corresponds to quest
		Player *p_caster =SafePlayerCast(GetUnitCaster());
		SpellEntry *triggerspell = dbcSpell.LookupEntry( TamingSpellid );
		Quest* tamequest = QuestStorage.LookupEntry( triggerspell->eff[1].EffectMiscValue );
		if ( tamequest && p_caster->GetQuestLogForEntry(tamequest->id )&& m_target->GetEntry() == tamequest->required_mob[0] )
		{
			if( RandChance( 75 ) )// 75% chance on success
			{
				Creature *tamed = ( ( m_target->IsCreature() ) ? ( SafeCreatureCast(m_target) ) : 0 );
				QuestLogEntry *qle = p_caster->GetQuestLogForEntry(tamequest->id );

				tamed->GetAIInterface()->HandleEvent( EVENT_LEAVECOMBAT, p_caster, 0 );
				Pet *pPet = objmgr.CreatePet( tamed->GetEntry(), p_caster->GeneratePetNumber() );
				pPet->SetInstanceID( p_caster->GetInstanceID() );
				pPet->CreateAsSummon( tamed->GetEntry(), tamed->GetCreatureInfo(), tamed, p_caster, triggerspell, 2, 900000 );
				pPet->CastSpell( tamed, triggerspell, false );
				tamed->SafeDelete();
				qle->SetMobCount( 0, 1 );
				qle->SendUpdateAddKill( 1 );
				qle->UpdatePlayerFields();
				qle->SendQuestComplete();
			}
			else
			{
				p_caster->SendCastResult( triggerspell->Id,SPELL_FAILED_TRY_AGAIN,0,0 );
			}
		}
		else
		{
			p_caster->SendCastResult( triggerspell->Id,SPELL_FAILED_BAD_TARGETS,0,0 );
		}
		TamingSpellid = 0;
	}
}

void Aura::SpellAuraModConfuse(bool apply)
{
	Unit* u_caster = GetUnitCaster();

	if( m_target->IsCreature() && SafeCreatureCast(m_target)->IsTotem() )
	{ 
		return;
	}

	Player *p_target;
	if( m_target->IsPlayer() )
		p_target = SafePlayerCast( m_target );
	else
		p_target = NULL;

	if(apply)
	{
		if( u_caster == NULL ) 
		{ 
			return;
		}

		// Check Mechanic Immunity
		mod->fixed_amount[0] = 0; //not immune
		if( m_target )
		{
			if( m_target->MechanicsDispels[MECHANIC_DISORIENTED]
			|| ( m_spellProto->MechanicsType == MECHANIC_POLYMORPHED && m_target->MechanicsDispels[MECHANIC_POLYMORPHED] )
			)
			{
				mod->fixed_amount[0] = 1; //immune
				return;
			}
		}
		SetNegative();

		if( p_target )
		{
			float *x = (float *)p_target->GetCreateIn64Extension( EXTENSION_ID_FEAR_X );
			float *y = (float *)p_target->GetCreateIn64Extension( EXTENSION_ID_FEAR_Y );
			float *z = (float *)p_target->GetCreateIn64Extension( EXTENSION_ID_FEAR_Z );
			int64 *t = p_target->GetCreateIn64Extension( EXTENSION_ID_FEAR_STAMP );
			*x = p_target->GetPositionX();
			*y = p_target->GetPositionY();
			*z = p_target->GetPositionZ();
			*t = getMSTime();
		}

		m_target->m_special_state |= UNIT_STATE_CONFUSE;
		m_target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_CONFUSED);

		m_target->setAItoUse(true);
		m_target->GetAIInterface()->HandleEvent(EVENT_WANDER, u_caster, 0);

		if( m_target->isCasting() )
			m_target->GetCurrentSpell()->safe_cancel();

		if(p_target)
		{
			// this is a hackfix to stop player from moving -> see AIInterface::_UpdateMovement() Wander AI for more info
			p_target->ClientControlUpdate( 0 );
			p_target->SpeedCheatDelay( GetDuration() );
		}
	}
	else if( mod->fixed_amount[0] == 0 ) //add these checks to mods where imunity can cancel only 1 mod and not whole spell
	{
		m_target->m_special_state &= ~UNIT_STATE_CONFUSE;
		m_target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_CONFUSED);
		if( p_target )
			p_target->SpeedCheatReset();

		m_target->GetAIInterface()->HandleEvent( EVENT_UNWANDER, NULL, 0 );

		if(p_target)
		{
			// re-enable movement
			p_target->ClientControlUpdate( 1 );
			m_target->setAItoUse(false);

			if( u_caster != NULL )
				sHookInterface.OnEnterCombat( p_target, u_caster );
		}
		else
			m_target->GetAIInterface()->AttackReaction(u_caster, 1, 0);
		//some clients somehow do not stop moving after getting back client control
		m_target->GetAIInterface()->StopMovement( 0 );
	}
}

void Aura::SpellAuraModCharm(bool apply)
{
	Unit* ucaster = GetUnitCaster();

	if( ucaster == NULL || ucaster->IsPlayer() == false )
		return;

	Player* caster = SafePlayerCast( ucaster );
	Creature* target = SafeCreatureCast( m_target );

	SetPositive(3); //we ignore the other 2 effect of this spell and force it to be a positive spell

	//!!!! AMG fix this. We are removed while we are handling aura !!! This is mem corruption
	if( m_target == NULL || m_target->GetTypeId() != TYPEID_UNIT )
	{ 
		return;
	}

	if( SafeCreatureCast( m_target )->IsTotem() )
	{ 
		return;
	}

	if( apply )
	{
		if( (int32)m_target->getLevel() > mod->m_amount || m_target->IsPet() )
		{ 
			return;
		}

		// this should be done properly
		if( target->GetEnslaveCount() >= 10 )
		{ 
			return;
		}

		if( caster->GetUInt64Value( UNIT_FIELD_CHARM ) != 0 )
		{ 
			return;
		}

		m_target->m_special_state |= UNIT_STATE_CHARM;
		m_target->SetCharmTempVal( m_target->GetUInt32Value( UNIT_FIELD_FACTIONTEMPLATE ) );
		m_target->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, caster->GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE));
		m_target->_setFaction();
//		m_target->UpdateOppFactionSet();
		m_target->GetAIInterface()->Init(m_target, AITYPE_PET, MOVEMENTTYPE_NONE, caster);
		m_target->SetUInt64Value(UNIT_FIELD_CHARMEDBY, caster->GetGUID());
		caster->SetUInt64Value(UNIT_FIELD_CHARM, target->GetGUID());
		//damn it, the other effects of enslaive demon will agro him on us anyway :S
		m_target->GetAIInterface()->WipeHateList();
		m_target->GetAIInterface()->WipeTargetList();
		m_target->GetAIInterface()->SetNextTarget( (Unit*)NULL);

		target->SetEnslaveCount(target->GetEnslaveCount() + 1);

		if( caster->GetSession() ) // crashfix
		{
			WorldPacket data(SMSG_PET_SPELLS, 500);
			data << target->GetGUID();
			data << uint16(1) << uint32(0) << uint32(0x1000);
			data << uint32(PET_SPELL_ATTACK);
			data << uint32(PET_SPELL_FOLLOW);
			data << uint32(PET_SPELL_STAY);
			for(int i = 0; i < 4; i++)
				data << uint32(0);
			data << uint32(PET_SPELL_AGRESSIVE);
			data << uint32(PET_SPELL_DEFENSIVE);
			data << uint32(PET_SPELL_PASSIVE);
			caster->GetSession()->SendPacket(&data);
			target->SetEnslaveSpell(m_spellProto->Id);
		}
	}
	else
	{
		m_target->m_special_state &= ~UNIT_STATE_CHARM;
		m_target->SetUInt32Value( UNIT_FIELD_FACTIONTEMPLATE, m_target->GetCharmTempVal() );
		m_target->_setFaction();
		m_target->GetAIInterface()->WipeHateList();
		m_target->GetAIInterface()->WipeTargetList();
//		m_target->UpdateOppFactionSet();
		m_target->GetAIInterface()->Init(m_target, AITYPE_AGRO, MOVEMENTTYPE_NONE);
		m_target->SetUInt64Value(UNIT_FIELD_CHARMEDBY, 0);

		if( caster != NULL && caster->GetSession() != NULL ) // crashfix
		{
			caster->SetUInt64Value(UNIT_FIELD_CHARM, 0);
			WorldPacket data(SMSG_PET_SPELLS, 8);
			data << uint64(0);
			data << uint32(0);
			caster->GetSession()->SendPacket(&data);
			target->SetEnslaveSpell(0);
		}
	}
}

void Aura::SpellAuraModFear(bool apply)
{
	Unit* u_caster = GetUnitCaster();

	if( m_target->IsCreature() && SafeCreatureCast(m_target)->IsTotem() )
	{ 
		return;
	}

	Player *p_target;
	if( m_target->IsPlayer() )
		p_target = SafePlayerCast( m_target );
	else
		p_target = NULL;
	if(apply)
	{
		if( u_caster == NULL ) 
		{ 
			return;
		}
		// Check Mechanic Immunity
		if( m_target )
		{
			uint32 UsedMechanic = MECHANIC_FLEEING; // can be horror, turned, fear, flee ...
			if( GetSpellProto()->eff[ mod->i ].EffectMechanic > 0 )
				UsedMechanic = GetSpellProto()->eff[ mod->i ].EffectMechanic;
			else if( GetSpellProto()->MechanicsType > 0 )
				UsedMechanic = GetSpellProto()->MechanicsType;
			if( UsedMechanic > MECHANIC_TOTAL )
				UsedMechanic = MECHANIC_FLEEING; 
			if( m_target->MechanicsDispels[ UsedMechanic ] )
			{
				m_flags |= 1 << mod->i;
				return;
			}
		}

		if( p_target )
		{
			float *x = (float *)p_target->GetCreateIn64Extension( EXTENSION_ID_FEAR_X );
			float *y = (float *)p_target->GetCreateIn64Extension( EXTENSION_ID_FEAR_Y );
			float *z = (float *)p_target->GetCreateIn64Extension( EXTENSION_ID_FEAR_Z );
			int64 *t = p_target->GetCreateIn64Extension( EXTENSION_ID_FEAR_STAMP );
			*x = p_target->GetPositionX();
			*y = p_target->GetPositionY();
			*z = p_target->GetPositionZ();
			*t = getMSTime();
		}
		SetNegative();

		m_target->m_special_state |= UNIT_STATE_FEAR;
		m_target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FLEEING);

		m_target->setAItoUse(true);
		m_target->GetAIInterface()->HandleEvent(EVENT_FEAR, u_caster, 0);
		m_target->m_fearmodifiers++;
		mod->fixed_amount[0] = m_target->GetUInt32Value( UNIT_FIELD_HEALTH );	//fear breaks if N% health dropped since feared
		mod->fixed_amount[1] = 0;
		if(p_target)
		{
			// this is a hackfix to stop player from moving -> see AIInterface::_UpdateMovement() Fear AI for more info
			p_target->ClientControlUpdate( 0 );
			p_target->SpeedCheatDelay( GetDuration() );
		}
	}
	else if( (m_flags & (1 << mod->i)) == 0 ) //add these checks to mods where imunity can cancel only 1 mod and not whole spell
	{
		m_target->m_fearmodifiers--;

		if(m_target->m_fearmodifiers <= 0)
		{
			m_target->m_special_state &= ~UNIT_STATE_FEAR;
			m_target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FLEEING);

			m_target->GetAIInterface()->HandleEvent( EVENT_UNFEAR, NULL, 0 );

			if(p_target)
			{
				// re-enable movement
				p_target->ClientControlUpdate( 1 );
				m_target->setAItoUse(false);

				if( u_caster != NULL )
					sHookInterface.OnEnterCombat( p_target, u_caster );
				p_target->SpeedCheatReset();
			}
			else
				m_target->GetAIInterface()->AttackReaction(u_caster, 1, 0);
		}

		//some clients somehow do not stop moving after getting back client control
		m_target->GetAIInterface()->StopMovement( 0 );
	}
}

void Aura::SpellAuraPeriodicHeal( bool apply )
{
	if( apply )
	{
		SetPositive();

		//hasting affects ticks too
		Unit *Unitc = GetUnitCaster();
		int32 period = GetSpellAmplitude( GetSpellProto(), Unitc, mod->i, m_castedItemId );

		mod->fixed_amount[0] = period;	//need this for Swiftmend to take into count hasting
		if( TryToStackAura( m_target, this, mod ) == false )
			sEventMgr.AddEvent( this, &Aura::EventPeriodicHeal, (int32)period, (int32)mod->list_ind, EVENT_AURA_PERIODIC_HEAL, period , 0, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
	}
	//if we get replaced by a stronger version then unstacking is good
	if( apply == false )
		TryToUnStackAura( m_target, this, mod );
}

void Aura::EventPeriodicHeal( int32 period, int32 mod_index )
{
	EventPeriodicHeal2( m_modList[ mod_index ].m_amount, mod_index, period, m_modList[ mod_index ].i ); 
	m_flags |= FIRST_TICK_ENDED;
}

void Aura::EventPeriodicHeal2( uint32 amount,uint32 mod_index,int32 period, int32 eff_index )
{
	if( m_target==NULL || !m_target->isAlive() )
	{ 
		return;
	}

	//cannot get healed
	if( m_target->HealTakenPctMod <= -100 )
		return;

	Unit* c = GetUnitCaster();
	uint32 pct_mod = m_modList[ mod_index ].m_pct_mod;

	if( c )
	{
		c->SpellHeal( m_target, GetSpellProto(), amount, pSpellId == 0, false, false, GetSpellProto()->quick_tickcount, pct_mod, eff_index );
/*		if (GetSpellProto()->NameHash == SPELL_HASH_HEALTH_FUNNEL && add > 0) 
		{
			dealdamage sdmg;

			sdmg.full_damage = amount;
			sdmg.resisted_damage = 0;
			sdmg.school_type = 0;
			sdmg.absorbed_damage = u_caster->ResilianceAbsorb( sdmg.full_damage, GetSpellProto(), u_caster );
			sdmg.full_damage -= sdmg.absorbed_damage;

			u_caster->DealDamage(u_caster, add, 0);
			u_caster->SendAttackerStateUpdate(u_caster, u_caster, &sdmg, add, 0, 0, 0, ATTACK);
		}*/
	}
	else
	{
		m_target->SpellHeal( m_target, GetSpellProto(), amount, pSpellId == 0, false, false, GetSpellProto()->quick_tickcount, pct_mod, eff_index );
	}

	if( m_target && ( GetSpellProto()->AuraInterruptFlags & AURA_INTERRUPT_ON_STAND_UP ) )
	{
		m_target->Emote( EMOTE_ONESHOT_EAT );
	}
}

void Aura::SpellAuraModAttackSpeed( bool apply )
{
	if( mod->m_amount < 0 )
		SetNegative();
	else
		SetPositive();

	if ( m_target->IsPlayer() )
	{
		if(apply)
		{

			SafePlayerCast( m_target )->ModAttackSpeed( mod->m_amount, MOD_MELEE );
			SafePlayerCast( m_target )->ModAttackSpeed( mod->m_amount, MOD_RANGED );
		}
		else
		{
			SafePlayerCast( m_target )->ModAttackSpeed( -mod->m_amount, MOD_MELEE );
			SafePlayerCast( m_target )->ModAttackSpeed( -mod->m_amount, MOD_RANGED );
		}
	}
	else
	{
		if(apply)
		{
			mod->fixed_amount[0] = m_target->GetModPUInt32Value(UNIT_FIELD_BASEATTACKTIME,mod->m_amount);
			mod->fixed_amount[1] = m_target->GetModPUInt32Value(UNIT_FIELD_BASEATTACKTIME_1,mod->m_amount);
			mod->fixed_amount[2] = m_target->GetModPUInt32Value(UNIT_FIELD_RANGEDATTACKTIME,mod->m_amount);
			m_target->ModUnsigned32Value(UNIT_FIELD_BASEATTACKTIME, -mod->fixed_amount[0]);
			m_target->ModUnsigned32Value(UNIT_FIELD_BASEATTACKTIME_1, -mod->fixed_amount[1]);
			m_target->ModUnsigned32Value(UNIT_FIELD_RANGEDATTACKTIME, -mod->fixed_amount[2]);
		}
		else
		{
			m_target->ModUnsigned32Value(UNIT_FIELD_BASEATTACKTIME, mod->fixed_amount[0]);
			m_target->ModUnsigned32Value(UNIT_FIELD_BASEATTACKTIME_1, mod->fixed_amount[1]);
			m_target->ModUnsigned32Value(UNIT_FIELD_RANGEDATTACKTIME, mod->fixed_amount[2]);
		}
		if( m_target->IsPet() )
		{
			if( apply )
				SafePetCast( m_target )->AuraMeleeHasteSum += mod->m_amount;
			else
				SafePetCast( m_target )->AuraMeleeHasteSum -= mod->m_amount;
			SafePetCast( m_target )->UpdateHaste();
		}
	}

}

void Aura::SpellAuraModAttackSpeedNoRegen(bool apply)
{
	SpellAuraModAttackSpeed( apply );
	if( m_target->IsPlayer() )
	{
		if( apply )
			SafePlayerCast( m_target )->m_attack_speed_mod_noRegen += mod->m_amount;
		else
			SafePlayerCast( m_target )->m_attack_speed_mod_noRegen -= mod->m_amount;
		SafePlayerCast( m_target )->UpdatePowerRegen();
	}
}

void Aura::SpellAuraModThreatGeneratedSchoolPCT(bool apply)
{
	if( !m_target )
	{ 
		return;
	}

	mod->m_amount < 0 ? SetPositive() : SetNegative();
	for( uint32 x = 0; x < SCHOOL_COUNT; x++ )
	{
		if( mod->m_miscValue & ( ( (uint32)1 ) << x ) )
		{
			if ( apply )
				m_target->ModGeneratedThreatModifyerPCT(x, mod->m_amount);
			else
				m_target->ModGeneratedThreatModifyerPCT(x, -(mod->m_amount));
		}
	}
}

void Aura::SpellAuraModTaunt(bool apply)
{
	Unit* m_caster = GetUnitCaster();
	if(!m_caster || !m_caster->isAlive())
	{ 
		return;
	}

	SetNegative();
	//some say taunt should not be working in PVP ?
	if( m_target->IsPlayer() == true )
	{
		return;
	}

	if(apply)
	{
		m_target->GetAIInterface()->AttackReaction(m_caster, 1, 0);
		m_target->GetAIInterface()->taunt(m_caster, true);
	}
	else
	{
		if(m_target->GetAIInterface()->getTauntedBy() == m_caster)
		{
			m_target->GetAIInterface()->taunt(m_caster, false);
		}
	}
}

void Aura::SpellAuraModStun(bool apply)
{
	if(!m_target) 
	{ 
		return;
	}

	if(apply)
	{
		// Check Mechanic Immunity
		// Stun is a tricky one... it's used for all different kinds of mechanics as a base Aura
		if( m_target && !IsPositive() 
			&& m_spellProto->NameHash != SPELL_HASH_ICE_BLOCK // ice block stuns you, don't want our own spells to ignore stun effects
			&& m_spellProto->NameHash != SPELL_HASH_CYCLONE 
			)  
		{
			if( ( ( m_spellProto->MechanicsType == MECHANIC_CHARMED || m_spellProto->eff[mod->i].EffectMechanic == MECHANIC_CHARMED ) &&  m_target->MechanicsDispels[MECHANIC_CHARMED] )
				|| ( ( m_spellProto->MechanicsType == MECHANIC_INCAPACIPATED || m_spellProto->eff[mod->i].EffectMechanic == MECHANIC_INCAPACIPATED ) && m_target->MechanicsDispels[MECHANIC_INCAPACIPATED] )
				|| ( ( m_spellProto->MechanicsType == MECHANIC_SAPPED || m_spellProto->eff[mod->i].EffectMechanic == MECHANIC_SAPPED ) && m_target->MechanicsDispels[MECHANIC_SAPPED] )
			|| ( m_target->MechanicsDispels[MECHANIC_STUNNED] )
				)
			{
				m_flags |= 1 << mod->i;
				return;
			}
		}
		SetNegative();

		m_target->m_rooted++;

		if(m_target->m_rooted == 1)
			m_target->Root();

		if (m_target->IsStealth())
			m_target->RemoveStealth();

		m_target->m_stunned++;
		m_target->m_special_state |= UNIT_STATE_STUN;
		m_target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);

		if(m_target->IsCreature())
			m_target->GetAIInterface()->SetNextTarget( (Unit*)NULL );

		// remove the current spell (for channelers)
		if(m_target->m_currentSpell 
			&& m_target->GetGUID() != m_casterGuid 
			&& m_target->m_currentSpell->getState() == SPELL_STATE_CASTING 
			)
		{
			//remove periodic casted beneficial spell -> report was for starfall but i guees it should be the same for other periodic spells ?
			for(uint32 x=FIRST_AURA_SLOT;x<MAX_POSITIVE_AURAS;x++)
				if( m_target->m_auras[x] && m_target->m_auras[x]->GetSpellProto()->quick_tickcount > 1 )
					m_target->m_auras[x]->Remove();
		}

		//interrupt casting while stunned
		m_target->InterruptSpell();

		//warrior talent - second wind triggers on stun and immobilize. This is not used as proc to be triggered always !
		Unit *caster = GetUnitCaster();
		if( caster && m_target )
			SafeUnitCast(caster)->EventStunOrImmobilize( m_target );
		if( m_target && caster )
			SafeUnitCast(m_target)->EventStunOrImmobilize( caster, true );
		m_target->InterruptSpell();
	}
	else if( (m_flags & (1 << mod->i)) == 0 ) //add these checks to mods where imunity can cancel only 1 mod and not whole spell
	{
		m_target->m_rooted--;

		if(m_target->m_rooted == 0)
			m_target->Unroot();

		m_target->m_stunned--;

		if(m_target->m_stunned == 0)
		{
			m_target->m_special_state &= ~UNIT_STATE_STUN;
			m_target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
		}

		// attack them back.. we seem to lose this sometimes for some reason
		if( m_target->IsUnit() )
		{
			Unit * target = GetUnitCaster();
			if( m_target->GetAIInterface() && m_target->GetAIInterface()->GetNextTarget() != 0)
				target = m_target->GetAIInterface()->GetNextTarget();

			if(!target) 
			{ 
				return;
			}
			m_target->GetAIInterface()->AttackReaction(target, 1, 0);
		}
	}
}

void Aura::SpellAuraModDamageDone(bool apply)
{
	int32 val;

	if( m_target->IsPlayer() )
	{
		uint32 index;

		if( mod->m_amount > 0 )
		{
			if( apply )
			{
				SetPositive();
				val = mod->m_amount;
			}
			else
			{
				val = -mod->m_amount;
			}
			index = PLAYER_FIELD_MOD_DAMAGE_DONE_POS;

		}
		else
		{
			if( apply )
			{
				SetNegative();
				val = -mod->m_amount;
			}
			else
			{
				val = mod->m_amount;
			}
			index = PLAYER_FIELD_MOD_DAMAGE_DONE_NEG;
		}

		for( uint32 x = 0; x < SCHOOL_COUNT; x++ )
		{
			if( mod->m_miscValue & ( ( (uint32)1 ) << x ) )
			{
				m_target->ModUnsigned32Value( index + x, val );
			}
		}
	}
	else if( m_target->IsCreature() )
	{
		if( mod->m_amount > 0 )
		{
			if( apply )
			{
				SetPositive();
				val = mod->m_amount;
			}
			else
			{
				val = -mod->m_amount;
			}

		}
		else
		{
			if( apply )
			{
				SetNegative();
				val = mod->m_amount;
			}
			else
			{
				val = -mod->m_amount;
			}
		}

		for( uint32 x = 0; x < SCHOOL_COUNT; x++ )
		{
			if( mod->m_miscValue & ( ( (uint32)1 ) << x ) )
			{
				SafeCreatureCast( m_target )->ModDamageDone[x] += val;
			}
		}
	}

	if( mod->m_miscValue & 1 )
		m_target->CalcDamage();
}

void Aura::SpellAuraModDamageTaken(bool apply)
{
	int32 val = (apply) ? mod->m_amount : -mod->m_amount;
	for(uint32 x=0;x<SCHOOL_COUNT;x++)
		if (mod->m_miscValue & (((uint32)1)<<x) )
			m_target->DamageTakenMod[x]+=val;
}

void Aura::SpellAuraDamageShield(bool apply)
{
	if(apply)
	{
		SetPositive();
		DamageProc ds;// = new DamageShield();
		ds.m_damage = mod->m_amount;
		ds.m_spellId = GetSpellProto()->Id;
		ds.m_school = GetSpellProto()->School;
		ds.m_flags = PROC_ON_MELEE_ATTACK_VICTIM; //maybe later we might want to add other flags too here
		ds.owner = (void*)this;
		m_target->m_damageShields.push_back(ds);
	}
	else
	{
		for(std::list<struct DamageProc>::iterator i = m_target->m_damageShields.begin();i != m_target->m_damageShields.end();i++)
		{
			if(i->owner==this)
			{
				 m_target->m_damageShields.erase(i);
				 return;
			}
		}
	}
}

void Aura::SpellAuraModStealth(bool apply)
{
	if(apply)
	{
		Player *p_target;
		if( m_target->IsPlayer() )
			p_target = SafePlayerCast( m_target );
		else
			p_target = NULL;
		if(p_target && p_target->m_bgHasFlag)
		{
			if(p_target->m_bg && p_target->m_bg->GetType() == BATTLEGROUND_WARSONG_GULCH)
				static_cast<WarsongGulch*>(p_target->m_bg)->HookOnFlagDrop(p_target);
			else if(p_target->m_bg && p_target->m_bg->GetType() == BATTLEGROUND_EYE_OF_THE_STORM)
				static_cast<EyeOfTheStorm*>(p_target->m_bg)->HookOnFlagDrop(p_target);
			else if(p_target->m_bg->GetType() == BATTLEGROUND_TWIN_PEAKS)
				static_cast<TwinPeaks*>(p_target->m_bg)->HookOnFlagDrop(p_target);
	    }
		Unit *u_caster = GetUnitCaster();
		Player *p_caster;
		if( u_caster && u_caster->IsPlayer() )
			p_caster = SafePlayerCast( u_caster );
		else
			p_caster = NULL;
		if( p_caster && p_caster!= p_target && p_caster->m_bg && p_caster->m_bgHasFlag )
		{
			if(p_caster->m_bg->GetType() == BATTLEGROUND_WARSONG_GULCH)
				static_cast<WarsongGulch*>(p_caster->m_bg)->HookOnFlagDrop(p_caster);
			else if(p_caster->m_bg->GetType() == BATTLEGROUND_EYE_OF_THE_STORM)
				static_cast<EyeOfTheStorm*>(p_caster->m_bg)->HookOnFlagDrop(p_caster);
			else if(p_caster->m_bg->GetType() == BATTLEGROUND_TWIN_PEAKS)
				static_cast<TwinPeaks*>(p_caster->m_bg)->HookOnFlagDrop(p_caster);
		 }

		SetPositive();

		//it sucks cause if we stealth more then once then this value will get oweverwritten. Any interrupt might fail removign stealth !
		m_target->SetStealth( GetSpellId() );

//		if( m_target->IsPlayer() && SafePlayerCast( m_target )->GetShapeShift() == 0 )
//			SafePlayerCast( m_target )->SetShapeShift( FORM_STEALTH );
//		else
//			m_target->SetByte(UNIT_FIELD_BYTES_2,3,FORM_STEALTH);//sneak anim - this is setshapeshift

		m_target->SetFlag(UNIT_FIELD_BYTES_1, STANDSTATE_FLAG_CREEP2);
		if( m_target->IsPlayer() )
		{
//			m_target->SetFlag(PLAYER_FIELD_BYTES2, PLAYER_FIELD_BYTES2_STEALTH_GLOW); 
//			if( m_spellProto->NameHash == SPELL_HASH_CAMOUFLAGE)
//				m_target->SetFlag(PLAYER_FIELD_BYTES2,0x20000000);//i think this makes the player untargatable or maybe just transparent ? Or sneak anim ?
		}

		m_target->RemoveAurasByInterruptFlag(AURA_INTERRUPT_ON_STEALTH | AURA_INTERRUPT_ON_INVINCIBLE);
		m_target->m_stealthLevel += ( mod->m_amount / 10 );

		//fade out making us loose all threat. 
		//Break casted spell
		//loose targetting
		if( m_target )
			m_target->VanishFromSight();

	}
	else 
	{
		m_target->m_stealthLevel -= ( mod->m_amount / 10 );

		//last stealth spell ?
		uint32 last_stealth_spell = 0;
		for( uint32 x = POS_AURAS_START; x < MAX_POSITIVE_AURAS1(m_target); x++ )
		{
			if( m_target->m_auras[x] != NULL 
				&& HasAuraType( m_target->m_auras[x]->GetSpellProto(), SPELL_AURA_MOD_STEALTH ) )
				last_stealth_spell++;
		}
		//not good, if we have multiple steath spells that only last one should remove stealth variables
		if( last_stealth_spell < 1 
//			|| m_target->GetStealth() == GetSpellId() 
			)
		{
			m_target->SetStealth(0);
//			if( m_target->IsPlayer() && SafePlayerCast( m_target )->GetShapeShift() == FORM_STEALTH )
//				SafePlayerCast( m_target )->SetShapeShift( FORM_NORMAL );
//			else
//				m_target->SetByte(UNIT_FIELD_BYTES_2,3,FORM_NORMAL);//sneak anim - this is setshapeshift

			m_target->RemoveFlag(UNIT_FIELD_BYTES_1, STANDSTATE_FLAG_CREEP2);

			if( m_target->IsPlayer() )
			{
//				m_target->RemoveFlag(PLAYER_FIELD_BYTES2, PLAYER_FIELD_BYTES2_STEALTH_GLOW);
//				if( m_spellProto->NameHash == SPELL_HASH_CAMOUFLAGE)
//					m_target->RemoveFlag(PLAYER_FIELD_BYTES2,0x20000000);//i think this makes the player untargatable or maybe just transparent ? Or sneak anim ?
				packetSMSG_COOLDOWN_EVENT cd;
				cd.guid = m_target->GetGUID();
				cd.spellid = m_spellProto->Id;
				SafePlayerCast(m_target)->GetSession()->OutPacket( SMSG_COOLDOWN_EVENT, sizeof(packetSMSG_COOLDOWN_EVENT), &cd);
/*				if( SafePlayerCast(m_target)->m_outStealthDamageBonusPeriod && SafePlayerCast(m_target)->m_outStealthDamageBonusPct )
					SafePlayerCast(m_target)->m_outStealthDamageBonusTimer = (uint32)UNIXTIME + SafePlayerCast(m_target)->m_outStealthDamageBonusPeriod; */
			}
		}
	}

	m_target->UpdateVisibility();
}

void Aura::SpellAuraModDetect(bool apply)
{
	if(apply)
	{
		//SetPositive();
		m_target->m_stealthDetectBonus += mod->m_amount;
	}
	else
		m_target->m_stealthDetectBonus -= mod->m_amount;
}

void Aura::SpellAuraModInvisibility(bool apply)
{
	SetPositive();
	if(m_spellProto->eff[mod->i].Effect == 128)
	{ 
		return;
	}

	ASSERT( mod->m_miscValue < INVIS_FLAG_TOTAL );

	uint64 Type = MAX( 1, mod->m_miscValue ); //do not use INVIS_FLAG_NORMAL
	uint64 Flag = (((uint64)1)<<(uint64)Type);
	if(apply)
	{
		m_target->m_invisFlag |= Flag;
		m_target->m_invisDetect |= Flag; //not sure, i think we should see others in the same plane..
		if( m_target->IsPlayer() )
		{
			if( GetSpellProto()->NameHash == SPELL_HASH_INVISIBILITY ) 
				SafePlayerCast(m_target)->SetFlag( PLAYER_FIELD_BYTES2, PLAYER_FIELD_BYTES2_INVISIBILITY_GLOW ); //Mage Invis self visual
		}

		m_target->RemoveAurasByInterruptFlag(AURA_INTERRUPT_ON_INVINCIBLE);
	}
	else
	{
		Flag = ~Flag;
		m_target->m_invisFlag &= Flag;
		m_target->m_invisDetect &= Flag; //not sure, i think we should see others in the same plane..
		if( m_target->IsPlayer() )
		{
			if( GetSpellProto()->NameHash == SPELL_HASH_INVISIBILITY ) 
				SafePlayerCast(m_target)->RemoveFlag( PLAYER_FIELD_BYTES2, PLAYER_FIELD_BYTES2_INVISIBILITY_GLOW );
		}
	}
	m_target->UpdateVisibility();
}

void Aura::SpellAuraModInvisibilityDetection(bool apply)
{
	//Always Positive

	assert(mod->m_miscValue < INVIS_FLAG_TOTAL);
	if(apply)
	{
		m_target->m_invisDetect |= (((uint64)1)<<(uint64)mod->m_miscValue);
		SetPositive ();
	}
	else
		m_target->m_invisDetect &= (((uint64)1)<<(uint64)mod->m_miscValue);

	if(m_target->IsPlayer())
		SafePlayerCast( m_target )->UpdateVisibility();
}

void Aura::SpellAuraModTotalHealthRegenPct(bool apply)
{
	if(apply)
	{
		SetPositive();

		//hasting affects ticks too
		Unit *Unitc = GetUnitCaster();
		int32 period = GetSpellAmplitude( GetSpellProto(), Unitc, mod->i, m_castedItemId );

		float amt = (float)mod->m_amount;
		if( GetSpellProto()->c_is_flags2 & SPELL_FLAG2_IS_VALUE_SCALED )
			amt = amt / 100.0f;	//some values are smaller then 1. like 0.5, we scale them here
		if( GetSpellProto()->c_is_flags2 & SPELL_FLAG2_IS_VALUE_OVER_TIME )
		{
//			amt = amt / (float)GetSpellProto()->quick_tickcount;	//some values are smaller then 1. like 0.5, we scale them here
			int32 ticks = GetDuration() / period; 
			amt = amt / (float)ticks;	//some values are smaller then 1. like 0.5, we scale them here
		}
		uint32 Flags = EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT;
		if( GetSpellProto()->c_is_flags2 & SPELL_FLAG2_IS_TICKING_IMEDIATLY )
			Flags |= EVENT_FLAG_TICK_ON_NEXT_UPDATE;
		sEventMgr.AddEvent(this, &Aura::EventPeriodicHealPct,amt,period,(int32)mod->i, EVENT_AURA_PERIODIC_HEALPERC, period , 0, Flags );
	}
}

void Aura::EventPeriodicHealPct(float RegenPct,int32 period, int32 eff_index)
{
	if(!m_target->isAlive())
	{ 
		return;
	}

	//cannot get healed
	if( m_target->HealTakenPctMod <= -100 )
		return;

	uint32 amount = float2int32(m_target->GetUInt32Value(UNIT_FIELD_MAXHEALTH) * (RegenPct / 100.0f));
	Unit* c = GetUnitCaster();
	if( c != NULL)
		c->SpellHeal( m_target, GetSpellProto(), amount, pSpellId == 0, false, false, GetSpellProto()->quick_tickcount, 100, eff_index, 0 );

/*	int32 add;
	{
		int32 bonus = 0;
		add = 0;

		if( c != NULL)
		{ 
			//ver 2
			bonus += c->GetSpellHealBonus( m_target, GetSpellProto(), amount, eff_index );
			if ( (GetSpellProto()->c_is_flags & SPELL_FLAG_IS_REQUIRING_SM_MOD) && c->SM_Mods )
				SM_PIValue(c->SM_Mods->SM_PDOT,&bonus,m_spellProto->GetSpellGroupType());

			add = bonus + amount;

			if( add < 0 )
				add = 0;

			if(	( !pSpellId || (GetSpellProto()->c_is_flags & SPELL_FLAG_IS_PROC_TRIGGER_PROC) ) && (GetSpellProto()->c_is_flags3 & SPELL_FLAG_IS_DISABLE_OTHER_SPELL_CPROC) == 0 )
			{
				add += c->HandleProc( PROC_ON_HEAL_EVENT | PROC_ON_DOT | PROC_ON_NOT_VICTIM, m_target, GetSpellProto(), &add ,0 );
				if( m_target )
					add += m_target->HandleProc( PROC_ON_HEAL_EVENT | PROC_ON_DOT | PROC_ON_VICTIM, c, GetSpellProto(), &add ,0 );
			}

			//!!handleproc removed us !!
			if( m_target == NULL )
				return;
		}
	}

	//Zack : old way, not considering any mods
//	add = float2int32(m_target->GetUInt32Value(UNIT_FIELD_MAXHEALTH) * (RegenPct / 100.0f));

	int32 absorb = 0;
	if( m_target->HealAbsorb <= add )
	{
		absorb = m_target->HealAbsorb;
		add -= absorb;
		m_target->HealAbsorb = 0;
	}
	else // if( unitTarget->HealAbsorb > amount )
	{
		absorb = add;
		m_target->HealAbsorb -= add;
		add = 0;
	}

	uint32 newHealth = m_target->GetUInt32Value(UNIT_FIELD_HEALTH) + add;
	if(newHealth <= m_target->GetUInt32Value(UNIT_FIELD_MAXHEALTH))
		m_target->SetUInt32Value(UNIT_FIELD_HEALTH, newHealth);
	else
		m_target->SetUInt32Value(UNIT_FIELD_HEALTH, m_target->GetUInt32Value(UNIT_FIELD_MAXHEALTH));

//	SendPeriodicAuraLog(m_casterGuid, m_target, m_spellProto->Id, m_spellProto->School, add, 0, 0, FLAG_PERIODIC_HEAL);
	Unit *m_caster = GetUnitCaster();
//	if( m_caster && m_target->IsPlayer() )
//		SendHealSpellOnPlayer( m_caster, SafePlayerCast( m_target ), add, 0, GetSpellProto() );
	if( m_caster )
//		m_caster->SendCombatLogMultiple( m_target, add, 0, absorb, 0, GetSpellProto()->spell_id_client, GetSpellProto()->SchoolMask, COMBAT_LOG_PERIODIC_HEAL, 0 );
		m_caster->SendCombatLogMultiple( m_target, add, 0, absorb, 0, GetSpellProto()->spell_id_client, GetSpellProto()->SchoolMask, COMBAT_LOG_HEAL, 0 );

	*/

	if( m_target )
	{
		if( ( GetSpellProto()->AuraInterruptFlags & AURA_INTERRUPT_ON_STAND_UP ) )
			m_target->Emote(EMOTE_ONESHOT_EAT);
		m_target->RemoveAurasByHeal();
	}
	m_flags |= FIRST_TICK_ENDED;
}

void Aura::SpellAuraModTotalManaRegenPct(bool apply)
{
	if(apply)
	{
		SetPositive();

		//hasting affects ticks too
		Unit *Unitc = GetUnitCaster();
		int32 period = GetSpellAmplitude( GetSpellProto(), Unitc, mod->i, m_castedItemId );
		float amt = (float)mod->m_amount;
		if( GetSpellProto()->c_is_flags2 & SPELL_FLAG2_IS_VALUE_SCALED )
			amt = amt / 100.0f;	//some values are smaller then 1. like 0.5, we scale them here
		if( GetSpellProto()->c_is_flags2 & SPELL_FLAG2_IS_VALUE_OVER_TIME )
		{
			int32 ticks = GetDuration() / period; 
			amt = amt / (float)ticks;	//some values are smaller then 1. like 0.5, we scale them here
		}

		uint32 Flags = EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT;
		if( GetSpellProto()->c_is_flags2 & SPELL_FLAG2_IS_TICKING_IMEDIATLY )
			Flags |= EVENT_FLAG_TICK_ON_NEXT_UPDATE;
		sEventMgr.AddEvent(this, &Aura::EventPeriodicManaPct,amt, EVENT_AURA_PERIOCIC_MANA, period , 0, Flags );
	}
}

void Aura::EventPeriodicManaPct(float RegenPct)
{
	if(!m_target->isAlive())
	{ 
		return;
	}

	uint32 add = float2int32(m_target->GetMaxPower( POWER_TYPE_MANA ) * (RegenPct / 100.0f));

	Unit *pcaster = GetUnitCaster();
	if( pcaster == NULL )
		pcaster = m_target;

	pcaster->Energize( m_target, GetSpellId(), add, POWER_TYPE_MANA, 1 );

	if(GetSpellProto()->AuraInterruptFlags & AURA_INTERRUPT_ON_STAND_UP)
	{
		m_target->Emote(EMOTE_ONESHOT_EAT);
	}
	m_flags |= FIRST_TICK_ENDED;
}

void Aura::SpellAuraModResistance(bool apply)
{
	uint32 Flag = mod->m_miscValue;
	int32 amt;
	if(apply)
	{
		amt = mod->m_amount;
		if(amt <0 )//dont' change it
			SetNegative();
		else
			SetPositive();
	}
	else
		amt = -mod->m_amount;

	if( m_target->IsPlayer() )
	{
		for( uint32 x = 0; x < SCHOOL_COUNT; x++ )
		{
			if(Flag & (((uint32)1)<< x) )
			{
				if(mod->m_amount>0)
					SafePlayerCast( m_target )->FlatResistanceModifierPos[x]+=amt;
				else
					SafePlayerCast( m_target )->FlatResistanceModifierNeg[x]-=amt;
				SafePlayerCast( m_target )->CalcResistance(x);
			}
		}
	}
	else if( m_target->IsCreature() )
	{
		for(uint32 x=0;x<SCHOOL_COUNT;x++)
		{
			if(Flag & (((uint32)1)<<x))
			{
				SafeCreatureCast(m_target)->FlatResistanceMod[x]+=amt;
				SafeCreatureCast(m_target)->CalcResistance(x);
			}
		}
	}
}

void Aura::SpellAuraPeriodicTriggerSpell(bool apply)
{
	if(m_spellProto->eff[mod->i].EffectTriggerSpell == 0)
	{ 
		return;
	}

	/*
	// This should be fixed in other way...
	if(IsPassive() &&
		m_spellProto->dummy != 2010 &&
		m_spellProto->dummy != 2020 &&
		m_spellProto->dummy != 2255 &&
		m_spellProto->Id != 8145 &&
		m_spellProto->Id != 8167 &&
		m_spellProto->Id != 8172)
	{
		Unit * target = (m_target != 0) ? m_target : GetUnitCaster();
		if(target == 0 || !target->IsPlayer())
			return; //what about creatures ?

		SpellEntry *proto = dbcSpell.LookupEntry( m_spellProto->EffectTriggerSpell[mod->i] );

		if( apply )
			SafePlayerCast( target )->AddOnStrikeSpell( proto, m_spellProto->eff[mod->i].EffectAmplitude );
		else
			SafePlayerCast( target )->RemoveOnStrikeSpell( proto );

		return;
	}
			*/

	if(apply)
	{
		//FIXME: positive or negative?
		uint32 sp = GetSpellProto()->eff[mod->i].EffectTriggerSpell;
		SpellEntry *spe = dbcSpell.LookupEntry(sp);
		if(!sp || !spe)
		{
			//	sp=22845;
			return;//null spell
		}

		Unit *Unitc = GetUnitCaster();
		if(!Unitc)
		{ 
			return;
		}

/*		if ( GetSpellProto()->Id == 23493 || GetSpellProto()->Id == 24379 )
			// Cebernic: Restoration on battleground fixes(from p2wow's merged)
			// it wasn't prefectly,just for tempfix
		{
			SetPositive();
			sEventMgr.AddEvent(this, &Aura::EventPeriodicHealPct,10.0f,(int32)GetSpellProto()->eff[mod->i].EffectAmplitude,(int32)mod->i,
			EVENT_AURA_PERIODIC_HEALPERC,	GetSpellProto()->eff[mod->i].EffectAmplitude,0,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);

			sEventMgr.AddEvent(this, &Aura::EventPeriodicManaPct,10.0f,
			EVENT_AURA_PERIOCIC_MANA,	GetSpellProto()->eff[mod->i].EffectAmplitude,0,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
			return;
		} */

		//hasting affects ticks too
		int32 period = GetSpellAmplitude( GetSpellProto(), Unitc, mod->i, m_castedItemId );
		uint32 Flags = EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT;
		if( GetSpellProto()->c_is_flags2 & SPELL_FLAG2_IS_TICKING_IMEDIATLY )
			Flags |= EVENT_FLAG_TICK_ON_NEXT_UPDATE;

		if(Unitc->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT))
		{
			sEventMgr.AddEvent(this, &Aura::EventPeriodicTriggerSpell, spe, (int32)0,Unitc->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT),
			EVENT_AURA_PERIODIC_TRIGGERSPELL, period , 0,Flags);

//            periodic_target = m_caster->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT);
		}
		else if(m_target)
		{
			sEventMgr.AddEvent(this, &Aura::EventPeriodicTriggerSpell, spe, (int32)0,m_target->GetGUID(),
				EVENT_AURA_PERIODIC_TRIGGERSPELL, period , 0,Flags);
//			periodic_target = m_target->GetGUID();
		}
	}
//	else
//		sEventMgr.RemoveEvents( this, EVENT_AURA_PERIODIC_TRIGGERSPELL ); //i hope we will not remove other mods of this aura too

}

void Aura::SpellAuraPeriodicTriggerSpellWithValue(bool apply)
{
	if(m_spellProto->eff[mod->i].EffectTriggerSpell == 0)
	{ 
		return;
	}
	if(apply)
	{
		uint32 sp = GetSpellProto()->eff[mod->i].EffectTriggerSpell;
		SpellEntry *spe = dbcSpell.LookupEntry(sp);
		if(!sp || !spe)
			return; // invalid spell

		Unit *m_caster = GetUnitCaster();
		if(!m_caster)
			return; // invalid caster

		//hasting affects ticks too
		Unit *Unitc = GetUnitCaster();
		int32 period = GetSpellAmplitude( GetSpellProto(), Unitc, mod->i, m_castedItemId );

		if(m_caster->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT))
		{
			sEventMgr.AddEvent(this, &Aura::EventPeriodicTriggerSpell, spe, (int32)mod->m_amount,m_caster->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT),
				EVENT_AURA_PERIODIC_TRIGGERSPELL,period, 0, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
//			periodic_target = m_caster->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT);
		}
		else if(m_target)
		{
			sEventMgr.AddEvent(this, &Aura::EventPeriodicTriggerSpell, spe,(int32)mod->m_amount,m_target->GetGUID(),
				EVENT_AURA_PERIODIC_TRIGGERSPELL,period, 0,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
//			periodic_target = m_target->GetGUID();
		}
	}
}

void Aura::EventPeriodicTriggerSpell(SpellEntry* spellInfo,int32 value_overwrite, uint64 periodic_target)
{
	// Trigger Spell
	// check for spell id
	Unit *m_caster;
	if( GetSpellProto()->c_is_flags & SPELL_FLAG_IS_TRIGGERED_BY_TARGET )
		m_caster = m_target;
	else
		m_caster = GetUnitCaster();
	if(!m_caster || !m_caster->IsInWorld())
	{ 
		return;
	}

	//sniper training cast condition is to stand still for 6 seconds
	if( ( spellInfo->NameHash == SPELL_HASH_SNIPER_TRAINING || spellInfo->NameHash == SPELL_HASH_CAMOUFLAGE ) && m_caster->IsPlayer() )
	{
		Player *p = SafePlayerCast(m_caster);
		if( p->last_moved + 6000 > getMSTime() )
			return;
		//avoid packet spam ? This spell is casted every second ...
//		if( p->HasAuraWithNameHash( SPELL_HASH_SNIPER_TRAINING ) )
//			return;
	}
	if( spellInfo->eff[0].EffectImplicitTargetA == EFF_TARGET_LOCATION_TO_SUMMON )			// Hellfire, if there are any others insert here
	{
		Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		spell->Init(m_caster, spellInfo, true, this);
		if( value_overwrite )
			spell->forced_basepoints[ 0 ] = value_overwrite;
		SpellCastTargets targets;
		targets.m_targetMask = TARGET_FLAG_SOURCE_LOCATION;
		targets.m_srcX = m_caster->GetPositionX();
		targets.m_srcY = m_caster->GetPositionY();
		targets.m_srcZ = m_caster->GetPositionZ();
		spell->prepare(&targets);
		return;
	}

	Object * oTarget = m_target->GetMapMgr()->_GetObject(periodic_target);

	if(oTarget==NULL)
	{ 
		return;
	}

	if(oTarget->GetTypeId()==TYPEID_DYNAMICOBJECT)
	{
		Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		spell->Init(m_caster, spellInfo, true, this);
		if( value_overwrite )
			spell->forced_basepoints[ 0 ] = value_overwrite;
		SpellCastTargets targets;
		targets.m_targetMask = TARGET_FLAG_DEST_LOCATION;
		targets.m_destX = oTarget->GetPositionX();
		targets.m_destY = oTarget->GetPositionY();
		targets.m_destZ = oTarget->GetPositionZ();
		spell->prepare(&targets);
		return;
	}

	if(!oTarget->IsUnit())
	{ 
		return;
	}

	Unit *pTarget = SafeUnitCast(oTarget);

	if(pTarget->IsDead())
	{
		SendInterrupted(SPELL_FAILED_TARGETS_DEAD, m_caster);
		SendChannelUpdate(0, m_caster);
//		this->Remove();
		return;
	}

	//maybe there are beneficial periodic spells too ? Ex Mind Sear
/*	if(pTarget != m_caster && !isAttackable(m_caster, pTarget) && (spellInfo->c_is_flags & SPELL_FLAG_IS_DAMAGING) )
	{
		SendInterrupted(SPELL_FAILED_BAD_TARGETS, m_caster);
		SendChannelUpdate(0, m_caster);
		this->Remove();
		return;
	} */

//	if(spellInfo->NameHash == SPELL_HASH_ARCANE_MISSILES ) // this is arcane missles to avoid casting on self
//		if(m_casterGuid == pTarget->GetGUID())
//		{ 
//			return;
//		}

	// set up our max Range
	float maxRange = GetMaxRange( dbcSpellRange.LookupEntry( spellInfo->rangeIndex ) );

	if( (GetSpellProto()->c_is_flags & SPELL_FLAG_IS_REQUIRING_SM_MOD) && m_caster->SM_Mods )
	{
		SM_FFValue( m_caster->SM_Mods->SM_FRange, &maxRange, spellInfo->GetSpellGroupType() );
		SM_PFValue( m_caster->SM_Mods->SM_PRange, &maxRange, spellInfo->GetSpellGroupType() );
	}

	maxRange *= 1.08f; // client is not counting Z distance ? Tested with penance

	if( 
		m_caster != pTarget //self stun like something explodes in hand
		// i wonder what this will break :(
		&& GetSpellProto()->ChannelInterruptFlags != 0//totem pulses need to continue even when shaman leaves vecinity and returns "earthen power"
		&& ( m_caster->IsStunned() || m_caster->IsFeared() || m_caster->GetDistanceSq( pTarget ) > ( maxRange*maxRange ) ) )
	{
		if( maxRange == 0 )
			sLog.outDebug("Spell has range = 0. Aborting cast \n");
		// no longer valid
		SendInterrupted(SPELL_FAILED_INTERRUPTED, m_caster);
		SendChannelUpdate(0, m_caster);
		this->Remove();
		return;
	}

	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init(m_caster, spellInfo, true, this);
	spell->ProcedOnSpell = GetSpellProto();
	if( value_overwrite )
		spell->forced_basepoints[ 0 ] = value_overwrite;
	SpellCastTargets targets( pTarget->GetGUID() );
	spell->prepare(&targets);
}

void Aura::SpellAuraPeriodicEnergize(bool apply)
{
	if(apply)
	{
		SetPositive();

		//hasting affects ticks too
		Unit *Unitc = GetUnitCaster();
		int32 period = GetSpellAmplitude( GetSpellProto(), Unitc, mod->i, m_castedItemId );

		mod->m_amount = mod->m_amount;	//seems like the extra 1 is not added anymore in 403 for effect basepoints

		sEventMgr.AddEvent(this, &Aura::EventPeriodicEnergize,(uint32)mod->m_amount,(uint32)mod->m_miscValue, EVENT_AURA_PERIODIC_ENERGIZE, period ,0,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
	}
}

void Aura::EventPeriodicEnergize( uint32 amount, uint32 type )
{
	if( type > POWER_TYPE_COUNT )
		return;

	if( GetUnitCaster() == NULL )
	{ 
		return;
	}
	
	GetUnitCaster()->Energize( m_target, m_spellProto->Id, amount, type, 1 );

	if( ( GetSpellProto()->AuraInterruptFlags & AURA_INTERRUPT_ON_STAND_UP ) && type == POWER_TYPE_MANA )
	{
		m_target->Emote( EMOTE_ONESHOT_EAT );
	}
}

void Aura::SpellAuraModPacify(bool apply)
{
	// Can't Attack
	if( apply )
	{
		if( m_spellProto->Id == 24937 || m_spellProto->NameHash == SPELL_HASH_BLESSING_OF_PROTECTION )
			SetPositive();
		else
			SetNegative();

		m_target->m_pacified++;
		m_target->m_special_state |= UNIT_STATE_PACIFY;
		m_target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
	}
	else
	{
		m_target->m_pacified--;

		if(m_target->m_pacified == 0)
		{
			m_target->m_special_state &= ~UNIT_STATE_PACIFY;
			m_target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
		}
	}
}

void Aura::SpellAuraModRoot(bool apply)
{
	if(apply)
	{
		// Check Mechanic Immunity
		if( m_target )
		{
			if( m_target->GetGUID() != GetCasterGUID() && m_target->MechanicsDispels[MECHANIC_ROOTED] )
			{
				m_flags |= 1 << mod->i;
				return;
			}
		}
		SetNegative();
		
		mod->fixed_amount[0] = m_target->GetHealth();	//frost nova and maybe others break after target recevies x% HP loss
		mod->fixed_amount[1] = 0;

		m_target->m_rooted++;

		//if we are in air and jumping, make us fall in 1 place
		//!! this seems to bug out orientation !!
//		m_target->GetAIInterface()->StopMovement( 0 );
		m_target->GetAIInterface()->MoveTo( m_target->GetPositionX(), m_target->GetPositionY(), m_target->GetPositionZ(), m_target->GetOrientation() );

		if(m_target->m_rooted == 1)
			m_target->Root();

		//warrior talent - second wind triggers on stun and immobilize. This is not used as proc to be triggered always !
		Unit *caster = GetUnitCaster();
		if( caster && m_target )
			SafeUnitCast(caster)->EventStunOrImmobilize( m_target );
		if( m_target && caster )
			SafeUnitCast(m_target)->EventStunOrImmobilize( caster, true );

		//i think we should gather these interrupt events and only call once ?
//		m_target->RemoveAurasByInterruptFlagButSkip( AURA_INTERRUPT_ON_HOSTILE_SPELL_INFLICTED | AURA_INTERRUPT_ON_START_ATTACK, GetSpellId() );
		m_target->RemoveAurasByInterruptFlagButSkip( AURA_INTERRUPT_ON_HOSTILE_SPELL_INFLICTED, GetSpellId() );

//		if (m_target->isCasting())
//			m_target->CancelSpell(NULL); //cancel spells.
	}
	else if( (m_flags & (1 << mod->i)) == 0 ) //add these checks to mods where imunity can cancel only 1 mod and not whole spell
	{
		m_target->m_rooted--;

		if(m_target->m_rooted == 0)
			m_target->Unroot();

		if(m_target->IsCreature())
			m_target->GetAIInterface()->AttackReaction(GetUnitCaster(), 1, 0);
	}
}

void Aura::SpellAuraModSilence(bool apply)
{
	if(apply)
	{
		if( m_target->GetGUID() != GetCasterGUID() && m_target->MechanicsDispels[MECHANIC_SILENCED] )
		{
			m_flags |= 1 << mod->i;
			return;
		}
		m_target->m_silenced++;
		m_target->m_special_state |= UNIT_STATE_SILENCE;
		m_target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED);

		// remove the current spell (for channelers)
		if(m_target->m_currentSpell 
			&& m_target->GetGUID() != m_casterGuid 
			&& ( m_target->m_currentSpell->GetProto()->SchoolMask & ~SCHOOL_MASK_NORMAL ) != 0
//			&&
//			m_target->m_currentSpell->getState() == SPELL_STATE_CASTING 
//			m_target->m_currentSpell->GetProto()->PreventionType == SPELL_PREVENTION_TYPE_SILENCE
			// Edit 2 : on 4.3.4 client a player made a video on retail that silence does indeed interrupt PVP cast even tough it says on spell description it does not
//			m_target->IsPlayer() == false // right now there are only a few silence spells and most say they only interrupt non player spells. "silencing shot" uses added effect "interrupt" to make sure it will break player cast also
			)
		{
			m_target->InterruptSpell();
		}
	}
	else if( (m_flags & (1 << mod->i)) == 0 )
	{
		m_target->m_silenced--;

		if(m_target->m_silenced == 0)
		{
			m_target->m_special_state &= ~UNIT_STATE_SILENCE;
			m_target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED);
		}
	}
}

void Aura::SpellAuraReflectSpells(bool apply)
{
	m_target->ModSpellReflectList( GetSpellId(), mod->m_amount, -1, GetSpellProto()->procCharges, apply );
/*	if(apply)
	{
		SpellEntry *sp = dbcSpell.LookupEntry(GetSpellId());
		if (sp == NULL) 
		{ 
			return;
		}

		CommitPointerListNode<struct ReflectSpellSchool> *itr;
		m_target->m_reflectSpellSchool.BeginLoop();
		for(itr = m_target->m_reflectSpellSchool.begin(); itr != m_target->m_reflectSpellSchool.end();itr = itr->Next() )
			if(GetSpellProto()->Id == itr->data->spellId)
				m_target->m_reflectSpellSchool.SafeRemove(itr,1);
		m_target->m_reflectSpellSchool.EndLoopAndCommit();

		ReflectSpellSchool *rss = new ReflectSpellSchool;

		rss->chance = mod->m_amount;
		rss->spellId = GetSpellId();
		rss->school = -1;
		rss->require_aura_hash = 0;
		rss->charges = sp->procCharges;
		m_target->m_reflectSpellSchool.push_front(rss);
	}
	else
	{
		CommitPointerListNode<struct ReflectSpellSchool> *itr;
		m_target->m_reflectSpellSchool.BeginLoop();
		for(itr = m_target->m_reflectSpellSchool.begin(); itr != m_target->m_reflectSpellSchool.end();itr = itr->Next() )
			if(GetSpellProto()->Id == itr->data->spellId)
				m_target->m_reflectSpellSchool.SafeRemove(itr,1);
		m_target->m_reflectSpellSchool.EndLoopAndCommit();
	} */
}

void Aura::SpellAuraModStat( bool apply )
{
	int32 stat = ( int32 )mod->m_miscValue;
	int32 statMask = ( int32 )m_spellProto->eff[mod->i].EffectMiscValueB;
	int32 val;

	if( apply )
	{
		val = mod->m_amount;
		if( val < 0 )
			SetNegative();
		else
			SetPositive();
	}
	else
	{
		val = -mod->m_amount;
	}

	if( statMask == 0 )
		statMask = 1 << stat;
	if( stat == -1 )//all stats
		statMask = -1;	//seen some rare cases when mask was not good
	for( uint32 i=0;i<STAT_COUNT;i++)
		if( statMask & ( 1 << i ) )
		{
			if( m_target->IsPlayer() )
			{
				if( mod->m_amount > 0 )
					SafePlayerCast( m_target )->FlatStatModPos[ i ] += val;
				else
					SafePlayerCast( m_target )->FlatStatModNeg[ i ] -= val;
				SafePlayerCast( m_target )->CalcStat( i );
			}
			else if( m_target->IsCreature() )
			{
				SafeCreatureCast( m_target )->FlatStatMod[ i ] += val;
				SafeCreatureCast( m_target )->CalcStat( i );
			}
		}
	if( m_target->IsPlayer() )
	{
		SafePlayerCast( m_target )->UpdateStats();
		SafePlayerCast( m_target )->UpdateChances();
	}
}

void Aura::SpellAuraModSkill(bool apply)
{
	if (m_target->IsPlayer())
	{
		if(apply)
		{
			SetPositive();
			SafePlayerCast( m_target )->_ModifySkillBonus(mod->m_miscValue, mod->m_amount);
		}
		else
			SafePlayerCast( m_target )->_ModifySkillBonus(mod->m_miscValue, -mod->m_amount);

		SafePlayerCast( m_target )->UpdateStats();
	}
}

void Aura::SpellAuraModIncreaseSpeed(bool apply)
{
	if(apply)
	{
		if( mod->m_amount < 0 && m_casterGuid != m_target->GetGUID() && ( m_target->MechanicsDispels[MECHANIC_ENSNARED] || m_target->MechanicsDispels[MECHANIC_SLOWED] ) )
		{
			m_flags |= 1 << mod->i;
			return;
		}

		if( (GetSpellProto()->c_is_flags & SPELL_FLAG_IS_REQUIRING_SM_MOD) && m_target->SM_Mods )
		{
			int32 speedmod=0;
			SM_FIValue(m_target->SM_Mods->SM_FSpeedMod,&speedmod,GetSpellProto()->GetSpellGroupType());
			mod->m_amount += speedmod;
		}

		m_target->speedIncreaseMap.push_front( mod );
	}
	else
		m_target->speedIncreaseMap.remove( mod );

	if( m_target->GetSpeedIncrease() )
		m_target->UpdateSpeed();
}

void Aura::SpellAuraModIncreaseMountedSpeed(bool apply)
{
/*	if( GetSpellId() == 68768 || GetSpellId() == 68769 && p_target != NULL )
	{
		int32 newspeed = 0;

		if( p_target->_GetSkillLineCurrent( SKILL_RIDING, true ) >= 150 )
			newspeed = 100;
		else if( p_target->_GetSkillLineCurrent( SKILL_RIDING, true ) >= 75 )
			newspeed = 60;

		mod->m_amount = newspeed; // EffectBasePoints + 1 (59+1 and 99+1)
	} */
	if(apply)
	{
		SetPositive();
		m_target->m_mountedspeedModifier += mod->m_amount;
	}
	else
		m_target->m_mountedspeedModifier -= mod->m_amount;
	m_target->UpdateSpeed();
}

void Aura::SpellAuraModCreatureRangedAttackPower(bool apply)
{
	if(apply)
	{
		for(uint32 x = 0; x < CREATURE_TYPES; x++)
			if (mod->m_miscValue & (((uint32)1)<<x) )
				m_target->CreatureRangedAttackPowerMod[x+1] += mod->m_amount;
		if(mod->m_amount < 0)
			SetNegative();
		else
			SetPositive();
	}
	else
	{
		for(uint32 x = 0; x < CREATURE_TYPES; x++)
		{
			if (mod->m_miscValue & (((uint32)1)<<x) )
			{
				m_target->CreatureRangedAttackPowerMod[x+1] -= mod->m_amount;
			}
		}
	}
	m_target->CalcDamage();
}

void Aura::SpellAuraModDecreaseSpeed(bool apply)
{
	//there can not be 2 slow downs only most powerfull is applied
	if(apply)
	{
		// Check Mechanic Immunity
		if( m_target )
		{
			if( m_target->MechanicsDispels[MECHANIC_ENSNARED] || m_target->MechanicsDispels[MECHANIC_SLOWED] )
			{
				m_flags |= 1 << mod->i;
				return;
			}
		}
		switch(m_spellProto->NameHash)
		{
			case SPELL_HASH_STEALTH:			// Stealth
				SetPositive();
				break;

			case SPELL_HASH_DAZED:			// Dazed
				SetNegative();
				break;

			default:
				/* burlex: this would be better as a if(caster is hostile to target) then effect = negative) */
				if(m_casterGuid != m_target->GetGUID())
					SetNegative();
				break;
		}

		if( (GetSpellProto()->c_is_flags & SPELL_FLAG_IS_REQUIRING_SM_MOD) && m_target->SM_Mods )
		{
			int32 speedmod=0;
			SM_FIValue(m_target->SM_Mods->SM_FSpeedMod,&speedmod,GetSpellProto()->GetSpellGroupType());
			mod->m_amount += speedmod;
		}

		m_target->speedReductionMap.push_front( mod );
		//m_target->m_slowdown=this;
		//m_target->m_speedModifier += mod->m_amount;
	}
	else 
//		if( (m_flags & (1 << mod->i)) == 0 ) //add these checks to mods where imunity can cancel only 1 mod and not whole spell
	{
		m_target->speedReductionMap.remove( mod );
		//m_target->m_speedModifier -= mod->m_amount;
		//m_target->m_slowdown=NULL;
	}
	if( m_target->GetSpeedDecrease() )
		m_target->UpdateSpeed();
}

void Aura::UpdateAuraModDecreaseSpeed()
{
	if( m_target )
	{
		if( m_target->MechanicsDispels[MECHANIC_ENSNARED] )
		{
			m_flags |= 1 << mod->i;
			return;
		}
	}
}
/*
void Aura::SpellAuraModIncreaseHealth(bool apply)
{
	int32 amt;

	if(apply)
	{
		//threet special cases. We should move these to scripted spells maybe
		switch(m_spellProto->NameHash)
		{
			case SPELL_HASH_GIFT_OF_LIFE:// Gift of Life
			  mod->m_amount = 1500;
			  break;
			case SPELL_HASH_LAST_STAND:// Last Stand
			  mod->m_amount = (uint32)(m_target->GetUInt32Value(UNIT_FIELD_MAXHEALTH) * 30 / 100 );
			  break;
			case SPELL_HASH_VAMPIRIC_BLOOD:
			  mod->m_amount = m_target->GetUInt32Value(UNIT_FIELD_MAXHEALTH) * mod->m_amount / 100 ;
			  break;
		}
		SetPositive();

		//adjust value so when it is removed it is not removing more then he should
//		int32 max_amt = m_target->GetUInt32Value(UNIT_FIELD_MAXHEALTH) - m_target->GetUInt32Value(UNIT_FIELD_HEALTH);
//		if( mod->m_amount > max_amt )
//			mod->m_amount = max_amt;

		amt = mod->m_amount;

	}
	else
		amt =- mod->m_amount;

	if(m_target->IsPlayer())
	{
		//maybe we should not adjust hitpoints too but only maximum health
		SafePlayerCast( m_target )->SetHealthFromSpell(SafePlayerCast( m_target )->GetHealthFromSpell() + amt);
		if( m_target->isAlive() )
			SafePlayerCast( m_target )->UpdateStats();
	}

	if(apply)
	{
		m_target->ModUnsigned32Value(UNIT_FIELD_HEALTH,amt);
	}
	else
	{
		if((int32)m_target->GetUInt32Value(UNIT_FIELD_HEALTH)>-amt)//watch it on remove value is negative
			m_target->ModUnsigned32Value(UNIT_FIELD_HEALTH,amt);
		else if( m_target->isAlive() )
			m_target->SetUInt32Value(UNIT_FIELD_HEALTH,1); //do not kill player but do strip him good
	}
} /**/

void Aura::SpellAuraModIncreaseEnergy(bool apply)
{
	if( mod->m_miscValue > POWER_TYPE_COUNT )
	{ 
		return;
	}

	SetPositive();

	if( m_target->GetPowerType() == mod->m_miscValue )
		m_target->ModPower(mod->m_miscValue,apply?mod->m_amount:-mod->m_amount); //not always
	//always or else druids can exploit this due to multiple power types
	m_target->ModMaxPower(mod->m_miscValue,apply?mod->m_amount:-mod->m_amount);

	if(mod->m_miscValue == POWER_TYPE_MANA && m_target->IsPlayer() )
	{
		int32 amt = apply ? mod->m_amount : -mod->m_amount;
		SafePlayerCast( m_target )->SetManaFromSpell(SafePlayerCast( m_target )->GetManaFromSpell() + amt);
	}
}

void Aura::SpellAuraModShapeshift(bool apply)
{
	Player *p_target;
	if( m_target->IsPlayer() )
		p_target = SafePlayerCast( m_target );
	else
		p_target = NULL;
	if( p_target == NULL )
	{ 
		return;
	}

	if( p_target->m_MountSpellId && p_target->m_MountSpellId != m_spellProto->Id )
		if( mod->m_miscValue != FORM_BATTLESTANCE && mod->m_miscValue != FORM_DEFENSIVESTANCE && mod->m_miscValue != FORM_BERSERKERSTANCE && mod->m_miscValue != FORM_SHADOW ) 
			m_target->RemoveAura( p_target->m_MountSpellId ); // these spells are not compatible

	uint32 additional_cast_spellId = 0;
	uint32 modelId = 0;
	uint8 PrevPowerType = m_target->GetPowerType();
	int32 RetainedPower = m_target->GetPower( m_target->GetPowerType() );

	bool freeMovements = false;

	switch( mod->m_miscValue )
	{
	case FORM_CAT:
		{//druid
			freeMovements = true;
			additional_cast_spellId = 3025;
			if(apply)
			{
				m_target->SetPowerType(POWER_TYPE_ENERGY);
				RetainedPower = 0;
//				m_target->SetMaxPower(POWER_TYPE_ENERGY,100);//100 Energy
//				m_target->SetPower(POWER_TYPE_ENERGY,0);//0 Energy
//				m_target->UpdatePowerAmm( true, POWER_TYPE_ENERGY );
				if(m_target->getRace() == RACE_NIGHTELF)
				{
//					modelId = 892;
					//get hair color
					const int models[4] = { 29405,29406,29407,29408 };
					uint32 hair_color = m_target->GetByte( PLAYER_BYTES, 3 );
					hair_color = hair_color % 4;	//no idea how many there are, i think 8 or 7
					modelId = models[ hair_color ];
				}
				else if(m_target->getRace() == RACE_TROLL)
				{
					//get hair color
					const int models[5] = { 33669,33668,33667,33666,33665 };
					uint32 hair_color = m_target->GetByte( PLAYER_BYTES, 3 );
					hair_color = hair_color % 5;	//no idea how many there are, i think 8 or 7
					modelId = models[ hair_color ];
				}
				else if(m_target->getRace() == RACE_WORGEN)
				{
					//get hair color
					const int models[5] = { 33664,33663,33662,33661,33660 };
					uint32 hair_color = m_target->GetByte( PLAYER_BYTES, 3 );
					hair_color = hair_color % 5;	//no idea how many there are, i think 8 or 7
					modelId = models[ hair_color ];
				}
				else //TAUREN
				{
//					modelId = 8571;
					const int models[4] = { 29409,29410,29411,29412 };
					uint32 hair_color = m_target->GetByte( PLAYER_BYTES, 3 );
					hair_color = hair_color % 4;	//no idea how many there are, i think 8 or 7
					modelId = models[ hair_color ];
				}
				sEventMgr.AddEvent( this, &Aura::EventParalelManaRegen, EVENT_PERIODIC_DUMMY_AURA_TRIGGER, 2000, 0, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
			}
			else
			{//turn back to mana
				//m_target->SetUInt32Value(UNIT_FIELD_BASEATTACKTIME,oldap);
				m_target->SetPowerType( POWER_TYPE_MANA );
				if(m_target->m_stealth)
				{
					uint32 sp = m_target->m_stealth;
					m_target->m_stealth = 0;
					m_target->RemoveAura(sp);//prowl
				}
//				m_target->RemoveAura(1850);//Dash rank1
//				m_target->RemoveAura(9821);//Dash rank2
//				m_target->RemoveAura(33357);//Dash rank3
			}
			SafePlayerCast( m_target )->UpdateAttackSpeed();

		} break;
	case FORM_TREE:
		{
			if(apply)
			{
				if( p_target && p_target->HasGlyphWithID( GLYPH_DRUID_TREANT ) )
					modelId  = 864;
				else
				{
					const int models[4] = { 37163,37164,37165,37166 };
					uint32 hair_color = m_target->GetByte( PLAYER_BYTES, 3 );
					hair_color = hair_color % 4;	//no idea how many there are, i think 8 or 7
					modelId = models[ hair_color ];
				}
			}
#ifdef SPECIFIC_14333_CLIENT_FIXES
			//for the lols, mod tree form into christmass tree winter time
//			modelId = 38842;
#endif
			freeMovements=true;
			additional_cast_spellId = 34123;//
		} break;
	case FORM_TRAVEL:
		{//druid
			freeMovements = true;
			additional_cast_spellId = 5419;
			modelId = 918;
		} break;
	case FORM_AQUA:
		{//druid aqua
			freeMovements = true;
			additional_cast_spellId = 5421;
			modelId = 2428;
		} break;
	case FORM_BEAR:
		{//druid only
			freeMovements = true;
			additional_cast_spellId = 1178;
			if(apply)
			{
				m_target->SetPowerType(POWER_TYPE_RAGE);
				RetainedPower = 0;
//				m_target->SetMaxPower(POWER_TYPE_RAGE, 1000);
//				m_target->SetPower(POWER_TYPE_RAGE, 0);//0 rage

				if(m_target->getRace() == RACE_NIGHTELF)
				{
//					modelId = 2281;
					const int models[5] = { 29414,29415,29416,29417,29413};	//1 is extra but which one ?
					uint32 hair_color = m_target->GetByte( PLAYER_BYTES, 3 );
					hair_color = hair_color % 4;	//no idea how many there are, i think 8 or 7
					modelId = models[ hair_color ];
				}
				else if(m_target->getRace() == RACE_TROLL)
				{
					//get hair color
					const int models[5] = { 33659,33658,33657,33656,33656 };
					uint32 hair_color = m_target->GetByte( PLAYER_BYTES, 3 );
					hair_color = hair_color % 5;	//no idea how many there are, i think 8 or 7
					modelId = models[ hair_color ];
				}
				else if(m_target->getRace() == RACE_WORGEN)
				{
					//get hair color
					const int models[5] = { 33654,33653,33652,33651,33650 };
					uint32 hair_color = m_target->GetByte( PLAYER_BYTES, 3 );
					hair_color = hair_color % 5;	//no idea how many there are, i think 8 or 7
					modelId = models[ hair_color ];
				}
				else //TAUREN
				{
//					modelId = 2289;
					const int models[4] = { 29418,29419,29420,29421 };
					uint32 hair_color = m_target->GetByte( PLAYER_BYTES, 3 );
					hair_color = hair_color % 4;	//no idea how many there are, i think 8 or 7
					modelId = models[ hair_color ];
				}
				sEventMgr.AddEvent( this, &Aura::EventParalelManaRegen, EVENT_PERIODIC_DUMMY_AURA_TRIGGER, 2000, 0, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );

				//some say there is a second effect
				SpellEntry* spellInfo = dbcSpell.LookupEntry( 21178 );

				Spell *sp = SpellPool.PooledNew( __FILE__, __LINE__ );
				sp->Init( m_target, spellInfo, true, NULL );
				SpellCastTargets tgt;
				tgt.m_unitTarget = m_target->GetGUID();
				sp->prepare( &tgt );
			}
			else
			{//reset back to mana
				m_target->SetPowerType( POWER_TYPE_MANA );
				m_target->RemoveAura( 21178 ); // remove Bear Form (Passive2)
			}
		} break;
	case FORM_DIREBEAR:
		{//druid only
			freeMovements = true;
			additional_cast_spellId = 9635;
			if(apply)
			{
				m_target->SetPowerType(POWER_TYPE_RAGE);
//				m_target->SetMaxPower(POWER_TYPE_RAGE, 1000);
				m_target->SetPower(POWER_TYPE_RAGE, 0);//0 rage
				RetainedPower = 0;
				if(m_target->getRace() == 4)//NE
				{
//					modelId = 2281;
					const int models[5] = { 29414,29415,29416,29417,29413};	//1 is extra but which one ?
					uint32 hair_color = m_target->GetByte( PLAYER_BYTES, 3 );
					hair_color = hair_color % 4;	//no idea how many there are, i think 8 or 7
					modelId = models[ hair_color ];
				}
				else if(m_target->getRace() == RACE_TROLL)
				{
					//get hair color
					const int models[5] = { 33659,33658,33657,33656,33656 };
					uint32 hair_color = m_target->GetByte( PLAYER_BYTES, 3 );
					hair_color = hair_color % 5;	//no idea how many there are, i think 8 or 7
					modelId = models[ hair_color ];
				}
				else if(m_target->getRace() == RACE_WORGEN)
				{
					//get hair color
					const int models[5] = { 33654,33653,33652,33651,33650 };
					uint32 hair_color = m_target->GetByte( PLAYER_BYTES, 3 );
					hair_color = hair_color % 5;	//no idea how many there are, i think 8 or 7
					modelId = models[ hair_color ];
				}
				else //TAUREN
				{
//					modelId = 2289;
					const int models[4] = { 29418,29419,29420,29421 };
					uint32 hair_color = m_target->GetByte( PLAYER_BYTES, 3 );
					hair_color = hair_color % 4;	//no idea how many there are, i think 8 or 7
					modelId = models[ hair_color ];
				}
				sEventMgr.AddEvent( this, &Aura::EventParalelManaRegen, EVENT_PERIODIC_DUMMY_AURA_TRIGGER, 2000, 0, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
			}
			else //reset back to mana
				m_target->SetPowerType( POWER_TYPE_MANA );
		} break;
	case FORM_GHOSTWOLF:
		{
			modelId = 4613;
			if( apply )
			{
				if( m_target->IsPlayer() && SafePlayerCast( m_target )->HasGlyphWithID( GLYPH_SHAMAN_SPECTRAL_WOLF ) )
					modelId = 1236;
				m_target->m_minSpeed = m_target->m_base_runSpeed;
			}
			else
				m_target->m_minSpeed = 0;
		} break;
	case FORM_BATTLESTANCE:
		{
			additional_cast_spellId = 21156;
			RetainedPower = 0;
		} break;
	case FORM_DEFENSIVESTANCE:
		{
			additional_cast_spellId = 7376; //we already chained cast this
			RetainedPower = 0;
		} break;
	case FORM_BERSERKERSTANCE:
		{
			additional_cast_spellId = 7381;
			RetainedPower = 0;
		} break;
	case FORM_SHADOW:
		{
		}break;
	case FORM_SHADOW_DANCE:
		{
		}break;
	case FORM_FLIGHT:
		{// druid
			freeMovements = true;
			additional_cast_spellId = 33948;
			if(apply)
			{
			    if(m_target->getRace() == RACE_NIGHTELF)
				    modelId = 20857;
			    else if(m_target->getRace() == RACE_WORGEN)
				    modelId = 37727;
			    else if(m_target->getRace() == RACE_TROLL)
				    modelId = 37728;
			    else
				    modelId = 20872;		//race tauren		
			}
		}break;
	case FORM_STEALTH:
		{// rogue
			if( m_target->m_stealth_dissalowed != 0 )
			{ 
				return;
			}
			//m_target->UpdateVisibility();
		} break;
	case FORM_MOONKIN:
		{//druid
			freeMovements = true;
			additional_cast_spellId = 24905;
			if(apply)
			{
				if(m_target->getRace() == RACE_NIGHTELF)
					modelId = 15374;
				else if(m_target->getRace() == RACE_WORGEN )
					modelId = 37173;	//using NE model until i find the right one
			    else if(m_target->getRace() == RACE_TROLL)
				    modelId = 37174;
				else
					modelId = 15375;
			}
		}break;
	case FORM_SWIFT: //not tested yet, right now going on trust
		{// druid
			freeMovements = true;
			additional_cast_spellId = 40121; //Swift Form Passive
			if(apply)
			{
				if(m_target->getRace() == RACE_NIGHTELF)
					modelId = 21243;
				else if(m_target->getRace() == RACE_WORGEN)
					modelId = 37729;
				else if(m_target->getRace() == RACE_TROLL)
					modelId = 37730;
				else //TAUREN
					modelId = 21244;
			}
		}break;
	case FORM_SPIRITOFREDEMPTION:
		{
			additional_cast_spellId = 27795;
			modelId = 12824;
		}break;
	case FORM_DEMON:
		{
			additional_cast_spellId = 59673; //could use this only when we learn the talent as it teaches 3 spells
			modelId = 25277;
			if (apply)
			{
				//m_target->m_modlanguage = LANG_DEMONIC;
				m_target->CastSpell(m_target, 54817, true);
				m_target->CastSpell(m_target, 54879, true);
				m_target->CastSpell(m_target, 61610, true);
			}
			else
			{
				//m_target->m_modlanguage = -1;
				m_target->RemoveAura(54817);
				m_target->RemoveAura(54879);
				m_target->RemoveAura(61610);
			}
		}break;
	case FORM_RESURRECT_AS_GHOUL:
		{
			additional_cast_spellId = 40251;
			modelId = 571;
		}break;
	}

	float HealthPCTBeforeShift = (float)m_target->GetHealth() / (float)m_target->GetMaxHealth();
	if( apply )
	{
		//loose rage
		if( m_target->IsPlayer() )
		{
			if( m_target->GetPowerType() == POWER_TYPE_RAGE )
			{
				int64 *p = (int64 *)m_target->GetExtension( EXTENSION_ID_FUROR_RAGE );
				if( p )
					RetainedPower = (int32)*p;
			}
			else if( m_target->GetPowerType() == POWER_TYPE_ENERGY )
			{
				int64 *p = (int64 *)m_target->GetExtension( EXTENSION_ID_FUROR_ENERGY );
				if( p )
					RetainedPower = (int32)*p;
			}
		}
		int32 PowerNow = m_target->GetPower( m_target->GetPowerType() );
		m_target->SetPower( m_target->GetPowerType(), MIN( RetainedPower, PowerNow ) );
		m_target->UpdatePowerAmm( true, m_target->GetPowerType() );

		if( additional_cast_spellId != GetSpellId() && m_target->IsPlayer() )
		{
			if( SafePlayerCast( m_target )->m_ShapeShifted )
				SafePlayerCast( m_target )->RemoveAura( SafePlayerCast( m_target )->m_ShapeShifted );

			SafePlayerCast( m_target )->m_ShapeShifted = GetSpellId();
		}

		if( modelId != 0 )
		{
			m_target->SetUInt32Value( UNIT_FIELD_DISPLAYID, modelId );
			int64 *IDStore = GetCreateIn64Extension( EXTENSION_ID_AURA_SHAPESHIFT_MODEL_BACKUP_VAL );
			int64 *IDStoreStamp = GetCreateIn64Extension( EXTENSION_ID_AURA_SHAPESHIFT_MODEL_BACKUP_STAMP );
			*IDStore = modelId;
			*IDStoreStamp = getMSTime();
			m_target->EventModelChange();
		}

		SafePlayerCast( m_target )->SetShapeShift( mod->m_miscValue );

		// check for spell id
		if( additional_cast_spellId > 0 )
		{ 
			SpellEntry* spellInfo = dbcSpell.LookupEntry(additional_cast_spellId );

			Spell *sp = SpellPool.PooledNew( __FILE__, __LINE__ );
			sp->Init( m_target, spellInfo, true, NULL );
			SpellCastTargets tgt(m_target->GetGUID());
			sp->prepare( &tgt );
		}

		// remove the caster from imparing movements
		if( freeMovements )
//			m_target->RemoveAllAurasByMechanic( MECHANIC_ENSNARED, -1, false );
			//since patch 4.0.6 changing shapeshift only clears shnares and not roots
			m_target->RemoveAllAuraType( SPELL_AURA_MOD_DECREASE_SPEED );
	}
	else
	{
		//remove auras that reauire this shapeshift(including talents)
		if( m_target->IsPlayer() == false )
		{
			for(uint32 i = FIRST_AURA_SLOT; i < MAX_PASSIVE_AURAS1(m_target); ++i)
			{
				if( m_target->m_auras[i] != NULL && m_target->m_auras[i]->GetSpellProto() && m_target->m_auras[i]->IsPositive() )
				{
					uint32 requiredShapeShift = m_target->m_auras[i]->GetSpellProto()->RequiredShapeShift;
					uint32 mask = ( 1 << ( mod->m_miscValue - 1 ) ) | ( 1 << (FORM_CUSTOM_NORMAL-1) );
					if( ( requiredShapeShift & mask ) == 0 )
						m_target->m_auras[i]->Remove();
				}
			}
		}
		//execute before changing shape back
		m_target->TryRestorePrevDisplayModAura();
		m_target->EventModelChange();

		if( additional_cast_spellId != GetSpellId() && additional_cast_spellId != 0 )
				m_target->RemoveAura( additional_cast_spellId );

		SafePlayerCast( m_target )->m_ShapeShifted = 0;
		SafePlayerCast( m_target )->SetShapeShift( 0 );

	}
	SafePlayerCast( m_target )->UpdateStats();

	//druid bear form. You gain / loose a lot of stamina that changes max health a lot. The PCT should be retained. Not sure about other form shift stamina changes. There might be more
	float HealthPCTAfterShift = (float)m_target->GetHealth() / (float)m_target->GetMaxHealth();
	if( HealthPCTBeforeShift != HealthPCTAfterShift )
	{
		uint32 NewHP = (float)m_target->GetMaxHealth() * HealthPCTBeforeShift;
		m_target->SetHealth( NewHP );
	}
}

void Aura::SpellAuraModEffectImmunity(bool apply)
{
	if( m_spellProto->Id == 24937 )
		SetPositive();

	if( m_spellProto->Id == 23333 || m_spellProto->Id == 23335 || m_spellProto->Id == 34976 )
	{
		if( !apply )
		{
            Player* plr = SafePlayerCast( GetUnitCaster() );
			if( plr == NULL || plr->GetTypeId() != TYPEID_PLAYER || plr->m_bg == NULL)
			{ 
				return;
			}

			plr->m_bg->HookOnFlagDrop(plr);
		}
	}
}

void Aura::SpellAuraModStateImmunity(bool apply)
{
	//%50 chance to dispel 1 magic effect on target
	//23922
}

void Aura::SendIgnoreStateAura()
{
	if( m_target == NULL || !m_target->IsPlayer() )
		return;
	//already sent packet for these
	//seems to bug out in 14333
//	if( IsVisible() )
//		return;
	WorldPacket tpacket(SMSG_AURA_UPDATE, 14+2);
	tpacket << m_target->GetNewGUID();
	tpacket << (uint8)(m_visualSlot2);	//some unexistent slot from our point of view
	tpacket << GetSpellProto()->Id;
	tpacket << (uint16)0x0F;	//cause blizz says so flags
	tpacket << (uint8)0x4B;	//cause blizz says so spell level (never !)
	tpacket << (uint8)0x00;	//cause blizz says so stack
	SafePlayerCast(m_target)->GetSession()->SendPacket( &tpacket );
}

void Aura::RemoveIgnoreStateAura()
{
	if( m_target == NULL || !m_target->IsPlayer() )
		return;
	//already sent packet for these
	if( IsVisible() == false )
		return;
	WorldPacket tpacket(SMSG_AURA_UPDATE, 14+2);
	tpacket << m_target->GetNewGUID();
	tpacket << (uint8)(m_visualSlot2);	//some unexistent slot from our point of view
	tpacket << uint32(0);
	SafePlayerCast(m_target)->GetSession()->SendPacket( &tpacket );
}

void Aura::SpellAuraModIgnoreState(bool apply)
{
	if( !m_target->IsPlayer() ) 
	{ 
		return;
	}
	//do we have to do anything here or client will let us use anything ?
	if( apply )
	{
//		uint32 slot = m_visualSlot;
		m_flags |= NEED_STATEIGNORE_RESEND_ON_MAPCHANGE;
		SafePlayerCast( m_target )->IgnoreSpellSpecialCasterStateRequirement++;
		//slot number needs to be large or you cannot remove it client side :S
		m_visualSlot2 = 90 - SafePlayerCast( m_target )->IgnoreSpellSpecialCasterStateRequirement;
		//trigger as soon as possible
//		sEventMgr.AddEvent( this, &Aura::SendIgnoreStateAura, EVENT_SEND_PACKET_TO_PLAYER_AFTER_LOGIN, 5000, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
		SendIgnoreStateAura();
		//hackfix until i fix it properly. On chaning maps we need to resend this
//		if( GetDuration() != (uint32)(-1) )
//			sEventMgr.AddEvent( this, &Aura::SendIgnoreStateAura, GetSpellId(), slot, EVENT_SEND_PACKET_TO_PLAYER_AFTER_LOGIN, 60*1000, 0, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
	}
	else if( SafePlayerCast( m_target )->IgnoreSpellSpecialCasterStateRequirement > 0 )
	{
		sEventMgr.RemoveEvents( this, EVENT_SEND_PACKET_TO_PLAYER_AFTER_LOGIN );
		RemoveIgnoreStateAura();
		SafePlayerCast( m_target )->IgnoreSpellSpecialCasterStateRequirement--;
	}
}

void Aura::SpellAuraModSchoolImmunity(bool apply)
{
//	if( apply && m_spellProto->NameHash == SPELL_HASH_BLESSING_OF_PROTECTION )
//		m_target->RemoveAurasByInterruptFlag(AURA_INTERRUPT_ON_INVINCIBLE);

	uint8 *SchoolImmunityList;
	if(apply)
	{
		//fixme me may be negative
		Unit * c = GetUnitCaster();
		if(c)
		{
			if( isAttackable(c,m_target) )
				SetNegative();
			else 
				SetPositive();
		}
		else
			SetPositive();
	}

	if( IsPositive() )
		SchoolImmunityList = m_target->SchoolImmunityAntiEnemy;
	else
		SchoolImmunityList = m_target->SchoolImmunityAntiFriend;

	for( uint32 ApplyBoth = 0; ApplyBoth <= (uint32)( ( GetSpellProto()->c_is_flags3 & SPELL_FLAG3_IS_SCHOOL_IMMUNE_ALL )!=0);ApplyBoth++)
	{
		int32 AuraParseStart,AuraParseEnd;
		if( SchoolImmunityList == m_target->SchoolImmunityAntiFriend )
		{
			AuraParseStart = POS_AURAS_START;
			AuraParseEnd = MAX_POSITIVE_AURAS1( m_target );
		}
		else
		{
			AuraParseStart = NEG_AURAS_START;
			AuraParseEnd = MAX_NEGATIVE_AURAS1( m_target );
		}
		if( apply )
		{
	//		sLog.outDebug("%s: value=%x", __FUNCTION__, mod->m_miscValue);
			for(uint32 i = 0; i < SCHOOL_COUNT; i++)
				if(mod->m_miscValue & (1<<i))
				{
					SchoolImmunityList[i]++;
					//maybe there is a spell attribute flag to detect this
	//				m_target->RemoveAurasOfSchool(i, false, true); //hmm, script this if you need. Cyclone does not need to remove DOTS, just make you immune to dmg
				}

			//disable Auras with school mask
/*			for( uint32 i = AuraParseStart; i < AuraParseEnd; i++ )
			{
				Aura *a = m_target->m_auras[i];
				if( a && a != this && ( a->GetSpellProto()->SchoolMask & mod->m_miscValue ) != 0 )
					a->ApplyModifiers( false );
			} */
		}
		else
		{
			for(int i = 0; i < SCHOOL_COUNT; i++)
				if( ( mod->m_miscValue & (1<<i) ) && SchoolImmunityList[i]>0 )
					SchoolImmunityList[i]--;

			//enable auras with school mask
/*			for( uint32 i = AuraParseStart; i < AuraParseEnd; i++ )
			{
				Aura *a = m_target->m_auras[i];
				if( a && a != this && ( a->GetSpellProto()->SchoolMask & mod->m_miscValue ) != 0 )
					a->ApplyModifiers( true );
			}*/
		}

		if( IsPositive() )
			SchoolImmunityList = m_target->SchoolImmunityAntiFriend;
		else
			SchoolImmunityList = m_target->SchoolImmunityAntiEnemy;
	}
}

void Aura::SpellAuraModAPBasedOnStat(bool apply)
{
	if( mod->m_miscValue > 5 )
	{ 
		return;
	}

	if( apply )
	{
		if(mod->m_amount<0)
			SetNegative();
		else
			SetPositive();

		mod->fixed_amount[ 0 ] = mod->m_amount * m_target->GetUInt32Value( UNIT_FIELD_STAT0 + mod->m_miscValue ) / 100;
		if( mod->fixed_amount[ 0 ] > 0 )
			m_target->ModUnsigned32Value(UNIT_FIELD_ATTACK_POWER_MOD_POS,mod->fixed_amount[ 0 ]);
		else
			m_target->ModUnsigned32Value(UNIT_FIELD_ATTACK_POWER_MOD_NEG,mod->fixed_amount[ 0 ]);
	}
	else
	{
		if( mod->fixed_amount[ 0 ] > 0 )
			m_target->ModUnsigned32Value(UNIT_FIELD_ATTACK_POWER_MOD_POS,mod->fixed_amount[ 0 ]);
		else
			m_target->ModUnsigned32Value(UNIT_FIELD_ATTACK_POWER_MOD_NEG,mod->fixed_amount[ 0 ]);
	}

	m_target->CalcDamage();
}

void Aura::SpellAuraModDmgImmunity(bool apply)
{

}

void Aura::SpellAuraModDispelImmunity(bool apply)
{
	if( mod->m_miscValue > DISPEL_COUNT )
		return;
	if(apply)
		m_target->dispels[mod->m_miscValue]++;
	else
		m_target->dispels[mod->m_miscValue]--;

	if(apply)
	{
		for(uint32 x=0;x<MAX_NEGATIVE_AURAS1(m_target);x++)
			if(m_target->m_auras[x])
				if( m_target->m_auras[x]->GetSpellProto()->GetDispelType() == (uint32)mod->m_miscValue )
					m_target->m_auras[x]->Remove();
	}
}

void Aura::SpellAuraProcTriggerSpell(bool apply)
{
	if(apply)
	{
		ProcTriggerSpell *pts = new ProcTriggerSpell( GetSpellProto(), this );
		pts->caster = m_casterGuid;
		if(GetSpellProto()->eff[mod->i].EffectTriggerSpell)
			pts->spellId=GetSpellProto()->eff[mod->i].EffectTriggerSpell;
		else
		{
			sLog.outDebug("Warning,trigger spell is null for spell %u",GetSpellProto()->Id);
			return;
		}
		Unit *caster = GetUnitCaster();
		int32 proc_Chance = pts->procChance;
		//sadly talents get casted out of order. So we need this to be recalculated every time
//		if( caster && caster->SM_Mods ) 
//			SM_FIValue( caster->SM_Mods->SM_FChanceOfSuccess, (int32*)&proc_Chance, GetSpellProto()->GetSpellGroupType() );
		if( proc_Chance <= 0 )
		{
			sLog.outDebug("Warning,trigger spell %u proc chance is less then 0",GetSpellProto()->Id);
			return;
		}
		else 
			pts->procChance = proc_Chance;
		pts->created_with_value = mod->m_amount;
		int charges = GetSpellProto()->procCharges;
		if( (GetSpellProto()->c_is_flags & SPELL_FLAG_IS_REQUIRING_SM_MOD) && caster != NULL && caster->SM_Mods )
		{
			SM_FIValue( caster->SM_Mods->SM_FCharges, &charges, GetSpellProto()->GetSpellGroupType() );
			SM_PIValue( caster->SM_Mods->SM_PCharges, &charges, GetSpellProto()->GetSpellGroupType() );
#ifdef COLLECTION_OF_UNTESTED_STUFF_AND_TESTERS
			float spell_flat_modifers=0;
			float spell_pct_modifers=0;
			SM_FIValue(GetUnitCaster()->SM_FCharges,&spell_flat_modifers,GetSpellProto()->GetSpellGroupType());
			SM_FIValue(GetUnitCaster()->SM_PCharges,&spell_pct_modifers,GetSpellProto()->GetSpellGroupType());
			if(spell_flat_modifers!=0 || spell_pct_modifers!=0)
				printf("!!!!!spell charge bonus mod flat %f , spell range bonus pct %f , spell range %f, spell group %u\n",spell_flat_modifers,spell_pct_modifers,maxRange,m_spellInfo->GetSpellGroupType());
#endif
		}
		pts->procCharges = charges;

		if( m_castedItemId )
		{
			if( GetSpellProto()->proc_interval == 0 )
				pts->procChance = 5;
			//this is needed to avoid item swap on very long cooldown items
			pts->LastTrigger = getMSTime() + MAX( GetSpellProto()->proc_interval, 2000 );
		}
		m_target->RegisterProcStruct(pts);
		sLog.outDebug("%u is registering %u chance %u flags %u charges %u triggeronself %u interval %u requires hash %u\n",pts->origId,pts->spellId,pts->procChance,m_spellProto->procFlags,charges,(m_spellProto->procFlags2 & PROC2_TARGET_SELF)!=0,m_spellProto->proc_interval,m_spellProto->ProcOnNameHash[0] || m_spellProto->ProcOnNameHash[1] || m_spellProto->ProcOnNameHash[2]);
	}
	else
	{
		m_target->UnRegisterProcStruct( this );
	}
}

void Aura::SpellAuraProcTriggerDamage(bool apply)
{
	if(apply)
	{
		DamageProc ds;
		ds.m_damage = mod->m_amount;
		ds.m_spellId = GetSpellProto()->Id;
		ds.m_school = GetSpellProto()->School;
		ds.m_flags = m_spellProto->procFlags;
		ds.owner = (void*)this;
		m_target->m_damageShields.push_back(ds);
		sLog.outDebug("registering dmg proc %u, school %u, flags %u, charges at least %u \n",ds.m_spellId,ds.m_school,ds.m_flags,m_spellProto->procCharges);
	}
	else
	{
		for(std::list<struct DamageProc>::iterator i = m_target->m_damageShields.begin();i != m_target->m_damageShields.end();i++)
		{
			if(i->owner == this)
			{
				m_target->m_damageShields.erase(i);
				break;
			}
		}
	}
}

void Aura::SpellAuraTrackCreatures(bool apply)
{
	if(m_target->IsPlayer())
	{
		if(apply)
		{
//			if(SafePlayerCast( m_target )->TrackingSpell)
//				m_target->RemoveAura( SafePlayerCast( m_target )->TrackingSpell);

			m_target->SetUInt32Value(PLAYER_TRACK_CREATURES,(uint32)1<< (mod->m_miscValue-1));
//			SafePlayerCast( m_target )->TrackingSpell = GetSpellId();
		}
		else
		{
//			SafePlayerCast( m_target )->TrackingSpell = 0;
			m_target->SetUInt32Value(PLAYER_TRACK_CREATURES,0);
		}
	}
}

void Aura::SpellAuraTrackResources(bool apply)
{
	if(m_target->IsPlayer())
	{
		if(apply)
		{
//			if(SafePlayerCast( m_target )->TrackingSpell)
//				m_target->RemoveAura(SafePlayerCast( m_target )->TrackingSpell);

			m_target->SetUInt32Value(PLAYER_TRACK_RESOURCES,(uint32)1<< (mod->m_miscValue-1));
//			SafePlayerCast( m_target )->TrackingSpell = GetSpellId();
		}
		else
		{
//			SafePlayerCast( m_target )->TrackingSpell = 0;
			m_target->SetUInt32Value(PLAYER_TRACK_RESOURCES,0);
		}
	}
}

void Aura::SpellAuraModParryPerc(bool apply)
{
	//if (m_target->IsPlayer())
	{
		int32 amt;
		if(apply)
		{
			amt = mod->m_amount;
			if(amt<0)
				SetNegative();
			else
				SetPositive();

		}
		else
			amt = -mod->m_amount;

		m_target->SetParryFromSpell(m_target->GetParryFromSpell() + amt );
		if ((m_target->IsPlayer()))
		{
			SafePlayerCast( m_target )->UpdateChances();
		}
	}
}

void Aura::SpellAuraModDodgePerc(bool apply)
{
	//if (m_target->IsPlayer())
	{
		int32 amt = mod->m_amount;
//		SM_FIValue(m_target->SM_FSPELL_VALUE, &amt, GetSpellProto()->GetSpellGroupType());
		if(apply)
		{
			if(amt<0)
				SetNegative();
			else
				SetPositive();
		}
		else
			amt = -amt;

		m_target->SetDodgeFromSpell(m_target->GetDodgeFromSpell() + amt );
		if( m_target->IsPlayer() )
			SafePlayerCast( m_target )->UpdateChances();
	}
}

void Aura::SpellAuraModBlockPerc(bool apply)
{
	//if (m_target->IsPlayer())
	{
		int32 amt;
		if(apply)
		{
			amt = mod->m_amount;
			if(amt<0)
				SetNegative();
			else
				SetPositive();
		}
		else
			amt = -mod->m_amount;

		m_target->SetBlockFromSpell(m_target->GetBlockFromSpell() + amt);
		if (m_target->IsPlayer())
		{
			SafePlayerCast( m_target )->UpdateStats();
		}
	}
}

void Aura::SpellAuraModCritPerc(bool apply)
{
	Player *p_target;
	if( m_target->IsPlayer() )
		p_target = SafePlayerCast( m_target );
	else
		p_target = NULL;
	if (m_target->IsPlayer())
	{
		if(apply)
		{
			WeaponModifier *md;
			md = new WeaponModifier;
			md->value = float(mod->m_amount);
			md->wclass = GetSpellProto()->GetEquippedItemClass();
			md->subclass = GetSpellProto()->GetEquippedItemSubClass();
			md->spellentry = GetSpellProto();
			p_target->tocritchance.push_front( md );
		}
		else
		{
			SimplePointerListNode<WeaponModifier> *i;
			for( i=p_target->tocritchance.begin();i!=p_target->tocritchance.end();i = i->Next() )
				if( i->data->spellentry == GetSpellProto() )
				{
					p_target->tocritchance.remove( i, 1 );
					break;
				}
		}
		SafePlayerCast( m_target )->UpdateChances();
	}
	else if( m_target->IsCreature())
	{
		if(apply)
			SafeCreatureCast(m_target)->crit_chance_mod += mod->m_amount;
		else
			SafeCreatureCast(m_target)->crit_chance_mod -= mod->m_amount;
	}
}

void Aura::SpellAuraPeriodicLeech(bool apply)
{
	if(apply)
	{
		SetNegative();
//		uint32 amt = mod->m_amount;

		//hasting affects ticks too
		Unit *Unitc = GetUnitCaster();
		int32 period = GetSpellAmplitude( GetSpellProto(), Unitc, mod->i, m_castedItemId );

//		sEventMgr.AddEvent(this, &Aura::EventPeriodicLeech,amt,period, (int32)mod->i,
		sEventMgr.AddEvent(this, &Aura::EventPeriodicLeech,mod->list_ind,period,
			EVENT_AURA_PERIODIC_LEECH, period ,0,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
	}
}

//void Aura::EventPeriodicLeech(uint32 amount,int32 period,int32 effect_index)
void Aura::EventPeriodicLeech(uint32 aura_list_ind,int32 period)
{
	Unit* m_caster = GetUnitCaster();

	if(!m_caster || !m_target)
	{ 
		return;
	}

	if(m_target->isAlive() && m_caster->isAlive())
	{
		if( m_target->SchoolImmunityAntiEnemy[GetSpellProto()->School] || m_target->SchoolImmunityAntiFriend[GetSpellProto()->School] )
		{ 
			return;
		}
		//check if still in range
/*
		Not sure what spell needs this. Devouring plague for sure does not need it. Maybe channeled spells ?
		float maxRange = GetMaxRange( dbcSpellRange.LookupEntry( GetSpellProto()->rangeIndex ) );
		if( Need_SM_Apply(GetSpellProto()) && m_caster->SM_Mods )
		{
			SM_FFValue( m_caster->SM_Mods->SM_FRange, &maxRange, GetSpellProto()->GetSpellGroupType() );
			SM_PFValue( m_caster->SM_Mods->SM_PRange, &maxRange, GetSpellProto()->GetSpellGroupType() );
#ifdef COLLECTION_OF_UNTESTED_STUFF_AND_TESTERS
			float spell_flat_modifers=0;
			float spell_pct_modifers=0;
			SM_FFValue(u_caster->SM_FRange,&spell_flat_modifers,GetProto()->GetSpellGroupType());
			SM_FFValue(u_caster->SM_PRange,&spell_pct_modifers,GetProto()->GetSpellGroupType());
			if(spell_flat_modifers!=0 || spell_pct_modifers!=0)
				printf("!!!!!spell range bonus mod flat %f , spell range bonus pct %f , spell range %f, spell group %u\n",spell_flat_modifers,spell_pct_modifers,maxRange,GetProto()->GetSpellGroupType());
#endif
		}
		if( maxRange * maxRange < m_caster->GetDistanceSq( m_target ) )
		{
//			m_caster->CancelSpell( NULL );
			Remove();		//suicide here ! I wonder if canceling spell already deleted us
			return;	
		}/**/


		SpellEntry *sp = GetSpellProto();
		Modifier *mod = &m_modList[ aura_list_ind ];
		int32 EffIndex = mod->i;
		uint32 SoulSiphonPCTBonus = mod->m_miscValue;		//i know this sucks, but it is not used apart from this

		dealdamage tdmg;
		tdmg.base_dmg = mod->m_amount;
		tdmg.pct_mod_final_dmg = mod->m_pct_mod;
		if( SoulSiphonPCTBonus > 0 )
			tdmg.pct_mod_final_dmg = mod->m_pct_mod * ( 100 + SoulSiphonPCTBonus ) / 100 / 100;
		m_caster->SpellNonMeleeDamageLog( m_target, sp, &tdmg, sp->quick_tickcount, EffIndex );
		int32 RealDamageDone = tdmg.full_damage - tdmg.resisted_damage - tdmg.absorbed_damage;

		float conv_coef = sp->eff[ EffIndex ].EffectConversionCoef;
		int32 heal_amount = float2int32( RealDamageDone * conv_coef );
		if( heal_amount < 0 )
			heal_amount = 0;

		if( m_caster != NULL && m_caster->isAlive() )
			m_caster->SpellHeal( m_caster, sp, heal_amount, true, true, false, sp->quick_tickcount, 100, EffIndex, 0 );
	}
}

void Aura::SpellAuraModHitChance(bool apply)
{
	if (m_target->IsUnit())
	{
		if (apply)
		{
			SafeUnitCast( m_target )->SetHitFromMeleeSpell( SafeUnitCast( m_target )->GetHitFromMeleeSpell() + mod->m_amount);
			if(mod->m_amount<0)
				SetNegative();
			else
				SetPositive();
		}
		else
		{
			SafeUnitCast( m_target )->SetHitFromMeleeSpell( SafeUnitCast( m_target )->GetHitFromMeleeSpell() - mod->m_amount);
			if( SafeUnitCast( m_target )->GetHitFromMeleeSpell() < 0 )
				SafeUnitCast( m_target )->SetHitFromMeleeSpell( 0 );
		}
	}
}

void Aura::SpellAuraModSpellHitChance(bool apply)
{
	if (m_target->IsPlayer())
	{
		if (apply)
		{
			SafePlayerCast( m_target )->SetHitFromSpell(SafePlayerCast( m_target )->GetHitFromSpell() + mod->m_amount);
			if(mod->m_amount<0)
				SetNegative();
			else
				SetPositive();
		}
		else
		{
			SafePlayerCast( m_target )->SetHitFromSpell(SafePlayerCast( m_target )->GetHitFromSpell() - mod->m_amount);
			if(SafePlayerCast( m_target )->GetHitFromSpell() < 0)
			{
				SafePlayerCast( m_target )->SetHitFromSpell(0);
			}
		}
	}
}

void Aura::SpellAuraTransform(bool apply)
{
	// Try a dummy SpellHandler
	if(sScriptMgr.CallScriptedDummyAura(GetSpellProto(), mod->i, this, apply))
	{ 
		return;
	}

	uint32 displayId = 0;
	CreatureInfo* ci = CreatureNameStorage.LookupEntry(mod->m_miscValue);

	if(ci)
	{
		displayId = ci->Male_DisplayID;
		if( m_target->getGender() && ci->Female_DisplayID )
			displayId = ci->Female_DisplayID;
	}

	Player *p_target;
	if( m_target->IsPlayer() )
	{
		p_target = SafePlayerCast( m_target );
		//this was bugging out stuff
		p_target->BackupRestoreOnceRaceGenderStyle( true );
	}
	else
		p_target = NULL;

	//in cataclysm polymorph does not remove mount ?
	//if you ask me this is very wrong ?
//	if(p_target && p_target->m_MountSpellId)
//		m_target->RemoveAura(p_target->m_MountSpellId);

	if( p_target && p_target->m_MountSpellId )
	{
		//mount but not while in air
		if( p_target->GetSession() && ( p_target->GetSession()->GetMovementInfo()->flags & ( MOVEFLAG_FLYING14333 | MOVEFLAG_AIR_SUSPENSION14333 ) ) == 0 )
			m_target->RemoveAura(p_target->m_MountSpellId);
	}

	if( GetSpellProto()->MechanicsType == MECHANIC_POLYMORPHED && m_target && IsPositive() == false )
	{
		if( apply ) 
			m_target->m_special_state |= UNIT_STATE_POLYMORPH;
		else
			m_target->m_special_state &= ~UNIT_STATE_POLYMORPH;

		//cancel casts as frog or sheep
		if( m_target->isCasting() )
			m_target->GetCurrentSpell()->safe_cancel();
	}
   // SetPositive();
	switch( GetSpellProto()->Id )
	{
		case 20584://wisp
			if( apply )
				displayId = 10045;
		break;

		case 30167: // Red Ogre Costume
		{
			if( apply )
				displayId = 11549;
		}break;

		case 41301: // Time-Lost Figurine
		{
			if( apply )
				displayId = 18628;
		}break;

		case 16739: // Orb of Deception
		{
			if( apply )
			{
				if(m_target->getRace() == RACE_ORC)
				{
					if( m_target->getGender() == 0 )
						displayId = 10139;
					else
						displayId = 10140;
				}
				if(m_target->getRace() == RACE_TAUREN)
				{
					if( m_target->getGender() == 0 )
						displayId = 10136;
					else
						displayId = 10147;
				}
				if(m_target->getRace() == RACE_TROLL)
				{
					if( m_target->getGender() == 0 )
						displayId = 10135;
					else
						displayId = 10134;
				}
				if(m_target->getRace() == RACE_UNDEAD)
				{
					if( m_target->getGender() == 0 )
						displayId = 10146;
					else
						displayId = 10145;
				}
				if(m_target->getRace() == RACE_BLOODELF)
				{
					if( m_target->getGender() == 0 )
						displayId = 17829;
					else
						displayId = 17830;
				}

				if(m_target->getRace() == RACE_GNOME)
				{
					if( m_target->getGender() == 0 )
						displayId = 10148;
					else
						displayId = 10149;
				}
				if(m_target->getRace() == RACE_DWARF)
				{
					if( m_target->getGender() == 0 )
						displayId = 10141;
					else
						displayId = 10142;
				}
				if(m_target->getRace() == RACE_HUMAN)
				{
					if( m_target->getGender() == 0 )
						displayId = 10137;
					else
						displayId = 10138;
				}
				if(m_target->getRace() == RACE_NIGHTELF)
				{
					if( m_target->getGender() == 0 )
						displayId = 10143;
					else
						displayId = 10144;
				}
				if(m_target->getRace() == RACE_DRAENEI)
				{
					if( m_target->getGender() == 0 )
						displayId = 17827;
					else
						displayId = 17828;
				}
			}
		}break;

		case 42365:	// murloc costume
			if( apply )
				displayId = 21723;
			break;

		case 74589:	// Faded Wyzard Hat
			if( apply )
			{
				static uint32 PossibleDisplays[16] = { 29907,29908,29909,30089,30096,30084,30085,30088,30086,30093,30094,30092,11670,7614,30095,19840 };
				displayId = PossibleDisplays[ RandomUInt() % 16 ];
			}
		break;

        // Corsair Costume
        case 51926:
        {
            if ( m_target->IsPlayer() == true && apply )
			{
				switch( m_target->getRace() )
				{
					// Blood Elf
					case RACE_BLOODELF:
						displayId = m_target->getGender() == GENDER_MALE ? 25032 : 25043;
						break;
					// Orc
					case RACE_ORC:
						displayId = m_target->getGender() == GENDER_MALE ? 25039 : 25050;
						break;
					// Troll
					case RACE_TROLL:
						displayId = m_target->getGender() == GENDER_MALE ? 25041 : 25052;
						break;
					// Tauren
					case RACE_TAUREN:
						displayId = m_target->getGender() == GENDER_MALE ? 25040 : 25051;
						break;
					// Undead
					case RACE_UNDEAD:
						displayId = m_target->getGender() == GENDER_MALE ? 25042 : 25053;
						break;
					// Draenei
					case RACE_DRAENEI:
						displayId = m_target->getGender() == GENDER_MALE ? 25033 : 25044;
						break;
					// Dwarf
					case RACE_DWARF:
						displayId = m_target->getGender() == GENDER_MALE ? 25034 : 25045;
						break;
					// Gnome
					case RACE_GNOME:
						displayId = m_target->getGender() == GENDER_MALE ? 25035 : 25046;
						break;
					// Human
					case RACE_HUMAN:
						displayId = m_target->getGender() == GENDER_MALE ? 25037 : 25048;
						break;
					// Night Elf
					case RACE_NIGHTELF:
						displayId = m_target->getGender() == GENDER_MALE ? 25038 : 25049;
						break;
					default:
						break;
				}
			}
            break;
        }
        // Pygmy Oil
        case 53806:
//            displayId = 22512; //was the good one
			displayId = 32093;
            break;
        // Honor the Dead
        case 65386:
        case 65495:
            displayId = m_target->getGender() == GENDER_MALE ? 29203 : 29204;
            break;
        // Darkspear Pride
        case 75532:
            displayId = m_target->getGender() == GENDER_MALE ? 31737 : 31738;
            break;
        // Gnomeregan Pride
        case 75531:
            displayId = m_target->getGender() == GENDER_MALE ? 31654 : 31655;

		case 118://polymorph
		case 851:
		case 5254:
		case 12824:
		case 12825:
		case 12826:
		case 13323:
		case 15534:
		case 22274:
		case 23603:
		case 28270:	 // Polymorph: Cow
		case 28271:	 // Polymorph: Turtle
		case 28272:	 // Polymorph: Pig
		case 61025:  // Polymorph: Serpent
		case 61305:  // Polymorph: Black Cat
		case 61721:  // Polymorph: Rabbit
		case 61780:  // Polymorph: Turkey
			{
				if(!displayId)
				{
					switch(GetSpellProto()->Id)
					{
					case 28270:	 // Cow
						displayId = 1060;
						break;

					case 28272:	 // Pig
						displayId = 16356 + RandomUInt(2);
						break;

					case 28271:	 // Turtle
						displayId = 16359 + RandomUInt(2);
						break;

					default:
						displayId = 856;
						break;

					}
				}

				if(apply)
				{
					if (GetUnitCaster() != NULL && m_target->IsCreature())
						m_target->GetAIInterface()->AttackReaction(GetUnitCaster(), 1, GetSpellId());

					Unit *u_caster = GetUnitCaster();
					if( ( displayId == 856 || displayId == 857 ) && u_caster && u_caster->IsPlayer() )
					{
						if( SafePlayerCast( u_caster )->HasGlyphWithID( GLYPH_MAGE_PENGUIN ) == true )
							displayId = 28216;
						else if( SafePlayerCast( u_caster )->HasGlyphWithID( GLYPH_MAGE_MONKEY ) == true )
							displayId = 39549;
					}

					// remove the current spell (for channelers)
					if(m_target->m_currentSpell && m_target->GetGUID() != m_casterGuid &&
						m_target->m_currentSpell->getState() == SPELL_STATE_CASTING )
					{
						m_target->InterruptSpell();
					}

					sEventMgr.AddEvent(this, &Aura::EventPeriodicHealNoBonus,(uint32)10000,EVENT_AURA_PERIODIC_HEAL,1000,0,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
					m_target->polySpell = GetSpellProto()->Id;
					//some reported that sheeps keep melee fighting
					if( m_target->IsPlayer() )
						SafePlayerCast( m_target )->EventAttackStop();
					//this glyph will remove DOTS from the target
					if( u_caster && u_caster->IsPlayer() && SafePlayerCast( u_caster )->HasGlyphWithID( GLYPH_MAGE_POLYMORPH ) == true )
					{
						for(uint32 x=MAX_POSITIVE_AURAS;x<m_target->m_auras_neg_size;x++)
							if( m_target->m_auras[x] && ( m_target->m_auras[x]->GetSpellProto()->c_is_flags & SPELL_FLAG_IS_DAMAGING ) && m_target->m_auras[x]->GetSpellProto()->quick_tickcount > 1 )
								m_target->m_auras[x]->Remove();
					}
				}
				else
				{
					m_target->polySpell = 0;
				}
			}break;

		case 19937:
			{
				if (apply)
				{
					// TODO: Sniff the spell / item, we need to know the real displayID
					// guessed this may not be correct
					// human = 7820
					// dwarf = 7819
					// halfling = 7818
					// maybe 7842 as its from a lesser npc
					displayId = 7842;
				}
			}break;
		case 47585:  // priest - Dispersion
			{
				if(apply)
				{
					// remove the caster from imparing movements
					m_target->RemoveAurasMovementImpairing();
					m_target->RemoveAllAurasByMechanic( MECHANIC_STUNNED, -1, false );
					m_target->MechanicsDispels[MECHANIC_STUNNED]++;
					m_target->MechanicsDispels[MECHANIC_FLEEING]++;
					m_target->MechanicsDispels[MECHANIC_SILENCED]++;
					//ammount is in pct
					// Walla:
					// The wrong way to regen mana.. there is a spell for that
					// http://www.wowhead.com/spell=49766
					//mod->m_amount = m_target->GetMaxPower( POWER_TYPE_MANA ) * 6 / 100;
					//sEventMgr.AddEvent(this, &Aura::EventPeriodicEnergize,(uint32)mod->m_amount,(uint32)POWER_TYPE_MANA, EVENT_AURA_PERIODIC_ENERGIZE,1000,6,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
				}
				else
				{
					if( m_target->MechanicsDispels[MECHANIC_STUNNED] > 0 )
						m_target->MechanicsDispels[MECHANIC_STUNNED]--;
					if( m_target->MechanicsDispels[MECHANIC_FLEEING] > 0 )
						m_target->MechanicsDispels[MECHANIC_FLEEING]--;
					if( m_target->MechanicsDispels[MECHANIC_SILENCED] > 0 )
						m_target->MechanicsDispels[MECHANIC_SILENCED]--;
				}
			}break;

		default:
		{
		}break;
	};

	if(apply)
	{
		if( displayId )
		{
			m_target->SetUInt32Value( UNIT_FIELD_DISPLAYID, displayId );
			int64 *IDStore = GetCreateIn64Extension( EXTENSION_ID_AURA_SHAPESHIFT_MODEL_BACKUP_VAL );
			int64 *IDStoreStamp = GetCreateIn64Extension( EXTENSION_ID_AURA_SHAPESHIFT_MODEL_BACKUP_STAMP );
			*IDStore = displayId;
			*IDStoreStamp = getMSTime();
		}
	}
	else
	{
		m_target->TryRestorePrevDisplayModAura();
	}

	if( m_target )
		m_target->EventModelChange();
}

void Aura::SpellAuraModSpellCritChance(bool apply)
{
	int32 amt;
	if(apply)
	{
		amt = mod->m_amount;
		if(amt<0)
			SetNegative();
		else
			SetPositive();
	}
	else
		amt = -mod->m_amount;

	m_target->spellcritperc += amt;

	if( m_target->IsPlayer() )
	{
		SafePlayerCast( m_target )->SetSpellCritFromSpell( SafePlayerCast( m_target )->GetSpellCritFromSpell() + amt );
		SafePlayerCast( m_target )->UpdateChanceFields();
	}
}

void Aura::SpellAuraIncreaseSwimSpeed(bool apply)
{
	if(apply)
	{
		if(m_target->isAlive())  
			SetPositive();
		mod->fixed_amount[0] = m_target->m_swimSpeed * mod->m_amount;
		m_target->m_swimSpeed += ( float ) mod->fixed_amount[0] / 100.0f;
	}
	else
		m_target->m_swimSpeed -= ( float ) mod->fixed_amount[0] / 100.0f;

	if( m_target->IsPlayer() )
		SafePlayerCast( m_target )->SetPlayerSpeed( SWIM, m_target->m_swimSpeed );
}

void Aura::SpellAuraModCratureDmgDone(bool apply)
{
	if(m_target->IsPlayer())
	{
		if(apply)
		{
			for(uint32 x = 0; x < CREATURE_TYPES; x++)
				if(mod->m_miscValue & (((uint32)1)<<x))
					SafePlayerCast( m_target )->IncreaseDamageByType[x+1] += mod->m_amount;

			mod->m_amount < 0 ? SetNegative() : SetPositive();
		}
		else
			for(uint32 x = 0; x < CREATURE_TYPES; x++)
				if(mod->m_miscValue & (((uint32)1)<<x) )
					SafePlayerCast( m_target )->IncreaseDamageByType[x+1] -= mod->m_amount;
	}
}

void Aura::SpellAuraPacifySilence(bool apply)
{
	// Can't Attack or Cast Spells
	if(apply)
	{
		if(m_spellProto->Id == 24937)
			SetPositive();
		else
			SetNegative();

		m_target->m_pacified++;
		m_target->m_silenced++;
		m_target->m_special_state |= UNIT_STATE_PACIFY | UNIT_STATE_SILENCE;
		m_target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_SILENCED);

		if(m_target->m_currentSpell && m_target->GetGUID() != m_casterGuid &&
			m_target->m_currentSpell->getState() == SPELL_STATE_CASTING )
			{
				m_target->InterruptSpell();
			}
	}
	else
	{
		m_target->m_pacified--;

		if(m_target->m_pacified == 0)
		{
			m_target->m_special_state &= ~UNIT_STATE_PACIFY;
			m_target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
		}

		m_target->m_silenced--;

		if(m_target->m_silenced == 0)
		{
			m_target->m_special_state &= ~UNIT_STATE_SILENCE;
			m_target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED);
		}
	}
}

void Aura::SpellAuraModScale(bool apply)
{
	if( apply )
		m_target->ObjectSizeMods.push_front( mod );
	else
		m_target->ObjectSizeMods.remove( mod );
	m_target->UpdateSizeMod();
}

void Aura::SpellAuraPeriodicHealthFunnel(bool apply)
{
	if(apply)
	{
		uint32 amt = mod->m_amount;
		//hasting affects ticks too
		Unit *Unitc = GetUnitCaster();
		int32 period = GetSpellAmplitude( GetSpellProto(), Unitc, mod->i, m_castedItemId );
		sEventMgr.AddEvent(this, &Aura::EventPeriodicHealthFunnel, amt,
			EVENT_AURA_PERIODIC_HEALTH_FUNNEL, period , 0,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
	}
}

void Aura::EventPeriodicHealthFunnel(uint32 amount)
{
	//Blood Siphon
	//Deals 200 damage per second.
	//Feeding Hakkar 1000 health per second.
	Unit* m_caster = GetUnitCaster();
	if(!m_caster)
	{ 
		return;
	}
	if(m_target->isAlive() && m_caster->isAlive())
	{
		amount -= m_target->ResilianceAbsorb( amount, GetSpellProto(), m_caster );
		m_caster->DealDamage(m_target, amount, GetSpellId(),false);
		uint32 healamt = 1000;
		uint32 newHealth = m_caster->GetUInt32Value(UNIT_FIELD_HEALTH) + healamt;

		uint32 mh = m_caster->GetUInt32Value(UNIT_FIELD_MAXHEALTH);
		if(newHealth <= mh)
			m_caster->SetUInt32Value(UNIT_FIELD_HEALTH, newHealth);
		else
			m_caster->SetUInt32Value(UNIT_FIELD_HEALTH, mh);

//		m_target->SendSpellPeriodicAuraLog(m_target, m_target, m_spellProto->Id, m_spellProto->School, 1000, 0, 0, FLAG_PERIODIC_LEECH, 0);
		m_caster->SendCombatLogMultiple(m_target,healamt,0,0,0, m_spellProto->spell_id_client, m_spellProto->SchoolMask, COMBAT_LOG_PERIODIC_HEAL,0,COMBAT_LOG_SUBFLAG_PERIODIC_HEAL);

		m_caster->RemoveAurasByHeal();
	}
}

void Aura::SpellAuraPeriodicManaLeech(bool apply)
{
	if(apply)
	{
		uint32 amt=mod->m_amount;

		//hasting affects ticks too
		Unit *Unitc = GetUnitCaster();
		int32 period = GetSpellAmplitude( GetSpellProto(), Unitc, mod->i, m_castedItemId );

		sEventMgr.AddEvent(this, &Aura::EventPeriodicManaLeech,amt,mod->i,
			EVENT_AURA_PERIODIC_LEECH, period ,0,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
	}
}

void Aura::EventPeriodicManaLeech(uint32 amount,uint32 effect_index)
{
	Unit* m_caster = GetUnitCaster();
	if(!m_caster)
	{ 
		return;
	}
	if(m_target->isAlive() && m_caster->isAlive())
	{
		int32 take_amt = (int32)MIN( (int32)amount , m_target->GetPower( POWER_TYPE_MANA ) );

		switch( m_spellProto->NameHash )
		{
			case SPELL_HASH_VIPER_STING:
			{
				if( m_target->GetPower( POWER_TYPE_MANA ) * ( take_amt / 100.0f ) < m_caster->GetMaxPower( POWER_TYPE_MANA ) * 0.02f )
					take_amt = float2int32( m_target->GetPower( POWER_TYPE_MANA ) * ( take_amt / 100.0f ) );
				else
					take_amt = float2int32( m_caster->GetMaxPower( POWER_TYPE_MANA ) * 0.02f );
			}break;
			case SPELL_HASH_DRAIN_MANA: // Warlock - Drain mana 
			{
				if( m_target->GetMaxPower( POWER_TYPE_MANA ) * take_amt / 100 < m_caster->GetMaxPower( POWER_TYPE_MANA ) * 6 / 100 )
					take_amt = float2int32( m_target->GetMaxPower( POWER_TYPE_MANA )* ( take_amt / 100.0f ) );
				else
					take_amt = int32( m_caster->GetMaxPower( POWER_TYPE_MANA ) * 6 / 100 );
			}break;
		}

		// Drained amount should be reduced by resilence 
		if(m_target->IsPlayer())
			take_amt -= m_target->ResilianceAbsorb( take_amt, GetSpellProto(), m_caster );

		if( take_amt <= 0 )
			return;

		int32 add_amt = (int32)MIN( float2int32( take_amt * GetSpellProto()->eff[effect_index].EffectConversionCoef ), (int32)m_target->GetPower( POWER_TYPE_MANA ) );
		uint32 cm = m_caster->GetPower( POWER_TYPE_MANA ) + add_amt;
		uint32 mm = m_caster->GetMaxPower( POWER_TYPE_MANA );
		if(cm <= mm)
			m_caster->SetPower( POWER_TYPE_MANA, cm);
		else
			m_caster->SetPower( POWER_TYPE_MANA, mm);
		m_target->ModPower( POWER_TYPE_MANA, -take_amt);
	}
}

void Aura::SpellAuraModCastingSpeed(bool apply)
{
	//haste is applied as : castspeed / ( 1 + hastebonus ) 
	// this aura is applied as : ( castspeed / ( 1 + hastebonus ) ) * ( 1 + X% )
	Player *p_target;
	if( m_target->IsPlayer() )
		p_target = SafePlayerCast( m_target );
	else
		p_target = NULL;
	if( p_target )
	{
		if(apply)
//			p_target->ModAttackSpeed( mod->m_amount, MOD_SPELL );
			p_target->m_AuraCastSpeedMods += mod->m_amount;	//value is negative by default
		else
//			p_target->ModAttackSpeed( -mod->m_amount, MOD_SPELL );
			p_target->m_AuraCastSpeedMods -= mod->m_amount;
		p_target->ModAttackSpeed( 0, MOD_SPELL );
	}
	else 
	{
		if(apply)
			m_target->ModFloatValue(UNIT_MOD_CAST_SPEED, 1.0f/(1.0f+mod->m_amount/100.0f) );
		else
			m_target->ModFloatValue(UNIT_MOD_CAST_SPEED, -1.0f/(1.0f+mod->m_amount/100.0f) );
	}
}

void Aura::SpellAuraFeignDeath(bool apply)
{
	if( m_target->IsPlayer() )
	{
		Player* pTarget = SafePlayerCast( m_target );
		WorldPacket data(50);
		if( apply )
		{
			pTarget->EventAttackStop();
			pTarget->SetFlag( UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH );
			pTarget->SetFlag( UNIT_FIELD_FLAGS, UNIT_FLAG_FEIGN_DEATH );
			//pTarget->SetFlag( UNIT_FIELD_FLAGS, UNIT_FLAG_DEAD );
			pTarget->SetFlag( UNIT_DYNAMIC_FLAGS, U_DYN_FLAG_DEAD );
			//pTarget->SetUInt32Value( UNIT_NPC_EMOTESTATE, EMOTE_STATE_DEAD );

			data.SetOpcode( SMSG_START_MIRROR_TIMER );
			data << uint32( TIMER_FEIGNDEATH );		// type
			data << uint32( GetDuration() );
			data << uint32( GetDuration() );
			data << uint32( 0xFFFFFFFF );
			data << uint8( 0 );
			data << uint32( m_spellProto->Id );		// ???
			pTarget->GetSession()->SendPacket( &data );

			data.Initialize( SMSG_CLEAR_TARGET );
			data << pTarget->GetGUID();
//			pTarget->setDeathState(DEAD);

			//now get rid of mobs agro. pTarget->CombatStatus.AttackersForgetHate() - this works only for already attacking mobs
			pTarget->AquireInrangeLock();
			InrangeLoopExitAutoCallback AutoLock;
			for(InRangeSetRecProt::iterator itr = pTarget->GetInRangeSetBegin( AutoLock ); itr != pTarget->GetInRangeSetEnd(); itr++ )
			{
				if((*itr)->IsUnit() && SafeUnitCast((*itr))->isAlive())
				{
					if((*itr)->GetTypeId()==TYPEID_UNIT)
						SafeUnitCast(*itr)->GetAIInterface()->RemoveThreatByPtr( pTarget );
					
					//if this is player and targeting us then we interrupt cast
					if( (*itr)->IsPlayer() && isAttackable( pTarget, (*itr) ) )
					{
						Player* plr = SafePlayerCast( *itr );

						if( plr->isCasting() 
							&& plr->GetSelection() == pTarget->GetGUID()
							&& plr->GetCurrentSpell()->m_targets.m_unitTarget == pTarget->GetGUID()
							)
							plr->InterruptSpell( ); //cancel current casting spell

						plr->GetSession()->SendPacket( &data );
					}
				}
			}
			pTarget->ReleaseInrangeLock();
			pTarget->setDeathState(ALIVE);
		}
		else
		{
			pTarget->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);
			pTarget->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FEIGN_DEATH);
			pTarget->RemoveFlag(UNIT_DYNAMIC_FLAGS, U_DYN_FLAG_DEAD);
			//pTarget->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DEAD);
			//pTarget->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
			data.SetOpcode(SMSG_STOP_MIRROR_TIMER);
			data << uint32(2);
			pTarget->GetSession()->SendPacket(&data);
		}
	}
}

void Aura::SpellAuraModDisarm(bool apply)
{
	Player *p_target;
	if( m_target->IsPlayer() )
		p_target = SafePlayerCast( m_target );
	else
		p_target = NULL;
	if(apply)
	{
		if( p_target != NULL && p_target->IsInFeralForm() ) 
		{ 
			return;
		}

		SetNegative();

		m_target->disarmed |= DISARM_TYPE_FLAG_MAINHAND;
		m_target->m_special_state |= UNIT_STATE_DISARMED;
		m_target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED);
	}
	else
	{
		m_target->disarmed &= ~DISARM_TYPE_FLAG_MAINHAND;
		m_target->m_special_state &= ~UNIT_STATE_DISARMED;
		m_target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED);
	}
	if( m_target && m_target->IsPlayer() )
	{
		//get item and "remove it"
		Item *it = SafePlayerCast( m_target )->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_MAINHAND );
		if( it )
			SafePlayerCast( m_target )->ApplyItemMods( it, EQUIPMENT_SLOT_MAINHAND, !apply );
		else
		{
			SafePlayerCast( m_target )->UpdateAttackSpeed();
			SafePlayerCast( m_target )->CalcDamage();
		}
	}
}

void Aura::SpellAuraModDisarmOffhand(bool apply)
{
	Player *p_target;
	if( m_target->IsPlayer() )
		p_target = SafePlayerCast( m_target );
	else
		p_target = NULL;
	if(apply)
	{
		if( p_target != NULL && p_target->IsInFeralForm() ) 
		{ 
			return;
		}

		SetNegative();

		m_target->disarmed |= DISARM_TYPE_FLAG_OFFHAND;
		m_target->m_special_state |= UNIT_STATE_DISARMED;
		m_target->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISARM_OFFHAND);
	}
	else
	{
		m_target->disarmed &= ~DISARM_TYPE_FLAG_OFFHAND;
		m_target->m_special_state &= ~UNIT_STATE_DISARMED;
		m_target->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISARM_OFFHAND);
	}
	if( m_target && m_target->IsPlayer() )
	{
		//get item and "remove it"
		Item *it = SafePlayerCast( m_target )->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_OFFHAND );
		if( it )
			SafePlayerCast( m_target )->ApplyItemMods( it, EQUIPMENT_SLOT_OFFHAND, !apply );
		else
		{
			SafePlayerCast( m_target )->UpdateAttackSpeed();
			SafePlayerCast( m_target )->CalcDamage();
		}
	}
}

void Aura::SpellAuraModDisarmRanged(bool apply)
{
	Player *p_target;
	if( m_target->IsPlayer() )
		p_target = SafePlayerCast( m_target );
	else
		p_target = NULL;
	if(apply)
	{
		if( p_target != NULL && p_target->IsInFeralForm() ) 
		{ 
			return;
		}

		SetNegative();

		m_target->disarmed |= DISARM_TYPE_FLAG_RANGED;
		m_target->m_special_state |= UNIT_STATE_DISARMED;
		m_target->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISARM_RANGED);
	}
	else
	{
		m_target->disarmed &= ~DISARM_TYPE_FLAG_RANGED;
		m_target->m_special_state &= ~UNIT_STATE_DISARMED;
		m_target->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISARM_RANGED);
	}
	if( m_target && m_target->IsPlayer() )
	{
		//get item and "remove it"
		Item *it = SafePlayerCast( m_target )->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_RANGED );
		if( it )
			SafePlayerCast( m_target )->ApplyItemMods( it, EQUIPMENT_SLOT_RANGED, !apply );
		else
		{
			SafePlayerCast( m_target )->UpdateAttackSpeed();
			SafePlayerCast( m_target )->CalcDamage();
		}
	}
}

void Aura::SpellAuraModStalked(bool apply)
{
	if(apply)
	{
		m_target->stalkedby = m_casterGuid;
		SetNegative();
	}
	else
	{
		m_target->stalkedby = 0;
	}
}

void Aura::SpellAuraSchoolAbsorb(bool apply)
{
	Absorb *ab;
	if(apply)
	{
		SetPositive();

		int32 val = mod->m_amount;
		Unit * caster = GetUnitCaster() ;
		if( caster )
		{
			//This will fix talents that affects damage absorved.
//			int flat = 0;
//			SM_FIValue( plr->SM_FMiscEffect, &flat, GetSpellProto()->GetSpellGroupType() );
//			val += float2int32( float( val * flat ) / 100.0f );
			if( GetSpellProto()->eff[ mod->i ].EffectSpellPowerCoef != SPELL_EFFECT_MAGIC_UNDECIDED_SPELL_POWER_COEFF )
				val += float2int32( float( caster->GetDamageDoneMod( GetSpellProto()->School ) ) * GetSpellProto()->eff[ mod->i ].EffectSpellPowerCoef );
		}
		if( GetSpellProto()->NameHash == SPELL_HASH_POWER_WORD__SHIELD && caster->IsPlayer() )
		{
			// PWS is threated as a heal spell and instead spell dmg it scales with healing
//			$shield=${((($SP*0.87)+$m1)*$<mastery>*$<spirit>*$<twin3>*$<ipws2>)}",
//			"$twin1=$?s47586[${1.02}][${1.00}]
//			$twin2=$?s47587[${1.04}][${$<twin1>}]
//			$twin3=$?s47588[${1.06}][${$<twin2>}]
//			$ipws1=$?s14748[${1.10}][${1.00}]
//			$mastery=$?s47540[${((100+$77484m1)/100)}][${1.00}] 
//			$spirit=$?s87336[${((100+$87336m1)/100)}][${1.00}]
			Aura *a;
			a = caster->HasAuraWithNameHash( SPELL_HASH_TWIN_DISCIPLINES, 0, AURA_SEARCH_PASSIVE );
			if( a )	//Twin Disciplines
				mod->m_pct_mod = mod->m_pct_mod * ( 100 + a->GetSpellProto()->eff[0].EffectBasePoints ) / 100;
			a = caster->HasAuraWithNameHash( SPELL_HASH_SPIRITUAL_HEALING, 0, AURA_SEARCH_PASSIVE );
			if( a )	//Spiritual Healing
				mod->m_pct_mod = mod->m_pct_mod * ( 100 + a->GetSpellProto()->eff[0].EffectBasePoints ) / 100;
		}

		val = val * mod->m_pct_mod / 100;

		//frikkin temp hackfix to stop people reporting broken shields 
		//somehow we should calc effect before sending the aura
		mod->m_amount = val;
		m_target->ModVisualAuraStackCount( this, 0 );

		ab = new Absorb;
		ab->OwnerAur = this;
		ab->amt = val;
		ab->spellid = GetSpellId();
		ab->caster = m_casterGuid;
//		ab->MaxAbsorbEvents = 0x00FFFFFF;
		
		//maybe later other spells will use it
/*		if( GetSpellProto()->NameHash == SPELL_HASH_INTERVENE ) //script aura interrupt so that only direct attack removes it not dots
		{
			ab->MaxAbsorbEvents = 1;
		}
		else */
		if( GetSpellProto()->NameHash == SPELL_HASH_ANTI_MAGIC_ZONE )
		{
			ab->MaxAbsorbCoeff = 0.75f;
		}
		else if( GetSpellProto()->NameHash == SPELL_HASH_ANTI_MAGIC_SHELL )
		{
			ab->MaxAbsorbCoeff = 0.75f;
			if( caster )
			{
				Aura *a = caster->HasAuraWithNameHash( SPELL_HASH_MAGIC_SUPPRESSION, 0, AURA_SEARCH_PASSIVE );
				if( a )
					ab->MaxAbsorbCoeff += a->m_modList[0].m_amount/ 100.0f;	//crossing fingers this will not have more then 1 effect in the future
			}
		}
		//maybe later other spells will use it
		else if( GetSpellProto()->NameHash == SPELL_HASH_THE_GENERAL_S_HEART )
			ab->MaxAbsorbPerHit = 205;
		else if( GetSpellProto()->NameHash == SPELL_HASH_CHEATING_DEATH ) //Cheat Death proc
		{
			ab->MaxAbsorbCoeff = val / 100.0f;
			ab->amt = 0x0FFFFFFF;	//any amount
		}

		//future bug removal
		if( ab->MaxAbsorbCoeff > 1.0f )
			ab->MaxAbsorbCoeff = 1.0f; //full absorb as long as we can absorb

		for(uint32 x=0;x<SCHOOL_COUNT;x++)
			if (mod->m_miscValue & (((uint32)1)<<x) )
				ab->schools[x]=true;
			else
				ab->schools[x]=false;
		m_target->Absorbs.push_front(ab);
		//store this custome structure to make sure we delete the right one on aura remove
		temp_custom_structure_holder = ab;
	}
	else
	{
		m_target->Absorbs.SafeRemove( (Absorb*)temp_custom_structure_holder, temp_custom_structure_holder != NULL );
	}
}

void Aura::SpellAuraModSpellCritChanceSchool(bool apply)
{
	if(apply)
	{
		for(uint32 x=0;x<SCHOOL_COUNT;x++)
			if (mod->m_miscValue & (((uint32)1)<<x))
				m_target->SpellCritChanceSchool[x] += mod->m_amount;
		if(mod->m_amount < 0)
			SetNegative();
		else
			SetPositive();
	}
	else
	{
		for(uint32 x=0;x<SCHOOL_COUNT;x++)
		{
			if (mod->m_miscValue & (((uint32)1)<<x) )
			{
				m_target->SpellCritChanceSchool[x] -= mod->m_amount;
				/*if(m_target->SpellCritChanceSchool[x] < 0)
					m_target->SpellCritChanceSchool[x] = 0;*/
			}
		}
	}
	if(m_target->IsPlayer())
		SafePlayerCast( m_target )->UpdateChanceFields();
}

void Aura::SpellAuraModPowerCost(bool apply)
{
	int32 val = (apply) ? mod->m_amount : -mod->m_amount;
	if (apply)
	{
		if(val > 0)
			SetNegative();
		else
			SetPositive();
	}
	for(uint32 x=0;x<SCHOOL_COUNT;x++)
		if (mod->m_miscValue & (((uint32)1)<<x) )
			m_target->ModFloatValue(UNIT_FIELD_POWER_COST_MULTIPLIER+x,val/100.0f);
}

void Aura::SpellAuraModPowerCostSchool(bool apply)
{
	if(apply)
	{
		for(uint32 x=1;x<SCHOOL_COUNT;x++)
			if (mod->m_miscValue & (((uint32)1)<<x) )
				m_target->PowerCostMod[x] += mod->m_amount;
	}
	else
	{
		for(uint32 x=1;x<SCHOOL_COUNT;x++)
		{
			if (mod->m_miscValue & (((uint32)1)<<x) )
			{
				m_target->PowerCostMod[x] -= mod->m_amount;
			}
		}
	}
}

void Aura::SpellAuraReflectSpellsSchool(bool apply)
{
/*	if(apply)
	{
		CommitPointerListNode<struct ReflectSpellSchool> *itr;
		m_target->m_reflectSpellSchool.BeginLoop();
		for(itr = m_target->m_reflectSpellSchool.begin(); itr != m_target->m_reflectSpellSchool.end();itr = itr->Next() )
			if(GetSpellProto()->Id == itr->data->spellId)
				m_target->m_reflectSpellSchool.SafeRemove(itr,1);
		m_target->m_reflectSpellSchool.EndLoopAndCommit();

		ReflectSpellSchool *rss = new ReflectSpellSchool;

		rss->chance = mod->m_amount;
		rss->spellId = GetSpellId();
		rss->require_aura_hash = 0;
		if(m_spellProto->Attributes == 0x400D0 && m_spellProto->AttributesEx == 0)
		{
			rss->school = (int)(log10((float)mod->m_miscValue) / log10((float)2));
		}
		else
			rss->school = m_spellProto->School;

		m_target->m_reflectSpellSchool.push_front(rss);
	}
	else
	{
		CommitPointerListNode<struct ReflectSpellSchool> *itr;
		m_target->m_reflectSpellSchool.BeginLoop();
		for(itr = m_target->m_reflectSpellSchool.begin(); itr != m_target->m_reflectSpellSchool.end();itr = itr->Next() )
			if(GetSpellProto()->Id == itr->data->spellId)
				m_target->m_reflectSpellSchool.SafeRemove(itr,1);
		m_target->m_reflectSpellSchool.EndLoopAndCommit();
	} */
	int32 School;
	if(apply)
	{
		if(m_spellProto->Attributes == 0x400D0 && m_spellProto->AttributesEx == 0)
			School = (int)(log10((float)mod->m_miscValue) / log10((float)2));
		else
			School = m_spellProto->School;
	}
	else
		School = 0;
	m_target->ModSpellReflectList( GetSpellId(), mod->m_amount, School, GetSpellProto()->procCharges, apply );
}

void Aura::SpellAuraModLanguage(bool apply)
{
	if(apply)
		m_target->m_modlanguage = mod->m_miscValue;
	else
		m_target->m_modlanguage = -1;
}

void Aura::SpellAuraAddFarSight(bool apply)
{
	if(apply)
	{
		if(m_target->GetTypeId() != TYPEID_PLAYER)
		{ 
			return;
		}

		//FIXME:grep aka Nublex will fix this
		//Make update circle bigger here
	}
	else
	{
		//Destroy new updated objects here if they are still out of update range
		//w/e
	}
}

void Aura::SpellAuraMechanicImmunity(bool apply)
{
//sLog.outString( "Aura::SpellAuraMechanicImmunity begun." );
	if(apply)
	{
		//sLog.outString( "mod->m_miscValue = %u" , (uint32) mod->m_miscValue );
		//sLog.outString( "Incrementing MechanicsDispels (current value: %u, new val: %u)" , m_target->MechanicsDispels[mod->m_miscValue] , m_target->MechanicsDispels[mod->m_miscValue] + 1 );
		assert(mod->m_miscValue < MECHANIC_TOTAL);
		m_target->MechanicsDispels[mod->m_miscValue]++;

		if(mod->m_miscValue != MECHANIC_HEALING && mod->m_miscValue != MECHANIC_INVULNERABLE && mod->m_miscValue != MECHANIC_SHIELDED) // dont remove bandages, Power Word and protection effect
		{
			// Supa's test run of Unit::RemoveAllAurasByMechanic 
			if( m_target ) 
			{	// just to be sure?
				m_target->RemoveAllAurasByMechanic( (uint32)mod->m_miscValue , (uint32)(-1) , false );

				if( mod->m_miscValue == MECHANIC_ROOTED && m_spellProto->NameHash == SPELL_HASH_BLINK )
				{
//					m_target->RemoveAllAurasByMechanicMulti( (1<<MECHANIC_STUNNED)|(1<<MECHANIC_INCAPACIPATED) , (uint32)(-1) , false );
					//already executed this in effectcalcoverride
//					m_target->RemoveAllAurasByMechanicMulti( (1<<MECHANIC_ROOTED)|(1<<MECHANIC_STUNNED) , (uint32)(-1) , false ); //should not remove Gouge, Ice trap, sap, fear, Repentance, Ring of frost....
#if 0
					for(uint32 x= MAX_POSITIVE_AURAS; x < MAX_NEGATIVE_AURAS1(m_target); ++x)
						if(m_target->m_auras[x])
							for(uint32 y = 0; y < MAX_SPELL_EFFECT_COUNT; ++y)
							{
/*								switch( m_target->m_auras[x]->GetSpellProto()->eff[y].EffectApplyAuraName )
								{
									case SPELL_AURA_MOD_STUN:
									case SPELL_AURA_MOD_ROOT:
										{
											m_target->m_auras[x]->Remove();
											y=100;	//removed aura, do not use it anymore !
										}
								}/**/
/*								if( m_target->m_auras[x]->GetSpellProto()->eff[y].EffectMechanic == MECHANIC_ROOTED || m_target->m_auras[x]->GetSpellProto()->eff[y].EffectMechanic == MECHANIC_STUNNED )
								{
									m_target->m_auras[x]->Remove();
									y=100;	//removed aura, do not use it anymore !
								}/**/
							}
#endif
				}
				else if( mod->m_miscValue == MECHANIC_ROOTED && m_spellProto->NameHash == SPELL_HASH_HAND_OF_FREEDOM && m_target->HasAuraWithNameHash( SPELL_HASH_DIVINE_PURPOSE, 0, AURA_SEARCH_PASSIVE ) )
				{
					//only with Divine Purpose
					m_target->RemoveAllAurasByMechanicMulti( (1<<MECHANIC_STUNNED)|(1<<MECHANIC_SAPPED) , (uint32)(-1) , false );
				}
				else if( m_spellProto->NameHash == SPELL_HASH_PVP_TRINKET || m_spellProto->NameHash == SPELL_HASH_EVERY_MAN_FOR_HIMSELF )
				{
					m_target->RemoveAurasMovementImpairing();
					m_target->RemoveAllAurasByMechanicMulti( (1<<MECHANIC_STUNNED)|(1<<MECHANIC_SAPPED) , (uint32)(-1) , false );
					// insignia of the A/H
					for(uint32 x= MAX_POSITIVE_AURAS; x < MAX_NEGATIVE_AURAS1(m_target); ++x)
						if(m_target->m_auras[x])
							for(uint32 y = 0; y < 3; ++y)
								switch(m_target->m_auras[x]->GetSpellProto()->eff[y].EffectApplyAuraName)
								{
									case SPELL_AURA_MOD_STUN:
									case SPELL_AURA_MOD_ROOT:
									case SPELL_AURA_MOD_CONFUSE:
									case SPELL_AURA_MOD_FEAR:
									case SPELL_AURA_MOD_DECREASE_SPEED:
									case SPELL_AURA_MOD_SILENCE:
									case SPELL_AURA_MOD_DISARM:
									case SPELL_AURA_MOD_DISARM_OFFHAND:
									case SPELL_AURA_MOD_DISARM_RANGED:
										m_target->m_auras[x]->Remove();
										y=100;	//removed aura, do not use it anymore !
								}
				}
			}
		}
		else
			SetNegative();
	}
	else if( m_target->MechanicsDispels[mod->m_miscValue] > 0 )
		m_target->MechanicsDispels[mod->m_miscValue]--;
}

void Aura::SpellAuraMounted(bool apply)
{
	Player *p_target;
	if( m_target->IsPlayer() )
		p_target = SafePlayerCast( m_target );
	else
		p_target = NULL;
	if(!p_target) 
	{ 
		return;
	}

	/*Shady: Is it necessary? Stealth should be broken since all spells with Mounted SpellEffect don't have "does not break stealth" flag (except internal Video mount spell). 
	So commented, cause we don't need useless checks and hackfixes*/
	/* if(m_target->IsStealth())
	{
		uint32 id = m_target->m_stealth;
		m_target->m_stealth = 0;
		m_target->RemoveAura(id);
	}*/

	if(apply)
	{
		SetPositive();

		//p_target->AdvanceSkillLine(762); // advance riding skill

		if(p_target->m_bg)
		{
			p_target->m_bg->HookOnMount(p_target);
			p_target->m_bg->HookOnFlagDrop(p_target);
		}

		if(p_target->m_MountSpellId)
			m_target->RemoveAura(p_target->m_MountSpellId);

		m_target->RemoveAurasByInterruptFlag(AURA_INTERRUPT_ON_MOUNT);

		p_target->m_MountSpellId = m_spellProto->Id;

		if( p_target->GetShapeShift() 
			&& p_target->GetShapeShift() != FORM_SHADOW
			&& p_target->GetShapeShift() != FORM_BATTLESTANCE && p_target->GetShapeShift() != FORM_DEFENSIVESTANCE && p_target->GetShapeShift() != FORM_BERSERKERSTANCE && p_target->m_ShapeShifted != m_spellProto->Id )
			p_target->RemoveAura( p_target->m_ShapeShifted );

		p_target->flying_aura = 0;

		//some mount auras do not have a display. Example : running wild
		uint32 displayId = 0;
		if( mod->m_miscValue )
		{
			//check if this mount is a vehicle. If it is then we spawn one and mount the player in it
			CreatureProto* cp = CreatureProtoStorage.LookupEntry(mod->m_miscValue);
			if( cp && cp->vehicleId )
				p_target->ConvertToVehicle( cp, false, true );
			if( p_target->mount_look_override == 0 )
			{
				//hackfix for invincible - non flying zones
	//			if( (GetSpellProto()->NameHash == SPELL_HASH_INVINCIBLE || GetSpellProto()->NameHash == SPELL_HASH_CELESTIAL_STEED )
	//				CAN_USE_FLYINGMOUNT( m_target->GetMapMgr() ) == 0 )
	//				mod->m_miscValue = GetSpellProto()->EffectMiscValue[ 1 ];	//this is plain wrong !

				CreatureInfo* ci = CreatureNameStorage.LookupEntry(mod->m_miscValue);
				if(!ci) 
				{ 
					return;
				}
				displayId = ci->Male_DisplayID;
				//darkmoon pony
				if( ci->Female_DisplayID && p_target && p_target->getGender() )
					displayId = ci->Female_DisplayID;
			}
			else 
				displayId = p_target->mount_look_override;
		}
		if( displayId )
			m_target->SetUInt32Value( UNIT_FIELD_MOUNTDISPLAYID , displayId);

		mod->fixed_amount[0] = 0;
		{
			uint32 spellId = 0;
			// find the spell we need
			MountTypeEntry *type = dbcMountTypeStore.LookupEntry( GetSpellProto()->eff[ mod->i ].EffectMiscValueB );
			if(!type)
				return;
	            
			uint32 plrskill = p_target->_GetSkillLineCurrent( SKILL_RIDING );
			uint32 map = p_target->GetMapId();
			uint32 maxSkill = 0;
			for(int i = 0; i < MAX_MOUNT_TYPE_COLUMN; i++)
			{
				MountCapabilityEntry *cap = dbcMountCapabilityStore.LookupEntry( type->capabilities[i] );
				if(!cap)
					continue;
				if( cap->map != -1 && cap->map != map )
					continue;
				if( cap->reqSkillLevel > plrskill || cap->reqSkillLevel <= maxSkill )
					continue;
				if( cap->reqSpell && !p_target->HasSpell(cap->reqSpell) )
					continue;
				maxSkill = cap->reqSkillLevel;
				spellId = cap->spell;
			}
			if(spellId != 0)
			{
	           p_target->CastSpell( p_target, spellId, true );	
			   mod->fixed_amount[0] = spellId;
			}
		}
		//let's find out the speed with which we should run 
/*		{
			uint32 skill_value = p_target->_GetSkillLineCurrent( SKILL_RIDING, true );
			uint32 riding_bonus = MAX( 150, MIN( skill_value, 310 ) );

			mod->m_amount = riding_bonus / 3;

			SpellAuraModIncreaseSpeed( apply );
			SpellAuraModIncreaseMountedSpeed( apply );
			SpellAuraIncreaseSwimSpeed(apply );

			mod->m_amount = riding_bonus ;
			//this is some hackshit
			if( ( GetSpellProto()->c_is_flags2 & SPELL_FLAG2_IS_FLY_MOUNT )
				&& p_target->GetMapMgr() && CAN_USE_FLYINGMOUNT(  p_target->GetMapMgr() )
	//			&& ( p_target->GetMapMgr()->GetMapId() == 530 || p_target->HasSpellwithNameHash( SPELL_HASH_FLIGHT_MASTER_S_LICENSE ) )
				&& ( p_target->GetMapMgr()->GetMapId() == 530 || p_target->HasSpell( 90267 ) ) )
			{
				//set fly capability + swim speed
				SpellAuraEnableFlight(apply);
			}
		} */

		//m_target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_MOUNTED_TAXI);

/*
		// Zack : disabled due to exploits
		// 1) summon was not always the same
		// 2) people were able to heal pet very easely
		//desummon summons
		if( p_target->GetSummon() != NULL )
		{
			p_target->GetSummon()->Remove( false, true, false ); // just remove for later re-call
			mod->fixed_amount[mod->i] = 1;
		}*/
	}
	else
	{
		p_target->m_MountSpellId = 0;

		{
//			if( p_target->flying_aura == GetSpellProto()->Id )
//			{
//				SpellAuraEnableFlight(apply);
//			}
		}
		p_target->flying_aura = 0;
		m_target->SetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID, 0);
		//m_target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_MOUNTED_TAXI);
			
		//if we had pet then respawn
//		if( mod->fixed_amount[mod->i]==1 )
//			p_target->SpawnActivePet();// get pet back
		p_target->RemoveAurasByInterruptFlag( AURA_INTERRUPT_ON_DISMOUNT );
		p_target->DestoryVehicle();	//only does it if we were owner. But we were trust me !

		{
			//this is the mount speed spell
			if( mod->fixed_amount[0] )
				p_target->RemoveAura( mod->fixed_amount[0] );
		}
/*		{
			mod->m_amount = mod->m_amount / 3;	//this is not a bug, we use Nx flight speed compared to ground speed
			SpellAuraModIncreaseSpeed( apply );
			SpellAuraModIncreaseMountedSpeed( apply );
			SpellAuraIncreaseSwimSpeed(apply );
		}*/
	}
	//does not seem to have any effect. This spell is zomming out the camera 60002
/*	if( p_target->GetSession() )
	{
		uint64 guid = p_target->GetGUID();
		uint8 *guid_bytes = (uint8*)&guid;
		sStackWorldPacket(data,SMSG_FORCE_CAMERA_FOLLOW_DISTANCE, 50);
		data << uint16( 0xC08F );
		data << ObfuscateByte( guid_bytes[1] );   
		data << ObfuscateByte( guid_bytes[3] );   
		data << ObfuscateByte( guid_bytes[2] );   
		data << ObfuscateByte( guid_bytes[0] );   
		data << uint32( p_target->m_PositionUpdateCounter++ );
//		data << float( apply ? 6.0f : 2.23f );
		data << float( 2.23f );
		data << ObfuscateByte( guid_bytes[6] );   
		data << ObfuscateByte( guid_bytes[7] );   
		p_target->GetSession()->SendPacket( &data );
	} */
}

void Aura::SpellAuraModDamagePercDone(bool apply)
{
/*	switch (GetSpellId()) //dirty or mb not fix bug with wand specializations
	{
	case 14524:
	case 14525:
	case 14526:
	case 14527:
	case 14528:
		return;
	} */
	//-100 makes you do no dmg at all
	if( mod->m_amount < -100 )
		mod->m_amount = -100;
/*	float val = MAX( 0.0001f, 1.0f + ((float)(mod->m_amount))/100.0f ); //postive increases dmg, negative decreases dmg
	if( apply == false )
		val = 1.0f / val; */
	if(m_target->IsPlayer())
	{
		float val;
		if( apply )
			val = (float)mod->m_amount / 100.0f;
		else
			val = -(float)mod->m_amount / 100.0f;
		if(GetSpellProto()->GetEquippedItemClass()==-1)//does not depend on weapon
		{
			for(uint32 i=0;i<SCHOOL_COUNT;i++)
				if (mod->m_miscValue & (((uint32)1)<<i) )
				{
					float oldnewval = m_target->GetFloatValue(PLAYER_FIELD_MOD_DAMAGE_DONE_PCT + i);
//					oldnewval *= val;
					oldnewval += val;
					m_target->SetFloatValue(PLAYER_FIELD_MOD_DAMAGE_DONE_PCT + i, oldnewval); // display to client (things that are weapon dependant dont get displayed)
				}
		}
		//do not double apply SCHOOL_NORMAL scale
		else if(mod->m_miscValue & 1)
		{
			Player *p_target;
			if( m_target->IsPlayer() )
				p_target = SafePlayerCast( m_target );
			else
				p_target = NULL;
			if(apply)
			{
				WeaponModifier *md = new WeaponModifier;
				md->value = val;
				md->wclass = GetSpellProto()->GetEquippedItemClass();
				md->subclass = GetSpellProto()->GetEquippedItemSubClass();
				md->spellentry = GetSpellProto();
				p_target->damagedone.push_front( md );
			}
			else
			{
				SimplePointerListNode<WeaponModifier> *i;
				for( i=p_target->damagedone.begin();i!=p_target->damagedone.end();i = i->Next() )
					if( i->data->spellentry == GetSpellProto() )
					{
						p_target->damagedone.remove( i, 1 );
						break;
					}
			}
		}
	}
	else
	{
		int32 val;
		if( apply )
			val = (float)mod->m_amount;
		else
			val = -(float)mod->m_amount;
		for(uint32 i=0;i<SCHOOL_COUNT;i++)
			if (mod->m_miscValue & (((uint32)1)<<i) )
//				SafeCreatureCast(m_target)->ModDamageDonePct[i] *= val;
				SafeCreatureCast(m_target)->ModDamageDonePct[i] += val;
	}
	m_target->CalcDamage();
}

void Aura::SpellAuraModPercStat(bool apply)
{
	int32 val;
	if(apply)
	{
		val = mod->m_amount;
		if(val<0)
			SetNegative();
		else
			SetPositive();
	}
	else
		val= -mod->m_amount;

	if (mod->m_miscValue == -1)//all stats
	{
		if(m_target->IsPlayer())
		{
			for(uint32 x=0;x<5;x++)
			{
				if(mod->m_amount>0)
					SafePlayerCast( m_target )->StatModPctPos[x] += val;
				else
					SafePlayerCast( m_target )->StatModPctNeg[x] -= val;

				SafePlayerCast( m_target )->CalcStat(x);
			}

			SafePlayerCast( m_target )->UpdateStats();
			SafePlayerCast( m_target )->UpdateChances();
		}
		else if( m_target->IsCreature() )
		{
			for(uint32 x=0;x<5;x++)
			{
				SafeCreatureCast(m_target)->StatModPct[x] += val;
				SafeCreatureCast(m_target)->CalcStat(x);
			}
		}
	}
	else
	{
		ASSERT(mod->m_miscValue < 5);
		if(m_target->IsPlayer())
		{
			if(mod->m_amount>0)
				SafePlayerCast( m_target )->StatModPctPos[mod->m_miscValue] += val;
			else
				SafePlayerCast( m_target )->StatModPctNeg[mod->m_miscValue] -= val;

			SafePlayerCast( m_target )->CalcStat(mod->m_miscValue);

			SafePlayerCast( m_target )->UpdateStats();
			SafePlayerCast( m_target )->UpdateChances();
		}
		else if( m_target->IsCreature() )
		{
			SafeCreatureCast(m_target)->StatModPct[mod->m_miscValue]+=val;
			SafeCreatureCast(m_target)->CalcStat(mod->m_miscValue);
		}
	}
}

void Aura::SpellAuraSplitDamage(bool apply)
{
	Unit * caster;

	if( !m_target || !m_target->IsUnit() )
	{ 
		return;
	}

	caster = SafeUnitCast( GetCaster() );
	if(!caster)
	{ 
		return;
	}

	if( GetSpellProto()->c_is_flags & SPELL_FLAG_IS_DMGSPLIT_CASTER_TARGET )
	{
		if( caster->m_damageSplitTarget == NULL )
			caster->m_damageSplitTarget = new DamageSplitTarget;

		if(apply)
		{
			caster->m_damageSplitTarget->m_flatDamageSplit = 0;
			caster->m_damageSplitTarget->m_spellId = GetSpellProto()->Id;
	//		caster->m_damageSplitTarget->m_pctDamageSplit = mod->m_miscValue / 100.0f;	//wtf, this is school mask not value
			caster->m_damageSplitTarget->m_pctDamageSplit = mod->m_amount / 100.0f;
//			caster->m_damageSplitTarget->damage_type = mod->m_AuraName;	//are you sure ? this is not used and bullshit ?
			caster->m_damageSplitTarget->creator = (void*)this;
			caster->m_damageSplitTarget->m_target = m_target->GetGUID();
		}
		else if( caster->m_damageSplitTarget )
		{
			delete caster->m_damageSplitTarget;
			caster->m_damageSplitTarget = NULL;
		}
	}

	if( ( GetSpellProto()->c_is_flags & SPELL_FLAG_IS_DMGSPLIT_TARGET_CASTER ) || ( GetSpellProto()->c_is_flags & ( SPELL_FLAG_IS_DMGSPLIT_TARGET_CASTER | SPELL_FLAG_IS_DMGSPLIT_CASTER_TARGET ) ) == 0 )
	{
		if( m_target->m_damageSplitTarget == NULL )
			m_target->m_damageSplitTarget = new DamageSplitTarget;

		if(apply)
		{
			m_target->m_damageSplitTarget->m_flatDamageSplit = 0;
			m_target->m_damageSplitTarget->m_spellId = GetSpellProto()->Id;
	//		m_target->m_damageSplitTarget->m_pctDamageSplit = mod->m_miscValue / 100.0f;	//wtf, this is school mask not value
			m_target->m_damageSplitTarget->m_pctDamageSplit = mod->m_amount / 100.0f;
//			m_target->m_damageSplitTarget->damage_type = mod->m_AuraName;
			m_target->m_damageSplitTarget->creator = (void*)this;
			m_target->m_damageSplitTarget->m_target = caster->GetGUID();
		}
		else if( m_target->m_damageSplitTarget )
		{
			delete m_target->m_damageSplitTarget;
			m_target->m_damageSplitTarget = NULL;
		}
	}
}

void Aura::SpellAuraModRegen(bool apply)
{
	if(apply)//seems like only positive
	{
		SetPositive ();
		sEventMgr.AddEvent(this, &Aura::EventPeriodicHealNoBonus,(uint32)((this->GetSpellProto()->eff[mod->i].EffectBasePoints+1)/5)*3,
			EVENT_AURA_PERIODIC_REGEN,3000,0,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
	}
}

void Aura::SpellAuraPeriodicDummy(bool apply)
{
	if( apply )
	{
		SetPositive();
		//hasting affects ticks too
		Unit *Unitc = GetUnitCaster();
		int32 period = GetSpellAmplitude( GetSpellProto(), Unitc, mod->i, m_castedItemId );
		mod->fixed_amount[0] = 0;	//just in case we need to count something :P
		uint32 Flags = EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT;
		if( GetSpellProto()->c_is_flags2 & SPELL_FLAG2_IS_TICKING_IMEDIATLY )
			Flags |= EVENT_FLAG_TICK_ON_NEXT_UPDATE;
		sEventMgr.AddEvent(this, &Aura::SpellAuraPeriodicDummyTrigger,true, (uint32)mod->list_ind, EVENT_PERIODIC_DUMMY_AURA_TRIGGER, period , 0, Flags);
	}
	else
	{
		SpellAuraPeriodicDummyTrigger( false, mod->i );
	}
}

void Aura::SpellAuraPeriodicDummyTrigger(bool apply, uint32 mod_i)
{
	if( m_spellProto->AuraPeriodicDummyTickHook 
		&& m_spellProto->AuraPeriodicDummyTickHook( mod_i, this, apply ) == true ) //default should break here
		return;

	switch( m_spellProto->NameHash )
	{
/*		
		case SPELL_HASH_ASPECT_OF_THE_VIPER:
			if( p_target )
				EventPeriodicEnergizeVariable((uint32)m_modList[mod_i].m_amount,(uint32)m_modList[mod_i].m_miscValue);
		break;
		*/
		case SPELL_HASH_LEATHER_SPECIALIZATION:
		case SPELL_HASH_MAIL_SPECIALIZATION:
		case SPELL_HASH_PLATE_SPECIALIZATION:
			{
				Player *p_target;
				if( m_target->IsPlayer() )
					p_target = SafePlayerCast( m_target );
				else
					p_target = NULL;
				//we need to wear full leather
				if( p_target )
				{
					bool condition_met = apply;
					uint32 stat_type = m_modList[ mod_i ].m_miscValue;
					if( stat_type > STAT_COUNT )
					{
						sLog.outDebug("Error, armor specialization is trying to set invalid stat type %u",stat_type);
						return;
					}
					uint32 slots_to_check[9] = { EQUIPMENT_SLOT_HEAD, EQUIPMENT_SLOT_SHOULDERS, EQUIPMENT_SLOT_CHEST, EQUIPMENT_SLOT_WAIST, EQUIPMENT_SLOT_LEGS, EQUIPMENT_SLOT_FEET, EQUIPMENT_SLOT_WRISTS, EQUIPMENT_SLOT_HANDS, 255 };
					uint32 req_subclass = GetSpellProto()->eff[ m_modList[ mod_i ].i ].EffectMiscValueB;
					for(uint32 i=0;slots_to_check[i]!=255;i++)
					{
						Item *it = p_target->GetItemInterface()->GetInventoryItem( slots_to_check[i] );
						if( it && it->GetProto()->SubClass != req_subclass )
						{
							condition_met = false;
							break;
						}
					}
					if( condition_met == true && m_modList[ mod_i ].fixed_amount[0] == 0 )
					{
						m_modList[ mod_i ].fixed_amount[0] = 1;
//						p_target->StatModPctPos[ stat_type ] += m_modList[ mod_i ].m_amount;
						p_target->TotalStatModPctPos[ stat_type ] += m_modList[ mod_i ].m_amount;
						p_target->CalcStat( stat_type );
						p_target->UpdateStats();
					}
					else if( condition_met == false && m_modList[ mod_i ].fixed_amount[0] != 0 )
					{
						m_modList[ mod_i ].fixed_amount[0] = 0;
//						p_target->StatModPctPos[ stat_type ] -= m_modList[ mod_i ].m_amount;
						p_target->TotalStatModPctPos[ stat_type ] -= m_modList[ mod_i ].m_amount;
						p_target->CalcStat( stat_type );
						p_target->UpdateStats();
					}
				}
			}break;
		case SPELL_HASH_LUNAR_SHOWER:
		{
			if( m_target->IsPlayer() && apply )
			{
				Player *p = SafePlayerCast(m_target);
				//we need to move to maintain this buff
				if( p->last_moved + 3000 < getMSTime() )
					return;
				//imba, this buff gets a stack if we move
				p->CastSpell( p, m_spellProto->Id, true );
			}
		}break;
		case SPELL_HASH_ENLIGHTENED_JUDGEMENTS:
		{
			Player *p_target;
			if( m_target->IsPlayer() )
				p_target = SafePlayerCast( m_target );
			else
				p_target = NULL;
			if( p_target )
			{
				//remove old value
				p_target->ModRating( PLAYER_RATING_MODIFIER_MELEE_HIT, -m_modList[ mod_i ].fixed_amount[0] );
				p_target->ModRating( PLAYER_RATING_MODIFIER_RANGED_HIT, -m_modList[ mod_i ].fixed_amount[0] );
				p_target->ModRating( PLAYER_RATING_MODIFIER_SPELL_HIT, -m_modList[ mod_i ].fixed_amount[0] );			
				//add new value
				if( apply )
				{
					m_modList[ mod_i ].fixed_amount[0] = m_target->GetStat( STAT_SPIRIT );
					p_target->ModRating( PLAYER_RATING_MODIFIER_MELEE_HIT, m_modList[ mod_i ].fixed_amount[0] );
					p_target->ModRating( PLAYER_RATING_MODIFIER_RANGED_HIT, m_modList[ mod_i ].fixed_amount[0] );
					p_target->ModRating( PLAYER_RATING_MODIFIER_SPELL_HIT, m_modList[ mod_i ].fixed_amount[0] );
				}
			}
		}break;
		case SPELL_HASH_MANA_BARRIER:	//boss fight spell
			{
				int32 missing_health = m_target->GetUInt32Value( UNIT_FIELD_MAXHEALTH ) - m_target->GetUInt32Value( UNIT_FIELD_HEALTH );
				int32 convert_mana_to_health;
				if( missing_health > (int32)m_target->GetPower( POWER_TYPE_MANA ) )
					convert_mana_to_health = (int32)m_target->GetPower( POWER_TYPE_MANA );
				else
					convert_mana_to_health = missing_health;
				m_target->ModUnsigned32Value( UNIT_FIELD_HEALTH, convert_mana_to_health );
				m_target->ModPower( POWER_TYPE_MANA, -convert_mana_to_health );
			}
			break;
		case SPELL_HASH_MIRRORED_SOUL:
			{
				//this is only executed on the boss, rest just suck the dmg up
				if( m_target->GetEntry() != 36502 )
					return;
				//is this first tick ?
				if( m_modList[ mod_i ].fixed_amount[0] == 0 )
					m_modList[ mod_i ].fixed_amount[0] = m_target->GetUInt32Value( UNIT_FIELD_HEALTH );
				int32 missing_health = m_target->GetUInt32Value( UNIT_FIELD_MAXHEALTH ) - m_target->GetUInt32Value( UNIT_FIELD_HEALTH );
				if( missing_health <= 0 )
					return;
				uint32 dealdamage = missing_health * 20 / 100;
				//search for the victim 
				m_target->AquireInrangeLock(); //make sure to release lock before exit function !
				InRangeSetRecProt::iterator itr;
				InrangeLoopExitAutoCallback AutoLock;
				for( itr = m_target->GetInRangeSetBegin( AutoLock ); itr != m_target->GetInRangeSetEnd(); itr++ )
				{
					if( (*itr) == m_target || !(*itr)->IsUnit() || !SafeUnitCast((*itr))->isAlive())
						continue;
					
					//yes, it is the same aura
					if( SafeUnitCast((*itr))->HasAuraWithNameHash( SPELL_HASH_MIRRORED_SOUL, 0, AURA_SEARCH_VISIBLE ) )
					{
						m_target->DealDamage( SafeUnitCast((*itr)), dealdamage, 69051 );
						//break; //- more then 1 target ?
					}
				}
				m_target->ReleaseInrangeLock();						
			}break;
/*		case SPELL_HASH_HEALING_RAIN:
		case SPELL_HASH_EFFLORESCENCE:
			{
				float r = 10.0f * 10.0f;
				InRangeSet targets;
				InRangeSet::iterator itr,itr2;
				m_target->AquireInrangeLock(); //make sure to release lock before exit function !
				for( itr2 = m_target->GetInRangeSetBegin(); itr2 != m_target->GetInRangeSetEnd();)
				{
					itr = itr2;
					itr2++; //maybe scripts can change list. Should use lock instead of this to prevent multiple changes. This protects to 1 deletion only
					if( !((*itr)->IsUnit()) || !SafeUnitCast( *itr )->isAlive() )
						continue;

					if( IsInrange( m_target->GetPositionX(), m_target->GetPositionY(), m_target->GetPositionZ(), (*itr), r ) && isFriendly( m_target, SafeUnitCast( *itr ) ) )
						targets.insert( (*itr) );
				}
				m_target->ReleaseInrangeLock();

				targets.insert( m_target );
				uint32 effectiveness = 100 / ( targets.size() / 6 + 1);	//aaargh, how to pass a PCT mod to the next spell ?
				SpellEntry *spInfo;
				if( m_spellProto->NameHash == SPELL_HASH_HEALING_RAIN )
				{
					spInfo = dbcSpell.LookupEntryForced( 73921 );
					SpellCastTargets tgt;
					for( itr = targets.begin(); itr != targets.end(); itr++)
					{
						Spell*sp=SpellPool.PooledNew( __FILE__, __LINE__ );
						sp->Init(m_target,spInfo,true,NULL);
						sp->forced_pct_mod[0] = effectiveness;
						tgt.m_unitTarget=(*itr)->GetGUID();
						sp->prepare(&tgt);
		//					m_target->CastSpell( m_target, 73921, true );
					}
				}
				else if( m_spellProto->NameHash == SPELL_HASH_EFFLORESCENCE )
				{
					spInfo = dbcSpell.LookupEntryForced( 81269 );
					int32 heal_amt = 0;
					Object *c = GetCaster();
					if( c && c->IsPlayer() )
					{
						int64 *p = c->GetCreateIn64Extension( SPELL_HASH_EFFLORESCENCE );
						heal_amt = (int32)*p;
					}
					if( heal_amt != 0 )
					{
						SpellCastTargets tgt;
						for( itr = targets.begin(); itr != targets.end(); itr++)
						{
							Spell*sp=SpellPool.PooledNew( __FILE__, __LINE__ );
							sp->Init(m_target,spInfo,true,NULL);
							sp->forced_basepoints[0] = heal_amt;
							tgt.m_unitTarget=(*itr)->GetGUID();
							sp->prepare(&tgt);
			//					m_target->CastSpell( m_target, 73921, true );
						}
					}
				}
			}
			break;*/
/*
		case SPELL_HASH_EARTHQUAKE: //earthquake
		{
			//there if a x% chance to knockback the targets
			if( apply )
			{
				Unit *caster = GetUnitCaster();
				if( caster && Rand( 10 ) )
					caster->CastSpell( target, 77505, true );
			}
		}break; */
		default:
			{
//				if( m_modList[mod_i].m_amount > 5 )
//					EventPeriodicDrink( uint32(float2int32(float(m_modList[mod_i].m_amount)/5.0f)));
				if( m_modList[mod_i].m_miscValue < POWER_TYPE_COUNT )
					EventPeriodicEnergizeVariable( m_modList[mod_i].m_amount, m_modList[mod_i].m_miscValue );
			}
	}
}

void Aura::EventPeriodicEnergizeVariable( uint32 amount, uint32 type )
{
	if( type > POWER_TYPE_COUNT )
	{
		return;
	}
	uint32 curEnergy = m_target->GetPower( type );
	uint32 maxEnergy = m_target->GetMaxPower( type );

	if( GetUnitCaster() != NULL )
		GetUnitCaster()->Energize( m_target, m_spellProto->Id, amount, type, 1 );
}

void Aura::EventPeriodicDrink(uint32 amount)
{
	uint32 v = m_target->GetPower( POWER_TYPE_MANA ) + amount;
	if( v > m_target->GetMaxPower( POWER_TYPE_MANA ) )
		v = m_target->GetMaxPower( POWER_TYPE_MANA );

	m_target->SetPower( POWER_TYPE_MANA, v);
//	m_target->SendSpellPeriodicAuraLog(m_target, m_target, m_spellProto->Id, m_spellProto->School, v, 0, 0, FLAG_PERIODIC_ENERGIZE, 0);
	m_target->Energize( m_target, m_spellProto->spell_id_client, amount, POWER_TYPE_MANA, 1 );
}

void Aura::EventPeriodicHealNoBonus(uint32 amount)
{
	if(!m_target->isAlive())
	{ 
		return;
	}

	uint32 ch = m_target->GetUInt32Value(UNIT_FIELD_HEALTH);
	ch+=amount;
	uint32 mh = m_target->GetUInt32Value(UNIT_FIELD_MAXHEALTH);

	if(ch>mh)
		m_target->SetUInt32Value(UNIT_FIELD_HEALTH,mh);
	else
		m_target->SetUInt32Value(UNIT_FIELD_HEALTH,ch);

	if(GetSpellProto()->AuraInterruptFlags & AURA_INTERRUPT_ON_STAND_UP)
	{
		m_target->Emote(EMOTE_ONESHOT_EAT);
	}
	else
	{
//		if(!(m_spellProto->BGR_one_buff_on_target & SPELL_TYPE_ARMOR))
//			SendPeriodicHealAuraLog( amount );
			m_target->SendCombatLogMultiple( m_target, amount, 0, 0, 0, GetSpellProto()->spell_id_client, GetSpellProto()->SchoolMask, COMBAT_LOG_PERIODIC_HEAL, 0, COMBAT_LOG_SUBFLAG_PERIODIC_HEAL );
	}

	m_target->RemoveAurasByHeal();
}

void Aura::SpellAuraModPowerRegen(bool apply)
{
	if(!mod->m_amount)
	{ 
		return;
	}

	if(apply)
	{
		if (mod->m_amount>0)
			SetPositive();
		else
			SetNegative();
		//because you can change power type with some auras
		if( mod->m_miscValue == m_target->GetPowerType() )
			mod->fixed_amount[0] = mod->m_amount;
		else
			mod->fixed_amount[0] = 0;
	}
	if ( m_target->IsPlayer() && mod->fixed_amount[0] )
	{
		int32 val = (apply) ? mod->fixed_amount[0]: -mod->fixed_amount[0];
#define CLIENT_MANA_REGEN_INTERVAL_SECONDS	5 //this is 3 for rage
		SafePlayerCast( m_target )->m_ModInterrMRegen += val / CLIENT_MANA_REGEN_INTERVAL_SECONDS;
		SafePlayerCast( m_target )->m_ModMPRegen += val / CLIENT_MANA_REGEN_INTERVAL_SECONDS;
		SafePlayerCast( m_target )->UpdatePowerRegen();
	}
}

void Aura::SpellAuraChannelDeathItem(bool apply)
{
	SetNegative(); //this should always be negative as npcs remove negative auras on death

	if(apply)
	{
		//dont need for now
	}
	else
	{
		if( m_target->IsCreature() || m_target->IsPlayer() )
		{
			if ( m_target->IsCreature() && SafeCreatureCast(m_target)->GetCreatureInfo() != NULL && SafeCreatureCast(m_target)->GetCreatureInfo()->Type == CRITTER )
			{ 
				return;
			}

			if(m_target->IsDead())
			{
				Player *pCaster = m_target->GetMapMgr()->GetPlayer( m_casterGuid );
				if(!pCaster)
				{ 
					return;
				}
				/*int32 delta=pCaster->getLevel()-m_target->getLevel();
				if(abs(delta)>5)
					return;*/

				uint32 itemid = GetSpellProto()->eff[mod->i].EffectItemType;

				ItemPrototype *proto = ItemPrototypeStorage.LookupEntry(itemid);
				if(pCaster->GetItemInterface()->CalculateFreeSlots(proto) > 0)
				{
					Item *item = objmgr.CreateItem(itemid,pCaster);
					if(!item) 
					{ 
						return;
					}


					item->SetUInt64Value(ITEM_FIELD_CREATOR,pCaster->GetGUID());
					if(!pCaster->GetItemInterface()->AddItemToFreeSlot(&item))
					{
						pCaster->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_INVENTORY_FULL);
						item->DeleteMe();
						return;
					}
					/*WorldPacket data(45);
					pCaster->GetSession()->BuildItemPushResult(&data, pCaster->GetGUID(), 1, 1, itemid ,0,0xFF,1,0xFFFFFFFF);
					pCaster->SendMessageToSet(&data, true);					*/
					SlotResult * lr = pCaster->GetItemInterface()->LastSearchResult();
					pCaster->GetSession()->SendItemPushResult(item,true,false,true,true,lr->ContainerSlot,lr->Slot,1);
				}
			}
		}
	}
}

void Aura::SpellAuraModDamagePercTaken(bool apply)
{
	if( apply )
	{
		if( mod->m_amount <= 0) 
			SetPositive();
		else
			SetNegative();
	}
	//-100 makes you dmg immune
	if( mod->m_amount < -100 )
		mod->m_amount = -100;
	//!! this is other way then in other places ! postive increases dmg taken, negative decreases dmg taken
/*	float val = MAX( 0.0001f, 1.0f + ((float)(mod->m_amount))/100.0f ); 
	if( apply == false )
		val = 1.0f / val;
	for(uint32 i=0;i<SCHOOL_COUNT;i++)
		if (mod->m_miscValue & (((uint32)1)<<i) )
			m_target->DamageTakenPctMod[i] *= val;	//reduce dmg not increase */
	int32 val;
	if( apply )
		val = +mod->m_amount; //value is negative here to reduce dmg
	else
		val = -mod->m_amount;
	for(uint32 i=0;i<SCHOOL_COUNT;i++)
		if (mod->m_miscValue & (((uint32)1)<<i) )
			m_target->DamageTakenPctMod[i] += val;	//reduce dmg not increase
}

void Aura::SpellAuraModRegenPercent(bool apply)
{
	if(apply)
		m_target->PctRegenModifier += mod->m_amount;
	else
		m_target->PctRegenModifier -= mod->m_amount;
}

void Aura::SpellAuraPeriodicDamagePercent(bool apply)
{
	if(apply)
	{
		//uint32 gr = GetSpellProto()->GetSpellGroupType();
		//if(gr)
		//{
		//	Unit*c=GetUnitCaster();
		//	if(c)
		//	{
		//		SM_FIValue(c->SM_FDOT,(int32*)&dmg,gr);
		//		SM_PIValue(c->SM_PDOT,(int32*)&dmg,gr);
		//	}
		//}

		/*if(m_spellProto->Id == 28347) //Dimensional Siphon
		{
			uint32 dmg = (m_target->GetUInt32Value(UNIT_FIELD_MAXHEALTH)*5)/100;
			sEventMgr.AddEvent(this, &Aura::EventPeriodicDamagePercent, dmg,
				EVENT_AURA_PERIODIC_DAMAGE_PERCENT, 1000, 0,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
		}
		else*/
		{
			uint32 dmg = mod->m_amount;

			//hasting affects ticks too
			Unit *Unitc = GetUnitCaster();
			int32 period = GetSpellAmplitude( GetSpellProto(), Unitc, mod->i, m_castedItemId );

			sEventMgr.AddEvent(this, &Aura::EventPeriodicDamagePercent, dmg, mod->i, mod->m_pct_mod, EVENT_AURA_PERIODIC_DAMAGE_PERCENT, period ,0,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
		}
		SetNegative();
	}
}

void Aura::EventPeriodicDamagePercent(uint32 amount, uint32 eff_index, int32 PCT )
{
	//DOT
	if(!m_target->isAlive())
	{ 
		return;
	}
	if( m_target->SchoolImmunityAntiEnemy[GetSpellProto()->School] || m_target->SchoolImmunityAntiFriend[GetSpellProto()->School] )
	{ 
		return;
	}

	Unit * c = GetUnitCaster();

	dealdamage tdmg;
	tdmg.base_dmg = amount * m_target->GetUInt32Value(UNIT_FIELD_MAXHEALTH)/100;
	tdmg.pct_mod_final_dmg = PCT;
	tdmg.DisableProc = (pSpellId!=0);
	tdmg.StaticDamage = true;
	tdmg.FirstTick = IsFirstTick();
	if(c)
		c->SpellNonMeleeDamageLog(m_target, GetSpellProto(), &tdmg, GetSpellProto()->quick_tickcount, eff_index);
	else
		m_target->SpellNonMeleeDamageLog(m_target, GetSpellProto(), &tdmg, GetSpellProto()->quick_tickcount, eff_index);
//	m_DamageDoneLastTick = tdmg.full_damage * 100 / ( tdmg.pct_mod_crit + 100 );
	m_flags |= FIRST_TICK_ENDED;
}

void Aura::SpellAuraModResistChance(bool apply)
{
	apply ? m_target->m_resistChance = mod->m_amount : m_target->m_resistChance = 0;
}

void Aura::SpellAuraModDetectRange(bool apply)
{
	Unit*m_caster=GetUnitCaster();
	if(!m_caster)
	{ 
		return;
	}
	if(apply)
	{
		SetNegative();
		m_caster->setDetectRangeMod(m_target->GetGUID(), mod->m_amount);
	}
	else
	{
		m_caster->unsetDetectRangeMod(m_target->GetGUID());
	}
}

void Aura::SpellAuraPreventsFleeing(bool apply)
{
	// Curse of Recklessness
}

void Aura::SpellAuraModUnattackable(bool apply)
{
/*
		Also known as Apply Aura: Mod Uninteractible
		Used by: Spirit of Redemption, Divine Intervention, Phase Shift, Flask of Petrification
		It uses one of the UNIT_FIELD_FLAGS, either UNIT_FLAG_NOT_SELECTABLE or UNIT_FLAG_NOT_ATTACKABLE_2
*/
}

void Aura::SpellAuraInterruptRegen(bool apply)
{
	if(apply)
		m_target->m_interruptRegen++;
	else
	{
		m_target->m_interruptRegen--;
		  if(m_target->m_interruptRegen < 0)
			m_target->m_interruptRegen = 0;
	}
}

void Aura::SpellAuraGhost(bool apply)
{
	if(m_target->IsPlayer())
	{
//		m_target->m_invisible = apply;

		if(apply)
		{
			m_target->m_invisFlag |= INVIS_FLAG_GHOST;
			SetNegative();
			SafePlayerCast( m_target )->SetMovement(MOVE_WATER_WALK);
		}
		else
		{
			m_target->m_invisFlag &= ~INVIS_FLAG_GHOST;
			SafePlayerCast( m_target )->SetMovement(MOVE_LAND_WALK);
		}
	}
}

void Aura::SpellAuraMagnet(bool apply)
{
	if(apply)
	{
		Unit *caster = GetUnitCaster();
		if (!caster)
		{ 
			return;
		}
		SetPositive();
		m_target->m_magnetcaster = caster->GetGUID();
		caster->m_magnetcharges = MAX( 1, mod->m_amount );	//we presume we can absorb always 1 
	}
	else
	{
		if( m_target )
			m_target->m_magnetcaster = 0;
		Unit *caster = GetUnitCaster();
		if( caster )
			caster->m_magnetcharges = 0;
	}
}

void Aura::SpellAuraManaShield(bool apply)
{
	if(apply)
	{
		SetPositive();
		m_target->m_manashieldamt = mod->m_amount ;
		m_target->m_manaShieldId = GetSpellId();
	}
	else
	{
		m_target->m_manashieldamt = 0;
		m_target->m_manaShieldId = 0;
	}
}

void Aura::SpellAuraSkillTalent(bool apply)
{
	if (m_target->IsPlayer())
	{
		if(apply)
		{
			SetPositive();
			SafePlayerCast( m_target )->_ModifySkillBonus(mod->m_miscValue,mod->m_amount);
		}
		else
			SafePlayerCast( m_target )->_ModifySkillBonus(mod->m_miscValue,-mod->m_amount);

		SafePlayerCast( m_target )->UpdateStats();
	}
}

void Aura::SpellAuraModAttackPower(bool apply)
{
	if(mod->m_amount<0)
	{
		SetNegative();
		if( apply )
			m_target->ModUnsigned32Value(UNIT_FIELD_ATTACK_POWER_MOD_NEG,mod->m_amount );
		else
			m_target->ModUnsigned32Value(UNIT_FIELD_ATTACK_POWER_MOD_NEG,-mod->m_amount );
	}
	else
	{
		SetPositive();
		if( apply )
			m_target->ModUnsigned32Value(UNIT_FIELD_ATTACK_POWER_MOD_POS,mod->m_amount );
		else
			m_target->ModUnsigned32Value(UNIT_FIELD_ATTACK_POWER_MOD_POS,-mod->m_amount );
	}

	m_target->CalcDamage();
}

void Aura::SpellAuraVisible(bool apply)
{
	//Show positive spells on target
	if(apply)
	{
		SetNegative();
	}
}

void Aura::SpellAuraModResistancePCT(bool apply)
{
	uint32 Flag = mod->m_miscValue;
	int32 amt;
	if(apply)
	{
		amt=mod->m_amount;
	 //   if(amt>0)SetPositive();
	   // else SetNegative();
	}
	else
		amt= -mod->m_amount;

	for(uint32 x=0;x<SCHOOL_COUNT;x++)
	{
		if(Flag & (((uint32)1)<< x))
		{
			if( m_target->IsPlayer() )
			{
				if(mod->m_amount>0)
				{
					SafePlayerCast( m_target )->ResistanceModPctPos[x] += amt;
				}
				else
				{
					SafePlayerCast( m_target )->ResistanceModPctNeg[x] -= amt;
				}
				SafePlayerCast( m_target )->CalcResistance(x);

			}
			else if( m_target->IsCreature())
			{
				SafeCreatureCast(m_target)->ResistanceModPct[x] += amt;
				SafeCreatureCast(m_target)->CalcResistance(x);
			}
		}
	}
}

void Aura::SpellAuraModCreatureAttackPower(bool apply)
{
	if( apply )
	{
		for( uint32 x = 0; x < CREATURE_TYPES; x++ )
			if( mod->m_miscValue & ( ( ( uint32 )1 ) << x ) )
				m_target->CreatureAttackPowerMod[x+1] += mod->m_amount;

		if( mod->m_amount > 0 )
			SetPositive();
		else
			SetNegative();
	}
	else
	{
		for( uint32 x = 0; x < CREATURE_TYPES; x++ )
		{
			if( mod->m_miscValue & ( ( ( uint32 )1 ) << x ) )
			{
				m_target->CreatureAttackPowerMod[x+1] -= mod->m_amount;
			}
		}
	}
	m_target->CalcDamage();
}

void Aura::SpellAuraModTotalThreat( bool apply )
{	
	if( apply )
	{
		if( mod->m_amount < 0 )
			SetPositive();
		else
			SetNegative();

		m_target->ModThreatModifyer( mod->m_amount );
	}
	else
		m_target->ModThreatModifyer(-(mod->m_amount));
}

void Aura::SpellAuraWaterWalk( bool apply )
{
	SetPositive();

	if( !m_target->IsPlayer() )
	{ 
		return;
	}
	if( apply )
		SafePlayerCast( m_target )->SetMovement(MOVE_WATER_WALK);
	else
		SafePlayerCast( m_target )->SetMovement(MOVE_LAND_WALK);
}

void Aura::SpellAuraFeatherFall( bool apply )
{
	SetPositive();

	if( !m_target->IsPlayer() )
	{ 
		return;
	}
	if( apply )
		SafePlayerCast( m_target )->SetMovement(MOVE_FEATHER_FALL);
	else
		SafePlayerCast( m_target )->SetMovement(MOVE_NORMAL_FALL);
}

void Aura::SpellAuraHover( bool apply )
{
	SetPositive();

	if( !m_target->IsPlayer() )
	{ 
		return;
	}
	if( apply )
		SafePlayerCast( m_target )->SetMovement(MOVE_HOVER);
	else
		SafePlayerCast( m_target )->SetMovement(MOVE_NO_HOVER);
}

void Aura::SpellAuraAddPctMod( bool apply )
{
	int32 val = apply ? mod->m_amount : -mod->m_amount;
	uint32 *AffectedGroups = GetSpellProto()->eff[mod->i].EffectSpellGroupRelation;

	if( m_target->SM_Mods == NULL )
		m_target->SM_Mods = new UnitSMMods;
//	sLog.outDebug("%s: AffectedGroups %I64x ,the smt type %u, val=%d",__FUNCTION__,AffectedGroups,mod->m_miscValue, val);
	switch( mod->m_miscValue )//let's generate warnings for unknown types of modifiers
	{
	case SMT_CRITICAL:
		SendModifierLog( &m_target->SM_Mods->SM_FCriticalChance, val, AffectedGroups, mod->m_miscValue, true );
		break;

	case SMT_DURATION:
		SendModifierLog( &m_target->SM_Mods->SM_PDur, val, AffectedGroups, mod->m_miscValue, true );
		break;

	case SMT_RADIUS:
		SendModifierLog( &m_target->SM_Mods->SM_PRadius, val, AffectedGroups, mod->m_miscValue, true );
		break;

	case SMT_RANGE:
		SendModifierLog( &m_target->SM_Mods->SM_PRange, val, AffectedGroups, mod->m_miscValue, true );
		break;

	case SMT_CAST_TIME:
		SendModifierLog( &m_target->SM_Mods->SM_PCastTime, val, AffectedGroups, mod->m_miscValue, true );
		break;

	case SMT_COST:
		SendModifierLog( &m_target->SM_Mods->SM_PCost, val, AffectedGroups, mod->m_miscValue, true );
		break;

	case SMT_CRITICAL_DAMAGE:
		SendModifierLog( &m_target->SM_Mods->SM_PCriticalDamage, val, AffectedGroups, mod->m_miscValue, true );
		break;

	case SMT_SPELL_VALUE_DOT:
		SendModifierLog( &m_target->SM_Mods->SM_PDOT, val, AffectedGroups, mod->m_miscValue, true );
		break;

	case SMT_NONINTERRUPT:
		SendModifierLog( &m_target->SM_Mods->SM_PNonInterrupt, val, AffectedGroups, mod->m_miscValue, true );
		break;

	case SMT_JUMP_REDUCE:
		SendModifierLog( &m_target->SM_Mods->SM_PJumpReduce, val, AffectedGroups, mod->m_miscValue, true );
		break;

	case SMT_EFFECT_BONUS:
		SendModifierLog( &m_target->SM_Mods->SM_PEffectBonus, val, AffectedGroups, mod->m_miscValue, true );
		break;
	case SMT_EFFECT_TARGET:
		SendModifierLog( &m_target->SM_Mods->SM_PEffectTargetBonus, val, AffectedGroups, mod->m_miscValue, true );
		break;

	case SMT_DAMAGE_DONE:
		SendModifierLog( &m_target->SM_Mods->SM_PDamageBonus, val, AffectedGroups, mod->m_miscValue, true );
		break;

	case SMT_MISC_EFFECT:
		SendModifierLog( &m_target->SM_Mods->SM_PMiscEffect, val, AffectedGroups, mod->m_miscValue, true );
		break;

	case SMT_PENALTY:
		SendModifierLog( &m_target->SM_Mods->SM_PPenalty, val, AffectedGroups, mod->m_miscValue, true );
		break;
	//TODO: disabled until clarified
	/*
	case SMT_ATTACK_POWER_BONUS:
		SendModifierLog(&m_target->SM_PAPBonus,val,AffectedGroups,mod->m_miscValue,true);
		break;
	*/
	case SMT_COOLDOWN_DECREASE:
		SendModifierLog( &m_target->SM_Mods->SM_PCooldownTime, val, AffectedGroups, mod->m_miscValue, true );
		break;
//	case SMT_GLOBAL_COOLDOWN: //in 4.3 client there are no PCT GCD auras
//		break;

	//there are 2 spells in 2.1.1 that will only need attack power bonus
	case SMT_ATTACK_POWER_AND_DMG_BONUS:
		{
			if( GetSpellProto()->NameHash == SPELL_HASH_SHADOWSTEP || GetSpellProto()->NameHash == SPELL_HASH_INCREASED_CRUSADER_STRIKE )
			{
				SendModifierLog( &m_target->SM_Mods->SM_PDamageBonus, val, AffectedGroups, mod->m_miscValue, true );
			}
			else
			{
				//these are seal of crusader spells
				SendModifierLog( &m_target->SM_Mods->SM_PDamageBonus, val, AffectedGroups, mod->m_miscValue, true );
				SendModifierLog( &m_target->SM_Mods->SM_PAPBonus, val, AffectedGroups, mod->m_miscValue, true );
			}
		}break;
	case SMT_CHARGES:
		SendModifierLog(&m_target->SM_Mods->SM_PCharges, val, AffectedGroups,mod->m_miscValue);
		break;
	case SMT_THREAT_REDUCED:
		SendModifierLog(&m_target->SM_Mods->SM_PThreat, val, AffectedGroups, mod->m_miscValue, true);
		break;
	//TODO:
	/*
	case SMT_TRIGGER:
	case SMT_TIME:
		break;
	*/
	//unknown Modifier type
	case SMT_RESIST_DISPEL:
		SendModifierLog( &m_target->SM_Mods->SM_PRezist_dispell, val, AffectedGroups, mod->m_miscValue, true );
		break;
	//!!!!!!!! these are custom
	case SMT_ADD_TO_EFFECT_VALUE_1:
		SendModifierLog(&m_target->SM_Mods->SM_PAddEffect1, val, AffectedGroups,SMT_MISC_EFFECT, true);
		break;
	case SMT_ADD_TO_EFFECT_VALUE_2:
		SendModifierLog(&m_target->SM_Mods->SM_PAddEffect2, val, AffectedGroups,SMT_MISC_EFFECT, true);
		break;
	case SMT_ADD_TO_EFFECT_VALUE_3:
		SendModifierLog(&m_target->SM_Mods->SM_PAddEffect3, val, AffectedGroups,SMT_MISC_EFFECT, true);
		break;
	case SMT_ADD_TO_EFFECT_VALUE_DUMMY:
		SendModifierLog(&m_target->SM_Mods->SM_PAddEffect_DUMMY, val, AffectedGroups,GetSpellProto()->eff[mod->i].EffectMiscValueB, true);
		break;
	default:
		sLog.outError( "Unknown spell modifier type %u in spell %u.<<--report this line to the developer\n", mod->m_miscValue, GetSpellId() );
		//don't add val, though we could formaly could do,but as we don't know what it is-> no sense
		break;
	}
}

void Aura::SendModifierLog(int32 **ObjectSMValues,int32 newval,uint32 *AffectsSMMask,uint8 type, uint8 pct )
{
	//WorldPacket data( SMSG_SET_FLAT_SPELL_MODIFIER + pct, 6 );
	packetSMSG_SET_FLAT_SPELL_MODIFIER data;
	data.unk1 = 1;
	data.counter = 1;
	data.type = type;

	if( *ObjectSMValues == 0 )
	{
		*ObjectSMValues = new int32[SPELL_GROUPS_BITS];
		memset( *ObjectSMValues, 0, sizeof( int32 ) * SPELL_GROUPS_BITS );
	}

	for( uint32 x = 0; x < 32; x++ )
	{
		uint32 tmask = 1 << x;
		for( uint32 i=0;i<3;i++)
			if( tmask & AffectsSMMask[i] )
			{
				(*ObjectSMValues)[x+i*32] += newval;
				if( !m_target->IsPlayer() )
					continue;

				data.group = x+i*32;
				data.vf = (float)(*ObjectSMValues)[x+i*32];

				if( pct )
					SafePlayerCast(m_target)->GetSession()->OutPacket( SMSG_SET_PCT_SPELL_MODIFIER, sizeof( packetSMSG_SET_FLAT_SPELL_MODIFIER ), &data );
				else
					SafePlayerCast(m_target)->GetSession()->OutPacket( SMSG_SET_FLAT_SPELL_MODIFIER, sizeof( packetSMSG_SET_FLAT_SPELL_MODIFIER ), &data );
			}
	}
}

/*
void Aura::SpellAuraAddTargetTrigger(bool apply)
{
	if (m_target == NULL)
	{ 
		return;
	}

	if( apply )
	{
		ProcTriggerSpell pts;
		pts.parentId = GetSpellProto()->Id;
		pts.caster = m_casterGuid;
		int charges = GetSpellProto()->procCharges;
		if( (GetSpellProto()->c_is_flags & SPELL_FLAG_IS_REQUIRING_SM_MOD) && u_caster != NULL )
		{
			SM_FIValue( u_caster->SM_FCharges, &charges, GetSpellProto()->GetSpellGroupType() );
			SM_PIValue( u_caster->SM_PCharges, &charges, GetSpellProto()->GetSpellGroupType() );
#ifdef COLLECTION_OF_UNTESTED_STUFF_AND_TESTERS
			float spell_flat_modifers=0;
			float spell_pct_modifers=0;
			SM_FIValue(u_caster->SM_FCharges,&spell_flat_modifers,GetSpellProto()->GetSpellGroupType());
			SM_FIValue(u_caster->SM_PCharges,&spell_pct_modifers,GetSpellProto()->GetSpellGroupType());
			if(spell_flat_modifers!=0 || spell_pct_modifers!=0)
				printf("!!!!!spell charge bonus mod flat %f , spell range bonus pct %f , spell range %f, spell group %u\n",spell_flat_modifers,spell_pct_modifers,maxRange,m_spellInfo->GetSpellGroupType());
#endif
		}
		pts.procCharges = charges;
		pts.i = mod->i;
		pts.LastTrigger = 0;

		if(GetSpellProto()->EffectTriggerSpell[mod->i])
			pts.spellId=GetSpellProto()->EffectTriggerSpell[mod->i];
		else
		{
			sLog.outDebug("Warning,trigger spell is null for spell %u",GetSpellProto()->Id);
			return;
		}
		m_target->m_procSpells.push_front(pts);
		sLog.outDebug("%u is registering %u chance %u flags %u charges %u triggeronself %u interval %u\n",pts.origId,pts.spellId,pts.procChance,m_spellProto->procFlags & ~PROC2_TARGET_SELF,charges,m_spellProto->procFlags & PROC2_TARGET_SELF,m_spellProto->proc_interval);
	}
	else
	{
		for(std::list<struct ProcTriggerSpell>::iterator itr = m_target->m_procSpells.begin();itr != m_target->m_procSpells.end();itr++)
		{
			if(itr->parentId == GetSpellId() && itr->caster == m_casterGuid && !itr->deleted)
			{
				itr->deleted = true;
				break; //only 1 instance of a proc spell per caster ?
			}
		}
	}
}*/

void Aura::SpellAuraModPowerRegPerc(bool apply)
{
	if( mod->m_miscValue < 0 || mod->m_miscValue >= POWER_TYPE_COUNT )
	{
		sLog.outDebug("Spell %u has missing regen mod type \n", GetSpellProto()->Id );
		return;
	}

	if(apply)
		m_target->PctPowerRegenModifier[mod->m_miscValue] += mod->m_amount;
	else
		m_target->PctPowerRegenModifier[mod->m_miscValue] -= mod->m_amount;

	if (m_target->IsPlayer())
		SafePlayerCast( m_target )->UpdatePowerRegen();
}

void Aura::SpellAuraOverrideClassScripts(bool apply)
{
	Unit *caster = GetUnitCaster();
//	if( m_target->IsPlayer() == false )
	{ 
//		return;
	}
	//just trying to catch a hacker
/*	if( apply 
		&& GetSpellProto()->NameHash != SPELL_HASH_FINGERS_OF_FROST //this can stack up to 3
		)
	{
		int32 Count = 0;
		SimplePointerListNode<SpellEffectOverrideScript> *itr,*itr2;
		for(itr = p_target->mSpellOverrideMap.begin(); itr != p_target->mSpellOverrideMap.end();)
		{
			itr2 = itr;
			itr = itr->Next();
			if( itr2->data->ori_spell == GetSpellProto() )
				Count++;
		}
		if( Count > 0 )
		{
			FILE *f = fopen( "Aura112Stacker.txt","at");
			if( f )
			{
				fprintf( f, "Cheater name %s - %d - %s - %d\n",p_target->GetName(), Count, GetSpellProto()->Name, GetSpellProto()->Id );
				fclose( f );
			}
		}
	}/**/ 
	//misc value is spell to add
	//spell familyname && grouprelation

//	Player *p_target = SafePlayerCast( m_target );

	switch( GetSpellProto()->NameHash )
	{
		case SPELL_HASH_ITEM___PRIEST_T13_HEALER_4P_BONUS__HOLY_WORD_AND_POWER_WORD__SHIELD_:	
		{
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->ApplyHandler = &HandlePriestT13Healer4P;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;
		case SPELL_HASH_IMPROVED_DEATH_STRIKE:	
		{
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount_pct[0] = GetSpellProto()->eff[2].EffectBasePoints;
				so->ApplyHandler = &HandleImprovedDeathStrike;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;
		case SPELL_HASH_FINGERS_OF_FROST:	
		{
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount_pct[0] = 25;	//valus is in the description
				so->ApplyHandler = &HandleFingersOfFrost;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;
		case SPELL_HASH_DEATH_S_EMBRACE:	
		{
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount_pct[0] = GetSpellProto()->eff[0].EffectBasePoints;	
				so->mod_amount_pct[1] = GetSpellProto()->eff[1].EffectBasePoints;	
				so->ApplyHandler = &HandleDeatsEmbrace;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;
		case SPELL_HASH_REND_AND_TEAR:
		{
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount[0] = GetSpellProto()->eff[0].EffectBasePoints;
				so->ApplyHandler = &HandleRendAndTear;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;
		case SPELL_HASH_MOLTEN_FURY:
		{
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount[0] = GetSpellProto()->eff[0].EffectBasePoints;
				so->ApplyHandler = &HandleMoltenFury;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;
		case SPELL_HASH_NATURE_S_BLESSING:
		{
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount[0] = GetSpellProto()->eff[0].EffectBasePoints;
				so->ApplyHandler = &HandleNatureSBlessing;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;
		case SPELL_HASH_FIERY_APOCALYPSE:
		{
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount[0] = GetSpellProto()->eff[1].EffectBasePoints;
				so->ApplyHandler = &HandleFieryApocalypse;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;
		case SPELL_HASH_POTENT_AFFLICTIONS:
		{
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount[0] = GetSpellProto()->eff[1].EffectBasePoints;
				so->ApplyHandler = &HandlePotentAfflictions;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;
		case SPELL_HASH_ESSENCE_OF_THE_VIPER:
		{
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount[0] = GetSpellProto()->eff[1].EffectBasePoints;
				so->ApplyHandler = &HandleEssenceOfTheViper;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;
		case SPELL_HASH_UNSHACKLED_FURY:
		{
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount[0] = GetSpellProto()->eff[2].EffectBasePoints;
				so->ApplyHandler = &HandleUnshackledFury;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;
/*		case SPELL_HASH_ENHANCED_ELEMENTS:
		{
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount[0] = GetSpellProto()->eff[1].EffectBasePoints;
				so->ApplyHandler = &HandleEnhancedElements;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break; */
		case SPELL_HASH_DEEP_HEALING:
		{
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount[0] = GetSpellProto()->eff[1].EffectBasePoints;
				so->ApplyHandler = &HandleDeepHealing;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;
		case SPELL_HASH_TOTAL_ECLIPSE:
		{
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount[0] = GetSpellProto()->eff[1].EffectBasePoints;
				so->ApplyHandler = &HandleTotalEclipse;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;
		case SPELL_HASH_POTENT_POISONS:
		{
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount[0] = GetSpellProto()->eff[2].EffectBasePoints;
				so->ApplyHandler = &HandlePotentPoisons;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;
		case SPELL_HASH_EXECUTIONER:
		{
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount[0] = GetSpellProto()->eff[1].EffectBasePoints;
				so->ApplyHandler = &HandleExecutioner;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;
		case SPELL_HASH_DREADBLADE:
		{
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount[0] = GetSpellProto()->eff[1].EffectBasePoints;
				so->ApplyHandler = &HandleDreadBlade;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break; 
		case SPELL_HASH_BLOOD_SHIELD:
		{
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount[0] = GetSpellProto()->eff[1].EffectBasePoints;
				so->ApplyHandler = &HandleBloodShield;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;
		case SPELL_HASH_FROZEN_HEART:
		{
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount[0] = GetSpellProto()->eff[1].EffectBasePoints;
				so->ApplyHandler = &HandleFrozenHeart;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;
		case SPELL_HASH_TORMENT_THE_WEAK:
		{
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount_pct[0] = mod->m_amount;
				so->ApplyHandler = &HandleTormentTheWeak;
				//no effect on custom handlers
//				so->required_namehash[0] = SPELL_HASH_FROSTFIRE_BOLT;.....
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;
		case SPELL_HASH_SOUL_SIPHON:
		{
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount_pct[0] = mod->m_amount / 3;
				so->ApplyHandler = &HandleSoulSiphon;
				//no effect on custom handlers
//				so->required_namehash[0] = SPELL_HASH_DRAIN_LIFE;
//				so->required_namehash[1] = SPELL_HASH_DRAIN_SOUL;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;
		case SPELL_HASH_SHATTER:
		{
			if( m_target->IsPlayer() == false )
			{ 
				return;
			}
			Player *p_target = SafePlayerCast( m_target );

			uint32 val = 0;
			uint32 val2 = 0;
			if( GetSpellProto()->Id == 11170 )
			{
				val = 1;	//stupid value is in description
				val2 = 10;	// bonus to frostbolt
			}
			else if( GetSpellProto()->Id == 12982 )
			{
				val = 2;	//stupid value is in description
				val2 = 20;	// bonus to frostbolt
			}
			if( apply )
				p_target->m_FrozenCritChanceMultiplier += val;
			else if( p_target->m_FrozenCritChanceMultiplier >= val )
				p_target->m_FrozenCritChanceMultiplier -= val;
			else
				p_target->m_FrozenCritChanceMultiplier = 1;
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount_pct[0] = val2;
				so->ApplyHandler = &HandleShatter;
				//std stuff
				so->ori_spell = GetSpellProto();
				p_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;
/*		case SPELL_HASH_GLYPH_OF_LESSER_HEALING_WAVE:
		{
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount_pct[0] = 100 + mod->m_amount;
				so->ApplyHandler = &HandleGlyphOfLesserHealingWave;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;*/
		case SPELL_HASH_GLYPH_OF_REGROWTH:
		{
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount_pct[0] = 100 + mod->m_amount;
				so->ApplyHandler = &HandleGlyphOfRegrowth;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;
		case SPELL_HASH_INCREASED_LESSER_HEALING_WAVE:	//there are 3
		case SPELL_HASH_SAVAGE_TOTEM_OF_THE_THIRD_WIND:
		case SPELL_HASH_HATEFUL_TOTEM_OF_THE_THIRD_WIND:
		case SPELL_HASH_DEADLY_TOTEM_OF_THE_THIRD_WIND:
		case SPELL_HASH_LK_ARENA_4_TOTEM_OF_THE_THIRD_WIND:
		case SPELL_HASH_LK_ARENA_5_TOTEM_OF_THE_THIRD_WIND:
		case SPELL_HASH_LK_ARENA_6_TOTEM_OF_THE_THIRD_WIND:
		case SPELL_HASH_COPY_OF_DEADLY_TOTEM_OF_THE_THIRD_WIND:
		{
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount[0] = mod->m_amount;	//the heal effect
				so->required_namehash[0] = SPELL_HASH_LESSER_HEALING_WAVE;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;
		case SPELL_HASH_INCREASED_FLASH_OF_LIGHT_HEALING:
		{
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount[0] = mod->m_amount;	//the heal effect
				so->required_namehash[0] = SPELL_HASH_FLASH_OF_LIGHT;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;
		case SPELL_HASH_INCREASED_SHOCK_DAMAGE:	
		{
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount[0] = mod->m_amount;	
				so->required_namehash[0] = SPELL_HASH_FROST_SHOCK;
				so->required_namehash[1] = SPELL_HASH_EARTH_SHOCK;
				so->required_namehash[2] = SPELL_HASH_FLAME_SHOCK;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;
		case SPELL_HASH_IMPROVED_MOONFIRE:	
		{
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount[0] = mod->m_amount;	
				so->mod_amount[1] = mod->m_amount;	
				so->required_namehash[0] = SPELL_HASH_MOONFIRE;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;
/*		case SPELL_HASH_MOLTEN_FURY:	
		{
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount_pct[0] = 100 + mod->m_amount;	
				so->mod_amount_pct[1] = 100 + mod->m_amount;	
				so->mod_amount_pct[2] = 100 + mod->m_amount;	
				so->ApplyHandler = &HandleMoltenFury;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break; */
		case SPELL_HASH_INCREASED_LIGHTNING_DAMAGE:	
		case SPELL_HASH_TOTEM_OF_HEX:	
		{
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount[0] = mod->m_amount;	
				so->required_namehash[0] = SPELL_HASH_CHAIN_LIGHTNING;
				so->required_namehash[1] = SPELL_HASH_LIGHTNING_BOLT;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;
		case SPELL_HASH_INCREASED_REJUVENATION_HEALING:	
		{
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount[0] = mod->m_amount;	
				so->mod_amount[1] = mod->m_amount;	
				so->mod_amount[2] = mod->m_amount;	
				so->required_namehash[0] = SPELL_HASH_REJUVENATION;
				so->required_aura_type[0] = SPELL_AURA_PERIODIC_HEAL;
				so->required_aura_type[1] = SPELL_AURA_PERIODIC_HEAL;
				so->required_aura_type[2] = SPELL_AURA_PERIODIC_HEAL;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;
		case SPELL_HASH_IMPROVED_CONSECRATION:	
		case SPELL_HASH_LIBRAM_OF_RESURGENCE:	
		{
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				if( GetSpellProto()->Id == 38422 )
					so->mod_amount_pct[0] = 100 + mod->m_amount;	
				else
					so->mod_amount[0] = mod->m_amount;	
				so->required_namehash[0] = SPELL_HASH_CONSECRATION;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;
		case SPELL_HASH_IMPROVED_STARFIRE:	
		case SPELL_HASH_IDOL_OF_THE_SHOOTING_STAR:	
		{
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount[0] = mod->m_amount;	
				so->required_namehash[0] = SPELL_HASH_STARFIRE;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;
		case SPELL_HASH_STARFIRE_BONUS:	
		{
			if( apply )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount_pct[0] = 100 + mod->m_amount;	
				so->ApplyHandler = &HandleStarfireBonus;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;
		case SPELL_HASH_TEST_OF_FAITH:	
		{
			if( apply && mod->i == 0 )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount_pct[0] = 100 + mod->m_amount;	
				so->ApplyHandler = &HandleTestOfFaith;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;
		case SPELL_HASH_MERCILESS_COMBAT:	
		{
			if( apply && mod->i == 0 )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount_pct[0] = 100 + mod->m_amount;	
				so->ApplyHandler = &HandleMercilessCombat;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;
/*		case SPELL_HASH_RAGE_OF_RIVENDARE:	
		{
			if( apply && mod->i == 0 )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount_pct[0] = 100 + mod->m_amount;	
				so->mod_amount_pct[1] = 100 + mod->m_amount;	
				so->mod_amount_pct[2] = 100 + mod->m_amount;	
				so->ApplyHandler = &HandleRageOfRivedare;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break; 
		case SPELL_HASH_TUNDRA_STALKER:	
		{
			if( apply && mod->i == 0 )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount_pct[0] = 100 + mod->m_amount;	
				so->ApplyHandler = &HandleTundraStalker;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break; */
		case SPELL_HASH_MARKED_FOR_DEATH_53241:	
		case SPELL_HASH_MARKED_FOR_DEATH_67823:	
		{
			if( apply && mod->i == 0 )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount_pct[0] = 100 + mod->m_amount;	
				so->ApplyHandler = &HandleMarkedForDeath;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break;
/*		case SPELL_HASH_NOURISH_HEAL_BOOST:	
		{
			if( apply && mod->i == 0 )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount_pct[0] = 100 + mod->m_amount;	
				so->ApplyHandler = &HandleNourishHealBoost;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break; */
/*		case SPELL_HASH_ASPECT_MASTERY:	
		{
			if( apply && mod->i == 0 )
			{
				SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
				so->mod_amount_pct[0] = 100 - (GetSpellProto()->eff[0].EffectBasePoints + 1);	
				so->mod_amount[1] = (GetSpellProto()->eff[1].EffectBasePoints + 1);	
				so->mod_amount_pct[2] = 100 + GetSpellProto()->eff[2].EffectBasePoints + 1;	
				so->ApplyHandler = &HandleAspectMastery;
				//std stuff
				so->ori_spell = GetSpellProto();
				m_target->mSpellOverrideMap.push_front( so );
				mod->extension = (void*)so;
			}
		}break; */
		case SPELL_HASH_ENIGMA_BLIZZARD_BONUS:
		{
		}break;
		default:
			{
				if( apply == true )
				{
					if( GetSpellProto()->ClassOverideApplyHandler != NULL )
					{
						SpellEffectOverrideScript *so = new SpellEffectOverrideScript;
						so->ApplyHandler = GetSpellProto()->ClassOverideApplyHandler;
						//std stuff
						so->mod_amount[0] = mod->m_amount;
						so->ori_spell = GetSpellProto();
						so->CasterGuid = caster->GetGUID();
						if( caster != m_target )
							so->CastedOnTarget = true;
						else
							so->CastedOnTarget = false;
						so->TargetGuid = m_target->GetGUID();
						m_target->mSpellOverrideMap.push_front( so );
						mod->extension = (void*)so;
					}
				}
			}break;
	};

	if( apply == false )
	{
		SimplePointerListNode<SpellEffectOverrideScript> *itr,*itr2;
		for(itr = m_target->mSpellOverrideMap.begin(); itr != m_target->mSpellOverrideMap.end();)
		{
			itr2 = itr;
			itr = itr->Next();
			if( (void*)(itr2->data) == mod->extension )
			{
				if( itr2->data->RemoveHandler ) 
					itr2->data->RemoveHandler( this, GetUnitCaster(), m_target, mod->m_amount, mod->i, itr2->data );
				m_target->mSpellOverrideMap.remove( itr2, 1 );//no leaks pls = delete on remove
				mod->extension = NULL;
			}
		}
	}
}

void Aura::SpellAuraModRangedDamageTaken(bool apply)
{
	if(apply)
		m_target->RangedDamageTaken += mod->m_amount;
	else
	{
		m_target->RangedDamageTaken -= mod->m_amount;;
		if( m_target->RangedDamageTaken < 0)
			m_target->RangedDamageTaken = 0;
	}
}

void Aura::SpellAuraModHealing(bool apply)
{
	int32 val;
	if(apply)
	{
		 val = mod->m_amount;
		 /*if(val>0)
			 SetPositive();
		 else
			 SetNegative();*/
	}
	else
		val=-mod->m_amount;

	m_target->HealTakenMod += val;
}

void Aura::SpellAuraIgnoreRegenInterrupt(bool apply)
{
	if(!m_target->IsPlayer())
	{ 
		return;
	}

	if(apply)
		SafePlayerCast( m_target )->PctIgnoreRegenModifier += ((float)(mod->m_amount))/100;
	else
		SafePlayerCast( m_target )->PctIgnoreRegenModifier -= ((float)(mod->m_amount))/100;
}

void Aura::SpellAuraModMechanicResistance(bool apply)
{
	if(apply)
	{
		assert( mod->m_miscValue < MECHANIC_TOTAL );
		m_target->MechanicsResistancesPCT[mod->m_miscValue]+=mod->m_amount;

		if(mod->m_miscValue != MECHANIC_HEALING && mod->m_miscValue != MECHANIC_INVULNERABLE && mod->m_miscValue != MECHANIC_SHIELDED ) // dont remove bandages, Power Word and protection effect
			SetPositive();
		else
			SetNegative();
	}
	else
		m_target->MechanicsResistancesPCT[mod->m_miscValue]-=mod->m_amount;
}

void Aura::SpellAuraModHealingPCT(bool apply)
{
	int32 val;
	if(apply)
	{
		 val = mod->m_amount;
		 if(val<0)
			SetNegative();
		 else
			SetPositive();
	}
	else
		val=-mod->m_amount;

	m_target->HealTakenPctMod += val;

	Player *p_target;
	if( m_target->IsPlayer() )
		p_target = SafePlayerCast( m_target );
	else
		p_target = NULL;
	if( p_target )
		p_target->SetFloatValue( PLAYER_FIELD_MOD_HEALING_PCT, (100 + m_target->HealTakenPctMod ) / 100.0f );
}

void Aura::SpellAuraUntrackable(bool apply)
{
    if(apply)
        m_target->SetFlag(UNIT_FIELD_BYTES_1, STANDSTATE_FLAG_UNTRACKABLE);
    else
        m_target->RemoveFlag(UNIT_FIELD_BYTES_1, STANDSTATE_FLAG_UNTRACKABLE);
}

void Aura::SpellAuraModRangedAttackPower(bool apply)
{
	if(apply)
	{
		if(mod->m_amount > 0)
		{
			SetPositive();
			m_target->ModUnsigned32Value(UNIT_FIELD_RANGED_ATTACK_POWER_MOD_POS,mod->m_amount);
		}
		else
		{
			SetNegative();
			m_target->ModUnsigned32Value(UNIT_FIELD_RANGED_ATTACK_POWER_MOD_NEG,mod->m_amount);
		}
	}
	else
	{
		if(mod->m_amount > 0)
			m_target->ModUnsigned32Value(UNIT_FIELD_RANGED_ATTACK_POWER_MOD_POS,-mod->m_amount);
		else
			m_target->ModUnsigned32Value(UNIT_FIELD_RANGED_ATTACK_POWER_MOD_NEG,-mod->m_amount);
	}

	m_target->CalcDamage();
}

void Aura::SpellAuraModMeleeDamageTaken(bool apply)
{
	if(apply)
	{
		if(mod->m_amount > 0)//does not exist but let it be
			SetNegative();
		else
			SetPositive();
		m_target->DamageTakenMod[0] += mod->m_amount;
	}
	else
		m_target->DamageTakenMod[0] -= mod->m_amount;
}

void Aura::SpellAuraModMeleeDamageTakenPct(bool apply)
{
	if(apply)
	{
		if(mod->m_amount>0)//does not exist but let it be
			SetNegative();
		else
			SetPositive();
	}
/*	float val = MAX( 0.0001f, 1.0f + ((float)(mod->m_amount))/100.0f ); //postive increases dmg taken, negative decreases dmg taken
	if( apply == false )
		val = 1.0f / val;
	m_target->DamageTakenPctMod[SCHOOL_NORMAL] *= val;	//reduce dmg not increase
	*/
	int32 val;
	if( apply )
		val = -mod->m_amount;
	else
		val = +mod->m_amount;
	m_target->DamageTakenPctMod[SCHOOL_NORMAL] += val;	//reduce dmg not increase
}

void Aura::SpellAuraRAPAttackerBonus(bool apply)
{
	if(apply)
	{
		m_target->RAPvModifier += mod->m_amount;
	}
	else
		m_target->RAPvModifier -= mod->m_amount;
}

void Aura::SpellAuraModIncreaseSpeedAlways(bool apply)
{
	if(apply)
	{
		SetPositive();
//		m_target->m_speedModifier += mod->m_amount;
		if( (GetSpellProto()->c_is_flags & SPELL_FLAG_IS_REQUIRING_SM_MOD) && m_target->SM_Mods)
		{
			int32 speedmod=0;
			SM_FIValue(m_target->SM_Mods->SM_FSpeedMod,&speedmod,GetSpellProto()->GetSpellGroupType());
			mod->m_amount += speedmod;
		}
		m_target->speedIncreaseMap.push_front( mod );
	}
	else
	{
//		m_target->m_speedModifier -= mod->m_amount;
		m_target->speedIncreaseMap.remove( mod );
	}

	if( m_target->GetSpeedIncrease() )
		m_target->UpdateSpeed();
}

void Aura::SpellAuraModIncreaseEnergyPerc( bool apply )
{
	SetPositive();
	Player *p_target;
	if( m_target->IsPlayer() )
		p_target = SafePlayerCast( m_target );
	else
	{
		return;
	}

	if(apply)
	{
		mod->m_amount = MAX( -99, mod->m_amount );
		p_target->m_manafromspellPCT += mod->m_amount;
	}
	else
	{
		p_target->m_manafromspellPCT -= mod->m_amount;
	}
	p_target->UpdatePowerRegen();
}

void Aura::SpellAuraModIgnoreArmorForWeapon( bool apply )
{
	if (m_target->IsPlayer())
	{
		Player *p_target = SafePlayerCast( m_target );
		if(apply)
		{
			WeaponModifier *md;
			md = new WeaponModifier;
			// weapon_target_armor_pct_ignore = 1 + value
			// new armor = armor * weapon_target_armor_pct_ignore
			md->value = float(mod->m_amount) / -100.0f;	
			md->wclass = GetSpellProto()->GetEquippedItemClass();
			md->subclass = GetSpellProto()->GetEquippedItemSubClass();
			md->spellentry = GetSpellProto();
			p_target->ignore_armor_pct.push_front( md );
		}
		else
		{
			SimplePointerListNode<WeaponModifier> *i;
			for( i=p_target->ignore_armor_pct.begin();i!=p_target->ignore_armor_pct.end();i = i->Next())
				if( i->data->spellentry == GetSpellProto() )
				{
					p_target->ignore_armor_pct.remove( i, 1 );
					break;
				}
		}
		SafePlayerCast( m_target )->UpdateChances();
	}
}

void Aura::SpellAuraModIncreaseMaxHealthPerc( bool apply )
{
	SetPositive();
	if( apply )
	{
		mod->fixed_amount[ 1 ] = mod->fixed_amount[ 0 ] = m_target->GetModPUInt32Value( UNIT_FIELD_MAXHEALTH, mod->m_amount );
		
		if( GetSpellProto()->NameHash == SPELL_HASH_FRENZIED_REGENERATION )
		{
			int32 max_increase = mod->fixed_amount[ 1 ];
			int32 can_increase = MAX( 0, max_increase - (int32)m_target->GetHealth() );
			mod->fixed_amount[1] = can_increase;
		}

		m_target->ModUnsigned32Value( UNIT_FIELD_MAXHEALTH, mod->fixed_amount[ 0 ] );
		m_target->ModUnsigned32Value( UNIT_FIELD_HEALTH, mod->fixed_amount[ 1 ] );
		if( m_target->IsPlayer() )
			( ( Player* )m_target )->SetHealthFromSpell( ( SafePlayerCast(m_target) )->GetHealthFromSpell() + mod->fixed_amount[ 0 ] );
		else if( m_target->IsPet() )
		{
			SafePetCast( m_target )->SetHealthFromSpell( ( SafePetCast(m_target) )->GetHealthFromSpell() + mod->fixed_amount[ 0 ] );
			SafePetCast( m_target )->CalcStat( STAT_STAMINA );
		}
	}
	else
	{
		m_target->ModUnsigned32Value( UNIT_FIELD_MAXHEALTH, -mod->fixed_amount[ 0 ] );
		if( (int32)m_target->GetUInt32Value( UNIT_FIELD_HEALTH ) > mod->fixed_amount[ 1 ] )
			m_target->ModUnsigned32Value( UNIT_FIELD_HEALTH, -mod->fixed_amount[ 1 ] );
		else if( m_target->isAlive() )
			//according to wowhead health is reseted to 1% min
			m_target->SetUInt32Value( UNIT_FIELD_HEALTH, m_target->GetUInt32Value( UNIT_FIELD_MAXHEALTH ) / 100 );
		//how on earth ?
		if( m_target->GetUInt32Value( UNIT_FIELD_HEALTH ) > m_target->GetUInt32Value( UNIT_FIELD_MAXHEALTH ) )
			m_target->SetUInt32Value( UNIT_FIELD_HEALTH, m_target->GetUInt32Value( UNIT_FIELD_MAXHEALTH ) );
		if( m_target->IsPlayer() )
			( ( Player* )m_target )->SetHealthFromSpell( (SafePlayerCast(m_target) )->GetHealthFromSpell() - mod->fixed_amount[ 0 ] );
		else if( m_target->IsPet() )
		{
			SafePetCast( m_target )->SetHealthFromSpell( ( SafePetCast(m_target) )->GetHealthFromSpell() - mod->fixed_amount[ 0 ] );
			SafePetCast( m_target )->CalcStat( STAT_STAMINA );
		}
	}
}

void Aura::SpellAuraModManaRegInterrupt( bool apply )
{
	if( m_target->IsPlayer() )
	{
		if( apply )
			SafePlayerCast( m_target )->m_ModInterrMRegenPCT += mod->m_amount;
		else
			SafePlayerCast( m_target )->m_ModInterrMRegenPCT -= mod->m_amount;

		SafePlayerCast( m_target )->UpdateStats();
	}
}

void Aura::SpellAuraModTotalStatPerc(bool apply)
{
	int32 stat = ( int32 )mod->m_miscValue;
	int32 statMask = ( int32 )m_spellProto->eff[mod->i].EffectMiscValueB;
	int32 val;

	if(apply)
	{
	   val= mod->m_amount;
	}
	else
	   val= -mod->m_amount;

	if( statMask == 0 )
		statMask = 1 << stat;
//	if( stat == -1 )//all stats
//		statMask = -1;	//seen some rare cases when mask was not good

	if(m_target->IsPlayer())
	{
		for(uint32 x=0;x<STAT_COUNT;x++)
			if( (1 << x) & statMask )
			{
				if(mod->m_amount>0)
					SafePlayerCast( m_target )->TotalStatModPctPos[x] += val;
				else
					SafePlayerCast( m_target )->TotalStatModPctNeg[x] -= val;
				SafePlayerCast( m_target )->CalcStat(x);
			}
	}
	else if( m_target->IsCreature() )
	{
		for(uint32 x=0;x<STAT_COUNT;x++)
			if( (1 << x) & statMask )
			{
				SafeCreatureCast(m_target)->TotalStatModPct[x] += val;
				SafeCreatureCast(m_target)->CalcStat(x);
			}
	}
	if(m_target->IsPlayer())
	{
		SafePlayerCast( m_target )->UpdateStats();
		SafePlayerCast( m_target )->UpdateChances();
	}
}

void Aura::SpellAuraModHaste( bool apply )
{
	if( mod->m_amount < 0 )
		SetNegative();
	else
		SetPositive();

	if( m_target->IsPlayer() )
	{
		if( apply )
			SafePlayerCast( m_target )->ModAttackSpeed( mod->m_amount, MOD_MELEE );	//positive increases speed
		else
			SafePlayerCast( m_target )->ModAttackSpeed( -mod->m_amount, MOD_MELEE );
	}
	else
	{
		if( apply )
		{
			mod->fixed_amount[ 0 ] = m_target->GetModPUInt32Value( UNIT_FIELD_BASEATTACKTIME, mod->m_amount );
			mod->fixed_amount[ 1 ] = m_target->GetModPUInt32Value( UNIT_FIELD_BASEATTACKTIME_1, mod->m_amount );

			if( (int32)m_target->GetUInt32Value ( UNIT_FIELD_BASEATTACKTIME ) <= mod->fixed_amount[ 0 ] )
				mod->fixed_amount[ 0 ] = m_target->GetUInt32Value ( UNIT_FIELD_BASEATTACKTIME ); //watch it, a negative timer might be bad ;)
			if( (int32)m_target->GetUInt32Value ( UNIT_FIELD_BASEATTACKTIME_1 ) <= mod->fixed_amount[ 1 ] )
				mod->fixed_amount[ 1 ] = m_target->GetUInt32Value ( UNIT_FIELD_BASEATTACKTIME_1 );//watch it, a negative timer might be bad ;)

			m_target->ModUnsigned32Value( UNIT_FIELD_BASEATTACKTIME, -mod->fixed_amount[ 0 ] );
			m_target->ModUnsigned32Value( UNIT_FIELD_BASEATTACKTIME_1, -mod->fixed_amount[ 1 ] );
			
			if ( m_target->IsCreature() )
				SafeCreatureCast( m_target )->m_speedFromHaste += mod->fixed_amount[ 0 ];
		}
		else
		{
			m_target->ModUnsigned32Value( UNIT_FIELD_BASEATTACKTIME, mod->fixed_amount[ 0 ] );
			m_target->ModUnsigned32Value( UNIT_FIELD_BASEATTACKTIME_1, mod->fixed_amount[ 1 ] );

			if ( m_target->IsCreature() )
				SafeCreatureCast( m_target )->m_speedFromHaste -= mod->fixed_amount[ 0 ];
		}
	}
}

void Aura::SpellAuraForceReaction( bool apply )
{
	map<uint32,uint32>::iterator itr;
	Player * p_target = SafePlayerCast( m_target );
	if( !m_target->IsPlayer() )
	{ 
		return;
	}

	if( apply )
	{
		itr = p_target->m_forcedReactions.find( mod->m_miscValue );
		if( itr != p_target->m_forcedReactions.end() )
			itr->second = mod->m_amount;
		else
			p_target->m_forcedReactions.insert( make_pair( mod->m_miscValue, mod->m_amount ) );
	}
	else
		p_target->m_forcedReactions.erase( mod->m_miscValue );

	WorldPacket data( SMSG_SET_FORCED_REACTIONS, ( 8 * p_target->m_forcedReactions.size() ) + 4 );
	data << uint32(p_target->m_forcedReactions.size());
	for( itr = p_target->m_forcedReactions.begin(); itr != p_target->m_forcedReactions.end(); ++itr )
	{
		data << itr->first;
		data << itr->second;
	}

	p_target->GetSession()->SendPacket( &data );
}

void Aura::SpellAuraModRangedHaste( bool apply )
{
	if( mod->m_amount < 0 )
		SetNegative();
	else
		SetPositive();

	if( m_target->IsPlayer() )
	{
		if( apply )
			SafePlayerCast( m_target )->ModAttackSpeed( mod->m_amount, MOD_RANGED );
		else
			SafePlayerCast( m_target )->ModAttackSpeed( -mod->m_amount, MOD_RANGED );
	}
	else
	{
		if( apply )
		{
			mod->fixed_amount[ 2 ] = m_target->GetModPUInt32Value(UNIT_FIELD_RANGEDATTACKTIME,mod->m_amount);
			m_target->ModUnsigned32Value(UNIT_FIELD_RANGEDATTACKTIME, -mod->fixed_amount[ 2 ]);
		}
		else m_target->ModUnsigned32Value(UNIT_FIELD_RANGEDATTACKTIME, mod->fixed_amount[ 2 ]);
	}
}

void Aura::SpellAuraModRangedAmmoHaste( bool apply )
{
	SetPositive();
	if( !m_target->IsPlayer() )
	{ 
		return;
	}

	if( apply )
		SafePlayerCast( m_target )->ModAttackSpeed( mod->m_amount, MOD_RANGED );
	else
		SafePlayerCast( m_target )->ModAttackSpeed( -mod->m_amount, MOD_RANGED );
}

void Aura::SpellAuraModResistanceExclusive(bool apply)
{
	if( apply )
		m_target->ResistanceExclusive.push_front( mod );
	else
		m_target->ResistanceExclusive.remove( mod );

	uint32 Flag = mod->m_miscValue;
	if( m_target->IsPlayer() )
	{
		for( uint32 x = 0; x < SCHOOL_COUNT; x++ )
			if(Flag & (((uint32)1)<< x) )
				SafePlayerCast( m_target )->CalcResistance(x);
	}
	else if( m_target->IsCreature() )
	{
		for(uint32 x=0;x<SCHOOL_COUNT;x++)
			if(Flag & (((uint32)1)<<x))
				SafeCreatureCast( m_target )->CalcResistance(x);
	}
}

//!! this works hand in hand with add combo points. One ads them the other one removes them
//note that these points if get consumed should not be removed. You might want to see Premeditation(14183) script for that
void Aura::SpellAuraRetainComboPoints(bool apply)
{
	if( m_target->IsPlayer() )
	{
		if( apply )
			SafePlayerCast( m_target )->m_tempComboPoints = SafePlayerCast( m_target )->m_comboPoints;
		else if( SafePlayerCast( m_target )->m_tempComboPoints > 0 )
			SafePlayerCast( m_target )->RemoveComboPoints( mod->m_amount );
	}
}

void Aura::SpellAuraResistPushback(bool apply)
{
	//DK:This is resist for spell casting delay
	//Only use on players for now

	if(m_target->IsPlayer())
	{
		int32 val;
		if(apply)
		{
			val = mod->m_amount;
			SetPositive();
		}
		else
			val=-mod->m_amount;

		for(uint32 x=0;x<SCHOOL_COUNT;x++)
			if (mod->m_miscValue & (((uint32)1)<<x) )
				SafePlayerCast( m_target )->SpellDelayResist[x] += val;
	}
}

void Aura::SpellAuraModShieldBlockPCT( bool apply )
{
	Player *p_target;
	if( m_target->IsPlayer() )
		p_target = SafePlayerCast( m_target );
	else
		p_target = NULL;
	if( p_target != NULL )
	{
		if( apply )
			p_target->m_modblockabsorbvaluePCT += ( uint32 )mod->m_amount;
		else
			p_target->m_modblockabsorbvaluePCT -= ( uint32 )mod->m_amount;
		p_target->UpdateStats();
	}
}

void Aura::SpellAuraTrackStealthed(bool apply)
{
//	Unit * c;
//	if((c=GetUnitCaster()) == NULL)
//	{ 
//		return;
//	}
//	c->trackStealth = apply;

	if( m_target->IsPlayer() == false )
		return;

	if( apply )
		m_target->SetFlag( PLAYER_FIELD_BYTES, PLAYER_FIELD_BYTES_TRACK_STEALTHED );
	else
		m_target->RemoveFlag( PLAYER_FIELD_BYTES, PLAYER_FIELD_BYTES_TRACK_STEALTHED );
}

void Aura::SpellAuraModDetectedRange(bool apply)
{
	if(!m_target->IsPlayer())
	{ 
		return;
	}
	if(apply)
	{
		SetPositive();
		SafePlayerCast( m_target )->DetectedRange += mod->m_amount;
	}
	else
	{
		SafePlayerCast( m_target )->DetectedRange -= mod->m_amount;
	}
}

void Aura::SpellAuraSplitDamageFlat(bool apply)
{
	if( !m_target || !m_target->IsUnit() )
	{ 
		return;
	}

	if (m_target->m_damageSplitTarget)
	{
		delete m_target->m_damageSplitTarget;
		m_target->m_damageSplitTarget = NULL;
	}

	if(apply)
	{
		DamageSplitTarget *ds = new DamageSplitTarget;
		ds->m_flatDamageSplit = mod->m_miscValue;
		ds->m_spellId = GetSpellProto()->Id;
		ds->m_pctDamageSplit = 0;
//		ds->damage_type = mod->m_AuraName;
		ds->creator = (void*)this;
		ds->m_target = GetCaster()->GetGUID();
		m_target->m_damageSplitTarget = ds;
//		printf("registering dmg split %u, amout= %u \n",ds->m_spellId, mod->m_amount, mod->m_miscValue, mod->m_type);
	}
}

void Aura::SpellAuraModStealthLevel(bool apply)
{
	if(apply)
	{
		SetPositive();
		m_target->m_stealthLevel += mod->m_amount;
	}
	else
		m_target->m_stealthLevel -= mod->m_amount;
}

void Aura::SpellAuraModUnderwaterBreathing(bool apply)
{
	if(m_target->IsPlayer())
	{
		uint32 m_UnderwaterMaxTimeSaved = SafePlayerCast( m_target )->m_UnderwaterMaxTime;
		if( apply )
			SafePlayerCast( m_target )->m_UnderwaterMaxTime *= (1 + mod->m_amount / 100 );
		else
			SafePlayerCast( m_target )->m_UnderwaterMaxTime /= (1 + mod->m_amount / 100 );
                SafePlayerCast( m_target )->m_UnderwaterTime *= SafePlayerCast( m_target )->m_UnderwaterMaxTime / m_UnderwaterMaxTimeSaved;
	}
}

void Aura::SpellAuraSafeFall(bool apply)
{
	SetPositive();

	if( !m_target->IsPlayer() )
	{ 
		return;
	}
	if( apply )
		( SafePlayerCast(m_target) )->m_safeFall += mod->m_amount;
	else
		( SafePlayerCast(m_target) )->m_safeFall -= mod->m_amount;
}

void Aura::SpellAuraCrowdControlImmunity(bool apply)
{
	SetPositive();

uint32 cc_mechanics[] = {
    MECHANIC_CHARMED ,
    MECHANIC_DISORIENTED, // 2
//    MECHANIC_DISARMED, // 3
    MECHANIC_DISTRACED, // 4
    MECHANIC_FLEEING, // 5
    MECHANIC_GRIPPED, // 6
    MECHANIC_ROOTED, // 7
    MECHANIC_SILENCED, // 9
    MECHANIC_ASLEEP, // 10
    MECHANIC_ENSNARED, // 11
	MECHANIC_STUNNED, // 12
	MECHANIC_FROZEN, // 13
	MECHANIC_INCAPACIPATED, // 14
	MECHANIC_POLYMORPHED, // 17
	MECHANIC_BANISHED, // 18
	MECHANIC_SHACKLED, // 20
	MECHANIC_INFECTED, // 22
	MECHANIC_TURNED, // 23
	MECHANIC_HORRIFIED, // 24
	MECHANIC_INTERRUPTED, // 26
	MECHANIC_DAZED, // 27
	MECHANIC_SAPPED, // 30
	0
		};
	//seems to be mechanic immunity but with more then 1 value
	if(apply)
	{
/*		for(int i=0;cc_mechanics[i] !=0;i++)
		{
			m_target->MechanicsDispels[cc_mechanics[i]]++;
			m_target->RemoveAllAurasByMechanic( cc_mechanics[i] , (uint32)(-1) , false );
		}*/
		uint32 mechanic_mask = mod->m_miscValue;	//can be moded by glyph
		for( int i=0;i<MIN(32,MECHANIC_TOTAL);i++)
			if( mechanic_mask & ( 1 << i ) )
			{
				m_target->MechanicsDispels[ i+1 ]++;
				m_target->RemoveAllAurasByMechanic( i+1 , (uint32)(-1) , false );
			}
	}
	else
	{
/*		for(int i=0;cc_mechanics[i] !=0;i++)
			if( m_target->MechanicsDispels[cc_mechanics[i]] > 0 )
				m_target->MechanicsDispels[cc_mechanics[i]]--;*/
		uint32 mechanic_mask = mod->m_miscValue;	//can be moded by glyph
		for( int i=0;i<MIN(32,MECHANIC_TOTAL);i++)
			if( ( mechanic_mask & ( 1 << i ) ) && m_target->MechanicsDispels[ i+1 ] > 0 )
				m_target->MechanicsDispels[ i+1 ]--;
	}
}

void Aura::SpellAuraModReputationAdjust(bool apply)
{
	/*SetPositive();
	bool updateclient = true;
	if(IsPassive())
		updateclient = false;	 // dont update client on passive

	if(m_target->GetTypeId()==TYPEID_PLAYER)
	{
		if(apply)
			  SafePlayerCast( m_target )->modPercAllReputation(mod->m_amount, updateclient);
		  else
			SafePlayerCast( m_targe t)->modPercAllReputation(-mod->m_amount, updateclient);
	}*/

	// This is _actually_ "Reputation gains increased by x%."
	// not increase all rep by x%.

	if(m_target->IsPlayer())
	{
		SetPositive();
		if(apply)
			SafePlayerCast( m_target )->pctReputationMod += mod->m_amount;
		else
			SafePlayerCast( m_target )->pctReputationMod -= mod->m_amount;
	}
}

void Aura::SpellAuraNoPVPCredit(bool apply)
{
	if(m_target->GetTypeId() != TYPEID_PLAYER)
	{ 
		return;
	}

	Player* pPlayer = SafePlayerCast( m_target );

	if (apply)
		pPlayer->m_honorless++;
	else
		pPlayer->m_honorless--;
}

void Aura::SpellAuraModHealthRegInCombat(bool apply)
{
	// demon armor etc, they all seem to be 5 sec.
	if(apply)
	{
		sEventMgr.AddEvent(this, &Aura::EventPeriodicHealNoBonus, uint32(mod->m_amount), EVENT_AURA_PERIODIC_HEALINCOMB, 5000, 0,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
	}
}

void Aura::EventPeriodicBurn(uint32 amount, uint32 misc)
{
	Unit*m_caster = GetUnitCaster();

	if(!m_caster)
	{ 
		return;
	}

	if(m_target->isAlive() && m_caster->isAlive())
	{
		if( m_target->SchoolImmunityAntiEnemy[GetSpellProto()->School] || m_target->SchoolImmunityAntiFriend[GetSpellProto()->School] )
		{ 
			return;
		}

		uint32 Amount = (uint32)MIN( (int32)amount, m_target->GetPower( misc ) );
		uint32 newHealth = m_target->GetPower( misc ) - Amount ;

//		m_target->SendSpellPeriodicAuraLog(m_target, m_target, m_spellProto->Id, m_spellProto->School, newHealth, 0, 0, FLAG_PERIODIC_DAMAGE, 0);
		int32 abs = m_target->ResilianceAbsorb( Amount, GetSpellProto(), m_caster );
		Amount -= abs;
		m_target->SendCombatLogMultiple(m_target,Amount,0,abs,0, m_spellProto->spell_id_client, m_spellProto->SchoolMask, COMBAT_LOG_SPELL_DAMAGE,0,COMBAT_LOG_SUBFLAG_PERIODIC_DAMAGE);
		m_caster->DealDamage(m_target, Amount, GetSpellProto()->Id);
	}
	m_flags |= FIRST_TICK_ENDED;
}

void Aura::SpellAuraPowerBurn(bool apply)
{
	//0 mana,1 rage, 3 energy
	if(apply)
	{
		//hasting affects ticks too
		Unit *Unitc = GetUnitCaster();
		int32 period = GetSpellAmplitude( GetSpellProto(), Unitc, mod->i, m_castedItemId );

		sEventMgr.AddEvent(this, &Aura::EventPeriodicBurn, uint32(mod->m_amount), (uint32)mod->m_miscValue, EVENT_AURA_PERIODIC_BURN, period , 0,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
	}
}

void Aura::SpellAuraModCritDmgSchool(bool apply)
{
	if(m_target->IsPlayer())
	{
		if(apply)
		{
			if( mod->m_miscValue & SCHOOL_MASK_NORMAL )
				SafePlayerCast( m_target )->m_modphyscritdmgPCT += (uint32)mod->m_amount;
			if( mod->m_miscValue & ( SCHOOL_MASK_HOLY | SCHOOL_MASK_FIRE | SCHOOL_MASK_NATURE | SCHOOL_MASK_FROST | SCHOOL_MASK_SHADOW | SCHOOL_MASK_ARCANE ) )
				SafePlayerCast( m_target )->m_modSpellCritdmgPCT += (uint32)mod->m_amount;
		}
		else
		{
			if( mod->m_miscValue & SCHOOL_MASK_NORMAL )
				SafePlayerCast( m_target )->m_modphyscritdmgPCT -= (uint32)mod->m_amount;
			if( mod->m_miscValue & ( SCHOOL_MASK_HOLY | SCHOOL_MASK_FIRE | SCHOOL_MASK_NATURE | SCHOOL_MASK_FROST | SCHOOL_MASK_SHADOW | SCHOOL_MASK_ARCANE ) )
				SafePlayerCast( m_target )->m_modSpellCritdmgPCT -= (uint32)mod->m_amount;
		}
	}
}


void Aura::SpellAuraWaterBreathing( bool apply )
{
   if( m_target->IsPlayer() )
   {
	   if( apply )
	   {
			SetPositive();
			WorldPacket data( 4 );
			data.SetOpcode( SMSG_STOP_MIRROR_TIMER );
			data << uint32( TIMER_BREATH );
			SafePlayerCast( m_target )->GetSession()->SendPacket( &data );
//			SafePlayerCast( m_target )->m_UnderwaterState = 0;
	   }
	   else if( SafePlayerCast( m_target )->m_UnderwaterState & UNDERWATERSTATE_UNDERWATER )
	   {
			WorldPacket data(SMSG_START_MIRROR_TIMER, 20);
			data << uint32( TIMER_BREATH ) << SafePlayerCast( m_target )->m_UnderwaterTime << SafePlayerCast( m_target )->m_UnderwaterMaxTime << int32(-1) << uint8(0) << uint32(0);
			SafePlayerCast( m_target )->GetSession()->SendPacket(&data);
	   }

	   SafePlayerCast( m_target )->m_bUnlimitedBreath = apply;
   }
}

void Aura::SpellAuraAPAttackerBonus(bool apply)
{
	if(apply)
	{
		m_target->APvModifier += mod->m_amount;
	}
	else
		m_target->APvModifier -= mod->m_amount;
}


void Aura::SpellAuraModAttackPowerPct(bool apply)
{
	//!!probably there is a flag or something that will signal if randeg or melee attack power !!! (still missing)
//	if(m_target->IsPlayer())
	{
		if(apply)
		{
			m_target->ModFloatValue(UNIT_FIELD_ATTACK_POWER_MULTIPLIER,(float)mod->m_amount/100);
		}
		else
			m_target->ModFloatValue(UNIT_FIELD_ATTACK_POWER_MULTIPLIER,-(float)mod->m_amount/100);
		m_target->CalcDamage();
	}
}

void Aura::SpellAuraModRangedAttackPowerPct(bool apply)
{
    if(m_target->IsPlayer())
	{
		m_target->ModFloatValue(UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER,((apply)?1:-1)*(float)mod->m_amount/100);
        m_target->CalcDamage();
    }
}

void Aura::SpellAuraIncreaseDamageTypePCT(bool apply)
{
	if(m_target->IsPlayer())
	{
		if(apply)
		{
			for(uint32 x = 0; x < CREATURE_TYPES; x++)
				if (mod->m_miscValue & (((uint32)1)<<x) )
					SafePlayerCast( m_target )->IncreaseDamageByTypePCT[x+1] += ((float)(mod->m_amount))/100;;
			if(mod->m_amount < 0)
				SetNegative();
			else
				SetPositive();
		}
		else
		{
			for(uint32 x = 0; x < CREATURE_TYPES; x++)
			{
				if (mod->m_miscValue & (((uint32)1)<<x) )
					SafePlayerCast( m_target )->IncreaseDamageByTypePCT[x+1] -= ((float)(mod->m_amount))/100.0f;
			}
		}
	}
}

void Aura::SpellAuraIncreaseCricticalTypePCT(bool apply)
{
	if(m_target->IsPlayer())
	{
		if(apply)
		{
			for(uint32 x = 0; x < CREATURE_TYPES; x++)
				if (mod->m_miscValue & (((uint32)1)<<x) )
					SafePlayerCast( m_target )->IncreaseCricticalByTypePCT[x+1] += ((float)(mod->m_amount))/100.0f;
			if(mod->m_amount < 0)
				SetNegative();
			else
				SetPositive();
		}
		else
		{
			for(uint32 x = 0; x < CREATURE_TYPES; x++)
			{
				if (mod->m_miscValue & (((uint32)1)<<x) )
					SafePlayerCast( m_target )->IncreaseCricticalByTypePCT[x+1] -= ((float)(mod->m_amount))/100.0f;
			}
		}
	}
}

void Aura::SpellAuraIncreasePartySpeed(bool apply)
{
	if(m_target->IsPlayer() && m_target->isAlive() && m_target->GetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID) == 0)
	{
		if(apply)
		{
//			m_target->m_speedModifier += mod->m_amount;
			if( (GetSpellProto()->c_is_flags & SPELL_FLAG_IS_REQUIRING_SM_MOD) && m_target->SM_Mods)
			{
				int32 speedmod=0;
				SM_FIValue(m_target->SM_Mods->SM_FSpeedMod,&speedmod,GetSpellProto()->GetSpellGroupType());
				mod->m_amount += speedmod;
			}
			m_target->speedIncreaseMap.push_front( mod );
		}
	}
	if( apply == false )
	{
//		m_target->m_speedModifier -= mod->m_amount;
		m_target->speedIncreaseMap.remove( mod );
	}
	if( m_target->GetSpeedIncrease() )
		m_target->UpdateSpeed();
}

void Aura::SpellAuraIncreaseSpellDamageByAttribute(bool apply)
{
	if(m_target->IsPlayer() == false )
		return;

	int32 val;

	uint32 stat = m_spellProto->eff[mod->i].EffectMiscValueB;

	if( stat > 5 )
		stat = 3;

	if(apply)
	{
		val = mod->m_amount;
		if(val<0)
			SetNegative();
		else
			SetPositive();
	}
	else
		val = -mod->m_amount;

//	SafePlayerCast( m_target )->DamageDoneByStatPCT[ stat ] += val;
	SafePlayerCast( m_target )->ModStatToSpellPower( stat, val );
	SafePlayerCast( m_target )->CalcStat( stat );

//	for(uint32 x=0;x<STAT_COUNT;x++)
//		if (mod->m_miscValue & (((uint32)1)<<x) )
//			SafePlayerCast( m_target )->DamageDoneByStatPCT[x] += val;
//	SafePlayerCast( m_target )->UpdateChanceFields();
}

void Aura::SpellAuraModSpellDamageByAP(bool apply)
{
/*	int32 val;

	if(apply)
	{
		Unit *u_caster = GetUnitCaster();
		if( u_caster == NULL )
			return;
		val = mod->m_amount * u_caster->GetAP() / 100;
		if(val<0)
			SetNegative();
		else
			SetPositive();

		mod->fixed_amount[ 0 ] = val; //we wish to have the same amount when we are removing the spell as when we were applying !
	}
	else
		val = -mod->fixed_amount[ 0 ];

	if(m_target->IsPlayer())
	{
		for(uint32 x=0;x<SCHOOL_COUNT;x++)
			if (mod->m_miscValue & (((uint32)1)<<x) )
				m_target->ModUnsigned32Value( PLAYER_FIELD_MOD_DAMAGE_DONE_POS + x, val );
		SafePlayerCast( m_target )->UpdateChanceFields();
	} */
	if( m_target->IsPlayer() )
	{
		if( apply )
//			SafePlayerCast( m_target )->AP_to_SP_coeff += mod->m_amount / 100.0f;
			SafePlayerCast( m_target )->ModAttackPowerToSpellPower( mod->m_amount );
		else
//			SafePlayerCast( m_target )->AP_to_SP_coeff -= mod->m_amount / 100.0f;
			SafePlayerCast( m_target )->ModAttackPowerToSpellPower( -mod->m_amount );
		SafePlayerCast( m_target )->UpdateAttackPowerToSpellPower();
	}
}

void Aura::SpellAuraIncreaseHealingByAttribute(bool apply)
{
	int32 val;

	uint32 stat = m_spellProto->eff[mod->i].EffectMiscValue;
	if( stat > 5 )
		stat = 3;

	if( stat >=5 )
	{
		sLog.outError("Aura::SpellAuraIncreaseHealingByAttribute::Unknown spell attribute type %u in spell %u.\n",mod->m_miscValue,GetSpellId());
		return;
	}

	if(apply)
	{
		//we wish to have the same amount when we are removing the spell as when we were applying !
		val = mod->m_amount;
		if(val<0)
			SetNegative();
		else
			SetPositive();
	}
	else
		val = -mod->m_amount;

	if(m_target->IsPlayer())
	{
		SafePlayerCast( m_target )->SpellHealDoneByAttribute[stat] += ((float)(val))/100.0f;
		
		//this will never reflect instant changes
		if( apply )
		{
			mod->fixed_amount[ mod->i ] = val * m_target->GetUInt32Value(UNIT_FIELD_STAT0 + stat) / 100 ;
			m_target->ModUnsigned32Value(PLAYER_FIELD_MOD_HEALING_DONE_POS, mod->fixed_amount[ mod->i ]);
		}
		else
			m_target->ModUnsigned32Value(PLAYER_FIELD_MOD_HEALING_DONE_POS, -mod->fixed_amount[ mod->i ]);

		SafePlayerCast( m_target )->UpdateChanceFields();
	}
}

void Aura::SpellAuraModHealingByAP(bool apply)
{
/*	int32 val;

	if(apply)
	{
		Unit *u_caster = GetUnitCaster();
		if( u_caster == NULL )
			return;
		val = mod->m_amount * u_caster->GetAP() / 100;
		if(val<0)
			SetNegative();
		else
			SetPositive();

		mod->fixed_amount[0] = val; //we wish to have the same amount when we are removing the spell as when we were applying !
	}
	else
		val = -mod->fixed_amount[0];

	if(m_target->IsPlayer())
	{
		m_target->HealDoneMod += val;
		m_target->ModUnsigned32Value(PLAYER_FIELD_MOD_HEALING_DONE_POS, val);
//		SafePlayerCast( m_target )->UpdateChanceFields();
	}
	*/
	if( m_target->IsPlayer() )
	{
		if( apply )
			SafePlayerCast( m_target )->AP_to_HP_coeff += mod->m_amount / 100.0f;
		else
			SafePlayerCast( m_target )->AP_to_HP_coeff -= mod->m_amount / 100.0f;
	}
}

void Aura::SpellAuraAddFlatModifier(bool apply)
{
	//seems like blizz does not add the extra "1" here as it does for almost all other effects
	int32 val = apply?( mod->m_amount):-( mod->m_amount);
	uint32 *AffectedGroups = GetSpellProto()->eff[mod->i].EffectSpellGroupRelation;

	if( m_target->SM_Mods == NULL )
		m_target->SM_Mods = new UnitSMMods;
//	sLog.outDebug("%s: AffectedGroups %I64x smt type %u\n", __FUNCTION__, AffectedGroups, mod->m_miscValue);
	switch (mod->m_miscValue)//let's generate warnings for unknown types of modifiers
	{
	case SMT_CRITICAL:
		SendModifierLog(&m_target->SM_Mods->SM_FCriticalChance,val,AffectedGroups,mod->m_miscValue);
		break;

	case SMT_DURATION:
		SendModifierLog(&m_target->SM_Mods->SM_FDur,val,AffectedGroups,mod->m_miscValue);
		break;

	case SMT_RADIUS:
		SendModifierLog(&m_target->SM_Mods->SM_FRadius,val,AffectedGroups,mod->m_miscValue);
		break;

	case SMT_RANGE:
		SendModifierLog(&m_target->SM_Mods->SM_FRange,val,AffectedGroups,mod->m_miscValue);
		break;

	case SMT_CAST_TIME:
		SendModifierLog(&m_target->SM_Mods->SM_FCastTime,val,AffectedGroups,mod->m_miscValue);
		break;

	case SMT_COST:
		SendModifierLog(&m_target->SM_Mods->SM_FCost,val,AffectedGroups,mod->m_miscValue);
		break;

	case SMT_SPELL_VALUE_DOT:
		SendModifierLog(&m_target->SM_Mods->SM_FDOT,val,AffectedGroups,mod->m_miscValue);
		break;

	case SMT_ADDITIONAL_TARGET:
		SendModifierLog(&m_target->SM_Mods->SM_FAdditionalTargets,val,AffectedGroups,mod->m_miscValue);
		break;

	case SMT_DAMAGE_DONE:
		SendModifierLog(&m_target->SM_Mods->SM_FDamageBonus,val,AffectedGroups,mod->m_miscValue);
		break;

	case SMT_EFFECT_BONUS:
		SendModifierLog(&m_target->SM_Mods->SM_FEffectBonus,val,AffectedGroups,mod->m_miscValue,true);
		break;

	case SMT_EFFECT_TARGET:
		SendModifierLog(&m_target->SM_Mods->SM_FEffectTargetBonus,val,AffectedGroups,mod->m_miscValue,true);
		break;

	case SMT_MISC_EFFECT:
		SendModifierLog(&m_target->SM_Mods->SM_FMiscEffect,val,AffectedGroups,mod->m_miscValue);
		break;

	case SMT_HITCHANCE:
		SendModifierLog(&m_target->SM_Mods->SM_FHitchance,val,AffectedGroups,mod->m_miscValue);
		break;

		// as far as I know its not yet used!!!
	case SMT_PENALTY:
//		SendModifierLog(&m_target->SM_FPenalty,val,AffectedGroups,mod->m_miscValue);
		//all values seem to be pct. Based of 8/8 spells
		SendModifierLog(&m_target->SM_Mods->SM_PPenalty,val,AffectedGroups,mod->m_miscValue,true);
		break;

	case SMT_COOLDOWN_DECREASE:
		SendModifierLog(&m_target->SM_Mods->SM_FCooldownTime, val, AffectedGroups,mod->m_miscValue);
		break;
	case SMT_GLOBAL_COOLDOWN:
		SendModifierLog(&m_target->SM_Mods->SM_FGlobalCooldownTime, val, AffectedGroups,mod->m_miscValue);
		break;

	case SMT_TRIGGER:
		SendModifierLog(&m_target->SM_Mods->SM_FChanceOfSuccess,val,AffectedGroups,mod->m_miscValue);
		break;

	case SMT_CHARGES:
		SendModifierLog(&m_target->SM_Mods->SM_FCharges, val, AffectedGroups,mod->m_miscValue);
		break;
	case SMT_THREAT_REDUCED:
		SendModifierLog(&m_target->SM_Mods->SM_FThreat, val, AffectedGroups,mod->m_miscValue);
		break;
/*	case SMT_TRIGGER:*/
	case SMT_TIME:
		SendModifierLog(&m_target->SM_Mods->SM_FTime, val, AffectedGroups, mod->m_miscValue);
		break;
	case SMT_RESIST_DISPEL:
		SendModifierLog(&m_target->SM_Mods->SM_FRezist_dispell, val, AffectedGroups,mod->m_miscValue);
		break;
	//!!!!!!!! these are custom
	case SMT_ADD_TO_EFFECT_VALUE_1:
		SendModifierLog(&m_target->SM_Mods->SM_FAddEffect1, val, AffectedGroups,SMT_MISC_EFFECT);
		break;
	case SMT_ADD_TO_EFFECT_VALUE_2:
		SendModifierLog(&m_target->SM_Mods->SM_FAddEffect2, val, AffectedGroups,SMT_MISC_EFFECT);
		break;
	case SMT_ADD_TO_EFFECT_VALUE_3:
		SendModifierLog(&m_target->SM_Mods->SM_FAddEffect3, val, AffectedGroups,SMT_MISC_EFFECT);
		break;
	case SMT_ADD_TO_EFFECT_VALUE_DUMMY:
		SendModifierLog(&m_target->SM_Mods->SM_FAddEffect_DUMMY, val, AffectedGroups, GetSpellProto()->eff[mod->i].EffectMiscValueB );
		break;
	default://unknown Modifier type
		sLog.outError("Unknown spell modifier type %u in spell %u.<<--report this line to the developer\n",	mod->m_miscValue,GetSpellId());
		//don't add val, though we could formaly could do,but as we don't know what it is-> no sense
		break;
	}
	//Hunter's BeastMastery talents.
/*	if( m_target->IsPlayer() )
	{
		Pet * p = SafePlayerCast(m_target)->GetSummon();
		if( p )
		{
			switch( GetSpellProto()->NameHash )
			{
			case SPELL_HASH_UNLEASHED_FURY:
				p->LoadPetAuras(0);
				break;
			case SPELL_HASH_THICK_HIDE:
				p->LoadPetAuras(1);
				break;
			case SPELL_HASH_ENDURANCE_TRAINING:
				p->LoadPetAuras(2);
				break;
//			case SPELL_HASH_BESTIAL_SWIFTNESS:
//				p->LoadPetAuras(3);
//				break;
			case SPELL_HASH_BESTIAL_DISCIPLINE:
				p->LoadPetAuras(4);
				break;
			case SPELL_HASH_FEROCITY:
				p->LoadPetAuras(5);
				break;
			case SPELL_HASH_ANIMAL_HANDLER:
				p->LoadPetAuras(6);
				break;
			case SPELL_HASH_CATLIKE_REFLEXES:
				p->LoadPetAuras(7);
				break;
			case SPELL_HASH_SERPENT_S_SWIFTNESS:
				p->LoadPetAuras(8);
				break;
			}
		}
	}*/
}


void Aura::SpellAuraModHealingDone(bool apply)
{
	int32 val;

	if( m_target->getClass() == DEATHKNIGHT )
	{ 
		return;
	}

	if(apply)
	{
		val=mod->m_amount;
		if(val<0)
			SetNegative();
		else
			SetPositive();
	}
	else
		val = -mod->m_amount;

//	for(uint32 x=0;x<SCHOOL_COUNT;x++)
//		if (mod->m_miscValue  & (((uint32)1)<<x) )
//			m_target->HealDoneMod[x] += val;
	m_target->HealDoneMod += val;

	if(m_target->IsPlayer())
	{
		SafePlayerCast( m_target )->UpdateChanceFields();
		m_target->SetUInt32Value(PLAYER_FIELD_MOD_HEALING_DONE_POS, m_target->GetUInt32Value(PLAYER_FIELD_MOD_HEALING_DONE_POS) + val);
	}
}

void Aura::SpellAuraModHealingDonePct(bool apply)
{
	int32 val;
	if(apply)
	{
		val=mod->m_amount;
		if(val<0)
			SetNegative();
		else
			SetPositive();
	}
	else
		val=-mod->m_amount;

/*	for(uint32 x=0;x<SCHOOL_COUNT;x++)
	{
		if (mod->m_miscValue  & (((uint32)1)<<x) )
		{
			m_target->HealDonePctMod[x] += val;
		}
	}*/
	m_target->HealDonePctMod += val;
	Player *p_target;
	if( m_target->IsPlayer() )
		p_target = SafePlayerCast( m_target );
	else
		p_target = NULL;
	if( p_target )
	{
		//i wonder which one of these 2 needs to be modded
		p_target->SetFloatValue( PLAYER_FIELD_MOD_HEALING_DONE_PCT,(100 + m_target->HealDonePctMod ) / 100.0f );
	}
}

void Aura::SpellAuraEmphaty(bool apply)
{
	SetPositive();
	Unit * caster = GetUnitCaster();
	if(caster == 0 || !m_target || caster->GetTypeId() != TYPEID_PLAYER)
	{ 
		return;
	}

	// Show extra info about beast
	uint32 dynflags = m_target->GetUInt32Value(UNIT_DYNAMIC_FLAGS);
	if(apply)
		dynflags |= U_DYN_FLAG_PLAYER_INFO;

	m_target->BuildFieldUpdatePacket(SafePlayerCast(caster), UNIT_DYNAMIC_FLAGS, dynflags);
}

void Aura::SpellAuraModOffhandDamagePCT(bool apply)
{
	//Used only by talents of rogue and warrior;passive,positive
	if(m_target->IsPlayer())
	{
		if(apply)
		{
			SetPositive();
			SafePlayerCast( m_target )->offhand_dmg_mod *= (100+mod->m_amount)/100.0f;
		}
		else
			SafePlayerCast( m_target )->offhand_dmg_mod /= (100+mod->m_amount)/100.0f;

		m_target->CalcDamage();
	}
}

void Aura::SpellAuraModPenetration(bool apply) // armor penetration & spell penetration
{
	if( m_spellProto->NameHash == SPELL_HASH_SERRATED_BLADES )
	{
		if(!m_target->IsPlayer())
		{ 
			return;
		}

		Player *plr = SafePlayerCast(m_target);
		if( apply )
		{
			mod->fixed_amount[ 0 ] = 0;
			if( m_spellProto->Id == 14171 )
				mod->fixed_amount[ 0 ] = m_target->getLevel() * 267;
			else if( m_spellProto->Id == 14172 )
				mod->fixed_amount[ 0 ] =  m_target->getLevel() * 543;
			else if( m_spellProto->Id == 14173 )
				mod->fixed_amount[ 0 ] =  m_target->getLevel() * 800;
			plr->TargetResistRedModFlat[0] += mod->fixed_amount[ 0 ] / 100.0f;
		}
		else
		{
			plr->TargetResistRedModFlat[0] -= mod->fixed_amount[ 0 ] / 100.0f;
		}
		return;
	}
	if(apply)
	{
		if(mod->m_amount < 0)
			SetPositive();
		else
			SetNegative();

		for(uint32 x=0;x<SCHOOL_COUNT;x++)
		{
			if (mod->m_miscValue & (((uint32)1)<<x))
				m_target->TargetResistRedModFlat[x] -= mod->m_amount;
		}

		if(m_target->IsPlayer()){
			if(mod->m_miscValue & 124)
				m_target->ModSignedInt32Value(PLAYER_FIELD_MOD_TARGET_RESISTANCE, mod->m_amount);
			if(mod->m_miscValue & 1)
				m_target->ModSignedInt32Value(PLAYER_FIELD_MOD_TARGET_PHYSICAL_RESISTANCE, mod->m_amount);
		}
	}
	else
	{
		for(uint32 x=0;x<SCHOOL_COUNT;x++)
		{
			if (mod->m_miscValue & (((uint32)1)<<x))
				m_target->TargetResistRedModFlat[x] += mod->m_amount;
		}
		if(m_target->IsPlayer()){
			if(mod->m_miscValue & 124)
				m_target->ModSignedInt32Value(PLAYER_FIELD_MOD_TARGET_RESISTANCE, -mod->m_amount);
			if(mod->m_miscValue & 1)
				m_target->ModSignedInt32Value(PLAYER_FIELD_MOD_TARGET_PHYSICAL_RESISTANCE, -mod->m_amount);
		}
	}
}

void Aura::SpellAuraIncreaseArmorByPctInt(bool apply)
{
	uint32 i_Int = m_target->GetUInt32Value(UNIT_FIELD_STAT3);

	int32 amt = float2int32(i_Int * ((float)mod->m_amount / 100.0f));
	amt *= (!apply) ? -1 : 1;

	for(uint32 x=0;x<SCHOOL_COUNT;x++)
	{
		if(mod->m_miscValue & (((uint32)1)<< x))
		{
			if(m_target->IsPlayer())
			{
				SafePlayerCast( m_target )->FlatResistanceModifierPos[x] += amt;
				SafePlayerCast( m_target )->CalcResistance(x);
			}
			else if(m_target->IsCreature())
			{
				SafeCreatureCast(m_target)->FlatResistanceMod[x] += amt;
				SafeCreatureCast(m_target)->CalcResistance(x);
			}
		}
	}
}

void Aura::SpellAuraReduceAttackerMHitChance(bool apply)
{
	if (!m_target->IsPlayer())
	{ 
		return;
	}
	if(apply)
	{
		SafePlayerCast( m_target )->m_resist_hit[ MOD_MELEE ] += mod->m_amount;
		//in 4.2 client this seems to affect ranged also
		SafePlayerCast( m_target )->m_resist_hit[ MOD_RANGED ] += mod->m_amount;
	}
	else
	{
		SafePlayerCast( m_target )->m_resist_hit[ MOD_MELEE ] -= mod->m_amount;
		//in 4.2 client this seems to affect ranged also
		SafePlayerCast( m_target )->m_resist_hit[ MOD_RANGED ] -= mod->m_amount;
	}
}

void Aura::SpellAuraReduceAttackerRHitChance(bool apply)
{
	if (!m_target->IsPlayer())
	{ 
		return;
	}
	if(apply)
		SafePlayerCast( m_target )->m_resist_hit[ MOD_RANGED ] += mod->m_amount;
	else
		SafePlayerCast( m_target )->m_resist_hit[ MOD_RANGED ] -= mod->m_amount;
}

void Aura::SpellAuraReduceAttackerSHitChance(bool apply)
{
	if (!m_target->IsPlayer())
	{ 
		return;
	}
	if(apply)
		SafePlayerCast( m_target )->m_resist_hit[ MOD_SPELL ] -= mod->m_amount;
	else
		SafePlayerCast( m_target )->m_resist_hit[ MOD_SPELL ] += mod->m_amount;
}

void Aura::SpellAuraReduceEnemyMCritChance(bool apply)
{
	if(!m_target->IsPlayer())
	{ 
		return;
	}
	if(apply)
	{
		//value is negative percent
		SafePlayerCast( m_target )->res_M_crit_set(SafePlayerCast( m_target )->res_M_crit_get()+mod->m_amount);
	}
	else
	{
		SafePlayerCast( m_target )->res_M_crit_set(SafePlayerCast( m_target )->res_M_crit_get()-mod->m_amount);
	}
}

void Aura::SpellAuraReduceEnemyRCritChance(bool apply)
{
	if(!m_target->IsPlayer())
	{ 
		return;
	}
	if(apply)
	{
		//value is negative percent
		SafePlayerCast( m_target )->res_R_crit_set(SafePlayerCast( m_target )->res_R_crit_get()+mod->m_amount);
	}
	else
	{
		SafePlayerCast( m_target )->res_R_crit_set(SafePlayerCast( m_target )->res_R_crit_get()-mod->m_amount);
	}
}

void Aura::SpellAuraLimitSpeed( bool apply )
{
//	int32 amount = ( apply ) ? mod->m_amount : -mod->m_amount; //bugs out sometimes
	if( apply )
		m_target->m_maxSpeed += (float)mod->m_amount; //this should be a list from which min should be used
	else
		m_target->m_maxSpeed = 0;
	m_target->UpdateSpeed();
}
/*
void Aura::SpellAuraIncreaseTimeBetweenAttacksPCT(bool apply)
{
	//haste is applied as : castspeed / ( 1 + hastebonus ) 
	if( p_target )
	{
		if(apply)
			p_target->ModAttackSpeed( mod->m_amount, MOD_SPELL );
		else
			p_target->ModAttackSpeed( -mod->m_amount, MOD_SPELL );
	}
	else 
	{
		if(apply)
			m_target->ModFloatValue(UNIT_MOD_CAST_SPEED, 1.0f/(1.0f+mod->m_amount/100.0f) );
		else
			m_target->ModFloatValue(UNIT_MOD_CAST_SPEED, -1.0f/(1.0f+mod->m_amount/100.0f) );
	}
}*/

void Aura::SpellAuraMeleeHaste( bool apply )
{
	if( mod->m_amount < 0 )
		SetNegative();
	else 
		SetPositive();
	
	if( m_target->IsPlayer() )
	{
		if( apply )
			SafePlayerCast( m_target )->ModAttackSpeed( mod->m_amount, MOD_MELEE );
		else
			SafePlayerCast( m_target )->ModAttackSpeed( -mod->m_amount, MOD_MELEE );
	}
	else
	{
		if( apply )
		{
			mod->fixed_amount[0] = m_target->GetModPUInt32Value( UNIT_FIELD_BASEATTACKTIME, mod->m_amount );
			mod->fixed_amount[1] = m_target->GetModPUInt32Value( UNIT_FIELD_BASEATTACKTIME_1, mod->m_amount );

			if( (int32)m_target->GetUInt32Value ( UNIT_FIELD_BASEATTACKTIME ) <= mod->fixed_amount[0] )
				mod->fixed_amount[0] = m_target->GetUInt32Value ( UNIT_FIELD_BASEATTACKTIME );
			if( (int32)m_target->GetUInt32Value ( UNIT_FIELD_BASEATTACKTIME_1 ) <= mod->fixed_amount[1] )
				mod->fixed_amount[1] = m_target->GetUInt32Value ( UNIT_FIELD_BASEATTACKTIME_1 );

			m_target->ModUnsigned32Value( UNIT_FIELD_BASEATTACKTIME, -mod->fixed_amount[0] );
			m_target->ModUnsigned32Value( UNIT_FIELD_BASEATTACKTIME_1, -mod->fixed_amount[1] );
		}
		else
		{
			m_target->ModUnsigned32Value( UNIT_FIELD_BASEATTACKTIME, mod->fixed_amount[0] );
			m_target->ModUnsigned32Value( UNIT_FIELD_BASEATTACKTIME_1, mod->fixed_amount[1] );
		}
	}
}

/*
void Aura::SpellAuraIncreaseSpellDamageByInt(bool apply)
{
	 float val;
	 if(apply)
	 {
		 val = mod->m_amount/100.0f;
		 if(mod->m_amount>0)
			 SetPositive();
		 else
			 SetNegative();
	 }
	 else
		val =- mod->m_amount/100.0f;

	if(m_target->IsPlayer())
	{
		for(uint32 x=1;x<SCHOOL_COUNT;x++)
		{
			if (mod->m_miscValue & (((uint32)1)<<x) )
			{
				SafePlayerCast( m_target )->SpellDmgDoneByInt[x]+=val;
			}
		}
	}
}

void Aura::SpellAuraIncreaseHealingByInt(bool apply)
{
	 float val;
	 if(apply)
	 {
		 val = mod->m_amount/100.0f;
		 if(val>0)
			 SetPositive();
		 else
			 SetNegative();
	 }
	 else
		val =- mod->m_amount/100.0f;

	if(m_target->IsPlayer())
	{
		for(uint32 x=1;x<SCHOOL_COUNT;x++)
		{
   //		 if (mod->m_miscValue & (((uint32)1)<<x) )
			{
				SafePlayerCast( m_target )->SpellHealDoneByInt[x]+=val;
			}
		}
	}
}
*/
void Aura::SpellAuraModAttackerCritChance(bool apply)
{
	int32 val  = (apply) ? mod->m_amount : -mod->m_amount;
	for(uint32 x=0;x<SCHOOL_COUNT;x++)
		if (mod->m_miscValue & (((uint32)1)<<x) )
			m_target->AttackerCritChanceMod[x] +=val;
}

void Aura::SpellAuraIncreaseAllWeaponSkill(bool apply)
{
	if (m_target->IsPlayer())
	{
		if(apply)
		{
			SetPositive();
//			SafePlayerCast( m_target )->ModSkillBonusType(SKILL_TYPE_WEAPON, mod->m_amount);
			//since the frikkin above line does not work we have to do it manually
			SafePlayerCast( m_target )->_ModifySkillBonus(SKILL_SWORDS, mod->m_amount);
			SafePlayerCast( m_target )->_ModifySkillBonus(SKILL_AXES, mod->m_amount);
			SafePlayerCast( m_target )->_ModifySkillBonus(SKILL_BOWS, mod->m_amount);
			SafePlayerCast( m_target )->_ModifySkillBonus(SKILL_GUNS, mod->m_amount);
			SafePlayerCast( m_target )->_ModifySkillBonus(SKILL_MACES, mod->m_amount);
			SafePlayerCast( m_target )->_ModifySkillBonus(SKILL_2H_SWORDS, mod->m_amount);
			SafePlayerCast( m_target )->_ModifySkillBonus(SKILL_STAVES, mod->m_amount);
			SafePlayerCast( m_target )->_ModifySkillBonus(SKILL_2H_MACES, mod->m_amount);
			SafePlayerCast( m_target )->_ModifySkillBonus(SKILL_2H_AXES, mod->m_amount);
			SafePlayerCast( m_target )->_ModifySkillBonus(SKILL_DAGGERS, mod->m_amount);
			SafePlayerCast( m_target )->_ModifySkillBonus(SKILL_CROSSBOWS, mod->m_amount);
//			SafePlayerCast( m_target )->_ModifySkillBonus(SKILL_SPEARS, mod->m_amount);
			SafePlayerCast( m_target )->_ModifySkillBonus(SKILL_WANDS, mod->m_amount);
			SafePlayerCast( m_target )->_ModifySkillBonus(SKILL_POLEARMS, mod->m_amount);
		}
		else
		{
//			SafePlayerCast( m_target )->ModSkillBonusType(SKILL_TYPE_WEAPON, -mod->m_amount);
			SafePlayerCast( m_target )->_ModifySkillBonus(SKILL_SWORDS, -mod->m_amount);
			SafePlayerCast( m_target )->_ModifySkillBonus(SKILL_AXES, -mod->m_amount);
			SafePlayerCast( m_target )->_ModifySkillBonus(SKILL_BOWS, -mod->m_amount);
			SafePlayerCast( m_target )->_ModifySkillBonus(SKILL_GUNS, -mod->m_amount);
			SafePlayerCast( m_target )->_ModifySkillBonus(SKILL_MACES, -mod->m_amount);
			SafePlayerCast( m_target )->_ModifySkillBonus(SKILL_2H_SWORDS, -mod->m_amount);
			SafePlayerCast( m_target )->_ModifySkillBonus(SKILL_STAVES, -mod->m_amount);
			SafePlayerCast( m_target )->_ModifySkillBonus(SKILL_2H_MACES, -mod->m_amount);
			SafePlayerCast( m_target )->_ModifySkillBonus(SKILL_2H_AXES, -mod->m_amount);
			SafePlayerCast( m_target )->_ModifySkillBonus(SKILL_DAGGERS, -mod->m_amount);
			SafePlayerCast( m_target )->_ModifySkillBonus(SKILL_CROSSBOWS, -mod->m_amount);
//			SafePlayerCast( m_target )->_ModifySkillBonus(SKILL_SPEARS, -mod->m_amount);
			SafePlayerCast( m_target )->_ModifySkillBonus(SKILL_WANDS, -mod->m_amount);
			SafePlayerCast( m_target )->_ModifySkillBonus(SKILL_POLEARMS, -mod->m_amount);
		}

		SafePlayerCast( m_target )->UpdateStats();
	}
}

void Aura::SpellAuraIncreaseHitRate( bool apply )
{
	if( !m_target->IsPlayer() )
	{ 
		return;
	}
	
	SafePlayerCast( m_target )->ModifyBonuses( SPELL_HIT_RATING, mod->m_amount, apply );
	SafePlayerCast( m_target )->UpdateStats();
}

void Aura::SpellAuraIncreaseRageFromDamageDealtPCT(bool apply)
{
	if(!m_target->IsPlayer())
	{ 
		return;
	}

	SafePlayerCast( m_target )->rageFromDamageDealt += (apply) ? mod->m_amount : -mod->m_amount;
}

int32 Aura::event_GetInstanceID()
{
	return m_target->event_GetInstanceID();
}

void Aura::SpellAuraReduceCritMeleeAttackDmg(bool apply)
{
	if(!m_target)
	{ 
		return;
	}

	signed int val;
	if(apply)
		val = mod->m_amount;
	else
		val = -mod->m_amount;

	for(uint32 x=0;x<SCHOOL_COUNT;x++)
		if (mod->m_miscValue & (((uint32)1)<<x) )
			m_target->CritMeleeDamageTakenPctMod[x] += val/100.0f;
}

void Aura::SpellAuraReduceCritRangedAttackDmg(bool apply)
{
	if(!m_target)
	{ 
		return;
	}

	signed int val;
	if(apply)
		val = mod->m_amount;
	else
		val = -mod->m_amount;

	for(uint32 x=0;x<SCHOOL_COUNT;x++)
		if (mod->m_miscValue & (((uint32)1)<<x) )
			m_target->CritRangedDamageTakenPctMod[x] += val/100.0f;
}

void Aura::SpellAuraEnableFlight(bool apply)
{
	if(apply)
	{
		if( m_target->GetMapMgr() && CAN_USE_FLYINGMOUNT( m_target->GetMapMgr() ) == 0 )
		{
			m_flags |= 1 << mod->i;
			return;
		}
		m_target->EnableFlight();
		m_target->m_flyspeedModifier += mod->m_amount;
		m_target->UpdateSpeed();
		if(m_target->IsPlayer())
		{
			SafePlayerCast( m_target )->flying_aura = m_spellProto->Id;
			SafePlayerCast( m_target )->SetMovement( MOVE_WATER_WALK );
		}
	}
	else if( (m_flags & (1 << mod->i)) == 0 )
	{
		m_target->DisableFlight();
		m_target->m_flyspeedModifier -= mod->m_amount;
		m_target->UpdateSpeed();
		if(m_target->IsPlayer())
		{
			SafePlayerCast( m_target )->flying_aura = 0;
			SafePlayerCast( m_target )->SetMovement( MOVE_LAND_WALK );
		}
	}
}

void Aura::SpellAuraEnableFlightWithUnmountedSpeed(bool apply)
{
	// Used in flight form (only so far)
	if(apply)
	{
		if( m_target->GetMapMgr() && CAN_USE_FLYINGMOUNT( m_target->GetMapMgr() ) == 0 )
		{
			m_flags |= 1 << mod->i;
			return;
		}
		m_target->EnableFlight();
		m_target->m_flyspeedModifier += mod->m_amount;
		m_target->UpdateSpeed();
		if(m_target->IsPlayer())
		{
			SafePlayerCast( m_target )->flying_aura = m_spellProto->Id;
		}
	}
	else if( (m_flags & (1 << mod->i)) == 0 )
	{
		m_target->DisableFlight();
		m_target->m_flyspeedModifier -= mod->m_amount;
		m_target->UpdateSpeed();
		if(m_target->IsPlayer())
		{
			SafePlayerCast( m_target )->flying_aura = 0;
		}
	}
}

void Aura::SpellAuraIncreaseMovementAndMountedSpeed( bool apply )
{
	if( apply )
		m_target->m_mountedspeedModifier += mod->m_amount;
	else
		m_target->m_mountedspeedModifier -= mod->m_amount;
	m_target->UpdateSpeed();
}

void Aura::SpellAuraIncreaseFlightSpeed( bool apply )
{
	if( apply )
		m_target->m_flyspeedModifier += mod->m_amount;
	else
		m_target->m_flyspeedModifier -= mod->m_amount;
	m_target->UpdateSpeed();
}


void Aura::SpellAuraIncreaseRating( bool apply )
{
	int v = apply ? mod->m_amount : -mod->m_amount;

	if( !m_target->IsPlayer() )
	{ 
		return;
	}

	// WEAPON_SKILL_RATING = 11 => EXPERTISE_RATING_2 = 37
	static const uint32 translate_to_item[26] = 
	{
		WEAPON_SKILL_RATING,
		DEFENSE_RATING,
		DODGE_RATING,
		PARRY_RATING,
		SHIELD_BLOCK_RATING,
		MELEE_HIT_RATING,
		RANGED_HIT_RATING,
		SPELL_HIT_RATING,
		MELEE_CRITICAL_STRIKE_RATING,
		RANGED_CRITICAL_STRIKE_RATING,
		SPELL_CRITICAL_STRIKE_RATING,
		RESILIENCE_RATING,	//MELEE_HIT_AVOIDANCE_RATING,
		RESILIENCE_RATING,	//RANGED_HIT_AVOIDANCE_RATING,
		RESILIENCE_RATING,	//SPELL_HIT_AVOIDANCE_RATING,
		MELEE_CRITICAL_AVOIDANCE_RATING,
		RANGED_CRITICAL_AVOIDANCE_RATING,
		SPELL_CRITICAL_AVOIDANCE_RATING,
		MELEE_HASTE_RATING,
		RANGED_HASTE_RATING,
		SPELL_HASTE_RATING,
		0,		//PLAYER_RATING_MODIFIER_MELEE_MAIN_HAND_SKILL
		0,		//PLAYER_RATING_MODIFIER_MELEE_OFF_HAND_SKILL
		HIT_AVOIDANCE_RATING,
		EXPERTISE_RATING,
		ARMOR_PENETRATION_RATING,
		ITEM_MOD_MASTERY_RATING
	};
	uint32 mask = mod->m_miscValue;
	Player* plr = SafePlayerCast( m_target );
	//MELEE_CRITICAL_AVOIDANCE_RATING + RANGED_CRITICAL_AVOIDANCE_RATING + SPELL_CRITICAL_AVOIDANCE_RATING
	//comes only as combination of them  - ModifyBonuses() not adding them individually anyhow
/*	if( ( mod->m_miscValue & (0x0004000|0x0008000|0x0010000) ) == (0x0004000|0x0008000|0x0010000) )
	{
			plr->ModifyBonuses( RESILIENCE_RATING, mod->m_amount, apply );
			mask &= ~(0x0004000|0x0008000|0x0010000);
	}*/

	//melee critical rating has been removed ? and merged with ranged critical rating ?
	if( ( ( 1 << 14 ) & mask ) )
	{
		mask |= ( 1 << 15 );
		mask &= ~( 1 << 14 );
	}
	//spell critical rating has been removed ? and merged with ranged critical rating ?
	if( ( ( 1 << 16 ) & mask ) )
	{
		mask |= ( 1 << 15 );
		mask &= ~( 1 << 16 );
	}

	//weapon skill is 0 and it is special
	for( uint32 x = 1; x <= 25; x++ )//skip x=0
		if( ( ( ( uint32 )1 ) << x ) & mask )
			plr->ModifyBonuses( translate_to_item[ x ], mod->m_amount, apply );

	//this should be depracated in cataclysm
	if( mod->m_miscValue & 1 )//weapon skill
	{
		std::map<uint32, uint32>::iterator i;
		for( uint32 y = 0; y < 20; y++ )
			if( m_spellProto->GetEquippedItemSubClass() & ( ( ( uint32 )1 ) << y ) )
			{
					i = SafePlayerCast( m_target )->m_wratings.find( y );
					if( i == SafePlayerCast( m_target )->m_wratings.end() )//no prev
					{
						SafePlayerCast( m_target )->m_wratings[y] = v;
					}else
					{
						i->second += v;
						if( i->second == 0 )
							SafePlayerCast( m_target )->m_wratings.erase( i );
					}
			}
	}

	plr->UpdateStats();
}

void Aura::EventPeriodicRegenManaStatPct(uint32 perc,uint32 stat)
{
	if(m_target->IsDead())
	{ 
		return;
	}

	//blizz says no
//	m_target->Energize( m_target, GetSpellId(), (m_target->GetUInt32Value(UNIT_FIELD_STAT0 + stat)*perc)/100, POWER_TYPE_MANA );

	uint32 mana = m_target->GetPower( POWER_TYPE_MANA ) + (m_target->GetUInt32Value(UNIT_FIELD_STAT0 + stat)*perc)/100;
	if(mana <= m_target->GetMaxPower( POWER_TYPE_MANA ))
		m_target->SetPower( POWER_TYPE_MANA, mana);
	else
		m_target->SetPower( POWER_TYPE_MANA, m_target->GetMaxPower( POWER_TYPE_MANA ));
	m_flags |= FIRST_TICK_ENDED;
}


void Aura::SpellAuraRegenManaStatPCT(bool apply)
{
	if(apply)
	{
		SetPositive();
		sEventMgr.AddEvent(this, &Aura::EventPeriodicRegenManaStatPct,(uint32)mod->m_amount,(uint32)mod->m_miscValue,  EVENT_AURA_REGEN_MANA_STAT_PCT, 5000, 0,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
	}
	else
	{
		//on aura remove all events are also removed.
	}
}
void Aura::SpellAuraRatingBasedOnStat(bool apply)
{
	//value based on stat
	int32 StatIndex = GetSpellProto()->eff[mod->i].EffectMiscValueB;
//	if( apply && StatIndex < STAT_COUNT )
//		mod->m_amount = m_target->GetUInt32Value( UNIT_FIELD_STAT0 + StatIndex ) * mod->m_amount / 100;

	int val = apply ? mod->m_amount : -mod->m_amount;

	if( !m_target->IsPlayer() )
	{ 
		return;
	}

	// WEAPON_SKILL_RATING = 11 => EXPERTISE_RATING_2 = 37
	Player* plr = SafePlayerCast( m_target );
//	for( uint32 x = 1; x <= EXPERTISE_RATING_2 - WEAPON_SKILL_RATING; x++ )//skip x=0
//		if( ( ( ( uint32 )1 ) << x ) & mod->m_miscValue )
//			plr->ModifyBonuses( WEAPON_SKILL_RATING + x, mod->m_amount, apply );
	for( uint32 x = WEAPON_SKILL_RATING + 1; x <= EXPERTISE_RATING_2; x++ )//skip x=0
	{
		int IndexAsOnPlayer = x - WEAPON_SKILL_RATING;
		if( ( ( ( uint32 )1 ) << IndexAsOnPlayer ) & mod->m_miscValue )
			plr->ModStatToRating( StatIndex, IndexAsOnPlayer, val );
	}

/*	//MELEE_CRITICAL_AVOIDANCE_RATING + RANGED_CRITICAL_AVOIDANCE_RATING + SPELL_CRITICAL_AVOIDANCE_RATING
	//comes only as combination of them  - ModifyBonuses() not adding them individually anyhow
	if( mod->m_miscValue & (0x0004000|0x0008000|0x0010000) )
			plr->ModifyBonuses( RESILIENCE_RATING, mod->m_amount, apply );

	if( mod->m_miscValue & 1 )//weapon skill
	{
		std::map<uint32, uint32>::iterator i;
		for( uint32 y = 0; y < 20; y++ )
			if( m_spellProto->GetEquippedItemSubClass() & ( ( ( uint32 )1 ) << y ) )
			{
					i = SafePlayerCast( m_target )->m_wratings.find( y );
					if( i == SafePlayerCast( m_target )->m_wratings.end() )//no prev
					{
						if( apply == true )
							SafePlayerCast( m_target )->m_wratings[y] = v;
					}else
					{
						i->second += v;
						if( i->second == 0 )
							SafePlayerCast( m_target )->m_wratings.erase( i );
					}
			}
	} */
	for(uint32 i = 0; i < STAT_COUNT; ++i)
		plr->CalcStat(i);

	plr->UpdateStats();
}

void Aura::SpellAuraFinishingMovesCannotBeDodged(bool apply)
{
	if(apply)
	{
		if( !m_target->IsPlayer() )
		{ 
			return;
		}

		SafePlayerCast( m_target )->m_finishingmovesdodge = true;
	}
	else
	{
		if( !m_target->IsPlayer() )
		{ 
			return;
		}

		SafePlayerCast( m_target )->m_finishingmovesdodge = false;
	}
}

void Aura::SpellAuraModStealthDetection(bool apply)
{
	if(apply)
	{
		m_target->m_stealthDetectBonus += 9999;
	}
	else
		m_target->m_stealthDetectBonus -= 9999;
}

void Aura::SpellAuraIncreaseMaxHealth(bool apply)
{
	int32 amount;
	if( apply )
		amount = mod->m_amount;
	else
		amount = -mod->m_amount;

	if( m_target->IsPlayer() )
	{
		SafePlayerCast( m_target )->SetHealthFromSpell( SafePlayerCast( m_target )->GetHealthFromSpell() + amount );
		SafePlayerCast( m_target )->UpdateStats();
	}
	else m_target->ModUnsigned32Value( UNIT_FIELD_MAXHEALTH, amount);
}

void Aura::SpellAuraSpiritOfRedemption(bool apply)
{
	if(!m_target->IsPlayer())
	{ 
		return;
	}

	if(apply)
	{
		m_target->SetFloatValue(OBJECT_FIELD_SCALE_X, 0.5);
		m_target->SetUInt32Value(UNIT_FIELD_HEALTH, 1);
	}
	else
	{
		m_target->SetFloatValue(OBJECT_FIELD_SCALE_X, 1);
		m_target->SetUInt32Value(UNIT_FIELD_HEALTH, 0);
	}
}

void Aura::SpellAuraIncreaseAttackerSpellCrit(bool apply)
{
	int32 val = mod->m_amount;

	if (apply)
	{
		if (mod->m_amount>0)
			SetNegative();
		else
			SetPositive();
	}
	else
		val = -val;

	for(uint32 x=0;x<SCHOOL_COUNT;x++)
	{
		if (mod->m_miscValue & (((uint32)1)<<x))
			m_target->AttackerCritChanceMod[x] += val;
	}
}

void Aura::SpellAuraIncreaseRepGainPct(bool apply)
{
	Player *p_target;
	if( m_target->IsPlayer() )
		p_target = SafePlayerCast( m_target );
	else
		p_target = NULL;
	if(p_target)
	{
		SetPositive();
		if(apply)
			p_target->pctReputationMod += mod->m_amount;//re use
		else
			p_target->pctReputationMod -= mod->m_amount;//re use
	}
}

void Aura::SpellAuraIncreaseExpGainPct(bool apply)
{
	Player *p_target;
	if( m_target->IsPlayer() )
		p_target = SafePlayerCast( m_target );
	else
		p_target = NULL;
	if(p_target)
	{
		SetPositive();
		if(apply)
			p_target->XPModBonusPCT += mod->m_amount;//re use
		else
			p_target->XPModBonusPCT -= mod->m_amount;//re use
	}
}

void Aura::SpellAuraIncreaseRangedAPStatPCT(bool apply)
{

	if( mod->m_miscValue > 5 )
	{ 
		return;
	}

	if( apply )
	{
		mod->fixed_amount[ 0 ] = m_target->GetUInt32Value( UNIT_FIELD_STAT0 + mod->m_miscValue ) * mod->m_amount / 100;
		if( mod->fixed_amount[ 0 ] > 0 )
		{
			SetPositive();
			m_target->ModUnsigned32Value( UNIT_FIELD_ATTACK_POWER_MOD_POS, mod->fixed_amount[ 0 ] );
		}
		else
		{
			SetNegative();
			m_target->ModUnsigned32Value( UNIT_FIELD_ATTACK_POWER_MOD_NEG, mod->fixed_amount[ 0 ] );
		}
	}
	else
	{
		if( mod->m_amount > 0 )
			m_target->ModUnsigned32Value( UNIT_FIELD_ATTACK_POWER_MOD_POS, -mod->fixed_amount[ 0 ] );
		else
			m_target->ModUnsigned32Value( UNIT_FIELD_ATTACK_POWER_MOD_NEG, -mod->fixed_amount[ 0 ] );
	}

	m_target->CalcDamage();
}

void Aura::SpellAuraModRangedDamageTakenPCT(bool apply)
{
	if(apply)
		m_target->RangedDamageTakenPct += mod->m_amount;
	else
		m_target->RangedDamageTakenPct -= mod->m_amount;
}

void Aura::SpellAuraModBlockValue(bool apply)
{
	Player *p_target;
	if( m_target->IsPlayer() )
		p_target = SafePlayerCast( m_target );
	else
		p_target = NULL;
	if( p_target != NULL)
 	{
		int32 amt;
 		if( apply )
 		{
			amt = mod->m_amount;
			if( amt < 0 )
				SetNegative();
			else
				SetPositive();
 		}
		else
		{
			amt = -mod->m_amount;
		}
		p_target->m_modblockvaluefromspells += amt;
		p_target->UpdateStats();
	}
}

void Aura::SendInterrupted(uint8 result, Object * m_caster)
{
	if( !m_caster->IsInWorld() )
	{ 
		return;
	}

	sStackWorldPacket( data, SMSG_SPELL_FAILURE, 20 );
	if( m_caster->IsPlayer() )
	{
		data << m_caster->GetNewGUID();
		data << m_spellProto->Id;
		data << uint8( result );
		SafePlayerCast( m_caster )->GetSession()->SendPacket( &data );
	}

	//Zack : is there a reason we need to send others spell fail reason ? Isn't this personal ?
	data.Initialize( SMSG_SPELL_FAILED_OTHER );
	data << m_caster->GetNewGUID();
	data << m_spellProto->Id;
	m_caster->SendMessageToSet( &data, true );
	/**/

//	m_interrupted = (int16)result;
}

void Aura::SendChannelUpdate(uint32 time, Object * m_caster)
{
	WorldPacket data(MSG_CHANNEL_UPDATE, 18);
	data << m_caster->GetNewGUID();
	data << time;

	m_caster->SendMessageToSet(&data, true);
}

void Aura::SpellAuraExpertieseModifier(bool apply)
{
	Player *p_target;
	if( m_target->IsPlayer() )
		p_target = SafePlayerCast( m_target );
	else
		p_target = NULL;
	if( !p_target )
	{ 
		return;
	}
	if( apply )
		p_target->m_ExpertiseMod += mod->m_amount;
	else
		p_target->m_ExpertiseMod -= mod->m_amount;
	p_target->UpdateStats();
}

void Aura::SpellAuraModPossessPet(bool apply)
{
	Unit *caster = GetUnitCaster();
	Player* pCaster;
	if( caster != NULL && caster->IsPlayer() && caster->IsInWorld() )
		pCaster = SafePlayerCast( caster );
	else
		return;

	if( !m_target->IsPet() || pCaster->GetSummon() != m_target )
	{ 
		return;
	}


	if( apply )
	{
		pCaster->Possess( m_target );
		pCaster->SpeedCheatDelay( GetDuration() );
	}
	else
	{
		pCaster->UnPossess();

	}
}

void Aura::SpellAuraReduceEffectDuration(bool apply)
{
	if( apply )
		SetPositive();

	if(mod->m_miscValue > 0 && mod->m_miscValue < MECHANIC_TOTAL)
	{
		//get the max from all auras possible
		int32 MaxValue = 0;
		int32 MinValue = 0;
		//on apply this buff is not yet added to the list
		if( apply )
		{
			if( mod->m_amount > 0 )
				MaxValue = mod->m_amount;
			else
				MinValue = mod->m_amount;
		}
		//search for other best option
		uint32 SearchRanges[6]={ POS_AURAS_START, MAX_POSITIVE_AURAS1(m_target),NEG_AURAS_START, MAX_NEGATIVE_AURAS1(m_target),PAS_AURAS_START, MAX_PASSIVE_AURAS1(m_target) };
		for( uint32 k=0;k<6;k+=2 )
		{
			for(uint32 i = SearchRanges[k]; i < SearchRanges[k+1]; ++i)
			{
				Aura *a = m_target->m_auras[i];
				if( a != NULL && ( a->m_flags & MODS_ARE_APPLIED ) )
				{
					for( uint32 j=0;j<a->m_modcount;j++)
						if( a->m_modList[j].m_AuraName == mod->m_AuraName 
							&& a->GetSpellProto()->eff[ a->m_modList[j].i ].EffectMiscValue == mod->m_miscValue )
						{
							if( a->m_modList[j].m_amount > 0 )
							{
								if( a->m_modList[j].m_amount > MaxValue )
									MaxValue = a->m_modList[j].m_amount;
							}
							else
							{
								if( a->m_modList[j].m_amount < MinValue )
									MinValue = a->m_modList[j].m_amount;
							}
						}
				}
			}
		}
		//apply merged mod values 
		m_target->MechanicDurationPctMod[ mod->m_miscValue ] = MaxValue + MinValue;
	}
}

void Aura::HandleAuraControlVehicle(bool apply)
{
	//we are casting this spell as player. Just making sure in future noone is dumb enough to forget that
	Unit *cu = GetUnitCaster();
	if( !cu || !cu->IsPlayer() || !m_target->IsUnit() )
	{ 
		return;
	}

	Player *pcaster = SafePlayerCast( cu );

	if( pcaster->GetSummon() )
		pcaster->GetSummon()->Dismiss();

    WorldPacket data(SMSG_CONTROL_VECHICLE, 0);
    pcaster->GetSession()->SendPacket(&data);

	return;

	Player *p_target;
	if( m_target->IsPlayer() )
		p_target = SafePlayerCast( m_target );
	else
		p_target = NULL;
	if( apply )
	{
		p_target->ClientControlUpdate( 1 );

//		pcaster->controlled_vehicle = SafeUnitCast( m_target );
//		pcaster->SetUInt64Value( UNIT_FIELD_CHARM, pcaster->controlled_vehicle->GetGUID() );
//		pcaster->SetUInt64Value( PLAYER_FARSIGHT, pcaster->controlled_vehicle->GetGUID() ); //focus camera on this 

		//add mana to the vehicle. It is supposed to already have mana
		for(int i=0;i<POWER_TYPE_COUNT;i++)
		{
			if( m_target->GetMaxPower( i ) < 100 )
				m_target->SetMaxPower( i, 100000 );
			m_target->SetPower(  i, m_target->GetMaxPower(  i ) );
		}

		//root the player
		pcaster->Root();
	
		//show the interface for the vehicle
		WorldPacket data2(SMSG_CONTROL_VECHICLE, 0);
		pcaster->GetSession()->SendPacket(&data2);

		//port ourselfs on it
		pcaster->SetPosition( m_target->GetPosition() );
		float o = m_target->GetOrientation();
		pcaster->SetOrientation( o );
/*		WorldPacket data3(SMSG_MOVE_TELEPORT_ACK, 74);
		data3 << pcaster->GetNewGUID();
		data3 << uint32(1); // no idea really
		data3 << uint32(0x200); // enable block for sending the guid and the floats
		data3 << uint16(0);
		data3 << uint32( getMSTime() ); //got a strong feeling this is something else
//		data3 << uint32( 0 ); //got a strong feeling this is something else
		data3 << pcaster->GetPosition();
		data3 << pcaster->GetOrientation();
		data3 << m_target->GetGUID(); //!!!need to change this to have a vehicle GUID 
		data3 << float( 0.213100001216 ); // maybe orientation for vehicle seat ?
		data3 << float( 0 );
		data3 << float( 1.86570000648 ); // maybe orientation for vehicle seat ?
		data3 << float( 0 );
		data3 << float( 1.86570000648 ); // some orientation or not ?
		data3 << uint32( 0 );
		data3 << uint8( 0 );
		pcaster->GetSession()->SendPacket(&data3);/**/

		//the pet spells. These should be fetched from some dbc i guess
		/*{
			if( m_target->IsCreature() )
			{
				Creature *vehicle = SafeCreatureCast( m_target );
				if( vehicle->GetCreatureInfo() )
				{
					CreatureSpellDataEntry * SpellData = dbcCreatureSpellData.LookupEntry( vehicle->GetCreatureInfo()->SpellDataID );
					if( SpellData )
						for( uint32 i = 0; i < 3; ++i )
							if( SpellData->Spells[i] != 0 )
								AddSpell( dbcSpell.LookupEntry( SpellData->Spells[i] ), false ); //add spell to pet
				}

		}*/

		//these are sent after we receive the tel ack

		//set our speed to the speed of the controlled vehicle
		pcaster->SetPlayerSpeed( RUN, 40 );

		WorldPacket data4(SMSG_PET_SPELLS, 62);
		data4 << m_target->GetGUID();
		data4 << uint32(1);
		data4 << uint32(0);
		data4 << uint32(0x101);
		data4 << uint32(0x0800CC28); //cc28 = 52264 = 
		data4 << uint32(0x0900CC2C);
		data4 << uint32(0x0A000000);
		data4 << uint32(0x0B000000);
		data4 << uint32(0x0C000000);
		data4 << uint32(0x0D000000);
		data4 << uint32(0x0E000000);
		data4 << uint32(0x0F000000);
		data4 << uint32(0x10000000);
		data4 << uint32(0x11000000);
		data4 << uint16(0x0);
		pcaster->GetSession()->SendPacket(&data4);
	}
	else
	{
		pcaster->ClientControlUpdate( 0 );
		pcaster->SetUInt64Value( UNIT_FIELD_CHARM, 0 );
		pcaster->SetUInt64Value( PLAYER_FARSIGHT, 0 );

		//make the sound of the vehicle. I'm sure this should be dinamic from somewhere :P
		WorldPacket data2(SMSG_PET_DISMISS_SOUND, 16);
		data2 << uint32(0x41);
		data2 << m_target->GetPosition();
		pcaster->GetSession()->SendPacket(&data2);

//		pcaster->controlled_vehicle = NULL;

		//unroot the player
		pcaster->Unroot();

		//put him near vehicle. Or not ?

		//need to send him no more pet spells ?
		WorldPacket data3(SMSG_PET_SPELLS, 8);
		data3 << uint32(0);
		data3 << uint32(0);
		pcaster->GetSession()->SendPacket(&data3);
	}
}

void Aura::SpellAuraConvertRune(bool apply)
{
	if( !m_target->IsPlayer() )
	{ 
		return;
	}
	if( m_spellProto->eff[mod->i].EffectMiscValueB > TOTAL_RUNE_TYPES 
		|| m_spellProto->eff[mod->i].EffectMiscValue > TOTAL_RUNE_TYPES
		)
		return;
	Player *ptarget = SafePlayerCast( m_target );
	if (apply)
	{
		SetPositive();
		int selected_slot = -1;
		for(int j=TOTAL_USED_RUNES-1;j>=0;j--)
			if( ptarget->m_rune_types[j] == m_spellProto->eff[mod->i].EffectMiscValue 
				&& ( ( GetSpellProto()->c_is_flags2 & SPELL_FLAG2_IS_CONVERTING_RECHARGING_RUNE ) == 0 || ptarget->m_runes[j] != MAX_RUNE_VALUE ) //converting depleted runes or any rune ?
				)
			{
				selected_slot = j;
				break;
			}
		mod->fixed_amount[ 0 ] = selected_slot;
		if( selected_slot == -1 )
			return; //cannot convert
		ptarget->m_rune_types[ mod->fixed_amount[ 0 ] ] = m_spellProto->eff[mod->i].EffectMiscValueB;
		ptarget->ConvertRuneIcon( mod->fixed_amount[ 0 ], m_spellProto->eff[mod->i].EffectMiscValueB );
		m_flags |= NEED_RUNETYPE_RESEND_ON_MAPCHANGE;
	}
	else
	{
		if( mod->fixed_amount[ 0 ] == -1 )
			return; //could not convert
		ptarget->m_rune_types[ mod->fixed_amount[ 0 ] ] = m_spellProto->eff[mod->i].EffectMiscValue;
		ptarget->ConvertRuneIcon( mod->fixed_amount[ 0 ] , m_spellProto->eff[mod->i].EffectMiscValue );
	}
}

void Aura::SpellAuraAddHealth(bool apply)
{
	int32 amount;
	if( apply )
	{
		SetPositive();
		amount = mod->m_amount;
	}
	else
		amount = -mod->m_amount;

	if( m_target->IsPlayer() )
	{
		if( apply == false )
		{
			int32 loose_hp = amount;
			if( -loose_hp > m_target->GetInt32Value( UNIT_FIELD_HEALTH ) )
				loose_hp = -MAX( 0, ( m_target->GetInt32Value( UNIT_FIELD_HEALTH ) - 1 ) );
			m_target->ModUnsigned32Value(UNIT_FIELD_HEALTH, loose_hp);
			m_target->ModUnsigned32Value( UNIT_FIELD_MAXHEALTH, loose_hp); //this seems useless, but it will avoid modding bear form health 2 times due to UpdateStats()
		}
		SafePlayerCast( m_target )->SetHealthFromSpell( SafePlayerCast( m_target )->GetHealthFromSpell() + amount );
		SafePlayerCast( m_target )->UpdateStats();
		if( apply == true )
			m_target->ModUnsigned32Value(UNIT_FIELD_HEALTH, amount);
	}
	else 
	{
		m_target->ModUnsigned32Value( UNIT_FIELD_MAXHEALTH, amount);
		if( -amount > m_target->GetInt32Value( UNIT_FIELD_HEALTH ) )
			amount = -MAX( 0, ( m_target->GetInt32Value( UNIT_FIELD_HEALTH ) - 1 ) );
		m_target->ModUnsigned32Value(UNIT_FIELD_HEALTH, amount);
	}
}

//!!! this is defenetly spell id/name/group based -> need to be remade
void Aura::SpellAuraRemoveReagentCost(bool apply)
{
	if(!m_target->IsPlayer())
	{ 
		return;
	}

	//get current mask
	uint32 cur_masks[SPELL_GROUPS_COUNT];
	for(uint32 i=0;i<SPELL_GROUPS_COUNT;i++)
		cur_masks[i] = m_target->GetUInt32Value(PLAYER_NO_REAGENT_COST_1 + i );

	if (apply)
	{
//		SafePlayerCast(m_target)->removeReagentCost = true;
		for(uint32 i=0;i<SPELL_GROUPS_COUNT;i++)
			cur_masks[i] |= GetSpellProto()->eff[ mod->i ].EffectSpellGroupRelation[i];
	}
	else
	{
//		SafePlayerCast(m_target)->removeReagentCost = false;
		for(uint32 i=0;i<SPELL_GROUPS_COUNT;i++)
			cur_masks[i] &= ~GetSpellProto()->eff[ mod->i ].EffectSpellGroupRelation[i];
	}

	//set new mask
    m_target->SetUInt32Value(PLAYER_NO_REAGENT_COST_1  , cur_masks[0]);
    m_target->SetUInt32Value(PLAYER_NO_REAGENT_COST_1+1, cur_masks[1]);
    m_target->SetUInt32Value(PLAYER_NO_REAGENT_COST_1+2, cur_masks[2]);
}

void Aura::SpellAuraIgnoreShapeshift( bool apply )
{
	if(!m_target->IsPlayer())
	{ 
		return;
	}

	if( apply )
	{
		m_visualSlot2 = 80-SafePlayerCast(m_target)->ignoreShapeShiftChecks;
		SafePlayerCast(m_target)->ignoreShapeShiftChecks++;
//		sEventMgr.AddEvent( this, &Aura::SendIgnoreStateAura, EVENT_SEND_PACKET_TO_PLAYER_AFTER_LOGIN, 5000, 1, 0 );
		SendIgnoreStateAura();
		m_flags |= NEED_STATEIGNORE_RESEND_ON_MAPCHANGE;
		//need to make these to be resent on changing maps
//		if( GetDuration() != (uint32)(-1) )
//			sEventMgr.AddEvent( this, &Aura::SendIgnoreStateAura, GetSpellId(), 3, EVENT_SEND_PACKET_TO_PLAYER_AFTER_LOGIN, 5*60*1000, 0, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
	}
	else
	{
		if( SafePlayerCast(m_target)->ignoreShapeShiftChecks > 0 )
			SafePlayerCast(m_target)->ignoreShapeShiftChecks--;
	}
}

void Aura::Refresh_SpellAuraIncreaseAPBasedOnArmor(uint32 mod_i)
{
	//!!!! AMG fix this. We are removed while we are handling aura !!! This is mem corruption
	if( !m_target )
		return; //yes this is possible

	//remove previous applied value
	m_target->ModUnsigned32Value(UNIT_FIELD_ATTACK_POWER_MOD_POS,-m_modList[mod_i].fixed_amount[ 0 ]);

	//get new value and reapply it
	uint32 effect_value = GetSpellProto()->eff[1].EffectBasePoints + 1;
	uint32 armor_bonus_per_effect_value = ( GetSpellProto()->eff[0].EffectBasePoints + 1 ) * ( GetSpellProto()->eff[1].EffectBasePoints + 1 );
	m_modList[mod_i].fixed_amount[ 0 ] = m_target->GetUInt32Value( UNIT_FIELD_RESISTANCES ) * effect_value / armor_bonus_per_effect_value;
	m_target->ModUnsigned32Value(UNIT_FIELD_ATTACK_POWER_MOD_POS,m_modList[mod_i].fixed_amount[ 0 ]);
	m_target->CalcDamage();
}

void Aura::SpellAuraIncreaseAPBasedOnArmor( bool apply )
{
	if( !m_target->IsUnit() )
	{ 
		return;
	}

	if(mod->m_amount<0)
		SetNegative();
	else
		SetPositive();

	if( apply )
	{
		mod->fixed_amount[ 0 ] = 0;
		Refresh_SpellAuraIncreaseAPBasedOnArmor(mod->i);
		sEventMgr.AddEvent(this, &Aura::Refresh_SpellAuraIncreaseAPBasedOnArmor, mod->i, 
			EVENT_AURA_PERIODIC_UPDATE, AURA_SpellAuraIncreaseAPBasedOnArmor_REFRESH_INTERVAL ,0,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
	}
	else
	{
		m_target->ModUnsigned32Value(UNIT_FIELD_ATTACK_POWER_MOD_POS,-mod->fixed_amount[ 0 ]);
		mod->fixed_amount[ 0 ] = 0;
		m_target->CalcDamage();
	}
}

//not sure - only killing spree and bladestorm used this effect. They need to cast but no other spell
void Aura::SpellAuraModNoCast( bool apply )
{
//	SpellAuraModSilence( apply );
//	SpellAuraPacifySilence( apply );
	if( apply )
	{
		uint32 dur = GetDuration();
		uint32 SchoolMask = 126;	//all except melee ?
		for(uint32 i=0;i<SCHOOL_COUNT;i++)
			if( SchoolMask & ( 1<<i) )
			{
				if( dur )
					m_target->SchoolCastPrevent[ i ]=dur+500+getMSTime();
				else
					m_target->SchoolCastPrevent[ i ]=2500+getMSTime(); //crap
				mod->fixed_amount[0] = m_target->SchoolCastPrevent[ i ];
			}
		//gray out player cast bar
		if( m_target->IsPlayer() )
			m_target->SetFlag( PLAYER_FLAGS, PLAYER_ALLOW_ONLY_SPELLS_MELEE );
	}
	else
	{
		//so bladestorm macro cancel would allow people cast spells
		uint32 SchoolMask = 126;	//all except melee ?
		for(uint32 i=0;i<SCHOOL_COUNT;i++)
			if( SchoolMask & ( 1<<i) )
			{
				if( m_target->SchoolCastPrevent[ i ] == mod->fixed_amount[0] )
					m_target->SchoolCastPrevent[ i ] = 0;
			}
		//gray out player cast bar
		if( m_target->IsPlayer() )
			m_target->RemoveFlag( PLAYER_FLAGS, PLAYER_ALLOW_ONLY_SPELLS_MELEE );
	}
}

//only Deterrence atm
void Aura::SpellAuraModRegisterCanCastScript( bool apply )
{
	if( m_target->IsPlayer() == false )
		return;

	Player *p_target = SafePlayerCast( m_target );
	if( apply == true )
	{
		if( GetSpellProto()->CanCastFilterHandler != NULL )
		{
			SpellCanCastScript *so = new SpellCanCastScript;
			so->CanCastFilterHandler = GetSpellProto()->CanCastFilterHandler;
			so->OwnerSpell = GetSpellProto();
			so->CasterGUID = GetCasterGUID();
			Unit *u_caster = GetUnitCaster();
			if( u_caster && u_caster->GetGroup() )
				so->CasterGroupId = u_caster->GetGroup()->GetID();
			p_target->mSpellCanCastOverrideMap.push_front( so );
			mod->extension = (void*)so;
		}
	}
	else
	{
		SimplePointerListNode<SpellCanCastScript> *itr,*itr2;
		for(itr = p_target->mSpellCanCastOverrideMap.begin(); itr != p_target->mSpellCanCastOverrideMap.end();)
		{
			itr2 = itr;
			itr = itr->Next();
			if( (void*)(itr2->data) == mod->extension )
			{
				p_target->mSpellCanCastOverrideMap.remove( itr2, 1 );//no leaks pls = delete on remove
				mod->extension = NULL;
			}
		}
	}
}

void Aura::SpellAuraModDMGTakenMechanic( bool apply )
{
	uint32 eff_mechanic;
	if( GetSpellProto()->eff[ mod->i ].EffectMiscValue )
		eff_mechanic = GetSpellProto()->eff[ mod->i ].EffectMiscValue;
	else
		eff_mechanic = GetSpellProto()->eff[ mod->i ].EffectMechanic;
	if( eff_mechanic >= MECHANIC_TOTAL || eff_mechanic == 0 )
	{
		sLog.outDebug("Warning mechanic %d is not supported for spell %d",GetSpellProto()->eff[ mod->i ].EffectMechanic,GetSpellProto()->Id );
		return;
	}
	if( apply )
//		m_target->ModDamageTakenByMechFlat[ eff_mechanic ] += mod->m_amount;
		m_target->ModDamageTakenByMechPCT[ eff_mechanic ] += mod->m_amount;	//in 403 it seems to be PCT all the time
	else
//		m_target->ModDamageTakenByMechFlat[ eff_mechanic ] -= mod->m_amount;
		m_target->ModDamageTakenByMechPCT[ eff_mechanic ] -= mod->m_amount;	//in 403 it seems to be PCT all the time
}

void Aura::SpellAuraModHasteAndCastSpeed( bool apply )
{
	//value is positive when we boost the speed and negative when we slow dow
//	if( apply )
//		mod->m_amount = -mod->m_amount;
	SpellAuraModHaste( apply );
	SpellAuraModRangedHaste( apply );
	SpellAuraModCastingSpeed( apply );
}

void Aura::SpellAuraModFaction( bool apply )
{
	if( m_target == NULL || m_target->IsPlayer() == false )
		return;
	Player *p_target = SafePlayerCast(m_target);
	if( apply )
	{
		mod->fixed_amount[ 0 ] = p_target->GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE);
		p_target->SetUInt32Value( UNIT_FIELD_FACTIONTEMPLATE, mod->m_miscValue );
		p_target->_setFaction();
	}
	else
	{
		p_target->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, mod->fixed_amount[ 0 ] );
		p_target->_setFaction();
	}
}

void Aura::SpellAuraModXPAndRepGainPCT( bool apply )
{
	if( m_target == NULL || m_target->IsPlayer() == false )
		return;
	Player *p_target = SafePlayerCast(m_target);
	if( apply )
	{
		p_target->XPModBonusPCT += mod->m_amount;
		p_target->pctReputationMod += mod->m_amount;
	}
	else
	{
		p_target->XPModBonusPCT -= mod->m_amount;
		p_target->pctReputationMod -= mod->m_amount;
	}
}

void Aura::SpellAuraModHonorGainPCT( bool apply )
{
	if( m_target == NULL || m_target->IsPlayer() == false )
		return;
	Player *p_target = SafePlayerCast(m_target);
	if( apply )
	{
		p_target->HonorModBonusPCT += mod->m_amount;
	}
	else
	{
		p_target->HonorModBonusPCT -= mod->m_amount;
	}
}

void Aura::SpellAuraModFlyghtSpeedPCT( bool apply )
{
	if( apply )
	{
		SetPositive();
		mod->fixed_amount[ 0 ] = m_target->m_flyspeedModifier * mod->m_amount / 100;
		m_target->m_flyspeedModifier += mod->fixed_amount[ 0 ];
	}
	else
	{
		m_target->m_flyspeedModifier -= mod->fixed_amount[ 0 ];
	}
	m_target->UpdateSpeed();
}


void Aura::SpellAuraModMountedSpeedPCT( bool apply )
{
	if( apply )
	{
		SetPositive();
		mod->fixed_amount[ 0 ] = m_target->m_mountedspeedModifier * mod->m_amount / 100;
		m_target->m_mountedspeedModifier += mod->fixed_amount[ 0 ];
	}
	else
	{
		m_target->m_mountedspeedModifier -= mod->fixed_amount[ 0 ];
	}
	m_target->UpdateSpeed();
}

void Aura::SpellAuraModOffensiveMagicalDurationPCT( bool apply )
{
	if( apply )
	{
		m_target->OffensiveMagicalDurationModPCT += mod->m_amount;	//value is already negative
	}
	else
	{
		m_target->OffensiveMagicalDurationModPCT -= mod->m_amount;
	}
}

void Aura::SpellAuraAuraSchoolImmunity( bool apply )
{
	if(apply)
	{
		//fixme me may be negative
		Unit * c = GetUnitCaster();
		if(c)
		{
			if(isAttackable(c,m_target))
				SetNegative();
			else
				SetPositive();
		}
		else
			SetPositive();

		for(uint32 i = 0; i < SCHOOL_COUNT; i++)
			if(mod->m_miscValue & (1<<i))
			{
				m_target->AuraSchoolImmunityList[i]++;
				//debuff imunity does not remove old auras, just blocks new ones
				if( GetSpellProto()->eff[ mod->i ].EffectApplyAuraName != SPELL_AURA_MOD_DEBUFF_IMMUNITY )
					m_target->RemoveAurasOfSchool(i, false, true);
			}
	}
	else
	{
		for(int i = 0; i < SCHOOL_COUNT; i++)
			if( ( mod->m_miscValue & (1<<i) ) && m_target->AuraSchoolImmunityList[i]>0 )
				m_target->AuraSchoolImmunityList[i]--;
	}
}

void Aura::SpellAuraCritHealDoneModPct( bool apply )
{
	if( m_target == NULL )
		return;
	if(apply)
	{
		m_target->HealCritDonePctMod += mod->m_amount;
	}
	else
	{
		m_target->HealCritDonePctMod -= mod->m_amount;
	}
}

void Aura::SpellAuraModDeflectChance(bool apply)
{
	if (!m_target->IsPlayer())
	{ 
		return;
	}
	if(apply)
	{
		SafePlayerCast( m_target )->m_resist_hit[ MOD_SPELL ] += mod->m_amount;
		SafePlayerCast( m_target )->m_resist_hit[ MOD_MELEE ] += mod->m_amount;
		SafePlayerCast( m_target )->m_resist_hit[ MOD_RANGED ] += mod->m_amount;
		SafePlayerCast( m_target )->m_deflect += mod->m_amount;
	}
	else
	{
		SafePlayerCast( m_target )->m_resist_hit[ MOD_SPELL ] -= mod->m_amount;
		SafePlayerCast( m_target )->m_resist_hit[ MOD_MELEE ] -= mod->m_amount;
		SafePlayerCast( m_target )->m_resist_hit[ MOD_RANGED ] -= mod->m_amount;
		SafePlayerCast( m_target )->m_deflect -= mod->m_amount;
	}
}

void Aura::SpellAuraPhase(bool apply)
{
	if ( m_target )
	{
//		if ( m_target->GetAuraStackCount(SPELL_AURA_PHASE) > 1 )
		if ( m_target->m_phase != 1 )
		{
			if ( m_target->IsPlayer() )
				SafePlayerCast(m_target)->GetSession()->SystemMessage("You can have only one phase aura!");
			Remove();
			return;
		}

		if (apply)
			m_target->Phase( PHASE_SET, m_spellProto->eff[mod->i].EffectMiscValue );
		else
			m_target->Phase( PHASE_RESET );

		if ( m_target->IsPlayer() )
		{
			sStackWorldPacket( data, SMSG_SET_PHASE_SHIFT, 8);
			data << uint32(m_target->m_phase);
			SafePlayerCast(m_target)->GetSession()->SendPacket(&data);
		}
	}
}

void Aura::SpellAuraMirrorDMG(bool apply)
{
	if(apply)
	{
		//create a proc on any dmg
		ProcTriggerSpell *pts = new ProcTriggerSpell( GetSpellProto(), this );
		pts->procFlags = PROC_ON_ANY_DAMAGE_VICTIM;
		pts->caster = m_casterGuid;	//we will deal dmg on this guy
		pts->procChance = 100;
		pts->procCharges = 0;
		pts->created_with_value = mod->m_amount;
		pts->spellId = 71948;		//we will use this for all spells since we need to force deal dmg to friendly
		m_target->RegisterProcStruct(pts);
	}
	else
	{
		m_target->UnRegisterProcStruct( this );
	}
}


void Aura::SpellAuraModIgnoreInterrupt(bool apply)
{
	//according to the one and only example it is enough to send the aura update packet
	//seems to be handled by client only by the presence of the aura 
	//confirmed to work with visible aura : for aspect of the fox, firestarter, spiritwaler's grace, inferno
	if( m_target->IsPlayer() == false )
		return;
	Player *p_target = SafePlayerCast( m_target );
	if(apply)
	{
		p_target->AddSpellIgnoringMoveInterrupt( mod->m_miscValue );
	}
	else
	{
		p_target->RemSpellIgnoringMoveInterrupt( mod->m_miscValue );
	}
}

void Aura::SpellAuraOpenStableUI(bool apply)
{
	if( apply )
	{
		Unit *uc = GetUnitCaster();
		if( uc == NULL || uc->IsPlayer() == false || uc->IsInWorld() == false )
			return;
		Player *pc = SafePlayerCast( uc );
		if( pc->GetSession() == NULL )
			return;
		pc->GetSession()->SendStabledPetList( pc->GetGUID() );
	}
}

void Aura::SpellAuraModSpellPowerPCT(bool apply)
{
	if( m_target->IsPlayer() == false )
		return;
	if(apply)
		m_target->ModFloatValue( PLAYER_FIELD_MOD_SPELL_POWER_PCT, (float)mod->m_amount / 100.0f );
	else
		m_target->ModFloatValue( PLAYER_FIELD_MOD_SPELL_POWER_PCT, (float)-mod->m_amount / 100.0f );
}

void Aura::SpellAuraModMastery(bool apply)
{
	if( m_target->IsPlayer() == false )
		return;
	if(apply)
		SafePlayerCast( m_target )->m_mastery += mod->m_amount;
	else
		SafePlayerCast( m_target )->m_mastery -= mod->m_amount;

	SafePlayerCast( m_target )->UpdateStats();
}


void Aura::SpellAuraModAutoAttackDmg(bool apply)
{
	Unit *u_target;
	if( m_target->IsUnit() )
		u_target = SafeUnitCast( m_target );
	else
		return;
	if(apply)
		u_target->MeleeDmgPCTMod += mod->m_amount;
	else
		u_target->MeleeDmgPCTMod -= mod->m_amount;
}

void Aura::SpellAuraModCritBlockChance(bool apply)
{
	Player *p_target;
	if( m_target->IsPlayer() )
		p_target = SafePlayerCast( m_target );
	else
		p_target = NULL;
	if( p_target == NULL )
		return;
	if(apply)
		p_target->m_crit_block_chance += mod->m_amount;
	else
		p_target->m_crit_block_chance -= mod->m_amount;
	p_target->UpdateChances();
}

void Aura::SpellAuraModDmgPenetrate(bool apply)
{
//	Player *p_target;
//	if( m_target->IsPlayer() )
//		p_target = SafePlayerCast( m_target );
//	else
//		p_target = NULL;
//	if( p_target == NULL )
//		return;
	Unit *u_caster = GetUnitCaster();
	if( u_caster == NULL )
		return;
	//this is caster / target specific atm
	if(apply)
	{
		int64 *store = m_target->GetCreateIn64Extension( EXTENSION_ID_AURA_PENETRATE_VAL );
		*store = *store + 1;
//		p_target->m_dmg_pct_penetrate += mod->m_amount;
	}
	else
	{
//		p_target->m_dmg_pct_penetrate -= mod->m_amount;
		int64 *store = m_target->GetCreateIn64Extension( EXTENSION_ID_AURA_PENETRATE_VAL );
		*store = *store - 1;
	}
}

void Aura::SpellAuraModRunicPowerRegenPCT(bool apply)
{
	Player *p_target;
	if( m_target->IsPlayer() )
		p_target = SafePlayerCast( m_target );
	else
		p_target = NULL;
	if( p_target == NULL )
		return;
	if(apply)
		p_target->m_mod_runic_power_gain_pct += mod->m_amount;
	else
		p_target->m_mod_runic_power_gain_pct -= mod->m_amount;
}

void Aura::SpellAuraModCritPercAll(bool apply)
{
	Player *p_target;
	if( m_target->IsPlayer() )
		p_target = SafePlayerCast( m_target );
	else
		p_target = NULL;
	//melee kinda mod
	if (m_target->IsPlayer())
	{
		if(apply)
		{
			WeaponModifier *md;
			md = new WeaponModifier;
			md->value = float(mod->m_amount);
			md->wclass = -1;
			md->subclass = -1;
			md->spellentry = GetSpellProto();
			p_target->tocritchance.push_front( md );
		}
		else
		{
			SimplePointerListNode<WeaponModifier> *i;
			for( i=p_target->tocritchance.begin();i!=p_target->tocritchance.end();i = i->Next() )
				if( i->data->spellentry == GetSpellProto() )
				{
					p_target->tocritchance.remove( i, 1 );
					break;
				}
		}
		SafePlayerCast( m_target )->UpdateChances();
	}
	else if( m_target->IsCreature())
	{
		if(apply)
			SafeCreatureCast(m_target)->crit_chance_mod += mod->m_amount;
		else
			SafeCreatureCast(m_target)->crit_chance_mod -= mod->m_amount;
	}
	//spell kinda mod
	if(apply)
	{
		for(uint32 x=0;x<SCHOOL_COUNT;x++)
			m_target->SpellCritChanceSchool[x] += mod->m_amount;
		if(mod->m_amount < 0)
			SetNegative();
		else
			SetPositive();
	}
	else
	{
		for(uint32 x=0;x<SCHOOL_COUNT;x++)
			m_target->SpellCritChanceSchool[x] -= mod->m_amount;
	}
	if(m_target->IsPlayer())
		SafePlayerCast( m_target )->UpdateChanceFields();
}

void Aura::SpellAuraModForceMoveForward(bool apply)
{
	if( apply )
		m_target->SetFlag( UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FORCE_MOVE );
	else
		m_target->RemoveFlag( UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FORCE_MOVE );
}

void Aura::SpellAuraModComprehendLanguage(bool apply)
{
	if( apply )
		m_target->SetFlag( UNIT_FIELD_FLAGS_2, UNIT_FLAG2_COMPREHEND_LANG );
	else
		m_target->RemoveFlag( UNIT_FIELD_FLAGS_2, UNIT_FLAG2_COMPREHEND_LANG );
}

void Aura::SpellAuraModAOEDmgTakenPct(bool apply)
{
	int32 val;
	if( apply )
	{
		val = mod->m_amount;
		if( val < 0 )
			SetPositive();
		else
			SetNegative();
	}
	else
		val = -mod->m_amount;

	for(uint32 i = 0; i < SCHOOL_COUNT; i++)
		if(mod->m_miscValue & (1<<i))
			m_target->AoeDamageTakenPctMod[ i ] += val;
}

void Aura::SpellAuraReduceEffectDurationDispelType(bool apply)
{
	int32 val;
	if(apply)
	{
		SetPositive();
		val = mod->m_amount; // TODO Only maximum effect should be used for Silence or Interrupt effects reduction
	}
	else
		val = -mod->m_amount;

	if( mod->m_miscValue > 0 && mod->m_miscValue < DISPEL_COUNT_NON_BENEFICIAL)
		m_target->DispelDurationPctMod[ mod->m_miscValue ] += val;
	else
		sLog.outDebug("Unsupported aura dispelduration mod type %u",mod->m_miscValue);
}
/*
void Aura::SpellAuraModReplaceSpell(bool apply)
{
	if( m_target->IsPlayer() == false )
		return;

	if( mod->m_amount == 0 && apply )
	{
		uint32 swap_to_spell = SafePlayerCast( m_target )->mSpellReplaces[ GetSpellId() ];
		if( swap_to_spell == 0 )
			return;

		mod->m_amount = swap_to_spell;	//this will be sent to client as aura ammount. Do not remove this !
	}

	if( apply )
	{
		//mod->m_miscValue needs to be set by us !!
		//!!client knows somehow what he needs to swap(remove)
//		if( mod->m_miscValue && mod->m_amount )
//			SafePlayerCast( m_target )->SwapActionButtonSpell( mod->m_miscValue, mod->m_amount, false, false, true );
		SafePlayerCast( m_target )->mTempSpells.insert( mod->m_amount );
	}
	else
	{
//		if( mod->m_miscValue && mod->m_amount )
//			SafePlayerCast( m_target )->SwapActionButtonSpell( mod->m_amount, mod->m_miscValue, false, false, true );
		SafePlayerCast( m_target )->mTempSpells.erase( mod->m_amount );
	}
}*/

void Aura::SpellAuraModAddTemporarySpell(bool apply)
{
	if( m_target->IsPlayer() == false )
		return;

	//need to send these spells to client that we know them !!
	if( apply )
	{
		//eeek, invalid spell, try to make the aura application a failure to avoid making client display an invalid aura
		if( mod->m_amount <= 1 )
		{
			//first, try to not mess up client side icons
			if( mod->m_miscValue > 1 )
				mod->m_amount = mod->m_miscValue;
			else
				m_flags |= (MOD_0_RESISTED+mod->i);
		}
		SafePlayerCast( m_target )->mTempSpells.insert( mod->m_amount );
		if( mod->m_miscValue )
			SafePlayerCast( m_target )->mSpellReplaces[ mod->m_miscValue ] = mod->m_amount;
	}
	else
	{
		SafePlayerCast( m_target )->mTempSpells.erase( mod->m_amount );
		if( mod->m_miscValue )
			SafePlayerCast( m_target )->mSpellReplaces[ mod->m_miscValue ] = 0;
	}
}

void Aura::SpellAuraModRangedTargetting(bool apply)
{
	if( apply )
	{
		m_target->ImmuneToRanged++;
	}
	else if( m_target->ImmuneToRanged > 0 )
	{
		m_target->ImmuneToRanged--;
	}
}

void Aura::SpellAuraModHealAbsorb(bool apply)
{
	if( apply )
		m_target->HealAbsorb += mod->m_amount;
	else if( m_target->HealAbsorb >= mod->m_amount )
		m_target->HealAbsorb -= mod->m_amount;
	else
		m_target->HealAbsorb = 0;
}

void Aura::SpellAuraModCasterCritChanceOnTargetOfSpells(bool apply)
{
	//NOT good ! This crit mod should be caster dependent and not global to all casters
	int32 val = apply ? mod->m_amount : -mod->m_amount;
	uint32 *AffectedGroups = GetSpellProto()->eff[mod->i].EffectSpellGroupRelation;

	if( m_target->SM_Mods == NULL )
		m_target->SM_Mods = new UnitSMMods;
	SendModifierLog( &m_target->SM_Mods->SM_FCriticalChanceTarget, val, AffectedGroups, mod->m_miscValue, true );
}

void Aura::SpellAuraModAOEDmgTakenPctFromCreature(bool apply)
{
	//NOT good ! This crit mod should be caster dependent and not global to all casters
	int32 val = apply ? mod->m_amount : -mod->m_amount;
	m_target->AoeDamageTakenPctModFromCreatures += val;
}

void Aura::SpellAuraModAPToSPExclusive(bool apply)
{
	if( m_target->IsPlayer() == false )
		return;
	int32 val;
	if( apply )
	{
//		val = mod->m_amount;
		val = GetSpellProto()->eff[mod->i].EffectBasePoints;
		ASSERT( mod->m_amount == GetSpellProto()->eff[mod->i].EffectBasePoints ); //players say this is getting imba. but why ? Maybe some SMT is boosting it ?
	}
	else
//		val = -mod->m_amount;
		val = -GetSpellProto()->eff[mod->i].EffectBasePoints;

	SafePlayerCast( m_target )->m_APToSPExclusivePCT += val;

	SafePlayerCast( m_target )->UpdateAttackPowerToSpellPower();
}
