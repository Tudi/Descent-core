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
#include "AuthCodes.h"
#include "svn_revision.h"

LoginErrorCode VerifyName(const char * name, size_t nlen)
{
	const char * p;
	size_t i;

	static const char * bannedCharacters = "\t\v\b\f\a\n\r\\\"\'\? <>[](){}_=+-|/!@#$%^&*~`.,0123456789\0";
	static const char * allowedCharacters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

	if( sWorld.m_limitedNames )
	{
		if( nlen == 0 )
			return CHAR_NAME_NO_NAME;
		else if( nlen < 2 )
			return CHAR_NAME_TOO_SHORT;
		else if( nlen > 12 )
			return CHAR_NAME_TOO_LONG;

		for( i = 0; i < nlen; ++i )
		{
			p = allowedCharacters;
			for( ; *p != 0; ++p )
			{
				if( name[i] == *p )
					goto cont;
			}
			return CHAR_NAME_INVALID_CHARACTER;
cont:
			continue;
		}
	}
	else
	{
		for(i = 0; i < nlen; ++i)
		{
			p = bannedCharacters;
			while(*p != 0 && name[i] != *p && name[i] != 0)
				++p;

			if(*p != 0)
				return CHAR_NAME_INVALID_CHARACTER;
		}
	}
	
	return CHAR_NAME_SUCCESS;
}

bool ChatHandler::HandleRenameAllCharacter(const char * args, WorldSession * m_session)
{
	uint32 uCount = 0;
	uint32 ts = getMSTime();
	QueryResult * result = CharacterDatabase.Query("SELECT guid, name FROM characters");
	if( result )
	{
		do 
		{
			uint32 uGuid = result->Fetch()[0].GetUInt32();
			const char * pName = result->Fetch()[1].GetString();
			size_t szLen = strlen(pName);

			if( VerifyName(pName, szLen) != CHAR_NAME_SUCCESS )
			{
				printf("renaming character %s, %u\n", pName,uGuid);
                Player * pPlayer = objmgr.GetPlayer(uGuid);
				if( pPlayer != NULL )
				{
					pPlayer->rename_pending = true;
					pPlayer->GetSession()->SystemMessage("Your character has had a force rename set, you will be prompted to rename your character at next login in conformance with server rules.");
				}

				CharacterDatabase.WaitExecute("UPDATE characters SET forced_rename_pending = 1 WHERE guid = %u", uGuid);
				++uCount;
			}

		} while (result->NextRow());
		delete result;
		result = NULL;
	}

	SystemMessage(m_session, "Procedure completed in %u ms. %u character(s) forced to rename.", getMSTime() - ts, uCount);
	return true;
}

void CapitalizeString(string& arg)
{
	if(arg.length() == 0) 
	{ 
		return;
	}
	arg[0] = toupper(arg[0]);
	for(uint32 x = 1; x < arg.size(); ++x)
		arg[x] = tolower(arg[x]);
}

void WorldSession::CharacterEnumProc(QueryResult * result)
{
	struct player_item
	{
		uint32 displayid;
		uint8 invtype;
		uint32 enchantment; // added in 2.4
	};

	player_item items[INVENTORY_SLOT_BAG_END];
	int8 slot;
	int8 containerslot;
	uint32 i;
	ItemPrototype * proto;
	QueryResult * res;
	uint32 num = 0;
	uint8 race;
	MaxAvailCharLevel=0;
	_side = -1; // side should be set on every enumeration for safety

	// should be more than enough.. 200 bytes per char..
	uint32 packet_size;
	if( result )
		packet_size = result->GetRowCount() * 200;
	else 
		packet_size = 600; // 3 chars ?
	if( packet_size == 0 )
		packet_size = 600; // 3 chars ?
/*
{SERVER} Packet: (0xC727) UNKNOWN PacketSize = 282 TimeStamp = 2453396
80 
00 00 00 00 
01 00 00 00 
D1 38 = 11010001 111000
00 04 01 81 01 00 00 00 00 00 00 00 00 98 01 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 1E 82 00 00 00 00 00 00 05 00 00 00 00 00 00 00 00 00 A4 26 00 00 00 00 00 00 07 9D 27 00 00 00 00 00 00 08 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4C 09 00 00 00 00 00 00 11 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 07 A4 01 48 4A 0B C6 B4 E8 05 C3 5B 13 A1 42 53 4E 65 77 77 61 72 00 00 00 00 00 00 00 00 00 00 00 00 00 03 00

80 - saint number
00 00 00 00 loop counter ?
02 00 00 00 - number of chars = 2*17 = 34 bits = 5 bytes
91 38 68 9C 00 - guid mask = 10010001	111000 , 1101000 10011100 ( second byte first bit is firstlogin ? )
04 hairstuff
01 gender
81 unk
02 level
0C 00 00 00 zone
00 00 00 00 pet 
98 guid[1]
01 race
02 guid[3]
00 00 A0 10 flags
00 00 00 00 petFamily
00 hair
00 order
00 ?
00 00 00 00 00 00 00 00 00 itemlook, enchant, type
00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00  
1E 82 00 00 00 00 00 00 05 
00 00 00 00 00 00 00 00 00 
A4 26 00 00 00 00 00 00 07 
9D 27 00 00 00 00 00 00 08 
00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 
4C 09 00 00 00 00 00 00 11 
00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 
07 face
A4 guid[2]
01 class
15 3F 0B C6 x
53 F7 07 C3 y
1A DD A0 42 z
53 guid[0]
4E 65 77 77 61 72 00 name
00 00 00 00 map
00 00 00 00 petlevel
04 80 00 10 char customization flags
03 facial hair
00 skin

09 hairstuff
00 gender
81 g[6]
01 level
00 00 00 00 zone
00 00 00 00 pet
CF guid[1]
01 race
02 guid[3]
00 00 00 00 flags
00 00 00 00 petfamily
06 hair
00 order
00 ?
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
87 31 00 00 00 00 00 00 14 00 00 00 00 00 00 00 00 00 D9 26 00 00 00 00 00 00 07 DA 26 00 00 00 00 00 00 08
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 D8 01 00 00 00 00 00 00 11
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00
0A face
A4 guid[2]
05 class
48 4A 0B C6 x
B4 E8 05 C3 y
5B 13 A1 42 z
A1 guid[0]
4E 65 77 70 72 69 73 74 00 name
00 00 00 00 map
00 00 00 00 petlevel
00 00 00 00 char customization flags
07 facial
05 skin

UNIT_FIELD_BYTES_0 = 2049 = 0x0801
UNIT_FIELD_FLAGS = 8 
UNIT_FIELD_FLAGS_2 = 2048 
PLAYER_FLAGS = 268435456 
PLAYER_BYTES = 84151048 = 0x05040B08
PLAYER_BYTES_2 = 33554437 = 0x02000005
PLAYER_FIELD_BYTES = 8 
68 9C 00 = 1101000 10011100 - mask  for guid = 03A8C924 01800000
05 hairstuff ( confirmed )
00 gender
81 - go6
01 level
00 00 00 00 zone
00 00 00 00 pet
C8 - go1
01 race
02 - go3
00 00 00 00 flags
00 00 00 00 petfamily
04 hair ( confirmed )
00 ?
00 ?
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 items
67 31 00 00 00 00 00 00 14 00 00 00 00 00 00 00 00 00 5A 07 00 00 00 00 00 00 07 C9 26 00 00 00 00 00 00 08 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 D8 01 00 00 00 00 00 00 11 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
0B face
A9 go2
08 class
48 4A 0B C6 x 
B4 E8 05 C3 y
5B 13 A1 42 z
25 go0
4E 65 77 6D 61 67 69 63 00 name
00 00 00 00 map
00 00 00 00 petlevel
00 00 00 00 flags
05 facial
08 skin
*/

//#define COPY_PASTE_BLIZZ_PACKET

	//make 2 packets then merge at the end
	WorldPacket part2_data(SMSG_CHAR_ENUM, packet_size );	
	WorldPacket part1_data(SMSG_CHAR_ENUM, packet_size );	
#ifndef COPY_PASTE_BLIZZ_PACKET
    uint8 byte_mask = 0;	//!guid mask is on 17 bits not 16!
    uint8 curPos = 0;
#endif
	part1_data << uint8( 0x80 );	//some client command
	part1_data << uint32( 0 );
	part1_data << num;
	uint8 EnumOrder = 2;
	if( result )
	{
		uint64 guid;
		uint8 Class;
		uint32 bytes2;
		uint32 flags;
		uint32 banned;
		uint32 ForceRename;
		Field *fields;
		do
		{
			fields = result->Fetch();
			guid = fields[0].GetUInt64();
			uint8 guidb[8];
			*(uint64*)guidb = guid;
			guidb[6] = EnumOrder++;	//this is a big hack to be able to get guids that contain "1"s These get eaten by the client due to obfuscation


			bytes2 = fields[6].GetUInt32();
			Class = fields[3].GetUInt8();			
			flags = fields[17].GetUInt32();
			race = fields[2].GetUInt8();
			uint32 PLAYER_BYTES = fields[5].GetUInt32();
			ForceRename = fields[16].GetUInt8();

			if( _side < 0 )
			{
				// work out the side
				static uint8 sides[RACE_COUNT] = { 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0 };
				_side = sides[race];
			}

			part1_data.writeBits( 0x0091, 8 );
			part1_data.writeBits( 0x0038, 8 );
			part1_data.writeBits( 0x0000, 1 );

		    part2_data << uint8(PLAYER_BYTES >> 24);		// Hair color
			part2_data << fields[4].GetUInt8();				// gender
			part2_data << ( guidb[6] );

			uint8 tl = fields[1].GetUInt8();
			if( tl > MaxAvailCharLevel )
				MaxAvailCharLevel = tl;
			part2_data << tl;								// Level

			part2_data << fields[12].GetUInt32();			// zoneid

			uint32 petDisplayId = 0,petFamily = 0;
			uint32 petLevel = 0;
			if( Class == WARLOCK || Class == HUNTER )
			{
				res = CharacterDatabase.Query("SELECT entry FROM playerpets WHERE ownerguid="I64FMTD" AND ( active MOD 10 ) =1", guid);
				if(res)
				{
					CreatureInfo *info = CreatureNameStorage.LookupEntry(res->Fetch()[0].GetUInt32());
					delete res;
					res = NULL;
					if( info )
					{
						petDisplayId = info->Male_DisplayID;
						petFamily = info->Family;
						petLevel = 1;
					}
				}
			}

			part2_data << petDisplayId;
			part2_data << ( guidb[1] );
			part2_data << race;						// race
			part2_data << ( guidb[3] );

			uint32 char_flags = 0;	
//			if (playerFlags & PLAYER_FLAG_NOHELM)
//				char_flags |= CHARACTER_FLAG_HIDE_HELM;	
//			if (playerFlags & PLAYER_FLAG_NOCLOAK)
//				char_flags |= CHARACTER_FLAG_HIDE_CLOAK;	
			if ( fields[15].GetUInt32() != 0 )
				char_flags |= CHARACTER_FLAG_GHOST;
			banned = fields[13].GetUInt32();
			if(banned && (banned<10 || banned > (uint32)UNIXTIME))
				char_flags |= CHARACTER_FLAG_LOCKED_BY_BILLING;
			if( ForceRename )
				char_flags |= CHARACTER_FLAG_RENAME;
			part2_data << char_flags; //default 0x10A00000

			part2_data << petFamily;
		    part2_data << uint8(PLAYER_BYTES >> 16);      // Hair style
			part2_data << uint8( 0 );	//?
			part2_data << uint8( 0 );	//go7

			res = CharacterDatabase.Query("SELECT containerslot, slot, entry, enchantments FROM playeritems WHERE ownerguid=%u and containerslot=-1 and slot < 23", GUID_LOPART(guid));

			memset(items, 0, sizeof(items));
			uint32 enchantid;
			EnchantEntry * enc;
			if(res)
			{
				do 
				{
					containerslot = res->Fetch()[0].GetInt8();
					slot = res->Fetch()[1].GetInt8();
					if( containerslot == -1 && slot < INVENTORY_SLOT_BAG_END && slot >= EQUIPMENT_SLOT_START )
					{
						proto = ItemPrototypeStorage.LookupEntry(res->Fetch()[2].GetUInt32());
						if( proto )
						{
							if( !( slot == EQUIPMENT_SLOT_HEAD && ( flags & ( uint32 )PLAYER_FLAG_NOHELM ) != 0 ) && 
								!( slot == EQUIPMENT_SLOT_BACK && ( flags & ( uint32 )PLAYER_FLAG_NOCLOAK ) != 0 ) ) 
							{
								items[slot].displayid = proto->DisplayInfoID;
								items[slot].invtype = proto->InventoryType;
								// weapon glows
								if( slot == EQUIPMENT_SLOT_MAINHAND || slot == EQUIPMENT_SLOT_OFFHAND )
								{
									// get enchant visual ID
									const char * enchant_field = res->Fetch()[3].GetString();	
									if( sscanf( enchant_field , "%u,0,0;" , (unsigned int *)&enchantid ) == 1 && enchantid > 0 )
									{
										enc = dbcEnchant.LookupEntry( enchantid );
										if( enc != NULL )
											items[slot].enchantment = enc->visual;
										else
											items[slot].enchantment = 0;;
									}
								}
							}
						}
					}
				} while(res->NextRow());
				delete res;
				res = NULL;
			}

			for( i = 0; i < INVENTORY_SLOT_BAG_END; ++i ) //23 * 5 bytes
				part2_data << items[i].displayid << uint32(items[i].enchantment) << items[i].invtype;

		    part2_data << uint8(PLAYER_BYTES >> 8);      // face
			part2_data << ( guidb[2] );
			part2_data << Class;						// class
			part2_data << fields[8].GetFloat();		// X
			part2_data << fields[9].GetFloat();		// Y
			part2_data << fields[10].GetFloat();		// Z
			part2_data << ( guidb[0] );
			part2_data << fields[7].GetString();		// name
			part2_data << fields[11].GetUInt32();		// Mapid
			part2_data << petLevel;
			part2_data << uint32(CHAR_CUSTOMIZE_FLAG_NONE);
			part2_data << uint8(bytes2 & 0xFF);		// facial hair
		    part2_data << uint8(PLAYER_BYTES & 0xFF);      // skin
			num++;
		}
		while( result->NextRow() );
	}
	part1_data.flushBits();
	part1_data.put<uint32>(5, num);
	part1_data << part2_data;

	//Log.Debug("Character Enum", "Built in %u ms.", getMSTime() - start_time);
	SendPacket( &part1_data );
}

void WorldSession::HandleCharEnumOpcode( WorldPacket & recv_data )
{	
	AsyncQuery * q = new AsyncQuery( new SQLClassCallbackP1<World, uint32>(World::getSingletonPtr(), &World::CharacterEnumProc, GetAccountId()) );
//	q->AddQuery("SELECT guid, level, race, class, gender, bytes, bytes2, name, positionX, positionY, positionZ, mapId, zoneId, banned, restState, deathstate, forced_rename_pending, player_flags, guild_data.guildid FROM characters LEFT JOIN guild_data ON characters.guid = guild_data.playerid WHERE acct=%u ORDER BY guid LIMIT 0,10", GetAccountId());
	q->AddQuery("SELECT guid, level, race, class, gender, bytes, bytes2, name, positionX, positionY, positionZ, mapId, zoneId, banned, restState, deathstate, forced_rename_pending, player_flags FROM characters WHERE acct=%u ORDER BY guid LIMIT 0,10", GetAccountId());
	CharacterDatabase.QueueAsyncQuery(q);
}

void WorldSession::LoadAccountDataProc(QueryResult * result)
{
	size_t len;
	const char * data;
	char * d;

	if(!result)
	{
		CharacterDatabase.Execute("INSERT INTO account_data VALUES(%u, '', '', '', '', '', '', '', '', '')", _accountId);
		return;
	}

	for(uint32 i = 0; i < 7; ++i)
	{
		data = result->Fetch()[1+i].GetString();
		len = data ? strlen(data) : 0;
		if(len > 1)
		{
			d = new char[len+1];
			memcpy(d, data, len+1);
			SetAccountData(i, d, true, (uint32)len);
		}
	}
}

void WorldSession::HandleCharCreateOpcode( WorldPacket & recv_data )
{
	CHECK_PACKET_SIZE(recv_data, 10);
	std::string name;
	uint8 race, class_;

	recv_data >> name >> race >> class_;
	recv_data.rpos(0);

	LoginErrorCode res = VerifyName( name.c_str(), name.length() );
	if( res != CHAR_NAME_SUCCESS )
	{
		OutPacket( SMSG_CHAR_CREATE, 1, &res );
		return;
	}
	
	res = g_characterNameFilter->Parse( name, false ) ? CHAR_NAME_PROFANE : CHAR_NAME_SUCCESS;
	if( res != CHAR_NAME_SUCCESS )
	{
		OutPacket( SMSG_CHAR_CREATE, 1, &res );
		return;
	}
	
	res = objmgr.GetPlayerInfoByName(name.c_str()) == NULL ? CHAR_CREATE_SUCCESS : CHAR_CREATE_NAME_IN_USE;
	if( res != CHAR_CREATE_SUCCESS )
	{
		OutPacket( SMSG_CHAR_CREATE, 1, &res );
		return;
	}
	res = sHookInterface.OnNewCharacter( race, class_, this, name.c_str() ) ? CHAR_CREATE_SUCCESS : CHAR_CREATE_ERROR;
	if( res != CHAR_CREATE_SUCCESS )
	{
		OutPacket( SMSG_CHAR_CREATE, 1, &res );
		return;
	}

	// check that the account can create TBC characters
	if( ( race == RACE_BLOODELF || race == RACE_DRAENEI ) && !HasFlag(ACCOUNT_FLAG_XPACK_01 | ACCOUNT_FLAG_XPACK_02) )
	{
		res=CHAR_CREATE_EXPANSION;
		OutPacket(SMSG_CHAR_CREATE, 1, &res);
		return;
	}

	if( class_ == DEATHKNIGHT && !HasFlag(ACCOUNT_FLAG_XPACK_02) )
	{
		res = CHAR_CREATE_EXPANSION;  //how on earth ? we did not even enable the menu for client. Hacker !
		OutPacket(SMSG_CHAR_CREATE, 1, &res );
		return;
	}

/*	if( class_ == DEATHKNIGHT && MaxAvailCharLevel < REQUIRED_LEVEL_TO_MAKE_DK)
	{
		res = CHAR_CREATE_LEVEL_REQUIREMENT; 
		OutPacket(SMSG_CHAR_CREATE, 1, &res );
		return;
	}*/

	QueryResult * result = CharacterDatabase.Query("SELECT COUNT(*) FROM banned_names WHERE name = '%s'", CharacterDatabase.EscapeString(name).c_str());
	if(result)
	{
		if(result->Fetch()[0].GetUInt32() > 0)
		{
			// That name is banned!
			res = CHAR_NAME_PROFANE;
			OutPacket( SMSG_CHAR_CREATE, 1, &res );
			delete result;
			result = NULL;
			return;
		}
		delete result;
		result = NULL;
	}
	// loading characters
	
	//checking number of chars is useless since client will not allow to create more than 10 chars
	//as the 'create' button will not appear (unless we want to decrease maximum number of characters)
	//Zack : well i could create a bot to create a lot of chars
	result = CharacterDatabase.Query( "SELECT COUNT(*) FROM characters WHERE acct = %u", GetAccountId() );
	if( result )
	{
		if( result->Fetch()[0].GetUInt32() >= 10 )
		{
			// We can't make any more characters.
			res = CHAR_CREATE_SERVER_LIMIT;
			OutPacket( SMSG_CHAR_CREATE, 1, &res );
			delete result;
			result = NULL;
			return;
		}
		delete result;
		result = NULL;
	}

	Player * pNewChar = objmgr.CreatePlayer();
	pNewChar->SetSession(this);
	if(!pNewChar->Create( recv_data ))
	{
		// failed.
		pNewChar->ok_to_remove = true;
		sGarbageCollection.AddObject( pNewChar );
		pNewChar = NULL;
		res = CHAR_CREATE_FAILED;
		OutPacket( SMSG_CHAR_CREATE, 1, &res );
		return;
	}

	//Same Faction limitation only applies to PVP and RPPVP realms :)
/*	uint32 realmType = sLogonCommHandler.GetRealmType();
	if( !HasGMPermissions() && realmType == REALMTYPE_PVP && _side >= 0 && !sWorld.crossover_chars) // ceberwow fixed bug
	{
		if( ((pNewChar->GetTeam()== 0) && (_side == 1)) || ((pNewChar->GetTeam()== 1) && (_side == 0)) )
		{
			pNewChar->ok_to_remove = true;
			delete pNewChar;
			pNewChar = NULL;
			res = CHAR_CREATE_PVP_TEAMS_VIOLATION;
			OutPacket( SMSG_CHAR_CREATE, 1, &res);
			return;
		}
	}*/

	pNewChar->UnSetBanned();
	pNewChar->addSpell(22027);	  // Remove Insignia

/*	if(pNewChar->getClass() == WARLOCK)
	{
		pNewChar->AddSummonSpell(416, 3110);		// imp fireball
		pNewChar->AddSummonSpell(417, 19505);
		pNewChar->AddSummonSpell(1860, 3716);
		pNewChar->AddSummonSpell(1863, 7814);
	}*/

	pNewChar->SaveToDB(true);	

	PlayerInfo *pn=new PlayerInfo ;
	pn->guid = pNewChar->GetLowGUID();
	pn->name = strdup_local(pNewChar->GetName());
	pn->_class = pNewChar->getClass();
	pn->race = pNewChar->getRace();
	pn->gender = pNewChar->getGender();
	pn->acct = GetAccountId();
	pn->m_Group=0;
	pn->subGroup=0;
	pn->m_loggedInPlayer=NULL;
	pn->team = pNewChar->GetTeam ();
	pn->guild=NULL;
	pn->guildRank=NULL;
	pn->guildMember=NULL;
	pn->lastOnline = UNIXTIME;
#ifdef VOICE_CHAT
	pn->groupVoiceId = -1;
#endif
	objmgr.AddPlayerInfo(pn);

	pNewChar->ok_to_remove = true;
	sGarbageCollection.AddObject( pNewChar );
	pNewChar = NULL;

	res = CHAR_CREATE_SUCCESS;
	OutPacket( SMSG_CHAR_CREATE, 1, &res );

	sLogonCommHandler.UpdateAccountCount( GetAccountId(), 1 );
}

void WorldSession::HandleCharDeleteOpcode( WorldPacket & recv_data )
{
	CHECK_PACKET_SIZE(recv_data, 8);
	uint8 fail = CHAR_DELETE_SUCCESS;

	uint64 guid;
	recv_data >> guid;

	//i wonder if this is valid for all bytes
	//this guid obfuscation was added in 14333 client
	GUID_un_obfuscate( guid );

	PlayerInfo * inf = objmgr.GetPlayerInfo((uint32)guid);
	if( inf == NULL || inf->acct != GetAccountId() )
	{
		uint8 EnumOrder = ( uint8 )( ( guid >> 48 ) - 2 );
		QueryResult * result = CharacterDatabase.Query("SELECT guid FROM characters WHERE acct = %u order by guid limit %u,1", _accountId, (uint32)EnumOrder );
		if( result == NULL )
		{ 
			return;
		}
		uint32 GUIDDB = result->Fetch()[0].GetUInt32();
		delete result;
		result = NULL;
		//double safety. This is very innacurate due to missing 1s, but acceptable
		if( ( guid & GUIDDB ) != (uint32)guid )
		{
			return;
		}
		inf = objmgr.GetPlayerInfo( (uint32)GUIDDB );
		if( inf == NULL )
		{
			return;
		}
	}

	if( objmgr.GetPlayer( (uint32)inf->guid) != NULL )
	{
		fail = CHAR_DELETE_FAILED;
	} 
	else 
	{

		if( inf != NULL && inf->m_loggedInPlayer == NULL )
		{
			QueryResult * result = CharacterDatabase.Query("SELECT name FROM characters WHERE guid = %u AND acct = %u", (uint32)inf->guid, _accountId);
			if(!result)
				return;

			if(inf->guild)
			{
				if(inf->guild->GetGuildLeader()==inf->guid)
					inf->guild->Disband();
				else
					inf->guild->RemoveGuildMember(inf,NULL);
			}

			string name = result->Fetch()[0].GetString();
			delete result;
			result = NULL;

			for(int i = 0; i < NUM_CHARTER_TYPES; ++i)
			{
				Charter * c = objmgr.GetCharterByGuid(guid, (CharterTypes)i);
				if(c != NULL)
					c->RemoveSignature((uint32)inf->guid);
			}


			for(int i = 0; i < NUM_ARENA_TEAM_TYPES; ++i)
			{
				ArenaTeam * t = objmgr.GetArenaTeamByGuid((uint32)inf->guid, i);
				if(t != NULL)
					t->RemoveMember(inf);
			}
			
			/*if( _socket != NULL )
				sPlrLog.write("Account: %s | IP: %s >> Deleted player %s", GetAccountName().c_str(), GetSocket()->GetRemoteIP().c_str(), name.c_str());*/
			
			sPlrLog.writefromsession(this, "deleted character %s (GUID: %u)", name.c_str(), (uint32)inf->guid);

			//first backup deleted chars if possible
			CharacterDatabase.WaitExecute("replace into characters_deleted (select * from characters WHERE guid = %u)", (uint32)inf->guid);
			CharacterDatabase.WaitExecute("delete from playeritems_deleted WHERE ownerguid = %u", (uint32)inf->guid);
			CharacterDatabase.WaitExecute("replace into playeritems_deleted (select * from playeritems WHERE ownerguid = %u)", (uint32)inf->guid);

			CharacterDatabase.WaitExecute("DELETE FROM characters WHERE guid = %u", (uint32)inf->guid);

			Corpse * c=objmgr.GetCorpseByOwner((uint32)inf->guid);
			if(c)
				CharacterDatabase.Execute("DELETE FROM corpses WHERE guid = %u", c->GetLowGUID());

			CharacterDatabase.Execute("DELETE FROM character_equipmentsets WHERE guid=%u",(uint32)inf->guid);
			CharacterDatabase.Execute("DELETE FROM playeritems WHERE ownerguid=%u",(uint32)inf->guid);
			CharacterDatabase.Execute("DELETE FROM gm_tickets WHERE playerGuid = %u", (uint32)inf->guid);
			CharacterDatabase.Execute("DELETE FROM playerpets WHERE ownerguid = %u", (uint32)inf->guid);
			CharacterDatabase.Execute("DELETE FROM playerpetspells WHERE ownerguid = %u", (uint32)inf->guid);
			CharacterDatabase.Execute("DELETE FROM tutorials WHERE playerId = %u", (uint32)inf->guid);
			CharacterDatabase.Execute("DELETE FROM questlog WHERE player_guid = %u", (uint32)inf->guid);
			CharacterDatabase.Execute("DELETE FROM playercooldowns WHERE player_guid = %u", (uint32)inf->guid);
			CharacterDatabase.Execute("DELETE FROM mailbox WHERE player_guid = %u", (uint32)inf->guid);
			CharacterDatabase.Execute("DELETE FROM social_friends WHERE character_guid = %u OR friend_guid = %u", (uint32)inf->guid, (uint32)inf->guid);
			CharacterDatabase.Execute("DELETE FROM social_ignores WHERE character_guid = %u OR ignore_guid = %u", (uint32)inf->guid, (uint32)inf->guid);

			/* remove player info */
			objmgr.DeletePlayerInfo((uint32)inf->guid);
		}
		else
			fail = CHAR_DELETE_FAILED;
		_side = -1; // ceberwow added it
	}

	OutPacket( SMSG_CHAR_DELETE, 1, &fail);
}

void WorldSession::HandleCharRenameOpcode(WorldPacket & recv_data)
{
	WorldPacket data( SMSG_CHAR_RENAME, recv_data.size() + 1 );

	uint64 guid;
	string name;
	recv_data >> guid >> name;

	//!!! only low part of the guid will be ok with last implementation !
	GUID_un_obfuscate( guid );	

	//fucking hackfix ! Client eats up bytes that contain only "1"
	PlayerInfo * inf = objmgr.GetPlayerInfo((uint32)guid);
	if( inf == NULL || inf->acct != GetAccountId() )
	{
		uint8 EnumOrder = ( uint8 )( ( guid >> 48 ) - 2 );
		QueryResult * result = CharacterDatabase.Query("SELECT guid FROM characters WHERE acct = %u order by guid limit %u,1", _accountId, (uint32)EnumOrder );
		if( result == NULL )
		{ 
			return;
		}
		uint32 GUIDDB = result->Fetch()[0].GetUInt32();
		delete result;
		result = NULL;
		//double safety. This is very innacurate due to missing 1s, but acceptable
		if( ( guid & GUIDDB ) != (uint32)guid )
		{
			return;
		}
		inf = objmgr.GetPlayerInfo( (uint32)GUIDDB );
	}
	if(inf == 0) 
	{ 
		return;
	}

	QueryResult * result = CharacterDatabase.Query("SELECT forced_rename_pending FROM characters WHERE guid = %u AND acct = %u", (uint32)inf->guid, _accountId);
	if( result == NULL )
	{ 
		return;
	}
	delete result;
	result = NULL;

	// Check name for rule violation.
	
	LoginErrorCode err = VerifyName( name.c_str(), name.length() );
	if( err != CHAR_NAME_SUCCESS )
	{
		data << uint8( err );
		data << guid << name;
		SendPacket(&data);
		return;
	}

	QueryResult * result2 = CharacterDatabase.Query("SELECT COUNT(*) FROM banned_names WHERE name = '%s'", CharacterDatabase.EscapeString(name).c_str());
	if(result2)
	{
		if(result2->Fetch()[0].GetUInt32() > 0)
		{
			// That name is banned!
			data << uint8( CHAR_NAME_PROFANE );
			data << guid << name;
			SendPacket(&data);
		}
		delete result2;
		result2 = NULL;
	}

	// Check if name is in use.
	if( objmgr.GetPlayerInfoByName( name.c_str() ) != NULL )
	{
		data << uint8( CHAR_CREATE_NAME_IN_USE );
		data << guid << name;
		SendPacket(&data);
		return;
	}

	// correct capitalization
	CapitalizeString(name);
	objmgr.RenamePlayerInfo(inf, inf->name, name.c_str());

	sPlrLog.writefromsession(this, "a rename was pending. renamed character %s (GUID: %u) to %s.", inf->name, inf->guid, name.c_str());

	// If we're here, the name is okay.
	free(inf->name);
	inf->name = strdup_local(name.c_str());
	CharacterDatabase.WaitExecute("UPDATE characters SET name = '%s' WHERE guid = %u", CharacterDatabase.EscapeString( name ).c_str(), (uint32)inf->guid);
	CharacterDatabase.WaitExecute("UPDATE characters SET forced_rename_pending = 0 WHERE guid = %u", (uint32)inf->guid);
	
	data << uint8( RESPONSE_SUCCESS ) << guid << name;
	SendPacket( &data );

	//this is only required in 4.3.3 client since we are missing proper reply :(
	HandleCharEnumOpcode( recv_data );
}


void WorldSession::HandlePlayerLoginOpcode( WorldPacket & recv_data )
{
//	CHECK_PACKET_SIZE(recv_data, 8);
	uint64 playerGuid = 0;

	sLog.outDebug( "WORLD: Recvd Player Logon Message" );

//	recv_data >> playerGuid; // this is the GUID selected by the player
   
	uint8 guidMark,byte;
	recv_data >> guidMark;
    // Bits are mixed up...
    // Let's skip the highguid part -- it's 0x0000 for players anyway
    if( guidMark & BIT_2 )
    {
        recv_data >> byte;
        playerGuid |= uint64(byte << 8);
    }
    if( guidMark & BIT_5 )
    {
        recv_data >> byte;
        playerGuid |= uint64(byte << 16);
    }
    if( guidMark & BIT_6 )
    {
        recv_data >> byte;
        playerGuid |= uint64(byte);
    }
    if( guidMark & BIT_3 )
    {
        recv_data >> byte;
        playerGuid |= uint64(byte) << 48;
    }
    if( guidMark & BIT_4 )
    {
        recv_data >> byte;
//        playerGuid |= uint64(byte);
    }
    if( guidMark & BIT_7 )
    {
        recv_data >> byte;
        playerGuid |= uint64(byte << 24);
    }

	//fucking hackfix ! Client eats up bytes that contain only "1"
	PlayerInfo * inf = objmgr.GetPlayerInfo((uint32)playerGuid);
	if( inf == NULL || inf->acct != GetAccountId() )
	{
		uint8 EnumOrder = ( uint8 )( ( playerGuid >> 48 ) - 2 );
		QueryResult * result = CharacterDatabase.Query("SELECT guid FROM characters WHERE acct = %u order by guid limit %u,1", _accountId, (uint32)EnumOrder );
		if( result == NULL )
		{ 
			sLog.outDebug( "WORLD: Could not find this guid for this account" );
			return;
		}
		uint32 GUIDDB = result->Fetch()[0].GetUInt32();
		delete result;
		result = NULL;
		//double safety. This is very innacurate due to missing 1s, but acceptable
		if( ( playerGuid & GUIDDB ) != (uint32)playerGuid )
		{
			sLog.outDebug( "WORLD: GUID seems to mismatch with what we sent to client" );
			return;
		}
		inf = objmgr.GetPlayerInfo( (uint32)GUIDDB );
	}

//	PlayerInfo * inf = objmgr.GetPlayerInfo((uint32)playerGuid);
	if( inf == NULL || inf->m_loggedInPlayer != NULL || inf->acct != GetAccountId() )
	{
		uint8 respons = CHAR_LOGIN_NO_CHARACTER;
		OutPacket( SMSG_CHARACTER_LOGIN_FAILED, 1, &respons );
		return;
	}

	if( objmgr.GetPlayer((uint32)inf->guid) != NULL || m_loggingInPlayer || _player )
	{
		// A character with that name already exists 0x3E
		uint8 respons = CHAR_LOGIN_DUPLICATE_CHARACTER;
		OutPacket( SMSG_CHARACTER_LOGIN_FAILED, 1, &respons );
		return;
	}

	Player* plr = new Player((uint32)inf->guid);
	ASSERT(plr);
	plr->SetSession(this);

	//break existing DC procedure if there was one
	SetLogoutTimer( 0 );

	m_bIsWLevelSet = false;
	
	Log.Debug("WorldSession", "Async loading player %u", (uint32)inf->guid);
	m_loggingInPlayer = plr;
	plr->LoadFromDB((uint32)inf->guid);
}

void WorldSession::FullLogin(Player * plr)
{
	Log.Debug("WorldSession", "Fully loading player %u", plr->GetLowGUID());
	SetPlayer(plr); 
	m_MoverWoWGuid.Init(_player->GetGUID());

	MapMgr *mgr = sInstanceMgr.GetInstance( _player );  
	if( mgr && mgr->m_battleground)
	{
		/* Don't allow player to login into a bg that has ended or is full */
		if (mgr->m_battleground->HasEnded() == true ||
			mgr->m_battleground->HasFreeSlots(_player->GetTeamInitial(), mgr->m_battleground->GetType() == false))
		{
				mgr = NULL;
		}
	}

	//entering more then 10 players in a 10 limit map ? 
	if( mgr && mgr->pInstance && mgr->pInstance->GetCurrentPlayerLimit() <= mgr->GetPlayerCount()  )
	{
		mgr = NULL;
	}

	/* Trying to log to an instance that doesn't exists anymore? */
	if (!mgr)
	{
		if(!IS_INSTANCE(_player->m_bgEntryPointMap))
		{
			_player->m_position.x = _player->m_bgEntryPointX;
			_player->m_position.y = _player->m_bgEntryPointY;
			_player->m_position.z = _player->m_bgEntryPointZ;
			_player->m_position.o = _player->m_bgEntryPointO;
			_player->m_mapId = _player->m_bgEntryPointMap;
		}
		else
		{
			_player->m_position.x = _player->GetBindPositionX();
			_player->m_position.y = _player->GetBindPositionY();
			_player->m_position.z = _player->GetBindPositionZ();
			_player->m_position.o = 0;
			_player->m_mapId = _player->GetBindMapId();
		}
	}

	// copy to movement array
//	movement_packet[0] = m_MoverWoWGuid.GetNewGuidMask();
//	memcpy(&movement_packet[1], m_MoverWoWGuid.GetNewGuid(), m_MoverWoWGuid.GetNewGuidLen());

	_player->UpdateAttackSpeed();
	/*if(plr->getLevel()>PLAYER_LEVEL_CAP)
		plr->SetUInt32Value(UNIT_FIELD_LEVEL,PLAYER_LEVEL_CAP);*/

	// enable trigger cheat by default
	// plr->TriggerpassCheat = HasGMPermissions();

	// Make sure our name exists (for premade system)
	PlayerInfo * info = objmgr.GetPlayerInfo(_player->GetLowGUID());
	if(info == 0)
	{
		info = new PlayerInfo;
		info->_class = _player->getClass();
		info->gender = _player->getGender();
		info->guid = _player->GetLowGUID();
		info->name = strdup_local(_player->GetName());
		info->lastLevel = _player->getLevel();
		info->lastOnline = UNIXTIME;
		info->lastZone = _player->GetZoneId();
		info->race = _player->getRace();
		info->team = _player->GetTeam();
		info->guild=NULL;
		info->guildRank=NULL;
		info->guildMember=NULL;
		info->m_Group=0;
		info->subGroup=0;
		objmgr.AddPlayerInfo(info);
	}
	_player->m_playerInfo = info;
	if(_player->m_playerInfo->guild)
	{
		_player->SetGuildId( _player->m_playerInfo->guild->GetGuildId() );
		_player->m_uint32Values[PLAYER_GUILDRANK] = _player->m_playerInfo->guildRank->iId;
	}

	//this is to enable the menu on client side so you may choose
//	if(info->m_Group == NULL)
	{
		plr->SendDungeonDifficulty();
		plr->SendRaidDifficulty();
	}

	/* world preload */
	packetSMSG_LOGIN_VERIFY_WORLD vwpck;
	vwpck.MapId = _player->GetMapId();
	vwpck.X = _player->GetPositionX();
	vwpck.Y = _player->GetPositionY();
	vwpck.Z = _player->GetPositionZ();
	vwpck.O = _player->GetOrientation();
	OutPacket( SMSG_LOGIN_VERIFY_WORLD, sizeof(packetSMSG_LOGIN_VERIFY_WORLD), &vwpck );

	info->m_loggedInPlayer = _player;
	Player *l_player = _player;

	// account data == UI config
	SendAccountDataTimes( PER_CHARACTER_CACHE_MASK );

	// send voicechat state - active/inactive
	/*
	{SERVER} Packet: (0x03C7) UNKNOWN PacketSize = 2
	|------------------------------------------------|----------------|
	|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
	|------------------------------------------------|----------------|
	|02 01							               |..              |
	-------------------------------------------------------------------
	*/
	
	sStackWorldPacket(data,0,500);
#ifdef VOICE_CHAT
	data.Initialize(SMSG_VOICE_SYSTEM_STATUS);
	data << uint8(2) << uint8(sVoiceChatHandler.CanUseVoiceChat() ? 1 : 0);
	SendPacket(&data);
#else
	/*
    public enum FeatureStatusFlags
    {
        VoiceChatAllowed = 0x40,
        HasTravelPass = 0x80,
    }
  <Packet OpCode="SMSG_FEATURE_SYSTEM_STATUS">
    <Simple>
      <Name>Flags</Name>
      <Type>Byte</Type>
      <SegmentType>FeatureStatusFlags</SegmentType>
    </Simple>
    <Simple>
      <Name>Complain System Status</Name>
      <Type>Byte</Type>
    </Simple>
    <Simple>
      <Name>Unknown Mail Url Related Value (SR)</Name>
      <Type>UInt</Type>
    </Simple>
  </Packet> */
	data.Initialize(SMSG_FEATURE_SYSTEM_STATUS);
	data << uint32( 0x001302C0 ) << uint16( 0 );
	SendPacket(&data);
#endif

	// Send MOTD
	data.Initialize(SMSG_MOTD);
	data << (uint32)(3);
	data << sWorld.GetMotd();
	SendPacket(&data);
	//or just use this ?
//	_player->BroadcastMessage(sWorld.GetMotd());

	//not handled yet. Not really required eighter ?
	data.Initialize(SMSG_LEARNED_DANCE_MOVES);
	data << (uint32)(0) << (uint32)(0);
	SendPacket(&data);
	data.Initialize(SMSG_UPDATE_INSTANCE_OWNERSHIP);
	data << (uint32)(0);
//	SendPacket(&data);


	// Set TIME OF LOGIN
	CharacterDatabase.Execute("UPDATE characters SET online = 1 WHERE guid = %u" , l_player->GetLowGUID());

	bool enter_world = true;
#ifndef CLUSTERING
	// Find our transporter and add us if we're on one.
	if(l_player->m_TransporterGUID != 0)
	{
		Transporter * pTrans = objmgr.GetTransporter(GUID_LOPART(l_player->m_TransporterGUID));
		if(pTrans)
		{
			if(l_player->IsDead())
			{
				l_player->ResurrectPlayer();
				l_player->SetUInt32Value(UNIT_FIELD_HEALTH, l_player->GetUInt32Value(UNIT_FIELD_MAXHEALTH));
				l_player->SetPower( POWER_TYPE_MANA, l_player->GetMaxPower( POWER_TYPE_MANA ));
			}

			float c_tposx = pTrans->GetPositionX() + l_player->m_TransporterX;
			float c_tposy = pTrans->GetPositionY() + l_player->m_TransporterY;
			float c_tposz = pTrans->GetPositionZ() + l_player->m_TransporterZ;
			if(l_player->GetMapId() != pTrans->GetMapId())	   // loaded wrong map
			{
				l_player->SetMapId(pTrans->GetMapId());
				sStackWorldPacket( dataw, SMSG_NEW_WORLD, 50);
				dataw << c_tposx << c_tposy << c_tposz << pTrans->GetMapId() << l_player->GetOrientation();
				SendPacket(&dataw);

				// shit is sent in worldport ack.
				enter_world = false;
			}

			l_player->SetPosition(c_tposx, c_tposy, c_tposz, plr->GetOrientation(), false);
			l_player->m_CurrentTransporter = pTrans;
			pTrans->AddPlayer(l_player);
		}
	}
#endif

	Log.Debug("Login", "Player %s logged in.", l_player->GetName());

	if(l_player->GetTeam() == 1)
		sWorld.HordePlayers++;
	else
		sWorld.AlliancePlayers++;

	if(l_player->m_FirstLogin)
	{
		uint32 racecinematic = l_player->myRace->CinematicSequence;
		if( l_player->getClass() == DEATHKNIGHT )
			racecinematic = 165;
#ifdef USING_BIG_ENDIAN
		swap32(&racecinematic);
#endif
		OutPacket(SMSG_TRIGGER_CINEMATIC, 4, &racecinematic);
	}

	sLog.outDetail( "WORLD: Created new player for existing players (%s)", l_player->GetName() );

	// Login time, will be used for played time calc
	l_player->m_playedtime[2] = (uint32)UNIXTIME;

	//Issue a message telling all guild members that this player has signed on
	if(l_player->IsInGuild())
	{
		Guild *pGuild = l_player->m_playerInfo->guild;
		if(pGuild)
		{
			sStackWorldPacket( data, SMSG_GUILD_EVENT, 550 );
			data << uint8(GUILD_EVENT_MOTD);
			data << uint8(0x01);
			if(pGuild->GetMOTD())
				data << pGuild->GetMOTD();
			else
				data << uint8(0);
			SendPacket(&data);

			pGuild->LogGuildEvent(GUILD_EVENT_HASCOMEONLINE, l_player->GetGUID(), 1, l_player->GetName());
			pGuild->SendGuildRankQuery( this );
			pGuild->SendGuildStatusUnk( this );
		}
	}

	// Send online status to people having this char in friendlist
	l_player->Social_TellFriendsOnline();
	// send friend list (for ignores)
	l_player->Social_SendFriendList(7);

#ifndef GM_TICKET_MY_MASTER_COMPATIBLE
	GM_Ticket * ticket = objmgr.GetGMTicketByPlayer(l_player->GetGUID());
	if(ticket != NULL)
	{
		//Send status change to gm_sync_channel
		Channel *chn = channelmgr.GetChannel(sWorld.getGmClientChannel().c_str(), l_player);
		if(chn)
		{
			std::stringstream ss;
			ss << "GmTicket:" << GM_TICKET_CHAT_OPCODE_ONLINESTATE;
			ss << ":" << ticket->guid;
			ss << ":1";
			chn->Say(l_player, ss.str().c_str(), NULL, true);
		}
	}
#endif

	// Send revision (if enabled)
#if (defined( WIN32 ) || defined( WIN64 ) )
//	l_player->BroadcastMessage("Powered by: %sArcEmu %s r%u/%s-Win-%s %s(Please report ALL bugs to www.ArcEmu.org/forums/)", MSG_COLOR_WHITE, BUILD_TAG,
//		BUILD_REVISION, CONFIG, ARCH, MSG_COLOR_LIGHTBLUE);		
#else
	l_player->BroadcastMessage("Powered by: %sArcEmu %s r%u/%s-%s %s(Please report ALL bugs to www.ArcEmu.org/forums/)", MSG_COLOR_WHITE, BUILD_TAG,
		BUILD_REVISION, PLATFORM_TEXT, ARCH, MSG_COLOR_LIGHTBLUE);
#endif

	if(sWorld.SendStatsOnJoin)
	{
//		l_player->BroadcastMessage("Online Players: %s%u |rPeak: %s%u|r Accepted Connections: %s%u",
//			MSG_COLOR_WHITE, sWorld.GetSessionCount(), MSG_COLOR_WHITE, sWorld.PeakSessionCount, MSG_COLOR_WHITE, sWorld.mAcceptedConnections);
	}

	//Calculate rest bonus if there is time between lastlogoff and now
	if( l_player->m_timeLogoff > 0 && l_player->GetUInt32Value(UNIT_FIELD_LEVEL) < l_player->GetUInt32Value(PLAYER_FIELD_MAX_LEVEL))	// if timelogoff = 0 then it's the first login
	{
		uint32 currenttime = (uint32)UNIXTIME;
		uint32 timediff = currenttime - l_player->m_timeLogoff;

		//Calculate rest bonus
		if( timediff > 0 ) 
			l_player->AddCalculatedRestXP(timediff);
	}

#ifdef CLUSTERING
	l_player->SetInstanceID(forced_instance_id);
	l_player->SetMapId(forced_map_id);
#else
	sHookInterface.OnEnterWorld2(l_player);
#endif

	if(info->m_Group)
		info->m_Group->Update();

#ifndef USE_HACK_TO_ENABLE_ACHIEVEMENTS
	l_player->SendAllAchievementData( this ); //one thing is for sure. If you do not send these at the right moments then client will not show them
#endif

	//concurency issue, while logging in player somehow gets kicked out and this pointer gets to null
	if( l_player != _player )
	{
//		if( l_player->deleted == false )
//			l_player->Virtual_Destructor();
		return;
	}
	if(enter_world && !l_player->GetMapMgr())
	{
		l_player->AddToWorld();
	}

	objmgr.AddPlayer(l_player);

}

bool ChatHandler::HandleRenameCommand(const char * args, WorldSession * m_session)
{
	// prevent buffer overflow
	if(strlen(args) > 100)
	{ 
		return false;
	}

	char name1[100];
	char name2[100];
	strcpy( name2, "");

	if(sscanf(args, "%s %s", name1, name2) != 2)
	{ 
		return false;
	}

	if( VerifyName( name2, strlen( name2 ) ) != CHAR_NAME_SUCCESS )
	{
		RedSystemMessage(m_session, "That name is invalid or contains invalid characters.");
		return true;
	}

	string new_name = name2;
	PlayerInfo * pi = objmgr.GetPlayerInfoByName(name1);
	if(pi == 0)
	{
		RedSystemMessage(m_session, "Player not found with this name.");
		return true;
	}

	if( objmgr.GetPlayerInfoByName(new_name.c_str()) != NULL )
	{
		RedSystemMessage(m_session, "Player found with this name in use already.");
		return true;
	}

	objmgr.RenamePlayerInfo(pi, pi->name, new_name.c_str());

	free(pi->name);
	pi->name = strdup_local(new_name.c_str());

	// look in world for him
	Player * plr = objmgr.GetPlayer(pi->guid);
	if(plr != 0)
	{
		plr->SetName(new_name);
		BlueSystemMessageToPlr(plr, "%s changed your name to '%s'.", m_session->GetPlayer()->GetName(), new_name.c_str());
		plr->SaveToDB(false);
	}
	else
	{
		CharacterDatabase.WaitExecute("UPDATE characters SET name = '%s' WHERE guid = %u", CharacterDatabase.EscapeString(new_name).c_str(), (uint32)pi->guid);
	}

	GreenSystemMessage(m_session, "Changed name of '%s' to '%s'.", name1, name2);
	sGMLog.writefromsession(m_session, "renamed character %s (GUID: %u) to %s", name1, pi->guid, name2);
	sPlrLog.writefromsession(m_session, "GM renamed character %s (GUID: %u) to %s", name1, pi->guid, name2);
	return true;
}
