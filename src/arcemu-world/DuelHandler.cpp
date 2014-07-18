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

void WorldSession::HandleDuelAccepted(WorldPacket & recv_data)
{
	if( !_player->IsInWorld() )
	{ 
		return;
	}

	if( _player->DuelingWith == NULL )
	{ 
		return;
	}

	if( _player->m_duelState != DUEL_STATE_FINISHED )
	{ 
		return;
	}
	
	if( _player->m_duelCountdownTimer > 0 )
	{ 
		return;
	}

//	_player->m_duelStatus = DUEL_STATUS_INBOUNDS;
//	_player->DuelingWith->m_duelStatus = DUEL_STATUS_INBOUNDS;

	_player->m_duelState = DUEL_STATE_COUNTDOWN;
	_player->DuelingWith->m_duelState = DUEL_STATE_COUNTDOWN;

	sStackWorldPacket( data, SMSG_DUEL_COUNTDOWN, 10 );
	data << uint32( DUEL_COUNTDOWN_MS );

	SendPacket( &data );
	_player->DuelingWith->m_session->SendPacket( &data );

	_player->m_duelCountdownTimer = DUEL_COUNTDOWN_MS;

	sEventMgr.AddEvent(_player, &Player::DuelCountdown, EVENT_PLAYER_DUEL_COUNTDOWN, 1000, DUEL_COUNTDOWN_MS / 1000, 0);
}

void WorldSession::HandleDuelCancelled(WorldPacket & recv_data)
{
	if( _player->DuelingWith ==  NULL )
	{ 
		return;
	}

	if( _player->m_duelState == DUEL_STATE_STARTED )
	{
		_player->DuelingWith->EndDuel( DUEL_WINNER_KNOCKOUT );
		return;
	}

	sStackWorldPacket( data, SMSG_DUEL_COMPLETE, 5 );
	data << uint8( 1 );

	SendPacket( &data );
	_player->DuelingWith->m_session->SendPacket( &data );

	GameObject* arbiter = _player->GetMapMgr() ? _player->GetMapMgr()->GetGameObject( _player->GetUInt64Value( PLAYER_DUEL_ARBITER ) ) : NULL;
	if( arbiter != NULL )
	{
		//there is a nifty trick here with punish tool. They create and remove the flag at the same time. Leaving a visual trace client side. Should send a delayed object delete for this also
		//the remaining flag is only a client side visual issue. Once you logout /relog they will be all gone
		arbiter->RemoveFromWorld( true );
		sGarbageCollection.AddObject( arbiter );
		arbiter = NULL;
 	}

	_player->DuelingWith->SetUInt64Value( PLAYER_DUEL_ARBITER, 0 );
	_player->SetUInt64Value( PLAYER_DUEL_ARBITER, 0 );

	_player->DuelingWith->SetUInt32Value( PLAYER_DUEL_TEAM, 0 );
	_player->SetUInt32Value( PLAYER_DUEL_TEAM, 0);

	_player->DuelingWith->m_duelState = DUEL_STATE_FINISHED;
	_player->m_duelState = DUEL_STATE_FINISHED;

	_player->DuelingWith->m_duelCountdownTimer = 0;
	_player->m_duelCountdownTimer = 0;

	_player->DuelingWith->DuelingWith = NULL;
	_player->DuelingWith = NULL;

}
