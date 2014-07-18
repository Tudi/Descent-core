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

initialiseSingleton( AddonMgr );

//#define DEBUG_PRINT_ADDON_PACKET			// Prints out Received addon packet when char logging in

// hacky key
static uint8 PublicKey[265] = { 0x02, 0x01, 0x01, 0xC3, 0x5B, 0x50, 0x84, 0xB9, 0x3E, 0x32, 0x42, 0x8C, 0xD0, 0xC7, 0x48, 0xFA, 0x0E, 0x5D, 0x54, 0x5A, 0xA3, 0x0E, 0x14, 0xBA, 0x9E, 0x0D, 0xB9, 0x5D, 0x8B, 0xEE, 0xB6, 0x84, 0x93, 0x45, 0x75, 0xFF, 0x31, 0xFE, 0x2F, 0x64, 0x3F, 0x3D, 0x6D, 0x07, 0xD9, 0x44, 0x9B, 0x40, 0x85, 0x59, 0x34, 0x4E, 0x10, 0xE1, 0xE7, 0x43, 0x69, 0xEF, 0x7C, 0x16, 0xFC, 0xB4, 0xED, 0x1B, 0x95, 0x28, 0xA8, 0x23, 0x76, 0x51, 0x31, 0x57, 0x30, 0x2B, 0x79, 0x08, 0x50, 0x10, 0x1C, 0x4A, 0x1A, 0x2C, 0xC8, 0x8B, 0x8F, 0x05, 0x2D, 0x22, 0x3D, 0xDB, 0x5A, 0x24, 0x7A, 0x0F, 0x13, 0x50, 0x37, 0x8F, 0x5A, 0xCC, 0x9E, 0x04, 0x44, 0x0E, 0x87, 0x01, 0xD4, 0xA3, 0x15, 0x94, 0x16, 0x34, 0xC6, 0xC2, 0xC3, 0xFB, 0x49, 0xFE, 0xE1, 0xF9, 0xDA, 0x8C, 0x50, 0x3C, 0xBE, 0x2C, 0xBB, 0x57, 0xED, 0x46, 0xB9, 0xAD, 0x8B, 0xC6, 0xDF, 0x0E, 0xD6, 0x0F, 0xBE, 0x80, 0xB3, 0x8B, 0x1E, 0x77, 0xCF, 0xAD, 0x22, 0xCF, 0xB7, 0x4B, 0xCF, 0xFB, 0xF0, 0x6B, 0x11, 0x45, 0x2D, 0x7A, 0x81, 0x18, 0xF2, 0x92, 0x7E, 0x98, 0x56, 0x5D, 0x5E, 0x69, 0x72, 0x0A, 0x0D, 0x03, 0x0A, 0x85, 0xA2, 0x85, 0x9C, 0xCB, 0xFB, 0x56, 0x6E, 0x8F, 0x44, 0xBB, 0x8F, 0x02, 0x22, 0x68, 0x63, 0x97, 0xBC, 0x85, 0xBA, 0xA8, 0xF7, 0xB5, 0x40, 0x68, 0x3C, 0x77, 0x86, 0x6F, 0x4B, 0xD7, 0x88, 0xCA, 0x8A, 0xD7, 0xCE, 0x36, 0xF0, 0x45, 0x6E, 0xD5, 0x64, 0x79, 0x0F, 0x17, 0xFC, 0x64, 0xDD, 0x10, 0x6F, 0xF3, 0xF5, 0xE0, 0xA6, 0xC3, 0xFB, 0x1B, 0x8C, 0x29, 0xEF, 0x8E, 0xE5, 0x34, 0xCB, 0xD1, 0x2A, 0xCE, 0x79, 0xC3, 0x9A, 0x0D, 0x36, 0xEA, 0x01, 0xE0, 0xAA, 0x91, 0x20, 0x54, 0xF0, 0x72, 0xD8, 0x1E, 0xC7, 0x89, 0xD2, 0x00, 0x00, 0x00, 0x00, 0x00 };

AddonMgr::AddonMgr()
{
	KnownAddons.clear();
}

AddonMgr::~AddonMgr()
{
	std::map<std::string, AddonEntry*>::iterator itr;
	for(itr = KnownAddons.begin(); itr!=KnownAddons.end(); ++itr) 
	{
		delete itr->second;
		itr->second = NULL;
	}
	KnownAddons.clear();
}

bool AddonMgr::IsAddonBanned(uint64 crc, std::string name)
{
	return false;	// bleh needs work
}

bool AddonMgr::IsAddonBanned(std::string name, uint64 crc)
{
	std::map<std::string,AddonEntry*>::iterator i = KnownAddons.find(name);
	if(i != KnownAddons.end())
	{
		if(i->second->banned)
		{
			sLog.outDebug("Addon %s is banned.", name.c_str());
			return true;
		}
	}
	else
	{
		// New addon. It'll be saved to db at server shutdown.
		AddonEntry *ent = new AddonEntry;
		ent->name = name;
		ent->crc = crc;
		ent->banned = false;	// by default.. we can change this I guess..
		ent->isNew = true;
		ent->showinlist = true;

		sLog.outDebug("Discovered new addon %s sent by client.", name.c_str());

		KnownAddons[ent->name] = ent;
	}

	return false;
}

bool AddonMgr::ShouldShowInList(std::string name)
{
	std::map<std::string,AddonEntry*>::iterator i = KnownAddons.find(name);
	if(i != KnownAddons.end())
	{
		if(i->second->showinlist)
			return true;
		else
			return false;
	}
	else
	{
		// New addon. It'll be saved to db at server shutdown.		
		AddonEntry *ent = new AddonEntry;
		ent->name = name;
		ent->crc = 0;
		ent->banned = false;	// by default.. we can change this I guess..
		ent->isNew = true;
		ent->showinlist = true;

		sLog.outDebug("Discovered new addon %s sent by client.", name.c_str());

		KnownAddons[ent->name] = ent;
	}

	return true;
}

void AddonMgr::SendAddonInfoPacket(WorldPacket *source, uint32 pos, WorldSession *m_session)
{

//	sLog.outDebug( "AddonMgr::SendAddonInfoPacket -> disabled until properly implemented" );
//	return;
/*
{CLIENT} Packet: (0x880A) CMSG_AUTH_SESSION PacketSize = 305 stamp = 12016866
|------------------------------------------------|----------------|
|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
|------------------------------------------------|----------------|
|01 8C 11 34 9A 0A 00 00 00 00 00 00 00 00 4D ED |...4..........M.|
|43 00 00 00 00 54 FD 3D 5C 00 00 00 00 76 4F BF |C....T.=\....vO.|
|00 00 00 00 0F 8D 08 00 00 00 1F E3 F3 AD 00 1F |................|
|5E 35 AA 30 41 00 F7 00 00 00 3B 03 00 00 78 9C |^5.0A.....;...x.|
|75 D2 3B 6E C3 30 0C 06 60 1F A4 97 89 1D 34 08 |u.;n.0..`.....4.|
|5A 2F B5 3A 17 B4 C4 C8 84 25 D2 A0 E5 A0 C9 41 |Z/.:.....%.....A|
|7A DE 3C 86 A0 03 A5 F5 A3 08 90 3F DF 9A A6 69 |z.<........?...i|
|13 5D AF A0 E1 E7 B0 51 0A DF C7 E6 F5 5E 72 E4 |.].....Q.....^r.|
|75 41 5F 4C 7B FE EA 84 8B 4A 32 0B 76 7E 22 3C |uA_L{....J2.v~"<|
|63 46 B6 1B EC D4 4F 80 92 24 5E 2A 8E 0C B6 6C |cF....O..$^*...l|
|BE 90 B0 69 2D E8 88 BA 4E B2 54 B8 94 84 27 C2 |...i-...N.T...'.|
|14 7A 62 CA B0 58 45 C4 81 38 9A 0D 3A 48 C8 01 |.zb..XE..8..:H..|
|D4 22 C9 23 94 4F 89 55 73 F8 5B 6C 5C C0 97 2F |.".#.O.Us.[l\../|
|A0 F0 AE 90 71 35 6A F6 38 6E D1 89 24 0B 0F E9 |....q5j.8n..$...|
|B2 4C 76 4A 7D 37 41 25 C0 7E D8 F4 8C F6 F2 9F |.LvJ}7A%.~......|
|E9 B6 C0 B3 7D 17 05 F3 20 7E C6 52 5B 53 0F 5E |....}... ~.R[S.^|
|C5 94 C7 94 36 E0 49 34 D6 1A BA C7 DE ED 41 1C |....6.I4......A.|
|65 EC 81 21 A2 95 8A 93 19 ED 53 71 0A 01 87 99 |e..!......Sq....|
|92 7D BF 77 26 46 FD 6F E9 6F FF 71 03 9D 2A D4 |.}.w&F.o.o.q..*.|
|F7                                              |.               |
addons uncompressed
1C 00 00 00 
42 6C 69 7A 7A 61 72 64 5F 47 75 69 6C 64 55 49 00 
00 00 00 00 00 00 00 00 00 
42 6C 69 7A 7A 61 72 64 5F 49 6E 73 70 65 63 74 55 49 00 
00 00 00 00 00 00 00 00 00 
42 6C 69 7A 7A 61 72 64 5F 47 75 69 6C 64 43 6F 6E 74 72 6F 6C 55 49 00 
00 00 00 00 00 00 00 00 00 
42 6C 69 7A 7A 61 72 64 5F 41 63 68 69 65 76 65 6D 65 6E 74 55 49 00 
00 00 00 00 00 00 00 00 00 
42 6C 69 7A 7A 61 72 64 5F 41 72 63 68 61 65 6F 6C 6F 67 79 55 49 00 
00 00 00 00 00 00 00 00 00 
42 6C 69 7A 7A 61 72 64 5F 41 72 65 6E 61 55 49 00 
00 00 00 00 00 00 00 00 00 
42 6C 69 7A 7A 61 72 64 5F 41 75 63 74 69 6F 6E 55 49 00 
00 00 00 00 00 00 00 00 00 
42 6C 69 7A 7A 61 72 64 5F 42 61 72 62 65 72 73 68 6F 70 55 49 00 
00 00 00 00 00 00 00 00 00 
42 6C 69 7A 7A 61 72 64 5F 42 61 74 74 6C 65 66 69 65 6C 64 4D 69 6E 69 6D 61 70 00 
00 00 00 00 00 00 00 00 00 
42 6C 69 7A 7A 61 72 64 5F 42 69 6E 64 69 6E 67 55 49 00 
00 00 00 00 00 00 00 00 00 
42 6C 69 7A 7A 61 72 64 5F 43 61 6C 65 6E 64 61 72 00 00 00 00 00 00 00 00 00 00 42 6C 69 7A 7A 61 72 64 5F 43 6F 6D 62 61 74 4C 6F 67 00 00 00 00 00 00 00 00 00 00 42 6C 69 7A 7A 61 72 64 5F 43 6F 6D 62 61 74 54 65 78 74 00 00 00 00 00 00 00 00 00 00 42 6C 69 7A 7A 61 72 64 5F 43 6F 6D 70 61 63 74 52 61 69 64 46 72 61 6D 65 73 00 00 00 00 00 00 00 00 00 00 42 6C 69 7A 7A 61 72 64 5F 44 65 62 75 67 54 6F 6F 6C 73 00 00 00 00 00 00 00 00 00 00 42 6C 69 7A 7A 61 72 64 5F 47 6C 79 70 68 55 49 00 00 00 00 00 00 00 00 00 00 42 6C 69 7A 7A 61 72 64 5F 47 4D 43 68 61 74 55 49 00 00 00 00 00 00 00 00 00 00 42 6C 69 7A 7A 61 72 64 5F 47 4D 53 75 72 76 65 79 55 49 00 00 00 00 00 00 00 00 00 00 42 6C 69 7A 7A 61 72 64 5F 47 75 69 6C 64 42 61 6E 6B 55 49 00 00 00 00 00 00 00 00 00 00 42 6C 69 7A 7A 61 72 64 5F 49 74 65 6D 53 6F 63 6B 65 74 69 6E 67 55 49 00 00 00 00 00 00 00 00 00 00 42 6C 69 7A 7A 61 72 64 5F 4D 61 63 72 6F 55 49 00 00 00 00 00 00 00 00 00 00 42 6C 69 7A 7A 61 72 64 5F 52 61 69 64 55 49 00 00 00 00 00 00 00 00 00 00 42 6C 69 7A 7A 61 72 64 5F 52 65 66 6F 72 67 69 6E 67 55 49 00 00 00 00 00 00 00 00 00 00 42 6C 69 7A 7A 61 72 64 5F 54 61 6C 65 6E 74 55 49 00 00 00 00 00 00 00 00 00 00 42 6C 69 7A 7A 61 72 64 5F 54 69 6D 65 4D 61 6E 61 67 65 72 00 00 00 00 00 00 00 00 00 00 42 6C 69 7A 7A 61 72 64 5F 54 6F 6B 65 6E 55 49 00 00 00 00 00 00 00 00 00 00 42 6C 69 7A 7A 61 72 64 5F 54 72 61 64 65 53 6B 69 6C 6C 55 49 00 00 00 00 00 00 00 00 00 00 
42 6C 69 7A 7A 61 72 64 5F 54 72 61 69 6E 65 72 55 49 
00 00 00 00 00 00 00 00 00 
00 
6C 96 44 4B 
*/
	source->ResetRead();
//	source->read_skip( 58 );	//this might change from 1 client version to another. Simly see packet CMSG_AUTH_SESSION and check where the zip header starts
	source->read_skip( 53 );	//this is digest + crap 
	std::string account;
	uint32 addonsize;
	*source >> account;
//	source->read_skip( 4 );
	*source >> addonsize;
	if( addonsize == 0 || source->rpos() + 4 >= source->wpos() )
		return;

	WorldPacket returnpacket;
	returnpacket.Initialize(SMSG_ADDON_INFO);	// SMSG_ADDON_INFO

	uint32 realsize;
	uLongf rsize;

	*source >> realsize;

	if( realsize == 0 )
		return; // no addons ?

	rsize = MIN( realsize, source->wpos() * 100 );
	size_t position = source->rpos();

	ByteBuffer unpacked;
	unpacked.resize( rsize );

	if( source->rpos() + 4 >= source->wpos() )
	{
		// we shouldnt get here.. but just in case this will stop any crash here.
		sLog.outDebug("Warning: Incomplete auth session sent.");
		return;
	}

	int32 result;
	result = uncompress((uint8*)unpacked.contents(), &rsize, (uint8*)(*source).contents() + position, (uLong)((*source).size() - position));

	if(result != Z_OK)
	{
		sLog.outError("Decompression of addon section of CMSG_AUTH_SESSION failed.");
		return;
	}

	sLog.outDetail("Decompression of addon section of CMSG_AUTH_SESSION succeeded.");
	
	uint32 addoncount;
	unpacked >> addoncount;
	
	if( addoncount > 100 )
		return; //have a hunch they are fucking with us

	uint8 Enable; // based on the parsed files from retool
	uint32 crc;
	uint32 unknown;
	bool have_new_addons = false;
	
	std::string name;
	for (uint32 i=0; i<addoncount; ++i)
	{
		unpacked >> name;
		unpacked >> Enable;
		unpacked >> crc;
		unpacked >> unknown;
		
		// Hacky fix, Yea I know its a hacky fix I will make a proper handler one's I got the crc crap
		if (crc != 0x4C1C776D) // CRC of public key version 2.0.1
		{
			returnpacket.append(PublicKey,264); // part of the hacky fix
			have_new_addons = true;
		}
		else
			returnpacket << uint8(0x02) << uint8(0x01) << uint8(0x00) << uint32(0) << uint8(0);
		/*if(!AppendPublicKey(returnpacket, name, crc))
			returnpacket << uint8(1) << uint8(0) << uint8(0);*/

		if( unpacked.rpos() >= unpacked.wpos() )
			break;
	}

	if( have_new_addons == false )
	{
		returnpacket << uint32( 0 );	//this is a counter !
	}
	else
	{
		//i will figure this out later :P
		returnpacket.CopyFromSnifferDump("36 00 00 00"); //this is the row counter for below rows
		returnpacket.CopyFromSnifferDump("6F 01 00 00 C1 CF 2D D2 AC 53 7B D9 F0 9C 83 E0 AC BB DF 79 97 5E 2D F9 EA 33 45 93 11 B0 6D 50 9D E9 AF F9 6C A3 44 4B 01 00 00 00 ");
		returnpacket.CopyFromSnifferDump("70 01 00 00 C1 CF 2D D2 AC 53 7B D9 F0 9C 83 E0 AC BB DF 79 80 4F EA D6 B4 B0 C7 16 8C 9B 81 DB 62 6E FE A5 6C A3 44 4B 01 00 00 00 ");
		returnpacket.CopyFromSnifferDump("71 01 00 00 C1 CF 2D D2 AC 53 7B D9 F0 9C 83 E0 AC BB DF 79 AC 6F 6B 32 29 A2 6F C8 ED 6E 9C 5F 72 F2 5E 8A 6C A3 44 4B 01 00 00 00 ");
		returnpacket.CopyFromSnifferDump("72 01 00 00 C1 CF 2D D2 AC 53 7B D9 F0 9C 83 E0 AC BB DF 79 A4 CB 69 29 05 DE 25 D3 4C 84 68 79 88 7D 05 53 6C A3 44 4B 01 00 00 00 ");
		returnpacket.CopyFromSnifferDump("73 01 00 00 C1 CF 2D D2 AC 53 7B D9 F0 9C 83 E0 AC BB DF 79 BE 03 A1 9F AC 0A 05 9C 3E E0 C2 E3 57 C5 70 BC 6C A3 44 4B 01 00 00 00 ");
		returnpacket.CopyFromSnifferDump("74 01 00 00 C1 CF 2D D2 AC 53 7B D9 F0 9C 83 E0 AC BB DF 79 9E 96 AE 11 AA FB 49 DC DC F7 A0 26 68 1D 34 AA 6C A3 44 4B 01 00 00 00 ");
		returnpacket.CopyFromSnifferDump("75 01 00 00 C1 CF 2D D2 AC 53 7B D9 F0 9C 83 E0 AC BB DF 79 4B C4 6A FA E2 54 47 85 6A D4 0B 8F 77 F4 DA 9E 6C A3 44 4B 01 00 00 00 ");
		returnpacket.CopyFromSnifferDump("76 01 00 00 C1 CF 2D D2 AC 53 7B D9 F0 9C 83 E0 AC BB DF 79 19 20 F3 B4 2C C2 10 4D A4 DD 77 28 FD 76 55 C0 6C A3 44 4B 01 00 00 00 ");
		returnpacket.CopyFromSnifferDump("77 01 00 00 C1 CF 2D D2 AC 53 7B D9 F0 9C 83 E0 AC BB DF 79 98 57 02 43 43 2B A8 0A 0F BB 3B BA 3E C6 D7 77 6C A3 44 4B 01 00 00 00 ");
		returnpacket.CopyFromSnifferDump("78 01 00 00 C1 CF 2D D2 AC 53 7B D9 F0 9C 83 E0 AC BB DF 79 A0 D2 5C 9A 6E B5 F8 3D 01 EE FB 7C 35 03 7E 05 6C A3 44 4B 01 00 00 00 ");
		returnpacket.CopyFromSnifferDump("79 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
		returnpacket.CopyFromSnifferDump("7A 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
		returnpacket.CopyFromSnifferDump("7B 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
		returnpacket.CopyFromSnifferDump("7C 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
		returnpacket.CopyFromSnifferDump("7D 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
		returnpacket.CopyFromSnifferDump("7E 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
		returnpacket.CopyFromSnifferDump("7F 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
		returnpacket.CopyFromSnifferDump("80 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
		returnpacket.CopyFromSnifferDump("81 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
		returnpacket.CopyFromSnifferDump("82 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
		returnpacket.CopyFromSnifferDump("83 01 00 00 C1 CF 2D D2 AC 53 7B D9 F0 9C 83 E0 AC BB DF 79 97 5E 2D F9 EA 33 45 93 11 B0 6D 50 9D E9 AF F9 7C A4 44 4B 01 00 00 00 ");
		returnpacket.CopyFromSnifferDump("84 01 00 00 C1 CF 2D D2 AC 53 7B D9 F0 9C 83 E0 AC BB DF 79 80 4F EA D6 B4 B0 C7 16 8C 9B 81 DB 62 6E FE A5 7C A4 44 4B 01 00 00 00 ");
		returnpacket.CopyFromSnifferDump("85 01 00 00 C1 CF 2D D2 AC 53 7B D9 F0 9C 83 E0 AC BB DF 79 AC 6F 6B 32 29 A2 6F C8 ED 6E 9C 5F 72 F2 5E 8A 7C A4 44 4B 01 00 00 00 ");
		returnpacket.CopyFromSnifferDump("86 01 00 00 C1 CF 2D D2 AC 53 7B D9 F0 9C 83 E0 AC BB DF 79 A4 CB 69 29 05 DE 25 D3 4C 84 68 79 88 7D 05 53 7C A4 44 4B 01 00 00 00 ");
		returnpacket.CopyFromSnifferDump("87 01 00 00 C1 CF 2D D2 AC 53 7B D9 F0 9C 83 E0 AC BB DF 79 BE 03 A1 9F AC 0A 05 9C 3E E0 C2 E3 57 C5 70 BC 7C A4 44 4B 01 00 00 00 ");
		returnpacket.CopyFromSnifferDump("88 01 00 00 C1 CF 2D D2 AC 53 7B D9 F0 9C 83 E0 AC BB DF 79 9E 96 AE 11 AA FB 49 DC DC F7 A0 26 68 1D 34 AA 7C A4 44 4B 01 00 00 00 ");
		returnpacket.CopyFromSnifferDump("89 01 00 00 C1 CF 2D D2 AC 53 7B D9 F0 9C 83 E0 AC BB DF 79 4B C4 6A FA E2 54 47 85 6A D4 0B 8F 77 F4 DA 9E 7C A4 44 4B 01 00 00 00 ");
		returnpacket.CopyFromSnifferDump("8A 01 00 00 C1 CF 2D D2 AC 53 7B D9 F0 9C 83 E0 AC BB DF 79 19 20 F3 B4 2C C2 10 4D A4 DD 77 28 FD 76 55 C0 7C A4 44 4B 01 00 00 00 ");
		returnpacket.CopyFromSnifferDump("8B 01 00 00 C1 CF 2D D2 AC 53 7B D9 F0 9C 83 E0 AC BB DF 79 98 57 02 43 43 2B A8 0A 0F BB 3B BA 3E C6 D7 77 7C A4 44 4B 01 00 00 00 ");
		returnpacket.CopyFromSnifferDump("8C 01 00 00 C1 CF 2D D2 AC 53 7B D9 F0 9C 83 E0 AC BB DF 79 A0 D2 5C 9A 6E B5 F8 3D 01 EE FB 7C 35 03 7E 05 7C A4 44 4B 01 00 00 00 ");
		returnpacket.CopyFromSnifferDump("8D 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
		returnpacket.CopyFromSnifferDump("8E 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
		returnpacket.CopyFromSnifferDump("8F 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
		returnpacket.CopyFromSnifferDump("90 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
		returnpacket.CopyFromSnifferDump("91 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
		returnpacket.CopyFromSnifferDump("92 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
		returnpacket.CopyFromSnifferDump("93 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
		returnpacket.CopyFromSnifferDump("94 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
		returnpacket.CopyFromSnifferDump("95 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
		returnpacket.CopyFromSnifferDump("96 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
		returnpacket.CopyFromSnifferDump("97 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
		returnpacket.CopyFromSnifferDump("98 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
		returnpacket.CopyFromSnifferDump("99 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
		returnpacket.CopyFromSnifferDump("9A 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
		returnpacket.CopyFromSnifferDump("9B 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
		returnpacket.CopyFromSnifferDump("9C 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
		returnpacket.CopyFromSnifferDump("9D 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
		returnpacket.CopyFromSnifferDump("9E 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
		returnpacket.CopyFromSnifferDump("9F 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
		returnpacket.CopyFromSnifferDump("A0 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
		returnpacket.CopyFromSnifferDump("A1 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
		returnpacket.CopyFromSnifferDump("A2 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
		returnpacket.CopyFromSnifferDump("A3 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
		returnpacket.CopyFromSnifferDump("A4 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4B A3 44 4B 02 00 00 00 ");
	}
	m_session->SendPacket(&returnpacket);
}

bool AddonMgr::AppendPublicKey(WorldPacket& data, string AddonName, uint32 CRC)
{
	if(CRC == 0x4C1C776D)
	{
		// Open public key file with that addon
		map<string, ByteBuffer>::iterator itr = AddonData.find(AddonName);
		if(itr != AddonData.end())
			data.append(itr->second);
		else
		{
			// open the file
			char path[500];
			snprintf(path, 500, "addons\\%s.pub", AddonName.c_str());
			FILE * f = fopen(path, "rb");
			if(f != 0)
			{
				// read the file into a bytebuffer
				ByteBuffer buf;
				fseek(f, 0, SEEK_END);
				uint32 length = 264/*ftell(f)*/;
				fseek(f, 0, SEEK_SET);
				buf.resize(length);
				fread((void*)buf.contents(), length, 1, f);
				fclose(f);

				AddonData[AddonName] = buf;
				data.append(buf);
			}
			else
			{
				ByteBuffer buf;
				buf.append(PublicKey, 264);
				AddonData[AddonName] = buf;
				data.append(buf);
			}
		}
		return true;
	}
	return false;
}

void AddonMgr::LoadFromDB()
{
	QueryResult *result = WorldDatabase.Query("SELECT * FROM clientaddons");
	if(!result)
	{
		sLog.outString("Query failed: SELECT * FROM clientaddons");
		return;
	}

	do 
	{
		Field *field = result->Fetch();
		AddonEntry *ent = new AddonEntry;

		ent->name = field[1].GetString();
		ent->crc = field[2].GetUInt64();
		ent->banned = (field[3].GetUInt32()>0? true:false);
		ent->isNew = false;
		if(result->GetFieldCount() == 5)				// To avoid crashes for stilly nubs who don't update table :P
			ent->showinlist = (field[4].GetUInt32()>0 ? true : false);

		KnownAddons[ent->name] = ent;
	} while(result->NextRow());

	delete result;
	result = NULL;
}

void AddonMgr::SaveToDB()
{
	sLog.outString("AddonMgr: Saving any new addons discovered in this session to database.");
	for(std::map<std::string, AddonEntry*>::iterator itr = KnownAddons.begin();itr!=KnownAddons.end();++itr)
	{
		if(itr->second->isNew)
		{
			sLog.outDetail("Saving new addon %s", itr->second->name.c_str());
			std::stringstream ss;
			ss << "INSERT INTO clientaddons (name, crc, banned, showinlist) VALUES(\""
				<< WorldDatabase.EscapeString(itr->second->name) << "\",\""
				<< itr->second->crc << "\",\""
				<< itr->second->banned << "\",\""
				<< itr->second->showinlist << "\");";

			WorldDatabase.Execute(ss.str().c_str());
		}
	}
}
