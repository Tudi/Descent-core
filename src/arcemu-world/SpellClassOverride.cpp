#include "StdAfx.h"

#define REGISTER_HANDLER( spell ) sp = dbcSpell.LookupEntryForced( spell ); \
	ASSERT( sp->ClassOverideApplyHandler == 0 ); \
	sp->ClassOverideApplyHandler = &SMT_##spell;
#define REGISTER_SAMEHANDLER(spell,copy_from) sp = dbcSpell.LookupEntryForced( spell ); \
	ASSERT( sp->ClassOverideApplyHandler == 0 ); \
	sp->ClassOverideApplyHandler = &SMT_##copy_from;

SPELL_EFFECT_OVERRIDE_RETURNS HandleDefaultEffectOverride(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	//test if we should trigger at all
	if( so->required_namehash[0] )
	{
		if( sp->GetProto()->NameHash != so->required_namehash[0] 
			&& sp->GetProto()->NameHash != so->required_namehash[1]
			&& sp->GetProto()->NameHash != so->required_namehash[2]
			)
			return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	}
	if( so->required_spell_id && sp->GetProto()->Id != so->required_spell_id )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( so->required_effect_type[i] != SPELL_EFFECT_NULL && sp->GetProto()->eff[i].Effect != so->required_effect_type[i] )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( so->required_aura_type[i] != SPELL_AURA_NONE && sp->GetProto()->eff[i].Effect != so->required_aura_type[i] )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

	//do our stuff
	value += so->mod_amount[ i ];
	if( so->mod_amount_pct[ i ] != 0 )
		value = value * so->mod_amount_pct[ i ] / 100;

	//this is a simple effect mod so no need to break execution
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SpellEffectOverrideScript::SpellEffectOverrideScript() 
{
	for(int i=0;i<3;i++)
	{
		mod_amount[i] = 0;		//for backward compatibility
		mod_amount_pct[i] = 0;	//for backward compatibility
		required_effect_type[i] = SPELL_EFFECT_NULL;
		required_aura_type[i] = SPELL_AURA_NONE;
		required_namehash[i] = 0;
	}
	ApplyHandler = &HandleDefaultEffectOverride;	// we can normally handle this
	RemoveHandler = NULL;
	required_spell_id = 0;
	ori_spell = NULL;
};

SPELL_EFFECT_OVERRIDE_RETURNS HandleGlyphOfLesserHealingWave(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( sp->GetProto()->NameHash != SPELL_HASH_LESSER_HEALING_WAVE )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( target == NULL || target->HasAuraWithNameHash( SPELL_HASH_EARTH_SHIELD, 0, AURA_SEARCH_POSITIVE ) == 0 )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	value = value * so->mod_amount_pct[ i ] / 100;
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleGlyphOfRegrowth(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( sp->GetProto()->NameHash != SPELL_HASH_REGROWTH )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( target == NULL || target->HasAuraWithNameHash( SPELL_HASH_EARTH_SHIELD, 0, AURA_SEARCH_POSITIVE ) == 0 )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	value = value * so->mod_amount_pct[ i ] / 100;
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
/*
SPELL_EFFECT_OVERRIDE_RETURNS HandleMoltenFury(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( target == NULL || target->GetHealthPct() > 35 )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( IsDamagingSpell(sp->GetProto(),i) == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	value = value * so->mod_amount_pct[ i ] / 100;
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}*/

SPELL_EFFECT_OVERRIDE_RETURNS HandleStarfireBonus(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( sp->GetProto()->NameHash != SPELL_HASH_STARFIRE )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( target == NULL || ( target->HasAuraWithNameHash( SPELL_HASH_INSECT_SWARM, 0, AURA_SEARCH_NEGATIVE ) == 0 && target->HasAuraWithNameHash( SPELL_HASH_MOONFIRE, AURA_SEARCH_NEGATIVE ) == 0) )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	value = value * so->mod_amount_pct[ i ] / 100;
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleTestOfFaith(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( target == NULL || target->GetHealthPct() > 50 )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( IsHealingEffect(sp->GetProto(),i) == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	value = value * so->mod_amount_pct[ i ] / 100;
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleMercilessCombat(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if(		( sp->GetProto()->NameHash == SPELL_HASH_ICY_TOUCH && i == 0 )
		||	( sp->GetProto()->NameHash != SPELL_HASH_HOWLING_BLAST && i == 1 )
		||	( sp->GetProto()->NameHash != SPELL_HASH_OBLITERATE && i == 1 )
		||	( sp->GetProto()->NameHash != SPELL_HASH_FROST_STRIKE && i == 1 )
		)
	{
		if( target && target->GetHealthPct() < 35 )
			value = value * so->mod_amount_pct[ 0 ] / 100;
	}
		
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

/*
SPELL_EFFECT_OVERRIDE_RETURNS HandleRageOfRivedare(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( target == NULL || target->HasAuraWithNameHash( SPELL_HASH_BLOOD_PLAGUE ) == 0 )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( IsDamagingSpell(sp->GetProto(),i) == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	value = value * so->mod_amount_pct[ i ] / 100;
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}*/

SPELL_EFFECT_OVERRIDE_RETURNS HandleTundraStalker(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( target == NULL || target->HasAuraWithNameHash( SPELL_HASH_FROST_FEVER, 0, AURA_SEARCH_NEGATIVE ) == 0 )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( IsDamagingSpell(sp->GetProto(),i) == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	value = value * so->mod_amount_pct[ i ] / 100;
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleMarkedForDeath(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( target == NULL || target->HasAuraWithNameHash( SPELL_HASH_HUNTER_S_MARK, 0, AURA_SEARCH_NEGATIVE ) == 0 )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	value = value * so->mod_amount_pct[ i ] / 100;
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
/*
SPELL_EFFECT_OVERRIDE_RETURNS HandleNourishHealBoost(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( target == NULL )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->NameHash != SPELL_HASH_NOURISH )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	uint32 pct_sum = 100;
	for( uint32 i = 0; i < MAX_POSITIVE_AURAS1( target ); i++ )
		if( target->m_auras[i] && ( target->m_auras[i]->GetSpellProto()->c_is_flags & SPELL_FLAG_IS_HEAL_OVER_TIME ) )
			pct_sum += so->mod_amount_pct[ i ] - 100;
	value = value * pct_sum / 100;
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}*/

SPELL_EFFECT_OVERRIDE_RETURNS HandleSoulSiphon(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( target == NULL )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->NameHash != SPELL_HASH_DRAIN_LIFE && sp->GetProto()->NameHash != SPELL_HASH_DRAIN_SOUL )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	uint32 coun_auras = 0;
	for( uint32 i = NEG_AURAS_START; i < MAX_NEGATIVE_AURAS1( target ); i++ )
		if( target->m_auras[i] 
//			&& target->m_auras[i]->GetCasterGUID() == caster->GetGUID()
			&& target->m_auras[i]->GetSpellProto()->spell_skilline[0] == SKILL_AFFLICTION )
			{
				coun_auras++;
				if( coun_auras == 3 )
					break;
			}
	if( sp->GetProto()->NameHash == SPELL_HASH_DRAIN_LIFE  )
	{
		//this is first effect
		if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_PERIODIC_DAMAGE )
		{
//			sp->forced_miscvalues[i] = so->mod_amount_pct[0] * coun_auras;	//pass the overall pct bonus
			//this was not scaling SP, but good
//			value += value * so->mod_amount_pct[0] * coun_auras / 100;
			sp->forced_pct_mod_cur += so->mod_amount_pct[0] * coun_auras;	//pass the overall pct bonus
			sp->forced_pct_mod[ 2 ] = so->mod_amount_pct[0] * coun_auras;	//crossing fingers this effect will not be used later
		}
		//because we are targetting self, we need to know the value from previous effect
		else if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MOD_TOTAL_HEALTH_REGEN_PCT )
		{
			value += value * sp->forced_pct_mod[ 2 ] / 100;
		}
	}
	else
		value += value * so->mod_amount_pct[0] * coun_auras / 100;
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleTormentTheWeak(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( target == NULL )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->NameHash != SPELL_HASH_FROSTBOLT 
		&& sp->GetProto()->NameHash != SPELL_HASH_FROSTFIRE_BOLT
		&& sp->GetProto()->NameHash != SPELL_HASH_PYROBLAST
		&& sp->GetProto()->NameHash != SPELL_HASH_ARCANE_MISSILES
		&& sp->GetProto()->NameHash != SPELL_HASH_ARCANE_BLAST
		&& sp->GetProto()->NameHash != SPELL_HASH_ARCANE_BARRAGE
		)
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->eff[i].Effect != SPELL_EFFECT_SCHOOL_DAMAGE 
		&& sp->GetProto()->eff[i].EffectApplyAuraName != SPELL_AURA_PERIODIC_DAMAGE 
		)
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	uint32 can_apply = 0;
	for( uint32 j = NEG_AURAS_START; j < MAX_NEGATIVE_AURAS1( target ); j++ )
		if( target->m_auras[j] &&
			( target->m_auras[j]->GetSpellProto()->eff[0].EffectMechanic == MECHANIC_SLOWED
			|| target->m_auras[j]->GetSpellProto()->eff[1].EffectMechanic == MECHANIC_SLOWED
			|| target->m_auras[j]->GetSpellProto()->eff[2].EffectMechanic == MECHANIC_SLOWED
			|| target->m_auras[j]->GetSpellProto()->eff[0].EffectMechanic == MECHANIC_ENSNARED
			|| target->m_auras[j]->GetSpellProto()->eff[1].EffectMechanic == MECHANIC_ENSNARED
			|| target->m_auras[j]->GetSpellProto()->eff[2].EffectMechanic == MECHANIC_ENSNARED ))
			{
				can_apply = 1;
				break;
			}
	if( can_apply == 1 )
	{
		SpellEntry *spellInfo = sp->GetProto();
		int32 spell_dmg_bonus = caster->GetSpellDmgBonus( target, spellInfo, value, i ) / (int32)spellInfo->quick_tickcount;
		spell_dmg_bonus = MAX( 0, spell_dmg_bonus );	//somebody somehow got this bugged and was doing millions of dmg :(
		int32 addition_dmg_boost = ( value + spell_dmg_bonus ) * so->mod_amount_pct[0]/ 100;
		addition_dmg_boost = MAX( 0, addition_dmg_boost );	//somebody somehow got this bugged and was doing millions of dmg :(
		value += addition_dmg_boost;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleDreadBlade(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( caster == NULL || caster->IsPlayer() == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->eff[i].Effect != SPELL_EFFECT_SCHOOL_DAMAGE 
		&& sp->GetProto()->eff[i].EffectApplyAuraName != SPELL_AURA_PERIODIC_DAMAGE )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( ( sp->GetProto()->SchoolMask & SCHOOL_MASK_SHADOW ) == 0 )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->belongs_to_player_class != caster->getClass() 
		|| sp->GetProto()->NameHash == SPELL_HASH_MIND_FLAY	//from gurth
		)
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	//Increases all Shadow damage done by 0%. Each point of Mastery increases Shadow damage by an additional 2.5%.
	{
//		int32 var_inc = float2int32(  SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->mod_amount[0] );
		int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->ori_spell->eff[1].EffectBasePoints );
		int32 tot_inc_pct = var_inc / 100;
//		int32 add_dmg = value * tot_inc_pct / 100;
//		value += add_dmg;
		sp->forced_pct_mod_cur += tot_inc_pct;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
} 
SPELL_EFFECT_OVERRIDE_RETURNS HandleBloodShield(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( caster == NULL || caster->IsPlayer() == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->NameHash != SPELL_HASH_DEATH_STRIKE	)
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( 
//		sp->GetProto()->eff[i].Effect != SPELL_EFFECT_HEAL && 
		sp->GetProto()->eff[i].Effect != SPELL_EFFECT_HEAL_MAXHEALTH_PCT )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->belongs_to_player_class != caster->getClass() )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( caster->HasAuraWithNameHash( SPELL_HASH_BLOOD_PRESENCE, 0, AURA_SEARCH_POSITIVE ) == NULL )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	//Each time you heal yourself via Death Strike, you gain 50% of the amount healed as a damage absorption shield.  Each point of Mastery increases the shield by an additional 6.25%.
	//Increases the damage done by your diseases by $s1%.  Each point of Mastery increases disease damage by an additional ${$77515m2/100}.1%.]
	//[Increases the damage done by your diseases by ${$77515m2/12.5}%.  Each point of Mastery increases disease damage by an additional ${$77515m2/100}.1%.]
	{
		int32 heal_val = value * caster->GetUInt32Value( UNIT_FIELD_MAXHEALTH ) / 100;
//		int32 var_shield = float2int32( heal_val * SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->mod_amount[0] );
		int32 var_shield = float2int32( heal_val * SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->ori_spell->eff[1].EffectBasePoints );
		int32 tot_shield = var_shield / 100 / 100;
		//prev shield amt so we can stack it
		{
			CommitPointerListNode<Absorb> *i;
			caster->Absorbs.BeginLoop();
			for( i = caster->Absorbs.begin(); i != caster->Absorbs.end(); i = i->Next() )
			{
				Absorb *tabs = i->data;
				if( tabs->spellid == 77535 )
				{
					tot_shield += tabs->amt;
					break;
				}
			}
			caster->Absorbs.EndLoopAndCommit();
		}
		if( tot_shield > caster->GetInt32Value( UNIT_FIELD_MAXHEALTH ) )
			tot_shield = caster->GetInt32Value( UNIT_FIELD_MAXHEALTH );
		//now cast the shield on us
		SpellEntry *spellInfo = dbcSpell.LookupEntry( 77535 );
		Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		spell->Init( caster, spellInfo ,true, NULL);
		spell->forced_basepoints[0] = tot_shield;
		SpellCastTargets targets( caster->GetGUID() );
		spell->prepare(&targets);
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleFrozenHeart(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( caster == NULL || caster->IsPlayer() == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->School != SCHOOL_FROST )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->belongs_to_player_class != caster->getClass() )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->eff[i].Effect != SPELL_EFFECT_SCHOOL_DAMAGE 
		&& sp->GetProto()->eff[i].Effect != SPELL_EFFECT_WEAPON_PERCENT_DAMAGE
		&& sp->GetProto()->eff[i].EffectApplyAuraName != SPELL_AURA_PERIODIC_DAMAGE )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	//Increases all frost damage done by 16%.  Each point of Mastery increases frost damage by an additional 2.0%.
	//$?s77514[Increases all frost damage done by $s1%.  Each point of Mastery increases frost damage by an additional ${$77514m2/100}.1%.]
	//[Increases all frost damage done by ${$77514m2/12.5}%.  Each point of Mastery increases frost damage by an additional ${$77514m2/100}.1%.]
	{
//		int32 var_inc = float2int32(  SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->mod_amount[0] );
		int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->ori_spell->eff[1].EffectBasePoints );
		int32 tot_inc_pct = var_inc / 100;
		int32 add_dmg = value * tot_inc_pct / 100;
		if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_WEAPON_PERCENT_DAMAGE )
			sp->forced_pct_mod_cur += tot_inc_pct;
		else
			value += add_dmg;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleExecutioner(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( caster == NULL || caster->IsPlayer() == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	//Increases the damage done by all your finishing moves, and the effectiveness of your Slice and Dice, by 16%.  Each point of Mastery increases damage by an additional 2.0%.
	//$?s76808[Increases the damage done by all your finishing moves by $76808s1%.  
	//Each point of Mastery increases damage by an additional ${$76808m3/100}.1%.]
	//[Increases the damage done by all your finishing moves by ${$76808m3/12.5}%.Each point of Mastery increases damage by an additional ${$76808m3/100}.1%.]
	if( ( sp->GetProto()->c_is_flags & SPELL_FLAG_IS_FINISHING_MOVE ) && ( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_WEAPON_DAMAGE || sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE ||  sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_PERIODIC_DAMAGE )
		|| ( sp->GetProto()->NameHash == SPELL_HASH_SLICE_AND_DICE && sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MOD_HASTE_NOREGEN ) )
	{
//		int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->mod_amount[0] );
		int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->ori_spell->eff[1].EffectBasePoints );
		int32 tot_inc_pct = var_inc / 100;
		int32 add_dmg = value * tot_inc_pct / 100;
		value += add_dmg;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandlePotentPoisons(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( caster == NULL || caster->IsPlayer() == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( ( sp->GetProto()->c_is_flags &  SPELL_FLAG_IS_POISON ) == 0 
			&& sp->GetProto()->NameHash != SPELL_HASH_ENVENOM 
			&& sp->GetProto()->NameHash != SPELL_HASH_VENOMOUS_WOUND	//SPELL_HASH_VENOMOUS_WOUNDS -> talent
			)
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->eff[i].Effect != SPELL_EFFECT_SCHOOL_DAMAGE 
		&& sp->GetProto()->eff[i].EffectApplyAuraName != SPELL_AURA_PERIODIC_DAMAGE 
		)
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	//Increases the damage done by your poisons by 28%.  Each point of Mastery increases damage by an additional 3.5%
	{
//		int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->mod_amount[0] );
		int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->ori_spell->eff[2].EffectBasePoints );
		int32 tot_inc_pct = var_inc / 100;
		int32 add_dmg = value * tot_inc_pct / 100;
		value += add_dmg;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleTotalEclipse(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( caster == NULL || caster->IsPlayer() == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	Player *p_caster = SafePlayerCast( caster );
	//this is a whole story:
	// we start with 0 solar energy. At this point the state of the solar bar is not decided
	// once you cast a spell that will modify the bar, an arrow is added to show the client in which direction it is possible to move the arrow
	// until you reach an eclipse it is only possible to move the arrow in that direction
	// once the eclipse is reached the arrow direction is changed and you can move the power bar the other eclipse way
	// once the bar is at 0 the eclipse buff is removed from the caster until the other eclipse bar is reached

#define SPELL_ID_ECLIPSE_LUNAR_BUFF 48518				//mod arcane spell dmg by x%
#define SPELL_ID_ECLIPSE_SOLAR_BUFF 48517				//mod nature spell dmg by x%
//#define SPELL_ID_ECLIPSE_POWERBAR_MOVE_LUNAR_BUFF 67484	//shows a small arrow on the power bar that eclipse is moving to lunar
//#define SPELL_ID_ECLIPSE_POWERBAR_MOVE_SOLAR_BUFF 67483	//shows a small arrow on the power bar that eclipse is moving to lunar
#define SPELL_ID_COMBAT_LOG_ECLIPSE_POWER_GAIN	  89265
#define ECPLISE_LUNAR_GAIN_AMT	-13
#define ECPLISE_LUNAR_GAIN_AMT2	-8
#define ECPLISE_SOLAR_GAIN_AMT	20
#define ECPLISE_SOLAR_GAIN_AMT2	8
#define ECPLISE_ECLIPSE_GAIN_AMT	15	//starsurge is special and will always give 
#define SPELL_ID_SUNFIRE_INSTEAD_MOONFIRE 93402
#define SPELL_ID_MOONFIRE_INSTEAD_SUNFIRE 8921

	if( p_caster->m_eclipsebar_state & ( ECLIPSE_BAR_MOVING_TO_LUNAR | ECLIPSE_BAR_NOT_DECIDED_STATE ) )
	{
		int32 gain;
		if( sp->GetProto()->NameHash == SPELL_HASH_WRATH )
		{
			gain = ECPLISE_LUNAR_GAIN_AMT;
			if( p_caster->HasAuraWithNameHash( SPELL_HASH_ITEM___DRUID_T12_BALANCE_4P_BONUS, 0, AURA_SEARCH_PASSIVE ) )
				gain -= 3;
		}
		else if( sp->GetProto()->NameHash == SPELL_HASH_INSECT_SWARM )
			gain = ECPLISE_LUNAR_GAIN_AMT2;
		else if( sp->GetProto()->NameHash == SPELL_HASH_SUNFIRE )
			gain = ECPLISE_LUNAR_GAIN_AMT2;
		else if( sp->GetProto()->NameHash == SPELL_HASH_STARSURGE )
			gain = -ECPLISE_ECLIPSE_GAIN_AMT;
		else
			gain = 0;
		if( gain != 0 )
		{
//			if( p_caster->m_eclipsebar_state == ECLIPSE_BAR_NOT_DECIDED_STATE )
//				p_caster->CastSpell( p_caster, SPELL_ID_ECLIPSE_POWERBAR_MOVE_LUNAR_BUFF, true );
			p_caster->m_eclipsebar_state = ECLIPSE_BAR_MOVING_TO_LUNAR;
			int32 cur = p_caster->GetPower(  POWER_TYPE_ECLIPSE );
			if( cur + gain < -100 )
				gain = -100 - cur;
			//give 13 lunar energy. Might need to convert this into a spell cast to let spell moders have effect
			p_caster->Energize( p_caster, SPELL_ID_COMBAT_LOG_ECLIPSE_POWER_GAIN, gain, POWER_TYPE_ECLIPSE, 0 ); 
			//check if we need to remove SOLAR eclipse
			int32 new_val = p_caster->GetPower(  POWER_TYPE_ECLIPSE );
			if( cur > 0 && new_val <= 0 )
			{
				p_caster->RemoveAura( SPELL_ID_ECLIPSE_SOLAR_BUFF );
				//sunfire talent added sunfire spell, we remove that
//				p_caster->addSpell( SPELL_ID_MOONFIRE_INSTEAD_SUNFIRE, true, false );
				p_caster->SwapActionButtonSpell( SPELL_ID_SUNFIRE_INSTEAD_MOONFIRE, SPELL_ID_MOONFIRE_INSTEAD_SUNFIRE, true, true );
//				p_caster->removeSpell( SPELL_ID_SUNFIRE_INSTEAD_MOONFIRE, false, false, 0 );
			}
			//check if we need to inverse the power bar direction
			if( new_val <= -100 )
			{
				p_caster->m_eclipsebar_state = ECLIPSE_BAR_MOVING_TO_SOLAR;
				p_caster->CastSpell( p_caster, SPELL_ID_ECLIPSE_LUNAR_BUFF, true );
//				p_caster->RemoveAura( SPELL_ID_ECLIPSE_POWERBAR_MOVE_LUNAR_BUFF );
//				p_caster->CastSpell( p_caster, SPELL_ID_ECLIPSE_POWERBAR_MOVE_SOLAR_BUFF, true );
			}
		}
	}
	if( p_caster->m_eclipsebar_state & ( ECLIPSE_BAR_MOVING_TO_SOLAR | ECLIPSE_BAR_NOT_DECIDED_STATE ) )
	{
		int32 gain;
		if( sp->GetProto()->NameHash == SPELL_HASH_STARFIRE )
		{
			gain = ECPLISE_SOLAR_GAIN_AMT;
			if( p_caster->HasAuraWithNameHash( SPELL_HASH_ITEM___DRUID_T12_BALANCE_4P_BONUS, 0, AURA_SEARCH_PASSIVE ) )
				gain += 5;
		}
		else if( sp->GetProto()->NameHash == SPELL_HASH_MOONFIRE )
			gain = ECPLISE_SOLAR_GAIN_AMT2;
		else if( sp->GetProto()->NameHash == SPELL_HASH_STARSURGE )
			gain = ECPLISE_ECLIPSE_GAIN_AMT;
		else
			gain = 0;
		if( gain != 0 )
		{
//			if( p_caster->m_eclipsebar_state == ECLIPSE_BAR_NOT_DECIDED_STATE )
//				p_caster->CastSpell( p_caster, SPELL_ID_ECLIPSE_POWERBAR_MOVE_SOLAR_BUFF, true );
			p_caster->m_eclipsebar_state = ECLIPSE_BAR_MOVING_TO_SOLAR;
			int32 cur = p_caster->GetPower(  POWER_TYPE_ECLIPSE );
			//give 20 solar energy. Might need to convert this into a spell cast to let spell moders have effect
			p_caster->Energize( p_caster, SPELL_ID_COMBAT_LOG_ECLIPSE_POWER_GAIN, gain, POWER_TYPE_ECLIPSE, 0 ); 
			//check if we need to remove LUNAR eclipse
			int32 new_val = p_caster->GetPower(  POWER_TYPE_ECLIPSE );
			if( cur < 0 && new_val >= 0 )
				p_caster->RemoveAura( SPELL_ID_ECLIPSE_LUNAR_BUFF );
			//check if we need to inverse the power bar direction
			if( new_val >= 100 )
			{
				p_caster->m_eclipsebar_state = ECLIPSE_BAR_MOVING_TO_LUNAR;
				p_caster->CastSpell( p_caster, SPELL_ID_ECLIPSE_SOLAR_BUFF, true );
//				p_caster->RemoveAura( SPELL_ID_ECLIPSE_POWERBAR_MOVE_SOLAR_BUFF );
//				p_caster->CastSpell( p_caster, SPELL_ID_ECLIPSE_POWERBAR_MOVE_LUNAR_BUFF, true );
				//sunfire talent will morph moonfire into sunfire
				if( p_caster->HasAuraWithNameHash( SPELL_HASH_SUNFIRE, 0, AURA_SEARCH_PASSIVE ) )
				{
//					p_caster->addSpell( SPELL_ID_SUNFIRE_INSTEAD_MOONFIRE, true, true );
					p_caster->SwapActionButtonSpell( SPELL_ID_MOONFIRE_INSTEAD_SUNFIRE, SPELL_ID_SUNFIRE_INSTEAD_MOONFIRE, true, true );
//					p_caster->removeSpell( SPELL_ID_MOONFIRE_INSTEAD_SUNFIRE, false, false, 0 );
				}
			}
		}
	}

	//Increases the bonus damage from Eclipse by 12%.  Each point of Mastery increases the bonus by an additional 1.5%.
	if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MOD_DAMAGE_PERCENT_DONE &&
		( sp->GetProto()->NameHash == SPELL_HASH_ECLIPSE__LUNAR_ || sp->GetProto()->NameHash == SPELL_HASH_ECLIPSE__SOLAR_ ))
	{
//		int32 var_inc = float2int32( p_caster->GetFloatValue( PLAYER_MASTERY ) * so->mod_amount[0] );
		int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->ori_spell->eff[1].EffectBasePoints );
		int32 tot_inc_pct = var_inc / 100;
		int32 add_dmg = value * tot_inc_pct / 100;
		value += add_dmg;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleDeepHealing(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( target == NULL || caster == NULL || caster->IsPlayer() == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	//not a heal spell in any of the effects ? we have no deal here
	if( ( sp->GetProto()->c_is_flags & SPELL_FLAG_IS_HEALING ) == 0 )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	//we need a direct heal spell = no ticks pls
	if( sp->GetProto()->eff[ i ].EffectAmplitude != 0 )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->belongs_to_player_class != caster->getClass() )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	//Increases the potency of your direct healing spells by up to 20%, based on the current health level of your target (lower health targets are healed for more).  Each point of Mastery increases direct heals by up to an additional 2.5%.
	{
//		int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->mod_amount[0] );
		int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->ori_spell->eff[1].EffectBasePoints );
		int32 tot_inc_pct = var_inc / 100;
		//values is diminished if target is close to 100% pct health
		int32 target_health_pct = target->GetHealthPct();
		int32 diminshing_contribution = 100 - target_health_pct / 2;	//no idea about this really :(
		tot_inc_pct = tot_inc_pct * diminshing_contribution / 100;
		sp->forced_pct_mod_cur += tot_inc_pct;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
	
SPELL_EFFECT_OVERRIDE_RETURNS SMT_77223(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( caster == NULL || caster->IsPlayer() == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( ( sp->GetProto()->SchoolMask &  ( SCHOOL_MASK_FIRE | SCHOOL_MASK_FROST | SCHOOL_MASK_NATURE ) ) == 0 )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->eff[i].Effect != SPELL_EFFECT_SCHOOL_DAMAGE 
		&& sp->GetProto()->eff[i].EffectApplyAuraName != SPELL_AURA_PERIODIC_DAMAGE 
		&& sp->GetProto()->eff[i].Effect != SPELL_EFFECT_WEAPON_PERCENT_DAMAGE	) //Lava Lash
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->belongs_to_player_class != caster->getClass() 
		&& sp->GetProto()->NameHash != SPELL_HASH_SHADOWBLAST
		&& sp->GetProto()->NameHash != SPELL_HASH_ICEBLAST
		&& sp->GetProto()->NameHash != SPELL_HASH_FLAMEBLAST
		&& sp->GetProto()->NameHash != SPELL_HASH_FLAMETONGUE_ATTACK
		&& sp->GetProto()->NameHash != SPELL_HASH_FROSTBRAND_ATTACK
		&& sp->GetProto()->NameHash != SPELL_HASH_AVALANCHE
		)
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

/*
	if( sp->GetProto()->eff[i].EffectSpellPowerCoef == ITEM_SPELL_DEFAULT_SP_BENEFIT_COEF //do not boost item casted spells
		|| sp->pSpellId != 0 ) //proc spell, might come from enchant
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
*/
	//Increases all Fire, Frost, and Nature damage done by 20%.  Each point of Mastery increases damage by an additional 2.5%
	{
//		int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->mod_amount[0] );
		int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->ori_spell->eff[1].EffectBasePoints );
		int32 tot_inc_pct = var_inc / 100;
//		sp->forced_pct_mod_cur += tot_inc_pct;
		sp->forced_pct_mod_cur = sp->forced_pct_mod_cur * ( 100 + tot_inc_pct ) / 100;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
	
SPELL_EFFECT_OVERRIDE_RETURNS HandleUnshackledFury(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( caster == NULL || caster->IsPlayer() == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

	if( 
//		( sp->GetProto()->NameHash == SPELL_HASH_RAGING_BLOW && sp->GetProto()->eff[i].Effect == SPELL_EFFECT_WEAPON_DAMAGE )
		( sp->GetProto()->NameHash == SPELL_HASH_RAGING_BLOW && sp->GetProto()->eff[i].Effect == SPELL_EFFECT_WEAPON_PERCENT_DAMAGE )
		|| ( sp->GetProto()->NameHash == SPELL_HASH_DEATH_WISH && sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MOD_DAMAGE_PERCENT_DONE )
		|| ( sp->GetProto()->NameHash == SPELL_HASH_ENRAGE  )
//		|| ( sp->GetProto()->NameHash == SPELL_HASH_BERSERKER_RAGE )
		|| ( sp->GetProto()->NameHash == SPELL_HASH_UNHOLY_FRENZY && sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MELEE_SLOW_PCT )
		|| ( sp->GetProto()->NameHash == SPELL_HASH_ENRAGED_REGENERATION && sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MOD_TOTAL_HEALTH_REGEN_PCT )
		)
	//Increases the benefit of abilities that cause or require you to be enraged by 37.6%. Each point of Mastery increases enrage effects by an additional 4.70%.
	{
//		int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->mod_amount[0] );
		int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->ori_spell->eff[2].EffectBasePoints );
		int32 tot_inc_pct = var_inc / 100;
		int32 add_val = value * tot_inc_pct / 100;
		value += add_val;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
	
SPELL_EFFECT_OVERRIDE_RETURNS HandleEssenceOfTheViper(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( caster == NULL || caster->IsPlayer() == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->SchoolMask <= 1 )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->eff[i].Effect != SPELL_EFFECT_SCHOOL_DAMAGE 
		&& sp->GetProto()->eff[i].EffectApplyAuraName != SPELL_AURA_PERIODIC_DAMAGE 
		&& sp->GetProto()->eff[i].Effect != SPELL_EFFECT_WEAPON_PERCENT_DAMAGE	)
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->belongs_to_player_class != caster->getClass() )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	//Increases all elemental damage you deal by 8%.  Each point of Mastery increases elemental damage by an additional 1.0%.
	{
//		int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->mod_amount[0] );
		int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->ori_spell->eff[1].EffectBasePoints );
		int32 tot_inc_pct = var_inc / 100;
//		sp->forced_pct_mod_cur += tot_inc_pct;
		sp->forced_pct_mod_cur = sp->forced_pct_mod_cur * ( 100 + tot_inc_pct ) / 100;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
	
SPELL_EFFECT_OVERRIDE_RETURNS HandleFieryApocalypse(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( caster == NULL || caster->IsPlayer() == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( ( sp->GetProto()->SchoolMask & SCHOOL_MASK_FIRE) == 0 )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->eff[i].Effect != SPELL_EFFECT_SCHOOL_DAMAGE 
		&& sp->GetProto()->eff[i].EffectApplyAuraName != SPELL_AURA_PERIODIC_DAMAGE )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->belongs_to_player_class != caster->getClass() )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	//Increases all fire damage you deal by 10%.  Each point of Mastery increases fire damage by an additional 1.25%.
	{
//		int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->mod_amount[0] );
		int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->ori_spell->eff[1].EffectBasePoints );
		int32 tot_inc_pct = var_inc / 100;
//		sp->forced_pct_mod_cur += tot_inc_pct;
		sp->forced_pct_mod_cur = sp->forced_pct_mod_cur * ( 100 + tot_inc_pct ) / 100;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
	
SPELL_EFFECT_OVERRIDE_RETURNS HandlePotentAfflictions(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( caster == NULL || caster->IsPlayer() == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( ( sp->GetProto()->SchoolMask & SCHOOL_MASK_SHADOW ) == 0 )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->eff[i].EffectApplyAuraName != SPELL_AURA_PERIODIC_DAMAGE )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->belongs_to_player_class != caster->getClass() )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	//Increases all periodic shadow damage you deal by 13.04%.  Each point of Mastery increases periodic shadow damage by an additional 1.63%.
	{
//		int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->mod_amount[0] );
		int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->ori_spell->eff[1].EffectBasePoints );
		int32 tot_inc_pct = var_inc / 100;
//		sp->forced_pct_mod_cur += tot_inc_pct;
		sp->forced_pct_mod_cur = sp->forced_pct_mod_cur * ( 100 + tot_inc_pct ) / 100;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
	
SPELL_EFFECT_OVERRIDE_RETURNS HandleNatureSBlessing(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( ( sp->GetProto()->c_is_flags & SPELL_FLAG_IS_HEALING ) == 0 )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

	if( ( sp->GetProto()->c_is_flags & SPELL_FLAG_IS_HEAL_OVER_TIME ) != 0 )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

	if( target && target->HasAuraWithNameHash( SPELL_HASH_EARTH_SHIELD, 0, AURA_SEARCH_POSITIVE ) == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

//	sp->forced_pct_mod_cur += so->mod_amount[0];
	sp->forced_pct_mod_cur = sp->forced_pct_mod_cur * ( 100 + so->mod_amount[0] ) / 100;

	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
	
SPELL_EFFECT_OVERRIDE_RETURNS HandleMoltenFury(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( target && target->GetHealthPct() > 35 )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

	if( ( sp->GetProto()->c_is_flags & SPELL_FLAG_IS_DAMAGING ) == 0 )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

	if( IsDamagingSpell(sp->GetProto(),i) == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

	sp->forced_pct_mod_cur += so->mod_amount[0];	//this mods dmg and absorb only...so it's ok :P

	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleRendAndTear(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if(  sp->GetProto()->NameHash != SPELL_HASH_MAUL && sp->GetProto()->NameHash != SPELL_HASH_SHRED )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

	if( sp->GetProto()->eff[i].Effect != SPELL_EFFECT_WEAPON_PERCENT_DAMAGE	
		&& sp->GetProto()->eff[i].Effect != SPELL_EFFECT_SCHOOL_DAMAGE
		)
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

	if( target == NULL || target->HasAuraStateFlag( AURASTATE_BLEED ) == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

	sp->forced_pct_mod_cur += so->mod_amount[0];	

	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleDeatsEmbrace(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if(  sp->GetProto()->NameHash == SPELL_HASH_DRAIN_LIFE )
	{
		if( caster == NULL || caster->GetHealthPct() > 25 )
			return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

		if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MOD_TOTAL_HEALTH_REGEN_PCT )
			value += so->mod_amount_pct[ 0 ];
	}
	else
	{
		if( target == NULL || target->GetHealthPct() > 25 )
			return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

		if( sp->GetProto()->School != SCHOOL_SHADOW )
			return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

		if( ( sp->GetProto()->c_is_flags & SPELL_FLAG_IS_DAMAGING ) == 0 )
			return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

		if( IsDamagingSpell(sp->GetProto(),i) == false )
			return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

		sp->forced_pct_mod_cur += so->mod_amount_pct[1];	//this mods dmg and absorb only...so it's ok :P
	}

	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
/*
SPELL_EFFECT_OVERRIDE_RETURNS HandleDeatsEmbrace(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( sp->GetProto()->NameHash == SPELL_HASH_DRAIN_LIFE && caster && caster->GetHealthPct() < 20)
		value = value * so->mod_amount_pct[ 0 ] / 100;
	if( target && target->GetHealthPct() < 35 && sp->GetProto()->School == SCHOOL_SHADOW )
		value = value * so->mod_amount_pct[ 1 ] / 100;
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
*/

SPELL_EFFECT_OVERRIDE_RETURNS HandleShatter(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if(  sp->GetProto()->NameHash != SPELL_HASH_FROSTBOLT )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

	if( sp->GetProto()->eff[i].Effect != SPELL_EFFECT_SCHOOL_DAMAGE )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

	if( target->HasAuraStateFlag( AURASTATE_FROZEN ) == false 
//		&& ( caster && caster->IsPlayer() && SafePlayerCast( caster )->m_ForceTargetFrozen <= 0 ) //fingers of frost should only affect ice lance and deep freez
		)
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

	sp->forced_pct_mod_cur += so->mod_amount_pct[0];	

	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleFingersOfFrost(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( sp->GetProto()->eff[i].Effect != SPELL_EFFECT_SCHOOL_DAMAGE )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

	if( sp->GetProto()->NameHash != SPELL_HASH_ICE_LANCE )
	{
		Aura *a = NULL;
		if( sp->GetProto()->NameHash == SPELL_HASH_FROSTFIRE_BOLT )
			a = caster->HasAuraWithNameHash( SPELL_HASH_BRAIN_FREEZE, 0, AURA_SEARCH_POSITIVE );
		if( a == NULL )
			return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	}

	//increase dmg by X%
	sp->forced_pct_mod_cur += so->mod_amount_pct[0];	

	//somehow we should increase crit chance and stuff

	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleImprovedDeathStrike(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if(  sp->GetProto()->NameHash != SPELL_HASH_DEATH_STRIKE )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_HEAL_MAXHEALTH_PCT )
	{
		//increase healing by X%
		value += value * so->ori_spell->eff[2].EffectBasePoints / 100;	
	}
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_WEAPON_PERCENT_DAMAGE )
	{
//		sp->forced_pct_mod_cur += ( so->ori_spell->eff[0].EffectBasePoints - 100 );	
		sp->forced_pct_mod_cur = ( sp->forced_pct_mod_cur * ( so->ori_spell->eff[0].EffectBasePoints ) ) / 100;
	}

	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandlePriestT13Healer4P(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( i == 0 )
	{
		if(  sp->GetProto()->NameHash == SPELL_HASH_POWER_WORD__SHIELD )
		{
			if( RandChance( so->ori_spell->eff[0].EffectBasePoints ) )
				sp->forced_pct_mod[0] += so->ori_spell->eff[1].EffectBasePoints;
		}
//		else if(  sp->GetProto()->NameHash == SPELL_HASH_RUPTURE )
//			value += so->ori_spell->eff[1].EffectBasePoints;
		else if(  sp->GetProto()->NameHash == SPELL_HASH_HOLY_WORD__CHASTISE 
			|| sp->GetProto()->NameHash == SPELL_HASH_HOLY_WORD__SANCTUARY
			|| sp->GetProto()->NameHash == SPELL_HASH_HOLY_WORD__SERENITY
			)
		{
			sp->forced_duration = sp->GetDuration() * ( 100 + so->ori_spell->eff[2].EffectBasePoints ) / 100;
			sp->Dur = sp->forced_duration;	//just to be sure
		}
	}

	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleWeightedBlades(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( sp->GetProto()->NameHash != SPELL_HASH_SINISTER_STRIKE
		&& sp->GetProto()->NameHash != SPELL_HASH_REVEALING_STRIKE )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->eff[i].Effect != SPELL_EFFECT_WEAPON_PERCENT_DAMAGE )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	sp->forced_pct_mod_cur += so->ori_spell->eff[0].EffectBasePoints;
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS SMT_16266(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	/*
	Increases the passive bonuses granted by your Flametongue Weapon and Earthliving Weapon abilities by $s1%, 
	the damage of your extra attacks from Windfury Weapon by $s3%, 
	and the effectiveness of the ongoing benefits of your Unleash Elements ability by $s2%.
	*/
	if( sp->GetProto()->NameHash == SPELL_HASH_FLAMETONGUE_ATTACK )
	{
		if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE )
		{
//			value += value * so->ori_spell->eff[0].EffectBasePoints / 100;
			sp->forced_pct_mod_cur += so->ori_spell->eff[0].EffectBasePoints;
		}
	}
	else if( sp->GetProto()->NameHash == SPELL_HASH_EARTHLIVING_WEAPON__PASSIVE_ )
	{
		if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MOD_HEALING_DONE )
			value += value * so->ori_spell->eff[0].EffectBasePoints / 100;
	}
	else if( sp->GetProto()->NameHash == SPELL_HASH_WINDFURY_WEAPON )
	{
		if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_DUMMY )
			value += value * so->ori_spell->eff[0].EffectBasePoints / 100;
	}
	else if( sp->GetProto()->NameHash == SPELL_HASH_UNLEASH_WIND
		|| sp->GetProto()->NameHash == SPELL_HASH_UNLEASH_FROST
		|| sp->GetProto()->NameHash == SPELL_HASH_UNLEASH_FLAME
		|| sp->GetProto()->NameHash == SPELL_HASH_UNLEASH_EARTH
		|| sp->GetProto()->NameHash == SPELL_HASH_UNLEASH_LIFE )
	{
		if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_APPLY_AURA )
			value += value * so->ori_spell->eff[0].EffectBasePoints / 100;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS SMT_33597(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
//When you cast your Innervate on yourself, you regain an additional $s1% of your total mana over its duration.
	if( sp->GetProto()->NameHash == SPELL_HASH_INNERVATE && target == caster )
	{
		if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_PERIODIC_ENERGIZE )
		{
			int32 max_mana = caster->GetMaxPower( POWER_TYPE_MANA );
			int32 pct_mana = max_mana * so->ori_spell->eff[0].EffectBasePoints / 100;
			int32 pct_mana_tick = pct_mana / sp->GetProto()->quick_tickcount;
			value += pct_mana_tick;
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
	
SPELL_EFFECT_OVERRIDE_RETURNS SMT_14748(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( sp->GetProto()->eff[i].EffectApplyAuraName != SPELL_AURA_SCHOOL_ABSORB )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->NameHash != SPELL_HASH_POWER_WORD__SHIELD )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

//	sp->forced_pct_mod_cur += so->ori_spell->eff[0].EffectBasePoints;
	sp->forced_pct_mod_cur = ( sp->forced_pct_mod_cur * (100 + so->ori_spell->eff[0].EffectBasePoints ) ) / 100;

	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
	
SPELL_EFFECT_OVERRIDE_RETURNS SMT_77484(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( caster == NULL || caster->IsPlayer() == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->eff[i].EffectApplyAuraName != SPELL_AURA_SCHOOL_ABSORB )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->belongs_to_player_class != caster->getClass() )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	//Increases  the potency of all your damage absorption spells by 20%.  Each point of Mastery increases the potency of absorbs by an additional 2.5%.
	{
		int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->ori_spell->eff[1].EffectBasePoints );
		int32 tot_inc_pct = var_inc / 100;
		tot_inc_pct += so->ori_spell->eff[0].EffectBasePoints;
//		sp->forced_pct_mod_cur += tot_inc_pct;
		sp->forced_pct_mod_cur = ( sp->forced_pct_mod_cur * (100 + tot_inc_pct ) ) / 100;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
	
SPELL_EFFECT_OVERRIDE_RETURNS SMT_77616(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( caster == NULL || caster->IsPlayer() == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( SafePlayerCast( caster )->mSpellReplaces[ 77606 ] != sp->GetProto()->Id )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	//cases when this spell is not healer or dmg spell ?
	if( so->mod_amount[0] <= 0 )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( i == 0 && ( IsDamagingSpell( sp->GetProto(), i ) == true || IsHealingEffect( sp->GetProto(), i ) >= 1 ) )
	{
		value = so->mod_amount[0];
		sp->static_dmg[i] = value;
		sp->forced_basepoints[i] = value;
		return SPELL_EFFECT_OVERRIDE_BREAK_EXECUTION;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
	
SPELL_EFFECT_OVERRIDE_RETURNS SMT_91316(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( sp->ProcedOnSpell == NULL || sp->ProcedOnSpell->NameHash != SPELL_HASH_PESTILENCE )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	sp->forced_pct_mod_cur += so->mod_amount[0];
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
	
SPELL_EFFECT_OVERRIDE_RETURNS SMT_47266(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( sp->GetProto()->NameHash != SPELL_HASH_INCINERATE && sp->GetProto()->NameHash != SPELL_HASH_CHAOS_BOLT )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	Aura *a = target->HasAuraWithNameHash( SPELL_HASH_IMMOLATE, 0, AURA_SEARCH_NEGATIVE );
	if( a == NULL )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	sp->forced_pct_mod_cur += so->mod_amount[0];
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
	
SPELL_EFFECT_OVERRIDE_RETURNS SMT_77700(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
// Increases the damage of your Lava Lash ability by $s2% for each application of your Searing Flames on the target,
// consuming them in the process, 
// and causes your Lava Lash to spread your Flame Shock from the target to up to four enemies within 12 yards.
	if( sp->GetProto()->NameHash != SPELL_HASH_LAVA_LASH )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( target == NULL )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	//how much can we increase the dmg ?
	int32 AuraCounter = target->RemoveAuraByNameHash( SPELL_HASH_SEARING_FLAMES, 0, AURA_SEARCH_NEGATIVE, MAX_PASSIVE_AURAS );
	//seems like no win case
	if( AuraCounter != 0 )
	{
//		sp->forced_pct_mod_cur += MIN( so->mod_amount[0] * AuraCounter, 500 ); //sanity check
		sp->forced_pct_mod_cur = sp->forced_pct_mod_cur * ( 100 + MIN( so->mod_amount[0] * AuraCounter, 500 ) ) / 100; //sanity check

		Aura *b = caster->HasAuraWithNameHash( SPELL_HASH_ITEM___SHAMAN_T12_ENHANCEMENT_2P_BONUS, 0, AURA_SEARCH_PASSIVE );
		if( b )
//			sp->forced_pct_mod_cur += AuraCounter * b->GetSpellProto()->eff[0].EffectBasePoints;
			sp->forced_pct_mod_cur = sp->forced_pct_mod_cur * (100 + AuraCounter * b->GetSpellProto()->eff[0].EffectBasePoints ) / 100;
	}

	//now spread up to 4 targets the Flame Shock if we can
	Aura *FS = target->HasAuraWithNameHash( SPELL_HASH_FLAME_SHOCK, 0, AURA_SEARCH_NEGATIVE );
	//no win
	if( FS != NULL )
	{
		SpellEntry *spe = FS->GetSpellProto();
		//now spread it
		float r = 12.0f*12.0f;
		int32 hit_counter = 0;
		caster->AquireInrangeLock(); //make sure to release lock before exit function !
		InrangeLoopExitAutoCallback AutoLock;
		for(InRangeSetRecProt::iterator itr = caster->GetInRangeSetBegin( AutoLock ); itr != caster->GetInRangeSetEnd(); itr++ )
		{
			// don't add objects that are not units and that are dead
			if( !( (*itr)->IsUnit() ) || ! SafeUnitCast( *itr )->isAlive() )
				continue;

			Unit *ttarget = SafeUnitCast( ( *itr ) );
			//do not refresh buff on target
			if( ttarget == target )
				continue;
			//this can be found in blizz changelog, no croud controlled units get affected
			if( ttarget->IsStunned() || ttarget->IsFeared() )
				continue;
			if( IsInrange(target,ttarget,r) )
			{
				if( isAttackable( caster, ttarget, !( sp->GetProto()->c_is_flags & SPELL_FLAG_IS_TARGETINGSTEALTHED ) ) )
				{
					uint8 did_hit_result = sp->DidHit(i, target );
					if( did_hit_result == SPELL_DID_HIT_SUCCESS )
					{
						Spell*sp=SpellPool.PooledNew( __FILE__, __LINE__ );
						sp->Init(caster, spe, true, NULL);
						SpellCastTargets tgt( ttarget->GetGUID() );
						sp->ProcedOnSpell = sp->GetProto();	
						if( spe->eff[0].EffectApplyAuraName != SPELL_AURA_PERIODIC_DAMAGE )
							sp->redirected_effect[0] = SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_ALL_TARGETS;
						if( spe->eff[1].EffectApplyAuraName != SPELL_AURA_PERIODIC_DAMAGE )
							sp->redirected_effect[1] = SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_ALL_TARGETS;
						if( spe->eff[2].EffectApplyAuraName != SPELL_AURA_PERIODIC_DAMAGE )
							sp->redirected_effect[2] = SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_ALL_TARGETS;
						sp->prepare(&tgt);
						//do we quit now ?
						hit_counter++;
						if( hit_counter >= 4 )
							break;
					}
				}
			}
		}
		caster->ReleaseInrangeLock();
	}

	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
	
SPELL_EFFECT_OVERRIDE_RETURNS SMT_63373(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
// Increases the damage done by your Lightning Bolt, Chain Lightning, Lava Lash, and Shock spells by $s1% 
// on targets afflicted by your Frostbrand Attack effect,
	//quickest check
	if( sp->GetProto()->eff[i].Effect != SPELL_EFFECT_SCHOOL_DAMAGE || target == NULL )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	//are we moddinf proper spell ?
	if( sp->GetProto()->NameHash != SPELL_HASH_LIGHTNING_BOLT 
		&& sp->GetProto()->NameHash != SPELL_HASH_CHAIN_LIGHTNING
		&& sp->GetProto()->NameHash != SPELL_HASH_LAVA_LASH
		&& sp->GetProto()->NameHash != SPELL_HASH_EARTH_SHOCK
		&& sp->GetProto()->NameHash != SPELL_HASH_FROST_SHOCK
		&& sp->GetProto()->NameHash != SPELL_HASH_FLAME_SHOCK )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	//does the target have the condition ?
	Aura *a = target->HasAuraWithNameHash( SPELL_HASH_FROSTBRAND_ATTACK, 0, AURA_SEARCH_NEGATIVE );
	if( a == NULL )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	//increase dmg pct
	sp->forced_pct_mod_cur += so->mod_amount[0];
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
	
SPELL_EFFECT_OVERRIDE_RETURNS SMT_20234(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	// Gives your Word of Glory a $s1% increased critical chance when used on targets with 35% or less health.
	if( sp->GetProto()->NameHash != SPELL_HASH_WORD_OF_GLORY )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( target == NULL || target->GetHealthPct() > 35 )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( caster == NULL )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	//mod the spell heal crit chance
//	sp->forced_heal_crit_mod[i] += so->mod_amount[0];
	int64 *data;
	data = caster->GetCreateIn64Extension( EXTENSION_ID_ONETIME_TARGETSPECIFIC_CRIT_MOD_TARGET );
	*data = target->GetGUID();
	data = caster->GetCreateIn64Extension( EXTENSION_ID_ONETIME_TARGETSPECIFIC_CRIT_MOD_SPELL );
	*data = sp->GetProto()->Id;
	data = caster->GetCreateIn64Extension( EXTENSION_ID_ONETIME_TARGETSPECIFIC_CRIT_MOD_VAL );
	*data = so->mod_amount[0];
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
	
SPELL_EFFECT_OVERRIDE_RETURNS SMT_58618(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	// Increases the Silence duration of your Strangulate ability by ${$58618m1/1000} sec when used on a target who is casting a spell.
	if( sp->GetProto()->NameHash != SPELL_HASH_STRANGULATE )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( target == NULL || target->isCasting() == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	sp->forced_duration = sp->GetDuration() + so->mod_amount[0];
	sp->Dur = sp->forced_duration;	//just to be sure
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
	
SPELL_EFFECT_OVERRIDE_RETURNS SMT_55692(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	// Your Smite spell inflicts an additional 20% damage against targets afflicted by Holy Fire.
	if( sp->GetProto()->NameHash != SPELL_HASH_SMITE )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( target == NULL || target->HasAuraWithNameHash( SPELL_HASH_HOLY_FIRE, 0, AURA_SEARCH_NEGATIVE ) == NULL )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	sp->forced_pct_mod_cur += so->mod_amount[0];
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
	
SPELL_EFFECT_OVERRIDE_RETURNS SMT_61205(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	// Increases the damage done by your Frostfire Bolt by $s1% and your Frostfire Bolt now deals $s3% additional damage over 12 sec, stacking up to 3 times, but no longer reduces the victim's movement speed.
	if( sp->GetProto()->NameHash != SPELL_HASH_FROSTFIRE_BOLT )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MOD_DECREASE_SPEED )
	{
		sp->redirected_effect[i] = SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_ALL_TARGETS;
//		so->mod_amount[0] = 0;	//making sure to clear the value for effect 3
		return SPELL_EFFECT_OVERRIDE_BREAK_EXECUTION;
	}
	else if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE )
	{
		sp->forced_pct_mod_cur += so->ori_spell->eff[0].EffectBasePoints;
//		so->mod_amount[0] = value;
	}
	else if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_PERIODIC_DAMAGE )
	{
		//take value from direct damage. Get a portion of it and apply it as a DOT
//		value = so->mod_amount[0] * so->ori_spell->eff[2].EffectBasePoints / 100 / sp->GetProto()->quick_tickcount;
		value = sp->eff_values[1] * (100+so->ori_spell->eff[2].EffectBasePoints) / 100 / sp->GetProto()->quick_tickcount;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS SMT_77495(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( target == NULL || caster == NULL || caster->IsPlayer() == false || sp->pSpellId != 0 || sp->ProcedOnSpell != NULL )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	//check if direct heal
//	if( sp->GetProto()->eff[i].Effect != SPELL_EFFECT_HEAL 
//		&& sp->GetProto()->eff[i].Effect != SPELL_EFFECT_HEAL_MAX_HEALTH 
//		&& sp->GetProto()->eff[i].Effect != SPELL_EFFECT_HEAL_MAXHEALTH_PCT )
//		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

	if( sp->GetProto()->eff[i].Effect != SPELL_EFFECT_HEAL )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->belongs_to_player_class != caster->getClass() )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

	//Increases the potency of your healing spells by 10% on targets already affected by one of your heal over time spells.  Each point of Mastery increases heal potency by an additional 1.25%
//	int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->mod_amount[0] );
	int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->ori_spell->eff[1].EffectBasePoints );
	int32 tot_inc_pct = var_inc / 100; // -> mastery is scaled by 100 to store it in INT32
//	sp->forced_pct_mod_cur += tot_inc_pct; //increase SP contribution also
	sp->forced_pct_mod_cur = sp->forced_pct_mod_cur * (100 + tot_inc_pct ) / 100; //increase SP contribution also
//	value += value * tot_inc_pct / 100;	//increase base value only

	//also boost HOTs ( this spell also ? Or just next ? )
	caster->CastSpellDelayed( caster->GetGUID(), 100977, 1, true );

	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS SMT_76547(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( caster == NULL || caster->IsPlayer() == false || sp->pSpellId != 0 || sp->ProcedOnSpell != NULL )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->eff[i].Effect != SPELL_EFFECT_SCHOOL_DAMAGE 
		&& sp->GetProto()->eff[i].EffectApplyAuraName != SPELL_AURA_PERIODIC_DAMAGE )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->belongs_to_player_class != caster->getClass() )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	//Increases all spell damage done by up to 12%, based on the amount of mana the Mage has unspent.  Each point of Mastery increases damage done by up to an additional 1.5%.
	{
//		int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->mod_amount[0] );
		int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->ori_spell->eff[1].EffectBasePoints );
		int32 tot_inc_pct = var_inc / 100;
		int32 mana_PCT = (caster->GetPower(  POWER_TYPE_MANA ) + 1 ) * 100 / ( caster->GetMaxPower( POWER_TYPE_MANA ) + 1 );
		tot_inc_pct = tot_inc_pct * mana_PCT / 100;
		sp->forced_pct_mod_cur += tot_inc_pct; //increase SP contribution also
//		value += value * tot_inc_pct / 100;	//increase base value only
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS SMT_76613(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( caster == NULL || caster->IsPlayer() == false || sp->pSpellId != 0 || sp->ProcedOnSpell != NULL )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( ( target && target->HasAuraStateFlag( AURASTATE_FROZEN ) == 0 ) 
		&& !( SafePlayerCast( caster )->m_ForceTargetFrozen > 0 && ( sp->GetProto()->NameHash == SPELL_HASH_ICE_LANCE || sp->GetProto()->NameHash == SPELL_HASH_DEEP_FREEZE ) ) //fingers of frost should only affect ice lance and deep freez
		)
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->eff[i].Effect != SPELL_EFFECT_SCHOOL_DAMAGE 
		&& sp->GetProto()->eff[i].EffectApplyAuraName != SPELL_AURA_PERIODIC_DAMAGE )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->belongs_to_player_class != caster->getClass() )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	//All your spells deal 20% increased damage against Frozen targets.  Each point of Mastery increases damage by an additional 2.5%.
	{
//		int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->mod_amount[0] );
		int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->ori_spell->eff[1].EffectBasePoints );
		int32 tot_inc_pct = var_inc / 100;
//		tot_inc_pct += so->ori_spell->eff[0].EffectBasePoints;
//		sp->forced_pct_mod_cur += tot_inc_pct;	//increase SP contribution also
		sp->forced_pct_mod_cur = sp->forced_pct_mod_cur * ( 100 + tot_inc_pct) /100;	//i know this looks imba, but PCT multiplyers stack multiplicatively not additively
//		value += value * tot_inc_pct / 100;	//increase base value only
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS SMT_76595(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( caster == NULL || caster->IsPlayer() == false || sp->pSpellId != 0 || sp->ProcedOnSpell != NULL )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->eff[i].EffectApplyAuraName != SPELL_AURA_PERIODIC_DAMAGE )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( ( sp->GetProto()->SchoolMask & SCHOOL_MASK_FIRE ) == 0 )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->belongs_to_player_class != caster->getClass() )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	//Increases the damage done by all your periodic fire damage effects by 20%.  Each point of Mastery increases periodic damage done by an additional 2.5%.
	{
//		int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->mod_amount[0] );
		int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->ori_spell->eff[1].EffectBasePoints );
		int32 tot_inc_pct = var_inc / 100;
		sp->forced_pct_mod_cur += tot_inc_pct;	//increase SP contribution also
//		value += value * tot_inc_pct / 100;	//increase base value only
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS SMT_77493(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( caster == NULL || caster->IsPlayer() == false || sp->pSpellId != 0 || sp->ProcedOnSpell != NULL )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( ( sp->GetProto()->eff[i].EffectApplyAuraName != SPELL_AURA_PERIODIC_DAMAGE && sp->GetProto()->eff[i].Effect != SPELL_EFFECT_SCHOOL_DAMAGE ) )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( SafePlayerCast( caster )->GetShapeShift() != FORM_CAT )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->eff[i].EffectMechanic != MECHANIC_BLEEDING )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->belongs_to_player_class != caster->getClass() )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	//Increases the damage done by your bleed abilities by 25.04%.  Each point of Mastery increases bleed damage by an additional 3.1%
	{
//		int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->mod_amount[0] );
		int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->ori_spell->eff[1].EffectBasePoints );
		int32 tot_inc_pct = var_inc / 100;
		sp->forced_pct_mod_cur += tot_inc_pct;	//increase SP contribution also
//		value += value * tot_inc_pct / 100;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS SMT_77494(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( caster == NULL || caster->IsPlayer() == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->NameHash != SPELL_HASH_SAVAGE_DEFENSE )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( SafePlayerCast( caster )->GetShapeShift() != FORM_BEAR )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->eff[i].EffectApplyAuraName != SPELL_AURA_SCHOOL_ABSORB )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	//Increases the damage absorbed by your Savage Defense ability by 32%.    Each point of Mastery increases the absorb by an additional 4%.
	{
//		int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->mod_amount[0] );
		int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->ori_spell->eff[1].EffectBasePoints );
		int32 tot_inc_pct = var_inc / 100;
//		sp->forced_pct_mod_cur += tot_inc_pct;
		value += value * tot_inc_pct / 100;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS SMT_57499( Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( sp->GetProto()->NameHash != SPELL_HASH_INTERVENE || i != 0 )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	//can't be post spell cast proc or else it can not mod the effect
	if( caster )
		caster->RemoveAurasMovementImpairing();
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

/*
SPELL_EFFECT_OVERRIDE_RETURNS SMT_77219(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( caster == NULL || caster->IsPlayer() == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->NameHash != SPELL_HASH_METAMORPHOSIS )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->eff[i].EffectApplyAuraName != SPELL_AURA_MOD_DAMAGE_PERCENT_DONE )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	//Increases the damage done by your demon servants and damage you deal while transformed into a demon by $77219s1%.  Each point of Mastery increases damage by an additional ${$77219m3/100}.1%.
	{
//		int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->mod_amount[0] );
		int32 var_inc = float2int32( SafePlayerCast( caster )->GetFloatValue( PLAYER_MASTERY ) * so->ori_spell->eff[2].EffectBasePoints );
		int32 tot_inc_pct = var_inc / 100;
//		sp->forced_pct_mod_cur += tot_inc_pct;
		value += value * tot_inc_pct / 100;	//should be like this according to wowwiki
//		value += tot_inc_pct;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
*/

SPELL_EFFECT_OVERRIDE_RETURNS SMT_57810(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( caster == NULL || caster->IsPlayer() == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->eff[i].EffectApplyAuraName != SPELL_AURA_PERIODIC_HEAL )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->belongs_to_player_class != caster->getClass() )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	sp->forced_pct_mod_cur += so->ori_spell->eff[0].EffectBasePoints;
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS SMT_56856(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( caster == NULL || caster->IsPlayer() == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->eff[i].Effect != SPELL_EFFECT_ENERGIZE )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->NameHash != SPELL_HASH_STEADY_SHOT && sp->GetProto()->NameHash != SPELL_HASH_COBRA_SHOT )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

	//it crashed
	if( caster->GetMapMgr() == NULL )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	//only mod on dazed targets
	Unit *tTarget = caster->GetMapMgr()->GetUnit( sp->m_targets.m_unitTarget );
	if( tTarget == NULL || tTarget->HasAuraWithMechanic( 0, AURA_SEARCH_NEGATIVE, (1<<MECHANIC_DAZED)|(1<<MECHANIC_ENSNARED) ) == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

	value += so->ori_spell->eff[0].EffectBasePoints;

	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS SMT_33167(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( caster == NULL || caster->IsPlayer() == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->NameHash != SPELL_HASH_DISPEL_MAGIC )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( target == NULL || isAttackable( target, caster ) )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

	value = max( value, so->ori_spell->eff[0].EffectBasePoints );

	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS SMT_82368(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( caster == NULL || caster->IsPlayer() == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->NameHash != SPELL_HASH_VICTORY_RUSH )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->eff[i].Effect != SPELL_EFFECT_HEAL_MAXHEALTH_PCT )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

	//nerf heal from 20 to 5%
	value = MIN( value, so->mod_amount[0] );

	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS SMT_54810(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( caster == NULL || caster->IsPlayer() == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->NameHash != SPELL_HASH_FRENZIED_REGENERATION )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_PERIODIC_TRIGGER_SPELL )
	{
		sp->redirected_effect[i] = SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_ALL_TARGETS;
		return SPELL_EFFECT_OVERRIDE_BREAK_EXECUTION;
	}
	if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MOD_HEALING_PCT )
		value = so->ori_spell->eff[0].EffectBasePoints;

	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS SMT_53295(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( caster == NULL || caster->IsPlayer() == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->NameHash != SPELL_HASH_SERPENT_STING )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->eff[i].EffectApplyAuraName != SPELL_AURA_MOD_RANGED_DAMAGE_TAKEN_PCT 
		&& sp->GetProto()->eff[i].EffectApplyAuraName != 271 //this is dmg taken but needs scripting which is not yet implemented 
		)
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

	value += so->ori_spell->eff[0].EffectBasePoints;

	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS SMT_31220(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( caster == NULL || caster->IsPlayer() == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->NameHash != SPELL_HASH_BACKSTAB && sp->GetProto()->NameHash != SPELL_HASH_HEMORRHAGE )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->eff[i].Effect != SPELL_EFFECT_WEAPON_PERCENT_DAMAGE )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

	value += so->ori_spell->eff[1].EffectBasePoints;

	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS SMT_85446(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( i != 0 )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( caster == NULL || caster->IsPlayer() == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->NameHash != SPELL_HASH_CLEANSE )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( caster != target )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

	for(uint32 x= NEG_AURAS_START; x < MAX_NEGATIVE_AURAS1(caster); ++x)
		if( caster->m_auras[x] && (caster->m_auras[x]->GetSpellProto()->c_is_flags2 & SPELL_FLAG2_IS_MOVEMENT_IMPAIRING_SPELL ) != 0)
		{
			caster->m_auras[x]->Remove();
			break;
		}

	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS SMT_53551(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( caster == NULL || caster->IsPlayer() == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->NameHash != SPELL_HASH_CLEANSE )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

	value = max( value, so->ori_spell->eff[0].EffectBasePoints );

	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS SMT_11095(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( sp->GetProto()->NameHash != SPELL_HASH_LIVING_BOMB
		&& sp->GetProto()->NameHash != SPELL_HASH_LIVING_BOMB_EXPLOSION	)
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->eff[i].EffectApplyAuraName != SPELL_AURA_PERIODIC_DAMAGE
		&& sp->GetProto()->eff[i].Effect != SPELL_EFFECT_SCHOOL_DAMAGE	)
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	sp->forced_pct_mod_cur += so->ori_spell->eff[1].EffectBasePoints;
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS SMT_56846(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( sp->GetProto()->NameHash != SPELL_HASH_IMMOLATION_TRAP )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->eff[i].EffectApplyAuraName != SPELL_AURA_PERIODIC_DAMAGE )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	//reduces duration by 6 seconds but will not change the dmg
	uint32 TickCountOriginal = 15000 / 3000;
	uint32 TickCountGlyphed = ( 15000 - 6000 ) / 3000;
	uint32 ModPCTWithoutSMTHelp = TickCountOriginal * 100 / TickCountGlyphed;
	sp->forced_pct_mod_cur += ( ModPCTWithoutSMTHelp - 100 );
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS SMT_57470(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( sp->GetProto()->eff[i].Effect != SPELL_EFFECT_HEAL )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( target == NULL )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->NameHash != SPELL_HASH_FLASH_HEAL
		&& sp->GetProto()->NameHash != SPELL_HASH_GREATER_HEAL
		&& sp->GetProto()->NameHash != SPELL_HASH_HEAL
		&& sp->GetProto()->NameHash != SPELL_HASH_PENANCE
		)
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( target->HasAuraWithNameHash( SPELL_HASH_WEAKENED_SOUL, 0, AURA_SEARCH_NEGATIVE ) == false
		&& target->HasAuraWithNameHash( SPELL_HASH_GRACE, 0, AURA_SEARCH_POSITIVE ) == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	int64 *data;
	data = caster->GetCreateIn64Extension( EXTENSION_ID_ONETIME_TARGETSPECIFIC_CRIT_MOD_TARGET );
	*data = target->GetGUID();
	data = caster->GetCreateIn64Extension( EXTENSION_ID_ONETIME_TARGETSPECIFIC_CRIT_MOD_SPELL );
	*data = sp->GetProto()->Id;
	data = caster->GetCreateIn64Extension( EXTENSION_ID_ONETIME_TARGETSPECIFIC_CRIT_MOD_VAL );
	*data = so->mod_amount[0];
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS SMT_79463(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( sp->GetProto()->NameHash != SPELL_HASH_SHADOW_BOLT )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->eff[i].Effect != SPELL_EFFECT_SCHOOL_DAMAGE )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	sp->forced_pct_mod_cur += so->mod_amount[0];
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS SMT_17810(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( sp->GetProto()->NameHash != SPELL_HASH_CORRUPTION )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->eff[i].EffectApplyAuraName != SPELL_AURA_PERIODIC_DAMAGE )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	sp->forced_pct_mod_cur += so->mod_amount[0];
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS SMT_63280(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( sp->GetProto()->NameHash != SPELL_HASH_SHAMANISTIC_RAGE )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->eff[i].Effect != SPELL_EFFECT_DISPEL )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	value += so->mod_amount[0];
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS SMT_63095(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( sp->GetProto()->NameHash != SPELL_HASH_ICE_BARRIER )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->eff[i].EffectApplyAuraName != SPELL_AURA_SCHOOL_ABSORB )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	sp->forced_pct_mod_cur += so->mod_amount[0];
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS SMT_56383(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( sp->GetProto()->NameHash != SPELL_HASH_MAGE_ARMOR )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->eff[i].EffectApplyAuraName != SPELL_AURA_MOD_TOTAL_MANA_REGEN_PCT )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	//maybe we should support scaled values
//	value += ( 50 + value * so->mod_amount[0] ) / 100; // 50 is due to roundup issue : 3 * 20% < 1
	value += value * so->mod_amount[0] / 100; // working if value is scaled
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS SMT_73683(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( caster == NULL || caster->IsPlayer() == false )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->eff[i].Effect != SPELL_EFFECT_SCHOOL_DAMAGE 
		&& sp->GetProto()->eff[i].EffectApplyAuraName != SPELL_AURA_PERIODIC_DAMAGE )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( ( sp->GetProto()->SchoolMask & SCHOOL_MASK_FIRE ) == 0 )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	sp->forced_pct_mod_cur = sp->forced_pct_mod_cur * ( 100 + so->mod_amount[0] ) / 100;
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
} 

SPELL_EFFECT_OVERRIDE_RETURNS SMT_54924(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( sp->GetProto()->NameHash != SPELL_HASH_DIVINE_PROTECTION )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	//remove physical protection
	if( sp->GetProto()->eff[i].EffectMiscValue == 1 )
	{
		sp->redirected_effect[i] = SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_ALL_TARGETS;
		return SPELL_EFFECT_OVERRIDE_BREAK_EXECUTION;
	}
	//increase magical protection by X
	value += so->ori_spell->eff[1].EffectBasePoints;
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
} 

SPELL_EFFECT_OVERRIDE_RETURNS SMT_84673(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( i != 0 )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->GetProto()->NameHash != SPELL_HASH_BLAST_WAVE )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( sp->m_targetUnits[i].size() < 2 )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
	if( RandomUInt() % 100 > so->ori_spell->procChance )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

	SpellCastTargets targets2( sp->m_targets ); 
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init( sp->m_caster, dbcSpell.LookupEntryForced( 2120 ) ,true, NULL); //Flamestrike
	spell->ProcedOnSpell = sp->GetProto();
	spell->pSpellId = so->ori_spell->Id;
	spell->prepare(&targets2);

	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
} 

SPELL_EFFECT_OVERRIDE_RETURNS SMT_36563(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	if( sp->GetProto()->NameHash != SPELL_HASH_AMBUSH
		&& sp->GetProto()->NameHash != SPELL_HASH_GARROTE )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

	if( sp->GetProto()->eff[i].EffectApplyAuraName != SPELL_AURA_PERIODIC_DAMAGE
		&& sp->GetProto()->eff[i].Effect != SPELL_EFFECT_WEAPON_PERCENT_DAMAGE	)
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

	sp->forced_pct_mod_cur = sp->forced_pct_mod_cur * ( 100 + so->mod_amount[0] ) / 100;

	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS SMT_1978(Spell *sp, Unit *caster, Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{

	// Noxious Stings is not learned yet. 99% of the case
	if( so->mod_amount[0] <= 1	)
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

	if( sp->GetProto()->dmg_type != SPELL_DMG_TYPE_RANGED )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

	//only mod caster dmg
	if( target == NULL || target->GetGUID() != so->CasterGuid )
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

	if( sp->GetProto()->eff[i].Effect != SPELL_EFFECT_SCHOOL_DAMAGE 
		&& sp->GetProto()->eff[i].EffectApplyAuraName != SPELL_AURA_PERIODIC_DAMAGE 
		&& sp->GetProto()->eff[i].Effect != SPELL_EFFECT_WEAPON_PERCENT_DAMAGE
		&& sp->GetProto()->eff[i].Effect != SPELL_EFFECT_WEAPON_DAMAGE
		)
		return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

	//only mod owner dmg because this can stack
	sp->forced_pct_mod_cur = sp->forced_pct_mod_cur * ( 100 + so->mod_amount[0] ) / 100;

	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

//these are imagined to be scripted SMT mods that have FIXED ! values. If you want proper SMT values with chained calc effect then you will probably script the aura that registers these
// you can also use these for pre cast procs. Proc system rught now is post cast 
void RegisterDefaultClassOverrides()
{
	SpellEntry *sp;
	//Serpent Sting - Noxious Stings
	REGISTER_HANDLER( 1978 );	
	//Shadowstep
	REGISTER_HANDLER( 36563 );	
	//Improved Flamestrike
	REGISTER_HANDLER( 84673 );	
	REGISTER_SAMEHANDLER( 84674, 84673 );
	//Glyph of Divine Protection
	REGISTER_HANDLER( 54924 );	
	//Unleash Flame
	REGISTER_HANDLER( 73683 );	
	//Glyph of Mage Armor
	REGISTER_HANDLER( 56383 );	
	//Enhanced Elements
	REGISTER_HANDLER( 77223 );
	//Glyph of Ice Barrier
	REGISTER_HANDLER( 63095 );
	//Glyph of Shamanistic Rage
	REGISTER_HANDLER( 63280 );
	//Improved Corruption
	REGISTER_HANDLER( 17810 );
	REGISTER_SAMEHANDLER( 17811, 17810 );
	REGISTER_SAMEHANDLER( 17812, 17810 );
	//Demon Soul: Succubus
	REGISTER_HANDLER( 79463 );
	//Renewed Hope
	REGISTER_HANDLER( 57470 );
	REGISTER_SAMEHANDLER( 57472, 57470 );
	//Glyph of Immolation Trap
	REGISTER_HANDLER( 56846 );
	//Critical Mass
	REGISTER_HANDLER( 11095 );
	REGISTER_SAMEHANDLER( 12872, 11095 );
	REGISTER_SAMEHANDLER( 12873, 11095 );
	//Sacred Cleansing
	REGISTER_HANDLER( 53551 );
	//Acts of Sacrifice
	REGISTER_HANDLER( 85446 );
	REGISTER_SAMEHANDLER( 85795, 85446 );
	//Sinister Calling
	REGISTER_HANDLER( 31220 );
	//Noxious Stings
	REGISTER_HANDLER( 53295 );
	REGISTER_SAMEHANDLER( 53296, 53295 );
	//Glyph of Frenzied Regeneration
	REGISTER_HANDLER( 54810 );	
	//Impending Victory - Victorious - nerf Victory Rush
	REGISTER_HANDLER( 82368 );	
	//Absolution
	REGISTER_HANDLER( 33167 );
	//Glyph of Dazzled Prey
	REGISTER_HANDLER( 56856 );
	//Genesis
	REGISTER_HANDLER( 57810 );
	REGISTER_SAMEHANDLER( 57811, 57810 );
	REGISTER_SAMEHANDLER( 57812, 57810 );
	//Master Demonologist
//	REGISTER_HANDLER( 77219 );
	//Warbringer
	REGISTER_HANDLER( 57499 );
	//Savage Defender - BEAR FORM ONLY
	REGISTER_HANDLER( 77494 );
	//Razor Claws - CAT FORM ONLY
	REGISTER_HANDLER( 77493 );
	//Flashburn
	REGISTER_HANDLER( 76595 );
	//Frostburn
	REGISTER_HANDLER( 76613 );
	//Mana Adept
	REGISTER_HANDLER( 76547 );
	//Harmony
	REGISTER_HANDLER( 77495 );
	//Glyph of Frostfire
	REGISTER_HANDLER( 61205 );
	//Glyph of Smite
	REGISTER_HANDLER( 55692 );
	//Glyph of Strangulate
	REGISTER_HANDLER( 58618 );
	//Last Word
	REGISTER_HANDLER( 20234 );
	REGISTER_SAMEHANDLER( 20235, 20234 );
	//Frozen Power
	REGISTER_HANDLER( 63373 );
	REGISTER_SAMEHANDLER( 63374, 63373 );
	//Improved Lava Lash
	REGISTER_HANDLER( 77700 );
	REGISTER_SAMEHANDLER( 77701, 77700 );
	//Fire and Brimstone
	REGISTER_HANDLER( 47266 );
	REGISTER_SAMEHANDLER( 47267, 47266 );
	REGISTER_SAMEHANDLER( 47268, 47266 );
	//Contagion
	REGISTER_HANDLER( 91316 );
	REGISTER_SAMEHANDLER( 91319, 91316 );
	//Dark Simulacrum
	REGISTER_HANDLER( 77616 );
	//Shield Discipline
	REGISTER_HANDLER( 77484 );
	//Improved Power Word: Shield
	REGISTER_HANDLER( 14748 );
	REGISTER_SAMEHANDLER( 14768, 14748 );
	//Dreamstate
	REGISTER_HANDLER( 33597 );
	REGISTER_SAMEHANDLER( 33599, 33597 );
	//Elemental Weapons
	REGISTER_HANDLER( 16266 );
	REGISTER_SAMEHANDLER( 29079, 16266 );
}