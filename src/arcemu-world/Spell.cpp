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

#define SPELL_CHANNEL_UPDATE_INTERVAL 1000

/// externals for spell system
extern pSpellEffect SpellEffectsHandler[TOTAL_SPELL_EFFECTS];
extern pSpellTarget SpellTargetHandler[EFF_TARGET_LIST_LENGTH_MARKER];

extern char* SpellEffectNames[TOTAL_SPELL_EFFECTS];

enum SpellTargetSpecification
{
    TARGET_SPECT_NONE       = 0,
    TARGET_SPEC_INVISIBLE   = 1,
    TARGET_SPEC_DEAD        = 2,
};

void SpellCastTargets::read( WorldPacket & data,uint64 caster )
{
	m_unitTarget = m_itemTarget = 0;
	m_srcX = m_srcY = m_srcZ = m_destX = m_destY = m_destZ = missilepitch = missilespeed = 0.0f;
	missileunkcheck = 0;
	unkdoodah = unkdoodah2 = 0;
	m_strTarget = "";

//	data >> m_missileFlags; - not sure all packets have it so this is set in client packet handler function
	data >> m_targetMask;
	data >> m_targetMaskExtended;

	if( m_targetMask == TARGET_FLAG_SELF )
	{
		m_unitTarget = caster;
	}

	if( m_targetMask & (TARGET_FLAG_OBJECT | TARGET_FLAG_UNIT | TARGET_FLAG_CORPSE | TARGET_FLAG_CORPSE2 ) )
	{
		WoWGuid guid2;
//		data >> guid2; //maybe just 1 byte ?
		data >> guid;
		m_unitTarget = guid.GetOldGuid();
	}

	if( m_targetMask & ( TARGET_FLAG_ITEM | TARGET_FLAG_TRADE_ITEM ) )
	{
		WoWGuid guid2;
//		data >> guid2;	//maybe just 1 byte ?
		data >> guid;
		m_itemTarget = guid.GetOldGuid();
	}

	if( m_targetMask & TARGET_FLAG_SOURCE_LOCATION )
	{
		data >> guid;	//added in 3.2
		data >> m_srcX >> m_srcY >> m_srcZ;

		if( !( m_targetMask & TARGET_FLAG_DEST_LOCATION ) )
		{
			m_destX = m_srcX;
			m_destY = m_srcY;
			m_destZ = m_srcZ;
		}
	}

	if( m_targetMask & TARGET_FLAG_DEST_LOCATION )
	{
		data >> guid >> m_destX >> m_destY >> m_destZ;
		if( !( m_targetMask & TARGET_FLAG_SOURCE_LOCATION ) )
		{
			m_srcX = m_destX;
			m_srcY = m_destY;
			m_srcZ = m_destZ;
		}
	}
	if( m_targetMask & TARGET_FLAG_PET_DEST_LOCATION )
	{
		WoWGuid guid2;
		data >> guid;
		data >> guid2;
//		targets.m_unitTarget = guid2.GetOldGuid();
		data >> m_destX >> m_destY >> m_destZ;
	}

	if( m_targetMask & TARGET_FLAG_STRING )
	{
		data >> m_strTarget;
	}

	if (m_missileFlags & 0x2)
 	{
		data >> missilepitch >> missilespeed >> missileunkcheck;

		if (missileunkcheck == 1)
		{
//			WoWGuid tguid;
//			data >> tguid;	//compressed guid instead of doodah !
//			data >> unkdoodah;
//			data >> unkdoodah2;
		}

		float dx = m_destX - m_srcX;
		float dy = m_destY - m_srcY;
		if (missilepitch != M_PI / 4 && missilepitch != -M_PI / 4)
			traveltime = (sqrtf(dx * dx + dy * dy) / (cosf(missilepitch) * missilespeed)) * 1000;
 	}
}

void SpellCastTargets::write( StackWorldPacket& data )
{
	data << m_targetMask;
	data << m_targetMaskExtended;

	if( /*m_targetMask == TARGET_FLAG_SELF || */m_targetMask & (TARGET_FLAG_UNIT | TARGET_FLAG_CORPSE | TARGET_FLAG_CORPSE2 | TARGET_FLAG_OBJECT ) )
        FastGUIDPack( data, m_unitTarget );

    if( m_targetMask & ( TARGET_FLAG_ITEM | TARGET_FLAG_TRADE_ITEM ) )
        FastGUIDPack( data, m_itemTarget );

	if( m_targetMask & TARGET_FLAG_SOURCE_LOCATION )
	{
		data << guid;	//3.3
		data << m_srcX << m_srcY << m_srcZ;
	}

	if( m_targetMask & TARGET_FLAG_DEST_LOCATION )
	{
		data << guid; //303 -> this is packed guid
		data << m_destX << m_destY << m_destZ;
	}
	if( m_targetMask & TARGET_FLAG_STRING )
		data << m_strTarget;
	if( m_targetMask & TARGET_FLAG_PET_DEST_LOCATION )
	{
		WoWGuid guid2;
		data << uint8(0);	//guid 1
		data << uint8(0);	//guid 2
		data << m_destX << m_destY << m_destZ;
	}
/*	if (m_missileFlags & 0x2)
 	{
		data << missilepitch << missilespeed << missileunkcheck;
		if (missileunkcheck == 1)
		{
			data << unkdoodah;
			data << unkdoodah2;
		}
 	}*/
}

Spell::Spell()
{
	m_bufferPoolId = OBJECT_WAS_ALLOCATED_STANDARD_WAY;
	m_caster = u_caster = p_caster = NULL;
}

void Spell::Virtual_Constructor()
{
}

void Spell::Init(Object* Caster, SpellEntry *info, bool triggered, Aura* aur)
{
	ASSERT( Caster != NULL )
	ASSERT( info != NULL );

#ifdef DEBUG_SPELL_CASTS
	if( info->Id < MAX_USABLE_SPELL_ID )
	{
		if( casted_spells[ info->Id ] == 0 )
			DumpToFileUniqueSpells( true );
		casted_spells[ info->Id ] = 1;
//printf("!!!spell cast %u -> %u pool id %u, caster guid %u \n",info->Id,last_N_spells_index,m_bufferPoolId,(uint32)(Caster->GetGUID()) );
//CStackWalker sw;
//sw.ShowCallstack();
		if( last_N_spells_FILE )
		{
			last_N_spells_index = ( last_N_spells_index + 1 ) % LOG_LAST_N_SPELLS;
			last_N_spells[ last_N_spells_index ] = info->Id;
			fseek(last_N_spells_FILE,SEEK_SET,0);
			for(int i=0;i<LOG_LAST_N_SPELLS;i++)
				fprintf(last_N_spells_FILE,"%u\n",last_N_spells[ (100 + last_N_spells_index - i ) % LOG_LAST_N_SPELLS ]);
		}
	}
#endif
	GlyphSlot = 0xFFFFFFFF; //we do not have it set
	UniqueTargets.clear();
	ModeratedTargets.clear();
	chaincast_jump_pct_mod = 100;
	bDurSet = 0;
	damage = 0;
	for( uint32 i=0;i<MAX_SPELL_EFFECT_COUNT;i++)
	{
		m_targetUnits[i].clear();
		bRadSet[i] = 0;
		forced_pct_mod[i] = 100;
		forced_basepoints[i] = 0;
		forced_miscvalues[i] = 0;
//		forced_heal_crit_mod[i] = 0;
		static_dmg[i] = 0;
		bRadSet[i] = false;
		redirected_effect[i] = SPELL_EFFECT_REDIRECT_FLAG_EXECUTE;
		eff_values[ i ] = 0;
	}

	m_spellInfo = info;
//	m_spellInfo_override = NULL;
	m_caster = Caster;
	duelSpell = false;
	deleted = false;

	i_caster = NULL; u_caster = NULL; p_caster = NULL;
	switch( Caster->GetTypeId() )
	{
		case TYPEID_PLAYER:
		{
//			g_caster = NULL;
//			i_caster = NULL;
			u_caster = SafeUnitCast( Caster );
			p_caster = SafePlayerCast( Caster );
			if( p_caster->GetDuelState() == DUEL_STATE_STARTED )
				duelSpell = true;
			//this is ugly so i will explain the case maybe someone has a better idea :
			// while casting a spell talent will trigger uppon the spell prepare faze
			// the effect of the talent is to add 1 combo point but when triggering spell finishes it will clear the extra combo point
			p_caster->m_spellcomboPoints = 0;
		} break;

		case TYPEID_UNIT:
		{
//			g_caster = NULL;
//			i_caster = NULL;
//			p_caster = NULL;
			u_caster = SafeUnitCast( Caster );
			if( u_caster->IsPet() && SafePetCast( u_caster)->GetPetOwner() != NULL && SafePetCast( u_caster )->GetPetOwner()->GetDuelState() == DUEL_STATE_STARTED )
				duelSpell = true;
		} break;

#ifndef SMALL_ITEM_OBJECT
		case TYPEID_ITEM:
		case TYPEID_CONTAINER:
		{
//			g_caster = NULL;
//			u_caster = NULL;
//			p_caster = NULL;
			i_caster = SafeItemCast( Caster );
			if( i_caster->GetOwner() && i_caster->GetOwner()->GetDuelState() == DUEL_STATE_STARTED )
				duelSpell = true;
		} break;
#endif

		case TYPEID_GAMEOBJECT:
		{
//			u_caster = NULL;
//			p_caster = NULL;
//			i_caster = NULL;
//			g_caster = SafeGOCast( Caster );
			//required for trap spell saling
			if( m_caster->GetUInt64Value( OBJECT_FIELD_CREATED_BY ) )
				u_caster = p_caster = m_caster->GetMapMgr()->GetPlayer( m_caster->GetUInt64Value( OBJECT_FIELD_CREATED_BY ) );
		} break;

		default:
			sLog.outDebug("[DEBUG][SPELL] Incompatible object type, please report this to the dev's");
			break;
	}

	m_spellState = SPELL_STATE_NULL;

	m_castPositionX = m_castPositionY = m_castPositionZ = 0;
	//TriggerSpellId = 0;
	//TriggerSpellTarget = 0;
//	m_triggeredSpell = triggered;
	m_AreaAura = false;

	m_triggeredByAura = aur;

	castedItemId = 0;

//	m_usesMana = false;
	m_Spell_Failed = false;
//	m_CanRelect = false;
	m_IsReflected = SPELL_REFLECT_NONE;
	hadEffect = false;
	bDurSet = false;

	cancastresult = SPELL_CANCAST_OK;

	m_requiresCP = 0;
	unitTarget = NULL;
	ModeratedTargets.clear();
	itemTarget = NULL;
	gameObjTarget = NULL;
	playerTarget = NULL;
	corpseTarget = NULL;
	add_damage = 0;
	m_Delayed = false;
	if( triggered == true )
		pSpellId = 1;	//dummy spell to mark that we need to delete auras...
	else
		pSpellId = 0;
	m_cancelled = false;
	ProcedOnSpell = 0;
	forced_duration = 0;
	SpellCastQueueIndex = 0;
//	m_reflectedParent = NULL;
//	m_isCasting = false;
	channel_delays = 0;
	//create rune avail snapshot
	m_rune_avail_before = 0;
	if( p_caster && p_caster->getClass() == DEATHKNIGHT )
	{
		for( int i=0;i<TOTAL_USED_RUNES;i++)
			if( p_caster->m_runes[ i ] >= MAX_RUNE_VALUE )
				m_rune_avail_before |= (1 << i);
	}
	m_missilePitch = 0;
	m_missileTravelTime = 0;
	power_cost = 0; //
}

Spell::~Spell()
{
	Virtual_Destructor();
	if( m_bufferPoolId != OBJECT_WAS_ALLOCATED_STANDARD_WAY )
		ASSERT( false ); //we are deleting a pooled object. This leads to mem corruptions
	m_bufferPoolId = OBJECT_WAS_DEALLOCATED;
}

void Spell::Virtual_Destructor()
{
	if( u_caster != NULL && u_caster->GetCurrentSpell() == this )
		u_caster->SetCurrentSpell(NULL);
	u_caster = NULL;

	if( p_caster )
	{
		if( hadEffect || ( cancastresult == SPELL_CANCAST_OK && !GetSpellFailed() ) )
			RemoveItems();
		p_caster = NULL;
	}
	m_caster = NULL;

	deleted = true;

//	if (m_spellInfo_override !=NULL)
//		delete[] m_spellInfo_override;

//	sEventMgr.RemoveEvents( this ); //do even spells have events ?
}

//i might forget conditions here. Feel free to add them
bool Spell::IsStealthSpell()
{
	//check if aura name is some stealth aura
	return ( ( GetProto()->c_is_flags3 & SPELL_FLAG3_IS_STEALTH_SPELL ) != 0 );
}

//i might forget conditions here. Feel free to add them
bool Spell::IsInvisibilitySpell()
{
	//check if aura name is some invisibility aura
	return HasAuraType( GetProto(), SPELL_AURA_MOD_INVISIBILITY );
}

void Spell::FillSpecifiedTargetsInArea( float srcx, float srcy, float srcz, uint32 ind, uint32 specification )
{
	FillSpecifiedTargetsInArea( ind, srcx, srcy, srcz, GetRadius(ind), specification );
}

// for the moment we do invisible targets
void Spell::FillSpecifiedTargetsInArea(uint32 i,float srcx,float srcy,float srcz, float range, uint32 specification)
{
	TargetsList *tmpMap=&m_targetUnits[i];
	//IsStealth()
	float r = range * range;
	uint8 did_hit_result;
	uint32 targettypecheck = GetTargetCreatureType( GetProto() );
	int32 MaxTargets = GetProto()->GetMaxTargets();
	if((GetProto()->c_is_flags & SPELL_FLAG_IS_REQUIRING_SM_MOD) && u_caster!= NULL && u_caster->SM_Mods != NULL )
		SM_FIValue(u_caster->SM_Mods->SM_FAdditionalTargets,&MaxTargets,GetProto()->GetSpellGroupType());
	m_caster->AquireInrangeLock(); //make sure to release lock before exit function !
	InrangeLoopExitAutoCallback AutoLock;
	for(InRangeSetRecProt::iterator itr = m_caster->GetInRangeSetBegin( AutoLock ); itr != m_caster->GetInRangeSetEnd(); itr++ )
	{
		// don't add objects that are not units and that are dead
		if( !( (*itr)->IsUnit() ) || ! SafeUnitCast( *itr )->isAlive())
			continue;

		//SafeUnitCast( *itr )->IsStealth()
		if( !TargetTypeCheck( (*itr), targettypecheck) )
			continue;

		if(IsInrange(srcx,srcy,srcz,(*itr),r))
		{
			if( u_caster != NULL )
			{
				if( isAttackable( u_caster, SafeUnitCast( *itr ), !( GetProto()->c_is_flags & SPELL_FLAG_IS_TARGETINGSTEALTHED ) ) )
				{
					did_hit_result = DidHit(i, SafeUnitCast( *itr ) );
					if( did_hit_result != SPELL_DID_HIT_SUCCESS )
						ModeratedTargets.push_back(SpellTargetMod((*itr)->GetGUID(), did_hit_result));
					else
					{
						SafeAddTarget(tmpMap, (*itr)->GetGUID());
						MaxTargets--;
						if( MaxTargets <= 0 )
							break;
					}
				}

			}
			else //cast from GO
			{
				if ( m_caster->IsGameObject() && m_caster->GetUInt32Value( OBJECT_FIELD_CREATED_BY ) )
				{
					//trap, check not to attack owner and friendly
					Unit *GoCreater = m_caster->GetMapMgr()->GetUnit( m_caster->GetUInt64Value( OBJECT_FIELD_CREATED_BY ) );
					if(isAttackable( GoCreater, (*itr), !(GetProto()->c_is_flags & SPELL_FLAG_IS_TARGETINGSTEALTHED)))
					{
						SafeAddTarget(tmpMap, (*itr)->GetGUID());
						MaxTargets--;
						if( MaxTargets <= 0 )
							break;
					}
				}
				else
				{
					SafeAddTarget(tmpMap, (*itr)->GetGUID());
					MaxTargets--;
					if( MaxTargets <= 0 )
						break;
				}
			}
		}
	}
	m_caster->ReleaseInrangeLock();
}
void Spell::FillAllTargetsInArea(const LocationVector & location,uint32 ind)
{
	FillAllTargetsInArea(ind,location.x,location.y,location.z,GetRadius(ind));
}

void Spell::FillAllTargetsInArea(float srcx,float srcy,float srcz,uint32 ind)
{
	FillAllTargetsInArea(ind,srcx,srcy,srcz,GetRadius(ind));
}

/// We fill all the targets in the area, including the stealth ed one's
void Spell::FillAllTargetsInArea(uint32 i,float srcx,float srcy,float srcz, float range)
{
	TargetsList *tmpMap=&m_targetUnits[i];
	float r = range*range;
	uint8 did_hit_result;
	uint32 targettypecheck = GetTargetCreatureType( GetProto() );
	int32 max_targets = GetProto()->GetMaxTargets();
	if((GetProto()->c_is_flags & SPELL_FLAG_IS_REQUIRING_SM_MOD) && u_caster && u_caster->SM_Mods )
		SM_FIValue(u_caster->SM_Mods->SM_FAdditionalTargets,&max_targets,GetProto()->GetSpellGroupType());
	if( max_targets <= 0 )
		max_targets = 65535;

	InRangeSetRecProt::iterator itr,itr2;
	m_caster->AquireInrangeLock(); //make sure to release lock before exit function !
	InrangeLoopExitAutoCallback AutoLock;
	for( itr2 = m_caster->GetInRangeSetBegin( AutoLock ); itr2 != m_caster->GetInRangeSetEnd();)
	{
		itr = itr2;
		itr2++; //maybe scripts can change list. Should use lock instead of this to prevent multiple changes. This protects to 1 deletion only
		if( !( (*itr)->IsUnit() ) 
				|| !SafeUnitCast( *itr )->isAlive() 
				//fix for spell not targetting totems ? But they are not supposed to target them no ?
				|| ( SafeUnitCast( *itr )->IsCreature() && SafeCreatureCast( *itr )->IsTotem())  //totems should be targeted by direct attack spells not AOE
				)
			continue;

		Unit *ut = SafeUnitCast( *itr );
		if( !TargetTypeCheck( ut, targettypecheck ) )
			continue;

		if( ( ut->GetPhase() & m_caster->GetPhase() ) == 0 )
			continue;

		if( IsInrange( srcx, srcy, srcz, ut, r ) )
		{
			if( u_caster != NULL )
			{
				if( isAttackable( u_caster, ut, !(GetProto()->c_is_flags & SPELL_FLAG_IS_TARGETINGSTEALTHED) ) )
				{
					if( !( sWorld.Collision && (GetProto()->c_is_flags3 & SPELL_FLAG3_IS_NEEDING_LOS_CHECK) && !CollideInterface.CheckLOS( m_caster->GetMapId(), m_caster->GetPositionNC(), ut->GetPositionNC() ) ) )
					{
						did_hit_result = DidHit( i, ut );
						if( did_hit_result == SPELL_DID_HIT_SUCCESS )
						{
							SafeAddTarget(tmpMap, ut->GetGUID());
							max_targets--;
							if( max_targets <= 0 )
								break;
						}
						else
							ModeratedTargets.push_back( SpellTargetMod( ut->GetGUID(), did_hit_result ) );
					}
				}
			}
			else //cast from GO
			{
				if( m_caster->IsGameObject() && m_caster->GetUInt32Value( OBJECT_FIELD_CREATED_BY ) )
				{
					//trap, check not to attack owner and friendly
					Unit *GoCreater = m_caster->GetMapMgr()->GetUnit( m_caster->GetUInt64Value( OBJECT_FIELD_CREATED_BY ) );
					if( isAttackable( GoCreater, SafeUnitCast( *itr ), !(GetProto()->c_is_flags & SPELL_FLAG_IS_TARGETINGSTEALTHED) ) )
					{
						SafeAddTarget(tmpMap, (*itr)->GetGUID());
						max_targets--;
						if( max_targets <= 0 )
							break;
					}
				}
				else
				{
					SafeAddTarget(tmpMap, (*itr)->GetGUID());
					max_targets--;
					if( max_targets <= 0 )
						break;
				}
			}
		}
	}
	m_caster->ReleaseInrangeLock();
}

// We fill all the targets in the area, including the stealth ed one's
void Spell::FillAllFriendlyInArea( uint32 i, float srcx, float srcy, float srcz, float range )
{
	TargetsList *tmpMap=&m_targetUnits[i];
	float r = range * range;
	uint8 did_hit_result;
	int32 MaxTargets;
	MaxTargets = GetProto()->GetMaxTargets();
	if((GetProto()->c_is_flags & SPELL_FLAG_IS_REQUIRING_SM_MOD) && u_caster && u_caster->SM_Mods )
		SM_FIValue(u_caster->SM_Mods->SM_FAdditionalTargets,&MaxTargets,GetProto()->GetSpellGroupType());
	InRangeSetRecProt::iterator itr,itr2;
	uint32 targettypecheck = GetTargetCreatureType( GetProto() );
	m_caster->AquireInrangeLock(); //make sure to release lock before exit function !
	SafeAddTarget(tmpMap, m_caster->GetGUID());
	InrangeLoopExitAutoCallback AutoLock;
	int32 FirstTargetLowHP = ( ( MaxTargets < 65535 ) && ( GetProto()->c_is_flags & SPELL_FLAG_IS_HEALING ) != 0 );
	for( int32 TargettingLoop=1+FirstTargetLowHP;TargettingLoop>=0;TargettingLoop--)
	{
		for( itr2 = m_caster->GetInRangeSetBegin( AutoLock ); itr2 != m_caster->GetInRangeSetEnd();)
		{
			itr = itr2;
			itr2++; //maybe scripts can change list. Should use lock instead of this to prevent multiple changes. This protects to 1 deletion only
			if( !((*itr)->IsUnit()) || !SafeUnitCast( *itr )->isAlive() )
				continue;

			if( TargettingLoop == 2 && SafeUnitCast( *itr )->GetMaxHealth() == SafeUnitCast( *itr )->GetHealth() )
				continue;

			if( !TargetTypeCheck( (*itr), targettypecheck ) )
				continue;

			if( IsInrange( srcx, srcy, srcz, (*itr), r ) )
			{
				if( u_caster != NULL )
				{
					if( isFriendly( u_caster, *itr ) && isAttackable( u_caster, *itr) == false )
					{
						did_hit_result = DidHit(i, SafeUnitCast( *itr ) );
						if( did_hit_result == SPELL_DID_HIT_SUCCESS )
							SafeAddTarget(tmpMap, (*itr)->GetGUID());
						else
							ModeratedTargets.push_back( SpellTargetMod( (*itr)->GetGUID(), did_hit_result ) );
					}
				}
				else //cast from GO
				{
					if( m_caster->IsGameObject() && m_caster->GetUInt32Value( OBJECT_FIELD_CREATED_BY ) )
					{
						//trap, check not to attack owner and friendly
						Unit *GoCreater = m_caster->GetMapMgr()->GetUnit( m_caster->GetUInt64Value( OBJECT_FIELD_CREATED_BY ) );
						if( isFriendly( GoCreater, SafeUnitCast( *itr ) ) )
							SafeAddTarget(tmpMap, (*itr)->GetGUID());
					}
					else
						SafeAddTarget(tmpMap, (*itr)->GetGUID());
				}
				if( MaxTargets <= (int32)tmpMap->size() )
				{
					m_caster->ReleaseInrangeLock();
					return;
				}
			}
		}
	}
	m_caster->ReleaseInrangeLock();
}

uint64 Spell::GetSinglePossibleEnemy(uint32 i,float prange)
{
	float r;
	if(prange)
		r = prange;
	else
	{
		r = GetProto()->base_range_or_radius_sqr;
		if( Need_SM_Apply(GetProto()) && u_caster && u_caster->SM_Mods )
		{
			SM_FFValue(u_caster->SM_Mods->SM_FRadius,&r,GetProto()->GetSpellGroupType());
			SM_PFValue(u_caster->SM_Mods->SM_PRadius,&r,GetProto()->GetSpellGroupType());
		}
	}
	float srcx = m_caster->GetPositionX(), srcy = m_caster->GetPositionY(), srcz = m_caster->GetPositionZ();
	uint32 targettypecheck = GetTargetCreatureType( GetProto() );
	m_caster->AquireInrangeLock(); //make sure to release lock before exit function !
	InrangeLoopExitAutoCallback AutoLock;
	for( InRangeSetRecProt::iterator itr = m_caster->GetInRangeSetBegin( AutoLock ); itr != m_caster->GetInRangeSetEnd(); itr++ )
	{
		if( !( (*itr)->IsUnit() ) || !SafeUnitCast( *itr )->isAlive() )
			continue;

		if( !TargetTypeCheck( (*itr), targettypecheck ) )
			continue;

		if(IsInrange(srcx,srcy,srcz,(*itr),r))
		{
			if( u_caster != NULL )
			{
				if(isAttackable(u_caster, *itr ,!(GetProto()->c_is_flags & SPELL_FLAG_IS_TARGETINGSTEALTHED)) && DidHit(i,((Unit*)*itr))==SPELL_DID_HIT_SUCCESS)
				{
					m_caster->ReleaseInrangeLock();
					return (*itr)->GetGUID();
				}
			}
			else //cast from GO
			{
				if( m_caster->IsGameObject() && m_caster->GetUInt32Value(OBJECT_FIELD_CREATED_BY) )
				{
					//trap, check not to attack owner and friendly
					Unit *GoCreater = m_caster->GetMapMgr()->GetUnit( m_caster->GetUInt64Value( OBJECT_FIELD_CREATED_BY ) );
					if( isAttackable( GoCreater, SafeUnitCast( *itr ),!(GetProto()->c_is_flags & SPELL_FLAG_IS_TARGETINGSTEALTHED)))
					{
						m_caster->ReleaseInrangeLock();
						return (*itr)->GetGUID();
					}
				}
			}
		}
	}
	m_caster->ReleaseInrangeLock();
	return 0;
}

uint64 Spell::GetSinglePossibleFriend(uint32 i,float prange)
{
	float r;
	if(prange)
		r = prange;
	else
	{
		r = GetProto()->base_range_or_radius_sqr;
		if( Need_SM_Apply(GetProto()) && u_caster && u_caster->SM_Mods )
		{
			SM_FFValue(u_caster->SM_Mods->SM_FRadius,&r,GetProto()->GetSpellGroupType());
			SM_PFValue(u_caster->SM_Mods->SM_PRadius,&r,GetProto()->GetSpellGroupType());
		}
	}
	float srcx=m_caster->GetPositionX(),srcy=m_caster->GetPositionY(),srcz=m_caster->GetPositionZ();
	uint32 targettypecheck = GetTargetCreatureType( GetProto() ) ;
	m_caster->AquireInrangeLock(); //make sure to release lock before exit function !
	InrangeLoopExitAutoCallback AutoLock;
	for(InRangeSetRecProt::iterator itr = m_caster->GetInRangeSetBegin( AutoLock ); itr != m_caster->GetInRangeSetEnd(); itr++ )
	{
		if( !( (*itr)->IsUnit() ) || !SafeUnitCast( *itr )->isAlive() )
			continue;

		if( !TargetTypeCheck( (*itr), targettypecheck) )
			continue;

		if(IsInrange(srcx,srcy,srcz,(*itr),r))
		{
			if( u_caster != NULL )
			{
				if( isFriendly( u_caster, ( *itr ) ) && DidHit(i, ((Unit*)*itr))==SPELL_DID_HIT_SUCCESS)
				{
					m_caster->ReleaseInrangeLock();
					return (*itr)->GetGUID();
				}
			}
			else //cast from GO
			{
				if( m_caster->IsGameObject() && m_caster->GetUInt32Value(OBJECT_FIELD_CREATED_BY) )
				{
					//trap, check not to attack owner and friendly
					Unit *GoCreater = m_caster->GetMapMgr()->GetUnit( m_caster->GetUInt64Value( OBJECT_FIELD_CREATED_BY ) );
					if( isFriendly( GoCreater, SafeUnitCast( *itr ) ) )
					{
						m_caster->ReleaseInrangeLock();
						return (*itr)->GetGUID();
					}
				}
			}
		}
	}
	m_caster->ReleaseInrangeLock();
	return 0;
}

uint8 Spell::DidHitFriendly(uint32 effindex,Unit* target)
{
	if( target == NULL )
		return SPELL_DID_HIT_MISS;

	//note resistchance is vise versa, is full hit chance
	Unit* u_victim = target;

	/************************************************************************/
	/* Check if the target is immune to this spell school                   */
	/* Unless the spell would actually dispel invulnerabilities             */
	/************************************************************************/
	// ice block and divine shield should allow incomming heals
	if( u_victim->SchoolImmunityAntiFriend[GetProto()->School] )
		return SPELL_DID_HIT_IMMUNE;

	/************************************************************************/
	/* Check if the unit is evading                                         */
	/************************************************************************/
	if( u_victim->IsCreature() && u_victim->GetAIInterface()->getAIState() == STATE_EVADE )
		return SPELL_DID_HIT_EVADE;

	/* Check if player target has god mode */
	//Zack : i want in debug mode to see other effects too
#ifndef _DEBUG
	if( target->IsPlayer() && SafePlayerCast( target )->bInvincible )
		return SPELL_DID_HIT_IMMUNE;
#endif

	return SPELL_DID_HIT_SUCCESS;
}

uint8 Spell::DidHit(uint32 effindex,Unit* target)
{
	//note resistchance is vise versa, is full hit chance
	Unit* u_victim = target;
	Player* p_victim = ( target->IsPlayer() ) ? SafePlayerCast( target ) : NULL;

	//
	float baseresist[3] = { 4.0f, 5.0f, 6.0f };
	int32 lvldiff;
	float resistchance ;
	if( u_victim == NULL )
		return SPELL_DID_HIT_MISS;

	/************************************************************************/
	/* Can't resist non-unit                                                */
	/************************************************************************/
	if(!u_caster)
		return SPELL_DID_HIT_SUCCESS;

	/************************************************************************/
	/* Can't reduce your own spells                                         */
	/************************************************************************/
	if(u_caster == u_victim)
		return SPELL_DID_HIT_SUCCESS;

	/************************************************************************/
	/* Check if the unit is evading                                         */
	/************************************************************************/
	if(u_victim->IsCreature() && u_victim->GetAIInterface()->getAIState()==STATE_EVADE)
		return SPELL_DID_HIT_EVADE;

	/************************************************************************/
	/* Check if the target is immune to this spell school                   */
	/* Unless the spell would actually dispel invulnerabilities             */
	/************************************************************************/
	if( isAttackable( u_caster, u_victim ) )
	{
		if( u_victim->SchoolImmunityAntiEnemy[ GetProto()->School ] && ( GetProto()->c_is_flags3 & SPELL_FLAG3_IS_DISPEL_MECHANIC ) == 0 )
			return SPELL_DID_HIT_IMMUNE;
	} /**/

	/* Check if player target has god mode */
	//Zack : i want in debug mode to see other effects too
#ifndef _DEBUG
	if( p_victim && p_victim->bInvincible )
		return SPELL_DID_HIT_IMMUNE;
#endif

	/*************************************************************************/
	/* Check if the target is immune to this mechanic                        */
	/*************************************************************************/
	if( u_victim != u_caster && u_victim->MechanicsDispels[GetProto()->MechanicsType])
	{
		// Immune - IF, and ONLY IF, there is no damage component!
		if( (GetProto()->c_is_flags & SPELL_FLAG_IS_DAMAGING) == 0 )
			return SPELL_DID_HIT_IMMUNE; // Moved here from Spell::CanCast
	}
	/************************************************************************/
	/* Check if the target has a % resistance to this mechanic              */
	/************************************************************************/
		/* Never mind, it's already done below. Lucky I didn't go through with this, or players would get double resistance. */

	/************************************************************************/
	/* Check if the spell is a melee attack and if it was missed/parried    */
	/************************************************************************/
	uint32 melee_test_result;
	if( GetProto()->is_melee_spell || GetProto()->is_ranged_spell )
	{
		uint32 _type;
		if( GetType() == SPELL_DMG_TYPE_RANGED )
			_type = RANGED;
		else
		{
			if (GetProto()->AttributesExC & FLAGS4_TYPE_OFFHAND)
				_type =  OFFHAND;
			else
				_type = MELEE;
		}

		melee_test_result = u_caster->GetSpellDidHitResult( u_victim, _type, GetProto() );
		if(melee_test_result != SPELL_DID_HIT_SUCCESS)
			return (uint8)melee_test_result;
	}

	/************************************************************************/
	/* Check if the spell is resisted.                                      */
	/************************************************************************/
	if( ( GetProto()->SchoolMask & SCHOOL_MASK_NORMAL )
		&& !GetProto()->MechanicsType 
		)
		return SPELL_DID_HIT_SUCCESS;

	bool pvp =(p_caster && p_victim);

	if(pvp)
		lvldiff = p_victim->getLevel() - p_caster->getLevel();
	else
		lvldiff = u_victim->getLevel() - u_caster->getLevel();
	if (lvldiff < 0)
	{
		resistchance = baseresist[0] +lvldiff;
	}
	else
	{
		if(lvldiff < 3)
		{
//			if(pvp)
//				resistchance = baseresist[lvldiff] + (((float)lvldiff-2.0f)*7.0f);
//			else
				resistchance = baseresist[lvldiff];
		}
		else
		{
			if(pvp)
				resistchance = baseresist[2] + (((float)lvldiff-2.0f)*7.0f);
			else
				resistchance = baseresist[2] + (((float)lvldiff-2.0f)*11.0f);
		}
	}
	//check mechanical resistance
	//i have no idea what is the best pace for this code
	if( GetProto()->MechanicsType < MECHANIC_TOTAL )
	{
		if(p_victim)
			resistchance += p_victim->MechanicsResistancesPCT[GetProto()->MechanicsType];
		else
			resistchance += u_victim->MechanicsResistancesPCT[GetProto()->MechanicsType];
	}
	//rating bonus
	if( p_caster != NULL )
	{
		resistchance -= p_caster->CalcRating( PLAYER_RATING_MODIFIER_SPELL_HIT );
		resistchance -= p_caster->GetHitFromSpell();

		//special aura bound handlers. Ex : Detterence needs to negate most casts except a few
		if( p_caster->mSpellCanTargetedOverrideMap.HasValues() )
		{
			SimplePointerListNode<SpellCanTargetedScript> *itr;
			for( itr = p_caster->mSpellCanTargetedOverrideMap.begin(); itr != p_caster->mSpellCanTargetedOverrideMap.end(); itr = itr->Next() )
			{
				uint32 NewError = itr->data->CanTargetedFilterHandler( this, p_caster, target, itr->data );
				if( NewError != SPELL_DID_HIT_SUCCESS )
					return NewError;
			}
		}
	}

	if(p_victim && GetProto()->School != 0)
		resistchance += p_victim->m_resist_hit[ MOD_SPELL ];

	if( this->GetProto()->eff[effindex].Effect == SPELL_EFFECT_DISPEL && Need_SM_Apply(GetProto()) && u_victim && u_victim->SM_Mods )
	{
		SM_FFValue(u_victim->SM_Mods->SM_FRezist_dispell,&resistchance,GetProto()->GetSpellGroupType());
		SM_PFValue(u_victim->SM_Mods->SM_PRezist_dispell,&resistchance,GetProto()->GetSpellGroupType());
#ifdef COLLECTION_OF_UNTESTED_STUFF_AND_TESTERS
		int spell_flat_modifers=0;
		int spell_pct_modifers=0;
		SM_FIValue(u_caster->SM_FRezist_dispell,&spell_flat_modifers,GetProto()->GetSpellGroupType());
		SM_FIValue(u_caster->SM_PRezist_dispell,&spell_pct_modifers,GetProto()->GetSpellGroupType());
		if(spell_flat_modifers!=0 || spell_pct_modifers!=0)
			printf("!!!!!spell dipell resist mod flat %d , spell dipell resist mod pct %d , spell dipell resist %d, spell group %u\n",spell_flat_modifers,spell_pct_modifers,resistchance,GetProto()->GetSpellGroupType());
#endif
	}

	if(Need_SM_Apply(GetProto()) && u_caster && u_caster->SM_Mods )
	{
		float hitchance=0;
		SM_FFValue(u_caster->SM_Mods->SM_FHitchance,&hitchance,GetProto()->GetSpellGroupType());
		resistchance -= hitchance;
#ifdef COLLECTION_OF_UNTESTED_STUFF_AND_TESTERS
		float spell_flat_modifers=0;
		SM_FFValue(u_caster->SM_FHitchance,&spell_flat_modifers,GetProto()->GetSpellGroupType());
		if(spell_flat_modifers!=0 )
			printf("!!!!!spell to hit mod flat %f, spell resist chance %f, spell group %u\n",spell_flat_modifers,resistchance,GetProto()->GetSpellGroupType());
#endif
	}

	if (GetProto()->Attributes & ATTRIBUTES_IGNORE_INVULNERABILITY)
		resistchance = 0.0f;

	if(resistchance >= 100.0f)
		return SPELL_DID_HIT_RESIST;
	else
	{
		uint8 res = SPELL_DID_HIT_SUCCESS;
//		if(resistchance<=1.0)//resist chance >=1
//			res =  (Rand(1.0f) ? SPELL_DID_HIT_RESIST : SPELL_DID_HIT_SUCCESS);
		if( resistchance > 1.0f )
			res = (RandChance(resistchance) ? SPELL_DID_HIT_RESIST : SPELL_DID_HIT_SUCCESS);

		if (res == SPELL_DID_HIT_SUCCESS) // proc handling. mb should be moved outside this function
		{
//			u_caster->HandleProc(PROC_ON_SPELL_LAND,target,GetProto());
		}

		return res;
	}

}
//generate possible target list for a spell. Use as last resort since it is not acurate
//this function makes a rough estimation for possible target !
//!!!disabled parts that were not tested !!
void Spell::GenerateTargets(SpellCastTargets *store_buff)
{
	float r = GetProto()->base_range_or_radius_sqr;
	if( Need_SM_Apply(GetProto()) && u_caster && u_caster->SM_Mods )
	{
		SM_FFValue(u_caster->SM_Mods->SM_FRadius,&r,GetProto()->GetSpellGroupType());
		SM_PFValue(u_caster->SM_Mods->SM_PRadius,&r,GetProto()->GetSpellGroupType());
	}
	uint32 cur;
	for(uint32 i=0;i<MAX_SPELL_EFFECT_COUNT;i++)
		for(uint32 j=0;j<2;j++)
		{
			if(j==0)
				cur = GetProto()->eff[i].EffectImplicitTargetA;
			else // if(j==1)
				cur = GetProto()->eff[i].EffectImplicitTargetB;
			switch(cur)
			{
				case EFF_TARGET_NONE:{
					//this is bad for us :(
					}break;
				case EFF_TARGET_SELF:{
						if(m_caster->IsUnit())
							store_buff->m_unitTarget = m_caster->GetGUID();
					}break;
					// need more research
				case 4:{ // dono related to "Wandering Plague", "Spirit Steal", "Contagion of Rot", "Retching Plague" and "Copy of Wandering Plague"
					}break;
				case EFF_TARGET_PET:
					{// Target: Pet
						if(p_caster && p_caster->GetSummon())
							store_buff->m_unitTarget = p_caster->GetSummon()->GetGUID();
					}break;
				case EFF_TARGET_SINGLE_ENEMY:// Single Target Enemy
				case 77:					// grep: i think this fits
				case 8: // related to Chess Move (DND), Firecrackers, Spotlight, aedm, Spice Mortar
				case EFF_TARGET_ALL_ENEMY_IN_AREA: // All Enemies in Area of Effect (TEST)
				case EFF_TARGET_ALL_ENEMY_IN_AREA_INSTANT: // All Enemies in Area of Effect instant (e.g. Flamestrike)
				case EFF_TARGET_ALL_ENEMIES_AROUND_CASTER:
				case EFF_TARGET_IN_FRONT_OF_CASTER:
				case EFF_TARGET_ALL_ENEMY_IN_AREA_CHANNELED:// All Enemies in Area of Effect(Blizzard/Rain of Fire/volley) channeled
				case 31:// related to scripted effects
				case 53:// Target Area by Players CurrentSelection()
				case 54:// Targets in Front of the Caster
					{
						if( p_caster != NULL )
						{
							Unit *selected = p_caster->GetMapMgr()->GetUnit(p_caster->GetSelection());
							if( isAttackable(p_caster,selected,!(GetProto()->c_is_flags & SPELL_FLAG_IS_TARGETINGSTEALTHED)) 
								&& ( sWorld.Collision == false || CollideInterface.CheckLOS( p_caster->GetMapId(), p_caster->GetPositionNC(), selected->GetPositionNC() ) )
								)
								store_buff->m_unitTarget = p_caster->GetSelection();
						}
						else if( u_caster != NULL )
						{
							if(	u_caster->GetAIInterface()->GetNextTarget() &&
								isAttackable(u_caster,u_caster->GetAIInterface()->GetNextTarget(),!(GetProto()->c_is_flags & SPELL_FLAG_IS_TARGETINGSTEALTHED)) &&
								u_caster->GetDistanceSq(u_caster->GetAIInterface()->GetNextTarget()) <= r 
								&& ( sWorld.Collision == false || CollideInterface.CheckLOS( u_caster->GetMapId(), u_caster->GetPositionNC(), u_caster->GetAIInterface()->GetNextTarget()->GetPositionNC() ) )
								)
							{
								store_buff->m_unitTarget = u_caster->GetAIInterface()->GetNextTarget()->GetGUID();
							}
							if(u_caster->GetAIInterface()->getAITargetsCount() && u_caster->GetMapMgr())
							{
								//try to get most hated creature
								u_caster->GetAIInterface()->LockAITargets(true);
								TargetMap *m_aiTargets = u_caster->GetAIInterface()->GetAITargets();
								TargetMap::iterator itr;
								for(itr = m_aiTargets->begin(); itr != m_aiTargets->end();itr++)
								{
									Unit *hate_t = u_caster->GetMapMgr()->GetUnit( itr->first );
									if( /*m_caster->GetMapMgr()->GetUnit(itr->first->GetGUID()) &&*/
										hate_t &&
										hate_t->GetMapMgr() == m_caster->GetMapMgr() &&
										hate_t->isAlive() &&
										m_caster->GetDistanceSq(hate_t) <= r &&
										isAttackable(u_caster,hate_t,!(GetProto()->c_is_flags & SPELL_FLAG_IS_TARGETINGSTEALTHED))
										&& ( sWorld.Collision == false || CollideInterface.CheckLOS( u_caster->GetMapId(), u_caster->GetPositionNC(), hate_t->GetPositionNC() ) )
										)
									{
										store_buff->m_unitTarget=itr->first;
										break;
									}
								}
								u_caster->GetAIInterface()->LockAITargets(false);
							}
							//totems do not have aitargets
							else if( u_caster->IsCreature() && SafeCreatureCast( u_caster )->IsTotem() )
							{
								SpellTargetAreaOfEffect( i, j );
							}
						}
						//try to get a whatever target
						if(!store_buff->m_unitTarget)
						{
							store_buff->m_unitTarget=GetSinglePossibleEnemy(i);
						}
						//if we still couldn't get a target, check maybe we could use
//						if(!store_buff->m_unitTarget)
//						{
//						}
					}break;
					// spells like 17278:Cannon Fire and 21117:Summon Son of Flame A
				case 17: // A single target at a xyz location or the target is a possition xyz
				case 18:// Land under caster.Maybe not correct
					{
						store_buff->m_srcX=m_caster->GetPositionX();
						store_buff->m_srcY=m_caster->GetPositionY();
						store_buff->m_srcZ=m_caster->GetPositionZ();
						store_buff->m_targetMask |= TARGET_FLAG_SOURCE_LOCATION;
					}break;
				case EFF_TARGET_ALL_RAID:
					{
						Player* p = p_caster;
						if( p == NULL)
						{
							if( u_caster->IsCreature() && SafeCreatureCast( u_caster )->IsTotem() )
								p = SafePlayerCast( SafeCreatureCast( u_caster )->GetTotemOwner() );
						}
						if( p != NULL )
						{
							if(IsInrange(m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),p,r))
							{
								store_buff->m_unitTarget = m_caster->GetGUID();
								break;
							}
							if( p->GetGroup() )
							{
								uint32 count = p->GetGroup()->GetSubGroupCount();
								for( uint8 k = 0; k < count; k++ )
								{
									SubGroup * subgroup = p->GetGroup()->GetSubGroup( k );
									if( subgroup )
									{
										p->GetGroup()->Lock();
										for(GroupMembersSet::iterator itr = subgroup->GetGroupMembersBegin(); itr != subgroup->GetGroupMembersEnd(); ++itr)
										{
											if(!(*itr)->m_loggedInPlayer || m_caster == (*itr)->m_loggedInPlayer)
												continue;
											if(IsInrange(m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),(*itr)->m_loggedInPlayer,r))
											{
												store_buff->m_unitTarget = (*itr)->m_loggedInPlayer->GetGUID();
												break;
											}
										}
										p->GetGroup()->Unlock();
									}
								}
							}
						}
					}break;
				case EFF_TARGET_ALL_PARTY_AROUND_CASTER:
					{// All Party Members around the Caster in given range NOT RAID!
						Player* p = p_caster;
						if( p == NULL)
						{
							if( u_caster->IsCreature() && SafeCreatureCast( u_caster )->IsTotem() )
								p = SafePlayerCast( SafeCreatureCast( u_caster )->GetTotemOwner() );
						}
						if( p != NULL )
						{
							if(IsInrange(m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),p,r))
							{
								store_buff->m_unitTarget = m_caster->GetGUID();
								break;
							}
							SubGroup * subgroup = p->GetGroup() ?
								p->GetGroup()->GetSubGroup(p->GetSubGroup()) : 0;

							if(subgroup)
							{
								p->GetGroup()->Lock();
								for(GroupMembersSet::iterator itr = subgroup->GetGroupMembersBegin(); itr != subgroup->GetGroupMembersEnd(); ++itr)
								{
									if(!(*itr)->m_loggedInPlayer || m_caster == (*itr)->m_loggedInPlayer)
										continue;
									if(IsInrange(m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),(*itr)->m_loggedInPlayer,r))
									{
										store_buff->m_unitTarget = (*itr)->m_loggedInPlayer->GetGUID();
										break;
									}
								}
								p->GetGroup()->Unlock();
							}
						}

/*						if ( u_caster != NULL && u_caster->IsCreature() )
						{
							//target friendly npcs
							for( InRangeSet::iterator itr = u_caster->GetInRangeSameFactsSetBegin(); itr != u_caster->GetInRangeSameFactsSetEnd(); itr++ )
							{
								if ( (*itr) != NULL && ((*itr)->GetTypeId() == TYPEID_UNIT || (*itr)->GetTypeId() == TYPEID_PLAYER) && (*itr)->IsInWorld() && ((Unit*)*itr)->isAlive() && IsInrange(u_caster, (*itr), r) )
								{
									store_buff->m_unitTarget = (*itr)->GetGUID();
									break;
								}
							}
						}*/
					}break;
				case EFF_TARGET_SINGLE_FRIEND:
				case 45:// Chain,!!only for healing!! for chain lightning =6
				case 57:// Targeted Party Member
					{// Single Target Friend
						if( p_caster != NULL )
						{
							if(isFriendly(p_caster,p_caster->GetMapMgr()->GetUnit(p_caster->GetSelection())))
								store_buff->m_unitTarget = p_caster->GetSelection();
							else store_buff->m_unitTarget = p_caster->GetGUID();
						}
						else if( u_caster != NULL )
						{
							if(u_caster->GetUInt64Value(UNIT_FIELD_CREATEDBY))
								store_buff->m_unitTarget = u_caster->GetUInt64Value(UNIT_FIELD_CREATEDBY);
							 else
							 {
								//target friendly npcs
/*								for( InRangeSet::iterator itr = u_caster->GetInRangeSameFactsSetBegin(); itr != u_caster->GetInRangeSameFactsSetEnd(); itr++ )
								{
									if ( (*itr) != NULL && ((*itr)->GetTypeId() == TYPEID_UNIT || (*itr)->GetTypeId() == TYPEID_PLAYER) && (*itr)->IsInWorld() && ((Unit*)*itr)->isAlive() && IsInrange(u_caster, (*itr), r) )
									{

										//few additional checks
										if (IsHealingSpell(GetProto()) && ((Unit*)*itr)->GetHealthPct() == 100 && !((Unit*)*itr)->HasAura(GetProto()->Id) 
										//!((Unit*)*itr)->HasActiveAura(GetProto()->Id, m_caster->GetGUID())
								 )
											continue;

										//check if an aura is being applied, and check if it already exists
										bool applies_aura=false;
										for (int i=0; i<3; i++)
										{
											if (GetProto()->eff[i].Effect == SPELL_EFFECT_APPLY_AURA || GetProto()->eff[i].Effect == SPELL_EFFECT_APPLY_AREA_AURA)
											{
												applies_aura=true;
												break;
											}
										}

										//majority of healing spells stack, infact I think they all do as of 2.0.1
										if (!IsHealingSpell(GetProto()) && applies_aura && ((Unit*)*itr)->HasAura(GetProto()->Id))
											continue;


										store_buff->m_unitTarget = (*itr)->GetGUID();
										break;
									 }
								}*/
							}
						}
					}break;
				case EFF_TARGET_GAMEOBJECT:
					{
						if(p_caster && p_caster->GetSelection())
							store_buff->m_unitTarget = p_caster->GetSelection();
					}break;
				case EFF_TARGET_DUEL:
					{// Single Target Friend Used in Duel
						if(p_caster && p_caster->DuelingWith && p_caster->DuelingWith->isAlive() && IsInrange(p_caster,p_caster->DuelingWith,r))
							store_buff->m_unitTarget = p_caster->GetSelection();
					}break;
				case EFF_TARGET_GAMEOBJECT_ITEM:{// Gameobject/Item Target
						//shit
					}break;
				case 27:{ // target is owner of pet
					// please correct this if not correct does the caster variablen need a Pet caster variable?
						if(u_caster && u_caster->IsPet())
							store_buff->m_unitTarget = SafePetCast( u_caster )->GetPetOwner()->GetGUID();
					}break;
				case EFF_TARGET_MINION:
				case 73:
					{// Minion Target
						if(m_caster->GetUInt64Value(UNIT_FIELD_SUMMON) == 0)
							store_buff->m_unitTarget = m_caster->GetGUID();
						else store_buff->m_unitTarget = m_caster->GetUInt64Value(UNIT_FIELD_SUMMON);
					}break;
				case 33://Party members of totem, inside given range
				case EFF_TARGET_SINGLE_PARTY:// Single Target Party Member
				case EFF_TARGET_ALL_PARTY: // all Members of the targets party
					{
						Player *p=NULL;
						if( p_caster != NULL )
								p = p_caster;
						else if( u_caster->IsCreature() && SafeCreatureCast( u_caster )->IsTotem() )
								p = SafePlayerCast( SafeCreatureCast( u_caster )->GetTotemOwner() );
						if( p_caster != NULL )
						{
							if(IsInrange(m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),p,r))
							{
								store_buff->m_unitTarget = p->GetGUID();
								break;
							}
							SubGroup * pGroup = p_caster->GetGroup() ?
								p_caster->GetGroup()->GetSubGroup(p_caster->GetSubGroup()) : 0;

							if( pGroup )
							{
								p_caster->GetGroup()->Lock();
								for(GroupMembersSet::iterator itr = pGroup->GetGroupMembersBegin();
									itr != pGroup->GetGroupMembersEnd(); ++itr)
								{
									if(!(*itr)->m_loggedInPlayer || p == (*itr)->m_loggedInPlayer)
										continue;
									if(IsInrange(m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),(*itr)->m_loggedInPlayer,r))
									{
										store_buff->m_unitTarget = (*itr)->m_loggedInPlayer->GetGUID();
										break;
									}
								}
								p_caster->GetGroup()->Unlock();
							}
						}

/*						if ( u_caster != NULL && u_caster->IsCreature() )
						{
							//target friendly npcs
							for( InRangeSet::iterator itr = u_caster->GetInRangeSameFactsSetBegin(); itr != u_caster->GetInRangeSameFactsSetEnd(); itr++ )
							{
								if ( (*itr) != NULL && ((*itr)->GetTypeId() == TYPEID_UNIT || (*itr)->GetTypeId() == TYPEID_PLAYER) && (*itr)->IsInWorld() && ((Unit*)*itr)->isAlive() && IsInrange(u_caster, (*itr), r) )								{
									store_buff->m_unitTarget = (*itr)->GetGUID();
									break;
								}
							}
						}*/
					}break;
				case 38:{//Dummy Target
					//have no idea
					}break;
				case EFF_TARGET_SELF_FISHING://Fishing
				case 46://Unknown Summon Atal'ai Skeleton
				case 47:// Portal
				case 52:	// Lightwells, etc
					{
						store_buff->m_unitTarget = m_caster->GetGUID();
					}break;
				case 40://Activate Object target(probably based on focus)
				case EFF_TARGET_TOTEM_EARTH:
				case EFF_TARGET_TOTEM_WATER:
				case EFF_TARGET_TOTEM_AIR:
				case EFF_TARGET_TOTEM_FIRE:// Totem
					{
						if( p_caster != NULL )
						{
							uint32 slot = cur - EFF_TARGET_TOTEM_EARTH;
							if( slot < 4 && p_caster->m_TotemSlots[slot] != 0 )
								store_buff->m_unitTarget = p_caster->m_TotemSlots[slot]->GetGUID();
						}
					}break;
				case 61:{ // targets with the same group/raid and the same class
					//shit again
				}break;
				case EFF_TARGET_ALL_FRIENDLY_IN_AREA:{

				}break;

			}//end switch
		}//end for
	if(store_buff->m_unitTarget)
		store_buff->m_targetMask |= TARGET_FLAG_UNIT;
	if(store_buff->m_srcX)
		store_buff->m_targetMask |= TARGET_FLAG_SOURCE_LOCATION;
	if(store_buff->m_destX)
		store_buff->m_targetMask |= TARGET_FLAG_DEST_LOCATION;
}//end function

uint8 Spell::prepare( SpellCastTargets * targets )
{
	uint8 ccr;

	//what about passive spells casted on player on login ?
//	if( !m_caster->IsInWorld() )
//	{
//		sLog.outDebug("Object "I64FMT" is casting Spell ID %u while not in World", m_caster->GetGUID(), GetProto()->Id);
//		SpellPool.PooledDelete( this );	//prepare early exit, cast / finish / cancel is not called
//		return SPELL_FAILED_DONT_REPORT;
//	}
	if( IsSpellDisabled( GetProto() ) )
	{ 
		SpellPool.PooledDelete( this, __FILE__, __LINE__ ); //prepare early exit, cast / finish / cancel is not called
		return SPELL_FAILED_SPELL_UNAVAILABLE;
	}
	if( GetProto()->RequiredPlayerClass 
		&& m_caster->IsUnit() 
		&& ( GetProto()->RequiredPlayerClass & (1 << SafeUnitCast(m_caster)->getClass() ) ) == 0 )
	{ 
		SpellPool.PooledDelete( this, __FILE__, __LINE__ ); //prepare early exit, cast / finish / cancel is not called
		return SPELL_FAILED_SPELL_UNAVAILABLE;
	}

	//crap hackfix because in some places we forgot to set on of the legacy values
	if( pSpellId != 0 && ProcedOnSpell == NULL )
		ProcedOnSpell = dbcSpell.LookupEntryForced( pSpellId );
	//crap hackfix because in some places we forgot to set on of the legacy values
	if( pSpellId == 0 && ProcedOnSpell != NULL )
		pSpellId = ProcedOnSpell->Id;

	//check if this spell is a non player spell and report the usage of it for GMs
	if( GetProto()->spell_skilline[0] == 0
		&& pSpellId == 0
		&& p_caster && p_caster->GetSession() && p_caster->GetSession()->GetPlayer() && p_caster->GetSession()->HasPermissions() && p_caster->GetMapMgr() )
	{
		Unit *target = p_caster->GetMapMgr()->GetUnit( targets->m_unitTarget );
		if( p_caster != target )
		{
			if( target && target->IsPlayer() )
				sGMLog.writefromsession(p_caster->GetSession(),"Casted non player spell %u -> %s on player %s",GetProto()->Id, GetProto()->Name, SafePlayerCast(target)->GetName() );
			else if( target && target->IsCreature() && (SafeCreatureCast(target))->GetCreatureInfo() )
				sGMLog.writefromsession(p_caster->GetSession(),"Casted non player spell %u -> %s on creature %s",GetProto()->Id, GetProto()->Name,SafeCreatureCast(target)->GetCreatureInfo()->Name );
			else 
				sGMLog.writefromsession(p_caster->GetSession(),"Casted non player spell %u -> %s on wtf no targets ?",GetProto()->Id, GetProto()->Name );
		}
	}

	// In case spell got cast from a script check fear/wander states
	if (!p_caster && u_caster && u_caster->GetAIInterface())
	{
		AIInterface *ai = u_caster->GetAIInterface();
		if (ai->getAIState() == STATE_FEAR || ai->getAIState() == STATE_WANDER)
		{ 
			SpellPool.PooledDelete( this, __FILE__, __LINE__ ); //prepare early exit, cast / finish / cancel is not called
			return SPELL_FAILED_NOT_READY;
		}
	}

	m_targets = *targets;

	if( pSpellId || ( p_caster != NULL && p_caster->CastTimeCheat ) )
		m_castTime = 0;
	else
	{
		m_castTime = 0;
		uint32 m_castTimeBase = 0;
		if( GetProto()->SpellScalingId )
		{
			SpellScalingEntry *ss = &GetProto()->ss;
			if( ss )
			{
				if( (int32)u_caster->getLevel() > ss->castScalingMaxLevel )
					m_castTime = ss->castTimeMax;
				else if( ss->castTimeMin > 0 )
				{
					float diff = (float)((int32)ss->castTimeMax - (int32)ss->castTimeMin); 
					float diff_per_level = diff / (float)ss->castScalingMaxLevel;
					m_castTime = float2int32( ss->castTimeMin + diff_per_level * u_caster->getLevel() );
					//avoid bugs
					if( m_castTime > (int32)ss->castTimeMax )
						m_castTime = (int32)ss->castTimeMax;
				}
			}
		}
		if( m_castTime == 0 && GetProto()->CastingTimeIndex )
			m_castTime = GetCastTime( dbcSpellCastTime.LookupEntry( GetProto()->CastingTimeIndex ) );

		if( m_castTime && Need_SM_Apply(GetProto()) && u_caster != NULL && u_caster->SM_Mods )
		{
			SM_FIValue( u_caster->SM_Mods->SM_FCastTime, (int32*)&m_castTime, GetProto()->GetSpellGroupType() );
			SM_PIValue( u_caster->SM_Mods->SM_PCastTime, (int32*)&m_castTime, GetProto()->GetSpellGroupType() );
#ifdef COLLECTION_OF_UNTESTED_STUFF_AND_TESTERS
			int spell_flat_modifers=0;
			int spell_pct_modifers=0;
			SM_FIValue(u_caster->SM_FCastTime,&spell_flat_modifers,GetProto()->GetSpellGroupType());
			SM_FIValue(u_caster->SM_PCastTime,&spell_pct_modifers,GetProto()->GetSpellGroupType());
			if(spell_flat_modifers!=0 || spell_pct_modifers!=0)
				printf("!!!!!spell casttime mod flat %d , spell casttime mod pct %d , spell casttime %d, spell group %u\n",spell_flat_modifers,spell_pct_modifers,m_castTime,GetProto()->GetSpellGroupType());
#endif
		}

		// handle MOD_CAST_TIME
		if( m_castTime > 0 && u_caster != NULL && i_caster == NULL && ( GetProto()->c_is_flags3 & SPELL_FLAG3_IS_NOT_USING_HASTE ) == 0 )
			m_castTime = float2int32( m_castTime * u_caster->GetFloatValue( UNIT_MOD_CAST_SPEED ) );
//			m_castTime = float2int32( m_castTime * MIN( 1.0f, u_caster->GetSpellHaste() ) ); //not sure if this is true
	}

/*	if( p_caster != NULL )
	{
		if( p_caster->cannibalize )
		{
			sEventMgr.RemoveEvents( p_caster, EVENT_CANNIBALIZE );
			p_caster->SetUInt32Value( UNIT_NPC_EMOTESTATE, 0 );
			p_caster->cannibalize = false;
		}
	} */

	//let us make sure cast_time is within decent range
	//this is a hax but there is no spell that has more then 10 minutes cast time

	if( m_castTime < 0 )
		m_castTime = 0;
	else if( m_castTime > 60 * 10 * 1000)
		m_castTime = 60 * 10 * 1000; //we should limit cast time to 10 minutes right ?

	m_timer = m_castTime;

	//if( p_caster != NULL )
	//   m_castTime -= 100;	  // session update time


	m_spellState = SPELL_STATE_PREPARING;

	if( pSpellId )
		cancastresult = SPELL_CANCAST_OK;
	else
		cancastresult = CanCast(false);

	//sLog.outString( "CanCast result: %u. Refer to SpellFailure.h to work out why." , cancastresult );

	ccr = cancastresult;
	if( cancastresult != SPELL_CANCAST_OK )
	{
		SendCastResult( cancastresult );

		if( pSpellId == 0 )	//do not cancel channeled spells by triggered spells
		{
			if( m_triggeredByAura )
			{
				SendChannelUpdate( 0 );
				if( u_caster != NULL )
					m_triggeredByAura->Remove();
			}
			else 
			{
				// HACK, real problem is the way spells are handled
				// when a spell is channeling and a new spell is casted
				// that is a channeling spell, but not triggert by a aura
				// the channel bar/spell is bugged
				if( u_caster && u_caster->GetUInt64Value( UNIT_FIELD_CHANNEL_OBJECT ) > 0 && u_caster->GetCurrentSpell() )
				{
					u_caster->GetCurrentSpell()->cancel();
					SendChannelUpdate( 0 );
					cancel();
//					SpellPool.PooledDelete( this ); //prepare early exit, cast / finish / cancel is not called
					sLog.outDebug("PrepareCast: Already channeling");
					return ccr;
				}
			}
		}
		finish();
		sLog.outDebug("PrepareCast: Cannot cast spell %s due to cancastresult", GetProto()->Name );
		return ccr;
	}
	else
	{
		//this is called both on prepare and cast. Some spells do not have prepare time and insta cast
		if( !pSpellId )
			CheckAndInterruptStealth();

		SendSpellStart();

		// start cooldown handler
		if( p_caster != NULL && !p_caster->CastTimeCheat && !pSpellId )
			AddStartCooldown();

		if( i_caster == NULL && p_caster != NULL && m_timer > 0 && !pSpellId)
			p_caster->delayAttackTimer( m_timer + 1000 );
			//p_caster->setAttackTimer(m_timer + 1000, false);

		// aura state removal
		if( GetProto()->RemoveCasterAuraState )
			u_caster->RemoveAuraStateFlag( GetProto()->RemoveCasterAuraState );

		if( u_caster && !pSpellId && !m_triggeredByAura )
		{
			uint32 Flags = AURA_INTERRUPT_ON_CAST_SPELL;
			if( GetProto()->MechanicsTypeFlags & (1<<MECHANIC_MOUNTED ) )
				Flags |= AURA_INTERRUPT_ON_MOUNT;
			u_caster->RemoveAurasByInterruptFlagButSkip( Flags, GetProto()->Id, GetProto()->NameHash);
		}
	}

	//instant cast(or triggered) and not channeling
	if( u_caster != NULL && ( m_castTime > 0 || GetProto()->ChannelInterruptFlags ) && !pSpellId )
	{
		m_castPositionX = m_caster->GetPositionX();
		m_castPositionY = m_caster->GetPositionY();
		m_castPositionZ = m_caster->GetPositionZ();

		u_caster->castSpell( this );
	}
	else
	{
		cast( false );
	}

	return ccr;
}

void Spell::cancel()
{
	if (m_spellState == SPELL_STATE_FINISHED)
	{ 
		return;
	}

	//avoid multiple cancels
	if( u_caster != NULL )
		u_caster->SetCurrentSpell( NULL );

	SendInterrupted(0);
	SendCastResult(SPELL_FAILED_INTERRUPTED);

	if(m_spellState == SPELL_STATE_CASTING)
	{
		if( u_caster != NULL )
			u_caster->RemoveAura( GetProto()->Id,0,AURA_SEARCH_POSITIVE );

		if(m_timer > 0 || m_Delayed)
		{
			if(m_caster && m_caster->IsInWorld())
			{
				Unit *pTarget = m_caster->GetMapMgr()->GetUnit( m_caster->GetUInt64Value( UNIT_FIELD_CHANNEL_OBJECT ) );
				if( !pTarget && p_caster )
					pTarget = p_caster->GetMapMgr()->GetUnit( p_caster->GetSelection() );

				if(pTarget)
				{
					pTarget->RemoveAura(GetProto()->Id, m_caster->GetGUID(),AURA_SEARCH_ALL);
				}
				if(m_AreaAura)//remove of blizz and shit like this
				{

					DynamicObject* dynObj=m_caster->GetMapMgr()->GetDynamicObject(m_caster->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT));
					if(dynObj)
						dynObj->Remove();
				}

				if( p_caster )
				{
					if (m_timer > 0)
						p_caster->delayAttackTimer(-m_timer);
					if(p_caster->GetSummonedObject())
					{
						if(p_caster->GetSummonedObject()->IsInWorld())
							p_caster->GetSummonedObject()->RemoveFromWorld(true);
						// for now..
						ASSERT(p_caster->GetSummonedObject()->GetTypeId() == TYPEID_GAMEOBJECT);
						sGarbageCollection.AddObject( p_caster->GetSummonedObject() );
						p_caster->SetSummonedObject(NULL);
					}
//					p_caster->setAttackTimer(1000, false);
				}
			 }
		}
	}
	SendChannelUpdate(0);

	//m_spellState = SPELL_STATE_FINISHED;

	// prevent memory corruption. free it up later.
	// if this is true it means we are currently in the cast() function somewhere else down the stack
	// (recursive spells) and we don't wanna have this class delete'd when we return to it.
	// at the end of cast() it will get freed anyway.
//	if( !m_isCasting )
		finish();
}

void Spell::AddCooldown()
{
	Player *tp_caster;
	uint32 ItemOrPetNumber = 0;

	if( p_caster )
		tp_caster = p_caster;
	else if( u_caster->IsPet() )
	{
		tp_caster = SafePetCast( u_caster )->GetPetOwner();
		ItemOrPetNumber = SafePetCast( u_caster )->GetPetNumber();
		SafePetCast( u_caster )->Cooldown_Add( GetProto() );
	}
	else
		tp_caster = NULL;

	if( i_caster )
		ItemOrPetNumber = i_caster->GetProto()->ItemId;

	if( tp_caster != NULL )
		tp_caster->Cooldown_Add( GetProto(), ItemOrPetNumber );
}

void Spell::AddStartCooldown()
{
	Player *tp_caster;
	if( p_caster )
		tp_caster = p_caster;
	else if( u_caster->IsPet() )
		tp_caster = SafePetCast( u_caster )->GetPetOwner();
	else
		tp_caster = NULL;
	if( tp_caster != NULL )
		tp_caster->Cooldown_AddStart( GetProto() );
}

void Spell::cast(bool check)
{
	//this should never happen
	if( m_cancelled )
	{
		cancel();
		return;
	}
	//even proc spells in process of casting can be queried. Pls note that this might get lost in case chained cast happens
	if( p_caster )
	{
		if( p_caster->IsSpectator() )
		{
			SendInterrupted( SPELL_FAILED_TARGET_FRIENDLY );
			finish();
			sLog.outDebug("SpellCast: Spectator mode");
			return;
		}
		p_caster->m_currentSpellAny = this;
	}

	if( duelSpell && (
		( p_caster != NULL && p_caster->GetDuelState() != DUEL_STATE_STARTED ) ||
		( u_caster != NULL && u_caster->IsPet() && SafePetCast( u_caster )->GetPetOwner() && SafePetCast( u_caster )->GetPetOwner()->GetDuelState() != DUEL_STATE_STARTED ) ) )
	{
		// Can't cast that!
		SendInterrupted( SPELL_FAILED_TARGET_FRIENDLY );
		finish();
		sLog.outDebug("SpellCast: Friendly target");
		return;
	}
//if( p_caster ) p_caster->BroadcastMessage( "Actually casting spell %s at %d", GetProto()->Name, getMSTime() );
	if( p_caster )
		sLog.outDebug("Spell::cast %u(%s), Player: %u", GetProto()->Id,GetProto()->Name, m_caster->GetLowGUID());
	else
		sLog.outDebug("Spell::cast %u(%s), Unit: %u", GetProto()->Id,GetProto()->Name, m_caster->GetLowGUID());

	if(check)
		cancastresult = CanCast(true);
	else
		cancastresult = SPELL_CANCAST_OK;

	if(cancastresult == SPELL_CANCAST_OK)
	{
		//some talents are able to redirect the effect of a spell to another effect. We are scripting these to boost lookup
		//handlers will set the "redirected_effect" variable of the spell to skip handling in the original spell
		if( GetProto()->EffRedirectHandler )
		{
			GetProto()->EffRedirectHandler( this );
			//can happen
			if( deleted )
			{
				sLog.outDebug("SpellCast: Script cast interrupted");
				return;
			}
		}

		if( GetProto()->Attributes & ATTRIBUTE_ON_NEXT_ATTACK)
		{
			if(!pSpellId)
			{
				// on next attack - we don't take the mana till it actually attacks.
				if(!HasPower())
				{
					SendInterrupted(SPELL_FAILED_NO_POWER);
					finish();
					sLog.outDebug("SpellCast: Not enough power");
					return;
				}
			}
			else
			{
				// this is the actual spell cast
				if(!TakePower())	// shouldn't happen
				{
					SendInterrupted(SPELL_FAILED_NO_POWER);
					finish();
					return;
				}
			}
		}
		else
		{
			if(!pSpellId)
			{
				if(!TakePower()) //not enough mana
				{
					//sLog.outDebug("Spell::Not Enough Mana");
					SendInterrupted(SPELL_FAILED_NO_POWER);
					finish();
					sLog.outDebug("SpellCast: Not enough power");
					return;
				}
				//this needs to be rewriten when we support vehicle spells. Right now is to avoid passengers stealthing and stuff
				if( p_caster )
					p_caster->ExitVehicle();
			}
		}

#ifdef _DEBUG
		uint32 total_targets_count = 0;
#endif
		for(uint32 i=0;i<MAX_SPELL_EFFECT_COUNT;i++)
        {
			if( 
				GetProto()->eff[i].Effect &&	
				GetProto()->eff[i].Effect != SPELL_EFFECT_PERSISTENT_AREA_AURA &&
				redirected_effect[i] == SPELL_EFFECT_REDIRECT_FLAG_EXECUTE
				)
			{
				 FillTargetMap(i);
#ifdef _DEBUG
				total_targets_count += (uint32)m_targetUnits[i].size();
#endif
			}
        }
#ifdef _DEBUG
		//this might happen for example campfire that spams the burn effect even though without targets
		if( total_targets_count == 0 )
			sLog.outDebug("SpellCast: Warning this spell could not find any targets ( id = %u )",GetProto()->Id );
#endif

#ifdef FORCED_GM_TRAINEE_MODE
		//redirect all spell to the caster
		if( p_caster && p_caster->GetSession() && p_caster->GetSession()->HasPermissions() && !p_caster->GetSession()->HasGMPermissions() )
		{
			for( uint32 i=0;i<MAX_SPELL_EFFECT_COUNT;i++)
				if( m_targetUnits[i].size() )
				{
					m_targetUnits[i].clear();
					m_targetUnits[i].push_back( p_caster->GetGUID() );
					UniqueTargets.clear();
					UniqueTargets.push_back( p_caster->GetGUID() );
				}
		}
#endif

		SendCastResult(cancastresult);
		if(cancastresult != SPELL_CANCAST_OK)
		{
			finish();
			sLog.outDebug("SpellCast: Cannot cast it for some reason");
			return;
		}
//		m_isCasting = true;
		//sLog.outString( "CanCastResult: %u" , cancastresult );
		if(!pSpellId)
			AddCooldown();

		//this is called both on prepare and cast. Some spells do not have prepare time and insta cast
		if(!pSpellId)
			CheckAndInterruptStealth();

		if(!((GetProto()->Attributes & ATTRIBUTE_ON_NEXT_ATTACK ) && pSpellId==false))//on next attack
		{
			SendSpellGo();

			//******************** SHOOT SPELLS ***********************
			//* Flags are now 1,4,19,22 (4718610) //0x480012

			if (GetProto()->AttributesExC & FLAGS4_PLAYER_RANGED_SPELLS && m_caster->IsPlayer() && m_caster->IsInWorld())
			{
                /// Part of this function contains a hack fix
                /// hack fix for shoot spells, should be some other resource for it
                //p_caster->SendSpellCoolDown(GetProto()->Id, GetProto()->RecoveryTime ? GetProto()->RecoveryTime : 2300);
/*
14333
{SERVER} Packet: (0x7277) SMSG_SPELL_COOLDOWN PacketSize = 17 TimeStamp = 9740624
C0 00 05 00 8C 96 50 F1 full guid 
00 ?
91 1C 01 00 spell
88 13 00 00 cooldown
{SERVER} Packet: (0x7277) SMSG_SPELL_COOLDOWN PacketSize = 17 TimeStamp = 10594183
C2 09 05 00 27 94 50 F1 
00 
4C 14 01 00 
B8 0B 00 00 
*/
				sStackWorldPacket(data, SMSG_SPELL_COOLDOWN, 20 );
				data << p_caster->GetGUID();
				data << uint8( 0 );
				data << GetProto()->Id;
				data << uint32(GetProto()->RecoveryTime ? GetProto()->RecoveryTime : 2300);
				p_caster->GetSession()->SendPacket(&data);
			}
			else
			{
				if( GetProto()->ChannelInterruptFlags != 0 && pSpellId == 0 )
				{
					/*
					Channeled spells are handled a little differently. The five second rule starts when the spell's channeling starts; i.e. when you pay the mana for it.
					The rule continues for at least five seconds, and longer if the spell is channeled for more than five seconds. For example,
					Mind Flay channels for 3 seconds and interrupts your regeneration for 5 seconds, while Tranquility channels for 10 seconds
					and interrupts your regeneration for the full 10 seconds.
					*/

					uint32 channelDuration = GetDuration();
					if( u_caster )
					{
						if( i_caster == NULL )
							channelDuration = (uint32)(channelDuration * u_caster->GetSpellHaste());
						if( m_targets.m_unitTarget != 0 && ( GET_TYPE_FROM_GUID( m_targets.m_unitTarget ) == HIGHGUID_TYPE_PLAYER ) )
						{
							Unit *MainTarget = u_caster->GetMapMgr()->GetPlayer( m_targets.m_unitTarget );
							if( MainTarget )
								::ApplyDiminishingReturnTimer( &channelDuration, MainTarget, GetProto()->DiminishStatus, true );
						}
					}
					m_spellState = SPELL_STATE_CASTING;
					SendChannelStart(channelDuration);
					if( p_caster != NULL )
					{
						//Use channel interrupt flags here
						if( p_caster->GetSummon() && ( GetProto()->eff[0].EffectImplicitTargetA == EFF_TARGET_PET || GetProto()->eff[0].EffectImplicitTargetB == EFF_TARGET_PET ) )
							u_caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, p_caster->GetSummon()->GetGUID() );	//not sure if this would work for health funnel
						else if(m_targets.m_targetMask == TARGET_FLAG_DEST_LOCATION || m_targets.m_targetMask == TARGET_FLAG_SOURCE_LOCATION)
							u_caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, p_caster->GetSelection());
						else if( m_targets.m_targetMask == TARGET_FLAG_ITEM )
							u_caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, m_targets.m_itemTarget );
						else if(p_caster->GetSelection() == m_caster->GetGUID())
						{
							if(p_caster->GetSummon())
								u_caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, p_caster->GetSummon()->GetGUID());
							else if(m_targets.m_unitTarget)
								u_caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, m_targets.m_unitTarget);
							else
								u_caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, p_caster->GetSelection());
						}
						else
						{
							if(p_caster->GetSelection())
								u_caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, p_caster->GetSelection());
							else if(p_caster->GetSummon())
								u_caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, p_caster->GetSummon()->GetGUID());
							else if(m_targets.m_unitTarget)
								u_caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, m_targets.m_unitTarget);
							else
							{
//								m_isCasting = false;
								cancel();
								sLog.outDebug("SpellCast: missing channel target");
								return;
							}
						}
					}
					else 
//						if( total_targets_count <= 1 )
						u_caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, m_targets.m_unitTarget);
					if(u_caster && u_caster->GetPowerType()==POWER_TYPE_MANA )
					{
						if(channelDuration <= 5000)
							u_caster->DelayPowerRegeneration(5000);
						else if( channelDuration != -1 ) //wtf infinit cast time ?
							u_caster->DelayPowerRegeneration(channelDuration);
					}
				}
			}

			std::vector<uint64>::iterator i, i2;
			//do not ping pong reflected spells. Each spell can be reflected only once : IsReflected
			if( IsReflected() == SPELL_REFLECT_NONE && ( GetProto()->c_is_flags2 & ( SPELL_FLAG2_IS_REFLECTABLE | SPELL_FLAG2_IS_NON_REFLECTABLE ) ) == SPELL_FLAG2_IS_REFLECTABLE && m_caster->IsInWorld())
			{
				//check if any of our targets would reflect it back to us
				for(i= UniqueTargets.begin();i != UniqueTargets.end();i++)
				{
					Unit *Target = m_caster->GetMapMgr()->GetUnit(*i);
					if(Target)
					   SetReflected( Reflect( Target ) );

					// if the spell is reflected then we do not need to further handle it
					if( IsReflected() == SPELL_REFLECT_ORIGINAL_CANCEL )
						break;
				}
				if( IsReflected() == SPELL_REFLECT_NONE )
				{
					SpellTargetsList::iterator i;
					for ( i = ModeratedTargets.begin(); i != ModeratedTargets.end(); i++ )
					{
						Unit *Target = m_caster->GetMapMgr()->GetUnit( i->TargetGuid );
						if(Target)
						   SetReflected( Reflect( Target ) );

						// if the spell is reflected then we do not need to further handle it
						if( IsReflected() == SPELL_REFLECT_ORIGINAL_CANCEL )
							break;
					}
				}
			}

			// we're much better to remove this here, because otherwise spells that change powers etc,
			// don't get applied.
			if(u_caster && !pSpellId && !m_triggeredByAura)
				u_caster->RemoveAurasByInterruptFlagButSkip(AURA_INTERRUPT_ON_CAST_SPELL, GetProto()->Id, GetProto()->NameHash);

            // if the spell is not reflected
			if( IsReflected() != SPELL_REFLECT_ORIGINAL_CANCEL )
			{
				for(uint32 x=0;x<MAX_SPELL_EFFECT_COUNT;x++)
					if( redirected_effect[x] == SPELL_EFFECT_REDIRECT_FLAG_EXECUTE )
				{
					//can happen
					if( deleted )
					{
						sLog.outDebug("SpellCast: Script cast interrupted");
						return;
					}

					uint32 cur_eff = GetProto()->eff[x].Effect;
                    // check if we actualy have a effect
					if( cur_eff )
					{
						if( cur_eff == SPELL_EFFECT_PERSISTENT_AREA_AURA )
							HandleEffects(m_caster->GetGUID(),x);
						else if ( m_targetUnits[x].empty() == false )
						{
							for(i= m_targetUnits[x].begin();i != m_targetUnits[x].end();)
                            {
								i2 = i++;
								HandleEffects((*i2),x);
                            }
						}
						else if( m_targets.m_itemTarget )
							HandleEffects( 0 , x );
						// Capt: The way this is done is NOT GOOD. Target code should be redone.
						else if( cur_eff == SPELL_EFFECT_TELEPORT_UNITS ||
							     cur_eff == SPELL_EFFECT_SUMMON ||
								 cur_eff == SPELL_EFFECT_SUMMON_OBJECT_SLOT1 ||
								 cur_eff == SPELL_EFFECT_SUMMON_OBJECT_SLOT2 ||
								 cur_eff == SPELL_EFFECT_SUMMON_OBJECT_SLOT3 ||
								 cur_eff == SPELL_EFFECT_SUMMON_OBJECT_SLOT4 
//								 || cur_eff == SPELL_EFFECT_TRIGGER_SPELL	//removed by zack. what about if we want to trigger spell on target and not us ?
								 //not good damaging spells suck with this
								 //GetProto()->EffectImplicitTargetA[x] == EFF_TARGET_LOCATION_TO_SUMMON ||
								 //GetProto()->EffectImplicitTargetB[x] == EFF_TARGET_LOCATION_TO_SUMMON
								 )
                        {
							HandleEffects(m_caster->GetGUID(),x);
                        }
					}
				}

				/* don't call HandleAddAura unless we actually have auras... - Burlex*/
//				if( GetProto()->eff[0].EffectApplyAuraName != 0 || GetProto()->eff[1].EffectApplyAuraName != 0 || GetProto()->eff[2].EffectApplyAuraName != 0)
				if( GetProto()->eff[0].EffectApplyAuraName != 0 || GetProto()->eff[1].EffectApplyAuraName != 0 || GetProto()->eff[2].EffectApplyAuraName != 0)
				{
					hadEffect = true; // spell has had an effect (for item removal & possibly other things)

					for(i= UniqueTargets.begin();i != UniqueTargets.end();i++)
						HandleAddAura((*i));
				}
				// spells that proc on spell cast, some talents
				if(
					u_caster &&	// well units should also proc on cast
//					p_caster && 
					u_caster->IsInWorld())
				{
					bool CalledCasterProcHandler = false; //it happens that durring the cast of the spell the target gets desummoned ( summon totem ) but we still need to call prochandler for caster
					for(i= UniqueTargets.begin();i != UniqueTargets.end();i++)
					{
						Unit * Target = u_caster->GetMapMgr()->GetUnit((*i));

						if(!Target)
							continue; //we already made this check, so why make it again ?

						//some spells simply do not have enough effect field so they cast another spell after suing up all 3 effects
						if( GetProto()->chained_cast )
						{
							Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
							spell->Init( u_caster, GetProto()->chained_cast, true, NULL );
							SpellCastTargets targets( (*i) );
							spell->prepare(&targets);
						}

						//can happen
						if( deleted )
						{
							sLog.outDebug("SpellCast: Script cast interrupted");
							return;
						}

						if(	( !pSpellId || (GetProto()->c_is_flags & SPELL_FLAG_IS_PROC_TRIGGER_PROC) ) && (GetProto()->c_is_flags3 & SPELL_FLAG3_IS_DISABLE_OTHER_SPELL_CPROC) == 0 )
						{
							//p_caster->HandleProc(PROC_ON_CAST_SPELL,Target, GetProto());
							//Target->HandleProc(PROC_ON_SPELL_LAND_VICTIM,u_caster,GetProto());
							//zack : !!!!!! damage represents calculate effect last value and not some actual dmg !!!
							if( u_caster == Target )
								u_caster->HandleProc( PROC_ON_CAST_SPELL | PROC_ON_SPELL_LAND_VICTIM,Target, GetProto(), &damage);
							else
							{
								u_caster->HandleProc(PROC_ON_CAST_SPELL,Target, GetProto(), &damage);
								Target->HandleProc(PROC_ON_SPELL_LAND_VICTIM,u_caster,GetProto(), &damage);
							}
							CalledCasterProcHandler = true;
						}

						//can happen
						if( deleted )
						{
							sLog.outDebug("SpellCast: Script cast interrupted");
							return;
						}

						//aura states like health below 20% should not be removed for sure
						//removing thing will defenetly create some exploit. Best is to make the aura that set the aurastate to remove the state :(
						uint32 tas = GetProto()->GetTargetAuraState();
						if( tas == AURASTATE_DODGE_BLOCK || tas == AURASTATE_PARRY )
							Target->RemoveAuraStateFlag( tas );
						if( Target->IsPlayer() )
						{
							//these are scripted a lot. But better then nothing to have them anywayz
							SafePlayerCast( Target )->Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET,GetProto()->Id,ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
							SafePlayerCast( Target )->Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET2,GetProto()->Id,ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
						}

						//some spells break stealth of the target. Actually most auras do ?
						if( ( GetProto()->c_is_flags3 & SPELL_FLAG3_IS_BREAKING_STEALTH_TARGET ) )
							Target->RemoveStealth();
					}
					//!! this is required for Area or summon spells that might not have any targets but proc events
					if( CalledCasterProcHandler == false && ( !pSpellId || (GetProto()->c_is_flags & SPELL_FLAG_IS_PROC_TRIGGER_PROC) ) )
					{
						u_caster->HandleProc(PROC_ON_CAST_SPELL,u_caster, GetProto(), &damage);
						//can happen
						if( deleted )
						{
							sLog.outDebug("SpellCast: Script cast interrupted");
							return;
						}
					}
					if( ModeratedTargets.empty() == false )
					{
						SpellTargetsList::iterator i;
						for ( i = ModeratedTargets.begin(); i != ModeratedTargets.end(); i++ )
						{
							if( (*i).TargetModType == SPELL_DID_HIT_IMMUNE || (*i).TargetModType == SPELL_DID_HIT_RESIST )
							{
								Unit * Target = u_caster->GetMapMgr()->GetUnit( (*i).TargetGuid );
								if( Target )
								{
									if( Target != u_caster )
									{
										Target->HandleProc( PROC_ON_IMMUNE_VICTIM, u_caster, GetProto(), &damage );
										u_caster->HandleProc( PROC_ON_IMMUNE, Target, GetProto(), &damage );
									}
									else
										u_caster->HandleProc( PROC_ON_IMMUNE | PROC_ON_IMMUNE_VICTIM, Target, GetProto(), &damage );
									//can happen
									if( deleted )
									{
										sLog.outDebug("SpellCast: Script cast interrupted");
										return;
									}
								}
							}
						}
					}
					if( p_caster )
					{
						p_caster->Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_CAST_SPELL,GetProto()->Id,ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
						p_caster->Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_CAST_SPELL2,GetProto()->Id,ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
					}
				}
			} /**/

//			m_isCasting = false;

			if(m_spellState != SPELL_STATE_CASTING)
			{
				finish();
				return;
			}
		}
		else //this shit has nothing to do with instant, this only means it will be on NEXT melee hit
		{
			// we're much better to remove this here, because otherwise spells that change powers etc,
			// don't get applied.

			if( u_caster && !pSpellId && !m_triggeredByAura )
				u_caster->RemoveAurasByInterruptFlagButSkip(AURA_INTERRUPT_ON_CAST_SPELL, GetProto()->Id, GetProto()->NameHash);

			//not sure if it must be there...
			/*if( p_caster != NULL )
			{
				if(p_caster->m_onAutoShot)
				{
					p_caster->GetSession()->OutPacket(SMSG_CANCEL_AUTO_REPEAT);
					p_caster->GetSession()->OutPacket(SMSG_CANCEL_COMBAT);
					p_caster->m_onAutoShot = false;
				}
			}*/

//			m_isCasting = false;
			SendCastResult(cancastresult);
			if( u_caster != NULL )
				u_caster->SetOnMeleeSpell(GetProto()->Id, SpellCastQueueIndex);

			finish();
			return;
		}
	}
	else
	{
		// cancast failed
		SendCastResult(cancastresult);
		finish();
		return;
	}

	//maybe some spells only trigger cooldown after expiration ?
	if( ( GetProto()->c_is_flags & SPELL_FLAG_IS_REQUIRECOOLDOWNUPDATE ) && m_caster->IsPlayer() )
		SafePlayerCast( m_caster )->StartClientCooldown( GetProto()->Id );

	if( !pSpellId && p_caster != NULL && p_caster->CooldownCheat )
		p_caster->ClearCooldownForSpell( GetProto()->Id );

}

void Spell::AddTime(uint32 type)
{
	if( channel_delays >= 3 )
		return; //spells get delayed top 3 times in 303
	channel_delays++;
	if(u_caster && u_caster->IsPlayer())
	{
		if( GetProto()->InterruptFlags & CAST_INTERRUPT_ON_DAMAGE_TAKEN)
		{
			cancel();
			return;
		}
		if( Need_SM_Apply(GetProto()) && u_caster && u_caster->SM_Mods )
		{
			float ch=0;
			SM_FFValue(u_caster->SM_Mods->SM_PNonInterrupt,&ch,GetProto()->GetSpellGroupType());
#ifdef COLLECTION_OF_UNTESTED_STUFF_AND_TESTERS
			float spell_pct_modifers=0;
			SM_FFValue(u_caster->SM_PNonInterrupt,&spell_pct_modifers,GetProto()->GetSpellGroupType());
			if(spell_pct_modifers!=0)
				printf("!!!!!spell interrupt chance mod pct %f , uninterrupt chance %f, spell group %u\n",spell_pct_modifers,ch,GetProto()->GetSpellGroupType());
#endif
			if(RandChance(ch))
			{ 
				return;
			}
		}
		if(m_spellState==SPELL_STATE_PREPARING)
		{
			// no pushback for some spells
			if ((GetProto()->InterruptFlags & CAST_INTERRUPT_PUSHBACK) == 0)
			{ 
				return;
			}

			int32 delay = m_castTime/(3+channel_delays);
			if( p_caster != NULL )
				delay -= delay * p_caster->SpellDelayResist[type] / 100;
			m_timer+=delay;
			if(m_timer>m_castTime)
			{
				delay -= (m_timer - m_castTime);
				m_timer=m_castTime;
				if(delay<0)
					delay = 1;
			}

			sStackWorldPacket(data,SMSG_SPELL_DELAYED,15);
			data << u_caster->GetNewGUID();
			data << uint32(delay);
			u_caster->SendMessageToSet(&data, true);

			if(!p_caster)
			{
				if(m_caster->GetTypeId() == TYPEID_UNIT)
					u_caster->GetAIInterface()->AddStopTime(delay);
			}
			//in case cast is delayed, make sure we do not exit combat
			else
			{
//				sEventMgr.ModifyEventTimeLeft(p_caster,EVENT_ATTACK_TIMEOUT,PLAYER_ATTACK_TIMEOUT_INTERVAL,true);
				// also add a new delay to offhand and main hand attacks to avoid cutting the cast short
				p_caster->delayAttackTimer(delay);
			}
		}
		else if( GetProto()->ChannelInterruptFlags != 48140)
		{
			int32 delay = GetDuration()/(2+channel_delays);
			m_timer-=delay;
			if(m_timer<0)
				m_timer=0;
			else
				p_caster->delayAttackTimer(-delay);

			m_Delayed = true;
			if(m_timer>0)
				SendChannelUpdate(m_timer);
			else
			{
//				SendChannelUpdate( 0 );
				if( p_caster->GetCurrentSpell() )
					p_caster->GetCurrentSpell()->cancel();
			}
		}
	}
}

void Spell::update(uint32 difftime)
{
	if(m_cancelled)
	{
		cancel();
		return;
	}

	// skip cast if we're more than 2/3 of the way through
	// TODO: determine which spells can be casted while moving.
	// Client knows this, so it should be easy once we find the flag.
	// XD, it's already there!
	if( ( GetProto()->InterruptFlags & CAST_INTERRUPT_ON_MOVEMENT ) &&
//		(((float)m_castTime / 1.5f) > (float)m_timer ) &&		//this is getting exploited by players with long channel time, you can move the second half
//		float(m_castTime)/float(m_timer) >= 2.0f		&&
		(
		m_castPositionX != m_caster->GetPositionX() ||
		m_castPositionY != m_caster->GetPositionY() ||
		m_castPositionZ != m_caster->GetPositionZ()
		)
		)
	{
		if( u_caster != NULL 
			&& ( ( GetProto()->c_is_flags2 & SPELL_FLAG2_IS_IGNORING_MOVE_INTERUPT ) == 0 )
			&& ( p_caster == NULL || p_caster->IsSpellIgnoringMoveInterrupt( GetProto()->NameHash ) == false ) )
		{
			if(u_caster->HasNoInterrupt() == 0 
//				&& GetProto()->EffectMechanic[1] != MECHANIC_INCAPACIPATED	//ugh what is this for ?
				)
			{
				//clear initial cooldown ? Some say you should be able to recast it if interrupted
				//do not clear cooldown for at least channeled spells. Evocation heals 45% of health as casted and you can spam it if we clear cooldown
				if( p_caster && GetProto()->ChannelInterruptFlags == 0 && p_caster->GetSession() )
					p_caster->ClearCooldownForSpell( GetProto()->Id );
				cancel();
				return;
			}
		}
	}

	switch(m_spellState)
	{
	case SPELL_STATE_PREPARING:
		{
			//printf("spell::update m_timer %u, difftime %d, newtime %d\n", m_timer, difftime, m_timer-difftime);
			if((int32)difftime >= m_timer)
				cast(true);
			else
			{
				m_timer -= difftime;
				if((int32)difftime >= m_timer)
				{
					m_timer = 0;
					cast(true);
				}
			}


		} break;
	case SPELL_STATE_CASTING:
		{
			if(m_timer > 0)
			{
				if((int32)difftime >= m_timer)
					m_timer = 0;
				else
					m_timer -= difftime;
			}
			if(m_timer <= 0)
			{
				SendChannelUpdate(0);
				finish();
				return;
			}
		} break;
	}
}

void Spell::finish()
{
	/*
	//this will leak memory if we early exit ..somehow this check should be made somewhere else
	if (m_spellState == SPELL_STATE_FINISHED)
	{ 
		return;
	} */
	//even proc spells in process of casting can be queried. Pls note that this might get lost in case chained cast happens
	if( p_caster )
		p_caster->m_currentSpellAny = NULL;

	m_spellState = SPELL_STATE_FINISHED;
	if( u_caster != NULL )
	{
		u_caster->m_canMove = true;
		// mana           channeled                                                     power type is mana
//		if(m_usesMana && (GetProto()->ChannelInterruptFlags == 0 && !pSpellId) && u_caster->GetPowerType()==POWER_TYPE_MANA )
//		if( power_cost > 0 && u_caster->GetPowerType()==POWER_TYPE_MANA )
		{
			/*
			Five Second Rule
			After a character expends mana in casting a spell, the effective amount of mana gained per tick from spirit-based regeneration becomes a ratio of the normal
			listed above, for a period of 5 seconds. During this period mana regeneration is said to be interrupted. This is commonly referred to as the five second rule.
			By default, your interrupted mana regeneration ratio is 0%, meaning that spirit-based mana regeneration is suspended for 5 seconds after casting.
			Several effects can increase this ratio, including:
			*/

//			u_caster->DelayPowerRegeneration(5000);
		}
	}
	/* Mana Regenerates while in combat but not for 5 seconds after each spell */
	/* Only if the spell uses mana, will it cause a regen delay.
	   is this correct? is there any spell that doesn't use mana that does cause a delay?
	   this is for creatures as effects like chill (when they have frost armor on) prevents regening of mana	*/

	//moved to spellhandler.cpp -> remove item when click on it! not when it finishes

	//enable pvp when attacking another player with spells
	if( p_caster != NULL )
	{
		if (GetProto()->Attributes & ATTRIBUTES_STOP_ATTACK 
//			&& ( p_caster->IsAttacking() )	//funny, but for example scatter shot will enable combat unless we tell client to not enable it
			)
		{
			p_caster->m_onAutoShot = false;
			p_caster->EventAttackStop();
			p_caster->smsg_AttackStop( p_caster->GetSelection() );
			p_caster->GetSession()->OutPacket( SMSG_CANCEL_COMBAT );
		}

		if( m_requiresCP >0 && !GetSpellFailed() )
		{
			if( p_caster->m_tempComboPoints )
				p_caster->m_tempComboPoints = 0;
			if( p_caster->m_spellcomboPoints )
			{
				p_caster->m_comboPoints = p_caster->m_spellcomboPoints;
				p_caster->m_spellcomboPoints = 0;
				p_caster->UpdateComboPoints(); //this will make sure we do not use any wrong values here
			}
			else
			{
				p_caster->NullComboPoints();
			}
		}
		if(m_Delayed)
		{
			Unit *pTarget = NULL;
			if( p_caster->IsInWorld() )
			{
				pTarget = p_caster->GetMapMgr()->GetUnit(m_caster->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT));
				if(!pTarget)
					pTarget = p_caster->GetMapMgr()->GetUnit(p_caster->GetSelection());
			}

			if(pTarget)
				pTarget->RemoveAura(GetProto()->Id, m_caster->GetGUID(),AURA_SEARCH_ALL);
			p_caster->RemoveAura(GetProto()->Id, m_caster->GetGUID(),AURA_SEARCH_ALL);
		}

		if( !pSpellId && p_caster != NULL && p_caster->CooldownCheat )
			p_caster->ClearCooldownForSpell( GetProto()->Id );

	}

	if( HasEffectType( GetProto(), SPELL_EFFECT_SUMMON_OBJECT ) )
		if( p_caster != NULL )
			p_caster->SetSummonedObject(NULL);
	/*
	Set cooldown on item
	*/
	if( i_caster && i_caster->GetOwner() && cancastresult == SPELL_CANCAST_OK && !GetSpellFailed() )
	{
			uint32 x;
		for(x = 0; x < 5; x++)
		{
			if(i_caster->GetProto()->Spells.Trigger[x] == USE)
			{
				if(i_caster->GetProto()->Spells.Id[x])
					break;
			}
		}
		//these are tinker spells from engeneering ;) 
		if( x >= 5 )
		{
			if( p_caster )
				p_caster->Cooldown_Add( GetProto(), i_caster->GetProto()->ItemId );
		}
		else
			i_caster->GetOwner()->Cooldown_AddItem( i_caster->GetProto() , x , GetProto()->Attributes & ATTRIBUTES_START_COOLDOWN_AFTER_COMBAT );
	}
	/*
	We set current spell only if this spell has cast time or is channeling spell
	otherwise it's instant spell and we delete it right after completion
	*/

	if( u_caster != NULL )
	{
		//if spell has cast time and it gets interrupted then we need to set it to null.
		//actually we should set current spell to null
//		if(!pSpellId && (GetProto()->ChannelInterruptFlags || m_castTime>0))
		if( u_caster->GetCurrentSpell() == this )
			u_caster->SetCurrentSpell( NULL );
	}

	// Send Spell cast info to QuestMgr
	if( cancastresult == SPELL_CANCAST_OK && p_caster != NULL && p_caster->IsInWorld() )
	{
		// Taming quest spells are handled in SpellAuras.cpp, in SpellAuraDummy
		// OnPlayerCast shouldn't be called here for taming-quest spells, in case the tame fails (which is handled in SpellAuras)
		bool isTamingQuestSpell = false;
/*		uint32 tamingQuestSpellIds[] = { 19688, 19694, 19693, 19674, 19697, 19696, 19687, 19548, 19689, 19692, 19699, 19700, 30099, 30105, 30102, 30646, 30653, 30654, 0 };
		uint32* spellidPtr = tamingQuestSpellIds;
		while( *spellidPtr ) // array ends with 0, so this works
		{
			if( *spellidPtr == m_spellInfo->Id ) // it is a spell for taming beast quest
			{
				isTamingQuestSpell = true;
				break;
			}
			++spellidPtr;
		}*/
		// Don't call QuestMgr::OnPlayerCast for next-attack spells, either.  It will be called during the actual spell cast.
//		if( !(hasAttribute(ATTRIBUTE_ON_NEXT_ATTACK) && !pSpellId) && !isTamingQuestSpell )
		if( pSpellId == 0 )
		{
			uint32 numTargets = 0;
			TargetsList::iterator itr = UniqueTargets.begin();
			for(; itr != UniqueTargets.end(); ++itr)
			{
				if( GET_TYPE_FROM_GUID(*itr) == HIGHGUID_TYPE_UNIT )
				{
					++numTargets;
					sQuestMgr.OnPlayerCast(p_caster,GetProto()->Id,*itr);
				}
			}
			if( numTargets == 0 )
			{
				uint64 guid = p_caster->GetTargetGUID();
					sQuestMgr.OnPlayerCast( p_caster, GetProto()->Id, guid );
			}
		}
	}
	if( deleted == false )
		SpellPool.PooledDelete( this, __FILE__, __LINE__ );
}

void Spell::SendCastResult(uint8 result,int32 p_extra )
{
	uint32 Extra = 0;
	if(result == SPELL_CANCAST_OK) 
	{ 
		return;
	}

	SetSpellFailed();

	if(!m_caster->IsInWorld()) 
	{ 
		return;
	}

	Player * plr = p_caster;

	if(!plr && u_caster)
		plr = u_caster->m_redirectSpellPackets;
	if(!plr) 
	{ 
		return;
	}

	// for some reason, the result extra is not working for anything, including SPELL_FAILED_REQUIRES_SPELL_FOCUS
	if( p_extra == 0 )
	{
		switch( result )
		{
		case SPELL_FAILED_REQUIRES_SPELL_FOCUS:
			Extra = GetProto()->GetRequiresSpellFocus();
			break;

		case SPELL_FAILED_REQUIRES_AREA:
			Extra = GetProto()->GetRequiresAreaId();
			if( GetProto()->GetRequiresAreaId() > 0 )
			{
				AreaGroup *ag = dbcAreaGroup.LookupEntry( GetProto()->GetRequiresAreaId() );
				if( ag )
				{
	//				uint16 plrarea = plr->GetMapMgr()->GetAreaID( plr->GetPositionX(), plr->GetPositionY() );
					uint16 plrarea = sTerrainMgr.GetAreaID( plr->GetMapId(), plr->GetPositionX(), plr->GetPositionY(), plr->GetPositionZ() );
					for( uint8 i = 0; i < 7; i++ )
						if( ag->AreaId[i] != 0 && ag->AreaId[i] != plrarea )
						{	
							Extra = ag->AreaId[i];
							break;
						}
				}
			}
			break;

		case SPELL_FAILED_TOTEMS:
			{
				uint32 *Totem = GetProto()->GetTotem();
				if( Totem )
					Extra = Totem[1] ? Totem[1] : Totem[0];
			}break;
		case SPELL_FAILED_ONLY_SHAPESHIFT:
			Extra = GetProto()->RequiredShapeShift;
			break;

		//case SPELL_FAILED_TOTEM_CATEGORY: seems to be fully client sided.
		}
	}
	else
		Extra = p_extra;

	//plr->SendCastResult(GetProto()->Id, result, SpellCastQueueIndex, Extra);
	if( Extra )
	{
		packetSMSG_CASTRESULT_EXTRA pe;
		pe.SpellId = GetProto()->spell_id_client;
		pe.ErrorMessage = result;
		pe.MultiCast = SpellCastQueueIndex;
		pe.Extra = Extra;
		plr->GetSession()->OutPacket( SMSG_CAST_FAILED, sizeof( packetSMSG_CASTRESULT_EXTRA ), &pe );
	}
	else if( result == SPELL_FAILED_NOT_READY )
	{
		sStackWorldPacket(data,SMSG_CAST_FAILED, 1+4+1+4+1+20);
		data << SpellCastQueueIndex;
		data << GetProto()->spell_id_client;
		data << uint8(SPELL_FAILED_NOT_READY);
		data << uint8(0);	//so which 0 is first ? no idea
		data << uint32(0);	//maybe this is cooldown remaining
		plr->GetSession()->SendPacket( &data );
	}
	else
	{
		packetSMSG_CASTRESULT pe;
		pe.SpellId = GetProto()->spell_id_client;
		pe.ErrorMessage = result;
		pe.MultiCast = SpellCastQueueIndex;
		plr->GetSession()->OutPacket( SMSG_CAST_FAILED, sizeof( packetSMSG_CASTRESULT ), &pe ); 
	}
}

// uint16 0xFFFF
enum SpellStartFlags
{
	//0x01
	SPELL_START_FLAG_DEFAULT = 0x02, // atm set as default flag
	//0x04
	//0x08
	//0x10
	SPELL_START_FLAG_RANGED = 0x20,
	//0x40
	//0x80
	//0x100
	//0x200
	//0x400
	//0x800
	//0x1000
	//0x2000
	//0x4000
	//0x8000
};

void Spell::SendSpellStart()
{
	// no need to send this on passive spells
	if( !m_caster->IsInWorld() 
		|| ( GetProto()->Attributes & ATTRIBUTES_PASSIVE )
		|| pSpellId
		//no point of sending visual packet if there is no visual about it
		//speed has spells like "shoot" which do have a visual effect after a while
		|| ( m_spellInfo->SpellVisual[0] == 0 && m_spellInfo->speed == 0 )
		// X Msec cast is lost due to latency ? Don't worry, spell_go packet will handle the visual part
		// also note that most cast times are 1.5 second but due to hasting they might get to 1 sec cast time
//		|| m_castTime <= 1100	
		|| ( m_spellInfo->c_is_flags3 & SPELL_FLAG3_IS_SKIPPING_AUTOFACE_ON_CAST )
		)
		return;

/*
8F 9D 4A 74 03 04 guid
8F 9D 4A 74 03 04 guid
01 cn
0D 08 00 00 spell
02 00 04 40 cast flags 
DC 05 00 00 cast time 1500  
02 00 00 00 - target mask and extra target mask
8F 9D 4A 74 03 04 target guid
CC 00 00 00 - mana cost update ? on spell go this number got smaller. Maybe the spell mana cost ?
00
13329
{SERVER} Packet: (0x7C75) SMSG_SPELL_START PacketSize = 44 TimeStamp = 13469594
CF 23 A4 0F 01 80 01 
CF 23 A4 0F 01 80 01 
01 
07 06 00 00 
02 00 04 10 
00 00 00 00 
40 00 00 00
00 
10 9C CA 44 
7D 3B 89 C5 
16 72 A4 41 


enum SpellCastFlags
{
    CAST_FLAG_NONE               = 0x00000000,
    CAST_FLAG_PENDING            = 0x00000001,              // aoe combat log?
    CAST_FLAG_HAS_TRAJECTORY     = 0x00000002,
    CAST_FLAG_UNKNOWN_3          = 0x00000004,
    CAST_FLAG_UNKNOWN_4          = 0x00000008,              // ignore AOE visual
    CAST_FLAG_UNKNOWN_5          = 0x00000010,
    CAST_FLAG_PROJECTILE         = 0x00000020,
    CAST_FLAG_UNKNOWN_7          = 0x00000040,
    CAST_FLAG_UNKNOWN_8          = 0x00000080,
    CAST_FLAG_UNKNOWN_9          = 0x00000100,
    CAST_FLAG_UNKNOWN_10         = 0x00000200,
    CAST_FLAG_UNKNOWN_11         = 0x00000400,
    CAST_FLAG_POWER_LEFT_SELF    = 0x00000800,
    CAST_FLAG_UNKNOWN_13         = 0x00001000,
    CAST_FLAG_UNKNOWN_14         = 0x00002000,
    CAST_FLAG_UNKNOWN_15         = 0x00004000,
    CAST_FLAG_UNKNOWN_16         = 0x00008000,
    CAST_FLAG_UNKNOWN_17         = 0x00010000,
    CAST_FLAG_ADJUST_MISSILE     = 0x00020000,
    CAST_FLAG_UNKNOWN_19         = 0x00040000,
    CAST_FLAG_VISUAL_CHAIN       = 0x00080000,
    CAST_FLAG_UNKNOWN_21         = 0x00100000,
    CAST_FLAG_RUNE_LIST          = 0x00200000,
    CAST_FLAG_UNKNOWN_23         = 0x00400000,
    CAST_FLAG_UNKNOWN_24         = 0x00800000,
    CAST_FLAG_UNKNOWN_25         = 0x01000000,
    CAST_FLAG_UNKNOWN_26         = 0x02000000,
    CAST_FLAG_IMMUNITY           = 0x04000000,
    CAST_FLAG_UNKNOWN_28         = 0x08000000,
    CAST_FLAG_UNKNOWN_29         = 0x10000000,
    CAST_FLAG_UNKNOWN_30         = 0x20000000,
    CAST_FLAG_HEAL_PREDICTION    = 0x40000000,
    CAST_FLAG_UNKNOWN_32         = 0x80000000
};

*/
	sStackWorldPacket(data,SMSG_SPELL_START,500 );

	uint32 cast_flags = 2;

	if( power_cost != 0 )
		cast_flags |= 0x00040000;	//no idea why and when this is sent. I think this delays power regen client side
//	if( GetType() == SPELL_DMG_TYPE_RANGED )
//		cast_flags |= 0x20;

    // hacky yeaaaa
	if( GetProto()->Id == 8326 ) // death
		cast_flags = 0x0F;
	if (m_missileTravelTime)
		cast_flags |= 0x1006000E;
//	if (cast_flags & 0x20000)	//(missleflags << 16) ?
//		data << m_missilePitch << m_missileTravelTime;

	if( i_caster != NULL )
		data << i_caster->GetNewGUID() << u_caster->GetNewGUID();
	else
		data << m_caster->GetNewGUID() << m_caster->GetNewGUID();

	data << SpellCastQueueIndex; 
	data << GetProto()->spell_id_client;
	data << cast_flags;
	data << (uint32)m_castTime;

//	if( cast_flags & 0x04 )
		m_targets.write( data );
//	else
		data << (uint32) 0;	//client simply does not care most of the time ? There must be a flag for this

	if( GetType() == SPELL_DMG_TYPE_RANGED )
	{
		ItemPrototype* ip = NULL;
		if( GetProto()->Id == SPELL_RANGED_THROW ) // throw
		{
			if( p_caster != NULL )
			{
				Item *itm = p_caster->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_RANGED );
				if( itm != NULL )
				{
					ip = itm->GetProto();
					/* Throwing Weapon Patch by Supalosa
					p_caster->GetItemInterface()->RemoveItemAmt(it->GetEntry(),1);
					(Supalosa: Instead of removing one from the stack, remove one from durability)
					We don't need to check if the durability is 0, because you can't cast the Throw spell if the thrown weapon is broken, because it returns "Requires Throwing Weapon" or something.
					*/

					// burlex - added a check here anyway (wpe suckers :P)
					if( itm->GetDurability() > 0 )
					{
						itm->SetDurability( itm->GetDurability() - 1 );
						if( itm->GetDurability() == 0 )
							p_caster->ApplyItemMods( itm, EQUIPMENT_SLOT_RANGED, false, true );
					}
				}
				else
				{
					ip = ItemPrototypeStorage.LookupEntry( 2512 );	/*rough arrow*/
				}
			}
		}
#ifndef CATACLYSM_SUPPORT
		else if( GetProto()->AttributesExC & FLAGS4_PLAYER_RANGED_SPELLS )
		{
			if( p_caster != NULL )
				ip = ItemPrototypeStorage.LookupEntry( p_caster->GetUInt32Value( PLAYER_AMMO_ID ) );
			else
				ip = ItemPrototypeStorage.LookupEntry( 2512 );	/*rough arrow*/
		}
#endif
		if( ip != NULL )
			data << ip->DisplayInfoID << ip->InventoryType;
		else 
			data << uint32( 0 ) << uint32 ( 0 ); 
	}

	m_caster->SendMessageToSet( &data, true );
//if( p_caster ) p_caster->BroadcastMessage( "Started casting spell %s at %d", GetProto()->Name, getMSTime() );
}

/************************************************************************/
/* General Spell Go Flags, for documentation reasons                    */
/************************************************************************/
enum SpellGoFlags
{
	//seems to make server send 1 less byte at the end. Byte seems to be 0 and not sent on triggered spells
	//this is used only when server also sends power update to client
	//maybe it is regen related ?
	SPELL_GO_FLAGS_LOCK_PLAYER_CAST_ANIM	= 0x01,  //also do not send standstate update
	//0x02
	//0x04
	//0x08 //seems like all of these mean some spell anim state
	//0x10
	SPELL_GO_FLAGS_RANGED           = 0x20, //2 functions are called on 2 values
	//0x40
	//0x80
	SPELL_GO_FLAGS_ITEM_CASTER      = 0x100,	//are you sure ?
	SPELL_GO_FLAGS_UNK200			= 0x200, 
	SPELL_GO_FLAGS_EXTRA_MESSAGE    = 0x400, //TARGET MISSES AND OTHER MESSAGES LIKE "Resist"
	SPELL_GO_FLAGS_POWER_UPDATE		= 0x800, //seems to work hand in hand with some visual effect of update actually
	//0x1000
	SPELL_GO_FLAGS_UNK2000			= 0x2000, 
	SPELL_GO_FLAGS_UNK1000			= 0x1000, 
	//0x4000
	SPELL_GO_FLAGS_UNK8000			= 0x8000, //seems to make server send extra 2 bytes before SPELL_GO_FLAGS_UNK1 and after SPELL_GO_FLAGS_UNK20000
	SPELL_GO_FLAGS_UNK20000			= 0x20000, //seems to make server send an uint32 after m_targets.write
	SPELL_GO_FLAGS_UNK40000			= 0x40000, //sent with mana update, maybe trigger delayed regen ?
	SPELL_GO_FLAGS_UNK80000			= 0x80000, //2 functions called (same ones as for ranged but different)
	SPELL_GO_FLAGS_RUNE_UPDATE		= 0x200000, //2 bytes for the rune cur and rune next flags
	SPELL_GO_FLAGS_UNK400000		= 0x400000, //seems to make server send an uint32 after m_targets.write
};

void Spell::SendSpellFail(Object *Attacker, Object *Target, uint32 SpellID, uint8 SpellFailType )
{
	sStackWorldPacket( data,SMSG_SPELL_GO,5500 );
	data << Attacker->GetNewGUID() << Attacker->GetNewGUID();
	data << uint8(0);		//cast counter
	if( SpellID == 0 )
		data << uint32(6603);		//spell ID
	else
		data << uint32(SpellID);	//spell ID
	data << uint32( 0x400 );
	data << getMSTime();
	data << (uint8)(0);		//hitted target count
	data << (uint8)(1);
	data << Target->GetGUID();
	data << (uint8)(SpellFailType);
	data << uint32( 0 ); //targets
	Attacker->SendMessageToSet( &data );
}

void Spell::SendSpellGo()
{
	// Fill UniqueTargets
	TargetsList::iterator i, j;
	for( uint32 x = 0; x < MAX_SPELL_EFFECT_COUNT; x++ )
	{
//		if( GetProto()->eff[x].Effect )
		{
			bool add = true;
			for( i = m_targetUnits[x].begin(); i != m_targetUnits[x].end(); i++ )
			{
				add = true;
				for( j = UniqueTargets.begin(); j != UniqueTargets.end(); j++ )
				{
					if( (*j) == (*i) )
					{
						add = false;
						break;
					}
				}
				if( add && (*i) != 0 )
					UniqueTargets.push_back( (*i) );
				//TargetsList::iterator itr = std::unique(m_targetUnits[x].begin(), m_targetUnits[x].end());
				//UniqueTargets.insert(UniqueTargets.begin(),));
				//UniqueTargets.insert(UniqueTargets.begin(), itr);
			}
			
			if( m_targets.m_itemTarget )
			{
				add = true;
				for( j = UniqueTargets.begin(); j != UniqueTargets.end(); j++ )
				{
					if( m_targets.m_itemTarget == (*j) )
					{
						add = false;
						break;
					}
				}
				if( add )
					UniqueTargets.push_back( m_targets.m_itemTarget );
			}
		}
	}

	// no need to send this on passive spells
	if( ( m_rune_avail_before & 0x80 ) == 0 )
	{
		if( !m_caster->IsInWorld() 
			|| ( GetProto()->Attributes & ATTRIBUTES_PASSIVE )
			|| ( pSpellId && m_spellInfo->SpellVisual[0] == 0 && m_spellInfo->speed == 0 )
			)
			return;
	}

/*
13329
{SERVER} Packet: (0xE654) SMSG_SPELL_GO PacketSize = 47 TimeStamp = 13469594
CF 23 A4 0F 01 80 01 
CF 23 A4 0F 01 80 01 
01 
07 06 00 00
00 03 04 10 
1B 4D 36 90 
00 
00 
40 00 00 00
00 10 9C CA 44 7D 3B 89 C5 16 72 A4 41 01 
*/
	// Start Spell
	sStackWorldPacket( data,SMSG_SPELL_GO,5500 );
	uint32 flags = 0;

	if ( GetType() == SPELL_DMG_TYPE_RANGED )
		flags |= SPELL_GO_FLAGS_RANGED; // 0x20 RANGED

	if( i_caster != NULL )
		flags |= SPELL_GO_FLAGS_ITEM_CASTER; // 0x100 ITEM CASTER

	if( ModeratedTargets.empty() == false )
		flags |= SPELL_GO_FLAGS_EXTRA_MESSAGE; // 0x400 TARGET MISSES AND OTHER MESSAGES LIKE "Resist"

	//experiments with rune updates
	uint8 cur_have_runes;
	if( p_caster )
	{
		if( p_caster->getClass() == DEATHKNIGHT ) //send our rune updates ^^
		{
			//see what we will have after cast
			cur_have_runes = 0;
			for( int i=0;i<TOTAL_USED_RUNES;i++)
				if( p_caster->m_runes[ i ] >= MAX_RUNE_VALUE )
					cur_have_runes |= (1 << i);
			if( cur_have_runes != m_rune_avail_before )
//				flags |= SPELL_GO_FLAGS_RUNE_UPDATE | SPELL_GO_FLAGS_POWER_UPDATE;
				flags |= SPELL_GO_FLAGS_RUNE_UPDATE | SPELL_GO_FLAGS_UNK40000 | SPELL_GO_FLAGS_POWER_UPDATE | SPELL_GO_FLAGS_ITEM_CASTER ;
		}
//		else 
//		if( p_caster->getClass() == HUNTER )
		//this will update client side power bar and add 5 second delayed regeneration
		//mana has default regeneration. What else needs work ?
	}
	if( ( p_caster != NULL || ( u_caster != NULL && u_caster->IsPet() ) ) && power_cost > 0 
//			&& GetProto()->powerType != POWER_TYPE_HOLY && GetProto()->powerType != POWER_TYPE_SOUL_SHARDS 
			)
			flags |= SPELL_GO_FLAGS_POWER_UPDATE | SPELL_GO_FLAGS_ITEM_CASTER | SPELL_GO_FLAGS_UNK40000;

/*
8F 9D 4A 74 03 04 guid
8F 9D 4A 74 03 04 guid
01 cn
0D 08 00 00 spell
00 09 04 40 flags
80 12 AD 6F time
01 targets size
9D 4A 74 03 00 00 00 04 
00 no moderated targets
02 00 00 00 targets mask
8F 9D 4A 74 03 04 unittarget
AD 00 00 00 - power update ( is this cost ? )
*/
	// hacky..
	if( GetProto()->Id == 8326 ) // death
		flags = SPELL_GO_FLAGS_ITEM_CASTER | 0x0D;

	if( i_caster != NULL && u_caster != NULL ) // this is needed for correct cooldown on items
	{
		data << i_caster->GetNewGUID() << u_caster->GetNewGUID();
	}
	else
	{
		data << m_caster->GetNewGUID() << m_caster->GetNewGUID();
	}

	data << SpellCastQueueIndex; //3.0.2
	data << GetProto()->spell_id_client;
	data << flags;
	data << getMSTime();
	data << (uint8)(UniqueTargets.size()); //number of hits
	writeSpellGoTargets( &data );

	if( flags & SPELL_GO_FLAGS_EXTRA_MESSAGE )
	{
		uint32 size = (uint32)ModeratedTargets.size();
		if( size > 255 )
			size = 255;
		data << (uint8)(size); //number if misses
		writeSpellMissedTargets( &data );
	}
	else 
		data << uint8( 0 ); //moderated target size is 0 since we did not set the flag

	m_targets.write( data ); // this write is included the target flag

	// er why handle it being null inside if if you can't get into if if its null
	if( GetType() == SPELL_DMG_TYPE_RANGED )
	{
		ItemPrototype* ip = NULL;
		if( GetProto()->Id == SPELL_RANGED_THROW )
		{
			if( p_caster != NULL )
			{
				Item* it = p_caster->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_RANGED );
				if( it != NULL )
					ip = it->GetProto();
			}
			else
				ip = ItemPrototypeStorage.LookupEntry(2512);	/*rough arrow*/
		}
		if( ip != NULL)
			data << ip->DisplayInfoID << ip->InventoryType;
		else 
			data << uint32( 0 ) << uint32( 0 );
	}

	//data order depending on flags : 0x800, 0x200000, 0x20000, 0x20, 0x80000, 0x40 (this is not spellgoflag but seems to be from spellentry or packet..)
//.text:00401110                 mov     eax, [ecx+14h] -> them
//.text:00401115                 cmp     eax, [ecx+10h] -> us
//	if( flags & SPELL_GO_FLAGS_ITEM_CASTER )
//		data << m_caster->GetNewGUID(); //this is a compressed guid. No idea what is used for

	if( u_caster && ( flags & SPELL_GO_FLAGS_POWER_UPDATE ) )
		data << uint32( u_caster->GetPower( GetProto()->powerType ) ); //no idea about this :S.If not sent there is no visual update

	if( flags & SPELL_GO_FLAGS_RUNE_UPDATE )
	{
		//we already substracted power
		data << uint8( m_rune_avail_before );
		data << uint8( cur_have_runes );
		for(uint8 i=0;i<TOTAL_USED_RUNES;i++)
			data << uint8( p_caster->m_runes[ i ] ); //values of the rune converted into byte. We just think it is 0 but maybe it is not :P 
	}
	
	m_caster->SendMessageToSet( &data, true );

	// spell log execute is still send 2.08
	// as I see with this combination, need to test it more
	//if (flags != 0x120 && GetProto()->Attributes & 16) // not ranged and flag 5
	//SendLogExecute(0,m_targets.m_unitTarget);
}

void Spell::writeSpellGoTargets( StackWorldPacket * data )
{
	TargetsList::iterator i;
	for ( i = UniqueTargets.begin(); i != UniqueTargets.end(); i++ )
	{
//		SendCastSuccess(*i);
		*data << (*i);
	}
}

void Spell::writeSpellMissedTargets( StackWorldPacket * data )
{
	/*
	 * The flags at the end known to us so far are.
	 * 1 = Miss
	 * 2 = Resist
	 * 3 = Dodge // mellee only
	 * 4 = Deflect
	 * 5 = Block // mellee only
	 * 6 = Evade
	 * 7 = Immune
	 */
	SpellTargetsList::iterator i;
	uint32 size_limiter = 0;
	if(u_caster && u_caster->isAlive())
	{
		for ( i = ModeratedTargets.begin(); i != ModeratedTargets.end(); i++ )
		{
			*data << (*i).TargetGuid;       // uint64
			*data << (*i).TargetModType;    // uint8
			///handle proc on resist spell
			Unit* target = u_caster->GetMapMgr()->GetUnit((*i).TargetGuid);
			if(target && target->isAlive())
			{
				u_caster->HandleProc(PROC_ON_RESIST_VICTIM,target,GetProto()/*,damage*/);		/** Damage is uninitialized at this point - burlex */
				target->CombatStatusHandler_ResetPvPTimeout(); // aaa
				u_caster->CombatStatusHandler_ResetPvPTimeout(); // bbb
			}
			size_limiter++;
			if( size_limiter >= 255 )
				break;
		}
	}
	else
		for ( i = ModeratedTargets.begin(); i != ModeratedTargets.end(); i++ )
		{
			*data << (*i).TargetGuid;       // uint64
			*data << (*i).TargetModType;    // uint8

			size_limiter++;
			if( size_limiter >= 255 )
				break;
		}
}
/*
void Spell::SendLogExecute(uint32 damage, uint64 & targetGuid)
{
	sStackWorldPacket(data,SMSG_SPELLLOGEXECUTE,50);
	data << m_caster->GetNewGUID();
	data << GetProto()->Id;
	data << uint32(1);
	data << GetProto()->SpellVisual[0];
	data << uint32(1);
	if (m_caster->GetGUID() != targetGuid)
		data << targetGuid;
	if (damage)
		data << damage;
	m_caster->SendMessageToSet(&data,true);
}*/

void Spell::SendInterrupted( uint8 result )
{
	SetSpellFailed();

	if( m_caster == NULL || !m_caster->IsInWorld() )
	{ 
		return;
	}

	sStackWorldPacket( data,SMSG_SPELL_FAILURE, 50 );

	// send the failure to pet owner if we're a pet
	Player *plr = p_caster;
	if( plr == NULL && m_caster->IsPet() )
 	{
		SafePetCast(m_caster)->SendCastFailed( m_spellInfo->Id, result );
 	}
	else
	{
		if( plr == NULL && u_caster != NULL && u_caster->m_redirectSpellPackets != NULL )
			plr = u_caster->m_redirectSpellPackets;
	
		if( plr != NULL && plr->IsPlayer() )
		{
			data << m_caster->GetNewGUID();
			data << SpellCastQueueIndex;
			data << m_spellInfo->Id;
			data << uint8( result );
			plr->GetSession()->SendPacket( &data );
		}
	}
	//Zack : is there a reason we need to send others spell fail reason ? Isn't this personal ?
//	if( result != 0 )
	{
		data.Initialize( SMSG_SPELL_FAILED_OTHER );
		data << m_caster->GetNewGUID();
		data << SpellCastQueueIndex;
		data << GetProto()->spell_id_client;
		m_caster->SendMessageToSet( &data, true );
	}
	/**/
}

void Spell::SendChannelUpdate(uint32 time)
{
	if(time == 0)
	{
		if(u_caster && u_caster->IsInWorld())
		{
			DynamicObject* dynObj=u_caster->GetMapMgr()->GetDynamicObject(u_caster->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT));
			if(dynObj)
				dynObj->Remove();

			u_caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT,0);
			u_caster->SetUInt32Value(UNIT_CHANNEL_SPELL,0);
		}
	}

	if (!p_caster)
	{ 
		return;
	}

	sStackWorldPacket(data, MSG_CHANNEL_UPDATE,20);
	data << p_caster->GetNewGUID();
	data << time;

	p_caster->SendMessageToSet(&data, true);
}

void Spell::SendChannelStart(uint32 duration)
{
	if (m_caster->GetTypeId() != TYPEID_GAMEOBJECT)
	{
		// Send Channel Start
		sStackWorldPacket(data,MSG_CHANNEL_START,50);
		data << m_caster->GetNewGUID();
		data << GetProto()->spell_id_client;
		data << duration;
		data << uint8(0);	//no idea in 4.x
		m_caster->SendMessageToSet(&data, true);
	}

	m_castTime = m_timer = duration;

	if( u_caster != NULL )
		u_caster->SetUInt32Value(UNIT_CHANNEL_SPELL,GetProto()->spell_id_client);

	/*
	Unit* target = objmgr.GetCreature( SafePlayerCast( m_caster )->GetSelection());
	if(!target)
		target = objmgr.GetObject<Player>( SafePlayerCast( m_caster )->GetSelection());
	if(!target)
		return;

	m_caster->SetUInt32Value(UNIT_FIELD_CHANNEL_OBJECT,target->GetGUIDLow());
	m_caster->SetUInt32Value(UNIT_FIELD_CHANNEL_OBJECT+1,target->GetGUIDHigh());
	//disabled it can be not only creature but GO as well
	//and GO is not selectable, so this method will not work
	//these fields must be filled @ place of call
	*/
}

void Spell::SendResurrectRequest(Player* target)
{
/*
13329
{SERVER} Packet: (0xAE7D) SMSG_RESURRECT_REQUEST PacketSize = 19 TimeStamp = 17222515
44 BB 38 03 00 00 00 01 guid
01 00 00 00 namesize ?
00 name ?
00 ?
01 status ?
51 C6 00 00 spell
*/
	string name;
	if( p_caster )
		name = p_caster->GetName();	//this is not required for players since due to GUID and name query it will be displayed anyway
	else if( m_caster->IsCreature() && SafeCreatureCast( m_caster )->myFamily )
		name = SafeCreatureCast( m_caster )->myFamily->name;
	else name ="";
	WorldPacket data(SMSG_RESURRECT_REQUEST, 8 + 4 + name.size() + 1 + 4 );
	data << m_caster->GetGUID();
	data << uint32( name.size() + 1 ); //null terminated
	data << name;
	data << uint8(0); //?
	data << uint8(1); //?
	data << GetProto()->spell_id_client;
	target->GetSession()->SendPacket(&data);
	target->m_resurrecter = m_caster->GetGUID();
}

void Spell::SendTameFailure( uint8 result )
{
    if( p_caster != NULL )
    {
        WorldPacket data( SMSG_PET_TAME_FAILURE, 1 );
        data << uint8( result );
        p_caster->GetSession()->SendPacket( &data );
    }
}

bool Spell::HasPower()
{
	if( u_caster != NULL )
		if(u_caster->HasFlag(UNIT_NPC_FLAGS,UNIT_NPC_FLAG_TRAINER))
		{ 
			return true;
		}

	if(p_caster && p_caster->PowerCheat)
	{ 
		return true;
	}

	// Free cast for battle preparation
	if( p_caster && IS_INSTANCE(p_caster->GetMapId()) )
	{
		if ( p_caster->HasAura(44521))
		{ 
			return true;
		}
		if ( p_caster->HasAura(44535))
		{ 
			return true;
		}
		if ( p_caster->HasAura(32727))
		{ 
			return true;
		}
	}

	int32 basePower = 0;
	int32 currentPower = 0;
	int32 pct_mod = 100;
	switch(GetProto()->powerType)
	{
	case POWER_TYPE_HEALTH:
		{
			return true;
			basePower = m_caster->GetUInt32Value(UNIT_FIELD_BASE_HEALTH);
			currentPower = u_caster->GetPower( GetProto()->powerType );
		} break;
	case POWER_TYPE_MANA:
		{
			basePower = m_caster->GetUInt32Value(UNIT_FIELD_BASE_MANA);
			currentPower = u_caster->GetPower( GetProto()->powerType );
//			m_usesMana = true;
		} break;
	case POWER_TYPE_RAGE:
	case POWER_TYPE_FOCUS:
	case POWER_TYPE_ENERGY:
	case POWER_TYPE_HAPPINESS:
	case POWER_TYPE_RUNIC:
	case POWER_TYPE_SOUL_SHARDS:
	case POWER_TYPE_ECLIPSE:
	case POWER_TYPE_HOLY:
		{
			if( i_caster && GetProto()->powerType == POWER_TYPE_HAPPINESS )
				currentPower = 0x00FFFFFF;
			else if( u_caster )
				currentPower = u_caster->GetPower( GetProto()->powerType );
		} break;
	case POWER_TYPE_RUNES:
		{
			if( !p_caster )
			{ 
				return true;	//mobs have runes all the time :D
			}
			//lookup the requirements for this spell
			SpellRuneCostEntry *cost = dbcSpellRuneCostEntry.LookupEntryForced( GetProto()->RuneCostID );
			if( !cost )
			{
				sLog.outDebug("Could not find runecost for spell %u with cost id %u",GetProto()->Id,GetProto()->RuneCostID);
				return true;
			}

			int32 cost_red = cost->costs[ 0 ] + cost->costs[ 1 ] + cost->costs[ 2 ];
			int32 cost_red_ori = cost_red;
			if( Need_SM_Apply(GetProto()) && u_caster && u_caster->SM_Mods )
			{
				SM_FIValue(u_caster->SM_Mods->SM_FCost,&cost_red,GetProto()->GetSpellGroupType());
				SM_PIValue(u_caster->SM_Mods->SM_PCost,&cost_red,GetProto()->GetSpellGroupType());
			}

			//not sure here. Description says there are runes that refill in 10 seconds. Runecosts sugest that not everything is consumed ? 
			//need to apply cost mods too !
			uint8 fail_count=0;
			for(int rune_type=0;rune_type<TOTAL_NORMAL_RUNE_TYPES;rune_type++)
				if( cost->costs[ rune_type ] )
				{
//						m_runes_to_update++;
					if( cost_red < cost_red_ori )
					{
						cost_red += 1;
						continue;
					}
					uint8 HaveRunes = p_caster->GetFullRuneCount( rune_type );
					if( cost->costs[ rune_type ] > HaveRunes )
						fail_count += cost->costs[ rune_type ] - HaveRunes;
				}

			if( fail_count == 0 || p_caster->GetFullRuneCount( RUNE_DEATH ) >= fail_count )
			{ 
				return true;
			}
			return false; 
		}break;
	default:
		{
			sLog.outDebug("unknown power type %u",GetProto()->powerType);
			// we should'nt be here to return
			return false;
		} break;
	}

	if( u_caster != NULL && ( ( GetProto()->AttributesEx & ATTRIBUTESEX_DRAIN_WHOLE_MANA )
								|| ( GetProto()->c_is_flags2 & SPELL_FLAG2_IS_DRAINING_ALL_POWER ) ) ) // Uses %100 mana
	{
		power_cost = currentPower;	//use up all we have atm. Procs that mod this will be dodged
		return true;
	}
	else
	{
		SpellPowerEntry *spe = dbcSpellPower.LookupEntryForced( GetProto()->SpellPowerId );
		if( spe == NULL )
		{
			return true;	//well...crap
		}
		if( spe->ManaCostPercentage )//Percentage spells cost % of !!!BASE!!! mana
		{
			power_cost = basePower * spe->ManaCostPercentage / 100;
		}
		else
		{
			power_cost = spe->manaCost;
		}
	}

	if( u_caster != NULL )
	{
		uint32 mod_school;
		if( GetProto()->powerType == POWER_TYPE_MANA )
			mod_school = GetProto()->School;
		else
			mod_school = SCHOOL_NORMAL;
		power_cost += u_caster->PowerCostMod[ mod_school ];//this is not percent!
		power_cost += float2int32(power_cost*u_caster->GetFloatValue(UNIT_FIELD_POWER_COST_MULTIPLIER+mod_school));
	}

	//apply modifiers
	if( Need_SM_Apply(GetProto()) && u_caster && u_caster->SM_Mods )
	{
		SM_FIValue(u_caster->SM_Mods->SM_FCost,&power_cost,GetProto()->GetSpellGroupType());
		SM_PIValue(u_caster->SM_Mods->SM_PCost,&power_cost,GetProto()->GetSpellGroupType());
	}

	if( power_cost <= currentPower ) // Unit has enough power (needed for creatures)
	{
		return true;
	}
	else
	{
		//probably client server sync issue ?
		if( p_caster )
			p_caster->UpdatePowerAmm( true );
		return false;
	}
}

bool Spell::TakePower()
{
	if( u_caster != NULL && u_caster->HasFlag(UNIT_NPC_FLAGS,UNIT_NPC_FLAG_TRAINER) )
	{ 
		return true;
	}

	if(p_caster && p_caster->PowerCheat)
	{ 
		return true;
	}

	// Free cast for battle preparation
	if( p_caster && IS_INSTANCE(p_caster->GetMapId()) )
	{
		if ( p_caster->HasAura(44521))
		{ 
			return true;
		}
		if ( p_caster->HasAura(44535))
		{ 
			return true;
		}
		if ( p_caster->HasAura(32727))
		{ 
			return true;
		}
	}

	switch(GetProto()->powerType)
	{
		case POWER_TYPE_HEALTH:
			{
				int32 currentPower = m_caster->GetUInt32Value( UNIT_FIELD_HEALTH );
				m_caster->DealDamage(u_caster, power_cost, 0,true);
				return true;
			}break;
		case POWER_TYPE_MANA:
		case POWER_TYPE_RAGE:
		case POWER_TYPE_FOCUS:
		case POWER_TYPE_ENERGY:
		case POWER_TYPE_HAPPINESS://!!!some might not work because we do not have power for this. Ex : 26677 - Cure Poison
		case POWER_TYPE_RUNIC:
		case POWER_TYPE_SOUL_SHARDS:
		case POWER_TYPE_ECLIPSE:
		case POWER_TYPE_HOLY:
			{
				if( i_caster && GetProto()->powerType == POWER_TYPE_HAPPINESS )
					return true;
				else if( u_caster )
				{
					int32 currentPower = u_caster->GetPower( GetProto()->powerType );
					if( power_cost <= currentPower) // Unit has enough power (needed for creatures)
					{
						if( power_cost > 0 )
						{
							u_caster->SetPower( GetProto()->powerType, currentPower - power_cost);
							if( p_caster )
								p_caster->LastSpellCost = power_cost;
//							if( u_caster && GetProto()->powerType == POWER_TYPE_MANA )
//								u_caster->UpdatePowerAmm( true, GetProto()->powerType );	//will be handled by spell go
						}
						return true;
					}
					else
						return false;
				}
			}break;
		case POWER_TYPE_RUNES:
			{
				if( !p_caster )
				{ 
					return true;
				}
//				if( !HasPower() )
//					return false;

				//lookup the requirements for this spell
				SpellRuneCostEntry *cost = dbcSpellRuneCostEntry.LookupEntryForced( GetProto()->RuneCostID );
				if( !cost )
				{
					sLog.outDebug("Could not find runecost for spell %u with cost id %u",GetProto()->Id,GetProto()->RuneCostID);
					return true;
				}

				//!!!need to work on this. No idea how to select to which rune the reduction applies !
				//right now i imagine 1 flat reduction eliminates 1 rune. Can't imagine the PCT ones though :(
				int32 cost_red = cost->costs[ 0 ] + cost->costs[ 1 ] + cost->costs[ 2 ];
				int32 cost_red_ori = cost_red;
				if( Need_SM_Apply(GetProto()) && u_caster && u_caster->SM_Mods )
				{
					SM_FIValue(u_caster->SM_Mods->SM_FCost,&cost_red,GetProto()->GetSpellGroupType());
					SM_PIValue(u_caster->SM_Mods->SM_PCost,&cost_red,GetProto()->GetSpellGroupType());
				}

				//not sure here. Description says there are runes that refill in 10 seconds. Runecosts sugest that not everything is consumed ? 
				//need to apply cost mods too !
				uint8 fail_count=0;
				for(int rune_type=0;rune_type<TOTAL_NORMAL_RUNE_TYPES;rune_type++)
					if( cost->costs[ rune_type ] )
					{
//						m_runes_to_update++;
						if( cost_red < cost_red_ori )
						{
							cost_red += 1;
							continue;
						}
						fail_count += p_caster->TakeFullRuneCount( rune_type, cost->costs[rune_type] );
					}
				if( fail_count != 0 )
				{
					fail_count = p_caster->TakeFullRuneCount( RUNE_DEATH, fail_count );
					sLog.outDebug("Spell is trying to consume more runes then we have. Eighter proc spell is eating power or something went wrong");
				}
				//add runic power if there is one
				int32 runic_gain = cost->runic_power_conversion;
				if( runic_gain )
				{
					if( Need_SM_Apply(GetProto()) && p_caster && p_caster->SM_Mods )
					{
						SM_FIValue(p_caster->SM_Mods->SM_FCost,&runic_gain,GetProto()->GetSpellGroupType());
						SM_PIValue(p_caster->SM_Mods->SM_PCost,&runic_gain,GetProto()->GetSpellGroupType());
					}
					runic_gain = p_caster->m_mod_runic_power_gain_pct * runic_gain / 100;
					p_caster->ModPower( POWER_TYPE_RUNIC, runic_gain );
//					u_caster->UpdatePowerAmm();	//can be handled by spell go
				}

				//DK talent that procs if all blood runes are on cooldown
				//70656 - Item - Death Knight T10 Melee 4P Bonus
				//96269 - Death's Advance
				if( p_caster->GetFullRuneCount( RUNE_UNHOLY ) == 0 )
					p_caster->HandleProc( PROC_ON_RUNE_DEPLETE_EVENT | PROC_ON_VICTIM | PROC_ON_NOT_VICTIM, p_caster, GetProto() );
				return true;
			}break;
		default:{
			sLog.outDebug("unknown power type");
			// we should'nt be here to return
			return false;
				}break;
	}

#ifdef COLLECTION_OF_UNTESTED_STUFF_AND_TESTERS
	int spell_flat_modifers=0;
	int spell_pct_modifers=0;
	SM_FIValue(u_caster->SM_FCost,&spell_flat_modifers,GetProto()->GetSpellGroupType());
	SM_FIValue(u_caster->SM_PCost,&spell_pct_modifers,GetProto()->GetSpellGroupType());
	if(spell_flat_modifers!=0 || spell_pct_modifers!=0)
		printf("!!!!!spell cost mod flat %d , spell cost mod pct %d , spell dmg %d, spell group %u\n",spell_flat_modifers,spell_pct_modifers,cost,GetProto()->GetSpellGroupType());
#endif

	return false; // unhandled situation
}

void Spell::HandleEffects(uint64 guid, uint32 i)
{
	uint32 id;
	playerTarget = 0;	//make sure to reset this so effect 1 will not use the same value effect 0 set
	if(guid == m_caster->GetGUID() || guid == 0)
	{
		unitTarget = u_caster;
		if( m_caster->IsGameObject() )
			gameObjTarget = SafeGOCast( m_caster );
		else
			gameObjTarget = NULL;
		playerTarget = p_caster;
		if( p_caster && m_targets.m_itemTarget && p_caster->GetItemInterface() )
			itemTarget = p_caster->GetItemInterface()->GetItemByGUID( m_targets.m_itemTarget );
		else
			itemTarget = i_caster;
	}
	else
	{
		if( p_caster && m_targets.m_itemTarget && p_caster->GetItemInterface() )
			itemTarget = p_caster->GetItemInterface()->GetItemByGUID( m_targets.m_itemTarget );
		if(!m_caster->IsInWorld())
		{
			unitTarget = 0;
			playerTarget = 0;
			itemTarget = 0;
			gameObjTarget = 0;
			corpseTarget = 0;
		}
		else if( itemTarget == NULL && ( m_targets.m_targetMask & TARGET_FLAG_TRADE_ITEM ) && p_caster != NULL )
		{
			//in 403 spells use this flag to create enchantments also
			itemTarget = p_caster->GetItemInterface()->GetItemByGUID( guid );
			if( itemTarget == NULL )
			{
				Player * plr = p_caster->GetTradeTarget();
				if(plr)
					itemTarget = plr->getTradeItem((uint32)guid);
			}
		}
		else
		{
//			unitTarget = NULL;
			switch(GET_TYPE_FROM_GUID(guid))
			{
			case HIGHGUID_TYPE_UNIT:
				unitTarget = m_caster->GetMapMgr()->GetCreature( guid );
				break;
			case HIGHGUID_TYPE_PET:
				unitTarget = m_caster->GetMapMgr()->GetPet( guid );
				break;
			case HIGHGUID_TYPE_PLAYER:
				{
					unitTarget =  m_caster->GetMapMgr()->GetPlayer( guid );
					if( unitTarget )
						playerTarget = SafePlayerCast(unitTarget);
				}break;
			case HIGHGUID_TYPE_ITEM:
				if( p_caster != NULL )
					itemTarget = p_caster->GetItemInterface()->GetItemByGUID(guid);

				break;
			case HIGHGUID_TYPE_GAMEOBJECT:
				gameObjTarget = m_caster->GetMapMgr()->GetGameObject( guid );
				break;
			case HIGHGUID_TYPE_CORPSE:
				corpseTarget = objmgr.GetCorpse((uint32)guid);
				break;
			}
		}
	}

	//healing or casting a spell on a player that is in combat will put us in combat also according to wowwiki. Even if it is a benefic spell
	if( unitTarget != u_caster && unitTarget && u_caster && pSpellId == 0 && unitTarget->CombatStatus.IsInCombat() )
		u_caster->CombatStatus.OnDamageDealt( unitTarget );

	id = GetProto()->eff[i].Effect;

	if( unitTarget && unitTarget->isAlive() == false && ( GetProto()->AttributesExB & SPELL_ATTR2_ALLOW_DEAD_TARGET) == 0 && ( GetProto()->AttributesExC & CAN_PERSIST_AND_CASTED_WHILE_DEAD) == 0 )
	{
		if( id < TOTAL_SPELL_EFFECTS )
			sLog.outDebug( "WORLD: Spell effect ERROR target died. Id = %u (%s), damage = %d, target = %u", id, SpellEffectNames[id], damage, guid );
		else
			sLog.outDebug( "WORLD: Spell effect ERROR target died. Id = %u (%s), damage = %d, target = %u", id, "Unknown", damage, guid );
		return;
	}
	//only pillar of frost is using this ? Maybe later for effects like feeling no remorse ...
	if( playerTarget && playerTarget->m_SpellEffectImmunities[ id ] > 0 )
	{
		if( id < TOTAL_SPELL_EFFECTS )
			sLog.outDebug( "WORLD: Spell effect IMMUNE id = %u (%s), damage = %d, target = %u", id, SpellEffectNames[id], damage, guid );
		else
			sLog.outDebug( "WORLD: Spell effect IMMUNE id = %u (%s), damage = %d, target = %u", id, "Unknown", damage, guid );
		return;
	}

	//effect and aura immunity
/*	if( ( GetProto()->c_is_flags3 & SPELL_FLAG3_IS_DISPEL_MECHANIC ) == 0 && u_caster != NULL && unitTarget != NULL )
	{
		if( isAttackable( u_caster, unitTarget ) )
		{
			if( unitTarget->SchoolImmunityAntiEnemy[ GetProto()->School ] )
			{
				if( pSpellId == 0 && m_targetUnits[ i ].size() <= 1 )
					SendCastResult( SPELL_FAILED_IMMUNE );
				return;
			}
		}
		else if( unitTarget->SchoolImmunityAntiFriend[ GetProto()->School ] )
		{
			if( pSpellId == 0 && m_targetUnits[ i ].size() <= 1 )
				SendCastResult( SPELL_FAILED_IMMUNE );
			return;
		}
	}/**/ 

	damage = CalculateEffect(i,unitTarget);
	eff_values[i] = damage;

	//you can still redirect this cast with a proc
	if( u_caster )
	{
		uint64 TargetGuid;
		if( unitTarget )
			TargetGuid = unitTarget->GetGUID();
		else
			TargetGuid = 0;
		if( u_caster->GetGUID() != TargetGuid )
		{
			u_caster->HandleProc( PROC_ON_PRE_CAST_SPELL, unitTarget, GetProto(), &damage );
			if( m_caster->GetMapMgr() )
			{
				unitTarget = m_caster->GetMapMgr()->GetUnit( TargetGuid );
				if( unitTarget )
					unitTarget->HandleProc( PROC_ON_PRE_SPELL_LAND_VICTIM, u_caster, GetProto(), &damage );
			}
		}
		else
			u_caster->HandleProc( PROC_ON_PRE_CAST_SPELL | PROC_ON_PRE_SPELL_LAND_VICTIM, unitTarget, GetProto(), &damage );
	}

	//script override
	if( redirected_effect[ i ] != SPELL_EFFECT_REDIRECT_FLAG_EXECUTE )
	{
		redirected_effect[ i ] &= ~SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_THIS_TARGET; 
		return;
	}

	if( id<TOTAL_SPELL_EFFECTS)
	{
		uint32 guid = 0;
		if( unitTarget )
			guid = unitTarget->GetLowGUID();
		sLog.outDebug( "WORLD: Spell effect id = %u (%s), damage = %d, target = %u", id, SpellEffectNames[id], damage, guid );

		/*if(unitTarget && p_caster && isAttackable(p_caster,unitTarget))
			sEventMgr.ModifyEventTimeLeft(p_caster,EVENT_ATTACK_TIMEOUT,PLAYER_ATTACK_TIMEOUT_INTERVAL);*/

		(*this.*SpellEffectsHandler[id])(i);
	}
	else
		sLog.outError("SPELL: unknown effect %u spellid %u", id, GetProto()->Id);
#ifdef _CRTDBG_MAP_ALLOC_TEST_MEM
	_CrtCheckMemory();
#endif
}

void Spell::HandleAddAura(uint64 guid)
{
	Unit * Target = 0;
	if(guid == 0)
	{ 
		return;
	}

	if(u_caster && u_caster->GetGUID() == guid)
		Target = u_caster;
	else if(m_caster->IsInWorld())
		Target = m_caster->GetMapMgr()->GetUnit(guid);

	if(!Target)
	{ 
		return;
	}

	// Applying an aura to a flagged target will cause you to get flagged.
    // self casting doesnt flag himself.
	if(Target->IsPlayer() && p_caster && p_caster != SafePlayerCast(Target))
	{
		if(SafePlayerCast(Target)->IsPvPFlagged())
			p_caster->SetPvPFlag();
	}

	// remove any auras with same type
	if( GetProto()->BGR_one_buff_on_target > 0)
	{
		uint32 skip_self;
		if( GetProto()->maxstack > 1 )
			skip_self = GetProto()->Id;
		else
			skip_self = 0;
		Target->RemoveAurasByBuffType(GetProto()->BGR_one_buff_on_target, m_caster->GetGUID(),skip_self,GetProto()->BGR_one_buff_on_target_skip_caster_check);
	}

	// avoid map corruption
	if(Target->GetInstanceID()!=m_caster->GetInstanceID())
	{ 
		return;
	}

	Target->QueueAuraAdd.BeginLoop(); //do not delete auras while we are using them !
	Aura *QueuedToAddAura = Target->FindQueuedAura( GetProto()->Id );
//	std::map<uint32,Aura*>::iterator itr=Target->tmpAura.find(GetProto()->Id);
//	if(itr!=Target->tmpAura.end())
	{
		if( QueuedToAddAura && QueuedToAddAura->m_deleted )
		{
			Target->QueueAuraAdd.SafeRemove( QueuedToAddAura, 0 );
		}
		else if(QueuedToAddAura)
		{
			int32 AurasAdded = 0;
			//add charges before the real Aura, these have 0 mods and will be removed by "RemoveAura" before the one with effect
			int32 charges = GetProto()->procCharges;
			if( GetProto()->procCharges > 0 )
			{
				if( Need_SM_Apply( GetProto() ) && u_caster != NULL && u_caster->SM_Mods )
				{
					SM_FIValue( u_caster->SM_Mods->SM_FCharges, &charges, GetProto()->GetSpellGroupType() );
					SM_PIValue( u_caster->SM_Mods->SM_PCharges, &charges, GetProto()->GetSpellGroupType() );
#ifdef COLLECTION_OF_UNTESTED_STUFF_AND_TESTERS
					float spell_flat_modifers=0;
					float spell_pct_modifers=0;
					SM_FIValue(u_caster->SM_FCharges,&spell_flat_modifers,itr->second->GetSpellProto()->GetSpellGroupType());
					SM_FIValue(u_caster->SM_PCharges,&spell_pct_modifers,itr->second->GetSpellProto()->GetSpellGroupType());
					if(spell_flat_modifers!=0 || spell_pct_modifers!=0)
						printf("!!!!!spell charge bonus mod flat %f , spell range bonus pct %f , spell range %f, spell group %u\n",spell_flat_modifers,spell_pct_modifers,maxRange,GetProto()->GetSpellGroupType());
#endif
				}
				if( charges > MAX_AURAS / 4 )
					charges = MAX_AURAS / 4; //seen this happen with spell with 10k charges to "deadlock" realm
				for(int i=0;i<charges-1;i++)
				{
					Aura *aur = AuraPool.PooledNew( __FILE__, __LINE__ );
					aur->Init( GetProto(), QueuedToAddAura->GetDuration(), QueuedToAddAura->GetCaster(), QueuedToAddAura->GetTarget(), i_caster);
					if( Target->AddAura(aur) )
						AurasAdded++;
					aur=NULL;
				}
			}
			// the real spell is added last so the modifier is removed last
			// !this can make the unit change tmp aura list !
			if( Target->AddAura( QueuedToAddAura ) )
				AurasAdded++;
			if( charges > 0 && AurasAdded > 0 )
			{
				charges = MIN( charges, AurasAdded );
				if( charges > 0 && !(GetProto()->procFlags2 & PROC2_REMOVEONUSE) )
				{
					if( Target->m_chargeSpells.find( GetProto()->Id ) == Target->m_chargeSpells.end() )
					{
						SpellCharge charge;
						charge.count=charges;
//						charge.spellId=itr->second->GetSpellId();
						charge.spe = GetProto();
						if( GetProto()->procFlagsRemove != 0 )
							charge.ProcFlag = GetProto()->procFlagsRemove;
						else
							charge.ProcFlag = GetProto()->procFlags;
//						Target->m_chargeSpells.insert( make_pair( GetProto()->Id, charge ) );
						Target->RegisterNewChargeStruct( charge );
					}
					else
					{
						Target->m_chargeSpells[ GetProto()->Id ].count += charges;	//zack, is it even possible to stack up mor ethen "charge" count ?
//						Target->m_chargeSpells[ GetProto()->Id ].count = charges;
						//just in case charges cannot stack
						//there is a bug here. sometimes charges will sum up to maxstack even if player does not have maxstack aura count on him
						int32 MaxStack = MAX( GetProto()->maxstack, 1 ) * charges;
						if( MaxStack < Target->m_chargeSpells[ GetProto()->Id ].count )
							Target->m_chargeSpells[ GetProto()->Id ].count = MaxStack;
					}
				}
		}
//			Target->tmpAura.erase(itr);
			Target->QueueAuraAdd.SafeRemove( QueuedToAddAura, 0 );
		}
	}
	Target->QueueAuraAdd.EndLoopAndCommit();
#ifdef _CRTDBG_MAP_ALLOC_TEST_MEM
	_CrtCheckMemory();
#endif
}

/*
void Spell::TriggerSpell()
{
	if(TriggerSpellId != 0)
	{
		// check for spell id
		SpellEntry *spellInfo = sSpellStore.LookupEntry(TriggerSpellId );

		if(!spellInfo)
		{
			sLog.outError("WORLD: unknown spell id %i\n", TriggerSpellId);
			return;
		}

		Spell *spell = new Spell(m_caster, spellInfo,false, NULL);
		WPAssert(spell);

		SpellCastTargets targets;
		if(TriggerSpellTarget)
			targets.m_unitTarget = TriggerSpellTarget;
		else
			targets.m_unitTarget = m_targets.m_unitTarget;

		spell->prepare(&targets);
	}
}*/
/*
void Spell::DetermineSkillUp()
{
	skilllinespell* skill = objmgr.GetSpellSkill(GetProto()->Id);
	if (m_caster->GetTypeId() == TYPEID_PLAYER)
	{
		if ((skill) && SafePlayerCast( m_caster )->HasSkillLine( skill->skilline ) )
		{
			uint32 amt = SafePlayerCast( m_caster )->GetBaseSkillAmt(skill->skilline);
			uint32 max = SafePlayerCast( m_caster )->GetSkillMax(skill->skilline);
			if (amt >= skill->grey) //grey
			{
			}
			else if ((amt >= (((skill->grey - skill->green) / 2) + skill->green)) && (amt < max))
			{
				if (rand()%100 < 33) //green
					SafePlayerCast( m_caster )->AdvanceSkillLine(skill->skilline);

			}
			else if ((amt >= skill->green) && (amt < max))
			{
				if (rand()%100 < 66) //yellow
					SafePlayerCast( m_caster )->AdvanceSkillLine(skill->skilline);

			}
			else
			{
				if (amt < max) //brown
					SafePlayerCast( m_caster )->AdvanceSkillLine(skill->skilline);
			}
		}
	}
}
*/

uint8 Spell::CanCast(bool tolerate)
{
	uint32 i;
	if( IsSpellDisabled( GetProto() ) )
	{ 
		return SPELL_FAILED_SPELL_UNAVAILABLE;
	}
	Unit *target = NULL;

	if( GetProto()->CanCastHandler )
	{
		uint32 special_check_res =  GetProto()->CanCastHandler( this );
		if( special_check_res != SPELL_CANCAST_OK )
			return special_check_res;
		else if( special_check_res == SPELL_CANCAST_OK_NOMORE_CHECKS )
			return SPELL_CANCAST_OK;
	}

	if( i_caster && i_caster->GetOwner() && i_caster->GetOwner()->GetMapMgr() )
		target = i_caster->GetOwner()->GetMapMgr()->GetUnit( m_targets.m_unitTarget );

	if(m_caster->IsInWorld())
	{
		Unit *target = m_caster->GetMapMgr()->GetUnit( m_targets.m_unitTarget );

		if( target )
		{
			//grrr hackfix for camouflage, you cannot hit players with ranged spells, somthing like immun to ranged attacks
			//i need to make this with variables, but the darn spell uses like 3 unique auras and i have noidea which one is which
			if( target->ImmuneToRanged && target != m_caster && ( GetProto()->c_is_flags & SPELL_FLAG_IS_HEALING ) == 0 )
			{
				if ( GetType() == SPELL_DMG_TYPE_RANGED //obviously, i wonder how many will fail 
				 || GetProto()->speed != 0 )//dmg is not instantly applied
				{
					return SPELL_FAILED_NO_VALID_TARGETS;
				}
				//insta cast ranged spells like mind spike
				float maxRange = GetMaxRange( dbcSpellRange.LookupEntryForced( GetProto()->rangeIndex ) );
				if( maxRange > 6.0f ) //not a melee spell ( generic range is 5 )
					return SPELL_FAILED_NO_VALID_TARGETS;
			}

			if( GetProto()->required_target_type != SPELL_TARGET_TYPE_REQUIRE_NO_CHECK )
			{
				uint32 target_value;
				if( GetProto()->required_target_type == SPELL_TARGET_TYPE_REQUIRE_FACTION_CHECK )
					target_value = target->_getFaction();
				else if( GetProto()->required_target_type == SPELL_TARGET_TYPE_REQUIRE_ENTRY_CHECK )
					target_value = target->GetEntry();
				bool target_check_passed = false;
				for(uint32 i=0;i<GetProto()->required_target_value.GetMaxSize();i++)
					if( GetProto()->required_target_value.GetValue( i ) == target_value )
					{
						target_check_passed = true;
						break;
					}
				if( target_check_passed == false )
					return SPELL_FAILED_BAD_TARGETS;
			}
			//check for forbearanbce on target
			if( GetProto()->MechanicsType == MECHANIC_INVULNERABLE && GetProto()->GetCasterAuraSpellNot() == 0 )
			{
				for(uint32 x=MAX_POSITIVE_AURAS;x<target->m_auras_neg_size;x++)
					if(target->m_auras[x] && target->m_auras[x]->GetSpellProto()->MechanicsType == MECHANIC_INVULNERABLE_FORBID )
					{ 
						return SPELL_FAILED_BAD_TARGETS;
					}
			}

			// GM Flagged Players should be immune to other players' casts, but not their own.
			if (target->IsPlayer() && target != m_caster && SafePlayerCast(target)->bGMTagOn != 0 )
			{ 
				return SPELL_FAILED_BM_OR_INVISGOD;
			}
		}
	}

	if( u_caster != NULL )
	{
		if ( ( GetProto()->Attributes & ATTRIBUTES_REQ_OOC) && u_caster->CombatStatus.IsInCombat() )
		{
			if( (GetProto()->Id !=  100 && GetProto()->Id != 6178 && GetProto()->Id != 11578 ) 
				|| ( p_caster != NULL && p_caster->ignoreShapeShiftChecks == false && p_caster->IgnoreSpellSpecialCasterStateRequirement==0) )	// Warbringer (Warrior 51Prot Talent effect)
				return SPELL_FAILED_TARGET_IN_COMBAT;
		}

		// check for cooldowns
		Player *tp_caster;
		uint32 ItemOrPetno = 0;
		int32 RemainingCooldown;
		if( p_caster )
		{
			tp_caster = p_caster;
			RemainingCooldown = tp_caster->Cooldown_Getremaining( GetProto(), ItemOrPetno );
		}
		else if( u_caster->IsPet() )
		{
			tp_caster = SafePetCast( u_caster )->GetPetOwner();
			ItemOrPetno = SafePetCast( u_caster )->GetPetNumber();
			RemainingCooldown = SafePetCast( u_caster )->CooldownCanCast( GetProto() );
		}
		else
		{
			tp_caster = NULL;
			RemainingCooldown = 0;
		}
		if(!tolerate && tp_caster && tp_caster->CooldownCheat == false )
		{ 
//tp_caster->BroadcastMessage("spell %s has %d cooldown %d",GetProto()->Name, RemainingCooldown, tp_caster->Cooldown_Getremaining( GetProto(), 0 ) );
			if( RemainingCooldown > 0 && RemainingCooldown < 2*24*60*60*1000 )
			{
				if( RemainingCooldown > 5000 )
					tp_caster->UpdateClientCooldown( GetProto()->Id, RemainingCooldown );
				return SPELL_FAILED_NOT_READY;
			}
		}
		//charge, intercept
		if( u_caster->m_rooted && ( GetProto()->c_is_flags3 & SPELL_FLAG3_IS_NOT_CASTABLE_ROOTED ) && ( GetProto()->c_is_flags2 & SPELL_FLAG2_IS_CASTABLE_STUNNED ) == 0 )
			return SPELL_FAILED_ROOTED;
	}

	//Shady: EquippedItemClass, EquippedItemSubClass (doesn't it handled with client?)

	if( ( p_caster != NULL || ( u_caster != NULL && u_caster->IsPet() ) ) && !HasPower() )
	{ 
		return SPELL_FAILED_NO_POWER;
	}

	if( p_caster != NULL )
	{
		//special aura bound handlers. Ex : Detterence needs to negate most casts except a few
		if( p_caster->mSpellCanCastOverrideMap.HasValues() )
		{
			SimplePointerListNode<SpellCanCastScript> *itr;
			for( itr = p_caster->mSpellCanCastOverrideMap.begin(); itr != p_caster->mSpellCanCastOverrideMap.end(); itr = itr->Next() )
			{
				uint32 NewError = itr->data->CanCastFilterHandler( this, itr->data );
				if( NewError != SPELL_CANCAST_OK )
					return NewError;
			}
		}
		if( ( GetProto()->Attributes & ATTRIBUTES_REQ_STEALTH ) && !p_caster->IsStealth() && !p_caster->ignoreShapeShiftChecks ) 
		{ 
			return SPELL_FAILED_ONLY_STEALTHED;
		}

		if (sWorld.Collision) 
		{
			if (GetProto()->MechanicsType == MECHANIC_MOUNTED)
			{
				if (CollideInterface.IsIndoor( p_caster->GetMapId(), p_caster->GetPositionNC() ))
				{ 
					return SPELL_FAILED_NO_MOUNTS_ALLOWED;
				}
			}
			else if( GetProto()->Attributes & ATTRIBUTES_ONLY_OUTDOORS )
			{
				if( !CollideInterface.IsOutdoor( p_caster->GetMapId(), p_caster->GetPositionNC() ) )
				{ 
					return SPELL_FAILED_ONLY_OUTDOORS;
				}
			}
		}

		//are we in an arena and the spell cooldown is longer then 15mins?
		if ( p_caster->m_bg && ( p_caster->m_bg->GetType() >= BATTLEGROUND_ARENA_2V2 && p_caster->m_bg->GetType() <= BATTLEGROUND_ARENA_5V5 ) &&
			( GetProto()->RecoveryTime > 900000 || GetProto()->CategoryRecoveryTime > 900000 ) )
			return SPELL_FAILED_SPELL_UNAVAILABLE;

		//some spells cannot be casted while in PVP
		if( (GetProto()->AuraInterruptFlags & AURA_INTERRUPT_CUSTOM_ON_PVP) && p_caster->IsPvPFlagged() )
		{ 
			return SPELL_FAILED_NO_VALID_TARGETS;
		}

		//not dead sure how this should be
/*		if(p_caster->GetDuelState() == DUEL_STATE_REQUESTED)
		{
			return SPELL_FAILED_TARGET_DUELING;
		} */

		// check if spell is allowed while player is on a taxi
		if(p_caster->m_onTaxi && !(GetProto()->c_is_flags2 & SPELL_FLAG2_IS_TAXI_CASTABLE_ONLY) )//Are mount castable spells allowed on a taxi?
		{
			return SPELL_FAILED_NOT_ON_TAXI;
		}
		
		if( !p_caster->m_onTaxi && (GetProto()->c_is_flags2 & SPELL_FLAG2_IS_TAXI_CASTABLE_ONLY ) )
		{
			return SPELL_FAILED_NOT_HERE;
		}

		// check if spell is allowed while player is on a transporter
		if(p_caster->m_CurrentTransporter)
		{
			// no mounts while on transporters
			if( HasAuraType( GetProto(), SPELL_AURA_MOUNTED ) )
			{ 
				return SPELL_FAILED_NOT_ON_TRANSPORT;
			}
		}

		// check if spell is allowed while not mounted
		if (!(GetProto()->Attributes & ATTRIBUTES_MOUNT_CASTABLE) && p_caster->IsMounted())
		{ 
			return SPELL_FAILED_NOT_MOUNTED;
		}

		// check if spell is allowed while shapeshifted
		if(p_caster->GetShapeShift())
		{
			switch(p_caster->GetShapeShift())
			{
				case FORM_MOONKIN:
				{
					//only balance spells and remove curse
					if( 
//						GetProto()->spell_skilline[0] != SKILL_BALANCE 
						//allow potions and stuff
						( GetProto()->spell_skilline[0] == SKILL_RESTORATION2 ||  GetProto()->spell_skilline[0] == SKILL_FERAL_COMBAT )
						&& GetProto()->NameHash != SPELL_HASH_REMOVE_CORRUPTION 
						&& GetProto()->NameHash != SPELL_HASH_ABOLISH_POISON 
						&& GetProto()->NameHash != SPELL_HASH_MARK_OF_THE_WILD
						&& GetProto()->NameHash != SPELL_HASH_BARKSKIN
						)
						return SPELL_FAILED_NOT_ON_SHAPESHIFT;
				}break;
				case FORM_TREE:
				{
					//only balance spells and remove curse
/*					if( 
						//GetProto()->spell_skilline[0] != SKILL_RESTORATION2 
						//allow potions
						( GetProto()->spell_skilline[0] == SKILL_BALANCE ||  GetProto()->spell_skilline[0] == SKILL_FERAL_COMBAT )
						&& GetProto()->NameHash != SPELL_HASH_INNERVATE
						&& GetProto()->NameHash != SPELL_HASH_BARKSKIN
						&& GetProto()->NameHash != SPELL_HASH_NATURE_S_GRASP
						&& GetProto()->NameHash != SPELL_HASH_ENTANGLING_ROOTS
						&& GetProto()->NameHash != SPELL_HASH_THORNS
						&& GetProto()->NameHash != SPELL_HASH_WRATH
						&& GetProto()->NameHash != SPELL_HASH_CYCLONE
						)
						return SPELL_FAILED_NOT_ON_SHAPESHIFT;			*/		
				}break;
				case FORM_BATTLESTANCE:
				case FORM_DEFENSIVESTANCE:
				case FORM_BERSERKERSTANCE:
				case FORM_SHADOW:
				case FORM_STEALTH:
				case FORM_LICHBORN:
				{
					break;
				}

				case FORM_SWIFT:
				case FORM_FLIGHT:
				{
					// check if item is allowed (only special items allowed in flight forms)
					if(i_caster && !(i_caster->GetProto()->Flags & ITEM_FLAG_SHAPESHIFT_OK))
					{ 
						return SPELL_FAILED_NO_ITEMS_WHILE_SHAPESHIFTED;
					}

					break;
				}
				//druid in normal forms is allowed to cast item spells just like non shapeshifted forms
				case FORM_CAT:
				case FORM_BEAR:
				case FORM_TRAVEL:
				case FORM_AQUA:
					{
					}break;	
				//case FORM_AMBIENT:
				//case FORM_GHOUL:
				//case FORM_DIREBEAR:
				//case FORM_CREATUREBEAR:
				//case FORM_GHOSTWOLF:

				case FORM_SPIRITOFREDEMPTION:
				{
					//Spirit of Redemption (20711) fix
					if (!(GetProto()->c_is_flags & SPELL_FLAG_IS_HEALING) && GetProto()->Id != 7355)
					{ 
						return SPELL_FAILED_CASTER_DEAD;
					}
					break;
				}
				case FORM_DEMON: //allowed to cast item spells just like non shapeshifted forms. Sure ?
				{
				}break;

				default:
				{
					// check if item is allowed (only special & equipped items allowed in other forms)
					if(i_caster && !(i_caster->GetProto()->Flags & ITEM_FLAG_SHAPESHIFT_OK))
						if(i_caster->GetProto()->InventoryType == INVTYPE_NON_EQUIP)
						{ 
							return SPELL_FAILED_NO_ITEMS_WHILE_SHAPESHIFTED;
						}
				}
			}
		}
		//Zack : not good
		//check if spell requires shapeshift
		//if( GetProto()->RequiredShapeShift && !( ((uint32)1 << (p_caster->GetShapeShift()-1)) & GetProto()->RequiredShapeShift ) )
		//	return SPELL_FAILED_ONLY_SHAPESHIFT;

		// item spell checks
		if(i_caster && i_caster->GetProto()) //Let's just make sure there's something here, so we don't crash ;)
		{
			//check for Recently Bandaged on target
			if( target && GetProto()->MechanicsType == MECHANIC_HEALING )
			{
				for(uint32 x=MAX_POSITIVE_AURAS;x<target->m_auras_neg_size;x++)
					if(target->m_auras[x] && target->m_auras[x]->GetSpellProto()->MechanicsType == MECHANIC_HEALING )
					{ 
							return SPELL_FAILED_BAD_TARGETS;
					}
			}

			if( i_caster->GetProto()->ZoneNameID && p_caster != NULL && i_caster->GetProto()->ZoneNameID != p_caster->GetZoneId() && i_caster->GetProto()->ZoneNameID != p_caster->GetAreaID() && i_caster->GetProto()->ZoneNameID != p_caster->GetMapId() )
			{ 
				return SPELL_FAILED_NOT_HERE;
			}
			if( i_caster->GetProto()->MapID && i_caster->GetProto()->MapID != i_caster->GetMapId() )
			{ 
				return SPELL_FAILED_NOT_HERE;
			}

			if(i_caster->GetProto()->Spells.Charges[0] != 0)
			{
				// check if the item has the required charges
				if(i_caster->GetUInt32Value(ITEM_FIELD_SPELL_CHARGES) == 0)
				{ 
					return SPELL_FAILED_ERROR;
				}
			}
		}

		// check if we have the required reagents
		if( !( i_caster != NULL && (i_caster->GetProto()->Flags &  ITEM_FLAGS_ENCHANT_SCROLL)) //these are enchanting scrolls and reagent was already consumend when enchanter created them
//			&& !(p_caster->removeReagentCost && GetProto()->AttributesExD & 2) )//zack : wtf is flag 2 put there for ?
//			&& p_caster->removeReagentCost == false 
			&& !( GetProto()->SpellGroupType[0] & p_caster->GetUInt32Value(PLAYER_NO_REAGENT_COST_1 + 0) 
					|| GetProto()->SpellGroupType[1] & p_caster->GetUInt32Value(PLAYER_NO_REAGENT_COST_1+1) 
					|| GetProto()->SpellGroupType[2] & p_caster->GetUInt32Value(PLAYER_NO_REAGENT_COST_1+2) )
			)
		{
			SpellReagentsEntry *sre = dbcSpellReagents.LookupEntryForced( GetProto()->SpellReagentsId );
			if( sre )
			{
				for(i=0; i<8 ;i++)
				{
					if( sre->Reagent[i] == 0 || sre->ReagentCount[i] == 0)
						continue;

					if(p_caster->GetItemInterface()->GetItemCount(sre->Reagent[i]) < sre->ReagentCount[i])
					{ 
						return SPELL_FAILED_ITEM_GONE;
					}
				}
			}
		}

		// check if we have the required tools, totems, etc
		uint32 *Totem = GetProto()->GetTotem();
		if( Totem )
			for(i=0; i<2 ;i++)
			{
				if( Totem[i] != 0)
				{
					//in 3.3 there are some items that if equipped will make you not require any totems in bag anymore
					Item *relic = p_caster->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_RANGED );
					//we have them all ?
					if( relic && relic->GetProto() && relic->GetProto()->TotemCategory == TOTEM_CATEGORY_ALL_TOTEMS )
						break;
					if( !p_caster->GetItemInterface()->GetItemCount( Totem[i] ) )
					{ 
						return SPELL_FAILED_TOTEMS;
					}
				}
			}

		// check if we have the required gameobject focus
		//Zack : temp removed until i figure out this. This field is defenetly not spell focus
/*		float focusRange;
		uint32 requires_spellFocus = GetProto()->GetRequiresSpellFocus();
		if( GetProto()->GetRequiresSpellFocus() )
		{
			bool found = false;

			
			m_caster->AquireInrangeLock(); //make sure to release lock before exit function !
			for(InRangeSet::iterator itr = p_caster->GetInRangeSetBegin(); itr != p_caster->GetInRangeSetEnd(); itr++ )
			{
				if((*itr)->GetTypeId() != TYPEID_GAMEOBJECT)
					continue;

				if((*itr)->GetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_TYPEID) != GAMEOBJECT_TYPE_SPELL_FOCUS)
					continue;

				GameObjectInfo *info = SafeGOCast((*itr))->GetInfo();
				if(!info)
				{
					sLog.outDebug("Warning: could not find info about game object %u",(*itr)->GetEntry());
					continue;
				}

				// professions use rangeIndex 1, which is 0yds, so we will use 5yds, which is standard interaction range.
				if(info->sound1)
					focusRange = float(info->sound1);
				else
					focusRange = GetMaxRange(dbcSpellRange.LookupEntry(GetProto()->rangeIndex));

				// check if focus object is close enough
				if(!IsInrange(p_caster->GetPositionX(), p_caster->GetPositionY(), p_caster->GetPositionZ(), (*itr), (focusRange * focusRange)))
					continue;

				if(info->SpellFocus == GetProto()->GetRequiresSpellFocus())
				{
					found = true;
					break;
				}
			}

			m_caster->ReleaseInrangeLock();
			if(!found)
			{ 
				return SPELL_FAILED_REQUIRES_SPELL_FOCUS;
			}
		}*/

/*		if( GetProto()->GetRequiresAreaId() > 0 ) 
		{
			AreaGroup *ag = dbcAreaGroup.LookupEntry( GetProto()->GetRequiresAreaId() );
			uint8 i;
//			uint16 plrarea = p_caster->GetMapMgr()->GetAreaID( p_caster->GetPositionX(), p_caster->GetPositionY() );
			uint16 plrarea = sTerrainMgr.GetAreaID( p_caster->GetMapId(), p_caster->GetPositionX(), p_caster->GetPositionY(), p_caster->GetPositionZ() );
			for( i = 0; i < 7; i++ )
				if( ag->AreaId[i] == plrarea )
					break;
			if( 7 == i )
			{ 
				return SPELL_FAILED_REQUIRES_AREA;
			}
		} */

		// aurastate check
		uint32 asr;
		asr = GetProto()->GetCasterAuraState();
		if( asr && !p_caster->HasAuraStateFlag( asr ) && p_caster->IgnoreSpellSpecialCasterStateRequirement==0 )
		{
			return SPELL_FAILED_CASTER_AURASTATE;
		}
		asr = GetProto()->GetCasterAuraStateNot();
		if( asr && p_caster->HasAuraStateFlag( asr ) )
		{ 
			return SPELL_FAILED_CASTER_AURASTATE;
		}
		// aura check
		asr = GetProto()->GetCasterAuraSpell();
		if( asr && !p_caster->HasAura( asr ) )
		{ 
			return SPELL_FAILED_NOT_READY;
		}
		asr = GetProto()->GetCasterAuraSpellNot();
		if( asr && p_caster->HasAura( asr ) )
		{ 
			return SPELL_FAILED_NOT_READY;
		}

		if( GetProto()->ResearchProject )
		{
			ResearchProjectEntry *rs = dbcResearchProject.LookupEntryForced( GetProto()->ResearchProject );
			if( rs )
			{
				ResearchBranchEntry *rbe = dbcResearchBranch.LookupEntryForced( rs->BranchId );
				if( rbe && p_caster->GetCurrencyCount( rbe->ReqCurrency ) < rs->req_fragments )
				{
					//check if player has some keystoanes and we will eat those up
					uint32 bonus = 12 * MIN( 1, p_caster->GetItemInterface()->GetItemCount( rbe->ScrollItemId ) );
					if( p_caster->GetCurrencyCount( rbe->ReqCurrency ) + bonus < rs->req_fragments )
					{
						return SPELL_FAILED_REAGENTS;
					}
					else
					{
						p_caster->GetItemInterface()->RemoveItemAmt( rbe->ScrollItemId, 1 );
					}
				}
			}
		}

		// Targetted Item Checks
		if( m_targets.m_itemTarget )
		{
			Item *i_target = NULL;

			// check if the targeted item is in the trade box
			if( m_targets.m_targetMask & TARGET_FLAG_TRADE_ITEM )
			{
				//in 403 this is used for item enchants self also
				i_target = p_caster->GetItemInterface()->GetItemByGUID( m_targets.m_itemTarget );
				switch( GetProto()->eff[0].Effect )
				{
					// only lockpicking and enchanting can target items in the trade box
					case SPELL_EFFECT_OPEN_LOCK:
					case SPELL_EFFECT_ENCHANT_ITEM:
					case SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY:
					{
						// check for enchants that can only be done on your own items
						if( GetProto()->AttributesExB & FLAGS3_ENCHANT_OWN_ONLY )
						{ 
							return SPELL_FAILED_BAD_TARGETS;
						}

						// get the player we are trading with
						Player* t_player = p_caster->GetTradeTarget();
						// get the targeted trade item
						if( t_player != NULL )
							i_target = t_player->getTradeItem((uint32)m_targets.m_itemTarget);
					}
				}
			}
			// targeted item is not in a trade box, so get our own item
			else
			{
				i_target = p_caster->GetItemInterface()->GetItemByGUID( m_targets.m_itemTarget );
			}

			// check to make sure we have a targeted item
			if( i_target == NULL )
			{ 
				return SPELL_FAILED_BAD_TARGETS;
			}

			ItemPrototype* proto = i_target->GetProto();

			// check to make sure we have it's prototype info
			if(!proto || ( i_target->GetDurability() == 0 && proto->MaxDurability != 0 ) )
			{ 
				return SPELL_FAILED_BAD_TARGETS;
			}

			// check to make sure the targeted item is acceptable
			switch(GetProto()->eff[0].Effect)
			{
				// Lock Picking Targeted Item Check
				case SPELL_EFFECT_OPEN_LOCK:
				{
					// this is currently being handled in SpellEffects
					break;
				}

				// Enchanting Targeted Item Check
				case SPELL_EFFECT_ENCHANT_ITEM:
				case SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY:
				case SPELL_EFFECT_ADD_SOCKET:
				{
					// If enchant is permanent and we are casting on Vellums
					if(GetProto()->eff[0].Effect == SPELL_EFFECT_ENCHANT_ITEM && GetProto()->eff[0].EffectItemType != 0 &&
					   (proto->ItemId == 38682 || proto->ItemId == 37602 || proto->ItemId == 43145 || 
						proto->ItemId == 39349 || proto->ItemId == 39350 || proto->ItemId == 43146 ))
					{
						// Weapons enchants
	/*					if(GetProto()->GetEquippedItemClass() == ITEM_CLASS_WEAPON)
						{
							// These are armor vellums
							if( proto->ItemId == 38682 || proto->ItemId == 37602 || proto->ItemId == 43145 )
								return SPELL_FAILED_BAD_TARGETS;

							// You tried to cast wotlk enchant on bad item
							if(GetProto()->SpellLevel.baseLevel == 60 && proto->ItemId != 43146)
								return SPELL_FAILED_BAD_TARGETS;
							
							// you tried to cast tbc enchant on bad item
							if(GetProto()->SpellLevel.baseLevel == 35 && proto->ItemId == 39349)
								return SPELL_FAILED_BAD_TARGETS;

							// you tried to cast non-lvl enchant on bad item
							if(GetProto()->SpellLevel.baseLevel == 0 && proto->ItemId != 39349)
								return SPELL_FAILED_BAD_TARGETS;

							break;
						}

						// Armors enchants
						else if( GetProto()->GetEquippedItemClass() == ITEM_CLASS_ARMOR )
						{
							// These are weapon vellums
							if( proto->ItemId == 39349 || proto->ItemId == 39350 || proto->ItemId == 43146 )
								return SPELL_FAILED_BAD_TARGETS;

							// You tried to cast wotlk enchant on bad item
							if(GetProto()->SpellLevel.baseLevel == 60 && proto->ItemId != 43145)
								return SPELL_FAILED_BAD_TARGETS;

							// you tried to cast tbc enchant on bad item
							if(GetProto()->SpellLevel.baseLevel == 35 && proto->ItemId == 38682)
								return SPELL_FAILED_BAD_TARGETS;

							// you tried to cast non-lvl enchant on bad item
							if(GetProto()->SpellLevel.baseLevel == 0 && proto->ItemId != 38682)
								return SPELL_FAILED_BAD_TARGETS;
						}
						*/
						// If We are here it means that we have right Vellum and right enchant to cast
						break;
					}
					// check if we have the correct class, subclass, and inventory type of target item
					if( GetProto()->GetEquippedItemClass() != (int32)proto->Class)
					{ 
						return SPELL_FAILED_BAD_TARGETS;
					}

					if( GetProto()->GetEquippedItemSubClass() && !(GetProto()->GetEquippedItemSubClass() & (1 << proto->SubClass)))
					{ 
						return SPELL_FAILED_BAD_TARGETS;
					}

					if( GetProto()->GetRequiredItemFlags() && !(GetProto()->GetRequiredItemFlags() & (1 << proto->InventoryType)))
					{ 
						return SPELL_FAILED_BAD_TARGETS;
					}

					if (GetProto()->eff[0].Effect == SPELL_EFFECT_ENCHANT_ITEM &&
						GetProto()->SpellLevel.baseLevel && (GetProto()->SpellLevel.baseLevel > proto->ItemLevel)
						&& proto->SubClass != ITEM_SUBCLASS_VELUM )
						return int8(SPELL_FAILED_BAD_TARGETS); // maybe there is different err code

					if( i_caster && i_caster->GetProto()->Flags == 2097216)
						break;

	//				if( GetProto()->AttributesExB & FLAGS3_ENCHANT_OWN_ONLY && !(i_target->IsSoulbound()))
	//				{ 
	//					return SPELL_FAILED_BAD_TARGETS;
	//				}

					break;
				}

				// Disenchanting Targeted Item Check
	/*			case SPELL_EFFECT_DISENCHANT:
				{
					// check if item can be disenchanted
					if(proto->DisenchantReqSkill < 1)
					{ 
						return SPELL_FAILED_CANT_BE_DISENCHANTED;
					}

					// check if we have high enough skill
					if((int32)p_caster->_GetSkillLineCurrent(SKILL_ENCHANTING) < proto->DisenchantReqSkill)
					{ 
						return SPELL_FAILED_CANT_BE_DISENCHANTED_SKILL;
					}

					break;
				}*/

				// Feed Pet Targeted Item Check
				case SPELL_EFFECT_FEED_PET:
				{
					Pet *pPet = p_caster->GetSummon();

					// check if we have a pet
					if(!pPet)
					{ 
						return SPELL_FAILED_NO_PET;
					}
					
					// check if pet lives
					if( !pPet->isAlive() )
					{ 
						return SPELL_FAILED_TARGETS_DEAD;
					}

					// check if item is food
					if(!proto->FoodType)
					{ 
						return SPELL_FAILED_BAD_TARGETS;
					}

					// check if food type matches pets diet
					if(!(pPet->GetPetDiet() & (1 << (proto->FoodType - 1))))
					{ 
						return SPELL_FAILED_WRONG_PET_FOOD;
					}

					// check food level: food should be max 30 lvls below pets level
					if(pPet->getLevel() > proto->ItemLevel + 30)
					{ 
						return SPELL_FAILED_FOOD_LOWLEVEL;
					}

					break;
				}

				// Prospecting Targeted Item Check
				case SPELL_EFFECT_PROSPECTING:
				{
					// check if the item can be prospected
					if(!(proto->Flags & ITEM_FLAG_PROSPECTABLE))
					{ 
						return SPELL_FAILED_CANT_BE_PROSPECTED;
					}

					// check if we have at least 5 of the item
					if(p_caster->GetItemInterface()->GetItemCount(proto->ItemId) < 5)
					{ 
						return SPELL_FAILED_ITEM_GONE;
					}

					// check if we have high enough skill
					if(p_caster->_GetSkillLineCurrent(SKILL_JEWELCRAFTING) < proto->RequiredSkillRank)
					{ 
						return SPELL_FAILED_LOW_CASTLEVEL;
					}

					break;
				}
				// Milling Targeted Item Check
				case SPELL_EFFECT_MILLING:
				{
					// check if the item can be prospected
					if(!(proto->Flags & ITEM_FLAG_MILLABLE))
					{ 
						return SPELL_FAILED_CANT_BE_PROSPECTED;
					}

					// check if we have at least 5 of the item
					if(p_caster->GetItemInterface()->GetItemCount(proto->ItemId) < 5)
					{ 
						return SPELL_FAILED_ITEM_GONE;
					}

					// check if we have high enough skill
					if(p_caster->_GetSkillLineCurrent(SKILL_INSCRIPTION) < proto->RequiredSkillRank)
					{ 
						return SPELL_FAILED_LOW_CASTLEVEL;
					}

					break;
				}
			}
		}
	}

	// set up our max Range
	float maxRange = GetMaxRange( dbcSpellRange.LookupEntryForced( GetProto()->rangeIndex ) );
		// latency compensation!!
		// figure out how much extra distance we need to allow for based on our movespeed and latency.
		if( u_caster && m_caster->GetMapMgr() && m_targets.m_unitTarget )
		{
			Player * utarget;

			utarget = m_caster->GetMapMgr()->GetPlayer( m_targets.m_unitTarget );
			if( utarget && SafePlayerCast( utarget )->m_isMoving )
				{
					// this only applies to PvP.
					uint32 lat = SafePlayerCast( utarget )->GetSession() ? SafePlayerCast( utarget )->GetSession()->GetLatency() : 0;

					// if we're over 500 get fucked anyway.. your gonna lag! and this stops cheaters too
					lat = ( lat > 500 ) ? 500 : lat;

					// calculate the added distance
					maxRange += ( u_caster->m_runSpeed * 0.001f ) * float( lat );
				}
		}
	if( Need_SM_Apply(GetProto()) && u_caster != NULL && u_caster->SM_Mods )
	{
		SM_FFValue( u_caster->SM_Mods->SM_FRange, &maxRange, GetProto()->GetSpellGroupType() );
		SM_PFValue( u_caster->SM_Mods->SM_PRange, &maxRange, GetProto()->GetSpellGroupType() );
#ifdef COLLECTION_OF_UNTESTED_STUFF_AND_TESTERS
		float spell_flat_modifers=0;
		float spell_pct_modifers=0;
		SM_FFValue(u_caster->SM_FRange,&spell_flat_modifers,GetProto()->GetSpellGroupType());
		SM_FFValue(u_caster->SM_PRange,&spell_pct_modifers,GetProto()->GetSpellGroupType());
		if(spell_flat_modifers!=0 || spell_pct_modifers!=0)
			printf("!!!!!spell range bonus mod flat %f , spell range bonus pct %f , spell range %f, spell group %u\n",spell_flat_modifers,spell_pct_modifers,maxRange,GetProto()->GetSpellGroupType());
#endif
	}

	// Targeted Location Checks (AoE spells)
	if( m_targets.m_targetMask == TARGET_FLAG_DEST_LOCATION )
	{
		if( !IsInrange( m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ, m_caster, ( maxRange * maxRange ) ) )
		{ 
			return SPELL_FAILED_OUT_OF_RANGE;
		}
		if (sWorld.Collision) 
		{
			LocationVector CastDest( m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ );
			if( !CollideInterface.CheckLOS( u_caster->GetMapId(), u_caster->GetPositionNC(), CastDest ) )
			{ 
				return SPELL_FAILED_LINE_OF_SIGHT;
			}
		}
	}

	// Targeted Unit Checks
	if(m_targets.m_unitTarget)
	{
		target = (m_caster->IsInWorld()) ? m_caster->GetMapMgr()->GetUnit(m_targets.m_unitTarget) : NULL;

		if(target)
		{
			// Partha: +2.52yds to max range, this matches the range the client is calculating.
			// see extra/supalosa_range_research.txt for more info

			if( tolerate ) // add an extra 33% to range on final check (squared = 1.78x)
			{
//				float localrange=maxRange + 2.52f;
//				float localrange=maxRange + target->GetFloatValue(UNIT_FIELD_BOUNDINGRADIUS);
				float localrange;
				if( target->GetAIInterface() )
					localrange=maxRange + target->GetAIInterface()->_CalcCombatRange( u_caster, false );
				else
					localrange=maxRange + target->GetModelHalfSize();
//				if( !IsInrange( m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ(), target, ( localrange * localrange * 1.78f ) ) )
				if( !IsInrange( m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ(), target, ( localrange * localrange ) ) )
				{ 
					return SPELL_FAILED_OUT_OF_RANGE;
				}
			}
			else
			{
//				float localrange=maxRange + 2.52f;
//				float localrange=maxRange + target->GetFloatValue(UNIT_FIELD_BOUNDINGRADIUS);
				float localrange;
				if( target->GetAIInterface() )
					localrange=maxRange + target->GetAIInterface()->_CalcCombatRange( u_caster, false );
				else
					localrange=maxRange + target->GetModelHalfSize();
				if( !IsInrange( m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ(), target, ( localrange * localrange ) ) )
				{ 
					return SPELL_FAILED_OUT_OF_RANGE;
				}
			}

			/* Target OOC check */
			if( ( GetProto()->AttributesEx & ATTRIBUTESEX_REQ_OOC_TARGET ) && target->CombatStatus.IsInCombat() )
			{ 
				return SPELL_FAILED_TARGET_IN_COMBAT;
			}

			if( p_caster != NULL || ( u_caster != NULL && u_caster->IsPet() )
//				&& target->IsPlayer() 
				)
			{
/*
				//not required in cataclysm ?
				if( GetProto()->Id == SPELL_RANGED_THROW)
				{
					Item * itm = p_caster->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_RANGED);
					if(!itm || (itm->GetDurability() == 0 && itm->GetProto()->MaxDurability > 0 ) )
					{ 
						return SPELL_FAILED_NO_AMMO;
					}
				}*/

				if (sWorld.Collision) 
				{
					if (u_caster->GetMapId() == target->GetMapId() && !CollideInterface.CheckLOS(u_caster->GetMapId(),u_caster->GetPositionNC(),target->GetPositionNC()))
					{ 
						return SPELL_FAILED_LINE_OF_SIGHT;
					}
				}

				// check aurastate
				uint32 tas = GetProto()->GetTargetAuraState();
				if( tas )
				{
					if( !target->HasAuraStateFlag( tas ) 
						&& p_caster->IgnoreSpellSpecialCasterStateRequirement==0 
						)
						return SPELL_FAILED_TARGET_AURASTATE;
				}
				uint32 ntas = GetProto()->GetTargetAuraStateNot();
				if( ntas && target->HasAuraStateFlag( ntas ) )
				{ 
					return SPELL_FAILED_TARGET_AURASTATE;
				}
				
				// check aura
				tas = GetProto()->GetTargetAuraSpell();
				if( tas && !target->HasAura( tas ) )
				{ 
					return SPELL_FAILED_NOT_READY;
				}
				ntas = GetProto()->GetTargetAuraSpellNot();
				if( ntas && target->HasAura( ntas ) )
				{ 
					return SPELL_FAILED_NOT_READY;
				}

				if( target->IsPlayer() && p_caster )
				{
					// disallow spell casting in sanctuary zones
					// allow attacks in duels
					if( p_caster->DuelingWith != target && !isFriendly( p_caster, target ) )
					{
						AreaTable* atCaster = dbcArea.LookupEntry( p_caster->GetAreaID() );
						AreaTable* atTarget = dbcArea.LookupEntry( SafePlayerCast( target )->GetAreaID() );
						if( atCaster->AreaFlags & AREA_FLAG_SANCTUARY || atTarget->AreaFlags & AREA_FLAG_SANCTUARY )
						{ 
							return SPELL_FAILED_NOT_HERE;
						}
					}
				}
//				else
//				{
//					if (target->GetAIInterface()->GetIsSoulLinked() && u_caster && target->GetAIInterface()->getSoullinkedWith() != u_caster)
//					{ 
//						return SPELL_FAILED_BAD_TARGETS;
//					}
//				}

/*				// pet training
				//not available in cataclysm anymore ?
				if( GetProto()->eff[0].EffectImplicitTargetA == EFF_TARGET_PET &&
					GetProto()->eff[0].Effect == SPELL_EFFECT_LEARN_SPELL )
				{
					Pet *pPet = p_caster->GetSummon();
					// check if we have a pet
					if( pPet == NULL )
					{ 
						return SPELL_FAILED_NO_PET;
					}

					// other checks
					SpellEntry* trig = dbcSpell.LookupEntry( GetProto()->eff[0].EffectTriggerSpell );
					if( trig == NULL )
					{ 
						return SPELL_FAILED_SPELL_UNAVAILABLE;
					}

					uint32 status = pPet->CanLearnSpell( trig );
					if( status != 0 )
					{ 
						return (uint8)(status);
					}
				} */

				//mind control or enslave demon
				for( int32 i=0;i<MAX_SPELL_EFFECT_COUNT;i++)
					if( GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MOD_POSSESS || GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MOD_CHARM	)
					{
						if( GetProto()->eff[i].EffectBasePoints )//got level req;
						{
							if((int32)target->getLevel() > GetProto()->eff[i].EffectBasePoints+1 + int32(p_caster->getLevel() - GetProto()->SpellLevel.spellLevel))
							{ 
								return SPELL_FAILED_HIGHLEVEL;
							}
							else if( target->IsCreature() )
							{
								Creature * c = SafeCreatureCast(target);
								if( c && c->GetCreatureInfo() )
								{ 
									if( c->GetCreatureInfo()->Rank > ELITE_ELITE )
										return SPELL_FAILED_CANT_BE_CHARMED;
									//do not let world bosses to be mind controled
									else if( c->GetProto()->modImmunities != 0 )
										return SPELL_FAILED_CANT_BE_CHARMED;
								}
							}
						}
					}
			}

			// if the target is not the unit caster and not the masters pet
			if(target != u_caster && !m_caster->IsPet())
			{
				/***********************************************************
				* Inface checks, these are checked in 2 ways
				* 1e way is check for damage type, as 3 is always ranged
				* 2e way is trough the data in the extraspell db
				*
				**********************************************************/

				// burlex: units are always facing the target!
				if( p_caster && ( GetProto()->FacingCasterFlags & ( CASTER_FACE_FACING | CASTER_FACE_FACING_WIDE | CASTER_FACE_FACING_WIDE2 | TARGET_FACE_CASTER ) ) )
				{
					if( ( GetProto()->FacingCasterFlags & TARGET_FACE_CASTER ) && target->isInFront(u_caster) == false ) //only gouge atm
					{
						return SPELL_FAILED_NOT_INFRONT;
					}
					else if( ( GetProto()->AttributesExB & FLAGS3_REQ_BEHIND_TARGET ) && target->isInFront(u_caster) == true )
					{ 
						return SPELL_FAILED_NOT_BEHIND;
					}
					else if( !u_caster->isInFront(target) ) //CASTER_FACE_TARGET
					{ 
						return SPELL_FAILED_UNIT_NOT_INFRONT;
					}
				}
			}

			// if target is already skinned, don't let it be skinned again
			if( GetProto()->eff[0].Effect == SPELL_EFFECT_SKINNING) // skinning
				if(target->IsUnit() && (SafeCreatureCast(target)->Skinned) )
				{ 
					return SPELL_FAILED_TARGET_UNSKINNABLE;
				}

			// all spells with target 61 need to be in group or raid
			// TODO: need to research this if its not handled by the client!!!
			/*if(
				GetProto()->eff[0].EffectImplicitTargetA == EFF_TARGET_AREAEFFECT_PARTY_AND_CLASS ||
				GetProto()->eff[1].EffectImplicitTargetA == EFF_TARGET_AREAEFFECT_PARTY_AND_CLASS ||
				GetProto()->eff[2].EffectImplicitTargetA == EFF_TARGET_AREAEFFECT_PARTY_AND_CLASS)
			{
				if( target->IsPlayer() && !SafePlayerCast( target )->InGroup() )
					return SPELL_FAILED_NOT_READY;//return SPELL_FAILED_TARGET_NOT_IN_PARTY or SPELL_FAILED_TARGET_NOT_IN_PARTY;
			}*/

			// pet's owner stuff
			/*if (GetProto()->eff[0].EffectImplicitTargetA == EFF_TARGET_PET_MASTER ||
				GetProto()->eff[1].EffectImplicitTargetA == EFF_TARGET_PET_MASTER ||
				GetProto()->eff[2].EffectImplicitTargetA == EFF_TARGET_PET_MASTER)
			{
				if (!target->IsPlayer())
					return SPELL_FAILED_TARGET_NOT_PLAYER; //if you are there something is very wrong
			}*/

			if( target != m_caster && target->dispels[GetProto()->GetDispelType()] )
			{ 
				return SPELL_FAILED_DAMAGE_IMMUNE;		
			}

			// if we're replacing a higher rank, deny it
			AuraCheckResponse acr = target->AuraCheck(GetProto()->NameHash, GetProto()->RankNumber,m_caster);
			if( acr.Error == AURA_CHECK_RESULT_HIGHER_BUFF_PRESENT )
			{ 
				return SPELL_FAILED_AURA_BOUNCED;
			}

			//check if we are trying to stealth or turn invisible but it is not allowed right now
			if( ( IsStealthSpell() || IsInvisibilitySpell() ) && u_caster->m_stealth_dissalowed != 0 )
			{
				return SPELL_FAILED_SPELL_UNAVAILABLE;
			}

			//special aura bound handlers. Ex : Detterence needs to negate most casts except a few
			if( target->IsPlayer() && SafePlayerCast( target )->mSpellCanTargetedOverrideMap.HasValues() )
			{
				SimplePointerListNode<SpellCanTargetedScript> *itr;
				for( itr = SafePlayerCast( target )->mSpellCanTargetedOverrideMap.begin(); itr != SafePlayerCast( target )->mSpellCanTargetedOverrideMap.end(); itr = itr->Next() )
				{
					uint32 NewError = itr->data->CanTargetedFilterHandler( this, m_caster, target, itr->data );
					if( NewError != SPELL_DID_HIT_SUCCESS )
						return SPELL_FAILED_BAD_TARGETS;
				}
			}
			/*SpellReplacement*rp=objmgr.GetReplacingSpells(GetProto()->Id);
			if(rp)
			{
				if(isAttackable(u_caster,target))//negative, replace only our own spell
				{
					for(uint32 x=0;x<rp->count;x++)
					{
						if(target->HasActiveAura(rp->spells[x],m_caster->GetGUID()))
						{
							return SPELL_FAILED_AURA_BOUNCED;
						}
					}
				}
				else
				{
					for(uint32 x=0;x<rp->count;x++)
					{
						if(target->HasActiveAura(rp->spells[x]))
						{
							return SPELL_FAILED_AURA_BOUNCED;
						}
					}
				}
			}	*/
		}
	}

	// Special State Checks (for creatures & players)
	if( u_caster )
	{
		if( ( GetProto()->c_is_flags2 & SPELL_FLAG2_IS_CASTABLE_SCHOOL_SILANCE ) == 0
			&& u_caster->SchoolCastPrevent[GetProto()->School] >= getMSTime()
			&& u_caster->m_silenced && GetProto()->School != NORMAL_DAMAGE // can only silence non-physical
			)
		{
			//in case spell is part of more then 1 school, then make sure it is silenced in ALL schools : ex frostfire orb + mindspike
			bool CanUseNonSilencedScool = false;
			for( uint32 i = SCHOOL_NORMAL+1;i<SCHOOL_COUNT;i++)
				if( ( GetProto()->SchoolMask & ( 1 << i ) ) != 0 && u_caster->SchoolCastPrevent[ i ] < getMSTime() )
				{
					CanUseNonSilencedScool = true;
					break;
				}
			if( CanUseNonSilencedScool == false )
				return SPELL_FAILED_SILENCED;
		}

		//zack : aa wtf ? breaking this, will fix after report comes in what this was for
/*		if(target) // -Supalosa- Shouldn't this be handled on Spell Apply? 
		{
			for( int i = 0; i < 3; i++ ) // if is going to cast a spell that breaks stun remove stun auras, looks a bit hacky but is the best way i can find
			{
				if( GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MECHANIC_IMMUNITY )
				{
					target->RemoveAllAurasByMechanic( GetProto()->eff[i].EffectMiscValue , (uint32)(-1) , true );
					// Remove all debuffs of that mechanic type.
					// This is also done in SpellAuras.cpp - wtf?
				}
				/*
				if( GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MECHANIC_IMMUNITY && (GetProto()->eff[i].EffectMiscValue == 12 || GetProto()->eff[i].EffectMiscValue == 17))
				{
					for(uint32 x=MAX_POSITIVE_AURAS;x<MAX_AURAS;x++)
						if(target->m_auras[x])
							if(target->m_auras[x]->GetSpellProto()->MechanicsType == GetProto()->eff[i].EffectMiscValue)
								target->m_auras[x]->Remove();
				}
				*/
/*			}
		}*/

		if( ( GetProto()->c_is_flags2 & SPELL_FLAG2_IS_CASTABLE_PACIFIED ) == 0
			&& ( GetProto()->c_is_flags & SPELL_FLAG_IS_DAMAGING ) != 0
			&& u_caster->IsPacified() && GetProto()->School == NORMAL_DAMAGE ) // only affects physical damage
		{
			return SPELL_FAILED_PACIFIED;
		}

		if( ( GetProto()->c_is_flags2 & SPELL_FLAG2_IS_CASTABLE_FEARED_CHARM_SLEEP ) == 0 && u_caster->IsFeared() )
		{
			return SPELL_FAILED_NOT_IN_CONTROL;
		}
		if( ( GetProto()->c_is_flags2 & SPELL_FLAG2_IS_CASTABLE_STUNNED ) == 0 && u_caster->IsStunned() )
		{
			if( p_caster == NULL || p_caster->HasGlyphWithID( GLYPH_PRIEST_OF_DESPERATION ) == false )
				return SPELL_FAILED_STUNNED;
		}

		if(u_caster->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT) > 0)
		{
			SpellEntry * t_spellInfo = (u_caster->GetCurrentSpell() ? u_caster->GetCurrentSpell()->GetProto() : NULL);

			if(!t_spellInfo || !pSpellId)
				return SPELL_FAILED_SPELL_IN_PROGRESS;
			else if (t_spellInfo)
			{
				if(
					t_spellInfo->eff[0].EffectTriggerSpell != GetProto()->Id &&
					t_spellInfo->eff[1].EffectTriggerSpell != GetProto()->Id &&
					t_spellInfo->eff[2].EffectTriggerSpell != GetProto()->Id)
				{
					return SPELL_FAILED_SPELL_IN_PROGRESS;
				}
			}
		}
	}

	// no problems found, so we must be ok
	return SPELL_CANCAST_OK;
}

void Spell::RemoveItems()
{
	// Item Charges & Used Item Removal
	if(i_caster)
	{
		if( sWorld.getIntRate( INTRATE_DISABLEITEMCONSUME ) == 0 )
		{
			// Stackable Item -> remove 1 from stack
			if ( i_caster->GetUInt32Value(ITEM_FIELD_STACK_COUNT) > 1 )
			{
				i_caster->ModUnsigned32Value( ITEM_FIELD_STACK_COUNT, -1 );
				i_caster->m_isDirty = true;
			}
			else
			{
				for ( uint32 x = 0; x < 5; x++ )
				{
					int32 charges = (int32)i_caster->GetUInt32Value( ITEM_FIELD_SPELL_CHARGES + x );
					if ( charges == -1 ) // if expendable item && item has no charges remaining -> delete item
					{
						//i bet this crashed happend due to some script. Items wihtout owners ?
						if( i_caster->GetOwner() && i_caster->GetOwner()->GetItemInterface() )
							i_caster->GetOwner()->GetItemInterface()->SafeFullRemoveItemByGuid( i_caster->GetGUID() );
						i_caster = NULL;
						break;
					}
					else if ( charges > 0 || charges < -1 ) // remove 1 charge
					{
						i_caster->ModSignedInt32Value( ITEM_FIELD_SPELL_CHARGES + x, ( charges < 0 ) ? +1 : -1 ); // if charges < 0 then -1 else +1
						i_caster->m_isDirty = true;
						break;
					}
				}
			}
		}
	}
	// Ammo Removal
#ifndef CATACLYSM_SUPPORT
	if( GetProto()->AttributesExB == FLAGS3_REQ_RANGED_WEAPON || GetProto()->AttributesExC & FLAGS4_PLAYER_RANGED_SPELLS)
	{
		p_caster->GetItemInterface()->RemoveItemAmt_ProtectPointer(p_caster->GetUInt32Value(PLAYER_AMMO_ID), 1, &i_caster);
	}
#endif
	// Reagent Removal
	if( !( i_caster != NULL && (i_caster->GetProto()->Flags &  ITEM_FLAGS_ENCHANT_SCROLL))	//these are enchanting scrolls and reagent was already consumend when enchanter created them
//		&& !(p_caster->removeReagentCost && GetProto()->AttributesExD & 2) ) //zack : wtf is flag 2 put there for ?
//		&& ( p_caster->removeReagentCost == false 
		&& ( ( GetProto()->SpellGroupType[0] & p_caster->GetUInt32Value(PLAYER_NO_REAGENT_COST_1 + 0) 
					|| GetProto()->SpellGroupType[1] & p_caster->GetUInt32Value(PLAYER_NO_REAGENT_COST_1+1) 
					|| GetProto()->SpellGroupType[2] & p_caster->GetUInt32Value(PLAYER_NO_REAGENT_COST_1+2) )
			|| (GetProto()->c_is_flags3 & SPELL_FLAG3_IS_DISABLE_OTHER_SPELL_CPROC ) != 0 ) )
	{
		SpellReagentsEntry *sre = dbcSpellReagents.LookupEntryForced( GetProto()->SpellReagentsId );
		if( sre )
		{
			for(uint32 i=0; i<8 ;i++)
			{
				if( sre->Reagent[i])
				{
					p_caster->GetItemInterface()->RemoveItemAmt_ProtectPointer(sre->Reagent[i], sre->ReagentCount[i], &i_caster);
				}
			}
		}
	}
}

int32 Spell::CalculateEffect(uint32 i,Unit *target)
{
	int32 value = 0;

	float basePointsPerLevel    = GetProto()->eff[i].EffectRealPointsPerLevel;
	int32 basePoints = GetProto()->eff[i].EffectBasePoints;
	int32 randomPoints = GetProto()->eff[i].EffectDieSides;
	int32 ComboPointScaleing = 1;
	forced_pct_mod_cur = forced_pct_mod[i];

	//added by Zack : some talents inherit their basepoints from the previously casted spell: see mage - Master of Elements
	if(forced_basepoints[i])
		basePoints = forced_basepoints[i];
	//Zack, somehow spells will scale with player level. Not dead sure about the formula 
	else
	{
		if( GetProto()->SpellScalingId && u_caster )
		{
			SpellScalingEntry *ss = &GetProto()->ss;
			if( ss->coeff_points_gtscale[ i ] )
			{
				int32 use_class = ss->playerClass;
				if( use_class != 0 )
				{
					//class 12 is general class
					if( use_class == -1 )
						use_class = 12;
					int32 use_level = (int32)u_caster->getLevel();
					//morron protection
					if( use_level > 100 )
						use_level = 100;
					gtClassLevelFloat *scale = dbcGTSpellScale.LookupEntry( use_level - 1 + (use_class-1) * 100 );
					if( scale )
					{
						float avg;
//						float min, max;
						if( ss->effLevelDiffCoeff != 1.0f )
						{
							//i guess this is downranking
							float coef2;
							coef2 = (float)(MIN( use_level, ss->effScalingMinLevel ));
							coef2 += (float)(MAX( 0, use_level - ss->effScalingMinLevel )) * ss->effLevelDiffCoeff;
							coef2 = coef2 / (float) use_level;
							coef2 = scale->val * coef2;
							avg = coef2 * ss->coeff_points_gtscale[ i ] ;
						}
						else
							avg = scale->val * ss->coeff_points_gtscale[ i ];
						//i wonder if this coefficient will be used for spell power bonus also
						if( ss->castTimeMin > 0 && ss->castScalingMaxLevel >= use_level )
						{
							float downranking ;
							if( ss->castTimeMax > 0 )
								downranking = (float)ss->castTimeMin+((float)((use_level-1)*(ss->castTimeMax-ss->castTimeMin))/(float)(ss->castScalingMaxLevel-1));
							else
								downranking = (float)ss->castTimeMin;
							downranking = MIN( ss->castTimeMax, downranking );
							downranking = downranking / (float)ss->castTimeMax;
							avg = avg * downranking;
						}
//						min = avg - avg * ss->coeff_randompoints_gtscale[ i ] / 2;
//						max = avg + avg * ss->coeff_randompoints_gtscale[ i ] / 2;
						//this is the value used in formulas as $m !!!
						basePoints = float2int32( avg );
						//noticed for shield of the rightous that ss->coeff_points_gtscale[ i ] should be larger with 0.0675290. Maybe our DBC is not up to date ? Or maybe the formula is wrong ?
						//noticed for hunter's mark the ss->coeff_points_gtscale[ i ] should be 1.57 instead 4.0
						randomPoints = float2int32( avg * ss->coeff_randompoints_gtscale[ i ] / 2.0f );
						//this is the value used in formulas as $b !!!
						ComboPointScaleing = float2int32( scale->val * ss->coeff_combopoint_extra_gtscale[ i ] );
					}
				}
			}
		}
	}

	// Random suffix value calculation 
	if(i_caster && i_caster->GetInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID) < 0 )
	{
		int32 suffix_entry = -i_caster->GetInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID);
        ItemRandomSuffixEntry * si = dbcItemRandomSuffix.LookupEntry( suffix_entry );
		EnchantEntry * ent;
		uint32 j,k;

		for(j = 0; j < 3; ++j)
		{
			if(si->enchantments[j] != 0)
			{
				ent = dbcEnchant.LookupEntry(si->enchantments[j]);
				for(k = 0; k < 3; ++k)
				{
					if(ent->spell[k] == GetProto()->Id)
					{
						if(si->prefixes[k] == 0)
							goto exit;

						value = RANDOM_SUFFIX_MAGIC_CALCULATION(si->prefixes[j], i_caster->GetItemRandomSuffixFactor());

						if(value == 0)
							goto exit;

						return value;
					}
				}
			}
		}
	}
exit:

	if( basePointsPerLevel != 0 && u_caster != NULL 
		// i bet people will QQ about this. 
		&& ( GetProto()->SpellScalingId == 0 || GetProto()->ss.coeff_points_gtscale[ i ] == 0 )	
		//DO NOT SCALE VALUE LIKE WEAPON DMG PERCENT ! In fact only scale flat dmg and nothing else ?
		//Swipe does not require it. Backstab requires it
//		&& ( GetProto()->eff[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE || GetProto()->eff[i].Effect == SPELL_EFFECT_DUMMYMELEE )
	)
	{
		int32 diff = -(int32)GetProto()->SpellLevel.baseLevel;
		if (GetProto()->SpellLevel.maxLevel && u_caster->getLevel()>GetProto()->SpellLevel.maxLevel)
			diff +=GetProto()->SpellLevel.maxLevel;
		else
			diff +=u_caster->getLevel();
		basePoints += float2int32(diff * basePointsPerLevel ); 
/*		int32 level = int32(u_caster->getLevel());
		if (level > int32(GetProto()->SpellLevel.maxLevel) && GetProto()->SpellLevel.maxLevel > 0)
            level = int32(GetProto()->SpellLevel.maxLevel);
        else if (level < int32(GetProto()->SpellLevel.baseLevel))
            level = int32(GetProto()->SpellLevel.baseLevel);
		level -= int32(GetProto()->SpellLevel.spellLevel);
        basePoints += int32(level * basePointsPerLevel); */
	}

	if(randomPoints < -1 )
		value = basePoints - ( RandomUInt() % ( -randomPoints ) );
	else 
	{
		if(randomPoints <= 1)
			value = basePoints;
		else
			value = basePoints + RandomUInt() % randomPoints;
	}

	//scripted shit
	int32 comboDamage = (int32)GetProto()->eff[i].EffectPointsPerComboPoint;
	//new formula overrides old one
	if( ComboPointScaleing != 1 )
		comboDamage = ComboPointScaleing;
	if( GetProto()->EffectHandler )
	{
		//do not change these. this is made like this so custom scripts can get both $m and $b values
		damage = value;
		if( ComboPointScaleing != 1 )
			value += ComboPointScaleing;	//403, not sure how this is correct, or even if it should be called combopoint damage
		uint32 ret = GetProto()->EffectHandler( this, target, value, i );

		//fully scripted effect is skipping rest of the default calculations. Not a recomended step
		if( ret == SPELL_EFFECT_OVERRIDE_BREAK_EXECUTION )
			return value;
	}
	//almost all combo related stuff are scripted.
	else
	{
		if( comboDamage && p_caster != NULL )
			value = ( value + comboDamage ) * p_caster->m_comboPoints;	//formula changed in 403 :(
	}
	if( comboDamage && p_caster != NULL )
	{
		m_requiresCP++;
	}

	if( p_caster != NULL )
	{
		ApplyClassScriptOverridesToEffect( value, i, p_caster, target );
#if 0
		// DK spells gain damage based on AP
		if( GetProto()->DK_AP_spell_bonus[i]!=0 && p_caster->GetItemInterface())
		{
/*			// http://deathknight.info/wiki/index.php?title=Mechanics -> all spells gain based on AP
			//Normalised Weapon Damage = Tool-tip Weapon Damage + (AP/14)*(Normalisation Constant) 
			//The Normalisation Constant for 2h weapons is 3.3
			//The Normalisation Constant for 1h weapons is 2.4
			Item *it = p_caster->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_MAINHAND );
			uint32 type = 0;
			uint32 bonus_value = 0;
			if( it && it->GetProto() )
				type = it->GetProto()->InventoryType;
			if( type == INVTYPE_WEAPON || type == INVTYPE_WEAPONMAINHAND )
				bonus_value += p_caster->GetAP() * 24 / 140;//		normalization = 2.4f;
			else if( type == INVTYPE_2HWEAPON )
				bonus_value += p_caster->GetAP() * 33 / 140;//		normalization = 3.3f;
			else 
				bonus_value += p_caster->GetAP(); */
			uint32 bonus_value = p_caster->GetAP();

			bonus_value = float2int32(((float)bonus_value) * GetProto()->DK_AP_spell_bonus[ i ] );

			// in case spell is over time then we divide the ammount
			if( ProcedOnSpell && ProcedOnSpell->quick_tickcount > GetProto()->quick_tickcount )
				value += bonus_value / ProcedOnSpell->quick_tickcount; //these are periodic trigger pells
			else if( GetProto()->c_is_flags2 & SPELL_FLAG2_IS_VALUE_OVER_TIME )
				value += bonus_value / GetProto()->quick_tickcount;
			else
				value += bonus_value;
		}
#endif
		if( (GetProto()->c_is_flags & SPELL_FLAG_IS_REQ_ITEM_CASTER_MOD_CHK) && i_caster )
		{
			if( p_caster->has_mixology_talent && ( GetProto()->c_is_flags & SPELL_FLAG_IS_FLASK_OR_ELIXIR ) )
				value = value*3/2;	//this is not flat 1.5, It depends on item and effect too
			if( p_caster->has_alchemist_stone_effect && ( GetProto()->c_is_flags & SPELL_FLAG_IS_ALCHEMIST_STONE_TARGET ) )
				value = value*140/100;	//fixed 40% extra effect
		}
	}

	// TODO: INHERIT ITEM MODS FROM REAL ITEM OWNER - BURLEX BUT DO IT PROPERLY
	if( Need_SM_Apply( GetProto() ) )
	{
		if( target && target != u_caster && target->SM_Mods )
		{
			//!! it is possible that this only affects first effect only ?
			SM_FIValue(target->SM_Mods->SM_FEffectTargetBonus,&value,GetProto()->GetSpellGroupType());
			SM_PIValue(target->SM_Mods->SM_PEffectTargetBonus,&value,GetProto()->GetSpellGroupType());
		}
		if( u_caster != NULL && u_caster->SM_Mods )
		{
			SM_FIValue(u_caster->SM_Mods->SM_FMiscEffect,&value,GetProto()->GetSpellGroupType());
			SM_PIValue(u_caster->SM_Mods->SM_PMiscEffect,&value,GetProto()->GetSpellGroupType());
			if( i == 0 )
			{
				SM_FIValue(u_caster->SM_Mods->SM_FAddEffect1,&value,GetProto()->GetSpellGroupType());
				SM_PIValue(u_caster->SM_Mods->SM_PAddEffect1,&value,GetProto()->GetSpellGroupType());
			}
			else if( i == 1 )
			{
				SM_FIValue(u_caster->SM_Mods->SM_FAddEffect2,&value,GetProto()->GetSpellGroupType());
				SM_PIValue(u_caster->SM_Mods->SM_PAddEffect2,&value,GetProto()->GetSpellGroupType());
			}
			else if( i == 2 )
			{
				SM_FIValue(u_caster->SM_Mods->SM_FAddEffect3,&value,GetProto()->GetSpellGroupType());
				SM_PIValue(u_caster->SM_Mods->SM_PAddEffect3,&value,GetProto()->GetSpellGroupType());
			}

			SM_FIValue(u_caster->SM_Mods->SM_FEffectBonus,&value,GetProto()->GetSpellGroupType());
			SM_PIValue(u_caster->SM_Mods->SM_PEffectBonus,&value,GetProto()->GetSpellGroupType());
		}
		else if( i_caster != NULL && target)
		{
			//we should inherit the modifiers from the conjured food caster
			Unit *item_creator = target->GetMapMgr()->GetUnit( i_caster->GetUInt64Value( ITEM_FIELD_CREATOR ) );
			if( item_creator != NULL && item_creator->SM_Mods )
			{
				SM_FIValue(item_creator->SM_Mods->SM_FMiscEffect,&value,GetProto()->GetSpellGroupType());
				SM_PIValue(item_creator->SM_Mods->SM_PMiscEffect,&value,GetProto()->GetSpellGroupType());

				SM_FIValue(item_creator->SM_Mods->SM_FEffectBonus,&value,GetProto()->GetSpellGroupType());
				SM_PIValue(item_creator->SM_Mods->SM_PEffectBonus,&value,GetProto()->GetSpellGroupType());
			}
		}
	}

	//this is some custom shit to make heroic instnaces a little bit harder to make then normal ones
	if( u_caster && u_caster->IsCreature() && u_caster->GetMapMgr() && target && target->IsPlayer() )
	{
		if( ( GetProto()->c_is_flags & SPELL_FLAG_IS_HEALING ) && IsHealingEffect( GetProto(), i ) )
		{
			if( IS_HEROIC_INSTANCE_DIFFICULTIE( u_caster->GetMapMgr()->instance_difficulty ) )
				value = (int32)( value * sWorld.mob_HeroicHealSpellFactor );
			else 
				value = (int32)( value * sWorld.mob_NormalHealSpellFactor );
		}
		if( ( GetProto()->c_is_flags & SPELL_FLAG_IS_DAMAGING ) && IsDamagingSpell( GetProto(), i ) )
		{
			if( IS_HEROIC_INSTANCE_DIFFICULTIE( u_caster->GetMapMgr()->instance_difficulty ) )
				value = (int32)( value * sWorld.mob_HeroicDmgSpellFactor );
			else 
				value = (int32)( value * sWorld.mob_NormalDmgSpellFactor );

			//check to not kill the poor mothe fucker in 1 blow ?
			if( sWorld.mob_SpellKillerBlowReduction && value < sWorld.mob_SpellKillerBlowBelow )
			{
				int32 max_dmg = (int32)( target->GetUInt32Value( UNIT_FIELD_MAXHEALTH ) / sWorld.mob_SpellKillerBlowReduction ) ;
				if( value > max_dmg )
					value = max_dmg;
			}
		}
	}
#ifdef _DEBUG
	if( p_caster )
	{
		int64 *val = p_caster->GetCreateIn64Extension( EXTENSION_ID_MOD_SPELL_VALUE );
		value += *val;
	}
#endif
	return value;
}

void Spell::HandleTeleport(uint32 id, Unit* Target)
{
	if(Target->GetTypeId()!=TYPEID_PLAYER)
	{ 
		return;
	}

	Player* pTarget = SafePlayerCast( Target );

	float x,y,z;
	uint32 mapid;

	// predefined behavior
	if (GetProto()->Id == 8690 || GetProto()->Id == 556 || GetProto()->Id == 39937)// 556 - Astral Recall ; 39937 - Ruby Slippers
	{
		x = pTarget->GetBindPositionX();
		y = pTarget->GetBindPositionY();
		z = pTarget->GetBindPositionZ();
		mapid = pTarget->GetBindMapId();
	}
	else // normal behavior
	{
		TeleportCoords* TC = TeleportCoordStorage.LookupEntry(id);
		if(!TC)
		{ 
			return;
		}

		x=TC->x;
		y=TC->y;
		z=TC->z;
		mapid=TC->mapId;
	}
	pTarget->EventAttackStop();
	pTarget->SetSelection(0);

	// We use a teleport event on this one. Reason being because of UpdateCellActivity,
	// the game object set of the updater thread WILL Get messed up if we teleport from a gameobject
	// caster.
	if(!sEventMgr.HasEvent(pTarget, EVENT_PLAYER_TELEPORT))
		sEventMgr.AddEvent(pTarget, &Player::EventTeleport, mapid, x, y, z, EVENT_PLAYER_TELEPORT, 1, 1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
}

void Spell::CreateItem(uint32 itemId)
{
    if( !itemId || !p_caster )
    { 
        return;
    }

	Item*			newItem;
	Item*			add;
	SlotResult		slotresult;
	ItemPrototype*	m_itemProto;

	m_itemProto = ItemPrototypeStorage.LookupEntry( itemId );
	if( m_itemProto == NULL )
	{ 
	    return;
	}

	if (p_caster->GetItemInterface()->CanReceiveItem(m_itemProto, 1))
	{
		SendCastResult(SPELL_FAILED_TOO_MANY_OF_ITEM);
		return;
	}

	add = p_caster->GetItemInterface()->FindItemLessMax(itemId, 1, false);
	if (!add)
	{
		slotresult = p_caster->GetItemInterface()->FindFreeInventorySlot(m_itemProto);
		if(!slotresult.Result)
		{
			 SendCastResult(SPELL_FAILED_TOO_MANY_OF_ITEM);
			 return;
		}

		newItem = objmgr.CreateItem(itemId, p_caster);
		AddItemResult result = p_caster->GetItemInterface()->SafeAddItem(newItem, slotresult.ContainerSlot, slotresult.Slot);
		if( result == ADD_ITEM_RESULT_ERROR )
		{
			newItem->DeleteMe();
			return;
		}

		newItem->SetUInt64Value(ITEM_FIELD_CREATOR,m_caster->GetGUID());
		newItem->SetUInt32Value(ITEM_FIELD_STACK_COUNT, damage);

		/*WorldPacket data(45);
		p_caster->GetSession()->BuildItemPushResult(&data, p_caster->GetGUID(), 1, 1, itemId ,0,0xFF,1,0xFFFFFFFF);
		p_caster->SendMessageToSet(&data, true);*/
		p_caster->GetSession()->SendItemPushResult(newItem,true,false,true,true,slotresult.ContainerSlot,slotresult.Slot,1);
		newItem->m_isDirty = true;

	}
	else
	{
		add->SetUInt32Value(ITEM_FIELD_STACK_COUNT,add->GetUInt32Value(ITEM_FIELD_STACK_COUNT) + damage);
		/*WorldPacket data(45);
		p_caster->GetSession()->BuildItemPushResult(&data, p_caster->GetGUID(), 1, 1, itemId ,0,0xFF,1,0xFFFFFFFF);
		p_caster->SendMessageToSet(&data, true);*/
		p_caster->GetSession()->SendItemPushResult(add,true,false,true,false,p_caster->GetItemInterface()->GetBagSlotByGuid(add->GetGUID()),0xFFFFFFFF,1);
		add->m_isDirty = true;
	}
}

void Spell::DetermineSkillUp(uint32 skillid,uint32 steps,uint32 targetlevel)
{
	if(!p_caster)
	{ 
		return;
	}
	if(p_caster->GetSkillUpChance(skillid)<0.01)
	{
		return;//to preven getting higher skill than max
	}
	steps = MAX( 1, steps );
	int32 diff=p_caster->_GetSkillLineCurrent(skillid,false)/5-targetlevel;
	if(diff<0)
		diff=-diff;
	float chance;
	if(diff<=5)
		chance=95.0;
	else if(diff<=10)
		chance=66;
	else if(diff <=15)
		chance=33;
	else 
	{ 
		return;
	}
	if(RandChance(chance*sWorld.getRate(RATE_SKILLCHANCE)))
		p_caster->_AdvanceSkillLine(skillid, float2int32( steps * sWorld.getRate(RATE_SKILLRATE)));
}

void Spell::DetermineSkillUp(uint32 skillid,uint32 steps)
{
	//This code is wrong for creating items and disenchanting.
	if(!p_caster)
	{ 
		return;
	}
	steps = MAX( 1, steps );
	float chance = 0.0f;
	skilllinespell* skill = GetProto()->spell_skill;
	if( skill && SafePlayerCast( m_caster )->_HasSkillLine( skill->skillId ) )
	{
		uint32 amt = SafePlayerCast( m_caster )->_GetSkillLineCurrent( skill->skillId, false );
		uint32 max = SafePlayerCast( m_caster )->_GetSkillLineMax( skill->skillId );
		if( amt >= max )
		{ 
			return;
		}
		if( amt >= skill->max_value ) //grey
			chance = 0.0f;
		else if( ( amt >= ( ( ( skill->max_value - skill->min_value) / 2 ) + skill->min_value ) ) ) //green
			chance = 33.0f;
		else if( amt >= skill->min_value ) //yellow
			chance = 66.0f;
		else //brown
			chance=100.0f;
		if(RandChance(chance*sWorld.getRate(RATE_SKILLCHANCE)))
			p_caster->_AdvanceSkillLine(skillid, float2int32( steps * sWorld.getRate(RATE_SKILLRATE)));
	}
}

bool Spell::SafeAddTarget(TargetsList* tgt,uint64 guid)
{
	if(guid == 0)
	{ 
		return false;
	}

#ifdef MAX_SPELL_TARGET_COUNT
	if( MAX_SPELL_TARGET_COUNT <= tgt->size() )
		return false;
#endif

	for(TargetsList::iterator i=tgt->begin();i!=tgt->end();i++)
		if((*i)==guid)
		{ 
			return false;
		}

	tgt->push_back(guid);
	return true;
}

void Spell::SafeAddMissedTarget(uint64 guid)
{
    for(SpellTargetsList::iterator i=ModeratedTargets.begin();i!=ModeratedTargets.end();i++)
        if((*i).TargetGuid==guid)
        {
            //sLog.outDebug("[SPELL] Something goes wrong in spell target system");
			// this isnt actually wrong, since we only have one missed target map,
			// whereas hit targets have multiple maps per effect.
            return;
        }

    ModeratedTargets.push_back(SpellTargetMod(guid,2));
}

void Spell::SafeAddModeratedTarget(uint64 guid, uint16 type)
{
	for(SpellTargetsList::iterator i=ModeratedTargets.begin();i!=ModeratedTargets.end();i++)
		if((*i).TargetGuid==guid)
        {
            //sLog.outDebug("[SPELL] Something goes wrong in spell target system");
			// this isnt actually wrong, since we only have one missed target map,
			// whereas hit targets have multiple maps per effect.
			return;
        }

	ModeratedTargets.push_back(SpellTargetMod(guid, (uint8)type));
}

SpellReflectState Spell::Reflect(Unit *refunit)
{
	SpellEntry * refspell = NULL;

	//do not bounce reflected spell. we can not reflect spells that we cast on ourself
	if( IsReflected() != SPELL_REFLECT_NONE || refunit == NULL || m_caster == refunit )
	{ 
		return SPELL_REFLECT_NONE;
	}

	//do not ping pong reflecting spells
	if( GetProto()->c_is_flags2 & SPELL_FLAG2_IS_NON_REFLECTABLE )
		return SPELL_REFLECT_NONE;

	refunit->m_reflectSpellSchool.BeginLoop();
	for(CommitPointerListNode<struct ReflectSpellSchool> *i = refunit->m_reflectSpellSchool.begin();i != refunit->m_reflectSpellSchool.end();i = i->Next())
	{
		ReflectSpellSchool *rss = i->data;
		if(rss->school == -1 || rss->school == (int32)GetProto()->School)
		{
			if( RandChance(rss->chance) )
			{
				//the god blessed special case : mage - Frost Warding = is an augmentation to frost warding
				if(rss->require_aura_hash && refunit->HasAuraWithNameHash( rss->require_aura_hash, 0, AURA_SEARCH_VISIBLE ) == NULL )
                {
					continue;
                }
				if (rss->charges > 0)
				{
					rss->charges--;
					if (rss->charges <= 0)
					{
						uint32 ID = rss->spellId;
						refunit->m_reflectSpellSchool.SafeRemove( i, 1 );
						rss = NULL;	//make sure it is not used somewhere else
						refunit->RemoveAura(ID,0,AURA_SEARCH_ALL);	//aura remove will not find this iterator !
					}
				}
				refspell = GetProto();
				break;
			}
		}
	}
	refunit->m_reflectSpellSchool.EndLoopAndCommit();

	if(!refspell) 
	{ 
		return SPELL_REFLECT_NONE;
	}

	//we cannot reflect spells back to items or GOs. But we can simply nulify the spellcast
	if( m_caster->IsUnit() == true )
	{
		Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		spell->Init( refunit, refspell, true, NULL);
		spell->SetReflected( SPELL_REFLECT_ALREADY_REFLECTED ); //this will deny the spell to be reflected again
		SpellCastTargets targets( m_caster->GetGUID() );
		//the reflected spell should have the values of the original caster. Mage casting fireball on warrior makes damn low damage when reflected
		for( uint32 i=0;i<MAX_SPELL_EFFECT_COUNT;i++)
		{
			spell->forced_basepoints[i] = CalculateEffect( i, refunit );
			if( refspell->Spell_value_is_scripted == false && IsDamagingSpell( refspell, i ) )
				spell->forced_basepoints[i] += u_caster->GetSpellDmgBonus( refunit, refspell, spell->forced_basepoints[i], i );
		}
		spell->prepare(&targets);
		//handle procs
		int32 SpellID = GetProto()->Id;
		if( refunit != u_caster )
		{
			refunit->HandleProc( PROC_ON_REFLECT_EVENT | PROC_ON_VICTIM, u_caster, GetProto(), &SpellID );
			u_caster->HandleProc( PROC_ON_REFLECT_EVENT | PROC_ON_NOT_VICTIM, refunit, GetProto(), &SpellID );
		}
		else
			u_caster->HandleProc( PROC_ON_REFLECT_EVENT | PROC_ON_VICTIM | PROC_ON_NOT_VICTIM, u_caster, GetProto(), &SpellID );
	}

	return SPELL_REFLECT_ORIGINAL_CANCEL;
}

void GetPVPDuration(uint32 * Duration, Unit * Target, SpellEntry * spell)
{
   uint32 status = spell->DiminishStatus;
   uint32 Grp = status & 0xFFFF;   // other bytes are if apply to pvp
   uint32 PvE = (status >> 16) & 0xFFFF;

   // Make sure we have a group
   if( Grp == 0xFFFF )
   { 
      return;
   }

   // Check if we don't apply to pve
   if(!PvE && !Target->IsPlayer() && !Target->IsPet())
   { 
      return;
   }

   // TODO: check for spells that should do this
   uint32 OldDur = *Duration;

   if( status == DIMINISHING_GROUP_MAX_PVP_DUR )
   {
	   *Duration = MIN( spell->DiminishStatusMaxPVPDur, OldDur );
	   return;
   }

   //this last max 10 sec in PVP and even then they get diminished
   if( OldDur > spell->DiminishStatusMaxPVPDur 
//	   &&
//	   ( status == DIMINISHING_GROUP_GOUGE_POLY_SAP 
//	   || status == DIMINISHING_GROUP_STUN
//	   || status == DIMINISHING_GROUP_ROOT
//	   || status == DIMINISHING_GROUP_FEAR
//	   || status == DIMINISHING_GROUP_SLEEP
//	   || status == DIMINISHING_GROUP_CHARM
//	   )
//	   && ( Target->IsPlayer() || Target->IsPet() ) 
	   )
   {
		*Duration = spell->DiminishStatusMaxPVPDur;
   }
}

void ApplyDiminishingReturnTimer(uint32 * Duration, Unit * Target, SpellEntry * spell)
{
	ApplyDiminishingReturnTimer( Duration, Target, spell->DiminishStatus, false, spell->DiminishStatusMaxPVPDur );
}

void ApplyDiminishingReturnTimer(uint32 * Duration, Unit * Target, uint32 grp, bool DoNotSetNew, uint32 MaxPVPDur )
{
   uint32 status = grp;
   uint32 Grp = status & 0xFFFF;   // other bytes are if apply to pvp
   uint32 PvE = (status >> 16) & 0xFFFF;

   // Make sure we have a group
   if( Grp == 0xFFFF )
   { 
      return;
   }

   // Check if we don't apply to pve
   if(!PvE && !Target->IsPlayer() && !Target->IsPet())
   { 
      return;
   }

   // TODO: check for spells that should do this
   uint32 OldDur = *Duration;
   uint32 NewDur = OldDur;
   uint32 count;

   if( status == DIMINISHING_GROUP_MAX_PVP_DUR )
   {
	   *Duration = MIN( MaxPVPDur, OldDur );
	   return;
   }

   //this last max 10 sec in PVP and even then they get diminished
   if( OldDur > MaxPVPDur 
//	   &&
//	   ( status == DIMINISHING_GROUP_GOUGE_POLY_SAP 
//	   || status == DIMINISHING_GROUP_STUN
//	   || status == DIMINISHING_GROUP_ROOT
//	   || status == DIMINISHING_GROUP_FEAR
//	   || status == DIMINISHING_GROUP_SLEEP
//	   || status == DIMINISHING_GROUP_CHARM
//	   )
//	   && ( Target->IsPlayer() || Target->IsPet() ) 
	   )
   {
		OldDur = MaxPVPDur;
		NewDur = MaxPVPDur;	//in case first sap
   }

   //we are applying an aura on target so we can increase this
   if( getMSTime() >= Target->m_diminishExpireStamp[Grp] )
   {
		Target->m_diminishCount[Grp] = 1;
	   //set new timer -> make sure we will be able to have 100(0),50(1),25(2),0(3+)
		count = 0;
   }
   else
   {
	   count = Target->m_diminishCount[Grp];
	   if( DoNotSetNew == false )
			Target->m_diminishCount[Grp]++;
   }

   if( count )
   {
	   if( count > 2 ) // Target immune to spell
		  NewDur = 0;
	   else 
	   {
		  uint32 tDur = OldDur >> count; //100%, 50%, 25% bitwise
		  if( status == DIMINISHING_GROUP_FIX_DIMINISHED )
		  {
			  if( tDur < 10000 )
				  tDur = 10000;
			  if( ( Target->IsPlayer() || Target->IsPet() ) && OldDur > tDur )
				  NewDur = tDur;
		  }
		  else if( tDur <= 1000 ) //4 sec, 2 sec, immune
			  NewDur = 0;
		  else
			  NewDur = tDur;
	   }
   }
	//if casted 15 seconds before the buff expires then the spell will be diminished
   if( NewDur && DoNotSetNew == false )
		Target->m_diminishExpireStamp[Grp] = getMSTime() + 15000 + NewDur;	//make sure we remove diminishing return only after we have aura expired

	*Duration = NewDur;
}

uint32 GetDiminishingGroup( SpellEntry *sp, uint32 NameHash )
{
	int32 grp = -1;
	bool pve = false;

	switch(NameHash)
	{
	case SPELL_HASH_BASH:
	case SPELL_HASH_CHEAP_SHOT:
	case SPELL_HASH_KIDNEY_SHOT:
	case SPELL_HASH_SHADOWFURY:
//	case SPELL_HASH_CHARGE_STUN:
//	case SPELL_HASH_INTERCEPT_STUN:
	case SPELL_HASH_CONCUSSION_BLOW:
	case SPELL_HASH_THROWDOWN:
	case SPELL_HASH_INTIMIDATION:
	case SPELL_HASH_WAR_STOMP:
	case SPELL_HASH_POUNCE:
	case SPELL_HASH_HAMMER_OF_JUSTICE:
	case SPELL_HASH_SHOCKWAVE:
//	case SPELL_HASH_CHARGE: //warrior 
	case SPELL_HASH_INTERCEPT:	//warrior
	case SPELL_HASH_IMPROVED_REVENGE:
	case SPELL_HASH_GAG_ORDER:
	case SPELL_HASH_DEEP_FREEZE:
	case SPELL_HASH_DEMON_CHARGE:
	case SPELL_HASH_GNAW:
	case SPELL_HASH_HOLY_WRATH:
	case SPELL_HASH_INFERNO_EFFECT:
	case SPELL_HASH_MAIM:
	case SPELL_HASH_RAVAGE:
	case SPELL_HASH_SONIC_BLAST:
	case SPELL_HASH_WEB_WRAP:
		{
			grp = DIMINISHING_GROUP_STUN;
			pve = true;
		}break;

//	case SPELL_HASH_STARFIRE_STUN:
 	case SPELL_HASH_STONECLAW_STUN:
	case SPELL_HASH_STUN:					// Generic ones
	case SPELL_HASH_BLACKOUT:
//	case SPELL_HASH_MACE_STUN_EFFECT:		// Mace Specialization
	case SPELL_HASH_IMPACT:		
//	case SPELL_HASH_SEAL_OF_JUSTICE:		
		{
			grp = DIMINISHING_GROUP_STUN_PROC;
			pve = true;
		}break;

	case SPELL_HASH_FROST_NOVA:
	case SPELL_HASH_RING_OF_FROST:
	case SPELL_HASH_FROST_TRAP:
	case SPELL_HASH_FREEZING_TRAP:
	case SPELL_HASH_FREEZE:
	case SPELL_HASH_PIN:
	case SPELL_HASH_EARTHGRAB:
	case SPELL_HASH_ENTANGLING_ROOTS:	//from natur's grasp
			grp = DIMINISHING_GROUP_ROOT;
		break;

	case SPELL_HASH_IMPROVED_WING_CLIP:	
	case SPELL_HASH_FROSTBITE:
	case SPELL_HASH_IMPROVED_HAMSTRING:
	case SPELL_HASH_ENTRAPMENT:
	case SPELL_HASH_SHATTERED_BARRIER:
	case SPELL_HASH_IMPROVED_CONE_OF_COLD:
//	case SPELL_HASH_PARALYSIS:		//this is a talent proc and not a direct cast spell
			grp = DIMINISHING_GROUP_ROOT_PROC;
		break;

	case SPELL_HASH_HIBERNATE:
 	case SPELL_HASH_WYVERN_STING:
 	case SPELL_HASH_SLEEP:
 	case SPELL_HASH_RECKLESS_CHARGE:		//Gobling Rocket Helmet
			grp = DIMINISHING_GROUP_SLEEP;
		break;

	case SPELL_HASH_CYCLONE:
		{
			grp = DIMINISHING_GROUP_BLIND_CYCLONE;
			pve = true;
		}break;

	case SPELL_HASH_GOUGE:
	case SPELL_HASH_REPENTANCE:				// Repentance
	case SPELL_HASH_SAP:
	case SPELL_HASH_POLYMORPH:				// Polymorph
	case SPELL_HASH_POLYMORPH__CHICKEN:		// Chicken
//	case SPELL_HASH_POLYMORPH__PIG:			// Pig
//	case SPELL_HASH_POLYMORPH__TURTLE:		// Turtle
	case SPELL_HASH_POLYMORPH__SHEEP:		// Good ol' sheep
	case SPELL_HASH_HEX:					
	case SPELL_HASH_HUNGERING_COLD:
	case SPELL_HASH_SHACKLE:
			grp = DIMINISHING_GROUP_GOUGE_POLY_SAP;
		break;

	case SPELL_HASH_FEAR:
	case SPELL_HASH_PSYCHIC_SCREAM:
	case SPELL_HASH_HOWL_OF_TERROR:
	case SPELL_HASH_SCARE_BEAST:
	case SPELL_HASH_INTIMIDATING_SHOUT:
	case SPELL_HASH_TURN_EVIL:
	case SPELL_HASH_BLIND:					//some say this needs to be in same group as fear 
			grp = DIMINISHING_GROUP_FEAR;
			if( sp->Id == 20511 ) //this intimidating shout glyph
				grp = DIMINISHING_GROUP_ROOT_PROC;
		break;

	case SPELL_HASH_SEDUCTION:
	case SPELL_HASH_ENSLAVE_DEMON:			// Enslave Demon
	case SPELL_HASH_MIND_CONTROL:
			grp = DIMINISHING_GROUP_CHARM;		//Charm???
		break;

/*	case SPELL_HASH_KIDNEY_SHOT:
		{
			grp = DIMINISHING_GROUP_KIDNEY_SHOT;
			pve = true;
		}break;*/

	case SPELL_HASH_DEATH_COIL:
	case SPELL_HASH_PSYCHIC_HORROR:
			if( sp->Id != 64058 )	//they said disarm has no diminishing return ?
				grp = DIMINISHING_GROUP_HORROR;
		break;
	

	case SPELL_HASH_BANISH:					// Banish
			grp = DIMINISHING_GROUP_BANISH;
		break;

	// group where only 10s limit in pvp is applied, not DR
	case SPELL_HASH_FREEZING_TRAP_EFFECT:	// Freezing Trap Effect
//	case SPELL_HASH_HAMSTRING:				// Hamstring - wowwiki says it should not have one
	case SPELL_HASH_CURSE_OF_TONGUES:
	case SPELL_HASH_JUDGEMENT:
		{
			grp = DIMINISHING_GROUP_FIX_DIMINISHED;
		}break;

	case SPELL_HASH_RIPOSTE:			// Riposte
	case SPELL_HASH_DISARM:			// Disarm
//	case SPELL_HASH_CHIMERA_SHOT:			
	case SPELL_HASH_DISMANTLE:			
	case SPELL_HASH_SNATCH:	
		{
			grp = DIMINISHING_GROUP_DISARM;
		}break;
	case SPELL_HASH_SILENCE:
	case SPELL_HASH_GARROTE___SILENCE:
	case SPELL_HASH_SILENCED___IMPROVED_COUNTERSPELL:
	case SPELL_HASH_SILENCED___IMPROVED_KICK:
	case SPELL_HASH_SILENCED___GAG_ORDER:
	case SPELL_HASH_ARCANE_TORRENT:
//	case SPELL_HASH_GARROTE:		//only silence ! not the normal buff also !
	case SPELL_HASH_IMPROVED_COUNTERSPELL:
	case SPELL_HASH_NETHER_SHOCK:
	case SPELL_HASH_SILENCING_SHOT:
	case SPELL_HASH_IMPROVED_KICK:
	case SPELL_HASH_SHIELD_OF_THE_TEMPLAR:
	case SPELL_HASH_SPELL_LOCK:
	case SPELL_HASH_STRANGULATE:
		{
 			grp = DIMINISHING_GROUP_SILENCE;
		}break;
//	case SPELL_HASH_SLOW:
	case SPELL_HASH_THUNDER_CLAP:
//	case SPELL_HASH_CHILBLAINS:			//only root will have DR, this is the "slow"
//	case SPELL_HASH_CHAINS_OF_ICE:		//there is a slow and there is a root if specced chilblains talent //only root has DR http://eu.battle.net/wow/en/forum/topic/7339014515
		{
 			grp = DIMINISHING_GROUP_SLOW;
		}break;
	case SPELL_HASH_SCATTER_SHOT:
	case SPELL_HASH_DRAGON_S_BREATH:
	case SPELL_HASH_SHACKLE_UNDEAD:
		{
 			grp = DIMINISHING_GROUP_DISORIENT;
		}break;
	case SPELL_HASH_CURSE_OF_THE_ELEMENTS:
	case SPELL_HASH_CURSE_OF_EXHAUSTION:
		{
 			grp = DIMINISHING_GROUP_MAX_PVP_DUR;
			sp->DiminishStatusMaxPVPDur = MIN( sp->quick_duration_min, 2 * 60 * 1000 );
		}break;
	case SPELL_HASH_HUNTER_S_MARK:
		{
 			grp = DIMINISHING_GROUP_MAX_PVP_DUR;
			sp->DiminishStatusMaxPVPDur = MIN( sp->quick_duration_min, 30 * 1000 );
		}break;
	case SPELL_HASH_FAERIE_FIRE:
	case SPELL_HASH_FAERIE_FIRE__FERAL_:
		{
 			grp = DIMINISHING_GROUP_MAX_PVP_DUR;
			sp->DiminishStatusMaxPVPDur = MIN( sp->quick_duration_min, 40 * 1000 );
		}break;
	}

	uint32 ret;
	if( pve )
		ret = grp | (1 << 16);
	else
		ret = grp;

	return ret;
}

void Spell::ApplyClassScriptOverridesToEffect(int32 &value, int32 i, Player *caster, Unit *target)
{
	if( caster == NULL )
		return;
	//!!!this atm is not list protected. In case one handler changes list it will crash!!
	SimplePointerListNode<SpellEffectOverrideScript> *itr;
	for( itr = caster->mSpellOverrideMap.begin(); itr != caster->mSpellOverrideMap.end(); itr = itr->Next() )
		itr->data->ApplyHandler( this, caster, target, value, i, itr->data );
	// this will be 99.9% empty
	if( target && target != caster )
		for( itr = target->mSpellOverrideMap.begin(); itr != target->mSpellOverrideMap.end(); itr = itr->Next() )
			if( ( itr->data->ori_spell->c_is_flags3 & SPELL_FLAG3_IS_OVERRIDE_ON_TARGET ) != 0 )
				itr->data->ApplyHandler( this, target, caster, value, i, itr->data );
}

uint32 Spell::GetDuration()
{
    if(bDurSet)
		return Dur;
    bDurSet=true;
	if( forced_duration )
	{
		this->Dur = forced_duration;
		if( (GetProto()->c_is_flags & SPELL_FLAG_IS_REQUIRING_SM_MOD) && u_caster && u_caster->SM_Mods )
		{
			SM_FIValue(u_caster->SM_Mods->SM_FDur,(int32*)&this->Dur,GetProto()->GetSpellGroupType());
			SM_PIValue(u_caster->SM_Mods->SM_PDur,(int32*)&this->Dur,GetProto()->GetSpellGroupType());
		}
#ifdef _DEBUG
		if( p_caster )
		{
			int64 *AddDur = p_caster->GetCreateIn64Extension( EXTENSION_ID_MOD_AUR_DURATION );
			this->Dur += *AddDur;
		}
#endif
		return this->Dur;
	}

    int32 c_dur = 0;
    if(GetProto()->DurationIndex)
    {
        SpellDuration *sd=dbcSpellDuration.LookupEntry(GetProto()->DurationIndex);
        if(sd)
        {
            //check for negative and 0 durations.
            //duration affected by level
            if((int32)sd->Duration1 < 0 && sd->Duration2 && u_caster)
            {
                this->Dur = uint32(((int32)sd->Duration1 + (sd->Duration2 * u_caster->getLevel())));
                if((int32)this->Dur > 0 && sd->Duration3 > 0 && (int32)this->Dur > (int32)sd->Duration3)
                {
                    this->Dur = sd->Duration3;
                }

                if((int32)this->Dur < 0)
					this->Dur = 0;
				c_dur = this->Dur;
            }
            if(sd->Duration1 >= 0 && !c_dur)
            {
                this->Dur = sd->Duration1;
            }
            //combo point lolerCopter? ;P
            if(p_caster)  
            {
                uint32 cp=p_caster->m_comboPoints;
                if(cp)
                {
                    uint32 bonus=(cp*(sd->Duration3-sd->Duration1))/5;
                    if(bonus)
                    {
                        this->Dur+=bonus;
                        m_requiresCP++;
                    }
                }
            }

            if( (GetProto()->c_is_flags & SPELL_FLAG_IS_REQUIRING_SM_MOD) && u_caster && u_caster->SM_Mods )
            {
                SM_FIValue(u_caster->SM_Mods->SM_FDur,(int32*)&this->Dur,GetProto()->GetSpellGroupType());
                SM_PIValue(u_caster->SM_Mods->SM_PDur,(int32*)&this->Dur,GetProto()->GetSpellGroupType());
#ifdef COLLECTION_OF_UNTESTED_STUFF_AND_TESTERS
				int spell_flat_modifers=0;
				int spell_pct_modifers=0;
				SM_FIValue(u_caster->SM_FDur,&spell_flat_modifers,GetProto()->GetSpellGroupType());
				SM_FIValue(u_caster->SM_PDur,&spell_pct_modifers,GetProto()->GetSpellGroupType());
				if(spell_flat_modifers!=0 || spell_pct_modifers!=0)
					printf("!!!!!spell duration mod flat %d , spell duration mod pct %d , spell duration %d, spell group %u\n",spell_flat_modifers,spell_pct_modifers,Dur,GetProto()->GetSpellGroupType());
#endif
            }
        }
        else
        {
            this->Dur = (uint32)-1;
        }
    }
    else
    {
         this->Dur = (uint32)-1;
    }

	if( p_caster && p_caster->has_mixology_talent && ( GetProto()->c_is_flags & SPELL_FLAG_IS_FLASK_OR_ELIXIR ) )
		this->Dur *= 2;

	//frikkin hackfix to make last tick happen
	this->Dur += GetProto()->quick_ExtendDuration;	//should let the last update to take place
#ifdef _DEBUG
		if( p_caster && this->Dur != (uint32)-1 )
		{
			int64 *AddDur = p_caster->GetCreateIn64Extension( EXTENSION_ID_MOD_AUR_DURATION );
			this->Dur += *AddDur;
		}
#endif

    return this->Dur;
}

float Spell::GetRadius(uint32 i)
{
	if(i>=MAX_SPELL_EFFECT_COUNT)return 0; //you are joking right ?
    if(bRadSet[i])return Rad[i];
    bRadSet[i]=true;
    Rad[i]=::GetRadius(dbcSpellRadius.LookupEntry(GetProto()->eff[i].EffectRadiusIndex));
    float maxrad=::GetRadius(dbcSpellRadius.LookupEntry(GetProto()->eff[i].EffectRadiusMaxIndex));
	Rad[i]=MAX( Rad[i], maxrad );
	if((GetProto()->c_is_flags & SPELL_FLAG_IS_REQUIRING_SM_MOD) && u_caster && u_caster->SM_Mods )
    {
        SM_FFValue(u_caster->SM_Mods->SM_FRadius,&Rad[i],GetProto()->GetSpellGroupType());
        SM_PFValue(u_caster->SM_Mods->SM_PRadius,&Rad[i],GetProto()->GetSpellGroupType());
#ifdef COLLECTION_OF_UNTESTED_STUFF_AND_TESTERS
		float spell_flat_modifers=0;
		float spell_pct_modifers=1;
		SM_FFValue(u_caster->SM_FRadius,&spell_flat_modifers,GetProto()->GetSpellGroupType());
		SM_PFValue(u_caster->SM_PRadius,&spell_pct_modifers,GetProto()->GetSpellGroupType());
		if(spell_flat_modifers!=0 || spell_pct_modifers!=1)
			printf("!!!!!spell radius mod flat %f , spell radius mod pct %f , spell radius %f, spell group %u\n",spell_flat_modifers,spell_pct_modifers,Rad[i],GetProto()->GetSpellGroupType());
#endif
    }

    return Rad[i];
}

void Spell::CheckAndInterruptStealth()
{
	if( p_caster )
	{
		if( p_caster->IsStealth() 
			&& ( (m_spellInfo->AttributesEx & ATTRIBUTESEX_NOT_BREAK_STEALTH) == 0
				&& (m_spellInfo->c_is_flags & ( SPELL_FLAG_IS_HEALING | SPELL_FLAG_IS_DAMAGING ) ) != 0 //unless this is a damaging or healing spell it's ok
				&& (m_spellInfo->Attributes & ATTRIBUTES_PASSIVE) == 0
				&& (pSpellId == 0 || ( ( dbcSpell.LookupEntry(pSpellId)->Attributes & ATTRIBUTES_PASSIVE ) == 0 ) )
				|| ( m_spellInfo->c_is_flags2 & SPELL_FLAG2_IS_BREAKING_STEALTH ) )
			)
		{
			p_caster->RemoveAura(p_caster->m_stealth,0,AURA_SEARCH_POSITIVE);
//			p_caster->m_stealth = 0;
		}
	}
}

void Spell::safe_cancel( bool ObjectIsDestroyed )
{
	//if ObjectIsDestroyed it will leak the spell as "cancel" will never be called
	m_cancelled = true;
}

bool IsInrange(LocationVector & location, Object * o, float square_r)
{
    float r = o->GetDistanceSq(location);
    return ( r<=square_r);
}
 /*  
bool IsInrange(Object * o1,Object * o2,float square_r)
{
    return IsInrange(o1->GetPositionX(),o1->GetPositionY(),o1->GetPositionZ(),
        o2->GetPositionX(),o2->GetPositionY(),o2->GetPositionZ(),square_r);
}*/

bool TargetTypeCheck(Object *obj,uint32 ReqCreatureTypeMask)
{
	if( !ReqCreatureTypeMask )
		return true;

	if( obj->IsCreature() )
	{
		CreatureInfo* inf = SafeCreatureCast( obj )->GetCreatureInfo();
		if( inf == NULL || !( 1 << ( inf->Type - 1 ) & ReqCreatureTypeMask ) )
			return false;
	}
	else if( obj->IsPlayer() )
	{
		uint32 ss = SafePlayerCast( obj )->GetShapeShift();
		if( ss == 0 && (UNIT_TYPE_HUMANOID_BIT & ReqCreatureTypeMask) == 0 )
			return false;
		static uint32 ShapeShiftToCreatureMask[ 33 ] ;
		ShapeShiftToCreatureMask[ FORM_NORMAL ] = UNIT_TYPE_HUMANOID_BIT;
		ShapeShiftToCreatureMask[ FORM_CAT ] = UNIT_TYPE_BEAST_BIT;
		ShapeShiftToCreatureMask[ FORM_TREE ] = UNIT_TYPE_HUMANOID_BIT;
		ShapeShiftToCreatureMask[ FORM_TRAVEL ] = UNIT_TYPE_BEAST_BIT;
		ShapeShiftToCreatureMask[ FORM_AQUA ] = UNIT_TYPE_BEAST_BIT;
		ShapeShiftToCreatureMask[ FORM_BEAR ] = UNIT_TYPE_BEAST_BIT;
		ShapeShiftToCreatureMask[ FORM_GHOUL ] = (1<<(UNDEAD-1));
		ShapeShiftToCreatureMask[ FORM_DIREBEAR ] = UNIT_TYPE_BEAST_BIT;
		ShapeShiftToCreatureMask[ FORM_CUSTOM_NORMAL ] = UNIT_TYPE_HUMANOID_BIT;
		ShapeShiftToCreatureMask[ FORM_PLAYER ] = UNIT_TYPE_HUMANOID_BIT;
		ShapeShiftToCreatureMask[ FORM_SHADOW_DANCE ] = UNIT_TYPE_HUMANOID_BIT;
		ShapeShiftToCreatureMask[ FORM_CREATUREBEAR ] = UNIT_TYPE_BEAST_BIT;
		ShapeShiftToCreatureMask[ FORM_CREATURECAT ] = UNIT_TYPE_BEAST_BIT;
		ShapeShiftToCreatureMask[ FORM_GHOSTWOLF ] = UNIT_TYPE_BEAST_BIT;
		ShapeShiftToCreatureMask[ FORM_BATTLESTANCE ] = UNIT_TYPE_HUMANOID_BIT;
		ShapeShiftToCreatureMask[ FORM_DEFENSIVESTANCE ] = UNIT_TYPE_HUMANOID_BIT;
		ShapeShiftToCreatureMask[ FORM_BERSERKERSTANCE ] = UNIT_TYPE_HUMANOID_BIT;
		ShapeShiftToCreatureMask[ FORM_ZOMBIE ] = (1<<(UNDEAD-1));
		ShapeShiftToCreatureMask[ FORM_DEMON ] = (1<<(DEMON-1));
		ShapeShiftToCreatureMask[ FORM_LICHBORN ] = (1<<(UNDEAD-1));
		ShapeShiftToCreatureMask[ FORM_SWIFT ] = UNIT_TYPE_BEAST_BIT;
		ShapeShiftToCreatureMask[ FORM_SHADOW ] = UNIT_TYPE_HUMANOID_BIT;
		ShapeShiftToCreatureMask[ FORM_FLIGHT ] = UNIT_TYPE_BEAST_BIT;
		ShapeShiftToCreatureMask[ FORM_STEALTH ] = UNIT_TYPE_HUMANOID_BIT;
		ShapeShiftToCreatureMask[ FORM_MOONKIN ] = UNIT_TYPE_BEAST_BIT;
		ShapeShiftToCreatureMask[ FORM_SPIRITOFREDEMPTION ] = UNIT_TYPE_HUMANOID_BIT;

		if( ss <= 32 && (ShapeShiftToCreatureMask[ss] & ReqCreatureTypeMask) == 0 )
			return false;
	}
	//invalid type or wrong object
	else 
		return false;

	return true;
}

int GetAiTargetType(SpellEntry *sp)
{
	/*	this is not good as one spell effect can target self and other one an enemy,
		maybe we should make it for each spell effect or use as flags */	
	if( 
		HasTargetType(sp,EFF_TARGET_INVISIBLE_OR_HIDDEN_ENEMIES_AT_LOCATION_RADIUS) ||
		HasTargetType(sp,EFF_TARGET_ALL_TARGETABLE_AROUND_LOCATION_IN_RADIUS) ||
		HasTargetType(sp,EFF_TARGET_ALL_ENEMY_IN_AREA) ||
		HasTargetType(sp,EFF_TARGET_ALL_ENEMY_IN_AREA_INSTANT) ||
		HasTargetType(sp,EFF_TARGET_ALL_ENEMY_IN_AREA_CHANNELED) ||
		HasTargetType(sp,EFF_TARGET_ALL_SCRIPTED_OR_TARGETABLE_AROUND_LOCATION_IN_RADIUS_OVER_TIME)
		)
		return TTYPE_DESTINATION;
	if( 
		HasTargetType(sp,EFF_TARGET_LOCATION_TO_SUMMON) ||
		HasTargetType(sp,EFF_TARGET_IN_FRONT_OF_CASTER) ||
		HasTargetType(sp,EFF_TARGET_ALL_FRIENDLY_IN_AREA) ||
		HasTargetType(sp,EFF_TARGET_PET_SUMMON_LOCATION) ||
		HasTargetType(sp,EFF_TARGET_LOCATION_INFRONT_CASTER)
		)
		return TTYPE_SOURCE;
	if( 
		HasTargetType(sp,EFF_TARGET_SINGLE_ENEMY) ||
		HasTargetType(sp,EFF_TARGET_ALL_ENEMIES_AROUND_CASTER) ||
		HasTargetType(sp,EFF_TARGET_DUEL) ||
		HasTargetType(sp,EFF_TARGET_SCRIPTED_OR_SINGLE_TARGET) ||
		HasTargetType(sp,EFF_TARGET_CHAIN) ||
		HasTargetType(sp,EFF_TARGET_CURRENT_SELECTION) ||
		HasTargetType(sp,EFF_TARGET_TARGET_AT_ORIENTATION_TO_CASTER) ||
		HasTargetType(sp,EFF_TARGET_MULTIPLE_GUARDIAN_SUMMON_LOCATION) ||
		HasTargetType(sp,EFF_TARGET_SELECTED_ENEMY_CHANNELED)
		)
		return TTYPE_SINGLETARGET;
	if( 
		HasTargetType(sp,EFF_TARGET_ALL_PARTY_AROUND_CASTER) ||
		HasTargetType(sp,EFF_TARGET_SINGLE_FRIEND) ||
		HasTargetType(sp,EFF_TARGET_PET_MASTER) ||
		HasTargetType(sp,EFF_TARGET_ALL_ENEMY_AT_ENEMY_IN_AREA_CHANNELED) ||
		HasTargetType(sp,EFF_TARGET_ALL_PARTY_IN_AREA) ||
		HasTargetType(sp,EFF_TARGET_SINGLE_PARTY) ||
		HasTargetType(sp,EFF_TARGET_ALL_PARTY) ||
		HasTargetType(sp,EFF_TARGET_ALL_RAID) ||
		HasTargetType(sp,EFF_TARGET_PARTY_MEMBER) ||
		HasTargetType(sp,EFF_TARGET_AREAEFFECT_PARTY_AND_CLASS)
		)
		return TTYPE_OWNER;
	if( 
		HasTargetType(sp,EFF_TARGET_SELF) ||
		HasTargetType(sp,4) ||
		HasTargetType(sp,EFF_TARGET_PET) ||
		HasTargetType(sp,EFF_TARGET_MINION)
		)
		return TTYPE_CASTER;
	return TTYPE_NULL;
}

bool IsTargetingStealthed(SpellEntry *sp)
{
	if(
		sp->NameHash == SPELL_HASH_CHARGE_STUN
		)
		return 0;
	if(
		HasTargetType(sp,EFF_TARGET_INVISIBLE_OR_HIDDEN_ENEMIES_AT_LOCATION_RADIUS) ||
		HasTargetType(sp,EFF_TARGET_ALL_ENEMIES_AROUND_CASTER) ||
		HasTargetType(sp,EFF_TARGET_ALL_ENEMY_IN_AREA_CHANNELED) ||
		HasTargetType(sp,EFF_TARGET_ALL_ENEMY_IN_AREA_INSTANT)
		)
		return 1;

	if(
		sp->NameHash == SPELL_HASH_MAGMA_TOTEM
		)
		return 1;

	return 0;
}

uint32 IsHealingSpell(SpellEntry *sp)
{
	if( IsHealingEffect( sp, 0 ) )
		return 1;
	if( IsHealingEffect( sp, 1 ) )
		return 2;
	if( IsHealingEffect( sp, 2 ) )
		return 3;
	//flash of light, holy light uses scripted effect which is not neceserally heal spell
	if( sp->NameHash == SPELL_HASH_HOLY_LIGHT || sp->NameHash == SPELL_HASH_FLASH_OF_LIGHT  )
		return 1;
	
    return 0;
}

uint32 IsEnergizingSpell(SpellEntry *sp)
{
	if( IsEnergizingEffect( sp, 0 ) )
		return 1;
	if( IsEnergizingEffect( sp, 1 ) )
		return 2;
	if( IsEnergizingEffect( sp, 2 ) )
		return 3;
	
    return 0;
}

bool CanAgroHash(uint32 spellhashname)
{
    if (spellhashname == SPELL_HASH_HUNTER_S_MARK //hunter's mark
		|| spellhashname == SPELL_HASH_CURSE_OF_THE_ELEMENTS )
        return false;
    else
        return true;
}
/*
void Spell::SendCastSuccess(Object * target)
{
	Player * plr = p_caster;
	if(!plr && u_caster)
		plr = u_caster->m_redirectSpellPackets;
	if(!plr||!plr->IsPlayer())
	{ 
		return;
	}

/*	WorldPacket data(SMSG_CLEAR_EXTRA_AURA_INFO, 13);
	data << ((target != 0) ? target->GetNewGUID() : uint64(0));
	data << GetProto()->Id;

	plr->GetSession()->SendPacket(&data);*/
/*
}

void Spell::SendCastSuccess(const uint64& guid)
{
	Player * plr = p_caster;
	if(!plr && u_caster)
		plr = u_caster->m_redirectSpellPackets;
	if(!plr || !plr->IsPlayer())
	{ 
		return;
	}

	// fuck bytebuffers
	unsigned char buffer[13];
	uint32 c = FastGUIDPack(guid, buffer, 0);
#ifdef USING_BIG_ENDIAN
	*(uint32*)&buffer[c] = swap32(GetProto()->Id);         c += 4;
#else
	*(uint32*)&buffer[c] = GetProto()->Id;                 c += 4;
#endif

//	plr->GetSession()->OutPacket(SMSG_CLEAR_EXTRA_AURA_INFO, c, buffer);
}*/
/*
bool IsBeneficSpell(SpellEntry *sp)
{
	uint32 cur;
	for(uint32 i=0;i<3;i++)
		for(uint32 j=0;j<2;j++)
		{
			if(j==0)
				cur = sp->eff[i].EffectImplicitTargetA;
			else // if(j==1)
				cur = sp->eff[i].EffectImplicitTargetB;
			switch(cur)
			{
				case EFF_TARGET_SELF:
				case EFF_TARGET_PET:
				case EFF_TARGET_ALL_PARTY_AROUND_CASTER:
				case EFF_TARGET_SINGLE_FRIEND:
				case 45:// Chain,!!only for healing!! for chain lightning =6
				case 57:// Targeted Party Member
				case 27: // target is owner of pet
				case EFF_TARGET_MINION:// Minion Target
				case 33://Party members of totem, inside given range
				case EFF_TARGET_SINGLE_PARTY:// Single Target Party Member
				case EFF_TARGET_ALL_PARTY: // all Members of the targets party
				case EFF_TARGET_SELF_FISHING://Fishing
				case 46://Unknown Summon Atal'ai Skeleton
				case 47:// Portal
				case 52:	// Lightwells, etc
				case 40://Activate Object target(probably based on focus)
				case EFF_TARGET_TOTEM_EARTH:
				case EFF_TARGET_TOTEM_WATER:
				case EFF_TARGET_TOTEM_AIR:
				case EFF_TARGET_TOTEM_FIRE:// Totem
				case 61: // targets with the same group/raid and the same class
				case 32:
				case 73:
					return true;
			}//end switch
		}//end for
	return false;
}

AI_SpellTargetType RecommandAISpellTargetType(SpellEntry *sp)
{
	uint32 cur;
	for(uint32 i=0;i<3;i++)
		for(uint32 j=0;j<2;j++)
		{
			if(j==0)
				cur = sp->eff[i].EffectImplicitTargetA;
			else // if(j==1)
				cur = sp->eff[i].EffectImplicitTargetB;
			switch(cur)
			{
				case EFF_TARGET_NONE:
				case EFF_TARGET_GAMEOBJECT:
				case EFF_TARGET_GAMEOBJECT_ITEM:// Gameobject/Item Target
				case EFF_TARGET_SELF_FISHING://Fishing
				case 47:// Portal
				case 52:	// Lightwells, etc
				case 40://Activate Object target(probably based on focus)
					return TTYPE_NULL;

				case EFF_TARGET_SELF:
				case 38://Dummy Target
				case 32:
				case 73:
					return TTYPE_CASTER;

				case EFF_TARGET_ALL_ENEMY_IN_AREA: // All Enemies in Area of Effect (TEST)
				case EFF_TARGET_ALL_ENEMY_IN_AREA_INSTANT: // All Enemies in Area of Effect instant (e.g. Flamestrike)
				case EFF_TARGET_ALL_ENEMY_IN_AREA_CHANNELED:// All Enemies in Area of Effect(Blizzard/Rain of Fire/volley) channeled
					return TTYPE_DESTINATION;

				case 4: // dono related to "Wandering Plague", "Spirit Steal", "Contagion of Rot", "Retching Plague" and "Copy of Wandering Plague"
				case EFF_TARGET_PET:
				case EFF_TARGET_SINGLE_ENEMY:// Single Target Enemy
				case 77:					// grep: i think this fits
				case 8: // related to Chess Move (DND), Firecrackers, Spotlight, aedm, Spice Mortar
				case EFF_TARGET_IN_FRONT_OF_CASTER:
				case 31:// related to scripted effects
				case 53:// Target Area by Players CurrentSelection()
				case 54:// Targets in Front of the Caster
				case EFF_TARGET_ALL_PARTY_AROUND_CASTER:
				case EFF_TARGET_SINGLE_FRIEND:
				case 45:// Chain,!!only for healing!! for chain lightning =6
				case 57:// Targeted Party Member
				case EFF_TARGET_DUEL:
				case 27: // target is owner of pet
				case EFF_TARGET_MINION:// Minion Target
				case 33://Party members of totem, inside given range
				case EFF_TARGET_SINGLE_PARTY:// Single Target Party Member
				case EFF_TARGET_ALL_PARTY: // all Members of the targets party
				case EFF_TARGET_TOTEM_EARTH:
				case EFF_TARGET_TOTEM_WATER:
				case EFF_TARGET_TOTEM_AIR:
				case EFF_TARGET_TOTEM_FIRE:// Totem
				case 61: // targets with the same group/raid and the same class
					return TTYPE_SINGLETARGET;

					// spells like 17278:Cannon Fire and 21117:Summon Son of Flame A
				case 17: // A single target at a xyz location or the target is a possition xyz
				case 18:// Land under caster.Maybe not correct
				case 46://Unknown Summon Atal'ai Skeleton
				case EFF_TARGET_ALL_ENEMIES_AROUND_CASTER:
					return TTYPE_SOURCE;

			}//end switch
		}//end for
	return TTYPE_NULL;// this means a new spell :P
}
*/

//amplitude or tick frequency/interval
int32 GetSpellAmplitude( SpellEntry *sp, Unit *Unitc, int32 i, uint32 ItemCast )
{
	int32 period = sp->eff[i].EffectAmplitude;
	if( Unitc != NULL && ItemCast == 0 )
	{
		if( Unitc->SM_Mods )
			SM_FIValue(Unitc->SM_Mods->SM_FTime,&period,sp->GetSpellGroupType());
		period = (int32)(period * Unitc->GetSpellHasteDots() );
	}
	//some spells tick under 200 in 4.3.0 client :(
	if( period < 100 )
		period = 100;
	return period;
}

void SetItemSpellProcInterval( SpellEntry *sp, int32 LoopCounter, int32 *OwnerInterval, bool IsTrinket )
{
	if( LoopCounter > 4 )
		return;
	if( sp == NULL )
		return;

	//default PPM will be set by : void ApplyNormalFixes()
	int32 DefaultPPM;
//	if( sp->procChance < 100 && sp->procChance > 0 )
//		DefaultPPM = MAX( DEFAULT_SPELL_PROC_INTERVAL, 60000 * ( 100 - sp->procChance ) / 100 ); //10% proc chance ?
//	else
		DefaultPPM = DEFAULT_SPELL_PROC_INTERVAL;//trigger at each event but no spams pls

	//first spells are loaded then item
	if( sp->proc_interval == DefaultPPM )
	{
		int32 MinInterval = 0; //either 100% duration of the buff or 50% if it can stack
		int32 Duration = DEFAULT_SPELL_ITEM_PROC_INTERVAL;
		if( sp->DurationIndex != 0 && sp->DurationIndex != 21 )
			Duration = GetDuration( dbcSpellDuration.LookupEntry( sp->DurationIndex ) );
		Duration = MAX( DEFAULT_SPELL_ITEM_PROC_INTERVAL, Duration );

		int32 ChildDuration = Duration;
		for(int i=0;i<MAX_SPELL_EFFECT_COUNT;i++)
			if( sp->eff[i].EffectTriggerSpell )
			{
				SpellEntry *sp2 = dbcSpell.LookupEntryForced( sp->eff[i].EffectTriggerSpell );
				SetItemSpellProcInterval( sp2, LoopCounter + 1, &ChildDuration, IsTrinket );
			}
		Duration = MAX( ChildDuration, Duration );

		//this is generic rule for trinkets
		if( IsTrinket == true )
		{
			MinInterval = Duration * 5;
		}
		else if( sp->procChance >= 100 )
		{
			MinInterval = Duration + Duration * 5 / 100 + DEFAULT_SPELL_ITEM_PROC_INTERVAL;
		}
		else
		{
			int32 ExpectedPPMCooldown = 60000 * ( 100 - sp->procChance ) / 100;
			MinInterval = MAX( Duration, ExpectedPPMCooldown );
			//give a slight chance to actually refresh the aura : 50% chance in that 25% of duration time
			MinInterval = MAX( MinInterval * 50 / 100, DEFAULT_SPELL_ITEM_PROC_INTERVAL );
//			MinInterval = MAX( MinInterval * 75 / 100, DEFAULT_SPELL_ITEM_PROC_INTERVAL );
//			MinInterval = MAX( MinInterval * 100 / 100, DEFAULT_SPELL_ITEM_PROC_INTERVAL );
		}

		//let those that can stack stack
		if( sp->maxstack > 1 )
			MinInterval = MIN( Duration / 3 , MinInterval );

		sp->proc_interval = MinInterval;
		if( OwnerInterval )
			*OwnerInterval = Duration;
	}
	else
	{
		if( OwnerInterval != NULL && sp->DurationIndex != 0 && sp->DurationIndex != 21 )
			*OwnerInterval = GetDuration( dbcSpellDuration.LookupEntry( sp->DurationIndex ) );
	}
}

void SetItemSpellSPCoeff( SpellEntry *sp, int32 LoopCounter )
{
	if( LoopCounter > 4 )
		return;
	if( sp == NULL )
		return;
	for(uint32 ti = 0;ti<MAX_SPELL_EFFECT_COUNT; ti++)
		if( sp->eff[ti].EffectSpellPowerCoef == SPELL_EFFECT_MAGIC_UNDECIDED_SPELL_POWER_COEFF )
			sp->eff[ti].EffectSpellPowerCoef = ITEM_SPELL_DEFAULT_SP_BENEFIT_COEF;	
	for(int i=0;i<MAX_SPELL_EFFECT_COUNT;i++)
		if( sp->eff[i].EffectTriggerSpell )
		{
			SpellEntry *sp2 = dbcSpell.LookupEntryForced( sp->eff[i].EffectTriggerSpell );
			SetItemSpellSPCoeff( sp2, LoopCounter + 1 );
		}
}

void GetSpellLinkByProto( uint32 SpellID, char *buffer )
{
	SpellEntry * sp = dbcSpell.LookupEntryForced( SpellID );
	if( sp == NULL )
	{
		sprintf( buffer, "" );
		return;
	}
	snprintf(buffer, 256, " |cff71d5ff|Hspell:%d|h[%s]|h|r ", sp->spell_id_client, sp->Name );
}

void SendInterruptLog( Object *Caster, uint32 CastedSpellID, Object *Target, uint32 InterruptedSpell)
{
	if( Caster == NULL || Target == NULL )
		return;
	sStackWorldPacket(data, SMSG_SPELLLOGEXECUTE, 40 + 10 );
	data << Caster->GetNewGUID();
	data << CastedSpellID;
	data << uint32( 1 );	//number of effects
	data << uint32( SPELL_EFFECT_INTERRUPT_CAST ); //GetProto()->eff[i].Effect
	data << Target->GetNewGUID();
	data << InterruptedSpell;
//	p_caster->SendMessageToSet( data );
//	if( p_caster->GetSession() )
//		p_caster->GetSession()->SendPacket( &data );
	//not sure this is required
	if( Target->IsPlayer() && SafePlayerCast( Target )->GetSession() )
		SafePlayerCast( Target )->GetSession()->SendPacket( &data );
}
void SendPowerStealLog( Player *p_caster, uint32 CastedSpellID, Object *Target, uint32 PowerAmt, uint32 PowerType,float Mult)
{
/*    m_effectExecuteData[effIndex]->append(target->GetPackGUID());
    *m_effectExecuteData[effIndex] << uint32(powerTaken);
    *m_effectExecuteData[effIndex] << uint32(powerType);
    *m_effectExecuteData[effIndex] << float(gainMultiplier); */
}
void SendGainExtraAttacksLog( Player *p_caster, uint32 CastedSpellID, Object *Target, uint32 Count)
{
/*   m_effectExecuteData[effIndex]->append(victim->GetPackGUID());
    *m_effectExecuteData[effIndex] << uint32(attCount);*/
}
void SendDurabilityDMGLog( Player *p_caster, uint32 CastedSpellID, Object *Target, uint32 ItemID, uint32 Slot)
{
/*    m_effectExecuteData[effIndex]->append(victim->GetPackGUID());
    *m_effectExecuteData[effIndex] << int32(itemId);
    *m_effectExecuteData[effIndex] << int32(slot);*/
}
void SendOpenLockLog( Player *p_caster, uint32 CastedSpellID, Object *Target)
{
//    m_effectExecuteData[effIndex]->append(obj->GetPackGUID());
}
void SendCreateItemLog( Player *p_caster, uint32 CastedSpellID, uint32 ItemID)
{
//    *m_effectExecuteData[effIndex] << uint32(entry);
}
void SendDestroyItemLog( Player *p_caster, uint32 CastedSpellID, uint32 ItemID)
{
//    *m_effectExecuteData[effIndex] << uint32(entry);
}
void SendSummonLog( Player *p_caster, uint32 CastedSpellID, Object *Target, uint32 Count)
{
/*
{SERVER} Packet: (0xE223) SMSG_SPELLLOGEXECUTE PacketSize = 30 TimeStamp = 22936312
8F 0A BA 62 04 02 - guid
71 A6 00 00 - spell 42609  Sinister Squashling
01 00 00 00 - count
1C 00 00 00 - effect 28 - summon
01 00 00 00 - execute data 1
F7 3E 0C 07 65 5D 30 F1 - execute data 2
*/
	if( p_caster == NULL )
		return;
	sStackWorldPacket(data, SMSG_SPELLLOGEXECUTE, 40 + 10 );
	data << p_caster->GetNewGUID();
	data << CastedSpellID;
	data << uint32( 1 );	//number of effects
	data << uint32( SPELL_EFFECT_SUMMON ); //GetProto()->eff[i].Effect
	data << Count;
	data << Target->GetNewGUID();
//	p_caster->SendMessageToSet( data );
	if( p_caster->GetSession() )
		p_caster->GetSession()->SendPacket( &data );
}
void SendUnSummonLog( Player *p_caster, uint32 CastedSpellID, Object *Target, uint32 Count)
{
//    m_effectExecuteData[effIndex]->append(obj->GetPackGUID());
}
void SendResurrectLog( Player *p_caster, uint32 CastedSpellID, Object *Target)
{
//    m_effectExecuteData[effIndex]->append(obj->GetPackGUID());
}
