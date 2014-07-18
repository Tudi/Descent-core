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

DynamicObject::DynamicObject(uint32 high, uint32 low)
{
	m_objectTypeId = TYPEID_DYNAMICOBJECT;
	internal_object_type = INTERNAL_OBJECT_TYPE_DYNOBJECT;
	m_valuesCount = DYNAMICOBJECT_END;
	m_uint32Values = _fields;
	memset(m_uint32Values, 0,(DYNAMICOBJECT_END)*sizeof(uint32));
	m_updateMask.SetCount(DYNAMICOBJECT_END);
	m_uint32Values[OBJECT_FIELD_TYPE] = TYPE_DYNAMICOBJECT|TYPE_OBJECT;
	m_uint32Values[OBJECT_FIELD_GUID] = low;
	m_uint32Values[OBJECT_FIELD_GUID+1] = high;
	m_wowGuid.Init(GetGUID());
	m_floatValues[OBJECT_FIELD_SCALE_X] = 1;


//	m_parentSpell=NULL;
	m_Expire_At_Stamp = 0;
//	m_Arm_At_Stamp = 0;
//	u_caster = 0;
	m_spellProto = 0;
//	p_caster = 0;
//	m_duration = 0;
	invisible =  false;
}

DynamicObject::~DynamicObject()
{
	Virtual_Destructor( );
}

void DynamicObject::Virtual_Destructor( )
{
//	Unit *caster = u_caster;
	//who said our caster pointer is ok ?
//	if( GetMapMgr() )
//		caster = GetMapMgr()->GetUnit( GetUInt64Value(DYNAMICOBJECT_CASTER) );
//	if(caster && caster->dynObj == this)
//		caster->dynObj = NULL;
//	u_caster = NULL;
	Object::Virtual_Destructor();
	m_objectTypeId = TYPEID_UNUSED;
	internal_object_type = INTERNAL_OBJECT_TYPE_NONE;
}

void DynamicObject::Create(Unit * caster, Spell * pSpell, float x, float y, float z, uint32 duration, float radius,int32 spell_val,int32 update_interval)
{
	INSTRUMENT_TYPECAST_CHECK_DYNO_OBJECT_TYPE
	Object::_Create(caster->GetMapId(),x, y, z, 0);
//	if(pSpell->g_caster)
//		m_parentSpell = pSpell;
//	if( pSpell->p_caster == NULL )
	{
		// try to find player caster here
//		if( caster->IsPlayer() )
//			p_caster = SafePlayerCast( caster );
	}
//	else
//		p_caster = pSpell->p_caster;

	m_spellProto = pSpell->GetProto();
	SetUInt64Value(DYNAMICOBJECT_CASTER, caster->GetGUID());

	m_uint32Values[OBJECT_FIELD_ENTRY] = m_spellProto->spell_id_client;
	m_uint32Values[DYNAMICOBJECT_BYTES] = 0x30000000 | pSpell->GetProto()->SpellVisual[0];
	m_uint32Values[DYNAMICOBJECT_SPELLID] = m_spellProto->spell_id_client;

	m_floatValues[DYNAMICOBJECT_RADIUS] = radius;
//	m_floatValues[DYNAMICOBJECT_POS_X]  = x;
//	m_floatValues[DYNAMICOBJECT_POS_Y]  = y;
//	m_floatValues[DYNAMICOBJECT_POS_Z]  = z;
	SetFloatValue( OBJECT_FIELD_SCALE_X, radius / 7.0f );	//no idea actually, but consacration spell had too small visual size 

//	m_duration = duration;
	m_Expire_At_Stamp = getMSTime() + duration;
//	u_caster = caster;
	m_faction = caster->m_faction;
	m_factionDBC = caster->m_factionDBC;

	if( pSpell->m_caster->IsGameObject() )
		PushToWorld( pSpell->m_caster->GetMapMgr() );
	else 
		PushToWorld(caster->GetMapMgr());

//	if(caster->dynObj != NULL)
	{
		//expires
//		caster->dynObj->Remove();
	}
//	caster->dynObj = this;

	spell_value = spell_val;
  
	sEventMgr.AddEvent(this, &DynamicObject::UpdateTargets, EVENT_DYNAMICOBJECT_UPDATE, MAX( update_interval, DYN_OBJ_UPDATE_INTERVAL ), 0,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
}
/*
void DynamicObject::AddInRangeObject( Object* pObj )
{
	INSTRUMENT_TYPECAST_CHECK_DYNO_OBJECT_TYPE
	Object::AddInRangeObject( pObj );
}*/

void DynamicObject::OnRemoveInRangeObject( Object* pObj )
{
	INSTRUMENT_TYPECAST_CHECK_DYNO_OBJECT_TYPE
	if( pObj->IsUnit() )
	{
		targets.erase( pObj->GetGUID() );
	}
	Object::QueueRemoveInRangeObject( pObj );
}

void DynamicObject::UpdateTargets()
{
	INSTRUMENT_TYPECAST_CHECK_DYNO_OBJECT_TYPE
//	if( m_Arm_At_Stamp > getMSTime() )
	{
//		return;
	}
	if( m_Expire_At_Stamp < getMSTime() 
		|| GetMapMgr() == NULL
		)
	{ 
		Remove();
		return;
	}
	Unit *u_caster = GetMapMgr()->GetUnit( GetUInt64Value(DYNAMICOBJECT_CASTER) );
	if( u_caster == NULL || u_caster->GetMapMgr() != GetMapMgr() )
	{
		Remove();
		return;
	}

	int32 m_duration = m_Expire_At_Stamp - getMSTime();
	if( m_duration < 0 )
		m_duration = -1;

	//there are marker dyns that simply hang arund
	bool ads_aura = false;
	for(uint32 i = 0; i < MAX_SPELL_EFFECT_COUNT; ++i)
		if(m_spellProto->eff[i].Effect == SPELL_EFFECT_PERSISTENT_AREA_AURA)
			ads_aura = true;

	if( ads_aura == true )
	{
		InrangeLoopExitAutoCallback AutoLock;
		InRangeSetRecProt::iterator itr = GetInRangeSetBegin( AutoLock ),itr2;
		InRangeSetRecProt::iterator iend = GetInRangeSetEnd();
		Unit * target;
		Aura * pAura;
		float radius = m_floatValues[DYNAMICOBJECT_RADIUS]*m_floatValues[DYNAMICOBJECT_RADIUS];

		this->AquireInrangeLock(); //make sure to release lock before exit function !
		while(itr != iend)
		{
//			target = *itr;
//			++itr;

			itr2 = itr;
			++itr;

			if( !( (*itr2)->IsUnit() ) 
					|| ! SafeUnitCast( *itr2 )->isAlive() 
					|| ( SafeUnitCast( *itr2 )->IsCreature() && SafeCreatureCast( *itr2 )->IsTotem()  )
					)
				continue;

			target = SafeUnitCast( *itr2 );

			if( GetDistanceSq(target) > radius )
				continue;

			if( ( m_spellProto->c_is_flags2 & SPELL_FLAG2_IS_AA_TARGETTING_EVERYONE ) == 0 )
			{
				if( ( m_spellProto->c_is_flags & SPELL_FLAG_IS_AA_TARGETTING_FRIENDLY ) )
				{
					if( isAttackable( u_caster, target, !(m_spellProto->c_is_flags & SPELL_FLAG_IS_TARGETINGSTEALTHED) == true ) )
					continue;
				}
				else if( isAttackable( u_caster, target, !(m_spellProto->c_is_flags & SPELL_FLAG_IS_TARGETINGSTEALTHED) ) == false )
					continue;
			}

			if( ( m_spellProto->c_is_flags2 & SPELL_FLAG2_IS_AA_CALLING_SCRIPT ) && m_spellProto->ProcHandler != NULL )
			{
				ProcHandlerContextShare context;
				memset( &context, 0, sizeof( context ) );
				context.in_Caller = u_caster;
				context.in_Victim = target;
				context.in_CastingSpell = m_spellProto;
				context.in_OwnerSpell = m_spellProto;
				context.in_dmg = spell_value;
				context.in_event = NULL;
				m_spellProto->ProcHandler( &context );
				if( context.out_handler_result == PROC_HANDLER_CONTINUE_TO_NEXT )
					continue;
			}

			// skip units already hit, their range will be tested later
			if(targets.find(target->GetGUID()) != targets.end())
				continue;

			pAura = AuraPool.PooledNew( __FILE__, __LINE__ );
			pAura->Init(m_spellProto, m_duration, u_caster, target);
//			pAura->m_CasterDyn = this->GetGUID();
			bool FirstEffectHasValue = true;
			for(uint32 i = 0; i < MAX_SPELL_EFFECT_COUNT; ++i)
			{
				if(m_spellProto->eff[i].Effect == SPELL_EFFECT_PERSISTENT_AREA_AURA)
				{
					if( FirstEffectHasValue == true )
					{
						pAura->AddMod(m_spellProto->eff[i].EffectApplyAuraName,	spell_value, m_spellProto->eff[i].EffectMiscValue, i, 100);
						FirstEffectHasValue = false;
					}
					else
					{
						//not many have more then 1 persistent aura effect on same Dyn
						SpellCastTargets targets( u_caster->GetGUID() );
						Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
						spell->Init( u_caster, m_spellProto ,true, NULL);
						int32 val = spell->CalculateEffect( i, target );
						SpellPool.PooledDelete( spell, __FILE__, __LINE__ );
//						pAura->AddMod(m_spellProto->eff[i].EffectApplyAuraName,	m_spellProto->eff[i].EffectBasePoints+1, m_spellProto->eff[i].EffectMiscValue, i);
						pAura->AddMod(m_spellProto->eff[i].EffectApplyAuraName,	val, m_spellProto->eff[i].EffectMiscValue, i, 100 );
					}
				}
			}
			target->AddAura(pAura);
			u_caster->HandleProc( PROC_ON_CAST_SPELL,target, m_spellProto);

			//set caster PVP flag if the target has PVP flag
			if( target->IsPvPFlagged() )
				u_caster->SetPvPFlag();

			// add to target list
			targets.insert(target->GetGUID());
		}

		this->ReleaseInrangeLock();
		// loop the targets, check the range of all of them
		DynamicObjectList::iterator jtr  = targets.begin();
		DynamicObjectList::iterator jtr2;
		DynamicObjectList::iterator jend = targets.end();
		
		while(jtr != jend)
		{
			target = GetMapMgr() ? GetMapMgr()->GetUnit(*jtr) : NULL;
			jtr2 = jtr;
			++jtr;

			if( target == NULL )
				targets.erase(jtr2);
			else if(GetDistanceSq(target) > radius)
			{
				target->RemoveAura(m_spellProto->Id);
				targets.erase(jtr2);
			}
		}

	}
}

void DynamicObject::Remove()
{
	INSTRUMENT_TYPECAST_CHECK_DYNO_OBJECT_TYPE
	// remove aura from all targets
	DynamicObjectList::iterator jtr  = targets.begin();
	DynamicObjectList::iterator jend = targets.end();
	Unit * target;

	while(jtr != jend)
	{
		target = GetMapMgr() ? GetMapMgr()->GetUnit(*jtr) : NULL;
		++jtr;
		if (target != NULL)
			target->RemoveAura(m_spellProto->Id);
	}

	if(IsInWorld())
		RemoveFromWorld(true);

	sGarbageCollection.AddObject( this );
}

Object *DynamicObject::GetTopOwner()
{
	Object *topA = this;
	//this is made with loop because. Ex : totems that spawn mobs -> mob -> totem -> owner
	while( topA && topA->GetUInt32Value( UNIT_FIELD_CREATEDBY ) && topA->GetMapMgr() )
	{
		Unit *ttopA = topA->GetMapMgr()->GetUnit( topA->GetUInt64Value( UNIT_FIELD_CREATEDBY ) );
		if( ttopA == topA || ttopA == NULL )
			break;	//someone managed to self create. Met god yet ?
		topA = ttopA;
	}
	return topA;
}

