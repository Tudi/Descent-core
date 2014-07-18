#include "StdAfx.h"

#define REGISTER_HANDLER( spell ) sp = dbcSpell.LookupEntryForced( spell ); \
	ASSERT( sp->ProcHandler == 0 ); \
	sp->ProcHandler = &PH_##spell;
#define REGISTER_SAMEHANDLER(spell,copy_from) sp = dbcSpell.LookupEntryForced( spell ); \
	ASSERT( sp->ProcHandler == 0 ); \
	sp->ProcHandler = &PH_##copy_from;

bool TargetGivesXPOrHonor( Unit *Victim, Unit *Attacker )
{
	if( Attacker == NULL || Victim == NULL )
		return false;
	//not our own summons
//	if( Victim->GetTopOwner() == attacker )
//		return false;
	//not any summons
//	if( Victim->GetUInt32Value( UNIT_FIELD_CREATEDBY ) != 0 || Victim->GetUInt32Value( UNIT_FIELD_SUMMONEDBY ) != 0 || Victim->GetUInt32Value( UNIT_CREATED_BY_SPELL ) != 0 )
	if( ( Victim->GetUInt32Value( UNIT_FIELD_CREATEDBY ) != 0 || Attacker->GetUInt32Value( UNIT_FIELD_CREATEDBY ) != 0 ) && Victim->IsPet() == false )
		return false;
	if( abs( (int32)Victim->getLevel() - (int32)Attacker->getLevel() ) < 10 )
		return true;
	return false;
}

void PH_71948( ProcHandlerContextShare *context )
{
	if( context->in_dmg <= 0 )
		return;
	int32 dmg_to_deal = context->in_dmg - context->in_abs;
	dmg_to_deal = dmg_to_deal * (int32)context->in_event->created_with_value / 100;
	if( dmg_to_deal <= 0 )
		return;
	Unit *target = context->in_Caller->GetMapMgr()->GetUnit( context->in_event->caster );
	if( target == NULL )
		return;
	int32 abs = target->ResilianceAbsorb( dmg_to_deal, context->in_OwnerSpell, context->in_Caller );
	dmg_to_deal -= abs;
	context->in_Caller->DealDamage( target, dmg_to_deal, context->in_event->origId );
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_69369( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL || context->in_Caller->IsPlayer() == false )
		return;
	//requires finishing move
	if( ( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_FINISHING_MOVE	) == 0 )
		return;
	uint32 chance = SafePlayerCast( context->in_Caller )->m_comboPoints * 20;
	if( RandomUInt() % 100 > chance )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_48391( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() && SafePlayerCast( context->in_Caller )->GetShapeShift() != FORM_MOONKIN )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_16959( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	Player * p = SafePlayerCast( context->in_Caller );
	if( p->GetShapeShift() != FORM_BEAR )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_34299( ProcHandlerContextShare *context )
{
	//this spell will only be available in these forms
//	if( context->in_Caller->IsPlayer() )
	{
//		uint8 SS = SafePlayerCast( context->in_Caller )->GetShapeShift();
//		if( SS == FORM_BEAR || SS == FORM_CAT )
		{
//			context->out_dmg_overwrite[0] = context->in_Caller->GetUInt32Value( UNIT_FIELD_MAXHEALTH ) * 4 / 100;
//			context->in_Caller->Energize( context->in_Caller, 34299, context->in_Caller->GetMaxPower( POWER_TYPE_MANA ) * 4 / 100, POWER_TYPE_MANA );
			context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
		}
	}
	//if we got here then do not continue execution
}

void PH_58179( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_SHRED &&
		context->in_CastingSpell->NameHash != SPELL_HASH_MAUL &&
		context->in_CastingSpell->NameHash != SPELL_HASH_MANGLE ) 
		return;

	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_16953( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	if( context->in_CastingSpell == NULL )
	{
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
		return;
	}
	Player * p = SafePlayerCast( context->in_Caller );
	if( p->GetShapeShift() != FORM_CAT || ( 
		context->in_CastingSpell->NameHash != SPELL_HASH_CLAW &&
		context->in_CastingSpell->NameHash != SPELL_HASH_RAKE &&
		context->in_CastingSpell->NameHash != SPELL_HASH_RAVAGE &&
		context->in_CastingSpell->NameHash != SPELL_HASH_SHRED ) )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
} 

void PH_14189( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->eff[0].Effect != SPELL_EFFECT_ADD_COMBO_POINTS &&
		context->in_CastingSpell->eff[1].Effect != SPELL_EFFECT_ADD_COMBO_POINTS &&
		context->in_CastingSpell->eff[2].Effect != SPELL_EFFECT_ADD_COMBO_POINTS  
//		&& context->in_CastingSpell->NameHash == SPELL_HASH_MUTILATE &&  //this spell is a combination of 3 spells
//		context->in_CastingSpell->NameHash == SPELL_HASH_MUTILATE_OFF_HAND 
		)
		return;
	//wait for this spell we are proccing to finish it
	context->in_Caller->CastSpellDelayed( context->in_Caller->GetGUID(), context->out_CastSpell->Id, 1, true);
//	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_31616( ProcHandlerContextShare *context )
{
	//yep, another special case: Nature's grace
	if( context->in_Caller->GetHealthPct() > 30 )
		return;
//	context->out_dmg_overwrite[0] = context->in_Caller->GetUInt32Value( UNIT_FIELD_MAXHEALTH ) * ( context->in_OwnerSpell->eff[0].EffectBasePoints + 1) / 100;
	int32 base_hp = context->in_Caller->GetUInt32Value( UNIT_FIELD_MAXHEALTH );
	//to avoid looped health increase
	if( context->in_Caller->IsPlayer() )
		base_hp -= SafePlayerCast( context->in_Caller )->GetHealthFromSpell();
	//wtf protection
	if( base_hp < 0 )
		base_hp = 0; 
	context->out_dmg_overwrite[0] = base_hp * ( context->in_OwnerSpell->eff[0].EffectBasePoints + 1) / 100;
	//no idea about this value :S
	if( context->in_Victim->GetAIInterface() )
		context->in_Victim->GetAIInterface()->modThreatByPtr( context->in_Caller, -context->out_dmg_overwrite[0]*2 );

	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_37309( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	if( SafePlayerCast( context->in_Caller )->GetShapeShift() != FORM_BEAR ||
		SafePlayerCast( context->in_Caller )->GetShapeShift() != FORM_DIREBEAR )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_37310( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	if( SafePlayerCast( context->in_Caller )->GetShapeShift() != FORM_BEAR ||
		SafePlayerCast( context->in_Caller )->GetShapeShift() != FORM_DIREBEAR )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_57514( ProcHandlerContextShare *context )
{
	if( context->in_Caller->HasAuraWithNameHash( SPELL_HASH_DEFENSIVE_STANCE, 0, AURA_SEARCH_POSITIVE ) == false )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_16459( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	if( SafePlayerCast( context->in_Caller )->GetItemInterface())
	{
		Item* it = SafePlayerCast( context->in_Caller )->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_MAINHAND );
		if( it != NULL && it->GetProto() )
		{
			uint32 reqskill=GetSkillByProto( it->GetProto()->Class, it->GetProto()->SubClass );
			if( reqskill != SKILL_SWORDS && reqskill != SKILL_2H_SWORDS )
				return;
		}
		else
			return; //no weapon no joy
	}
	else
		return; //no weapon no joy
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_12721( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	Item* it;
	if( SafePlayerCast( context->in_Caller )->GetItemInterface())
	{
		it = SafePlayerCast( context->in_Caller )->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_MAINHAND );
		if( it != NULL && it->GetProto() )
		{
			//class 2 means weapons ;)
			if( it->GetProto()->Class != 2 )
				return;
		}
		else 
			return; //no weapon no joy
	}
	else 
		return; //no weapon no joy

	uint32 multiplyer=0;
	if(context->in_event->origId==12834)
		multiplyer = 16;//level 1 of the talent should apply 20 of avarege melee weapon dmg
	else if(context->in_event->origId==12849)
		multiplyer = 32;
	else if (context->in_event->origId==12867)
		multiplyer = 48;
	if(multiplyer)
	{
//		Item *it;
//		it = SafePlayerCast( context->in_Caller )->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_MAINHAND );
//		uint32 value = float2int32( it->GetProto()->getDPS() );
		SpellEntry *sp = dbcSpell.LookupEntryForced( context->in_event->spellId );
//		uint32 value = float2int32( context->in_Caller->GetFloatValue( UNIT_FIELD_MINDAMAGE ) );
		dmg_storage_struct tdmg;
		memset( &tdmg, 0, sizeof( tdmg ) );
//		CalculateDamage( context->in_Caller, context->in_Victim, MELEE, sp->GetSpellGroupType(), sp, tdmg );
		CalculateDamage( context->in_Caller, context->in_Victim, MELEE, 0, NULL, tdmg );
		uint32 value = ( tdmg.base_min + tdmg.base_max ) / 2;
		value = multiplyer * value / 100;
		value = value / sp->quick_tickcount;
		context->out_dmg_overwrite[ 0 ] = value;
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_52437( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	SafePlayerCast(context->in_Caller)->ClearCooldownForSpell( 86346 );	//colosus smash
//	context->in_event->LastTrigger = getMSTime(); // consider it triggered
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}
/*
void PH_12964( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	//let's recalc chance to cast since we have a full 100 all time on this one
	Item* it;
	if( SafePlayerCast( context->in_Caller )->GetItemInterface() )
	{
		it = SafePlayerCast( context->in_Caller )->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_MAINHAND );
		if( !( it != NULL && it->GetProto() ) )
			return; //no weapon no joy
	}
	else
		return; //no weapon no joy
	//float chance=float(it->GetProto()->Delay)*float(talentlevel)/600.0f;
	uint32 talentlevel = 0;
	switch( context->in_OwnerSpell->Id )
	{
		//mace specialization
		case 12284:	{talentlevel = 1;}break;
		case 12701:	{talentlevel = 2;}break;
		case 12702:	{talentlevel = 3;}break;
		case 12703:	{talentlevel = 4;}break;
		case 12704:	{talentlevel = 5;}break;

		//Unbridled Wrath
		case 12999:	{talentlevel = 1;}break;
		case 13000:	{talentlevel = 2;}break;
		case 13001:	{talentlevel = 3;}break;
		case 13002:	{talentlevel = 4;}break;
	}

	float chance = float( it->GetProto()->Delay ) * float(talentlevel ) / 300.0f; //zack this had a very low proc rate. Kinda liek a waisted talent
	uint32 myroll = RandomUInt( 100 );
	if( myroll > chance )
		return; 
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
} */

void PH_12654( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->School != SCHOOL_FIRE )
		return;
	if( context->in_dmg <= 0 )
		return;
//	if( context->in_CastingSpell->quick_tickcount > 1 )
	if( ( context->in_events_filter_flags & PROC_ON_SPELL_HIT_EVENT ) == 0 )
		return;
	SpellEntry* spellInfo = dbcSpell.LookupEntryForced( context->in_event->spellId ); //we already modified this spell on server loading so it must exist
//	int32 fire_spell_dmg = context->in_CastingSpell->quick_tickcount * ( context->in_dmg - context->in_abs );
	int32 fire_spell_dmg = context->in_dmg;	//if we multiply by dots then ignite will do 9x fireball dmg ...100k ?
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints * fire_spell_dmg / 100;
//	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints * context->in_Caller->GetDamageDoneMod( SCHOOL_FIRE ) / 100;
	context->out_dmg_overwrite[0] = context->out_dmg_overwrite[0] / spellInfo->quick_tickcount;	//over 2 seconds with 2 ticks ?
	if( context->out_dmg_overwrite[0] < 0 || context->out_dmg_overwrite[0] > 100000 ) 
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_64343( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( ( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_DAMAGING ) == 0 )
		return;
	//..reset the cooldown on Fire Blast..
	if( context->in_Caller->IsPlayer() )
		SafePlayerCast( context->in_Caller )->ClearCooldownForSpell( 2136 );	//Fire Blast
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_48108( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	//sadly this will also be called on DOT ticks. Would need to detect only direct casts
	if( ( context->in_events_filter_flags & PROC_ON_NO_DOT ) == 0 )
		return;
	//only trigger effect for specified spells
	if( context->in_CastingSpell->NameHash != SPELL_HASH_FIREBALL &&
		context->in_CastingSpell->NameHash != SPELL_HASH_FROSTFIRE_BOLT && 
		context->in_CastingSpell->NameHash != SPELL_HASH_SCORCH && 
		context->in_CastingSpell->NameHash != SPELL_HASH_PYROBLAST && 
		context->in_CastingSpell->NameHash != SPELL_HASH_FIRE_BLAST ) 
	{
		return;
	}
	//Any time you score 2 non-periodic critical strikes in a row with your Fireball, Frostfire Bolt, Scorch, Pyroblast, or Fire Blast spells, you have a $m1% chance to trigger your Hot Streak effect.
	//we hit the target with correct spell but it is critical then we may reset the counter
	uint32 *TempDataStore = (uint32*)&context->in_event->created_with_value;
	if( ( context->in_events_filter_flags & PROC_ON_CRIT ) != PROC_ON_CRIT )
	{
		TempDataStore[0] = 0;
		return;
	}
	//we casted a different spell before ? Reset our counter
/*	if( TempDataStore[1] != context->in_CastingSpell->NameHash )
	{
		TempDataStore[0] = 1;
		TempDataStore[1] = context->in_CastingSpell->NameHash;
		return;
	}*/
	TempDataStore[0]++;
	//need 2 consecutive casts like above list
	if( TempDataStore[0] < 2 )
		return; 
	TempDataStore[0] = 0; //we have the 2 casts, now reset the counter so we can proc on the same 2 casts again
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_37116( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	Player* mPlayer = SafePlayerCast(context->in_Caller);
	if (!mPlayer->IsInFeralForm() || 
		(mPlayer->GetShapeShift() != FORM_CAT &&
		mPlayer->GetShapeShift() != FORM_BEAR &&
		mPlayer->GetShapeShift() != FORM_DIREBEAR))
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_45182( ProcHandlerContextShare *context )
{
	if( context->in_dmg <= 0 )
		return;
	//is this a killer blow ?
	int32 new_health = (int32)context->in_Caller->GetHealth() - (context->in_dmg - context->in_abs);
	if( new_health > 0 )
		return;
	//we should remain with a top 10% health
	int32 health_now = context->in_Caller->GetHealth();
	int32 top_health_keep = health_now * 10 / 100; //10% is hardcoded in description
	int32 health_to_keep = MIN( health_now, top_health_keep );
	if( health_to_keep < (context->in_dmg - context->in_abs) )
		context->out_dmg_absorb += (context->in_dmg - context->in_abs);
	else
		context->out_dmg_absorb += health_to_keep - (context->in_dmg - context->in_abs);
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[1].EffectBasePoints;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}
/*
void PH_31665( ProcHandlerContextShare *context )
{
	//if this is proc on steathing then we let it just pass
	//if it is casted when removing stealth then we force duration 6 seconds
	if( context->in_events_filter_flags & PROC_ON_AURA_REMOVE )
	{
		context->in_Caller->RemoveAuraByNameHash( SPELL_HASH_MASTER_OF_SUBTLETY );	//does not remove the original talent aura
		//now proc a new spell with forced duration
		SpellCastTargets targets( context->in_Caller->GetGUID() );
		SpellEntry *spellInfo = dbcSpell.LookupEntry( context->in_event->spellId );
		Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		spell->Init(context->in_Caller, spellInfo ,true, NULL);
		spell->forced_duration = 6000;
		spell->forced_basepoints[0] = context->in_OwnerSpell->eff[0].EffectBasePoints + 1;
		spell->ProcedOnSpell = context->in_CastingSpell;
		spell->pSpellId=context->in_OwnerSpell->Id;
		spell->prepare(&targets);
		return;
	}
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints + 1;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}*/

void PH_31125( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	//only trigger effect for specified spells
	if( context->in_CastingSpell->NameHash != SPELL_HASH_BACKSTAB && //backstab
		context->in_CastingSpell->NameHash != SPELL_HASH_SINISTER_STRIKE && //sinister strike
		context->in_CastingSpell->NameHash != SPELL_HASH_SHIV && //shiv
		context->in_CastingSpell->NameHash != SPELL_HASH_GOUGE ) //gouge
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_79264( ProcHandlerContextShare *context )
{
	//target needs to provide XP
	if( TargetGivesXPOrHonor( context->in_Victim, context->in_Caller ) == false )
		return;
//	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	SpellCastTargets targets( context->in_Victim->GetGUID() );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init( context->in_Victim, context->out_CastSpell ,true, NULL);
	spell->ProcedOnSpell = context->in_CastingSpell;
	spell->pSpellId=context->in_OwnerSpell->Id;
	spell->prepare(&targets);
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_27285( ProcHandlerContextShare *context )
{
	if( context->in_dmg <= 0 )
		return;
	int32 RealDamage = context->in_dmg - context->in_abs;
	bool can_proc_now = false;
	//if we proced on spell tick
	if( ( context->in_events_filter_flags & PROC_ON_SPELL_HIT_VICTIM_DOT ) == PROC_ON_SPELL_HIT_VICTIM_DOT )
	{
		if( !context->in_CastingSpell )
			return;
		//only trigger effect for specified spells
		if( context->in_CastingSpell->NameHash != SPELL_HASH_SEED_OF_CORRUPTION )						
			return;
		//this spell builds up in time
		if( RealDamage > 0 )
			context->in_event->procCharges += RealDamage;
		if( (int32)context->in_event->procCharges >= context->in_OwnerSpell->eff[ 1 ].EffectBasePoints && //if charge built up
			RealDamage < (int32)context->in_Caller->GetUInt32Value( UNIT_FIELD_HEALTH ) ) //if this is not a killer blow
			can_proc_now = true;
	}
	else 
		can_proc_now = true; //target died
	if( can_proc_now == false )
		return;
	Unit *new_caster = context->in_Victim;
	if( new_caster && new_caster->isAlive() )
	{
		SpellEntry *spellInfo = dbcSpell.LookupEntry( context->in_event->spellId ); //we already modified this spell on server loading so it must exist
		Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		spell->Init( new_caster, spellInfo ,true, NULL );
		SpellCastTargets targets2;
		targets2.m_destX = context->in_Caller->GetPositionX();
		targets2.m_destY = context->in_Caller->GetPositionY();
		targets2.m_destZ = context->in_Caller->GetPositionZ();
		spell->prepare(&targets2);
	}
	context->in_event->deleted = true;
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_18371( ProcHandlerContextShare *context )
{
	context->out_dmg_overwrite[0] = ( context->in_OwnerSpell->eff[2].EffectBasePoints + 1 ) * context->in_Caller->GetMaxPower( POWER_TYPE_MANA ) / 100;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_31117( ProcHandlerContextShare *context )
{
	//dispell Unstable Affliction
//	if( context->in_dmg != 30108 )
//		return;

	Unit *u_victim = context->in_Caller;
	Aura *ta = u_victim->HasAura( context->in_OwnerSpell->Id, 0, AURA_SEARCH_NEGATIVE );
	if( ta )
	{
		Unit *u_caster = ta->GetUnitCaster();
		if( u_caster == NULL )
			return;
		uint32 TickCount = context->in_OwnerSpell->quick_tickcount;	
//		uint32 AuraDuration = ta->GetDuration();
//		uint32 Amplitude = GetSpellAmplitude( ta->GetSpellProto(), u_caster, 1, false );
//		TickCount = AuraDuration / Amplitude; //seems like "9" is hardcoded in spell description
		TickCount = 9; //seems like "9" is hardcoded in spell description

		dealdamage tdmg;
		tdmg.base_dmg = ta->m_modList[0].m_amount;
		tdmg.pct_mod_final_dmg = ta->m_modList[0].m_pct_mod;
		tdmg.DisableProc = true;
		tdmg.StaticDamage = false;
		u_caster->SpellNonMeleeDamageCalc( u_victim, ta->GetSpellProto(), &tdmg, ta->GetSpellProto()->quick_tickcount, ta->m_modList[0].i );
		//remove critical. Conflagrate can crit too
		if( tdmg.pct_mod_crit )
			tdmg.full_damage = tdmg.full_damage * 100 / ( 100 + tdmg.pct_mod_crit );
		int32 ress = tdmg.full_damage - tdmg.resisted_damage - tdmg.absorbed_damage;
		if( ress < 0 )
			ress = 0;
//		context->out_dmg_overwrite[0] = ress * TickCount;

		Spell* spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		SpellCastTargets targets2( u_victim->GetGUID() );
		spell->Init( u_caster, context->out_CastSpell, true, NULL );
		spell->forced_basepoints[0] = ress * TickCount;
		spell->forced_basepoints[1] = ress * TickCount;
		spell->ProcedOnSpell = context->in_CastingSpell;
		spell->pSpellId=context->in_OwnerSpell->Id;
		spell->prepare( &targets2 );
		context->in_event->LastTrigger = getMSTime(); // consider it triggered
		return;
	}
	else 
		context->out_dmg_overwrite[0] = ( context->in_OwnerSpell->eff[0].EffectBasePoints + 1 ) * 9;

	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}
/*
void PH_63106( ProcHandlerContextShare *context )
{
	if( context->in_dmg <= 0 )
		return;
	context->out_dmg_overwrite[0] = ( context->in_OwnerSpell->eff[0].EffectBasePoints + 1 ) * (context->in_dmg - context->in_abs) / 100;
	if( context->out_dmg_overwrite[0] <= 0 )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}*/

void PH_54181( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( ( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_DAMAGING ) == 0 )
		return;
	if( context->in_dmg <= 0 || context->in_dmg <= context->in_abs )
		return;
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints * (context->in_dmg - context->in_abs) / 100;
	if( context->out_dmg_overwrite[0] <= 0 )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_47422( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
//	Aura *a = context->in_Victim->HasAuraWithNameHash( SPELL_HASH_CORRUPTION,  context->in_Caller->GetGUID() );
	Aura *a = context->in_Victim->HasAuraWithNameHash( SPELL_HASH_CORRUPTION, 0, AURA_SEARCH_NEGATIVE );
	if( a )
		a->ResetDuration();
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

//!! i'm not sure this code is even used anymore
void PH_25228( ProcHandlerContextShare *context )
{
	//we need a pet for this, else we do not trigger it at all
	if( context->in_Caller->IsPlayer() == false )
		return;
	if( context->in_dmg <= 0 )
		return;
	Unit* new_caster;
	if( SafePlayerCast( context->in_Caller )->GetSummon() )
		new_caster = SafePlayerCast( context->in_Caller )->GetSummon();
	else if( context->in_Caller->GetUInt64Value( UNIT_FIELD_CHARM ) )
		new_caster = context->in_Caller->GetMapMgr()->GetUnit( context->in_Caller->GetUInt64Value( UNIT_FIELD_CHARM ) );
	else
		new_caster = NULL;
	if( new_caster != NULL && new_caster->isAlive() )
	{
		SpellEntry* spellInfo = dbcSpell.LookupEntry( 25228 ); //we already modified this spell on server loading so it must exist
		Spell* spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		spell->Init( new_caster, spellInfo, true, NULL );
		spell->forced_basepoints[0] = context->in_dmg;
		SpellCastTargets targets2;
		targets2.m_unitTarget = context->in_Caller->GetGUID();
		spell->prepare( &targets2 );
	}
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_32386( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
/*	if( context->in_CastingSpell->NameHash != SPELL_HASH_CORRUPTION && // Corruption
		context->in_CastingSpell->NameHash != SPELL_HASH_CURSE_OF_AGONY && //CoA
		context->in_CastingSpell->NameHash != SPELL_HASH_SIPHON_LIFE && //Siphon Life
		context->in_CastingSpell->NameHash != SPELL_HASH_SEED_OF_CORRUPTION ) //SoC
		return; */

	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_18118( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash == SPELL_HASH_RAIN_OF_FIRE )
	{
		if( RandChance( context->in_OwnerSpell->eff[1].EffectBasePoints ) == false )
			return;
		context->out_CastSpell = dbcSpell.LookupEntryForced( 85387 );	//Aftermath - stun
	}
	else //this is conflagrate, rank 1 has 50% proc chance
	{
		if( RandChance( context->in_OwnerSpell->eff[2].EffectBasePoints ) == false )
			return;
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_54370( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;//this should not ocur unless we made a fuckup somewhere
	//only trigger effect for specified spells
	if(!(context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_DAMAGING))
		return;
	//let's see if we have an absorb aura on us 
	bool has_absorb_aura = false;
	for(uint32 i=POS_AURAS_START;i<MAX_POSITIVE_AURAS1(context->in_Caller);i++)
		if( context->in_Caller->m_auras[i] && 
			( context->in_Caller->m_auras[i]->GetSpellProto()->eff[0].EffectApplyAuraName == SPELL_AURA_SCHOOL_ABSORB
			|| context->in_Caller->m_auras[i]->GetSpellProto()->eff[1].EffectApplyAuraName == SPELL_AURA_SCHOOL_ABSORB
			|| context->in_Caller->m_auras[i]->GetSpellProto()->eff[2].EffectApplyAuraName == SPELL_AURA_SCHOOL_ABSORB ) )
		{
			has_absorb_aura = true;
			break;
		}
	if( has_absorb_aura == false )
		return;
	//depending on school we change the ID of the cast spell
	uint32 school_spells[SCHOOL_COUNT]={0,54370,54371,54375,54372,54374,54373};
	uint32 spellId = school_spells[ context->in_CastingSpell->School ];

	SpellEntry* spellInfo = dbcSpell.LookupEntry( spellId ); 
	Spell* spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init( context->in_Caller, spellInfo, true, NULL );
	SpellCastTargets targets2( context->in_Caller->GetGUID() );
	spell->forced_basepoints[0] = -context->in_OwnerSpell->eff[0].EffectBasePoints;
	spell->prepare( &targets2 );

	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_30294( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_SHADOWBURN && 
		context->in_CastingSpell->NameHash != SPELL_HASH_SOUL_FIRE &&
		context->in_CastingSpell->NameHash != SPELL_HASH_CHAOS_BOLT
		)
		return;

//	context->out_dmg_overwrite[0] = ( context->in_dmg - context->in_abs ) * (context->in_OwnerSpell->eff[0].EffectBasePoints+1)/100;
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints;
	context->out_dmg_overwrite[1] = context->out_dmg_overwrite[0] ;
//	if( context->out_dmg_overwrite[0] <= 0 )
//		return;
	context->in_Caller->CastSpell( context->in_Caller, 57669, true );	//cast refreshment
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}


void PH_53230( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash == SPELL_HASH_RAPID_KILLING )
	{
		context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[1].EffectBasePoints;
		context->out_CastSpell = dbcSpell.LookupEntryForced( 58883 );
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_6150( ProcHandlerContextShare *context )
{
	//nasty :(. We need this active aspect
	if( context->in_Caller->HasAuraWithNameHash( SPELL_HASH_ASPECT_OF_THE_HAWK, 0, AURA_SEARCH_POSITIVE ) == 0 
//		&& context->in_Caller->HasAuraWithNameHash( SPELL_HASH_ASPECT_OF_THE_DRAGONHAWK ) == 0 
		)
		return;
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[1].EffectBasePoints+1;

	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_53295( ProcHandlerContextShare *context )
{
	//dmg is the dispelled spell id
	SpellEntry *spd = dbcSpell.LookupEntryForced( context->in_dmg );
	if( spd == NULL || spd->NameHash != SPELL_HASH_WYVERN_STING )
		return;
	//cast on target this spell
	SpellCastTargets targets2( context->in_Victim->GetGUID() );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init(context->in_Caller, spd ,true, NULL);
//	spell->forced_duration = 6000;	//crap, this is complicated :P
	spell->ProcedOnSpell = context->in_CastingSpell;
	spell->pSpellId=context->in_OwnerSpell->Id;
	spell->prepare(&targets2);
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_57669( ProcHandlerContextShare *context )
{
	if( context->in_Caller->getClass() == WARLOCK )
	{
		context->out_dmg_overwrite[0] = context->in_Caller->GetMaxPower( POWER_TYPE_MANA ) *(context->in_OwnerSpell->eff[0].EffectBasePoints+1)/100;
		//energize self
		context->in_Caller->Energize( context->in_Caller, 57669, context->out_dmg_overwrite[0], POWER_TYPE_MANA, 0 );
		//and our demon
		if( context->in_Caller->IsPlayer() && SafePlayerCast(context->in_Caller)->GetSummon() )
			context->in_Caller->Energize( SafePlayerCast(context->in_Caller)->GetSummon(), 57669, context->out_dmg_overwrite[0], POWER_TYPE_MANA, 0 );
		context->in_event->LastTrigger = getMSTime(); // consider it triggered
		//also has a chance to restore mana for others in party
		if( RandChance( context->in_OwnerSpell->eff[1].EffectBasePoints+1 ) == false )
			return;
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_54648( ProcHandlerContextShare *context )
{
	//actually cast it on the caster that put this aura on us
	SpellCastTargets targets2( context->in_event->caster );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	SpellEntry *spd = dbcSpell.LookupEntryForced( context->in_event->spellId );
	spell->Init(context->in_Caller, spd ,true, NULL);
	spell->ProcedOnSpell = context->in_CastingSpell;
	spell->pSpellId=context->in_OwnerSpell->Id;
	spell->prepare(&targets2);
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_44413( ProcHandlerContextShare *context )
{
	if( ( context->in_events_filter_flags & PROC_ON_AURA_REMOVE_EVENT ) == PROC_ON_AURA_REMOVE_EVENT )
	{
		//we need mana shield to get removed
		if( context->in_CastingSpell->NameHash != SPELL_HASH_MANA_SHIELD )
			return;
		//mana shield removed because it got consumed
		if( context->in_Victim->m_manashieldamt != 0 )
			return;
		context->out_CastSpell = dbcSpell.LookupEntryForced( 86261 ); //knockback attackers
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
		context->in_event->procInterval = context->in_OwnerSpell->proc_interval; //!! need to catch the aura remove event because this will most probably get removed in 1 hit
	}
	else
	{
		//When you absorb damage your spell damage is increased by $s1% of the amount absorbed for $44413d.  
		//Total spell damage increase cannot exceed 5% of your health.
		context->out_dmg_overwrite[0] = (context->in_OwnerSpell->eff[0].EffectBasePoints + 1) * context->in_abs / 100;
	//	if( context->out_dmg_overwrite[0] > (int32)context->in_Caller->GetUInt32Value( UNIT_FIELD_HEALTH ) * 5 / 100 )
	//		context->out_dmg_overwrite[0] = (int32)context->in_Caller->GetUInt32Value( UNIT_FIELD_HEALTH ) * 5 / 100;
		//for sake of people reporting it as not working
		if( context->out_dmg_overwrite[0] < 1 )
			context->out_dmg_overwrite[0] = 1;
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
		context->in_event->procInterval = 0; //!! need to catch the aura remove event because this will most probably get removed in 1 hit
	}
}

void PH_12579( ProcHandlerContextShare *context )
{
	// Winter's Chill shouldn't proc on self
	if (context->in_Victim == context->in_Caller) 
		return;
	if( context->in_CastingSpell == NULL )
		return;//this should not ocur unless we made a fuckup somewhere
	//only trigger effect for specified spells
	if(!(context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_DAMAGING))
		return;
	//only frost spels
	if( context->in_CastingSpell->School != SCHOOL_FROST )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_63675( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;//this should not ocur unless we made a fuckup somewhere

	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init( context->in_Caller, context->in_CastingSpell ,true, NULL);
	int32 flat_tick = spell->CalculateEffect( 0, context->in_Victim );
	SpellPool.PooledDelete( spell, __FILE__, __LINE__ );

	int32 spell_dmg_bonus = context->in_Caller->GetSpellDmgBonus( context->in_Victim, context->in_CastingSpell, flat_tick, 0 );
	int32 dmg_tick = spell_dmg_bonus + flat_tick;
	if( dmg_tick <= 0 )
		return;
	context->out_dmg_overwrite[0] = dmg_tick * (context->in_OwnerSpell->eff[0].EffectBasePoints) * context->in_CastingSpell->quick_tickcount / 100;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_63544( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;//this should not ocur unless we made a fuckup somewhere
	if( context->in_dmg <= 0 )
       return;

	Aura *a = context->in_Victim->HasAuraWithNameHash( SPELL_HASH_RENEW, 0, AURA_SEARCH_POSITIVE );
	if( a )
	{
		//did we proc already on this Aura ? Not bullet proof but the chance to get the same aura is very small
		if( context->in_event->custom_holder == a )
			return;
		context->in_event->custom_holder = a;

		uint32 Duration = a->GetDuration();
		uint32 Amplitude = GetSpellAmplitude( a->GetSpellProto(), context->in_Caller, 0, a->m_castedItemId );
		uint32 TickCount = Duration / Amplitude;
		int32 Dmg = context->in_dmg;
		if( ( context->in_events_filter_flags & ( PROC_ON_CRIT ) ) == PROC_ON_CRIT )
			Dmg = Dmg / 2;

		context->out_dmg_overwrite[0] = Dmg * (context->in_OwnerSpell->eff[0].EffectBasePoints) * TickCount / 100;
	}

//	int32 heal_tick = context->in_dmg + context->in_Caller->GetSpellHealBonus( context->in_Victim, context->in_CastingSpell, context->in_dmg, 0);
//	context->out_dmg_overwrite[0] = heal_tick * (context->in_OwnerSpell->eff[0].EffectBasePoints) * TickCount / 100;

	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_33110( ProcHandlerContextShare *context )
{
	//victim got a hit in the face so we jump on next injured target
	//find aura on self and get it's value
	Aura *pa = context->in_Caller->HasAura( context->in_OwnerSpell->Id );
	if( !pa || !context->in_Caller->GetMapMgr() )
		return; //omg we have this proc on us and on second check we don't ? Return instead of continue since this seems to be a corupted object

	int32 JumpsMade = pa->m_modList[0].m_miscValue;
	//check if we jumped proctimes
	if( JumpsMade >= 4 )
		return;

	//we use same caster as firts caster to be able to get heal bonuses !
	Unit *oricaster = context->in_Caller->GetMapMgr()->GetUnit( pa->GetCasterGUID() );
	if( !oricaster || !oricaster->IsPlayer() )
		return;

	//the nasty part : get a new target ^^
	Player *p_oricaster = SafePlayerCast(oricaster);
	Unit *AuraTarget = pa->GetTarget();
	Player *Next_new_target;
	Next_new_target = NULL;

	//create a new session GUID if not done yet. Note that this does not support multiple priests
	int64 *SessionGUID = AuraTarget->GetCreateIn64Extension( EXTENSION_ID_PRAYER_OF_MENDING_BOUNCE );
	if( JumpsMade == 0 )
		*SessionGUID = ((*SessionGUID)&0x0000FFFF) + ( p_oricaster->GetLowGUID() << 16 ) + 1;

	//always heal the target from which we are removing this aura
	p_oricaster->CastSpell( AuraTarget, 33110, true ); //delayed so first we get the dmg then the heal
//	p_oricaster->CastSpellDelayed( AuraTarget, 33110, 1 ); //delayed so first we get the dmg then the heal

	Next_new_target = NULL;
	if( p_oricaster->GetGroup() )
	{
		Group * group = SafePlayerCast( p_oricaster )->GetGroup();
		float range = 40.0f * 40.0f;
		uint32 count = group->GetSubGroupCount();
		group->Lock();
		for( uint8 k = 0; k < count; k++ )
		{
			SubGroup * subgroup = group->GetSubGroup( k );
			if( subgroup )
			{
				for( GroupMembersSet::iterator itr = subgroup->GetGroupMembersBegin(); itr != subgroup->GetGroupMembersEnd(); ++itr )
				{
					if( (*itr)->m_loggedInPlayer == NULL )
						continue;
					if( (*itr)->m_loggedInPlayer->isAlive() == false	)
						continue;
					//cannot target duel
					if( p_oricaster->DuelingWith == (*itr)->m_loggedInPlayer )
						continue;
					//we cannot retarget the same person
//					if( (*itr)->m_loggedInPlayer == context->in_Caller ) 
//						continue;
					//cannot target the oricaster
//					if( (*itr)->m_loggedInPlayer == p_oricaster ) 
//						continue;
					//needs to be in jump range
					if( IsInrange(p_oricaster,(*itr)->m_loggedInPlayer, range) == false )
						continue;
					//get best target. Try to get rid of the oricaster if possible
					if( Next_new_target == NULL || Next_new_target->GetHealthPct() >= (*itr)->m_loggedInPlayer->GetHealthPct() )
					{
						//do not retarget
						int64 *TargetSessionGUID = (*itr)->m_loggedInPlayer->GetCreateIn64Extension( EXTENSION_ID_PRAYER_OF_MENDING_BOUNCE );
						if( *SessionGUID != *TargetSessionGUID )
							Next_new_target = (*itr)->m_loggedInPlayer;
					}
				}
			}
		}
		group->Unlock();
	}

	//try to jump, again, on the new target
	if( Next_new_target != NULL )
	{
		//mark target as already received a heal from us from this cast
		int64 *TargetSessionGUID = p_oricaster->GetCreateIn64Extension( EXTENSION_ID_PRAYER_OF_MENDING_BOUNCE );
		*TargetSessionGUID = *SessionGUID;

		Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		spell->Init( p_oricaster, context->in_OwnerSpell ,true, NULL);
		spell->forced_miscvalues[0] = JumpsMade + 1 ;
		SpellCastTargets targets2( Next_new_target->GetGUID() ); //no target so spelltargeting will get an injured party member
		spell->prepare( &targets2 );
	}
//	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_15258( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;//this should not ocur unless we made a fuckup somewhere
	if( context->in_CastingSpell->School != SCHOOL_SHADOW || !( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_DAMAGING ) ) //we need damaging spells for this, so we suppose all shadow spells casted on target are dmging spells = Wrong
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}
/*
void PH_15473( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return; //hmm, how on earth did they use this talent ?
	if( SafePlayerCast( context->in_Caller )->GetShapeShift() != FORM_SHADOW )
		return;
	//remove movement imparing effects from caster
	context->in_Caller->RemoveAurasByInterruptFlag(AURA_INTERRUPT_ON_ANY_DAMAGE_TAKEN);
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
} */

void PH_27813( ProcHandlerContextShare *context )
{
 	if( context->in_Caller->IsPlayer() == false )
		return; //hmm, how on earth did they use this talent ?
	if( context->in_dmg <= 0 )
        return;
    SpellEntry *parentproc= dbcSpell.LookupEntry(context->in_OwnerSpell->Id);
	SpellEntry *spellInfo = dbcSpell.LookupEntry(context->in_event->spellId);
	if (!parentproc || !spellInfo)
		return;
	int32 val = parentproc->eff[0].EffectBasePoints + 1;
    Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init(context->in_Caller, spellInfo ,true, NULL);
	spell->forced_basepoints[0] = (val*context->in_dmg)/(spellInfo->quick_tickcount)/100; //per tick
    SpellCastTargets targets2( context->in_Caller->GetGUID() );
    spell->prepare(&targets2);
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_51945( ProcHandlerContextShare *context )
{
//	if( !context->in_CastingSpell || !( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_HEALING ) )
//		return;
	//only working on targets with low health
	if( context->in_OwnerSpell->NameHash == SPELL_HASH_BLESSING_OF_THE_ETERNALS )
	{
		if( context->in_Victim->GetHealthPct() > 35 )
			return;
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_52179( ProcHandlerContextShare *context )
{
	//self stun ? noway
	if( context->in_Victim == context->in_Caller )
		return;
	if( context->in_CastingSpell->MechanicsType != MECHANIC_SILENCED
		&& context->in_CastingSpell->MechanicsType != MECHANIC_STUNNED
		&& context->in_CastingSpell->MechanicsType != MECHANIC_FLEEING )
		return;
	SpellCastTargets targets2( context->in_Caller->GetGUID() ); //no target so spelltargeting will get an injured party member
	SpellEntry *spellInfo = dbcSpell.LookupEntry( context->in_event->spellId );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init(context->in_Caller, spellInfo ,true, NULL);
	spell->forced_basepoints[0] = -context->in_OwnerSpell->eff[0].EffectBasePoints - 1;
	spell->forced_duration = context->in_CastingSpell->quick_duration_min;
	spell->ProcedOnSpell = context->in_CastingSpell;
	spell->pSpellId=context->in_OwnerSpell->Id;
	spell->prepare(&targets2);
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_70831( ProcHandlerContextShare *context )
{
	//get the count of Maelstrom Weapon
	Aura *p = context->in_Caller->HasAura( 53817 );
	if( ( p && p->m_visualSlot < MAX_VISIBLE_AURAS && context->in_Caller->m_auraStackCount[ p->m_visualSlot ] == 5 ) == false )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_63375( ProcHandlerContextShare *context )
{
	if( context->in_OwnerSpell->NameHash == SPELL_HASH_PRIMAL_WISDOM )
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	else if( context->in_CastingSpell )
	{
		context->out_dmg_overwrite[0] = context->in_CastingSpell->eff[0].EffectBasePoints;
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	}
}

void PH_16177( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( !(context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_HEALING) ) //healing spell
		return;
	//crit hits reduce dmg taken 
	if( ( context->in_events_filter_flags & PROC_ON_CRIT ) == PROC_ON_CRIT )
		context->in_Caller->CastSpell( context->in_Victim, context->out_CastSpell, true );

	if( ( context->in_events_filter_flags & PROC_ON_HEAL_SPELL ) == PROC_ON_HEAL_SPELL )
	{
		int32 flat_inc = context->in_dmg * context->in_OwnerSpell->eff[1].EffectBasePoints / 100;
		//did we get buffed by this spell before ? If so, then we add that to us
		Aura *PrevApply = context->in_Victim->HasAura( 105284, 0, AURA_SEARCH_POSITIVE );
		if( PrevApply && PrevApply->m_modList[0].m_AuraName == SPELL_AURA_MOD_INCREASE_HEALTH )
			flat_inc += PrevApply->m_modList[0].m_amount;
		int32 SpellHealth = 0;
		if( context->in_Victim->IsPlayer() )
			SpellHealth = SafePlayerCast( context->in_Victim )->GetHealthFromSpell();
		int32 inc_limit = ( context->in_Victim->GetMaxHealth() - SpellHealth ) / 100 * 10;
		if( flat_inc > inc_limit )
			flat_inc = inc_limit;
		context->out_dmg_overwrite[0] = flat_inc;
		context->out_CastSpell = dbcSpell.LookupEntryForced( 105284 ); //Ancestral Vigor
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_16246( ProcHandlerContextShare *context )
{
	if( context->in_OwnerSpell->Id == 39805 )
		return; // Lightning Overload Proc is already free
	if( context->in_CastingSpell == NULL )
		return;
	if(context->in_CastingSpell->NameHash!=SPELL_HASH_LIGHTNING_BOLT&&
		context->in_CastingSpell->NameHash!=SPELL_HASH_CHAIN_LIGHTNING&&
		context->in_CastingSpell->NameHash!=SPELL_HASH_EARTH_SHOCK&&
		context->in_CastingSpell->NameHash!=SPELL_HASH_FLAME_SHOCK&&
		context->in_CastingSpell->NameHash!=SPELL_HASH_FROST_SHOCK)
		return;
	SpellCastTargets targets2( context->in_Victim->GetGUID() ); //no target so spelltargeting will get an injured party member
	SpellEntry *spellInfo = dbcSpell.LookupEntry( context->in_event->spellId );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init( context->in_Caller, spellInfo ,true, NULL);
	spell->ProcedOnSpell = context->in_CastingSpell;
	spell->pSpellId=context->in_OwnerSpell->Id;
	spell->prepare(&targets2);
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_33750( ProcHandlerContextShare *context )
{
 	if( context->in_Caller->IsPlayer() == false )
		return; 
#ifdef OLD_VERSION_NO_VISUALS
	SpellEntry *sp_for_the_logs;
	Item *it;
/*
	if(SafePlayerCast( context->in_Caller )->GetItemInterface())
	{
		it = SafePlayerCast( context->in_Caller )->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_MAINHAND);
		if(it)
		{
			EnchantmentInstance * Enchantment = it->GetEnchantment(ITEM_ENCHANT_SLOT_TEMPORARY2);
			if( Enchantment )
			{
				uint32 tspellid;
				if( Enchantment->Enchantment->Id == 283 )
					tspellid = 8232;
				else if( Enchantment->Enchantment->Id == 284 )
					tspellid = 8235;
				else if( Enchantment->Enchantment->Id == 525 )
					tspellid = 10486;
				else if( Enchantment->Enchantment->Id == 1669 )
					tspellid = 16362;
				else if( Enchantment->Enchantment->Id == 2636 )
					tspellid = 25505;
				else if( Enchantment->Enchantment->Id == 3785 )
					tspellid = 58801;
				else if( Enchantment->Enchantment->Id == 3786 )
					tspellid = 58803;
				else if( Enchantment->Enchantment->Id == 3787 )
					tspellid = 58804;
				else return;
				sp_for_the_logs = dbcSpell.LookupEntry( tspellid );
			}
			else return;
		}
		else return;
	}
	else return;
*/
/*
	uint32 AP_owerride = sp_for_the_logs->eff[1].EffectBasePoints + 1;
	//we have to recalc the value of this spell
	uint32 dmg2 = SafePlayerCast( context->in_Caller )->GetMainMeleeDamage(AP_owerride);
	context->in_Caller->Strike( context->in_Victim, MELEE, sp_for_the_logs, dmg2, 0, 0, true, false );
	if( SafePlayerCast( context->in_Caller )->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_OFFHAND) )
		context->in_Caller->Strike( context->in_Victim, MELEE, sp_for_the_logs, dmg2, 0, 0, true, false );
*/
	//in 4.3 and we will calc effect value
	sp_for_the_logs = dbcSpell.LookupEntryForced( 8232 );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init( context->in_Caller, sp_for_the_logs, true, NULL);
	int32 AP_owerride = spell->CalculateEffect( 1, context->in_Victim );
	//we have to recalc the value of this spell
	uint32 dmg2 = SafePlayerCast( context->in_Caller )->GetMainMeleeDamage(AP_owerride);
	SpellPool.PooledDelete( spell );

	//has offhand ?
	it = SafePlayerCast( context->in_Caller )->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_OFFHAND);
	bool has_offhand;
	if( it && it->GetProto()->Class == ITEM_CLASS_WEAPON && (context->in_events_filter_flags & PROC_OFFHAND_WEAPON) )
		has_offhand = true;
	else
		has_offhand = false;

//	SpellCastTargets targets2( context->in_Caller->GetGUID() ); //no target so spelltargeting will get an injured party member
	for(int32 i=0;i<3;i++)
	{
		if( has_offhand )
			context->in_Caller->Strike( context->in_Victim, OFFHAND, sp_for_the_logs, dmg2, 0, 0, true, false );
		else
			context->in_Caller->Strike( context->in_Victim, MELEE, sp_for_the_logs, dmg2, 0, 0, true, false );
/*		Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		spell->Init(context->in_Caller, context->out_CastSpell ,true, NULL);
		spell->forced_basepoints[0] = value / 14;	//damage is roughly AP / 14
		spell->ProcedOnSpell = context->in_CastingSpell;
		spell->pSpellId = context->in_OwnerSpell->Id;
		spell->prepare(&targets2);*/
	}
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
//	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
#else

	//ranged spell. Maybe wee should test in general, if target is further then melee range yards then just exit
	if( context->in_CastingSpell && context->in_CastingSpell->NameHash == SPELL_HASH_UNLEASH_ELEMENTS )
		return;

	context->in_event->LastTrigger = getMSTime(); // consider it triggered, do not chain proc it ;)

	//has offhand ?
	Item *it = SafePlayerCast( context->in_Caller )->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_OFFHAND);
	bool has_offhand;
	if( it && it->GetProto()->Class == ITEM_CLASS_WEAPON && (context->in_events_filter_flags & PROC_OFFHAND_WEAPON) )
		has_offhand = true;
	else
		has_offhand = false;

	uint32 ExtraAP = context->in_event->created_with_value;
	uint32 ExtraDMG = ExtraAP / 14;
	SpellEntry *MainHand = dbcSpell.LookupEntryForced( 25504 );
	SpellEntry *OffHand = dbcSpell.LookupEntryForced( 33750 );
	SpellCastTargets targets2( context->in_Victim->GetGUID() ); 
	for(int32 i=0;i<3;i++)
	{
		Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		spell->Init(context->in_Caller, MainHand ,true, NULL);
		spell->forced_basepoints[0] = ExtraDMG;	
		spell->ProcedOnSpell = context->in_CastingSpell;
		spell->pSpellId = context->in_OwnerSpell->Id;
		spell->prepare(&targets2);
		if( has_offhand )
		{
			Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
			spell->Init(context->in_Caller, OffHand ,true, NULL);
			spell->forced_basepoints[0] = ExtraDMG;	
			spell->ProcedOnSpell = context->in_CastingSpell;
			spell->pSpellId = context->in_OwnerSpell->Id;
			spell->prepare(&targets2);
		}
	}
#endif
}

void PH_14157( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;//this should not ocur unless we made a fuckup somewhere
	//we need a finishing move for this 
	if( !(context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_FINISHING_MOVE) || context->in_Victim==context->in_Caller)
		return;
	//to avoid the combo points getting removed due to this proc cycle
	context->in_Caller->CastSpellDelayed( context->in_Caller->GetGUID(), context->out_CastSpell->Id, 1, true );
//	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_98440( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;//this should not ocur unless we made a fuckup somewhere
	int32 proc_Chance2;
	//chance is based actually on combopoint count and not 100% always 
	if( ( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_FINISHING_MOVE) && context->in_Caller->IsPlayer())
		proc_Chance2 = SafePlayerCast( context->in_Caller )->m_comboPoints*context->in_OwnerSpell->eff[1].EffectBasePoints;
	else 
		return;
	if(!RandChance(proc_Chance2))
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_58427( ProcHandlerContextShare *context )
{
	if( ( context->in_events_filter_flags & PROC_ON_AURA_REMOVE ) == PROC_ON_AURA_REMOVE )
	{
		Aura *a = context->in_Caller->HasAura( context->out_CastSpell->Id );
		if( a )	//maybe manually removed it ?
		{
			a->SetDuration( 20000 );
			a->ResetDuration();
		}
		return;
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_31665( ProcHandlerContextShare *context )
{
	if( ( context->in_events_filter_flags & PROC_ON_AURA_REMOVE ) == PROC_ON_AURA_REMOVE )
	{
		Aura *a = context->in_Caller->HasAura( context->out_CastSpell->Id );
		if( a )	//maybe manually removed it ?
		{
			a->SetDuration( 6000 );
			a->ResetDuration();
		}
		return;
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_32861( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell && ( context->in_Caller == context->in_Victim || !( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_DAMAGING ) ) ) //no self casts allowed or beneficial spells
		return;//this should not ocur unless we made a fuckup somewhere

	dealdamage tdmg;
	tdmg.base_dmg = context->in_OwnerSpell->eff[0].EffectBasePoints+1;
	context->in_Caller->SpellNonMeleeDamageLog( context->in_Victim, context->in_OwnerSpell, &tdmg );

	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_12536( ProcHandlerContextShare *context )
{
	//requires damageing spell
	if( context->in_CastingSpell == NULL )
		return;//this should not ocur unless we made a fuckup somewhere
	if( !( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_DAMAGING ) )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_57761( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	//frost
	if( context->in_CastingSpell->School != SCHOOL_FROST )
		return;
	//damaging
	if( (context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_DAMAGING) == 0 )
		return;
	//speel needs to have chill effect -> no idea :S, all ?
	if( (context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_CHILL_SPELL) == 0 )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_12484( ProcHandlerContextShare *context )
{
	if(  context->in_Victim == context->in_Caller ) //Blizzard
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_29077( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
//	if( CastingSpell->School!=SCHOOL_FIRE && CastingSpell->School!=SCHOOL_FROST) //fire and frost criticals
//		continue;
	int32 costpct = context->in_Caller->GetUInt32Value( UNIT_FIELD_BASE_MANA ) * context->in_CastingSpell->PowerEntry.ManaCostPercentage / 100;
	context->out_dmg_overwrite[0] = (context->in_CastingSpell->PowerEntry.manaCost + costpct)* ( context->in_OwnerSpell->eff[0].EffectBasePoints + 1 ) / 100;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_51185( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell->NameHash == SPELL_HASH_TIGER_S_FURY )
	{
		SpellCastTargets targets2( context->in_Caller->GetGUID() ); //no target so spelltargeting will get an injured party member
		SpellEntry *spellInfo = dbcSpell.LookupEntry( 51178 );
		Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		spell->Init( context->in_Caller, spellInfo ,true, NULL);
		spell->ProcedOnSpell = context->in_CastingSpell;
		spell->pSpellId=context->in_OwnerSpell->Id;
		spell->forced_basepoints[0] = context->in_OwnerSpell->eff[ 1 ].EffectBasePoints; //!! not same as next !
		spell->prepare(&targets2);
		return;
	}
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[ 0 ].EffectBasePoints + 1; //!! not same as next !
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}
/*
void PH_51178( ProcHandlerContextShare *context )
{
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[ 1 ].EffectBasePoints + 1;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}*/

void PH_13810( ProcHandlerContextShare *context )
{
	if( context->in_Caller->m_ObjectSlots[ 0 ] == NULL ) 
		return; //no trap active ?
	GameObject *trap = context->in_Caller->GetMapMgr()->GetGameObject( context->in_Caller->m_ObjectSlots[0] );

	SpellCastTargets targets2; //no target so spelltargeting will get an injured party member
	targets2.m_targetMask = TARGET_FLAG_SOURCE_LOCATION;
	targets2.m_srcX = trap->GetPositionX();
	targets2.m_srcY = trap->GetPositionY();
	targets2.m_srcZ = trap->GetPositionZ();

	SpellEntry *spellInfo = dbcSpell.LookupEntry( context->in_event->spellId );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init(context->in_Caller, spellInfo ,true, NULL);
	spell->ProcedOnSpell = context->in_CastingSpell;
	spell->pSpellId = context->in_OwnerSpell->Id;
	spell->prepare(&targets2);
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_62606( ProcHandlerContextShare *context )
{
	uint8 shapeshift = context->in_Caller->GetByte( UNIT_FIELD_BYTES_2, 3 );
	if( shapeshift != FORM_BEAR && shapeshift != FORM_DIREBEAR )
		return;
	context->out_dmg_overwrite[0] = context->in_Caller->GetAP() * ( context->in_OwnerSpell->eff[0].EffectBasePoints + 1 ) / 100;
	//wtf protection ?
	if( context->out_dmg_overwrite[0] < 0 )
		context->out_dmg_overwrite[0] = 0;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_34720( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	uint32 mana_cost_prev = context->in_Caller->GetPower( POWER_TYPE_MANA ) * context->in_CastingSpell->PowerEntry.ManaCostPercentage / 100 + context->in_CastingSpell->PowerEntry.manaCost;
	context->out_dmg_overwrite[0] = mana_cost_prev * 40 / 100;
//	Energize( this, 34720, dmg_overwrite, POWER_TYPE_MANA );
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_63468( ProcHandlerContextShare *context )
{
	if( context->in_dmg <= 0 )
		return;
	context->out_dmg_overwrite[0] = context->in_dmg * ( context->in_OwnerSpell->eff[0].EffectBasePoints + 1) / 100;
	context->out_dmg_overwrite[0] = context->out_dmg_overwrite[0] / context->out_CastSpell->quick_tickcount;
	if( context->out_dmg_overwrite[0] <= 0 )
		return;
	//try to stack with previous version without refreshing previous version
	Aura *a = context->in_Victim->HasAura( context->out_CastSpell->Id, 0, AURA_SEARCH_NEGATIVE );
	if( a )
	{
		int32 Period = GetSpellAmplitude( context->out_CastSpell, context->in_Caller, 0, 0 );

		int32 TotalTicks = a->GetDuration() / Period;
		int32 TotalDamageToDo = TotalTicks * a->m_modList[0].m_amount;
		int32 TimeRemainingPCT = a->GetTimeLeft() * 100 / a->GetDuration();
		int32 DamageLeftToDo = TotalDamageToDo * TimeRemainingPCT / 100;
		int32 DamageLeftToDoTick = DamageLeftToDo / TotalTicks;

		int32 TicksToBeDone = MAX( 1, a->GetTimeLeft() / Period );
		int32 DamageToBeDoneRemaining = TicksToBeDone * a->m_modList[0].m_amount;
		int32 DamageToBeDoneRemainingTick = DamageToBeDoneRemaining / TotalTicks;

		int32 DamageTickBestAprox = MIN( DamageLeftToDoTick, DamageToBeDoneRemainingTick );	//this was scaling to infinity

		a->m_modList[0].m_amount = context->out_dmg_overwrite[0] + DamageTickBestAprox;
		a->ResetDuration();
		return;
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_47753( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( (context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_HEALING ) == 0 )
		return;
	if(! ( (context->in_events_filter_flags & PROC_ON_CRIT)==PROC_ON_CRIT || context->in_CastingSpell->NameHash == SPELL_HASH_PRAYER_OF_HEALING ) )
		return;
	if( context->in_dmg <= 0 )
		return;
	// from 4.0.6 wowhead comments : will refresh duration and stack amount up to 40% of owner max health !
	context->out_dmg_overwrite[0] = context->in_dmg * ( context->in_OwnerSpell->eff[0].EffectBasePoints + 1 ) / 100;
	int32 MaxAmount = context->in_Victim->GetMaxHealth() / 100 * 40 ;
	if( context->out_dmg_overwrite[0] < MaxAmount )
	{
		//see if we already have this aura on us
		CommitPointerListNode<Absorb> *i;
		context->in_Victim->Absorbs.BeginLoop();
		for( i = context->in_Victim->Absorbs.begin(); i != context->in_Victim->Absorbs.end(); i = i->Next() )
		{
			Absorb *tabs = i->data;
			if( tabs->spellid == context->out_CastSpell->Id 
//				tabs->caster == context->in_Caller->GetGUID() //stacks from different casters
				)
			{
				context->out_dmg_overwrite[0] += tabs->amt;
				break;
			}
		}
		context->in_Victim->Absorbs.EndLoopAndCommit();
	}
	if( context->out_dmg_overwrite[0] > MaxAmount )
		context->out_dmg_overwrite[0] = MaxAmount;

	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_63731( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	context->out_dmg_overwrite[0] = ( context->in_CastingSpell->PowerEntry.manaCost + context->in_CastingSpell->PowerEntry.ManaCostPercentage * context->in_Caller->GetMaxPower( POWER_TYPE_MANA ) / 100 ) * ( context->in_OwnerSpell->eff[0].EffectBasePoints + 1 );
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_33619( ProcHandlerContextShare *context )
{
	if( ( context->in_events_filter_flags & PROC_ON_CAST_SPELL_EVENT ) && context->in_CastingSpell && context->in_CastingSpell->NameHash == SPELL_HASH_POWER_WORD__SHIELD )
	{
		//create a new event on the target. Target can be different then caster !
		ProcTriggerSpell *pts = new ProcTriggerSpell( context->in_OwnerSpell, NULL );
		pts->caster = context->in_Caller->GetGUID();
		pts->spellId = context->out_CastSpell->Id;
		pts->procChance = 100;
		pts->created_with_value = 1;
		pts->procFlags = PROC_ON_ABSORB_VICTIM;
		context->in_Victim->RegisterProcStruct( pts );
	}
	else if( ( context->in_events_filter_flags & PROC_ON_AURA_REMOVE_EVENT ) && context->in_CastingSpell && context->in_CastingSpell->NameHash == SPELL_HASH_POWER_WORD__SHIELD )
	{
		context->in_Victim->UnRegisterProcStruct( NULL, context->in_OwnerSpell->Id, 1 );
	}
	else if( ( context->in_events_filter_flags & PROC_ON_ABSORB_EVENT ) )
	{
	//	if( context->in_CastingSpell == NULL )
	//		return;
		//we should detect if this absorb was made by Power word : shield. But the darn thing gets removed after depletion
		//crossing fingers this is not an absorb from resiliance
		if( context->in_Caller->HasAuraWithNameHash( SPELL_HASH_POWER_WORD__SHIELD, 0, AURA_SEARCH_POSITIVE ) == 0 //the right way
				//frikkin hack guessing if this was the shield last breath. Shield can soak aprox 4000 dmg. Resiliance can soak up to 2000 ?
	//		  || !( context->in_abs > 400 && context->in_Caller->HasAuraWithNameHash( SPELL_HASH_WEAKENED_SOUL, 0, AURA_SEARCH_NEGATIVE ) )
			) 
			return;
		//make a direct strike then exit rest of handler
		//this will get reported as doing too much damage because abs contains resisted dmg also. Armor absorbs half of the dmg than the rest is absorbed..
		context->out_dmg_overwrite[0] = context->in_abs * ( context->in_OwnerSpell->eff[0].EffectBasePoints + 1 ) / 100;
		//somehow we should make this not cause any threat (tobedone)
	//	SpellNonMeleeDamageLog( victim, power_word_id, dmg_overwrite, false, true );
	//	context->in_Caller->SpellNonMeleeDamageLog( context->in_Victim, 33619, context->out_dmg_overwrite[0], false, true );
	//	context->in_event->LastTrigger = getMSTime(); // consider it triggered
	//	return;
		//or the old traditional way ? What if this gets resisted and stuff ?
	//	dmg_overwrite = Abs * ( ospinfo->eff[0].EffectBasePoints + 1 ) / 100;
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	}
}

void PH_66235( ProcHandlerContextShare *context )
{
 	if( context->in_Caller->IsPlayer() == false )
		return; 
	if( context->in_dmg <= 0 )
		return;
	//Damage that takes you below 35% health is reduced by 20%.  
	int32 new_health = (int32)context->in_Caller->GetHealth() - ( context->in_dmg - context->in_abs);
	if( new_health > 0 )
		return;	
	context->out_dmg_absorb += (-new_health) + 5;	//avoid death
	//400 defense skill gets you no heal
//	int32 defense_skill = SafePlayerCast( context->in_Caller )->_GetSkillLineCurrent( SKILL_DEFENSE );
//						defense_skill = MAX( 0, defense_skill - 400); - good
//	defense_skill = MAX( 0, defense_skill - 300); // - not good but people report it as a bug if it works blizzlike :(
//	uint32 heal_pct = defense_skill * 100 / 140 * (context->in_OwnerSpell->eff[1].EffectBasePoints + 1) / 100;
//	uint32 heal_pct = ( context->in_OwnerSpell->eff[1].EffectBasePoints + 1 ) / 100;
	uint32 heal_pct = 15;
	uint32 heal_flat = context->in_Caller->GetUInt32Value(UNIT_FIELD_MAXHEALTH) * heal_pct / 100;
	context->out_dmg_overwrite[0] = heal_flat;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	return;
/*
	uint32 dmg_overwrite = heal_flat;
	SpellEntry *spellInfo = dbcSpell.LookupEntry( context->in_event->spellId );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init(context->in_Caller, spellInfo ,true, NULL);
	spell->forced_basepoints[0] = dmg_overwrite;
	spell->ProcedOnSpell = context->in_CastingSpell;
	spell->pSpellId=context->in_OwnerSpell->Id;
//printf("!!!!! wil proc spell with target guid %u \n",(uint32)(targets.m_unitTarget) & LOWGUID_ENTRY_MASK);
	SpellCastTargets targets2( context->in_Caller->GetGUID() ); //no target so spelltargeting will get an injured party member
	spell->prepare(&targets2);
//	context->in_event->LastTrigger = getMSTime(); // consider it triggered
	context->in_event->LastTrigger = getMSTime() + 60000 * 2; // consider it triggered and do not let it trigger again for a while
	*/
}

void PH_20178( ProcHandlerContextShare *context )
{
	//do not proc it on abilities like extra attacks
	if( context->in_CastingSpell != NULL )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_31934( ProcHandlerContextShare *context )
{
	if( context->in_dmg <= 0 )
		return;
	int32 res = (context->in_dmg - context->in_abs)/2;
	if( res > 0 )
		context->out_dmg_absorb += res;
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_20272( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL || context->in_Victim != context->in_Caller )
		return;
	uint32 mana_cost_prev = context->in_Caller->GetPower( POWER_TYPE_MANA ) * context->in_CastingSpell->PowerEntry.ManaCostPercentage / 100 + context->in_CastingSpell->PowerEntry.manaCost;
	uint32 dmg_overwrite = mana_cost_prev * (context->in_OwnerSpell->eff[1].EffectBasePoints + 1) / 100;
	context->in_Caller->Energize( context->in_Caller, 20272, dmg_overwrite, POWER_TYPE_MANA, 0 );
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

/*
void PH_31930( ProcHandlerContextShare *context )
{
	context->in_Caller->Energize( context->in_Caller, context->in_OwnerSpell->Id, context->in_Caller->GetUInt32Value( UNIT_FIELD_BASE_MANA ) * 25 / 100, POWER_TYPE_MANA );
	SpellEntry *spellInfo = dbcSpell.LookupEntry( 57669 );//replenish the whole party
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init(context->in_Caller, spellInfo ,true, NULL);
	spell->ProcedOnSpell = context->in_CastingSpell;
	spell->pSpellId=context->in_OwnerSpell->Id;
	SpellCastTargets targets2( context->in_Caller->GetGUID() ); //no target so spelltargeting will get an injured party member
	spell->prepare(&targets2);
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}*/

void PH_54203( ProcHandlerContextShare *context )
{
	if( context->in_dmg <= 0 )
		return;
	const uint32 tickcount = 12000 / 2000; // hehe, never do this ;P
	context->out_dmg_overwrite[0] = ( context->in_OwnerSpell->eff[0].EffectBasePoints + 1 ) * ( context->in_dmg - context->in_abs ) / (100  * tickcount );
	if( context->out_dmg_overwrite[0] <= 0 )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_31786( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	//trigger only on heal spell cast by NOT us
	if( context->in_Victim == context->in_Caller )
		return; 
	if( context->in_dmg <= 0 )
		return;
//	if( !( CastingSpell->c_is_flags & SPELL_FLAG_IS_HEALING ) )
//		continue; 
	//this is not counting the bonus effects on heal
	context->out_dmg_overwrite[0] = context->in_dmg * (context->in_OwnerSpell->eff[0].EffectBasePoints + 1 ) / 100;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_25997( ProcHandlerContextShare *context )
{
	if( context->in_Victim == context->in_Caller )
		return; //not self casted crits
	//requires damageing spell
	if( context->in_CastingSpell == NULL || ( context->in_CastingSpell->SchoolMask & ( SCHOOL_MASK_ANY_MAGICAL ) ) == 0 )
		return;//this should not ocur unless we made a fuckup somewhere
	if(!(context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_DAMAGING))
		return;
	if( context->in_dmg <= 0 )
		return;
	//this should contain the same values as the fixed ones
	context->out_dmg_overwrite[0] = ( context->in_dmg *  (context->in_OwnerSpell->eff[0].EffectBasePoints + 1 )) / 100 ; //only half dmg

	/*
	provided by a patch to give same result ?
	if(itr2->origId == 9799)
		dmg_overwrite = (dmg *  15) / 100;
	
	if(itr2->origId == 25988)
		dmg_overwrite = ( dmg *  30) / 100;
		*/

	int32 half_health = context->in_Caller->GetUInt32Value(UNIT_FIELD_HEALTH) >> 1;
	if( context->out_dmg_overwrite[0] > half_health )
		context->out_dmg_overwrite[0] = half_health ;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}
/*
void PH_59052( ProcHandlerContextShare *context )
{
	//reset cooldown on Howling Blast
	if( context->in_Caller->IsPlayer() )
	{
		SafePlayerCast(context->in_Caller)->ClearCooldownForSpell( 49184 );
		SafePlayerCast(context->in_Caller)->ClearCooldownForSpell( 51409 );
		SafePlayerCast(context->in_Caller)->ClearCooldownForSpell( 51410 );
		SafePlayerCast(context->in_Caller)->ClearCooldownForSpell( 51411 );
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
} */

void PH_71519( ProcHandlerContextShare *context )
{
	uint32 spellId = 0;
	if( context->in_Caller->getClass() == DEATHKNIGHT)
	{
		//DK: Str, Haste, Crit
		const uint32 spells[3] = {71484,71492,71491};
		spellId = spells[ RandomUInt() % 3 ];
	}
	else if( context->in_Caller->getClass() == DRUID )
	{
		//Druid: Str, Agi, Haste
		const uint32 spells[3] = {71484,71485,71492};
		spellId = spells[ RandomUInt() % 3 ];
	}
	else if( context->in_Caller->getClass() == HUNTER )
	{
		//Hunter: Agi, Crit, AP
		const uint32 spells[3] = {71485,71487,71486};
		spellId = spells[ RandomUInt() % 3 ];
	}
	else if( context->in_Caller->getClass() == PALADIN )
	{
		//Paladin: Str, Haste, Crit
		const uint32 spells[3] = {71484,71492,71487};
		spellId = spells[ RandomUInt() % 3 ];
	}
	else if( context->in_Caller->getClass() == ROGUE )
	{
		//Rogue: Agi, Haste, AP
		const uint32 spells[3] = {71485,71492,71486};
		spellId = spells[ RandomUInt() % 3 ];
	}
	else if( context->in_Caller->getClass() == WARRIOR )
	{
		//Warrior: Str, Crit, Haste
		const uint32 spells[3] = {71484,71487,71492};
		spellId = spells[ RandomUInt() % 3 ];
	}
	else 
//							if( getClass() == SHAMAN )	//and rest of the classes
	{
		//Shaman: Agi, AP, Haste
		const uint32 spells[3] = {71485,71486,71492};
		spellId = spells[ RandomUInt() % 3 ];
	}
	SpellEntry *spellInfo = dbcSpell.LookupEntry( spellId );//replenish the whole party
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init(context->in_Caller, spellInfo ,true, NULL);
	spell->ProcedOnSpell = context->in_CastingSpell;
	spell->pSpellId=context->in_OwnerSpell->Id;
	SpellCastTargets targets2( context->in_event->caster ); //no target so spelltargeting will get an injured party member
	spell->prepare(&targets2);
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_71633( ProcHandlerContextShare *context )
{
	if( (int32)context->in_Caller->GetHealthPct() > context->in_OwnerSpell->eff[0].EffectBasePoints )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_40972( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if(!(context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_HEALING))
		return;
	//Melee attacks which reduce you below $s1% health cause you to gain $71633s1 armor for $71633d.  Cannot occur more than once every 30 sec.
	if( (int32)context->in_Caller->GetHealthPct() > context->in_OwnerSpell->eff[0].EffectBasePoints )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_71566( ProcHandlerContextShare *context )
{
	//Each time your spells heal a target you have a chance to cause another nearby friendly target to be instantly healed for $71610s1.
	if( context->in_CastingSpell == NULL )
		return;
	if(!(context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_HEALING))
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_71610( ProcHandlerContextShare *context )
{
	//Each time your spells heal a target you have a chance to cause another nearby friendly target to be instantly healed for $71610s1.
	if( context->in_CastingSpell == NULL )
		return;
	if(!(context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_HEALING))
		return;
	//need to find a new target with low health
	Unit *minHPunit = NULL;
	InRangeSetRecProt::iterator itr;
	context->in_Caller->AquireInrangeLock(); //make sure to release lock before exit function !
	InrangeLoopExitAutoCallback AutoLock;
	for( itr = context->in_Caller->GetInRangeSetBegin( AutoLock ); itr != context->in_Caller->GetInRangeSetEnd(); itr++ )
	{
		if( !(*itr)->IsUnit() || !SafeUnitCast((*itr))->isAlive())
			continue;

		//we target stuff that has no full health. No idea if we must fill target list or not :(
		if( (*itr)->GetUInt32Value( UNIT_FIELD_HEALTH ) == (*itr)->GetUInt32Value( UNIT_FIELD_MAXHEALTH ) )
			continue;

		if( !isAttackable(context->in_Caller,(*itr)) )
		{
			if( minHPunit == NULL )
				minHPunit = SafeUnitCast(*itr);
			else if( minHPunit->GetHealthPct() > SafeUnitCast(*itr)->GetHealthPct() )
				minHPunit = SafeUnitCast(*itr);
		}
	}
	context->in_Caller->ReleaseInrangeLock();	

	if( minHPunit == NULL )
		minHPunit = context->in_Caller;

	SpellEntry *spellInfo = dbcSpell.LookupEntry( context->in_event->spellId );//replenish the whole party
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init(context->in_Caller, spellInfo ,true, NULL);
	spell->ProcedOnSpell = context->in_CastingSpell;
	spell->pSpellId=context->in_OwnerSpell->Id;
	SpellCastTargets targets2( minHPunit->GetGUID() ); //no target so spelltargeting will get an injured party member
	spell->prepare(&targets2);
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_72418( ProcHandlerContextShare *context )
{
	//needs benficial spell
	if( isAttackable(context->in_Caller,context->in_Victim ) )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_60530( ProcHandlerContextShare *context )
{
	//needs benficial spell
	if( context->in_CastingSpell == NULL )
		return;
	//need direct spells
	if( context->in_CastingSpell->eff[0].EffectAmplitude != 0 && context->in_CastingSpell->eff[1].EffectAmplitude != 0 && context->in_CastingSpell->eff[2].EffectAmplitude != 0 )
		return;
	if( ( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_HEALING ) == 0 )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_71888( ProcHandlerContextShare *context )
{
	uint32 spellId;
	//the casted spell depends on the power type of the target
	switch( context->in_Victim->GetPowerType() )
	{
		case POWER_TYPE_MANA : spellId = 71888;			break;
		case POWER_TYPE_ENERGY : spellId = 71887;		break;
		case POWER_TYPE_RAGE : spellId = 71886;			break;
		default: return;
	}
	SpellEntry *spellInfo = dbcSpell.LookupEntry( spellId );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init(context->in_Caller, spellInfo ,true, NULL);
	spell->ProcedOnSpell = context->in_CastingSpell;
	spell->pSpellId = context->in_OwnerSpell->Id;
	SpellCastTargets targets2( context->in_Caller->GetGUID() ); //no target so spelltargeting will get an injured party member
	spell->prepare(&targets2);
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_71881( ProcHandlerContextShare *context )
{
	uint32 spellId;
	//the casted spell depends on the power type of the target
	switch( context->in_Victim->GetPowerType() )
	{
		case POWER_TYPE_MANA : spellId = 71881;			break;
		case POWER_TYPE_ENERGY : spellId = 71882;		break;
		case POWER_TYPE_RAGE : spellId = 71882;			break;
		default: return;
	}
	SpellEntry *spellInfo = dbcSpell.LookupEntry( spellId );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init(context->in_Caller, spellInfo ,true, NULL);
	spell->ProcedOnSpell = context->in_CastingSpell;
	spell->pSpellId = context->in_OwnerSpell->Id;
	SpellCastTargets targets2( context->in_Caller->GetGUID() ); //no target so spelltargeting will get an injured party member
	spell->prepare(&targets2);
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_72416( ProcHandlerContextShare *context )
{
	//needs harmfull spell
	if( isAttackable(context->in_Caller,context->in_Victim ) == false )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_71564( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	//need direct spells
	if( context->in_CastingSpell->eff[0].EffectAmplitude != 0 && context->in_CastingSpell->eff[1].EffectAmplitude != 0 && context->in_CastingSpell->eff[2].EffectAmplitude != 0 )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_33370( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	//need direct spells
	if( !( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_DAMAGING ) )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_67354( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( 
//		context->in_CastingSpell->NameHash != SPELL_HASH_MANGLE__BEAR_ 
//		&& context->in_CastingSpell->NameHash != SPELL_HASH_MANGLE__CAT_
		context->in_CastingSpell->NameHash != SPELL_HASH_LACERATE
		&& context->in_CastingSpell->NameHash != SPELL_HASH_SWIPE
		&& context->in_CastingSpell->NameHash != SPELL_HASH_SHRED
		)
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_60520( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( !( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_DAMAGING ) && !( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_HEALING ))
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_60518( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( !( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_HEALING ))
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_53372( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( !( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_DAMAGING ))
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_45055( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( !( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_DAMAGING ))
		return;
	//need periodic spells
	if( context->in_CastingSpell->eff[0].EffectAmplitude == 0 && context->in_CastingSpell->eff[1].EffectAmplitude == 0 && context->in_CastingSpell->eff[2].EffectAmplitude == 0 )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_38324( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( !( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_HEALING ))
		return;
	//need periodic spells
	if( context->in_CastingSpell->eff[0].EffectAmplitude == 0 && context->in_CastingSpell->eff[1].EffectAmplitude == 0 && context->in_CastingSpell->eff[2].EffectAmplitude == 0 )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_75493( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( !( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_HEALING ))
		return;
	//need direct spells
	if( context->in_CastingSpell->eff[0].EffectAmplitude != 0 || context->in_CastingSpell->eff[1].EffectAmplitude != 0 || context->in_CastingSpell->eff[2].EffectAmplitude != 0 )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_379( ProcHandlerContextShare *context )
{
//	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints + 1; ;
	context->out_dmg_overwrite[0] = context->in_event->created_with_value ;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_30675( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_dmg <= 0 )
		return;	//never, i tell you that never !
	SpellEntry *spellInfo = dbcSpell.LookupEntry( context->in_CastingSpell->Id );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init(context->in_Caller, spellInfo ,true, NULL);
	spell->forced_basepoints[0] = context->in_dmg / 2;	//half damage but same spell
	spell->static_dmg[0] = 1;	//do not double add the spell power coefficient
	spell->ProcedOnSpell = context->in_CastingSpell;
	spell->pSpellId = context->in_OwnerSpell->Id;
	SpellCastTargets targets2( context->in_Caller->GetGUID() ); //no target so spelltargeting will get an injured party member
	spell->prepare(&targets2);
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_63685( ProcHandlerContextShare *context )
{
	//target needs to ne at min N yards
	if( context->in_Victim == NULL )
		return;
	SpellEntry *spe = dbcSpell.LookupEntry( context->in_event->spellId );
//	float r = ( spe->eff[0].EffectBasePoints + 1.0f ) * ( spe->eff[0].EffectBasePoints + 1.0f );
	float r = 15.0f * 15.0f;
	if( context->in_Caller->GetDistanceSq( context->in_Victim ) < r )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_48517( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_STARFIRE )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_48518( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_WRATH )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_37379( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if (context->in_CastingSpell->School != SCHOOL_SHADOW || !(context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_DAMAGING))
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_71905( ProcHandlerContextShare *context )
{
	//count the number of soul shards on us
	SpellEntry *spe = dbcSpell.LookupEntry( context->in_event->spellId );
	uint32 count = context->in_Caller->CountAura( 71905, AURA_SEARCH_POSITIVE );
	if( count == spe->maxstack )
	{
		context->in_Caller->RemoveAura( 71905, 0, AURA_SEARCH_ALL, MAX_PASSIVE_AURAS );	//soul fragment
		context->in_Caller->RemoveAura( 72521, 0, AURA_SEARCH_ALL, MAX_PASSIVE_AURAS );	//shadowmourne effect low 
		context->in_Caller->RemoveAura( 72523, 0, AURA_SEARCH_ALL, MAX_PASSIVE_AURAS );	//shadowmourne effect high
		uint32 spellId = 71904;	//Chaos Bane
		SpellEntry *spellInfo = dbcSpell.LookupEntry( spellId );
		Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		spell->Init(context->in_Caller, spellInfo ,true, NULL);
		spell->ProcedOnSpell = context->in_CastingSpell;
		spell->pSpellId = context->in_OwnerSpell->Id;
		SpellCastTargets targets2( context->in_Caller->GetGUID() ); //no target so spelltargeting will get an injured party member
		spell->prepare(&targets2);
		context->in_event->LastTrigger = getMSTime(); // consider it triggered
		return;
	}
	//visual effect some souls around the caster
//	if( count == 0 )
	{
		SpellEntry *spellInfo = dbcSpell.LookupEntry( 72521 );
		Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		spell->Init(context->in_Caller, spellInfo ,true, NULL);
		spell->ProcedOnSpell = context->in_CastingSpell;
		spell->forced_duration = 60000;
		spell->pSpellId = context->in_OwnerSpell->Id;
		SpellCastTargets targets2( context->in_Caller->GetGUID() ); //no target so spelltargeting will get an injured party member
		spell->prepare(&targets2);
	}
	//visual effect some souls around the caster with larger area
	if( count >= 5 )
	{
		SpellEntry *spellInfo = dbcSpell.LookupEntry( 72523 );
		Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		spell->Init(context->in_Caller, spellInfo ,true, NULL);
		spell->ProcedOnSpell = context->in_CastingSpell;
		spell->forced_duration = 60000;
		spell->pSpellId = context->in_OwnerSpell->Id;
		SpellCastTargets targets2( context->in_Caller->GetGUID() ); //no target so spelltargeting will get an injured party member
		spell->prepare(&targets2);
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_71432( ProcHandlerContextShare *context )
{
	//count the number of soul shards on us
	SpellEntry *spe = dbcSpell.LookupEntry( context->in_event->spellId );
	uint32 count = context->in_Caller->CountAura( 71432, AURA_SEARCH_POSITIVE );
	if( count == spe->maxstack )
	{
		context->in_Caller->RemoveAura( 71432, 0, AURA_SEARCH_ALL, MAX_PASSIVE_AURAS );
		uint32 spellId = 71433;	
		SpellEntry *spellInfo = dbcSpell.LookupEntry( spellId );
		Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		spell->Init(context->in_Caller, spellInfo ,true, NULL);
		spell->ProcedOnSpell = context->in_CastingSpell;
		spell->pSpellId = context->in_OwnerSpell->Id;
		SpellCastTargets targets2( context->in_Caller->GetGUID() ); //no target so spelltargeting will get an injured party member
		spell->prepare(&targets2);
		context->in_event->LastTrigger = getMSTime(); // consider it triggered
		return;
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_37378( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->School != SCHOOL_FIRE || !(context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_DAMAGING) )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_68055( ProcHandlerContextShare *context )
{
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_37656( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}
/*
void PH_40452( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_MANGLE__BEAR_ || context->in_CastingSpell->NameHash != SPELL_HASH_MANGLE__CAT_)
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}*/

void PH_40445( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_STARFIRE )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_40446( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_REJUVENATION )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_40441( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_SHADOW_WORD__PAIN )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_40440( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_RENEW )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_40461( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	//we need a finishing move for this 
	if(!(context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_FINISHING_MOVE) || context->in_Victim==context->in_Caller)
		return;
	if( context->in_Caller->IsPlayer() == false )
		return;
	//chance is combo point based
	uint32 combo_points = SafePlayerCast( context->in_Caller )->m_comboPoints;
	if( RandChance( combo_points * 20 ) == false )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_37445( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_REPLENISH_MANA )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_16886( ProcHandlerContextShare *context )
{
	// Remove aura if it exists so it gets reapplied
//	if ( context->in_Caller->HasAura(16886))
//		context->in_Caller->RemoveAura(16886);
	context->out_dmg_overwrite[ 0 ] = context->in_OwnerSpell->eff[ 0 ].EffectBasePoints;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_48504( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
//	if( context->in_CastingSpell->Id == 48503)
//		return; //do not loop proc
	if( context->in_CastingSpell->NameHash != SPELL_HASH_SWIFTMEND 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_REGROWTH 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_NOURISH 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_HEALING_TOUCH 
		)
		return;
	if( context->in_dmg <= 0 )
		return;
	context->out_dmg_overwrite[0] = context->in_dmg * (context->in_OwnerSpell->eff[0].EffectBasePoints + 1) / 100;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_48503( ProcHandlerContextShare *context )
{
	context->out_dmg_overwrite[0] = (int32)context->in_event->created_with_value;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_64411( ProcHandlerContextShare *context )
{
//	if( dmg == -1 )
//		continue;//this is general proc. We need heal proc with an ammount
	if( context->in_CastingSpell == NULL )
		return;
//	if(!(CastingSpell->c_is_flags & SPELL_FLAG_IS_HEALING)) //requires healing spell.
//		continue;
	if( context->in_dmg <= 0 )
		return;
	SpellEntry *spe = dbcSpell.LookupEntry( context->in_event->spellId );
	context->out_dmg_overwrite[0] = context->in_dmg * (spe->eff[0].EffectBasePoints + 1) / 100;
	if( context->out_dmg_overwrite[0] <= 0 )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_65006( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( !(context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_HEALING) && !(context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_DAMAGING)) //requires healing spell.
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_60229( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( !(context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_HEALING) && !(context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_DAMAGING)) //requires healing spell.
		return;
    SpellEntry *spellInfo;
	Spell *spell;
	SpellCastTargets targets2( context->in_Caller->GetGUID() );
	uint32 spell_id,maxstat_value=0;
	if( context->in_Caller->GetUInt32Value( UNIT_FIELD_STRENGTH ) > maxstat_value )
	{
		maxstat_value = context->in_Caller->GetUInt32Value( UNIT_FIELD_STRENGTH );
		spell_id = 60229;//Increases your Strength by $s1 for $d.
	}
	if( context->in_Caller->GetUInt32Value( UNIT_FIELD_AGILITY ) > maxstat_value )
	{
		maxstat_value = context->in_Caller->GetUInt32Value( UNIT_FIELD_AGILITY );
		spell_id = 60233;//Increases your Agility by $s1 for $d.
	}
	if( context->in_Caller->GetUInt32Value( UNIT_FIELD_INTELLECT ) > maxstat_value )
	{
		maxstat_value = context->in_Caller->GetUInt32Value( UNIT_FIELD_INTELLECT );
		spell_id = 60234;//Increases your Intellect by $s1 for $d.
	}
	if( context->in_Caller->GetUInt32Value( UNIT_FIELD_SPIRIRT ) > maxstat_value )
	{
		maxstat_value = context->in_Caller->GetUInt32Value( UNIT_FIELD_SPIRIRT );
		spell_id = 60235;//Increases your Spirit by $s1 for $d.
	}
	spellInfo = dbcSpell.LookupEntry(spell_id);	
	if (spellInfo)
	{
		spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		spell->Init(context->in_Caller, spellInfo ,true, NULL);
		spell->prepare(&targets2);
	}
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_51124( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell != NULL )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_51789( ProcHandlerContextShare *context )
{
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints + 1;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_50163( ProcHandlerContextShare *context )
{
	//target needs to hold XP or honor. Kinda quick test
	if( TargetGivesXPOrHonor( context->in_Victim, context->in_Caller ) == false )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

//!!! this proc can get disabled by : Runic Corruption
void PH_51460( ProcHandlerContextShare *context )
{
/*	//need auto attacks = normal phisical attacks
	if( context->in_CastingSpell != NULL )
		return;
	if( context->in_dmg <= 0 )
		return;
	context->out_dmg_overwrite[0] = (context->in_OwnerSpell->eff[0].EffectBasePoints + 1) * ( context->in_dmg - context->in_abs ) / 100;
	if( context->out_dmg_overwrite[0] <= 0 )
		return; */
	Aura *a=context->in_Caller->HasAuraWithNameHash( SPELL_HASH_RUNIC_CORRUPTION, 0, AURA_SEARCH_POSITIVE );
	if( a )
	{
		int32 dur = a->GetTimeLeft();
		int32 new_dur = MIN( 9000, dur + 3000 );
		a->SetDuration( new_dur );
		a->ResetDuration();
		return;
	}
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_50536( ProcHandlerContextShare *context )
{
	//we need the damaging death coil not the healing one
	if( context->in_Caller == context->in_Victim )
		return;
	if( context->in_Caller->IsPlayer() == false )
		return;
	if( context->in_dmg <= 0 )
		return;
	context->out_dmg_overwrite[0] = (context->in_OwnerSpell->eff[0].EffectBasePoints + 1) * ( context->in_dmg - context->in_abs ) / 100;
	if( context->out_dmg_overwrite[0] <= 0 || context->out_dmg_overwrite[0] > 5000 )
		return;
	//talk about hardcoded shit
	if( SafePlayerCast(context->in_Caller)->HasGlyphWithID( GLYPH_DEATHKNIGHT_UNHOLY_BLIGHT ) )
		context->out_dmg_overwrite[0] += context->out_dmg_overwrite[0] * 40 / 100;	
	context->out_dmg_overwrite[0] = context->out_dmg_overwrite[0] / context->out_CastSpell->quick_tickcount;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}
/*
void PH_40471( ProcHandlerContextShare *context )
{
	uint32 spellId;
	if( context->in_CastingSpell == NULL )
		return;
	else if( context->in_CastingSpell->NameHash == SPELL_HASH_JUDGEMENT_OF_WISDOM ||
		context->in_CastingSpell->NameHash == SPELL_HASH_JUDGEMENT_OF_JUSTICE ||
		context->in_CastingSpell->NameHash == SPELL_HASH_JUDGEMENT_OF_LIGHT)
		spellId = 40472;
	else if( context->in_CastingSpell->NameHash == SPELL_HASH_FLASH_OF_LIGHT ||
		context->in_CastingSpell->NameHash == SPELL_HASH_HOLY_LIGHT )
		spellId = 40471;
	else 
		return; //not good for us
	SpellEntry *spellInfo = dbcSpell.LookupEntry( spellId );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init(context->in_Caller, spellInfo ,true, NULL);
	spell->ProcedOnSpell = context->in_CastingSpell;
	spell->pSpellId = context->in_OwnerSpell->Id;
	SpellCastTargets targets2( context->in_Caller->GetGUID() ); //no target so spelltargeting will get an injured party member
	spell->prepare(&targets2);
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}*/

void PH_57319( ProcHandlerContextShare *context )
{
	uint32 spellId;
	if( context->in_Victim->GetPowerType() == POWER_TYPE_RAGE )
		spellId = 57320;
	else if( context->in_Victim->GetPowerType() == POWER_TYPE_RUNIC )
		spellId = 57321;
	else 
		return; //not good for us
	SpellEntry *spellInfo = dbcSpell.LookupEntry( spellId );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init( context->in_Caller, spellInfo ,true, NULL );
	spell->ProcedOnSpell = context->in_CastingSpell;
	spell->pSpellId = context->in_OwnerSpell->Id;
	SpellCastTargets targets2( context->in_Victim->GetGUID() ); //no target so spelltargeting will get an injured party member
	spell->prepare(&targets2);
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}
/*
void PH_67371( ProcHandlerContextShare *context )
{
	SpellEntry *spellInfo = dbcSpell.LookupEntry( context->in_event->spellId );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init( context->in_Caller, spellInfo ,true, NULL );
	spell->ProcedOnSpell = context->in_CastingSpell;
	spell->pSpellId = context->in_OwnerSpell->Id;
	SpellCastTargets targets2( context->in_Caller->GetGUID() ); //no target so spelltargeting will get an injured party member
	spell->prepare(&targets2);
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}
*/
void PH_67696( ProcHandlerContextShare *context )
{
	//needs beneficial spell
	if( !context->in_CastingSpell || IsSpellBenefic(context->in_CastingSpell) == false )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_64713( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell || IsSpellHarmfull(context->in_CastingSpell) == false )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_67772( ProcHandlerContextShare *context )
{
	uint32 spellId = context->in_event->spellId;
	//we choose spell based on atribute
	if( context->in_Caller->GetUInt32Value( UNIT_FIELD_STRENGTH ) > context->in_Caller->GetUInt32Value( UNIT_FIELD_AGILITY ) )
		spellId = 67773;
	SpellEntry *spellInfo = dbcSpell.LookupEntry( spellId );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init( context->in_Caller, spellInfo ,true, NULL );
	spell->ProcedOnSpell = context->in_CastingSpell;
	spell->pSpellId = context->in_OwnerSpell->Id;
	SpellCastTargets targets2( context->in_Caller->GetGUID() ); //no target so spelltargeting will get an injured party member
	spell->prepare(&targets2);
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_67703( ProcHandlerContextShare *context )
{
	uint32 spellId = context->in_event->spellId;
	//we choose spell based on atribute
	if( context->in_Caller->GetUInt32Value( UNIT_FIELD_STRENGTH ) > context->in_Caller->GetUInt32Value( UNIT_FIELD_AGILITY ) )
		spellId = 67708;
	SpellEntry *spellInfo = dbcSpell.LookupEntry( spellId );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init( context->in_Caller, spellInfo ,true, NULL );
	spell->ProcedOnSpell = context->in_CastingSpell;
	spell->pSpellId = context->in_OwnerSpell->Id;
	SpellCastTargets targets2( context->in_Caller->GetGUID() ); //no target so spelltargeting will get an injured party member
	spell->prepare(&targets2);
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_67713( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell || (context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_DAMAGING ) == 0 )
		return;
	uint32 spellId = context->in_event->spellId;
	//You gain a Mote of Flame each time you cause a damaging spell critical strike.  When you reach $s1 Motes, they will release, firing a Pillar of Flame for $67714s1 damage.  Mote of Flame cannot be gained more often than once every 2 sec.
	if( context->in_Caller->CountAura( spellId, AURA_SEARCH_POSITIVE ) == context->in_OwnerSpell->eff[0].EffectBasePoints )
	{
		context->in_Caller->RemoveAura( spellId, 0 );
		spellId = 67714;
		SpellEntry *spellInfo = dbcSpell.LookupEntry( spellId );
		Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		spell->Init(context->in_Caller, spellInfo ,true, NULL);
		spell->ProcedOnSpell = context->in_CastingSpell;
		spell->pSpellId = context->in_OwnerSpell->Id;
		SpellCastTargets targets2( context->in_Victim->GetGUID() ); //no target so spelltargeting will get an injured party member
		spell->prepare(&targets2);
		context->in_event->LastTrigger = getMSTime(); // consider it triggered
		return;
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_67759( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell || (context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_DAMAGING ) == 0 )
		return;
	uint32 spellId = context->in_event->spellId;
	//You gain a Mote of Flame each time you cause a damaging spell critical strike.  When you reach $s1 Motes, they will release, firing a Pillar of Flame for $67714s1 damage.  Mote of Flame cannot be gained more often than once every 2 sec.
	if( context->in_Caller->CountAura( spellId, AURA_SEARCH_POSITIVE ) == context->in_OwnerSpell->eff[0].EffectBasePoints )
	{
		context->in_Caller->RemoveAura( spellId, 0, AURA_SEARCH_ALL, MAX_PASSIVE_AURAS );
		spellId = 67760;
		SpellEntry *spellInfo = dbcSpell.LookupEntry( spellId );
		Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		spell->Init(context->in_Caller, spellInfo ,true, NULL);
		spell->ProcedOnSpell = context->in_CastingSpell;
		spell->pSpellId = context->in_OwnerSpell->Id;
		SpellCastTargets targets2( context->in_Victim->GetGUID() ); //no target so spelltargeting will get an injured party member
		spell->prepare(&targets2);
		context->in_event->LastTrigger = getMSTime(); // consider it triggered
		return;
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_64569( ProcHandlerContextShare *context )
{
	if( context->in_Caller->GetHealthPct() >= 35 )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_50895( ProcHandlerContextShare *context )
{
	if( context->in_dmg <= 0 )
		return;
	context->out_dmg_overwrite[0] = ( ( context->in_dmg - context->in_abs ) *  (context->in_OwnerSpell->eff[0].EffectBasePoints + 1 )) / 100;	//just proc a periodic DMG on target with fixed value. Note that spell ID is wrong
	if( context->out_dmg_overwrite[0] <= 0 )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_64801( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;

	context->out_dmg_overwrite[0] = ( context->in_CastingSpell->eff[0].EffectBasePoints + 1 ) * context->in_CastingSpell->quick_tickcount;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_70802( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
/*	if( CastingSpell->NameHash != SPELL_HASH_EVISCERATE &&
		CastingSpell->NameHash != SPELL_HASH_SLICE_AND_DICE &&
		CastingSpell->NameHash != SPELL_HASH_ENVENOM &&
		CastingSpell->NameHash != SPELL_HASH_DEADLY_THROW &&
		CastingSpell->NameHash != SPELL_HASH_EXPOSE_ARMOR &&
		CastingSpell->NameHash != SPELL_HASH_KIDNEY_SHOT &&
		CastingSpell->NameHash != SPELL_HASH_RUPTURE
		)
		continue; */
	//we need a finishing move for this 
	if(!(context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_FINISHING_MOVE) || context->in_Victim==context->in_Caller)
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_71023( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_dmg <= 0 )
		return;
	context->out_dmg_overwrite[0] = (context->in_OwnerSpell->eff[0].EffectBasePoints+1)*( context->in_dmg - context->in_abs )/100;
	if( context->out_dmg_overwrite[0] <= 0 )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_70809( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_dmg <= 0 )
		return;
	context->out_dmg_overwrite[0] = (context->in_OwnerSpell->eff[0].EffectBasePoints+1)*( context->in_dmg - context->in_abs )/100/context->in_OwnerSpell->quick_tickcount;
	if( context->out_dmg_overwrite[0] <= 0 )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_70765( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_Caller->IsPlayer() )
		SafePlayerCast(context->in_Caller)->ClearCooldownForSpell( 53385 ); //divine storm
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_69734( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	uint32 spellId;
	SpellCastTargets targets2( context->in_Victim->GetGUID() ); //no target so spelltargeting will get an injured party member
	if( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_DAMAGING )
		spellId = 62402;
	else if( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_HEALING )
	{
		spellId = 69734;
		targets2.m_unitTarget = context->in_Caller->GetGUID();
	}
	else
		return;
	SpellEntry *spellInfo = dbcSpell.LookupEntry( spellId );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init(context->in_Caller, spellInfo ,true, NULL);
	spell->ProcedOnSpell = context->in_CastingSpell;
	spell->pSpellId = context->in_OwnerSpell->Id;
	spell->prepare(&targets2);
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_70657( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == true )
	{
		if( SafePlayerCast(context->in_Caller)->GetFullRuneCount( RUNE_BLOOD ) != 0 ||
			SafePlayerCast(context->in_Caller)->GetFullRuneCount( RUNE_FROST ) != 0 ||
			SafePlayerCast(context->in_Caller)->GetFullRuneCount( RUNE_UNHOLY ) != 0 )
			return;
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_72260( ProcHandlerContextShare *context )
{
	//get saurfang GUID
	Creature *saurfang = context->in_Caller->GetMapMgr()->GetCreature( context->in_event->caster );
	//self heal the mofo
	if( saurfang )
		saurfang->CastSpell( context->in_event->caster, 72260, true );
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_72255( ProcHandlerContextShare *context )
{
	//actaully cast this spell on all players that have 72293
	InRangeSetRecProt::iterator itr,itr3;
	context->in_Caller->AquireInrangeLock();
	InrangeLoopExitAutoCallback AutoLock;
	for( itr3 = context->in_Caller->GetInRangeSetBegin( AutoLock ); itr3 != context->in_Caller->GetInRangeSetEnd(); )
	{
		itr = itr3;
		itr3++;
		if( !((*itr)->IsUnit()) || !SafeUnitCast((*itr))->isAlive() )
			continue;

		if( SafeUnitCast((*itr))->HasAura( 72293, 0, AURA_SEARCH_NEGATIVE ) == false )
			continue;
		context->in_Caller->CastSpell( SafeUnitCast((*itr)), 72255, true );
	}
	context->in_Caller->ReleaseInrangeLock();
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_55533( ProcHandlerContextShare *context )
{
	if( context->in_Victim == context->in_Caller )
		return;
	if( context->in_dmg <= 0 )
		return;
	context->out_dmg_overwrite[0] = context->in_dmg * (context->in_OwnerSpell->eff[0].EffectBasePoints + 1) / 100;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_54425( ProcHandlerContextShare *context )
{
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[2].EffectBasePoints + 1;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}
/*
void PH_48210( ProcHandlerContextShare *context )
{
	SpellCastTargets targets2( context->in_event->caster );
	SpellEntry *spellInfo = dbcSpell.LookupEntry( context->in_event->spellId );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init(context->in_Caller, spellInfo ,true, NULL);
	spell->ProcedOnSpell = context->in_CastingSpell;
	spell->forced_basepoints[0] = context->in_event->created_with_value;
	spell->pSpellId = context->in_OwnerSpell->Id;
	spell->prepare(&targets2);
	context->in_event->deleted = true;
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}*/
/*
void PH_16953( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell || 
		context->in_CastingSpell->Id == 14189 ||
		context->in_CastingSpell->Id == 16953 ||
		context->in_CastingSpell->Id == 16959 )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}*/
/*
void PH_20167( ProcHandlerContextShare *context )
{
	uint32 AP = context->in_Caller->GetAP();
	uint32 SPH = context->in_Caller->GetDamageDoneMod( SCHOOL_HOLY );
	context->out_dmg_overwrite[0] = ( AP + SPH ) * 15 / 100;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}
*/
void PH_34919( ProcHandlerContextShare *context )
{
	// try to get from ourself the caster of this buff
	Aura *ta = context->in_Caller->HasAuraWithNameHash( SPELL_HASH_VAMPIRIC_TOUCH, context->in_Victim->GetGUID(), AURA_SEARCH_NEGATIVE );
	 //ofc we have aura but maybe not from attacker
	if( !ta )
		return;
	Unit *ac = ta->GetUnitCaster();
	if( !ac || ac != context->in_Victim )
		return;
	//now make the orignal caster cast this spell
	SpellEntry *spellInfo = dbcSpell.LookupEntry( context->in_event->spellId );
    Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init(context->in_Victim, spellInfo ,true, NULL);
    SpellCastTargets targets2( context->in_Victim->GetGUID() );
    spell->prepare(&targets2);
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_15290( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;//this should not ocur unless we made a fuckup somewhere
	if( ( context->in_CastingSpell->SchoolMask & SCHOOL_MASK_SHADOW ) == 0  || !( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_DAMAGING ) ) //we need damaging spells for this, so we suppose all shadow spells casted on target are dmging spells = Wrong
		return;
#ifndef TEMP_DISABLE_SPELL_COEFS
	//need single targeting spell dmg and not AOE
	if( context->in_CastingSpell->spell_coef_flags & SPELL_FLAG_AOE_SPELL )
		return;
#endif
	if( ( context->in_dmg - context->in_abs ) <= 0 )
		return;
	context->out_dmg_overwrite[0] = ( context->in_dmg - context->in_abs ) * context->in_OwnerSpell->eff[0].EffectBasePoints / 100;	

	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}
/*
void PH_47755( ProcHandlerContextShare *context )
{
//	uint32 spellId;
//	if( context->in_Caller->GetPowerType() == POWER_TYPE_RAGE )
//		spellId = 63653;
//	else if( context->in_Caller->GetPowerType() == POWER_TYPE_RUNIC )
//		spellId = 63652;
//	else if( context->in_Caller->GetPowerType() == POWER_TYPE_ENERGY )
//		spellId = 63655;
//	else return;
	//energize ourself too
	//if( victim != this )
	//	this->CastSpell( victim, 47755, true );
	//we need the caster of the aura not the person it burst
	Player *p_caster = context->in_Caller->GetMapMgr()->GetPlayer( context->in_event->caster );
	if( p_caster )
		context->in_Caller->Energize( p_caster, 47755, p_caster->GetMaxPower( POWER_TYPE_MANA ) * context->in_OwnerSpell->eff[0].EffectBasePoints / 1000, POWER_TYPE_MANA );	//yes, divide by 1000 and not 100
//	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}*/

void PH_974( ProcHandlerContextShare *context )
{
	if( context->in_events_filter_flags & PROC_ON_HEAL_EVENT )
		return;
	SpellEntry *spellInfo = dbcSpell.LookupEntry(context->in_event->spellId );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init(context->in_Caller, spellInfo ,true, NULL);
	spell->ProcedOnSpell = context->in_CastingSpell;
	//Spell *spell = new Spell(this,spellInfo,false,0,true,false);
	spell->pSpellId=context->in_event->spellId;
	spell->SpellEffectDummy(0);
	SpellPool.PooledDelete( spell, __FILE__, __LINE__ );
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_34650( ProcHandlerContextShare *context )
{
	SpellEntry *spellInfo = dbcSpell.LookupEntry( context->in_event->spellId );
    Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init( context->in_Caller, spellInfo ,true, NULL);
    SpellCastTargets targets2( context->in_Caller->GetUInt64Value( UNIT_FIELD_CREATEDBY ) );
    spell->prepare(&targets2);
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_50454( ProcHandlerContextShare *context )
{
	//we restore master health 
//	dmg_overwrite = ( dmg - Abs ) * 150 / 100;	//wowhead formula
	if( context->in_dmg - context->in_abs <= 0 )
		return;
	int32 dmg_overwrite = ( context->in_dmg - context->in_abs ) * 15 / 100;	//wowhead formula
	if( dmg_overwrite <= 0 )
		return;
	//sanity check
	if( dmg_overwrite > 1500 )
		return;	
	SpellEntry *spellInfo = dbcSpell.LookupEntry( context->in_event->spellId );
    Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init( context->in_Caller, spellInfo ,true, NULL);
	spell->forced_basepoints[0] = dmg_overwrite;
    SpellCastTargets targets2( context->in_Caller->GetUInt64Value( UNIT_FIELD_CREATEDBY ) );
    spell->prepare(&targets2);
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

//dazed can come from a lot of spell !
void PH_50411( ProcHandlerContextShare *context )
{
/*	if( context->in_OwnerSpell->NameHash == SPELL_HASH_BRAMBLES )
	{
		//only daze if we have barkskin aura on us
		if( ( context->in_events_filter_flags & PROC_ON_MELEE_ATTACK_VICTIM ) == PROC_ON_MELEE_ATTACK_VICTIM )
		{
			if( Rand( context->in_OwnerSpell->eff[2].EffectBasePoints + 1 + 10 ) == false )
				return;
			if( context->in_Caller->HasAuraWithNameHash( SPELL_HASH_BARKSKIN, 0, AURA_SEARCH_PASSIVE ) == 0 )
				return;
			context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
		}
		if( ( context->in_events_filter_flags & PROC_ON_CAST_SPELL ) == PROC_ON_CAST_SPELL )
		{
			if( context->in_CastingSpell == NULL || context->in_CastingSpell->NameHash != SPELL_HASH_FORCE_OF_NATURE )
				return;
			//find our treants and cast the boost spell on them
			std::list<uint64>::iterator gitr,gitr2;
			for( gitr2 = context->in_Caller->m_guardians.begin(); gitr2 != context->in_Caller->m_guardians.end();)
			{
				gitr = gitr2;
				gitr2++;
				Creature *tc = context->in_Caller->GetMapMgr()->GetCreature( (*gitr) );
				if( tc == NULL )
				{
					context->in_Caller->m_guardians.erase( gitr );
					continue;
				}
				if( tc->GetEntry() == 1964 )
					tc->CastSpell( tc, 50419, true );
			}
			return;
		}
	} */
}

void PH_63165( ProcHandlerContextShare *context )
{
	//needs target to have less then x% health
	if( (int32)context->in_Victim->GetHealthPct() > context->in_OwnerSpell->eff[1].EffectBasePoints + 1 )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_15571( ProcHandlerContextShare *context )
{
	//do not proc without dmg
	if( context->in_dmg - context->in_abs <= 0 )
		return;
	//do not proc if this is a dot
	if( context->in_CastingSpell && context->in_CastingSpell->quick_tickcount > 1 )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_50384( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsUnit() == false )
		return;
	//this is effect apply
	int32 val = 0;
	if( ( context->in_events_filter_flags & PROC_ON_CAST_SPELL ) == PROC_ON_CAST_SPELL && context->in_event->custom_holder == 0 )
	{
		context->in_event->custom_holder = (void*)1;	//make sure to apply and remove effect only once in case chained cast is used for "presence"
		context->in_event->created_with_value = context->in_Caller->GetUInt32Value( UNIT_FIELD_STAMINA ) * (context->in_OwnerSpell->eff[0].EffectBasePoints + 1)/100;
		val = (int32)context->in_event->created_with_value;
	}
	else if( ( context->in_events_filter_flags & PROC_ON_AURA_REMOVE ) == PROC_ON_AURA_REMOVE && context->in_event->custom_holder != 0 )
	{
		context->in_event->custom_holder = 0;	//make sure to apply and remove effect only once in case chained cast is used for "presence"
		val = -(int32)context->in_event->created_with_value;
	}

	if( val )
	{
		if( context->in_Caller->IsPlayer() )
		{
			SafePlayerCast( context->in_Caller )->FlatStatModPos[ STAT_STAMINA ] += val;
			SafePlayerCast( context->in_Caller )->CalcStat( STAT_STAMINA );
			SafePlayerCast( context->in_Caller )->UpdateStats();
			SafePlayerCast( context->in_Caller )->UpdateChances();
		}
		else if( context->in_Caller->IsCreature() )
		{
			SafeCreatureCast( context->in_Caller )->FlatStatMod[ STAT_STAMINA ] += val;
			SafeCreatureCast( context->in_Caller )->CalcStat( STAT_STAMINA );
		}
	}
	//!! do not continue execution cause this is a self proc -> would loop to "infinit"
}

void PH_50365( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	if( context->in_CastingSpell == NULL )
		return;
	//the apply 
	if( context->in_event->custom_holder == NULL && context->in_CastingSpell->NameHash == SPELL_HASH_BLOOD_PRESENCE )
	{
		//Increases your rune regeneration by $s3% and 
//		for(uint32 i=0; i<RUNE_TYPE_COUNT;i++)
//			SafePlayerCast( context->in_Caller )->m_mod_rune_power_gain_coeff[i] += context->in_OwnerSpell->eff[2].EffectBasePoints / 100.0f;
		//reduces the chance that you will be critically hit by melee attacks while in Blood Presence by $s2%.  
		SafePlayerCast( context->in_Caller )->res_M_crit_set(SafePlayerCast( context->in_Caller )->res_M_crit_get()+context->in_OwnerSpell->eff[1].EffectBasePoints);
		//In addition, while in Frost Presence or Unholy Presence, you retain $s1% damage reduction from Blood Presence.
/*		float val = MAX( 0.0001f, 1.0f + ((float)(context->in_OwnerSpell->eff[0].EffectBasePoints))/100.0f ); //postive increases dmg taken, negative decreases dmg taken
		for(uint32 x=0;x<SCHOOL_COUNT;x++)
			context->in_Caller->DamageTakenPctMod[x] *= val; */
		int32 val = -context->in_OwnerSpell->eff[0].EffectBasePoints;
		for(uint32 i=0;i<SCHOOL_COUNT;i++)
			context->in_Caller->DamageTakenPctMod[i] += val;	//reduce dmg not increase
		SafePlayerCast( context->in_Caller )->UpdateStats();

		context->in_event->custom_holder = (void*)1;
	}
	else if( context->in_event->custom_holder == (void*)1 )
	{
		//Increases your rune regeneration by $s3% and 
//		for(uint32 i=0; i<RUNE_TYPE_COUNT;i++)
//			SafePlayerCast( context->in_Caller )->m_mod_rune_power_gain_coeff[i] -= context->in_OwnerSpell->eff[2].EffectBasePoints / 100.0f;
		//reduces the chance that you will be critically hit by melee attacks while in Blood Presence by $s2%.  
		SafePlayerCast( context->in_Caller )->res_M_crit_set(SafePlayerCast( context->in_Caller )->res_M_crit_get()-context->in_OwnerSpell->eff[1].EffectBasePoints);
		//In addition, while in Frost Presence or Unholy Presence, you retain $s1% damage reduction from Blood Presence.
/*		float val = MAX( 0.0001f, 1.0f + ((float)(context->in_OwnerSpell->eff[0].EffectBasePoints))/100.0f ); //postive increases dmg taken, negative decreases dmg taken
		val = 1.0f / val;
		for(uint32 x=0;x<SCHOOL_COUNT;x++)
			context->in_Caller->DamageTakenPctMod[x] *= val; */
		int32 val = context->in_OwnerSpell->eff[0].EffectBasePoints;
		for(uint32 i=0;i<SCHOOL_COUNT;i++)
			context->in_Caller->DamageTakenPctMod[i] += val;	//reduce dmg not increase
		SafePlayerCast( context->in_Caller )->UpdateStats();

		context->in_event->custom_holder = NULL;
	}
	//!! do not continue execution cause this is a self proc -> would loop to "infinit"
}

void PH_65661( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == false )
		return;
	//make sure we are dual weilding
	if( context->in_Caller->IsPlayer() == false )
		return;
	Item *it = SafePlayerCast( context->in_Caller )->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_OFFHAND );
	if( it == NULL || it->GetProto()->Class != ITEM_CLASS_WEAPON )	//DKs can only weild weapons and nothing else in offhand ?
		return;
	uint32 spell_id = 0;
	if( context->in_CastingSpell->NameHash == SPELL_HASH_DEATH_STRIKE )
//		spell_id = 66953;	//blizz implemented this in more then 1 way. He has some specially made spells but combat log merges dmg...
		spell_id = 66188;	//blizz implemented this in more then 1 way. He has some specially made spells but combat log merges dmg...
	else if( context->in_CastingSpell->NameHash == SPELL_HASH_OBLITERATE )
		spell_id = 66198;
	else if( context->in_CastingSpell->NameHash == SPELL_HASH_PLAGUE_STRIKE )
//		spell_id = 66992;
		spell_id = 66216;
	else if( context->in_CastingSpell->NameHash == SPELL_HASH_RUNE_STRIKE )
		spell_id = 66217;
	else if( context->in_CastingSpell->NameHash == SPELL_HASH_BLOOD_STRIKE )
//		spell_id = 66979;
		spell_id = 66215;
	else if( context->in_CastingSpell->NameHash == SPELL_HASH_FROST_STRIKE )
//		spell_id = 66962;
		spell_id = 66196;
	if( spell_id )
	{
		SpellEntry *spellInfo = dbcSpell.LookupEntry( spell_id );
		Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		spell->Init( context->in_Caller, spellInfo ,true, NULL);
		SpellCastTargets targets2( context->in_Victim->GetGUID() );
		spell->pSpellId = context->in_event->spellId;
		spell->prepare( &targets2 );
		context->in_event->LastTrigger = getMSTime(); // consider it triggered
	}
	//!! do not continue execution cause this is a self proc -> would loop to "infinit"
}

void PH_88433( ProcHandlerContextShare *context )
{
	//refresh Lifebloom on the target
	if( context->in_Victim == NULL )
		return;
 	for(uint32 t=POS_AURAS_START;t<MAX_POSITIVE_AURAS1(context->in_Victim);t++)
		if( context->in_Victim->m_auras[ t ] 
			&& context->in_Victim->m_auras[ t ]->GetSpellProto()->NameHash == SPELL_HASH_LIFEBLOOM )
				context->in_Victim->m_auras[ t ]->ResetDuration();
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_53652( ProcHandlerContextShare *context )
{
	//cast a similar heal spell on the beacon target. we can get the beacon target from the proc info we saved
	if( context->in_CastingSpell == NULL || !(context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_HEALING) ) //healing spell
		return;

	//some say that if you are the beacon and the caster of the beacon then you should not get the heal at all
	if( context->in_Caller->GetGUID() == context->in_event->caster && context->in_Caller == context->in_Victim )
		return;

	if( context->in_CastingSpell->NameHash != SPELL_HASH_WORD_OF_GLORY 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_HOLY_SHOCK 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_FLASH_OF_LIGHT 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_DIVINE_LIGHT 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_LIGHT_OF_DAWN 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_HOLY_LIGHT )
		return;

	SpellEntry *spellInfo = dbcSpell.LookupEntry( context->in_event->spellId );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init( context->in_Caller, spellInfo ,true, NULL);
	SpellCastTargets targets2( context->in_event->caster );	//caster is actually the target guid
	spell->pSpellId = context->in_event->spellId;
	if( context->in_CastingSpell->NameHash == SPELL_HASH_HOLY_LIGHT )
		spell->forced_basepoints[0] = context->in_dmg;
	else
		spell->forced_basepoints[0] = context->in_dmg / 2;
	spell->prepare( &targets2 );
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_79683( ProcHandlerContextShare *context )
{
	//cast a similar heal spell on the beacon target. we can get the beacon target from the proc info we saved
	if( context->in_CastingSpell == NULL || !(context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_DAMAGING) ) //healing spell
		return;
	//disable proccing us if we have hot streak tallent
	if( context->in_Caller->GetExtension( EXTENSION_ID_DISABLE_HOT_STREAK ) != NULL ) 
	{
		context->in_Caller->SetExtension( EXTENSION_ID_DISABLE_HOT_STREAK, NULL );
		context->in_Caller->UnRegisterProcStruct( context->in_Caller, 79684, 1, 1 );
		context->in_event->procFlags = 0;	//disable proccing Arcane Missiles!
		return;
	}

	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_82661( ProcHandlerContextShare *context )
{
	if( context->in_Caller )
	{
		uint32 energy_restore = 2;
		Aura *a = context->in_Caller->HasAuraWithNameHash( SPELL_HASH_ONE_WITH_NATURE, 0, AURA_SEARCH_PASSIVE );
		if( a )
			energy_restore += a->GetSpellProto()->eff[1].EffectBasePoints;
		context->in_Caller->Energize( context->in_Caller, context->in_OwnerSpell->Id, energy_restore, POWER_TYPE_FOCUS, 0 );
	}
	//SELF PROC, DO not CONTINUE
}

void PH_37243( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	//if we are casting lifebloom we additionally cast Replenishment 
	if( ( context->in_events_filter_flags & PROC_ON_CAST_SPELL ) == PROC_ON_CAST_SPELL && context->in_CastingSpell->NameHash == SPELL_HASH_LIFEBLOOM )
	{
		context->in_Caller->CastSpell( context->in_Caller, 57669, true );	//Replenishment
//		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;	//and cast that insta replenish spell also
	}
	if( ( context->in_events_filter_flags & PROC_ON_HEAL_EVENT )
		&& ( context->in_CastingSpell->NameHash == SPELL_HASH_LIFEBLOOM || context->in_CastingSpell->NameHash == SPELL_HASH_REJUVENATION )
		&& RandChance( 20 )
		&& context->in_event->created_with_value < (int32)getMSTime() )
	{
		SpellEntry * sp = dbcSpell.LookupEntryForced( context->in_event->origId );
		uint32 ammt = ( sp->eff[0].EffectBasePoints + 1 ) * context->in_Caller->GetMaxPower( POWER_TYPE_MANA ) / 100;
		context->in_Caller->Energize( context->in_Caller, 57669, ammt, POWER_TYPE_MANA, 0 );	
		context->in_event->created_with_value = getMSTime() + 12000; // consider it triggered
	}
}
/*
void PH_90174( ProcHandlerContextShare *context )
{
	//proc on no ability but normal melee attack
	if( context->in_CastingSpell != NULL )
		return;
	if( context->in_Caller->IsPlayer() == false )
		return;
	//proc chance is calculated dynamically
	float chance = context->in_Caller->GetFloatValue( PLAYER_MASTERY ) * context->in_OwnerSpell->eff[1].EffectBasePoints / 100.0f;
	if( Rand( chance ) == false )
		return;
	//let's try to remember how many power we have atm. I wonder if this is 0 cause we just got called
//	context->in_event->created_with_value = context->in_Caller->GetMaxPower(  POWER_TYPE_HOLY );
	//hackfix, we are supposed to cast next spell as 3 charge holy power, we will just max out our holy power :(
	context->in_Caller->SetPower(  POWER_TYPE_HOLY, context->in_Caller->GetMaxPower(  POWER_TYPE_HOLY ) );
	//let the aura be aplied
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}*/

void PH_86273( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL || context->in_CastingSpell->quick_tickcount != 1 )
		return;
	if( context->in_Caller->IsPlayer() == false )
		return;
	if( context->in_dmg - context->in_abs <= 0 )
		return;
	int32 conversion_pct = float2int32( context->in_Caller->GetFloatValue( PLAYER_MASTERY ) * context->in_OwnerSpell->eff[1].EffectBasePoints / 100.0f );
	int32 absorb_value = conversion_pct * ( context->in_dmg - context->in_abs ) / 100;
	if( absorb_value <= 0 )
		return;
	//check if target has absorb shield 
	int32 has_shield = -1;
	CommitPointerListNode<Absorb> *i;
	context->in_Victim->Absorbs.BeginLoop();
	for( i = context->in_Victim->Absorbs.begin(); i != context->in_Victim->Absorbs.end(); i = i->Next() )
		if( i->data->spellid == context->out_CastSpell->Id 
			&& i->data->caster == context->in_Caller->GetGUID() //nerf from 4.2 : paladin can only refresh it's own shield
			)
		{
			has_shield = 1;
			absorb_value += i->data->amt;
			break;
		}
	context->in_Victim->Absorbs.EndLoopAndCommit();
	//this nerf was added in 4.2 : http://www.wowpedia.org/Illuminated_Healing
	if( absorb_value > (int32)context->in_Caller->GetMaxHealth() / 3 )
		absorb_value = (int32)context->in_Caller->GetMaxHealth() / 3;
	//if we do not need to create a new shield then we simply update the old one
	if( has_shield == 1 )
	{
		//reset aura duration
		Aura *a = context->in_Victim->HasAura( context->out_CastSpell->Id );
		if( a )
		{
			a->ResetDuration();
			a->m_modList[0].m_amount = absorb_value;
			context->in_Victim->ModVisualAuraStackCount( a, 0 );
		}
		//add extra amt to shield
		i->data->amt = absorb_value;
		// there is no need to cast the spell anymore
		return;
	}
	//create a new shield if we could not refresh old one
	context->out_dmg_overwrite[0] = absorb_value;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_86392( ProcHandlerContextShare *context )
{
	//proc on no ability but normal melee attack
	if( context->in_CastingSpell != NULL )
		return;
	if( context->in_Caller->IsPlayer() == false )
		return;
	//main hand only
	if( ( context->in_events_filter_flags & PROC_OFFHAND_WEAPON ) != 0 )
		return;
	//proc chance is calculated dynamically
	//[Your main-hand attacks have a ${$76806m2/12.5}% chance to grant you an attack that deals damage equal to $86392s2% of a main-hand attack. 
	//Each point of Mastery increases the chance by an additional ${$76806m2/100}.2%.]
	float base_chance = context->in_OwnerSpell->eff[1].EffectBasePoints / 12.5f;
	float chance = base_chance + context->in_Caller->GetFloatValue( PLAYER_MASTERY ) * context->in_OwnerSpell->eff[1].EffectBasePoints / 100.0f;
	if( RandChance( chance ) == false )
		return;
	//calc some ppm for this
	context->in_event->procInterval = MAX( 1000, MIN( 60000, 60000 * ( 100 - chance ) / 100 / 4 ) );
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_59578( ProcHandlerContextShare *context )
{
	//proc on no ability but normal melee attack only. Skip abilities
	if( context->in_CastingSpell != NULL )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_77222( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_Caller->IsPlayer() == false )
		return;
	//proc chance is calculated dynamically
	float chance = context->in_Caller->GetFloatValue( PLAYER_MASTERY ) * context->in_OwnerSpell->eff[1].EffectBasePoints / 100.0f;
	if( RandChance( chance ) == false )
		return;
	//cast a similar spell
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init( context->in_Caller, context->in_CastingSpell ,true, NULL);
	SpellCastTargets targets2( context->in_Victim->GetGUID() );	
	spell->pSpellId = context->in_event->spellId;
	spell->forced_pct_mod[0] = 75;	//75% dmg of the original
	spell->forced_pct_mod[1] = 75;	//75% dmg of the original
	spell->forced_pct_mod[2] = 75;	//75% dmg of the original
	spell->prepare( &targets2 );
	if( context->in_Caller->HasAuraWithNameHash( SPELL_HASH_ITEM___SHAMAN_T13_ELEMENTAL_4P_BONUS__ELEMENTAL_OVERLOAD_, 0, AURA_SEARCH_PASSIVE ) )
		context->in_Caller->CastSpell( context->in_Caller, 105821, true );	//Time Rupture
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
	//this is a self proc, do not continue execution !
}

void PH_77489( ProcHandlerContextShare *context )
{
	//we need direct healing
	if( context->in_CastingSpell == NULL )
		return;
	if( (context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_HEALING) == 0 )
		return;
	if( context->in_CastingSpell->quick_tickcount != 1 )
		return;
	if( context->in_Caller->IsPlayer() == false )
		return;
	if( ( context->in_dmg - context->in_abs ) <= 0 )
		return;
	int32 var_inc = float2int32( context->in_Caller->GetFloatValue( PLAYER_MASTERY ) * context->in_OwnerSpell->eff[1].EffectBasePoints );
	context->out_dmg_overwrite[0] = ( context->in_dmg - context->in_abs ) * var_inc / 100 / 100;
//	context->out_dmg_overwrite[0] = context->out_dmg_overwrite[0] / context->out_CastSpell->quick_tickcount;
	uint32 RealAmplitude = GetSpellAmplitude( context->out_CastSpell, context->in_Caller, 0, 0 );
	uint32 SpellDuration = GetDuration( dbcSpellDuration.LookupEntry( context->out_CastSpell->DurationIndex ) );
	uint32 RealTickCount = SpellDuration / RealAmplitude;
	context->out_dmg_overwrite[0] = context->out_dmg_overwrite[0] / RealTickCount;
	if( context->out_dmg_overwrite[0] <= 0 )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_76858( ProcHandlerContextShare *context )
{
	//proc on no ability but normal melee attack
//	if( context->in_CastingSpell != NULL )
//		return;
	if( context->in_Caller->IsPlayer() == false )
		return;
	//proc chance is calculated dynamically
	float chance = context->in_Caller->GetFloatValue( PLAYER_MASTERY ) * context->in_OwnerSpell->eff[1].EffectBasePoints / 100.0f;
	if( RandChance( chance ) == false )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_76663( ProcHandlerContextShare *context )
{
	//proc on no ability but normal melee attack
//	if( context->in_CastingSpell != NULL )
//		return;
	if( context->in_Caller->IsPlayer() == false )
		return;
	//proc chance is calculated dynamically
	float chance = context->in_Caller->GetFloatValue( PLAYER_MASTERY ) * context->in_OwnerSpell->eff[1].EffectBasePoints / 100.0f;
	if( RandChance( chance ) == false )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_65142( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_PLAGUE_STRIKE &&
		context->in_CastingSpell->NameHash != SPELL_HASH_ICY_TOUCH &&
		context->in_CastingSpell->NameHash != SPELL_HASH_CHAINS_OF_ICE &&
		context->in_CastingSpell->NameHash != SPELL_HASH_OUTBREAK
		)
		return; 
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_87023( ProcHandlerContextShare *context )
{
	//we need a dmg that will kill us
	if( context->in_dmg - context->in_abs < (int32)context->in_Caller->GetUInt32Value( UNIT_FIELD_HEALTH ) )
		return;
	context->out_dmg_absorb += context->in_dmg + 1;	//absorb all
	//heal ourself to X% of max health
	int32 heal_ammt = context->in_Caller->GetUInt32Value( UNIT_FIELD_MAXHEALTH ) * context->in_OwnerSpell->eff[1].EffectBasePoints / 100;
	heal_ammt -= context->in_Caller->GetInt32Value( UNIT_FIELD_HEALTH );
	if( heal_ammt <= 0 )
		heal_ammt = 1;
	context->out_dmg_overwrite[1] = heal_ammt;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_89007( ProcHandlerContextShare *context )
{
	int32 req_dmg_to_proc = context->in_OwnerSpell->eff[1].EffectBasePoints * (int32)context->in_Caller->GetUInt32Value( UNIT_FIELD_MAXHEALTH ) / 100;
	if( context->in_dmg >= req_dmg_to_proc || ( context->in_CastingSpell && context->in_CastingSpell->NameHash == SPELL_HASH_SHADOW_WORD__DEATH ) )
	{
		context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints+1;
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	}
}

void PH_93400( ProcHandlerContextShare *context )
{
	//this is not normal, but people reported it :S If you are already casting starsurge then the proc is wasted
	if( context->in_Caller->GetCurrentSpell() && context->in_Caller->GetCurrentSpell()->GetProto()->NameHash == SPELL_HASH_STARSURGE )
		return;
	//also clear the cooldown on starsurge
	if( context->in_Caller->IsPlayer() )
		SafePlayerCast( context->in_Caller )->ClearCooldownForSpell( 78674 );
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_81162( ProcHandlerContextShare *context )
{
	//When a damaging attack brings you below 30% of your maximum health
	if( context->in_Caller->IsPlayer() == false || context->in_Caller->GetHealthPct() < 30 )
		return;
	if( ( context->in_dmg - context->in_abs ) <= 0 )
		return;
	int32 new_hp = (int32)context->in_Caller->GetUInt32Value( UNIT_FIELD_HEALTH ) - ( context->in_dmg - context->in_abs );
	int32 new_hp_pct = new_hp * 100 / context->in_Caller->GetUInt32Value( UNIT_FIELD_MAXHEALTH );
	if( new_hp_pct > 30 )
		return;
//	SafePlayerCast( context->in_Caller )->ActivateRuneType( RUNE_BLOOD );	//refresh a rune
	SafePlayerCast( context->in_Caller )->ClearCooldownForSpell( 48982 );	//rune tap
	context->in_Caller->CastSpell( context->in_Caller, 96171, true );		//make rune tap free
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_86662( ProcHandlerContextShare *context )
{
	//if there is nothing to interrupt then we can ignore triggering this
//	if( context->in_Victim->isCasting() == false )
//		return;
	//the above code will not work since we are using a post cast event ( spell cast already got interrupted )
/*	for(uint32 i=0;i<SCHOOL_COUNT;i++)
		if( context->in_Victim->SchoolCastPrevent[i] > getMSTime() )
		{
			context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
			return;
		} */
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_30069( ProcHandlerContextShare *context )
{
	//melee atacks have a 20 chance to restore rage
	if( ( context->in_events_filter_flags & PROC_ON_MELEE_ATTACK_EVENT ) && context->in_CastingSpell == NULL )
	{
		if(  RandChance( 20 ) )
			context->in_Caller->CastSpell( context->in_Caller, 92576, true );	//give rage;
	}
	else if( context->in_CastingSpell && ( context->in_CastingSpell->NameHash == SPELL_HASH_REND || context->in_CastingSpell->NameHash == SPELL_HASH_DEEP_WOUND ) )
		//if we got here that is because we are aplying a bleed effect
	{
		context->in_Caller->CastSpell( context->in_Victim, 46856, true );	//mod bleed damage;
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;		//also mod physical dmg taken
	}
}

void PH_30070( ProcHandlerContextShare *context )
{
	//melee atacks have a 20 chance to restore rage
	if( ( context->in_events_filter_flags & PROC_ON_MELEE_ATTACK_EVENT ) && context->in_CastingSpell == NULL )
	{
		if(  RandChance( 40 ) )
			context->in_Caller->CastSpell( context->in_Caller, 92576, true );	//give rage;
	}
	else if( context->in_CastingSpell && ( context->in_CastingSpell->NameHash == SPELL_HASH_REND || context->in_CastingSpell->NameHash == SPELL_HASH_DEEP_WOUNDS ) )
		//if we got here that is because we are aplying a bleed effect
	{
		context->in_Caller->CastSpell( context->in_Victim, 46857, true );	//mod bleed damage;
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;		//also mod physical dmg taken
	}
}

void PH_85386( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false || context->in_Caller->GetHealthPct() < 20 )
		return;
	if( ( context->in_dmg - context->in_abs ) <= 0 )
		return;
	int32 new_hp = (int32)context->in_Caller->GetUInt32Value( UNIT_FIELD_HEALTH ) - ( context->in_dmg - context->in_abs );
	int32 new_hp_pct = new_hp * 100 / context->in_Caller->GetUInt32Value( UNIT_FIELD_MAXHEALTH );
	if( new_hp_pct > 20 )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_84614( ProcHandlerContextShare *context )
{
	//cast on everyone rend
	InRangeSetRecProt::iterator itr;
	bool found_rend = false;
	context->in_Caller->AquireInrangeLock();
	InrangeLoopExitAutoCallback AutoLock;
	for( itr = context->in_Caller->GetInRangeSetBegin( AutoLock ); itr != context->in_Caller->GetInRangeSetEnd(); itr++ )
	{
		if( !(*itr)->IsUnit() )
			continue;

		if( SafeUnitCast(*itr)->HasAuraWithNameHash( SPELL_HASH_REND, 0, AURA_SEARCH_NEGATIVE ) )
		{
			found_rend = true;
			break;
		}
	}
	context->in_Caller->ReleaseInrangeLock();
	if( found_rend == true )
	{
		context->in_Caller->AquireInrangeLock();
		for( itr = context->in_Caller->GetInRangeSetBegin( AutoLock ); itr != context->in_Caller->GetInRangeSetEnd(); itr++ )
		{
			if( !(*itr)->IsUnit() || !SafeUnitCast((*itr))->isAlive())
				continue;

			//we target stuff that has no full health. No idea if we must fill target list or not :(
			if( (*itr)->isInRange( context->in_Caller, 20.0f ) == false )
				continue;

			if( isAttackable(context->in_Caller,(*itr)) )
				context->in_Caller->CastSpell( SafeUnitCast(*itr) ,772 , true );	//REND
		}
		context->in_Caller->ReleaseInrangeLock();
	}
	//this is a self proc, do not continue execution !
}

void PH_38412( ProcHandlerContextShare *context )
{
	//only in shadow form
	if( context->in_Caller->IsPlayer() == false || SafePlayerCast( context->in_Caller )->GetShapeShift() != FORM_SHADOW )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_76691( ProcHandlerContextShare *context )
{
	if( ( context->in_dmg - context->in_abs ) <= 0 )
		return;
	//As of Patch 4.3.2 Vengeance is no longer triggered by receiving damage from other players.
	if( context->in_Victim->IsPlayer() ) //caller is the one getting the hit and Victim is attacker
		return;
	//Each time you take damage, you gain 5% of the damage taken as attack power, up to a maximum of 10% of your health.
	int32 max_AP_gain = ( context->in_dmg - context->in_abs ) * context->in_OwnerSpell->eff[0].EffectBasePoints / 100;
	int32 hp_5_pct = context->in_Caller->GetUInt32Value( UNIT_FIELD_BASE_HEALTH ) + context->in_Caller->GetUInt32Value( UNIT_FIELD_STAMINA ) * 14;
	hp_5_pct = hp_5_pct * 10 / 100;
	if( hp_5_pct < max_AP_gain )
		max_AP_gain = hp_5_pct;
	if( max_AP_gain <= 0 )
		return;
	context->out_dmg_overwrite[ 0 ] = context->out_dmg_overwrite[ 1 ] = context->out_dmg_overwrite[ 2 ] = max_AP_gain;
	//make them stack, cheat a bit we have 5 auras with same name
	Aura *smallest = NULL;
	for(uint32 i=0;i<5;i++)
	{
		Aura *a = context->in_Caller->HasAura( 76827 + i );
		if( a == NULL )
		{
			context->out_CastSpell = dbcSpell.LookupEntryForced( 76827 + i );
			context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
			return;
		}
		else if( smallest == NULL || a->m_modList[0].m_amount < smallest->m_modList[0].m_amount )
		{
			smallest = a;
		}
	}
	if( smallest && smallest->m_modList[0].m_amount < context->out_dmg_overwrite[ 0 ] )
	{
		context->out_CastSpell = smallest->GetSpellProto();
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	}
}

void PH_47930( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_FLASH_HEAL &&
		context->in_CastingSpell->NameHash != SPELL_HASH_GREATER_HEAL &&
		context->in_CastingSpell->NameHash != SPELL_HASH_PENANCE &&
		context->in_CastingSpell->NameHash != SPELL_HASH_HEAL
		)
		return; 
	//only proc on friendly target (pennance will target enemy also)
	if( isAttackable( context->in_Victim, context->in_Caller, false ) )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_96266( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash == SPELL_HASH_HEAL
		|| context->in_CastingSpell->NameHash == SPELL_HASH_GREATER_HEAL
		|| context->in_CastingSpell->NameHash == SPELL_HASH_FLASH_HEAL )
	{

		Aura *a=context->in_Victim->HasAuraWithNameHash( SPELL_HASH_WEAKENED_SOUL, 0, AURA_SEARCH_NEGATIVE );
		if( a )
		{
			int32 dur = a->GetTimeLeft();
			int32 new_dur = dur - ( context->in_OwnerSpell->eff[0].EffectBasePoints * 1000 );
			if( new_dur <= 500 )
				a->Remove();
			else
			{
				a->SetDuration( new_dur );
				a->ResetDuration();
			}
		}
	}
	else if( context->in_CastingSpell->NameHash == SPELL_HASH_INNER_FOCUS )
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_81661( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	//not smite ? 
	if( context->in_CastingSpell->NameHash == SPELL_HASH_MIND_FLAY )
		context->out_CastSpell = dbcSpell.LookupEntryForced( context->in_OwnerSpell->eff[1].EffectTriggerSpell );

	//enable cast of Archangel
//	uint32 counts = context->in_Caller->CountAura( context->out_CastSpell->Id, AURA_SEARCH_POSITIVE );
//	if( counts >= 4 )
		context->in_Caller->CastSpell( context->in_Caller, 94709, true ); //enable it even with 1 stack

	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_83489( ProcHandlerContextShare *context )
{
	//only if target is below X% hp
	if( (int32)context->in_Victim->GetHealthPct() > context->in_OwnerSpell->eff[1].EffectBasePoints )
		return;
	if( context->in_CastingSpell && context->in_CastingSpell->NameHash == SPELL_HASH_STEADY_SHOT )
		context->in_Caller->Energize( context->in_Caller, 77443, context->in_OwnerSpell->eff[0].EffectBasePoints, POWER_TYPE_FOCUS, 0 );
	else
		context->in_Caller->Energize( context->in_Caller, 91954, context->in_OwnerSpell->eff[0].EffectBasePoints, POWER_TYPE_FOCUS, 0 );
	//!!self proc, do not continue
}

void PH_82925( ProcHandlerContextShare *context )
{
//	Aura *a = context->in_Caller->HasAuraWithNameHash( SPELL_HASH_READY__SET__AIM___, 0, AURA_SEARCH_POSITIVE );
//	if( a && a->GetStackCount() >= a->GetSpellProto()->maxstack - 1 )
	uint32 count = context->in_Caller->CountAura( 82925, AURA_SEARCH_POSITIVE );
	if( count >= 4 )
	{
		context->out_CastSpell = dbcSpell.LookupEntryForced( 82926 );
		context->in_Caller->RemoveAuraByNameHash( SPELL_HASH_READY__SET__AIM___, 0, AURA_SEARCH_ALL, MAX_PASSIVE_AURAS );
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_53220( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
/*	if( context->in_CastingSpell->NameHash != SPELL_HASH_STEADY_SHOT )
	{
		context->in_event->created_with_value = 0;
		return;
	}
	context->in_event->created_with_value++;
	//proc on second steady shot cast
	if( context->in_event->created_with_value < 2 )
		return;
	*/
	if( context->in_CastingSpell->NameHash != SPELL_HASH_STEADY_SHOT )
		return;
	if( context->in_event->created_with_value > 2 )
		context->in_event->created_with_value = 0;
	context->in_event->created_with_value++;
	if( context->in_event->created_with_value == 2 )
	{
		context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints;
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	}
}

void PH_83559( ProcHandlerContextShare *context )
{
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[1].EffectBasePoints;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_85759( ProcHandlerContextShare *context )
{
	if( context->in_Caller->GetMapMgr() == NULL )
		return;
	//when we proc we will cast this same spell on the "other" target also
	uint64 other_target_guid = context->in_event->created_with_value;
	other_target_guid = other_target_guid << 32;
	other_target_guid |= (uint32)context->in_event->custom_holder;
	Unit *other_target = context->in_Caller->GetMapMgr()->GetUnit( (uint64)other_target_guid );
	if( other_target == NULL )
		other_target = context->in_Caller->GetMapMgr()->GetPet( other_target_guid );
	if( other_target == NULL || other_target->IsUnit() == false || other_target->IsInWorld() == false )
		return;
//	other_target->CastSpell( other_target, context->out_CastSpell, true );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init( other_target, context->out_CastSpell ,true, NULL);
	SpellCastTargets targets2( other_target->GetGUID() );	
	spell->pSpellId = context->in_event->spellId;
	if( other_target->GetGUID() != context->in_event->caster )
	{
		spell->forced_basepoints[0] = context->in_OwnerSpell->eff[0].EffectBasePoints / 3;
		spell->forced_basepoints[0] = spell->forced_basepoints[1];
	}
	spell->prepare( &targets2 );

	//do not proc it again on target !
}

void PH_94286( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL 
		|| (context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_HEALING ) == 0
		|| context->in_Caller == context->in_Victim )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_87426( ProcHandlerContextShare *context )
{
	//proc chance is calculated dynamically
	float chance = (float)(context->in_OwnerSpell->eff[0].EffectBasePoints+1);
	if( context->in_Caller->IsPlayer() && SafePlayerCast( context->in_Caller )->last_moved + 2000 > getMSTime() )
		chance *= 5;
	if( RandChance( chance ) == false )
		return;

	//count the number of shadow apparitions we have atm. Do not create more then 4
	std::list<uint64>::iterator gitr;
	uint32 apparition_count = 0;
	for( gitr = context->in_Caller->m_guardians.begin(); gitr != context->in_Caller->m_guardians.end(); gitr++ )
	{
		Creature *tc = context->in_Caller->GetMapMgr()->GetCreature( (*gitr) );
		if( tc && tc->GetEntry() == 46954 )
			apparition_count++;
	}
	if( apparition_count >= 4 )
		return;

	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_81751( ProcHandlerContextShare *context )
{
 	if( context->in_Caller->IsPlayer() == false )
		return; 
	uint64 new_target = Spell::FindLowestFriendly( SafePlayerCast( context->in_Caller ), 40*40, context->in_Victim->GetPositionX(), context->in_Victim->GetPositionY() ); // within 40 yards
	if( new_target == 0 )	// shouldn't happen
		return;
	if( ( context->in_dmg - context->in_abs ) <= 0 )
		return;
	SpellCastTargets targets2( new_target ); //no target so spelltargeting will get an injured party member
	SpellEntry *spellInfo = dbcSpell.LookupEntry( context->in_event->spellId );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init(context->in_Caller, spellInfo ,true, NULL);
	spell->forced_basepoints[0] = ( context->in_dmg - context->in_abs ) * context->in_OwnerSpell->eff[0].EffectBasePoints / 100; 

	//priest is only healed half :(. Value is scripted also to not get double boosted by SP
	if( new_target == context->in_Caller->GetGUID() )
		spell->forced_basepoints[0] = spell->forced_basepoints[0] / 2;

	spell->ProcedOnSpell = context->in_CastingSpell;
	spell->pSpellId = context->in_OwnerSpell->Id;
	spell->prepare(&targets2);
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
	//no need to continue trigger execution by default way
}

void PH_91320( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL 
		|| (context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_HEALING ) == 0 )
		return;
	if( context->in_Caller->HasAuraWithNameHash( SPELL_HASH_BLIND_SPOT, 0, AURA_SEARCH_NEGATIVE ) )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_91141( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL 
		|| (context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_HEALING ) == 0 )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_88688( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_SMITE
		&& context->in_CastingSpell->NameHash != SPELL_HASH_HEAL
		&& context->in_CastingSpell->NameHash != SPELL_HASH_FLASH_HEAL
		&& context->in_CastingSpell->NameHash != SPELL_HASH_GREATER_HEAL
		)
	{
		return;
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_14893( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_BINDING_HEAL
		&& context->in_CastingSpell->NameHash != SPELL_HASH_HEAL
		&& context->in_CastingSpell->NameHash != SPELL_HASH_FLASH_HEAL
		&& context->in_CastingSpell->NameHash != SPELL_HASH_GREATER_HEAL
		&& context->in_CastingSpell->NameHash != SPELL_HASH_PENANCE
		&& context->in_CastingSpell->NameHash != SPELL_HASH_PRAYER_OF_MENDING
		&& context->in_CastingSpell->NameHash != SPELL_HASH_PRAYER_OF_HEALING
		&& context->in_CastingSpell->NameHash != SPELL_HASH_CIRCLE_OF_HEALING
		)
	{
		return;
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_14751( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash == SPELL_HASH_BINDING_HEAL
		|| context->in_CastingSpell->NameHash == SPELL_HASH_HEAL
		|| context->in_CastingSpell->NameHash == SPELL_HASH_FLASH_HEAL
		|| context->in_CastingSpell->NameHash == SPELL_HASH_GREATER_HEAL
		)
	{
		context->in_Caller->CastSpell( context->in_Caller, 81208, true ); // Chakra: Serenity
	}
	else if( context->in_CastingSpell->NameHash == SPELL_HASH_PRAYER_OF_HEALING
		|| context->in_CastingSpell->NameHash == SPELL_HASH_PRAYER_OF_MENDING
		)
	{
		context->in_Caller->CastSpell( context->in_Caller, 81206, true ); // Chakra: Sanctuary
//		context->in_Caller->CastSpell( context->in_Caller, 81207, true ); // Chakra: Sanctuary
	}
	else if( context->in_CastingSpell->NameHash == SPELL_HASH_SMITE
		|| context->in_CastingSpell->NameHash == SPELL_HASH_MIND_SPIKE
		)
	{
		context->in_Caller->CastSpell( context->in_Caller, 81209, true ); // Chakra: Chastise
	}
	//self proc, do not continue !
}

void PH_81585( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell || !( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_HEALING ) )
		return;
	//we need direct heal spell = no amplitude presence
	if( context->in_CastingSpell->quick_tickcount > 1 )
		return;
	//find a renew on the target and try to refresh the duration
	Aura *a = context->in_Victim->HasAuraWithNameHash( SPELL_HASH_RENEW, 0, AURA_SEARCH_POSITIVE );
	if( a )
		a->ResetDuration();
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_93068( ProcHandlerContextShare *context )
{
	if( context->in_Victim->IsPoisoned() == 0 )
		return;
/*	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_INSTANT_POISON
		&& context->in_CastingSpell->NameHash != SPELL_HASH_WOUND_POISON
		&& context->in_CastingSpell->NameHash != SPELL_HASH_MIND_NUMBING_POISON
		&& context->in_CastingSpell->NameHash != SPELL_HASH_CRIPPLING_POISON
		&& context->in_CastingSpell->NameHash != SPELL_HASH_DEADLY_POISON
//		&& context->in_CastingSpell->NameHash != SPELL_HASH_ANESTHETIC_POISON
		)
		return; */
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_79136( ProcHandlerContextShare *context )
{
	if( context->in_Victim->IsPoisoned() == 0 )
		return;
	uint32 energy_amt = context->in_OwnerSpell->eff[1].EffectBasePoints;
	if( context->in_CastingSpell 
		&& context->in_CastingSpell->NameHash == SPELL_HASH_RUPTURE 
		&& ( context->in_dmg - context->in_abs ) > (int32)context->in_Victim->GetHealth() )
		energy_amt += 6;
	context->in_Caller->Energize( context->in_Caller, context->out_CastSpell->Id, energy_amt, POWER_TYPE_ENERGY, 0 );
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_58683( ProcHandlerContextShare *context )
{
	if( context->in_Victim->IsPoisoned() == 0 )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}
/*
void PH_35542( ProcHandlerContextShare *context )
{
 	if( context->in_Caller->IsPlayer() == false )
		return; 
	if(context->in_dmg )
		return;
	//this needs offhand weapon
	Item* it = SafePlayerCast( context->in_Caller )->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_OFFHAND );
	if( it == NULL || it->GetProto()->InventoryType != INVTYPE_WEAPON )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}*/
void PH_35542( ProcHandlerContextShare *context )
{
	if( ( context->in_events_filter_flags & PROC_OFFHAND_WEAPON ) == 0 
		&& ( context->in_CastingSpell == NULL || context->in_CastingSpell->NameHash != SPELL_HASH_MAIN_GAUCHE )
		)
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_91021( ProcHandlerContextShare *context )
{
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_89765( ProcHandlerContextShare *context )
{
//	if( context->in_CastingSpell == NULL )
//		return;
//	context->in_Caller->Energize( context->in_Caller, context->in_OwnerSpell->Id, context->in_OwnerSpell->eff[0].EffectBasePoints, POWER_TYPE_ENERGY );
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_92295( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell != NULL && context->in_Caller->IsPlayer() )
	{
		if( context->in_CastingSpell->NameHash == SPELL_HASH_GREATER_HEAL )
		{
			//Inner Focus
			SafePlayerCast( context->in_Caller )->ModCooldown( 89485, -context->in_OwnerSpell->eff[0].EffectBasePoints * 1000, false );
		}
		else if( context->in_CastingSpell->NameHash == SPELL_HASH_SMITE )
		{
			//penance
			SafePlayerCast( context->in_Caller )->ModCooldown( 47540, -context->in_OwnerSpell->eff[1].EffectBasePoints, false );
		}
	}
	//!! self proc
}
/*
void PH_14747( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() )
	{
		int32 mod;
		if( context->in_events_filter_flags & PROC_ON_CAST_SPELL )
			mod = -context->in_OwnerSpell->eff[0].EffectBasePoints;
		else
			mod = context->in_OwnerSpell->eff[0].EffectBasePoints;
		for(uint32 x=1;x<SCHOOL_COUNT;x++)
//			context->in_Caller->DamageTakenPctMod[x] += mod/100.0f;
	}
	//!! self proc
}*/

void PH_47569( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return; //hmm, how on earth did they use this talent ?
	context->in_Caller->RemoveAurasMovementImpairing();
	context->in_Caller->RemoveAllAurasByMechanic( MECHANIC_STUNNED, -1, false );
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
	//!! self proc
}

void PH_82368( ProcHandlerContextShare *context )
{
	if( context->in_Victim->GetHealthPct() > 20 )
		return;
	context->out_dmg_overwrite[1] = context->in_OwnerSpell->eff[0].EffectBasePoints;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_89775( ProcHandlerContextShare *context )
{
	if( ( context->in_dmg - context->in_abs ) <= 0 )
		return;
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints * ( context->in_dmg - context->in_abs ) / 100;
	context->out_dmg_overwrite[0] = context->out_dmg_overwrite[0] / context->out_CastSpell->quick_tickcount;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_88765( ProcHandlerContextShare *context )
{
	//we need to have lighting shield on us
	Aura *a = context->in_Caller->HasAuraWithNameHash( SPELL_HASH_LIGHTNING_SHIELD, 0, AURA_SEARCH_POSITIVE );
	if( a == NULL )
		return;
	int32 charges_now = context->in_Caller->CountAuraNameHash( SPELL_HASH_LIGHTNING_SHIELD, AURA_SEARCH_POSITIVE );
	if( charges_now < context->in_OwnerSpell->eff[0].EffectBasePoints )
	{
		Aura *aur = AuraPool.PooledNew( __FILE__, __LINE__ );
		aur->Init( a->GetSpellProto(),a->GetDuration(),a->GetCaster(),a->GetTarget(), NULL );
		//curses the hacks !. this spell can overstack. but not the original spell, that should stack top 3
		//but but this is the original spell :(
		if( context->in_Caller->IsPlayer() )
			SafePlayerCast( context->in_Caller )->AuraStackCheat = true;
		context->in_Caller->AddAura( aur );
		context->in_Caller->m_chargeSpells[ a->GetSpellId() ].count++;	//also add a charge
		if( context->in_Caller->IsPlayer() )
			SafePlayerCast( context->in_Caller )->AuraStackCheat = false;
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_88767( ProcHandlerContextShare *context )
{
	//we need to have lighting shield on us
	int32 charges_now = context->in_Caller->CountAuraNameHash( SPELL_HASH_LIGHTNING_SHIELD, AURA_SEARCH_POSITIVE );
	int32 keep_charges = context->in_OwnerSpell->eff[0].EffectBasePoints;
	if( charges_now <= keep_charges )
		return;
	int32 charges_eaten = charges_now - keep_charges;
	//the excess charges are eaten up and dmg is produced from them
	context->in_Caller->RemoveAuraByNameHash( SPELL_HASH_LIGHTNING_SHIELD, 0, AURA_SEARCH_POSITIVE, charges_eaten );
	//this is the overlay
	context->in_Caller->RemoveAuraByNameHash( SPELL_HASH_FULMINATION_, 0, AURA_SEARCH_POSITIVE, 1 );

	//for later : 26364 -> lightning shield dmg spell id
	SpellCastTargets targets( context->in_Victim->GetGUID() );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init( context->in_Caller, context->out_CastSpell ,true, NULL);
//	spell->forced_pct_mod[0] = 100 + charges_eaten * 33; //with 6 extra charges 6*33 = 132% dmg boost
	spell->forced_pct_mod[0] = 100 * charges_eaten; //with 6 extra charges 6*33 = 132% dmg boost
	spell->ProcedOnSpell = context->in_CastingSpell;
	spell->pSpellId=context->in_OwnerSpell->Id;
	spell->prepare( &targets );
	//no continue, we just casted it ourself
}

void PH_77747( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	//check the newly created fire totem
	Creature *fire_totem = SafePlayerCast( context->in_Caller )->m_TotemSlots[EFF_TARGET_TOTEM_FIRE - EFF_TARGET_TOTEM_EARTH];
	if( fire_totem == NULL )
		return;	//wtf failed totem cast ?

	SpellCastTargets targets( fire_totem->GetGUID() );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init( fire_totem, context->out_CastSpell ,true, NULL);
	spell->prepare( &targets );
	//no continue, we just casted it ourself
}

void PH_26364( ProcHandlerContextShare *context )
{
	//do not proc on DOTS
	if( context->in_CastingSpell && context->in_CastingSpell->quick_tickcount > 1 )
		return;
	//we need to have lighting shield on us
	Aura *a = context->in_Caller->HasAuraWithNameHash( SPELL_HASH_LIGHTNING_SHIELD, 0, AURA_SEARCH_POSITIVE );
	if( a == NULL )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_31589( ProcHandlerContextShare *context )
{
	//casting spell is targeting both owner and victim
	if( context->in_Victim == context->in_Caller )
		return;
	//do not refresh slow, only add it
	Aura *has = context->in_Victim->HasAuraWithNameHash( SPELL_HASH_SLOW, 0, AURA_SEARCH_NEGATIVE );
	if( has != NULL )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_79132( ProcHandlerContextShare *context )
{
	//casting spell is targeting both owner and victim
	if( (int32)context->in_Victim->GetHealthPct() > context->in_OwnerSpell->eff[1].EffectBasePoints )
		return;
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_51664( ProcHandlerContextShare *context )
{
	Aura *a = context->in_Caller->HasAuraWithNameHash( SPELL_HASH_SLICE_AND_DICE, 0, AURA_SEARCH_POSITIVE );
	if( a )
	{
		a->SetDuration( 23000 );	//max with 5 combo points...hmmmm
		a->ResetDuration();
	}
	//!! self proc
}

void PH_84748( ProcHandlerContextShare *context )
{
	//simulate diminishing return
	if( context->in_event->custom_holder != context->in_Victim )
	{
		context->in_event->custom_holder = context->in_Victim;
		context->in_event->created_with_value = 0;
	}
	//no need to refresh it
//	if( context->in_Victim->HasAura( context->out_CastSpell->Id ) )
//		return;
	//counter for the dimininishing
	context->in_event->created_with_value++;

#define STACKS_UNTIL_STAGE_1	4
#define STAGE_1_SPELL			84745		//Shallow Insight
#define STACKS_UNTIL_STAGE_2	8
#define STAGE_2_SPELL			84746		//Moderate Insight
#define STACKS_UNTIL_STAGE_3	12
#define STAGE_3_SPELL			84747		//Deep Insight
#define STACKS_UNTIL_FINAL_STAGE	15		//worst case this is 25 seconds, best case it is 25*15 seconds = 4 minutes ?

	//there is no point to reduce duration even more then 5 seconds 
	if( context->in_event->created_with_value > STACKS_UNTIL_FINAL_STAGE )
	{
		context->in_event->created_with_value = 1;
//		return;
	}

	//cast visual part
	uint32 pct_mod;
	if( context->in_event->created_with_value > STACKS_UNTIL_STAGE_2 )
	{
		context->in_Caller->CastSpell( context->in_Victim, STAGE_3_SPELL, true );	//Deep Insight
		if( context->in_event->created_with_value == STACKS_UNTIL_STAGE_2 + 1 )
			context->in_Caller->RemoveAura( STAGE_2_SPELL ); // Moderate Insight
		pct_mod = 30;
	}
	else if( context->in_event->created_with_value > STACKS_UNTIL_STAGE_1 )
	{
		context->in_Caller->CastSpell( context->in_Victim, STAGE_2_SPELL, true );	//Moderate Insight
		if( context->in_event->created_with_value == STACKS_UNTIL_STAGE_1 + 1 )
			context->in_Caller->RemoveAura( STAGE_1_SPELL ); // Shallow Insight
		pct_mod = 20;
	}
	else 
	{
		if( context->in_event->created_with_value == 1 )
			context->in_Caller->RemoveAura( STAGE_3_SPELL ); // Shallow Insight
		context->in_Caller->CastSpell( context->in_Victim, STAGE_1_SPELL, true );
		pct_mod = 10;
	}
	//now cast the scaled spell that will actually mod the dmg on target
	SpellCastTargets targets( context->in_Victim->GetGUID() );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init( context->in_Caller, context->out_CastSpell ,true, NULL);
//	spell->forced_duration[0] = 5 + 10 / ( context->in_event->created_with_value / 2 );
//	spell->forced_duration[1] = spell->forced_duration[0];
//	spell->forced_basepoints[0] = MIN( context->in_event->created_with_value * 7, context->in_OwnerSpell->eff[0].EffectBasePoints );
	spell->forced_basepoints[0] = pct_mod;
	spell->forced_basepoints[1] = spell->forced_basepoints[0];
	spell->ProcedOnSpell = context->in_CastingSpell;
	spell->pSpellId=context->in_OwnerSpell->Id;
	spell->prepare( &targets );
	//no continue, we just casted it ourself
}

void PH_14171( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL || context->in_Caller->IsPlayer() == false )
		return;
	uint32 chance = SafePlayerCast( context->in_Caller )->m_comboPoints * context->in_OwnerSpell->eff[0].EffectBasePoints;
	if( RandomUInt() % 100 > chance )
		return;
	Aura *a = context->in_Victim->HasAuraWithNameHash( SPELL_HASH_RUPTURE, 0, AURA_SEARCH_NEGATIVE );
	if( a )
		a->ResetDuration();
	//!! self proc
}

void PH_84590( ProcHandlerContextShare *context )
{
	Aura *a = context->in_Caller->HasAuraWithNameHash( SPELL_HASH_SLICE_AND_DICE, 0, AURA_SEARCH_POSITIVE );
	if( a )
		a->ResetDuration();
	a = context->in_Caller->HasAuraWithNameHash( SPELL_HASH_RECUPERATE, 0, AURA_SEARCH_POSITIVE );
	if( a )
		a->ResetDuration();
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_91024( ProcHandlerContextShare *context )
{
	if( context->in_Victim == context->in_Caller || isAttackable( context->in_Victim, context->in_Caller ) == false )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_92233( ProcHandlerContextShare *context )
{
	int32 health_pct_before = context->in_Caller->GetHealthPct();
	int32 health_after = context->in_Caller->GetHealth() - ( context->in_dmg - context->in_abs );
	int32 health_pct_after = health_after * 100 / context->in_Caller->GetMaxHealth();
	if( health_pct_before >= context->in_OwnerSpell->eff[0].EffectBasePoints 
		&& health_pct_after <= context->in_OwnerSpell->eff[0].EffectBasePoints
		)
	{
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	}
}

void PH_52752( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;

	if( ( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_HEALING ) == 0 )
		return;

	if( ( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_HEAL_OVER_TIME ) != 0 )
		return;

	if( context->in_CastingSpell->GetMaxTargets() > 1 && context->in_CastingSpell->GetMaxTargets() != NO_MAX_TARGETS_DEFINED )
		return;

	if( ( context->in_dmg - context->in_abs ) <= 0 )
		return;

	uint64 new_target = Spell::FindLowestHealthRaidMember( SafePlayerCast( context->in_Caller ), 40*40); // within 40 yards
	if( new_target == 0 )	// shouldn't happen
		return;
	SpellCastTargets targets2( new_target ); //no target so spelltargeting will get an injured party member
	SpellEntry *spellInfo = dbcSpell.LookupEntry( context->in_event->spellId );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init(context->in_Caller, spellInfo ,true, NULL);
	spell->forced_basepoints[0] = ( context->in_dmg - context->in_abs ) * (context->in_OwnerSpell->eff[0].EffectBasePoints + 1) / 100; 
	spell->ProcedOnSpell = context->in_CastingSpell;
	spell->pSpellId = context->in_OwnerSpell->Id;
	spell->prepare(&targets2);
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_82987( ProcHandlerContextShare *context )
{
	if( ( context->in_dmg - context->in_abs ) <= 0 )
		return;
	context->out_dmg_overwrite[0] = ( context->in_dmg - context->in_abs ) * context->in_OwnerSpell->eff[0].EffectBasePoints / 100;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_53390( ProcHandlerContextShare *context )
{
	context->out_dmg_overwrite[0] = -context->in_OwnerSpell->eff[0].EffectBasePoints;
	context->out_dmg_overwrite[1] = context->in_OwnerSpell->eff[0].EffectBasePoints;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_77800( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return; //wtf ?
	SpellPowerEntry *spe = dbcSpellPower.LookupEntryForced( context->in_CastingSpell->SpellPowerId );
	if( spe )
	{
		int32 mana_cost = spe->manaCost + (context->in_Caller->GetUInt32Value(UNIT_FIELD_BASE_MANA) * spe->ManaCostPercentage)/100; 
		int32 reduction = mana_cost * context->in_OwnerSpell->eff[0].EffectBasePoints / 100;
		context->out_dmg_overwrite[0] = -reduction;
	}
	context->out_dmg_overwrite[1] = context->in_OwnerSpell->eff[1].EffectBasePoints;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_83098( ProcHandlerContextShare *context )
{
	context->out_dmg_overwrite[0] = context->in_Caller->GetPower(  POWER_TYPE_MANA ) * context->in_OwnerSpell->eff[0].EffectBasePoints / 100;
	context->out_dmg_overwrite[1] = context->out_dmg_overwrite[0];
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_11426( ProcHandlerContextShare *context )
{
	if( context->in_Caller->GetHealthPct() > 50 )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_44544( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	//no targets for this spell ( all immune )
	if( context->in_Caller == context->in_Victim )
		return;
	if( context->in_OwnerSpell->NameHash == SPELL_HASH_IMPROVED_FREEZE )
	{
		//we are a water elemental and we will cast this on our owner
		Unit *u = context->in_Caller->GetTopOwner();
		u->CastSpell( u, 44544, true );
		u->CastSpell( u, 44544, true );
		//no need to continue
	}
	else 
	{
		if( ( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_CHILL_SPELL ) == NULL )
			return;
		//should proc and remove also ?
//		if( context->in_OwnerSpell->NameHash == SPELL_HASH_ICE_LANCE )
//			return;
		if( context->in_CastingSpell->NameHash == SPELL_HASH_CHILLED )	//from frost armor. Since 4.1 no longer procs it
			return;
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	}
}

void PH_19615( ProcHandlerContextShare *context )
{
	//only proc on Bite, Claw, Smack
	if( context->in_CastingSpell == NULL )
		return;
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints;
	context->out_dmg_overwrite[1] = context->out_dmg_overwrite[0];
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_29178( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( ( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_DAMAGING ) == NULL )
		return;
	if( context->in_CastingSpell->quick_tickcount > 1 )
		return;
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_82921( ProcHandlerContextShare *context )
{
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_91394( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;

	//reduce target healing
	if( ( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_CHILL_SPELL ) != NULL )
	{
		SpellCastTargets targets2( context->in_Victim->GetGUID() ); 
		SpellEntry *spellInfo = dbcSpell.LookupEntry( 68391 );
		Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		spell->Init( context->in_Caller, spellInfo ,true, NULL );
		spell->forced_basepoints[0] = context->in_OwnerSpell->eff[1].EffectBasePoints; 
		spell->ProcedOnSpell = context->in_CastingSpell;
		spell->pSpellId = context->in_OwnerSpell->Id;
		spell->prepare(&targets2);
	}

	if( ( context->in_dmg - context->in_abs ) <= 0 )
		return;
	if( ( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_DAMAGING ) == 0 )
		return;
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints * ( context->in_dmg - context->in_abs ) / 100;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_91299( ProcHandlerContextShare *context )
{
	//remove all bad dots from the target
	for(uint32 t=NEG_AURAS_START;t<MAX_NEGATIVE_AURAS1(context->in_Victim);t++)
		if( context->in_Victim->m_auras[ t ] 
			&& context->in_Victim->m_auras[ t ]->GetSpellProto()->quick_tickcount > 1  )
		{
			context->in_Victim->m_auras[ t ]->Remove();
		}

	//no continue for self proc
}

void PH_23694( ProcHandlerContextShare *context )
{
	//when reapplying hamstring
	Aura *a = context->in_Victim->HasAuraWithNameHash( SPELL_HASH_HAMSTRING, 0, AURA_SEARCH_NEGATIVE );
	if( a == NULL )
		return;
	//only proc on reaply, which should be less then 15 from previous function call
	if( context->in_event->created_with_value > (int32)getMSTime() )
	{
		return;
	}
	context->in_event->created_with_value = getMSTime() + context->in_OwnerSpell->eff[0].EffectBasePoints;
	//how the heck can we detect if we are reapplying ? This is a post cast event not a precast event :(
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_33876( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;

	if( context->in_CastingSpell->Id == 16979 ) //Feral Charge - bear
		context->in_Caller->CastSpell( context->in_Caller, context->in_OwnerSpell->eff[1].EffectTriggerSpell, true );
	else if( context->in_CastingSpell->Id == 49376 ) //Feral Charge - cat
		context->in_Caller->CastSpell( context->in_Caller, context->in_OwnerSpell->eff[2].EffectTriggerSpell, true );
	//custom handled, no continue
}

void PH_57893( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	if( SafePlayerCast( context->in_Caller )->GetShapeShift() != FORM_BEAR )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_85416( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	//only proc for main target, no aoe
	if( context->in_Victim->GetGUID() != SafePlayerCast(context->in_Caller)->GetSelection() )
		return;
//	Aura *a = context->in_Victim->HasAuraWithNameHash( SPELL_HASH_AVENGER_S_SHIELD, 0, AURA_SEARCH_NEGATIVE );
//	if( a )
//		a->ResetDuration();
	SafePlayerCast(context->in_Caller)->ClearCooldownForSpell( 31935 );	//Avenger's Shield
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;	//this is for the visuals
}

void PH_16858( ProcHandlerContextShare *context )
{
	//3 stacks - we just casted 1 so 2 casts are enough ?
	context->in_Caller->CastSpell( context->in_Victim, 91565, true ); //Faerie Fire
	context->in_Caller->CastSpell( context->in_Victim, 91565, true ); //Faerie Fire
//	context->in_Caller->CastSpell( context->in_Caller, 91565, true ); //Faerie Fire
	//!!self proc
}

void PH_90174( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_JUDGEMENT
		&& context->in_CastingSpell->NameHash != SPELL_HASH_EXORCISM
		&& context->in_CastingSpell->NameHash != SPELL_HASH_TEMPLAR_S_VERDICT
		&& context->in_CastingSpell->NameHash != SPELL_HASH_DIVINE_STORM
		&& context->in_CastingSpell->NameHash != SPELL_HASH_INQUISITION
		&& context->in_CastingSpell->NameHash != SPELL_HASH_HOLY_WRATH
		&& context->in_CastingSpell->NameHash != SPELL_HASH_HAMMER_OF_WRATH
		)
		return; 
	//the spell cast will do the same
//	context->in_Caller->SetPower(  POWER_TYPE_HOLY, context->in_Caller->GetMaxPower(  POWER_TYPE_HOLY ) );
	context->out_dmg_overwrite[0] = context->in_Caller->GetPower( POWER_TYPE_HOLY );
//	context->in_Caller->CastSpell( context->in_Caller, 90174, true );	//visual

	context->in_Caller->Energize( context->in_Caller, 88675, context->in_Caller->GetMaxPower( POWER_TYPE_HOLY ), POWER_TYPE_HOLY, 0 );
	
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_88063( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_dmg <= 0 )
		return;
	if( context->in_CastingSpell->NameHash == SPELL_HASH_WORD_OF_GLORY 
		&& context->in_Caller == context->in_Victim	)
		*context->in_dmg_loc += context->in_dmg * context->in_OwnerSpell->eff[0].EffectBasePoints / 100;
	int32 can_heal = context->in_Victim->GetMaxHealth() - context->in_Victim->GetHealth();
	int32 overheal_amt = *context->in_dmg_loc - can_heal;
	if( overheal_amt <= 0 )
		return;
	context->out_dmg_overwrite[0] = overheal_amt;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_88676( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false ||
		SafePlayerCast( context->in_Caller )->m_currentSpellAny == NULL )
		return;
	Spell *castingspell = SafePlayerCast( context->in_Caller )->m_currentSpellAny;
	if( castingspell->GetProto()->powerType != POWER_TYPE_HOLY )
		return;
	context->out_dmg_overwrite[0] = castingspell->power_cost;
	//hmm, maybe we need to delay the cast ? When is the power taken away for this spell anyway ?
//	uint32 HolyPowerSpent = SafePlayerCast( context->in_Caller )->LastSpellCost;
	context->in_Caller->CastSpellDelayed2( context->in_Caller->GetGUID(), context->out_CastSpell->Id, 1, true, castingspell->power_cost );
//	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_31848( ProcHandlerContextShare *context )
{
	//divine plea
	context->in_Caller->Energize( context->in_Caller, 54428, context->in_OwnerSpell->eff[1].EffectBasePoints, POWER_TYPE_HOLY, 0 );
	//!! self proc, no continue
}

void PH_56819( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	SafePlayerCast(context->in_Caller)->ClearCooldownForSpell( 1766 );	//kick
	SafePlayerCast(context->in_Caller)->ClearCooldownForSpell( 51722 );	//Dismantle
	SafePlayerCast(context->in_Caller)->ClearCooldownForSpell( 76577 );	//Smoke Bomb
	//!! self proc, no continue
}

void PH_80863( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	if( (int32)context->in_Victim->GetHealthPct() > context->in_OwnerSpell->eff[0].EffectBasePoints )
		return;
	Aura *a = context->in_Victim->HasAuraWithNameHash( SPELL_HASH_RIP, 0, AURA_SEARCH_NEGATIVE );
	if( a )
		a->ResetDuration();
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_58879( ProcHandlerContextShare *context )
{
	if( ( context->in_dmg - context->in_abs ) <= 0 )
		return;
	context->out_dmg_overwrite[0] = ( context->in_dmg - context->in_abs ) * ( context->in_OwnerSpell->eff[0].EffectBasePoints) / 100;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_86678( ProcHandlerContextShare *context )
{
	if( ( context->in_dmg - context->in_abs ) <= 0 || context->in_Caller->GetMapMgr() == NULL )
		return;
	if( context->in_event->procCharges <= 0 )
		return;
	context->in_event->procCharges--;
	//we need to find our guardian and force him to cast it
	Unit *u = context->in_Caller->GetMapMgr()->GetUnit( context->in_event->caster );
	if( u && u->isAlive() )
	{
		SpellCastTargets targets( context->in_Victim->GetGUID() );
		SpellEntry *spellInfo = dbcSpell.LookupEntry( context->in_event->spellId );
		Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		spell->Init( u, spellInfo ,true, NULL);
		spell->forced_basepoints[0] = ( context->in_dmg - context->in_abs ) ;
		spell->forced_basepoints[1] = ( context->in_dmg - context->in_abs ) * 10 / 100;
		spell->ProcedOnSpell = context->in_CastingSpell;
		spell->pSpellId=context->in_OwnerSpell->Id;
		spell->prepare(&targets);
	}
}

void PH_17080( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash == SPELL_HASH_TIGER_S_FURY
		|| context->in_CastingSpell->NameHash == SPELL_HASH_BERSERK )
	{
		context->in_Caller->CastSpell( context->in_Caller, context->in_OwnerSpell->eff[1].EffectTriggerSpell, true );
	}
	if( context->in_CastingSpell->NameHash == SPELL_HASH_ENRAGE
		|| context->in_CastingSpell->NameHash == SPELL_HASH_BERSERK )
	{
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	}
}

void PH_51179( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;

	if( ( context->in_events_filter_flags & PROC_ON_CAST_SPELL_EVENT ) )
	{
		Aura *a = context->in_Caller->HasAuraWithNameHash( SPELL_HASH_REJUVENATION );
		if( a && a->m_modList[0].m_AuraName == SPELL_AURA_PERIODIC_HEAL )
		{
			uint32 BaseHeal = a->m_modList[0].m_amount;
			float SummaryModCoeff;
			int32 bonus = context->in_Caller->GetSpellHealBonus( context->in_Victim, context->in_CastingSpell, BaseHeal, 0, &SummaryModCoeff );
			SummaryModCoeff = SummaryModCoeff * ( a->m_modList[0].m_pct_mod / 100.0f );
			int32 heal_amt = float2int32( ( BaseHeal + bonus ) * SummaryModCoeff ); 

			uint32 Duration = a->GetDuration();
			uint32 TickInterval = GetSpellAmplitude( a->GetSpellProto(), context->in_Caller, a->m_modList[0].i, 0 );
			uint32 TickCount = Duration / TickInterval;
			heal_amt = heal_amt * ( context->in_OwnerSpell->eff[1].EffectBasePoints ) * TickCount / 100;
			context->in_Caller->SpellHeal( context->in_Victim, context->in_CastingSpell, heal_amt, false, true );
			return;
		}
	}
/*
	if( context->in_events_filter_flags & PROC_ON_CAST_SPELL_EVENT )
	{
		context->in_event->created_with_value = 1; //mark that we can heal on next event
		return;
	}
	//we already procced on this aura
	if( context->in_event->created_with_value != 1 )
	{
		return;
	}
	context->in_event->created_with_value = 0;
//	uint32 heal_amt = ( context->in_CastingSpell->eff[0].EffectBasePoints ) * ( context->in_OwnerSpell->eff[1].EffectBasePoints ) * context->in_CastingSpell->quick_tickcount / 100;

	uint32 heal_amt = context->in_dmg;
	if( context->in_events_filter_flags & PROC_ON_CRIT )
		heal_amt = heal_amt / 2;
	heal_amt = heal_amt * ( context->in_OwnerSpell->eff[1].EffectBasePoints ) * context->in_CastingSpell->quick_tickcount / 100;

	context->in_Caller->SpellHeal( context->in_Victim, context->in_CastingSpell, heal_amt, false, true );
*/
	//self proc = no continue !
}

void PH_81069( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;

	//if solar or lunar eclipse just happened
	if( context->in_CastingSpell->Id == 48518 || context->in_CastingSpell->Id == 48517 )
	{
		context->out_CastSpell = dbcSpell.LookupEntryForced( context->in_OwnerSpell->eff[1].EffectTriggerSpell );
		context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[2].EffectBasePoints;
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	}
	//wrath gives lunar energy
	else if( RandChance( context->in_OwnerSpell->eff[0].EffectBasePoints ) )
	{
		if( context->in_CastingSpell->NameHash == SPELL_HASH_WRATH )
			context->out_dmg_overwrite[0] = -13;	//ECPLISE_LUNAR_GAIN_AMT
		else if( context->in_CastingSpell->NameHash == SPELL_HASH_STARFIRE )
			context->out_dmg_overwrite[0] = 20;	//ECPLISE_SOLAR_GAIN_AMT
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	}
	//relax
}

void PH_85421( ProcHandlerContextShare *context )
{
	int32 dmg = context->in_dmg - context->in_abs;
	if( dmg <= 0 )
		return;
	Unit *u = context->in_Caller->GetTopOwner();
	if( u->IsPlayer() == false )
		return;
	int32 dmg_total = dmg * ( context->in_OwnerSpell->eff[0].EffectBasePoints ) / 100;
//	dmg_total += context->in_Caller->GetSpellDmgBonus( context->in_Victim, context->out_CastSpell, dmg_total, 0 );
	int32 dmg_tick = dmg_total / context->out_CastSpell->quick_tickcount;
//	int32 max_tick = float2int32(( u->GetDamageDoneMod( SCHOOL_SHADOW ) * 0.7f + context->in_OwnerSpell->eff[0].EffectBasePoints ) / 7.0f);
	int32 max_tick = float2int32(( u->GetDamageDoneMod( SCHOOL_SHADOW ) * 1.4f + context->in_OwnerSpell->eff[0].EffectBasePoints ) / 7.0f);
	if( dmg_tick > max_tick )
		dmg_tick = max_tick;
	if( dmg_tick <= 0 )
		return;

	context->out_dmg_overwrite[0] = dmg_tick;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;

/*	SpellCastTargets targets( context->in_Victim->GetGUID() );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init(context->in_Caller, context->out_CastSpell ,true, NULL);
	spell->forced_basepoints[0] = dmg_tick;
	spell->ProcedOnSpell = context->in_CastingSpell;
	spell->pSpellId=context->in_OwnerSpell->Id;
	spell->prepare(&targets); */

	//this spell is casted by pet owner always !
}
/*
void PH_91713( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;

	if( ( context->in_events_filter_flags & PROC_ON_CAST_SPELL ) != 0 )
	{
		// shadow ward into neather ward
		SafePlayerCast( context->in_Caller )->SwapActionButtonSpell( 6229, 91711, true, true );
	}
	else
	{
		// neather ward into shadow ward
		SafePlayerCast( context->in_Caller )->SwapActionButtonSpell( 91711, 6229, true, true );
	}
	//self proc !
}*/

void PH_85455( ProcHandlerContextShare *context )
{
	if( ( context->in_dmg - context->in_abs ) <= 0 )
		return;

	//do not double dmg the same target : put bane on target then cast some spell on it to add extra 15% dmg
	if( context->in_event->caster == context->in_Victim->GetGUID() )
		return;

	SpellEntry *spellInfo = dbcSpell.LookupEntry( context->in_event->spellId );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init( context->in_Caller, spellInfo ,true, NULL);
	SpellCastTargets targets2( context->in_event->caster );	//caster is actually the target guid
	spell->pSpellId = context->in_event->spellId;
	spell->forced_basepoints[0] = ( context->in_dmg - context->in_abs ) * context->in_OwnerSpell->eff[0].EffectBasePoints / 100;
	spell->prepare( &targets2 );
//	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_88448( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPet() == false )
		return;
	Player *p = SafePetCast( context->in_Caller )->GetPetOwner();
	if( p == NULL )
		return;

//	if( p->HasAuraWithNameHash( SPELL_HASH_DEMONIC_REBIRTH_MARKER, 0, AURA_SEARCH_NEGATIVE ) )
//		return;
//	p->CastSpell( p, 89140, true ); //SPELL_HASH_DEMONIC_REBIRTH_MARKER

	int64 *CoolDown = p->GetCreateIn64Extension( EXTENSION_ID_DEMONIC_REBIRTH_MARKER_COOLDOWN );
	if( *CoolDown > getMSTime() )
		return;
	*CoolDown = getMSTime() + context->in_event->procInterval;

	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init( p, context->out_CastSpell ,true, NULL);
	SpellCastTargets targets2( p->GetGUID() );	
	spell->pSpellId = context->in_event->spellId;
	spell->forced_basepoints[0] = -context->in_OwnerSpell->eff[0].EffectBasePoints;
	spell->prepare( &targets2 );
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_50091( ProcHandlerContextShare *context )
{
	if( ( context->in_dmg - context->in_abs ) <= 0 )
		return;

	if( context->in_CastingSpell == NULL )
		return;

	if( context->in_CastingSpell->NameHash != SPELL_HASH_HAUNT )
		return;

	if( context->in_event->created_with_value != 0 )
		return;

	context->in_event->created_with_value += MAX( 0, context->in_dmg - context->in_abs );
	//!!self proc !!
}

void PH_85383( ProcHandlerContextShare *context )
{
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_93974( ProcHandlerContextShare *context )
{
	sEventMgr.AddEvent(context->in_Victim,&Unit::AuraOfForebodingDelayedCheck, context->in_Victim->GetPositionX(),context->in_Victim->GetPositionY(), 6.0f, context->in_OwnerSpell->eff[1].EffectTriggerSpell, context->in_Caller->GetGUID(), EVENT_DELAYED_SPELLCAST,6000,1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_89603( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	Aura *a = context->in_Victim->HasAuraWithNameHash( SPELL_HASH_IMMOLATE, context->in_Caller->GetGUID(), AURA_SEARCH_NEGATIVE );
	if( a )
		a->ResetDuration();
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_70664( ProcHandlerContextShare *context )
{
	//no not proc on copy spells of the rejuvenation
	Aura *a = context->in_Victim->HasAuraWithNameHash( SPELL_HASH_REJUVENATION, 0, AURA_SEARCH_POSITIVE );
	if( a && a->pSpellId )
		return;
	//search for a new target 
	Unit *minHPunit = NULL;
	InRangeSetRecProt::iterator itr;
	context->in_Caller->AquireInrangeLock(); //make sure to release lock before exit function !
	InrangeLoopExitAutoCallback AutoLock;
	for( itr = context->in_Caller->GetInRangeSetBegin( AutoLock ); itr != context->in_Caller->GetInRangeSetEnd(); itr++ )
	{
		if( !(*itr)->IsUnit() || !SafeUnitCast((*itr))->isAlive() || context->in_Victim == *itr )
			continue;

		//we target stuff that has no full health. No idea if we must fill target list or not :(
		if( (*itr)->GetUInt32Value( UNIT_FIELD_HEALTH ) == (*itr)->GetUInt32Value( UNIT_FIELD_MAXHEALTH ) )
			continue;

		if( !isAttackable(context->in_Caller,(*itr)) )
		{
			if( minHPunit == NULL )
				minHPunit = SafeUnitCast(*itr);
			else if( minHPunit->GetHealthPct() > SafeUnitCast(*itr)->GetHealthPct() )
				minHPunit = SafeUnitCast(*itr);
		}
	}
	context->in_Caller->ReleaseInrangeLock();	

	if( minHPunit == NULL )
		minHPunit = context->in_Caller;

	SpellEntry *spellInfo = dbcSpell.LookupEntry( 774 );//Rejuvenation
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init(context->in_Caller, spellInfo ,true, NULL);
	spell->ProcedOnSpell = context->in_CastingSpell;
	spell->pSpellId=context->in_OwnerSpell->Id;
	SpellCastTargets targets2( minHPunit->GetGUID() ); //no target so spelltargeting will get an injured party member
	spell->prepare(&targets2);
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_70845( ProcHandlerContextShare *context )
{
	context->out_dmg_overwrite[0] = context->in_Caller->GetMaxHealth() * context->in_OwnerSpell->eff[1].EffectBasePoints / 100;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_89911( ProcHandlerContextShare *context )
{
	if( ( context->in_events_filter_flags & PROC_ON_HEAL_EVENT ) != 0 ) //only penance can heal
	{
		context->out_CastSpell = dbcSpell.LookupEntryForced( 89913 );
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	}
	else if( context->in_CastingSpell && context->in_CastingSpell->NameHash == SPELL_HASH_CHAKRA ) //penance can be casted too, but we only proc on chackra
	{
		if( ( context->in_events_filter_flags & PROC_ON_CAST_SPELL ) == PROC_ON_CAST_SPELL ) 
		{
			context->out_CastSpell = dbcSpell.LookupEntryForced( 89912 );
			context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
		}
		else
		{
			context->in_Caller->RemoveAura( 89912, 0, AURA_SEARCH_ALL, MAX_PASSIVE_AURAS );
		}
	}
}

void PH_89925( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash == SPELL_HASH_FOCUS_FIRE ) 
	{
/*		Aura *a = context->in_Caller->HasAuraWithNameHash( SPELL_HASH_FRENZY, 0, AURA_SEARCH_POSITIVE );
		uint32 stack_count = 0;
		if( a )
			stack_count = a->GetStackCount();
		if( stack_count > 0 )
		{
			context->out_CastSpell = dbcSpell.LookupEntryForced( 1 );	//need to get the spell for this :(
			context->out_dmg_overwrite[0] = stack_count * context->in_OwnerSpell->eff[0].EffectBasePoints;
			context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
		} */
	}
	else if( context->in_CastingSpell->NameHash == SPELL_HASH_CHIMERA_SHOT )
	{
		if( RandChance( context->in_OwnerSpell->procChance ) )
		{
			context->out_CastSpell = dbcSpell.LookupEntryForced( 89928 );	//give focus
			if( context->out_CastSpell != NULL )	//required for migrating 403 to 410
				context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
		}
	}
	else if( context->in_CastingSpell->NameHash == SPELL_HASH_BLACK_ARROW ) 
	{
		context->out_CastSpell = dbcSpell.LookupEntryForced( 95933 );		//extra dmg
		if( context->out_CastSpell != NULL )	//required for migrating 403 to 410
			context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	}
}

void PH_90159( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	uint32 stack_count = context->in_Victim->CountAuraNameHash( SPELL_HASH_LIFEBLOOM, AURA_SEARCH_POSITIVE );
	if( stack_count == 3 )
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_61258( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->c_is_flags2 & SPELL_FLAG2_IS_MOVEMENT_IMPAIRING_SPELL ) 
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_87173( ProcHandlerContextShare *context )
{
//	if( Distance2DSq( context->in_Caller->GetPositionX(), context->in_Caller->GetPositionY(), context->in_Victim->GetPositionX(), context->in_Victim->GetPositionY() ) < 15*15 )
	if( DistanceSq( context->in_Caller, context->in_Victim ) < 9*9 )	//max cast ditance is 10 ? how can one cast it from 15 yards ?
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_85696( ProcHandlerContextShare *context )
{
	context->in_Caller->Energize( context->in_Caller, 85696, 3, POWER_TYPE_HOLY, 0 );
	//!!self proc !!
}

void PH_85547( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash == SPELL_HASH_CURSE_OF_THE_ELEMENTS )
	{
		int32 max_tagets = context->in_OwnerSpell->eff[2].EffectBasePoints;
		float r = ( float )context->in_OwnerSpell->eff[0].EffectBasePoints;
		float srcx = context->in_Victim->GetPositionX();
		float srcy = context->in_Victim->GetPositionY();
		float srcz = context->in_Victim->GetPositionZ();
		r = r * r;
		InRangeSetRecProt::iterator itr,itr2;
		context->in_Caller->AquireInrangeLock(); //make sure to release lock before exit function !
		InrangeLoopExitAutoCallback AutoLock;
		for( itr2 = context->in_Caller->GetInRangeSetBegin( AutoLock ); itr2 != context->in_Caller->GetInRangeSetEnd();)
		{
			itr = itr2;
			itr2++; //maybe scripts can change list. Should use lock instead of this to prevent multiple changes. This protects to 1 deletion only
			if( !( (*itr)->IsUnit() ) 
					|| !SafeUnitCast( *itr )->isAlive() 
					//fix for spell not targetting totems ? But they are not supposed to target them no ?
	//				|| ( SafeUnitCast( *itr )->IsCreature() && SafeCreatureCast( *itr )->IsTotem()) 
				|| (*itr) == context->in_Victim
					)
				continue;

			if( IsInrange( srcx, srcy, srcz, (*itr), r ) && isAttackable( context->in_Caller, ( *itr ) ) )
			{
				context->in_Caller->CastSpell( SafeUnitCast(*itr), context->out_CastSpell, true );
				//maybe first make a list of possible targets then cast the spell ? To avoid deadlocks ofc
				max_tagets--;
				if( max_tagets <= 0 )
					break;
			}
		}
		context->in_Caller->ReleaseInrangeLock();
	}
	else //SPELL_HASH_CURSE_OF_WEAKNESS
	{
		if( context->in_Victim->GetPowerType() == POWER_TYPE_RAGE )
			context->out_CastSpell = dbcSpell.LookupEntryForced( 85539 ); // Jinx - mod power cost
		else if( context->in_Victim->GetPowerType() == POWER_TYPE_ENERGY )
			context->out_CastSpell = dbcSpell.LookupEntryForced( 85540 ); // Jinx - mod power cost
		else if( context->in_Victim->GetPowerType() == POWER_TYPE_RUNIC )
			context->out_CastSpell = dbcSpell.LookupEntryForced( 85541 ); // Jinx - mod power cost
		else if( context->in_Victim->GetPowerType() == POWER_TYPE_FOCUS )
			context->out_CastSpell = dbcSpell.LookupEntryForced( 85542 ); // Jinx - mod power cost
		else
			return;
		context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[1].EffectBasePoints;
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	}
	//self proc !!
}

void PH_86664( ProcHandlerContextShare *context )
{
	Aura *a = context->in_Caller->HasAuraWithNameHash( SPELL_HASH_SOULBURN, 0, AURA_SEARCH_POSITIVE );
	if( a == NULL )
		return;
	a->Remove();
	a = NULL;
	float r = 15.0f * 15.0f;
	float srcx = context->in_Victim->GetPositionX();
	float srcy = context->in_Victim->GetPositionY();
	float srcz = context->in_Victim->GetPositionZ();
	InRangeSetRecProt::iterator itr,itr2;
	context->in_Caller->AquireInrangeLock(); //make sure to release lock before exit function !
	InrangeLoopExitAutoCallback AutoLock;
	for( itr2 = context->in_Caller->GetInRangeSetBegin( AutoLock ); itr2 != context->in_Caller->GetInRangeSetEnd();)
	{
		itr = itr2;
		itr2++; //maybe scripts can change list. Should use lock instead of this to prevent multiple changes. This protects to 1 deletion only
		if( !( (*itr)->IsUnit() ) 
				|| !SafeUnitCast( *itr )->isAlive() 
				//fix for spell not targetting totems ? But they are not supposed to target them no ?
//				|| ( SafeUnitCast( *itr )->IsCreature() && SafeCreatureCast( *itr )->IsTotem()) 
//			|| (*itr) == context->in_Victim
				)
			continue;

		if( IsInrange( srcx, srcy, srcz, (*itr), r ) && isAttackable( context->in_Caller, ( *itr ) ) )
		{
			context->in_Caller->CastSpell( SafeUnitCast(*itr), 172, true );	//corruption
			//maybe first make a list of possible targets then cast the spell ? To avoid deadlocks ofc
		}
	}
	context->in_Caller->ReleaseInrangeLock();

//	if( context->in_Victim->IsDead() )
		context->in_Caller->Energize( context->in_Caller, 86664, 1, POWER_TYPE_SOUL_SHARDS, 0 );
	//self proc !!
}

void PH_86183( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	SafePlayerCast( context->in_Caller )->ModCooldown( 16166, context->in_OwnerSpell->eff[0].EffectBasePoints, false );
	//!! self proc )
}

void PH_55277( ProcHandlerContextShare *context )
{
	SpellEntry *spellInfo = dbcSpell.LookupEntry( 55277 );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init( context->in_Caller, spellInfo ,true, NULL);
	spell->ProcedOnSpell = context->in_CastingSpell;
	spell->pSpellId=context->in_OwnerSpell->Id;
	spell->forced_pct_mod[0] = context->in_OwnerSpell->eff[0].EffectBasePoints * 100;
	SpellCastTargets targets2( context->in_Caller->GetGUID() ); //no target so spelltargeting will get an injured party member
	spell->prepare(&targets2);
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
	//we casted it no need to continue
}

void PH_90166( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL || context->in_CastingSpell->Id != 33876 )	//mangle cat
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_11094( ProcHandlerContextShare *context )
{
	//self proc !
	context->in_Caller->RemoveAurasMovementImpairing();
}

void PH_91342( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() )
	{
		Pet *p = SafePlayerCast( context->in_Caller )->GetSummon();
		if( p )
		{
			uint32 stacks = p->CountAuraNameHash( SPELL_HASH_SHADOW_INFUSION, AURA_SEARCH_POSITIVE );
			if( stacks >= 4 )
				context->in_Caller->CastSpell( context->in_Caller, 93426, true );	//dark transformation enabler
		}
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_81229( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() )
	{
		Player *p = SafePlayerCast( context->in_Caller );
		uint32 empty_runes[TOTAL_USED_RUNES];
		uint32 empty_rune_counter = 0;
		//get a fully depleted rune
		for(int j=0;j<TOTAL_USED_RUNES;j++)
//			if( p->m_runes[j] == 0 )
			if( p->m_runes[j] <= 50 )	//just in case we made some hackfix somewhere else to not put exactly 0 for this value
				empty_runes[empty_rune_counter++]=j;
		if( empty_rune_counter )
		{
			uint32 random_index = RandomUInt() % empty_rune_counter;
			uint32 RefreshIndex = empty_runes[random_index];
			p->m_runes[ RefreshIndex ] = MAX_RUNE_VALUE;			//activate it
			p->UpdateRuneCooldowns();
//			p->ConvertRuneIcon( RefreshIndex, RUNE_DEATH );
//			p->ConvertRuneIcon( RefreshIndex, p->m_rune_types[RefreshIndex] );
//p->BroadcastMessage( "refreshing rune");
			context->in_event->LastTrigger = getMSTime(); // consider it triggered
		}
	}
	//!! self proc !
}

void PH_87204( ProcHandlerContextShare *context )
{
	//this is for vampiric touch
	if( ( context->in_events_filter_flags & PROC_ON_PRE_DISPELL_AURA_CASTER_EVENT )
		&& RandChance( context->in_OwnerSpell->eff[0].EffectBasePoints ) )
	{
		//let's see if vampiric touch is getting dispelled
		SpellEntry *sp = dbcSpell.LookupEntryForced( context->in_dmg );
		if( sp == NULL || sp->NameHash != SPELL_HASH_VAMPIRIC_TOUCH )
			return;
		//cast it our target location
/*		SpellCastTargets targets( context->in_Victim->GetGUID() );
		targets.m_destX = context->in_Victim->GetPositionX();
		targets.m_destY = context->in_Victim->GetPositionY();
		targets.m_destZ = context->in_Victim->GetPositionZ();
		Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		spell->Init(context->in_Caller, context->out_CastSpell ,true, NULL);
		spell->ProcedOnSpell = context->in_CastingSpell;
		spell->pSpellId = context->in_OwnerSpell->Id;
		spell->prepare(&targets); */
		//we need to delay cast this spell or "dispel" will remove it
		context->in_Caller->CastSpellDelayed( context->in_Victim->GetGUID(), context->out_CastSpell->Id, 1, true );
	}
	else if( context->in_CastingSpell && context->in_CastingSpell->NameHash == SPELL_HASH_MIND_FLAY 
		&& ( context->in_events_filter_flags & PROC_ON_CRIT ) != 0
		&& context->in_Caller->IsPlayer() )
	{
		//reduce shadow fiend cooldown by 10 sec
		SafePlayerCast( context->in_Caller )->ModCooldown( 34433, -1000 * context->in_OwnerSpell->eff[1].EffectBasePoints, false );
	}
}

void PH_85497( ProcHandlerContextShare *context )
{
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[1].EffectBasePoints;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_56829( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	if( context->in_Victim != SafePlayerCast(context->in_Caller)->GetSummon() )
		return;
	Player * p = SafePlayerCast(context->in_Caller);
//	sEventMgr.AddEvent( p,&Player::ClearCooldownForSpell, (uint32)34477, EVENT_DELAYED_SPELLCAST,1,1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
	p->ClearCooldownForSpell( 34477 );	//Misdirection
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_96379( ProcHandlerContextShare *context )
{
//	if( context->in_CastingSpell->quick_tickcount != 1 )
//		return;
	if( ( context->in_dmg - context->in_abs ) <= 0 )
		return;
	if( context->in_CastingSpell == NULL )
		return;
	if( ( context->in_CastingSpell->c_is_flags3 & SPELL_FLAG3_IS_DIRECT_TARGET ) == 0 )
		return;
	//blizzlike anti packet spam
//	if( context->in_Caller->GetHealth() == context->in_Caller->GetMaxHealth() )
//		return;
	//single target spell
//	if( context->in_CastingSpell->eff[0].EffectImplicitTargetA != EFF_TARGET_SINGLE_ENEMY && context->in_CastingSpell->eff[1].EffectImplicitTargetA != EFF_TARGET_SINGLE_ENEMY && context->in_CastingSpell->eff[2].EffectImplicitTargetA != EFF_TARGET_SINGLE_ENEMY )
//		return;
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[1].EffectBasePoints * ( context->in_dmg - context->in_abs ) / 100;
	context->in_Caller->SpellHeal( context->in_Caller, context->out_CastSpell, context->out_dmg_overwrite[0], false, true, false, 1 );
//	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_79095( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_RUPTURE && context->in_CastingSpell->NameHash != SPELL_HASH_EVISCERATE
		&& context->in_CastingSpell->NameHash != SPELL_HASH_ENVENOM && context->in_CastingSpell->NameHash != SPELL_HASH_DEADLY_THROW )
		return;
	int32 reduce_ms = context->in_OwnerSpell->eff[0].EffectBasePoints * SafePlayerCast( context->in_Caller )->m_comboPoints;
	SafePlayerCast( context->in_Caller )->ModCooldown( 13750, -reduce_ms, false );	//Adrenaline Rush
	SafePlayerCast( context->in_Caller )->ModCooldown( 51690, -reduce_ms, false );	//Killing Spree
	SafePlayerCast( context->in_Caller )->ModCooldown( 73981, -reduce_ms, false );	//Redirect
	SafePlayerCast( context->in_Caller )->ModCooldown( 2983, -reduce_ms, false );	//Sprint
}

void PH_84617( ProcHandlerContextShare *context )
{
	//we only mod our caster dmg
	if( context->in_Victim->IsPlayer() == false )
		return;
	if( context->in_Victim->GetGUID() != context->in_event->caster )
		return;
	if( context->in_CastingSpell == NULL )
		return;
//	if( SafePlayerCast( context->in_Victim )->m_currentSpellAny == NULL )
//		return;
	//we need a finishing move for this 
	if( !(context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_FINISHING_MOVE) )
		return;
	if( context->in_CastingSpell->NameHash == SPELL_HASH_KIDNEY_SHOT || context->in_CastingSpell->NameHash == SPELL_HASH_EXPOSE_ARMOR )
	{
		//well this sucks we should mod the duration of the aura that we did not put on the target yet
		Aura *a = context->in_Caller->HasAuraWithNameHash( context->in_CastingSpell->NameHash, 0, AURA_SEARCH_NEGATIVE ); //we just received this aura on ourself from real caster
		if( a )
		{
			uint32 dur = a->GetDuration();
			dur += dur * (int32)context->in_event->created_with_value / 100;
			a->SetDuration( dur );
			a->ResetDuration();
		}
	}
	//add pct : rupture, eviscerate
	else if( context->in_dmg_loc != NULL && *context->in_dmg_loc > 0 )
	{
//		*context->in_dmg_loc += (*context->in_dmg_loc) * context->in_OwnerSpell->eff[2].EffectBasePoints / 100;
		//can be moded by Glyph of Revealing Strike
		*context->in_dmg_loc += (*context->in_dmg_loc) * (int32)context->in_event->created_with_value / 100;
	}
//	context->in_event->deleted = true;	//suicide
	//!!!suicide !!! naaah this would lead to mem corruption
	context->in_Caller->RemoveAura( context->in_OwnerSpell->Id );
}

void PH_96263( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	if( context->in_Caller->GetHealthPct() < 30 )
		return;
	int32 new_hp = context->in_Caller->GetHealth() - ( context->in_dmg - context->in_abs );
	int32 new_pct = new_hp * 100 / context->in_Caller->GetMaxHealth();
	if( new_pct > 30 )
		return;
	context->out_dmg_overwrite[0] = float2int32( context->in_Caller->GetAP() * 2.8f );
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_99262( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell || !( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_HEALING ) )
		return;
	//we need direct heal spell = no amplitude presence
	if( context->in_CastingSpell->quick_tickcount > 1 )
		return;
	//let's not abuse it
	if( context->in_Victim->HasAuraWithNameHash( SPELL_HASH_VITAL_SPARK, 0, AURA_SEARCH_POSITIVE ) )
		return;
	Aura *a = context->in_Victim->HasAuraWithNameHash( SPELL_HASH_TORMENT, 0, AURA_SEARCH_POSITIVE );
	if( a )
	{
		uint16 stacks_torment = a->GetStackCount();
		//get a vital spark for each 3 torments
		for( uint16 i=0;i<stacks_torment/3;i++)
			context->in_Victim->CastSpell( context->in_Victim, context->out_CastSpell, true );
	}
	//we handled it !
}

void PH_49588( ProcHandlerContextShare *context )
{
	//target needs to hold XP or honor. Kinda quick test
	if( TargetGivesXPOrHonor( context->in_Victim, context->in_Caller ) == false )
		return;
	//reset cooldown on death grip
	if( context->in_Caller->IsPlayer() )
		SafePlayerCast(context->in_Caller)->ClearCooldownForSpell( 49576 );
	//self proc !!!
} 

void PH_64128( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash == SPELL_HASH_CURE_DISEASE )
	{
		if( RandChance( context->in_OwnerSpell->eff[1].EffectBasePoints ) )
		{
			context->out_CastSpell = dbcSpell.LookupEntryForced( context->in_OwnerSpell->eff[1].EffectTriggerSpell ); //remove one 1 poison
			context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
		}
	}
	else
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
} 

void PH_56161( ProcHandlerContextShare *context )
{
	context->out_dmg_overwrite[0] = ( context->in_dmg - context->in_abs ) * context->in_OwnerSpell->eff[0].EffectBasePoints / 100 / context->out_CastSpell->quick_tickcount;
	if( context->out_dmg_overwrite[0] < 0 )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
} 

void PH_33143( ProcHandlerContextShare *context )
{
	//Whenever you are victim of an attack equal to damage greater than $s2% of your total health or critically hit by any non-periodic attack, you gain Blessed Resilience increasing all healing received by $s1% lasting for $33143d.
	if( ( context->in_events_filter_flags & PROC_ON_CRIT )
		&& ( context->in_CastingSpell == NULL || context->in_CastingSpell->quick_tickcount <= 1 ) )
	{
		context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints;
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
		return;
	}
	int32 real_dmg = context->in_dmg - context->in_abs;
	int32 dmg_pct = real_dmg * 100 / context->in_Caller->GetMaxHealth();
	if( dmg_pct < context->in_OwnerSpell->eff[1].EffectBasePoints )
		return;
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
} 

void PH_83077( ProcHandlerContextShare *context )
{
	//all dmg is absorbed ?
	if( context->in_dmg < context->in_abs )
		return;
	Aura *a = context->in_Victim->HasAuraWithNameHash( SPELL_HASH_SERPENT_STING, 0, AURA_SEARCH_NEGATIVE );
	//right now we can't properly handle this script without an aura to store proc state
	if( a == NULL )
		return;
	//just refreshing the buff. We will proc on dmg
	if( context->in_events_filter_flags & PROC_ON_CAST_SPELL_EVENT )
	{
		a->m_modList[2].fixed_amount[0] = 0;
		return;
	}
	if( a->m_modList[2].fixed_amount[0] != 0 )
		return;
	a->m_modList[2].fixed_amount[0] = 1;

	if( a )
	{
		uint32 Duration = a->GetDuration();
		uint32 Amplitude = GetSpellAmplitude( a->GetSpellProto(), context->in_Caller, 0, a->m_castedItemId );
		uint32 TickCount = Duration / Amplitude;
		int32 Dmg = ( context->in_dmg - context->in_abs );
		if( context->in_events_filter_flags & PROC_ON_CRIT )
			Dmg = Dmg / 1.5f;

//		int32 spell_dmg_bonus = context->in_Caller->GetSpellDmgBonus( context->in_Victim, a->GetSpellProto(), a->m_modList[0].m_amount, 0 );
//		context->out_dmg_overwrite[0] = ( spell_dmg_bonus + a->m_modList[0].m_amount ) * TickCount * context->in_OwnerSpell->eff[0].EffectBasePoints / 100 ;

		context->out_dmg_overwrite[0] = Dmg * TickCount * context->in_OwnerSpell->eff[0].EffectBasePoints / 100 ;

		if( context->out_dmg_overwrite[0] <= 0 )
			return;
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
} 

void PH_50434( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	//the trick of not proccing chains of ice -> frost fever -> chains of ice
	if( context->in_CastingSpell->NameHash == SPELL_HASH_FROST_FEVER )
	{
		context->in_Caller->CastSpell( context->in_Victim, context->in_OwnerSpell->eff[0].EffectTriggerSpell, true);
		return;
	}
	if( context->in_CastingSpell->NameHash == SPELL_HASH_CHAINS_OF_ICE )
		context->out_CastSpell = dbcSpell.LookupEntryForced( context->in_OwnerSpell->eff[1].EffectTriggerSpell );
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
} 

void PH_101568( ProcHandlerContextShare *context )
{
	//target needs to hold XP or honor. Kinda quick test
	if( TargetGivesXPOrHonor( context->in_Victim, context->in_Caller ) == false )
		return;
	if( context->in_Caller->HasAuraWithNameHash( SPELL_HASH_FROST_PRESENCE, 0, AURA_SEARCH_POSITIVE ) == NULL && context->in_Caller->HasAuraWithNameHash( SPELL_HASH_UNHOLY_PRESENCE, 0, AURA_SEARCH_POSITIVE ) == NULL )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
} 

void PH_96945( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	if( (int32)context->in_Caller->GetHealthPct() < context->in_OwnerSpell->eff[0].EffectBasePoints )
		return;
	int32 new_hp = context->in_Caller->GetHealth() - ( context->in_dmg - context->in_abs );
	int32 new_pct = new_hp * 100 / context->in_Caller->GetMaxHealth();
	if( new_pct > context->in_OwnerSpell->eff[0].EffectBasePoints )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_96962( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL || context->in_CastingSpell->quick_tickcount <= 1 )
		return;
	if( ( context->in_dmg - context->in_abs ) <= 0 )
		return;
	if( context->in_CastingSpell == NULL )
		return;
	if( ( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_DAMAGING ) == 0 )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_99063( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_FROSTBOLT 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_FIREBALL 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_FROSTFIRE_BOLT 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_ARCANE_BLAST 
		)
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_99132( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_FLASH_HEAL 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_HEAL 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_GREATER_HEAL 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_PRAYER_OF_MENDING 
		)
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_99136( ProcHandlerContextShare *context )
{
	if( isAttackable( context->in_Caller, context->in_Victim, false ) == true )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_99202( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( ( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_DAMAGING ) == 0 )
		return;
	if( context->in_Caller->IsPlayer() )
		SafePlayerCast( context->in_Caller )->ClearCooldownForSpell( 2894 );	//Fire Elemental Totem
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_99232( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_SHADOW_BOLT 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_INCINERATE 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_SOUL_FIRE 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_DRAIN_SOUL 
		)
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_99244( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_101085( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( ( context->in_dmg - context->in_abs ) <= 0 )
		return;
	if( ( context->in_events_filter_flags & PROC_ON_HEAL_EVENT ) != 0 ) 
		return;
	if( context->in_Victim == context->in_Caller ) //no masochism
		return;
//	context->out_dmg_overwrite[0] = context->in_dmg;
//	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;

	//!! This implementation is not the best. It might not tick first direct dmg on combined spells. 
	// Ex : immolate has both direct dmg and DOT. It might trigger on DOT and do the ticks copy, but might forget to copy the direct dmg part


	bool TickBecauseWeSelectedThisDOT = false;
	Aura *a = NULL;
	if( context->in_event->created_with_value != -1 ) //do not assign new tracking until the old aura expired
	{
		context->in_event->created_with_value = MIN( context->in_event->created_with_value, context->in_Victim->m_auras_neg_size );
		a = context->in_Victim->m_auras[ context->in_event->created_with_value ];
		if( a != NULL && a == context->in_event->custom_holder )
		{
			if( context->in_CastingSpell == a->GetSpellProto() )
				TickBecauseWeSelectedThisDOT = true;
		}
		else
		{
			context->in_event->created_with_value = -1;
//if( context->in_Caller->IsPlayer() ) SafePlayerCast( context->in_Caller )->BroadcastMessage( "Abandoned monitoring old DOTS" );
		}
	}

	//check if we should proc due to proc interval and chance
	if( TickBecauseWeSelectedThisDOT == false )
	{
		if( ( ( context->in_events_filter_flags & ( PROC_ON_DOT | PROC_ON_FIRST_TICK ) ) == ( PROC_ON_DOT | PROC_ON_FIRST_TICK ) ) //it is a DOT first Tick
			  || ( context->in_events_filter_flags & ( PROC_ON_DOT ) ) == 0 //it is direct dmg spell
			) 
		{
//			if( context->in_event->LastTrigger + 4000 > getMSTime() )
//				return; 
	
			int32 ProcChance = 7.5;
//ProcChance = 200;
			uint32 RealProcChance100 = context->in_event->procSuccessCounter * 100 * 100 / context->in_event->procEventCounter;
			uint32 ExpectedEventSuccessInterval100 = 100 * ProcChance;
			float  ChanceAdjustment = (float)ExpectedEventSuccessInterval100 / (float)RealProcChance100;
			if( ChanceAdjustment < 1.0f )
				ChanceAdjustment = ChanceAdjustment - 1.0f;

			context->in_event->procEventCounter++;
			int32 ChanceAdjustmentInt = float2int32( 10.0f * ChanceAdjustment ); //with N(500) events this should have time to adjust chance
			if( ChanceAdjustmentInt < -(int32)ProcChance )
				ChanceAdjustmentInt = -( (int32)ProcChance - 2 ); //leave 2% chance to fate
			int32 proc_Chance = (int32)ProcChance + ChanceAdjustmentInt;
			if( proc_Chance <= 0 )
				proc_Chance = 1;
			if( RandChance( proc_Chance ) == false )
				return;

			context->in_event->procSuccessCounter++;
			//Reset from time to time. If chance is 1% : sucess is 100 and event is 10000
			if( context->in_event->procSuccessCounter > 500 )
			{
				context->in_event->procSuccessCounter = 1;
				context->in_event->procEventCounter = 1;
			}
		}
		else
		{
			return;
		}

		//DOTS will be duplicated at every tick
		if( ( context->in_events_filter_flags & ( PROC_ON_DOT | PROC_ON_FIRST_TICK ) ) == ( PROC_ON_DOT | PROC_ON_FIRST_TICK ) 
			|| context->in_CastingSpell->quick_tickcount > 1 //for spells like immolate where first tick is direct dmg, duplicate the other ticks as well
			) 
		{
			a = context->in_Victim->HasAura( context->in_CastingSpell->Id, 0, AURA_SEARCH_NEGATIVE );
			if( a )
			{
				context->in_event->created_with_value = a->m_auraSlot;
				context->in_event->custom_holder = a;
//if( context->in_Caller->IsPlayer() ) SafePlayerCast( context->in_Caller )->BroadcastMessage( "Started monitoring new DOTS %s", context->in_CastingSpell->Name );
			}
		}

		//!!duplicates the spell and not the DMG. For DOT, we will proc on EVERY DMG tick
		context->in_event->LastTrigger = getMSTime(); // consider it triggered
	}

	//duplicate the DMG
	dealdamage tdmg;
	tdmg.base_dmg = ( context->in_dmg - context->in_abs );
	tdmg.StaticDamage = true;
	tdmg.DisableProc = true;
	context->in_Caller->SpellNonMeleeDamageLog( context->in_Victim, context->out_CastSpell, &tdmg, context->in_events_filter_flags & ( PROC_ON_DOT ) );
//if( context->in_Caller->IsPlayer() ) SafePlayerCast( context->in_Caller )->BroadcastMessage( "ticking on %s with %d, isdot %d", context->in_CastingSpell->Name, context->in_dmg - context->in_abs, ( context->in_events_filter_flags & ( PROC_ON_DOT )) != 0 );
}

void PH_96890( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( ( context->in_dmg - context->in_abs ) <= 0 )
		return;
	if( ( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_DAMAGING ) == 0 )
		return;
	//give a chance to eat up the charges 
	if( RandChance( 25 ) )
	{
		Aura *a = context->in_Caller->HasAuraWithNameHash( SPELL_HASH_ELECTRICAL_CHARGE, 0, AURA_SEARCH_POSITIVE );
		uint32 charges = 1;
		if( a )
			charges += a->GetStackCount();
		a = NULL; // morron protection
		context->in_Caller->RemoveAuraByNameHash( SPELL_HASH_ELECTRICAL_CHARGE, 0, AURA_SEARCH_POSITIVE, MAX_PASSIVE_AURAS );
		context->out_CastSpell = dbcSpell.LookupEntryForced( 96891 );
		context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints * charges;
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_96977( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	Player *p_caster = SafePlayerCast( context->in_Caller );
	uint32 SpellID = 0;
	if( p_caster->CalcRating( PLAYER_RATING_MODIFIER_MELEE_CRIT ) > p_caster->CalcRating( PLAYER_RATING_MODIFIER_MELEE_HASTE )
		&& p_caster->CalcRating( PLAYER_RATING_MODIFIER_MELEE_CRIT ) > p_caster->CalcRating( PLAYER_RATING_MODIFIER_MASTERY ) )
		SpellID = 96978;	//crit
	else if( p_caster->CalcRating( PLAYER_RATING_MODIFIER_MELEE_HASTE ) > p_caster->CalcRating( PLAYER_RATING_MODIFIER_MASTERY ) )
		SpellID = 96977;	//haste
	else
		SpellID = 96979;	//mastery
	context->out_CastSpell = dbcSpell.LookupEntryForced( SpellID );
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_97139( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	Player *p_caster = SafePlayerCast( context->in_Caller );
	uint32 SpellID = 0;
	if( p_caster->CalcRating( PLAYER_RATING_MODIFIER_MELEE_CRIT ) > p_caster->CalcRating( PLAYER_RATING_MODIFIER_MELEE_HASTE )
		&& p_caster->CalcRating( PLAYER_RATING_MODIFIER_MELEE_CRIT ) > p_caster->CalcRating( PLAYER_RATING_MODIFIER_MASTERY ) )
		SpellID = 97140;	//crit
	else if( p_caster->CalcRating( PLAYER_RATING_MODIFIER_MELEE_HASTE ) > p_caster->CalcRating( PLAYER_RATING_MODIFIER_MASTERY ) )
		SpellID = 97139;	//haste
	else
		SpellID = 97141;	//mastery
	context->out_CastSpell = dbcSpell.LookupEntryForced( SpellID );
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_31803( ProcHandlerContextShare *context )
{
	if( ( context->in_events_filter_flags & PROC_ON_CAST_SPELL_EVENT ) != 0 && context->in_CastingSpell != NULL )
	{
		if( context->in_CastingSpell->NameHash != SPELL_HASH_JUDGEMENT
			&& context->in_CastingSpell->NameHash != SPELL_HASH_HAMMER_OF_WRATH 
			&& context->in_CastingSpell->NameHash != SPELL_HASH_EXORCISM
			&& context->in_CastingSpell->NameHash != SPELL_HASH_TEMPLAR_S_VERDICT
			)
			return;
	}
	uint32 StackCount = context->in_Victim->CountAura( 31803, AURA_SEARCH_NEGATIVE ); //CENSURE
	//should we also refresh the duration of censure ?
	if( StackCount >= 5 && context->in_CastingSpell == NULL )
		context->in_Caller->CastSpell( context->in_Victim, 42463, true ); //deal 15% weapon dmg
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_89024( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( (context->in_CastingSpell->c_is_flags2 & SPELL_FLAG2_IS_PROCCING_PURSUIT_OF_JUSTICE) == 0 )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_77487( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() && SafePlayerCast( context->in_Caller )->GetFirstTalentSpecTree() != 795 )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_51699( ProcHandlerContextShare *context )
{
	//only if critting with spell or ability. Ignore melee or ranged hits
	if( context->in_CastingSpell == NULL )
		return;

	//get the caster of this event and f spam filter allows it then we force him to cast this on he's target
	if( context->in_Caller->GetMapMgr() == NULL )
		return;
	Player *pc = context->in_Caller->GetMapMgr()->GetPlayer( context->in_event->caster );
	if( pc == NULL )
		return;

	//check if the proc is on cooldown
	int64 *cooldown_holder = pc->GetCreateIn64Extension( EXTENSION_ID_HONOR_AMONG_THIEVES_COOLDOWN );
	if( *cooldown_holder > getMSTime() )
		return;
	*cooldown_holder = getMSTime() + context->in_OwnerSpell->eff[1].EffectBasePoints;

	uint64 target = pc->m_comboTarget;
	if( target == 0 )
	{
		Unit *ut = context->in_Caller->GetMapMgr()->GetUnit( pc->GetSelection() );
		if( isAttackable( pc, ut ) )
			target = pc->GetSelection();
	}
	if( target )
		pc->CastSpell( target, context->out_CastSpell->Id, true );

	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_84584( ProcHandlerContextShare *context )
{
	Aura *a = context->in_Victim->HasAuraWithNameHash( SPELL_HASH_REND, 0, AURA_SEARCH_NEGATIVE );
	if( a )
		a->ResetDuration();
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_88852( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell->NameHash == SPELL_HASH_HOLY_RADIANCE )
	{
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
		return;
	}
	bool is_target_beacon = false;
	uint64 tcasterguid = context->in_Caller->GetGUID();
	CommitPointerListNode<ProcTriggerSpell> *itr;
	for(itr = context->in_Caller->m_procSpells.begin();itr != context->in_Caller->m_procSpells.end();itr = itr->Next() )
		if(itr->data->origId == 53563 && !itr->data->deleted)
		{
			//is our beacon target the current target ?
			if( itr->data->caster == context->in_Victim->GetGUID() )
				is_target_beacon = true;
			break;
		}
	if( is_target_beacon == false )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_16038( ProcHandlerContextShare *context )
{
	Aura *a = context->in_Victim->HasAuraWithNameHash( SPELL_HASH_FLAME_SHOCK, 0, AURA_SEARCH_NEGATIVE );
	if( a )
	{
		uint32 new_dur = MAX( a->GetDuration(), 6000 );
		a->SetDuration( new_dur );
		a->ResetDuration();
	}
	//custom handler, do not continue execution!
}

void PH_99092( ProcHandlerContextShare *context )
{
	if( ( context->in_dmg - context->in_abs ) <= 0 )
		return;
	context->out_dmg_overwrite[0] = ( context->in_OwnerSpell->eff[0].EffectBasePoints ) * (context->in_dmg - context->in_abs) / 100;
	if( context->out_dmg_overwrite[0] <= 0 )
		return;
	context->out_dmg_overwrite[0] = context->out_dmg_overwrite[0] / context->out_CastSpell->quick_tickcount;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_94686( ProcHandlerContextShare *context )
{
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[2].EffectBasePoints;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_48301( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	if( SafePlayerCast( context->in_Caller )->GetShapeShift() != FORM_SHADOW )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}


void PH_103419( ProcHandlerContextShare *context )
{
	if( context->in_dmg <= 2 )
		return;
	if( context->in_dmg >= context->in_OwnerSpell->eff[0].EffectBasePoints )
	{
		context->in_Caller->RemoveAura( context->in_OwnerSpell->Id );
		return;	//!!!suicide
	}
	context->out_dmg_absorb = context->in_dmg - 1;
	//!!self proc, do not continue !
}


void PH_102663( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( ( context->in_CastingSpell->c_is_flags & ( SPELL_FLAG_IS_DAMAGING | SPELL_FLAG_IS_HEALING ) ) == 0 )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_97130( ProcHandlerContextShare *context )
{
 	if( context->in_Caller->IsPlayer() == false )
		return; 
	if( ( context->in_dmg - context->in_abs ) <= 0 )
		return;
	//Damage that takes you below 35% health is reduced by 20%.  
	int32 new_health = (int32)context->in_Caller->GetHealth() - ( context->in_dmg - context->in_abs );
	int32 new_health_pct = new_health * 100 / context->in_Caller->GetMaxHealth();
	int32 old_health_pct = context->in_Caller->GetHealthPct();
//	if( old_health_pct > context->in_OwnerSpell->eff[0].EffectBasePoints && new_health_pct < context->in_OwnerSpell->eff[0].EffectBasePoints )
	if( new_health_pct < context->in_OwnerSpell->eff[0].EffectBasePoints )
	{
		context->out_dmg_absorb += context->in_dmg + 5;	//avoid death
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	}
}

void PH_108008( ProcHandlerContextShare *context )
{
 	if( context->in_Caller->IsPlayer() == false )
		return; 
	if( ( context->in_dmg - context->in_abs ) <= 0 )
		return;
	//Damage that takes you below 35% health is reduced by 20%.  
	int32 new_health = (int32)context->in_Caller->GetHealth() - ( context->in_dmg - context->in_abs );
	int32 new_health_pct = new_health * 100 / context->in_Caller->GetMaxHealth();
	int32 old_health_pct = context->in_Caller->GetHealthPct();
//	if( old_health_pct > context->in_OwnerSpell->eff[0].EffectBasePoints && new_health_pct < context->in_OwnerSpell->eff[0].EffectBasePoints )
	if( new_health_pct < context->in_OwnerSpell->eff[0].EffectBasePoints )
	{
		context->out_dmg_absorb += ( context->in_dmg - context->in_abs ) + 5;	//avoid death
		context->out_dmg_overwrite[0] = ( context->in_dmg - context->in_abs ) * ( context->in_OwnerSpell->eff[1].EffectBasePoints + 1 ) / 100;
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	}
}

void PH_98996( ProcHandlerContextShare *context )
{
	if( ( context->in_dmg - context->in_abs ) <= 0 )
		return;
	dealdamage tdmg;
	tdmg.base_dmg = ( context->in_dmg - context->in_abs ) * context->in_OwnerSpell->eff[0].EffectBasePoints / 100;
	tdmg.StaticDamage = true;
	tdmg.DisableProc = true;
	context->in_Caller->SpellNonMeleeDamageLog( context->in_Victim, context->in_CastingSpell, &tdmg );
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
	//!!self proc
}

void PH_99011( ProcHandlerContextShare *context )
{
	if( ( context->in_events_filter_flags & PROC_ON_AURA_REMOVE ) == PROC_ON_AURA_REMOVE )
	{
		if( context->in_CastingSpell && context->in_CastingSpell->NameHash == SPELL_HASH_BARKSKIN )
		{
			context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
		}
	}
	else if ( context->in_Caller->IsPlayer()
			&& context->in_CastingSpell 
			&& ( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_FINISHING_MOVE ) )
	{
		uint32 chance = SafePlayerCast( context->in_Caller )->m_comboPoints * 20;
		if( RandomUInt() % 100 > chance )
			return;
		Aura *a = context->in_Caller->HasAuraWithNameHash( SPELL_HASH_BERSERK, 0, AURA_SEARCH_POSITIVE );
		if( a )
		{
			a->SetDuration( a->GetDuration() + 2000 );
			a->ResetDuration();
			context->in_event->LastTrigger = getMSTime(); // consider it triggered
		}
	}
}

void PH_99015( ProcHandlerContextShare *context )
{
	uint64 new_target = Spell::FindLowestHealthRaidMember( SafePlayerCast( context->in_Caller ), 15*15); // within 15 yards
	if( new_target == 0 || new_target == context->in_Caller->GetGUID() )	// shouldn't happen
		return;
	SpellCastTargets targets2( new_target ); //no target so spelltargeting will get an injured party member
	SpellEntry *spellInfo = dbcSpell.LookupEntry( context->in_event->spellId );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init(context->in_Caller, spellInfo ,true, NULL);
	spell->forced_basepoints[0] = ( context->in_dmg - context->in_abs ) * (context->in_OwnerSpell->eff[0].EffectBasePoints ) / 100; 
	spell->ProcedOnSpell = context->in_CastingSpell;
	spell->pSpellId = context->in_OwnerSpell->Id;
	spell->prepare(&targets2);
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_99134( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash != SPELL_HASH_FLASH_HEAL 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_GREATER_HEAL 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_HEAL 
		&& context->in_CastingSpell->NameHash != SPELL_HASH_PRAYER_OF_MENDING 
		)
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_99158( ProcHandlerContextShare *context )
{
	if( ( context->in_events_filter_flags & PROC_ON_AURA_REMOVE ) == PROC_ON_AURA_REMOVE )
	{
		context->in_Caller->RemoveAuraByNameHash( SPELL_HASH_DARK_FLAMES );
	}
	else 
	{
		bool RequiredAuras[3];
		RequiredAuras[0] = RequiredAuras[1] = RequiredAuras[2] = false;
		if( context->in_CastingSpell->NameHash == SPELL_HASH_SHADOW_WORD__PAIN )
			RequiredAuras[0] = true;
		else if( context->in_CastingSpell->NameHash == SPELL_HASH_DEVOURING_PLAGUE )
			RequiredAuras[1] = true;
		else if( context->in_CastingSpell->NameHash == SPELL_HASH_VAMPIRIC_TOUCH )
			RequiredAuras[2] = true;
		if( RequiredAuras[0] == false )
			RequiredAuras[0] = context->in_Victim->HasAuraWithNameHash( SPELL_HASH_SHADOW_WORD__PAIN, 0, AURA_SEARCH_NEGATIVE ) != 0;
		if( RequiredAuras[1] == false )
			RequiredAuras[1] = context->in_Victim->HasAuraWithNameHash( SPELL_HASH_DEVOURING_PLAGUE, 0, AURA_SEARCH_NEGATIVE ) != 0;
		if( RequiredAuras[2] == false )
			RequiredAuras[2] = context->in_Victim->HasAuraWithNameHash( SPELL_HASH_VAMPIRIC_TOUCH, 0, AURA_SEARCH_NEGATIVE ) != 0;
		if( RequiredAuras[0] == true && RequiredAuras[1] == true && RequiredAuras[2] == true )
			context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	}
}

void PH_99173( ProcHandlerContextShare *context )
{
 	if( context->in_Caller->IsPlayer() == false )
		return; 
	if( ( context->in_dmg - context->in_abs ) <= 0 )
		return;
	context->out_dmg_overwrite[0] = ( context->in_dmg - context->in_abs ) * context->in_OwnerSpell->eff[0].EffectBasePoints / 100 / context->out_CastSpell->quick_tickcount;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_99187( ProcHandlerContextShare *context )
{
	int32 our_pick = RandomUInt() % 3;
	if( our_pick == 1 )
		context->out_CastSpell = dbcSpell.LookupEntryForced( 99186 );	//Future on Fire
	else if( our_pick == 2 )
		context->out_CastSpell = dbcSpell.LookupEntryForced( 99188 );	//Master of Flames
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_99204( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	int32 reduce_ms = context->in_OwnerSpell->eff[0].EffectBasePoints * 1000;
	SafePlayerCast( context->in_Caller )->ModCooldown( 2894, -reduce_ms, false );	//Fire elemental totem
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_105582( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	//old hp too low ?
	if( context->in_Caller->GetHealthPct() < 35 )
		return;
	int32 new_hp = context->in_Caller->GetHealth() - ( context->in_dmg - context->in_abs );
	int32 new_pct = new_hp * 100 / context->in_Caller->GetMaxHealth();
	//new HP too high ?
	if( new_pct > 35 )
		return;
	//need to trigger it 2 times
	context->in_Caller->CastSpell( context->in_Caller, context->out_CastSpell, true );
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_105647( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash == SPELL_HASH_RUNIC_EMPOWERMENT )
	{
		if( RandChance( context->in_OwnerSpell->eff[0].EffectBasePoints ) )
			context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	}
	else
	{
		if( RandChance( context->in_OwnerSpell->eff[1].EffectBasePoints ) )
			context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	}
}

void PH_105725( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash == SPELL_HASH_MANGLE
		&& ( context->in_events_filter_flags & PROC_ON_CRIT ) == PROC_ON_CRIT
		&& RandChance( context->in_OwnerSpell->eff[0].EffectBasePoints )
		&& context->in_Caller->HasAuraWithNameHash( SPELL_HASH_PULVERIZE, 0, AURA_SEARCH_POSITIVE )	)
	{
		context->out_CastSpell = dbcSpell.LookupEntryForced( 62606 );	//Savage Defense
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	}
	else if( context->in_CastingSpell->NameHash == SPELL_HASH_FEROCIOUS_BITE
		&& (int32)context->in_Victim->GetHealthPct() < context->in_OwnerSpell->eff[1].EffectBasePoints
		&& context->in_Caller->HasAuraWithNameHash( SPELL_HASH_BLOOD_IN_THE_WATER, 0, AURA_SEARCH_PASSIVE )	//this can come from T12 set bonus as well
		)
	{
		Aura *rip = context->in_Victim->HasAuraWithNameHash( SPELL_HASH_RIP, 0, AURA_SEARCH_NEGATIVE );
		if( rip )
			rip->ResetDuration();
		context->in_event->LastTrigger = getMSTime(); // consider it triggered
	}
}

void PH_105770( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	Aura *a=context->in_Victim->HasAuraWithNameHash( context->in_CastingSpell->NameHash, 0, AURA_SEARCH_VISIBLE );
	if( a )
	{
		int64 *AlreadyTriggered = a->GetCreateIn64Extension( EXTENSION_ID_T13_REJUVENATION_MARKER );
		if( *AlreadyTriggered == 0 )
		{
			a->SetDuration( a->GetDuration() * 2 );
			a->ResetDuration();
			*AlreadyTriggered = 1;
			context->in_event->LastTrigger = getMSTime(); // consider it triggered
		}
	}
}

void PH_105786( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash == SPELL_HASH_SOUL_FIRE
		&& context->in_Caller->HasAuraWithNameHash( SPELL_HASH_SOULBURN, 0, AURA_SEARCH_POSITIVE ) )
	{
		context->in_Caller->Energize( context->in_Caller, 101977, 1, POWER_TYPE_SOUL_SHARDS, 0 ); // no idea about the spell id !
		return;
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_105785( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( ( context->in_events_filter_flags & PROC_ON_CAST_SPELL ) == PROC_ON_CAST_SPELL )
	{
		uint32 chance = 100;
		if( context->in_CastingSpell->NameHash == SPELL_HASH_ARCANE_BLAST )
			chance = 1000;
		else if( context->in_CastingSpell->NameHash == SPELL_HASH_FIREBALL 
			|| context->in_CastingSpell->NameHash == SPELL_HASH_PYROBLAST 
			|| context->in_CastingSpell->NameHash == SPELL_HASH_FROSTFIRE_BOLT 
			|| context->in_CastingSpell->NameHash == SPELL_HASH_FROSTBOLT )
			chance = context->in_OwnerSpell->eff[0].EffectBasePoints;
		if( RandChance( chance ) )
		{
			context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
			if( context->in_Caller->HasAuraWithNameHash( SPELL_HASH_ITEM___MAGE_T13_4P_BONUS__ARCANE_POWER__COMBUSTION__AND_ICY_VEINS_, 0, AURA_SEARCH_PASSIVE ) )
				context->in_Caller->CastSpell( context->in_Caller,	105791, true ); // stolen time ( extended version )
		}
	}
	else
	{
		if( context->in_CastingSpell->NameHash == SPELL_HASH_ARCANE_POWER
			|| context->in_CastingSpell->NameHash == SPELL_HASH_COMBUSTION 
			|| context->in_CastingSpell->NameHash == SPELL_HASH_ICY_VEINS )
			context->in_Caller->RemoveAuraByNameHash( SPELL_HASH_STOLEN_TIME );
	}
}
/*
void PH_105860( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( ( context->in_events_filter_flags & PROC_ON_AURA_REMOVE ) != 0 )
	{
		context->in_Caller->RemoveAllAuraByNameHash( context->out_CastSpell->Id );
		return;
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}*/

void PH_108126( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->NameHash == SPELL_HASH_BLOODTHIRST )
	{
		if( RandChance( context->in_OwnerSpell->eff[0].EffectBasePoints ) )
			context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	}
	else
	{
		if( RandChance( 13 ) )
			context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	}
}

void PH_105909( ProcHandlerContextShare *context )
{
 	if( context->in_Caller->IsPlayer() == false )
		return; 
	if( ( context->in_dmg - context->in_abs ) <= 0 )
		return;
	context->out_dmg_overwrite[0] = ( context->in_dmg - context->in_abs ) * context->in_OwnerSpell->eff[0].EffectBasePoints / 100;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_105911( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	Player *p_target = SafePlayerCast( context->in_Caller );
	Group * group = p_target->GetGroup(); 
	if( group == NULL )
		return;
	uint32 count = group->GetSubGroupCount();
	// Loop through each raid group.
	group->Lock();
	for( uint8 k = 0; k < count; k++ )
	{
		SubGroup * subgroup = group->GetSubGroup( k );
		if( subgroup == NULL )
			continue;
		for( GroupMembersSet::iterator itr = subgroup->GetGroupMembersBegin(); itr != subgroup->GetGroupMembersEnd(); ++itr )
			if( (*itr)->m_loggedInPlayer && (*itr)->m_loggedInPlayer != p_target && (*itr)->m_loggedInPlayer->isAlive()	)
			{
				Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
				spell->Init( (*itr)->m_loggedInPlayer, context->in_CastingSpell ,true, NULL);
				spell->ProcedOnSpell = context->in_CastingSpell;
				spell->pSpellId=context->in_OwnerSpell->Id;
				spell->forced_basepoints[0] = context->in_CastingSpell->eff[0].EffectBasePoints * context->in_OwnerSpell->eff[0].EffectBasePoints / 100;
				spell->forced_basepoints[1] = context->in_CastingSpell->eff[1].EffectBasePoints * context->in_OwnerSpell->eff[0].EffectBasePoints / 100;
				spell->forced_basepoints[2] = context->in_CastingSpell->eff[2].EffectBasePoints * context->in_OwnerSpell->eff[0].EffectBasePoints / 100;
				SpellCastTargets targets2( (*itr)->m_loggedInPlayer->GetGUID() ); //no target so spelltargeting will get an injured party member
				spell->prepare(&targets2);
			}
	}
	group->Unlock();
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_109941( ProcHandlerContextShare *context )
{
	uint32 count = context->in_Caller->CountAura( context->out_CastSpell->Id, AURA_SEARCH_POSITIVE );
	//stacks max 50, so 100 / 20 = 5% chance per extra stack
	if( count > 30 )
	{
//		int32 proc_chance = ( count - 30 ) * 5;
		int32 proc_chance = ( count - 30 ) * 2;
		if( RandChance( proc_chance ) )
		{
			context->in_Caller->RemoveAura( context->out_CastSpell->Id, 0, AURA_SEARCH_ALL, MAX_PASSIVE_AURAS );
			context->out_CastSpell = dbcSpell.LookupEntryForced( 109949 ); //Fury of the Destroyer
		}
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_109950( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL || context->in_Caller->IsPlayer() == false )
		return;
	if( ( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_FINISHING_MOVE	) == 0 )
		return;
	//to avoid the combo points getting removed due to this proc cycle
	context->in_Caller->CastSpellDelayed( context->in_Caller->GetGUID(), context->out_CastSpell->Id, 1, true );
	//double check
/*	if( context->in_Caller->IsPlayer() )
	{
		SafePlayerCast( context->in_Caller )->m_comboPoints = context->out_CastSpell->eff[0].EffectBasePoints;
		SafePlayerCast( context->in_Caller )->UpdateComboPoints();
	}*/
//	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_105869( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( ( context->in_events_filter_flags & PROC_ON_AURA_REMOVE ) == PROC_ON_AURA_REMOVE)
	{
//		if( context->in_Caller->HasAuraWithNameHash( context->in_CastingSpell->NameHash, 0, AURA_SEARCH_POSITIVE ) == false )
			context->in_Caller->RemoveAuraByNameHash( context->out_CastSpell->NameHash, 0, AURA_SEARCH_ALL, MAX_PASSIVE_AURAS );
		return;
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_81022( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	if( context->in_CastingSpell->NameHash == SPELL_HASH_FRENZIED_REGENERATION )
	{
		Player *p_target = SafePlayerCast( context->in_Caller );
		Group * group = p_target->GetGroup(); 
		if( group == NULL )
			return;
		uint32 count = group->GetSubGroupCount();
		// Loop through each raid group.
		group->Lock();
		for( uint8 k = 0; k < count; k++ )
		{
			SubGroup * subgroup = group->GetSubGroup( k );
			if( subgroup == NULL )
				continue;
			for( GroupMembersSet::iterator itr = subgroup->GetGroupMembersBegin(); itr != subgroup->GetGroupMembersEnd(); ++itr )
				if( (*itr)->m_loggedInPlayer && (*itr)->m_loggedInPlayer != p_target && (*itr)->m_loggedInPlayer->isAlive()	)
				{
					Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
					spell->Init( (*itr)->m_loggedInPlayer, context->in_CastingSpell ,true, NULL);
					spell->ProcedOnSpell = context->in_CastingSpell;
					spell->pSpellId=context->in_OwnerSpell->Id;
					SpellCastTargets targets2( (*itr)->m_loggedInPlayer->GetGUID() ); 
					spell->prepare(&targets2);
				}
		}
		group->Unlock();
		context->in_event->LastTrigger = getMSTime(); // consider it triggered
	}
	else
	{
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	}
}

void PH_108000( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell || !( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_HEALING ) )
		return;
	uint64 new_target = Spell::FindLowestHealthRaidMember( SafePlayerCast( context->in_Caller ), 15000*15000); // within 15 yards
	if( new_target == 0 
//		|| new_target == context->in_Caller->GetGUID() 
		)	// shouldn't happen
		return;
	SpellCastTargets targets2( new_target ); //no target so spelltargeting will get an injured party member
	SpellEntry *spellInfo = dbcSpell.LookupEntry( context->in_event->spellId );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init(context->in_Caller, spellInfo ,true, NULL);
	spell->ProcedOnSpell = context->in_CastingSpell;
	spell->pSpellId = context->in_OwnerSpell->Id;
	spell->prepare(&targets2);
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_91724( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( ( context->in_events_filter_flags & PROC_ON_AURA_REMOVE ) == PROC_ON_AURA_REMOVE )
	{
//		if( context->in_Caller->HasAuraWithNameHash( context->in_CastingSpell->NameHash, 0, AURA_SEARCH_POSITIVE ) == false )
			context->in_Caller->RemoveAuraByNameHash( context->out_CastSpell->NameHash );
		return;
	}
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_109800( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( ( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_DAMAGING ) == 0 )
		return;
	SpellCastTargets targets2( context->in_Victim->GetGUID() ); 
	targets2.m_destX = context->in_Victim->GetPositionX();
	targets2.m_destY = context->in_Victim->GetPositionY();
	targets2.m_destZ = context->in_Victim->GetPositionZ();
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init( context->in_Caller, context->out_CastSpell ,true, NULL);
	spell->ProcedOnSpell = context->in_CastingSpell;
	spell->pSpellId = context->in_OwnerSpell->Id;
	spell->prepare(&targets2);
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_61882( ProcHandlerContextShare *context )
{
	//wtf ? we do not wish to dmg ourself
	if( context->in_Victim == context->in_Caller )
		return;
	//we have a chance to stun our target
	if( RandChance( 10 ) )
		context->in_Caller->CastSpell( context->in_Victim, 77505, true );
	//deal dmg
	context->in_Caller->CastSpell( context->in_Victim, 77478, true );
//	int32 dmg = context->in_dmg + context->in_Caller->GetDamageDoneMod( SCHOOL_NATURE );
/*	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init( context->in_Caller, context->in_CastingSpell ,true, NULL);
	int32 flat_tick = spell->CalculateEffect( 0, context->in_Victim );
	SpellPool.PooledDelete( spell );
	context->in_Caller->SpellNonMeleeDamageLog( context->in_Victim, context->in_OwnerSpell, context->in_dmg, false, true, false, 1, 100, 0 );
	*/
	//make DYN to not register the target !
	context->out_handler_result = PROC_HANDLER_CONTINUE_TO_NEXT;
}

void PH_54743( ProcHandlerContextShare *context )
{
	if( (int32)context->in_Victim->GetHealthPct() >= context->in_OwnerSpell->eff[0].EffectBasePoints )
		return;
	Aura *a = context->in_Victim->HasAuraWithNameHash( SPELL_HASH_REGROWTH, 0, AURA_SEARCH_POSITIVE );
	if( a && a->GetTimeLeft() <= 2000 )
		a->ResetDuration();
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_82898( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	//Disengage 
	if( ( context->in_events_filter_flags & PROC_ON_MELEE_ATTACK_VICTIM ) == PROC_ON_MELEE_ATTACK_VICTIM )
		SafePlayerCast( context->in_Caller )->ModCooldown( 781, -context->in_OwnerSpell->eff[0].EffectBasePoints, false );
	//Deterrence
	else
		SafePlayerCast( context->in_Caller )->ModCooldown( 19263, -context->in_OwnerSpell->eff[1].EffectBasePoints, false );
	//!! self proc
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_97709( ProcHandlerContextShare *context )
{
	//do not stack it !
	if( context->in_Caller->HasAura( context->out_CastSpell->Id ) )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_94339( ProcHandlerContextShare *context )
{
	//this spell has 2 ranks
	Aura *a = context->in_Caller->HasAuraWithNameHash( SPELL_HASH_FUNGAL_GROWTH, 0, AURA_SEARCH_PASSIVE );
	if( a && a->GetSpellId() == 78788 )	//lower rank
		context->in_Caller->CastSpell( context->in_Victim, 81288, true );
	else
		context->in_Caller->CastSpell( context->in_Victim, 81281, true );	//strong version
	//break execution,we made the cast ourself
	context->out_handler_result = PROC_HANDLER_CONTINUE_TO_NEXT;
}

void PH_96172( ProcHandlerContextShare *context )
{
	//proc on no ability but normal melee attack
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_Caller->IsPlayer() == false )
		return;
	int32 RealDmg = context->in_dmg - context->in_abs;
	if( RealDmg <= 0 )
		return;
	//proc chance is calculated dynamically
	int32 dmg_pct = float2int32( context->in_Caller->GetFloatValue( PLAYER_MASTERY ) * context->in_OwnerSpell->eff[1].EffectBasePoints );
	int32 dmg = RealDmg * dmg_pct / 100 / 100;
//	if( ( context->in_events_filter_flags & ( PROC_ON_CRIT ) ) == PROC_ON_CRIT )
//		dmg = dmg / 2;
	context->out_dmg_overwrite[0] = dmg;
//	context->in_Caller->SpellNonMeleeDamageLog( context->in_Victim, context->in_OwnerSpell, dmg, true, false );
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_56160( ProcHandlerContextShare *context )
{
	Aura *a = context->in_Victim->HasAura( 17 );	//Power Word: Shield
	int32 heal_amt = 0;
	if( a )	
		heal_amt = a->GetModAmount( 0 ) * context->in_OwnerSpell->eff[0].EffectBasePoints / 100;
	context->out_dmg_overwrite[0] = heal_amt;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_97985( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell->NameHash == SPELL_HASH_DASH )
	{
		context->in_Caller->RemoveAllAurasByMechanicMulti( (1<<MECHANIC_ROOTED)|(1<<MECHANIC_ENSNARED)|(1<<MECHANIC_SLOWED)|(1<<MECHANIC_DAZED), -1, true );
	}
	else //if( context->in_CastingSpell->NameHash == SPELL_HASH_STAMPEDING_ROAR )
	{
		context->in_Victim->RemoveAllAurasByMechanicMulti( (1<<MECHANIC_ROOTED)|(1<<MECHANIC_ENSNARED)|(1<<MECHANIC_SLOWED)|(1<<MECHANIC_DAZED), -1, true );
	}
	//do not consider it procced as we need to handle all the spell targets not just 1
}
/*
void PH_84674( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false ||
		SafePlayerCast( context->in_Caller )->m_currentSpellAny == NULL )
		return;

	Spell *castingspell = SafePlayerCast( context->in_Caller )->m_currentSpellAny;
	if( castingspell->GetProto()->eff[0].Effect != SPELL_EFFECT_SCHOOL_DAMAGE || castingspell->m_targetUnits[0].size() < 2 )
		return;

	SpellCastTargets targets2( context->in_Victim->GetGUID() ); 
	targets2.m_destX = castingspell->m_targets.m_destX;
	targets2.m_destY = castingspell->m_targets.m_destY;
	targets2.m_destZ = castingspell->m_targets.m_destZ;
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init( context->in_Caller, dbcSpell.LookupEntryForced( 2120 ) ,true, NULL); //Flamestrike
	spell->ProcedOnSpell = context->in_CastingSpell;
	spell->pSpellId = context->in_OwnerSpell->Id;
	spell->prepare(&targets2);

	//!! self proc
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
} */

void PH_33881( ProcHandlerContextShare *context )
{
	if( context->in_Caller->GetHealthPct() >= 50 )	//present only in rank 1
		return;
	context->out_CastSpell = dbcSpell.LookupEntryForced( 774 ); //Rejuvenation
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_35101( ProcHandlerContextShare *context )
{
	context->in_Caller->CastSpell( context->in_Victim, context->out_CastSpell, true );
	//do not mark it as procced so multi shot can daze multiple enemies at once
}

void PH_81331( ProcHandlerContextShare *context )
{
 	if( context->in_Caller->IsPlayer() == false )
		return; 
	//this needs 2H weapon
	Item* it = SafePlayerCast( context->in_Caller )->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_MAINHAND );
	if( it == NULL || it->GetProto()->InventoryType != INVTYPE_2HWEAPON )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_83582( ProcHandlerContextShare *context )
{
 	if( context->in_Caller->IsPlayer() == false )
		return; 
	//only proc on fire DOTS
	if( context->in_CastingSpell == NULL )
		return;
	if( ( context->in_CastingSpell->SchoolMask & SCHOOL_MASK_FIRE ) == 0 )
		return;
	if( context->in_CastingSpell->quick_tickcount == 1 )
		return;
	bool should_cast = false;
	//count the number of active fire dots near us
	int64 *list_len = context->in_Caller->GetCreateIn64Extension( EXTENSION_ID_PYROMANIAC_COUNTER );
	int64 ori_len = *list_len;
	int64 new_len;
	//adding it on a new target or refreshing it ?
	if( ( context->in_events_filter_flags & PROC_ON_CAST_SPELL ) == PROC_ON_CAST_SPELL )
	{
		bool have_it = false;
		for( uint32 i=0;i<*list_len;i++)
		{
			int64 *target_guid = context->in_Caller->GetCreateIn64Extension( EXTENSION_ID_PYROMANIAC_GUID_START + i );
			if( *target_guid == context->in_Victim->GetGUID() )
			{
				have_it = true;
				break;
			}
		}
		if( have_it == false )
		{
			should_cast = true;
			//try to put it in an empty slot
			bool need_new_save = true;
			for( uint32 i=0;i<*list_len;i++)
			{
				int64 *target_guid = context->in_Caller->GetCreateIn64Extension( EXTENSION_ID_PYROMANIAC_GUID_START + i );
				if( *target_guid == 0 )
				{
					*target_guid = context->in_Victim->GetGUID();
					need_new_save = false;
					break;
				}
			}
			if( need_new_save == true )
			{
				int64 *new_target_guid = context->in_Caller->GetCreateIn64Extension( EXTENSION_ID_PYROMANIAC_GUID_START + (int32)*list_len );
				if( *list_len < 200 )
					*list_len = ( *list_len + 1 );
				*new_target_guid = context->in_Victim->GetGUID();
			}
		}
	}
	else if( ( context->in_events_filter_flags & PROC_ON_AURA_REMOVE ) == PROC_ON_AURA_REMOVE )
	{
		for( uint32 i=0;i<*list_len;i++)
		{
			int64 *target_guid = context->in_Caller->GetCreateIn64Extension( EXTENSION_ID_PYROMANIAC_GUID_START + i );
			if( *target_guid == context->in_Victim->GetGUID() )
				*target_guid = 0;
		}
	}
	new_len = 0;
	for( uint32 i=0;i<*list_len;i++)
	{
		int64 *target_guid = context->in_Caller->GetCreateIn64Extension( EXTENSION_ID_PYROMANIAC_GUID_START + i );
		if( *target_guid != 0 )
			new_len++;
	}
	if( should_cast == true && new_len >= 3 ) 
	{
		context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints;
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	}
	else if( should_cast == false && new_len < 3 )
	{
		context->in_Caller->RemoveAura( context->out_CastSpell->Id );
		//no need to continue with casting
	}
}

void PH_77616( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_CastingSpell->powerType != POWER_TYPE_MANA )
		return;
	// do not steal boss spells. Right now only works on players
	if( context->in_Victim->IsPlayer() == false )
		return;
	//some strange bug to cast boss spells casted by us due to scripting. I hope proc spells will not trigger dark simulacrum :(
	if( context->in_Victim == context->in_Caller )
		return;
	SpellPowerEntry *spe = dbcSpellPower.LookupEntryForced( context->in_CastingSpell->Id );
	if( spe == NULL || ( spe->ManaCostPercentage == 0 && spe->manaCost == 0 ) )
		return;
	//actually cast it on the caster that put this aura on us
	Player *oric = context->in_Caller->GetMapMgr()->GetPlayer( context->in_event->caster );
	if( oric == NULL )
		return;
	SpellCastTargets targets2( context->in_event->caster );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	SpellEntry *spd = dbcSpell.LookupEntryForced( context->in_event->spellId );
	spell->Init( oric, spd ,true, NULL);
	spell->ProcedOnSpell = context->in_CastingSpell;
	spell->forced_basepoints[0] = context->in_CastingSpell->Id; //add this as a temp spell
	//somehow memorize the spell dmg / heal this spell would have made by the original caster
//	if( context->in_events_filter_flags & ( PROC_ON_SPELL_HIT_DOT | PROC_ON_HEAL_SPELL ) )
	if( ( context->in_CastingSpell->c_is_flags & ( SPELL_FLAG_IS_DAMAGING | SPELL_FLAG_IS_HEALING ) )
		|| ( context->in_CastingSpell->c_is_flags3 & SPELL_FLAG3_IS_SAFE_SIMULACRUM_STEAL ) )
	{
//		spell->forced_basepoints[1] = context->in_dmg; //add this as a temp spell
		Spell *tspell = SpellPool.PooledNew( __FILE__, __LINE__ );
		tspell->Init( context->in_Caller, context->in_CastingSpell ,true, NULL);
		spell->forced_basepoints[1] = tspell->CalculateEffect( 0, context->in_Victim );
		if( context->in_CastingSpell->Spell_value_is_scripted == false && IsDamagingSpell( context->in_CastingSpell, 0 ) )
			spell->forced_basepoints[1] += context->in_Caller->GetSpellDmgBonus( context->in_Victim, context->in_CastingSpell, spell->forced_basepoints[1], 0 );		
		SpellPool.PooledDelete( tspell, __FILE__, __LINE__ );
	}
	spell->pSpellId=context->in_OwnerSpell->Id;
	spell->prepare(&targets2);
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_67598( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	int32 energy_consume = context->in_Caller->GetPower( POWER_TYPE_ENERGY );
	energy_consume = ( energy_consume / context->in_OwnerSpell->eff[1].EffectBasePoints ) * context->in_OwnerSpell->eff[1].EffectBasePoints;
	//no heal for you ?
	if( energy_consume == 0 )
		return;
	int32 heal = context->in_OwnerSpell->eff[0].EffectBasePoints * energy_consume / context->in_OwnerSpell->eff[1].EffectBasePoints;
	//this is supposed to be less then 5
	if( heal > 10 )
		heal = 10;
	heal = context->in_Caller->GetMaxHealth() * heal / 100;
	context->in_Caller->ModPower( POWER_TYPE_ENERGY, -energy_consume );

	//cast a dummy spell. Note that Ferocious Bite does not heal by default
/*	SpellCastTargets targets2( context->in_Caller->GetGUID() );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	SpellEntry *spd = dbcSpell.LookupEntry( 22568 );	//Ferocious Bite
	spell->Init( context->in_Caller, spd ,true, NULL);
	spell->SetUnitTarget( context->in_Caller );
	spell->m_triggeredSpell = true;
	spell->forced_basepoints[0] = heal; 
	spell->Heal( heal, false, 1 );
	SpellPool.PooledDelete( spell ); */

	context->in_Caller->SpellHeal( context->in_Victim, dbcSpell.LookupEntry( 22568 ), heal, false, true ); //Ferocious Bite

	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_54815( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_Caller->IsPlayer() == false )
		return;
	if( SafePlayerCast( context->in_Caller )->GetShapeShift() != FORM_CAT )
		return;
	Aura *a = context->in_Victim->HasAuraWithNameHash( SPELL_HASH_RIP, 0, AURA_SEARCH_NEGATIVE );
	if( context->in_event->custom_holder != a )
	{
		context->in_event->custom_holder = a;
		context->in_event->created_with_value = 0;
	}
	context->in_event->created_with_value++;
	if( a && context->in_event->created_with_value <= context->in_OwnerSpell->eff[1].EffectBasePoints / context->in_OwnerSpell->eff[0].EffectBasePoints )
	{
		int32 dur = a->GetTimeLeft();
		int32 new_dur = dur + context->in_OwnerSpell->eff[0].EffectBasePoints * 1000;
		a->SetDuration( new_dur );
		a->ResetDuration();
	}
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_63622( ProcHandlerContextShare *context )
{
	if( ( context->in_events_filter_flags & PROC_ON_AURA_REMOVE ) == PROC_ON_AURA_REMOVE
		|| context->in_CastingSpell->NameHash == SPELL_HASH_UNHOLY_PRESENCE )
	{
		context->in_Caller->RemoveAura( context->out_CastSpell->Id );
		return;
	}
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_63621( ProcHandlerContextShare *context )
{
	if( ( context->in_events_filter_flags & PROC_ON_AURA_REMOVE ) == PROC_ON_AURA_REMOVE
		|| context->in_CastingSpell->NameHash == SPELL_HASH_FROST_PRESENCE )
	{
		context->in_Caller->RemoveAura( context->out_CastSpell->Id );
		return;
	}
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_63611( ProcHandlerContextShare *context )
{
	if( ( context->in_events_filter_flags & PROC_ON_AURA_REMOVE ) == PROC_ON_AURA_REMOVE
		|| context->in_CastingSpell->NameHash == SPELL_HASH_BLOOD_PRESENCE )
	{
		context->in_Caller->RemoveAura( context->out_CastSpell->Id );
		return;
	}
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_94006( ProcHandlerContextShare *context )
{
	if( ( context->in_events_filter_flags & PROC_ON_CRIT ) != 0 )
		context->in_event->created_with_value++;
	else
		context->in_event->created_with_value = 0;
	//only cast on second crit
	if( context->in_event->created_with_value >= 2 )
	{
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
		context->in_event->created_with_value = 0;
	}
}

void PH_96268( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	Player *p = SafePlayerCast( context->in_Caller );
	int32 OnCooldown = 0;
	int32 MaxValue = 0;
	for(int j=0;j<TOTAL_USED_RUNES;j++)
		if( p->m_rune_types[j] == RUNE_UNHOLY && p->m_runes[j] < MAX_RUNE_VALUE )
		{
			OnCooldown++;
			if( MaxValue < p->m_runes[j] )
				MaxValue = p->m_runes[j];
		}
	if( OnCooldown != 2 )
		return;
	int32 TimeRemaining = MAX_RUNE_VALUE - MaxValue; //in bytes
	TimeRemaining = TimeRemaining * 1000 / 2 * p->GetFloatValue( PLAYER_RUNE_REGEN_1 ) - 1000; //in seconds
	if( TimeRemaining < 1000 )
		return;
	//remove old aura-duration
	context->in_Caller->RemoveAura( context->out_CastSpell->Id );
	//aply a new aura
	SpellCastTargets targets( context->in_Caller->GetGUID() );
	SpellEntry *spellInfo = dbcSpell.LookupEntry( context->in_event->spellId );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init(context->in_Caller, spellInfo ,true, NULL);
	spell->forced_duration = TimeRemaining ;
	spell->forced_basepoints[0] = context->in_OwnerSpell->eff[0].EffectBasePoints + 1;
	spell->ProcedOnSpell = context->in_CastingSpell;
	spell->pSpellId=context->in_OwnerSpell->Id;
	spell->prepare(&targets);

	//get how much time remains until we rechacrge the rune
/*	{
		context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints;
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	}/**/
}

void PH_53398( ProcHandlerContextShare *context )
{
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_54825( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	SafePlayerCast( context->in_Caller )->ModCooldown( 17116, -context->in_OwnerSpell->eff[0].EffectBasePoints*1000, false );
	//!! self proc
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_67481( ProcHandlerContextShare *context )
{
	if( ( context->in_dmg - context->in_abs ) <= 0 )
		return;
	//get the real owner of this aura
	Unit *owner = context->in_Caller->GetMapMgr()->GetUnit( context->in_event->caster );
	Unit *Pet = NULL;
	if( owner && owner->IsInWorld() && owner->isAlive() )
	{
		if( owner->IsPet() )
			Pet = owner;
		else if( owner->IsPlayer() && SafePlayerCast( owner )->GetSummon() )
			Pet = SafePlayerCast( owner )->GetSummon();
	}
	if( Pet )
	{
		Spell* spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		spell->Init( Pet, context->out_CastSpell, true, NULL );
		SpellCastTargets targets2( Pet->GetGUID() );
		spell->forced_basepoints[0] = MAX( 1, context->in_OwnerSpell->eff[1].EffectBasePoints * ( context->in_dmg - context->in_abs ) / 100 );
		spell->prepare( &targets2 );
		context->in_event->LastTrigger = getMSTime(); // consider it triggered
	}
}

void PH_70893( ProcHandlerContextShare *context )
{
	//we need basic attack
	if( context->in_CastingSpell != NULL )
		return;
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_29341( ProcHandlerContextShare *context )
{
	//get the real owner of this aura
	Unit *owner = context->in_Caller->GetMapMgr()->GetUnit( context->in_event->caster );
	if( owner && owner->IsInWorld() && owner->isAlive() )
		owner->Energize( owner, context->out_CastSpell->Id, 3, POWER_TYPE_SOUL_SHARDS, 0 );
	//!! self proc, no continue
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_56229( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	//only proc if target is below x% health. Useless since shadowburn already has this cast condition
	if( (int32)context->in_Victim->GetHealthPct() > context->in_OwnerSpell->eff[0].EffectBasePoints )
		return;
	//check the unit we casted shadowburn before
	Unit *prev_taget = context->in_Caller->GetMapMgr()->GetUnit( (uint64)context->in_event->created_with_value );
	context->in_event->created_with_value = (int64)context->in_Victim->GetGUID();
	//is this the first cast of shadowburn ?
	if( prev_taget == NULL || prev_taget->isAlive() == false )
	{
		SafePlayerCast( context->in_Caller )->ClearCooldownForSpell( 17877 ); //Shadowburn
	}
	//this is second cast and it seems target still did not die ? The we put this proc on cooldown
	else 
	{
		//reset counter and add cooldown
		context->in_event->created_with_value = 0;
		//!! self proc, no continue
		context->in_event->LastTrigger = getMSTime(); // consider it triggered
	}
	//filter out even call for aura target
//	if( context->in_Caller->GetGUID() != context->in_event->caster )
//		return;
	//clear cooldown for shadowburn
//	if( context->in_events_filter_flags & PROC_ON_CAST_SPELL )
//	{
//	}
//	else
/*	{
		//proc if we failed to kill the target
		if( context->in_Victim->isAlive() == false )
			return;
		//!! self proc, no continue
		context->in_event->LastTrigger = getMSTime(); // consider it triggered
	}*/
}

void PH_63067( ProcHandlerContextShare *context )
{
	if( (int32)context->in_Victim->GetHealthPct() >= context->in_OwnerSpell->eff[0].EffectBasePoints )
		return;
	//this is "pre dmg" proc handling :( = will always fail
	if( context->in_Victim->isAlive() == false )
		return;
	//he will most probably die
	if( (int32)context->in_Victim->GetHealth() < ( context->in_dmg - context->in_abs ) )
		return;
	SafePlayerCast( context->in_Caller )->ClearCooldownForSpell( 53351 ); //Kill Shot
	//!! self proc, no continue
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_81141( ProcHandlerContextShare *context )
{
	if( context->in_Victim->HasAuraWithNameHash( SPELL_HASH_BLOOD_PLAGUE, 0, AURA_SEARCH_NEGATIVE ) == NULL )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_65264( ProcHandlerContextShare *context )
{
	//dispell flame shock
	if( context->in_dmg != 8050 )
		return;
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_34936( ProcHandlerContextShare *context )
{
	//idiot nerf to not get bugreports about it : While casting these spells and talent procs it gets removed without having it's effect. Now it does not proc
	if( context->in_Caller->isCasting() )
	{
		SpellEntry * sp = context->in_Caller->GetCurrentSpell()->GetProto();
		if( sp->NameHash == SPELL_HASH_SHADOW_BOLT || sp->NameHash == SPELL_HASH_INCINERATE )
			return;
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_23880( ProcHandlerContextShare *context )
{
	context->out_dmg_overwrite[0] = context->in_Caller->GetMaxHealth() * context->in_OwnerSpell->eff[1].EffectBasePoints / 1000 / 100;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_97618( ProcHandlerContextShare *context )
{
	if( context->in_Victim->isCasting() == false )
		return;
	//get the interrupted spell school
	int32 SchoolMask = context->in_Victim->GetCurrentSpell()->GetProto()->SchoolMask;
	if( SchoolMask & SCHOOL_MASK_FIRE )
		context->out_CastSpell = dbcSpell.LookupEntryForced( 97618 );
	else if( SchoolMask & SCHOOL_MASK_NATURE )
		context->out_CastSpell = dbcSpell.LookupEntryForced( 97620 );
	else if( SchoolMask & SCHOOL_MASK_FROST )
		context->out_CastSpell = dbcSpell.LookupEntryForced( 97619 );
	else if( SchoolMask & SCHOOL_MASK_SHADOW )
		context->out_CastSpell = dbcSpell.LookupEntryForced( 97622 );
	else if( SchoolMask & SCHOOL_MASK_ARCANE )
		context->out_CastSpell = dbcSpell.LookupEntryForced( 97621 );
	else
		return;

	uint32 lvl = context->in_Caller->getLevel();
	int32 res;
	if( lvl < 70 )
		res = lvl;
	else if( lvl < 80 )
		res = lvl + ( lvl - 70 ) * 5 ;
	else 
		res = lvl + ( lvl - 70 ) * 5 + ( lvl - 80 ) * 7;

	context->out_dmg_overwrite[0] = res / context->in_OwnerSpell->eff[0].EffectBasePoints;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_81262( ProcHandlerContextShare *context )
{
	//The dynamic object just caled us
/*	if( context->in_event == NULL )
	{
		int64 *p = context->in_Caller->GetCreateIn64Extension( SPELL_HASH_EFFLORESCENCE );
		int32 heal_amt = (int32)*p;
		SpellEntry *spInfo = dbcSpell.LookupEntryForced( 81269 );
		Spell*sp=SpellPool.PooledNew( __FILE__, __LINE__ );
		sp->Init( context->in_Victim ,spInfo,true,NULL);
		sp->forced_basepoints[0] = heal_amt;
		SpellCastTargets tgt;
		tgt.m_unitTarget= context->in_Victim->GetGUID();
		sp->prepare(&tgt);
		context->out_handler_result = PROC_HANDLER_CONTINUE_TO_NEXT;
		return;
	}*/
	if( ( context->in_dmg - context->in_abs ) <= 0 )
		return;
	context->out_dmg_overwrite[0] = ( context->in_OwnerSpell->eff[0].EffectBasePoints ) * (context->in_dmg - context->in_abs) / 100;
	//let's try to avoid bullshit values
	if( context->out_dmg_overwrite[0] <= 0 || context->out_dmg_overwrite[0] > (int32)context->in_Caller->GetMaxHealth() )
		return;
//	int64 *p = context->in_Caller->GetCreateIn64Extension( SPELL_HASH_EFFLORESCENCE );
//	*p = context->out_dmg_overwrite[0];
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_105996( ProcHandlerContextShare *context )
{
	if( ( context->in_dmg - context->in_abs ) <= 0 )
		return;
	if( context->in_CastingSpell == NULL )
		return;
	//we need direct heal spell = no amplitude presence
	if( context->in_CastingSpell->quick_tickcount > 1 )
		return;
	//duplicate the direc heal spell amount and distribute it among players in range
	InRangePlayerSetRecProt Targets;
	InRangePlayerSetRecProt::iterator itr2;
	context->in_Caller->AquireInrangeLock(); //make sure to release lock before exit function !
	InrangeLoopExitAutoCallback AutoLock;
	for( itr2 = context->in_Caller->GetInRangePlayerSetBegin( AutoLock ); itr2 != context->in_Caller->GetInRangePlayerSetEnd(); itr2++)
	{
		float dist =  (*itr2)->GetDistance2dSq( context->in_Caller );
		if( dist <= (50.0f*50.0f) )
			Targets.insert( (*itr2) );
	}
	context->in_Caller->ReleaseInrangeLock();
	if( context->in_Caller->IsPlayer() )
		Targets.insert( SafePlayerCast( context->in_Caller ) );
	uint32 TargetsCount = (uint32)Targets.size();
	if( TargetsCount > 0 )
	{
		uint32 HealAmt = ( context->in_dmg - context->in_abs ) / TargetsCount;
		for( itr2 = Targets.begin(); itr2 != Targets.end(); itr2++)
			context->in_Caller->Heal( (*itr2), context->out_CastSpell->Id, HealAmt );
	}
}

void PH_105992( ProcHandlerContextShare *context )
{
	//we need a dmg that will kill us
 	if( ( context->in_dmg - context->in_abs ) < (int32)context->in_Caller->GetUInt32Value( UNIT_FIELD_HEALTH ) )
		return;
	context->out_dmg_absorb += context->in_dmg + 1;	//absorb all
	//heal ourself to X% of max health
	int32 heal_ammt = context->in_Caller->GetUInt32Value( UNIT_FIELD_MAXHEALTH );
	heal_ammt -= context->in_Caller->GetInt32Value( UNIT_FIELD_HEALTH );
	if( heal_ammt <= 0 )
		heal_ammt = 1;
//	context->in_Caller->Heal( context->in_Caller, context->out_CastSpell->Id, heal_ammt );
	context->out_dmg_overwrite[0] = heal_ammt;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_109445( ProcHandlerContextShare *context )
{
	context->in_Caller->CastSpell( context->in_Victim, context->in_CastingSpell->eff[0].EffectTriggerSpell, true );
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_110215( ProcHandlerContextShare *context )
{
	//what ?
	if( context->in_abs <= 0  )
		return;
	context->out_dmg_overwrite[0] = context->in_abs + context->in_dmg;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_56851( ProcHandlerContextShare *context )
{
	//switching specs
	if( context->in_Victim == context->in_Caller )
		return;
	if( ( context->in_events_filter_flags & PROC_ON_AURA_REMOVE ) == PROC_ON_AURA_REMOVE )
	{
		context->in_Victim->m_maxSpeed = 0;
	}
	else
	{
		float OriSpeed = context->in_Victim->m_base_runSpeed * context->in_OwnerSpell->eff[0].EffectBasePoints / 100;
//		float CurSpeed = context->in_Victim->m_maxSpeed * context->in_OwnerSpell->eff[0].EffectBasePoints / 100;
//		context->in_event->created_with_value = MAX( OriSpeed, CurSpeed); //always mod with flat values
		context->in_Victim->m_maxSpeed = OriSpeed;	//decrease max speed ( base value is 0). This should be alist and pick min from the list
	}
	context->in_Victim->UpdateSpeed();
	//self proc !
}

void PH_48153( ProcHandlerContextShare *context )
{
	//we need a dmg that will kill us
	if( context->in_dmg - context->in_abs < (int32)context->in_Caller->GetUInt32Value( UNIT_FIELD_HEALTH ) )
		return;
	context->out_dmg_absorb += context->in_dmg + 1;	//absorb all
	//of $s2% of their maximum health
	int32 heal_ammt = context->in_Caller->GetUInt32Value( UNIT_FIELD_MAXHEALTH ) * context->in_OwnerSpell->eff[1].EffectBasePoints / 100;
	//however this should be after removign Guardian Spirit which already boosts healing by 60%
	heal_ammt = heal_ammt * 100 / ( 100 + context->in_OwnerSpell->eff[0].EffectBasePoints );
//	heal_ammt -= context->in_Caller->GetInt32Value( UNIT_FIELD_HEALTH );
	if( heal_ammt <= 0 )
		heal_ammt = 1;
//	context->in_Caller->Heal( context->in_Caller, context->out_CastSpell->Id, heal_ammt );
	context->out_dmg_overwrite[0] = heal_ammt;
	context->in_event->deleted = true; //proc once 
	//remove the aura in next step
	Aura *a = context->in_Caller->HasAura( context->in_OwnerSpell->Id, 0, AURA_SEARCH_POSITIVE );
	if( a )
		a->RemoveDelayed();
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_73757( ProcHandlerContextShare *context )
{
	if( context->in_OwnerSpell->NameHash == SPELL_HASH_GLYPH_OF_BLURRED_SPEED )
	{
		if( ( context->in_events_filter_flags & PROC_ON_AURA_REMOVE ) == PROC_ON_AURA_REMOVE )
		{
			context->in_Caller->RemoveAura( context->out_CastSpell->Id );
			return;
		}
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	}
}

void PH_56838( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell && context->in_CastingSpell->NameHash == SPELL_HASH_FESTERING_STRIKE )
		context->in_Caller->CastSpell( context->in_Caller, context->in_OwnerSpell->eff[1].EffectTriggerSpell, true );
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_56372( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() )
		SafePlayerCast( context->in_Caller )->ClearCooldownForSpell( 122 ); //frost nova
	//!self proc
}

void PH_74208( ProcHandlerContextShare *context )
{
	if( context->in_Victim->IsPlayer() )
		return;
	if( context->in_Victim->IsCreature() )
	{
		CreatureInfo *ci = SafeCreatureCast( context->in_Victim )->GetCreatureInfo();
		if( ci != NULL && ci->Type == ELEMENTAL )
			context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	}
}

void PH_50725( ProcHandlerContextShare *context )
{
	//get the owner of the Vigilance Aura
	Player *p = context->in_Caller->GetMapMgr()->GetPlayer( context->in_event->caster );
	if( p )
	{
		p->ClearCooldownForSpell( 355 ); //Taunt
		//proc vengeance on us as if %d dmg was done to us
		ProcHandlerContextShare Tcontext( *context );
		Tcontext.in_dmg = Tcontext.in_dmg * Tcontext.in_OwnerSpell->eff[1].EffectBasePoints / 100;
		Tcontext.in_OwnerSpell = dbcSpell.LookupEntryForced( 93098 ); //Vengeance
		Tcontext.in_Caller = p;
		PH_76691( &Tcontext );
		if( Tcontext.out_handler_result == PROC_HANDLER_CONTINUE_EXECUTION )
		{
			SpellCastTargets targets( p->GetGUID() );
			Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
			spell->Init( p, Tcontext.out_CastSpell ,true, NULL);
			spell->forced_basepoints[0] = Tcontext.out_dmg_overwrite[ 0 ];
			spell->ProcedOnSpell = dbcSpell.LookupEntryForced( 50725 );
			spell->pSpellId=50725;
			spell->prepare(&targets);
		}
	}
}

void PH_65156( ProcHandlerContextShare *context )
{
	//charge and intercept share cooldown
	if( context->in_Caller->IsPlayer() == false )
		return;
	if( context->in_CastingSpell == NULL )
		return;
	Player *p = SafePlayerCast( context->in_Caller );
	int64 InterceptCategory = dbcSpell.LookupEntryForced( 20252 )->Category;
	int64 ChargeCategory = dbcSpell.LookupEntryForced( 100 )->Category;
	int64 InterceptCooldown = 0;
	int64 ChargeCooldown = 0;
	PlayerCooldownMap::iterator itr;
	for( itr = p->m_cooldownMap[COOLDOWN_TYPE_CATEGORY].begin(); itr != p->m_cooldownMap[COOLDOWN_TYPE_CATEGORY].end(); itr++)
	{
		if( itr->first == ChargeCategory )
			ChargeCooldown = MAX( ChargeCooldown, (int64)itr->second.ExpireTime - (int64)getMSTime() );
		if( itr->first == InterceptCategory )
			InterceptCooldown = MAX( InterceptCooldown, (int64)itr->second.ExpireTime - (int64)getMSTime() );
	}
	for( itr = p->m_cooldownMap[COOLDOWN_TYPE_SPELL].begin(); itr != p->m_cooldownMap[COOLDOWN_TYPE_SPELL].end(); itr++)
	{
		if( itr->first == 100 )
			ChargeCooldown = MAX( ChargeCooldown, (int64)itr->second.ExpireTime - (int64)getMSTime() );
		if( itr->first == 20252 )
			InterceptCooldown = MAX( InterceptCooldown, (int64)itr->second.ExpireTime - (int64)getMSTime() );
	}
	ChargeCooldown = MIN( ChargeCooldown, 30000 );
	InterceptCooldown = MIN( InterceptCooldown, 30000 );
	if( context->in_CastingSpell->NameHash == SPELL_HASH_INTERCEPT )
	{
		int64 AddToCharge = InterceptCooldown - ChargeCooldown; 
		if( AddToCharge )
			p->ModCooldown( 100, AddToCharge, true ); //charge
	}
	else
	{
		int64 AddToIntercept = ChargeCooldown - InterceptCooldown; 
		if( AddToIntercept > 0 )
			p->ModCooldown( 20252, AddToIntercept, true ); //intercept
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	}
}

void PH_54833( ProcHandlerContextShare *context )
{
	if( context->in_Caller == context->in_Victim )
		return;
	if( isAttackable( context->in_Caller, context->in_Victim ) )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_63058( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	if( ( context->in_events_filter_flags & PROC_ON_AURA_REMOVE ) == PROC_ON_AURA_REMOVE )
	{
//		for(uint32 x=0;x<SCHOOL_COUNT;x++)
//			context->in_Caller->AttackerCritChanceMod[x] += context->in_OwnerSpell->eff[0].EffectBasePoints;	
		context->in_Caller->RemoveAura( 63058, 0, AURA_SEARCH_POSITIVE );
	}
	else
	{
//		for(uint32 x=0;x<SCHOOL_COUNT;x++)
//			context->in_Caller->AttackerCritChanceMod[x] -= context->in_OwnerSpell->eff[0].EffectBasePoints;	
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	}
}

void PH_83676( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	if( ( context->in_events_filter_flags & PROC_ON_AURA_REMOVE ) == PROC_ON_AURA_REMOVE )
	{
		context->in_Victim->RemoveAura( context->out_CastSpell->Id );
	}
	else
	{
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	}
}

void PH_85106( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell != NULL && context->in_Caller->IsPlayer() )
	{
		if( context->in_CastingSpell->NameHash == SPELL_HASH_SHADOW_BOLT ||
			context->in_CastingSpell->NameHash == SPELL_HASH_HAND_OF_GUL_DAN ||
			context->in_CastingSpell->NameHash == SPELL_HASH_SOUL_FIRE ||
			context->in_CastingSpell->NameHash == SPELL_HASH_INCINERATE 
			)
		{
			//Metamorphosis - Demon Form
			SafePlayerCast( context->in_Caller )->ModCooldown( 47241, -context->in_OwnerSpell->eff[1].EffectBasePoints * 1000, false );
			context->in_event->LastTrigger = getMSTime(); // consider it triggered
		}
	}
	//!self proc !
}

void PH_32554( ProcHandlerContextShare *context )
{
	//the aura on player boosts pet
	if( context->in_Caller->IsPlayer() && context->in_CastingSpell != NULL && context->in_CastingSpell->NameHash == SPELL_HASH_LIFE_TAP && SafePlayerCast( context->in_Caller )->GetSummon() )
	{
		SpellCastTargets targets( SafePlayerCast( context->in_Caller )->GetSummon()->GetGUID() );
		Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		spell->Init( context->in_Caller, context->out_CastSpell ,true, NULL);
		// abs dmg in this only case is actually restored mana !
		spell->forced_basepoints[0] = context->in_abs * context->in_OwnerSpell->eff[0].EffectBasePoints / 100;
		spell->ProcedOnSpell = context->in_OwnerSpell;
		spell->pSpellId=context->in_OwnerSpell->Id;
		spell->prepare(&targets);
	}
	//the aura on pet boosts pet owner
	else if( context->in_CastingSpell == NULL && context->in_Caller->IsPet() && SafePetCast( context->in_Caller )->GetPetOwner() )
	{
		SpellCastTargets targets( SafePetCast( context->in_Caller )->GetPetOwner()->GetGUID() );
		Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		spell->Init( context->in_Caller, context->out_CastSpell ,true, NULL);
		// abs dmg in this only case is actually restored mana !
		spell->forced_basepoints[0] = SafePetCast( context->in_Caller )->GetPetOwner()->GetMaxPower( POWER_TYPE_MANA ) * context->in_OwnerSpell->eff[2].EffectBasePoints / 100;
		spell->ProcedOnSpell = context->in_OwnerSpell;
		spell->pSpellId=context->in_OwnerSpell->Id;
		spell->prepare(&targets);
	}
}

void PH_81301( ProcHandlerContextShare *context )
{
	//target needs to hold XP or honor. Kinda quick test
	if( TargetGivesXPOrHonor( context->in_Victim, context->in_Caller ) == false )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_55685( ProcHandlerContextShare *context )
{
	//increase heal amount of initial tick by x%
	*context->in_dmg_loc += *context->in_dmg_loc * context->in_OwnerSpell->eff[0].EffectBasePoints / 100;
	//self proc !
}

void PH_90811( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false  )
		return;
	//when you heal other
	if( context->in_Caller == context->in_Victim )
		return;
	*context->in_dmg_loc += *context->in_dmg_loc * context->in_OwnerSpell->eff[0].EffectBasePoints / 100;
	uint32 HolyPowerSpent = SafePlayerCast( context->in_Caller )->LastSpellCost;
	HolyPowerSpent = MAX( 1, MIN( HolyPowerSpent, 3 ) );
	context->out_dmg_overwrite[0] = HolyPowerSpent * context->in_OwnerSpell->eff[1].EffectBasePoints;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_56805( ProcHandlerContextShare *context )
{
	//we should somehow detect what is proccing us
	SafePlayerCast( context->in_Caller )->ModCooldown( 1766, -context->in_OwnerSpell->eff[1].EffectBasePoints, false ); //kick
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_24406( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	Unit *ut = SafePlayerCast( context->in_Caller )->GetSummon();
	if( ut == NULL )
		return;
	//dispell : DISPEL_MAGIC, DISPEL_CURSE
	ut->CastSpell( ut, context->out_CastSpell, true );
	//dispell : DISPEL_DISEASE, DISPEL_POISON
	SpellCastTargets targets( ut->GetGUID() );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init( ut, context->out_CastSpell ,true, NULL);
	spell->forced_miscvalues[0] = DISPEL_DISEASE;
	spell->forced_miscvalues[1] = DISPEL_POISON;
	spell->ProcedOnSpell = context->in_OwnerSpell;
	spell->pSpellId=context->in_OwnerSpell->Id;
	spell->prepare(&targets);
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}
/*
void PH_53300( ProcHandlerContextShare *context )
{
	if( ( context->in_events_filter_flags & PROC_ON_AURA_REMOVE ) == PROC_ON_AURA_REMOVE )
	{
		if( context->in_Caller->GetGUID() == context->in_event->caster )
			context->in_Victim->RemoveAura( context->out_CastSpell->Id, context->in_event->caster, AURA_SEARCH_POSITIVE );
	}
	else
	{
		context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[0].EffectBasePoints;
		context->out_dmg_overwrite[1] = context->in_OwnerSpell->eff[0].EffectBasePoints;
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	}
} */

void PH_83495( ProcHandlerContextShare *context )
{
	bool apply;
	if( ( context->in_events_filter_flags & PROC_ON_AURA_REMOVE ) == PROC_ON_AURA_REMOVE )
		apply = false;
	else
		apply = true;
	context->in_Caller->ModSpellReflectList( context->in_OwnerSpell->Id, context->in_OwnerSpell->eff[0].EffectBasePoints, -1, 0x00FFFFFF, apply );
	//self proc !
}

void PH_74434( ProcHandlerContextShare *context )
{
	if( !context->in_CastingSpell )
		return;
	if( context->in_CastingSpell->NameHash == SPELL_HASH_SEARING_PAIN )
	{
		int64 *data;
		data = context->in_Caller->GetCreateIn64Extension( EXTENSION_ID_ONETIME_TARGETSPECIFIC_CRIT_MOD_TARGET );
		*data = context->in_Victim->GetGUID();
		data = context->in_Caller->GetCreateIn64Extension( EXTENSION_ID_ONETIME_TARGETSPECIFIC_CRIT_MOD_SPELL );
		*data = 5676; //searing pain
		data = context->in_Caller->GetCreateIn64Extension( EXTENSION_ID_ONETIME_TARGETSPECIFIC_CRIT_MOD_VAL );
		*data = 100;
		context->out_CastSpell = dbcSpell.LookupEntryForced( 79440 );
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	}
	//self proc !
}

void PH_92931( ProcHandlerContextShare *context )
{
	if( context->in_Victim->GetHealthPct() > 25 )
		return;
	Aura *a = context->in_Victim->HasAuraWithNameHash( SPELL_HASH_UNSTABLE_AFFLICTION );
	if( a )
		a->ResetDuration();
	//self proc !
}

void PH_77221( ProcHandlerContextShare *context )
{
	//we need THE metamorphosis spell
	if( context->in_CastingSpell == NULL || context->in_CastingSpell->Id != 47241 )
		return;
	if( ( context->in_events_filter_flags & PROC_ON_AURA_REMOVE ) == PROC_ON_AURA_REMOVE )
	{
		context->in_Caller->RemoveAura( context->out_CastSpell->Id, 0, AURA_SEARCH_PASSIVE, 1 );
		return;
	}
	if( context->in_Caller->IsPlayer() == false )
		return;
	//proc chance is calculated dynamically
	int32 var_inc = float2int32( context->in_Caller->GetFloatValue( PLAYER_MASTERY ) * context->in_OwnerSpell->eff[2].EffectBasePoints );
	var_inc = var_inc / 100;
	context->out_dmg_overwrite[0] = var_inc;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_58686( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	Player *p = SafePlayerCast( context->in_Caller );
	uint32 StrengthBonus = p->GetStat( STRENGTH ) * context->in_OwnerSpell->eff[0].EffectBasePoints / 100;
	uint32 StaminaBonus = p->GetStat( STAMINA ) * context->in_OwnerSpell->eff[0].EffectBasePoints / 100;
	uint32 APBonus = StrengthBonus * 2;
	uint32 DMGBonus = APBonus / 14;
	if( p->GetSummon() )
	{
		Pet *pet = p->GetSummon();
		pet->BaseStats[STAT_STRENGTH] += StrengthBonus;
		pet->CalcStat( STAT_STRENGTH );
		pet->BaseStats[STAT_STAMINA] += StaminaBonus;
		pet->CalcStat( STAT_STAMINA );
		pet->BaseDamage[0] += DMGBonus;
		pet->BaseDamage[1] += DMGBonus;
//		pet->OwnerAPContribPCT += pet->OwnerAPContribPCT * context->in_OwnerSpell->eff[0].EffectBasePoints / 100;
		pet->OwnerAPContribPCT += context->in_OwnerSpell->eff[0].EffectBasePoints;
		pet->CalcDamage();
	}
	else
	{
		std::list<uint64>::iterator gitr;
		for( gitr = p->m_guardians.begin(); gitr != p->m_guardians.end(); gitr++ )
		{
			Creature *tc = p->GetMapMgr()->GetCreature( (*gitr) );
			if( tc && tc->GetEntry() == 26125 )
			{
				tc->ModUnsigned32Value( UNIT_FIELD_MAXHEALTH, StaminaBonus * 10 );
				tc->ModUnsigned32Value( UNIT_FIELD_HEALTH, StaminaBonus * 10 );
				tc->SetFloatValue( UNIT_FIELD_MINDAMAGE, tc->GetFloatValue( UNIT_FIELD_MINDAMAGE) + DMGBonus );
				tc->SetFloatValue( UNIT_FIELD_MAXDAMAGE, tc->GetFloatValue( UNIT_FIELD_MAXDAMAGE) + DMGBonus );
			}
		}
	}
	//self proc, no continue !
}

void PH_96206( ProcHandlerContextShare *context )
{
	int64 *TargetCounter = context->in_Caller->GetCreateIn64Extension( EXTENSION_ID_NATURESBOUNTY_COUNTER );
	if( ( context->in_events_filter_flags & PROC_ON_AURA_REMOVE_EVENT ) == PROC_ON_AURA_REMOVE_EVENT )
	{
		for(uint32 i=0;i<*TargetCounter;i++)
		{
			int64 *ThisTarget = context->in_Caller->GetCreateIn64Extension( EXTENSION_ID_NATURESBOUNTY_GUID_START + i );
			if( *ThisTarget == context->in_Victim->GetGUID() )
			{
				*ThisTarget = 0;
				break;
			}
		}
	}
	else
	{
		//do we have this target already ?
		bool SkippAdd = false;
		for(uint32 i=0;i<*TargetCounter;i++)
		{
			int64 *ThisTarget = context->in_Caller->GetCreateIn64Extension( EXTENSION_ID_NATURESBOUNTY_GUID_START + i );
			if( *ThisTarget == context->in_Victim->GetGUID() )
			{
				SkippAdd = true;
				break;
			}
		}
		if( SkippAdd == false )
		{
			bool AddedToEmptySlot = false;
			for(uint32 i=0;i<*TargetCounter;i++)
			{
				int64 *ThisTarget = context->in_Caller->GetCreateIn64Extension( EXTENSION_ID_NATURESBOUNTY_GUID_START + i );
				if( *ThisTarget == 0 )
				{
					*ThisTarget = context->in_Victim->GetGUID();
					AddedToEmptySlot = true;
					break;
				}
			}
			if( AddedToEmptySlot == false )
			{
				int64 *ThisTarget = context->in_Caller->GetCreateIn64Extension( EXTENSION_ID_NATURESBOUNTY_GUID_START + *TargetCounter );
				*ThisTarget = context->in_Victim->GetGUID();
				if( *TargetCounter < 200 )
					*TargetCounter += 1;
			}
		}
	}
	//count the number of targets right now
	uint32 CountedActive = 0;
	for(uint32 i=0;i<*TargetCounter;i++)
	{
		int64 *ThisTarget = context->in_Caller->GetCreateIn64Extension( EXTENSION_ID_NATURESBOUNTY_GUID_START + i );
		if( *ThisTarget != 0 )
			CountedActive++;
	}

	if( CountedActive >= 3 && ( context->in_events_filter_flags & PROC_ON_CAST_SPELL_EVENT ) )
	{
		context->out_dmg_overwrite[0] = -context->in_OwnerSpell->eff[1].EffectBasePoints;
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
	}
	else if( CountedActive < 3 && ( context->in_events_filter_flags & PROC_ON_AURA_REMOVE_EVENT ) )
		context->in_Caller->RemoveAura( context->out_CastSpell->Id, 0, AURA_SEARCH_POSITIVE, MAX_AURAS );
}

void PH_97954( ProcHandlerContextShare *context )
{
	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[2].EffectBasePoints;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_20240( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsCrowdControlledNoAttack() )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_53476( ProcHandlerContextShare *context )
{
	//get removed on direct attack
	bool NeedRemove = false;
	if( context->in_CastingSpell == NULL  //not spell
		|| (
			// context->in_CastingSpell->quick_tickcount <= 1 
			( context->in_events_filter_flags & PROC_ON_DOT ) == 0
			&& ( context->in_CastingSpell->c_is_flags2 & SPELL_FLAG2_IS_ENEMY_AOE_TARGETTING ) == 0 ) //spell but not AOE or DOT
		)
	{
		NeedRemove = true;
	}
	//backup. but why ?
//	if( context->in_event->custom_holder == NULL )
//		context->in_event->custom_holder = (void*)context->in_event->created_with_value;
	int32 DMGToBeDone = MAX( 0, context->in_dmg - context->in_abs );
	//can we still absorb this ?
	if( context->in_event->created_with_value < DMGToBeDone )
	{
		NeedRemove = true;
	}
	int32 CanAbsorbDMG = MIN( DMGToBeDone, context->in_event->created_with_value );
	context->in_event->created_with_value -= CanAbsorbDMG;
	context->out_dmg_absorb += CanAbsorbDMG;
//	*context->in_abs_loc += DMGToBeDone; //should be enough, maybe above ?
	if( NeedRemove == true )
		context->in_Caller->RemoveAura( context->out_CastSpell->Id, 0, AURA_SEARCH_POSITIVE, MAX_AURAS );
}

void PH_95746( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	Player *p_caster = SafePlayerCast( context->in_Caller );
	if( ( p_caster->m_eclipsebar_state & ECLIPSE_BAR_MOVING_TO_LUNAR ) && p_caster->HasAura( 48517 ) == NULL )
		p_caster->Energize( p_caster, context->out_CastSpell->Id, -context->in_OwnerSpell->eff[0].EffectBasePoints, POWER_TYPE_ECLIPSE, 0 ); 
	if( ( p_caster->m_eclipsebar_state & ECLIPSE_BAR_MOVING_TO_SOLAR ) && p_caster->HasAura( 48518 ) == NULL )
		p_caster->Energize( p_caster, context->out_CastSpell->Id, context->in_OwnerSpell->eff[1].EffectBasePoints, POWER_TYPE_ECLIPSE, 0 ); 
	//we handled this
	context->in_event->LastTrigger = getMSTime(); // consider it triggered
}

void PH_53386( ProcHandlerContextShare *context )
{
	//only proc when we swing our weapon
	if( ( context->in_events_filter_flags & PROC_ON_MELEE_ATTACK_EVENT ) == 0 )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_96219( ProcHandlerContextShare *context )
{
	if( context->in_Caller != context->in_Victim )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_49088( ProcHandlerContextShare *context )
{
	if( context->in_abs <= 0 )
		return;
	uint32 HealthPCTMod = 50 + context->in_OwnerSpell->eff[0].EffectBasePoints;
	uint32 TotalPowerToGive = HealthPCTMod * 10;
	uint32 FullShield = context->in_Caller->GetMaxHealth() * HealthPCTMod / 100;
	uint32 ShieldNow = 0;
	CommitPointerListNode<Absorb> *i;
	context->in_Caller->Absorbs.BeginLoop();
	for( i = context->in_Caller->Absorbs.begin(); i != context->in_Caller->Absorbs.end(); i = i->Next() )
	{
		Absorb *tabs = i->data;
		if( tabs->spellid == 48707 ) //Anti-Magic Shell 
		{
			ShieldNow += tabs->amt;
			if( context->in_event->custom_holder != tabs || context->in_event->created_with_value < ShieldNow )
			{
				context->in_event->custom_holder = tabs;
				context->in_event->created_with_value = FullShield;
			}
		}
	}
	context->in_Caller->Absorbs.EndLoopAndCommit();

	int32 PrevShieldValue = context->in_event->created_with_value;
	int32 ShieldChangeSinceLastTime = PrevShieldValue - ShieldNow;
	context->in_event->created_with_value = ShieldNow;

	if( ShieldNow <= 0 )
		return;

	uint32 ShieldMissingPCT = ShieldChangeSinceLastTime * 100 / FullShield;

	context->out_dmg_overwrite[0] = TotalPowerToGive * ShieldMissingPCT / 100; //no idea about the value :(
	//maybe simply not proc it ? what about DOTs ?
	if( context->out_dmg_overwrite[0] < 10 )
	{
		context->out_dmg_overwrite[0] = 10;
		context->in_event->created_with_value = PrevShieldValue; //restore to not slowly darin it
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_20424( ProcHandlerContextShare *context )
{
	//proc when the seals proc and not when the seals are casted
//	if( context->in_CastingSpell != NULL || ( context->in_CastingSpell->c_is_flags & SPELL_FLAG_IS_PROC_TRIGGER_PROC ) == 0 )
//		return;
/*	if( ( context->in_events_filter_flags & PROC_ON_CAST_SPELL_EVENT ) != 0 && context->in_CastingSpell != NULL )
	{
		if( context->in_CastingSpell->NameHash != SPELL_HASH_JUDGEMENT
			&& context->in_CastingSpell->NameHash != SPELL_HASH_HAMMER_OF_WRATH 
			&& context->in_CastingSpell->NameHash != SPELL_HASH_EXORCISM
			&& context->in_CastingSpell->NameHash != SPELL_HASH_TEMPLAR_S_VERDICT
			)
			return;
	} */
	//we are already in this function on a previous call
	if( context->in_event->created_with_value == 1 )
		return;
	//get the active seal we have on 
	uint32 FoundSeal = 0;
	for(uint32 x=FIRST_AURA_SLOT;x<MAX_POSITIVE_AURAS1(context->in_Caller);x++)
	{
		Aura *a = context->in_Caller->m_auras[x];
		if(a && ( a->GetSpellProto()->BGR_one_buff_on_target & SPELL_TYPE_SEAL ) )
		{ 
			if( a->GetSpellProto()->NameHash != SPELL_HASH_SEAL_OF_RIGHTEOUSNESS
				&& a->GetSpellProto()->NameHash != SPELL_HASH_SEAL_OF_TRUTH
				&& a->GetSpellProto()->NameHash != SPELL_HASH_SEAL_OF_JUSTICE )
				return;
			FoundSeal = a->GetSpellProto()->NameHash;
			break;
		}
	}
	if( FoundSeal == 0 )
		return;
	//hit everyone withing melee range except our proc target ?
	if( FoundSeal == SPELL_HASH_SEAL_OF_RIGHTEOUSNESS && ( context->in_CastingSpell == NULL || context->in_CastingSpell->NameHash != SPELL_HASH_SEAL_OF_RIGHTEOUSNESS ) )
	{
		context->in_event->created_with_value = 1; //lock this function to not create pron on proc loop !
		InRangeSetRecProt::iterator itr;
		context->in_Caller->AquireInrangeLock(); //make sure to release lock before exit function !
		InrangeLoopExitAutoCallback AutoLock;
		for( itr = context->in_Caller->GetInRangeSetBegin( AutoLock ); itr != context->in_Caller->GetInRangeSetEnd(); itr++ )
		{
			if( !(*itr)->IsUnit() || !SafeUnitCast((*itr))->isAlive())
				continue;

			if( (*itr) == context->in_Victim )
				continue;

			if( (*itr)->GetDistance2dSq( context->in_Caller ) > 5.0f * 5.0f )
				continue;

			if( !isAttackable( context->in_Caller, (*itr) ) )
				continue;

			context->in_Caller->CastSpell( SafeUnitCast((*itr)), 25742, true ); //Seal of Righteousness
		}
		context->in_Caller->ReleaseInrangeLock();	
		context->in_event->created_with_value = 0; //lock this function to not create pron on proc loop !
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_23602( ProcHandlerContextShare *context )
{
	if( ( context->in_events_filter_flags & PROC_ON_REFLECT_EVENT ) )
		context->out_dmg_overwrite[0] = context->out_CastSpell->eff[0].EffectBasePoints + 600; //it is in the description
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_107785( ProcHandlerContextShare *context )
{
	uint32 Spells[3] = { 107785, 107787, 107789 };
	uint32 SelectedRandomSpell = Spells[ RandomUInt() % 4 ];
	context->out_CastSpell = dbcSpell.LookupEntryForced( SelectedRandomSpell );
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_109867( ProcHandlerContextShare *context )
{
	uint32 Spells[3] = { 109867, 109869, 109871 };
	uint32 SelectedRandomSpell = Spells[ RandomUInt() % 4 ];
	context->out_CastSpell = dbcSpell.LookupEntryForced( SelectedRandomSpell );
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_109868( ProcHandlerContextShare *context )
{
	static uint32 Spells[3] = { 109868, 109870, 109872 };
	uint32 Index = RandomUInt() % 3;
	uint32 SelectedRandomSpell = Spells[ Index ];
	context->out_CastSpell = dbcSpell.LookupEntryForced( SelectedRandomSpell );
	ASSERT( context->out_CastSpell ); //wtf crash ?
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_90289( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	SafePlayerCast( context->in_Caller )->ClearCooldownForSpell( 49576 );
	//Self proc, no need to continue !
}

void PH_83301( ProcHandlerContextShare *context )
{
	if( context->in_Caller == context->in_Victim )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_86700( ProcHandlerContextShare *context )
{
	//palading can proc it for ( on ) Ancient Guardian
	if( context->in_Caller->GetGUID() != context->in_event->caster )
	{
		Unit *Guardian = context->in_Caller->GetMapMgr()->GetUnit( context->in_event->caster );
		if( Guardian == NULL )
		{
			context->in_event->deleted = 1;
			return;
		}
		Guardian->CastSpell( Guardian, context->out_CastSpell, true );
	}
	else
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_57318( ProcHandlerContextShare *context )
{
	if( ( context->in_events_filter_flags & PROC_ON_AURA_REMOVE ) == PROC_ON_AURA_REMOVE )
	{
		Aura *a = context->in_Caller->HasAura( context->out_CastSpell->Id, 0, AURA_SEARCH_POSITIVE );
		if( a )
			a->Remove();
		return;
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_51533( ProcHandlerContextShare *context )
{
	//casting one spell will force the other wolf to cast the spell as well
	uint64 *OtherWolfGuid = (uint64*)context->in_Caller->GetCreateIn64Extension( EXTENSION_ID_FERAL_SPIRIT_COPY );
	Unit *OtherWolf = context->in_Caller->GetMapMgr()->GetUnit( *OtherWolfGuid );
	if( OtherWolf )
		OtherWolf->CastSpell( context->in_Victim, context->in_CastingSpell, true );
	//we handled this, no need to continue
}

void PH_16187( ProcHandlerContextShare *context )
{
	if( context->in_Caller->IsPlayer() == false )
		return;
	//we handled this, no need to continue
	Player *p_caster = SafePlayerCast( context->in_Caller );
	if( p_caster->m_TotemSlots[ 1 ] != 0 && p_caster->m_TotemSlots[ 1 ]->IsInWorld() )
		p_caster->m_TotemSlots[ 1 ]->HealDonePctMod += context->in_OwnerSpell->eff[0].EffectBasePoints;
}

void PH_101033( ProcHandlerContextShare *context )
{
	uint32 StrengthCoeff;
	if( context->in_CastingSpell->NameHash == SPELL_HASH_HEALING_WAVE || context->in_CastingSpell->NameHash == SPELL_HASH_GREATER_HEALING_WAVE )
		StrengthCoeff = 100;
	else if( context->in_CastingSpell->NameHash == SPELL_HASH_HEALING_SURGE || context->in_CastingSpell->NameHash == SPELL_HASH_RIPTIDE || context->in_CastingSpell->NameHash == SPELL_HASH_UNLEASH_LIFE )
		StrengthCoeff = 60;
	else if( context->in_CastingSpell->NameHash == SPELL_HASH_CHAIN_HEAL )
		StrengthCoeff = 30;
	else
		return;
	//needs to have water shield active
	if( context->in_Caller->HasAuraWithNameHash( SPELL_HASH_WATER_SHIELD, 0, AURA_SEARCH_POSITIVE ) == NULL )
		return;

	SpellCastTargets targets( context->in_Caller->GetGUID() );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init( context->in_Caller, context->out_CastSpell ,true, NULL);
	spell->forced_pct_mod[0] = StrengthCoeff;
	spell->ProcedOnSpell = context->in_OwnerSpell;
	spell->pSpellId=context->in_OwnerSpell->Id;
	spell->prepare(&targets);

//	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[2].EffectBasePoints;
//	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_36650( ProcHandlerContextShare *context )
{
	int64 *DamageTaken = context->in_Caller->GetCreateIn64Extension( EXTENSION_ID_VULNERABILITY_DMG );
	int64 *VulnerabilityType = context->in_Caller->GetCreateIn64Extension( EXTENSION_ID_VULNERABILITY_TYPE );
	uint32 DmgSchool;
	if( context->in_CastingSpell == NULL )
		DmgSchool = SCHOOL_NORMAL;
	else
		DmgSchool = context->in_CastingSpell->School;

	if( *VulnerabilityType == DmgSchool )
		*DamageTaken = *DamageTaken + context->in_dmg;
	else
	{
		context->out_dmg_absorb += context->in_dmg;
		context->in_Victim->Heal( context->in_Caller, context->in_CastingSpell ? context->in_CastingSpell->Id : 0, context->in_dmg );
	}
}

void PH_87193( ProcHandlerContextShare *context )
{
	uint32 melee_test_result = context->in_Caller->GetSpellDidHitResult( context->in_Victim, RANGED, context->out_CastSpell );
	if( melee_test_result != SPELL_DID_HIT_SUCCESS )
	{
		Spell::SendSpellFail( context->in_Caller, context->in_Victim, context->out_CastSpell->Id, melee_test_result );
	}
	else
		context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_49998( ProcHandlerContextShare *context )
{
	if( context->in_dmg - context->in_abs <= 0 )
		return;
	uint32 StoreSecond = getMSTime() / 1000;
	uint32 StoreIndex = StoreSecond % 5;
	uint32 *Store = (uint32*)context->in_Caller->GetCreateIn64Extension( EXTENSION_ID_DAMAGE_SUFFERED_INPAST_1 + StoreIndex  ); 
	if( Store[0] != StoreSecond )
		Store[1] = 0;
	Store[0] = StoreSecond;
	Store[1] += context->in_dmg - context->in_abs;
	//custom handler, no need to continue
}

void PH_16870( ProcHandlerContextShare *context )
{
	//do not proc on profession spells
	if( context->in_CastingSpell && context->in_CastingSpell->belongs_to_player_class != DRUID )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_17941( ProcHandlerContextShare *context )
{
	//do not proc if we are already casting shadow bolt. It would add / remove the buff without effect and people will report it
	if( context->in_Caller->isCasting() && context->in_Caller->GetCurrentSpell()->GetProto()->NameHash == SPELL_HASH_SHADOW_BOLT )
	{
		//cast the spell we wanted to cast
		context->in_Caller->CastSpell( context->in_Caller, context->out_CastSpell, true );
		//notify
		std::map<uint32,struct SpellCharge>::iterator iter;
		for( iter = context->in_Caller->m_chargeSpells.begin(); iter!= context->in_Caller->m_chargeSpells.end(); iter++ )
		{
			if( iter->second.spe->NameHash == SPELL_HASH_SHADOW_TRANCE && iter->second.GotCreatedInThisLoop == true )
				iter->second.SkipFirstEvents = 1;
		}
		return;
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_74002( ProcHandlerContextShare *context )
{
	if( context->in_event->custom_holder == 0 )
	{
		context->in_event->custom_holder = (void*)1;
		Aura *a = context->in_Caller->HasAuraWithNameHash( SPELL_HASH_COMBAT_READINESS, 0, AURA_SEARCH_POSITIVE );
		if( a )
		{
			a->SetDuration( a->GetDuration() + 10000 );	//allow full duration
			a->ResetDuration();
		}
	}
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_77829( ProcHandlerContextShare *context )
{
	if( context->in_Caller->isCasting() )
		context->out_dmg_absorb += ( context->in_dmg - context->in_abs ) * context->in_OwnerSpell->eff[0].EffectBasePoints / 100;
	// self proc
}

void PH_90721( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;

	SpellEntry *DispelledProto = dbcSpell.LookupEntryForced( context->in_dmg );
	SpellCastTargets targets( context->in_Caller->GetGUID() );
	Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
	spell->Init( context->in_Caller, context->out_CastSpell ,true, NULL);
	spell->forced_basepoints[0] = (DispelledProto->NameHash == SPELL_HASH_FROST_FEVER);
	spell->redirected_effect[0] = (DispelledProto->NameHash == SPELL_HASH_FROST_FEVER);
	spell->forced_basepoints[1] = (DispelledProto->NameHash == SPELL_HASH_BLOOD_PLAGUE);
	spell->redirected_effect[1] = (DispelledProto->NameHash == SPELL_HASH_BLOOD_PLAGUE);
	spell->ProcedOnSpell = context->in_OwnerSpell;
	spell->pSpellId=context->in_OwnerSpell->Id;
	spell->prepare(&targets);

//	context->out_dmg_overwrite[0] = context->in_OwnerSpell->eff[2].EffectBasePoints;
//	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_107821( ProcHandlerContextShare *context )
{
	//this is a CC shot, proccing will cancel the CC
	if( context->in_CastingSpell != NULL && context->in_CastingSpell->NameHash == SPELL_HASH_SCATTER_SHOT )
		return;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void PH_62758( ProcHandlerContextShare *context )
{
	if( context->in_CastingSpell == NULL )
		return;
	if( context->in_Caller->IsPet() == false )
		return;
	if( SafePetCast( context->in_Caller )->GetPower( POWER_TYPE_FOCUS ) <= 50 )
		return;
	//get focus cost 
	SpellPowerEntry *spe = dbcSpellPower.LookupEntryForced( context->in_CastingSpell->SpellPowerId );
	if( spe == NULL )
		return;
	uint32 SpellCost = spe->manaCost;
	int32 ExtraCost = SpellCost * context->in_OwnerSpell->eff[1].EffectBasePoints / 100;
	SafePetCast( context->in_Caller )->ModPower( POWER_TYPE_FOCUS, -ExtraCost );
	//mod dmg
	if( context->in_dmg_loc )
		*context->in_dmg_loc += *context->in_dmg_loc * context->in_OwnerSpell->eff[0].EffectBasePoints / 100;
	//!self proc
}

void PH_53511( ProcHandlerContextShare *context )
{
	if( context->in_Victim->GetHealthPct() >= 35 )
		return;
	//mod dmg
	if( context->in_dmg_loc )
		*context->in_dmg_loc += *context->in_dmg_loc * context->in_OwnerSpell->eff[0].EffectBasePoints / 100;
	//!self proc
}

void PH_59913( ProcHandlerContextShare *context )
{
	//target needs to hold XP or honor. Kinda quick test
	if( TargetGivesXPOrHonor( context->in_Victim, context->in_Caller ) == false )
		return;
	context->out_dmg_overwrite[0] = context->in_Caller->GetMaxHealth() * context->in_OwnerSpell->eff[0].EffectBasePoints / 100 ;
	context->out_handler_result = PROC_HANDLER_CONTINUE_EXECUTION;
}

void RegisterProcHandlers()
{
	SpellEntry *sp;
	//Swift Hand of Justice
	REGISTER_HANDLER( 59913 );
	//Feeding Frenzy
	REGISTER_HANDLER( 53511 );
	//Wild Hunt
	REGISTER_HANDLER( 62758 );
	//Item - Dragon Soul - Proc - Agi Ranged Gun
	REGISTER_HANDLER( 107821 );
	REGISTER_SAMEHANDLER( 109856, 107821 ); //Item - Dragon Soul - Proc - Agi Ranged Gun LFR
	REGISTER_SAMEHANDLER( 109858, 107821 ); //Item - Dragon Soul - Proc - Agi Ranged Gun Heroic
	//Resilient Infection
	REGISTER_HANDLER( 90721 );
	//Ancestral Resolve
	REGISTER_HANDLER( 77829 );
	//Combat Readiness
	REGISTER_HANDLER( 74002 );
	//Nightfall / Glyph of Corruption
	REGISTER_HANDLER( 17941 );
	//Omen of Clarity - Clearcasting
	REGISTER_HANDLER( 16870 );
	//Death Strike
	REGISTER_HANDLER( 49998 );
	//Paralysis
	REGISTER_HANDLER( 87193 );
	REGISTER_SAMEHANDLER( 87194, 87193 );
	//Phase Slip Vulnerability - dummy spell for custom boss encounter. We will overwrite it
	REGISTER_HANDLER( 36650 );
	//Resurgence
	REGISTER_HANDLER( 101033 );
	//Soothing Rains
	REGISTER_HANDLER( 16187 );
	REGISTER_SAMEHANDLER( 16205, 16187 );
	//Feral Spirit
	REGISTER_HANDLER( 51533 );
	//Sanctified Wrath
	REGISTER_HANDLER( 57318 );
	//Ancient Power
	REGISTER_HANDLER( 86700 );
	//Improved Cone of Cold
	REGISTER_HANDLER( 83301 );
	REGISTER_SAMEHANDLER( 83302, 83301 );
	//Glyph of Resillient Grip
	REGISTER_HANDLER( 90289 );
	//Item - Dragon Soul - Proc - Agi Melee 1H Axe - No'Kaled, the Elements of Death
	REGISTER_HANDLER( 107785 );
	REGISTER_HANDLER( 109867 );
	REGISTER_HANDLER( 109868 );
	//Shield Specialization
	REGISTER_HANDLER( 23602 );
	REGISTER_SAMEHANDLER( 84994, 23602 );
	REGISTER_SAMEHANDLER( 84993, 23602 );
	//Seals of Command
	REGISTER_HANDLER( 20424 );
	//Magic Suppression 
	REGISTER_HANDLER( 49088 );
	//s9 - Holy Walk 
	REGISTER_HANDLER( 96219 );
	//Rune of Cinderglacier
	REGISTER_HANDLER( 53386 );
	//Sudden Eclipse - Item - Moonkin Starfire Bonus
	REGISTER_HANDLER( 95746 );
	//Intervene
	REGISTER_HANDLER( 53476 );
	//Retaliation
	REGISTER_HANDLER( 20240 );
	//Shield Mastery
	REGISTER_HANDLER( 97954 );
	//Nature's Bounty
	REGISTER_HANDLER( 96206 );
	//Glyph of Raise Dead 
	REGISTER_HANDLER( 58686 );
	//Master Demonologist - mastery 
	REGISTER_HANDLER( 77221 );
	//Pandemic 
	REGISTER_HANDLER( 92931 );
	//Soulburn 
	REGISTER_HANDLER( 74434 );
	//Mirrored Blades 
	REGISTER_HANDLER( 83495 );
	//Point of No Escape
//	REGISTER_HANDLER( 53300 );
	//Improved Mend Pet
	REGISTER_HANDLER( 24406 );
	//Glyph of Kick
	REGISTER_HANDLER( 56805 );
	//Selfless Healer
	REGISTER_HANDLER( 90811 );
	//Glyph of Prayer of Mending
	REGISTER_HANDLER( 55685 );
	//Glyph of Spirit Tap
	REGISTER_HANDLER( 81301 );
	//Mana Feed
	REGISTER_HANDLER( 32554 );
	//Impending Doom
	REGISTER_HANDLER( 85106 );
	//Resistance is Futile
	REGISTER_HANDLER( 83676 );
	//Glyph of Barkskin
	REGISTER_HANDLER( 63058 );
	//Glyph of Innervate
	REGISTER_HANDLER( 54833 );
	//juggernaut
	REGISTER_HANDLER( 65156 );
	//Vigilance
	REGISTER_HANDLER( 50725 );
	//Elemental Disruption - Enchant Weapon - Elemental Slayer
	REGISTER_HANDLER( 74208 );
	//Glyph of Ice Block
	REGISTER_HANDLER( 56372 );
	//Reaping
	REGISTER_HANDLER( 56838 );
	//Glyph of Blurred Speed
	REGISTER_HANDLER( 73757 );
	//Guardian Spirit
	REGISTER_HANDLER( 48153 );
	//Glyph of Concussive Shot
	REGISTER_HANDLER( 56851 );
	//Consuming Shroud
	REGISTER_HANDLER( 110215 );
	//Deck Fire
	REGISTER_HANDLER( 109445 );
	//Timeloop
	REGISTER_HANDLER( 105992 );
	//Essence of Dreams
	REGISTER_HANDLER( 105996 );
	//Efflorescence
	REGISTER_HANDLER( 81262 );
	//Seasoned Winds
	REGISTER_HANDLER( 97618 );
	//Bloodthirst
	REGISTER_HANDLER( 23880 );
	//Backlash
	REGISTER_HANDLER( 34936 );
	//Lava Flows
	REGISTER_HANDLER( 65264 );
	//Crimson Scourge
	REGISTER_HANDLER( 81141 );
	//Glyph of Kill Shot
	REGISTER_HANDLER( 63067 );
	//Glyph of Shadowburn
	REGISTER_HANDLER( 56229 );
	//Shadowburn
	REGISTER_HANDLER( 29341 );
	//Culling the Herd
	REGISTER_HANDLER( 70893 );
	//Roar of Sacrifice
	REGISTER_HANDLER( 67481 );
	//Glyph of Healing Touch
	REGISTER_HANDLER( 54825 );
	//Invigoration
	REGISTER_HANDLER( 53398 );
	//Death's Advance
	REGISTER_HANDLER( 96268 );
	//Killing Streak
	REGISTER_HANDLER( 94006 );
	REGISTER_SAMEHANDLER( 94007, 94006 );
	//Improved Blood Presence
	REGISTER_HANDLER( 63611 );
	//Improved Frost Presence
	REGISTER_HANDLER( 63621 );
	//Improved Unholy Presence
	REGISTER_HANDLER( 63622 );
	//Glyph of Bloodletting
	REGISTER_HANDLER( 54815 );
	//Glyph of Ferocious Bite
	REGISTER_HANDLER( 67598 );
	//Dark Simulacrum
	REGISTER_HANDLER( 77616 );
	//Pyromaniac
	REGISTER_HANDLER( 83582 );
	//Might of the Frozen Wastes
	REGISTER_HANDLER( 81331 );
	//Concussive Barrage
	REGISTER_HANDLER( 35101 );
	//Nature's Ward
	REGISTER_HANDLER( 33881 );
	//Improved Flamestrike
//	REGISTER_HANDLER( 84674 );
	//Feral Swiftness
	REGISTER_HANDLER( 97985 );
	//Glyph of Power Word: Shield
	REGISTER_HANDLER( 56160 );
	//Hand of Light
	REGISTER_HANDLER( 96172 );
	//Fungal Growth Visual
	REGISTER_HANDLER( 94339 );
	//atered form
	REGISTER_HANDLER( 97709 );
	//Crouching Tiger, Hidden Chimera
	REGISTER_HANDLER( 82898 );
	//Glyph of Regrowth
	REGISTER_HANDLER( 54743 );
	//Earthquake - called from Dynamic object update
	REGISTER_HANDLER( 61882 );
	//Inner Sanctum
	REGISTER_HANDLER( 91724 );
	//Nick of Time
	REGISTER_HANDLER( 108000 );
	REGISTER_SAMEHANDLER( 109825, 108000 );
	REGISTER_SAMEHANDLER( 109822, 108000 );
	//Shadowbolt Volley
	REGISTER_HANDLER( 109800 );
	REGISTER_SAMEHANDLER( 108005, 109800 );
	REGISTER_SAMEHANDLER( 109798, 109800 );
	//Item - Death Knight T13 Blood 4P Bonus
	REGISTER_SAMEHANDLER( 105587, 105911 );
	//Item - Druid T13 Feral 4P Bonus (Frenzied Regeneration and Stampede)
	REGISTER_HANDLER( 81022 );
	REGISTER_SAMEHANDLER( 81021, 81022 );
	//Item - Paladin T13 Protection 2P Bonus (Judgement) - Delayed Judgement
	REGISTER_SAMEHANDLER( 105801, 105909 );
	//Item - Dragon Soul Legendary Daggers - Shadows of the Destroyer
	REGISTER_HANDLER( 109941 );
	REGISTER_HANDLER( 109950 );
	//Item - Warrior T13 Protection 4P Bonus (Shield Wall)
	REGISTER_HANDLER( 105911 );
	//Item - Warrior T13 Protection 2P Bonus (Revenge)
	REGISTER_HANDLER( 105909 );
	//Item - Warrior T13 Arms and Fury 4P Bonus (Colossus Smash)
	REGISTER_HANDLER( 108126 );
	//Item - Warrior T13 Arms and Fury 2P Bonus (Inner Rage)
//	REGISTER_HANDLER( 105860 );
	//Item - Shaman T13 Enhancement 2P Bonus (Maelstrom Weapon)
	REGISTER_HANDLER( 105869 );
	//Item - Paladin T13 Retribution 4P Bonus (Zealotry)
//	REGISTER_SAMEHANDLER( 105819, 105860 );
	//Item - Shaman T13 Restoration 4P Bonus (Spiritwalker's Grace)
//	REGISTER_SAMEHANDLER( 105877, 105860 );
	//Item - Mage T13 2P Bonus (Haste Rating)
	REGISTER_HANDLER( 105785 );
	//Item - Warlock T13 4P Bonus (Soulburn) 
	REGISTER_HANDLER( 105786 );
	//Item - Druid T13 Restoration 4P Bonus (Rejuvenation)
	REGISTER_HANDLER( 105770 );
	//Item - Druid T13 Feral 2P Bonus (Savage Defense and Blood In The Water)
	REGISTER_HANDLER( 105725 );
	//Item - Death Knight T13 DPS 4P Bonus
	REGISTER_HANDLER( 105647 );
	//Item - Death Knight T13 Blood 2P Bonus
	REGISTER_HANDLER( 105582 );
	//Item - Shaman T12 Elemental 2P Bonus
	REGISTER_HANDLER( 99204 );
	//Item - Rogue T12 4P Bonus
	REGISTER_HANDLER( 99187 );
	//Item - Rogue T12 2P Bonus
	REGISTER_HANDLER( 99173 );
	//Item - Warrior T12 Protection 2P Bonus
	REGISTER_SAMEHANDLER( 99240, 99173 );
	//Item - Druid T12 Feral 2P Bonus
	REGISTER_SAMEHANDLER( 99002, 99173 );
	//Item - Priest T12 Shadow 4P Bonus
	REGISTER_HANDLER( 99158 );
	//Item - Priest T12 Healer 2P Bonus
	REGISTER_HANDLER( 99134 );
	//Item - Druid T12 Restoration 4P Bonus
	REGISTER_HANDLER( 99015 );
	REGISTER_SAMEHANDLER( 99070, 99015 );
	//Item - Druid T12 Feral 4P Bonus
	REGISTER_HANDLER( 99011 );
	//Item - Death Knight T12 DPS 4P Bonus
	REGISTER_HANDLER( 98996 );
	//Item - Paladin T12 Protection 2P Bonus
	REGISTER_SAMEHANDLER( 99074, 98996 );
	//Indomitable - attack wish reduce you below x% health
	REGISTER_HANDLER( 108008 );
	//Loom of Fate - attack wish reduce you below x% health
	REGISTER_HANDLER( 97130 );
	//Foul Gift
	REGISTER_HANDLER( 102663 );
	//periodic damaging : DOT
	{
		//Necromantic Focus
		REGISTER_HANDLER( 96962 );
		REGISTER_SAMEHANDLER( 97132, 96962 );
		REGISTER_SAMEHANDLER( 97131, 96962 );
		REGISTER_SAMEHANDLER( 99221, 96962 );	//Item - Warlock T12 2P Bonus
	}
	//damaging = when you deal damage
	{
		//Haste
		REGISTER_SAMEHANDLER( 109789, 64343 );
		//Power of the Great Ones
		REGISTER_HANDLER( 53372 );
		//Charged
		REGISTER_SAMEHANDLER( 37234, 53372);
		//Energized
		REGISTER_SAMEHANDLER( 37214, 53372);
		//Relentlessness
		REGISTER_SAMEHANDLER( 37601, 53372);
		//http://www.wowhead.com/?item=32488 Ashtongue Talisman of Insight
		REGISTER_SAMEHANDLER( 40483, 53372);
	}
	//healing
	{
		//Eye of Blazing Power
		REGISTER_SAMEHANDLER( 97137, 51945 );
		//Item - Dragon Soul - Proc - Int Spirit Mace 1H
		REGISTER_SAMEHANDLER( 107836, 51945 );
		//Item - Dragon Soul Stacking Healer Trinket
		REGISTER_SAMEHANDLER( 107963, 51945 );
		//Haste
		REGISTER_SAMEHANDLER( 109805, 51945 );
		REGISTER_SAMEHANDLER( 109803, 51945 );
		//Item - Dragon Soul Stacking Healer Trinket
		REGISTER_SAMEHANDLER( 109814, 51945 );
		REGISTER_SAMEHANDLER( 109812, 51945 );
		//Item - Dragon Soul - Proc - Int Spirit Mace 1H LFR
		REGISTER_SAMEHANDLER( 109848, 51945 );
		//Item - Dragon Soul - Proc - Int Spirit Mace 1H Heroic
		REGISTER_SAMEHANDLER( 109850, 51945 );
	}
	//harmful
	{
		//Item - Dragon Soul Stacking Caster Trinket
		REGISTER_SAMEHANDLER( 109793, 64713 );
		//Item - Dragon Soul Stacking Caster Trinket
		REGISTER_SAMEHANDLER( 109795, 64713 );
		//Item - Dragon Soul - Proc - Int Hit Dagger Heroic
		REGISTER_SAMEHANDLER( 109851, 64713 );
		//Item - Dragon Soul - Proc - Int Hit Dagger Heroic
		REGISTER_SAMEHANDLER( 109854, 64713 );
		//Item - Dragon Soul - Proc - Int Hit Dagger
		REGISTER_SAMEHANDLER( 107831, 64713 );
		//Item - Dragon Soul Stacking Caster Trinket
		REGISTER_SAMEHANDLER( 107971, 64713 );
		//Item - Dragon Soul Stacking Caster Trinket
		REGISTER_SAMEHANDLER( 109796, 64713 );
		REGISTER_SAMEHANDLER( 109794, 64713 );
		//Item - Dragon Soul - Proc - Int Hit Dagger Heroic
		REGISTER_SAMEHANDLER( 109855, 64713 );
		//Item - Dragon Soul - Proc - Int Hit Dagger LFR
		REGISTER_SAMEHANDLER( 109853, 64713 );
	}
	//Blade Barrier
	REGISTER_HANDLER( 103419 );
	REGISTER_SAMEHANDLER( 103562, 103419 );	//Lesser Blade Barrier
	//Improved Mind Blast
	REGISTER_HANDLER( 48301 );
	//Crusade
	REGISTER_HANDLER( 94686 );
	//Item - Paladin T12 Retribution 2P Bonus
	REGISTER_HANDLER( 99092 );
	//Call of Flame
	REGISTER_HANDLER( 16038 );
	//Tower of Radiance
	REGISTER_HANDLER( 88852 );
	//Lambs to the Slaughter
	REGISTER_HANDLER( 84584 );
	REGISTER_SAMEHANDLER( 84585, 84584 );
	REGISTER_SAMEHANDLER( 84586, 84584 );
	//Honor Among Thieves
	REGISTER_HANDLER( 51699 );
	//Shadow Orb Power
	REGISTER_HANDLER( 77487 );
	//Pursuit of Justice
	REGISTER_HANDLER( 89024 );
	//Seal of Truth
	REGISTER_HANDLER( 31803 );
	//Matrix Restabilizer
	REGISTER_HANDLER( 97139 );
	//Matrix Restabilizer
	REGISTER_HANDLER( 96977 );
	//Electrical Charge
	REGISTER_HANDLER( 96890 );
	//Wrath of Tarecgosa
	REGISTER_HANDLER( 101085 );
	//Item - Druid Firelands Cosmetic Transform Controller
	REGISTER_HANDLER( 99244 );
	//Item - Warlock T12 4P Bonus
	REGISTER_HANDLER( 99232 );
	//Item - Shaman T12 Elemental 2P Bonus
	REGISTER_HANDLER( 99202 );
	//Item - Priest T12 Healer 4P Bonus
	REGISTER_HANDLER( 99136 );
	//Item - Priest T12 Healer 2P Bonus
	REGISTER_HANDLER( 99132 );
	//Item - Mage T12 2P Bonus
	REGISTER_HANDLER( 99063 );
	//Loom of Fate
	REGISTER_HANDLER( 96945 );
	REGISTER_SAMEHANDLER( 97129, 96945 );
	//Dark Succor
	REGISTER_HANDLER( 101568 );
	//Chilblains
	REGISTER_HANDLER( 50434 );
	REGISTER_SAMEHANDLER( 50435, 50434 );
	//Improved Serpent Sting
	REGISTER_HANDLER( 83077 );
	//Blessed Resilience
	REGISTER_HANDLER( 33143 );
	//Glyph of Prayer of Healing
	REGISTER_HANDLER( 56161 );
	//Body and Soul
	REGISTER_HANDLER( 64128 );
	REGISTER_SAMEHANDLER( 65081, 64128 );
	//Unholy Command
	REGISTER_HANDLER( 49588 );
	//Vital Spark
	REGISTER_HANDLER( 99262 );
	//Sacred Shield
	REGISTER_HANDLER( 96263 );
	//Revealing Strike	
	REGISTER_HANDLER( 84617 );
	//Restless Blades	
	REGISTER_HANDLER( 79095 );
	REGISTER_SAMEHANDLER( 79096, 79095 );
	//Fel Armor	
	REGISTER_HANDLER( 96379 );
	//Glyph of Misdirection
	REGISTER_HANDLER( 56829 );
	//Speed of Light
	REGISTER_HANDLER( 85497 );
	//Sin and Punishment
	REGISTER_HANDLER( 87204 );
	//Runic Empowerment
	REGISTER_HANDLER( 81229 );
	//Shadow Infusion
	REGISTER_HANDLER( 91342 );
	//Molten Shields
	REGISTER_HANDLER( 11094 );
	//Item - Druid T11 Feral 4P Bonus
	REGISTER_HANDLER( 90166 );
	//Glyph of Stoneclaw Totem
	REGISTER_HANDLER( 55277 );
	//Feedback
	REGISTER_HANDLER( 86183 );
	//Soulburn: Seed of Corruption
	REGISTER_HANDLER( 86664 );
	//jinx
	REGISTER_HANDLER( 85547 );
	REGISTER_SAMEHANDLER( 86105, 85547 );
	//Zealotry
	REGISTER_HANDLER( 85696 );
	//Long Arm of the Law
	REGISTER_HANDLER( 87173 );
	//s9 - Runic Power Back on Snare/Root - Runic Return
	REGISTER_HANDLER( 61258 );
	//Item - Druid T11 Restoration 4P Bonus
	REGISTER_HANDLER( 90159 );
	//Item - Hunter T11 4P Bonus
	REGISTER_HANDLER( 89925 );
	//Item - Priest T11 Healer 4P Bonus
	REGISTER_HANDLER( 89911 );
	//Stoicism - Item - Warrior T10 Protection 4P Bonus
	REGISTER_HANDLER( 70845 );
	//Aura of Foreboding
	REGISTER_HANDLER( 89603 );
	//Aura of Foreboding
	REGISTER_HANDLER( 93974 );
	REGISTER_SAMEHANDLER( 93987, 93974 );
	//Improved Soul Fire
	REGISTER_HANDLER( 85383 );
	//Haunt
	REGISTER_HANDLER( 50091 );
	//Demonic Rebirth
	REGISTER_HANDLER( 88448 );
	//Bane of Havoc
	REGISTER_HANDLER( 85455 );
	//Nether Ward
//	REGISTER_HANDLER( 91713 );
	//Burning Embers
	REGISTER_HANDLER( 85421 );
	//Euphoria
	REGISTER_HANDLER( 81069 );
	//Gift of the Earthmother
	REGISTER_HANDLER( 51179 );
	//Primal Madness
	REGISTER_HANDLER( 17080 );
	//Guardian of Ancient Kings - heal spell
	REGISTER_HANDLER( 86678 );
	//Spirit Hunt - feral spirit heal spell
	REGISTER_HANDLER( 58879 );
	//Blood in the Water
	REGISTER_HANDLER( 80863 );
	//Glyph of Preparation
	REGISTER_HANDLER( 56819 );
	//Shield of the Templar
	REGISTER_HANDLER( 31848 );
	//Eternal Glory
	REGISTER_HANDLER( 88676 );
	//Guarded by the Light
	REGISTER_HANDLER( 88063 );
	//Divine Purpose
	REGISTER_HANDLER( 90174 );
	//Feral Aggression
	REGISTER_HANDLER( 16858 );
	//Grand Crusader
	REGISTER_HANDLER( 85416 );
	//Natural Reaction
	REGISTER_HANDLER( 57893 );
	REGISTER_SAMEHANDLER( 59071, 57893 );
	//Stampede
	REGISTER_HANDLER( 33876 );
	//Improved Hamstring
	REGISTER_HANDLER( 23694 );
	//Glyph of Blind
	REGISTER_HANDLER( 91299 );
	//Permafrost
	REGISTER_HANDLER( 91394 );
	//Bombardment
	REGISTER_HANDLER( 82921 );
	//Elemental Devastation
	REGISTER_HANDLER( 29178 );
	//Hunter - Frenzy - pet
	REGISTER_HANDLER( 19615 );
	//Fingers of Frost
	REGISTER_HANDLER( 44544 );
	//Reactive Barrier - Ice Barrier
	REGISTER_HANDLER( 11426 );
	//Improved Mana Gem
	REGISTER_HANDLER( 83098 );
	//Focused Insight
	REGISTER_HANDLER( 77800 );
	//Tidal Waves
	REGISTER_HANDLER( 53390 );
	//Telluric Currents
	REGISTER_HANDLER( 82987 );
	//Ancestral Awakening
	REGISTER_HANDLER( 52752 );
	//Item - Proc Dodge Below 35%
	REGISTER_HANDLER( 92233 );
	REGISTER_SAMEHANDLER( 92235, 92233 );	//Item - Proc Mastery Below 35%
	REGISTER_SAMEHANDLER( 92355, 92233 );	//Item - Proc Mastery Below 35%
	//Item - Proc Mastery
	REGISTER_HANDLER( 91024 );
	REGISTER_SAMEHANDLER( 91047, 91024 );	//Item - Proc Spell Power
	REGISTER_SAMEHANDLER( 92320, 91024 );	//Item - Proc Mastery
	REGISTER_SAMEHANDLER( 91011, 91024 );	//Item - Proc Mastery
	REGISTER_SAMEHANDLER( 92319, 91024 );	//Item - Proc Mastery
	//Deadly Momentum
	REGISTER_HANDLER( 84590 );
	//Serrated Blades
	REGISTER_HANDLER( 14171 );
	//Bandit's Guile
	REGISTER_HANDLER( 84748 );
	//Cut to the Chase
	REGISTER_HANDLER( 51664 );
	//Murderous Intent
	REGISTER_HANDLER( 79132 );
	//Nether Vortex - Slow
	REGISTER_HANDLER( 31589 );
	//Static Shock - leeching lightning shield proc
	REGISTER_HANDLER( 26364 );
	//Totemic Wrath
	REGISTER_HANDLER( 77747 );
	//Fulmination
	REGISTER_HANDLER( 88767 );
	//Rolling Thunder
	REGISTER_HANDLER( 88765 );
	//Glyph of Hemorrhage
	REGISTER_HANDLER( 89775 );
	//Victorious - Impending Victory
	REGISTER_HANDLER( 82368 );
	//Phantasm
	REGISTER_HANDLER( 47569 );
	//Train of Thought
	REGISTER_HANDLER( 92295 );
	//Glyph of Backstab
	REGISTER_HANDLER( 89765 );
	//Find Weakness
	REGISTER_HANDLER( 91021 );
	//Combat Potency
	REGISTER_HANDLER( 35542 );
	REGISTER_SAMEHANDLER( 35545, 35542 );
	REGISTER_SAMEHANDLER( 35546, 35542 );
	//Savage Combat
	REGISTER_HANDLER( 58683 );
	REGISTER_SAMEHANDLER( 58684, 58683 );
	//Venomous Wounds
	REGISTER_HANDLER( 79136 );
	//Master Poisoner
	REGISTER_HANDLER( 93068 );
	//Chakra
	REGISTER_HANDLER( 14751 );
	//Chakra - serenity
	REGISTER_HANDLER( 81585 );
	//Inspiration
	REGISTER_HANDLER( 14893 );
	REGISTER_SAMEHANDLER( 15357, 14893 );
	//Surge of Light
	REGISTER_HANDLER( 88688 );
	//Anthem
	REGISTER_HANDLER( 91141 );
	//Inner Eye
	REGISTER_HANDLER( 91320 );
	//Atonement
	REGISTER_HANDLER( 81751 );
	REGISTER_SAMEHANDLER( 94472, 81751 );
	//Summon Shadowy Apparition Spawn
	REGISTER_HANDLER( 87426 );
	//Protector of the Innocent
	REGISTER_HANDLER( 94286 );
	REGISTER_SAMEHANDLER( 94288, 94286 );
	REGISTER_SAMEHANDLER( 94289, 94286 );
	//Dark Intent
	REGISTER_HANDLER( 85759 );	//haxing an existing spell, every class should have he's own spell 
	//Posthaste
	REGISTER_HANDLER( 83559 );
	//Improved Steady Shot
	REGISTER_HANDLER( 53220 );
	//Master Marksman
	REGISTER_HANDLER( 82925 );
	//Termination
	REGISTER_HANDLER( 83489 );
	//Evangelism
	REGISTER_HANDLER( 81661 );
	REGISTER_SAMEHANDLER( 81660, 81661 );
	//Strength of Soul
	REGISTER_HANDLER( 96266 );
	REGISTER_SAMEHANDLER( 96267, 96266 );
	//Vengeance
	REGISTER_HANDLER( 76691 );
	//Improved Mind Blast
	REGISTER_HANDLER( 38412 );
	//Blood and Thunder
	REGISTER_HANDLER( 84614 );
	//Die by the Sword
	REGISTER_HANDLER( 85386 );
	REGISTER_SAMEHANDLER( 86624, 85386 );
	//Blood Frenzy
	REGISTER_HANDLER( 30069 );
	REGISTER_HANDLER( 30070 );
	//Rude Interruption
//	REGISTER_HANDLER( 86662 );
//	REGISTER_SAMEHANDLER( 86663, 86662 );
	//Will of the Necropolis
	REGISTER_HANDLER( 81162 );
	//Shooting Stars
	REGISTER_HANDLER( 93400 );
	//Masochism
	REGISTER_HANDLER( 89007 );
	//Cauterize
	REGISTER_HANDLER( 87023 );
	//Ebon Plague
	REGISTER_HANDLER( 65142 );
	//Wild Quiver
	REGISTER_HANDLER( 76663 );
	//Strikes of Opportunity
	REGISTER_HANDLER( 76858 );
	//Echo of Light
	REGISTER_HANDLER( 77489 );
	//Elemental Overload
	REGISTER_HANDLER( 77222 );
	//The Art of War
	REGISTER_HANDLER( 59578 );
	//Main Gauche
	REGISTER_HANDLER( 86392 );
	//Illuminated Healing
	REGISTER_HANDLER( 86273 );
	//Revitalize
	REGISTER_HANDLER( 37243 );
	//Aspect of the Fox - !!! self proc !!
	REGISTER_HANDLER( 82661 );
	//arcane missiles only procs on dmging spells
	REGISTER_HANDLER( 79683 );
	//beacon of light
	REGISTER_HANDLER( 53652 );
	//Empowered Touch
	REGISTER_HANDLER( 88433 );
	//Threat of Thassarian
	REGISTER_HANDLER( 65661 );
	//Improved Blood Presence
	REGISTER_HANDLER( 50365 );
	//Improved Frost Presence
	REGISTER_HANDLER( 50384 );
	//hunter - Aspect of the Cheetah
	REGISTER_HANDLER( 15571 );
	//warlock - Decimation
	REGISTER_HANDLER( 63165 );
	REGISTER_SAMEHANDLER( 63167, 63165 );
	//druid -> Brambles
	REGISTER_HANDLER( 50411 );
	//Health Leech -> DK bloodworms
	REGISTER_HANDLER( 50454 );
	//Mana Leech -> priest shadowfiend
	REGISTER_HANDLER( 34650 );
	//shaman - Earth Shield
	REGISTER_HANDLER( 974 );
	//priest - Rapture
//	REGISTER_HANDLER( 47755 );
	//priest - Grace
	REGISTER_HANDLER( 47930 );
	REGISTER_SAMEHANDLER( 77613, 47930 );
	//priest - Vampiric Embrace
	REGISTER_HANDLER( 15290 );
	//priest - Vampiric Touch
	REGISTER_HANDLER( 34919 );
	//seal of Light -> a chance of healing paladin for ${0.15*$AP+0.15*$SPH}
//	REGISTER_HANDLER( 20167 );
	//Primal Fury - cat form
	REGISTER_HANDLER( 16953 );
	//Primal Fury - bear and direbear form
	REGISTER_HANDLER( 16959 );
	//warlock Haunt
//	REGISTER_HANDLER( 48210 );
	//warlock Improved Felhunter
//	REGISTER_HANDLER( 54425 );
	//Glyph of Healing Wave
	REGISTER_HANDLER( 55533 );
	//Mark of the Fallen Champion -> dmg proc
	REGISTER_HANDLER( 72255 );
	//Mark of the Fallen Champion -> heal proc
	REGISTER_HANDLER( 72260 );
	//Item - Death Knight T10 Melee 4P Bonus
	REGISTER_HANDLER( 70657 );
	//Purified Shard of the Scale - Equip Effect
	REGISTER_HANDLER( 69734 );
	//Shiny Shard of the Scale - Equip Effect
	REGISTER_SAMEHANDLER( 62402, 69734 );
	//Item - Paladin T10 Retribution 2P Bonus
	REGISTER_HANDLER( 70765 );
	//Item - Druid T10 Restoration 4P Bonus (Rejuvenation)
	REGISTER_HANDLER( 70664 );
	//Item - Priest T10 Healer 2P Bonus
	REGISTER_HANDLER( 70809 );
	//Item - Druid T10 Balance 4P Bonus
	REGISTER_HANDLER( 71023 );
	//Item - Rogue T10 4P Bonus
	REGISTER_HANDLER( 70802 );
	//Item - Druid T8 Restoration 4P Bonus
	REGISTER_HANDLER( 64801 );
	//Item - Shaman T8 Elemental 4P Bonus
	REGISTER_HANDLER( 50895 );
	//item : Enchant Weapon - Blood Draining
	REGISTER_HANDLER( 64569 );
	REGISTER_SAMEHANDLER( 75477, 64569 );
	REGISTER_SAMEHANDLER( 75480, 64569 );
	//Shard of Flame
	REGISTER_HANDLER( 67759 );
	//Mote of Flame
	REGISTER_HANDLER( 67713 );
	//Paragon - ver 2
	REGISTER_HANDLER( 67772 );
	//Paragon - ver 1
	REGISTER_HANDLER( 67703 );
	//Flame of the Heavens
	REGISTER_HANDLER( 64713 );
	//Volatile Power
	REGISTER_SAMEHANDLER( 67743, 64713 );
	//Elusive Power
	REGISTER_SAMEHANDLER( 67669, 64713 );
	// trinket spells like deat's verdic : 6 TOC items
	REGISTER_HANDLER( 67696 );
	// paladin - Item - Paladin T9 Retribution Relic (Seal of Vengeance)
//	REGISTER_HANDLER( 67371 );
	// paladin - blessing of sanctuary
	REGISTER_HANDLER( 57319 );
	// Enduring Light
//	REGISTER_HANDLER( 40471 );
	// Deathknight - Unholy Blight
	REGISTER_HANDLER( 50536 );
	// DK - Necrosis
	REGISTER_HANDLER( 51460 );
	// DK - Butchery
	REGISTER_HANDLER( 50163 );
	// DK - Blade Barrier
//	REGISTER_HANDLER( 51789 );
	// DK - killing machine only procs on normal melee attacks and not abbilities
	REGISTER_HANDLER( 51124 );
	// DK - Blood-Caked Blade - also melee only
	REGISTER_SAMEHANDLER( 50463, 51124);
	//Darkmoon Card: Greatness
	REGISTER_HANDLER( 60229);
	//item - Eye of the Broodmother
	REGISTER_HANDLER( 65006);
	//item - Val'anyr Hammer of Ancient Kings
	REGISTER_HANDLER( 64411);
	//Living Seed the heal proc
	REGISTER_HANDLER( 48503);
	//Living Seed
	REGISTER_HANDLER( 48504);
	//druid - Nature's Grace
	REGISTER_HANDLER( 16886);
	//using a mana gem grants you 225 spell damage for 15 sec
	REGISTER_HANDLER( 37445);
	//http://www.wowhead.com/?item=32492 Ashtongue Talisman of Lethality
	REGISTER_HANDLER( 40461);
	//http://www.wowhead.com/?item=32490 Ashtongue Talisman of Acumen
	REGISTER_HANDLER( 40440);
	//http://www.wowhead.com/?item=32490 Ashtongue Talisman of Acumen
	REGISTER_HANDLER( 40441);
	//Rejuvenation has a 25% chance to grant up to 210 healing for 8 sec
	REGISTER_HANDLER( 40446);
	//Starfire has a 25% chance to grant up to 150 spell damage for 8 sec
	REGISTER_HANDLER( 40445);
	//http://www.wowhead.com/?item=32486 Ashtongue Talisman of Equilibrium
//	REGISTER_HANDLER( 40452);
	//http://www.wowhead.com/?item=32496  Memento of Tyrande
	REGISTER_HANDLER( 37656);
	//Epiphany :Each spell you cast can trigger an Epiphany, increasing your mana regeneration by 24 for 30 sec.
	REGISTER_SAMEHANDLER( 28804, 37656);
	//Judgements of the Just
	REGISTER_HANDLER( 68055);
	//item - Shadowflame Hellfire and RoF
	REGISTER_HANDLER( 37378 );
	//Item - Anger Capacitor
	REGISTER_HANDLER( 71432 );
	//Item - Shadowmourne Legendary
	REGISTER_HANDLER( 71905 );
	//SETBONUSES
	REGISTER_HANDLER( 37379 );
	//druid - Eclipse - part 1
	REGISTER_HANDLER( 48517 );
	//druid - Eclipse - part 2
	REGISTER_HANDLER( 48518 );
	//Freeze
	REGISTER_HANDLER( 63685 );
	//shaman - Lightning Overload
	REGISTER_HANDLER( 30675 );
	//shaman - Earth Shield - the heal
	REGISTER_HANDLER( 379 );
	//HoTs on Heals - //periodic healing spell
	REGISTER_HANDLER( 38324 );
	//Eyes of Twilight - //direct healing spell
	REGISTER_HANDLER( 75493);
	//Eyes of Twilight
	REGISTER_SAMEHANDLER( 75494, 75493);
	//Augment Pain - //periodic damaging spell
	REGISTER_HANDLER( 45055 );
	//Item - Icecrown 25 Heroic Caster Trinket 2
	REGISTER_SAMEHANDLER( 71636, 45055);
	//Item - Icecrown 25 Normal Caster Trinket 2
	REGISTER_SAMEHANDLER( 71605, 45055);
	//Talisman of Troll Divinity	- need to make direct casts !
	REGISTER_HANDLER( 60518 );
	//Charm of the Witch Doctor
	REGISTER_SAMEHANDLER( 43821, 60518);
	//Healing Discount  - all healing
	REGISTER_SAMEHANDLER( 37706, 60518);
	//item - Band of the Eternal Restorer
	REGISTER_SAMEHANDLER( 35087, 60518);
	//Vial of the Sunwel
	REGISTER_SAMEHANDLER( 45062, 60518);
	//Wave Trance
	REGISTER_SAMEHANDLER( 39950, 60518);
	//Ribbon of Sacrifice
	REGISTER_SAMEHANDLER( 38333, 60518);
	//Spark of Life
	REGISTER_HANDLER( 60520 );
	//Item - Druid T9 Feral Relic (Lacerate, Swipe, Mangle, and Shred)
	REGISTER_HANDLER( 67354 );
	//Spell Haste Trinket http://www.wowhead.com/?item=28190 scarab of the infinite cicle
	REGISTER_HANDLER( 33370 );
	//Charm of Mighty Mojo
	REGISTER_SAMEHANDLER( 43819, 33370);
	//Item - Chamber of Aspects 25 Nuker Trinket
	REGISTER_SAMEHANDLER( 75466, 33370);
	//Item - Chamber of Aspects 25 Heroic Nuker Trinket
	REGISTER_SAMEHANDLER( 75473, 33370);
	//Deadly Precision - direct cast and not over time
	REGISTER_HANDLER( 71564 );
	//Item - Icecrown Reputation Ring Caster Trigger
	REGISTER_HANDLER( 72416 );
	//Item - Icecrown 25 Normal Caster Weapon Proc
	REGISTER_SAMEHANDLER( 71843, 72416);
	//Volatile Power
	REGISTER_SAMEHANDLER( 67735, 72416);
	//Forgotten Knowledge
	REGISTER_SAMEHANDLER( 38317, 72416);
	//Item - Icecrown 25 Normal Caster Trinket 1 Base
	REGISTER_SAMEHANDLER( 71601, 72416);
	//Item - Icecrown 25 Heroic Caster Trinket 1 Base
	REGISTER_SAMEHANDLER( 71644, 72416);
	//item - Band of the Eternal Sage
	REGISTER_SAMEHANDLER( 35084, 72416);
	//Item - Icecrown 25 Normal Dagger Proc
	REGISTER_HANDLER( 71881 );
	//Item - Icecrown 25 Heroic Dagger Proc
	REGISTER_HANDLER( 71888 );
	//Forethought Talisman
	REGISTER_HANDLER( 60530 );
	//Item - Icecrown Reputation Ring Healer Trigger
	REGISTER_HANDLER( 72418 );
	//Item - Paladin T10 Holy Relic (Holy Shock)
	REGISTER_SAMEHANDLER( 71192, 72418);
	//Escalating Power
	REGISTER_SAMEHANDLER( 67739, 72418);
	//Escalating Power
	REGISTER_SAMEHANDLER( 67723, 72418);
	//Coliseum 5 Healer Trinket
	REGISTER_SAMEHANDLER( 67666, 72418);
	//Item - Icecrown 25 Normal Healer Trinket 2
	REGISTER_HANDLER( 71610 );
	//Item - Icecrown 25 Heroic Healer Trinket 2
	REGISTER_SAMEHANDLER( 71641, 71610);
	//Item - Icecrown Dungeon Healer Trinket
	REGISTER_HANDLER( 71566 );
	//Bonus Healing
	REGISTER_HANDLER( 40972 );
	//Item - Icecrown 25 Normal Tank Trinket 1
	REGISTER_HANDLER( 71633 );
	REGISTER_SAMEHANDLER( 71639, 71633);
	REGISTER_SAMEHANDLER( 52419, 71633);
	REGISTER_SAMEHANDLER( 45058, 71633);
	//Item - Icecrown 25 Normal Melee Trinket
	REGISTER_HANDLER( 71519 );
	//DK - Rime
//	REGISTER_HANDLER( 59052 );
	//paladin - Eye for an Eye
	REGISTER_HANDLER( 25997 );
	//paladin - Spiritual Attunement
	REGISTER_HANDLER( 31786 );
	//Paladin - Sheath of Light
//	REGISTER_HANDLER( 54203 );
	//Paladin - Judgements of the Wise
//	REGISTER_HANDLER( 31930 );
	//Paladin - Illumination
//	REGISTER_HANDLER( 20272 );
	//Paladin - Reckoning
	REGISTER_HANDLER( 20178 );
	//Paladin - Ardent Defender - part that heals if HP it is a killer blow
	REGISTER_HANDLER( 66235 );
	//priest - Reflective Shield 
	REGISTER_HANDLER( 33619 );
	//priest - Divine Aegis
	REGISTER_HANDLER( 47753 );
	//Piercing Shots
	REGISTER_HANDLER( 63468 );
	//Hunter - Thrill of the Hunt
	REGISTER_HANDLER( 34720 );
	//druid - Savage Defense
	REGISTER_HANDLER( 62606 );
	//Hunter - frost trap should trigger at the location of the trap and not the target
	REGISTER_HANDLER( 13810 );
	//druid - King of the Jungle 1
	REGISTER_HANDLER( 51185 );
	//druid - King of the Jungle 2
//	REGISTER_HANDLER( 51178 );
	//mage - Master of Elements
	REGISTER_HANDLER( 29077 );
	//mage - Improved Blizzard
	REGISTER_HANDLER( 12484 );
	REGISTER_SAMEHANDLER( 12485, 12484);
	REGISTER_SAMEHANDLER( 12486, 12484);
	// Brain Freeze
	REGISTER_HANDLER( 57761 );
	// mage - Arcane Concentration
	REGISTER_HANDLER( 12536 );
	// Priest - Shadowguard
	REGISTER_HANDLER( 32861 );
	REGISTER_SAMEHANDLER( 38379, 32861);
	// rogue - Overkill
	REGISTER_HANDLER( 58427 );
	// rogue - Master of Subtlety
	REGISTER_HANDLER( 31665 );
	// rogue - Relentless Strikes
	REGISTER_HANDLER( 98440 );
	// rogue - Ruthlessness
	REGISTER_HANDLER( 14157 );
	// shaman - windfury weapon
	REGISTER_HANDLER( 33750 );
	// Clearcasting
	REGISTER_HANDLER( 16246 );
	// shaman - Ancestral Fortitude
	REGISTER_HANDLER( 16177 );
	REGISTER_SAMEHANDLER( 16236, 16177);
	// shaman - Improved Stormstrike
	REGISTER_HANDLER( 63375 );
	// shaman - Item - Shaman T10 Enhancement 4P Bonus
	REGISTER_HANDLER( 70831 );
	// Astral Shift
	REGISTER_HANDLER( 52179 );
	// Earthliving
	REGISTER_HANDLER( 51945 );
	// priest - Blessed Recovery
	REGISTER_HANDLER( 27813 );
	REGISTER_SAMEHANDLER( 27817, 27813);
	REGISTER_SAMEHANDLER( 27818, 27813);
	// priest - Improved Shadowform
//	REGISTER_HANDLER( 15473 );
	// priest - Shadow Weaving
//	REGISTER_HANDLER( 15258 );
	// priest - prayer of mending
	REGISTER_HANDLER( 33110 );
	// priest - Empowered Renew - Divine Touch
	REGISTER_HANDLER( 63544 );
	// priest - Improved Devouring Plague
	REGISTER_HANDLER( 63675 );
	// mage - Winter's Chill
	REGISTER_HANDLER( 12579 );
	// mage - Incanter's Absorption
	REGISTER_HANDLER( 44413 );
	// mage - Focus Magic
	REGISTER_HANDLER( 54648 );
	// warlock - Soul Leech
	// hunter - Hunting Party
	REGISTER_HANDLER( 57669 );
	// hunter : Noxious Stings
	REGISTER_HANDLER( 53295 );
	// hunter : Improved Aspect of the Hawk
	REGISTER_HANDLER( 6150 );
	// hunter - Rapid Recuperation proc 1
	REGISTER_HANDLER( 53230 );
	REGISTER_SAMEHANDLER( 54227, 53230);
	// warlock - Soul Leech
	REGISTER_HANDLER( 30294 );
	// warlock - Nether Protection
	REGISTER_HANDLER( 54370 );
	// warlock - Aftermath
	REGISTER_HANDLER( 18118 );
	// warlock - Shadow Embrace
	REGISTER_HANDLER( 32386 );
	REGISTER_SAMEHANDLER( 32388, 32386);
	REGISTER_SAMEHANDLER( 32389, 32386);
	// warlock soul link
	REGISTER_HANDLER( 25228 );
	// warlock - Everlasting Affliction
	REGISTER_HANDLER( 47422 );
	// warlock - Fel Synergy
	REGISTER_HANDLER( 54181 );
	// warlock - Siphon Life
//	REGISTER_HANDLER( 63106 );
	// warlock - Unstable Affliction
	REGISTER_HANDLER( 31117 );
	// warlock - Improved Drain Soul
//	REGISTER_HANDLER( 18371 );
	// warlock - Seed of Corruption
	REGISTER_HANDLER( 27285 );
	// warlock - drain soul
	REGISTER_HANDLER( 79264 );
	// rogue - blade twisting
	REGISTER_HANDLER( 31125 );
	// rogue - Cheat Death
	REGISTER_HANDLER( 45182 );
	// druid - Primal Fury
	REGISTER_HANDLER( 37116 );
	REGISTER_SAMEHANDLER( 37117, 37116);
	// hot streak
	REGISTER_HANDLER( 48108 );
	// impact
	REGISTER_HANDLER( 64343 );
	// Mage ignite talent only for fire dmg
	REGISTER_HANDLER( 12654 );
//	// warrior - Battle Trance
//	REGISTER_HANDLER( 12964 );
	// warrior - Sudden Death
	REGISTER_HANDLER( 52437 );
	// deep wound requires a melee weapon
	REGISTER_HANDLER( 12721 );
	// sword specialization
	REGISTER_HANDLER( 16459 );
	// Improved Defensive Stance
	REGISTER_HANDLER( 57514 );
	REGISTER_SAMEHANDLER( 57516, 57514);
	//Bloodlust cat
	REGISTER_HANDLER( 37310 );
	//Bloodlust bear
	REGISTER_HANDLER( 37309 );
	//Nature's Guardian
	REGISTER_HANDLER( 31616 );
	//seal fate
	REGISTER_HANDLER( 14189 );
	//general proc handler for aura 300 -> mirror dmg on caster
	REGISTER_HANDLER( 71948 );
	// druid - Predator's Swiftness
	REGISTER_HANDLER( 69369 );
	// Druid - Owlkin Frenzy
	REGISTER_HANDLER( 48391 );
	// Druid - Improved Leader of the Pack
	REGISTER_HANDLER( 34299 );
	// Druid - Infected Wounds
	REGISTER_HANDLER( 58179 );
	REGISTER_SAMEHANDLER( 58180, 58179);
	// Druid - Blood Frenzy Proc
//	REGISTER_HANDLER( 16953 );
}
