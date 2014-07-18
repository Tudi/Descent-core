#include "StdAfx.h"

#define REGISTER_HANDLER( spell, i ) sp = dbcSpell.LookupEntryForced( spell ); \
	ASSERT( sp->TargettingOverride[i] == 0 ); \
	sp->TargettingOverride[i] = &TO_ ## spell ## _ ## i;
#define REGISTER_SAMEHANDLER(spell, i , copy_from) sp = dbcSpell.LookupEntryForced( spell ); \
	ASSERT( sp->TargettingOverride[i] == 0 ); \
	sp->TargettingOverride[i] = &TO_ ## copy_from ## _ ## i;
#define REGISTER_SAME_INDEX_HANDLER(spell, src_i , dest_i) sp = dbcSpell.LookupEntryForced( spell ); \
	ASSERT( sp->TargettingOverride[dest_i] == 0 ); \
	sp->TargettingOverride[dest_i] = &TO_ ## spell ## _ ## src_i;

#define REGISTER_FILTERHANDLER( spell ) sp = dbcSpell.LookupEntryForced( spell ); \
	ASSERT( sp->CanTargetedFilterHandler == 0 ); \
	sp->CanTargetedFilterHandler = &CTFILTERH_##spell;
#define REGISTER_SAMEFILTERHANDLER(spell,copy_from) sp = dbcSpell.LookupEntryForced( spell ); \
	ASSERT( sp->CanTargetedFilterHandler == 0 ); \
	sp->CanTargetedFilterHandler = &CTFILTERH_##copy_from;

void TO_50882_0(Spell *sp,uint8 i)
{
	//Icy Talons only self proc if we do not have improved icy talons on ourself
	if( sp->u_caster && sp->u_caster->HasAuraWithNameHash( SPELL_HASH_IMPROVED_ICY_TALONS, 0, AURA_SEARCH_PASSIVE ) == 0 )
	{
		TargetsList *tmpMap=&sp->m_targetUnits[i];
		sp->SafeAddTarget( tmpMap  ,sp->u_caster->GetGUID() );
	}
}

void TO_48743_1(Spell *sp,uint8 i)
{
	TargetsList *tmpMap=&sp->m_targetUnits[i];
/*	if( sp->p_caster && sp->p_caster->GetSummon() )
		sp->SafeAddTarget(tmpMap,sp->p_caster->GetSummon()->GetGUID());

	InRangeSet::iterator itr,itr2;
	sp->m_caster->AquireInrangeLock(); //make sure to release lock before exit function !
	for( itr2 = sp->m_caster->GetInRangeSetBegin(); itr2 != sp->m_caster->GetInRangeSetEnd();)
	{
		itr = itr2;
		itr2++;
		if( !(*itr)->IsUnit() 
			|| !SafeUnitCast((*itr))->isAlive()
			|| ( (*itr)->GetUInt32Value(UNIT_FIELD_SUMMONEDBY) != sp->m_caster->GetUInt32Value(OBJECT_FIELD_GUID)
				 && (*itr)->GetUInt32Value(UNIT_FIELD_CREATEDBY) != sp->m_caster->GetUInt32Value(OBJECT_FIELD_GUID) )
			)
			continue;
		//our creations are always friendly
		sp->SafeAddTarget(tmpMap,(*itr)->GetGUID());
		break;
	}
	sp->m_caster->ReleaseInrangeLock();
	*/
	Creature *summon = sp->p_caster->GetMapMgr()->GetCreature( sp->m_targets.m_unitTarget );
	if( summon )
		sp->SafeAddTarget(tmpMap, sp->m_targets.m_unitTarget );
}

void TO_64382_0(Spell *sp,uint8 i)
{
	Unit * pTarget = sp->m_caster->GetMapMgr()->GetUnit(sp->m_targets.m_unitTarget);
	if( pTarget && pTarget->isAlive() )
	{
		TargetsList *tmpMap=&sp->m_targetUnits[i];
		sp->SafeAddTarget(tmpMap, pTarget->GetGUID());	
	}
}

void TO_51690_1(Spell *sp,uint8 i)
{
	//just gather targets the normal way
	sp->SpellTargetLandUnderCaster( i, 0 );
	//now in case we did not gather enough, just repeat adding one of them
	int32 target_repeats = sp->GetProto()->GetMaxTargets() - (int32)sp->m_targetUnits[i].size();
	uint64 first_target;
	if( sp->m_targetUnits[i].size() )
		first_target = sp->m_targetUnits[i][0];
	else
		first_target = sp->m_targets.m_unitTarget;
	for(int32 j=0;j<target_repeats;j++)
		sp->m_targetUnits[i].push_back( first_target );
}
/*
void TO_1535_0(Spell *sp,uint8 i)
{
	if( sp->p_caster && sp->p_caster->m_TotemSlots[EFF_TARGET_TOTEM_FIRE - EFF_TARGET_TOTEM_EARTH] )
		sp->m_targetUnits[i].push_back( sp->p_caster->m_TotemSlots[EFF_TARGET_TOTEM_FIRE - EFF_TARGET_TOTEM_EARTH]->GetGUID() );
} */

void TO_47541_1(Spell *sp,uint8 i)
{
	Unit * pTarget = sp->m_caster->GetMapMgr()->GetUnit( sp->m_targets.m_unitTarget );
	if( pTarget && pTarget->isAlive() && isAttackable( sp->m_caster, pTarget ) == false )
	{
		if( pTarget->IsPlayer() && ( pTarget->getRace() == RACE_UNDEAD || pTarget->HasAuraWithNameHash( SPELL_HASH_LICHBORNE, 0, AURA_SEARCH_POSITIVE ) ) )
			sp->m_targetUnits[i].push_back( sp->m_targets.m_unitTarget );
		else if( pTarget->IsCreature() && SafeCreatureCast( pTarget )->GetCreatureInfo() && SafeCreatureCast( pTarget )->GetCreatureInfo()->Type == UNDEAD )
			sp->m_targetUnits[i].push_back( sp->m_targets.m_unitTarget );
	}
}
/*
void TO_49016_1(Spell *sp,uint8 i)
{
	if( sp->u_caster )
	{
		TargetsList *tmpMap=&sp->m_targetUnits[i];
		sp->SafeAddTarget( tmpMap  ,sp->u_caster->GetGUID() );
	}
} */

void TO_46579_1(Spell *sp,uint8 i)
{
	TargetsList *tmpMap=&sp->m_targetUnits[i];

	float range = GetMaxRange( dbcSpellRange.LookupEntry( sp->m_spellInfo->rangeIndex ) );
	range *= range;

	InRangeSetRecProt::iterator itr,itr2;
	sp->m_caster->AquireInrangeLock(); //make sure to release lock before exit function !
	InrangeLoopExitAutoCallback AutoLock;
	for( itr2 = sp->m_caster->GetInRangeSetBegin( AutoLock ); itr2 != sp->m_caster->GetInRangeSetEnd(); )
	{
		itr = itr2;
		itr2++;

		if( !((*itr)->IsUnit()) || !SafeUnitCast((*itr))->isAlive() )
			continue;

		Unit *pTarget = SafeUnitCast(*itr);
		if(IsInrange(sp->m_caster->GetPositionX(),sp->m_caster->GetPositionY(),sp->m_caster->GetPositionZ(),pTarget,range))
		{
			if(isAttackable(sp->u_caster,pTarget))
			{
				uint32 did_hit_result = sp->DidHit(i,pTarget);
				if(did_hit_result==SPELL_DID_HIT_SUCCESS)
				{
					//check for undead or demon
					if( pTarget->getRace() == RACE_UNDEAD 
						|| pTarget->HasAuraWithNameHash( SPELL_HASH_LICHBORNE, 0, AURA_SEARCH_POSITIVE )
						|| ( pTarget->IsPlayer() && SafePlayerCast( pTarget )->GetShapeShift() == FORM_DEMON )
						|| ( pTarget->IsCreature() && SafeCreatureCast( pTarget )->GetCreatureInfo() && ( SafeCreatureCast( pTarget )->GetCreatureInfo()->Type == DEMON || SafeCreatureCast( pTarget )->GetCreatureInfo()->Type == UNDEAD || SafeCreatureCast( pTarget )->GetCreatureInfo()->Type == DRAGONSKIN || SafeCreatureCast( pTarget )->GetCreatureInfo()->Type == ELEMENTAL ) )
						)
					sp->SafeAddTarget(tmpMap, (*itr)->GetGUID());
				}
			}
		}
	}
	sp->m_caster->ReleaseInrangeLock();
}

void TO_81261_2(Spell *sp,uint8 i)
{
	if( sp->u_caster )
	{
		//this is melee weapon enchant. When it procs we are targetting the vistim
		Unit *sel = sp->u_caster->GetMapMgr()->GetUnit( sp->u_caster->GetTargetGUID() );
		if( sel )
		{
			sp->m_targets.m_targetMask = TARGET_FLAG_DEST_LOCATION;
			sp->m_targets.m_destX = sel->GetPositionX();
			sp->m_targets.m_destY = sel->GetPositionY();
			sp->m_targets.m_destZ = sel->GetPositionZ();
		}
	}
}

void TO_82690_0(Spell *sp,uint8 i)
{
	TargetsList *tmpMap=&sp->m_targetUnits[i];

	Unit *Target = NULL;
	float best_dist = 9999.0f;
	InRangeSetRecProt::iterator itr,itr2;
	sp->m_caster->AquireInrangeLock(); //make sure to release lock before exit function !
	InrangeLoopExitAutoCallback AutoLock;
	for( itr2 = sp->m_caster->GetInRangeSetBegin( AutoLock ); itr2 != sp->m_caster->GetInRangeSetEnd();)
	{
		itr = itr2;
		itr2++;
		if( !(*itr)->IsUnit() 
			|| !SafeUnitCast((*itr))->isAlive()
			|| isAttackable( sp->m_caster, SafeUnitCast(*itr) ) == false
			)
			continue;

		float tdist = DistanceSq( (*itr), sp->m_caster );
		if( tdist < best_dist )
		{
			best_dist = tdist;
			Target = SafeUnitCast(*itr);
		}
	}
	sp->m_caster->ReleaseInrangeLock();

	if( Target && best_dist < 40.0f * 40.0f )
		sp->SafeAddTarget(tmpMap,Target->GetGUID());
}

void TO_2812_1(Spell *sp,uint8 i)
{
	TargetsList::iterator it;
	for( it = sp->m_targetUnits[0].begin(); it != sp->m_targetUnits[0].end(); it++ )
	{
		Unit * pTarget = sp->m_caster->GetMapMgr()->GetUnit( (*it) );
		bool CanAdd = false;
		if( pTarget && pTarget->isAlive() && isAttackable( sp->m_caster, pTarget ) == true )
		{
//			if( pTarget->getRace() == RACE_UNDEAD && pTarget->HasAuraWithNameHash( SPELL_HASH_LICHBORNE, 0, AURA_SEARCH_POSITIVE ) ) 
			if( pTarget->HasAuraWithNameHash( SPELL_HASH_LICHBORNE, 0, AURA_SEARCH_POSITIVE ) ) 
				CanAdd = true;
			if( pTarget->IsCreature() && SafeCreatureCast( pTarget )->GetCreatureInfo() )
			{
				uint32 Type = SafeCreatureCast( pTarget )->GetCreatureInfo()->Type;
				if( Type == DEMON || Type == UNDEAD )
					CanAdd = true;
				if( sp->p_caster && sp->p_caster->HasGlyphWithID( GLYPH_PALADIN_HOLY_WRATH ) && ( Type == DRAGONSKIN || Type == ELEMENTAL ) )
					CanAdd = true;
			}
		}
		if( CanAdd == true )
			sp->m_targetUnits[i].push_back( (*it) );
	}
}

void TO_47540_0(Spell *sp,uint8 i)
{
	//this is a nerfed version of target single enemy
	Unit * pTarget = sp->m_caster->GetMapMgr()->GetUnit( sp->m_targets.m_unitTarget );
	if( pTarget && pTarget->isAlive() && isAttackable( sp->m_caster, pTarget ) == true )
		sp->m_targetUnits[i].push_back( sp->m_targets.m_unitTarget );
}

void TO_47540_1(Spell *sp,uint8 i)
{
	if( sp->m_targetUnits[0].empty() == true && sp->GetProto()->eff[i].EffectImplicitTargetA == EFF_TARGET_SINGLE_FRIEND )
		sp->SpellTargetSingleTargetFriend( i, 0 );
}

void TO_78164_0(Spell *sp,uint8 i)
{
	TargetsList *tmpMap=&sp->m_targetUnits[i];
	// this implementation is wrong.... this one is for totems
	if( sp->u_caster != NULL )
	{
		Unit *u = sp->u_caster->GetTopOwner();
		if( u && u->IsPlayer() )
		{
			float r = sp->GetRadius(i);
			r *= r;
			Player *p = SafePlayerCast( u );

			if( p->getLevel() < 80 && IsInrange(sp->m_caster->GetPositionX(),sp->m_caster->GetPositionY(),sp->m_caster->GetPositionZ(),p,r))
				sp->SafeAddTarget(tmpMap,p->GetGUID());

			SubGroup * pGroup = p->GetGroup() ?	p->GetGroup()->GetSubGroup(p->GetSubGroup()) : 0;

			if(pGroup)
			{
				p->GetGroup()->Lock();
				for(GroupMembersSet::iterator itr = pGroup->GetGroupMembersBegin();	itr != pGroup->GetGroupMembersEnd(); ++itr)
				{
					if(!(*itr)->m_loggedInPlayer || p == (*itr)->m_loggedInPlayer) 
						continue;
					if( (*itr)->m_loggedInPlayer->getLevel() >= 80 )
						continue;
					if(IsInrange(sp->m_caster->GetPositionX(),sp->m_caster->GetPositionY(),sp->m_caster->GetPositionZ(),(*itr)->m_loggedInPlayer,r))
						sp->SafeAddTarget(tmpMap,(*itr)->m_loggedInPlayer->GetGUID());
				}
				p->GetGroup()->Unlock();
			}
		}
	}
}

void TO_85222_0(Spell *sp,uint8 i)
{
	TargetsList *tmpMap=&sp->m_targetUnits[i];
	//try to order raid members that are in front of us by their health amount
	std::map<uint32, Player *> low_hp_members;	//note that this will not gather players with same amount of HP
	if( sp->u_caster != NULL )
	{
		Unit *u = sp->u_caster->GetTopOwner();
		if( u && u->IsPlayer() )
		{
			float r = sp->GetRadius(i);
			r *= r;
			Player *p = SafePlayerCast( u );

			SubGroup * pGroup = p->GetGroup() ?	p->GetGroup()->GetSubGroup(p->GetSubGroup()) : 0;

			if(pGroup)
			{
				p->GetGroup()->Lock();
				for(GroupMembersSet::iterator itr = pGroup->GetGroupMembersBegin();	itr != pGroup->GetGroupMembersEnd(); ++itr)
				{
					if( !(*itr)->m_loggedInPlayer ) 
						continue;
					if( sp->m_spellInfo->cone_width ? sp->m_caster->isInArc( (*itr)->m_loggedInPlayer , sp->m_spellInfo->cone_width ) : sp->m_caster->isInFront((*itr)->m_loggedInPlayer) ) // !!! is the target within our cone ?
					{
						if(IsInrange(sp->m_caster->GetPositionX(),sp->m_caster->GetPositionY(),sp->m_caster->GetPositionZ(),(*itr)->m_loggedInPlayer,r))
							sp->SafeAddTarget(tmpMap,(*itr)->m_loggedInPlayer->GetGUID());
						low_hp_members[ (*itr)->m_loggedInPlayer->GetMaxHealth() - (*itr)->m_loggedInPlayer->GetHealth() ] = (*itr)->m_loggedInPlayer;
					}
				}
				p->GetGroup()->Unlock();
			}
		}
		int32 jumps = sp->GetProto()->GetMaxTargets();
		if((sp->GetProto()->c_is_flags & SPELL_FLAG_IS_REQUIRING_SM_MOD) && sp->u_caster->SM_Mods )
			SM_FIValue(sp->u_caster->SM_Mods->SM_FAdditionalTargets,&jumps,sp->GetProto()->GetSpellGroupType());
		std::map<uint32, Player *>::iterator itr;
		for( itr = low_hp_members.end(); itr != low_hp_members.begin() && jumps !=0 ; )
		{
			itr--;
			jumps--;
			sp->SafeAddTarget(tmpMap,itr->second->GetGUID());
		}
	}
}

void TO_78675_1(Spell *sp,uint8 i)
{
	if( sp->u_caster )
	{
		Unit *sel = sp->u_caster->GetMapMgr()->GetUnit( sp->u_caster->GetTargetGUID() );
		if( sel )
		{
			sp->m_targets.m_targetMask = TARGET_FLAG_DEST_LOCATION;
			sp->m_targets.m_destX = sel->GetPositionX();
			sp->m_targets.m_destY = sel->GetPositionY();
			sp->m_targets.m_destZ = sel->GetPositionZ();
		}
	}
}

/*
void TO_55233_0(Spell *sp,uint8 i)
{
	if( sp->p_caster && sp->p_caster->HasAuraWithNameHash( SPELL_HASH_ITEM___DEATH_KNIGHT_T13_BLOOD_4P_BONUS, 0, AURA_SEARCH_PASSIVE ) )
		sp->SpellTargetAllRaid( 0, 0 );
}

void TO_22842_0(Spell *sp,uint8 i)
{
	if( sp->p_caster && sp->p_caster->HasAuraWithNameHash( SPELL_HASH_ITEM___DRUID_T13_FERAL_4P_BONUS__FRENZIED_REGENERATION_AND_STAMPEDE_, 0, AURA_SEARCH_PASSIVE ) )
		sp->SpellTargetAllRaid( 0, 0 );
}*/

void TO_50315_0(Spell *sp,uint8 i)
{
	sp->m_caster->AquireInrangeLock(); //make sure to release lock before exit function !
	InrangeLoopExitAutoCallback AutoLock;
	for (InRangePlayerSetRecProt::iterator itrPlr = sp->m_caster->GetInRangePlayerSetBegin( AutoLock ); itrPlr != sp->m_caster->GetInRangePlayerSetEnd(); ++itrPlr)
	{
		// don't add objects that are not units and that are dead
		if( (*itrPlr)->isAlive() == false )
			continue;
		if( IsInrange( (*itrPlr), sp->m_caster, 30.0f*30.0f ) == false )
			continue;
		sp->SafeAddTarget( &sp->m_targetUnits[i], (*itrPlr)->GetGUID());
	}
	sp->m_caster->ReleaseInrangeLock();
}

void TO_527_1(Spell *sp,uint8 i)
{
	//only target ourself if we did not manage to dispel enemy
	if( sp->m_targetUnits[0].empty() && sp->u_caster )
		sp->SafeAddTarget( &sp->m_targetUnits[i], sp->u_caster->GetGUID());
}

void TO_5728_2(Spell *sp,uint8 i)
{
	Player *pcaster = NULL;
	if( sp->u_caster )
	{
		Unit *uo = sp->u_caster->GetTopOwner();
		if( uo->IsPlayer() )
			pcaster = SafePlayerCast( uo );
	}
	if( pcaster )
	{
//		sp->SafeAddTarget( &sp->m_targetUnits[i], sp->p_caster->GetGUID());
		//note : pos 0 is earth totem = our stone totem that we will not shield
		for(uint32 x=1;x<4;x++)
			if( pcaster->m_TotemSlots[x] )
				sp->SafeAddTarget( &sp->m_targetUnits[i], pcaster->m_TotemSlots[x]->GetGUID());
	}
}

void TO_105367_0(Spell *sp,uint8 i)
{
	//try to find a crystal conductor near us. We target at least 1 player to jump first
	bool FoundConductor = false;
	if( sp->p_caster != NULL )
	{
		Unit *Target = NULL;
		float best_dist = 9999.0f;
		InRangeSetRecProt::iterator itr,itr2;
		sp->m_caster->AquireInrangeLock(); //make sure to release lock before exit function !
		InrangeLoopExitAutoCallback AutoLock;
		for( itr2 = sp->m_caster->GetInRangeSetBegin( AutoLock ); itr2 != sp->m_caster->GetInRangeSetEnd();)
		{
			itr = itr2;
			itr2++;
			if( !(*itr)->IsUnit() || !SafeUnitCast((*itr))->isAlive() )
				continue;
			if( (*itr)->GetEntry() != 56165 )
				continue;
			float tdist = sp->u_caster->GetDistance2dSq( (*itr) );
			if( tdist > 10.0f * 10.0f )
				continue;
			FoundConductor = true;
			//note that we have 0 target for chain cast = no more jumps
			sp->SafeAddTarget( &sp->m_targetUnits[i], (*itr)->GetGUID());
			sp->forced_pct_mod[i] = 100 * 100;
			sp->forced_basepoints[i] = SafeUnitCast((*itr))->GetMaxHealth() * 10 / 100;
			sp->p_caster->DealDamage( SafeUnitCast((*itr)), SafeUnitCast((*itr))->GetMaxHealth() * 20 / 100, sp->GetProto()->Id );
			break;
		}
		sp->m_caster->ReleaseInrangeLock();
	}
	//if not then arc to nearest player that was not the source of this cast
	if( FoundConductor == false )
	{
		float BestDistance = 10000000.0f;
		Player *ClosestPlayer = NULL;
		sp->m_caster->AquireInrangeLock(); //make sure to release lock before exit function !
		InrangeLoopExitAutoCallback AutoLock;
		for (InRangePlayerSetRecProt::iterator itrPlr = sp->m_caster->GetInRangePlayerSetBegin( AutoLock ); itrPlr != sp->m_caster->GetInRangePlayerSetEnd(); ++itrPlr)
		{
			// don't add objects that are not units and that are dead
			if( (*itrPlr)->isAlive() == false )
				continue;
			if( (*itrPlr)->GetCurrentSpell() != NULL && (*itrPlr)->GetCurrentSpell()->GetProto()->Id == sp->GetProto()->Id )
				continue;
			float tdist = sp->u_caster->GetDistance2dSq( (*itrPlr) );
			if( tdist < 10.0f * 10.0f && tdist < BestDistance )
			{
				tdist = BestDistance;
				ClosestPlayer = (*itrPlr);
			}
		}
		sp->m_caster->ReleaseInrangeLock();
		if( ClosestPlayer )
		{
//			sp->SafeAddTarget( &sp->m_targetUnits[i], ClosestPlayer->GetGUID());
			sp->SafeAddTarget( &sp->m_targetUnits[0], ClosestPlayer->GetGUID());
			sp->SafeAddTarget( &sp->m_targetUnits[1], ClosestPlayer->GetGUID());
		}
	}
}

void TO_1725_0(Spell *sp,uint8 i)
{
//	if( sp->u_caster && sp->u_caster->CombatStatus.IsInCombat() )
//		return;
	//fill targets normally
	sp->SpellTargetAreaOfEffect( i, 0 );	
	TargetsList t = sp->m_targetUnits[i];
	sp->m_targetUnits[i].clear();
	//now remove those that are in combat
	TargetsList::iterator l;
	for(l=t.begin();l!=t.end();l++)
	{
		Unit *u = sp->m_caster->GetMapMgr()->GetUnit( *l );
		if( u == NULL || u->CombatStatus.IsInCombat() )
			continue;
		sp->m_targetUnits[i].push_back( *l );
	}
}

void TO_55328_1(Spell *sp,uint8 i)
{
	Unit *to;
	if( sp->u_caster )
		to = sp->u_caster->GetTopOwner();
	else
		to = NULL;
	if( to->IsPlayer() )
	{
		for( uint32 j=0;j<4;j++ )
			if( SafePlayerCast( to )->m_TotemSlots[j] )
				sp->SafeAddTarget( &sp->m_targetUnits[i], SafePlayerCast( to )->m_TotemSlots[j]->GetGUID() );
		sp->SafeAddTarget( &sp->m_targetUnits[i], sp->m_caster->GetGUID() );
	}
}

void TO_81262_0(Spell *sp,uint8 i)
{
	if( sp->u_caster )
	{
		Unit *sel = sp->u_caster->GetMapMgr()->GetUnit( sp->u_caster->GetTargetGUID() );
		if( sel && isAttackable( sp->u_caster, sel ) == false )
		{
			sp->m_targets.m_targetMask = TARGET_FLAG_DEST_LOCATION;
			sp->m_targets.m_destX = sel->GetPositionX();
			sp->m_targets.m_destY = sel->GetPositionY();
			sp->m_targets.m_destZ = sel->GetPositionZ();
		}
		else
		{
			sp->m_targets.m_targetMask = TARGET_FLAG_DEST_LOCATION;
			sp->m_targets.m_destX = sp->u_caster->GetPositionX();
			sp->m_targets.m_destY = sp->u_caster->GetPositionY();
			sp->m_targets.m_destZ = sp->u_caster->GetPositionZ();
		}
	}
}

void TO_871_0(Spell *sp,uint8 i)
{
	if( sp->pSpellId )
		sp->SafeAddTarget( &sp->m_targetUnits[i], sp->m_targets.m_unitTarget );
	else
	{
#ifdef _DEBUG
		ASSERT( sp->GetProto()->eff[i].EffectImplicitTargetA == EFF_TARGET_SELF );
#endif
		sp->SafeAddTarget( &sp->m_targetUnits[i], sp->m_caster->GetGUID() );
	}
}

void TO_70940_0(Spell *sp,uint8 i)
{
	//generic aquisition
	uint32 TypeA = sp->m_spellInfo->eff[i].EffectImplicitTargetA;
	if( TypeA < EFF_TARGET_LIST_LENGTH_MARKER )
		(sp->*SpellTargetHandler[TypeA])(i, 0);
	//remove caster from the list
	for(TargetsList::iterator j = sp->m_targetUnits[i].begin();j!=sp->m_targetUnits[i].end();j++)
		if( (*j) == sp->m_caster->GetGUID() )
		{
			sp->m_targetUnits[i].erase( j );
			break;
		}
}

uint32 CTFILTERH_76577( Spell *sp, Object *Caster, Object *FilterOwner, SpellCanTargetedScript *cts )
{
	//not single target spell ?
	if( Caster == NULL || Caster->IsUnit() == false )
		return SPELL_DID_HIT_SUCCESS;
	//AOE can pass through
	if( sp->GetProto()->c_is_flags2 & SPELL_FLAG2_IS_ENEMY_AOE_TARGETTING )
		return SPELL_DID_HIT_SUCCESS;
	//AOE can pass through
	if( sp->GetProto()->c_is_flags3 & SPELL_FLAG3_IS_FRIENDLY_AOE_TARGETING )
		return SPELL_DID_HIT_SUCCESS;
	//unsure case
	if( ( sp->GetProto()->c_is_flags3 & SPELL_FLAG3_IS_DIRECT_TARGET ) == 0 )
		return SPELL_DID_HIT_SUCCESS;
	//smoke caster group can cast on enemy inside smoke
	if( SafeUnitCast( Caster )->GetGroup() != NULL && SafeUnitCast( Caster )->GetGroup()->GetID() == cts->CasterGroupId && isAttackable( Caster, FilterOwner )  )
		return SPELL_DID_HIT_SUCCESS;
	//check if caster has this same aura also
	Aura *a = SafeUnitCast( Caster )->HasAura( cts->OwnerSpell->Id );
	if( a )
		return SPELL_DID_HIT_SUCCESS;
	return SPELL_DID_HIT_MISS;
}

void TO_8349_1(Spell *sp,uint8 i)
{
	uint32 TypeA = sp->m_spellInfo->eff[i].EffectImplicitTargetA;
	if( TypeA < EFF_TARGET_LIST_LENGTH_MARKER )
		(sp->*SpellTargetHandler[TypeA])(i, 0);
	//remove main target from the list
	for(TargetsList::iterator j = sp->m_targetUnits[i].begin();j!=sp->m_targetUnits[i].end();j++)
		if( (*j) == sp->m_targets.m_unitTarget )
		{
			sp->m_targetUnits[i].erase( j );
			break;
		}
}

void TO_59754_0(Spell *sp,uint8 i)
{
	uint32 TypeA = sp->m_spellInfo->eff[i].EffectImplicitTargetA;
	if( TypeA < EFF_TARGET_LIST_LENGTH_MARKER )
		(sp->*SpellTargetHandler[TypeA])(i, 0);
	//party members except self
	for(TargetsList::iterator j = sp->m_targetUnits[i].begin();j!=sp->m_targetUnits[i].end();j++)
		if( (*j) == sp->m_caster->GetGUID() )
		{
			sp->m_targetUnits[i].erase( j );
			break;
		}
}

void TO_51640_0(Spell *sp,uint8 i)
{
	sp->m_targetUnits[i].push_back( sp->p_caster->GetSelection() );
}

void TO_42340_0(Spell *sp,uint8 i)
{
	TargetsList *tmpMap=&sp->m_targetUnits[i];

	float bestdist = 99999999999.0f;
	Unit *BestFire = NULL;
	InRangeSetRecProt::iterator itr,itr2;
	sp->m_caster->AquireInrangeLock(); //make sure to release lock before exit function !
	InrangeLoopExitAutoCallback AutoLock;
	for( itr2 = sp->m_caster->GetInRangeSetBegin( AutoLock ); itr2 != sp->m_caster->GetInRangeSetEnd(); )
	{
		itr = itr2;
		itr2++;
		if( !(*itr)->IsUnit() )
			continue;
		if( (*itr)->GetEntry() != 23537 )
			continue;
		float dist = (*itr)->GetDistanceSq( sp->m_targets.m_destX, sp->m_targets.m_destY, sp->m_targets.m_destZ );
		if( dist < 8.0f * 8.0f && bestdist > dist )
		{
			bestdist = dist;
			BestFire = SafeUnitCast( (*itr) ) ;
		}
	}
	sp->m_caster->ReleaseInrangeLock();
		//our creations are always friendly
	if( BestFire )
		sp->SafeAddTarget(tmpMap,BestFire->GetGUID() );
} 

void RegisterTargetingOverride()
{
	SpellEntry *sp;
	//Throw Bucket
	REGISTER_HANDLER( 42340, 0 );	
	//Taunt Flag
	REGISTER_HANDLER( 51640, 0 );	
	//Glyph of Rune Tap
	REGISTER_HANDLER( 59754, 0 );	
	//Smoke Bomb
	REGISTER_FILTERHANDLER( 76577 );	
	//Divine Guardian
	REGISTER_HANDLER( 70940, 0 );	
	//Shield Wall
	REGISTER_HANDLER( 871, 0 );	
	//Efflorescence
	REGISTER_HANDLER( 81262, 0 );	
	//Stoneclaw Totem - shield
	REGISTER_HANDLER( 55328, 1 );	
	//Distract
	REGISTER_HANDLER( 1725, 0 );	
	//Cristal conductor - lightning chain - hagara encounter for dragon soul raid
	REGISTER_HANDLER( 105367, 0 );	
	REGISTER_SAME_INDEX_HANDLER( 105367, 0, 1);
	//Stoneclaw Totem
	REGISTER_HANDLER( 5728, 2 );	
	//Dispel Magic
	REGISTER_HANDLER( 527, 1 );	
	//Disco Ball
	REGISTER_HANDLER( 50315, 0 );	
	//Frenzied Regeneration
//	REGISTER_HANDLER( 22842, 0 );	
//	REGISTER_SAME_INDEX_HANDLER( 22842, 0, 1);
//	REGISTER_SAME_INDEX_HANDLER( 22842, 0, 2);
	//Vampiric Blood
//	REGISTER_HANDLER( 55233, 0 );	
//	REGISTER_SAME_INDEX_HANDLER( 55233, 0, 1);
	//Solar Beam
	REGISTER_HANDLER( 78675, 1 );	
	//Speed of Light
	REGISTER_HANDLER( 85222, 0 );	
	//Drums of the Wild
	REGISTER_HANDLER( 78164, 0 );	
	//Penance should not heal if it is already doing dmg
	REGISTER_HANDLER( 47540, 0 );	
	REGISTER_HANDLER( 47540, 1 );	
	//Holy Wrath
	REGISTER_HANDLER( 2812, 1 );	
	//Flame Orb
//	REGISTER_HANDLER( 82690, 0 );	
	//Solar Beam - this spell is ok, we are only scripting it until i find how to detect when to spawn the AA at target location instead caster
	REGISTER_HANDLER( 81261, 2 );	
	//Deathfrost
	REGISTER_HANDLER( 46579, 1 );	
	//hysteria - self target to dmg - this is actually protection in case some smartass ads atttack check
//	REGISTER_HANDLER( 49016, 1 );	
	//death coil only heals undead
	REGISTER_HANDLER( 47541, 1 );	
	//fire nova triggers at totem location and not caster 
//	REGISTER_HANDLER( 1535, 0 );
	//do not target main target, only around him
	REGISTER_HANDLER( 8349, 1 );
	//killing spree
	REGISTER_HANDLER( 51690, 1 );	//we need exactly N targets :P 
	//Shattering Throw
	REGISTER_HANDLER( 64382, 0 );	//targets immune targets also
	REGISTER_SAME_INDEX_HANDLER( 64382, 0, 1);
	//Death Pact
	REGISTER_HANDLER( 48743, 1 );	//kill minions
}
