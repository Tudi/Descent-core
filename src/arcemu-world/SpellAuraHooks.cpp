#include "StdAfx.h"

#define REGISTER_HANDLER( spell ) sp = dbcSpell.LookupEntryForced( spell ); \
	ASSERT( sp->AuraHandleHook == 0 ); \
	sp->AuraHandleHook = &AH_##spell;
#define REGISTER_SAMEHANDLER(spell,copy_from) sp = dbcSpell.LookupEntryForced( spell ); \
	ASSERT( sp->AuraHandleHook == 0 ); \
	sp->AuraHandleHook = &AH_##copy_from;

//register scripted aura add / removes here(ones that are not already dummy ? )
//the idea is to add additional behavior to some spells(auras) without inserting hack checks everywhere
/*
SPELL_EFFECT_OVERRIDE_RETURNS AH_68992( Aura *aur, bool apply, uint8 i )
{
	if( i == 0 )
	{
		if( apply )
		{
			Unit *target = aur->GetTarget();
			if( target->IsPlayer() )
				SafePlayerCast( target )->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_WORGEN_TRANSFORM2);
		}
		else
		{
			Unit *target = aur->GetTarget();
			if( target->IsPlayer() )
				SafePlayerCast( target )->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_WORGEN_TRANSFORM2);
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION; 
}
*/
/*
SPELL_EFFECT_OVERRIDE_RETURNS AH_5487( Aura *aur, bool apply, uint8 i )
{
	if( i == 0 )
	{
		if( apply )
		{
			Unit *target = aur->GetTarget();
			if( target->HasAuraWithNameHash( SPELL_HASH_LEADER_OF_THE_PACK, 0, AURA_SEARCH_PASSIVE ) )
				sp->u_caster->CastSpell( target, 24932, true );	//also cast the area aura
		}
		else
		{
			Unit *target = aur->GetTarget();
			target->removeauras( 24932 );
		}
	}
}/**/
/*
SPELL_EFFECT_OVERRIDE_RETURNS AH_30482( Aura *aur, bool apply, uint8 i )
{
	if( aur->GetSpellProto()->eff[i].EffectApplyAuraName == SPELL_AURA_REDUCE_ATTACKER_CRICTICAL_HIT_CHANCE_PCT )
	{
		if( apply == true )
		{
			Unit *target = aur->GetTarget();
			//if we have the talent Firestarter then the actuall effect will come from spell 86941
			if( target->HasAuraWithNameHash( SPELL_HASH_FIRESTARTER, 0, AURA_SEARCH_PASSIVE ) )
			{
				//cast the new spell. Maybe we are supposed to replace just the effect ID ?
				//casting new spell might help us in proc chains
				target->CastSpell( target, 86941, true );
				return SPELL_EFFECT_OVERRIDE_BREAK_EXECUTION;
			}
		}
		else
		{
			Unit *target = aur->GetTarget();
			//if we have the talent Firestarter then the actuall effect will come from spell 86941
			if( target->HasAuraWithNameHash( SPELL_HASH_FIRESTARTER, 0, AURA_SEARCH_PASSIVE ) )
			{
				target->RemoveAura( 86941 );
				return SPELL_EFFECT_OVERRIDE_BREAK_EXECUTION;
			}
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
*/

SPELL_EFFECT_OVERRIDE_RETURNS AH_82661( Aura *aur, bool apply, uint8 i )
{
	if( aur->GetSpellProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MOD_IGNORE_INTERRUPT )
	{
		Unit *target = aur->GetTarget();
		if( target->IsPlayer() )
		{
			Player *p_target = SafePlayerCast( target );
			if(apply)
			{
				p_target->AddSpellIgnoringMoveInterrupt( SPELL_HASH_STEADY_SHOT );
				p_target->AddSpellIgnoringMoveInterrupt( SPELL_HASH_COBRA_SHOT );
			}
			else
			{
				p_target->RemSpellIgnoringMoveInterrupt( SPELL_HASH_STEADY_SHOT );
				p_target->RemSpellIgnoringMoveInterrupt( SPELL_HASH_COBRA_SHOT );
			}
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS AH_17( Aura *aur, bool apply, uint8 i )
{
	if( apply == false && aur->GetSpellProto()->eff[i].EffectApplyAuraName == SPELL_AURA_SCHOOL_ABSORB )
	{
		Absorb *abs = (Absorb*)aur->temp_custom_structure_holder;
		if( abs && abs->amt <= 0 )
		{
			Unit * caster = aur->GetUnitCaster();
			if( caster )
			{
				Aura *a = caster->HasAuraWithNameHash( SPELL_HASH_RAPTURE, 0, AURA_SEARCH_PASSIVE );
				if( a )
				{
					//ever initialized ?
					if( a->m_modList[0].fixed_amount[0] < (int32)getMSTime() )
					{
						uint32 energize_pct = a->GetSpellProto()->eff[0].EffectBasePoints;
						Aura *b = caster->HasAuraWithNameHash( SPELL_HASH_ITEM___PRIEST_T13_HEALER_4P_BONUS__HOLY_WORD_AND_POWER_WORD__SHIELD_, 0, AURA_SEARCH_PASSIVE );
						if( b )
							energize_pct += b->GetSpellProto()->eff[1].EffectBasePoints;
						caster->Energize( caster, 47755, caster->GetMaxPower( POWER_TYPE_MANA ) * energize_pct / 100, POWER_TYPE_MANA, 0 );	
						a->m_modList[0].fixed_amount[0] = getMSTime() + a->GetSpellProto()->proc_interval;	// 12 seconds
					}
				}
			}
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
/*
SPELL_EFFECT_OVERRIDE_RETURNS AH_81208( Aura *aur, bool apply, uint8 i )
{
	Unit *target = aur->GetTarget();
	if( i == 0 && target->IsPlayer() )
	{
		if( apply )
		{
			aur->mod->fixed_amount[0] = 0;
			//unlearn Holy Word: Chastise
			if( target->HasAuraWithNameHash( SPELL_HASH_REVELATIONS, 0, AURA_SEARCH_PASSIVE ) )
			{
//				SafePlayerCast( target )->addSpell( 88684 );					//Holy Word: Serenity
				SafePlayerCast( target )->SwapActionButtonSpell( 88625, 88684, true, false );
//				SafePlayerCast( target )->removeSpell( 88625, false, 0, 0 );	//Holy Word: Chastise
				aur->mod->fixed_amount[0] = 1;
			}
		}
		else
		{
			//learn Holy Word: Chastise
			if( aur->mod->fixed_amount[0] == 1 )
			{
//				SafePlayerCast( target )->addSpell( 88625 );					//Holy Word: Chastise
				SafePlayerCast( target )->SwapActionButtonSpell( 88684, 88625, false, true );
//				SafePlayerCast( target )->removeSpell( 88684, false, 0, 0 );	//Holy Word: Serenity
			}
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS AH_81206( Aura *aur, bool apply, uint8 i )
{
	Unit *target = aur->GetTarget();
	if( i == 0 && target->IsPlayer() )
	{
		if( apply )
		{
			aur->mod->fixed_amount[0] = 0;
			//unlearn Holy Word: Chastise
			if( target->HasAuraWithNameHash( SPELL_HASH_REVELATIONS, 0, AURA_SEARCH_PASSIVE ) )
			{
//				SafePlayerCast( target )->addSpell( 88685 );					//Holy Word: Serenity
				SafePlayerCast( target )->SwapActionButtonSpell( 88625, 88685, true, false );
//				SafePlayerCast( target )->removeSpell( 88625, false, 0, 0 );	//Holy Word: Chastise
				aur->mod->fixed_amount[0] = 1;
			}
		}
		else
		{
			//learn Holy Word: Chastise
			if( aur->mod->fixed_amount[0] == 1 )
			{
//				SafePlayerCast( target )->addSpell( 88625 );					//Holy Word: Chastise
				SafePlayerCast( target )->SwapActionButtonSpell( 88685, 88625, false, true );
//				SafePlayerCast( target )->removeSpell( 88685, false, 0, 0 );	//Holy Word: Serenity
			}
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}*/

SPELL_EFFECT_OVERRIDE_RETURNS AH_48108( Aura *aur, bool apply, uint8 i )
{
	Unit *target = aur->GetTarget();
	if( i == 0 && target->IsPlayer() )
	{
		if( apply )
		{
//			SafePlayerCast( target )->addSpell( 92315 );						//Pyroblast!
//			SafePlayerCast( target )->removeSpell( 11366, false, true, 92315 );	//Pyroblast!
//			SafePlayerCast( target )->SwapActionButtonSpell( 11366, 92315, true, false );
			if( SafePlayerCast( target )->HasSpell( 11366 ) )
				SafePlayerCast( target )->mSpellReplaces[ 11366 ] = 92315;
		}
		else
		{
//			SafePlayerCast( target )->SwapActionButtonSpell( 92315, 11366, false, true );
//			SafePlayerCast( target )->removeSpell( 92315, false, true, 11366 );	//Pyroblast!
//			SafePlayerCast( target )->removeSpell( 92315, false, false, 0 );	//Pyroblast!
			SafePlayerCast( target )->mSpellReplaces[ 11366 ] = 0;
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS AH_82926( Aura *aur, bool apply, uint8 i )
{
	Unit *target = aur->GetTarget();
	if( i == 0 && target->IsPlayer() )
	{
		if( apply )
		{
//			SafePlayerCast( target )->addSpell( 82928 );					//Aimed Shot!
//			SafePlayerCast( target )->SwapActionButtonSpell( 19434, 82928, true, false );
			SafePlayerCast( target )->mSpellReplaces[ 19434 ] = 82928;
		}
		else
		{
//			SafePlayerCast( target )->SwapActionButtonSpell( 82928, 19434, false, true );
//			SafePlayerCast( target )->removeSpell( 82928, false, 0, 0 );	//Aimed Shot!
			SafePlayerCast( target )->mSpellReplaces[ 19434 ] = 0;
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS AH_11327( Aura *aur, bool apply, uint8 i )
{
	Unit *target = aur->GetTarget();
	if( i == 0 
		&& apply == false 
		&& target 
		&& aur->GetTimeLeft() <= 0	//buff was not removed due to interrupt. Needed to avid exploit : vanish -> backstab -> stealth -> ambush
		)
//		target->CastSpell( target, 1784, true );
		target->CastSpellDelayed( target->GetGUID(), 1784, 50, false );
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
/*
SPELL_EFFECT_OVERRIDE_RETURNS AH_1943( Aura *aur, bool apply, uint8 i )
{
	Unit *target = aur->GetTarget();
	if( i == 0 && target )
	{
		if( apply == true )
		{
			//!!! i think this should defenetly not stack !
			aur->mod->fixed_amount[2] = 0;
			Unit *u = aur->GetUnitCaster();
			if( u )
			{
				Aura *a = u->HasAuraWithNameHash( SPELL_HASH_SANGUINARY_VEIN, 0, AURA_SEARCH_PASSIVE );
				if( a )
				{
					aur->mod->fixed_amount[2] = a->GetSpellProto()->eff[0].EffectBasePoints;
					for(uint32 x=0;x<7;x++)
//						target->DamageTakenPctMod[x] += aur->mod->fixed_amount[2] / 100.0f;
				}
			}
		}
		else
		{
			for(uint32 x=0;x<7;x++)
//				target->DamageTakenPctMod[x] -= aur->mod->fixed_amount[2] / 100.0f;
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}*/
/*
SPELL_EFFECT_OVERRIDE_RETURNS AH_81021( Aura *aur, bool apply, uint8 i )
{
	Unit *target = aur->GetTarget();
	if( i == 0 && target->IsPlayer() )
	{
		if( apply )
		{
//			SafePlayerCast( target )->SwapActionButtonSpell( 6785, 81170, true, false );
			SafePlayerCast( target )->mSpellReplaces[ 6785 ] = 81170;
		}
		else
		{
//			SafePlayerCast( target )->SwapActionButtonSpell( 81170, 6785, false, true );
			SafePlayerCast( target )->mSpellReplaces[ 6785 ] = 81170;
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
} */

SPELL_EFFECT_OVERRIDE_RETURNS AH_33891( Aura *aur, bool apply, uint8 i )
{
	Unit *target = aur->GetTarget();
	if( i == 0 )
	{
		if( apply )
		{
			target->CastSpell( target, 5420, true );	//mod armor and healing
			target->CastSpell( target, 81097, true );	//boost spell 1
			target->CastSpell( target, 81098, true );	//boost spell 2
		}
		else
		{
			target->RemoveAura( 5420,0,AURA_SEARCH_ALL );
			target->RemoveAura( 81097,0,AURA_SEARCH_ALL );
			target->RemoveAura( 81098,0,AURA_SEARCH_ALL );
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS AH_48181( Aura *aur, bool apply, uint8 i )
{
	if( apply == false )
	{
		Unit *target = aur->GetTarget();
		ProcTriggerSpell *pts = target->HasProcSpell( 50091 );
		if( pts )
		{
			int32 dmg_received = pts->created_with_value;
			if( dmg_received > 0 )
			{
				Unit *caster = aur->GetUnitCaster();
				//cast the soul animation
				if( target && caster )
					target->CastSpell( caster, 50091, false );
				//heal the owner
				if( caster )
				{
					SpellEntry *spellInfo = dbcSpell.LookupEntry( 48210 );
					Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
					spell->Init( caster, spellInfo ,true, NULL);
					SpellCastTargets targets2( caster->GetGUID() );	
					spell->forced_basepoints[0] = dmg_received;
//					spell->static_dmg[0] = dmg_received;	//some say demon armor should boost this
					spell->prepare( &targets2 );
				}
			}
		}
		target->UnRegisterProcStruct( NULL, 50091 );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS AH_99256( Aura *aur, bool apply, uint8 i )
{
	Unit *target = aur->GetTarget();
	if( i == 1 )
	{
		if( apply == false )
			target->CastSpell( target, 99257, true );	//on removal the target becomes tormented
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS AH_97121( Aura *aur, bool apply, uint8 i )
{
	Unit *target = aur->GetTarget();
	if( i == 0 && apply == false )
		target->RemoveAuraByNameHash( SPELL_HASH_VICTORIOUS, 0, AURA_SEARCH_POSITIVE, MAX_PASSIVE_AURAS );
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS AH_12043( Aura *aur, bool apply, uint8 i )
{
	Unit *target = aur->GetTarget();
	if( i == 0 && apply == false && target && target->IsPlayer() )
	{
		SafePlayerCast( target )->Cooldown_AddStart( dbcSpell.LookupEntryForced( 12043 ) );
		SafePlayerCast( target )->StartClientCooldown( 12043 );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS AH_15473( Aura *aur, bool apply, uint8 i )
{
	if( i == 0 )
	{
		Unit *target = aur->GetTarget();
		//Shadowform - just visual dummy required for 4.3
		uint32 SpellId = 107903;
		if( target->IsPlayer() && SafePlayerCast( target )->HasGlyphWithID( GLYPH_PRIEST_SHADOWFORM ) )
			SpellId = 107904;
		if( apply )
			target->CastSpell( target, SpellId, true );
		else
			target->RemoveAura( SpellId,0,AURA_SEARCH_ALL );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS AH_31821( Aura *aur, bool apply, uint8 i )
{
	if( i == 0 )
	{
		Unit *target = aur->GetTarget(); //this is actually caster
		Aura *a;
		if( apply )
		{
			InRangeSetRecProt::iterator itr;
			target->AquireInrangeLock(); //make sure to release lock before exit function !
			InrangeLoopExitAutoCallback AutoLock;
			for( itr = target->GetInRangeSetBegin( AutoLock ); itr != target->GetInRangeSetEnd(); itr++ )
			{
				if(!((*itr)->IsUnit()) || !SafeUnitCast((*itr))->isAlive())
					continue;
				a = SafeUnitCast((*itr))->HasAuraWithNameHash( SPELL_HASH_CONCENTRATION_AURA, target->GetGUID(), AURA_SEARCH_POSITIVE );
				if( a != NULL )
					target->CastSpell( SafeUnitCast((*itr)), 64364, true ); //immunity
			}
			target->ReleaseInrangeLock();

			a = target->HasAuraWithNameHash( SPELL_HASH_CONCENTRATION_AURA, target->GetGUID(), AURA_SEARCH_POSITIVE );
			if( a != NULL )
			{
				target->CastSpell( target, 64364, true ); //immunity
			}
			else
			{
				if( a == NULL )
					a = target->HasAuraWithNameHash( SPELL_HASH_DEVOTION_AURA, target->GetGUID(), AURA_SEARCH_POSITIVE );
				if( a == NULL )
					a = target->HasAuraWithNameHash( SPELL_HASH_RESISTANCE_AURA, target->GetGUID(), AURA_SEARCH_POSITIVE );
				if( a == NULL )
					a = target->HasAuraWithNameHash( SPELL_HASH_RETRIBUTION_AURA, target->GetGUID(), AURA_SEARCH_POSITIVE );
				if( a != NULL )
				{
					SpellEntry *sp = a->GetSpellProto();
					a->Remove();
					a = NULL;
					target->CastSpellDelayed( target->GetGUID(), sp->Id, 1, true );
				}
			}
		}
		else
		{
			target->RemoveAura( 64364 );	//immunity
			a = target->HasAuraWithNameHash( SPELL_HASH_DEVOTION_AURA, target->GetGUID(), AURA_SEARCH_POSITIVE );
			if( a == NULL )
				a = target->HasAuraWithNameHash( SPELL_HASH_RESISTANCE_AURA, target->GetGUID(), AURA_SEARCH_POSITIVE );
			if( a == NULL )
				a = target->HasAuraWithNameHash( SPELL_HASH_RETRIBUTION_AURA, target->GetGUID(), AURA_SEARCH_POSITIVE );
			if( a != NULL )
			{
				SpellEntry *sp = a->GetSpellProto();
				a->Remove();
				a = NULL;
				target->CastSpellDelayed( target->GetGUID(), sp->Id, 1, true );
			}
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
/*
SPELL_EFFECT_OVERRIDE_RETURNS AH_110311( Aura *aur, bool apply, int i )
{
	Unit *target = aur->GetTarget();
	if( i == 0 && apply == false && target )
		target->CastSpellDelayed( target->GetGUID(), 96243, 50 );
//	else if( target && target->HasAura( 31574 ) && aur->GetTimeLeft() == 1 )
//		target->CastSpell( target->GetGUID(), 96243, true);
//	else if( target && target->HasAura( 31575 ) && aur->GetTimeLeft() == 2 )
//		target->CastSpell( target->GetGUID(), 96243, true );
//	else if( target && target->HasAura( 54354 ) )
//		target->CastSpell( target->GetGUID(), 96243, true );
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}*/

SPELL_EFFECT_OVERRIDE_RETURNS AH_106368( Aura *aur, bool apply, uint8 i )
{
	Unit *target = aur->GetTarget();
	//cast Heroic Will that will shield us for 5 seconds 
	if( i == 0 &&  target )
	{
		if( apply == false && ( aur->m_flags & WAS_REMOVED_ON_PLAYER_REQUEST ) )
			target->CastSpell( target->GetGUID(), 106108, 1 );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS AH_106108( Aura *aur, bool apply, uint8 i )
{
	Unit *target = aur->GetTarget();
	//cast Twilight Shift that will take us into phase 2 
	if( i == 0 && target )
	{
		if( apply == false ) 
			target->CastSpellDelayed( target->GetGUID(), 106368, 50, true );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS AH_105925( Aura *aur, bool apply, uint8 i )
{
	Unit *target = aur->GetTarget();
	if( i == 0 && target )
	{
		//if we have heroic will and we will not receive any dmg from us then we cast "Faded Into Twilight"
		if( apply == false )
		{
			if( target->HasAura( 106108 ) ) 
				target->CastSpellDelayed( target->GetGUID(), 109461, 50, true );
			else
				target->CastSpellDelayed( target->GetGUID(), 110073, 50, true ); //fading light insta kill if possible
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS AH_106199( Aura *aur, bool apply, uint8 i )
{
	Unit *target = aur->GetTarget();
	if( i == 0 && target )
	{
		//on dispel we jump to a new target
		if( aur->m_flags & WAS_REMOVED_ON_DISPEL )
		{
			uint32 NewSpellId;
			if( RandomUInt( 70 ) )
				NewSpellId = 106199;	//Blood Corruption: Death
			else
				NewSpellId = 106200;	//Blood Corruption: Earth
			AI_Spell sp;
			sp.spelltargetType = TTYPE_AT_RANGE_PLAYER_RANDOM;
			sp.minrange = 0.0f;
			sp.maxrange = 200;
			Unit *t = target->GetAIInterface()->FindTargetForSpellTargetType( &sp );
			if( t )
				t->CastSpell( t, NewSpellId, true );
		}
		//on expire we apply our evil buff
		else
		{
			if( aur->GetSpellId() == 106199 )
				target->CastSpell( target, 106201, true );
			else
				target->CastSpell( target, 106213, true );
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS AH_44457( Aura *aur, bool apply, uint8 i )
{
	if( apply == false )
	{
		if( ( aur->m_flags & WAS_REMOVED_ON_DISPEL ) || aur->GetTimeLeft() < 1000 )
		{
			Unit *ucaster = aur->GetUnitCaster();
			if( ucaster )
				ucaster->CastSpell( aur->GetTarget(), aur->GetSpellProto()->eff[1].EffectTriggerSpell, true );
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS AH_25956( Aura *aur, bool apply, uint8 i )
{
	if( apply == false )
	{
		if( aur->GetTarget()->IsPlayer() == false )
			return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
		Player *p = SafePlayerCast( aur->GetTarget() );
		int32 Mod = -aur->m_modList[0].fixed_amount[0];
		if( Mod != 0 )
		{
			aur->m_modList[0].fixed_amount[0] = 0;
			uint32 *AffectedGroups = aur->GetSpellProto()->eff[0].EffectSpellGroupRelation;
			aur->SendModifierLog( &p->SM_Mods->SM_PCooldownTime, Mod, AffectedGroups, aur->GetSpellProto()->eff[0].EffectMiscValue, true ); 
		}

	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS AH_33763( Aura *aur, bool apply, uint8 i )
{
	if( apply == false )
	{
		Unit *m_target = aur->GetTarget();
		if( m_target->isAlive() == false )
		{ 
			return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
		}
		//do not proc if this is getting removed due to aplying to new target ( from script )
		if( aur->GetTimeLeft() > 500 && ( ( aur->m_flags & WAS_REMOVED_ON_DISPEL ) == 0 ) )
		{ 
			return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
		}
		// apply ONCE only.
		int64 *LockExecution = (int64*)m_target->GetCreateIn64Extension( EXTENSION_ID_LIFEBLOOM_DISABLER ); //"lifebloomdisabler"
		if( *LockExecution >= getMSTime() )
		{ 
			return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
		}
		*LockExecution = getMSTime() + 1000;

		//after we made sure the stacks will not heal due to auto remove. We can exit this function
		if( ( aur->m_flags & ( WAS_REMOVED_ON_PLAYER_REQUEST | WAS_REMOVED_ON_SCRIPT_REQUEST ) ) != 0 )
			return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;

		Unit * pCaster = aur->GetUnitCaster();
		if( pCaster == NULL )
			pCaster = m_target;

		// Remove other Lifeblooms - but do NOT handle unapply again
//		bool expired = true;
/*		for(uint32 x=MAX_AURAS;x<MAX_POSITIVE_AURAS1( m_target );x++)
		{
			if(m_target->m_auras[x] && m_target->m_auras[x]->GetSpellId() == aur->GetSpellId() )
			{
				if( m_target->m_auras[x]->GetTimeLeft() != 0 )
					expired = false;
				m_target->m_auras[x]->Remove();
			}
		} */
//		if( expired )
		{

/*
Heals the target for ${$o1*$<mult>} over $d.  When Lifebloom expires or is dispelled, the target is instantly healed for ${$m2*$<bloom>}. 
This effect can stack up to $u times on the same target. $?a33891[][Lifebloom can be active only on one target at a time.]$?s33891[
|C0033AA11Tree of Life: Can be cast on unlimited targets.|R][]

176,"$genesis1=$?s57810[${1+0.01*$57810m1}][${1}]
$genesis2=$?s57811[${1+0.01*$57811m1}][${$<genesis1>}]
$genesis3=$?s57812[${1+0.01*$57812m1}][${$<genesis2>}]
$mult=${$<genesis3>}
$mstrshape=$?s48411[${1+0.01*$48411m1}][${1}]
$gotem1=$?s51179[${1+0.01*$51179m1}][${1}]
$gotem2=$?s51180[${1+0.01*$51180m1}][${$<gotem1>}]
$gotem3=$?s51181[${1+0.01*$51181m1}][${$<gotem2>}]
$tol=$?a5420[${1+0.01+$5420m1}][${1}]
$gift=$?s87305[${1.25}][${1}]
$bloom=${$<gift>*$<genesis3>*$<mstrshape>*$<gotem3>*$<tol>}",
5420 - Tree of Life
48411 - Master Shapeshifter
51181 - Gift of the Earthmother
57812 - Genesis - balance tree
87305 - Gift of Nature - 
*/
			int32 earthmother = 0;
			int32 LifeBloomCount = m_target->CountAuraNameHash( SPELL_HASH_LIFEBLOOM, AURA_SEARCH_POSITIVE );
			Spell *spell=SpellPool.PooledNew( __FILE__, __LINE__ );
			spell->Init( pCaster, aur->GetSpellProto(), true, NULL );
			SpellCastTargets targets2( m_target->GetGUID() );
//			spell->SetUnitTarget( m_target );
			{
				Aura *a = pCaster->HasAuraWithNameHash( SPELL_HASH_GIFT_OF_THE_EARTHMOTHER, 0, AURA_SEARCH_PASSIVE );
				if( a )
					earthmother = a->GetSpellProto()->eff[0].EffectBasePoints;
			}
			spell->redirected_effect[0] = SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_ALL_TARGETS;
			spell->forced_pct_mod[1] += LifeBloomCount * 100;
			spell->forced_pct_mod[1] = ( spell->forced_pct_mod[1] * ( 100 + earthmother ) ) / 100;
			spell->redirected_effect[2] = SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_ALL_TARGETS;
			spell->prepare( &targets2 );
/*			{
				int32 dmg = spell->CalculateEffect( 1, m_target );
				earthmother = earthmother * dmg / 100;
				spell->Heal( dmg, false, 1 );	
				spell->Heal( earthmother, false, 1 );	
			}
			SpellPool.PooledDelete( spell );*/
			/*
			//was not able to find earth mother combat log ID 
			if( earthmother != 0 )
			{
				SpellEntry *spearthmother = dbcSpell.LookupEntryForced( 51180 );
				Spell *spell=SpellPool.PooledNew( __FILE__, __LINE__ );
				spell->Init(pCaster, spearthmother, true, NULL);
				spell->SetUnitTarget( m_target );
				spell->Heal( earthmother );	
				SpellPool.PooledDelete( spell );
			}*/
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS AH_16191( Aura *aur, bool apply, uint8 i )
{
	if( apply == true && aur->GetSpellProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MOD_STAT )
	{
		Unit *c = aur->GetUnitCaster();
		if( c )
		{
			Unit *owner = c->GetTopOwner();
			aur->mod->m_amount = aur->GetSpellProto()->eff[i].EffectBasePoints * owner->GetStat( STAT_SPIRIT ) / 100;
			//my max health should be owner's max health
			uint32 MyMaxHealth = owner->GetMaxHealth() * 10 / 100;
			if(c->GetMaxHealth() < MyMaxHealth )
			{
				c->SetMaxHealth( MyMaxHealth );
				c->SetHealth( MyMaxHealth );
			}
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS AH_642( Aura *aur, bool apply, uint8 i )
{
	if( apply == true && aur->GetSpellProto()->eff[i].EffectApplyAuraName == SPELL_AURA_SCHOOL_IMMUNITY )
	{
		//remove dots from target
		Unit *t = aur->GetTarget();
		for(uint32 x=MAX_POSITIVE_AURAS;x<MAX_NEGATIVE_AURAS1(t);x++)
		{
			Aura *pAura = t->m_auras[i];
			if( pAura != aur && pAura != NULL && !pAura->IsPassive() && !pAura->IsPositive() && !(pAura->GetSpellProto()->Attributes & ATTRIBUTES_IGNORE_INVULNERABILITY) )
				pAura->Remove();
		}
		t->RemoveAurasByInterruptFlag( AURA_INTERRUPT_ON_INVINCIBLE );

		for(uint32 i = 0; i < SCHOOL_COUNT; i++)
			if( aur->mod->m_miscValue & (1<<i) )
				t->RemoveAurasOfSchool(i, false, true); //hmm, script this if you need. Cyclone does not need to remove DOTS, just make you immune to dmg
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
/*
SPELL_EFFECT_OVERRIDE_RETURNS AH_44572( Aura *aur, bool apply, uint8 i )
{
	if( i == 0 )
	{
		Unit *t = aur->GetTarget();
		if( apply == true )
			t->SetAuraStateFlag( AURASTATE_FROZEN );
		else
			t->RemoveAuraStateFlag( AURASTATE_FROZEN );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}*/

SPELL_EFFECT_OVERRIDE_RETURNS AH_108649( Aura *aur, bool apply, uint8 i )
{
	if( i == 1 && apply == false )
	{
		Unit *t = aur->GetTarget();
		if( t->GetMapId() != 967 )
			return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
		//create a parasite
		uint32 cr_entry = aur->GetSpellProto()->eff[1].EffectMiscValue;
		CreatureProto * proto = CreatureProtoStorage.LookupEntry(cr_entry);
		CreatureInfo * info = CreatureNameStorage.LookupEntry(cr_entry);
		if(!proto || !info)
		{
			sLog.outDetail("Warning : Missing summon creature template %u used by spell %u!",cr_entry,aur->GetSpellProto()->Id);
			return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
		}
		float x, y, z;
		Creature * p = t->GetMapMgr()->CreateCreature(cr_entry);
		x = t->GetPositionX();
		y = t->GetPositionY();
		z = t->GetPositionZ();
		p->Load(proto, x, y, z);
//		p->SetZoneId( t->GetZoneId() );
		p->SetUInt32Value( UNIT_FIELD_FACTIONTEMPLATE, proto->Faction );
		p->m_faction = dbcFactionTemplate.LookupEntry(proto->Faction);
		if(p->m_faction)
			p->m_factionDBC = dbcFaction.LookupEntry(p->m_faction->Faction);
		p->PushToWorld( t->GetMapMgr() );
		//make sure they will be desumonized (roxor)
		sEventMgr.AddEvent( p, &Creature::SummonExpire, EVENT_SUMMON_EXPIRE, 5*60*1000, 1,0);
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS AH_106464( Aura *aur, bool apply, uint8 i )
{
	Unit *target = aur->GetTarget();
	//cast Enter the dream that will shield us for 5 seconds 
	if( i == 0 &&  target )
	{
		if( apply == false && ( aur->m_flags & WAS_REMOVED_ON_PLAYER_REQUEST ) )
			target->CastSpell( target->GetGUID(), 106466, 1 );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS AH_48263( Aura *aur, bool apply, uint8 i )
{
	Unit *target = aur->GetTarget();
	//cast Enter the dream that will shield us for 5 seconds 
	if( i == 0 &&  target )
	{
		if( apply )
		{
			target->SetAuraStateFlag( AURASTATE_DODGE_BLOCK ); //yes, it's 2 times because the spell will keep removing it
			target->SetAuraStateFlag( AURASTATE_DODGE_BLOCK );
		}
		else
		{
			target->RemoveAuraStateFlag( AURASTATE_DODGE_BLOCK );
			target->RemoveAuraStateFlag( AURASTATE_DODGE_BLOCK );
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS AH_33786( Aura *aur, bool apply, uint8 i )
{
	Unit *target = aur->GetTarget();
	//avoid ice block and buble cast from paladin
	if( i == 0 &&  target )
	{
		aur->SpellAuraModSilence( apply );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}/**/

SPELL_EFFECT_OVERRIDE_RETURNS AH_81660( Aura *aur, bool apply, uint8 i )
{
	Unit *target = aur->GetTarget();
	//on expire remove enabler also
	if( apply == false && i == 0 && target && ( aur->GetTimeLeft() < 500 || ( aur->m_flags & WAS_REMOVED_ON_PLAYER_REQUEST ) ) )
		target->RemoveAura( 94709, 0, AURA_SEARCH_ALL, MAX_AURAS );
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

void RegisterAuraHooks()
{
	SpellEntry *sp;
	//Evangelism
	REGISTER_HANDLER( 81660 );
	REGISTER_SAMEHANDLER( 87117, 81660 ); // Dark Evangelism
	REGISTER_SAMEHANDLER( 81661, 81660 ); 
	REGISTER_SAMEHANDLER( 87118, 81660 ); 
	//Blood Presence
	REGISTER_HANDLER( 48263 );
	//Enter the dream - madness boss spell
	REGISTER_HANDLER( 106464 );
	REGISTER_SAMEHANDLER( 106457, 106464 ); // Ysera presence
	//Corrupting Parasite - madness boss spell
	REGISTER_HANDLER( 108649 );
	//Deep Freeze
//	REGISTER_HANDLER( 44572 );
	//cyclone
	REGISTER_HANDLER( 33786 ); 
	REGISTER_HANDLER( 642 ); // divine shield
	REGISTER_SAMEHANDLER( 45438, 642 ); // Ice Block
	//Mana Tide - Mana Tide Totem
	REGISTER_HANDLER( 16191 );
	//LifeBloom
	REGISTER_HANDLER( 33763 );
	//Sanctity of Battle
	REGISTER_HANDLER( 25956 );
	//living bomb 
	REGISTER_HANDLER( 44457 );
	//Blood Corruption: Death - boss script Spine of Deathwing 
	REGISTER_HANDLER( 106199 );
	REGISTER_SAMEHANDLER( 106200, 106199 ); // Blood Corruption: Earth
	//Fading Light - boss script Ultraxion 
	REGISTER_HANDLER( 105925 );
	//Heroic Will - boss script Ultraxion 
	REGISTER_HANDLER( 106108 );
	//Twilight Shift - boss script Ultraxion 
	REGISTER_HANDLER( 106368 );
	//Aura Mastery
	REGISTER_HANDLER( 31821 );
	//Shadowform
	REGISTER_HANDLER( 15473 );
	//Presence of Mind
	REGISTER_HANDLER( 12043 );
	//Victory
	REGISTER_HANDLER( 97121 );
	REGISTER_SAMEHANDLER( 96908, 97121 );
	//Torment - PVE boss spell
	REGISTER_HANDLER( 99256 );
	//Haunt
	REGISTER_HANDLER( 48181 );
	//Tree of Life - has a bunch of additional casts
	REGISTER_HANDLER( 33891 );
	//Stampede - ravage free version
//	REGISTER_HANDLER( 81021 );
//	REGISTER_SAMEHANDLER( 81022, 81021 ); 
	//Rupture - for talent Sanguinary Vein
//	REGISTER_HANDLER( 1943 );
//	REGISTER_SAMEHANDLER( 703, 1943 ); //Garrote - for talent Sanguinary Vein
	//vanish removal leads to stealth
	REGISTER_HANDLER( 11327 );
	//Master Marksman - Aimed Shot!
	REGISTER_HANDLER( 82926 );
	//Hot Streak - Pyroblast!
	REGISTER_HANDLER( 48108 );
	//Chakra: Serenity
//	REGISTER_HANDLER( 81208 );
	//Chakra: Sanctuary
//	REGISTER_HANDLER( 81206 );
	//Power Word: Shield -> rapture
	REGISTER_HANDLER( 17 );
	//Molten Armor - Firestarter - Your Molten Armor allows you to cast the Scorch spell while moving instead of reducing the chance you are critically hit.
//	REGISTER_HANDLER( 30482 );
	//aspect of the fox
	REGISTER_HANDLER( 82661 );
	//bear form, in cast we have talent Leader of the Pack the we will cast the area aura
//	REGISTER_HANDLER( 5487 );
	//bear form, in cast we have talent Leader of the Pack the we will cast the area aura
//	REGISTER_SAMEHANDLER( 768, 5487 );
	//Darkflight, worgen's sprint spell.
//	REGISTER_HANDLER( 68992 );
}