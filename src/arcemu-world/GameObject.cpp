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
GameObject::GameObject(uint64 guid)
{
	m_objectTypeId = TYPEID_GAMEOBJECT;
	internal_object_type = INTERNAL_OBJECT_TYPE_GAMEOBJECT;
	m_valuesCount = GAMEOBJECT_END;
	m_uint32Values = _fields;
	memset(m_uint32Values, 0,(GAMEOBJECT_END)*sizeof(uint32));
	m_updateMask.SetCount(GAMEOBJECT_END);
	SetUInt32Value( OBJECT_FIELD_TYPE,TYPE_GAMEOBJECT|TYPE_OBJECT);
	SetUInt64Value( OBJECT_FIELD_GUID,guid);
	m_wowGuid.Init(GetGUID());
 
	SetFloatValue( OBJECT_FIELD_SCALE_X, 1);//info->Size  );

	counter=0;//not needed at all but to prevent errors that var was not inited, can be removed in release

	bannerslot = bannerauraslot = -1;

	invisible = false;
	invisibilityFlag = INVIS_FLAG_NORMAL;
	spell = 0;
	charges = ChargesInitial = -1;
	m_ritualcaster = 0;
	m_ritualtarget = 0;
	m_ritualmembers = NULL;
	m_ritualspell = 0;

	m_quests = NULL;
	pInfo = NULL;
	myScript = NULL;
	m_spawn = 0;
	loot.gold = 0;
	m_deleted = false;
//	usage_remaining = 1;
	m_respawnCell=NULL;
	m_battleground = NULL;
//	SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_ANIMPROGRESS, 100); // spawn dors as closed ?
//	SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_ANIMPROGRESS, 0xFF); // mostly used for 3.1.1 client. But why ?
	SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, GO_STATE_READY); // mostly used for 3.1.1 client. But why ?
	m_rotationX = 0;
	m_noRespawn = true;
	checkrate = 0xFF;	//this value is only valid if we have a spell. In case we have a spell we will overwrite this value
	m_Arm_At_Stamp = 0;
}


GameObject::~GameObject()
{
	Virtual_Destructor();
}

void GameObject::Virtual_Destructor()
{
	ASSERT( m_noRespawn == true );
	if(myScript != NULL)
	{
		myScript->Destroy();	 //deletes script !
		myScript = NULL;
	}
	sEventMgr.RemoveEvents(this);
	if(m_ritualmembers)
	{
		for( uint32 i=0;i<pInfo->SpellFocus;i++)
			if( m_ritualmembers[i] )
			{
				Player *p = objmgr.GetPlayer( m_ritualmembers[i] );
				if( p && p->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT ) == this->GetGUID() )
				{
					p->SetUInt64Value( UNIT_FIELD_CHANNEL_OBJECT, 0 );
					p->SetUInt32Value( UNIT_CHANNEL_SPELL, 0 );
				}
			}
		delete[] m_ritualmembers;
		m_ritualmembers = NULL;
	}

	uint32 guid = GetUInt32Value(OBJECT_FIELD_CREATED_BY);
	if(guid)
	{
		Player *plr = objmgr.GetPlayer(guid);
		if(plr && plr->GetSummonedObject() == this)
			plr->SetSummonedObject(NULL);
	}

	if( m_respawnCell!=NULL && m_noRespawn == false )
	{
		m_respawnCell->_respawnObjects.erase(this);
		m_respawnCell = NULL;
	}/**/

	if( m_battleground != NULL && m_battleground->GetType() == BATTLEGROUND_ARATHI_BASIN )
	{
		if( bannerslot >= 0 && static_cast<ArathiBasin*>(m_battleground)->m_controlPoints[bannerslot] == this )
			static_cast<ArathiBasin*>(m_battleground)->m_controlPoints[bannerslot] = NULL;

		if( bannerauraslot >= 0 && static_cast<ArathiBasin*>(m_battleground)->m_controlPointAuras[bannerauraslot] == this )
			static_cast<ArathiBasin*>(m_battleground)->m_controlPointAuras[bannerauraslot] = NULL;
		m_battleground = NULL;
	}
	Object::Virtual_Destructor();
	m_objectTypeId = TYPEID_UNUSED;
	internal_object_type = INTERNAL_OBJECT_TYPE_NONE;
}

bool GameObject::CreateFromProto(uint32 entry,uint32 mapid, float x, float y, float z, float ang, float r0, float r1, float r2, float r3)
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
	pInfo= GameObjectNameStorage.LookupEntry(entry);
	if(!pInfo)
	{
		return false;
	}

	Object::_Create( mapid, x, y, z, ang );
	SetUInt32Value( OBJECT_FIELD_ENTRY, entry );
	
	SetPosition(x, y, z, ang);
	SetParentRotation(0, r0);
	SetParentRotation(1, r1);
	SetParentRotation(2, r2);
	SetParentRotation(3, r3);

	if( m_spawn )
		m_rotationX = m_spawn->precise_facing;	//this was provided by a trusted source and we will trust it blindly over our crappy formula

	UpdateGoRotationXFromOrientation();

	if( pInfo->Type == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING )
	{
	    SetByte( GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_ANIMPROGRESS, 255 );
		SetByte( GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, GO_DESTRUCTIBLE_INTACT );
	}
	else
	{
	    SetByte( GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_ANIMPROGRESS, 0 );
		SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, GO_STATE_READY );
	}
	SetUInt32Value( GAMEOBJECT_DISPLAYID, pInfo->DisplayID );
	SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_TYPEID, pInfo->Type ); 
	SetFloatValue( OBJECT_FIELD_SCALE_X, pInfo->Scale );
   
	InitAI();

	 return true;
}

void GameObject::TrapSearchTarget()
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
	Update(100);
}

void GameObject::ConvertToCaster( uint32 SpellId, float Radius, uint32 CheckRate,int32 Charges, int32 RespawnTime)
{
	if( SpellId == 0 )
		return;
	SpellEntry *sp = dbcSpell.LookupEntryForced( SpellId );
	if( sp == NULL || sp->Id == 1 )
		return;
	spell = sp;
	if( Radius < 0 )
	{
		float r = 0;
		for(uint32 i=0;i<MAX_SPELL_EFFECT_COUNT;i++)
			if(sp->eff[i].Effect)
			{
				float t = GetRadius(dbcSpellRadius.LookupEntry(sp->eff[i].EffectRadiusIndex));
				if(t > r)
					r = t;
			}
		/*
		if(r < 0.1)//no range
			r = GetMaxRange(dbcSpellRange.LookupEntry(sp->rangeIndex));
		if( pInfo->Type==GAMEOBJECT_TYPE_TRAP )
			r = MAX( r, GetMaxRange(dbcSpellRange.LookupEntry(sp->rangeIndex)) );
			*/
		if( r < 2.0f )
			r = 2.0f;
		rangeSQ = r*r;//square to make code faster
	}
	else
		rangeSQ = Radius * Radius;

	if( CheckRate <= 0 )
		checkrate = 20;//once in 2 seconds
	else
		checkrate = CheckRate;

	charges = ChargesInitial = Charges;

	if( RespawnTime > 0 )
	{
//		m_noRespawn = false;
		RespawnCaster = RespawnTime;
	}

}

void GameObject::Update(uint32 p_time)
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE

	Object::Update( p_time );	//99% this does nothing

	if(!IsInWorld())
	{ 
		return;
	}

	if(m_deleted)
	{ 
		return;
	}

	if( m_Arm_At_Stamp > getMSTime() )
	{
		return;
	}

	//there is a chance that owner logs out and this kinda produces a bad pointer check
	Unit *m_summoner = NULL;
	bool m_summonedGo = false;
	if( GetUInt64Value( OBJECT_FIELD_CREATED_BY ) )
	{
		m_summonedGo = true;
		m_summoner = GetMapMgr()->GetUnit( GetUInt64Value( OBJECT_FIELD_CREATED_BY ) );
		if( !m_summoner || m_summoner->GetMapMgr() != GetMapMgr() )
		{
			ExpireAndDelete();
			return;
		}
	}

	if(spell && (GetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE) == GO_STATE_READY) && charges != 0 )
	{
		if(checkrate > 1)
		{
			counter++;
			if(counter < checkrate)
			{ 
				return;
			}
			counter = 0;
		}
		float BestDistance = 999999.0f;
		InrangeLoopExitAutoCallback AutoLock;
		InRangeSetRecProt::iterator itr = GetInRangeSetBegin( AutoLock );
		InRangeSetRecProt::iterator it2 = itr;
		InRangeSetRecProt::iterator iend = GetInRangeSetEnd();
		Unit * pUnit = NULL;

		if( m_summoner != NULL && ( charges == 1 || ( spell->c_is_flags2 & SPELL_FLAG2_IS_ENEMY_AOE_TARGETTING ) == 0 ) )
		{
			this->AquireInrangeLock(); //make sure to release lock before exit function !
			for(; it2 != iend;)
			{
				itr = it2;
				++it2;
				if( (*itr)->IsUnit() == false )
					continue;
				if( (*itr)->IsCreature() == true && SafeCreatureCast( (*itr) )->IsTotem() && (*itr)->GetEntry() != 5925 ) //grounding totem can take traps
					continue;
				Unit *tUnit = SafeUnitCast(*itr);
//				uint32 TEntry = tUnit->GetEntry(); //debugging
				float dist = GetDistance2dSq( tUnit );
				float distzSq = ( GetPositionZ() - (tUnit)->GetPositionZ() );
				if( (*itr) != m_summoner && dist <= rangeSQ && distzSq < rangeSQ && dist < BestDistance && isAttackable( m_summoner, tUnit ) )
				{
					BestDistance = dist;
					pUnit = SafeUnitCast(*itr);
				}
			}
			this->ReleaseInrangeLock();
			if( pUnit != NULL )
			{
				Spell * sp=SpellPool.PooledNew( __FILE__, __LINE__ );
//				sp->Init( m_summoner, spell, true, NULL); //spells will fail saying out of range :(
				sp->Init( this, spell, true, NULL);
				SpellCastTargets tgt( pUnit->GetGUID() );
				tgt.m_targetMask |= TARGET_FLAG_DEST_LOCATION;
				tgt.m_destX = GetPositionX();
				tgt.m_destY = GetPositionY();
				tgt.m_destZ = GetPositionZ();
				sp->prepare(&tgt);

				// proc on trap trigger
				if( pInfo->Type == GAMEOBJECT_TYPE_TRAP )
				{
					if( m_summoner != NULL )
						m_summoner->HandleProc( PROC_ON_TRAP_TRIGGER, pUnit, spell );
				} 

				if(m_summonedGo)
				{
					ExpireAndDelete();
					this->ReleaseInrangeLock();
					return;
				}
			}
		}
		
		itr = GetInRangeSetBegin( AutoLock );
		it2 = itr;
		iend = GetInRangeSetEnd();
		this->AquireInrangeLock(); //make sure to release lock before exit function !
		for(; it2 != iend;)
		{
			itr = it2;
			++it2;

			if( (*itr)->IsCreature() == true && SafeCreatureCast( (*itr) )->IsTotem() && (*itr)->GetEntry() != 5925 ) //grounding totem can take traps
				continue;

			float dist = GetDistance2dSq((*itr));
			float distz = abs( GetPositionZ() - (*itr)->GetPositionZ() );
			if( (*itr) != m_summoner 
				&& (*itr)->IsUnit() 
				&& ( dist <= rangeSQ && distz < rangeSQ )
				&& !(*itr)->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_MOUNTED_TAXI | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_DEAD)
				)
			{
				pUnit = SafeUnitCast(*itr);
				if(m_summonedGo)
				{
					if(!m_summoner)
					{
						ExpireAndDelete();
						ReleaseInrangeLock();
						return;
					}
					if(!isAttackable(m_summoner,pUnit))
						continue;
				}
				
				Spell * sp=SpellPool.PooledNew( __FILE__, __LINE__ );
				sp->Init((Object*)this,spell,true,NULL);
				SpellCastTargets tgt( pUnit->GetGUID() );
				tgt.m_destX = GetPositionX();
				tgt.m_destY = GetPositionY();
				tgt.m_destZ = GetPositionZ();
				sp->prepare(&tgt);

				// proc on trap trigger
				if( pInfo->Type == GAMEOBJECT_TYPE_TRAP )
				{
					if( m_summoner != NULL )
						m_summoner->HandleProc( PROC_ON_TRAP_TRIGGER, pUnit, spell );
				} 

				if(m_summonedGo)
				{
					ExpireAndDelete();
					this->ReleaseInrangeLock();
					return;
				}

				if( charges > 0 )
				{
					charges--;
					if( charges == 0 )
					{
						//needed in battlegrounds to respawn buffs
						if( RespawnCaster > 0 )
						{
							charges = ChargesInitial;
							Despawn( 1, RespawnCaster );
						}
						this->ReleaseInrangeLock();
						return;	 // on area dont continue.
					}
				}

				if(spell->eff[0].EffectImplicitTargetA == EFF_TARGET_ALL_ENEMY_IN_AREA_INSTANT || spell->eff[0].EffectImplicitTargetB == EFF_TARGET_ALL_ENEMY_IN_AREA_INSTANT )
				{
					this->ReleaseInrangeLock();
					return;	 // on area dont continue.
				}
			}
		}
		this->ReleaseInrangeLock();
	}
}

void GameObject::OnPrePushToWorld()
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
	UpdateGoRotationXFromOrientation();
	charges = ChargesInitial; //reset charges count in case this is a respawn
}

void GameObject::Spawn(MapMgr * m)
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
	PushToWorld(m);	
	CALL_GO_SCRIPT_EVENT(this, OnSpawn)();
	loot.looters.clear();
}

void GameObject::Despawn(uint32 delay, uint32 respawntime)
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
	if(delay)
	{
		if( sEventMgr.HasEvent(this,EVENT_GAMEOBJECT_EXPIRE) == false )
			sEventMgr.AddEvent(this, &GameObject::Despawn, respawntime, EVENT_GAMEOBJECT_EXPIRE, delay, 1,0);	//need to have this executed in world context
		return;
	}
	else
		Despawn( respawntime );
}

void GameObject::Despawn(uint32 respawntime)
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
	if( !IsGameObject() )
	{
		sLog.outDebug(" GameObject::Despawn : !!! omg invalid call on wrong object !\n");
		ASSERT(false);
		return;
	}
	//do not loop add / readd events if map is already shutting down
//	if( GetMapMgr()->_shutdown == true )
//		return;
	//zack: so what if it got removed before ?
//	if(!IsInWorld())
//  {
//	 
//		return;
//	}

	//empty loot
	loot.items.clear();
	loot.currencies.clear();

	//This is for go get deleted while looting
	if( m_spawn && m_noRespawn == false )
	{
		SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, m_spawn->state);
		SetUInt32Value(GAMEOBJECT_FLAGS, m_spawn->flags);
		//do not fully delete spawns ? Why would you want to do that ?
		//!!! I might be wrong about this. But some scripts are deleting static spawns. And that is not nice. Maybe temp delete them only ?
		if( respawntime == 0 )
			respawntime = GetInfo()->respawn_time;
	}

	CALL_GO_SCRIPT_EVENT(this, OnDespawn)();

	if( respawntime && GetMapMgr() )
	{
		//just to make sure we save values
		MapMgr *oldMap = GetMapMgr();

		sEventMgr.RemoveEvents(this);
		//remove from world also deletes all our events
		Object::RemoveFromWorld(false);

		//needs to be after removing from world
		m_respawnCell = oldMap->GetCellByCoords( GetPositionX(), GetPositionY() );
		if( m_respawnCell && m_respawnCell->_respawnObjects.find( this ) == m_respawnCell->_respawnObjects.end() )
		{
			m_respawnCell->_respawnObjects.insert( this );
			sEventMgr.AddEvent(oldMap, &MapMgr::EventRespawnGameObject, this, GetPositionX(), GetPositionY(), EVENT_GAMEOBJECT_ITEM_SPAWN, respawntime, 1, 0);
		}
	}
	else
	{
		Object::RemoveFromWorld(true);
		//this might be respawning later
		if( m_spawn == NULL )
			ExpireAndDelete();
	}
}

void GameObject::SaveToDB(const char *to_file_name)
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
	std::stringstream ss;
	ss << "REPLACE INTO gameobject_spawns VALUES("
		<< ((m_spawn == NULL) ? 0 : m_spawn->id) << ","
		<< "'" << GetEntry() << "',"
		<< "'" << GetMapId() << "',"
		<< "'" << GetPositionX() << "',"
		<< "'" << GetPositionY() << "',"
		<< "'" << GetPositionZ() << "',"
		<< "'" << GetOrientation() << "',"
		<< "'" << GetFloatValue(GAMEOBJECT_PARENTROTATION) << "',"
		<< "'" << GetFloatValue(GAMEOBJECT_PARENTROTATION_1) << "',"
		<< "'" << GetFloatValue(GAMEOBJECT_PARENTROTATION_2) << "',"
		<< "'" << GetFloatValue(GAMEOBJECT_PARENTROTATION_3) << "',"
		<< "'" << (uint32)GetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE) << "',"
		<< "'" << GetUInt32Value(GAMEOBJECT_FLAGS) << "',"
		<< "'" << GetUInt32Value(GAMEOBJECT_FACTION) << "',"
		<< "'" << GetFloatValue(OBJECT_FIELD_SCALE_X) << "',"
		<< "'" << ((uint32)m_rotationX)<< "',"
		<< "'" << ((uint32)(m_rotationX>>32))<< "'"
		<< ")";
	if( to_file_name != NULL )
	{
		FILE * OutFile;

		OutFile = fopen( to_file_name, "at" );
		if (!OutFile) 
			return;
		fwrite( ss.str().c_str(), 1, ss.str().size(), OutFile );
		fclose( OutFile );
	}
	else
		WorldDatabase.Execute(ss.str().c_str());
}

void GameObject::InitAI()
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
	
	if(!pInfo)
	{ 
		return;
	}
	
	// this fixes those fuckers in booty bay
	if(pInfo->SpellFocus == 0 &&
		pInfo->sound1 == 0 &&
		pInfo->sound2 == 0 &&
		pInfo->sound3 != 0 &&
		pInfo->sound5 != 3 &&
		pInfo->sound9 == 1)
		return;

	if(pInfo->DisplayID == 1027)//Shaman Shrine
	{
		if(pInfo->ID != 177964 && pInfo->ID != 153556)
		{
			//Deactivate
			//SetUInt32Value(GAMEOBJECT_DYNAMIC, 0);
		}
	}


	uint32 spellid = 0;
	if(pInfo->Type==GAMEOBJECT_TYPE_TRAP)
	{	
		charges = ChargesInitial = 1;
		spellid = pInfo->sound3;
	}
	else if(pInfo->Type == GAMEOBJECT_TYPE_SPELL_FOCUS)
	{
		// get spellid from attached gameobject - by sound2 field
		if( pInfo->sound2 == 0 )
		{ 
			return;
		}
		if( GameObjectNameStorage.LookupEntry( pInfo->sound2 ) == NULL )
		{ 
			return;
		}
		spellid = GameObjectNameStorage.LookupEntry( pInfo->sound2 )->sound3;
	}
	else if(pInfo->Type == GAMEOBJECT_TYPE_RITUAL)
	{	
		m_ritualmembers = new uint64[pInfo->SpellFocus];
		memset(m_ritualmembers,0,sizeof(uint64)*pInfo->SpellFocus);
	}
    else if(pInfo->Type == GAMEOBJECT_TYPE_CHEST)
    {
        Lock *pLock = dbcLock.LookupEntry(GetInfo()->SpellFocus);
        if(pLock)
        {
            for(uint32 i=0; i < 5; i++)
            {
               if( pLock->locktype[i] == GO_LOCKTYPE_SKILL_REQ ) //locktype;
               {
                    //herbalism and mining;
                    if(pLock->lockmisc[i] == LOCKTYPE_MINING || pLock->lockmisc[i] == LOCKTYPE_HERBALISM)
                    {
						CalcMineRemaining(true);
                    }
                }
            }
        }

    }
	else if ( pInfo->Type == GAMEOBJECT_TYPE_FISHINGHOLE )
	{
		CalcFishRemaining( true );
	}

	if( myScript == NULL )
		myScript = sScriptMgr.CreateAIScriptClassForGameObject(GetEntry(), this);

	// hackfix for bad spell in BWL
	if(!spellid || spellid == 22247)
	{ 
		return;
	}

	SpellEntry *sp= dbcSpell.LookupEntry(spellid);
	if(!sp)
	{
		spell = NULL;
		return;
	}
	else
	{
		spell = sp;
	}
	//ok got valid spell that will be casted on target when it comes close enough
	//get the range for that 
	
	float r = 0;

	for(uint32 i=0;i<3;i++)
	{
		if(sp->eff[i].Effect)
		{
			float t = GetRadius(dbcSpellRadius.LookupEntry(sp->eff[i].EffectRadiusIndex));
			if(t > r)
				r = t;
		}
	}
/*
	if(r < 0.1)//no range
		r = GetMaxRange(dbcSpellRange.LookupEntry(sp->rangeIndex));
	if( pInfo->Type==GAMEOBJECT_TYPE_TRAP 
		&& sp->rangeIndex != 13 && sp->rangeIndex != 173//13 is anywhere
		)
	{
		r = MAX( r, GetMaxRange(dbcSpellRange.LookupEntry(sp->rangeIndex)) );
		if( r >= 10 )
			r = 10;
	} */
	if( r < 2.0f )
		r = 2.0f;

	rangeSQ = r*r;//square to make code faster
	checkrate = 20;//once in 2 seconds
	
}

bool GameObject::Load(GOSpawn *spawn)
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
	if(!CreateFromProto(spawn->entry,0,spawn->x,spawn->y,spawn->z,spawn->o,spawn->parent_rot1,spawn->parent_rot2,spawn->parent_rot3,spawn->parent_rot4))
	{ 
		return false;
	}

	m_spawn = spawn;
	m_phase = spawn->phase;

	if( spawn->flags != 0 )
		SetUInt32Value(GAMEOBJECT_FLAGS,spawn->flags);
	SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE,spawn->state);	

	//!!!!!!these are elevators. I'm leaving this here so next time i will not search 1 hour for them
/*	if( pInfo->Type == GAMEOBJECT_TYPE_TRANSPORT )
	{
		SetUInt32Value(GAMEOBJECT_FLAGS, 40 );
		SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, 1);	
	} */

	if(spawn->faction)
	{
		SetFaction(spawn->faction);
	}
	SetFloatValue(OBJECT_FIELD_SCALE_X,spawn->scale);
	_LoadQuests();
	CALL_GO_SCRIPT_EVENT(this, OnCreate)();
	CALL_GO_SCRIPT_EVENT(this, OnSpawn)();

	InitAI();

	_LoadQuests();
	return true;
}

void GameObject::DeleteFromDB()
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
	if( m_spawn != NULL )
		WorldDatabase.Execute("DELETE FROM gameobject_spawns WHERE id=%u", m_spawn->id);
}

void GameObject::EventCloseDoor()
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
	SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, GO_STATE_READY);
	SetUInt32Value(GAMEOBJECT_FLAGS, 0);
}

void GameObject::UseFishingNode(Player *player)
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
	sEventMgr.RemoveEvents( this );
	if( GetUInt32Value( GAMEOBJECT_FLAGS ) != GAMEOBJECT_CLICKABLE ) // Clicking on the bobber before something is hooked
	{
		player->GetSession()->OutPacket( SMSG_FISH_NOT_HOOKED );
		EndFishing( player, true );
		return;
	}
	
	/* Unused code: sAreaStore.LookupEntry(GetMapMgr()->GetAreaID(GetPositionX(),GetPositionY()))->ZoneId*/
	uint32 zone = player->GetAreaID();
	if( zone == 0 ) // If the player's area ID is 0, use the zone ID instead
		zone = player->GetZoneId();

	FishingZoneEntry *entry = FishingZoneStorage.LookupEntry( zone );

	if( entry == NULL ) // No fishing information found for area or zone, log an error, and end fishing
	{
		sLog.outError( "ERROR: Fishing zone information for zone %d not found!", zone );
		EndFishing( player, true );
		return;
	}
	uint32 maxskill = entry->MaxSkill;
	uint32 minskill = entry->MinSkill;

	if( player->_GetSkillLineCurrent( SKILL_FISHING, false ) < maxskill )	
		player->_AdvanceSkillLine( SKILL_FISHING, float2int32( 1.0f * sWorld.getRate( RATE_SKILLRATE ) ) );

	GameObject * school = NULL;
	this->AquireInrangeLock(); //make sure to release lock before exit function !
	InrangeLoopExitAutoCallback AutoLock;
	for ( InRangeSetRecProt::iterator it = GetInRangeSetBegin( AutoLock ); it != GetInRangeSetEnd(); ++it )
	{
		if ( (*it) == NULL || (*it)->GetTypeId() != TYPEID_GAMEOBJECT || (*it)->GetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_TYPEID) != GAMEOBJECT_TYPE_FISHINGHOLE)
			continue;
		school = SafeGOCast( *it );
		if ( !isInRange( school, (float)school->GetInfo()->sound1 ) )
		{
			school = NULL;
			continue;
		}
		else
			break;
	}
	this->ReleaseInrangeLock();

	if ( school != NULL ) // open school loot if school exists
	{
//		lootmgr.FillGOLoot( &school->loot, school->GetEntry(), school->GetMapMgr() ? ( school->GetMapMgr()->iInstanceMode ? true : false ) : false );
		// zack : blizz recycles gameobject entry. We use something that is same for all entry variations
		lootmgr.FillGOLoot(&school->loot,school->GetInfo()->sound1, (school->GetMapMgr() != NULL) ? (school->GetMapMgr()->instance_difficulty) : 0);
		player->SendLoot( school->GetGUID(), LOOT_FISHING );

		EndFishing( player, false );
		school->CatchFish();
		if ( !school->CanFish() )
//			sEventMgr.AddEvent( school, &GameObject::Despawn, ( 1800000 + RandomUInt( 3600000 ) ), EVENT_GAMEOBJECT_EXPIRE, 10000, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT ); // respawn in 30 - 90 minutes
//			sEventMgr.AddEvent( school, &GameObject::Despawn, ( 300000 + RandomUInt( 300000 ) ), EVENT_GAMEOBJECT_EXPIRE, 10000, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT ); // respawn in 30 - 90 minutes
//			sEventMgr.AddEvent( school, &GameObject::Despawn, ( school->GetInfo()->respawn_time + RandomUInt( school->GetInfo()->respawn_time ) ), EVENT_GAMEOBJECT_EXPIRE, 1, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT ); 
			sEventMgr.AddEvent( school, &GameObject::Despawn, school->GetInfo()->respawn_time, EVENT_GAMEOBJECT_EXPIRE, 10000, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT ); 
	}
	else if( RandChance( ( ( player->_GetSkillLineCurrent( SKILL_FISHING, true ) - minskill ) * 100 ) / maxskill ) ) // Open loot on success, otherwise FISH_ESCAPED.
	{	
		lootmgr.FillFishingLoot( &loot, zone );
		player->SendLoot( GetGUID(), LOOT_FISHING );
		player->Event_AchiementCriteria_Received( ACHIEVEMENT_CRITERIA_TYPE_LOOT_TYPE,LOOT_FISHING,ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
		EndFishing( player, false );
	}
	else // Failed
	{
		player->GetSession()->OutPacket( SMSG_FISH_ESCAPED );
		EndFishing( player, true );
	}

}

void GameObject::EndFishingEvent(uint64 guid, bool abort )
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
	if( IsInWorld() )
	{
		Player *player = GetMapMgr()->GetPlayer( guid );
		if( player )
			EndFishing( player, abort );
	}
	else
		EndFishing( NULL, abort );
}

void GameObject::EndFishing(Player *player, bool abort )
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
	if( player )
	{
		Spell * spell = player->GetCurrentSpell();
		
		if(spell)
		{
			if(abort)   // abort becouse of a reason
			{
				//FIXME: here 'failed' should appear over progress bar
				spell->SendChannelUpdate(0);
				//spell->cancel();
				spell->finish();
			}
			else		// spell ended
			{
				spell->SendChannelUpdate(0);
				spell->finish();
			}
		}
	}

	if(!abort)
	{
//		sEventMgr.AddEvent(this, &GameObject::ExpireAndDelete, EVENT_GAMEOBJECT_EXPIRE, 1, 1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
//		sEventMgr.AddEvent(this, &GameObject::Despawn, ( GetInfo()->respawn_time + RandomUInt( GetInfo()->respawn_time ) ), EVENT_GAMEOBJECT_EXPIRE, 1, 1, 0 ); 
		sEventMgr.AddEvent(this, &GameObject::Despawn, (uint32)20000, GetInfo()->respawn_time, EVENT_GAMEOBJECT_EXPIRE, 1, 1, 0 ); 
	}
	else
//		ExpireAndDelete();
//		sEventMgr.AddEvent( this, &GameObject::Despawn, ( GetInfo()->respawn_time + RandomUInt( GetInfo()->respawn_time ) ), EVENT_GAMEOBJECT_EXPIRE, 1, 1, 0 ); 
		sEventMgr.AddEvent( this, &GameObject::Despawn, GetInfo()->respawn_time , EVENT_GAMEOBJECT_EXPIRE, 1, 1, 0 ); 
}

void GameObject::FishHooked(uint64 guid)
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
	if( IsInWorld() )
	{
		Player *player = GetMapMgr()->GetPlayer( guid );
		if( player && player->GetSession() )
		{
			sStackWorldPacket(  data, SMSG_GAMEOBJECT_CUSTOM_ANIM, 50); 
			data << GetGUID();
			data << (uint32)(0); // value < 4
			player->GetSession()->SendPacket(&data);
		}
	}
	//SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, 0);
	//BuildFieldUpdatePacket(player, GAMEOBJECT_FLAGS, GAMEOBJECT_CLICKABLE);
	SetUInt32Value(GAMEOBJECT_FLAGS, GAMEOBJECT_CLICKABLE);
 }

/////////////
/// Quests

void GameObject::AddQuest(QuestRelation *Q)
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
	m_quests->push_back(Q);
}

void GameObject::DeleteQuest(QuestRelation *Q)
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
	list<QuestRelation *>::iterator it;
	for( it = m_quests->begin(); it != m_quests->end(); ++it )
	{
		if( ( (*it)->type == Q->type ) && ( (*it)->qst == Q->qst ) )
		{
			delete (*it);
			(*it) = NULL;
			m_quests->erase(it);
			break;
		}
	}
}

Quest* GameObject::FindQuest(uint32 quest_id, uint8 quest_relation)
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
   
	list< QuestRelation* >::iterator it;
	for( it = m_quests->begin(); it != m_quests->end(); ++it )
	{
		QuestRelation* ptr = (*it);
		if( ( ptr->qst->id == quest_id ) && ( ptr->type & quest_relation ) )
		{
			return ptr->qst;
		}
	}
	return NULL;
}

uint16 GameObject::GetQuestRelation(uint32 quest_id)
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
	uint16 quest_relation = 0;
	list< QuestRelation* >::iterator it;
	for( it = m_quests->begin(); it != m_quests->end(); ++it )
	{
		if( (*it) != NULL && (*it)->qst->id == quest_id )
		{
			quest_relation |= (*it)->type;
		}
	}
	return quest_relation;
}

uint32 GameObject::NumOfQuests()
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
	return (uint32)m_quests->size();
}

void GameObject::_LoadQuests()
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
	sQuestMgr.LoadGOQuests(this);
}

/////////////////
// Summoned Go's

void GameObject::_Expire()
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
	sEventMgr.RemoveEvents(this);
	if(IsInWorld())
		RemoveFromWorld(true);

	sGarbageCollection.AddObject( this );
}

void GameObject::ExpireAndDelete()
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
	if( !IsGameObject() )
	{
		sLog.outDebug(" GameObject::Despawn : !!! omg invalid call on wrong object !\n");
		ASSERT(false);
		return;
	}
	if(m_deleted)
	{	 
		return;
	}

	m_deleted = true;
	
	/* remove any events */
	sEventMgr.RemoveEvents(this);
	if( IsInWorld() )
		sEventMgr.AddEvent(this, &GameObject::_Expire, EVENT_GAMEOBJECT_EXPIRE, 1, 1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
	else
		_Expire();
}

void GameObject::Deactivate()
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
	SetUInt32Value(GAMEOBJECT_DYNAMIC, 0);
}

void GameObject::CallScriptUpdate()
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
	ASSERT(myScript);
	if( myScript )
		myScript->AIUpdate();
}

void GameObject::OnPushToWorld()
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
	Object::OnPushToWorld();
	CALL_INSTANCE_SCRIPT_EVENT( m_mapMgr, OnGameObjectPushToWorld )( this );
	CALL_GO_SCRIPT_EVENT(this, OnPushToWorld)();
}

void GameObject::OnRemoveInRangeObject(Object* pObj)
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
	Object::QueueRemoveInRangeObject(pObj);
	if( GetUInt64Value( OBJECT_FIELD_CREATED_BY ) == pObj->GetGUID() && pObj->IsUnit() )
	{
		Unit *m_summoner = SafeUnitCast( pObj );
		for(int i = 0; i < 4; i++)
			if ( m_summoner->m_ObjectSlots[i] == GetGUID())
				m_summoner->m_ObjectSlots[i] = 0;
		ExpireAndDelete();
	}
}

void GameObject::RemoveFromWorld(bool free_guid)
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE

	if( GetUInt64Value( OBJECT_FIELD_CREATED_BY ) )
	{
		Unit *m_summoner = GetMapMgr()->GetUnit( GetUInt64Value( OBJECT_FIELD_CREATED_BY ) );
		if( m_summoner )
		{
			for(int i = 0; i < 4; i++)
				if( m_summoner->m_ObjectSlots[i] == GetGUID() )
					m_summoner->m_ObjectSlots[i] = 0;
		}
	}

	sStackWorldPacket( data, SMSG_GAMEOBJECT_DESPAWN_ANIM, 10 );
	data << GetGUID();
	SendMessageToSet(&data,true);

	sEventMgr.RemoveEvents(this, EVENT_GAMEOBJECT_TRAP_SEARCH_TARGET);
	Object::RemoveFromWorld(free_guid);
}

bool GameObject::HasLoot()
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
    int count=0;
    for(vector<__LootItem>::iterator itr = loot.items.begin(); itr != loot.items.end(); ++itr)
	{
		if( itr->item.itemproto->Bonding == ITEM_BIND_QUEST || itr->item.itemproto->Bonding == ITEM_BIND_QUEST2 )
			continue;

		count += (itr)->iItemsCount;
	}

    return (count>0);

}

uint32 GameObject::GetGOReqSkill()  
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
	if(GetEntry() == 180215) 
	{ 
		return 300;
	}

	if(GetInfo() == NULL)
	{ 
		return 0;
	}

	Lock *lock = dbcLock.LookupEntry( GetInfo()->SpellFocus );
	if(!lock) 
	{ 
		return 0;
	}
	for(uint32 i=0;i<5;i++)
		if(lock->locktype[i] == GO_LOCKTYPE_SKILL_REQ && lock->minlockskill[i])
		{
			return lock->minlockskill[i];
		}
	return 0;
}
/*
        const int PACK_COEFF_YZ = 1 << 20;
        const int PACK_COEFF_X = 1 << 21;
        public static ulong Pack(this Quaternion quat)
        {
            int w_sign = quat.W >= 0 ? 1 : -1;

            long x = ((int)(quat.X * (double)PACK_COEFF_X)) * w_sign & ((1 << 22) - 1);
            long y = ((int)(quat.Y * (double)PACK_COEFF_YZ)) * w_sign & ((1 << 21) - 1);
            long z = ((int)(quat.Z * (double)PACK_COEFF_YZ)) * w_sign & ((1 << 21) - 1);

            return (ulong)(z | (y << 21) | (x << 42));
        }
        public static Quaternion UnpackQuaternion(this ulong value)
        {
            var x = (float)(value >> 42) / (float)PACK_COEFF_X;
            var y = (float)(value << 22 >> 43) / (float)PACK_COEFF_YZ;
            var z = (float)(value << 43 >> 43) / (float)PACK_COEFF_YZ;

            var w = 1.0f - (x * x + y * y + z * z);
            w = Math.Sign(w) * (float)Math.Sqrt(Math.Abs(w));

            return new Quaternion(x, y, z, w);
        }
*/
void GameObject::UpdateGoRotationXFromOrientation()
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE

	//probably using the new spawning method
	if( m_spawn && m_spawn->o == GetOrientation() && m_rotationX == m_spawn->precise_facing )
		return;

	m_rotationX = 0;

	//normalize it : 0 - 6.28
	float ori = GetOrientation();
	if( ori > 6.28f )
		ori = ori - ((int)(ori/6.28f))*6.28f;
	if( ori < 0.0f )
		ori = ori - ((int)(ori/6.28f))*6.28f;
	//convert it to range : [-3.14,3.14]
	float new_ori;
	if( ori > 3.14f )
		new_ori = ori - 6.14f;
	else
		new_ori = ori;
	SetOrientation( new_ori );

	float ang = GetOrientation();
    static double const atan_pow = atan(pow(2.0f, -20.0f));
	float f_rot1; 
	if( ang < 0 )
		f_rot1 = 1 + sin( ((float)M_PI+ang) / 2.3648f )*(1.0f + ang / (float)M_PI); //there must be a much much better way for this. Just could not find it yet :(
	else
		f_rot1 = sin(ang / 2.0f); 
	int64 i_rot1 = (int64)( f_rot1 / atan_pow );
	m_rotationX |= (i_rot1 << 43 >> 43) & 0x00000000001FFFFF;
	//float f_rot2 = sin(0.0f / 2.0f);
	//int64 i_rot2 = f_rot2 / atan(pow(2.0f, -20.0f));
	//rotation |= (((i_rot2 << 22) >> 32) >> 11) & 0x000003FFFFE00000;

	//float f_rot3 = sin(0.0f / 2.0f);
	//int64 i_rot3 = f_rot3 / atan(pow(2.0f, -21.0f));
	//rotation |= (i_rot3 >> 42) & 0x7FFFFC0000000000;

/*
	//i keep telling people that the formula is plain wrong. Nobody cares
	double f_rot1 = sin(GetOrientation() / 2.0f);
	double f_rot2 = cos(GetOrientation() / 2.0f);
	float r2=GetParentRotation(2);
	float r3=GetParentRotation(3);
	if(r2== 0.0f && r3== 0.0f)
	{
		r2 = (float)f_rot1;
		r3 = (float)f_rot2;
		SetParentRotation(2, r2);
		SetParentRotation(3, r3);
	}*/

	//from blizz : 
	//2.7838	1031841
	//2.60926	1011652
	//2.43473	983765
	//2.37364	972222
	//1.78896	817768
	//1.56207	738213
	//1.48353	708407
	//1.25664	616337
	//1.24791	612630
	//0.863937	438996
	//0.67195	345705
	//0.575957	297811
	//0.0698136	36595
	//0.0261791	13725

	//-3.11539	1048666
	//-2.82743	1061486
	//-2.75761	1067842
	//-2.46964	1107203
	//-2.23402	1154698
	//-2.08567	1191355
	//-2.03331	1205494
	//-1.69297	1311815
	//-1.46608	1395518
	//-0.98611	1600841
	//-0.890117	1645729
	//-0.680677	1747131
	//-0.445059	1865734
}
 
void GameObject::TakeDamage(uint32 ammount, Object* mcaster, Object* pcaster, uint32 spellid)
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
	if(pInfo->Type != GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING)
		return;

	if(Health > ammount)
		Health -= ammount;
	else if(Health < ammount)
		Health = 0;

	if(Health == 0)
		return;

	if(HasFlag(GAMEOBJECT_FLAGS,GO_FLAG_DAMAGED) && !HasFlag(GAMEOBJECT_FLAGS,GO_FLAG_DESTROYED))
	{
		if(Health <= 0)
		{
			RemoveFlag(GAMEOBJECT_FLAGS,GO_FLAG_DAMAGED);
			SetFlag(GAMEOBJECT_FLAGS,GO_FLAG_DESTROYED);
			SetUInt32Value(GAMEOBJECT_DISPLAYID,pInfo->Unknown1);
		}
	}
	else if(!HasFlag(GAMEOBJECT_FLAGS,GO_FLAG_DAMAGED) && !HasFlag(GAMEOBJECT_FLAGS,GO_FLAG_DESTROYED))
	{
		if(Health <= pInfo->sound5)
		{
			if(pInfo->Unknown1 == 0)
				Health = 0;
			else if(Health == 0)
				Health = 1;
			SetFlag(GAMEOBJECT_FLAGS,GO_FLAG_DAMAGED);
			SetUInt32Value(GAMEOBJECT_DISPLAYID,pInfo->sound4);
		}
	}

	WorldPacket data(SMSG_DESTRUCTIBLE_BUILDING_DAMAGE, 20);
	data << mcaster->GetNewGUID() << pcaster->GetNewGUID();
	data << uint32(ammount) << spellid;
	mcaster->SendMessageToSet(&data, (mcaster->IsPlayer() ? true : false));
}

void GameObject::Rebuild()
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
	RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_DAMAGED);
	RemoveFlag(GAMEOBJECT_FLAGS,GO_FLAG_DESTROYED);
	SetUInt32Value(GAMEOBJECT_DISPLAYID, pInfo->DisplayID);
	Health = pInfo->SpellFocus + pInfo->sound5;
}

void GameObject::TriggerScriptEventStr(string func)
{
	INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE
	if( myScript )
		myScript->StringFunctionCallStr( func.c_str() );
}
