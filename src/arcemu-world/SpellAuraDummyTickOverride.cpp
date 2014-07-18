#include "StdAfx.h"

#define REGISTER_HANDLER( spell ) sp = dbcSpell.LookupEntryForced( spell ); \
	ASSERT( sp->AuraPeriodicDummyTickHook == 0 ); \
	sp->AuraPeriodicDummyTickHook = &ADTH_##spell;
#define REGISTER_SAMEHANDLER(spell,copy_from) sp = dbcSpell.LookupEntryForced( spell ); \
	ASSERT( sp->AuraPeriodicDummyTickHook == 0 ); \
	sp->AuraPeriodicDummyTickHook = &ADTH_##copy_from;
/*
bool ADTH_59566( uint32 i, Aura *aur, bool apply )
{
	//check owner spell proc chance
	if( aur->pSpellId == 0 )
		return true;
	SpellEntry *OwnerSpell = dbcSpell.LookupEntryForced( aur->pSpellId );
	if( !Rand( OwnerSpell->eff[0].EffectBasePoints ) )
		return true;
	//we are the totem and we are targetting nearby friendly units
	Unit *unitTarget = aur->GetTarget();
	for(uint32 x=MAX_POSITIVE_AURAS;x<unitTarget->m_auras_neg_size;x++)
		if( unitTarget->m_auras[x] && unitTarget->m_auras[x]->GetSpellProto()->MechanicsType == MECHANIC_ENSNARED )
			unitTarget->m_auras[x]->Remove();
	return true; //we handled this tick, no need to take further actions
}*/

bool ADTH_25956( uint32 i, Aura *aur, bool apply )
{
	if( aur->GetTarget()->IsPlayer() == false )
		return false;
	Player *p = SafePlayerCast( aur->GetTarget() );
	// Haste 1 = 100% cast time , 0.5 = 50% cast time
	int32 NewValue = -(100-float2int32(p->GetSpellHaste()*100.0f)); //95 for the sake of rounding
	if( aur->m_modList[0].fixed_amount[0] != NewValue )
	{
		int32 Mod;
		if( aur->m_modList[0].fixed_amount[0] == 0 )
		{
			aur->m_modList[0].fixed_amount[0] = NewValue;
			Mod = NewValue;
		}
		else
		{
			Mod = -aur->m_modList[0].fixed_amount[0] + NewValue;
			aur->m_modList[0].fixed_amount[0] = NewValue;
		}
		uint32 *AffectedGroups = aur->GetSpellProto()->eff[0].EffectSpellGroupRelation;
		aur->SendModifierLog( &p->SM_Mods->SM_PCooldownTime, Mod, AffectedGroups, aur->GetSpellProto()->eff[0].EffectMiscValue, true ); 
	}
	return true;
}

bool ADTH_83676( uint32 i, Aura *aur, bool apply )
{
	if( apply )
	{
		Unit *u_target = aur->GetTarget();
		if( aur->m_modList[0].fixed_amount[0] != 0 )
		{
			//did we move ?
			if( aur->m_modList[0].fixed_amount[0] != float2int32( u_target->GetPositionX() * 10.0f ) || aur->m_modList[0].fixed_amount[1] != float2int32( u_target->GetPositionY() * 10.0f ) )
			{
				int32 chance = 8;
				if( aur->pSpellId )
					chance = MIN( chance, dbcSpell.LookupEntryForced( aur->pSpellId )->eff[0].EffectBasePoints );
				if( RandChance( chance ) )
				{
					Object *u_caster = aur->GetCaster();
					if( u_caster != NULL && u_caster->IsPlayer() )
						SafePlayerCast( u_caster )->CastSpell( SafePlayerCast( u_caster ), 82897, true );
				}
			}
		}
		aur->m_modList[0].fixed_amount[0] = u_target->GetPositionX() * 10.0f;
		aur->m_modList[0].fixed_amount[1] = u_target->GetPositionY() * 10.0f;
//		aur->m_modList[0].fixed_amount[2] = u_target->GetPositionZ() * 10.0f;
	}
	return true;
}

bool ADTH_87806( uint32 i, Aura *aur, bool apply )
{
	if( apply )
	{
		Unit *u_target = aur->GetTarget();
		if( u_target->getClass() == WARRIOR )
			u_target->CastSpell( u_target, 87545, true ); //str
		else if( u_target->getClass() == PALADIN )
		{
			if( u_target->GetUInt32Value(UNIT_FIELD_STRENGTH) > u_target->GetUInt32Value(UNIT_FIELD_INTELLECT) )
				u_target->CastSpell( u_target, 87545, true );
			else
				u_target->CastSpell( u_target, 87547, true );
		}
		else if( u_target->getClass() == HUNTER )
			u_target->CastSpell( u_target, 87546, true );	//agi
		else if( u_target->getClass() == ROGUE )
			u_target->CastSpell( u_target, 87546, true );	//agi
		else if( u_target->getClass() == PRIEST )
			u_target->CastSpell( u_target, 87548, true );	//spi
		else if( u_target->getClass() == DEATHKNIGHT )
			u_target->CastSpell( u_target, 87545, true );
		else if( u_target->getClass() == SHAMAN )
			u_target->CastSpell( u_target, 87547, true );	//int
		else if( u_target->getClass() == MAGE )
			u_target->CastSpell( u_target, 87547, true );
		else if( u_target->getClass() == WARLOCK )
			u_target->CastSpell( u_target, 87547, true );
		else if( u_target->getClass() == DRUID )
		{
			if( u_target->GetUInt32Value(UNIT_FIELD_STRENGTH) > u_target->GetUInt32Value(UNIT_FIELD_INTELLECT) )
				u_target->CastSpell( u_target, 87545, true );
			else
				u_target->CastSpell( u_target, 87547, true );
		}
	}
	return true;
}

bool ADTH_51532( uint32 i, Aura *aur, bool apply )
{
	if( apply )
	{
		Unit *u_target = aur->GetTarget();
		if( aur->m_modList[0].fixed_amount[0] == 0 )
		{
			aur->m_modList[0].fixed_amount[1] = getMSTime();
			aur->m_modList[0].fixed_amount[0] = 1;
		}
		if( u_target->CombatStatus.IsInCombat() )
		{
			aur->m_modList[0].fixed_amount[0] = 2; //we are in combat
			return true;
		}
		if( aur->m_modList[0].fixed_amount[0] == 2 )
		{
			aur->m_modList[0].fixed_amount[1] = getMSTime();
			aur->m_modList[0].fixed_amount[0] = 1; //we are not in combat
			return true;
		}
		//enough time passed since our combat ? The start boosting our speed
#ifdef _DEBUG
		if( aur->m_modList[0].fixed_amount[1] + 10 * 1000 > (int32)getMSTime() )
#else
		if( aur->m_modList[0].fixed_amount[1] + 2 * 60 * 1000 > (int32)getMSTime() )
#endif
			return true;
		//do not break mount speed
		if( u_target->GetMount() || u_target->IsStealth() )
			return true;
//		u_target->CastSpell( u_target, 48604, true ); //speed for a while - 2min - 300 - Overclock
//		u_target->CastSpell( u_target, 72883, true ); //speed for a while - 2min - 400 - Hurry it up, Buddy
		u_target->CastSpell( u_target, 51559, true ); //speed for a while - 2min - 400 - Hurry it up, Buddy
		aur->m_modList[0].fixed_amount[1] = getMSTime() + 2 * 60 * 1000;
	}
	return true;
}

bool ADTH_48438( uint32 i, Aura *aur, bool apply )
{
	//increase effect with each tick. Starts at 150% and ends with 50%
	if( apply )
	{
		if( aur->m_modList[1].fixed_amount[0] == 0 )
		{
			Unit *Unitc = aur->GetUnitCaster();
			int32 period = GetSpellAmplitude( aur->GetSpellProto(), Unitc, 0, 0 );
			aur->m_modList[1].fixed_amount[0] = MAX( 1, aur->GetDuration() / period );
			aur->m_modList[1].fixed_amount[1] = 0;
			aur->m_modList[1].fixed_amount[2] = 0;
		}
		//with float seems to have a 0.02% error :(
		float	TicksMade = aur->m_modList[1].fixed_amount[1];
		float	TickIncreaseStep = 75.0f / aur->m_modList[1].fixed_amount[0];
		int32	ModChange = float2int32( 75.0f / 2.0f - TicksMade * TickIncreaseStep );
		aur->m_modList[0].m_pct_mod = aur->m_modList[0].m_pct_mod - aur->m_modList[1].fixed_amount[2] + ModChange;
		aur->m_modList[1].fixed_amount[1]++;
		aur->m_modList[1].fixed_amount[2] = ModChange;
	}/**/
	return true;
}

bool ADTH_1343( uint32 i, Aura *aur, bool apply )
{
	if( apply )
	{
		Unit *u = aur->GetTarget();
//		if( u->IsPlayer() &&  u->HasFlag(PLAYER_FLAGS, PLAYER_FLAG_DND) )
			u->CastSpell( u, 99732, true );

	}
	return true;
}

bool ADTH_87916( uint32 i, Aura *aur, bool apply )
{
	if( apply )
	{
		Unit *u = aur->GetTarget();
		if( u->IsPlayer() == false )
			return true;
		Player *p_caster = SafePlayerCast( u );
		if( p_caster->GetStat( STAT_INTELLECT ) > p_caster->GetStat( STAT_AGILITY ) && p_caster->GetStat( STAT_INTELLECT ) > p_caster->GetStat( STAT_STRENGTH ) )
			p_caster->CastSpell( p_caster, 87558, true );
		else if( p_caster->GetStat( STAT_AGILITY ) > p_caster->GetStat( STAT_STRENGTH ) )
			p_caster->CastSpell( p_caster, 87557, true );
		else
			p_caster->CastSpell( p_caster, 87556, true );
	}
	return true;
}

bool ADTH_10848( uint32 i, Aura *aur, bool apply )
{
	if( apply )
	{
		Unit *u = aur->GetTarget();
		if( u->IsPlayer() == false )
			return false;
		Player *p_caster = SafePlayerCast( u );
		if( p_caster->m_bg == NULL )
			return false;
		uint32 TimePassed = aur->GetTimePassed();
		uint32 TimeUntil1TickDeath = 5*60*1000;
		uint32 PCTTimePassed = TimePassed * 100 / TimeUntil1TickDeath;
//		uint32 SumOfPCT = 100 * ( 100 + 1 ) / 2;
		uint32 MaxHP = p_caster->GetMaxHealth();
		uint32 HealthReductionTick = MaxHP * PCTTimePassed / 6 / 100;
		HealthReductionTick = MIN( HealthReductionTick, p_caster->GetHealth() - 1 );
		if( HealthReductionTick > 0 )
			p_caster->SetHealth( p_caster->GetHealth() - HealthReductionTick );
	}
	return true;
}

bool ADTH_76657( uint32 i, Aura *aur, bool apply )
{
	if( apply )
	{
		Unit *u = aur->GetTarget();
		if( u->IsPlayer() == false )
			return false;
		Player *p_caster = SafePlayerCast( u );
		Pet *p = p_caster->GetSummon();
		if( p == NULL )
		{
			aur->m_modList[0].fixed_amount[0] = 0;
			return false;
		}
		//should have value
		int32 var_inc = float2int32( p_caster->GetFloatValue( PLAYER_MASTERY ) * aur->GetSpellProto()->eff[1].EffectBasePoints ) / 100;
		if( var_inc != aur->m_modList[0].fixed_amount[0] )
		{
			//remove old
			for( uint32 school=0;school<SCHOOL_COUNT;school++)
			{
				p->ModDamageDonePct[school] -= aur->m_modList[0].fixed_amount[0];
				p->ModDamageDonePct[school] += var_inc;
			}
			//add new
			aur->m_modList[0].fixed_amount[0] = var_inc;
			//update pet dmg
			p->CalcDamage();
		}
	}
	return true;
}

bool ADTH_52234( uint32 i, Aura *aur, bool apply )
{
	if( apply )
	{
		Unit *u = aur->GetTarget();
		//is it applied ? Should it be ?
		if( aur->m_modList[0].fixed_amount[0] == 0 && u->GetHealthPct() > 35 )
		{
			aur->m_modList[0].fixed_amount[0] = 1;

			if( aur->m_modList[0].m_AuraName == SPELL_AURA_MOD_DAMAGE_PERCENT_DONE )
			{
				aur->mod = &aur->m_modList[0];
				aur->SpellAuraModDamagePercDone( false );
			}
			if( aur->m_modList[1].m_AuraName == SPELL_AURA_REDUCE_ATTACKER_CRICTICAL_HIT_CHANCE_PCT )
			{
				aur->mod = &aur->m_modList[1];
				aur->SpellAuraIncreaseAttackerSpellCrit( false );
			}
		}
		else if( aur->m_modList[0].fixed_amount[0] == 1 && u->GetHealthPct() < 35 )
		{
			aur->m_modList[0].fixed_amount[0] = 0;

			if( aur->m_modList[0].m_AuraName == SPELL_AURA_MOD_DAMAGE_PERCENT_DONE )
			{
				aur->mod = &aur->m_modList[0];
				aur->SpellAuraModDamagePercDone( true );
			}
			if( aur->m_modList[1].m_AuraName == SPELL_AURA_REDUCE_ATTACKER_CRICTICAL_HIT_CHANCE_PCT )
			{
				aur->mod = &aur->m_modList[1];
				aur->SpellAuraIncreaseAttackerSpellCrit( true );
			}
		}
	}
	return true;
}

void RegisterAuraPeriodicDummyTickHandlers()
{
	SpellEntry *sp;
	//Cornered
	REGISTER_HANDLER( 52234 );
	REGISTER_SAMEHANDLER( 53497, 52234 );
	//Master of Beasts - update pet dmg based on mastery
	REGISTER_HANDLER( 76657 );
	//Shroud of Death - making it custom so it decreases your health without triggering any procs and not killing you
	REGISTER_HANDLER( 10848 );
	//Goblin Barbecue Feast
	REGISTER_HANDLER( 87916 );
	//Custom Spell to peroidic cast a spell while afk
	REGISTER_HANDLER( 1343 );
	//Wild Growth
	REGISTER_HANDLER( 48438 );
	//Custom Spell to increase move speed while not in combat
	REGISTER_HANDLER( 51532 );
	//Seafood Magnifique Feast
	REGISTER_HANDLER( 87806 );
	//Resistance is Futile
	REGISTER_HANDLER( 83676 );
	//Sanctity of Battle
	REGISTER_HANDLER( 25956 );
	//Earthen Power
//	REGISTER_HANDLER( 59566 );
}