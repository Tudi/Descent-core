#include "StdAfx.h"

#define REGISTER_HANDLER( spell ) sp = dbcSpell.LookupEntryForced( spell ); \
	ASSERT( sp->CanCastHandler == 0 ); \
	sp->CanCastHandler = &CCH_##spell;
#define REGISTER_SAMEHANDLER(spell,copy_from) sp = dbcSpell.LookupEntryForced( spell ); \
	ASSERT( sp->CanCastHandler == 0 ); \
	sp->CanCastHandler = &CCH_##copy_from;

#define REGISTER_FILTERHANDLER( spell ) sp = dbcSpell.LookupEntryForced( spell ); \
	ASSERT( sp->CanCastFilterHandler == 0 ); \
	sp->CanCastFilterHandler = &CCFILTERH_##spell;
#define REGISTER_SAMEFILTERHANDLER(spell,copy_from) sp = dbcSpell.LookupEntryForced( spell ); \
	ASSERT( sp->CanCastFilterHandler == 0 ); \
	sp->CanCastFilterHandler = &CCFILTERH_##copy_from;

uint32 CCH_642(Spell *sp)
{
	//forbearance is casted after shielding target, avanging wrath cannot be casted for a while after shielding
	//this is also handled client side...so relax...take a coke
//	if( sp->u_caster && sp->u_caster->HasAura( 25771 ) )
//		return SPELL_FAILED_BAD_TARGETS;
	Unit * pTarget = sp->m_caster->GetMapMgr()->GetUnit( sp->m_targets.m_unitTarget );
	if( pTarget && pTarget->HasAura( 25771 ) )
		return SPELL_FAILED_BAD_TARGETS;
	return SPELL_CANCAST_OK;
}
/*
uint32 CCH_1134(Spell *sp)
{
	if( sp->u_caster && sp->u_caster->GetPower( POWER_TYPE_RAGE ) < 750 )
		return SPELL_FAILED_NO_POWER;
	sp->u_caster->ModPower( POWER_TYPE_RAGE, -750 );//steal the cookies
	return SPELL_CANCAST_OK;
} */

uint32 CCH_47541(Spell *sp)
{
	Unit * pTarget = sp->m_caster->GetMapMgr()->GetUnit( sp->m_targets.m_unitTarget );
	if( pTarget && pTarget->isAlive() && isAttackable( sp->m_caster, pTarget ) == false )
	{
		//some say that undead race is "forsaken" as type not undead
//		if( pTarget->IsPlayer() && ( pTarget->getRace() == RACE_UNDEAD || pTarget->HasAuraWithNameHash( SPELL_HASH_LICHBORNE, AURA_SEARCH_POSITIVE ) ) )
		if( pTarget->IsPlayer() &&  pTarget->HasAuraWithNameHash( SPELL_HASH_LICHBORNE, 0, AURA_SEARCH_POSITIVE ) )
			return SPELL_CANCAST_OK;
		if( pTarget->IsCreature() && SafeCreatureCast( pTarget )->GetCreatureInfo() && SafeCreatureCast( pTarget )->GetCreatureInfo()->Type == UNDEAD )
			return SPELL_CANCAST_OK;
	}
	if( isAttackable( sp->m_caster, pTarget ) == true )
		return SPELL_CANCAST_OK;
	return SPELL_FAILED_BAD_TARGETS;
}

uint32 CCH_80987(Spell *sp)
{
	Creature * pTarget = sp->m_caster->GetMapMgr()->GetCreature( sp->m_targets.m_unitTarget );
	if( pTarget && pTarget->GetEntry() == 43229 )
		return SPELL_CANCAST_OK;
	return SPELL_FAILED_BAD_TARGETS;
}

uint32 CCH_62599(Spell *sp)
{
	Creature * pTarget = sp->m_caster->GetMapMgr()->GetCreature( sp->m_targets.m_unitTarget );
	if( pTarget && ( pTarget->GetEntry() == 32868 || pTarget->GetEntry() == 32869 ) )
		return SPELL_CANCAST_OK;
	return SPELL_FAILED_BAD_TARGETS;
}

uint32 CCH_93773(Spell *sp)
{
	Creature * pTarget = sp->m_caster->GetMapMgr()->GetCreature( sp->m_targets.m_unitTarget );
	if( pTarget && ( pTarget->GetEntry() == 41251 ) )
		return SPELL_CANCAST_OK;
	return SPELL_FAILED_BAD_TARGETS;
}

uint32 CCH_44572(Spell *sp)
{
	//this is a nerfed version of target single enemy
	Unit * target = sp->m_caster->GetMapMgr()->GetUnit( sp->m_targets.m_unitTarget );

	if( target )
	{
		if( target->HasAuraStateFlag( AURASTATE_FROZEN ) || ( sp->p_caster && sp->p_caster->m_ForceTargetFrozen > 0 ) )
			return SPELL_CANCAST_OK;
/*		bool has_chill_effect = false;
		for(uint32 t=NEG_AURAS_START;t<MAX_NEGATIVE_AURAS1(target);t++)
			if( target->m_auras[ t ] 
				&& target->m_auras[ t ]->GetSpellProto()->c_is_flags & SPELL_FLAG_IS_CHILL_SPELL )
			{
				has_chill_effect = true;
				break;
			}

		if( has_chill_effect == false && sp->u_caster )
			has_chill_effect = (sp->u_caster->HasAuraWithNameHash( SPELL_HASH_FINGERS_OF_FROST, 0, AURA_SEARCH_POSITIVE )!=NULL);

		if( has_chill_effect == false )
			return SPELL_FAILED_BAD_TARGETS;
			*/
		if( sp->u_caster && sp->u_caster->HasAuraWithNameHash( SPELL_HASH_FINGERS_OF_FROST, 0, AURA_SEARCH_POSITIVE ) != NULL )
			return SPELL_CANCAST_OK;

		return SPELL_FAILED_BAD_TARGETS;
	}
	return SPELL_CANCAST_OK;
}

uint32 CCH_48743(Spell *sp)
{
	sp->m_targets.m_unitTarget = 0;
	if( sp->p_caster == NULL )
		return SPELL_FAILED_REAGENTS;
/*
	if(  sp->p_caster->GetSummon() == NULL )
	{
		//try to get a whatever summon
		std::list<uint64>::iterator gitr,gitr2;
		for( gitr2 = sp->p_caster->m_guardians.begin(); gitr2 != sp->p_caster->m_guardians.end();)
		{
			gitr = gitr2;
			gitr2++;
			Creature *tc = GetMapMgr()->GetCreature( (*gitr) );
			if( tc == NULL )
				m_guardians.erase( gitr );
			if( tc->isAlive() )
			{
				sp->m_targets.m_unitTarget = tc->GetGUID();
				return SPELL_CANCAST_OK;
			}
		}
	}
	else
	{
		sp->m_targets.m_unitTarget = sp->p_caster->GetSummon()->GetGUID();
		return SPELL_CANCAST_OK;
	}
	*/
	InRangeSetRecProt::iterator itr,itr2;
	sp->m_caster->AquireInrangeLock(); //make sure to release lock before exit function !
	InrangeLoopExitAutoCallback AutoLock;
	for( itr2 = sp->m_caster->GetInRangeSetBegin( AutoLock ); itr2 != sp->m_caster->GetInRangeSetEnd();)
	{
		itr = itr2;
		itr2++;
		if( !(*itr)->IsUnit() 
			|| !SafeUnitCast((*itr))->isAlive()
			|| ( (*itr)->GetUInt32Value(UNIT_FIELD_SUMMONEDBY) != sp->m_caster->GetUInt32Value(OBJECT_FIELD_GUID)
				 && (*itr)->GetUInt32Value(UNIT_FIELD_CREATEDBY) != sp->m_caster->GetUInt32Value(OBJECT_FIELD_GUID) )
		    || (*itr)->GetEntry() == 27893 //Dancing Rune Weapon
			)
			continue;
		//our creations are always friendly
		sp->m_targets.m_unitTarget = (*itr)->GetGUID();
		break;
	}
	sp->m_caster->ReleaseInrangeLock();

	if( sp->m_targets.m_unitTarget != 0 )
		return SPELL_CANCAST_OK;

	return SPELL_FAILED_REAGENTS;
}

uint32 CCH_32146(Spell *sp)
{
	Creature *corpse = sp->m_caster->GetMapMgr()->GetInterface()->GetCreatureNearestCoords( sp->m_caster->GetPositionX(), sp->m_caster->GetPositionY(), sp->m_caster->GetPositionZ(), 18240);
	if( corpse != NULL )
		if( sp->m_caster->CalcDistance( sp->m_caster, corpse) > 5)
		{ 
			return SPELL_FAILED_NOT_HERE;
		}
	return SPELL_CANCAST_OK;
}

uint32 CCH_605(Spell *sp)
{
	Unit *target =sp->m_caster->GetMapMgr()->GetUnit( sp->m_targets.m_unitTarget );
	if( target && ( ( target->IsPlayer() == false && target->getLevel() > 41 ) || target->HasAuraWithNameHash( SPELL_HASH_MIND_CONTROL, 0, AURA_SEARCH_NEGATIVE ) ) )
	{ 
		return SPELL_FAILED_BAD_TARGETS;
	}
	return SPELL_CANCAST_OK;
}

uint32 CCH_37830(Spell *sp)
{
	if( sp->m_targets.m_unitTarget != sp->m_caster->GetGUID() )
	{ 
		return SPELL_FAILED_BAD_TARGETS;
	}
	return SPELL_CANCAST_OK;
}

uint32 CCH_39246(Spell *sp)
{
	if( sp->m_caster->GetMapMgr() == NULL )
		return SPELL_FAILED_NOT_HERE;
	Creature *cleft = sp->m_caster->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(sp->m_caster->GetPositionX(), sp->m_caster->GetPositionY(), sp->m_caster->GetPositionZ(), 22105);
	if( cleft == NULL || cleft->isAlive() == false )
	{ 
		return SPELL_FAILED_NOT_HERE;
	}
	return SPELL_CANCAST_OK;
}

uint32 CCH_30988(Spell *sp)
{
	if( sp->m_caster->GetMapMgr() == NULL )
		return SPELL_FAILED_NOT_HERE;

	Creature *corpse = sp->m_caster->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(sp->m_caster->GetPositionX(), sp->m_caster->GetPositionY(), sp->m_caster->GetPositionZ(), 17701);
	if( corpse == NULL || sp->m_caster->CalcDistance(sp->m_caster, corpse) > 5  || corpse->isAlive() )
	{ 
		return SPELL_FAILED_NOT_HERE;
	}
	return SPELL_CANCAST_OK;
}

uint32 CCH_43723(Spell *sp)
{
	if( sp->m_caster->GetMapMgr() == NULL )
		return SPELL_FAILED_NOT_HERE;

	Creature *abysal = sp->m_caster->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(sp->m_caster->GetPositionX(), sp->m_caster->GetPositionY(), sp->m_caster->GetPositionZ(), 19973);
	if(abysal != NULL)
	{
		if(!abysal->isAlive())
			if(!(sp->p_caster->GetItemInterface()->GetItemCount(31672, FALSE) > 1 && sp->p_caster->GetItemInterface()->GetItemCount(31673, FALSE) > 0 && sp->p_caster->CalcDistance(sp->p_caster, abysal) < 10))
			{ 
				return SPELL_FAILED_NOT_HERE;
			}
	}
	else
		return SPELL_FAILED_NOT_HERE;
	return SPELL_CANCAST_OK;
}

uint32 CCH_32307(Spell *sp)
{
	if( sp->m_caster->GetMapMgr() == NULL )
		return SPELL_FAILED_NOT_HERE;

	Creature *kilsorrow = sp->m_caster->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(sp->m_caster->GetPositionX(), sp->m_caster->GetPositionY(), sp->m_caster->GetPositionZ());
	if(kilsorrow == NULL || kilsorrow->isAlive() || sp->m_caster->CalcDistance(sp->m_caster, kilsorrow) > 1)
	{ 
		return SPELL_FAILED_NOT_HERE;
	}
	if(kilsorrow->GetEntry() != 17147 && kilsorrow->GetEntry() != 17148 && kilsorrow->GetEntry() != 18397 && kilsorrow->GetEntry() != 18658 && kilsorrow->GetEntry() != 17146)
	{ 
		return SPELL_FAILED_NOT_HERE;
	}

	return SPELL_CANCAST_OK;
}

uint32 CCH_9005(Spell *sp)
{
	if( sp->p_caster && ( sp->p_caster->GetShapeShift() != FORM_CAT || sp->p_caster->HasAuraWithNameHash( SPELL_HASH_PROWL, 0, AURA_SEARCH_POSITIVE ) == 0 ) )
		return SPELL_FAILED_BAD_TARGETS;

	return SPELL_CANCAST_OK;
}
/*
uint32 CCH_46584(Spell *sp)
{
	Object *corpse = NULL;
	InRangeSet::iterator itr;
	for( itr = sp->m_caster->GetInRangeSetBegin(); itr != sp->m_caster->GetInRangeSetEnd();itr++)
	{
		if( (*itr)->IsCorpse() )
		{
			corpse = (*itr);
			break;
		}
		if(!((*itr)->IsUnit()) || SafeUnitCast((*itr))->isAlive())
			continue;
		if( (*itr)->IsCreature() && SafeCreatureCast((*itr))->GetCreatureInfo()
			&& SafeCreatureCast((*itr))->GetCreatureInfo()->Type == HUMANOID )
		{
			corpse = (*itr);
			break;
		}
//				if( (*itr)->IsPlayer() )
//				{
//					corpse = (*itr);
//					break;
//				}
	}

	if( corpse )
	{
		//oh crap, what to do now ? what about global events and such ? memleak ?
		corpse->RemoveFromWorld( false );
	}
	//check if player has corpse dust
	else if( sp->p_caster && sp->p_caster->GetItemInterface()->RemoveItemAmt( 37201, 1 ) == 0 )
		return SPELL_FAILED_ITEM_GONE;

	return SPELL_CANCAST_OK;
} */

uint32 CCH_7266(Spell *sp)
{
	if( sp->p_caster == NULL )
		return SPELL_FAILED_NOT_HERE;

/* 
- stormwind
0x40204138
0x00000008 AREA_FLAG_SLAVE_CAPITAL
0x00000010 AREA_FLAG_UNK3
0x00000020 AREA_FLAG_SLAVE_CAPITAL2
0x00000100 AREA_FLAG_CAPITAL
0x00004000 AREA_FLAG_OUTLAND2
0x00200000 AREA_FLAG_TOWN
- goldshire
0x40304040
0x00000040 AREA_FLAG_UNK4
0x00004000 AREA_FLAG_OUTLAND2
0x00100000 AREA_FLAG_LOWLEVEL
0x00200000 AREA_FLAG_TOWN
- orgrimmar
0x40204138
0x00000008 AREA_FLAG_SLAVE_CAPITAL
0x00000010 AREA_FLAG_UNK3
0x00000020 AREA_FLAG_SLAVE_CAPITAL2
0x00000100 AREA_FLAG_CAPITAL
0x00004000 AREA_FLAG_OUTLAND2
0x00200000 AREA_FLAG_TOWN
*/
	AreaTable* at = dbcArea.LookupEntry( sp->p_caster->GetAreaID() );
	if(at->AreaFlags & ( AREA_FLAG_SLAVE_CAPITAL | AREA_FLAG_SLAVE_CAPITAL2 | AREA_FLAG_CAPITAL ) )
	{ 
//disabled as people wanted to be able to duel inside cities. uncomment this if you want to deny city duel
//		return SPELL_FAILED_NO_DUELING;
	}
//	if( atCaster->AreaFlags & ( AREA_FLAG_CITY | AREA_FLAG_CAPITAL | AREA_FLAG_ARENA | AREA_FLAG_ARENA_INSTANCE | AREA_PVPARENA ) )

	// instance & stealth checks
	if( sp->p_caster->GetMapMgr() && sp->p_caster->GetMapMgr()->GetMapInfo() 
		&& IS_DUEL_ENABLED_MAP( sp->p_caster->GetMapMgr()->GetMapInfo() ) == false )
	{ 
			return SPELL_FAILED_NO_DUELING;
	}
	if( sp->p_caster->IsStealth() )
	{ 
		return SPELL_FAILED_CANT_DUEL_WHILE_STEALTHED;
	}

	return SPELL_CANCAST_OK;
}

uint32 CCH_25860(Spell *sp)
{
	if( sp->p_caster == NULL || sp->p_caster->IsMounted() == false )
		return SPELL_FAILED_ONLY_MOUNTED;

	return SPELL_CANCAST_OK;
}

uint32 CCH_2825(Spell *sp)
{
	Unit * target = sp->m_caster->GetMapMgr()->GetUnit( sp->m_targets.m_unitTarget );
	if( target == NULL )
		return SPELL_FAILED_BAD_TARGETS;
	if( target->HasAuraWithNameHash( SPELL_HASH_SATED, 0, AURA_SEARCH_NEGATIVE ) )
		return SPELL_FAILED_NOT_READY;	//i wonder what is correct error code
	if( target->HasAuraWithNameHash( SPELL_HASH_EXHAUSTION, 0, AURA_SEARCH_NEGATIVE ) )
		return SPELL_FAILED_NOT_READY;	//i wonder what is correct error code
	if( target->HasAuraWithNameHash( SPELL_HASH_INSANITY, 0, AURA_SEARCH_NEGATIVE ) )
		return SPELL_FAILED_NOT_READY;	//i wonder what is correct error code
	if( target->HasAuraWithNameHash( SPELL_HASH_TEMPORAL_DISPLACEMENT, 0, AURA_SEARCH_NEGATIVE ) )
		return SPELL_FAILED_NOT_READY;	//i wonder what is correct error code

	return SPELL_CANCAST_OK;
}

uint32 CCH_61336(Spell *sp)
{
	Unit * target = sp->m_caster->GetMapMgr()->GetUnit( sp->m_targets.m_unitTarget );
	if( target == NULL )
		return SPELL_FAILED_BAD_TARGETS;
	if( target->IsPlayer() && SafePlayerCast(target)->IsInFeralForm() == false )
		return SPELL_FAILED_ONLY_SHAPESHIFT;

	return SPELL_CANCAST_OK;
}

uint32 CCH_1515(Spell *sp)
{
	Unit * target = sp->m_caster->GetMapMgr()->GetUnit( sp->m_targets.m_unitTarget );
	if( target == NULL )
		return SPELL_FAILED_BAD_TARGETS;
	Player *p_caster = sp->p_caster;

	if( !p_caster )
		return PETTAME_INVALIDCREATURE;
	uint8 result = 0;
	Unit* tgt = target;
	if( tgt == NULL )
	{
		// we have to pick a target manually as this is a dummy spell which triggers tame effect at end of channeling
		if( p_caster->GetSelection() != 0 )
			tgt =  p_caster->GetMapMgr()->GetUnit( p_caster->GetSelection() );
		else
		{
			p_caster->BroadcastMessage("Spell could not find a target to tame. Select one " );
			return SPELL_FAILED_UNKNOWN;
		}
	}
	if( tgt == NULL )
	{
		p_caster->BroadcastMessage("Spell could not find a target to tame. Select one " );
		return PETTAME_INVALIDCREATURE;
	}

	Creature *tame = tgt->GetTypeId() == TYPEID_UNIT ? SafeCreatureCast( tgt ) : NULL;

	if ( tame == NULL )
	{
		p_caster->BroadcastMessage("Selection is not a creature " );
		result = PETTAME_INVALIDCREATURE;
	}
	else if( !tame->isAlive() )
	{
		p_caster->BroadcastMessage("Cannot tame dead targets" );
		result = PETTAME_DEAD;
	}
	else if( tame->IsPet() )
	{
		p_caster->BroadcastMessage("Target is already a pet" );
		result = PETTAME_CREATUREALREADYOWNED;
	}
	else if( !tame->GetCreatureInfo() || tame->GetCreatureInfo()->Type != BEAST || !tame->GetCreatureInfo()->Family )
	{
		p_caster->BroadcastMessage("Target is not considered a beast or family info is missing" );
		result = PETTAME_NOTTAMEABLE;
	}
	else if( !p_caster->isAlive() || p_caster->getClass() != HUNTER )
	{
		p_caster->BroadcastMessage("You cannot tame while dead or not a hunter" );
		result = PETTAME_UNITSCANTTAME;
	}
	else if( tame->getLevel() > p_caster->getLevel() + 5 )
	{
		p_caster->BroadcastMessage("Selected creature level is too high" );
		result = PETTAME_TOOHIGHLEVEL;
	}
	else if( p_caster->GetUnstabledPetNumber() > MAX_ALLOWED_UNSTABLED_PETS )
	{
		p_caster->BroadcastMessage("You already have an unstabled pet. Abandon or stable him in order to get another one");
		result = PETTAME_ANOTHERSUMMONACTIVE;
	}
	else if( p_caster->GetPetCount() >= MAX_ALLOWED_TOTAL_PETS )
	{
		p_caster->BroadcastMessage("You cannot have more pets" );
		result = PETTAME_TOOMANY;
	}
	//cannot tame exotic beasts
	else if( tame->GetCreatureInfo()->IsExotic() == true && p_caster->HasAuraWithNameHash( SPELL_HASH_BEAST_MASTERY, 0, AURA_SEARCH_PASSIVE ) == false )
	{
		p_caster->BroadcastMessage("You need to spec beast mastery talent tree to tame exotic pets" );
		result = PETTAME_CANTCONTROLEXOTIC;
	}
	else
	{
		CreatureFamilyEntry* cf = dbcCreatureFamily.LookupEntry( tame->GetCreatureInfo()->Family );
		if( cf && !cf->tameable )
		{
			p_caster->BroadcastMessage("Creature Family info is missing. Report this to devs" );
			result = PETTAME_NOTTAMEABLE;

		}
	}
	if( result != 0 )
	{
		sp->SendTameFailure( result );
		return SPELL_FAILED_DONT_REPORT;
	}

	return SPELL_CANCAST_OK;
}

uint32 CCH_13907(Spell *sp)
{
	Unit * target = sp->m_caster->GetMapMgr()->GetUnit( sp->m_targets.m_unitTarget );
	if( target == NULL )
		return SPELL_FAILED_BAD_TARGETS;
	if ( target->IsCreature() == false )
	{ 
		return SPELL_FAILED_SPELL_UNAVAILABLE;
	}
	CreatureInfo* inf = SafeCreatureCast( target )->GetCreatureInfo();
	if( inf == NULL || inf->Type != DEMON )
		return false;

	return SPELL_CANCAST_OK;
}

uint32 CCH_1850(Spell *sp)
{
	Unit * target = sp->m_caster->GetMapMgr()->GetUnit( sp->m_targets.m_unitTarget );
	if( target == NULL )
		return SPELL_FAILED_BAD_TARGETS;
	Player *p_caster = sp->p_caster;

	if( !p_caster )
		return SPELL_FAILED_ONLY_SHAPESHIFT;

	if(p_caster && p_caster->GetShapeShift() != FORM_CAT)
	{ 
		return SPELL_FAILED_ONLY_SHAPESHIFT;
	}

	return SPELL_CANCAST_OK;
}

uint32 CCH_982(Spell *sp)
{
	Unit * target = sp->m_caster->GetMapMgr()->GetUnit( sp->m_targets.m_unitTarget );
	if( target == NULL )
		return SPELL_FAILED_BAD_TARGETS;
	Player *p_caster = sp->p_caster;

	if( !p_caster )
		return SPELL_FAILED_BAD_TARGETS;

	Pet *pPet = p_caster->GetSummon();
	if( pPet == NULL || !pPet->IsDead() )
	{ 
		return SPELL_FAILED_TARGET_NOT_DEAD;
	}

	return SPELL_CANCAST_OK;
}

uint32 CCH_7620(Spell *sp)
{
	uint32 entry = sp->GetProto()->eff[0].EffectMiscValue;
	if(entry == GO_FISHING_BOBBER)
	{
		//uint32 mapid = p_caster->GetMapId();
		float px=sp->u_caster->GetPositionX();
		float py=sp->u_caster->GetPositionY();
		//float pz=u_caster->GetPositionZ();
		float orient = sp->m_caster->GetOrientation();
		float posx = 0,posy = 0,posz = 0;
		float co = cos(orient);
		float si = sin(orient);
		MapMgr * map = sp->m_caster->GetMapMgr();

		float r;
		float land_height;
		for(r=20; r>10; r--)
		{
			posx = px + r * co;
			posy = py + r * si;
			if( (sTerrainMgr.GetLiquidFlags( sp->u_caster->GetMapId(), posx, posy, INVALID_HEIGHT ) & MAP_LIQUID_TYPE_WATER) == 0  )//water
				continue;
			posz = sTerrainMgr.GetLiquidHeight(sp->u_caster->GetMapId(),posx,posy, 0.0f );
			if( posz == VMAP_VALUE_NOT_INITIALIZED )
				continue;
			land_height = map->GetLandHeight(posx,posy);
			if( posz > land_height )//water
				break;
		}
		if( r<=10 || posz <= land_height )
		{ 
			return SPELL_FAILED_NOT_FISHABLE;
		}

		// if we are already fishing, dont cast it again
		if(sp->p_caster->GetSummonedObject())
			if(sp->p_caster->GetSummonedObject()->GetEntry() == GO_FISHING_BOBBER)
			{ 
				return SPELL_FAILED_SPELL_IN_PROGRESS;
			}
		sp->m_targets.m_destX = posx;
		sp->m_targets.m_destY = posy;
		sp->m_targets.m_destZ = posz;
	}
	return SPELL_CANCAST_OK;
}

uint32 CCH_24275(Spell *sp)
{
	Unit * target = sp->m_caster->GetMapMgr()->GetUnit( sp->m_targets.m_unitTarget );
	if( target == NULL )
		return SPELL_FAILED_BAD_TARGETS;

	if( target->GetHealthPct() > 20 
		&& sp->u_caster
		&& sp->u_caster->HasAura( 57318 ) == false	//this is Sanctified Wrath cast enabler
		)
	{ 
		 return SPELL_FAILED_BAD_TARGETS;
	}

	return SPELL_CANCAST_OK;
}

uint32 CCH_18562(Spell *sp)
{
	Unit * target = sp->m_caster->GetMapMgr()->GetUnit( sp->m_targets.m_unitTarget );
	if( target == NULL )
		return SPELL_FAILED_BAD_TARGETS;

	if( target->HasAuraWithNameHash( SPELL_HASH_REGROWTH, 0, AURA_SEARCH_POSITIVE ) != NULL
		|| target->HasAuraWithNameHash( SPELL_HASH_REJUVENATION, 0, AURA_SEARCH_POSITIVE ) != NULL
		)
	{ 
		return SPELL_CANCAST_OK;
	}

	return SPELL_FAILED_BAD_TARGETS;
}

uint32 CCH_89821(Spell *sp)
{
	Creature * pTarget = sp->m_caster->GetMapMgr()->GetCreature( sp->m_targets.m_unitTarget );
	if( pTarget && pTarget->GetEntry() == 48269 )
		return SPELL_CANCAST_OK;
	return SPELL_FAILED_BAD_TARGETS;
}

uint32 CCH_65455(Spell *sp)
{
	Creature * pTarget = sp->m_caster->GetMapMgr()->GetCreature( sp->m_targets.m_unitTarget );
	if( pTarget && pTarget->GetEntry() == 1989 )
		return SPELL_CANCAST_OK;
	return SPELL_FAILED_BAD_TARGETS;
}

uint32 CCH_77682(Spell *sp)
{
	Creature * pTarget = sp->m_caster->GetMapMgr()->GetCreature( sp->m_targets.m_unitTarget );
	if( pTarget && pTarget->GetEntry() == 41520 )
		return SPELL_CANCAST_OK;
	return SPELL_FAILED_BAD_TARGETS;
}

uint32 CCH_20473(Spell *sp)
{
	Unit *pTarget = sp->m_caster->GetMapMgr()->GetUnit( sp->m_targets.m_unitTarget );
	if( pTarget )
	{
		if( isAttackable( pTarget, sp->m_caster ) && DistanceSq( pTarget, sp->m_caster ) > 20.0f*20.0f )
			return SPELL_FAILED_OUT_OF_RANGE;
		else if( DistanceSq( pTarget, sp->m_caster ) > 40.0f*40.0f )
			return SPELL_FAILED_OUT_OF_RANGE;
	}
	return SPELL_CANCAST_OK;
}

uint32 CCH_97670(Spell *sp)
{
	Creature * pTarget = sp->m_caster->GetMapMgr()->GetCreature( sp->m_targets.m_unitTarget );
	if( pTarget && pTarget->GetEntry() == 52834 )
		return SPELL_CANCAST_OK;
	return SPELL_FAILED_BAD_TARGETS;
}

uint32 CCH_33206(Spell *sp)
{
	if( sp->p_caster )
	{
		if( sp->p_caster->IsFeared() == false )
			return SPELL_CANCAST_OK;
		if( sp->p_caster->HasGlyphWithID( GLYPH_PRIEST_PAIN_SUPRESSION ) == true )
			return SPELL_CANCAST_OK_NOMORE_CHECKS;
		return SPELL_FAILED_NOT_IN_CONTROL;
	}
	return SPELL_CANCAST_OK;
}

uint32 CCH_75554(Spell *sp)
{
	if( sp->p_caster )
	{
		uint32 area_id = sp->p_caster->GetAreaID();
		//need to be in mount hyjal near NPC http://www.wowhead.com/npc=39726
		if( sp->p_caster->GetMapId() != 1 || area_id != 4998 )
			return SPELL_FAILED_NOT_HERE;
	}
	return SPELL_CANCAST_OK;
}

uint32 CCH_50979(Spell *sp)
{
	Creature * pTarget = sp->m_caster->GetMapMgr()->GetCreature( sp->m_targets.m_unitTarget );
	if( pTarget && pTarget->GetEntry() == 28213 )
		return SPELL_CANCAST_OK;
	return SPELL_FAILED_BAD_TARGETS;
}

uint32 CCH_48020(Spell *sp)
{
	if( sp->p_caster == NULL )
		return SPELL_FAILED_BAD_TARGETS;
	GameObject *DemonicCircle = sp->p_caster->GetMapMgr()->GetGameObject( sp->p_caster->m_ObjectSlots[0] );
	if( DemonicCircle == NULL )
		return SPELL_FAILED_BAD_TARGETS;
	if( DemonicCircle->GetDistance2dSq( sp->p_caster ) > 40*40 )
		return SPELL_FAILED_OUT_OF_RANGE;
	return SPELL_CANCAST_OK;
}

uint32 CCH_17962(Spell *sp)
{
	Unit * pTarget = sp->m_caster->GetMapMgr()->GetUnit( sp->m_targets.m_unitTarget );
	if( pTarget && pTarget->HasAuraWithNameHash( SPELL_HASH_IMMOLATE, 0, AURA_SEARCH_NEGATIVE ) )
		return SPELL_CANCAST_OK;
	return SPELL_FAILED_BAD_TARGETS;
}

uint32 CCH_20252(Spell *sp)
{
	if( sp->u_caster->m_rooted > 0 )
		return SPELL_FAILED_ROOTED;
	return SPELL_CANCAST_OK;
}

uint32 CCH_1776(Spell *sp)
{
	if( sp->p_caster && sp->p_caster->HasGlyphWithID( GLYPH_ROGUE_GOUGE ) )
		return SPELL_CANCAST_OK;
	if( sp->m_targets.m_unitTarget)
	{
		Unit *target = (sp->m_caster->IsInWorld()) ? sp->m_caster->GetMapMgr()->GetUnit(sp->m_targets.m_unitTarget) : NULL;

		if( target == NULL || target->isInFront( sp->u_caster ) == false )
			return SPELL_FAILED_NOT_INFRONT;
	}
	return SPELL_CANCAST_OK;
}

uint32 CCH_781(Spell *sp)
{
	if( sp->u_caster != NULL && sp->u_caster->CombatStatus.IsInCombat() == false )
		return SPELL_FAILED_NOT_HERE;
	return SPELL_CANCAST_OK;
}

uint32 CCH_1953(Spell *sp)
{
	if( sp->u_caster == NULL )
		return SPELL_FAILED_BAD_TARGETS;
	if( sp->u_caster->polySpell )
		return SPELL_FAILED_BAD_TARGETS;
	for( uint32 i=NEG_AURAS_START;i<MAX_NEGATIVE_AURAS1( sp->u_caster ); i++ )
		if( sp->u_caster->m_auras[ i ] && ( sp->u_caster->m_auras[ i ]->GetSpellProto()->c_is_flags3 & SPELL_FLAG3_IS_BLOCKING_BLINK ) )
			return SPELL_FAILED_BAD_TARGETS;
	if( sp->p_caster && sp->p_caster->m_bg && sp->p_caster->m_bg->HasStarted() == false )
		return SPELL_FAILED_NOT_IN_BATTLEGROUND;
	return SPELL_CANCAST_OK;
}

uint32 CCH_6544(Spell *sp)
{
	if( sp->u_caster->m_rooted > 0 )
		return SPELL_FAILED_ROOTED;
	if( sp->u_caster )
	{
		if( sp->m_targets.m_destZ != 0 && sp->m_targets.m_destZ - 7.0f > sp->u_caster->GetPositionZ() )
				return SPELL_FAILED_OUT_OF_RANGE;
		else if( sp->m_targets.m_srcZ != 0 && sp->m_targets.m_srcZ - 7.0f > sp->u_caster->GetPositionZ() )
				return SPELL_FAILED_OUT_OF_RANGE;

		if( sp->m_targets.m_destX != 0 && Distance2DSq( sp->u_caster->GetPositionX(), sp->u_caster->GetPositionY(), sp->m_targets.m_destX, sp->m_targets.m_destY ) < 7.0f * 7.0f )
				return SPELL_FAILED_TOO_CLOSE;
		else if( sp->m_targets.m_srcX != 0 && Distance2DSq( sp->u_caster->GetPositionX(), sp->u_caster->GetPositionY(), sp->m_targets.m_srcX, sp->m_targets.m_srcY ) < 7.0f * 7.0f )
				return SPELL_FAILED_TOO_CLOSE;
	}
	return SPELL_CANCAST_OK;
}

uint32 CCH_20577(Spell *sp)
{
	bool check = false;
	float rad = sp->GetRadius( 0 );
	rad *= rad;
	sp->m_caster->AquireInrangeLock();
	InrangeLoopExitAutoCallback AutoLock;
	for(InRangeSetRecProt::iterator i = sp->m_caster->GetInRangeSetBegin( AutoLock ); i != sp->m_caster->GetInRangeSetEnd(); ++i)
	{
		if( (*i)->IsUnit() == false )
			continue;
//		if( SafeUnitCast((*i))->getDeathState() != CORPSE )
		if( SafeUnitCast((*i))->isAlive() == true ) //even unreleased player corpses
			continue;
		if( (*i)->IsCreature() )
		{
			CreatureInfo *cn = SafeCreatureCast((*i))->GetCreatureInfo();
			if(cn && (cn->Type == HUMANOID || cn->Type == UNDEAD) )
			{
				if(sp->m_caster->GetDistance2dSq((*i)) < rad)
				{
					sp->SafeAddTarget( &sp->m_targetUnits[0], (*i)->GetGUID() );
					check = true;
					break;
				}
			}
		}
		else if( (*i)->IsPlayer() && (*i)->GetTeam() != sp->m_caster->GetTeam() )
		{
			if(sp->m_caster->GetDistance2dSq((*i)) < rad)
			{
				sp->SafeAddTarget( &sp->m_targetUnits[0], (*i)->GetGUID() );
				check = true;
				break;
			}
		}
	}
	sp->m_caster->ReleaseInrangeLock();
	
	if( check == false )
		return SPELL_FAILED_NO_EDIBLE_CORPSES;
	return SPELL_CANCAST_OK;
}

uint32 CCH_14183(Spell *sp)
{
	if( sp->u_caster == NULL || ( sp->u_caster->IsStealth() == false && sp->u_caster->HasAuraWithNameHash( SPELL_HASH_SHADOW_DANCE, 0, AURA_SEARCH_POSITIVE | AURA_SEARCH_PASSIVE ) == NULL ) )
		return SPELL_FAILED_BAD_TARGETS;
	return SPELL_CANCAST_OK;
}

uint32 CCH_51640(Spell *sp)
{
	if( sp->p_caster == NULL )
		return SPELL_FAILED_BAD_TARGETS;
	Player *Target = sp->p_caster->GetMapMgr()->GetPlayer( sp->p_caster->GetSelection() );
	if( Target == NULL || Target->isAlive() == true )
		return SPELL_FAILED_BAD_TARGETS;
	return SPELL_CANCAST_OK;
}

uint32 CCH_83967(Spell *sp)
{
	if( sp->p_caster == NULL )
		return SPELL_FAILED_BAD_TARGETS;
	if( sp->p_caster->CombatStatus.IsInCombat() == true )
		return SPELL_FAILED_BAD_TARGETS;
	if( IS_INSTANCE( sp->p_caster->GetMapId() ) != 0 )
		return SPELL_FAILED_NOT_HERE;
	return SPELL_CANCAST_OK;
}

uint32 CCH_57806(Spell *sp)
{
	Creature * pTarget = sp->m_caster->GetMapMgr()->GetCreature( sp->m_targets.m_unitTarget );
	if( pTarget == NULL || pTarget->isAlive() )
		return SPELL_FAILED_BAD_TARGETS;
	if( pTarget->GetEntry() == 30202 && pTarget->GetEntry() == 31043 ) // Reanimated Crusader
		return SPELL_CANCAST_OK;
	return SPELL_FAILED_BAD_TARGETS;
}

uint32 CCH_53271(Spell *sp)
{
	if( sp->p_caster == NULL || sp->p_caster->GetSummon() == NULL|| sp->p_caster->GetSummon()->isAlive() == false )
		return SPELL_FAILED_NO_PET;
	return SPELL_CANCAST_OK;
}
/*
uint32 CCH_30213(Spell *sp)
{
	if( sp->u_caster && ( sp->u_caster->isCasting() || sp->u_caster->HasAuraWithNameHash( SPELL_HASH_FELSTORM, 0, AURA_SEARCH_POSITIVE ) ) )
		return SPELL_FAILED_BAD_TARGETS;
	return SPELL_CANCAST_OK;
}*/

uint32 CCH_5308(Spell *sp)
{
	Unit * pTarget = sp->m_caster->GetMapMgr()->GetUnit( sp->m_targets.m_unitTarget );
	if( pTarget == NULL || (int32)pTarget->GetHealthPct() > sp->GetProto()->eff[1].EffectBasePoints )
		return SPELL_FAILED_TARGET_AURASTATE;
	return SPELL_CANCAST_OK;
}

uint32 CCH_68645(Spell *sp)
{
	Unit * pTarget = sp->m_caster->GetMapMgr()->GetUnit( sp->m_targets.m_unitTarget );
	if( sp->m_targets.m_destX )
	{
		float dist = sp->m_caster->GetDistanceSq( sp->m_targets.m_destX, sp->m_targets.m_destY, sp->m_targets.m_destZ );
		float maxRange = GetMaxRange( dbcSpellRange.LookupEntryForced( sp->GetProto()->rangeIndex ) );
		if( dist > maxRange * maxRange )
			return SPELL_FAILED_OUT_OF_RANGE;
	}
	return SPELL_CANCAST_OK;
}

uint32 CCH_87296(Spell *sp)
{
	if( sp->p_caster->m_bg )
		return SPELL_FAILED_NOT_HERE;
	return SPELL_CANCAST_OK;
}

uint32 CCH_85555(Spell *sp)
{
	Creature * pTarget = sp->m_caster->GetMapMgr()->GetCreature( sp->m_targets.m_unitTarget );
	if( pTarget && pTarget->GetEntry() == 45851 )
		return SPELL_CANCAST_OK;
	return SPELL_FAILED_BAD_TARGETS;
}

uint32 CCH_64306(Spell *sp)
{
	Creature * pTarget = sp->m_caster->GetMapMgr()->GetCreature( sp->m_targets.m_unitTarget );
	if( pTarget && pTarget->GetEntry() == 33206 )
		return SPELL_CANCAST_OK;
	return SPELL_FAILED_BAD_TARGETS;
}

uint32 CCH_48427(Spell *sp)
{
	if( sp->p_caster )
	{
		uint32 area_id = sp->p_caster->GetAreaID();
		//within the center of the lumbermill at New Hearthglen
		if( sp->p_caster->GetMapId() != 1 || area_id != 4998 )
			return SPELL_FAILED_NOT_HERE;
	}
	return SPELL_CANCAST_OK;
}

uint32 CCH_77571(Spell *sp)
{
	if( sp->p_caster )
	{
		uint32 area_id = sp->p_caster->GetAreaID();
		//wetlands. Needs nearby "Marsh Fire"
		if( sp->p_caster->GetMapId() != 1 || area_id != 4998 )
			return SPELL_FAILED_NOT_HERE;
	}
	return SPELL_CANCAST_OK;
}

uint32 CCH_88914(Spell *sp)
{
	Creature * pTarget = sp->m_caster->GetMapMgr()->GetCreature( sp->m_targets.m_unitTarget );
	if( pTarget && pTarget->GetEntry() == 46141 )
		return SPELL_CANCAST_OK;
	return SPELL_FAILED_BAD_TARGETS;
}

uint32 CCH_23359(Spell *sp)
{
	Creature * pTarget = sp->m_caster->GetMapMgr()->GetCreature( sp->m_targets.m_unitTarget );
	if( pTarget && pTarget->GetEntry() == 5359 )
		return SPELL_CANCAST_OK;
	return SPELL_FAILED_BAD_TARGETS;
}

uint32 CCH_48045(Spell *sp)
{
	if( sp->m_targets.m_unitTarget == sp->m_caster->GetGUID() )
		return SPELL_FAILED_BAD_TARGETS;
	return SPELL_CANCAST_OK;
}

uint32 CCFILTERH_19263( Spell *sp, SpellCanCastScript *ccs )
{
	if( sp->i_caster != NULL )
		return SPELL_CANCAST_OK;
	switch( sp->GetProto()->NameHash )
	{
		case SPELL_HASH_MASTER_S_CALL:
		case SPELL_HASH_CALL_PET:
		case SPELL_HASH_CALL_PET_1:
		case SPELL_HASH_CALL_PET_2:
		case SPELL_HASH_CALL_PET_3:
		case SPELL_HASH_CALL_PET_4:
		case SPELL_HASH_CALL_PET_5:
		case SPELL_HASH_DISMISS_PET:
		case SPELL_HASH_DISENGAGE:
		case SPELL_HASH_FEIGN_DEATH:
		case SPELL_HASH_TRAP_LAUNCHER:
		case SPELL_HASH_FREEZING_TRAP:
		case SPELL_HASH_SNAKE_TRAP:
		case SPELL_HASH_ICE_TRAP:
		case SPELL_HASH_SILENCING_SHOT:
		case SPELL_HASH_SCARE_BEAST:
		case SPELL_HASH_FLARE:
		case SPELL_HASH_KILL_COMMAND:
		case SPELL_HASH_CAMOUFLAGE:
		case SPELL_HASH_REVIVE_PET:
		case SPELL_HASH_MEND_PET:
			return SPELL_CANCAST_OK;
			break;
		default:
			return SPELL_FAILED_SILENCED;
	};
	return SPELL_FAILED_SILENCED;
}

uint32 CCFILTERH_76577( Spell *sp, SpellCanCastScript *ccs )
{
	//AOE can pass through
	if( sp->GetProto()->c_is_flags2 & SPELL_FLAG2_IS_ENEMY_AOE_TARGETTING )
		return SPELL_CANCAST_OK;
	//AOE can pass through
	if( sp->GetProto()->c_is_flags3 & SPELL_FLAG3_IS_FRIENDLY_AOE_TARGETING )
		return SPELL_CANCAST_OK;
	//unsure case
	if( ( sp->GetProto()->c_is_flags3 & SPELL_FLAG3_IS_DIRECT_TARGET ) == 0 )
		return SPELL_DID_HIT_SUCCESS;
	//we are casting from inside to outside
	if( sp->u_caster == NULL )
		return SPELL_CANCAST_OK;
	Unit *Target = sp->m_caster->GetMapMgr()->GetUnit( sp->m_targets.m_unitTarget );
	if( Target == NULL )
		return SPELL_CANCAST_OK;
	//smoke caster group can cast on enemy inside smoke
	if( sp->u_caster->GetGroup() != NULL && sp->u_caster->GetGroup()->GetID() == ccs->CasterGroupId && isAttackable( sp->u_caster, Target ) )
		return SPELL_CANCAST_OK;
	Aura *a = Target->HasAura( ccs->OwnerSpell->Id );
	if( a == NULL )
		return SPELL_FAILED_LINE_OF_SIGHT;
	return SPELL_CANCAST_OK;
}

uint32 CCH_1719( Spell *sp )
{
	if( sp->u_caster && sp->u_caster->HasAura( 20230, 0, AURA_SEARCH_POSITIVE ) )
		return SPELL_FAILED_NO_VALID_TARGETS;
	return SPELL_CANCAST_OK;
}

uint32 CCH_20230( Spell *sp )
{
	if( sp->u_caster && sp->u_caster->HasAura( 1719, 0, AURA_SEARCH_POSITIVE ) )
		return SPELL_FAILED_NO_VALID_TARGETS;
	return SPELL_CANCAST_OK;
}

uint32 CCH_82692( Spell *sp )
{
	if( sp->p_caster == NULL || sp->p_caster->GetSummon() == NULL || sp->p_caster->GetSummon()->HasAuraWithNameHash( SPELL_HASH_FRENZY_EFFECT, 0, AURA_SEARCH_POSITIVE ) == 0 )
		return SPELL_FAILED_NO_VALID_TARGETS;
	return SPELL_CANCAST_OK;
}

uint32 CCFILTERH_33786( Spell *sp, SpellCanCastScript *ccs )
{
	//only allow trinkets and bestial wrath
/*	switch( sp->GetProto()->NameHash )
	{
		case SPELL_HASH_DIVINE_SHIELD:
			return SPELL_CANCAST_OK;
			break;
		default:
			return SPELL_FAILED_SILENCED;
	}; */

	return SPELL_FAILED_SILENCED;
}

uint32 CCH_80964( Spell *sp )
{
	if( sp->u_caster && ( sp->u_caster->m_special_state & UNIT_STATE_ROOT ) )
	{
		Unit *Target = sp->m_caster->GetMapMgr()->GetUnit( sp->m_targets.m_unitTarget );
		if( Target == NULL || DistanceSq( sp->u_caster, Target ) > PLAYER_DEFAULT_MELEE_RANGE * PLAYER_DEFAULT_MELEE_RANGE )
			return SPELL_FAILED_NO_VALID_TARGETS;
	}
	return SPELL_CANCAST_OK;
}

uint32 CCH_55709( Spell *sp )
{
	if( ( sp->p_caster && ( sp->p_caster->GetSummon() == NULL || sp->p_caster->GetSummon()->isAlive() ) )
		|| ( sp->u_caster && sp->u_caster->IsPet() && sp->u_caster->isAlive() ) ) 
	{
		return SPELL_FAILED_NO_VALID_TARGETS;
	}
	return SPELL_CANCAST_OK;
}


void RegisterCanCastOverrides()
{
	SpellEntry *sp;
	//Heart of the Phoenix
	REGISTER_HANDLER( 55709 ); 
	//Skull Bash
	REGISTER_HANDLER( 80964 ); 
	REGISTER_SAMEHANDLER( 80965, 80964); 
	//Cyclone
	REGISTER_FILTERHANDLER( 33786 ); 
	//Focus Fire
	REGISTER_HANDLER( 82692 ); 
	//Recklessness
	REGISTER_HANDLER( 1719 ); 
	//Retaliation
	REGISTER_HANDLER( 20230 ); 
	//Smoke Bomb
	REGISTER_FILTERHANDLER( 76577 ); 
	//Deterrence
	REGISTER_FILTERHANDLER( 19263 ); 
	//Mind Sear
	REGISTER_HANDLER( 48045 ); 
	REGISTER_SAMEHANDLER( 5143, 48045); //Arcane Missiles no self casts
	//Transmogrify!
	REGISTER_HANDLER( 23359 ); 
	//Wyrmhunter Hooks
	REGISTER_HANDLER( 88914 ); 
	//Water Blast
	REGISTER_HANDLER( 77571 ); 
	//Levine Family Termites
	REGISTER_HANDLER( 48427 ); 
	//Call Withered Ent
	REGISTER_HANDLER( 64306 ); 
	//Burn Corpse
	REGISTER_HANDLER( 85555 ); 
	//Marked for Retrieval
	REGISTER_HANDLER( 87296); 
	REGISTER_SAMEHANDLER( 87293, 87296); 
	//Goblin Rocket Pack
	REGISTER_HANDLER( 68645 );	
	//Execute
	REGISTER_HANDLER( 5308 );	
	//Master's Call
	REGISTER_HANDLER( 53271 );	
	//Legion Strike
//	REGISTER_HANDLER( 30213 );	
	//Sprinkle Holy Water
	REGISTER_HANDLER( 57806 );	
	//Have Group, Will Travel
	REGISTER_HANDLER( 83967 );	
	//Taunt Flag
	REGISTER_HANDLER( 51640 );	
	//Premeditation
	REGISTER_HANDLER( 14183 );	
	//Cannibalize
	REGISTER_HANDLER( 20577 );	
	//Heroic Leap
	REGISTER_HANDLER( 6544 );	
	//Blink
	REGISTER_HANDLER( 1953 );	
	//Disengage
	REGISTER_HANDLER( 781 );	
	//Gouge
	REGISTER_HANDLER( 1776 );
	//intercept
	REGISTER_HANDLER( 20252); 
	REGISTER_SAMEHANDLER( 36554, 20252); //Shadowstep
	//Conflagrate
	REGISTER_HANDLER( 17962 );	
	//Demonic Circle : teleport
	REGISTER_HANDLER( 48020 );	
	//Scared Softknuckle
	REGISTER_HANDLER( 50979 );	
	//Flame Ascendancy
	REGISTER_HANDLER( 75554 );	
	//Pain Suppression
	REGISTER_HANDLER( 33206 );	
	//Enchanted Salve
	REGISTER_HANDLER( 97670 );	
	//Holy Shock
	REGISTER_HANDLER( 20473 );	
	//Duarn's Net
	REGISTER_HANDLER( 77682 );	
	//Ireroot Seeds - Nature's Fury
	REGISTER_HANDLER( 65455 );	
	//Helcular's Rod
	REGISTER_HANDLER( 89821 );	
	//Swiftmend
	REGISTER_HANDLER( 18562 );	
	//Hammer of Wrath
	REGISTER_HANDLER( 24275 );	
	//Fishing - maybe we should do this in spellfixes.cpp and check for target 39 
	REGISTER_HANDLER( 7620 );	
	REGISTER_SAMEHANDLER( 7731, 7620);
	REGISTER_SAMEHANDLER( 7732, 7620);
	REGISTER_SAMEHANDLER( 18248, 7620);
	REGISTER_SAMEHANDLER( 33095, 7620);
	REGISTER_SAMEHANDLER( 51294, 7620);
	REGISTER_SAMEHANDLER( 62734, 7620);
	REGISTER_SAMEHANDLER( 63275, 7620);
	REGISTER_SAMEHANDLER( 88868, 7620);
	//Revive Pet
	REGISTER_HANDLER( 982 );
	//Dash
	REGISTER_HANDLER( 1850 );
	//Smite Demon
	REGISTER_HANDLER( 13907 );
	//tame beast
	REGISTER_HANDLER( 1515 );
	//survival instincts
	REGISTER_HANDLER( 61336 );
	//bloodlust
	REGISTER_HANDLER( 2825 );
	REGISTER_SAMEHANDLER( 32182, 2825 );	//heroism
	REGISTER_SAMEHANDLER( 90355, 2825 );	//Ancient Hysteria
	REGISTER_SAMEHANDLER( 80353, 2825 );	//Time Warp
	//Reindeer Transformation
	REGISTER_HANDLER( 25860 );	
	//Duel
	REGISTER_HANDLER( 7266 );	
	//Raise Dead
//	REGISTER_HANDLER( 46584 );	
	//Pounce
	REGISTER_HANDLER( 9005 );	
	//Plant Warmaul Ogre Banner
	REGISTER_HANDLER( 32307 );	
	//Demon Broiled Surprise
	REGISTER_HANDLER( 43723 );	
	//Place Draenei Banner
	REGISTER_HANDLER( 30988 );	
	//Fumping
	REGISTER_HANDLER( 39246 );	
	//Repolarized Magneto Sphere
	REGISTER_HANDLER( 37830 );	
	//mind control
	REGISTER_HANDLER( 605 );	
	//Liquid Fire
	REGISTER_HANDLER( 32146 );	
	//Death Pact
	REGISTER_HANDLER( 48743 );	
	//Deep freez
	REGISTER_HANDLER( 44572 );	
	//Viewpoint Equalizer - Equalize Viewpoint
	REGISTER_HANDLER( 93773 );	
	//Dryad Spear
	REGISTER_HANDLER( 62599 );	
	//Deepvein's Patch Kit
	REGISTER_HANDLER( 80987 );	
	//death coil only heals undead
	REGISTER_HANDLER( 47541 );	
	//Inner Rage
//	REGISTER_HANDLER( 1134 );

	REGISTER_HANDLER( 642 );	//divine shield
	REGISTER_SAMEHANDLER( 1022, 642 );	//Hand of Protection
	REGISTER_SAMEHANDLER( 633, 642 );	//Lay on Hands
//	REGISTER_SAMEHANDLER( 31884, 642 );	//Avenging Wrath
//	REGISTER_SAMEHANDLER( 498, 31884 );	//Divine Protection
}