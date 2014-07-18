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

#ifndef UNIX
#include <cmath>
#endif


#if (defined( WIN32 ) || defined( WIN64 ) )
#define HACKY_CRASH_FIXES 1		// SEH stuff
#endif

AIInterface::AIInterface()
{
	m_waypoints=NULL;
	m_canMove = true;
	m_destinationX = m_destinationY = m_destinationZ = 0;
	UnitToFollowGUID = 0;
//	FollowDistance = 0.0f;
	m_followAngle = float(M_PI/2);
	m_TimeRemainingReachDestination = 0;
	m_WayPointsShowing = false;
	m_WayPointsShowBackwards = false;
	m_currentWaypoint = 0;
	m_moveBackward = false;
	m_moveType = 0;
	m_moveRun = false;
	m_moveSprint = false;
	m_moveFly = false;
	m_creatureState = STOPPED;
	m_canCallForHelp = false;
	m_hasCalledForHelp = false;
	m_fleeTimer = 0;
	m_FleeDuration = 0;
	m_canFlee = false;
	m_hasFleed = false;
	m_canRangedAttack = false;
	m_FleeHealth = m_CallForHelpHealth = 0.0f;
	m_AIState = STATE_IDLE;

//	m_updateAssist = false;
//	m_updateTargets = false;
//	m_updateAssistTimer = 1;
	m_updateTargetsTimer = 0;
	m_updateTargetsTimer2 = 0;

	m_nextSpell = NULL;
	m_nextTarget = 0;
	totemspell = NULL;
	m_Unit = NULL;
	m_PetOwnerGUID = NULL;
	m_aiCurrentAgent = AGENT_NULL;
	UnitToFear = NULL;
//	m_outOfCombatRange = 2500;	//huuuge distance
	m_outOfCombatRange = 45*45;	//needs to be larger then large spell cast range

	tauntedBy = NULL;
//	soullinkedWith = NULL;
//	isSoulLinked = false;
//	m_lastFollowX = m_lastFollowY = 0;
	m_totemspelltime = 0;
	m_formationFollowAngle = 0.0f;
	m_formationFollowDistance = 0.0f;
	m_formationLinkTarget = 0;
	m_formationLinkSqlId = 0;
	m_currentHighestThreat = 0;

	disable_combat = false;

	disable_melee = false;
	disable_ranged = false;
	disable_spell = false;

	disable_targeting = false;

	spell_global_cooldown_stamp = 0;
//	UnitToFollow_backup = NULL;
	m_isGuard = false;
	m_isNeutralGuard = false;
	skip_reset_hp = false;
	timed_emotes = NULL;
	timed_emote_expire = 0xFFFFFFFF;
	m_MovementState = MOVEMENTSTATE_STOP;

	m_aiTargets.clear();
//	m_assistTargets.clear();
	m_custom_waypoint_map = NULL;
	call_friend_cooldown = 0;
	entered_combat_stamp = 0xEFFFFFFF;

	m_forced_target = 0;
	agro_range_mod = 1.0f;

#ifdef HACKY_SERVER_CLIENT_POS_SYNC
	moved_for_attack = false;
#endif
	DamageDoneLastMeleeStrike = 0;
	m_UnitToMoveTo = 0;
}

void AIInterface::EventAiInterfaceParamsetFinish()
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if( timed_emotes && timed_emotes->begin() != timed_emotes->end() )
	{
		next_timed_emote = timed_emotes->begin();
		timed_emote_expire = (*next_timed_emote)->expire_after;
	}
}

void AIInterface::Init(Unit *un, AIType at, MovementType mt)
{
//	ASSERT(at != AITYPE_PET);

	m_AIType = at;
	m_MovementType = mt;

	m_AIState = STATE_IDLE;
	m_MovementState = MOVEMENTSTATE_STOP;

	m_Unit = un;

	/*if(!m_DefaultMeleeSpell)
	{
		m_DefaultMeleeSpell = new AI_Spell;
		m_DefaultMeleeSpell->entryId = 0;
		m_DefaultMeleeSpell->spellType = 0;
		m_DefaultMeleeSpell->agent = AGENT_MELEE;
		m_DefaultSpell = m_DefaultMeleeSpell;
	}*/
	m_guardTimer = getMSTime();
	//!!! Object is not yet initialized. Object type is not known atm
	if( m_Unit->IsPlayer() )
	{
		disable_targeting = true;
		skip_reset_hp = true;
	}
}

AIInterface::~AIInterface()
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	m_spells.clear( 1 );
	if( m_custom_waypoint_map )
	{
//		m_custom_waypoint_map->DestroyContent(); //elements are destroyed by creature waypoint list. Silly to store it in 2 lists but we need compatibility
		delete m_custom_waypoint_map;
		m_custom_waypoint_map = NULL;
	}
}

void AIInterface::Init(Unit *un, AIType at, MovementType mt, Unit *owner)
{
//	ASSERT(at == AITYPE_PET || at == AITYPE_TOTEM);

	m_AIType = at;
	m_MovementType = mt;

	m_AIState = STATE_IDLE;
	m_MovementState = MOVEMENTSTATE_STOP;

	m_Unit = un;
	if( owner )
	{
		m_PetOwnerGUID = owner->GetGUID();

		//inherit PVP settings from owner
		if( owner->IsPvPFlagged() )
			m_Unit->SetPvPFlag();
		if( owner->IsFFAPvPFlagged() )
			m_Unit->SetFFAPvPFlag();
		m_outOfCombatRange += 90*90;	//let then run after targets from mile
	}
	if( m_Unit->IsPlayer() )
	{
		disable_targeting = true;
		skip_reset_hp = true;
	}
}

void AIInterface::CancelSpellCast()
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	m_nextSpell = NULL;
	if( m_Unit && m_Unit->m_currentSpell)
		m_Unit->m_currentSpell->safe_cancel();
}

void AIInterface::HandleEvent(uint32 event, Unit* pUnit, uint32 misc1)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if( m_Unit == NULL ) 
	{ 
		return;
	}

	if(m_AIState != STATE_EVADE)
	{
		switch(event)
		{
		case EVENT_ENTERCOMBAT:
		case EVENT_CHAINGENTERCOMBAT:
			{
				entered_combat_stamp = getMSTime();
				if( pUnit == NULL || pUnit->IsDead() || m_Unit->IsDead() ) 
				{ 
					return;
				}

				/* send the message */
				if( m_Unit->GetTypeId() == TYPEID_UNIT )
				{
					if( SafeCreatureCast( m_Unit )->has_combat_text )
						objmgr.HandleMonsterSayEvent( SafeCreatureCast( m_Unit ), MONSTER_SAY_EVENT_ENTER_COMBAT );

					CALL_SCRIPT_EVENT(m_Unit, OnCombatStart)(pUnit);

/*					if( SafeCreatureCast( m_Unit )->m_spawn && ( SafeCreatureCast( m_Unit )->m_spawn->channel_target_go || SafeCreatureCast( m_Unit )->m_spawn->channel_target_creature))
					{
						m_Unit->SetUInt32Value(UNIT_CHANNEL_SPELL, 0);
						m_Unit->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, 0);
					}*/
				}

				// Stop the emote - change to fight emote
				if( m_Unit->IsCreature() )
				{
					if( m_Unit->GetUInt32Value( UNIT_VIRTUAL_ITEM_SLOT_ID_1 ) == 0 )
						m_Unit->SetUInt32Value( UNIT_NPC_EMOTESTATE, EMOTE_STATE_READY1H_ALLOW_MOVEMENT );
					else
						m_Unit->SetUInt32Value( UNIT_NPC_EMOTESTATE, EMOTE_STATE_READY2H_ALLOW_MOVEMENT );
				}
				else
					m_Unit->SetUInt32Value( UNIT_NPC_EMOTESTATE, EMOTE_STATE_READY1H_ALLOW_MOVEMENT );

				m_moveRun = true; //run to the target

				// dismount if mounted
				if( m_Unit->IsCreature() && SafeCreatureCast( m_Unit )->GetCreatureInfo() && !(SafeCreatureCast( m_Unit )->GetCreatureInfo()->Flags1 & CREATURE_FLAG1_FIGHT_MOUNTED))
					m_Unit->SetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID, 0);

				if(m_AIState != STATE_ATTACKING)
					StopMovement(0);

				m_AIState = STATE_ATTACKING;
				if(pUnit && pUnit->GetInstanceID() == m_Unit->GetInstanceID())
				{
					if( disable_targeting == false )
						m_Unit->SetUInt64Value(UNIT_FIELD_TARGET, pUnit->GetGUID());
				}
				if( m_Unit->GetMapMgr() && m_Unit->GetMapMgr()->GetMapInfo() && IS_MAP_RAID( m_Unit->GetMapMgr()->GetMapInfo() ) )
				{
					if(m_Unit->GetTypeId() == TYPEID_UNIT)
					{
						if(SafeCreatureCast(m_Unit)->GetCreatureInfo() && SafeCreatureCast(m_Unit)->GetCreatureInfo()->Rank == 3)
						{
							 m_Unit->GetMapMgr()->AddCombatInProgress(m_Unit->GetGUID());
						}
					}
				}

				if( event == EVENT_ENTERCOMBAT )
				{
					HandleChainAggro(pUnit);
					if(m_AIType == AITYPE_PET)
					{
						if( m_Unit->IsPet() )
						{
							if( m_Unit->isAlive() && m_Unit->IsInWorld() )
							{
								SafePetCast( m_Unit )->HandleAutoCastEvent( AUTOCAST_EVENT_ENTER_COMBAT );
							}
						}
					}
				}
				//give 1 threat to this unit if were not on the threat list
				modThreatByPtr( pUnit, 1 );

				//Zack : Put mob into combat animation. Take out weapons and start to look serious :P
				m_Unit->smsg_AttackStart( pUnit );

				//threath handling
				if( m_Unit->IsCreature() )
					SafeCreatureCast( m_Unit )->m_HighestThreath = 0;
			}break;
		case EVENT_DAMAGEDEALT:
			{
				modThreatByPtr( pUnit, 1 );
			}break;
		case EVENT_LEAVECOMBAT:
			{
				entered_combat_stamp = 0xEFFFFFFF;

				if( m_Unit->IsPet() == false && m_Unit->IsCreature() )
					m_Unit->RemoveNegativeAuras();

				m_Unit->SetWeaponSheath( WEAPON_SHEATH_RESTORE_SPAWN ); //put away Weapon

				Unit* target = NULL;
				if (m_Unit->GetMapMgr() && m_Unit->GetMapMgr()->GetMapInfo())
				{
					switch (m_Unit->GetMapMgr()->GetMapInfo()->type_flags)
					{
					case INSTANCE_FLAG_STATIC_MAP:
					case INSTANCE_FLAG_PVP:
						if (m_outOfCombatRange && _CalcDistanceFromHome() < m_outOfCombatRange)
							target = FindTarget();
						break;

					default:
						target = FindTarget();
						break;
					}

					if(target != NULL)
					{
						AttackReaction(target, 1, 0);
						return;
					}
				}

				//cancel spells that we are casting. Should remove bug where creatures cast a spell after they died
				CancelSpellCast();

				// restart emote
				if(m_Unit->GetTypeId() == TYPEID_UNIT)
				{
					if( SafeCreatureCast( m_Unit )->has_combat_text )
						objmgr.HandleMonsterSayEvent( SafeCreatureCast( m_Unit ), MONSTER_SAY_EVENT_ON_COMBAT_STOP );

					if( SafeCreatureCast( m_Unit )->original_emotestate )
						m_Unit->SetUInt32Value( UNIT_NPC_EMOTESTATE, SafeCreatureCast( m_Unit )->original_emotestate );
					else
						m_Unit->SetUInt32Value( UNIT_NPC_EMOTESTATE, 0 );

/*					if(SafeCreatureCast(m_Unit)->m_spawn && (SafeCreatureCast( m_Unit )->m_spawn->channel_target_go || SafeCreatureCast( m_Unit )->m_spawn->channel_target_creature ) )
					{
						if(SafeCreatureCast(m_Unit)->m_spawn->channel_target_go)
							sEventMgr.AddEvent( SafeCreatureCast( m_Unit ), &Creature::ChannelLinkUpGO, SafeCreatureCast( m_Unit )->m_spawn->channel_target_go, EVENT_CREATURE_CHANNEL_LINKUP, 1000, 5, 0 );

						if(SafeCreatureCast(m_Unit)->m_spawn->channel_target_creature)
							sEventMgr.AddEvent( SafeCreatureCast( m_Unit ), &Creature::ChannelLinkUpCreature, SafeCreatureCast( m_Unit )->m_spawn->channel_target_creature, EVENT_CREATURE_CHANNEL_LINKUP, 1000, 5, 0 );
					}*/
				}

				//reset ProcCount
				ResetProcCounts();
				m_moveRun = true;
				ClearThreathList();	//must be before clear aitargets
				LockAITargets(true);
				m_aiTargets.clear();
				LockAITargets(false);
				m_fleeTimer = 0;
				m_hasFleed = false;
				m_hasCalledForHelp = false;
				m_nextSpell = NULL;
				SetNextTarget( (Unit*)NULL );
				m_Unit->CombatStatus.ClearAttackers();

				if(m_AIType == AITYPE_PET)
				{
					m_AIState = STATE_FOLLOWING;
					UnitToFollowGUID = m_PetOwnerGUID;
//					FollowDistance = 3.0f;
//					m_lastFollowX = m_lastFollowY = 0;
					if( m_Unit->IsPet() )
					{
						SafePetCast( m_Unit )->SetPetAction( PET_ACTION_FOLLOW );
						if( m_Unit->isAlive() && m_Unit->IsInWorld() )
						{
							SafePetCast( m_Unit )->HandleAutoCastEvent( AUTOCAST_EVENT_LEAVE_COMBAT );
						}
						disable_targeting = false;	//in case forced targetting was set and our target died then we allow new target aquisition
					}
					HandleEvent(EVENT_FOLLOWOWNER, 0, 0);
				}
				else
				{
					m_AIState = STATE_EVADE;

					//you can find sweet spots to make mobs go run back and forth without dmging you
					if( m_AIType != AITYPE_PET && !skip_reset_hp && m_Unit->IsPlayer() == false )
						m_Unit->SetUInt32Value(UNIT_FIELD_HEALTH,m_Unit->GetUInt32Value(UNIT_FIELD_MAXHEALTH));

					//why clear follow ?
//					uint64 SavedFollow = UnitToFollowGUID;
//					UnitToFollowGUID = 0;
//					FollowDistance = 0.0f;
//					m_lastFollowX = m_lastFollowY = 0;

					if( m_Unit->isAlive() && m_Unit->IsCreature() )
					{
						MoveTo(SafeCreatureCast(m_Unit)->GetSpawnX(),SafeCreatureCast(m_Unit)->GetSpawnY(),SafeCreatureCast(m_Unit)->GetSpawnZ(),SafeCreatureCast(m_Unit)->GetSpawnO());
//						if( m_Unit->IsCreature() )
						{
							Creature *aiowner = SafeCreatureCast(m_Unit);
							//clear tagger.
//							aiowner->Tagged = false;
							aiowner->TaggerGroupId = 0;
							aiowner->TaggerGuid = 0;
							aiowner->SetUInt32Value(UNIT_DYNAMIC_FLAGS,aiowner->GetUInt32Value(UNIT_DYNAMIC_FLAGS) & ~(U_DYN_FLAG_TAGGED_BY_OTHER |U_DYN_FLAG_LOOTABLE));
							aiowner->m_lootMethod = -1;
						}
					}
					CALL_SCRIPT_EVENT(m_Unit, OnCombatStop)( NULL );
				}

//				if(m_Unit->GetMapMgr() && m_Unit->GetMapMgr()->GetMapInfo() && m_Unit->GetMapMgr()->GetMapInfo()->type == INSTANCE_RAID)
				{
					if( m_Unit->GetMapMgr() && m_Unit->GetTypeId() == TYPEID_UNIT )
					{
//						if(SafeCreatureCast(m_Unit)->GetCreatureInfo() && SafeCreatureCast(m_Unit)->GetCreatureInfo()->Rank == 3)
						{
							  m_Unit->GetMapMgr()->RemoveCombatInProgress(m_Unit->GetGUID());
							  if( pUnit )
								m_Unit->GetMapMgr()->RemoveCombatInProgress( pUnit->GetGUID() );
						}
					}
				}

				// Remount if mounted
				if(m_Unit->GetTypeId() == TYPEID_UNIT)
				{
					if( SafeCreatureCast( m_Unit )->m_spawn )
						m_Unit->SetUInt32Value( UNIT_FIELD_MOUNTDISPLAYID, SafeCreatureCast( m_Unit )->m_spawn->MountedDisplayID );
				}

				//Zack : not sure we need to send this. Did not see it in the dumps since mob died eventually but it seems logical to make this
				m_Unit->smsg_AttackStop( pUnit );
			}break;
		case EVENT_DAMAGETAKEN:
			{
				if( pUnit == NULL ) 
				{ 
					sLog.outDebug("EVENT_DAMAGETAKEN:attacker is NULL.Exiting");
					return;
				}

				if( m_Unit->IsCreature() && SafeCreatureCast( m_Unit )->has_combat_text )
					objmgr.HandleMonsterSayEvent( SafeCreatureCast( m_Unit ), MONSTER_SAY_EVENT_ON_DAMAGE_TAKEN );

				HandleChainAggro(pUnit);
				CALL_SCRIPT_EVENT(m_Unit, OnDamageTaken)(pUnit, float(misc1));
				modThreatByPtr(pUnit, misc1);	//misc1 already contains the precalculated threath based on dmg ...
				//Zack : As secsy this is we will try to keep ourself in combat using the ::strike and ::spellnonmeleedmglog functions to be able to monitor proper events
//				m_Unit->CombatStatus.OnDamageDealt(pUnit);
			}break;
		case EVENT_FOLLOWOWNER:
			{
				m_AIState = STATE_FOLLOWING;
				if(m_Unit->IsPet())
					(SafePetCast(m_Unit))->SetPetAction(PET_ACTION_FOLLOW);
				UnitToFollowGUID = m_PetOwnerGUID;
//				m_lastFollowX = m_lastFollowY = 0;
//				FollowDistance = 4.0f;

				ClearThreathList();	//must be before clear aitargets
				LockAITargets(true);
				m_aiTargets.clear();
				LockAITargets(false);
				m_fleeTimer = 0;
				m_hasFleed = false;
				m_hasCalledForHelp = false;
				m_nextSpell = NULL;
				SetNextTarget( (Unit*)NULL );
				m_moveRun = true;
			}break;

		case EVENT_FEAR:
			{
				if( pUnit == NULL ) 
				{ 
					return;
				}

				SetUnitToFear(pUnit);

				CALL_SCRIPT_EVENT(m_Unit, OnFear)(pUnit, 0);
				m_AIState = STATE_FEAR;
				StopMovement(1);

//				UnitToFollow_backup = UnitToFollowGUID;
//				UnitToFollowGUID = 0;
//				m_lastFollowX = m_lastFollowY = 0;
//				FollowDistance_backup = FollowDistance;
//				FollowDistance = 0.0f;

//				LockAITargets(true);
//				m_aiTargets.clear(); // we'll get a new target after we are unfeared
//				LockAITargets(false);
				m_fleeTimer = 0;
				m_hasFleed = false;
				m_hasCalledForHelp = false;

				// update speed
				m_moveRun = true;
				getMoveFlags();

				SetNextSpell( NULL );
				SetNextTarget( (Unit*)NULL);
			}break;

		case EVENT_UNFEAR:
			{
//				UnitToFollowGUID = UnitToFollow_backup;
//				FollowDistance = FollowDistance_backup;
				m_AIState = STATE_IDLE; // we need this to prevent permanent fear, wander, and other problems

				SetUnitToFear(NULL);
				StopMovement(1);
			}break;

		case EVENT_WANDER:
			{
				if( pUnit == NULL ) 
				{ 
					return;
				}

				//CALL_SCRIPT_EVENT(m_Unit, OnWander)(pUnit, 0); FIXME
				m_wanderStartX = m_Unit->GetPositionX();
				m_wanderStartY = m_Unit->GetPositionY();

				m_AIState = STATE_WANDER;
				StopMovement(1);

//				UnitToFollow_backup = UnitToFollowGUID;
//				UnitToFollowGUID = 0;
//				m_lastFollowX = m_lastFollowY = 0;
//				FollowDistance_backup = FollowDistance;
//				FollowDistance = 0.0f;

//				LockAITargets(true);
//				m_aiTargets.clear(); // we'll get a new target after we are unwandered
//				LockAITargets(false);
				m_fleeTimer = 0;
				m_hasFleed = false;
				m_hasCalledForHelp = false;

				// update speed
				m_moveRun = true;
				getMoveFlags();

				SetNextSpell(NULL);
				SetNextTarget( (Unit*)NULL );
			}break;

		case EVENT_UNWANDER:
			{
//				UnitToFollowGUID = UnitToFollow_backup;
//				FollowDistance = FollowDistance_backup;
				m_AIState = STATE_IDLE; // we need this to prevent permanent fear, wander, and other problems

				StopMovement(1);
			}break;

		default:
			{
			}break;
		}
	}

	//Should be able to do this stuff even when evading
	switch(event)
	{
		case EVENT_UNITDIED:
		{
			if( pUnit == NULL ) 
			{ 
				return;
			}

			if( SafeCreatureCast( m_Unit )->has_combat_text )
				objmgr.HandleMonsterSayEvent( SafeCreatureCast( m_Unit ), MONSTER_SAY_EVENT_ON_DIED );

			CALL_SCRIPT_EVENT(m_Unit, OnDied)(pUnit);

			if ( m_Unit->IsCreature() )
				CALL_INSTANCE_SCRIPT_EVENT( m_Unit->GetMapMgr(), OnCreatureDeath )( SafeCreatureCast(m_Unit) , pUnit );

			m_AIState = STATE_IDLE;

			StopMovement(0);
			ClearThreathList();	//must be before clear aitargets
			LockAITargets(true);
			m_aiTargets.clear();
			LockAITargets(false);
			UnitToFollowGUID = 0;
//			m_lastFollowX = m_lastFollowY = 0;
			UnitToFear = NULL;
//			FollowDistance = 0.0f;
			m_fleeTimer = 0;
			m_hasFleed = false;
			m_hasCalledForHelp = false;
			m_nextSpell = NULL;

			SetNextTarget( (Unit*)NULL );
			//reset ProcCount
			ResetProcCounts();
		
			//reset waypoint to 0
			m_currentWaypoint = 0;
			
			//loose the mount for sure
			m_Unit->SetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID, 0);

			// There isn't any need to do any attacker checks here, as
			// they should all be taken care of in DealDamage

			//removed by Zack : why do we need to go to our master if we just died ? On next spawn we will be spawned near him after all
/*			if(m_AIType == AITYPE_PET)
			{
				SetUnitToFollow(m_PetOwner);
				SetFollowDistance(3.0f);
				HandleEvent(EVENT_FOLLOWOWNER, m_Unit, 0);
			}*/

			Instance *pInstance = NULL;
			if(m_Unit->GetMapMgr())
				pInstance = m_Unit->GetMapMgr()->pInstance;
			if(m_Unit->GetMapMgr() && m_Unit->GetTypeId() == TYPEID_UNIT && !m_Unit->IsPet() && pInstance 
//				&& (pInstance->m_mapInfo->type == INSTANCE_RAID || pInstance->m_mapInfo->type == INSTANCE_DUNGEON || pInstance->m_mapInfo->type == INSTANCE_MULTIMODE))
				&& IS_MAP_INSTANCE( pInstance->m_mapInfo ) )
			{
				InstanceBossInfoMap *bossInfoMap = objmgr.m_InstanceBossInfoMap[m_Unit->GetMapMgr()->GetMapId()];
				Creature *pCreature = SafeCreatureCast( m_Unit );
				bool found = false;

				if(IS_PERSISTENT_INSTANCE(pInstance) && bossInfoMap != NULL && pCreature->GetProto())
				{
					uint32 npcGuid = pCreature->GetProto()->Id;
					InstanceBossInfoMap::const_iterator bossInfo = bossInfoMap->find(npcGuid);
					if(bossInfo != bossInfoMap->end())
					{
						found = true;
						m_Unit->GetMapMgr()->pInstance->m_killedNpcs.insert( npcGuid );
						m_Unit->GetMapMgr()->pInstance->SaveToDB();
						for(InstanceBossTrashList::iterator trash = bossInfo->second->trash.begin(); trash != bossInfo->second->trash.end(); ++trash)
						{
							Creature *c = m_Unit->GetMapMgr()->GetSqlIdCreature((*trash));
							if(c != NULL)
								c->m_noRespawn = true;	//delete without adding to mapmanager respawn list
						}
						if(!pInstance->m_persistent)
						{
							pInstance->m_persistent = true;
							pInstance->SaveToDB();
							for(PlayerStorageMap::iterator itr = m_Unit->GetMapMgr()->m_PlayerStorage.begin(); itr != m_Unit->GetMapMgr()->m_PlayerStorage.end(); ++itr)
							{
								(*itr).second->SetPersistentInstanceId(pInstance);
							}
						}
					}
				}

				if (found == false)
				{
					// No instance boss information ... so fallback ...
					uint32 npcGuid = pCreature->GetSQL_id();
					m_Unit->GetMapMgr()->pInstance->m_killedNpcs.insert( npcGuid );
					m_Unit->GetMapMgr()->pInstance->SaveToDB();
				}
			}
			if(m_Unit->GetMapMgr() && m_Unit->GetMapMgr()->GetMapInfo() 
//				&& m_Unit->GetMapMgr()->GetMapInfo()->type == INSTANCE_RAID
				&& IS_MAP_RAID( m_Unit->GetMapMgr()->GetMapInfo() )
				)
			{
				if(m_Unit->GetTypeId() == TYPEID_UNIT)
				{
					if(SafeCreatureCast(m_Unit)->GetCreatureInfo() && SafeCreatureCast(m_Unit)->GetCreatureInfo()->Rank == 3)
					{
						m_Unit->GetMapMgr()->RemoveCombatInProgress(m_Unit->GetGUID());
					}
				}
			}


			//remove negative auras
			//if( m_Unit->IsCreature() )
			//	m_Unit->RemoveNegativeAuras();

		}break;
	}
}

void AIInterface::Update(uint32 p_time)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	float tdist;
	if(m_AIType == AITYPE_TOTEM)
	{
		if( spell_global_cooldown_stamp <= getMSTime() && totemspell )
		{
			for(int i=0;i<MAX_NUMBER_OF_SPELLS_FOR_TOTEM;i++)
				if( totemspell->spells[i] )
			{
				//no need to spam cast passive auras !
				if( ( totemspell->spells[i]->eff[0].Effect == SPELL_EFFECT_APPLY_AREA_AURA || totemspell->spells[i]->eff[0].Effect == SPELL_EFFECT_APPLY_AREA_AURA2
					|| totemspell->spells[i]->eff[1].Effect == SPELL_EFFECT_APPLY_AREA_AURA || totemspell->spells[i]->eff[1].Effect == SPELL_EFFECT_APPLY_AREA_AURA2
					|| totemspell->spells[i]->eff[2].Effect == SPELL_EFFECT_APPLY_AREA_AURA || totemspell->spells[i]->eff[2].Effect == SPELL_EFFECT_APPLY_AREA_AURA2 ) 
					&& m_Unit->HasAura( totemspell->spells[i]->Id ) )
					continue;

				Spell *pSpell = SpellPool.PooledNew( __FILE__, __LINE__ );
//				pSpell->Init(m_Unit, totemspell->spells[i], true, 0);
				pSpell->Init(m_Unit, totemspell->spells[i], false, 0);	//someone reported that we should see cast bar

				SpellCastTargets targets(0);
				Unit *CurTarget = GetNextTarget();
				if( !CurTarget ||
					!CurTarget->isAlive() ||
					!IsInrange(m_Unit,CurTarget,pSpell->GetProto()->base_range_or_radius_sqr) ||
					!isAttackable(m_Unit, CurTarget,!(pSpell->GetProto()->c_is_flags & SPELL_FLAG_IS_TARGETINGSTEALTHED)) ||
					!( sWorld.Collision && CollideInterface.CheckLOS( m_Unit->GetMapId(), m_Unit->GetPositionNC(), CurTarget->GetPositionNC() ) )
					)
				{
					//we set no target and see if we managed to fid a new one
					SetNextTarget( (Unit*)NULL );
					//something happend to our target, pick another one
					pSpell->GenerateTargets(&targets);
					if(targets.m_targetMask & TARGET_FLAG_UNIT)
						SetNextTarget( targets.m_unitTarget );
				}
				if(GetNextTarget())
				{
					SpellCastTargets targets(GetNextTarget()->GetGUID());
					pSpell->prepare(&targets);
					// need proper cooldown time!
					spell_global_cooldown_stamp = getMSTime() + m_totemspelltime;
				}
				else 
				{
					spell_global_cooldown_stamp = getMSTime() + MOB_SPELLCAST_GLOBAL_COOLDOWN;
					SpellPool.PooledDelete( pSpell, __FILE__, __LINE__ );
				}
				// these will *almost always* be AoE, so no need to find a target here.
	//			SpellCastTargets targets(m_Unit->GetGUID());
	//			Spell * pSpell = new Spell(m_Unit, totemspell, true, 0);
	//			pSpell->prepare(&targets);
				// need proper cooldown time!
	//			m_totemspelltimer = m_totemspelltime;
			}
		}
		return;
	}

	_UpdateTimer(p_time);
	if( m_Unit->isAlive() && m_AIState != STATE_FEAR && m_AIState != STATE_WANDER )
		_UpdateTargets();
	//move before combat so we may be able to reach our target without him escaping us
	_UpdateMovement(p_time);
	if(m_Unit->isAlive() && m_AIState != STATE_IDLE 
		&& m_AIState != STATE_FOLLOWING && m_AIState != STATE_FEAR 
		&& m_AIState != STATE_WANDER && m_AIState != STATE_SCRIPTMOVE)
	{
		if(m_AIType == AITYPE_PET )
		{
			if(!m_Unit->bInvincible && m_Unit->IsPet()) 
			{
				Pet * pPet = SafePetCast(m_Unit);
	
				if(pPet->GetPetAction() == PET_ACTION_ATTACK || pPet->GetPetState() != PET_STATE_PASSIVE )
				{
					//check if our target is still attackable. This happens on duel end
					if( GetNextTarget() == NULL 
						|| isAttackable( pPet, GetNextTarget() ) == false 
						|| GetNextTarget()->IsCrowdControlledNoAttack() //stop attacking
						)
					{
//						RemoveThreatByPtr( GetNextTarget() );
//						SetNextTarget( GetMostHated() );
						HandleEvent( EVENT_LEAVECOMBAT, m_Unit, 0);
						pPet->SetPetAction( PET_ACTION_FOLLOW );
					}
					_UpdateCombat(p_time);
				}
			}
			//we just use any creature as a pet guardian
			else if(!m_Unit->IsPet())
			{
				_UpdateCombat(p_time);
			}
		}
		else
		{
			_UpdateCombat(p_time);
		}
	}
	_UpdateThreathList();

	if(m_AIState==STATE_EVADE)
	{
		if( m_Unit->IsCreature() )
			tdist = m_Unit->GetDistanceSq( SafeCreatureCast(m_Unit)->GetSpawnX(), SafeCreatureCast(m_Unit)->GetSpawnY(), SafeCreatureCast(m_Unit)->GetSpawnZ() );
		else
			tdist = 0.0f;
		if(tdist <= 4.0f/*2.0*/)
		{
			m_AIState = STATE_IDLE;
			m_moveRun = false;
			// Set health to full if they at there last location before attacking
			if( m_AIType != AITYPE_PET && !skip_reset_hp && m_Unit->IsPlayer() == false )
				m_Unit->SetUInt32Value(UNIT_FIELD_HEALTH,m_Unit->GetUInt32Value(UNIT_FIELD_MAXHEALTH));
		}
		else
		{
			if( m_creatureState == STOPPED && m_Unit->IsCreature() )
				MoveTo(SafeCreatureCast(m_Unit)->GetSpawnX(), SafeCreatureCast(m_Unit)->GetSpawnY(), SafeCreatureCast(m_Unit)->GetSpawnZ(), SafeCreatureCast(m_Unit)->GetSpawnO());
		}
	}

	if(m_fleeTimer)
	{
		if(m_fleeTimer > p_time)
		{
			m_fleeTimer -= p_time;
			_CalcDestinationAndMove(GetNextTarget(), 5.0f);
		}
		else
		{
			m_fleeTimer = 0;
			SetNextTarget( GetMostHated() );
		}
	}

	//Pet Dismiss after a certian ditance away
	/*if(m_AIType == AITYPE_PET && m_PetOwner != NULL)
	{
		float dist = (m_Unit->GetInstanceID() == m_PetOwner->GetInstanceID()) ? 
			m_Unit->GetDistanceSq(m_PetOwner) : 99999.0f;

		if(dist > 8100.0f) //90 yard away we Dismissed
		{
			DismissPet();
			return;
		}
	}*/

	if ( !GetNextTarget() && !m_fleeTimer && m_creatureState == STOPPED && m_AIState == STATE_IDLE && m_Unit->isAlive() )
	{
		if ( timed_emote_expire <= p_time ) // note that creature might go idle and p_time might get big next time ...We do not skip emotes because of lost time
		{
			if ( (*next_timed_emote)->type == 1) //standstate
			{
				m_Unit->SetStandState( (*next_timed_emote)->value );
				m_Unit->SetUInt32Value ( UNIT_NPC_EMOTESTATE, 0 );
			}
			else if ( (*next_timed_emote)->type == 2) //emotestate
			{
				m_Unit->SetUInt32Value ( UNIT_NPC_EMOTESTATE, (*next_timed_emote)->value );
				m_Unit->SetStandState( 0 );
			}
			else if ( (*next_timed_emote)->type == 3) //oneshot emote
			{
				m_Unit->SetUInt32Value ( UNIT_NPC_EMOTESTATE, 0 );
				m_Unit->SetStandState( 0 );
				m_Unit->Emote( (EmoteType)(*next_timed_emote)->value );         // Animation
			}
			if ( (*next_timed_emote)->msg )
				m_Unit->SendChatMessage((*next_timed_emote)->msg_type, (*next_timed_emote)->msg_lang, (*next_timed_emote)->msg);

			timed_emote_expire = (*next_timed_emote)->expire_after; //should we keep lost time ? I think not 
			next_timed_emote++;
			if ( next_timed_emote == timed_emotes->end() )
				next_timed_emote = timed_emotes->begin();
		}
		else 
			timed_emote_expire -= p_time;
	}
}

void AIInterface::_UpdateTimer(uint32 p_time)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
/*	if(m_updateAssistTimer > p_time)
	{
		m_updateAssistTimer -= p_time;
	}else
	{
		m_updateAssist = true;
		m_updateAssistTimer = TARGET_UPDATE_INTERVAL_ON_PLAYER * 2 - m_updateAssistTimer - p_time;
	}*/

/*	if(m_updateTargetsTimer > p_time)
	{
		m_updateTargetsTimer -= p_time;
	}else
	{
		m_updateTargets = true;
		m_updateTargetsTimer = TARGET_UPDATE_INTERVAL_ON_PLAYER * 2 - m_updateTargetsTimer - p_time;
	}
	*/
}

void AIInterface::_UpdateTargets()
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
#ifdef _DEBUG
	uint32 Entry = m_Unit->GetEntry();
#endif
	if( m_Unit->IsPlayer() || (m_AIType != AITYPE_PET && disable_targeting ))
	{ 
		return;
	}
	if( ( SafeCreatureCast(m_Unit) )->GetCreatureInfo() && ( SafeCreatureCast(m_Unit) )->GetCreatureInfo()->Type == CRITTER )
	{ 
		return;
	}

	if(  m_Unit->GetMapMgr() == NULL )
	{ 
		return; 
	}

	AssistTargetSet::iterator i, i2;
	TargetMap::iterator itr, it2;

	// Find new Assist Targets and remove old ones
	if(m_AIState == STATE_FLEEING)
	{
		FindFriends(100.0f/*10.0*/,true);
//		FindFriends(25.0f );
	}
	else if(m_AIState != STATE_IDLE && m_AIState != STATE_SCRIPTIDLE)
	{
//		FindFriends(100.0f/*10.0f*/,false);
		FindFriends(25.0f,false );
	}

/*	if( m_updateAssist )
	{
		m_updateAssist = false;
	/*	deque<Unit*> tokill;

		//modified for vs2005 compatibility
		for(i = m_assistTargets.begin(); i != m_assistTargets.end(); ++i)
		{
			if(m_Unit->GetDistanceSq((*i)) > 2500.0f|| !(*i)->isAlive() || !(*i)->CombatStatus.IsInCombat())
			{
				tokill.push_back(*i);
			}
		}

		for(deque<Unit*>::iterator i2 = tokill.begin(); i2 != tokill.end(); ++i2)
			m_assistTargets.erase(*i2);*/

/*		MapMgr *pmgr = m_Unit->GetMapMgr();
		if( pmgr )
			for(i = m_assistTargets.begin(); i != m_assistTargets.end();)
			{
				i2 = i++;
				Unit *pu = pmgr->GetUnit( *i2 );
				if(pu == NULL || pu->event_GetInstanceID() != m_Unit->event_GetInstanceID() ||
					!pu->isAlive() || m_Unit->GetDistanceSq(pu) >= 2500.0f || !pu->CombatStatus.IsInCombat()
					|| !((pu)->m_phase & m_Unit->m_phase) )
				{
					m_assistTargets.erase( i2 );
				}
			}
	}*/

	if( m_updateTargetsTimer <= getMSTime() )
	{
		m_updateTargetsTimer = getMSTime() + TARGET_UPDATE_INTERVAL_ON_PLAYER;
//		m_updateTargets = false;
		/*deque<Unit*> tokill;

		//modified for vs2005 compatibility
		for(itr = m_aiTargets.begin(); itr != m_aiTargets.end();++itr)
		{
			if(!itr->first->isAlive() || m_Unit->GetDistanceSq(itr->first) >= 6400.0f)
			{
				tokill.push_back(itr->first);
			}
		}
		for(deque<Unit*>::iterator itr = tokill.begin(); itr != tokill.end(); ++itr)
			m_aiTargets.erase((*itr));
		tokill.clear();*/

		LockAITargets(true);

		for(itr = m_aiTargets.begin(); itr != m_aiTargets.end();)
		{
			it2 = itr++;

			Unit *ai_t = m_Unit->GetMapMgr()->GetUnit( it2->first );
			if (ai_t == NULL) 
			{
				m_aiTargets.erase( it2 );
			} 
			else 
			{
				bool instance = false;
				if (m_Unit->GetMapMgr() && m_Unit->GetMapMgr()->GetMapInfo())
					instance = IS_MAP_INSTANCE( m_Unit->GetMapMgr()->GetMapInfo() );

				if( ai_t->event_GetInstanceID() != m_Unit->event_GetInstanceID() 
					|| !ai_t->isAlive() 
					|| (!instance && m_Unit->GetDistanceSq(ai_t) >= 80.0f*80.0f)
					|| !((ai_t)->m_phase & m_Unit->m_phase)
					|| isAttackable( m_Unit, ai_t ) == false
					|| ( m_Unit->IsPet() && ai_t->IsCrowdControlledNoAttack() ) //stop attacking
					) 
				{
					m_aiTargets.erase( it2 );
					ai_t->CombatStatus.BreakCombatLink( m_Unit );
				}
			}
		}

		LockAITargets(false);
		
		if(m_aiTargets.empty() == true 
			&& m_AIState != STATE_IDLE && m_AIState != STATE_FOLLOWING 
			&& m_AIState != STATE_EVADE && m_AIState != STATE_FEAR 
			&& m_AIState != STATE_WANDER && m_AIState != STATE_SCRIPTIDLE)
		{
			if (m_Unit->GetMapMgr() && m_Unit->GetMapMgr()->GetMapInfo())
			{
				Unit* target = NULL;
				switch (m_Unit->GetMapMgr()->GetMapInfo()->type_flags)
				{
				case INSTANCE_FLAG_STATIC_MAP:
				case INSTANCE_FLAG_PVP:
					if (m_outOfCombatRange && _CalcDistanceFromHome() < m_outOfCombatRange)
						target = FindTarget();
					break;
				default:
					target = FindTarget();
					break;
				}

				if(target != NULL)
					AttackReaction(target, 1, 0);
			}
		}
		else if( m_aiTargets.empty() == true && (m_AIType == AITYPE_PET && (m_Unit->IsPet() && SafePetCast(m_Unit)->GetPetState() == PET_STATE_AGGRESSIVE) || (!m_Unit->IsPet() && disable_melee == false ) ) )
		{
			 Unit* target = FindTarget();
			 if( target )
			 {
				 AttackReaction(target, 1, 0);
			 }

		}
	}
	// Find new Targets when we are ooc
	if((m_AIState == STATE_IDLE || m_AIState == STATE_SCRIPTIDLE) 
//		&& m_assistTargets.size() == 0
		)
	{
		Unit* target = FindTarget();
		if(target)
		{
			AttackReaction(target, 1, 0);
		}
	}
}

///====================================================================
///  Desc: Updates Combat Status of m_Unit
///====================================================================
void AIInterface::_UpdateCombat(uint32 p_time)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if( m_AIType != AITYPE_PET && disable_combat == true )
	{ 
		return;
	}
	//do not attack when we are not supposed to be able to attack
	if( m_Unit->IsCrowdControlledNoAttack() )
	{
		return;
	}

	//no combat needed if we are already casting a spell
	if( m_Unit->isCasting() )
	{
		return;
	}

	Unit *local_target = GetNextTarget();	// !!! you should repeat this step every time you call an external function. You can never know what will despawn our target

	//just make sure we are not hitting self. This was reported as an exploit.Should never ocure anyway
	if( local_target == m_Unit )
	{
		local_target = GetMostHated();
		SetNextTarget( local_target );
	}

	uint16 agent = m_aiCurrentAgent;

	// If creature is very far from spawn point return to spawnpoint
	// If at instance dont return -- this is wrong ... instance creatures always returns to spawnpoint, dunno how do you got this ideia. 
	// If at instance returns to spawnpoint after empty agrolist
	if(	m_AIType != AITYPE_PET 
		&& m_AIState != STATE_EVADE
		&& m_AIState != STATE_SCRIPTMOVE
		&& !IS_IN_INSTANCE( this )
//		&& (!m_is_in_instance || m_Unit->GetDistanceSq(m_returnX,m_returnY,m_returnZ) > 100*100 ) 
		&& (m_outOfCombatRange && 
		( ( m_Unit->IsCreature() && m_Unit->GetDistanceSq( SafeCreatureCast(m_Unit)->GetSpawnX(), SafeCreatureCast(m_Unit)->GetSpawnY(), SafeCreatureCast(m_Unit)->GetSpawnZ() ) > m_outOfCombatRange )
				|| ( local_target && m_Unit->GetDistanceSq( local_target ) > m_outOfCombatRange ) ) ) )
	{
		HandleEvent( EVENT_LEAVECOMBAT, m_Unit, 0 );
	}
	else if( (local_target == NULL && m_AIState != STATE_FOLLOWING && m_AIState != STATE_SCRIPTMOVE )
		|| ( local_target != NULL && !(local_target->m_phase & m_Unit->m_phase) ) ) // the target or we changed phase, stop attacking
	{
		if( IS_IN_INSTANCE( this ) )
			local_target = FindTarget();
		else 
			local_target = GetMostHated() ;
		SetNextTarget( local_target );
		if( local_target == NULL )
			HandleEvent( EVENT_LEAVECOMBAT, m_Unit, 0 );
	}

	if( local_target != NULL && local_target->IsCrowdControlledNoAttack() )
		local_target = FindTarget();

#ifdef HACKY_SERVER_CLIENT_POS_SYNC
	if( moved_for_attack && GetNextTarget() && m_creatureState != MOVING )
	{
		//make sure we reached the exact desired location. 
		// due to combat updates creature might interrupt moving and start attacking and does not get to destination making us get out of range errors
		if(	m_destinationX )
			m_Unit->SetPosition(m_destinationX,m_destinationY,m_destinationZ, 0 );
		//send a forced update position to client
		StopMovement(0); 
		//no need to update position until mob moves to nev target
		moved_for_attack = false;
	}
#endif

/*	if (sWorld.Collision) 
	{
		float target_land_z=0.0f;
		if ( m_Unit->GetMapMgr() != NULL && GetNextTarget() != NULL )
		{
			if (!m_moveFly)
			{
				target_land_z = CollideInterface.GetHeight(m_Unit->GetMapId(), GetNextTarget()->GetPositionX(), GetNextTarget()->GetPositionY(), GetNextTarget()->GetPositionZ() + 2.0f);
				if ( target_land_z == NO_WMO_HEIGHT )
					target_land_z = m_Unit->GetMapMgr()->GetLandHeight(GetNextTarget()->GetPositionX(), GetNextTarget()->GetPositionY());

				if (fabs(GetNextTarget()->GetPositionZ() - target_land_z) > _CalcCombatRange(GetNextTarget(), false))
				{
					if ( GetNextTarget()->GetTypeId() != TYPEID_PLAYER )
					{
						if ( target_land_z > m_Unit->GetMapMgr()->GetWaterHeight(GetNextTarget()->GetPositionX(), GetNextTarget()->GetPositionY()) )
							HandleEvent( EVENT_LEAVECOMBAT, m_Unit, 0); //bugged npcs, probly db fault
					}
					else if (SafePlayerCast(GetNextTarget())->GetSession() != NULL)
					{
						MovementInfo* mi=SafePlayerCast(GetNextTarget())->GetSession()->GetMovementInfo();

						if ( mi != NULL && !(mi->flags & MOVEFLAG_FALLING) && !(mi->flags & MOVEFLAG_SWIMMING) && !(mi->flags & MOVEFLAG_LEVITATE))
							HandleEvent( EVENT_LEAVECOMBAT, m_Unit, 0);
					}
				}
			}
		}
	}*/

	if ( local_target != NULL && local_target->GetTypeId() == TYPEID_UNIT && m_AIState == STATE_EVADE )
	{
		HandleEvent( EVENT_LEAVECOMBAT, m_Unit, 0);
		local_target = GetNextTarget();
	}
#ifdef HACKY_CRASH_FIXES
	bool cansee = (local_target != NULL) ? CheckCurrentTarget() : NULL;
#else
	bool cansee;
	if(local_target && local_target->event_GetCurrentInstanceId() == m_Unit->event_GetCurrentInstanceId())
	{
		if( m_Unit->GetTypeId() == TYPEID_UNIT )
			cansee = SafeCreatureCast( m_Unit )->CanSee( local_target );
		else
			cansee = SafePlayerCast( m_Unit )->CanSee( local_target );
	}
	else 
	{
		if( local_target )
			SetNextTarget( (Unit*)NULL );			// corupt pointer

		cansee = false;
	}
#endif
	local_target = GetNextTarget();
	if( cansee && local_target && local_target->isAlive() && m_AIState != STATE_EVADE && !m_Unit->isCasting() )
	{
		if( agent == AGENT_NULL || ( m_AIType == AITYPE_PET && !m_nextSpell ) ) // allow pets autocast
		{
			if( !m_nextSpell )
				m_nextSpell = getSpell();

			if(m_canFlee && !m_hasFleed 
				&& ((m_Unit->GetUInt32Value(UNIT_FIELD_HEALTH) / m_Unit->GetUInt32Value(UNIT_FIELD_MAXHEALTH)) < m_FleeHealth ))
				agent = AGENT_FLEE;
			else if(m_canCallForHelp 
				&& !m_hasCalledForHelp 
				//&& (m_CallForHelpHealth > (m_Unit->GetUInt32Value(UNIT_FIELD_HEALTH) / (m_Unit->GetUInt32Value(UNIT_FIELD_MAXHEALTH) > 0 ? m_Unit->GetUInt32Value(UNIT_FIELD_MAXHEALTH) : 1)))
				)
				agent = AGENT_CALLFORHELP;
			else if(m_nextSpell)
			{
				if(m_nextSpell->agent != AGENT_NULL)
					agent = m_nextSpell->agent;
				else
					agent = AGENT_MELEE;
			}
			else
			{
				agent = AGENT_MELEE;
			}
		}
		if(agent == AGENT_RANGED || agent == AGENT_MELEE)
		{
			if(m_canRangedAttack)
			{
				agent = AGENT_MELEE;
				if(local_target->GetTypeId() == TYPEID_PLAYER)
				{
					float dist = m_Unit->GetDistanceSq(GetNextTarget());
					if( SafePlayerCast( local_target )->m_currentMovement == MOVE_ROOT || dist >= 64.0f )
						agent =  AGENT_RANGED;
				}
				else if( local_target->m_canMove == false || m_Unit->GetDistanceSq( local_target ) >= 64.0f )
				   agent = AGENT_RANGED;
			}
			else
			{
				agent = AGENT_MELEE;
			}
		}

		if(this->disable_melee && agent == AGENT_MELEE)
			agent = AGENT_NULL;
		else if(this->disable_ranged && agent == AGENT_RANGED)
			agent = AGENT_NULL;
		else if(this->disable_spell && agent == AGENT_SPELL)
			agent = AGENT_NULL;

		switch(agent)
		{
		case AGENT_MELEE:
			{
				m_Unit->SetWeaponSheath( WEAPON_SHEATH_MELEE );//draw weapon if it is put away
				float combatReach[2]; // Calculate Combat Reach
				float distance = m_Unit->CalcDistance( local_target );

				combatReach[0] = MAX( local_target->GetModelHalfSize(), DISTANCE_TO_SMALL_TO_WALK );
				combatReach[1] = MAX( _CalcCombatRange( local_target , false), DISTANCE_TO_SMALL_TO_WALK );

				if(	(
//					distance >= combatReach[0] &&  //removed by Zack. You can create an exploit with this that creature will never attack
					distance <= combatReach[1] + DISTANCE_TO_SMALL_TO_WALK ) 
//					|| gracefull_hit_on_target == GetNextTarget() 
					) // Target is in Range -> Attack
				{
//					gracefull_hit_on_target = NULL;
					if( UnitToFollowGUID != 0 )
					{
						UnitToFollowGUID = 0; //we shouldn't be following any one
//						m_lastFollowX = m_lastFollowY = 0;
						//m_Unit->setAttackTarget(NULL);  // remove ourselves from any target that might have been followed
					}
					
//					FollowDistance = 0.0f;
//					m_moveRun = false;
					//FIXME: offhand shit
					if(m_Unit->isAttackReady(false) && !m_fleeTimer)
					{
						m_creatureState = ATTACKING;
						bool infront = m_Unit->isInFront( local_target );

						if(!infront) // set InFront
						{
							//prevent mob from rotating while stunned
							if(!m_Unit->IsStunned ())
							{
								setInFront( local_target );
								infront = true;
							}							
						}
						if(infront)
						{
							m_Unit->setAttackTimer(0, false);
#ifdef ENABLE_CREATURE_DAZE
							//we require to know if strike was succesfull. If there was no dmg then target cannot be dazed by it
							Unit *t_unit = GetNextTarget();
							if( !t_unit )
								return; //omg lol, in seconds we lost target. This might be possible due to the Eventrelocated
							uint32 health_before_strike = t_unit->GetUInt32Value(UNIT_FIELD_HEALTH);
#endif
							m_Unit->Strike( local_target, ( agent == AGENT_MELEE ? MELEE : RANGED ), NULL, 0, 0, 0, false, false );

							DamageDoneLastMeleeStrike = health_before_strike - t_unit->GetUInt32Value(UNIT_FIELD_HEALTH);

							local_target = GetNextTarget();
#ifdef ENABLE_CREATURE_DAZE
							//now if the target is facing his back to us then we could just cast dazed on him :P
							//as far as i know dazed is casted by most of the creatures but feel free to remove this code if you think otherwise
							if(local_target && m_Unit->m_factionDBC &&
								!(m_Unit->m_factionDBC->RepListId == -1 && m_Unit->m_faction->FriendlyMask==0 && m_Unit->m_faction->HostileMask==0) /* neutral creature */
								&& local_target->IsPlayer() && health_before_strike>local_target->GetUInt32Value(UNIT_FIELD_HEALTH)
								&& RandChance(m_Unit->get_chance_to_daze( local_target ))
								&& m_Unit->GetTopOwner() == m_Unit )
							{
								float our_facing=m_Unit->calcRadAngle(m_Unit->GetPositionX(),m_Unit->GetPositionY(),local_target->GetPositionX(),local_target->GetPositionY());
								float his_facing=local_target->GetOrientation();
								if(fabs(our_facing-his_facing)<CREATURE_DAZE_TRIGGER_ANGLE && !local_target->HasAura(CREATURE_SPELL_TO_DAZE, 0, AURA_SEARCH_NEGATIVE))
								{
									SpellEntry *info = dbcSpell.LookupEntry(CREATURE_SPELL_TO_DAZE);
									Spell *sp = SpellPool.PooledNew( __FILE__, __LINE__ );
									sp->Init(m_Unit, info, false, NULL);
									SpellCastTargets targets;
									targets.m_unitTarget = GetNextTarget()->GetGUID();
									sp->prepare(&targets);
								}
								local_target = GetNextTarget();
							}
#endif
						}
					}
				}
				// Target out of Range -> Run to it if he is not too far from our cusy litle shithole where we spawned
//				if( local_target )
				else if( IS_IN_INSTANCE( this ) == true || ( m_Unit->IsCreature() && m_Unit->GetDistanceSq( SafeCreatureCast(m_Unit)->GetSpawnX(), SafeCreatureCast(m_Unit)->GetSpawnY(), SafeCreatureCast(m_Unit)->GetSpawnZ() ) < m_outOfCombatRange ) )
//				else 
				{
					//calculate next move
//					float dist = combatReach[1]; //this is theoretically right but annoying formula in game
//					float dist = combatReach[1] - m_Unit->GetFloatValue( UNIT_FIELD_COMBATREACH ); //ignore our combat reach, make sure target (player) can reach us first.

					//practical tests show that we really should try to jump on target to get good results :S
					//simply ignore combat reach and move as close as visually not annoying 
					float dist;
					if( m_Unit->GetModelHalfSize() > local_target->GetModelHalfSize() )
						dist = m_Unit->GetModelHalfSize(); 
					else 
						dist = local_target->GetModelHalfSize();
					//removed by Zack. You can create an exploit with this that creature will never attack
//					if (distance<combatReach[0]) //if we are inside one each other
//						dist = -(combatReach[1] - distance);
//					gracefull_hit_on_target = GetNextTarget(); // this is an exploit where you manage to move the exact speed that mob will reposition itself all the time

					m_moveRun = true;
					_CalcDestinationAndMove( local_target, dist);
					local_target = GetNextTarget();
				}
				//let's forget about this target kk ?
				else
				{
					HandleEvent( EVENT_LEAVECOMBAT, m_Unit, 0);
					local_target = NULL;
				}
			}break;
		case AGENT_RANGED:
			{
				m_Unit->SetWeaponSheath( WEAPON_SHEATH_RANGED );
				float combatReach[2]; // Calculate Combat Reach
				float distance = m_Unit->CalcDistance( local_target );

				combatReach[0] = 8.0f;
				combatReach[1] = 30.0f;

				if(distance >= combatReach[0] && distance <= combatReach[1]) // Target is in Range -> Attack
				{
					if(UnitToFollowGUID != 0)
					{
						UnitToFollowGUID = NULL; //we shouldn't be following any one
//						m_lastFollowX = m_lastFollowY = 0;
						//m_Unit->setAttackTarget(NULL);  // remove ourselves from any target that might have been followed
					}
					
//					FollowDistance = 0.0f;
//					m_moveRun = false;
					//FIXME: offhand shit
					if(m_Unit->isAttackReady(false) && !m_fleeTimer)
					{
						m_creatureState = ATTACKING;
						bool infront = m_Unit->isInFront( local_target );

						if(!infront) // set InFront
						{
							//prevent mob from rotating while stunned
							if(!m_Unit->IsStunned ())
							{
								setInFront( local_target );
								infront = true;
							}							
						}

						if(infront)
						{
							m_Unit->setAttackTimer(0, false);
							SpellEntry *info = dbcSpell.LookupEntry(SPELL_RANGED_GENERAL);
							if(info)
							{
								Spell *sp = SpellPool.PooledNew( __FILE__, __LINE__ );
								sp->Init(m_Unit, info, false, NULL);
								SpellCastTargets targets;
								targets.m_unitTarget = local_target->GetGUID();
								sp->prepare(&targets);
								//Lets make spell handle this
								//m_Unit->Strike( GetNextTarget(), ( agent == AGENT_MELEE ? MELEE : RANGED ), NULL, 0, 0, 0 );
								local_target = GetNextTarget();
							}
						}
					}
				}
				else // Target out of Range -> Run to it
				{
					//calculate next move
					float dist;

					if(distance < combatReach[0])// Target is too near
						dist = 9.0f;
					else
						dist = 20.0f;

					m_moveRun = true;
					_CalcDestinationAndMove( local_target, dist);
					local_target = GetNextTarget();
				}
			}break;
		case AGENT_SPELL:
			{
				if( !m_nextSpell || m_nextSpell->spell == NULL )
					return;  // this shouldnt happen

				Unit *deafult_target = NULL;
				if( m_forced_target )
				{
					deafult_target = m_Unit->GetMapMgr()->GetUnit( m_forced_target );
					m_forced_target = 0;
				}
				if( deafult_target == NULL )
					deafult_target = FindTargetForSpellTargetType( m_nextSpell );
				if( deafult_target == NULL )
				{
//					m_nextSpell->cooldowntime = getMSTime() + m_nextSpell->initial_cooldown;
					m_nextSpell = NULL;
					return; //no, we can't cast this spell yet
				}

				m_Unit->SetWeaponSheath( WEAPON_SHEATH_NONE );

				SpellCastTime *sd = dbcSpellCastTime.LookupEntry( m_nextSpell->spell->CastingTimeIndex );

				float distance = m_Unit->CalcDistance( local_target );
				bool los = true;

/*				if (sWorld.Collision)
					los = CollideInterface.CheckLOS( m_Unit->GetMapId(), m_Unit->GetPositionNC(), local_target->GetPositionNC() ); */

				if(los 
					&& ( ( distance <= m_nextSpell->maxrange + m_Unit->GetModelHalfSize() 
//					&& distance >= m_nextSpell->minrange 
					) 
							|| m_nextSpell->maxrange == 0) ) // Target is in Range -> Attack
				{

					// we are actually casting a spell so set cooldown and stuff
					// set cooldown
					if( m_Unit->IsPet() == FALSE )
					{
						if( m_Unit->GetMapMgr() && IS_HEROIC_INSTANCE_DIFFICULTIE( m_Unit->GetMapMgr()->instance_difficulty ) )
							m_nextSpell->cooldowntime = (uint32)( getMSTime() + m_nextSpell->cooldown * sWorld.mob_HeroicSpellCooldownPCT );
						else
							m_nextSpell->cooldowntime = getMSTime() + m_nextSpell->cooldown;
					}

					// set proccount
					m_nextSpell->procCounter++;

					//global coolwon
					uint32 spell_global_cooldown_stamp;
					//let's not spam spells one after another exactly at 2 sec interval. Kinda lame no ?
					if( m_nextSpell->global_cooldown == 2000 )
						spell_global_cooldown_stamp = MAX( MOB_SPELLCAST_GLOBAL_COOLDOWN, m_nextSpell->global_cooldown + RandomUInt( 2 * m_nextSpell->global_cooldown ) );
					else
						spell_global_cooldown_stamp = MAX( MOB_SPELLCAST_GLOBAL_COOLDOWN, m_nextSpell->global_cooldown );
					spell_global_cooldown_stamp = getMSTime() + spell_global_cooldown_stamp;

					SpellEntry* spellInfo = m_nextSpell->spell;

					/* if in range stop moving so we don't interrupt the spell */
					//do not stop for instant spells
					if(sd && GetCastTime(sd) != 0)
						StopMovement(0);

					if( m_nextSpell->shout_before_cast && m_nextSpell->shout_before_cast[0] != 0 )
						m_Unit->SendChatMessage( CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, m_nextSpell->shout_before_cast );

					uint32 targettype = m_nextSpell->spelltargetType;
					switch(targettype)
					{
					case TTYPE_SOURCE:
						{
							m_Unit->CastSpellAoF( deafult_target, deafult_target->GetPositionX(),deafult_target->GetPositionY(),deafult_target->GetPositionZ(), spellInfo, true);
							break;
						}
					case TTYPE_DESTINATION:
						{
							m_Unit->CastSpellAoF( deafult_target, m_Unit->GetPositionX(),m_Unit->GetPositionY(),m_Unit->GetPositionZ(), spellInfo, true);
							break;
						}
					default:
						SpellCastTargets targets( deafult_target->GetGUID() );
						CastSpell(m_Unit, spellInfo, targets);
						break;
					}
					local_target = GetNextTarget();

					//finished with this spell 
					m_nextSpell = NULL;
				}
				else // Target out of Range -> Run to it
				{
					//calculate next move
					m_moveRun = true;
					float close_to_enemy = 0.0f;
					if( distance > m_nextSpell->maxrange )
						close_to_enemy = m_nextSpell->maxrange - DISTANCE_TO_SMALL_TO_WALK ;
					else if( distance < m_nextSpell->minrange )
						close_to_enemy = m_nextSpell->minrange + DISTANCE_TO_SMALL_TO_WALK ;

					if( close_to_enemy < 0 )
						close_to_enemy = 0;
						
					_CalcDestinationAndMove( local_target, close_to_enemy ); //if we make exact movement we will never position perfectly
					local_target = GetNextTarget();
					/*Destination* dst = _CalcDestination(GetNextTarget(), dist);
					MoveTo(dst->x, dst->y, dst->z,0);
					delete dst;*/
				}
			}break;
		case AGENT_FLEE:
			{
				//float dist = 5.0f;

				m_moveRun = false;
				if(m_fleeTimer == 0)
					m_fleeTimer = m_FleeDuration;

				/*Destination* dst = _CalcDestination(GetNextTarget(), dist);
				MoveTo(dst->x, dst->y, dst->z,0);
				delete dst;*/
				_CalcDestinationAndMove( local_target, 5.0f);
				local_target = GetNextTarget();
				if(!m_hasFleed)
					CALL_SCRIPT_EVENT(m_Unit, OnFlee)(GetNextTarget());

				m_AIState = STATE_FLEEING;
				//removed by Zack : somehow creature starts to attack sefl. Just making sure it is not this one
//				m_nextTarget = m_Unit;
//				m_Unit->SetUInt64Value(UNIT_FIELD_TARGET, 0);
				SetNextTarget( (Unit*)NULL );
				local_target = GetNextTarget();

				m_Unit->SendChatMessage(CHAT_MSG_CHANNEL, LANG_UNIVERSAL, "%s attempts to run away in fear!" );

				//m_Unit->SendChatMessage(CHAT_MSG_MONSTER_EMOTE, LANG_UNIVERSAL, msg);
				//sChatHandler.FillMessageData(&data, CHAT_MSG_MONSTER_EMOTE, LANG_UNIVERSAL, msg, m_Unit->GetGUID());			   
			   
				m_hasFleed = true;
			}break;
		case AGENT_CALLFORHELP:
			{
				FindFriends( 100.0f /*10.0f*/,true );
				m_hasCalledForHelp = true; // We only want to call for Help once in a Fight.
				if( m_Unit->GetTypeId() == TYPEID_UNIT )
						objmgr.HandleMonsterSayEvent( SafeCreatureCast( m_Unit ), MONSTER_SAY_EVENT_CALL_HELP );
				CALL_SCRIPT_EVENT( m_Unit, OnCallForHelp )();
			}break;
		}
	}
	else if( !local_target || local_target->GetInstanceID() != m_Unit->GetInstanceID() || !local_target->isAlive() 
//		|| !cansee 
		)
	{
		SetNextTarget( (Unit*)NULL );
		HandleEvent( EVENT_LEAVECOMBAT, m_Unit, 0);
		local_target = GetNextTarget();
		// no more target
		//m_Unit->setAttackTarget(NULL);
	}
}

void AIInterface::DismissPet()
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	/*
	if(m_AIType != AITYPE_PET)
		return;

	if(!m_PetOwner)
		return;
	
	if(m_PetOwner->GetTypeId() != TYPEID_PLAYER)
		return;

	if(m_Unit->GetUInt32Value(UNIT_CREATED_BY_SPELL) == 0)
		SafePlayerCast( m_PetOwner )->SetFreePetNo(false, (int)m_Unit->GetUInt32Value(UNIT_FIELD_PETNUMBER));
	SafePlayerCast( m_PetOwner )->SetPet(NULL);
	SafePlayerCast( m_PetOwner )->SetPetName("");
	
	//FIXME:Check hunter pet or not
	//FIXME:Check enslaved creature
	m_PetOwner->SetUInt64Value(UNIT_FIELD_SUMMON, 0);
	
	WorldPacket data;
	data.Initialize(SMSG_PET_SPELLS);
	data << (uint64)0;
	SafePlayerCast( m_PetOwner )->GetSession()->SendPacket(&data);
	
	sEventMgr.RemoveEvents(((Creature*)m_Unit));
	if(m_Unit->IsInWorld())
	{
		m_Unit->RemoveFromWorld();
	}
	//setup an event to delete the Creature
	sEventMgr.AddEvent(((Creature*)this->m_Unit), &Creature::DeleteMe, EVENT_DELETE_TIMER, 1, 1);*/
}

void AIInterface::AttackReaction(Unit* pUnit, uint32 damage_dealt, uint32 spellId)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if( m_AIState == STATE_EVADE || !pUnit || !pUnit->isAlive() || m_Unit->IsDead() || m_Unit == pUnit )
	{ 
		sLog.outDebug("AttackReaction:Cannot react to this target.Exiting");
		return;
	}
	//do not gain agro from targets that are not attackable 
	if( pUnit->HasFlag( UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NOT_ATTACKABLE_2 | UNIT_FLAG_DEAD ) )
	{
		return;
	}
/*	if( sWorld.Collision && pUnit->IsPlayer() )
	{
		float target_land_z=0.0f;
		if ( m_Unit->GetMapMgr() != NULL )
		{
			if (!m_moveFly)
			{
				target_land_z = CollideInterface.GetHeight(m_Unit->GetMapId(), pUnit->GetPositionX(), pUnit->GetPositionY(), pUnit->GetPositionZ() + 2.0f);
				if ( target_land_z == NO_WMO_HEIGHT )
					target_land_z = m_Unit->GetMapMgr()->GetLandHeight(pUnit->GetPositionX(), pUnit->GetPositionY());

				if (fabs(pUnit->GetPositionZ() - target_land_z) > _CalcCombatRange(pUnit, false) )
				{
					if ( pUnit->GetTypeId()!=TYPEID_PLAYER && target_land_z > m_Unit->GetMapMgr()->GetWaterHeight(pUnit->GetPositionX(), pUnit->GetPositionY()) )
					{ 
						sLog.outDebug("AttackReaction:Target is too far away.Exiting");
						return;
					}
					else if( pUnit->IsPlayer() && SafePlayerCast(pUnit)->GetSession() != NULL )
					{
						MovementInfo* mi=SafePlayerCast(pUnit)->GetSession()->GetMovementInfo();

						if ( mi != NULL && (mi->flags & ( MOVEFLAG_FALLING | MOVEFLAG_SWIMMING | MOVEFLAG_LEVITATE)) != 0 )
						{ 
							sLog.outDebug("AttackReaction:Cannot react to player that is in the air.Exiting");
							return;
						}
					}
				}
			}
		}
	} */

	if ( m_Unit->IsPlayer() == false //we are a player, do not make client switch our target to most hated due to redirects
		&& pUnit->IsPlayer() && SafePlayerCast(pUnit)->GetMisdirectionTarget() != 0 && m_Unit->GetMapMgr() )
	{
		Unit *mTarget = m_Unit->GetMapMgr()->GetUnit(SafePlayerCast(pUnit)->GetMisdirectionTarget());
		if (mTarget != NULL && mTarget->isAlive() )
			pUnit = mTarget;
	}

	if( (m_AIState == STATE_IDLE || m_AIState == STATE_FOLLOWING) && m_Unit->GetAIInterface()->GetAllowedToEnterCombat())
	{
		WipeTargetList();
		
		HandleEvent(EVENT_ENTERCOMBAT, pUnit, 0);
	}

	HandleEvent(EVENT_DAMAGETAKEN, pUnit, _CalcThreat(damage_dealt, spellId ? dbcSpell.LookupEntryForced(spellId) : NULL, pUnit));
}

void AIInterface::HealReaction(Unit* caster, Unit* victim, SpellEntry* sp, uint32 amount)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if(!caster || !victim || (sp && sp->ThreatForSpellCoef == 0) || isAttackable( caster, victim) == false 
		)
	{ 
		return;
	}

	bool casterInList = false, victimInList = false;

	if(m_aiTargets.find(caster->GetGUID()) != m_aiTargets.end())
		casterInList = true;

	if(m_aiTargets.find(victim->GetGUID()) != m_aiTargets.end())
		victimInList = true;

	if(!victimInList && !casterInList) // none of the Casters is in the Creatures Threat list
	{ 
		return;
	}

	int32 threat = int32(amount / 2 / 4); //to make life easier for noobs we scale heal agro to 25%
	if (caster->getClass() == PALADIN)
		threat = threat / 2; //Paladins only get 50% threat per heal than other classes

	threat += caster->GetThreatModifyer();
	if (sp != NULL)
	{
		threat += (threat * caster->GetGeneratedThreatModifyerPCT(sp->School) / 100);
		threat = int32(threat * sp->ThreatForSpellCoef);
	}
	else
		threat += (threat * caster->GetGeneratedThreatModifyerPCT( SCHOOL_NORMAL ) / 100);

	if (threat < 1)
		threat = 1;

	if(!casterInList && victimInList) // caster is not yet in Combat but victim is
	{
		// get caster into combat if he's hostile
		if(isAttackable(m_Unit, caster))
			modThreatByPtr(caster, threat);
	}
	else if(casterInList && victimInList) // both are in combat already
		modThreatByPtr(caster, threat);

	else // caster is in Combat already but victim is not
	{
		modThreatByPtr(caster, threat);
		// both are players so they might be in the same group
		if( caster->GetTypeId() == TYPEID_PLAYER && victim->GetTypeId() == TYPEID_PLAYER )
		{
			if( SafePlayerCast( caster )->GetGroup() == SafePlayerCast( victim )->GetGroup() )
			{
				// get victim into combat since they are both
				// in the same party
				if( isAttackable( m_Unit, victim ) )
					modThreatByPtr( victim, threat );
			}
		}
	}
}

void AIInterface::OnDeath(Object* pKiller)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if(pKiller->GetTypeId() == TYPEID_PLAYER || pKiller->GetTypeId() == TYPEID_UNIT)
		HandleEvent(EVENT_UNITDIED, SafeUnitCast(pKiller), 0);
	else
		HandleEvent(EVENT_UNITDIED, m_Unit, 0);
}

//function is designed to make a quick check on target to decide if we can attack it
bool AIInterface::UnsafeCanOwnerAttackUnit(Unit *pUnit)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if( !pUnit->isAlive() )
	{ 
		return false;
	}

	if( pUnit->bInvincible )
	{ 
		return false;
	}

	//do not agro units that are faking death. Should this be based on chance ?
	if( pUnit->HasFlag( UNIT_FIELD_FLAGS, UNIT_FLAG_FEIGN_DEATH ) )
	{ 
		return false;
	}

	if( !(pUnit->m_phase & m_Unit->m_phase) ) //Not in the same phase
		return false;
/*
	removed so FFA can work -> ishostile should handle this
	//don't agro neutrals
	if( ( pUnit->IsPlayer() || pUnit->IsPet() )
		&& m_Unit->m_factionDBC
		&& m_Unit->m_factionDBC->RepListId == -1 
		&& m_Unit->m_faction->HostileMask == 0 
		&& m_Unit->m_faction->FriendlyMask == 0
		)
		return false;
	else if( ( m_Unit->IsPlayer() || m_Unit->IsPet() )
			&& pUnit->m_factionDBC
			&& pUnit->m_factionDBC->RepListId == -1 
			&& pUnit->m_faction->HostileMask == 0 
			&& pUnit->m_faction->FriendlyMask == 0
			)
		return false; */

	//make sure we do not agro flying stuff
	if( abs( pUnit->GetPositionZ() - m_Unit->GetPositionZ() ) > _CalcCombatRange( pUnit, false ) )
		return false; //blizz has this set to 250 but uses pathfinding

	//check PVP
	//pets and summons should not enagage in combat with non pvp stuff
	if( m_Unit->GetUInt32Value( UNIT_FIELD_CREATEDBY ) )
	{
		//don't attack owner
		if( m_Unit->GetUInt64Value(UNIT_FIELD_CREATEDBY) == pUnit->GetGUID() )
		{ 
			return false; 
		}
		Player *owner = m_Unit->GetMapMgr()->GetPlayer( m_Unit->GetUInt32Value( UNIT_FIELD_CREATEDBY ) );

		if( owner )
		{
			if( owner->DuelingWith && owner->GetDuelState() == DUEL_STATE_STARTED )
			{
				//we are dueling this guy
				if( owner->DuelingWith == pUnit )
					return true;
				//if we are dueling this guy's owner
				if( owner->DuelingWith->GetGUID() == pUnit->GetUInt32Value( UNIT_FIELD_CREATEDBY ) )
					return true;
			}
			//if we are in pvp and target is in PVP
			if( owner->IsPvPFlagged() == false && pUnit->IsPvPFlagged() == false )
				return false;
		}
	}

	//do not agro neutral or not attackable targets
	if(  isHostile(m_Unit,pUnit) == false || isAttackable(m_Unit,pUnit )== false)
	{ 
		return false;
	}

	return true;
}

//this function might be slow but so it should not be spammed
//!!!this function has been reported the biggest bottleneck on emu in 2008 07 04
Unit* AIInterface::FindTarget()
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
// find nearest hostile Target to attack
	if( disable_combat == true ) 
	{ 
		return NULL;
	}

	if(  m_Unit->GetMapMgr() == NULL )
	{ 
		return NULL; 
	}

	Unit* target = NULL;
	Unit* critterTarget = NULL;
	float distance = 999999.0f; // that should do it.. :p
//	float crange;
//	float z_diff;

	InRangeSetRecProt::iterator itr, itr2;
	InRangePlayerSetRecProt::iterator pitr, pitr2;
//	Object *pObj;
	Unit *pUnit;
	float dist;
	bool pvp=true;
//	if(m_Unit->GetTypeId()==TYPEID_UNIT&&(SafeCreatureCast(m_Unit)->GetCreatureInfo()&&SafeCreatureCast(m_Unit)->GetCreatureInfo()->Civilian) )
//		pvp=false;

	//target is immune to all form of attacks, cant attack either.
	// not attackable creatures sometimes fight enemies in scripted fights though
	if( m_Unit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NOT_ATTACKABLE_2) && m_Unit->GetUInt32Value( UNIT_FIELD_CREATEDBY ) == 0 ) //some scripted mobs like guardian of ancient kings has this flag and can still attack
	{ 
		return 0;
	}
#ifdef _DEBUG
	uint32 Entry = m_Unit->GetEntry();
#endif
	if (m_isNeutralGuard)
	{
		Player *tmpPlr;
		InrangeLoopExitAutoCallback AutoLock;
		for (InRangePlayerSetRecProt::iterator itrPlr = m_Unit->GetInRangePlayerSetBegin( AutoLock ); itrPlr != m_Unit->GetInRangePlayerSetEnd(); ++itrPlr)
		{
			tmpPlr = (*itrPlr);
			if (tmpPlr == NULL)
				continue;
			if (tmpPlr->GetTaxiState())
				continue;
			if (tmpPlr->bInvincible)
				continue;
			if (tmpPlr->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FEIGN_DEATH))
				continue;
			if (tmpPlr->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_9))
				continue;
			if (tmpPlr->IsInvisible())
				continue;
			if( !(tmpPlr->m_phase & m_Unit->m_phase) ) //Not in the same phase, skip this target
				continue;
			if (tmpPlr->GetSelection() == 0)
				continue;
			else
			{
				Unit *pPTarget = GetUnit()->GetMapMgr()->GetUnit( tmpPlr->GetSelection() );
				if( pPTarget == NULL)
					continue;
				if (!pPTarget->IsPlayer())
					continue;
				if( tmpPlr->CombatStatus.IsInCombat() == false )
					continue;
				if (tmpPlr->DuelingWith == SafePlayerCast(pPTarget))
					continue;
				if( isAttackable( tmpPlr, pPTarget ) == false )
					continue;
			}

			if( tmpPlr->IsCrowdControlledNoAttack() ) //stop attacking
				continue;

			dist = m_Unit->GetDistanceSq(tmpPlr);

			if (dist > 2500.0f)
				continue;
			if (distance > dist)
			{
/*				if (sWorld.Collision) 
				{
					if( CollideInterface.CheckLOS( m_Unit->GetMapId(), m_Unit->GetPositionNC(), tmpPlr->GetPositionNC() ) )
					{
						distance = dist;
						target = SafeUnitCast(tmpPlr);
					}
				} 
				else  */
				{
					distance = dist;
					target = SafeUnitCast(tmpPlr);
				}
			}
		}
		if (target)
		{
//			m_Unit->m_runSpeed = m_Unit->m_base_runSpeed * 2.0f;
			AttackReaction(target, 1, 0);

			SendAIReaction( 0, AI_REACTION_CLIENT_AUTO_ALWAYS_FACE_TARGET );

			return target;
		}
		distance = 999999.0f; //Reset Distance for normal check
	}

	//we have a high chance that we will agro a player
	//this is slower then oppfaction list BUT it has a lower chance that contains invalid pointers
	InrangeLoopExitAutoCallback AutoLock;
	for( pitr2 = m_Unit->GetInRangePlayerSetBegin( AutoLock ); pitr2 != m_Unit->GetInRangePlayerSetEnd(); )
	{
		pitr = pitr2;
		++pitr2;

		Player *pUnit = *pitr;

		if( UnsafeCanOwnerAttackUnit( pUnit ) == false )
			continue;
/*		if( !isAttackable( m_Unit, pUnit ) )
			continue;

		//pets and summons should not enagage in combat with non pvp stuff
		if( m_Unit->GetUInt32Value( UNIT_FIELD_CREATEDBY ) )
		{
			//if we are dueling this player then ignore checks
			if( pUnit->DuelingWith == NULL )
			{
				//target player is not PVP flagged and he is not a duel target
				if( pUnit->IsPvPFlagged() == false //target is not PVP flagged
					|| m_Unit->IsPvPFlagged() == false )//waiting for owner to trigger PVP flag
					continue;
			}
			else 
			{
				//check to attack only the duel target of our owner
			}
		}*/

		//on blizz there is no Z limit check 
		dist = m_Unit->GetDistanceSq(pUnit);

		if(dist > distance)	 // we want to find the CLOSEST target
			continue;
	
		if( pUnit->IsCrowdControlledNoAttack() ) //stop attacking
			continue;

		if(dist <= _CalcAggroRange(pUnit) )
		{
/*			if (sWorld.Collision) 
			{
				if( CollideInterface.CheckLOS( m_Unit->GetMapId( ), m_Unit->GetPositionNC( ), pUnit->GetPositionNC( ) ) )
				{
					distance = dist;
					target = pUnit;
				}
			} 
			else */
			{
				distance = dist;
				target = pUnit;
			}
		}
	}

	//a lot less times are check inter faction mob wars :)
	if( target == NULL && m_updateTargetsTimer2 < getMSTime() )
	{
		m_updateTargetsTimer2 = getMSTime() + TARGET_UPDATE_INTERVAL;
		m_Unit->AquireInrangeLock(); //make sure to release lock before exit function !
		InrangeLoopExitAutoCallback AutoLock;
		for( itr2 = m_Unit->GetInRangeSetBegin( AutoLock ); itr2 != m_Unit->GetInRangeSetEnd(); )
		{
			itr = itr2;
			++itr2;

			if( !(*itr)->IsUnit() )
				continue;

			pUnit = SafeUnitCast( (*itr) );

            // We checked for player targets before
            if( pUnit->IsPlayer() )
                continue;

			if( UnsafeCanOwnerAttackUnit( pUnit ) == false )
				continue;

			//pets and summons should not enagage in combat with non pvp stuff
/*			if( m_Unit->GetUInt32Value( UNIT_FIELD_CREATEDBY ) )
			{
				Player *owner = m_Unit->GetMapMgr()->GetPlayer( m_Unit->GetUInt32Value( UNIT_FIELD_CREATEDBY ) );
				if( owner && owner->DuelingWith == NULL )
				{
					//target player is not PVP flagged and he is not a duel target
					if( pUnit->IsPvPFlagged() == false //target is not PVP flagged
						|| m_Unit->IsPvPFlagged() == false ) //waiting for owner to trigger PVP flag
						continue;
				}
				else
				{
					//check if that mob is a summon of the target player
				}
			}

            // if the target is not attackable we are not going to attack it and find a new target, if possible
			if( !isAttackable( m_Unit, pUnit ) )
				continue; */

			//on blizz there is no Z limit check 
			dist = m_Unit->GetDistanceSq(pUnit);

			if(pUnit->m_faction && pUnit->m_faction->Faction == 28)// only Attack a critter if there is no other Enemy in range
			{
				if(dist < 225.0f)	// was 10
					critterTarget = pUnit;
				continue;
			}

			if(dist > distance)	 // we want to find the CLOSEST target
				continue;
	
			if( pUnit->IsCrowdControlledNoAttack() ) //stop attacking
				continue;

			if(dist <= _CalcAggroRange(pUnit) )
			{
/*				if (sWorld.Collision) 
				{
					if( CollideInterface.CheckLOS( m_Unit->GetMapId( ), m_Unit->GetPositionNC( ), pUnit->GetPositionNC( ) ) )
					{
						distance = dist;
						target = pUnit;
					}
				} 
				else */
				{
					distance = dist;
					target = pUnit;
				}
			}
		}
		m_Unit->ReleaseInrangeLock();
	}

	if( !target )
	{
		target = critterTarget;
	}

	//just in case desperate times requires us to take measures. If we really did not manage to find anything to attack then try to ask our owner to attack something
	if( target == NULL )
	{
		Unit *topo = m_Unit->GetTopOwner();
		if( topo != m_Unit && topo->IsPlayer() && SafePlayerCast( topo )->IsAttacking() )
		{
			target = topo->GetMapMgr()->GetUnit( SafePlayerCast( topo )->GetSelection() );
		}
	}

	if( target )
	{
/*		if(m_isGuard)
		{
			m_Unit->m_runSpeed = m_Unit->m_base_runSpeed * 2.0f;
			m_fastMove = true;
		}*/

		AttackReaction(target, 1, 0);
		if(target->IsPlayer())
		{
			SendAIReaction( 0, AI_REACTION_CLIENT_AUTO_ALWAYS_FACE_TARGET, SafePlayerCast( target )->GetSession() );
		}
		//if our target is a pet, then we shoudl try to agro owner also for later case
		if(target->GetUInt32Value(UNIT_FIELD_CREATEDBY) != 0)
		{
			Unit* target2 = m_Unit->GetMapMgr()->GetPlayer(target->GetUInt32Value(UNIT_FIELD_CREATEDBY));
			/*if(!target2)
			{
				target2 = sObjHolder.GetObject<Player>(target->GetUInt32Value(UNIT_FIELD_CREATEDBY));
			}*/
			if(target2)
			{
				AttackReaction(target2, 1, 0);
			}
		}
	}
	return target;
}

void AIInterface::SendAIReaction( uint64 Target, uint32 ReactionType, WorldSession *Session )
{
	//do not auto face targets for creatures that need to stand in 1 place / orientation
	if( disable_targeting )
		return;
	if( Target != 0 )
		m_Unit->SetUInt64Value( UNIT_FIELD_TARGET, Target );
	sStackWorldPacket( data,SMSG_AI_REACTION, 15 );
	data << m_Unit->GetGUID() << uint32( ReactionType );		// Aggro sound
	if( Session != NULL )
		Session->SendPacket( &data );
	else
		m_Unit->SendMessageToSet( &data, true );
}

bool AIInterface::FindFriends(float dist,bool urgent)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE

	if(  m_Unit->GetMapMgr() == NULL )
	{ 
		return false; 
	}

	//no function spamming
	if( getMSTime() < call_friend_cooldown && urgent == false )
	{ 
		return false;
	}

	call_friend_cooldown = getMSTime() + CALL_FOR_FRIEND_COOLDOWN;

	bool result = false;
	TargetMap::iterator it;

	InRangeSetRecProt::iterator itr;
	Unit *pUnit;

	float dist_mod = 1.0f;
	if( m_Unit->IsCreature() && SafeCreatureCast( m_Unit )->GetProto() )
		dist_mod = 1 + MAX( SafeCreatureCast( m_Unit )->GetProto()->CombatReach, SafeCreatureCast( m_Unit )->GetProto()->BoundingRadius );

	m_Unit->AquireInrangeLock(); //make sure to release lock before exit function !
	InrangeLoopExitAutoCallback AutoLock;
	for( itr = m_Unit->GetInRangeSetBegin( AutoLock ); itr != m_Unit->GetInRangeSetEnd(); itr++ )
	{
		if(!(*itr) || (*itr)->GetTypeId() != TYPEID_UNIT || m_Unit==(*itr))
			continue;

		pUnit = SafeUnitCast((*itr));
		if(!pUnit->isAlive())
			continue;

		if(pUnit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
		{
			continue;
		}
		if(pUnit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_9))
		{
			continue;
		}

		//if selected inrange unit can help us and he is idleing
//		if( isCombatSupport( m_Unit, pUnit ) && ( pUnit->GetAIInterface()->getAIState() == STATE_IDLE || pUnit->GetAIInterface()->getAIState() == STATE_SCRIPTIDLE ) )//Not sure
		if( isCombatSupport( m_Unit, pUnit ) )//Not sure
		{
			//and he is in range too
			float dist_mod_now = dist_mod;
			if( pUnit->IsCreature() && SafeCreatureCast( pUnit )->GetProto() )
				dist_mod_now += 1 + MAX( SafeCreatureCast( pUnit )->GetProto()->CombatReach, SafeCreatureCast( pUnit )->GetProto()->BoundingRadius );
			dist_mod_now *= dist_mod_now;	//just square it
			float dist_now = m_Unit->GetDistanceSq(pUnit);	//just for debug
			if( dist_now < dist + dist_mod_now )
			{
/*				if( m_assistTargets.count( pUnit->GetGUID() ) == 0 )
				{
					result = true;
					m_assistTargets.insert(pUnit->GetGUID());
				}*/

				LockAITargets(true);

				//no idea how we get to this but if we are not on a map then we do not need attacktargets either
				if( m_Unit->GetMapMgr() == NULL ) 
					m_aiTargets.clear();

				//don't let mob spam for call for help instantly
				if( pUnit->GetAIInterface() )
				{
					pUnit->GetAIInterface()->call_friend_cooldown = getMSTime() + CALL_FOR_FRIEND_COOLDOWN;
					pUnit->GetAIInterface()->m_hasCalledForHelp = true;
				}

				//agro all the the targets that the mob we are helping has
				for(it = m_aiTargets.begin(); it != m_aiTargets.end(); ++it)
				{
					Unit *ai_t = m_Unit->GetMapMgr()->GetUnit( it->first );
					if( ai_t && pUnit->GetAIInterface() && isAttackable(ai_t,pUnit) )
						pUnit->GetAIInterface()->AttackReaction( ai_t, 1, 0 );
				}

				LockAITargets(false);

			}
		}
	}
	m_Unit->ReleaseInrangeLock();

	// check if we're a civillan, in which case summon guards on a despawn timer
/*	uint8 civilian = (SafeCreatureCast(m_Unit)->GetCreatureInfo()) ? (SafeCreatureCast(m_Unit)->GetCreatureInfo()->Civilian) : 0;
	uint32 family = (SafeCreatureCast(m_Unit)->GetCreatureInfo()) ? (SafeCreatureCast(m_Unit)->GetCreatureInfo()->Type) : 0;
	if(family == HUMANOID && civilian && getMSTime() > m_guardTimer && !IS_INSTANCE(m_Unit->GetMapId()))
	{
		m_guardTimer = getMSTime() + 15000;
		uint16 AreaId = m_Unit->GetMapMgr()->GetAreaID(m_Unit->GetPositionX(),m_Unit->GetPositionY());
		AreaTable * at = dbcArea.LookupEntry(AreaId);
		if(!at)
		{ 
			return result;
		}

		ZoneGuardEntry * zoneSpawn = ZoneGuardStorage.LookupEntry(at->ZoneId);
		if(!zoneSpawn) 
		{ 
			return result;
		}

		uint32 team = 1; // horde default
		if(isAlliance(m_Unit))
			team = 0;

		uint32 guardid = zoneSpawn->AllianceEntry;
		if(team == 1) guardid = zoneSpawn->HordeEntry;
		{ 
		if(!guardid) 
		{ 
			return result;
		}
		}

		CreatureInfo * ci = CreatureNameStorage.LookupEntry(guardid);
		if(!ci)
		{ 
			return result;
		}

		float x = m_Unit->GetPositionX() + (float)( (float)(rand() % 150 + 100) / 1000.0f );
		float y = m_Unit->GetPositionY() + (float)( (float)(rand() % 150 + 100) / 1000.0f );
		float z;

		if (sWorld.Collision) {
			z = CollideInterface.GetHeight(m_Unit->GetMapId(), x, y, m_Unit->GetPositionZ() + 2.0f);
			if( z == NO_WMO_HEIGHT )
				z = m_Unit->GetMapMgr()->GetLandHeight(x, y);

			if( fabs( z - m_Unit->GetPositionZ() ) > 10.0f )
				z = m_Unit->GetPositionZ();
		} else {
			z = m_Unit->GetPositionZ();
			float adt_z = m_Unit->GetMapMgr()->GetLandHeight(x, y);
			if(fabs(z - adt_z) < 3)
				z = adt_z;
		}

		CreatureProto * cp = CreatureProtoStorage.LookupEntry(guardid);
		if(!cp) return result;

		uint32 languageid = (team == 0) ? LANG_COMMON : LANG_ORCISH;
		m_Unit->SendChatMessage(CHAT_MSG_MONSTER_SAY, languageid, "Guards!");

		uint8 spawned = 0;
	
		InRangePlayerSet::iterator hostileItr = m_Unit->GetInRangePlayerSetBegin();
		for(; hostileItr != m_Unit->GetInRangePlayerSetEnd(); hostileItr++)
		{
			if(spawned >= 3)
				break;

			if(!isAttackable(*hostileItr, m_Unit))
				continue;

			Creature * guard = m_Unit->GetMapMgr()->CreateCreature(guardid);
			guard->Load(cp, x, y, z);
			guard->SetInstanceID(m_Unit->GetInstanceID());
			guard->SetZoneId(m_Unit->GetZoneId());
			guard->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP); // shitty DBs 
//			guard->m_noRespawn=true;
		
			if(guard->CanAddToWorld())
			{
				uint32 t = RandomUInt(8)*1000;
				if(t==0)
					guard->PushToWorld(m_Unit->GetMapMgr());
				else
					sEventMgr.AddEvent(guard,&Creature::AddToWorld, m_Unit->GetMapMgr(), EVENT_UNK, t, 1, 0);
			}
			else
			{
				guard->SafeDelete();
				return result;
			}
			
			sEventMgr.AddEvent(guard, &Creature::SetGuardWaypoints, EVENT_UNK, 10000, 1,0);
			sEventMgr.AddEvent(guard, &Creature::SafeDelete, EVENT_CREATURE_SAFE_DELETE, 60*5*1000, 1,0);
			spawned++;
		}
	}*/

	return result;
}

float AIInterface::_CalcAggroRange(Unit* target)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	//float baseAR = 15.0f; // Base Aggro Range
					// -8	 -7	 -6	 -5	 -4	 -3	 -2	 -1	 0	  +1	 +2	 +3	 +4	 +5	 +6	 +7	+8
	//float baseAR[17] = {29.0f, 27.5f, 26.0f, 24.5f, 23.0f, 21.5f, 20.0f, 18.5f, 17.0f, 15.5f, 14.0f, 12.5f, 11.0f,  9.5f,  8.0f,  6.5f, 5.0f};
	float baseAR[17] = {19.0f, 18.5f, 18.0f, 17.5f, 17.0f, 16.5f, 16.0f, 15.5f, 15.0f, 14.5f, 12.0f, 10.5f, 8.5f,  7.5f,  6.5f,  6.5f, 5.0f};
	// Lvl Diff -8 -7 -6 -5 -4 -3 -2 -1 +0 +1 +2  +3  +4  +5  +6  +7  +8
	// Arr Pos   0  1  2  3  4  5  6  7  8  9 10  11  12  13  14  15  16
	int8 lvlDiff = (int8)(target->getLevel() - m_Unit->getLevel());
	uint8 realLvlDiff = lvlDiff;
	if(lvlDiff > 8)
	{
		lvlDiff = 8;
	}
	if(lvlDiff < -8)
	{
		lvlDiff = -8;
	}
	if(m_Unit->IsCreature() && !SafeCreatureCast(m_Unit)->CanSee(target))
		return 0;
	
	float AggroRange = baseAR[lvlDiff + 8];
	if(realLvlDiff > 8)
	{
		AggroRange += AggroRange * ((lvlDiff - 8) * 5 / 100);
	}

	// Multiply by elite value
	if(m_Unit->IsCreature() && SafeCreatureCast(m_Unit)->GetCreatureInfo() && SafeCreatureCast(m_Unit)->GetCreatureInfo()->Rank > 0)
		AggroRange *= (SafeCreatureCast(m_Unit)->GetCreatureInfo()->Rank) * 1.50f;

	if(AggroRange > 40.0f) // cap at 40.0f
	{
		AggroRange = 40.0f;
	}
  /*  //printf("aggro range: %f , stealthlvl: %d , detectlvl: %d\n",AggroRange,target->GetStealthLevel(),m_Unit->m_stealthDetectBonus);
	if(! ((Creature*)m_Unit)->CanSee(target))
	{
		AggroRange =0;
	//	AggroRange *= ( 100.0f - (target->m_stealthLevel - m_Unit->m_stealthDetectBonus)* 20.0f ) / 100.0f;
	}
*/
	// SPELL_AURA_MOD_DETECT_RANGE
	int32 modDetectRange = target->getDetectRangeMod(m_Unit->GetGUID());
	AggroRange += modDetectRange;
	if(target->IsPlayer())
		AggroRange += SafePlayerCast( target )->DetectedRange;
	AggroRange *= agro_range_mod;

	if(AggroRange < 3.0f)
		AggroRange = 3.0f;
	if(AggroRange > 40.0f) // cap at 40.0f
		AggroRange = 40.0f;
	
	AggroRange = AggroRange*AggroRange;

	if( AggroRange > m_outOfCombatRange )
		AggroRange = m_outOfCombatRange;

	//do not agro it anymore if we would go out of reach walking there
	float DistHomeIfweGetThere;
	if( m_Unit->IsCreature() )
		DistHomeIfweGetThere = target->GetDistanceSq( SafeCreatureCast(m_Unit)->GetSpawnX(), SafeCreatureCast(m_Unit)->GetSpawnY(), SafeCreatureCast(m_Unit)->GetSpawnZ() );
	else
		DistHomeIfweGetThere = 0.0f;
	if( DistHomeIfweGetThere + 5.0f * 5.0f > m_outOfCombatRange )
		AggroRange = 1.0f;

	return AggroRange;
}

void AIInterface::_CalcDestinationAndMove(Unit *target, float dist)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if(!m_canMove || m_Unit->IsStunned())
	{
		StopMovement(0); //Just Stop
		return;
	}
	
	if( target && ( target->IsCreature() || target->IsPlayer()) )
	{
#ifdef HACKY_SERVER_CLIENT_POS_SYNC
		moved_for_attack = true;
#endif
		float ResX = target->GetPositionX();
		float ResY = target->GetPositionY();

		//avoid eating bandwidth with useless movement packets when target did not move since last position
		//this will work since it turned into a common mith that when you pull mob you should not move :D
		if( ( abs(m_Unit->GetPositionX() - ResX) < DISTANCE_TO_SMALL_TO_WALK
			 && abs(m_Unit->GetPositionY() - ResY) < DISTANCE_TO_SMALL_TO_WALK )
			|| m_creatureState == MOVING)
			return;

		float ResZ = target->GetPositionZ();

		float angle_0 = m_Unit->calcAngle(m_Unit->GetPositionX(), m_Unit->GetPositionY(), ResX, ResY) * float(M_PI) / 180.0f;
		float angle;
		if( target->CombatStatusHandler_GetAttackerCount() > 1 )
		{
			//now check attacker count and try to find a not yet taken spot
			float angle_per_attacker = 2*M_PI/(target->CombatStatusHandler_GetAttackerCount()+1); 
			//new we should get somehow our attacker index...we have none ? Might be bigger then attacker list !
			uint32 attacker_index = target->CombatStatusHandler_GetAttackerIndex( m_Unit->GetGUID() );
			//based on index we should circle the target
			angle = angle_0 + angle_per_attacker * attacker_index;
		}
		else
			angle = angle_0;
		float x = dist * cosf(angle);
		float y = dist * sinf(angle);

		if(target->GetTypeId() == TYPEID_PLAYER && SafePlayerCast( target )->m_isMoving )
		{
			// cater for moving player vector based on orientation
			x -= cosf(target->GetOrientation());
			y -= sinf(target->GetOrientation());
		}

		m_destinationX = ResX - x;
		m_destinationY = ResY - y;
		m_destinationZ = ResZ;
		m_UnitToMoveTo = target->GetGUID();
	}
	else
	{
		target = NULL;
		m_destinationX = m_Unit->GetPositionX();
		m_destinationY = m_Unit->GetPositionY();
		m_destinationZ = m_Unit->GetPositionZ();
	}

/*
	if (sWorld.Collision) {
		float target_land_z=0.0f;
		if( m_Unit->GetMapMgr() != NULL )
		{
			if(m_moveFly != true)
			{
				target_land_z = CollideInterface.GetHeight(m_Unit->GetMapId(), m_nextPosX, m_nextPosY, m_nextPosZ + 2.0f);
				if( target_land_z == NO_WMO_HEIGHT )
					target_land_z = m_Unit->GetMapMgr()->GetLandHeight(m_nextPosX, m_nextPosY);
			}
		}

		if (m_nextPosZ > m_Unit->GetMapMgr()->GetWaterHeight(m_nextPosX, m_nextPosY) && target_land_z != 0.0f)
			m_nextPosZ=target_land_z;
	}
*/

	float dx = m_destinationX - m_Unit->GetPositionX();
	float dy = m_destinationY - m_Unit->GetPositionY();
	if(dy != 0.0f)
	{
		float angle = atan2(dx, dy);
		m_Unit->SetOrientation(angle);
	}

	if(m_creatureState != MOVING && dx != 0 )
		UpdateMove();
}

float AIInterface::_CalcCombatRange(Unit* target, bool ranged)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if(!target)
	{
		return 0;
	}
	float range = 0.0f;
	float rang = 0.0f;
	if(ranged)
	{
		rang = 5.0f;
	}

	float selfreach = m_Unit->GetFloatValue(UNIT_FIELD_COMBATREACH);
	float targetradius;
//	targetradius = target->GetFloatValue(UNIT_FIELD_BOUNDINGRADIUS); //this is plain wrong. Represents i have no idea what :)
	targetradius = target->GetModelHalfSize();
	float selfradius;
//	selfradius = m_Unit->GetFloatValue(UNIT_FIELD_BOUNDINGRADIUS); //this is plain wrong. Represents i have no idea what :)
	selfradius = m_Unit->GetModelHalfSize();
//	float targetscale = target->GetFloatValue(OBJECT_FIELD_SCALE_X);
//	float selfscale = m_Unit->GetFloatValue(OBJECT_FIELD_SCALE_X);

//	range = ((((targetradius*targetradius)*targetscale) + selfreach) + ((selfradius*selfscale) + rang));
	range = targetradius + selfreach + selfradius + rang;
//	if(range > 28.29f) range = 28.29f;
//	{ 
	return range;
//	}
}

float AIInterface::_CalcDistanceFromHome()
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if (m_AIType == AITYPE_PET)
	{
		if( !m_Unit->GetMapMgr() )
		{ 
			return 0;
		}
		//there is a chance that player logs out while pet is still in world for a brief moment
		Unit *t = m_Unit->GetMapMgr()->GetUnit( m_Unit->GetUInt64Value(UNIT_FIELD_SUMMONEDBY) );
		if( !t ) 
		{ 
			return 0;
		}
		return m_Unit->GetDistanceSq( t );
	}
	else if(m_Unit->IsCreature())
	{
		if( SafeCreatureCast(m_Unit)->m_spawn )
			return m_Unit->GetDistanceSq(SafeCreatureCast(m_Unit)->m_spawn->x,SafeCreatureCast(m_Unit)->m_spawn->y,SafeCreatureCast(m_Unit)->m_spawn->z);
		else 
		{
			return m_Unit->GetDistanceSq( SafeCreatureCast(m_Unit)->GetSpawnX(), SafeCreatureCast(m_Unit)->GetSpawnY(), SafeCreatureCast(m_Unit)->GetSpawnZ() );
		}
	}

	return 0.0f;
}

/************************************************************************************************************
SendMoveToPacket:
Comments: Some comments on the SMSG_MONSTER_MOVE packet: 
	the uint8 field:
		0: Default															known
		1: Don't move														known
		2: there is an extra 3 floats, also known as a vector				unknown
		3: there is an extra uint64 most likely a guid.						unknown
		4: there is an extra float that causes the orientation to be set.	known
		
		note:	when this field is 1. 
			there is no need to send  the next 3 uint32's as they are'nt used by the client
	
	the MoveFlags:
		0x00000000 - Walk
		0x00000100 - Run
		0x00000200 - Fly
		some comments on that 0x00000300 - Fly = 0x00000100 | 0x00000200

	waypoints:
		TODO.... as they somehow seemed to be changed long time ago..
		
*************************************************************************************************************/

void AIInterface::SendMoveToPacket(float toX, float toY, float toZ, float toO, uint32 time, uint32 MoveFlags, float Zspeed, WorldSession *Session)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	//this should NEVER be called directly !!!!!!
	//use MoveTo()
/* 3.3.5
FB AE 18 33 F1 8B 40 F1
00 
F7 76 D1 44
1A C5 8A C5
3E 3C A9 41
D3 D8 B3 00 
00 
00 10 00 00 
79 07 00 00
03 00 00 00
B5 DC D2 44
A7 2F 8B C5 
02 E7 98 41 
01 E0 BF 00
EC AF 80 00 
14333
8F F1 D6 F7 02 01 
20 - flags
58 B9 78 C2 
B1 42 A3 C3
9E 9B 5A 42 
08 C6 05 00 
04 
02 37 81 3F 
00 00 20 00 
00 00 00 00 
01 00 00 00 
45 69 70 C2 
EA E6 A3 C3 
9E 9B 5A 42 0467026 
*/
	//this is needed as some mobs are spawned right near their next and onyl waypoint, so they keep spamming move packet at each update
//	if( abs( m_Unit->GetPositionX() - toX ) < DISTANCE_TO_SMALL_TO_WALK ) 
//		return; //we don't want little movements here and there

#ifdef USE_SPELL_CAST_TO_DEBUG_CREATURE_MOVE_SPAM
	m_Unit->CastSpell( m_Unit, USE_SPELL_CAST_TO_DEBUG_CREATURE_MOVE_SPAM, true );	//using this spell to see if mobs spam movement packet
#endif

	sStackWorldPacket( data, SMSG_MONSTER_MOVE, 150);
	data << m_Unit->GetNewGUID();
	data << uint8(0);	//some flags
	data << m_Unit->GetPositionX() << m_Unit->GetPositionY() << m_Unit->GetPositionZ();
	if( m_Unit->IsPlayer() )
		data << uint32( SafePlayerCast( m_Unit )->m_PositionUpdateCounter++ );
	else
		data << getMSTime();

//debuging why mobs randomly move  / get out of sync -> remove unknown flags, add something that blizz seem to use all the time 
//MoveFlags = MoveFlags & ~(0x200000|0x800);
//MoveFlags |= 0x02000000; //this and sometimes 0x06000000 is used by blizz

	// Check if we have an orientation
	if(toO != 0.0f)
	{
		data << uint8( MonsterMoveFacingAngle );
		data << toO;
	} 
	else 
	{
		data << uint8(0);
	}
	data << MoveFlags;
/*	if(MoveFlags & 0x200000)
	{
		data << uint8(0);
		data << uint32(0);
	}
	if(MoveFlags & 0x800)
	{
		data << float(0);
		data << uint32(0);
	} */
	data << time;
	if( MoveFlags & Parabolic )
	{
		data << float( Zspeed );
		data << uint32( 0 );	//no idea
	}
	data << uint32(1);	  // 1 waypoint
	data << toX << toY << toZ;

	bool self = m_Unit->IsPlayer();
	if( Session )
		Session->SendPacket( &data );
	else
		m_Unit->SendMessageToSet( &data, self );
}

/*
void AIInterface::SendMoveToSplinesPacket(std::list<Waypoint> wp, bool run)
{
	if(!m_canMove)
	{
		return;
	}

	WorldPacket data;

	uint8 DontMove = 0;
	uint32 travelTime = 0;
	for(std::list<Waypoint>::iterator i = wp.begin(); i != wp.end(); i++)
	{
		travelTime += i->time;
	}

	data.Initialize( SMSG_MONSTER_MOVE );
	data << m_Unit->GetNewGUID();
	data << m_Unit->GetPositionX() << m_Unit->GetPositionY() << m_Unit->GetPositionZ();
	data << getMSTime();
	data << uint8(DontMove);
	data << uint32(run ? 0x00000100 : 0x00000000);
	data << travelTime;
	data << (uint32)wp.size();
	for(std::list<Waypoint>::iterator i = wp.begin(); i != wp.end(); i++)
	{
		data << i->x;
		data << i->y;
		data << i->z;
	}

	m_Unit->SendMessageToSet( &data, false );
}
*/
bool AIInterface::StopMovement(uint32 time)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	m_TimeRemainingReachDestination = 0;
	m_creatureState = STOPPED;

	m_destinationX = m_destinationY = m_destinationZ = 0;

#ifdef USE_SPELL_CAST_TO_DEBUG_CREATURE_MOVE_SPAM
	if( time )
		m_Unit->CastSpell( m_Unit, USE_SPELL_CAST_TO_DEBUG_CREATURE_MOVE_SPAM, true );	//using this spell to see if mobs spam movement packet
#endif
	//let client decide our orientation
	//zack : use position update to avoid desyncronizations ?
	sStackWorldPacket(data,SMSG_MONSTER_MOVE,30);
	data << m_Unit->GetNewGUID();
	data << uint8(0); //some flags
	data << m_Unit->GetPositionX() << m_Unit->GetPositionY() << m_Unit->GetPositionZ();
	if( m_Unit->IsPlayer() )
		data << uint32( SafePlayerCast( m_Unit )->m_PositionUpdateCounter++ );
	else
		data << getMSTime();
	data << uint8(1);   // "DontMove = 1"
	m_Unit->SendMessageToSet( &data, true );
	/**/

	//maybe we should update our position client side in case there is some desyncronization
//	SendMoveToPacket( m_Unit->GetPositionX(), m_Unit->GetPositionY(), m_Unit->GetPositionZ(), m_Unit->GetOrientation(),time,0);
	return true;
}

void AIInterface::MoveTo(float x, float y, float z, float o)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE

	if(!m_canMove || m_Unit->IsStunned())
	{
		StopMovement(0); //Just Stop
		return;
	}


	m_destinationX = x;
	m_destinationY = y;
	m_destinationZ = z;

/*	//Andy
#ifdef COLLISION
	float target_land_z=0.0f;
	if( m_Unit->GetMapMgr() != NULL )
	{
		if(m_moveFly != true)
		{
			target_land_z = CollideInterface.GetHeight(m_Unit->GetMapId(), m_nextPosX, m_nextPosY, m_nextPosZ + 2.0f);
			if( target_land_z == NO_WMO_HEIGHT )
				target_land_z = m_Unit->GetMapMgr()->GetLandHeight(m_nextPosX, m_nextPosY);
		}
	}

	if (m_nextPosZ > m_Unit->GetMapMgr()->GetWaterHeight(m_nextPosX, m_nextPosY) && target_land_z != 0.0f)
		m_nextPosZ=target_land_z;
#endif*/

//	if ( m_creatureState != MOVING )	//Zack : and what if we switch destination while moving ?
		UpdateMove();
}

bool AIInterface::IsFlying()
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if(m_moveFly)
	{ 
		return true;
	}
	
	/*float z = m_Unit->GetMapMgr()->GetLandHeight(m_Unit->GetPositionX(), m_Unit->GetPositionY());
	if(z)
	{
		if(m_Unit->GetPositionZ() >= (z + 1.0f)) //not on ground? Oo
		{
			return true;
		}
	}
	return false;*/
	if( m_Unit->GetTypeId() == TYPEID_PLAYER )
	{ 
		return SafePlayerCast( m_Unit )->FlyCheat;
	}

	return false;
}

uint32 AIInterface::getMoveFlags()
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	uint32 MoveFlags = 0;
	MoveFlags |= Forward;	//default flag for movement to ask client to do collision for us
	if(m_moveFly == true) //Fly
	{
		MoveFlags = MONSTER_MOVE_FLAG_FLY;
	}
	else if(m_moveSprint == true) //Sprint
	{
		MoveFlags = MONSTER_MOVE_FLAG_RUN;
	}
	else if(m_moveRun == true) //Run
	{
		MoveFlags = MONSTER_MOVE_FLAG_RUN;
	}
	//trying to improve people falling through the ground
//	if( m_Unit->IsPlayer() && ( m_AIState == STATE_FEAR || m_AIState == STATE_WANDER ) )
//	{
//		MoveFlags |= MONSTER_MOVE_LEVITATING;
//		MoveFlags |= MONSTER_MOVE_FLY | MONSTER_MOVE_UNK9;
		//Forward, Falling
//	}
/*	else //Walk
	{
		m_runSpeed = m_Unit->m_walkSpeed*0.001f;
		MoveFlags = 0x000;
	}*/
	return MoveFlags;
}

void AIInterface::UpdateMove()
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	//this should NEVER be called directly !!!!!!
	//use MoveTo()
	float distance = m_Unit->CalcDistance(m_destinationX,m_destinationY,m_destinationZ);

	//this is needed as some mobs are spawned right near their next and onyl waypoint, so they keep spamming move packet at each update
	if( distance < DISTANCE_TO_SMALL_TO_WALK ) 
		return; //we don't want little movements here and there
	
	/*if(m_moveFly != true)
	{
		if(m_Unit->GetMapMgr())
		{
			float adt_Z = m_Unit->GetMapMgr()->GetLandHeight(m_destinationX, m_destinationY);
			if(fabsf(adt_Z - m_destinationZ) < 3.0f)
				m_destinationZ = adt_Z;
		}
	}*/

	//this prevents updatemovement working on this function
	//m_nextPosX = m_nextPosY = m_nextPosZ = 0;

	uint32 moveTime;
/* #ifdef INHERIT_FOLLOWED_UNIT_SPEED vojta - this is pointless and we dont need it anymore
	if( UnitToFollow )
	{
//		moveTime = (uint32) (distance * 1000 / UnitToFollow->m_runSpeed ); //i wonder if runpeed can ever frop to 0
		//life sucks, due to calculations the pet will move slower with corect formulas. We add some catch-up speed
		moveTime = (uint32) (distance * 1000 / ( UnitToFollow->m_runSpeed * sqrt( distance ) ) ); //i wonder if runpeed can ever frop to 0
	}
#endif */
	float MoveSpeed;
	if(m_moveFly)
		MoveSpeed = m_Unit->m_flySpeed;
	else if( m_moveRun )
		MoveSpeed = m_Unit->m_runSpeed;
	else 
		MoveSpeed = m_Unit->m_walkSpeed;

	//some say pets move 2x fast when owner is mounted
	if( m_Unit->IsPet() )
	{
		if( SafePetCast( m_Unit )->GetPetOwner() && SafePetCast( m_Unit )->GetPetOwner()->IsMounted() )
			MoveSpeed = SafePetCast( m_Unit )->GetPetOwner()->m_runSpeed  * 1.5f;
	}

	moveTime = (uint32) (distance * 1000.0f / MoveSpeed);

	if( m_Unit->IsCreature() )
	{
		Creature *creature = SafeCreatureCast(m_Unit);
		// check if we're returning to our respawn location. if so, reset back to default
		// orientation
		if(creature->GetSpawnX() == m_destinationX &&
			creature->GetSpawnY() == m_destinationY)
		{
			float o = creature->GetSpawnO();
			creature->SetOrientation(o);
		} else {
			// Calculate the angle to our next position

			float dx = (float)m_destinationX - m_Unit->GetPositionX();
			float dy = (float)m_destinationY - m_Unit->GetPositionY();
			if(dy != 0.0f)
			{
				float angle = atan2(dy, dx);
				m_Unit->SetOrientation(angle);
			}
		}
	}

	if (m_Unit->GetCurrentSpell() == NULL)
		SendMoveToPacket(m_destinationX, m_destinationY, m_destinationZ, 0, moveTime, getMoveFlags());

	m_TimeRemainingReachDestination = moveTime;
	m_LastMoveUpdateStamp = getMSTime();
//	m_timeToMove = moveTime;
//	m_timeMoved = 0;
//	if(m_moveTimer == 0)
//		m_moveTimer =  UNIT_MOVEMENT_INTERPOLATE_INTERVAL; // update every few msecs

	m_creatureState = MOVING;
}

void AIInterface::SendCurrentMove(Player* plyr/*uint64 guid*/)
{
	if(m_destinationX != 0.0f 
//		&& m_destinationY == 0.0f && m_destinationZ == 0.0f
		)
		SendMoveToPacket(m_destinationX, m_destinationY, m_destinationZ, 0.0f, m_TimeRemainingReachDestination, getMoveFlags(), 0.0f, plyr != NULL ? plyr->GetSession() : NULL );
}

bool AIInterface::setInFront(Unit* target) // not the best way to do it, though
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	//angle the object has to face
	float angle = m_Unit->calcAngle(m_Unit->GetPositionX(), m_Unit->GetPositionY(), target->GetPositionX(), target->GetPositionY() ); 
	//Change angle slowly 2000ms to turn 180 deg around
	if(angle > 180) angle += 90;
	else angle -= 90; //angle < 180
	m_Unit->getEasyAngle(angle);
	//Convert from degrees to radians (180 deg = PI rad)
	float orientation = angle / float(180 / M_PI);
	//Update Orentation Server Side
	m_Unit->SetPosition(m_Unit->GetPositionX(), m_Unit->GetPositionY(), m_Unit->GetPositionZ(), orientation);
	
	return m_Unit->isInFront(target);
}

Unit* AIInterface::getUnitToFollow()
{ 
	if( UnitToFollowGUID == 0 )
		return NULL;
	Unit * u = m_Unit->GetMapMgr()->GetUnit( UnitToFollowGUID );
	if( u == 0 )
		UnitToFollowGUID = 0;
	return u; 
}

bool AIInterface::addWayPoint(WayPoint* wp)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if(!m_waypoints)
		m_waypoints = new WayPointVect ;
	if(!wp) 
	{ 
		return false;
	}
	if(wp->id <= 0)
		return false; //not valid id

	if( m_waypoints->GetValue( wp->id ) == NULL )
	{
		m_waypoints->SetValue( wp->id, wp );
		return true;
	}
	return false;
}

void AIInterface::changeWayPointID(uint32 oldwpid, uint32 newwpid)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if(!m_waypoints)
	{ 
		return;
	}
	if(newwpid <= 0) 
		return; //not valid id
	if(newwpid > m_waypoints->GetMaxSize()) 
		return; //not valid id
	if(oldwpid > m_waypoints->GetMaxSize())
	{ 
		return;
	}

	if(newwpid == oldwpid) 
		return; //same spot

	//already wp with that id ?
	WayPoint* originalwp = getWayPoint(newwpid);
	if(!originalwp) 
	{ 
		return;
	}
	WayPoint* oldwp = getWayPoint(oldwpid);
	if(!oldwp) 
	{ 
		return;
	}

	//swap indexes
	oldwp->id = newwpid;
	originalwp->id = oldwpid;
	//swap the order of WP
	m_waypoints->SetValue(newwpid,oldwp);
	m_waypoints->SetValue(oldwpid,originalwp);

	//SaveAll to db
	saveWayPoints();
}

void AIInterface::deleteWayPoint(uint32 wpid)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if( !m_waypoints )
	{ 
		return;
	}
	if( wpid < 0 ) 
		return; //not valid id
	if( wpid > m_waypoints->GetMaxSize() ) 
		return; //not valid id
	if( m_waypoints->GetValue(wpid) == NULL )
		return; // already deleted ?

	uint32 sql_id = m_waypoints->GetValue(wpid)->id;
	//delete the node
	delete m_waypoints->GetValue(wpid);
	//shift rest one to the left
	uint32 max_index = m_waypoints->GetMaxSize();
	for(uint32 i=wpid;i<max_index-1;i++)
		m_waypoints->SetValue(i,m_waypoints->GetValue(i+1));

	WorldDatabase.Execute("DELETE FROM creature_waypoints WHERE spawnid = %u and waypointid = %u", SafeCreatureCast(GetUnit())->GetSQL_id(), sql_id);
}

bool AIInterface::showWayPoints(Player* pPlayer, bool Backwards)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if(!m_waypoints)
	{ 
		return false;
	}

	//wpid of 0 == all
	if(m_WayPointsShowing == true) 
	{ 
		return false;
	}

	m_WayPointsShowing = true;

	WayPoint* wp;
	uint32 max_wp = m_waypoints->GetMaxSize();
	for (uint32 iter=0;iter < max_wp; iter++)
	{
		wp = m_waypoints->GetValue( iter );
		if( !wp )
			continue;

		//Create
		Creature* pWayPoint = new Creature((uint64)HIGHGUID_TYPE_WAYPOINT << 32 | wp->id);
		pWayPoint->CreateWayPoint(wp->id,pPlayer->GetMapId(),wp->x,wp->y,wp->z,0);
		pWayPoint->SetUInt32Value(OBJECT_FIELD_ENTRY, 300000);
		pWayPoint->SetFloatValue(OBJECT_FIELD_SCALE_X, 0.5f);
		if(Backwards)
		{
			uint32 DisplayID = (wp->backwardskinid == 0)? GetUnit()->GetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID) : wp->backwardskinid;
			pWayPoint->SetUInt32Value(UNIT_FIELD_DISPLAYID, DisplayID);
			pWayPoint->SetUInt32Value(UNIT_NPC_EMOTESTATE, wp->backwardemoteid);
		}
		else
		{
			uint32 DisplayID = (wp->forwardskinid == 0)? GetUnit()->GetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID) : wp->forwardskinid;
			pWayPoint->SetUInt32Value(UNIT_FIELD_DISPLAYID, DisplayID);
			pWayPoint->SetUInt32Value(UNIT_NPC_EMOTESTATE, wp->forwardemoteid);
		}
		pWayPoint->SetUInt32Value(UNIT_FIELD_LEVEL, wp->id);
		pWayPoint->SetUInt32Value(UNIT_NPC_FLAGS, 0);
//			pWayPoint->SetUInt32Value(UNIT_FIELD_AURA+32, 8326); //invisable & deathworld look
		pWayPoint->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE , pPlayer->GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE));
		pWayPoint->SetUInt32Value(UNIT_FIELD_HEALTH, 1);
		pWayPoint->SetUInt32Value(UNIT_FIELD_MAXHEALTH, 1);
		pWayPoint->SetUInt32Value(UNIT_FIELD_STAT0, wp->flags);

		//Create on client
		ByteBuffer buf(2500);
		uint32 count = pWayPoint->BuildCreateUpdateBlockForPlayer(&buf, pPlayer);
		pPlayer->PushCreationData( &buf, count, pWayPoint );

		//Cleanup
		delete pWayPoint;
		pWayPoint = NULL;
	}
	return true;
}

bool AIInterface::hideWayPoints(Player* pPlayer)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if(!m_waypoints)
	{ 
		return false;
	}

	//wpid of 0 == all
	if(m_WayPointsShowing != true) 
	{ 
		return false;
	}
	m_WayPointsShowing = false;

	// slightly better way to do this
	uint64 guid;

	uint32 max_wp = m_waypoints->GetMaxSize();
	for (uint32 iter=0;iter < max_wp; iter++)
	{
		WayPoint* wp = m_waypoints->GetValue( iter );
		if( !wp )
			continue;
		// avoid C4293
		guid = ((uint64)HIGHGUID_TYPE_WAYPOINT << 32) | wp->id;
		WoWGuid wowguid(guid);
		pPlayer->PushOutOfRange(wowguid);
	}
	return true;
}

bool AIInterface::saveWayPoints()
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if(!m_waypoints)
	{ 
		return false;
	}

	if(!GetUnit()) 
	{ 
		return false;
	}
	{ 
	if(GetUnit()->GetTypeId() != TYPEID_UNIT) 
	{ 
		return false;
	}
	}

	WorldDatabase.Execute("DELETE FROM creature_waypoints WHERE spawnid = %u", SafeCreatureCast(GetUnit())->GetSQL_id());
	std::stringstream ss;

	//Save
	uint32 max_wp = m_waypoints->GetMaxSize();
	for (uint32 iter=0;iter < max_wp; iter++)
	{
		WayPoint* wp = m_waypoints->GetValue( iter );
		if( !wp )
			continue;
		ss.str("");
		ss << "REPLACE INTO creature_waypoints ";
		ss << "(spawnid,waypointid,position_x,position_y,position_z,waittime,flags,forwardemoteoneshot,forwardemoteid,backwardemoteoneshot,backwardemoteid,forwardskinid,backwardskinid) VALUES (";
		ss << SafeCreatureCast(GetUnit())->GetSQL_id() << ", ";
		ss << wp->id << ", ";
		ss << wp->x << ", ";
		ss << wp->y << ", ";
		ss << wp->z << ", ";
		ss << wp->waittime << ", ";
		ss << wp->flags << ", ";
		ss << wp->forwardemoteoneshot << ", ";
		ss << wp->forwardemoteid << ", ";
		ss << wp->backwardemoteoneshot << ", ";
		ss << wp->backwardemoteid << ", ";
		ss << wp->forwardskinid << ", ";
		ss << wp->backwardskinid << ")\0";
		QueryResult *result = WorldDatabase.Query( ss.str().c_str() );
		if( result )
			delete result;
	}
	return true;
}

void AIInterface::deleteWaypoints()
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if(!m_waypoints)
	{ 
		return;
	}
	m_waypoints->DestroyContent();
	//save to db
	WorldDatabase.Execute("DELETE FROM creature_waypoints WHERE spawnid = %u", SafeCreatureCast(GetUnit())->GetSQL_id());
}

WayPoint* AIInterface::getWayPoint(uint32 wpid)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if(!m_waypoints)
	{ 
		return NULL;
	}
	return m_waypoints->GetValue(wpid);
}

void AIInterface::_UpdateMovement(uint32 p_time)
{
	if( !m_Unit->isAlive() )
	{
		StopMovement(0);
		return;
	}

	//move after finishing our current spell
	if ( m_Unit->GetCurrentSpell() != NULL )
	{ 
		return;
	}

	int32 TimeDiff;
	if( m_LastMoveUpdateStamp != 0 )
		TimeDiff = (int32)getMSTime() - (int32)m_LastMoveUpdateStamp;
	else
		TimeDiff = p_time;
	//this needs to be able to recover time in case NPC is walking and casting at the same time
	m_LastMoveUpdateStamp = getMSTime();
	if( TimeDiff <= 0 )
		return;

	if( (int32)m_TimeRemainingReachDestination > TimeDiff )
		m_TimeRemainingReachDestination -= TimeDiff;
	else
		m_TimeRemainingReachDestination = 0;

	if( m_creatureState == WAITING && m_TimeRemainingReachDestination == 0 )
		m_creatureState = STOPPED;

	if(m_creatureState == MOVING)
	{
		if( m_TimeRemainingReachDestination != 0 ) //update our position
		{
			float MoveSpeed;
			if(m_moveFly)
				MoveSpeed = m_Unit->m_flySpeed;
			else if( m_moveRun )
				MoveSpeed = m_Unit->m_runSpeed;
			else 
				MoveSpeed = m_Unit->m_walkSpeed;

			//some say pets move 2x fast when owner is mounted
			if( m_Unit->IsPet() )
			{
				if( SafePetCast( m_Unit )->GetPetOwner() && SafePetCast( m_Unit )->GetPetOwner()->IsMounted() )
					MoveSpeed = SafePetCast( m_Unit )->GetPetOwner()->m_runSpeed  * 1.5f;
			}

			float dist_to_move = TimeDiff * MoveSpeed / 1000.0f;	//correct is to use sin + cos + radius
			float x,y,z;
			if( m_Unit->GetPositionX() < m_destinationX )
				x = m_Unit->GetPositionX() + dist_to_move;
			else
				x = m_Unit->GetPositionX() - dist_to_move;
			if( m_Unit->GetPositionY() < m_destinationY )
				y = m_Unit->GetPositionY() + dist_to_move;
			else
				y = m_Unit->GetPositionY()- dist_to_move;
			if( m_Unit->GetPositionZ() != m_destinationZ )
			{
				if( m_Unit->GetPositionZ() < m_destinationZ )
					z = m_Unit->GetPositionZ() + dist_to_move;
				else
					z = m_Unit->GetPositionZ() - dist_to_move;
			}
			else
				z = m_Unit->GetPositionZ();

			m_Unit->SetPosition( x, y, z, m_Unit->GetOrientation() );

			//check if our target changed position and we should try to move to a new location
			if( m_UnitToMoveTo )
			{
				Unit *Target = m_Unit->GetMapMgr()->GetUnit( m_UnitToMoveTo );
				if( Target )
				{
					bool UpdateTargetPos = false;
					float CollisionDist = Target->GetModelHalfSize() + m_Unit->GetModelHalfSize();
					float CollisionDistsSQ = CollisionDist * CollisionDist;
					float DistTargetMovedFromOurDest = Distance3DSq( m_destinationX, m_destinationY, m_destinationZ, Target->GetPositionX(), Target->GetPositionY(), Target->GetPositionZ() );
					float DistUsFromTarget = 0;
					if( DistTargetMovedFromOurDest < DISTANCE_TO_SMALL_TO_WALK * DISTANCE_TO_SMALL_TO_WALK + CollisionDistsSQ ) //target is close to where we last seen him
					{
						DistUsFromTarget = Distance3DSq( m_Unit->GetPositionX(), m_Unit->GetPositionY(), m_Unit->GetPositionZ(), Target->GetPositionX(), Target->GetPositionY(), Target->GetPositionZ() );
						if( DistUsFromTarget + CollisionDistsSQ < DistTargetMovedFromOurDest ) //target seems to be closer to us then we are to our destination position ( comming to us )
							UpdateTargetPos = true;
					}
					else
						UpdateTargetPos =  true;
					if( UpdateTargetPos )
					{
						_CalcDestinationAndMove( Target, m_Unit->GetModelHalfSize() );
//						char DebugMsg[500];
//						sprintf( DebugMsg, "we are adjusting our destination %f %f %f", CollisionDist, DistTargetMovedFromOurDest, DistUsFromTarget );
//						m_Unit->SendChatMessage( CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, DebugMsg );
					}
				}
			}
		}
		else if( m_TimeRemainingReachDestination == 0 ) //reached our desination
		{
			if(m_moveType == MOVEMENTTYPE_WANTEDWP)//We reached wanted wp stop now
				m_moveType = MOVEMENTTYPE_DONTMOVEWP;

			float wayO = 0.0f;

			if((GetWayPointsCount() != 0) && (m_AIState == STATE_IDLE || m_AIState == STATE_SCRIPTMOVE)) //if we attacking don't use wps
			{
				WayPoint* wp = getWayPoint(getCurrentWaypoint());
				if(wp)
				{
					CALL_SCRIPT_EVENT(m_Unit, OnReachWP)(wp->id, !m_moveBackward);
					if(SafeCreatureCast(m_Unit)->has_waypoint_text)
						objmgr.HandleMonsterSayEvent(SafeCreatureCast(m_Unit), MONSTER_SAY_EVENT_RANDOM_WAYPOINT);

					//Lets face to correct orientation
					wayO = wp->o;
					if( wp->waittime > 0 ) //wait before next move
					{
						m_creatureState = WAITING;
						m_TimeRemainingReachDestination = MAX( 2000, wp->waittime );
					}
					if(!m_moveBackward)
					{
						if(wp->forwardemoteoneshot)
						{
							GetUnit()->Emote(EmoteType(wp->forwardemoteid));
						}
						else
						{
							if(GetUnit()->GetUInt32Value(UNIT_NPC_EMOTESTATE) != wp->forwardemoteid)
							{
								GetUnit()->SetUInt32Value(UNIT_NPC_EMOTESTATE, wp->forwardemoteid);
							}
						}
					}
					else
					{
						if(wp->backwardemoteoneshot)
						{
							GetUnit()->Emote(EmoteType(wp->backwardemoteid));
						}
						else
						{
							if(GetUnit()->GetUInt32Value(UNIT_NPC_EMOTESTATE) != wp->backwardemoteid)
							{
								GetUnit()->SetUInt32Value(UNIT_NPC_EMOTESTATE, wp->backwardemoteid);
							}
						}
					}
				}
				else
				{
					m_creatureState = WAITING;
					m_TimeRemainingReachDestination = 5000 + RandomUInt(m_moveRun ? 5000 : 10000); // wait before next move
				}
			}

			//in case we do not wait between waypoints then we consider ourself stopped
			if( m_creatureState != WAITING )
				m_creatureState = STOPPED;
			m_moveSprint = false;

			if(m_MovementType == MOVEMENTTYPE_DONTMOVEWP)
				m_Unit->SetPosition(m_destinationX, m_destinationY, m_destinationZ, wayO, true);
			else
				m_Unit->SetPosition(m_destinationX, m_destinationY, m_destinationZ, m_Unit->GetOrientation(), true);

			m_destinationX = m_destinationY = m_destinationZ = 0;
			//update client side position to avoid desyncronizations
//			StopMovement(0);
		}
	}
	else
	{
		//we are not moving anymore
		m_UnitToMoveTo = 0;
	}
	if( m_creatureState == STOPPED && (m_AIState == STATE_IDLE || m_AIState == STATE_SCRIPTMOVE) && m_TimeRemainingReachDestination==0 && UnitToFollowGUID == 0 ) //creature is stopped and out of Combat
	{
		if(sWorld.getAllowMovement() == false) //is creature movement enabled?
		{ 
			return;
		}

		if( m_Unit->IsCreature() && SafeCreatureCast(m_Unit)->isSpiritService() ) //if Spirit Healer don't move
		{ 
			return;
		}

		// do we have a formation?
		if(m_formationLinkSqlId != 0)
		{
			if(!m_formationLinkTarget)
			{
				// haven't found our target yet
				Creature * c = SafeCreatureCast(m_Unit);
				if(!c->haslinkupevent)
				{
					// register linkup event
					c->haslinkupevent = true;
					sEventMgr.AddEvent(c, &Creature::FormationLinkUp, m_formationLinkSqlId, 
						EVENT_CREATURE_FORMATION_LINKUP, 1000, 0,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
				}
			}
			else
			{
				// we've got a formation target, set unittofollow to this
				UnitToFollowGUID = m_formationLinkTarget->GetGUID();
//				FollowDistance = m_formationFollowDistance;
				m_followAngle = m_formationFollowAngle;
			}
		}
		if( UnitToFollowGUID == 0 )
		{
			// no formation, use waypoints
			int destpoint = -1;

			//don't do anything if we are ment to not use WP
			if( m_moveType == MOVEMENTTYPE_DONT_USE_WP )
			{
			}
			// If creature has no waypoints just wander aimlessly around spawnpoint
			else if(GetWayPointsCount()==0) //no waypoints
			{
				/*	if(m_moveRandom)
				{
				if((rand()%10)==0)																																	
				{																																								  
				float wanderDistance = rand()%4 + 2;
				float wanderX = ((wanderDistance*rand()) / RAND_MAX) - wanderDistance / 2;																											   
				float wanderY = ((wanderDistance*rand()) / RAND_MAX) - wanderDistance / 2;																											   
				float wanderZ = 0; // FIX ME ( i dont know how to get apropriate Z coord, maybe use client height map data)																											 

				if(m_Unit->CalcDistance(m_Unit->GetPositionX(), m_Unit->GetPositionY(), m_Unit->GetPositionZ(), ((Creature*)m_Unit)->respawn_cord[0], ((Creature*)m_Unit)->respawn_cord[1], ((Creature*)m_Unit)->respawn_cord[2])>15)																		   
				{   
				//return home																																				 
				MoveTo(((Creature*)m_Unit)->respawn_cord[0],((Creature*)m_Unit)->respawn_cord[1],((Creature*)m_Unit)->respawn_cord[2],false);
				}   
				else
				{
				MoveTo(m_Unit->GetPositionX() + wanderX, m_Unit->GetPositionY() + wanderY, m_Unit->GetPositionZ() + wanderZ,false);
				}	
				}	
				}
				*/	
//				return;																																				   
			}																																						  
			else //we do have waypoints
			{
				if(m_moveType == MOVEMENTTYPE_RANDOMWP) //is random move on if so move to a random waypoint
				{
					if(GetWayPointsCount() > 1)
						destpoint = RandomUInt((uint32)GetWayPointsCount());
				}
				else if (m_moveType == MOVEMENTTYPE_CIRCLEWP) //random move is not on lets follow the path in circles
				{
					// 1 -> 10 then 1 -> 10
					m_currentWaypoint++;
					if (m_currentWaypoint > GetWayPointsCount()) m_currentWaypoint = 1; //Happens when you delete last wp seems to continue ticking
					destpoint = m_currentWaypoint;
					m_moveBackward = false;
				}
				else if(m_moveType == MOVEMENTTYPE_WANTEDWP)//Move to wanted wp
				{
					if(m_currentWaypoint)
					{
						if(GetWayPointsCount() > 0)
						{
							destpoint = m_currentWaypoint;
						}
						else
							destpoint = -1;
					}
				}
				else if(m_moveType == MOVEMENTTYPE_FORWARDTHANSTOP)// move to end, then stop
				{
					++m_currentWaypoint;
					if(m_currentWaypoint > GetWayPointsCount())
					{
						//hmm maybe we should stop being path walker since we are waiting here anyway
						destpoint = -1;
					}
					else
						destpoint = m_currentWaypoint;
				}
				else if(m_moveType != MOVEMENTTYPE_QUEST && m_moveType != MOVEMENTTYPE_DONTMOVEWP)//4 Unused
				{
					// 1 -> 10 then 10 -> 1
					if (m_currentWaypoint > GetWayPointsCount()) m_currentWaypoint = 1; //Happens when you delete last wp seems to continue ticking
					if (m_currentWaypoint == GetWayPointsCount()) // Are we on the last waypoint? if so walk back
						m_moveBackward = true;
					if (m_currentWaypoint == 1) // Are we on the first waypoint? if so lets goto the second waypoint
						m_moveBackward = false;
					if (!m_moveBackward) // going 1..n
						destpoint = ++m_currentWaypoint;
					else				// going n..1
						destpoint = --m_currentWaypoint;
				}

				if(destpoint != -1)
				{
					WayPoint* wp = getWayPoint(destpoint);
					if(wp)
					{
						if(!m_moveBackward)
						{
							if((wp->forwardskinid != 0) && (GetUnit()->GetUInt32Value(UNIT_FIELD_DISPLAYID) != wp->forwardskinid))
							{
								GetUnit()->SetUInt32Value(UNIT_FIELD_DISPLAYID, wp->forwardskinid);
								GetUnit()->EventModelChange();
							}
						}
						else
						{
							if((wp->backwardskinid != 0) && (GetUnit()->GetUInt32Value(UNIT_FIELD_DISPLAYID) != wp->backwardskinid))
							{
								GetUnit()->SetUInt32Value(UNIT_FIELD_DISPLAYID, wp->backwardskinid);
								GetUnit()->EventModelChange();
							}
						}
						m_moveFly = (wp->flags & AI_INTERFACE_DB_FLAG_MOVE_FLY) ? 1 : 0;
						m_moveRun = (wp->flags & AI_INTERFACE_DB_FLAG_MOVE_RUN) ? 1 : 0;
						MoveTo(wp->x, wp->y, wp->z, 0);
					}
				}
			}
		}
	}

	//Fear Code. Fear roots player
	if(m_AIState == STATE_FEAR && UnitToFear != NULL && m_creatureState == STOPPED && m_TimeRemainingReachDestination == 0
		&& GetUnit()->m_rooted <= 1 && GetUnit()->m_stunned == 0 )
	{
		float Fx,Fy,Fz,Fo;
		float FxC,FyC; //detect facewall before it happens
		
		if( UnitToFear->IsPlayer() && 
			( SafePlayerCast( UnitToFear )->HasGlyphWithID( GLYPH_PRIEST_PSYCHIC_SCREAM ) 
				|| SafePlayerCast( UnitToFear )->HasGlyphWithID( GLYPH_WARRIOR_INTIMIDATING_SHOUT )	//should be available only for intimidating shout. Blizz used a stun spell as proc as solution
				|| SafePlayerCast( UnitToFear )->HasGlyphWithID( GLYPH_WARLOCK_FEAR )	
			) )
			return;

		float dist = m_Unit->CalcDistance(UnitToFear);
		if( dist < 30.0f )	// too far ? Stop running
		{
		// Calculate new angle to target.
			Fo = m_Unit->calcRadAngle(UnitToFear->GetPositionX(), UnitToFear->GetPositionY(), m_Unit->GetPositionX(), m_Unit->GetPositionY());
			double fAngleAdd = RandomFloat( ((M_PI/2) * 2) ) - (M_PI/2);
			Fo += (float)fAngleAdd;
			float RadX = (RandomFloat(20.f)+10.0f);
			float RadY = (RandomFloat(20.f)+10.0f);
			Fx = m_Unit->GetPositionX() + RadX*cosf(Fo);
			Fy = m_Unit->GetPositionY() + RadY*sinf(Fo);
			FxC = m_Unit->GetPositionX() + (RadX+2.0f)*cosf(Fo);
			FyC = m_Unit->GetPositionY() + (RadY+2.0f)*sinf(Fo);
		}
		else
		{
			//wait, maybe caster gets closer to us then we run some more
			m_TimeRemainingReachDestination = 100;
			m_creatureState = WAITING;
			return;
		}

/*			if (sWorld.Collision)
		{
			Fz = CollideInterface.GetHeight(m_Unit->GetMapId(), Fx, Fy, m_Unit->GetPositionZ() + 2.0f);
			if( Fz == NO_WMO_HEIGHT )
                Fz = m_Unit->GetMapMgr()->GetLandHeight(Fx, Fy);
			else
			{
				if( CollideInterface.GetFirstPoint(m_Unit->GetMapId(), m_Unit->GetPositionX(), m_Unit->GetPositionY(), m_Unit->GetPositionZ() + 2.0f,
					Fx, Fy, Fz + 2.0f, Fx, Fy, Fz, -1.0f) )
				{
					//Fz = CollideInterface.GetHeight(m_Unit->GetMapId(), Fx, Fy, m_Unit->GetPositionZ() + 2.0f);
				}
			}

			if( fabs( m_Unit->GetPositionZ() - Fz ) > 10.0f )
			{
				m_FearTimer=getMSTime() + 500;
			}
			else if( CollideInterface.CheckLOS(m_Unit->GetMapId(), m_Unit->GetPositionX(), m_Unit->GetPositionY(), m_Unit->GetPositionZ() + 2.0f, Fx, Fy, Fz) )
			{
				MoveTo(Fx, Fy, Fz, Fo);
				m_FearTimer = m_totalMoveTime + getMSTime() + 400;
			}
			else
			{
				StopMovement(0);
			}
		} 
		else */
		{
			// without these next checks we could fall through the "ground" (WMO) and get stuck
			// wander won't work correctly in cities until we get some way to fix this and remove these checks
			float currentZ = m_Unit->GetPositionZ();
			float landZ = m_Unit->GetMapMgr()->GetLandHeight(m_Unit->GetPositionX(), m_Unit->GetPositionY(), currentZ);

			Fz = m_Unit->GetMapMgr()->GetLandHeight(Fx, Fy, currentZ);
			
			if( abs( currentZ - landZ ) > 4.0f  //missing WMO probably it is a city
				|| Fz == VMAP_VALUE_NOT_INITIALIZED	//maybe we still need the value to be set
				|| ( sWorld.Collision == true && !CollideInterface.CheckLOS(m_Unit->GetMapId(),m_Unit->GetPositionX(),m_Unit->GetPositionY(),m_Unit->GetPositionZ(),FxC, FyC, Fz + 2.0f) )
				)
			{
				m_TimeRemainingReachDestination = 100;
				m_creatureState = WAITING;
			}
			else 
			{
				MoveTo(Fx, Fy, Fz + 1.0f, Fo);
			}
		}
	}

	// Wander AI movement code
	// wander should make the target return to the location it started to wander
	if( m_AIState == STATE_WANDER && m_creatureState == STOPPED && m_TimeRemainingReachDestination == 0 && GetUnit()->m_rooted == 0 && GetUnit()->m_stunned == 0 )
	{
		// calculate a random distance and angle to move
		float wanderD = RandomFloat(3.0f) + 2.0f;
		float wanderO = RandomFloat(6.283f);
		float wanderX = m_wanderStartX + wanderD * cosf(wanderO);
		float wanderY = m_wanderStartY + wanderD * sinf(wanderO);
		//detect collision before it happens
		float wanderXC = m_wanderStartX + ( wanderD + 2.0f ) * cosf(wanderO);
		float wanderYC = m_wanderStartY + ( wanderD + 2.0f ) * cosf(wanderO);

		//messed up, but people do report it as bug. Hunter misdirection shoul dhave high chance for people walking into traps
		if( RandomUInt() % 30 )
		{
			m_Unit->AquireInrangeLock(); //make sure to release lock before exit function !
			InrangeLoopExitAutoCallback AutoLock;
			for(InRangeSetRecProt::iterator itr = m_Unit->GetInRangeSetBegin( AutoLock ); itr != m_Unit->GetInRangeSetEnd(); itr++ )
			{
				// don't add objects that are not units and that are dead
				if( !( (*itr)->IsGameObject() ) || (*itr)->GetByte( GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_TYPEID) != GAMEOBJECT_TYPE_TRAP )
					continue;

				if( IsInrange( m_wanderStartX, m_wanderStartY, (*itr), 5.0f * 5.0f ) == false )
					continue;

				wanderX = (*itr)->GetPositionX();
				wanderY = (*itr)->GetPositionY();
			}
			m_Unit->ReleaseInrangeLock(); //make sure to release lock before exit function !
		}

/*		if (sWorld.Collision)
		{
			float wanderZ = CollideInterface.GetHeight(m_Unit->GetMapId(), wanderX, wanderY, m_Unit->GetPositionZ() + 2.0f);
			float wanderZ2 = wanderZ;
			if( wanderZ == NO_WMO_HEIGHT )
				wanderZ = m_Unit->GetMapMgr()->GetLandHeight(wanderX, wanderY);
			else
			{
				if( CollideInterface.GetFirstPoint(m_Unit->GetMapId(), m_Unit->GetPositionX(), m_Unit->GetPositionY(), m_Unit->GetPositionZ() + 2.0f,
					wanderX, wanderY, wanderZ + 2.0f, wanderX, wanderY, wanderZ, -1.0f) )
				{
					//wanderZ = CollideInterface.GetHeight(m_Unit->GetMapId(), wanderX, wanderY, m_Unit->GetPositionZ() + 2.0f);
				}
				else
					wanderZ = wanderZ2;
			}

			if( fabs( m_Unit->GetPositionZ() - wanderZ ) > 10.0f )
			{
				m_WanderTimer=getMSTime() + 1000;
			}
			else if(CollideInterface.CheckLOS(m_Unit->GetMapId(), m_Unit->GetPositionX(), m_Unit->GetPositionY(), m_Unit->GetPositionZ() + 2.0f, wanderX, wanderY, wanderZ))
			{
				m_Unit->SetOrientation(wanderO);
				MoveTo(wanderX, wanderY, wanderZ, wanderO);
				m_WanderTimer = getMSTime() + m_totalMoveTime + 300; // time till next move (+ pause)
			}
			else
			{
				StopMovement(0);
			}
		} 
		else */
		{
			// without these next checks we could fall through the "ground" (WMO) and get stuck
			// wander won't work correctly in cities until we get some way to fix this and remove these checks
			float currentZ = m_Unit->GetPositionZ();
			float landZ = m_Unit->GetMapMgr()->GetLandHeight(m_Unit->GetPositionX(), m_Unit->GetPositionY(), currentZ);

			float wanderZ = m_Unit->GetMapMgr()->GetLandHeight(wanderX, wanderY, currentZ);

			if( abs( currentZ - landZ ) > 4.0f		// are we more than N yards above ground? (possible WMO)
				|| wanderZ == VMAP_VALUE_NOT_INITIALIZED	//maybe we still need the value to be set
				|| ( sWorld.Collision == true && !CollideInterface.CheckLOS(m_Unit->GetMapId(),m_Unit->GetPositionX(),m_Unit->GetPositionY(),m_Unit->GetPositionZ(),wanderXC, wanderYC, wanderZ + 2.0f) )
				)
			{
				m_TimeRemainingReachDestination = 100;
				m_creatureState = WAITING;
			}
			else
			{
//				m_Unit->SetOrientation(wanderO);
				MoveTo(wanderX, wanderY, wanderZ + 1.0f, wanderO);
			}
		}
	}

	//Unit Follow Code
	if( UnitToFollowGUID != 0 )
	{
		Unit *UnitToFollow = m_Unit->GetMapMgr()->GetUnit( UnitToFollowGUID );
		if( UnitToFollow == NULL || UnitToFollow->event_GetInstanceID() != m_Unit->event_GetInstanceID() || !UnitToFollow->IsInWorld() )
		{
			UnitToFollow = NULL;
			UnitToFollowGUID = 0;
		}
		else
		{
			if( m_AIState == STATE_IDLE || m_AIState == STATE_FOLLOWING )
			{
				float dist = m_Unit->GetDistanceSq(UnitToFollow);

				// re-calculate orientation based on target's movement
/*				if( m_lastFollowX != UnitToFollow->GetPositionX() || m_lastFollowY != UnitToFollow->GetPositionY() )
				{
					float dx = UnitToFollow->GetPositionX() - m_Unit->GetPositionX();
					float dy = UnitToFollow->GetPositionY() - m_Unit->GetPositionY();
					if(dy != 0.0f)
					{
						float angle = atan2(dx,dy);
						m_Unit->SetOrientation(angle);
					}
					m_lastFollowX = UnitToFollow->GetPositionX();
					m_lastFollowY = UnitToFollow->GetPositionY();
				} */

				if (dist > ( FollowDistance * FollowDistance ) ) //if out of range
				{
					m_AIState = STATE_FOLLOWING;
					
					if(dist > 12.0f) //25 yard away lets run else we will loose the them
						m_moveRun = true;
					else 
						m_moveRun = false;

					if(m_AIType == AITYPE_PET || UnitToFollow == m_formationLinkTarget) //Unit is Pet/formation
					{
						if(dist > 15.0f*15.0f/*30*/)
							m_moveSprint = true;

						float delta_x = UnitToFollow->GetPositionX();
						float delta_y = UnitToFollow->GetPositionY();
						float d = 3;
						if(m_formationLinkTarget)
							d = m_formationFollowDistance;

						MoveTo(delta_x+(d*(cosf(m_followAngle+UnitToFollow->GetOrientation()))),
							delta_y+(d*(sinf(m_followAngle+UnitToFollow->GetOrientation()))),
							UnitToFollow->GetPositionZ(),UnitToFollow->GetOrientation());				
					}
					else
					{
						_CalcDestinationAndMove( UnitToFollow, FollowDistance );
					}
				}
			}
		}
	}
}

void AIInterface::CastSpell(Unit* caster, SpellEntry *spellInfo, SpellCastTargets targets)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if( m_AIType != AITYPE_PET && disable_spell )
	{ 
		return;
	}

	// Stop movement while casting.
	m_AIState = STATE_CASTING;
#ifdef _AI_DEBUG
	sLog.outString("AI DEBUG: Unit %u casting spell %s on target "I64FMT, caster->GetEntry(), 
		sSpellStore.LookupString(spellInfo->Name), targets.m_unitTarget);
#endif

	//i wonder if this will lead to a memory leak :S
	Spell *nspell = SpellPool.PooledNew( __FILE__, __LINE__ );
	nspell->Init(caster, spellInfo, false, NULL);
	nspell->prepare(&targets);
}

SpellEntry *AIInterface::getSpellEntry(uint32 spellId)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	SpellEntry *spellInfo = dbcSpell.LookupEntry(spellId );

	if(!spellInfo)
	{
		sLog.outError("WORLD: unknown spell id %i\n", spellId);
		return NULL;
	}

	return spellInfo;
}

AI_Spell *AIInterface::getSpell()
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	uint32 nowtime = getMSTime();
	if( spell_global_cooldown_stamp > nowtime )
	{ 
		return NULL;
	}

	// look at our spells
	AI_Spell *sp = NULL;
	AI_Spell *def_spell = NULL;
	uint32 smallest_cool_time = MAX_INT;

	if(m_Unit->IsPet())
	{
		sp = def_spell = SafePetCast(m_Unit)->HandleAutoCastEvent();
	}
	else
	{
		uint32 unitHPpct = m_Unit->GetHealthPct();
		//The hard part is that we need to cycle spells. If cooldown is smaller then global cooldown then same spell gets spammed
		SimplePointerListNode<AI_Spell> *itr;
		for(itr = m_spells.begin(); itr != m_spells.end();itr = itr->next)
		{
			sp = itr->data;

			//can't imagine spells that are not agent spells in this list :P
			if(sp->agent != AGENT_SPELL)
				continue;

			//this spell procs only x times
			if( sp->procCount && sp->procCounter >= sp->procCount )
				continue;

			// to not spam searching in this list we will update spells only at intervals with smallest cooldown times
			if(	nowtime < sp->cooldowntime )
			{
				if( sp->cooldowntime < smallest_cool_time )
					smallest_cool_time = sp->cooldowntime;
				continue; //spell is cooling down. No need to select it to be casted
			}

			//do we need N seconds to pass to cast this spell ?
			if( sp->initial_cooldown && entered_combat_stamp + sp->initial_cooldown > nowtime )
				continue;

			//check if this is like boss phases
			if( sp->spelltargetType != TTYPE_FRIENDLY_TO_HEAL )
			{
				if( sp->min_hp_pct && unitHPpct < sp->min_hp_pct )
					continue;
				if( sp->max_hp_pct && unitHPpct > sp->max_hp_pct )
					continue;
			}

			//see if this spell is available in this difficulty mode
			if( (sp->difficulty_mask & (1 << ( m_Unit->GetMapMgr()->instance_difficulty + 2 ) )) == 0 )
				continue;

			//chance to cast this -> wow rand chance to fail and not to cast ? 

			uint32 roll = RandomUInt( 100 );
			if( roll > sp->procChance )
				continue;
/*
			//!not yet implemented
			//focus/mana requirement
			switch(sp->spell->powerType)
			{
			case POWER_TYPE_MANA:
				if(m_Unit->GetPower( POWER_TYPE_MANA ) < sp->spell->GetManaCost())
					continue;
				break;

			case POWER_TYPE_FOCUS:
				if(m_Unit->GetUInt32Value(UNIT_FIELD_POWER3) < sp->spell->GetManaCost())
					continue;
				break;
			}*/

			//all conditions are filled. we can stop searching now
			def_spell = sp;
			smallest_cool_time = MOB_SPELLCAST_REFRESH_COOLDOWN_INTERVAL;
			break; // we selected a spell. We can exit search loop
		}
	}

	// save some loops if waiting for cooldownz
	if( smallest_cool_time != MAX_INT && def_spell == NULL )
		smallest_cool_time = MAX( MOB_SPELLCAST_REFRESH_COOLDOWN_INTERVAL, smallest_cool_time - nowtime );
	else 
		smallest_cool_time = MOB_SPELLCAST_REFRESH_COOLDOWN_INTERVAL;
	spell_global_cooldown_stamp = nowtime + smallest_cool_time;

#ifdef _AI_DEBUG
	sLog.outString("AI DEBUG: Returning no spell for unit %u", m_Unit->GetEntry());
#endif
	return def_spell;
}

void AIInterface::addSpellToList(AI_Spell *sp)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if(!sp || !sp->spell)
	{ 
		return;
	}

	AI_Spell * sp2 = new AI_Spell;
	memcpy(sp2, sp, sizeof(AI_Spell));
	m_spells.push_front(sp2);
}

uint32 AIInterface::getThreatByGUID(uint64 guid)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE

	if(  m_Unit->GetMapMgr() == NULL )
	{ 
		return 0; 
	}

	Unit *obj = m_Unit->GetMapMgr()->GetUnit(guid);
	if(obj)
	{ 
		return getThreatByPtr(obj);
	}

	return 0;
}

uint32 AIInterface::getThreatByPtr(Unit* obj)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if( !obj  || m_Unit->GetMapMgr() == NULL)
	{ 
		return 0;
	}
	TargetMap::iterator it = m_aiTargets.find(obj->GetGUID());
	if(it != m_aiTargets.end())
	{
		Unit *tempUnit = m_Unit->GetMapMgr()->GetUnit(it->first);
		if (tempUnit)
		{ 
			return it->second + tempUnit->GetThreatModifyer();
		}
		else
			return it->second;

	}
	return 0;
}

/*
#if defined(WIN32) && defined(HACKY_CRASH_FIXES)
__declspec(noinline) bool ___CheckTarget(Unit * ptr, Unit * him)
{
	__try
	{
		if( him->GetInstanceID() != ptr->GetInstanceID() || !him->isAlive() || !isAttackable( ptr, him ) )
		{
			return false;
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
	return true;
}
#endif
*/

//should return a valid target
Unit *AIInterface::GetMostHated()
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if(  m_Unit->GetMapMgr() == NULL )
	{ 
		return NULL; 
	}

	Unit *ResultUnit=NULL;

	//override mosthated with taunted target. Basic combat checks are made for it. 
	//What happens if we can't see tauntedby unit ?
	ResultUnit = getTauntedBy();
	if(ResultUnit)
	{ 
		return ResultUnit;
	}

	if( m_forced_target )
	{
		ResultUnit=m_Unit->GetMapMgr()->GetUnit( m_forced_target );
		if( ResultUnit == 0 )
			m_forced_target = 0;
		else
			return ResultUnit;
	}

	pair<Unit*, int32> currentTarget;
	currentTarget.first = 0;
	currentTarget.second = -1;

	LockAITargets(true);

	TargetMap::iterator it2 = m_aiTargets.begin();
	TargetMap::iterator itr;
	for(; it2 != m_aiTargets.end();)
	{
		itr = it2;
		++it2;

		/* check the target is valid */
/*
#if defined(WIN32) && defined(HACKY_CRASH_FIXES)
		if(!___CheckTarget( m_Unit, itr->first ) )
		{
			if( m_nextTarget == itr->first )
				m_nextTarget = NULL;

			m_aiTargets.erase(itr);
			continue;
		}
#else
		if(itr->first->event_GetCurrentInstanceId() != m_Unit->event_GetCurrentInstanceId() || !itr->first->isAlive() || !isAttackable(m_Unit, itr->first))
		{
			m_aiTargets.erase(itr);
			continue;
		}
#endif
*/
		// this is a much slower version then the previous one but it causes a lot of crashes and that is above speed right now.
		Unit *ai_t = m_Unit->GetMapMgr()->GetUnit( itr->first );

		if( !ai_t || ai_t->GetInstanceID() != m_Unit->GetInstanceID() || !ai_t->isAlive() 
			|| !isAttackable( m_Unit, ai_t ) 
			|| !((ai_t)->m_phase & m_Unit->m_phase)
			)
		{
			if( GetNextTarget() == ai_t )
				SetNextTarget( (Unit*)NULL );

			m_aiTargets.erase(itr);
			continue;
		}

		if((itr->second + ai_t->GetThreatModifyer()) > currentTarget.second)
		{
			/* new target */
			currentTarget.first = ai_t;
			currentTarget.second = itr->second + ai_t->GetThreatModifyer();
			m_currentHighestThreat = currentTarget.second;
		}

		/* there are no more checks needed here... the needed checks are done by CheckTarget() */
	}

	LockAITargets(false);

	return currentTarget.first;
}
Unit *AIInterface::GetSecondHated()
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if(  m_Unit->GetMapMgr() == NULL )
	{ 
		return NULL; 
	}

	Unit *ResultUnit=GetMostHated();

	pair<Unit*, int32> currentTarget;
	currentTarget.first = 0;
	currentTarget.second = -1;

	LockAITargets(true);

	TargetMap::iterator it2 = m_aiTargets.begin();
	TargetMap::iterator itr;
	for(; it2 != m_aiTargets.end();)
	{
		itr = it2;
		++it2;

		/* check the target is valid */
		Unit *ai_t = m_Unit->GetMapMgr()->GetUnit( itr->first );
		if(!ai_t || ai_t->GetInstanceID() != m_Unit->GetInstanceID() || !ai_t->isAlive() || !isAttackable(m_Unit, ai_t)
			|| !((ai_t)->m_phase & m_Unit->m_phase)
			)
		{
			if( ai_t )
				ai_t->CombatStatus.BreakCombatLink( m_Unit );
			m_aiTargets.erase(itr);
			continue;
		}

		if((itr->second + ai_t->GetThreatModifyer()) > currentTarget.second &&
			ai_t != ResultUnit)
		{
			/* new target */
			currentTarget.first = ai_t;
			currentTarget.second = itr->second + ai_t->GetThreatModifyer();
			m_currentHighestThreat = currentTarget.second;
		}
	}

	LockAITargets(false);

	return currentTarget.first;
}
/*
bool AIInterface::modThreatByGUID(uint64 guid, int32 mod)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if (!m_aiTargets.size())
	{ 
		return false;
	}

	if(  m_Unit->GetMapMgr() == NULL )
	{ 
		return false; 
	}

	Unit *obj = m_Unit->GetMapMgr()->GetUnit(guid);
	if(obj)
	{ 
		return modThreatByPtr(obj, mod);
	}

	return false;
}*/

bool AIInterface::modThreatByPtr(Unit* obj, int32 mod)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if(!obj)
	{ 
		return false;
	}
	//players can not gain agro unless special state
	if( m_Unit->IsPlayer() )
	{
		return false;
	}
	//do not agro dead targets. They might seem dead due to scripting 
	if( obj->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_MOUNTED_TAXI | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_DEAD) )
	{
		return false;
	}

	LockAITargets(true);

	int32 tempthreat;
	TargetMap::iterator it = m_aiTargets.find(obj->GetGUID());
	if(it != m_aiTargets.end())
	{
		it->second += mod;
		if (it->second < 1)
			it->second = 1;

		tempthreat = it->second + obj->GetThreatModifyer();
		if (tempthreat < 1)
			tempthreat = 1;
		if(tempthreat > m_currentHighestThreat)
		{
			// new target!
			if(!GetIsTaunted())
			{
				m_currentHighestThreat = tempthreat;
				SetNextTarget(obj);
			}
		}
	}
	else
	{
		m_aiTargets.insert( make_pair( obj->GetGUID(), mod ) );

		tempthreat = mod + obj->GetThreatModifyer();
		if (tempthreat < 1)
			tempthreat = 1;
		if(tempthreat > m_currentHighestThreat)
		{
			if(!GetIsTaunted())
			{
				m_currentHighestThreat = tempthreat;
				SetNextTarget(obj);
			}
		}
	}

	LockAITargets(false);

	if(obj == GetNextTarget())
	{
		// check for a possible decrease in threat.
		if(mod < 0)
		{
			SetNextTarget(GetMostHated());
			//if there is no more new targets then we can walk back home ?
			if(!GetNextTarget())
				HandleEvent(EVENT_LEAVECOMBAT, m_Unit, 0);
		}
	}
	return true;
}

void AIInterface::RemoveThreatByPtr(Unit* obj)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if(!obj)
	{ 
		return;
	}

	LockAITargets(true);

	TargetMap::iterator it = m_aiTargets.find(obj->GetGUID());
	if(it != m_aiTargets.end())
	{
		obj->CombatStatus.BreakCombatLink( m_Unit );
		m_aiTargets.erase(it);
		//check if we are in combat and need a new target
		if(obj==GetNextTarget())
		{
			SetNextTarget(GetMostHated());
			//if there is no more new targets then we can walk back home ?
			if(!GetNextTarget())
				HandleEvent(EVENT_LEAVECOMBAT, m_Unit, 0);
		}
	}

	LockAITargets(false);
}

/*
void AIInterface::addAssistTargets(Unit* Friend)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	// stop adding stuff that gives errors on linux!
	m_assistTargets.insert(Friend->GetGUID());
}*/

void AIInterface::WipeHateList()
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	for(TargetMap::iterator itr = m_aiTargets.begin(); itr != m_aiTargets.end(); ++itr)
		itr->second = 0;
	m_currentHighestThreat = 0;
}
void AIInterface::ClearHateList() //without leaving combat
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	for(TargetMap::iterator itr = m_aiTargets.begin(); itr != m_aiTargets.end(); ++itr)
		itr->second = 1;
	m_currentHighestThreat = 1;
}

void AIInterface::WipeTargetList()
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	SetNextTarget( (Unit*)NULL );

	m_nextSpell = NULL;
	m_currentHighestThreat = 0;
	ClearHateList();	//send to players that we are not agroing them
	LockAITargets( true );
	m_aiTargets.clear();
	LockAITargets( false );
	m_Unit->CombatStatus.ClearAttackers();
	tauntedBy = NULL;
}

bool AIInterface::taunt(Unit* caster, bool apply)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if(apply)
	{
		//wowwiki says that we cannot owerride this spell
		if(GetIsTaunted())
		{ 
			return false;
		}

		if(!caster)
		{
			return false;
		}

		//check if we can attack taunter. Maybe it's a hack or a bug if we fail this test
		if(isAttackable(m_Unit, caster))
		{
			//check if we have to add him to our agro list
			//GetMostHated(); //update our most hated list/ Note that at this point we do not have a taunter yet. If we would have then this funtion will not give real mosthated
			int32 oldthreat = getThreatByPtr(caster);
			//make sure we rush the target anyway. Since we are not tauted yet, this will also set our target
			modThreatByPtr(caster,abs(m_currentHighestThreat-oldthreat)+1); //we need to be the most hated at this moment
//			SetNextTarget(caster);
		}
		tauntedBy = caster;
	}
	else
	{
		tauntedBy = NULL;
		//taunt is over, we should get a new target based on most hated list
		SetNextTarget(GetMostHated());
	}

	return true;
}

Unit* AIInterface::getTauntedBy()
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if(GetIsTaunted())
	{
		return tauntedBy;
	}
	else
	{
		return NULL;
	}
}

bool AIInterface::GetIsTaunted()
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE

	if( tauntedBy )
	{	
		if( tauntedBy->isAlive() )
			return true;
		tauntedBy = NULL;
	}

	return false;
}
/*
void AIInterface::SetSoulLinkedWith(Unit* target)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if (!target)
	{ 
		return;
	}
	soullinkedWith = target;
	isSoulLinked = true;
}

Unit* AIInterface::getSoullinkedWith()
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if(GetIsTaunted())
	{
		return soullinkedWith;
	}
	else
	{
		return NULL;
	}
}

bool AIInterface::GetIsSoulLinked()
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if(isSoulLinked)
	{
		if(!soullinkedWith || !soullinkedWith->isAlive())
		{
			isSoulLinked = false;
			soullinkedWith = NULL;
		}
	}
	return isSoulLinked;
}*/

void AIInterface::EventRemoveInrangeUnit(Unit* target)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if( target == NULL )
	{ 
		return;
	}

	if( target->GetGUID() == UnitToFollowGUID )		  // fix for crash here
	{
		UnitToFollowGUID = 0;
//		m_lastFollowX = m_lastFollowY = 0;
//		FollowDistance = 0;
	}

//	if( target->GetGUID() == UnitToFollow_backup )
//		UnitToFollow_backup = NULL;

/*	AssistTargetSet::iterator  itr = m_assistTargets.find(target->GetGUID());
	if(itr != m_assistTargets.end())
		m_assistTargets.erase(itr);
		*/


	LockAITargets(true);

	TargetMap::iterator it2 = m_aiTargets.find( target->GetGUID() );
	if( it2 != m_aiTargets.end() || target == GetNextTarget() )
	{
		target->CombatStatus.BreakCombatLink( m_Unit );

		if(it2 != m_aiTargets.end())
		{
			m_aiTargets.erase(it2);
		}

		if (target == GetNextTarget())	 // no need to cast on these.. mem addresses are still the same
		{
			SetNextTarget( (Unit*)NULL );
			m_nextSpell = NULL;

			// find the one with the next highest threat
			GetMostHated();
		}
	}

	LockAITargets(false);

	if( target->GetTypeId() == TYPEID_UNIT )
	{
		it2 = target->GetAIInterface()->m_aiTargets.find( m_Unit->GetGUID() );
		if( it2 != target->GetAIInterface()->m_aiTargets.end() )
		{
			target->CombatStatus.BreakCombatLink( m_Unit );
			target->GetAIInterface()->LockAITargets(true);
			target->GetAIInterface()->m_aiTargets.erase( it2 );
			target->GetAIInterface()->LockAITargets(false);
		}
        
		if( target->GetAIInterface()->GetNextTarget() == m_Unit )
		{
			target->GetAIInterface()->SetNextTarget( (Unit*)NULL );
			target->GetAIInterface()->m_nextSpell = NULL;
			target->GetAIInterface()->GetMostHated();
		}

		if( target->GetAIInterface()->UnitToFollowGUID == m_Unit->GetGUID() )
			target->GetAIInterface()->UnitToFollowGUID = 0;
	}

	if(target == UnitToFear)
		UnitToFear = NULL;

	if(tauntedBy == target)
		tauntedBy = NULL;
}

uint32 AIInterface::_CalcThreat(uint32 damage, SpellEntry * sp, Unit* Attacker)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if(!Attacker || (sp && sp->ThreatForSpellCoef == 0) )
		return 0; // No attacker means no threat and we prevent crashes this way

	if (isSameFaction(m_Unit,Attacker))
	{ 
		return 0;
	}

	int32 mod = damage;
	if( sp != NULL )
		mod += sp->ThreatForSpell;

	mod += Attacker->GetThreatModifyer();

	if( sp != NULL )
		mod += int32(mod * sp->ThreatForSpellCoef);

	if( sp != NULL && Need_SM_Apply(sp) && Attacker && Attacker->SM_Mods )
	{
		SM_FIValue(Attacker->SM_Mods->SM_FThreat,&mod,sp->GetSpellGroupType());
		SM_PIValue(Attacker->SM_Mods->SM_PThreat,&mod,sp->GetSpellGroupType());
	}

	static const float ThreathModClass[PLAYER_CLASS_TYPE_COUNT]={0.0f,1.7f,1.5f,0.3f,0.3f,0.3f,1.5f,1.0f,0.3f,0.3f,0.0f,1.2f};
	uint32 AtCl = Attacker->getClass();
	if( AtCl < PLAYER_CLASS_TYPE_COUNT )
		mod = int32(ThreathModClass[ AtCl ] * mod);
	if( mod > 35000 )
		mod = 35000;	//cap it, maybe some bugged spells here ?
//	if (Attacker->getClass() == ROGUE)
//		mod = int32(mod * 0.71); // Rogues generate 0.71x threat per damage.

	// modify threat by Buffs
	if (sp != NULL)
		mod += (mod * Attacker->GetGeneratedThreatModifyerPCT( sp->School ) / 100);
	else
		mod += (mod * Attacker->GetGeneratedThreatModifyerPCT( SCHOOL_NORMAL ) / 100);

	if (mod < 1)
		mod = 1;

	return mod;
}

void AIInterface::WipeReferences()
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	m_nextSpell = 0;
	m_currentHighestThreat = 0;
	ClearHateList();	//tell players we are not attacking them
	LockAITargets(true);
	m_aiTargets.clear();
	LockAITargets(false);
	SetNextTarget( (Unit*)NULL );
	UnitToFear = 0;
	UnitToFollowGUID = 0;
	tauntedBy = 0;
	m_Unit->AquireInrangeLock(); //make sure to release lock before exit function !
	//Clear targettable
	InrangeLoopExitAutoCallback AutoLock;
	for(InRangeSetRecProt::iterator itr = m_Unit->GetInRangeSetBegin( AutoLock ); itr != m_Unit->GetInRangeSetEnd(); ++itr)
		if( (*itr) && (*itr)->GetTypeId() == TYPEID_UNIT && SafeUnitCast(*itr)->GetAIInterface())
			SafeUnitCast(*itr)->GetAIInterface()->RemoveThreatByPtr( m_Unit );
	m_Unit->ReleaseInrangeLock();

}

void AIInterface::ResetProcCounts()
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	//amg stop putting AI agent spells for pets so owner can exploit it with WPE and stuff
	if( m_Unit->IsPet() )
		m_spells.clear( 1 );
	SimplePointerListNode<AI_Spell> *itr;
	for(itr = m_spells.begin(); itr != m_spells.end();itr = itr->next)
	{
		itr->data->procCounter = 0;
		itr->data->cooldowntime = 0;//reset initial cooldown like enrage
	}
}
/*
//we only cast once a spell and we will set his health and resistances. Note that this can be made with db too !
void AIInterface::Event_Summon_EE_totem(uint32 summon_duration)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	//some say it should inherit the level of the caster
	Unit *caster = m_Unit->GetMapMgr()->GetUnit( m_Unit->GetUInt64Value( UNIT_FIELD_CREATEDBY ) );
//	uint32 new_level = 0;
//	if( caster )
//		new_level = caster->getLevel( );
	//timer should not reach this value thus not cast this spell again
	spell_global_cooldown_stamp = 0xEFFFFFFF;
	//creatures do not support PETs and the spell uses that effect so we force a summon guardian thing
//	Unit *ourslave=m_Unit->create_guardian(15352,summon_duration,float(-M_PI*2), new_level );
	Unit *ourslave=m_Unit->create_guardian( 15352, summon_duration );
	if(ourslave)
	{
		SafeCreatureCast(ourslave)->ResistanceModPct[NATURE_DAMAGE]=100;//we should be imune to nature dmg. This can be also set in db
		SafeCreatureCast(ourslave)->BaseAttackType = SCHOOL_NATURE;
//		SafeCreatureCast(ourslave)->m_noRespawn = true;
		//- Earth Stun (37982)
		//- taunt
        // we want the elemental to have the same pvp flag as the shaman who popped the totem
        if( caster->IsPvPFlagged() )
            ourslave->SetPvPFlag();
        if( caster->IsFFAPvPFlagged() )
            ourslave->SetFFAPvPFlag();
	}
}

//we only cast once a spell and we will set his health and resistances. Note that this can be made with db too !
void AIInterface::Event_Summon_FE_totem(uint32 summon_duration)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	//some say it should inherit the level of the caster
	Unit *caster = m_Unit->GetMapMgr()->GetUnit( m_Unit->GetUInt64Value( UNIT_FIELD_CREATEDBY ) );
//	uint32 new_level = 0;
//	if( caster )
//		new_level = caster->getLevel( );
	//timer should not reach this value thus not cast this spell again
	spell_global_cooldown_stamp = 0xEFFFFFFF;
	//creatures do not support PETs and the spell uses that effect so we force a summon guardian thing
//	Unit *ourslave=m_Unit->create_guardian(15438,summon_duration,float(-M_PI*2), new_level);
	Unit *ourslave=m_Unit->create_guardian( 15438, summon_duration );
	if(ourslave)
	{
		//m_Unit->summonPet = ourslave;
		SafeCreatureCast(ourslave)->ResistanceModPct[FIRE_DAMAGE]=100;//we should be imune to fire dmg. This can be also set in db
		SafeCreatureCast(ourslave)->BaseAttackType = SCHOOL_FIRE;
//		SafeCreatureCast(ourslave)->m_noRespawn = true;
//		- also : select * from dbc_spell where name like "%fire blast%"
//		- also : select * from dbc_spell where name like "%fire nova"
	}
}
*/
/*
void AIInterface::CancelSpellCast()
{
	//hmm unit spell casting is not the same as Ai spell casting ? Have to test this
	if(m_Unit->isCasting())
		m_Unit->m_currentSpell->safe_cancel();
	//i can see this crashing already :P.
	m_AIState = STATE_IDLE;
}
*/

void AIInterface::EventChangeFaction( Unit *ForceAttackersToHateThisInstead )
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	m_nextSpell = 0;
	m_currentHighestThreat = 0;
	//we need a new hatred list
	ClearHateList();
	LockAITargets(true);
	m_aiTargets.clear();
	LockAITargets(false);
	//we need a new assist list
//	m_assistTargets.clear();
	//Clear targettable
	if( ForceAttackersToHateThisInstead == NULL )
	{
		m_Unit->AquireInrangeLock(); //make sure to release lock before exit function !
		InrangeLoopExitAutoCallback AutoLock;
		for(InRangeSetRecProt::iterator itr = m_Unit->GetInRangeSetBegin( AutoLock ); itr != m_Unit->GetInRangeSetEnd(); ++itr)
			if( (*itr) && (*itr)->GetTypeId() == TYPEID_UNIT && SafeUnitCast(*itr)->GetAIInterface() )
				SafeUnitCast(*itr)->GetAIInterface()->RemoveThreatByPtr( m_Unit );
		m_Unit->ReleaseInrangeLock();
		SetNextTarget( (Unit*)NULL );
	}
	else
	{
		m_Unit->AquireInrangeLock(); //make sure to release lock before exit function !
		InrangeLoopExitAutoCallback AutoLock;
		for(InRangeSetRecProt::iterator itr = m_Unit->GetInRangeSetBegin( AutoLock ); itr != m_Unit->GetInRangeSetEnd(); ++itr)
			if( (*itr) && (*itr)->GetTypeId() == TYPEID_UNIT && SafeUnitCast(*itr)->GetAIInterface() 
				&& SafeUnitCast(*itr)->GetAIInterface()->getThreatByPtr( m_Unit ) )//this guy will join me in fight since i'm telling him "sorry i was controlled"
			{
				SafeUnitCast(*itr)->GetAIInterface()->modThreatByPtr( ForceAttackersToHateThisInstead, 10 ); //just aping to be bale to hate him in case we got nothing else
				SafeUnitCast(*itr)->GetAIInterface()->RemoveThreatByPtr( m_Unit );
			}
		m_Unit->ReleaseInrangeLock();
		modThreatByPtr( ForceAttackersToHateThisInstead, 1 );
		SetNextTarget( ForceAttackersToHateThisInstead );
	}
}

bool isGuard(uint32 id)
{
	switch(id)
	{
		/* stormwind guards */
	case 68:
	case 1423:
	case 1756:
	case 15858:
	case 15859:
	case 16864:
	case 20556:
	case 18948:
	case 18949:
	case 1642:
		/* ogrimmar guards */
	case 3296:
	case 15852:
	case 15853:
	case 15854:
	case 18950:
		/* undercity guards */
	case 5624:
	case 18971:
	case 16432:
		/* exodar */
	case 16733:
	case 18815:
		/* thunder bluff */
	case 3084:
		/* silvermoon */
	case 16221:
	case 17029:
	case 16222:
		/* ironforge */
	case 727:
	case 5595:
	case 12996:
		/* darnassus? */
		{
			return true;
		}break;
	}
	return false;
}

bool isNeutralGuard(uint32 id)
{
	switch(id)
	{
		// Ratchet
	case 3502:
		// Booty Bay
	case 4624:
	case 15088:
		// Gadgetzan
	case 9460:
		// Argent Dawn
	case 11102:
	case 16378:
		// Cenarion Hold
	case 15184:
		// Moonglade
	case 11822:
		// Everlook
	case 11190:
		// Cenarion Refuge
	case 17855:
		// Throne of the elements
	case 18099:
	case 18101:
	case 18102:
		// Area 52
	case 20484:
	case 20485:
		// Cosmowrench
	case 22494:
		// Mudsprocket
	case 23636:
		// Concert Bruiser
	case 23721:
		// Shattered Sun
	case 26253:
	case 24994:
			return true;
		break;
	}

	return false;
}

void AIInterface::WipeCurrentTarget()
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	LockAITargets(true);
	Unit *ut = GetNextTarget();
	TargetMap::iterator itr = m_aiTargets.find( ut ? ut->GetGUID() : 0 );
	if( itr != m_aiTargets.end() )
	{
		m_aiTargets.erase( itr );
		ut->CombatStatus.BreakCombatLink( m_Unit );
	}
	LockAITargets(false);

	SetNextTarget( (Unit*)NULL );

//	if( GetNextTarget() == UnitToFollow )
//		UnitToFollow = NULL;

//	if( GetNextTarget() == UnitToFollow_backup )
//		UnitToFollow_backup = NULL;
}

#ifdef HACKY_CRASH_FIXES

bool AIInterface::CheckCurrentTarget()
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	bool cansee = false;

	//in case target was removed from map since our last check on him
	Unit *NextTarget = GetNextTarget();
	if( NextTarget == NULL )
	{
		WipeCurrentTarget();
		return false;
	}
	if( NextTarget && NextTarget->GetInstanceID() == m_Unit->GetInstanceID())
	{
		if( m_Unit->GetTypeId() == TYPEID_UNIT )
			cansee = SafeCreatureCast( m_Unit )->CanSee( NextTarget );
		else
			cansee = SafePlayerCast( m_Unit )->CanSee( NextTarget );
	}
	else 
	{
		WipeCurrentTarget();
	}

	return cansee;
}
/*
bool AIInterface::TargetUpdateCheck(Unit * ptr)
{
	__try
	{
		if( ptr->event_GetInstanceID() != m_Unit->event_GetInstanceID() ||
			!ptr->isAlive() || m_Unit->GetDistanceSq(ptr) >= 6400.0f )
		{
			return false;
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}

	return true;
}
*/
#endif
Unit* AIInterface::GetNextTarget()
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if (m_nextTarget && m_Unit && m_Unit->GetMapMgr()) 
	{ 
		return m_Unit->GetMapMgr()->GetUnit(m_nextTarget);
	}
	{ 
		return NULL;
	}
}

void AIInterface::SetNextTarget (Unit *nextTarget) 
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if (nextTarget)
		SetNextTarget(nextTarget->GetGUID());
	else
		SetNextTarget((uint64)NULL);
}

void AIInterface::SetNextTarget (uint64 nextTarget) 
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	m_nextTarget = nextTarget; 
	if( disable_targeting == false )
		m_Unit->SetUInt64Value(UNIT_FIELD_TARGET, m_nextTarget);
	if(nextTarget)
	{
#ifdef ENABLE_GRACEFULL_HIT
		have_graceful_hit=false;
#endif
	}
}

void AIInterface::SetWaypointMap(WayPointMap * m)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if( !m )
	{ 
		return;
	}
	if( m_custom_waypoint_map )
	{
		delete m_custom_waypoint_map;
		m_custom_waypoint_map = NULL;
	}
	m_custom_waypoint_map = new WayPointVect( (uint32)m->size() );
	for(WayPointMap::iterator itr = m->begin(); itr != m->end(); ++itr)
		m_custom_waypoint_map->push_back( *itr );
	SetWaypointMap( m_custom_waypoint_map );
}

void AIInterface::HandleChainAggro(Unit *u)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	if (!m_Unit->IsInWorld() || !m_Unit->isAlive() )
	{ 
		return;
	}

	if( m_Unit->m_chain == NULL )
	{
		return;
	}
	for (std::set<Unit*>::iterator itr=m_Unit->m_chain->m_units.begin(); itr!=m_Unit->m_chain->m_units.end(); ++itr)
	{
		if (!(*itr)->IsInWorld() || !(*itr)->isAlive() || (m_Unit->m_chain->m_chainrange != 0 && m_Unit->CalcDistance(*itr) > m_Unit->m_chain->m_chainrange))
			continue;
		
		if ((*itr)->GetAIInterface()->GetAITargets()->find(u->GetGUID()) == (*itr)->GetAIInterface()->GetAITargets()->end())
		{
			if((*itr)->GetAIInterface()->getAIState() == STATE_IDLE || (*itr)->GetAIInterface()->getAIState() == STATE_FOLLOWING)
				(*itr)->GetAIInterface()->HandleEvent(EVENT_ENTERCOMBAT, u, 0);
			else
				(*itr)->GetAIInterface()->GetAITargets()->insert(TargetMap::value_type(u->GetGUID(), 1));
		}
	}
}

Unit* AIInterface::FindTargetForSpellTargetType(AI_Spell *sp)
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
   uint32 targettype = sp->spelltargetType;
   switch(targettype)
   {
   case TTYPE_CASTER:		
	return m_Unit;
   case TTYPE_SINGLETARGET:	
	return GetNextTarget();
   case TTYPE_SOURCE:		
	return m_Unit;
   case TTYPE_OWNER:	
	   {
		   Unit *t = m_Unit->GetMapMgr()->GetUnit( m_Unit->GetUInt64Value( UNIT_FIELD_CREATEDBY ) );
		   if( t )
		   { 
			   return t;
		   }
		   return m_Unit;
	   }
   case TTYPE_SECOND_MOST_HATED:	
	return GetSecondHated();
   case TTYPE_HEALER:	
	   {
		   float min_radius = sp->minrange;
		   float max_radius = sp->maxrange;
		   InRangeSetRecProt::iterator itr;
			m_Unit->AquireInrangeLock(); //make sure to release lock before exit function !
			InrangeLoopExitAutoCallback AutoLock;
			for( itr = m_Unit->GetInRangeSetBegin( AutoLock ); itr != m_Unit->GetInRangeSetEnd(); itr++ )
			{

				if(!((*itr)->IsUnit()) || !SafeUnitCast((*itr))->isAlive())
					continue;

				uint8 unit_class = SafeUnitCast((*itr))->getClass();
				if( unit_class != PALADIN && unit_class != PRIEST && unit_class != SHAMAN && unit_class != DRUID )
					continue;

				float distance = m_Unit->CalcDistance( (*itr) );
				//is Creature in range
				if( distance < min_radius || distance > max_radius )
					continue;

				if( !isAttackable( m_Unit, SafeUnitCast(*itr)) )
					continue;

				//found a target 
				m_Unit->ReleaseInrangeLock();
				return SafeUnitCast(*itr);
			}
			m_Unit->ReleaseInrangeLock();
			return NULL; //no target :(
	   }
   case TTYPE_TANK:
	   {
		   float min_radius = sp->minrange;
		   float max_radius = sp->maxrange;
		   InRangeSetRecProt::iterator itr;
			m_Unit->AquireInrangeLock(); //make sure to release lock before exit function !
			InrangeLoopExitAutoCallback AutoLock;
			for( itr = m_Unit->GetInRangeSetBegin( AutoLock ); itr != m_Unit->GetInRangeSetEnd(); itr++ )
			{

				if(!((*itr)->IsUnit()) || !SafeUnitCast(*itr)->isAlive())
					continue;

				uint8 unit_class = SafeUnitCast(*itr)->getClass();
				if( unit_class != PALADIN && unit_class != WARRIOR && unit_class != DRUID )
					continue;

				float distance = m_Unit->CalcDistance( (*itr) );
				//is Creature in range
				if( distance < min_radius || distance > max_radius )
					continue;

				if( !isAttackable( m_Unit, SafeUnitCast(*itr)) )
					continue;

				//found a target 
				m_Unit->ReleaseInrangeLock();
				return SafeUnitCast(*itr);
			}
			m_Unit->ReleaseInrangeLock();
			return NULL; //no target :(
	   }
   case TTYPE_DPS:
	   {
		   float min_radius = sp->minrange;
		   float max_radius = sp->maxrange;
		   InRangeSetRecProt::iterator itr;
			m_Unit->AquireInrangeLock(); //make sure to release lock before exit function !
			InrangeLoopExitAutoCallback AutoLock;
			for( itr = m_Unit->GetInRangeSetBegin( AutoLock ); itr != m_Unit->GetInRangeSetEnd(); itr++ )
			{

				if(!((*itr)->IsUnit()) || !SafeUnitCast(*itr)->isAlive())
					continue;

				uint8 unit_class = SafeUnitCast(*itr)->getClass();
				if( unit_class != HUNTER && unit_class != ROGUE && unit_class != MAGE && unit_class != DEATHKNIGHT && unit_class != WARLOCK )
					continue;

				float distance = m_Unit->CalcDistance( (*itr) );
				//is Creature in range
				if( distance < min_radius || distance > max_radius )
					continue;

				if( !isAttackable( m_Unit, SafeUnitCast(*itr)) )
					continue;

				//found a target 
				m_Unit->ReleaseInrangeLock();
				return SafeUnitCast(*itr);
			}
			m_Unit->ReleaseInrangeLock();
			return NULL; //no target :(
	   }   case TTYPE_DESTINATION:	return GetNextTarget();
   case TTYPE_AT_RANGE:
	   {
		   float min_radius = sp->minrange;
		   float max_radius = sp->maxrange;
		   InRangeSetRecProt::iterator itr;
			m_Unit->AquireInrangeLock(); //make sure to release lock before exit function !
			InrangeLoopExitAutoCallback AutoLock;
			for( itr = m_Unit->GetInRangeSetBegin( AutoLock ); itr != m_Unit->GetInRangeSetEnd(); itr++ )
			{

				if(!((*itr)->IsUnit()) || !SafeUnitCast(*itr)->isAlive())
					continue;

				float distance = m_Unit->CalcDistance( (*itr) );
				//is Creature in range
				if( distance < min_radius || distance > max_radius )
					continue;

				if( !isAttackable( m_Unit, SafeUnitCast(*itr)) )
					continue;

				if( RandChance( sp->procChance ) == false )
					continue;

				//found a target 
				m_Unit->ReleaseInrangeLock();
				return SafeUnitCast(*itr);
			}
			m_Unit->ReleaseInrangeLock();
			return NULL; //no target :(
	   }
   case TTYPE_AT_RANGE_PLAYER:
	   {
		   float min_radius = sp->minrange;
		   float max_radius = sp->maxrange;
		   InRangePlayerSetRecProt::iterator itr;
			m_Unit->AquireInrangeLock(); //make sure to release lock before exit function !
			InrangeLoopExitAutoCallback AutoLock;
			for( itr = m_Unit->GetInRangePlayerSetBegin( AutoLock ); itr != m_Unit->GetInRangePlayerSetEnd(); itr++ )
			{
				if(!((*itr)->IsUnit()) || !SafeUnitCast(*itr)->isAlive())
					continue;

				float distance = m_Unit->CalcDistance( (*itr) );
				//is Creature in range
				if( distance < min_radius || distance > max_radius )
					continue;

				if( !isAttackable( m_Unit, SafeUnitCast(*itr)) )
					continue;

				if( RandChance( sp->procChance ) == false )
					continue;

				//found a target 
				m_Unit->ReleaseInrangeLock();
				return SafeUnitCast(*itr);
			}
			m_Unit->ReleaseInrangeLock();
			return NULL; //no target :(
	   }
   case TTYPE_AT_RANGE_PLAYER_RANDOM:
	   {
		   float min_radius = sp->minrange;
		   float max_radius = sp->maxrange;
		   uint32 SuitableTargets = 0;
		   InRangePlayerSetRecProt::iterator itr;
			m_Unit->AquireInrangeLock(); //make sure to release lock before exit function !
			InrangeLoopExitAutoCallback AutoLock;
			for( itr = m_Unit->GetInRangePlayerSetBegin( AutoLock ); itr != m_Unit->GetInRangePlayerSetEnd(); itr++ )
			{
				if(!((*itr)->IsUnit()) || !SafeUnitCast(*itr)->isAlive())
					continue;

				float distance = m_Unit->CalcDistance( (*itr) );
				//is Creature in range
				if( distance < min_radius || distance > max_radius )
					continue;

				if( !isAttackable( m_Unit, SafeUnitCast(*itr)) )
					continue;

				SuitableTargets++;
			}
			m_Unit->ReleaseInrangeLock();

			if( SuitableTargets <= 0 )
				return NULL;

			int32 SelectedLocation;
//			if( SuitableTargets == 1 )
//				SelectedLocation = 0; 
//			else
				SelectedLocation = RandomUInt( SuitableTargets - 1 );
			m_Unit->AquireInrangeLock(); //make sure to release lock before exit function !
			for( itr = m_Unit->GetInRangePlayerSetBegin( AutoLock ); itr != m_Unit->GetInRangePlayerSetEnd(); itr++ )
			{
				if(!((*itr)->IsUnit()) || !SafeUnitCast(*itr)->isAlive())
					continue;

				float distance = m_Unit->CalcDistance( (*itr) );
				//is Creature in range
				if( distance < min_radius || distance > max_radius )
					continue;

				if( !isAttackable( m_Unit, SafeUnitCast(*itr)) )
					continue;

				SelectedLocation--;
				if( SelectedLocation <= 0 )
				{
					m_Unit->ReleaseInrangeLock();
					return SafeUnitCast(*itr);
				}
			}
			m_Unit->ReleaseInrangeLock();
			return NULL; //no target :(
	   }
   case TTYPE_ENEMY_FOR_BUFF:
	   {
		   float min_radius = sp->minrange;
		   float max_radius = sp->maxrange;
		   InRangePlayerSetRecProt::iterator itr;
			m_Unit->AquireInrangeLock(); //make sure to release lock before exit function !
			InrangeLoopExitAutoCallback AutoLock;
			for( itr = m_Unit->GetInRangePlayerSetBegin( AutoLock ); itr != m_Unit->GetInRangePlayerSetEnd(); itr++ )
			{

				if(!((*itr)->IsUnit()) || !SafeUnitCast(*itr)->isAlive())
					continue;

				float distance = m_Unit->CalcDistance( (*itr) );
				//is Creature in range
				if( distance < min_radius || distance > max_radius )
					continue;

				if( !isAttackable( m_Unit, SafeUnitCast(*itr)) )
					continue;

				if( RandChance( sp->procChance ) == false )
					continue;

				//check if target has buff
				if( SafeUnitCast(*itr)->HasAura( sp->spell->Id ) )
					continue;

				//found a target 
				m_Unit->ReleaseInrangeLock();
				return SafeUnitCast(*itr);
			}
			m_Unit->ReleaseInrangeLock();
			return NULL; //no target :(
	   }
   case TTYPE_FRIENDLY_FOR_BUFF:
	   {
		   float min_radius = sp->minrange;
		   float max_radius = sp->maxrange;
		   InRangeSetRecProt::iterator itr;
			m_Unit->AquireInrangeLock(); //make sure to release lock before exit function !
			InrangeLoopExitAutoCallback AutoLock;
			for( itr = m_Unit->GetInRangeSetBegin( AutoLock ); itr != m_Unit->GetInRangeSetEnd(); itr++ )
			{

				if(!((*itr)->IsUnit()) || !SafeUnitCast(*itr)->isAlive())
					continue;

				float distance = m_Unit->CalcDistance( (*itr) );
				//is Creature in range
				if( distance < min_radius || distance > max_radius )
					continue;

				if( isAttackable( m_Unit, SafeUnitCast(*itr)) )
					continue;

				//check if target has buff
				if( SafeUnitCast(*itr)->HasAura( sp->spell->Id ) )
					continue;

				//found a target 
				m_Unit->ReleaseInrangeLock();
				return SafeUnitCast(*itr);
			}
			m_Unit->ReleaseInrangeLock();

			//consider selecting ourself
			if( !m_Unit->HasAura( sp->spell->Id ) )
			{ 
				return m_Unit;
			}

			return NULL; //no target :(
	   }   
   case TTYPE_FRIENDLY_TO_HEAL:
	   {
		   float min_radius = sp->minrange;
		   float max_radius = sp->maxrange;
		   InRangeSetRecProt::iterator itr;
			m_Unit->AquireInrangeLock(); //make sure to release lock before exit function !
			InrangeLoopExitAutoCallback AutoLock;
			for( itr = m_Unit->GetInRangeSetBegin( AutoLock ); itr != m_Unit->GetInRangeSetEnd(); itr++ )
			{

				if(!((*itr)->IsUnit()) || !SafeUnitCast(*itr)->isAlive())
					continue;

				float distance = m_Unit->CalcDistance( (*itr) );
				//is Creature in range
				if( distance < min_radius || distance > max_radius )
					continue;

				if( isAttackable( m_Unit, SafeUnitCast(*itr) ) )
					continue;

				uint32 target_hp = SafeUnitCast(*itr)->GetHealthPct();
				if( target_hp < sp->min_hp_pct || target_hp > sp->max_hp_pct )
					continue;

				//check if target has buff
				if( (sp->spell->c_is_flags & SPELL_FLAG_IS_BUFF) && SafeUnitCast(*itr)->HasAura( sp->spell->Id ) )
					continue;

				//found a target 
				m_Unit->ReleaseInrangeLock();
				return SafeUnitCast(*itr);
			}
			m_Unit->ReleaseInrangeLock();
			//consider selecting ourself
			uint32 target_hp = m_Unit->GetHealthPct();
			if( target_hp > sp->min_hp_pct && target_hp < sp->max_hp_pct )
			{
				//check if target has buff
				if( (sp->spell->c_is_flags & SPELL_FLAG_IS_BUFF)==0 || !m_Unit->HasAura( sp->spell->Id ) )
				{ 
					return m_Unit;
				}
			}
			return NULL; //no target :(
	   }   
   default:
      sLog.outError("AI Agents: Targettype of AI agent spell %u for creature %u not set", sp->spell->Id, SafeCreatureCast( m_Unit )->GetCreatureInfo()->Id );
   }
   return NULL;
}

void AIInterface::_UpdateThreathList()
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	//sanity check
	if(  m_Unit->GetMapMgr() == NULL )
		return; 
	//no need to monitor threath for players i think
	if( m_Unit->IsCreature() == false )
		return;
	Creature *owner = SafeCreatureCast( m_Unit );
	//client is up to date ?
	if( m_currentHighestThreat == owner->m_HighestThreath )
		return;
	//let's not spam the client
	if( getMSTime() < owner->m_NextThreathUpdateStamp )
		return;
	owner->m_NextThreathUpdateStamp = getMSTime() + THREATH_CLIENT_UPDATE_INTERVAL;
	//we are not in combat ?
	Unit *most_hated = GetNextTarget();
	if( most_hated == NULL )
		return;

	//on first hit we send a special version of the packet
/*	
	//seems like there is no need to be blizzlike about this
	if( owner->m_NextThreathUpdateStamp == 0 )
	{
		sStackWorldPacket( data, SMSG_HIGHEST_THREAT_UPDATE, 2*9 + 8 );
		data << owner->GetNewGUID();
		data << uint8( 0 );	//no target guid
		data << uint32( 1 );	//1 target in list
		data << most_hated->GetNewGUID();
		data << hated_targets;
	}/**/
	//update spam protection
	owner->m_HighestThreath = m_currentHighestThreat;

	//build up the packet and send it
	uint32 hated_targets = MIN( (uint32)m_aiTargets.size(), 20 );
	if( hated_targets == 0 )	//wtf check
		return;
	sStackWorldPacket( data, SMSG_HIGHEST_THREAT_UPDATE, 20 * 14 + 18 );

	data << owner->GetNewGUID();
	data << most_hated->GetNewGUID();
	data << hated_targets;

	LockAITargets(true);

	TargetMap::iterator itr;
	for(itr = m_aiTargets.begin(); itr != m_aiTargets.end(); itr++)
	{
		WoWGuid tguid( itr->first );
		data << tguid ;
		data << uint32( itr->second );
		//make sure there is no very strange case of overspawned area
		hated_targets--;
		if( hated_targets == 0 )
			break;
	}
	for(itr = m_aiTargets.begin(); itr != m_aiTargets.end(); itr++)
	{
		Unit *ai_t = m_Unit->GetMapMgr()->GetUnit( itr->first );
		if( ai_t && ai_t->IsPlayer() && SafePlayerCast( ai_t )->GetSession() )
			SafePlayerCast( ai_t )->GetSession()->SendPacket( &data );
	}
	LockAITargets(false);
}

void AIInterface::ClearThreathList()
{
	INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE
	//sanity check
	if(  m_Unit->GetMapMgr() == NULL )
		return; 
	//no need to monitor threath for players i think
	if( m_Unit->IsCreature() == false )
		return;
	Creature *owner = SafeCreatureCast( m_Unit );

	//update spam protection
	owner->m_HighestThreath = 0;

	sStackWorldPacket( data, SMSG_THREAT_CLEAR, 9 );
	data << owner->GetNewGUID();

	//build up the packet and send it
	uint32 hated_targets = MIN( (uint32)m_aiTargets.size(), 20 );
	if( hated_targets == 0 )	//wtf check
	{
		owner->SendMessageToSet( &data, true );
		return;
	}

	LockAITargets(true);
	TargetMap::iterator itr;
	for(itr = m_aiTargets.begin(); itr != m_aiTargets.end(); itr++)
	{
		Unit *ai_t = m_Unit->GetMapMgr()->GetUnit( itr->first );
		if( ai_t && ai_t->IsPlayer() )
		{
			if( SafePlayerCast( ai_t )->GetSession() )
				SafePlayerCast( ai_t )->GetSession()->SendPacket( &data );
			//if unit is not attacking the player then the player has no need to know about the mob either
			ai_t->CombatStatus.BreakCombatLink( m_Unit );
		}
	}
	LockAITargets(false);
}

void AIInterface::SetPetOwner(Unit * owner)
{ 
	if( owner )
		m_PetOwnerGUID = owner->GetGUID(); 
	else
		m_PetOwnerGUID = 0;
}

Unit *AIInterface::GetPetOwner()
{ 
	if( m_PetOwnerGUID == 0 )
		return NULL;
	if( m_Unit->GetMapMgr() == NULL )
		return NULL;
	return m_Unit->GetMapMgr()->GetUnit( m_PetOwnerGUID ); 
}
