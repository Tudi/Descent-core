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

enum GMticketType
{
	GM_TICKET_TYPE_STUCK				= 1,
	GM_TICKET_TYPE_BEHAVIOR_HARASSMENT	= 2,
	GM_TICKET_TYPE_GUILD				= 3,
	GM_TICKET_TYPE_ITEM					= 4,
	GM_TICKET_TYPE_ENVIRONMENTAL		= 5,
	GM_TICKET_TYPE_NON_QUEST_CREEP		= 6,
	GM_TICKET_TYPE_QUEST_QUEST_NPC		= 7,
	GM_TICKET_TYPE_TECHNICAL			= 8,
	GM_TICKET_TYPE_ACCOUNT_BILLING		= 9,
	GM_TICKET_TYPE_CHARACTER			= 10,
};

void WorldSession::HandleGMTicketCreateOpcode(WorldPacket & recv_data)
{
/*
{CLIENT} Packet: (0xF654) UNKNOWN PacketSize = 34 stamp = 6401313
|------------------------------------------------|----------------|
|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
|------------------------------------------------|----------------|
|00 00 00 00 B5 41 0A C6 60 FB B3 C2 3D 5A A7 42 |.....A..`...=Z.B|
|74 65 73 74 00 11 00 00 00 00 00 00 00 00 00 00 |test............|
|00 00                                           |..              |
-------------------------------------------------------------------
00 00 00 00 
B5 41 0A C6 
60 FB B3 C2
3D 5A A7 42 
74 65 73 74 00 
11 00 00 00 
00 - text 2
00 00 00 00 - counter
00 00 00 00 - zip size

{CLIENT} Packet: (0xF654) UNKNOWN PacketSize = 1598 TimeStamp = 25375559
12 02 00 00 
72 91 EF C4 
07 88 A7 45 
A8 D6 46 C1 
49 20 68 61 76 65 20 61 20 63 68 61 72 20 6E 61 6D 65 20 73 63 72 65 65 74 68 2E 20 48 61 76 65 6E 27 74 20 70 6C 61 79 65 64 20 6F 6E 20 69 74 20 6F 76 65 72 20 61 20 79 65 61 72 2E 20 57 68 65 6E 20 69 20 74 72 79 20 74 6F 20 6C 6F 67 69 6E 20 69 74 20 73 61 79 73 20 22 79 6F 75 20 63 61 6E 6E 6F 74 20 6C 6F 67 20 69 6E 20 75 6E 74 69 6C 20 74 68 65 20 63 68 61 72 61 63 74 65 72 20 75 70 64 61 74 65 20 70 72 6F 63 65 73 73 20 79 6F 75 20 72 65 63 65 6E 74 6C 79 20 69 6E 69 74 69 61 74 65 64 20 69 73 20 63 6F 6D 70 6C 65 74 65 64 22 2E 20 49 20 74 72 69 65 64 20 74 6F 20 67 6F 6F 67 6C 65 20 74 68 69 73 20 61 6E 64 20 66 69 6E 64 20 6F 75 74 20 77 68 61 74 20 69 73 20 68 61 70 70 65 6E 69 6E 67 2E 20 49 20 66 6F 75 6E 64 20 6F 75 74 20 74 68 61 74 20 69 20 69 73 20 70 72 6F 62 61 62 6C 79 20 70 65 6E 64 69 6E 67 20 73 6F 6D 65 20 70 61 69 64 20 73 65 72 76 69 63 65 2E 20 48 6F 77 20 64 6F 20 69 20 63 61 6E 63 65 6C 20 74 68 69 73 20 73 65 72 76 69 63 65 20 3F 20 41 6E 64 20 68 6F 77 20 64 6F 20 69 20 66 69 6E 64 20 77 68 69 63 68 20 73 65 72 76 69 63 65 20 69 73 20 70 65 6E 64 69 6E 67 20 3F 20 49 20 6A 75 73 74 20 77 61 6E 74 20 74 6F 20 75 73 65 20 74 68 61 20 63 68 61 72 20 61 67 61 69 6E 20 3A 28 00
11 00 00 00 
00 
15 00 00 00 
3A 02 00 00 39 02 00 00 39 02 00 00 70 00 00 00 69 00 00 00 
67 00 00 00 66 00 00 00 60 00 00 00 5C 00 00 00 54 00 00 00 
53 00 00 00 33 00 00 00 27 00 00 00 24 00 00 00 1B 00 00 00 
15 00 00 00 
0C 00 00 00 09 00 00 00 06 00 00 00 04 00 00 00 01 00 00 00 
08 11 00 00 78 9C ED 57 5B 6F E2 46 14 EE F3 FE 8A 93 A7 7D 49 D5 F1 95 31 AA 5A D9 D8 10 76 B9 09 9B A2 ED 2A 0F 13 7B C0 D3 D8 1E 34 1E 87 10 E5 AF F5 A9 7F AC 63 48 A2 A2 64 2D D9 4A A5 55 55 90 40 F6 1C 0E DF 77 BE 73 73 74 D8 D1 3E 7C D5 7A D7 97 30 48 49 51 D0 4C 5D 46 82 24 14 7E 84 01 93 07 75 B0 C8 C8 81 0A 98 91 BC B6 5D B3 2C 23 B9 E0 37 54 48 75 18 D2 22 51 87 A3 D5 D8 57 87 48 47 A7 97 61 D8 EE 10 EB CA C0 8D 25 BB A3 B0 3B 3A F9 A7 89 66 7B D8 77 6D 65 12 D1 7B A9 4E E0 31 DE 3C BD 1E AF 98 A4 79 DF 32 90 86 FA E7 6F AC F7 D1 63 FA 35 C8 15 80 92 65 B7 30 C8 B8 4C AF 1F D3 47 71 FD 21 6A CF 67 41 13 C1 4B 52 C4 E9 B7 D9 98 AE 6F 62 A3 1D 9B C9 10 3E C5 B0 08 A3 8B FF 12 2C 9F 3D FC F5 E7 B7 11 E9 BE 6D 0D 0C B7 15 A2 BD BC 81 37 A4 D7 35 6C 9D 09 AF 21 03 6B A6 D5 73 F4 7E 7D 52 E7 40 46 73 5A 48 56 E5 30 17 F4 98 02 E0 A0 ED 4F A5 24 F1 2D EC 4A D9 2D F2 24 95 E4 B6 21 EA 43 07 99 03 A7 65 D4 A7 B0 76 97 30 72 A7 41 78 59 87 1F 00 26 41 F4 31 04 7F 0E E1 7C 1A 80 BB 0C 66 2E 2C 7E 5B 28 58 D3 F9 0C 86 E6 E8 F7 CB FA 1A A6 EE E7 20 84 2F F3 15 B8 E0 05 51 14 2C 61 31 71 BF 04 CB 4E F4 C6 B1 60 B2 12 1B 12 D3 06 8E 81 E5 20 C3 6E C5 71 1D 85 27 1D 11 EA A1 24 79 D1 D1 C1 AF 4A F8 24 DF 42 B0 52 B2 82 C2 15 4B 9E D4 33 1D 67 04 94 D4 01 EA 44 CE CB 58 59 4A 7A DF D0 98 34 CB 74 7B 83 B6 35 E3 85 20 39 0C 04 D9 48 A0 3B 16 83 BF 08 E1 86 66 B2 7B 29 5D F1 7B 56 95 0D 48 FD DE C0 F0 FD B6 48 27 6B D8 33 99 9E A4 20 86 61 51 FA 2C 85 6D 59 E7 42 68 18 39 B6 A1 1B D8 7C D6 C4 17 64 CB 8B 32 26 19 85 09 DD 82 2B 72 2E BA 37 D7 90 57 59 99 12 29 A9 68 48 36 D3 C1 36 46 AD 88 FE BC B8 5B 40 A0 DC 16 EA 6F 7F 01 60 25 64 9C DF B2 62 0B 1B 2E 80 DE C7 74 27 19 2F 48 F6 E4 AD 84 7D CA 81 16 7F F0 03 D4 BF 25 25 E4 55 9C D6 DF 7B 0A 09 BF 50 05 39 AD 4A A9 44 85 9C A8 02 A1 40 8A 04 12 9A B0 98 48 9A 5C D6 66 25 95 50 ED 60 59 DF 00 6F F4 B1 3C DA B8 82 16 04 24 25 79 09 BC 00 5E 09 A8 E3 57 73 AD BD D6 C5 5E 63 52 81 A4 30 2E 36 FC E2 A2 5B C2 8C 38 61 24 6D C8 6D 4B F3 82 1E F2 5A 76 DF F2 8D EE 6B 23 6C E0 B3 54 51 0E 7A 4A 27 D3 B1 9F 53 65 7A 50 95 26 18 AF 4A 18 72 21 2B 55 C8 03 22 92 53 21 AB 38 B8 AB 41 34 56 8D EC 6A BE 0A 03 D0 EC 2D EC 68 B7 2C 5A 65 4A CA A6 42 41 A6 E7 B7 2B E9 F7 69 56 78 D4 AD F2 49 21 88 32 DD 36 68 69 0E 3D E4 58 41 4B 52 D1 1B A4 6C CD 71 CE B5 D4 34 0B EB A6 D3 D3 71 1F 1B 47 76 51 2A 28 05 BE 81 50 F2 82 96 27 7E AF 3D E9 48 33 CE 3D 59 A6 89 B1 ED 68 FA 8B A7 3D AF FD AC C9 5D 93 1B 64 9F B9 B1 D5 16 A3 63 BB A7 BF C2 23 53 0A 6B 56 24 27 5F 75 19 59 97 B8 3E D8 1F 6F FE 3F DF 5F 6F 68 82 E6 19 8F 1B BA 84 EE FA 36 6E C7 4F E1 8D AE 02 58 CE 5D 1F DC 91 3B 9E 7D 3F D0 C6 F0 69 A5 C2 BD 76 67 11 44 73 18 05 11 BC 0F DA 7F AB 4A 05 DD A9 76 49 93 4E A0 96 6A E0 90 8C 14 0D 90 7C 8C 3D CF 6A 05 49 72 0E 37 24 81 03 AF 20 61 C9 B1 EA 4A AE 66 E9 5E A8 CF 4E 40 D5 53 19 2F 1E 9A A4 76 5C CB 6B 37 AB 12 5E C8 7A 94 6E D5 18 56 DB 58 7C 88 33 D5 AD 14 D2 9C BE 4C 79 35 CF 09 24 A2 52 2C 58 01 7B 22 60 AB 00 9D 06 75 AA 9A D2 89 9B DA 90 D4 EC 3F 8E EC EF 27 93 67 F3 8E 8B C1 52 AA B9 5B 35 2D 58 6E 80 CD 76 29 F1 1E 03 D2 42 A8 DE E6 3B 71 9A AA FE 9E 91 5D D9 F0 90 62 A1 A1 E3 1B ED 16 E4 8D B9 7D F8 F5 98 E7 F9 D3 B2 A7 E9 D7 1F 7E F8 1B A7 46 13 DF 
//above is a compressed something that translates to this
Type: [17], Channel: [Trade - City], Player Name: [Willamrobert], Sender GUID: [020000000336AF82], Active player: [02000000016B8DA6], Text: [ |cffffffff|Hitem:53010:0:0:0:0:0:0:0:82:0|h[Embersilk Cloth]|h|r] Type: [17], Channel: [Trade - City], Player Name: [Pedrosanch], Sender GUID: [0200000004AD4832], Active player: [02000000016B8DA6], Text: [LF Jc PST!] Type: [17], Channel: [Trade - City], Player Name: [Pedrosanch], Sender GUID: [0200000004AD4832], Active player: [02000000016B8DA6], Text: [LF Jc PST!] Type: [17], Channel: [Trade - City], Player Name: [Dizý], Sender GUID: [0200000002D65C3A], Active player: [02000000016B8DA6], Text: [wtb |cffffffff|Hitem:52185:0:0:0:0:0:0:1038145792:85:0|h[Elementium Ore]|h|r 90g/stack pst] Type: [17], Channel: [Trade - City], Player Name: [Pahtak], Sender GUID: [0200000004F904C9], Active player: [02000000016B8DA6], Text: [LFM WAR GAMES, PST LET'S DO SOME ARENA PVP- CMON F4GZ, PVP MAKES YOU A BETTER PLAYER] Type: [17], Channel: [Trade - City], Player Name: [Icriturface], Sender GUID: [0200000004E59036], Active player: [02000000016B8DA6], Text: [WTS |cff0070dd|Hitem:52980:0:0:0:0:0:0:0:85:0|h[Pristine Hide]|h|r 499G ea PST] Type: [17], Channel: [Trade - City], Player Name: [Blisstex], Sender GUID: [0200000003154A7C], Active player: [02000000016B8DA6], Text: [LF BS to Craft epic DPS belt PST!] Type: [17], Channel: [Trade - City], Player Name: [Hoxius], Sender GUID: [0200000003D7C3DD], Active player: [02000000016B8DA6], Text: [LF LW with |cffa335ee|Hitem:56550:0:0:0:0:0:0:1809632384:85:0|h[Dragonscale Leg Armor]|h|r] Type: [17], Channel: [Trade - City], Player Name: [Soulshatter], Sender GUID: [0200000004498680], Active player: [02000000016B8DA6], Text: [ is looking for exceptional players who enjoy PvP as much as we do! Must be mature and dedicated, we set up Rated BG's and Arena teams on our calender! PST for more Info!!!] Type: [17], Channel: [Trade - City], Player Name: [Goaiahx], Sender GUID: [02000000051BE70B], Active player: [02000000016B8DA6], Text: [wts |cffffffff|Hitem:60838:0:0:0:0:0:0:2007498496:85:0|h[Mysterious Fortune Card]|h|r on AUCTION HOUSE 16g per] Type: [17], Channel: [Trade - City], Player Name: [Ultio], Sender GUID: [0200000003D04BDC], Active player: [02000000016B8DA6], Text: [WTS |cff0070dd|Hitem:52980:0:0:0:0:0:0:0:85:0|h[Pristine Hide]|h|r 498G] Type: [17], Channel: [Trade - City], Player Name: [Hanrahangx], Sender GUID: [0200000004FB095E], Active player: [02000000016B8DA6], Text: [WTT |cff0070dd|Hitem:61998:0:0:0:0:0:0:1158249728:83:0|h[Three of Stones]|h|r |cff0070dd|Hitem:62013:0:0:0:0:0:0:1544886912:83:0|h[Two of Waves]|h|r |cff0070dd|Hitem:62006:0:0:0:0:0:0:683228672:83:0|h[Three of the Winds]|h|rfor 5,8 of winds] Type: [17], Channel: [Trade - City], Player Name: [Pahtak], Sender GUID: [0200000004F904C9], Active player: [02000000016B8DA6], Text: [LFM WAR GAMES, PST LET'S DO SOME ARENA PVP- CMON F4GZ, PVP MAKES YOU A BETTER PLAYER] Type: [17], Channel: [Trade - City], Player Name: [Dremloc], Sender GUID: [02000000052AD689], Active player: [02000000016B8DA6], Text: [ON THE ROAD AGAIN] Type: [17], Channel: [Trade - City], Player Name: [Dremloc], Sender GUID: [02000000052AD689], Active player: [02000000016B8DA6], Text: [I JUST WANT TO GET ON THE ROAD AGAIN] Type: [17], Channel: [Trade - City], Player Name: [Hanrahangx], Sender GUID: [0200000004FB095E], Active player: [02000000016B8DA6], Text: [reported] Type: [17], Channel: [Trade - City], Player Name: [Randalan], Sender GUID: [0200000004D88BB5], Active player: [02000000016B8DA6], Text: [too bad you did the song wrong] Type: [17], Channel: [Trade - City], Player Name: [Cloonz], Sender GUID: [020000000529A5BB], Active player: [02000000016B8DA6], Text: [dont forget to cyclone some players as a druid in war games and have the switch teams] Type: [17], Channel: [Trade - City], Player Name: [Dremloc], Sender GUID: [02000000052AD689], Active player: [02000000016B8DA6], Text: [NO!!] Type: [17], Channel: [Trade - City], Player Name: [Rtardu], Sender GUID: [02000000044AE845], Active player: [02000000016B8DA6], Text: [WTS |cff0070dd|Hitem:52980:0:0:0:0:0:0:0:85:0|h[Pristine Hide]|h|r 500G ea] Type: [17], Channel: [Trade - City], Player Name: [Mindlapse], Sender GUID: [02000000050F9D3D], Active player: [02000000016B8DA6], Text: [f4gz? you must be 12] 
*/
	uint32 map;
	float x, y, z;
	std::string message = "";
	GM_Ticket *ticket = new GM_Ticket;

	// recv Data
	recv_data >> map;
	recv_data >> x;
	recv_data >> y;
	recv_data >> z;
	recv_data >> message; 
	//not using this atm
/*
	uint32 always17;
	uint32 somecounter;
	uint32 zip_size;
	std::string message2 = "";
	recv_data >> always17;
	recv_data >> message2;
	recv_data >> somecounter;
	for(uint32 i=0;i<somecounter;i++)
	{
		uint32 unkvalues;
		recv_data >> unkvalues;
	}
	recv_data >> zip_size;
	recv_data >> zip_content;
*/

	// Remove pending tickets
	objmgr.RemoveGMTicketByPlayer(GetPlayer()->GetGUID());

	ticket->guid = objmgr.GenerateTicketID();
	ticket->playerGuid = GetPlayer()->GetGUID();
	ticket->map = map;
	ticket->posX = x;
	ticket->posY = y;
	ticket->posZ = z;
	ticket->message = message;
	ticket->timestamp = (uint32)UNIXTIME;
	ticket->name = GetPlayer()->GetName();
	ticket->level = GetPlayer()->getLevel();
	ticket->deleted_by = 0;
	ticket->assignedToPlayer = 0;
	ticket->comment = "";

	// Add a new one
	objmgr.AddGMTicket(ticket, false);

	// Response - no errors
	sStackWorldPacket( data, SMSG_GMTICKET_CREATE, 10 );
	data << uint32(2);

	SendPacket(&data);

	// send message indicating new ticket
	Channel *chn = channelmgr.GetChannel(sWorld.getGmClientChannel().c_str(),GetPlayer());
	if(chn)
	{
		std::stringstream ss;
#ifdef GM_TICKET_MY_MASTER_COMPATIBLE
		ss << "GmTicket 5, " << ticket->name;
#else
		ss << "GmTicket:" << GM_TICKET_CHAT_OPCODE_NEWTICKET;
		ss << ":" << ticket->guid;
		ss << ":" << ticket->level;
		ss << ":" << ticket->name;
#endif
		chn->Say(_player, ss.str().c_str(), NULL, true);
	}
}

void WorldSession::HandleGMTicketUpdateOpcode(WorldPacket & recv_data)
{
	std::string message = "";
	sStackWorldPacket( data,SMSG_GMTICKET_UPDATETEXT, 10);

	// recv Data
	recv_data >> message;

	// Update Ticket
	GM_Ticket *ticket = objmgr.GetGMTicketByPlayer(GetPlayer()->GetGUID());
	if(!ticket) // Player doesnt have a GM Ticket yet
	{
		// Response - error couldnt find existing Ticket
		data << uint32(1);

		SendPacket(&data);
		return;
	}
	ticket->message = message;
	ticket->timestamp = (uint32)UNIXTIME;

	objmgr.UpdateGMTicket(ticket);

	// Response - no errors
//	data << uint32(2);
	data << uint32(4);	//this was sent in 403 client

	SendPacket(&data);

#ifndef GM_TICKET_MY_MASTER_COMPATIBLE
	Channel *chn = channelmgr.GetChannel(sWorld.getGmClientChannel().c_str(),GetPlayer());
	if(chn)
	{
		std::stringstream ss;
		ss << "GmTicket:" << GM_TICKET_CHAT_OPCODE_UPDATED;
		ss << ":" << ticket->guid;
		chn->Say(_player, ss.str().c_str(), NULL, true);
	}
#endif
}

void WorldSession::HandleGMTicketDeleteOpcode(WorldPacket & recv_data)
{
	GM_Ticket* ticket = objmgr.GetGMTicketByPlayer(GetPlayer()->GetGUID());

	// Remove Tickets from Player
	objmgr.RemoveGMTicketByPlayer(GetPlayer()->GetGUID());

	// Response - no errors
	sStackWorldPacket( data, SMSG_GMTICKET_DELETETICKET, 10);
	data << uint32(9);
	SendPacket(&data);

	// send message to gm_sync_chan
	Channel *chn = channelmgr.GetChannel(sWorld.getGmClientChannel().c_str(), GetPlayer());
	if(chn && ticket != NULL)
	{
		std::stringstream ss;
#ifdef GM_TICKET_MY_MASTER_COMPATIBLE
		ss << "GmTicket 1," << ticket->name;
#else
		ss << "GmTicket:" << GM_TICKET_CHAT_OPCODE_REMOVED;
		ss << ":" << ticket->guid;
#endif
		chn->Say(_player, ss.str().c_str(), NULL, true);
	}
}

void WorldSession::HandleGMTicketGetTicketOpcode(WorldPacket & recv_data)
{
	sStackWorldPacket( data, SMSG_GMTICKET_GETTICKET, 800);
	// no data

	// get Current Ticket
	GM_Ticket *ticket = objmgr.GetGMTicketByPlayer(GetPlayer()->GetGUID());

	if(!ticket) // no Current Ticket
	{
		data << uint32(10);
		SendPacket(&data);
		return;
	}
/*
13329
{SERVER} Packet: (0xF47F) SMSG_GMTICKET_GETTICKET PacketSize = 58 TimeStamp = 8054612
06 00 00 00 
96 91 5B 00 ?
77 68 61 74 20 68 61 70 70 65 6E 65 64 20 74 6F 20 6D 79 20 6F 74 68 65 72 20 74 69 63 6B 65 74 20 3F 00 - what happened to my other ticket ?
00 
45 2E 42 37 
7D 3C 33 49 
AE 52 A2 3A 
00 
00
*/
	// Send current Ticket
	data << uint32(6); // unk
	data << uint32( getMSTime() );	//not sure, maybe time passsed since creation ? Value seemed too large for it though :(
	data << ticket->message.c_str();
	data << uint8(0);	//? old one said map = ticket->map
	data << uint32(0);	//?
	data << uint32(0);	//?
	data << uint32(0);	//?
	data << uint8(0);	//?
	data << uint8(0);	//?

	SendPacket(&data);
}


void WorldSession::HandleGMTicketSystemStatusOpcode(WorldPacket & recv_data)
{
	sStackWorldPacket( data, SMSG_GMTICKET_SYSTEMSTATUS, 10);

	// no data

	// Response - System is working Fine
	if(sWorld.getGMTicketStatus())
		data << uint32(1);
	else
		data << uint32(0);

	SendPacket(&data);
}

void WorldSession::HandleGMTicketToggleSystemStatusOpcode(WorldPacket & recv_data)
{
	if(!HasGMPermissions())
		return;

	sWorld.toggleGMTicketStatus();
}
