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

/////////////////////////////////////////////////
//  Debug Chat Commands
//

#include "StdAfx.h"


bool ChatHandler::HandlePlayerUnfear(const char* args, WorldSession *m_session)
{
	Player *_player = m_session->GetPlayer();
	if( (_player->m_special_state & ( UNIT_STATE_CONFUSE | UNIT_STATE_FEAR ) ) != 0 )
	{
		_player->BroadcastMessage("You are still feared. Wait for it to expire.");
		return false;
	}
	float *z = (float *)_player->GetCreateIn64Extension( EXTENSION_ID_FEAR_Z );
	if( *z == 0.0f )
	{
		_player->BroadcastMessage("No saved location found");
		return false;
	}
	float diff_z = *z - _player->GetPositionZ();
	if( abs( diff_z ) < 5 )
	{
		_player->BroadcastMessage("Your location is too close to previous location. Denied");
		return false;
	}
	float *x = (float *)_player->GetCreateIn64Extension( EXTENSION_ID_FEAR_X );
	float *y = (float *)_player->GetCreateIn64Extension( EXTENSION_ID_FEAR_Y );
	float dist = Distance2DSq( *x, *y, _player->GetPositionX(), _player->GetPositionY() );
	if( dist > 60*60 )
	{
		if( *x == 1.0f )
			_player->BroadcastMessage("You already got recalled to this location once. Denied");
		else
			_player->BroadcastMessage("Your location is too far from fear location (%f). Denied",dist);
		return false;
	}
	int64 *t = _player->GetCreateIn64Extension( EXTENSION_ID_FEAR_STAMP );
	if( *t + 2*60*1000 < getMSTime() )
	{
		_player->BroadcastMessage("Too much time passed since fear. Denied");
		return false;
	}
	_player->SpeedCheatDelay( 4000 );
	_player->SafeTeleport( _player->GetMapId(), _player->GetInstanceID(), *x, *y, *z + 2, _player->GetOrientation() );
	*x = 1.0f;
	*y = 1.0f;
	return true;
}

bool ChatHandler::HandlePlayerFixZ(const char* args, WorldSession *m_session)
{
	Player *_player = m_session->GetPlayer();
	if( _player->GetMapId() == 169 ) //emerald dream map for events and random spawns
	{
		_player->BroadcastMessage("This command is disabled on this map");
		return true;
	}
	float suggested_z = _player->GetMapMgr()->GetLandHeight( _player->GetPositionX(), _player->GetPositionY(), _player->GetPositionZ() );
	if( suggested_z != VMAP_VALUE_NOT_INITIALIZED )
	{
		float diff_z = suggested_z - _player->GetPositionZ();
		if( abs( diff_z ) < 5 )
		{
			_player->BroadcastMessage("Your location is close to collision location. Cannot improve it");
			return false;
		}
		else
		{
			_player->SpeedCheatDelay( 4000 );
			_player->SafeTeleport( _player->GetMapId(), _player->GetInstanceID(), _player->GetPositionX(), _player->GetPositionY(), suggested_z+5, _player->GetOrientation() );
		}
	}
	return true;
}

bool ChatHandler::HandlePlayerUnstuck(const char* args, WorldSession *m_session)
{
	Player *_player = m_session->GetPlayer();
	if( _player->getDeathState() != CORPSE )
	{ 
		_player->BroadcastMessage("Your are not considered dead. Relog if client is not updated");
		return true;
	}

	if( _player->m_bg != NULL )
	{ 
		_player->BroadcastMessage("This command cannot be executed here");
		return true;
	}

	int64 *ext = (int64 *)_player->GetCreateIn64Extension( EXTENSION_ID_UNSTUCK_STAMP );
	if( *ext > getMSTime() )
	{ 
		_player->BroadcastMessage("Anti spam protection. Remaining time %d seconds",uint32(*ext) );
		return true;
	}
	*ext = getMSTime() + 30000;

	_player->BroadcastMessage("Resurrecting in a few seconds at nearest graveyard");
	if( _player )
	  _player->RepopAtGraveyard( _player->GetPositionX(), _player->GetPositionY(), _player->GetPositionZ(), _player->GetMapId() );

	sEventMgr.AddEvent( _player, &Player::ResurrectPlayer, EVENT_PLAYER_FORECED_RESURECT, 3000, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
	return true;
}

bool ChatHandler::HandlePlayerRecall(const char* args, WorldSession *m_session)
{
	Player *_player = m_session->GetPlayer();
	if( _player->m_bg != NULL )
	{ 
		_player->BroadcastMessage("This command cannot be executed here");
		return true;
	}

	uint32 TimeSinceLogin = _player->GetPlayedTimeSinceLogin();
	if( TimeSinceLogin > 30 )
	{ 
		_player->BroadcastMessage("You can only use this command after login for 30 sec ");
		return true;
	}

	//revive player to avoid all those tickets about being stuck
	if( _player->isAlive() == false )
		_player->ResurrectPlayer();

	//some noobs miss bind location so we force them in main city
	if( _player->GetBindPositionX() == 0 ) 
	{
		if( _player->GetTeam() == 0 )
			_player->SetBindPoint( -9100,406,93,0,0);
		else
			_player->SetBindPoint( 1371,-4370,27,1,0);
	}

	sEventMgr.AddEvent(_player,&Player::EventTeleport,_player->GetBindMapId(),_player->GetBindPositionX(),_player->GetBindPositionY(),
		_player->GetBindPositionZ(),EVENT_PLAYER_TELEPORT,50,1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);

	return true;
}

bool ChatHandler::HandleClearLoot(const char* args, WorldSession *m_session)
{
	Player *_player = m_session->GetPlayer();

	if( _player->m_bg != NULL || IS_STATIC_MAP( _player->GetMapMgr()->GetMapInfo() ) == 0 )
	{ 
		_player->BroadcastMessage("This command cannot be executed here");
		return true;
	}

	if( _player->GetGroup() != NULL )
	{ 
		_player->BroadcastMessage("Not while in group");
		return true;
	}

	Creature *Selection = _player->GetMapMgr()->GetCreature( _player->GetSelection() );

	if( Selection == NULL || Selection->IsCreature() == false || Selection->IsDead() == false || Selection->TaggerGroupId != 0 || Selection->TaggerGuid != _player->GetGUID() )
	{
		_player->BroadcastMessage("Invalid selection");
		return true;
	}

	Selection->loot.items.clear();	
	Selection->loot.gold = 0;

	return true;
}


bool ChatHandler::HandleGuildKick(const char* args, WorldSession *m_session)
{
	Player *_player = m_session->GetPlayer();
	if(!_player->m_playerInfo->guild)
	{
		Guild::SendGuildCommandResult( m_session, GUILD_CREATE_S,"",GUILD_PLAYER_NOT_IN_GUILD );
		return true;
	}

	if( _player->m_playerInfo->guildMember->pRank->CanPerformCommand( GR_RIGHT_REMOVE ) == false )
	{
		sLog.outDebug("We do not have enough rights to remove players");
		Guild::SendGuildCommandResult( m_session, GUILD_CREATE_S,"",GUILD_PERMISSIONS );
		return true;
	}

	if( args == NULL )
	{
		sLog.outDebug("Missing parameter to [Playername] to be kicked");
		_player->BroadcastMessage( "Missing playername that should be kicked" );
		Guild::SendGuildCommandResult( m_session, GUILD_CREATE_S,"",GUILD_PLAYER_NOT_FOUND );
		return true;
	}
	PlayerInfo * dstplr = objmgr.GetPlayerInfoByName( args );
	if(dstplr==NULL)
	{ 
		_player->BroadcastMessage( "Could not find player to kick" );
		Guild::SendGuildCommandResult( m_session, GUILD_CREATE_S,"",GUILD_PLAYER_NOT_FOUND );
		return true;
	}
	_player->m_playerInfo->guild->RemoveGuildMember( dstplr, m_session );

	return true;
}

bool ChatHandler::HandleSpectate(const char* args, WorldSession *m_session)
{
	Player *_player = m_session->GetPlayer();
	
	if( _player->IsSpectator() )
	{
		_player->SpectatorRemove();
	}
	else
	{
		if( _player->CombatStatus.IsInCombat() )
		{
			_player->BroadcastMessage( "Can't specate while in combat" );
			return true;
		}
		if( _player->m_bg || IS_MAP_INSTANCE( _player->GetMapMgr()->GetMapInfo() ) )
		{
			_player->BroadcastMessage( "Can only start spectate from static maps" );
			return true;
		}
		_player->SpectatorApply();
	}

	return true;
}

bool ChatHandler::HandleSpectateAtPlayer(const char* args, WorldSession *m_session)
{
	Player *_player = m_session->GetPlayer();
	if( _player->IsSpectator() == false )
	{
		_player->BroadcastMessage( "You need to be in spectator mode to use this command" );
		return true;
	}
	if( args == NULL )
	{
		_player->BroadcastMessage( "Missing playername that should spectated" );
		return true;
	}
	PlayerInfo * dstplr = objmgr.GetPlayerInfoByName( args );
	if( dstplr==NULL || dstplr->m_loggedInPlayer == NULL )
	{ 
		_player->BroadcastMessage( "Could not find player to spectate" );
		return true;
	}
	_player->SafeTeleport( dstplr->m_loggedInPlayer->GetMapMgr(), dstplr->m_loggedInPlayer->GetPosition() );
	_player->SetUInt64Value( PLAYER_FARSIGHT, 0 );
	return true;
}

bool ChatHandler::HandleSpectatePlayer(const char* args, WorldSession *m_session)
{
	Player *_player = m_session->GetPlayer();
	if( _player->IsSpectator() == false )
	{
		_player->BroadcastMessage( "You need to be in spectator mode to use this command" );
		return true;
	}
	if( args == NULL )
	{
		_player->BroadcastMessage( "Missing playername that should spectated" );
		return true;
	}
	PlayerInfo * dstplr = objmgr.GetPlayerInfoByName( args );
	if( dstplr==NULL || dstplr->m_loggedInPlayer == NULL )
	{ 
		_player->BroadcastMessage( "Could not find player to spectate" );
		return true;
	}
	if( dstplr->m_loggedInPlayer == _player )
	{
		_player->BroadcastMessage( "No point to self spectate" );
		return true;
	}
	if( dstplr->m_loggedInPlayer->GetDistanceSq( _player ) > 30.0f * 30.0f )
	{
		_player->BroadcastMessage( "Target is too far to bind view directly. First teleport near him" );
		return true;
	}

	_player->SafeTeleport( dstplr->m_loggedInPlayer->GetMapMgr(), dstplr->m_loggedInPlayer->GetPosition() );

	_player->SetUInt64Value( PLAYER_FARSIGHT, 0 );
	_player->SetUInt64Value( PLAYER_FARSIGHT, dstplr->m_loggedInPlayer->GetGUID() );

	_player->BroadcastMessage( "Specatating player %s ", dstplr->m_loggedInPlayer->GetName() );
	return true;
}

static UINT64 LastGUID;

bool ChatHandler::HandleFocusArena(const char* args, WorldSession *m_session)
{
	Player *_player = m_session->GetPlayer();

#if 0
	// Sadly this is broken because in cataclysm you can't set the target on client side :S. You can clear it, but no idea how to set it
//	return false;
{
//	if( _player->GetUInt64Value( UNIT_FIELD_TARGET ) == 0 )
	{
		LastGUID = 115;
		Player *plr = objmgr.GetPlayer( LastGUID );
//		_player->SetSelection( LastGUID );
//		_player->SetUInt64Value( UNIT_FIELD_TARGET, LastGUID );
		sStackWorldPacket( data, SMSG_CLEAR_TARGET, 8 + 10 );
		data << uint64( 0 );
		m_session->SendPacket( &data );
//		dealdamage dmg;
//		memset( &dmg, 0, sizeof( dmg ) );
//		_player->SendAttackerStateUpdate( plr, _player, &dmg, 1, 1, 1, HITSTATUS_ABSORBED, 0 );
				{
					sStackWorldPacket( data ,SMSG_ATTACKERSTATEUPDATE, 250 );
					data << (uint32)HITSTATUS_NOACTION;   
					data << plr->GetNewGUID();
					data << _player->GetNewGUID();
					data << (uint32)0;				// Realdamage;
					data << uint32(0);
					data << (uint8)1;					// Damage type counter / swing type
					data << (uint32)0;				  // Damage school
					data << (float)0;	// Damage float
					data << (uint32)0;	// Damage amount
					data << uint8( 0 );
					data << uint32( 0 );				// can be 0,1000 or -1
					data << uint32( 0 );
					_player->GetSession()->SendPacket( &data );
				}
	}
/*	else
	{
		LastGUID = _player->GetUInt64Value( UNIT_FIELD_TARGET );
		_player->SetSelection( 0 );
		_player->SetUInt64Value( UNIT_FIELD_TARGET, 0 );
		sStackWorldPacket( data, SMSG_CLEAR_TARGET, 8 + 10 );
		data << uint64( 0 );
		m_session->SendPacket( &data );
	}*/
	return true;
}
#endif
	if( _player->m_bg == NULL || IS_ARENA( _player->m_bg->GetType() ) == false )
	{
		_player->BroadcastMessage( "Only inside arena" );
		return true;
	}
	Arena *a = (Arena*)_player->m_bg;
	if( a->HasStarted() == false )
	{
		_player->BroadcastMessage( "Match has not started yet" );
		return true;
	}
	if( args == NULL )
	{
		_player->BroadcastMessage( "Missing player index that you want to target" );
		return true;
	}

	int Index = atoi( args );
	if( Index < 1 || Index > 5 )
	{
		_player->BroadcastMessage( "Index out of bounds. Valid numbers are [1,5]" );
		return true;
	}

	int EnemyTeam = 1 - _player->GetTeam();
	int IndexCounter = 1;
	for(InRangePlayerSet::iterator itr = a->m_players[EnemyTeam].begin(); itr != a->m_players[EnemyTeam].end(); ++itr) 
	{
		Player *plr = *itr;
		if( Index == IndexCounter )
		{
			if( _player->CanSee( plr ) == false )
				_player->BroadcastMessage( "Player is not visible to us" );
			else
			{
//				_player->SetSelection( plr->GetGUID() );
//				_player->SetUInt64Value( UNIT_FIELD_TARGET, plr->GetGUID() );
				//big hack as of Cataclysm you can't force client to set selection
				sStackWorldPacket( data, SMSG_CLEAR_TARGET, 8 + 10 );
				data << uint64( 0 );
				m_session->SendPacket( &data );
				{
					sStackWorldPacket( data ,SMSG_ATTACKERSTATEUPDATE, 250 );
					data << (uint32)HITSTATUS_NOACTION;   
					data << plr->GetNewGUID();
					data << _player->GetNewGUID();
					data << (uint32)0;				// Realdamage;
					data << uint32(0);
					data << (uint8)1;					// Damage type counter / swing type
					data << (uint32)0;				  // Damage school
					data << (float)0;	// Damage float
					data << (uint32)0;	// Damage amount
					data << uint8( 0 );
					data << uint32( 0 );				// can be 0,1000 or -1
					data << uint32( 0 );
					_player->GetSession()->SendPacket( &data );
				}
			}
			break;
		}
		IndexCounter++;
	}

	return true;
}

bool ChatHandler::HandleQueueCast(const char* args, WorldSession *m_session)
{
	Player *_player = m_session->GetPlayer();
	if( args == NULL )
	{
		_player->BroadcastMessage( "Missing parameters [queue slot] [spellid] [MS delay]" );
		return true;
	}

	uint32 SlotId, SpellId, Delay;
	if( sscanf(args, "%u %u %u", &SlotId, &SpellId, &Delay) < 3 )
		return false;

	if( SlotId > 5 )
	{
		_player->BroadcastMessage( "Can't queue more then 5 spells" );
		return true;
	}

	SpellEntry *sp = dbcSpell.LookupEntryForced( SpellId );
	if( sp == NULL || ( sp->Attributes & ATTRIBUTES_PASSIVE ) || _player->HasSpell( SpellId ) == false )
	{
		_player->BroadcastMessage( "Can't cast spell %d", SpellId );
		return true;
	}

	if( Delay > 5 * 60 * 1000 )
	{
		_player->BroadcastMessage( "Delay can't be larger then 5 * 60 * 1000" );
		return true;
	}

	int64 *SlotReserved = _player->GetCreateIn64Extension( EXTENSION_ID_DELAYEDCAST_1 + SlotId );
//	if( sEventMgr.HasEvent( _player, EVENT_PLAYER_DELAYED_CAST0 + SlotId ) )
	if( *SlotReserved > getMSTime() )
	{
//		_player->BroadcastMessage( "Removed old queued cast %d", SlotId );
//		sEventMgr.RemoveEvents( _player, EVENT_PLAYER_DELAYED_CAST0 + SlotId );
		_player->BroadcastMessage( "QueueSlot is used for %d", *SlotReserved - getMSTime() );
		return true;
	}
	*SlotReserved = getMSTime() + Delay;

	_player->CastSpellDelayed( _player->GetSelection(), SpellId, Delay, false );
//	sEventMgr.AddEvent( _player, EVENT_PLAYER_DELAYED_CAST0 + SlotId )

	return true;
}

#define MAX_VOID_STORAGE_SLOTS 200

uint32 VoidStorageGetUpdateCache( Player *_player, uint32 Entry, uint32 Slot, bool ForcedUpdate, bool Persistent, bool JustCheck = false )
{
//	uint32 ExtensionId = crc32((const unsigned char*)"VoidStorage", (unsigned int)strlen("VoidStorage"));
	int64 *VoidCache = _player->GetCreateIn64Extension( EXTENSION_ID_VOID_STORAGE_SLOTS_START + Slot );

	if( JustCheck == true )
		return *VoidCache;

	if( Entry != 0 && ForcedUpdate == false && *VoidCache != 0 )
		return *VoidCache;

	*VoidCache = Entry;

	if( Persistent == true )
	{
		QueryResult * result;
		if( Entry == 0 )
			result = CharacterDatabase.Query( "delete from VoidStorage where OwnerGuid=%d and slot=%d", _player->GetLowGUID(), Slot );
		else
			result = CharacterDatabase.Query( "replace into VoidStorage (OwnerGuid,entry,slot) values (%d,%d,%d)", _player->GetLowGUID(), Entry, Slot );
		if( result )
		{
			delete result;
			result = NULL;
		}
	}

	return 0;
}

bool ChatHandler::HandleVoidStorageList(const char* args, WorldSession *m_session)
{
	Player *_player = m_session->GetPlayer();
//	uint32 ExtensionId = crc32((const unsigned char*)"VoidStorage", (unsigned int)strlen("VoidStorage"));
	int64 *IsCached = _player->GetCreateIn64Extension( EXTENSION_ID_VOID_STORAGE_ISCACHED );
	if( *IsCached == 0 )
	{
		*IsCached = 1;
		QueryResult * result;
		result = CharacterDatabase.Query( "Select Entry, Slot from VoidStorage where OwnerGuid=%d", _player->GetLowGUID() );
		if( result )
		{
			do
			{
				Field *fields = result->Fetch();
				uint32 Entry = fields[0].GetInt32();
				uint32 Slot = fields[1].GetInt32();
				VoidStorageGetUpdateCache( _player, Entry, Slot, true, false );
			}while( result->NextRow() );
			delete result;
			result = NULL;
		}
	}

	char ReturnBuffer[MAX_VOID_STORAGE_SLOTS * ( 6 + 1 + 2 )];
	sprintf_s( ReturnBuffer, MAX_VOID_STORAGE_SLOTS * ( 6 + 1 + 2 ), "#VoidStorageList " );

	for( uint32 i=0;i<MAX_VOID_STORAGE_SLOTS;i++)
		if( _player->GetExtension( EXTENSION_ID_VOID_STORAGE_SLOTS_START + i ) )
		{
			int64 *VoidCache = _player->GetCreateIn64Extension( EXTENSION_ID_VOID_STORAGE_SLOTS_START + i );
			sprintf_s( ReturnBuffer, MAX_VOID_STORAGE_SLOTS * ( 6 + 1 + 2 ), "%s %d %d", ReturnBuffer, (uint32)*VoidCache, i );
		}

	WorldPacket *data = sChatHandler.FillMessageData( CHAT_MSG_SYSTEM, LANG_UNIVERSAL, ReturnBuffer, _player->GetGUID(), 0, 0, _player );
	_player->GetSession()->SendPacket(data);
	delete data;

	return true;
}

bool ChatHandler::HandleVoidStoragePut(const char* args, WorldSession *m_session)
{
	Player *_player = m_session->GetPlayer();

	if (!*args)
	{
		_player->BroadcastMessage("Usage : voidput [ItemEntry] [DestSlot]");
		return false;
	}

	uint32 Entry, Slot, ParamCount;
	ParamCount = sscanf(args, "%u %u", &Entry, &Slot );

	if( ParamCount != 2 )
	{
		_player->BroadcastMessage("Usage : voidput [ItemEntry] [DestSlot]");
		return false;
	}

	if( Slot >= MAX_VOID_STORAGE_SLOTS )
	{
		_player->BroadcastMessage("Error: Void Storage slot is larger then max %d", MAX_VOID_STORAGE_SLOTS );
		_player->BroadcastMessage("Usage : voidput [ItemEntry] [DestSlot]");
		return false;
	}

	Item *it = _player->GetItemInterface()->FindItemLessMax( Entry, 0, false );

	if( it == NULL )
	{
		_player->BroadcastMessage("Error: Void Storage could not find item %d in your backpack", Entry );
		_player->BroadcastMessage("Usage : voidput [ItemEntry] [DestSlot]");
		return false;
	}

	if( it->GetProto()->MaxCount > 1 )
	{
		_player->BroadcastMessage("Error: Void Storage can't store stackable items ( %u )", Entry );
		_player->BroadcastMessage("Usage : voidput [ItemEntry] [DestSlot]");
		return false;
	}

	uint32 SlotTaken = VoidStorageGetUpdateCache( _player, Entry, Slot, false, true );
	if( SlotTaken > 0 )
	{
		_player->BroadcastMessage("Error: Void Storage slot %d is already used by %d", Slot, SlotTaken );
		_player->BroadcastMessage("Usage : voidput [ItemEntry] [DestSlot]");
		return false;
	}

	bool result = _player->GetItemInterface()->SafeFullRemoveItemByGuid( it->GetGUID() );

	char ReturnBuffer[MAX_VOID_STORAGE_SLOTS * ( 6 + 1 + 2 )];
	sprintf_s( ReturnBuffer, MAX_VOID_STORAGE_SLOTS * ( 6 + 1 + 2 ), "#VoidStorageput %d %d ", Entry, Slot );

	WorldPacket *data = sChatHandler.FillMessageData( CHAT_MSG_SYSTEM, LANG_UNIVERSAL, ReturnBuffer, _player->GetGUID(), 0, 0, _player );
	_player->GetSession()->SendPacket(data);
	delete data;

	return true;
}

bool ChatHandler::HandleVoidStorageGet(const char* args, WorldSession *m_session)
{
	Player *_player = m_session->GetPlayer();

	if (!*args)
	{
		_player->BroadcastMessage("Usage : voidget [SrcSlot]");
		return false;
	}

	uint32 Slot, ParamCount;
	ParamCount = sscanf(args, "%u", &Slot );

	if( ParamCount != 1 )
	{
		_player->BroadcastMessage("Usage : voidget [SrcSlot]");
		return false;
	}

	if( Slot >= MAX_VOID_STORAGE_SLOTS )
	{
		_player->BroadcastMessage("Error: Void Storage slot is larger then max %d", MAX_VOID_STORAGE_SLOTS );
		_player->BroadcastMessage("Usage : voidget [SrcSlot]");
		return false;
	}

	uint32 EntryInSlot = VoidStorageGetUpdateCache( _player, 0, Slot, false, true, true );
	if( EntryInSlot <= 0 )
	{
		_player->BroadcastMessage("Error: Void Storage slot %d is not used", Slot );
		_player->BroadcastMessage("Usage : voidget [SrcSlot]");
		return false;
	}

	VoidStorageGetUpdateCache( _player, 0, Slot, false, true );
	_player->GetItemInterface()->AddItemById( EntryInSlot, 1, 0 );

	char ReturnBuffer[MAX_VOID_STORAGE_SLOTS * ( 6 + 1 + 2 )];
	sprintf_s( ReturnBuffer, MAX_VOID_STORAGE_SLOTS * ( 6 + 1 + 2 ), "#VoidStorageget %d %d ", EntryInSlot, Slot );

	WorldPacket *data = sChatHandler.FillMessageData( CHAT_MSG_SYSTEM, LANG_UNIVERSAL, ReturnBuffer, _player->GetGUID(), 0, 0, _player );
	_player->GetSession()->SendPacket(data);
	delete data;

	return true;
}

void PrintWarGameStatus( Player *Inviter, Player *p1, Player *p2 );

bool ChatHandler::HandleWargames(const char* args, WorldSession *m_session)
{
	Player *_player = m_session->GetPlayer();

	if (!*args)
	{
		_player->BroadcastMessage("Subcommands : groupinfo, sudendeath, pdeathlimit, tdeathlimit, sizecheck");
		_player->BroadcastMessage("Usage : wargames <subcommand>");
		return false;
	}

	char SubCommand[500];
	uint32 Param, ParamCount;
	ParamCount = sscanf(args, "%s %u", SubCommand, &Param );

	_player->WargamesInitialize();
	if( strstr( SubCommand, "sudendeath" ) )
	{
		_player->m_Wargames->SuddenDeath = 1 - _player->m_Wargames->SuddenDeath;
		_player->BroadcastMessage("Sudden Death : %d",_player->m_Wargames->SuddenDeath );
	}
	else if( strstr( SubCommand, "pdeathlimit" ) )
	{
		_player->m_Wargames->PlayerLives = MAX( 1, ParamCount );
		_player->BroadcastMessage("Max player deaths per match: %d", _player->m_Wargames->PlayerLives );
	}
	else if( strstr( SubCommand, "tdeathlimit" ) )
	{
		_player->m_Wargames->TeamLives = MAX( 1, ParamCount );
		_player->BroadcastMessage("max team deaths per match : %d", _player->m_Wargames->TeamLives );
	}
	else if( strstr( SubCommand, "sizecheck" ) )
	{
		if( _player->m_bg != NULL )
		{
			_player->BroadcastMessage("Not while inside BG" );
			return true;
		}
		_player->m_Wargames->SkipPartySizeCheck = 1 - _player->m_Wargames->SkipPartySizeCheck;
		_player->BroadcastMessage("unbalanced teams : %d", _player->m_Wargames->SkipPartySizeCheck );
	}
	else if( strstr( SubCommand, "groupinfo" ) )
	{
		Player *selection = _player->GetMapMgr()->GetPlayer( _player->GetSelection() );
		if( selection == NULL || selection->GetGroup() == NULL )
			_player->BroadcastMessage("You need to select a char that has a group" );
		else
			PrintWarGameStatus( selection, _player, selection );
	}
	else if( strstr( SubCommand, "resend" ) )
	{
		if( _player->GetGroup() == NULL || _player->GetGroup()->GetLeader() != _player->getPlayerInfo() )
		{
			_player->BroadcastMessage("You need to be group leader" );
			return true;
		}
		if( _player->m_bg == NULL )
		{
			_player->BroadcastMessage("You need to be inside a BG. Maybe pass leadership ?" );
			return true;
		}
		uint32 maxPlayers = BattlegroundManager.GetMaximumPlayers( _player->m_Wargames->BGType );

		if( _player->m_Wargames->SkipPartySizeCheck )
		{
			for( uint32 i = 0; i< 2; i++ )
			{
				if( _player->m_bg->m_players[ i ].begin() != _player->m_bg->m_players[ i ].end() )
				{
					Player *p1 = *_player->m_bg->m_players[ i ].begin();
					if( p1->GetGroup() )
						_player->m_bg->AddGroupToTeam( p1->GetGroup(), i );
				}
			}
		}
		else
		{
			int32 ToAdd = abs( (int32)_player->m_bg->m_players[ 1 ].size() - (int32)_player->m_bg->m_players[ 0 ].size() );

			if( _player->m_bg->m_players[ 0 ].begin() != _player->m_bg->m_players[ 0 ].end() && _player->m_bg->m_players[ 0 ].size() < _player->m_bg->m_players[ 1 ].size() )
			{
				Player *p1 = *_player->m_bg->m_players[ 0 ].begin();
				if( p1->GetGroup() )
					_player->m_bg->AddGroupToTeam( p1->GetGroup(), 0, ToAdd );
			}
			if( _player->m_bg->m_players[ 1 ].begin() != _player->m_bg->m_players[ 1 ].end() && _player->m_bg->m_players[ 1 ].size() < _player->m_bg->m_players[ 0 ].size() )
			{
				Player *p1 = *_player->m_bg->m_players[ 1 ].begin();
				if( p1->GetGroup() )
					_player->m_bg->AddGroupToTeam( p1->GetGroup(), 1, ToAdd );
			}
		}
	}
	else
		_player->BroadcastMessage("Unknown subcommand" );

	return true;
}

bool ChatHandler::HandleRespawnWorld(const char* args, WorldSession *m_session)
{
	Player *_player = m_session->GetPlayer();

	_player->BroadcastMessage("Forcing visible object update" );
#ifdef _DEBUG
//	*_player->GetCreateIn64Extension( EXTENSION_ID_SIMULATE_FAULTY_SPAWNING ) = 1;
#endif
	ByteBuffer buf(2500);
	//despawn objects ?
	InRangeSet::iterator itr;
	for( itr = _player->GetVisibleSetBegin(); itr != _player->GetVisibleSetEnd(); itr++ )
	{
		Object *pObj = (*itr);
		buf.clear();
		uint32 count = pObj->BuildCreateUpdateBlockForPlayer( &buf, _player );
		_player->PushCreationData( &buf, count, pObj );
	}

	return true;
}

bool ChatHandler::HandleListMyAuras(const char* args, WorldSession *m_session)
{
	Player *_player = m_session->GetPlayer();

	//server flooding ?
	if( *_player->GetCreateIn64Extension( EXTENSION_ID_PLAYER_CHATCOMMAND_LISTAURAS_ANTISPAM ) > getMSTime() )
		return true;
	*_player->GetCreateIn64Extension( EXTENSION_ID_PLAYER_CHATCOMMAND_LISTAURAS_ANTISPAM ) = getMSTime() + 500;

	int Start = FIRST_AURA_SLOT,End = MAX_PASSIVE_AURAS;
	if( *args )
	{
		uint32 Param, ParamCount;
		ParamCount = sscanf(args, "%u", &Param );
		if( Param == 1 )
			End = MAX_POSITIVE_AURAS;
		else if( Param == 2 )
		{
			Start = MAX_POSITIVE_AURAS;
			End = MAX_AURAS;
		}
		else if( Param == 3 )
		{
			Start = MAX_AURAS;
			End = MAX_POSITIVE_AURAS;
		}
	}

	char ChatSendBuffer[65535];
	sprintf_s( ChatSendBuffer, 65535, "" );
	int SendCounter = 0;

	for( int32 i=Start;i<End;i++)
	{
		Aura *a = _player->m_auras[ i ];
		if( a != NULL )
		{
			sprintf_s( ChatSendBuffer, 65535, "%s,|cff71d5ff|Hspell:%d|h[%s(%d)]|h", ChatSendBuffer, a->GetSpellProto()->Id,  a->GetSpellProto()->Name, a->GetSpellProto()->Id );
			SendCounter++;
			if( SendCounter >= 10 )
			{
				_player->BroadcastMessage( "%s", ChatSendBuffer );
				sprintf_s( ChatSendBuffer, 65535, "" );
				SendCounter = 0;
			}
		}
	}

	return true;
}