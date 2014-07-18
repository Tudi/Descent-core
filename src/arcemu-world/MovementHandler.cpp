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
#define SWIMMING_TOLERANCE_LEVEL -0.08f
#define MOVEMENT_PACKET_TIME_DELAY 400	//might be caused by emu sending packet in delayed mode

void WorldSession::HandleMoveWorldportAckOpcode( WorldPacket & recv_data )
{
	GetPlayer()->SetPlayerStatus(NONE);
//	if( sEventMgr.HasEvent( GetPlayer(), EVENT_CLIENT_FINISHED_LOADING_MAP ) == false )
//		sEventMgr.AddEvent( GetPlayer(), &Player::EventClientFinishedLoadingScreen, EVENT_CLIENT_FINISHED_LOADING_MAP, 5000, 1, 0 );
	if(_player->IsInWorld())
	{
		// get outta here
		return;
	}
	sLog.outDebug( "WORLD: got CMSG_MOVE_WORLDPORT_ACK." );
	
	if(_player->m_CurrentTransporter && _player->GetMapId() != _player->m_CurrentTransporter->GetMapId())
	{
		/* wow, our pc must really suck. */
		Transporter * pTrans = _player->m_CurrentTransporter;
		float c_tposx = pTrans->GetPositionX() + _player->m_TransporterX;
		float c_tposy = pTrans->GetPositionY() + _player->m_TransporterY;
		float c_tposz = pTrans->GetPositionZ() + _player->m_TransporterZ;

		WorldPacket dataw(SMSG_NEW_WORLD, 20);
		dataw << c_tposx << c_tposy << c_tposz << pTrans->GetMapId() << _player->GetOrientation();
		SendPacket(&dataw);
	}
	else
	{
		_player->m_TeleportState = 2;
		_player->AddToWorld();
	}
}

void WorldSession::HandleLoadingScreenStatusUpdate( WorldPacket & recv_data )
{
	uint8 status;
	uint32 map_id;

	recv_data >> status;
	recv_data >> map_id;

	//called even on DC
	if( GetPlayer() )
	{
		if( status & 0x80 )
		{
//printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!started loading!!!!!!!!!!!!!!!!!!!!\n");
//			GetPlayer()->m_ClientFinishedLoading = 0;
		}
		else
		{
//printf("###############################################finished loading######################\n");
//			sLog.outDebug("Client finished loading map");
//			GetPlayer()->m_ClientFinishedLoading |= CLIENT_FINISHED_LOADING_MAP;	
//			GetPlayer()->EventClientFinishedLoadingScreen();
			if( sEventMgr.HasEvent( GetPlayer(), EVENT_CLIENT_FINISHED_LOADING_MAP ) == false )
				sEventMgr.AddEvent( GetPlayer(), &Player::EventClientFinishedLoadingScreen, EVENT_CLIENT_FINISHED_LOADING_MAP, 5000, 1, 0 );
		}
	}
}

void WorldSession::HandleMoveTeleportAckOpcode( WorldPacket & recv_data )
{
//	uint64 guid;
//	WoWGuid guid;
//	recv_data >> guid;
/*
14333
{CLIENT} Packet: (0xB8C3) UNKNOWN PacketSize = 13 stamp = 18912453
|------------------------------------------------|----------------|
|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
|------------------------------------------------|----------------|
|39 69 95 20 01 00 00 00 00 02 03 05 04          |9i. .........   | //guid is 05 04 03 02 00 00 00 00 -> it is obfuscated
*/
//	if(guid == _player->GetGUID())
//	if(guid.GetOldGuid() == _player->GetGUID())
	{
		if(sWorld.antihack_teleport && !(HasPermissions() && sWorld.no_antihack_on_gm) && _player->GetPlayerStatus() != TRANSFER_PENDING)
		{
			/* we're obviously cheating */
			sCheatLog.writefromsession(this, "Used teleport hack, disconnecting.");
			GetPlayer()->SoftDisconnect();
			return;
		}

		if(sWorld.antihack_teleport && !(HasPermissions() && sWorld.no_antihack_on_gm) && _player->m_position.DistanceSq(_player->m_sentTeleportPosition) > 625.0f)	/* 25.0f*25.0f */
		{
			/* cheating.... :( */
			sCheatLog.writefromsession(this, "Used teleport hack {2}, disconnecting.");
			GetPlayer()->SoftDisconnect();
			return;
		}

		sLog.outDebug( "WORLD: got CMSG_MOVE_TELEPORT_ACK." );
		GetPlayer()->SetPlayerStatus(NONE);
//		if( sEventMgr.HasEvent( GetPlayer(), EVENT_CLIENT_FINISHED_LOADING_MAP ) == false )
//			sEventMgr.AddEvent( GetPlayer(), &Player::EventClientFinishedLoadingScreen, EVENT_CLIENT_FINISHED_LOADING_MAP, 5000, 1, 0 );
		if( GetPlayer()->m_rooted <= 0 )
			GetPlayer()->SetMovement(MOVE_UNROOT);
		_player->SpeedCheatReset();

		m_bIsWLevelSet = false;
		m_wLevel = -5000;

		if(_player->m_sentTeleportPosition.x != 999999.0f)
		{
//			_player->m_position = _player->m_sentTeleportPosition;
			_player->SetPosition( _player->m_sentTeleportPosition );
			_player->m_sentTeleportPosition.ChangeCoords(999999.0f,999999.0f,999999.0f);
		}

		if(GetPlayer()->GetSummon() != NULL)		// move pet too
			GetPlayer()->GetSummon()->SetPosition((GetPlayer()->GetPositionX() + 2), (GetPlayer()->GetPositionY() + 2), GetPlayer()->GetPositionZ(), float(M_PI));
	}

}

void _HandleBreathing(MovementInfo &movement_info, Player * _player, WorldSession * pSession, WorldPacket & recv_data )
{

	// no water breathing is required
	if( !sWorld.BreathingEnabled || _player->FlyCheat 
//		|| _player->m_bUnlimitedBreath 
		|| !_player->isAlive() || _player->bInvincible )
	{
		// player is flagged as in water or lava or slime
		if( _player->m_UnderwaterState & (UNDERWATERSTATE_SWIMMING |UNDERWATERSTATE_LAVA | UNDERWATERSTATE_SLIME ) )
			_player->m_UnderwaterState &= ~(UNDERWATERSTATE_SWIMMING |UNDERWATERSTATE_LAVA | UNDERWATERSTATE_SLIME );

		// player is flagged as under water
		if( _player->m_UnderwaterState & UNDERWATERSTATE_UNDERWATER )
		{
			_player->m_UnderwaterState &= ~UNDERWATERSTATE_UNDERWATER;
/*
{SERVER} Packet: (0xAD74) SMSG_START_MIRROR_TIMER PacketSize = 21 TimeStamp = 7400781
01 00 00 00 
20 BF 02 00 
20 BF 02 00 
FF FF FF FF 
00 
00 00 00 00 spellid
*/
			if( _player->m_bUnlimitedBreath == false )
			{
				WorldPacket data(SMSG_START_MIRROR_TIMER, 20);
				data << uint32( TIMER_BREATH ) << _player->m_UnderwaterTime << _player->m_UnderwaterMaxTime << int32(-1) << uint32(0) << uint8(0);
				pSession->SendPacket(&data);
			}
		}

		// player is above water level
		if( pSession->m_bIsWLevelSet )
		{
			if( ( movement_info.z 
//				+ _player->m_noseLevel
				) > pSession->m_wLevel )
			{
				_player->RemoveAurasByInterruptFlag( AURA_INTERRUPT_ON_LEAVE_WATER );
				// unset swim session water level
				pSession->m_bIsWLevelSet = false;
			}
		}

		return;
	}

	//check lava and slime
	ZLiquidStatus LiquidStatus = sTerrainMgr.GetLiquidStatus( _player->GetMapId(), movement_info.x, movement_info.y, movement_info.z );
	_player->m_UnderwaterState &= ~( UNDERWATERSTATE_LAVA | UNDERWATERSTATE_SLIME );
	bool nose_under_water = false;
	if( LiquidStatus & ( LIQUID_MAP_IN_WATER | LIQUID_MAP_UNDER_WATER ) )
	{
		uint8 terraininfo = sTerrainMgr.GetLiquidFlags( _player->GetMapId(), movement_info.x, movement_info.y, movement_info.z );
//float LiquidHeight = sTerrainMgr.GetLiquidHeight( _player->GetMapId(), movement_info.x, movement_info.y, movement_info.z );
//float suggested_z = _player->GetMapMgr()->GetLandHeight( _player->GetPositionX(), _player->GetPositionY(), movement_info.z );
//sLog.outDebug("We are in liquid type %d, our level %f, liquid level %f, earth level %f", terraininfo, movement_info.z, LiquidHeight, suggested_z );
		if( terraininfo & MAP_LIQUID_TYPE_MAGMA )
			_player->m_UnderwaterState |= UNDERWATERSTATE_LAVA;
		if( terraininfo & MAP_LIQUID_TYPE_SLIME )
			_player->m_UnderwaterState |= UNDERWATERSTATE_SLIME;
		if( terraininfo & ( MAP_LIQUID_TYPE_WATER | MAP_LIQUID_TYPE_OCEAN ) && ( LiquidStatus & LIQUID_MAP_UNDER_WATER ) )
			nose_under_water = true;
	}

	//player is swiming and not flagged as in the water
	if( recv_data.GetOpcode() == MSG_MOVE_START_SWIM
//		movement_info.flags & MOVEFLAG_SWIMMING && 
		&& !( _player->m_UnderwaterState & UNDERWATERSTATE_SWIMMING ) )
	{
		// dismount if mounted
		_player->RemoveAurasByInterruptFlag( AURA_INTERRUPT_ON_ENTER_WATER );

		// get water level only if it was not set before
		if( !pSession->m_bIsWLevelSet )
		{
			// water level is somewhere below the nose of the character when entering water
			pSession->m_wLevel = movement_info.z + _player->m_noseLevel * 0.95f;
			pSession->m_bIsWLevelSet = true;
		}

		_player->m_UnderwaterState |= UNDERWATERSTATE_SWIMMING;
	}

	// player is not swimming and is not stationary and is flagged as in the water
	if( 
//		( movement_info.flags & ( MOVEFLAG_SWIMMING | MOVEFLAG_MOVE_STOP ) ) == 0 
//		( recv_data.GetOpcode() == MSG_MOVE_STOP_SWIM || recv_data.GetOpcode() == MSG_MOVE_JUMP_STOP_SWIM )
//		( recv_data.GetOpcode() != MSG_MOVE_START_SWIM && recv_data.GetOpcode() != MSG_MOVE_STOP_SWIM && recv_data.GetOpcode() != MSG_MOVE_JUMP_STOP_SWIM )
//		&& 
		_player->m_UnderwaterState & UNDERWATERSTATE_SWIMMING )
	{
		// player is above water level
		if( ( movement_info.z - 2.0f + _player->m_noseLevel ) > pSession->m_wLevel )
		{
			_player->RemoveAurasByInterruptFlag( AURA_INTERRUPT_ON_LEAVE_WATER );
	
			// unset swim session water level
			pSession->m_bIsWLevelSet = false;

			_player->m_UnderwaterState &= ~UNDERWATERSTATE_SWIMMING;
		}
	}

	// player is flagged as in the water and is not flagged as under the water
	if( _player->m_UnderwaterState & UNDERWATERSTATE_SWIMMING && !( _player->m_UnderwaterState & UNDERWATERSTATE_UNDERWATER ) )
	{
		//the player is in the water and has gone under water, requires breath bar.
		if( ( movement_info.z + _player->m_noseLevel ) < pSession->m_wLevel )
		{
			_player->m_UnderwaterState |= UNDERWATERSTATE_UNDERWATER;
			if( _player->m_bUnlimitedBreath == false )
			{
				WorldPacket data(SMSG_START_MIRROR_TIMER, 20);
				data << uint32( TIMER_BREATH ) << _player->m_UnderwaterTime << _player->m_UnderwaterMaxTime << int32(-1) << uint8(0) << uint32(0);
				pSession->SendPacket(&data);
			}
		}
	}

	// player is flagged as in the water and is flagged as under the water
	if( ( _player->m_UnderwaterState & UNDERWATERSTATE_SWIMMING ) && ( _player->m_UnderwaterState & UNDERWATERSTATE_UNDERWATER ) )
	{
		//the player is in the water but their face is above water, no breath bar neeeded.
		if( ( movement_info.z + _player->m_noseLevel ) > pSession->m_wLevel )
		{
			_player->m_UnderwaterState &= ~UNDERWATERSTATE_UNDERWATER;
			WorldPacket data(SMSG_START_MIRROR_TIMER, 20);
			data << uint32( TIMER_BREATH ) << _player->m_UnderwaterTime << _player->m_UnderwaterMaxTime << int32(10) << uint8(0) << uint32(0);
			pSession->SendPacket(&data);
		}
	}

	// player is flagged as not in the water and is flagged as under the water
	if( ( ( _player->m_UnderwaterState & UNDERWATERSTATE_SWIMMING ) == 0 || nose_under_water == false ) && ( _player->m_UnderwaterState & UNDERWATERSTATE_UNDERWATER ) )
	{
		//the player is out of the water, no breath bar neeeded.
		if( ( movement_info.z + _player->m_noseLevel ) > pSession->m_wLevel )
		{
			_player->m_UnderwaterState &= ~UNDERWATERSTATE_UNDERWATER;
			WorldPacket data(SMSG_START_MIRROR_TIMER, 20);
			data << uint32( TIMER_BREATH ) << _player->m_UnderwaterTime << _player->m_UnderwaterMaxTime << int32(10) << uint8(0) << uint32(0);
			pSession->SendPacket(&data);
		}
	}

}

//this is valid only on local testing to avoid thread collisions
void DumpUnhandledMovePacket_( WorldPacket &data, char *comment )
{
//#define I_WAT_TO_CATCH_MOVEMENTBUGS 

#if defined( _DEBUG ) || defined( I_WAT_TO_CATCH_MOVEMENTBUGS )
	FILE *f = fopen("UnkMovePackets.log","at");
	if( f )
	{
		fprintf( f, "{CLIENT} Packet: ( %05d ) %s PacketSize=%d Comment=%s\n",data.GetOpcode(),LookupName(data.GetOpcode(), g_worldOpcodeNames), data.size(), comment );
		for( uint32 i=0; i<data.size(); i++)
			fprintf( f,"%02X ",data.contents()[i]);
		fprintf(f,"\n\n");
		fclose(f);
	}
#endif
}

// Zack : i'm not shitting you. There is a hack to move some other player by chaning the "mover guid" inside the packet 
// to prevent this we should know the active mover guid all the time and skip all other packets
void WorldSession::HandleMovementOpcodes( WorldPacket & recv_data )
{
//	_player->m_ClientFinishedLoading |= CLIENT_FINISHED_CREATING_PLAYER;

	if(!_player->IsInWorld() || _player->m_uint32Values[UNIT_FIELD_CHARMEDBY] || _player->GetPlayerStatus() == TRANSFER_PENDING || _player->GetTaxiState())
	{ 
		return;
	}

	// spell cancel on movement, for now only fishing is added
	Object * t_go = _player->m_SummonedObject;
	if (t_go)
	{
		if (t_go->GetEntry() == GO_FISHING_BOBBER)
			SafeGOCast(t_go)->EndFishing(GetPlayer(),true);
	}

	/************************************************************************/
	/* Clear standing state to stand.				                        */
	/************************************************************************/
//	if( recv_data.GetOpcode() == MSG_MOVE_START_FORWARD )
//		_player->SetStandState( STANDSTATE_STAND );
	/************************************************************************/
	/* Make sure the packet is the correct size range.                      */
	/************************************************************************/
/*	if (recv_data.size() > sizeof(MovementInfo) + 16) 
	{ 
		GetPlayer()->SoftDisconnect(); 
		return; 
	}/**/

	/************************************************************************/
	/* Read Movement Data Packet                                            */
	/************************************************************************/
#ifndef JayZMoveHandler
	movement_info.init(recv_data,m_MoverWoWGuid);
#else
	movement_info.ReadMovementInfo( recv_data );
	//not kidding. When stopping strafe we receive no proper orientation. We set orientation unchanged
	if( ( movement_info.orientation > -0.01f && movement_info.orientation < 0.01f && _player->strafing ) || recv_data.GetOpcode() == MSG_MOVE_STOP_STRAFE )
	{
//GetPlayer()->BroadcastMessage( "!!Strafe %s, Orientation %f", LookupName(recv_data.GetOpcode(), g_worldOpcodeNames), movement_info.orientation );
		movement_info.orientation = prev_movement_info.orientation;
	}
//GetPlayer()->BroadcastMessage( "1Opcode %s, Orientation %f", LookupName(recv_data.GetOpcode(), g_worldOpcodeNames), movement_info.orientation );

	char unhandled_reason[5000];
	unhandled_reason[0]=0;
	bool SecondChance = false;
TRY_SECOND_CHANCE:
	if( SecondChance == true )
	{
		recv_data.ResetRead();
		movement_info.ReadMovementInfoSecondChance( recv_data );
	}

	//not yet implemented movement handler
#ifdef _DEBUG
	if( movement_info.x == -1 )
		sprintf( unhandled_reason, "%s - not yet implemented handler",unhandled_reason);
#endif
	if( movement_info.x == -1.0f ) 
		sprintf( unhandled_reason, "%s - Unhandled packet with opcode %d ", unhandled_reason, recv_data.GetOpcode() );
	if( ( abs( prev_movement_info.x - movement_info.x ) > _player->m_flySpeed * 7 && abs( prev_movement_info.y - movement_info.y ) < _player->m_flySpeed * 7 	) && prev_movement_info.x != 0 ) 
		sprintf( unhandled_reason, "%s - X diff is too large %f, limit %f",unhandled_reason,prev_movement_info.x - movement_info.x, _player->m_flySpeed * 7 );
	if( ( abs( prev_movement_info.x - movement_info.x ) < _player->m_flySpeed * 7 && abs( prev_movement_info.y - movement_info.y ) > _player->m_flySpeed * 7 	) && prev_movement_info.x != 0 ) 
		sprintf( unhandled_reason, "%s - Y diff is too large %f, limit %f",unhandled_reason,prev_movement_info.y - movement_info.y, _player->m_flySpeed * 7 );
	if( movement_info.orientation > 3*M_PI ) 
		sprintf( unhandled_reason, "%s - Abnormal orientation %f",unhandled_reason,movement_info.orientation);
	if( movement_info.orientation < -3*M_PI ) 
		sprintf( unhandled_reason, "%s - Abnormal orientation %f",unhandled_reason,movement_info.orientation);
	if( movement_info.FallTime > 50000 ) 
		sprintf( unhandled_reason, "%s - Abnormal falltime %d",unhandled_reason,movement_info.FallTime);
	if( movement_info.moverGUID != GetPlayer()->GetGUID() ) 
		sprintf( unhandled_reason, "%s - Bad mover guid %d ",unhandled_reason,(uint32)movement_info.moverGUID);
	if( unhandled_reason[0]!=0 )
	{
		if( SecondChance == false )
		{
			unhandled_reason[0] = 0;
			SecondChance = true;
			goto TRY_SECOND_CHANCE;
		}
		DumpUnhandledMovePacket_( recv_data, unhandled_reason );
		sLog.outDebug("!!Bad data for movement info for opcode 0x%04X = %s x diff %05.05f , y diff %05.05f",recv_data.GetOpcode(),LookupName(recv_data.GetOpcode(), g_worldOpcodeNames),abs( prev_movement_info.x - movement_info.x ),abs( prev_movement_info.y - movement_info.y ));
		sLog.outDebug("!!Bad data for movement info for opcode 0x%04X = %s o %05.05f , t %05d",recv_data.GetOpcode(),LookupName(recv_data.GetOpcode(), g_worldOpcodeNames),movement_info.orientation,movement_info.FallTime);
		prev_movement_info.x = 0;
		return;
	}
	memcpy( &prev_movement_info, &movement_info, sizeof( movement_info ) );
#endif
#ifdef _DEBUG
//	if( movement_info.mover_guid != 0 && movement_info.mover_guid != m_MoverWoWGuid.GetOldGuid() )
//		printf(" There is an issue with opcode %s = %05X \n", LookupName(recv_data.GetOpcode(), g_worldOpcodeNames),recv_data.GetOpcode() );
#endif

	//another WPE exploit to move other player chars only client side(i think )
/*	if( 
		movement_info.ControlerGUID.GetOldGuid() != 0	//crap in 420 client this is not sent anymore
		&&	movement_info.ControlerGUID.GetOldGuid() != _player->GetGUID() 
		&& movement_info.ControlerGUID.GetOldGuid() != m_MoverWoWGuid.GetOldGuid() )
	{
		return;
	}*/
	//stupid WPE hack that crashes other clients
	if( *(uint32*)&movement_info.z == 0xFFFFFFFE || movement_info.z <= -5000 || movement_info.z >= 5000 )
	{
		sCheatLog.writefromsession(this, "Player %s is probably WPE plane hacking with z = %f.",_player->GetName(),movement_info.z);
		GetPlayer()->SoftDisconnect();
		return; //!! do not send corrupted packet to others or they will 132 crash
	}

	//WPE FLYHACK enable check
	if( recv_data.GetOpcode() == CMSG_MOVE_SET_FLY 
		&& !(!sWorld.antihack_flight || _player->m_TransporterGUID != 0 || _player->GetTaxiState() || (sWorld.no_antihack_on_gm && _player->GetSession()->HasPermissions()) )
		&& !_player->flying_aura && !_player->FlyCheat
		)
	{
		_player->BroadcastMessage( "Flyhack detected. In case server was wrong then make a report how to reproduce this case. You will be logged out in 7 seconds." );
		sCheatLog.writefromsession( _player->GetSession(), "Caught %s fly hacking", _player->GetName() );
		sEventMgr.AddEvent( _player, &Player::_Kick, EVENT_PLAYER_KICK, 7000, 1, 0 );
	}

	//rXYZ memory edit tele hack. If you move you will start falling, there is no golden rule for the fall time
//#define DETECT_rXYZ_MEMORY_JUMP_FLYHACK
#ifdef DETECT_rXYZ_MEMORY_JUMP_FLYHACK
	if( movement_info.FallTime == 0 
//		&& !_player->flying_aura //adding this as temp solution because people keep reporting they are rolled back a lot :(
		)
	{
		if( recv_data.GetOpcode() == MSG_MOVE_START_TURN_RIGHT )
		{
			_player->SDetector->rot_xp = _player->GetPositionX();
			_player->SDetector->rot_yp = _player->GetPositionY();
			_player->SDetector->rot_zp = _player->GetPositionZ();
			float cur_dist = Distance3DSq( movement_info.x, movement_info.y, movement_info.z, _player->SDetector->rot_x, _player->SDetector->rot_y, _player->SDetector->rot_z );
			if( cur_dist > 0.0 )
				_player->SDetector->rot_seq = 1;
			else
				_player->SDetector->rot_seq = 0;
			_player->SDetector->rot_x = movement_info.x;
			_player->SDetector->rot_y = movement_info.y;
			_player->SDetector->rot_z = movement_info.z;
		}
		else if( _player->SDetector->rot_seq == 1 && recv_data.GetOpcode() == MSG_MOVE_STOP_TURN )
		{
			float cur_dist = Distance3DSq( movement_info.x, movement_info.y, movement_info.z, _player->SDetector->rot_x, _player->SDetector->rot_y, _player->SDetector->rot_z );
			if( cur_dist == 0.0 )
				_player->SDetector->rot_seq = 2;
		}
		else if( _player->SDetector->rot_seq == 2 && recv_data.GetOpcode() == MSG_MOVE_START_TURN_LEFT )
		{
			float cur_dist = Distance3DSq( movement_info.x, movement_info.y, movement_info.z, _player->SDetector->rot_x, _player->SDetector->rot_y, _player->SDetector->rot_z );
			if( cur_dist == 0.0 )
				_player->SDetector->rot_seq = 3;
		}
		else if( _player->SDetector->rot_seq == 3 && recv_data.GetOpcode() == MSG_MOVE_STOP_TURN )
		{
			float cur_dist = Distance3DSq( movement_info.x, movement_info.y, movement_info.z, _player->SDetector->rot_x, _player->SDetector->rot_y, _player->SDetector->rot_z );
			if( cur_dist == 0.0 )
				_player->SDetector->rot_seq = 4;
			//time to trigger teleport back ?
			if( _player->SDetector->rot_seq == 4 )
			{
				_player->SpeedCheatDelay( 4000 );
				if( _player->m_MountSpellId )
					_player->BroadcastMessage("Collision Error. Auto adjusting");
				_player->SafeTeleport( _player->GetMapId(), _player->GetInstanceID(), _player->SDetector->rot_xp, _player->SDetector->rot_yp, _player->SDetector->rot_zp, movement_info.orientation );
				return; //or else you can get booted for "some" reason
			}
		}
		else
		{
			_player->SDetector->rot_seq = 0;
//			_player->SDetector->rot_x = movement_info.x;
//			_player->SDetector->rot_y = movement_info.y;
//			_player->SDetector->rot_z = movement_info.z;
		}
	}
/*	else
	{
		_player->SDetector->rot_x = movement_info.x;
		_player->SDetector->rot_y = movement_info.y;
		_player->SDetector->rot_z = movement_info.z;
	}*/
#endif
#if 0
		//right now it uses these opcodes, might need to add more later
		if( recv_data.GetOpcode() == MSG_MOVE_START_TURN_LEFT || recv_data.GetOpcode() == MSG_MOVE_START_TURN_RIGHT || recv_data.GetOpcode() == MSG_MOVE_STOP_TURN 
//			|| recv_data.GetOpcode() == MSG_MOVE_SET_PITCH 
//			|| recv_data.GetOpcode() == MSG_MOVE_START_PITCH_DOWN || recv_data.GetOpcode() == MSG_MOVE_START_PITCH_UP
//			|| recv_data.GetOpcode() == MSG_MOVE_SET_FACING
			)
		{
			//check for fly cheat 
//			if( _player->flying_aura == 0 )
			{
				float last_z = _player->SDetector->GetLastZ();
				_player->SDetector->SetLastZ( movement_info.z );
				float Z_diff = last_z - movement_info.z;
				//we teleported up or down and there is no fall detection by the client
				if( Z_diff != 0.0f )
				{
					float suggested_z = _player->GetMapMgr()->GetLandHeight( _player->GetPositionX(), _player->GetPositionY(), movement_info.z );
					//we can compare our hight to a valid height ?
					if( suggested_z != VMAP_VALUE_NOT_INITIALIZED )
					{
						float Z_diff_earth = movement_info.z - suggested_z; //negative is below ground
						if( Z_diff_earth > 3 ) //flyhack ?
						{
							_player->SDetector->cheat_threat_z+=1;
							_player->BroadcastMessage("Underground fall detector : Height difference is %f. Prepare for autoadjust", Z_diff_earth );
						}
			/*				else if( Z_diff_earth < -10 ) //fall underground ?
						{
							_player->SDetector->cheat_threat_z+=2;
							_player->BroadcastMessage("Underground fall detector : Height difference is %f. Prepare for autoadjust", Z_diff_earth );
						} */
						else
						{
							_player->SDetector->cheat_threat_z = 0;
						}
					}
				}
			}
#endif
#ifdef _DEBUG
	if( _player->GetExtension( EXTENSION_ID_POSITION_X_RECORD ) )
	{
		int64 *X = _player->GetCreateIn64Extension( EXTENSION_ID_POSITION_X_RECORD );
		int64 *Y = _player->GetCreateIn64Extension( EXTENSION_ID_POSITION_Y_RECORD );
		int64 NX = _player->GetPositionX();
		int64 NY = _player->GetPositionY();
		if( sqrt( double( (*X-NX)*(*X-NX) + (*Y-NY)*(*Y-NY) ) ) > 3 * 3 )
		{
			FILE *f = fopen("BGRealmSpawns.txt","at");
			if( f )
			{
				*X = NX;
				*Y = NY;
				fprintf( f, "array(%d,%d,%d,%d),", (int)_player->GetPositionX(), (int)_player->GetPositionY(), (int)_player->GetPositionZ(), (int)_player->GetOrientation() );
				fclose( f );
			}
		}
	}
#endif
	/************************************************************************/
	/* Update player movement state                                         */
	/************************************************************************/
	bool moved = true;
	switch( recv_data.GetOpcode() )
	{
	case MSG_MOVE_HEARTBEAT:
		moved = _player->m_isMoving;
		break; //we continue doing what we were doing before
	case MSG_MOVE_START_FORWARD:
	case MSG_MOVE_START_BACKWARD:
		_player->moving = true;
		break;
	case MSG_MOVE_START_STRAFE_LEFT:
	case MSG_MOVE_START_STRAFE_RIGHT:
		_player->strafing = true;
		break;
/*	case MSG_MOVE_JUMP:
		_player->jumping = true;
		break;*/
	case MSG_MOVE_STOP:
		_player->moving = false;
		break;
	case MSG_MOVE_STOP_STRAFE:
		_player->strafing = false;
		break;
/*	case MSG_MOVE_FALL_LAND:
		_player->jumping = false;
		break;*/
	default:
		moved = false;
		break;
	}
	
	if( moved )
	{
		if( !_player->moving && !_player->strafing && !_player->jumping )
			_player->m_isMoving = false;
		else
			_player->m_isMoving = true;
	}

	if( _player->moving )
		_player->last_moved = getMSTime();

	//update the detector -> make sure hearthbeat messages are not eaten by below code !
	if( sWorld.antihack_speed )
	{
		if( !_player->GetTaxiState() && _player->m_TransporterGUID == 0 )
		{
			// simplified: just take the fastest speed. less chance of fuckups too
			float speed = ( _player->flying_aura ) ? _player->m_flySpeed : _player->m_runSpeed;
			speed = MAX( speed, _player->m_swimSpeed ); //damn glyph of aquatic form

//			_player->SDetector->AddSample( movement_info.x, movement_info.y, movement_info.time, speed );
			_player->SDetector->AddSample( movement_info.x, movement_info.y, movement_info.z, 0, speed, GetLatency() );

			if( _player->SDetector->IsCheatDetected() )
			{
				_player->SDetector->ReportCheater( _player );
				return;
			}
		}
		else
			_player->SDetector->EventSpeedChange();
	}

/*
	//avoid accepting client spamming us with updates. If the change is so small it is not worth updating then ignore whole packet
#define UNWORTHY_POS_UPDATE_SIZE 0.25
#define UNWORTHY_ORIENTATION_UPDATE_SIZE ( 6.28 / 32 )
	if( recv_data.GetOpcode() == _player->last_received_move_opcode
		&& abs( _player->GetPositionX() - movement_info.x ) < UNWORTHY_POS_UPDATE_SIZE
		&& abs( _player->GetPositionY() - movement_info.y ) < UNWORTHY_POS_UPDATE_SIZE
		&& abs( _player->GetPositionZ() - movement_info.z ) < UNWORTHY_POS_UPDATE_SIZE
		&& abs( _player->GetOrientation() - movement_info.orientation ) < UNWORTHY_ORIENTATION_UPDATE_SIZE )
	{
//printf("!!!!!!! eliminated packet sending\n");
		return;
	} /**/
	_player->last_received_move_opcode = recv_data.GetOpcode();

	if( _player->GetVehicle() 
//		&& movement_info.ControlerGUID.GetOldGuid() == _player->GetVehicle()->GetGUID() 
		)
	{
		Vehicle *veh = _player->GetVehicle( );
		if( veh && veh->GetVehicleController() == _player )
			veh->MoveVehicle( movement_info.x, movement_info.y, movement_info.z, movement_info.orientation );
	}

	// if we are simply moving.
	// todo need to check and confirm all this
	//check infinit fall
	if(
//		( movement_info.flags & MOVEFLAG_FALLING ) 
//		&& abs( _player->z_axisposition - movement_info.z ) > 200
		movement_info.FallTime > 5000	//10 sec fall time ? quite large ?
		)
	{
		float suggested_z = _player->GetMapMgr()->GetLandHeight( _player->GetPositionX(), _player->GetPositionY(), movement_info.z );
		if( suggested_z != VMAP_VALUE_NOT_INITIALIZED )
		{
			_player->SpeedCheatDelay( 4000 );
			_player->SafeTeleport( _player->GetMapId(), _player->GetInstanceID(), _player->GetPositionX(), _player->GetPositionY(), suggested_z+10, _player->GetOrientation() );
			return; //or else you get booted for "some" reason
		}
	}
#ifdef ENABLE_STATISTICS_GENERATED_HEIGHTMAP
//printf("prinf Our height %f and suggested one %f flags %u and f2 %u\n",movement_info.z,sSVMaps.GetHeight( _player->GetMapId(), movement_info.x, movement_info.y, movement_info.z ),movement_info.flags,movement_info.flags2);
//printf("prinf Our height %f and suggested one %f \n",movement_info.z,sTerrainMgr.GetHeight( _player->GetMapId(), movement_info.x, movement_info.y, movement_info.z ));
//printf("prinf area id %d, liquid flags %d \n",sTerrainMgr.GetAreaID( _player->GetMapId(), movement_info.x, movement_info.y, movement_info.z ),sTerrainMgr.GetLiquidFlags( _player->GetMapId(), movement_info.x, movement_info.y, movement_info.z ));
	//add this sample to our heightMapCollector
	if( 
//		movement_info.flags <= MOVEFLAG_STRAFE_RIGHT 
//		movement_info.flags <= (1 << 7 ) 
//		&& movement_info.movementflags == 0 
//		&& recv_data.GetOpcode() != MSG_MOVE_FALL_LAND
		movement_info.FallTime == 0
		&& ( movement_info.flags & ( MOVEFLAG_TURN_LEFT14333 | MOVEFLAG_TURN_RIGHT14333 | MOVEFLAG_FALLING1433 | MOVEFLAG_FLYING14333 | MOVEFLAG_SWIMMING14333 | MOVEFLAG_AIR_SUSPENSION14333 ) ) == 0	//do not sample when inside water or air swim bug
		&& _player->GetTaxiState() == false
		&& _player->m_TransporterGUID == 0
		&& _player->flying_aura == 0
		)
	{
		//rXYZ is a memory hack that uses turning packets to move players in the air. Do not sample 
		//right now these are used. Might need to add more later
		if( recv_data.GetOpcode() != MSG_MOVE_START_TURN_LEFT
			&& recv_data.GetOpcode() != MSG_MOVE_STOP_TURN
			&& recv_data.GetOpcode() != MSG_MOVE_START_TURN_RIGHT )
			sSVMaps.AddSample( _player->GetMapId(), movement_info.x, movement_info.y, movement_info.z );
//		printf("!!! add sample !! and f1 = %u and f2 = %u, ft = %u\n",movement_info.flags,movement_info.flag16,movement_info.FallTime );
	} 
#endif

//printf("opcode %u and difs %f,%f,%f,%f\n",_player->last_received_move_opcode,abs( _player->GetPositionX() - movement_info.x ),abs( _player->GetPositionY() - movement_info.y ),abs( _player->GetPositionZ() - movement_info.z ),abs( _player->GetOrientation() - movement_info.orientation ));

	//Anti Multi-Jump Check
/*	if( recv_data.GetOpcode() == MSG_MOVE_JUMP && _player->jumping == true && !GetPermissionCount())
	{
		sCheatLog.writefromsession(this, "Detected jump hacking for player %s",_player->GetName() );
		GetPlayer()->SoftDisconnect();
		return;
	}
//	if( recv_data.GetOpcode() == MSG_MOVE_FALL_LAND || (movement_info.flags & MOVEFLAG_SWIMMING) )
	if( recv_data.GetOpcode() == MSG_MOVE_START_SWIM )
		_player->jumping = false;
//	if( !_player->jumping && (recv_data.GetOpcode() == MSG_MOVE_JUMP || (movement_info.flags & MOVEFLAG_FALLING)))
	if( !_player->jumping && (recv_data.GetOpcode() == MSG_MOVE_JUMP || movement_info.FallTime != 0 ) )
		_player->jumping = true;
*/
	/************************************************************************/
	/* Remove Emote State                                                   */
	/************************************************************************/
	if(_player->m_uint32Values[UNIT_NPC_EMOTESTATE])
		_player->SetUInt32Value(UNIT_NPC_EMOTESTATE,0);

	/************************************************************************/
	/* Make sure the co-ordinates are valid.                                */
	/************************************************************************/
	if( !((movement_info.y >= _minY) && (movement_info.y <= _maxY)) || !((movement_info.x >= _minX) && (movement_info.x <= _maxX)) )
	{
		GetPlayer()->SoftDisconnect();
		return;
	}

	/************************************************************************/
	/* Dump movement flags - Wheee!                                         */
	/************************************************************************/
#if 0
	printf("=========================================================\n");
	printf("Full movement flags: 0x%.8X\n", movement_info.flags);
	uint32 z, b;
	for(z = 1, b = 1; b < 32;)
	{
		if(movement_info.flags & z)
			printf("   Bit %u (0x%.8X or %u) is set!\n", b, z, z);

		z <<= 1;
		b+=1;
	}
	printf("=========================================================\n");
#endif

	/************************************************************************/
	/* Orientation dumping                                                  */
	/************************************************************************/
#if 0
	printf("Packet: 0x%03X (%s)\n", recv_data.GetOpcode(), LookupName( recv_data.GetOpcode(), g_worldOpcodeNames ) );
	printf("Orientation: %.10f\n", movement_info.orientation);
#endif

	/************************************************************************/
	/* Calculate the timestamp of the packet we have to send out            */
	/************************************************************************/
	/************************************************************************/
	/* Copy into the output buffer.                                         */
	/************************************************************************/
	if( _player->m_inRangePlayers.empty() == false 
//&& _player->GetNewGUID().GetNewGuidLen() > 3
//&& recv_data.GetOpcode() == MSG_MOVE_START_TURN_RIGHT
		)
	{
#ifndef JayZMoveHandler
/*
{SERVER} Packet: (0x5341) SMSG_PLAYER_MOVE PacketSize = 32 TimeStamp = 3277877
1C 00 51 - it has 6 bits = number of bytes in guid
00 00 00 00 - maybe some flags, maybe same as received at position 7
84 40 18 40 2.3789377212524  orientation ?
7E maybe g[0] - 1?
9B F7 CF 05 - time in MS
A5 maybe g[1] + 1 ?
50 maybe g[2] - 1 ?
D0 72 09 C6 -8796.703125 		x
E3 B7 CE C3 -413.43661499023 	y
29 21 A3 42 81.564765930176 	z
05 maybe g[3] + 1 ?
00 maybe g[7] - 1 ?
*/
		uint64 old_guid = m_MoverWoWGuid.GetOldGuid();
		uint8  *guid_bytes = (uint8*)&old_guid;
/*		MoveinfoFlags tflags;
		memset( &tflags, 0, sizeof( tflags ) );
		if( guid_bytes[0] )
			tflags.GuidByte0 = 1;
		if( guid_bytes[1] )
			tflags.GuidByte1 = 1;
		if( guid_bytes[2] )
			tflags.GuidByte2 = 1;
		if( guid_bytes[3] )
			tflags.GuidByte3 = 1;
		if( guid_bytes[4] )
			tflags.GuidByte4 = 1;
		if( guid_bytes[5] )
			tflags.GuidByte5 = 1;
		if( guid_bytes[6] )
			tflags.GuidByte6 = 1;
		if( guid_bytes[7] )
			tflags.GuidByte7 = 1; */
		sStackWorldPacket( data, SMSG_PLAYER_MOVE, 33 );
		data << uint8( 0x1C );
		data << uint8( 0 );
		data << uint8( 0x50 );
//		data.Write( (const uint8*)&tflags, 3 );
		data << uint32( 0 );	//movement_info.flag - aaaargh, seems to make char randomly move, like spline is sent or something
		data << float( movement_info.orientation );
//		if( guid_bytes[0] )
			data << uint8( ObfuscateByte( guid_bytes[0] ) );
		data << uint32( movement_info.client_ms_time );
//		if( guid_bytes[1] )
			data << uint8( ObfuscateByte( guid_bytes[1] ) );
//		if( guid_bytes[2] )
			data << uint8( ObfuscateByte( guid_bytes[2] ) );
		data << float( movement_info.x );
		data << float( movement_info.y );
		data << float( movement_info.z );
//		if( guid_bytes[3] )
			data << uint8( ObfuscateByte( guid_bytes[3] ) );
//		int32 move_time;
//		move_time = (movement_info.time - (mstime - m_clientTimeDelay)) + MOVEMENT_PACKET_TIME_DELAY + mstime;
		//zack : client sends time as he's getmstime(). Server ads lag compensation to server getmstime() and sends to other clients
		// destination client tries to compensate lag. If we send a smaller timestamp, he will move target frither then he is
		// if we send larger timestamp then it will start moving after X seconds as moving at src client
		// do we need to add lag to destination client or only source client ?
//			move_time = movement_info.time + 4000; - used as experiment : others see me react after 4 seconds delay
		//we need to sincronize the 2 clocks for the 2 clients
//		move_time = getMSTime() + _latency + 100; // let's have a relative value to be able to track lag for different clients
//		move_time = getMSTime() + _latency; // maybe client will handle latency compensation ?
//		memcpy(&movement_packet[pos], recv_data.contents(), recv_data.size());
//		memcpy(movement_packet, recv_data.contents(), recv_data.size());	//3.2 movement packet includes mover guid
//		*(uint32*)&movement_packet[pos+6] = move_time;
//		size_t pos = (size_t)m_MoverWoWGuid.GetNewGuidLen() + 1;
#else
		WorldPacket data( SMSG_PLAYER_MOVE, 55 );
		movement_info.client_ms_time = getMSTime() + _latency;
		movement_info.WriteMovementInfo( data );
#endif
		/************************************************************************/
		/* Distribute to all inrange players.                                   */
		/************************************************************************/
		for(InRangePlayerSetRecProt::iterator itr = _player->m_inRangePlayers.begin(); itr != _player->m_inRangePlayers.end(); ++itr)
		{
			//removing client lag compensation. I think this is handled by client. Or we need to add it at each syncronized packet not just movement
//			*(uint32*)&movement_packet[pos+6] = uint32(move_time + (*itr)->GetSession()->m_clientTimeDelay);
//			*(uint32*)&movement_packet[pos+6] = uint32(move_time - (*itr)->GetSession()->_latency);
			/**/
//printf("our client delay %d, he's client delay %u, our latency %u, he's latency %u\n",m_clientTimeDelay,(*itr)->GetSession()->m_clientTimeDelay,_latency,(*itr)->GetSession()->_latency);
			if( (*itr)->GetSession() )
				(*itr)->GetSession()->SendPacket( &data );			
		}
	}

	/************************************************************************/
	/* Falling damage checks                                                */
	/************************************************************************/

	if( _player->blinked )
	{
		_player->blinked = false;
		_player->m_fallDisabledUntil = UNIXTIME + 5;
		_player->SpeedCheatDelay( 1000 ); //some say they managed to trigger system with knockback. Maybe they moved in air ?
	}
	else
	{
		if( recv_data.GetOpcode() == MSG_MOVE_FALL_LAND )
		{
			// player has finished falling
			//if _player->z_axisposition contains no data then set to current position
			if( !_player->z_axisposition )
				_player->z_axisposition = movement_info.z;

			// calculate distance fallen
			uint32 falldistance = float2int32( _player->z_axisposition - movement_info.z );

			/*Safe Fall*/
			if( (int32)falldistance > _player->m_safeFall )
				falldistance -= _player->m_safeFall;
			else
				falldistance = 1;
			if( _player->m_safe_fall == true )
				falldistance = 1;

			//checks that player has fallen more than 12 units, otherwise no damage will be dealt
			//falltime check is also needed here, otherwise sudden changes in Z axis position, such as using !recall, may result in death			
			if( _player->isAlive() && !_player->bInvincible && falldistance > 12 && ( UNIXTIME >= _player->m_fallDisabledUntil )
				&& _player->SchoolImmunityAntiEnemy[ SCHOOL_NORMAL ] == 0 && _player->SchoolImmunityAntiFriend[ SCHOOL_NORMAL ] == 0 
//				&& movement_info.FallTime > 2000	//seems like this is only sent if we jump off something and not when we break a flight
				)
			{
				// 1.7% damage for each unit fallen on Z axis over 13
				uint32 health_loss = float2int32( float( _player->GetUInt32Value( UNIT_FIELD_MAXHEALTH ) * ( ( falldistance - 12 ) * 0.017 ) ) );
													
				if( health_loss >= _player->GetUInt32Value( UNIT_FIELD_HEALTH ) )
				{
					health_loss = _player->GetUInt32Value( UNIT_FIELD_HEALTH );
					_player->Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_DEATHS_FROM,2,ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
				}
				else
					_player->Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_FALL_WITHOUT_DYING,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,falldistance*1000,ACHIEVEMENT_EVENT_ACTION_SET_MAX);

				_player->SendEnvironmentalDamageLog( _player->GetGUID(), DAMAGE_FALL, health_loss );
				_player->DealDamage( _player, health_loss, true );

				//_player->RemoveStealth(); // ceberwow : why again? lost stealth by AURA_INTERRUPT_ON_ANY_DAMAGE_TAKEN already. 
			}
			_player->z_axisposition = 0.0f;
		}
		else
			//whilst player is not falling, continuosly update Z axis position.
			//once player lands this will be used to determine how far he fell.
//			if( !( movement_info.flags & MOVEFLAG_FALLING ) )
			if( movement_info.FallTime == 0 )
				_player->z_axisposition = movement_info.z;
	}

	/************************************************************************/
	/* Transporter Setup                                                    */
	/************************************************************************/
	if(!_player->m_lockTransportVariables)
	{
		if(_player->m_TransporterGUID && !movement_info.transGuid)
		{
			/* we left the transporter we were on */
			if(_player->m_CurrentTransporter)
			{
				_player->m_CurrentTransporter->RemovePlayer(_player);
				_player->m_CurrentTransporter = NULL;
			}

			_player->m_TransporterGUID = 0;
			_player->SpeedCheatReset();
		}
		else if(movement_info.transGuid)
		{
			if(!_player->m_TransporterGUID)
			{
				/* just walked into a transport */
				if(_player->IsMounted())
					_player->RemoveAura(_player->m_MountSpellId);

				_player->m_CurrentTransporter = objmgr.GetTransporter(GUID_LOPART(movement_info.transGuid));
				if(_player->m_CurrentTransporter)
					_player->m_CurrentTransporter->AddPlayer(_player);

				/* set variables */
				_player->m_TransporterGUID = movement_info.transGuid;
				_player->m_TransporterTime = movement_info.transTime;
				_player->m_TransporterX = movement_info.transX;
				_player->m_TransporterY = movement_info.transY;
				_player->m_TransporterZ = movement_info.transZ;
			}
			else
			{
				/* no changes */
				_player->m_TransporterTime = movement_info.transTime;
				_player->m_TransporterX = movement_info.transX;
				_player->m_TransporterY = movement_info.transY;
				_player->m_TransporterZ = movement_info.transZ;
			}
		}
		/*float x = movement_info.x - movement_info.transX;
		float y = movement_info.y - movement_info.transY;
		float z = movement_info.z - movement_info.transZ;
		Transporter* trans = _player->m_CurrentTransporter;
		if(trans) sChatHandler.SystemMessageToPlr(_player, "Client t pos: %f %f\nServer t pos: %f %f   Diff: %f %f", x,y, trans->GetPositionX(), trans->GetPositionY(), trans->CalcDistance(x,y,z), trans->CalcDistance(movement_info.x, movement_info.y, movement_info.z));*/
	}

	/************************************************************************/
	/* Anti-Speed Hack Checks                                               */
	/************************************************************************/

	

	/************************************************************************/
	/* Breathing System                                                     */
	/************************************************************************/
	_HandleBreathing(movement_info, _player, this, recv_data );

	/************************************************************************/
	/* Remove Spells                                                        */
	/************************************************************************/
	uint32 flags = AURA_INTERRUPT_ON_MOVEMENT;
//	if( !( movement_info.flags & MOVEFLAG_SWIMMING || movement_info.flags & MOVEFLAG_FALLING ) )
	if( recv_data.GetOpcode() == MSG_MOVE_STOP_SWIM 
//		|| recv_data.GetOpcode() == MSG_MOVE_JUMP_STOP_SWIM // this is removing aquatic form. I wonder what else it breaks commenting it
		|| !( _player->m_UnderwaterState & ( UNDERWATERSTATE_UNDERWATER | UNDERWATERSTATE_SWIMMING ) ) )
		flags |= AURA_INTERRUPT_ON_LEAVE_WATER;
//	if( movement_info.flags & MOVEFLAG_SWIMMING )
	if( recv_data.GetOpcode() == MSG_MOVE_START_SWIM || ( _player->m_UnderwaterState & UNDERWATERSTATE_SWIMMING ) )
		flags |= AURA_INTERRUPT_ON_ENTER_WATER;
//	if( movement_info.flags & ( MOVEFLAG_TURN_LEFT | MOVEFLAG_TURN_RIGHT ) )
//		flags |= AURA_INTERRUPT_ON_TURNING;
	_player->RemoveAurasByInterruptFlag( flags );
	//client will not send HandleCancelChannellingOpcode when strafing
	Spell *m_currentSpell = GetPlayer()->m_currentSpell;
	if( //_player->strafing && 
		moved == true 
		&&  m_currentSpell != NULL
		&& GetPlayer()->IsSpellIgnoringMoveInterrupt( m_currentSpell->GetProto()->NameHash ) == false 
		&& ( m_currentSpell->GetProto()->c_is_flags2 & SPELL_FLAG2_IS_IGNORING_MOVE_INTERUPT ) == 0 
		&& ( ( m_currentSpell->GetProto()->ChannelInterruptFlags != 0 ) || ( m_currentSpell->GetProto()->AuraInterruptFlags & AURA_INTERRUPT_ON_MOVEMENT ) != 0 || ( m_currentSpell->GetProto()->InterruptFlags & CAST_INTERRUPT_ON_MOVEMENT ) )
		)
	{
//		if( GetPlayer() )
//			GetPlayer()->ClearCooldownForSpell( GetPlayer()->m_currentSpell->GetProto()->Id );
		GetPlayer()->m_currentSpell->safe_cancel();
	}

	/************************************************************************/
	/* Update our position in the server.                                   */
	/************************************************************************/
	if( _player->m_CurrentCharm && _player->GetMapMgr() )
	{
		Unit *cc = _player->GetMapMgr()->GetUnit( _player->m_CurrentCharm );
		if( cc )
			cc->SetPosition(movement_info.x, movement_info.y, movement_info.z, movement_info.orientation);
	}
	else
	{
		if(!_player->m_CurrentTransporter) 
		{
			//felt underground ? Kill player ? Why is it he's fault ?
			if( !_player->SetPosition(movement_info.x, movement_info.y, movement_info.z, movement_info.orientation) )
			{
//				_player->SetUInt32Value(UNIT_FIELD_HEALTH, 0);
//				_player->KillPlayer();
			}
		}
		else
		{
			_player->SetPosition(movement_info.x, movement_info.y, movement_info.z, 
				movement_info.orientation + movement_info.transO, false);
		}
	}	
}

void WorldSession::HandleMoveTimeSkippedOpcode( WorldPacket & recv_data )
{
	
}

void WorldSession::HandleMoveNotActiveMoverOpcode( WorldPacket & recv_data )
{

}


void WorldSession::HandleSetActiveMoverOpcode( WorldPacket & recv_data )
{
	// set current movement object
	uint64 guid;
	recv_data >> guid;

	if(guid != m_MoverWoWGuid.GetOldGuid())
	{
		// make sure the guid is valid and we aren't cheating
		if( !(_player->m_CurrentCharm == guid) &&
			!(_player->GetGUID() == guid) )
		{
			// cheater!
			return;
		}

		// generate wowguid
		if(guid != 0)
			m_MoverWoWGuid.Init(guid);
		else
			m_MoverWoWGuid.Init(_player->GetGUID());

		// set up to the movement packet
//		movement_packet[0] = m_MoverWoWGuid.GetNewGuidMask();
//		memcpy(&movement_packet[1], m_MoverWoWGuid.GetNewGuid(), m_MoverWoWGuid.GetNewGuidLen());
	}
}

void WorldSession::HandleMoveSplineCompleteOpcode(WorldPacket &recvPacket)
{

}

void WorldSession::HandleMountSpecialAnimOpcode(WorldPacket &recvdata)
{
	WorldPacket data(SMSG_MOUNTSPECIAL_ANIM,8);
	data << _player->GetGUID();
	_player->SendMessageToSet(&data, true);
}

void WorldSession::HandleWorldportOpcode(WorldPacket & recv_data)
{
	uint32 unk;
	uint32 mapid;
	float x,y,z,o;
	recv_data >> unk >> mapid >> x >> y >> z >> o;

	//printf("\nTEST: %u %f %f %f %f", mapid, x, y, z, o);
	
	if(!_player->IsInWorld())
	{ 
		return;
	}

	if(!HasGMPermissions())
	{
		SendNotification("You do not have permission to use this function.");
		return;
	}

	LocationVector vec(x,y,z,o);
	_player->SafeTeleport(mapid,0,vec);
}

void WorldSession::HandleTeleportToUnitOpcode(WorldPacket & recv_data)
{
	uint8 unk;
	Unit * target;
	recv_data >> unk;

	if(!_player->IsInWorld())
	{ 
		return;
	}

	if(!HasGMPermissions())
	{
		SendNotification("You do not have permission to use this function.");
		return;
	}

	if( (target = _player->GetMapMgr()->GetUnit(_player->GetSelection())) == NULL )
	{ 
		return;
	}

	_player->SafeTeleport(_player->GetMapId(), _player->GetInstanceID(), target->GetPosition());
}

void WorldSession::HandleTeleportCheatOpcode(WorldPacket & recv_data)
{
	float x,y,z,o;
	LocationVector vec;

	if(!HasGMPermissions())
	{
		SendNotification("You do not have permission to use this function.");
		return;
	}

	recv_data >> x >> y >> z >> o;
	vec.ChangeCoords(x,y,z,o);
	_player->SafeTeleport(_player->GetMapId(),_player->GetInstanceID(),vec);
}

#ifndef JayZMoveHandler
#ifdef _DEBUG
	#define DumpUnhandledMovePacket( x ) DumpUnhandledMovePacket_(x)
#else
	#define DumpUnhandledMovePacket( x ) ;
#endif

#define INSERT_FALL_READ_CODE 				if( flag & 0x01 ) \
				{ \
					data >> jump_unk_neg_small; \
					data >> FallTime; \
					data >> jump_xySpeed; \
					data >> jump_sinAngle; \
					data >> jump_cosAngle; \
				} \

void MovementInfo::init(WorldPacket & data, WoWGuid &mover_guid)
{
	FallTime = 0;
	transGuid = 0;
	//in 14333 client there are multiple types of packets based on the opcode
	//they are very similar but obfuscated guid insertion might differ

	//or maybe even flag can be skipped. Not sure based on what is present the jump packet :(
	//should convert this into a 7 byte long struct or something
	data.read_skip(3);	
	data >> flag;

	switch( data.GetOpcode() )
	{
		case MSG_MOVE_SET_FACING:
			{
				data >> x;
				data >> y;
				data >> z;
				data >> orientation;
				data >> client_ms_time;

				if( mover_guid.GetNewGuid()[3] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[2] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[1] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[0] )
					data.read_skip(1);
			}break;
		case MSG_MOVE_JUMP:
			{
				data >> orientation;
				data >> x;
				data >> y;
				data >> z;
				data >> client_ms_time;

				if( mover_guid.GetNewGuid()[1] )
					data.read_skip(1);

				//read fall segment
				INSERT_FALL_READ_CODE

				if( mover_guid.GetNewGuid()[0] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[3] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[2] )
					data.read_skip(1);
			}break;
		case MSG_MOVE_FALL_LAND:
			{
				data >> client_ms_time;
				data >> x;
				data >> y;
				data >> z;
				data >> orientation;

				if( mover_guid.GetNewGuid()[2] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[3] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[0] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[1] )
					data.read_skip(1);
			}break;
		case MSG_MOVE_HEARTBEAT:
			{
				data >> orientation;
				data >> x;
				data >> y;
				data >> z;
				data >> client_ms_time;
				
				//read fall segment
				INSERT_FALL_READ_CODE

				if( mover_guid.GetNewGuid()[1] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[0] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[2] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[3] )
					data.read_skip(1);
			}break;
		case MSG_MOVE_STOP:
			{
				data >> client_ms_time;
				data >> x;
				data >> y;
				data >> z;
				data >> orientation;

				if( mover_guid.GetNewGuid()[1] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[3] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[2] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[0] )
					data.read_skip(1);
			}break;
		case MSG_MOVE_START_STRAFE_RIGHT:
			{
				data >> client_ms_time;
				data >> orientation;
				data >> x;
				data >> y;
				data >> z;

				if( mover_guid.GetNewGuid()[0] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[2] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[3] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[1] )
					data.read_skip(1);

				//read fall segment
				INSERT_FALL_READ_CODE
			}break;
		case MSG_MOVE_START_STRAFE_LEFT:
			{
				data >> orientation;
				data >> client_ms_time;
				data >> x;
				data >> y;
				data >> z;

				if( mover_guid.GetNewGuid()[3] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[1] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[2] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[0] )
					data.read_skip(1);
			}break;
		case MSG_MOVE_STOP_STRAFE:
			{
				data >> x;
				data >> y;
				data >> z;
				data >> client_ms_time;
				data >> orientation;
				if( mover_guid.GetNewGuid()[2] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[0] )
					data.read_skip(1);

				//read fall segment
				INSERT_FALL_READ_CODE

				if( mover_guid.GetNewGuid()[1] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[3] )
					data.read_skip(1);

			}break;
		case MSG_MOVE_START_FORWARD:
			{
				data >> client_ms_time;
				data >> x;
				data >> y;
				data >> z;
				data >> orientation;

				if( mover_guid.GetNewGuid()[1] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[2] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[3] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[0] )
					data.read_skip(1);
			}break;
		case MSG_MOVE_START_BACKWARD:
			{
				data >> x;
				data >> y;
				data >> z;
				data >> client_ms_time;
				data >> orientation;

				if( mover_guid.GetNewGuid()[0] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[2] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[3] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[1] )
					data.read_skip(1);
			}break;
		case MSG_MOVE_START_TURN_LEFT:
			{
				data >> orientation;
				data >> client_ms_time;
				data >> x;
				data >> y;
				data >> z;

				if( mover_guid.GetNewGuid()[2] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[0] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[1] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[3] )
					data.read_skip(1);
			}break;
		case MSG_MOVE_START_TURN_RIGHT:
			{
				data >> orientation;
				data >> client_ms_time;
				data >> x;
				data >> y;
				data >> z;

				if( mover_guid.GetNewGuid()[1] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[0] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[2] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[3] )
					data.read_skip(1);
			}break;
		case MSG_MOVE_STOP_TURN:
			{
				data >> orientation;
				data >> client_ms_time;
				data >> x;
				data >> y;
				data >> z;

				if( mover_guid.GetNewGuid()[2] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[0] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[1] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[3] )
					data.read_skip(1);
			}break;
		default:
			{
			}break;
	};

	//it would be normal to have data for these. If not then dump the packet to analize it later
	if( data.rpos() != data.wpos() )
		DumpUnhandledMovePacket( data );
	if( flag & 1 )
	{
		if( jump_unk_neg_small < 20 || jump_unk_neg_small > 20 )
			DumpUnhandledMovePacket( data );
		if( jump_xySpeed < 20 || jump_xySpeed > 20 )
			DumpUnhandledMovePacket( data );
		if( jump_sinAngle < -1 || jump_unk_neg_small > 1 )
			DumpUnhandledMovePacket( data );
		if( jump_cosAngle < -1 || jump_cosAngle > 1 )
			DumpUnhandledMovePacket( data );
		if( FallTime < 0 || FallTime > 50000 )
			DumpUnhandledMovePacket( data );
	}
#if 0
	switch( data.GetOpcode() )
	{
		case MSG_MOVE_FALL_LAND:
			{
				/*
{CLIENT} Packet: (0x88E1) MSG_MOVE_FALL_LAND PacketSize = 31 stamp = 17402457
1A 00 18 00 00 00
00 
55 8B 09 01 
14 36 D7 C4
A4 38 A8 45 
19 FF BE 3F 
96 B2 62 40 
02 g[3]
03 g[2]
04 g[1]
05 g[0]
				*/
//				uint16	guid_masks[3];
//				uint8	*guid_bytes = (uint8*)&mover_guid;
				
//				data >> guid_masks[0];
//				data >> guid_masks[1];
//				data >> guid_masks[2];
data.read_skip(6);	//or maybe even flag can be skipped. Not sure based on what is present the jump packet :(
				data >> flag;
				data >> client_ms_time;
				data >> x;
				data >> y;
				data >> z;
				data >> orientation;

//				if( guid_masks[0] & 0x0008 )
//					data >> guid_bytes[2];
//				if( guid_masks[1] & 0x0010 )
//					data >> guid_bytes[3];
//				if( guid_masks[0] & 0x0002 )
//					data >> guid_bytes[0];	//02 00 08 00 00 00 - 2 bits and 1 guid
//				if( guid_masks[0] & 0x0010 )
//					data >> guid_bytes[1];

				if( mover_guid.GetNewGuid()[2] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[3] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[0] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[1] )
					data.read_skip(1);
			}break;
		case MSG_MOVE_HEARTBEAT:
			{
				/*
{CLIENT} Packet: (0x28E3) MSG_MOVE_HEARTBEAT PacketSize = 51 stamp = 17402457
A0 01 00 00 08 00 
83 
96 B2 62 40 orientation
14 36 D7 C4 x
A4 38 A8 45 y
19 FF BE 3F z
55 8B 09 01 time
00 00 00 00 
00 00 00 00 
00 00 00 00 
6B BC 6B BF -0.92084378004074 
1F A5 C7 BE -0.38993164896965 
05 g[1] 4
04 g[0]	5
02 g[2]	3
03 g[3]	2                                       
				*/
//				uint16	guid_masks[3];
//				uint8	*guid_bytes = (uint8*)&mover_guid;
				
//				data >> guid_masks[0];
//				data >> guid_masks[1];
//				data >> guid_masks[2];
data.read_skip(6);	//or maybe even flag can be skipped. Not sure based on what is present the jump packet :(
				data >> flag;
				data >> orientation;
				data >> x;
				data >> y;
				data >> z;
				data >> client_ms_time;
				
				//read fall segment
				INSERT_FALL_READ_CODE

//				if( guid_masks[0] & 0x0080 )
//					data >> guid_bytes[1];
//				if( guid_masks[2] & 0x0008 )
//					data >> guid_bytes[0];	
//				if( guid_masks[0] & 0x0020 )
//					data >> guid_bytes[2];	
//				if( guid_masks[0] & 0x0100 )
//					data >> guid_bytes[3];

				if( mover_guid.GetNewGuid()[1] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[0] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[2] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[3] )
					data.read_skip(1);
			}break;
		case MSG_MOVE_START_BACKWARD:
			{
				/*
{CLIENT} Packet: (0x0863) MSG_MOVE_START_BACKWARD PacketSize = 31 stamp = 17422051
50 01 60 00 00 00 
80 
9C 46 D7 C4 
8D 34 A8 45 
5A 8D C0 3F 
2F D8 09 01 
B7 ED 89 40 
04 g[0]	5 
02 g[2]	3 
03 g[3]	2        
05 g[1] 4                                  
				*/
//				uint16	guid_masks[3];
//				uint8	*guid_bytes = (uint8*)&mover_guid;
				
//				data >> guid_masks[0];
//				data >> guid_masks[1];
//				data >> guid_masks[2];
data.read_skip(6);	//or maybe even flag can be skipped. Not sure based on what is present the jump packet :(
				data >> flag;
				data >> x;
				data >> y;
				data >> z;
				data >> client_ms_time;
				data >> orientation;

//				if( guid_masks[0] & 0x0040 )
//					data >> guid_bytes[0];
//				if( guid_masks[1] & 0x0020 )
//					data >> guid_bytes[2];	
//				if( guid_masks[0] & 0x0100 )
//					data >> guid_bytes[3];	
//				if( guid_masks[0] & 0x0040 )
//					data >> guid_bytes[1];

				if( mover_guid.GetNewGuid()[0] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[2] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[3] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[1] )
					data.read_skip(1);
			}break;
		case MSG_MOVE_STOP:
			{
				/*
{CLIENT} Packet: (0x286A) MSG_MOVE_STOP PacketSize = 31 stamp = 17422129
40 00 00 00 39 00 
00 
93 D8 09 01 
F9 40 D7 C4 
DD 37 A8 45 
1C BA BD 3F 
B7 ED 89 40 
05 g[1] 4 
03 g[3]	2   
02 g[2]	3  
04 g[0]	5                                  
				*/
//				uint16	guid_masks[3];
//				uint8	*guid_bytes = (uint8*)&mover_guid;
				
//				data >> guid_masks[0];
//				data >> guid_masks[1];
//				data >> guid_masks[2];
data.read_skip(6);	//or maybe even flag can be skipped. Not sure based on what is present the jump packet :(
				data >> flag;
				data >> client_ms_time;
				data >> x;
				data >> y;
				data >> z;
				data >> orientation;

//				if( guid_masks[2] & 0x0010 )
//					data >> guid_bytes[1];
//				if( guid_masks[0] & 0x0040 )
//					data >> guid_bytes[3];	
//				if( guid_masks[2] & 0x00008 )
//					data >> guid_bytes[2];	
//				if( guid_masks[2] & 0x0020 )
//					data >> guid_bytes[0];

				if( mover_guid.GetNewGuid()[1] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[3] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[2] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[0] )
					data.read_skip(1);
			}break;
		case MSG_MOVE_START_STRAFE_RIGHT:
			{
				/*
{CLIENT} Packet: (0xB843) MSG_MOVE_START_STRAFE_RIGHT PacketSize = 31 stamp = 17444499
80 00 00 11 80 09 
00 
DA 2F 0A 01 t
5F 3D 9A 40 o
7A 3D D7 C4 x
E3 2A A8 45 y
E9 C4 C1 3F z
05 g[1] 4 
03 g[3]	2 
02 g[2]	3 
04 g[0]	5                                  
				*/
//				uint16	guid_masks[3];
//				uint8	*guid_bytes = (uint8*)&mover_guid;
				
//				data >> guid_masks[0];
//				data >> guid_masks[1];
//				data >> guid_masks[2];
data.read_skip(6);	//or maybe even flag can be skipped. Not sure based on what is present the jump packet :(
				data >> flag;
				data >> client_ms_time;
				data >> orientation;
				data >> x;
				data >> y;
				data >> z;

//				if( guid_masks[2] & 0x0100 )
//					data >> guid_bytes[1];
//				if( guid_masks[1] & 0x0100 )
//					data >> guid_bytes[3];	
//				if( guid_masks[0] & 0x0080 )
//					data >> guid_bytes[2];	
//				if( guid_masks[2] & 0x0800 )
//					data >> guid_bytes[0];

				if( mover_guid.GetNewGuid()[0] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[2] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[3] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[1] )
					data.read_skip(1);

				//read fall segment
				INSERT_FALL_READ_CODE
			}break;
		case MSG_MOVE_STOP_STRAFE:
			{
				/*
{CLIENT} Packet: (0xA8E0) MSG_MOVE_STOP_STRAFE PacketSize = 31 stamp = 17444624
80 00 00 00 60 00
60 
E2 63 D7 C4 x
D9 29 A8 45 y
C3 CE B3 3F z
5F 3D 9A 40 o
6F 30 0A 01 t
02 g[2]	3
04 g[0]	5
05 g[1] 4
03 g[3]	2                                  
				*/
//				uint16	guid_masks[3];
//				uint8	*guid_bytes = (uint8*)&mover_guid;
				
//				data >> guid_masks[0];
//				data >> guid_masks[1];
//				data >> guid_masks[2];
data.read_skip(6);	//or maybe even flag can be skipped. Not sure based on what is present the jump packet :(
				data >> flag;
				data >> client_ms_time;
				data >> orientation;
				data >> x;
				data >> y;
				data >> z;

//				if( guid_masks[2] & 0x0000 )
//					data >> guid_bytes[2];//????
//				if( guid_masks[2] & 0x0020 )
//					data >> guid_bytes[0];

				if( mover_guid.GetNewGuid()[2] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[0] )
					data.read_skip(1);

				//read fall segment
				INSERT_FALL_READ_CODE

//				if( guid_masks[0] & 0x0000 )
//					data >> guid_bytes[1];	//????	
//				if( guid_masks[0] & 0x0080 )
//					data >> guid_bytes[3];
				if( mover_guid.GetNewGuid()[1] )
					data.read_skip(1);
				if( mover_guid.GetNewGuid()[3] )
					data.read_skip(1);

			}break;
		default:
			{
			}break;
	};

	//it would be normal to have data for these. If not then dump the packet to analize it later
	if( data.rpos() != data.wpos() )
		DumpUnhandledMovePacket( data );
	if( flag & 1 )
	{
		if( jump_unk_neg_small < 20 || jump_unk_neg_small > 20 )
			DumpUnhandledMovePacket( data );
		if( jump_xySpeed < 20 || jump_xySpeed > 20 )
			DumpUnhandledMovePacket( data );
		if( jump_sinAngle < -1 || jump_unk_neg_small > 1 )
			DumpUnhandledMovePacket( data );
		if( jump_cosAngle < -1 || jump_cosAngle > 1 )
			DumpUnhandledMovePacket( data );
		if( FallTime < 0 || FallTime > 50000 )
			DumpUnhandledMovePacket( data );
	}
//	mover_guid_obfuscated = mover_guid;
//	GUID_un_obfuscate( mover_guid );
#endif
/*
{CLIENT} Packet: (0x9848) MSG_MOVE_SET_FACING PacketSize = 33 TimeStamp = 3371296
F8 00 00 00 00 00 A0 48 4A 0B C6 B4 E8 05 C3 5B 13 A1 42 BA B0 A8 40 03 73 33 00 A9 02 00 C8 81 25 
{CLIENT} Packet: (0x28E3) MSG_MOVE_HEARTBEAT PacketSize = 33 TimeStamp = 3397984
B0 01 00 00 00 01 E0 DC A5 A0 40 DC DB 0A C6 D8 E8 3F C2 07 62 AD 42 37 DB 33 00 81 C8 00 25 A9 02 
{CLIENT} Packet: (0x18E1) MSG_MOVE_START_FORWARD PacketSize = 33 TimeStamp = 3436781
97 00 00 00 00 00 E0 CB 72 34 00 8B B5 0A C6 18 CA 8F C2 1B 1A A7 42 13 09 A3 40 00 C8 A9 02 25 81 
{CLIENT} Packet: (0x286A) MSG_MOVE_STOP PacketSize = 33 TimeStamp = 3401656
40 00 00 00 7D 00 00 
98 E9 33 00 NOT time, maybe packet serializer ?
8B B5 0A C6 -8877.3857421875 
18 CA 8F C2 -71.894714355469 
1B 1A A7 42 83.550987243652 
13 09 A3 40 5.0948576927185 
81 C8 02 A9 25 00 - obfuscated compressed guid, seems to depend on packet type how it is obfuscated

//this is mover guid from other packet
CF - mask
24 C9 A8 03 80 01 - guid compressed
24 C9 A8 03 00 00 80 01 - full guid
81 = g[6]-1
C8 = g[2]-1
02 
A9 25 00

{CLIENT} Packet: (0x9848) MSG_MOVE_SET_FACING PacketSize = 33 TimeStamp = 3371421
F8 00 00 00 00 00 A0 
48 4A 0B C6 B4 E8 05 C3 5B 13 A1 42 A0 CE BE 40 7B 73 33 00 A9 02 00 C8 81 25 
{CLIENT} Packet: (0x9848) MSG_MOVE_SET_FACING PacketSize = 33 TimeStamp = 3373406
F8 00 00 00 00 01 A0 
20 49 0B C6 not time !
21 AD 05 C3 E1 17 A1 42 24 78 4B 3F 3A 7B 33 00 A9 02 00 C8 81 25 

*/
	/*
//	uint64 transGuid = 0;
//	uint32 unk13 = 0;
	float pitch;
	uint32 unklast;
	float jump_sinAngle;
	float jump_cosAngle;
	float jump_xySpeed;
	uint32 spline_unk;
	uint16 movementflags;
	uint32 unk420_3;
	uint8 obfuscation_code_selector;
//	uint8 obfuscated_mover_guid[8];
//	uint64 *assembled_old_guid = &obfuscated_mover_guid[0];

	transGuid = 0;
//	unk13 = 0;

	//reset falltime in case we do not read the value from packet !
	FallTime = 0;

	//!! first 13 bytes are wtf ?
	// last N bytes are obfuscated GUID
	// maybe first 4 bytes are client latency ?
//	data >> ControlerGUID;	//mover guid ? 0 if same as ourself ?
	data >> obfuscation_code_selector;	//based on this we need to reorder guid bytes, number of bits here represent the number of butes we need to read later
	data >> flags;
	data >> movementflags;	//might need to completly remove dependency on this. Seems like 420 client completly remade flags
	data >> unk420_3;
	data >> x >> y >> z >> orientation;

	if (flags & MOVEFLAG_TAXI)
		data >> transGuid >> transX >> transY >> transZ >> transO >> transTime >> transSeat;

	if (flags & (MOVEFLAG_SWIMMING | MOVEFLAG_AIR_SWIMMING) || movementflags & 0x20)
		data >> pitch;

	data >> unklast;

	if (flags & MOVEFLAG_FALLING || flags & MOVEFLAG_REDIRECTED)
		data >> FallTime >> jump_sinAngle >> jump_cosAngle >> jump_xySpeed;

	if (flags & MOVEFLAG_SPLINE_MOVER)
		data >> spline_unk;
 */
/*	if( data.rpos() != data.wpos() )
			sLog.outDebug("Extra bits of movement packet left");
			*/
/*
{SERVER} Packet: (0x00EE) MSG_MOVE_HEARTBEAT PacketSize = 41 TimeStamp = 3167171
CF 8C 61 0C 02 80
01 
01 00 20 00
00 00 
78 51 3F D9 time
64 E6 0C C6 -x
72 4E 02 44 -y
67 50 8B 42 -z
2C C8 5B 40 -o
00 00 00 80 
A5 03 00 00 
{SERVER} Packet: (0x00DA) MSG_MOVE_SET_FACING PacketSize = 37 TimeStamp = 3167171
CF 54 9A 0F 01 80 
01 
04 00 00 00
00 00
5E 4C 3F D9
99 89 0B C6
41 75 1B 44 
C0 0B C7 42 
0A DD 78 40 
F7 00 00 00 
{CLIENT} Packet: (0x00B5) MSG_MOVE_START_FORWARD PacketSize = 30 TimeStamp = 3258093
09 00 00 00
00 00 
A8 B6 31 00
B4 D6 0B C6
3E 7C 06 C3
A2 33 A7 42
2A EB AD 40
00 00 00 00 
{SERVER} Packet: (0x00B5) MSG_MOVE_START_FORWARD PacketSize = 37 TimeStamp = 3267000
CF 64 B6 0F 02 80 01 
11 00 00 00 
00 00 
81 D1 40 D9 E2 91 0A C6 F6 79 20 C3 F4 3B A0 42 66 2D 76 3F B8 02 00 00 

	*/
}

/*
void MovementInfo::write(WorldPacket & data)
{
	data << flags << movementflags << getMSTime();

	data << x << y << z << orientation;

	if (flags & MOVEFLAG_TAXI)
		data << transGuid << transX << transY << transZ << transO << transTime << transSeat;

	if (flags & (MOVEFLAG_SWIMMING | MOVEFLAG_AIR_SWIMMING) || flag16 & 0x20)
		data << pitch;

	data << unklast;

	if (flags & MOVEFLAG_FALLING || flags & MOVEFLAG_REDIRECTED)
		data << FallTime << jump_sinAngle << jump_cosAngle << jump_xySpeed;

	if (flags & MOVEFLAG_SPLINE_MOVER)
		data << spline_unk;
	
	if(unk13)
		data << unk13;
}*/
#endif //NOT JayZMoveHandler

#ifdef JayZMoveHandler

enum MovementFlags_
{
    MOVEMENTFLAG_NONE                  = 0x00000000,
    MOVEMENTFLAG_FORWARD               = 0x00000001,
    MOVEMENTFLAG_BACKWARD              = 0x00000002,
    MOVEMENTFLAG_STRAFE_LEFT           = 0x00000004,
    MOVEMENTFLAG_STRAFE_RIGHT          = 0x00000008,
    MOVEMENTFLAG_LEFT                  = 0x00000010,
    MOVEMENTFLAG_RIGHT                 = 0x00000020,
    MOVEMENTFLAG_PITCH_UP              = 0x00000040,
    MOVEMENTFLAG_PITCH_DOWN            = 0x00000080,
    MOVEMENTFLAG_WALKING               = 0x00000100,               // Walking
    MOVEMENTFLAG_ONTRANSPORT           = 0x00000200,               // Used for flying on some creatures
    MOVEMENTFLAG_LEVITATING            = 0x00000400,
    MOVEMENTFLAG_ROOT                  = 0x00000800,
    MOVEMENTFLAG_JUMPING               = 0x00001000,
    MOVEMENTFLAG_FALLING               = 0x00002000,               // damage dealt on that type of falling
    MOVEMENTFLAG_PENDING_STOP          = 0x00004000,
    MOVEMENTFLAG_PENDING_STRAFE_STOP   = 0x00008000,
    MOVEMENTFLAG_PENDING_FORWARD       = 0x00010000,
    MOVEMENTFLAG_PENDING_BACKWARD      = 0x00020000,
    MOVEMENTFLAG_PENDING_STRAFE_LEFT   = 0x00040000,
    MOVEMENTFLAG_PENDING_STRAFE_RIGHT  = 0x00080000,
    MOVEMENTFLAG_PENDING_ROOT          = 0x00100000,
    MOVEMENTFLAG_SWIMMING              = 0x00200000,               // appears with fly flag also
    MOVEMENTFLAG_ASCENDING             = 0x00400000,               // press "space" when flying
    MOVEMENTFLAG_DESCENDING            = 0x00800000,
    MOVEMENTFLAG_CAN_FLY               = 0x01000000,               // can fly
    MOVEMENTFLAG_FLYING                = 0x02000000,               // hover
    MOVEMENTFLAG_SPLINE_ELEVATION      = 0x04000000,               // used for flight paths
    MOVEMENTFLAG_SPLINE_ENABLED        = 0x08000000,               // used for flight paths
    MOVEMENTFLAG_WATERWALKING          = 0x10000000,               // prevent unit from falling through water
    MOVEMENTFLAG_FALLING_SLOW          = 0x20000000,               // active rogue safe fall spell (passive)
    MOVEMENTFLAG_HOVER                 = 0x40000000,               // hover, cannot jump

    MOVEMENTFLAG_MOVING         =
        MOVEMENTFLAG_FORWARD |MOVEMENTFLAG_BACKWARD  |MOVEMENTFLAG_STRAFE_LEFT|MOVEMENTFLAG_STRAFE_RIGHT|
        MOVEMENTFLAG_PITCH_UP|MOVEMENTFLAG_PITCH_DOWN|MOVEMENTFLAG_JUMPING
        |MOVEMENTFLAG_FALLING|MOVEMENTFLAG_ASCENDING| MOVEMENTFLAG_SPLINE_ELEVATION,
    MOVEMENTFLAG_TURNING        =
        MOVEMENTFLAG_LEFT | MOVEMENTFLAG_RIGHT,
};
enum MovementFlags2
{
    MOVEMENTFLAG2_NONE                     = 0x00000000,
    MOVEMENTFLAG2_NO_STRAFE                = 0x00000001,
    MOVEMENTFLAG2_NO_JUMPING               = 0x00000002,
    MOVEMENTFLAG2_UNK3                     = 0x00000004,        // Overrides various clientside checks
    MOVEMENTFLAG2_FULL_SPEED_TURNING       = 0x00000008,
    MOVEMENTFLAG2_FULL_SPEED_PITCHING      = 0x00000010,
    MOVEMENTFLAG2_ALWAYS_ALLOW_PITCHING    = 0x00000020,
    MOVEMENTFLAG2_UNK7                     = 0x00000040,
    MOVEMENTFLAG2_UNK8                     = 0x00000080,
    MOVEMENTFLAG2_UNK9                     = 0x00000100,
    MOVEMENTFLAG2_UNK10                    = 0x00000200,
    MOVEMENTFLAG2_INTERPOLATED_MOVEMENT    = 0x00000400,
    MOVEMENTFLAG2_INTERPOLATED_TURNING     = 0x00000800,
    MOVEMENTFLAG2_INTERPOLATED_PITCHING    = 0x00001000,
    MOVEMENTFLAG2_UNK14                    = 0x00002000,
    MOVEMENTFLAG2_UNK15                    = 0x00004000,
    MOVEMENTFLAG2_UNK16                    = 0x00008000,

    // player only?
    MOVEMENTFLAG2_INTERPOLATED =
        MOVEMENTFLAG2_INTERPOLATED_MOVEMENT |
        MOVEMENTFLAG2_INTERPOLATED_TURNING |
        MOVEMENTFLAG2_INTERPOLATED_PITCHING
};

enum MovementStatusElements
{
    MSEFlags,
    MSEFlags2,
    MSETimestamp,
    MSEHavePitch,
    MSEGuidByte0,
    MSEGuidByte1,
    MSEGuidByte2,
    MSEGuidByte3,
    MSEGuidByte4,
    MSEGuidByte5,
    MSEGuidByte6,
    MSEGuidByte7,
    MSEHaveFallData,
    MSEHaveFallDirection,
    MSEHaveTransportData,
    MSETransportHaveTime2,
    MSETransportHaveTime3,
    MSETransportGuidByte0,
    MSETransportGuidByte1,
    MSETransportGuidByte2,
    MSETransportGuidByte3,
    MSETransportGuidByte4,
    MSETransportGuidByte5,
    MSETransportGuidByte6,
    MSETransportGuidByte7,
    MSEHaveSpline,
    MSEHaveSplineElev,
    MSEPositionX,
    MSEPositionY,
    MSEPositionZ,
    MSEPositionO,
    MSEGuidByte0_2,
    MSEGuidByte1_2,
    MSEGuidByte2_2,
    MSEGuidByte3_2,
    MSEGuidByte4_2,
    MSEGuidByte5_2,
    MSEGuidByte6_2,
    MSEGuidByte7_2,
    MSEPitch,
    MSEFallTime,
    MSETransportGuidByte0_2,
    MSETransportGuidByte1_2,
    MSETransportGuidByte2_2,
    MSETransportGuidByte3_2,
    MSETransportGuidByte4_2,
    MSETransportGuidByte5_2,
    MSETransportGuidByte6_2,
    MSETransportGuidByte7_2,
    MSESplineElev,
    MSEFallHorizontalSpeed,
    MSEFallVerticalSpeed,
    MSEFallCosAngle,
    MSEFallSinAngle,
    MSETransportSeat,
    MSETransportPositionO,
    MSETransportPositionX,
    MSETransportPositionY,
    MSETransportPositionZ,
    MSETransportTime,
    MSETransportTime2,
    MSETransportTime3,
    MSE_COUNT
};

MovementStatusElements PlayerMoveSequence[] = {
MSEHavePitch,
MSEGuidByte6,
MSEHaveFallData,
MSEHaveFallDirection,
MSEGuidByte1,
MSEGuidByte2,
MSEFlags2,
MSEGuidByte0,
MSEHaveTransportData,
MSETransportGuidByte1,
MSETransportGuidByte4,
MSETransportGuidByte0,
MSETransportHaveTime2,
MSETransportGuidByte6,
MSETransportGuidByte3,
MSETransportGuidByte2,
MSETransportGuidByte7,
MSETransportHaveTime3,
MSETransportGuidByte5,
MSEGuidByte3,
MSEGuidByte4,
MSEGuidByte5,
MSEHaveSpline,
MSEGuidByte7,
MSEHaveSplineElev,
MSEFlags,
MSEPositionO,
MSEGuidByte0_2,
MSEPitch,
MSEGuidByte4_2,
MSEFallTime,
MSEFallHorizontalSpeed,
MSEFallCosAngle,
MSEFallSinAngle,
MSEFallVerticalSpeed,
MSETimestamp,
MSEGuidByte1_2,
MSETransportGuidByte7_2,
MSETransportGuidByte3_2,
MSETransportGuidByte1_2,
MSETransportSeat,
MSETransportTime2,
MSETransportGuidByte0_2,
MSETransportGuidByte6_2,
MSETransportPositionX,
MSETransportPositionY,
MSETransportPositionZ,
MSETransportGuidByte4_2,
MSETransportPositionO,
MSETransportTime3,
MSETransportGuidByte5_2,
MSETransportTime,
MSETransportGuidByte2_2,
MSEGuidByte2_2,
MSEPositionX,
MSEPositionY,
MSEPositionZ,
MSEGuidByte6_2,
MSESplineElev,
MSEGuidByte5_2,
MSEGuidByte3_2,
MSEGuidByte7_2
};

MovementStatusElements MovementFallLandSequence[] = {
MSEGuidByte5,
MSEGuidByte6,
MSEGuidByte4,
MSEGuidByte1,
MSEGuidByte2,
MSEHaveSpline,
MSEFlags2,
MSEGuidByte7,
MSEGuidByte3,
MSEGuidByte0,
MSEFlags,
MSEHaveFallData,
MSEHaveFallDirection,
MSEHaveTransportData,
MSETransportGuidByte6,
MSETransportGuidByte3,
MSETransportGuidByte7,
MSETransportGuidByte4,
MSETransportGuidByte1,
MSETransportGuidByte0,
MSETransportGuidByte2,
MSETransportGuidByte5,
MSETransportHaveTime3,
MSETransportHaveTime2,
MSEHavePitch,
MSEHaveSplineElev,
MSETimestamp,
MSEPositionX,
MSEPositionY,
MSEPositionZ,
MSEPositionO,
MSEGuidByte7_2,
MSEGuidByte2_2,
MSEGuidByte3_2,
MSEGuidByte0_2,
MSEGuidByte1_2,
MSEGuidByte5_2,
MSEFallVerticalSpeed,
MSEFallTime,
MSEFallHorizontalSpeed,
MSEFallCosAngle,
MSEFallSinAngle,
MSETransportTime,
MSETransportPositionX,
MSETransportPositionY,
MSETransportPositionZ,
MSETransportPositionO,
MSETransportSeat,
MSETransportGuidByte3_2,
MSETransportGuidByte1_2,
MSETransportTime3,
MSETransportGuidByte6_2,
MSETransportGuidByte0_2,
MSETransportGuidByte5_2,
MSETransportTime2,
MSETransportGuidByte7_2,
MSETransportGuidByte4_2,
MSETransportGuidByte2_2,
MSEPitch,
MSESplineElev,
MSEGuidByte6_2,
MSEGuidByte4_2,
};

MovementStatusElements MovementHeartBeatSequence[] = {
MSEGuidByte1,
MSEGuidByte5,
MSEGuidByte2,
MSEFlags2,
MSEGuidByte3,
MSEGuidByte4,
MSEHaveSpline,
MSEFlags,
MSEGuidByte0,
MSEGuidByte6,
MSEGuidByte7,
MSEHaveTransportData,
MSETransportGuidByte6,
MSETransportGuidByte3,
MSETransportGuidByte7,
MSETransportGuidByte4,
MSETransportGuidByte1,
MSETransportGuidByte0,
MSETransportGuidByte2,
MSETransportGuidByte5,
MSETransportHaveTime3,
MSETransportHaveTime2,
MSEHavePitch,
MSEHaveSplineElev,
MSEHaveFallData,
MSEHaveFallDirection,
MSEPositionO,
MSEPositionX,
MSEPositionY,
MSEPositionZ,
MSETimestamp,
MSEGuidByte6_2,
MSETransportTime,
MSETransportPositionX,
MSETransportPositionY,
MSETransportPositionZ,
MSETransportPositionO,
MSETransportSeat,
MSETransportGuidByte3_2,
MSETransportGuidByte1_2,
MSETransportTime3,
MSETransportGuidByte6_2,
MSETransportGuidByte0_2,
MSETransportGuidByte5_2,
MSETransportTime2,
MSETransportGuidByte7_2,
MSETransportGuidByte4_2,
MSETransportGuidByte2_2,
MSEPitch,
MSESplineElev,
MSEFallVerticalSpeed,
MSEFallTime,
MSEFallHorizontalSpeed,
MSEFallCosAngle,
MSEFallSinAngle,
MSEGuidByte5_2,
MSEGuidByte1_2,
MSEGuidByte7_2,
MSEGuidByte0_2,
MSEGuidByte2_2,
MSEGuidByte4_2,
MSEGuidByte3_2,
};

MovementStatusElements MovementJumpSequence[] = {
MSEGuidByte5,
MSEGuidByte1,
MSEGuidByte6,
MSEFlags,
MSEGuidByte2,
MSEHaveSpline,
MSEGuidByte3,
MSEFlags2,
MSEGuidByte4,
MSEGuidByte0,
MSEGuidByte7,
MSEHaveFallData,
MSEHaveFallDirection,
MSEHaveTransportData,
MSETransportGuidByte6,
MSETransportGuidByte3,
MSETransportGuidByte7,
MSETransportGuidByte4,
MSETransportGuidByte1,
MSETransportGuidByte0,
MSETransportGuidByte2,
MSETransportGuidByte5,
MSETransportHaveTime3,
MSETransportHaveTime2,
MSEHaveSplineElev,
MSEHavePitch,
MSEPositionO,
MSEPositionX,
MSEPositionY,
MSEPositionZ,
MSETimestamp,
MSEGuidByte1_2,
MSEFallVerticalSpeed,
MSEFallTime,
MSEFallHorizontalSpeed,
MSEFallCosAngle,
MSEFallSinAngle,
MSETransportTime,
MSETransportPositionX,
MSETransportPositionY,
MSETransportPositionZ,
MSETransportPositionO,
MSETransportSeat,
MSETransportGuidByte3_2,
MSETransportGuidByte1_2,
MSETransportTime3,
MSETransportGuidByte6_2,
MSETransportGuidByte0_2,
MSETransportGuidByte5_2,
MSETransportTime2,
MSETransportGuidByte7_2,
MSETransportGuidByte4_2,
MSETransportGuidByte2_2,
MSEGuidByte6_2,
MSEGuidByte4_2,
MSESplineElev,
MSEGuidByte0_2,
MSEPitch,
MSEGuidByte5_2,
MSEGuidByte3_2,
MSEGuidByte7_2,
MSEGuidByte2_2,
};

MovementStatusElements MovementSetFacingSequence[] = {
MSEGuidByte3,
MSEGuidByte1,
MSEGuidByte0,
MSEGuidByte7,
MSEFlags2,
MSEHaveSpline,
MSEGuidByte4,
MSEFlags,
MSEGuidByte6,
MSEGuidByte5,
MSEGuidByte2,
MSEHaveFallData,
MSEHaveFallDirection,
MSEHaveTransportData,
MSETransportGuidByte6,
MSETransportGuidByte3,
MSETransportGuidByte7,
MSETransportGuidByte4,
MSETransportGuidByte1,
MSETransportGuidByte0,
MSETransportGuidByte2,
MSETransportGuidByte5,
MSETransportHaveTime3,
MSETransportHaveTime2,
MSEHavePitch,
MSEHaveSplineElev,
MSEPositionX,
MSEPositionY,
MSEPositionZ,
MSEPositionO,
MSETimestamp,
MSEFallVerticalSpeed,
MSEFallTime,
MSEFallHorizontalSpeed,
MSEFallCosAngle,
MSEFallSinAngle,
MSEGuidByte2_2,
MSEGuidByte4_2,
MSETransportTime,
MSETransportPositionX,
MSETransportPositionY,
MSETransportPositionZ,
MSETransportPositionO,
MSETransportSeat,
MSETransportGuidByte3_2,
MSETransportGuidByte1_2,
MSETransportTime3,
MSETransportGuidByte6_2,
MSETransportGuidByte0_2,
MSETransportGuidByte5_2,
MSETransportTime2,
MSETransportGuidByte7_2,
MSETransportGuidByte4_2,
MSETransportGuidByte2_2,
MSEGuidByte3_2,
MSEGuidByte7_2,
MSEPitch,
MSEGuidByte5_2,
MSEGuidByte1_2,
MSEGuidByte6_2,
MSESplineElev,
MSEGuidByte0_2,
};

MovementStatusElements MovementSetPitchSequence[] = {
MSEGuidByte4,
MSEGuidByte6,
MSEGuidByte2,
MSEFlags2,
MSEGuidByte1,
MSEGuidByte7,
MSEGuidByte5,
MSEGuidByte3,
MSEHaveSpline,
MSEGuidByte0,
MSEFlags,
MSEHaveFallData,
MSEHaveFallDirection,
MSEHaveTransportData,
MSETransportGuidByte6,
MSETransportGuidByte3,
MSETransportGuidByte7,
MSETransportGuidByte4,
MSETransportGuidByte1,
MSETransportGuidByte0,
MSETransportGuidByte2,
MSETransportGuidByte5,
MSETransportHaveTime3,
MSETransportHaveTime2,
MSEHaveSplineElev,
MSEHavePitch,
MSEPositionO,
MSETimestamp,
MSEPositionX,
MSEPositionY,
MSEPositionZ,
MSEGuidByte2_2,
MSEGuidByte6_2,
MSEGuidByte5_2,
MSEFallVerticalSpeed,
MSEFallTime,
MSEFallHorizontalSpeed,
MSEFallCosAngle,
MSEFallSinAngle,
MSEGuidByte3_2,
MSETransportTime,
MSETransportPositionX,
MSETransportPositionY,
MSETransportPositionZ,
MSETransportPositionO,
MSETransportSeat,
MSETransportGuidByte3_2,
MSETransportGuidByte1_2,
MSETransportTime3,
MSETransportGuidByte6_2,
MSETransportGuidByte0_2,
MSETransportGuidByte5_2,
MSETransportTime2,
MSETransportGuidByte7_2,
MSETransportGuidByte4_2,
MSETransportGuidByte2_2,
MSEGuidByte0_2,
MSEGuidByte1_2,
MSESplineElev,
MSEGuidByte7_2,
MSEGuidByte4_2,
MSEPitch,
};

MovementStatusElements MovementStartBackwardSequence[] = {
MSEGuidByte4,
MSEGuidByte1,
MSEGuidByte5,
MSEFlags2,
MSEGuidByte3,
MSEGuidByte6,
MSEGuidByte0,
MSEGuidByte2,
MSEGuidByte7,
MSEFlags,
MSEHaveSpline,
MSEHaveFallData,
MSEHaveFallDirection,
MSEHaveTransportData,
MSETransportGuidByte6,
MSETransportGuidByte3,
MSETransportGuidByte7,
MSETransportGuidByte4,
MSETransportGuidByte1,
MSETransportGuidByte0,
MSETransportGuidByte2,
MSETransportGuidByte5,
MSETransportHaveTime3,
MSETransportHaveTime2,
MSEHavePitch,
MSEHaveSplineElev,
MSEPositionX,
MSEPositionY,
MSEPositionZ,
MSETimestamp,
MSEPositionO,
MSEGuidByte0_2,
MSEGuidByte5_2,
MSEFallVerticalSpeed,
MSEFallTime,
MSEFallHorizontalSpeed,
MSEFallCosAngle,
MSEFallSinAngle,
MSEGuidByte4_2,
MSEGuidByte2_2,
MSETransportTime,
MSETransportPositionX,
MSETransportPositionY,
MSETransportPositionZ,
MSETransportPositionO,
MSETransportSeat,
MSETransportGuidByte3_2,
MSETransportGuidByte1_2,
MSETransportTime3,
MSETransportGuidByte6_2,
MSETransportGuidByte0_2,
MSETransportGuidByte5_2,
MSETransportTime2,
MSETransportGuidByte7_2,
MSETransportGuidByte4_2,
MSETransportGuidByte2_2,
MSEGuidByte6_2,
MSEPitch,
MSEGuidByte3_2,
MSESplineElev,
MSEGuidByte1_2,
MSEGuidByte7_2,
};

MovementStatusElements MovementStartForwardSequence[] = {
MSEGuidByte0,
MSEHaveSpline,
MSEGuidByte5,
MSEGuidByte3,
MSEGuidByte4,
MSEGuidByte2,
MSEGuidByte7,
MSEFlags2,
MSEFlags,
MSEGuidByte6,
MSEGuidByte1,
MSEHavePitch,
MSEHaveFallData,
MSEHaveFallDirection,
MSEHaveTransportData,
MSETransportGuidByte6,
MSETransportGuidByte3,
MSETransportGuidByte7,
MSETransportGuidByte4,
MSETransportGuidByte1,
MSETransportGuidByte0,
MSETransportGuidByte2,
MSETransportGuidByte5,
MSETransportHaveTime3,
MSETransportHaveTime2,
MSEHaveSplineElev,
MSETimestamp,
MSEPositionX,
MSEPositionY,
MSEPositionZ,
MSEPositionO,
MSEGuidByte7_2,
MSEPitch,
MSEGuidByte1_2,
MSEGuidByte2_2,
MSEFallVerticalSpeed,
MSEFallTime,
MSEFallHorizontalSpeed,
MSEFallCosAngle,
MSEFallSinAngle,
MSEGuidByte3_2,
MSEGuidByte5_2,
MSEGuidByte0_2,
MSEGuidByte6_2,
MSETransportTime,
MSETransportPositionX,
MSETransportPositionY,
MSETransportPositionZ,
MSETransportPositionO,
MSETransportSeat,
MSETransportGuidByte3_2,
MSETransportGuidByte1_2,
MSETransportTime3,
MSETransportGuidByte6_2,
MSETransportGuidByte0_2,
MSETransportGuidByte5_2,
MSETransportTime2,
MSETransportGuidByte7_2,
MSETransportGuidByte4_2,
MSETransportGuidByte2_2,
MSEGuidByte4_2,
MSESplineElev,
};

MovementStatusElements MovementStartStrafeLeftSequence[] = {
MSEGuidByte5,
MSEGuidByte0,
MSEGuidByte3,
MSEFlags,
MSEGuidByte6,
MSEGuidByte1,
MSEGuidByte4,
MSEFlags2,
MSEHaveSpline,
MSEGuidByte7,
MSEGuidByte2,
MSEHaveTransportData,
MSETransportGuidByte6,
MSETransportGuidByte3,
MSETransportGuidByte7,
MSETransportGuidByte4,
MSETransportGuidByte1,
MSETransportGuidByte0,
MSETransportGuidByte2,
MSETransportGuidByte5,
MSETransportHaveTime3,
MSETransportHaveTime2,
MSEHaveSplineElev,
MSEHaveFallData,
MSEHaveFallDirection,
MSEHavePitch,
MSEPositionO,
MSETimestamp,
MSEPositionX,
MSEPositionY,
MSEPositionZ,
MSEGuidByte3_2,
MSETransportTime,
MSETransportPositionX,
MSETransportPositionY,
MSETransportPositionZ,
MSETransportPositionO,
MSETransportSeat,
MSETransportGuidByte3_2,
MSETransportGuidByte1_2,
MSETransportTime3,
MSETransportGuidByte6_2,
MSETransportGuidByte0_2,
MSETransportGuidByte5_2,
MSETransportTime2,
MSETransportGuidByte7_2,
MSETransportGuidByte4_2,
MSETransportGuidByte2_2,
MSEGuidByte5_2,
MSEGuidByte1_2,
MSEGuidByte4_2,
MSEGuidByte2_2,
MSEGuidByte0_2,
MSESplineElev,
MSEGuidByte6_2,
MSEFallVerticalSpeed,
MSEFallTime,
MSEFallHorizontalSpeed,
MSEFallCosAngle,
MSEFallSinAngle,
MSEGuidByte7_2,
MSEPitch,
};

MovementStatusElements MovementStartStrafeRightSequence[] = {
MSEGuidByte2,
MSEFlags,
MSEGuidByte3,
MSEFlags2,
MSEGuidByte0,
MSEGuidByte6,
MSEHaveSpline,
MSEGuidByte1,
MSEGuidByte4,
MSEGuidByte5,
MSEGuidByte7,
MSEHaveSplineElev,
MSEHavePitch,
MSEHaveTransportData,
MSETransportGuidByte6,
MSETransportGuidByte3,
MSETransportGuidByte7,
MSETransportGuidByte4,
MSETransportGuidByte1,
MSETransportGuidByte0,
MSETransportGuidByte2,
MSETransportGuidByte5,
MSETransportHaveTime3,
MSETransportHaveTime2,
MSEHaveFallData,
MSEHaveFallDirection,
MSETimestamp,
MSEPositionO,
MSEPositionX,
MSEPositionY,
MSEPositionZ,
MSEGuidByte7_2,
MSEGuidByte1_2,
MSESplineElev,
MSEGuidByte3_2,
MSEPitch,
MSETransportTime,
MSETransportPositionX,
MSETransportPositionY,
MSETransportPositionZ,
MSETransportPositionO,
MSETransportSeat,
MSETransportGuidByte3_2,
MSETransportGuidByte1_2,
MSETransportTime3,
MSETransportGuidByte6_2,
MSETransportGuidByte0_2,
MSETransportGuidByte5_2,
MSETransportTime2,
MSETransportGuidByte7_2,
MSETransportGuidByte4_2,
MSETransportGuidByte2_2,
MSEGuidByte5_2,
MSEGuidByte2_2,
MSEGuidByte6_2,
MSEGuidByte4_2,
MSEGuidByte0_2,
MSEFallVerticalSpeed,
MSEFallTime,
MSEFallHorizontalSpeed,
MSEFallCosAngle,
MSEFallSinAngle,
};

MovementStatusElements MovementStartTurnLeftSequence[] = {
MSEFlags,
MSEGuidByte3,
MSEGuidByte5,
MSEGuidByte7,
MSEFlags2,
MSEGuidByte6,
MSEHaveSpline,
MSEGuidByte0,
MSEGuidByte2,
MSEGuidByte1,
MSEGuidByte4,
MSEHaveFallData,
MSEHaveFallDirection,
MSEHaveSplineElev,
MSEHavePitch,
MSEHaveTransportData,
MSETransportGuidByte6,
MSETransportGuidByte3,
MSETransportGuidByte7,
MSETransportGuidByte4,
MSETransportGuidByte1,
MSETransportGuidByte0,
MSETransportGuidByte2,
MSETransportGuidByte5,
MSETransportHaveTime3,
MSETransportHaveTime2,
MSEPositionO,
MSETimestamp,
MSEPositionX,
MSEPositionY,
MSEPositionZ,
MSEGuidByte2_2,
MSEGuidByte6_2,
MSEFallVerticalSpeed,
MSEFallTime,
MSEFallHorizontalSpeed,
MSEFallCosAngle,
MSEFallSinAngle,
MSEGuidByte4_2,
MSESplineElev,
MSEGuidByte0_2,
MSEGuidByte7_2,
MSEPitch,
MSEGuidByte1_2,
MSEGuidByte5_2,
MSEGuidByte3_2,
MSETransportTime,
MSETransportPositionX,
MSETransportPositionY,
MSETransportPositionZ,
MSETransportPositionO,
MSETransportSeat,
MSETransportGuidByte3_2,
MSETransportGuidByte1_2,
MSETransportTime3,
MSETransportGuidByte6_2,
MSETransportGuidByte0_2,
MSETransportGuidByte5_2,
MSETransportTime2,
MSETransportGuidByte7_2,
MSETransportGuidByte4_2,
MSETransportGuidByte2_2,
};

MovementStatusElements MovementStartTurnRightSequence[] = {
MSEGuidByte0,
MSEFlags,
MSEGuidByte7,
MSEHaveSpline,
MSEGuidByte4,
MSEGuidByte6,
MSEGuidByte3,
MSEGuidByte1,
MSEFlags2,
MSEGuidByte2,
MSEGuidByte5,
MSEHaveFallData,
MSEHaveFallDirection,
MSEHavePitch,
MSEHaveSplineElev,
MSEHaveTransportData,
MSETransportGuidByte6,
MSETransportGuidByte3,
MSETransportGuidByte7,
MSETransportGuidByte4,
MSETransportGuidByte1,
MSETransportGuidByte0,
MSETransportGuidByte2,
MSETransportGuidByte5,
MSETransportHaveTime3,
MSETransportHaveTime2,
MSEPositionO,
MSETimestamp,
MSEPositionX,
MSEPositionY,
MSEPositionZ,
MSEGuidByte1_2,
MSEGuidByte6_2,
MSEFallVerticalSpeed,
MSEFallTime,
MSEFallHorizontalSpeed,
MSEFallCosAngle,
MSEFallSinAngle,
MSEGuidByte0_2,
MSEGuidByte5_2,
MSEGuidByte2_2,
MSEPitch,
MSEGuidByte4_2,
MSEGuidByte3_2,
MSEGuidByte7_2,
MSESplineElev,
MSETransportTime,
MSETransportPositionX,
MSETransportPositionY,
MSETransportPositionZ,
MSETransportPositionO,
MSETransportSeat,
MSETransportGuidByte3_2,
MSETransportGuidByte1_2,
MSETransportTime3,
MSETransportGuidByte6_2,
MSETransportGuidByte0_2,
MSETransportGuidByte5_2,
MSETransportTime2,
MSETransportGuidByte7_2,
MSETransportGuidByte4_2,
MSETransportGuidByte2_2,
};

MovementStatusElements MovementStopSequence[] = {
MSEGuidByte4,
MSEGuidByte3,
MSEFlags,
MSEGuidByte5,
MSEGuidByte6,
MSEGuidByte0,
MSEGuidByte1,
MSEGuidByte2,
MSEGuidByte7,
MSEHaveSpline,
MSEFlags2,
MSEHavePitch,
MSEHaveSplineElev,
MSEHaveFallData,
MSEHaveFallDirection,
MSEHaveTransportData,
MSETransportGuidByte6,
MSETransportGuidByte3,
MSETransportGuidByte7,
MSETransportGuidByte4,
MSETransportGuidByte1,
MSETransportGuidByte0,
MSETransportGuidByte2,
MSETransportGuidByte5,
MSETransportHaveTime3,
MSETransportHaveTime2,
MSETimestamp,
MSEPositionX,
MSEPositionY,
MSEPositionZ,
MSEPositionO,
MSEGuidByte6_2,
MSEGuidByte5_2,
MSEGuidByte1_2,
MSEGuidByte3_2,
MSEPitch,
MSEGuidByte2_2,
MSESplineElev,
MSEGuidByte4_2,
MSEGuidByte0_2,
MSEGuidByte7_2,
MSEFallVerticalSpeed,
MSEFallTime,
MSEFallHorizontalSpeed,
MSEFallCosAngle,
MSEFallSinAngle,
MSETransportTime,
MSETransportPositionX,
MSETransportPositionY,
MSETransportPositionZ,
MSETransportPositionO,
MSETransportSeat,
MSETransportGuidByte3_2,
MSETransportGuidByte1_2,
MSETransportTime3,
MSETransportGuidByte6_2,
MSETransportGuidByte0_2,
MSETransportGuidByte5_2,
MSETransportTime2,
MSETransportGuidByte7_2,
MSETransportGuidByte4_2,
MSETransportGuidByte2_2,
};

MovementStatusElements MovementStopStrafeSequence[] = {
MSEGuidByte3,
MSEFlags,
MSEHaveSpline,
MSEGuidByte4,
MSEGuidByte0,
MSEFlags2,
MSEGuidByte5,
MSEGuidByte6,
MSEGuidByte7,
MSEGuidByte1,
MSEGuidByte2,
MSEHaveTransportData,
MSETransportGuidByte6,
MSETransportGuidByte3,
MSETransportGuidByte7,
MSETransportGuidByte4,
MSETransportGuidByte1,
MSETransportGuidByte0,
MSETransportGuidByte2,
MSETransportGuidByte5,
MSETransportHaveTime3,
MSETransportHaveTime2,
MSEHaveSplineElev,
MSEHavePitch,
MSEHaveFallData,
MSEHaveFallDirection,
MSEPositionX,
MSEPositionY,
MSEPositionZ,
MSETimestamp,
MSEPositionO,
MSEGuidByte2_2,
MSEGuidByte7_2,
MSEGuidByte5_2,
MSETransportTime,
MSETransportPositionX,
MSETransportPositionY,
MSETransportPositionZ,
MSETransportPositionO,
MSETransportSeat,
MSETransportGuidByte3_2,
MSETransportGuidByte1_2,
MSETransportTime3,
MSETransportGuidByte6_2,
MSETransportGuidByte0_2,
MSETransportGuidByte5_2,
MSETransportTime2,
MSETransportGuidByte7_2,
MSETransportGuidByte4_2,
MSETransportGuidByte2_2,
MSEGuidByte0_2,
MSESplineElev,
MSEPitch,
MSEFallVerticalSpeed,
MSEFallTime,
MSEFallHorizontalSpeed,
MSEFallCosAngle,
MSEFallSinAngle,
MSEGuidByte1_2,
MSEGuidByte3_2,
MSEGuidByte4_2,
MSEGuidByte6_2,
};

MovementStatusElements MovementStopTurnSequence[] = {
MSEGuidByte3,
MSEGuidByte5,
MSEGuidByte4,
MSEGuidByte2,
MSEFlags2,
MSEGuidByte0,
MSEGuidByte7,
MSEGuidByte6,
MSEGuidByte1,
MSEHaveSpline,
MSEFlags,
MSEHaveFallData,
MSEHaveFallDirection,
MSEHaveTransportData,
MSETransportGuidByte6,
MSETransportGuidByte3,
MSETransportGuidByte7,
MSETransportGuidByte4,
MSETransportGuidByte1,
MSETransportGuidByte0,
MSETransportGuidByte2,
MSETransportGuidByte5,
MSETransportHaveTime3,
MSETransportHaveTime2,
MSEHaveSplineElev,
MSEHavePitch,
MSEPositionO,
MSETimestamp,
MSEPositionX,
MSEPositionY,
MSEPositionZ,
MSEGuidByte4_2,
MSEGuidByte2_2,
MSEGuidByte5_2,
MSEFallVerticalSpeed,
MSEFallTime,
MSEFallHorizontalSpeed,
MSEFallCosAngle,
MSEFallSinAngle,
MSEGuidByte0_2,
MSEGuidByte7_2,
MSEGuidByte6_2,
MSETransportTime,
MSETransportPositionX,
MSETransportPositionY,
MSETransportPositionZ,
MSETransportPositionO,
MSETransportSeat,
MSETransportGuidByte3_2,
MSETransportGuidByte1_2,
MSETransportTime3,
MSETransportGuidByte6_2,
MSETransportGuidByte0_2,
MSETransportGuidByte5_2,
MSETransportTime2,
MSETransportGuidByte7_2,
MSETransportGuidByte4_2,
MSETransportGuidByte2_2,
MSESplineElev,
MSEGuidByte1_2,
MSEGuidByte3_2,
MSEPitch,
};

MovementStatusElements MovementStartAscendSequence[] = {
MSEGuidByte2,
MSEGuidByte3,
MSEGuidByte6,
MSEGuidByte4,
MSEGuidByte0,
MSEHaveSpline,
MSEGuidByte1,
MSEGuidByte5,
MSEFlags,
MSEFlags2,
MSEGuidByte7,
MSEHaveTransportData,
MSETransportGuidByte6,
MSETransportGuidByte3,
MSETransportGuidByte7,
MSETransportGuidByte4,
MSETransportGuidByte1,
MSETransportGuidByte0,
MSETransportGuidByte2,
MSETransportGuidByte5,
MSETransportHaveTime3,
MSETransportHaveTime2,
MSEHaveFallData,
MSEHaveFallDirection,
MSEHavePitch,
MSEHaveSplineElev,
MSEPositionO,
MSEPositionX,
MSEPositionY,
MSEPositionZ,
MSETimestamp,
MSETransportTime,
MSETransportPositionX,
MSETransportPositionY,
MSETransportPositionZ,
MSETransportPositionO,
MSETransportSeat,
MSETransportGuidByte3_2,
MSETransportGuidByte1_2,
MSETransportTime3,
MSETransportGuidByte6_2,
MSETransportGuidByte0_2,
MSETransportGuidByte5_2,
MSETransportTime2,
MSETransportGuidByte7_2,
MSETransportGuidByte4_2,
MSETransportGuidByte2_2,
MSEGuidByte5_2,
MSEGuidByte3_2,
MSEGuidByte4_2,
MSEFallVerticalSpeed,
MSEFallTime,
MSEFallHorizontalSpeed,
MSEFallCosAngle,
MSEFallSinAngle,
MSEGuidByte0_2,
MSEGuidByte2_2,
MSEPitch,
MSEGuidByte6_2,
MSESplineElev,
MSEGuidByte7_2,
MSEGuidByte1_2,
};

MovementStatusElements MovementStartDescendSequence[] = {
MSEGuidByte7,
MSEGuidByte0,
MSEGuidByte2,
MSEGuidByte1,
MSEGuidByte6,
MSEGuidByte4,
MSEGuidByte5,
MSEHaveSpline,
MSEGuidByte3,
MSEFlags2,
MSEFlags,
MSEHaveFallData,
MSEHaveFallDirection,
MSEHavePitch,
MSEHaveSplineElev,
MSEHaveTransportData,
MSETransportGuidByte6,
MSETransportGuidByte3,
MSETransportGuidByte7,
MSETransportGuidByte4,
MSETransportGuidByte1,
MSETransportGuidByte0,
MSETransportGuidByte2,
MSETransportGuidByte5,
MSETransportHaveTime3,
MSETransportHaveTime2,
MSETimestamp,
MSEPositionX,
MSEPositionY,
MSEPositionZ,
MSEPositionO,
MSEGuidByte3_2,
MSEGuidByte4_2,
MSEGuidByte2_2,
MSEFallVerticalSpeed,
MSEFallTime,
MSEFallHorizontalSpeed,
MSEFallCosAngle,
MSEFallSinAngle,
MSEPitch,
MSEGuidByte5_2,
MSESplineElev,
MSETransportTime,
MSETransportPositionX,
MSETransportPositionY,
MSETransportPositionZ,
MSETransportPositionO,
MSETransportSeat,
MSETransportGuidByte3_2,
MSETransportGuidByte1_2,
MSETransportTime3,
MSETransportGuidByte6_2,
MSETransportGuidByte0_2,
MSETransportGuidByte5_2,
MSETransportTime2,
MSETransportGuidByte7_2,
MSETransportGuidByte4_2,
MSETransportGuidByte2_2,
MSEGuidByte1_2,
MSEGuidByte0_2,
MSEGuidByte7_2,
MSEGuidByte6_2,
};

MovementStatusElements MovementStartSwimSequence[] = {
MSEGuidByte1,
MSEGuidByte5,
MSEFlags,
MSEGuidByte2,
MSEHaveSpline,
MSEGuidByte6,
MSEFlags2,
MSEGuidByte4,
MSEGuidByte7,
MSEGuidByte0,
MSEGuidByte3,
MSEHavePitch,
MSEHaveSplineElev,
MSEHaveFallData,
MSEHaveFallDirection,
MSEHaveTransportData,
MSETransportGuidByte6,
MSETransportGuidByte3,
MSETransportGuidByte7,
MSETransportGuidByte4,
MSETransportGuidByte1,
MSETransportGuidByte0,
MSETransportGuidByte2,
MSETransportGuidByte5,
MSETransportHaveTime3,
MSETransportHaveTime2,
MSEPositionO,
MSEPositionX,
MSEPositionY,
MSEPositionZ,
MSETimestamp,
MSEPitch,
MSEGuidByte4_2,
MSESplineElev,
MSEFallVerticalSpeed,
MSEFallTime,
MSEFallHorizontalSpeed,
MSEFallCosAngle,
MSEFallSinAngle,
MSEGuidByte0_2,
MSEGuidByte7_2,
MSETransportTime,
MSETransportPositionX,
MSETransportPositionY,
MSETransportPositionZ,
MSETransportPositionO,
MSETransportSeat,
MSETransportGuidByte3_2,
MSETransportGuidByte1_2,
MSETransportTime3,
MSETransportGuidByte6_2,
MSETransportGuidByte0_2,
MSETransportGuidByte5_2,
MSETransportTime2,
MSETransportGuidByte7_2,
MSETransportGuidByte4_2,
MSETransportGuidByte2_2,
MSEGuidByte6_2,
MSEGuidByte5_2,
MSEGuidByte2_2,
MSEGuidByte1_2,
MSEGuidByte3_2,
};

MovementStatusElements MovementStopAscendSequence[] = {
MSEHaveSpline,
MSEGuidByte5,
MSEGuidByte3,
MSEGuidByte1,
MSEFlags,
MSEGuidByte4,
MSEGuidByte7,
MSEGuidByte2,
MSEFlags2,
MSEGuidByte6,
MSEGuidByte0,
MSEHavePitch,
MSEHaveSplineElev,
MSEHaveFallData,
MSEHaveFallDirection,
MSEHaveTransportData,
MSETransportGuidByte6,
MSETransportGuidByte3,
MSETransportGuidByte7,
MSETransportGuidByte4,
MSETransportGuidByte1,
MSETransportGuidByte0,
MSETransportGuidByte2,
MSETransportGuidByte5,
MSETransportHaveTime3,
MSETransportHaveTime2,
MSETimestamp,
MSEPositionX,
MSEPositionY,
MSEPositionZ,
MSEPositionO,
MSEGuidByte5_2,
MSEGuidByte2_2,
MSEGuidByte0_2,
MSEGuidByte7_2,
MSEPitch,
MSEGuidByte3_2,
MSEGuidByte4_2,
MSEGuidByte1_2,
MSESplineElev,
MSEFallVerticalSpeed,
MSEFallTime,
MSEFallHorizontalSpeed,
MSEFallCosAngle,
MSEFallSinAngle,
MSEGuidByte6_2,
MSETransportTime,
MSETransportPositionX,
MSETransportPositionY,
MSETransportPositionZ,
MSETransportPositionO,
MSETransportSeat,
MSETransportGuidByte3_2,
MSETransportGuidByte1_2,
MSETransportTime3,
MSETransportGuidByte6_2,
MSETransportGuidByte0_2,
MSETransportGuidByte5_2,
MSETransportTime2,
MSETransportGuidByte7_2,
MSETransportGuidByte4_2,
MSETransportGuidByte2_2,
};

MovementStatusElements* GetMovementStatusElementsSequence(uint16 opcode)
{
    switch(opcode)
    {
    case SMSG_PLAYER_MOVE:
        return PlayerMoveSequence;
    case MSG_MOVE_FALL_LAND:
        return MovementFallLandSequence;
    case MSG_MOVE_HEARTBEAT:
        return MovementHeartBeatSequence;
    case MSG_MOVE_JUMP:
        return MovementJumpSequence;
    case MSG_MOVE_SET_FACING:
        return MovementSetFacingSequence;
    case MSG_MOVE_SET_PITCH:
        return MovementSetPitchSequence;
    case MSG_MOVE_START_BACKWARD:
        return MovementStartBackwardSequence;
    case MSG_MOVE_START_FORWARD:
        return MovementStartForwardSequence;
    case MSG_MOVE_START_STRAFE_LEFT:
        return MovementStartStrafeLeftSequence;
    case MSG_MOVE_START_STRAFE_RIGHT:
        return MovementStartStrafeRightSequence;
    case MSG_MOVE_START_TURN_LEFT:
        return MovementStartTurnLeftSequence;
    case MSG_MOVE_START_TURN_RIGHT:
        return MovementStartTurnRightSequence;
    case MSG_MOVE_STOP:
        return MovementStopSequence;
    case MSG_MOVE_STOP_STRAFE:
        return MovementStopStrafeSequence;
    case MSG_MOVE_STOP_TURN:
        return MovementStopTurnSequence;
    case MSG_MOVE_START_ASCEND:
        return MovementStartAscendSequence;
    case MSG_MOVE_START_DESCEND:
        return MovementStartDescendSequence;
    case MSG_MOVE_START_SWIM:
        return MovementStartSwimSequence;
    case MSG_MOVE_STOP_ASCEND:
        return MovementStopAscendSequence;
    }
    return NULL;
}

void MovementInfo::ReadMovementInfoSecondChance(WorldPacket &data)
{
	switch( data.GetOpcode() )
	{
		case MSG_MOVE_JUMP_STOP_SWIM:
		{
			flags = 0;
			flags2 = 0;
//            flags = data.readBits(30);
//            flags2 = data.readBits(12);
			uint8 ContentMask[7];
		    uint8 *guid = (uint8 *)&moverGUID;
			*(uint64*)guid = 0;
			data >> (uint32)(*(uint32*)&ContentMask[0] );
			data >> ContentMask[4];
			data >> ContentMask[5];
			data >> ContentMask[6];
			float UnkFloat;
			data >> UnkFloat;
			data >> x;
			data >> y;
			data >> z;
			data >> client_ms_time;
			if( ContentMask[0] & 0x40 )
				data >> guid[3];
			if( ContentMask[0] & 0x08 )
				data >> guid[0];
			if( ContentMask[0] & 0x80 )
				data >> guid[2];
			data >> orientation;
			if( ContentMask[0] & 0x02 )
				data >> guid[1];
			GUID_un_obfuscate( moverGUID );
/*
20 22 24 26 = 21 23 25 27
{CLIENT} Packet: ( 06378 ) MSG_MOVE_JUMP_STOP_SWIM PacketSize=35 Comment= - not yet implemented handler - Bad mover guid 2228480 
CA 16 80 00 00 00 04 = 4 + 1 + 1 + 1 = 7
DA 00 19 3F 0.59766924381256 
60 B1 0B C6 -8940.34375 
33 8E E5 C2 -114.77773284912 
5E C3 B0 42 88.381576538086 
BF 49 82 00 
21 G3
27 G0
23 G2
68 D9 B5 BE -0.35517430305481 
25 G1
{CLIENT} Packet: ( 06378 ) MSG_MOVE_JUMP_STOP_SWIM PacketSize=32 Comment= - not yet implemented handler - Bad mover guid 0 
08 16 80 00 00 00 04  = 1 + 1 + 1 + 1 = 4
E7 5F 6B 40 o1
75 DA 0B C6 x
30 45 E4 C2 y
9E C0 AF 42 z
1D 3E 6D 00 t
E2 g0
DD 69 0D BF o2
{CLIENT} Packet: ( 06378 ) MSG_MOVE_JUMP_STOP_SWIM PacketSize=32 Comment= - not yet implemented handler - X diff is too large -8937.448242
02 16 80 00 06 00 04 
6E FC 76 40 o1	3.8591570854187 
8C AA 0B C6 x
C1 EC E8 C2 y
FC E7 A4 42 z
C7 27 70 00 t
46 E4 BD BE o2	-0.37088221311569 
0E g1
{CLIENT} Packet: ( 06378 ) MSG_MOVE_JUMP_STOP_SWIM PacketSize=32 Comment= - not yet implemented handler - X diff is too large -8934.480469
80 16 80 00 02 00 04 
EB 8C A7 40 
EC 9D 0B C6 
E8 6A E5 C2 
D4 DB A4 42 
85 3A 76 00 
1E g2		//001F0000
A7 F4 C9 BE
{CLIENT} Packet: ( 06378 ) MSG_MOVE_JUMP_STOP_SWIM PacketSize=32 Comment= - not yet implemented handler - X diff is too large -8935.201172
40 16 80 00 02 00 04 
B1 39 AC 40 
CE A0 0B C6 
94 81 E5 C2 
08 E4 A4 42
39 A6 78 00 
3E g3		//3F000000
9A 1A 7B BE 
*/
			flags |= MOVEFLAG_AIR_SUSPENSION14333;
		}break;
		case MSG_MOVE_START_DESCEND:
		{
/*
20 22 24 26 = 21 23 25 27
{CLIENT} Packet: ( 06249 ) MSG_MOVE_START_DESCEND PacketSize=35 Comment= - Bad mover guid 2228480 
31 60 00 00 08 00 30 = 3 + 2 + 1 + 2 = 7
13 4A 82 00 t
60 B1 0B C6 x
33 8E E5 C2 y
7B 1D B3 42 z
DA 00 19 3F o1
23 G2
27 G0
68 D9 B5 BE o2
21 G3
25 G1
{CLIENT} Packet: ( 06249 ) MSG_MOVE_START_DESCEND PacketSize=32 Comment= - X diff is too large -8821.944672
01 60 00 00 40 00 30 = 1 + 2 + 1 + 2 = 6
1F 74 68 00 t
A7 A7 0B C6 x
D4 C3 E2 C2 y
D1 D1 AF 42 z
E6 7C 51 40 o1
E2 g0
F6 0F 9F BE o2
{CLIENT} Packet: ( 06249 ) MSG_MOVE_START_DESCEND PacketSize=32 Comment= - Bad mover guid 256 
11 60 00 00 00 00 10 
67 80 72 00 t
15 FA 0B C6 x
30 AC DA C2 y
D4 BB AC 42 z
D0 FC 83 40 o1 4.1246109008789 
00 00 00 00 
0E g1
{CLIENT} Packet: ( 06249 ) MSG_MOVE_START_DESCEND PacketSize=32 Comment= - Bad mover guid 0 
01 60 00 00 08 00 10 
C8 3A 76 00 
EC 9D 0B C6
E8 6A E5 C2 
96 B4 A8 42
EB 8C A7 40 
1E g2
A7 F4 C9 BE
{CLIENT} Packet: ( 06249 ) MSG_MOVE_START_DESCEND PacketSize=32 Comment= - Bad mover guid 10158080 
21 60 00 04 00 00 10 
02 A7 78 00 
CE A0 0B C6
94 81 E5 C2 
CC 84 AA 42
91 EE B6 40 
9A 1A 7B BE 
3E g3
{CLIENT} Packet: ( 06249 ) MSG_MOVE_START_DESCEND PacketSize=60 Comment= - Abnormal orientation 130310114986892740000000.000000 - Abnormal falltime 12255232 - Bad mover guid 4653121 
11 60 00 00 48 00 33 
48 00 
88 7B 4C 00 
A5 C0 BE C1 
B8 42 14 C1 
A3 67 01 C1 
DC 65 46 40 
3B ED 00 00 00 00 BB 00 00 00 00 
A5 C0 BE C1 
B8 42 14 C1 
A3 67 01 C1
DC 65 46 40 
FF 13 07 F0 
*/
			flags = 0;
			flags2 = 0;
//            flags = data.readBits(30);
//            flags2 = data.readBits(12);

			uint8 ContentMask[7];
		    uint8 *guid = (uint8 *)&moverGUID;
			*(uint64*)guid = 0;
			data >> (uint32)(*(uint32*)&ContentMask[0] );
			data >> ContentMask[4];
			data >> ContentMask[5];
			data >> ContentMask[6];
			data >> client_ms_time;
			data >> x;
			data >> y;
			data >> z;
			data >> orientation;
			if( ContentMask[4] & 0x08 )
				data >> guid[2];
			if( ContentMask[6] & 0x20 )
				data >> guid[0];
			float UnkFloat;
			data >> UnkFloat;
			if( ContentMask[0] & 0x20 )
				data >> guid[3];
			if( ContentMask[0] & 0x10 )
				data >> guid[1];

			GUID_un_obfuscate( moverGUID );

			flags |= MOVEFLAG_AIR_SUSPENSION14333;
		}break;
		case MSG_MOVE_STOP_ASCEND:
		{
			flags = 0;
			flags2 = 0;
//            flags = data.readBits(30);
//            flags2 = data.readBits(12);

			uint8 ContentMask[7];
		    uint8 *guid = (uint8 *)&moverGUID;
			*(uint64*)guid = 0;
			data >> (uint32)(*(uint32*)&ContentMask[0] );
			data >> ContentMask[4];
			data >> ContentMask[5];
			data >> ContentMask[6];
			data >> client_ms_time;
			data >> x;
			data >> y;
			data >> z;
			data >> orientation;
			if( ContentMask[1] & 0x80 )
				data >> guid[3];
			if( ContentMask[0] & 0x20 )
				data >> guid[2];
			float UnkFloat;
			data >> UnkFloat;
			if( ContentMask[0] & 0x10 )
				data >> guid[1];
			if( ContentMask[0] & 0x40 )
				data >> guid[0];

			GUID_un_obfuscate( moverGUID );
/*
20 22 24 26 = 21 23 25 27
{CLIENT} Packet: ( 02121 ) MSG_MOVE_STOP_ASCEND PacketSize=35 Comment= - Bad mover guid 570461440 
70 80 00 B8 00 04 08 
9E 26 A4 00 t
84 F5 0B C6 -8957.37890625 
61 80 DF C2 -111.75074005127 
7B 47 C5 42 98.639610290527 
11 EB C0 40 6.0286946296692 
21 g3
23 g2
8C F4 C9 BD -0.09861096739769 
25 g1
27 g0
{CLIENT} Packet: ( 02121 ) MSG_MOVE_STOP_ASCEND PacketSize=32 Comment= - Bad mover guid 0 
40 00 00 B8 00 00 08 
AA 9D B2 00 
7A A2 0B C6 
37 F9 E2 C2 
1A 62 A9 42 
85 8C B8 40 
00 00 00 00 
27 g0
{CLIENT} Packet: ( 02121 ) MSG_MOVE_STOP_ASCEND PacketSize=32 Comment= - Bad mover guid 256 
10 00 00 B8 00 00 08 
40 BF B3 00
ED 9A 0B C6 
E3 52 E5 C2 
D8 A3 A8 42 
9C 5F A5 40
00 00 00 00 
25 g1
{CLIENT} Packet: ( 02121 ) MSG_MOVE_STOP_ASCEND PacketSize=32 Comment= - Bad mover guid 570425344 
20 00 00 B8 00 00 08
8D 9E B4 00 
D5 A0 0B C6
A0 56 E6 C2 
22 AA A9 42 
04 F6 B5 40
23 g2
00 00 00 00 
{CLIENT} Packet: ( 02121 ) MSG_MOVE_STOP_ASCEND PacketSize=32 Comment= - Bad mover guid 0 
00 80 00 B8 00 00 08 
1B 33 B5 00
80 D5 0B C6
04 26 DC C2 
E0 8A A8 42 
92 91 C5 40 
21 g3
00 00 00 00 
*/
			flags |= MOVEFLAG_AIR_SUSPENSION14333;
		}break;
/*
{CLIENT} Packet: ( 47202 ) MSG_MOVE_STOP_SWIM PacketSize=50 Comment= - X diff is too large 2179.619873 - Bad mover guid 0 
80 00 28 00 01 00 26 
76 4E 77 40 3.8641638755798 - o
E6 29 08 45 2178.6186523438  - x
DD 94 25 C5  -2649.3039550781 - y
97 36 C5 C1 -24.651655197144 - z
95 F8 AB 01 34 9B AB - guid + time ?
00 00 00 00 
00 00 00 00 
71 1C 97 40  4.7222218513489 
11 07 40 BF -0.7501078248024 
FA 4B 29 BF -0.66131556034088 
{CLIENT} Packet: ( 47202 ) MSG_MOVE_STOP_SWIM PacketSize=50 Comment= - X diff is too large -9061.677734 - Bad mover guid 0 
80 00 28 00 01 00 26 
32 6F 42 40
E8 9A 0D C6 
23 3B FF 43 
D9 30 8B 42 
78 5D 15 4C 39 92 5F 
00 00 00 00 
00 00 00 00 
71 1C B7 40 
EB A0 7E BF 
18 B4 D3 3D
*/
		default:
			break;
	};
}

void MovementInfo::ReadMovementInfo(WorldPacket &data)
{
    HaveTransportData = false;
    HaveTransportTime2 = false;
    HaveTransportTime3 = false;
    HavePitch = false;
    HaveFallData = false;
    HaveFallDirection = false;
    HaveSplineElevation = false;
    HaveSpline = false;
    uint8 *guid = (uint8 *)&moverGUID;
    uint8 *tguid = (uint8 *)&transGuid;
    *(uint64*)guid = 0;
    *(uint64*)tguid = 0;

	x = -1.0f;	//just to signal not yet handled packets
	FallTime = 0;
	transGuid = 0;
    MovementStatusElements *sequence = GetMovementStatusElementsSequence( data.GetOpcode() );
    if(sequence == NULL)
	{
        return;
	}
    for(uint32 i=0; i < MSE_COUNT; i++)
    {
        MovementStatusElements element = sequence[i];
        
        if (element >= MSEGuidByte0 && element <= MSEGuidByte7)
        {
            data.ReadByteMask(guid[element - MSEGuidByte0]);
            continue;
        }

        if (element >= MSETransportGuidByte0 &&
            element <= MSETransportGuidByte7)
        {
            if (HaveTransportData)
                data.ReadByteMask(tguid[element - MSETransportGuidByte0]);
            continue;
        }

        if (element >= MSEGuidByte0_2 && element <= MSEGuidByte7_2)
        {
            data.ReadByteSeq(guid[element - MSEGuidByte0_2]);
            continue;
        }

        if (element >= MSETransportGuidByte0_2 &&
            element <= MSETransportGuidByte7_2)
        {
            if (HaveTransportData)
                data.ReadByteSeq(tguid[element - MSETransportGuidByte0_2]);
            continue;
        }

        switch (element)
        {
            case MSEFlags:
                flags = data.readBits(30);
                break;
            case MSEFlags2:
                flags2 = data.readBits(12);
                break;
            case MSETimestamp:
                data >> client_ms_time;
                break;
            case MSEHavePitch:
                HavePitch = data.readBit();
                break;
            case MSEHaveFallData:
                HaveFallData = data.readBit();
                break;
            case MSEHaveFallDirection:
                if (HaveFallData)
                    HaveFallDirection = data.readBit();
                break;
            case MSEHaveTransportData:
                HaveTransportData = data.readBit();
                break;
            case MSETransportHaveTime2:
                if (HaveTransportData)
                    HaveTransportTime2 = data.readBit();
                break;
            case MSETransportHaveTime3:
                if (HaveTransportData)
                    HaveTransportTime3 = data.readBit();
                break;
            case MSEHaveSpline:
                HaveSpline = data.readBit();
                break;
            case MSEHaveSplineElev:
                HaveSplineElevation = data.readBit();
                break;
            case MSEPositionX:
//                data >> pos;
                data >> x;
                data >> y;
                data >> z;
                break;
            case MSEPositionY:
            case MSEPositionZ:
                break;  // assume they always go as vector of 3
            case MSEPositionO:
                data >> orientation;
                break;
            case MSEPitch:
                if (HavePitch)
                    data >> pitch;
                break;
            case MSEFallTime:
                if (HaveFallData)
                    data >> FallTime;
                break;
            case MSESplineElev:
                if (HaveSplineElevation)
                    data >> splineElevation;
                break;
            case MSEFallHorizontalSpeed:
                if (HaveFallDirection)
                    data >> j_xyspeed;
                break;
            case MSEFallVerticalSpeed:
                if (HaveFallData)
                    data >> j_zspeed;
                break;
            case MSEFallCosAngle:
                if (HaveFallDirection)
                    data >> j_cosAngle;
                break;
            case MSEFallSinAngle:
                if (HaveFallDirection)
                    data >> j_sinAngle;
                break;
            case MSETransportSeat:
                if (HaveTransportData)
                    data >> t_seat;
                break;
            case MSETransportPositionO:
                if (HaveTransportData)
                    data >> transO;
                break;
            case MSETransportPositionX:
                if (HaveTransportData)
				{
//                    data >> t_pos;
	                data >> transX;
	                data >> transY;
	                data >> transZ;
				}
                break;
            case MSETransportPositionY:
            case MSETransportPositionZ:
                break;  // assume they always go as vector of 3
            case MSETransportTime:
                if (HaveTransportData)
                    data >> t_time;
                break;
            case MSETransportTime2:
                if (HaveTransportTime2)
                    data >> t_time2;
                break;
            case MSETransportTime3:
                if (HaveTransportTime3)
                    data >> t_time3;
                break;
            default:
				sLog.outDebug("Incorrect sequence element detected at ReadMovementInfo");
        }
    }
//printf("x=%04.04f y=%04.04f z=%04.04f opcode 0x%04x\n",pos.x,pos.y,pos.z,data.GetOpcode() );
//    moverGUID = *(uint64*)guid;
//    transGuid = *(uint64*)tguid;
}


void MovementInfo::WriteMovementInfo(WorldPacket &data)
{
/*    bool HaveTransportData = HasMovementFlag(MOVEMENTFLAG_ONTRANSPORT);
    bool HaveTransportTime2 = (flags2 & MOVEMENTFLAG2_INTERPOLATED_MOVEMENT) != 0;
    bool HaveTransportTime3 = false;
    bool HavePitch = (HasMovementFlag(MovementFlags(MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING))) 
        || (flags2 & MOVEMENTFLAG2_ALWAYS_ALLOW_PITCHING);
    bool HaveFallData = HasExtraMovementFlag(MOVEMENTFLAG2_INTERPOLATED_TURNING);
    bool HaveFallDirection = HasMovementFlag(MOVEMENTFLAG_JUMPING);
    bool HaveSplineElevation = HasMovementFlag(MOVEMENTFLAG_SPLINE_ELEVATION);
    bool HaveSpline = false; 
	/**/

    MovementStatusElements *sequence = GetMovementStatusElementsSequence( SMSG_PLAYER_MOVE );
    if(!sequence)
        return;
    uint8 *guid = (uint8 *)&moverGUID;
    uint8 *tguid = (uint8 *)&transGuid;
    for(uint32 i=0; i < MSE_COUNT; i++)
    {
        MovementStatusElements element = sequence[i];
        
        if (element >= MSEGuidByte0 && element <= MSEGuidByte7)
        {
            data.WriteByteMask(guid[element - MSEGuidByte0]);
            continue;
        }

        if (element >= MSETransportGuidByte0 &&
            element <= MSETransportGuidByte7)
        {
            if (HaveTransportData)
                data.WriteByteMask(tguid[element - MSETransportGuidByte0]);
            continue;
        }

        if (element >= MSEGuidByte0_2 && element <= MSEGuidByte7_2)
        {
            data.WriteByteSeq(guid[element - MSEGuidByte0_2]);
            continue;
        }

        if (element >= MSETransportGuidByte0_2 &&
            element <= MSETransportGuidByte7_2)
        {
            if (HaveTransportData)
                data.WriteByteSeq(tguid[element - MSETransportGuidByte0_2]);
            continue;
        }

        switch (element)
        {
            case MSEFlags:
                data.writeBits(flags, 30);
                break;
            case MSEFlags2:
                data.writeBits(flags2, 12);
                break;
            case MSETimestamp:
                data << client_ms_time;	//might have got overwritten
                break;
            case MSEHavePitch:
                data.writeBit(HavePitch);
                break;
            case MSEHaveFallData:
                data.writeBit(HaveFallData);
                break;
            case MSEHaveFallDirection:
                if (HaveFallData)
                    data.writeBit(HaveFallDirection);
                break;
            case MSEHaveTransportData:
                data.writeBit(HaveTransportData);
                break;
            case MSETransportHaveTime2:
                if (HaveTransportData)
                    data.writeBit(HaveTransportTime2);
                break;
            case MSETransportHaveTime3:
                if (HaveTransportData)
                    data.writeBit(HaveTransportTime3);
                break;
            case MSEHaveSpline:
                data.writeBit(HaveSpline);
                break;
            case MSEHaveSplineElev:
                data.writeBit(HaveSplineElevation);
                break;
            case MSEPositionX:
//                data << pos;
                data << x;
                data << y;
                data << z;
                break;
            case MSEPositionY:
            case MSEPositionZ:
                break;  // assume they always go as vector of 3
            case MSEPositionO:
                data << orientation;
                break;
            case MSEPitch:
                if (HavePitch)
                    data << pitch;
                break;
            case MSEFallTime:
                if (HaveFallData)
                    data << FallTime;
                break;
            case MSESplineElev:
                if (HaveSplineElevation)
                    data << splineElevation;
                break;
            case MSEFallHorizontalSpeed:
                if (HaveFallDirection)
                    data << j_xyspeed;
                break;
            case MSEFallVerticalSpeed:
                if (HaveFallData)
                    data << j_zspeed;
                break;
            case MSEFallCosAngle:
                if (HaveFallDirection)
                    data << j_cosAngle;
                break;
            case MSEFallSinAngle:
                if (HaveFallDirection)
                    data << j_sinAngle;
                break;
            case MSETransportSeat:
                if (HaveTransportData)
                    data << t_seat;
                break;
            case MSETransportPositionO:
                if (HaveTransportData)
                    data << transO;
                break;
            case MSETransportPositionX:
                if (HaveTransportData)
				{
//                    data << t_pos;
					data << transX;
					data << transY;
					data << transZ;
				}
                break;
            case MSETransportPositionY:
            case MSETransportPositionZ:
                break;  // assume they always go as vector of 3
            case MSETransportTime:
                if (HaveTransportData)
                    data << t_time;
                break;
            case MSETransportTime2:
                if (HaveTransportTime2)
                    data << t_time2;
                break;
            case MSETransportTime3:
                if (HaveTransportTime3)
                    data << t_time3;
                break;
            default:
				sLog.outDebug("Incorrect sequence element detected at ReadMovementInfo");
        }
    }
}
#endif