#include "StdAfx.h"

#define REGISTER_HANDLER( spell ) sp = dbcSpell.LookupEntryForced( spell ); \
	ASSERT( sp->EffectHandler == 0 ); \
	sp->EffectHandler = &EH_##spell;
#define REGISTER_SAMEHANDLER(spell,copy_from) sp = dbcSpell.LookupEntryForced( spell ); \
	ASSERT( sp->EffectHandler == 0 ); \
	sp->EffectHandler = &EH_##copy_from;

///////////////////////////////////////////////////////////////////////////////////////////
// Static Spell Functions used in Spell.cpp to calculate spell effect value
///////////////////////////////////////////////////////////////////////////////////////////
/*
SPELL_EFFECT_OVERRIDE_RETURNS HandleSteadyShot(Spell *sp,Unit *target, int32 &value, int32 i)
{
	//A steady shot that causes ${$RAP*0.3+$m1} damage.
	//	Actual Equation (http://www.wowwiki.com/Steady_Shot)
	//		* The tooltip is proven to be wrong and the following is the best player worked out formula so far with data taken from [1]
	//		* Formula: DamagePercentageBonus*RangedWeaponSpecialization*(150 + WeaponDamage/WeaponSpeed*2.8 + 0.2*RAP + [Dazed: 175])
	if(i==0 && sp->u_caster)
	{
		if( sp->p_caster != NULL )
		{
			Item *it;
			if(sp->p_caster->GetItemInterface())
			{
				it = sp->p_caster->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_RANGED);
				if(it)
				{
					float weapondmg = RandomFloat(1) * it->GetProto()->getDPS();
					value += float2int32( sp->GetProto()->eff[0].EffectBasePoints + weapondmg*2.8f);
				}
			}
		}
		if(target && target->IsDazed() )
			value += sp->GetProto()->eff[1].EffectBasePoints;
		value += (uint32)(sp->u_caster->GetRAP() * 0.1f);
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}*/

SPELL_EFFECT_OVERRIDE_RETURNS HandleDeadlyPoison(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( (sp->GetProto()->c_is_flags & SPELL_FLAG_IS_POISON) && sp->GetProto()->eff[i].EffectApplyAuraName==SPELL_AURA_PERIODIC_DAMAGE && sp->u_caster )
	{
		//Each strike has a $2823h% chance of poisoning the enemy for ${$2818m1*4+0.12*$AP} Nature damage over $2818d.  Stacks up to 5 times on a single target.
		value = value * 4 + sp->u_caster->GetAP() * 12 / 100;
		value = value / sp->GetProto()->quick_tickcount;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleInstantPoison(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( (sp->GetProto()->c_is_flags & SPELL_FLAG_IS_POISON) && i==0 && sp->GetProto()->eff[i].Effect==SPELL_EFFECT_SCHOOL_DAMAGE && sp->u_caster )
	{
		//Each strike has a $57967h% chance of poisoning the enemy which instantly inflicts ${$57964m1+0.10*$AP} Nature damage.
		value = value + sp->u_caster->GetAP() * 10 / 100;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleWoundPoison(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( (sp->GetProto()->c_is_flags & SPELL_FLAG_IS_POISON) && i==1 && sp->GetProto()->eff[i].Effect==SPELL_EFFECT_SCHOOL_DAMAGE && sp->u_caster )
	{
		//Each strike has a $57967h% chance of poisoning the enemy which instantly inflicts ${$57964m1+0.10*$AP} Nature damage.
		value = value + sp->u_caster->GetAP() * 4 / 100;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleArcaneShot(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i==0 && sp->u_caster )//An instant shot that causes [RAP * 0.15 + 492] Arcane damage.
	{
		value = (int32)( sp->u_caster->GetRAP() * 0.042f ) + value;
//		if( u_caster->HasAurasOfNameHashWithCaster(SPELL_HASH_RAPID_KILLING, u_caster) ) //Arcane shot deals Y% extra damage with rapid killing buff
//			value += (uint32)(value * GetProto()->RankNumber / 10);
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleSerpentSting(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i==0 && sp->u_caster )//Stings the target, causing ${$RAP*0.2+$m1*5} Nature damage over $d.  Only one Sting per Hunter can be active on any one target.
		value = (int32)((( sp->u_caster->GetRAP() * 0.20f ) + value * 5) / sp->GetProto()->quick_tickcount );
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleKillShot(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i==0 && sp->u_caster )//An instant shot that causes [RAP * 0.15 + 492] Arcane damage.
		value = (int32)( sp->u_caster->GetRAP() * 0.40f ) + value;
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleEviscerate(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->p_caster != NULL ) //Eviscerate
	{
		//1 point  : ${$m1+(($b1*1)+$AP*0.03)*$<mult>}-${$M1+(($b1*1)+$AP*0.07)*$<mult>} damage
		//  1 point : (177 + ((536 * 1) + AP * 0.091) * 1)-(529 + ((536 * 1) + AP * 0.091) * 1) damage
		//	2 points: (177 + ((536 * 2) + AP * 0.182) * 1)-(529 + ((536 * 2) + AP * 0.182) * 1) damage
		//	3 points: (177 + ((536 * 3) + AP * 0.273) * 1)-(529 + ((536 * 3) + AP * 0.273) * 1) damage
		//	4 points: (177 + ((536 * 4) + AP * 0.364) * 1)-(529 + ((536 * 4) + AP * 0.364) * 1) damage
		//	5 points: (177 + ((536 * 5) + AP * 0.455) * 1)-(529 + ((536 * 5) + AP * 0.455) * 1) damage

		uint32 point_count = sp->p_caster->m_comboPoints;
		if( point_count )
		{
			//formula is not perfect because value contains some randomness and that gets multiplied by max 5 times
			uint32 point_count = sp->p_caster->m_comboPoints;
			uint32 dolarB = value - sp->damage;
			uint32 dolarM = value - dolarB;
			uint32 min_value = dolarM + float2int32( ( dolarB + sp->p_caster->GetAP() * 0.091f) * point_count );
			uint32 rand_max  = value + 1;
			uint32 random = RandomUInt() % rand_max;
			value = min_value + random;
//			sp->p_caster->NullComboPoints();	//just in case spell is missing combo cost
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleDivineSpiritPrayerOfSpirit(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i>0 && sp->p_caster )
	{
		Aura *a;
		a = sp->p_caster->HasAura( 33174 );
		if( a && a->m_modcount && a->mod )
			value = (sp->GetProto()->eff[0].EffectBasePoints + 1) * a->mod->m_amount / 100;
		else
		{
			a = sp->p_caster->HasAura( 33182 );
			if( a && a->m_modcount && a->mod )
				value = (sp->GetProto()->eff[0].EffectBasePoints + 1) * a->mod->m_amount / 100;
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

/*
SPELL_EFFECT_OVERRIDE_RETURNS HandleSealOfLight(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->u_caster )
	{
		//each melee attack a chance to heal the Paladin for ${0.15*$AP+0.15*$SPH}.  Only one Seal can be active on the Paladin at any one time.
		value = ( sp->u_caster->GetAP() + sp->u_caster->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SCHOOL_HOLY) ) * 15 / 100;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleFlashOfLight(Spell *sp,Unit *target, int32 &value, int32 i)
{
	//description does not say it procs a new flash of light anymore( only in 320).
	//this is :Infusion of Light talent
	if( sp->GetProto()->Id != 66922 && target && target->HasAuraWithNameHash( SPELL_HASH_SACRED_SHIELD ) )	//no cyclic casts
	{
		SpellEntry *spellInfo = dbcSpell.LookupEntry( 66922 ); //sadly this value is only in description :(
		Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		spell->Init( sp->m_caster, spellInfo ,true, NULL );
		spell->forced_basepoints[0] = value * ( spellInfo->eff[0].EffectBasePoints + 1 ) / 100 / spellInfo->quick_tickcount;	//another x%
		SpellCastTargets targets2( target->GetGUID() );
		spell->prepare(&targets2);
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}*/

SPELL_EFFECT_OVERRIDE_RETURNS HandleHysteria(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( target && i == 1 )
	{
//		value = target->GetUInt32Value( UNIT_FIELD_BASE_HEALTH ) * value / 100;
		value = target->GetUInt32Value( UNIT_FIELD_MAXHEALTH ) * value / 100;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleRend(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( target && sp->p_caster )
	{
		if( sp->GetProto()->eff[ i ].Effect == SPELL_EFFECT_SCHOOL_DAMAGE )
		{
			value = value * 8;	//this is 101% wrong, but to be as tooltip says. probably i'm modding wrong spell ? There are 2 rend spells, first casts a second
		}
		if( sp->GetProto()->eff[ i ].EffectApplyAuraName == SPELL_AURA_PERIODIC_DAMAGE )
		{
			// Wounds the target causing them to bleed for $o1 damage plus an additional ${0.2*5*(($MWB+$mwb)/2+$AP/14*$MWS)} (based on weapon damage) over $d.
			Item *it;
			if( sp->p_caster->GetItemInterface() )
			{
				it = sp->p_caster->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_MAINHAND);
				if(it)
				{
					float weapondmg = it->GetProto()->getDPS();
					float MVS = it->GetProto()->Delay;
					if( it->GetProto()->InventoryType == INVTYPE_2HWEAPON )
						MVS *= 2.12f;	//to be according to client shown value
					float tvalue =  0.25f * 6.0f * (weapondmg + (float)sp->p_caster->GetAP()*MVS/14000.0f);
					//an additional (0.25 * 6 * ((MWB + mwb) / 2 + AP / 14 * MWS))
					value = value + float2int32( tvalue );
					value = value / sp->GetProto()->quick_tickcount; //per tick ofc
				}
			}
		}
//		if( sp->GetProto()->eff[2].EffectBasePoints && target->GetHealthPct() > 75 )
//			value = value * (sp->GetProto()->eff[2].EffectBasePoints + 1 ) / 100;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
/*
SPELL_EFFECT_OVERRIDE_RETURNS HandleFerociousBite(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->p_caster != NULL  )
	{
		value += (uint32)( ( sp->p_caster->GetAP() * 0.1526f ) + ( sp->p_caster->GetPower( POWER_TYPE_ENERGY ) * sp->GetProto()->eff[i].EffectConversionCoef ) );
		sp->p_caster->SetPower( POWER_TYPE_ENERGY, 0 );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
SPELL_EFFECT_OVERRIDE_RETURNS HandleFaerieFire(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->u_caster && i == 2 )
	{
		//Deals ${$AP*0.15+1} damage and additional threat when used in Bear Form or Dire Bear Form.
		value = sp->u_caster->GetAP() * 15 / 100 + 1;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleReplenishment1(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i==0 )
		value = target->GetMaxPower( POWER_TYPE_MANA ) * 25 / 100;
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
*/

/*
SPELL_EFFECT_OVERRIDE_RETURNS HandleVictoryRush(Spell *sp,Unit *target, int32 &value, int32 i)
{
	//causing ${$AP*$m1/100} damage
	if(i==0 && sp->u_caster)
		value = (value*sp->u_caster->GetAP())/100;
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
*/
/*
SPELL_EFFECT_OVERRIDE_RETURNS HandleOwlkinFrenzy(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i==2 && sp->u_caster )
	{ // restore $s3% base mana every $T3 sec
		value = value * sp->u_caster->GetPower( POWER_TYPE_MANA ) / 100;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}*/
/*
SPELL_EFFECT_OVERRIDE_RETURNS HandleRake(Spell *sp,Unit *target, int32 &value, int32 i)
{
	//Rake the target for ${$AP/100+$m1} bleed damage and an additional ${$m2*3+$AP*0.06} damage over $d.
	if( sp->u_caster != NULL )
	{
		float ap = (float)sp->u_caster->GetAP();
		if(i==0)
			value+=(uint32)ceilf((ap*0.01f));	// / 100
		else if(i==1)
			value=(int32)ceilf((float(value * 3) + ceilf((ap*0.06f))) / 3.0f);
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}*/

SPELL_EFFECT_OVERRIDE_RETURNS HandleGarotte(Spell *sp,Unit *target, int32 &value, int32 i)
{
	// WoWWiki says +( 0.18 * attack power / number of ticks )
	// Tooltip gives no specific reading, but says ", increased by your attack power.".
	if( sp->u_caster != NULL && i == 0 )
			value += (uint32) ceilf( ( sp->u_caster->GetAP() * 0.18f ) / 6 );
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleRupture(Spell *sp,Unit *target, int32 &value, int32 i)
{
//	1pt = Attack Power * 0.04 + x
//	2pt = Attack Power * 0.10 + y
//	3pt = Attack Power * 0.18 + z
//	4pt = Attack Power * 0.21 + a
//	5pt = Attack Power * 0.24 + b
/*
Finishing move that causes damage over time, increased by your attack power.  Lasts longer per combo point:
  1 point  : ((141 + 20 * 1 * 1 + 0.015 * AP * 1) * 0.5 * 8) damage over 8 seconds
  2 points: ((141 + 20 * 1 * 2 + 0.024 * AP * 1) * 0.5 * 10) damage over 10 seconds
  3 points: ((141 + 20 * 1 * 3 + 0.03 * AP * 1) * 0.5 * 12) damage over 12 seconds
  4 points: ((141 + 20 * 1 * 4 + 0.03428571 * AP * 1) * 0.5 * 14) damage over 14 seconds
  5 points: ((141 + 20 * 1 * 5 + 0.0375 * AP * 1) * 0.5 * 16) damage over 16 seconds
  */
	if( sp->p_caster != NULL && i == 0 )
	{
		int8 cp = sp->p_caster->m_comboPoints;
		static float point_coef[6]={0.015f,0.015f,0.024f,0.03f,0.03428571f,0.0375f};
		if( cp < 6 )
			value += 10 * cp + float2int32( sp->u_caster->GetAP() * point_coef[ cp ] );
		value = value * 2;	//we tick every 2 seconds ( tooltip is inverse )

		uint32 new_dutaion = 6000 + 2000 * cp;
		sp->forced_duration = new_dutaion;
		sp->Dur = new_dutaion;
//		sp->p_caster->NullComboPoints();	//just in case spell is missing combo cost
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
/*
SPELL_EFFECT_OVERRIDE_RETURNS HandleRip(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->u_caster != NULL )
		value += (uint32)ceilf(sp->u_caster->GetAP() * 0.036f);
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}*/

SPELL_EFFECT_OVERRIDE_RETURNS HandleExplosiveShot(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->u_caster != NULL ) 
	{
		//218,"$damage=${$RAP*0.273+$M1}"
		value += float2int32( sp->u_caster->GetRAP() * 0.274f );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleBlackArrow(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->u_caster != NULL ) 
	{
		//..and dealing [RAP * 0.1 + 785] Shadow damage over 15 sec.
//		value = value * 130 / 100; //in patch 4.3 this got boosted by 40% ?
//		value += sp->u_caster->GetRAP() * 14 / 100 / sp->GetProto()->quick_tickcount;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
/*
SPELL_EFFECT_OVERRIDE_RETURNS HandleRevenge(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->u_caster != NULL )
	{
		//Instantly counterattack an enemy for ${$m1+$AP*0.2} to ${$M1+$AP*0.2} damage.   Revenge is only usable after the warrior blocks, dodges or parries an attack.
		value += sp->u_caster->GetAP() * 20 / 100;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
/*
SPELL_EFFECT_OVERRIDE_RETURNS HandleMangooseBite(Spell *sp,Unit *target, int32 &value, int32 i)
{
	// ${$AP*0.2+$m1} damage.
	if( sp->u_caster != NULL )
		value+=sp->u_caster->GetAP()/5;
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}*/

SPELL_EFFECT_OVERRIDE_RETURNS HandleBloodFury(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( target ) // 20572 - orc racial Blood Fury -> each class has it's own so do not namhash this 
	{
		value = target->GetUInt32Value( UNIT_FIELD_LEVEL ) * 4 + 2;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

/*
SPELL_EFFECT_OVERRIDE_RETURNS HandleJudgeSealOfVengeanceSealOfCorruption(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->p_caster && target ) // Seal of vengeance: Judgement + Seal of Corruption: Judgement
	{
		value += float2int32(1 + 0.22f * sp->u_caster->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SCHOOL_HOLY) + 0.14f * sp->u_caster->GetAP());
		Aura *p = sp->u_caster->HasAura( sp->m_spellInfo->Id );
		if( p && p->m_visualSlot < MAX_VISIBLE_AURAS )
		{
			uint32 c = target->m_auraStackCount[ p->m_visualSlot ];
			value += float2int32(value * 1.0f + (c / 100.0f));
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}


SPELL_EFFECT_OVERRIDE_RETURNS HandleSealOfRightous(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->p_caster && i==0 ) // Seal of Righteousness: ..granting each melee attack ${$MWS*(0.022*$AP+0.044*$SPH)} additional Holy damage.  Only one Seal can be active on the Paladin at any one time.
	{
		Item *it;
		float MVS = 0;
		if(sp->p_caster->GetItemInterface())
		{
			it = sp->p_caster->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_MAINHAND);
			if(it)
				MVS = ( it->GetProto()->Delay + 75 ) / 1000.0f; //yeah that 75 is really strange but at this time it produces blizzlike value :S
		}
		else 
			MVS = sp->p_caster->GetUInt32Value( UNIT_FIELD_BASEATTACKTIME ) / 1000.0f;
		value = (int32)( MVS * float2int32( 0.044f * sp->u_caster->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SCHOOL_HOLY) + 0.022f * sp->u_caster->GetAP()) );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}*/

/*
SPELL_EFFECT_OVERRIDE_RETURNS HandleHammerOfRightous(Spell *sp,Unit *target, int32 &value, int32 i)
{
/*	if( sp->p_caster ) // Seal of Righteousness: ..granting each melee attack ${$MWS*(0.022*$AP+0.044*$SPH)} additional Holy damage.  Only one Seal can be active on the Paladin at any one time.
	{
		Item *it;
		if(sp->p_caster->GetItemInterface())
		{
			it = sp->p_caster->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_MAINHAND);
			if(it)
			{
				uint32 dmg = float2int32( (it->GetProto()->Damage[0].Min + it->GetProto()->Damage[0].Max)/2.0f );
				uint32 dps = dmg * 1000 / it->GetProto()->Delay; 
				value = dps * 4;
			}
		}
	}/**/
/*	if( sp->u_caster )
	{
		float dmg = (sp->u_caster->GetFloatValue(UNIT_FIELD_MINDAMAGE) + sp->u_caster->GetFloatValue(UNIT_FIELD_MAXDAMAGE))/2.0f;
		float dps = dmg * 1000 / sp->u_caster->GetUInt32Value(UNIT_FIELD_BASEATTACKTIME_1);
		value = float2int32( dps * 4.0f );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
/*
SPELL_EFFECT_OVERRIDE_RETURNS HandleShieldOfRightous(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->p_caster && i==0 ) // Shield of Righteousness
	{
		value += sp->p_caster->GetUInt32Value( PLAYER_SHIELD_BLOCK );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
*/
SPELL_EFFECT_OVERRIDE_RETURNS HandleExorcism(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[ i ].Effect == SPELL_EFFECT_SCHOOL_DAMAGE && sp->p_caster && target ) 
	{
		// Causes ${(($m1+$M1)/2)+(0.344*$cond($gt($SP,$AP),$SP,$AP))} Holy damage $?s54934[plus ${($m1+$M1)/2*0.0688} over $d ][]to an enemy target.  If the target is Undead or Demon, it will always critically hit.
		// Causes (2591 + 2891 ) / 2 + (0.344 * cond($gt($SP, AP, $SP, AP)))) Holy damage to an enemy target.  If the target is Undead or Demon, it will always critically hit.
		uint32 SPH = sp->u_caster->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SCHOOL_HOLY);
		SPH = SPH * sp->p_caster->GetDamageDonePctMod( SCHOOL_HOLY );
		uint32 AP = sp->u_caster->GetAP();
		uint32 dominant_power = MAX( SPH, AP );
		value = value + float2int32( dominant_power * 0.344f );
		sp->forced_basepoints[0] = value;	//ah crap !!!hackfix to be able to pass value to second effect !
	}
	else if( sp->GetProto()->eff[ i ].EffectApplyAuraName == SPELL_AURA_PERIODIC_DAMAGE )
	{
		//plus ${($m1+$M1)/2*0.0688} over $d 
		value = float2int32( value / 2 * 0.0688f / sp->GetProto()->quick_tickcount );	//this needs scripting from a talent. Disabling it until then
		if( sp->p_caster && sp->p_caster->HasGlyphWithID( GLYPH_PALADIN_EXORCISM ) )
			value += sp->forced_basepoints[0] * 20 / 100 / sp->GetProto()->quick_tickcount;
		else 
			sp->redirected_effect[i] = SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_ALL_TARGETS;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleShatteringThrow(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( target )
	{
		if( sp->GetProto()->eff[ i ].Effect == SPELL_EFFECT_SCHOOL_DAMAGE )
		{
			//...causing ${$64382m1+$AP*.50} damage ...
//			value += sp->u_caster->GetAP() / 2;
//			if( sp->p_caster )
//				value += sp->p_caster->GetNormalizedMeleeDamage();
//			value += sp->u_caster->GetAP();
			value += float2int32( sp->u_caster->GetAP() * 1.14914f );
		}
		else if( sp->GetProto()->eff[ i ].EffectApplyAuraName == SPELL_AURA_MOD_RESISTANCE_PCT )
		{
			//if target has imunities then remove those
			for( uint32 x = 0; x < MAX_POSITIVE_AURAS1(target); x++ )
			{
				if( target->m_auras[x] != NULL &&
					( target->m_auras[x]->GetSpellProto()->MechanicsType == MECHANIC_INVULNERABLE ) )
					{
						target->m_auras[x]->Remove();
						value = 0;	//skipp decreasing armor if we removed imunity
					}
			}
		} 
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

//Heroic Throw
SPELL_EFFECT_OVERRIDE_RETURNS EH_57755(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE && sp->u_caster)
	{
		value += sp->u_caster->GetAP();
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

//Shield Slam
SPELL_EFFECT_OVERRIDE_RETURNS EH_23922(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE && sp->p_caster != NULL )
	{
/*		Item *it = sp->p_caster->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_OFFHAND );
		if( it && it->GetProto()->InventoryType == INVTYPE_SHIELD )
		{
			uint32 blockable_damage = sp->p_caster->GetUInt32Value( PLAYER_SHIELD_BLOCK );
			if( blockable_damage > 10000 )
				blockable_damage = 10000;
			value += blockable_damage;
		} */
		value += float2int32( sp->u_caster->GetAP() * 0.9425f );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
/*
SPELL_EFFECT_OVERRIDE_RETURNS HandleGouge(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 ) //gouge
	{
		if( sp->u_caster != NULL )
			value += (uint32)ceilf(sp->u_caster->GetAP() * 0.21f);
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
*/
/*
SPELL_EFFECT_OVERRIDE_RETURNS HandleDeathStrike(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->u_caster && target )
	{
		if( i == 1 ) 
		{
			//try to find the target we casted the spell on
			//target is attack target now, heal will target the caster
			uint32 disease_cnt = target->GetDKDiseaseCount();
//			value = sp->u_caster->GetUInt32Value(UNIT_FIELD_BASE_HEALTH) * disease_cnt * 5 / 100; //pct heal of the disease dmg. This is quite some value
			sp->forced_basepoints[2] = sp->u_caster->GetUInt32Value(UNIT_FIELD_MAXHEALTH) * disease_cnt * 5 / 100; //pct heal of the disease dmg. This is quite some value
		}
		else if( i == 2 )
		{
			value = sp->forced_basepoints[2];
			if( (int32)sp->u_caster->GetUInt32Value(UNIT_FIELD_MAXHEALTH) < value * 5 )
				value = 0;
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
} */

SPELL_EFFECT_OVERRIDE_RETURNS HandleBloodBoil(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( target )
	{
		//...base damage is 275-315 (a flat increase of 95) Attack power coefficient is 0.095 (an additional 0.035 * AP).
		uint32 disease_cnt = target->GetDKDiseaseCount();
		disease_cnt = MIN( disease_cnt, 3);	//no more bugget values this gets limited here
		if( disease_cnt )
			value += 35 * disease_cnt;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleBloodStrikeObliterate(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( target )
	{
		// Instantly strike the enemy, causing $s2% weapon damage plus ${$m1*$m2/100}, total damage increased by ${$m3/2}.1% for each of your diseases on the target.
		if( i == 1 ) //count diseases on target
		{
			//try to find the target we casted the spell on
			uint32 disease_cnt = target->GetDKDiseaseCount();
			//we already calced add_dmg in effect0 but we are overwriting that
//			add_damage = ( sp->GetProto()->eff[0].EffectBasePoints+1) * ( sp->GetProto()->eff[1].EffectBasePoints+1) / 100 * disease_cnt *( sp->GetProto()->eff[2].EffectBasePoints+1)/200;
			sp->add_damage = sp->add_damage * value / 100 + sp->add_damage * disease_cnt * ( sp->GetProto()->eff[2].EffectBasePoints+1)/200;
			//pct dmg is increased too
			value = value + disease_cnt*(sp->GetProto()->eff[2].EffectBasePoints+1)/2;
//			if( sp->GetProto()->NameHash == SPELL_HASH_OBLITERATE )
//			{
				//talent that removes the effect of removing diseases
//				if( sp->p_caster && sp->p_caster->HasAuraWithNameHash( SPELL_HASH_ANNIHILATION, 0, AURA_SEARCH_PASSIVE ) == false )
//				{
//					target->RemoveAuraByNameHash( SPELL_HASH_BLOOD_PLAGUE );
//					target->RemoveAuraByNameHash( SPELL_HASH_FROST_FEVER );
//				}
//			}
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleHearthStrike(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( target )
	{
		// causing $s2% weapon damage plus ${$m1*$m2/100}, total damage increased by $m3% for each of your diseases on the target.
		if( i == 1 ) //count diseases on target
		{
			//try to find the target we casted the spell on
			uint32 disease_cnt = target->GetDKDiseaseCount();
			//simpleformula that is not in sinc with client opinion :(
			int32 bonus_pct_dmg = (sp->GetProto()->eff[2].EffectBasePoints + 1)*disease_cnt ;
			if( sp->u_caster != NULL && sp->u_caster->SM_Mods )
				SM_PIValue(sp->u_caster->SM_Mods->SM_PCost,&bonus_pct_dmg,sp->GetProto()->GetSpellGroupType());
			sp->add_damage = sp->add_damage + sp->add_damage * bonus_pct_dmg / 100;
			value += bonus_pct_dmg;
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
/*
SPELL_EFFECT_OVERRIDE_RETURNS HandleScourgeStrike(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( target )
	{
		//deals $s2% of weapon damage as Shadow damage plus ${$m1*$m2*1.25/100}, total damage increased $s3% per each of your diseases on the target.
		if( i == 1 ) //count diseases on target
		{
			//we already calced add_dmg in effect0 but we are overwriting that
			sp->add_damage = sp->add_damage*125/100;
			//try to find the target we casted the spell on
			uint32 disease_cnt = target->GetDKDiseaseCount();
			//simpleformula that is not in sinc with client opinion :(
			int32 bonus_pct_dmg = (sp->GetProto()->eff[2].EffectBasePoints + 1)*disease_cnt ;
			if( sp->u_caster != NULL && sp->u_caster->SM_Mods )
				SM_PIValue(sp->u_caster->SM_Mods->SM_PCost,&bonus_pct_dmg,sp->GetProto()->GetSpellGroupType());
			sp->add_damage = sp->add_damage * (bonus_pct_dmg + 100 )/ 100;
			value += bonus_pct_dmg;
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}*/

SPELL_EFFECT_OVERRIDE_RETURNS HandleBloodCakedStrike(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( target )
	{
		// hits for $50463s1% weapon damage plus ${$50463m1/2}.1% for each of your diseases on the target.
		if(  sp->GetProto()->eff[i].Effect == SPELL_EFFECT_WEAPON_PERCENT_DAMAGE ) //count diseases on target
		{
			//try to find the target we casted the spell on
			uint32 disease_cnt = target->GetDKDiseaseCount();
			disease_cnt = MIN( disease_cnt, 3 );
			//simpleformula that is not in sinc with client opinion :(
			int32 bonus_add_dmg = 12 * disease_cnt;
//			if( sp->u_caster != NULL && sp->u_caster->SM_Mods )
//				SM_PIValue(sp->u_caster->SM_Mods->SM_PCost,&bonus_add_dmg,sp->GetProto()->GetSpellGroupType());
//			value += bonus_add_dmg;
			value = 30 + bonus_add_dmg;	
//			sp->forced_pct_mod[ i ] += bonus_add_dmg;
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

/*
SPELL_EFFECT_OVERRIDE_RETURNS HandlePlagueStrike(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i==0 && target )
	{
		//remove 1 heal over time effect from target
		for(uint32 t=0;t<MAX_POSITIVE_AURAS1(target);t++)
			if( target->m_auras[ t ] && target->m_auras[ t ]->GetSpellProto()->c_is_flags & SPELL_FLAG_IS_HEAL_OVER_TIME )
			{
				target->m_auras[ t ]->Remove();
				break;
			}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
} */
/*
SPELL_EFFECT_OVERRIDE_RETURNS HandleRuneStrike(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 1 && sp->u_caster)
	{
		//Strike the target for $s2% weapon damage plus ${$m2*$AP*$m3/10000}.  Only usable after an attack is dodged or parried.  Can't be dodged, blocked, or parried.
		sp->add_damage = value * sp->u_caster->GetAP() * (sp->GetProto()->eff[2].EffectBasePoints + 1) / 10000;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}*/

SPELL_EFFECT_OVERRIDE_RETURNS HandleIceLance(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( target && sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE )
	{
		//frozen is : Deep Freeze, Frost Nova, Improved Cone of Cold Proc, Water Elemental Freeze, Fingers of Frost Proc and Shattered Barrier Proc
		//chill is : frostbolt and cone of cold
/*		bool target_is_frozen = false;	
		for(uint32 t=NEG_AURAS_START;t<MAX_NEGATIVE_AURAS1(target);t++)
			if( target->m_auras[ t ] && target->m_auras[ t ]->GetSpellProto()->c_is_flags & SPELL_FLAG_IS_CHILL_SPELL )
			{
				has_chill_effect = true;
				break;
			} */

		bool target_is_frozen = target->HasAuraStateFlag( AURASTATE_FROZEN );
		if( target_is_frozen == false && sp->p_caster )
			target_is_frozen = ( sp->p_caster->m_ForceTargetFrozen > 0 );

		if( target_is_frozen ) //Ice Lance deals 3x damage if target is frozen
			sp->forced_pct_mod_cur += 100;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleChaosBane(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->m_targetUnits[i].size() )
	{
		//Deals $s1 Shadow damage, split between all enemy targets within $a1 yards of the impact crater.
		value = value / (int32)(sp->m_targetUnits[i].size());	//if this is 0 then this function would never get called
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleTympanicTantum(Spell *sp,Unit *target, int32 &value, int32 i)
{
	//Deals $s1% damage every $t1 sec for $d. Nearby enemies are also dazed for the duration.
	if( target && i == 0)
	{
		value = target->GetUInt32Value( UNIT_FIELD_BASE_HEALTH ) * (sp->GetProto()->eff[i].EffectBasePoints + 1) / 100;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleConflagrate(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( target )
	{
		//$?s56235[Causes][Consumes] an Immolate or Shadowflame effect on the enemy target to instantly deal damage equal to 60% of your Immolate or Shadowflame, and causes an additional $s3% damage over $d.
		if( i == 0 )
		{
			Aura *ta = target->HasAuraWithNameHash( SPELL_HASH_IMMOLATE, 0, AURA_SEARCH_NEGATIVE );
			if( ta == NULL || ta->m_modcount == 0 )
				ta = target->HasAuraWithNameHash( SPELL_HASH_SHADOWFLAME, 0, AURA_SEARCH_NEGATIVE );

			if( ta != NULL && ta->m_modcount != 0 )
			{
				//immolate total dmg
				uint32 TickCount;
//				uint32 SpellDmgBonus;
				if( sp->u_caster )
				{
					uint32 AuraDuration = ta->GetDuration();
					uint32 Amplitude = GetSpellAmplitude( ta->GetSpellProto(), sp->u_caster, 2, sp->i_caster!=NULL );
					TickCount = AuraDuration / Amplitude;
				}
				else
					TickCount = ta->GetSpellProto()->quick_tickcount;

//				if( ta->m_DamageDoneLastTick )
//					sp->forced_basepoints[0] = ta->m_DamageDoneLastTick * TickCount;
//				else
				{
					dealdamage tdmg;
					tdmg.base_dmg = ta->m_modList[0].m_amount;
					tdmg.pct_mod_final_dmg = ta->m_modList[0].m_pct_mod;
					tdmg.DisableProc = true;
					tdmg.StaticDamage = false;
					if( sp->u_caster )
						sp->u_caster->SpellNonMeleeDamageCalc( target, ta->GetSpellProto(), &tdmg, ta->GetSpellProto()->quick_tickcount, ta->m_modList[0].i );
					//remove critical. Conflagrate can crit too
					if( tdmg.pct_mod_crit )
						tdmg.full_damage = tdmg.full_damage * 100 / tdmg.pct_mod_crit;
					int32 ress = tdmg.full_damage - tdmg.resisted_damage - tdmg.absorbed_damage;
					if( ress < 0 )
						ress = 0;
					sp->forced_basepoints[0] = ress * TickCount;
				}
//				sp->forced_basepoints[0] *= 2;	//for the sake of DD part
				//only 60% dmg is converted to instant dmg
				value = sp->forced_basepoints[0] * value / 100;
				ta = NULL;					//sanity
			}
			else sp->forced_basepoints[0] = 0;
		}
		else if( i == 1 )
		{
			//only 40% dmg is converted to DOTs
			value = sp->forced_basepoints[0] * value / 100 / sp->GetProto()->quick_tickcount;
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleHandOfReckoning(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && target )
	{
		if( target->IsPlayer() )
			value = 0;
//		else if( sp->u_caster && target->IsCreature() && target->GetAIInterface()->GetNextTarget() != sp->m_caster )
		else if( sp->u_caster )
			value = sp->u_caster->GetAP() * 50 / 100;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleAvangersShield(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && sp->u_caster )
	{
		value += ( sp->u_caster->GetAP() +  sp->u_caster->GetDamageDoneMod( SCHOOL_HOLY ) ) * 7 / 100 + 1;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleEnvenom(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && sp->p_caster )//Envenom
	{
/*
Finishing move that consumes your Deadly Poison and deals instant poison damage.  Following the Envenom attack your Deadly Poison application chance is increased by 15%, and your Instant Poison application frequency by 75%, for 1 sec plus an additional 1 sec per combo point.  Poison doses, up to the number of combo points spent, are consumed to increase Envenom's damage:
  1 point  : (AP * 0.09 * 1 + (240 * 1)) damage
  2 points: Up to (AP * 0.18 * 1 + (240 * 2)) damage
  3 points: Up to (AP * 0.27 * 1 + (240 * 3)) damage
  4 points: Up to (AP * 0.36 * 1 + (240 * 4)) damage
  5 points: Up to (AP * 0.45 * 1 + (240 * 5)) damage
  */
		uint32 SafeComboPointCount = MIN( sp->p_caster->m_comboPoints, 5 );
		value = (uint32)( ( sp->p_caster->GetAP() * 0.09f + value ) * SafeComboPointCount );
		sp->m_requiresCP++;
		if( target && sp->p_caster->HasAuraWithNameHash( SPELL_HASH_MASTER_POISONER, 0, AURA_SEARCH_PASSIVE ) == false )
			target->RemoveAuraByNameHash( SPELL_HASH_DEADLY_POISON, 0, AURA_SEARCH_NEGATIVE, MAX_PASSIVE_AURAS );
//		sp->p_caster->NullComboPoints();	//just in case spell is missing combo cost
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleEvocationGlyph(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MOD_TOTAL_HEALTH_REGEN_PCT )
	{
		if( sp->p_caster && sp->p_caster->HasGlyphWithID( GLYPH_MAGE_EVOCATION) )
		{
			if( sp->GetProto()->c_is_flags2 & SPELL_FLAG2_IS_VALUE_OVER_TIME )
				value = dbcSpell.LookupEntryForced( 56380 )->eff[0].EffectBasePoints * ( 1 + sp->GetProto()->quick_tickcount );	//also restore X% of max HP over Y seconds
			else
				value = dbcSpell.LookupEntryForced( 56380 )->eff[0].EffectBasePoints;	//also restore X% of max HP over Y seconds
		}
		else 
			sp->redirected_effect[i] = SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_ALL_TARGETS;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleAspectMastery(Spell *sp,Unit *caster,Unit *target, int32 &value, int32 i, SpellEffectOverrideScript *so)
{
	//Aspect of the Viper - Reduces the damage penalty by $s1%.
/*	if( sp->GetProto()->NameHash == SPELL_HASH_ASPECT_OF_THE_VIPER && i == 1 )
	{
		value = value * so->mod_amount_pct[0] / 100;	
	}
	//Aspect of the Monkey - Reduces the damage done to you while active by $s2%.
	else  if( sp->GetProto()->NameHash == SPELL_HASH_ASPECT_OF_THE_MONKEY && i == 1 )
	{
		value += so->mod_amount[1];	//value was - but need + ?
	}
	//Aspect of the Hawk - Increases the attack power bonus by $s3%.
	else  */
	if( sp->GetProto()->NameHash == SPELL_HASH_ASPECT_OF_THE_HAWK && i == 0 )
	{
		value = value * so->mod_amount_pct[2] / 100;
	}
	//Aspect of the Dragonhawk - Combines the bonuses from Aspect of the Monkey and Hawk.
//	else if( sp->GetProto()->NameHash == SPELL_HASH_ASPECT_OF_THE_DRAGONHAWK && i == 1 )
	else if( (sp->GetProto()->Id == 61846 || sp->GetProto()->Id == 61847) && i == 0 )	//the additional cast mod only
			value = value * so->mod_amount_pct[2] / 100;
	else if( sp->GetProto()->Id == 61848 && i == 1 )	//the additional cast mod only
			value += so->mod_amount[1];	
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleCounterAttack(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->u_caster )
		value += sp->u_caster->GetAP() * 20 / 100;	//also restore 60% of max HP over X seconds
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS HandleImprovedDemonicTactics(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->u_caster )
	{
		int32 base_critchance_now = 0;
		for( uint32 t=0;t<SCHOOL_COUNT;t++)
			base_critchance_now = MAX( base_critchance_now, float2int32( sp->u_caster->SpellCritChanceSchool[t] ));
		base_critchance_now	+= float2int32( sp->u_caster->spellcritperc );

		value = base_critchance_now * value / 100;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

//Sacrifice
SPELL_EFFECT_OVERRIDE_RETURNS EH_47986(Spell *sp,Unit *target, int32 &value, int32 i)
{
	//25% of voidvalker HP
	if( i == 1 && target )
		value = target->GetMaxHealth() * value / 100 ;
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

//Sonic Screech
SPELL_EFFECT_OVERRIDE_RETURNS EH_64422(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 &&  sp->m_targetUnits[i].size() > 1 )
		value = value / (int32)(sp->m_targetUnits[i].size());	//should already target mobs infront
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

//Pulsing Shockwave
SPELL_EFFECT_OVERRIDE_RETURNS EH_52942(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 &&  target && sp->u_caster )
	{
		float _distance = sp->u_caster->CalcDistance( target );
		if( _distance >= 2.0f )
			value = float2int32(value * _distance);
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

//Incinerate
SPELL_EFFECT_OVERRIDE_RETURNS EH_29722(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 &&  target && target->HasAuraStateFlag( AURASTATE_IMMOLATE ) )
	{
//		value += value / 6;
		sp->forced_pct_mod_cur += 100/6;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

//gore
SPELL_EFFECT_OVERRIDE_RETURNS EH_35290(Spell *sp,Unit *target, int32 &value, int32 i)
{
	//this check will probably always fail :(
	if( i == 0 &&  sp->p_caster && sp->p_caster->Cooldown_CanCast( sp->GetProto() ) == false )
		value = value * 2;
	else if( RandChance( 50 ) )
		value = value * 2;
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

//Bloodthirst
SPELL_EFFECT_OVERRIDE_RETURNS EH_23881(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && sp->u_caster)
		value = sp->u_caster->GetAP()*value/100;
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

//Intercept
SPELL_EFFECT_OVERRIDE_RETURNS EH_20253(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 1 && sp->u_caster)
		value = sp->u_caster->GetAP() * 12 / 100;
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

//Thunder Clap
SPELL_EFFECT_OVERRIDE_RETURNS EH_6343(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && sp->u_caster)
		value = sp->u_caster->GetAP() * 20 / 100;
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_19891(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[ i ].EffectApplyAuraName == SPELL_AURA_MOD_RESISTANCE_EXCLUSIVE && sp->u_caster)
	{
		uint32 lvl = sp->u_caster->getLevel();
		if( lvl < 70 )
			value = lvl;
		else if( lvl < 80 )
			value = lvl + ( lvl - 70 ) * 5 ;
		else 
			value = lvl + ( lvl - 70 ) * 5 + ( lvl - 80 ) * 7;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

/*
SPELL_EFFECT_OVERRIDE_RETURNS HandleInnervate(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( target )
	{
		value = value * target->GetUInt32Value( UNIT_FIELD_BASE_MANA ) / 100 / sp->GetProto()->quick_tickcount;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}*/

SPELL_EFFECT_OVERRIDE_RETURNS EH_77799(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( target && i == 0 && sp->u_caster )
	{
		Aura *a;
		a = target->HasAuraWithNameHash( SPELL_HASH_IMMOLATE, sp->u_caster->GetGUID(), AURA_SEARCH_NEGATIVE );
		if( a )
		{
			uint32 NewDuration = MIN( 15000, a->GetTimeLeft() + 6000 );
			a->SetDuration( NewDuration );
			a->ResetDuration();
		}
//		a = target->HasAuraWithNameHash( SPELL_HASH_UNSTABLE_AFFLICTION, sp->u_caster->GetGUID(), AURA_SEARCH_NEGATIVE ); 
		a = target->HasAura( 30108, sp->u_caster->GetGUID(), AURA_SEARCH_NEGATIVE ); //cause there is a unstable affliction silence with same name
		if( a )
		{
			uint32 NewDuration = MIN( 15000, a->GetTimeLeft() + 6000 );
			a->SetDuration( NewDuration );
			a->ResetDuration();
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_48020(Spell *sp,Unit *target, int32 &value, int32 i)
{
	//maybe i should have done this with proc spells :(
	if( i == 0 && sp->u_caster && sp->u_caster->HasAuraWithNameHash( SPELL_HASH_SOULBURN, 0, AURA_SEARCH_POSITIVE ) )
		sp->u_caster->CastSpell( sp->u_caster, 79438, true );
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_20167(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->p_caster )
	{
		if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_HEAL )
		{
			//heal the Paladin for 0.15 * AP.15 * holy power and restore $20167s2% of the paladin's base mana.  
			value = ( sp->p_caster->GetAP() + sp->p_caster->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SCHOOL_HOLY) ) * 15 / 100;
//			value = float2int32(( (sp->p_caster->GetAP() + 0.15f) * sp->p_caster->GetPower( POWER_TYPE_HOLY) ) * 0.15f);
//			sp->p_caster->SetAuraStateFlag( AURASTATE_JUDGEMENT, true );
		}
		else if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_ENERGIZE )
		{
			value = sp->p_caster->GetUInt32Value( UNIT_FIELD_BASE_MANA ) * value / 100;
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_31803(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->p_caster ) // Holy Vengeance/ Blood Corruption
	{
		// which deals ${(0.01*$SPH+0.0270*$AP)*5*$<sop>/100} additional Holy damage over $31803d.  
		// $sop1=$?s20224[${105}][${100}]
		// $sop2=$?s20225[${110}][${$<sop1>}]
		// $sop=$?s20330[${115}][${$<sop2>}]
		//deals ((0.01 * holy power.0193 * AP) * 5 * 100 / 100) additional Holy damage over 15 
//		value = float2int32( sp->p_caster->GetAP() * ( sp->p_caster->GetPower( POWER_TYPE_HOLY ) + 0.0193f) * 0.05f);
//		value = float2int32( ( sp->p_caster->GetAP() * 0.0270 + sp->p_caster->GetDamageDoneMod( SCHOOL_HOLY ) * 0.01f ) * 5.0f);
//		value = float2int32( sp->p_caster->GetAP() * 0.0270f * 5.0f ); //we will add 
//		value /= sp->GetProto()->quick_tickcount;
		value = MAX( 1, value );
	}
/*	if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_DUMMY && target )
	{
		// do we already have a DOT aura ? If so we mod that and only apply a dummy
		Aura *a = target->HasAuraWithNameHash( sp->GetProto()->NameHash, 0, AURA_SEARCH_NEGATIVE );
		if( a && a->m_modcount > 0 && a->m_modList[ 0 ].m_AuraName == SPELL_AURA_PERIODIC_DAMAGE )
		{
			a->m_modList[ 0 ].m_amount = MAX( a->m_modList[ 0 ].m_amount, value );
			int32 Count = target->CountNegAuraWithNamehash( sp->GetProto()->NameHash );
			if( Count < sp->GetProto()->maxstack )
				a->m_modList[ 0 ].m_pct_mod += 100;
			//target has already dot ? then we will simply add a dummy aura to not spam combat log
			sp->redirected_effect[1] = SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_ALL_TARGETS;
		}
		//if dot is not registered we will use DOT aura only
		else
			sp->redirected_effect[i] = SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_ALL_TARGETS;
	} */
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_20170(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE && sp->u_caster )
	{
		//deal ${$MWS*(0.005*$AP+0.01*$SPH)*$<sop>/100} additional Holy damage. 
		uint32 MWS = 2000;
		if( sp->p_caster && sp->p_caster->GetItemInterface() )
		{
			Item *it = sp->p_caster->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_MAINHAND );
			if(it)
				MWS = it->GetProto()->Delay;
		}
		uint32 AP = sp->u_caster->GetAP();
		uint32 HOLYP = sp->u_caster->GetPower( POWER_TYPE_HOLY );
		uint32 SPH = sp->u_caster->GetDamageDoneMod( SCHOOL_HOLY );
		value = float2int32( MWS * ( 0.005f * AP + 0.01f * SPH ) * HOLYP / 1000.0f );
//		value = float2int32( MWS * ( 0.005f * (AP+0.01f) * HOLYP ) / 1000.0f );
	}
//	if( i == 0 )
//		sp->u_caster->SetAuraStateFlag( AURASTATE_JUDGEMENT, true );
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
/*
SPELL_EFFECT_OVERRIDE_RETURNS EH_20154(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 )
	{
		sp->u_caster->SetAuraStateFlag( AURASTATE_JUDGEMENT, true );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_31801(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 )
	{
		sp->u_caster->SetAuraStateFlag( AURASTATE_JUDGEMENT );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}*/

SPELL_EFFECT_OVERRIDE_RETURNS EH_77767(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 )
	{
		if( sp->u_caster ) 
		{
			//Deals weapon damage plus (276 + (RAP * 0.017)) in the form of Nature damage and increases the duration of your Serpent Sting on the target by 6 sec.
			uint32 RAP = sp->u_caster->GetRAP();
			value += float2int32(RAP * 0.017f);
			if( target )
			{
				Aura *a;
				a = target->HasAuraWithNameHash( SPELL_HASH_SERPENT_STING, sp->u_caster->GetGUID(), AURA_SEARCH_NEGATIVE );
				if( a )
				{
					uint32 NewDuration = MIN( 15000, a->GetTimeLeft() + 6000 );
					a->SetDuration( NewDuration );
					a->ResetDuration();
				}
			}
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_56641(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 )
	{
		if( sp->u_caster ) 
		{
			//A steady shot that causes 100% weapon damage plus RAP*0.021+280. Generates 9 Focus.
			uint32 RAP = sp->u_caster->GetRAP();
			value += float2int32(RAP * 0.021f);
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_53209(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_HEAL && sp->u_caster )
	{
		value = sp->u_caster->GetUInt32Value( UNIT_FIELD_MAXHEALTH ) * 5 / 100;
	}
	else if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_WEAPON_DAMAGE )
	{
		if( sp->u_caster ) 
		{
			//An instant shot that causes ranged weapon damage plus RAP*0.488+874, refreshing the duration of  your Serpent Sting and healing you for 5% of your total health.
			//217,"$damage=${$RAP*0.732+$m2}",
			uint32 RAP = sp->u_caster->GetRAP();
			value += float2int32(RAP * 0.732f);
			if( target )
			{
				Aura *a;
				a = target->HasAuraWithNameHash( SPELL_HASH_SERPENT_STING, sp->u_caster->GetGUID(), AURA_SEARCH_NEGATIVE );
				if( a )
				{
//					a->SetDuration( a->GetDuration() + 6000 );
					a->ResetDuration();
				}
			}
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_20043(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && sp->u_caster)
	{
		uint32 lvl = sp->u_caster->getLevel();
		if( lvl < 70 )
			value = lvl;
		else if( lvl < 80 )
			value = lvl + ( lvl - 70 ) * 5 ;
		else 
			value = lvl + ( lvl - 70 ) * 5 + ( lvl - 80 ) * 7;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_845(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && sp->u_caster)
	{
		value += float2int32(sp->u_caster->GetAP() * 0.562f);
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_36946(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && sp->u_caster)
	{
		//Consecrates the land beneath the Paladin, doing (8 * (0 + 0.04 * holy power.04 * AP)) Holy damage over 10 sec to enemies who enter the area.
		//0 damage every 5 seconds.
//		uint32 value_total = 8 * ( value + float2int32( ( (sp->u_caster->GetDamageDoneMod( SCHOOL_HOLY ) * 0.04f * 0.04f * sp->u_caster->GetAP() ) ) ));
		uint32 HP = sp->u_caster->GetPower( POWER_TYPE_HOLY );
		uint32 value_total = 8 * ( value + float2int32( ( ((HP + 0.04f) * 0.04f * sp->u_caster->GetAP() ) ) ));
//		value_total = value_total / 5;	//somehow value is not the same as on the pannel
		value = value_total / 20;	//1 tick at every 0.5 sec over 10 seconds
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_32379(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE && target )
	{
		if( (int32)target->GetHealth() > value && sp->u_caster )
		{
//			sp->u_caster->SpellNonMeleeDamageLog( sp->u_caster, sp->GetProto(), value, true, false, false, 1, 0);
			//damage is supposed to be delayed. Don't ask why this is important for some to make a report about it
//			sp->u_caster->CastSpell( sp->u_caster, 32409, true );
			int32 caster_dmg = value;
			int32 pct_mod = 100;
			if( sp->p_caster )
			{
				Aura *a = sp->p_caster->HasAuraWithNameHash( SPELL_HASH_ITEM___PRIEST_T13_SHADOW_2P_BONUS__SHADOW_WORD__DEATH_, 0, AURA_SEARCH_PASSIVE );
				if( a )
				{
					pct_mod -= a->GetSpellProto()->eff[1].EffectBasePoints;
					if( pct_mod < 0 )
						pct_mod = 0;
				}
			}
			sEventMgr.AddEvent( (Object*)sp->u_caster, &Object::EventSpellDamage, sp->u_caster->GetGUID(), sp->GetProto(), (uint32)caster_dmg, (int32)pct_mod, (int32)i, EVENT_SPELL_DAMAGE_HIT, uint32(1500), 1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
			//damn, we should make this happen only every 6 seconds
			if( sp->p_caster && target->GetHealthPct() <= 25 && sp->p_caster->HasGlyphWithID( GLYPH_PRIEST_SHADOW_WORD_DEATH ) )
			{
				if( sp->p_caster->Cooldown_CanCast( dbcSpell.LookupEntryForced( 55682 ) ) )
				{
					sp->p_caster->ClearCooldownForSpell( 32379 ); //shadow word death
					sp->p_caster->_Cooldown_Add( COOLDOWN_TYPE_SPELL, 55682, getMSTime() + 6000, 55682, 0 ); //this is the glyph spell, we use it to be able to count the seconds
//					sp->p_caster->Cooldown_Add( dbcSpell.LookupEntryForced( 55682 ), NULL );	//this is the glyph spell, we use it to be able to count the seconds
				}
			}
		}
		if( target->GetHealthPct() < 25 )
		{
			sp->forced_pct_mod_cur += 200;	//extra 2 times dmg ( total 3 times dmg )
			Aura *a = sp->u_caster->HasAuraWithNameHash( SPELL_HASH_MIND_MELT, 0, AURA_SEARCH_PASSIVE );
			if( a )
				sp->forced_pct_mod_cur += a->GetSpellProto()->eff[0].EffectBasePoints;
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_41637(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 )
	{
		//Places a spell on the target that heals them for (((2971 + ($SP * 0.318)) * 1))
		uint32 max_power = MAX( sp->u_caster->GetDamageDoneMod( SCHOOL_HOLY ),sp->u_caster->GetDamageDoneMod( SCHOOL_SHADOW ) );
		value += max_power * 318 / 1000;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_23455(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 1 )	//th heal
	{
		value = value / MAX(1,(uint32)sp->m_targetUnits[1].size());	//value is divided between targets
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_467(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 )
	{
		value += sp->u_caster->GetDamageDoneMod( SCHOOL_NATURE ) * 168 / 1000;	
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_79060(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[ i ].EffectApplyAuraName == SPELL_AURA_MOD_RESISTANCE_EXCLUSIVE && sp->u_caster)
	{
		uint32 lvl = sp->u_caster->getLevel();
		if( lvl < 70 )
			value = lvl ;
		else if( lvl < 80 )
			value = lvl + ( lvl - 70 ) * 5 ;
		else 
			value = lvl + ( lvl - 70 ) * 5 + ( lvl - 80 ) * 7;
		value = value / 2;	//only for this spell ?
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_3606(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && sp->u_caster )
	{
		Unit *p = sp->u_caster->GetTopOwner();
		if( p && p->HasAuraWithNameHash( SPELL_HASH_SEARING_FLAMES, 0, AURA_SEARCH_PASSIVE ) )
//			sp->u_caster->CastSpell( target, 77661, true );	//also cast the dot
			p->CastSpell( target, 77661, true );	//also cast the dot Searing Flames
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_85673(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && sp->p_caster )
	{
		//Consumes all Holy Power to heal a friendly target for ${(($m1+$M1)/2)+0.198*$AP} $?s93466[plus ${($m1+$M1)/2} over $d ][]per charge of Holy Power.
		int32 counter = sp->u_caster->GetPower( POWER_TYPE_HOLY );
		sp->power_cost = MAX( counter, sp->power_cost );
		sp->p_caster->LastSpellCost = counter;
//		counter = MAX( counter, (uint32)sp->power_cost );
		sp->forced_pct_mod_cur += counter * 100; //we are adding multiplier based on cost + count. Cost is base forced_pct_mod
		sp->u_caster->SetPower(  POWER_TYPE_HOLY, 0 );
		value += float2int32( sp->u_caster->GetAP() * 0.198f );
	}
	//this only has a value if talented in it
	else if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_PERIODIC_HEAL )
	{
		if( sp->p_caster == NULL || sp->p_caster->HasGlyphWithID( GLYPH_PALADIN_LONG_WORD ) == false )
		{
			sp->redirected_effect[i] = 1;
			//will need to implement it later !!!
			value = 0;
		}
		sp->forced_pct_mod_cur += 100; //because glyph will nerf this one also by half, we are readding it
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_53600(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && sp->u_caster )
	{
		uint32 counter = sp->u_caster->GetPower( POWER_TYPE_HOLY );
		counter = MAX( counter, (uint32)sp->power_cost );
		if( counter == 2 )
		{
//			value = value * 3;
			//100% is included in default value we only add the extra
			sp->forced_pct_mod_cur += 200;	//mod the whole dmg ( SP included )
		}
		else if( counter == 3 )
		{
//			value = value * 6;
			//100% is included in default value we only add the extra
			sp->forced_pct_mod_cur += 500;	//mod the whole dmg ( SP included )
		}
	}
	sp->u_caster->SetPower( POWER_TYPE_HOLY, 0 );
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_84963(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && sp->u_caster )
	{
		uint32 counter = sp->u_caster->GetPower( POWER_TYPE_HOLY );
		counter = MAX( counter, (uint32)sp->power_cost );
		if( sp->u_caster->HasAuraWithNameHash( SPELL_HASH_ITEM___PALADIN_T11_RETRIBUTION_4P_BONUS, 0, AURA_SEARCH_PASSIVE ) )
			counter++;
		uint32 duration_mod = counter;
		uint32 new_dutaion = sp->GetDuration() * duration_mod;
		sp->forced_duration = new_dutaion;
		sp->Dur = new_dutaion;
	}
	sp->u_caster->SetPower( POWER_TYPE_HOLY, 0 );
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_85256(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_WEAPON_PERCENT_DAMAGE && sp->u_caster )
	{
/*
"$crusade1=$?s31868[${30*1.3}]?s31867[${30*1.2}]?s31866[${30*1.1}][${30}]
$crusade2=$?s31868[${90*1.3}]?s31867[${90*1.2}]?s31866[${90*1.1}][${90}]
$crusade3=$?s31868[${235*1.3}]?s31867[${235*1.2}]?s31866[${235*1.1}][${235}]
$templar1=$?s63220[${$<crusade1>*1.15}][${$<crusade1>}]
$templar2=$?s63220[${$<crusade2>*1.15}][${$<crusade2>}]
$templar3=$?s63220[${$<crusade3>*1.15}][${$<crusade3>}]",
*/
		uint32 counter = sp->u_caster->GetPower( POWER_TYPE_HOLY );
		counter = MAX( counter, (uint32)sp->power_cost );
		if( counter == 1 )
			value = 30;
		else if( counter == 2 )
			value = 90;
		else if( counter == 3 )
			value = 235;
	}
	sp->u_caster->SetPower( POWER_TYPE_HOLY, 0 );
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_5171(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && sp->p_caster )
	{
		uint32 duration_mod = sp->p_caster->m_comboPoints;
		uint32 new_dutaion = 6000 + 3000 * duration_mod;
		sp->forced_duration = new_dutaion;
		sp->Dur = new_dutaion;
		sp->m_requiresCP = 1;
//		sp->p_caster->NullComboPoints();	//required
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_33878(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_WEAPON_DAMAGE )
	{
		value = value * 95 / 100;	//this is according to wowhead description. 95 is actually the value of effect weapon percent damage that will be calculated in the future
	} 
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_78(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE && sp->u_caster )
	{
//		uint32 basepoints2 = sp->GetProto()->eff[1].EffectBasePoints + 1;
//		uint32 levelpoints = sp->u_caster->getLevel() * sp->GetProto()->eff[1].EffectRealPointsPerLevel;
//		uint32 pct_mod = levelpoints + basepoints2;
//		value = value + sp->u_caster->GetAP() * pct_mod/ 100;	
		value = 8 + sp->u_caster->GetAP() * 75 / 100;	
	} 
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_52174(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE && sp->u_caster )
	{
		value = 1 + sp->u_caster->GetAP() * 50 / 100;	
	} 
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_1822(Spell *sp,Unit *target, int32 &value, int32 i)
{
	//Rake the target for ${$AP*0.147+$m1} Bleed damage and an additional $?s80315[${$m2*5+$AP*0.735}]?s80314[${$m2*4+$AP*0.588}][${$m2*3+$AP*0.441}] Bleed damage over $d.  Awards $s3 combo $lpoint:points;.
	if( sp->u_caster )
	{
		if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE )
		{
			value += float2int32( sp->u_caster->GetAP()* 0.147f );
		}
		else if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_PERIODIC_DAMAGE )
		{
			value = ( value * 3 + float2int32( sp->u_caster->GetAP() * 0.441f ) ) / sp->GetProto()->quick_tickcount;	
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_1079(Spell *sp,Unit *target, int32 &value, int32 i)
{
//Finishing move that causes damage over time.  Damage increases per combo point and by your attack power:
//   1 point: ${($m1+$b1*1+0.0207*$AP)*8} damage over $d.
//   2 points: ${($m1+$b1*2+0.0414*$AP)*8} damage over $d.
//   3 points: ${($m1+$b1*3+0.0621*$AP)*8} damage over $d.
//   4 points: ${($m1+$b1*4+0.0828*$AP)*8} damage over $d.
//   5 points: ${($m1+$b1*5+0.1035*$AP)*8} damage over $d.$?s54815[
	if( sp->p_caster )
	{
		if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_PERIODIC_DAMAGE )
		{
			//${$m2*3+$AP*0.18}
			uint32 point_count = sp->p_caster->m_comboPoints;
			uint32 dolarB = value - sp->damage;
			uint32 dolarM = value - dolarB;
//			value = ( value / 3 + point_count * value + sp->u_caster->GetAP() * point_count / 100 ) / sp->GetProto()->quick_tickcount;	
			value = ( dolarM + point_count * dolarB + sp->u_caster->GetAP() * point_count * 207 / 10000 ) ;	
			//cause blizz is an ass and keeps changing the mechanics of spells
//			if( target && sp->GetProto()->MechanicsType != MECHANIC_BLEEDING )
//			{
//				value += value * target->ModDamageTakenByMechPCT[MECHANIC_BLEEDING] / 100;
//				value += target->ModDamageTakenByMechFlat[MECHANIC_BLEEDING];
//			}
//			sp->p_caster->NullComboPoints();	//just in case spell is missing combo cost
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_33745(Spell *sp,Unit *target, int32 &value, int32 i)
{
	//Lacerates the enemy target, dealing ${$m2+($AP*0.0552)} damage and making them bleed for ${5*($m1+($AP*0.00369))} damage over $d.  Damage increased by attack power.  This effect stacks up to $u times on the same target.
	if( sp->GetProto()->eff[ i ].EffectApplyAuraName == SPELL_AURA_PERIODIC_DAMAGE && sp->u_caster)
	{
//		value = float2int32(5 * (23 + (sp->u_caster->GetAP() * 0.0077f)));
		value = float2int32(5 * (value + sp->u_caster->GetAP() * 0.00369f));
		//cause blizz is an ass and keeps changing the mechanics of spells
		if( target && sp->GetProto()->MechanicsType != MECHANIC_BLEEDING )
		{
			value += value * target->ModDamageTakenByMechPCT[MECHANIC_BLEEDING] / 100;
			value += target->ModDamageTakenByMechFlat[MECHANIC_BLEEDING];
		}
	}
	else if( sp->GetProto()->eff[ i ].Effect == SPELL_EFFECT_SCHOOL_DAMAGE && sp->u_caster)
//		value = float2int32(295 + (sp->u_caster->GetAP() * 0.115f));
		value += float2int32( sp->u_caster->GetAP() * 0.0552f );
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_77758(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->u_caster )
	{
		//Deals ${($AP*0.0982+$m1)*1.04} to ${($AP*0.0982+$M1)*1.04} damage
		if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE )
		{
			value += float2int32( sp->u_caster->GetAP() * 0.0982f );	
			value = float2int32( value * 1.04f );
		}
		else if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_PERIODIC_DAMAGE )
		{
			//bleed for ${($AP*0.0167+$m2)*1.04*3} damage over $d
//			value = ( value + float2int32( sp->u_caster->GetAP() * 0.033f ) ) / sp->GetProto()->quick_tickcount;	
			value = float2int32( ( value + sp->u_caster->GetAP() * 0.0167f ) * 1.04f);	
			//hmmm, when we fix that spell mechanic is not effect mechanic this will double boost spells
			if( target && sp->GetProto()->MechanicsType != MECHANIC_BLEEDING )
			{
				value += value * target->ModDamageTakenByMechPCT[MECHANIC_BLEEDING] / 100;
				value += target->ModDamageTakenByMechFlat[MECHANIC_BLEEDING];
			}
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_52610(Spell *sp,Unit *target, int32 &value, int32 i)
{
//Finishing move that consumes combo points on any nearby target to increase autoattack damage done by $62071s2%.  Only useable while in Cat Form.  Lasts longer per combo point:
//   1 point  : 14 seconds
//   2 points: 19 seconds
//   3 points: 24 seconds
//   4 points: 29 seconds
//   5 points: 34 seconds
	if( i == 0 && sp->p_caster )
	{
		uint32 duration_mod = sp->p_caster->m_comboPoints;
		uint32 new_dutaion = 9000 + 5000 * duration_mod;
		sp->forced_duration = new_dutaion;
		sp->Dur = new_dutaion;
		sp->m_requiresCP++;
//		sp->p_caster->NullComboPoints();
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_779(Spell *sp,Unit *target, int32 &value, int32 i)
{
	// ${$AP*0.06+$m1} damage.
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE && sp->u_caster != NULL )
//		value+=float2int32( sp->u_caster->GetAP()*0.06f );
		value+=float2int32( sp->u_caster->GetAP()*0.06f*2.30f );	//using this to get same value as client says :(
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_16857(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && target )
	{
		if( target->IsStealth() )
			target->RemoveStealth();
	}
	// Deals ${$AP*0.15+1} damage and additional threat when used in Bear Form.
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE && sp->p_caster != NULL )
	{
		if( sp->p_caster->GetShapeShift() == FORM_BEAR )
			value= 1 + float2int32( sp->u_caster->GetAP()*0.15f );
		else
			value = 0;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_6807(Spell *sp,Unit *target, int32 &value, int32 i)
{
	// An attack that instantly deals ${8+$ap*0.36-1} physical damage.
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE && sp->p_caster != NULL )
	{
		value = 7 + float2int32( sp->u_caster->GetAP()*0.36f );
		if( target && sp->GetProto()->MechanicsType != MECHANIC_BLEEDING )
		{
			value += value * target->ModDamageTakenByMechPCT[MECHANIC_BLEEDING] / 100;
			value += target->ModDamageTakenByMechFlat[MECHANIC_BLEEDING];
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_22570(Spell *sp,Unit *target, int32 &value, int32 i)
{
		//Finishing move that causes damage and stuns the target.  Causes more damage and lasts longer per combo point:
//   1 point  : ${$b1*1+$m1+1.55*$mw}-${$b1*1+$M1+1.55*$MW} damage, 1 sec
//   2 points: ${$b1*2+$m1+1.55*$mw}-${$b1*2+$M1+1.55*$MW} damage, 2 sec
//   3 points: ${$b1*3+$m1+1.55*$mw}-${$b1*3+$M1+1.55*$MW} damage, 3 sec
//   4 points: ${$b1*4+$m1+1.55*$mw}-${$b1*4+$M1+1.55*$MW} damage, 4 sec
//   5 points: ${$b1*5+$m1+1.55*$mw}-${$b1*5+$M1+1.55*$MW} damage, 5 sec
	if(sp->GetProto()->eff[i].Effect == SPELL_EFFECT_WEAPON_DAMAGE && sp->p_caster )
	{
		uint32 point_count = sp->p_caster->m_comboPoints;
		uint32 dolarB = value - sp->damage;
		uint32 dolarM = value - dolarB;
		value = dolarB * point_count + dolarM;
//		sp->p_caster->NullComboPoints();	//just in case spell is missing combo cost
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_5221(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_WEAPON_DAMAGE )
	{
		value = value * 267 / 100;	//this is according to wowhead description. 267 is actually the value of effect weapon percent damage that will be calculated in the future
		if( target && sp->GetProto()->MechanicsType != MECHANIC_BLEEDING )
			value += target->ModDamageTakenByMechFlat[MECHANIC_BLEEDING];
	} 
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_WEAPON_PERCENT_DAMAGE )
	{
		if( target && sp->GetProto()->MechanicsType != MECHANIC_BLEEDING )
			sp->forced_pct_mod_cur = sp->forced_pct_mod_cur * ( 100 + target->ModDamageTakenByMechPCT[MECHANIC_BLEEDING] ) / 100;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_8092(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE && sp->p_caster )
	{
		//eat up shadow orbs
		uint32 PCT_mod = 0, PCT_mod_count = 0, PCT_mod_DOTS = 0;
		for(uint32 i = FIRST_AURA_SLOT; i < sp->u_caster->m_auras_neg_size; ++i)
			if( sp->u_caster->m_auras[i] 
				&& sp->u_caster->m_auras[i]->GetSpellProto()->NameHash == SPELL_HASH_SHADOW_ORB )
			{ 
				PCT_mod += sp->u_caster->m_auras[i]->GetSpellProto()->eff[0].EffectBasePoints;
				PCT_mod_count++;
				sp->u_caster->m_auras[i]->Remove();
			}
		//each point of mastery will further improve PCT mod
		if( PCT_mod )
		{
			Aura *a = sp->u_caster->HasAuraWithNameHash( SPELL_HASH_SHADOW_ORB_POWER, 0, AURA_SEARCH_PASSIVE );
			if( a )
			{
				int32 var_inc = float2int32( sp->p_caster->GetFloatValue( PLAYER_MASTERY ) * a->GetSpellProto()->eff[1].EffectBasePoints );
				PCT_mod_DOTS = PCT_mod + var_inc / 100;
				PCT_mod = PCT_mod + PCT_mod_count * var_inc / 100;
			}
			//also cast Empowered Shadow
//			sp->u_caster->CastSpell( sp->u_caster, 95799, true );
			SpellCastTargets targets( sp->u_caster->GetGUID() );
			SpellEntry *spellInfo = dbcSpell.LookupEntry( 95799 );
			Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
			spell->Init( sp->u_caster, spellInfo ,true, NULL);
			spell->forced_basepoints[ 0 ] = PCT_mod_DOTS;
			spell->forced_basepoints[ 1 ] = PCT_mod_DOTS;
			spell->forced_basepoints[ 2 ] = PCT_mod_DOTS;
			spell->prepare(&targets);
		}
		sp->forced_pct_mod_cur += PCT_mod;
	} 
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_73510(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE && sp->p_caster )
	{
		//eat up shadow orbs
		uint32 PCT_mod = 0, PCT_mod_count = 0, PCT_mod_DOTS = 0;
		for(uint32 i = FIRST_AURA_SLOT; i < sp->u_caster->m_auras_pos_size; ++i)
			if( sp->u_caster->m_auras[i] 
				&& sp->u_caster->m_auras[i]->GetSpellProto()->NameHash == SPELL_HASH_SHADOW_ORB )
			{ 
				PCT_mod += sp->u_caster->m_auras[i]->GetSpellProto()->eff[0].EffectBasePoints;
				sp->u_caster->m_auras[i]->Remove();
				//also cast Empowered Shadow
//				sp->u_caster->CastSpell( sp->u_caster, 95799, true );
				PCT_mod_count++;
			}
		//extinguishes your shadow damage-over-time effects from the target in the process.
		for(uint32 i = MAX_POSITIVE_AURAS; i < target->m_auras_neg_size; ++i)
		{
			Aura *a = target->m_auras[i];
			if( a 
				&& ( a->GetSpellProto()->SchoolMask & SCHOOL_MASK_SHADOW )
				&& ( a->GetSpellProto()->NameHash != SPELL_HASH_MIND_SPIKE )
				&& ( a->GetSpellProto()->c_is_flags & SPELL_FLAG_IS_DAMAGING )
				&& ( a->GetSpellProto()->quick_tickcount > 1 )
				&& ( a->GetCaster() == sp->p_caster ) )
			{ 
				a->Remove();
			}
		}

		//each point of mastery will further improve PCT mod
		if( PCT_mod )
		{
			Aura *a = sp->u_caster->HasAuraWithNameHash( SPELL_HASH_SHADOW_ORB_POWER, 0, AURA_SEARCH_PASSIVE );
			if( a )
			{
				int32 var_inc = float2int32( sp->p_caster->GetFloatValue( PLAYER_MASTERY ) * a->GetSpellProto()->eff[1].EffectBasePoints );
				PCT_mod_DOTS = PCT_mod + var_inc / 100;
				PCT_mod = PCT_mod + PCT_mod_count * var_inc / 100;
			}
			SpellCastTargets targets( sp->u_caster->GetGUID() );
			SpellEntry *spellInfo = dbcSpell.LookupEntry( 95799 );
			Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
			spell->Init( sp->u_caster, spellInfo ,true, NULL);
			spell->forced_basepoints[ 0 ] = PCT_mod;
			spell->forced_basepoints[ 1 ] = PCT_mod;
			spell->forced_basepoints[ 2 ] = PCT_mod;
			spell->prepare(&targets);
		}
		sp->forced_pct_mod_cur += PCT_mod;
	} 
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
/*
SPELL_EFFECT_OVERRIDE_RETURNS EH_62606(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_SCHOOL_ABSORB && sp->u_caster )
		value = value * sp->u_caster->GetAP() / 100;
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}*/

SPELL_EFFECT_OVERRIDE_RETURNS EH_49184(Spell *sp,Unit *target, int32 &value, int32 i)
{
	//((1257 + 1387 / 2) + AP * 0.44)) Frost damage to that foe, 
	//and ((0.5 * (((1257 + 1387 / 2) + (AP * 0.44)))) Frost damage to all other enemies within 0 yards.)
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE && sp->p_caster && target )
	{
		value += float2int32( sp->u_caster->GetAP() * 0.44f );
		if( target->GetGUID() != sp->p_caster->GetSelection() )
			value = value / 2;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_5308(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE 
		&& sp->p_caster && target 
		&& ( target->HasAuraStateFlag(AURASTATE_HEALTH_BELOW_20_PCT) || sp->p_caster->IgnoreSpellSpecialCasterStateRequirement!=0 )
		)
	{
		uint32 max_rage_convert = sp->GetProto()->eff[1].EffectBasePoints * 10;	//200
		/*
		Attempt to finish off a wounded foe, causing (10 + AP * 0.437 * 100 / 100) physical damage and 
		consumes up to 20 additional rage to deal up to ($ap * 0.874 * 100 / 100 - 1) additional damage.  
		Only usable on enemies that have less than 20% health.

		Attempt to finish off a wounded foe, causing ${10+$AP*0.437*$m1/100} physical damage and 
		consumes up to $m2 additional rage to deal up to ${$ap*0.874*$m1/100-1} additional damage.  
		Only usable on enemies that have less than 20% health.
		*/

		//1 proc of this
//		sp->u_caster->RemoveAuraByNameHash( SPELL_HASH_SUDDEN_DEATH );

		uint32 dolarB = value - sp->damage;
		uint32 dolarM = value - dolarB;
		float add_damage = 10 + sp->u_caster->GetAP() * 0.437f * dolarM / 100 ;
		//this is a big hack to get the same values as client shows on GUI :(
//		float add_damage = 10 + sp->u_caster->GetAP() * 0.787f * dolarM / 100 ;
		int32 rage_consume;
		if( sp->u_caster->GetPower( POWER_TYPE_RAGE ) > (int32) max_rage_convert )
			rage_consume = max_rage_convert;
		else
			rage_consume = sp->u_caster->GetPower( POWER_TYPE_RAGE );
		if( rage_consume > 0 )
			add_damage += rage_consume * sp->u_caster->GetAP() * 0.874f * dolarM / 100 / 200;

		value = float2int32( add_damage );
		// send spell damage log
	//	SpellEntry *sp_for_the_logs = dbcSpell.LookupEntry(20647);
	//	pSpell->u_caster->Strike( target, MELEE, sp_for_the_logs, float2int32( add_damage ), 0, 0, true, true );
		Aura *a = sp->u_caster->HasAuraWithNameHash( SPELL_HASH_SUDDEN_DEATH, 0, AURA_SEARCH_PASSIVE );
		if( a )
			rage_consume -= a->GetSpellProto()->eff[0].EffectBasePoints * 10;
		rage_consume = MAX( 0, rage_consume ); //safety
		sp->power_cost = rage_consume;
		sp->u_caster->ModPower( POWER_TYPE_RAGE, -rage_consume );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_2565(Spell *sp,Unit *target, int32 &value, int32 i)
{
	//	Increases your chance to block by $s1% for $d.  
	// In addition, if your total chance to block or avoid an attack exceeds 100%, your chance to critically block is increased by the excess.
	if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MOD_CRIT_BLOCK_CHANCE_PCT && sp->p_caster )
	{
		if( sp->p_caster->GetFloatValue( PLAYER_BLOCK_PERCENTAGE ) > 100 )
			value = float2int32( sp->p_caster->GetFloatValue( PLAYER_BLOCK_PERCENTAGE ) ) - 100;
		else
			value = 0;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_47541(Spell *sp,Unit *target, int32 &value, int32 i)
{
	//Fire a blast of unholy energy, causing (985 + 0.3 * AP * 1) Shadow damage to an enemy target
	// or healing ((985 + 0.3 * AP * 1) * 3.5) damage on a friendly Undead target.
	/*
84,"$morbidity=$?s49565[${1.15}]
?s49564[${1.10}]
?s48963[${1.05}]
[${1.00}]
$damage=
$?s63333[${($m1+0.27*$AP)*1.15*$<morbidity>}]
[${($m1+0.3*$AP)*$<morbidity>}]
$healing=${$<damage>*3.5}",
*/
	if( sp->u_caster )
	{
		value += float2int32( sp->u_caster->GetAP() * 0.3f );
		if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_HEAL )
			value = float2int32( value * 3.5f );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_8676(Spell *sp,Unit *target, int32 &value, int32 i)
{
	//Ambush the target, causing 190% weapon damage plus (367 * 190 / 100) to the target 
	//((190 * 1.447)% plus (367 * 190 / 100 * 1.447) if a dagger is equipped).  Must be stealthed and behind the target.  Awards 2 combo point.
	//Ambush the target, causing $m2% weapon damage plus ${$m1*$m2/100} to the target (${$m2*1.447}% plus ${$m1*$m2/100*1.447} if a dagger is equipped). 
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_WEAPON_PERCENT_DAMAGE )
	{
		uint32 dolarB = value - sp->damage;
		uint32 dolarM = value - dolarB;
		sp->add_damage = sp->add_damage * dolarM / 100;
		if( sp->p_caster && sp->p_caster->GetItemInterface() )
		{
			Item *it = sp->p_caster->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_MAINHAND);
			if( it != NULL && it->GetProto()->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER )
			{
//				sp->forced_pct_mod_cur += 45;
				value = value * 1447 / 1000;
				sp->add_damage = sp->add_damage * 1447 / 1000;
			}
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
/*
SPELL_EFFECT_OVERRIDE_RETURNS EH_47540(Spell *sp,Unit *target, int32 &value, int32 i)
{
	//lame hack: this spell has too many effects. It should have another 2 effects to insta cast trigger spell on target
	if( target && sp->u_caster )
		sp->u_caster->CastSpell( target, sp->GetProto()->eff[i].EffectTriggerSpell, true );
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}*/

SPELL_EFFECT_OVERRIDE_RETURNS EH_22568(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->p_caster != NULL ) //Ferocious Bite
	{
		//Finishing move that causes damage per combo point and consumes up to 35 additional energy to increase damage by up to 100%.  Damage is increased by your attack power.
		//Finishing move that causes damage per combo point and consumes up to $m2 additional energy to increase damage by up to 100%.  Damage is increased by your attack power.
		//1 point  : ${$m1+$b1*1*$<mult>+0.109*$AP*$<mult>}-${$M1+$b1*1*$<mult>+0.109*$AP*$<mult>} damage
		//2 points: ${$m1+$b1*2*$<mult>+0.218*$AP*$<mult>}-${$M1+$b1*2*$<mult>+0.218*$AP*$<mult>} damage
		//1 point  : (200 + 501 * 1 * 1 + 0.109 * AP * 1)-(434 + 501 * 1 * 1 + 0.109 * AP * 1) damage
		//2 points: (200 + 501 * 2 * 1 + 0.218 * AP * 1)-(434 + 501 * 2 * 1 + 0.218 * AP * 1) damage
		//3 points: (200 + 501 * 3 * 1 + 0.327 * AP * 1)-(434 + 501 * 3 * 1 + 0.2327 * AP * 1) damage
		//4 points: (200 + 501 * 4 * 1 + 0.436 * AP * 1)-(434 + 501 * 4 * 1 + 0.436 * AP * 1) damage
		//5 points: (200 + 501 * 5 * 1 + 0.545 * AP * 1)-(434 + 501 * 5 * 1 + 0.545 * AP * 1) damage
		uint32 point_count = sp->p_caster->m_comboPoints;
		if( point_count )
		{
			//formula is not perfect because value contains some randomness and that gets multiplied by max 5 times
			uint32 point_count = sp->p_caster->m_comboPoints;
			uint32 dolarB = value - sp->damage;
			uint32 dolarM = value - dolarB;
			uint32 min_value = dolarM + float2int32( ( dolarB + sp->p_caster->GetAP() * 0.109f) * point_count );
			uint32 rand_max  = value + 1;
			uint32 random = RandomUInt() % rand_max;
			value = min_value + random;
//			sp->p_caster->NullComboPoints();	//just in case spell is missing combo cost
			int32 energy_consume = sp->p_caster->GetPower( POWER_TYPE_ENERGY );
			if( energy_consume > sp->GetProto()->eff[1].EffectBasePoints )
				energy_consume = sp->GetProto()->eff[1].EffectBasePoints;
			value += value * energy_consume / sp->GetProto()->eff[1].EffectBasePoints;
			sp->p_caster->ModPower( POWER_TYPE_ENERGY, -energy_consume );
			sp->p_caster->UpdatePowerAmm( true, POWER_TYPE_ENERGY );
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
/*
SPELL_EFFECT_OVERRIDE_RETURNS EH_2812(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE && sp->u_caster )
	{
		uint32 counter = sp->u_caster->GetPower( POWER_TYPE_HOLY );
		counter = MAX( counter, (uint32)sp->power_cost );
		//Sends bolts of holy power in all directions, causing (0.61 * holy power + 2435) Holy damage divided among all targets within 10 yds and stunning all Demons and Undead for 3 sec.
		value += float2int32( 0.61f * counter );
		value = value / MAX(1,(uint32)sp->m_targetUnits[i].size());	//divide the dmg among targets
		sp->u_caster->SetPower(  POWER_TYPE_HOLY, 0 );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
} */

SPELL_EFFECT_OVERRIDE_RETURNS EH_83381(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE && sp->u_caster )
	{
		// 219,"$bmMastery=$?s76657[${1+0.01*$76657m1}][${1}]$damage=${($83381m1+($RAP*0.516))*$<bmMastery>}
		//Give the command to kill, causing your pet to instantly inflict (849 + (RAP * 0.516)) damage to its target. Your Pet's happiness increases the damage done.
		if( sp->u_caster->IsPet() && SafePetCast( sp->u_caster )->GetPetOwner() )
		{
			Player *owner = SafePetCast( sp->u_caster )->GetPetOwner();
			value += float2int32( owner->GetRAP() * 0.516f );	//owner will read spell description ...
//			value = float2int32( value * SafePetCast( sp->u_caster )->GetHappinessDmgMod() );
			//this is Killing Streak talent proc effect
			int32 SMT_PCT_mod = 0;
			if( owner->SM_Mods )
			{
				SM_FIValue( owner->SM_Mods->SM_PDamageBonus,&SMT_PCT_mod,sp->GetProto()->GetSpellGroupType());
//				value += value * SMT_PCT_mod / 100;
				sp->forced_pct_mod_cur += SMT_PCT_mod;
			}
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_16827(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE && sp->u_caster )
	{
		// (186 + ((RAP * 0.40) * 0.20)) damage.
		if( sp->u_caster->IsPet() && SafePetCast( sp->u_caster )->GetPetOwner() )
		{
			value += float2int32( ( SafePetCast( sp->u_caster )->GetPetOwner()->GetRAP() * 0.40f ) * 0.20f );	//owner will read spell description ...
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_17253(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE && sp->u_caster )
	{
		// (186 + ((RAP * 0.40) * 0.20)) damage.
		if( sp->u_caster->IsPet() && SafePetCast( sp->u_caster )->GetPetOwner() )
		{
			value = float2int32( ( value + SafePetCast( sp->u_caster )->GetPetOwner()->GetRAP() * 0.40f ) * 0.20f );	//owner will read spell description ...
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_91322(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && sp->u_caster )
		sp->u_caster->RemoveAuraByNameHash( SPELL_HASH_INNER_EYE, 0, AURA_SEARCH_POSITIVE, MAX_PASSIVE_AURAS );
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_18562(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_HEAL && target )
	{
		if( sp->p_caster && sp->p_caster->HasGlyphWithID( GLYPH_DRUID_SWIFTMEND ) )
			return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
		if( target->RemoveAuraByNameHash( SPELL_HASH_REGROWTH, 0, AURA_SEARCH_POSITIVE ) == 0 )
			target->RemoveAuraByNameHash( SPELL_HASH_REJUVENATION, 0, AURA_SEARCH_POSITIVE );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_8647(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && sp->p_caster && sp->p_caster->HasAuraWithNameHash( SPELL_HASH_IMPROVED_EXPOSE_ARMOR, 0, AURA_SEARCH_PASSIVE ) )
		sp->m_requiresCP = -100;
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_60103(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_WEAPON_PERCENT_DAMAGE && sp->p_caster )
	{
		Player *p_caster = sp->p_caster;
		bool has_flametongue = false;
		if( p_caster->GetItemInterface() )
		{
			Item *it = p_caster->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_OFFHAND );
			if( it == NULL )
				return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
		
			EnchantmentInstance * ench = it->GetEnchantment( ITEM_ENCHANT_SLOT_TEMPORARY2 ); // temp enchantment slot
			if(ench)
			{
				EnchantEntry* Entry = ench->Enchantment;
				for( uint32 c = 0; c < 3; c++ )
				{
					if( Entry->type[c] && Entry->spell[c] )
					{
						SpellEntry *sp = dbcSpell.LookupEntry( Entry->spell[c] );
						if( sp != NULL && sp->NameHash == SPELL_HASH_FLAMETONGUE_WEAPON__PASSIVE_ )
						{
							has_flametongue = true;
							break;
						}
					}
				}
			}
		}
		//crappy hackfixes
/*		Aura *a = p_caster->HasAuraWithNameHash( SPELL_HASH_IMPROVED_LAVA_LASH, 0, AURA_SEARCH_PASSIVE );
		if( a )
		{
			uint32 counter = target->RemoveAuraByNameHash( SPELL_HASH_SEARING_FLAMES, 0, AURA_SEARCH_NEGATIVE, MAX_PASSIVE_AURAS );
			sp->forced_pct_mod_cur += a->GetSpellProto()->eff[1].EffectBasePoints * counter;
			Aura *b = p_caster->HasAuraWithNameHash( SPELL_HASH_ITEM___SHAMAN_T12_ENHANCEMENT_2P_BONUS, 0, AURA_SEARCH_PASSIVE );
			if( b )
				sp->forced_pct_mod_cur += counter * b->GetSpellProto()->eff[0].EffectBasePoints;
		}*/
		//extra dmg pct
		if( has_flametongue )
		{
			int32 PCTBonus = sp->GetProto()->eff[1].EffectBasePoints;
//			sp->forced_pct_mod_cur += sp->GetProto()->eff[1].EffectBasePoints;
			sp->forced_pct_mod_cur = sp->forced_pct_mod_cur * ( 100 + PCTBonus ) / 100;
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_73981(Spell *sp,Unit *target, int32 &value, int32 i)
{
//	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_ADD_COMBO_POINTS && sp->p_caster )
	if( i == 0 && sp->p_caster )
	{
		Player *p_caster = sp->p_caster;
		p_caster->m_comboTarget = target->GetGUID();
		p_caster->UpdateComboPoints();
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_1064(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_HEAL )
	{
		if( target )
		{
			if( sp->u_caster->HasAuraWithNameHash( SPELL_HASH_ITEM___SHAMAN_T12_RESTORATION_4P_BONUS, 0, AURA_SEARCH_PASSIVE ) )
				sp->forced_pct_mod_cur += 25;	//crap crap crap, fixed values = bad ! this is hardcoded in riptide 3rd effect
			else if( target->RemoveAuraByNameHash( SPELL_HASH_RIPTIDE, 0, AURA_SEARCH_POSITIVE, MAX_PASSIVE_AURAS ) )
				sp->forced_pct_mod_cur += 25;	//crap crap crap, fixed values = bad ! this is hardcoded in riptide 3rd effect
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_543(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_SCHOOL_ABSORB && sp->u_caster )
	{
		value += float2int32( sp->u_caster->GetDamageDoneMod( SCHOOL_ARCANE ) * 0.807f );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_1463(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MANA_SHIELD && sp->u_caster )
	{
		value += float2int32( sp->u_caster->GetDamageDoneMod( SCHOOL_ARCANE ) * 0.807f );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_11426(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_SCHOOL_ABSORB && sp->u_caster )
	{
		value += float2int32( sp->u_caster->GetDamageDoneMod( SCHOOL_ARCANE ) * 0.807f );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
/*
SPELL_EFFECT_OVERRIDE_RETURNS EH_82366(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->p_caster )
	{
		//8 * (0 + 0.04 * holy power.04 * AP) Holy damage over 10 sec
		value = float2int32( sp->p_caster->GetAP() * ( sp->p_caster->GetPower( POWER_TYPE_HOLY )+ 0.04f ) * 0.32f);
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}*/

SPELL_EFFECT_OVERRIDE_RETURNS EH_31930(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_PERIODIC_ENERGIZE && sp->u_caster )
	{
		value = sp->u_caster->GetUInt32Value( UNIT_FIELD_BASE_MANA ) * value / 100 / sp->GetProto()->quick_tickcount;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_73680(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && sp->p_caster && target )
	{
		if( sp->p_caster->GetItemInterface() )
		{
			Item *it[2];
			it[ 0 ] = sp->p_caster->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_MAINHAND );
			it[ 1 ] = sp->p_caster->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_OFFHAND );
			for( int32 i=0;i<2;i++)
				if( it[i] != NULL )
				{
					EnchantmentInstance * ench = it[i]->GetEnchantment( ITEM_ENCHANT_SLOT_TEMPORARY2 ); 
					if(ench)
					{
						if( ench->Enchantment->Id == 3345 ) // Earthliving Weapon
							sp->p_caster->CastSpell( target, 73685, true);
						else if( ench->Enchantment->Id == 5 ) // Flametongue Weapon
							sp->p_caster->CastSpell( target, 73683, true);
						else if( ench->Enchantment->Id == 2 ) // Frostbrand Weapon
							sp->p_caster->CastSpell( target, 73682, true);
						else if( ench->Enchantment->Id == 3021 ) // Rockbiter Weapon
							sp->p_caster->CastSpell( target, 73684, true);
						else if( ench->Enchantment->Id == 283 ) // Windfury Weapon
							sp->p_caster->CastSpell( target, 73681, true); // Unleash Wind
					}
				}
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_82692(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MOD_RANGED_HASTE && sp->p_caster && target )
	{
		Pet *pet = sp->p_caster->GetSummon();
		if( pet )
		{
			uint32 stack_count = pet->RemoveAuraByNameHash( SPELL_HASH_FRENZY_EFFECT, 0, AURA_SEARCH_POSITIVE, MAX_PASSIVE_AURAS );
			value = value * stack_count;
		}
		else
			value = 0;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_7814(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if(sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE && sp->u_caster )
	{
		//causing (208 + (0.612 * ($SP * 0.5)))  Shadow damage.
		value += float2int32( 0.612f * ( sp->u_caster->GetDamageDoneMod( SCHOOL_SHADOW ) + 0.5f ) );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_63375(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if(sp->GetProto()->eff[i].Effect == SPELL_EFFECT_ENERGIZE && sp->u_caster )
	{
		value = sp->u_caster->GetUInt32Value( UNIT_FIELD_BASE_MANA ) * value / 100;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}


SPELL_EFFECT_OVERRIDE_RETURNS EH_44572(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( target 
		&& sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MOD_STUN 
		&& target->IsCreature()
		&& SafeCreatureCast( target )->GetProto()
		&& (SafeCreatureCast( target )->GetProto()->modImmunities & 32) != 0 //MECHANIC_STUNNED
		)
	{
		uint8 has_chill_effect = 0;
		for(uint32 t=NEG_AURAS_START;t<MAX_NEGATIVE_AURAS1(target);t++)
			if( target->m_auras[ t ] 
				&& target->m_auras[ t ]->GetSpellProto()->c_is_flags & SPELL_FLAG_IS_CHILL_SPELL )
			{
				has_chill_effect = 1;
				break;
			}

		if( has_chill_effect == false && sp->u_caster )
			has_chill_effect = (sp->u_caster->HasAuraWithNameHash( SPELL_HASH_FINGERS_OF_FROST, 0, AURA_SEARCH_POSITIVE )!=NULL);

		SpellCastTargets targets2( target->GetGUID() ); 
		SpellEntry *spellInfo = dbcSpell.LookupEntry( 71757 );	
		Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		spell->Init( sp->m_caster, spellInfo ,true, NULL);
		spell->forced_pct_mod[0] = 100 + 100*has_chill_effect; 
		spell->prepare(&targets2);
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_79136(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if(sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE && sp->u_caster )
	{
		//only a mith ?
		value += float2int32( 0.176f * sp->u_caster->GetAP() );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_79637(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if(sp->GetProto()->eff[i].Effect == SPELL_EFFECT_DUMMY && sp->u_caster )
	{
		if( sp->u_caster->GetStat( STAT_AGILITY ) > sp->u_caster->GetStat( STAT_STRENGTH ) && sp->u_caster->GetStat( STAT_AGILITY ) > sp->u_caster->GetStat( STAT_INTELLECT ) )
			sp->u_caster->CastSpell( sp->u_caster, 79639, true );
		else if( sp->u_caster->GetStat( STAT_STRENGTH ) > sp->u_caster->GetStat( STAT_INTELLECT ) )
			sp->u_caster->CastSpell( sp->u_caster, 79638, true );
		else
			sp->u_caster->CastSpell( sp->u_caster, 79640, true );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_5487(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if(sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MOD_BASE_RESISTANCE_PCT && sp->u_caster )
	{
		if( sp->u_caster->getLevel() < 65 )
			value = 40;
		else
			value = 120;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_10444(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if(sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE && sp->p_caster )
	{
		//get avg weapon speed( because we have no idea which weapon is proccing this effect )
		uint32 number_of_enchants = 0;
		uint32 sum_weapon_speeds = 0;
		Item *it[2];
		it[ 0 ] = sp->p_caster->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_MAINHAND );
		it[ 1 ] = sp->p_caster->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_OFFHAND );
		for( int32 i=0;i<2;i++)
			if( it[i] != NULL )
			{
				EnchantmentInstance * ench = it[i]->GetEnchantment( ITEM_ENCHANT_SLOT_TEMPORARY2 ); 
				if( ench && ench->Enchantment->Id == 5 ) // Flametongue Weapon
				{
					number_of_enchants++;
					sum_weapon_speeds += it[i]->GetProto()->Delay;
				}
			}
		if( number_of_enchants == 0 )
			value = 0;
		else
		{
			Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
			spell->Init( sp->p_caster, dbcSpell.LookupEntryForced( 8024 ) ,true, NULL);
			int32 S2 = spell->CalculateEffect( 1, target );
			int32 MaxValue = S2 / 25;
			int32 MinValue = S2 / 77;
			int32 ValueDiff = MaxValue - MinValue;
			SpellPool.PooledDelete( spell, __FILE__, __LINE__ );

			uint32 avg_speed = sum_weapon_speeds / number_of_enchants;
			//we say that a weapon with speed 2100 gets full bonus = 100% and tha min PCT is given to weapon speed 1 sec as 20 %
			// ps : there are weapons with speed 8, shadowmourne has 4 sec speed
			int32 value_pct_mod = avg_speed * 100 / 2100;
			value = MinValue + ValueDiff * value_pct_mod / 100;
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_89906(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_PERIODIC_ENERGIZE && sp->u_caster )
	{
		value = value * sp->u_caster->GetUInt32Value( UNIT_FIELD_BASE_MANA ) / 100 / sp->GetProto()->quick_tickcount;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_25742(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE && sp->u_caster )
	{
		uint32 weapon_speed = 2000;
		if( sp->p_caster && sp->p_caster->GetItemInterface() )
		{
			Item *it = sp->p_caster->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_MAINHAND );
			if(it)
				weapon_speed = it->GetProto()->Delay;
		}
		// (MWS * (0.011 * AP.022 * holy power) * 100 / 100) 
//		uint32 HOLYP = sp->u_caster->GetPower( POWER_TYPE_HOLY );		//rigth now my client says 208 and i deal 9
//		value = float2int32( weapon_speed / 1000.0f * 0.011f * HOLYP * ( sp->u_caster->GetAP() + 0.22f ) );
		uint32 HOLYP = sp->u_caster->GetDamageDoneMod( SCHOOL_HOLY );	//${$MWS*(0.011*$AP+0.022*$SPH)*$<sop>/100} 
		value = float2int32( weapon_speed / 1000.0f * ( 0.011f * sp->u_caster->GetAP() + 0.022f * HOLYP ) );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_1766(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 
		&& sp->p_caster 
		&& target
		&& target->isCasting()
		&& sp->p_caster->HasGlyphWithID( GLYPH_ROGUE_KICK )
		)
	{
		sp->p_caster->ModCooldown( sp->GetProto()->Id, 6000 + 4000, false );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_90361(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->u_caster )
	{
		int32 RAP = sp->u_caster->GetTopOwner()->GetRAP();
		if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_HEAL )
		{
			value += float2int32( RAP * 0.35f * 0.5f );
		}
		else if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_PERIODIC_HEAL )
		{
			value += float2int32( RAP * 0.35f * 0.335f );	//spell already has flag to divide ticks over interval
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
/*
SPELL_EFFECT_OVERRIDE_RETURNS EH_22842(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->u_caster )
	{
		if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MOD_HEALING_PCT )
		{
			//has nothing to do with this effect, in previous effect we reaised health by 30%
			//now we make sure our health is not less then 30% of max health
			if( sp->u_caster->GetHealthPct() < 30 )
			{
				uint32 pct30health = sp->u_caster->GetMaxHealth() * 30 / 100;
				sp->u_caster->SetHealth( pct30health );
			}
		}
//		else if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MOD_INCREASE_HEALTH_2 )
		{
			//increases health to $22842s2% (if below that value)
//			int32 max_increase = target->GetMaxHealth() * value / 100;
//			int32 can_increase = MAX( 0, max_increase - (int32)target->GetHealth() );
//			value = can_increase;
//			return SPELL_EFFECT_OVERRIDE_BREAK_EXECUTION;
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}*/

SPELL_EFFECT_OVERRIDE_RETURNS EH_80313(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_WEAPON_PERCENT_DAMAGE && target )
	{
		uint32 counter = target->RemoveAuraByNameHash( SPELL_HASH_LACERATE, 0, AURA_SEARCH_NEGATIVE, MAX_PASSIVE_AURAS );
		counter = MIN( counter, 5 ); //should stack max 3. Just a safery check
		if( counter > 0 )
		{
			//Deals 80% weapon damage plus additional (450 * 80 / 100) damage for each of your Lacerate applications on the target
			sp->add_damage += sp->GetProto()->eff[0].EffectBasePoints * sp->GetProto()->eff[2].EffectBasePoints * counter / 100;
			//and increases your melee critical strike chance by 3% for each Lacerate application consumed for 10 sec.
			SpellCastTargets targets2( sp->m_caster->GetGUID() ); 
			SpellEntry *spellInfo = dbcSpell.LookupEntry( 80951 );	
			Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
			spell->Init( sp->m_caster, spellInfo ,true, NULL);
			spell->forced_basepoints[0] = spellInfo->eff[0].EffectBasePoints * counter; 
			spell->ProcedOnSpell = sp->GetProto();
			spell->pSpellId=sp->GetProto()->Id;
			spell->prepare(&targets2);
		}
	} 
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_16511(Spell *sp,Unit *target, int32 &value, int32 i)
{
	//An instant strike that deals 110% weapon damage ((110 * 1.45)% if a dagger is equipped)
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_WEAPON_PERCENT_DAMAGE )
	{
		if( sp->p_caster && sp->p_caster->GetItemInterface() )
		{
			Item *it = sp->p_caster->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_MAINHAND);
			if( it != NULL && it->GetProto()->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER )
				sp->forced_pct_mod_cur += 45;
		}
		if( sp->p_caster != NULL )
			sp->p_caster->AddComboPoints( sp->p_caster->GetSelection(), 1);
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_50464(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_HEAL && target )
	{
		//this darn thing seems to create a stack corruption or somehow alter spell "unitTarget" variable
		uint32 index_limit = MIN( MAX_POSITIVE_AURAS1( target ), MAX_POSITIVE_AURAS );
		for( uint32 j = POS_AURAS_START; j < index_limit; j++ )
		{
			Aura *a = target->m_auras[j];
			if(  a
				&& ( a->GetSpellProto()->NameHash == SPELL_HASH_REJUVENATION
					|| a->GetSpellProto()->NameHash == SPELL_HASH_REGROWTH
					|| a->GetSpellProto()->NameHash == SPELL_HASH_LIFEBLOOM
					|| a->GetSpellProto()->NameHash == SPELL_HASH_WILD_GROWTH
				) )
				{
					sp->forced_pct_mod_cur += 20;
					break;
				}
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_29166(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_PERIODIC_ENERGIZE ) 
	{
		value = sp->u_caster->GetMaxPower(  POWER_TYPE_MANA ) * value / 100;
		if( target == sp->p_caster )
		{
			Aura *a = target->HasAuraWithNameHash( SPELL_HASH_DREAMSTATE, 0, AURA_SEARCH_PASSIVE );
			if( a )
				value += value * a->GetSpellProto()->eff[0].EffectBasePoints / 100;
		}
		value = value / sp->GetProto()->quick_tickcount;	//value over duration
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_55095(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_PERIODIC_DAMAGE && sp->u_caster ) 
	{
		value = float2int32( ( value + sp->u_caster->GetAP() * 0.055f ) * 1.15f );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_91711(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_SCHOOL_ABSORB && sp->u_caster )
	{
		value += float2int32( sp->u_caster->GetDamageDoneMod( SCHOOL_ARCANE ) * 0.807f );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_48181(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( target && sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE )
	{
		//dealing ((($SP * 0.5577) * 1.25) + 922) Shadow damage
//		value += float2int32( sp->u_caster->GetDamageDoneMod( SCHOOL_SHADOW ) * sp->u_caster->GetDamageDonePctMod( SCHOOL_SHADOW ) * 0.5577f * 1.25f );
		//register a proc struct that will gather DMG done by this effect ONLY
		ProcTriggerSpell *pts = new ProcTriggerSpell( dbcSpell.LookupEntryForced( 50091 ), sp->m_caster );
		pts->procFlags = PROC_ON_ANY_DAMAGE_VICTIM;
		pts->procChance = 100;
		pts->spellId = 50091;	//!!self proc for scripting
		pts->caster = sp->m_caster->GetGUID();
		pts->created_with_value = 0;	//we will sum the dmg here
		pts->LastTrigger = 0; //instantly trigger
		target->RegisterProcStruct( pts );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_19434(Spell *sp,Unit *target, int32 &value, int32 i)
{
	//deals 200% ranged weapon damage plus (RAP * 0.724)+776.
	//209,"$damage=${($RAP*0.724)+$m1}",
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_WEAPON_PERCENT_DAMAGE && sp->u_caster )
	{
		sp->add_damage += float2int32( sp->u_caster->GetRAP() * 0.724f );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_6262(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && sp->u_caster && sp->u_caster->HasAuraWithNameHash( SPELL_HASH_SOULBURN, 0, AURA_SEARCH_POSITIVE ) )
		sp->u_caster->CastSpell( sp->u_caster, 79437, true ); // Soulburn: Healthstone
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_HEAL && sp->u_caster )
	{
		value = value * sp->u_caster->GetUInt32Value( UNIT_FIELD_BASE_HEALTH ) / 100;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_85696(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && sp->u_caster )
	{
		sp->power_cost = 0;
		sEventMgr.AddEvent(sp->u_caster, &Unit::Energize, sp->u_caster, (uint32)85696, (int32)3, (uint32)POWER_TYPE_HOLY, (uint8)0, EVENT_DELAYED_SPELLCAST, 1, 1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
//		sp->u_caster->Energize( sp->u_caster, 0, 3, POWER_TYPE_HOLY );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_6229(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_SCHOOL_ABSORB && sp->u_caster )
	{
		value += float2int32( sp->u_caster->GetDamageDoneMod( SCHOOL_SHADOW ) * 0.807f );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_83853(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_PERIODIC_DAMAGE && sp->u_caster && target )
	{
		//count the fire dots DTS on target
		int32 DPS_SUM = 0;	//we scale 10 times DPS cause some proc in less then 1 second
//		int32 RemainingDMGSum = 0;
		int32 sum_counter = 0;
		for(uint32 t=NEG_AURAS_START;t<MAX_NEGATIVE_AURAS1(target);t++)
		{
			Aura *a = target->m_auras[ t ];
			if( a 
				&& a->GetCasterGUID() == sp->m_caster->GetGUID()
				&& a->GetSpellProto()->NameHash != SPELL_HASH_COMBUSTION
				)
			{
				SpellEntry *spe = a->GetSpellProto();
				if( spe->SchoolMask & SCHOOL_MASK_FIRE )
				{
					for( uint32 i=0;i<a->m_modcount;i++)
						if( spe->eff[ a->m_modList[i].i ].EffectApplyAuraName == SPELL_AURA_PERIODIC_DAMAGE )
						{

							Unit *u_caster = sp->u_caster;
							Unit *u_victim = target;
							dealdamage tdmg;
							tdmg.base_dmg = a->m_modList[i].m_amount;
							tdmg.pct_mod_final_dmg = a->m_modList[i].m_pct_mod;
							tdmg.DisableProc = true;
							tdmg.StaticDamage = false;
							u_caster->SpellNonMeleeDamageCalc( u_victim, a->GetSpellProto(), &tdmg, a->GetSpellProto()->quick_tickcount, a->m_modList[i].i );
							//remove critical. Conflagrate can crit too
							if( tdmg.pct_mod_crit )
								tdmg.full_damage = tdmg.full_damage * 100 / ( 100 + tdmg.pct_mod_crit );
							int32 TickDamage = tdmg.full_damage - tdmg.resisted_damage - tdmg.absorbed_damage;
							if( TickDamage < 0 )
								TickDamage = 0;

//							int32 base_dmg = a->m_modList[i].m_amount;
//							int32 bonus_dmg = 0;
//							if( uc )
//								bonus_dmg = uc->GetSpellDmgBonus( target, spe, base_dmg, a->m_modList[i].i );
							
//							DPS_SUM += (base_dmg+bonus_dmg) * 1000 / sp->eff[ a->m_modList[i].i ].EffectAmplitude;
							int32 TickDuration = GetSpellAmplitude( spe, u_caster, a->m_modList[i].i, a->m_castedItemId );
							int32 TickCount = a->GetDuration() / TickDuration;
//							int32 TickDamage = (base_dmg+bonus_dmg);
							if( TickDamage < 40000 )
							{
//								DPS_SUM += TickDamage * 1000 / TickDuration;
								DPS_SUM += TickDamage * TickCount;

//								int32 RemainingTime = a->GetTimeLeft();
//								int32 RemainingTicks = ( RemainingTime / TickDuration );
//								RemainingDMGSum += TickDamage * RemainingTicks;

								sum_counter++;
							}
						}
				}
			}
		}
		if( DPS_SUM > 0 )
		{
//			value = DPS_SUM * sp->GetProto()->eff[i].EffectAmplitude / ( 1000 * sum_counter );
//			value = DPS_SUM * 1000 / sp->GetProto()->eff[i].EffectAmplitude ;
			int32 TickDuration = GetSpellAmplitude( sp->GetProto(), sp->u_caster, i, 0 );
			int32 TickCount = sp->GetDuration() / TickDuration;
			value = DPS_SUM / TickCount;
		}
//		value = RemainingDMGSum / sp->GetProto()->eff[i].EffectAmplitude;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
/*
SPELL_EFFECT_OVERRIDE_RETURNS EH_63560(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && sp->u_caster )
	{
		sp->u_caster->RemoveAura( 93426, 0 );	//dark transformation enabler
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}*/

SPELL_EFFECT_OVERRIDE_RETURNS EH_73975(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MOD_HEAL_ABSORB && sp->u_caster && target )
	{
		value += float2int32( sp->u_caster->GetAP() * 0.75f );
		//it gets reduced by resiliance
		int32 absorb = target->ResilianceAbsorb( value, sp->GetProto(), sp->u_caster );
		value -= absorb;
		if( value < 0 )
			value = 0;
		//!! this effect stacks, but not as aura ! This is a dirty hack !
		//worst case we should find the aura on target and add the ammount from that. But this is the only spell using this type of aura ..
		value += target->HealAbsorb;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_43265(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_PERIODIC_DAMAGE && sp->u_caster && target )
	{
		// causing ${$m1+($AP*0.064)} Shadow damage every sec that targets remain in the area for $d.
		value += float2int32( sp->u_caster->GetAP() * 0.064f );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_85222(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_HEAL && sp->u_caster )
	{
		uint32 counter = sp->u_caster->GetPower( POWER_TYPE_HOLY );
		counter = MAX( counter, (uint32)sp->power_cost );
		//Sends bolts of holy power in all directions, causing (0.61 * holy power + 2435) Holy damage divided among all targets within 10 yds and stunning all Demons and Undead for 3 sec.
		//Consumes all Holy Power to send a wave of healing energy before you, healing up to $?s54940[4][6] of the most injured targets in your party or raid within a 30 yard frontal cone for $s1 per charge of Holy Power.
//		uint32 randompoints = value / 2 + ( RandomUInt() % ( value / 2 ) );
//		value = ( value + randompoints ) * ( counter + 1 );
		value = ( value ) * ( counter + 1 );
		sp->u_caster->SetPower( POWER_TYPE_HOLY, 0 );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_84996(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && sp->p_caster && sp->p_caster->GetGroup() && sp->p_caster->IsGroupLeader() )
	{
		DynamicObject *dynObj = sp->p_caster->GetMapMgr()->CreateDynamicObject();
		dynObj->invisible = true;	//group only can see

		sp->p_caster->GetGroup()->SpawnRaidMarker( sp->GetProto()->Id, dynObj );
		if( sp->m_targets.m_destX != 0 )
		{
			dynObj->SetInstanceID(sp->p_caster->GetInstanceID());
			dynObj->Create( sp->p_caster, sp, sp->m_targets.m_destX, sp->m_targets.m_destY, sp->m_targets.m_destZ, 30*60*1000, 5, value, sp->GetProto()->eff[0].EffectAmplitude);
		}
		//because it belongs to group and not the caster
//		sp->p_caster->dynObj = NULL;
		//avoid doing whatever else spell wanted to do
		sp->redirected_effect[ i ] = SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_ALL_TARGETS;
	}
	return SPELL_EFFECT_OVERRIDE_BREAK_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_6789(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE && target )
	{
		sp->forced_basepoints[2] = target->GetHealth();
		sp->forced_basepoints[3] = value;
		sp->m_targets.m_unitTarget = target->GetGUID(); //just making sure
	}
	else if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_HEAL && target )
	{
		//description says 300% and effect has 244% ?
		Unit *ttarget = target->GetMapMgr()->GetUnit( sp->m_targets.m_unitTarget );
		if( ttarget )
			sp->forced_basepoints[2] = MAX( 1, 3 * ( sp->forced_basepoints[2] - (int32)ttarget->GetHealth() ) );
		else
			sp->forced_basepoints[2] = MAX( 1, 3 * sp->forced_basepoints[3] );
//		sp->forced_basepoints[2] = MAX( 1, sp->forced_basepoints[2] );	//just in case heal event ocured
//		sp->forced_basepoints[2] = MAX( 1, 3 * MAX( MIN( 100000, SafePlayerCast(target)->m_dmg_made_last_time ), sp->forced_basepoints[3] )  );	//well this sucks, because if some dmg procs then last dmg made will be small
		value = sp->forced_basepoints[2];
		sp->static_dmg[2] = value;
	}
	return SPELL_EFFECT_OVERRIDE_BREAK_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_88942(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE )
	{
		uint32 tcount = MAX( (uint32)sp->m_targetUnits[i].size(),1 );
		sp->forced_basepoints[i] = value / tcount;	//split dmg between targets
		value = value / tcount;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_99353(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE )
	{
		value = MAX( target->GetInt32Value( UNIT_FIELD_MAXHEALTH ) * 90 / 100, sp->GetProto()->eff[2].EffectBasePoints );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_98649(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE && sp->u_caster )
	{
		value = sp->u_caster->GetHealth();
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_49998(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_WEAPON_PERCENT_DAMAGE && sp->p_caster )
	{
		if( sp->p_caster->HasGlyphWithID( GLYPH_DEATHKNIGHT_DEATH_STRIKE ) )
		{
			uint32 RunicPower = sp->p_caster->GetPower( POWER_TYPE_RUNIC ) / 10;
			uint32 PCTBoost = MIN( RunicPower * 2 / 5, 40 );
			value += PCTBoost;
		}
	}
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_HEAL_MAXHEALTH_PCT && sp->p_caster )
	{
		//count dmg suffered in the last 5 minutes
		uint32 StoreSecond = getMSTime() / 1000;
		int32 DmgSuffered = 0;
		for( int32 t=0;t<5;t++)
		{
			uint32 StoreSecondIndex = ( StoreSecond - t );
			uint32 StoreIndex = StoreSecondIndex % 5;
			uint32 *Store = (uint32*)sp->p_caster->GetCreateIn64Extension( EXTENSION_ID_DAMAGE_SUFFERED_INPAST_1 + StoreIndex  ); 
			if( Store[0] < StoreSecond - 5 )
				continue;
			DmgSuffered += Store[1];
		}
		DmgSuffered = DmgSuffered * 20 / 100;
		int32 PCTHealthLost = MIN( 100, DmgSuffered * 100 / sp->p_caster->GetMaxHealth() );
		value = MAX( value, PCTHealthLost );

		if( sp->p_caster->HasAura( 101568, 0, AURA_SEARCH_POSITIVE ) ) //Dark Succor
			value = MAX( value, 20 );
		sp->static_dmg[i] = value; //do not crit or mod by other
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;	//needed for blood shield to proc
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_99007(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_ENERGIZE && sp->u_caster )
	{
		value = value * target->GetUInt32Value( UNIT_FIELD_BASE_MANA ) / 100;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_55090(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_DUMMY && sp->p_caster && target )
	{
		uint32 disease_cnt = target->GetDKDiseaseCount();
		disease_cnt = MIN( disease_cnt, 3 );
		value = sp->p_caster->m_dmg_made_last_time * value * disease_cnt / 100;
		if( value > 1 )
		{
			SpellEntry* TriggeredSpell = dbcSpell.LookupEntryForced( sp->GetProto()->eff[i].EffectTriggerSpell );
			if( TriggeredSpell == NULL )
				return SPELL_EFFECT_OVERRIDE_BREAK_EXECUTION;
			Spell *sp2=SpellPool.PooledNew( __FILE__, __LINE__ );
			sp2->Init( sp->p_caster,TriggeredSpell,true,NULL);
			sp2->forced_basepoints[0] = value;	
			SpellCastTargets tgt( target->GetGUID() );
			sp2->prepare(&tgt);
		}
	}
	return SPELL_EFFECT_OVERRIDE_BREAK_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_687(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MOD_REPLACE_SPELL && sp->p_caster )
	{
		if( sp->p_caster->HasAuraWithNameHash( SPELL_HASH_NETHER_WARD, 0, AURA_SEARCH_PASSIVE ) )
			value = 91711;	//transform shadow ward into this
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_38177(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && sp->p_caster )
	{
		sp->p_caster->CastSpell( target, 38178, true );
		if( target->IsCreature() )
			SafeCreatureCast(target)->Despawn(1000, 360000);
	}
	return SPELL_EFFECT_OVERRIDE_BREAK_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_6615(Spell *sp,Unit *target, int32 &value, int32 i)
{
//Makes you immune to stun and movement impairing effects for the next (cond($gt(level, 77), 30 - (level) * 3, 30)) sec.   Does not remove effects already on the imbiber.
	if( i == 0 && sp->u_caster && sp->u_caster->getLevel() > 77 )
	{
		uint32 new_dutaion = 1000 * ( 30 - (sp->u_caster->getLevel() - 77) * 3 );
		sp->forced_duration = new_dutaion;
		sp->Dur = new_dutaion;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_44614(Spell *sp,Unit *target, int32 &value, int32 i)
{
	//slow , direct dmg , dot
	if( sp->p_caster && sp->redirected_effect[i] == 0 )
	{
		if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_PERIODIC_DAMAGE )
		{
			if( sp->p_caster->HasGlyphWithID( GLYPH_MAGE_FROSTFIRE ) == false )
			{
				sp->redirected_effect[i] = SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_ALL_TARGETS;
				return SPELL_EFFECT_OVERRIDE_BREAK_EXECUTION;
			}
		}
		//if we have glyph we already redirected this effect
		if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MOD_DECREASE_SPEED && sp->p_caster )
		{
			if( sp->p_caster->HasGlyphWithID( GLYPH_MAGE_FROSTFIRE ) == true )
			{
				sp->redirected_effect[i] = SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_ALL_TARGETS;
				return SPELL_EFFECT_OVERRIDE_BREAK_EXECUTION;
			}
			//do not allow slow to stack
			else 
			{
				Aura *a = target->HasAuraWithNameHash( SPELL_HASH_FROSTFIRE_BOLT, 0, AURA_SEARCH_NEGATIVE );
				if( a )
				{
					a->ResetDuration();
					sp->redirected_effect[i] = SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_ALL_TARGETS;
					return SPELL_EFFECT_OVERRIDE_BREAK_EXECUTION;
				}
			}
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_6360(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE && sp->u_caster )
	{
		Unit *uo = sp->u_caster->GetTopOwner();
		int32 sp_shadow = uo->GetDamageDoneMod( SCHOOL_SHADOW );
		value += float2int32( sp_shadow * 0.85f*0.5f );	
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_85948(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCRIPT_EFFECT && target )
	{
		//and increases the duration of your Blood Plague, Frost Fever, and Chains of Ice effects on the target by up to 6 sec.
		Aura *a;
		a = target->HasAuraWithNameHash( SPELL_HASH_BLOOD_PLAGUE, 0, AURA_SEARCH_NEGATIVE );
		if( a )
		{
//			uint32 new_dur = MAX( a->GetDuration(), 6000 );
			uint32 new_dur = a->GetDuration() + 6000;
			a->SetDuration( new_dur );
			a->ResetDuration();
		}
		a = target->HasAuraWithNameHash( SPELL_HASH_FROST_FEVER, 0, AURA_SEARCH_NEGATIVE );
		if( a )
		{
//			uint32 new_dur = MAX( a->GetDuration(), 6000 );
			uint32 new_dur = a->GetDuration() + 6000;
			a->SetDuration( new_dur );
			a->ResetDuration();
		}
		a = target->HasAuraWithNameHash( SPELL_HASH_CHAINS_OF_ICE, 0, AURA_SEARCH_NEGATIVE );
		if( a )
		{
//			uint32 new_dur = MAX( a->GetDuration(), 6000 );
			uint32 new_dur = a->GetDuration() + 6000;
			a->SetDuration( new_dur );
			a->ResetDuration();
		}
	}
	return SPELL_EFFECT_OVERRIDE_BREAK_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_109831(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_HEALTH_LEECH && sp->u_caster )
	{
		//amount equal to ${$m1/10}.1% of your maximum health
		value = sp->u_caster->GetMaxHealth() * ( value + 1 ) / 100 / 10;
		value = float2int32( value / sp->GetProto()->eff[i].EffectConversionCoef ); //we are calculating dmg that will get converted into heal
		//morron protection
		if( value < 0 )
			value = 1;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_1464(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && sp->u_caster )
	{
		sp->u_caster->setAttackTimer( 0, true );
		sp->u_caster->setAttackTimer( 0, false );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_34428(Spell *sp,Unit *target, int32 &value, int32 i)
{
	//causing ${$AP*$m1/100} damage
	if(i==0 && sp->u_caster)
	{
		value = (value*sp->u_caster->GetAP())/100;
		sp->u_caster->RemoveAuraStateFlag( AURASTATE_LASTKILLWITHHONOR );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_54640(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && sp->p_caster && sp->p_caster->m_bg && sp->p_caster->m_bg->GetType() == BATTLEGROUND_STRAND_OF_THE_ANCIENT )
	{
		StrandOfTheAncient * sota = static_cast<StrandOfTheAncient *>(sp->p_caster->m_bg);
		sota->OnPlatformTeleport(sp->p_caster);
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_21651(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && sp->p_caster )
	{
		sp->p_caster->RemoveStealth();
		sp->p_caster->RemoveInvisibility();
		sp->p_caster->RemoveAuraByNameHash(SPELL_HASH_DIVINE_SHIELD);
		sp->p_caster->RemoveAuraByNameHash(SPELL_HASH_DIVINE_PROTECTION);
		sp->p_caster->RemoveAuraByNameHash(SPELL_HASH_BLESSING_OF_PROTECTION);
		sp->p_caster->ExitVehicle();
		sp->p_caster->InterruptSpell(); //to avoid the bug of casting mount then capping flag
		if( sp->p_caster->IsMounted() )
			sp->p_caster->RemoveAura( sp->p_caster->m_MountSpellId, 0, AURA_SEARCH_POSITIVE );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_1784(Spell *sp,Unit *target, int32 &value, int32 i)
{
	Player *p_caster;
	if( target && target->IsPlayer() )
		p_caster = SafePlayerCast( target );
	else
		p_caster = sp->p_caster;
	if( i == 0 && p_caster && p_caster->m_bg  )
	{
		if( p_caster->m_bg )
		{
			p_caster->m_bg->HookOnFlagDrop( p_caster );
			if( p_caster->isCasting() && ( p_caster->GetCurrentSpell()->GetProto()->c_is_flags2 & SPELL_FLAG2_IS_BREAKING_STEALTH ) )
				p_caster->GetCurrentSpell()->safe_cancel();
		}
/*		if(p_caster->m_bg->GetType() == BATTLEGROUND_WARSONG_GULCH
			|| p_caster->m_bg->GetType() == BATTLEGROUND_TWIN_PEAKS
			)
		{
			if(p_caster->GetTeam() == 0)
				p_caster->RemoveAura(23333);	// ally player drop horde flag if they have it
			else
				p_caster->RemoveAura(23335); 	// horde player drop ally flag if they have it
		}
		else if(p_caster->m_bg->GetType() == BATTLEGROUND_EYE_OF_THE_STORM)
			p_caster->RemoveAura(34976);	// drop the flag
			*/
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_1856(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && target == sp->p_caster )
	{
		Player *p_caster = sp->p_caster;
		target->AquireInrangeLock();
		InrangeLoopExitAutoCallback AutoLock;
		for (InRangeSetRecProt::iterator iter = target->GetInRangeSetBegin( AutoLock );iter != target->GetInRangeSetEnd(); ++iter)
		{
			if ((*iter) == NULL || !(*iter)->IsUnit())
				continue;

			if (!SafeUnitCast(*iter)->isAlive())
				continue;

//			if( SafeUnitCast(*iter)->CanSee( sp->p_caster ) == false ) //vanish is a special stealth
			{
				if (SafeUnitCast(*iter)->GetCurrentSpell() && SafeUnitCast(*iter)->GetCurrentSpell()->GetUnitTarget() == target)
					SafeUnitCast(*iter)->GetCurrentSpell()->safe_cancel();

				if(SafeUnitCast(*iter)->GetAIInterface() != NULL )
					SafeUnitCast(*iter)->GetAIInterface()->RemoveThreatByPtr( target );
			}
		}			
		target->ReleaseInrangeLock();
		for( uint32 x = MAX_POSITIVE_AURAS; x < MAX_NEGATIVE_AURAS1(target); x++ )
		{
			if( target->m_auras[x] != NULL )
			{
				if( target->m_auras[x]->GetSpellProto()->MechanicsType == MECHANIC_ROOTED || target->m_auras[x]->GetSpellProto()->MechanicsType == MECHANIC_ENSNARED ) // Remove roots and slow spells
				{
					target->m_auras[x]->Remove();
				}
				else // if got immunity for slow, remove some that are not in the mechanics
				{
					if( HasAuraType( target->m_auras[x]->GetSpellProto(), SPELL_AURA_MOD_DECREASE_SPEED )
						|| HasAuraType( target->m_auras[x]->GetSpellProto(), SPELL_AURA_MOD_ROOT )
						|| HasAuraType( target->m_auras[x]->GetSpellProto(), SPELL_AURA_MOD_STALKED )
						)
					{
						target->m_auras[x]->Remove();
					}
				}
			}
		}

		if(p_caster->IsMounted())
			p_caster->RemoveAura(p_caster->m_MountSpellId);

//		p_caster->CastSpell( p_caster, 1784, true );	//stealth
	}

	//redirecting handler for BG check
	EH_1784( sp, target, value, i );

	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
/*
SPELL_EFFECT_OVERRIDE_RETURNS EH_33750(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_WEAPON_DAMAGE )
	{
		Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		spell->Init( refunit, dbcSpell.LookupEntryForced( 33757 ), true, NULL);
		value = CalculateEffect( 1, target );
		SpellPool.PooledDelete( spell );
	} 
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}*/
/*
SPELL_EFFECT_OVERRIDE_RETURNS EH_77478(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE && sp->u_caster )
	{
		//dealing ${$77478m1+$SPN*0.11} Physical damage
		//the trick is in the talent that converts AP to SP
		value += float2int32( sp->u_caster->GetDamageDoneMod( SCHOOL_NATURE ) * 0.11f );
	} 
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}*/

SPELL_EFFECT_OVERRIDE_RETURNS EH_53385(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->p_caster ) //heal is based on total damage made to N enemies
	{
//		if( i == 0 && forced_basepoints[ 2 ] == 0 )
//		{
//			forced_basepoints[ 2 ] = p_caster->m_dmg_made_since_login; 
//		}
//		else 
			if( i == 1 )
		{
			//not good since some procs may alter the actual dmg we are looking for
//			int32 dmg_diff = p_caster->m_dmg_made_since_login - forced_basepoints[ 2 ];
//			if( dmg_diff < 0 ) 
//				dmg_diff = 0;	//noway
			//value = dmg_diff * ( GetProto()->eff[1].EffectBasePoints + 1) / 100; 
			//learn to code protection
			uint32 hit_target_with_dmg = (uint32)(sp->m_targetUnits[0].size());
			if( hit_target_with_dmg > 3 )
			{
				if( hit_target_with_dmg >= 4 )
					sp->forced_basepoints[2] = 1;
				else
					sp->redirected_effect[2] = SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_ALL_TARGETS;
				hit_target_with_dmg = 3;
			}
			//bug protection
			if( sp->p_caster->m_dmg_made_last_time > 10000 )
				sp->p_caster->m_dmg_made_last_time = 10000;
			value = sp->p_caster->m_dmg_made_last_time * hit_target_with_dmg * ( sp->GetProto()->eff[1].EffectBasePoints ) / 100; 
			//now divide the amount between the number of party members we are healing
			if( sp->m_targetUnits[1].size() )
				value = value / (int32)(sp->m_targetUnits[1].size());
			//!!! hackfix to avoid making paladins unkillable by protecting themself then maxheal
			if( target && value > (int32)target->GetUInt32Value( UNIT_FIELD_MAXHEALTH ) * 20 / 100 )
				value = target->GetUInt32Value( UNIT_FIELD_MAXHEALTH ) * 20 / 100;
			//do not mod it by SP
			sp->static_dmg[ i ] = value+1;
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}


SPELL_EFFECT_OVERRIDE_RETURNS EH_81708(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_HEAL && sp->u_caster )
	{
		//amount equal to 1.6% of your maximum health -> this is according to some wowhead desciting 
		value = target->GetMaxHealth() * 16 / 1000;
		sp->static_dmg[ i ] = value+1;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_56131(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_HEAL && target )
	{
		//amount equal to ${$m1/10}.1% of your maximum health
		value = target->GetMaxHealth() * value / 100;
		//morron protection
		if( value < 0 )
			value = 1;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_20243(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_HEALTH_LEECH && target )
	{
		//amount equal to ${$m1/10}.1% of your maximum health
		value = target->GetMaxHealth() * value / 100;
		//morron protection
		if( value < 0 )
			value = 1;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
/*
SPELL_EFFECT_OVERRIDE_RETURNS EH_12292(Spell *sp,Unit *target, int32 &value, int32 i)
{
	//note this is just a hack for noobs ! because the aura has already 0 value( should have )
	if( i == 0 && sp->p_caster && sp->p_caster->HasGlyphWithID( GLYPH_WARRIOR_DEATH_WISH ) )
	{
		if( sp->GetProto()->eff[2].EffectApplyAuraName == SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN )
			sp->redirected_effect[2] = SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_ALL_TARGETS;
		else if( sp->GetProto()->eff[1].EffectApplyAuraName == SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN )
			sp->redirected_effect[1] = SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_ALL_TARGETS;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}*/

SPELL_EFFECT_OVERRIDE_RETURNS EH_33872(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MOD_HEALING_PCT && target )
	{
		if( target->IsPlayer() && SafePlayerCast( target )->GetShapeShift() != FORM_CAT )
		{
			value = 0;
			sp->redirected_effect[i] = SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_ALL_TARGETS;
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_48505(Spell *sp,Unit *target, int32 &value, int32 i)
{
	//reset our internal star hit counter
	if( i == 0 && sp->p_caster )
	{
		int64 *p = sp->p_caster->GetCreateIn64Extension( EXTENSION_ID_STARFALL_HIT_COUNTER );
		*p = 0;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_50288(Spell *sp,Unit *target, int32 &value, int32 i)
{
	//Every time a star hits we increase the hit counter. After a fixed hit count we start ignoring extra hits
	if( i == 0 && sp->p_caster )
	{
		int64 *p = sp->p_caster->GetCreateIn64Extension( EXTENSION_ID_STARFALL_HIT_COUNTER );
		if( *p >= 20 )
		{
			sp->redirected_effect[0] = SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_ALL_TARGETS;
			sp->redirected_effect[1] = SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_ALL_TARGETS;
			sp->redirected_effect[2] = SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_ALL_TARGETS;
		}
		*p = *p + 1;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_33763(Spell *sp,Unit *target, int32 &value, int32 i)
{
	//Every time a star hits we increase the hit counter. After a fixed hit count we start ignoring extra hits
	if( i == 0 && sp->p_caster && target )
	{
		if( sp->p_caster->GetShapeShift() != FORM_TREE )
		{
			uint64 *p = (uint64 *)sp->p_caster->GetCreateIn64Extension( EXTENSION_ID_LIFEBLOOM_PREV_TARGET );
			Unit *old_target = sp->p_caster->GetMapMgr()->GetUnit( *p );
			if( old_target != NULL && old_target != target )
				old_target->RemoveAura( sp->GetProto()->Id, 0, AURA_SEARCH_POSITIVE, MAX_AURAS ); //same target can stack it 3 times
			*p = target->GetGUID();
		}
	}
	//only direct heal when we got removed. This will be force enabled by script
	if( i == 0 )
		sp->redirected_effect[ 1 ] = SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_ALL_TARGETS;

	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_50461(Spell *sp,Unit *target, int32 &value, int32 i)
{
	//Every time a star hits we increase the hit counter. After a fixed hit count we start ignoring extra hits
	if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_SCHOOL_ABSORB && sp->u_caster )
	{
		Unit *owner = sp->u_caster->GetTopOwner();
		value = 10000 + owner->GetAP() * 2;	//it's in the description ?
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
} 

SPELL_EFFECT_OVERRIDE_RETURNS EH_19505(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_DISPEL && sp->u_caster )
	{
		//ffs, i donno how to make this work, let's count the auras on the target
		uint32 count_auras = 0;
		for( uint32 i = FIRST_AURA_SLOT; i < MAX_POSITIVE_AURAS1( target ); i++ )
			if( target->m_auras[i] )
				count_auras++;
		sp->forced_basepoints[2] = count_auras;
		sp->m_targets.m_unitTarget = target->GetGUID();
	}
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_TRIGGER_SPELL && sp->u_caster && target->IsPet() )
	{
		Unit *dispel_target = sp->u_caster->GetMapMgr()->GetUnit( sp->m_targets.m_unitTarget );
		if( dispel_target )
		{
			uint32 count_auras = 0;
			for( uint32 i = FIRST_AURA_SLOT; i < MAX_POSITIVE_AURAS1( dispel_target ); i++ )
				if( dispel_target->m_auras[i] )
					count_auras++;
			if( count_auras == sp->forced_basepoints[2] )
			{
				sp->redirected_effect[1] = SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_ALL_TARGETS;
				sp->redirected_effect[2] = SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_ALL_TARGETS;
			}
		}
	}
	//could have put this in heal spell target gathering also
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_TRIGGER_SPELL && sp->u_caster && target->IsPlayer() )
	{
		if( SafePlayerCast( target )->HasGlyphWithID( GLYPH_WARLOCK_OF_FELHUNTER ) == false )
			sp->redirected_effect[i] = SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_ALL_TARGETS;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
} 

SPELL_EFFECT_OVERRIDE_RETURNS EH_19658(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_HEAL && sp->u_caster && target )
	{
		Unit *owner;
		if( target->IsPet() )
			owner = target->GetTopOwner();
		else
			owner = target;
		value = value + float2int32( owner->GetDamageDoneMod( SCHOOL_SHADOW ) * 0.5f * 0.3f );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_55233(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MOD_INCREASE_HEALTH && target )
	{
		value = target->GetMaxHealth() * value / 100 ;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_86346(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MOD_DMG_PENETRATE && target && target->IsPlayer() )
	{
		value = value / 2;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_1776(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && sp->p_caster )
	{
		if( sp->u_caster != NULL )
			value += (uint32)ceilf(sp->u_caster->GetAP() * 0.21f);
		sp->forced_miscvalues[i] = 0;
		//signal the interrupt handler that we do not break on bleeds
		if( sp->p_caster->HasAuraWithNameHash( SPELL_HASH_SANGUINARY_VEIN, 0, AURA_SEARCH_PASSIVE ) )
		{
			for( uint32 j=0;j<MAX_SPELL_EFFECT_COUNT;j++)
				if( sp->GetProto()->eff[j].EffectApplyAuraName == SPELL_AURA_MOD_STUN )
					sp->forced_miscvalues[j] = 1;
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_527(Spell *sp,Unit *target, int32 &value, int32 i)
{
	//only remove debuffs from self if we did not remove on first effect
	if( i == 1 )
	{
		if( sp->forced_basepoints[0] > 1 )
			sp->redirected_effect[i] = SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_ALL_TARGETS;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_104322(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && sp->u_caster )
		sp->u_caster->Heal( sp->u_caster, sp->GetProto()->Id, value * 10 );
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_103527(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE && sp->m_targetUnits[i].size() )
	{
		//Deals $s1 Shadow damage, split between all enemy targets within $a1 yards of the impact crater.
		value = value / (int32)(sp->m_targetUnits[i].size());	//if this is 0 then this function would never get called
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_110317(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE && target )
	{
		value = target->GetMaxHealth() * value / 100;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_54049(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE )
	{
		// ${$M1+(1.228*($SP*0.5))} Shadow damage plus an additional $s3% damage for each of your damage over time effects on the target.
		uint32 dolarB = value - sp->damage;
		uint32 dolarM = value - dolarB;
		float SP;
		if( sp->u_caster )
		{
			Unit *u_caster = sp->u_caster->GetTopOwner();
			SP = u_caster->GetDamageDoneMod( SCHOOL_SHADOW );
		}
		else
			SP = 0;
		uint32 DOTCount = 0;
		for(uint32 t=NEG_AURAS_START;t<MAX_NEGATIVE_AURAS1(target);t++)
		{
			Aura *a = target->m_auras[ t ];
			if( a )
			{
				SpellEntry *spl = a->GetSpellProto();
				if( ( spl->c_is_flags & SPELL_FLAG_IS_DAMAGING ) && spl->quick_tickcount > 1 )
					DOTCount++;
			}
		}
		value = float2int32( ( dolarM + 1.228*SP*0.5f ) * ( 100 + DOTCount * sp->GetProto()->eff[2].EffectBasePoints ) / 100 );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_3110(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE )
	{
		// Deals (((($SP * 0.50) * 0.657) + 124) * 1) Fire damage to a target.
		float SP;
		if( sp->u_caster )
			SP = sp->u_caster->GetTopOwner()->GetDamageDoneMod( SCHOOL_SHADOW );
		else
			SP = 0;
		value = float2int32( 0.657f*0.5f*SP ) + value;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_80353(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( target && target->HasAura( 80354, 0, AURA_SEARCH_NEGATIVE ) )
	{
		sp->redirected_effect[ i ] |= SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_THIS_TARGET;
		return SPELL_EFFECT_OVERRIDE_BREAK_EXECUTION;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_90355(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( target && target->HasAura( 95809, 0, AURA_SEARCH_NEGATIVE ) )
	{
		sp->redirected_effect[ i ] |= SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_THIS_TARGET;
		return SPELL_EFFECT_OVERRIDE_BREAK_EXECUTION;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_100977(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->p_caster )
	{
		int32 var_inc = float2int32( sp->p_caster->GetFloatValue( PLAYER_MASTERY ) * 125 );	//125 comes from 77495
		value = var_inc / 100; // -> mastery is scaled by 100 to store it in INT32
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_25228(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_SPLIT_DAMAGE )
	{
		if( sp->p_caster && sp->p_caster->HasGlyphWithID( GLYPH_WARLOCK_SOUL_LINK ) )
			value += dbcSpell.LookupEntryForced( 63312 )->eff[0].EffectBasePoints; // Glyph of Soul Link
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_16191(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MOD_STAT && sp->u_caster )
	{
		value = value * sp->u_caster->GetStat( STAT_SPIRIT ) / 100;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_15290(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_HEAL )
	{
		if( sp->u_caster != target )
		value = value / 2; //half heal for rest of the party
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_48411(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_DUMMY )
	{
		//we will handle this as dummy and cast another spell
		if( sp->p_caster && ( sp->p_caster->GetShapeShift() == FORM_TREE || sp->p_caster->GetShapeShift() == 0 ) )
			sp->redirected_effect[0] = SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_THIS_TARGET;
		//disable the healing aura
		else
			sp->redirected_effect[1] = SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_THIS_TARGET;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
/*
SPELL_EFFECT_OVERRIDE_RETURNS EH_13797(Spell *sp,Unit *target, int32 &value, int32 i)
{
// Place a fire trap that will burn the first enemy to approach for $<damage> Fire damage over $13797d.  Trap will exist for $13795d.
// 121,"$damage=$?s56846[${($RAP*0.04+$13797m1)*3}][${($RAP*0.02+$13797m1)*5}]]
// $duration=$?s56846[${3}][${5}]
// $mult=$?s56846[${4}][${2}]",
// Glyph of Immolation Trap : Decreases the duration of the effect from your Immolation Trap by 6 sec, but damage while active is increased by $56846s2%.

	if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_PERIODIC_DAMAGE && sp->u_caster )
	{
		value += float2int32( sp->u_caster->GetRAP() * 0.02f );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
} */

SPELL_EFFECT_OVERRIDE_RETURNS EH_30213(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_WEAPON_DAMAGE )
	{
		//plus ${(($SP*0.50)*2)*0.264+$m2} divided among all targets within $A2 yards
		uint32 SP = sp->u_caster->GetDamageDoneMod( SCHOOL_SHADOW );
		value += float2int32( SP * 0.264f );
		uint32 TargetCount = (uint32)sp->UniqueTargets.size();
		if( TargetCount > 0 )
		{
//			value /= TargetCount;
			sp->forced_pct_mod_cur = 100 / TargetCount;
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_1120(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_PERIODIC_DAMAGE && target )
	{
		if( target->GetHealthPct() <= 25 )
			sp->forced_pct_mod_cur += 100;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
/*
SPELL_EFFECT_OVERRIDE_RETURNS HandleReplenishment2(Spell *sp,Unit *target, int32 &value, int32 i)
{
	//there are 2 Replenishment spells that do different things !
	//+ Judgements of the Wise
	if( target && sp->GetProto()->eff[ i ].EffectApplyAuraName == SPELL_AURA_PERIODIC_ENERGIZE )
	{
		value = target->GetMaxPower( POWER_TYPE_MANA ) * value * 15 / 10 / 100;
		//if i have 30k mana * 1% = 300
		value = value / sp->GetProto()->quick_tickcount;		// the value is over 10 seconds, aura duration is 15 seconds every 1 second
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}*/

SPELL_EFFECT_OVERRIDE_RETURNS EH_57669(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_PERIODIC_ENERGIZE && target )
	{
		//Replenishes $s1% of maximum mana per 10 sec for $57669d.
		uint32 ManaPer10Sec = target->GetMaxPower( POWER_TYPE_MANA ) * 1 / 100;
		uint32 TickDuration = sp->GetProto()->eff[i].EffectAmplitude;
		float  TicksIn10Sec = 10000.0f / TickDuration;
		uint32 RegenPerTick = float2int32( ManaPer10Sec / TicksIn10Sec );
		value = RegenPerTick; /**/
		//if i have 30k mana * 1% = 300
//		value = target->GetMaxPower( POWER_TYPE_MANA ) * value * 15 / 10 / 100;
//		value = value / sp->GetProto()->quick_tickcount;		// the value is over 10 seconds, aura duration is 15 seconds every 1 second
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_770(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && target )
	{
		if( target->IsStealth() )
			target->RemoveStealth();
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_7744(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && sp->u_caster )
	{
		sp->u_caster->CastSpell( sp->u_caster, 72757, false ); //similar effects will trigger cooldown
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_53476(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( 
//		sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_SCHOOL_ABSORB 
		sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_PROC_TRIGGER_SPELL 
		&& sp->u_caster )
	{
		//absorbing damage done against them from the next attack equal to $53476s2% of your pet's total health.
		value = sp->u_caster->GetMaxHealth() * value / 100;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_9512(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_ENERGIZE && target )
	{
		//Instantly restores (100 - 2 * max(0, level)) energy.
		int32 level = target->getLevel() - 40;
		value = 100 - 2 * MAX( 0, level );
		value = MAX( value, 10 );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_105723(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE && target )
	{
		float Radius = sp->GetRadius( i );
		float dist = sp->u_caster->GetDistance2dSq( target );
		dist = sqrt( dist ) + 0.1f;
		if( dist >= Radius )
			value = 0;
		else
		{
			float DMGScale = 1.0f - dist / Radius;
			value = value * DMGScale;
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_109045(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE && target )
	{
		value = value * target->GetMaxHealth() / 100;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_1953(Spell *sp,Unit *target, int32 &value, int32 i)
{
	//need to remove stun before the jump. Auras get applied after spell effects !
	if( i == 0 && target )
	{
		target->RemoveAllAurasByMechanicMulti( (1<<MECHANIC_ROOTED)|(1<<MECHANIC_STUNNED) , (uint32)(-1) , false ); //should not remove Gouge, Ice trap, sap, fear, Repentance, Ring of frost....
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_22842(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MOD_INCREASE_HEALTH && sp->u_caster )
	{
//		uint32 NewMaxHealth = sp->u_caster->GetMaxHealth() * ( 100 + value ) / 100;
//		uint32 MinHealthToHave = NewMaxHealth * value / 100;
		uint32 MinHealthToHave = sp->u_caster->GetMaxHealth() * 30 / 100;
		value = sp->u_caster->GetMaxHealth() * value / 100;
		//heals the player to at least 30% of max health and it should not get removed if aura gets removed
		if( sp->u_caster->GetHealth() < MinHealthToHave )
			sp->u_caster->SetHealth( MinHealthToHave - sp->u_caster->GetHealth() );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_22845(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_HEAL && target )
	{
		if( target->IsPlayer() == false || target->isAlive() == false )
			return SPELL_EFFECT_OVERRIDE_BREAK_EXECUTION;
		Player* mPlayer = SafePlayerCast( target );
		if( mPlayer->GetShapeShift() != FORM_BEAR )
			return SPELL_EFFECT_OVERRIDE_BREAK_EXECUTION;
		int32 val = mPlayer->GetPower( POWER_TYPE_RAGE );
		int32 consume_amt = 100;
		if( val >= consume_amt )
		{
			val -= consume_amt;
			mPlayer->SetPower( POWER_TYPE_RAGE, val );
			mPlayer->UpdatePowerAmm( true, POWER_TYPE_RAGE );
			uint32 maxhealth = mPlayer->GetUInt32Value( UNIT_FIELD_MAXHEALTH );
			float conversion_pct = consume_amt / 10.0f * 0.15f / 100.0f;	//1 point of rage into 0.15% hp
			value = float2int32( maxhealth * conversion_pct );
//			if( ProcedOnSpell )
//				mPlayer->Heal(mPlayer,ProcedOnSpell->Id,heal_amt);	
//			else
//				mPlayer->Heal(mPlayer,22845,heal_amt);
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_48743(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_HEAL && sp->u_caster )
	{
		value = sp->u_caster->GetUInt32Value(UNIT_FIELD_MAXHEALTH) * value / 100; 
		if( sp->p_caster && sp->p_caster->GetSummon() )
			sp->p_caster->GetSummon()->Dismiss();
		else if( target && target->IsCreature() && SafeCreatureCast( target )->m_spawn == NULL )
			SafeCreatureCast( target )->Despawn( 1, 0 );

	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_48707(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( target && i == 0)
	{
		//magic suppression
		int32 PCT = 50;
		if( target->SM_Mods )
			SM_FIValue(target->SM_Mods->SM_FAddEffect1, &PCT, sp->GetProto()->GetSpellGroupType());
		value = target->GetUInt32Value(UNIT_FIELD_MAXHEALTH)*PCT/100;	//value is in description
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_44457(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && sp->p_caster && target )
	{
		int64 *Index = sp->p_caster->GetCreateIn64Extension( EXTENSION_ID_LIVINGBOM_TARGET_COUNTER );
		uint64 *GUIDInSlot = (uint64 *)sp->p_caster->GetCreateIn64Extension( EXTENSION_ID_LIVINGBOM_TARGET_1 + *Index );
		Unit *old_target = sp->p_caster->GetMapMgr()->GetUnit( *GUIDInSlot );
		if( old_target != NULL && old_target != target )
			old_target->RemoveAura( sp->GetProto()->Id ); //remove from this target
		*GUIDInSlot = target->GetGUID();
		*Index = ( *Index + 1 ) % 3; //we can have max 3 targets with activ Living Bomb
	}

	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_20583(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[ i ].EffectApplyAuraName == SPELL_AURA_MOD_RESISTANCE_EXCLUSIVE && sp->u_caster)
	{
		value = sp->u_caster->getLevel();
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_1329(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[ i ].Effect == SPELL_EFFECT_DUMMYMELEE && target )
	{
		if(	target->IsPoisoned() )
			sp->forced_pct_mod_cur = sp->forced_pct_mod_cur * 120 / 100;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_13165(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[ i ].EffectApplyAuraName == SPELL_AURA_MOD_RANGED_ATTACK_POWER && target )
	{
		//only for 4.3.4 client. EEEEEk
		if( sp->GetProto()->eff[0].EffectBasePoints == 20 )
			value += 700;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
/*
SPELL_EFFECT_OVERRIDE_RETURNS EH_28730(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && target )
	{
		//i know the description says it should interrupt only PVE spell cast. But people insisted hard this should interrupt PVP also
		target->InterruptSpell();
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}*/

SPELL_EFFECT_OVERRIDE_RETURNS EH_54785(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && sp->u_caster )
	{
		float Orientation = sp->u_caster->GetOrientation();
		float Radius = 16.0f;
		float DestX = sp->u_caster->GetPositionX() + Radius * sin ( -Orientation + float(M_PI) / 2.0f );
		float DestY = sp->u_caster->GetPositionY() + Radius * cos ( -Orientation + float(M_PI) / 2.0f );
//sp->p_caster->BroadcastMessage("orientation now %f %f %f",Orientation, DestX, DestY );
		float landh = sp->u_caster->GetMapMgr()->GetLandHeight( DestX, DestY );
		if( abs( landh - sp->u_caster->GetPositionZ() ) < 6.0f )
		{
			//16 yards in front of us
			sp->m_targets.m_destX = DestX;
			sp->m_targets.m_destY = DestY;
			sp->m_targets.m_destZ = landh;
			sp->redirected_effect[ 2 ];	//make it a jump
		}
		else
			sp->redirected_effect[ 0 ]; //make it a knockback
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_676(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && target )
	{
		target->RemoveAuraByNameHash( SPELL_HASH_BLADESTORM, 0, AURA_SEARCH_POSITIVE );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_56815(Spell *sp,Unit *target, int32 &value, int32 i)
{
	//stupid idiot fucked up spell. Blood presence will deny the aurastate remove
	if( i == 0 && sp->u_caster )
	{
		if( sp->u_caster->HasAuraWithNameHash( SPELL_HASH_BLOOD_PRESENCE, 0, AURA_SEARCH_POSITIVE ) )
			sp->u_caster->SetAuraStateFlag( sp->GetProto()->RemoveCasterAuraState );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_1719(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && sp->p_caster )
	{
//		int32 RemainingCooldown = sp->p_caster->Cooldown_Getremaining( dbcSpell.LookupEntryForced( 20230 ) );
//		if( RemainingCooldown < 12000 )
//			sp->p_caster->ModCooldown( 20230, 12000 - RemainingCooldown, true );
		sp->p_caster->RemoveAura( 20230, 0, AURA_SEARCH_POSITIVE );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_20230(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && sp->p_caster )
	{
//		int32 RemainingCooldown = sp->p_caster->Cooldown_Getremaining( dbcSpell.LookupEntryForced( 1719 ) );
//		if( RemainingCooldown < 12000 )
//			sp->p_caster->ModCooldown( 1719, 12000 - RemainingCooldown, true );
		sp->p_caster->RemoveAura( 1719, 0, AURA_SEARCH_POSITIVE );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
} 

SPELL_EFFECT_OVERRIDE_RETURNS EH_48263(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( i == 0 && sp->p_caster )
	{
		sp->p_caster->SetPower( POWER_TYPE_RUNIC, 0 );
		sp->p_caster->UpdatePowerAmm( true, POWER_TYPE_RUNIC );
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_8349(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[ i ].Effect == SPELL_EFFECT_SCHOOL_DAMAGE && target )
	{
		//this spell has diminishing returns on DMG. After 6 hits the dmg is divided between targets
		uint32 TargetCount = (UINT32)( sp->m_targetUnits[i].size() );
		if( TargetCount > 6 )
		{
			uint32 DmgPct = 600 / TargetCount;
			sp->forced_pct_mod_cur = sp->forced_pct_mod_cur - 100 + DmgPct;
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}
/*
SPELL_EFFECT_OVERRIDE_RETURNS EH_32375(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[ i ].Effect == SPELL_EFFECT_DISPEL && target && isAttackable( sp->m_caster, target ) == false )
	{
		//Dispels magic in a 15 yard radius, removing all harmful spells from each friendly target and 1 beneficial spells from each enemy target.  Affects a maximum of 10 friendly targets and 10 enemy targets.
		value = MAX_AURAS;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}*/

SPELL_EFFECT_OVERRIDE_RETURNS EH_45477(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[ i ].Effect == SPELL_EFFECT_SCHOOL_DAMAGE && sp->u_caster )
	{
		//Chills the target for ${(($m1+$M1)/2)+($AP*0.2)} Frost damage 
		value += sp->u_caster->GetAP() * 20 / 100;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_108126(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[ i ].EffectApplyAuraName == SPELL_AURA_MOD_DMG_PENETRATE && sp->u_caster )
	{
		if( target->IsPlayer() )
			value = value / 2;
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_53508(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE && sp->u_caster )
	{
		// 216,"$damage=${$M1+(($RAP*0.40)*0.10)}",
		if( sp->u_caster->IsPet() && SafePetCast( sp->u_caster )->GetPetOwner() )
		{
			value = float2int32( ( value + SafePetCast( sp->u_caster )->GetPetOwner()->GetRAP() * 0.40f ) * 0.10f );	//owner will read spell description ...
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

SPELL_EFFECT_OVERRIDE_RETURNS EH_1680(Spell *sp,Unit *target, int32 &value, int32 i)
{
	if( sp->GetProto()->eff[i].Effect == SPELL_EFFECT_TRIGGER_SPELL && sp->p_caster )
	{
		if( sp->forced_basepoints[0] == 0
			&& sp->m_targetUnits[i].size() >= 4 )
		{
			sp->p_caster->ModCooldown( sp->GetProto()->Id, -sp->GetProto()->eff[2].EffectBasePoints * 1000, false );
			sp->forced_basepoints[0] = 1; //only mod it once
		}
	}
	return SPELL_EFFECT_OVERRIDE_CONTINUE_EXECUTION;
}

void RegisterEffectOverrides()
{
	SpellEntry *sp;
	//Whirlwind
	REGISTER_HANDLER( 1680 );	
	//Wolverine Bite
	REGISTER_HANDLER( 53508 );	
	//Colossus Smash
	REGISTER_HANDLER( 108126 );	
	//Icy Touch
	REGISTER_HANDLER( 45477 );	
	//Mass Dispel
//	REGISTER_HANDLER( 32375 );	
	//Fire Nova
	REGISTER_HANDLER( 8349 );	
	//Blood presence
	REGISTER_HANDLER( 48263 );	
	REGISTER_SAMEHANDLER( 48266, 48263 );	//frost presence
	REGISTER_SAMEHANDLER( 48265, 48263 );	//unholy presence
	//Retaliation
	REGISTER_HANDLER( 20230 );	
	//Recklessness
	REGISTER_HANDLER( 1719 );	
	//Rune Strike
	REGISTER_HANDLER( 56815 );	
	//Disarm
	REGISTER_HANDLER( 676 );	
//	REGISTER_SAMEHANDLER( 118093, 676 );
	REGISTER_SAMEHANDLER( 65545, 676 );	// Psychic Horror
	//Demon Leap
	REGISTER_HANDLER( 54785 );	
	//arcane torrent
//	REGISTER_HANDLER( 28730 );	
//	REGISTER_SAMEHANDLER( 50613, 28730 );
//	REGISTER_SAMEHANDLER( 80483, 28730 );
//	REGISTER_SAMEHANDLER( 25046, 28730 );
	//Restore Energy - Thistle Tea
	//Aspect of the Hawk
	REGISTER_HANDLER( 13165 );
	//Mutilate
	REGISTER_HANDLER( 1329 );
	//Nature Resistance racial
	REGISTER_HANDLER( 20583 ); 
	REGISTER_SAMEHANDLER( 20551, 20583 ); //Nature Resistance racial
	//living bomb
	REGISTER_HANDLER( 44457 );	
	//Anti-Magic Shell
	REGISTER_HANDLER( 48707 );	
	//Death pact
	REGISTER_HANDLER( 48743 );	
	//Frenzied Regeneration
	REGISTER_HANDLER( 22842 );	
	REGISTER_HANDLER( 22845 );	
	//Blink
	REGISTER_HANDLER( 1953 );	
	//CAUTERIZE - Alexstraza boss
	REGISTER_HANDLER( 109045 );	
	//Elementium Blast
	REGISTER_HANDLER( 105723 );	
	REGISTER_SAMEHANDLER( 109600, 105723 );
	REGISTER_SAMEHANDLER( 109601, 105723 );
	REGISTER_SAMEHANDLER( 109602, 105723 );
	//Restore Energy - Thistle Tea
	REGISTER_HANDLER( 9512 );	
	//Intervene - pet
	REGISTER_HANDLER( 53476 );	
	//racial - Will of the Forsaken
	REGISTER_HANDLER( 7744 );	
	//Faerie Fire 
	REGISTER_HANDLER( 770 );	
	//Replenishment - Soul Leech
	REGISTER_HANDLER( 57669 );	
	REGISTER_SAMEHANDLER( 34919, 57669 );//Vampiric Touch
	//Drain Soul
	REGISTER_HANDLER( 1120 );	
	//Legion Strike
	REGISTER_HANDLER( 30213 );	
	//Immolation Trap
//	REGISTER_HANDLER( 13797 );	
	//Master Shapeshifter
	REGISTER_HANDLER( 48411 );	
	//Vampiric Embrace
	REGISTER_HANDLER( 15290 );	
	//Mana Tide
	REGISTER_HANDLER( 16191 );	
	//soul link
	REGISTER_HANDLER( 25228 );	
	//Harmony - the DOT MOD
	REGISTER_HANDLER( 100977 );	
	//Time Warp
	REGISTER_HANDLER( 80353 );	
	//Ancient Hysteria
	REGISTER_HANDLER( 90355 );	
	//Firebolt - Imp
	REGISTER_HANDLER( 3110 );	
	//Shadow Bite
	REGISTER_HANDLER( 54049 );	
	//Watery Entrenchment - Hagara boss encounter
	REGISTER_HANDLER( 110317 );	
	//Void Diffusion - Zonozz boss encounter
	REGISTER_HANDLER( 103527 );	
	REGISTER_SAMEHANDLER( 106375, 103527 );	// ultraxion - Unstable Monstrosity 
	REGISTER_SAMEHANDLER( 106401, 103527 );	// Goriona - Twilight Onslaught 
	//Psychic Drain - Zonozz boss encounter
	REGISTER_HANDLER( 104322 );	
	REGISTER_SAMEHANDLER( 104608, 104322 );	
	//Dispel Magic
	REGISTER_HANDLER( 527 );	
	//Gouge
	REGISTER_HANDLER( 1776 );	
	//Colossus Smash
	REGISTER_HANDLER( 86346 );	
	//Vampiric Blood
	REGISTER_HANDLER( 55233 );	
	REGISTER_SAMEHANDLER( 53478, 55233 );		//Last Stand
	REGISTER_SAMEHANDLER( 53479, 55233 );		//Last Stand
	REGISTER_SAMEHANDLER( 12975, 55233 );		//Last Stand
	//Devour Magic Heal
	REGISTER_HANDLER( 19658 );	
	//Devour Magic
	REGISTER_HANDLER( 19505 );	
	//Anti-Magic Zone
	REGISTER_HANDLER( 50461 );	
	//Lifebloom
	REGISTER_HANDLER( 33763 );	
	//Starfall
	REGISTER_HANDLER( 48505 );	
	REGISTER_HANDLER( 50288 );	
	//Nurturing Instinct
	REGISTER_HANDLER( 33872 );	
	REGISTER_SAMEHANDLER( 33873, 33872 );	
	//Death Wish
//	REGISTER_HANDLER( 12292 );	
	//devastate
	REGISTER_HANDLER( 20243 );	
	//Glyph of Dispel Magic
	REGISTER_HANDLER( 56131 );	
	//Lifeblood
	REGISTER_HANDLER( 81708 );	
	REGISTER_SAMEHANDLER( 74497, 81708 );	
	REGISTER_SAMEHANDLER( 55428, 81708 );	
	REGISTER_SAMEHANDLER( 55480, 81708 );	
	REGISTER_SAMEHANDLER( 55500, 81708 );	
	REGISTER_SAMEHANDLER( 55501, 81708 );	
	REGISTER_SAMEHANDLER( 55502, 81708 );	
	REGISTER_SAMEHANDLER( 55503, 81708 );	
	//Divine Storm
	REGISTER_HANDLER( 53385 );	
	//Earthquake
//	REGISTER_HANDLER( 77478 );	
	//Windfury Attack
//	REGISTER_HANDLER( 33750 );	
	// Stomp - boss Morchok
	REGISTER_SAMEHANDLER( 109034, 88942 );	// split dmg between targets
	REGISTER_SAMEHANDLER( 103414, 88942 );	// split dmg between targets
	// a few spells that should force BG flag drop
	REGISTER_HANDLER( 1784 );	// stealth
	REGISTER_SAMEHANDLER( 5215, 1784 ); // prowl
	REGISTER_SAMEHANDLER( 498, 1784 ); // Divine protection
	REGISTER_SAMEHANDLER( 642, 1784 ); // Divine shield
	REGISTER_SAMEHANDLER( 1022, 1784 ); // Hand of Protection
//	REGISTER_SAMEHANDLER( 1856, 1784 ); // Vanish
	REGISTER_SAMEHANDLER( 45438, 1784 ); // Ice block
	REGISTER_SAMEHANDLER( 58984, 1784 ); // Shadowmeld
	REGISTER_SAMEHANDLER( 17624, 1784 ); // Petrification
	REGISTER_SAMEHANDLER( 66, 1784 ); // Invisibility
	REGISTER_SAMEHANDLER( 55004, 1784 ); // Nitro Boosts
	REGISTER_SAMEHANDLER( 5384, 1784 ); // Feign Death
	// Arathi Basin opening spell, remove stealth, invisibility, etc.
	REGISTER_HANDLER( 21651 );
	// WSG and The eye - pickup flag
	REGISTER_SAMEHANDLER( 23333, 21651 ); 
	REGISTER_SAMEHANDLER( 23335, 21651 ); 
	REGISTER_SAMEHANDLER( 34976, 21651 ); 
	//SOTA - Transporter platforms
	REGISTER_HANDLER( 54640 );
	//Victory Rush
	REGISTER_HANDLER( 34428 );
	//Slam
	REGISTER_HANDLER( 1464 );
	//Drain Life - for Item - Dragon Soul - Proc - Str Tank Sword Heroic
	REGISTER_HANDLER( 109831 );
	REGISTER_SAMEHANDLER( 109828, 109831 ); //Item - Dragon Soul - Proc - Str Tank Sword LFR
	REGISTER_SAMEHANDLER( 108022, 109831 ); //Item - Dragon Soul - Proc - Str Tank Sword
	//Festering Strike
	REGISTER_HANDLER( 85948 );
	//Whiplash
	REGISTER_HANDLER( 6360 );
	//Frostfire Bolt
	REGISTER_HANDLER( 44614 );
	//Free Action Potion
	REGISTER_HANDLER( 6615 );
	//Blackwhelp Net
	REGISTER_HANDLER( 38177 );
	//Demon Armor
	REGISTER_HANDLER( 687 );
	REGISTER_SAMEHANDLER( 28176, 687 ); //Fel Armor
	//Scourge Strike
	REGISTER_HANDLER( 55090 );
	//Heartfire
	REGISTER_HANDLER( 99007 );
	REGISTER_SAMEHANDLER( 99069, 99007 ); //Fires of Heaven
	REGISTER_SAMEHANDLER( 99189, 99007 ); //Flametide
	REGISTER_SAMEHANDLER( 99131, 99007 ); //Divine Fire
	//Death Strike
	REGISTER_HANDLER( 49998 );
	//Meltdown - PVE boss
	REGISTER_HANDLER( 98649 );
	//Decimation Blade - Baleroc PVE boss
	REGISTER_HANDLER( 99353 );
	//Meteor Slash - Argaloth PVE boss
	REGISTER_HANDLER( 88942 );
	REGISTER_SAMEHANDLER( 98474, 88942 ); //Flame Scythe
	//Death Coil - warlock
	REGISTER_HANDLER( 6789 );
	//Raid Group Markers
	REGISTER_HANDLER( 84996 );
	REGISTER_SAMEHANDLER( 84997, 84996 );
	REGISTER_SAMEHANDLER( 84998, 84996 );
	REGISTER_SAMEHANDLER( 84999, 84996 );
	REGISTER_SAMEHANDLER( 85000, 84996 );
	//Light of Dawn
	REGISTER_HANDLER( 85222 );
	//Death and Decay
	REGISTER_HANDLER( 43265 );
	//Necrotic Strike
	REGISTER_HANDLER( 73975 );
	//Dark transformation
//	REGISTER_HANDLER( 63560 );
	//Combustion	- the dot part
	REGISTER_HANDLER( 83853 );
	//Shadow Ward
	REGISTER_HANDLER( 6229 );
	//Zealotry
	REGISTER_HANDLER( 85696 );
	//Healthstone
	REGISTER_HANDLER( 6262 );
	//Aimed Shot
	REGISTER_HANDLER( 19434 );
	REGISTER_SAMEHANDLER( 82928, 19434 );
	//Haunt
	REGISTER_HANDLER( 48181 );
	//Nether Ward
	REGISTER_HANDLER( 91711 );
	//frost fever
	REGISTER_HANDLER( 55095 );
	REGISTER_SAMEHANDLER( 55078, 55095 );	//blood plague
	//Innervate
	REGISTER_HANDLER( 29166 );
	//Nourish
	REGISTER_HANDLER( 50464 );
	//Hemorrhage
	REGISTER_HANDLER( 16511 );
	//Pulverize
	REGISTER_HANDLER( 80313 );
	//Frenzied regeneration
//	REGISTER_HANDLER( 22842 );
	//Spirit Mend
	REGISTER_HANDLER( 90361 );
	//Kick
	REGISTER_HANDLER( 1766 );
	//Seal of Righteousness
	REGISTER_HANDLER( 25742 );
	//Judgements of the Bold
	REGISTER_HANDLER( 89906 );
	//flametongue weapon dmg proc
	REGISTER_HANDLER( 10444 );
	//Bear Form
	REGISTER_HANDLER( 5487 );
	//Flask of Enhancement
	REGISTER_HANDLER( 79637 );
	//Venomous Wounds
	REGISTER_HANDLER( 79136 );
	//deep freeze
	REGISTER_HANDLER( 44572 );
	//primal wisdom
	REGISTER_HANDLER( 63375 );
	//Lash of pain
	REGISTER_HANDLER( 7814 );
	//Focus Fire
	REGISTER_HANDLER( 82692 );
	//Unleash Elements
	REGISTER_HANDLER( 73680 );
	//Judgements of the Wise
	REGISTER_HANDLER( 31930 );
	//Consecration - for 406 client 
//	REGISTER_HANDLER( 82366 );
	//Ice Barrier
	REGISTER_HANDLER( 11426 );
	//Mana Shield
	REGISTER_HANDLER( 1463 );
	//Mage Ward
	REGISTER_HANDLER( 543 );
	//Chain Heal - riptide consume
	REGISTER_HANDLER( 1064 );
	//Redirect
	REGISTER_HANDLER( 73981 );
	//Lava Lash
	REGISTER_HANDLER( 60103 );
	//rogue - expose armor - improved expose armor
	REGISTER_HANDLER( 8647 );
	//rogue - vanish
	REGISTER_HANDLER( 1856 );
	//druid - swiftmend
	REGISTER_HANDLER( 18562 );
	//Blind Spot
	REGISTER_HANDLER( 91322 );
	//Claw - pet 
	REGISTER_HANDLER( 16827 );
	REGISTER_SAMEHANDLER( 17253, 16827 ); //Bite - pet 
	REGISTER_SAMEHANDLER( 49966, 16827 ); //Smack - pet 
	//Kill Command
	REGISTER_HANDLER( 83381 );
	//Holy Wrath
//	REGISTER_HANDLER( 2812 );
	//Ferocious Bite
	REGISTER_HANDLER( 22568 );
	//Penance
//	REGISTER_HANDLER( 47540 );
	//Ambush
	REGISTER_HANDLER( 8676 );
	//Death Coil
	REGISTER_HANDLER( 47541 );
	//Shield Block
	REGISTER_HANDLER( 2565 );
	//execute
	REGISTER_HANDLER( 5308 );
	//Howling Blast
	REGISTER_HANDLER( 49184 );
	//Savage Defense
//	REGISTER_HANDLER( 62606 );	- already handled by handleproc !
	//Mind Spike
	REGISTER_HANDLER( 73510 );
	//Mind Blast
	REGISTER_HANDLER( 8092 );
	//Shred
	REGISTER_HANDLER( 5221 );
	//Maim
	REGISTER_HANDLER( 22570 );
	//Maul
	REGISTER_HANDLER( 6807 );
	//Faerie Fire (Feral)
	REGISTER_HANDLER( 16857 );
	//Swipe(bear form)
	REGISTER_HANDLER( 779 );
	//Savage Roar - maybe it is not required to mod the duration here
	REGISTER_HANDLER( 52610 );
	//Thrash
	REGISTER_HANDLER( 77758 );
	//Rip
	REGISTER_HANDLER( 1079 );
	//Rake
	REGISTER_HANDLER( 1822 );
	//Heroic Leap
	REGISTER_HANDLER( 52174 );
	//Heroic Strike
	REGISTER_HANDLER( 78 );
	//mangle
	REGISTER_HANDLER( 33878 );
	REGISTER_SAMEHANDLER( 33876, 33878 );
	//slice and dice
	REGISTER_HANDLER( 5171 );
	//word of glory
	REGISTER_HANDLER( 85673 );
	//Inquisition
	REGISTER_HANDLER( 84963 );
	//Shield of the Righteous
	REGISTER_HANDLER( 53600 );
	//Templar's Verdict
	REGISTER_HANDLER( 85256 );
	//searing bolt - searing totem - chained cast spell for talent Searing Flames
	REGISTER_HANDLER( 3606 );	
	//Mark of the Wild - chained cast spell
	REGISTER_HANDLER( 79060 );	
	REGISTER_SAMEHANDLER( 79062, 79060 );//Blessing of Kings
	REGISTER_SAMEHANDLER( 90363, 79060 );//Embrace of the Shale Spider
	//Thorns
	REGISTER_HANDLER( 467 );	
	//Holy Nova - the heal proc
	REGISTER_HANDLER( 23455 );	
	//Prayer of Mending
	REGISTER_HANDLER( 41637 );	//the proc not the cast spell
	//Shadow Word: Death
	REGISTER_HANDLER( 32379 );
	//Consecration
	REGISTER_HANDLER( 36946 );	
	//Cleave
	REGISTER_HANDLER( 845 );
	//Aspect of the Wild
	REGISTER_HANDLER( 20043 );
	//Chimera shot
	REGISTER_HANDLER( 53209 );
	//Steady shot
	REGISTER_HANDLER( 56641 );
	//Cobra shot
	REGISTER_HANDLER( 77767 );
	//Seal of Righteousness
//	REGISTER_HANDLER( 31801 );
	//Seal of Righteousness
//	REGISTER_HANDLER( 20154 );
	//Seal of Justice
	REGISTER_HANDLER( 20170 );
	//Seal of Insight
	REGISTER_HANDLER( 20167 );
	//Censure (Seal of truth)
	REGISTER_HANDLER( 31803 );
	//Demonic Circle: Teleport
	REGISTER_HANDLER( 48020 );
	//Fel Flame
	REGISTER_HANDLER( 77799 );
	//Lacerate
	REGISTER_HANDLER( 33745 );
	//Resistance Aura
	REGISTER_HANDLER( 19891 );
	//Shadow Protection
	REGISTER_SAMEHANDLER( 79106, 19891 );
	//Elemental Resistance Totem
	REGISTER_SAMEHANDLER( 8185, 19891 );
	//Thunder Clap
	REGISTER_HANDLER( 6343 );
	//Intercept
	REGISTER_HANDLER( 20253 );
	//Heroic Throw
	REGISTER_HANDLER( 57755 );
	//Shield Slam
	REGISTER_HANDLER( 23922 );
	REGISTER_SAMEHANDLER( 23885, 23922 );
	//Bloodthirst
	REGISTER_HANDLER( 23881 );
	//gore - Your boar gores the enemy for $35290s1.  Causes double damage if used within 6 sec of a Charge.
	REGISTER_HANDLER( 35290 );
	//Incinerate
	REGISTER_HANDLER( 29722 );
	//Pulsing Shockwave
	REGISTER_HANDLER( 52942 );
	REGISTER_SAMEHANDLER( 59837, 52942 );
	//Sonic Screech, Auriaya encounter
	REGISTER_HANDLER( 64422 );
	REGISTER_SAMEHANDLER( 64688, 64422 );
	//Meteor Slash
	REGISTER_SAMEHANDLER( 45150, 64422 );
}