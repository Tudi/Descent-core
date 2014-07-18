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

void WorldSession::HandleUseItemOpcode(WorldPacket& recvPacket)
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	typedef std::list<Aura*> AuraList;
	
	Player* p_User = GetPlayer();
	sLog.outDetail("WORLD: got use Item packet, data length = %i",recvPacket.size());
	int8 bagIndex,slot;
	uint64 item_guid;
	uint8 cn;
	uint32 spellId = 0,glyphSlot;
	uint8 unk;

	recvPacket >> bagIndex >> slot >> cn >> spellId >> item_guid >> glyphSlot;
	recvPacket >> unk;

#ifdef _DEBUG
	uint32 NeedRecastJump=0;
RecastJumpHere:
#endif

	Item* tmpItem = NULL;
	tmpItem = p_User->GetItemInterface()->GetInventoryItem(bagIndex,slot);
	if (!tmpItem)
		tmpItem = p_User->GetItemInterface()->GetInventoryItem(slot);
	if (!tmpItem)
	{ 
		sLog.outDetail("WORLD: use Item packet : Cannot find source item");
		return;
	}
	ItemPrototype *itemProto = tmpItem->GetProto();
	if(!itemProto)
	{ 
		sLog.outDetail("WORLD: use Item packet : Item has no proto");
		return;
	}

	if(_player->getDeathState()==CORPSE)
	{ 
		sLog.outDetail("WORLD: use Item packet : we are dead");
		return;
	}

	if(itemProto->Bonding == ITEM_BIND_ON_USE)
		tmpItem->SoulBind();

	if(sScriptMgr.CallScriptedItem(tmpItem,_player))
	{ 
		sLog.outDetail("WORLD: use Item packet : Scripted");
		return;
	}

	if( itemProto->InventoryType != 0 && !_player->GetItemInterface()->IsEquipped(itemProto->ItemId) )//Equipable items cannot be used before they're equipped. Prevents exploits
	{
		sLog.outDetail("WORLD: use Item packet : Cannot use unequipped item");
		return;//Prevents exploits such as keeping an on-use trinket in your bag and using WPE to use it from your bag in mid-combat.
	}

	if(itemProto->QuestId)
	{
		// Item Starter
		Quest *qst = QuestStorage.LookupEntry(itemProto->QuestId);
		if(!qst) 
		{ 
			sLog.outDetail("WORLD: use Item packet : missing quest");
			return;
		}

		WorldPacket data;
		sQuestMgr.BuildQuestDetails(&data, qst, tmpItem, 0, language, _player);
		SendPacket(&data);
		if( qst->quest_flags & QUEST_FLAGS_AUTO_ACCEPT )
		{
			uint64 guid = tmpItem->GetGUID();
			sQuestMgr.TryAutoAcceptQuest( this, guid, qst->id );
		}
	}

	if(itemProto->RequiredLevel)
	{
		if(_player->getLevel() < itemProto->RequiredLevel)
		{
			_player->GetItemInterface()->BuildInventoryChangeError(tmpItem,NULL,INV_ERR_ITEM_RANK_NOT_ENOUGH);
			sLog.outDetail("WORLD: use Item packet : Need more levels");
			return;
		}
	}

	if(itemProto->RequiredSkill)
	{
		if(!_player->_HasSkillLine(itemProto->RequiredSkill))
		{
			_player->GetItemInterface()->BuildInventoryChangeError(tmpItem,NULL,INV_ERR_ITEM_RANK_NOT_ENOUGH);
			sLog.outDetail("WORLD: use Item packet : Need skills");
			return;
		}

		if(itemProto->RequiredSkillRank)
		{
			if(_player->_GetSkillLineCurrent(itemProto->RequiredSkill, false) < itemProto->RequiredSkillRank)
			{
				_player->GetItemInterface()->BuildInventoryChangeError(tmpItem,NULL,INV_ERR_ITEM_RANK_NOT_ENOUGH);
				sLog.outDetail("WORLD: use Item packet : need skill rank");
				return;
			}
		}
	}
	
//	if( itemProto->AllowableClass && !(_player->getClassMask() & itemProto->AllowableClass) || itemProto->AllowableRace && !(_player->getRaceMask() & itemProto->AllowableRace) )
	if( ( itemProto->AllowableClass && !(_player->getClassMask() & itemProto->AllowableClass) ) || ( itemProto->AllowableRace && !(_player->getRaceMask() & itemProto->AllowableRace) ) )
	{
		_player->GetItemInterface()->BuildInventoryChangeError(tmpItem,NULL,INV_ERR_YOU_CAN_NEVER_USE_THAT_ITEM);
		sLog.outDetail("WORLD: use Item packet : wrong class");
		return;
	}		

	if(_player->m_currentSpell)
	{
		_player->SendCastResult(_player->m_currentSpell->GetProto()->Id, SPELL_FAILED_SPELL_IN_PROGRESS, cn, 0);
		sLog.outDetail("WORLD: use Item packet : we are casting");
		return;
	}
	
//	uint32 force_instant_cast = 0;
//	for(uint32 x = 0; x < 5 ; x++)
//		if( itemProto->Spells.Id[x] )
//			force_instant_cast++;
	uint32 CastedSpells = 0;
	SpellCastTargets targets(recvPacket, _player->GetGUID());
	//now these are enchantment tinker spells
	list<uint32>::iterator itr = tmpItem->enchant_spell_book.begin();
	for(uint32 x = 0; x < 5 + tmpItem->enchant_spell_book.size(); x++)
	{
		if( x < 5 )
			spellId = itemProto->Spells.Id[x];
		//nasty hack to integrate tinker spells :(
		else
		{
			spellId = *itr;
			itr++;
		}
		if( spellId == 0 || ( x<5 && itemProto->Spells.Trigger[x] != USE  ) )
			continue;

		// check for spell id
		SpellEntry *spellInfo = dbcSpell.LookupEntryForced( spellId );

		if ( !spellInfo )
		{
			sLog.outError("WORLD: unknown spell id %i\n", spellId);
			return;
		}

		if( !sHookInterface.OnCastSpell( _player, spellInfo ) )
		{
			sLog.outError("WORLD: use Item packet : Scripted spell");
			continue; // script interface used up this spell cast, jump to the next one ?
		}

		if (spellInfo->AuraInterruptFlags & AURA_INTERRUPT_ON_STAND_UP)
		{
			if (p_User->CombatStatus.IsInCombat() || p_User->IsMounted())
			{
				_player->GetItemInterface()->BuildInventoryChangeError(tmpItem,NULL,INV_ERR_CANT_DO_IN_COMBAT);
				sLog.outDetail("WORLD: use Item packet : Not while in combat");
				return;
			}
			if(p_User->GetStandState()!=1)
				p_User->SetStandState(STANDSTATE_SIT);
			// loop through the auras and removing existing eating spells
		}
		else
		{ // cebernic: why not stand up
			if (!p_User->CombatStatus.IsInCombat() && !p_User->IsMounted())
			{
				if( p_User->GetStandState() )//not standing-> standup
				{
					p_User->SetStandState( STANDSTATE_STAND );//probably mobs also must standup
				}
			}
		}

		// cebernic: remove stealth on using item
		if (!(spellInfo->AttributesEx & ATTRIBUTESEX_NOT_BREAK_STEALTH))
		{
			if( p_User->IsStealth() )
				p_User->RemoveAllAuraType( SPELL_AURA_MOD_STEALTH );
		}

		int32 RemainingCooldown = _player->Cooldown_Getremaining( spellInfo );
		if( RemainingCooldown > 0 || RemainingCooldown == PLAYER_SPELL_COOLDOWN_NOT_STARTED )
		{
			_player->SendCastResult(spellInfo->Id, SPELL_FAILED_NOT_READY, cn, 0);
			if( RemainingCooldown != PLAYER_SPELL_COOLDOWN_NOT_STARTED )
				_player->UpdateClientCooldown( spellInfo->Id, RemainingCooldown );
			sLog.outDetail("WORLD: use Item packet : in cooldown");
//			return;
			continue; // maybe some other spell can still be casterd
		}
#ifdef CLEAR_DUEL_COOLDOWNS
//		_player->m_DuelSpells.insert( spellInfo->Id );
#endif 
		Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		//if item has more then 1 spell then we make cast instant to not interrupt each other
		spell->Init(_player, spellInfo, ( CastedSpells > 1 ), NULL);
		uint8 result;
		spell->SpellCastQueueIndex=cn;
		spell->i_caster = tmpItem;
		spell->GlyphSlot = glyphSlot;
		result = spell->prepare(&targets);
		CastedSpells++;
	}
	_player->Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_USE_ITEM,itemProto->ItemId,ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
#ifdef _DEBUG
	int64 *ShouldRecast = (int64*)GetPlayer()->GetExtension( EXTENSION_ID_REPLACE_REPEATCAST );
	if( ShouldRecast != NULL && *ShouldRecast > NeedRecastJump )
	{
		NeedRecastJump++;
		goto RecastJumpHere;
	}
#endif
}

void WorldSession::HandleGlyphRemove(WorldPacket& recvPacket)
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}

	uint32 glyphslot;
	recvPacket >> glyphslot;

	if( glyphslot > GLYPHS_COUNT )
		return; //amg wtf ? Cheater !

	uint32 old_glyph_entry = _player->GetUInt32Value( PLAYER_FIELD_GLYPHS_1 + glyphslot );
	GlyphPropertiesEntry *glyph = dbcGlyphPropertiesStore.LookupEntry( old_glyph_entry );
	if( glyph == NULL )//strange
	{
		_player->SetUInt32Value( PLAYER_FIELD_GLYPHS_1 + glyphslot, 0 );
		return; 
	}

	_player->RemoveAura( glyph->SpellId );
	_player->SetUInt32Value( PLAYER_FIELD_GLYPHS_1 + glyphslot, 0 );
	_player->m_specs[ _player->m_talentActiveSpec ].glyphs[ glyphslot ] = 0;
}

void WorldSession::HandleCastSpellOpcode(WorldPacket& recvPacket)
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}

/*
01 cn
0D 08 00 00 spell
00 missle
00 00 00 00 ?
02 00 target mask -> unittarget
00 00 target mask more
8F 9D 4A 74 03 04 -> target getguid
*/
	uint32 spellId;
	uint8 cn;
	uint32 glyphSlot;
	uint8 missileflag;
	WoWGuid additional_target;

	recvPacket >> cn >> spellId;
	recvPacket >> glyphSlot;
	recvPacket >> missileflag;

#ifdef _DEBUG
	int64 *SrcID = _player->GetCreateIn64Extension( EXTENSION_ID_REPLACE_SPELL_1 );
	int64 *DstID = _player->GetCreateIn64Extension( EXTENSION_ID_REPLACE_SPELL_2 );
	if( *SrcID == spellId && *DstID != 0 )
		spellId = *DstID;
#endif
#ifdef _DEBUG
	uint32 NeedRecastJump=0;
RecastJumpHere:
#endif
	//used by ex : trap launcher
	uint32 swap_to_spell = _player->mSpellReplaces[ spellId ];
	if( swap_to_spell )
		spellId = swap_to_spell;
	
	//unstuck can be casted even when dead
	if( ( _player->getDeathState()==CORPSE || _player->isAlive() == false || _player->IsDead() == true ) 
		&& spellId != 7355 
		&& _player->HasAuraWithNameHash( SPELL_HASH_SPIRIT_OF_REDEMPTION, 0, AURA_SEARCH_POSITIVE ) == 0
		)
	{ 
		return;
	}

	// check for spell id
	SpellEntry *spellInfo = dbcSpell.LookupEntryForced(spellId );

	if(!spellInfo || !sHookInterface.OnCastSpell(_player, spellInfo))
	{
		sLog.outError("HandleCast: unknown spell id %i\n", spellId);
		return;
	}

/*  this is breaks capturing flags at arathi basin (marcelo)
	if (spellInfo->Attributes & ATTRIBUTES_NO_CAST)
	{
		sLog.outError("WORLD: attempt to cast spell %i, %s which has ATTRIBUTES_NO_CAST\n", spellId, spellInfo->Name);
		return;
	}*/

	sLog.outDetail("HandleCast: got cast spell packet, spellId - %i (%s), data length = %i",	spellId, spellInfo->Name, recvPacket.size());
	
	// Cheat Detection only if player and not from an item
	// this could fuck up things but meh it's needed ALOT of the newbs are using WPE now
	// WPE allows them to mod the outgoing packet and basicly choose what ever spell they want :(
	if( ( spellInfo->c_is_flags2 & SPELL_FLAG2_IS_NON_CLIENT_CASTABLE) 
		&& _player->mSpellReplaces[ spellInfo->ReplacingSpell ] == 0 ) 
	{
		sLog.outDetail("HandleCast: Spell should not be castable now");
		return;
	}

	if( ( swap_to_spell != spellId
		&& !GetPlayer()->HasSpell(spellId) )
			|| ( spellInfo->Attributes & ATTRIBUTES_PASSIVE )
		//need to properly implement this or spells thought by talents will not work !
//		|| ( spellInfo->c_is_flags2 & SPELL_FLAG2_IS_TALENT_SPEC ) //somehow they always manage to get some talent to exploit. This was generated manually in case passive flag is missing
		)
	{
		//double check to see if this is a vehicle spell
		if( GetPlayer()->CanCastVehicleSpell(spellId) == false 
			&& ( GetPlayer()->GetGroup() == NULL || GetPlayer()->GetGroup()->CanCast( GetPlayer(), spellId ) == false )
			&& ( spellInfo->c_is_flags2 & SPELL_FLAG2_IS_CASTABLE_BY_ANYONE ) == 0
			)
		{
			sLog.outDetail("HandleCast: Spell isn't casted because player \"%s\" is cheating", GetPlayer()->GetName());
			return;
		}
	}

	if (GetPlayer()->GetOnMeleeSpell() != spellId)
	{
		//autoshot 75
		if( (spellInfo->AttributesExB & FLAGS3_ACTIVATE_AUTO_SHOT)
			&& ( spellInfo->Attributes & ATTRIBUTES_STOP_ATTACK ) == 0
			)	// auto shot..
		{
			//sLog.outString( "HandleSpellCast: Auto Shot-type spell cast (id %u, name %s)" , spellInfo->Id , spellInfo->Name );
			Item *weapon = GetPlayer()->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_RANGED);
			if(!weapon) 
			{ 
				sLog.outDebug("HandleCast: Requires ranged weapon");
				return;
			}
			uint32 spellid;
			switch(weapon->GetProto()->SubClass)
			{
			case 2:			 // bows
			case 3:			 // guns
            case 18:		 // crossbow
				spellid = SPELL_RANGED_GENERAL;
				break;
			case 16:			// thrown
				spellid = SPELL_RANGED_THROW;
				break;
			case 19:			// wands
				spellid = SPELL_RANGED_WAND;
				break;
			default:
				spellid = 0;
				break;
			}
		   
			if(!spellid) 
				spellid = spellInfo->Id;
			
			if(!_player->m_onAutoShot)
			{
//				_player->m_AutoShotTarget = _player->GetSelection();
				uint32 duration = _player->GetUInt32Value(UNIT_FIELD_RANGEDATTACKTIME);
				SpellCastTargets targets(recvPacket,GetPlayer()->GetGUID());
				if(!targets.m_unitTarget)
				{
					sLog.outString( "Cancelling auto-shot cast because targets.m_unitTarget is null!" );
					return;
				}
				SpellEntry *sp = dbcSpell.LookupEntry(spellid);
			
				_player->m_AutoShotSpell = sp;
				_player->m_AutoShotDuration = duration;
				//This will fix fast clicks
				if(_player->m_AutoShotAttackTimer < 500)
					_player->m_AutoShotAttackTimer = 500;
				_player->m_onAutoShot = true;
			}

			sLog.outDebug("HandleCast: Nothing to cast. This was autocast spell called by client");
			return;
		}
		/*const char * name = sSpellStore.LookupString(spellInfo->Name);
		if(name)
			sChatHandler.SystemMessageToPlr(_player, "%sSpell Cast:%s %s %s[Group %u, family %u]", MSG_COLOR_LIGHTBLUE,
			MSG_COLOR_SUBWHITE, name, MSG_COLOR_YELLOW, spellInfo->GetSpellGroupType(), spellInfo->SpellFamilyName);*/

        if(_player->m_currentSpell)
        {
			if( _player->m_currentSpell->getState() == SPELL_STATE_CASTING 
				|| ( _player->m_currentSpell->GetProto() == spellInfo && spellInfo->ChannelInterruptFlags != 0 ) //channeled spell should be simply recasted without the interrupt message
				)	
			{
				// cancel the existing channel spell, cast this one
				_player->m_currentSpell->safe_cancel();
				_player->m_currentSpell->update( 0 );
			}
			else
			{
				// send the error message
				_player->SendCastResult(spellInfo->Id, SPELL_FAILED_SPELL_IN_PROGRESS, cn, 0);
				sLog.outDebug("HandleCast: Already casting");
				return;
			}
        }


		uint64 caster_GUID;
		Vehicle *in_vehicle = GetPlayer()->GetVehicle();
		if( in_vehicle != NULL )
			caster_GUID = in_vehicle->GetGUID();
		else
			caster_GUID = GetPlayer()->GetGUID();
		SpellCastTargets targets;
//		targets.m_missileFlags = missileflag;
		targets.read( recvPacket, caster_GUID );

		// some anticheat stuff
		if( spellInfo->self_cast_only )
		{
			if( targets.m_unitTarget && targets.m_unitTarget != _player->GetGUID() )
			{
				// send the error message
				_player->SendCastResult(spellInfo->Id, SPELL_FAILED_BAD_TARGETS, cn, 0);
				sLog.outDebug("HandleCast: Invalid target");
				return;
			}
		}

#ifdef CLEAR_DUEL_COOLDOWNS
//		_player->m_DuelSpells.insert( spellInfo->Id );
#endif 
		Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		spell->Init(GetPlayer(), spellInfo, false, NULL);
		spell->SpellCastQueueIndex = cn;
//		spell->m_missileFlags = missileflag;
		spell->GlyphSlot = glyphSlot;
		spell->prepare(&targets);

		if( targets.m_unitTarget && _player != NULL && targets.m_unitTarget != GetPlayer()->GetGUID() && GetPlayer()->GetMapMgr() )
		{
			Unit *target = GetPlayer()->GetMapMgr()->GetUnit( targets.m_unitTarget );
			if( target && isAttackable( GetPlayer(), target ) )
				GetPlayer()->last_casted_enemy_spell = spellInfo;
		}
	}
#ifdef _DEBUG
	int64 *ShouldRecast = (int64*)GetPlayer()->GetExtension( EXTENSION_ID_REPLACE_REPEATCAST );
	if( ShouldRecast != NULL && *ShouldRecast > NeedRecastJump )
	{
		NeedRecastJump++;
		goto RecastJumpHere;
	}
#endif
}

void WorldSession::HandleCancelCastOpcode(WorldPacket& recvPacket)
{
	uint32 spellId;
	uint8 cast_number;
	recvPacket >> cast_number;
	recvPacket >> spellId;

	if(GetPlayer()->m_currentSpell)
	{
//!!!! big hack, will need to find the opcode for this later when we catch up with client version
		if( ( GetPlayer()->m_currentSpell->GetProto()->c_is_flags3 && SPELL_FLAG3_IS_PROPERLY_IMPLEMENTED_CANCEL ) == 0 && GetPlayer()->IsSpellIgnoringMoveInterrupt( GetPlayer()->m_currentSpell->GetProto()->NameHash ) )
			return;
		if( GetPlayer() )
			GetPlayer()->ClearCooldownForSpell( GetPlayer()->m_currentSpell->GetProto()->Id );
		GetPlayer()->m_currentSpell->safe_cancel();
	}
}

void WorldSession::HandleCancelAuraOpcode( WorldPacket& recvPacket )
{
	uint32 spellId;
	recvPacket >> spellId;
	
	if( _player->m_currentSpell && _player->m_currentSpell->GetProto()->Id == spellId )
		_player->m_currentSpell->safe_cancel();
	else
	{
		SpellEntry *info = dbcSpell.LookupEntryForced( spellId );

		if( info != NULL && (info->Attributes & ATTRIBUTES_CANT_CANCEL) == 0 )
		{
			Aura *a = _player->HasAura( spellId );
			if( a && a->IsPassive() == false && a->IsPositive() )
			{
				a->m_flags |= WAS_REMOVED_ON_PLAYER_REQUEST;
				if( a->GetSpellProto()->maxstack <= 1 )
					a->Remove();
				else
					_player->RemoveAura( spellId, 0, AURA_SEARCH_ALL, MAX_PASSIVE_AURAS );
//				_player->RemoveAura( spellId );
				sLog.outDebug("Removing all auras with ID: %u",spellId);
			}
		}
	}

}

void WorldSession::HandleCancelChannellingOpcode( WorldPacket& recvPacket)
{
	uint32 spellId;
	recvPacket >> spellId;

	Player *plyr = GetPlayer();
	if(!plyr)
	{ 
		return;
	}
	if(plyr->m_currentSpell)
	{		
		plyr->m_currentSpell->safe_cancel();
	}
}

void WorldSession::HandleCancelAutoRepeatSpellOpcode(WorldPacket& recv_data)
{
	//sLog.outString("Received CMSG_CANCEL_AUTO_REPEAT_SPELL message.");
	//on original we automatically enter combat when creature got close to us
//	GetPlayer()->GetSession()->OutPacket(SMSG_CANCEL_COMBAT);
	GetPlayer()->m_onAutoShot = false;
}

void WorldSession::HandleAddDynamicTargetOpcode(WorldPacket & recvPacket)
{
/*
13329
{CLIENT} Packet: (0xC674) CMSG_PET_CAST_SPELL PacketSize = 31 TimeStamp = 1132099
AE 6F 01 99 3B 8F 40 F1 guid
02 CN
73 82 00 00 spell
00 some flags
40 00 00 00 target mask
00 guid
4B D2 C7 44 x
42 11 8A C5 y
43 2F 62 41 z

{CLIENT} Packet: (0x01F0) CMSG_PET_CAST_SPELL PacketSize = 99 TimeStamp = 4126312
72 ED 04 E9 6C 00 50 F1 
03 cast number
06 E1 00 00 spell
02 
60 00 m_targetMask
00 00 m_targetMaskExtended
00 guid
D8 9D A6 45 m_srcX
A3 0E 27 45 m_srcY
C9 CA CF 43 m_srcZ
00 guid
0F 00 A8 45 m_destX
4E C9 25 45 m_destY
9E 8C CC 43 m_destZ
6E BB 7B 3E missilepitch 0.2458321750164 
B4 04 34 42 missilespeed 45.004592895508 
01 missileunkcheck
B7 00 00 00 
DF 72 ED 04 E9 6C 50 F1 - compressed guid
00 00 00 00 
3B 00 57 F6 3E 00 F2 9D A6 45 50 30 27 45 27 C0 CC 43 1B DD BD 40 C2 A5 BB 3E 1E 00 00 00 

{SERVER} Packet: (0x0131) SMSG_SPELL_START PacketSize = 59 TimeStamp = 4126515
DF 72 ED 04 E9 6C 50 F1 
DF 72 ED 04 E9 6C 50 F1 
03 cast number
06 E1 00 00 spell
0E 00 06 10 cast flags
DC 05 00 00 cast time
60 00 00 00
00 D8 9D A6 45 A3 0E 27 45 C9 CA CF 43 
00 0F 00 A8 45 4E C9 25 45 9E 8C CC 43 

{SERVER} Packet: (0x0132) SMSG_SPELL_GO PacketSize = 70 TimeStamp = 4128562
DF 72 ED 04 E9 6C 50 F1 
DF 72 ED 04 E9 6C 50 F1
03 cast number
06 E1 00 00 spell
0C 03 17 10 flags
38 9E 65 FF time
00 target count
00 moderated target size
60 00 00 00 m_targetMask + m_targetMaskExtended
00 D8 9D A6 45 A3 0E 27 45 C9 CA CF 43 
00 47 05 A7 45 11 76 2B 45 79 98 CC 43 
A9 B5 20 3F AF 07 00 00 01 

{CLIENT} Packet: (0x01F0) CMSG_PET_CAST_SPELL PacketSize = 99 TimeStamp = 4141390
72 ED 04 E9 6C 00 50 F1 
05 
06 E1 00 00 
02 
60 00 00 00 
00 9D 8C A3 45 46 B6 25 45 E8 C4 CF 43 
00 6B 57 A3 45 17 7F 28 45 18 71 CC 43 
C2 FF 49 3E missilepitch 0.19726470112801
39 FD 33 42 missilespeed 44.997287750244 
01 missileunkcheck
B7 00 00 00 ?
DF 72 ED 04 E9 6C 50 F1 00 00 00 00 3B 00 2D 31 3F 00 09 8C A3 45 4D B5 25 45 9E 8C CC 43 D4 05 DC 3F 77 2D 43 3E 83 01 00 00 

{SERVER} Packet: (0x0131) SMSG_SPELL_START PacketSize = 59 TimeStamp = 4141468
DF 72 ED 04 E9 6C 50 F1 
DF 72 ED 04 E9 6C 50 F1 
05 
06 E1 00 00 
0E 00 06 10
DC 05 00 00
60 00 00 00 00 9D 8C A3 45 46 B6 25 45 E8 C4 CF 43 00 6B 57 A3 45 17 7F 28 45 18 71 CC 43 

*/
	uint64 guid;
	uint32 spellid;
	uint8 SpellCastQueueIndex;
	uint8 missileflag;
	recvPacket >> guid >> SpellCastQueueIndex >> spellid >> missileflag;
	
	SpellEntry * sp = dbcSpell.LookupEntryForced(spellid);
	if ( !sp ) 
	{ 
		return;
	}
	// Summoned Elemental's Freeze
	Unit *nc = NULL;
	bool check = false;

    if (spellid == 33395)
    {
        if (!_player->m_Summon)
        { 
            return;
        }
    }
	else if ( _player->GetSummon() && _player->GetSummon()->GetGUID() == guid )
	{
		AI_Spell *sp = _player->GetSummon()->GetAISpellForSpellId( spellid );
		if( sp == NULL || (sp->cooldowntime && getMSTime() < sp->cooldowntime ) )
			return;
		check =  true;
		nc = _player->GetSummon();
	}
    else if ( guid == _player->m_CurrentCharm )
    {
		nc = _player->GetMapMgr()->GetUnit( _player->m_CurrentCharm );
    }

	if( spellid == 33395 && _player->GetSummon() )	// Summoned Water Elemental's freeze
	{
		SpellCastTargets targets(recvPacket,_player->GetSummon()->GetGUID());
		Spell * pSpell = SpellPool.PooledNew( __FILE__, __LINE__ );
		pSpell->Init(_player->GetSummon(), sp, false, 0);
		pSpell->prepare(&targets);
	}
	else if( nc && nc->GetAIInterface() )		// trinket?
	{
		if( check == false )
		{
			SimplePointerListNode<AI_Spell> *itr2;
			for(itr2 = nc->GetAIInterface()->m_spells.begin(); itr2 != nc->GetAIInterface()->m_spells.end();itr2 = itr2->next)
				if(itr2->data->spell->Id == spellid)
				{
					check = true;
					break;
				}
		}
		if( check == false )
		{ 
			check = nc->CanCastVehicleSpell( spellid );
			if( check == false )
			{
				return;
			}
		}
		SpellCastTargets targets;
		targets.m_missileFlags = missileflag;
		targets.read(recvPacket,nc->GetGUID());
		Spell * pSpell = SpellPool.PooledNew( __FILE__, __LINE__ );
		pSpell->Init(nc, sp, false, 0);
		pSpell->SpellCastQueueIndex = SpellCastQueueIndex;
		pSpell->m_missileFlags = missileflag;
		pSpell->m_missilePitch = targets.missilepitch;
		pSpell->m_missileTravelTime = targets.traveltime;
		pSpell->m_missilespeed = targets.missilespeed;
		pSpell->m_missileunkcheck = targets.missileunkcheck;
		pSpell->m_missle_unkdoodah = targets.unkdoodah;
		pSpell->m_missle_unkdoodah2 = targets.unkdoodah2;
		pSpell->prepare(&targets);
	}
}

