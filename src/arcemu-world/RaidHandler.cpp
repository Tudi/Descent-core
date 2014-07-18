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

void WorldSession::HandleConvertGroupToRaidOpcode(WorldPacket & recv_data)
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	// This is just soooo easy now   
	Group *pGroup = _player->GetGroup();
	if(!pGroup) 
	{ 
		return;
	}

	if ( pGroup->GetLeader() != _player->m_playerInfo )   //access denied
	{
		SendPartyCommandResult(_player, 0, "", ERR_PARTY_YOU_ARE_NOT_LEADER);
		return;
	}

	if( pGroup->GetGroupType() & GROUP_TYPE_RAID )
	{
		if( pGroup->MemberCount() > MAX_GROUP_SIZE_PARTY )
			SendPartyCommandResult(_player, 0, "", ERR_PARTY_IS_FULL);
		else
			pGroup->ShrinkToParty( );
	}
	else
		pGroup->ExpandToRaid( false );
	SendPartyCommandResult(_player, 0, "", ERR_PARTY_NO_ERROR);
}

void WorldSession::HandleGroupChangeSubGroup(WorldPacket & recv_data)
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	std::string name;
	uint8 subGroup;

	recv_data >> name;
	recv_data >> subGroup;

	PlayerInfo * inf = objmgr.GetPlayerInfoByName(name.c_str());
	if(inf == NULL || inf->m_Group == NULL || inf->m_Group != _player->m_playerInfo->m_Group)
	{ 
		return;
	}

	_player->GetGroup()->MovePlayer(inf, subGroup);
}

void WorldSession::HandleGroupAssistantLeader(WorldPacket & recv_data)
{
	uint64 guid;
	uint8 on;

	if(!_player->IsInWorld())
	{ 
		return;
	}

	if(_player->GetGroup() == NULL)
	{ 
		return;
	}

	if ( _player->GetGroup()->GetLeader() != _player->m_playerInfo )   //access denied
	{
		SendPartyCommandResult(_player, 0, "", ERR_PARTY_YOU_ARE_NOT_LEADER);
		return;
	}

	recv_data >> guid >> on;
	if(on == 0)
        _player->GetGroup()->SetAssistantLeader(NULL);
	else
	{
		PlayerInfo * np = objmgr.GetPlayerInfo((uint32)guid);
		if(np==NULL)
			_player->GetGroup()->SetAssistantLeader(NULL);
		else
		{
			if(_player->GetGroup()->HasMember(np))
				_player->GetGroup()->SetAssistantLeader(np);
		}
	}
}

void WorldSession::HandleGroupPromote(WorldPacket& recv_data)
{
	uint8 promotetype, on;
	uint64 guid;

	if(!_player->IsInWorld())
	{ 
		return;
	}

	if(_player->GetGroup() == NULL)
	{ 
		return;
	}

	if ( _player->GetGroup()->GetLeader() != _player->m_playerInfo )   //access denied
	{
		SendPartyCommandResult(_player, 0, "", ERR_PARTY_YOU_ARE_NOT_LEADER);
		return;
	}

	recv_data >> promotetype >> on;
	recv_data >> guid;

	void(Group::*function_to_call)(PlayerInfo*);
	function_to_call = 0;

	if(promotetype == 0)
		function_to_call = &Group::SetMainTank;
	else if(promotetype==1)
		function_to_call = &Group::SetMainAssist;

	if(on == 0)
		(_player->GetGroup()->*function_to_call)(NULL);
	else
	{
		PlayerInfo * np = objmgr.GetPlayerInfo((uint32)guid);
		if(np==NULL)
			(_player->GetGroup()->*function_to_call)(NULL);
		else
		{
			if(_player->GetGroup()->HasMember(np))
				(_player->GetGroup()->*function_to_call)(np);
		}
	}
}

void WorldSession::HandleRequestRaidInfoOpcode(WorldPacket & recv_data)
{  
	//		  SMSG_RAID_INSTANCE_INFO			 = 716,  //(0x2CC)	
	//sInstanceSavingManager.BuildRaidSavedInstancesForPlayer(_player);
	sInstanceMgr.BuildRaidSavedInstancesForPlayer(_player);
}

void WorldSession::HandleReadyCheckOpcode(WorldPacket& recv_data)
{
	Group * pGroup  = _player->GetGroup();
	WorldPacket data(MSG_RAID_READY_CHECK, 20);
	uint8 ready;

	if(!pGroup || ! _player->IsInWorld())
	{ 
		return;
	}

	if(recv_data.size() == 0)
	{
		if( pGroup->GetLeader() == _player->m_playerInfo || pGroup->GetAssistantLeader() == _player->m_playerInfo )
		{
			/* send packet to group */
			data.Initialize( MSG_RAID_READY_CHECK );
			data << _player->GetGUID();
			pGroup->SendPacketToAllButOne(&data, NULL);
		}
		else
		{
			SendNotification(NOTIFICATION_MESSAGE_NO_PERMISSION);
		}
	}
	else
	{
		if(_player->m_playerInfo != pGroup->GetLeader())
		{
			recv_data >> ready;

			data.Initialize( MSG_RAID_READY_CHECK_CONFIRM );
			data << _player->GetGUID();
			data << ready;
			pGroup->SendPacketToAllButOne(&data, NULL);

//			if(pGroup->GetLeader() && pGroup->GetLeader()->m_loggedInPlayer)
//				pGroup->GetLeader()->m_loggedInPlayer->GetSession()->SendPacket(&data);
		}
	}
}


void WorldSession::HandleRoleCheckOpcode(WorldPacket& recv_data)
{
	Group * pGroup  = _player->GetGroup();

	if(!pGroup || ! _player->IsInWorld())
	{ 
		return;
	}

/*
{SERVER} Packet: (0x6672) SMSG_GUILD_EVENT PacketSize = 16 TimeStamp = 10835002
10 01 51 75 63 69 6B 00 
23 A4 0F 01 00 00 80 01 

{SERVER} Packet: (0xCB41) SMSG_ROLE_POLL_BEGIN PacketSize = 7 TimeStamp = 10846577
ED -> guid mask ?
00 - go4 ?
00 - go7 ?
A5 - go1
81 - go6
22 - go0
0E - go2
-> obfuscated guid ? 

{SERVER} Packet: (0x4321) SMSG_ROLE_CHANGED_INFORM PacketSize = 22 TimeStamp = 10850961
87 FF 
0E 
00 
22 
00 
81 
22
00 00 00 00 -> old role ?
A5 
00 
0E 
08 00 00 00 -> new role ?
00 
81 
A5 
*/
	uint8 guid[8];
	*(uint64*)guid = _player->GetGUID();

/*	WorldPacket data(SMSG_ROLE_POLL_BEGIN, 20);
	data << uint8( 0xED );	//obfuscated leader guid mask
	data << ObfuscateByte( guid[4] );	//maybe 7 ?
	data << ObfuscateByte( guid[7] );
	data << ObfuscateByte( guid[1] );
	data << ObfuscateByte( guid[6] );
	data << ObfuscateByte( guid[0] );
	data << ObfuscateByte( guid[2] );
	*/
	WorldPacket data(SMSG_ROLE_POLL_BEGIN, 20);
	data << uint8( 0 );	//obfuscated leader guid mask
	pGroup->SendPacketToAll( &data );

}

