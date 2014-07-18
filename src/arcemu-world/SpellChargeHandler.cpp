#include "StdAfx.h"

#define REGISTER_HANDLER( spell ) sp = dbcSpell.LookupEntryForced( spell ); \
	ASSERT( sp->ChargeHandler == 0 ); \
	sp->ChargeHandler = &CH_##spell;
#define REGISTER_SAMEHANDLER(spell,copy_from) sp = dbcSpell.LookupEntryForced( spell ); \
	ASSERT( sp->ChargeHandler == 0 ); \
	sp->ChargeHandler = &CH_##copy_from;

void CH_57529( ProcHandlerContextShare *context )
{
	//need damaging spell
	if( !context->in_CastingSpell )
		return;
	if( !( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_DAMAGING ) )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_47383( ProcHandlerContextShare *context )
{
//	if( (iter2->second.ProcFlag & PROC_ON_CAST_SPELL) == 0 )
//		continue;
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_INCINERATE && context->in_CastingSpell->NameHash != SPELL_HASH_SOUL_FIRE )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}
/*
void CH_53220( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_AIMED_SHOT && context->in_CastingSpell->NameHash != SPELL_HASH_ARCANE_SHOT && context->in_CastingSpell->NameHash != SPELL_HASH_CHIMERA_SHOT )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}*/

void CH_69369( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	SpellCastTime *sd = dbcSpellCastTime.LookupEntry( context->in_CastingSpell->CastingTimeIndex );
	if(sd && sd->CastTime==0)
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_60503( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_OVERPOWER )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_64343( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_FIRE_BLAST )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_70757( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_HOLY_LIGHT )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}
/*
void CH_46833( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_STARFIRE )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
} */

void CH_53817( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_LIGHTNING_BOLT 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_CHAIN_LIGHTNING 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_HEX 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_HEALING_RAIN
		&& !( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_HEALING )
		)
		return;
	//nature spell from herbalism
	if( context->in_CastingSpell->NameHash == SPELL_HASH_LIFEBLOOD
		|| context->in_CastingSpell->NameHash == SPELL_HASH_SPINAL_HEALING_INJECTOR
		)
		return;
	//try to remove all 5 stacks ?
	context->in_Caller->RemoveAura( 53817, 0, AURA_SEARCH_POSITIVE, 5 );
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_61595( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_SHADOW_BOLT && context->in_CastingSpell->NameHash != SPELL_HASH_INCINERATE )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_57761( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_FIREBALL 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_FROSTFIRE_BOLT 
		)
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_46916( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_SLAM )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_56453( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_EXPLOSIVE_SHOT && context->in_CastingSpell->NameHash != SPELL_HASH_ARCANE_SHOT )
		return;

	//there is no SMT on this spell. We either replace cast it or simply manually clear cooldown
	if( context->in_CastingSpell->NameHash == SPELL_HASH_EXPLOSIVE_SHOT && context->in_Caller->IsPlayer() )
		SafePlayerCast( context->in_Caller )->ClearCooldownForSpell( context->in_CastingSpell->Id );

	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_16870( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	//if this is a free spell already then no point loosing the charge
	SpellPowerEntry *spe = dbcSpellPower.LookupEntryForced( context->in_CastingSpell->SpellPowerId );
	if( spe == NULL )
	{
		return;
	}
	if( spe->ManaCostPercentage <= 0 && spe->manaCost <= 0 )
	{
		return;
	}
	//cast-time damaging or healing spell or offensive feral ability
	bool IsFeralOffensive = false;
	if( context->in_CastingSpell->spell_skilline[0] == SKILL_FERAL_COMBAT && isAttackable( context->in_Caller, context->in_Victim ) )
		IsFeralOffensive = true;
	//only spells with cast time eat up the charge
	if( ( context->in_CastingSpell->CastingTimeIndex == 1 || context->in_CastingSpell->CastingTimeIndex == 0 ) //191,200,205
		&& IsFeralOffensive == false )
	{
		return;
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_65156( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_SLAM && context->in_CastingSpell->NameHash != SPELL_HASH_MORTAL_STRIKE )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_59578( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_EXORCISM )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_12536( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( ( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_DAMAGING ) == 0 )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_53672( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_FLASH_OF_LIGHT 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_HOLY_LIGHT 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_DIVINE_LIGHT 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_HOLY_RADIANCE 
		)
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_48108( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
//	if( context->in_CastingSpell->NameHash != SPELL_HASH_PYROBLAST )
	if( context->in_CastingSpell->NameHash != SPELL_HASH_PYROBLAST_ ) // the free version
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_54741( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_FLAMESTRIKE )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_54277( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_SHADOW_BOLT
		&& context->in_CastingSpell->NameHash != SPELL_HASH_INCINERATE
		&& context->in_CastingSpell->NameHash != SPELL_HASH_CHAOS_BOLT
		)
		return;
//	if( context->in_CastingSpell->spell_skilline[0] != SKILL_DESTRUCTION )
//		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_51124( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
//	if( context->in_CastingSpell->NameHash != SPELL_HASH_ICY_TOUCH && context->in_CastingSpell->NameHash != SPELL_HASH_HOWLING_BLAST && context->in_CastingSpell->NameHash != SPELL_HASH_FROST_STRIKE )
	if( context->in_CastingSpell->NameHash != SPELL_HASH_OBLITERATE && context->in_CastingSpell->NameHash != SPELL_HASH_FROST_STRIKE )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_59052( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_HOWLING_BLAST 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_ICY_TOUCH
		)
		return;
	Aura *a = context->in_Caller->HasAuraWithNameHash( SPELL_HASH_ITEM___DEATH_KNIGHT_T13_DPS_2P_BONUS, 0, AURA_SEARCH_PASSIVE );
//	context->in_event->created_with_value++;
	if( a && RandChance( a->GetSpellProto()->eff[1].EffectBasePoints ) 
//		&& context->in_event->created_with_value <= 1 
		)
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_43339( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_EARTH_SHOCK &&
			context->in_CastingSpell->NameHash != SPELL_HASH_FLAME_SHOCK &&
			context->in_CastingSpell->NameHash != SPELL_HASH_FROST_SHOCK )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_12043( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	//Presence of Mind and Nature's Swiftness should only get removed
	//when a non-instant and bellow 10 sec. Also must be nature :>
//	if(!sd->CastTime||sd->CastTime>10000) continue;
	SpellCastTime *sd = dbcSpellCastTime.LookupEntryForced(context->in_CastingSpell->CastingTimeIndex);
//	if(!sd || sd->CastTime==0 || sd->CastTime>10000)	//this version led to an exploit some say
	if( sd == NULL || sd->CastTime == 0 ) //insta cast spells
		return;
	//this might not be valid for all wow versions !
	if( Need_SM_Apply( context->in_CastingSpell ) && context->in_Caller->SM_Mods )
	{
		SpellEntry *sp = dbcSpell.LookupEntryForced( 12043 ); //this is us
		//this might not be valid for all wow versions !
		if( sp->eff[0].EffectApplyAuraName == SPELL_AURA_ADD_PCT_MODIFIER )
		{
			//check if the casted spell was affected at all
			int32 m_castTime = sd->CastTime;
			SM_FIValue( context->in_Caller->SM_Mods->SM_FCastTime, (int32*)&m_castTime, context->in_CastingSpell->GetSpellGroupType() );
			SM_PIValue( context->in_Caller->SM_Mods->SM_PCastTime, (int32*)&m_castTime, context->in_CastingSpell->GetSpellGroupType() );
			//we did not mod this spell, no point to loose PM
			if( m_castTime == sd->CastTime )
				return;
		}
	}
	else
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_17116( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
//	if( CastingSpell->School!=SCHOOL_NATURE||(!sd->CastTime||sd->CastTime>10000)) continue;
	SpellCastTime *sd = dbcSpellCastTime.LookupEntry(context->in_CastingSpell->CastingTimeIndex);
//	if( (!sd || sd->CastTime==0 || sd->CastTime>10000 || CastingSpell->School!=SCHOOL_NATURE )
	if( ((sd && sd->CastTime==0) || context->in_CastingSpell->School!=SCHOOL_NATURE )
		&& (context->in_Caller->getClass() == SHAMAN || context->in_Caller->getClass() == DRUID )	//spellsteal exploit to have infinit casts
		) 
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_14177( ProcHandlerContextShare *context )
{
//	if(context->in_Victim==context->in_Caller || isFriendly(context->in_Caller, context->in_Victim))
	if( context->in_CastingSpell == NULL )
		return;
	//mutilate just casts another 2 spells, itself is not damaging spell
	if( ( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_DAMAGING ) == 0 && context->in_CastingSpell->NameHash != SPELL_HASH_MUTILATE )
		return;
	if( context->in_CastingSpell->quick_tickcount > 1 )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_20178( ProcHandlerContextShare *context )
{
	//do not remove ourself when we melee hit due to our own proc
	if( context->in_CastingSpell && context->in_CastingSpell->NameHash == SPELL_HASH_RECKONING )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_85509( ProcHandlerContextShare *context )
{
	//do not remove ourself when we melee hit due to our own proc
	if( context->in_CastingSpell && context->in_CastingSpell->NameHash != SPELL_HASH_EXORCISM )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_16188( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	SpellCastTime *sd = dbcSpellCastTime.LookupEntry( context->in_CastingSpell->CastingTimeIndex );
	if(sd && sd->CastTime==0)
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_74434( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_SOUL_FIRE &&
			context->in_CastingSpell->NameHash != SPELL_HASH_SEARING_PAIN &&
//			context->in_CastingSpell->NameHash != SPELL_HASH_CREATE_HEALTHSTONE &&
			context->in_CastingSpell->NameHash != SPELL_HASH_HEALTHSTONE &&
			context->in_CastingSpell->NameHash != SPELL_HASH_DEMONIC_CIRCLE__TELEPORT &&
			context->in_CastingSpell->NameHash != SPELL_HASH_DRAIN_LIFE )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_89485( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_FLASH_HEAL &&
			context->in_CastingSpell->NameHash != SPELL_HASH_BINDING_HEAL &&
			context->in_CastingSpell->NameHash != SPELL_HASH_GREATER_HEAL &&
			context->in_CastingSpell->NameHash != SPELL_HASH_PRAYER_OF_HEALING )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_50227( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_SHIELD_SLAM )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_81340( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_DEATH_COIL )
		return;
	Aura *a = context->in_Caller->HasAuraWithNameHash( SPELL_HASH_ITEM___DEATH_KNIGHT_T13_DPS_2P_BONUS, 0, AURA_SEARCH_PASSIVE );
//	context->in_event->created_with_value++;
	if( a && RandChance( a->GetSpellProto()->eff[0].EffectBasePoints ) 
//		&& context->in_event->created_with_value <= 1 
		)
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_86627( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_HEROIC_STRIKE )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}
/*
void CH_84584( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_EXECUTE
		&& context->in_CastingSpell->NameHash != SPELL_HASH_OVERPOWER
		&& context->in_CastingSpell->NameHash != SPELL_HASH_MORTAL_STRIKE
		)
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
} */

void CH_88819( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_HOLY_SHOCK	)
		return;
	if( context->in_Caller->IsPlayer() )
		SafePlayerCast( context->in_Caller )->ClearCooldownForSpell( 20473 );	//holy shock
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_88688( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_FLASH_HEAL	)
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_63731( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_GREATER_HEAL	
		&& context->in_CastingSpell->NameHash != SPELL_HASH_PRAYER_OF_HEALING	
		)
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}
/*
void CH_6346( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->eff[0].EffectApplyAuraName != SPELL_AURA_MOD_FEAR	
		&& context->in_CastingSpell->eff[1].EffectApplyAuraName != SPELL_AURA_MOD_FEAR
		&& context->in_CastingSpell->eff[2].EffectApplyAuraName != SPELL_AURA_MOD_FEAR
		)
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
} */

void CH_14751( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_HEAL	
		&& context->in_CastingSpell->NameHash != SPELL_HASH_FLASH_HEAL	
		&& context->in_CastingSpell->NameHash != SPELL_HASH_GREATER_HEAL	
		&& context->in_CastingSpell->NameHash != SPELL_HASH_BINDING_HEAL	
		&& context->in_CastingSpell->NameHash != SPELL_HASH_PRAYER_OF_HEALING	
		&& context->in_CastingSpell->NameHash != SPELL_HASH_PRAYER_OF_MENDING	
		&& context->in_CastingSpell->NameHash != SPELL_HASH_MIND_SPIKE	
		&& context->in_CastingSpell->NameHash != SPELL_HASH_SMITE	
		)
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_87096( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_SHOCKWAVE )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_82368( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_VICTORY_RUSH )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_16166( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_LIGHTNING_BOLT
		&& context->in_CastingSpell->NameHash != SPELL_HASH_CHAIN_LIGHTNING	
		&& context->in_CastingSpell->NameHash != SPELL_HASH_LAVA_BURST	
		)
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_81292( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_MIND_BLAST	)
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_36032( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash == SPELL_HASH_ARCANE_BLAST )
		return;
	if( context->in_CastingSpell->School != SCHOOL_ARCANE )
		return;
	if( ( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_DAMAGING ) == 0 )
		return;
	//we should remove all charges
	context->in_Caller->RemoveAura( 36032, 0, AURA_SEARCH_ALL, MAX_PASSIVE_AURAS );	//i wonder if this will create any mem corruption
//	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_84590( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == true )
	{
		SafePlayerCast(context->in_Caller)->ClearCooldownForSpell( 5171 );	//slice and dice
		SafePlayerCast(context->in_Caller)->ClearCooldownForSpell( 73651 );	//recuperate
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_77800( ProcHandlerContextShare *context )
{
	if( !( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_HEALING ) )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_53390( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell->NameHash != SPELL_HASH_HEALING_WAVE
		&& context->in_CastingSpell->NameHash != SPELL_HASH_GREATER_HEALING_WAVE	
		&& context->in_CastingSpell->NameHash != SPELL_HASH_HEALING_SURGE	
		)
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_324( ProcHandlerContextShare *context )
{
	//do not proc on DOTS
	if( context->in_CastingSpell && context->in_CastingSpell->quick_tickcount > 1 )
		return;
	if( context->in_Caller->IsPlayer() )
	{
		Player *p = SafePlayerCast( context->in_Caller );
		Aura *ImprLighgtningShield = p->HasAuraWithNameHash( SPELL_HASH_IMPROVED_LIGHTNING_SHIELD, 0, AURA_SEARCH_PASSIVE );
		if( ImprLighgtningShield )
		{
			if( ( context->in_events_filter_flags & ( PROC_ON_ANY_DAMAGE_VICTIM ) ) == 0 )
				return;
			//acording to wowhead this has a 3 second cooldown
			int64 *LastProc = ImprLighgtningShield->GetCreateIn64Extension( EXTENSION_ID_IMPROVED_LIGHTNING_SHIELD_PROC_STAMP );
			if( *LastProc > getMSTime() )
				return;
			*LastProc = getMSTime() + 3000;
			//add a charge up to 9 instead consuming it
			Aura *a = p->HasAuraWithNameHash( SPELL_HASH_LIGHTNING_SHIELD, 0, AURA_SEARCH_POSITIVE );
			if( a ) // would be strange to get 0 here considering we pocced on the aura
			{
				int32 charges_now = p->CountAuraNameHash( SPELL_HASH_LIGHTNING_SHIELD, AURA_SEARCH_POSITIVE );
				if( charges_now < 9 )
				{
					Aura *aur = AuraPool.PooledNew( __FILE__, __LINE__ );
					aur->Init( a->GetSpellProto(),a->GetDuration(),a->GetCaster(),a->GetTarget(), NULL );
					//curses the hacks !. this spell can overstack. but not the original spell, that should stack top 3
					//but but this is the original spell :(
					p->AuraStackCheat = true;
					p->AddAura( aur );
					p->m_chargeSpells[ a->GetSpellId() ].count++;	//also add a charge
					p->AuraStackCheat = false;
				}
				if( charges_now >= 3 )
				{
					//fullmination overlay
					if( p->HasAuraWithNameHash( SPELL_HASH_FULMINATION_, 0, AURA_SEARCH_POSITIVE ) == NULL )
						p->CastSpell( p, 95774, true );
				}
			}
			return;
		}
		if( SafePlayerCast( context->in_Caller )->HasGlyphWithID( GLYPH_SHAMAN_LIGHTNING_SHIELD ) )
		{
			//charge count should not drop below 3
			Aura *a = context->in_Caller->HasAuraWithNameHash( SPELL_HASH_LIGHTNING_SHIELD, 0, AURA_SEARCH_POSITIVE );
			if( a && a->GetStackCount() <= 3 )
				return;
		}
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_81021( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell->NameHash != SPELL_HASH_RAVAGE_ )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_94686( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL || context->in_CastingSpell->NameHash != SPELL_HASH_HOLY_LIGHT )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_90174( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	//only remove on spells that eat up holy power
	if( context->in_CastingSpell->powerType != POWER_TYPE_HOLY )
		return;
	//we should know how much we had before cast, we just set this to max :(
	//remember how much holy power we had to restore it later
	Aura *a = context->in_Caller->HasAura( 90174, 0, AURA_SEARCH_POSITIVE );
	if( a )
	{
		int32 PowerAmtToRestore = a->m_modList[0].m_amount;
//		context->in_Caller->SetPower( POWER_TYPE_HOLY, PowerAmtToRestore );
//		context->in_Caller->UpdatePowerAmm( true, POWER_TYPE_HOLY );
//		context->in_Caller->Energize( context->in_Caller, 88675, PowerAmtToRestore, POWER_TYPE_HOLY );
		sEventMgr.AddEvent( context->in_Caller, &Unit::Energize, context->in_Caller, (uint32)88675, PowerAmtToRestore, (uint32)POWER_TYPE_HOLY, (uint8)0, EVENT_DELAYED_SPELLCAST, 500, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
	}
//	context->in_Caller->RemoveAura( 90174 );	//visual
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}
/*
void CH_90174( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	//only remove on spells that eat up holy power
	if( context->in_CastingSpell->powerType != POWER_TYPE_HOLY )
		return;
	//we should know how much we had before cast, we just set this to max :(
//	context->in_Caller->SetPower(  POWER_TYPE_HOLY, context->in_Caller->GetMaxPower(  POWER_TYPE_HOLY ) );
	context->in_Caller->Energize( context->in_Caller, 88675, context->in_Caller->GetMaxPower( POWER_TYPE_HOLY ), POWER_TYPE_HOLY );
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
} */

void CH_81093( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell->NameHash != SPELL_HASH_STARFIRE )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_16974( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	SpellCastTime *sd = dbcSpellCastTime.LookupEntry( context->in_CastingSpell->CastingTimeIndex );
	if(sd && sd->CastTime==0)
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_89937( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell->NameHash != SPELL_HASH_FEL_FLAME )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_90472( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell->NameHash != SPELL_HASH_EVISCERATE
		&& context->in_CastingSpell->NameHash != SPELL_HASH_ENVENOM
		)
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_73683( ProcHandlerContextShare *context )
{
	if( ( context->in_CastingSpell->SchoolMask & SCHOOL_MASK_FIRE ) == 0 )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_73685( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL 
		|| !( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_HEALING )
		|| ( ( context->in_CastingSpell->c_is_flags3 & SPELL_FLAG3_IS_DIRECT_TARGET ) == 0 )
		)
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_79683( ProcHandlerContextShare *context )
{
	//remove the cast enabler aura after first arcane missles cast
	if( context->in_CastingSpell == NULL 
		|| context->in_CastingSpell->NameHash != SPELL_HASH_ARCANE_MISSILES )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_77769( ProcHandlerContextShare *context )
{
	//remove the cast enabler aura after first arcane missles cast
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_ICE_TRAP 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_IMMOLATION_TRAP 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_SNAKE_TRAP 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_FREEZING_TRAP 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_EXPLOSIVE_TRAP 
		)
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_82926( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_AIMED_SHOT_ )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_101568( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_DEATH_STRIKE )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_99060( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_KILL_COMMAND 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_ARCANE_SHOT 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_AIMED_SHOT 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_STEADY_SHOT 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_SCATTER_SHOT 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_MULTI_SHOT 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_KILL_SHOT 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_COBRA_SHOT 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_COBRA_SHOT 
		)
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_98057( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_AVENGER_S_SHIELD )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_14183( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_Caller->IsPlayer() == false )
		return;
	if( SafePlayerCast( context->in_Caller )->m_currentSpellAny == NULL )
		return;
	if( SafePlayerCast( context->in_Caller )->m_currentSpellAny->m_requiresCP == 0 )
		return;
	//do not remove any combo points when removing this aura. Consider the points consumed by the curently casted spell
	Aura *a = context->in_Caller->HasAuraWithNameHash( SPELL_HASH_PREMEDITATION, 0, AURA_SEARCH_POSITIVE );
	a->m_modList[0].m_amount =  0;
	a->m_modList[0].fixed_amount[ 0 ] = 0;
//	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_99207( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_LAVA_BURST )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_47283( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_SOUL_FIRE )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_77616( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	//how did this get on a non player target ?
	if( context->in_Caller->IsPlayer() == false )
	{
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
		return;
	}
	//only self remove if this is a temp spell.
//	if( SafePlayerCast( context->in_Caller )->mTempSpells.find( context->in_CastingSpell->Id ) == SafePlayerCast( context->in_Caller )->mTempSpells.end() )
//		return;
	// Not any temp spell. Temp spell for Dark Simulacrum
	if( SafePlayerCast( context->in_Caller )->mSpellReplaces[ 77606 ] != context->in_CastingSpell->Id )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_96171( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_RUNE_TAP )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_81141( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_BLOOD_BOIL )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_44544( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_ICE_LANCE
		&& context->in_CastingSpell->NameHash != SPELL_HASH_DEEP_FREEZE )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_82897( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_KILL_COMMAND )
		return;
	context->in_Caller->CastSpell( context->in_Caller, 86316, true ); //Resistance is Futile - refund 40 focus
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_53257( ProcHandlerContextShare *context )
{
	//only on abilities : Bite, Claw, Smack
	if( context->in_CastingSpell == NULL )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_83359( ProcHandlerContextShare *context )
{
	//only on abilities : Bite, Claw, Smack...
	if( context->in_CastingSpell == NULL )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_36563( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_AMBUSH
		&& context->in_CastingSpell->NameHash != SPELL_HASH_GARROTE )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_88448( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_SUMMON_IMP
		&& context->in_CastingSpell->NameHash != SPELL_HASH_SUMMON_VOIDWALKER
		&& context->in_CastingSpell->NameHash != SPELL_HASH_SUMMON_SUCCUBUS
		&& context->in_CastingSpell->NameHash != SPELL_HASH_SUMMON_FELHOUND
		&& context->in_CastingSpell->NameHash != SPELL_HASH_SUMMON_FELHUNTER
		&& context->in_CastingSpell->NameHash != SPELL_HASH_SUMMON_FELGUARD
		)
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_20240( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsCrowdControlledNoAttack() )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_53386( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( ( context->in_CastingSpell->SchoolMask & ( SCHOOL_MASK_FROST | SCHOOL_MASK_SHADOW ) ) == 0 )
		return;
	//consume it on spell cast instead DOT tick
	if( context->in_events_filter_flags & PROC_ON_DOT )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_59887( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	//cool bug made by blizz ?
	if( context->in_CastingSpell->ChannelInterruptFlags != 0 )
		return;
	if( context->in_CastingSpell->NameHash == SPELL_HASH_RENEW )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void CH_17941( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_SHADOW_BOLT )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}
/*
void CH_52437( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_EXECUTE )
		return;
	if( context->in_Caller->IsPlayer() == false ||
		SafePlayerCast( context->in_Caller )->m_currentSpellAny == NULL )
		return;
	Spell *castingspell = SafePlayerCast( context->in_Caller )->m_currentSpellAny;
	int32 MaxRageKeep = context->in_OwnerSpell->eff[0].EffectBasePoints * 10;
	int32 HaveRageNow = context->in_Caller->GetPower( POWER_TYPE_RAGE );
	int32 CanHaveMaxRage = context->in_Caller->GetMaxPower( POWER_TYPE_RAGE );
	int32 NewRage = MIN( HaveRageNow + MaxRageKeep, CanHaveMaxRage );
	context->in_Caller->SetPower( POWER_TYPE_RAGE, NewRage );
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}*/

void RegisterChargeHandlers()
{
	SpellEntry *sp;
	//Sudden Death
//	REGISTER_HANDLER( 52437 );
	//Nightfall / Glyph of Corruption
	REGISTER_HANDLER( 17941 );
	//Borrowed Time
	REGISTER_HANDLER( 59887 );
	REGISTER_SAMEHANDLER( 59888, 59887 );
	REGISTER_SAMEHANDLER( 59889, 59887 );
	//Rune of Cinderglacier
	REGISTER_HANDLER( 53386 );
	//Retaliation
	REGISTER_HANDLER( 20240 );
	//Demonic Rebirth
	REGISTER_HANDLER( 88448 );
	//Shadowstep
	REGISTER_HANDLER( 36563 );
	//Sic 'Em!
	REGISTER_HANDLER( 83359 );
	REGISTER_SAMEHANDLER( 89388, 83359 );
	//Cobra Strikes
	REGISTER_HANDLER( 53257 );
	//Resistance is Futile
	REGISTER_HANDLER( 82897 );
	//Fingers of Frost
	REGISTER_HANDLER( 44544 );
	//Crimson Scourge
	REGISTER_HANDLER( 81141 );
	//Will of the Necropolis
	REGISTER_HANDLER( 96171 );
	//Dark Simulacrum
	REGISTER_HANDLER( 77616 );
	//Empowered Imp
	REGISTER_HANDLER( 47283 );
	//Volcano - Item - Shaman T12 Elemental 4P Bonus
	REGISTER_HANDLER( 99207 );
	//Premeditation
	REGISTER_HANDLER( 14183 );
	//Grand Crusader
	REGISTER_HANDLER( 98057 );
	REGISTER_SAMEHANDLER( 85416, 98057 );
	//Item - Hunter T12 4P Bonus
	REGISTER_HANDLER( 99060 );
	//Glyph of Dark Succor
	REGISTER_HANDLER( 101568 );
	//Master Marksman - FIRE!
	REGISTER_HANDLER( 82926 );
	//Trap Launcher
	REGISTER_HANDLER( 77769 );
	REGISTER_SAMEHANDLER( 82946, 77769 );
	//Arcane Missiles
	REGISTER_HANDLER( 79683 );
	//Unleash Life
	REGISTER_HANDLER( 73685 );
	//Unleash Flame
	REGISTER_HANDLER( 73683 );
	//Item - Rogue T11 4P Bonus
	REGISTER_HANDLER( 90472 );
	//Item - Warlock T11 4P Bonus
	REGISTER_HANDLER( 89937 );
	//Predatory Strikes
	REGISTER_HANDLER( 16974 );
	//Fury of Stormrage
	REGISTER_HANDLER( 81093 );
	//Divine Purpose
	REGISTER_HANDLER( 90174 );
	//Crusader
	REGISTER_HANDLER( 94686 );
	//Stampede
	REGISTER_HANDLER( 81021 );
	REGISTER_SAMEHANDLER( 81022, 81021 );
	//Lightning Shield
	REGISTER_HANDLER( 324 );
	//Tidal Waves
	REGISTER_HANDLER( 53390 );
	//Focused Insight
	REGISTER_HANDLER( 77800 );
	//Deadly Momentum
	REGISTER_HANDLER( 84590 );
	//Arcane Blast
	REGISTER_HANDLER( 36032 );
	//Mind Melt
	REGISTER_HANDLER( 81292 );
	REGISTER_SAMEHANDLER( 87160, 81292 );
	//Elemental Mastery
	REGISTER_HANDLER( 16166 );
	//Victorious - Impending Victory
	REGISTER_HANDLER( 82368 );
	//Thunderstruck
	REGISTER_HANDLER( 87096 );
	//Chakra
	REGISTER_HANDLER( 14751 );
	//Fear Ward
//	REGISTER_HANDLER( 6346 );
	//Serendipity
	REGISTER_HANDLER( 63731 );
	REGISTER_SAMEHANDLER( 63735, 63731 );
	//Surge of Light
	REGISTER_HANDLER( 88688 );
	//Daybreak
	REGISTER_HANDLER( 88819 );
	//Lambs to the Slaughter
//	REGISTER_HANDLER( 84584 );
//	REGISTER_SAMEHANDLER( 84585, 84584 );
//	REGISTER_SAMEHANDLER( 84586, 84584 );
	//Incite
	REGISTER_HANDLER( 86627 );
	//Sudden Doom
	REGISTER_HANDLER( 81340 );
	//Sword and Board
	REGISTER_HANDLER( 50227 );
	//Hand of Light
//	REGISTER_HANDLER( 90174 );
	//Inner Focus
	REGISTER_HANDLER( 89485 );
	//Soulburn
	REGISTER_HANDLER( 74434 );
	//druid - Nature's Swiftness
	REGISTER_HANDLER( 17116 );
	//Shaman - Nature's Swiftness
	REGISTER_HANDLER( 16188 );
	//Denounce
	REGISTER_HANDLER( 85509 );
	//Reckoning - remove only when we actually use the spell
	REGISTER_HANDLER( 20178 );
	//cold blood will get removed on offensive spell
	REGISTER_HANDLER( 14177 );
	//Presence of Mind
	REGISTER_HANDLER( 12043 );
	//Shaman - Shamanist Focus
	REGISTER_HANDLER( 43339 );
	//DK - Rime
	REGISTER_HANDLER( 59052 );
	//DK - Killing Machine
	REGISTER_HANDLER( 51124 );
	//warlock - Backdraft - only remove on destruction spells
	REGISTER_HANDLER( 54277 );
	//mage - Firestarter - only remove on flamestrike
	REGISTER_HANDLER( 54741 );
	//mage - Hot Streak
	REGISTER_HANDLER( 48108 );
	//paladin - Infusion of Light
	REGISTER_HANDLER( 53672 );
	REGISTER_SAMEHANDLER( 54149, 53672 );
	//mage Arcane Concentration - clearcasting
	REGISTER_HANDLER( 12536 );
	//paladin - Art of War
	REGISTER_HANDLER( 59578 );
	//warrior - juggernaut
	REGISTER_HANDLER( 65156 );
	//Druid: Omen of Clarity -> clearcasting
	REGISTER_HANDLER( 16870 );
	//hunter - lock and load
	REGISTER_HANDLER( 56453 );
	//warrior - Bloodsurge
	REGISTER_HANDLER( 46916 );
	//mage brain freez
	REGISTER_HANDLER( 57761 );
	//Corruption Triggers Crit
	REGISTER_HANDLER( 61595 );
	//shaman - Maelstrom Weapon
	REGISTER_HANDLER( 53817 );
	//Item - Moonkin Starfire Bonus
//	REGISTER_HANDLER( 46833 );
	//Item - Paladin T10 Holy 4P Bonus
	REGISTER_HANDLER( 70757 );
	//Impact
	REGISTER_HANDLER( 64343 );
	//Taste for Blood
	REGISTER_HANDLER( 60503 );
	//Predator's Swiftness
	REGISTER_HANDLER( 69369 );
	//Improved Steady Shot
//	REGISTER_HANDLER( 53220 );
	//warlock molten core
	REGISTER_HANDLER( 47383 );
	REGISTER_SAMEHANDLER( 71162, 47383 );
	REGISTER_SAMEHANDLER( 71165, 47383 );
	//mage arcane potency
	REGISTER_HANDLER( 57529 );
	REGISTER_SAMEHANDLER( 57531, 57529 );
}
