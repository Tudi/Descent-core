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

//
// WorldSession.cpp
//

#include "StdAfx.h"

#define WORLDSOCKET_TIMEOUT		240				//seems like some players have insane loadup time and this triggers if smaller then 4 minutes
#define PLAYER_LOGOUT_DELAY		(20*1000)		/* 20 seconds should be more than enough to gank ya. */

OpcodeHandler WorldPacketHandlers[NUM_MSG_TYPES];

WorldSession::WorldSession(uint32 id, string Name, WorldSocket *sock) : _player(0), _socket(sock), _accountId(id), _accountName(Name),
_logoutTime(0), permissions(NULL), permissioncount(0), _loggingOut(false)
{
//	memset(movement_packet, 0, sizeof(movement_packet));
	m_currMsTime = getMSTime();
	bDeleted = false;
	m_bIsWLevelSet = false;
	floodLines = 0;
	floodTime = UNIXTIME;
	_updatecount = 0;
	m_loggingInPlayer=NULL;
	language=0;
	m_muted = 0;
	_side = -1;
	MaxAvailCharLevel = 0;
	movement_info.FallTime = 0;
	session_instanceId = WORLD_INSTANCE;

	for(uint32 x=0;x<ACCOUNT_DATA_SIZE;x++)
		sAccountData[x].data=NULL;	
	m_lastRecvPacket = m_lastPing = (uint32)UNIXTIME;
	m_ClientTimeSpeedHackDetectionCount = 0;
#ifdef _DEBUG
	memset( &prev_movement_info, 0 , sizeof( prev_movement_info ));
#endif
	m_wLevel = -99999;
#ifdef GM_COMMAND_TO_LOG_PLAYER_INCOMMING_PACKETS
	LogIncommingPackets = NULL;
	LogOutGoungPackets = NULL;
#endif
}

WorldSession::~WorldSession()
{
	deleteMutex.Acquire();

	if(HasGMPermissions())
		sWorld.gmList.erase(this);

	if(_player)
	{
		sLog.outError("warning: logged out player in worldsession destructor");
		LogoutPlayer(true);
	}

	if(permissions)
	{
		delete [] permissions;
		permissions = NULL;
	}

	WorldPacket *packet;

	while((packet = _recvQueue.Pop()) != 0)
		delete packet;

	for(uint32 x=0;x<ACCOUNT_DATA_SIZE;x++)
		if(sAccountData[x].data)
		{
			delete [] sAccountData[x].data;
			sAccountData[x].data = NULL;
		}

#ifndef CLUSTERING
	// Hmm, and why not DC ?
	if(_socket)
	{
//		_socket->SetSession(0);
		Disconnect();
	}
#endif

	//what about memleaks ?
	if(m_loggingInPlayer)
		m_loggingInPlayer->SetSession(NULL);

	deleteMutex.Release();

#ifdef GM_COMMAND_TO_LOG_PLAYER_INCOMMING_PACKETS
	if( LogIncommingPackets != NULL )
		fclose( LogIncommingPackets );
	if( LogOutGoungPackets != NULL )
		fclose( LogOutGoungPackets );
#endif
}

void WorldSession::_Delete()
{
	delete this;
}

#ifdef GM_COMMAND_TO_LOG_PLAYER_INCOMMING_PACKETS

void LogSessionPacket( unsigned int direction, uint16 opcode, uint32 len, const uint8 *data, FILE *LogPackets )
{
	FILE *m_file = LogPackets;

	unsigned int line = 1;
	unsigned int countpos = 0;
	uint16 lenght = len;
	unsigned int count = 0;

	fprintf(m_file, "{%s} Packet: (0x%04X) %s PacketSize = %u stamp = %u\n", (direction ? "SERVER" : "CLIENT"), opcode,
		LookupName(opcode, g_worldOpcodeNames), lenght, getMSTime() );
	fprintf(m_file, "|------------------------------------------------|----------------|\n");
	fprintf(m_file, "|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|\n");
	fprintf(m_file, "|------------------------------------------------|----------------|\n");

	if(lenght > 0)
	{
		fprintf(m_file, "|");
		for (count = 0 ; count < lenght ; count++)
		{
			if (countpos == 16)
			{
				countpos = 0;

				fprintf(m_file, "|");

				for (unsigned int a = count-16; a < count;a++)
				{
					if ((data[a] < 32) || (data[a] > 126))
						fprintf(m_file, ".");
					else
						fprintf(m_file, "%c",data[a]);
				}

				fprintf(m_file, "|\n");

				line++;
				fprintf(m_file, "|");
			}

			fprintf(m_file, "%02X ",data[count]);

			//FIX TO PARSE PACKETS WITH LENGHT < OR = TO 16 BYTES.
			if (count+1 == lenght && lenght <= 16)
			{
				for (unsigned int b = countpos+1; b < 16;b++)
					fprintf(m_file, "   ");

				fprintf(m_file, "|");

				for (unsigned int a = 0; a < lenght;a++)
				{
					if ((data[a] < 32) || (data[a] > 126))
						fprintf(m_file, ".");
					else
						fprintf(m_file, "%c",data[a]);
				}

				for (unsigned int c = count; c < 15;c++)
					fprintf(m_file, " ");

				fprintf(m_file, "|\n");
			}

			//FIX TO PARSE THE LAST LINE OF THE PACKETS WHEN THE LENGHT IS > 16 AND ITS IN THE LAST LINE.
			if (count+1 == lenght && lenght > 16)
			{
				for (unsigned int b = countpos+1; b < 16;b++)
					fprintf(m_file, "   ");

				fprintf(m_file, "|");

				unsigned short print = 0;

				for (unsigned int a = line * 16 - 16; a < lenght;a++)
				{
					if ((data[a] < 32) || (data[a] > 126))
						fprintf(m_file, ".");
					else
						fprintf(m_file, "%c",data[a]);

					print++;
				}

				for (unsigned int c = print; c < 16;c++)
					fprintf(m_file, " ");

				fprintf(m_file, "|\n");
			}

			countpos++;
		}
	}
	fprintf(m_file, "-------------------------------------------------------------------\n\n");
	fflush(m_file);
}

void LogSessionPacket( unsigned int direction, FILE *LogPackets, WorldPacket *packet )
{
	if( packet->size() > 0 )
		LogSessionPacket( direction, packet->GetOpcode(), packet->size(), packet->contents(), LogPackets );
}

void LogSessionPacket( unsigned int direction, FILE *LogPackets, StackWorldPacket *packet )
{
	LogSessionPacket( direction, packet->GetOpcode(), packet->GetSize(), packet->GetBufferPointer(), LogPackets );
}

#endif

int WorldSession::Update(uint32 InstanceID)
{
	m_currMsTime = getMSTime();

#ifndef CLUSTERING
	++_updatecount;
	//only update once in a while. Queued packets happen when socket buffer is full, there is no need to update queued packets in same cycle when buffer got full
//	if(((_updatecount) % 2) == 0 && _socket)
	if( _socket )
		_socket->UpdateQueuedPackets();
#endif

	WorldPacket *packet;

	//hmm, trying to update a deleted session. return 2 will remove us from their list
	if( InstanceID != session_instanceId || bDeleted == true )
	{
		// We're being updated by the wrong thread.
		// "Remove us!" - 2
		return 2;
	}

	Player *local_player = _player;	//because it seems there is a thread concurency issue here. Something is making session _player to get null. Double Login ?
	//fucked up somehow. Using relog will make player get double deleted
	//!! this might push the crash to some other location, need to make a lock regarding SetPlayer( NULL )
	if( local_player != NULL && local_player->deleted != OBJ_AVAILABLE )
		SetPlayer( NULL );

	//normal logout
	if( _logoutTime && m_currMsTime >= _logoutTime )
	{
		// Check if the player is in the process of being moved. We can't delete him
		// if we are.
		if(_player )
		{
			if( _player->m_beingPushed )
			{
				// Abort..
				return 0;
			}

			LogoutPlayer(true);
			//this is to delay socket deconnection in case player is just switching chars (char enum list after logout)
			_logoutTime = m_currMsTime + PLAYER_LOGOUT_DELAY;
			return 2;
		}

		//close socket
		Disconnect();

		//logout procedure was finished. Remove network and list bounds
		Delete( true );
	}
	// Socket disconnection.
	else if( !_socket )
	{
		// Check if the player is in the process of being moved. We can't delete him
		// if we are.
		if( _player && _player->m_beingPushed )
		{
			// Abort..
			return 0;
		}

		if( !_logoutTime )
			_logoutTime = m_currMsTime + PLAYER_LOGOUT_DELAY;
		
	}

	while ((packet = _recvQueue.Pop()) != 0)
	{
		ASSERT(packet);

		if(packet->GetOpcode() >= NUM_MSG_TYPES)
			sLog.outError("[Session] Received out of range packet with opcode 0x%.4X", packet->GetOpcode());
		else
		{
			OpcodeHandler *Handler = &WorldPacketHandlers[packet->GetOpcode()];
			if( ( Handler->status & STATUS_LOGGEDIN ) && !_player && Handler->handler != 0 )
			{
				sLog.outError("[Session] Received unexpected/wrong state packet with opcode %s (0x%.4X)",LookupName(packet->GetOpcode(), g_worldOpcodeNames), packet->GetOpcode());
			}
			else
			{
				// Valid Packet :>
				if(Handler->handler == 0)
				{
					if( _player )
						sLog.outError("[Session] Received unhandled packet with opcode %s (0x%.4X) from %s",LookupName(packet->GetOpcode(), g_worldOpcodeNames), packet->GetOpcode(), _player->GetName() );
					else 
						sLog.outError("[Session] Received unhandled packet with opcode %s (0x%.4X)",LookupName(packet->GetOpcode(), g_worldOpcodeNames), packet->GetOpcode());
				}
				//imagine 1 packet disconnects the player instantly (no idea how) and the rest in queue would crash the server
				else 
//					if( _player && ( Handler->status & STATUS_INWORLD ) && _player->IsInWorld() )
				{
#ifdef GM_COMMAND_TO_LOG_PLAYER_INCOMMING_PACKETS
					if( LogIncommingPackets != NULL )
						LogSessionPacket( 0, LogIncommingPackets, packet );
#endif

					(this->*Handler->handler)(*packet);
				}
			}
		}

		delete packet;
		packet = NULL;

		if(InstanceID != session_instanceId)
		{
			// If we hit this -> means a packet has changed our map. Should we even process rest of the packets ?
			return 2;
		}
	}

#ifdef USE_PACKET_COMPRESSION
	//flush packets that we have queued to avoid accumulating lag. This might have been called from other locations also. Calling it more then once should not make a difference
	if( _socket != NULL )
		_socket->UpdateCompressedPackets();
#endif

	//logout due to ping timeout
	if( m_lastRecvPacket + WORLDSOCKET_TIMEOUT < (uint32)UNIXTIME )
	{
		// Check if the player is in the process of being moved. We can't delete him
		// if we are.
		if( _player && _player->m_beingPushed )
		{
			// Abort..
			m_lastRecvPacket = (uint32)UNIXTIME + WORLDSOCKET_TIMEOUT;	//extend this so thread collision will have smaller chance
			return 0;
		}

		// ping timeout!
		if( _socket != NULL )
			Disconnect();

		//start the logout procedure
		if(!_logoutTime)
			_logoutTime = m_currMsTime + PLAYER_LOGOUT_DELAY;
	}

	return 0;
}

void WorldSession::SetPlayer(Player *plr)
{
	if( plr == NULL && _player != NULL && _player->only_player_gm.size() > 0 )
	{
		if(permissions)
		{
			delete [] permissions;	//in case of reload or multi load permissions
			permissions = NULL;
		}
		permissions = new char[1];
		memset(permissions, 0, 1);
		permissioncount = 0;
	}
	_player = plr; 
}

void WorldSession::LogoutPlayer(bool Save)
{
	if( _loggingOut )
		return;

	_loggingOut = true;

	if( _player != NULL )
	{
		Player *plr = _player;
		SetPlayer( NULL );
		sEventMgr.RemoveEvents( plr ); //make sure we will not issue another player delete in some other thread.
		objmgr.RemovePlayer( plr );
		plr->ok_to_remove = true;
		
		sHookInterface.OnLogout( plr );
		plr->EndDuel( DUEL_WINNER_RETREAT );

		if( plr->m_currentLoot && plr->IsInWorld() )
		{
			Object* obj = plr->GetMapMgr()->_GetObject( plr->m_currentLoot );
			if( obj != NULL )
			{
				switch( obj->GetTypeId() )
				{
				case TYPEID_UNIT:
					SafeCreatureCast( obj )->loot.looters.erase( plr->GetLowGUID() );
					break;
				case TYPEID_GAMEOBJECT:
					SafeGOCast( obj )->loot.looters.erase( plr->GetLowGUID() );
					break;
				}
			}
		}

#ifndef GM_TICKET_MY_MASTER_COMPATIBLE
		GM_Ticket * ticket = objmgr.GetGMTicketByPlayer(plr->GetGUID());
		if(ticket != NULL)
		{
			//Send status change to gm_sync_channel
			Channel *chn = channelmgr.GetChannel(sWorld.getGmClientChannel().c_str(), plr);
			if(chn)
			{
				std::stringstream ss;
				ss << "GmTicket:" << GM_TICKET_CHAT_OPCODE_ONLINESTATE;
				ss << ":" << ticket->guid;
				ss << ":0";
				chn->Say(plr, ss.str().c_str(), NULL, true);
			}
		}
#endif

		// part channels
		plr->CleanupChannels();

		if( plr->m_CurrentTransporter != NULL )
			plr->m_CurrentTransporter->RemovePlayer( plr );

		// cancel current spell
		if( plr->m_currentSpell != NULL )
			plr->m_currentSpell->safe_cancel( true );

		plr->Social_TellFriendsOffline();

		if( plr->GetTeam() == 1 )
		{
			if( sWorld.HordePlayers )
				sWorld.HordePlayers--;
		}
		else
		{
			if( sWorld.AlliancePlayers )
				sWorld.AlliancePlayers--;
		}

		if( plr->m_bg )
		{
			//at some point somehow players have this pointer set while the BG is already "deleted" 
			//not sure the problem starts here but sure as hell it will not hurt to have this extra check until i figure out crash reason
			if( BattlegroundManager.IsValidBGPointer( plr->m_bg ) )
				plr->m_bg->RemovePlayer( plr, true );
			plr->m_bg = NULL;
		}

		if( plr->m_bgIsQueued )
			BattlegroundManager.RemovePlayerFromQueues( plr );

		//Duel Cancel on Leave
		plr->EndDuel( DUEL_WINNER_RETREAT );

		//Issue a message telling all guild members that this player signed off
		if( plr->IsInGuild() )
		{
			Guild* pGuild = plr->m_playerInfo->guild;
			if( pGuild != NULL )
				pGuild->LogGuildEvent( GUILD_EVENT_HASGONEOFFLINE, GET_PLAYER_GUID( plr->m_playerInfo->guid ), 1, plr->GetName() );
		}

		plr->GetItemInterface()->EmptyBuyBack();
		
		sLfgMgr.LFGDungeonLeave( plr );
		
		// Save HP/Mana
//		plr->load_health = plr->GetUInt32Value( UNIT_FIELD_HEALTH );
//		plr->load_mana = plr->GetPower( POWER_TYPE_MANA );

		if( plr->GetSummon() != NULL )
		{
			plr->GetSummon()->Dismiss( true, true );
			plr->SetSummon( NULL );
		}

		//_player->SaveAuras();

		if( Save )
			plr->SaveToDB(false);
		
		plr->RemoveAllAuras();
		if( plr->IsInWorld() )
			plr->RemoveFromWorld();
		
		if( plr->m_playerInfo )
		{
			plr->m_playerInfo->m_loggedInPlayer = NULL;

			if( plr->m_playerInfo->m_Group != NULL )
				plr->m_playerInfo->m_Group->Update();
		}
	  
		// Remove the "player locked" flag, to allow movement on next login. Strange, we already saved the player
		plr->RemoveFlag( UNIT_FIELD_FLAGS, UNIT_FLAG_LOCK_PLAYER );

		// Save Honor Points
		//_player->SaveHonorFields();

		// Update any dirty account_data fields.
		bool dirty = false;
		if( sWorld.m_useAccountData )
		{
			std::stringstream ss;
			ss << "UPDATE account_data SET ";
			for(uint32 ui=0;ui<ACCOUNT_DATA_SIZE;ui++)
			{
				if(sAccountData[ui].bIsDirty)
				{
					if(dirty)
						ss <<",";
					ss << "uiconfig"<< ui <<"=\"";
					if(sAccountData[ui].data)
					{
						CharacterDatabase.EscapeLongString(sAccountData[ui].data, sAccountData[ui].sz, ss);
						//ss.write(sAccountData[ui].data,sAccountData[ui].sz);
					}
					ss << "\"";
					dirty = true;
					sAccountData[ui].bIsDirty = false;
				}
			}			
			if(dirty)
			{
				ss	<<" WHERE acct="<< _accountId <<";";
				CharacterDatabase.ExecuteNA(ss.str().c_str());
			}
		}

		sGarbageCollection.AddObject( plr );
		//_player = NULL;

		if( GetSocket() )
			OutPacket(SMSG_LOGOUT_COMPLETE, 0, NULL);
		sLog.outDebug( "SESSION: Sent SMSG_LOGOUT_COMPLETE Message" );
	}
	_loggingOut = false;

	SetLogoutTimer(0);
}
/*
void WorldSession::SendBuyFailed(uint64 guid, uint32 itemid, uint8 error)
{
	WorldPacket data(13);
	data.SetOpcode(SMSG_BUY_FAILED);
	data << guid << itemid << error;
	SendPacket(&data);
} */

void WorldSession::SendSellItem(uint64 vendorguid, uint64 itemid, uint8 error)
{
	WorldPacket data(17);
	data.SetOpcode(SMSG_SELL_ITEM);
	data << vendorguid << itemid << error;
	SendPacket(&data);
}

void WorldSession::LoadSecurity(std::string securitystring)
{
	std::list<char> tmp;
	bool hasa = false;
	for(uint32 i = 0; i < securitystring.length(); ++i)
	{
		char c = securitystring.at(i);
		c = tolower(c);
//		if(c == '4' || c == '3')
//			c = 'a';	// for the lazy people

		if(c == 'a')
		{
			// all permissions
			tmp.push_back('a');
			hasa = true;
		}
		else if(!hasa && (c == '0') && i == 0)
			break;
		else if(!hasa || (hasa && (c == 'z')))
		{
			tmp.push_back(c);
		}
	}

	if(permissions)
	{
		delete [] permissions;	//in case of reload or multi load permissions
		permissions = NULL;
	}
	permissions = new char[tmp.size()+1];
	memset(permissions, 0, tmp.size()+1);
	permissioncount = (uint32)tmp.size();
	int k = 0;
	for(std::list<char>::iterator itr = tmp.begin(); itr != tmp.end(); ++itr)
		permissions[k++] = (*itr);
	
	if(permissions[tmp.size()] != 0)
		permissions[tmp.size()] = 0;

	sLog.outDebug("Loaded permissions for %u. (%u) : [%s]", this->GetAccountId(), permissioncount, securitystring.c_str());
}

void WorldSession::SetSecurity(std::string securitystring)
{
	if( permissions )
	{
		delete [] permissions;
		permissions = NULL;
	}
	LoadSecurity(securitystring);

	// update db
	CharacterDatabase.Execute("UPDATE accounts SET gm=\"%s\" WHERE acct=%u", CharacterDatabase.EscapeString(string(permissions)).c_str(), _accountId);
}

bool WorldSession::CanUseCommand(char cmdstr)
{
#ifdef ANYONE_CAN_USE_ALL_COMMANDS
	permissions=new char[2];
	permissions[0]='a';
	permissions[1]='z';
	permissioncount=2;
	return true;
#endif
	if(permissioncount == 0)
		return false;
	if(cmdstr == 0)
		return true;
	if(permissions[0] == 'a' && cmdstr != 'z')   // all
		return true;

	for(int i = 0; i < permissioncount; ++i)
		if(permissions[i] == cmdstr)
			return true;

	return false;
}

void WorldSession::SendNotification(const char *message, ...)
{
	if( !message ) return;
	va_list ap;
	va_start(ap, message);
	char msg1[1024];
	vsnprintf(msg1,1024, message,ap);
	WorldPacket data(SMSG_NOTIFICATION, strlen(msg1) + 1);
	data << msg1;
	SendPacket(&data);
}

void WorldSession::InitPacketHandlerTable()
{
	// Nullify Everything, default to STATUS_LOGGEDIN
	for(uint32 i = 0; i < NUM_MSG_TYPES; ++i)
	{
//		WorldPacketHandlers[i].status = STATUS_LOGGEDIN | STATUS_INWORLD;
		WorldPacketHandlers[i].status = STATUS_LOGGEDIN ;
		WorldPacketHandlers[i].handler = 0;
	}
	// Login
	WorldPacketHandlers[CMSG_CHAR_ENUM].handler								 = &WorldSession::HandleCharEnumOpcode;
	WorldPacketHandlers[CMSG_CHAR_ENUM].status								  = STATUS_AUTHED;

	WorldPacketHandlers[CMSG_CHAR_CREATE].handler							   = &WorldSession::HandleCharCreateOpcode;
	WorldPacketHandlers[CMSG_CHAR_CREATE].status								= STATUS_AUTHED;

	WorldPacketHandlers[CMSG_CHAR_DELETE].handler							   = &WorldSession::HandleCharDeleteOpcode;
	WorldPacketHandlers[CMSG_CHAR_DELETE].status								= STATUS_AUTHED;

	WorldPacketHandlers[CMSG_CHAR_RENAME].handler							   = &WorldSession::HandleCharRenameOpcode;
	WorldPacketHandlers[CMSG_CHAR_RENAME].status								= STATUS_AUTHED;

	WorldPacketHandlers[CMSG_PLAYER_LOGIN].handler							  = &WorldSession::HandlePlayerLoginOpcode; 
	WorldPacketHandlers[CMSG_PLAYER_LOGIN].status							   = STATUS_AUTHED;

	WorldPacketHandlers[CMSG_REALM_SPLIT].handler							  = &WorldSession::HandleRealmStateRequestOpcode;
	WorldPacketHandlers[CMSG_REALM_SPLIT].status							   = STATUS_AUTHED;


	// Queries
	WorldPacketHandlers[MSG_CORPSE_QUERY].handler							   = &WorldSession::HandleCorpseQueryOpcode;
	WorldPacketHandlers[CMSG_CORPSE_MAP_POSITION_QUERY].handler		  	   = &WorldSession::HandleCorpseQueryMapOpcode;
	WorldPacketHandlers[CMSG_NAME_QUERY].handler								= &WorldSession::HandleNameQueryOpcode;
	WorldPacketHandlers[CMSG_QUERY_TIME].handler								= &WorldSession::HandleQueryTimeOpcode;
	WorldPacketHandlers[CMSG_WORLD_STATE_UI_TIMER_UPDATE].handler			= &WorldSession::HandleWorldStateUITimerUpdate;
	WorldPacketHandlers[CMSG_CREATURE_QUERY].handler							= &WorldSession::HandleCreatureQueryOpcode;
	WorldPacketHandlers[CMSG_GAMEOBJECT_QUERY].handler						  = &WorldSession::HandleGameObjectQueryOpcode;
	WorldPacketHandlers[CMSG_PAGE_TEXT_QUERY].handler						   = &WorldSession::HandlePageTextQueryOpcode;
	WorldPacketHandlers[CMSG_ITEM_NAME_QUERY].handler						   = &WorldSession::HandleItemNameQueryOpcode;
	WorldPacketHandlers[CMSG_QUERY_INSPECT_ACHIEVEMENTS].handler			   = &WorldSession::HandleAchievmentQueryOpcode;
	WorldPacketHandlers[CMSG_GET_MIRRORIMAGE_DATA].handler						= &WorldSession::HandleMirrorImageQueryOpcode;

	// Movement
	WorldPacketHandlers[MSG_MOVE_HEARTBEAT].handler							 = &WorldSession::HandleMovementOpcodes;
	WorldPacketHandlers[CMSG_MOVE_WORLDPORT_ACK].handler						 = &WorldSession::HandleMoveWorldportAckOpcode;
	WorldPacketHandlers[CMSG_LOADING_SCREEN_NOTIFY].handler						 = &WorldSession::HandleLoadingScreenStatusUpdate;
	WorldPacketHandlers[MSG_MOVE_JUMP].handler								  = &WorldSession::HandleMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_START_ASCEND].handler							= &WorldSession::HandleMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_STOP_ASCEND].handler					= &WorldSession::HandleMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_START_FORWARD].handler						 = &WorldSession::HandleMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_START_BACKWARD].handler						= &WorldSession::HandleMovementOpcodes;
    WorldPacketHandlers[MSG_MOVE_SET_FACING].handler                            = &WorldSession::HandleMovementOpcodes;
    WorldPacketHandlers[MSG_MOVE_START_STRAFE_LEFT].handler					 = &WorldSession::HandleMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_START_STRAFE_RIGHT].handler					= &WorldSession::HandleMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_STOP_STRAFE].handler						   = &WorldSession::HandleMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_START_TURN_LEFT].handler					   = &WorldSession::HandleMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_START_TURN_RIGHT].handler					  = &WorldSession::HandleMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_STOP_TURN].handler							 = &WorldSession::HandleMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_START_PITCH_UP].handler						= &WorldSession::HandleMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_START_PITCH_DOWN].handler					  = &WorldSession::HandleMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_STOP_PITCH].handler							= &WorldSession::HandleMovementOpcodes;
//	WorldPacketHandlers[MSG_MOVE_SET_RUN_MODE].handler						  = &WorldSession::HandleMovementOpcodes;
//	WorldPacketHandlers[MSG_MOVE_SET_WALK_MODE].handler						 = &WorldSession::HandleMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_SET_PITCH].handler							 = &WorldSession::HandleMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_START_SWIM].handler							= &WorldSession::HandleMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_STOP_SWIM].handler							 = &WorldSession::HandleMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_JUMP_STOP_SWIM].handler						= &WorldSession::HandleMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_FALL_LAND].handler							 = &WorldSession::HandleMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_STOP].handler								  = &WorldSession::HandleMovementOpcodes;
	WorldPacketHandlers[CMSG_MOVE_SET_FLY].handler					= &WorldSession::HandleMovementOpcodes;
	WorldPacketHandlers[MSG_MOVE_STOP_ASCEND].handler				   = &WorldSession::HandleMovementOpcodes;
	WorldPacketHandlers[CMSG_MOVE_TIME_SKIPPED].handler						 = &WorldSession::HandleMoveTimeSkippedOpcode;
	WorldPacketHandlers[CMSG_MOVE_NOT_ACTIVE_MOVER].handler					 = &WorldSession::HandleMoveNotActiveMoverOpcode;
	WorldPacketHandlers[CMSG_SET_ACTIVE_MOVER].handler						  = &WorldSession::HandleSetActiveMoverOpcode;
    WorldPacketHandlers[CMSG_MOVE_CHNG_TRANSPORT].handler                         = &WorldSession::HandleMovementOpcodes;
	// ACK
	WorldPacketHandlers[CMSG_MOVE_TELEPORT_ACK].handler						  = &WorldSession::HandleMoveTeleportAckOpcode;
	WorldPacketHandlers[CMSG_FORCE_WALK_SPEED_CHANGE_ACK].handler			   = &WorldSession::HandleAcknowledgementOpcodes;
	WorldPacketHandlers[CMSG_MOVE_FEATHER_FALL_ACK].handler					 = &WorldSession::HandleAcknowledgementOpcodes;
	WorldPacketHandlers[CMSG_MOVE_WATER_WALK_ACK].handler					   = &WorldSession::HandleAcknowledgementOpcodes;
	WorldPacketHandlers[CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK].handler		  = &WorldSession::HandleAcknowledgementOpcodes;
	WorldPacketHandlers[CMSG_FORCE_TURN_RATE_CHANGE_ACK].handler				= &WorldSession::HandleAcknowledgementOpcodes;
	WorldPacketHandlers[CMSG_FORCE_RUN_SPEED_CHANGE_ACK].handler				= &WorldSession::HandleAcknowledgementOpcodes;
	WorldPacketHandlers[CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK].handler		   = &WorldSession::HandleAcknowledgementOpcodes;
	WorldPacketHandlers[CMSG_FORCE_SWIM_SPEED_CHANGE_ACK].handler			   = &WorldSession::HandleAcknowledgementOpcodes;
	WorldPacketHandlers[CMSG_FORCE_MOVE_ROOT_ACK].handler					   = &WorldSession::HandleAcknowledgementOpcodes;
	WorldPacketHandlers[CMSG_FORCE_MOVE_UNROOT_ACK].handler					   = &WorldSession::HandleAcknowledgementOpcodes;
	WorldPacketHandlers[CMSG_MOVE_KNOCK_BACK_ACK].handler					   = &WorldSession::HandleAcknowledgementOpcodes;
	WorldPacketHandlers[CMSG_MOVE_HOVER_ACK].handler							= &WorldSession::HandleAcknowledgementOpcodes;
	WorldPacketHandlers[CMSG_MOVE_FLY_CHANGE_ACK].handler							= &WorldSession::HandleAcknowledgementOpcodes;
	WorldPacketHandlers[MSG_MOVE_START_DESCEND].handler							= &WorldSession::HandleMovementOpcodes;
	
	// Action Buttons
	WorldPacketHandlers[CMSG_SET_ACTION_BUTTON].handler						 = &WorldSession::HandleSetActionButtonOpcode;
	WorldPacketHandlers[CMSG_REPOP_REQUEST].handler							 = &WorldSession::HandleRepopRequestOpcode;
	WorldPacketHandlers[CMSG_TEL_TO_GRAVEYARD].handler						 = &WorldSession::HandleTelToGraveyardOpcode;
		
	// Loot
	WorldPacketHandlers[CMSG_AUTOSTORE_LOOT_ITEM].handler					   = &WorldSession::HandleAutostoreLootItemOpcode;
	WorldPacketHandlers[CMSG_LOOT_CURRENCY].handler							 = &WorldSession::HandleAutostoreLootCurrencyOpcode;
	WorldPacketHandlers[CMSG_LOOT_MONEY].handler								= &WorldSession::HandleLootMoneyOpcode;
	WorldPacketHandlers[CMSG_LOOT].handler									  = &WorldSession::HandleLootOpcode;
	WorldPacketHandlers[CMSG_LOOT_RELEASE].handler							  = &WorldSession::HandleLootReleaseOpcode;
	WorldPacketHandlers[CMSG_LOOT_ROLL].handler								 = &WorldSession::HandleLootRollOpcode;
	WorldPacketHandlers[CMSG_LOOT_MASTER_GIVE].handler						  = &WorldSession::HandleLootMasterGiveOpcode;
	WorldPacketHandlers[CMSG_OPT_OUT_OF_LOOT].handler						= &WorldSession::HandleSetAutoLootPassOpcode;
	
	// Player Interaction
	WorldPacketHandlers[CMSG_WHO].handler									   = &WorldSession::HandleWhoOpcode;
	WorldPacketHandlers[CMSG_LOGOUT_REQUEST].handler							= &WorldSession::HandleLogoutRequestOpcode;
	WorldPacketHandlers[CMSG_PLAYER_LOGOUT].handler							 = &WorldSession::HandlePlayerLogoutOpcode;
	WorldPacketHandlers[CMSG_LOGOUT_CANCEL].handler							 = &WorldSession::HandleLogoutCancelOpcode;
	WorldPacketHandlers[CMSG_ZONEUPDATE].handler								= &WorldSession::HandleZoneUpdateOpcode;
//	WorldPacketHandlers[CMSG_SET_TARGET_OBSOLETE].handler					   = &WorldSession::HandleSetTargetOpcode;
	WorldPacketHandlers[CMSG_SET_SELECTION].handler							 = &WorldSession::HandleSetSelectionOpcode;
	WorldPacketHandlers[CMSG_STANDSTATECHANGE].handler						  = &WorldSession::HandleStandStateChangeOpcode;
	WorldPacketHandlers[CMSG_CANCEL_MOUNT_AURA].handler								= &WorldSession::HandleDismountOpcode;
	
	// Friends
	WorldPacketHandlers[CMSG_CONTACT_LIST].handler							   = &WorldSession::HandleFriendListOpcode;
	WorldPacketHandlers[CMSG_ADD_FRIEND].handler								= &WorldSession::HandleAddFriendOpcode;
	WorldPacketHandlers[CMSG_DEL_FRIEND].handler								= &WorldSession::HandleDelFriendOpcode;
	WorldPacketHandlers[CMSG_ADD_IGNORE].handler								= &WorldSession::HandleAddIgnoreOpcode;
	WorldPacketHandlers[CMSG_DEL_IGNORE].handler								= &WorldSession::HandleDelIgnoreOpcode;
	WorldPacketHandlers[CMSG_BUG].handler									   = &WorldSession::HandleBugOpcode;
	WorldPacketHandlers[CMSG_SET_CONTACT_NOTES].handler							= &WorldSession::HandleSetFriendNote;
	
	// Areatrigger
	WorldPacketHandlers[CMSG_AREATRIGGER].handler							   = &WorldSession::HandleAreaTriggerOpcode;
	
	// Account Data
	WorldPacketHandlers[CMSG_UPDATE_ACCOUNT_DATA].handler						= &WorldSession::HandleUpdateAccountData;
	WorldPacketHandlers[CMSG_REQUEST_ACCOUNT_DATA].handler						= &WorldSession::HandleRequestAccountData;
	WorldPacketHandlers[CMSG_TOGGLE_PVP].handler								= &WorldSession::HandleTogglePVPOpcode;
	
	// Faction / Reputation
	WorldPacketHandlers[CMSG_SET_FACTION_ATWAR].handler							= &WorldSession::HandleSetAtWarOpcode;
	WorldPacketHandlers[CMSG_SET_WATCHED_FACTION].handler					= &WorldSession::HandleSetWatchedFactionIndexOpcode;
	WorldPacketHandlers[CMSG_SET_FACTION_INACTIVE].handler						= &WorldSession::HandleSetFactionInactiveOpcode;
	
	// Player Interaction
//	WorldPacketHandlers[CMSG_GAMEOBJ_USE].handler							   = &WorldSession::HandleGameObjectUse;
	//maybe the same ? The issue is that some GOs will use both packets at the same time making a dual triggering of the event
	WorldPacketHandlers[CMSG_GAMEOBJ_REPORT_USE].handler							   = &WorldSession::HandleGameObjectUse;
	WorldPacketHandlers[CMSG_PLAYED_TIME].handler								= &WorldSession::HandlePlayedTimeOpcode;
	WorldPacketHandlers[CMSG_SETSHEATHED].handler								= &WorldSession::HandleSetSheathedOpcode;
	WorldPacketHandlers[CMSG_CHAT_MSG_SAY].handler								= &WorldSession::HandleMessagechatOpcode;
	WorldPacketHandlers[CMSG_CHAT_MSG_YELL].handler								= &WorldSession::HandleMessagechatOpcode;
	WorldPacketHandlers[CMSG_CHAT_MSG_CHANNEL_SAY].handler						= &WorldSession::HandleMessagechatOpcode;
	WorldPacketHandlers[CMSG_CHAT_MSG_GUILD].handler							= &WorldSession::HandleMessagechatOpcode;
	WorldPacketHandlers[CMSG_CHAT_MSG_WISPER].handler							= &WorldSession::HandleMessagechatOpcode;
	WorldPacketHandlers[CMSG_CHAT_MSG_AFK].handler								= &WorldSession::HandleMessagechatOpcode;
	WorldPacketHandlers[CMSG_CHAT_MSG_DND].handler								= &WorldSession::HandleMessagechatOpcode;
	WorldPacketHandlers[CMSG_CHAT_MSG_OFFICER].handler							= &WorldSession::HandleMessagechatOpcode;
	WorldPacketHandlers[CMSG_CHAT_MSG_PARTY].handler							= &WorldSession::HandleMessagechatOpcode;
	WorldPacketHandlers[CMSG_CHAT_MSG_PARTY_LEADER].handler						= &WorldSession::HandleMessagechatOpcode;
	WorldPacketHandlers[CMSG_CHAT_MSG_RAID].handler								= &WorldSession::HandleMessagechatOpcode;
	WorldPacketHandlers[CMSG_CHAT_MSG_RAID_LEADER].handler						= &WorldSession::HandleMessagechatOpcode;
	WorldPacketHandlers[CMSG_CHAT_MSG_BATTLEGROUND].handler						= &WorldSession::HandleMessagechatOpcode;
	WorldPacketHandlers[CMSG_CHAT_MSG_BATTLEGROUND_LEADER].handler				= &WorldSession::HandleMessagechatOpcode;
	WorldPacketHandlers[CMSG_CHAT_MSG_RAID_WARNING].handler						= &WorldSession::HandleMessagechatOpcode;
	WorldPacketHandlers[CMSG_CHAT_MSG_EMOTE].handler							= &WorldSession::HandleMessagechatOpcode;
	WorldPacketHandlers[CMSG_TEXT_EMOTE].handler								= &WorldSession::HandleTextEmoteOpcode;
	WorldPacketHandlers[CMSG_CLEAR_EMOTE_STATE].handler							= &WorldSession::HandleClearEmoteOpcode;
	WorldPacketHandlers[CMSG_INSPECT].handler									= &WorldSession::HandleInspectOpcode;
	WorldPacketHandlers[CMSG_ALTER_APPEARANCE].handler							= &WorldSession::HandleBarberShopResult;
	
	// Channels
	WorldPacketHandlers[CMSG_JOIN_CHANNEL].handler							  = &WorldSession::HandleChannelJoin;
	WorldPacketHandlers[CMSG_LEAVE_CHANNEL].handler							 = &WorldSession::HandleChannelLeave;
	WorldPacketHandlers[CMSG_CHANNEL_LIST].handler							  = &WorldSession::HandleChannelList;
	WorldPacketHandlers[CMSG_CHANNEL_PASSWORD].handler						  = &WorldSession::HandleChannelPassword;
	WorldPacketHandlers[CMSG_CHANNEL_SET_OWNER].handler						 = &WorldSession::HandleChannelSetOwner;
	WorldPacketHandlers[CMSG_CHANNEL_OWNER].handler							 = &WorldSession::HandleChannelOwner;
	WorldPacketHandlers[CMSG_CHANNEL_MODERATOR].handler						 = &WorldSession::HandleChannelModerator;
	WorldPacketHandlers[CMSG_CHANNEL_UNMODERATOR].handler					   = &WorldSession::HandleChannelUnmoderator;
	WorldPacketHandlers[CMSG_CHANNEL_MUTE].handler							  = &WorldSession::HandleChannelMute;
	WorldPacketHandlers[CMSG_CHANNEL_UNMUTE].handler							= &WorldSession::HandleChannelUnmute;
	WorldPacketHandlers[CMSG_CHANNEL_INVITE].handler							= &WorldSession::HandleChannelInvite;
	WorldPacketHandlers[CMSG_CHANNEL_KICK].handler							  = &WorldSession::HandleChannelKick;
	WorldPacketHandlers[CMSG_CHANNEL_BAN].handler							   = &WorldSession::HandleChannelBan;
	WorldPacketHandlers[CMSG_CHANNEL_UNBAN].handler							 = &WorldSession::HandleChannelUnban;
	WorldPacketHandlers[CMSG_CHANNEL_ANNOUNCEMENTS].handler					 = &WorldSession::HandleChannelAnnounce;
	WorldPacketHandlers[CMSG_CHANNEL_MODERATE].handler						  = &WorldSession::HandleChannelModerate;
	WorldPacketHandlers[CMSG_GET_CHANNEL_MEMBER_COUNT].handler					= &WorldSession::HandleChannelNumMembersQuery;
	WorldPacketHandlers[CMSG_CHANNEL_DISPLAY_LIST].handler					= &WorldSession::HandleChannelRosterQuery;
	
	// Groups / Raids
	WorldPacketHandlers[CMSG_GROUP_INVITE].handler							  = &WorldSession::HandleGroupInviteOpcode;
	WorldPacketHandlers[CMSG_GROUP_CANCEL].handler							  = &WorldSession::HandleGroupCancelOpcode;
	WorldPacketHandlers[CMSG_GROUP_INVITE_REPLY].handler					  = &WorldSession::HandleGroupAcceptOpcode;
	WorldPacketHandlers[CMSG_GROUP_DECLINE].handler							 = &WorldSession::HandleGroupDeclineOpcode;
	WorldPacketHandlers[CMSG_GROUP_UNINVITE].handler							= &WorldSession::HandleGroupUninviteOpcode;
	WorldPacketHandlers[CMSG_GROUP_UNINVITE_GUID].handler					   = &WorldSession::HandleGroupUninviteGUIDOpcode;
	WorldPacketHandlers[CMSG_GROUP_SET_LEADER].handler						  = &WorldSession::HandleGroupSetLeaderOpcode;
	WorldPacketHandlers[CMSG_GROUP_DISBAND].handler							 = &WorldSession::HandleGroupDisbandOpcode;
	WorldPacketHandlers[CMSG_LOOT_METHOD].handler							   = &WorldSession::HandleLootMethodOpcode;
	WorldPacketHandlers[MSG_MINIMAP_PING].handler							   = &WorldSession::HandleMinimapPingOpcode;
	WorldPacketHandlers[CMSG_GROUP_RAID_CONVERT].handler						= &WorldSession::HandleConvertGroupToRaidOpcode;
	WorldPacketHandlers[CMSG_GROUP_CHANGE_SUB_GROUP].handler					= &WorldSession::HandleGroupChangeSubGroup;
	WorldPacketHandlers[CMSG_GROUP_ASSISTANT_LEADER].handler					= &WorldSession::HandleGroupAssistantLeader;
	WorldPacketHandlers[CMSG_REQUEST_RAID_INFO].handler						 = &WorldSession::HandleRequestRaidInfoOpcode;
	WorldPacketHandlers[MSG_RAID_READY_CHECK].handler						   = &WorldSession::HandleReadyCheckOpcode;
	WorldPacketHandlers[MSG_RAID_TARGET_UPDATE].handler					  = &WorldSession::HandleSetPlayerIconOpcode;
	WorldPacketHandlers[CMSG_REQUEST_PARTY_MEMBER_STATS].handler				= &WorldSession::HandlePartyMemberStatsOpcode;
	WorldPacketHandlers[MSG_PARTY_ASSIGNMENT].handler								= &WorldSession::HandleGroupPromote;
	WorldPacketHandlers[MSG_GROUP_SET_ROLE].handler							= &WorldSession::HandleSetPlayerRole;
	WorldPacketHandlers[CMSG_RAID_MARKER_CLEAR].handler							= &WorldSession::HandleRaidMarkerClear;
	WorldPacketHandlers[CMSG_RAID_UNK_STATUS_QUERY].handler							= &WorldSession::HandleRaidUnkQuery;
	WorldPacketHandlers[CMSG_GROUP_ROLE_CHECK].handler						   = &WorldSession::HandleRoleCheckOpcode;

	// LFG System
	/*
	WorldPacketHandlers[CMSG_SET_LFG_COMMENT].handler							= &WorldSession::HandleSetLookingForGroupComment;
	WorldPacketHandlers[MSG_LOOKING_FOR_GROUP].handler							= &WorldSession::HandleMsgLookingForGroup;
	WorldPacketHandlers[CMSG_SET_LOOKING_FOR_GROUP].handler						= &WorldSession::HandleSetLookingForGroup;
//	WorldPacketHandlers[CMSG_SET_LOOKING_FOR_MORE].handler						= &WorldSession::HandleSetLookingForMore;
	WorldPacketHandlers[CMSG_SET_LFG_COMMENT].handler							= &WorldSession::HandleEnableAutoJoin;
//	WorldPacketHandlers[CMSG_LFG_CLEAR_AUTOJOIN].handler							= &WorldSession::HandleDisableAutoJoin;
//	WorldPacketHandlers[CMSG_LFM_SET_AUTOFILL].handler					= &WorldSession::HandleEnableAutoAddMembers;
//	WorldPacketHandlers[CMSG_LFM_SET_AUTOFILL].handler					= &WorldSession::HandleDisableAutoAddMembers;
//	WorldPacketHandlers[CMSG_CLEAR_LOOKING_FOR_GROUP].handler				= &WorldSession::HandleLfgClear; */
	WorldPacketHandlers[CMSG_LFG_JOIN].handler								= &WorldSession::HandleLFGJoin;
	WorldPacketHandlers[CMSG_LFG_LEAVE].handler								= &WorldSession::HandleLFGLeave;
	WorldPacketHandlers[CMSG_LFG_PROPOSAL_RESULT].handler					= &WorldSession::HandleLFGAnswerJoin;
	WorldPacketHandlers[CMSG_LFG_SET_ROLES].handler							= &WorldSession::HandleLFGSetFilterRoles;
	WorldPacketHandlers[CMSG_SET_LFG_COMMENT].handler						= &WorldSession::HandleLFGSetComment;
	WorldPacketHandlers[CMSG_LFG_SET_BOOT_VOTE].handler						= &WorldSession::HandleLFGKickVote;
	WorldPacketHandlers[CMSG_LFG_TELEPORT].handler							= &WorldSession::HandleLFGTeleport;
	WorldPacketHandlers[CMSG_LFG_PLAYER_LOCK_INFO_REQUEST].handler			= &WorldSession::HandleLFGPlayerListRequest;
	WorldPacketHandlers[CMSG_LFD_PARTY_LOCK_INFO_REQUEST].handler			= &WorldSession::HandleLFGGroupListRequest;
	WorldPacketHandlers[CMSG_SEARCH_LFG_JOIN].handler						= &WorldSession::HandleLFGQueueInstanceJoin;
	WorldPacketHandlers[CMSG_SEARCH_LFG_LEAVE].handler						= &WorldSession::HandleLFGQueueInstanceLeave;
	WorldPacketHandlers[CMSG_LFG_STATUS_QUERY].handler						= &WorldSession::HandleLFGStatusQuery;
	
	// Taxi / NPC Interaction
	WorldPacketHandlers[CMSG_TAXINODE_STATUS_QUERY].handler					 = &WorldSession::HandleTaxiNodeStatusQueryOpcode;
	WorldPacketHandlers[CMSG_ENABLETAXI].handler							 = &WorldSession::HandleTaxiEnableOpcode;
	WorldPacketHandlers[CMSG_TAXIQUERYAVAILABLENODES].handler				   = &WorldSession::HandleTaxiQueryAvaibleNodesOpcode;
	WorldPacketHandlers[CMSG_ACTIVATETAXI].handler							  = &WorldSession::HandleActivateTaxiOpcode;
	WorldPacketHandlers[MSG_TABARDVENDOR_ACTIVATE].handler					  = &WorldSession::HandleTabardVendorActivateOpcode;
	WorldPacketHandlers[CMSG_BANKER_ACTIVATE].handler						   = &WorldSession::HandleBankerActivateOpcode;
	WorldPacketHandlers[CMSG_BUY_BANK_SLOT].handler							 = &WorldSession::HandleBuyBankSlotOpcode;
	WorldPacketHandlers[CMSG_TRAINER_LIST].handler							  = &WorldSession::HandleTrainerListOpcode;
	WorldPacketHandlers[CMSG_TRAINER_BUY_SPELL].handler						 = &WorldSession::HandleTrainerBuySpellOpcode;
	WorldPacketHandlers[CMSG_PETITION_SHOWLIST].handler						 = &WorldSession::HandleCharterShowListOpcode;
	WorldPacketHandlers[MSG_AUCTION_HELLO].handler							  = &WorldSession::HandleAuctionHelloOpcode;
	WorldPacketHandlers[CMSG_GOSSIP_HELLO].handler							  = &WorldSession::HandleGossipHelloOpcode;
	WorldPacketHandlers[CMSG_GOSSIP_SELECT_OPTION].handler					  = &WorldSession::HandleGossipSelectOptionOpcode;
	WorldPacketHandlers[CMSG_SPIRIT_HEALER_ACTIVATE].handler					= &WorldSession::HandleSpiritHealerActivateOpcode;
	WorldPacketHandlers[CMSG_NPC_TEXT_QUERY].handler							= &WorldSession::HandleNpcTextQueryOpcode;
	WorldPacketHandlers[CMSG_BINDER_ACTIVATE].handler						   = &WorldSession::HandleBinderActivateOpcode;
//	WorldPacketHandlers[CMSG_CLEAR_LOOKING_FOR_GROUP].handler					= &WorldSession::HandleMultipleActivateTaxiOpcode;
	
	// Item / Vendors
	WorldPacketHandlers[CMSG_SWAP_INV_ITEM].handler							 = &WorldSession::HandleSwapInvItemOpcode;
	WorldPacketHandlers[CMSG_SWAP_INV_ITEM_EQUIPPED].handler				 = &WorldSession::HandleSwapInvItemOpcode;
	WorldPacketHandlers[CMSG_SWAP_ITEM].handler								 = &WorldSession::HandleSwapItemOpcode;
	WorldPacketHandlers[CMSG_DESTROYITEM].handler							   = &WorldSession::HandleDestroyItemOpcode;
	WorldPacketHandlers[CMSG_AUTOEQUIP_ITEM].handler							= &WorldSession::HandleAutoEquipItemOpcode;
	WorldPacketHandlers[CMSG_ITEM_QUERY_SINGLE].handler						 = &WorldSession::HandleItemQuerySingleOpcode;
	WorldPacketHandlers[CMSG_ITEM_QUERY_MULTIPLE].handler					 = &WorldSession::HandleItemQueryMultipleOpcode;
	WorldPacketHandlers[CMSG_SELL_ITEM].handler								 = &WorldSession::HandleSellItemOpcode;
	WorldPacketHandlers[CMSG_BUY_ITEM_IN_SLOT].handler						  = &WorldSession::HandleBuyItemInSlotOpcode;
//	WorldPacketHandlers[CMSG_BUY_ITEM].handler								  = &WorldSession::HandleBuyItemOpcode;	//depracated ?
	WorldPacketHandlers[CMSG_LIST_INVENTORY].handler							= &WorldSession::HandleListInventoryOpcode;
	WorldPacketHandlers[CMSG_AUTOSTORE_BAG_ITEM].handler						= &WorldSession::HandleAutoStoreBagItemOpcode;
	WorldPacketHandlers[CMSG_SET_AMMO].handler								  = &WorldSession::HandleAmmoSetOpcode;
	WorldPacketHandlers[CMSG_BUYBACK_ITEM].handler							  = &WorldSession::HandleBuyBackOpcode;
	WorldPacketHandlers[CMSG_SPLIT_ITEM].handler								= &WorldSession::HandleSplitOpcode;
	WorldPacketHandlers[CMSG_READ_ITEM].handler								 = &WorldSession::HandleReadItemOpcode;
	WorldPacketHandlers[CMSG_REPAIR_ITEM].handler							   = &WorldSession::HandleRepairItemOpcode;
	WorldPacketHandlers[CMSG_AUTOBANK_ITEM].handler							 = &WorldSession::HandleAutoBankItemOpcode;
	WorldPacketHandlers[CMSG_AUTOSTORE_BANK_ITEM].handler					   = &WorldSession::HandleAutoStoreBankItemOpcode;
	WorldPacketHandlers[CMSG_CANCEL_TEMP_ENCHANTMENT].handler			  = &WorldSession::HandleCancelTemporaryEnchantmentOpcode;
	WorldPacketHandlers[CMSG_SOCKET_GEMS].handler								= &WorldSession::HandleInsertGemOpcode;
	WorldPacketHandlers[CMSG_WRAP_ITEM].handler									= &WorldSession::HandleWrapItemOpcode;
	WorldPacketHandlers[CMSG_EQUIPMENT_SET_SAVE].handler					= &WorldSession::HandleEquipmentSetSave;
	WorldPacketHandlers[CMSG_EQUIPMENT_SET_DELETE].handler					= &WorldSession::HandleEquipmentSetDelete;
	WorldPacketHandlers[CMSG_EQUIPMENT_SET_USE].handler						= &WorldSession::HandleEquipmentSetUse;
	WorldPacketHandlers[CMSG_ITEM_REFUND_INFO_REQUEST].handler				= &WorldSession::HandleItemRefundInfoRequest;
	WorldPacketHandlers[CMSG_ITEM_REFUND].handler							= &WorldSession::HandleItemRefund;
	WorldPacketHandlers[CMSG_REFORGING_REFORGE].handler						= &WorldSession::HandleReforgeItem;
	
	// Spell System / Talent System
	WorldPacketHandlers[CMSG_USE_ITEM].handler								  = &WorldSession::HandleUseItemOpcode;
	WorldPacketHandlers[CMSG_CAST_SPELL].handler								= &WorldSession::HandleCastSpellOpcode;
	WorldPacketHandlers[CMSG_SPELLCLICK].handler								  = &WorldSession::HandleSpellClick;
	WorldPacketHandlers[CMSG_CANCEL_CAST].handler							   = &WorldSession::HandleCancelCastOpcode;
	WorldPacketHandlers[CMSG_CANCEL_AURA].handler							   = &WorldSession::HandleCancelAuraOpcode;
	WorldPacketHandlers[CMSG_CANCEL_CHANNELLING].handler						= &WorldSession::HandleCancelChannellingOpcode;
	WorldPacketHandlers[CMSG_CANCEL_AUTO_REPEAT_SPELL].handler				  = &WorldSession::HandleCancelAutoRepeatSpellOpcode;
	WorldPacketHandlers[CMSG_LEARN_TALENT].handler							  = &WorldSession::HandleLearnTalentOpcode;
	WorldPacketHandlers[CMSG_LEARN_PREVIEW_TALENTS].handler					  = &WorldSession::HandleLearnPreviewTalentOpcode;
	WorldPacketHandlers[CMSG_UNLEARN_TALENTS].handler						   = &WorldSession::HandleUnlearnTalents;
	WorldPacketHandlers[MSG_TALENT_WIPE_CONFIRM].handler						= &WorldSession::HandleUnlearnTalents;
	WorldPacketHandlers[CMSG_REMOVE_GLYPH].handler								= &WorldSession::HandleGlyphRemove;
	
	// Combat / Duel
	WorldPacketHandlers[CMSG_ATTACKSWING].handler							   = &WorldSession::HandleAttackSwingOpcode;
	WorldPacketHandlers[CMSG_ATTACKSTOP].handler								= &WorldSession::HandleAttackStopOpcode;
	WorldPacketHandlers[CMSG_DUEL_ACCEPTED].handler							 = &WorldSession::HandleDuelAccepted;
	WorldPacketHandlers[CMSG_DUEL_CANCELLED].handler							= &WorldSession::HandleDuelCancelled;
	
	// Trade
	WorldPacketHandlers[CMSG_INITIATE_TRADE].handler							= &WorldSession::HandleInitiateTrade;
	WorldPacketHandlers[CMSG_INITIATE_TRADE_ACK].handler					= &WorldSession::HandleInitiateTradeACK;
	WorldPacketHandlers[CMSG_BEGIN_TRADE].handler							   = &WorldSession::HandleBeginTrade;
//	WorldPacketHandlers[CMSG_BUSY_TRADE].handler								= &WorldSession::HandleBusyTrade;
	WorldPacketHandlers[CMSG_IGNORE_TRADE].handler							  = &WorldSession::HandleIgnoreTrade;
	WorldPacketHandlers[CMSG_ACCEPT_TRADE].handler							  = &WorldSession::HandleAcceptTrade;
	WorldPacketHandlers[CMSG_UNACCEPT_TRADE].handler							= &WorldSession::HandleUnacceptTrade;
	WorldPacketHandlers[CMSG_CANCEL_TRADE].handler							  = &WorldSession::HandleCancelTrade;
	WorldPacketHandlers[CMSG_SET_TRADE_ITEM].handler							= &WorldSession::HandleSetTradeItem;
	WorldPacketHandlers[CMSG_CLEAR_TRADE_ITEM].handler						  = &WorldSession::HandleClearTradeItem;
	WorldPacketHandlers[CMSG_SET_TRADE_GOLD].handler							= &WorldSession::HandleSetTradeGold;
	
	// Quest System
	WorldPacketHandlers[CMSG_QUESTGIVER_STATUS_QUERY].handler				   = &WorldSession::HandleQuestgiverStatusQueryOpcode;
	WorldPacketHandlers[CMSG_QUESTGIVER_HELLO].handler						  = &WorldSession::HandleQuestgiverHelloOpcode;
	WorldPacketHandlers[CMSG_QUESTGIVER_ACCEPT_QUEST].handler				   = &WorldSession::HandleQuestgiverAcceptQuestOpcode;
	WorldPacketHandlers[CMSG_QUESTGIVER_CANCEL].handler						 = &WorldSession::HandleQuestgiverCancelOpcode;
	WorldPacketHandlers[CMSG_QUESTGIVER_CHOOSE_REWARD].handler				  = &WorldSession::HandleQuestgiverChooseRewardOpcode;
	WorldPacketHandlers[CMSG_QUESTGIVER_REQUEST_REWARD].handler				 = &WorldSession::HandleQuestgiverRequestRewardOpcode;
	WorldPacketHandlers[CMSG_QUEST_QUERY].handler							   = &WorldSession::HandleQuestQueryOpcode;
	WorldPacketHandlers[CMSG_QUESTGIVER_QUERY_QUEST].handler					= &WorldSession::HandleQuestGiverQueryQuestOpcode;
	WorldPacketHandlers[CMSG_QUESTGIVER_COMPLETE_QUEST].handler				 = &WorldSession::HandleQuestgiverCompleteQuestOpcode;
	WorldPacketHandlers[SMSG_QUEST_QUERY_QUESTFINISHERS].handler			   = &WorldSession::HandleQuestQueryQuestFinishers;
	WorldPacketHandlers[CMSG_QUESTLOG_REMOVE_QUEST].handler					 = &WorldSession::HandleQuestlogRemoveQuestOpcode;
	WorldPacketHandlers[MSG_QUEST_PUSH_RESULT].handler						  = &WorldSession::HandleQuestPushResult;
	WorldPacketHandlers[CMSG_QUEST_POI_QUERY].handler						  = &WorldSession::HandleQuestPOIQuery;
	WorldPacketHandlers[CMSG_QUERY_QUESTS_COMPLETED].handler						  = &WorldSession::HandleQuestPOIQuery;
	
	WorldPacketHandlers[CMSG_RECLAIM_CORPSE].handler							= &WorldSession::HandleCorpseReclaimOpcode;
	WorldPacketHandlers[CMSG_RESURRECT_RESPONSE].handler						= &WorldSession::HandleResurrectResponseOpcode;
	WorldPacketHandlers[CMSG_REQUEST_CEMETERY_LIST_RESPONSE].handler			= &WorldSession::HandleCemeteryListQuery;
	WorldPacketHandlers[CMSG_PUSHQUESTTOPARTY].handler						  = &WorldSession::HandlePushQuestToPartyOpcode;

	// Auction System
	WorldPacketHandlers[CMSG_AUCTION_LIST_ITEMS].handler					= &WorldSession::HandleAuctionListItems;
	WorldPacketHandlers[CMSG_AUCTION_LIST_BIDDER_ITEMS].handler				= &WorldSession::HandleAuctionListBidderItems;
	WorldPacketHandlers[CMSG_AUCTION_SELL_ITEM].handler						= &WorldSession::HandleAuctionSellItem;
	WorldPacketHandlers[CMSG_AUCTION_LIST_OWNER_ITEMS].handler				= &WorldSession::HandleAuctionListOwnerItems;
	WorldPacketHandlers[CMSG_AUCTION_PLACE_BID].handler						= &WorldSession::HandleAuctionPlaceBid;
	WorldPacketHandlers[CMSG_AUCTION_REMOVE_ITEM].handler					= &WorldSession::HandleCancelAuction;
	WorldPacketHandlers[CMSG_AUCTION_LIST_PENDING_SALES].handler			= &WorldSession::HandleAuctionQueryPendingSales;
	
	// Mail System
	WorldPacketHandlers[CMSG_GET_MAIL_LIST].handler							 = &WorldSession::HandleGetMail;
	WorldPacketHandlers[CMSG_ITEM_TEXT_QUERY].handler						   = &WorldSession::HandleItemTextQuery;
	WorldPacketHandlers[CMSG_SEND_MAIL].handler								 = &WorldSession::HandleSendMail;
	WorldPacketHandlers[CMSG_MAIL_TAKE_MONEY].handler						   = &WorldSession::HandleTakeMoney;
	WorldPacketHandlers[CMSG_MAIL_TAKE_ITEM].handler							= &WorldSession::HandleTakeItem;
	WorldPacketHandlers[CMSG_MAIL_MARK_AS_READ].handler						 = &WorldSession::HandleMarkAsRead;
	WorldPacketHandlers[CMSG_MAIL_RETURN_TO_SENDER].handler					 = &WorldSession::HandleReturnToSender;
	WorldPacketHandlers[CMSG_MAIL_DELETE].handler							   = &WorldSession::HandleMailDelete;
	WorldPacketHandlers[MSG_QUERY_NEXT_MAIL_TIME].handler					   = &WorldSession::HandleMailTime;
	WorldPacketHandlers[CMSG_MAIL_CREATE_TEXT_ITEM].handler					 = &WorldSession::HandleMailCreateTextItem;
	
	// Guild Query (called when not logged in sometimes)
	WorldPacketHandlers[CMSG_GUILD_QUERY].handler							   = &WorldSession::HandleGuildQuery;
	WorldPacketHandlers[CMSG_GUILD_QUERY].status								= STATUS_AUTHED;

	// Guild System
	WorldPacketHandlers[CMSG_GUILD_CREATE].handler							  = &WorldSession::HandleCreateGuild;
	WorldPacketHandlers[CMSG_GUILD_INVITE].handler							  = &WorldSession::HandleInviteToGuild;
	WorldPacketHandlers[CMSG_GUILD_ACCEPT].handler							  = &WorldSession::HandleGuildAccept;
	WorldPacketHandlers[CMSG_GUILD_DECLINE].handler							 = &WorldSession::HandleGuildDecline;
	WorldPacketHandlers[CMSG_GUILD_INFO].handler								= &WorldSession::HandleGuildInfo;
	WorldPacketHandlers[CMSG_GUILD_ROSTER].handler							  = &WorldSession::HandleGuildRoster;
	WorldPacketHandlers[CMSG_GUILD_PROMOTE].handler							 = &WorldSession::HandleGuildPromote;
//	WorldPacketHandlers[CMSG_GUILD_DEMOTE].handler							  = &WorldSession::HandleGuildDemote;
	WorldPacketHandlers[CMSG_GUILD_LEAVE].handler							   = &WorldSession::HandleGuildLeave;
	WorldPacketHandlers[CMSG_GUILD_REMOVE].handler							  = &WorldSession::HandleGuildRemove;
	WorldPacketHandlers[CMSG_GUILD_DISBAND].handler							 = &WorldSession::HandleGuildDisband;
	WorldPacketHandlers[CMSG_GUILD_LEADER].handler							  = &WorldSession::HandleGuildLeader;
	WorldPacketHandlers[CMSG_GUILD_MOTD].handler								= &WorldSession::HandleGuildMotd;
	WorldPacketHandlers[CMSG_GUILD_RANK_EDIT].handler							= &WorldSession::HandleGuildRankEdit;
	WorldPacketHandlers[CMSG_GUILD_ADD_RANK].handler							= &WorldSession::HandleGuildAddRank;
	WorldPacketHandlers[CMSG_GUILD_DEL_RANK].handler							= &WorldSession::HandleGuildDelRank;
	WorldPacketHandlers[CMSG_GUILD_PROMOTE_DEMOTE_RANK].handler					= &WorldSession::HandleGuildPromoteDemoteRank;
//	WorldPacketHandlers[CMSG_GUILD_SET_PUBLIC_NOTE].handler					 = &WorldSession::HandleGuildSetPublicNote;
//	WorldPacketHandlers[CMSG_GUILD_SET_OFFICER_NOTE].handler					= &WorldSession::HandleGuildSetOfficerNote;
	WorldPacketHandlers[CMSG_GUILD_SET_NOTE].handler							= &WorldSession::HandleGuildSetNote;
	WorldPacketHandlers[CMSG_PETITION_BUY].handler							  = &WorldSession::HandleCharterBuy;
	WorldPacketHandlers[CMSG_PETITION_SHOW_SIGNATURES].handler				  = &WorldSession::HandleCharterShowSignatures;
	WorldPacketHandlers[CMSG_TURN_IN_PETITION].handler						  = &WorldSession::HandleCharterTurnInCharter;
	WorldPacketHandlers[CMSG_PETITION_QUERY].handler							= &WorldSession::HandleCharterQuery;
	WorldPacketHandlers[CMSG_OFFER_PETITION].handler							= &WorldSession::HandleCharterOffer;
	WorldPacketHandlers[CMSG_PETITION_SIGN].handler							 = &WorldSession::HandleCharterSign;
	WorldPacketHandlers[MSG_PETITION_RENAME].handler							= &WorldSession::HandleCharterRename;
	WorldPacketHandlers[MSG_PETITION_DECLINE].handler							= &WorldSession::HandleCharterSignDecline;
	WorldPacketHandlers[MSG_SAVE_GUILD_EMBLEM].handler						  = &WorldSession::HandleSaveGuildEmblem;
	WorldPacketHandlers[CMSG_GUILD_SET_INFO_TEXT].handler					 = &WorldSession::HandleSetGuildInformation;
	WorldPacketHandlers[MSG_QUERY_GUILD_BANK_TEXT].handler					= &WorldSession::HandleGuildBankQueryText;
	WorldPacketHandlers[CMSG_SET_GUILD_BANK_TEXT].handler					= &WorldSession::HandleSetGuildBankText;				
	WorldPacketHandlers[MSG_GUILD_EVENT_LOG_QUERY].handler					= &WorldSession::HandleGuildLog;
	WorldPacketHandlers[CMSG_GUILD_BANKER_ACTIVATE].handler					= &WorldSession::HandleGuildBankOpenVault;
	WorldPacketHandlers[CMSG_GUILD_BANK_BUY_TAB].handler					= &WorldSession::HandleGuildBankBuyTab;
	WorldPacketHandlers[MSG_GUILD_BANK_MONEY_WITHDRAWN].handler				= &WorldSession::HandleGuildBankGetAvailableAmount;
	WorldPacketHandlers[CMSG_GUILD_BANK_UPDATE_TAB].handler					= &WorldSession::HandleGuildBankModifyTab;
	WorldPacketHandlers[CMSG_GUILD_BANK_SWAP_ITEMS].handler					= &WorldSession::HandleGuildBankDepositItem;
	WorldPacketHandlers[CMSG_GUILD_BANK_WITHDRAW_MONEY].handler				= &WorldSession::HandleGuildBankWithdrawMoney;
	WorldPacketHandlers[CMSG_GUILD_BANK_DEPOSIT_MONEY].handler				= &WorldSession::HandleGuildBankDepositMoney;
	WorldPacketHandlers[CMSG_GUILD_BANK_QUERY_TAB].handler					= &WorldSession::HandleGuildBankViewTab;
	WorldPacketHandlers[MSG_GUILD_BANK_LOG_QUERY].handler					= &WorldSession::HandleGuildBankViewLog;
	WorldPacketHandlers[MSG_GUILD_PERMISSIONS].handler						= &WorldSession::HandleGuildGetFullPermissions;
	WorldPacketHandlers[CMSG_GUILD_RANK_QUERY].handler						= &WorldSession::HandleGuildQueryRanks;
	WorldPacketHandlers[CMSG_GUILD_QUERY_UNK].handler						= &WorldSession::HandleGuildQueryUnk;
	WorldPacketHandlers[MSG_GUILD_UNK].handler								= &WorldSession::HandleGuildUnk;
	WorldPacketHandlers[CMSG_QUERY_GUILD_SHOP_TAB].handler					= &WorldSession::HandleGuildQueryGuildTabData;
	WorldPacketHandlers[CMSG_QUERY_GUILD_XP_LIMIT_TODAY].handler			= &WorldSession::HandleGuildXPLimitToday;
	WorldPacketHandlers[CMSG_QUERY_GUILD_XP_STATUS_TAB].handler				= &WorldSession::HandleGuildXPStatusTab;
	WorldPacketHandlers[CMSG_QUERY_GUILD_NEWS_TAB_INFO].handler				= &WorldSession::HandleGuildGuildLogsTab;
	WorldPacketHandlers[CMSG_QUERY_GUILD_NEWS_MAKE_STICKY].handler			= &WorldSession::HandleGuildNewsToggleSticky;
	WorldPacketHandlers[CMSG_GUILD_PLAYERS_LOGS_QUERY].handler				= &WorldSession::HandleGuildQueryLogsPlayers;
	WorldPacketHandlers[CMSG_QUERY_GUILD_UNK].handler						= &WorldSession::HandleGuildQueryUnk2;
	WorldPacketHandlers[CMSG_GUILD_FINDER_QUERY_PENDING_REQUESTS].handler	= &WorldSession::HandleGuildFinderQueryState;
	WorldPacketHandlers[CMSG_GUILD_FINDER_SET_DETAIL].handler				= &WorldSession::HandleGuildFinderCharSetFilter;
	WorldPacketHandlers[CMSG_GUILD_FINDER_REQUEST_ADD].handler				= &WorldSession::HandleGuildFinderPlayerAddRequest;
	WorldPacketHandlers[CMSG_GUILD_FINDER_REQUEST_DEL].handler				= &WorldSession::HandleGuildFinderPlayerDelRequest;
	WorldPacketHandlers[CMSG_GUILD_FINDER_GUILD_SET_DETAIL].handler			= &WorldSession::HandleGuildFinderGuildSetFilter;
	WorldPacketHandlers[CMSG_GUILD_FINDER_GUILD_GET_REQUESTS].handler		= &WorldSession::HandleGuildFinderGuildGetRequests;
	WorldPacketHandlers[CMSG_GUILD_FINDER_GUILD_DEL_REQUEST].handler		= &WorldSession::HandleGuildFinderGuildDelRequest;
//	WorldPacketHandlers[CMSG_ACHIEVEMNT_RECV_FEEDBACK].handler				= &WorldSession::HandleAchievementRecvFeedback;
	
	// Tutorials
	WorldPacketHandlers[CMSG_TUTORIAL_FLAG].handler							 = &WorldSession::HandleTutorialFlag;
	WorldPacketHandlers[CMSG_TUTORIAL_CLEAR].handler							= &WorldSession::HandleTutorialClear;
	WorldPacketHandlers[CMSG_TUTORIAL_RESET].handler							= &WorldSession::HandleTutorialReset;
	
	// Pets
	WorldPacketHandlers[CMSG_PET_ACTION].handler								= &WorldSession::HandlePetAction;
	WorldPacketHandlers[CMSG_REQUEST_PET_INFO].handler						  = &WorldSession::HandlePetInfo;
	WorldPacketHandlers[CMSG_PET_NAME_QUERY].handler							= &WorldSession::HandlePetNameQuery;
	WorldPacketHandlers[CMSG_BUY_STABLE_SLOT].handler						   = &WorldSession::HandleBuyStableSlot;
	WorldPacketHandlers[CMSG_STABLE_PET].handler								= &WorldSession::HandleStablePet;
	WorldPacketHandlers[CMSG_UNSTABLE_PET].handler							  = &WorldSession::HandleUnstablePet;
	WorldPacketHandlers[CMSG_STABLE_SWAP_PET].handler							  = &WorldSession::HandleStableSwapPet;
	WorldPacketHandlers[MSG_LIST_STABLED_PETS].handler						  = &WorldSession::HandleStabledPetList;
	WorldPacketHandlers[CMSG_PET_SET_ACTION].handler							= &WorldSession::HandlePetSetActionOpcode;
	WorldPacketHandlers[CMSG_PET_RENAME].handler								= &WorldSession::HandlePetRename;
	WorldPacketHandlers[CMSG_PET_ABANDON].handler							   = &WorldSession::HandlePetAbandon;
//	WorldPacketHandlers[CMSG_PET_UNLEARN].handler								= &WorldSession::HandlePetUnlearn;
	WorldPacketHandlers[CMSG_PET_SPELL_AUTOCAST].handler						= &WorldSession::HandlePetSpellAutocast;
	WorldPacketHandlers[CMSG_PET_CANCEL_AURA].handler						= &WorldSession::HandlePetCancelAura;
	WorldPacketHandlers[CMSG_PET_LEARN_TALENT].handler						= &WorldSession::HandlePetLearnTalent;
	WorldPacketHandlers[CMSG_LEARN_PREVIEW_TALENTS_PET].handler				= &WorldSession::HandlePetLearnPreviewTalent;
	
	// Battlegrounds
	WorldPacketHandlers[CMSG_BATTLEFIELD_LIST].handler						 = &WorldSession::HandleBattlefieldListOpcode;
	WorldPacketHandlers[CMSG_BATTLEFIELD_JOIN_QUEUE].handler				 = &WorldSession::HandleBattleMasterJoinOpcode;
	WorldPacketHandlers[CMSG_BATTLEFIELD_LEAVE_JOIN_REPLY].handler			 = &WorldSession::HandleBattlefieldJoinLeaveSelect;
	WorldPacketHandlers[CMSG_REQUEST_RATED_ARENA_INFO].handler				 = &WorldSession::HandleBattlefieldRequestRatedArenaRewards;
	WorldPacketHandlers[CMSG_REQUEST_RATED_BG_INFO].handler					 = &WorldSession::HandleBattlefieldRequestRatedBGRewards;
	WorldPacketHandlers[CMSG_BATTLEFIELD_GROUP_WARGAME_SCORE].handler		 = &WorldSession::HandleBattlefieldRequestWargameScore;
	WorldPacketHandlers[CMSG_PVP_TYPES_ENABLED].handler						 = &WorldSession::HandleBattlefieldQueryEnableTypes;
	WorldPacketHandlers[CMSG_WARGAMES_INVITE_PARTY_LEADER].handler			 = &WorldSession::HandleWargamesInviteParty;
	WorldPacketHandlers[CMSG_WARGAMES_INVITE_PARTY_LEADER_OTHER_REPLY].handler	 = &WorldSession::HandleWargamesInvitePartyReply;

//	WorldPacketHandlers[CMSG_BATTLEFIELD_PORT].handler						  = &WorldSession::HandleBattlefieldPortOpcode;
//	WorldPacketHandlers[CMSG_BATTLEFIELD_STATUS].handler						= &WorldSession::HandleBattlefieldStatusOpcode;
	WorldPacketHandlers[CMSG_BATTLEMASTER_HELLO].handler						= &WorldSession::HandleBattleMasterHelloOpcode;
	WorldPacketHandlers[CMSG_BATTLEMASTER_JOIN_ARENA].handler				= &WorldSession::HandleArenaJoinOpcode;
//	WorldPacketHandlers[CMSG_BATTLEMASTER_JOIN].handler						 = &WorldSession::HandleBattleMasterJoinOpcode;
	WorldPacketHandlers[CMSG_LEAVE_BATTLEFIELD].handler						 = &WorldSession::HandleLeaveBattlefieldOpcode;
	WorldPacketHandlers[CMSG_AREA_SPIRIT_HEALER_QUERY].handler				  = &WorldSession::HandleAreaSpiritHealerQueryOpcode;
	WorldPacketHandlers[CMSG_AREA_SPIRIT_HEALER_QUEUE].handler				  = &WorldSession::HandleAreaSpiritHealerQueueOpcode;
	WorldPacketHandlers[CMSG_BATTLEGROUND_PLAYER_POSITIONS].handler			  = &WorldSession::HandleBattlegroundPlayerPositionsOpcode;
//	WorldPacketHandlers[MSG_PVP_LOG_DATA].handler							   = &WorldSession::HandlePVPLogDataOpcode;
	WorldPacketHandlers[CMSG_BATTLEFIELD_REQUEST_SCORE_DATA].handler		   = &WorldSession::HandlePVPLogDataOpcode;
	WorldPacketHandlers[CMSG_INSPECT_HONOR_STATS].handler						= &WorldSession::HandleInspectHonorStatsOpcode;
	WorldPacketHandlers[CMSG_SET_ACTIONBAR_TOGGLES].handler					 = &WorldSession::HandleSetActionBarTogglesOpcode;
	WorldPacketHandlers[CMSG_MOVE_SPLINE_DONE].handler						  = &WorldSession::HandleMoveSplineCompleteOpcode;
	
	// GM Ticket System
	WorldPacketHandlers[CMSG_GMTICKET_CREATE].handler						   = &WorldSession::HandleGMTicketCreateOpcode;
	WorldPacketHandlers[CMSG_GMTICKET_UPDATETEXT].handler					   = &WorldSession::HandleGMTicketUpdateOpcode;
	WorldPacketHandlers[CMSG_GMTICKET_DELETETICKET].handler					 = &WorldSession::HandleGMTicketDeleteOpcode;
	WorldPacketHandlers[CMSG_GMTICKET_GETTICKET].handler						= &WorldSession::HandleGMTicketGetTicketOpcode;
	WorldPacketHandlers[CMSG_GMTICKET_SYSTEMSTATUS].handler					 = &WorldSession::HandleGMTicketSystemStatusOpcode;
	WorldPacketHandlers[CMSG_GMTICKETSYSTEM_TOGGLE].handler					 = &WorldSession::HandleGMTicketToggleSystemStatusOpcode;
	WorldPacketHandlers[CMSG_UNLEARN_SKILL].handler							 = &WorldSession::HandleUnlearnSkillOpcode;
	
	// Meeting Stone / Instances
	WorldPacketHandlers[CMSG_SUMMON_RESPONSE].handler							= &WorldSession::HandleSummonResponseOpcode;
	WorldPacketHandlers[CMSG_RESET_INSTANCES].handler							= &WorldSession::HandleResetInstanceOpcode;
	WorldPacketHandlers[CMSG_SELF_RES].handler								  = &WorldSession::HandleSelfResurrectOpcode;
	WorldPacketHandlers[MSG_RANDOM_ROLL].handler								= &WorldSession::HandleRandomRollOpcode;
	WorldPacketHandlers[MSG_SET_DUNGEON_DIFFICULTY].handler                        = &WorldSession::HandleDungeonDifficultyOpcode;
	WorldPacketHandlers[MSG_SET_RAID_DIFFICULTY].handler                        = &WorldSession::HandleRaidDifficultyOpcode;

	// Misc
	WorldPacketHandlers[CMSG_OPEN_ITEM].handler								 = &WorldSession::HandleOpenItemOpcode;
	WorldPacketHandlers[CMSG_COMPLETE_CINEMATIC].handler						= &WorldSession::HandleCompleteCinematic;
	WorldPacketHandlers[CMSG_MOUNTSPECIAL_ANIM].handler						 = &WorldSession::HandleMountSpecialAnimOpcode;
	WorldPacketHandlers[CMSG_TOGGLE_CLOAK].handler							  = &WorldSession::HandleToggleCloakOpcode;
	WorldPacketHandlers[CMSG_TOGGLE_HELM].handler							   = &WorldSession::HandleToggleHelmOpcode;
	WorldPacketHandlers[CMSG_SET_TITLE].handler							= &WorldSession::HandleSetVisibleRankOpcode;
//	WorldPacketHandlers[CMSG_REPORT_SPAM].handler								= &WorldSession::HandleReportSpamOpcode;
	WorldPacketHandlers[CMSG_REQUEST_RESEARCH_HISTORY].handler				= &WorldSession::HandleRequestResearchHistory;
	WorldPacketHandlers[CMSG_ASK_OBJECT_CREATE_UNKNOWN].handler				= &WorldSession::HandleUnknownClientObjectCreateQuery;

	WorldPacketHandlers[CMSG_PET_CAST_SPELL].handler				= &WorldSession::HandleAddDynamicTargetOpcode;


	// Arenas
	WorldPacketHandlers[CMSG_ARENA_TEAM_QUERY].handler = &WorldSession::HandleArenaTeamQueryOpcode;
	WorldPacketHandlers[CMSG_ARENA_TEAM_ROSTER].handler = &WorldSession::HandleArenaTeamRosterOpcode;
	WorldPacketHandlers[CMSG_ARENA_TEAM_INVITE].handler = &WorldSession::HandleArenaTeamAddMemberOpcode;
	WorldPacketHandlers[CMSG_ARENA_TEAM_ACCEPT].handler = &WorldSession::HandleArenaTeamInviteAcceptOpcode;
	WorldPacketHandlers[CMSG_ARENA_TEAM_DECLINE].handler = &WorldSession::HandleArenaTeamInviteDenyOpcode;
	WorldPacketHandlers[CMSG_ARENA_TEAM_LEAVE].handler = &WorldSession::HandleArenaTeamLeaveOpcode;
	WorldPacketHandlers[CMSG_ARENA_TEAM_REMOVE].handler = &WorldSession::HandleArenaTeamRemoveMemberOpcode;
	WorldPacketHandlers[CMSG_ARENA_TEAM_DISBAND].handler = &WorldSession::HandleArenaTeamDisbandOpcode;
	WorldPacketHandlers[CMSG_ARENA_TEAM_LEADER].handler = &WorldSession::HandleArenaTeamPromoteOpcode;
	WorldPacketHandlers[MSG_INSPECT_ARENA_TEAMS].handler = &WorldSession::HandleInspectArenaStatsOpcode;
	WorldPacketHandlers[CMSG_ARENA_TEAM_CREATE].handler = &WorldSession::HandleArenaTeamCreate;
	WorldPacketHandlers[CMSG_INSPECT_RATEG_BG].handler = &WorldSession::HandleInspectRatedBG;

	//calendar
	WorldPacketHandlers[CMSG_CALENDAR_GET_NUM_PENDING].handler = &WorldSession::HandleCalendarGetNumPending;

#ifdef CLUSTERING
	WorldPacketHandlers[CMSG_PING].handler = &WorldSession::HandlePingOpcode;
#endif

	// cheat/gm commands?
	WorldPacketHandlers[MSG_MOVE_TELEPORT_CHEAT].handler = &WorldSession::HandleTeleportCheatOpcode;
	WorldPacketHandlers[CMSG_TELEPORT_TO_UNIT].handler = &WorldSession::HandleTeleportToUnitOpcode;
	WorldPacketHandlers[CMSG_WORLD_TELEPORT].handler = &WorldSession::HandleWorldportOpcode;

	// voicechat
//	WorldPacketHandlers[CMSG_ENABLE_MICROPHONE].handler = &WorldSession::HandleEnableMicrophoneOpcode;
//	WorldPacketHandlers[CMSG_VOICE_CHAT_QUERY].handler = &WorldSession::HandleVoiceChatQueryOpcode;
//	WorldPacketHandlers[CMSG_CHANNEL_VOICE_QUERY].handler = &WorldSession::HandleChannelVoiceQueryOpcode;

//	WorldPacketHandlers[0x038C].handler = &WorldSession::Handle38C;
//	WorldPacketHandlers[0x038C].status = STATUS_AUTHED;

	WorldPacketHandlers[CMSG_QUESTGIVER_STATUS_MULTIPLE_QUERY].handler = &WorldSession::HandleInrangeQuestgiverQuery;

	// Vehicles
	WorldPacketHandlers[CMSG_ENTER_VEHICLE].handler =							&WorldSession::HandleEnterVehicleOpcode;
	WorldPacketHandlers[CMSG_DISMISS_CONTROLLED_VEHICLE].handler =				&WorldSession::HandleVehicleDismiss;
	WorldPacketHandlers[CMSG_EJECT_PASSENGER].handler =							&WorldSession::HandleVehicleEjectPassenger;
	WorldPacketHandlers[CMSG_REQUEST_PLAYER_VEHICLE_EXIT].handler =				&WorldSession::HandleVehicleDismiss;
	WorldPacketHandlers[CMSG_REQUEST_VEHICLE_SWITCH_SEAT].handler =				&WorldSession::HandleVehicleSwitchSeat;
//	WorldPacketHandlers[CMSG_REQUEST_VEHICLE_EXIT].handler					= &WorldSession::HandleExitVehicle;





	//packets that should not be compressed ever !
	//k, maybe some could be compressed, try to filter those :P
	WorldPacketHandlers[ SMSG_UPDATE_OBJECT ].skip_compress = 1;		//as long as this cannot be serialized with compressed update object, we cannot send it compressed
	WorldPacketHandlers[ SMSG_COMPRESSED_UPDATE_OBJECT ].skip_compress = 1;
	WorldPacketHandlers[ SMSG_COMPRESSED_MOVES ].skip_compress = 1;
	WorldPacketHandlers[ SMSG_UNCOMPRESSED_MOVES ].skip_compress = 1;
	WorldPacketHandlers[ SMSG_PONG ].skip_compress = 1;					//cheat latency with quick reply :)
	WorldPacketHandlers[ SMSG_AUCTION_LIST_RESULT_COMPRESSED ].skip_compress = 1;		
	WorldPacketHandlers[ SMSG_ALL_ACHIEVEMENT_DATA_COMPRESSED ].skip_compress = 1;		
	WorldPacketHandlers[ SMSG_QUERY_INSPECT_ACHIEVEMENTS_COMPRESSED ].skip_compress = 1;		
	WorldPacketHandlers[ SMSG_SEND_QUEUED_PACKETS ].skip_compress = 1;			
	WorldPacketHandlers[ SMSG_CHAR_ENUM ].skip_compress = 1;			
	WorldPacketHandlers[ SMSG_TUTORIAL_FLAGS ].skip_compress = 1;			
	WorldPacketHandlers[ MSG_SET_DUNGEON_DIFFICULTY ].skip_compress = 1;	
	WorldPacketHandlers[ SMSG_REDIRECT_CLIENT ].skip_compress = 1;				
	WorldPacketHandlers[ SMSG_AUTH_CHALLENGE ].skip_compress = 1;				
	WorldPacketHandlers[ SMSG_LOGIN_VERIFY_WORLD ].skip_compress = 1;			
	WorldPacketHandlers[ SMSG_FEATURE_SYSTEM_STATUS ].skip_compress = 1;		
	WorldPacketHandlers[ SMSG_CONTACT_LIST ].skip_compress = 1;					
	WorldPacketHandlers[ SMSG_BINDPOINTUPDATE ].skip_compress = 1;				
	WorldPacketHandlers[ SMSG_TALENTS_INFO ].skip_compress = 1;					
	WorldPacketHandlers[ SMSG_INITIAL_SPELLS ].skip_compress = 1;				
	WorldPacketHandlers[ SMSG_ACTION_BUTTONS ].skip_compress = 1;				
	WorldPacketHandlers[ SMSG_INITIALIZE_FACTIONS ].skip_compress = 1;					
	WorldPacketHandlers[ SMSG_INIT_WORLD_STATES ].skip_compress = 1;					
	WorldPacketHandlers[ SMSG_LOGIN_SETTIMESPEED ].skip_compress = 1;					
}

void SessionLogWriter::writefromsession(WorldSession* session, const char* format, ...)
{
	if(!IsOpen())
		return;

	va_list ap;
	va_start(ap, format);
#define local_buffer_size 15000 //this is due to stack limitation for profiler :S
	char out[local_buffer_size];

	time_t t = UNIXTIME;
	tm* aTm = localtime(&t);
	snprintf(out, local_buffer_size, "[%-4d-%02d-%02d %02d:%02d:%02d] ",aTm->tm_year+1900,aTm->tm_mon+1,aTm->tm_mday,aTm->tm_hour,aTm->tm_min,aTm->tm_sec);
	size_t l = strlen(out);

	snprintf(&out[l], local_buffer_size - l, "Account %u [%s], IP %s, Player %s :: ", (unsigned int)session->GetAccountId(), session->GetAccountName().c_str(),
		session->GetSocket() ? session->GetSocket()->GetRemoteIP().c_str() : "NOIP", 
		session->GetPlayer() ? session->GetPlayer()->GetName() : "nologin");

	l = strlen(out);
	vsnprintf(&out[l], local_buffer_size - l, format, ap);

	fprintf(m_file, "%s\n", out);

	if( session->GetPlayer() )
		CharacterDatabase.Execute( "INSERT INTO gm_logs values( NULL,'%u','%u','%s' )", (unsigned int)session->GetAccountId(), session->GetPlayer()->GetLowGUID(), CharacterDatabase.EscapeString( out ).c_str() );
	else
		CharacterDatabase.Execute( "INSERT INTO gm_logs values( NULL,'%u','0','%s' )", (unsigned int)session->GetAccountId(), CharacterDatabase.EscapeString( out ).c_str() );

	fflush(m_file);
	va_end(ap);
}

#ifdef CLUSTERING
void WorldSession::HandlePingOpcode(WorldPacket& recvPacket)
{
	uint32 pong;
	recvPacket >> pong;
	WorldPacket data(SMSG_PONG, 4);
	data << pong;
	SendPacket(&data);
}

#endif

void WorldSession::SystemMessage(const char * format, ...)
{
	WorldPacket * data;
	char buffer[1024];
	va_list ap;
	va_start(ap,format);
	vsnprintf(buffer,1024,format,ap);
	va_end(ap);

	data = sChatHandler.FillSystemMessageData(buffer);
	SendPacket(data);
	delete data;
	data = NULL;
}

void WorldSession::SendChatPacket(WorldPacket * data, uint32 langpos, int32 lang, WorldSession * originator)
{
#ifndef USING_BIG_ENDIAN
	if(lang == -1)
		*(uint32*)&data->contents()[langpos] = lang;
	else
	{
		if(CanUseCommand('c') || (originator && originator->CanUseCommand('c')))
			*(uint32*)&data->contents()[langpos] = LANG_UNIVERSAL;
		else
			*(uint32*)&data->contents()[langpos] = lang;
	}
#else
	if(lang == -1)
		*(uint32*)&data->contents()[langpos] = swap32(lang);
	else
	{
		if(CanUseCommand('c') || (originator && originator->CanUseCommand('c')))
			*(uint32*)&data->contents()[langpos] = swap32(uint32(LANG_UNIVERSAL));
		else
			*(uint32*)&data->contents()[langpos] = swap32(lang);
	}
#endif

	SendPacket(data);
}

void WorldSession::SendItemPushResult(Item * pItem, bool Created, bool Received, bool SendToSet, bool NewItem, uint16 DestBagSlot, uint32 DestSlot, uint32 AddCount)
{
	if( _player == NULL )
		return;
/*
{SERVER} Packet: (0xDF54) SMSG_ITEM_PUSH_RESULT PacketSize = 45 TimeStamp = 18093156
E9 D7 38 03 00 00 00 01 
01 00 00 00 
01 00 00 00 
01 00 00 00 
FF 
18 00 00 00 
BF 8F 00 00
80 B7 67 14 
00 00 00 00 
01 00 00 00 
01 00 00 00 
*/
	/*WorldPacket data(SMSG_ITEM_PUSH_RESULT, 60);
	data << _player->GetGUID();
	data << uint32(Received);
	data << uint32(Created);
	data << uint32(1);
	data << uint8(DestBagSlot);
	data << uint32(NewItem ? DestSlot : 0xFFFFFFFF);
	data << pItem->GetEntry();
	data << pItem->GetItemRandomSuffixFactor();
	data << pItem->GetUInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID);
	data << AddCount;
	data << pItem->GetUInt32Value(ITEM_FIELD_STACK_COUNT);*/

	packetSMSG_ITEM_PUSH_RESULT data;
	data.guid = _player->GetGUID();
	data.received = Received;
	data.created = Created;
	data.unk1 = 1;
	data.destbagslot = (uint8)DestBagSlot;
	data.destslot = NewItem ? DestSlot : 0xFFFFFFFF;
	data.entry = pItem->GetEntry();
	data.suffix = pItem->GetItemRandomSuffixFactor();
	data.randomprop = pItem->GetUInt32Value( ITEM_FIELD_RANDOM_PROPERTIES_ID );
	data.count = AddCount;
	data.stackcount = pItem->GetUInt32Value(ITEM_FIELD_STACK_COUNT);

	if(SendToSet)
	{
		/*if(Created)
			_player->SendMessageToSet(&data, true);
		else
		{*/
			/*if(_player->GetGroup())
				_player->GetGroup()->SendPacketToAll(&data);
			else
				SendPacket(&data);*/
		/*}*/

		if( _player->GetGroup() )
			_player->GetGroup()->OutPacketToAll( SMSG_ITEM_PUSH_RESULT, sizeof( packetSMSG_ITEM_PUSH_RESULT ), &data );
		else
			OutPacket( SMSG_ITEM_PUSH_RESULT, sizeof( packetSMSG_ITEM_PUSH_RESULT ), &data );
	}
	else
	{
		//SendPacket(&data);
		OutPacket( SMSG_ITEM_PUSH_RESULT, sizeof( packetSMSG_ITEM_PUSH_RESULT ), &data );
	}
}

void WorldSession::Delete(bool removefromlist)
{
	//do not multiple mark object for deletion
	if( bDeleted )
		return;

	//set deleted to not loop function
	bDeleted = true;

	//remove from session list also to allow reconnect
	if( removefromlist == true )
		sWorld.RemoveSession( GetAccountId() );

	//very probably already disconnected but better make sure
	Disconnect();

	//should not happen
	if( _player )
		LogoutPlayer( _player->deleted == OBJ_AVAILABLE );

	//rare crashexploit. Somehow DC then relog to cause player object get double deleted
	if( _player && _player->deleted == OBJ_AVAILABLE )
		SetPlayer( NULL );

	//register delayed object deletion
	sEventMgr.AddEvent( this, &WorldSession::_Delete, EVENT_DELETE_SESSION,GARBAGE_DELETE_DELAY_WS,1,EVENT_FLAG_DELETES_OBJECT );
}
/*
void WorldSession::Handle38C(WorldPacket & recv_data)
{
	uint32 v;
	recv_data >> v;

	WorldPacket data(0x038B, 17);
	data << v << uint32(0);
	data << "01/01/01";
	SendPacket(&data);
} */

/*
	2008/10/04
	MultiLanguages on each player session.
	LocalizedWorldSrv translating core message from sql.
	LocalizedMapName translating MAP Title from sql.
	LocalizedBroadCast translating new broadcast system from sql.
	Full merged from p2wow 's branch (p2branch).
	cebernic@gmail.com
*/

const char* WorldSession::LocalizedWorldSrv(uint32 id)
{
	char szError[64];
	WorldStringTable * wst = WorldStringTableStorage.LookupEntry(id);
	if(!wst){
		memset(szError,0,64);
		sprintf(szError,"ID:%u is a bad WorldString TEXT!",id);
		return &szError[0];
	}

	LocalizedWorldStringTable * lpi = (language>0) ? sLocalizationMgr.GetLocalizedWorldStringTable(id,language):NULL;
	if(lpi)
		return lpi->Text;
	else
		return wst->text;
}

const char* WorldSession::LocalizedMapName(uint32 id)
{
	char szError[64];
	MapInfo * mi = WorldMapInfoStorage.LookupEntry(id);
	if(!mi){
		memset(szError,0,64);
		sprintf(szError,"ID:%u still have no map title yet!",id);
		return &szError[0];
	}

	LocalizedWorldMapInfo * lpi = (language>0) ? sLocalizationMgr.GetLocalizedWorldMapInfo(id,language):NULL;
	if(lpi)
		return lpi->Text;
	else
		return mi->name;
}

const char* WorldSession::LocalizedBroadCast(uint32 id)
{
	char szError[64];
	WorldBroadCast * wb = WorldBroadCastStorage.LookupEntry(id);
	if(!wb){
		memset(szError,0,64);
		sprintf(szError,"ID:%u is a invaild WorldBroadCast message!",id);
		return &szError[0];
	}

	LocalizedWorldBroadCast * lpi = (language>0) ? sLocalizationMgr.GetLocalizedWorldBroadCast(id,language):NULL;
	if(lpi)
		return lpi->Text;
	else
		return wb->text;
}

void WorldSession::SendAccountDataTimes(uint32 mask)
{
    WorldPacket data( SMSG_ACCOUNT_DATA_TIMES, 4+1+4+8*4 ); // changed in WotLK
    data << uint32( UNIXTIME );                             // unix time of something
    data << uint8(1);
    data << uint32(mask);                                   // type mask
    for(uint32 i = 0; i < NUM_ACCOUNT_DATA_TYPES; ++i)
		if(mask & (1 << i)){
            //data << uint32(GetAccountData(AccountDataType(i))->Time);// also unix time
			data << uint32( 0 );
		}
    SendPacket(&data);
}

void WorldSession::SendPacketCompressed(WorldPacket* packet)
{
	uint32 size = (uint32)packet->size();
	uint32 destsize = size + size/10 + 16;
	int rate = sWorld.getIntRate(INTRATE_COMPRESSION);

	if(size >= 40000 && rate < 6)
		rate = 6;

	if(size <= 100)
		rate = 0;			// don't bother compressing packet smaller than this, zlib doesnt really handle them well

	// set up stream
	z_stream stream;
	stream.zalloc = 0;
	stream.zfree  = 0;
	stream.opaque = 0;

	if(deflateInit(&stream, rate) != Z_OK)
	{
		sLog.outError("deflateInit failed.");
		return;
	}

	uint8 *buffer = new uint8[destsize];

	// set up stream pointers
	stream.next_out  = (Bytef*)buffer+4;
	stream.avail_out = destsize;
	stream.next_in   = (Bytef*)packet->contents();
	stream.avail_in  = size;

	// call the actual process
	if(deflate(&stream, Z_NO_FLUSH) != Z_OK ||
		stream.avail_in != 0)
	{
		sLog.outError("deflate failed.");
		delete [] buffer;
		return;
	}

	// finish the deflate
	if(deflate(&stream, Z_FINISH) != Z_STREAM_END)
	{
		sLog.outError("deflate failed: did not end stream");
		delete [] buffer;
		return;
	}

	// finish up
	if(deflateEnd(&stream) != Z_OK)
	{
		sLog.outError("deflateEnd failed.");
		delete [] buffer;
		return;
	}

	// fill in the full size of the compressed stream
	*(uint32*)&buffer[0] = size;

	// send it
	OutPacket(packet->GetOpcode(), (uint16)stream.total_out + 4, (const void*)buffer);

	// cleanup memory
	delete [] buffer;
}

void WorldSession::SendPacket(WorldPacket* packet)
{
#ifdef GM_COMMAND_TO_LOG_PLAYER_INCOMMING_PACKETS
	if( LogOutGoungPackets != NULL )
		LogSessionPacket( 1, LogOutGoungPackets, packet );
#endif
	if(_socket && _socket->IsConnected())
	{
		//client is busy loading the map
/*		if( _player && ( _player->GetPlayerStatus( ) & TRANSFER_PENDING ) )
		{
			_player->CopyAndSendDelayedPacket( packet );
			return;
		}
		else */
			_socket->SendPacket(packet);
	}
}

void WorldSession::SendPacket(StackWorldPacket * packet)
{
#ifdef GM_COMMAND_TO_LOG_PLAYER_INCOMMING_PACKETS
	if( LogOutGoungPackets != NULL )
		LogSessionPacket( 1, LogOutGoungPackets, packet );
#endif
	if(_socket && _socket->IsConnected())
	{
/*		//client is busy loading the map
		if( _player && ( _player->GetPlayerStatus( ) & TRANSFER_PENDING ) )
		{
			_player->CopyAndSendDelayedPacket( packet );
			return;
		}
		else */
			_socket->SendPacket(packet);
	}
}

ARCEMU_INLINE void WorldSession::OutPacket(uint16 opcode)
{
	if(_socket && _socket->IsConnected())
		_socket->OutPacket(opcode, 0, NULL);
}

void WorldSession::OutPacket(uint16 opcode, uint16 len, const void* data)
{
#ifdef GM_COMMAND_TO_LOG_PLAYER_INCOMMING_PACKETS
	if( LogOutGoungPackets != NULL )
		LogSessionPacket( 1, opcode, len, (const uint8*)data, LogOutGoungPackets );
#endif
	if(_socket && _socket->IsConnected())
		_socket->OutPacket(opcode, len, data);
}
