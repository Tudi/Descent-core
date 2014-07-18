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

Unit::Unit()
{
	int i;

	m_objectTypeId = TYPEID_UNIT;
	internal_object_type = INTERNAL_OBJECT_TYPE_UNIT;

	m_attackTimer = 0;
	m_attackTimer_1 = 0;
	m_dualWield = false;

	m_fearmodifiers = 0;
	m_state = 0;
	m_special_state = 0;
	m_deathState = ALIVE;
	m_currentSpell = NULL;
	m_meleespell = 0;
	m_TotemSlots[0] = NULL;
	m_TotemSlots[1] = NULL;
	m_TotemSlots[2] = NULL;
	m_TotemSlots[3] = NULL;
	m_ObjectSlots[0] = 0;
	m_ObjectSlots[1] = 0;
	m_ObjectSlots[2] = 0;
	m_ObjectSlots[3] = 0;
	m_silenced = 0;
	disarmed   = false;

	//DK:modifiers
	PctRegenModifier = 100;
	RegenModifier = 0;
	for( i = 0; i < POWER_TYPE_COUNT; i++ )
		PctPowerRegenModifier[i] = 0.0f;
	PctPowerRegenModifier[POWER_TYPE_MANA] = 100.0f * ( sWorld.getRate(RATE_POWER1) - 1.0f );
	PctPowerRegenModifier[POWER_TYPE_FOCUS] = 100.0f * ( sWorld.getRate(RATE_POWER3) - 1.0f );
	PctPowerRegenModifier[POWER_TYPE_ENERGY] = 100.0f * ( sWorld.getRate(RATE_POWER4) - 1.0f );

	m_speedModifier = 0;
	m_slowdown = 0;
	m_speedup = 0;
	m_mountedspeedModifier=0;
	m_minSpeed = 1;
	for(i=0;i<MECHANIC_TOTAL;i++)
	{
		MechanicsDispels[i]=0;
		MechanicsResistancesPCT[i]=0;
		ModDamageTakenByMechPCT[i]=0;
		ModDamageTakenByMechFlat[i]=0;
		MechanicDurationPctMod[i]=0;
	}
	OffensiveMagicalDurationModPCT = 0;
	for(i=0;i<DISPEL_COUNT_NON_BENEFICIAL;i++)
		DispelDurationPctMod[i]=0;
	m_pacified = 0;
	m_interruptRegen = 0;
	m_resistChance = 0;
	m_powerRegenPCT = 0;
	RAPvModifier=0;
	APvModifier=0;
	stalkedby=0;

	m_stunned = 0;
	m_manashieldamt=0;
	m_rooted = 0;
	m_canMove = 0;
	m_noInterrupt = 0;
	m_modlanguage = -1;
	m_magnetcaster = 0;
	
	critterPet = NULL;
	summonPet = NULL;

	m_useAI = false;
	for(i=0;i<10;i++)
		dispels[i]=0;
	for(i=0;i<CREATURE_TYPES;i++)
	{
		CreatureAttackPowerMod[i] = 0;
		CreatureRangedAttackPowerMod[i] = 0;
	}

	m_invisFlag = INVIS_FLAG_NORMAL;
	m_invisDetect = INVIS_FLAG_NORMAL;

	m_stealthLevel = 0;
	m_stealthDetectBonus = 0;
	m_stealth = 0;
	m_stealth_dissalowed = 0;

	for(i=0;i<5;i++)
		BaseStats[i]=0;

	m_H_regenTimer = 2000;
	m_P_regenTimer = 2000;

	//	if(GetTypeId() == TYPEID_PLAYER) //only player for now
	//		CalculateActualArmor();

	m_emoteState = 0;
	m_oldEmote = 0;	
	
	BaseDamage[0]=0;
	BaseOffhandDamage[0]=0;
	BaseRangedDamage[0]=0;
	BaseDamage[1]=0;
	BaseOffhandDamage[1]=0;
	BaseRangedDamage[1]=0;

	m_CombatUpdateTimer = 0;
	for(i=0;i<SCHOOL_COUNT;i++)
	{
		SchoolImmunityAntiEnemy[i] = 0;
		SchoolImmunityAntiFriend[i] = 0;
		AuraSchoolImmunityList[i] = 0;
		BaseResistance[i] = 0;
//		HealDoneMod[i] = 0;
//		HealDonePctMod[i] = 0;
//		HealTakenMod[i] = 0;
//		HealTakenPctMod[i] = 0;
		DamageTakenMod[i] = 0;
		AoeDamageTakenPctMod[i] = 100;
//		DamageDoneModPCT[i]= 0;
		SchoolCastPrevent[i]=0;
		DamageTakenPctMod[i] = 100;
		SpellCritChanceSchool[i] = 0;
		PowerCostMod[i] = 0;
		TargetResistRedModFlat[i] = 0; // armor penetration & spell penetration
		AttackerCritChanceMod[i]=0;
		CritMeleeDamageTakenPctMod[i]=0;
		CritRangedDamageTakenPctMod[i]=0;
		m_generatedThreatModifyerPCT[i] = 0;
	}
	AoeDamageTakenPctModFromCreatures = 100;
	HealDonePctMod = 0;
	HealCritDonePctMod = 0;
	HealDoneMod = 0;
	HealTakenMod = 0;
	HealTakenPctMod = 0;
//	DamageTakenPctModOnHP35 = 1;
	RangedDamageTaken = RangedDamageTakenPct = 0;
	MeleeDmgPCTMod = 100;

	for(i = 0; i < 5; i++)
	{
		m_detectRangeGUID[i] = 0;
		m_detectRangeMOD[i] = 0;
	}

//	trackStealth = false;

	m_threatModifyer = 0;
	
	// diminishing return stuff
	for(uint32 i=0;i<DIMINISHCOUNT;i++)
	{
		m_diminishCount[i] = 0;
		m_diminishExpireStamp[i] = 0;
	}
	for(uint32 i=0;i<MAX_VISIBLE_AURAS;i++)
		m_auraStackCount[i] = 0;

//	dynObj = NULL;
//	pLastSpell = 0;
	m_flyspeedModifier = 0;
	bInvincible = false;
	m_redirectSpellPackets = 0;
	can_parry = false;
	spellcritperc = 0;
	EventsSpellsOverall = EventsMeleeOverall = EventsRangedOverall = 100;
	EventsSpellsCrit = EventsMeleeCrit = EventsRangedCrit = 5;

	polySpell = 0;
	m_procCounter = 0;
//	fearSpell = 0;
	CombatStatus.SetUnit(this);
	m_temp_summon=false;
//	m_spellsbusy=false;
	m_interruptedRegenStamp = 0;
	m_hitfrommeleespell	 = 0;
	m_damageSplitTarget = NULL;
	m_extrastriketarget = 0;
	m_extrastriketargetc = 0;
	trigger_on_stun = 0;
	trigger_on_stun_chance = 100;
	trigger_on_stun_victim = 0;
	trigger_on_stun_chance_victim = 100;
	ModelHalfSize = 1.5f; //worst case unit size. (Should be overwritten)

	m_blockfromspell		= 0;
	m_dodgefromspell		= 0;
	m_parryfromspell		= 0;

	m_damageShields.clear();
	m_reflectSpellSchool.SafeClear();	
	m_procSpells.SafeClear();	
	m_procSpellsHaveTheseFlags = 0;
//	memset( m_procSpellsHaveTheseFlagsCount, 0, sizeof( m_procSpellsHaveTheseFlagsCount ) );
	m_chargeSpells.clear();				
//	tmpAura.clear();
	QueueAuraAdd.SafeClear();
	m_extraStrikeTargets.SafeClear();	
	//since auras are cleared these lists are theoretically empty
	m_auras_pos_size = FIRST_AURA_SLOT + 1;
	m_auras_neg_size = MAX_POSITIVE_AURAS + 1;
	m_auras_pas_size = MAX_AURAS + 1;
	for(uint32 i=0;i<MAX_AURAS + MAX_PASSIVE_AURAS;i++)
		m_auras[i] = NULL;
	m_chain = NULL;
	m_vehicle_data = NULL;
	m_CurrentVehicleSeat = VEHICLE_PASSENGER_SLOT_INVALID;
//	m_aura_packet_serialization = 0;
	m_vehicle_data = NULL;
	for(uint32 i=0;i<AURASTATE_MAX_INDEX;i++)
		m_AuraStates[i] = 0;

	SM_Mods = NULL;
	ImmuneToRanged = 0;
	HealAbsorb = 0;
	//all powers stored in their place ?
	memset( PowerFields, POWER_TYPE_MANA, sizeof( PowerFields ) );
	for(uint32 i=0;i<POWER_TYPE_COUNT;i++)
		PowerFields[i]=-1;	//do not mod these

	//official Values
	m_walkSpeed = 2.5f;
	m_runSpeed = PLAYER_NORMAL_RUN_SPEED+1.0f;//so they can catch up
	m_maxSpeed = 0;
	m_base_runSpeed = m_runSpeed;
	m_base_walkSpeed = m_walkSpeed;

	m_flySpeed = 14.0f;
	m_backFlySpeed = 4.5f;

	m_backWalkSpeed = 4.5f;	// this should really be named m_backRunSpeed
	m_swimSpeed = 4.722222f;
	m_backSwimSpeed = 2.5f;
	m_turnRate = 3.141593f;
	m_pitchRate = 3.141593f;

	m_PositionUpdateCounter = 0;

	//let this be the last thing you do :P or ai interface might not initialize properly because of missing default values !
	m_aiInterface = new AIInterface();
	m_aiInterface->Init(this, AITYPE_AGRO, MOVEMENTTYPE_NONE);
}

Unit::~Unit()
{
	Virtual_Destructor();
}

void Unit::Virtual_Destructor( )
{
	ObjectLock.Acquire();
	sEventMgr.RemoveEvents( this );

	//start to remove badptrs, if you delete from the heap null the ptr's damn!
	RemoveAllAuras();

	if( m_chain != NULL )
	{
		m_chain->RemoveUnit(this);
		m_chain = NULL;
	}

	if( m_aiInterface )
	{
		delete m_aiInterface;
		m_aiInterface = NULL;
	}

	if( m_currentSpell ) 
	{
		m_currentSpell->safe_cancel( true );
		m_currentSpell = NULL;
	}

	if( m_damageSplitTarget ) 
	{
		delete m_damageSplitTarget;
		m_damageSplitTarget = NULL;
	}

	//well we should have moved stuff from here much earlier
	m_extraStrikeTargets.SafeClear( );	ASSERT( m_extraStrikeTargets.begin() == NULL );
	m_extra_attacks.SafeClear( );		ASSERT( m_extra_attacks.begin() == NULL );

	//auras that are left in the queu to be added to the mob but for some reason they were never processed
//	std::map<uint32,Aura*>::iterator tmpAuraitr;
//	for( tmpAuraitr = tmpAura.begin(); tmpAuraitr != tmpAura.end(); tmpAuraitr++)
//		tmpAuraitr->second->Remove();
//	tmpAura.clear();
	while( QueueAuraAdd.Empty() == 0 )
	{
		CommitPointerListNode<Aura> *itr;
		QueueAuraAdd.BeginLoop();
		for( itr = QueueAuraAdd.begin(); itr != QueueAuraAdd.end(); itr = itr->Next() )
		{
			QueueAuraAdd.SafeRemove( itr, 0 );
//			itr->data->Remove();
			AuraPool.PooledDelete( itr->data, __FILE__, __LINE__ );
		}
		QueueAuraAdd.EndLoopAndCommit();
		// unit is getting destroyed but it is still used somewhere. Like he is casting a spell ;)
		if( QueueAuraAdd.IsInLoop() )
			break;
	}
	QueueAuraAdd.SafeClear( 0 );

	mSpellOverrideMap.clear( 1 );

	// reflects not created by auras need to be deleted manually
	m_reflectSpellSchool.SafeClear( );	ASSERT( m_reflectSpellSchool.begin() == NULL );

	m_damageShields.clear();
	m_procSpells.SafeClear( );		ASSERT( m_procSpells.begin() == NULL );
	m_chargeSpells.clear();
	ExitVehicle();
	if( m_vehicle_data && m_vehicle_data->GetOwner() == this )
		DestoryVehicle();

	if( SM_Mods )
	{
		delete SM_Mods;
		SM_Mods = NULL;
	}

	//empty loot
	loot.items.clear();
	loot.currencies.clear();
	ObjectLock.Release();
	Object::Virtual_Destructor();
}

void Unit::Update( uint32 p_time )
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	
	Object::Update( p_time );	//99% this does nothing

	_UpdateSpells( p_time );

	if(!IsDead())
	{
		//-----------------------POWER & HP REGENERATION-----------------
        if( p_time >= m_H_regenTimer )
		    RegenerateHealth();
	    else
		    m_H_regenTimer -= p_time;

		if( p_time >= m_P_regenTimer )
		{
			bool is_interrupted;
			if( m_interruptedRegenStamp > getMSTime() )
				is_interrupted = true;
			else if( CombatStatus.IsInCombat() )
				is_interrupted = true;
			else
				is_interrupted = false;
			RegeneratePower( is_interrupted );
		}
		else
			m_P_regenTimer -= p_time;

		if(m_aiInterface != NULL && m_useAI)
			m_aiInterface->Update(p_time);

		// if health changed since last time. 
		// must be here to avoid deadlocks
//		if(HasUpdateField( UNIT_FIELD_HEALTH ))	//this shit fails to trigger when not regnerating health :S
		if( GetUInt32Value(UNIT_FIELD_HEALTH) != GetUInt32Value(UNIT_FIELD_MAXHEALTH) )
			EventHealthChangeSinceLastUpdate();
	}
}

bool Unit::canReachWithAttack(Unit *pVictim)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if( GetMapId() != pVictim->GetMapId() )
	{ 
		return false;
	}

//	float targetreach = pVictim->GetFloatValue(UNIT_FIELD_COMBATREACH);
	float selfreach;
	if( IsPlayer() )
//		selfreach = 5.0f; // minimum melee range, UNIT_FIELD_COMBATREACH is too small and used eg. in melee spells
		selfreach = 1.5f; // minimum melee range, UNIT_FIELD_COMBATREACH is too small and used eg. in melee spells
	else
		selfreach = m_floatValues[UNIT_FIELD_COMBATREACH];

	float targetradius;
//	targetradius = pVictim->m_floatValues[UNIT_FIELD_BOUNDINGRADIUS]; //this is plain wrong. Represents i have no idea what :)
	targetradius = pVictim->GetModelHalfSize();
	float selfradius;;
//	selfradius = m_floatValues[UNIT_FIELD_BOUNDINGRADIUS];
	selfradius = GetModelHalfSize();
//	float targetscale = pVictim->m_floatValues[OBJECT_FIELD_SCALE_X];
//	float selfscale = m_floatValues[OBJECT_FIELD_SCALE_X];

	//float distance = sqrt(GetDistanceSq(pVictim));
	float delta_x = pVictim->GetPositionX() - GetPositionX();
	float delta_y = pVictim->GetPositionY() - GetPositionY();
	float distance = sqrt(delta_x*delta_x + delta_y*delta_y);


//	float attackreach = (((targetradius*targetscale) + selfreach) + (((selfradius*selfradius)*selfscale)+1.50f));
	float attackreach = targetradius + selfreach + selfradius; // 1.5 + 1.5 + 1.5

	//formula adjustment for player side.
	if( IsPlayer() )
	{
/*
		if( attackreach <= 8 && attackreach >= 5 && targetradius >= 1.80f)
			attackreach = 11; //giant type units

		if( attackreach > 11)
			attackreach = 11; //distance limited to max 11 yards attack range //max attack distance

		if( attackreach < 5 )
			attackreach = 5; //normal units with too small reach.

		//range can not be less than 5 yards - this is normal combat range, SCALE IS NOT SIZE
*/
		// latency compensation!!
		// figure out how much extra distance we need to allow for based on our movespeed and latency.
		if( pVictim->IsPlayer() && SafePlayerCast( pVictim )->m_isMoving )
		{
			// this only applies to PvP.
			uint32 lat = SafePlayerCast( pVictim )->GetSession() ? SafePlayerCast( pVictim )->GetSession()->GetLatency() : 0;

			// if we're over 500 get fucked anyway.. your gonna lag! and this stops cheaters too
			lat = ( lat > 500 ) ? 500 : lat;

			// calculate the added distance : 8 * 0.0005f * 500 = 2
			attackreach += ( m_runSpeed * 0.0005f ) * float( lat );
		}

		if( SafePlayerCast( this )->m_isMoving )
		{
			// this only applies to PvP.
			uint32 lat = SafePlayerCast( this )->GetSession() ? SafePlayerCast( this )->GetSession()->GetLatency() : 0;

			// if we're over 500 get fucked anyway.. your gonna lag! and this stops cheaters too
			lat = ( lat > 500) ? 500 : lat;

			// calculate the added distance : 8 * 0.0005f * 500 = 2
			attackreach += ( m_runSpeed * 0.0005f ) * float( lat );
		}
	}
	// 4.5 + 2 + 2 = 8.5 when 2 players are running at 500 ms latency
	return ( distance <= attackreach );
}

void Unit::GiveGroupXP(Unit *pVictim, Player *PlayerInGroup)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if(!PlayerInGroup) 
	{ 
		return;
	}
	if(!pVictim) 
	{ 
		return;
	}
	if(!PlayerInGroup->InGroup()) 
	{ 
		return;
	}
	Group *pGroup = PlayerInGroup->GetGroup();
	uint32 xp;
	if(!pGroup) 
	{ 
		return;
	}

	//Get Highest Level Player, Calc Xp and give it to each group member
	Player *pHighLvlPlayer = NULL;
	Player *pGroupGuy = NULL;
	  int active_player_count=0;
	Player *active_player_list[MAX_GROUP_SIZE_RAID];//since group is small we can afford to do this ratehr then recheck again the whole active player set
	int total_level=0;
	float xp_mod = 1.0f;

/*	if(pGroup->GetGroupType() == GROUP_TYPE_RAID)
	{   //needs to change
		//Calc XP
		xp = CalculateXpToGive(pVictim, PlayerInGroup);
		xp /= pGroup->MemberCount();

		GroupMembersSet::iterator itr;
		for(uint32 i = 0; i < pGroup->GetSubGroupCount(); i++)
		{
			for(itr = pGroup->GetSubGroup(i)->GetGroupMembersBegin(); itr != pGroup->GetSubGroup(i)->GetGroupMembersEnd(); ++itr)
			{
				if((*itr)->getLevel() < sWorld.LevelCap)
					(*itr)->GiveXP(xp, pVictim->GetGUID(), true);
			}
		}
	}
	else if(pGroup->GetGroupType() == GROUP_TYPE_PARTY) */
	//change on 2007 04 22 by Zack
	//we only take into count players that are near us, on same map
	GroupMembersSet::iterator itr;
	pGroup->Lock();
	for(uint32 i = 0; i < pGroup->GetSubGroupCount(); i++) {
		for(itr = pGroup->GetSubGroup(i)->GetGroupMembersBegin(); itr != pGroup->GetSubGroup(i)->GetGroupMembersEnd(); ++itr)
		{
			pGroupGuy = (*itr)->m_loggedInPlayer;
			if( pGroupGuy && 
				pGroupGuy->isAlive() && 
//				PlayerInGroup->GetInstanceID()==pGroupGuy->GetInstanceID() &&
				pVictim->GetMapMgr() == pGroupGuy->GetMapMgr() && 
				pGroupGuy->GetDistanceSq(pVictim)<100*100
				)
			{
				active_player_list[active_player_count]=pGroupGuy;
				active_player_count++;
				total_level += pGroupGuy->getLevel();
				if(pHighLvlPlayer)
				{
					if(pGroupGuy->getLevel() > pHighLvlPlayer->getLevel())
						pHighLvlPlayer = pGroupGuy;
				}
				else 
					pHighLvlPlayer = pGroupGuy;
			}
		}
	}
	pGroup->Unlock();
	if(active_player_count<1) //killer is always close to the victim. This should never execute
	{
		if(PlayerInGroup == 0)
		{
			PlayerInfo * pleaderinfo = pGroup->GetLeader();
			if(!pleaderinfo->m_loggedInPlayer)
			{ 
				return;
			}

			PlayerInGroup = pleaderinfo->m_loggedInPlayer;
		}

		xp = CalculateXpToGive(pVictim, PlayerInGroup);
		PlayerInGroup->GiveXP(xp, pVictim->GetGUID(), true);
	}
	else
	{
		if( pGroup->GetGroupType() == GROUP_TYPE_PARTY)
		{
			if(active_player_count==3)
				xp_mod=1.1666f;
			else if(active_player_count==4)
				xp_mod=1.3f;
			else if(active_player_count==5)
				xp_mod=1.4f;
			else xp_mod=1;//in case we have only 2 members ;)
		}
		else if(pGroup->GetGroupType() == GROUP_TYPE_RAID)
			xp_mod=0.5f;

		if(pHighLvlPlayer == 0)
		{
			PlayerInfo * pleaderinfo = pGroup->GetLeader();
			if(!pleaderinfo->m_loggedInPlayer)
			{ 
				return;
			}

			pHighLvlPlayer = pleaderinfo->m_loggedInPlayer;
		}

		xp = CalculateXpToGive(pVictim, pHighLvlPlayer);
		//i'm not sure about this formula is correct or not. Maybe some brackets are wrong placed ?
		for(int i=0;i<active_player_count;i++)
		{
			Player * plr = active_player_list[i];
			uint32 giveXpToPlayer = float2int32( ( ( xp * plr->getLevel() ) / total_level ) * xp_mod);
			plr->GiveXP(giveXpToPlayer , pVictim->GetGUID(), true );

			if( sEventMgr.ModifyEventTimeLeft(active_player_list[i],EVENT_LASTKILLWITHHONOR_FLAG_EXPIRE,20000) == false )
				sEventMgr.AddEvent(SafeUnitCast(active_player_list[i]),&Unit::EventAurastateExpire,(uint32)AURASTATE_LASTKILLWITHHONOR,EVENT_LASTKILLWITHHONOR_FLAG_EXPIRE,20000,1,0);
			if( active_player_list[i]->HasAuraStateFlag(AURASTATE_LASTKILLWITHHONOR) == false )
				active_player_list[i]->SetAuraStateFlag(AURASTATE_LASTKILLWITHHONOR); //got used up by a spell before expire

			if ( plr->GetSummon() && plr->GetSummon()->CanGainXP() )
			{
				uint32 pet_xp = (int)(CalculateXpToGive( pVictim, plr->GetSummon() ) * xp_mod); // vojta: this isn't blizzlike probably but i have no idea, feel free to correct it
				if ( pet_xp > 0 )
					plr->GetSummon()->GiveXP( pet_xp );
			}
		}
	}
		/* old code start before 2007 04 22
		GroupMembersSet::iterator itr;
		for(uint32 i = 0; i < pGroup->GetSubGroupCount(); i++)
		{
			for(itr = pGroup->GetSubGroup(i)->GetGroupMembersBegin(); itr != pGroup->GetSubGroup(i)->GetGroupMembersEnd(); ++itr)
			{
				pGroupGuy = (*itr);
				if(pGroupGuy)
				{
					if(pHighLvlPlayer)
					{
						if(pGroupGuy->getLevel() > pHighLvlPlayer->getLevel())
						{
							pHighLvlPlayer = pGroupGuy;
						}
					}
					else
						pHighLvlPlayer = pGroupGuy;
				}
			}
		}

		//Calc XP
		xp = CalculateXpToGive(pVictim, pHighLvlPlayer);
		uint32 giveXP = 0;

		for(uint32 i = 0; i < pGroup->GetSubGroupCount(); i++)
		{
			for(itr = pGroup->GetSubGroup(i)->GetGroupMembersBegin(); itr != pGroup->GetSubGroup(i)->GetGroupMembersEnd(); ++itr)
			{
				pGroupGuy = (*itr);
				giveXP = xp * pGroupGuy->getLevel() / (pHighLvlPlayer->getLevel() + pGroupGuy->getLevel());
				if(pGroupGuy->getLevel() < sWorld.LevelCap)
					pGroupGuy->GiveXP(giveXP, pVictim->GetGUID(), true);
			}
		}
	}*/
}

//needed cause memory menager crashes for external allocations :(
void Unit::RegisterScriptProcStruct( ProcTriggerSpell &pts )
{
	ProcTriggerSpell *ptsc = new ProcTriggerSpell( pts );	//core allocation instead of external one ?
	if( ptsc->origId == ptsc->spellId )
		ptsc->origId = 1; //dummy spells because these procs are not created by spell but script
	m_procSpells.push_front( ptsc );
	MarkHaveProcEvent( ptsc->procFlags );
}

uint32 Unit::HandleProc( uint32 flag, Unit* victim, SpellEntry* CastingSpell, int32 *dmg, int32 *Abs )
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	//why would anyone call it without flags ? Well ..ask "strike"
	if( flag == 0 )
		return 0;
	if( ( m_procSpellsHaveTheseFlags & flag ) == 0 )
		return 0;
//	if( IsPlayer() && SafePlayerCast(this)->m_onTaxi == true )
//		return;
	++m_procCounter; // !!! do not forget to decrease this before exit !!
	/* hmm whats a reasonable value here */
	if( m_procCounter > 3 )
	{
		/* something has proceed over 10 times in a loop :/ dump the spellids to the crashlog, as the crashdump will most likely be useless. */
		// BURLEX FIX ME!
		//OutputCrashLogLine("HandleProc %u SpellId %u (%s) %u", flag, spellId, sSpellStore.LookupString(sSpellStore.LookupEntry(spellId)->Name), m_procCounter);
		--m_procCounter;
		return 0;
	}

	uint32 now_in_ms = getMSTime();
	uint32 resisted_dmg = 0;
	ProcHandlerContextShare context;
	context.in_Caller = this;
	context.in_Victim = victim;
	context.in_CastingSpell = CastingSpell;
	if( dmg )
	{
		context.in_dmg = *dmg;
		context.in_dmg_loc = dmg;
	}
	else
	{
		context.in_dmg = 0;
		context.in_dmg_loc = NULL;
	}
	if( Abs )
	{
		context.in_abs = *Abs;
		context.in_abs_loc = Abs;
	}
	else
	{
		context.in_abs = 0;
		context.in_abs_loc = NULL;
	}
	context.out_dmg_absorb = 0;
	context.in_events_filter_flags = flag;

	uint32 ParamFilterFlags = flag & PROC_EVENT_FILTER_FLAGS;
	uint32 ParamEventFlags = flag & PROC_EVENT_PROC_FLAGS;

	ProcTriggerSpell	*itr2;
	CommitPointerListNode<ProcTriggerSpell> *itr;
	m_procSpells.BeginLoop();
	for(itr = m_procSpells.begin();itr != m_procSpells.end();itr = itr->Next() )
	{
		itr2 = itr->data;
		if( itr2->deleted )
		{
			if( m_procCounter == 1 )	//if this is a nested proc then we should have this set to TRUE by the father proc
				m_procSpells.SafeRemove( itr, 1 );
			continue;
		}

		//if we killed target with a proc, then do not double proc to do more dmg. Avoid duel kills ?
//		if( this->isAlive() == false || victim->isAlive() == false )
//			break;

		//flags can be mixed, we test them in mixed way. Maybe we should handle each one ?
		if( (itr2->procFlags & ParamEventFlags ) == 0 )
			continue;
		//test for must have flags
		uint32 SpellRequiresFilterFlags = ( itr2->procFlags & PROC_EVENT_FILTER_FLAGS );
		if( ( SpellRequiresFilterFlags & ParamFilterFlags ) != SpellRequiresFilterFlags )
			continue;

		if( CastingSpell != NULL )
		{
			//this is to avoid spell proc on spellcast loop. We use dummy that is same for both spells
			//if( CastingSpell->Id == itr2->spellId )
			if( 
//				CastingSpell->Id == itr2->origId || // seed of corruption and pyroclasm proc on self 
				CastingSpell->Id == itr2->spellId 
//				&& (CastingSpell->c_is_flags & SPELL_FLAG_IS_PROC_TRIGGER_PROC) == 0	//some rare spells do not have a proc just get scripted. If we marked this spell then let him proc on self
				)
			{
				//printf("WOULD CRASH HERE ON PROC: CastingId: %u, OrigId: %u, SpellId: %u\n", CastingSpell->Id, itr2->origId, itr2->spellId);
				continue;
			}
		}

		uint32 spellId = itr2->spellId;
		if( !spellId )
			continue; //there are procs without spells ?
		

		//check if we can trigger due to time limitation
		if( itr2->procInterval )
		{
			if( itr2->LastTrigger + itr2->procInterval > now_in_ms )
				continue; //we can't trigger it yet.
		}

		//lookupentry returns first spell for wrong ids to avoid crash, there is no need for NULL check
		SpellEntry* spe  = dbcSpell.LookupEntry( spellId );

		uint32 origId = itr2->origId;

		if( origId == 0 )
			origId = itr2->spellId; // item procs have it like this !
		SpellEntry* ospinfo = dbcSpell.LookupEntry( origId );//no need to check if exists or not since we were not able to register this trigger if it would not exist :P

		if( spe->ProcOnNameHash[0] != 0 )
		{
			if( CastingSpell == NULL )
				continue;

			if( CastingSpell->NameHash != spe->ProcOnNameHash[0] &&
				CastingSpell->NameHash != spe->ProcOnNameHash[1] &&
				CastingSpell->NameHash != spe->ProcOnNameHash[2] )
				continue;
		}
		if( ospinfo->ProcOnNameHash[0] != 0 )
		{
			if( CastingSpell == NULL )
				continue;

			if( CastingSpell->NameHash != ospinfo->ProcOnNameHash[0] &&
				CastingSpell->NameHash != ospinfo->ProcOnNameHash[1] &&
				CastingSpell->NameHash != ospinfo->ProcOnNameHash[2] )
				continue;
		}

		uint32 proc_Chance = itr2->procChance;
		if( SM_Mods )
			SM_FIValue( SM_Mods->SM_FChanceOfSuccess, (int32*)&proc_Chance, ospinfo->GetSpellGroupType() );
#ifdef _DEBUG
		if( IsPlayer() )
		{
			int64 *AddDur = GetCreateIn64Extension( EXTENSION_ID_MOD_PROCCHANCE );
			proc_Chance = MAX( proc_Chance, *AddDur );
		}
		if( IsPet() )
		{
			int64 *AddDur = SafePetCast( this )->GetPetOwner()->GetCreateIn64Extension( EXTENSION_ID_MOD_PROCCHANCE );
			proc_Chance = MAX( proc_Chance, *AddDur );
		}
#endif
		uint32 RealProcChance100 = itr2->procSuccessCounter * 100 * 100 / itr2->procEventCounter;
		uint32 ExpectedEventSuccessInterval100 = 100 * proc_Chance;
		float  ChanceAdjustment = (float)ExpectedEventSuccessInterval100 / (float)RealProcChance100;
		if( ChanceAdjustment < 1.0f )
			ChanceAdjustment = ChanceAdjustment - 1.0f;

		itr2->procEventCounter++;
		int32 ChanceAdjustmentInt = float2int32( 10.0f * ChanceAdjustment ); //with N(500) events this should have time to adjust chance
		if( ChanceAdjustmentInt < -(int32)proc_Chance )
			ChanceAdjustmentInt = -( (int32)proc_Chance - 2 ); //leave 2% chance to fate
		proc_Chance = (int32)proc_Chance + ChanceAdjustmentInt;
		if( proc_Chance <= 0 )
			proc_Chance = 1;
		if( RandChance( proc_Chance ) == false )
			continue;

		int dmg_overwrite = 0, dmg_overwrite1 = 0, dmg_overwrite2 = 0;

		context.out_CastSpell = dbcSpell.LookupEntry( spellId );
		if( spe->ProcHandler )
		{
			context.in_event = itr2;
			context.in_OwnerSpell = ospinfo;
			context.out_handler_result = PROC_HANDLER_CONTINUE_TO_NEXT;
			context.out_dmg_absorb = 0;
			context.out_dmg_overwrite[ 0 ] = context.out_dmg_overwrite[ 1 ] = context.out_dmg_overwrite[ 2 ] = 0;
			spe->ProcHandler( &context );
			resisted_dmg += context.out_dmg_absorb;
			if( context.out_handler_result == PROC_HANDLER_CONTINUE_TO_NEXT )
			{
				//!!mark it as procced !! -> only if handler return is not early exited !!
//				if( itr2->LastTrigger <= now_in_ms - itr2->procInterval )	//avoid custom mods to this value
//					itr2->LastTrigger = now_in_ms; // consider it triggered
				itr2->procEventCounter--; //for some reason event was not good. Or maybe there is no reason to proc it at all
				continue;
			}
			dmg_overwrite = context.out_dmg_overwrite[ 0 ];
			dmg_overwrite1 = context.out_dmg_overwrite[ 1 ];
			dmg_overwrite2 = context.out_dmg_overwrite[ 2 ];
		}
		itr2->procSuccessCounter++;
		//Reset from time to time. If chance is 1% : sucess is 100 and event is 10000
		if( itr2->procSuccessCounter > 500 )
		{
			itr2->procSuccessCounter = 1;
			itr2->procEventCounter = 1;
		}

		//since we did not allow to remove auras like these with interrupt flag we have to remove them manually.
		if( itr2->procFlags2 & PROC2_REMOVEONUSE )
			RemoveAura( origId );

		//anti morron protection : do not cast the same spell that registered us. If we need to, solve it somehow else
		if( context.out_CastSpell == NULL || context.out_CastSpell->Id == itr2->origId )
			continue;

		SpellCastTargets targets( victim->GetGUID() );
		if( itr2->procFlags2 & PROC2_TARGET_SELF )
			targets.m_unitTarget = GetGUID();

		itr2->LastTrigger = now_in_ms; // consider it triggered

		Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		spell->Init(this, context.out_CastSpell ,true, NULL);
		spell->forced_basepoints[0] = dmg_overwrite;
		spell->forced_basepoints[1] = dmg_overwrite1;
		spell->forced_basepoints[2] = dmg_overwrite2;
		spell->ProcedOnSpell = CastingSpell;
		spell->pSpellId=origId;
//printf("!!!!! wil proc spell with target guid %u \n",(uint32)(targets.m_unitTarget) & LOWGUID_ENTRY_MASK);
		spell->prepare(&targets);
	}
	m_procSpells.EndLoopAndCommit();

	context.in_event = NULL;
	context.in_OwnerSpell = NULL;
	std::map<uint32,struct SpellCharge>::iterator iter,iter2;
	uint32 bulky_time = getMSTime();
	for( iter = m_chargeSpells.begin(); iter!= m_chargeSpells.end(); )
	{
		iter2 = iter;
		iter++;
		if(iter2->second.count)
		{
			//trying to fix proc spells that proc and get removed on same cycle : ex clearcasting procs on casting and gets removed on next casting
//			if( iter2->second.created_at_stamp >= bulky_time )
//				continue;
			if( iter2->second.GotCreatedInThisLoop == true )
			{
				iter2->second.GotCreatedInThisLoop = false;
				continue;
			}
			//if we procced these then we should not remove them
			if( CastingSpell == iter2->second.spe )
				continue;

			//flags can be mixed, we test them in mixed way. Maybe we should handle each one ?
			if( (iter2->second.ProcFlag & ParamEventFlags ) == 0 )
				continue;
			//test for must have flags
			uint32 SpellRequiresFilterFlags = ( iter2->second.ProcFlag & PROC_EVENT_FILTER_FLAGS );
			if( ( SpellRequiresFilterFlags & ParamFilterFlags ) != SpellRequiresFilterFlags )
				continue;

//			if((iter2->second.ProcFlag & flag))
			{
				//Fixes for spells that dont lose charges when dmg is absorbd
//				if( dmg == 0 && 0 != (iter2->second.ProcFlag & ( PROC_ON_CRIT_HIT_VICTIM | PROC_ON_PHYSICAL_ATTACK_VICTIM | PROC_ON_MELEE_ATTACK_VICTIM | PROC_ON_RANGED_CRIT_ATTACK_VICTIM | PROC_ON_RANGED_ATTACK_VICTIM | PROC_ON_ANY_DAMAGE_VICTIM | PROC_ON_MELEE_ATTACK | PROC_ON_RANGED_ATTACK | PROC_ON_RANGED_CRIT_ATTACK | PROC_ON_PHYSICAL_ATTACK | PROC_ON_CRIT_ATTACK))) 
//				if( dmg == 0 && 0 != (iter2->second.ProcFlag & ( PROC_ON_CRIT_HIT_VICTIM | PROC_ON_MELEE_ATTACK_VICTIM | PROC_ON_RANGED_CRIT_ATTACK_VICTIM | PROC_ON_RANGED_ATTACK_VICTIM | PROC_ON_ANY_DAMAGE_VICTIM | PROC_ON_MELEE_ATTACK | PROC_ON_RANGED_ATTACK | PROC_ON_RANGED_CRIT_ATTACK | PROC_ON_CRIT_ATTACK))) 
				if( dmg == 0 && 0 != (iter2->second.ProcFlag & ( PROC_ON_MELEE_ATTACK_EVENT|PROC_ON_RANGED_ATTACK_EVENT|PROC_ON_SPELL_HIT_EVENT))) 
					continue;

				if( iter2->second.spe->ChargeHandler )
				{
					context.out_handler_result = PROC_HANDLER_CONTINUE_TO_NEXT;
					iter2->second.spe->ChargeHandler( &context );
					if( context.out_handler_result == PROC_HANDLER_CONTINUE_TO_NEXT )
						continue;
				}

				if( iter2->second.SkipFirstEvents > 0 )
				{
					iter2->second.SkipFirstEvents--;
					continue;
				}

				sLog.outDebug("Removing aura %u as charge expanded,remaining charges %u",iter2->second.spe->Id,iter2->second.count);
				this->RemoveAura(iter2->second.spe->Id);
			}
		}
	}

	if( m_procCounter > 0 )
		--m_procCounter;
	if( m_procCounter == 0 )
	{
		//atomic list parsing - my ass
		for( iter = m_chargeSpells.begin(); iter!= m_chargeSpells.end();)
		{
			iter2=iter++;
			if( iter2->second.count <= 0 )
			{
				UnMarkHaveProcEvent( iter2->second.ProcFlag );
				m_chargeSpells.erase(iter2);
			}
		}
	}

	return resisted_dmg;
}

//damage shield is a triggered spell by owner to atacker
void Unit::HandleProcDmgShield(uint32 flag, Unit* attacker)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	//make sure we do not loop dmg procs
	if(this==attacker || !attacker)
	{ 
		return;
	}
	//charges are already removed in handleproc
	sStackWorldPacket(data,SMSG_SPELLDAMAGESHIELD,40);
	std::list<DamageProc>::iterator i;
	std::list<DamageProc>::iterator i2;
	std::list<DamageProc> tlist = m_damageShields;	//cause dealdamage might change the list !
	uint32 ParamFilterFlags = flag & PROC_EVENT_FILTER_FLAGS;
	uint32 ParamEventFlags = flag & PROC_EVENT_PROC_FLAGS;
	for(i = tlist.begin();i != tlist.end();)     // Deal Damage to Attacker
	{
		i2 = i++; //we should not proc on proc.. not get here again.. not needed.Better safe then sorry.
		if(	(ParamEventFlags & (*i2).m_flags) )
		{
			uint32 SpellRequiresFilterFlags = ( (*i2).m_flags & PROC_EVENT_FILTER_FLAGS );
			if( ( SpellRequiresFilterFlags & ParamFilterFlags ) != SpellRequiresFilterFlags )
				continue;
/*			if(PROC_SCRIPTED_PROCTEST & (*i2).m_flags)
			{
				data.Initialize(SMSG_SPELLDAMAGESHIELD);
				data << this->GetGUID();
				data << attacker->GetGUID();
				data << (*i2).m_spellId;
				data << (*i2).m_damage;
				data << uint32(attacker->GetOverkill((*i2).m_damage));
				data << ( 1 << (*i2).m_school );
				data << uint32( 0 );	//found in 420
#ifdef MINIMIZE_NETWORK_TRAFIC
				SendMessageToDuel( &data );
#else
				SendMessageToSet(&data,true);
#endif
				int32 abs = attacker->ResilianceAbsorb( (*i2).m_damage, dbcSpell.LookupEntryForced( (*i2).m_spellId ), this );
				int32 dmg_to_deal = (*i2).m_damage - abs;
				this->DealDamage(attacker,(uint32)dmg_to_deal,(*i2).m_spellId);
			}
			else */
			{
				SpellEntry	*ability=dbcSpell.LookupEntry((*i2).m_spellId);
				this->Strike( attacker, RANGED, ability, 0, 0, (*i2).m_damage, true, true );//can dmg shields miss at all ?
			}
		}
	}
}

/*
void Unit::HandleProcSpellOnSpell(Unit* Victim,uint32 damage,bool critical)
{
}
*/
bool Unit::isCasting()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	return (m_currentSpell != NULL);
}

bool Unit::IsInInstance()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	MapInfo *pMapinfo = WorldMapInfoStorage.LookupEntry(this->GetMapId());
	if(pMapinfo)
	{ 
		return ( IS_MAP_INSTANCE( pMapinfo ) );
	}

	return false;
}

void Unit::RegenerateHealth()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	m_H_regenTimer = 2000;//set next regen time 

	if(!isAlive() || GetUInt32Value( UNIT_FIELD_HEALTH ) == 0 )
	{ 
		return;
	}

	// player regen
	if(this->IsPlayer())
	{
		// These only NOT in combat
		if(!CombatStatus.IsInCombat())
		{
			SafePlayerCast( this )->RegenerateHealth(false);
		}
		else
			SafePlayerCast( this )->RegenerateHealth(true);
	}
	else
	{
		// Only regen health out of combat
		if(!CombatStatus.IsInCombat())
			SafeCreatureCast(this)->RegenerateHealth();
	}
}

void Unit::RegeneratePower(bool isinterrupted)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE

	m_P_regenTimer = 2000;//set next regen time 

	if(!isAlive())
	{ 
		return;
	}

	// player regen
	if(this->IsPlayer())
	{
		uint32 powertype = GetPowerType();
		switch(powertype)
		{
		case POWER_TYPE_FOCUS:
			m_P_regenTimer = 500;	//needs to be small for better client server sync
			SafePlayerCast( this )->RegenerateFocus(isinterrupted);
			break;
		case POWER_TYPE_MANA:
			m_P_regenTimer = MANA_REGEN_INTERVAL_SERVER_SECONDS_USAGE * 1000;	//large value will make client side : out of mana errors
			SafePlayerCast( this )->RegenerateMana(isinterrupted);
			break;
		case POWER_TYPE_ENERGY:
			m_P_regenTimer = 250;	//needs to be small for better clietn server sync
			SafePlayerCast( this )->RegenerateEnergy();
			break;

		case POWER_TYPE_RAGE:
			{
				// These only NOT in combat
				if(!CombatStatus.IsInCombat())
				{
					m_P_regenTimer = 3000;
					SafePlayerCast( this )->LooseRage( 30 - SafePlayerCast( this )->m_ModInterrMRegen );
				}
				else
				{
					m_P_regenTimer = 3000;
					if( SafePlayerCast( this )->m_ModInterrMRegen )
						SafePlayerCast( this )->LooseRage( -( SafePlayerCast( this )->m_ModInterrMRegen ) );
				}

			}break;
		case POWER_TYPE_RUNIC:
			{
				m_P_regenTimer = 1000; 
				//death runes get converted through spell and do not regenerate
				if( IsPlayer() )
				{
					SafePlayerCast(this)->RegenerateRunes();
					SafePlayerCast(this)->RegenerateRunicPower();
				}
			}break;
		}
		//secondary power type regens
		switch(getClass())
		{
			case DRUID:
				{
					// druids regen mana when shapeshifted
					if( powertype != POWER_TYPE_MANA )
						SafePlayerCast( this )->RegenerateMana( isinterrupted );
					SafePlayerCast( this )->RegenerateEclipsePower();
				}break;
			case PALADIN:
				{
					SafePlayerCast( this )->RegenerateHolyPower();
				}break;
			case WARLOCK:
				{
					SafePlayerCast( this )->RegenerateSoulPower();
				}break;
		}
	}
	else
	{
		uint32 powertype = GetPowerType();
		switch(powertype)
		{
		case POWER_TYPE_MANA:
			if( IsPet() )
				SafePetCast(this)->RegenerateMana();
			else
				SafeCreatureCast(this)->RegenerateMana();
			break;
		case POWER_TYPE_FOCUS:
			SafeCreatureCast(this)->RegenerateFocus();
			break;
		case POWER_TYPE_ENERGY:
			SafeCreatureCast(this)->RegenerateEnergy();
			break;
		}
	}
}

float Unit::CalculateResistanceReductionPCT(Unit *pVictim, dealdamage *dmg, SpellEntry* ability, int32 eff_index)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE

	//bleed damage is skipping resistance check
	if( ability && 
		( ( eff_index < MAX_SPELL_EFFECT_COUNT && ability->eff[ eff_index ].EffectMechanic == MECHANIC_BLEEDING )
		 || ability->MechanicsType == MECHANIC_BLEEDING ) )
		return 0.0f;

	if( dmg->school_type == SCHOOL_NORMAL)//physical
	{		
		float weapon_target_armor_ignore_mod;
		float ArmorReduce = TargetResistRedModFlat[ SCHOOL_NORMAL ];
		Player *pOwner;
		if( this->IsPlayer() )
			pOwner = SafePlayerCast( this );
		else if( this->IsPet() )
			pOwner = SafePetCast( this )->GetPetOwner();
		else
			pOwner = NULL;

		if( pOwner )
		{
			weapon_target_armor_ignore_mod = pOwner->weapon_target_armor_pct_ignore;
			//seems like client gets bigger values somehow. Like 20% boost
			weapon_target_armor_ignore_mod -= pOwner->CalcRating( PLAYER_RATING_MODIFIER_ARMOR_PENETRATION_RATING ) / 100.0f;
//printf("!!!!!!!!weapon_target_armor_pct_ignore = %f\n",weapon_target_armor_pct_ignore);
			//noob protection, somhow armor penetration was too strong and making players too weak
//			if( weapon_target_armor_ignore_mod <= 0.40f )
//				weapon_target_armor_ignore_mod = 0.40f;
		}
		else
			weapon_target_armor_ignore_mod = 1.0f;

		if( pVictim->GetExtension( EXTENSION_ID_AURA_PENETRATE_VAL ) )
		{
			Aura *a = pVictim->HasAuraWithType( SPELL_AURA_MOD_DMG_PENETRATE, GetGUID(), AURA_SEARCH_NEGATIVE ); //only from this caster !
			if( a )
				for( uint32 i=0;i<a->m_modcount;i++)
					if( a->m_modList[i].m_AuraName == SPELL_AURA_MOD_DMG_PENETRATE )
						weapon_target_armor_ignore_mod -= ( a->m_modList[i].m_amount / 100.0f );
//			weapon_target_armor_ignore_mod -= SafePlayerCast( pVictim )->m_dmg_pct_penetrate / 100.0f;
		}

		//bugprotection
		if( weapon_target_armor_ignore_mod > 1.0f )
			weapon_target_armor_ignore_mod = 1.0f;	//penetration cannot increase armor. WTF bug ?
		//not needed to be checked. we will quit if armor gets negative
//		else if( weapon_target_armor_ignore_mod < 0.0f )
//			weapon_target_armor_ignore_mod = 0.0f;	//fully penetrated

		float victim_res = pVictim->GetResistance( SCHOOL_NORMAL ) * weapon_target_armor_ignore_mod;

		if( ArmorReduce >= victim_res )		// fully penetrated :O
		{ 
			return 0.0f;
		}

//		double Reduction = double(pVictim->GetResistance(0)) / double(pVictim->GetResistance(0)+400+(85*getLevel()));
		//dmg reduction formula from xinef
		float Reduction = 0;
		victim_res = victim_res - ArmorReduce;

		if( getLevel() > 80 )
			Reduction = float(victim_res) / float( victim_res + 306.64857f * getLevel() );
		else if( getLevel() > 60 ) 
			Reduction = float(victim_res) / float(victim_res + 467.5f * getLevel() - 22167.5f);
		else 
			Reduction = float(victim_res) / float(victim_res + 85.f * getLevel() + 400.f);

//		if(Reduction > 0.75f) 
//			Reduction = 0.75f;
		if(Reduction > 0.85f) 
			Reduction = 0.85f;
		if( Reduction > 0 )
			return Reduction;
	}
	else
	{
		// applying resistance to other type of damage 
		int32 RResist;
		float AverageResistance;
		if( pVictim->getLevel() > getLevel() )
			RResist = float2int32( float(pVictim->GetResistance( dmg->school_type ) + ( pVictim->getLevel() - this->getLevel()) * 5 ) - TargetResistRedModFlat[dmg->school_type] ); 
		else 
			RResist = float2int32( float(pVictim->GetResistance( dmg->school_type )) - TargetResistRedModFlat[dmg->school_type] ); 
		if( RResist < 0 )
			RResist = 0;
		AverageResistance = (float)( RResist ) / (float)( getLevel() * 6.9295f + RResist );
		if( AverageResistance > 0.85f )
			AverageResistance = 0.85f;
		if( AverageResistance > 0 )
			return AverageResistance;
	}
	return 0.0f;
}

int32 Unit::CalculateResistanceReduction(Unit *pVictim, dealdamage *dmg, SpellEntry* ability, int32 eff_index)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	int32 RemainingDMG = dmg->full_damage - dmg->absorbed_damage - dmg->resisted_damage;
	if( RemainingDMG <= 0 )
		return 0;
	float AverageResistance = 0.0f;
	float ArmorReduce;
	int32 reduction = 0;

	//bleed damage is skipping resistance check
	if( ability && 
		( ( eff_index < MAX_SPELL_EFFECT_COUNT && ability->eff[ eff_index ].EffectMechanic == MECHANIC_BLEEDING )
		 || ability->MechanicsType == MECHANIC_BLEEDING ) )
		return 0;

	if( dmg->school_type == SCHOOL_NORMAL)//physical
	{		
		float weapon_target_armor_ignore_mod;
		ArmorReduce = TargetResistRedModFlat[0];
		if(this->IsPlayer())
		{
			weapon_target_armor_ignore_mod = SafePlayerCast( this )->weapon_target_armor_pct_ignore;
			//seems like client gets bigger values somehow. Like 20% boost
			weapon_target_armor_ignore_mod -= SafePlayerCast( this )->CalcRating( PLAYER_RATING_MODIFIER_ARMOR_PENETRATION_RATING ) / 100.0f;
//printf("!!!!!!!!weapon_target_armor_pct_ignore = %f\n",weapon_target_armor_pct_ignore);
			//noob protection, somhow armor penetration was too strong and making players too weak
//			if( weapon_target_armor_ignore_mod <= 0.40f )
//				weapon_target_armor_ignore_mod = 0.40f;
		}
		else
			weapon_target_armor_ignore_mod = 1.0f;

		if( pVictim->GetExtension( EXTENSION_ID_AURA_PENETRATE_VAL ) )
		{
			Aura *a = pVictim->HasAuraWithType( SPELL_AURA_MOD_DMG_PENETRATE, GetGUID(), AURA_SEARCH_NEGATIVE );
			if( a )
				for( uint32 i=0;i<a->m_modcount;i++)
					if( a->m_modList[i].m_AuraName == SPELL_AURA_MOD_DMG_PENETRATE )
						weapon_target_armor_ignore_mod -= ( a->m_modList[i].m_amount / 100.0f );
//			weapon_target_armor_ignore_mod -= SafePlayerCast( pVictim )->m_dmg_pct_penetrate / 100.0f;
		}

		//bugprotection
		if( weapon_target_armor_ignore_mod > 1.0f )
			weapon_target_armor_ignore_mod = 1.0f;	//penetration cannot increase armor. WTF bug ?
		//not needed to be checked. we will quit if armor gets negative
//		else if( weapon_target_armor_ignore_mod < 0.0f )
//			weapon_target_armor_ignore_mod = 0.0f;	//fully penetrated

		float victim_res = pVictim->GetResistance( SCHOOL_NORMAL ) * weapon_target_armor_ignore_mod;

		if( ArmorReduce >= victim_res )		// fully penetrated :O
		{ 
			return 0;
		}

//		double Reduction = double(pVictim->GetResistance(0)) / double(pVictim->GetResistance(0)+400+(85*getLevel()));
		//dmg reduction formula from xinef
		float Reduction = 0;
		victim_res = victim_res - ArmorReduce;

		if( getLevel() > 80 )
//			Reduction = float(victim_res) / float(victim_res + 2167.5f * getLevel() - 158167.5f);
//			Reduction = float(victim_res) / float(victim_res + 2882.11f * getLevel() - 158167.5f);
//			Reduction = float(victim_res) / float( victim_res + 306.64857f * getLevel() );
			Reduction = float(victim_res) / float( victim_res + 270.64857f * getLevel() ); //QQ adjust number 1
		else if( getLevel() > 60 ) 
			Reduction = float(victim_res) / float(victim_res + 467.5f * getLevel() - 22167.5f);
		else 
			Reduction = float(victim_res) / float(victim_res + 85.f * getLevel() + 400.f);

		if(Reduction > 0.75f) 
			Reduction = 0.75f;
		if( Reduction > 0 )
//			dmg[0].resisted_damage += (uint32)(dmg[0].full_damage*Reduction);
			reduction = float2int32( RemainingDMG * Reduction );
	}
	else
	{
		// applying resistance to other type of damage 
//		int32 RResist = float2int32( float(pVictim->GetResistance( (*dmg).school_type ) + ((pVictim->getLevel() > getLevel()) ? (pVictim->getLevel() - this->getLevel()) * 5 : 0)) - TargetResistRedModFlat[(*dmg).school_type] ); 
		int32 RResist;
		if( pVictim->getLevel() > getLevel() )
			RResist = float2int32( float(pVictim->GetResistance( dmg->school_type ) + (pVictim->getLevel() - this->getLevel()) * 5 ) - TargetResistRedModFlat[dmg->school_type] ); 
		else 
			RResist = float2int32( float(pVictim->GetResistance( dmg->school_type )) - TargetResistRedModFlat[dmg->school_type] ); 
		if( RResist < 0 )
			RResist = 0;
//		AverageResistance = (float)(RResist) / (float)(getLevel() * 5.0f * 0.75f);
//		AverageResistance = (float)( 100.0f*RResist ) / (float)( getLevel() * 6.9295f + RResist );
		AverageResistance = (float)( RResist ) / (float)( getLevel() * 6.9295f + RResist );
		if( AverageResistance > 0.85f )
			AverageResistance = 0.85f;

		// NOT WOWWIKILIKE but i think it's actual to add some fullresist chance frome resistances
/*		if (!ability || !(ability->Attributes & ATTRIBUTES_IGNORE_INVULNERABILITY))
		{
			float Resistchance = (float)pVictim->GetResistance( (*dmg).school_type)/(float)pVictim->getLevel();
			Resistchance *= Resistchance;
			if( IsPlayer() )
			{
				Resistchance -= SafePlayerCast( this )->CalcRating( PLAYER_RATING_MODIFIER_SPELL_HIT );
				Resistchance -= SafePlayerCast( this )->GetHitFromSpell();
			}

			if( ability )
				Resistchance += pVictim->m_resist_hit[ MOD_SPELL ];

			if( Resistchance > 0 && Rand( Resistchance ) )
				AverageResistance = 1.0f;
		}*/	
		if( AverageResistance > 0 )
			reduction = float2int32( RemainingDMG * AverageResistance );
	}
	//maybe gather from here and there and add resiliance here ? Right now it is found in : dealdamage
//	dmg[0].full_damage -= reduction;	  
	return reduction;
}

uint32 Unit::GetSpellDidHitResult( Unit* pVictim, uint32 weapon_damage_type, SpellEntry* ability )
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	Item * it = NULL;
	float hitchance          = 0.0f;
	float dodge				 = 0.0f;
	float parry				 = 0.0f;
	float block				 = 0.0f;

	float hitmodifier		 = 0;
	int32 self_skill;
	int32 victim_skill;
//	uint32 SubClassSkill	 = SKILL_UNARMED;

	bool backAttack			 = !pVictim->isInFront( this ); // isInBack is bugged!
	uint32 vskill            = 0;

	Player *p_attacker;
	if( IsPlayer() )
		p_attacker = SafePlayerCast( this );
	else if( IsPet() )
		p_attacker = SafePetCast( this )->GetPetOwner();
	else
		p_attacker = NULL;

	Player *p_victim;
	if( pVictim->IsPlayer() )
		p_victim = SafePlayerCast( pVictim );
	else if( pVictim->IsPet() )
		p_victim = SafePetCast( pVictim )->GetPetOwner();
	else
		p_victim = NULL;

	//==========================================================================================
	//==============================Victim Skill Base Calculation===============================
	//==========================================================================================
	if( p_victim )
	{
		vskill = p_victim->_GetSkillLineCurrent( SKILL_DEFENSE );
		if( !backAttack )
		{
			if ( weapon_damage_type != RANGED )
			{
				dodge = p_victim->GetDodgeChance();
				if(pVictim->can_parry && !disarmed)
					parry = p_victim->GetParryChance();
			}
			// Is an offhand equipped and is it a shield?
			if( ( p_victim->disarmed & DISARM_TYPE_FLAG_OFFHAND ) == 0  )
			{
				Item* it2 = p_victim->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_OFFHAND );
				if( it2 != NULL && it2->GetProto()->InventoryType == INVTYPE_SHIELD )
					block = p_victim->GetBlockChance();
			}
		}
		victim_skill = float2int32( vskill + floorf( p_victim->CalcRating( PLAYER_RATING_MODIFIER_DEFENCE ) ) );

	}
	//--------------------------------mob defensive chances-------------------------------------
	else
	{
		if( weapon_damage_type != RANGED && !backAttack )
			dodge = pVictim->GetUInt32Value(UNIT_FIELD_STAT1) / 14.5f; // what is this value?
		victim_skill = pVictim->getLevel() * 5;
		if( pVictim->IsCreature() ) 
		{ 
			Creature * c = SafeCreatureCast(pVictim);
			if(c&&c->GetCreatureInfo()&&c->GetCreatureInfo()->Rank == ELITE_WORLDBOSS)
				victim_skill = std::max(victim_skill,((int32)this->getLevel()+3)*5); //used max to avoid situation when lowlvl hits boss.
		} 
	}
	//==========================================================================================
	//==============================Attacker Skill Base Calculation=============================
	//==========================================================================================
	if( p_attacker )
	{	  
		self_skill = 0;
		hitmodifier = p_attacker->GetHitFromMeleeSpell();  

		switch( weapon_damage_type )
		{
		case MELEE:   // melee main hand weapon
			if( ( disarmed & DISARM_TYPE_FLAG_MAINHAND ) == 0 )
				it = p_attacker->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_MAINHAND );
			else
				it = NULL;
			hitmodifier += p_attacker->CalcRating( PLAYER_RATING_MODIFIER_MELEE_HIT );
			if( pVictim->IsPlayer() )
				hitmodifier -= SafePlayerCast( pVictim )->m_resist_hit[ MOD_MELEE ];
			self_skill = float2int32( p_attacker->CalcRating( PLAYER_RATING_MODIFIER_MELEE_MAIN_HAND_SKILL ) );
			break;
		case OFFHAND: // melee offhand weapon (dualwield)
			if( ( disarmed & DISARM_TYPE_FLAG_OFFHAND ) == 0 )
				it = p_attacker->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_OFFHAND );
			else
				it = NULL;
			hitmodifier += p_attacker->CalcRating( PLAYER_RATING_MODIFIER_MELEE_HIT );
			if( pVictim->IsPlayer() )
				hitmodifier -= SafePlayerCast( pVictim )->m_resist_hit[ MOD_MELEE ];
			self_skill = float2int32( p_attacker->CalcRating( PLAYER_RATING_MODIFIER_MELEE_OFF_HAND_SKILL ) );
			break;
		case RANGED:  // ranged weapon
			if( ( disarmed & DISARM_TYPE_FLAG_RANGED ) == 0 )
				it = p_attacker->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_RANGED );
			else
				it = NULL;
			hitmodifier += p_attacker->CalcRating( PLAYER_RATING_MODIFIER_RANGED_HIT );
			if( pVictim->IsPlayer() )
				hitmodifier -= SafePlayerCast( pVictim )->m_resist_hit[ MOD_RANGED ];
			self_skill = float2int32( p_attacker->CalcRating( PLAYER_RATING_MODIFIER_RANGED_SKILL ) );
			break;
		}

		// erm. some spells don't use ranged weapon skill but are still a ranged spell and use melee stats instead
		// i.e. hammer of wrath
		if( ability )
		{
			if( ability->NameHash == SPELL_HASH_HAMMER_OF_WRATH ) 
			{
				it = p_attacker->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_MAINHAND );
				hitmodifier += p_attacker->CalcRating( PLAYER_RATING_MODIFIER_MELEE_HIT );
				self_skill = float2int32( p_attacker->CalcRating( PLAYER_RATING_MODIFIER_MELEE_MAIN_HAND_SKILL ) );
			}
			else if( ability->NameHash == SPELL_HASH_AVENGER_S_SHIELD ) 
			{
				self_skill = p_attacker->_GetSkillLineCurrent( SKILL_DEFENSE );
			}
		}
		self_skill += p_attacker->getLevel() * 5;
	}
	else
	{
		self_skill = this->getLevel() * 5;
		if(m_objectTypeId == TYPEID_UNIT) 
		{ 
			Creature * c = SafeCreatureCast(this);
			if(c&&c->GetCreatureInfo()&&c->GetCreatureInfo()->Rank == ELITE_WORLDBOSS)
				self_skill = std::max(self_skill,((int32)pVictim->getLevel()+3)*5);//used max to avoid situation when lowlvl hits boss.
		} 
	}
	//==========================================================================================
	//==============================Special Chances Base Calculation============================
	//==========================================================================================
	//<THE SHIT> to avoid Linux bug. 
	float diffVcapped = (float)self_skill;
	if(int32(pVictim->getLevel()*5)>victim_skill)
		diffVcapped -=(float)victim_skill;
	else
		diffVcapped -=(float)(pVictim->getLevel()*5);

	float diffAcapped = (float)victim_skill;
	if(int32(this->getLevel()*5)>self_skill)
		diffAcapped -=(float)self_skill;
	else
		diffAcapped -=(float)(this->getLevel()*5);
	//<SHIT END>

	//--------------------------------by victim state-------------------------------------------
	if(pVictim->IsPlayer()&&pVictim->GetStandState()) //every not standing state is >0
	{
		hitchance = 100.0f;
	}
	//--------------------------------by damage type and by weapon type-------------------------
	if( weapon_damage_type == RANGED ) 
	{
		dodge=0.0f;
		parry=0.0f;
	}

	//--------------------------------by skill difference---------------------------------------
	float vsk = (float)self_skill - (float)victim_skill;
	dodge = std::max( 0.0f, dodge - vsk * 0.04f );
	if( parry )
		parry = std::max( 0.0f, parry - vsk * 0.04f );
	if( block )
		block = std::max( 0.0f, block - vsk * 0.04f );

	if( vsk > 0 )
		hitchance = std::max( hitchance, 95.0f + vsk * 0.02f + hitmodifier );
	else
	{
		if( pVictim->IsPlayer() )
			hitchance = std::max( hitchance, 95.0f + vsk * 0.1f + hitmodifier ); //wowwiki multiplier - 0.04 but i think 0.1 more balanced
		else
			hitchance = std::max( hitchance, 100.0f + vsk * 0.6f + hitmodifier ); //not wowwiki but more balanced
	}

	if( SM_Mods && ability && (ability->c_is_flags & SPELL_FLAG_IS_REQUIRING_SM_MOD) )
	{
		SM_FFValue( SM_Mods->SM_FHitchance, &hitchance, ability->GetSpellGroupType() );
#ifdef COLLECTION_OF_UNTESTED_STUFF_AND_TESTERS
		float spell_flat_modifers=0;
		SM_FFValue(SM_FHitchance,&spell_flat_modifers,ability->GetSpellGroupType());
		if(spell_flat_modifers!=0 )
			printf("!!!!!spell resist mod flat %f,  spell resist bonus %f, spell group %u\n",spell_flat_modifers,hitchance,ability->GetSpellGroupType());
#endif
	}

	if( ( ability && ability->Attributes & ATTRIBUTES_CANT_BE_DPB )
		|| pVictim->m_stunned > 0 )
	{
		dodge = 0.0f;
		parry = 0.0f;
		block = 0.0f;
	}
	else
	{
		//wowwiki says the sum of dodge + parry + block < 102. We should scale them accordingly
	}

	//unconditional hit avoidance
	if( pVictim->IsPlayer() && SafePlayerCast( pVictim )->m_deflect > 0.0f && RandChance( SafePlayerCast( pVictim )->m_deflect ) )
		return SPELL_DID_HIT_MISS;

	float Roll = RandomFloat(100.0f);
	float miss_chance = MAX(0.0f,100.0f-hitchance);
	float chances[4];
	chances[0]=miss_chance;
	chances[1]=chances[0]+dodge;
	chances[2]=chances[1]+parry;
	chances[3]=chances[2]+block;
	static const uint32 roll_results[5] = { SPELL_DID_HIT_MISS,SPELL_DID_HIT_DODGE,SPELL_DID_HIT_DEFLECT,SPELL_DID_HIT_BLOCK,SPELL_DID_HIT_SUCCESS };


#ifdef USE_DIMINISHING_FOR_DODGE_BLOCK_MISS_PARRY
	float chancesBase[4];
	chancesBase[0]=miss_chance;
	chancesBase[1]=dodge;
	chancesBase[2]=parry;
	chancesBase[3]=block;
	uint32 r = 0;
	// apply diminishing returns 
	// not really what we want but similar
	// we actually diminish the time when the victim can avoid us. No time means we can hit him
	// wowwiki says that cap is around 80%, that means that every 3rd hit should be working on target
	// note that this is inverse thinking, attacker is eating up the dodge chance over time of a victim. Each attacker has it's own timer
	{
		while (r<4)
		{
			if( Roll<=chances[r] && chancesBase[r] != 0 )
			{
				//do diminishmisses
				if(  r < 3 )
				{
					uint32 rawduration;
					if( weapon_damage_type == MELEE )
						rawduration = 10000000 / ( 1 + GetUInt32Value( UNIT_FIELD_BASEATTACKTIME ) ); //slow attacks pass through more quickly : 10 / 2 = 5-2-hit ; 10 / 1 = 10 - 5 - 2 - hit
					else if( weapon_damage_type == OFFHAND )
						rawduration = 10000000 / ( 1 + GetUInt32Value( UNIT_FIELD_BASEATTACKTIME_1 ) );  //slow attacks pass through more quickly : 10 / 2 = 5-2-hit ; 10 / 1 = 10 - 5 - 2 - hit
					else if( weapon_damage_type == RANGED )
						rawduration = 10000000 / ( 1 + GetUInt32Value( UNIT_FIELD_RANGEDATTACKTIME ) ); //slow attacks pass through more quickly : 10 / 2 = 5-2-hit ; 10 / 1 = 10 - 5 - 2 - hit
					else
						rawduration = 2000;
					//after X hits we should be able to hit at least once
					ApplyDiminishingReturnTimer( &rawduration, this, DIMINISHING_GROUP_MISS + r );	//because it is cumulative
					if( rawduration != 0 )
						break;
				}
				else
					break;
			}
			r++;
		}
	}
	return roll_results[r];
#else
	//==========================================================================================
	//==============================One Roll Processing=========================================
	//==========================================================================================
	//--------------------------------cummulative chances generation----------------------------

	//--------------------------------roll------------------------------------------------------
	uint32 r = 0;
	while (r<4&&Roll>chances[r])
	{
		r++;
	}
	return roll_results[r];
#endif
}

void Unit::Strike( Unit* pVictim, uint32 weapon_damage_type, SpellEntry* ability, int32 add_damage, int32 pct_dmg_mod, uint32 exclusive_damage, bool disable_proc, bool skip_hit_check )
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
//==========================================================================================
//==============================Unacceptable Cases Processing===============================
//==========================================================================================
	if(!pVictim->isAlive() || !isAlive()  || IsStunned() || IsPacified() || IsFeared())
	{ 
		return;
	}

	if( skip_hit_check == false && IsPlayer() )
	{
		bool ability_is_AOE = ( ability != NULL && ( ability->c_is_flags2 & SPELL_FLAG2_IS_ENEMY_AOE_TARGETTING ) != 0 );
		if( ability_is_AOE == false && !isInFront(pVictim) )
		{
			SafePlayerCast( this )->GetSession()->OutPacket(SMSG_ATTACKSWING_BADFACING);
			return;
		}
	}

//==========================================================================================
//==============================Variables Initialization====================================
//==========================================================================================
	dealdamage dmg;
	if( ability )
		dmg.school_type = ability->School;
	
	Item * it = NULL;
	
	float hitchance          = 0.0f;
	float dodge				 = 0.0f;
	float parry				 = 0.0f;
	float glanc              = 0.0f;
	float block				 = 0.0f;
	float crit				 = 0.0f;
	float crush              = 0.0f;

	uint32 hit_status		 = 0;

	int32 blocked_damage	 = 0;
	int32  realdamage		 = 0;

	uint32 vstate			 = 1;
	uint32 aproc			 = PROC_ON_NO_CRIT | PROC_ON_NO_DOT; //will remove this flag if we crit
	   
	float hitmodifier		 = 0;
	int32 self_skill;
	int32 victim_skill;
//	uint32 SubClassSkill	 = SKILL_UNARMED;

	bool backAttack			 = !pVictim->isInFront( this );
	uint32 vskill            = 0;
	
	if(ability)
	{
		dmg.school_type = ability->School;
		if( ability->quick_tickcount > 1 )
		{
			aproc &= ~PROC_ON_NO_DOT;
			aproc |= PROC_ON_DOT;
		}
	}
	else
	{
		if ( IsCreature() )
			dmg.school_type = SafeCreatureCast( this )->BaseAttackType;
		else
			dmg.school_type = SCHOOL_NORMAL;
	}

	Player *p_attacker;
	if( IsPlayer() )
		p_attacker = SafePlayerCast( this );
	else if( IsPet() )
		p_attacker = SafePetCast( this )->GetPetOwner();
	else
		p_attacker = NULL;

	Player *p_victim;
	if( pVictim->IsPlayer() )
		p_victim = SafePlayerCast( pVictim );
	else if( pVictim->IsPet() )
		p_victim = SafePetCast( pVictim )->GetPetOwner();
	else
		p_victim = NULL;

//==========================================================================================
//==============================Victim Skill Base Calculation===============================
//==========================================================================================
	if( p_victim )
	{
		vskill = p_victim->_GetSkillLineCurrent( SKILL_DEFENSE );

		if( !backAttack )
		{
			if ( weapon_damage_type != RANGED )
			{
				dodge = p_victim->GetDodgeChance();
				if(pVictim->can_parry && !disarmed)
					parry = p_victim->GetParryChance();
			}
			// Is an offhand equipped and is it a shield?
			if( ( p_victim->disarmed & DISARM_TYPE_FLAG_OFFHAND ) == 0  )
			{
				Item* it2 = p_victim->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_OFFHAND );
				if( it2 != NULL && it2->GetProto()->InventoryType == INVTYPE_SHIELD )
					block = p_victim->GetBlockChance();
			}
		}
		victim_skill = float2int32( vskill + floorf( p_victim->CalcRating( PLAYER_RATING_MODIFIER_DEFENCE ) ) );
	}
//--------------------------------mob defensive chances-------------------------------------
	else if( pVictim->IsCreature() )
	{
		// not a player, must be a creature
		Creature* c = SafeCreatureCast( pVictim );
		// mobs can dodge attacks from behind
		if ( weapon_damage_type != RANGED )
		{
			dodge = pVictim->GetUInt32Value( UNIT_FIELD_AGILITY ) / 14.5f;
			dodge += pVictim->GetDodgeFromSpell();
		}

		if ( !backAttack && !disarmed )
		{
			parry = c->GetBaseParry();
			parry += pVictim->GetParryFromSpell();
		}

		victim_skill = pVictim->getLevel() * 5;
		if( c->GetCreatureInfo() && c->GetCreatureInfo()->Rank == ELITE_WORLDBOSS )
			victim_skill = std::max( victim_skill, ( (int32)getLevel() + 3 ) * 5 ); //used max to avoid situation when lowlvl hits boss.
	}
	//i know i know we already set the attack timer, but let's try to make sure anyway
	//not sure if this breaks anything but seems like DK's have insane DMG for some reason. Maybe they melee strike after ability = insane DPS ?
	if( ability == NULL )
		setAttackTimer(0,weapon_damage_type==OFFHAND);
//	if( weapon_damage_type == RANGED && IsPlayer() )
//		SafePlayerCast( this )->m_AutoShotAttackTimer = SafePlayerCast( this )->m_AutoShotDuration;
//==========================================================================================
//==============================Attacker Skill Base Calculation=============================
//==========================================================================================
	if( p_attacker )
	{	  
		self_skill = 0;

		hitmodifier = p_attacker->GetHitFromMeleeSpell();  
		
		switch( weapon_damage_type )
		{
		case MELEE:   // melee main hand weapon
			if( ( disarmed & DISARM_TYPE_FLAG_MAINHAND ) == 0 )
				it = p_attacker->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_MAINHAND );
			else
				it = NULL;
			self_skill = float2int32( p_attacker->CalcRating( PLAYER_RATING_MODIFIER_MELEE_MAIN_HAND_SKILL ) );
			if (it && it->GetProto() && ability == NULL )
				dmg.school_type = it->GetProto()->dmg_type;
			crit = p_attacker->GetFloatValue(PLAYER_CRIT_PERCENTAGE);
			break;
		case OFFHAND: // melee offhand weapon (dualwield)
			if( ( disarmed & DISARM_TYPE_FLAG_OFFHAND ) == 0 )
				it = p_attacker->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_OFFHAND );
			else
				it = NULL;
			self_skill = float2int32( p_attacker->CalcRating( PLAYER_RATING_MODIFIER_MELEE_OFF_HAND_SKILL ) );
			hit_status |= HITSTATUS_DUALWIELD;//animation
			if (it && it->GetProto() && ability == NULL)
				dmg.school_type = it->GetProto()->dmg_type;
//			crit = p_attacker->GetFloatValue(PLAYER_OFFHAND_CRIT_PERCENTAGE);
			crit = p_attacker->GetFloatValue(PLAYER_CRIT_PERCENTAGE);
			break;
		case RANGED:  // ranged weapon
			if( ( disarmed & DISARM_TYPE_FLAG_RANGED ) == 0 )
				it = p_attacker->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_RANGED );
			else
				it = NULL;
			self_skill = float2int32( p_attacker->CalcRating( PLAYER_RATING_MODIFIER_RANGED_SKILL ) );
			if (it && it->GetProto() && ability == NULL)
				dmg.school_type = it->GetProto()->dmg_type;
			crit = p_attacker->GetFloatValue(PLAYER_RANGED_CRIT_PERCENTAGE);
			break;
		}
		self_skill += p_attacker->getLevel() * 5;
//		crit = p_attacker->GetFloatValue(PLAYER_CRIT_PERCENTAGE);
	}
	else
	{
		self_skill = this->getLevel() * 5;
		if( this->IsCreature() ) 
		{ 
			Creature * c = SafeCreatureCast(this);
			if( c->GetCreatureInfo() )
				self_skill += c->GetCreatureInfo()->Rank * 5;//used max to avoid situation when lowlvl hits boss.
		} 
		crit = 5.0f; //will be modified later
	}
//==========================================================================================
//==============================Special Chances Base Calculation============================
//==========================================================================================

//--------------------------------crushing blow chance--------------------------------------
	//http://www.wowwiki.com/Crushing_blow
	if( p_victim && !p_attacker && !ability && !dmg.school_type)
	{
		int32 baseDefense = p_victim->_GetSkillLineCurrent(SKILL_DEFENSE, false);
		int32 skillDiff = self_skill - baseDefense;
		if ( skillDiff >= 15)
			crush = -15.0f + 2.0f * skillDiff; 
		else 
			crush = 0.0f;
	}
//--------------------------------glancing blow chance--------------------------------------
	//http://www.wowwiki.com/Glancing_blow
	// did my own quick research here, seems base glancing against equal level mob is about 5%
	// and goes up 5% each level. Need to check this further.
	float diffAcapped = victim_skill - std::min((float)self_skill, getLevel() * 5.0f);

	if( p_attacker && !p_victim && !ability)
	{
		glanc = 5.0f + diffAcapped;
		if(glanc<0)
			glanc = 0.0f;
	}
//==========================================================================================
//==============================Advanced Chances Modifications==============================
//==========================================================================================
//--------------------------------by talents------------------------------------------------
	if( pVictim->IsPlayer() )
	{
		if( weapon_damage_type != RANGED )
		{
			crit += SafePlayerCast(pVictim)->res_M_crit_get();
			hitmodifier += SafePlayerCast(pVictim)->m_resist_hit[ MOD_MELEE ];
		}
		else 
		{
			crit += SafePlayerCast(pVictim)->res_R_crit_get(); //this could be ability but in that case we overwrite the value
			hitmodifier += SafePlayerCast(pVictim)->m_resist_hit[ MOD_RANGED ];
		}
	}
	
	if( IsCreature() )
		crit += SafeCreatureCast(this)->crit_chance_mod;

	crit += (float)(pVictim->AttackerCritChanceMod[SCHOOL_NORMAL]);
//--------------------------------by skill difference---------------------------------------

	float vsk = (float)(self_skill - victim_skill);
	dodge = std::max( 0.0f, dodge - vsk * 0.04f );
	if( parry )
		parry = std::max( 0.0f, parry - vsk * 0.04f );
	if( block )
		block = std::max( 0.0f, block - vsk * 0.04f );

	crit += pVictim->IsPlayer() ? vsk * 0.04f : min( vsk * 0.2f, 0.0f ); 

	// http://www.wowwiki.com/Miss
	float misschance;
	float ask = -vsk;
	if (pVictim->IsPlayer())
	{
		if (ask > 0)
			misschance = ask * 0.04f;
		else
			misschance = ask * 0.02f;
	}
	else
	{
		if (ask <= 10)
			misschance = ask * 0.1f;
		else
			misschance = 2 + (ask - 10) * 0.4f;
	}
	hitchance = 100.0f - misschance; // base miss chances are worked out further down

	if( ability )
	{
		if( ability->EffectCanCrit[ 0 ] == true ) //hmm, should know properly the effect we are using. Need to add it to "strike" as param
		{
			if( Need_SM_Apply( ability ) )
			{
				if( this->SM_Mods )
				{
					SM_FFValue(SM_Mods->SM_FCriticalChance,&crit,ability->GetSpellGroupType());
					SM_FFValue(SM_Mods->SM_FHitchance,&hitchance,ability->GetSpellGroupType());
				}
				if( pVictim->SM_Mods )
					SM_FFValue(pVictim->SM_Mods->SM_FCriticalChanceTarget, &crit, ability->GetSpellGroupType());
#ifdef COLLECTION_OF_UNTESTED_STUFF_AND_TESTERS
				float spell_flat_modifers=0;
				SM_FFValue(SM_FCriticalChance,&spell_flat_modifers,ability->GetSpellGroupType());
				if(spell_flat_modifers!=0)
					printf("!!!!spell critchance mod flat %f ,spell group %u\n",spell_flat_modifers,ability->GetSpellGroupType());
#endif
			}
		//--------------------------------by ratings------------------------------------------------
		//	crit -= pVictim->IsPlayer() ? SafePlayerCast(pVictim)->CalcRating( PLAYER_RATING_MODIFIER_MELEE_CRIT_RESILIENCE ) * 2 : 0.0f;
			if( ability && ability->CritHandler != NULL )
			{
				ProcHandlerContextShare context;
				memset( &context, 0, sizeof( ProcHandlerContextShare ) );
				context.in_Caller = this;
				context.in_Victim = pVictim;
				context.out_dmg_overwrite[0] = context.in_dmg = float2int32( crit );
				ability->CritHandler( &context );
				crit = (float)context.out_dmg_overwrite[0];
			}
		}
		else
			crit =0;
	}
	if( crit <= 0 ) 
		crit=0.0f;
	else
	{
		switch( weapon_damage_type )
		{
			case MELEE:   // melee main hand weapon
			case OFFHAND: // melee offhand weapon (dualwield)
			{
				float EffectiveCritchance = 100.0f * ( (float)EventsMeleeCrit / (float)EventsMeleeOverall );
				crit = crit * ( crit / EffectiveCritchance ) ; // cause 50 already gives the feeling it is proccing all the time
				EventsMeleeOverall++;
			}break;
			case RANGED:  // ranged weapon
			{
				float EffectiveCritchance = 100.0f * ( (float)EventsRangedCrit / (float)EventsRangedOverall );
				crit = crit * ( crit / EffectiveCritchance ) ; // cause 50 already gives the feeling it is proccing all the time
				EventsRangedOverall++;
			}break;
		}
	}
	if( p_attacker )
	{
		hitmodifier += (weapon_damage_type == RANGED) ? p_attacker->CalcRating( PLAYER_RATING_MODIFIER_RANGED_HIT ) : p_attacker->CalcRating( PLAYER_RATING_MODIFIER_MELEE_HIT );
//		dodge -= p_attacker->CalcRating( PLAYER_RATING_MODIFIER_MELEE_HIT_AVOIDANCE ); // EXPERTISE will give you expertise skill, EXPERTISE2 gives you the % bonus from the rating
//		float Expertise = p_attacker->CalcRating( PLAYER_RATING_MODIFIER_EXPERTISE );
		float Expertise = p_attacker->GetUInt32Value( PLAYER_EXPERTISE ) / 4; //this is blizzlike
		dodge -= Expertise;
		if(dodge<0) 
			dodge=0.0f;
		parry -= Expertise;
		if(parry<0) 
			parry=0.0f;
	}
	

//--------------------------------by damage type and by weapon type-------------------------
	if( weapon_damage_type == RANGED ) 
	{
		dodge=0.0f;
		parry=0.0f;
		glanc=0.0f;
	}

	if( this->IsPlayer() )
	{
//		it = SafePlayerCast( this )->GetItemInterface()->GetInventoryItem( EQUIPMENT_SLOT_OFFHAND );
		if ( !ability && it != NULL
//		 && (it->GetProto()->InventoryType == INVTYPE_WEAPON || it->GetProto()->InventoryType == INVTYPE_WEAPONOFFHAND)
			&& m_dualWield == true
			)
			// offhand weapon can either be a 1 hander weapon or an offhander weapon
			hitmodifier -= 24.0f; //dualwield miss chance
		else
			hitmodifier -= 5.0f; // base miss chance
	}
	else
	{
		// mobs base hit chance
		hitmodifier -= 5.0f;
	}

	hitchance += hitmodifier;

	//Hackfix for Surprise Attacks
	if(  this->IsPlayer() && ability && SafePlayerCast( this )->m_finishingmovesdodge && ( ability->c_is_flags & SPELL_FLAG_IS_FINISHING_MOVE) )
		dodge = 0.0f;

	if( skip_hit_check )
	{
		hitchance = 100.0f;
		dodge = 0.0f;
		parry = 0.0f;
		block = 0.0f;
	}

	if( ( ability != NULL && ( ability->Attributes & ATTRIBUTES_CANT_BE_DPB ) )
		|| pVictim->m_stunned > 0 )
	{
		dodge = 0.0f;
		parry = 0.0f;
		block = 0.0f;
	}

//--------------------------------by victim state-------------------------------------------
	if( pVictim->IsPlayer() && pVictim->GetStandState() ) //every not standing state is >0
	{
		hitchance = 100.0f;
		dodge = 0.0f;
		parry = 0.0f;
		block = 0.0f;
		crush = 0.0f;
		crit = 100.0f;
	}
	if(backAttack)
	{
		if(pVictim->IsPlayer())
		{
			//However mobs can dodge attacks from behind
			dodge = 0.0f;
		}
		parry = 0.0f;
		block = 0.0f;
	}
//==========================================================================================
//==============================One Roll Processing=========================================
//==========================================================================================
//--------------------------------cummulative chances generation----------------------------
	float miss_chance = MAX(0.0f,100.0f-hitchance);
	float Roll = RandomFloat(100.0f);
	uint32 r = 0;
	float chances[7];
	chances[0]=miss_chance;
	chances[1]=chances[0]+dodge;
	chances[2]=chances[1]+parry;
	chances[3]=chances[2]+glanc;
	chances[4]=chances[3]+block;
	chances[5]=chances[4]+crit;
	chances[6]=chances[5]+crush;
#ifdef USE_DIMINISHING_FOR_DODGE_BLOCK_MISS_PARRY
	float chancesBase[7];
	chancesBase[0]=miss_chance;
	chancesBase[1]=dodge;
	chancesBase[2]=parry;
	chancesBase[3]=glanc;
	chancesBase[4]=block;
	chancesBase[5]=crit;
	chancesBase[6]=crush;
	// apply diminishing returns 
	// not really what we want but similar
	// we actually diminish the time when the victim can avoid us. No time means we can hit him
	// wowwiki says that cap is around 80%, that means that every 3rd hit should be working on target
	// note that this is inverse thinking, attacker is eating up the dodge chance over time of a victim. Each attacker has it's own timer
	{
		while (r<5)
		{
			if( Roll<=chances[r] && chancesBase[r] != 0 )
			{
				//do diminishmisses
				if(  r < 3 )
				{
					uint32 rawduration;
					if( weapon_damage_type == MELEE )
						rawduration = 10000000 / ( 1 + GetUInt32Value( UNIT_FIELD_BASEATTACKTIME ) ); //slow attacks pass through more quickly : 10 / 2 = 5-2-hit ; 10 / 1 = 10 - 5 - 2 - hit
					else if( weapon_damage_type == OFFHAND )
						rawduration = 10000000 / ( 1 + GetUInt32Value( UNIT_FIELD_BASEATTACKTIME_1 ) );  //slow attacks pass through more quickly : 10 / 2 = 5-2-hit ; 10 / 1 = 10 - 5 - 2 - hit
					else if( weapon_damage_type == RANGED )
						rawduration = 10000000 / ( 1 + GetUInt32Value( UNIT_FIELD_RANGEDATTACKTIME ) ); //slow attacks pass through more quickly : 10 / 2 = 5-2-hit ; 10 / 1 = 10 - 5 - 2 - hit
					else
						rawduration = 2000;
					//after X hits we should be able to hit at least once
					ApplyDiminishingReturnTimer( &rawduration, this, DIMINISHING_GROUP_MISS + r );	//because it is cumulative
					if( rawduration != 0 )
						break;
				}
				else
					break;
			}
			r++;
		}
		while (r<7&&Roll>chances[r])
			r++;
	}
#else
	//printf("%s:-\n", IsPlayer() ? "Player" : "Mob");
	//printf(" miss: %.2f\n", chances[0]);
	//printf("dodge: %.2f\n", dodge);
	//printf("parry: %.2f\n", parry);
	//printf("glanc: %.2f\n", glanc);
	//printf("block: %.2f\n", block);
	//printf(" crit: %.2f\n", crit);
	//printf("crush: %.2f\n", crush);

//--------------------------------roll------------------------------------------------------
	while (r<7&&Roll>chances[r])
		r++;
#endif
	//does not depend on facing or diminishing returns
	if( pVictim->IsPlayer() && SafePlayerCast( pVictim )->m_deflect > 0.0f && RandChance( SafePlayerCast( pVictim )->m_deflect ) )
		r = 0; //miss this target

	//critical with scaled values to give better statistics. Note that this does not monitor short interval chnages in crit chance
	if( r == 5 )
	{
		switch( weapon_damage_type )
		{
			case MELEE:   // melee main hand weapon
			case OFFHAND: // melee offhand weapon (dualwield)
				EventsMeleeCrit++;
				break;
			case RANGED:  // ranged weapon
				EventsRangedCrit++;
				break;
		}
	}

	int32 block_PCT_amt = 0;
//--------------------------------postroll processing---------------------------------------
	switch(r)
	{ 
//--------------------------------miss------------------------------------------------------
	case 0:
		hit_status |= HITSTATUS_MISS;
		aproc |= PROC_ON_DODGE_EVENT;
		vstate = DEFLECT; //?
		// dirty ai agro fix
		if(pVictim->GetTypeId() == TYPEID_UNIT && pVictim->GetAIInterface()->GetNextTarget() == NULL)
			pVictim->GetAIInterface()->AttackReaction(this, 1, 0);
//		Spell::SendSpellFail( this, pVictim, ability ? ability->Id:0, SPELL_DID_HIT_MISS );
		break;
//--------------------------------dodge-----------------------------------------------------
	case 1:
		hit_status |= HITSTATUS_MISS;
		// dirty ai agro fix
		if(pVictim->GetTypeId() == TYPEID_UNIT && pVictim->GetAIInterface()->GetNextTarget() == NULL)
			pVictim->GetAIInterface()->AttackReaction(this, 1, 0);

		CALL_SCRIPT_EVENT(pVictim, OnTargetDodged)(this);
		CALL_SCRIPT_EVENT(this, OnDodged)(this);
		vstate = DODGE;
		aproc |= PROC_ON_DODGE_EVENT;
		pVictim->Emote(EMOTE_ONESHOT_PARRYUNARMED);			// Animation
//		Spell::SendSpellFail( this, pVictim, ability ? ability->Id:0, SPELL_DID_HIT_DODGE );
		if( pVictim->IsPlayer() )
		{
			//allmighty warrior overpower
			if( this->IsPlayer() && SafePlayerCast( this )->getClass() == WARRIOR )
			{
				SafePlayerCast( this )->AddComboPoints( pVictim->GetGUID(), 1 );
				SafePlayerCast( this )->UpdateComboPoints();
				if( sEventMgr.ModifyEventTimeLeft( SafePlayerCast( this ), EVENT_COMBO_POINT_CLEAR_FOR_TARGET, 5000 ) == false )
					sEventMgr.AddEvent( SafePlayerCast( this ), &Player::NullComboPoints, (uint32)EVENT_COMBO_POINT_CLEAR_FOR_TARGET, (uint32)5000, (uint32)1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
			}
			if( sEventMgr.ModifyEventTimeLeft(pVictim,EVENT_DODGE_BLOCK_FLAG_EXPIRE,5000) == false )
				sEventMgr.AddEvent(pVictim,&Unit::EventAurastateExpire,(uint32)AURASTATE_DODGE_BLOCK,EVENT_DODGE_BLOCK_FLAG_EXPIRE,5000,1,0);
			if( pVictim->HasAuraStateFlag(AURASTATE_DODGE_BLOCK) == false )
				pVictim->SetAuraStateFlag(AURASTATE_DODGE_BLOCK);	//SB@L: Enables spells requiring dodge
		}
		break;
//--------------------------------parry-----------------------------------------------------
	case 2:
		hit_status |= HITSTATUS_MISS;
		// dirty ai agro fix
		if(pVictim->GetTypeId() == TYPEID_UNIT && pVictim->GetAIInterface()->GetNextTarget() == NULL)
			pVictim->GetAIInterface()->AttackReaction(this, 1, 0);

		CALL_SCRIPT_EVENT(pVictim, OnTargetParried)(this);
		CALL_SCRIPT_EVENT(this, OnParried)(this);
		vstate = PARRY;
		aproc |= PROC_ON_DODGE_EVENT;	//hmm, we should have special parry proc flag but as i seen these 2 go in same handler
		pVictim->Emote(EMOTE_ONESHOT_PARRYUNARMED);			// Animation
//		Spell::SendSpellFail( this, pVictim, ability ? ability->Id:0, SPELL_DID_HIT_DEFLECT );
		if(pVictim->IsPlayer())
		{
			if( sEventMgr.ModifyEventTimeLeft( pVictim, EVENT_PARRY_FLAG_EXPIRE, 5000 ) == false )
				sEventMgr.AddEvent( pVictim, &Unit::EventAurastateExpire, (uint32)AURASTATE_PARRY,EVENT_PARRY_FLAG_EXPIRE, 5000, 1, 0 );
			if( pVictim->HasAuraStateFlag( AURASTATE_PARRY ) == false )
				pVictim->SetAuraStateFlag( AURASTATE_PARRY );	//SB@L: Enables spells requiring parry
			if( sEventMgr.ModifyEventTimeLeft( pVictim, EVENT_DODGE_BLOCK_FLAG_EXPIRE, 5000 ) == false )
				sEventMgr.AddEvent( pVictim, &Unit::EventAurastateExpire, (uint32)AURASTATE_DODGE_BLOCK, EVENT_DODGE_BLOCK_FLAG_EXPIRE, 5000, 1, 0 );
			if( pVictim->HasAuraStateFlag( AURASTATE_DODGE_BLOCK ) == false )
				pVictim->SetAuraStateFlag( AURASTATE_DODGE_BLOCK );	//SB@L: Enables spells requiring dodge
		}
		break;
//--------------------------------not miss,dodge or parry-----------------------------------
	default:
		hit_status |= HITSTATUS_HITANIMATION;//hit animation on victim
		if( pVictim->SchoolImmunityAntiEnemy[ dmg.school_type ] || pVictim->SchoolImmunityAntiFriend[ dmg.school_type ] )
		{
			vstate = IMMUNE;		
			dmg.resisted_damage = dmg.full_damage = realdamage = 0;	//i know we already set these. But morron protection can never hurt
		}
		else
	{
//--------------------------------state proc initialization---------------------------------
			dmg_storage_struct tdmg;
			memset( &tdmg, 0, sizeof( tdmg ) );
			if( weapon_damage_type != RANGED )
			{
				aproc |= PROC_ON_MELEE_ATTACK_EVENT;
			}
			else
			{
				aproc |= PROC_ON_RANGED_ATTACK_EVENT;
//				if(ability && ability->Id==3018 && IsPlayer() && getClass()==HUNTER)
//					aproc |= PROC_ON_AUTO_SHOT_HIT;
			}
//--------------------------------base damage calculation-----------------------------------
			if(exclusive_damage)
				tdmg.base_min = tdmg.base_max = exclusive_damage;
			else
			{
				if( ability && ability->Spell_value_is_scripted == false )
					CalculateDamage( this, pVictim, weapon_damage_type, ability->GetSpellGroupType(), ability, tdmg );
				else			
					CalculateDamage( this, pVictim, weapon_damage_type, 0, ability, tdmg );
			}
			tdmg.pct_mod_add += 100;

			if(SM_Mods && ability && Need_SM_Apply( ability ))
			{	
//				SM_FIValue(((Unit*)this)->SM_FDamageBonus,&dmg.full_damage,ability->GetSpellGroupType());
//				SM_PIValue(((Unit*)this)->SM_PDamageBonus,&dmg.full_damage,ability->GetSpellGroupType());
				SM_FIValue(((Unit*)this)->SM_Mods->SM_FDamageBonus,&tdmg.flat_mod_add,ability->GetSpellGroupType());
				//weapon damage PCT boost is not aditive but multiplicative : use crusader strike to check ingame : 35395
				SM_FIValue(((Unit*)this)->SM_Mods->SM_PDamageBonus,&tdmg.pct_mod_add,ability->GetSpellGroupType());
#ifdef COLLECTION_OF_UNTESTED_STUFF_AND_TESTERS
				int spell_flat_modifers=0;
				int spell_pct_modifers=0;
				SM_FIValue(((Unit*)this)->SM_FDamageBonus,&spell_flat_modifers,ability->GetSpellGroupType());
				SM_FIValue(((Unit*)this)->SM_PDamageBonus,&spell_pct_modifers,ability->GetSpellGroupType());
				if(spell_flat_modifers!=0 || spell_pct_modifers!=0)
					printf("!!!!!spell dmg bonus mod flat %d , spell dmg bonus pct %d , spell dmg bonus %d, spell group %u\n",spell_flat_modifers,spell_pct_modifers,dmg.full_damage,ability->GetSpellGroupType());
#endif
			}
			tdmg.flat_mod_add += pVictim->DamageTakenMod[dmg.school_type];
			if( ability == NULL )
				tdmg.pct_mod_add = tdmg.pct_mod_add * MeleeDmgPCTMod / 100;
			else 
			{
				if( ability->c_is_flags2 & SPELL_FLAG2_IS_ENEMY_AOE_TARGETTING )
				{
					tdmg.pct_mod_add = tdmg.pct_mod_add * pVictim->AoeDamageTakenPctMod[ dmg.school_type ] / 100;
					if( IsCreature() )
						tdmg.pct_mod_add = tdmg.pct_mod_add * pVictim->AoeDamageTakenPctModFromCreatures / 100;
				}
				uint32 spell_effect_index = 0; //hmmm, need to find a way to make this work better
				//DK, Paladin, hunter
/*
Obliterate:
MH: ((AP/14)*2.4 + 650 + Weapon Damage)*1.6*(1-DR)*(1 + 0.04)*(1 + 3*0.15 + 0.2)*(1 + 2*0.125)*(1 + 0.1 + 0.05)
OH: ((AP/14)*2.4 + 650 + Weapon Damage)*1.6*(1-DR)*(1 + 0.04)*(1 + 3*0.15 + 0.2)*(1 + 2*0.125)*(1 + 0.1 + 0.05)*0.5*1.25

Explanation: Weapon Damage here is a random amount of damage from the weapon doing the strike. It's not normalized in any way. (AP/14)*2.4 is normalized to 2.4 weapon speed regardless of what speed the actual weapon is. 1.6 is the 160% weapon damage that obliterate does. DR here is damage reduction from armor. It varries from boss dummy to boss dummy but stayed in the 0.68 - 0.73 range or so. I'm not sure what it is or if it varies for actual raid bosses but it was easy to calculate on the dummy. (1 + 0.04) accounts for brittle bones assuming it is up. (1 + 2*0.125) accounts for diseases. Note, each disease increases damage done equally, though frost fever will appear to increase it more due to brittle bones. (1 + 3*0.15 + 0.2) accounts for 3/3 annihilation and glyph of obliterate. (1 + 0.1 + 0.05) accounts for frost presence and 2/2 improved frost presence. 0.5*1.25 accounts for the 50% damange reduction on the offhand, and 3/3 nerves of cold steel damage increase.

Blood Strike
MH: ((AP/14)*2.4 + 850 + Weapon Damage)*0.8*(1-DR)*(1 + 0.04)*(1 + 2*0.19)*(1 + 0.1 + 0.05)
OH: ((AP/14)*2.4 + 850 + Weapon Damage)*0.8*(1-DR)*(1 + 0.04)*(1 + 2*0.19)*(1 + 0.1 + 0.05)*0.5*1.25

Only thing special of note here is that for some reason I cannot explain, blood strike does around an extra 19% damage per disease. I verified this at multiple AP levels, even talentless and naked.

Frost Strike:
MH: ((AP/14)*2.4 + 248 + Weapon Damage)*1.1*(1 + 0.1 + 0.05)*(1 + 0.02*MasteryRating)
OH: ((AP/14)*2.4 + 248 + Weapon Damage)*1.1*(1 + 0.1 + 0.05)*(1 + 0.02*MasteryRating)*0.5*1.25

Howling Blast:
(AP*0.4 + Rand(1151,1251))*(1 + 0.1 + 0.05)*(1 + 0.02*MasteryRating)
The sim doesn't seem to roll for what value to use and instead defaults to 1201 every time. This is incorrect. I did not check if alternate targets are hit for 60% damage.

Plague Strike:
MH: ((AP/14)*2.4 + 421 + Weapon Damage)*1.0*(1-DR)*(1 + 0.04)*(1 + 0.1 + 0.05)
OH: ((AP/14)*2.4 + 421 + Weapon Damage)*1.0*(1-DR)*(1 + 0.04)*(1 + 0.1 + 0.05)*0.5*1.25
*/
				if( ability->AttackPowerToSpellDamageCoeff != 0.0f || ability->AP_spell_bonus_total[ spell_effect_index ] != 0.0f )
				{
					float AP = MAX( GetAP(), GetRAP() );
					float normalization;
					if( this->IsPlayer() && exclusive_damage == 0 ) //when it is not school_dmg
						normalization = SafePlayerCast( this )->GetWeaponNormalizationCoeffForAP( (WeaponDamageType)weapon_damage_type );
					else
						normalization = 1.0f;
					tdmg.flat_mod_add += float2int32( AP * ability->AttackPowerToSpellDamageCoeff * normalization );
					tdmg.flat_mod_final += float2int32( AP * ability->AP_spell_bonus_total[ spell_effect_index ] );
				}
			}

			//rogue talent Sanguinary Vein
			if( getClass() == ROGUE )
			{
				int64 *ext = (int64*)GetExtension( EXTENSION_ID_SANGUINARY_VEIN_VAL );
				if( ext )
				{
//					bool is_bleeding = (pVictim->HasAuraWithMechanic( 0, AURA_SEARCH_NEGATIVE, ( 1 << MECHANIC_BLEEDING ) )!=NULL);
//					if( is_bleeding )
					uint32 BleedCount = 0;
					for(uint32 x=MAX_POSITIVE_AURAS;x<MAX_NEGATIVE_AURAS1( pVictim );x++)
						if( pVictim->m_auras[x] && ( pVictim->m_auras[x]->GetSpellProto()->MechanicsTypeFlags & ( 1 << MECHANIC_BLEEDING ) ) )
							BleedCount++;
					if( BleedCount >= 1 )
						tdmg.pct_mod_add = tdmg.pct_mod_add * ( 100 + (int32)(*ext) ) / 100;
				}
			}

			if( weapon_damage_type == RANGED )
			{
				tdmg.flat_mod_add += pVictim->RangedDamageTaken;
				tdmg.pct_mod_add = tdmg.pct_mod_add * ( 100 + pVictim->RangedDamageTakenPct ) / 100;
			}
					
//			tdmg.pct_mod_base += pct_dmg_mod; //weapon damage PCT will get multiplied and not added to other PCT boosts

//			tdmg.flat_mod_add += add_damage;	//add damage is extra for spells like scourgestrike : does 40% weapon dmg + x

			tdmg.pct_mod_add = float2int32( tdmg.pct_mod_add * pVictim->DamageTakenPctMod[ dmg.school_type ] ) / 100;
			if( dmg.school_type != SCHOOL_NORMAL  //normal school is already included as base dmg scaler
				|| exclusive_damage != 0 ) //this comes from SPELL_EFFECT_SCHOOL_DAMAGE and does not contain weapon based PCT mod
				tdmg.pct_mod_add = float2int32( tdmg.pct_mod_add * GetDamageDonePctMod(dmg.school_type) );

//			if( dmg.school_type != SCHOOL_NORMAL )
//				tdmg.pct_mod_add += float2int32((GetDamageDonePctMod( dmg.school_type ) - 1) * 100.0f);

			if( ability != NULL && ability->MechanicsType < MECHANIC_TOTAL
//				&& ability->NameHash == SPELL_HASH_SHRED 
				)
			{
//				tdmg.pct_mod_add += float2int32(pVictim->ModDamageTakenByMechPCT[MECHANIC_BLEEDING]);
//				tdmg.flat_mod_add += pVictim->ModDamageTakenByMechFlat[MECHANIC_BLEEDING];
				//this should be ability mechanic and not spell mechanic :(
				//this will fail in all cases i guess :(
				tdmg.pct_mod_add = tdmg.pct_mod_add * ( 100 + pVictim->ModDamageTakenByMechPCT[ ability->MechanicsType ] ) / 100;
				tdmg.flat_mod_add += pVictim->ModDamageTakenByMechFlat[ ability->MechanicsType ];
			}

			//pet happiness state dmg modifier
/*			if( IsPet() && !SafePetCast(this)->IsSummon() )
			{
//				dmg.full_damage = ( dmg.full_damage <= 0 ) ? 0 : float2int32( dmg.full_damage * SafePetCast( this )->GetHappinessDmgMod() );
				tdmg.pct_mod_add += float2int32( SafePetCast( this )->GetHappinessDmgMod() * 100.0f);
			} */

			//should pct mod mod flat mod ? Not sure about this formula, but pct mods sould not mod each other for sure.
			int32 dmg_diff = tdmg.base_max - tdmg.base_min;
			int32 dmg_diff1 = 1 + dmg_diff * 20 / 100;
			int32 dmg_diff2 = dmg_diff * 40 / 100; //will never reach max dmg, but it will be much stable DPS
//			dmg.full_damage = tdmg.base_min + ( RandomUInt() % dmg_diff );
			dmg.full_damage = tdmg.base_min + dmg_diff2 + ( RandomUInt() % ( dmg_diff1 ) );
			dmg.full_damage += tdmg.flat_mod_add;	//flat mod is AP
			dmg.full_damage += add_damage;	//add damage is extra for spells like scourgestrike : does 40% weapon dmg + x
			dmg.full_damage += dmg.full_damage * pct_dmg_mod / 100;	//pct_mod_base is weapon damage mod
			dmg.full_damage = dmg.full_damage * tdmg.pct_mod_add / 100;
			dmg.full_damage += tdmg.flat_mod_final;

			if(dmg.full_damage < 0)
				dmg.full_damage = 0;
//--------------------------------check for special hits------------------------------------
			switch(r)
			{
//--------------------------------glancing blow---------------------------------------------
			case 3:
				{
					float low_dmg_mod = 1.5f - (0.05f * diffAcapped);
					if(this->getClass() == MAGE || this->getClass() == PRIEST || this->getClass() == WARLOCK) //casters = additional penalty.
					{
						low_dmg_mod -= 0.7f;
					}
					if(low_dmg_mod<0.01)
						low_dmg_mod = 0.01f;
					if(low_dmg_mod>0.91)
						low_dmg_mod = 0.91f;
					float high_dmg_mod = 1.2f - (0.03f * diffAcapped);
					if(this->getClass() == MAGE || this->getClass() == PRIEST || this->getClass() == WARLOCK) //casters = additional penalty.
					{
						high_dmg_mod -= 0.3f;
					}
					if(high_dmg_mod>0.99)
						high_dmg_mod = 0.99f;
					if(high_dmg_mod<0.2)
						high_dmg_mod = 0.2f;

					float damage_reduction = (high_dmg_mod + low_dmg_mod) / 2.0f;
					if(damage_reduction > 0)
					{
						dmg.full_damage = float2int32(damage_reduction * float(dmg.full_damage));
					}
					hit_status |= HITSTATUS_GLANCING;
				}
				break;
//--------------------------------block-----------------------------------------------------
			case 4:
				{
					Item* shield = p_victim->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_OFFHAND);
					if( shield != NULL )
					{
						if( shield->GetProto()->InventoryType == INVTYPE_SHIELD )
						{
//							uint32 block_multiplier = 100 + SafePlayerCast( pVictim )->m_modblockabsorbvaluePCT;
//							if( block_multiplier < 100 )
//								block_multiplier = 100;
//							blocked_damage = float2int32( shield->GetProto()->Block + SafePlayerCast( pVictim )->m_modblockvaluefromspells + pVictim->GetUInt32Value( PLAYER_RATING_MODIFIER_BLOCK ) )) + pVictim->GetUInt32Value( UNIT_FIELD_STAT0 ) / 2 )) ) * block_multiplier);
//							blocked_damage = ( shield->GetProto()->Block + SafePlayerCast( pVictim )->m_modblockvaluefromspells + pVictim->GetUInt32Value( UNIT_FIELD_STAT0 ) / 2 ) * block_multiplier);
							block_PCT_amt = p_victim->GetInt32Value( PLAYER_SHIELD_BLOCK );
							//check for critical block - this is a warrior mastery only ? wowwiki says anyone that can wear plate
							hit_status |= HITSTATUS_BLOCK;
							pVictim->Emote(EMOTE_ONESHOT_PARRYSHIELD);// Animation
							if( pVictim->IsPlayer() )
							{
								float crit_block_pct_chance = p_victim->GetFloatValue( PLAYER_SHIELD_BLOCK_CRIT_PERCENTAGE );
								int ExcessAvoidance = MAX( 0, dodge + parry + block - 100 ); //according to wowwiki the max for 4.3 client is 126.5%
								ExcessAvoidance = MIN( 50, ExcessAvoidance ); 
								if( RandChance( crit_block_pct_chance + ExcessAvoidance ) )
								{
									block_PCT_amt += 30;
									hit_status &= ~HITSTATUS_BLOCK;
									hit_status |= HITSTATUS_CRITICAL_BLOCK;
								}
							}						
							blocked_damage = block_PCT_amt * dmg.full_damage / 100;
						}
						else
						{
							blocked_damage = 0;
						}

						if(dmg.full_damage <= (int32)blocked_damage)
							vstate = BLOCK;
						if( blocked_damage )
						{
							CALL_SCRIPT_EVENT(pVictim, OnTargetBlocked)(this, blocked_damage);
							CALL_SCRIPT_EVENT(this, OnBlocked)(pVictim, blocked_damage);
							aproc |= PROC_ON_BLOCK_EVENT;
						}
						if( pVictim->IsPlayer() )//not necessary now but we'll have blocking mobs in future
						{   
							if( sEventMgr.ModifyEventTimeLeft(pVictim,EVENT_DODGE_BLOCK_FLAG_EXPIRE,5000) == false )
								sEventMgr.AddEvent(pVictim,&Unit::EventAurastateExpire,(uint32)AURASTATE_DODGE_BLOCK,EVENT_DODGE_BLOCK_FLAG_EXPIRE,5000,1,0);
							if( pVictim->HasAuraStateFlag(AURASTATE_DODGE_BLOCK) == false )
								pVictim->SetAuraStateFlag(AURASTATE_DODGE_BLOCK);	//SB@L: Enables spells requiring dodge
						}
					}
				}
				break;
//--------------------------------critical hit----------------------------------------------
			case 5:
			{
					hit_status |= HITSTATUS_CRICTICAL;
					int32 dmgbonus;
//					if( weapon_damage_type != RANGED )
//						dmgbonus = dmg.full_damage / 2;	//150% dmg
//					else
//						dmgbonus = dmg.full_damage;	//200% dmg
					//int 403 ? I could not get this confirmed 100% :(
//					if( weapon_damage_type != RANGED )
						dmgbonus = dmg.full_damage;	//200% dmg
//					else
//						dmgbonus = dmg.full_damage * 3 / 2;	//150% dmg
					//sLog.outString( "DEBUG: Critical Strike! Full_damage: %u" , dmg.full_damage );
					if(SM_Mods && ability && Need_SM_Apply( ability ) )
					{
						int32 dmg_bonus_pct = 100;
						SM_FIValue(SM_Mods->SM_PCriticalDamage,&dmg_bonus_pct,ability->GetSpellGroupType());
						dmgbonus = float2int32( float(dmgbonus) * (float(dmg_bonus_pct)/100.0f) );
					}
					
					//sLog.outString( "DEBUG: After CritMeleeDamageTakenPctMod: %u" , dmg.full_damage );
					if(IsPlayer())
					{
						if( weapon_damage_type != RANGED )
						{
							float critextra=float(SafePlayerCast( this )->m_modphyscritdmgPCT);
							dmg.full_damage += int32((float(dmg.full_damage)*critextra/100.0f));
						}
						if(!pVictim->IsPlayer())
							dmg.full_damage += float2int32(dmg.full_damage*SafePlayerCast( this )->IncreaseCricticalByTypePCT[(SafeCreatureCast(pVictim))->GetCreatureInfo() ? (SafeCreatureCast(pVictim))->GetCreatureInfo()->Type : 0]);
					//sLog.outString( "DEBUG: After IncreaseCricticalByTypePCT: %u" , dmg.full_damage );
					}

					dmg.full_damage += dmgbonus;

					if( weapon_damage_type == RANGED )
						dmg.full_damage = dmg.full_damage - float2int32(dmg.full_damage * CritRangedDamageTakenPctMod[dmg.school_type]);
					else 
						dmg.full_damage = dmg.full_damage - float2int32(dmg.full_damage * CritMeleeDamageTakenPctMod[dmg.school_type]);

/*					if(pVictim->IsPlayer())
					{
						//Resilience is a special new rating which was created to reduce the effects of critical hits against your character.
						float dmg_reduction_pct = 2.2f * SafePlayerCast(pVictim)->CalcRating( PLAYER_RATING_MODIFIER_MELEE_CRIT_RESILIENCE ) / 100.0f;
						if( dmg_reduction_pct > 0.8f )
							dmg_reduction_pct = 0.8f; //we cannot resist more then he is criticalling us, there is no point of the critical then :P
						int32 resi_red = float2int32( dmg.full_damage*dmg_reduction_pct );
						dmg.absorbed_damage += resi_red;
						dmg.full_damage = dmg.full_damage - resi_red;
						//sLog.outString( "DEBUG: After Resilience check: %u" , dmg.full_damage );
					} */
					
					if (pVictim->GetTypeId() == TYPEID_UNIT && SafeCreatureCast(pVictim)->GetCreatureInfo() && SafeCreatureCast(pVictim)->GetCreatureInfo()->Rank != ELITE_WORLDBOSS)
						pVictim->Emote( EMOTE_ONESHOT_WOUNDCRITICAL );
					aproc |= PROC_ON_CRIT;
					aproc &= ~PROC_ON_NO_CRIT;

					if( IsPlayer() || IsPet() )
					{
						if( sEventMgr.ModifyEventTimeLeft( this, EVENT_CRIT_FLAG_EXPIRE, 5000 ) == false )
							sEventMgr.AddEvent( this, &Unit::EventAurastateExpire, (uint32)AURASTATE_CRITICAL , EVENT_CRIT_FLAG_EXPIRE, 5000, 1, 0 );
						if( HasAuraStateFlag( AURASTATE_CRITICAL ) == false )
							SetAuraStateFlag( AURASTATE_CRITICAL );
					}

					CALL_SCRIPT_EVENT(pVictim, OnTargetCritHit)(this, float(dmg.full_damage));
					CALL_SCRIPT_EVENT(this, OnCritHit)(pVictim, float(dmg.full_damage));
				}
				break;
//--------------------------------crushing blow---------------------------------------------
			case 6:
				hit_status |= HITSTATUS_CRUSHINGBLOW;
				dmg.full_damage = (dmg.full_damage * 3) >> 1;
				break;
//--------------------------------regular hit-----------------------------------------------
			default:
				break;	
			}
//==========================================================================================
//==============================Post Roll Damage Processing=================================
//==========================================================================================
//--------------------------absorption------------------------------------------------------
			/*{
//--------------------------resiliance reducing--------------------------------------------------
				dmg.resisted_damage += pVictim->ResilianceAbsorb( dmg.full_damage - dmg.resisted_damage - dmg.absorbed_damage, ability, this );
//--------------------------armor reducing--------------------------------------------------
				dmg.absorbed_damage += pVictim->AbsorbDamage( dmg.school_type, dmg.full_damage - dmg.resisted_damage - dmg.absorbed_damage );
				//mana shield, only use it if there is any dmg left to reduce. Note that bubles work before armor or resistance
				dmg.absorbed_damage += pVictim->ManaShieldAbsorb( dmg.full_damage - dmg.resisted_damage - dmg.absorbed_damage );
				dmg.resisted_damage += CalculateResistanceReduction( pVictim, &dmg, ability);	
				if( dmg.absorbed_damage )
					vproc |= PROC_ON_ABSORB_VICTIM;
				realdamage = dmg.full_damage - dmg.resisted_damage - dmg.absorbed_damage - blocked_damage;
			}/**/
			{
//--------------------------armor reducing--------------------------------------------------
				float ResilianceReduceCoef = pVictim->ResilianceAbsorbPCT( this ); 
				float ArmorReduceCoef = CalculateResistanceReductionPCT( pVictim, &dmg, ability );
				float TotalResistCoef = 1.0f - MAX( 0.0f, MIN( 1.0f, ( 1.0f - ResilianceReduceCoef ) * ( 1.0f - ArmorReduceCoef ) * (( 100.0f - block_PCT_amt ) / 100.0f)));	
				dmg.resisted_damage += MAX( 0, ( dmg.full_damage - dmg.absorbed_damage ) ) * TotalResistCoef;
				dmg.absorbed_damage += pVictim->AbsorbDamage( dmg.school_type, dmg.full_damage - dmg.resisted_damage - dmg.absorbed_damage );
				//mana shield, only use it if there is any dmg left to reduce. Note that bubles work before armor or resistance
				dmg.absorbed_damage += pVictim->ManaShieldAbsorb( dmg.full_damage - dmg.resisted_damage - dmg.absorbed_damage );
				if( dmg.absorbed_damage )
					aproc |= PROC_ON_ABSORB_EVENT;
				realdamage = dmg.full_damage - dmg.resisted_damage - dmg.absorbed_damage - blocked_damage;
			}
		}
		break;
	}

//==========================================================================================
//==============================Post Roll Special Cases Processing==========================
//==========================================================================================
//------------------------------- Special Effects Processing
	// Paladin: Blessing of Sacrifice, and Warlock: Soul Link
	if( pVictim->m_damageSplitTarget )
	{
//		dmg.full_damage = pVictim->DoDamageSplitTarget(dmg.full_damage, dmg.school_type, true);
//		realdamage = dmg.full_damage;
		//zack : why remove spell efects impact ?
		int32 dmg_reamining = pVictim->DoDamageSplitTarget( realdamage, dmg.school_type, true );
		dmg.absorbed_damage += (realdamage - dmg_reamining);
		realdamage = dmg_reamining;
	}

	//--------------------------special states processing---------------------------------------
	if(pVictim->GetTypeId() == TYPEID_UNIT)
	{
		if( pVictim->GetAIInterface() && (pVictim->GetAIInterface()->getAIState()== STATE_EVADE 
//			|| (pVictim->GetAIInterface()->GetIsSoulLinked() && pVictim->GetAIInterface()->getSoullinkedWith() != this)
			))
		{
			vstate = EVADE;
			realdamage = 0;
			dmg.full_damage = 0;
			dmg.resisted_damage = 0;
		}
	}
	if(pVictim->bInvincible == true)
	{
		dmg.resisted_damage = dmg.full_damage; //godmode
		realdamage = 0;
	}
//--------------------------dirty fixes-----------------------------------------------------
	//vstate=1-wound,2-dodge,3-parry,4-interrupt,5-block,6-evade,7-immune,8-deflect	
	// the above code was remade it for reasons : damage shield needs moslty same flags as handleproc + dual wield should proc too ?
	if( !disable_proc 
//		&& weapon_damage_type != OFFHAND //Zack : but why disable this ?
		)
	{
		if( weapon_damage_type == OFFHAND )
			aproc |= PROC_OFFHAND_WEAPON;

		//damage shield must come before handleproc to not loose 1 charge : speel gets removed before last charge
		if( realdamage > 0 || ( aproc & PROC_ON_BLOCK_EVENT ) )
		{
			pVictim->HandleProcDmgShield( ( aproc | PROC_ON_VICTIM ),this);
			HandleProcDmgShield( ( aproc | PROC_ON_NOT_VICTIM ),pVictim);
		}

		int32 resisted_dmg;
		int32 param_abs = (dmg.resisted_damage + dmg.absorbed_damage);
		if( this != pVictim )
		{
			this->HandleProc( ( aproc | PROC_ON_NOT_VICTIM ),pVictim, ability, &dmg.full_damage, &param_abs); //maybe using dmg.resisted_damage is better sometimes but then if using godmode dmg is resisted instead of absorbed....bad
	//		if( realdamage > 0 )
	//			aproc |= PROC_ON_ANY_DAMAGE_DONE;
			resisted_dmg = pVictim->HandleProc( (aproc | PROC_ON_VICTIM),this, ability, &dmg.full_damage, &param_abs);
		}
		else
			resisted_dmg = pVictim->HandleProc( (aproc | PROC_ON_NOT_VICTIM | PROC_ON_VICTIM),this, ability, &dmg.full_damage, &param_abs);
		resisted_dmg += param_abs - (dmg.resisted_damage + dmg.absorbed_damage); 

		if( resisted_dmg > 0 )
		{
			dmg.absorbed_damage += resisted_dmg;	//using as temporary until figure out properlogs :(
			realdamage -= resisted_dmg;
		}
		realdamage = dmg.full_damage - dmg.resisted_damage - dmg.absorbed_damage - blocked_damage;
	}
	if( realdamage <= 0 || pVictim->SchoolImmunityAntiEnemy[ dmg.school_type ] || pVictim->SchoolImmunityAntiFriend[ dmg.school_type ] )
	{
		realdamage = 0;
		vstate = IMMUNE;
	}
//==========================================================================================
//==============================Data Sending================================================
//==========================================================================================
	if( !ability || realdamage == 0 )
	{
		if( dmg.resisted_damage != 0 )
			hit_status |= HITSTATUS_RESIST;
		if( dmg.absorbed_damage != 0 )
			hit_status |= HITSTATUS_ABSORBED;

		if( ability )
			dmg.SpellId = ability->spell_id_client;
		SendAttackerStateUpdate(this, pVictim, &dmg, realdamage, dmg.absorbed_damage, blocked_damage, hit_status, vstate);
	}
	else
	{
//		if( realdamage > 0 )//FIXME: add log for miss,block etc for ability and ranged
		{
			uint32 school_mask;
			if( dmg.school_type < SCHOOL_COUNT )
				school_mask = g_spellSchoolConversionTable[ dmg.school_type ];
			else
				school_mask = SCHOOL_NORMAL;
//			SendSpellNonMeleeDamageLog( this, pVictim, ability->Id, realdamage, dmg.school_type, dmg.resisted_damage, 0, false, blocked_damage, ( ( hit_status & HITSTATUS_CRICTICAL ) != 0 ), true );
			this->SendCombatLogMultiple( pVictim, realdamage, dmg.resisted_damage, dmg.absorbed_damage, blocked_damage, ability->spell_id_client, school_mask, COMBAT_LOG_SPELL_DAMAGE, ( ( hit_status & HITSTATUS_CRICTICAL ) != 0 ) );
		}
/*		else if(realdamage==0)
		{	
			if( vstate == DODGE )
				SendSpellLog( this, pVictim,ability->spell_id_client, SPELL_LOG_DODGE );
			else if( vstate == PARRY )
				SendSpellLog( this, pVictim,ability->spell_id_client, SPELL_LOG_PARRY );
			else if( vstate == EVADE )
				SendSpellLog( this, pVictim,ability->spell_id_client, SPELL_LOG_EVADE );
			else if( vstate == BLOCK )
				SendSpellLog( this, pVictim,ability->spell_id_client, SPELL_LOG_BLOCK );
			else 
				SendSpellLog( this, pVictim,ability->spell_id_client, SPELL_LOG_IMMUNE );
				
		} */
		//FIXME: add log for miss,block etc for ability and ranged
		//example how it works
		//SendSpellLog(this,pVictim,ability->Id,SPELL_LOG_MISS);
	}

//==========================================================================================
//==============================Damage Dealing==============================================
//==========================================================================================

	//reset out of combat timer
	this->CombatStatus.OnDamageDealt( pVictim );
	DealDamage(pVictim, realdamage, 0); //break auras even if we deal 0 dmg
	if( realdamage > 0 )
	{
		//pVictim->HandleProcDmgShield(PROC_ON_MELEE_ATTACK_VICTIM,this);
//		HandleProcDmgShield(PROC_ON_MELEE_ATTACK_VICTIM,pVictim);
		if(pVictim->GetCurrentSpell())
			pVictim->GetCurrentSpell()->AddTime(0);
	}
	else
	{		
		// have to set attack target here otherwise it wont be set
		// because dealdamage is not called.
		// setAttackTarget(pVictim);
//		this->CombatStatus.OnDamageDealt( pVictim );
	}
//==========================================================================================
//==============================Post Damage Dealing Processing==============================
//==========================================================================================
//--------------------------durability processing-------------------------------------------
	if( pVictim->IsPlayer() )
	{
//		SafePlayerCast( pVictim )->GetItemInterface()->ReduceItemDurability();
		if( !this->IsPlayer() )
		{
			SafePlayerCast( pVictim )->GetItemInterface()->ReduceItemDurability();
		}
		else
		{
//			 SafePlayerCast( this )->GetItemInterface()->ReduceItemDurability();
		}
	}
	else
	{
		if( this->IsPlayer() )//not pvp
		{
			SafePlayerCast( this )->GetItemInterface()->ReduceItemDurability();
		}
	}
	//--------------------------rage processing-------------------------------------------------
	//http://www.wowwiki.com/Formulas:Rage_generation

	if( dmg.full_damage && IsPlayer() && GetPowerType() == POWER_TYPE_RAGE && !ability)
	{
		float val;
		uint32 level = getLevel();
//		float conv;
//		if( level <= PLAYER_LEVEL_CAP )
//			conv = AttackToRageConversionTable[ level ];
//		else 
//			conv = 3.75f / (0.0091107836f * level * level + 3.225598133f * level + 4.2652911f);

		// Hit Factor
//		float f = ( weapon_damage_type == OFFHAND ) ? 1.75f : 3.5f;
		float f = 6.5f;
		if( weapon_damage_type == OFFHAND )
			f = f / 2;

		if( hit_status & HITSTATUS_CRICTICAL )
			f *= 2.0f;

		uint32 s = 1000;

		// Weapon speed (normal)
		Item* weapon = ( SafePlayerCast( this )->GetItemInterface())->GetInventoryItem( INVENTORY_SLOT_NOT_SET, ( weapon_damage_type == OFFHAND ? EQUIPMENT_SLOT_OFFHAND : EQUIPMENT_SLOT_MAINHAND ) );
		if( weapon == NULL )
		{
			if( weapon_damage_type == OFFHAND )
				s = GetUInt32Value( UNIT_FIELD_BASEATTACKTIME_1 );
			else
				s = GetUInt32Value( UNIT_FIELD_BASEATTACKTIME );
		}
		else
		{
			uint32 entry = weapon->GetEntry();
			ItemPrototype* pProto = ItemPrototypeStorage.LookupEntry( entry );
			if( pProto != NULL )
			{
				s = pProto->Delay;
			}
		}

//		val = conv * dmg.full_damage + f * s / 2.0f;
//		val = s * f / 1000.f;
		val = s * f / 2000.f;
		val *= SafePlayerCast( this )->rageFromDamageDealt / 100.0f;
		val *= 10.0f;
		val *= sWorld.getRate(RATE_POWER2);

		//float r = ( 7.5f * dmg.full_damage / c + f * s ) / 2.0f;
		//float p = ( 1 + ( SafePlayerCast( this )->rageFromDamageDealt / 100.0f ) );
		//sLog.outDebug( "Rd(%i) d(%i) c(%f) f(%f) s(%f) p(%f) r(%f) rage = %f", realdamage, dmg.full_damage, c, f, s, p, r, val );

		ModPower( POWER_TYPE_RAGE, (int32)val );
//		UpdatePowerAmm();

	}

	// I am receiving damage 
	// Tudi - this will be called in Object: dealdamage for remaining dmg. We only process the absorbed amount here
	if( dmg.full_damage && pVictim->IsPlayer() && pVictim->GetPowerType() == POWER_TYPE_RAGE && pVictim->CombatStatus.IsInCombat() )
		pVictim->RageEventDamageTaken( MIN( dmg.full_damage, dmg.resisted_damage + dmg.absorbed_damage ), this );
		
	RemoveAurasByInterruptFlag(AURA_INTERRUPT_ON_START_ATTACK);
//--------------------------extra strikes processing----------------------------------------
	if( m_extra_attacks.IsInLoop() == false )
	{
		uint32 time_now = getMSTime();
		m_extra_attacks.BeginLoop();
		for(CommitPointerListNode<ExtraAttack> *itr = m_extra_attacks.begin();itr != m_extra_attacks.end();itr = itr->Next() )
		{
			ExtraAttack *i = itr->data;
			if( i->count && i->next_proc_stamp <  time_now )
			{
				//this might get changed when we strike !
				i->count--;

				Strike( pVictim, i->dmg_type, i->sp, i->add_dmg, i->pct_dmg_mod, 0, false, false);

				i->next_proc_stamp = time_now + i->sp->proc_interval;
			}
		}
		m_extra_attacks.EndLoopAndCommit();
	}

	if(m_extrastriketargetc > 0 && m_extrastriketarget == 0)
	{
		m_extrastriketarget = 1;

		m_extraStrikeTargets.BeginLoop();
		for(CommitPointerListNode<ExtraStrike> *itx = m_extraStrikeTargets.begin();itx != m_extraStrikeTargets.end(); itx = itx->Next() )
		{
			ExtraStrike *ex = itx->data;

			if (ex->deleted) 
				continue;

			AquireInrangeLock(); //make sure to release lock before exit function !
			InrangeLoopExitAutoCallback AutoLock;
			InRangeSetRecProt::iterator itr;
			for( itr = GetInRangeSetBegin( AutoLock ); itr != GetInRangeSetEnd(); ++itr)
			{
				if (!(*itr) || (*itr) == pVictim || !(*itr)->IsUnit())
					continue;

				if( canReachWithAttack( SafeUnitCast((*itr)) ) && isAttackable(this, (*itr)) && (*itr)->isInFront(this) && !SafeUnitCast((*itr))->IsPacified())
				{
					// Sweeping Strikes hits cannot be dodged, missed or parried (from wowhead)
					bool skip_hit_check2 = ex->spell_info->Id == 12328 ? true : false;
					//zack : should we use the spell id the registered this extra strike when striking ? It would solve a few proc on proc problems if so ;)
//					Strike( SafeUnitCast( *itr ), weapon_damage_type, ability, add_damage, pct_dmg_mod, exclusive_damage, false, skip_hit_check );
					Strike( SafeUnitCast( *itr ), weapon_damage_type, ex->spell_info, add_damage, pct_dmg_mod, exclusive_damage, false, skip_hit_check2 );
					break;
				}
			}
			ReleaseInrangeLock();

			// Sweeping Strikes charges are used up regardless whether there is a secondary target in range or not. (from wowhead)
			if (ex->charges > 0)
			{
				ex->charges--;
				if (ex->charges <= 0)
				{
					ex->deleted = true;
					m_extrastriketargetc--;
				}
			}
		}
		m_extraStrikeTargets.EndLoopAndCommit();

		m_extrastriketarget = 0;
	}
}	

void Unit::smsg_AttackStop(Unit* pVictim)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if(!pVictim)
	{ 
		return;
	}

	sStackWorldPacket( data, SMSG_ATTACKSTOP, 50 );
	Player *p;

	if( IsPlayer() )
		p = SafePlayerCast( this );
	//is this even required ?
	else if( IsPet() )
		p = SafePetCast( this )->GetPetOwner();
	else 
		p = NULL;

	if( p && p->GetSession() )
	{
		data << pVictim->GetNewGUID();
		data << GetNewGUID();
		data << uint32(0);
		p->GetSession()->SendPacket( &data );
		data.Initialize( SMSG_ATTACKSTOP );
	}

	data << GetNewGUID();
	data << pVictim->GetNewGUID();
	data << uint32(0);
	SendMessageToSet(&data, true );
//	RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_COMBAT);
	SetByte(UNIT_FIELD_BYTES_2, 0, WEAPON_SHEATH_NONE);

	// stop swinging, reset pvp timeout. if neither enemy or us will perform offensive actions then combat will time out in N seconds
/*	if( pVictim->IsPlayer() )
	{
		pVictim->CombatStatusHandler_ResetPvPTimeout();
		CombatStatusHandler_ResetPvPTimeout();
	}
	//this is for PVE. As long as we have a bad aura on target we are not allowed to exit combat ?
	else
	{
		pVictim->CombatStatus.RemoveAttacker( this, GetGUID() );
		CombatStatus.RemoveAttackTarget( pVictim );
	} */
}

void Unit::smsg_AttackStop(const uint64 &victimGuid)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	sStackWorldPacket( data, SMSG_ATTACKSTOP , 50);
	data << GetNewGUID();
	FastGUIDPack(data, victimGuid);
	data << uint32( 0 );
	SendMessageToSet(&data, IsPlayer());
//	RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_COMBAT);
	SetByte(UNIT_FIELD_BYTES_2, 0, WEAPON_SHEATH_NONE);
}

void Unit::smsg_AttackStart(Unit* pVictim)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
    // Prevent user from ignoring attack speed and stopping and start combat really really fast
    /*if(!isAttackReady())
        setAttackTimer(uint32(0));
    else if(!canReachWithAttack(pVictim))
    {
        setAttackTimer(uint32(500));
        //pThis->GetSession()->OutPacket(SMSG_ATTACKSWING_NOTINRANGE);
    }
    else if(!isInFront(pVictim))
    {
        setAttackTimer(uint32(500));
        //pThis->GetSession()->OutPacket(SMSG_ATTACKSWING_NOTINRANGE);
    }*/

    // Send out ATTACKSTART
    sStackWorldPacket( data,SMSG_ATTACKSTART, 20 );
    data << GetGUID();
    data << pVictim->GetGUID();
//    SendMessageToSet(&data, true);
    SendMessageToSet(&data, false);
    sLog.outDebug( "WORLD: Sent SMSG_ATTACKSTART" );

    // FLAGS changed so other players see attack animation
//	SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_COMBAT);
	SetByte(UNIT_FIELD_BYTES_2, 0, WEAPON_SHEATH_MELEE);
/*    if(GetTypeId() == TYPEID_PLAYER)
    {
        Player* pThis = SafePlayerCast( this );
        if( pThis->cannibalize)
        {
            sEventMgr.RemoveEvents(pThis, EVENT_CANNIBALIZE);
            pThis->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
            pThis->cannibalize = false;
        }
    } */
}

uint16	Unit::FindVisualSlot(Aura *a,bool IsPos)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if( a == NULL )
	{ 
		return AURA_IN_PROCESS_TO_ADD;
	}
	uint32 from,to;
	uint32 SpellId = a->GetSpellId();
	if( a->IsPassive() )
	{
		from = MAX_AURAS;
		to = MIN( MAX_VISIBLE_PAS_AURAS+MAX_AURAS, m_auras_pas_size );
	}
	else if( IsPos )
	{
		from = FIRST_AURA_SLOT;
		to = m_auras_pos_size;
	}
	else
	{
		from = MAX_POSITIVE_AURAS;
		to = m_auras_neg_size;
	}
	//check for already visual same aura
	for(uint32 i=from;i<to;i++)
		if( m_auras[i] != NULL
			&& m_auras[i] != a 
			&& m_auras[i]->GetSpellId() == SpellId 
			&& ( m_auraStackCount[i] < m_auras[i]->GetSpellProto()->maxstack || IsPos == false )
			// && maybe we should also check if caster is the same for aura. Is it worth spamming the space ? Let's just group them
			)
			return m_auras[i]->m_visualSlot;
	//try to find an empty slot
	for(uint32 i=from;i<to;i++)
		if( m_auraStackCount[i] == 0 ) 
			return i;
	//worst case scenario due to bugs
	return AURA_INVALID_VISUAL_SLOT;
}

bool Unit::AddAura(Aura *aur)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
/*	if( GetMapMgr() && CAN_USE_FLYINGMOUNT( GetMapMgr() ) == 0 )
	{
		for( uint32 i = 0; i < 3; ++i )
		{
			// Can't use flying auras in non-outlands.
			if( aur->GetSpellProto()->eff[i].EffectApplyAuraName == SPELL_AURA_ENABLE_FLIGHT2 || aur->GetSpellProto()->eff[i].EffectApplyAuraName == SPELL_AURA_ENABLE_FLIGHT_NO_SPEED )
			{
				sLog.outDebug("Unit : addaura : removing aura without add since it cannot be casted except on specific maps = %s\n",aur->GetSpellProto()->Name );
				sEventMgr.RemoveEvents(aur);	//no events atm ?
				AuraPool.PooledDelete( aur, __FILE__, __LINE__ );
				return false;
			}
		}
	} */
	
	//should be able to add auras, but they should not apply their effects as long as we are immune to them
	if( aur->IsPositive() == false 
		&& aur->GetSpellProto()->School 
		&& SchoolImmunityAntiEnemy[aur->GetSpellProto()->School] 
//		&& SchoolImmunityAntiFriend[aur->GetSpellProto()->School] 
		&& this->GetGUID() != aur->GetCasterGUID()
	)
	{
		sLog.outDebug("Unit : addaura : removing aura without add since we are immune to it = %s\n",aur->GetSpellProto()->Name );
		sEventMgr.RemoveEvents(aur);	//no events atm ?
		AuraPool.PooledDelete( aur, __FILE__, __LINE__ );
		return false;
	}/**/

	uint16 first_free_slot=AURA_IN_PROCESS_TO_ADD;
	if( !aur->IsPassive() 
		|| aur->IsAreaAura()	//we need to check area auras to not add them multiple times
		|| aur->GetSpellProto()->maxstack > 0			//donno how to filter these :( Ex : weapon chain enchant is passive but should not stack
//		|| aur->GetSpellProto()->HasStackGroupFlags != 0
		)
	{
		int32 maxStack = aur->GetSpellProto()->maxstack;
		//uint32 aurName = aur->GetSpellProto()->Name;
		//uint32 aurRank = aur->GetSpellProto()->Rank;
		if( aur->GetSpellProto()->procCharges > 0 )
		{
			int32 charges = aur->GetSpellProto()->procCharges;
			Unit *AuraCaster = aur->GetUnitCaster();
			if( (aur->GetSpellProto()->c_is_flags & SPELL_FLAG_IS_REQUIRING_SM_MOD) && AuraCaster != NULL && AuraCaster->SM_Mods )
			{
				SM_FIValue( AuraCaster->SM_Mods->SM_FCharges, &charges, aur->GetSpellProto()->GetSpellGroupType() );
				SM_PIValue( AuraCaster->SM_Mods->SM_PCharges, &charges, aur->GetSpellProto()->GetSpellGroupType() );
#ifdef COLLECTION_OF_UNTESTED_STUFF_AND_TESTERS
				float spell_flat_modifers=0;
				float spell_pct_modifers=0;
				SM_FIValue(aur->GetUnitCaster()->SM_FCharges,&spell_flat_modifers,aur->GetSpellProto()->GetSpellGroupType());
				SM_FIValue(aur->GetUnitCaster()->SM_PCharges,&spell_pct_modifers,aur->GetSpellProto()->GetSpellGroupType());
				if(spell_flat_modifers!=0 || spell_pct_modifers!=0)
					printf("!!!!!spell charge bonus mod flat %f , spell range bonus pct %f , spell range %f, spell group %u\n",spell_flat_modifers,spell_pct_modifers,maxRange,m_spellInfo->GetSpellGroupType());
#endif
			}
			//some auras build up charges, for those maxstack is larger then 1 add
			if( charges > (int32)maxStack )
				maxStack=charges;
		}
		if( IsPlayer() && SafePlayerCast( this )->AuraStackCheat )
			maxStack = 999;

		SpellEntry * info = aur->GetSpellProto();
		//uint32 flag3 = aur->GetSpellProto()->Flags3;

//		AuraCheckResponse acr;
//		WorldPacket data( 21 );
		bool deleteAur = false;

		//check if we already have this aura by this caster -> update duration
		// Nasty check for Blood Fury debuff (spell system based on namehashes is bs anyways)
		{
			uint32 f = 0;
			uint32 start,end;
			if( aur->IsPositive() )
			{
				start = FIRST_AURA_SLOT;
				end = m_auras_pos_size;
			}
			else if( aur->IsPassive() )
			{
				start = MAX_PASSIVE_AURAS;
				end = m_auras_pas_size;
			}
			else
			{
				start = MAX_POSITIVE_AURAS;
				end = m_auras_neg_size;
			}
			for( uint32 x = start; x < end; x++ )
			{
				Aura *curaur = m_auras[x];
				if( curaur )
				{
					// Check for auras by specific type.
					// Check for auras with the same name and a different rank.					
					if( info->BGR_one_buff_on_target > 0
						&& (curaur->GetSpellProto()->BGR_one_buff_on_target & info->BGR_one_buff_on_target)
						&& maxStack == 0)
					{
						if( ( info->BGR_one_buff_on_target_skip_caster_check == true || curaur->m_casterGuid == aur->m_casterGuid )
							|| ( curaur->GetSpellProto()->NameHash == info->NameHash && aur->IsPositive() )	//if maxstack == 0 then these should never stack
							)
						{
							sLog.outDebug("Unit : addaura : removing aura without add since target cannot have more = %s\n",aur->GetSpellProto()->Name );
							deleteAur = true;
							//refresh the timer
							if( curaur->GetSpellProto()->Id == aur->GetSpellProto()->Id )
								curaur->ResetDuration( aur->GetDuration() );
							break;
						}
					}
					else
					{
						AuraCheckResponse acr = AuraCheck( aur, curaur, maxStack );
						if( acr.Error == AURA_CHECK_RESULT_HIGHER_BUFF_PRESENT )
						{
							sLog.outDebug("Unit : addaura : removing aura without add since target has better = %s\n",aur->GetSpellProto()->Name );
							deleteAur = true;
							//refresh the timer
							if( curaur->GetSpellProto()->Id == aur->GetSpellProto()->Id )
							{
								//update duration,the same aura (update the whole stack whenever we cast a new one)
								curaur->ResetDuration( aur->GetDuration() );
								//important to use new duration because due to diminishing returns refreshmight get smaller!
								curaur->UpdateModifiers();
							}
							break;
						}
						else if( acr.Error == AURA_CHECK_RESULT_LOWER_BUFF_PRESENT )
						{
							// remove the lower aura
							curaur->Remove();
							// no more checks on bad ptr
							first_free_slot = x;
							break;
						}
						else if( acr.Error == AURA_CHECK_RESULT_CANNOT_STACK )
						{
							sLog.outDebug("Unit : addaura : removing aura without add since target cannot stack more = %s\n",aur->GetSpellProto()->Name );
							deleteAur = true;
							//refresh the timer
							if( curaur->GetSpellProto()->Id == aur->GetSpellProto()->Id )
							{
								//update duration,the same aura (update the whole stack whenever we cast a new one)
								uint32 end2=x+1;
								if( curaur->GetSpellProto()->maxstack > 1 )
									end2=end; 
								for( uint32 y = x; y < end2; y++ )
									if( m_auras[y] && m_auras[y]->GetSpellProto()->Id == aur->GetSpellProto()->Id )
									{
										m_auras[y]->ResetDuration( aur->GetDuration() );
										//important to use new duration because due to diminishing returns refreshmight get smaller!
										m_auras[y]->UpdateModifiers();
									}
								if( curaur->m_modcount > 0 ) //charge auras have no values and will make client show 0 as their value
									ModVisualAuraStackCount( curaur, 0 );
							}
							break;
						}
						//all stacks should have time refreshed 
						//!!! todo -> refresh all auras due to premature exit !!!
						if( curaur->GetSpellProto()->Id == aur->GetSpellProto()->Id )
						{
							if( ( curaur->GetSpellProto()->c_is_flags3 & SPELL_FLAG3_IS_SKIP_REFRESH_SAME_AURAS_OTHERS ) == 0 )
								//update duration,the same aura (update the whole stack whenever we cast a new one)
								curaur->ResetDuration( aur->GetDuration() );
						}
					}					   
				}
				else if( first_free_slot == AURA_IN_PROCESS_TO_ADD )
				{
					first_free_slot = x;
					if( x + 1 == end )
					{
						if( aur->IsPositive() && m_auras_pos_size < MAX_POSITIVE_AURAS)
							m_auras_pos_size++;
						else if( m_auras_neg_size < MAX_AURAS )
							m_auras_neg_size++;
					}
				}
			}
		}

		if(deleteAur)
		{
			sEventMgr.RemoveEvents(aur);
			AuraPool.PooledDelete( aur, __FILE__, __LINE__ );
			return false;
		}
	}
	//this is for passive auras
	else
	{
		for( uint32 x = MAX_AURAS; x < m_auras_pas_size; x++ )
			if( !m_auras[x] )
			{
				first_free_slot = x;
				if( x + 1 == m_auras_pas_size && m_auras_pas_size < MAX_AURAS + MAX_PASSIVE_AURAS ) //if this was the last value of the list then we can extend it
					m_auras_pas_size++;
				break;
			}
	}
			
//printf("!!!!! the 3 vals pos = %u, neg = %u, pas = %u\n",m_auras_pos_size - 0,m_auras_neg_size - MAX_POSITIVE_AURAS,m_auras_pas_size - MAX_AURAS);

	//Zack : if all mods were resisted it means we did not apply anything and we do not need to delete this spell eighter
	// also abandon if there are no more free slots to add this aura to
	if( first_free_slot == AURA_IN_PROCESS_TO_ADD )
	{
		//notify client that we are imune to this spell ?
		sLog.outDebug("Unit : addaura : removing aura without add since target has no more slots left = %s\n",aur->GetSpellProto()->Name );
		sEventMgr.RemoveEvents(aur);
		AuraPool.PooledDelete( aur, __FILE__, __LINE__ );
		return false;
	}

/*	
	//some auras need this to ne sent. Just can't recall one :)
	WorldPacket data(SMSG_AURACASTLOG, 28);
	data << aur->m_casterGuid;
	data << GetGUID();
	data << aur->GetSpellId();
	data << uint64(0);
	SendMessageToSet(&data, true);*/

	// this is temporary aura. It is not added to unit until effect is applied
	aur->m_auraSlot = AURA_IN_PROCESS_TO_ADD;	
	aur->ApplyModifiers(true);

	if( aur->m_deleted )
	{
		sLog.outDebug("Unit : addaura : Impossible, adding a deleted aura = %d\n",aur->m_spellId );
		return false;
	}

	//there is a chance that apply modifiers will actually try to cancel this aura because it calls some interrupt that will remove aura from list before it gets applied
	if( 
		//aur->m_deleted == true || 
		aur->TargetWasImuneToMods() )
	{
//		Spell::SendSpellFail( aur->GetCaster(), this, aur->GetSpellId(), SPELL_FAILED_IMMUNE );
		sLog.outDebug("Unit : addaura : removing aura without add since target resisted all mods = %s\n",aur->GetSpellProto()->Name );
		aur->Remove();
		return false;
	}
	if( m_auras[ first_free_slot ] != NULL )
	{
		//while applying effect of this aura we might have added other aura to the selected slot
		first_free_slot = AURA_IN_PROCESS_TO_ADD;
		if( aur->IsPassive() )
		{
			for( uint32 x = MAX_AURAS; x < m_auras_pas_size; x++ )
				if( m_auras[x] == NULL )
				{
					first_free_slot = x;
					if( x + 1 == m_auras_pas_size && m_auras_pas_size < MAX_AURAS + MAX_PASSIVE_AURAS ) //if this was the last value of the list then we can extend it
						m_auras_pas_size++;
					break;
				}
		}
		else if( aur->IsPositive() )
		{
			for( uint32 x = FIRST_AURA_SLOT; x < m_auras_pos_size; x++ )
				if( m_auras[x] == NULL )
				{
					first_free_slot = x;
					if( x + 1 == m_auras_pos_size && m_auras_pos_size < MAX_POSITIVE_AURAS ) //if this was the last value of the list then we can extend it
						m_auras_pos_size++;
					break;
				}
		}
		else
		{
			for( uint32 x = MAX_POSITIVE_AURAS; x < m_auras_neg_size; x++ )
				if( m_auras[x] == NULL )
				{
					first_free_slot = x;
					if( x + 1 == m_auras_neg_size && m_auras_neg_size < MAX_AURAS ) //if this was the last value of the list then we can extend it
						m_auras_neg_size++;
					break;
				}
		}
	}
	//we just ran out of slots ?
	if( first_free_slot == AURA_IN_PROCESS_TO_ADD )
	{
		sLog.outDebug("Unit : addaura : removing aura without add since target had no more free slots = %s\n",aur->GetSpellProto()->Name );
		aur->Remove();
		return false;
	}
	//register this aura
	aur->m_auraSlot = first_free_slot;
	m_auras[ first_free_slot ] = aur;

	aur->m_visualSlot=AURA_INVALID_VISUAL_SLOT;
	//search for a visual slot
	// if aura is not passive, will have icon client side or has flag to forced show
	if( aur->IsVisible() )
	{
		aur->m_visualSlot = FindVisualSlot( aur , aur->IsPositive() );
		ModVisualAuraStackCount( aur, 1 ); //this might fail but there is no biggie about it :P
	}

	// We add 500ms here to allow for the last tick in DoT spells. This is a dirty hack, but at least it doesn't crash like my other method.
	// - Burlex
	if(aur->GetDuration() > 0)
	{
		sEventMgr.AddEvent(aur, &Aura::Remove, EVENT_AURA_REMOVE, aur->GetDuration(), 1,
			EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT | EVENT_FLAG_DELETES_OBJECT);
	}

	//aur->RelocateEvents();

	// Reaction from enemy AI - note that we are also adding threath in a lot of other places. This might be useless
	// this does not include refreshed aura threath
	Unit *pCaster = aur->GetUnitCaster();
	uint32 InterruptInvincible = (aur->GetSpellProto()->AuraInterruptFlags & AURA_INTERRUPT_ON_INVINCIBLE);
	if(	!aur->IsPositive() 
//		&& IsCreature()	//is it worth agroing non AI using units ?
		&& pCaster != this
		&& pCaster	//wow this actually happened to me
		&& pCaster->isAlive()
		&& this->isAlive()
		&& ( aur->GetSpellProto()->ThreatForSpellCoef 
//			|| aur->IsCombatStateAffecting() 
		) )	  // Creature
	{
		//also reset combat status timer
		pCaster->CombatStatus.OnDamageDealt(this);
		//max 3 mods
		uint32 aura_mod_sum=0;
		for(uint32 i=0;i<aur->m_modcount;i++)
			aura_mod_sum += aur->m_modList[i].m_amount;
		if( aura_mod_sum <= 0 )
			aura_mod_sum = 1;
		if(( aur->GetSpellProto()->c_is_flags & SPELL_FLAG_IS_HEALING ) == 0 )
			m_aiInterface->AttackReaction(pCaster, aura_mod_sum, aur->GetSpellId());
	}

	//!!!! attackreaction can remove this newly added aura !!!
	if( InterruptInvincible && pCaster)
	{
		pCaster->RemoveStealth();
		pCaster->RemoveInvisibility();
		pCaster->RemoveAuraByNameHash(SPELL_HASH_ICE_BLOCK);
		pCaster->RemoveAuraByNameHash(SPELL_HASH_DIVINE_SHIELD);
		pCaster->RemoveAuraByNameHash(SPELL_HASH_BLESSING_OF_PROTECTION);
	}

	return true;
}

bool Unit::RemoveAuras(uint32 * SpellIds)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if(!SpellIds || *SpellIds == 0)
	{ 
		return false;
	}

	uint32 x,y;
	bool res = false;
	for(x=FIRST_AURA_SLOT;x<MAX_AURAS+MAX_PASSIVE_AURAS;x++)
		if(m_auras[x])
			for(y=0;SpellIds[y] != 0;++y)
				if(m_auras[x]->GetSpellId()==SpellIds[y])
				{
					m_auras[x]->Remove();
					res = true;
				}
	return res;
}

uint32 Unit::RemoveAuraByHeal(uint32 Slot)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	uint32 res = 0;
	if ( m_auras[Slot] )
	{
		switch( m_auras[Slot]->GetSpellId() )
		{
			// remove after heal
			case 35321:
			case 38363:
			case 39215:
			{
				m_auras[Slot]->Remove();
				res = 1;
			} break;
			// remove when healed to 100%
			case 31956:
			case 38801:
			case 43093:
			{
				if ( GetUInt32Value( UNIT_FIELD_HEALTH ) == GetUInt32Value( UNIT_FIELD_MAXHEALTH ) )
				{
					m_auras[Slot]->Remove();
					res = 1;
				}
			} break;
			// remove at p% health
			case 38772:
			{
				uint32 p = m_auras[Slot]->GetSpellProto()->eff[1].EffectBasePoints;
				if ( GetUInt32Value( UNIT_FIELD_MAXHEALTH ) * p <= GetUInt32Value( UNIT_FIELD_HEALTH ) * 100 )
				{
					m_auras[Slot]->Remove();
					res = 1;
				}
			} break;
		}
	}
	return res;
}

void Unit::RemoveAurasMovementImpairing()
{
	for(uint32 x= NEG_AURAS_START; x < MAX_NEGATIVE_AURAS1(this); ++x)
		if(m_auras[x] && (m_auras[x]->GetSpellProto()->c_is_flags2 & SPELL_FLAG2_IS_MOVEMENT_IMPAIRING_SPELL ) != 0)
			m_auras[x]->Remove();
}

uint32 Unit::RemoveAurasByHeal()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	uint32 res = 0;
	for(uint32 x=FIRST_AURA_SLOT;x<m_auras_pos_size;x++)
		res += RemoveAuraByHeal( x );
	for(uint32 x=MAX_POSITIVE_AURAS;x<m_auras_neg_size;x++)
		res += RemoveAuraByHeal( x );
	for(uint32 x=MAX_AURAS;x<m_auras_pas_size;x++)
		res += RemoveAuraByHeal( x );

	return res;
}

//apart from spells loosing a charge you probably wanted to use removeallauras to make sure all charges and invisible parts are removed
int32 Unit::RemoveAura(uint32 spellId, const uint64 &guid, uint32 SearchFlags, int32 Count)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
   
	int32 Count2 = Count;
	if( guid != 0 )
	{
		if( SearchFlags & AURA_SEARCH_POSITIVE )
			for(uint32 x=FIRST_AURA_SLOT;x<m_auras_pos_size;x++)
				if(m_auras[x] && m_auras[x]->GetSpellId()==spellId && m_auras[x]->m_casterGuid == guid)
				{
					m_auras[x]->Remove();
					Count--;
					if( Count <= 0 )
						return (Count2-Count);
				}
		if( SearchFlags & AURA_SEARCH_NEGATIVE )
			for(uint32 x=MAX_POSITIVE_AURAS;x<m_auras_neg_size;x++)
				if(m_auras[x] && m_auras[x]->GetSpellId()==spellId && m_auras[x]->m_casterGuid == guid)
				{
					m_auras[x]->Remove();
					Count--;
					if( Count <= 0 )
						return (Count2-Count);
				}
		if( SearchFlags & AURA_SEARCH_PASSIVE )
			for(uint32 x=MAX_AURAS;x<m_auras_pas_size;x++)
				if(m_auras[x] && m_auras[x]->GetSpellId()==spellId && m_auras[x]->m_casterGuid == guid)
				{
					m_auras[x]->Remove();
					Count--;
					if( Count <= 0 )
						return (Count2-Count);
				}
	}
	else
	{
		if( SearchFlags & AURA_SEARCH_POSITIVE )
			for(uint32 x=FIRST_AURA_SLOT;x<m_auras_pos_size;x++)
				if(m_auras[x] && m_auras[x]->GetSpellId()==spellId )
				{
					m_auras[x]->Remove();
					Count--;
					if( Count <= 0 )
						return (Count2-Count);
				}
		if( SearchFlags & AURA_SEARCH_NEGATIVE )
			for(uint32 x=MAX_POSITIVE_AURAS;x<m_auras_neg_size;x++)
				if(m_auras[x] && m_auras[x]->GetSpellId()==spellId )
				{
					m_auras[x]->Remove();
					Count--;
					if( Count <= 0 )
						return (Count2-Count);
				}
		if( SearchFlags & AURA_SEARCH_PASSIVE )
			for(uint32 x=MAX_AURAS;x<m_auras_pas_size;x++)
				if(m_auras[x] && m_auras[x]->GetSpellId()==spellId )
				{
					m_auras[x]->Remove();
					Count--;
					if( Count <= 0 )
						return (Count2-Count);
				}
	}
	return (Count2-Count);
}

int32 Unit::RemoveAuraByNameHash(uint32 namehash, const uint64 &guid, uint32 SearchFlags, int32 Count)
{
	int32 Count2 = Count;
	if( guid != 0 )
	{
		if( SearchFlags & AURA_SEARCH_POSITIVE )
			for(uint32 x=FIRST_AURA_SLOT;x<m_auras_pos_size;x++)
				if(m_auras[x] && m_auras[x]->GetSpellProto()->NameHash==namehash && m_auras[x]->m_casterGuid == guid)
				{
					m_auras[x]->Remove();
					Count--;
					if( Count <= 0 )
						return Count2;
				}
		if( SearchFlags & AURA_SEARCH_NEGATIVE )
			for(uint32 x=MAX_POSITIVE_AURAS;x<m_auras_neg_size;x++)
				if(m_auras[x] && m_auras[x]->GetSpellProto()->NameHash==namehash && m_auras[x]->m_casterGuid == guid)
				{
					m_auras[x]->Remove();
					Count--;
					if( Count <= 0 )
						return Count2;
				}
		if( SearchFlags & AURA_SEARCH_PASSIVE )
			for(uint32 x=MAX_AURAS;x<m_auras_pas_size;x++)
				if(m_auras[x] && m_auras[x]->GetSpellProto()->NameHash==namehash && m_auras[x]->m_casterGuid == guid)
				{
					m_auras[x]->Remove();
					Count--;
					if( Count <= 0 )
						return Count2;
				}
	}
	else
	{
		if( SearchFlags & AURA_SEARCH_POSITIVE )
			for(uint32 x=FIRST_AURA_SLOT;x<m_auras_pos_size;x++)
				if(m_auras[x] && m_auras[x]->GetSpellProto()->NameHash==namehash )
				{
					m_auras[x]->Remove();
					Count--;
					if( Count <= 0 )
						return Count2;
				}
		if( SearchFlags & AURA_SEARCH_NEGATIVE )
			for(uint32 x=MAX_POSITIVE_AURAS;x<m_auras_neg_size;x++)
				if(m_auras[x] && m_auras[x]->GetSpellProto()->NameHash==namehash )
				{
					m_auras[x]->Remove();
					Count--;
					if( Count <= 0 )
						return Count2;
				}
		if( SearchFlags & AURA_SEARCH_PASSIVE )
			for(uint32 x=MAX_AURAS;x<m_auras_pas_size;x++)
				if(m_auras[x] && m_auras[x]->GetSpellProto()->NameHash==namehash )
				{
					m_auras[x]->Remove();
					Count--;
					if( Count <= 0 )
						return Count2;
				}
	}
	return (Count2-Count);
}

void Unit::RemovePositiveAuras()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	for(uint32 x=FIRST_AURA_SLOT;x<m_auras_pos_size;x++)
		if(m_auras[x])
		{
            if(m_auras[x]->GetSpellProto()->AttributesExC & CAN_PERSIST_AND_CASTED_WHILE_DEAD)
                continue;
            else
			    m_auras[x]->Remove();
		}
}

void Unit::RemoveNegativeAuras()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	for(uint32 x=MAX_POSITIVE_AURAS;x<m_auras_neg_size;x++)
		if(m_auras[x])
		{
            if(m_auras[x]->GetSpellProto()->AttributesExC & CAN_PERSIST_AND_CASTED_WHILE_DEAD)
                continue;
            else
			    m_auras[x]->Remove();
		}
}

void Unit::RemoveAllAuras()
{
	bool found_auras;
	do
	{
		found_auras = false;
		for(uint32 x=FIRST_AURA_SLOT;x<MAX_AURAS + MAX_PASSIVE_AURAS;x++)
			if(m_auras[x])
			{
				m_auras[x]->Remove();
				found_auras = true;
			}
	//funny huh? Some auras add auras on remove ;) We want ALL auras cleared !
	}while( found_auras == true );
}

//ex:to remove morph spells
void Unit::RemoveAllAuraType(uint32 auratype)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	for(uint32 x=FIRST_AURA_SLOT;x<m_auras_pos_size;x++)
		if(m_auras[x])
		{
			SpellEntry *proto=m_auras[x]->GetSpellProto();
			if(proto->eff[0].EffectApplyAuraName==auratype || proto->eff[1].EffectApplyAuraName==auratype || proto->eff[2].EffectApplyAuraName==auratype)
				RemoveAura(m_auras[x]->GetSpellId());//remove all morph auras containig to this spell (like wolf motph also gives speed)
		}
	for(uint32 x=MAX_POSITIVE_AURAS;x<m_auras_neg_size;x++)
		if(m_auras[x])
		{
			SpellEntry *proto=m_auras[x]->GetSpellProto();
			if(proto->eff[0].EffectApplyAuraName==auratype || proto->eff[1].EffectApplyAuraName==auratype || proto->eff[2].EffectApplyAuraName==auratype)
				RemoveAura(m_auras[x]->GetSpellId());//remove all morph auras containig to this spell (like wolf motph also gives speed)
		}
/*	for(uint32 x=MAX_AURAS;x<m_auras_pas_size;x++)
		if(m_auras[x])
		{
			SpellEntry *proto=m_auras[x]->GetSpellProto();
			if(proto->eff[0].EffectApplyAuraName==auratype || proto->eff[1].EffectApplyAuraName==auratype || proto->eff[2].EffectApplyAuraName==auratype)
				RemoveAura(m_auras[x]->GetSpellId());//remove all morph auras containig to this spell (like wolf motph also gives speed)
		}*/
}


//ex:to remove morph spells
void Unit::RemoveAllAuraFromSelfType2(uint32 auratype, uint32 butskip_hash)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	for(uint32 x=FIRST_AURA_SLOT;x<m_auras_pos_size;x++)
		if(m_auras[x])
		{
			SpellEntry *proto=m_auras[x]->GetSpellProto();
			if( proto->BGR_one_buff_from_caster_on_self == auratype 
				&& proto->NameHash != butskip_hash
				&& m_auras[x]->GetCaster() == this 
				)
				RemoveAura(m_auras[x]->GetSpellId());//remove all morph auras containig to this spell (like wolf motph also gives speed)
		}
	for(uint32 x=MAX_POSITIVE_AURAS;x<m_auras_neg_size;x++)
		if(m_auras[x])
		{
			SpellEntry *proto=m_auras[x]->GetSpellProto();
			if( proto->BGR_one_buff_from_caster_on_self == auratype 
				&& proto->NameHash != butskip_hash
				&& m_auras[x]->GetCaster() == this 
				)
				RemoveAura(m_auras[x]->GetSpellId());//remove all morph auras containig to this spell (like wolf motph also gives speed)
		}
}

void Unit::RemoveInvisibility()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	RemoveAllAuraType( SPELL_AURA_MOD_INVISIBILITY );
}

/*
bool Unit::SetAurDuration(uint32 spellId,Unit* caster,uint32 duration)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	Aura*aur=HasAura(spellId,caster->GetGUID());
	if(!aur)
	{ 
		return false;
	}
	aur->SetDuration(duration);
	sEventMgr.ModifyEventTimeLeft(aur, EVENT_AURA_REMOVE, duration);
			
	if( aur->m_visualSlot < MAX_VISIBLE_AURAS )
	{
		uint8 flags;
		flags = 0; // do not send guid
		flags |= VISUALAURA_FLAG_VISIBLE; //seems to be always present
		if( aur->m_casterGuid == GetGUID() )
			flags |= VISUALAURA_FLAG_SELF_CAST;
		if( !aur->IsPositive() )
			flags |= VISUALAURA_FLAG_NEGATIVE; 
		else
			flags |= VISUALAURA_FLAG_UNK_POS | VISUALAURA_FLAG_REMOVABLE;
		if( aur->GetDuration() != (uint32)-1 ) //yes it is stupid to force -1 to be uint32 but that is the way it is made elsewhere
			flags |= VISUALAURA_FLAG_HAS_DURATION; 
		sStackWorldPacket( data, SMSG_AURA_UPDATE, 50);
		FastGUIDPack(data, GetGUID());
		data << uint8(aur->m_visualSlot);
		data << uint32(aur->GetSpellId());
		data << (uint8)flags;
//		data << (uint8)(m_aura_packet_serialization++);
		data << (uint8)(aur->GetSpellProto()->GetMaxLevel());
		data << uint8(m_auraStackCount[aur->m_visualSlot]);

		if( (flags & VISUALAURA_FLAG_SELF_CAST)==0 )
			FastGUIDPack(data, aur->m_casterGuid);

		//seems like this depends on flag 0x20
		if( flags & VISUALAURA_FLAG_HAS_DURATION )
			data << uint32(aur->GetDuration()) << uint32(aur->GetTimeLeft());
		SendMessageToSet(&data, true);
	}
			
	return true;
}


bool Unit::SetAurDuration(uint32 spellId,uint32 duration)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	Aura*aur=HasAura(spellId);

	if(!aur) 
	{ 
		return false;
	}
	

	aur->SetDuration(duration);
	sEventMgr.ModifyEventTimeLeft(aur, EVENT_AURA_REMOVE, duration);
		
	if( aur->m_visualSlot && aur->m_visualSlot < MAX_VISIBLE_AURAS )
	{
		uint8 flags;
		flags = 0; // do not send guid
		flags |= VISUALAURA_FLAG_VISIBLE; //seems to be always present
		if( aur->m_casterGuid == GetGUID() )
			flags |= VISUALAURA_FLAG_SELF_CAST;
		if( !aur->IsPositive() )
			flags |= VISUALAURA_FLAG_NEGATIVE; 
		else
			flags |= VISUALAURA_FLAG_UNK_POS | VISUALAURA_FLAG_REMOVABLE;
		if( aur->GetDuration() != (uint32)-1 ) //yes it is stupid to force -1 to be uint32 but that is the way it is made elsewhere
			flags |= VISUALAURA_FLAG_HAS_DURATION; 
		sStackWorldPacket( data, SMSG_AURA_UPDATE, 50);
		FastGUIDPack(data, GetGUID());
		data << uint8(aur->m_visualSlot);
		data << uint32(aur->GetSpellId());
		data << (uint16)flags;
///		data << (uint8)(m_aura_packet_serialization++);
		data << (uint8)(aur->GetSpellProto()->GetMaxLevel());
		data << uint8(m_auraStackCount[aur->m_visualSlot]);

		if( (flags & VISUALAURA_FLAG_SELF_CAST)==0 )
			FastGUIDPack(data, aur->m_casterGuid);

		//seems like this depends on flag 0x20
		if( flags & VISUALAURA_FLAG_HAS_DURATION )
			data << uint32(aur->GetDuration()) << uint32(aur->GetTimeLeft());
		SendMessageToSet(&data, true);
	}

	return true;
}*/

void Unit::_UpdateSpells( uint32 time )
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	/* to avoid deleting the current spell */
	if(m_currentSpell != NULL)
	{
//		m_spellsbusy=true;
		if( m_currentSpell->deleted == true )  //wow, somehow someone interrupted us 
			m_currentSpell = NULL;
		else
			m_currentSpell->update(time);
//		m_spellsbusy=false;
	}
}

void Unit::CastSpellDelayed( uint64 targetGuid, uint32 SpellID, uint32 Delay, bool Triggered)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
//	sEventMgr.AddEvent( this, &Unit::CastSpell, (uint64)targetGuid, (uint32)SpellID, (bool)false, (uint32)EVENT_DELAYED_SPELLCAST, Delay, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
	sEventMgr.AddEvent( this, &Unit::EventCastSpell2, (uint64)targetGuid, (uint32)SpellID, Triggered, (uint32)EVENT_DELAYED_SPELLCAST, Delay, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
}

void Unit::CastSpellDelayed2( uint64 targetGuid, uint32 SpellID, uint32 Delay, bool Triggered, int32 ValOverride0)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
//	sEventMgr.AddEvent( this, &Unit::CastSpell, (uint64)targetGuid, (uint32)SpellID, (bool)false, (uint32)EVENT_DELAYED_SPELLCAST, Delay, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
	sEventMgr.AddEvent( this, &Unit::EventCastSpell4, (uint64)targetGuid, SpellID, Triggered, ValOverride0, (uint32)EVENT_DELAYED_SPELLCAST, Delay, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
}

void Unit::EventCastSpell4( uint64 targetGuid, uint32 SpellID, bool Triggered, int32 ValOverride0 )
{
	SpellEntry *Sp = dbcSpell.LookupEntryForced( SpellID );
	SpellCastTargets targets(targetGuid);
	Spell *newSpell = SpellPool.PooledNew( __FILE__, __LINE__ );
	newSpell->Init(this, Sp, Triggered, 0);
	newSpell->forced_basepoints[0] = ValOverride0;
	newSpell->prepare(&targets);
}

void Unit::castSpell( Spell * pSpell )
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	// check if we have a spell already casting etc
	if(m_currentSpell && pSpell != m_currentSpell)
	{
/*	
		//removed by zack : Spell system does not really handle well paralel casting. We are forced to cancel previous cast
		//eventually make creatures be able to cast secondary spell that is tracked or figure out where is this deleted
		if(m_spellsbusy)
		{
			// shouldn't really happen. but due to spell sytem bugs there are some cases where this can happen.
			sEventMgr.AddEvent(this,&Unit::CancelSpell,m_currentSpell,EVENT_UNIT_DELAYED_SPELL_CANCEL,1,1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
		}
		else*/
		m_currentSpell->safe_cancel( true );
	}

	m_currentSpell = pSpell;
//	pLastSpell = pSpell->GetProto();
}

int32 Unit::GetSpellDmgBonus( Unit *pVictim, SpellEntry *spellInfo, int32 base_dmg, int32 spell_effect_index, float *RetSumarryCoeff )
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	int32 bonus_damage = 0;
	int32 bonus_damage_final = 0;
	Unit* caster = this;
	uint32 school = spellInfo->School;
//	bool IsMultiSchoolSpell = ( 1 << spellInfo->School ) != spellInfo->SchoolMask;

//	if( spellInfo->c_is_flags & SPELL_FLAG_IS_NOT_USING_DMG_BONUS )
//	{ 
//		return 0;
//	}

//------------------------------by school---------------------------------------------------
	bonus_damage += caster->GetDamageDoneMod(school);
	bonus_damage += pVictim->DamageTakenMod[school];
	bonus_damage += pVictim->ModDamageTakenByMechFlat[spellInfo->MechanicsType];
//------------------------------by victim type----------------------------------------------
	if(pVictim->IsCreature() && SafeCreatureCast(pVictim)->GetCreatureInfo() && caster->IsPlayer() )
		bonus_damage += SafePlayerCast(caster)->IncreaseDamageByType[SafeCreatureCast(pVictim)->GetCreatureInfo()->Type]; //this might create a crash. Leaving it like this to detect new creature types :)
//==============================+Spell Damage Bonus Modifications===========================
	if( spell_effect_index > MAX_SPELL_EFFECT_COUNT )
		bonus_damage = float2int32( bonus_damage * spellInfo->SpellPowerCoefAvg );	//!!note that every effect has it's own Coef !!! this is not the right way to calc coeff !!!
	else 
	{
		bonus_damage = float2int32( bonus_damage * spellInfo->eff[ spell_effect_index ].EffectSpellPowerCoef );
	
		//DK, Paladin, hunter
		if( spellInfo->AttackPowerToSpellDamageCoeff != 0.0f || spellInfo->AP_spell_bonus_total[ spell_effect_index ] != 0.0f )
		{
			float AP = MAX( GetAP(), GetRAP() );
			float normalization;
//			if( this->IsPlayer() )
//				normalization = SafePlayerCast( this )->GetWeaponNormalizationCoeffForAP( MELEE );//main hand always ? Need to detect this properly
//			else
				normalization = 1.0f;
			bonus_damage += float2int32( AP * spellInfo->AttackPowerToSpellDamageCoeff * normalization );
			bonus_damage_final = float2int32( AP * spellInfo->AP_spell_bonus_total[ spell_effect_index ] );
		}
	}
//==============================Bonus Adding To Main Damage=================================
	//bonus_damage +=pVictim->DamageTakenMod[school]; Bad copy-past i guess :P
	int32 dmg_bonus_pct_mod=100;
	float summaryPCTmod = caster->GetDamageDonePctMod( school, spellInfo->SchoolMask ); //value is initialized with 1
	if( (spellInfo->c_is_flags & SPELL_FLAG_IS_REQUIRING_SM_MOD) && caster->SM_Mods )
	{
		SM_FIValue(caster->SM_Mods->SM_FDamageBonus, &bonus_damage, spellInfo->GetSpellGroupType());
		int32 TotalDotDMGMod = 100;
		SM_FIValue(caster->SM_Mods->SM_PPenalty, &dmg_bonus_pct_mod, spellInfo->GetSpellGroupType());		
		summaryPCTmod = summaryPCTmod * ( TotalDotDMGMod / 100.0f );
		TotalDotDMGMod = 100;
		SM_FIValue(caster->SM_Mods->SM_PDamageBonus,&TotalDotDMGMod,spellInfo->GetSpellGroupType());
		summaryPCTmod = summaryPCTmod * ( TotalDotDMGMod / 100.0f );
//		bonus_damage += (base_dmg+bonus_damage)*dmg_bonus_pct/100;
//		bonus_damage += bonus_damage*dmg_bonus_pct/100;
#ifdef COLLECTION_OF_UNTESTED_STUFF_AND_TESTERS
		spell_flat_modifers=0;
		spell_pct_modifers=0;
		SM_FIValue(caster->SM_FDamageBonus,&spell_flat_modifers,spellInfo->GetSpellGroupType());
		SM_FIValue(caster->SM_PDamageBonus,&spell_pct_modifers,spellInfo->GetSpellGroupType());
		if(spell_flat_modifers!=0 || spell_pct_modifers!=0)
			printf("!!!!!spell dmg bonus mod flat %d , spell dmg bonus pct %d , spell dmg bonus %d, spell group %u\n",spell_flat_modifers,spell_pct_modifers,bonus_damage,spellInfo->GetSpellGroupType());
#endif

		if( spellInfo->eff[ spell_effect_index ].EffectAmplitude > 0 )
		{
			int32 TotalDotDMGMod = 100;
			SM_FIValue( caster->SM_Mods->SM_FDOT, (int32*)&bonus_damage, spellInfo->GetSpellGroupType());
			SM_PIValue( caster->SM_Mods->SM_PDOT, (int32*)&TotalDotDMGMod, spellInfo->GetSpellGroupType());
			summaryPCTmod = summaryPCTmod * ( TotalDotDMGMod / 100.0f );
#ifdef COLLECTION_OF_UNTESTED_STUFF_AND_TESTERS
			int spell_flat_modifers=0;
			int spell_pct_modifers=0;
			SM_FIValue( caster->SM_Mods->SM_FDOT, (int32*)&spell_flat_modifers, spellInfo->GetSpellGroupType());
			SM_PIValue( caster->SM_Mods->SM_PDOT, (int32*)&spell_pct_modifers, spellInfo->GetSpellGroupType());
			if(spell_flat_modifers!=0 || spell_pct_modifers!=0)
				printf("!!!!!spell periodic dmg mod flat %d , spell dmg bonus pct %d , spell dmg bonus %d, spell group %u\n",spell_flat_modifers,spell_pct_modifers,dmg,gr);
#endif
		}
	}
	if( spellInfo->c_is_flags2 & SPELL_FLAG2_IS_ENEMY_AOE_TARGETTING )
	{
		summaryPCTmod = summaryPCTmod * pVictim->AoeDamageTakenPctMod[ school ] / 100.0f;
		if( IsCreature() )
			summaryPCTmod = summaryPCTmod * pVictim->AoeDamageTakenPctModFromCreatures / 100.0f;
	}
//	summaryPCTmod += caster->DamageDoneModPCT[school];	// BURLEX FIXME
	if( spell_effect_index > MAX_SPELL_EFFECT_COUNT )
		summaryPCTmod = summaryPCTmod * ( 1.0f + pVictim->ModDamageTakenByMechPCT[ spellInfo->MechanicsType ] / 100.0f );
	else 
		summaryPCTmod = summaryPCTmod * ( 1.0f + pVictim->ModDamageTakenByMechPCT[ spellInfo->eff[ spell_effect_index ].EffectMechanic ] / 100.0f );
	
//	int32 res = ((base_dmg+bonus_damage)*summaryPCTmod + bonus_damage*dmg_bonus_pct_mod/100);
	int32 res = ( bonus_damage * dmg_bonus_pct_mod / 100 );
//	int32 res = (base_dmg*(dmg_bonus_pct_mod-100)/100+(bonus_damage*dmg_bonus_pct_mod/100));// DK spells get no dmg bonus at all either threat those specially or include in calc base value also
	if( RetSumarryCoeff )
		*RetSumarryCoeff = summaryPCTmod;
	else
		res = float2int32( ( res + base_dmg ) * summaryPCTmod );	//!! value can be negative( ex : 50% reduction ) !!

	res += bonus_damage_final;
	return res;
}

int32 Unit::GetSpellHealBonus(Unit *pVictim, SpellEntry *spellInfo,int32 base_heal,int32 spell_effect_index, float *RetSumarryCoeff)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	int32 bonus = 0;
	uint32 school = spellInfo->School;

	if( spellInfo->Spell_value_is_scripted == true )
	{ 
		return 0;
	}

	//Basic bonus
	if( CanClassReceiveDMGOrHealBonus( ) )
	{
		if( this->IsPlayer() && SafePlayerCast( this )->m_APToSPExclusivePCT )
			bonus += SPELL_POWER_TO_HEALING_POWER_CONVERSION_FACTOR * GetAP() * SafePlayerCast( this )->m_APToSPExclusivePCT / 100;
		else
			bonus += HealDoneMod;
	}
	bonus += pVictim->HealTakenMod;

	//Bonus from Intellect & Spirit
	if( IsPlayer() )
	{
		Player *p_caster = SafePlayerCast(this);
		for(uint32 a = 0; a < 5; a++)
			if( p_caster->SpellHealDoneByAttribute[a] )
				bonus += float2int32(p_caster->SpellHealDoneByAttribute[a] * (float)p_caster->GetUInt32Value(UNIT_FIELD_STAT0 + a));

		bonus += float2int32( p_caster->GetAP() * p_caster->AP_to_HP_coeff );
	}

	int32 spell_power_bonus_pct = 0;
	if( SM_Mods )
		SM_FIValue( SM_Mods->SM_PPenalty, &spell_power_bonus_pct, spellInfo->GetSpellGroupType() );
	if( spell_power_bonus_pct )
		bonus += HealDoneMod * spell_power_bonus_pct / 100 / spellInfo->quick_tickcount;

	//Spell Coefficient
	if( spell_effect_index == -1 )
		bonus = float2int32( bonus * spellInfo->SpellPowerCoefAvg );	//!!note that every effect has it's own Coef !!! this is not the right way to calc coeff !!!
	else if( spellInfo->eff[ spell_effect_index ].EffectSpellPowerCoef > 0.0f )
	{
		if( spellInfo->eff[ spell_effect_index ].EffectSpellPowerCoef == SPELL_EFFECT_MAGIC_UNDECIDED_SPELL_POWER_COEFF )
			bonus = 0;
		else
			bonus = float2int32( bonus * spellInfo->eff[ spell_effect_index ].EffectSpellPowerCoef );
	}

	//dmg bonus mods whole value
	int32 SMT_PCT_mod = 100;
	if( Need_SM_Apply(spellInfo) && SM_Mods )
	{
		SM_FIValue( SM_Mods->SM_FDamageBonus,&bonus,spellInfo->GetSpellGroupType());
		SM_FIValue( SM_Mods->SM_PDamageBonus,&SMT_PCT_mod,spellInfo->GetSpellGroupType());
		if( spellInfo->quick_tickcount > 1 )
		{
			SM_FIValue( SM_Mods->SM_FDOT,&bonus,spellInfo->GetSpellGroupType());
			SM_FIValue( SM_Mods->SM_PDOT,&SMT_PCT_mod,spellInfo->GetSpellGroupType());
		}
	}

	int32 limited_mod = MAX(pVictim->HealTakenPctMod,-50);	//there is a logic in auras which stack and which not. THIS IS A HACKFIX !
//	bonus += base_heal * ( SMT_PCT_mod + HealDonePctMod + limited_mod ) / 100;
//	bonus += ( base_heal + bonus ) * ( SMT_PCT_mod + HealDonePctMod + limited_mod ) / 100;
	if( RetSumarryCoeff )
	{
		*RetSumarryCoeff = (float)( ( 0 + SMT_PCT_mod ) * ( 100 + HealDonePctMod ) * ( 100 + limited_mod ) ) / ( 100.0f * 100.0f * 100.0f );
	}
	else
		bonus += ( ( ( ( base_heal + bonus ) * ( 0 + SMT_PCT_mod )) / 100 * ( 100 + HealDonePctMod ) ) / 100 * ( 100 + limited_mod ) ) / 100;

	if( bonus < 0 )
		bonus = 0;
	
//	bonus = bonus / HOTticks;

	return bonus;
}

void Unit::InterruptSpell( uint64 OnlyIfTargettingMe, bool ClearCooldown )
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if(m_currentSpell)
	{
		if( OnlyIfTargettingMe != 0 && m_currentSpell->m_targets.m_unitTarget != OnlyIfTargettingMe )
			return;
		//some say interrupted casts should clear cooldown
		if( ClearCooldown && IsPlayer() )
			SafePlayerCast( this )->ClearCooldownForSpell( m_currentSpell->GetProto()->Id );
/*		//ok wtf is this
		//m_currentSpell->SendInterrupted(SPELL_FAILED_INTERRUPTED);
		//m_currentSpell->cancel();
		if(m_spellsbusy)
		{
			// shouldn't really happen. but due to spell sytem bugs there are some cases where this can happen.
			sEventMgr.AddEvent(this,&Unit::CancelSpell,m_currentSpell,EVENT_UNIT_DELAYED_SPELL_CANCEL,1,1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
			m_currentSpell=NULL;
		}
		else*/
			m_currentSpell->safe_cancel();
	}
}

void Unit::DeMorph()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	// hope it solves it :)
	uint32 displayid = this->GetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID);
	this->SetUInt32Value(UNIT_FIELD_DISPLAYID, displayid);
	EventModelChange();
}

void Unit::Emote(EmoteType emote)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if(emote == 0)
	{ 
		return;
	}

	sStackWorldPacket( data, SMSG_EMOTE, 20 );
	data << uint32(emote);
	data << this->GetGUID();
	SendMessageToSet (&data, true);
}

void Unit::SendChatMessageToPlayer(uint8 type, uint32 lang, const char *msg, Player *plr)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	size_t UnitNameLength = 0, MessageLength = 0;
	CreatureInfo *ci = (IsCreature()) ? SafeCreatureCast(this)->GetCreatureInfo() : NULL;

	if(ci == NULL || plr == NULL)
	{ 
		return;
	}

	UnitNameLength = strlen((char*)ci->Name) + 1;
	MessageLength = strlen((char*)msg) + 1;
	//wtf mem error ?
	if( UnitNameLength > 200 || MessageLength > 200 )
	{
		return; 
	}

	sStackWorldPacket( data, SMSG_MESSAGECHAT, 35 + 1000 );
	data << type;
	data << lang;
	data << GetGUID();
	data << uint32(0);			// new in 2.1.0
	data << uint32(UnitNameLength);
	data << ci->Name;
	data << uint64(0);
	data << uint32(MessageLength);
	data << msg;
	data << uint8(0x00);
	plr->GetSession()->SendPacket(&data);
}

void Unit::SendChatMessageAlternateEntry(uint32 entry, uint8 type, uint32 lang, const char * msg)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	size_t UnitNameLength = 0, MessageLength = 0;
	CreatureInfo *ci;

	ci = CreatureNameStorage.LookupEntry(entry);
	if(!ci)
	{ 
		return;
	}

	UnitNameLength = strlen((char*)ci->Name) + 1;
	MessageLength = strlen((char*)msg) + 1;
	//wtf mem error ?
	if( UnitNameLength > 200 || MessageLength > 200 )
	{
		return; 
	}

	sStackWorldPacket( data, SMSG_MESSAGECHAT, 750 );
	data << type;
	data << lang;
	data << GetGUID();
	data << uint32(0);			// new in 2.1.0
	data << uint32(UnitNameLength);
	data << ci->Name;
	data << uint64(0);
	data << uint32(MessageLength);
	data << msg;
	data << uint8(0x00);
	SendMessageToSet(&data, true);
}

void Unit::SendChatMessage(uint8 type, uint32 lang, const char *msg)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	size_t UnitNameLength = 0, MessageLength = 0;
	CreatureInfo *ci = (IsCreature()) ? SafeCreatureCast(this)->GetCreatureInfo() : NULL;

	if(ci == NULL)
	{ 
		return;
	}

	UnitNameLength = strlen((char*)ci->Name) + 1;
	MessageLength = strlen((char*)msg) + 1;
	//wtf mem error ?
	if( UnitNameLength > 200 || MessageLength > 200 )
	{
		return; 
	}

	sStackWorldPacket( data, SMSG_MESSAGECHAT, 35 + 1000 );
	data << type;
	data << lang;
	data << GetGUID();
	data << uint32(0);			// new in 2.1.0
	data << uint32(UnitNameLength);
	data << ci->Name;
	data << uint64(0);
	data << uint32(MessageLength);
	data << msg;
	data << uint8(0x00);
	SendMessageToSet(&data, true);
}

void Unit::WipeHateList()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
 
	GetAIInterface()->WipeHateList(); 
}
void Unit::ClearHateList()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	GetAIInterface()->ClearHateList();
}

void Unit::WipeTargetList() 
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
 
	GetAIInterface()->WipeTargetList(); 
}

#if 0
void Unit::AddInRangeObject(Object* pObj)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if(pObj->IsUnit() )
	{
//		if( isHostile( this, (Unit*)pObj ) )
//			m_oppFactsInRange.insert(pObj);

//		if ( isFriendly( this, (Unit*)pObj ) )
//			m_sameFactsInRange.insert(pObj);

		// commented - this code won't work anyway due to objects getting added in range before they are created - burlex
		/*if( GetTypeId() == TYPEID_PLAYER )
		{
			if( SafePlayerCast( this )->InGroup() )
			{
				if( pObj->GetTypeId() == TYPEID_UNIT )
				{
					if(((Creature*)pObj)->Tagged)
					{
						if( SafePlayerCast( this )->GetGroup()->HasMember( pObj->GetMapMgr()->GetPlayer( (uint32)((Creature*)pObj)->TaggerGuid ) ) )
						{
							uint32 Flags;
							Flags = ((Creature*)pObj)->m_uint32Values[UNIT_DYNAMIC_FLAGS];
							Flags |= U_DYN_FLAG_TAPPED_BY_PLAYER;
							ByteBuffer buf1(500);
							((Creature*)pObj)->BuildFieldUpdatePacket(&buf1, UNIT_DYNAMIC_FLAGS, Flags);
							SafePlayerCast( this )->PushUpdateData( &buf1, 1 );
						}
					}
				}
			}
		}		*/
	}

	Object::QueueAddInRangeObject(pObj);
}//427
#endif

void Unit::QueueRemoveInRangeObject(Object* pObj)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
//	m_oppFactsInRange.erase(pObj);
//	m_sameFactsInRange.erase(pObj);

	if( pObj->IsUnit() )
	{
		Unit *pUnit = SafeUnitCast(pObj);
		if( GetAIInterface() )
			GetAIInterface()->EventRemoveInrangeUnit( pUnit );

		if( GetUInt64Value(UNIT_FIELD_CHARM) == pObj->GetGUID() )
			if(m_currentSpell)
				m_currentSpell->safe_cancel( );

        if( critterPet == pObj )
		{
			critterPet->SafeDelete();
			critterPet = 0;
		}
	}
	Object::QueueRemoveInRangeObject(pObj);
}

//Events
void Unit::EventAddEmote(EmoteType emote, uint32 time)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	m_oldEmote = GetUInt32Value(UNIT_NPC_EMOTESTATE);
	SetUInt32Value(UNIT_NPC_EMOTESTATE,emote);
	sEventMgr.AddEvent(this, &Creature::EmoteExpire, EVENT_UNIT_EMOTE, time, 1,0);
}

void Unit::EmoteExpire()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	SetUInt32Value(UNIT_NPC_EMOTESTATE, m_oldEmote);
	sEventMgr.RemoveEvents(this, EVENT_UNIT_EMOTE);
}


uint32 Unit::GetResistance(uint32 type)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	return GetUInt32Value(UNIT_FIELD_RESISTANCES+type);
}

void Unit::MoveToWaypoint(uint32 wp_id)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if(this->m_useAI && this->GetAIInterface() != NULL)
	{
		AIInterface *ai = GetAIInterface();
		WayPoint *wp = ai->getWayPoint(wp_id);
		if(!wp)
		{
			sLog.outString("WARNING: Invalid WP specified in MoveToWaypoint.");
			return;
		}

		ai->m_currentWaypoint = wp_id;
		if(wp->flags!=0)
			ai->m_moveRun = true;
		ai->MoveTo(wp->x, wp->y, wp->z, 0);
	}
}

bool Unit::CanClassReceiveDMGOrHealBonus()
{
	if( IsCreature() )
		return true;
	//warriors, DKs should not have spell power bonus
	static bool class_power_enabler[12]={ false, false, true, false, false, true, false, true, true, true, true, true };
	if( getClass() < 12 )
		return class_power_enabler[ getClass() ];
	return false;
}

int32 Unit::GetDamageDoneMod(uint32 school, uint32 SchoolMask)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE

	//some class like warrior get dmg from AP and not SP
	if( CanClassReceiveDMGOrHealBonus() == false )
		return 0;

	if( SchoolMask != 0 && ( 1 << school ) != SchoolMask )
	{
		int32 MaxMod = 0;
		for( uint32 i=SCHOOL_NORMAL;i<SCHOOL_COUNT;i++)
			if( SchoolMask & ( 1 << i ) )
				MaxMod = MAX( MaxMod, GetDamageDoneMod( i, 0 ) );
		return MaxMod;
	}
	else if (school < SCHOOL_COUNT)
	{
		if( this->IsPlayer() )
		{ 
			//spell mental quickness for shaman
			if( SafePlayerCast( this )->m_APToSPExclusivePCT )
				return ( GetAP() * SafePlayerCast( this )->m_APToSPExclusivePCT / 100 );

		   int32 flat = (int32)GetUInt32Value( PLAYER_FIELD_MOD_DAMAGE_DONE_POS + school ) - (int32)GetUInt32Value( PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + school );
//		   for(int i=0;i<STAT_COUNT;i++)
//			   flat += SafePlayerCast( this )->DamageDoneByStatPCT[ i ] * GetStat( i ) / 100;
//		   flat += float2int32( GetAP() * SafePlayerCast( this )->AP_to_SP_coeff ); //not sure if this should be exclusive or included as base value as SP
		   float coef = GetFloatValue( PLAYER_FIELD_MOD_SPELL_POWER_PCT );
		   return float2int32( (float)flat * coef );
		}
		else
		   return SafeCreatureCast( this )->ModDamageDone[school];
	}
	else
		sLog.outDebug("[NOTICE] You have bad DB, spell school = %u\n",school);
	return 0;
}
	
float Unit::GetDamageDonePctMod(uint32 school, uint32 SchoolMask)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if( SchoolMask != 0 && ( 1 << school ) != SchoolMask )
	{
		float MaxMod = 0.0f;
		for( uint32 i=SCHOOL_NORMAL;i<SCHOOL_COUNT;i++)
			if( SchoolMask & ( 1 << i ) )
				MaxMod = MAX( MaxMod, GetDamageDonePctMod( i, 0 ) );
		return MaxMod;
	}
	else if (school < SCHOOL_COUNT)
	{
	    if(this->IsPlayer())
	    { 
		   return m_floatValues[PLAYER_FIELD_MOD_DAMAGE_DONE_PCT+school];
	    }
		else
		   return SafeCreatureCast(this)->ModDamageDonePct[school]/100.0f;
	}
	else
		sLog.outDebug("[NOTICE] You have bad DB, spell school = %u\n",school);
	return 0;
}

void Unit::CalcDamage()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if( IsPlayer() )
		SafePlayerCast( this )->CalcDamage();
	else
	{
		if( IsPet() )
		{
			SafePetCast(this)->UpdateSP();
			SafePetCast(this)->UpdateAP();
		}
		float r;
		float delta;
		float mult;
	
		float ap_bonus = float(GetAP())/14000.0f;

		float bonus = ap_bonus * ( GetUInt32Value(UNIT_FIELD_BASEATTACKTIME) + SafeCreatureCast( this )->m_speedFromHaste );
	
		delta = float(SafeCreatureCast(this)->ModDamageDone[0]);
		mult = float(SafeCreatureCast(this)->ModDamageDonePct[0])/100.0f;
		r = ( BaseDamage[0] + bonus ) * mult + delta;
		// give some diversity to pet damage instead of having a 77-78 damage range (as an example)
		SetFloatValue(UNIT_FIELD_MINDAMAGE,r > 0 ? ( IsPet() ? r * 0.9f : r ) : 0 );
		r = ( BaseDamage[1] + bonus ) * mult + delta;
		SetFloatValue(UNIT_FIELD_MAXDAMAGE, r > 0 ? ( IsPet() ? r * 1.1f : r ) : 0 );
	
	//	SetFloatValue(UNIT_FIELD_MINRANGEDDAMAGE,BaseRangedDamage[0]*mult+delta);
	//	SetFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE,BaseRangedDamage[1]*mult+delta);
  
	}	
}

//returns absorbed dmg
int32 Unit::ManaShieldAbsorb( int32 dmg )
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if( m_manashieldamt <= 0 || dmg <= 0 )
	{ 
		return 0;
	}
	//mana shield group->16. the only

	int32 mana = GetPower( POWER_TYPE_MANA );
	int32 effectbonus = 0;
//	SM_Mods->SM_PEffectBonus ? SM_Mods->SM_PEffectBonus[16] : 0;

	int32 ConversionPCT = 100;	//each spell has it's own conversion rate ! Right now only 3 spells use this effect
	int32 potential = (mana*ConversionPCT)/((100+effectbonus));
	if(potential>m_manashieldamt)
		potential = m_manashieldamt;

	if((int32)dmg<potential)
		potential = dmg;

	int32 cost = (potential*(100+effectbonus))/ConversionPCT;

//	SetPower( POWER_TYPE_MANA,mana-cost);
//	UpdatePowerAmm( true, POWER_TYPE_MANA ); //not sure this will work
	Energize( this, m_manaShieldId, -cost, POWER_TYPE_MANA, 0 );

	m_manashieldamt -= potential;
	if(!m_manashieldamt)
		RemoveAura(m_manaShieldId);
	return potential;	
}

int32 Unit::AbsorbDamage( uint32 School, int32 dmg )
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE

	if( School >= SPELL_SCHOOL_COUNT )
		return 0;
	
	if( bInvincible )
		return dmg;

	//avoid bugs
	if( dmg <= 0 )
		return 0;

	uint32 abs = 0;
	CommitPointerListNode<Absorb> *i;
	Absorbs.BeginLoop();
	for( i = Absorbs.begin(); i != Absorbs.end(); i = i->Next() )
	{
		Absorb *tabs = i->data;
		if(tabs->schools[ School ] )
		{
			uint32 toabsorb = dmg;
			if( tabs->MaxAbsorbCoeff )
			{
				float coef = tabs->MaxAbsorbCoeff;	//this is smaller then 1 !
				float tdmg = (float)(dmg);
				toabsorb = float2int32(coef*tdmg);
			}
			if( tabs->MaxAbsorbPerHit < toabsorb )
				toabsorb = tabs->MaxAbsorbPerHit;
//			if( tabs->MaxAbsorbEvents <= 0 )
//				continue;
//			tabs->MaxAbsorbEvents--;
			// toabsorb <= dmg
			if( (int32)toabsorb >= tabs->amt )//remove this absorb
			{
				//decrease dmg with amount max we can absorb
				dmg -= tabs->amt;
				abs += tabs->amt;
				tabs->amt = 0;			//mark to delte us

//				RemoveAura( tabs->spellid );
				//i need to see if power word shield is still active to be able to properly script some talents. Lame solution i know :(
//				sEventMgr.AddEvent( this, &Unit::RemoveAuraEvent, tabs->spellid, EVENT_DELAYED_SPELLCAST, 1, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
//				Aura *a = HasAura( tabs->spellid );
//				if( a )
//					a->RemoveDelayed();
				if( tabs->OwnerAur )
					tabs->OwnerAur->RemoveDelayed();
//				Absorbs.erase( i );		//also made in remove aura
				if( dmg <= 0 )//absorbed all dmg
					break;		
			}
/*			if( tabs->MaxAbsorbEvents <= 0 )
			{
				Aura *a = HasAura( tabs->spellid );
				if( a )
					a->RemoveDelayed();
			} */
			//absorb "full" dmg - well sometimes there is a fallthrough
			else
			{
				abs += toabsorb;
				tabs->amt -= toabsorb;
				dmg -= toabsorb;	//0 most of the time but can be maxabsorb or falltrough

				//update client side icon
				if( tabs->OwnerAur )
				{
					//can it have more then 1 absorbs ? they will never stack ? 
					for( uint32 i=0;i<tabs->OwnerAur->m_modcount;i++)
						if( tabs->OwnerAur->m_modList[i].m_AuraName == SPELL_AURA_SCHOOL_ABSORB )
						{
							tabs->OwnerAur->m_modList[i].m_amount -= toabsorb;
							tabs->OwnerAur->GetTarget()->ModVisualAuraStackCount( tabs->OwnerAur, 0 );
							break;
						}
				}

				break;
			}
		}
	}
	Absorbs.EndLoopAndCommit();

	return abs;
}

bool Unit::setDetectRangeMod(const uint64 &guid, int32 amount)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	int next_free_slot = -1;
	for(int i = 0; i < 5; i++)
	{
		if(m_detectRangeGUID[i] == 0 && next_free_slot == -1)
		{
			next_free_slot = i;
		}
		if(m_detectRangeGUID[i] == guid)
		{
			m_detectRangeMOD[i] = amount;
			return true;
		}
	}
	if(next_free_slot != -1)
	{
		m_detectRangeGUID[next_free_slot] = guid;
		m_detectRangeMOD[next_free_slot] = amount;
		return true;
	}
	return false;
}

void Unit::unsetDetectRangeMod(const uint64 &guid)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	for(int i = 0; i < 5; i++)
	{
		if(m_detectRangeGUID[i] == guid)
		{
			m_detectRangeGUID[i] = 0;
			m_detectRangeMOD[i] = 0;
		}
	}
}

int32 Unit::getDetectRangeMod(const uint64 &guid)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	for(int i = 0; i < 5; i++)
	{
		if(m_detectRangeGUID[i] == guid)
		{
			return m_detectRangeMOD[i];
		}
	}
	return 0;
}

void Unit::SetStandState(uint8 standstate)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	//only take actions if standstate did change.
	StandState bef = GetStandState();
	if( bef == standstate )
	{ 
		return;
	}

	SetByte( UNIT_FIELD_BYTES_1, 0, standstate );
	if( standstate == STANDSTATE_STAND )//standup
		RemoveAurasByInterruptFlag(AURA_INTERRUPT_ON_STAND_UP);

	if( m_objectTypeId == TYPEID_PLAYER )
		SafePlayerCast( this )->GetSession()->OutPacket( SMSG_STANDSTATE_UPDATE, 1, &standstate );
}

void Unit::DropAurasOnDeath()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	for( uint32 i=0;i<2;i++) //two times because some auras proc on remove and might revive us :(
	{
		for(uint32 x=FIRST_AURA_SLOT;x<MAX_AURAS;x++)
			if(m_auras[x])
			{
				if(	(m_auras[x]->GetSpellProto()->AttributesExC & CAN_PERSIST_AND_CASTED_WHILE_DEAD)
	//				&& m_auras[x]->IsPositive() 
					)
					continue;
				else
					m_auras[x]->Remove();
			}
	}
}

void Unit::UpdateSpeed()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if(GetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID) == 0)
	{
		if(IsPlayer())
			m_runSpeed = m_base_runSpeed*(1.0f + ((float)m_speedModifier)/100.0f);
		else
		{
			m_runSpeed = m_base_runSpeed*(1.0f + ((float)m_speedModifier)/100.0f);
		}
	}
	else
	{
		if(IsPlayer())
		{
			m_runSpeed = m_base_runSpeed*(1.0f + ((float)m_mountedspeedModifier)/100.0f);
			m_runSpeed += (m_speedModifier<0) ? (m_base_runSpeed*((float)m_speedModifier)/100.0f) : 0;
		}
		else
		{
			m_runSpeed = m_base_runSpeed*(1.0f + ((float)m_mountedspeedModifier)/100.0f);
			m_runSpeed += (m_speedModifier<0) ? (m_base_runSpeed*((float)m_speedModifier)/100.0f) : 0;
		}
	}

	if( IsCreature() && SafeCreatureCast(this)->GetProto() )
		m_flySpeed = SafeCreatureCast(this)->GetProto()->fly_speed *(1.0f + ((float)m_flyspeedModifier)/100.0f);
	else 
		m_flySpeed = PLAYER_NORMAL_FLIGHT_SPEED*(1.0f + ((float)m_flyspeedModifier)/100.0f);

	// Limit speed due to effects such as http://www.wowhead.com/?spell=31896 [Judgement of Justice]
	if( m_maxSpeed && m_runSpeed > m_maxSpeed )
	{
		m_runSpeed = m_maxSpeed;
		if( m_runSpeed <= 0.0f )
			m_runSpeed = m_minSpeed;
	}
	if( m_minSpeed && m_runSpeed < m_minSpeed )
		m_runSpeed = m_minSpeed;
	if( m_runSpeed <= 0.0f )
		m_runSpeed = 1.0f;

	if(IsPlayer())
	{
		if(SafePlayerCast(this)->m_changingMaps)
			SafePlayerCast( this )->resend_speed = true;
		else
		{
			SafePlayerCast( this )->SetPlayerSpeed(RUN, m_runSpeed);
			SafePlayerCast( this )->SetPlayerSpeed(FLY, m_flySpeed);
		}
	}
}

void Unit::EventSummonPetExpire()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if(summonPet)
	{
		if(summonPet->GetEntry() == 7915)//Goblin Bomb
		{
			SpellEntry *spInfo = dbcSpell.LookupEntry(13259);
			if(!spInfo)
			{ 
				return;
			}

			Spell*sp=SpellPool.PooledNew( __FILE__, __LINE__ );
			sp->Init(summonPet,spInfo,true,NULL);
			SpellCastTargets tgt;
			tgt.m_unitTarget=summonPet->GetGUID();
			sp->prepare(&tgt);
		}
		else
		{
			summonPet->RemoveFromWorld(false, true);
			sGarbageCollection.AddObject( summonPet );
			summonPet = NULL;
		}
	}
	sEventMgr.RemoveEvents(this, EVENT_SUMMON_PET_EXPIRE);
}

uint8 Unit::CastSpell(Unit* Target, SpellEntry* Sp, bool triggered)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if( Sp == NULL )
	{ 
		return SPELL_FAILED_UNKNOWN;
	}

	Spell *newSpell = SpellPool.PooledNew( __FILE__, __LINE__ );
	newSpell->Init(this, Sp, triggered, 0);
	SpellCastTargets targets(0);
	if(Target)
	{
		targets.m_targetMask |= TARGET_FLAG_UNIT;
		targets.m_unitTarget = Target->GetGUID();
		targets.m_targetMask |= TARGET_FLAG_DEST_LOCATION;
		targets.m_destX = Target->GetPositionX();
		targets.m_destY = Target->GetPositionY();
		targets.m_destZ = Target->GetPositionZ();
	}
	else
	{
		newSpell->GenerateTargets(&targets);
	}
	return newSpell->prepare(&targets);
}

uint8 Unit::CastSpell(Unit* Target, uint32 SpellID, bool triggered)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	SpellEntry * ent = dbcSpell.LookupEntry(SpellID);
	if(ent == NULL) 
	{ 
		return SPELL_FAILED_UNKNOWN;
	}

	return CastSpell(Target, ent, triggered);
}

uint8 Unit::CastSpell(const uint64 &targetGuid, SpellEntry* Sp, bool triggered)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if( Sp == NULL )
	{ 
		return SPELL_FAILED_UNKNOWN;
	}

	SpellCastTargets targets(targetGuid);
	Spell *newSpell = SpellPool.PooledNew( __FILE__, __LINE__ );
	newSpell->Init(this, Sp, triggered, 0);
	return newSpell->prepare(&targets);
}

uint8 Unit::CastSpell(uint64 targetGuid, uint32 SpellID, bool triggered)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	SpellEntry * ent = dbcSpell.LookupEntry(SpellID);
	if(ent == 0) 
	{ 
		return SPELL_FAILED_UNKNOWN;
	}

	return CastSpell(targetGuid, ent, triggered);
}
void Unit::CastSpellAoF(Unit *SuggestedTarget, float x ,float y, float z, SpellEntry* Sp, bool triggered)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if( Sp == NULL )
	{ 
		return;
	}

	SpellCastTargets targets;
	targets.m_destX = x;
	targets.m_destY = y;
	targets.m_destZ = z;
	targets.m_targetMask=TARGET_FLAG_DEST_LOCATION;
	if( SuggestedTarget != NULL )
	{
		targets.m_targetMask |= TARGET_FLAG_UNIT;
		targets.m_unitTarget = SuggestedTarget->GetGUID();
	}
	Spell *newSpell = SpellPool.PooledNew( __FILE__, __LINE__ );
	newSpell->Init(this, Sp, triggered, 0);
	newSpell->prepare(&targets);
}

void Unit::PlaySpellVisual(const uint64 &target, uint32 spellVisual)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	WorldPacket data(SMSG_PLAY_SPELL_VISUAL, 12);
	data << target << spellVisual;
	SendMessageToSet(&data, true);
}

void Unit::Root()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	this->m_special_state |= UNIT_STATE_ROOT;

	if(m_objectTypeId == TYPEID_PLAYER)
	{
		SafePlayerCast( this )->SetMovement(MOVE_ROOT);
	}
	else
	{
		m_aiInterface->m_canMove = false;
		m_aiInterface->StopMovement(1);
	}
}

void Unit::Unroot()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	this->m_special_state &= ~UNIT_STATE_ROOT;

	if(m_objectTypeId == TYPEID_PLAYER)
	{
		SafePlayerCast( this )->SetMovement(MOVE_UNROOT);
	}
	else
	{
		m_aiInterface->m_canMove = true;
	}
}

void Unit::RemoveAurasByBuffType(uint32 buff_type, const uint64 &guid, uint32 skip, bool skip_caster_check)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if( skip_caster_check == true || guid == 0 )
	{
		for(uint32 x=FIRST_AURA_SLOT;x<m_auras_pos_size;x++)
			if(m_auras[x] && (m_auras[x]->GetSpellProto()->BGR_one_buff_on_target & buff_type) && m_auras[x]->GetSpellId()!=skip)
					m_auras[x]->Remove();
		for(uint32 x=MAX_POSITIVE_AURAS;x<m_auras_neg_size;x++)
			if(m_auras[x] && (m_auras[x]->GetSpellProto()->BGR_one_buff_on_target & buff_type) && m_auras[x]->GetSpellId()!=skip)
					m_auras[x]->Remove();
		//windfury is passive :(
		for(uint32 x=MAX_AURAS;x<m_auras_pas_size;x++)
			if(m_auras[x] && (m_auras[x]->GetSpellProto()->BGR_one_buff_on_target & buff_type) && m_auras[x]->GetSpellId()!=skip)
					m_auras[x]->Remove();
	}
	else
	{
		for(uint32 x=FIRST_AURA_SLOT;x<m_auras_pos_size;x++)
			if(m_auras[x] && (m_auras[x]->GetSpellProto()->BGR_one_buff_on_target & buff_type) && m_auras[x]->GetSpellId()!=skip)
				if(m_auras[x]->m_casterGuid == guid)
					m_auras[x]->Remove();
		for(uint32 x=MAX_POSITIVE_AURAS;x<m_auras_neg_size;x++)
			if(m_auras[x] && (m_auras[x]->GetSpellProto()->BGR_one_buff_on_target & buff_type) && m_auras[x]->GetSpellId()!=skip)
				if(m_auras[x]->m_casterGuid == guid)
					m_auras[x]->Remove();
		//windfury is passive :(
		for(uint32 x=MAX_AURAS;x<m_auras_pas_size;x++)
			if(m_auras[x] && (m_auras[x]->GetSpellProto()->BGR_one_buff_on_target & buff_type) && m_auras[x]->GetSpellId()!=skip)
					m_auras[x]->Remove();
	}
}

void Unit::RemoveAurasByBuffIndexType(uint32 buff_index_type, const uint64 &guid)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	for(uint32 x=FIRST_AURA_SLOT;x<m_auras_pos_size;x++)
		if(m_auras[x] && m_auras[x]->GetSpellProto()->BGR_one_buff_from_caster_on_1target == buff_index_type)
			if(!guid || (guid && m_auras[x]->m_casterGuid == guid))
				m_auras[x]->Remove();
	for(uint32 x=MAX_POSITIVE_AURAS;x<m_auras_neg_size;x++)
		if(m_auras[x] && m_auras[x]->GetSpellProto()->BGR_one_buff_from_caster_on_1target == buff_index_type)
			if(!guid || (guid && m_auras[x]->m_casterGuid == guid))
				m_auras[x]->Remove();
}

AuraCheckResponse Unit::AuraCheck(Aura *new_aura, Aura *old_aura, uint32 MaxStackNow)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	AuraCheckResponse resp;

	//aaah come on, noway to trigger this ?
/*	if( new_aura == NULL || old_aura == NULL )
	{
		resp.Error = AURA_CHECK_RESULT_NONE;
//		resp.Misc  = 0;
		return resp;
	}/**/
	const	SpellEntry *nasp = new_aura->GetSpellProto();
	const	SpellEntry *oasp = old_aura->GetSpellProto();
	bool	ShareSameGroup = false;
	if( nasp->HasStackGroupFlags & oasp->HasStackGroupFlags )
	{
		for(uint32 i=0;i<MAX_SPELL_STACK_GROUPS;i++)
			if( nasp->StackGroupMasks[i] & oasp->StackGroupMasks[i] )
			{
				ShareSameGroup = true;
				break;	//help compiler unroll loop ?
			}
	}
	if( ShareSameGroup == false )
	{
		if( nasp->NameHash != oasp->NameHash //compare auras of same type only
	//		|| nasp->activeIconID == 0		//non visible auras always stack ?
			|| old_aura->m_visualSlot > MAX_VISIBLE_AURAS		//invisible auras stack
			|| MaxStackNow > m_auraStackCount[ old_aura->m_visualSlot ] //they could still stack more
			|| nasp->maxstack != oasp->maxstack //stacking rule does not match
			|| ( new_aura->m_casterGuid != old_aura->m_casterGuid && (oasp->c_is_flags & ( SPELL_FLAG_IS_DAMAGING | SPELL_FLAG_IS_STACKABLE_OTHER_CASTER_POS ) ) != 0 )//damaging spells always stack if not from same caster
			)
		{
			resp.Error = AURA_CHECK_RESULT_NONE;
	//		resp.Misc  = 0;
			return resp;
		}
	}
	resp.Error = AURA_CHECK_RESULT_HIGHER_BUFF_PRESENT;
	uint32 min_modcount = MIN(MIN( new_aura->m_modcount, old_aura->m_modcount ),MAX_AURA_MOD_COUNT);
	for( uint32 i=0;i<min_modcount;i++)
		if( new_aura->m_modList[i].m_amount > old_aura->m_modList[i].m_amount ) //for negative auras we should use ABS
		{
			resp.Error = AURA_CHECK_RESULT_LOWER_BUFF_PRESENT;
			break;
		}
	if( 
		resp.Error == AURA_CHECK_RESULT_HIGHER_BUFF_PRESENT										//not decided strength yet
//		&& nasp->NameHash == oasp->NameHash			//compare auras of same type only
		&& old_aura->m_visualSlot < MAX_VISIBLE_AURAS											//invisible auras stack
		//aura has not yet been added, no visual slot
		&& new_aura->IsVisible() == true														//proc spells, 1 is visible the one is not
		&& ( (oasp->c_is_flags & SPELL_FLAG_IS_DAMAGING) == 0				//damaging spells always stack if not from same caster
				|| new_aura->m_casterGuid == old_aura->m_casterGuid )
		&& MaxStackNow <= m_auraStackCount[ old_aura->m_visualSlot ]							//they could still stack more
		&& nasp->maxstack == oasp->maxstack			//stacking rule must match
		)
	{
		resp.Error = AURA_CHECK_RESULT_CANNOT_STACK;
//		resp.Misc  = 0;
		return resp;
	}
	return resp;
}

AuraCheckResponse Unit::AuraCheck(uint32 name_hash, uint32 rank, Object *caster)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	AuraCheckResponse resp;

	// no error for now
	resp.Error = AURA_CHECK_RESULT_NONE;
//	resp.Misc  = 0;

	// look for spells with same namehash
	for(uint32 x=FIRST_AURA_SLOT;x<m_auras_pos_size;x++)
	{
		Aura *a=m_auras[x];
//		if(m_auras[x] && m_auras[x]->GetSpellProto()->NameHash == name_hash && m_auras[x]->GetCaster()==caster)
		if(a && a->GetSpellProto()->NameHash == name_hash
			//check if we can stack it more
			&& (a->GetSpellProto()->maxstack < 1 || a->m_visualSlot > MAX_VISIBLE_AURAS || a->GetSpellProto()->maxstack <= m_auraStackCount[ a->m_visualSlot ] )
			)
		{
			// we've got an aura with the same name as the one we're trying to apply
//			resp.Misc = m_auras[x]->GetSpellProto()->Id;

			// compare the rank to our applying spell
			if(a->GetSpellProto()->RankNumber > rank)
				resp.Error = AURA_CHECK_RESULT_HIGHER_BUFF_PRESENT;
			else
				resp.Error = AURA_CHECK_RESULT_LOWER_BUFF_PRESENT;

			// we found something, save some loops and exit
			return resp;
		}
	}
	for(uint32 x=MAX_POSITIVE_AURAS;x<m_auras_neg_size;x++)
	{
		Aura *a=m_auras[x];
//		if(m_auras[x] && m_auras[x]->GetSpellProto()->NameHash == name_hash && m_auras[x]->GetCaster()==caster)
		if(a && a->GetSpellProto()->NameHash == name_hash
			//check if we can stack it more
			&& (a->GetSpellProto()->maxstack < 1 || a->m_visualSlot > MAX_VISIBLE_AURAS || a->GetSpellProto()->maxstack <= m_auraStackCount[ a->m_visualSlot ] )
			)
		{
			// we've got an aura with the same name as the one we're trying to apply
//			resp.Misc = m_auras[x]->GetSpellProto()->Id;

			// compare the rank to our applying spell
			if(a->GetSpellProto()->RankNumber > rank)
				resp.Error = AURA_CHECK_RESULT_HIGHER_BUFF_PRESENT;
			else
				resp.Error = AURA_CHECK_RESULT_LOWER_BUFF_PRESENT;

			// we found something, save some loops and exit
			return resp;
		}
	}

	// return it back to our caller
	return resp;
}
/*
AuraCheckResponse Unit::AuraCheck(uint32 name_hash, uint32 rank, Aura* aur, Object *caster)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	AuraCheckResponse resp;

	// no error for now
	resp.Error = AURA_CHECK_RESULT_NONE;
	resp.Misc  = 0;

	// look for spells with same namehash
//	if(aur->GetSpellProto()->NameHash == name_hash && aur->GetCaster()==caster)
	if(aur->GetSpellProto()->NameHash == name_hash)
	{
		// we've got an aura with the same name as the one we're trying to apply
		resp.Misc = aur->GetSpellProto()->Id;

		// compare the rank to our applying spell
		if(aur->GetSpellProto()->RankNumber > rank)
			resp.Error = AURA_CHECK_RESULT_HIGHER_BUFF_PRESENT;
		else
			resp.Error = AURA_CHECK_RESULT_LOWER_BUFF_PRESENT;
	}

	// return it back to our caller
	return resp;
}*/

void Unit::OnPushToWorld()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	for(uint32 x=FIRST_AURA_SLOT;x<MAX_AURAS + MAX_PASSIVE_AURAS;x++)
		if( m_auras[x] != NULL )
			m_auras[x]->event_Relocate();
}

void Unit::RemoveFromWorld(bool free_guid)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	CombatStatus.ClearAttackers();
	sHookInterface.OnRemoveFromWorld( this );
	if(critterPet != 0)
	{
		critterPet->RemoveFromWorld(false, true);
		critterPet = 0;
	}
	ExitVehicle();

//	if(dynObj != 0)
	{
//		dynObj->Remove();
//		dynObj = NULL;
	}

	for(uint32 i = 0; i < 4; ++i)
	{
		if(m_ObjectSlots[i] != 0)
		{
			GameObject * obj = m_mapMgr->GetGameObject(m_ObjectSlots[i]);
			if(obj)
				obj->ExpireAndDelete();

			m_ObjectSlots[i] = 0;
		}
	}

	DismissAllGuardians();

	//area auras expire when we leave world
	for(uint32 x = FIRST_AURA_SLOT; x < MAX_AURAS+MAX_PASSIVE_AURAS; ++x)
		if( m_auras[x] != NULL )
		{
			m_auras[x]->RemoveAA();	//!! this removes auras only from targets. We ar enot included here !
			if( m_auras[x] && ( m_auras[x]->GetSpellProto()->c_is_flags2 & SPELL_FLAG2_IS_MAP_CAST_BOUND ) )
				m_auras[x]->Remove();
		}

	//no need to port casting state from one map to another ( i hope )
	if( m_currentSpell )
	{
		m_currentSpell->safe_cancel( true );	//on next update the spell will be removed ...
		//just in case we are fully leaving world then not leave a leaked spell
//		m_currentSpell->cancel();
		m_currentSpell = NULL;
	}

	Object::RemoveFromWorld(free_guid);

	for(uint32 x = FIRST_AURA_SLOT; x < MAX_AURAS+MAX_PASSIVE_AURAS; ++x)
	{
		Aura *a = m_auras[x];
		if( a != NULL )
		{
			//how can this happen ?
			if( a->m_deleted != 0 )
			{
//				m_auras[x] = NULL;
				printf("!!OMG, this aura is deleted but it is still on player! %d - %s \n",a->GetSpellProto()->Id,a->GetSpellProto()->Name);
				continue;
			}
			//!!!relocate after remove from world
			a->event_Relocate();
		}
	}

	if( m_aiInterface )
		m_aiInterface->WipeReferences();
}

void Unit::RemoveAurasByInterruptFlagButSkip_slot(uint32 flag, uint32 skip,uint32 skiphash,uint32 slot, uint32 dmg)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	Aura * a;
	a = m_auras[slot];
	//some spells do not get removed all the time only at specific intervals
	if( (a->m_spellProto->AuraInterruptFlags & flag) && (a->m_spellProto->Id != skip) )
	{
		//last minute chance to script the removal avoidance
		if( a->m_spellProto->AuraInterruptHandler && a->m_spellProto->AuraInterruptHandler( this,a,flag,skip,skiphash, dmg ) == false )
			return;
		a->Remove();
	}
}

void Unit::RemoveAurasByInterruptFlagButSkip(uint32 flag, uint32 skip,uint32 skiphash, uint32 dmg)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	for(uint32 x=FIRST_AURA_SLOT;x<m_auras_pos_size;x++)
		if( m_auras[x] )
			RemoveAurasByInterruptFlagButSkip_slot(flag,skip,skiphash,x,dmg);
	for(uint32 x=MAX_POSITIVE_AURAS;x<m_auras_neg_size;x++)
		if( m_auras[x] )
			RemoveAurasByInterruptFlagButSkip_slot(flag,skip,skiphash,x,dmg);
//	for(uint32 x=MAX_AURAS;x<m_auras_pas_size;x++)
//		if( m_auras[x] )
//			RemoveAurasByInterruptFlagButSkip_slot(flag,skip,skiphash,x);
}

bool CheckAuraHasMissingRequiredItem( uint32 Class, uint32 SubClass, Aura *a )
{
	if( a )
	{
		int32 RequiredItemClass = a->GetSpellProto()->GetEquippedItemClass();
		if( RequiredItemClass != -1 )
		{
			if( Class != RequiredItemClass )
				return true;
			int32 RequiredItemSubClass = a->GetSpellProto()->GetEquippedItemSubClass();
			if( RequiredItemSubClass != -1 )
			{
				if( ( ( 1 << SubClass ) & RequiredItemClass ) == 0 )
					return true;
			}
		}
	}
	return false;
}

void Unit::RemoveAurasByInterruptItemChange( uint32 Class, uint32 SubClass )
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if( this->IsPlayer() == false )
		return;
	for(uint32 x=FIRST_AURA_SLOT;x<m_auras_pos_size;x++)
	{
		Aura *a = m_auras[x];
		if( CheckAuraHasMissingRequiredItem( Class, SubClass, a ) == true )
			a->Remove();
	}
}

/*
void Unit::RemoveAurasByInterruptFlag(uint32 flag)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	Aura * a;
	for(uint32 x=FIRST_AURA_SLOT;x<m_auras_pos_size;x++)
	{
		a = m_auras[x];
		if(a == NULL)
			continue;
		
		//some spells do not get removed all the time only at specific intervals
		if((a->m_spellProto->AuraInterruptFlags & flag) && !(a->m_spellProto->procFlags2 & PROC2_REMOVEONUSE))
		{
			a->Remove();
//			m_auras[x] = NULL;	//remove might trigger aura add and put it in same slot as the removed one. Do not use this !
		}
	}
	for(uint32 x=MAX_POSITIVE_AURAS;x<m_auras_neg_size;x++)
	{
		a = m_auras[x];
		if(a == NULL)
			continue;
		
		//some spells do not get removed all the time only at specific intervals
		if((a->m_spellProto->AuraInterruptFlags & flag) && !(a->m_spellProto->procFlags2 & PROC2_REMOVEONUSE))
		{
			a->Remove();
//			m_auras[x] = NULL;	//remove might trigger aura add and put it in same slot as the removed one. Do not use this !
		}
	}
}*/

void Unit::RemoveAurasByZoneChange()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if( !IsPlayer() || !IsInWorld() )
		return; //right now zoning is not made for non players :(
//	uint16 plrarea = SafePlayerCast(this)->GetMapMgr()->GetAreaID( GetPositionX(), GetPositionY() );
    uint16 plrarea = sTerrainMgr.GetAreaID( GetMapId(), GetPositionX(), GetPositionY(), GetPositionZ() );
	for(uint32 x=FIRST_AURA_SLOT;x<m_auras_pos_size;x++)
		if(m_auras[x] && m_auras[x]->GetSpellProto()->GetRequiresAreaId() )
		{
			AreaGroup *ag = dbcAreaGroup.LookupEntry( m_auras[x]->GetSpellProto()->GetRequiresAreaId() );
			if( !ag )
				continue;
			for( uint8 i = 0; i < 7; i++ )
				if( ag->AreaId[i] != 0 && ag->AreaId[i] != plrarea )
				{	
				    m_auras[x]->Remove();
					break;
				}
		}
}

void Unit::RemoveDuelAuras()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	for(uint32 x=FIRST_AURA_SLOT;x<m_auras_pos_size;x++)
		if(m_auras[x] && m_auras[x]->WasCastInDuel() )
			m_auras[x]->Remove();
	for(uint32 x=MAX_POSITIVE_AURAS;x<m_auras_neg_size;x++)
		if(m_auras[x] && m_auras[x]->WasCastInDuel() )
			m_auras[x]->Remove();
	for(uint32 x=MAX_AURAS;x<m_auras_pas_size;x++)
		if(m_auras[x] && m_auras[x]->WasCastInDuel() )
			m_auras[x]->Remove();
}

void Unit::RemoveBGAuras()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	for(uint32 x=FIRST_AURA_SLOT;x<m_auras_pos_size;x++)
		if(m_auras[x] 
			&& m_auras[x]->WasCastInBG() 
			&& m_auras[x]->IsPassive() == false
				)
			m_auras[x]->Remove();
	for(uint32 x=MAX_POSITIVE_AURAS;x<m_auras_neg_size;x++)
		if(m_auras[x] && m_auras[x]->WasCastInBG() )
			m_auras[x]->Remove();
	for(uint32 x=MAX_AURAS;x<m_auras_pas_size;x++)
		if(m_auras[x] 
			&& m_auras[x]->WasCastInBG() 
			&& m_auras[x]->IsPassive() == false
				)
			m_auras[x]->Remove();
}

Aura *Unit::HasAuraWithNameHash(uint32 name_hash,const uint64 &guid, uint32 SearchFlags )
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if( guid == 0 )
	{
		if( SearchFlags & AURA_SEARCH_POSITIVE )
			for(uint32 x=FIRST_AURA_SLOT;x<m_auras_pos_size;x++)
				if(m_auras[x] && m_auras[x]->GetSpellProto()->NameHash == name_hash)
				{ 
					return m_auras[x];
				}
		if( SearchFlags & AURA_SEARCH_NEGATIVE )
			for(uint32 x=MAX_POSITIVE_AURAS;x<m_auras_neg_size;x++)
				if(m_auras[x] && m_auras[x]->GetSpellProto()->NameHash == name_hash)
				{ 
					return m_auras[x];
				}
		if( SearchFlags & AURA_SEARCH_PASSIVE )
			for(uint32 x=MAX_AURAS;x<m_auras_pas_size;x++)
				if(m_auras[x] && m_auras[x]->GetSpellProto()->NameHash == name_hash)
					return m_auras[x];
	}
	else
	{
		if( SearchFlags & AURA_SEARCH_POSITIVE )
			for(uint32 x=FIRST_AURA_SLOT;x<m_auras_pos_size;x++)
				if(m_auras[x] && m_auras[x]->GetSpellProto()->NameHash == name_hash && m_auras[x]->m_casterGuid == guid )
				{ 
					return m_auras[x];
				}
		if( SearchFlags & AURA_SEARCH_NEGATIVE )
			for(uint32 x=MAX_POSITIVE_AURAS;x<m_auras_neg_size;x++)
				if(m_auras[x] && m_auras[x]->GetSpellProto()->NameHash == name_hash && m_auras[x]->m_casterGuid == guid )
				{ 
					return m_auras[x];
				}
		if( SearchFlags & AURA_SEARCH_PASSIVE )
			for(uint32 x=MAX_AURAS;x<m_auras_pas_size;x++)
				if(m_auras[x] && m_auras[x]->GetSpellProto()->NameHash == name_hash && m_auras[x]->m_casterGuid == guid )
					return m_auras[x];
	}

	return 0;
}

Aura* Unit::HasAura(uint32 spellId, const uint64 &guid, uint32 SearchFlags)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if( guid != 0 )
	{
		if( SearchFlags & AURA_SEARCH_POSITIVE )
			for(uint32 x=FIRST_AURA_SLOT;x<m_auras_pos_size;x++)
				if(m_auras[x] && m_auras[x]->GetSpellId() == spellId && m_auras[x]->m_casterGuid == guid)
				{ 
						return m_auras[x];
				}
		if( SearchFlags & AURA_SEARCH_NEGATIVE )
			for(uint32 x=MAX_POSITIVE_AURAS;x<m_auras_neg_size;x++)
				if(m_auras[x] && m_auras[x]->GetSpellId() == spellId && m_auras[x]->m_casterGuid == guid)
				{ 
						return m_auras[x];
				}
		if( SearchFlags & AURA_SEARCH_PASSIVE )
			for(uint32 x=MAX_AURAS;x<m_auras_pas_size;x++)
				if(m_auras[x] && m_auras[x]->GetSpellId() == spellId && m_auras[x]->m_casterGuid == guid)
				{ 
						return m_auras[x];
				}
	}
	else
	{
		if( SearchFlags & AURA_SEARCH_POSITIVE )
			for(uint32 x=FIRST_AURA_SLOT;x<m_auras_pos_size;x++)
				if(m_auras[x] && m_auras[x]->GetSpellId()==spellId)
				{ 
					return m_auras[x];
				}
		if( SearchFlags & AURA_SEARCH_NEGATIVE )
			for(uint32 x=MAX_POSITIVE_AURAS;x<m_auras_neg_size;x++)
				if(m_auras[x] && m_auras[x]->GetSpellId()==spellId)
				{ 
					return m_auras[x];
				}
		if( SearchFlags & AURA_SEARCH_PASSIVE )
			for(uint32 x=MAX_AURAS;x<m_auras_pas_size;x++)
				if(m_auras[x] && m_auras[x]->GetSpellId()==spellId)
				{ 
					return m_auras[x];
				}
	}
	return NULL;
}

Aura *Unit::HasAuraWithMechanic(uint32 mechanic, uint32 SearchFlags, uint32 MechanicFlags)
{
	if( MechanicFlags == 0 )
	{
		if( SearchFlags & AURA_SEARCH_POSITIVE )
			for(uint32 x=FIRST_AURA_SLOT;x<m_auras_pos_size;x++)
				if(m_auras[x] && m_auras[x]->GetSpellProto()->MechanicsType == mechanic )
				{ 
					return m_auras[x];
				}
		if( SearchFlags & AURA_SEARCH_NEGATIVE )
			for(uint32 x=MAX_POSITIVE_AURAS;x<m_auras_neg_size;x++)
				if(m_auras[x] && m_auras[x]->GetSpellProto()->MechanicsType == mechanic )
				{ 
					return m_auras[x];
				}
		if( SearchFlags & AURA_SEARCH_PASSIVE )
			for(uint32 x=MAX_AURAS;x<m_auras_pas_size;x++)
				if(m_auras[x] && m_auras[x]->GetSpellProto()->MechanicsType == mechanic )
					return m_auras[x];
	}
	else
	{
		if( SearchFlags & AURA_SEARCH_POSITIVE )
			for(uint32 x=FIRST_AURA_SLOT;x<m_auras_pos_size;x++)
				if( m_auras[x] && ( m_auras[x]->GetSpellProto()->MechanicsTypeFlags & MechanicFlags ) )
				{ 
					return m_auras[x];
				}
		if( SearchFlags & AURA_SEARCH_NEGATIVE )
			for(uint32 x=MAX_POSITIVE_AURAS;x<m_auras_neg_size;x++)
				if( m_auras[x] && ( m_auras[x]->GetSpellProto()->MechanicsTypeFlags & MechanicFlags ) )
				{ 
					return m_auras[x];
				}
		if( SearchFlags & AURA_SEARCH_PASSIVE )
			for(uint32 x=MAX_AURAS;x<m_auras_pas_size;x++)
				if( m_auras[x] && ( m_auras[x]->GetSpellProto()->MechanicsTypeFlags & MechanicFlags ) )
					return m_auras[x];
	}

	return NULL;
}

Aura *Unit::HasAuraWithType( uint32 Type, const uint64 &guid, uint32 SearchFlags )
{
	if( SearchFlags & AURA_SEARCH_POSITIVE )
		for(uint32 x=FIRST_AURA_SLOT;x<m_auras_pos_size;x++)
		{
			Aura *a = m_auras[x];
			if( a == NULL )
				continue;
			for( uint32 i=0;i<a->m_modcount;i++)
				if( a->m_modList[i].m_AuraName == Type && ( guid == 0 || a->GetCasterGUID() == guid ) )
					return a;
		}
	if( SearchFlags & AURA_SEARCH_NEGATIVE )
		for(uint32 x=MAX_POSITIVE_AURAS;x<m_auras_neg_size;x++)
		{
			Aura *a = m_auras[x];
			if( a == NULL )
				continue;
			for( uint32 i=0;i<a->m_modcount;i++)
				if( a->m_modList[i].m_AuraName == Type && ( guid == 0 || a->GetCasterGUID() == guid ) )
					return a;
		}
	if( SearchFlags & AURA_SEARCH_PASSIVE )
		for(uint32 x=MAX_AURAS;x<m_auras_pas_size;x++)
		{
			Aura *a = m_auras[x];
			if( a == NULL )
				continue;
			for( uint32 i=0;i<a->m_modcount;i++)
				if( a->m_modList[i].m_AuraName == Type && ( guid == 0 || a->GetCasterGUID() == guid ) )
					return a;
		}

	return NULL;
}

void UpdateHealAbsorbAuras( Unit *pVictim, int32 absorb )
{
	for(uint32 x=MAX_POSITIVE_AURAS;x<pVictim->m_auras_neg_size;x++)
	{
		Aura *a = pVictim->m_auras[x];
		if( a == NULL )
			continue;
		for( uint32 i=0;i<a->m_modcount;i++)
			if( a->m_modList[i].m_AuraName == SPELL_AURA_MOD_HEAL_ABSORB )
			{
				if( a->m_modList[i].m_amount > absorb )
				{
					a->m_modList[i].m_amount -= absorb;
					pVictim->ModVisualAuraStackCount( a, 0 );
				}
				else
				{
					absorb -= a->m_modList[i].m_amount;
					a->m_modList[i].m_amount = 0;
					a->RemoveDelayed();
					break;
				}
			}
	}
}

uint32 Unit::CountAura(uint32 spell_id, uint32 SearchFlags)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	uint32 ret=0;
	if( SearchFlags & AURA_SEARCH_POSITIVE )
		for(uint32 x = FIRST_AURA_SLOT; x < m_auras_pos_size; ++x)
			if(m_auras[x] && m_auras[x]->GetSpellProto()->Id == spell_id)
				ret++;
	if( SearchFlags & AURA_SEARCH_NEGATIVE )
		for(uint32 x = MAX_POSITIVE_AURAS; x < m_auras_neg_size; ++x)
			if(m_auras[x] && m_auras[x]->GetSpellProto()->Id == spell_id)
				ret++;
	if( SearchFlags & AURA_SEARCH_PASSIVE )
		for(uint32 x=MAX_AURAS;x<m_auras_pas_size;x++)
			if(m_auras[x] && m_auras[x]->GetSpellProto()->Id == spell_id )
				ret++;
	return ret;
}

uint32 Unit::CountAuraNameHash(uint32 namehash, uint32 SearchFlags)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	uint32 ret=0;
	if( SearchFlags & AURA_SEARCH_POSITIVE )
		for(uint32 x = FIRST_AURA_SLOT; x < m_auras_pos_size; ++x)
			if(m_auras[x] && m_auras[x]->GetSpellProto()->NameHash == namehash )
				ret++;
	if( SearchFlags & AURA_SEARCH_NEGATIVE )
		for(uint32 x = MAX_POSITIVE_AURAS; x < m_auras_neg_size; ++x)
			if(m_auras[x] && m_auras[x]->GetSpellProto()->NameHash == namehash )
				ret++;
	if( SearchFlags & AURA_SEARCH_PASSIVE )
		for(uint32 x=MAX_AURAS;x<m_auras_pas_size;x++)
			if(m_auras[x] && m_auras[x]->GetSpellProto()->NameHash == namehash )
				ret++;
	return ret;
}

uint32 Unit::GetDKDiseaseCount()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	uint32 ret=0;
	ret += (HasAura( 55095, 0, AURA_SEARCH_NEGATIVE )!=NULL); //frost feaver
	ret += (HasAura( 55078, 0, AURA_SEARCH_NEGATIVE )!=NULL); //blood plague
	ret += (HasAura( 65142, 0, AURA_SEARCH_NEGATIVE )!=NULL); //Ebon Plague

	return ret;
}

bool Unit::IsPoisoned()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	for(uint32 x=FIRST_AURA_SLOT;x<m_auras_pos_size;x++)
		if(m_auras[x] && m_auras[x]->GetSpellProto()->c_is_flags & SPELL_FLAG_IS_POISON)
		{ 
			return true;
		}
	for(uint32 x=MAX_POSITIVE_AURAS;x<m_auras_neg_size;x++)
		if(m_auras[x] && m_auras[x]->GetSpellProto()->c_is_flags & SPELL_FLAG_IS_POISON)
		{ 
			return true;
		}
	for(uint32 x=MAX_AURAS;x<m_auras_pas_size;x++)
		if(m_auras[x] && m_auras[x]->GetSpellProto()->c_is_flags & SPELL_FLAG_IS_POISON)
		{ 
			return true;
		}

	return false;
}

void Unit::TryRestorePrevDisplayModAura()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	uint32 LastApplyStamp = 0;
	uint32 LastID = GetNativeDisplayId();
	for(uint32 x=FIRST_AURA_SLOT;x<m_auras_pos_size;x++)
		if( m_auras[x] )
		{ 
			uint32 *LastIDStore = (uint32*)m_auras[x]->GetExtension( EXTENSION_ID_AURA_SHAPESHIFT_MODEL_BACKUP_VAL );
			if( LastIDStore != NULL )
			{
				uint32 *LastIDStoreStamp = (uint32*)m_auras[x]->GetExtension( EXTENSION_ID_AURA_SHAPESHIFT_MODEL_BACKUP_STAMP );
				if( *LastIDStoreStamp > LastApplyStamp )
				{
					LastApplyStamp = *LastIDStoreStamp;
					LastID = *LastIDStore;
				}
			}
		}
	for(uint32 x=MAX_POSITIVE_AURAS;x<m_auras_neg_size;x++)
		if( m_auras[x] )
		{ 
			uint32 *LastIDStore = (uint32*)m_auras[x]->GetExtension( EXTENSION_ID_AURA_SHAPESHIFT_MODEL_BACKUP_VAL );
			if( LastIDStore != NULL )
			{
				uint32 *LastIDStoreStamp = (uint32*)m_auras[x]->GetExtension( EXTENSION_ID_AURA_SHAPESHIFT_MODEL_BACKUP_STAMP );
				if( *LastIDStoreStamp > LastApplyStamp )
				{
					LastApplyStamp = *LastIDStoreStamp;
					LastID = *LastIDStore;
				}
			}
		}
//	for(uint32 x=MAX_AURAS;x<m_auras_pas_size;x++)
	SetDisplayId( LastID );
}

inline void AddVisualAuraToPacket( Unit *u, Aura *aur, StackWorldPacket &data, bool full_send = false )	
{	
	if( aur	
		&& aur->m_visualSlot < MAX_VISIBLE_AURAS		
		&& ( full_send == false || aur->m_modcount != 0 )	//these are charges for charge spells that have no effect other then visual placeholder	
		)
	{	
		//this is a dummy aura created by us. Make sure to not mess up client with it
		if( aur->GetSpellProto()->spell_id_client == 0 )
			return;

		if( full_send == false )
			data << u->GetNewGUID();
		data << (uint8)aur->m_visualSlot; //slot	
		data << aur->GetSpellProto()->spell_id_client; //spell		

		uint16 flags;	
		flags = 0; // do not send guid	
//		flags |= VISUALAURA_FLAG_VISIBLE_1 | VISUALAURA_FLAG_VISIBLE_2 | VISUALAURA_FLAG_VISIBLE_3; //seems to be always present	
		if( aur->m_casterGuid == u->GetGUID() )	
			flags |= VISUALAURA_FLAG_SELF_CAST;	

		if( !aur->IsPositive() )	
			flags |= VISUALAURA_FLAG_NEGATIVE; 
		else	
			flags |= VISUALAURA_FLAG_REMOVABLE;	
		if( aur->GetDuration() != (uint32)-1 ) //yes it is stupid to force -1 to be uint32 but that is the way it is made elsewhere 
			flags |= VISUALAURA_FLAG_HAS_DURATION;	

		//just in case we want to force some specific flag sending. Ex : aura 333 
		flags |= aur->GetSpellProto()->ForcedAuraVisibleFlags; 

		data << (uint16)flags;	
		data << (uint8)( u->getLevel() );	//might need to send aura caster serialization here and not ours !	
		data << uint8( u->m_auraStackCount[ aur->m_visualSlot ] );	

		if( (flags & VISUALAURA_FLAG_SELF_CAST)==0 )	
			FastGUIDPack(data, aur->m_casterGuid);	

		//seems like this depends on flag 0x20	
		if( (flags & VISUALAURA_FLAG_HAS_DURATION) )	
			data << uint32(aur->GetDuration()) << uint32(aur->GetTimeLeft());	

		//number of sent values here are based on VISUALAURA_FLAG_VISIBLE_1 | VISUALAURA_FLAG_VISIBLE_2 | VISUALAURA_FLAG_VISIBLE_3 flags
		if( (flags & VISUALAURA_FLAG_SEND_VALUES) )
		{
			//this part is needed or client craps out
			uint32 need_values = ((flags & VISUALAURA_FLAG_VISIBLE_1)!=0) + ((flags & VISUALAURA_FLAG_VISIBLE_2)!=0) + ((flags & VISUALAURA_FLAG_VISIBLE_3)!=0);
			uint32 have_values = MIN( aur->m_modcount, need_values );
			//really hope we have these in the same order as spell effects say they should be
			for(uint32 i=0;i<have_values;i++)
				data << int32( aur->m_modList[i].m_amount );
			for(uint32 i=have_values;i<need_values;i++)
				data << int32( 0 );
		}
	}	
}	

uint32 Unit::ModVisualAuraStackCount(Aura *aur, int32 count)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if( !aur )
	{ 
		sLog.outDebug("ModVisualAuraStackCount : Aura is NULL! Exiting");
		return 0;
	}

	uint16 slot = aur->m_visualSlot;

	if( slot >= MAX_VISIBLE_AURAS)
	{ 
		//this will happen to all non visible auras, no need to panic
		sLog.outDebug("ModVisualAuraStackCount : Aura visual slot is invalid! Exiting");
		return 0;
	}

	if(count < 0 && m_auraStackCount[slot] <= -count )
	{
		m_auraStackCount[ slot ] = 0;
		sStackWorldPacket( data, SMSG_AURA_UPDATE, 25);
		data << GetNewGUID();
		data << uint8(slot);
		data << uint32(0);
		SendMessageToSet(&data, true);
	}
	else
	{
		m_auraStackCount[ slot ] += count;
		//only send negative auras for creatures
//		if( (!aur->IsPositive() && IsCreature()) || IsPet() || IsPlayer() )
		if( aur->GetSpellProto()->spell_id_client != 0 )
		{
#ifdef _DEBUG
//			if( this->IsPlayer() ) SafePlayerCast( this )->BroadcastMessage("Adding Aura %s with duration %d", aur->GetSpellProto()->Name, aur->GetDuration() );
#endif
			sStackWorldPacket( data, SMSG_AURA_UPDATE, 50 );
			AddVisualAuraToPacket(this, aur ,data);
//			if( data.GetSize() > 3 )	//this happens when aura has no visual client side
				SendMessageToSet(&data, true);
		}
	}

	return m_auraStackCount[slot];
}

void Unit::SendAllAurasToPlayer(uint32 GUID)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if( !GetMapMgr() )
		return; //not in world anymore ?
	Player *plr = GetMapMgr()->GetPlayer( GUID );
	SendAurasToPlayer( plr );

	//need to move this to a new function. But it's 1 AM ..:(
	if( plr && IsPlayer() )
	{
		Player *pPlayer = SafePlayerCast( this );
		if( plr->GetVehicle() && plr->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_VEHICLE) )
		{
			sStackWorldPacket( data, SMSG_CONVERT_VEHICLE, 9+4);
			data << plr->GetNewGUID() ;
			data << uint32( plr->GetCurrentVehicleID() );
			pPlayer->GetSession()->SendPacket( &data );
		}
		if( pPlayer->GetVehicle() && pPlayer->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_VEHICLE) )
		{
			sStackWorldPacket( data, SMSG_CONVERT_VEHICLE, 9+4);
			data << pPlayer->GetNewGUID() ;
			data << uint32( pPlayer->GetCurrentVehicleID() );
			plr->GetSession()->SendPacket( &data );
		}
	}
}

void Unit::SendAurasToPlayer(Player *plr)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE

	if( !plr )
	{ 
		return;
	}

//	uint16 send_flags;
//	if( !IsPlayer() && !IsPet() )
//		send_flags = VISUALAURA_FLAG_NEGATIVE; //send only negative buffs for mobs
//	else 
//		send_flags = 0xFF;

	sStackWorldPacket(data,SMSG_AURA_UPDATE_ALL,(MAX_AURAS+MAX_PASSIVE_AURAS) * 25 + 8);
	data << GetNewGUID();
	for( uint32 i=POS_AURAS_START; i<MAX_POSITIVE_AURAS1(this); i++ )
		AddVisualAuraToPacket(this,m_auras[i],data, true);
	for( uint32 i=NEG_AURAS_START; i<MAX_NEGATIVE_AURAS1(this); i++ )
		AddVisualAuraToPacket(this,m_auras[i],data, true);
	for( uint32 i=PAS_AURAS_START; i<MAX_PASSIVE_AURAS1(this); i++ )
		AddVisualAuraToPacket(this,m_auras[i],data, true);

	if( data.GetSize() > 8 || plr == this )
		plr->GetSession()->SendPacket( &data );
}

void Unit::RemoveAurasOfSchool(uint32 School, bool Positive, bool Immune)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	uint32 SchoolMask = 1 << School;
	if( Positive )
		for(uint32 x=FIRST_AURA_SLOT;x<m_auras_pos_size;x++)
			if( m_auras[x] && ( m_auras[x]->GetSpellProto()->SchoolMask & SchoolMask ) && (Immune==false || (m_auras[x]->GetSpellProto()->Attributes & ATTRIBUTES_IGNORE_INVULNERABILITY)==0 ) )
				m_auras[x]->Remove();
	for(uint32 x=MAX_POSITIVE_AURAS;x<m_auras_neg_size;x++)
		if( m_auras[x] && ( m_auras[x]->GetSpellProto()->SchoolMask & SchoolMask ) && (Immune==false || (m_auras[x]->GetSpellProto()->Attributes & ATTRIBUTES_IGNORE_INVULNERABILITY)==0 ) )
			m_auras[x]->Remove();
}

void Unit::EnableFlight()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE

	if( IsPlayer() == false )
		return;

	uint64 guid = GetGUID();
	uint8 *guid_bytes = (uint8*)&guid;
	WorldPacket *data = new WorldPacket( SMSG_MOVE_SET_CAN_FLY, 30 );
	*data << uint8( 0xF5 );	//amgic guid mask
    *data << ObfuscateByte( guid_bytes[0] );   
	*data << uint32( SafePlayerCast( this )->m_PositionUpdateCounter++ );
    *data << ObfuscateByte( guid_bytes[7] );   
    *data << ObfuscateByte( guid_bytes[6] );   
    *data << ObfuscateByte( guid_bytes[3] );   
    *data << ObfuscateByte( guid_bytes[1] );   
    *data << ObfuscateByte( guid_bytes[2] );   

	SafePlayerCast( this )->m_setflycheat = true;
	if( SafePlayerCast(this)->m_changingMaps == false )
	{
		SendMessageToSet( data, true);
		delete data;
		data = NULL;
	}
	else
	{
		SafePlayerCast( this )->z_axisposition = 0.0f;
		SafePlayerCast( this )->delayedPackets.add( data );
	}
}

void Unit::DisableFlight()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE

	if( IsPlayer() == false )
		return;

	uint64 guid = GetGUID();
	uint8 *guid_bytes = (uint8*)&guid;
	WorldPacket *data = new WorldPacket( SMSG_MOVE_UNSET_CAN_FLY, 30 );
	*data << uint8( 0x7D );
	*data << ObfuscateByte( guid_bytes[2] );   
	*data << ObfuscateByte( guid_bytes[1] );   
	*data << ObfuscateByte( guid_bytes[0] );   
	*data << ObfuscateByte( guid_bytes[6] );   
	*data << ObfuscateByte( guid_bytes[7] );   
	*data << uint32( SafePlayerCast( this )->m_PositionUpdateCounter++ );
	*data << ObfuscateByte( guid_bytes[3] );   

	SafePlayerCast( this )->m_setflycheat = false;
	if( SafePlayerCast(this)->m_changingMaps == false )
	{
		SendMessageToSet(data, true);
		delete data;
		data = NULL;
	}
	else
	{
		SafePlayerCast( this )->z_axisposition = 0.0f;
		SafePlayerCast( this )->delayedPackets.add( data );
	}
}

void Unit::SendKnockbackPacket(float pcos,float psin,float radius,float verticalspeed)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE

	uint64 guid = GetGUID();
	uint8 *guid_bytes = (uint8*)&guid;
	sStackWorldPacket( data, SMSG_MOVE_KNOCK_BACK, 30 );
	data << uint8( 0xDB );
	data << pcos;
	data << psin;
	data << verticalspeed; //negativ to make player move backwards ? ex -7
	data << ObfuscateByte( guid_bytes[6] );   
	data << uint32( m_PositionUpdateCounter++ );
	data << radius;	// ex 15
	data << ObfuscateByte( guid_bytes[1] );   
	data << ObfuscateByte( guid_bytes[0] );   
	data << ObfuscateByte( guid_bytes[2] );   
	data << ObfuscateByte( guid_bytes[3] );   
	data << ObfuscateByte( guid_bytes[7] );   

	SendMessageToSet( &data, true);
}

bool Unit::IsDazed()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
/*	for(uint32 x = FIRST_AURA_SLOT; x < m_auras_pos_size; ++x)
		if(m_auras[x])
		{
			if(m_auras[x]->GetSpellProto()->MechanicsType == MECHANIC_ENSNARED)
				return true;
			for(uint32 y=0;y<3;y++)
				if(m_auras[x]->GetSpellProto()->EffectMechanic[y]==MECHANIC_ENSNARED)
					return true;
		}*/
	for(uint32 x = MAX_POSITIVE_AURAS; x < m_auras_neg_size; ++x)
		if(m_auras[x])
		{
			if(m_auras[x]->GetSpellProto()->MechanicsType == MECHANIC_ENSNARED)
			{ 
				return true;
			}
			for(uint32 y=0;y<3;y++)
				if(m_auras[x]->GetSpellProto()->eff[y].EffectMechanic==MECHANIC_ENSNARED)
				{ 
					return true;
				}
		}

	return false;

}

void Unit::UpdateVisibility()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	ByteBuffer buf(2500);
//	InRangeSetRecProt::iterator itr;
	uint32 count;
	bool can_see;
	bool is_visible;
	Player *pl;
	Object * pObj;
	Player * plr;

	if( IsPlayer() )
	{
		plr = SafePlayerCast( this );
		InrangeLoopExitAutoCallback AutoLock;
		for(InRangeSetRecProt::iterator itr = GetInRangeSetBegin( AutoLock ); itr != GetInRangeSetEnd(); )
		{
			pObj = (*itr);
			++itr;

			can_see = plr->CanSee(pObj);
			is_visible = plr->GetVisibility( pObj );
			if(can_see)
			{
				if(!is_visible)
				{
					buf.clear();
					count = pObj->BuildCreateUpdateBlockForPlayer( &buf, plr );
					plr->PushCreationData( &buf, count, pObj );
					plr->AddVisibleObject(pObj);
				}
			}
			else
			{
				if(is_visible)
				{
//					pObj->DestroyForPlayer(plr);
					plr->PushOutOfRange( pObj->GetGUID() );
					plr->RemoveVisibleObject( pObj );
				}
			}

			if( pObj->IsPlayer() )
			{
				pl = SafePlayerCast( pObj );
				can_see = pl->CanSee( plr );
				is_visible = pl->GetVisibility( plr );
				if( can_see )
				{
					if(!is_visible)
					{
						buf.clear();
						count = plr->BuildCreateUpdateBlockForPlayer( &buf, pl );
						pl->PushCreationData( &buf, count, plr );
						pl->AddVisibleObject(plr);
					}
				}
				else
				{
					if(is_visible)
					{
//						plr->DestroyForPlayer(pl);
						pl->PushOutOfRange( plr->GetGUID() );
						pl->RemoveVisibleObject( plr );
					}
				}
			}
		}
	}
	else			// For units we can save a lot of work
	{
		InrangeLoopExitAutoCallback AutoLock;
		for(InRangePlayerSetRecProt::iterator it2 = GetInRangePlayerSetBegin( AutoLock ); it2 != GetInRangePlayerSetEnd(); ++it2)
		{
			can_see = (*it2)->CanSee(this);
			is_visible = (*it2)->GetVisibility(this);
			if(!can_see)
			{
				if(is_visible)
				{
//					DestroyForPlayer(*it2);
					(*it2)->PushOutOfRange( GetGUID() );
					(*it2)->RemoveVisibleObject( this );
				}
			}
			else
			{
				if(!is_visible)
				{
					buf.clear();
					count = BuildCreateUpdateBlockForPlayer(&buf, *it2);
					(*it2)->PushCreationData( &buf, count, this );
					(*it2)->AddVisibleObject(this);
				}
			}
		}
	}
}

void Unit::EventHealthChangeSinceLastUpdate()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	int pct = GetHealthPct();

	if( IsPlayer() )
	{
		if( getClass() == MAGE )
		{
			Player *plr = SafePlayerCast( this );
			uint32 proc_spell = 0;
			//hackfix for Mage talent !!! Fiery Payback
			if( pct < 35 )
			{
				//add once
				if( !HasAuraStateFlag(AURASTATE_HEALTH_BELOW_35_PCT ) )
				{
					if( plr->HasSpell( 44440 ) )
						proc_spell = 44440;
					else if( plr->HasSpell( 44441 ) )
						proc_spell = 44441;
				}
				if( proc_spell )
				{
					SpellCastTargets targets( GetGUID() );
					SpellEntry *spellInfo = dbcSpell.LookupEntry( proc_spell );
					Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
					spell->Init(this, spellInfo ,true, NULL);
					spell->prepare(&targets);
				}
			}
			//remove once if it was added
			else if( HasAuraStateFlag(AURASTATE_HEALTH_BELOW_35_PCT ) )
			{
				RemoveAura( 44440, GetGUID() );
				RemoveAura( 44441, GetGUID() );
			}
		}
/*		else if( getClass() == DEATHKNIGHT )
		{
			Player *plr = SafePlayerCast( this );
			uint32 proc_spell = 0;
			if( proc_spell )
			{
				SpellCastTargets targets( GetGUID() );
				SpellEntry *spellInfo = dbcSpell.LookupEntry( proc_spell );
				Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
				spell->Init(this, spellInfo ,true, NULL);
				spell->prepare(&targets);
			}
		} */
		else if( getClass() == WARRIOR )
		{
			if( pct > 20 && HasAuraStateFlag( AURASTATE_HEALTH_BELOW_20_PCT ) )
			{
				//Die By The Sword = SPELL_HASH_DIE_BY_THE_SWORD
				RemoveAura( 85386, 0, AURA_SEARCH_POSITIVE );
				RemoveAura( 86624, 0, AURA_SEARCH_POSITIVE );
			}
		}
	}

	if(pct<75)
	{
		if( pct<35 )
		{
			SetAuraStateFlag( AURASTATE_HEALTH_BELOW_35_PCT, true );
			if( pct < 20 )
				SetAuraStateFlag(AURASTATE_HEALTH_BELOW_20_PCT, true );
			else if( pct > 20 )
				RemoveAuraStateFlag(AURASTATE_HEALTH_BELOW_20_PCT, true);
		}
		else if( pct>=35 )
		{
			RemoveAuraStateFlag( AURASTATE_HEALTH_BELOW_35_PCT, true );
			RemoveAuraStateFlag( AURASTATE_HEALTH_BELOW_20_PCT, true);
		}
		RemoveAuraStateFlag( AURASTATE_HEALTH_ABOVE_75_PCT, true );
	}
	else if( pct > 75 )
	{
		SetAuraStateFlag( AURASTATE_HEALTH_ABOVE_75_PCT, true);
		RemoveAuraStateFlag( AURASTATE_HEALTH_BELOW_35_PCT, true );
		RemoveAuraStateFlag(AURASTATE_HEALTH_BELOW_20_PCT, true);
	}
}

int32 Unit::GetAP()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
    int32 baseap = GetInt32Value(UNIT_FIELD_ATTACK_POWER) + GetInt32Value(UNIT_FIELD_ATTACK_POWER_MOD_POS) - GetInt32Value(UNIT_FIELD_ATTACK_POWER_MOD_NEG);
	if( baseap < 0 )
		baseap = 0;
    float totalap = float(baseap)*(GetFloatValue(UNIT_FIELD_ATTACK_POWER_MULTIPLIER)+1);
	if(totalap>=0)
	{ 
		return float2int32(totalap);
	}
	return	0;
}

int32 Unit::GetRAP()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
    int32 baseap = GetInt32Value(UNIT_FIELD_RANGED_ATTACK_POWER) + GetInt32Value(UNIT_FIELD_RANGED_ATTACK_POWER_MOD_POS) - GetInt32Value(UNIT_FIELD_RANGED_ATTACK_POWER_MOD_NEG);
	if( baseap < 0 )
		baseap = 0;
    float totalap = float(baseap)*(GetFloatValue(UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER)+1);
	if(totalap>=0)
	{ 
		return float2int32(totalap);
	}
	return	0;
}

bool Unit::GetSpeedDecrease()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	int32 before=m_speedModifier;
	m_speedModifier -= m_slowdown;
	m_slowdown = 0;
	SimplePointerListNode< Modifier > *itr;
	for(itr = speedReductionMap.begin(); itr != speedReductionMap.end(); itr = itr->Next() )
		m_slowdown = MIN( m_slowdown, itr->data->m_amount );

	if(m_slowdown<-100)
		m_slowdown = -100; //do not walk backwards !

	m_speedModifier += m_slowdown;
	//save bandwidth :P
	if(m_speedModifier!=before)
	{ 
		return true;
	}
	return false;
}

bool Unit::GetSpeedIncrease()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	int32 before=m_speedModifier;
	m_speedModifier -= m_speedup;
	m_speedup = 0;

	SimplePointerListNode< Modifier > *itr;
	for(itr = speedIncreaseMap.begin(); itr != speedIncreaseMap.end(); itr = itr->Next() )
		if( itr->data->m_AuraName != SPELL_AURA_MOD_INCREASE_SPEED_ALWAYS )
			m_speedup = MAX( m_speedup, itr->data->m_amount );
	for(itr = speedIncreaseMap.begin(); itr != speedIncreaseMap.end(); itr = itr->Next() )
		if( itr->data->m_AuraName == SPELL_AURA_MOD_INCREASE_SPEED_ALWAYS )
			m_speedup += itr->data->m_amount;

	m_speedModifier += m_speedup;
	//save bandwidth :P
	if(m_speedModifier!=before)
	{ 
		return true;
	}
	return false;
}

void Unit::UpdateSizeMod()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if( ObjectSizeMods.HasValues() == false )
	{
		SetFloatValue( OBJECT_FIELD_SCALE_X, 1.0f );
		return;
	}
	int32 MinScaleDowns = 0,MaxScaleUps = 0, SumScaleUps = 0;

	SimplePointerListNode< Modifier > *itr;
	for(itr = ObjectSizeMods.begin(); itr != ObjectSizeMods.end(); itr = itr->Next() )
	{
		MinScaleDowns = MIN( MinScaleDowns, itr->data->m_amount );
		MaxScaleUps = MAX( MaxScaleUps, itr->data->m_amount );
		SumScaleUps += itr->data->m_amount;
	}

	int32 avg = 0, avg_c = 0;
	if( SumScaleUps > 50 && SumScaleUps <= 200 )
		SetFloatValue( OBJECT_FIELD_SCALE_X, 1.0f + (float)SumScaleUps / 100.0f );
	else
	{
		if( MinScaleDowns < 0 )
		{
			avg = avg + 100 + MinScaleDowns;
			avg_c++;
		}
		if( MaxScaleUps > 0 )
		{
			avg = avg + 100 + MaxScaleUps;
			avg_c++;
		}
		if( avg_c > 0.0f )
			SetFloatValue( OBJECT_FIELD_SCALE_X, (float)avg / (float)avg_c / 100.0f );
	}
}

void Unit::GetResistanceExclusive( int32 SchoolMask, int32 &pos, int32 &neg )
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE

	neg = 0;
	pos = 0;
	SimplePointerListNode< Modifier > *itr;
	for(itr = ResistanceExclusive.begin(); itr != ResistanceExclusive.end(); itr = itr->Next() )
		if( itr->data->m_miscValue & SchoolMask )
		{
			if( itr->data->m_amount < 0 )
				neg = MIN( neg, itr->data->m_amount );
			else
				pos = MAX( pos, itr->data->m_amount );
	}
}

void Unit::EventCastSpell3(SpellCastTargets targets, SpellEntry * Sp)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	Spell * pSpell = SpellPool.PooledNew( __FILE__, __LINE__ );
	pSpell->Init( this, Sp, true, NULL);
	pSpell->prepare(&targets);
}

void Unit::SetFacing(float newo)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	SetOrientation(newo);
	/*WorldPacket data(40);
	data.SetOpcode(MSG_MOVE_SET_FACING);
	data << GetNewGUID();
	data << (uint32)0; //flags
	data << (uint32)0; //time
	data << GetPositionX() << GetPositionY() << GetPositionZ() << newo;
	data << (uint32)0; //unk
	SendMessageToSet( &data, false );*/

	/*WorldPacket data(SMSG_MONSTER_MOVE, 60);
	data << GetNewGUID();
	data << m_position << getMSTime();
	data << uint8(4) << newo;
	data << uint32(0x00000000);		// flags
	data << uint32(0);				// time
	data << m_position;				// position
	SendMessageToSet(&data, true);*/
	
	m_aiInterface->SendMoveToPacket(m_position.x,m_position.y,m_position.z,m_position.o,1,MONSTER_MOVE_FLAG_RUN); // MoveFlags = 0x100 (run)
}

//summons are as guardians but there can only be one of them at a time
Unit* Unit::create_summon(uint32 summon_entry,uint32 duration, uint32 lvl, GameObject * obj, LocationVector * Vec)
{
	RemoveFieldSummon();
	Unit *tguard = create_guardian( summon_entry, duration, lvl, obj, Vec );
	//this will swap castbar to this creature + show creature under player icon!! 
//	if( tguard )
//		SetUInt64Value( UNIT_FIELD_SUMMON, tguard->GetGUID() );
	return tguard;
}

//guardians are temporary spawn that will inherit master faction and will folow them. Apart from that they have their own mind
Unit* Unit::create_guardian(uint32 guardian_entry,uint32 duration, uint32 lvl, GameObject * obj, LocationVector * Vec, SpellEntry *CreatedBy, bool UseHPScale)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	CreatureProto * proto = CreatureProtoStorage.LookupEntry(guardian_entry);
	CreatureInfo * info = CreatureNameStorage.LookupEntry(guardian_entry);
	
	if(!proto || !info)
	{
		sLog.outDetail("Warning : Missing summon creature template %u !",guardian_entry);
		return NULL;
	}
	//well this is a really rare case
	if( GetMapMgr() == NULL )
	{
		return NULL;	
	}
	
	Creature* p = GetMapMgr()->CreateCreature(guardian_entry);
	p->SetInstanceID(GetMapMgr()->GetInstanceID());
	p->m_phase = GetPhase();

	//update guardian list and see if we could add more
	std::list<uint64>::iterator gitr,gitr2;
	for( gitr2 = m_guardians.begin(); gitr2 != m_guardians.end();)
	{
		gitr = gitr2;
		gitr2++;
		Creature *tc = GetMapMgr()->GetCreature( (*gitr) );
		if( tc == NULL )
			m_guardians.erase( gitr );
	}
	//avoid creating infinit guardians !
	if( m_guardians.size() > 30 )
	{
		p->SafeDelete();
		return NULL;
	}
	m_guardians.push_back( p->GetGUID() );

	float m_followAngle = GetSummonAngle( p->GetGUID() );
	float x = 3 * ( cosf( m_followAngle + GetOrientation() ) );
	float y = 3 * ( sinf( m_followAngle + GetOrientation() ) );
	float z = 0;
	
	//need to be precise this to be able to spawn units that do spell effects at specific locations
	if( Vec )
	{
		x = Vec->x;
		y = Vec->y;
		z = Vec->z;
	}
	//Summoned by a GameObject?
	else if ( !obj ) 
	{
		x += GetPositionX();
		y += GetPositionY();
		z += GetPositionZ();
	}
	else //if so, we should appear on it's location ;)
	{
		x += obj->GetPositionX();
		y += obj->GetPositionY();
		z += obj->GetPositionZ();
	}
	p->Load(proto, x, y, z);

	uint32 spawn_level;
	if ( lvl != 0 )
		spawn_level = lvl;
	else
		spawn_level = getLevel();

	/* MANA */
	p->SetPowerType( POWER_TYPE_MANA );
	p->SetMaxPower(POWER_TYPE_MANA,p->GetMaxPower( POWER_TYPE_MANA )+28+10*spawn_level);
	p->SetPower( POWER_TYPE_MANA,p->GetPower( POWER_TYPE_MANA )+28+10*spawn_level);
	/* HEALTH */
	if( UseHPScale && info->base_HP_mod > 0 )
	{
//		float Lvl1HP = proto->MinHealth / MAX( 1, proto->MinLevel );
//		float LvlNowHP = Lvl1HP * spawn_level;
		float NewHP = ( p->GetUInt32Value(UNIT_FIELD_MAXHEALTH)+( 28+30*spawn_level ) ) * info->base_HP_mod;
		if( NewHP < MAX_INT )
			p->SetUInt32Value( UNIT_FIELD_MAXHEALTH, float2int32 ( NewHP ) );
	}
	else
		p->SetUInt32Value(UNIT_FIELD_MAXHEALTH, p->GetUInt32Value(UNIT_FIELD_MAXHEALTH ) );
	p->SetUInt32Value(UNIT_FIELD_HEALTH, p->GetUInt32Value(UNIT_FIELD_MAXHEALTH) );
	/* LEVEL */
	p->SetUInt32Value(UNIT_FIELD_LEVEL, spawn_level);

	/* adjust damage - most stuff need scripting as they scale based on AP or SP*/
	uint32 dominant_power_value;
	if( IsPlayer() )
	{
		if( getClass() == WARRIOR || getClass() == ROGUE || getClass() == DEATHKNIGHT )
			dominant_power_value = GetAP();
		else if( getClass() == WARLOCK )
			dominant_power_value = GetDamageDoneMod(SCHOOL_SHADOW);
		else if( getClass() == MAGE )
			dominant_power_value = MAX(MAX(GetDamageDoneMod(SCHOOL_FIRE), GetDamageDoneMod(SCHOOL_FROST)), GetDamageDoneMod(SCHOOL_ARCANE) );
		else if( getClass() == PRIEST )
			dominant_power_value = MAX(GetDamageDoneMod(SCHOOL_HOLY), GetDamageDoneMod(SCHOOL_SHADOW) );
		else if( getClass() == HUNTER )
			dominant_power_value = GetRAP();
		else if( getClass() == PALADIN )
			dominant_power_value = MAX( GetDamageDoneMod(SCHOOL_HOLY), GetAP() );
		else if( getClass() == SHAMAN || getClass() == DRUID )
			dominant_power_value = MAX( GetDamageDoneMod(SCHOOL_NATURE), GetAP() );
		else
			dominant_power_value = 400 / spawn_level; //240 dmg using 12% formula at lvl 80
	}
	else 
		dominant_power_value = float2int32( GetFloatValue( UNIT_FIELD_MINDAMAGE ) );

	//not dead sure this is used all the time
	if( SM_Mods && CreatedBy != NULL && Need_SM_Apply(CreatedBy) )
	{
		int32 flat_bonus = 0;
		int32 pct_bonus = 100;
		SM_FIValue(SM_Mods->SM_FDamageBonus,&flat_bonus,CreatedBy->GetSpellGroupType());
		SM_FIValue(SM_Mods->SM_PDamageBonus,&pct_bonus,CreatedBy->GetSpellGroupType());
		dominant_power_value = flat_bonus + dominant_power_value * pct_bonus / 100;
	}

	float dps_boost_inherit = MAX( getLevel(), dominant_power_value * 75 / 100.0f / 14.f);	//default dmg in case we have none is aprox 6% of dominant owner power
	float dmg_now = dps_boost_inherit * 2000 / 1000;	//2000 comes from default attack time. Using fixed values to avoid terminators
	float max_dmg = dmg_now + dmg_now / 2;
	//inheritance also boosts spell power and attack power
	p->SetInt32Value(UNIT_FIELD_ATTACK_POWER, dominant_power_value * 75 / 100 );
	for(uint32 i=0;i<SCHOOL_COUNT;i++)
		p->ModDamageDone[i] = dominant_power_value * 15 / 100;

	p->SetFloatValue( UNIT_FIELD_MINDAMAGE, dmg_now );
	p->SetFloatValue( UNIT_FIELD_MAXDAMAGE, max_dmg );

//	p->SetUInt32Value( UNIT_FIELD_BYTES_0, 133120 );
//	p->SetUInt32Value( UNIT_FIELD_FLAGS, 8 );
//	p->SetUInt32Value( UNIT_FIELD_FLAGS_2, 2048 );
//	p->SetUInt32Value( UNIT_CREATED_BY_SPELL, 58834 );
//	p->SetUInt32Value( UNIT_FIELD_BYTES_2, 257 );
	p->SetUInt64Value(UNIT_FIELD_SUMMONEDBY, GetGUID());
    p->SetUInt64Value(UNIT_FIELD_CREATEDBY, GetGUID());
	p->SetUInt32Value( UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED );
//    p->SetZoneId(GetZoneId());
	p->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE,GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE));
	p->_setFaction();

	p->GetAIInterface()->Init(p,AITYPE_PET,MOVEMENTTYPE_NONE,this);
	p->GetAIInterface()->SetUnitToFollow(this);
	p->GetAIInterface()->SetUnitToFollowAngle( m_followAngle );
//	p->GetAIInterface()->SetFollowDistance(3.0f);
//	p->m_noRespawn = true;

	if( this->IsPvPFlagged() )
		p->SetPvPFlag();
	if( this->IsFFAPvPFlagged() )
		p->SetFFAPvPFlag();

	p->PushToWorld(GetMapMgr());

	if( duration )
		sEventMgr.AddEvent(p, &Creature::SummonExpire, EVENT_SUMMON_EXPIRE, duration, 1,0);

	//if owner is in combat then agro target
//	if( CombatStatus.IsInCombat() )
	{
		Unit *topA = GetTopOwner();
		Unit *Possible_target;
//		Possible_target = topA->GetMapMgr()->GetUnit( topA->CombatStatus.GetPrimaryAttackTarget() );	//cause we can target other for heal
		// maybe no agro yet ? Try to target selection
		Possible_target = topA->GetMapMgr()->GetUnit( topA->GetUInt64Value( UNIT_FIELD_TARGET ) );
		//still no agro ? maybe we are in combat and we really hate someone
		if( Possible_target == NULL || isAttackable( this, Possible_target ) == false )
			Possible_target = topA->GetAIInterface()->GetMostHated();
	
		if( Possible_target && isAttackable( this, Possible_target ) && Possible_target->IsCrowdControlledNoAttack() == false && p->GetDistance2dSq( Possible_target ) <= 45.0f * 45.0f )
			p->GetAIInterface()->AttackReaction( Possible_target, 1, 0 );
	}
//	if( IsPlayer() && CreatedBy )
//		SendSummonLog( SafePlayerCast( this ), CreatedBy->Id, p, 1 );

	return p;//lol, will compiler make the pointer conversion ?

}

void Unit::DismissAllGuardians() // on death or remove world
{
	std::list<uint64>::iterator gitr;
	for( gitr = m_guardians.begin(); gitr != m_guardians.end(); gitr++ )
	{
		Creature *tc = GetMapMgr()->GetCreature( (*gitr) );
		if( tc )
			tc->Despawn( 0, 0 );
	}
	m_guardians.clear();
}

float Unit::get_chance_to_daze(Unit *target)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
//	if(GetTypeId()!=TYPEID_UNIT)
//		return 0.0f;
	float attack_skill = float( getLevel() ) * 5.0f;
	float defense_skill;
	if( target->IsPlayer() )
		defense_skill = float( SafePlayerCast( target )->_GetSkillLineCurrent( SKILL_DEFENSE, false ) );
	else defense_skill = float( target->getLevel() * 5 );
	if( !defense_skill )
		defense_skill = 1;
	float chance_to_daze = attack_skill * 20 / defense_skill;//if level is equal then we get a 20% chance to daze
	chance_to_daze = chance_to_daze * std::min(target->getLevel() / 30.0f, 1.0f );//for targets below level 30 the chance decreses
	if( chance_to_daze > 40 )
	{ 
		return 40.0f;
	}
	else
		return chance_to_daze;
}

//note that there is a race condition here. Depending which one of us will leave combat first we might not exit combat
//to solve this we should double check> try to leave combat for NPC1, update NPC2 to leave combat then check NPC1 again
void CombatStatusHandler::UpdateFlag()
{
	bool n_status = InternalIsInCombat();
	if(n_status != m_lastStatus)
	{
		m_lastStatus = n_status;
		if(n_status)
		{
			//printf(I64FMT" is now in combat.\n", m_Unit->GetGUID());
			m_Unit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_COMBAT);
			m_Unit->SetByte(UNIT_FIELD_BYTES_2, 0, WEAPON_SHEATH_MELEE);
			m_Unit->addStateFlag(UF_ATTACKING);
		}
		else
		{
			//printf(I64FMT" is no longer in combat.\n", m_Unit->GetGUID());
			m_Unit->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_COMBAT);
			m_Unit->SetByte(UNIT_FIELD_BYTES_2, 0, WEAPON_SHEATH_NONE);
			m_Unit->clearStateFlag(UF_ATTACKING);
			if( m_Unit->IsPlayer() )
				SafePlayerCast(m_Unit)->StartOutOfCombatCooldowns();
		}
	}
}

bool CombatStatusHandler::InternalIsInCombat()
{
	if( m_CombatTargets.empty() == false )
	{
		return true;
	}

	if(m_Unit->IsPlayer() && m_Unit->GetMapMgr() && m_Unit->GetMapMgr()->IsCombatInProgress())
	{ 
		return true;
	}

	return false;
}

bool CombatStatusHandler::IsAttacking(Unit * pTarget)
{
	//this check is only available in PVP and rogues may exit combat even in PVE ?
	if( pTarget->IsPlayer() )
		return false;
	// check the target for any of our DoT's.
	for(uint32 i = MAX_POSITIVE_AURAS; i < MAX_NEGATIVE_AURAS1(pTarget); ++i)
		if(pTarget->m_auras[i] != NULL)
			if( m_Unit->GetGUID() == pTarget->m_auras[i]->m_casterGuid
//				&& pTarget->m_auras[i]->IsCombatStateAffecting()
				&& ( pTarget->m_auras[i]->GetSpellProto()->c_is_flags & SPELL_FLAG_IS_DAMAGING )
				)
			{ 
				return true;
			}

	// place any additional checks here
	return false;
} 

//we are not attacking him and he should not attack us
void CombatStatusHandler::BreakCombatLink(Unit * pTarget, uint8 ChainCall)
{
	AttackerMap::iterator itr;
	itr = m_CombatTargets.find( pTarget->GetGUID() );
	if(itr != m_CombatTargets.end())
		m_CombatTargets.erase(itr); 
	UpdateFlag();
	//break combat link for target also
	if( ChainCall == 0 )
		pTarget->CombatStatus.BreakCombatLink( m_Unit, 1 );
}

//every time we damage someone we extend the combat timeout and add the bastard to our combat relation table(we fight him he fights us)
void CombatStatusHandler::OnDamageDealt(Unit * pTarget, bool IsDOT )
{
	// we added an aura, or dealt some damage to a target. they need to have us as an attacker, and they need to be our attack target if not.
	//printf("OnDamageDealt to "I64FMT" from "I64FMT"\n", pTarget->GetGUID(), m_Unit->GetGUID());
	if( pTarget == m_Unit )
	{ 
		return;
	}

	//no need to be in combat if dead
	if( !pTarget->isAlive() || !m_Unit->isAlive() )
	{ 
		return;
	}
		
	m_CombatTargets.insert( pTarget->GetGUID() ); 
	UpdateFlag();
	m_Unit->CombatStatusHandler_ResetPvPTimeout();

	pTarget->CombatStatus.m_CombatTargets.insert( m_Unit->GetGUID() );
	pTarget->CombatStatus.UpdateFlag();
	pTarget->CombatStatusHandler_ResetPvPTimeout();
}

void CombatStatusHandler::ClearAttackers()
{
	// this is a FORCED function, only use when the reference will be destroyed.
	if( m_Unit->GetMapMgr() )
	{
		AttackerMap::iterator itr;
		for(itr = m_CombatTargets.begin(); itr != m_CombatTargets.end(); ++itr)
		{
			Unit * pt = m_Unit->GetMapMgr()->GetUnit(*itr);
			if(pt)
			{
				pt->CombatStatus.m_CombatTargets.erase(m_Unit->GetGUID());
				pt->CombatStatus.UpdateFlag();
			}
		}
	}
	m_CombatTargets.clear();
	UpdateFlag();
}

bool CombatStatusHandler::IsInCombat()
{
	if( m_Unit == NULL || !m_Unit->IsInWorld() )
	{ 
		return false;
	}

	//if we are in combat then no need to make other checks
	if( m_lastStatus == true )
	{
		return true;
	}

	//until we order pet to stop attacking we are both in combat
	if( m_Unit->IsPlayer() 
		&& SafePlayerCast( m_Unit )->GetSummon() 
		&& SafePlayerCast( m_Unit )->GetSummon()->CombatStatus.m_lastStatus == true )
	{
			return true;
	}

	//until owner fights, pet fights also
	else if( m_Unit->IsPet() 
		&& SafePetCast(m_Unit)->GetPetOwner() 
		&& SafePetCast(m_Unit)->GetPetOwner()->CombatStatus.m_lastStatus == true )
	{
		return true;
	}

	//maybe we should check all our defenders and guardians also ?
	return false;
}

void Unit::CombatStatusHandler_ResetPvPTimeout()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if(!IsPlayer())
	{ 
		return;
	}

	if( sEventMgr.ModifyEventTimeLeft( this, EVENT_ATTACK_TIMEOUT, 6000 ) == false )
		sEventMgr.AddEvent(this, &Unit::CombatStatusHandler_UpdatePvPTimeout, EVENT_ATTACK_TIMEOUT, 6000, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
}

void Unit::CombatStatusHandler_UpdatePvPTimeout()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	CombatStatus.ClearAttackers();
	//this is required in case situation happens : we update, NPC dies, NPC does not do any attack to register a PVP timout event, we get stuck in combat
	if( CombatStatus.IsInCombat() )
		CombatStatusHandler_ResetPvPTimeout();
}

void Unit::Heal(Unit *target, uint32 SpellId, uint32 amount)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
//Static heal
	if(!target || !SpellId || !amount)
	{ 
		return;
	}

	sStackWorldPacket( data, SMSG_SPELLHEALLOG, 50 );
	data << target->GetNewGUID();
	data << this->GetNewGUID();
	data << uint32(SpellId);  
	data << uint32(amount);   
	if( target->IsUnit() )
		data << uint32( SafeUnitCast(target)->GetOverHeal( amount ) );	// overheal
	else
		data << uint32( amount );
	data << uint32( 0 );
	data << uint16(0);	 //this is critical message
#ifdef MINIMIZE_NETWORK_TRAFIC
	SendMessageToDuel( &data );
#else
	this->SendMessageToSet(&data,true);
#endif

	uint32 ch=target->GetUInt32Value(UNIT_FIELD_HEALTH);
	uint32 mh=target->GetUInt32Value(UNIT_FIELD_MAXHEALTH);
	if(mh!=ch)
	{
		ch += amount;
		if(ch > mh)
		{
			target->SetUInt32Value(UNIT_FIELD_HEALTH, mh);
			amount += mh-ch;
		}
		else 
			target->SetUInt32Value(UNIT_FIELD_HEALTH, ch);

		target->RemoveAurasByHeal();
	}
}

void Unit::Energize( Unit* target, uint32 SpellId, int32 amount, uint32 type, uint8 is_periodic )
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	//Static energize
	if( !target || !SpellId || !amount )
	{ 
		return;
	}
	
	int32 amount2;
	if( getClass() == DEATHKNIGHT  && IsPlayer() && type <= TOTAL_NORMAL_RUNE_TYPES )
	{
		Player *p = SafePlayerCast( this );
		for(int j=0;j<TOTAL_USED_RUNES;j++)
			if( p->m_rune_types[j] == type && p->m_runes[j] < MAX_RUNE_VALUE )
			{
				p->m_runes[j] = MAX_RUNE_VALUE;
				p->UpdateRuneIcon( j );
			}
		return; // there is not need to send packet now
	}
	else if( type <= POWER_TYPE_COUNT )
	{
		int32 cur = target->GetPower(  type );
		int32 max = target->GetMaxPower( type );
		
		/*if( max == cur ) // can we show null power gains in client? eg. zero happiness gain should be show...
			return;*/

		if( cur + amount > max )
			amount2 = max - cur;
		else
			amount2 = amount;

		target->SetPower(  type, cur + amount2 );
	}

/*	sStackWorldPacket( datamr, SMSG_SPELLENERGIZELOG, 50 );
	datamr << target->GetNewGUID();
	datamr << this->GetNewGUID();
	datamr << SpellId;
	datamr << type;
	datamr << amount; 
#ifdef MINIMIZE_NETWORK_TRAFIC
	SendMessageToDuel( &datamr );
#else
	SendMessageToSet( &datamr, true );
#endif
	*/
	if( SpellId )
	{
		if( is_periodic )
			SendCombatLogMultiple( target, amount, type, 0, 0, SpellId, 0, COMBAT_LOG_PERIODIC_ENERGIZE, 0, COMBAT_LOG_SUBFLAG_PERIODIC_ENERGIZE );
		else
			SendCombatLogMultiple( target, amount, type, 0, 0, SpellId, 0, COMBAT_LOG_ENERGIZE, 0, is_periodic );
	}
	//same shit but blizz used this. No idea why
/*	WorldPacket datamr( SMSG_PERIODICAURALOG, 30 );
	datamr << target->GetNewGUID();
	datamr << this->GetNewGUID();
	datamr << SpellId;
	datamr << uint32(1); //just 1
	datamr << uint32( FLAG_PERIODIC_HEAL + FLAG_PERIODIC_LEECH ); //well i know this is wrong but client used this
	datamr << uint32( type ); 
	datamr << amount; //if amount is 0 then it is not shown
	SendMessageToSet( &datamr, true ); /**/
}

void Unit::InheritSMMods(Unit *inherit_from)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE

	if( inherit_from->SM_Mods == NULL )
		return;

	if( SM_Mods == NULL )
		SM_Mods = new UnitSMMods;

	SM_Mods->InheritFrom( inherit_from->SM_Mods );
}

#if 0
void Unit::CancelSpell(Spell * ptr)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
/*
	if(ptr)
		ptr->cancel();
	else */
	if(m_currentSpell)
	{
		// this logically might seem a little bit twisted
		// crash situation : an already deleted spell will be called to get canceled by eventmanager
		// solution : We should not delay spell canceling more then second spell canceling.
		// problem : might remove spells that should not be removed. Not sure about it :(
		// sEventMgr.RemoveEvents(this,EVENT_UNIT_DELAYED_SPELL_CANCEL);
		m_currentSpell->safe_cancel();
	}
}
#endif

void Unit::EventStrikeWithAbility(uint64 guid, SpellEntry * sp, uint32 damage)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	Unit * victim = m_mapMgr ? m_mapMgr->GetUnit(guid) : NULL;
	if(victim)
		Strike( victim, RANGED, sp, 0, 0, 0, false, true );
}

void Unit::DispelAll(bool positive)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if( positive )
	{
		for(uint32 i = FIRST_AURA_SLOT; i < m_auras_pos_size; ++i)
			if( m_auras[i]!=NULL)
				m_auras[i]->Remove();
	}
	else 
		for(uint32 i = MAX_POSITIVE_AURAS; i < m_auras_neg_size; ++i)
			if( m_auras[i]!=NULL )
				m_auras[i]->Remove();
}

bool Unit::RemoveAllAurasByMechanic( uint32  MechanicType, uint32 MaxDispel , bool HostileOnly ) // Removes all (de)buffs on unit of a specific mechanic type.
{
	return RemoveAllAurasByMechanicMulti( ( 1 << MechanicType ), MaxDispel, HostileOnly );
}

/* bool Unit::RemoveAllAurasByMechanic (renamed from MechanicImmunityMassDispel)
- Removes all auras on this unit that are of a specific mechanic.
- Useful for things like.. Apply Aura: Immune Mechanic, where existing (de)buffs are *always supposed* to be removed.
- I'm not sure if this goes here under unit.
* Arguments:
	- uint32 MechanicType
		*

* Returns;
	- False if no buffs were dispelled, true if more than 0 were dispelled.
*/
bool Unit::RemoveAllAurasByMechanicMulti( uint32 MechanicTypeFlags , uint32 MaxDispel = -1 , bool HostileOnly = true )
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	//sLog.outString( "Unit::MechanicImmunityMassDispel called, mechanic: %u" , MechanicType );
	uint32 DispelCount = MaxDispel;
	uint32 start_at;

	if( HostileOnly )
		start_at = MAX_POSITIVE_AURAS;
	else
		start_at = FIRST_AURA_SLOT;

	for(uint32 x = start_at ; x < m_auras_neg_size ; x++ )
		if( m_auras[x] )
		{
			if( m_auras[x]->GetSpellProto()->MechanicsTypeFlags & MechanicTypeFlags ) // Remove all mechanics of type MechanicType (my english goen boom)
			{
				//sLog.outString( "Removed aura. [AuraSlot %u, SpellId %u]" , x , m_auras[x]->GetSpellId() );
				m_auras[x]->Remove(); // EZ-Remove
				DispelCount--;
				if( DispelCount == 0 )
				{ 
					return true;
				}
			}
			else if( MechanicTypeFlags & ( 1 << MECHANIC_ENSNARED ) ) // if got immunity for slow, remove some that are not in the mechanics
			{
				for( int i=0 ; i<MAX_SPELL_EFFECT_COUNT ; i++ )
				{
					// SNARE + ROOT
					if( m_auras[x]->GetSpellProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MOD_DECREASE_SPEED || m_auras[x]->GetSpellProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MOD_ROOT )
					{
						m_auras[x]->Remove();
						break;
					}
				}
			}
		}
	return ( DispelCount != MaxDispel );
}
/*
void Unit::RemoveAllMovementImpairing()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	for( uint32 x = MAX_POSITIVE_AURAS; x < m_auras_neg_size; x++ )
	{
		if( m_auras[x] != NULL )
		{
			if( m_auras[x]->GetSpellProto()->MechanicsType == MECHANIC_ROOTED 
				|| m_auras[x]->GetSpellProto()->MechanicsType == MECHANIC_ENSNARED
				|| m_auras[x]->GetSpellProto()->MechanicsType == MECHANIC_DAZED)

			{
				m_auras[x]->Remove();
			}
			else
			{
				for( int i = 0; i < 3; i++ )
				{
					if( m_auras[x]->GetSpellProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MOD_DECREASE_SPEED 
					|| m_auras[x]->GetSpellProto()->eff[i].EffectApplyAuraName == SPELL_AURA_MOD_ROOT )
					{
						m_auras[x]->Remove();
						break;
					}
				}
			}
		}
	}
}*/

void Unit::setAttackTimer(int32 time, bool offhand)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if(!time)
		time = offhand ? m_uint32Values[UNIT_FIELD_BASEATTACKTIME_1] : m_uint32Values[UNIT_FIELD_BASEATTACKTIME];

	time = std::max(1000,float2int32(float(time)*GetSpellHaste()));

	if(time>300000)		// just in case.. shouldn't happen though
		time=offhand ? m_uint32Values[UNIT_FIELD_BASEATTACKTIME_1] : m_uint32Values[UNIT_FIELD_BASEATTACKTIME];

	if(offhand)
		m_attackTimer_1 = getMSTime() + time;
	else
		m_attackTimer = getMSTime() + time;
}

bool Unit::isAttackReady(bool offhand)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if(offhand)
	{ 
		return (getMSTime() >= m_attackTimer_1) ? true : false;
	}
	else
		return (getMSTime() >= m_attackTimer) ? true : false;
}

void Unit::ReplaceAIInterface(AIInterface *new_interface) 
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
 
	delete m_aiInterface;	//be carefull when you do this. Might screw unit states !
	m_aiInterface = new_interface; 
}
/*
void Unit::EventUpdateFlag()  
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
  
	CombatStatus.TryToClearAttackTargets(); 
}*/

void Unit::EventModelChange()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	UnitModelSizeEntry  *newmodelhalfsize = NULL;

	//mounts are bigger then normal size
	if( GetUInt32Value( UNIT_FIELD_MOUNTDISPLAYID ) )
		newmodelhalfsize = UnitModelSizeStorage.LookupEntry( GetUInt32Value( UNIT_FIELD_MOUNTDISPLAYID ) );
	if( newmodelhalfsize == NULL )
		newmodelhalfsize = UnitModelSizeStorage.LookupEntry( GetUInt32Value( UNIT_FIELD_DISPLAYID ) );

	if( newmodelhalfsize )
	{
		ModelHalfSize = newmodelhalfsize->HalfSize;
//		if( IsCreature() && SafeCreatureCast(this)->GetProto() )
//			ModelHalfSize *=  SafeCreatureCast(this)->GetProto()->BoundingRadius;
	}
	else
		ModelHalfSize = 1.0f; //baaad, but it happens :(
}

void Unit::RemoveFieldSummon()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	uint64 guid = GetUInt64Value(UNIT_FIELD_SUMMON);
	if(guid && GetMapMgr())
	{
		Creature *summon = GetMapMgr()->GetCreature(guid);
		if (summon)
		{
			summon->RemoveFromWorld(false,true);
			summon->SafeDelete();
		}
		//field summons should not have this. Only pets
//		SetUInt64Value(UNIT_FIELD_SUMMON, 0);
	}
}

//what is an Immobilize spell ? Have to add it later to spell effect handler
void Unit::EventStunOrImmobilize(Unit *proc_target, bool is_victim)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if ( this == proc_target )
		return; //how and why would we stun ourselfs

	int32 t_trigger_on_stun,t_trigger_on_stun_chance;
	if( is_victim == false )
	{
		t_trigger_on_stun = trigger_on_stun;
		t_trigger_on_stun_chance = trigger_on_stun_chance;
	}
	else
	{
		t_trigger_on_stun = trigger_on_stun_victim;
		t_trigger_on_stun_chance = trigger_on_stun_chance_victim;
	}

	if( t_trigger_on_stun )
	{
		if( t_trigger_on_stun_chance < 100 && !RandChance( t_trigger_on_stun_chance ) )
		{ 
			return;
		}

		SpellEntry *spellInfo = dbcSpell.LookupEntry(t_trigger_on_stun);

		if(!spellInfo)
		{ 
			return;
		}

		Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		spell->Init(this, spellInfo ,true, NULL);
		SpellCastTargets targets;

		if ( spellInfo->procFlags2 & PROC2_TARGET_SELF )
			targets.m_unitTarget = GetGUID();
		else if ( proc_target ) 
			targets.m_unitTarget = proc_target->GetGUID();
		else 
			targets.m_unitTarget = GetGUID();
		spell->prepare(&targets);
	}
}
void Unit::RemoveExtraStrikeTarget(SpellEntry *spell_info)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	for(CommitPointerListNode<ExtraStrike> *i = m_extraStrikeTargets.begin();i != m_extraStrikeTargets.end(); i = i->Next() )
	{
		if(i->data->deleted == false && spell_info == i->data->spell_info)
		{
			m_extrastriketargetc--;
			i->data->deleted = true;
		}
	}
}

void Unit::AddExtraStrikeTarget(SpellEntry *spell_info, uint32 charges)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	for(CommitPointerListNode<ExtraStrike> *i = m_extraStrikeTargets.begin();i != m_extraStrikeTargets.end(); i = i->Next() )
	{
		//a pointer check or id check ...should be the same
		if(spell_info == i->data->spell_info)
		{
			if (i->data->deleted == true)
			{
				i->data->deleted = false;
				m_extrastriketargetc++;
			}
			i->data->charges = charges;
			return;
		}
	}

	ExtraStrike *es = new ExtraStrike;

	es->spell_info = spell_info;
	es->charges = charges;
	es->deleted = false;
	m_extraStrikeTargets.push_front(es);
	m_extrastriketargetc++;
}

int32 Unit::DoDamageSplitTarget(int32 res, uint32 school_type, bool melee_dmg)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if( res <= 0 )
		return 0;
	Unit * splittarget;
	int32 splitdamage, tmpsplit;
	DamageSplitTarget * ds = m_damageSplitTarget;
		
	splittarget = (GetMapMgr() != NULL) ? GetMapMgr()->GetUnit( ds->m_target ) : NULL;
	if( splittarget != NULL && res > 0 && splittarget->isAlive() ) 
	{
		// calculate damage
		tmpsplit = ds->m_flatDamageSplit;
		if( tmpsplit > res)
			tmpsplit = res; // prevent < 0 damage
		if( tmpsplit > splittarget->GetInt32Value( UNIT_FIELD_HEALTH ) )
			tmpsplit = splittarget->GetInt32Value( UNIT_FIELD_HEALTH );
		splitdamage = tmpsplit;
		res -= tmpsplit;

		tmpsplit = float2int32( ds->m_pctDamageSplit * res );
		if( tmpsplit > res )
			tmpsplit = res;
		if( tmpsplit > splittarget->GetInt32Value( UNIT_FIELD_HEALTH ) )
			tmpsplit = splittarget->GetInt32Value( UNIT_FIELD_HEALTH );
		splitdamage += tmpsplit;
		res -= tmpsplit;

		if( splitdamage )
		{
			int32 abs = splittarget->ResilianceAbsorb( splitdamage, NULL, NULL );
			splitdamage -= abs;
			splittarget->DealDamage(splittarget, splitdamage, ds->m_spellId);

			// Send damage log
			if (melee_dmg) 
			{
				dealdamage sdmg;

				sdmg.full_damage = splitdamage;
				sdmg.resisted_damage = 0;
				sdmg.school_type = school_type;
				sdmg.absorbed_damage = abs;
				SendAttackerStateUpdate(this, splittarget, &sdmg, splitdamage, 0, 0, 0, ATTACK);
			}
			else 
			{
//				SendSpellNonMeleeDamageLog(this, splittarget, ds->m_spellId, splitdamage, school_type, 0, 0, true, 0, 0, true);
				uint32 school_mask;
				if( school_type < SCHOOL_COUNT )
					school_mask = g_spellSchoolConversionTable[ school_type ];
				else
					school_mask = SCHOOL_NORMAL;
				this->SendCombatLogMultiple( splittarget, splitdamage, 0, abs, 0, ds->m_spellId, school_mask, COMBAT_LOG_SPELL_DAMAGE, 0 );
			}
		}
	}

	return res;
}

void Unit::UpdatePowerAmm(bool forced, int8 type, int32 value)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE

//return; //is this required in cataclysm ? Seems like we can diurectly update power fields again

	if( type == -1 )
		type = GetPowerType();
	if( value == 0xFFFF )
		value = GetPower(  type );
	else
		forced = true;

	Unit *p = GetTopOwner();
	if( p->IsPlayer() == false
		|| SafePlayerCast(p)->GetSession() == NULL
		|| ( forced==false 
			&& m_updateMask.GetBit( UNIT_FIELD_POWER1 + PowerFields[ type ] ) == 0
				) //there is a small chance there is no power update needed
		)
		return;
/*
13329
*/
	sStackWorldPacket( data, SMSG_POWER_UPDATE, 19+10);
	FastGUIDPack(data, GetGUID());
	data << uint32( 1 );	//block counter
//	foreach block
	{
		data << uint8( type );
		data << int32( value );
	}
//	SendMessageToSet(&data, true); //we make the power fields private and send this packet to all players
	SafePlayerCast( p )->GetSession()->SendPacket(&data); //we send packet only to self. Rest of players get power amm from A9
}
/*
void Unit::SendHealLog(Object *from,uint32 spell,uint32 amm,bool self)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if( !amm )
	{ 
		return;
	}

	sStackWorldPacket( data, SMSG_SPELLHEALLOG, 50 );
	data << GetNewGUID();
	data << from->GetNewGUID();
	data << spell;
	data << amm;
	data << uint8(0); //critical
#ifdef MINIMIZE_NETWORK_TRAFIC
	SendMessageToDuel( &data );
#else
	SendMessageToSet(&data, true);
#endif
}*/

bool Unit::IsPvPFlagged()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
   return HasByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_PVP);
}

void Unit::SetPvPFlag()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
    SetByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_PVP);

	if( GetUInt32Value( UNIT_FIELD_CREATEDBY ) && GetMapMgr() )
	{
		Unit *owner = GetMapMgr()->GetUnit( GetUInt64Value( UNIT_FIELD_CREATEDBY ) );
		if( owner && owner!=this && !owner->IsPvPFlagged() )
			owner->SetPvPFlag();
	}
}

//! Removal
void Unit::RemovePvPFlag()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
   RemoveByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_PVP);
}

bool Unit::IsFFAPvPFlagged()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
 
   return HasByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_FFA_PVP);
}

void Unit::SetFFAPvPFlag()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
    SetByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_FFA_PVP);

	if( GetUInt32Value( UNIT_FIELD_CREATEDBY ) && GetMapMgr() )
	{
		Unit *owner = GetMapMgr()->GetUnit( GetUInt64Value( UNIT_FIELD_CREATEDBY ) );
		if( owner && owner!=this && !owner->IsFFAPvPFlagged() )
			owner->SetFFAPvPFlag();
	}
}

void Unit::RemoveFFAPvPFlag()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
   RemoveByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_FFA_PVP);
}

//draw weapon while in combat or restore sheath
void Unit::SetWeaponSheath(uint8 new_sheath)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if( new_sheath == WEAPON_SHEATH_RESTORE_SPAWN )
	{
		if( IsCreature() )
		{
			Creature *pcr = SafeCreatureCast(this);
			if( pcr->m_spawn )
				SetByte(UNIT_FIELD_BYTES_2, 0, (uint8)pcr->m_spawn->bytes2);
			else 
				SetByte(UNIT_FIELD_BYTES_2, 0, WEAPON_SHEATH_NONE);
		}
		else
			SetByte(UNIT_FIELD_BYTES_2, 0, WEAPON_SHEATH_NONE);
	}
	else 
		SetByte(UNIT_FIELD_BYTES_2, 0, new_sheath);
}

void Unit::SetPower(uint32 type, int32 value)
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	if( PowerFields[type] == - 1)
		return;
	uint32 maxpower = GetMaxPower( type );
	if(value < MinPowerType[type])
	  value = MinPowerType[type];
	else if(value > (int32)maxpower)
	  value = maxpower;
	//well fuck, what about regenerating secondary power
//	if( GetPowerType() == type )
		SetUInt32Value( UNIT_FIELD_POWER1 + PowerFields[ type ], value );
}

int32 Unit::GetPower( uint32 type )
{ 
	if( PowerFields[type] == - 1)
		return 0;
	return GetInt32Value( UNIT_FIELD_POWER1 + PowerFields[ type ] );
}   

void Unit::AddExtraAttack( ExtraAttack *ea )
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	//check if we are stacking aura that is giving extra strikes. We do not proc all extra strikes at once for these
	for(CommitPointerListNode<ExtraAttack> *i = m_extra_attacks.begin();i != m_extra_attacks.end();i = i->Next() )
		if( i->data->sp == ea->sp )
		{
			i->data->count += ea->count;	//merge auras of same 
			delete ea;
			return;
		}
	m_extra_attacks.push_front( ea );
}

Unit *Unit::GetTopOwner()
{
	Unit *topA = this;
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

void Unit::RageEventDamageTaken(int32 full_dmg, Unit *attacker )
{
	int32 val;
//	float level = (float)getLevel();
	// Conversion Value
//	float c = 0.0091107836f * level * level + 3.225598133f * level + 4.2652911f;
//	val = 2.5f * dmg.full_damage / c;
	val = full_dmg * 100 * 2 / ( GetMaxHealth() + 1);	//1 % health loss into 2 rage points

	// Berserker Rage Effect + 35% rage when taking damage by Aziel
//	if ( pVictim->HasAuraWithNameHash( SPELL_HASH_BERSERKER_RAGE, 0, AURA_SEARCH_POSITIVE ) )
//		val *= 1.35f;

	if( IsPlayer() )
		val = val * SafePlayerCast( this )->rageFromDamageTaken / 100;

	val *= 10;
	val *= sWorld.getRate(RATE_POWER2);

	//sLog.outDebug( "Rd(%i) d(%i) c(%f) rage = %f", realdamage, dmg.full_damage, c, val );

	ModPower( POWER_TYPE_RAGE, (int32)val );
//	UpdatePowerAmm();
}

Group *Unit::GetGroup() 
{
	Unit *topA = GetTopOwner();
	if( topA->IsPlayer() )
		return SafePlayerCast( topA )->GetGroup();
	return NULL; 
}

bool Unit::CanCastVehicleSpell( uint32 spellID )
{
	Vehicle * vehicle = GetVehicle();
	if( vehicle != NULL )
		return vehicle->CanCastSpell( spellID );
	return false;
} 

void Unit::smsg_TalentsInfo()
{
	INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE
	WorldPacket data(SMSG_TALENTS_INFO, 1000);
	data << uint8( IsPet() );
	if( IsPet() && SafePetCast( this )->GetPetOwner() && SafePetCast( this )->GetPetOwner()->GetSession() )	// send just the update
	{
		SafePetCast( this )->fill_talent_info_in_packet( data );
		SafePetCast( this )->GetPetOwner()->GetSession()->SendPacket(&data);
	} 
	else if( IsPlayer() && SafePlayerCast( this )->GetSession() )
	{
		SafePlayerCast( this )->fill_talent_info_in_packet( data );
		SafePlayerCast( this )->GetSession()->SendPacket(&data);
	}
}

float Unit::ResilianceAbsorbPCT( Object *attacker )
{
	//resiliance is a PVP only attribute
	if( attacker != NULL && attacker->IsCreature() == true && attacker->IsPet() == false && SafeCreatureCast( attacker )->IsPvPFlagged() == false )
		return 0.0f;

	Player *pus;
	if( IsPlayer() )
		pus = SafePlayerCast( this );
	else if( IsPet() )
		pus = SafePetCast( this )->GetPetOwner() ;
	else
	{
		Unit *t = this->GetTopOwner();
		if( t->IsPlayer() )
			pus = SafePlayerCast( t );
		else
			pus = NULL;
	}
	if( pus )
	{
		float dmg_reduction_pct;
		//atm we are using the same resiliance for everything. There is no point to test what is what if they have the same value
		dmg_reduction_pct = SafePlayerCast(pus)->CalcRating( PLAYER_RATING_MODIFIER_RANGED_CRIT_RESILIENCE ) / 100.0f;
		dmg_reduction_pct = dmg_reduction_pct * 0.75f;  //this is blizzlike
//		dmg_reduction_pct = dmg_reduction_pct * 0.88f; 
//float dmg_reduction_pct1 = SafePlayerCast(pus)->CalcRating( PLAYER_RATING_MODIFIER_MELEE_CRIT ) / 100.0f;
//float dmg_reduction_pct2 = SafePlayerCast(pus)->CalcRating( PLAYER_RATING_MODIFIER_RANGED_CRIT_RESILIENCE ) / 100.0f;
//float dmg_reduction_pct3 = SafePlayerCast(pus)->CalcRating( PLAYER_RATING_MODIFIER_SPELL_CRIT ) / 100.0f;
		//if this is crit dmg then multiply reisliance by 2.2f
		if( dmg_reduction_pct >= 0.8f )
			dmg_reduction_pct = 0.8f; // not sure about this cap, defenetly not good to resist all
		return dmg_reduction_pct;
	}
	return 0.0f;
}

int32 Unit::ResilianceAbsorb(int32 dmg, SpellEntry *sp, Object *attacker )
{
	//morron protection
	if( dmg <= 0 )
		return 0;

	float dmg_reduction_pct = ResilianceAbsorbPCT( attacker );
	int32 reduction = float2int32( dmg * dmg_reduction_pct );
	return reduction;
}

//after N seconds if we did not leave this area we will get another spell
void Unit::AuraOfForebodingDelayedCheck(float x, float y, float radius, uint32 castspell, uint64 caster)
{
	//because it crashed
	if( GetMapMgr() == NULL )
		return;
	float dist = GetDistanceSq( x, y, GetPositionZ() );
	if( dist <= radius )
	{
		Unit *pcaster = GetMapMgr()->GetUnit( caster );
		if( pcaster )
			pcaster->CastSpell( this, castspell, true );
	}
}

void Unit::SMSG_HoverChange( bool enable_hover )
{
	if( enable_hover )
	{
		sStackWorldPacket( data, SMSG_MOVE_SET_HOVER_DIST, 50 );
		uint64 guid = GetGUID();
		uint8 *guid_bytes = (uint8*)&guid;
		data << uint8( 0xBB );
		data << ObfuscateByte( guid_bytes[7] );   
		data << ObfuscateByte( guid_bytes[6] );   
		data << ObfuscateByte( guid_bytes[2] );   
		data << ObfuscateByte( guid_bytes[0] );   
		data << uint32( 3 );
		data << ObfuscateByte( guid_bytes[3] );   
		data << ObfuscateByte( guid_bytes[1] );   
		data << uint16( 0 );
		data << uint32( m_PositionUpdateCounter++ );
		data << uint16( 0x0307 );
		data << uint32( m_PositionUpdateCounter++ );
		data << uint16( 0x4701 );
		data << uint32( m_PositionUpdateCounter++ );
		data << uint16( 0x0B25 );
		SendMessageToSet(&data, true);
	}
	else
	{
		sStackWorldPacket( data, SMSG_MOVE_SET_HOVER_DIST, 50 );
		uint64 guid = GetGUID();
		uint8 *guid_bytes = (uint8*)&guid;
		data << uint8( 0xBB );
		data << ObfuscateByte( guid_bytes[7] );   
		data << ObfuscateByte( guid_bytes[6] );   
		data << ObfuscateByte( guid_bytes[2] );   
		data << ObfuscateByte( guid_bytes[0] );   
		data << uint32( 3 );
		data << ObfuscateByte( guid_bytes[3] );   
		data << ObfuscateByte( guid_bytes[1] );   
		data << uint16( 0 );
		data << uint32( m_PositionUpdateCounter++ );
		data << uint16( 0x5B45 );
		data << uint32( m_PositionUpdateCounter++ );
		data << uint16( 0xC325 );
		data << uint32( m_PositionUpdateCounter++ );
		data << uint16( 0xC705 );
		SendMessageToSet(&data, true);
	}
}

Unit *Unit::CheckSpellTargetRedirect( Spell *spell )
{
	//general case, there is like 1-2 spells that can redirect spell targetting
	if( m_magnetcaster == 0 )
		return this;
	//we cannot redirect melee spells
	if( ( spell->GetProto()->c_is_flags & SPELL_FLAG_IS_ALLOW_MAGNET_TARGET ) == 0 )
		return this;
	//wtf check
	if( GetMapMgr() == NULL )
		return this;
	Unit *MagnetTarget = GetMapMgr()->GetUnit( m_magnetcaster );
	//no more redirect target
	if ( MagnetTarget == NULL || MagnetTarget->isAlive() == false )
	{
		m_magnetcaster = 0;
		return this;
	}
	//check for remaining redirect charges
	if( MagnetTarget->m_magnetcharges <= 0 )
		return this;
	if( MagnetTarget->m_magnetcharges == 1 ) 
	{
		MagnetTarget->m_magnetcharges = 0;
		if( MagnetTarget->IsCreature() )
		{
			uint32 m_castTime = GetCastTime( dbcSpellCastTime.LookupEntry( spell->GetProto()->CastingTimeIndex ) );
			SafeCreatureCast( MagnetTarget)->Despawn( m_castTime + 1000, 0 );
			//rewrite spell target for the next effects also
			spell->m_targets.m_unitTarget = MagnetTarget->GetGUID();
		}
	}
	else
	{
		MagnetTarget->m_magnetcharges--;
	}

	return MagnetTarget;
}

void Unit::ModSpellReflectList( uint32 OwnerSpell, uint32 chance, int32 school, uint32 charges, bool apply )
{
	if(apply)
	{
		SpellEntry *sp = dbcSpell.LookupEntry( OwnerSpell );
		if (sp == NULL) 
		{ 
			return;
		}

		CommitPointerListNode<struct ReflectSpellSchool> *itr;
		m_reflectSpellSchool.BeginLoop();
		for(itr = m_reflectSpellSchool.begin(); itr != m_reflectSpellSchool.end();itr = itr->Next() )
			if( OwnerSpell == itr->data->spellId )
				m_reflectSpellSchool.SafeRemove(itr,1);
		m_reflectSpellSchool.EndLoopAndCommit();

		ReflectSpellSchool *rss = new ReflectSpellSchool;

		rss->chance = chance;
		rss->spellId = OwnerSpell;
		rss->school = school;
		rss->require_aura_hash = 0;
		rss->charges = charges;
		m_reflectSpellSchool.push_front(rss);
	}
	else
	{
		CommitPointerListNode<struct ReflectSpellSchool> *itr;
		m_reflectSpellSchool.BeginLoop();
		for(itr = m_reflectSpellSchool.begin(); itr != m_reflectSpellSchool.end();itr = itr->Next() )
			if( OwnerSpell == itr->data->spellId)
				m_reflectSpellSchool.SafeRemove(itr,1);
		m_reflectSpellSchool.EndLoopAndCommit();
	}
}

void Unit::VanishFromSight()
{
	//combat status
	CombatStatus.ClearAttackers();
	//others targetting us
	InRangeSetRecProt::iterator itr2;
	AquireInrangeLock(); //make sure to release lock before exit function !
	InrangeLoopExitAutoCallback AutoLock;
	for( itr2 = GetInRangeSetBegin( AutoLock ); itr2 != GetInRangeSetEnd(); )
	{
		Object *ot = *itr2;
		itr2++;
		if( ot->IsUnit() == false || !SafeUnitCast(ot)->isAlive() )
			continue;
		Unit *ut = SafeUnitCast( ot );
		if( ut->IsCreature() && SafeCreatureCast( ut )->CanSee( this ) == true )
			continue;
		if( ut->IsPlayer() && SafePlayerCast( ut )->CanSee( this ) == true )
			continue;
//		if( ot->IsCreature() && SafeCreatureCast( ot )->GetAIInterface() )
//			SafeCreatureCast( ot )->GetAIInterface()->RemoveThreatByPtr( this );	
		if( isAttackable( ut, this, false ) )
			ut->InterruptSpell( this->GetGUID() );
		if( ot->IsPlayer() && SafePlayerCast( ot )->GetTarget() == this->GetGUID() )
			SafePlayerCast( ot )->SetTarget( 0 );
	}
	ReleaseInrangeLock();
}
/*
  arcemu-world.exe!Unit::FindQueuedAura Line 7406
   arcemu-world.exe!Spell::SpellEffectApplyAura Line 2642
   arcemu-world.exe!Spell::HandleEffects Line 3471
   arcemu-world.exe!Spell::cast Line 1876
   arcemu-world.exe!Spell::prepare Line 1494
   arcemu-world.exe!Player::SetShapeShift Line 12366
   arcemu-world.exe!Aura::SpellAuraModShapeshift Line 5461
   arcemu-world.exe!Aura::ApplyModifiers Line 1227
   arcemu-world.exe!Aura::Remove Line 1047
   arcemu-world.exe!Unit::AddAura Line 3123
   arcemu-world.exe!Spell::HandleAddAura Line 3579
   arcemu-world.exe!Spell::cast Line 1906
   arcemu-world.exe!Spell::prepare Line 1494
   arcemu-world.exe!WorldSession::HandleCastSpellOpcode Line 453
   arcemu-world.exe!WorldSession::Update Line 214
   arcemu-world.exe!MapMgr::_PerformObjectDuties Line 2143
   arcemu-world.exe!MapMgr::Do Line 1807
   arcemu-world.exe!MapMgr::run Line 1725
   arcemu-world.exe!thread_proc Line 317
*/
Aura *Unit::FindQueuedAura( uint32 SpellID )
{
	Aura *ret = NULL;
	CommitPointerListNode<Aura> *itr;
	QueueAuraAdd.BeginLoop();
	for( itr = QueueAuraAdd.begin(); itr != QueueAuraAdd.end(); itr = itr->Next() )
		if( itr->IsDeleted == 0 //due to multi threading this has a chance to get to non 0 :(
			&& itr->data->GetSpellId() == SpellID ) //due to multi threading there is a chance this gets to NULL
		{
			ret = itr->data;
			break;
		}
	QueueAuraAdd.EndLoopAndCommit();
	return ret;
}

void Unit::MarkHaveProcEvent( uint32 MultiFlags )
{
	//do not take into count filter flags because they can exclude each other
	m_procSpellsHaveTheseFlags |= ( MultiFlags & PROC_EVENT_PROC_FLAGS );
/*	for(uint32 i=0;i<NUMBER_OF_EVENT_FLAGS_USED;i++)
		if( ( (1<<i) & MultiFlags ) )
		{
			m_procSpellsHaveTheseFlags |= (1<<i);
			m_procSpellsHaveTheseFlagsCount[i]++;
		} */
}

void Unit::UnMarkHaveProcEvent( uint32 MultiFlags )
{
/*	for(uint32 i=0;i<NUMBER_OF_EVENT_FLAGS_USED;i++)
		if( ( (1<<i) & MultiFlags ) )
		{
			m_procSpellsHaveTheseFlagsCount[i]--;
			if( m_procSpellsHaveTheseFlagsCount[i] == 0 )
				m_procSpellsHaveTheseFlags &= ~(1<<i);
		} */
}

void Unit::RegisterProcStruct( ProcTriggerSpell *pts )
{
	MarkHaveProcEvent( pts->procFlags );
	m_procSpells.push_front( pts );
}

int32 Unit::UnRegisterProcStruct( void *Owner, uint32 SpellId, int32 Count, uint8 DisableEnable )
{
	uint32 RemovedCount = 0;
	CommitPointerListNode<ProcTriggerSpell> *itr;
	for(itr = m_procSpells.begin();itr != m_procSpells.end();itr = itr->Next() )
		if( itr->data->deleted == false
			&& ( SpellId == 0 || itr->data->origId == SpellId )
			&& ( Owner == NULL || itr->data->owner == Owner )
			)
		{
			if( DisableEnable == 1 )
			{
				itr->data->procFlags = 0;
			}
			else if( DisableEnable == 2 )
			{
				itr->data->procFlags = dbcSpell.LookupEntryForced( itr->data->origId )->procFlags;
			}
			else
			{
				UnMarkHaveProcEvent( itr->data->procFlags );
				itr->data->deleted = true;
			}
			Count--;
			RemovedCount++;
			if( Count <= 0 )
				break;
		}
	return RemovedCount;
}

ProcTriggerSpell *Unit::HasProcSpell( uint32 SpellId )
{
	CommitPointerListNode<ProcTriggerSpell> *itr;
	for(itr = m_procSpells.begin();itr != m_procSpells.end();itr = itr->Next() )
		if( itr->data->deleted == false	&& itr->data->origId == SpellId )
			return	itr->data;
	return NULL;
}

void Unit::RegisterNewChargeStruct( SpellCharge &pts )
{
	MarkHaveProcEvent( pts.ProcFlag );
	m_chargeSpells.insert( make_pair( pts.spe->Id, pts ) );
}

void Unit::ClearTargetGUID( )
{
	uint64 TargetGUID = GetTargetGUID();
	if( TargetGUID == 0 )
		return;
	SetUInt64Value( UNIT_FIELD_TARGET, 0 );
	if( GetMapMgr() == NULL )
		return;
	Unit *MyTarget = GetMapMgr()->GetUnit( TargetGUID );
	if( MyTarget == NULL )
		return;
	if( GetAIInterface() )
		GetAIInterface()->EventRemoveInrangeUnit( MyTarget );
	if( IsPlayer() && SafePlayerCast( this )->GetSession() )
	{
		sStackWorldPacket( data, SMSG_CLEAR_TARGET, 8 + 10 );
		data << uint64( 0 );
		SafePlayerCast( this )->GetSession()->SendPacket( &data );
	}
}

void UnitChain::AddUnit(Unit* u)
{
	m_units.insert(u);
	u->m_chain = this;
}

void UnitChain::RemoveUnit(Unit* u)
{
	m_units.erase(u);
	u->m_chain = NULL;

	if (m_units.empty() == true && !m_persist)
		delete this;
}

UnitSMMods::UnitSMMods()
{
	SM_FCriticalChanceTarget=SM_FCriticalChance=SM_FDur=SM_PDur=SM_PRadius=SM_FRadius=SM_PRange=SM_FRange=SM_PCastTime=SM_FCastTime=SM_PCriticalDamage=SM_PDOT=SM_FDOT=SM_FEffectBonus=SM_FEffectTargetBonus=SM_PEffectBonus=SM_PEffectTargetBonus=SM_FDamageBonus=SM_PDamageBonus=SM_PMiscEffect=SM_FMiscEffect=SM_FHitchance=SM_PAPBonus=SM_PCost=SM_FCost=SM_PNonInterrupt=SM_PJumpReduce=SM_FSpeedMod=SM_FAdditionalTargets=SM_PPenalty=SM_PCooldownTime=SM_FCooldownTime=SM_FChanceOfSuccess=SM_FRezist_dispell=SM_PRezist_dispell=SM_FCharges=SM_PCharges=SM_FThreat=SM_PThreat=SM_FTime=SM_FAddEffect1=SM_FAddEffect2=SM_FAddEffect3=SM_PAddEffect1=SM_PAddEffect2=SM_PAddEffect3=SM_FAddEffect_DUMMY=SM_PAddEffect_DUMMY=NULL;
	SM_FGlobalCooldownTime=NULL;
}

UnitSMMods::~UnitSMMods()
{
	if( SM_FCriticalChance != NULL ) {
		delete [] SM_FCriticalChance;
		SM_FCriticalChance = NULL;
	}

	if( SM_FCriticalChanceTarget != NULL ) {
		delete [] SM_FCriticalChanceTarget;
		SM_FCriticalChanceTarget = NULL;
	}

	if( SM_FDur != NULL ) {
		delete [] SM_FDur;
		SM_FDur = NULL;
	}
	
	if( SM_PDur != NULL ) {
		delete [] SM_PDur;
		SM_PDur = NULL;
	}

	if( SM_FRadius != NULL ) {
		delete [] SM_FRadius;
		SM_FRadius = NULL;
	}

	if( SM_FRange != NULL ) {
		delete [] SM_FRange;
		SM_FRange = NULL;
	}

	if( SM_PCastTime != NULL ) {
		delete [] SM_PCastTime;
		SM_PCastTime = NULL;
	}

	if(SM_FCastTime != NULL ) {
		delete [] SM_FCastTime;
		SM_FCastTime = NULL;
	}

	if(SM_PCriticalDamage != NULL ) {
		delete [] SM_PCriticalDamage;
		SM_PCriticalDamage = NULL;
	}

	if(SM_FDOT != NULL ) {
		delete [] SM_FDOT;
		SM_FDOT = NULL;
	}

	if(SM_PDOT != NULL ) {
		delete [] SM_PDOT;
		SM_PDOT = NULL;
	}

	if(SM_PEffectBonus != NULL ) {
		delete [] SM_PEffectBonus;
		SM_PEffectBonus = NULL;
	}

	if(SM_PEffectTargetBonus != NULL ) {
		delete [] SM_PEffectTargetBonus;
		SM_PEffectTargetBonus = NULL;
	}

	if(SM_FEffectBonus != NULL ) {
		delete [] SM_FEffectBonus;
		SM_FEffectBonus = NULL;
	}

	if(SM_FEffectTargetBonus != NULL ) {
		delete [] SM_FEffectTargetBonus;
		SM_FEffectTargetBonus = NULL;
	}

	if(SM_FDamageBonus != NULL ) {
		delete [] SM_FDamageBonus;
		SM_FDamageBonus = NULL;
	}

	if(SM_PDamageBonus != NULL ) {
		delete [] SM_PDamageBonus;
		SM_PDamageBonus = NULL;
	}

	if(SM_PMiscEffect != NULL ) {
		delete [] SM_PMiscEffect;
		SM_PMiscEffect = NULL;
	}

	if(SM_FMiscEffect != NULL ) {
		delete [] SM_FMiscEffect;
		SM_FMiscEffect = NULL;
	}

	if(SM_FHitchance != NULL ) {
		delete [] SM_FHitchance;
		SM_FHitchance = NULL;
	}

	if(SM_PRange != NULL ) {
		delete [] SM_PRange;
		SM_PRange = NULL;
	}

	if(SM_PRadius != NULL ) {
		delete [] SM_PRadius;
		SM_PRadius = NULL;
	}

	if(SM_PAPBonus != NULL ) {
		delete [] SM_PAPBonus;
		SM_PAPBonus = NULL;
	}

	if(SM_PCost != NULL ) {
		delete [] SM_PCost;
		SM_PCost = NULL;
	}

	if(SM_FCost != NULL ) {
		delete [] SM_FCost;
		SM_FCost = NULL;
	}

	if(SM_FAdditionalTargets != NULL ) {
		delete [] SM_FAdditionalTargets;
		SM_FAdditionalTargets = NULL;
	}

	if(SM_PJumpReduce != NULL ) {
		delete [] SM_PJumpReduce;
		SM_PJumpReduce = NULL;
	}

	if(SM_FSpeedMod != NULL ) {
		delete [] SM_FSpeedMod;
		SM_FSpeedMod = NULL;
	}

	if(SM_PNonInterrupt != NULL ) {
		delete [] SM_PNonInterrupt;
		SM_PNonInterrupt = NULL;
	}

//	if(SM_FPenalty != NULL ) {
//		delete [] SM_FPenalty;
//		SM_FPenalty = NULL;
//	}

	if(SM_PPenalty != NULL ) {
		delete [] SM_PPenalty;
		SM_PPenalty = NULL;
	}

	if(SM_FCooldownTime != NULL ) {
		delete [] SM_FCooldownTime;
		SM_FCooldownTime = NULL;
	}

	if(SM_PCooldownTime != NULL ) {
		delete [] SM_PCooldownTime;
		SM_PCooldownTime = NULL;
	}

	if(SM_FGlobalCooldownTime != NULL ) {
		delete [] SM_FGlobalCooldownTime;
		SM_FGlobalCooldownTime = NULL;
	}

	if(SM_FChanceOfSuccess != NULL ) {
		delete [] SM_FChanceOfSuccess;
		SM_FChanceOfSuccess = NULL;
	}

	if(SM_FRezist_dispell != NULL ) {
		delete [] SM_FRezist_dispell;
		SM_FRezist_dispell = NULL;
	}

	if(SM_PRezist_dispell != NULL ) {
		delete [] SM_PRezist_dispell;
		SM_PRezist_dispell = NULL;
	}

	if(SM_FCharges != NULL ) {
		delete [] SM_FCharges;
		SM_FCharges = NULL;
	}

	if(SM_PCharges != NULL ) {
		delete [] SM_PCharges;
		SM_PCharges = NULL;
	}

	if(SM_FThreat != NULL ) {
		delete [] SM_FThreat;
		SM_FThreat = NULL;
	}

	if(SM_PThreat != NULL ) {
		delete [] SM_PThreat;
		SM_PThreat = NULL;
	}

	if(SM_FTime != NULL ) {
		delete [] SM_FTime;
		SM_FTime = NULL;
	}

	if(SM_FAddEffect1 != NULL ) {
		delete [] SM_FAddEffect1;
		SM_FAddEffect1 = NULL;
	}

	if(SM_FAddEffect2 != NULL ) {
		delete [] SM_FAddEffect2;
		SM_FAddEffect2 = NULL;
	}

	if(SM_FAddEffect3 != NULL ) {
		delete [] SM_FAddEffect3;
		SM_FAddEffect3 = NULL;
	}

	if(SM_PAddEffect1 != NULL ) {
		delete [] SM_PAddEffect1;
		SM_PAddEffect1 = NULL;
	}

	if(SM_PAddEffect2 != NULL ) {
		delete [] SM_PAddEffect2;
		SM_PAddEffect2 = NULL;
	}

	if(SM_PAddEffect3 != NULL ) {
		delete [] SM_PAddEffect3;
		SM_PAddEffect3 = NULL;
	}
	if(SM_FAddEffect_DUMMY != NULL ) {
		delete [] SM_FAddEffect_DUMMY;
		SM_FAddEffect_DUMMY = NULL;
	}
	if(SM_PAddEffect_DUMMY != NULL ) {
		delete [] SM_PAddEffect_DUMMY;
		SM_PAddEffect_DUMMY = NULL;
	}
}

void UnitSMMods::InheritFrom(UnitSMMods *ParentMods)
{
	if(ParentMods == NULL)
	{ 
		return;
	}
	if(ParentMods->SM_FCriticalChance)
	{
		if(SM_FCriticalChance==0)
			SM_FCriticalChance = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_FCriticalChance,ParentMods->SM_FCriticalChance,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_FCriticalChanceTarget)
	{
		if(SM_FCriticalChanceTarget==0)
			SM_FCriticalChanceTarget = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_FCriticalChanceTarget,ParentMods->SM_FCriticalChanceTarget,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_FDur)
	{
		if(SM_FDur==0)
			SM_FDur = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_FDur,ParentMods->SM_FDur,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_PDur)
	{
		if(SM_PDur==0)
			SM_PDur = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_PDur,ParentMods->SM_PDur,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_PRadius)
	{
		if(SM_PRadius==0)
			SM_PRadius = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_PRadius,ParentMods->SM_PRadius,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_FRadius)
	{
		if(SM_FRadius==0)
			SM_FRadius = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_FRadius,ParentMods->SM_FRadius,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_FRange)
	{
		if(SM_FRange==0)
			SM_FRange = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_FRange,ParentMods->SM_FRange,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_PCastTime)
	{
		if(SM_PCastTime==0)
			SM_PCastTime = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_PCastTime,ParentMods->SM_PCastTime,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_FCastTime)
	{
		if(SM_FCastTime==0)
			SM_FCastTime = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_FCastTime,ParentMods->SM_FCastTime,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_PCriticalDamage)
	{
		if(SM_PCriticalDamage==0)
			SM_PCriticalDamage = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_PCriticalDamage,ParentMods->SM_PCriticalDamage,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_FDOT)
	{
		if(SM_FDOT==0)
			SM_FDOT = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_FDOT,ParentMods->SM_FDOT,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_PDOT)
	{
		if(SM_PDOT==0)
			SM_PDOT = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_PDOT,ParentMods->SM_PDOT,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_FEffectBonus)
	{
		if(SM_FEffectBonus==0)
			SM_FEffectBonus = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_FEffectBonus,ParentMods->SM_FEffectBonus,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_FEffectTargetBonus)
	{
		if(SM_FEffectTargetBonus==0)
			SM_FEffectTargetBonus = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_FEffectTargetBonus,ParentMods->SM_FEffectTargetBonus,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_PEffectBonus)
	{
		if(SM_PEffectBonus==0)
			SM_PEffectBonus = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_PEffectBonus,ParentMods->SM_PEffectBonus,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_PEffectTargetBonus)
	{
		if(SM_PEffectTargetBonus==0)
			SM_PEffectTargetBonus = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_PEffectTargetBonus,ParentMods->SM_PEffectTargetBonus,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_FDamageBonus)
	{
		if(SM_FDamageBonus==0)
			SM_FDamageBonus = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_FDamageBonus,ParentMods->SM_FDamageBonus,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_PDamageBonus)
	{
		if(SM_PDamageBonus==0)
			SM_PDamageBonus = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_PDamageBonus,ParentMods->SM_PDamageBonus,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_PMiscEffect)
	{
		if(SM_PMiscEffect==0)
			SM_PMiscEffect = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_PMiscEffect,ParentMods->SM_PMiscEffect,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_FMiscEffect)
	{
		if(SM_FMiscEffect==0)
			SM_FMiscEffect = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_FMiscEffect,ParentMods->SM_FMiscEffect,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_FHitchance)
	{
		if(SM_FHitchance==0)
			SM_FHitchance = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_FHitchance,ParentMods->SM_FHitchance,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_PRange)
	{
		if(SM_PRange==0)
			SM_PRange = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_PRange,ParentMods->SM_PRange,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_PRadius)
	{
		if(SM_PRadius==0)
			SM_PRadius = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_PRadius,ParentMods->SM_PRadius,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_PAPBonus)
	{
		if(SM_PAPBonus==0)
			SM_PAPBonus = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_PAPBonus,ParentMods->SM_PAPBonus,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_PCost)
	{
		if(SM_PCost==0)
			SM_PCost = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_PCost,ParentMods->SM_PCost,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_FCost)
	{
		if(SM_FCost==0)
			SM_FCost = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_FCost,ParentMods->SM_FCost,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_FAdditionalTargets)
	{
		if(SM_FAdditionalTargets==0)
			SM_FAdditionalTargets = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_FAdditionalTargets,ParentMods->SM_FAdditionalTargets,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_PJumpReduce)
	{
		if(SM_PJumpReduce==0)
			SM_PJumpReduce = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_PJumpReduce,ParentMods->SM_PJumpReduce,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_FSpeedMod)
	{
		if(SM_FSpeedMod==0)
			SM_FSpeedMod = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_FSpeedMod,ParentMods->SM_FSpeedMod,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_PNonInterrupt)
	{
		if(SM_PNonInterrupt==0)
			SM_PNonInterrupt = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_PNonInterrupt,ParentMods->SM_PNonInterrupt,sizeof(int32)*SPELL_GROUPS_BITS);
	}
//	if(ParentMods->SM_FPenalty)
//	{
//		if(SM_FPenalty==0)
//			SM_FPenalty = new int32[SPELL_GROUPS_BITS];
//		memcpy(SM_FPenalty,ParentMods->SM_FPenalty,sizeof(int32)*SPELL_GROUPS_BITS);
//	}
	if(ParentMods->SM_FCooldownTime)
	{
		if(SM_FCooldownTime==0)
			SM_FCooldownTime = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_FCooldownTime,ParentMods->SM_FCooldownTime,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_PCooldownTime)
	{
		if(SM_PCooldownTime==0)
			SM_PCooldownTime = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_PCooldownTime,ParentMods->SM_PCooldownTime,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_FGlobalCooldownTime)
	{
		if(SM_FGlobalCooldownTime==0)
			SM_FGlobalCooldownTime = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_FGlobalCooldownTime,ParentMods->SM_FGlobalCooldownTime,sizeof(int32)*SPELL_GROUPS_BITS);
	} 
	if(ParentMods->SM_FChanceOfSuccess)
	{
		if(SM_FChanceOfSuccess==0)
			SM_FChanceOfSuccess = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_FChanceOfSuccess,ParentMods->SM_FChanceOfSuccess,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_FRezist_dispell)
	{
		if(SM_FRezist_dispell==0)
			SM_FRezist_dispell = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_FRezist_dispell,ParentMods->SM_FRezist_dispell,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_PRezist_dispell)
	{
		if(SM_PRezist_dispell==0)
			SM_PRezist_dispell = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_PRezist_dispell,ParentMods->SM_PRezist_dispell,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_FCharges)
	{
		if(SM_FCharges==0)
			SM_FCharges = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_FCharges,ParentMods->SM_FCharges,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_PCharges)
	{
		if(SM_PCharges==0)
			SM_PCharges = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_PCharges,ParentMods->SM_PCharges,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_FThreat)
	{
		if(SM_FThreat==0)
			SM_FThreat = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_FThreat,ParentMods->SM_FThreat,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_PThreat)
	{
		if(SM_PThreat==0)
			SM_PThreat = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_PThreat,ParentMods->SM_PThreat,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_FTime)
	{
		if(SM_FTime==0)
			SM_FTime = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_FTime,ParentMods->SM_FTime,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_FAddEffect1)
	{
		if(SM_FAddEffect1==0)
			SM_FAddEffect1 = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_FAddEffect1,ParentMods->SM_FAddEffect1,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_FAddEffect2)
	{
		if(SM_FAddEffect2==0)
			SM_FAddEffect2 = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_FAddEffect2,ParentMods->SM_FAddEffect2,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_FAddEffect3)
	{
		if(SM_FAddEffect3==0)
			SM_FAddEffect3 = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_FAddEffect3,ParentMods->SM_FAddEffect3,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_PAddEffect1)
	{
		if(SM_PAddEffect1==0)
			SM_PAddEffect1 = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_PAddEffect1,ParentMods->SM_PAddEffect1,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_PAddEffect2)
	{
		if(SM_PAddEffect2==0)
			SM_PAddEffect2 = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_PAddEffect2,ParentMods->SM_PAddEffect2,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_PAddEffect3)
	{
		if(SM_PAddEffect3==0)
			SM_PAddEffect3 = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_PAddEffect3,ParentMods->SM_PAddEffect3,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_FAddEffect_DUMMY)
	{
		if(SM_FAddEffect_DUMMY==0)
			SM_FAddEffect_DUMMY = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_FAddEffect_DUMMY,ParentMods->SM_FAddEffect_DUMMY,sizeof(int32)*SPELL_GROUPS_BITS);
	}
	if(ParentMods->SM_PAddEffect_DUMMY)
	{
		if(SM_PAddEffect_DUMMY==0)
			SM_PAddEffect_DUMMY = new int32[SPELL_GROUPS_BITS];
		memcpy(SM_PAddEffect_DUMMY,ParentMods->SM_PAddEffect_DUMMY,sizeof(int32)*SPELL_GROUPS_BITS);
	}
}
