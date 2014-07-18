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

int isBgEnemy(Object* objA, Object* objB)
{
	// if objA is in battleground check objB hostile based on teams
	if( ( objA->IsInBg() && (objA->IsPlayer() || objA->IsPet() || ( objA->IsCreature() && SafeCreatureCast( objA )->IsTotem() ) ) ) )
	{
		if ( (objB->IsPlayer() || objB->IsPet() || ( objB->IsCreature() && SafeCreatureCast( objB )->IsTotem() ) ) )
		{
			if ( objB->IsInBg() == false )
			{
				// objA is in battleground, objB no, so return false
				return 0;
			}
			else
			{
				uint32 teamA = objA->GetTeam();
				uint32 teamB = objB->GetTeam();
				if (teamA == (uint32)-1 || teamB == (uint32)-1)
				{
					return 0;
				}

				return teamA != teamB;
			}
		}
	}

	return -1;
}

bool isHostile(Object* objA, Object* objB)// B is hostile for A?
{
	if(!objA || !objB)
		return false;
	bool hostile = false;

	if(objA == objB)
		return false;   // can't attack self.. this causes problems with buffs if we dont have it :p

	if(objA->GetTypeId() == TYPEID_CORPSE)
		return false;

	if(objB->GetTypeId() == TYPEID_CORPSE)
		return false;

	if( !(objA->m_phase & objB->m_phase) ) //What you can't see, can't be hostile!
		return false;

	int ret = isBgEnemy(objA, objB);
	if (ret != -1) 
		return ret == 1;

	// there are areas where fighting is forbidden
	AreaTable *atA;
	AreaTable *atB;

	if( objA->IsPlayer() )
	{
		atA = dbcArea.LookupEntry( SafePlayerCast( objA )->GetAreaID() );
		if( ( atA && atA->AreaFlags & AREA_FLAG_SANCTUARY) )
			return false;
		//check if is same group then friendly. Before faction check
		if( SafePlayerCast( objA )->GetGroup() && objB->IsPlayer() && SafePlayerCast( objA )->GetGroup() == SafePlayerCast( objB )->GetGroup() )
			return false;
	}

	if( objB->IsPlayer() )
	{
		atB = dbcArea.LookupEntry( SafePlayerCast( objB )->GetAreaID() );
		if( atB && ( atB->AreaFlags & AREA_FLAG_SANCTUARY ) )
			return false;
	}

	uint32 faction = objB->m_faction->Mask;
	uint32 host = objA->m_faction->HostileMask;

	if(faction & host)
		hostile = true;

	// check friend/enemy list
	for(uint32 i = 0; i < 4; i++)
	{
		if(objA->m_faction->EnemyFactions[i] == objB->m_faction->Faction)
		{
			hostile = true;
			break;
		}
		if(objA->m_faction->FriendlyFactions[i] == objB->m_faction->Faction)
		{
			hostile = false;
			break;
		}
	}

	// Reputation System Checks
	if(objA->IsPlayer() && !objB->IsPlayer())	   // PvE
	{
		if(objB->m_factionDBC->RepListId >= 0)
			hostile = SafePlayerCast( objA )->IsHostileBasedOnReputation( objB->m_factionDBC );
	}
	
	if(objB->IsPlayer() && !objA->IsPlayer())	   // PvE
	{
		if(objA->m_factionDBC->RepListId >= 0)
			hostile = SafePlayerCast( objB )->IsHostileBasedOnReputation( objA->m_factionDBC );
	}

	return hostile;
}

/// Where we check if we object A can attack object B. This is used in many feature's
/// Including the spell class and the player class.
bool isAttackable(Object* objA, Object* objB, bool CheckStealth)// A can attack B?
{
	if(!objA || !objB 
//		|| objB->m_factionDBC == NULL || objA->m_factionDBC == NULL
		)
		return false;

	if(objA == objB)
		return false;   // can't attack self.. this causes problems with buffs if we don't have it :p

	if(objA->IsCorpse() || objB->IsCorpse() )
		return false;

	//pending state or something
	if( !objA->IsInWorld() || !objB->IsInWorld() )
		return false;

	if( !(objA->m_phase & objB->m_phase) ) //What you can't see, you can't support either...
		return false;

	// Checks for untouchable, unattackable
	if( objA->IsUnit() 
		&& objA->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_MOUNTED_TAXI | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_DEAD) //it's a bug if this tries to attack without scripting
		&& SafeUnitCast( objA )->bInvincible == false )	//in case this is a scripted mob that is invincible and non selectable he should still be able to shoot others
		return false;
	if( objB->IsUnit())
	{
		// do not attack already dead targets, avoid having -1 life bug
		if( objB->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_MOUNTED_TAXI | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_DEAD) //it's a bug if this tries to attack without scripting
//			&& SafeUnitCast( objB )->bInvincible == false //in case this is a scripted mob that is invincible and non selectable he should still be able to shoot others
			)	
			return false;

		/// added by Zack : 
        /// we cannot attack shealthed units. Maybe checked in other places too ?
		/// !! warning, this presumes that objA is attacking ObjB
        /// Capt: Added the possibility to disregard this (regarding the spell class)
		if( SafeUnitCast(objB)->IsStealth() && CheckStealth )
		{
			if( ( objA->IsCreature() && SafeCreatureCast( objA )->CanSee( SafeUnitCast( objB ) ) == false ) || ( objA->IsPlayer() && SafePlayerCast( objA )->CanSee( objB ) == false ) )
				return false;
		}
	}

	//cannot attack players inside vehicle directly. First destroy the vehicle
	if( objB->IsUnit() && SafeUnitCast(objB)->IsVehicleSeatHealthShielded() )
		return false;
	//vehicle is attacking the target and not the player inside it
	if( objA->IsUnit() && SafeUnitCast(objA)->IsVehicleSeatHealthShielded() )
		return false;

	int ret = isBgEnemy(objA, objB);
	if (ret != -1) 
		return ret == 1;

	//grouping any summon and checking only owners -> 99.99999% true
	if( objA->IsUnit() && objB->IsUnit() )
	{
		Unit *topA, *topB;
		topA = SafeUnitCast( objA );
		topB = SafeUnitCast( objB );
		//this is made with loop because. Ex : totems that spawn mobs -> mob -> totem -> owner
		while( topA && topA->GetUInt32Value( UNIT_FIELD_CREATEDBY ) && topA->GetMapMgr() )
		{
			Unit *ttopA = topA->GetMapMgr()->GetUnit( topA->GetUInt64Value( UNIT_FIELD_CREATEDBY ) );
			if( ttopA == topA || ttopA == NULL )
				break;	//someone managed to self create. Met god yet ?
			topA = ttopA;
		}
		while( topB && topB->GetUInt32Value( UNIT_FIELD_CREATEDBY ) && topB->GetMapMgr() )
		{
			Unit *ttopB = topB->GetMapMgr()->GetUnit( topB->GetUInt64Value( UNIT_FIELD_CREATEDBY ) );
			if( ttopB == topB || ttopB == NULL )
				break;	//someone managed to self create. Met god yet ?
			topB = ttopB;
		}

		//really sucks if this gets to be NULL. Means we can boot these guys off the map
		if( topA && topB )
		{
			//units in same group are never hostile. FFA flag is only available for units
			if( topA->IsFFAPvPFlagged() && topB->IsFFAPvPFlagged() && topA != topB )
			{
				if( topB->GetGroup() && topB->GetGroup() == topA->GetGroup() )
					return false;
				return true;	//not in same group means FFA !
			}

			//units dueling : attacker owner is dueling our target owner
			if( topB->IsPlayer() 
				&& SafePlayerCast( topB )->DuelingWith == topA
				&& SafePlayerCast( topB )->GetDuelState() == DUEL_STATE_STARTED )
				return true;

			// do not let people attack each other in sanctuary
			//pets and players should not attack targets that have no PVP flag on atm. PVP flag togles automatically on non creature stuff
			if( topA->IsPlayer() && topB->IsPlayer() )
			{
				//pets and players should not attack targets that have no PVP flag on atm. PVP flag togles automatically on non creature stuff
				if( SafeUnitCast( topB )->IsPvPFlagged() == false )
					return false;
				AreaTable *atA;
				AreaTable *atB;
				atA = dbcArea.LookupEntry( SafePlayerCast( topA )->GetAreaID() );
				atB = dbcArea.LookupEntry( SafePlayerCast( topB )->GetAreaID() );
				if( atA->AreaFlags & AREA_FLAG_SANCTUARY || atB->AreaFlags & AREA_FLAG_SANCTUARY )
					return false;
			}
		}
		else 
			return false;
	}

	if(objA->m_faction == objB->m_faction)  // same faction can't kill each other unless in ffa pvp/duel
		return false;

	bool attackable = isHostile(objA, objB); // B is attackable if its hostile for A
	/*if((objA->m_faction->HostileMask & 8) && (objB->m_factionDBC->RepListId != 0) && 
		(objB->GetTypeId() != TYPEID_PLAYER) && objB->m_faction->Faction != 31) // B is attackable if its a neutral Creature*/

	if( attackable == false )
		attackable = isHostile(objB, objA); 
	// Neutral Creature Check
	if(objA->IsPlayer() || objA->IsPet())
	{
		if(objB->m_factionDBC->RepListId == -1 && objB->m_faction->HostileMask == 0 && objB->m_faction->FriendlyMask == 0)
		{
			attackable = true;
		}
	}
	//avoid scenario when  A can attack B but B cannot attack A
	if(attackable == false && (objB->IsPlayer() || objB->IsPet() ) )
	{
		if(objA->m_factionDBC->RepListId == -1 && objA->m_faction->HostileMask == 0 && objA->m_faction->FriendlyMask == 0)
		{
			attackable = true;
		}
	}

	return attackable;
}

bool isCombatSupport(Object* objA, Object* objB)// B combat supports A?
{
	if(!objA || !objB)
		return false;

	if(objA->GetTypeId() == TYPEID_CORPSE )
		return false;

	if(objB->GetTypeId() == TYPEID_CORPSE)
		return false;

//	if(objB->m_faction == 0 || objA->m_faction == 0)
//		return false;

	if( objA->IsPet() || objB->IsPet() ) // fixes an issue where horde pets would chain aggro horde guards and vice versa for alliance.
		return false;

	if( !(objA->m_phase & objB->m_phase) ) //What you can't see, you can't support either...
		return false;

	bool combatSupport = false;

	uint32 fSupport = objB->m_faction->FriendlyMask;
	uint32 myFaction = objA->m_faction->Mask;

	if(myFaction & fSupport)
	{
		combatSupport = true;
	}
	// check friend/enemy list
	for(uint32 i = 0; i < 4; i++)
	{
		if(objB->m_faction->EnemyFactions[i] == objA->m_faction->Faction)
		{
			combatSupport = false;
			break;
		}
		if(objB->m_faction->FriendlyFactions[i] == objA->m_faction->Faction)
		{
			combatSupport = true;
			break;
		}
	}
	return combatSupport;
}


bool isAlliance(Object* objA)// A is alliance?
{
	FactionTemplateDBC * m_sw_faction = dbcFactionTemplate.LookupEntry(11);
	FactionDBC * m_sw_factionDBC = dbcFaction.LookupEntry(72);
	if(!objA 
//		|| objA->m_factionDBC == NULL || objA->m_faction == NULL
		)
		return true;

	if(m_sw_faction == objA->m_faction || m_sw_factionDBC == objA->m_factionDBC)
		return true;

	//bool hostile = false;
	uint32 faction = m_sw_faction->Faction;
	uint32 host = objA->m_faction->HostileMask;

	if(faction & host)
		return false;

	// check friend/enemy list
	for(uint32 i = 0; i < 4; i++)
	{
		if(objA->m_faction->EnemyFactions[i] == faction)
			return false;
	}

	faction = objA->m_faction->Faction;
	host = m_sw_faction->HostileMask;

	if(faction & host)
		return false;

	// check friend/enemy list
	for(uint32 i = 0; i < 4; i++)
	{
		if(objA->m_faction->EnemyFactions[i] == faction)
			return false;
	}

	return true;
}







