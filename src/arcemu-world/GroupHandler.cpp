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

//////////////////////////////////////////////////////////////
/// This function handles CMSG_GROUP_INVITE
//////////////////////////////////////////////////////////////
void WorldSession::HandleGroupInviteOpcode( WorldPacket & recv_data )
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	CHECK_PACKET_SIZE(recv_data, 1);
	std::string membername;
	Player * player = NULL;
	Group *group = NULL;

	recv_data.read_skip( 10 );	//no idea, they were all 0
	recv_data >> membername;

	//tried to make sure the other guy can't accept the invited
/*	if( _player->HasBeenInvited() )
	{ 
		return;
	} */

	player = objmgr.GetPlayer(membername.c_str(), false);

	if ( player == NULL)
	{
		SendPartyCommandResult(_player, 0, membername, ERR_PARTY_CANNOT_FIND);
		return;
	}

	if (player == _player)
	{
		return;
	}

	if ( _player->InGroup() && !_player->IsGroupLeader() )
	{
		SendPartyCommandResult(_player, 0, "", ERR_PARTY_YOU_ARE_NOT_LEADER);
		return;
	}

	group = _player->GetGroup();
	if( group != NULL )
	{
		if(group->IsFull())
		{
			SendPartyCommandResult(_player, 0, "", ERR_PARTY_IS_FULL);
			return;
		}
	}

	if ( player->InGroup() )
	{
		SendPartyCommandResult(_player, player->GetGroup()->GetGroupType(), membername, ERR_PARTY_ALREADY_IN_GROUP);
		return;
	}
	
	if(player->GetTeam()!=_player->GetTeam() && _player->GetSession()->GetPermissionCount() == 0 && !sWorld.interfaction_group)
	{
		SendPartyCommandResult(_player, 0, membername, ERR_PARTY_WRONG_FACTION);
		return;
	}

	//tried to make sure the other guy can't accept the invited
/*	if ( player->HasBeenInvited() )
	{
		SendPartyCommandResult(_player, 0, membername, ERR_PARTY_ALREADY_IN_GROUP);
		return;
	}*/

	if( player->Social_IsIgnoring( _player->GetLowGUID() ) )
	{
		SendPartyCommandResult(_player, 0, membername, ERR_PARTY_IS_IGNORING_YOU);
		return;
	}

/*
{SERVER} Packet: (0x1F7F) SMSG_GROUP_INVITE PacketSize = 16 TimeStamp = 30686468
01 
55 64 72 65 61 00 
AA 2A 00 00 
00 00 00 00 
00 

14333
{SERVER} Packet: (0x4723) UNKNOWN PacketSize = 44 TimeStamp = 17757047
4E E0 1001110	11100000
6F 00 00 00 
03 go3
00 00 00 00 
00 00 00 00 
4C go2
44 75 6E 65 6D 61 75 6C 00 Dunemaul 
00 00 00 00 
41 6E 61 72 6B 65 00 		Anarke 
00 1F 59 15 
00 go7
AF go1
81 go6
EB go0

{SERVER} Packet: (0xFA67) SMSG_NAME_QUERY_RESPONSE PacketSize = 20 TimeStamp = 17676894
CF EA AE 4D 02 80 01 00 41 6E 61 72 6B 65 00 00 02 00 06 00 

{SERVER} Packet: (0x4723) UNKNOWN PacketSize = 46 TimeStamp = 18686064
4E E0 
6F 00 00 00 
00 
00 00 00 00 
42 00 00 00 
81 
44 75 6E 65 6D 61 75 6C 00 
00 00 00 00 
44 72 61 69 73 69 75 73 00 
00 
69 E3 1F 00 B0 81 F7 

11001111
{SERVER} Packet: (0xFA67) SMSG_NAME_QUERY_RESPONSE PacketSize = 22 TimeStamp = 18570343
CF F6 B1 80 01 80 01 00 44 72 61 69 73 69 75 73 00 00 0A 00 03 00
*/
	uint64 inviter_guid = _player->GetGUID();
	uint8 *guid_bytes = (uint8*)&inviter_guid;
	uint16 guid_mask = 0;

	//!!not confirmed, but seems to work !
/*	if( guid_bytes[0] )
		guid_mask |= BIT_0;
	if( guid_bytes[1] )
		guid_mask |= BIT_1;
	if( guid_bytes[2] )
		guid_mask |= BIT_2;
	if( guid_bytes[3] )
		guid_mask |= BIT_3;*/
	guid_mask = 0xE04E;

	sStackWorldPacket( data,SMSG_GROUP_INVITE, 100);
	data << uint16(guid_mask);//guid mask
	data << uint32( 0 );	//?
//	if( guid_bytes[3] )
		data << ObfuscateByte( guid_bytes[3] );
	data << uint32( 0x00000000 );
	data << uint32( 0x00000000 );
//	if( guid_bytes[2] )
		data << ObfuscateByte( guid_bytes[2] );

	data << "";	//realm name
	data << uint32( 0x00000000 );
	data << GetPlayer()->GetName();

	data << uint32( 0 );

//	if( guid_bytes[7] )
		data << ObfuscateByte( guid_bytes[7] );
//	if( guid_bytes[1] )
		data << ObfuscateByte( guid_bytes[1] );
//	if( guid_bytes[6] )
		data << ObfuscateByte( guid_bytes[6] );
//	if( guid_bytes[0] )
		data << ObfuscateByte( guid_bytes[0] );

	player->GetSession()->SendPacket(&data);

	uint32 gtype = 0;
	if(group)
		gtype = group->GetGroupType();

	SendPartyCommandResult(_player, gtype, membername, ERR_PARTY_NO_ERROR);

	// 16/08/06 - change to guid to prevent very unlikely event of a crash in deny, etc
	player->SetInviter( _player->GetLowGUID() );
	// 16/08/06 - change to guid to prevent very unlikely event of a crash in deny, etc
	_player->SetInviter( player->GetLowGUID() );//bugfix if player invtied 2 people-> he can be in 2 parties

}

///////////////////////////////////////////////////////////////
///This function handles CMSG_GROUP_CANCEL:
///////////////////////////////////////////////////////////////
void WorldSession::HandleGroupCancelOpcode( WorldPacket & recv_data )
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	sLog.outDebug( "WORLD: got CMSG_GROUP_CANCEL." );
}

////////////////////////////////////////////////////////////////
///This function handles CMSG_GROUP_ACCEPT and deny:
////////////////////////////////////////////////////////////////
void WorldSession::HandleGroupAcceptOpcode( WorldPacket & recv_data )
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}

	uint8 packet_type;
	recv_data >> packet_type;
	if( packet_type == 0 ) //0x80 for group accept and 0 is for decline
	{
		HandleGroupDeclineOpcode( recv_data );
		return;
	}

	Player *player = objmgr.GetPlayer(_player->GetInviter());
	if ( !player )
	{ 
		return;
	}

	//check if this invite is still valid
	if( player->GetInviter() != _player->GetLowGUID() || _player->GetInviter() != player->GetLowGUID() )
	{
		_player->BroadcastMessage( "Group invite from player %s expired", player->GetName() );
		player->BroadcastMessage( "Group invite from player %s expired", _player->GetName() );
		return;
	}
	
	player->SetInviter(0);
	_player->SetInviter(0);
	
	Group *grp = player->GetGroup();

	if(grp)
	{
		grp->AddMember(_player->m_playerInfo);
		_player->dungeon_difficulty = grp->dungeon_difficulty;
		_player->raid_difficulty = grp->raid_difficulty;
		_player->SendDungeonDifficulty();
		_player->SendRaidDifficulty();

        //sInstanceSavingManager.ResetSavedInstancesForPlayer(_player);
		return;
	}
	
	// If we're this far, it means we have no existing group, and have to make one.
	grp = new Group(true);
	grp->dungeon_difficulty = player->dungeon_difficulty;
	grp->raid_difficulty = player->raid_difficulty;
	grp->AddMember(player->m_playerInfo);		// add the inviter first, therefore he is the leader
	grp->AddMember(_player->m_playerInfo);	   // add us.
	_player->dungeon_difficulty = grp->dungeon_difficulty;
	_player->raid_difficulty = grp->raid_difficulty;
	_player->SendDungeonDifficulty();
	_player->SendRaidDifficulty();

	Instance *instance = sInstanceMgr.GetInstanceByIds(player->GetMapId(), player->GetInstanceID());
	if(instance != NULL && instance->m_creatorGuid == player->GetLowGUID())
	{
		grp->AddSavedInstance(instance->m_mapId,instance->instance_difficulty,instance->m_instanceId);
		instance->m_creatorGroup = grp->GetID();
		instance->m_creatorGuid = 0;
		instance->SaveToDB();
	}

    //sInstanceSavingManager.ResetSavedInstancesForPlayer(_player);

	// Currentgroup and all that shit are set by addmember.
}

///////////////////////////////////////////////////////////////////////////////////////
///This function handles CMSG_GROUP_DECLINE:
//////////////////////////////////////////////////////////////////////////////////////
void WorldSession::HandleGroupDeclineOpcode( WorldPacket & recv_data )
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	sStackWorldPacket( data, SMSG_GROUP_DECLINE, 100);

	Player *player = objmgr.GetPlayer(_player->GetInviter());
	if(!player) 
	{ 
		return;
	}

	data << GetPlayer()->GetName();

	player->GetSession()->SendPacket( &data );

	if( player->GetInviter() == _player->GetLowGUID() )
		player->SetInviter(0);
	if( _player->GetInviter() == player->GetLowGUID() )
		_player->SetInviter(0);
}

//////////////////////////////////////////////////////////////////////////////////////////
///This function handles CMSG_GROUP_UNINVITE:
//////////////////////////////////////////////////////////////////////////////////////////
void WorldSession::HandleGroupUninviteOpcode( WorldPacket & recv_data )
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	CHECK_PACKET_SIZE(recv_data, 1);
	std::string membername;
	Player *player;
	PlayerInfo * info;

	recv_data >> membername;

	player = objmgr.GetPlayer(membername.c_str(), false);
	info = objmgr.GetPlayerInfoByName(membername.c_str());
	_player->GroupUninvite(player, info);
}

//////////////////////////////////////////////////////////////////////////////////////////
///This function handles CMSG_GROUP_UNINVITE_GUID:
//////////////////////////////////////////////////////////////////////////////////////////
void WorldSession::HandleGroupUninviteGUIDOpcode( WorldPacket & recv_data )
{
	sLog.outDebug( "WORLD: got CMSG_GROUP_UNINVITE_GUID." );
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	CHECK_PACKET_SIZE(recv_data, 1);
	std::string membername;
	Player *player;
	PlayerInfo * info;
	uint64 guid;
	recv_data >> guid;

	player = objmgr.GetPlayer( guid );
	info = objmgr.GetPlayerInfo(guid);

	_player->GroupUninvite(player, info);
}

//////////////////////////////////////////////////////////////////////////////////////////
///This function handles CMSG_GROUP_SET_LEADER:
//////////////////////////////////////////////////////////////////////////////////////////
void WorldSession::HandleGroupSetLeaderOpcode( WorldPacket & recv_data )
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	// important note _player->GetName() can be wrong.
	CHECK_PACKET_SIZE(recv_data, 1);
	uint64 MemberGuid;
	Player * player;

	recv_data >> MemberGuid;
	
	player = objmgr.GetPlayer((uint32)MemberGuid);

	if ( player == NULL )
	{
		//SendPartyCommandResult(_player, 0, membername, ERR_PARTY_CANNOT_FIND);
		SendPartyCommandResult(_player, 0, _player->GetName(), ERR_PARTY_CANNOT_FIND);
		return;
	}

	if(!_player->IsGroupLeader())
	{
		SendPartyCommandResult(_player, 0, "", ERR_PARTY_YOU_ARE_NOT_LEADER);
		return;
	}
	
	if(player->GetGroup() != _player->GetGroup())
	{
		//SendPartyCommandResult(_player, 0, membername, ERR_PARTY_IS_NOT_IN_YOUR_PARTY);
		SendPartyCommandResult(_player, 0, _player->GetName(), ERR_PARTY_IS_NOT_IN_YOUR_PARTY);
		return;
	}

	Group *pGroup = _player->GetGroup();
	if(pGroup)
		pGroup->SetLeader(player,false);
}

//////////////////////////////////////////////////////////////////////////////////////////
///This function handles CMSG_GROUP_DISBAND:
//////////////////////////////////////////////////////////////////////////////////////////
void WorldSession::HandleGroupDisbandOpcode( WorldPacket & recv_data )
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	Group* pGroup = _player->GetGroup();
	if(!pGroup) 
	{ 
		return;
	}

	// cant leave a battleground group (blizzlike 3.3.3)
	if( pGroup->GetGroupType() & GROUP_TYPE_BG )
		return;

	//pGroup->Disband();
	pGroup->RemovePlayer(_player->m_playerInfo);
}

//////////////////////////////////////////////////////////////////////////////////////////
///This function handles CMSG_LOOT_METHOD:
//////////////////////////////////////////////////////////////////////////////////////////
void WorldSession::HandleLootMethodOpcode( WorldPacket & recv_data )
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	CHECK_PACKET_SIZE(recv_data, 16);
	uint32 lootMethod;
	uint64 lootMaster;
	uint32 threshold;

	recv_data >> lootMethod >> lootMaster >>threshold;
  
	if(!_player->IsGroupLeader())
	{
		SendPartyCommandResult(_player, 0, "", ERR_PARTY_YOU_ARE_NOT_LEADER);
		return;
	}
	
	Group* pGroup = _player->GetGroup(); 

	if( pGroup == NULL)
	{ 
		return;
	}

	Player * pLootMaster = objmgr.GetPlayer((uint32)lootMaster);

	if ( pLootMaster )
		pGroup->SetLooter(pLootMaster , lootMethod, threshold );
	else
		pGroup->SetLooter(_player , lootMethod, threshold );
}

void WorldSession::HandleMinimapPingOpcode( WorldPacket & recv_data )
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	CHECK_PACKET_SIZE(recv_data, 8);
	if(!_player->InGroup())
	{ 
	return;
	}
	Group * party= _player->GetGroup();
	if(!party)
	{ 
		return;
	}

	float x,y;
	recv_data >> x >>y;
	WorldPacket data( MSG_MINIMAP_PING, 20 );
	data << _player->GetGUID();
	data << x << y;
	party->SendPacketToAllButOne(&data, _player);
}

void WorldSession::HandleSetPlayerIconOpcode(WorldPacket& recv_data)
{
	/*
{CLIENT} Packet: (0x0321) MSG_RAID_TARGET_UPDATE PacketSize = 9 TimeStamp = 40891325
07 
FB B8 05 D6 16 00 30 F1 
{SERVER} Packet: (0x0321) MSG_RAID_TARGET_UPDATE PacketSize = 18 TimeStamp = 40891715
00 
D3 EC 28 00 00 00 00 04 
07 
FB B8 05 D6 16 00 30 F1 
*/
	uint64 guid;
	uint8 icon;
	Group * pGroup = _player->GetGroup();
	if(!_player->IsInWorld() || !pGroup) 
	{ 
		return;
	}

	recv_data >> icon;
	if(icon == 0xFF)
	{
		// client request
		sStackWorldPacket( data, MSG_RAID_TARGET_UPDATE, 200 );
		data << uint8(1);
		for(uint8 i = 0; i < 8; ++i)
			data << i << pGroup->m_targetIcons[i];

		SendPacket(&data);
	}
	else if(_player->IsGroupLeader())
	{
		recv_data >> guid;
		if(icon > 7)
			return;			// whhopes,buffer overflow :p

		// setting icon
		WorldPacket data( MSG_RAID_TARGET_UPDATE, 20);
		data << uint8(0) << _player->GetGUID();
		data << icon << guid;
		pGroup->SendPacketToAll(&data);

		pGroup->m_targetIcons[icon] = guid;
	}
}

void WorldSession::SendPartyCommandResult(Player *pPlayer, uint32 p1, std::string name, uint32 err)
{
/*
{SERVER} Packet: (0x4F7D) SMSG_PARTY_COMMAND_RESULT PacketSize = 26 TimeStamp = 30918515
00 00 00 00 - gorup type ?
55 64 72 65 61 00 - name
00 00 00 00 - no error
00 00 00 00 ?
00 00 00 00 ?
00 00 00 00 ?
*/
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	// if error message do not work, please sniff it and leave me a message
	if(pPlayer)
	{
		sStackWorldPacket( data, SMSG_PARTY_COMMAND_RESULT, 100 );
		data << p1;
		if(!name.length())
			data << uint8(0);
		else
			data << name.c_str();

		data << err;
		data << uint32( 0 );
		data << uint32( 0 );
		data << uint32( 0 );
		pPlayer->GetSession()->SendPacket(&data);
	}
}

void WorldSession::HandleSetPlayerRole( WorldPacket & recv_data )
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	sLog.outDebug( "WORLD: got MSG_GROUP_SET_ROLE." );
	//02 guid mask ?
	//08 00 00 00 role
	//4F guid ?
	//ex2 : 1E 08 00 00 00 07 05 03 02  
/*
14333
{CLIENT} Packet: (0x6626) CMSG_SET_SELECTION PacketSize = 8 TimeStamp = 50545618
3B 08 1B 01 00 00 80 06 
3A 09 1A 00 00 00 81 07

{CLIENT} Packet: (0x1011) MSG_GROUP_SET_ROLE PacketSize = 11 TimeStamp = 50549783
5F 
08 00 00 00 
3A 09 81 00 1A 07 

{SERVER} Packet: (0x4321) SMSG_ROLE_CHANGED_INFORM PacketSize = 22 TimeStamp = 50550017
87 FF mask
1A go2
07 go7
3A go0
00 go3
81 go6
3A go0
00 00 00 00 old role ?
09 go1
07 go7
1A go2
08 00 00 00 new role
00 go3
81 go6
09 go1
*/
	uint8	guid_mask;
	uint32	new_role;
	recv_data >> guid_mask;
	recv_data >> new_role;
	//i think rest of the packet is guid based on mask

	//tell the group of our setup
	WorldPacket data( SMSG_ROLE_CHANGED_INFORM, 50);
	uint64 GUID = _player->GetGUID();
	uint8 *guid_bytes = (uint8 *)&GUID;
	uint16 mask = 0x6B83;

	data << mask;

//	if( guid_bytes[2] )
		data << ObfuscateByte( guid_bytes[2] );
//	if( guid_bytes[0] )
		data << ObfuscateByte( guid_bytes[0] );
//	if( guid_bytes[3] )
		data << ObfuscateByte( guid_bytes[3] );
//	if( guid_bytes[0] )
		data << ObfuscateByte( guid_bytes[0] );

	data << uint32( _player->GroupPlayerRole );

//	if( guid_bytes[1] )
		data << ObfuscateByte( guid_bytes[1] );
//	if( guid_bytes[2] )
		data << ObfuscateByte( guid_bytes[2] );

	data << uint32( new_role );

//	if( guid_bytes[3] )
		data << ObfuscateByte( guid_bytes[3] );
//	if( guid_bytes[1] )
		data << ObfuscateByte( guid_bytes[1] );
	
	_player->GroupPlayerRole  = new_role;

	if( _player->GetGroup() )
	{
		_player->GetGroup()->SendPacketToAll(&data);
		_player->GetGroup()->Update();
	}
	else
		SendPacket( &data );
}

void WorldSession::HandleRaidMarkerClear( WorldPacket & recv_data )
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	uint8 guid_mask;
	uint8 clear_slot;

	recv_data >> guid_mask;
	recv_data >> clear_slot;	//5 means all
	//rest of the packet is the obfuscated guid mask

	sLog.outDebug( "WORLD: got CMSG_RAID_MARKER_CLEAR." );
	if( _player->GetGroup() && _player->IsGroupLeader() )
		_player->GetGroup()->ClearRaidMarkers( clear_slot );
}

void WorldSession::HandleRaidUnkQuery( WorldPacket & recv_data )
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	sLog.outDebug( "WORLD: got CMSG_RAID_UNK_STATUS_QUERY." );
	sLog.outDebug( "!!!! temp hack funtion to test some functionality in 14333. Remove later !!!" );
/*
14333
//these all seem to come at once
{CLIENT} Packet: (0xB633) UNKNOWN PacketSize = 8 TimeStamp = 11485152
D7 74 3E 04 00 00 80 01 
{CLIENT} Packet: (0x3226) UNKNOWN PacketSize = 0 TimeStamp = 11485152
{CLIENT} Packet: (0x50D0) UNKNOWN PacketSize = 0 TimeStamp = 11485152
{CLIENT} Packet: (0x7622) CMSG_MEETINGSTONE_INFO PacketSize = 0 TimeStamp = 11485168
{CLIENT} Packet: (0xB6E3) UNKNOWN PacketSize = 0 TimeStamp = 11485168
{CLIENT} Packet: (0x0460) UNKNOWN PacketSize = 12 TimeStamp = 11485168
8A 13 00 00 00 00 D6 3F 81 75 05 00 
{CLIENT} Packet: (0xB8E3) UNKNOWN PacketSize = 0 TimeStamp = 11485168
{CLIENT} Packet: (0x2848) UNKNOWN PacketSize = 11 TimeStamp = 11485168
3F 47 09 00 00 3F D6 05 75 81 00 
{CLIENT} Packet: (0x2848) UNKNOWN PacketSize = 11 TimeStamp = 11485168
3F FA 00 00 00 3F D6 05 75 81 00 
{CLIENT} Packet: (0x0050) CMSG_RAID_UNK_STATUS_QUERY PacketSize = 0 TimeStamp = 11485339
{CLIENT} Packet: (0x7622) CMSG_MEETINGSTONE_INFO PacketSize = 0 TimeStamp = 11485339

//another sample
{CLIENT} Packet: (0x0050) UNKNOWN PacketSize = 0 TimeStamp = 8067435
{CLIENT} Packet: (0x7622) CMSG_MEETINGSTONE_INFO PacketSize = 0 TimeStamp = 8067435
{SERVER} Packet: (0x7A27) SMSG_LFG_UPDATE_PLAYER PacketSize = 2 TimeStamp = 8067732
0E 00 
{SERVER} Packet: (0x66FA) MSG_RAID_TARGET_UPDATE PacketSize = 1 TimeStamp = 8068153
01 
{SERVER} Packet: (0x9341) SMSG_RAID_MARKER_STATUS PacketSize = 4 TimeStamp = 8068153
00 00 00 00 
{SERVER} Packet: (0x666E) SMSG_LFG_UPDATE_PARTY PacketSize = 2 TimeStamp = 8068153
0E 00 
{SERVER} Packet: (0x2A3F) SMSG_UPDATE_WORLD_STATE PacketSize = 9 TimeStamp = 8074284
57 14 00 00 06 00 00 00 00 
{SERVER} Packet: (0x2A3F) SMSG_UPDATE_WORLD_STATE PacketSize = 9 TimeStamp = 8074689
58 14 00 00 03 00 00 00 00 
*/
/*	sStackWorldPacket( data, 0x7A27, 9 + 2 + 1 + 4 + 2 );
	data << uint16( 0x000E );
	SendPacket( &data );

	data.Initialize( 0x66FA );
	data << uint8( 1 );
	SendPacket( &data );

	data.Initialize( 0x9341 );
	data << uint32( 0 );
	SendPacket( &data );

	data.Initialize( 0x666E );
	data << uint16( 0x000E );
	SendPacket( &data );

	data.Initialize( 0x2A3F );
	data << uint32( 0x00001457 );
	data << uint32( 0x00000006 );
	data << uint8( 0 );
	SendPacket( &data );

	data.Initialize( 0x2A3F );
	data << uint32( 0x00001458 );
	data << uint32( 0x00000003 );
	data << uint8( 0 );
	SendPacket( &data );/**/
}