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

void WorldSession::HandleLFGJoin(WorldPacket& recvPacket)
{
    sLog.outDebug("CMSG_LFG_JOIN");

    uint8		map_count,unk_count;
	uint16		unk1;
	uint8		unk3;

/*
14480
{CLIENT} Packet: (0x3EB2) CMSG_LFG_JOIN PacketSize = 40 TimeStamp = 51443045
08 00 00 00 DPS role
00 00 unk
01 counter 1
49 01 00 02 map and type
03 counter ?
00 00 00 bytes
6E 6F 62 6F 64 79 20 75 73 65 73 20 74 68 69 73 20 66 65 61 74 75 72 65 00 comment
*/
	recvPacket >> GetPlayer()->LFG_roles;
	recvPacket >> unk1;
    recvPacket >> map_count;	
    if ( map_count == 0 )	//maybe this is also used to leave the search ?
        return;

	//clear old list 
	GetPlayer()->LFG_dungeons.BeginLoop();
	GetPlayer()->LFG_dungeons.SafeClear();
    for (int8 i = 0 ; i < map_count; ++i)
    {
		LFG_store *tstore = new LFG_store;
		uint32 tval;

        recvPacket >> tval;

		tstore->type = tval >> 24;
		tstore->map_id = tval & 0x00FFFFFF;

		GetPlayer()->LFG_dungeons.push_front( tstore );
    }
	GetPlayer()->LFG_dungeons.EndLoopAndCommit();

    recvPacket >> unk_count;						
    for (int8 i = 0 ; i < unk_count; ++i)
        recvPacket >> unk3;                       

    recvPacket >> GetPlayer()->LFG_comment;
	//!! hack, include gear score in comment !!!
//	GetPlayer()->LFG_comment = GetPlayer()->LFG_GetGearScore( ) + ":" + GetPlayer()->LFG_comment;

	sLfgMgr.LFGDungeonJoin( GetPlayer() );
}

void WorldSession::HandleLFGLeave(WorldPacket& recvPacket)
{
    sLog.outDebug("CMSG_LFG_LEAVE");
	sLfgMgr.LFGDungeonLeave( GetPlayer() );
}

void WorldSession::HandleLFGAnswerJoin(WorldPacket& recvPacket)
{
}

void WorldSession::HandleLFGSetFilterRoles(WorldPacket& recvPacket)
{
    sLog.outDebug("CMSG_LFG_SET_ROLES");
    recvPacket >> GetPlayer()->LFG_roles;   
}

void WorldSession::HandleLFGSetComment(WorldPacket& recvPacket)
{
    sLog.outDebug("CMSG_SET_LFG_COMMENT");
    recvPacket >> GetPlayer()->LFG_comment;
	//!! hack, include gear score in comment !!!
//	GetPlayer()->LFG_comment = GetPlayer()->LFG_GetGearScore( ) + ":" + GetPlayer()->LFG_comment;
}

void WorldSession::HandleLFGKickVote(WorldPacket& recvPacket)
{
}

void WorldSession::HandleLFGTeleport(WorldPacket& recvPacket)
{
}

//from DBC fetch all possible dungeons and send a list that we may join
void WorldSession::HandleLFGPlayerListRequest(WorldPacket& recvPacket)
{
/*
14333
01 02 01 00 06 01 01 00 00 00 07 00 00 00 00 00 00 00 00 00 00 00 07 00 00 00 00 00 00 00 07 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01 00 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 00 04 00 00 00 00 00 00 00 00 00 00 00 00 08 00 00 00 00 00 00 00 00 00 00 00 00 58 1B 00 00 2A 12 00 00 01 20 CB 00 00 53 DE 00 00 01 00 00 00 00 A6 00 00 00 B4 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 BC 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 AA 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 A0 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 93 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 A3 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 1E 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 26 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 ED 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 EF 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 F0 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 F1 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 F2 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 DD 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 D1 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 EE 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 D3 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 C9 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 B9 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 89 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 08 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 02 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 14 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 30 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 CC 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 CD 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 CE 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 D0 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 D2 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 D4 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 D5 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 D6 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 D7 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 D8 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 D9 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 DB 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 DC 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 AB 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 8A 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 C4 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 C2 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 C3 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 1C 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 01 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 97 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 C1 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 B6 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 BE 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 88 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 A4 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 AD 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 AE 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 AF 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 B0 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 B1 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 B2 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 B3 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 B5 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 B7 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 B8 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 BA 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 BB 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 BD 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 BF 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 C0 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 C6 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 C7 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 CA 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 CB 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 2A 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 35 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 29 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 28 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 2D 01 00 06 02 00 00 00 00 00 00 00 00 00 00 00 2E 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 2F 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 30 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 31 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 32 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 33 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 34 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 04 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 36 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 38 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 4A 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 42 01 00 05 02 00 00 00 00 00 00 00 00 00 00 00 48 01 00 02 02 00 00 00 00 00 00 00 00 00 00 00 41 01 00 05 02 00 00 00 00 00 00 00 00 00 00 00 2C 01 00 06 02 00 00 00 00 00 00 00 00 00 00 00 46 01 00 05 02 00 00 00 00 00 00 00 00 00 00 00 20 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 CF 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 00 01 00 05 02 00 00 00 00 00 00 00 00 00 00 00 01 01 00 02 02 00 00 00 00 00 00 00 00 00 00 00 06 01 00 06 02 00 00 00 00 00 00 00 00 00 00 00 1E 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 2B 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 1D 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 03 01 00 06 02 00 00 00 00 00 00 00 00 00 00 00 04 01 00 06 02 00 00 00 00 00 00 00 00 00 00 00 26 01 00 02 02 00 00 00 00 00 00 00 00 00 00 00 39 01 00 02 02 00 00 00 00 00 00 00 00 00 00 00 3B 01 00 02 02 00 00 00 00 00 00 00 00 00 00 00 3C 01 00 02 02 00 00 00 00 00 00 00 00 00 00 00 3D 01 00 02 02 00 00 00 00 00 00 00 00 00 00 00 3F 01 00 05 02 00 00 00 00 00 00 00 00 00 00 00 43 01 00 05 02 00 00 00 00 00 00 00 00 00 00 00 44 01 00 05 02 00 00 00 00 00 00 00 00 00 00 00 47 01 00 05 02 00 00 00 00 00 00 00 00 00 00 00 49 01 00 02 02 00 00 00 00 00 00 00 00 00 00 00 3E 01 00 02 02 00 00 00 00 00 00 00 00 00 00 00 40 01 00 05 02 00 00 00 00 00 00 00 00 00 00 00 3A 01 00 02 02 00 00 00 00 00 00 00 00 00 00 00 45 01 00 05 02 00 00 00 00 00 00 00 00 00 00 00 F5 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 F6 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 E0 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 DF 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 F7 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 F8 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 F9 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 FA 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 28 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 16 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 18 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 1A 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 AC 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 24 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 A1 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 92 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 95 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 96 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 8C 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 E1 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 11 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 22 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 1F 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 F3 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 F4 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 FB 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 FC 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 FD 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 FE 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 FF 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 2E 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 9F 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 05 01 00 06 02 00 00 00 00 00 00 00 00 00 00 00 32 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 12 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 14 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 17 01 00 02 02 00 00 00 00 00 00 00 00 00 00 00 18 01 00 02 02 00 00 00 00 00 00 00 00 00 00 00 37 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 25 01 00 02 02 00 00 00 00 00 00 00 00 00 00 00 06 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 66 01 00 02 02 00 00 00 00 00 00 00 00 00 00 00 4E 01 00 05 02 00 00 00 00 00 00 00 00 00 00 00 55 01 00 06 02 00 00 00 00 00 00 00 00 00 00 00 54 01 00 05 02 00 00 00 00 00 00 00 00 00 00 00 DA 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 E3 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 94 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 E2 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 69 01 00 02 02 00 00 00 00 00 00 00 00 00 00 00 6A 01 00 02 02 00 00 00 00 00 00 00 00 00 00 00 20 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 
*/
    sLog.outDebug("CMSG_LFG_PLAYER_LOCK_INFO_REQUEST");
{
	WorldPacket data( SMSG_LFG_PLAYER_INFO, 2648 );
//	data.CopyFromSnifferDump("03 55 01 00 06 01 B0 36 00 00 D0 7E 01 00 8C 01 00 00 B0 36 00 00 D0 7E 01 00 B0 36 00 00 D0 7E 01 00 10 A4 00 00 D0 7E 01 00 B0 36 00 00 00 00 00 00 B0 36 00 00 00 02 00 00 00 00 00 00 00 00 00 00 00 01 0F 11 01 00 28 82 01 00 01 00 00 00 00 04 00 00 00 00 00 00 00 00 00 00 00 00 08 00 00 00 00 00 00 00 00 00 00 00 00 20 B5 0C 00 00 00 00 00 01 8C 01 00 00 00 00 00 00 B0 36 00 00 01 2D 01 00 06 01 58 1B 00 00 68 BF 00 00 8C 01 00 00 00 00 00 00 68 BF 00 00 B0 36 00 00 D0 7E 01 00 10 A4 00 00 D0 7E 01 00 B0 36 00 00 00 00 00 00 58 1B 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 20 B5 0C 00 00 00 00 00 01 8C 01 00 00 00 00 00 00 58 1B 00 00 01 2C 01 00 06 01 01 00 00 00 07 00 00 00 00 00 00 00 00 00 00 00 07 00 00 00 00 00 00 00 07 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 D8 87 09 00 00 00 00 00 00 90 00 00 00 8A 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 C4 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 C2 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 C3 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 1C 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 01 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 97 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 C1 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 B6 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 BE 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 88 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 A4 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 AD 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 AE 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 AF 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 B0 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 B1 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 A5 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 10 01 00 01 03 00 00 00 00 00 00 00 00 00 00 00 ED 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 EF 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 F0 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 F1 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 F2 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 DD 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 D1 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 EE 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 D3 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 C9 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 B9 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 89 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 0A 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 08 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 B2 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 B3 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 B5 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 B7 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 B8 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 BA 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 BB 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 BD 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 BF 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 C0 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 C6 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 C7 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 CA 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 CB 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 B4 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 BC 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 12 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 AA 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 A0 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 93 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 A3 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 1E 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 26 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 02 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 14 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 30 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 CC 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 CD 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 CE 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 D0 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 D2 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 D4 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 D5 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 D6 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 D7 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 D8 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 D9 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 DB 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 DC 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 AB 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 FA 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 28 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 16 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 18 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 1A 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 AC 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 24 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 A1 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 92 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 95 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 96 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 8C 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 E1 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 11 01 00 01 03 00 00 00 00 00 00 00 00 00 00 00 22 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 1F 01 00 01 07 04 00 00 00 00 00 00 00 00 00 00 F3 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 F4 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 FB 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 FC 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 FD 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 FE 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 FF 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 2E 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 9F 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 05 01 00 06 03 00 00 00 00 00 00 00 00 00 00 00 32 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 12 01 00 01 03 00 00 00 00 00 00 00 00 00 00 00 14 01 00 01 03 00 00 00 00 00 00 00 00 00 00 00 17 01 00 02 03 00 00 00 00 00 00 00 00 00 00 00 18 01 00 02 03 00 00 00 00 00 00 00 00 00 00 00 25 01 00 02 03 00 00 00 00 00 00 00 00 00 00 00 06 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 54 01 00 05 06 00 00 00 00 00 00 00 00 00 00 00 DA 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 E3 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 94 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 E2 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 10 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 6A 01 00 02 06 00 00 00 00 00 00 00 00 00 00 00 20 01 00 01 07 04 00 00 00 00 00 00 00 00 00 00 2A 01 00 01 E9 03 00 00 00 00 00 00 00 00 00 00 35 01 00 01 E9 03 00 00 00 00 00 00 00 00 00 00 29 01 00 01 E9 03 00 00 00 00 00 00 00 00 00 00 28 01 00 01 E9 03 00 00 00 00 00 00 00 00 00 00 32 01 00 01 E9 03 00 00 00 00 00 00 00 00 00 00 34 01 00 01 E9 03 00 00 00 00 00 00 00 00 00 00 04 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 0E 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 36 01 00 01 E9 03 00 00 00 00 00 00 00 00 00 00 4A 01 00 01 03 00 00 00 00 00 00 00 00 00 00 00 20 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 CF 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 00 01 00 05 03 00 00 00 00 00 00 00 00 00 00 00 01 01 00 02 03 00 00 00 00 00 00 00 00 00 00 00 06 01 00 06 03 00 00 00 00 00 00 00 00 00 00 00 1E 01 00 01 07 04 00 00 00 00 00 00 00 00 00 00 2B 01 00 01 E9 03 00 00 00 00 00 00 00 00 00 00 1D 01 00 01 07 04 00 00 00 00 00 00 00 00 00 00 02 01 00 06 03 00 00 00 00 00 00 00 00 00 00 00 03 01 00 06 03 00 00 00 00 00 00 00 00 00 00 00 04 01 00 06 03 00 00 00 00 00 00 00 00 00 00 00 0C 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 26 01 00 02 03 00 00 00 00 00 00 00 00 00 00 00 F5 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 F6 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 E0 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 DF 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 F7 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 F8 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 F9 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 " );
	data.CopyFromSnifferDump("03 2D 01 00 06 01 58 1B 00 00 68 BF 00 00 8C 01 00 00 00 00 00 00 68 BF 00 00 00 00 00 00 D0 7E 01 00 00 00 00 00 D0 7E 01 00 B0 36 00 00 00 00 00 00 58 1B 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 01 0F 11 01 00 28 82 01 00 01 00 00 00 00 04 00 00 00 00 00 00 00 00 00 00 00 00 08 00 00 00 00 00 00 00 00 00 00 00 00 20 B5 0C 00 00 00 00 00 01 8C 01 00 00 00 00 00 00 58 1B 00 00 01 2C 01 00 06 01 01 00 00 00 07 00 00 00 00 00 00 00 00 00 00 00 07 00 00 00 00 00 00 00 07 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01 00 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 00 04 00 00 00 00 00 00 00 00 00 00 00 00 08 00 00 00 00 00 00 00 00 00 00 00 00 D8 87 09 00 00 00 00 00 01 8B 01 00 00 00 00 00 00 B0 36 00 00 01 55 01 00 06 01 B0 36 00 00 D0 7E 01 00 8C 01 00 00 00 00 00 00 D0 7E 01 00 00 00 00 00 D0 7E 01 00 00 00 00 00 D0 7E 01 00 B0 36 00 00 00 00 00 00 B0 36 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 01 0F 11 01 00 28 82 01 00 01 00 00 00 00 04 00 00 00 00 00 00 00 00 00 00 00 00 08 00 00 00 00 00 00 00 00 00 00 00 00 20 B5 0C 00 00 00 00 00 01 8C 01 00 00 00 00 00 00 B0 36 00 00 01 8E 00 00 00 02 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 14 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 30 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 CC 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 CD 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 CE 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 D0 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 D2 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 D4 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 D5 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 D6 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 D7 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 D8 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 D9 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 DB 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 DC 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 AB 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 8A 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 C4 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 C2 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 C3 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 1C 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 01 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 97 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 C1 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 B6 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 BE 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 88 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 A4 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 AD 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 AE 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 AF 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 B0 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 B1 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 A5 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 10 01 00 01 03 00 00 00 00 00 00 00 00 00 00 00 ED 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 EF 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 F0 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 F1 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 F2 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 DD 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 D1 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 EE 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 D3 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 C9 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 B9 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 89 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 0A 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 08 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 B2 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 B3 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 B5 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 B7 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 B8 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 BA 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 BB 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 BD 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 BF 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 C0 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 C6 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 C7 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 CA 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 CB 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 B4 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 BC 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 12 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 AA 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 A0 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 93 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 A3 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 1E 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 26 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 2A 01 00 01 E9 03 00 00 00 00 00 00 00 00 00 00 35 01 00 01 E9 03 00 00 00 00 00 00 00 00 00 00 29 01 00 01 E9 03 00 00 00 00 00 00 00 00 00 00 28 01 00 01 E9 03 00 00 00 00 00 00 00 00 00 00 32 01 00 01 E9 03 00 00 00 00 00 00 00 00 00 00 34 01 00 01 E9 03 00 00 00 00 00 00 00 00 00 00 04 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 0E 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 36 01 00 01 E9 03 00 00 00 00 00 00 00 00 00 00 4A 01 00 01 03 00 00 00 00 00 00 00 00 00 00 00 20 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 CF 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 00 01 00 05 03 00 00 00 00 00 00 00 00 00 00 00 01 01 00 02 03 00 00 00 00 00 00 00 00 00 00 00 06 01 00 06 03 00 00 00 00 00 00 00 00 00 00 00 1E 01 00 01 07 04 00 00 00 00 00 00 00 00 00 00 2B 01 00 01 E9 03 00 00 00 00 00 00 00 00 00 00 1D 01 00 01 07 04 00 00 00 00 00 00 00 00 00 00 02 01 00 06 03 00 00 00 00 00 00 00 00 00 00 00 03 01 00 06 03 00 00 00 00 00 00 00 00 00 00 00 04 01 00 06 03 00 00 00 00 00 00 00 00 00 00 00 0C 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 26 01 00 02 03 00 00 00 00 00 00 00 00 00 00 00 F5 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 F6 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 E0 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 DF 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 F7 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 F8 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 F9 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 FA 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 28 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 16 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 18 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 1A 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 AC 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 24 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 A1 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 92 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 95 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 96 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 8C 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 E1 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 11 01 00 01 03 00 00 00 00 00 00 00 00 00 00 00 22 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 1F 01 00 01 07 04 00 00 00 00 00 00 00 00 00 00 F3 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 F4 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 FB 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 FC 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 FD 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 FE 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 FF 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 2E 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 9F 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 05 01 00 06 03 00 00 00 00 00 00 00 00 00 00 00 32 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 12 01 00 01 03 00 00 00 00 00 00 00 00 00 00 00 14 01 00 01 03 00 00 00 00 00 00 00 00 00 00 00 17 01 00 02 03 00 00 00 00 00 00 00 00 00 00 00 18 01 00 02 03 00 00 00 00 00 00 00 00 00 00 00 25 01 00 02 03 00 00 00 00 00 00 00 00 00 00 00 06 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 DA 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 E3 00 00 02 03 00 00 00 00 00 00 00 00 00 00 00 94 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 E2 00 00 05 03 00 00 00 00 00 00 00 00 00 00 00 10 00 00 01 03 00 00 00 00 00 00 00 00 00 00 00 20 01 00 01 07 04 00 00 00 00 00 00 00 00 00 00 " );
	SendPacket( &data );
	return;
}
	uint32 counter_pos,counter_val;
	sStackWorldPacket( data, SMSG_LFG_PLAYER_INFO, 5000 );
	//build up random dungeon list 
	counter_pos = data.GetSize();
	counter_val = 0;
	data << uint8( 0 ); //-> holder for the number of random dungeons available
/*    for (uint32 i = 0; i < dbcLFGDungeonStore.GetNumRows(); ++i)
    {
		LFGDungeonEntry *de = dbcLFGDungeonStore.LookupRow(i);
        if ( de->type == LFG_TYPE_RANDOM && de->minlevel <= GetPlayer()->getLevel() && GetPlayer()->getLevel() <= de->maxlevel )
		{
			uint32 combined_data = de->map_id | (de->type << 24);
			data << uint32( combined_data );
			data << uint8( 0 ); //done ?
			//these are the reward details, need to implement these later
			data << uint32(0);	//base money
			data << uint32(0);	//basexp
			data << uint32(0);	//rand money
			data << uint32(0);	//rand xp
			data << uint8(0);	//has item rewards
			counter_val++;
		}
    }
	data.WriteAtPos( (uint8)counter_val, counter_pos );*/
	//this is needed or custom realms will not be able to use RB
	uint32 mylevel = GetPlayer()->getLevel();
	if( mylevel > PLAYER_LEVEL_CAP_BLIZZLIKE )
		mylevel = PLAYER_LEVEL_CAP_BLIZZLIKE;
	//these are available dungeons without special rewards
	counter_pos = data.GetSize();
	counter_val = 0;
	data << uint32( 0 ); //-> holder for the number of random dungeons available
    for (uint32 i = 0; i < dbcLFGDungeonStore.GetNumRows(); ++i)
    {
		LFGDungeonEntry *de = dbcLFGDungeonStore.LookupRow(i);
		if( de->type == LFG_TYPE_RANDOM || de->type == LFG_TYPE_ZONE || de->type == LFG_TYPE_QUEST )
			continue;
        LfgStatusCode sc = LFG_LOCKSTATUS_OK;
		if (de->minlevel > mylevel )
            sc = LFG_LOCKSTATUS_TOO_LOW_LEVEL;
        else if (de->maxlevel < mylevel )
            sc = LFG_LOCKSTATUS_TOO_HIGH_LEVEL;
		if( sc == LFG_LOCKSTATUS_OK )
		{
			MapInfo * pMapInfo = WorldMapInfoStorage.LookupEntry( de->map_id );
			if( pMapInfo == NULL )
				sc = LFG_LOCKSTATUS_RAID_LOCKED;
		}
		uint32 combined_data = de->map_id | (de->type << 24);
		data << uint32( combined_data );
		data << uint32( sc );
		counter_val++;
    }
	data.WriteAtPos( counter_val, counter_pos );

    SendPacket(&data);
}

//from DBC fetch all possible dungeons and send a list that we may join as group
void WorldSession::HandleLFGGroupListRequest(WorldPacket& recvPacket)
{
    sLog.outDebug("CMSG_LFD_PARTY_LOCK_INFO_REQUEST");

/*
01 - 1 guid
1E 7D 81 02 00 00 00 07 guid
5C 00 00 00 number of entries for this guid
F1 00 00 05 02 00 00 00 
F2 00 00 05 02 00 00 00 
DD 00 00 05 02 00 00 00 D1 00 00 01 02 00 00 00 D3 00 00 05 02 00 00 00 C9 00 00 05 03 00 00 00 B9 00 00 05 03 00 00 00 89 00 00 01 03 00 00 00 0A 00 00 01 03 00 00 00 08 00 00 01 03 00 00 00 10 00 00 01 03 00 00 00 AC 00 00 01 03 00 00 00 B4 00 00 05 03 00 00 00 BC 00 00 05 03 00 00 00 0C 00 00 01 03 00 00 00 0E 00 00 01 03 00 00 00 14 00 00 01 03 00 00 00 CE 00 00 01 02 00 00 00 CF 00 00 01 02 00 00 00 D2 00 00 05 02 00 00 00 D4 00 00 05 02 00 00 00 D5 00 00 05 02 00 00 00 CD 00 00 05 02 00 00 00 16 00 00 01 03 00 00 00 FC 00 00 05 02 00 00 00 D7 00 00 05 02 00 00 00 D9 00 00 05 02 00 00 00 DB 00 00 05 02 00 00 00 8A 00 00 01 03 00 00 00 1C 00 00 01 03 00 00 00 01 00 00 01 03 00 00 00 20 00 00 01 03 00 00 00 97 00 00 01 03 00 00 00 B6 00 00 05 03 00 00 00 BE 00 00 05 03 00 00 00 88 00 00 01 03 00 00 00 A4 00 00 01 03 00 00 00 AD 00 00 01 03 00 00 00 AE 00 00 01 03 00 00 00 AB 00 00 01 03 00 00 00 B2 00 00 05 03 00 00 00 B3 00 00 05 03 00 00 00 B5 00 00 05 03 00 00 00 B7 00 00 05 03 00 00 00 B8 00 00 05 03 00 00 00 BA 00 00 05 03 00 00 00 BB 00 00 05 03 00 00 00 BD 00 00 05 03 00 00 00 BF 00 00 05 03 00 00 00 C0 00 00 05 03 00 00 00 C6 00 00 01 03 00 00 00 E2 00 00 05 02 00 00 00 CB 00 00 01 02 00 00 00 12 00 00 01 03 00 00 00 AA 00 00 01 03 00 00 00 93 00 00 01 03 00 00 00 A3 00 00 01 03 00 00 00 1E 00 00 01 03 00 00 00 26 00 00 01 03 00 00 00 02 00 00 01 03 00 00 00 04 00 00 01 03 00 00 00 06 00 00 01 03 00 00 00 92 00 00 01 03 00 00 00 94 00 00 01 03 00 00 00 95 00 00 01 03 00 00 00 96 00 00 01 03 00 00 00 8C 00 00 01 03 00 00 00 FB 00 00 01 02 00 00 00 FD 00 00 01 02 00 00 00 FE 00 00 05 02 00 00 00 FF 00 00 01 02 00 00 00 00 01 00 05 02 00 00 00 10 01 00 01 03 00 00 00 11 01 00 01 03 00 00 00 1D 01 00 01 02 00 00 00 06 01 00 06 02 00 00 00 1E 01 00 01 02 00 00 00 12 01 00 01 03 00 00 00 14 01 00 01 03 00 00 00 1F 01 00 01 02 00 00 00 20 01 00 01 02 00 00 00 02 01 00 06 03 00 00 00 03 01 00 06 03 00 00 00 04 01 00 06 03 00 00 00 A5 00 00 01 03 00 00 00 F5 00 00 01 02 00 00 00 F9 00 00 05 02 00 00 00 28 00 00 01 03 00 00 00 18 00 00 01 03 00 00 00 1A 00 00 01 03 00 00 00 22 00 00 01 03 00 00 00 24 00 00 01 03 00 00 00 
*/
	if( GetPlayer() == NULL )
		return; // noway
	if( GetPlayer()->GetGroup() == NULL )
		return; // hmm client never asked this unless you had a group

	//get raid min and max level
	uint32 raid_minlevel = GetPlayer()->getLevel();
	uint32 raid_maxlevel = GetPlayer()->getLevel();
	uint8 player_count = 0;
	Group * group = GetPlayer()->GetGroup(); 
	// Loop through each raid group.
	for( uint8 k = 0; k < group->GetSubGroupCount(); k++ )
	{
		SubGroup * subgroup = group->GetSubGroup( k );
		if( subgroup )
		{
			group->Lock();
			for( GroupMembersSet::iterator itr = subgroup->GetGroupMembersBegin(); itr != subgroup->GetGroupMembersEnd(); ++itr )
			{
				if( (*itr)->m_loggedInPlayer == NULL )
					continue;
				if( (*itr)->m_loggedInPlayer->getLevel() < raid_minlevel )
					raid_minlevel = (*itr)->m_loggedInPlayer->getLevel();
				if( (*itr)->m_loggedInPlayer->getLevel() > raid_maxlevel )
					raid_maxlevel = (*itr)->m_loggedInPlayer->getLevel();
				player_count++;
			}
			group->Unlock();
		}
	}
	if( raid_minlevel > PLAYER_LEVEL_CAP_BLIZZLIKE )
		raid_minlevel = PLAYER_LEVEL_CAP_BLIZZLIKE;
	if( raid_maxlevel > PLAYER_LEVEL_CAP_BLIZZLIKE )
		raid_maxlevel = PLAYER_LEVEL_CAP_BLIZZLIKE;

	uint32 counter_pos,counter_val;
	sStackWorldPacket( data, SMSG_LFG_PARTY_INFO, 5000 );
	data << uint8( 1 );	//1 player count = leader
	data << uint64( GetPlayer()->GetGUID() );
	//build up random dungeon list 
	counter_pos = data.GetSize();
	counter_val = 0;
	data << uint32( 0 ); //-> holder for the number of random dungeons available
    for (uint32 i = 0; i < dbcLFGDungeonStore.GetNumRows(); ++i)
    {
		LFGDungeonEntry *de = dbcLFGDungeonStore.LookupRow(i);
		if( de->type == LFG_TYPE_RANDOM || de->type == LFG_TYPE_ZONE )
			continue;
        LfgStatusCode sc = LFG_LOCKSTATUS_OK;
        if (de->minlevel > raid_minlevel)
            sc = LFG_LOCKSTATUS_TOO_LOW_LEVEL;
        else if (de->maxlevel < raid_maxlevel)
            sc = LFG_LOCKSTATUS_TOO_HIGH_LEVEL;
		if( sc == LFG_LOCKSTATUS_OK )
		{
			MapInfo * pMapInfo = WorldMapInfoStorage.LookupEntry( de->map_id );
			if( pMapInfo == NULL )
				sc = LFG_LOCKSTATUS_RAID_LOCKED;
		}
		data << uint32( de->ID );
		data << uint32( sc );
		counter_val++;
    }
	data.WriteAtPos( counter_val, counter_pos );

    SendPacket(&data);
}

void WorldSession::HandleLFGQueueInstanceJoin(WorldPacket& recvPacket)
{
    sLog.outDebug("CMSG_SEARCH_LFG_JOIN");

	uint32		combined_data;
	recvPacket >> combined_data;

	GetPlayer()->RB_map_id = combined_data & 0x00FFFFFF;
	GetPlayer()->RB_type = combined_data >> 24;

	sLfgMgr.RBSendLookingPlayerList( GetPlayer() );
}

void WorldSession::HandleLFGQueueInstanceLeave(WorldPacket& recvPacket)
{
	GetPlayer()->RB_map_id = 0;
	GetPlayer()->RB_type = 0;
}

void WorldSession::HandleLFGStatusQuery(WorldPacket& recvPacket)
{
	sStackWorldPacket( data, SMSG_LFG_UPDATE_PLAYER, 2 + 100 );
	data << uint16( 0x000E );
	SendPacket( &data );

	data.Initialize( SMSG_LFG_UPDATE_PARTY );
	data << uint16( 0x000E );
	SendPacket( &data );

	HandleLFGPlayerListRequest( recvPacket );
}

#if 0
/*
void WorldSession::HandleLfgProposalResultOpcode(WorldPacket &recv_data)
{
    sLog.outDebug("CMSG_LFG_PROPOSAL_RESULT");

    uint32 lfgGroupID;                                      // Internal lfgGroupID
    bool accept;                                            // Accept to join?
    recv_data >> lfgGroupID;
    recv_data >> accept;

    sLFGMgr.UpdateProposal(lfgGroupID, GetPlayer()->GetGUIDLow(), accept);
}

void WorldSession::HandleLfgSetBootVoteOpcode(WorldPacket &recv_data)
{
    sLog.outDebug("CMSG_LFG_SET_BOOT_VOTE");

    bool agree;                                             // Agree to kick player
    recv_data >> agree;

    sLFGMgr.UpdateBoot(GetPlayer(), agree);
}

void WorldSession::HandleLfgTeleportOpcode(WorldPacket &recv_data)
{
    sLog.outDebug("CMSG_LFG_TELEPORT");

    bool out;
    recv_data >> out;

    sLFGMgr.TeleportPlayer(GetPlayer(), out);
}

void WorldSession::SendLfgUpdateParty(uint8 updateType)
{
    bool join = false;
    bool extrainfo = false;
    bool queued = false;

    switch(updateType)
    {
    case LFG_UPDATETYPE_JOIN_PROPOSAL:
        extrainfo = true;
        break;
    case LFG_UPDATETYPE_ADDED_TO_QUEUE:
        extrainfo = true;
        join = true;
        queued = true;
        break;
    case LFG_UPDATETYPE_CLEAR_LOCK_LIST:
        // join = true;  // TODO: Sometimes queued and extrainfo - Check ocurrences...
        queued = true;
        break;
    case LFG_UPDATETYPE_PROPOSAL_BEGIN:
        extrainfo = true;
        join = true;
        break;
    }

    LfgDungeonSet *dungeons = GetPlayer()->GetLfgDungeons();
    uint8 size = dungeons->size();
    std::string comment = GetPlayer()->GetLfgComment();
    sLog.outDebug("SMSG_LFG_UPDATE_PARTY");
    WorldPacket data(SMSG_LFG_UPDATE_PARTY, 1 + 1 + (extrainfo ? 1 : 0) * (1 + 1 + 1 + 1 + 1 + size * 4 + comment.length()));
    data << uint8(updateType);                              // Lfg Update type
    data << uint8(extrainfo);                               // Extra info
    if (extrainfo)
    {
        data << uint8(join);                                // LFG Join
        data << uint8(queued);                              // Join the queue
        data << uint8(0);                                   // unk - Always 0
        data << uint8(0);                                   // unk - Always 0
        for (uint8 i = 0; i < 3; ++i)
            data << uint8(0);                               // unk - Always 0

        data << uint8(size);

        for (LfgDungeonSet::const_iterator it = dungeons->begin(); it != dungeons->end(); ++it)
            data << uint32(*it);

        data << comment;
    }
    SendPacket(&data);
}

void WorldSession::SendLfgRoleChosen(uint64 guid, uint8 roles)
{
    sLog.outDebug("SMSG_LFG_ROLE_CHOSEN");

    WorldPacket data(SMSG_LFG_ROLE_CHOSEN, 8 + 1 + 4);
    data << uint64(guid);                                   // Guid
    data << uint8(roles > 0);                               // Ready
    data << uint32(roles);                                  // Roles
    SendPacket(&data);
}

void WorldSession::SendLfgQueueStatus(uint32 dungeon, int32 waitTime, int32 avgWaitTime, int32 waitTimeTanks, int32 waitTimeHealer, int32 waitTimeDps, uint32 queuedTime, uint8 tanks, uint8 healers, uint8 dps)
{
    sLog.outDebug("SMSG_LFG_QUEUE_STATUS");
    WorldPacket data(SMSG_LFG_QUEUE_STATUS, 4 + 4 + 4 + 4 + 4 +4 + 1 + 1 + 1 + 4);

    data << uint32(dungeon);                                // Dungeon
    data << uint32(avgWaitTime);                            // Average Wait time
    data << uint32(waitTime);                               // Wait Time
    data << uint32(waitTimeTanks);                          // Wait Tanks
    data << uint32(waitTimeHealer);                         // Wait Healers
    data << uint32(waitTimeDps);                            // Wait Dps
    data << uint8(tanks);                                   // Tanks needed
    data << uint8(healers);                                 // Healers needed
    data << uint8(dps);                                     // Dps needed
    data << uint32(queuedTime);                             // Player wait time in queue
    SendPacket(&data);
}

void WorldSession::SendLfgPlayerReward(uint32 rdungeonEntry, uint32 sdungeonEntry, uint8 done, const LfgReward *reward, const Quest *qRew)
{
    if (!rdungeonEntry || !sdungeonEntry || !qRew)
        return;

    uint8 itemNum = qRew ? qRew->GetRewItemsCount() : 0;
    
    sLog.outDebug("SMSG_LFG_PLAYER_REWARD");
    WorldPacket data(SMSG_LFG_PLAYER_REWARD, 4 + 4 + 1 + 4 + 4 + 4 + 4 + 4 + 1 + itemNum * (4 + 4 + 4));
    data << uint32(rdungeonEntry);                          // Random Dungeon Finished
    data << uint32(sdungeonEntry);                          // Dungeon Finished
    data << uint8(done);
    data << uint32(1);
    data << uint32(qRew->GetRewOrReqMoney());
    data << uint32(qRew->XPValue(GetPlayer()));
    data << uint32(reward->reward[done].variableMoney);
    data << uint32(reward->reward[done].variableXP);
    data << uint8(itemNum);
    if (itemNum)
    {
        ItemPrototype const* iProto = NULL;
        for (uint8 i = 0; i < QUEST_REWARDS_COUNT; ++i)
        {
            if (!qRew->RewItemId[i])
                continue;

            iProto = ObjectMgr::GetItemPrototype(qRew->RewItemId[i]);

            data << uint32(qRew->RewItemId[i]);
            data << uint32(iProto ? iProto->DisplayInfoID : 0);
            data << uint32(qRew->RewItemCount[i]);
        }
    }
    SendPacket(&data);
}

void WorldSession::SendLfgBootPlayer(LfgPlayerBoot *pBoot)
{
    sLog.outDebug("SMSG_LFG_BOOT_PLAYER");

    int8 playerVote = pBoot->votes[GetPlayer()->GetGUIDLow()];
    uint8 votesNum = 0;
    uint8 agreeNum = 0;
    uint32 secsleft = uint8((pBoot->cancelTime - time(NULL)) / 1000);
    for (LfgAnswerMap::const_iterator it = pBoot->votes.begin(); it != pBoot->votes.end(); ++it)
    {
        if (it->second != LFG_ANSWER_PENDING)
        {
            ++votesNum;
            if (it->second == LFG_ANSWER_AGREE)
                ++agreeNum;
        }
    }

    WorldPacket data(SMSG_LFG_BOOT_PLAYER, 1 + 1 + 1 + 8 + 4 + 4 + 4 + 4 + pBoot->reason.length());
    data << uint8(pBoot->inProgress);                       // Vote in progress
    data << uint8(playerVote != -1);                        // Did Vote
    data << uint8(playerVote == 1);                         // Agree
    data << uint64(MAKE_NEW_GUID(pBoot->victimLowGuid, 0, HIGHGUID_PLAYER)); // Victim GUID
    data << uint32(votesNum);                               // Total Votes
    data << uint32(agreeNum);                               // Agree Count
    data << uint32(secsleft);                               // Time Left
    data << uint32(pBoot->votedNeeded);                     // Needed Votes
    data << pBoot->reason.c_str();                          // Kick reason
    SendPacket(&data);
}

void WorldSession::SendUpdateProposal(uint32 proposalId, LfgProposal *pProp)
{
    if (!pProp)
        return;

    uint32 pLogGuid = GetPlayer()->GetGUIDLow();
    LfgProposalPlayerMap::const_iterator itPlayer = pProp->players.find(pLogGuid);
    if (itPlayer == pProp->players.end())                   // Player MUST be in the proposal
        return;

    LfgProposalPlayer *ppPlayer = itPlayer->second;
    uint32 pLowGroupGuid = ppPlayer->groupLowGuid;
    uint32 dLowGuid = pProp->groupLowGuid;
    uint32 dungeonId = pProp->dungeonId;
    uint32 isSameDungeon = GetPlayer()->GetGroup() && GetPlayer()->GetGroup()->GetLfgDungeonEntry() == dungeonId;

    sLog.outDebug("SMSG_LFG_PROPOSAL_UPDATE");
    WorldPacket data(SMSG_LFG_PROPOSAL_UPDATE, 4 + 1 + 4 + 4 + 1 + 1 + pProp->players.size() * (4 + 1 + 1 + 1 + 1 +1));
    if (!dLowGuid && GetPlayer()->GetLfgDungeons()->size() == 1)    // New group - select the dungeon the player selected
        dungeonId = *GetPlayer()->GetLfgDungeons()->begin();
    if (LFGDungeonEntry const *dungeon = sLFGDungeonStore.LookupEntry(dungeonId))
        dungeonId = dungeon->Entry();
    data << uint32(dungeonId);                              // Dungeon
    data << uint8(pProp->state);                            // Result state
    data << uint32(proposalId);                             // Internal Proposal ID
    data << uint32(0);                                      // Bosses killed - FIXME
    data << uint8(isSameDungeon);                           // Silent (show client window)
    data << uint8(pProp->players.size());                   // Group size

    for (itPlayer = pProp->players.begin(); itPlayer != pProp->players.end(); ++itPlayer)
    {
        ppPlayer = itPlayer->second;
        data << uint32(ppPlayer->role);                     // Role
        data << uint8(itPlayer->first == pLogGuid);         // Self player
        if (!ppPlayer->groupLowGuid)                        // Player not it a group
        {
            data << uint8(0);                               // Not in dungeon
            data << uint8(0);                               // Not same group
        }
        else
        {
            data << uint8(ppPlayer->groupLowGuid == dLowGuid);  // In dungeon (silent)
            data << uint8(ppPlayer->groupLowGuid == pLowGroupGuid); // Same Group than player
        }
        data << uint8(ppPlayer->accept != LFG_ANSWER_PENDING); // Answered
        data << uint8(ppPlayer->accept == LFG_ANSWER_AGREE); // Accepted
    }
    SendPacket(&data);
}

void WorldSession::SendLfgUpdateSearch(bool update)
{
    sLog.outDebug("SMSG_LFG_UPDATE_SEARCH");

    WorldPacket data(SMSG_LFG_UPDATE_SEARCH, 1);
    data << uint8(update);                                  // In Lfg Queue?
    SendPacket(&data);
}

void WorldSession::SendLfgDisabled()
{
    sLog.outDebug("SMSG_LFG_DISABLED");
    WorldPacket data(SMSG_LFG_DISABLED, 0);
    SendPacket(&data);
}

void WorldSession::SendLfgOfferContinue(uint32 dungeonEntry)
{
    sLog.outDebug("SMSG_LFG_OFFER_CONTINUE");

    WorldPacket data(SMSG_LFG_OFFER_CONTINUE, 4);
    data << uint32(dungeonEntry);
    SendPacket(&data);
}

void WorldSession::SendLfgTeleportError(uint8 err)
{
    sLog.outDebug("SMSG_LFG_TELEPORT_DENIED");

    WorldPacket data(SMSG_LFG_TELEPORT_DENIED, 4);
    data << uint32(err);                                    // Error
    SendPacket(&data);
}

LFGMgr::LFGMgr()
{
    m_QueueTimer = 0;
    m_WaitTimeAvg = -1;
    m_WaitTimeTank = -1;
    m_WaitTimeHealer = -1;
    m_WaitTimeDps = -1;
    m_NumWaitTimeAvg = 0;
    m_NumWaitTimeTank = 0;
    m_NumWaitTimeHealer = 0;
    m_NumWaitTimeDps = 0;
    m_update = true;
    m_lfgProposalId = 1;
    GetAllDungeons();
}

LFGMgr::~LFGMgr()
{
    for (LfgRewardMap::iterator itr = m_RewardMap.begin(); itr != m_RewardMap.end(); ++itr)
        delete itr->second;
    m_RewardMap.clear();

    m_EncountersByAchievement.clear();

    for (LfgQueueInfoMap::iterator it = m_QueueInfoMap.begin(); it != m_QueueInfoMap.end(); ++it)
        delete it->second;
    m_QueueInfoMap.clear();

    for (LfgProposalMap::iterator it = m_Proposals.begin(); it != m_Proposals.end(); ++it)
        delete it->second;
    m_Proposals.clear();

    for (LfgPlayerBootMap::iterator it = m_Boots.begin(); it != m_Boots.end(); ++it)
        delete it->second;
    m_Boots.clear();

    for (LfgRoleCheckMap::iterator it = m_RoleChecks.begin(); it != m_RoleChecks.end(); ++it)
        delete it->second;
    m_RoleChecks.clear();

    for (LfgDungeonMap::iterator it = m_CachedDungeonMap.begin(); it != m_CachedDungeonMap.end(); ++it)
        delete it->second;
    m_CachedDungeonMap.clear();

    m_CompatibleMap.clear();
    m_QueueInfoMap.clear();
    m_currentQueue.clear();
    m_newToQueue.clear();
}

/// <summary>
/// Load achievement <-> encounter associations
/// </summary>
void LFGMgr::LoadDungeonEncounters()
{
    m_EncountersByAchievement.clear();

    uint32 count = 0;
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT achievementId, dungeonId FROM lfg_dungeon_encounters");

    if (!result)
    {
        barGoLink bar(1);
        bar.step();

        sLog.outString();
        sLog.outErrorDb(">> Loaded 0 dungeon encounter lfg associations. DB table `lfg_dungeon_encounters` is empty!");
        return;
    }

    barGoLink bar(result->GetRowCount());

    Field* fields = NULL;
    do
    {
        bar.step();
        fields = result->Fetch();
        uint32 achievementId = fields[0].GetUInt32();
        uint32 dungeonId = fields[1].GetUInt32();

        if (AchievementEntry const* achievement = sAchievementStore.LookupEntry(achievementId))
        {
            if (!(achievement->flags & ACHIEVEMENT_FLAG_COUNTER))
            {
                sLog.outErrorDb("Achievement %u specified in table `lfg_dungeon_encounters` is not a statistic!", achievementId);
                continue;
            }
        }
        else
        {
            sLog.outErrorDb("Achievement %u specified in table `lfg_dungeon_encounters` does not exist!", achievementId);
            continue;
        }

        if (!sLFGDungeonStore.LookupEntry(dungeonId))
        {
            sLog.outErrorDb("Dungeon %u specified in table `lfg_dungeon_encounters` does not exist!", dungeonId);
            continue;
        }

        m_EncountersByAchievement[achievementId] = dungeonId;
        ++count;
    } while (result->NextRow());

    sLog.outString();
    sLog.outString(">> Loaded %u dungeon encounter lfg associations.", count);
}

/// <summary>
/// Load rewards for completing dungeons
/// </summary>
void LFGMgr::LoadRewards()
{
    for (LfgRewardMap::iterator itr = m_RewardMap.begin(); itr != m_RewardMap.end(); ++itr)
        delete itr->second;
    m_RewardMap.clear();

    uint32 count = 0;
    // ORDER BY is very important for GetRandomDungeonReward!
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT dungeonId, maxLevel, firstQuestId, firstMoneyVar, firstXPVar, otherQuestId, otherMoneyVar, otherXPVar FROM lfg_dungeon_rewards ORDER BY dungeonId, maxLevel ASC");

    if (!result)
    {
        barGoLink bar(1);
        bar.step();

        sLog.outString();
        sLog.outErrorDb(">> Loaded 0 lfg dungeon rewards. DB table `lfg_dungeon_rewards` is empty!");
        return;
    }

    barGoLink bar(result->GetRowCount());

    Field* fields = NULL;
    do
    {
        bar.step();
        fields = result->Fetch();
        uint32 dungeonId = fields[0].GetUInt32();
        uint32 maxLevel = fields[1].GetUInt8();
        uint32 firstQuestId = fields[2].GetUInt32();
        uint32 firstMoneyVar = fields[3].GetUInt32();
        uint32 firstXPVar = fields[4].GetUInt32();
        uint32 otherQuestId = fields[5].GetUInt32();
        uint32 otherMoneyVar = fields[6].GetUInt32();
        uint32 otherXPVar = fields[7].GetUInt32();

        if (!sLFGDungeonStore.LookupEntry(dungeonId))
        {
            sLog.outErrorDb("Dungeon %u specified in table `lfg_dungeon_rewards` does not exist!", dungeonId);
            continue;
        }

        if (!maxLevel || maxLevel > sWorld.getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
        {
            sLog.outErrorDb("Level %u specified for dungeon %u in table `lfg_dungeon_rewards` can never be reached!", maxLevel, dungeonId);
            maxLevel = sWorld.getIntConfig(CONFIG_MAX_PLAYER_LEVEL);
        }

        if (firstQuestId && !sObjectMgr.GetQuestTemplate(firstQuestId))
        {
            sLog.outErrorDb("First quest %u specified for dungeon %u in table `lfg_dungeon_rewards` does not exist!", firstQuestId, dungeonId);
            firstQuestId = 0;
        }

        if (otherQuestId && !sObjectMgr.GetQuestTemplate(otherQuestId))
        {
            sLog.outErrorDb("Other quest %u specified for dungeon %u in table `lfg_dungeon_rewards` does not exist!", otherQuestId, dungeonId);
            otherQuestId = 0;
        }

        m_RewardMap.insert(LfgRewardMap::value_type(dungeonId, new LfgReward(maxLevel, firstQuestId, firstMoneyVar, firstXPVar, otherQuestId, otherMoneyVar, otherXPVar)));
        ++count;
    } while (result->NextRow());

    sLog.outString();
    sLog.outString(">> Loaded %u lfg dungeon rewards.", count);
}

// Temporal add to try to find bugs that leaves data inconsistent
void LFGMgr::Cleaner()
{
    LfgQueueInfoMap::iterator itQueue;
    LfgGuidList::iterator itGuidListRemove;
    LfgGuidList eraseList;

    for (LfgQueueInfoMap::iterator it = m_QueueInfoMap.begin(); it != m_QueueInfoMap.end();)
    {
        itQueue = it++;
        // Remove empty queues
        if (!itQueue->second)
        {
            sLog.outError("LFGMgr::Cleaner: removing " UI64FMTD " from QueueInfoMap, data is null", itQueue->first);
            m_QueueInfoMap.erase(itQueue);
        }
        // Remove queue with empty players
        else if(!itQueue->second->roles.size())
        {
            sLog.outError("LFGMgr::Cleaner: removing " UI64FMTD " from QueueInfoMap, no players in queue!", itQueue->first);
            m_QueueInfoMap.erase(itQueue);
        }
    }

    // Remove from NewToQueue those guids that do not exist in queueMap
    for (LfgGuidList::iterator it = m_newToQueue.begin(); it != m_newToQueue.end();)
    {
        itGuidListRemove = it++;
        if (m_QueueInfoMap.find(*itGuidListRemove) == m_QueueInfoMap.end())
        {
            eraseList.push_back(*itGuidListRemove);
            m_newToQueue.erase(itGuidListRemove);
            sLog.outError("LFGMgr::Cleaner: removing " UI64FMTD " from newToQueue, no queue info with that guid", *itGuidListRemove);
        }
    }

    // Remove from currentQueue those guids that do not exist in queueMap
    for (LfgGuidList::iterator it = m_currentQueue.begin(); it != m_currentQueue.end();)
    {
        itGuidListRemove = it++;
        if (m_QueueInfoMap.find(*itGuidListRemove) == m_QueueInfoMap.end())
        {
            eraseList.push_back(*itGuidListRemove);
            m_newToQueue.erase(itGuidListRemove);
            sLog.outError("LFGMgr::Cleaner: removing " UI64FMTD " from currentQueue, no queue info with that guid", *itGuidListRemove);
        }
    }

    for (LfgGuidList::iterator it = eraseList.begin(); it != eraseList.end(); ++it)
    {
        if (IS_GROUP(*it))
        {
            if (Group *grp = sObjectMgr.GetGroupByGUID(GUID_LOPART(*it)))
                for (GroupReference *itr = grp->GetFirstMember(); itr != NULL; itr = itr->next())
                    if (Player *plr = itr->getSource())
                        plr->GetSession()->SendLfgUpdateParty(LFG_UPDATETYPE_REMOVED_FROM_QUEUE);
        }
        else
            if (Player *plr = sObjectMgr.GetPlayer(*it))
                plr->GetSession()->SendLfgUpdatePlayer(LFG_UPDATETYPE_REMOVED_FROM_QUEUE);
    }
}

void LFGMgr::Update(uint32 diff)
{
    if (!m_update || !sWorld.getBoolConfig(CONFIG_DUNGEON_FINDER_ENABLE))
        return;

    m_update = false;
    time_t currTime = time(NULL);

    // Remove obsolete role checks
    LfgRoleCheckMap::iterator itRoleCheck;
    LfgRoleCheck *pRoleCheck;
    for (LfgRoleCheckMap::iterator it = m_RoleChecks.begin(); it != m_RoleChecks.end();)
    {
        itRoleCheck = it++;
        pRoleCheck = itRoleCheck->second;
        if (currTime < pRoleCheck->cancelTime)
            continue;
        pRoleCheck->result = LFG_ROLECHECK_MISSING_ROLE;

        WorldPacket data(SMSG_LFG_ROLE_CHECK_UPDATE, 4 + 1 + 1 + pRoleCheck->dungeons.size() * 4 + 1 + pRoleCheck->roles.size() * (8 + 1 + 4 + 1));
        sLog.outDebug("SMSG_LFG_ROLE_CHECK_UPDATE");
        BuildLfgRoleCheck(data, pRoleCheck);
        Player *plr = NULL;
        for (LfgRolesMap::const_iterator itRoles = pRoleCheck->roles.begin(); itRoles != pRoleCheck->roles.end(); ++itRoles)
        {
            plr = sObjectMgr.GetPlayer(itRoles->first);
            if (!plr)
                continue;
            plr->GetSession()->SendPacket(&data);
            plr->GetLfgDungeons()->clear();
            plr->SetLfgRoles(ROLE_NONE);

            if (itRoles->first == pRoleCheck->leader)
                plr->GetSession()->SendLfgJoinResult(LFG_JOIN_FAILED, pRoleCheck->result);
        }
        delete pRoleCheck;
        m_RoleChecks.erase(itRoleCheck);
    }

    // Remove obsolete proposals
    LfgProposalMap::iterator itRemove;
    for (LfgProposalMap::iterator it = m_Proposals.begin(); it != m_Proposals.end();)
    {
        itRemove = it++;
        if (itRemove->second->cancelTime < currTime)
            RemoveProposal(itRemove, LFG_UPDATETYPE_PROPOSAL_FAILED);
    }

    // Remove obsolete kicks
    LfgPlayerBootMap::iterator itBoot;
    LfgPlayerBoot *pBoot;
    for (LfgPlayerBootMap::iterator it = m_Boots.begin(); it != m_Boots.end();)
    {
        itBoot = it++;
        pBoot = itBoot->second;
        if (pBoot->cancelTime < currTime)
        {
            Group *grp = sObjectMgr.GetGroupByGUID(itBoot->first);
            pBoot->inProgress = false;
            for (LfgAnswerMap::const_iterator itVotes = pBoot->votes.begin(); itVotes != pBoot->votes.end(); ++itVotes)
                if (Player *plrg = sObjectMgr.GetPlayer(itVotes->first))
                    if (plrg->GetGUIDLow() != pBoot->victimLowGuid)
                        plrg->GetSession()->SendLfgBootPlayer(pBoot);
            if (grp)
                grp->SetLfgKickActive(false);
            delete pBoot;
            m_Boots.erase(itBoot);
        }
    }

    // Consistency cleaner
    Cleaner();

    // Check if a proposal can be formed with the new groups being added
    LfgProposalList proposals;
    LfgGuidList firstNew;
    while (!m_newToQueue.empty())
    {
        firstNew.push_back(m_newToQueue.front());
        if (IS_GROUP(firstNew.front()))
            CheckCompatibility(firstNew, &proposals);       // Check if the group itself match
        if (!proposals.size())
            FindNewGroups(firstNew, m_currentQueue, &proposals);

        if (proposals.size())                               // Group found!
        {
            LfgProposal *pProposal = *proposals.begin();
            // TODO: Create algorithm to select better group based on GS (uses to be good tank with bad healer and viceversa)

            // Remove groups in the proposal from new and current queues (not from queue map)
            for (LfgGuidList::const_iterator it = pProposal->queues.begin(); it != pProposal->queues.end(); ++it)
            {
                m_currentQueue.remove(*it);
                m_newToQueue.remove(*it);
            }
            m_Proposals[++m_lfgProposalId] = pProposal;

            uint32 lowGuid = 0;
            for (LfgProposalPlayerMap::const_iterator itPlayers = pProposal->players.begin(); itPlayers != pProposal->players.end(); ++itPlayers)
            {
                lowGuid = itPlayers->first;
                if (Player *plr = sObjectMgr.GetPlayer(itPlayers->first))
                {
                    if (plr->GetGroup())
                        plr->GetSession()->SendLfgUpdateParty(LFG_UPDATETYPE_PROPOSAL_BEGIN);
                    else
                        plr->GetSession()->SendLfgUpdatePlayer(LFG_UPDATETYPE_PROPOSAL_BEGIN);
                    plr->GetSession()->SendUpdateProposal(m_lfgProposalId, pProposal);
                }
            }

            if (pProposal->state == LFG_PROPOSAL_SUCCESS)
                UpdateProposal(m_lfgProposalId, lowGuid, true);

            // Clean up
            for (LfgProposalList::iterator it = proposals.begin(); it != proposals.end(); ++it)
            {
                if ((*it) == pProposal)                     // Do not remove the selected proposal;
                    continue;
                (*it)->queues.clear();
                for (LfgProposalPlayerMap::iterator itPlayers = (*it)->players.begin(); itPlayers != (*it)->players.end(); ++itPlayers)
                    delete itPlayers->second;
                (*it)->players.clear();
                delete (*it);
            }
            proposals.clear();
        }
        else
        {
            m_currentQueue.push_back(m_newToQueue.front()); // Group not found, add this group to the queue.
            m_newToQueue.pop_front();
        }
        firstNew.clear();
    }

    // Update all players status queue info
    if (m_QueueTimer > LFG_QUEUEUPDATE_INTERVAL)
    {
        m_QueueTimer = 0;
        time_t currTime = time(NULL);
        int32 waitTime;
        LfgQueueInfo *queue;
        uint32 dungeonId;
        uint32 queuedTime;
        uint8 role;
        for (LfgQueueInfoMap::const_iterator itQueue = m_QueueInfoMap.begin(); itQueue != m_QueueInfoMap.end(); ++itQueue)
        {
            queue = itQueue->second;
            if (!queue)
            {
                sLog.outError("LFGMgr::Update: %s (lowguid: %u) queued with null queue info!", IS_GROUP(itQueue->first) ? "group" : "player", GUID_LOPART(itQueue->first));
                continue;
            }
            dungeonId = *queue->dungeons.begin();
            queuedTime = uint32(currTime - queue->joinTime);
            role = ROLE_NONE;
            for (LfgRolesMap::const_iterator itPlayer = queue->roles.begin(); itPlayer != queue->roles.end(); ++itPlayer)
                role |= itPlayer->second;

            waitTime = -1;
            if (role & ROLE_TANK)
            {
                if (role & ROLE_HEALER || role & ROLE_DAMAGE)
                    waitTime = m_WaitTimeAvg;
                else
                    waitTime = m_WaitTimeTank;
            }
            else if (role & ROLE_HEALER)
            {
                if (role & ROLE_DAMAGE)
                    waitTime = m_WaitTimeAvg;
                else
                    waitTime = m_WaitTimeHealer;
            }
            else if (role & ROLE_DAMAGE)
                waitTime = m_WaitTimeDps;

            for (LfgRolesMap::const_iterator itPlayer = queue->roles.begin(); itPlayer != queue->roles.end(); ++itPlayer)
                if (Player * plr = sObjectMgr.GetPlayer(itPlayer->first))
                    plr->GetSession()->SendLfgQueueStatus(dungeonId, waitTime, m_WaitTimeAvg, m_WaitTimeTank, m_WaitTimeHealer, m_WaitTimeDps, queuedTime, queue->tanks, queue->healers, queue->dps);
        }
    }
    else
        m_QueueTimer += diff;
    m_update = true;
}

/// <summary>
/// Add a guid to new queue, checks consistency
/// </summary>
/// <param name="uint64">Player or group guid</param>
void LFGMgr::AddGuidToNewQueue(uint64 guid)
{
    // Consistency check
    LfgGuidList::const_iterator it;
    for (it = m_newToQueue.begin(); it != m_newToQueue.end(); ++it)
    {
        if (*it == guid)
        {
            sLog.outError("LFGMgr::AddGuidToNewQueue: " UI64FMTD " being added to queue and it was already added. ignoring", guid);
            break;
        }
    }
    if (it == m_newToQueue.end())
    {
        LfgGuidList::iterator itRemove;
        for (LfgGuidList::iterator it = m_currentQueue.begin(); it != m_currentQueue.end() && *it != guid;)
        {
            itRemove = it++;
            if (*itRemove == guid)
            {
                sLog.outError("LFGMgr::AddGuidToNewQueue: " UI64FMTD " being added to queue and already in current queue (removing to readd)", guid);
                m_currentQueue.erase(itRemove);
                break;
            }
        }
        // Add to queue
        m_newToQueue.push_back(guid);
        sLog.outDebug("LFGMgr::AddGuidToNewQueue: %u added to m_newToQueue (size: %u)", GUID_LOPART(guid), m_newToQueue.size());
    }
}

/// <summary>
/// Creates a QueueInfo and adds it to the queue. Tries to match a group before joining.
/// </summary>
/// <param name="uint64">Player or group guid</param>
/// <param name="LfgRolesMap *">Player roles</param>
/// <param name="LfgDungeonSet *">Selected dungeons</param>
void LFGMgr::AddToQueue(uint64 guid, LfgRolesMap *roles, LfgDungeonSet *dungeons)
{
    if (!roles || !roles->size() || !dungeons)
    {
        sLog.outError("LFGMgr::AddToQueue: " UI64FMTD " has no roles or no dungeons", guid);
        return;
    }

    LfgQueueInfo *pqInfo = new LfgQueueInfo();
    pqInfo->joinTime = time_t(time(NULL));
    for (LfgRolesMap::const_iterator it = roles->begin(); it != roles->end(); ++it)
    {
        if (pqInfo->tanks && it->second & ROLE_TANK)
            --pqInfo->tanks;
        else if (pqInfo->healers && it->second & ROLE_HEALER)
            --pqInfo->healers;
        else
            --pqInfo->dps;
    }
    for (LfgRolesMap::const_iterator itRoles = roles->begin(); itRoles != roles->end(); ++itRoles)
        pqInfo->roles[itRoles->first] = itRoles->second;

    
    LfgDungeonSet *expandedDungeons = dungeons;
    if (isRandomDungeon(*dungeons->begin()))                // Expand random dungeons
    {
        LfgDungeonMap dungeonMap;
        dungeonMap[guid] = GetDungeonsByRandom(*dungeons->begin());
        PlayerSet players;
        Player *plr;
        for (LfgRolesMap::const_iterator it = roles->begin(); it != roles->end(); ++it)
        {
            plr = sObjectMgr.GetPlayer(it->first);
            if (plr)
                players.insert(plr);
        }

        if (players.size() != roles->size())
        {
            sLog.outError("LFGMgr::AddToQueue: " UI64FMTD " has players %d offline. Can't join queue", guid, uint8(roles->size() - players.size()));
            pqInfo->dungeons.clear();
            pqInfo->roles.clear();
            players.clear();
            delete pqInfo;
            return;
        }
        // Check restrictions
        expandedDungeons = CheckCompatibleDungeons(&dungeonMap, &players);
        players.clear();
    }

    for (LfgDungeonSet::const_iterator it = expandedDungeons->begin(); it != expandedDungeons->end(); ++it)
        pqInfo->dungeons.insert(*it);

    sLog.outDebug("LFGMgr::AddToQueue: " UI64FMTD " joining with %u members", guid, pqInfo->roles.size());
    m_QueueInfoMap[guid] = pqInfo;
    AddGuidToNewQueue(guid);
}

/// <summary>
/// Removes the player/group from all queues
/// </summary>
/// <param name="uint64">Player or group guid</param>
/// <returns>bool</returns>
bool LFGMgr::RemoveFromQueue(uint64 guid)
{
    bool ret = false;
    uint32 before = m_QueueInfoMap.size();

    m_currentQueue.remove(guid);
    m_newToQueue.remove(guid);
    RemoveFromCompatibles(guid);

    LfgQueueInfoMap::iterator it = m_QueueInfoMap.find(guid);
    if (it != m_QueueInfoMap.end())
    {
        delete it->second;
        m_QueueInfoMap.erase(it);
        ret = true;
    }
    sLog.outDebug("LFGMgr::RemoveFromQueue: " UI64FMTD " %s - Queue(%u)", guid, before != m_QueueInfoMap.size() ? "Removed": "Not in queue", m_QueueInfoMap.size());
    return ret;
}

/// <summary>
/// Adds the player/group to lfg queue
/// </summary>
/// <param name="Player *">Player</param>
void LFGMgr::Join(Player *plr)
{
    Group *grp = plr->GetGroup();

    if (grp && grp->GetLeaderGUID() != plr->GetGUID())
        return;

    uint64 guid = grp ? grp->GetGUID() : plr->GetGUID();

    sLog.outDebug("LFGMgr::Join: %u joining with %u members", GUID_LOPART(guid), grp ? grp->GetMembersCount() : 1);
    LfgJoinResult result = LFG_JOIN_OK;
    // Previous checks before joining
    LfgQueueInfoMap::iterator itQueue = m_QueueInfoMap.find(guid);
    if (itQueue != m_QueueInfoMap.end())
    {
        result = LFG_JOIN_INTERNAL_ERROR;
        sLog.outError("LFGMgr::Join: %s (lowguid: %u) trying to join but is already in queue!", grp ? "group" : "player", GUID_LOPART(guid));
    }
    else if (plr->InBattleground() || plr->InArena())
        result = LFG_JOIN_USING_BG_SYSTEM;
    else if (plr->HasAura(LFG_SPELL_DESERTER))
        result = LFG_JOIN_DESERTER;
    else if (plr->HasAura(LFG_SPELL_COOLDOWN))
        result = LFG_JOIN_RANDOM_COOLDOWN;
    else
    {
        // Check if all dungeons are valid
        for (LfgDungeonSet::const_iterator it = plr->GetLfgDungeons()->begin(); it != plr->GetLfgDungeons()->end(); ++it)
        {
            if (!GetDungeonGroupType(*it))
            {
                result = LFG_JOIN_DUNGEON_INVALID;
                break;
            }
        }
    }

    // Group checks
    if (grp && result == LFG_JOIN_OK)
    {
        if (grp->GetMembersCount() > MAXGROUPSIZE)
            result = LFG_JOIN_TOO_MUCH_MEMBERS;
        else
        {
            Player *plrg;
            uint8 memberCount = 0;
            for (GroupReference *itr = grp->GetFirstMember(); itr != NULL && result == LFG_JOIN_OK; itr = itr->next())
            {
                plrg = itr->getSource();
                if (plrg)
                {
                    if (plrg->HasAura(LFG_SPELL_DESERTER))
                        result = LFG_JOIN_PARTY_DESERTER;
                    else if (plrg->HasAura(LFG_SPELL_COOLDOWN))
                        result = LFG_JOIN_PARTY_RANDOM_COOLDOWN;
                    ++memberCount;
                }
            }
            if (memberCount != grp->GetMembersCount())
                result = LFG_JOIN_DISCONNECTED;
        }
    }

    if (result != LFG_JOIN_OK)                              // Someone can't join. Clear all stuf
    {
        plr->GetLfgDungeons()->clear();
        plr->SetLfgRoles(ROLE_NONE);
        plr->GetSession()->SendLfgJoinResult(result, 0);
        plr->GetSession()->SendLfgUpdateParty(LFG_UPDATETYPE_ROLECHECK_FAILED);
        return;
    }

    if (grp)
    {
        Player *plrg = NULL;
        LfgDungeonSet *dungeons;
        for (GroupReference *itr = plr->GetGroup()->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            plrg = itr->getSource();                        // Not null, checked earlier
            if (plrg != plr)
            {
                dungeons = plrg->GetLfgDungeons();
                dungeons->clear();
                for (LfgDungeonSet::const_iterator itDungeon = plr->GetLfgDungeons()->begin(); itDungeon != plr->GetLfgDungeons()->end(); ++itDungeon)
                    dungeons->insert(*itDungeon);
            }
            plrg->GetSession()->SendLfgUpdateParty(LFG_UPDATETYPE_JOIN_PROPOSAL);
        }
        UpdateRoleCheck(grp, plr);
    }
    else
    {
        plr->GetSession()->SendLfgJoinResult(LFG_JOIN_OK, 0);
        plr->GetSession()->SendLfgUpdatePlayer(LFG_UPDATETYPE_JOIN_PROPOSAL);
        LfgRolesMap roles;
        roles[plr->GetGUIDLow()] = plr->GetLfgRoles();
        AddToQueue(plr->GetGUID(), &roles, plr->GetLfgDungeons());
        roles.clear();
    }
}

/// <summary>
/// Leave the lfg queue
/// </summary>
/// <param name="Player *">Player (could be NULL)</param>
/// <param name="Group *">Group (could be NULL)</param>
void LFGMgr::Leave(Player *plr, Group *grp )
{
    if (plr && !plr->GetLfgUpdate())
        return;

    uint64 guid = grp ? grp->GetGUID() : plr ? plr->GetGUID() : 0;

    // Remove from Role Checks
    if (grp)
    {
        grp->SetLfgQueued(false);
        LfgRoleCheckMap::const_iterator itRoleCheck = m_RoleChecks.find(GUID_LOPART(guid));
        if (itRoleCheck != m_RoleChecks.end())
        {
            UpdateRoleCheck(grp);                           // No player to update role = LFG_ROLECHECK_ABORTED
            return;
        }
    }

    // Remove from queue
    RemoveFromQueue(guid);

    // Remove from Proposals
    for (LfgProposalMap::iterator it = m_Proposals.begin(); it != m_Proposals.end(); ++it)
    {
        // Mark the player/leader of group who left as didn't accept the proposal
        for (LfgProposalPlayerMap::iterator itPlayer = it->second->players.begin(); itPlayer != it->second->players.end(); ++itPlayer)
            if ((plr && itPlayer->first == plr->GetGUIDLow()) || (grp && itPlayer->first == GUID_LOPART(grp->GetLeaderGUID())))
                itPlayer->second->accept = LFG_ANSWER_DENY;
    }

    if (grp)
    {
        for (GroupReference *itr = grp->GetFirstMember(); itr != NULL; itr = itr->next())
            if (Player *plrg = itr->getSource())
            {
                plrg->GetSession()->SendLfgUpdateParty(LFG_UPDATETYPE_REMOVED_FROM_QUEUE);
                plrg->GetLfgDungeons()->clear();
                plrg->SetLfgRoles(ROLE_NONE);
            }
    }
    else
    {
        plr->GetSession()->SendLfgUpdatePlayer(LFG_UPDATETYPE_REMOVED_FROM_QUEUE);
        plr->GetLfgDungeons()->clear();
        plr->SetLfgRoles(ROLE_NONE);
    }
}

/// <summary>
/// Given a Lfg group checks if leader needs to be show the popup to select more players
/// </summary>
/// <param name="Group *">Group than needs new players</param>
void LFGMgr::OfferContinue(Group *grp)
{
    if (grp && grp->GetLfgStatus() != LFG_STATUS_COMPLETE)
        if (Player *leader = sObjectMgr.GetPlayer(grp->GetLeaderGUID()))
            leader->GetSession()->SendLfgOfferContinue(grp->GetLfgDungeonEntry(false));
}

/// <summary>
/// Check the queue to try to match groups. Returns all the posible matches
/// </summary>
/// <param name="LfgGuidList &">Guids we trying to match with the rest of groups</param>
/// <param name="LfgGuidList">All guids in queue</param>
/// <param name="LfgProposalList *">Proposals found.</param>
void LFGMgr::FindNewGroups(LfgGuidList &check, LfgGuidList all, LfgProposalList *proposals)
{
    ASSERT(proposals);
    if (!check.size() || check.size() > MAXGROUPSIZE)
        return;

    sLog.outDebug("LFGMgr::FindNewGroup: (%s) - all(%s)", ConcatenateGuids(check).c_str(), ConcatenateGuids(all).c_str());
    LfgGuidList compatibles;
    // Check individual compatibilities
    for (LfgGuidList::iterator it = all.begin(); it != all.end(); ++it)
    {
        check.push_back(*it);
        if (CheckCompatibility(check, proposals))
            compatibles.push_back(*it);
        check.pop_back();
    }

    while (compatibles.size() > 1)
    {
        check.push_back(compatibles.front());
        compatibles.pop_front();
        FindNewGroups(check, compatibles, proposals);
        check.pop_back();
    }
}

/// <summary>
/// Check compatibilities between groups.
/// </summary>
/// <param name="LfgGuidList &">Guids we checking compatibility</param>
/// <returns>bool</returns>
/// <param name="LfgProposalList *">Proposals found.</param>
bool LFGMgr::CheckCompatibility(LfgGuidList check, LfgProposalList *proposals)
{
    std::string strGuids = ConcatenateGuids(check);

    if (check.size() > MAXGROUPSIZE || !check.size())
    {
        sLog.outDebug("LFGMgr::CheckCompatibility: (%s): Size wrong - Not compatibles", strGuids.c_str());
        return false;
    }
   
    // No previous check have been done, do it now
    uint8 numPlayers = 0;
    uint8 numLfgGroups = 0;
    uint32 groupLowGuid = 0;
    LfgQueueInfoMap pqInfoMap;
    LfgQueueInfoMap::iterator itQueue;
    for (LfgGuidList::const_iterator it = check.begin(); it != check.end() && numLfgGroups < 2 && numPlayers <= MAXGROUPSIZE; ++it)
    {
        itQueue = m_QueueInfoMap.find(*it);
        if (itQueue == m_QueueInfoMap.end())
        {
            sLog.outError("LFGMgr::CheckCompatibility: " UI64FMTD " is not queued but listed as queued!", *it);
            return false;
        }
        pqInfoMap[*it] = itQueue->second;
        numPlayers += itQueue->second->roles.size();

        if (IS_GROUP(*it))
        {
            uint32 lowGuid = GUID_LOPART(*it);
            if (Group *grp = sObjectMgr.GetGroupByGUID(lowGuid))
                if (grp->isLFGGroup())
                {
                    if (!numLfgGroups)
                        groupLowGuid = lowGuid;
                    ++numLfgGroups;
                }
        }
    }

    if (check.size() == 1 && numPlayers != MAXGROUPSIZE) // Single group with less than MAXGROUPSIZE - Compatibles
        return true;

    if (check.size() > 1)
    {
        // Previously cached?
        LfgAnswer answer = GetCompatibles(strGuids);
        if (answer != LFG_ANSWER_PENDING)
        {
            if (numPlayers != MAXGROUPSIZE || answer == LFG_ANSWER_DENY)
            {
                sLog.outDebug("LFGMgr::CheckCompatibility: (%s) compatibles (cached): %d", strGuids.c_str(), answer);
                return bool(answer);
            }
            // MAXGROUPSIZE + LFG_ANSWER_AGREE = Match - we don't have it cached so do calcs again
        }
        else if (check.size() > 2)
        {
            uint64 frontGuid = check.front();
            check.pop_front();

            // Check all-but-new compatibilities (New,A,B,C,D) --> check(A,B,C,D)
            if (!CheckCompatibility(check, proposals))      // Group not compatible
            {
                sLog.outDebug("LFGMgr::CheckCompatibility: (%s) no compatibles (%s not compatibles)", strGuids.c_str(), ConcatenateGuids(check).c_str());
                SetCompatibles(strGuids, false);
                return false;
            }
            check.push_front(frontGuid);
            // all-but-new compatibles, now check with new
        }
    }

    // Do not match - groups already in a lfgDungeon or too much players
    if (numLfgGroups > 1 || numPlayers > MAXGROUPSIZE)
    {
        pqInfoMap.clear();
        SetCompatibles(strGuids, false);
        if (numLfgGroups > 1)
            sLog.outDebug("LFGMgr::CheckCompatibility: (%s) More than one Lfggroup (%u)", strGuids.c_str(), numLfgGroups);
        else
            sLog.outDebug("LFGMgr::CheckCompatibility: (%s) Too much players (%u)", strGuids.c_str(), numPlayers);
        return false;
    }

    // ----- Player checks -----
    LfgRolesMap rolesMap;
    uint32 newLeaderLowGuid = 0;
    for (LfgQueueInfoMap::const_iterator it = pqInfoMap.begin(); it != pqInfoMap.end(); ++it)
    {
        for (LfgRolesMap::const_iterator itRoles = it->second->roles.begin(); itRoles != it->second->roles.end(); ++itRoles)
        {
            // Assign new leader
            if (itRoles->second & ROLE_LEADER && (!newLeaderLowGuid || urand(0, 1)))
                newLeaderLowGuid = itRoles->first;
            rolesMap[itRoles->first] = itRoles->second;
        }
    }

    if (rolesMap.size() != numPlayers)
    {
        sLog.outError("LFGMgr::CheckCompatibility: There is a player multiple times in queue.");
        pqInfoMap.clear();
        rolesMap.clear();
        return false;
    }

    Player *plr;
    PlayerSet players;
    for (LfgRolesMap::const_iterator it = rolesMap.begin(); it != rolesMap.end(); ++it)
    {
        plr = sObjectMgr.GetPlayer(it->first);
        if (!plr)
            sLog.outDebug("LFGMgr::CheckCompatibility: (%s) Warning! %u offline!", strGuids.c_str(), it->first);
            
        for (PlayerSet::const_iterator itPlayer = players.begin(); itPlayer != players.end() && plr; ++itPlayer)
        {
            // Do not form a group with ignoring candidates
            if (plr->GetSocial()->HasIgnore((*itPlayer)->GetGUIDLow()) || (*itPlayer)->GetSocial()->HasIgnore(plr->GetGUIDLow()))
                plr = NULL;
            // neither with diferent faction if it's not a mixed faction server
            else if (plr->GetTeam() != (*itPlayer)->GetTeam() && !sWorld.getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_GROUP))
                plr = NULL;
        }
        if (plr)
            players.insert(plr);
    }

    // if we dont have the same ammount of players then we have self ignoring candidates or different faction groups
    // otherwise check if roles are compatible
    if (players.size() != numPlayers || !CheckGroupRoles(rolesMap))
    {
        if (players.size() != numPlayers)
            sLog.outDebug("LFGMgr::CheckCompatibility: (%s) Player offline, ignoring or diff teams", strGuids.c_str());
        else
            sLog.outDebug("LFGMgr::CheckCompatibility: (%s) Roles not compatible", strGuids.c_str());
        pqInfoMap.clear();
        rolesMap.clear();
        players.clear();
        SetCompatibles(strGuids, false);
        return false;
    }

    // ----- Selected Dungeon checks -----
    // Check if there are any compatible dungeon from the selected dungeons
    LfgDungeonMap dungeonMap;
    for (LfgQueueInfoMap::const_iterator it = pqInfoMap.begin(); it != pqInfoMap.end(); ++it)
        dungeonMap[it->first] = &it->second->dungeons;

    LfgDungeonSet *compatibleDungeons = CheckCompatibleDungeons(&dungeonMap, &players);
    dungeonMap.clear();
    pqInfoMap.clear();
    if (!compatibleDungeons || !compatibleDungeons->size())
    {
        if (compatibleDungeons)
            delete compatibleDungeons;
        players.clear();
        rolesMap.clear();
        SetCompatibles(strGuids, false);
        return false;
    }
    SetCompatibles(strGuids, true);

    // ----- Group is compatible, if we have MAXGROUPSIZE members then match is found
    if (numPlayers != MAXGROUPSIZE)
    {
        players.clear();
        rolesMap.clear();
        sLog.outDebug("LFGMgr::CheckCompatibility: (%s) Compatibles but not match. Players(%u)", strGuids.c_str(), numPlayers);
        return true;
    }
    sLog.outDebug("LFGMgr::CheckCompatibility: (%s) MATCH! Group formed", strGuids.c_str());

    // Select a random dungeon from the compatible list
    LfgDungeonSet::iterator itDungeon = compatibleDungeons->begin();
    uint32 selectedDungeon = urand(0, compatibleDungeons->size() - 1);
    while (selectedDungeon > 0)
    {
        ++itDungeon;
        --selectedDungeon;
    }
    selectedDungeon = *itDungeon;
    compatibleDungeons->clear();
    delete compatibleDungeons;

    // Create a new proposal
    LfgProposal *pProposal = new LfgProposal(selectedDungeon);
    pProposal->cancelTime = time_t(time(NULL)) + LFG_TIME_PROPOSAL;
    pProposal->queues = check;
    pProposal->groupLowGuid = groupLowGuid;

    // Assign new roles to players and assign new leader
    LfgProposalPlayer *ppPlayer;
    uint32 lowGuid;
    PlayerSet::const_iterator itPlayers = players.begin();
    if (!newLeaderLowGuid)
    {
        uint8 pos = urand(0, players.size() - 1);
        for (uint8 i = 0; i < pos; ++i)
            ++itPlayers;
        newLeaderLowGuid = (*itPlayers)->GetGUIDLow();
    }
    pProposal->leaderLowGuid = newLeaderLowGuid;

    uint8 numAccept = 0;
    for (itPlayers = players.begin(); itPlayers != players.end(); ++itPlayers)
    {
        lowGuid = (*itPlayers)->GetGUIDLow();
        ppPlayer = new LfgProposalPlayer();
        Group *grp = (*itPlayers)->GetGroup();
        if (grp)
        {
            ppPlayer->groupLowGuid = grp->GetLowGUID();
            if (grp->GetLfgDungeonEntry() == selectedDungeon && ppPlayer->groupLowGuid == pProposal->groupLowGuid) // Player from existing group, autoaccept
            {
                ppPlayer->accept = LFG_ANSWER_AGREE;
                ++numAccept;
            }
        }
        ppPlayer->role = rolesMap[lowGuid];
        pProposal->players[lowGuid] = ppPlayer;
    }
    if (numAccept == MAXGROUPSIZE)
        pProposal->state = LFG_PROPOSAL_SUCCESS;

    if (!proposals)
        proposals = new LfgProposalList();
    proposals->push_back(pProposal);

    rolesMap.clear();
    players.clear();
    return true;
}

/// <summary>
/// Update the Role check info with the player selected role.
/// </summary>
/// <param name="Group *">Group</param>
/// <param name="Player *">Player (optional, default NULL)</param>
void LFGMgr::UpdateRoleCheck(Group *grp, Player *plr )
{
    if (!grp)
        return;

    uint32 rolecheckId = grp->GetLowGUID();
    LfgRoleCheck *pRoleCheck = NULL;
    LfgRolesMap check_roles;
    LfgRoleCheckMap::iterator itRoleCheck = m_RoleChecks.find(rolecheckId);
    LfgDungeonSet *dungeons = plr->GetLfgDungeons();
    bool newRoleCheck = itRoleCheck == m_RoleChecks.end();
    if (newRoleCheck)
    {
        if (grp->GetLeaderGUID() != plr->GetGUID())
            return;

        pRoleCheck = new LfgRoleCheck();
        pRoleCheck->cancelTime = time_t(time(NULL)) + LFG_TIME_ROLECHECK;
        pRoleCheck->result = LFG_ROLECHECK_INITIALITING;
        pRoleCheck->leader = plr->GetGUIDLow();

        for (GroupReference *itr = grp->GetFirstMember(); itr != NULL; itr = itr->next())
            if (Player *plrg = itr->getSource())
                pRoleCheck->roles[plrg->GetGUIDLow()] = 0;

        // Check if it's offer continue or trying to find a new instance after a random assigned (Join Random + LfgGroup)
        if (grp->isLFGGroup() && dungeons->size() == 1 && isRandomDungeon(*dungeons->begin()) && grp->GetLfgDungeonEntry())
            pRoleCheck->dungeons.insert(grp->GetLfgDungeonEntry());
        else
            for (LfgDungeonSet::const_iterator itDungeon = dungeons->begin(); itDungeon != dungeons->end(); ++itDungeon)
                pRoleCheck->dungeons.insert(*itDungeon);
    }
    else
        pRoleCheck = itRoleCheck->second;

    LfgLockStatusMap *playersLockMap = NULL;
    if (plr)
    {
        // Player selected no role.
        if (plr->GetLfgRoles() < ROLE_TANK)
            pRoleCheck->result = LFG_ROLECHECK_NO_ROLE;
        else
        {
            // Check if all players have selected a role
            pRoleCheck->roles[plr->GetGUIDLow()] = plr->GetLfgRoles();
            uint8 size = 0;
            for (LfgRolesMap::const_iterator itRoles = pRoleCheck->roles.begin(); itRoles != pRoleCheck->roles.end() && itRoles->second != ROLE_NONE; ++itRoles)
                ++size;

            if (pRoleCheck->roles.size() == size)
            {
                // use temporal var to check roles, CheckGroupRoles modifies the roles
                check_roles = pRoleCheck->roles;
                if (!CheckGroupRoles(check_roles))              // Group is not posible
                    pRoleCheck->result = LFG_ROLECHECK_WRONG_ROLES;
                else
                {
                    // Check if we can find a dungeon for that group
                    pRoleCheck->result = LFG_ROLECHECK_FINISHED;
                    if (pRoleCheck->dungeons.size() > 1)
                        playersLockMap = GetPartyLockStatusDungeons(plr, &pRoleCheck->dungeons);
                    else
                    {
                        LfgDungeonSet::const_iterator it = pRoleCheck->dungeons.begin();
                        LFGDungeonEntry const *dungeon = sLFGDungeonStore.LookupEntry(*it);
                        if (dungeon && dungeon->type == LFG_TYPE_RANDOM)
                            playersLockMap = GetPartyLockStatusDungeons(plr, GetDungeonsByRandom(*it));
                        else
                            playersLockMap = GetPartyLockStatusDungeons(plr, &pRoleCheck->dungeons);
                    }
                }
            }
        }
    }
    else
        pRoleCheck->result = LFG_ROLECHECK_ABORTED;

    WorldSession *session;
    WorldPacket data(SMSG_LFG_ROLE_CHECK_UPDATE, 4 + 1 + 1 + pRoleCheck->dungeons.size() * 4 + 1 + pRoleCheck->roles.size() * (8 + 1 + 4 + 1));
    sLog.outDebug("SMSG_LFG_ROLE_CHECK_UPDATE");
    BuildLfgRoleCheck(data, pRoleCheck);

    Player *plrg = NULL;
    for (GroupReference *itr = grp->GetFirstMember(); itr != NULL; itr = itr->next())
    {
        plrg = itr->getSource();
        if (!plrg)
            continue;

        session = plrg->GetSession();
        if (!newRoleCheck && plr)
            session->SendLfgRoleChosen(plr->GetGUID(), plr->GetLfgRoles());
        session->SendPacket(&data);

        switch(pRoleCheck->result)
        {
        case LFG_ROLECHECK_INITIALITING:
            continue;
        case LFG_ROLECHECK_FINISHED:
            if (!playersLockMap)
                session->SendLfgUpdateParty(LFG_UPDATETYPE_ADDED_TO_QUEUE);
            else
            {
                if (grp->GetLeaderGUID() == plrg->GetGUID())
                {
                    uint32 size = 0;
                    for (LfgLockStatusMap::const_iterator it = playersLockMap->begin(); it != playersLockMap->end(); ++it)
                        size += 8 + 4 + it->second->size() * (4 + 4);
                    WorldPacket data(SMSG_LFG_JOIN_RESULT, 4 + 4 + size);
                    sLog.outDebug("SMSG_LFG_JOIN_RESULT");
                    data << uint32(LFG_JOIN_PARTY_NOT_MEET_REQS); // Check Result
                    data << uint32(0);                      // Check Value (always 0 when PartyNotMeetReqs
                    BuildPartyLockDungeonBlock(data, playersLockMap);
                    session->SendPacket(&data);
                }
                session->SendLfgUpdateParty(LFG_UPDATETYPE_ROLECHECK_FAILED);
                plrg->GetLfgDungeons()->clear();
                plrg->SetLfgRoles(ROLE_NONE);
            }
            break;
        default:
            if (grp->GetLeaderGUID() == plrg->GetGUID())
                session->SendLfgJoinResult(LFG_JOIN_FAILED, pRoleCheck->result);
            session->SendLfgUpdateParty(LFG_UPDATETYPE_ROLECHECK_FAILED);
            plrg->GetLfgDungeons()->clear();
            plrg->SetLfgRoles(ROLE_NONE);
            break;
        }
    }

    if (pRoleCheck->result == LFG_ROLECHECK_FINISHED)
    {
        grp->SetLfgQueued(true);
        AddToQueue(grp->GetGUID(), &pRoleCheck->roles, &pRoleCheck->dungeons);
    }

    if (pRoleCheck->result != LFG_ROLECHECK_INITIALITING)
    {
        pRoleCheck->dungeons.clear();
        pRoleCheck->roles.clear();
        delete pRoleCheck;
        if (!newRoleCheck)
            m_RoleChecks.erase(itRoleCheck);
    }
    else if (newRoleCheck)
        m_RoleChecks[rolecheckId] = pRoleCheck;
}

/// <summary>
/// Remove from cached compatible dungeons any entry that contains the given guid
/// </summary>
/// <param name="uint64">guid to remove from any list</param>
void LFGMgr::RemoveFromCompatibles(uint64 guid)
{
    LfgGuidList lista;
    lista.push_back(guid);
    std::string strGuid = ConcatenateGuids(lista);
    lista.clear();
    
    LfgCompatibleMap::iterator it;
    for (LfgCompatibleMap::iterator itNext = m_CompatibleMap.begin(); itNext != m_CompatibleMap.end();)
    {
        it = itNext++;
        if (it->first.find(strGuid) != std::string::npos)    // Found, remove it
        {
            sLog.outDebug("LFGMgr::RemoveFromCompatibles: Removing " UI64FMTD " from (%s)", guid, it->first.c_str());
            m_CompatibleMap.erase(it);
        }
    }
}

/// <summary>
/// Set the compatibility of a list of guids
/// </summary>
/// <param name="std::string">list of guids concatenated by |</param>
/// <param name="bool">compatibles or not</param>
void LFGMgr::SetCompatibles(std::string key, bool compatibles)
{
    sLog.outDebug("LFGMgr::SetCompatibles: (%s): %d", key.c_str(), LfgAnswer(compatibles));
    m_CompatibleMap[key] = LfgAnswer(compatibles);
}

/// <summary>
/// Get the compatible dungeons between two groups from cache
/// </summary>
/// <param name="std::string">list of guids concatenated by |</param>
/// <returns>LfgAnswer, 
LfgAnswer LFGMgr::GetCompatibles(std::string key)
{
    LfgAnswer answer = LFG_ANSWER_PENDING;
    LfgCompatibleMap::iterator it = m_CompatibleMap.find(key);
    if (it != m_CompatibleMap.end())
        answer = it->second;

    sLog.outDebug("LFGMgr::GetCompatibles: (%s): %d", key.c_str(), answer);
    return answer;
}

/// <summary>
/// Given a list of groups checks the compatible dungeons. If players is not null also check restictions
/// </summary>
/// <param name="LfgDungeonMap *">dungeons to check</param>
/// <param name="PlayerSet *">Players to check restrictions</param>
/// <returns>LfgDungeonSet*</returns>
LfgDungeonSet* LFGMgr::CheckCompatibleDungeons(LfgDungeonMap *dungeonsMap, PlayerSet *players)
{
    if (!dungeonsMap || dungeonsMap->empty())
        return NULL;

    LfgDungeonMap::const_iterator itMap = ++dungeonsMap->begin();
    LfgDungeonSet *compatibleDungeons = new LfgDungeonSet();
    
    bool compatibleDungeon;

    // Get the first group and compare with the others to select all common dungeons
    for (LfgDungeonSet::const_iterator itDungeon = dungeonsMap->begin()->second->begin(); itDungeon != dungeonsMap->begin()->second->end(); ++itDungeon)
    {
        compatibleDungeon = true;
        for (LfgDungeonMap::const_iterator it = itMap; it != dungeonsMap->end() && compatibleDungeon; ++it)
            if (it->second->find(*itDungeon) == it->second->end())
                compatibleDungeon = false;
        if (compatibleDungeon)
            compatibleDungeons->insert(*itDungeon);
    }

    if (players && !players->empty())
    {
        // now remove those with restrictions
        LfgLockStatusMap *pLockDungeons = GetGroupLockStatusDungeons(players, compatibleDungeons, false);
        if (pLockDungeons) // Found dungeons not compatible, remove them from the set
        {
            LfgLockStatusSet *pLockSet = NULL;
            LfgDungeonSet::iterator itDungeon;
            for (LfgLockStatusMap::const_iterator itLockMap = pLockDungeons->begin(); itLockMap != pLockDungeons->end() && compatibleDungeons->size(); ++itLockMap)
            {
                pLockSet = itLockMap->second;
                for(LfgLockStatusSet::const_iterator itLockSet = pLockSet->begin(); itLockSet != pLockSet->end(); ++itLockSet)
                {
                    itDungeon = compatibleDungeons->find((*itLockSet)->dungeon);
                    if (itDungeon != compatibleDungeons->end())
                         compatibleDungeons->erase(itDungeon);
                }
                pLockSet->clear();
                delete pLockSet;
            }
            pLockDungeons->clear();
            delete pLockDungeons;
        }
    }

    // Any compatible dungeon after checking restrictions?
    if (!compatibleDungeons->size())
    {
        delete compatibleDungeons;
        compatibleDungeons = NULL;
    }
    return compatibleDungeons;
}

/// <summary>
/// Check if a group can be formed with the given group
/// </summary>
/// <param name="LfgRolesMap &">Roles to check</param>
/// <param name="bool">Used to remove ROLE_LEADER</param>
/// <returns>bool</returns>
bool LFGMgr::CheckGroupRoles(LfgRolesMap &groles, bool removeLeaderFlag )
{
    if (!groles.size())
        return false;

    uint8 damage = 0;
    uint8 tank = 0;
    uint8 healer = 0;

    if (removeLeaderFlag)
        for (LfgRolesMap::iterator it = groles.begin(); it != groles.end(); ++it)
            it->second &= ~ROLE_LEADER;

    for (LfgRolesMap::iterator it = groles.begin(); it != groles.end(); ++it)
    {
        switch(it->second)
        {
        case ROLE_NONE:
            return false;
        case ROLE_TANK:
            if (tank == LFG_TANKS_NEEDED)
                return false;
            tank++;
            break;
        case ROLE_HEALER:
            if (healer == LFG_HEALERS_NEEDED)
                return false;
            healer++;
            break;
        case ROLE_DAMAGE:
            if (damage == LFG_DPS_NEEDED)
                return false;
            damage++;
            break;
        default:
            if (it->second & ROLE_TANK)
            {
                it->second -= ROLE_TANK;
                if (CheckGroupRoles(groles, false))
                    return true;
                it->second += ROLE_TANK;
            }

            if (it->second & ROLE_HEALER)
            {
                it->second -= ROLE_HEALER;
                if (CheckGroupRoles(groles, false))
                    return true;
                it->second += ROLE_HEALER;
            }

            if (it->second & ROLE_DAMAGE)
            {
                it->second -= ROLE_DAMAGE;
                return CheckGroupRoles(groles, false);
            }
            break;
        }
    }
    return true;
}

/// <summary>
/// Update Proposal info with player answer
/// </summary>
/// <param name="uint32">Id of the proposal</param>
/// <param name="uint32">Player low guid</param>
/// <param name="bool">Player answer</param>
void LFGMgr::UpdateProposal(uint32 proposalId, uint32 lowGuid, bool accept)
{
    // Check if the proposal exists
    LfgProposalMap::iterator itProposal = m_Proposals.find(proposalId);
    if (itProposal == m_Proposals.end())
        return;
    LfgProposal *pProposal = itProposal->second;

    // Check if proposal have the current player
    LfgProposalPlayerMap::iterator itProposalPlayer = pProposal->players.find(lowGuid);
    if (itProposalPlayer == pProposal->players.end())
        return;
    LfgProposalPlayer *ppPlayer = itProposalPlayer->second;

    ppPlayer->accept = LfgAnswer(accept);
    if (!accept)
    {
        RemoveProposal(itProposal, LFG_UPDATETYPE_PROPOSAL_DECLINED);
        return;
    }

    LfgPlayerList players;
    Player *plr;

    // check if all have answered and reorder players (leader first)
    bool allAnswered = true;
    for (LfgProposalPlayerMap::const_iterator itPlayers = pProposal->players.begin(); itPlayers != pProposal->players.end(); ++itPlayers)
    {
        plr = sObjectMgr.GetPlayer(itPlayers->first);

        if (plr)
        {
            if (itPlayers->first == pProposal->leaderLowGuid)
                players.push_front(plr);
            else
                players.push_back(plr);
        }

        if (itPlayers->second->accept != LFG_ANSWER_AGREE)  // No answer (-1) or not accepted (0)
            allAnswered = false;
    }

    if (!allAnswered)
    {
        for (LfgPlayerList::const_iterator it = players.begin(); it != players.end(); ++it)
            (*it)->GetSession()->SendUpdateProposal(proposalId, pProposal);
    }
    else
    {
        bool sendUpdate = pProposal->state != LFG_PROPOSAL_SUCCESS;
        pProposal->state = LFG_PROPOSAL_SUCCESS;
        time_t joinTime = time_t(time(NULL));
        uint8 role = 0;
        int32 waitTime = -1;
        LfgQueueInfoMap::iterator itQueue;
        uint64 guid = 0;

        // Create a new group (if needed)
        Group *grp = sObjectMgr.GetGroupByGUID(pProposal->groupLowGuid);
        for (LfgPlayerList::const_iterator it = players.begin(); it != players.end(); ++it)
        {
            plr = *it;
            if (sendUpdate)
                plr->GetSession()->SendUpdateProposal(proposalId, pProposal);
            plr->SetLfgUpdate(false);
            if (plr->GetGroup())
            {
                guid = plr->GetGroup()->GetGUID();
                plr->GetSession()->SendLfgUpdateParty(LFG_UPDATETYPE_GROUP_FOUND);
                if (plr->GetGroup() != grp)
                    plr->RemoveFromGroup();
            }
            else
            {
                guid = plr->GetGUID();
                plr->GetSession()->SendLfgUpdatePlayer(LFG_UPDATETYPE_GROUP_FOUND);
            }

            if (!grp)
            {
                grp = new Group();
                grp->Create(plr->GetGUID(), plr->GetName());
                grp->ConvertToLFG();
                sObjectMgr.AddGroup(grp);
            }
            else if (plr->GetGroup() != grp)
            {
                grp->SetLfgQueued(false);
                grp->AddMember(plr->GetGUID(), plr->GetName());
            }
            plr->SetLfgUpdate(true);
            // Update timers
            role = plr->GetLfgRoles();
            itQueue = m_QueueInfoMap.find(guid);
            if (itQueue == m_QueueInfoMap.end())
            {
                sLog.outError("LFGMgr::UpdateProposal: Queue info for guid " UI64FMTD " not found!", guid);
                waitTime = -1;
            }
            else
            {
                waitTime = int32(joinTime - itQueue->second->joinTime);

                if (role & ROLE_TANK)
                {
                    if (role & ROLE_HEALER || role & ROLE_DAMAGE)
                        m_WaitTimeAvg = int32((m_WaitTimeAvg * m_NumWaitTimeAvg + waitTime) / ++m_NumWaitTimeAvg);
                    else
                        m_WaitTimeTank = int32((m_WaitTimeTank * m_NumWaitTimeTank + waitTime) / ++m_NumWaitTimeTank);
                }
                else if (role & ROLE_HEALER)
                {
                    if (role & ROLE_DAMAGE)
                        m_WaitTimeAvg = int32((m_WaitTimeAvg * m_NumWaitTimeAvg + waitTime) / ++m_NumWaitTimeAvg);
                    else
                        m_WaitTimeHealer = int32((m_WaitTimeHealer * m_NumWaitTimeHealer + waitTime) / ++m_NumWaitTimeHealer);
                }
                else if (role & ROLE_DAMAGE)
                    m_WaitTimeDps = int32((m_WaitTimeDps * m_NumWaitTimeDps + waitTime) / ++m_NumWaitTimeDps);
            }

            grp->SetLfgRoles(plr->GetGUID(), pProposal->players[plr->GetGUIDLow()]->role);
        }

        // Set the dungeon difficulty
        LFGDungeonEntry const *dungeon = sLFGDungeonStore.LookupEntry(pProposal->dungeonId);
        ASSERT(dungeon);
        grp->SetDungeonDifficulty(Difficulty(dungeon->difficulty));
        grp->SetLfgDungeonEntry(dungeon->Entry());
        grp->SetLfgStatus(LFG_STATUS_NOT_SAVED);
        grp->SendUpdate();

        // Remove players/groups from Queue
        for (LfgGuidList::const_iterator it = pProposal->queues.begin(); it != pProposal->queues.end(); ++it)
            RemoveFromQueue(*it);

        // Teleport Player
        for (LfgPlayerList::const_iterator it = players.begin(); it != players.end(); ++it)
            TeleportPlayer(*it, false);

        for (LfgProposalPlayerMap::const_iterator it = pProposal->players.begin(); it != pProposal->players.end(); ++it)
            delete it->second;
        pProposal->players.clear();
        pProposal->queues.clear();
        delete pProposal;
        m_Proposals.erase(itProposal);
    }
    players.clear();
}

/// <summary>
/// Remove a proposal from the pool, remove the group that didn't accept (if needed) and readd the other members to the queue
/// </summary>
/// <param name="LfgProposalMap::iterator">Proposal to remove</param>
/// <param name="LfgUpdateType">Type of removal (LFG_UPDATETYPE_PROPOSAL_FAILED, LFG_UPDATETYPE_PROPOSAL_DECLINED)</param>
void LFGMgr::RemoveProposal(LfgProposalMap::iterator itProposal, LfgUpdateType type)
{
    Player *plr;
    uint64 guid;
    LfgUpdateType updateType;
    LfgQueueInfoMap::iterator itQueue;
    LfgProposal *pProposal = itProposal->second;
    pProposal->state = LFG_PROPOSAL_FAILED;

    // Mark all people that didn't answered as no accept
    if (type == LFG_UPDATETYPE_PROPOSAL_FAILED)
        for (LfgProposalPlayerMap::const_iterator it = pProposal->players.begin(); it != pProposal->players.end(); ++it)
            if (it->second->accept != LFG_ANSWER_AGREE)
                it->second->accept = LFG_ANSWER_DENY;

    // Inform players
    for (LfgProposalPlayerMap::const_iterator it = pProposal->players.begin(); it != pProposal->players.end(); ++it)
    {
        plr = sObjectMgr.GetPlayer(it->first);
        if (!plr)
            continue;
        guid = plr->GetGroup() ? plr->GetGroup()->GetGUID(): plr->GetGUID();

        plr->GetSession()->SendUpdateProposal(itProposal->first, pProposal);
        // Remove members that didn't accept
        itQueue = m_QueueInfoMap.find(guid);
        if (it->second->accept == LFG_ANSWER_DENY)
        {
            updateType = type;
            plr->GetLfgDungeons()->clear();
            plr->SetLfgRoles(ROLE_NONE);

            if (itQueue != m_QueueInfoMap.end())
                m_QueueInfoMap.erase(itQueue);
            RemoveFromCompatibles(guid);
        }
        else                                                // Readd to queue
        {
            if (itQueue == m_QueueInfoMap.end())            // Can't readd! misssing queue info!
                updateType = LFG_UPDATETYPE_REMOVED_FROM_QUEUE;
            else
            {
                itQueue->second->joinTime = time_t(time(NULL));
                AddGuidToNewQueue(guid);
                updateType = LFG_UPDATETYPE_ADDED_TO_QUEUE;
            }
        }

        if (plr->GetGroup())
            plr->GetSession()->SendLfgUpdateParty(updateType);
        else
            plr->GetSession()->SendLfgUpdatePlayer(updateType);

    }

    for (LfgProposalPlayerMap::const_iterator it = pProposal->players.begin(); it != pProposal->players.end(); ++it)
        delete it->second;
    pProposal->players.clear();
    pProposal->queues.clear();
    delete pProposal;
    m_Proposals.erase(itProposal);
}

/// <summary>
/// Initialize a boot kick vote
/// </summary>
/// <param name="Group *">Group</param>
/// <param name="uint32">Player low guid who inits the vote kick</param>
/// <param name="uint32">Player low guid to be kicked </param>
/// <param name="std::string">kick reason</param>
void LFGMgr::InitBoot(Group *grp, uint32 iLowGuid, uint32 vLowguid, std::string reason)
{
    if (!grp)
        return;

    LfgPlayerBoot *pBoot = new LfgPlayerBoot();
    pBoot->inProgress = true;
    pBoot->cancelTime = time_t(time(NULL)) + LFG_TIME_BOOT;
    pBoot->reason = reason;
    pBoot->victimLowGuid = vLowguid;
    pBoot->votedNeeded = GROUP_LFG_KICK_VOTES_NEEDED;
    PlayerSet players;

    uint32 pLowGuid = 0;
    for (GroupReference *itr = grp->GetFirstMember(); itr != NULL; itr = itr->next())
    {
        if (Player *plrg = itr->getSource())
        {
            pLowGuid = plrg->GetGUIDLow();
            if (pLowGuid == vLowguid)
                pBoot->votes[pLowGuid] = LFG_ANSWER_DENY;   // Victim auto vote NO
            else if (pLowGuid == iLowGuid)
                pBoot->votes[pLowGuid] = LFG_ANSWER_AGREE;  // Kicker auto vote YES
            else
            {
                pBoot->votes[pLowGuid] = LFG_ANSWER_PENDING;// Other members need to vote
                players.insert(plrg);
            }
        }
    }

    for (PlayerSet::const_iterator it = players.begin(); it != players.end(); ++it)
        (*it)->GetSession()->SendLfgBootPlayer(pBoot);

    grp->SetLfgKickActive(true);
    m_Boots[grp->GetLowGUID()] = pBoot;
}

/// <summary>
/// Update Boot info with player answer
/// </summary>
/// <param name="Player *">Player guid</param>
/// <param name="bool">Player answer</param>
void LFGMgr::UpdateBoot(Player *plr, bool accept)
{
    Group *grp = plr ? plr->GetGroup() : NULL;
    if (!grp)
        return;

    uint32 bootId = grp->GetLowGUID();
    uint32 lowGuid = plr->GetGUIDLow();

    LfgPlayerBootMap::iterator itBoot = m_Boots.find(bootId);
    if (itBoot == m_Boots.end())
        return;

    LfgPlayerBoot *pBoot = itBoot->second;
    if (!pBoot)
        return;

    if (pBoot->votes[lowGuid] != LFG_ANSWER_PENDING)        // Cheat check: Player can't vote twice
        return;

    pBoot->votes[lowGuid] = LfgAnswer(accept);

    uint8 votesNum = 0;
    uint8 agreeNum = 0;
    for (LfgAnswerMap::const_iterator itVotes = pBoot->votes.begin(); itVotes != pBoot->votes.end(); ++itVotes)
    {
        if (itVotes->second != LFG_ANSWER_PENDING)
        {
            ++votesNum;
            if (itVotes->second == LFG_ANSWER_AGREE)
                ++agreeNum;
        }
    }

    if (agreeNum == pBoot->votedNeeded ||                   // Vote passed
        votesNum == pBoot->votes.size() ||                  // All voted but not passed
        (pBoot->votes.size() - votesNum + agreeNum) < pBoot->votedNeeded) // Vote didnt passed
    {
        // Send update info to all players
        pBoot->inProgress = false;
        for (LfgAnswerMap::const_iterator itVotes = pBoot->votes.begin(); itVotes != pBoot->votes.end(); ++itVotes)
            if (Player *plrg = sObjectMgr.GetPlayer(itVotes->first))
                if (plrg->GetGUIDLow() != pBoot->victimLowGuid)
                    plrg->GetSession()->SendLfgBootPlayer(pBoot);

        if (agreeNum == pBoot->votedNeeded)                 // Vote passed - Kick player
        {
            Player::RemoveFromGroup(grp, MAKE_NEW_GUID(pBoot->victimLowGuid, 0, HIGHGUID_PLAYER));
            if (Player *victim = sObjectMgr.GetPlayer(pBoot->victimLowGuid))
                victim->TeleportToBGEntryPoint();
            OfferContinue(grp);
            grp->SetLfgKicks(grp->GetLfgKicks() + 1);
        }
        grp->SetLfgKickActive(false);
        delete pBoot;
        m_Boots.erase(itBoot);
    }
}

/// <summary>
/// Teleports the player in or out the dungeon
/// </summary>
/// <param name="Player *">Player</param>
/// <param name="bool">Teleport out</param>
void LFGMgr::TeleportPlayer(Player *plr, bool out)
{
    if (out)
    {
        plr->TeleportToBGEntryPoint();
        return;
    }

    if (!plr->isAlive())
    {
        plr->GetSession()->SendLfgTeleportError(LFG_TELEPORTERROR_PLAYER_DEAD);
        return;
    }

    if (plr->IsFalling() || plr->hasUnitState(UNIT_STAT_JUMPING))
    {
        plr->GetSession()->SendLfgTeleportError(LFG_TELEPORTERROR_FALLING);
        return;
    }

    // TODO Add support for LFG_TELEPORTERROR_FATIGUE and LFG_TELEPORTERROR_INVALID_LOCATION
    if (Group *grp = plr->GetGroup())
        if (LFGDungeonEntry const *dungeon = sLFGDungeonStore.LookupEntry(grp->GetLfgDungeonEntry()))
            if (AreaTrigger const* at = sObjectMgr.GetMapEntranceTrigger(dungeon->map))
            {
                if (!plr->GetMap()->IsDungeon() && !plr->GetMap()->IsRaid())
                    plr->SetBattlegroundEntryPoint();
                plr->RemoveAurasByType(SPELL_AURA_MOUNTED);
                // TODO: Teleport to group
                plr->TeleportTo(at->target_mapId, at->target_X, at->target_Y, at->target_Z, at->target_Orientation);
            }
}

/// <summary>
/// Give completion reward to player
/// </summary>
/// <param name="const uint32">dungeonId</param>
/// <param name="Player *">player</param>
void LFGMgr::RewardDungeonDoneFor(const uint32 dungeonId, Player *player)
{
    Group* group = player->GetGroup();
    if (!group || !group->isLFGGroup())
        return;

    // Mark dungeon as finished
    if (!group->isLfgDungeonComplete())
        group->SetLfgStatus(LFG_STATUS_COMPLETE);

    // Clear player related lfg stuff
    uint32 rDungeonId = *player->GetLfgDungeons()->begin();
    player->GetLfgDungeons()->clear();
    player->SetLfgRoles(ROLE_NONE);
 
    // Give rewards only if its a random dungeon
    LFGDungeonEntry const *dungeon = sLFGDungeonStore.LookupEntry(dungeonId);
    if (!dungeon || dungeon->type != LFG_TYPE_RANDOM)
        return;

    // Mark random dungeon as complete
    uint8 index = player->isLfgDungeonDone(rDungeonId) ? 1 : 0;
    if (!index)
        player->SetLfgDungeonDone(rDungeonId);

    // Update achievements
    if (dungeon->difficulty == DUNGEON_DIFFICULTY_HEROIC)
        player->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_USE_LFD_TO_GROUP_WITH_PLAYERS, 1);

    LfgReward const* reward = GetRandomDungeonReward(rDungeonId, player->getLevel());
    if (!reward)
        return;

    Quest const* qReward = sObjectMgr.GetQuestTemplate(reward->reward[index].questId);
    if (!qReward)
        return;

    // Give rewards
    player->GetSession()->SendLfgPlayerReward(dungeon->Entry(), group->GetLfgDungeonEntry(false), index, reward, qReward);

    if (qReward->GetRewItemsCount() > 0)
    {
        for (uint32 i = 0; i < QUEST_REWARDS_COUNT; ++i)
        {
            if (uint32 itemId = qReward->RewItemId[i])
            {
                ItemPosCountVec dest;
                if (player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemId, qReward->RewItemCount[i]) == EQUIP_ERR_OK)
                {
                    Item* item = player->StoreNewItem(dest, itemId, true, Item::GenerateItemRandomPropertyId(itemId));
                    player->SendNewItem(item, qReward->RewItemCount[i], true, false);
                }
            }
        }
    }

    // Not give XP in case already completed once repeatable quest
    uint32 XP = uint32(qReward->XPValue(player) * sWorld.getRate(RATE_XP_QUEST));

    XP += (MAXGROUPSIZE - group->GetMembersCount()) * reward->reward[index].variableXP;

    // Give player extra money if GetRewOrReqMoney > 0 and get ReqMoney if negative
    int32 moneyRew = qReward->GetRewOrReqMoney();

    if (player->getLevel() < sWorld.getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
        player->GiveXP(XP, NULL);
    else
        moneyRew += int32(qReward->GetRewMoneyMaxLevel() * sWorld.getRate(RATE_DROP_MONEY));

    moneyRew += (MAXGROUPSIZE - group->GetMembersCount()) * reward->reward[index].variableMoney;

    if (moneyRew)
        player->ModifyMoney(moneyRew);
}

// --------------------------------------------------------------------------//
// Packet Functions
// --------------------------------------------------------------------------//

/// <summary>
/// Build lfgRolecheck packet
/// </summary>
/// <param name="WorldPacket &">WorldPacket</param>
/// <param name="LfgRoleCheck *">RoleCheck info</param>
void LFGMgr::BuildLfgRoleCheck(WorldPacket &data, LfgRoleCheck *pRoleCheck)
{
    ASSERT(pRoleCheck);

    Player *plr;
    uint8 roles;

    data << uint32(pRoleCheck->result);                     // Check result
    data << uint8(pRoleCheck->result == LFG_ROLECHECK_INITIALITING);
    data << uint8(pRoleCheck->dungeons.size());             // Number of dungeons
    LFGDungeonEntry const *dungeon;
    for (LfgDungeonSet::iterator it = pRoleCheck->dungeons.begin(); it != pRoleCheck->dungeons.end(); ++it)
    {
        dungeon = sLFGDungeonStore.LookupEntry(*it);        // not null - been checked at join time
        if (!dungeon)
        {
            sLog.outError("LFGMgr::BuildLfgRoleCheck: Dungeon %u does not exist in dbcs", *it);
            data << uint32(0);
        }
        else
            data << uint32(dungeon->Entry());               // Dungeon
    }

    data << uint8(pRoleCheck->roles.size());                // Players in group
    // Leader info MUST be sent 1st :S
    roles = pRoleCheck->roles[pRoleCheck->leader];
    uint64 guid = MAKE_NEW_GUID(pRoleCheck->leader, 0, HIGHGUID_PLAYER);
    data << uint64(guid);                                   // Guid
    data << uint8(roles > 0);                               // Ready
    data << uint32(roles);                                  // Roles
    plr = sObjectMgr.GetPlayer(guid);
    data << uint8(plr ? plr->getLevel() : 0);               // Level

    for (LfgRolesMap::const_iterator itPlayers = pRoleCheck->roles.begin(); itPlayers != pRoleCheck->roles.end(); ++itPlayers)
    {
        if (itPlayers->first == pRoleCheck->leader)
            continue;

        roles = itPlayers->second;
        guid = MAKE_NEW_GUID(itPlayers->first, 0, HIGHGUID_PLAYER);
        data << uint64(guid);                               // Guid
        data << uint8(roles > 0);                           // Ready
        data << uint32(roles);                              // Roles
        plr = sObjectMgr.GetPlayer(guid);
        data << uint8(plr ? plr->getLevel() : 0);           // Level
    }
}

/// <summary>
/// Build Party Dungeon lock status packet
/// </summary>
/// <param name="WorldPacket &">WorldPacket</param>
/// <param name="LfgLockStatusMap *">lock status map</param>
void LFGMgr::BuildPartyLockDungeonBlock(WorldPacket &data, LfgLockStatusMap *lockMap)
{
    if (!lockMap || !lockMap->size())
    {
        data << uint8(0);
        return;
    }

    data << uint8(lockMap->size());
    for (LfgLockStatusMap::const_iterator it = lockMap->begin(); it != lockMap->end(); ++it)
    {
        data << uint64(MAKE_NEW_GUID(it->first, 0, HIGHGUID_PLAYER)); // Player guid
        BuildPlayerLockDungeonBlock(data, it->second);
    }
    lockMap->clear();
    delete lockMap;
}

/// <summary>
/// Build Player Dungeon lock status packet
/// </summary>
/// <param name="WorldPacket &">WorldPacket</param>
/// <param name="LfgLockStatusSet *">lock status list</param>
void LFGMgr::BuildPlayerLockDungeonBlock(WorldPacket &data, LfgLockStatusSet *lockSet)
{
    if (!lockSet || !lockSet->size())
    {
        data << uint8(0);
        return;
    }
    data << uint32(lockSet->size());                        // Size of lock dungeons
    for (LfgLockStatusSet::iterator it = lockSet->begin(); it != lockSet->end(); ++it)
    {
        data << uint32((*it)->dungeon);                     // Dungeon entry + type
        data << uint32((*it)->lockstatus);                  // Lock status
        delete (*it);
    }
    lockSet->clear();
    delete lockSet;
}

// --------------------------------------------------------------------------//
// Auxiliar Functions
// --------------------------------------------------------------------------//

/// <summary>
/// Given a group get the dungeons that can't be done and reason
/// </summary>
/// <param name="PlayerSet *">Players to check lock status</param>
/// <param name="LfgDungeonSet *">Dungeons to check</param>
/// <param name="bool">Use dungeon entry (true) or id (false)</param>
/// <returns>LfgLockStatusMap*</returns>
LfgLockStatusMap* LFGMgr::GetGroupLockStatusDungeons(PlayerSet *pPlayers, LfgDungeonSet *dungeons, bool useEntry )
{
    if (!pPlayers || !dungeons)
        return NULL;

    LfgLockStatusSet *dungeonSet = NULL;
    LfgLockStatusMap *dungeonMap = new LfgLockStatusMap();
    for (PlayerSet::const_iterator itr = pPlayers->begin(); itr != pPlayers->end(); ++itr)
    {
        dungeonSet = GetPlayerLockStatusDungeons(*itr, dungeons, useEntry);
        if (dungeonSet)
            (*dungeonMap)[(*itr)->GetGUIDLow()] = dungeonSet;
    }

    if (!dungeonMap->size())
    {
        delete dungeonMap;
        dungeonMap = NULL;
    }
    return dungeonMap;
}

/// <summary>
/// Get all Group members list of dungeons that can't be done and reason
/// leader excluded as the list given is he list he can do
/// </summary>
/// <param name="Player *">Player to get Party Lock info</param>
/// <param name="LfgDungeonSet *">Dungeons to check</param>
/// <returns>LfgLockStatusMap*</returns>
LfgLockStatusMap* LFGMgr::GetPartyLockStatusDungeons(Player *plr, LfgDungeonSet *dungeons )
{
    if (!plr)
        return NULL;

    if (!dungeons)
        dungeons = GetAllDungeons();

    Group *grp = plr->GetGroup();
    if (!grp)
        return NULL;

    PlayerSet *pPlayers = new PlayerSet();
    Player *plrg;
    for (GroupReference *itr = grp->GetFirstMember(); itr != NULL; itr = itr->next())
    {
        plrg = itr->getSource();
        if (plrg && plrg != plr)
            pPlayers->insert(plrg);
    }
    LfgLockStatusMap *dungeonMap = GetGroupLockStatusDungeons(pPlayers, dungeons);
    pPlayers->clear();
    delete pPlayers;
    return dungeonMap;
}

/// <summary>
/// Get list of dungeons player can't do and reasons
/// </summary>
/// <param name="Player *">Player to check lock status</param>
/// <param name="LfgDungeonSet *">Dungeons to check</param>
/// <param name="bool">Use dungeon entry (true) or id (false)</param>
/// <returns>LfgLockStatusSet*</returns>
LfgLockStatusSet* LFGMgr::GetPlayerLockStatusDungeons(Player *plr, LfgDungeonSet *dungeons , bool useEntry )
{
    LfgLockStatusSet *list = new LfgLockStatusSet();
    LfgLockStatus *lockstatus = NULL;
    LFGDungeonEntry const *dungeon;
    LfgLockStatusType locktype;
    uint8 level = plr->getLevel();
    uint8 expansion = plr->GetSession()->Expansion();
    AccessRequirement const* ar;

    if (!dungeons)
        dungeons = GetAllDungeons();

    for (LfgDungeonSet::const_iterator it = dungeons->begin(); it != dungeons->end(); ++it)
    {
        dungeon = sLFGDungeonStore.LookupEntry(*it);
        if (!dungeon) // should never happen - We provide a list from sLFGDungeonStore
            continue;

        ar = sObjectMgr.GetAccessRequirement(dungeon->map, Difficulty(dungeon->difficulty));

        locktype = LFG_LOCKSTATUS_OK;
        if (dungeon->expansion > expansion)
            locktype = LFG_LOCKSTATUS_INSUFFICIENT_EXPANSION;
        else if (sDisableMgr.IsDisabledFor(DISABLE_TYPE_MAP, dungeon->map, plr))
            locktype = LFG_LOCKSTATUS_RAID_LOCKED;
        else if (dungeon->difficulty > DUNGEON_DIFFICULTY_NORMAL && plr->GetBoundInstance(dungeon->map, Difficulty(dungeon->difficulty)))
            locktype = LFG_LOCKSTATUS_RAID_LOCKED;
        else if (dungeon->minlevel > level)
            locktype = LFG_LOCKSTATUS_TOO_LOW_LEVEL;
        else if (dungeon->maxlevel < level)
            locktype = LFG_LOCKSTATUS_TOO_HIGH_LEVEL;
        else if (locktype == LFG_LOCKSTATUS_OK && ar)
        {
            if (ar->achievement && !plr->GetAchievementMgr().HasAchieved(sAchievementStore.LookupEntry(ar->achievement)))
                locktype = LFG_LOCKSTATUS_RAID_LOCKED; // FIXME: Check the correct lock value
            else if (plr->GetTeam() == ALLIANCE && ar->quest_A && !plr->GetQuestRewardStatus(ar->quest_A))
                locktype = LFG_LOCKSTATUS_QUEST_NOT_COMPLETED;
            else if (plr->GetTeam() == HORDE && ar->quest_H && !plr->GetQuestRewardStatus(ar->quest_H))
                locktype = LFG_LOCKSTATUS_QUEST_NOT_COMPLETED;
            else
                if (ar->item)
                {
                    if (!plr->HasItemCount(ar->item, 1) && (!ar->item2 || !plr->HasItemCount(ar->item2, 1)))
                        locktype = LFG_LOCKSTATUS_MISSING_ITEM;
                }
                else if (ar->item2 && !plr->HasItemCount(ar->item2, 1))
                    locktype = LFG_LOCKSTATUS_MISSING_ITEM;
        }

        if (locktype != LFG_LOCKSTATUS_OK)
        {
            lockstatus = new LfgLockStatus();
            lockstatus->dungeon = useEntry ? dungeon->Entry(): dungeon->ID;
            lockstatus->lockstatus = locktype;
            list->insert(lockstatus);
        }
    }
    if (!list->size())
    {
        delete list;
        list = NULL;
    }
    return list;
}

/// <summary>
/// Get the dungeon list that can be done.
/// </summary>
/// <returns>LfgDungeonSet*</returns>
LfgDungeonSet* LFGMgr::GetAllDungeons()
{
    LfgDungeonSet *alldungeons = m_CachedDungeonMap[0];

    if (alldungeons)
        return alldungeons;

    LfgDungeonSet *dungeons;
    LFGDungeonEntry const *dungeon;

    alldungeons = new LfgDungeonSet();
    m_CachedDungeonMap[0] = alldungeons;
    for (uint32 i = 0; i < sLFGDungeonStore.GetNumRows(); ++i)
    {
        dungeon = sLFGDungeonStore.LookupEntry(i);
        if (!dungeon || dungeon->type == LFG_TYPE_ZONE)
            continue;
        dungeons = m_CachedDungeonMap[dungeon->grouptype];
        if (!dungeons)
        {
            dungeons = new LfgDungeonSet();
            m_CachedDungeonMap[dungeon->grouptype] = dungeons;
        }
        if (dungeon->type != LFG_TYPE_RANDOM)
            dungeons->insert(dungeon->ID);
        alldungeons->insert(dungeon->ID);
    }

    return alldungeons;
}

/// <summary>
/// Get the dungeon list that can be done given a random dungeon entry.
/// Special case: randomdungeon == 0 then will return all dungeons
/// </summary>
/// <param name="uint32">Random dungeon entry</param>
/// <returns>LfgDungeonSet*</returns>
LfgDungeonSet* LFGMgr::GetDungeonsByRandom(uint32 randomdungeon)
{
    uint8 groupType = 0;
    if (LFGDungeonEntry const *dungeon = sLFGDungeonStore.LookupEntry(randomdungeon))
        groupType = dungeon->grouptype;

    return m_CachedDungeonMap[groupType];
}

/// <summary>
/// Get the random dungeon list that can be done at a certain level and expansion.
/// </summary>
/// <param name="uint8">Player level</param>
/// <param name="uint8">Player account expansion</param>
/// <returns>LfgDungeonSet*</returns>
LfgDungeonSet* LFGMgr::GetRandomDungeons(uint8 level, uint8 expansion)
{
    LfgDungeonSet *list = new LfgDungeonSet();
    LFGDungeonEntry const *dungeon;
    for (uint32 i = 0; i < sLFGDungeonStore.GetNumRows(); ++i)
    {
        dungeon = sLFGDungeonStore.LookupEntry(i);
        if (dungeon && dungeon->expansion <= expansion && dungeon->type == LFG_TYPE_RANDOM &&
            dungeon->minlevel <= level && level <= dungeon->maxlevel)
            list->insert(dungeon->Entry());
    }
    return list;
}

/// <summary>
/// Get the reward of a given random dungeon at a certain level
/// </summary>
/// <param name="uint32">random dungeon id</param>
/// <param name="uint8">Player level</param>
/// <returns>LfgReward const*</returns>
LfgReward const* LFGMgr::GetRandomDungeonReward(uint32 dungeon, uint8 level)
{
    LfgReward const* rew = NULL;
    LfgRewardMapBounds bounds = m_RewardMap.equal_range(dungeon & 0x00FFFFFF);
    for (LfgRewardMap::const_iterator itr = bounds.first; itr != bounds.second; ++itr)
    {
        rew = itr->second;
        // ordered properly at loading
        if (itr->second->GetMaxLevel() >= level)
            break;
    }

    return rew;
}

/// <summary>
/// Given a Dungeon id returns the dungeon Group Type
/// </summary>
/// <param name="uint32">Dungeon id</param>
/// <returns>uint8: GroupType</returns>
uint8 LFGMgr::GetDungeonGroupType(uint32 dungeonId)
{
    LFGDungeonEntry const *dungeon = sLFGDungeonStore.LookupEntry(dungeonId);
    if (!dungeon)
        return 0;

    return dungeon->grouptype;
}

/// <summary>
/// Given a Dungeon id returns if it's random
/// </summary>
/// <param name="uint32">Dungeon id</param>
/// <returns>bool</returns>
bool LFGMgr::isRandomDungeon(uint32 dungeonId)
{
    LFGDungeonEntry const *dungeon = sLFGDungeonStore.LookupEntry(dungeonId);
    if (!dungeon)
        return false;

    return dungeon->type == LFG_TYPE_RANDOM;
}

/// <summary>
/// Given a list of guids returns the concatenation using | as delimiter
/// </summary>
/// <param name="LfgGuidList ">list of guids</param>
/// <returns>std::string</returns>
std::string LFGMgr::ConcatenateGuids(LfgGuidList check)
{
    if (check.empty())
        return "";

    LfgGuidSet guidSet;   
    while (!check.empty())
    {
        guidSet.insert(check.front());
        check.pop_front();
    }

    std::ostringstream o;
    LfgGuidSet::iterator it = guidSet.begin();
    o << *it;
    for (++it; it != guidSet.end(); ++it)
        o << "|" << *it;
    guidSet.clear();
    return o.str();
} 

enum LFGenum
{
    LFG_TIME_ROLECHECK       = 2*MINUTE,
    LFG_TIME_BOOT            = 2*MINUTE,
    LFG_TIME_PROPOSAL        = 2*MINUTE,
    LFG_TANKS_NEEDED         = 1,
    LFG_HEALERS_NEEDED       = 1,
    LFG_DPS_NEEDED           = 3,
    LFG_QUEUEUPDATE_INTERVAL = 15000,
    LFG_SPELL_COOLDOWN       = 71328,
    LFG_SPELL_DESERTER       = 71041,
    LFG_MAX_KICKS            = 3,
};

enum LfgType
{
    LFG_TYPE_DUNGEON = 1,
    LFG_TYPE_RAID    = 2,
    LFG_TYPE_QUEST   = 3,
    LFG_TYPE_ZONE    = 4,
    LFG_TYPE_HEROIC  = 5,
    LFG_TYPE_RANDOM  = 6,
};

enum LfgProposalState
{
    LFG_PROPOSAL_INITIATING = 0,
    LFG_PROPOSAL_FAILED     = 1,
    LFG_PROPOSAL_SUCCESS    = 2,
};

enum LfgGroupType
{
    LFG_GROUPTYPE_ALL          = 0,                         // Internal use, represents all groups.
    LFG_GROUPTYPE_CLASSIC      = 1,
    LFG_GROUPTYPE_BC_NORMAL    = 2,
    LFG_GROUPTYPE_BC_HEROIC    = 3,
    LFG_GROUPTYPE_WTLK_NORMAL  = 4,
    LFG_GROUPTYPE_WTLK_HEROIC  = 5,
    LFG_GROUPTYPE_CLASSIC_RAID = 6,
    LFG_GROUPTYPE_BC_RAID      = 7,
    LFG_GROUPTYPE_WTLK_RAID_10 = 8,
    LFG_GROUPTYPE_WTLK_RAID_25 = 9,
};

enum LfgLockStatusType
{
    LFG_LOCKSTATUS_OK                        = 0,           // Internal use only
    LFG_LOCKSTATUS_INSUFFICIENT_EXPANSION    = 1,
    LFG_LOCKSTATUS_TOO_LOW_LEVEL             = 2,
    LFG_LOCKSTATUS_TOO_HIGH_LEVEL            = 3,
    LFG_LOCKSTATUS_TOO_LOW_GEAR_SCORE        = 4,
    LFG_LOCKSTATUS_TOO_HIGH_GEAR_SCORE       = 5,
    LFG_LOCKSTATUS_RAID_LOCKED               = 6,
    LFG_LOCKSTATUS_ATTUNEMENT_TOO_LOW_LEVEL  = 1001,
    LFG_LOCKSTATUS_ATTUNEMENT_TOO_HIGH_LEVEL = 1002,
    LFG_LOCKSTATUS_QUEST_NOT_COMPLETED       = 1022,
    LFG_LOCKSTATUS_MISSING_ITEM              = 1025,
    LFG_LOCKSTATUS_NOT_IN_SEASON             = 1031,
};

enum LfgTeleportError
{
    //LFG_TELEPORTERROR_UNK1           = 0,                 // No reaction
    LFG_TELEPORTERROR_PLAYER_DEAD      = 1,
    LFG_TELEPORTERROR_FALLING          = 2,
    //LFG_TELEPORTERROR_UNK2           = 3,                 // You can't do that right now
    LFG_TELEPORTERROR_FATIGUE          = 4,
    //LFG_TELEPORTERROR_UNK3           = 5,                 // No reaction
    LFG_TELEPORTERROR_INVALID_LOCATION = 6,
    //LFG_TELEPORTERROR_UNK4           = 7,                 // You can't do that right now
    //LFG_TELEPORTERROR_UNK5           = 8,                 // You can't do that right now
};

enum LfgJoinResult
{
    LFG_JOIN_OK                    = 0,                     // Joined (no client msg)
    LFG_JOIN_FAILED                = 1,                     // RoleCheck Failed
    LFG_JOIN_GROUPFULL             = 2,                     // Your group is full
    LFG_JOIN_UNK3                  = 3,                     // No client reaction
    LFG_JOIN_INTERNAL_ERROR        = 4,                     // Internal LFG Error
    LFG_JOIN_NOT_MEET_REQS         = 5,                     // You do not meet the requirements for the chosen dungeons
    LFG_JOIN_PARTY_NOT_MEET_REQS   = 6,                     // One or more party members do not meet the requirements for the chosen dungeons
    LFG_JOIN_MIXED_RAID_DUNGEON    = 7,                     // You cannot mix dungeons, raids, and random when picking dungeons
    LFG_JOIN_MULTI_REALM           = 8,                     // The dungeon you chose does not support players from multiple realms
    LFG_JOIN_DISCONNECTED          = 9,                     // One or more party members are pending invites or disconnected
    LFG_JOIN_PARTY_INFO_FAILED     = 10,                    // Could not retrieve information about some party members
    LFG_JOIN_DUNGEON_INVALID       = 11,                    // One or more dungeons was not valid
    LFG_JOIN_DESERTER              = 12,                    // You can not queue for dungeons until your deserter debuff wears off
    LFG_JOIN_PARTY_DESERTER        = 13,                    // One or more party members has a deserter debuff
    LFG_JOIN_RANDOM_COOLDOWN       = 14,                    // You can not queue for random dungeons while on random dungeon cooldown
    LFG_JOIN_PARTY_RANDOM_COOLDOWN = 15,                    // One or more party members are on random dungeon cooldown
    LFG_JOIN_TOO_MUCH_MEMBERS      = 16,                    // You can not enter dungeons with more that 5 party members
    LFG_JOIN_USING_BG_SYSTEM       = 17,                    // You can not use the dungeon system while in BG or arenas
    LFG_JOIN_FAILED2               = 18,                    // RoleCheck Failed
};

enum LfgRoleCheckResult
{
    LFG_ROLECHECK_FINISHED     = 1,                         // Role check finished
    LFG_ROLECHECK_INITIALITING = 2,                         // Role check begins
    LFG_ROLECHECK_MISSING_ROLE = 3,                         // Someone didn't selected a role after 2 mins
    LFG_ROLECHECK_WRONG_ROLES  = 4,                         // Can't form a group with that role selection
    LFG_ROLECHECK_ABORTED      = 5,                         // Someone leave the group
    LFG_ROLECHECK_NO_ROLE      = 6,                         // Someone selected no role
};

enum LfgAnswer
{
    LFG_ANSWER_PENDING = -1,
    LFG_ANSWER_DENY    = 0,
    LFG_ANSWER_AGREE   = 1,
};

// Dungeon and reason why player can't join
struct LfgLockStatus
{
    uint32 dungeon;
    LfgLockStatusType lockstatus;
};

// Reward info
struct LfgReward
{
    uint32 maxLevel;
    struct
    {
        uint32 questId;
        uint32 variableMoney;
        uint32 variableXP;
    } reward[2];

    LfgReward(uint32 _maxLevel, uint32 firstQuest, uint32 firstVarMoney, uint32 firstVarXp, uint32 otherQuest, uint32 otherVarMoney, uint32 otherVarXp)
        : maxLevel(_maxLevel)
    {
        reward[0].questId = firstQuest;
        reward[0].variableMoney = firstVarMoney;
        reward[0].variableXP = firstVarXp;
        reward[1].questId = otherQuest;
        reward[1].variableMoney = otherVarMoney;
        reward[1].variableXP = otherVarXp;
    }
};

typedef std::map<uint32, uint8> LfgRolesMap;
typedef std::map<uint32, LfgAnswer> LfgAnswerMap;
typedef std::list<uint64> LfgGuidList;
typedef std::map<uint64, LfgDungeonSet*> LfgDungeonMap;

// Stores player or group queue info
struct LfgQueueInfo
{
    LfgQueueInfo(): tanks(LFG_TANKS_NEEDED), healers(LFG_HEALERS_NEEDED), dps(LFG_DPS_NEEDED) {};
    time_t joinTime;                                        // Player queue join time (to calculate wait times)
    uint8 tanks;                                            // Tanks needed
    uint8 healers;                                          // Healers needed
    uint8 dps;                                              // Dps needed
    LfgDungeonSet dungeons;                                 // Selected Player/Group Dungeon/s
    LfgRolesMap roles;                                      // Selected Player Role/s
};

struct LfgProposalPlayer
{
    LfgProposalPlayer(): role(0), accept(LFG_ANSWER_PENDING), groupLowGuid(0) {};
    uint8 role;                                             // Proposed role
    LfgAnswer accept;                                       // Accept status (-1 not answer | 0 Not agree | 1 agree)
    uint32 groupLowGuid;                                    // Original group guid (Low guid) 0 if no original group
};

typedef std::map<uint32, LfgProposalPlayer*> LfgProposalPlayerMap;

// Stores all Dungeon Proposal after matching candidates
struct LfgProposal
{
    LfgProposal(uint32 dungeon): dungeonId(dungeon), state(LFG_PROPOSAL_INITIATING), groupLowGuid(0), leaderLowGuid(0) {}

    ~LfgProposal()
    {
        for (LfgProposalPlayerMap::iterator it = players.begin(); it != players.end(); ++it)
            delete it->second;
        players.clear();
        queues.clear();
    };
    uint32 dungeonId;                                       // Dungeon to join
    LfgProposalState state;                                 // State of the proposal
    uint32 groupLowGuid;                                    // Proposal group (0 if new)
    uint32 leaderLowGuid;                                   // Leader guid.
    time_t cancelTime;                                      // Time when we will cancel this proposal
    LfgGuidList queues;                                     // Queue Ids to remove/readd
    LfgProposalPlayerMap players;                           // Player current groupId

};

// Stores all rolecheck info of a group that wants to join LFG
struct LfgRoleCheck
{
    time_t cancelTime;
    LfgRolesMap roles;
    LfgRoleCheckResult result;
    LfgDungeonSet dungeons;
    uint32 leader;
};

// Stores information of a current vote to kick someone from a group
struct LfgPlayerBoot
{
    time_t cancelTime;                                      // Time left to vote
    bool inProgress;                                        // Vote in progress
    LfgAnswerMap votes;                                     // Player votes (-1 not answer | 0 Not agree | 1 agree)
    uint32 victimLowGuid;                                   // Player guid to be kicked (can't vote)
    uint8 votedNeeded;                                      // Votes needed to kick the player
    std::string reason;                                     // kick reason
};

typedef InRangePlayerSet PlayerSet;
typedef std::set<LfgLockStatus*> LfgLockStatusSet;
typedef std::vector<LfgProposal*> LfgProposalList;
typedef std::map<uint32, LfgLockStatusSet*> LfgLockStatusMap;
typedef std::map<uint64, LfgQueueInfo*> LfgQueueInfoMap;
typedef std::map<uint32, LfgRoleCheck*> LfgRoleCheckMap;
typedef std::map<uint32, LfgProposal*> LfgProposalMap;
typedef std::map<uint32, LfgPlayerBoot*> LfgPlayerBootMap;
typedef std::multimap<uint32, LfgReward const*> LfgRewardMap;
typedef std::pair<LfgRewardMap::const_iterator, LfgRewardMap::const_iterator> LfgRewardMapBounds;
typedef std::list<Player *> LfgPlayerList;
typedef std::set<uint64> LfgGuidSet;
typedef std::map<std::string, LfgAnswer> LfgCompatibleMap;


class LFGMgr
{
    friend class ACE_Singleton<LFGMgr, ACE_Null_Mutex>;
    public:
        LFGMgr();
        ~LFGMgr();

        void Join(Player *plr);
        void Leave(Player *plr, Group *grp = NULL);
        void OfferContinue(Group *grp);
        void TeleportPlayer(Player *plr, bool out);
        void UpdateProposal(uint32 proposalId, uint32 lowGuid, bool accept);
        void UpdateBoot(Player *plr, bool accept);
        void UpdateRoleCheck(Group *grp, Player *plr = NULL);
        void Update(uint32 diff);

        bool isRandomDungeon(uint32 dungeonId);
        void InitBoot(Group *grp, uint32 plowGuid, uint32 vlowGuid, std::string reason);

        void LoadDungeonEncounters();
        void LoadRewards();
        void RewardDungeonDoneFor(const uint32 dungeonId, Player* player);
        uint32 GetDungeonIdForAchievement(uint32 achievementId)
        {
            std::map<uint32, uint32>::iterator itr = m_EncountersByAchievement.find(achievementId);
            if (itr != m_EncountersByAchievement.end())
                return itr->second;

            return 0;
        };

        LfgLockStatusMap* GetPartyLockStatusDungeons(Player *plr, LfgDungeonSet *dungeons = NULL);
        LfgDungeonSet* GetRandomDungeons(uint8 level, uint8 expansion);
        LfgLockStatusSet* GetPlayerLockStatusDungeons(Player *plr, LfgDungeonSet *dungeons = NULL, bool useEntry = true);
        LfgReward const* GetRandomDungeonReward(uint32 dungeon, uint8 level);
        void BuildPlayerLockDungeonBlock(WorldPacket &data, LfgLockStatusSet *lockSet);
        void BuildPartyLockDungeonBlock(WorldPacket &data, LfgLockStatusMap *lockMap);

    private:
        void Cleaner();
        void AddGuidToNewQueue(uint64 guid);
        void AddToQueue(uint64 guid, LfgRolesMap *roles, LfgDungeonSet *dungeons);

        bool RemoveFromQueue(uint64 guid);
        void RemoveProposal(LfgProposalMap::iterator itProposal, LfgUpdateType type);

        void FindNewGroups(LfgGuidList &check, LfgGuidList all, LfgProposalList *proposals);

        bool CheckGroupRoles(LfgRolesMap &groles, bool removeLeaderFlag = true);
        bool CheckCompatibility(LfgGuidList check, LfgProposalList *proposals);
        LfgDungeonSet* CheckCompatibleDungeons(LfgDungeonMap *dungeonsMap, PlayerSet *players);
        void SetCompatibles(std::string concatenatedGuids, bool compatibles);
        LfgAnswer GetCompatibles(std::string concatenatedGuids);
        void RemoveFromCompatibles(uint64 guid);
        std::string ConcatenateGuids(LfgGuidList check);

        void BuildLfgRoleCheck(WorldPacket &data, LfgRoleCheck *pRoleCheck);
        void BuildAvailableRandomDungeonList(WorldPacket &data, Player *plr);
        void BuildBootPlayerBlock(WorldPacket &data, LfgPlayerBoot *pBoot, uint32 lowGuid);

        LfgLockStatusMap* GetGroupLockStatusDungeons(PlayerSet *pPlayers, LfgDungeonSet *dungeons, bool useEntry = true);
        LfgDungeonSet* GetDungeonsByRandom(uint32 randomdungeon);
        LfgDungeonSet* GetAllDungeons();
        uint8 GetDungeonGroupType(uint32 dungeon);

        LfgRewardMap m_RewardMap;                           // Stores rewards for random dungeons
        std::map<uint32, uint32> m_EncountersByAchievement; // Stores dungeon ids associated with achievements (for rewards)
        LfgDungeonMap m_CachedDungeonMap;                   // Stores all dungeons by groupType
        LfgQueueInfoMap m_QueueInfoMap;                     // Queued groups
        LfgGuidList m_currentQueue;                         // Ordered list. Used to find groups
        LfgGuidList m_newToQueue;                           // New groups to add to queue
        LfgCompatibleMap m_CompatibleMap;                   // Compatible dungeons
        LfgProposalMap m_Proposals;                         // Current Proposals
        LfgPlayerBootMap m_Boots;                           // Current player kicks
        LfgRoleCheckMap m_RoleChecks;                       // Current Role checks
        uint32 m_QueueTimer;                                // used to check interval of update
        uint32 m_lfgProposalId;                             // used as internal counter for proposals
        int32 m_WaitTimeAvg;
        int32 m_WaitTimeTank;
        int32 m_WaitTimeHealer;
        int32 m_WaitTimeDps;
        uint32 m_NumWaitTimeAvg;
        uint32 m_NumWaitTimeTank;
        uint32 m_NumWaitTimeHealer;
        uint32 m_NumWaitTimeDps;
        bool m_update;
}; 
*/
#endif


















/*
void WorldSession::HandleSetLookingForGroupComment(WorldPacket& recvPacket)
{
	std::string comment;
		
	recvPacket >> comment;
	
	GetPlayer()->Lfgcomment = comment;	
}

void WorldSession::HandleEnableAutoJoin(WorldPacket& recvPacket)
{
	uint32 i;

	// make sure they're not queued in any invalid cases
	for(i = 0; i < MAX_LFG_QUEUE_ID; ++i)
	{
		if(_player->LfgDungeonId[i] != 0)
		{
			if(LfgDungeonTypes[_player->LfgDungeonId[i]] == LFG_TYPE_NONE 
				|| LfgDungeonTypes[_player->LfgDungeonId[i]] == LFG_TYPE_QUEST
				|| LfgDungeonTypes[_player->LfgDungeonId[i]] == LFG_TYPE_ZONE
				|| LfgDungeonTypes[_player->LfgDungeonId[i]] >= LFG_MAX_TYPES
				)
			{
				return;
			}
		}
	}

	// enable autojoin, join any groups if possible.
	_player->m_Autojoin = true;
	
	for(i = 0; i < MAX_LFG_QUEUE_ID; ++i)
	{
		if(_player->LfgDungeonId[i] != 0)
		{
			_player->SendMeetingStoneQueue(_player->LfgDungeonId[i], 1);
			sLfgMgr.UpdateLfgQueue(_player->LfgDungeonId[i]);
		}
	}
}

void WorldSession::HandleDisableAutoJoin(WorldPacket& recvPacket)
{
	uint32 i;
	_player->m_Autojoin = false;

	for(i = 0; i < MAX_LFG_QUEUE_ID; ++i)
	{
		if(_player->LfgDungeonId[i] != 0)
		{
			if( LfgDungeonTypes[_player->LfgDungeonId[i]] == LFG_TYPE_DUNGEON 
				|| LfgDungeonTypes[_player->LfgDungeonId[i]] == LFG_TYPE_RAID
				|| LfgDungeonTypes[_player->LfgDungeonId[i]] == LFG_TYPE_HEROIC_DUNGEON
				|| LfgDungeonTypes[_player->LfgDungeonId[i]] == LFG_TYPE_ANY_DUNGEON
				|| LfgDungeonTypes[_player->LfgDungeonId[i]] == LFG_TYPE_ANY_HEROIC_DUNGEON
				|| LfgDungeonTypes[_player->LfgDungeonId[i]] == LFG_TYPE_DAILY_DUNGEON
				|| LfgDungeonTypes[_player->LfgDungeonId[i]] == LFG_TYPE_DAILY_HEROIC_DUNGEON
				)
				_player->SendMeetingStoneQueue(_player->LfgDungeonId[i], 0);
		}
	}
}

void WorldSession::HandleEnableAutoAddMembers(WorldPacket& recvPacket)
{
	uint32 i;
	_player->m_AutoAddMem = true;

	for(i = 0; i < MAX_LFG_QUEUE_ID; ++i)
	{
		if(_player->LfgDungeonId[i] != 0)
		{
			sLfgMgr.UpdateLfgQueue(_player->LfgDungeonId[i]);
		}
	}
}

void WorldSession::HandleDisableAutoAddMembers(WorldPacket& recvPacket)
{
	_player->m_AutoAddMem = false;	
}

void WorldSession::HandleMsgLookingForGroup(WorldPacket& recvPacket)
{
	// this is looking for more 
	uint32 LfgType,LfgDungeonId,unk1;
	recvPacket >> LfgType >> LfgDungeonId >> unk1;
	
	if(LfgDungeonId > MAX_DUNGEONS)
	{ 
		return;
	}

	if(LfgDungeonId)
		sLfgMgr.SendLfgList(_player, LfgDungeonId);
}

void WorldSession::HandleSetLookingForGroup(WorldPacket& recvPacket)
{
	uint32 LfgQueueId;
	uint16 LfgDungeonId;
	uint8 LfgType,unk1;
	uint32 i;
	
	recvPacket >> LfgQueueId >> LfgDungeonId >> unk1 >> LfgType;
	
	if(LfgDungeonId >= MAX_DUNGEONS || LfgQueueId >= MAX_LFG_QUEUE_ID || LfgType != LfgDungeonTypes[LfgDungeonId])		// last one is for cheaters
	{ 
		return;
	}
	
	if(_player->LfgDungeonId[LfgQueueId] != 0)
		sLfgMgr.RemovePlayerFromLfgQueue(_player, _player->LfgDungeonId[LfgQueueId]);
	
	_player->LfgDungeonId[LfgQueueId]=LfgDungeonId;
	_player->LfgType[LfgQueueId]=LfgType;

	if(LfgDungeonId)
	{
		sLfgMgr.SetPlayerInLFGqueue(_player, LfgDungeonId);

		if( LfgType != LFG_TYPE_NONE && LfgType != LFG_TYPE_QUEST && LfgType != LFG_TYPE_ZONE && LfgType < LFG_MAX_TYPES )
		{
			sLfgMgr.UpdateLfgQueue(LfgDungeonId);
			if(_player->m_Autojoin)
				_player->SendMeetingStoneQueue(LfgDungeonId, 1);
		}
	}
	else
	{
		for(i = 0; i < MAX_LFG_QUEUE_ID; ++i)
		{
			if(_player->LfgDungeonId[i] != 0)
				break;
		}

		if( i == MAX_LFG_QUEUE_ID )
			_player->PartLFGChannel();
	}
}

void WorldSession::HandleSetLookingForMore(WorldPacket& recvPacket)
{
	uint16 LfgDungeonId;
	uint8 LfgType,unk1;

	recvPacket >> LfgDungeonId >> unk1 >> LfgType;
	if( LfgDungeonId >= MAX_DUNGEONS )
	{ 
		return;
	}

	// remove from an existing queue
	if(LfgDungeonId != _player->LfmDungeonId && _player->LfmDungeonId)
		sLfgMgr.RemovePlayerFromLfmList(_player, _player->LfmDungeonId);

	_player->LfmDungeonId = LfgDungeonId;
	_player->LfmType = LfgType;

	if(LfgDungeonId)
		sLfgMgr.SetPlayerInLfmList(_player, LfgDungeonId);
}

void WorldSession::HandleLfgClear(WorldPacket & recvPacket)
{
	sLfgMgr.RemovePlayerFromLfgQueues(_player);
}

void WorldSession::HandleLfgInviteAccept(WorldPacket & recvPacket)
{
	CHECK_INWORLD_RETURN
	
	_player->PartLFGChannel();
	if(
//		_player->m_lfgMatch == NULL && 
		_player->m_lfgInviterGuid == 0
		)
	{
//		if(_player->m_lfgMatch == NULL)
//			OutPacket(SMSG_LFG_MATCHMAKING_AUTOJOIN_FAILED_NO_PLAYER);		// Matched Player(s) have gone offline.
//		else
//			OutPacket(SMSG_LFG_MATCHMAKING_AUTOJOIN_FAILED);				// Group no longer available.

		return;
	}

/*	if( _player->m_lfgMatch != NULL )
	{
		// move into accepted players
		_player->m_lfgMatch->lock.Acquire();
		_player->m_lfgMatch->PendingPlayers.erase(_player);

		if( !_player->GetGroup() )
		{
			_player->m_lfgMatch->AcceptedPlayers.insert(_player);

			if(!_player->m_lfgMatch->PendingPlayers.size())
			{
				// all players have accepted
				Group * pGroup = new Group(true);
				for(InRangePlayerSet::iterator itr = _player->m_lfgMatch->AcceptedPlayers.begin(); itr != _player->m_lfgMatch->AcceptedPlayers.end(); ++itr)
					pGroup->AddMember((*itr)->m_playerInfo);

				_player->m_lfgMatch->pGroup = pGroup;
			}
		}
		_player->m_lfgMatch->lock.Release();
	}
	else */
/*	{
		Player * pPlayer = objmgr.GetPlayer(_player->m_lfgInviterGuid);
		if( pPlayer == NULL )
		{
//			OutPacket(SMSG_LFG_MATCHMAKING_AUTOJOIN_FAILED_NO_PLAYER);			// Matched Player(s) have gone offline.
			return;
		}

		if( pPlayer->GetGroup() == NULL || pPlayer->GetGroup()->IsFull() || pPlayer->GetGroup()->GetLeader() != pPlayer->m_playerInfo )
		{
//			OutPacket(SMSG_LFG_MATCHMAKING_AUTOJOIN_FAILED);
			return;
		}

		pPlayer->GetGroup()->AddMember(_player->m_playerInfo);
	}
	_player->m_lfgInviterGuid = 0;
//	_player->m_lfgMatch = NULL;
}
*/