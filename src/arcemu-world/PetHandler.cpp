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

void WorldSession::HandlePetAction(WorldPacket & recv_data)
{
	if(!_player->IsInWorld()) 
	{ 
		sLog.outDebug("HandlePetAction:Owner is not in world.Exiting");
		return;
	}

	//WorldPacket data;
	uint64 petGuid = 0;
	uint32 composite_val;
	uint32 misc;
	uint8 action;

	uint64 targetguid = 0;
	recv_data >> petGuid >> composite_val;

	//in 403 spells are on 24 bits for sure
	action = composite_val >> 24;
	misc = composite_val & 0x00FFFFFF;
	//recv_data.hexlike();

	//printf("Pet_Action: 0x%.4X 0x%.4X\n", misc, action);

	if(action == PET_ACTION_ACTION && misc == PET_ACTION_DISMISS )
	{
		Vehicle *veh = _player->GetVehicle( );
		if( veh && veh->GetGUID() == petGuid )
			veh->RemovePassenger( _player );
	}
	if(GET_TYPE_FROM_GUID(petGuid) == HIGHGUID_TYPE_UNIT)
	{
		Creature *pCharm = GetPlayer()->GetMapMgr()->GetCreature( petGuid );
		if(!pCharm) 
		{ 
			sLog.outDebug("HandlePetAction:Could not find creature on this map. Exiting");
			return;
		}

		// must be a mind controled creature..
		if(action == PET_ACTION_ACTION)
		{
			recv_data >> targetguid;
			switch(misc)
			{
			case PET_ACTION_ATTACK:
				{
					if(!sEventMgr.HasEvent(_player, EVENT_PLAYER_CHARM_ATTACK))
					{
						uint32 timer = pCharm->GetUInt32Value(UNIT_FIELD_BASEATTACKTIME);
						if(!timer) 
							timer = 2000;

						sEventMgr.AddEvent(_player, &Player::_EventCharmAttack, EVENT_PLAYER_CHARM_ATTACK, timer, 0,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
						_player->_EventCharmAttack();
					}
				}break;
			}
		}
		sLog.outDebug("HandlePetAction:Finished handling simple summon case. Exiting");
		return;
	}

	Pet *pPet = _player->GetMapMgr()->GetPet( petGuid );
	
	if( !pPet )
	{ 
		sLog.outDebug("HandlePetAction:Could not find pet on this map. Exiting");
		return;
	}
	
	if( !pPet->isAlive() && misc != 55709 )
	{
		pPet->SendActionFeedback( PET_FEEDBACK_PET_DEAD );	
		if( !( action == PET_ACTION_ACTION && misc == PET_ACTION_DISMISS ) )
		{
			sLog.outDebug("HandlePetAction:Pet is dead. Exiting");
			return;
		}
	}

	Unit *pTarget = NULL;

	if(action == PET_ACTION_SPELL || action == PET_ACTION_SPELL_1 || action == PET_ACTION_SPELL_2 || (action == PET_ACTION_ACTION && misc == PET_ACTION_ATTACK ) || ( ( action == PET_ACTION_ACTION || action == PET_ACTION_NOTDEF ) && misc == PET_ACTION_GO_TO_POSITION) ) // >> target
	{
		if( recv_data.rpos() < recv_data.wpos() )
		{
			recv_data >> targetguid;
			pTarget = _player->GetMapMgr()->GetUnit(targetguid);
		}
		if(!pTarget) 
			pTarget = pPet;	// target self
	}
	float GotoX,GotoY,GotoZ;
	if( ( action == PET_ACTION_ACTION || action == PET_ACTION_NOTDEF ) && misc == PET_ACTION_GO_TO_POSITION )
	{
		recv_data >> GotoX;
		recv_data >> GotoY;
		recv_data >> GotoZ;
	}

APPLY_ACTIONS_MORE_PETS:
	switch(action)
	{
	case PET_ACTION_ACTION:
	case PET_ACTION_NOTDEF:
		{
			pPet->SetPetAction(misc);	   // set current action
			//make pet stand up
			if( misc!=PET_ACTION_STAY )// stand up 
				pPet->SetStandState( STANDSTATE_STAND );
			switch(misc)
			{
			case PET_ACTION_ATTACK:
				{
					// make sure the target is attackable
					if( pTarget == pPet || !isAttackable( pPet, pTarget ) )
 					{
						pPet->SendActionFeedback( PET_FEEDBACK_CANT_ATTACK_TARGET );
						sLog.outDebug("HandlePetAction:Unable to perform on non attackable target. Exiting");
						return;
					}

					// Clear the threat
					pPet->GetAIInterface()->WipeTargetList();
					pPet->GetAIInterface()->WipeHateList();

					if( pPet->isCasting() )
						pPet->GetCurrentSpell()->safe_cancel();

					// Attack target with melee if the owner if we dont have spells - other wise cast. All done by AIInterface.
//					if( pPet->GetAIInterface()->getUnitToFollow() == NULL )
						pPet->GetAIInterface()->SetUnitToFollow(_player );

					// EVENT_PET_ATTACK
					//some say with macro you can abuse attack timer somehow
					pPet->setAttackTimer(2000, false);	
					pPet->setAttackTimer(2000, true);
					pPet->GetAIInterface()->AttackReaction(pTarget, 1, 0);	//try to add some agro
					pPet->GetAIInterface()->SetAIState(STATE_ATTACKING);	//order should be kept. First agro then force attacking state just in case 
					pPet->GetAIInterface()->m_forced_target = pTarget->GetGUID();	//should not let pet retarget other due to agro
/*
//how the hell do we stop the flashing attack button ?
		local name, subtext, texture, isToken, isActive, autoCastAllowed, autoCastEnabled = GetPetActionInfo(i);
		if ( not isToken ) then
			petActionIcon:SetTexture(texture);
			petActionButton.tooltipName = name;
		else
			petActionIcon:SetTexture(_G[texture]);
			petActionButton.tooltipName = _G[name];
		end
		petActionButton.isToken = isToken;
		petActionButton.tooltipSubtext = subtext;
		if ( isActive ) then
			if ( IsPetAttackAction(i) ) then
				PetActionButton_StartFlash(petActionButton);
				-- the checked texture looks a little confusing at full alpha (looks like you have an extra ability selected)
				petActionButton:GetCheckedTexture():SetAlpha(0.5);
			else
				PetActionButton_StopFlash(petActionButton);
				petActionButton:GetCheckedTexture():SetAlpha(1.0);
			end
			petActionButton:SetChecked(1);
		else
			PetActionButton_StopFlash(petActionButton);
			petActionButton:SetChecked(0);
		end
*/
				}break;
			case PET_ACTION_FOLLOW:
				{
					// Clear the threat
					pPet->GetAIInterface()->HandleEvent(EVENT_LEAVECOMBAT, pPet->GetAIInterface()->GetMostHated(), 0);
					pPet->GetAIInterface()->WipeTargetList();
					pPet->GetAIInterface()->WipeHateList();

					if( pPet->isCasting() )
						pPet->GetCurrentSpell()->safe_cancel();

					// Follow the owner... run to him...
					pPet->GetAIInterface()->SetUnitToFollow(_player);
					pPet->GetAIInterface()->HandleEvent(EVENT_FOLLOWOWNER, pPet, 0);
				}break;
			case PET_ACTION_STAY:
				{
					// Clear the threat
					pPet->GetAIInterface()->HandleEvent(EVENT_LEAVECOMBAT, pPet->GetAIInterface()->GetMostHated(), 0);
					pPet->GetAIInterface()->WipeTargetList();
					pPet->GetAIInterface()->WipeHateList();
					pPet->GetAIInterface()->m_forced_target = 0;

					if( pPet->isCasting() )
						pPet->GetCurrentSpell()->safe_cancel();

					// Stop following the owner, and sit.
					pPet->GetAIInterface()->SetUnitToFollow(NULL);
					pPet->SetStandState(STANDSTATE_SIT);
				}break;
			case PET_ACTION_GO_TO_POSITION:
				{
//{CLIENT} Packet: (0x7232) CMSG_PET_ACTION PacketSize = 32 stamp = 17176334
//02 00 00 01 00 00 40 F1 04 00 00 07 
//00 00 00 00 00 00 00 00 
//1F 7C 0B C6 1F B4 E8 C2 B8 EA A4 42 

					// Clear the threat
					pPet->GetAIInterface()->WipeTargetList();
					pPet->GetAIInterface()->WipeHateList();
					pPet->GetAIInterface()->m_forced_target = 0;
					pPet->GetAIInterface()->SetUnitToFollow(NULL);
					pPet->GetAIInterface()->HandleEvent(EVENT_LEAVECOMBAT, pPet, 0);

					if( pPet->isCasting() )
						pPet->GetCurrentSpell()->safe_cancel();

					float distance = pPet->CalcDistance( GotoX, GotoY, GotoZ );
					//this is needed as some mobs are spawned right near their next and onyl waypoint, so they keep spamming move packet at each update
					if( distance < DISTANCE_TO_SMALL_TO_WALK ) 
						GetPlayer()->BroadcastMessage( "Pet : Destination too close. Will implement one day.." );
					else
					{
						pPet->GetAIInterface()->m_moveRun = true;
						pPet->GetAIInterface()->MoveTo( GotoX, GotoY, GotoZ, 0.0f );
					}
				}break;
			case PET_ACTION_DISMISS:
				{
					// Bye byte...
					pPet->Dismiss();
				}break;
			}
		}break;

	case PET_ACTION_SPELL_2:
	case PET_ACTION_SPELL_1:
	case PET_ACTION_SPELL:
		{
			// misc == spellid
			SpellEntry * entry = dbcSpell.LookupEntry( misc );
			if( entry == NULL ) 
			{ 
				sLog.outDebug("HandlePetAction:Could not find spell with id %u. Exiting",misc);
				return;
			}

			if( pPet->CooldownCanCast( entry ) > 0 )
			{
				pPet->SendCastFailed( misc, SPELL_FAILED_NOT_READY );
				sLog.outDebug("HandlePetAction:Spell is cooling down. Exiting");
				return;
			}

			AI_Spell * sp = pPet->GetAISpellForSpellId( entry->Id );
			if( sp != NULL )
			{
				{
//					if( ( entry->c_is_flags & SPELL_FLAG_IS_BUFF ) == 0 )
					{
						// make sure the target is attackable if spell is a bad spell for us. Need to define what is a negative spell :(
//						if( pTarget != pPet && pTarget != _player && !isAttackable( pPet, pTarget ) )
						if( (entry->c_is_flags & SPELL_FLAG_IS_DAMAGING) && !isAttackable( pPet, pTarget ) )
						{
							pPet->SendActionFeedback( PET_FEEDBACK_CANT_ATTACK_TARGET );
							sLog.outDebug("HandlePetAction:Target is not attackable. Exiting");
							return;
						}
					}

					if(sp->autocast_type != AUTOCAST_EVENT_ATTACK)
					{
//						if(sp->autocast_type == AUTOCAST_EVENT_OWNER_ATTACKED)
//							pPet->CastSpell(_player, sp->spell, false);
//						else
//							pPet->CastSpell(pPet, sp->spell, false);
							pPet->CastSpell(pTarget, sp->spell, false);
					}
					else if( entry->rangeIndex == 1 ) //self reach only, we could add spells that are "beneficial" here
					{
						pPet->CastSpell(pPet, sp->spell, false);
					}
					//should we always agro our spell cast target ? Ex: sucubus invisibility
					else if( isAttackable( pPet, pTarget ) == true )
					{
						// Clear the threat
						pPet->GetAIInterface()->WipeTargetList();
						pPet->GetAIInterface()->WipeHateList();

						pPet->GetAIInterface()->AttackReaction(pTarget, 1, 0);
//						pPet->GetAIInterface()->SetNextSpell(sp); //this will not work if pet is in passive state
						pPet->CastSpell(pTarget, sp->spell, false);
						pPet->GetAIInterface()->m_forced_target = pTarget->GetGUID();	//should not let pet retarget other due to agro
					}
					else if( IsSpellDirectFirendlyTargetting( entry ) )
					{
						pPet->CastSpell( pTarget, sp->spell, false );
					}
					else
					{
						if( pPet->GetPetOwner() )
							pPet->GetPetOwner()->BroadcastMessage("This spell can only be casted on attackable targets");
					}
				}
			}
			else
				sLog.outDebug("HandlePetAction:Pet does not have spell %u(cheat ?). Exiting",misc);
		}break;
	case PET_ACTION_STATE:
		{
			pPet->SetPetState(misc);
		}break;
	default:
		{
			printf("WARNING: Unknown pet action received. Action = %.4X, Misc = %.4X\n", action, misc);
		}break;
	}
	
	//this is shaman feral spirit
	if( GetPlayer()->GetSummon( 1 ) != NULL && pPet != GetPlayer()->GetSummon( 1 ) )
	{
		pPet = GetPlayer()->GetSummon( 1 );
		goto APPLY_ACTIONS_MORE_PETS;
	}
	/* Send pet action sound - WHEE THEY TALK */
/*
{CLIENT} Packet: (0x7232) CMSG_PET_ACTION PacketSize = 32 TimeStamp = 61022645
0A 14 00 23 58 7F 40 F1 - guid
31 4C 00 - misc
01 - action
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 

{SERVER} Packet: (0x6A7B) SMSG_PET_ACTION_FEEDBACK PacketSize = 5 TimeStamp = 61022770
02 31 4C 00 00 
*/
/*
	//in 4.3.3 this is not used near pet action ? or the opcode is wrong ?
	WorldPacket actionp(SMSG_PET_ACTION_SOUND, 12);
	actionp << pPet->GetGUID() << uint32(1);
	SendPacket(&actionp);
	*/
}

void WorldSession::HandlePetInfo(WorldPacket & recv_data)
{
	//only for hunters and warlock ?
	sLog.outDebug("HandlePetInfo is called");

	GetPlayer()->UpdateClientCallPetSpellIcons();
}

void WorldSession::HandlePetNameQuery(WorldPacket & recv_data)
{
	sLog.outDebug("Received CMSG_PET_NAME_QUERY ");
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	uint32 petNumber = 0;
	uint64 petGuid = 0;

	recv_data >> petNumber >> petGuid;
	Pet *pPet = _player->GetMapMgr()->GetPet( petGuid ) ;
	if(!pPet)
	{ 
		return;
	}

	WorldPacket data(8 + pPet->GetName()->size());
	data.SetOpcode(SMSG_PET_NAME_QUERY_RESPONSE);
	data << pPet->GetUInt32Value( UNIT_FIELD_PETNUMBER );
	data << pPet->GetName()->c_str();
	data << pPet->GetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP);		// stops packet flood
	data << uint8(0);	//unk 4.0.3
	SendPacket(&data);
}

void WorldSession::HandleStablePet(WorldPacket & recv_data)
{
	if(!_player->IsInWorld()) 
	{ 
		sLog.outDebug("HandleStablePet: we are not ingame.Exiting");
		return;
	}

	uint8 guid_mask;
	uint32 pet_number;
	uint8 target_slot;

	recv_data >> guid_mask;
	recv_data >> pet_number;
	recv_data >> target_slot;
	//rest of the packet is the obfuscated guid

	// remove pet from world and association with player
	Pet *pPet = _player->GetSummon();
	if(pPet && pPet->GetUInt32Value(UNIT_CREATED_BY_SPELL) != 0) 
	{ 
		sLog.outDebug("HandleStablePet: Pet was created by spell.Exiting");
		return;
	}
	
	PlayerPet *DestPet = _player->GetPlayerPet( -1, -1, target_slot );
	PlayerPet *SrcPet = _player->GetPlayerPet( pet_number );

	if( SrcPet == NULL )
	{
		sLog.outDebug("HandleStablePet: Could not find pet to stable.Exiting");
		return;
	}

	if( DestPet )
		DestPet->StableSlot = SrcPet->StableSlot;

	SrcPet->StableSlot = target_slot;

/*
14333
{CLIENT} Packet: (0x28E1) UNKNOWN PacketSize = 12 TimeStamp = 27305915
3F AF EB 9A 00 06 95 F0 31 C0 BB 97 

{SERVER} Packet: (0x8363) UNKNOWN PacketSize = 16 TimeStamp = 27306383
AF EB 9A 00 06 00 00 00 00 00 00 00 04 00 00 00 

{SERVER} Packet: (0xEA36) SMSG_STABLE_RESULT PacketSize = 1 TimeStamp = 27306383
08 
*/
	WorldPacket data(16);
	data.SetOpcode(SMSG_STABLE_RESULT_DETAILED);
	data << pet_number;
	data << uint32( target_slot ); 
	data << uint32( 0 );
	data << uint32( pet_number );	//this is the index we used in pet_list packet from _player->m_Pets
	SendPacket(&data);

	data.Initialize(SMSG_STABLE_RESULT);
	data << uint8(0x08);  // success
	SendPacket(&data);
}

void WorldSession::HandleUnstablePet(WorldPacket & recv_data)
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	
	uint64 npcguid = 0;
	uint32 petnumber = 0;

	recv_data >> npcguid >> petnumber;
	PlayerPet *pet = _player->GetPlayerPet(petnumber);
	if(!pet)
	{
		sLog.outError("PET SYSTEM: Player "I64FMT" tried to unstable non-existant pet %d", _player->GetGUID(), petnumber);
		return;
	}
//	_player->SpawnPet( petnumber );
//	pet->stablestate = STABLE_STATE_ACTIVE;

	WorldPacket data(1);
	data.SetOpcode(SMSG_STABLE_RESULT);
	data << uint8(0x9); // success?
	SendPacket(&data);
}
void WorldSession::HandleStableSwapPet(WorldPacket & recv_data)
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}

	uint64 npcguid = 0;
	uint32 petnumber = 0;
	recv_data >> npcguid >> petnumber;

	PlayerPet *pet = _player->GetPlayerPet(petnumber);
	if(!pet)
	{
		sLog.outError("PET SYSTEM: Player "I64FMT" tried to unstable non-existant pet %d", _player->GetGUID(), petnumber);
		return;
	}
	Pet *pPet = _player->GetSummon();
	if(pPet && pPet->GetUInt32Value(UNIT_CREATED_BY_SPELL) != 0) 
	{ 
		return;
	}

	//stable current pet
	PlayerPet *pet2 = _player->GetPlayerPet(_player->GetUnstabledPetNumber());
	if(!pet2) 
	{ 
		return;
	}
	if(pPet)
		pPet->Dismiss(true, true);	// no safedelete needed
//	pet2->stablestate = STABLE_STATE_PASSIVE;

	//unstable selected pet
//	_player->SpawnPet(petnumber);
//	pet->stablestate = STABLE_STATE_ACTIVE;

	WorldPacket data;
	data.SetOpcode(SMSG_STABLE_RESULT);
	data << uint8(0x09);
	SendPacket(&data);
}

void WorldSession::HandleStabledPetList(WorldPacket & recv_data)
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	uint64 npcguid = 0;
	recv_data >> npcguid;
	SendStabledPetList( npcguid );
}

void WorldSession::SendStabledPetList(uint64 GUID)
{
	if( _player == NULL || !_player->IsInWorld() ) 
	{ 
		return;
	}
/*
13329
{SERVER} Packet: (0x5E5D) MSG_LIST_STABLED_PETS PacketSize = 84 TimeStamp = 146962375
00 00 00 00 00 00 00 00 guid
03 list size
14 stable slot size ? = 20
02 00 00 00 pet number
B7 0C 99 00 ?
46 88 00 00 entry
51 00 00 00 level ?
42 69 72 64 20 6F 66 20 50 72 65 79 00 Bird of Prey
01 stable active
05 00 00 00 
19 F6 47 00 
96 4B 00 00 
46 00 00 00 
4A 6F 69 6E 74 00 Joint
03 stable passive ? 1 + slot ? noway
00 00 00 00 
B5 40 5F 00 
DB 02 00 00 
54 00 00 00 
43 61 74 00 Cat 
01 
14333
{SERVER} Packet: (0xB272) MSG_LIST_STABLED_PETS PacketSize = 131 TimeStamp = 27304028
C1 96 00 00 94 BA 30 F1 
05 
14 
01 00 00 00 number
B6 14 9A 00 this is guid bytes 1,2,3,4
28 B0 00 00 entry
55 00 00 00 level
47 61 72 66 69 65 6C 64 00 name Garfield 
01 active

04 00 00 00 number
AF EB 9A 00 
8D B1 00 00 
55 00 00 00 
44 6F 67 00 
01 

02 00 00 00 
B7 0C 99 00 
46 88 00 00 
51 00 00 00 
42 69 72 64 20 6F 66 20 50 72 65 79 00 
01 

05 00 00 00 
19 F6 47 00 
96 4B 00 00 
46 00 00 00 
4A 6F 69 6E 74 00 
03 

00 00 00 00 
B5 40 5F 00 
DB 02 00 00 
55 00 00 00 
43 61 74 00 
01 
  <Packet OpCode="MSG_LIST_STABLED_PETS" Sender="Server">
    <Simple>
      <Name>VendorEntityId</Name>
      <Type>Guid</Type>
    </Simple>
    <Simple>
      <Name>NumPets</Name>
      <Type>Byte</Type>
    </Simple>
    <Simple>
      <Name>NumOwnedSlots</Name>
      <Type>Byte</Type>
    </Simple>
    <List LengthSegment="NumPets">
      <Name>Pets</Name>
      <Simple>
        <Name>Pet Index (?)</Name>
        <Type>UInt</Type>
      </Simple>
      <Simple>
        <Name>Pet Number</Name>
        <Type>UInt</Type>
      </Simple>
      <Simple>
        <Name>Pet Entry Id</Name>
        <Type>UInt</Type>
      </Simple>
      <Simple>
        <Name>Pet Level</Name>
        <Type>UInt</Type>
      </Simple>
      <Simple>
        <Name>Pet Name</Name>
        <Type>CString</Type>
      </Simple>
      <Simple>
        <Name>Slot Num</Name>
        <Type>Byte</Type>
      </Simple>
    </List>
*/
	sStackWorldPacket( data, MSG_LIST_STABLED_PETS, 10 + 15 * 35 + 30);

	data << GUID;

	data << uint8( _player->m_Pets.size() );
//	data << uint8(_player->m_StableSlotCount);
	data << uint8( 20 );
	std::list<PlayerPet*> list_copy( _player->m_Pets );
	for(std::list<PlayerPet*>::iterator itr = list_copy.begin(); itr != list_copy.end(); ++itr)
	{
		data << uint32( (*itr)->StableSlot ); // pet no. Not really. Just some index, probably something like stable slot
		data << uint32( (*itr)->number );	//this is guid bytes 1,2,3,4!
		data << uint32( (*itr)->entry ); // entryid
		data << uint32( (*itr)->level ); // level
		data.AppendSafeStr( (*itr)->name.c_str(), 10 );		  // name
		if( IsPetActive( (*itr) ) )
			data << uint8( 1 );
		else
			data << uint8( 3 );
	}

	SendPacket(&data);
}

void WorldSession::HandleBuyStableSlot(WorldPacket &recv_data)
{
	if( !_player->IsInWorld() || _player->GetStableSlotCount() >= 4 ) 
	{ 
		return;
	}

//	BankSlotPrice* bsp = dbcStableSlotPrices.LookupEntry( _player->GetStableSlotCount() + 1 );
//	int32 cost = ( bsp != NULL ) ? bsp->Price : 99999999;
	
	WorldPacket data( SMSG_STABLE_RESULT, 1 );
	
/*	if( cost > _player->GetGold( ) )
	{
		data << uint8(1); // not enough money
		SendPacket( &data );
 		return;
	}
 	_player->ModGold( -cost );*/
 	
	data << uint8( 0x0A );
	SendPacket( &data );
 	
	_player->m_StableSlotCount = MIN( _player->m_StableSlotCount, MAX_STABLE_SLOTS );
}


void WorldSession::HandlePetSetActionOpcode(WorldPacket& recv_data)
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	uint32 unk1;
	uint32 unk2;
	uint32 slot;
	uint32 composite_val;
	uint32 spell;
	uint8 state;
	recv_data >> unk1 >> unk2 >> slot >> composite_val;

	state = composite_val >> 24;
	spell = composite_val & 0x00FFFFFF;

	if( slot >= PET_ACTIONBAR_MAXSIZE )
	{ 
		return;
	}

	Pet * pet = _player->GetSummon();
	if( pet == NULL )
	{
		return;
	}

	if( composite_val == 0 || composite_val == PET_SPELL_PASSIVE || composite_val == PET_SPELL_DEFENSIVE 
		|| composite_val == PET_SPELL_AGRESSIVE || composite_val == PET_SPELL_STAY 
		|| composite_val == PET_SPELL_FOLLOW || composite_val == PET_SPELL_ATTACK 
		|| composite_val == PET_SPELL_ASSIST || composite_val == PET_SPELL_GOTO 
		)
	{
		pet->ActionBar[slot] = composite_val;
	}
	else
	{
		SpellEntry * spe = dbcSpell.LookupEntryForced( spell );
		if( spe == NULL )
		{ 
			return;
		}

		// do we have the spell? if not don't set it (exploit fix)
		PetSpellMap::iterator itr = pet->GetSpells()->find( spe );
		if( itr == pet->GetSpells()->end( ) )
		{ 
			return;
		}
		pet->ActionBar[slot] = spell;
		pet->SetSpellState(spell, state);
	}
}

void WorldSession::HandlePetRename(WorldPacket & recv_data)
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	uint64 guid;
	string name;
	recv_data >> guid >> name;

	if(!_player->GetSummon() || _player->GetSummon()->GetGUID() != guid)
	{
		sChatHandler.SystemMessage(this, "That pet is not your current pet, or you do not have a pet.");
		return;
	}

	Pet * pet = _player->GetSummon();
	pet->Rename(name);

	// Disable pet rename.
//	pet->SetUInt32Value(UNIT_FIELD_BYTES_2, 1 | (0x28 << 8) | (0x2 << 16));
	pet->SetUInt32Value( UNIT_FIELD_BYTES_2, U_FIELD_BYTES_ANIMATION_FROZEN | (UNIT_BYTE2_FLAG_PET_DETAILS << 16) );// 0x3 -> Enable pet rename.
}

void WorldSession::HandlePetAbandon(WorldPacket & recv_data)
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	Pet * pet = _player->GetSummon();
	if(!pet) 
	{ 
		return;
	}

	pet->Abandon();
}
/*
void WorldSession::HandlePetUnlearn(WorldPacket & recv_data)
{
	if( !_player->IsInWorld() )
	{ 
		return;
	}

	uint64 guid;
	recv_data >> guid;

	Pet* pPet = _player->GetSummon();
	if( pPet == NULL || pPet->GetGUID() != guid )
	{
		sChatHandler.SystemMessage(this, "That pet is not your current pet, or you do not have a pet.");
		return;
	}

	int32 cost = pPet->GetUntrainCost();
	if( cost > ( int64 )_player->GetGold( ) )
	{
		WorldPacket data(SMSG_BUY_FAILED, 12);
		data << uint64( _player->GetGUID() );
		data << uint32( 0 );
		data << uint8( 2 );		//not enough money
		return;	
	}
	_player->ModGold( -cost );
	pPet->WipeTalents();
	pPet->SetTalentPoints( pPet->GetTalentPointsForLevel( pPet->getLevel() ) );
} */

void WorldSession::HandlePetSpellAutocast( WorldPacket& recvPacket )
{
	// handles toggle autocast from spellbook
	if( !_player->IsInWorld() )
	{ 
		return;
	}

	uint64 guid;
    uint32 spellid;
    uint8  state;
    recvPacket >> guid >> spellid;

	Pet * pPet = _player->GetSummon();
	if( pPet == NULL )
	{ 
		return;
	}
	
	SpellEntry * spe = dbcSpell.LookupEntryForced( spellid );
	if( spe == NULL )
	{ 
		return;
	}
	
	// do we have the spell? if not don't set it (exploit fix)
	PetSpellMap::iterator itr = pPet->GetSpells()->find( spe );
	if( itr == pPet->GetSpells()->end( ) )
	{ 
		return;
	}

	uint8 OldState = itr->second;
	if( OldState == AUTOCAST_SPELL_STATE ) 
		state = DEFAULT_SPELL_STATE;
	else
		state = AUTOCAST_SPELL_STATE;
	pPet->SetSpellState( spellid, state );
}

void WorldSession::HandlePetCancelAura( WorldPacket& recvPacket )
{
	if( !_player->IsInWorld() )
	{ 
		return;
	}

	uint64 guid;
    uint16 spellid;

	recvPacket >> guid >> spellid;

	Pet * pPet = _player->GetSummon();
	if( pPet == NULL )
	{ 
		return;
	}

	if( !pPet->RemoveAura( spellid ) )
		sLog.outError("PET SYSTEM: Player "I64FMT" failed to cancel aura %u from pet", _player->GetGUID(), spellid );
}

void WorldSession::HandlePetLearnTalent( WorldPacket & recvPacket )
{
	if( !_player->IsInWorld() )
	{ 
		return;
	}

	uint64 guid;
    uint32 talentid;
	uint32 talentcol;

	recvPacket >> guid >> talentid >> talentcol;

	Pet * pPet = _player->GetSummon();
	if( pPet == NULL )
	{ 
		return;
	}
	pPet->LearnTalent( talentid, talentcol, false );
}

void WorldSession::HandlePetLearnPreviewTalent( WorldPacket & recvPacket )
{
    uint32 talentcount;
    uint32 talentid;
    uint32 rank;

    sLog.outDebug("Recieved packet CMSG_LEARN_PREVIEW_TALENTS_PET.");

    if( !_player->IsInWorld() )
        return;

	uint64 guid;
	recvPacket >> guid;

	Pet * pPet = _player->GetSummon();
	if( pPet == NULL )
	{ 
		return;
	}
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 0x04C1 CMSG_LEARN_TALENTS_MULTIPLE
    //  As of 3.2.2.10550 the client sends this packet when clicking "learn" on the talent interface (in preview talents mode)
    //  This packet tells the server which talents to learn
    //
    // Structure:
    //
    // struct talentdata{
    //  uint32 talentid;                - unique numeric identifier of the talent (index of talent.dbc)
    //  uint32 talentrank;              - rank of the talent
    //  };
    //
    // uint32 talentcount;              - number of talentid-rank pairs in the packet
    // talentdata[ talentcount ];
    //  
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    recvPacket >> talentcount;

    for( uint32 i = 0; i < talentcount; ++i )
    {
        recvPacket >> talentid;
        recvPacket >> rank;

        pPet->LearnTalent( talentid, rank, true );
    }
	pPet->smsg_TalentsInfo();
}