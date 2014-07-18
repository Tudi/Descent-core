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

void WorldSession::HandleGuildQuery(WorldPacket & recv_data)
{
	CHECK_PACKET_SIZE(recv_data, 4);
/*
13329
{CLIENT} Packet: (0xDC55) CMSG_GUILD_QUERY PacketSize = 16 TimeStamp = 146963140
DB 30 00 00 00 80 F1 1F - guild GUID
BB 6A BF 03 00 00 80 01 - guild member
*/

	uint64 guildGUID;
	uint64 guildMember;
	recv_data >> guildGUID;
	recv_data >> guildMember;

	uint32 guildId = uint32( guildGUID );
/*	if( guildMember != _player->GetGUID() )
	{
		sLog.outDebug("Client asked for guild info that is not our guild. We are not going to answer even if blizz does so");
		return;
	}

	// we can skip some searches here if this is our guild
	if(_player && _player->GetGuildId() == guildId && _player->m_playerInfo->guild) 
	{
		_player->m_playerInfo->guild->SendGuildQuery(this);
		return;
	} */
	
	Guild * pGuild = objmgr.GetGuild( guildId );
	if(!pGuild)
	{ 
		return;
	}

	pGuild->SendGuildQuery( this );
}

void WorldSession::HandleCreateGuild(WorldPacket & recv_data)
{
}

/*void WorldSession::SendGuildCommandResult(uint32 typecmd,const char *  str,uint32 cmdresult)
{
	WorldPacket data;
	data.SetOpcode(SMSG_GUILD_COMMAND_RESULT);
	data << typecmd;
	data << str;
	data << cmdresult;
	SendPacket(&data);
}*/

void WorldSession::HandleInviteToGuild(WorldPacket & recv_data)
{
	CHECK_PACKET_SIZE(recv_data, 1);
/*
13329
{CLIENT} Packet: (0x277C) CMSG_GUILD_INVITE PacketSize = 8 stamp = 6783391
|------------------------------------------------|----------------|
|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
|------------------------------------------------|----------------|
|73 6D 61 73 68 65 72 00                         |smasher.        |
-------------------------------------------------------------------
{SERVER} Packet: (0x38A0) UNKNOWN PacketSize = 66 TimeStamp = 16026700
0E 00 00 00 1D E9 C6 00 00 80 F1 1F 00 09 00 00 00 1A 00 00 00 01 00 00 00 21 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 44 65 74 68 72 6F 6E 65 20 52 6F 79 61 6C 74 79 00 52 75 6E 61 7A 7A 69 00 
0E 00 00 00 
1D E9 C6 00 00 80 F1 1F guildid
00 09 00 00 00 1A 00 00 00 01 00 00 00 21 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 44 65 74 68 72 6F 6E 65 20 52 6F 79 61 6C 74 79 00 52 75 6E 61 7A 7A 69 00 

*/
	std::string inviteeName;
	recv_data >> inviteeName;

	Player *plyr = objmgr.GetPlayer( inviteeName.c_str() , false);
	Guild *pGuild = _player->m_playerInfo->guild;
	
	if(!plyr)
	{
		Guild::SendGuildCommandResult(this, GUILD_INVITE_S,inviteeName.c_str(),GUILD_PLAYER_NOT_FOUND);
		return;
	}
	else if(!pGuild)
	{
		Guild::SendGuildCommandResult(this, GUILD_CREATE_S,"",GUILD_PLAYER_NOT_IN_GUILD);
		return;
	}

	if( plyr->GetGuildId() )
	{
		Guild::SendGuildCommandResult(this, GUILD_INVITE_S,plyr->GetName(),ALREADY_IN_GUILD);
		return;
	}
	else if( plyr->GetGuildInvitersGuid())
	{
		Guild::SendGuildCommandResult(this, GUILD_INVITE_S,plyr->GetName(),ALREADY_INVITED_TO_GUILD);
		return;
	}
	else if(!_player->m_playerInfo->guildRank->CanPerformCommand(GR_RIGHT_INVITE))
	{
		Guild::SendGuildCommandResult(this, GUILD_INVITE_S,"",GUILD_PERMISSIONS);
		return;
	}
	else if(plyr->GetTeam()!=_player->GetTeam() && _player->GetSession()->GetPermissionCount() == 0 && !sWorld.interfaction_guild)
	{
		Guild::SendGuildCommandResult(this, GUILD_INVITE_S,"",GUILD_NOT_ALLIED);
		return;
	}
	else if( pGuild->GetNumMembers() >= 700 )
	{
		plyr->BroadcastMessage(" Guild Invite : Guild contains too many members and might crash client ");
		return;
	}
	Guild::SendGuildCommandResult(this, GUILD_INVITE_S,inviteeName.c_str(),GUILD_U_HAVE_INVITED);
	pGuild->AddGuildLogEntry(GUILD_LOG_EVENT_INVITE, 2, (uint32)( GetPlayer()->GetGUID() ), (uint32)( plyr->GetGUID() ) );
	//41
  /*
13329
{SERVER} Packet: (0x38A0) SMSG_GUILD_INVITE PacketSize = 53 TimeStamp = 13561463
0E 00 00 00 m_emblemColor
CA 7E A9 00 00 00 F3 1F 
00 - ?
05 00 00 00 m_borderStyle
20 00 00 00 m_backgroundColor
01 00 00 00 - maybe inviter level
71 00 00 00 m_emblemStyle
00 00 00 00 
00 00 00 00 
05 00 00 00 m_borderColor
52 61 74 73 00 - guildname
42 61 6E 6B 79 65 00 - player
14333
{SERVER} Packet: (0x9F65) SMSG_GUILD_INVITE PacketSize = 55 TimeStamp = 8801888
05 52 mask 
00 00 00 00 ?
55 73 65 72 78 00 Userx name
0F 00 00 00 ?
05 00 00 00 ?
0F 00 00 00 ?
05 00 00 00 ?
07 go2
1E go7
8A 00 00 00 emblem
86 go0
41 20 4D 75 72 64 65 72 20 6F 66 20 46 6F 65 73 00 A Murder of Foes guild name
F3 go6
00 
2B go1
*/
	uint8 gguid[8];
	*(uint64*)gguid = pGuild->GetGuildId();

	WorldPacket data(SMSG_GUILD_INVITE, 100);
	data << uint16( 0x5205 );	//guild guid mask
	data << pGuild->GetPropEmblemColor(); //?
	data << _player->GetName();
	data << pGuild->GetPropEmblemColor(); //?
	data << pGuild->GetPropEmblemColor(); //?
	data << pGuild->GetPropEmblemColor(); //?
	data << pGuild->GetPropEmblemColor(); //?
	data << ObfuscateByte( gguid[2] );
	data << ObfuscateByte( gguid[7] );
	data << pGuild->GetPropEmblemStyle();	
	data << ObfuscateByte( gguid[0] );
	data << pGuild->GetGuildName();		
	data << ObfuscateByte( gguid[6] );
	data << uint8( 0 ); 
	data << ObfuscateByte( gguid[1] );
	plyr->GetSession()->SendPacket(&data);

	plyr->SetGuildInvitersGuid( _player->GetLowGUID() );	
//	data << uint32( pGuild->GetLevel() + 20 );
//	data << uint32( pGuild->GetNumMembers() + 20);	
}

void WorldSession::HandleGuildAccept(WorldPacket & recv_data)
{
	Player *plyr = GetPlayer();

	if(!plyr)
	{ 
		return;
	}

	Player *inviter = objmgr.GetPlayer( plyr->GetGuildInvitersGuid() );
	plyr->UnSetGuildInvitersGuid();

	if(!inviter)
	{
		return;
	}

	Guild *pGuild = inviter->m_playerInfo->guild;
	if(!pGuild)
	{
		return;
	}

	pGuild->AddGuildMember(plyr->m_playerInfo, NULL);
}

void WorldSession::HandleGuildDecline(WorldPacket & recv_data)
{
	WorldPacket data;

	Player *plyr = GetPlayer();

	if(!plyr)
	{ 
		return;
	}

	Player *inviter = objmgr.GetPlayer( plyr->GetGuildInvitersGuid() );
	plyr->UnSetGuildInvitersGuid(); 

	if(!inviter)
	{ 
		return;
	}

/*
13329
{SERVER} Packet: (0x6F74) SMSG_GUILD_DECLINE PacketSize = 7 TimeStamp = 9673187
44 72 75 66 69 00 00 
*/
	data.Initialize(SMSG_GUILD_DECLINE);
	data << plyr->GetName();
	data << uint8( 0 );
	inviter->GetSession()->SendPacket(&data);
}

void WorldSession::HandleSetGuildInformation(WorldPacket & recv_data)
{
	CHECK_PACKET_SIZE(recv_data, 1);
/*
13329
{CLIENT} Packet: (0x9BC2) CMSG_GUILD_SET_INFO_TEXT PacketSize = 22 stamp = 5843891
|------------------------------------------------|----------------|
|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
|------------------------------------------------|----------------|
|45 00 00 00 00 00 00 00 0A 00 00 00 00 00 F1 1F |E...............|
|67 75 69 6E 66 00                               |guinf.          |
-------------------------------------------------------------------
14333
{CLIENT} Packet: (0x8461) CMSG_GUILD_SET_INFO_TEXT PacketSize = 16 stamp = 5894310
D5 E3 masks
F0 1E ?
67 75 69 00 text
08 14 02 12 06 18 04 16 - obfuscated guids

UPDATE guilds SET guildid=0x13 WHERE guildName='randomguidss';
UPDATE guild_ranks SET guildid=0x13 WHERE guildid=327680;
UPDATE guild_data SET guildid=0x13 WHERE playerid=38;

UPDATE guilds SET guildid=0x1500 WHERE guildName='randomguidss';
UPDATE guild_ranks SET guildid=0x1500 WHERE guildid=0x13;
UPDATE guild_data SET guildid=0x1500 WHERE playerid=38;

UPDATE guilds SET guildid=0x17000000 WHERE guildName='randomguidss';
UPDATE guild_ranks SET guildid=0x17000000 WHERE guildid=0x1500;
UPDATE guild_data SET guildid=0x17000000 WHERE playerid=38;

- pg0 + gg2
44 00 61 00 27 04
- pg1 + gg2
40 C0 61 00 03 04 25
- pg2 + gg2
50 00 61 00 23 04
- pg3 + gg2
40 41 61 00 05 21 04
- pg0 + gg0
04 40 61 00 12 27
- pg0 + gg1
84 00 61 00 14 27
- pg0 + gg2
44 00 61 00 27 04
- pg0 + gg3
04 02 61 00 16 27
*/
	uint8 guid_mask1;
	uint8 guid_mask2;
	std::string NewGuildInfo;

	recv_data >> guid_mask1;
	recv_data >> guid_mask2;
	recv_data >> NewGuildInfo;

	Guild *pGuild = _player->m_playerInfo->guild;
	if(!pGuild)
	{
		Guild::SendGuildCommandResult(this, GUILD_CREATE_S,"",GUILD_PLAYER_NOT_IN_GUILD);
		return;
	}

	pGuild->SetGuildInformation(NewGuildInfo.c_str(), this);
}

void WorldSession::HandleGuildInfo(WorldPacket & recv_data)
{
	if(_player->GetGuild() != NULL)
		_player->GetGuild()->SendGuildInfo(this);
}

void WorldSession::HandleGuildRoster(WorldPacket & recv_data)
{
	if( !_player || !_player->m_playerInfo || !_player->m_playerInfo->guild)
	{ 
		return;
	}

	_player->m_playerInfo->guild->SendGuildRoster(this);
}

void WorldSession::HandleGuildPromote(WorldPacket & recv_data)
{
	/*
{CLIENT} Packet: (0x9982) CMSG_GUILD_PROMOTE PacketSize = 16 stamp = 874869
|------------------------------------------------|----------------|
|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
|------------------------------------------------|----------------|
|CB 00 00 00 00 00 00 00 45 00 00 00 00 00 00 00 |........E.......|
-------------------------------------------------------------------
14333
{CLIENT} Packet: (0xA4A0) UNKNOWN PacketSize = 16 TimeStamp = 7152115
5D 6D 01 00 00 00 00 75 8C B6 03 A7 76 6A 03 05 
{CLIENT} Packet: (0xA4A0) UNKNOWN PacketSize = 16 TimeStamp = 7175406
5D 6D 03 00 00 00 00 75 8C B6 03 A7 76 6A 03 05 

guid inviter : A6 8D 6B 01 00 00 00 02 ->  A7 8C 6A 00 FF FF FF 03	
5D mask1
6D mask2
03 00 00 00 rank ?
00 goi[3]
75 
8C goi[1]
B6 
03 go[7] inviter or target
A7 goi[0]
76 
6A goi[2]
03 go[7] inviter or target
05 

gpt0
10 01 m1 &0x10
04 00 00 00 02 E2
gpt1
00 21 m2 &0x20
04 00 00 00 0E 02
gpt2
40 01 m1 &0x40
04 00 00 00 23 02
gpt3
04 01 m1 & 0x04
04 00 00 00 02 3E

gpp0
10 01 m2 &0x01
04 00 00 00 02 E2
gpp1
10 08 m2 &0x08
04 00 00 00 0E E2
gpp2
10 40 m2 &0x40
04 00 00 00 E2 1E
gpp3
10 04 m2 &0x04
03 00 00 00 3E E2
gpt01234
54 21 
05 00 00 00 
33 gpt2
35 gpt1
02 gpp0
37 gpt0
31 gpt3
gpt01234 gpp01234
54 6D 
04 00 00 00 
21 gpp3 m2 &0x04
33 gpt2 m1 &0x40
25 gpp1 m2 &0x08
35 gpt1 m2 &0x20
27 gpp0 m2 &0x01
37 gpt0 m1 &0x10
23 gpp2 m2 &0x40
31 gpt3 m1 &0x04
*/
	if( !_player->m_playerInfo->guild )
	{
		Guild::SendGuildCommandResult(this, GUILD_CREATE_S,"",GUILD_PLAYER_NOT_IN_GUILD);
		return;
	}

	uint8 guid_mask1;
	uint8 guid_mask2;
	uint32 new_rank;
	uint8 TargetPlayerGUID[8];
	uint8 PromoterPlayerGUID[8];
	*(uint64*)TargetPlayerGUID = 0;
	*(uint64*)PromoterPlayerGUID = 0;

	recv_data >> guid_mask1;
	recv_data >> guid_mask2;
	recv_data >> new_rank;

	if( guid_mask2 & BIT_0x04 )
		recv_data >> PromoterPlayerGUID[3];
	if( guid_mask1 & BIT_0x40 )
		recv_data >> TargetPlayerGUID[2];

	//not a mistake that it is same mask
	if( guid_mask2 & BIT_0x08 )
		recv_data >> PromoterPlayerGUID[1];
	if( guid_mask2 & BIT_0x20 )
		recv_data >> TargetPlayerGUID[1];

	if( guid_mask2 & BIT_0x01 )
		recv_data >> PromoterPlayerGUID[0]; 
	if( guid_mask1 & BIT_0x10 )
		recv_data >> TargetPlayerGUID[0];

	if( guid_mask2 & BIT_0x40 )
		recv_data >> PromoterPlayerGUID[2];
	if( guid_mask1 & BIT_0x04 )
		recv_data >> TargetPlayerGUID[3];

	GUID_un_obfuscate( TargetPlayerGUID );
	GUID_un_obfuscate( PromoterPlayerGUID ); //name of the variable could be wrong
	uint64 TheGuidWeWant;
	if( _player->GetGUID() == *(uint64*)TargetPlayerGUID )
		TheGuidWeWant = *(uint64*)PromoterPlayerGUID;
	else
		TheGuidWeWant = *(uint64*)TargetPlayerGUID;
	PlayerInfo * dstplr = objmgr.GetPlayerInfo( TheGuidWeWant );
	if(dstplr==NULL)
	{ 
		return;
	}

	if( _player->m_playerInfo->guild->IsValidRank( new_rank ) == false )
	{
		sLog.outDebug("Trying to set an invalid new rank. Exiting ");
		return;
	}

	if( dstplr->guildRank == NULL || new_rank < dstplr->guildRank->iId )
		_player->m_playerInfo->guild->PromoteGuildMember( dstplr, this, new_rank );
	else if( new_rank > dstplr->guildRank->iId )
		_player->m_playerInfo->guild->DemoteGuildMember( dstplr, this, new_rank );
}

/*
void WorldSession::HandleGuildDemote(WorldPacket & recv_data)
{
	CHECK_PACKET_SIZE(recv_data, 1);

	std::string name;
	recv_data >> name;

	if(!_player->m_playerInfo->guild)
	{
		Guild::SendGuildCommandResult(this, GUILD_CREATE_S,"",GUILD_PLAYER_NOT_IN_GUILD);
		return;
	}

	PlayerInfo * dstplr = objmgr.GetPlayerInfoByName(name.c_str());
	if(dstplr==NULL)
	{ 
		return;
	}

	_player->m_playerInfo->guild->DemoteGuildMember(dstplr, this);
}*/

void WorldSession::HandleGuildLeave(WorldPacket & recv_data)
{
	if(!_player->m_playerInfo->guild)
	{
		Guild::SendGuildCommandResult(this, GUILD_CREATE_S,"",GUILD_PLAYER_NOT_IN_GUILD);
		return;
	}

	_player->m_playerInfo->guild->RemoveGuildMember(_player->m_playerInfo, this);
}

void WorldSession::HandleGuildRemove(WorldPacket & recv_data)
{
	if(!_player->m_playerInfo->guild)
	{
		Guild::SendGuildCommandResult(this, GUILD_CREATE_S,"",GUILD_PLAYER_NOT_IN_GUILD);
		return;
	}

	if( _player->m_playerInfo->guildMember->pRank->CanPerformCommand( GR_RIGHT_REMOVE ) == false )
	{
		sLog.outDebug("We do not have enough rights to remove players");
		return;
	}
/*
{CLIENT} Packet: (0x0461) CMSG_GUILD_REMOVE PacketSize = 4 stamp = 4949069
00 0C - gm
02 glo0
E2 gto0
{CLIENT} Packet: (0x0461) CMSG_GUILD_REMOVE PacketSize = 4 stamp = 5066304
00 18 
0E gt1
02 gl0
{CLIENT} Packet: (0x0461) CMSG_GUILD_REMOVE PacketSize = 4 stamp = 5109438
10 08 
1E gt2
02 gl0
{CLIENT} Packet: (0x0461) CMSG_GUILD_REMOVE PacketSize = 4 stamp = 5138189
00 88 
02 gl0 
3E gt3
{CLIENT} Packet: (0x0461) CMSG_GUILD_REMOVE PacketSize = 7 stamp = 5199622
10 9C 
14 gt2
16 gt1
02 gl0 
18 gt0
12 gt3

{CLIENT} Packet: (0x0461) CMSG_GUILD_REMOVE PacketSize = 7 stamp = 8789174
10 9C 
14 16 
27 
18 12                            |....'..         |
{CLIENT} Packet: (0x0461) CMSG_GUILD_REMOVE PacketSize = 7 stamp = 8856816
10 B4
14 16 18 
25 
12                            |.....%.         |
{CLIENT} Packet: (0x0461) CMSG_GUILD_REMOVE PacketSize = 7 stamp = 8911369
11 94 
14 16 18 
23 
12                            |.....#.         |
{CLIENT} Packet: (0x0461) CMSG_GUILD_REMOVE PacketSize = 7 stamp = 8971913
12 94 
14 
21 
16 18 12                            |...!...         |
{CLIENT} Packet: (0x0461) CMSG_GUILD_REMOVE PacketSize = 10 stamp = 9031163
13 BC 
14 gt2
21 gl3
16 gt1
27 gl0
18 gt0
25 gl1
23 gl2
12 gt3
*/
	uint16 guid_mask;
	uint8 TargetPlayerGUID[8];
	uint8 PromoterPlayerGUID[8];
	*(uint64*)TargetPlayerGUID = 0;
	*(uint64*)PromoterPlayerGUID = 0;

	recv_data >> guid_mask;

	if( guid_mask & 0x0400 )
		recv_data >> TargetPlayerGUID[2];
	if( guid_mask & 0x0002 )
		recv_data >> PromoterPlayerGUID[3];
	if( guid_mask & 0x1000 )
		recv_data >> TargetPlayerGUID[1];
	if( guid_mask & 0x0800 )
		recv_data >> PromoterPlayerGUID[0];
	if( guid_mask & 0x0010 )
		recv_data >> TargetPlayerGUID[0];
	if( guid_mask & 0x2000 )
		recv_data >> PromoterPlayerGUID[1];
	if( guid_mask & 0x0001 )
		recv_data >> PromoterPlayerGUID[2];
	if( guid_mask & 0x8000 )
		recv_data >> TargetPlayerGUID[3];

	GUID_un_obfuscate( TargetPlayerGUID );
	PlayerInfo * dstplr = objmgr.GetPlayerInfo( *(uint64*)TargetPlayerGUID );
	if(dstplr==NULL)
	{ 
		return;
	}
	_player->m_playerInfo->guild->RemoveGuildMember(dstplr, this);
}

void WorldSession::HandleGuildDisband(WorldPacket & recv_data)
{
	if(!_player->m_playerInfo->guild)
	{
		Guild::SendGuildCommandResult(this, GUILD_CREATE_S,"",GUILD_PLAYER_NOT_IN_GUILD);
		return;
	}

	if(_player->m_playerInfo->guild->GetGuildLeader() != _player->GetLowGUID())
	{
		Guild::SendGuildCommandResult(this, GUILD_INVITE_S, "", GUILD_PERMISSIONS);
		return;
	}

	_player->m_playerInfo->guild->Disband();
}

void WorldSession::HandleGuildLeader(WorldPacket & recv_data)
{
	CHECK_PACKET_SIZE(recv_data, 1);

	std::string name;
	recv_data >> name;

	if(!_player->m_playerInfo->guild)
	{
		Guild::SendGuildCommandResult(this, GUILD_CREATE_S, "", GUILD_PLAYER_NOT_IN_GUILD);
		return;
	}

	PlayerInfo * dstplr = objmgr.GetPlayerInfoByName(name.c_str());
	if(dstplr==NULL)
	{
		Guild::SendGuildCommandResult(this, GUILD_CREATE_S, name.c_str(), GUILD_PLAYER_NOT_FOUND);
		return;
	}

	_player->m_playerInfo->guild->ChangeGuildMaster(dstplr, this);
}

void WorldSession::HandleGuildMotd(WorldPacket & recv_data)
{
	if(!_player->m_playerInfo->guild)
	{
		Guild::SendGuildCommandResult(this, GUILD_CREATE_S,"",GUILD_PLAYER_NOT_IN_GUILD);
		return;
	}
/*
{CLIENT} Packet: (0x99C2) CMSG_GUILD_MOTD PacketSize = 21 stamp = 5606129
|0A 00 00 00 00 00 F1 1F 45 00 00 00 00 00 00 00 |........E.......|
|6D 6F 74 64 00                                  |motd.           |
-------------------------------------------------------------------
14333
{CLIENT} Packet: (0x2480) CMSG_GUILD_MOTD PacketSize = 16 stamp = 3756363
3A mask1
5B mask2
1C 
1D 
1F 
F0 
1E 
04 go2
6D 6F 74 64 00 
02 go3
08 go0
06 go1

{CLIENT} Packet: (0x2480) CMSG_GUILD_MOTD PacketSize = 15 stamp = 13750692
player 539108390 = 20 22 24 26 = 21 23 25 27
guid 420943123 = 19 17 15 13 = 18 16 14 12
1A 79 = 11010 1001111
23 gp2
21 gp3
25 gp1
16 gg2
74 65 73 74 00 
18 gg3
12 gg0
27 gp0
14 gg1

{CLIENT} Packet: (0x2480) CMSG_GUILD_MOTD PacketSize = 14 stamp = 15731047
18 79 = 11000 	1111001 = M1 bit1 is gg0
12 79 = 10010 	1111001 = M1 bit4 is gg1
1A 78 = 11010 	1111000 = M2 bit 0 is gg2
1A 71 = 11010 	1110001 = M2 bit 3 is gg3
1A 59 = 11010	1011001 = M2 bit 5 is gp0
1A 39 = 11010	0111001 = M2 bit6 is gp1 
0A 79 = 01010	1111001 = M1 bit4 is gp2
1A 69 = 11010	1101001 = M2 bit4 is gp3

UPDATE guilds SET guildid=1 WHERE guildName='allga';
UPDATE guild_ranks SET guildid=1 WHERE guildid=1;
UPDATE guild_data SET guildid=1 WHERE playerid=1;
UPDATE characters SET guid=2237478 WHERE NAME='allguida';
UPDATE guilds SET leaderguid=2237478 WHERE guildid=420943123;
UPDATE guild_data SET playerid=2237478 WHERE guildid=420943123;
*/
	uint8 guid_mask1;
	uint8 guid_mask2;
	uint8 GuildGUID[8];
	uint8 PlayerGUID[8];
//	*(uint64*)GuildGUID = 0;
//	*(uint64*)PlayerGUID = 0;
//	uint8 g;
	std::string motd;

	recv_data >> guid_mask1; //guild
	recv_data >> guid_mask2; //us

	if( guid_mask1 & BIT_4 )
		recv_data >> PlayerGUID[2];
	if( guid_mask2 & BIT_4 )
		recv_data >> PlayerGUID[3];
	if( guid_mask2 & BIT_6 )
		recv_data >> PlayerGUID[1];
	if( guid_mask2 & BIT_0 )
		recv_data >> GuildGUID[2];
	recv_data >> motd;
/*	if( guid_mask1 & BIT_x )
		recv_data >> GuildGUID[3];
	if( guid_mask2 & BIT_x )
		recv_data >> GuildGUID[0];
	if( guid_mask2 & BIT_x )
		recv_data >> PlayerGUID[0];
	if( guid_mask2 & BIT_x )
		recv_data >> GuildGUID[1];
*/

	_player->m_playerInfo->guild->SetMOTD(motd.c_str(), this);
}

void WorldSession::HandleGuildRankEdit(WorldPacket & recv_data)
{
	CHECK_PACKET_SIZE(recv_data, 9);

	if(!_player->m_playerInfo->guild)
	{
		Guild::SendGuildCommandResult(this, GUILD_CREATE_S,"",GUILD_PLAYER_NOT_IN_GUILD);
		return;
	}

	if(GetPlayer()->GetLowGUID() != _player->m_playerInfo->guild->GetGuildLeader())
	{
		Guild::SendGuildCommandResult(this, GUILD_INVITE_S,"",GUILD_PERMISSIONS);
		return;
	}

	uint32 rankId;
	string newName;
	uint32 i;
	GuildRank * pRank;
/*
13329
{CLIENT} Packet: (0x0982) CMSG_GUILD_RANK_EDIT PacketSize = 108 stamp = 22981661
|------------------------------------------------|----------------|
|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
|------------------------------------------------|----------------|
|DF FF 0F 00 07 00 00 00 07 00 00 00 07 00 00 00 |................|
|07 00 00 00 07 00 00 00 07 00 00 00 07 00 00 00 |................|
|07 00 00 00 01 00 00 00 08 00 00 00 00 00 F1 1F |................|
|45 00 00 00 00 00 00 00 07 00 00 00 01 00 00 00 |E...............|
|07 00 00 00 07 00 00 00 07 00 00 00 07 00 00 00 |................|
|07 00 00 00 07 00 00 00 07 00 00 00 07 00 00 00 |................|
|DF FF 2F 00 4F 66 66 69 63 65 72 00             |../.Officer.    |
-------------------------------------------------------------------
{CLIENT} Packet: (0x0982) UNKNOWN PacketSize = 108 stamp = 22983892
|------------------------------------------------|----------------|
|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
|------------------------------------------------|----------------|
|DF FF 0F 00 07 00 00 00 07 00 00 00 07 00 00 00 |................|
|07 00 00 00 07 00 00 00 07 00 00 00 07 00 00 00 |................|
|07 00 00 00 01 00 00 00 08 00 00 00 00 00 F1 1F |................|
|45 00 00 00 00 00 00 00 07 00 00 00 01 00 00 00 |E...............|
|07 00 00 00 07 00 00 00 07 00 00 00 07 00 00 00 |................|
|07 00 00 00 07 00 00 00 07 00 00 00 07 00 00 00 |................|
|DF FF 0F 00 4F 66 66 69 63 65 72 00             |....Officer.    |
-------------------------------------------------------------------

DF FF 0F 00 default rights
07 00 00 00 max stack withdraw / day
07 00 00 00 
07 00 00 00 
07 00 00 00 
07 00 00 00 
07 00 00 00 
07 00 00 00 
07 00 00 00 
01 00 00 00 rankid ?
08 00 00 00 00 00 F1 1F guild GUID
45 00 00 00 00 00 00 00 player guid
07 00 00 00 gold limit
01 00 00 00 rankid ?
07 00 00 00 tabrights
07 00 00 00 
07 00 00 00 
07 00 00 00 
07 00 00 00 
07 00 00 00 
07 00 00 00 
07 00 00 00 
DF FF 2F 00 new rights
4F 66 66 69 63 65 72 00 name            
{CLIENT} Packet: (0x0982) CMSG_GUILD_RANK_EDIT PacketSize = 108 TimeStamp = 14077109
43 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
04 00 00 00 
CA 7E A9 00 00 00 F3 1F 
E5 64 E4 01 00 00 00 03 
00 00 00 00 - gold limit
03 00 00 00 - for real ID
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
43 00 00 00 
6E 6F 6F 62 6C 65 74 00 
14333
{CLIENT} Packet: (0xA440) CMSG_GUILD_RANK_EDIT PacketSize = 98 stamp = 6925679
|11 06 07 00 00 00 07 00 00 00 07 00 00 00 07 00 |................|
|00 00 07 00 00 00 07 00 00 00 01 00 00 00 FF FF |................|
|FF EF 2B 02 00 00 01 00 00 00 01 00 00 00 0F 00 |..+.............|
|00 00 00 00 00 00 2B 02 00 00 FF FF FF EF 9A 02 |......+.........|
|00 00 00 00 00 00 00 00 00 00 FF F1 0D 00 FF FF |................|
|FF EF 07 00 00 00 F0 00 1E 4F 66 66 69 63 65 72 |.........Officer|
|00 02                                           |..              |

{CLIENT} Packet: (0xA440) CMSG_GUILD_RANK_EDIT PacketSize = 108 stamp = 14333184
player 539108390 = 20 22 24 26 = 21 23 25 27
guid 420943123 = 19 17 15 13 = 18 16 14 12
EB 82 = 11101011	10000010
FF FF FF FF 07 00 00 00 07 00 00 00 07 00 00 00 07 00 00 00 07 00 00 00 07 00 00 00 07 00 00 00 07 00 00 00 00 00 00 00 00 00 00 00 
FF F1 0D 00 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 00 00 00 00 00 00 00 00 FF F1 0D 00 
21 gp3 
18 gg3 
16 gg2
12 gg0 
23 gp2
25 gp1
14 gg1
47 75 69 6C 64 20 4D 61 73 74 65 72 61 00 
27 gp0

{CLIENT} Packet: (0xA440) CMSG_GUILD_RANK_EDIT PacketSize = 107 stamp = 15741358
EB 80 = 11101011	10000000 = M2 bit1 is gg0
E9 82 = 11101001	10000010 = M1 bit1 is gg1
CB 82 = 11001011	10000010 = M1 bit5 is gg2
6B 82 = 01101011 	10000010 = M1 bit7 is gg3
EA 82 = 11101010	10000010 = M1 bit0 is gp0
AB 82 = 10101011	10000010 = M1 bit6 is gp1 
E3 82 = 11100011	10000010 = M1 bit3 is gp2
EB 02 = 11101011	00000010 = M2 bit7 is gp3

UPDATE guilds SET guildid=1 WHERE guildName='allga';
UPDATE guild_ranks SET guildid=1 WHERE guildid=1;
UPDATE guild_data SET guildid=1 WHERE playerid=1;
UPDATE characters SET guid=539108390 WHERE NAME='allguida';
UPDATE guilds SET leaderguid=539108390 WHERE guildid=420943123;
UPDATE guild_data SET playerid=539108390 WHERE guildid=420943123;
*/
	int32 gold_limit;

	uint8 guid_mask1,guid_mask2;
	uint32 iStacksPerDay[MAX_GUILD_BANK_TABS];
	uint32 iFlags[MAX_GUILD_BANK_TABS];
	recv_data >> guid_mask1 >> guid_mask2;

	recv_data >> gold_limit;

	for(uint32 j=0;j<MIN(MAX_GUILD_BANK_TABS,8);j++)
		recv_data >> iFlags[j];

	uint32 rankId1,NewRights;
	recv_data >> rankId;
	recv_data >> rankId1;
	recv_data >> NewRights;

	for(uint32 j=0;j<MIN(MAX_GUILD_BANK_TABS,8);j++)
		recv_data >> iStacksPerDay[j];

	uint32 OldRights;
	recv_data >> OldRights;

	uint8 guild_GUID[8];
	uint8 player_GUID[8];
	if( guid_mask2 & BIT_7 )	
		recv_data >> player_GUID[3];//obfuscated
	if( guid_mask1 & BIT_7 )
		recv_data >> guild_GUID[3];	//obfuscated
	if( guid_mask1 & BIT_5 )
		recv_data >> guild_GUID[2];	//obfuscated
	if( guid_mask2 & BIT_1 )	
		recv_data >> guild_GUID[0];	//obfuscated
	if( guid_mask1 & BIT_3 )	
		recv_data >> player_GUID[2]; //obfuscated
	if( guid_mask1 & BIT_6 )	
		recv_data >> player_GUID[1]; //obfuscated
	if( guid_mask1 & BIT_1 )	
		recv_data >> guild_GUID[1];	//obfuscated

	recv_data >> newName;

	if( guid_mask1 & BIT_0 )	
		recv_data >> player_GUID[0];	//obfuscated

	//trying to mod other people guild
	if( _player->m_playerInfo->guild == NULL )
	{
		sLog.outDebug("We do not belong to this guild. We cannot mod it");
		return;
	}
//	GuildGUID = _player->m_playerInfo->guild->GetGuildId();

	//check if we have the right to mod ranks
	if( _player->m_playerInfo->guildMember->pRank->CanPerformCommand( GR_RIGHT_PROMOTE ) == false )
	{
		sLog.outDebug("We do not have enough rights to mod it");
		return;
	}

	pRank = _player->m_playerInfo->guild->GetGuildRank(rankId);
	if(pRank == NULL)
	{ 
		sLog.outDebug("Could not find rank we are seaching to edit");
		return;
	}

	// do not touch guild masters withdraw limit
	if( rankId != 0 )
	{
		pRank->iGoldLimitPerDay = gold_limit;
		if( pRank->iGoldLimitPerDay < 0 )
			pRank->iGoldLimitPerDay = 0xEFFFFFFF;
	}

	for(i = 0; i < MAX_GUILD_BANK_TABS; ++i)
	{
		pRank->iTabPermissions[i].iStacksPerDay = iStacksPerDay[i];
		pRank->iTabPermissions[i].iFlags = iFlags[i];
	}

	pRank->iRights = NewRights; 

	if(newName.length() < 2)
		newName = string(pRank->szRankName);
	
	if(strcmp(newName.c_str(), pRank->szRankName) != 0)
	{
		// name changed
		char * pTmp = pRank->szRankName;
		pRank->szRankName = strdup_local(newName.c_str());
		free(pTmp);
	} 

	CharacterDatabase.Execute("REPLACE INTO guild_ranks VALUES(%u, %u, \"%s\", %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
		_player->m_playerInfo->guild->GetGuildId(), pRank->iId, CharacterDatabase.EscapeString(newName).c_str(),
		pRank->iRights, pRank->iGoldLimitPerDay,
		pRank->iTabPermissions[0].iFlags, pRank->iTabPermissions[0].iStacksPerDay,
		pRank->iTabPermissions[1].iFlags, pRank->iTabPermissions[1].iStacksPerDay,
		pRank->iTabPermissions[2].iFlags, pRank->iTabPermissions[2].iStacksPerDay,
		pRank->iTabPermissions[3].iFlags, pRank->iTabPermissions[3].iStacksPerDay,
		pRank->iTabPermissions[4].iFlags, pRank->iTabPermissions[4].iStacksPerDay,
		pRank->iTabPermissions[5].iFlags, pRank->iTabPermissions[5].iStacksPerDay,
		pRank->iTabPermissions[6].iFlags, pRank->iTabPermissions[6].iStacksPerDay,
		pRank->iTabPermissions[7].iFlags, pRank->iTabPermissions[7].iStacksPerDay
		);

//	_player->m_playerInfo->guild->SendGuildRoster( this );
	_player->m_playerInfo->guild->SendGuildRankChangeNotify( this );
	char smallstr[2];
	smallstr[0] = 0x30 + uint8( rankId );
	smallstr[1] = 0;
	_player->m_playerInfo->guild->LogGuildEvent( GUILD_EVENT_RANK_EDITED, 0, 1, smallstr );
}

void WorldSession::HandleGuildAddRank(WorldPacket & recv_data)
{
/*
13329
{CLIENT} Packet: (0x0BC2) UNKNOWN PacketSize = 77 stamp = 22880385
|------------------------------------------------|----------------|
|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
|------------------------------------------------|----------------|
|77 00 28 00 15 20 00 F9 29 10 7A 90 48 00 05 00 |w.(.. ..).z.H...|
|00 00 35 52 7C 00 15 48 00 80 45 00 00 00 00 00 |..5R|..H..E.....|
|00 00 F8 6E 15 11 19 10 00 F9 9A F9 15 48 01 65 |...n.........H.e|
|98 00 01 0F 10 77 15 F9 24 ED 44 00 00 37 A1 B2 |.....w..$.D..7..|
|FC 6B 4E 24 36 00 00 00 61 00 53 89 01          |.kN$6...a.S..   |
-------------------------------------------------------------------
{CLIENT} Packet: (0x0BC2) CMSG_GUILD_ADD_RANK PacketSize = 77 TimeStamp = 14062515
77 00 29 00 
3C 20 00 F7 
1B 10 13 A8 
E0 00 04 00 
00 00 F3 52 
7C 00 3C 18 
00 FF E5 64 
E4 01 00 00 
00 03 F6 6E 
3C 12 1A E0 
00 F7 4B F6 
3C 48 00 65 
68 00 01 14 
10 04 3C F6 
F4 58 14 00 
00 E8 52 B2 
CC 6B 4E F4 
E7 00 00 00 
61 00 53 22 00 
*/
//	string rankName;
	Guild * pGuild = _player->GetGuild();

	if(pGuild == NULL)
	{
		Guild::SendGuildCommandResult(this, GUILD_CREATE_S, "", GUILD_PLAYER_NOT_IN_GUILD);
		return;
	}

	if(pGuild->GetGuildLeader() != _player->GetLowGUID())
	{
		Guild::SendGuildCommandResult(this, GUILD_CREATE_S, "", GUILD_PERMISSIONS);
		return;
	}

//	recv_data >> rankName;
//	rankName = "New Rank";
//	if(rankName.size() < 2)
//	{ 
//		return;
//	}

//	pGuild->CreateGuildRank(rankName.c_str(), GR_RIGHT_DEFAULT, false);
	pGuild->CreateGuildRank( NULL, GR_RIGHT_DEFAULT, false);

/*
{SERVER} Packet: (0x0870) UNKNOWN PacketSize = 104 TimeStamp = 14062703
00 00 00 00 
00 A2 50 B4 AA 2A 00 00 C7 B5 04 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
1A 00 00 00 -26
00 00 00 00 00 00 00 00 00 00 00 00 70 FD 70 90 
FF 7F 00 00 CD 3B 98 00 00 00 00 00 70 FD 70 90 
FF 7F 00 00 08 5C 27 BC AA 2A 00 00 98 FD 70 90 
FF 7F 00 00 51 14 00 00 00 00 00 00 11 00 00 00 
*/
	pGuild->SendGuildRankChangeNotify( this );
	pGuild->LogGuildEvent( GUILD_EVENT_RANK_ADDED, 0, 0 );
//	pGuild->SendGuildQuery(NULL);	//will be asked by the client
//	pGuild->SendGuildRoster(this);	//will be asked by the client
}

void WorldSession::HandleGuildPromoteDemoteRank(WorldPacket & recv_data)
{
/*
13329
{CLIENT} Packet: (0x0942) UNKNOWN PacketSize = 13 stamp = 22705118
|------------------------------------------------|----------------|
|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
|------------------------------------------------|----------------|
|80 02 00 00 00 45 00 00 00 00 00 00 00          |.....E.......   | promote rank 3 to 2
-------------------------------------------------------------------
{CLIENT} Packet: (0x0942) UNKNOWN PacketSize = 13 stamp = 22802946
|------------------------------------------------|----------------|
|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
|------------------------------------------------|----------------|
|00 02 00 00 00 45 00 00 00 00 00 00 00          |.....E.......   | demote rank 3 to 4
-------------------------------------------------------------------
14333
{CLIENT} Packet: (0x0401) CMSG_GUILD_PROMOTE_DEMOTE_RANK PacketSize = 11 TimeStamp = 12568719
2F 80 04 00 00 00 8C A7 6A 00 03 
*/
	uint8 guid_mask;
	uint8 action_type;
	uint32 rank_id;
//	uint64 playerGUID;

	recv_data >> guid_mask;
	recv_data >> action_type;
	recv_data >> rank_id;
//	recv_data >> playerGUID;

//	Player *plr = objmgr.GetPlayer( playerGUID );
	Player *plr = GetPlayer();
	if( plr == NULL )
	{
		sLog.outDebug("Could not find player");
		return;
	}
	if( plr->m_playerInfo->guild == NULL )
	{
		sLog.outDebug("Player does not have a guild");
		return;
	}
	if( plr->m_playerInfo->guildMember->pRank->iId != 0 )
	{
		Guild::SendGuildCommandResult(this, GUILD_CREATE_S, "", GUILD_PERMISSIONS);
		sLog.outDebug("Player does not have the rights to modify ranks");
		return;
	}
	if( rank_id <= 0 || rank_id == 1 )
	{
		sLog.outDebug("Rank 0 cannot be demoted or promoted");
		return;
	}

	uint32 new_id;
	if( action_type == 0x80 ) //promote rank
		new_id = rank_id - 1;
	else
		new_id = rank_id + 1;

	if( new_id < 1 || new_id >= MAX_GUILD_RANKS )
	{
		sLog.outDebug("New rank ID has an unaceptable value");
		return;
	}

	plr->m_playerInfo->guild->SwapGuildRank( rank_id, new_id );
	plr->m_playerInfo->guild->SendGuildRankChangeNotify( this );
	plr->m_playerInfo->guild->LogGuildEvent( GUILD_EVENT_RANK_PROMOTE_DEMOTE, 0, 0 );
}

void WorldSession::HandleGuildDelRank(WorldPacket & recv_data)
{
/*
13329
{CLIENT} Packet: (0x0B82) UNKNOWN PacketSize = 12 stamp = 22864754
|------------------------------------------------|----------------|
|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
|------------------------------------------------|----------------|
|02 00 00 00 45 00 00 00 00 00 00 00             |....E.......    |
-------------------------------------------------------------------
14333
{CLIENT} Packet: (0xA4E0) CMSG_GUILD_DEL_RANK PacketSize = 6 stamp = 3672747
|------------------------------------------------|----------------|
|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
|------------------------------------------------|----------------|
|02 08 00 00 00 02                               |......          |
-------------------------------------------------------------------
*/

	Guild * pGuild = _player->GetGuild();

	if(pGuild == NULL)
	{
		Guild::SendGuildCommandResult(this, GUILD_CREATE_S, "", GUILD_PLAYER_NOT_IN_GUILD);
		return;
	}

	if(pGuild->GetGuildLeader() != _player->GetGUID())
	{
		Guild::SendGuildCommandResult(this, GUILD_CREATE_S, "", GUILD_PERMISSIONS);
		return;
	}

	uint32	rankID;
	uint8	guid_mask; //bit1=go1

	recv_data >> guid_mask;
	recv_data >> rankID;

	GuildRank *gr = pGuild->GetGuildRank( rankID );

	if( gr == NULL )
	{
		sLog.outDebug("Guild does not have rank with ID %d", rankID);
		return;
	}
	pGuild->RemoveGuildRank( gr, this );

/*
- this was sent only when i deleted the rank
{SERVER} Packet: (0x0870) UNKNOWN PacketSize = 104 TimeStamp = 13185437
00 00 00 00 00 FD 70 90 FF 7F 00 00 B8 71 04 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
1A 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 02 00 00 00 00 00 00 00 F8 86 4A 03 00 00 00 03 90 38 40 C1 AA 2A 00 00 B4 71 04 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 12 00 00 00 
*/
	pGuild->SendGuildRankChangeNotify( this );
	pGuild->LogGuildEvent( GUILD_EVENT_RANK_DELETED, 0, 0 );
//	pGuild->SendGuildQuery(NULL);		//this will be asked by the client
//	pGuild->SendGuildRankQuery(NULL);	//this will be asked by the client
//	pGuild->SendGuildRoster(this);
}
/*
void WorldSession::HandleGuildSetPublicNote(WorldPacket & recv_data)
{
	string target, newnote;
	recv_data >> target >> newnote;

	PlayerInfo * pTarget = objmgr.GetPlayerInfoByName(target.c_str());
	if(pTarget == NULL)
	{ 
		return;
	}

	if(!pTarget->guild)
	{ 
		return;
	}

	pTarget->guild->SetPublicNote(pTarget, newnote.c_str(), this);
}


void WorldSession::HandleGuildSetOfficerNote(WorldPacket & recv_data)
{
	string target, newnote;
	recv_data >> target >> newnote;

	PlayerInfo * pTarget = objmgr.GetPlayerInfoByName(target.c_str());
	if(pTarget == NULL)
	{ 
		return;
	}

	if(!pTarget->guild)
	{ 
		return;
	}

	pTarget->guild->SetOfficerNote(pTarget, newnote.c_str(), this);
} */

void WorldSession::HandleGuildSetNote(WorldPacket & recv_data)
{
/*

{CLIENT} Packet: (0x84A0) CMSG_GUILD_SET_NOTE PacketSize = 32 TimeStamp = 14871185
// player guid = A6 8D 6B 01 02 ( 10001111 ) = A6 8D 6B 01 00 00 00 02 = A7 8C 6A 00 XX XX XX 03
F6 FA 36 80 
E9 gg6 ?
00 pg 
03 pg 
03 pg
A7 pg
8C pg
00 pg
70 75 62 6C 69 63 20 6E 6F 74 65 00 
8C pg
F9 gg
A7 pg
F3 gg
1E gg
6A pg
6A pg
88 gg
00 pg

{SERVER} Packet: (0x8721) UNKNOWN PacketSize = 19 TimeStamp = 14871512
DE 00 - 11011110
03 pg7
A7 pg0
6A pg2
70 75 62 6C 69 63 20 6E 6F 74 65 00 - public note 
8C pg1
00 pg3

player 539108390 = 20 22 24 26 = 21 23 25 26
guid 420943123 = 19 17 15 13 = 18 16 14 12
{CLIENT} Packet: (0x84A0) CMSG_GUILD_SET_NOTE PacketSize = 23 stamp = 19376759
96 DA 32 80 10010110 11011010 110010 10000000 - public note
16 gg2	M2bit7
18 gg3	M3bit5
37 gpt0	M4bit7
35 gpt1 M2bit6
31 gpt3 M3bit4
70 62 6E 6F 74 65 00 
25 gpm1	M2bit1
12 gg0	M2bit4
27 gpm0 M2bit3
33 gpt2 M1bit1
23 gpm2 M3bit1
14 gg1  M1bit7
21 gpm3 M1bit2

96 D2 32 00 10010110 11010010 110010 00000000 - M2bit3 gpm0			M4bit7 gpt0
16 18 25 21 70 62 6E 6F 74 65 00 25 12 23 23 14 21
96 98 32 80 10010110 10011000 110010 10000000 - M2bit1 gpm1			M2bit6 gpt1
16 18 27 21 70 62 6E 6F 74 65 00 12 27 23 23 14 21   
94 DA 30 80 10010100 11011010 110000 10000000 - M3bit1 gpm2			M1bit1 gpt2
16 18 27 25 21 70 62 6E 6F 74 65 00 25 12 27 14 21                                  
92 DA 22 80 10010010 11011010 100010 10000000 - M1bit2 gpm3			M3bit4 gpt3
16 18 27 25 70 62 6E 6F 74 65 00 25 12 27 23 23 14                                  
guild bytes
96 CA 32 80 10010110 11001010 110010 10000000 - M2bit4 gg0
16 18 27 25 21 70 62 6E 6F 74 65 00 25 27 23 23 14 21
16 DA 32 80 00010110 11011010 110010 10000000 - M1bit7 gg1
16 18 27 25 21 70 62 6E 6F 79 65 00 25 12 27 23 23 21                               
96 5A 32 80 10010110 01011010 110010 10000000 - M2bit7 gg2
18 27 25 21 70 62 6E 6F 74 65 00 25 12 27 23 23 14 21    
96 DA 12 80 10010110 11011010 010010 10000000 - M3bit5 gg3
16 27 25 21 70 62 6E 6F 74 65 00 25 12 27 23 23 14 21                              
target guid bytes
96 DA 32 80 10010110 11011010 110010 10000000
16 18 37 35 31 70 62 6E 6F 74 65 00 25 12 27 33 23 14 21                            
96 DA 32 00 10010110 11011010 110010 00000000 - M4bit7 gpt0
16 18 35 31 70 62 6E 6F 74 65 00 25 12 27 33 23 14 21  
96 9A 32 80 10010110 10011010 110010 10000000 - M2bit6 gpt1
16 18 37 31 70 62 6E 6F 74 65 00 25 12 27 33 23 14 21                               
94 DA 32 80 10010100 11011010 110010 10000000 - M1bit1 gpt2
16 18 37 35 31 70 62 6E 6F 74 65 00 25 12 27 23 14 21                               
96 DA 22 80 10010110 11011010 100010 10000000 - M3bit4 gpt3
16 18 37 35 70 62 6E 6F 74 65 00 25 12 27 33 23 14 21                               


{CLIENT} Packet: (0x84A0) CMSG_GUILD_SET_NOTE PacketSize = 22 stamp = 19611540
86 DA 32 80 - officer note - is missing bit 7 from first byte. That is all the difference
16 gg2
18 gg3
27 gp0
25 gp1
21 gp3
6F 6E 6F 74 65 00 
25 gp1
12 gg0
27 gp0
23 gp2
23 gp2
14 gg1
21 gp3 
*/

	uint8 guid_mask[4];
	uint8 guild_GUID[8];
	uint8 PlayerGuid_modder[8];
	uint8 PlayerGuid_target[8];
	*(uint64*)guild_GUID = 0;
	*(uint64*)PlayerGuid_modder = 0;
	*(uint64*)PlayerGuid_target = 0;

	string newnote;
	bool IsOfficerNote;

	recv_data >> guid_mask[0] >> guid_mask[1] >> guid_mask[2] >> guid_mask[3];
	if( guid_mask[0] & BIT_0x10 )
		IsOfficerNote = false;
	else
		IsOfficerNote = true;

	if( guid_mask[1] & BIT_7 )
		recv_data >> guild_GUID[2];
	if( guid_mask[2] & BIT_5 )
		recv_data >> guild_GUID[3];
	if( guid_mask[3] & BIT_7 )
		recv_data >> PlayerGuid_target[0];
	if( guid_mask[1] & BIT_6 )
		recv_data >> PlayerGuid_target[1];
	if( guid_mask[2] & BIT_4 )
		recv_data >> PlayerGuid_target[3];

	recv_data >> newnote;

	if( guid_mask[1] & BIT_1 )
		recv_data >> PlayerGuid_modder[1];
	if( guid_mask[1] & BIT_4 )
		recv_data >> guild_GUID[0];
	if( guid_mask[1] & BIT_3 )
		recv_data >> PlayerGuid_modder[0];
	if( guid_mask[0] & BIT_1 )
		recv_data >> PlayerGuid_target[2];
	if( guid_mask[2] & BIT_1 )
		recv_data >> PlayerGuid_modder[2];
	if( guid_mask[0] & BIT_7 )
		recv_data >> guild_GUID[1];
	if( guid_mask[0] & BIT_2 )
		recv_data >> PlayerGuid_modder[3];

	GUID_un_obfuscate( PlayerGuid_target );


	PlayerInfo * pTarget = objmgr.GetPlayerInfo( *(uint64*)PlayerGuid_target );
	if(pTarget == NULL)
	{ 
		sLog.outDebug("Could not find target GUID %d",*(uint32*)PlayerGuid_target);
		return;
	}

	if( pTarget->guild == NULL || pTarget->guild != GetPlayer()->GetGuild() )
	{ 
		sLog.outDebug("Could not find target guild or target guild does not match");
		return;
	}

	if( IsOfficerNote == false )
		pTarget->guild->SetPublicNote(pTarget, newnote.c_str(), this);
	else
		pTarget->guild->SetOfficerNote(pTarget, newnote.c_str(), this);
}

void WorldSession::HandleSaveGuildEmblem(WorldPacket & recv_data)
{
	uint64 guid;
	Guild * pGuild = _player->GetGuild();
	int32 cost = MONEY_ONE_GOLD * 10;
	uint32 emblemStyle, emblemColor, borderStyle, borderColor, backgroundColor;
/*
13329
{CLIENT} Packet: (0x9D74) MSG_SAVE_GUILD_EMBLEM PacketSize = 28 TimeStamp = 17963906
B2 08 00 00 BC 13 30 F1 
7A 00 00 00 
0F 00 00 00 
00 00 00 00 
06 00 00 00 
29 00 00 00 

{SERVER} Packet: (0x9D74) MSG_SAVE_GUILD_EMBLEM PacketSize = 4 TimeStamp = 17964406
00 00 00 00 
*/
	WorldPacket data(MSG_SAVE_GUILD_EMBLEM, 4);
	recv_data >> guid;
	
	CHECK_PACKET_SIZE(recv_data, 28);
	CHECK_INWORLD_RETURN;
	CHECK_GUID_EXISTS(guid);

	recv_data >> emblemStyle >> emblemColor >> borderStyle >> borderColor >> backgroundColor;
	if(pGuild==NULL)
	{
		data << uint32(ERR_GUILDEMBLEM_NOGUILD);
		SendPacket(&data);
		return;
	}

	if(pGuild->GetGuildLeader() != _player->GetLowGUID())
	{
		data << uint32(ERR_GUILDEMBLEM_NOTGUILDMASTER);
		SendPacket(&data);
		return;
	}

	if(_player->GetGold() < (uint32)cost)
	{
		data << uint32(ERR_GUILDEMBLEM_NOTENOUGHMONEY);
		SendPacket(&data);
		return;
	}

	data <<	uint32(ERR_GUILDEMBLEM_SUCCESS);
	SendPacket(&data);

	// set in memory and database
	pGuild->SetTabardInfo(emblemStyle, emblemColor, borderStyle, borderColor, backgroundColor);

	// update all clients (probably is an event for this, again.)
	pGuild->SendGuildQuery(NULL);
}

// Charter part
void WorldSession::HandleCharterBuy(WorldPacket & recv_data)
{
	/*
	{CLIENT} Packet: (0x01BD) CMSG_PETITION_BUY PacketSize = 85
	|------------------------------------------------|----------------|
	|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
	|------------------------------------------------|----------------|
	|50 91 00 00 6E 13 01 F0 00 00 00 00 00 00 00 00 |P...n...........|
	|00 00 00 00 53 74 6F 72 6D 62 72 69 6E 67 65 72 |....Stormbringer|
	|73 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 |s...............|
	|00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 |................|
	|00 00 00 00 00 00 00 00 00 00 00 00 00 01 00 00 |................|
	|00 00 00 00 00								  |.....		   |
	-------------------------------------------------------------------
	*/

	if(!_player->IsInWorld())
		return;

	uint8 error;

	// Arena team charters are full of crap
	uint64 creature_guid;
	uint64 crap;
	uint32 crap2;
	string name;
	uint64 crap3, crap4, crap5, crap6, crap7, crap8;
	uint32 crap9;
	uint8 crap10;
	uint32 arena_index;
	uint32 crap11;

		
	recv_data >> creature_guid;
	recv_data >> crap >> crap2;
	recv_data >> name;
	recv_data >> crap3 >> crap4 >> crap5 >> crap6 >> crap7 >> crap8;
	recv_data >> crap9;
	recv_data >> crap10;
	recv_data >> arena_index;
	recv_data >> crap11;

	Creature * crt = _player->GetMapMgr()->GetCreature( creature_guid );
	if(!crt || arena_index >= 4 )
	{
		GetPlayer()->SoftDisconnect();
		return;
	}

/*	if(crt->GetEntry()==19861 || crt->GetEntry()==18897 || crt->GetEntry()==19856)		// i am lazy! 
	{
		uint32 arena_type = arena_index - 1;
		if(arena_type > 2)
			return;

		if(_player->m_arenaTeams[arena_type] || _player->m_charters[arena_index])
		{
			SendNotification("You are already in an arena team.");
			return;
		}

		ArenaTeam * t = objmgr.GetArenaTeamByName(name, arena_type);
		if(t != NULL)
		{
			sChatHandler.SystemMessage(this,"That name is already in use.");
			return;
		}

		if(objmgr.GetCharterByName(name, (CharterTypes)arena_index))
		{
			sChatHandler.SystemMessage(this,"That name is already in use.");
			return;
		}

		if(_player->m_charters[arena_type])
		{
			SendNotification("You already have an arena charter.");
			return;
		}

		static uint32 item_ids[] = {ARENA_TEAM_CHARTER_2v2, ARENA_TEAM_CHARTER_3v3, ARENA_TEAM_CHARTER_5v5};
		static uint32 costs[] = {ARENA_TEAM_CHARTER_2v2_COST,ARENA_TEAM_CHARTER_3v3_COST,ARENA_TEAM_CHARTER_5v5_COST};

		if(_player->GetGold() < costs[arena_type])
			return;			// error message needed here

		ItemPrototype * ip = ItemPrototypeStorage.LookupEntry(item_ids[arena_type]);
		ASSERT(ip);
		SlotResult res = _player->GetItemInterface()->FindFreeInventorySlot(ip);
		if(res.Result == 0)
		{
			_player->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_INVENTORY_FULL);
			return;
		}

		error = _player->GetItemInterface()->CanReceiveItem(ip,1);
		if(error)
		{
			_player->GetItemInterface()->BuildInventoryChangeError(NULL,NULL,error);
		}
		else
		{
			// Create the item and charter
			Item * i = objmgr.CreateItem(item_ids[arena_type], _player);
			Charter * c = objmgr.CreateCharter(_player->GetLowGUID(), (CharterTypes)arena_index);
			c->GuildName = name;
			c->ItemGuid = i->GetGUID();

			i->SetUInt32Value(ITEM_FIELD_STACK_COUNT, 1);
			i->SetUInt32Value(ITEM_FIELD_FLAGS, ITEM_FLAG_SOULBOUND);
			i->SetUInt32Value(ITEM_FIELD_ENCHANTMENT_1_1 , c->GetID());
			i->SetUInt32Value(ITEM_FIELD_PROPERTY_SEED, 57813883);
			if( !_player->GetItemInterface()->AddItemToFreeSlot(&i) )
			{
				c->Destroy();
				i->DeleteMe();
				return;
			}

			c->SaveToDB();

//			WorldPacket data(45);
//			BuildItemPushResult(&data, _player->GetGUID(), ITEM_PUSH_TYPE_RECEIVE, 1, item_ids[arena_type], 0);
//			SendPacket(&data);
			SendItemPushResult(i, false, true, false, true, _player->GetItemInterface()->LastSearchItemBagSlot(), _player->GetItemInterface()->LastSearchItemSlot(), 1);
			_player->ModGold( -(int32)costs[arena_type]);
			_player->m_charters[arena_index] = c;
			_player->SaveToDB(false);
		}
	}
	else */
	{
		ItemPrototype *ip = ItemPrototypeStorage.LookupEntry( ITEM_ENTRY_GUILD_CHARTER );
		int32 cost = MAX(1000,ip->BuyPrice);
        if( (int64)_player->GetGold( ) < cost )
        {
            SendNotification("You don't have enough money.");
            return;
        }

		Guild * g = objmgr.GetGuildByGuildName(name);
		Charter * c = objmgr.GetCharterByName(name, CHARTER_TYPE_GUILD);
		if(g != 0 || c != 0)
		{
			SendNotification("A guild with that name already exists.");
			return;
		}

		if(_player->m_charters[CHARTER_TYPE_GUILD])
		{
			SendNotification("You already have a guild charter.");
			return;
		}

		SlotResult res = _player->GetItemInterface()->FindFreeInventorySlot(ip);
		if(res.Result == 0)
		{
			_player->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_INVENTORY_FULL);
			return;
		}

		error = _player->GetItemInterface()->CanReceiveItem(ItemPrototypeStorage.LookupEntry(ITEM_ENTRY_GUILD_CHARTER),1);
		if(error)
		{
			_player->GetItemInterface()->BuildInventoryChangeError(NULL,NULL,error);
		}
		else
		{
			// Meh...
			crt->PlaySoundTarget( 0x000019C2 );

			// Create the item and charter
			Item * i = objmgr.CreateItem(ITEM_ENTRY_GUILD_CHARTER, _player);
			c = objmgr.CreateCharter(_player->GetLowGUID(), CHARTER_TYPE_GUILD);
			c->GuildName = name;
			c->ItemGuid = i->GetGUID();


			i->SetUInt32Value(ITEM_FIELD_STACK_COUNT, 1);
			i->SetUInt32Value(ITEM_FIELD_FLAGS, ITEM_FLAG_SOULBOUND);
			i->SetUInt32Value(ITEM_FIELD_ENCHANTMENT_1_1 , c->GetID());
			i->SetUInt32Value(ITEM_FIELD_PROPERTY_SEED, 57813883);
			if( !_player->GetItemInterface()->AddItemToFreeSlot(&i) )
			{
				c->Destroy();
				i->DeleteMe();
				return;
			}

			c->SaveToDB();

			/*data.clear();
			data.resize(45);
			BuildItemPushResult(&data, _player->GetGUID(), ITEM_PUSH_TYPE_RECEIVE, 1, ITEM_ENTRY_GUILD_CHARTER, 0);
			SendPacket(&data);*/
			SendItemPushResult(i, false, true, false, true, _player->GetItemInterface()->LastSearchItemBagSlot(), _player->GetItemInterface()->LastSearchItemSlot(), 1);

			_player->m_charters[CHARTER_TYPE_GUILD] = c;
			_player->ModGold( -cost );
			_player->SaveToDB(false);
		}
	}
}

void SendShowSignatures(Charter * c, uint64 item_guid, Player * p)
{
/*
13329
{SERVER} Packet: (0x747C) SMSG_PETITION_SHOW_SIGNATURES PacketSize = 21 TimeStamp = 26846430
67 0C E8 4D 02 00 80 46 
3B 08 1B 01 00 00 80 06 
5B 5E 3F 00 
00 - no signature :(
14333
{SERVER} Packet: (0xFABB) UNKNOWN PacketSize = 21 TimeStamp = 4717844
13 AD 8A A8 03 00 00 42 A6 8D 6B 01 00 00 00 02 B2 33 DD 00 00 
*/
	WorldPacket data(SMSG_PETITION_SHOW_SIGNATURES, 100);
	data << item_guid;
	data << (uint64)c->GetLeader();
	data << c->GetID();
	data << uint8(c->SignatureCount);
	for(uint32 i = 0; i < c->Slots; ++i)
	{
		if(c->Signatures[i] == 0) continue;
		data << uint64(c->Signatures[i]) << uint32(1);
	}
	data << uint8(0);
	p->GetSession()->SendPacket(&data);
}

void WorldSession::HandleCharterShowSignatures(WorldPacket & recv_data)
{
	Charter * pCharter;
	uint64 item_guid;
	recv_data >> item_guid;
	pCharter = objmgr.GetCharterByItemGuid(item_guid);
	
	if(pCharter)
		SendShowSignatures(pCharter, item_guid, _player);
}

void WorldSession::HandleCharterQuery(WorldPacket & recv_data)
{
	/*
13329
{SERVER} Packet: (0x7755) SMSG_PETITION_QUERY_RESPONSE PacketSize = 83 TimeStamp = 26842093
5B 5E 3F 00 
3B 08 1B 01 00 00 80 06 
66 69 6E 69 73 68 68 69 6D 00 
00 
04 00 00 00 slots
04 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00 
00 00 

	{SERVER} Packet: (0x01C7) SMSG_PETITION_QUERY_RESPONSE PacketSize = 77
	|------------------------------------------------|----------------|
	|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
	|------------------------------------------------|----------------|
	|20 08 00 00 28 32 01 00 00 00 00 00 53 74 6F 72 | ...(2......Stor|
	|6D 62 72 69 6E 67 65 72 73 00 00 09 00 00 00 09 |mbringers.......|
	|00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 |................|
	|00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 |................|
	|00 00 00 00 00 00 00 00 00 00 00 00 00		  |.............   |
	-------------------------------------------------------------------
	
	uint32 charter_id
	uint64 leader_guid
	string guild_name
	uint8  0			<-- maybe subname? or some shit.. who knows
	uint32 9
	uint32 9
	uint32[9] signatures
	uint8  0
	uint8  0
	*/
//this is wrong there are 42 bytes after 9 9, 4*9+2=38 not 42,
	//moreover it can't signature, blizz uses always fullguid so it must be uin64
	//moreover this does not show ppl who signed this, for this purpose there is another opcde
	uint32 charter_id;
	uint64 item_guid;
	recv_data >> charter_id;
	recv_data >> item_guid;
	/*Charter * c = objmgr.GetCharter(charter_id,CHARTER_TYPE_GUILD);
	if(c == 0)
		c = objmgr.GetCharter(charter_id, CHARTER_TYPE_ARENA_2V2);
	if(c == 0)
		c = objmgr.GetCharter(charter_id, CHARTER_TYPE_ARENA_3V3);
	if(c == 0)
		c = objmgr.GetCharter(charter_id, CHARTER_TYPE_ARENA_5V5);*/

	Charter * c = objmgr.GetCharterByItemGuid(item_guid);
	if(c == 0)
		return;

	WorldPacket data(SMSG_PETITION_QUERY_RESPONSE, 100);
	data << charter_id;
	data << (uint64)c->LeaderGuid;
	data << c->GuildName << uint8(0);
	if(c->CharterType == CHARTER_TYPE_GUILD)
	{
		data << uint32(c->Slots) << uint32(c->Slots);
	}
	else
	{
		/*uint32 v = c->CharterType;
		if(c->CharterType == CHARTER_TYPE_ARENA_3V3)
			v=2;
		else if(c->CharterType == CHARTER_TYPE_ARENA_5V5)
			v=4;

		data << v << v;*/
		data << uint32(c->Slots) << uint32(c->Slots);
	}

	data << uint32(0);                                      // 4
    data << uint32(0);                                      // 5
    data << uint32(0);                                      // 6
    data << uint32(0);                                      // 7
    data << uint32(0);                                      // 8
    data << uint16(0);                                      // 9 2 bytes field
    
	data << uint32(0);                                      // 10 - player level limitation ? 0 - no limit
	data << uint32(0);										// 11 - player level limitation ?

    data << uint32(0);                                      // 12
    data << uint32(0);                                      // 13 count of next strings?
    data << uint32(0);                                      // 14
	data << uint32(0);
	data << uint16(0);

	if (c->CharterType == CHARTER_TYPE_GUILD)
	{
		data << uint32(0);
	}
	else
	{
		data << uint32(1);
	}

	SendPacket(&data);
}

void WorldSession::HandleCharterOffer( WorldPacket & recv_data )
{
	uint32 shit;
	uint64 item_guid, target_guid;
	Charter * pCharter;
	recv_data >> shit >> item_guid >> target_guid;
	
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	Player * pTarget = _player->GetMapMgr()->GetPlayer( target_guid );
	pCharter = objmgr.GetCharterByItemGuid(item_guid);

	if( !pCharter )
	{
		SendNotification("Item not found.");
		return;
	}

	if(pTarget == 0 || pTarget->GetTeam() != _player->GetTeam() || pTarget == _player && !sWorld.interfaction_guild)
	{
		SendNotification("Target is of the wrong faction.");
		return;
	}

	if(!pTarget->CanSignCharter(pCharter, _player))
	{
		SendNotification("Target player cannot sign your charter for one or more reasons.");
		return;
	}

	SendShowSignatures(pCharter, item_guid, pTarget);
}

void WorldSession::HandleCharterSign( WorldPacket & recv_data )
{
	uint64 item_guid;
	recv_data >> item_guid;

	if( _player == NULL || _player->GetMapMgr() == NULL )
	{
		return;
	}
	Charter * c = objmgr.GetCharterByItemGuid(item_guid);
	if( c == NULL )
	{ 
		return;
	}

	for(uint32 i = 0; i < c->SignatureCount; ++i)
	{
		if(c->Signatures[i] == _player->GetGUID())
		{
			SendNotification("You have already signed that charter.");
			return;
		}
	}

	if( c->IsFull() == true )
	{ 
		SendNotification("You need more signatures.");
		return;
	}

	c->AddSignature(_player->GetLowGUID());
	c->SaveToDB();
	_player->m_charters[c->CharterType] = c;
	_player->SaveToDB(false);

	Player * l = _player->GetMapMgr()->GetPlayer( c->GetLeader() );
	if(l == 0)
	{ 
		return;
	}
/*
13329
{SERVER} Packet: (0xAF56) UNKNOWN PacketSize = 20 TimeStamp = 46963180
67 0C E8 4D 02 00 80 46 5D D9 3B 04 00 00 80 06 00 00 00 00 
14333
{SERVER} Packet: (0x6E6F) UNKNOWN PacketSize = 20 TimeStamp = 4790931
13 AD 8A A8 03 00 00 42 77 B7 74 04 00 00 00 02 00 00 00 00 
*/
	WorldPacket data(SMSG_PETITION_SIGN_RESULTS, 100);
	data << item_guid << _player->GetGUID() << uint32(0);
	l->GetSession()->SendPacket(&data);
	data.clear();
	data << item_guid << (uint64)c->GetLeader() << uint32(0);
	SendPacket(&data);
}

void WorldSession::HandleCharterTurnInCharter(WorldPacket & recv_data)
{
	uint64 mooguid;
	recv_data >> mooguid;
	Charter * pCharter = objmgr.GetCharterByItemGuid(mooguid);
	if(!pCharter) 
	{ 
		return;
	}

	if(pCharter->CharterType == CHARTER_TYPE_GUILD)
	{
		Charter * gc = _player->m_charters[CHARTER_TYPE_GUILD];
		if(gc == 0)
		{ 
			return;
		}
		if(gc->SignatureCount < GUILD_CHARTER_SIGNATURES_REQ && Config.MainConfig.GetBoolDefault("Server", "RequireAllSignatures", false))
		{
			Guild::SendTurnInPetitionResult( this, ERR_PETITION_NOT_ENOUGH_SIGNATURES );
			return;
		}

		// dont know hacky or not but only solution for now
		// If everything is fine create guild

		Guild *pGuild = Guild::Create();
		pGuild->CreateFromCharter(gc, this);

		// Destroy the charter
		_player->m_charters[CHARTER_TYPE_GUILD] = 0;
		gc->Destroy();

		_player->GetItemInterface()->RemoveItemAmt(ITEM_ENTRY_GUILD_CHARTER, 1);
		sHookInterface.OnGuildCreate(_player, pGuild);
	}
/*	else
	{
		// Arena charter - TODO: Replace with correct messages 
		ArenaTeam * team;
		uint32 type;
		uint32 i;
		uint32 icon, iconcolor, bordercolor, border, background;
		recv_data >> iconcolor >>icon >> bordercolor >> border >> background;

		switch(pCharter->CharterType)
		{
		case CHARTER_TYPE_ARENA_2V2:
			type = ARENA_TEAM_TYPE_2V2;
			break;

		case CHARTER_TYPE_ARENA_3V3:
			type = ARENA_TEAM_TYPE_3V3;
			break;

		case CHARTER_TYPE_ARENA_5V5:
			type = ARENA_TEAM_TYPE_5V5;
			break;

		default:
			SendNotification("Internal Error");
			return;
		}

		if(_player->m_arenaTeams[pCharter->CharterType-1] != NULL)
		{
			sChatHandler.SystemMessage(this, "You are already in an arena team.");
			return;
		}

		if(pCharter->SignatureCount < pCharter->GetNumberOfSlotsByType() && Config.MainConfig.GetBoolDefault("Server", "RequireAllSignatures", false))
		{
			Guild::SendTurnInPetitionResult( this, ERR_PETITION_NOT_ENOUGH_SIGNATURES );
			return;
		}

		team = new ArenaTeam(type, objmgr.GenerateArenaTeamId());
		team->m_name = pCharter->GuildName;
		team->m_emblemColour = iconcolor;
		team->m_emblemStyle = icon;
		team->m_borderColour = bordercolor;
		team->m_borderStyle = border;
		team->m_backgroundColour = background;
		team->m_leader=_player->GetLowGUID();
		team->m_stat_rating=ARENA_TEAM_START_RATING;
        
		objmgr.AddArenaTeam(team);
		objmgr.UpdateArenaTeamRankings();
		team->AddMember(_player->m_playerInfo);
		

		// Add the members 
		for(i = 0; i < pCharter->SignatureCount; ++i)
		{
			PlayerInfo * info = objmgr.GetPlayerInfo(pCharter->Signatures[i]);
			if(info)
			{
				team->AddMember(info);
			}
		}

		_player->GetItemInterface()->SafeFullRemoveItemByGuid(mooguid);
		_player->m_charters[pCharter->CharterType] = NULL;
		pCharter->Destroy();
	}*/

	Guild::SendTurnInPetitionResult( this, ERR_PETITION_NOT_ENOUGH_SIGNATURES );
}

void WorldSession::HandleCharterSignDecline(WorldPacket & recv_data)
{
/*
{CLIENT} Packet: (0xC454) MSG_PETITION_DECLINE PacketSize = 8 stamp = 33100869
|------------------------------------------------|----------------|
|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
|------------------------------------------------|----------------|
|A8 0A F8 09 00 00 00 44                         |.......D        |
-------------------------------------------------------------------
*/
	uint64 item_guid;
	recv_data >> item_guid;
	Charter * pCharter = objmgr.GetCharterByItemGuid(item_guid);
	
	if(pCharter && GetPlayer() && GetPlayer()->GetMapMgr() )
	{
		Player *owner = GetPlayer()->GetMapMgr()->GetPlayer( pCharter->LeaderGuid );
		if( owner && owner->GetSession() )
		{
			sStackWorldPacket( data, MSG_PETITION_DECLINE, 8 + 1 );
			data << GetPlayer()->GetGUID();
			owner->GetSession()->SendPacket( &data );
		}
	}
}

void WorldSession::HandleCharterRename(WorldPacket & recv_data)
{
	uint64 guid;
	string name;
	recv_data >> guid >> name;

	Charter * pCharter = objmgr.GetCharterByItemGuid(guid);
	if(pCharter == 0)
	{ 
		return;
	}

	Guild * g = objmgr.GetGuildByGuildName(name);
	Charter * c = objmgr.GetCharterByName(name, (CharterTypes)pCharter->CharterType);
	if(c || g)
	{
		SendNotification("That name is in use by another guild.");
		return;
	}

	c = pCharter;
	c->GuildName = name;
	c->SaveToDB();
/*
13329
{CLIENT} Packet: (0x577F) MSG_PETITION_RENAME PacketSize = 18 TimeStamp = 26948689
67 0C E8 4D 02 00 80 46 6B 69 6C 6C 67 6E 6F 6D 65 00 
*/
	WorldPacket data(MSG_PETITION_RENAME, 100);
	data << guid << name;
	SendPacket(&data);
}

void WorldSession::HandleGuildLog(WorldPacket & recv_data)
{
	if(!_player->m_playerInfo->guild)
	{ 
		return;
	}

	_player->m_playerInfo->guild->SendGuildLog(this);
}

void WorldSession::HandleGuildBankBuyTab(WorldPacket & recv_data)
{
/*
13329
{CLIENT} Packet: (0x5D56) CMSG_GUILD_BANK_BUY_TAB PacketSize = 9 stamp = 2601067
|------------------------------------------------|----------------|
|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
|------------------------------------------------|----------------|
|23 00 00 00 00 00 C0 F1 00                      |#........       |
-------------------------------------------------------------------
*/
	uint64 guid;
	recv_data >> guid;

	if(!_player->IsInWorld())
	{ 
		return;
	}

	if(!_player->m_playerInfo->guild)
	{ 
		return;
	}

	if(_player->m_playerInfo->guild->GetGuildLeader() != _player->GetLowGUID())
	{
		Guild::SendGuildCommandResult(this, GUILD_MEMBER_S, "", GUILD_PERMISSIONS);
		return;
	}

	if(_player->m_playerInfo->guild->GetBankTabCount() < MAX_GUILD_BANK_TABS )	//!!last 2 tabs cannot be bought !!!
	{
		//                                        tab1, tab2, tab3, tab4, tab5, tab6
		const static int32 GuildBankPrices[MAX_GUILD_BANK_TABS] = { 100, 250,  500,  1000, 2500, 5000, 10000, 20000 };
		int32 cost = MONEY_ONE_GOLD * GuildBankPrices[_player->m_playerInfo->guild->GetBankTabCount()];

		if(_player->GetGold() < (unsigned)cost)
			return;

		_player->ModGold( -cost);
		_player->m_playerInfo->guild->BuyBankTab(this);
		_player->m_playerInfo->guild->LogGuildEvent(GUILD_EVENT_BANKTABBOUGHT, GetPlayer()->GetGUID(), 1, "");
		_player->m_playerInfo->guild->SendGuildBank(this,0);	//trying this as temp fix until i find event GUILD_EVENT_BANKTABBOUGHT
	}
}

void WorldSession::HandleGuildBankGetAvailableAmount(WorldPacket & recv_data)
{
	// calculate using the last withdrawl blablabla
	//this is handled the same on blizz. If player has no guild then there is no reply sent
	if(_player->m_playerInfo->guildMember == NULL)
	{ 
		return;
	}

	uint64 money = _player->m_playerInfo->guild->GetBankBalance();
	uint64 avail = _player->m_playerInfo->guildMember->CalculateAvailableAmount();

	/* pls gm mi hero poor give 1 gold coin pl0x */
	WorldPacket data(MSG_GUILD_BANK_MONEY_WITHDRAWN, 4);
	data << uint64(money>avail ? avail : money);
	SendPacket(&data);
}

void WorldSession::HandleGuildBankModifyTab(WorldPacket & recv_data)
{
/*
13329
{CLIENT} Packet: (0x8D7E) UNKNOWN PacketSize = 36 stamp = 2979650
|------------------------------------------------|----------------|
|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
|------------------------------------------------|----------------|
|23 00 00 00 00 00 C0 F1 00 66 69 72 73 74 00 49 |#........first.I|
|4E 56 5F 31 48 5F 41 75 63 68 69 6E 64 6F 75 6E |NV_1H_Auchindoun|
|5F 30 31 00                                     |_01.            |
-------------------------------------------------------------------
23 00 00 00 00 00 C0 F1 guid
00 slot
66 69 72 73 74 00 tabname
49 4E 56 5F 31 48 5F 41 75 63 68 69 6E 64 6F 75 6E 5F 30 31 00 tabicon                                    
*/
	GuildBankTab * pTab;
	uint64 guid;
	uint8 slot;
	string tabname;
	string tabicon;
	char * ptmp;

	recv_data >> guid;
	recv_data >> slot;
	recv_data >> tabname;
	recv_data >> tabicon;

	if(_player->m_playerInfo->guild==NULL)
	{ 
		return;
	}

	pTab = _player->m_playerInfo->guild->GetBankTab((uint32)slot);
	if(pTab==NULL)
	{ 
		return;
	}

	if( slot >= MAX_GUILD_BANK_TABS )
	{
		return;
	}

	if( ( _player->m_playerInfo->guildRank->iTabPermissions[ slot ].iFlags & ( GRT_RIGHT_UPDATE_NAME | GRT_RIGHT_VIEW_CONTENT ) ) == 0 )
	{
		Guild::SendGuildCommandResult(this, GUILD_MEMBER_S, "", GUILD_PERMISSIONS);
		return;
	}
//	if(_player->m_playerInfo->guild->GetGuildLeader() != _player->GetLowGUID())
	{
//		Guild::SendGuildCommandResult(this, GUILD_MEMBER_S, "", GUILD_PERMISSIONS);
//		return;
	}

	if(tabname.size())
	{
		if( !(pTab->szTabName && strcmp(pTab->szTabName, tabname.c_str()) == 0) )
		{
			ptmp = pTab->szTabName;
			pTab->szTabName = strdup_local(tabname.c_str());
			if(ptmp)
				free(ptmp);
	
			CharacterDatabase.Execute("UPDATE guild_banktabs SET tabName = \"%s\" WHERE guildId = %u AND tabId = %u", 
				CharacterDatabase.EscapeString(tabname).c_str(), _player->m_playerInfo->guild->GetGuildId(), (uint32)slot);
		}
	}
	else
	{
		if(pTab->szTabName)
		{
			ptmp = pTab->szTabName;
			pTab->szTabName = NULL;
			if(ptmp)
				free(ptmp);

			CharacterDatabase.Execute("UPDATE guild_banktabs SET tabName = '' WHERE guildId = %u AND tabId = %u", 
				_player->m_playerInfo->guild->GetGuildId(), (uint32)slot);
		}
	}

	if(tabicon.size())
	{
		if( !(pTab->szTabIcon && strcmp(pTab->szTabIcon, tabicon.c_str()) == 0) )
		{
			ptmp = pTab->szTabIcon;
			pTab->szTabIcon = strdup_local(tabicon.c_str());
			if(ptmp)
				free(ptmp);

			CharacterDatabase.Execute("UPDATE guild_banktabs SET tabIcon = \"%s\" WHERE guildId = %u AND tabId = %u", 
				CharacterDatabase.EscapeString(tabicon).c_str(), _player->m_playerInfo->guild->GetGuildId(), (uint32)slot);
		}
	}
	else
	{
		if(pTab->szTabIcon)
		{
			ptmp = pTab->szTabIcon;
			pTab->szTabIcon = NULL;
			if(ptmp)
				free(ptmp);

			CharacterDatabase.Execute("UPDATE guild_banktabs SET tabIcon = '' WHERE guildId = %u AND tabId = %u", 
				_player->m_playerInfo->guild->GetGuildId(), (uint32)slot);
		}
	}

	// update the client
//	_player->m_playerInfo->guild->SendGuildBankInfo(this);
	_player->m_playerInfo->guild->SendGuildBank(this,0);
}

void WorldSession::HandleGuildBankWithdrawMoney(WorldPacket & recv_data)
{
	uint64 guid;
	uint32 money;
/*
13329
{CLIENT} Packet: (0xDF76) UNKNOWN PacketSize = 16 stamp = 2910526
|------------------------------------------------|----------------|
|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
|------------------------------------------------|----------------|
|23 00 00 00 00 00 C0 F1 10 27 00 00 00 00 00 00 |#........'......|
-------------------------------------------------------------------
*/
	recv_data >> guid;
	recv_data >> money;

	if(_player->m_playerInfo->guild==NULL)
	{ 
		return;
	}

	_player->m_playerInfo->guild->WithdrawMoney(this, money);
}

void WorldSession::HandleGuildBankDepositMoney(WorldPacket & recv_data)
{
	uint64 guid;
	uint32 money;

	recv_data >> guid;
	recv_data >> money;

	if(_player->m_playerInfo->guild==NULL)
	{ 
		return;
	}

	_player->m_playerInfo->guild->DepositMoney(this, money);
}

void WorldSession::HandleGuildBankDepositItem(WorldPacket & recv_data)
{
#ifdef FORCED_GM_TRAINEE_MODE
	if( CanUseCommand('k') && !HasGMPermissions() )
	{
		GetPlayer()->BroadcastMessage( "You are not allowed to use this feature" );
		return;
	}
#endif
	sLog.outDetail("WORLD: got CMSG_GUILD_BANK_SWAP_ITEMS packet");
	uint64 guid;
	uint8 source_isfrombank;
	uint32 wtf;
	uint8 wtf2;
	uint32 i;

	Guild * pGuild = _player->m_playerInfo->guild;
	GuildMember * pMember = _player->m_playerInfo->guildMember;
	
	if(pGuild==NULL || pMember==NULL)
	{ 
		return;
	}
/*
{CLIENT} Packet: (0x557C) CMSG_GUILD_BANK_SWAP_ITEMS PacketSize = 23 stamp = 17486167
|------------------------------------------------|----------------|
|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
|------------------------------------------------|----------------|
|23 00 00 00 00 00 C0 F1 00 00 33 00 00 00 00 00 |#.........3.....|
|FF 18 00 00 00 00 00                            |.......         |
-------------------------------------------------------------------
*/
	recv_data >> guid >> source_isfrombank;
	if(source_isfrombank)
	{
		GuildBankTab * pSourceTab;
		GuildBankTab * pDestTab;
		Item * pSourceItem;
		Item * pDestItem;
		uint8 dest_bank;
		uint8 dest_bankslot;
		uint8 source_bank;
		uint8 source_bankslot;

		/* read packet */
		recv_data >> dest_bank;
		recv_data >> dest_bankslot;
		recv_data >> wtf;
		recv_data >> source_bank;
		recv_data >> source_bankslot;

		/* sanity checks to avoid overflows */
		if(source_bankslot >= MAX_GUILD_BANK_SLOTS ||
			dest_bankslot >= MAX_GUILD_BANK_SLOTS ||
			source_bank >= MAX_GUILD_BANK_TABS ||
			dest_bank >= MAX_GUILD_BANK_TABS)
		{
			return;
		}

		/* make sure we have permissions */
		if(!pMember->pRank->CanPerformBankCommand(GR_RIGHT_GUILD_BANK_DEPOSIT_ITEMS, dest_bank) || 
			!pMember->pRank->CanPerformBankCommand(GR_RIGHT_GUILD_BANK_DEPOSIT_ITEMS, source_bank))
			return;

		/* locate the tabs */
		pSourceTab = pGuild->GetBankTab((uint32)source_bank);
		pDestTab = pGuild->GetBankTab((uint32)dest_bank);
		if(pSourceTab == NULL || pDestTab == NULL)
		{ 
			return;
		}

		pSourceItem = pSourceTab->pSlots[source_bankslot];
		pDestItem = pDestTab->pSlots[dest_bankslot];

		if(pSourceItem == NULL && pDestItem == NULL)
		{ 
			return;
		}
		/* perform the actual swap */
		pSourceTab->pSlots[source_bankslot] = pDestItem;
		pDestTab->pSlots[dest_bankslot] = pSourceItem;

		/* update the client */
		if(pSourceTab == pDestTab)
		{
			/* send both slots in the packet */
			pGuild->SendGuildBank(this, pSourceTab, source_bankslot, dest_bankslot);
		}
		else
		{
			/* send a packet for each different bag */
			pGuild->SendGuildBank(this, pSourceTab, source_bankslot, -1);
			pGuild->SendGuildBank(this, pDestTab, dest_bankslot, -1);
		}

		/* update in sql */
		if(pDestItem == NULL)
		{
			/* this means the source slot is no longer being used. */
			CharacterDatabase.Execute("DELETE FROM guild_bankitems WHERE guildId = %u AND tabId = %u AND slotId = %u",
				pGuild->GetGuildId(), (uint32)pSourceTab->iTabId, (uint32)source_bankslot);
		}
		else
		{
			/* insert the new item */
			CharacterDatabase.Execute("REPLACE INTO guild_bankitems VALUES(%u, %u, %u, %u)", 
				pGuild->GetGuildId(), (uint32)pSourceTab->iTabId, (uint32)source_bankslot, pDestItem->GetLowGUID());
		}

		if(pSourceItem == NULL)
		{
			/* this means the destination slot is no longer being used. */
			CharacterDatabase.Execute("DELETE FROM guild_bankitems WHERE guildId = %u AND tabId = %u AND slotId = %u",
				pGuild->GetGuildId(), (uint32)pDestTab->iTabId, (uint32)dest_bankslot);
		}
		else
		{
			/* insert the new item */
			CharacterDatabase.Execute("REPLACE INTO guild_bankitems VALUES(%u, %u, %u, %u)", 
				pGuild->GetGuildId(), (uint32)pDestTab->iTabId, (uint32)dest_bankslot, pSourceItem->GetLowGUID());
		}
	}
	else
	{
		uint8 source_bagslot;
		uint8 source_slot;
		uint8 dest_bank;
		uint8 dest_bankslot;
		uint8 withdraw_stack=0;
		uint8 deposit_stack=0;
		GuildBankTab * pTab;
		Item * pSourceItem;
		Item * pDestItem;
		Item * pSourceItem2;

		/* read packet */
		recv_data >> dest_bank;
		recv_data >> dest_bankslot;
		recv_data >> wtf;
		recv_data >> wtf2;
		if(wtf2)
			recv_data >> withdraw_stack;

		recv_data >> source_bagslot;
		recv_data >> source_slot;

		if(!(source_bagslot == 1 && source_slot==0))
			recv_data >> wtf2 >> deposit_stack;

		/* sanity checks to avoid overflows */
		if(dest_bank >= MAX_GUILD_BANK_TABS)
		{
			return;
		}

		/* make sure we have permissions */
		if(!pMember->pRank->CanPerformBankCommand(GR_RIGHT_GUILD_BANK_DEPOSIT_ITEMS, dest_bank))
		{ 
			return;
		}

		/* get tab */
		pTab = pGuild->GetBankTab((uint32)dest_bank);
		if(pTab==NULL)
		{ 
			return;
		}

		/* check if we are autoassigning */
		if(dest_bankslot == 0xff)
		{
			for(i = 0; i < MAX_GUILD_BANK_SLOTS; ++i)
			{
				if(pTab->pSlots[i] == NULL)
				{
					dest_bankslot = (uint8)i;
					break;
				}
			}

			if(dest_bankslot==0xff)
			{
				_player->GetItemInterface()->BuildInventoryChangeError(NULL, NULL, INV_ERR_BAG_FULL);
				return;
			}
		}

		/* another check here */
		if(dest_bankslot >= MAX_GUILD_BANK_SLOTS)
		{ 
			return;
		}

		/* check if we're pulling an item from the bank, make sure we're not cheating. */
		pDestItem = pTab->pSlots[dest_bankslot];

		/* grab the source/destination item */
		if(source_bagslot == 1 && source_slot == 0)
		{
			// find a free bag slot
			if(pDestItem == NULL)
			{
				// dis is fucked up mate
				return;
			}

			SlotResult sr = _player->GetItemInterface()->FindFreeInventorySlot(pDestItem->GetProto());
			if(!sr.Result)
			{
				_player->GetItemInterface()->BuildInventoryChangeError(NULL, NULL, INV_ERR_BAG_FULL);
				return;
			}

			source_bagslot = sr.ContainerSlot;
			source_slot = sr.Slot;
		}

		if( source_bagslot==0xff && source_slot < INVENTORY_SLOT_ITEM_START && pDestItem != NULL)
		{
			sCheatLog.writefromsession(this,"Tried to equip an item from the guild bank (WPE HACK)");
			SystemMessage("You don't have permission to do that.");
			return;
		}

		if(pDestItem != NULL)
		{
			if(pMember->pRank->iTabPermissions[dest_bank].iStacksPerDay == 0)
			{
				SystemMessage("You don't have permission to do that.");
				return;
			}

			if(pMember->pRank->iTabPermissions[dest_bank].iStacksPerDay > 0)
			{
				if(pMember->CalculateAllowedItemWithdraws(dest_bank) == 0)
				{
					// a "no permissions" notice would probably be better here
					SystemMessage("You have withdrawn the maximum amount for today.");
					return;
				}

				/* reduce his count by one */
				pMember->OnItemWithdraw(dest_bank);
			}
		}

		pSourceItem = _player->GetItemInterface()->GetInventoryItem(source_bagslot, source_slot);

		/* make sure that both arent null - wtf ? */
		if(pSourceItem == NULL && pDestItem == NULL)
		{ 
			return;
		}

		if(pSourceItem != NULL)
		{
			// make sure its not a soulbound item
			if(pSourceItem->IsSoulbound() || pSourceItem->GetProto()->Class == ITEM_CLASS_QUEST)
			{
				_player->GetItemInterface()->BuildInventoryChangeError(NULL, NULL, INV_ERR_CANT_DROP_SOULBOUND);
				return;
			}

			// pull the item from the slot
			if(deposit_stack && pSourceItem->GetUInt32Value(ITEM_FIELD_STACK_COUNT) > deposit_stack)
			{
				pSourceItem2 = pSourceItem;
				pSourceItem = objmgr.CreateItem(pSourceItem2->GetEntry(), _player);
				pSourceItem->SetUInt32Value(ITEM_FIELD_STACK_COUNT, deposit_stack);
				pSourceItem->SetUInt32Value(ITEM_FIELD_CREATOR, pSourceItem2->GetUInt32Value(ITEM_FIELD_CREATOR));
				pSourceItem2->ModUnsigned32Value(ITEM_FIELD_STACK_COUNT, -(int32)deposit_stack);
				pSourceItem2->m_isDirty=true;
			}
			else
			{
				if(!_player->GetItemInterface()->SafeRemoveAndRetreiveItemFromSlot(source_bagslot, source_slot, false))
				{ 
					return;
				}

				pSourceItem->RemoveFromWorld();
			}
		}

		/* perform the swap. */
		/* pSourceItem = Source item from players backpack coming into guild bank */
		if(pSourceItem == NULL)
		{
			/* splitting */
			if(pDestItem != NULL && deposit_stack>0 && pDestItem->GetUInt32Value(ITEM_FIELD_STACK_COUNT) > deposit_stack)
			{
				pSourceItem2 = pDestItem;

				pSourceItem2->ModUnsigned32Value(ITEM_FIELD_STACK_COUNT, -(int32)deposit_stack);
				pSourceItem2->SaveToDB(0,0,true, NULL);

				pDestItem = objmgr.CreateItem(pSourceItem2->GetEntry(), _player);
				pDestItem->SetUInt32Value(ITEM_FIELD_STACK_COUNT, deposit_stack);
				pDestItem->SetUInt32Value(ITEM_FIELD_CREATOR, pSourceItem2->GetUInt32Value(ITEM_FIELD_CREATOR));
			}
			else
			{
				/* that slot in the bank is now empty. */
				pTab->pSlots[dest_bankslot] = NULL;
				CharacterDatabase.Execute("DELETE FROM guild_bankitems WHERE guildId = %u AND tabId = %u AND slotId = %u",
					pGuild->GetGuildId(), (uint32)pTab->iTabId, (uint32)dest_bankslot);
			}			
		}
		else
		{
			/* there is a new item in that slot. */
			pTab->pSlots[dest_bankslot] = pSourceItem;
			CharacterDatabase.Execute("REPLACE INTO guild_bankitems VALUES(%u, %u, %u, %u)", 
				pGuild->GetGuildId(), (uint32)pTab->iTabId, (uint32)dest_bankslot, pSourceItem->GetLowGUID());

			/* remove the item's association with the player */
			pSourceItem->SetOwner(NULL);
			pSourceItem->SetUInt32Value(ITEM_FIELD_OWNER, 0);
			pSourceItem->SaveToDB(0, 0, true, NULL);

			/* log it */
			pGuild->LogGuildBankAction(GUILD_BANK_LOG_EVENT_DEPOSIT_ITEM, _player->GetLowGUID(), pSourceItem->GetEntry(), 
				(uint8)pSourceItem->GetUInt32Value(ITEM_FIELD_STACK_COUNT), pTab);
		}

		/* pDestItem = Item from bank coming into players backpack */
		if(pDestItem == NULL)
		{
			/* the item has already been removed from the players backpack at this stage,
			   there isnt really much to do at this point. */			
		}
		else
		{
			/* the guild was robbed by some n00b! :O */
			pDestItem->SetOwner(_player);
			pDestItem->SetUInt32Value(ITEM_FIELD_OWNER, _player->GetLowGUID());
			pDestItem->SaveToDB(source_bagslot, source_slot, true, NULL);

			/* add it to him in game */
			if( _player->GetItemInterface()->SafeAddItem(pDestItem, source_bagslot, source_slot) == ADD_ITEM_RESULT_ERROR )
			{
				/* this *really* shouldn't happen. */
				if(!_player->GetItemInterface()->AddItemToFreeSlot(&pDestItem))
				{
					//pDestItem->DeleteFromDB();
					pDestItem->DeleteMe();
					pDestItem = NULL;
				}
			}
			else
			{
				/* log it */
				pGuild->LogGuildBankAction(GUILD_BANK_LOG_EVENT_WITHDRAW_ITEM, _player->GetLowGUID(), pDestItem->GetEntry(), 
					(uint8)pDestItem->GetUInt32Value(ITEM_FIELD_STACK_COUNT), pTab);
			}
		}

		/* update the clients view of the bank tab */
		pGuild->SendGuildBank(this, pTab, dest_bankslot);
	}		
}

void WorldSession::HandleGuildBankOpenVault(WorldPacket & recv_data)
{
/*
13329
{CLIENT} Packet: (0x045E) CMSG_GUILD_BANKER_ACTIVATE PacketSize = 9 stamp = 1518108
|------------------------------------------------|----------------|
|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
|------------------------------------------------|----------------|
|23 00 00 00 00 00 C0 F1 01                      |#........       |
-------------------------------------------------------------------
*/
	GameObject * pObj;
	uint64 guid;

	if(!_player->IsInWorld() || _player->m_playerInfo->guild==NULL)
	{
		Guild::SendGuildCommandResult(this, GUILD_CREATE_S, "", GUILD_PLAYER_NOT_IN_GUILD);
		return;
	}

	recv_data >> guid;
	pObj = _player->GetMapMgr()->GetGameObject( guid );
	if(pObj==NULL)
	{ 
		return;
	}

//	_player->m_playerInfo->guild->SendGuildBankInfo(this);
	_player->m_playerInfo->guild->SendGuildBank(this,0);
}

void WorldSession::HandleGuildBankViewTab(WorldPacket & recv_data)
{
	uint64 guid;
	uint8 tabid;
	GuildBankTab * pTab;
	Guild * pGuild = _player->m_playerInfo->guild;

/*
{CLIENT} Packet: (0xA623) CMSG_GUILD_BANK_QUERY_TAB PacketSize = 10 stamp = 10458837
|------------------------------------------------|----------------|
|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
|------------------------------------------------|----------------|
|1E 00 00 00 00 00 12 F1 00 00                   |..........      |
-------------------------------------------------------------------
*/
	recv_data >> guid;
	recv_data >> tabid;

	//Log.Warning("HandleGuildBankViewTab", "Tab %u", (uint32)tabid);

	// maybe last uint8 is "show additional info" such as tab names? *shrug*
	if(pGuild==NULL)
	{ 
		return;
	}

	pTab = pGuild->GetBankTab((uint32)tabid);
	if(pTab==NULL)
	{ 
		return;
	}

	pGuild->SendGuildBank(this, pTab);
}

#if REALLY_NEED_REDUNDANCY
void Guild::SendGuildBankInfo(WorldSession * pClient)
{
	GuildMember * pMember = pClient->GetPlayer()->m_playerInfo->guildMember;

	if(pMember==NULL)
	{ 
		return;
	}

/*
96 65 AB 05 00 00 00 00 -balance
00 ?
FF FF FF FF withdraw limit
01 tab info
04 - 4 tabs
54 61 62 31 00 - tabname -Tab1 
49 4E 56 5F 49 6E 73 63 72 69 70 74 69 6F 6E 5F 4D 69 6E 6F 72 47 6C 79 70 68 30 39 00 - icon name ? INV_Inscription_MinorGlyph09 
54 61 62 32 00 - Tab2
49 4E 56 5F 45 6E 63 68 61 6E 74 5F 45 73 73 65 6E 63 65 43 6F 73 6D 69 63 47 72 65 61 74 65 72 00 - INV_Enchant_EssenceCosmicGreater 
54 61 62 33 00 - Tab3 
49 4E 56 5F 53 63 72 6F 6C 6C 5F 31 31 00 - INV_Scroll_11 
54 61 62 34 00 - Tab4 
5F 6F 72 62 5F 33 00 - _orb_3 
44 ? seems to be some counter ? 68
00 
F8 0D 00 00 
20 80 00 00 
00 00 00 00 
09 00 00 00 
00 00 00 00 
00 00 
01 
6C B3 00 00 
20 80 00 00 
00 00 00 00 
0A 00 00 00
00 00 00 00
01 00 
02 
7E AA 00 00 20 80 00 00 00 00 00 00 02 00 00 00 00 00 00 00 01 00 03 E2 2A 00 00 20 80 00 00 00 00 00 00 14 00 00 00 00 00 00 00 00 00 04 49 93 00 00 00 80 00 00 00 00 00 00 0A 00 00 00 00 00 00 00 01 00 05 AE 8F 00 00 00 80 00 00 00 00 00 00 14 00 00 00 00 00 00 00 00 00 06 D8 9B 00 00 20 80 00 00 00 00 00 00 01 00 00 00 00 00 00 00 01 00 07 F8 0D 00 00 20 80 00 00 00 00 00 00 14 00 00 00 00 00 00 00 00 00 08 AE 8F 00 00 00 80 00 00 00 00 00 00 14 00 00 00 00 00 00 00 00 00 09 46 93 00 00 00 80 00 00 00 00 00 00 03 00 00 00 00 00 00 00 01 00 0A 49 93 00 00 00 80 00 00 00 00 00 00 05 00 00 00 00 00 00 00 01 00 0B 50 A3 00 00 00 80 00 00 00 00 00 00 14 00 00 00 00 00 00 00 00 00 0C 2C 74 00 00 00 80 00 00 00 00 00 00 03 00 00 00 00 00 00 00 00 00 0D F1 7F 00 00 20 80 00 00 00 00 00 00 01 00 00 00 00 00 00 00 01 00 0E 1E 6B 00 00 00 80 00 00 00 00 00 00 01 00 00 00 00 00 00 00 00 00 0F 48 93 00 00 00 80 00 00 00 00 00 00 0A 00 00 00 00 00 00 00 01 00 10 C6 87 00 00 00 80 00 00 00 00 00 00 0E 00 00 00 00 00 00 00 01 00 11 83 A2 00 00 00 80 00 00 00 00 00 00 05 00 00 00 00 00 00 00 01 00 12 01 1A 00 00 00 80 00 00 00 00 00 00 0A 00 00 00 00 00 00 00 01 00 13 97 0E 00 00 00 80 00 00 00 00 00 00 01 00 00 00 00 00 00 00 01 00 14 E2 2A 00 00 20 80 00 00 00 00 00 00 12 00 00 00 00 00 00 00 00 00 15 CD 87 00 00 20 80 00 00 00 00 00 00 05 00 00 00 00 00 00 00 01 00 16 A9 06 00 00 00 80 00 00 00 00 00 00 01 00 00 00 00 00 00 00 00 00 17 39 5A 00 00 20 80 00 00 00 00 00 00 01 00 00 00 00 00 00 00 00 00 18 3C 5A 00 00 20 80 00 00 00 00 00 00 01 00 00 00 00 00 00 00 00 00 19 CD A6 00 00 20 80 00 00 00 00 00 00 01 00 00 00 00 00 00 00 00 00 1A C8 87 00 00 20 80 00 00 00 00 00 00 0F 00 00 00 00 00 00 00 01 00 1B BA 47 00 00 20 80 00 00 00 00 00 00 01 00 00 00 00 00 00 00 00 00 1F F9 05 00 00 00 80 00 00 00 00 00 00 01 00 00 00 00 00 00 00 00 00 20 58 B3 00 00 00 80 00 00 00 00 00 00 01 00 00 00 00 00 00 00 01 00 21 FC 55 00 00 20 00 00 00 00 00 00 00 01 00 00 00 00 00 00 00 01 00 22 6B 59 00 00 20 80 00 00 00 00 00 00 01 00 00 00 00 00 00 00 01 00 23 DD 52 00 00 00 80 00 00 00 00 00 00 04 00 00 00 00 00 00 00 01 00 24 04 A9 00 00 20 80 00 00 00 00 00 00 06 00 00 00 00 00 00 00 01 00 26 7A 19 00 00 00 80 00 00 00 00 00 00 03 00 00 00 00 00 00 00 01 00 27 5E A8 00 00 00 80 00 00 00 00 00 00 01 00 00 00 00 00 00 00 00 00 28 4E A3 00 00 00 80 00 00 00 00 00 00 05 00 00 00 00 00 00 00 00 00 38 C5 9F 00 00 00 80 00 00 00 00 00 00 05 00 00 00 00 00 00 00 01 00 39 D3 9F 00 00 20 80 00 00 00 00 00 00 06 00 00 00 00 00 00 00 01 00 3A DC A7 00 00 00 80 00 00 00 00 00 00 04 00 00 00 00 00 00 00 01 00 3B 9D A5 00 00 00 80 00 00 00 00 00 00 09 00 00 00 00 00 00 00 01 00 3C 9C A5 00 00 00 80 00 00 00 00 00 00 05 00 00 00 00 00 00 00 01 00 3F D7 9F 00 00 00 80 00 00 00 00 00 00 06 00 00 00 00 00 00 00 01 00 40 D4 9F 00 00 00 80 00 00 00 00 00 00 05 00 00 00 00 00 00 00 01 00 41 DE A7 00 00 00 80 00 00 00 00 00 00 05 00 00 00 00 00 00 00 01 00 42 DA A7 00 00 00 80 00 00 00 00 00 00 04 00 00 00 00 00 00 00 01 00 43 9F A7 00 00 00 80 00 00 00 00 00 00 05 00 00 00 00 00 00 00 01 00 46 A0 A9 00 00 00 80 00 00 00 00 00 00 0A 00 00 00 00 00 00 00 01 00 47 A4 A9 00 00 00 80 00 00 00 00 00 00 05 00 00 00 00 00 00 00 01 00 48 44 A2 00 00 00 80 00 00 00 00 00 00 05 00 00 00 00 00 00 00 01 00 49 95 A0 00 00 00 80 00 00 00 00 00 00 07 00 00 00 00 00 00 00 01 00 4A 94 A0 00 00 20 80 00 00 00 00 00 00 08 00 00 00 00 00 00 00 01 00 4D A8 A9 00 00 00 80 00 00 00 00 00 00 09 00 00 00 00 00 00 00 01 00 4E 96 A9 00 00 00 80 00 00 00 00 00 00 13 00 00 00 00 00 00 00 01 00 4F F9 A6 00 00 00 80 00 00 00 00 00 00 06 00 00 00 00 00 00 00 01 00 50 89 A0 00 00 00 80 00 00 00 00 00 00 05 00 00 00 00 00 00 00 01 00 51 86 A0 00 00 00 80 00 00 00 00 00 00 05 00 00 00 00 00 00 00 01 00 54 21 AA 00 00 00 80 00 00 00 00 00 00 04 00 00 00 00 00 00 00 01 00 55 17 AA 00 00 00 80 00 00 00 00 00 00 05 00 00 00 00 00 00 00 01 00 56 32 AB 00 00 00 80 00 00 00 00 00 00 06 00 00 00 00 00 00 00 01 00 57 20 AA 00 00 00 80 00 00 00 00 00 00 02 00 00 00 00 00 00 00 01 00 58 E2 A5 00 00 00 80 00 00 00 00 00 00 05 00 00 00 00 00 00 00 01 00 5B 31 AB 00 00 00 80 00 00 00 00 00 00 05 00 00 00 00 00 00 00 01 00 5C 10 AA 00 00 20 80 00 00 00 00 00 00 0B 00 00 00 00 00 00 00 01 00 5D 1F AA 00 00 00 80 00 00 00 00 00 00 05 00 00 00 00 00 00 00 01 00 5E 33 AB 00 00 00 80 00 00 00 00 00 00 04 00 00 00 00 00 00 00 01 00 5F 1A AA 00 00 00 80 00 00 00 00 00 00 04 00 00 00 00 00 00 00 01 00 60 D7 A5 00 00 20 80 00 00 00 00 00 00 04 00 00 00 00 00 00 00 01 00 

13329
{SERVER} Packet: (0xBC76) SMSG_GUILD_BANK_LIST PacketSize = 265 TimeStamp = 13161484
64 25 89 1A 
00 00 00 00 00 00 00 00 
00 01 
07 - 7 tabs
50 6F 74 69 6F 6E 73 20 48 65 72 62 73 00 - tabname
49 4E 56 5F 41 6C 63 68 65 6D 79 5F 45 6E 63 68 61 6E 74 65 64 56 69 61 6C 00 - tab icon name
45 6E 63 68 61 6E 74 20 53 63 72 6F 6C 6C 73 00 
49 4E 56 5F 4D 69 73 63 5F 44 72 61 67 6F 6E 4B 69 74 65 5F 30 33 00 
47 65 6D 20 43 6C 6F 74 68 00 
49 4E 56 5F 4A 65 77 65 6C 63 72 61 66 74 69 6E 67 5F 53 74 61 72 4F 66 45 6C 75 6E 65 5F 30 33 00 
45 70 69 63 73 20 52 65 63 65 70 69 65 73 00 
49 4E 56 5F 41 78 65 5F 31 48 5F 43 61 74 61 63 6C 79 73 6D 5F 43 5F 30 31 00 
46 6F 6F 64 20 44 72 69 6E 6B 73 00 
49 4E 56 5F 4D 69 73 63 5F 46 6F 6F 64 5F 35 33 00 
4F 6C 64 20 45 6E 63 68 61 6E 74 00 
49 4E 56 5F 44 61 74 61 43 72 79 73 74 61 6C 30 34 00 
4F 6C 64 20 53 74 75 66 66 00 
49 4E 56 5F 4D 69 73 63 5F 43 6F 69 6E 5F 31 33 00 
00 
*/
	WorldPacket data(SMSG_GUILD_BANK_LIST, 500);
	data << uint64(m_bankBalance);
	data << uint8(0);	//this is tab id
	data << uint32(0);	//no withdraw info
	data << uint8(1);// Packet type: 0-tab content, 1-tab info,
	data << uint8(m_bankTabCount);

	for(uint32 i = 0; i < m_bankTabCount; ++i)
	{
		GuildBankTab * pTab = GetBankTab(i);
		if(pTab==NULL || !pMember->pRank->CanPerformBankCommand(GR_RIGHT_GUILD_BANK_VIEW_TAB, i))
		{
			data << uint16(0);		// shouldn't happen
			continue;
		}

		if(pTab->szTabName)
			data << pTab->szTabName;
		else
			data << uint8(0);

		if(pTab->szTabIcon)
			data << pTab->szTabIcon;
		else
			data << uint8(0);
	}

	data << uint8(0);	//we are not sending items
	pClient->SendPacket(&data);
}
#endif

void Guild::SendGuildBank(WorldSession * pClient, GuildBankTab * pTab, int8 updated_slot1 /* = -1 */, int8 updated_slot2 /* = -1 */)
{
	size_t pos;
	uint32 count=0;
	uint32 pTabID;

	if( pTab == NULL )
		pTabID = 0;
	else
		pTabID = pTab->iTabId;

	WorldPacket data(SMSG_GUILD_BANK_LIST, 1100);
	GuildMember * pMember = pClient->GetPlayer()->m_playerInfo->guildMember;

	if(pMember==NULL || !pMember->pRank->CanPerformBankCommand(GR_RIGHT_GUILD_BANK_VIEW_TAB, pTabID))
	{ 
		return;
	}

	//Log.Debug("SendGuildBank", "sending tab %u to client.", pTab->iTabId);
/*
96 65 AB 05 00 00 00 00 -balance
00 ?
FF FF FF FF withdraw limit
01 tab info
04 - 4 tabs
54 61 62 31 00 - tabname -Tab1 
49 4E 56 5F 49 6E 73 63 72 69 70 74 69 6F 6E 5F 4D 69 6E 6F 72 47 6C 79 70 68 30 39 00 - icon name ? INV_Inscription_MinorGlyph09 
54 61 62 32 00 - Tab2
49 4E 56 5F 45 6E 63 68 61 6E 74 5F 45 73 73 65 6E 63 65 43 6F 73 6D 69 63 47 72 65 61 74 65 72 00 - INV_Enchant_EssenceCosmicGreater 
54 61 62 33 00 - Tab3 
49 4E 56 5F 53 63 72 6F 6C 6C 5F 31 31 00 - INV_Scroll_11 
54 61 62 34 00 - Tab4 
5F 6F 72 62 5F 33 00 - _orb_3 
44 ? seems to be some counter ? 68
00 slot
F8 0D 00 00 item entry
20 80 00 00 random property ID
00 00 00 00 stack count
09 00 00 00 unk 2.4
00 00 00 00 unk 3.3
00 ? 0 or 1
00 -enchant count
01 6C B3 00 00 20 80 00 00 00 00 00 00 0A 00 00 00 00 00 00 00 01 00 
02 7E AA 00 00 20 80 00 00 00 00 00 00 02 00 00 00 00 00 00 00 01 00 
03 E2 2A 00 00 20 80 00 00 00 00 00 00 14 00 00 00 00 00 00 00 00 00 10978 Small Glimmering Shard
04 49 93 00 00 00 80 00 00 00 00 00 00 0A 00 00 00 00 00 00 00 01 00 
05 AE 8F 00 00 00 80 00 00 00 00 00 00 14 00 00 00 00 00 00 00 00 00 ......

{SERVER} Packet: (0xBC76) SMSG_GUILD_BANK_LIST PacketSize = 885 TimeStamp = 13178906
06 2A 89 1A 00 00 00 00 deposit count ?
00 tab id ?
00 00 00 00 allowed withdraw
01 - seems to be the same
07 -> send tab count and tab name / icon if tab id=0 ?
50 6F 74 69 6F 6E 73 20 48 65 72 62 73 00 
49 4E 56 5F 41 6C 63 68 65 6D 79 5F 45 6E 63 68 61 6E 74 65 64 56 69 61 6C 00 
45 6E 63 68 61 6E 74 20 53 63 72 6F 6C 6C 73 00 
49 4E 56 5F 4D 69 73 63 5F 44 72 61 67 6F 6E 4B 69 74 65 5F 30 33 00 
47 65 6D 20 43 6C 6F 74 68 00 
49 4E 56 5F 4A 65 77 65 6C 63 72 61 66 74 69 6E 67 5F 53 74 61 72 4F 66 45 6C 75 6E 65 5F 30 33 00 
45 70 69 63 73 20 52 65 63 65 70 69 65 73 00 
49 4E 56 5F 41 78 65 5F 31 48 5F 43 61 74 61 63 6C 79 73 6D 5F 43 5F 30 31 00 
46 6F 6F 64 20 44 72 69 6E 6B 73 00 
49 4E 56 5F 4D 69 73 63 5F 46 6F 6F 64 5F 35 33 00 
4F 6C 64 20 45 6E 63 68 61 6E 74 00 
49 4E 56 5F 44 61 74 61 43 72 79 73 74 61 6C 30 34 00 
4F 6C 64 20 53 74 75 66 66 00 
49 4E 56 5F 4D 69 73 63 5F 43 6F 69 6E 5F 31 33 00 
14 - tabs items
00 F9 CE 00 00 00 00 03 00 00 00 00 00 0A 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
01 F9 CE 00 00 00 00 03 00 00 00 00 00 14 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
02 F9 CE 00 00 00 00 03 00 00 00 00 00 14 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
06 2A B5 00 00 00 00 03 00 00 00 00 00 14 00 00 00 00 00 00 00 01 00 00 00 00 00 00 00 00 00 ...

{SERVER} Packet: (0xBC76) SMSG_GUILD_BANK_LIST PacketSize = 1441 TimeStamp = 14282093
EC 41 8A 1A 00 00 00 00 deposit
01 tab ID
00 00 00 00 balance ?
01 - tab info
2E - 46 item count
00 4B CD 00 00 00 00 03 00 00 00 00 00 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
07 F2 CD 00 00 00 00 03 00 00 00 00 00 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
08 F1 CD 00 00 00 00 03 00 00 00 00 00 14 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
0D EE CD 00 00 20 00 03 00 00 00 00 00 02 00 00 00 00 00 00 00 01 00 00 00 00 00 00 00 00 00 
17 CF A0 00 00 00 80 03 00 00 00 00 00 03 00 00 00 00 00 00 00 01 00 00 00 00 00 00 00 00 00 .....

{SERVER} Packet: (0xEA6A) SMSG_GUILD_BANK_LIST PacketSize = 46 TimeStamp = 39389300
00 00 00 00 00 00 00 00 
00 
FF FF FF FF - withdraw
00 ? -> no name blocks present ?
01 
08 51 91 00 
00 20 
00 03 00 00 00 00 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 

{SERVER} Packet: (0xEA6A) SMSG_GUILD_BANK_LIST PacketSize = 140 TimeStamp = 18219855
35 E7 16 00 00 00 00 00 
00 - tabid 
02 00 00 00 - withdraw 
01 - 1?
04 - tabcount
50 72 6F 66 66 20 4D 61 74 73 00 - Proff Mats 
49 4E 56 5F 41 6C 63 68 65 6D 79 5F 43 72 79 73 74 61 6C 56 69 61 6C 00 - INV_Alchemy_CrystalVial 
42 41 47 53 00 49 4E 56 5F 48 6F 6C 69 64 61 79 5F 43 68 72 69 73 74 6D 61 73 5F 50 72 65 73 65 6E 74 5F 30 31 00 
50 6F 74 73 00 
49 4E 56 5F 50 6F 74 69 6F 6E 5F 30 39 00 46 6F 6F 64 20 61 6E 64 20 44 72 69 6E 6B 00 
49 4E 56 5F 4D 69 73 63 5F 46 6F 6F 64 5F 36 34 00 
00 
*/

	data << uint64( m_bankBalance );			// amount you have deposited
	data << uint8( pTabID );	// unknown
	data << uint32( pMember->CalculateAllowedItemWithdraws( pTabID ) );		// remaining stacks for this day
	uint8 SendGuildTabText = ( pTabID == 0 ) && ( m_bankTabCount != 0 );
	data << uint8( SendGuildTabText );		// permission to view it client side ?

	if( SendGuildTabText )
	{
		data << uint8(m_bankTabCount);

		for(uint32 i = 0; i < m_bankTabCount; ++i)
		{
			GuildBankTab * pTabt = GetBankTab(i);
			if(pTabt==NULL || !pMember->pRank->CanPerformBankCommand(GR_RIGHT_GUILD_BANK_VIEW_TAB, i))
			{
				data << uint16(0);		// shouldn't happen
				continue;
			}

			if(pTabt->szTabName)
				data << pTabt->szTabName;
			else
				data << uint8(0);

			if(pTabt->szTabIcon)
				data << pTabt->szTabIcon;
			else
				data << uint8(0);
		}
	}

	// no need to send tab names here..
	pos = data.wpos();
	data << uint8(0);		// number of items, will be filled later

	if( pTab != NULL )
	{
		for(int32 j = 0; j < MAX_GUILD_BANK_SLOTS; ++j)
		{
			if(pTab->pSlots[j] != NULL)
			{
				if(updated_slot1 >= 0 && j == updated_slot1)
					updated_slot1 = -1;

				if(updated_slot2 >= 0 && j == updated_slot2)
					updated_slot2 = -1;

				++count;

				// what i don't understand here, where is the field for random properties? :P - Burlex
				data << uint8(j);			// slot
				data << pTab->pSlots[j]->GetEntry();

	//			data << (uint32)pTab->pSlots[j]->GetItemRandomPropertyId();
	//			if (pTab->pSlots[j]->GetItemRandomPropertyId())
	//				data << (uint32)pTab->pSlots[j]->GetItemRandomSuffixFactor();
				data << uint32(0x00030000);			//some flags, met 3-4 types : 20 80 02 00,20 80 03 00,00 00 03 00
				data << uint32(0);			// unknown value
				data << uint32(pTab->pSlots[j]->GetUInt32Value(ITEM_FIELD_STACK_COUNT));
				data << uint32(0);			// unknown value, sometimes it is 1
				data << uint32(0);			// all were 0 in the logs i made
				data << uint32(0);			// unknown value, sometimes it is 1
				data << uint8(0);			// unknown 2.4.2 - it is 0 or 1
				uint32 Enchant0 = 0;
				if (pTab->pSlots[j]->GetEnchantment(0))
					Enchant0 = pTab->pSlots[j]->GetEnchantment(0)->Enchantment->Id;
				if (Enchant0)
				{
					data << uint8(1);			// number of enchants
					data << uint8(0);			// enchantment slot
					data << uint32(Enchant0);	// enchantment id
				}
				else
					data << uint8(0);		// no enchantment
			}
		}

		// send the forced update slots
		if(updated_slot1 >= 0)
		{
			// this should only be hit if the items null though..
			if(pTab->pSlots[updated_slot1]==NULL)
			{
				++count;
				data << uint8(updated_slot1);
				data << uint32(0);
			}
		}

		if(updated_slot2 >= 0)
		{
			// this should only be hit if the items null though..
			if(pTab->pSlots[updated_slot2]==NULL)
			{
				++count;
				data << uint8(updated_slot2);
				data << uint32(0);
			}
		}
		*(uint8*)&data.contents()[pos] = (uint8)count;// push number of items
	}

	pClient->SendPacket(&data);
}

void WorldSession::HandleGuildGetFullPermissions(WorldPacket & recv_data)
{
	WorldPacket data(MSG_GUILD_PERMISSIONS, 61);
	GuildRank * pRank = _player->GetGuildRankS();
	uint32 i;

	if(_player->GetGuild() == NULL)
	{ 
		return;
	}

	data << pRank->iId;
	data << pRank->iRights;
	data << pRank->iGoldLimitPerDay;
	data << uint8(_player->GetGuild()->GetBankTabCount());

	for(i = 0; i < MAX_GUILD_BANK_TABS; ++i) {
		data << pRank->iTabPermissions[i].iFlags;
		data << pRank->iTabPermissions[i].iStacksPerDay;
	}

	SendPacket(&data);
}

void WorldSession::HandleGuildBankViewLog(WorldPacket & recv_data)
{
	/* slot 6 = i'm requesting money log */
	uint8 slotid;
	recv_data >> slotid;

	if(_player->GetGuild() == NULL)
	{ 
		return;
	}

	_player->GetGuild()->SendGuildBankLog(this, slotid);
}

void WorldSession::HandleGuildBankQueryText( WorldPacket & recv_data )
{
	if( _player->GetGuild() == NULL )
	{ 
		return;
	}

	uint8 tabid;
	recv_data >> tabid;

	GuildBankTab * tab = _player->GetGuild()->GetBankTab( tabid );
	if( tab == NULL )
	{ 
		return;
	}

	uint32 len = tab->szTabInfo != NULL ? (uint32)strlen( tab->szTabInfo ) : 1;

	WorldPacket data( MSG_QUERY_GUILD_BANK_TEXT, 1 + len );
	data << tabid;
	
	if( tab->szTabInfo != NULL )
		data << tab->szTabInfo;
	else 
		data << uint8( 0 );

	SendPacket( &data );
}

void WorldSession::HandleSetGuildBankText( WorldPacket & recv_data )
{
	GuildMember * pMember = _player->m_playerInfo->guildMember;

	if( _player->GetGuild() == NULL || pMember == NULL )
	{ 
		return;
	}

/*
13329
{CLIENT} Packet: (0x7D5E) UNKNOWN PacketSize = 17 stamp = 3205711
|------------------------------------------------|----------------|
|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
|------------------------------------------------|----------------|
|00 67 75 69 6C 64 20 62 61 6E 6B 20 74 65 78 74 |.guild bank text|
|00                                              |.               |
-------------------------------------------------------------------
*/
	uint8 tabid;
	string text;

	recv_data >> tabid >> text;

	GuildBankTab * tab = _player->GetGuild()->GetBankTab( tabid );
	if( tab != NULL && 
		pMember->pRank->CanPerformBankCommand( GR_RIGHT_GUILD_BANK_CHANGE_TABTXT, tabid ) )
	{
		tab->szTabInfo = strdup_local( text.c_str() );
		char txt[2];
		txt[0] = 0x30 + tabid;	//textify
		txt[1] = 0;
		_player->GetGuild()->LogGuildEvent( GUILD_EVENT_TABINFO, 0, 1, txt );
	
		CharacterDatabase.Execute("UPDATE guild_banktabs SET tabInfo = \"%s\" WHERE guildId = %u AND tabId = %u", 
			CharacterDatabase.EscapeString(text).c_str(), _player->m_playerInfo->guild->GetGuildId(), (uint32)tabid );
	}
}

void WorldSession::HandleGuildQueryRanks(WorldPacket & recv_data)
{
	if( GetPlayer()->m_playerInfo->guild == NULL )
	{ 
		return;
	}

//	uint64 GuildGUID;
//	recv_data >> GuildGUID;
//	uint32 guildId = GET_GUILD_ID( GuildGUID );
//	Guild * pGuild = objmgr.GetGuild( guildId );
//	if( !pGuild || GetPlayer()->m_playerInfo->guild != pGuild )
//	{ 
//		sLog.outDebug("Player is not in this guild");
//		return;
//	}
	Guild * pGuild = GetPlayer()->m_playerInfo->guild;
	if( pGuild )
	{
		pGuild->SendGuildRankQuery( this );
/*
143333
{SERVER} Packet: (0x1B23) SMSG_GUILD_QUERY_UNK_STATUS PacketSize = 26 TimeStamp = 12365356
C0 
03 00 00 00 
07 00 00 00 
F0 A6 76 00 
1F 00 00 00 
74 65 73 74 00 msg
01 00 00 00 
*/
		//[OpcodeDescription("uint8 flags, if (0x80) { uint32s, string, uint32s }")]
		sStackWorldPacket( data, SMSG_GUILD_FINDER_STATUS_GUILD_RECRUITING, 21 + 100 + 10 );
		if( pGuild->m_GF_is_public == false || pGuild->GetGuildLeader() != GetPlayer()->GetGUID() )
			data << uint8( 0 );
		else
		{
			data << uint8( 0xC0 );
			data << uint32( pGuild->GetGuildFinderFilterActivity() );
			data << uint32( pGuild->GetGuildFinderFilterRole() );
			data << uint32( 0x0076A6F0 ); //?
			data << uint32( pGuild->GetGuildFinderFilterInterest() );
			data << pGuild->GetGuildFinderDesc();
			data << uint32( pGuild->GetGuildFinderFilterLevel() );
		}
		SendPacket( &data );
	}
}

void WorldSession::HandleGuildQueryUnk(WorldPacket & recv_data)
{
	if( GetPlayer()->m_playerInfo->guild == NULL )
	{ 
		return;
	}

/*
13329
{CLIENT} Packet: (0x1288) CMSG_GUILD_QUERY_UNK PacketSize = 16 TimeStamp = 13564349
CA 7E A9 00 00 00 F3 1F guild guid
0C 40 5F 03 00 00 00 03 our guid
{SERVER} Packet: (0x18E0) SMSG_GUILD_QUERY_UNK_RESPONSE PacketSize = 9 TimeStamp = 13564598
00 
00 00 00 00 
00 00 00 00 
when inside WSG
00 
08 00 00 00 
01 00 00 00
*/
	sLog.outDebug("Warning, sending a temp implemented packet with static content. Need to remake this later");
	uint64 GuildGUID;
	recv_data >> GuildGUID;
	uint32 guildId = GET_GUILD_ID( GuildGUID );
	Guild * pGuild = objmgr.GetGuild( guildId );
	if( !pGuild || GetPlayer()->m_playerInfo->guild != pGuild )
	{ 
		return;
	}

	WorldPacket data(SMSG_GUILD_QUERY_UNK_RESPONSE, 1100);
	data << uint8( 0 );
//	if( GetPlayer()->GetGroup() )
//	{
//		data << uint32( 8 );
//		data << uint32( 1 );
//	}
//	else
	{
		data << uint32( 0 );
		data << uint32( 0 );
	}
	SendPacket(&data);
}

void WorldSession::HandleGuildUnk(WorldPacket & recv_data)
{
	if( GetPlayer()->m_playerInfo->guild == NULL )
	{ 
		return;
	}
/*
13329
{CLIENT} Packet: (0x2E54) MSG_UNKNOWN PacketSize = 0 TimeStamp = 13564364
{SERVER} Packet: (0x2E54) MSG_UNKNOWN PacketSize = 77 TimeStamp = 13564552
04 00 00 00 43 60 00 00 00 00 00 00 02 03 00 00 00 05 00 00 00 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
06 00 00 00 43 40 00 00 00 00 00 00 07 03 00 00 00 00 00 00 00 03 00 00 00 00 00 00 00 03 00 00 00 00 00 00 00 03 00 00 00 00 00 00 00 07 00 00 00 00 00 00 00 03 00 00 00 00 00 00 00 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
04 00 00 00 53 00 00 00 00 00 00 00 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
05 00 00 00 53 40 00 00 00 00 00 00 03 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
05 00 00 00 43 40 00 00 00 00 00 00 06 03 00 00 00 00 00 00 00 03 00 00 00 00 00 00 00 03 00 00 00 00 00 00 00 03 00 00 00 00 00 00 00 07 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
*/
	//no idea what it does. Only adding this handler so maybe i can notice a client GUI change somewhere
	sLog.outDebug("Warning, sending a temp implemented packet with static content. Need to remake this later");
	WorldPacket data(MSG_GUILD_UNK, 77 + 10 );
//	data.CopyFromSnifferDump("06 00 00 00 43 40 00 00 00 00 00 00 07 03 00 00 00 00 00 00 00 03 00 00 00 00 00 00 00 03 00 00 00 00 00 00 00 03 00 00 00 00 00 00 00 07 00 00 00 00 00 00 00 03 00 00 00 00 00 00 00 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00");
	data.CopyFromSnifferDump("04 00 00 00 53 00 00 00 00 00 00 00 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00");
	SendPacket(&data);
}

void WorldSession::HandleGuildQueryGuildTabData(WorldPacket & recv_data)
{
	if( GetPlayer()->m_playerInfo->guild == NULL )
	{ 
		return;
	}
/*
13329
{CLIENT} Packet: (0x1280) UNKNOWN PacketSize = 12 TimeStamp = 3864160
00 00 00 00 E5 64 E4 01 00 00 00 03 
{SERVER} Packet: (0x3870) UNKNOWN PacketSize = 1044 TimeStamp = 3864253
25 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 06 00 00 00 06 00 00 00 05 00 00 00 07 00 00 00 05 00 00 00 07 00 00 00 07 00 00 00 07 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 04 00 00 00 06 00 00 00 06 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 07 00 00 00 07 00 00 00 07 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 60 E3 16 00 00 00 00 00 60 E3 16 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 40 4B 4C 00 00 00 00 00 40 4B 4C 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 40 4B 4C 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 40 4B 4C 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 60 E3 16 00 00 00 00 00 80 84 1E 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 60 E3 16 00 00 00 00 00 80 84 1E 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 60 E3 16 00 00 00 00 00 60 E3 16 00 00 00 00 00 60 E3 16 00 00 00 00 00 60 E3 16 00 00 00 00 00 00 E1 F5 05 00 00 00 00 00 C2 EB 0B 00 00 00 00 00 C2 EB 0B 00 00 00 00 00 1B B7 00 00 00 00 00 00 1B B7 00 00 00 00 00 00 1B B7 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 80 C3 C9 01 00 00 00 00 7D 13 00 00 7D 13 00 00 51 13 00 00 51 13 00 00 AB 13 00 00 AB 13 00 00 3B 14 00 00 51 14 00 00 A7 13 00 00 51 14 00 00 05 14 00 00 18 14 00 00 FC 12 00 00 17 14 00 00 2E 15 00 00 FC 12 00 00 17 14 00 00 2E 15 00 00 AC 13 00 00 5B 15 00 00 59 15 00 00 A0 13 00 00 4F 13 00 00 20 14 00 00 26 14 00 00 50 13 00 00 50 13 00 00 50 13 00 00 52 13 00 00 52 13 00 00 52 13 00 00 52 13 00 00 52 13 00 00 52 13 00 00 74 15 00 00 30 13 00 00 7C 13 00 00 B2 03 00 00 4D 04 20 00 B2 03 00 00 4D 04 20 00 B2 03 00 00 4D 04 20 00 B2 03 00 00 B2 03 00 00 4D 04 20 00 4D 04 20 00 FF FF FF FF 00 00 00 00 4D 04 20 00 4D 04 20 00 4D 04 20 00 B2 03 00 00 B2 03 00 00 B2 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4D 04 20 00 B2 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 B2 03 00 00 4D 04 20 00 00 00 00 00 46 D6 1A 4D 79 F7 00 00 78 F7 00 00 E7 F6 00 00 E6 F6 00 00 FA FE 00 00 50 FF 00 00 52 FF 00 00 54 FF 00 00 51 FF 00 00 53 FF 00 00 A2 F6 00 00 A6 F7 00 00 7F F7 00 00 8E FB 00 00 8F FB 00 00 90 FB 00 00 91 FB 00 00 92 FB 00 00 50 F5 00 00 4F F5 00 00 9B FF 00 00 DA FF 00 00 4E F3 00 00 28 0A 01 00 4F F3 00 00 56 F2 00 00 57 F2 00 00 58 F2 00 00 06 F2 00 00 F6 F1 00 00 F1 F1 00 00 F0 F1 00 00 EF F1 00 00 EB F1 00 00 23 06 01 00 5A F3 00 00 95 F6 00 00 
=
25 00 00 00 - item counter
05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 06 00 00 00 06 00 00 00 05 00 00 00 07 00 00 00 05 00 00 00 07 00 00 00 
07 00 00 00 07 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 
05 00 00 00 05 00 00 00 04 00 00 00 06 00 00 00 06 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 
05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 07 00 00 00 07 00 00 00 07 00 00 00 - maybe required guild level ?
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ?
60 E3 16 00 00 00 00 00 60 E3 16 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 40 4B 4C 00 00 00 00 00 
40 4B 4C 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 40 4B 4C 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 40 4B 4C 00 00 00 00 00 
C0 C6 2D 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 60 E3 16 00 00 00 00 00 80 84 1E 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 
60 E3 16 00 00 00 00 00 80 84 1E 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 60 E3 16 00 00 00 00 00 60 E3 16 00 00 00 00 00 
60 E3 16 00 00 00 00 00 60 E3 16 00 00 00 00 00 00 E1 F5 05 00 00 00 00 00 C2 EB 0B 00 00 00 00 00 C2 EB 0B 00 00 00 00 
00 1B B7 00 00 00 00 00 00 1B B7 00 00 00 00 00 00 1B B7 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 
C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00
C0 E1 E4 00 00 00 00 00 80 C3 C9 01 00 00 00 00 - gold cost
7D 13 00 00 7D 13 00 00 51 13 00 00 51 13 00 00 AB 13 00 00 AB 13 00 00 3B 14 00 00 51 14 00 00 A7 13 00 00 51 14 00 00 
05 14 00 00 18 14 00 00 FC 12 00 00 17 14 00 00 2E 15 00 00 FC 12 00 00 17 14 00 00 2E 15 00 00 AC 13 00 00 5B 15 00 00 
59 15 00 00 A0 13 00 00 4F 13 00 00 20 14 00 00 26 14 00 00 50 13 00 00 50 13 00 00 50 13 00 00 52 13 00 00 52 13 00 00 
52 13 00 00 52 13 00 00 52 13 00 00 52 13 00 00 74 15 00 00 30 13 00 00 7C 13 00 00 - required achievement criteria
B2 03 00 00 4D 04 20 00 B2 03 00 00 4D 04 20 00 B2 03 00 00 4D 04 20 00 B2 03 00 00 B2 03 00 00 4D 04 20 00 4D 04 20 00 
FF FF FF FF 00 00 00 00 4D 04 20 00 4D 04 20 00 4D 04 20 00 B2 03 00 00 B2 03 00 00 B2 03 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 4D 04 20 00 B2 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 B2 03 00 00 4D 04 20 00 00 00 00 00 - required rep level id ?
A7 3C 0C 4D - UNIX TIME
79 F7 00 00 78 F7 00 00 E7 F6 00 00 E6 F6 00 00 FA FE 00 00 50 FF 00 00 52 FF 00 00 54 FF 00 00 51 FF 00 00 53 FF 00 00 
A2 F6 00 00 A6 F7 00 00 7F F7 00 00 8E FB 00 00 8F FB 00 00 90 FB 00 00 91 FB 00 00 92 FB 00 00 50 F5 00 00 4F F5 00 00 
9B FF 00 00 DA FF 00 00 4E F3 00 00 28 0A 01 00 4F F3 00 00 56 F2 00 00 57 F2 00 00 58 F2 00 00 06 F2 00 00 F6 F1 00 00 
F1 F1 00 00 F0 F1 00 00 EF F1 00 00 EB F1 00 00 23 06 01 00 5A F3 00 00 95 F6 00 00 - item ID

local achievementID, itemID, itemName, iconTexture, repLevel, moneyCost = GetGuildRewardInfo(index);

14333
68 CC 26 4E 2A 00 00 00 D0 12 13 00 00 00 00 00 A0 25 26 00 00 00 00 00 60 E3 16 00 00 00 00 00 60 E3 16 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 40 4B 4C 00 00 00 00 00 40 4B 4C 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 40 4B 4C 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 40 4B 4C 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 60 E3 16 00 00 00 00 00 80 84 1E 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 60 E3 16 00 00 00 00 00 80 84 1E 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 60 E3 16 00 00 00 00 00 60 E3 16 00 00 00 00 00 60 E3 16 00 00 00 00 00 60 E3 16 00 00 00 00 00 00 E1 F5 05 00 00 00 00 00 C2 EB 0B 00 00 00 00 00 C2 EB 0B 00 00 00 00 00 1B B7 00 00 00 00 00 00 1B B7 00 00 00 00 00 00 1B B7 00 00 00 00 00 00 1B B7 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 80 C3 C9 01 00 00 00 00 C0 E1 E4 00 00 00 00 00 00 00 00 00 00 00 00 00 7D 13 00 00 7D 13 00 00 51 13 00 00 51 13 00 00 AB 13 00 00 AB 13 00 00 3B 14 00 00 51 14 00 00 A7 13 00 00 51 14 00 00 B4 16 00 00 18 14 00 00 FC 12 00 00 17 14 00 00 2E 15 00 00 FC 12 00 00 17 14 00 00 2E 15 00 00 AC 13 00 00 5B 15 00 00 59 15 00 00 A0 13 00 00 4F 13 00 00 20 14 00 00 26 14 00 00 50 13 00 00 50 13 00 00 50 13 00 00 50 13 00 00 52 13 00 00 52 13 00 00 52 13 00 00 52 13 00 00 52 13 00 00 52 13 00 00 52 13 00 00 74 15 00 00 30 13 00 00 7C 13 00 00 D0 16 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 04 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 06 00 00 00 06 00 00 00 05 00 00 00 06 00 00 00 05 00 00 00 06 00 00 00 07 00 00 00 06 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 04 00 00 00 06 00 00 00 06 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 07 00 00 00 07 00 00 00 07 00 00 00 07 00 00 00 59 0E 01 00 5A 0E 01 00 79 F7 00 00 78 F7 00 00 E7 F6 00 00 E6 F6 00 00 FA FE 00 00 50 FF 00 00 52 FF 00 00 54 FF 00 00 51 FF 00 00 53 FF 00 00 A2 F6 00 00 A6 F7 00 00 7F F7 00 00 8E FB 00 00 8F FB 00 00 90 FB 00 00 91 FB 00 00 92 FB 00 00 50 F5 00 00 4F F5 00 00 9B FF 00 00 DA FF 00 00 4E F3 00 00 28 0A 01 00 4F F3 00 00 04 11 01 00 56 F2 00 00 57 F2 00 00 58 F2 00 00 FF 10 01 00 06 F2 00 00 F6 F1 00 00 F1 F1 00 00 F0 F1 00 00 EF F1 00 00 EB F1 00 00 23 06 01 00 5A F3 00 00 95 F6 00 00 79 15 01 00 FF FF FF FF FF FF FF FF B2 03 00 00 4D 04 20 00 B2 03 00 00 4D 04 20 00 B2 03 00 00 4D 04 20 00 B2 03 00 00 B2 03 00 00 4D 04 20 00 4D 04 20 00 FF FF FF FF 00 00 00 00 4D 04 20 00 4D 04 20 00 4D 04 20 00 B2 03 00 00 B2 03 00 00 B2 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4D 04 20 00 B2 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 B2 03 00 00 4D 04 20 00 00 00 00 00 00 00 00 00 
*/
	sLog.outDebug("Warning, sending a temp implemented packet with static content. Need to remake this later");
	//see above for structure description. Need to implement this properly. Right now it is static
	WorldPacket data(SMSG_QUERY_GUILD_SHOP_TAB, 1044 + 10);
//	data.CopyFromSnifferDump("25 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 06 00 00 00 06 00 00 00 05 00 00 00 07 00 00 00 05 00 00 00 07 00 00 00 07 00 00 00 07 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 04 00 00 00 06 00 00 00 06 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 07 00 00 00 07 00 00 00 07 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 60 E3 16 00 00 00 00 00 60 E3 16 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 40 4B 4C 00 00 00 00 00 40 4B 4C 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 40 4B 4C 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 40 4B 4C 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 60 E3 16 00 00 00 00 00 80 84 1E 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 60 E3 16 00 00 00 00 00 80 84 1E 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 60 E3 16 00 00 00 00 00 60 E3 16 00 00 00 00 00 60 E3 16 00 00 00 00 00 60 E3 16 00 00 00 00 00 00 E1 F5 05 00 00 00 00 00 C2 EB 0B 00 00 00 00 00 C2 EB 0B 00 00 00 00 00 1B B7 00 00 00 00 00 00 1B B7 00 00 00 00 00 00 1B B7 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 80 C3 C9 01 00 00 00 00 7D 13 00 00 7D 13 00 00 51 13 00 00 51 13 00 00 AB 13 00 00 AB 13 00 00 3B 14 00 00 51 14 00 00 A7 13 00 00 51 14 00 00 05 14 00 00 18 14 00 00 FC 12 00 00 17 14 00 00 2E 15 00 00 FC 12 00 00 17 14 00 00 2E 15 00 00 AC 13 00 00 5B 15 00 00 59 15 00 00 A0 13 00 00 4F 13 00 00 20 14 00 00 26 14 00 00 50 13 00 00 50 13 00 00 50 13 00 00 52 13 00 00 52 13 00 00 52 13 00 00 52 13 00 00 52 13 00 00 52 13 00 00 74 15 00 00 30 13 00 00 7C 13 00 00 B2 03 00 00 4D 04 20 00 B2 03 00 00 4D 04 20 00 B2 03 00 00 4D 04 20 00 B2 03 00 00 B2 03 00 00 4D 04 20 00 4D 04 20 00 FF FF FF FF 00 00 00 00 4D 04 20 00 4D 04 20 00 4D 04 20 00 B2 03 00 00 B2 03 00 00 B2 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4D 04 20 00 B2 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 B2 03 00 00 4D 04 20 00 00 00 00 00 ");
//	data << uint32( UNIXTIME );
//	data.CopyFromSnifferDump("79 F7 00 00 78 F7 00 00 E7 F6 00 00 E6 F6 00 00 FA FE 00 00 50 FF 00 00 52 FF 00 00 54 FF 00 00 51 FF 00 00 53 FF 00 00 A2 F6 00 00 A6 F7 00 00 7F F7 00 00 8E FB 00 00 8F FB 00 00 90 FB 00 00 91 FB 00 00 92 FB 00 00 50 F5 00 00 4F F5 00 00 9B FF 00 00 DA FF 00 00 4E F3 00 00 28 0A 01 00 4F F3 00 00 56 F2 00 00 57 F2 00 00 58 F2 00 00 06 F2 00 00 F6 F1 00 00 F1 F1 00 00 F0 F1 00 00 EF F1 00 00 EB F1 00 00 23 06 01 00 5A F3 00 00 95 F6 00 00 ");
	data.CopyFromSnifferDump("68 CC 26 4E 2A 00 00 00 D0 12 13 00 00 00 00 00 A0 25 26 00 00 00 00 00 60 E3 16 00 00 00 00 00 60 E3 16 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 40 4B 4C 00 00 00 00 00 40 4B 4C 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 40 4B 4C 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 40 4B 4C 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 60 E3 16 00 00 00 00 00 80 84 1E 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 60 E3 16 00 00 00 00 00 80 84 1E 00 00 00 00 00 C0 C6 2D 00 00 00 00 00 60 E3 16 00 00 00 00 00 60 E3 16 00 00 00 00 00 60 E3 16 00 00 00 00 00 60 E3 16 00 00 00 00 00 00 E1 F5 05 00 00 00 00 00 C2 EB 0B 00 00 00 00 00 C2 EB 0B 00 00 00 00 00 1B B7 00 00 00 00 00 00 1B B7 00 00 00 00 00 00 1B B7 00 00 00 00 00 00 1B B7 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 C0 E1 E4 00 00 00 00 00 80 C3 C9 01 00 00 00 00 C0 E1 E4 00 00 00 00 00 00 00 00 00 00 00 00 00 7D 13 00 00 7D 13 00 00 51 13 00 00 51 13 00 00 AB 13 00 00 AB 13 00 00 3B 14 00 00 51 14 00 00 A7 13 00 00 51 14 00 00 B4 16 00 00 18 14 00 00 FC 12 00 00 17 14 00 00 2E 15 00 00 FC 12 00 00 17 14 00 00 2E 15 00 00 AC 13 00 00 5B 15 00 00 59 15 00 00 A0 13 00 00 4F 13 00 00 20 14 00 00 26 14 00 00 50 13 00 00 50 13 00 00 50 13 00 00 50 13 00 00 52 13 00 00 52 13 00 00 52 13 00 00 52 13 00 00 52 13 00 00 52 13 00 00 52 13 00 00 74 15 00 00 30 13 00 00 7C 13 00 00 D0 16 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 04 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 06 00 00 00 06 00 00 00 05 00 00 00 06 00 00 00 05 00 00 00 06 00 00 00 07 00 00 00 06 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 04 00 00 00 06 00 00 00 06 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 05 00 00 00 07 00 00 00 07 00 00 00 07 00 00 00 07 00 00 00 59 0E 01 00 5A 0E 01 00 79 F7 00 00 78 F7 00 00 E7 F6 00 00 E6 F6 00 00 FA FE 00 00 50 FF 00 00 52 FF 00 00 54 FF 00 00 51 FF 00 00 53 FF 00 00 A2 F6 00 00 A6 F7 00 00 7F F7 00 00 8E FB 00 00 8F FB 00 00 90 FB 00 00 91 FB 00 00 92 FB 00 00 50 F5 00 00 4F F5 00 00 9B FF 00 00 DA FF 00 00 4E F3 00 00 28 0A 01 00 4F F3 00 00 04 11 01 00 56 F2 00 00 57 F2 00 00 58 F2 00 00 FF 10 01 00 06 F2 00 00 F6 F1 00 00 F1 F1 00 00 F0 F1 00 00 EF F1 00 00 EB F1 00 00 23 06 01 00 5A F3 00 00 95 F6 00 00 79 15 01 00 FF FF FF FF FF FF FF FF B2 03 00 00 4D 04 20 00 B2 03 00 00 4D 04 20 00 B2 03 00 00 4D 04 20 00 B2 03 00 00 B2 03 00 00 4D 04 20 00 4D 04 20 00 FF FF FF FF 00 00 00 00 4D 04 20 00 4D 04 20 00 4D 04 20 00 B2 03 00 00 B2 03 00 00 B2 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4D 04 20 00 B2 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 B2 03 00 00 4D 04 20 00 00 00 00 00 00 00 00 00 ");
	SendPacket(&data);
}

void WorldSession::HandleGuildXPLimitToday(WorldPacket & recv_data)
{
	if( GetPlayer()->m_playerInfo->guild == NULL )
	{ 
		return;
	}
/*
13329
{CLIENT} Packet: (0x1B82) UNKNOWN PacketSize = 8 TimeStamp = 3864160
CA 7E A9 00 00 00 F3 1F 
{SERVER} Packet: (0x1838) UNKNOWN PacketSize = 8 TimeStamp = 3864363
70 4E 5F 00 00 00 00 00 - guild XP level limit / day
*/
	//seems to be fixed. Should add a config for it
	WorldPacket data(SMSG_QUERY_GUILD_XP_LIMIT_TODAY, 8 + 10);
	data << uint64( GUILD_DAILY_XP_LIMIT );
	SendPacket(&data);
}

void WorldSession::HandleGuildXPStatusTab(WorldPacket & recv_data)
{
	if( GetPlayer()->m_playerInfo->guild == NULL )
	{ 
		return;
	}
	GetPlayer()->m_playerInfo->guild->SendGuildXPStatus( this );
}

void WorldSession::HandleGuildGuildLogsTab(WorldPacket & recv_data)
{
	if( GetPlayer()->m_playerInfo->guild == NULL )
	{ 
		return;
	}

	GetPlayer()->m_playerInfo->guild->GuildNewsSendAll( this );
}

void WorldSession::HandleGuildQueryLogsPlayers(WorldPacket & recv_data)
{
	if( GetPlayer()->m_playerInfo->guild == NULL )
	{ 
		return;
	}
/*
13329
{SERVER} Packet: (0x147C) SMSG_GUILD_LOGS_QUERY_RESPONSE PacketSize = 1673 TimeStamp = 19002796
00 00 00 00 - counter 0 ( maybe guild market buy logs ? )
00 00 00 00 - counter 1 ( maybe guild owner public notes logs )
CD D5 25 4D - Thu, 06 Jan 2011 14:46:37 GMT
EE 63 01 0B - gametime 
00 00 00 00 - counter
F0 37 B2 43 - seems to be present with or without counter
1B 00 00 00 - counter 27 ( does not depend on guild size, order seems to change over time )
44 02 00 00 80 3A 09 00 00 00 00 00 
70 02 00 00 80 3A 09 00 00 00 00 00 
F5 02 00 00 80 3A 09 00 00 00 00 00 
24 02 00 00 80 3A 09 00 00 00 00 00 
15 02 00 00 80 3A 09 00 00 00 00 00 
68 02 00 00 80 3A 09 00 00 00 00 00 
38 02 00 00 80 F4 03 00 00 00 00 00 
20 02 00 00 80 3A 09 00 00 00 00 00 
35 01 00 00 80 F4 03 00 30 0B 01 00 
14 02 00 00 80 3A 09 00 00 00 00 00 
16 02 00 00 80 3A 09 00 00 00 00 00 
F9 00 00 00 80 3A 09 00 00 00 00 00 
13 02 00 00 80 3A 09 00 00 00 00 00 
9D 02 00 00 80 3A 09 00 00 00 00 00 
9F 02 00 00 80 3A 09 00 00 00 00 00 
99 01 00 00 80 3A 09 00 00 00 00 00 
D5 01 00 00 80 3A 09 00 00 00 00 00 
26 02 00 00 80 3A 09 00 00 00 00 00 
34 02 00 00 80 3A 09 00 00 00 00 00 
35 02 00 00 80 3A 09 00 00 00 00 00 
67 02 00 00 80 3A 09 00 00 00 00 00 
89 02 00 00 80 3A 09 00 00 00 00 00 
5B 02 00 00 80 3A 09 00 00 00 00 00 
FD 01 00 00 80 F4 03 00 30 0B 01 00 
D4 02 00 00 80 3A 09 00 00 00 00 00 
77 02 00 00 80 3A 09 00 00 00 00 00 
F2 02 00 00 80 3A 09 00 00 00 00 00 - 27 values
06 00 00 00 - counter = static content
B9 01 00 00 00 00 00 00 01 00 00 00 00 00 00 00 02 00 00 00 00 50 96 0A 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 A8 00 00 00 A8 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
BA 01 00 00 00 00 00 00 01 00 00 00 00 00 00 00 02 00 00 00 00 50 A0 0A 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 A8 00 00 00 A8 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
BB 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 A8 00 00 00 50 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
76 01 00 00 00 00 00 00 00 00 00 00 01 00 00 00 01 00 00 00 00 78 A7 07 00 B8 21 08 00 78 51 08 00 38 81 08 00 38 B1 08 00 78 21 09 00 38 51 09 00 F8 80 09 00 F8 B0 09 00 78 16 0A 00 F8 50 0A 00 B8 80 0A 00 B8 B0 0A 00 F8 20 0B 00 B8 50 0B 00 78 80 0B 00 78 B0 0B 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 30 00 00 00 A8 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 43 61 6C 65 6E 64 61 72 5F 44 61 72 6B 6D 6F 6F 6E 46 61 69 72 65 45 6C 77 79 6E 6E 00 Calendar_DarkmoonFaireElwynn
77 01 00 00 00 00 00 00 00 00 00 00 01 00 00 00 01 00 00 00 00 38 91 07 00 F8 00 08 00 F8 30 08 00 F8 60 08 00 B8 90 08 00 78 00 09 00 B8 30 09 00 B8 60 09 00 78 90 09 00 38 00 0A 00 78 30 0A 00 78 60 0A 00 38 90 0A 00 B8 B7 0A 00 38 30 0B 00 38 60 0B 00 78 87 0B 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 30 00 00 00 A8 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 43 61 6C 65 6E 64 61 72 5F 44 61 72 6B 6D 6F 6F 6E 46 61 69 72 65 4D 75 6C 67 6F 72 65 00 Calendar_DarkmoonFaireMulgore 
78 01 00 00 00 00 00 00 00 00 00 00 01 00 00 00 01 00 00 00 01 78 A0 07 00 38 10 08 00 78 40 08 00 38 70 08 00 B8 A1 08 00 78 07 09 00 38 40 09 00 B8 67 09 00 78 97 09 00 38 07 0A 00 78 37 0A 00 78 67 0A 00 38 97 0A 00 F8 10 0B 00 38 37 0B 00 38 71 0B 00 F8 A0 0B 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 30 00 00 00 A8 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 43 61 6C 65 6E 64 61 72 5F 44 61 72 6B 6D 6F 6F 6E 46 61 69 72 65 54 65 72 6F 6B 6B 61 72 00 Calendar_DarkmoonFaireTerokkar 
*/
	//no idea what it does. Only adding this handler so maybe i can notice a client GUI change somewhere
	sLog.outDebug("Warning, sending a temp implemented packet with static content. Need to remake this later");
	WorldPacket data(SMSG_GUILD_PLAYERS_LOGS_QUERY_RESPONSE, 200);
	data << uint32( 0 ); // counter for player related logs, ex achievement, loot, buy
	data << uint32( 0 ); // history of public messages
	data << uint32( UNIXTIME );
	data << uint32( TimeToGametime( UNIXTIME ) );
	data << uint32( 0 );	// history of public messages
	//static list, changes only order, first value is the counter
	//on second tought these seem to be some calendar related stuff. They do seem to change rarely
	data.CopyFromSnifferDump("00 32 30 43 1A 00 00 00 35 02 00 00 80 3A 09 00 00 00 00 00 70 02 00 00 80 3A 09 00 00 00 00 00 F5 02 00 00 80 3A 09 00 00 00 00 00 24 02 00 00 80 3A 09 00 00 00 00 00 44 02 00 00 80 3A 09 00 00 00 00 00 67 02 00 00 80 3A 09 00 00 00 00 00 15 02 00 00 80 3A 09 00 00 00 00 00 89 02 00 00 80 3A 09 00 00 00 00 00 99 01 00 00 80 3A 09 00 00 00 00 00 D5 01 00 00 80 3A 09 00 00 00 00 00 20 02 00 00 80 3A 09 00 00 00 00 00 14 02 00 00 80 3A 09 00 00 00 00 00 16 02 00 00 80 3A 09 00 00 00 00 00 D0 02 00 00 80 3A 09 00 00 00 00 00 34 02 00 00 80 3A 09 00 00 00 00 00 26 02 00 00 80 3A 09 00 00 00 00 00 5B 02 00 00 80 3A 09 00 00 00 00 00 F9 00 00 00 80 3A 09 00 00 00 00 00 68 02 00 00 80 3A 09 00 00 00 00 00 9D 02 00 00 80 3A 09 00 00 00 00 00 9F 02 00 00 80 3A 09 00 00 00 00 00 D4 02 00 00 80 3A 09 00 00 00 00 00 F2 02 00 00 80 3A 09 00 00 00 00 00 77 02 00 00 80 3A 09 00 00 00 00 00 FD 01 00 00 80 F4 03 00 30 0B 01 00 13 02 00 00 80 3A 09 00 00 00 00 00 00 00 00 00 ");
	SendPacket(&data);
}

void WorldSession::HandleGuildQueryUnk2(WorldPacket & recv_data)
{
	if( GetPlayer()->m_playerInfo->guild == NULL )
	{ 
		return;
	}
/*
13329
{CLIENT} Packet: (0x9942) UNKNOWN PacketSize = 8 TimeStamp = 9352125
CA 7E A9 00 00 00 F3 1F 
{SERVER} Packet: (0xB29E) UNKNOWN PacketSize = 3363 TimeStamp = 9352187
14 0D 00 00 78 01 E3 66 18 05 A4 84 40 27 23 29 AA 47 D5 2E 19 0D 02 92 42 E0 14 49 AA 47 15 EF 1A 0D 02 92 42 E0 28 49 AA 47 15 AF 1E 0D 02 92 42 60 1B 49 AA 47 15 FB 8E B6 27 48 4A 04 AC CC 24 29 1F F1 8A 3F 33 8D F8 20 20 29 00 96 02 55 03 00 A2 5F 06 D4 00 00 1E 00 00 00 D8 5E 05 AC AA 2A 00 00 00 00 00 00 00 00 00 00 34 CD CB 40 00 00 00 00 38 DB B8 AA 00 00 00 00 A0 CE CB 40 00 00 00 00 38 DB B8 AA AA 2A 00 00 E0 DF CB 40 00 00 00 00 38 DB B8 AA AA 2A 00 00 09 60 24 4D 00 00 00 00 5D 86 00 00 00 00 00 00 A0 CE CB 40 00 00 00 00 F7 85 69 00 00 00 00 00 E0 DF CB 40 00 00 00 00 38 DB B8 AA AA 2A 00 00 D0 CE CB 40 00 00 00 00 0A 70 94 00 00 00 00 00 00 00 00 00 00 00 00 00 E0 DF CB 40 5D 86 00 00 70 DB B8 AA AA 2A 00 00 E0 DF CB 40 00 00 00 00 A0 DF CB 40 00 00 00 00 B4 6F 69 00 00 00 00 00 12 7F 2E 1C 80 BC 50 E2 33 D5 44 01 E7 C1 E5 60 00 00 00 00 7E 26 17 56 10 6E 67 0F 00 00 00 00 85 5B B3 61 20 96 8A 37 02 5B 1E 09 25 BE 00 33 4F CB E4 02 76 D6 84 17 E8 51 05 9D B5 B1 D7 22 2B C4 2F 37 05 4B F3 B2 63 05 79 ED 5F 45 35 8B 76 A9 AD A9 06 51 91 17 42 55 F0 2F A0 7A B3 F1 34 D2 39 9F DF 86 88 48 64 24 F4 A0 96 E4 EC 4F 76 B8 0A D4 5B C9 53 5E BD 83 BD C8 AC 22 57 11 26 79 ED 69 92 2F 2B 6A 43 D8 B9 51 A6 BE 4E F0 44 DF 14 64 46 ED 6C 80 E0 82 8B B2 03 72 82 8A FC C1 61 F0 FE D5 C1 4E D4 5B C5 D6 B6 97 A4 1B C6 4E F6 A8 59 D1 E1 5B 07 CB 45 9F AD 6D 61 52 6D B9 83 C1 D5 89 3A 68 27 E8 EE 72 75 03 6D F8 FA 93 01 08 12 85 CF E1 EA 06 1E 1C D5 A1 96 14 7A DB 13 E7 F1 4C 25 DB 18 89 B1 3B 4B 41 90 CF 27 E3 02 12 F7 F0 55 66 9D 4C D4 D1 7C D6 AE 4E F6 B0 D1 2A A6 9D 59 2A 7D 93 79 41 A0 2D 4A F8 66 BE 8D 15 44 3B F2 40 8B A8 32 0B 80 BC 10 A2 93 89 74 18 DD 55 C0 15 E1 85 A6 45 A4 B7 5F C7 95 44 F3 65 CF A1 72 00 45 83 6A AC C7 9D D8 08 40 E2 8E AA 36 AB DD 45 73 95 14 C3 95 36 F6 D8 60 15 23 BB 42 A4 EA F2 DB 6A 52 89 14 D3 A3 C2 F9 08 EB 6A 80 30 3C 67 20 67 22 29 08 68 A7 D9 C6 02 47 2C F2 D9 5F 9C B6 B1 D8 47 8A BE FC 82 8D 55 05 29 3E B5 3A 68 8D 29 89 50 8B 61 23 18 14 B6 76 57 1D 01 97 88 99 D7 86 4D 6F BB BA 83 2E 64 B6 DC 4B F2 B5 B0 3B 0E 80 C4 3D 64 DD BB C6 EB 08 A7 A8 DF ED F3 77 30 7F 09 79 2C 46 49 D0 75 44 A0 2D 4F 52 47 53 22 EA DE F5 5F 52 3B 38 A3 4F 21 01 B6 3F 95 04 11 01 1D EC 3C DE 15 0F 12 56 49 81 1B 8C 82 0C 48 EE 0C 77 51 9F BE 33 98 31 EC B3 B1 C0 6C 9F 4E 24 01 1E 0F B7 D6 DB D9 C8 16 AB B8 69 41 8C 74 70 9B 9D 5D 7F 5B D1 A1 CF CE B6 66 59 C5 DF 3E 8F 91 48 AF DC 43 FA 1A 49 E0 3A 58 F5 7E D4 9F 8F 96 5A 36 76 B0 FA E3 34 BE 52 32 0F 74 B0 B0 0E AB 58 56 E8 AE 83 76 62 3C C4 BA 5A 0B 46 66 10 6C 22 4D 9F 8B 34 1C B7 1C 6D 2D 69 F4 36 C6 48 AE B5 A3 CF A2 42 5D 02 21 8D 77 10 C0 72 C9 E7 03 A2 01 48 DC 89 13 32 31 CC A9 3D 76 B6 14 61 96 37 AC 91 CA BF 9B 2B D0 16 22 4E CF AA 3B C2 4C 61 0E C7 2E 7B 13 8B AA A0 64 62 98 71 18 7E 29 B5 33 FF 74 0A 1B 2F BD 53 67 67 2B DF 55 74 6D 68 DD 74 AD 31 65 D8 42 9E 4A D8 E8 EA C8 E7 73 B0 3D 91 B8 87 CD 61 2C 55 5F 60 E2 A3 5A D9 47 6D 16 71 5E 6F B8 49 AB 1F 57 5F C1 0E 69 5F C1 1C F8 02 43 1B 43 F1 B8 D2 69 FF 67 A5 19 C6 7B 20 2E A6 82 5E 32 74 8D DD BA 6D 11 7B F0 05 E6 83 DB 72 EF C3 99 99 7B E0 78 D8 C6 FE 67 A9 57 7D 9A E1 E9 B6 28 F9 BA C2 10 12 3B 29 7C 7F 02 03 ED A8 21 89 77 96 04 5D B8 1E 46 9A 0B 29 FD 4A F4 F0 47 98 1E 0E 36 28 14 E3 A8 D7 CB CE 73 36 56 74 CD 20 74 FE 9E 5C 0D 5F 91 CF B2 C1 06 A5 62 84 38 5C DF B3 2C A0 54 FB BB 23 5D 2C 75 C1 CE FA 6D 29 C2 B0 6E 6A 11 DD 9A B9 0B 6C 94 DA CF 3F BC EE 9F 65 0F 00 00 00 00 00 00 00 00 CB 34 A1 EE DA 07 00 00 00 00 00 00 A8 D8 CB 40 00 00 00 00 55 02 00 00 DC AF D7 E5 90 05 00 00 00 00 00 00 00 00 00 00 00 00 00 00 89 66 BC 00 00 00 00 00 B0 76 DD 00 00 00 00 00 12 D3 CB 40 00 00 00 00 00 00 00 00 FF FF FF FF EB 07 00 00 EB 07 00 00 10 D3 CB 40 00 00 00 00 3B 83 69 00 7D 6C 00 00 E0 DF CB 40 00 00 00 00 30 DB CB 40 00 00 00 00 00 D3 CB 40 00 00 00 00 FD 07 00 00 00 00 00 00 90 DB CB 40 00 00 00 00 F8 72 94 00 00 00 00 00 50 D3 CB 40 00 00 00 00 3B 83 69 00 7D 6C 00 00 F7 3A BA 0F 00 00 00 00 00 00 00 00 00 00 00 00 23 07 00 00 00 00 00 00 F7 D6 CB 40 00 00 00 00 FF 03 00 00 02 8B 3C 03 2F 03 00 00 00 00 00 00 16 13 6C 0F 00 00 00 00 00 00 00 00 00 00 00 00 F2 06 00 00 00 00 00 00 D3 D6 CB 40 00 00 00 00 22 04 00 00 FF FF FF FF DB 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 89 66 BC 00 00 00 00 00 B0 76 DD 00 00 00 00 00 F2 D3 CB 40 00 00 00 00 00 00 00 00 FF FF FF FF 03 07 00 00 03 07 00 00 F0 D3 CB 40 00 00 00 00 3B 83 69 00 7D 6C 00 00 E0 DF CB 40 00 00 00 00 30 DB CB 40 00 00 00 00 20 DC CB 40 00 00 00 00 96 E1 6A 00 00 00 00 00 90 DB CB 40 00 00 00 00 F8 72 94 00 00 00 00 00 14 3E B3 E3 DA AE 54 2B 6A 46 20 B9 56 94 26 86 30 DC CB 40 00 00 00 00 A2 20 43 B4 AC 04 83 9E 7D 6C 00 00 F2 06 00 00 78 01 B5 94 7F 48 13 61 18 C7 9F EB F6 C3 95 B5 E5 2D CD 8A 66 45 25 D1 C4 B1 FF 04 DD 55 2F 2F 48 45 14 EA 7F C9 A0 12 22 23 14 0A 6A 04 B5 2C 2B 8C F9 4F C5 64 1C A2 60 45 20 52 12 F4 03 B1 19 A2 64 E1 1F AD 12 C3 7E AC 46 30 90 0E CB 11 41 F6 3C 77 5B 5C 49 31 14 5F 78 EE BE EF 7B F7 7C DE 7B EE F9 DE 09 B0 14 00 44 11 0F F1 CB CF F2 1C 6A AA 6B DA 94 B0 96 AA CD 83 5F 72 54 61 AA D3 55 65 71 A8 66 59 84 23 95 4E 7E D2 BA 81 B9 15 85 19 F5 FD 36 8F 8C D9 70 F8 A1 9D 4E 20 B8 9A 6C D0 E9 02 A0 C4 75 B8 50 65 01 68 0B 87 2B 72 05 BA 7C C6 07 30 33 23 88 30 23 C4 63 43 B8 21 B1 59 AB 93 5B F9 7A F6 2A A5 30 A3 BE F9 5C 94 29 E9 DB 3D 7A 4A 62 B7 D8 20 36 84 0A 13 89 0D 79 74 68 F7 35 23 DB 44 12 F9 B6 DF 7C 77 34 CD 3F 3B 28 F1 E6 81 12 B6 3B 5F 61 46 9D 70 C9 32 25 99 A7 33 CF 8E 7C 77 14 57 B2 E2 77 0F A7 F9 AF 4F 49 FC 76 61 39 0B F6 44 98 51 6F E5 6A 05 F1 3D 8F E8 05 A7 9F BF 7B 18 55 56 FC E4 48 9A 9F FC 20 F1 02 4F 39 3B FF 26 C2 8C 7A DF 58 51 1F 71 3B FA D6 D2 49 7F 3F C9 11 54 D9 F0 53 77 BF 6A AD 16 FD 98 40 E3 81 AB A0 E6 47 97 C4 4F DF A8 60 2D D7 22 EC F8 A4 5D A6 F5 86 EA 18 76 AD 1F 75 AB 7C 74 70 09 7F 0F 00 00 00 00 00 00 00 00 C2 82 1D 0F BC 04 00 00 00 00 00 00 AE D8 CB 40 00 00 00 00 51 02 00 00 AC 78 20 0B 76 02 00 00 00 00 00 00 B1 10 6C 0F 00 00 00 00 00 00 00 00 00 00 00 00 8D 04 00 00 00 00 00 00 CA D8 CB 40 00 00 00 00 36 02 00 00 00 00 00 00 62 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 89 66 BC 00 00 00 00 00 B0 76 DD 00 00 00 00 00 62 D6 CB 40 00 00 00 00 00 00 00 00 FF FF FF FF 9E 04 00 00 9E 04 00 00 60 D6 CB 40 00 00 00 00 3B 83 69 00 7D 6C 00 00 E0 DF CB 40 00 00 00 00 30 DB CB 40 00 00 00 00 50 D6 CB 40 00 00 00 00 B0 04 00 00 00 00 00 00 90 DB CB 40 00 00 00 00 F8 72 94 00 00 00 00 00 07 10 6C 0F 00 00 00 00 00 00 00 00 00 00 00 00 E3 03 00 00 00 00 00 00 D3 D8 CB 40 00 00 00 00 23 02 00 00 8D 04 00 00 CB 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 89 66 BC 00 00 00 00 00 B0 76 DD 00 00 00 00 00 02 D7 CB 40 00 00 00 00 00 00 00 00 FF FF FF FF F4 03 00 00 F4 03 00 00 00 D7 CB 40 00 00 00 00 3B 83 69 00 7D 6C 00 00 E0 DF CB 40 00 00 00 00 30 DB CB 40 00 00 00 00 F0 D6 CB 40 00 00 00 00 06 04 00 00 00 00 00 00 90 DB CB 40 00 00 00 00 F8 72 94 00 00 00 00 00 66 3B D7 99 6C D2 ED DC D7 34 74 31 FB B9 36 5F D7 65 3B 83 0F AF 24 61 CE 9E 5B 99 87 B9 17 FB 7D 6C 00 00 E3 03 00 00 78 01 63 64 60 65 60 60 60 66 03 12 ED 45 92 8C 1F BF 6F 9B C8 52 CC 6C F0 F1 BB E0 34 08 7D 78 21 4B 0F AB C1 C7 FE 15 13 E3 99 99 BF 8B 08 B2 1E 64 37 F8 C8 F8 7D F3 64 16 45 2B 83 8F CC 09 40 8D 20 F0 47 55 3C AC E6 9F BA DB 85 25 C9 2E 1F 52 96 BB 2C 53 DD E2 00 96 60 50 00 D2 07 80 78 82 43 AE CC 74 20 0D E2 3F 00 F3 6F F3 7B 3A 80 30 9B 3C 03 C3 33 07 3E 06 06 86 E4 6A 06 16 20 C5 20 C0 C0 00 54 C1 00 B4 84 01 64 0B 27 90 AD 68 05 24 18 1A EC 19 18 99 18 39 80 AC 14 28 76 70 E2 67 40 E7 33 02 E5 F6 01 31 43 03 10 03 25 2F B0 43 F0 BB 1A 7B BB 59 33 25 ED 5E 00 0D 04 61 90 79 20 B5 60 FA BB E9 1A 4C 2F DD B4 D0 70 E3 9A 98 EE F2 DC 72 B9 CB 9B 7B C7 1D C0 8A 29 F1 12 D0 12 9A 78 29 55 8D 81 01 84 51 BD E4 CD CE 7A 08 18 5D CC 09 20 09 20 00 C5 12 73 95 86 5B C6 B2 0C 97 5E D7 E5 2E 5C 21 99 0E 60 09 22 BD D4 04 8E 25 E7 6A 06 50 7A 81 C5 12 D0 12 06 90 2D A0 00 3D 04 0C 69 90 13 40 B1 04 8A 19 77 20 0F 39 A6 98 81 7C 31 90 12 A4 58 69 2C ED B4 4B D1 66 B2 3F CA C4 C0 00 C2 E0 14 80 12 33 52 CF 98 A0 89 0D 18 5B 40 9D 20 6F D8 FD 54 77 D3 59 97 E5 F2 D6 60 B9 4E 06 7B 0F 00 00 00 00 00 00 00 00 2F B1 01 15 BA 01 00 00 00 00 00 00 BC D9 CB 40 00 00 00 00 41 01 00 00 CE 0E 93 DF 84 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 89 66 BC 00 00 00 00 00 B0 76 DD 00 00 00 00 00 32 D9 CB 40 00 00 00 00 00 00 00 00 FF FF FF FF CB 01 00 00 CB 01 00 00 30 D9 CB 40 00 00 00 00 3B 83 69 00 7D 6C 00 00 E0 DF CB 40 00 00 00 00 30 DB CB 40 00 00 00 00 D9 3D 76 0F 00 00 00 00 00 00 00 00 00 00 00 00 35 01 00 00 00 00 00 00 AE DA CB 40 00 00 00 00 4A 00 00 00 00 00 00 00 F6 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 89 66 BC 00 00 00 00 00 B0 76 DD 00 00 00 00 00 B2 D9 CB 40 00 00 00 00 00 00 00 00 FF FF FF FF 46 01 00 00 46 01 00 00 B0 D9 CB 40 00 00 00 00 3B 83 69 00 7D 6C 00 00 E0 DF CB 40 00 00 00 00 30 DB CB 40 00 00 00 00 A0 D9 CB 40 00 00 00 00 58 01 00 00 00 00 00 00 90 DB CB 40 00 00 00 00 F8 72 94 00 00 00 00 00 7F 00 00 00 00 00 00 00 46 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 89 66 BC 00 00 00 00 00 7D 6C 00 00 35 01 00 00 78 01 EB 63 62 62 60 60 60 E6 06 12 1F E5 58 18 85 3F 7E 34 60 05 91 46 4C 20 D2 98 19 48 7E FE C9 92 DA 67 F0 F1 F3 6F 08 C5 6B 7E BE CD E0 E3 45 8D 77 40 99 27 8C E1 FD 40 99 52 66 31 10 55 C1 2C 01 A4 18 BF 2F AC 60 DC DD 1B F0 91 F9 01 D0 50 06 06 6B 86 4A 35 F1 30 95 5D 8C 47 F9 72 94 5C 05 FE AC 76 50 3B 7B D0 61 12 57 9B 1D 58 9A E1 81 03 03 43 82 23 03 C3 04 87 5C 99 E9 40 B6 02 10 83 C4 26 38 DC E6 F7 04 63 16 A0 13 41 7A D8 E4 19 18 9E 39 F0 31 30 30 24 57 33 88 00 29 06 01 06 06 A0 4A 06 A0 85 0C 20 1B 39 81 EC DD BD 40 82 A1 C1 9E 81 91 89 31 8A 95 81 21 05 C8 03 61 07 27 7E 06 74 3E 3B 50 FC 38 1B 90 60 64 60 62 E0 60 60 B8 00 14 00 61 06 86 05 0E 40 89 66 BC 00 00 00 00 00 B0 76 DD 00 00 00 00 00 A2 3C 76 0F 00 00 00 00 00 00 00 00 FF FF FF FF 2D 00 00 00 2D 00 00 00 A0 3C 76 0F 00 00 00 00 E0 DF CB 40 00 00 00 00 14 0D 00 00 00 00 00 00 45 26 17 56 00 00 00 00 20 DD CB 40 00 00 00 00 00 00 00 00 00 00 00 00 90 DB CB 40 00 00 00 
=
14 0D 00 00 78 01 E3 66 18 05 A4 84 40 27 23 29 AA 47 D5 2E 19 0D 02 92 42 E0 14 49 AA 47 15 EF 1A 0D 02 92 42 E0 28 49 AA 47 15 AF 1E 0D 02 92 42 60 1B 49 AA 47 15 FB 8E B6 27 48 4A 04 AC CC 24 29 1F F1 8A 3F 33 8D F8 20 20 29 00 96 02 55 03
= uncompresed
0B 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 89 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 A4 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 CA 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 BA 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 C5 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 AB 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 B6 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4D 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 F3 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 A5 00 00 00 
{SERVER} Packet: (0xB29E) UNKNOWN PacketSize = 3363 TimeStamp = 644830
14 0D 00 00 78 01 ED 95 CF 4B 1B 51 10 C7 57 63 62 A1 6A 6E F6 62 D9 83 88 42 11 04 51 6F 51 F0 5C DA 7B 2F 05 8F 52 02 5E F4 A0 79 82 87 E2 C9 7F 40 EB 45 4F 1E B4 20 16 B4 71 0B A5 08 35 28 5A B4 42 C8 3E B0 D0 08 A1 AC 3F AA 2F FA 92 F7 ED CC 26 0A DE 72 F0 50 D6 7C 61 67 67 F7 0D CB CE 67 BF 6F F6 A9 65 59 87 87 14 AA AA 88 C0 74 8D 65 19 94 E4 D9 43 05 E9 00 39 18 F9 2E 5B F7 7A B6 93 57 7E 35 73 5C B6 AD 05 B4 6D C1 6E D4 E8 3F 05 3C BA 57 56 A4 D0 CC 59 71 6A C8 0D 3E F8 05 A2 BA 09 23 8D 2E 52 CB 32 26 34 12 80 03 08 6F E2 5C 48 C2 F2 BC 96 AE 76 A1 44 F6 99 48 B8 89 8A BE 42 70 8B BE 53 6B 46 02 50 E1 E0 36 F9 80 9D AD 33 2F C2 45 C0 20 80 0C 59 49 23 C3 77 B4 7B 03 A5 79 C5 4E 1C 7D 3A 70 9A 0A F8 CA 57 2C B2 DD 7B F1 17 2A EB 2E C1 8B 3C E0 EB FC F7 8F FA C6 BC 0C 64 79 22 5D 7B 94 16 47 79 3A F5 E7 99 CC 1C A7 3E D0 B7 34 E9 AA 5A 62 5E 08 55 41 54 48 60 8D EA AE D8 48 77 12 77 99 2A 65 EC 30 DE AE 24 51 88 3F 72 97 BD A4 FE 79 E3 65 A4 2A 6D 3B E8 E4 38 A3 B9 95 3F C1 3C 9E 6D A4 0F 98 CF D1 C9 A1 43 43 25 21 A3 06 F1 0A BF 4C 30 CA C2 B4 15 8D 0F 00 D7 BB 52 AF 24 D5 76 7C 35 0D C7 33 98 54 2D ED E4 AD E8 09 46 37 73 AD 26 3D A5 EC FC DA 9F CE 9B DF 3F DE BC DA 83 D8 38 23 CB 89 53 F4 AE FF EC 1B 6E C2 7E 1C 97 4E DA FD 38 DC 33 3D 10 1E 89 0C 3E 19 6B 88 05 03 D1 BD 2E 2E 6A 6F FF 8F C0 3C 2E C8 38 45 B4 52 24 49 3F 86 FC C8 2E EB C6 96 A0 A2 D8 97 F2 0A D5 97 55 9F C7 CC 67 7B 27 95 4A 45 3B BA 5E 84 8E 03 3C 0E 17 89 DE 3F DF C1 AF 27 14 3F F1 A5 5A 09 F3 4F 8A AE 74 8B 0A 2B 85 76 73 8D 8A EC C3 D0 BE F7 C2 69 63 5C 34 D5 AA 38 19 CB 9A 68 A6 89 2E B1 16 EB 5C 4B CB 46 D9 84 6A E8 64 7E 2B 85 0E F6 A1 F4 4B 6D 41 37 5B 70 20 DB 70 05 19 93 AD EA 98 CE 39 19 1B D3 2C 6C A1 1B 1B 3B E7 BE 20 E6 29 CC 28 F6 C1 BD E7 9D 7B DF 39 E7 FE EE F9 9D 93 BC 98 F2 04 00 48 79 34 C5 2C 9D 2E 97 D7 1A 19 38 39 3D 24 19 A2 63 A6 99 47 E6 48 B4 C3 78 A1 D9 1A 89 B6 5A 8E B6 90 68 B3 58 59 04 34 ED 0D 4D 0B 6A 5A C8 F2 3C EF 75 6A 5A 53 AE 67 96 CC 5F D5 44 4B 61 21 EF F9 34 D1 5A F8 FD EB A4 F9 35 ED 1F BB 70 FD AF 26 5E 2E D0 EC 34 E1 16 22 66 B7 49 FD 64 11 AB B0 49 03 2C 4B 3C 52 98 A5 E3 9E 93 4F 1C 7B E0 17 27 FF 43 7E 76 A2 97 D4 C5 3F 5D A3 7C E8 A2 B4 C1 65 73 C4 10 ED B3 94 53 74 C9 0D 60 24 9C F0 73 6A 7B 7A 5A E0 25 2C 7D 21 84 99 4F BE A2 8E 9D 18 96 79 1D A0 40 06 08 2A 00 61 B9 FD E9 21 7A 67 7D 9E 46 58 9E CD AC 10 23 35 1F E0 27 39 03 00 6C FE 2C E1 2D 0B 80 BE 80 3E 0B 00 47 49 A7 F7 F2 16 9A E0 E2 31 30 A4 18 72 9F 01 F0 92 C6 43 56 33 41 AF 1F A7 75 76 07 7B 01 A6 D2 B4 51 33 72 AD FC 93 26 00 1E 06 DE 23 5F 6B 32 E6 34 9B 37 C0 99 F8 B8 12 CF 3A 42 78 60 C9 AE DA 52 8E C9 C2 68 27 70 28 C8 A6 70 2A 0F 26 C2 D1 EB 27 28 72 72 70 FE CE 30 C5 E1 18 8C 7C 09 9C 9D 1F 32 EB F1 F7 BC 00 4E AF 36 AA D2 C8 DB 5B 86 03 D6 BD 60 22 1F 9E A7 00 66 2E 17 99 44 12 38 11 F1 87 93 53 50 D7 80 85 8F FC F8 7E 51 93 9A A3 34 60 A5 33 20 DE 7B DE AD 47 D3 60 00 8D 55 5E 35 7B A9 0E 6B 6E FB 71 4F 55 AD DA F8 41 3D FE 36 DD 26 BE 39 F5 5A 03 7E ED F2 63 D9 5C AD BA 15 3F 5A D2 E2 C1 D7 53 47 23 62 9C 3A 74 03 9B DE B5 9E 2A 7A FD FF A8 93 48 1B FF A4 BC 81 36 C6 0B B5 78 37 D7 8F D5 D7 9F 53 CF 07 46 E3 F7 B4 83 2A A0 20 BB 02 65 8D FD E2 2E A9 1A A2 67 7C E1 4E AD A6 C5 12 B3 E6 CD B7 5C F8 ED 78 10 7B 26 67 94 BC BF 1C B2 76 EF 8C 26 4B D9 62 4D F7 26 24 E6 8C 8F CC 28 0A D3 26 DC C9 EE B4 9A 3E 42 6F EB 6B 5A AF F3 69 A9 4F 24 D4 F4 37 37 82 E5 6C 6F A7 4D 1E 89 C9 09 2D 66 37 53 23 93 DC BC 41 0F C3 F9 D2 57 89 F7 5B BB F1 BD 89 AF 94 03 BF 7C 2A F3 3A B7 A4 9A BA 3D 5B 84 E3 49 84 43 41 80 A3 30 9C 66 3E BF 80 03 06 7D 0D EB 75 AE 69 B3 F8 9C A6 75 6D 4A D8 93 8F FD D4 A6 78 24 40 8A 9A AB 3C 7A 44 57 57 1C D8 33 7F 0E 6F EE FB 4C 59 B9 95 23 B3 C9 8E 10 99 AB A8 D6 E3 88 3C BB 8D 28 E6 93 ED FC 6B 23 B9 F9 D4 F4 70 92 0E 39 AA F1 C3 D1 2E 2C 8E 7C A7 5C 5F FD 48 16 1B 94 A4 2F 42 DB E4 1C 05 01 8E C2 49 72 25 09 69 8C A3 AF 4B 50 49 C9 30 71 AE 40 CE A7 99 47 42 82 62 57 D4 8D 70 EE 5C 72 E2 E1 D4 6E CC 29 9D 55 E0 F3 76 61 C3 1E B6 0D 87 82 6C 0A 47 DF CB F4 3A F7 B6 E4 E0 FC BA 5F FC 07 90 DC 64 C9 0F 67 E7 E1 82 03 FB 7F EC C2 D1 BC 79 65 A1 23 43 16 1B 3B 81 43 41 1E 0F 9C A8 B3 DB AB E7 DA DC 35 17 9E 2E 3E 8B 65 E7 1E 2A 8D 53 D6 35 34 DB 6E 08 CE 6E EA 63 71 AE 79 93 E4 5A D2 0D 21 D6 65 DB C8 B7 8B E9 4E B4 96 F6 62 C5 3B CB 8A B3 7F 7C ED 07 68 DB 7C A3 20 8F 25 41 FF 01 3E 6E 60 8C 17 82 01 32 48 47 72 A8 A4 34 E7 86 C2 F7 63 71 59 7B FB 9A 37 9E 0B 4C A2 DD 80 DD F9 AC 89 79 BC 01 06 D6 07 92 A5 9F 20 1C 16 42 00 00 00 00 96 E1 6A 00 00 00 00 00 39 5A 1E 3E 2B F5 8F 36 FF 78 94 A4 BC 19 29 43 69 3B E7 3A 62 2D 5F 46 4A 6D EB 0A A5 67 E5 B1 30 1C 16 42 00 00 00 00 7A 2E 23 A5 36 75 BB 0C DE 31 79 FA B6 8C 94 E1 DC 57 1C D9 D2 AC AF 78 43 D3 70 20 E9 EE 90 57 53 75 62 FF D9 3E 91 B8 6E 8A 6B 3F 4D 8F 3C 32 AF 2A F5 2B D5 0D 54 C8 DF 89 7B A0 6C F6 71 31 1F A5 D3 8F 0B 74 02 E4 C5 8B DD B2 69 98 36 7A 43 FE E9 1F 10 4F D6 6C 10 8F C9 8A A2 C7 46 5B 95 B3 8B 64 F3 2F A0 86 07 01 E3 73 E9 88 E9 41 42 80 43 FE 6B 8E 3E 04 40 40 F6 EF 69 A6 9E 03 9A 26 11 3E 3A AC 71 DE 70 35 91 27 D4 CD BA 0C 11 4A C6 1E A8 23 C8 D7 99 33 97 19 DF 51 65 E4 29 BC F1 65 31 2B 00 00 00 00 00 00 00 00 00 00 04 00 00 00 00 00 00 00 60 2C CB AC AA 2A 00 00 04 00 00 00 00 00 00 00 70 15 16 42 00 00 00 00 19 7C B0 00 00 00 00 00 AE 6D AF 83 E3 47 D9 91 25 58 64 89 D7 E1 B4 FD 08 90 0F FF BC 09 10 68 60 2C CB AC AA 2A 00 00 88 C4 19 07 00 00 00 00 48 C4 19 07 00 00 00 00 37 6D 30 65 8E 73 2C E6 5B 5E B1 BC 3C 2E F2 BF CA 7B 82 E5 CD 2E 4D 0E DF BC CA DB 7F 77 7C D2 6D 78 12 BA 00 64 65 EF B6 BC B3 4A 55 AE B7 55 E4 C3 78 24 B5 27 6D 6E 73 97 8A 4B DC 74 EF AA 77 02 3B CE 16 49 DC F7 93 D0 77 60 31 D3 B8 E6 44 FE D7 F6 F0 91 13 BE 00 00 00 00 00 00 00 00 01 00 00 00 00 00 00 00 E0 15 16 42 00 00 00 00 00 00 00 00 00 00 00 00 22 00 00 00 00 00 00 00 40 17 16 42 00 00 00 00 EC 80 B0 00 00 00 00 00 AB 02 00 00 D2 49 2F 70 29 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 F0 16 16 42 00 00 00 00 40 17 16 42 00 00 00 00 90 19 16 42 00 00 00 00 08 0D 36 07 00 00 00 00 00 00 00 00 00 00 00 00 22 00 00 00 00 00 00 00 C2 80 B0 00 00 00 00 00 E0 1F 16 42 00 00 00 00 30 1B 16 42 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 B2 18 16 42 00 00 00 00 20 00 00 00 00 00 00 00 1E 00 00 00 00 00 00 00 00 22 02 0C 00 01 03 0C 00 00 00 00 00 00 00 00 3B 83 69 00 7D 6C 00 00 7D 6C 00 00 C9 04 00 00 78 01 CD 53 41 68 13 41 28 84 F6 AA AA 2A 00 00 00 00 00 00 04 00 00 00 40 C3 19 07 00 00 00 00 88 C4 19 07 00 00 00 00 90 02 00 00 02 00 00 00 6F 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 89 66 BC 00 00 00 00 00 90 76 DD 00 00 00 00 00 F2 16 16 42 00 00 00 00 60 2C CB AC AA 2A 00 00 05 04 00 00 05 04 00 00 F0 16 16 42 00 00 00 00 04 00 00 00 1E 00 00 00 00 00 00 00 00 00 00 00 18 00 00 00 00 00 00 00 4C 1A 16 42 00 00 00 00 18 00 00 00 00 00 00 00 00 18 16 42 00 00 00 00 EC B8 B0 00 00 00 00 00 00 00 00 00 FF FF FF FF B2 03 00 00 B2 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 B4 18 16 42 00 00 00 00 60 2C CB AC AA 2A 00 00 04 00 00 00 00 00 00 00 4D 92 79 68 31 2B 00 00 00 00 00 00 00 00 00 00 04 00 00 00 00 00 00 00 80 C2 19 07 00 00 00 00 C0 C2 19 07 00 00 00 00 08 0D 36 07 00 00 00 00 90 19 16 42 00 00 00 00 90 18 16 42 00 00 00 00 CD D6 86 00 00 00 00 00 4E 1A 16 42 00 00 00 00 57 B3 32 30 E8 5F 5F CF CD C0 D0 60 0F C1 40 41 20 00 D9 5E 7B 8D EB 18 B7 B9 1B D8 6C 64 7B 02 0E 73 1D CB DA E8 E8 B2 ED B0 B5 33 32 9B 11 99 C3 26 CF C0 F0 CC 81 0F 68 4C FD 6F 01 11 90 91 02 0C 0C 0E 40 2A DF 89 81 01 E4 0D A0 07 80 34 90 00 D9 CC C8 C8 C8 7D 8C 91 21 05 C8 03 61 07 27 7E 06 74 7E 28 50 1C D0 D7 01 AC AA 2A 00 00 4E 1A 16 42 00 00 00 00 30 30 5C 00 8A 81 F0 97 D0 D7 01 AC AA 2A 00 00 10 D8 01 AC AA 2A 00 00 C0 18 16 42 00 00 00 00 44 E9 2C AC AA 2A 00 00 00 00 00 00 00 00 00 00 38 52 A0 07 00 00 00 00 00 00 00 00 00 00 00 00 24 02 00 00 00 00 00 00 63 19 16 42 00 00 00 00 94 01 00 00 00 00 00 00 9B 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 89 66 BC 00 00 00 00 00 90 76 DD 00 00 00 00 00 C2 18 16 42 00 00 00 00 00 00 00 00 FF FF FF FF 35 02 00 00 35 02 00 00 C0 18 16 42 00 00 00 00 3B 83 69 00 7D 6C 00 00 EC 81 F6 AA AA 2A 00 00 00 00 00 00 00 00 00 00 B8 01 00 00 00 00 00 00 74 1A 16 42 00 00 00 00 87 00 00 00 00 00 00 00 3C 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 89 66 BC 00 00 00 00 00 90 76 DD 00 00 00 00 00 32 19 16 42 00 00 00 00 68 35 32 07 00 00 00 00 00 00 00 00 C9 01 00 00 64 01 00 00 00 00 00 00 E9 19 16 42 00 00 00 00 0E 01 00 00 00 00 00 00 61 00 00 00 00 00 00 00 3D 35 32 07 00 00 00 00 00 00 00 00 00 00 00 00 39 01 00 00 00 00 00 00 09 1A 16 42 00 00 00 00 F3 00 00 00 00 00 00 00 51 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 89 66 BC 00 00 00 00 00 C8 C5 50 07 00 00 00 00 00 00 00 00 00 00 00 00 F4 00 00 00 00 00 00 00 2B 1A 16 42 00 00 00 00 CC 00 00 00 00 00 00 00 33 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 89 66 BC 00 00 00 00 00 90 76 DD 00 00 00 00 00 F2 19 16 42 00 00 00 00 00 00 00 00 FF FF FF FF 05 01 00 00 05 01 00 00 F0 19 16 42 00 00 00 00 3B 83 69 00 7D 6C 00 00 E0 1F 16 42 00 00 00 00 30 1B 16 42 00 00 00 00 E0 19 16 42 00 00 00 00 17 01 00 00 00 00 00 00 90 1B 16 42 00 00 00 00 F8 72 94 00 00 00 00 00 21 B3 01 A8 B3 BF 34 B8 85 99 51 0F C8 42 33 2B 01 28 34 78 C0 0F 46 EC 6E 39 73 93 81 C1 BF 83 7D 6C 00 00 F4 00 00 00 78 01 63 64 60 62 00 82 FE 89 71 47 99 19 F5 80 2C 1E 10 77 A8 81 6F 40 07 B3 30 02 89 EF 07 05 0C 16 F0 18 7C 65 03 09 60 F3 85 09 50 10 00 FF 12 07 87 20 1E A5 10 98 86 65 1B 88 19 BC 4F A5 2A C0 72 1C E8 A8 03 69 62 98 78 76 B9 75 11 68 E3 BF CB 6C 96 7C 43 2F 0C 80 5B 72 0B F5 03 B9 BF 04 BC 90 77 D8 3D BC 04 1C A5 50 0A 81 32 BD 6B 7A 6B CC B3 7C EF 0B E7 D2 56 28 00 00 00 00 C0 1A 16 42 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 90 1B 16 42 00 00 00 00 89 66 BC 00 00 00 00 00 90 76 DD 00 00 00 00 00 12 D0 2B 07 00 00 00 00 00 00 00 00 FF FF FF FF 2E 00 00 00 2E 00 00 00 10 D0 2B 07 00 00 00 00 E0 1F 16 42 00 00 00 00 14 0D 00 00 00 00 00 00 B8 09 6D 09 00 00 00 00 20 1D 16 42 00 00 00 00 00 00 00 00 00 00 00 00 90 1B 16 42 00 00 00 
{SERVER} Packet: (0xB29E) UNKNOWN PacketSize = 3363 TimeStamp = 36018478
14 0D 00 00 78 01 ED 96 4F 68 D3 70 14 C7 5F 12 EB 02 FE 69 4F 6B 2F 5D EB 49 10 A1 3B 08 03 FF 35 22 8A B0 A9 BB 88 97 09 1D F3 32 04 FF C0 58 61 93 FC B2 3A C4 9D 2C 88 88 C8 8C E0 A6 63 1E 84 09 0E AA AE 41 98 C3 8A 38 DC 41 41 BA 80 20 6C 0E DB 42 DD 62 B3 24 CF F7 2B 4C AF 3D B7 7D 90 97 97 5F BE 04 DE 27 EF F7 7E 6F 07 00 14 C2 E4 9A 56 13 81 31 01 C0 43 CC 62 12 2F 4F 45 5A 9C 60 E6 C7 AB 4B B3 AA D9 FB 18 BA 35 F9 BA C2 3F 52 42 F6 C8 B7 1B B4 C1 C5 74 44 30 A2 10 28 21 DE 46 DD C1 AA C5 48 C1 03 CB 5A AA 2C 48 5C 5F CF 36 41 C9 BD 46 2F EF D9 1F 4F 58 E3 5D 30 70 08 0F 9E 6A DF 0B A0 B4 28 9F 23 50 E8 49 ED 84 0C E2 87 B3 BD FD C1 00 53 97 87 EA 19 46 0D B9 E5 48 E3 99 2E E2 BA AF 06 75 53 92 21 04 7C 63 31 DC A0 0B C7 11 3F 6D B2 7C 01 B3 7A A5 E7 A8 10 4C F0 9D 16 68 FD 2A C2 94 78 40 79 CB 9F D0 B5 B0 88 FE ED C2 04 03 4D 94 BB 41 6C 24 8A F3 94 AC 67 A3 59 25 41 1C 8A B3 5F F4 73 5D D2 37 97 85 43 D4 D7 F0 46 91 1C 0F 30 41 9D AE 69 CF 08 C1 CA 6A 13 45 AD 95 F0 92 84 BF 79 F9 94 F1 08 5E 61 54 49 EC 5E D9 57 71 72 EF DA 52 1B 6B BA 3A 84 EE 2F CF 4F EF 93 36 22 69 E5 06 47 7B 9A F2 4F D3 38 91 BF 60 79 04 6B C1 C1 E9 37 17 3D B4 30 66 B3 10 EA 7F B0 9F C3 2C F2 DE 46 F6 10 F5 7D AB 9D 7B B2 23 00 09 E9 6A 3B FF 29 76 92 FB 86 31 1F 4D 4C 1E 7B EE EA 7D 77 2A 67 F2 3F 41 4B 8F D1 40 65 C4 A3 5E 47 95 81 E1 17 20 3A 30 97 93 D4 A5 D4 D3 E0 CA CC FE 5D D3 37 17 05 59 09 74 68 34 97 8D 30 23 0E 70 4C 88 8A A3 0A 6C AE CF 08 AD 93 FF EB 4F 3E 5C 87 14 CB 74 BA 39 B4 19 E7 86 97 F1 C1 8B EF 49 3A FE 50 EA 3C 99 BC 1B EE 9B 5F 83 E3 B7 04 71 98 2F 99 D7 9C 78 4A 34 62 25 BC DF 66 E0 F9 F7 D5 6A D3 D1 84 27 C2 A8 2A 6E 53 42 9A 26 2B 5B 78 FE 05 5B 0B 75 74 9F A4 5C FE 02 52 7B 2C C0 00 00 00 FF FF FF FF 7F 0A 00 00 7F 0A 00 00 80 20 D5 40 00 00 00 00 3B 83 69 00 7D 6C 00 00 E0 2F D5 40 00 00 00 00 30 2B D5 40 00 00 00 00 70 20 D5 40 00 00 00 00 91 0A 00 00 00 00 00 00 90 2B D5 40 00 00 00 00 F8 72 94 00 00 00 00 00 09 3E 3C 18 30 19 B4 8B 9C 04 30 DD F3 F7 16 41 15 B8 51 80 14 F2 72 51 93 F0 78 B8 EF 06 19 9D 86 FB CD 1B 00 00 00 00 00 00 00 00 7D 4C 95 55 A2 09 00 00 00 00 00 00 3C 25 D5 40 00 00 00 00 B9 05 00 00 CB FB DE DE F4 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 89 66 BC 00 00 00 00 00 90 76 DD 00 00 00 00 00 42 21 D5 40 00 00 00 00 00 00 00 00 FF FF FF FF B3 09 00 00 B3 09 00 00 40 21 D5 40 00 00 00 00 3B 83 69 00 7D 6C 00 00 E0 2F D5 40 00 00 00 00 30 2B D5 40 00 00 00 00 30 21 D5 40 00 00 00 00 C5 09 00 00 00 00 00 00 90 2B D5 40 00 00 00 00 F8 72 94 00 00 00 00 00 1D D8 AE 2D 9B B8 9B 72 86 D6 DC 74 C2 6A 9F 4C 9E A7 09 92 C7 03 F9 4F F6 29 E0 6C 3F 5C 62 93 7D 6C 00 00 A2 09 00 00 78 01 CD 55 5D 68 14 57 14 3E 77 D6 5D 67 4D CD 36 49 6B B0 0D BB 1B B0 1B 7F 92 BA 91 E6 21 08 DD C9 74 6C 55 6A AC A0 50 44 34 62 A0 E0 1A AD 14 5B AC 85 CE 43 4A 68 D1 56 5A 30 36 6D C0 96 5A 6C 89 29 24 E2 0F 12 9B 92 EC 84 5D D1 F5 07 51 10 7F 1E B4 DA 87 3E 2C 2B 6C 09 95 F4 3B 77 66 ED 38 CD 52 9B FA E0 85 73 EF B9 77 EE F9 EE 3D E7 3B E7 0E 51 25 11 F9 9E 42 97 DF 11 E9 0E C6 73 F9 19 8D 47 B6 61 78 ED 40 DF 8F 18 5E 3F B0 1F 8B 13 F1 3D E2 F2 C1 2F B4 5C 76 78 E8 AC CF 14 D9 EE 40 86 87 B3 E7 B2 18 0A 33 C3 E2 BC 12 CF 8D 15 7E BF 6A 8A A8 F1 10 AC AA 2A 00 00 00 00 00 00 C1 78 8E C6 24 08 00 00 00 00 00 00 09 26 D5 40 00 00 00 00 EE 04 00 00 BC FD 8A 30 41 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 89 66 BC 00 00 00 00 00 90 76 DD 00 00 00 00 00 C2 22 D5 40 00 00 00 00 00 00 00 00 FF FF FF FF 35 08 00 00 35 08 00 00 C0 22 D5 40 00 00 00 00 3B 83 69 00 7D 6C 00 00 E0 2F D5 40 00 00 00 00 30 2B D5 40 00 00 00 00 B0 22 D5 40 00 00 00 00 47 08 00 00 00 00 00 00 90 2B D5 40 00 00 00 00 F8 72 94 00 00 00 00 00 EC A0 18 84 4F 09 CA 11 1D 99 09 12 8A 78 06 5A BB 23 9A 1E 22 EF 9C C3 20 03 E0 23 85 54 A2 73 7D 6C 00 00 24 08 00 00 78 01 BD 55 6F 48 53 51 14 3F CF A7 CB 3F 91 C3 88 FE 50 CC BE F4 C7 4A 07 33 42 4C F7 7C 2B 35 8A 22 D4 3E 94 73 33 13 2A 27 49 13 95 FC E0 12 B5 84 0A A1 42 8D 24 C5 A0 21 99 08 42 99 52 43 6F 66 10 91 24 54 54 A2 21 41 81 1F D6 B4 7D CA 75 CE 7B 3A DF 9E 56 A3 C0 07 E7 DD 7B CE 3D E7 9E FB BB BF 73 EF 05 88 02 00 3E 0C 7F DE 24 43 B6 2F 42 3F E3 AD DD 52 4D AD 47 F3 FD C8 A0 DE ED B9 B1 37 B1 1F 1B CF B7 F3 03 7A 37 E7 CD 99 BD 68 76 E9 67 78 2B 86 D0 D7 56 B7 3B B3 E9 C9 21 F6 BE AD 97 CD 96 16 89 7D C9 E9 46 79 24 56 C0 36 0D A0 41 28 DE D8 88 7D D2 C7 51 1A 84 0F D1 07 24 D1 E8 00 BE 08 AB D0 AD A0 12 42 B1 01 2D 00 7A 00 26 01 CA 12 81 7D B3 0B 7F E0 30 02 17 C2 65 F0 00 A7 50 23 11 C4 68 50 EB EB D1 4E D3 81 03 25 1C E0 F5 0A 59 A4 78 70 19 5F 26 00 90 90 CE 51 43 AD F7 DD B1 1C 3B AF 82 34 74 BA 84 75 58 3B 59 4B 6F 91 78 AD FA 8C 20 39 4B 10 5C D8 0F 06 92 29 10 12 26 01 CA 42 90 EC 08 43 5A 12 42 A2 65 28 21 A9 75 A2 06 B7 69 09 38 AD C6 13 E8 4C 12 08 A7 A9 3D 09 19 0B 64 28 39 25 8B 6D 3B E8 64 1D F1 17 C4 98 61 A7 82 21 6D 5A 70 70 54 0C 61 12 A0 2C 04 27 B1 5F 5E 02 31 D4 17 12 08 47 AD 13 43 48 0A AE 18 65 09 86 D6 E0 84 24 81 90 A6 1E 97 E7 AB 8B EE EB 98 85 4D 9E EB 60 55 26 9B B8 6B C7 33 81 62 E4 22 0B B6 E8 54 0C 61 12 A0 2C 94 3D DF 45 93 C9 45 57 A9 82 A4 D6 D7 A2 E7 4E 72 57 C0 D1 D5 0E E2 1E 97 08 8F CC 00 24 81 70 DC 7D 8B E1 A4 97 59 D8 44 45 0F FB F8 C9 22 46 8D 8C 29 18 FA 47 38 98 64 99 E0 68 6D 91 D9 1A ED 4C 98 C0 83 7D F8 24 FB 91 D9 C3 DA 2A 2C A2 B2 5F A3 7F 28 21 DA 53 15 49 9B 01 9C AE 26 02 30 0E 28 70 33 1A B2 35 00 B7 9A 9B 53 57 CE 95 33 80 CF C7 F1 E0 E3 3C CE D1 91 10 BD 9B B7 A2 13 7D 74 DF 14 BE 3A CC 3C AE FB 6C 88 AB 11 4D B5 96 14 79 24 56 00 F8 C7 C3 E9 1C 05 A0 2C 44 FD 08 D2 3D 4F 3D 2D E6 4F 87 93 C6 75 E4 AE A0 FE 6D 7B 49 2A C5 C7 C4 00 90 04 52 FF 22 AE 6C 51 25 3F 9D DD C7 1A 7F 76 B1 89 E3 75 62 B9 7D 40 A0 98 FF AA 64 4C B2 4C D4 1F DD 14 35 4F FD C4 86 FD 6C FB 95 6E F6 26 AF 4E 54 F6 97 A4 1E E3 FE 4E BD B7 2B B5 B8 40 3E F6 5C 28 6D 31 71 1F 76 D9 C6 EE 3E 68 61 57 13 EA 45 43 AE 27 E8 73 02 FA 70 E8 8E 03 F8 9C 07 70 33 B7 1C 89 C1 D3 2D 89 BC DD F4 0E 1A B7 5A 59 E7 78 B3 34 F7 E4 B4 99 39 2E DD 96 FA 64 CF 8A BF 23 DB 4B 0B 99 23 A3 55 EA D7 0F 9C 5D B0 4F DB FC FE 64 F7 CF 43 FE CF 9B FC F3 F8 ED BF 99 5F AA 7F 79 49 00 0B 0F A4 A9 52 1B 4A D6 B9 07 12 77 06 68 6B A8 60 0B 5C 34 20 DF 55 53 AA BB 4A AD AF 43 CF 68 72 57 14 AC C1 70 DD 68 30 AC 16 EE E1 AB 44 82 1F FE 17 1E C8 5F 67 96 E6 39 27 D5 40 00 00 00 00 39 03 00 00 B9 6E 87 FE 10 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 89 66 BC 00 00 00 00 00 90 76 DD 00 00 00 00 00 B2 26 D5 40 00 00 00 00 00 00 00 00 FF FF FF FF 4F 04 00 00 4F 04 00 00 B0 26 D5 40 00 00 00 00 3B 83 69 00 7D 6C 00 00 E0 2F D5 40 00 00 00 00 30 2B D5 40 00 00 00 00 A0 26 D5 40 00 00 00 00 61 04 00 00 00 00 00 00 90 2B D5 40 00 00 00 00 F8 72 94 00 00 00 00 00 F0 01 00 00 BC 3B 54 83 29 8C 2B 70 30 2B 00 00 00 00 00 00 00 00 00 00 18 00 00 00 00 00 00 00 4C 2A D5 40 00 00 00 00 18 00 00 00 00 00 00 00 00 28 D5 40 00 00 00 00 EC B8 B0 00 00 00 00 00 00 00 00 00 00 00 00 00 89 66 BC 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4C 2A D5 40 00 00 00 00 18 00 00 00 00 00 00 00 16 00 00 00 00 00 00 00 00 16 01 21 00 01 01 0F 00 00 00 00 00 00 00 00 CE B8 B0 00 00 00 00 00 50 27 D5 40 00 00 00 00 A6 03 00 00 00 00 00 00 16 00 00 00 00 00 00 00 F8 72 94 00 36 00 00 00 38 89 05 AC AA 2A 00 00 1F F7 71 AC 41 E6 7F 00 4E 2A D5 40 00 00 00 00 B3 0E 45 60 32 7F 1E AA 7D 6C 00 00 83 03 00 00 78 01 63 64 60 63 60 60 F0 72 F2 AA AA 2A 00 00 00 00 00 00 7E 67 62 74 BC 02 00 00 00 00 00 00 C0 29 D5 40 00 00 00 00 3F 01 00 00 08 52 CA 02 88 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 89 66 BC 00 00 00 00 00 90 76 DD 00 00 00 00 00 32 28 D5 40 00 00 00 00 30 87 05 AC AA 2A 00 00 4E 2A D5 40 00 00 00 00 30 28 D5 40 00 00 00 00 30 87 05 AC AA 2A 00 00 70 87 05 AC AA 2A 00 00 C0 28 D5 40 00 00 00 00 36 00 00 00 00 00 00 00 D0 5E 05 AC AA 2A 00 00 30 28 D5 40 00 00 00 00 38 F6 86 00 00 00 00 00 F0 29 D5 40 00 00 00 00 C0 28 D5 40 00 00 00 00 50 29 D5 40 00 00 00 00 30 87 05 AC AA 2A 00 00 10 29 D5 40 00 00 00 00 6A B5 AC 00 00 00 00 00 B0 28 D5 40 00 00 00 00 D8 5E 05 AC 01 00 00 00 01 00 00 00 21 00 01 01 F8 5F 05 AC AA 2A 00 00 B0 28 D5 40 00 00 00 00 C2 8A 86 00 00 00 00 00 4F D2 20 4D 00 00 00 00 D0 5E 05 AC AA 2A 00 00 D8 5E 05 AC AA 2A 00 00 D8 5E 05 AC AA 2A 00 00 10 29 D5 40 00 00 00 00 E8 5E 05 AC AA 2A 00 00 D8 C2 B6 AA AA 2A 00 00 30 87 05 AC AA 2A 00 00 10 29 D5 40 00 00 00 00 51 CD 85 00 00 00 00 00 90 76 DD 00 00 00 00 00 4E 2A D5 40 00 00 00 00 00 00 00 00 FF FF FF FF 16 00 00 00 16 00 00 00 80 29 D5 40 00 00 00 00 D8 5E 05 AC AA 2A 00 00 D0 5E 05 AC AA 2A 00 00 80 29 D5 40 00 00 00 00 01 00 00 00 00 00 00 00 D0 5E 05 AC AA 2A 00 00 46 41 DC AA AA 2A 00 00 00 00 00 00 00 00 00 00 12 01 00 00 00 00 00 00 3F 2A D5 40 00 00 00 00 25 41 DC AA AA 2A 00 00 00 00 00 00 00 00 00 00 F1 00 00 00 00 00 00 00 2C 2A D5 40 00 00 00 00 C8 00 00 00 00 00 00 00 34 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 89 66 BC 00 00 00 00 00 90 76 DD 00 00 00 00 00 F2 29 D5 40 00 00 00 00 00 00 00 00 FF FF FF FF 02 01 00 00 02 01 00 00 F0 29 D5 40 00 00 00 00 3B 83 69 00 7D 6C 00 00 E0 2F D5 40 00 00 00 00 30 2B D5 40 00 00 00 00 AC 63 17 AC AA 2A 00 00 00 00 00 00 00 00 00 00 78 00 00 00 00 00 00 00 AD 2A D5 40 00 00 00 00 4E 00 00 00 12 01 00 00 35 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 89 66 BC 00 00 00 00 00 90 76 DD 00 00 00 00 00 72 2A D5 40 00 00 00 00 00 00 00 00 FF FF FF FF 89 00 00 00 89 00 00 00 70 2A D5 40 00 00 00 00 3B 83 69 00 7D 6C 00 00 E0 2F D5 40 00 00 00 00 30 2B D5 40 00 00 00 00 60 2A D5 40 00 00 00 00 9B 00 00 00 00 00 00 00 90 2B D5 40 00 00 00 00 F8 72 94 00 00 00 00 00 6D AD C1 AB 2A 19 AC 6A 4B 2F 7E 55 FE 32 20 B3 74 7B 60 AA 0C 20 4E 52 80 50 18 64 07 07 03 00 7D 6C 00 00 78 00 00 00 78 01 B3 66 62 66 00 82 EF 46 0A 35 46 E9 06 1F D9 80 6C 16 90 00 3A 28 51 01 8A 7C 9F 6B 5A 22 90 8D 47 D5 6B 45 90 2A 15 13 FC AA 9A D4 18 18 00 87 82 0D 62 2A 00 00 00 00 00 00 FF FF FF FF 55 00 00 00 55 00 00 00 30 80 72 AB AA 2A 00 00 E0 2F D5 40 00 00 00 00 14 0D 00 00 00 00 00 00 F4 86 35 48 00 00 00 00 20 2D D5 40 00 00 00 00 00 00 00 00 00 00 00 00 90 2B D5 40 00 00 00 

14333
{SERVER} Packet: (0xAE44) UNKNOWN PacketSize = 413 TimeStamp = 7937128
14 0D 00 00 78 01 E3 66 18 05 A4 84 40 27 23 29 AA 47 D5 2E 01 06 C1 86 FD DF 6E 5E 2B 6A 75 FD 3E D3 07 C8 FB 29 C4 C1 C0 CC C0 D0 C0 CA 70 00 1C 96 2C 0C 4C E0 60 E2 18 0D 2C 60 08 9C 02 E2 4F B7 BF FD 6F F8 CE 3A 1A 1E 44 84 C0 2E A0 9A 82 FF DF C3 C5 8F 3E F3 E3 F9 F0 BF F7 C5 DF 13 BF F2 03 9F BD AA 9B BD 3B 46 80 89 8B C1 86 E1 03 3B 50 89 03 83 02 43 03 23 D3 81 FF FF FF 7F DD F8 EC FF E3 FF FC 6C 0C 4F FE 5F 7F 1A B7 EA F7 FB 16 22 AC 19 36 4A 8E 02 7D D2 D5 00 CC 7F A3 80 A8 10 D8 06 54 F5 E9 EF 4F B3 D8 C7 FB 6D 6E FB C6 D9 FD 4F AD 9C A6 C8 F2 E0 02 53 B3 BC C7 87 6C B1 8A AA FA B7 6B B3 EC 5F FF FF 5D F6 0F 98 B6 EA C3 74 46 78 FD E0 0B F4 7F 03 30 24 EE 46 5E 92 03 86 48 FA CF FF 1D 7B 55 BE FE 3F FC 9F FB 47 BD C5 FF FE 47 FF 19 FF 00 65 19 19 78 13 18 18 1E FC 9F F7 5F 9E 81 25 9B C9 BE 6B 61 0A 87 D0 95 BD FF EF F3 FF 7B A7 2F 4B 54 C4 0C 13 45 AC C0 AC F8 A3 7E 7F ED D4 4C E1 4A 1F DF 57 40 5F 35 F8 33 30 1C B0 67 F8 67 0E F6 E1 01 7E 20 95 B2 EF 14 73 CD E5 A6 E5 E2 2F 26 29 EA CE 68 13 08 E4 70 14 30 17 60 78 F8 BF B9 FE C0 7E 26 03 71 C3 2C BE 9F 4C 49 D5 DF 77 69 87 AF DC 6D BB 73 A7 FC 22 16 07 4E 0E 1E BB 03 17 41 BA 87 15 F8 0C 69 2C 0C 2B 3F D1 D2 33 4B 69 69 F8 30 34 7B 35 D0 4F 00 17 6B 99 85 
*/
	//Only seen it a few times. Not spammed all the time it seems
	sLog.outDebug("Warning, sending a temp implemented packet with static content. Need to remake this later");
	WorldPacket data(SMSG_QUERY_GUILD_UNK_COMPRESSED, 3363 + 10);
//	data.CopyFromSnifferDump("14 0D 00 00 78 01 E3 66 18 05 A4 84 40 27 23 29 AA 47 D5 2E 19 0D 02 92 42 E0 14 49 AA 47 15 EF 1A 0D 02 92 42 E0 28 49 AA 47 15 AF 1E 0D 02 92 42 60 1B 49 AA 47 15 FB 8E B6 27 48 4A 04 AC CC 24 29 1F F1 8A 3F 33 8D F8 20 20 29 00 96 02 55 03");
	data.CopyFromSnifferDump("14 0D 00 00 78 01 ED 96 4F 68 D3 70 14 C7 5F 12 EB 02 FE 69 4F 6B 2F 5D EB 49 10 A1 3B 08 03 FF 35 22 8A B0 A9 BB 88 97 09 1D F3 32 04 FF C0 58 61 93 FC B2 3A C4 9D 2C 88 88 C8 8C E0 A6 63 1E 84 09 0E AA AE 41 98 C3 8A 38 DC 41 41 BA 80 20 6C 0E DB 42 DD 62 B3 24 CF F7 2B 4C AF 3D B7 7D 90 97 97 5F BE 04 DE 27 EF F7 7E 6F 07 00 14 C2 E4 9A 56 13 81 31 01 ");
	SendPacket(&data);
}

void WorldSession::HandleGuildNewsToggleSticky(WorldPacket & recv_data)
{
	//player already has a guild ? how could he look for a new one ?
	if( GetPlayer()->m_playerInfo->guild == NULL )
	{ 
		return;
	}
/*
-create sticky
{CLIENT} Packet: (0x8401) CMSG_QUERY_GUILD_NEWS_MAKE_STICKY PacketSize = 7 stamp = 17148175
09 00 03 01 00 00 00 - event guid 259
{CLIENT} Packet: (0x8401) CMSG_QUERY_GUILD_NEWS_MAKE_STICKY PacketSize = 7 stamp = 17150359
09 00 01 01 00 00 00 - event guid 257
-remove sticky
{CLIENT} Packet: (0x8401) CMSG_QUERY_GUILD_NEWS_MAKE_STICKY PacketSize = 7 stamp = 16935499
01 00 E2 90 78 00 00                 
{CLIENT} Packet: (0x8401) CMSG_QUERY_GUILD_NEWS_MAKE_STICKY PacketSize = 7 stamp = 17223773
01 00 02 01 00 00 00 - event guid 258

{CLIENT} Packet: (0x8401) CMSG_QUERY_GUILD_NEWS_MAKE_STICKY PacketSize = 10 stamp = 17811460
E9 00 - ( mask1 & 8 ) = remove sticky
23 00 00 00 - event guid
18 12 16 14 - player guids obfuscated

09 00 - 1 is unsticky, 9 is sticky
03 01 00 00 - event GUID
00 player guid depending on mask
*/
	uint8	mask1,mask2;
	uint32	EventId;
	recv_data >> mask1 >> mask2;
	recv_data >> EventId;

	GetPlayer()->m_playerInfo->guild->GuildNewsToggleSticky( EventId, this );
}

void WorldSession::HandleGuildFinderQueryState(WorldPacket & recv_data)
{
	//player already has a guild ? how could he look for a new one ?
	if( GetPlayer()->m_playerInfo->guild != NULL )
	{ 
		return;
	}

	GetPlayer()->GuildFinderSendPendingRequests();
}

void WorldSession::HandleGuildFinderCharSetFilter(WorldPacket & recv_data)
{
	//player already has a guild ? how could he look for a new one ?
	if( GetPlayer()->m_playerInfo->guild != NULL )
	{ 
		return;
	}
	uint32 FilterInterest;
	uint32 FilterActivity;
	uint32 FilterRole;
	uint32 FilterLevel;

	recv_data >> FilterRole;
	recv_data >> FilterActivity;
	recv_data >> FilterLevel;		
	recv_data >> FilterInterest;
/*
14333
{SERVER} Packet: (0xCF21) SMSG_GUILD_FINDER_RESULT_LIST PacketSize = 33136 TimeStamp = 1429858
AF 00 00 00 - 175 counter
some mask 1 byte / guild, probably obfuscated guid bytes mask
77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 77 7F 7F 7F 77 7F 7F 7F 7F 77 77 7F 7F 77 7F 7F 7F 7F 77 7F 7F 7F 7F 7F 75 7F 7F 7F 7F 7F 7F 7F 7F 7F 77 7F 7F 7F 7F 7F 77 7F 7F 7F 7F 7F 7F 7F 7F 7F 7F 7F 7F 7F 7F 7F 77 7F 7F 77 7F 77 7F 7F 7F 7F 7F 7F 7F 7F 7F 77 37 7F 7F 7F 7F 7F 7F 7F 7F 77 7F 77 7D 7F 77 77 77 7F 77 7F 

E7 go1	
00 go2
00 00 00 00 ?
00 desc
1F 00 00 00 - ?
81 go3 by 0x20 flag
07 00 00 00 ? fixed ?
28 00 00 00 achievement points
01 00 00 00 guild level
00 ?
57 65 20 52 61 69 64 20 57 69 74 68 6F 75 74 20 50 61 6E 74 73 00 - We Raid Without Pants ( Name )
FF FF FF FF magic numbers found int guild roster also, available only to higher level guilds ?
FF FF FF FF magic numbers found int guild roster also, available only to higher level guilds ?
03 00 00 00 ? fixed ?
1E go7
FF FF FF FF magic numbers found int guild roster also, available only to higher level guilds ?
FF FF FF FF magic numbers found int guild roster also, available only to higher level guilds ?
5C go0
00 ?
FF FF FF FF magic numbers found int guild roster also, available only to higher level guilds ?
F7 go6
D4 00 00 00 - total members
*/

	//ask for our pending invite requests
	sStackWorldPacket( data, SMSG_GUILD_FINDER_RESULT_LIST, 40000 );	//this is a huge packet, 32k...
	uint32 counter,counter_pos,tcounter,maxcounter;
	uint32 ourfilter = 0;
	ourfilter = ourfilter | ((FilterInterest & 0xFF));
	ourfilter = ourfilter | ((FilterActivity & 0xFF) << 8);
	ourfilter = ourfilter | ((FilterRole & 0xFF) << 16);
	if( GetPlayer()->getLevel() == PLAYER_LEVEL_CAP_BLIZZLIKE )
		ourfilter = ourfilter | (GF_GFLF_MAX_LEVEL << 24);
	ourfilter = ourfilter | (GF_GFLF_ANY_LEVEL << 24);
	
	maxcounter = 200;
	counter = 0;
	counter_pos = data.GetSize();
	data << uint32( counter );	// number of repeating structs
	GuildMap::iterator itr;
	tcounter = 0;
	for( itr = objmgr.GetGuildsBegin(); itr != objmgr.GetGuildsEnd() && tcounter < maxcounter; itr++ )
	{
		if( itr->second->m_GF_is_public == false )
			continue;
		uint32 filter_filter = itr->second->GetGuildFinderFilter() & ourfilter;
		uint32 matches = 0;
		if( filter_filter & 0x000000FF )
			matches++;
		if( filter_filter & 0x0000FF00 )
			matches++;
		if( filter_filter & 0x00FF0000 )
			matches++;
		if( filter_filter & 0xFF000000 )
			matches++;
		if( matches != 4 )
			continue;
		if( GetPlayer()->GuildFinderHasPendingRequest( itr->second->GetGuildId() ) )
			continue;
		counter++;
		tcounter++;
		data << uint8( 0x77 );	//obfuscated guid mask
	}
	tcounter = 0;
	for( itr = objmgr.GetGuildsBegin(); itr != objmgr.GetGuildsEnd() && tcounter < maxcounter; itr++ )
		{
			if( itr->second->m_GF_is_public == false )
				continue;
			uint32 filter_filter = itr->second->GetGuildFinderFilter() & ourfilter;
			uint32 matches = 0;
			if( filter_filter & 0x000000FF )
				matches++;
			if( filter_filter & 0x0000FF00 )
				matches++;
			if( filter_filter & 0x00FF0000 )
				matches++;
			if( filter_filter & 0xFF000000 )
				matches++;
			if( matches != 4 )
				continue;
			Player *p = objmgr.GetPlayer( itr->second->GetGuildLeader() );
//			if( p == NULL )
//				continue;
			if( GetPlayer()->GuildFinderHasPendingRequest( itr->second->GetGuildId() ) )
				continue;
			tcounter++;
			uint8 guild_guid[8];
			*(uint64*)guild_guid = itr->second->GetGuildId();
			data << ObfuscateByte( guild_guid[1] );	
			data << ObfuscateByte( guild_guid[2] );
			data << uint32( 0 );			//seen 0 or 1
			data << itr->second->GetGuildFinderDesc();
			data << uint32( 0x0000001F );	//filter interest mask ?
			data << ObfuscateByte( guild_guid[3] );	//go with flag 0x20
			data << uint32( 0x00000007 );	//filter class mask ?
			if( p )
				data << uint32( p->m_achievement_points );	//well this is guild achievement points and not player one
			else
				data << uint32( 0 );	
			data << uint32( itr->second->GetLevel() );
			data << uint8( 0x00 );
			data << itr->second->GetGuildName();
			data << uint32( 0xFFFFFFFF );	//high level guid has value
			data << uint32( 0xFFFFFFFF );	//high level guid has value
			data << uint32( 0x00000003 );	//filter availability mask ?
			data << ObfuscateByte( guild_guid[7] );	
			data << uint32( 0xFFFFFFFF );	//high level guid has value
			data << uint32( 0xFFFFFFFF );	//high level guid has value
			data << ObfuscateByte( guild_guid[0] );	
			data << uint8( 0x00 );			//?
			data << uint32( 0xFFFFFFFF );	//high level guid has value
			data << ObfuscateByte( guild_guid[6] );	//go
			data << uint32( itr->second->GetNumMembers() );
		}
	data.WriteAtPos( counter, counter_pos );
	/**/
	//create a fake packet so we can decode request add packet easier
/*	sStackWorldPacket( data, SMSG_GUILD_FINDER_RESULT_LIST, 10000 );	//this is a huge packet, 32k...
	uint32 guid_byte_count = 4;
	data << uint32( guid_byte_count );	// number of repeating structs
	for(uint32 i=0;i< guid_byte_count;i++)
		data << uint8( 0x77 );	//obfuscated guid mask
	uint8 guild_guid[8];
	*(uint64*)guild_guid = 0;
	for(uint32 i=0;i<guid_byte_count;i++)
	{
		*(uint64*)guild_guid = 0;
		guild_guid[i] = 0x10+i;
		data << ObfuscateByte( guild_guid[1] );	
		data << ObfuscateByte( guild_guid[2] );
		data << uint32( 1 );			//seen 0 or 1
		data << "desc";
		data << uint32( 0x0000001F );	//?
		data << ObfuscateByte( guild_guid[3] );	//go with flag 0x20
		data << uint32( 0x00000007 );	//?
		data << uint32( i );
		data << uint32( i*10 ); //level
		data << uint8( 0x00 );
		char tname[50];
		sprintf( tname, "name%d",i);
		data << tname;
		data << uint32( 0xFFFFFFFF );	//high level guid has value
		data << uint32( 0xFFFFFFFF );	//high level guid has value
		data << uint32( 0x00000003 );	//?
		data << ObfuscateByte( guild_guid[7] );	
		data << uint32( 0xFFFFFFFF );	//high level guid has value
		data << uint32( 0xFFFFFFFF );	//high level guid has value
		data << ObfuscateByte( guild_guid[0] );	
		data << uint8( 0x00 );			//?
		data << uint32( 0xFFFFFFFF );	//high level guid has value
		data << ObfuscateByte( guild_guid[6] );	//go
		data << uint32( i*100 );
	}
	/**/
	SendPacket(&data);
}

void WorldSession::HandleGuildFinderPlayerAddRequest(WorldPacket & recv_data)
{
	//player already has a guild ? how could he look for a new one ?
	if( GetPlayer()->m_playerInfo->guild != NULL )
	{ 
		return;
	}
/*
14333
	{CLIENT} Packet: (0x0108) CMSG_GUILD_FINDER_REQUEST_ADD PacketSize = 55 TimeStamp = 1485238
	D7 guild guid mask 11010111 - i could nto find any other value here :(
	03 00 00 00 activity
	1F 00 00 00 interest
	02 00 00 00 role
	45 75 72 6F 70 65 20 70 6C 61 79 65 72 20 6C 6F 73 74 20 69 6E 20 55 53 00 Europe player lost in US 
	81 go3
	59 6F 73 68 69 6D 69 74 7A 75 00 Yoshimitzu
	03 go2
	05 go1
	1E go7
	F7 go6
	6A go0
*/
	uint8 guild_GUID_mask;
	uint8 guild_GUID[8];
	string player_message;
	string player_name;
	*(uint64*)guild_GUID = 0;

	uint32 FilterInterest;
	uint32 FilterActivity;
	uint32 FilterRole;

	recv_data >> guild_GUID_mask;

	recv_data >> FilterActivity;
	recv_data >> FilterInterest;
	recv_data >> FilterRole;
	recv_data >> player_message;

	if( guild_GUID_mask & BIT_0x04 )
		recv_data >> guild_GUID[3];

	recv_data >> player_name;

	if( guild_GUID_mask & BIT_0x10 )
		recv_data >> guild_GUID[2];
	if( guild_GUID_mask & BIT_0x80 )
		recv_data >> guild_GUID[1];
	if( guild_GUID_mask & BIT_0x02 )
		recv_data >> guild_GUID[7];
	if( guild_GUID_mask & BIT_0x40 )
		recv_data >> guild_GUID[6];
	if( guild_GUID_mask & BIT_0x01 )
		recv_data >> guild_GUID[0];

	GUID_un_obfuscate( guild_GUID );
	Guild *g = objmgr.GetGuild( *(uint32*)guild_GUID );
	if( g )
	{
		GuildFinderRequest *r = new GuildFinderRequest;
		r->guild_id = g->GetGuildId();
		r->guild_name = g->GetGuildName();
		r->player_guid = GetPlayer()->GetLowGUID();
		r->player_message = " " + player_message + " ";
		r->player_name = GetPlayer()->GetName();
		r->stamp_unix = (uint32)UNIXTIME;
		GetPlayer()->GuildFinderAddRequest( r );
		g->GuildFinderAddRequest( r );
		//failed to add
		if( r->ref_count == 0 )
			delete r;
		Player *p = objmgr.GetPlayer( g->GetGuildLeader() );
		if( p == NULL )
			GetPlayer()->BroadcastMessage("Guild Leader is offline. Only he can review requests.");
		else if( p->GetSession() )
			p->GetSession()->OutPacket( SMSG_GUILD_FINDER_GUILD_REQUESTS_INTEROGATED );
	}
	else
	{
		GetPlayer()->BroadcastMessage("You managed to find a ghey bug. You probably tried to request membership from the first guild in the list. If not then report this error");
	}
/*	//just to find the masks later !
	else
	{
		sLog.outDebug("Received guid ID : %X %X %X %X %X %X %X %X",guild_GUID[0],guild_GUID[1],guild_GUID[2],guild_GUID[3],guild_GUID[4],guild_GUID[5],guild_GUID[6],guild_GUID[7]);
		GuildFinderRequest *r = new GuildFinderRequest;
		r->guild_id = *(uint32*)guild_GUID;
		char tname[50];
		sprintf( tname, "name%d",*(uint32*)guild_GUID);
		r->guild_name = tname;
		r->player_guid = GetPlayer()->GetLowGUID();
		r->player_message = "messg";
		r->player_name = GetPlayer()->GetName();
		r->stamp_unix = UNIXTIME;
		GetPlayer()->GuildFinderAddRequest( r );
	}/**/
	//ask for our pending invite requests
	sStackWorldPacket( data, SMSG_GUILD_FINDER_REQUEST_ACK, 1 );
	SendPacket(&data);
}

void WorldSession::HandleGuildFinderPlayerDelRequest(WorldPacket & recv_data)
{
	//player already has a guild ? how could he look for a new one ?
	if( GetPlayer()->m_playerInfo->guild != NULL )
	{ 
		return;
	}

	uint8 guild_GUID_mask;
	uint8 guild_GUID[8];
	*(uint64*)guild_GUID = 0;

	recv_data >> guild_GUID_mask;

//	sLog.outDebug("Received GUID mask : 0x%X",guild_GUID_mask);
//	return;

//	if( guild_GUID_mask & BIT_X )
//		recv_data >> guild_GUID[6];
	if( guild_GUID_mask & BIT_0x02 )
		recv_data >> guild_GUID[2];
	if( guild_GUID_mask & BIT_0x40 )
		recv_data >> guild_GUID[0];
	if( guild_GUID_mask & BIT_0x20 )
		recv_data >> guild_GUID[1];
	if( guild_GUID_mask & BIT_0x04 )
		recv_data >> guild_GUID[3];
//	if( guild_GUID_mask & BIT_X )
//		recv_data >> guild_GUID[7];

	GUID_un_obfuscate( guild_GUID );
	Guild *g = objmgr.GetGuild( *(uint32*)guild_GUID );
	if( g )
	{
		g->GuildFinderDelRequest( GetPlayer()->GetGUID() );
		Player *p = objmgr.GetPlayer( g->GetGuildLeader() );
		if( p && p->GetSession() )
			p->GetSession()->OutPacket( SMSG_GUILD_FINDER_GUILD_REQUESTS_INTEROGATED );
	}
	GetPlayer()->GuildFinderDelRequest( *(uint64*)guild_GUID );

	//ask for our pending invite requests
	sStackWorldPacket( data, SMSG_GUILD_FINDER_REQUEST_ACK, 1 );
	SendPacket(&data);
}

void WorldSession::HandleGuildFinderGuildSetFilter(WorldPacket & recv_data)
{
	//player already has a guild ? how could he look for a new one ?
	if( GetPlayer()->GetGuild() == NULL 
		|| GetPlayer()->GetGuild()->GetGuildLeader() != GetPlayer()->GetGUID()
		)
	{ 
		GetPlayer()->BroadcastMessage("Only Guild Leaders can perform this action");
		return;
	}

	uint8	FilterPublic;
	uint32	FilterInterest;
	uint32	FilterActivity;
	uint32	FilterRole;
	uint32	FilterLevelFlags;
	string	message;

	recv_data >> FilterPublic;	//0x80 means we are public
	recv_data >> FilterRole;
	recv_data >> FilterInterest;
	recv_data >> FilterActivity;
	recv_data >> FilterLevelFlags;		
	recv_data >> message;

	GetPlayer()->m_playerInfo->guild->GuildFinderSetFilter( (FilterPublic & 0x80)!=0, FilterInterest, FilterActivity, FilterRole, FilterLevelFlags, message.c_str() );
}

void WorldSession::HandleGuildFinderGuildGetRequests(WorldPacket & recv_data)
{
	//player already has a guild ? how could he look for a new one ?
	if( GetPlayer()->GetGuild() == NULL 
		|| GetPlayer()->GetGuild()->GetGuildLeader() != GetPlayer()->GetGUID()
		)
	{ 
		GetPlayer()->BroadcastMessage("Only Guild Leaders can perform this action");
		return;
	}

	float	Serializer;
	recv_data >> Serializer;	//huge float
	Serializer	+= 0x23;		//whatever increase ? this number grows with super speed

	Guild *g = GetPlayer()->GetGuild();
	uint32 requests_count = (uint32)g->m_guild_finder_requests.size();
/*
14333
{SERVER} Packet: (0xFA67) SMSG_NAME_QUERY_RESPONSE PacketSize = 19 TimeStamp = 3750825
8F E6 0E 49 03 03 00 52 61 79 65 6E 65 00 00 09 01 08 00 
E6 0E 49 03 00 00 00 03
E7 0F 48 02 00 00 00 02

{SERVER} Packet: (0x0B47) UNKNOWN PacketSize = 65 TimeStamp = 3847842
01 00 00 00 counter
32 EE 36 4E serializer
D9 guid mask
48 go2
1F 00 00 00 
02 go7
0F go1
E7 go0
52 61 79 65 6E 65 00 - requester name
73 6E 69 66 66 20 73 6E 69 66 66 00 - requester msg
02 00 00 00 ?
00 EA 24 00 reamining pending time
70 04 00 00 ? - 1136 
04 00 00 00 ?
00 00 00 00 
08 00 00 00 role ?
02 00 00 00 
02 go3
*/
	uint32 tcounter,maxcounter=100;
	g->GuildFinderCleanOfflines();
	WorldPacket data( SMSG_GUILD_FINDER_GUILD_GET_REQUESTS, requests_count * 20 );
	data << requests_count;
	data << Serializer;
	std::list< GuildFinderRequest *>::iterator itr;
	tcounter = 0;
	for( itr = g->m_guild_finder_requests.begin(); itr != g->m_guild_finder_requests.end() && tcounter < maxcounter; itr++,tcounter++ )
		data << uint8( 0xD9 );
	tcounter = 0;
	for( itr = g->m_guild_finder_requests.begin(); itr != g->m_guild_finder_requests.end() && tcounter < maxcounter; itr++,tcounter++ )
	{
		uint8 guild_guid[8];
		*(uint64*)guild_guid = (*itr)->player_guid;
		data << ObfuscateByte( guild_guid[2] );
		data << uint32( 0x0000001F );	//filter interest mask ?
		data << ObfuscateByte( guild_guid[7] );
		data << ObfuscateByte( guild_guid[1] );
		data << ObfuscateByte( guild_guid[0] );
		data << (*itr)->player_name;
		data << (*itr)->player_message;
		data << uint32( 0x00000002 );	//?
		data << uint32( UNIXTIME + MAX_GUILD_FINDER_PENDING_SECONDS - (*itr)->stamp_unix );	//?
		data << uint32( 0x00000470 );	//?
		data << uint32( 0x00000004 );	//?
		data << uint32( 0x00000000 );	//?
		data << uint32( 0x00000008 );	//?
		data << uint32( 0x00000002 );	//?
		data << ObfuscateByte( guild_guid[3] );

//		data << uint32( 0x00000003 );	//filter availability mask ?
//		data << uint32( 0x00000007 );	//filter class mask ?
//		data << ObfuscateByte( guild_guid[6] );
//		data << uint32( getLevel() );
	}
	SendPacket(&data);
}

void WorldSession::HandleGuildFinderGuildDelRequest(WorldPacket & recv_data)
{
	//player already has a guild ? how could he look for a new one ?
	if( GetPlayer()->GetGuild() == NULL 
		|| GetPlayer()->GetGuild()->GetGuildLeader() != GetPlayer()->GetGUID()
		)
	{ 
		GetPlayer()->BroadcastMessage("Only Guild Leaders can perform this action");
		return;
	}

	uint8 guid_mask;
	uint8 guidb[8];
	*(uint64*)guidb = 0;

	recv_data >> guid_mask;	

	if( guid_mask & BIT_3 )
		recv_data >> guidb[1];	
	if( guid_mask & BIT_1 )
		recv_data >> guidb[3];	
	if( guid_mask & BIT_2 )
		recv_data >> guidb[0];	
	if( guid_mask & BIT_0 )
		recv_data >> guidb[2];	

	GUID_un_obfuscate( guidb );

	//on blizz declining an invitation blacklistst the used
	GetPlayer()->GetGuild()->GuildFinderDelRequest( *(uint64*)guidb );
	//tell client to refresh he's list
	OutPacket( SMSG_GUILD_FINDER_GUILD_REQUESTS_INTEROGATED );

	//missing packet to refresh player guild request list
	Player *p = objmgr.GetPlayer( *(uint64*)guidb );
	if( p )
	{
		p->BroadcastMessage("Guild %s declined your invitation request",GetPlayer()->GetGuild()->GetGuildName() );
		p->GuildFinderSendPendingRequests();
	}
}