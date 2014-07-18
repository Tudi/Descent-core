/*
* arcemu MMORPG Server
* Copyright (C) 2005-2007 arcemu Team <http://www.arcemuemu.com/>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/
// Coded by Captnoord for arcemu (2007), please give feedback to me if your able to.

#include "StdAfx.h"

/// Function pointer holder
pSpellTarget SpellTargetHandler[EFF_TARGET_LIST_LENGTH_MARKER] = 
{
	&Spell::SpellTargetDefault,				 // 0
	&Spell::SpellTargetSelf,					// 1
	&Spell::SpellTargetNULL,					// 2 Not handled (Test spells)
	&Spell::SpellTargetInvisibleAOE,			// 3
	&Spell::SpellTargetFriendly,				// 4
	&Spell::SpellTargetPet,					 // 5
	&Spell::SpellTargetSingleTargetEnemy,	   // 6
	&Spell::SpellTargetNULL,					// 7 - this should not have a default targetting function
	&Spell::SpellTargetCustomAreaOfEffect,	  // 8
	&Spell::SpellTargetNULL,					// 9 Not handled (no spells)
	&Spell::SpellTargetNULL,					// 10 Not handled (no spells)
	&Spell::SpellTargetNULL,					// 11 Not handled (Test spells)
	&Spell::SpellTargetNULL,					// 12 Not handled (no spells)
	&Spell::SpellTargetNULL,					// 13 Not handled (no spells)
	&Spell::SpellTargetNULL,					// 14 Not handled (no spells)
	&Spell::SpellTargetAreaOfEffect,			// 15
	&Spell::SpellTargetAreaOfEffect,			// 16
	&Spell::SpellTargetNULL,					// 17 Target is a location
	&Spell::SpellTargetLandUnderCaster,		 // 18
	&Spell::SpellTargetNULL,					// 19 Not handled (Test spells)
	&Spell::SpellTargetAllPartyMembersRangeNR,  // 20
	&Spell::SpellTargetSingleTargetFriend,	  // 21
	&Spell::SpellTargetAoE,					 // 22
	&Spell::SpellTargetSingleGameobjectTarget,  // 23
	&Spell::SpellTargetInFrontOfCaster,		 // 24
	&Spell::SpellTargetSingleFriend,			// 25
	&Spell::SpellTargetGameobject_itemTarget,   // 26
	&Spell::SpellTargetPetOwner,				// 27
	&Spell::SpellTargetEnemysAreaOfEffect,	  // 28 channeled
	&Spell::SpellTargetTypeTAOE,				// 29
	&Spell::SpellTargetAllyBasedAreaEffect,	// 30
	&Spell::SpellTargetScriptedEffects,		 // 31
	&Spell::SpellTargetSummon,				  // 32
	&Spell::SpellTargetNearbyPartyMembers,	  // 33
	&Spell::SpellTargetAllPartyMembersRangeNR,	// 34 Not handled (No spells)
	&Spell::SpellTargetSingleTargetPartyMember, // 35
	&Spell::SpellTargetScriptedEffects2,		// 36
	&Spell::SpellTargetPartyMember,			 // 37
	&Spell::SpellTargetDummyTarget,			 // 38
	&Spell::SpellTargetFishing,				 // 39
	&Spell::SpellTargetType40,				  // 40
	&Spell::SpellTargetTotem,				   // 41
	&Spell::SpellTargetTotem,				   // 42
	&Spell::SpellTargetTotem,				   // 43
	&Spell::SpellTargetTotem,				   // 44
	&Spell::SpellTargetChainTargeting,		  // 45
	&Spell::SpellTargetSimpleTargetAdd,		 // 46
	&Spell::SpellTargetSimpleTargetAdd,		 // 47
	&Spell::SpellTargetSimpleTargetAdd,		 // 48
	&Spell::SpellTargetSimpleTargetAdd,		 // 49
	&Spell::SpellTargetSimpleTargetAdd,		 // 50
	&Spell::SpellTargetNULL,					// 51
	&Spell::SpellTargetSimpleTargetAdd,		 // 52
	&Spell::SpellTargetTargetAreaSelectedUnit,  // 53
	&Spell::SpellTargetInFrontOfCaster2,		// 54
	&Spell::SpellTargetNULL,					// 55 Not handled (Not realy handled by the current spell system)
	&Spell::SpellTargetAllRaid,		  			// 56
	&Spell::SpellTargetTargetPartyMember,	   // 57
	&Spell::SpellTargetNULL,					// 58
	&Spell::SpellTargetNULL,					// 59
	&Spell::SpellTargetNULL,					// 60 // scripted target fill..
	&Spell::SpellTargetRaidSameClass,	  // 61
	&Spell::SpellTargetNULL,					// 62 // targets the priest champion, big range..
	&Spell::SpellTargetSimpleTargetAdd,		 // 63 // summon [] creature
	&Spell::SpellTargetSimpleTargetAdd,		 // 64 // summon [] creature
	&Spell::SpellTargetSimpleTargetAdd,		 // 65 // summon [] creature
	&Spell::SpellTargetSimpleTargetAdd,		 // 66 // summon [] creature
	&Spell::SpellTargetSimpleTargetAdd,		 // 67 // summon [] creature
	&Spell::SpellTargetNULL,					// 68 Not handled (No spells)
	&Spell::SpellTargetTotem,				   // 69 // Spitfire totem
	&Spell::SpellTargetNULL,					// 70 Not handled (No spells)
	&Spell::SpellTargetNULL,					// 71 Not handled (No spells)
	&Spell::SpellTargetSimpleTargetAdd,		 // 72 // summon [] creature + trow items
	&Spell::SpellTargetSummon,				  // 73
	&Spell::SpellTargetEnemysAreaOfEffect,		// 74	- !not tested
	&Spell::SpellTargetNULL,					// 75
	&Spell::SpellTargetEnemysAreaOfEffect,		// 76	- !not tested
	&Spell::SpellTargetSingleTargetEnemy,	   // 77
	&Spell::SpellTargetNULL,					// 78
	&Spell::SpellTargetNULL,					// 79
	&Spell::SpellTargetNULL,					// 80
	&Spell::SpellTargetNULL,					// 81
	&Spell::SpellTargetNULL,					// 82
	&Spell::SpellTargetNULL,					// 83
	&Spell::SpellTargetNULL,					// 84
	&Spell::SpellTargetNULL,					// 85
	&Spell::SpellTargetFriendly,				// 86 - summon a blood magot
	&Spell::SpellTargetEnemysAreaOfEffect,		// 87 - more like location. Just filling packet
	&Spell::SpellTargetEnemysAreaOfEffect,		// 88 - !not sure
	&Spell::SpellTargetEnemysAreaOfEffect,		// 89 - !not sure
	&Spell::SpellTargetCritterPet,				// 90
	&Spell::SpellTargetNULL,					// 91
	&Spell::SpellTargetNULL,					// 92
	&Spell::SpellTargetNULL,					// 93
	&Spell::SpellTargetEnemysAreaOfEffect,		// 94
	&Spell::SpellTargetNULL,					// 95
	&Spell::SpellTargetNULL,					// 96
	&Spell::SpellTargetNULL,					// 97
	&Spell::SpellTargetRaidInrangeAtTarget,		// 98 
	&Spell::SpellTargetNULL,					// 99
	&Spell::SpellTargetMultiplePartyInjured,	// 100
	&Spell::SpellTargetMultipleRaidInjured,		// 101
	&Spell::SpellTargetRaidInrangeAtTarget,		// 102
	&Spell::SpellTargetNULL,					// 103
	&Spell::SpellTargetInFrontOfCaster,			// 104	-> can be location too ?
	&Spell::SpellTargetSinglePartyInjured,		// 105 -> !!!custom !!!
	&Spell::SpellTargetInFrontOfCasterFriendly,	// 106	EFF_TARGET_CUSTOM_IN_FRONT_OF_CASTER_FRIENDLY
	// all 81 > n spelltargettype's are from test spells
};

/* LEFTOVER RESEARCH, this contains note's comments from Captnoord
// these are left here because of the importance's of the info.
// comments and additional information related to this send to Captnoord

// Type 4:
// 4 is related to Diseases fun to give it a try..
// dono related to "Wandering Plague", "Spirit Steal", "Contagion of Rot", "Retching Plague" and "Copy of Wandering Plague"

// Type 7:
// Point Blank Area of Effect
// think its wrong, related to 2 spells, "Firegut Fear Storm" and "Mind Probe"
// FillAllTargetsInArea(tmpMap,m_targets.m_destX,m_targets.m_destY,m_targets.m_destZ,GetRadius(i));

//fear storm is nice
//Score 5.7	 Vote: [-] [+] by plle, 1.5 years ago
//when you set that ogre head on the top of the rock and all orges see it they /yell: FLEE (and) RUN!!
//and then all ogres get fear storm =D 

//this quest
//Score 6.9	 Vote: [-] [+] by ewendim, 1.1 years ago
//happens while doing this quest http://www.thottbot.com/?qu=3825

// Type 11
// this is related to spellID: 4, as I think is a gm spell

// 11 select id,name, EffectImplicitTargetA1 from spell where EffectImplicitTargetA1 = 11;
// +----+----------------------+------------------------+
// | id | name				 | EffectImplicitTargetA1 |
// +----+----------------------+------------------------+
// |  4 | Word of Recall Other |					 11 |
// +----+----------------------+------------------------+

// type 17:
// spells like 17278:Cannon Fire and 21117:Summon Son of Flame A
// A single target at a xyz location or the target is a possition xyz

// select id,name, EffectImplicitTargetA1 from spell where EffectImplicitTargetB1 = 9;
// +-------+----------------------------+------------------------+
// | id	| name					   | EffectImplicitTargetA1 |
// +-------+----------------------------+------------------------+
// |	 1 | Word of Recall (OLD)	   |					  1 |
// |	 3 | Word of Mass Recall (OLD)  |					 20 |
// |   556 | Astral Recall			  |					  1 |
// |  8690 | Hearthstone				|					  1 |
// | 39937 | There's No Place Like Home |					  1 |
// +-------+----------------------------+------------------------+

// type 10: is not used
// type 12: is not used
// type 13: is not used
// type 14: is not used

// type 19: is target zone I think (test spells)

// type 48: is summon wild unit
// type 49: is summon friend unit
// type 50: something related to the pref 2
// type 51: is targeting objects / egg's / possible to use those while flying


// type 55 related to blink and Netherstep... I think this one sets the xyz where you should end...
// type 56 is related to aura holder... Player 1 give's me a aura and that aura has as target me. I wear then the aura / spell and it targeting me

// type 58 proc triggeret target... 
// Apply Aura: Proc Trigger Spell
// Retching Plague
// 10% chance.

// type 59 related to 2 visual spells
// type 60 1 target related. rest is unknown or not clear

// type 62 targets the best player of [class]
// type 63 targets chess player... something like that
// type 64 something related to wharp storm... and the wharpstorm creature..
// type 65 target enemy's weapon... Item target..... "A" target type, "B" target type is target enemy unit...
// type 66 related to summon some unit...
// type 67 related to summon some unit...
// type 68 is not used
// type 69 is totem stuff level 70 totem stuff
// type 70 is not used
// type 71 is not used
// type 72 target enemy unit... (I think it does not matter if its a friendly unit)
// type 73 is serverside scripted.... Mostly summons
// type 74 Target Random Enemy
// type 75 Target location in front of the caster
// type 76 target a area... of target...
// type 77 target single enemy
// type 78 units in front of caster ( test spell )
// type 79 is not used
// type 80 related to summon some unit
// type 81 > N are not handled because they are test spells 

*/

/// Fill the target map with the targets
/// the targets are specified with numbers and handled accordingly
void Spell::FillTargetMap(uint32 i)
{
	uint32 cur;
	
	if( GetProto()->TargettingOverride[i] )
	{
		GetProto()->TargettingOverride[i]( this, i );
		return;
	}

	uint32 TypeA = m_spellInfo->eff[i].EffectImplicitTargetA;
	uint32 TypeB = m_spellInfo->eff[i].EffectImplicitTargetB;

//printf("effect %u and target 1 %u target 2 %u\n",i,TypeA,TypeB);
	// if all secondary targets are 0 then use only primary targets
	if(!TypeB)
	{
		if( TypeA < EFF_TARGET_LIST_LENGTH_MARKER )
			(this->*SpellTargetHandler[TypeA])(i, 0);		//0=A

		return;
	}

	// if all primary targets are 0 then use only secondary targets
	if(!TypeA)
	{
		if( TypeB < EFF_TARGET_LIST_LENGTH_MARKER )
			(this->*SpellTargetHandler[TypeB])(i, 1);		//1=B

		return;
	}

	// j = 0
	cur = m_spellInfo->eff[i].EffectImplicitTargetA;
	if (cur < EFF_TARGET_LIST_LENGTH_MARKER)
	{
		(this->*SpellTargetHandler[cur])(i,0);	//0=A
	}

	// j = 1
	cur = m_spellInfo->eff[i].EffectImplicitTargetB;
	if (cur < EFF_TARGET_LIST_LENGTH_MARKER)
	{
		(this->*SpellTargetHandler[cur])(i,1);	//1=B
	}
}

//order target vect in order for chain lightning animation to look somewhat good
//!! this is bad as charge effect diminishes with each jump. We should gather targets without reordering
/*
void OrderChainTargetVect( Object *Caster, std::vector<uint64> *UniqueTargets )
{
	if( Caster == NULL || Caster->GetMapMgr() == NULL )
		return;
	std::vector<uint64> ListBackup = *UniqueTargets;
	UniqueTargets->clear();
	float	ClosestDistance;
	Object	*ClosestTarget,*ClosestPrevTarget = Caster;
	uint32	TargetCount = ListBackup.size();
	for( uint32 j=0;j<TargetCount;j++ )
	{
		ClosestDistance = 600.0f * 600.0f ;
		int32 Index = -1;
		for( uint32 i=0;i<TargetCount;i++ )
		{
			if( ListBackup[i] )
			{
				Object *curTarget = Caster->GetMapMgr()->GetUnit( ListBackup[i] );
				float curDist = curTarget->GetDistance2dSq( ClosestPrevTarget );
				if( curDist < ClosestDistance )
				{
					ClosestDistance = curDist;
					ClosestTarget = curTarget;
					Index = i;
				}
			}
		}
		if( Index > -1 )
		{
			ClosestPrevTarget = ClosestTarget;
			UniqueTargets->push_back( ClosestTarget->GetGUID() );
			ListBackup[ Index ] = 0;
		}
	}
}
*/

//#define I_AM_STUPID_BUT_I_JUST_WANT_TO_TRY_THIS
void Spell::SpellTargetNULL(uint32 i, uint32 j)
{
	sLog.outDebug("[SPELL][TARGET] Unhandled target typeA: %u typeB: %u", m_spellInfo->eff[j].EffectImplicitTargetA, m_spellInfo->eff[j].EffectImplicitTargetB);

	#ifdef I_AM_STUPID_BUT_I_JUST_WANT_TO_TRY_THIS
		TargetsList *tmpMap=&m_targetUnits[i];
		SafeAddTarget(tmpMap,m_targets.m_unitTarget);
	#endif
}
//#undef I_AM_STUPID_BUT_I_JUST_WANT_TO_TRY_THIS

/// Spell Target Handling for type 0: Default targeting
void Spell::SpellTargetDefault(uint32 i, uint32 j)
{
	if(j==0 || (m_caster->IsPet() && j==1))
	{
		TargetsList *tmpMap=&m_targetUnits[i];

		if(m_targets.m_unitTarget)
			SafeAddTarget(tmpMap,m_targets.m_unitTarget);
		else if(m_targets.m_itemTarget)
			SafeAddTarget(tmpMap,m_targets.m_itemTarget);
		else if( (m_spellInfo->eff[i].Effect == SPELL_EFFECT_ADD_FARSIGHT
			|| m_spellInfo->eff[i].Effect == SPELL_EFFECT_SUMMON_DEMON
			|| m_spellInfo->eff[i].Effect == SPELL_EFFECT_APPLY_GLYPH) 
			)
			SafeAddTarget(tmpMap,m_caster->GetGUID());
	}
}

/// Spell Target Handling for type 1: Self Target + in moon skin form party member in radius
void Spell::SpellTargetSelf(uint32 i, uint32 j)
{
	TargetsList *tmpMap=&m_targetUnits[i];
	//if(p_caster)
	//{
	//	if(m_spellInfo->RequiredShapeShift && (p_caster->getClass()==DRUID || p_caster->getClass()==WARRIOR))
	//	{
	//		if(m_spellInfo->eff[i].Effect != SPELL_EFFECT_LEARN_SPELL)//in talents
	//		{	
	//			if(!p_caster->GetShapeShift())
	//				return;
	//			if(!((((uint32)1)<< (p_caster->GetShapeShift()-1)) & m_spellInfo->RequiredShapeShift))
	//				return;
	//		}
	//	}
	//}
	SafeAddTarget(tmpMap,m_caster->GetGUID());
}
void Spell::SpellTargetInvisibleAOE(uint32 i, uint32 j)
{
	FillSpecifiedTargetsInArea(i,m_targets.m_destX,m_targets.m_destY,m_targets.m_destZ,GetRadius(i),1); //TARGET_SPEC_INVISIBLE);
}

/// Spell Target Handling for type 4: Target is holder of the aura
void Spell::SpellTargetFriendly(uint32 i, uint32 j)
{
	// O fuck we are contagious...
	// this off course is not tested yet. 
	if (p_caster)
	{
		TargetsList *tmpMap=&m_targetUnits[i];
		SafeAddTarget(tmpMap,p_caster->GetGUID());
	}
}

/// Spell Target Handling for type 5: Target: Pet
void Spell::SpellTargetPet(uint32 i, uint32 j)
{
	if(p_caster)
	{
		TargetsList *tmpMap=&m_targetUnits[i];
		if(p_caster->GetSummon())
			SafeAddTarget(tmpMap,p_caster->GetSummon()->GetGUID());
	}
	//pet selfcast ? i wonder if this needs to target always our pet instead of self
	else if( u_caster && u_caster->IsPet() )
		SafeAddTarget(&m_targetUnits[i],u_caster->GetGUID());
}

/// Spell Target Handling for type 6 and 77: Single Target Enemy (grep thinks 77 fits in 6)
void Spell::SpellTargetSingleTargetEnemy(uint32 i, uint32 j)
{
	if(!m_caster->IsInWorld())
		return;

	Unit * pTarget = m_caster->GetMapMgr()->GetUnit(m_targets.m_unitTarget);

	if( p_caster && p_caster == pTarget ) // spell bug: target is supposed to be a single enemy, not self
	{
		//!!! can bug out when a proc spell PROC_ON_CAST_SPELL will target caster and has no range !
		pTarget = p_caster->GetMapMgr()->GetUnit( p_caster->GetSelection() );
	}
	else if( u_caster )
	{
		Unit *owner = u_caster->GetTopOwner();
		if( pTarget == owner )
			pTarget = u_caster->GetAIInterface()->GetMostHated();
	}

	if( pTarget == m_caster )
		return;

	if( !pTarget )
		return;

	TargetsList *tmpMap=&m_targetUnits[i];

	//do not add targets that cannot be targeted due to invalid target type
	if( !TargetTypeCheck( pTarget, GetTargetCreatureType( m_spellInfo ) ) )
		return;

	if( p_caster )
	{
		// this is mostly used for things like duels
		if(
//			pTarget->IsPlayer() && 
			!isAttackable(p_caster, pTarget, !( GetProto()->c_is_flags & SPELL_FLAG_IS_TARGETINGSTEALTHED ) ))
		{
//			cancastresult = SPELL_FAILED_BAD_TARGETS; //Zack: maybe other effects can still work. Ex : priest heal / dmg spell
			return;
		}
	}

	//m_magnetcaster - magnettarget to redirect spell casts
	pTarget = pTarget->CheckSpellTargetRedirect( this );

	//number of additional targets
	float range = GetMaxRange( dbcSpellRange.LookupEntry( m_spellInfo->rangeIndex ) );//this is probably wrong
	if( Need_SM_Apply(GetProto()) && u_caster != NULL && u_caster->SM_Mods )
	{
		SM_FFValue( u_caster->SM_Mods->SM_FRange, &range, GetProto()->GetSpellGroupType() );
		SM_PFValue( u_caster->SM_Mods->SM_PRange, &range, GetProto()->GetSpellGroupType() );
#ifdef COLLECTION_OF_UNTESTED_STUFF_AND_TESTERS
		float spell_flat_modifers=0;
		float spell_pct_modifers=0;
		SM_FFValue(u_caster->SM_FRange,&spell_flat_modifers,GetProto()->GetSpellGroupType());
		SM_FFValue(u_caster->SM_PRange,&spell_pct_modifers,GetProto()->GetSpellGroupType());
		if(spell_flat_modifers!=0 || spell_pct_modifers!=0)
			printf("!!!!!spell range bonus mod flat %f , spell range bonus pct %f , spell range %f, spell group %u\n",spell_flat_modifers,spell_pct_modifers,maxRange,GetProto()->GetSpellGroupType());
#endif
	}
	range += 1.5f; //this is the bounding radious of a player and maybe we should consider one for enemy also ?
	range *= range;

	uint8 did_hit_result = DidHit(i,pTarget);
	if(did_hit_result != SPELL_DID_HIT_SUCCESS)
		SafeAddModeratedTarget(pTarget->GetGUID(), did_hit_result);
	else if( pSpellId == 0 || range >= m_caster->GetDistanceSq( pTarget ) )	//range check is required for triggered spells
		SafeAddTarget(tmpMap, pTarget->GetGUID());

	int32 jumps = m_spellInfo->eff[i].EffectChainTarget;

	// extra targets by auras
	if( u_caster && u_caster->SM_Mods )
	{
		SM_FIValue( u_caster->SM_Mods->SM_FAdditionalTargets,(int32*)&jumps, m_spellInfo->GetSpellGroupType() );
	}

	if( jumps > 1 )
	{
		jumps = jumps - 1;
		jumps = MIN( MAX_SPELL_TARGET_COUNT, jumps );
		Unit *PrevTarget = pTarget;
		for( ;jumps>0;jumps-- )
		{
			if( PrevTarget == NULL )
				break;

			Unit *BestTarget = NULL;
			float BestDistance = 99999999.0f;

			//get closest target to the previous target
			InRangeSetRecProt::iterator itr,itr2;
			m_caster->AquireInrangeLock(); //make sure to release lock before exit function !
			InrangeLoopExitAutoCallback AutoLock;
			for( itr2 = m_caster->GetInRangeSetBegin( AutoLock ); itr2 != m_caster->GetInRangeSetEnd(); )
			{
				itr = itr2;
				itr2++;
				if((*itr)->GetGUID()==m_targets.m_unitTarget)
					continue;
				if( !((*itr)->IsUnit()) || !SafeUnitCast((*itr))->isAlive() 
	//				|| 
	//				((*itr)->IsCreature() && ((Creature*)(*itr))->IsTotem())
					)
					continue;

				float Dist = Distance2DSq( PrevTarget->GetPositionX(), PrevTarget->GetPositionY(), (*itr)->GetPositionX(), (*itr)->GetPositionY() );
				if( Dist <= range && Dist < BestDistance )
				{
					if(isAttackable(u_caster, (*itr), !( GetProto()->c_is_flags & SPELL_FLAG_IS_TARGETINGSTEALTHED ) ))
					{
						if( HasTarget( (*itr)->GetGUID(), tmpMap ) == false )
						{
							BestDistance = Dist;
							BestTarget = SafeUnitCast((*itr));
						}
					}
				}
			}
			m_caster->ReleaseInrangeLock();

			if( BestTarget != NULL )
			{
				did_hit_result = DidHit(i, BestTarget );
				if(did_hit_result==SPELL_DID_HIT_SUCCESS)
					SafeAddTarget(tmpMap, BestTarget->GetGUID());
				else
					SafeAddModeratedTarget( BestTarget->GetGUID(), did_hit_result );
			}
			PrevTarget = BestTarget;
		}
	}
//	OrderChainTargetVect( m_caster, tmpMap );
}

/// Spell Target Handling for type 8:  related to Chess Move (DND), Firecrackers, Spotlight, aedm, Spice Mortar
/// Seems to be some kind of custom area of effect... Scripted... or something like that
void Spell::SpellTargetCustomAreaOfEffect(uint32 i, uint32 j)
{
	// This should be good enough for now
	if( m_targets.m_destX == 0 )
	{
		bool can_target_unit = false;
		if( m_targets.m_unitTarget && m_caster->GetMapMgr() )
		{
			Unit *u = m_caster->GetMapMgr()->GetUnit(m_targets.m_unitTarget);
			float maxRange = GetMaxRange( dbcSpellRange.LookupEntryForced( GetProto()->rangeIndex ) );
			if( u != NULL && IsInrange( m_caster, u, maxRange * maxRange ) )
			{
/*				m_targets.m_targetMask |= TARGET_FLAG_DEST_LOCATION;
				m_targets.m_destX = m_caster->GetPositionX();
				m_targets.m_destY = m_caster->GetPositionY();
				m_targets.m_destZ = m_caster->GetPositionZ(); */
				FillAllTargetsInArea(i,u->GetPositionX(),u->GetPositionY(),u->GetPositionZ(),GetRadius(i));
				can_target_unit = true;
			}
		}
		if( can_target_unit == false )
			FillAllTargetsInArea(i,m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),GetRadius(i));
	}
	else
		FillAllTargetsInArea(i,m_targets.m_destX,m_targets.m_destY,m_targets.m_destZ,GetRadius(i));
}

/// Spell Target Handling for type 15 / 16: All Enemies in Area of Effect (instant)
void Spell::SpellTargetAreaOfEffect(uint32 i, uint32 j)
{
	if( m_targets.m_destX == 0 )
		FillAllTargetsInArea(i,m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),GetRadius(i));
	else
		FillAllTargetsInArea(i,m_targets.m_destX,m_targets.m_destY,m_targets.m_destZ,GetRadius(i));
}

/// Spell Target Handling for type 18: Land under caster
void Spell::SpellTargetLandUnderCaster(uint32 i, uint32 j) /// I don't think this is the correct name for this one
{
	TargetsList *tmpMap=&m_targetUnits[i];
	if(m_spellInfo->eff[i].Effect != SPELL_EFFECT_SUMMON_DEMON && m_spellInfo->eff[i].Effect != SPELL_EFFECT_SUMMON_OBJECT_WILD && m_spellInfo->eff[i].Effect != SPELL_EFFECT_SUMMON_OBJECT)
		FillAllTargetsInArea(i,m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),GetRadius(i));
	else
		SafeAddTarget(tmpMap,m_caster->GetGUID());
}

/// Spell Target Handling for type 18: All Party Members around the Caster in given range NOT RAID
void Spell::SpellTargetAllPartyMembersRangeNR(uint32 i, uint32 j)
{
	TargetsList* tmpMap = &m_targetUnits[i];
	Player* p = p_caster;

	if( p == NULL )
	{
		if( u_caster->IsCreature() && SafeCreatureCast( u_caster)->IsTotem() )
			p = SafePlayerCast( SafeCreatureCast( u_caster )->GetTotemOwner() );
		else if( u_caster->IsPet() && SafePetCast( u_caster )->GetPetOwner() ) 
			p = SafePetCast( u_caster )->GetPetOwner();
		else if( u_caster->GetUInt64Value( UNIT_FIELD_CREATEDBY ) )
		{
			Unit *t = u_caster->GetMapMgr()->GetUnit( u_caster->GetUInt64Value( UNIT_FIELD_CREATEDBY ) );
			if ( t && t->IsPlayer() )
				p = SafePlayerCast( t );
		}
	}

	if( p == NULL )
		return;

	int32 jumps = GetProto()->GetMaxTargets();
	if((GetProto()->c_is_flags & SPELL_FLAG_IS_REQUIRING_SM_MOD) && u_caster && u_caster->SM_Mods )
		SM_FIValue(u_caster->SM_Mods->SM_FAdditionalTargets,&jumps,GetProto()->GetSpellGroupType());

	float r = GetRadius(i);

	r *= r;
	if( IsInrange( m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ(), p, r ) )
		SafeAddTarget( tmpMap, p->GetGUID() );	 

	SubGroup* subgroup = p->GetGroup() ? p->GetGroup()->GetSubGroup( p->GetSubGroup() ) : 0;

	if( subgroup != NULL )
	{				
		p->GetGroup()->Lock();
		for(GroupMembersSet::iterator itr = subgroup->GetGroupMembersBegin(); itr != subgroup->GetGroupMembersEnd(); ++itr)
		{
			if(!(*itr)->m_loggedInPlayer || m_caster == (*itr)->m_loggedInPlayer) 
				continue;
			if(IsInrange(m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),(*itr)->m_loggedInPlayer,r))
			{
				uint8 did_hit_result = DidHitFriendly(i,(*itr)->m_loggedInPlayer);
				if(did_hit_result==SPELL_DID_HIT_SUCCESS)
				{
					//m_magnetcaster - magnettarget to redirect spell casts
					SafeAddTarget(tmpMap,(*itr)->m_loggedInPlayer->GetGUID());
					jumps--;
					if( jumps == 0 )
						break;
				}
				//hmm, summons are free targets ? how about a more detailed check here ?
				if( (*itr)->m_loggedInPlayer->GetSummon() )
				{
					uint8 did_hit_result = DidHitFriendly(i,(*itr)->m_loggedInPlayer->GetSummon() );
					if(did_hit_result==SPELL_DID_HIT_SUCCESS)
					{
						//m_magnetcaster - magnettarget to redirect spell casts
						SafeAddTarget(tmpMap,(*itr)->m_loggedInPlayer->GetSummon()->GetGUID());
						jumps--;
						if( jumps == 0 )
							break;
					}
				}
			}
		}
		p->GetGroup()->Unlock();
	}
}

/// Spell Target Handling for type 21: Single Target Friend
void Spell::SpellTargetSingleTargetFriend(uint32 i, uint32 j)
{
	TargetsList *tmpMap=&m_targetUnits[i];
	Unit *Target;
	if(m_targets.m_unitTarget == m_caster->GetGUID())
		Target = u_caster;
	else
		Target = m_caster->GetMapMgr() ? m_caster->GetMapMgr()->GetUnit(m_targets.m_unitTarget) : NULL;

	if( Target && isAttackable(Target,m_caster) == true )
		return;

	//macro casted selfcasting
	if( Target == NULL )
		Target = u_caster;

	uint8 did_hit_result = DidHitFriendly( i, Target );
	if( did_hit_result != SPELL_DID_HIT_SUCCESS )
		return;

	float range= GetMaxRange(dbcSpellRange.LookupEntry(m_spellInfo->rangeIndex));
	if( Need_SM_Apply(GetProto()) && u_caster != NULL && u_caster->SM_Mods )
	{
		SM_FFValue( u_caster->SM_Mods->SM_FRange, &range, GetProto()->GetSpellGroupType() );
		SM_PFValue( u_caster->SM_Mods->SM_PRange, &range, GetProto()->GetSpellGroupType() );
#ifdef COLLECTION_OF_UNTESTED_STUFF_AND_TESTERS
		float spell_flat_modifers=0;
		float spell_pct_modifers=0;
		SM_FFValue(u_caster->SM_FRange,&spell_flat_modifers,GetProto()->GetSpellGroupType());
		SM_FFValue(u_caster->SM_PRange,&spell_pct_modifers,GetProto()->GetSpellGroupType());
		if(spell_flat_modifers!=0 || spell_pct_modifers!=0)
			printf("!!!!!spell range bonus mod flat %f , spell range bonus pct %f , spell range %f, spell group %u\n",spell_flat_modifers,spell_pct_modifers,maxRange,GetProto()->GetSpellGroupType());
#endif
	}
	range += 1.5f; //this is the bounding radious of a player and maybe we should consider one for enemy also ?
	range *= 1.07f;	//seems like visually there is this difference. Seen it with penance
	if(IsInrange (m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),Target, range*range))
		SafeAddTarget(tmpMap,m_targets.m_unitTarget);
}

/// Spell Target Handling for type 22: Enemy Targets around the Caster//changed party members around you
/// place around the target / near the target //targeted Area effect
void Spell::SpellTargetAoE(uint32 i, uint32 j) // something special
// grep: this is *totally* broken. AoE only attacking friendly party members and self
// is NOT correct. // not correct at all:P
// 3.3.3 -> not correct again : paired with fill all friendly in a lot of spells will target everything for healing + damaging... 
// !! this is a chained target gathering function. Depending of paired method it will gather sometimes enemies, friends, party members
{
	FillAllTargetsInArea(i,m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),GetRadius(i));
}

/// Spell Target Handling for type 23: Gameobject Target
void Spell::SpellTargetSingleGameobjectTarget(uint32 i, uint32 j)
{
	TargetsList *tmpMap=&m_targetUnits[i];
	SafeAddTarget(tmpMap,m_targets.m_unitTarget);
}

/// Spell Target Handling for type 24: Targets in Front of the Caster
void Spell::SpellTargetInFrontOfCaster(uint32 i, uint32 j)
{
	float Radius = GetRadius(i);
	TargetsList *tmpMap=&m_targetUnits[i];
	InRangeSetRecProt::iterator itr;
	uint8 did_hit_result;
	m_caster->AquireInrangeLock(); //make sure to release lock before exit function !
	InrangeLoopExitAutoCallback AutoLock;
	for( itr = m_caster->GetInRangeSetBegin( AutoLock ); itr != m_caster->GetInRangeSetEnd(); itr++ )
	{
		if(!((*itr)->IsUnit()) || !SafeUnitCast((*itr))->isAlive())
			continue;
		//is Creature in range
		if(m_caster->isInRange((*itr), Radius))
		{
			if( m_spellInfo->cone_width ? m_caster->isInArc( (*itr) , m_spellInfo->cone_width ) : m_caster->isInFront((*itr)) ) // !!! is the target within our cone ?
			{
				if(isAttackable(u_caster, (*itr), !( GetProto()->c_is_flags & SPELL_FLAG_IS_TARGETINGSTEALTHED ) ))
				{
					//not dead sure. AOE for sure ignores LOS, checked on blizz 
					if(sWorld.Collision && CollideInterface.CheckLOS( m_caster->GetMapId(), m_caster->GetPositionNC(), (*itr)->GetPositionNC() ) ) 
					{
						did_hit_result = DidHit(i,SafeUnitCast(*itr));
						if(did_hit_result==SPELL_DID_HIT_SUCCESS)
							SafeAddTarget(tmpMap,(*itr)->GetGUID());
						else
							SafeAddModeratedTarget((*itr)->GetGUID(), did_hit_result);
					}
				}	
			}
		}
	}
	m_caster->ReleaseInrangeLock();
}

/// Spell Target Handling for type 25: Single Target Friend	 // Used o.a. in Duel
void Spell::SpellTargetSingleFriend(uint32 i, uint32 j)
{
	TargetsList *tmpMap=&m_targetUnits[i];
	Unit *target;
	if( m_caster->GetMapMgr() )
		target = m_caster->GetMapMgr()->GetUnit( m_targets.m_unitTarget );
	else if( m_caster->IsUnit() && m_caster->GetGUID() == m_targets.m_unitTarget )
		target = SafePlayerCast( m_caster ); //not in world yet
	else
		target = NULL;
	if( target != NULL )
	{
		uint8 did_hit_result;
			//case of duels
		if( isAttackable(u_caster, target) )
			did_hit_result = DidHit(i,target);
		else
			did_hit_result = DidHitFriendly(i,target);
		if(did_hit_result==SPELL_DID_HIT_SUCCESS)
		{
			//m_magnetcaster - magnettarget to redirect spell casts
			target = target->CheckSpellTargetRedirect( this );
			SafeAddTarget(tmpMap, m_targets.m_unitTarget);
		}
		else
			SafeAddModeratedTarget(m_targets.m_unitTarget, did_hit_result);
	}
}

/// Spell Target Handling for type 26: unit target/Item Target
/// game object and item related... research pickpocket stuff
void Spell::SpellTargetGameobject_itemTarget(uint32 i, uint32 j)
{
	TargetsList *tmpMap=&m_targetUnits[i];
	if(m_targets.m_unitTarget)
		SafeAddTarget(tmpMap,m_targets.m_unitTarget);

	if(m_targets.m_itemTarget)
		SafeAddTarget(tmpMap,m_targets.m_itemTarget);
}

/// Spell Target Handling for type 27: target is owner of pet
void Spell::SpellTargetPetOwner(uint32 i, uint32 j)
{ 
	TargetsList* tmpMap = &m_targetUnits[i];
	if( u_caster != NULL && u_caster->IsPet() && SafePetCast( u_caster )->GetPetOwner() )
		SafeAddTarget( tmpMap, u_caster->GetUInt64Value( UNIT_FIELD_SUMMONEDBY ) );
	else if( u_caster != NULL && u_caster->GetAIInterface() && u_caster->GetAIInterface()->GetPetOwner() )
		SafeAddTarget( tmpMap, u_caster->GetAIInterface()->GetPetOwner()->GetGUID() );
	else if( u_caster != NULL )
		SafeAddTarget( tmpMap, u_caster->GetTopOwner()->GetGUID() );
}

/// this is handled in DO
/// Spell Target Handling for type 28: All Enemies in Area of Effect(Blizzard/Rain of Fire/volley) channeled
void Spell::SpellTargetEnemysAreaOfEffect(uint32 i, uint32 j)
{
	FillAllTargetsInArea(i,m_targets.m_destX,m_targets.m_destY,m_targets.m_destZ,GetRadius(i));
	//heroic leap for example need to target something to be able to jump there :( Not sure this is correct tough
	if( GetProto()->eff[i].EffectImplicitTargetA == 87 && m_targetUnits[i].empty() && u_caster != NULL )
		SpellTargetSelf( i, j );
}

// all object around the the caster / object
/*
mysql> select id,name from spell where EffectImplicitTargetb1 = 29;
+-------+-----------------------------------------------+
| 23467 | Tower Buff									|
| 32087 | Putrid Cloud								  |
| 34378 | Thrall Calls Thunder						  |
| 35487 | Seed of Revitalization Lightning Cloud Visual |
| 36037 | Rina's Bough Lightning Cloud Visual		   |
+-------+-----------------------------------------------+
*/
/// Spell Target Handling for type 29: all object around the the caster / object (so it seems)
void Spell::SpellTargetTypeTAOE(uint32 i, uint32 j)
{
	if( m_caster->GetMapMgr() == NULL )
		return;

	Unit* Target = m_caster->GetMapMgr()->GetUnit(m_targets.m_unitTarget);
	if( Target == NULL )
		return;

	// tranquility
	if( u_caster != NULL && m_spellInfo->NameHash == SPELL_HASH_TRANQUILITY )
		m_targetUnits[i].push_back( u_caster->GetGUID() );
	else
		FillAllTargetsInArea( (LocationVector&)Target->GetPosition(), i );
}

/// Spell Target Handling for type 30: PBAE Party Based Area Effect
void Spell::SpellTargetAllyBasedAreaEffect(uint32 i, uint32 j)
{
	/* Description
	We take the selected party member(also known as target), then we get a list of all the party members in the area

	// Used in
	26043 -> Battle Shout
	*/
	FillAllFriendlyInArea(i,m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),GetRadius(i));
}

/// Spell Target Handling for type 31: related to scripted effects
void Spell::SpellTargetScriptedEffects(uint32 i, uint32 j)
{
	FillAllTargetsInArea(i,m_targets.m_destX,m_targets.m_destY,m_targets.m_destZ,GetRadius(i));
}

/// Spell Target Handling for type 32 / 73: related to summoned pet or creature
void Spell::SpellTargetSummon(uint32 i, uint32 j)
{// Minion Target
	TargetsList *tmpMap=&m_targetUnits[i];
	if(m_caster->GetUInt64Value(UNIT_FIELD_SUMMON) == 0)
		SafeAddTarget(tmpMap,m_caster->GetGUID());
	else
		SafeAddTarget(tmpMap,m_caster->GetUInt64Value(UNIT_FIELD_SUMMON));
}

/// Spell Target Handling for type 33: Party members of totem, inside given range
void Spell::SpellTargetNearbyPartyMembers(uint32 i, uint32 j)
{
	float r = GetRadius(i);
	r *= r;
	TargetsList *tmpMap=&m_targetUnits[i];
	// this implementation is wrong.... this one is for totems
	if( u_caster != NULL )
	{
		Unit *u = u_caster->GetTopOwner();
		if( u && u->IsPlayer() )
		{
//			if( SafeCreatureCast( u_caster )->IsTotem() )
			{
//				Player* p = SafePlayerCast( SafeCreatureCast( u_caster )->GetTotemOwner() );
				Player *p = SafePlayerCast( u );
//				if( p == NULL)
//					return;

				if(IsInrange(m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),p,r))
					SafeAddTarget(tmpMap,p->GetGUID());

				SubGroup * pGroup = p->GetGroup() ?
					p->GetGroup()->GetSubGroup(p->GetSubGroup()) : 0;

				if(pGroup)
				{
					p->GetGroup()->Lock();
					for(GroupMembersSet::iterator itr = pGroup->GetGroupMembersBegin();
						itr != pGroup->GetGroupMembersEnd(); ++itr)
					{
						if(!(*itr)->m_loggedInPlayer || p == (*itr)->m_loggedInPlayer) 
							continue;
						if(IsInrange(m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),(*itr)->m_loggedInPlayer,r))
						{
							uint8 did_hit_result = DidHitFriendly(i,(*itr)->m_loggedInPlayer);
							if(did_hit_result==SPELL_DID_HIT_SUCCESS)
								SafeAddTarget(tmpMap,(*itr)->m_loggedInPlayer->GetGUID());
							//hmm, summons are free targets ? how about a more detailed check here ?
							if( (*itr)->m_loggedInPlayer->GetSummon() )
							{
								uint8 did_hit_result = DidHitFriendly(i,(*itr)->m_loggedInPlayer->GetSummon() );
								if(did_hit_result==SPELL_DID_HIT_SUCCESS)
									SafeAddTarget(tmpMap,(*itr)->m_loggedInPlayer->GetSummon()->GetGUID());
							}
						}
					}
					p->GetGroup()->Unlock();
				}
			}
		}
	}
}

/// Spell Target Handling for type 35: Single Target Party Member (if not in party then the target can not be himself)
/// this one requeres more research
void Spell::SpellTargetSingleTargetPartyMember(uint32 i, uint32 j)
{
	TargetsList *tmpMap=&m_targetUnits[i];
	if(!m_caster->IsInWorld())
		return;

	Unit* Target = m_caster->GetMapMgr()->GetPlayer( m_targets.m_unitTarget );
	if(!Target)
		return;
	float range=GetMaxRange(dbcSpellRange.LookupEntry(m_spellInfo->rangeIndex));
	if( Need_SM_Apply(GetProto()) && u_caster != NULL && u_caster->SM_Mods )
	{
		SM_FFValue( u_caster->SM_Mods->SM_FRange, &range, GetProto()->GetSpellGroupType() );
		SM_PFValue( u_caster->SM_Mods->SM_PRange, &range, GetProto()->GetSpellGroupType() );
#ifdef COLLECTION_OF_UNTESTED_STUFF_AND_TESTERS
		float spell_flat_modifers=0;
		float spell_pct_modifers=0;
		SM_FFValue(u_caster->SM_FRange,&spell_flat_modifers,GetProto()->GetSpellGroupType());
		SM_FFValue(u_caster->SM_PRange,&spell_pct_modifers,GetProto()->GetSpellGroupType());
		if(spell_flat_modifers!=0 || spell_pct_modifers!=0)
			printf("!!!!!spell range bonus mod flat %f , spell range bonus pct %f , spell range %f, spell group %u\n",spell_flat_modifers,spell_pct_modifers,maxRange,GetProto()->GetSpellGroupType());
#endif
	}
	range += 1.5f; //this is the bounding radious of a player and maybe we should consider one for enemy also ?
	if(IsInrange(m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),Target,range*range))
		SafeAddTarget(tmpMap,m_targets.m_unitTarget);
}

/// Spell Target Handling for type 36: these targets are scripted :s or something.. there seems to be a system...
void Spell::SpellTargetScriptedEffects2(uint32 i, uint32 j)
{
	//TargetsList *tmpMap=&m_targetUnits[i];

}

/// Spell Target Handling for type 37: all Members of the targets party
void Spell::SpellTargetPartyMember(uint32 i, uint32 j)
{
	if(!m_caster->IsInWorld())
		return;

	TargetsList *tmpMap=&m_targetUnits[i];
	// if no group target self
	Player * Target = m_caster->GetMapMgr()->GetPlayer( m_targets.m_unitTarget );
	if(!Target)
		return;

	SubGroup * subgroup = Target->GetGroup() ?
		Target->GetGroup()->GetSubGroup(Target->GetSubGroup()) : 0;

	float rsqr = GetRadius( i );
	if( rsqr == 0 )
		rsqr = 40*40;//kinda like a bug. 0 range to target a party member ? Highly impossible
	else rsqr *= rsqr;	
	if(subgroup)
	{
		Target->GetGroup()->Lock();
		for(GroupMembersSet::iterator itr = subgroup->GetGroupMembersBegin(); itr != subgroup->GetGroupMembersEnd(); ++itr)
			//if you are picky you could also check if on same map. Let's face it you won't similar positions on different maps
			if((*itr)->m_loggedInPlayer && IsInrange(Target,(*itr)->m_loggedInPlayer, rsqr) )
			{
				uint8 did_hit_result = DidHitFriendly(i,(*itr)->m_loggedInPlayer);
				if(did_hit_result==SPELL_DID_HIT_SUCCESS)
					SafeAddTarget(tmpMap,(*itr)->m_loggedInPlayer->GetGUID());
				//hmm, summons are free targets ? how about a more detailed check here ?
				if( (*itr)->m_loggedInPlayer->GetSummon() )
				{
					uint8 did_hit_result = DidHitFriendly(i,(*itr)->m_loggedInPlayer->GetSummon() );
					if(did_hit_result==SPELL_DID_HIT_SUCCESS)
						SafeAddTarget(tmpMap,(*itr)->m_loggedInPlayer->GetSummon()->GetGUID());
				}
			}
		Target->GetGroup()->Unlock();
	}
	else
	{
		SafeAddTarget(tmpMap,Target->GetGUID());
	}
}

/// Spell Target Handling for type 38: Dummy Target (Server-side script effect)
void Spell::SpellTargetDummyTarget(uint32 i, uint32 j)
{
	TargetsList *tmpMap=&m_targetUnits[i];
	switch( m_spellInfo->Id )
	{
		case 12938:
			{
				//FIXME:this ll be immortal targets
				FillAllTargetsInArea(i,m_targets.m_destX,m_targets.m_destY,m_targets.m_destZ,GetRadius(i));
			}
			break;
		case 30427:
		{
			uint32 cloudtype;
			Creature *creature;

			if(!p_caster)
				return;

			InRangeSetRecProt::iterator itr,itr2;
			m_caster->AquireInrangeLock(); //make sure to release lock before exit function !
			InrangeLoopExitAutoCallback AutoLock;
			for(itr2 = p_caster->GetInRangeSetBegin( AutoLock ); itr2 != p_caster->GetInRangeSetEnd(); )
			{
				itr=itr2;
				++itr2;
				if((*itr)->GetTypeId() == TYPEID_UNIT && p_caster->GetDistanceSq((*itr)) < 400)
				{
					creature=SafeCreatureCast((*itr));
					cloudtype=creature->GetEntry();
					if(cloudtype == 24222 || cloudtype == 17408 || cloudtype == 17407 || cloudtype == 17378)
					{
						p_caster->SetSelection(creature->GetGUID());
						m_caster->ReleaseInrangeLock();
						return;
					}
				}
			}
			m_caster->ReleaseInrangeLock();
		}break;
		case 51729: // Blessing of Zim'Torga
		case 51265: // Blessing of Zim'Abwa
		case 52051: // Blessing of Zim'Rhuk
			SafeAddTarget(tmpMap, m_targets.m_unitTarget);
			break;
		default:
			break;
	};
	SafeAddTarget(tmpMap,m_caster->GetGUID());
}

/// Spell Target Handling for type 39: Fishing
void Spell::SpellTargetFishing(uint32 i, uint32 j)
{
	TargetsList *tmpMap=&m_targetUnits[i];
	SafeAddTarget(tmpMap,m_caster->GetGUID());
}

/// Spell Target Handling for type 40: Activate Object target(probably based on focus)
/// movement proc, like traps.
void Spell::SpellTargetType40(uint32 i, uint32 j)
{
	//TargetsList *tmpMap=&m_targetUnits[i];
}

/// Spell Target Handling for type 41 / 42 / 43 / 44: Totems
void Spell::SpellTargetTotem(uint32 i, uint32 j)
{
	TargetsList *tmpMap=&m_targetUnits[i];
	if( p_caster )
	{
		int32 slot = m_spellInfo->eff[i].EffectImplicitTargetA - EFF_TARGET_TOTEM_EARTH;
		if( slot >=0 && slot < 4 && p_caster->m_TotemSlots[slot] )
			SafeAddTarget( tmpMap, p_caster->m_TotemSlots[slot]->GetGUID() );
	}
	if( tmpMap->empty() )
		SafeAddTarget(tmpMap,m_caster->GetGUID());
}

/// Spell Target Handling for type 45: Chain,!!only for healing!! for chain lightning =6 
void Spell::SpellTargetChainTargeting(uint32 i, uint32 j)
{
	if( !m_caster->IsInWorld() )
		return;

	TargetsList *tmpMap=&m_targetUnits[i];
	//if selected target is party member, then jumps on party
	Unit* firstTarget;

	bool PartyOnly = false;
	float range = GetMaxRange(dbcSpellRange.LookupEntry(m_spellInfo->rangeIndex));//this is probably wrong,
	if( Need_SM_Apply(GetProto()) && u_caster != NULL && u_caster->SM_Mods )
	{
		SM_FFValue( u_caster->SM_Mods->SM_FRange, &range, GetProto()->GetSpellGroupType() );
		SM_PFValue( u_caster->SM_Mods->SM_PRange, &range, GetProto()->GetSpellGroupType() );
#ifdef COLLECTION_OF_UNTESTED_STUFF_AND_TESTERS
		float spell_flat_modifers=0;
		float spell_pct_modifers=0;
		SM_FFValue(u_caster->SM_FRange,&spell_flat_modifers,GetProto()->GetSpellGroupType());
		SM_FFValue(u_caster->SM_PRange,&spell_pct_modifers,GetProto()->GetSpellGroupType());
		if(spell_flat_modifers!=0 || spell_pct_modifers!=0)
			printf("!!!!!spell range bonus mod flat %f , spell range bonus pct %f , spell range %f, spell group %u\n",spell_flat_modifers,spell_pct_modifers,maxRange,GetProto()->GetSpellGroupType());
#endif
	}
	range += 1.5f; //this is the bounding radious of a player and maybe we should consider one for enemy also ?
	//this is cast distance, not searching distance
	range *= range;

	firstTarget = m_caster->GetMapMgr()->GetPlayer( m_targets.m_unitTarget );
	if( firstTarget && p_caster != NULL )
	{
		if( p_caster->InGroup() )
			if( p_caster->GetSubGroup() == SafePlayerCast( firstTarget )->GetSubGroup() )
				PartyOnly=true;					
	}
	else
	{
		firstTarget = m_caster->GetMapMgr()->GetUnit(m_targets.m_unitTarget);
		if(!firstTarget) 
			return;
	}

	SafeAddTarget(tmpMap,firstTarget->GetGUID());

	int32 jumps = m_spellInfo->eff[i].EffectChainTarget;
	if((m_spellInfo->c_is_flags & SPELL_FLAG_IS_REQUIRING_SM_MOD) && u_caster && u_caster->SM_Mods )
		SM_FIValue(u_caster->SM_Mods->SM_FAdditionalTargets,(int32*)&jumps,m_spellInfo->GetSpellGroupType());

	if( jumps <= 0 )
		return;

	jumps--;
	if(PartyOnly)
	{
		GroupMembersSet::iterator itr;
		SubGroup * pGroup = p_caster->GetGroup() ?
			p_caster->GetGroup()->GetSubGroup(p_caster->GetSubGroup()) : 0;

		if(pGroup)
		{
			p_caster->GetGroup()->Lock();
			for(itr = pGroup->GetGroupMembersBegin();
				itr != pGroup->GetGroupMembersEnd(); ++itr)
			{
				if(!(*itr)->m_loggedInPlayer || (*itr)->m_loggedInPlayer==u_caster || !(*itr)->m_loggedInPlayer->isAlive() )
					continue;

				//we target stuff that has no full health. No idea if we must fill target list or not :(
				if( (*itr)->m_loggedInPlayer->GetUInt32Value( UNIT_FIELD_HEALTH ) == (*itr)->m_loggedInPlayer->GetUInt32Value( UNIT_FIELD_MAXHEALTH ) )
					continue;

				if( IsInrange(u_caster,(*itr)->m_loggedInPlayer, range) )
				{
					uint8 did_hit_result = DidHit(i,(*itr)->m_loggedInPlayer);
					if(did_hit_result==SPELL_DID_HIT_SUCCESS)
					{
						SafeAddTarget(tmpMap,(*itr)->m_loggedInPlayer->GetGUID());
						if(!--jumps)
						{
							p_caster->GetGroup()->Unlock();
//							OrderChainTargetVect( m_caster, tmpMap );
							return;
						}
					}
					//hmm, summons are free targets ? how about a more detailed check here ?
					if( (*itr)->m_loggedInPlayer->GetSummon() )
					{
						uint8 did_hit_result = DidHitFriendly(i,(*itr)->m_loggedInPlayer->GetSummon() );
						if(did_hit_result==SPELL_DID_HIT_SUCCESS)
						{
							//m_magnetcaster - magnettarget to redirect spell casts
							SafeAddTarget(tmpMap,(*itr)->m_loggedInPlayer->GetSummon()->GetGUID());
							jumps--;
							if( jumps == 0 )
								break;
						}
					}
				}
			}
			p_caster->GetGroup()->Unlock();
		}
		if( jumps && PartyOnly )
			SafeAddTarget( tmpMap, u_caster->GetGUID() );
	}//find nearby friendly target
	else
	{
		InRangeSetRecProt::iterator itr;
		firstTarget->AquireInrangeLock(); //make sure to release lock before exit function !
		InrangeLoopExitAutoCallback AutoLock;
		for( itr = firstTarget->GetInRangeSetBegin( AutoLock ); itr != firstTarget->GetInRangeSetEnd(); itr++ )
		{
			if( !(*itr)->IsUnit() || !SafeUnitCast((*itr))->isAlive())
				continue;

			//we target stuff that has no full health. No idea if we must fill target list or not :(
			if( (*itr)->GetUInt32Value( UNIT_FIELD_HEALTH ) == (*itr)->GetUInt32Value( UNIT_FIELD_MAXHEALTH ) )
				continue;

			if(IsInrange(firstTarget,*itr, range))
			{
				if(!isAttackable(u_caster,(*itr), !( GetProto()->c_is_flags & SPELL_FLAG_IS_TARGETINGSTEALTHED ) ))
				{
					uint8 did_hit_result = DidHit(i,SafeUnitCast(*itr));
					if(did_hit_result==SPELL_DID_HIT_SUCCESS)
					{
						SafeAddTarget(tmpMap,(*itr)->GetGUID());
						if(!--jumps)
						{
							firstTarget->ReleaseInrangeLock();
//							OrderChainTargetVect( m_caster, tmpMap );
							return;
						}
					}
				}
			}
		}
		firstTarget->ReleaseInrangeLock();
	}
//	OrderChainTargetVect( m_caster, tmpMap );
}

/// Spell target handling for commonly used simple target add's
/// mostly used in scripted and kinda unknown target

/// This handler is used in
/// Spell Target Handling for type 46: Unknown Summon Atal'ai Skeleton
/// Spell Target Handling for type 47: Portal (Not sure, fix me.)
/// Spell Target Handling for type 48: fix me
/// Spell Target Handling for type 49: fix me
/// Spell Target Handling for type 50: fix me
/// Spell Target Handling for type 52: Lightwells, etc (Need more research)

void Spell::SpellTargetSimpleTargetAdd(uint32 i, uint32 j)
{
	SafeAddTarget(&m_targetUnits[i],m_caster->GetGUID());
}

/// Spell Target Handling for type 53: Target Area by Players CurrentSelection()
void Spell::SpellTargetTargetAreaSelectedUnit(uint32 i, uint32 j)
{
	//TargetsList *tmpMap=&m_targetUnits[i];
	Unit *Target = NULL;
	if(m_caster->IsInWorld())
	{
		if(p_caster)
			Target = m_caster->GetMapMgr()->GetUnit(p_caster->GetSelection());
		else
			Target = m_caster->GetMapMgr()->GetUnit(m_targets.m_unitTarget);
	}

	if(!Target)
		return;

	FillAllTargetsInArea((LocationVector&)Target->GetPosition(), i);
}

/// Spell Target Handling for type 54: Targets in Front of the Caster
void Spell::SpellTargetInFrontOfCaster2(uint32 i, uint32 j)
{
	float r = GetRadius(i);
	TargetsList *tmpMap=&m_targetUnits[i];
	InRangeSetRecProt::iterator itr,itr2;
	uint8 did_hit_result;
	m_caster->AquireInrangeLock(); //make sure to release lock before exit function !
	InrangeLoopExitAutoCallback AutoLock;
	for( itr2 = m_caster->GetInRangeSetBegin( AutoLock ); itr2 != m_caster->GetInRangeSetEnd();)
	{
		itr = itr2;
		itr2++;
		if(!((*itr)->IsUnit()) || !SafeUnitCast((*itr))->isAlive())
			continue;
		//is Creature in range
		if(m_caster->isInRange((*itr),r))
		{
			if(m_caster->isInFront((*itr)))
			{
				if(isAttackable(u_caster, (*itr), !( GetProto()->c_is_flags & SPELL_FLAG_IS_TARGETINGSTEALTHED ) ))
				{
					did_hit_result = DidHit(i,SafeUnitCast(*itr));
					if(did_hit_result==SPELL_DID_HIT_SUCCESS)
						SafeAddTarget(tmpMap,(*itr)->GetGUID());
					else
						SafeAddModeratedTarget((*itr)->GetGUID(), did_hit_result);
				}	
			}
		}
	}
	m_caster->ReleaseInrangeLock();
}

// Spell Target Handling for type 56: Target all raid members. (WotLK)
void Spell::SpellTargetAllRaid(uint32 i, uint32 j)
{
/*	if( !m_caster->IsInWorld() || !m_caster->IsUnit() )
		return;

	Group * group = SafeUnitCast( m_caster )->GetGroup(); */
	if( !m_caster->IsInWorld() )
		return;

	Unit *target;
/*	if( m_caster->IsPet() )
		target = SafePetCast( m_caster )->GetPetOwner(); //pets
	else if( u_caster != NULL )
	{
		if( u_caster->IsCreature() && u_caster->GetAIInterface() ) //totems
			target = u_caster->GetAIInterface()->GetPetOwner();
		else if( u_caster->GetUInt64Value( UNIT_FIELD_CREATEDBY ) )
			target = u_caster->GetMapMgr()->GetUnit( u_caster->GetUInt64Value( UNIT_FIELD_CREATEDBY ) );
		else
			target = u_caster;
	}*/
	if( u_caster )
		target = u_caster->GetTopOwner();
	else 
		target = p_caster;

	if( !target || !target->IsPlayer() )
		return;

	Player *p_target = SafePlayerCast( target );

	TargetsList* tmpMap = &m_targetUnits[i];
	SafeAddTarget( tmpMap, m_caster->GetGUID() );
	//special case for tremor totem
	if( target && target->IsPlayer() )
	{
		if( SafePlayerCast( target )->GetSummon( 0 ) )
			SafeAddTarget( tmpMap,SafePlayerCast( target )->GetSummon( 0 )->GetGUID() );
		//tremor totem and spirit wolf
		if( SafePlayerCast( target )->GetSummon( 1 ) )
			SafeAddTarget( tmpMap,SafePlayerCast( target )->GetSummon( 1 )->GetGUID() );
	}

	//pet casting on groupless owner
	if( m_caster != target )
		SafeAddTarget( tmpMap, target->GetGUID() );

	int32 max_targets = GetProto()->GetMaxTargets();
	if((GetProto()->c_is_flags & SPELL_FLAG_IS_REQUIRING_SM_MOD) && u_caster && u_caster->SM_Mods )
		SM_FIValue(u_caster->SM_Mods->SM_FAdditionalTargets,&max_targets,GetProto()->GetSpellGroupType());

	Group * group = SafePlayerCast( p_target )->GetGroup(); 
	if( group == NULL )
		return;
	
	uint32 count = group->GetSubGroupCount();

	// Loop through each raid group.
	bool SkippedAny = false;
	for( uint8 SmartTargetting=0;SmartTargetting<2 && max_targets>0;SmartTargetting++)
	{
		for( uint8 k = 0; k < count; k++ )
		{
			SubGroup * subgroup = group->GetSubGroup( k );
			if( subgroup )
			{
				group->Lock();
				for( GroupMembersSet::iterator itr = subgroup->GetGroupMembersBegin(); itr != subgroup->GetGroupMembersEnd(); ++itr )
				{
					if( (*itr)->m_loggedInPlayer && (*itr)->m_loggedInPlayer != p_target && (*itr)->m_loggedInPlayer->isAlive()	)
					{
						bool SmartTargetSelect_SkipThisTarget;
						if( SmartTargetting > 0 )
							SmartTargetSelect_SkipThisTarget = false;
						else if( max_targets > 200 )
							SmartTargetSelect_SkipThisTarget = false;
						else if( ( GetProto()->c_is_flags & SPELL_FLAG_IS_HEALING ) != 0 && (*itr)->m_loggedInPlayer->GetUInt32Value( UNIT_FIELD_HEALTH ) == (*itr)->m_loggedInPlayer->GetUInt32Value( UNIT_FIELD_MAXHEALTH ) )
							SmartTargetSelect_SkipThisTarget = true;
						else if( ( GetProto()->c_is_flags2 & SPELL_FLAG2_IS_ENERGIZING ) != 0 && (*itr)->m_loggedInPlayer->GetPower( (*itr)->m_loggedInPlayer->GetPowerType() ) == (*itr)->m_loggedInPlayer->GetMaxPower( (*itr)->m_loggedInPlayer->GetPowerType() ) )
							SmartTargetSelect_SkipThisTarget = true;
						
						if( SmartTargetSelect_SkipThisTarget == false )
						{
							SafeAddTarget( tmpMap,(*itr)->m_loggedInPlayer->GetGUID() );

							//hmm, summons are free targets ? how about a more detailed check here ?
							if( (*itr)->m_loggedInPlayer->GetSummon() )
								SafeAddTarget( tmpMap,(*itr)->m_loggedInPlayer->GetSummon()->GetGUID() );

							max_targets--;
							if( max_targets == 0 )
							{
								group->Unlock();
								return;
							}
						}
						else
							SkippedAny = true;
					}
				}
				group->Unlock();
			}
		}
		if( SkippedAny == false )
			break;
	}
}

/// Spell Target Handling for type 57: Targeted Party Member
void Spell::SpellTargetTargetPartyMember(uint32 i, uint32 j)
{
	if(!m_caster->IsInWorld())
		return;

	if (!m_caster->IsPet() && !m_caster->IsPlayer())
		return;

	TargetsList *tmpMap=&m_targetUnits[i];
	Unit* Target = m_caster->GetMapMgr()->GetUnit(m_targets.m_unitTarget);
	Unit* Caster = SafeUnitCast(m_caster);

	if(!Target || !Caster)
		return;

	if (!Target->IsPet() && !Target->IsPlayer())
		return;

	if (Caster->IsPet() && SafePetCast(Caster)->GetPetOwner() != NULL)
		Caster = SafePetCast(Caster)->GetPetOwner();

	if(Target->IsPet() && SafePetCast(Target)->GetPetOwner() != NULL)
		Target = SafePetCast(Target)->GetPetOwner();

	if (Caster != Target)
	{
		Group *c_group = SafePlayerCast(Caster)->GetGroup();

		if( !c_group )
			return; //caster or caster master are not in group, cannot cast spell on this target

		Group *t_group = SafePlayerCast(Target)->GetGroup();

		if( !t_group )
			return; //target does not have a group

		if( t_group != c_group )
			return; //caster and target are not in same group

	}
	float range=GetMaxRange(dbcSpellRange.LookupEntry(GetProto()->rangeIndex));
	if( Need_SM_Apply(GetProto()) && u_caster != NULL && u_caster->SM_Mods )
	{
		SM_FFValue( u_caster->SM_Mods->SM_FRange, &range, GetProto()->GetSpellGroupType() );
		SM_PFValue( u_caster->SM_Mods->SM_PRange, &range, GetProto()->GetSpellGroupType() );
#ifdef COLLECTION_OF_UNTESTED_STUFF_AND_TESTERS
		float spell_flat_modifers=0;
		float spell_pct_modifers=0;
		SM_FFValue(u_caster->SM_FRange,&spell_flat_modifers,GetProto()->GetSpellGroupType());
		SM_FFValue(u_caster->SM_PRange,&spell_pct_modifers,GetProto()->GetSpellGroupType());
		if(spell_flat_modifers!=0 || spell_pct_modifers!=0)
			printf("!!!!!spell range bonus mod flat %f , spell range bonus pct %f , spell range %f, spell group %u\n",spell_flat_modifers,spell_pct_modifers,maxRange,GetProto()->GetSpellGroupType());
#endif
	}
	range += 1.5f; //this is the bounding radious of a player and maybe we should consider one for enemy also ?
	if(IsInrange(m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),Target,range*range))
		SafeAddTarget(tmpMap,m_targets.m_unitTarget);
}


/// Spell Target Handling for type 61: targets with the same group/raid and the same class
void Spell::SpellTargetRaidSameClass(uint32 i, uint32 j)
{
	TargetsList *tmpMap=&m_targetUnits[i];
	if(!m_caster->IsInWorld())
		return;

	Player * Target = m_caster->GetMapMgr()->GetPlayer( m_targets.m_unitTarget );
	if(!Target)
		return;

	Group * group = Target->GetGroup();
	if( group )
	{
		group->Lock();
		uint32 count = group->GetSubGroupCount();
		// Loop through each raid group.
		for( uint8 k = 0; k < count; k++ )
		{
			SubGroup * subgroup = group->GetSubGroup( k );
			if(subgroup && subgroup->GetMemberCount() > 0 )
			{
				for(GroupMembersSet::iterator itr = subgroup->GetGroupMembersBegin(); itr != subgroup->GetGroupMembersEnd(); ++itr)
				{
					if(!(*itr)->m_loggedInPlayer || Target->getClass() != (*itr)->m_loggedInPlayer->getClass()) 
						continue;
					SafeAddTarget(tmpMap,(*itr)->m_loggedInPlayer->GetGUID());
				}
			}
		}
		group->Unlock();
	}
}

/// Spell Target Handling for type 90: Target: critter pet
void Spell::SpellTargetCritterPet(uint32 i, uint32 j)
{
	if(u_caster && u_caster->critterPet)
	{
		TargetsList *tmpMap=&m_targetUnits[i];
		SafeAddTarget(tmpMap,u_caster->critterPet->GetGUID());
	}
}

//right now we only make sure that party is injured. Maybe later if requested pick the most injured one
void Spell::SpellTargetSinglePartyInjured(uint32 i, uint32 j)
{
	//! we are prepared that on proc we do not get any specified target, we add the possibility however !
	TargetsList *tmpMap=&m_targetUnits[i];
	if(u_caster == NULL || !m_caster->IsInWorld() )
		return;

	Unit *owner = u_caster->GetTopOwner();
	if( owner->IsPlayer() )
	{
		Player *pcaster = SafePlayerCast( owner );
		GroupMembersSet::iterator itr;
		SubGroup * pGroup = pcaster->GetGroup() ?	pcaster->GetGroup()->GetSubGroup(pcaster->GetSubGroup()) : NULL;
		if(pGroup)
		{
			pcaster->GetGroup()->Lock();
			float range=GetMaxRange(dbcSpellRange.LookupEntry(GetProto()->rangeIndex));
			if( Need_SM_Apply(GetProto()) && u_caster != NULL && u_caster->SM_Mods )
			{
				SM_FFValue( u_caster->SM_Mods->SM_FRange, &range, GetProto()->GetSpellGroupType() );
				SM_PFValue( u_caster->SM_Mods->SM_PRange, &range, GetProto()->GetSpellGroupType() );
		#ifdef COLLECTION_OF_UNTESTED_STUFF_AND_TESTERS
				float spell_flat_modifers=0;
				float spell_pct_modifers=0;
				SM_FFValue(u_caster->SM_FRange,&spell_flat_modifers,GetProto()->GetSpellGroupType());
				SM_FFValue(u_caster->SM_PRange,&spell_pct_modifers,GetProto()->GetSpellGroupType());
				if(spell_flat_modifers!=0 || spell_pct_modifers!=0)
					printf("!!!!!spell range bonus mod flat %f , spell range bonus pct %f , spell range %f, spell group %u\n",spell_flat_modifers,spell_pct_modifers,maxRange,GetProto()->GetSpellGroupType());
		#endif
			}
			range += 1.5f; //this is the bounding radious of a player and maybe we should consider one for enemy also ?
			range *= range;
			for(itr = pGroup->GetGroupMembersBegin();itr != pGroup->GetGroupMembersEnd(); ++itr)
			{
				if(!(*itr)->m_loggedInPlayer || (*itr)->m_loggedInPlayer==u_caster || !(*itr)->m_loggedInPlayer->isAlive() )
					continue;

				//we target stuff that has no full health. No idea if we must fill target list or not :(
				if( (*itr)->m_loggedInPlayer->GetUInt32Value( UNIT_FIELD_HEALTH ) == (*itr)->m_loggedInPlayer->GetUInt32Value( UNIT_FIELD_MAXHEALTH ) )
					continue;

				if( IsInrange(u_caster,(*itr)->m_loggedInPlayer, range) )
				{
					SafeAddTarget(tmpMap,(*itr)->m_loggedInPlayer->GetGUID());
					pcaster->GetGroup()->Unlock();
					return;
				}
			}
			pcaster->GetGroup()->Unlock();
		}
	}
	//if we got here then we did not find any group member for some reason
	Player * Target = m_caster->GetMapMgr()->GetPlayer((uint32)m_targets.m_unitTarget);
	if( Target 
		&& isAttackable(m_caster, Target, false) == false 
		&& Target->GetHealth() != Target->GetMaxHealth() )
		SafeAddTarget(tmpMap,Target->GetGUID());
	else if( owner->GetHealth() != owner->GetMaxHealth() )
		SafeAddTarget(tmpMap,owner->GetGUID());
	else
	{
		if( owner->IsPlayer() && u_caster->GetHealth() == u_caster->GetMaxHealth() )
			SafeAddTarget(tmpMap,owner->GetGUID());	//favor player owned as target, used by some AOE mobs
		else
			SafeAddTarget(tmpMap,u_caster->GetGUID());
	}
}

void Spell::SpellTargetMultiplePartyInjured(uint32 i, uint32 j)
{
	//! we are prepared that on proc we do not get any specified target, we add the possibility however !
	TargetsList *tmpMap=&m_targetUnits[i];
	if(u_caster == NULL || !m_caster->IsInWorld() )
		return;

	Unit *owner = u_caster->GetTopOwner();

	if( owner->GetHealth() != owner->GetMaxHealth() )
		SafeAddTarget(tmpMap,owner->GetGUID());

	if( owner->IsPlayer() )
	{
		Player *pcaster = SafePlayerCast( owner );
		GroupMembersSet::iterator itr;
		SubGroup * pGroup = pcaster->GetGroup() ?	pcaster->GetGroup()->GetSubGroup(pcaster->GetSubGroup()) : NULL;
		if(pGroup)
		{
			float range=GetMaxRange(dbcSpellRange.LookupEntry(GetProto()->rangeIndex));
			if( Need_SM_Apply(GetProto()) && u_caster != NULL && u_caster->SM_Mods )
			{
				SM_FFValue( u_caster->SM_Mods->SM_FRange, &range, GetProto()->GetSpellGroupType() );
				SM_PFValue( u_caster->SM_Mods->SM_PRange, &range, GetProto()->GetSpellGroupType() );
		#ifdef COLLECTION_OF_UNTESTED_STUFF_AND_TESTERS
				float spell_flat_modifers=0;
				float spell_pct_modifers=0;
				SM_FFValue(u_caster->SM_FRange,&spell_flat_modifers,GetProto()->GetSpellGroupType());
				SM_FFValue(u_caster->SM_PRange,&spell_pct_modifers,GetProto()->GetSpellGroupType());
				if(spell_flat_modifers!=0 || spell_pct_modifers!=0)
					printf("!!!!!spell range bonus mod flat %f , spell range bonus pct %f , spell range %f, spell group %u\n",spell_flat_modifers,spell_pct_modifers,maxRange,GetProto()->GetSpellGroupType());
		#endif
			}
			range += 1.5f; //this is the bounding radious of a player and maybe we should consider one for enemy also ?
			range *= range;

			int32 max_targets = GetProto()->GetMaxTargets();
			if((GetProto()->c_is_flags & SPELL_FLAG_IS_REQUIRING_SM_MOD) && u_caster && u_caster->SM_Mods )
				SM_FIValue(u_caster->SM_Mods->SM_FAdditionalTargets,&max_targets,GetProto()->GetSpellGroupType());

			bool CheckFullHealth = ( max_targets < MAX_SPELL_TARGET_COUNT );
			bool CouldAddMore = false;
TRY_ADD_ANY_TARGETS:
			for(itr = pGroup->GetGroupMembersBegin();itr != pGroup->GetGroupMembersEnd(); ++itr)
			{
				if(!(*itr)->m_loggedInPlayer || (*itr)->m_loggedInPlayer==u_caster || !(*itr)->m_loggedInPlayer->isAlive() )
					continue;

				//we target stuff that has no full health. No idea if we must fill target list or not :(
				if( CheckFullHealth == true )
				{
					if( (*itr)->m_loggedInPlayer->GetUInt32Value( UNIT_FIELD_HEALTH ) == (*itr)->m_loggedInPlayer->GetUInt32Value( UNIT_FIELD_MAXHEALTH ) ) 
					{
						CouldAddMore = true;
						continue;
					}
				}

				if( IsInrange(u_caster,(*itr)->m_loggedInPlayer, range) )
				{
					if( SafeAddTarget(tmpMap,(*itr)->m_loggedInPlayer->GetGUID()) )
						max_targets--;
					if( max_targets <= 0 )
					{
						pcaster->GetGroup()->Unlock();
						return;
					}
				}
			}
			pcaster->GetGroup()->Unlock();
			if( CheckFullHealth == true && CouldAddMore == true && max_targets > 0 )
			{
				CheckFullHealth = false;
				goto TRY_ADD_ANY_TARGETS;
			}
		}
	}
	//if we got here then we did not find any group member for some reason
	Player * Target = m_caster->GetMapMgr()->GetPlayer((uint32)m_targets.m_unitTarget);
	if( Target 
		&& isAttackable(m_caster, Target, false) == false 
		&& Target->GetHealth() != Target->GetMaxHealth() )
		SafeAddTarget(tmpMap,Target->GetGUID());
	else if( owner->GetHealth() != owner->GetMaxHealth() )
		SafeAddTarget(tmpMap,owner->GetGUID());
	else
	{
		if( owner->IsPlayer() && u_caster->GetHealth() == u_caster->GetMaxHealth() )
			SafeAddTarget(tmpMap,owner->GetGUID());	//favor player owned as target, used by some AOE mobs
		else
			SafeAddTarget(tmpMap,u_caster->GetGUID());
	}
}

void Spell::SpellTargetMultipleRaidInjured(uint32 i, uint32 j)
{
	//! we are prepared that on proc we do not get any specified target, we add the possibility however !
	TargetsList *tmpMap=&m_targetUnits[i];
	if(u_caster == NULL || !m_caster->IsInWorld() )
		return;

	int32 max_targets = GetProto()->GetMaxTargets();
	Unit *owner = u_caster->GetTopOwner();
	if( owner->IsPlayer() )
	{
		float range=GetMaxRange(dbcSpellRange.LookupEntry(GetProto()->rangeIndex));
		if( Need_SM_Apply(GetProto()) && u_caster != NULL && u_caster->SM_Mods )
		{
			SM_FFValue( u_caster->SM_Mods->SM_FRange, &range, GetProto()->GetSpellGroupType() );
			SM_PFValue( u_caster->SM_Mods->SM_PRange, &range, GetProto()->GetSpellGroupType() );
#ifdef COLLECTION_OF_UNTESTED_STUFF_AND_TESTERS
			float spell_flat_modifers=0;
			float spell_pct_modifers=0;
			SM_FFValue(u_caster->SM_FRange,&spell_flat_modifers,GetProto()->GetSpellGroupType());
			SM_FFValue(u_caster->SM_PRange,&spell_pct_modifers,GetProto()->GetSpellGroupType());
			if(spell_flat_modifers!=0 || spell_pct_modifers!=0)
				printf("!!!!!spell range bonus mod flat %f , spell range bonus pct %f , spell range %f, spell group %u\n",spell_flat_modifers,spell_pct_modifers,maxRange,GetProto()->GetSpellGroupType());
#endif
		}
		range += 1.5f; //this is the bounding radious of a player and maybe we should consider one for enemy also ?
		range *= range;

		if((GetProto()->c_is_flags & SPELL_FLAG_IS_REQUIRING_SM_MOD) && u_caster && u_caster->SM_Mods )
			SM_FIValue(u_caster->SM_Mods->SM_FAdditionalTargets,&max_targets,GetProto()->GetSpellGroupType());

		float at_x,at_y;
		at_x = u_caster->GetPositionX();
		at_y = u_caster->GetPositionY();
		if( ( j == 0 && GetProto()->eff[i].EffectImplicitTargetA == EFF_TARGET_CUSTOM_RAID_INJURED_MULTI_AT_TARGET )
			|| (j == 1 && GetProto()->eff[i].EffectImplicitTargetB == EFF_TARGET_CUSTOM_RAID_INJURED_MULTI_AT_TARGET ) )
		{
			Player * Target = m_caster->GetMapMgr()->GetPlayer( (uint32)m_targets.m_unitTarget );
			if( Target )
			{
				at_x = Target->GetPositionX();
				at_y = Target->GetPositionY();
			}
		}
		if( max_targets < MAX_SPELL_TARGET_COUNT )
		{
			do {
				uint64 TargetGuid = FindLowestHealthRaidMember( SafePlayerCast( owner ), range, at_x, at_y, tmpMap );
				if( TargetGuid != owner->GetGUID() && TargetGuid != 0 && SafeAddTarget( tmpMap, TargetGuid ) )
					max_targets--;
				else
					break;
			}while( max_targets > 0 );
		}
		else
		{
			Group *group = owner->GetGroup();
			if(group)
			{
				group->Lock();
				for(uint32 j = 0; j < group->GetSubGroupCount(); ++j)
				{
					for(GroupMembersSet::iterator itr = group->GetSubGroup(j)->GetGroupMembersBegin(); itr != group->GetSubGroup(j)->GetGroupMembersEnd(); ++itr)
						if((*itr)->m_loggedInPlayer && (*itr)->m_loggedInPlayer->isAlive() && IsInrange( at_x, at_y, (*itr)->m_loggedInPlayer, range ) == true )
							SafeAddTarget( tmpMap, (*itr)->m_loggedInPlayer->GetGUID() );
				}
				group->Unlock();
			}
		}
	}
	//make sure to add caster if possible
	if( max_targets > 0 )
		SafeAddTarget( tmpMap, u_caster->GetGUID() );
}

bool HasGUIDInList(TargetsList* tgt,uint64 guid)
{
	for(TargetsList::iterator i=tgt->begin();i!=tgt->end();i++)
		if((*i)==guid)
		{ 
			return true;
		}
	return false;
}

// returns Guid of lowest percentage health friendly party or raid target within sqrt('dist') yards
uint64 Spell::FindLowestHealthRaidMember(Player *Target, uint32 dist, float at_x, float at_y, TargetsList *NotInList)
{

	if(!Target || !Target->IsInWorld())
		return 0;

	uint64 lowestHealthTarget = Target->GetGUID();
	uint32 lowestHealthPct = Target->GetHealthPct();
	//used when we are measuring distance from another spot then target player(caster)
	if( at_x == 0.0f )
	{
		at_x = Target->GetPositionX();
		at_y = Target->GetPositionY();
	}
	Group *group = Target->GetGroup();
	if(group)
	{
		group->Lock();
		for(uint32 j = 0; j < group->GetSubGroupCount(); ++j)
		{
			for(GroupMembersSet::iterator itr = group->GetSubGroup(j)->GetGroupMembersBegin(); itr != group->GetSubGroup(j)->GetGroupMembersEnd(); ++itr)
			{
				if( (*itr)->m_loggedInPlayer && (*itr)->m_loggedInPlayer->isAlive() && IsInrange( at_x, at_y, ((*itr)->m_loggedInPlayer), dist ) == true )
				{
					uint32 healthPct = (*itr)->m_loggedInPlayer->GetHealthPct();
					if(healthPct < lowestHealthPct && ( NotInList == NULL || HasGUIDInList( NotInList, (*itr)->m_loggedInPlayer->GetGUID() ) == false ) )
					{
						lowestHealthPct = healthPct;
						lowestHealthTarget = (*itr)->m_loggedInPlayer->GetGUID();
					}
				}
			}
		}
		group->Unlock();
	}
	return lowestHealthTarget;
}

// returns Guid of lowest percentage health friendly party or raid target within sqrt('dist') yards
uint64 Spell::FindLowestFriendly(Player *Target, uint32 distsq, float at_x, float at_y, TargetsList *NotInList)
{

	if(!Target || !Target->IsInWorld())
		return 0;

	uint64 lowestHealthTarget = Target->GetGUID();
	uint32 lowestHealthPct = Target->GetHealthPct();

	InRangeSetRecProt::iterator itr,itr2;
	Target->AquireInrangeLock(); //make sure to release lock before exit function !
	InrangeLoopExitAutoCallback AutoLock;
	for( itr2 = Target->GetInRangeSetBegin( AutoLock ); itr2 != Target->GetInRangeSetEnd();)
	{
		itr = itr2;
		itr2++;
		if(!((*itr)->IsUnit()) || !SafeUnitCast((*itr))->isAlive())
			continue;
		//is Creature in range
		if( IsInrange( at_x, at_y, (*itr), distsq ) )
		{
			if( isFriendly( Target, (*itr) ) )
			{
				uint32 healthPct = SafeUnitCast(*itr)->GetHealthPct();
				if( healthPct < lowestHealthPct && ( NotInList == NULL || HasGUIDInList( NotInList, (*itr)->GetGUID() ) == false ) )
				{
					lowestHealthPct = healthPct;
					lowestHealthTarget = (*itr)->GetGUID();
				}
			}	
		}
	}
	Target->ReleaseInrangeLock();
	return lowestHealthTarget;
}

// CUSTOM !!
void Spell::SpellTargetRaidInrangeAtTarget(uint32 i, uint32 j)
{
/*	if( !m_caster->IsInWorld() || !m_caster->IsUnit() )
		return;

	Group * group = SafeUnitCast( m_caster )->GetGroup(); */

	if( !m_caster->IsInWorld() )
		return;

	Unit *m_target = m_caster->GetMapMgr()->GetUnit( m_targets.m_unitTarget );

	if( !m_target )
		return;

	Unit *target;
	if( m_caster->IsPet() )
		target = SafePetCast( m_caster )->GetPetOwner(); //pets
	else if( u_caster != NULL )
	{
		if( u_caster->IsCreature() && u_caster->GetAIInterface() ) //totems
			target = u_caster->GetAIInterface()->GetPetOwner();
		else if( u_caster->GetUInt64Value( UNIT_FIELD_CREATEDBY ) )
			target = u_caster->GetMapMgr()->GetUnit( u_caster->GetUInt64Value( UNIT_FIELD_CREATEDBY ) );
		else
			target = u_caster;
	}
	else 
		target = p_caster;

	if( !target || !target->IsPlayer() )
		return;

	Player *p_target = SafePlayerCast( target );

	TargetsList* tmpMap = &m_targetUnits[i];

	int32 max_targets = GetProto()->GetMaxTargets();
	if((GetProto()->c_is_flags & SPELL_FLAG_IS_REQUIRING_SM_MOD) && u_caster && u_caster->SM_Mods )
		SM_FIValue(u_caster->SM_Mods->SM_FAdditionalTargets,&max_targets,GetProto()->GetSpellGroupType());

	Group * group = SafePlayerCast( p_target )->GetGroup(); 
	if( group == NULL )
	{
		SafeAddTarget( tmpMap, m_caster->GetGUID() );
		return;
	}
	
	uint32 count = group->GetSubGroupCount();
	float range=GetRadius(i);
	range*=range;

	// Loop through each raid group.
	for( uint8 k = 0; k < count; k++ )
	{
		SubGroup * subgroup = group->GetSubGroup( k );
		if( subgroup )
		{
			group->Lock();
			for( GroupMembersSet::iterator itr = subgroup->GetGroupMembersBegin(); itr != subgroup->GetGroupMembersEnd(); ++itr )
			{
				if( (*itr)->m_loggedInPlayer && (*itr)->m_loggedInPlayer->isAlive()
					&& IsInrange( (*itr)->m_loggedInPlayer ,m_target,range)
					&& (( GetProto()->c_is_flags & SPELL_FLAG_IS_HEALING ) == 0 || (*itr)->m_loggedInPlayer->GetUInt32Value( UNIT_FIELD_HEALTH ) != (*itr)->m_loggedInPlayer->GetUInt32Value( UNIT_FIELD_MAXHEALTH ) )
					&& (*itr)->m_loggedInPlayer->DuelingWith != m_caster //do not heal duel target even if he is in our group
					)
				{
					SafeAddTarget( tmpMap,(*itr)->m_loggedInPlayer->GetGUID() );
					max_targets--;
					if( max_targets == 0 )
					{
						group->Unlock();
						return;
					}
				}
			}
			group->Unlock();
		}
	}

	//ex wild growth should be castable on self even if not in party ?
	if( p_target == m_caster && p_target->isAlive()
//					&& (( GetProto()->c_is_flags & SPELL_FLAG_IS_HEALING ) == 0 || (*itr)->m_loggedInPlayer->GetUInt32Value( UNIT_FIELD_HEALTH ) != (*itr)->m_loggedInPlayer->GetUInt32Value( UNIT_FIELD_MAXHEALTH ) )
		&& max_targets > 0 )
	{
		SafeAddTarget( tmpMap, m_caster->GetGUID() );
		max_targets--;
	}

	//seems like we did not find enough suitable targets for our spell 
	if( ( GetProto()->c_is_flags & SPELL_FLAG_IS_HEALING ) && max_targets > 0 && max_targets < (int32)GetProto()->GetMaxTargets() )
	{
		for( uint8 k = 0; k < count; k++ )
		{
			SubGroup * subgroup = group->GetSubGroup( k );
			if( subgroup )
			{
				group->Lock();
				for( GroupMembersSet::iterator itr = subgroup->GetGroupMembersBegin(); itr != subgroup->GetGroupMembersEnd(); ++itr )
				{
					if( (*itr)->m_loggedInPlayer && (*itr)->m_loggedInPlayer->isAlive()	
						&& IsInrange( (*itr)->m_loggedInPlayer ,m_target, range )
						&& (*itr)->m_loggedInPlayer->DuelingWith != m_caster //do not heal duel target even if he is in our group
						)
					{
						SafeAddTarget( tmpMap,(*itr)->m_loggedInPlayer->GetGUID() );
						max_targets--;
						if( max_targets == 0 )
						{
							group->Unlock();
							return;
						}
					}
				}
				group->Unlock();
			}
		}
	}
}

/// used by 1 spell only ?
void Spell::SpellTargetInFrontOfCasterFriendly(uint32 i, uint32 j)
{
	float Radius = GetRadius(i);
	TargetsList *tmpMap=&m_targetUnits[i];
	InRangeSetRecProt::iterator itr;
	m_caster->AquireInrangeLock(); //make sure to release lock before exit function !
	InrangeLoopExitAutoCallback AutoLock;
	for( itr = m_caster->GetInRangeSetBegin( AutoLock ); itr != m_caster->GetInRangeSetEnd(); itr++ )
	{
		if(!((*itr)->IsUnit()) || !SafeUnitCast((*itr))->isAlive())
			continue;
		//is Creature in range
		if(m_caster->isInRange((*itr), Radius) == false )
			continue;
		if( m_spellInfo->cone_width ? m_caster->isInArc( (*itr) , m_spellInfo->cone_width ) : m_caster->isInFront((*itr)) ) // !!! is the target within our cone ?
		{
			if( isFriendly(u_caster, (*itr)) == true )
				SafeAddTarget(tmpMap,(*itr)->GetGUID());
		}
	}
	m_caster->ReleaseInrangeLock();
}