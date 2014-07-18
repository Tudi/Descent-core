/*
* ArcEmu MMORPG Server
* Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
* Copyright (C) 2008-2009 <http://www.ArcEmu.org/>
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

initialiseSingleton(CBattlegroundManager);
typedef CBattleground*(*CreateBattlegroundFunc)(MapMgr* mgr,uint32 iid,uint32 group, uint32 type);

const static uint32 BGMapIds[ BATTLEGROUND_NUM_TYPES ] =
{
	0,		// 0
	30,		// AV
	489,	// WSG
	529,	// AB
	0,		// 2v2
	0,		// 3v3
	0,		// 5v5
	566,	// EOTS
	0,
	607,	// SOTA
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	726,
	0,0,0,0,0,0,0,0,0,0,0,761
};

const static CreateBattlegroundFunc BGCFuncs[BATTLEGROUND_NUM_TYPES] = {
	NULL,								// 0
	&AlteracValley::Create,				// AV
	&WarsongGulch::Create,				// WSG
	&ArathiBasin::Create,				// AB
	NULL,								// 2v2
	NULL,								// 3v3
	NULL,								// 5v5
	&EyeOfTheStorm::Create,				// EotS
	NULL,
	&StrandOfTheAncient::Create,		// SOTA, needs to be updated when SOTA is in
	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	
	&TwinPeaks::Create,	NULL,	
	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	
	&BattleForGilneas::Create,	
};

CBattlegroundManager::CBattlegroundManager()
:
EventableObject(),
m_maxBattlegroundId(0)
{
	int i;
	//dbcBattlemasterListStore.LookupEntry(

	sEventMgr.AddEvent(this, &CBattlegroundManager::EventQueueUpdate, EVENT_BATTLEGROUND_QUEUE_UPDATE, 15000, 0,0);

	for (i=0; i<BATTLEGROUND_NUM_TYPES; i++) {
		m_instances[i].clear();
	}

}

CBattlegroundManager::~CBattlegroundManager()
{

}

bool CBattlegroundManager::IsValidBGPointer( CBattleground *checkme )
{
	m_instanceLock.Acquire();
	for(int i=0;i<BATTLEGROUND_NUM_TYPES;i++)
		for(map<uint32, CBattleground* >::iterator itr = m_instances[i].begin(); itr != m_instances[i].end(); ++itr)
			if( itr->second == checkme )
			{
				m_instanceLock.Release();
				return true;
			}
	m_instanceLock.Release();
	return false;
}
/*
uint32 CBattlegroundManager::GetMap(uint32 bg_index)
{
	if (bg_index >= BATTLEGROUND_NUM_TYPES)
	{ 
		return 0;
	}
	return BGMapIds[ bg_index ];
}*/

void CBattlegroundManager::HandleBattlegroundListPacket(WorldSession * m_session, uint32 BattlegroundType, bool battlemaster)
{
/*	
6A 1A 00 C8 4D 00 30 F1 
00 - battlemaster
06 00 00 00 - BATTLEGROUND_ARENA_5V5
0E unk
00 00 00 00
	if(BattlegroundType == BATTLEGROUND_ARENA_2V2 || BattlegroundType == BATTLEGROUND_ARENA_3V3 || BattlegroundType == BATTLEGROUND_ARENA_5V5)
	{
		WorldPacket data(SMSG_BATTLEFIELD_LIST, 17);
		data << m_session->GetPlayer()->GetGUID() << uint32(6) << uint32(0xC) << uint8(0);
		m_session->SendPacket(&data);
		return;
	}*/
/*
for strand of ancients
00 00 00 00 00 00 00 00 
01 - pvp menu
02 00 00 00 - type
07 ?
0C 00 00 00 -> count = 12
07 00 00 00
0A 00 00 00
01 00 00 00
0D 00 00 00
0F 00 00 00 
0E 00 00 00 
05 00 00 00
02 00 00 00
04 00 00 00 
08 00 00 00 
09 00 00 00
0B 00 00 00 
for alterac valey
00 00 00 00 00 00 00 00
01 
02 00 00 00 
07 
0F 00 00 00 
01 00 00 00 0E 00 00 00 05 00 00 00 08 00 00 00 09 00 00 00 0B 00 00 00 10 00 00 00 0C 00 00 00 03 00 00 00 06 00 00 00 11 00 00 00 0D 00 00 00 12 00 00 00 02 00 00 00 13 00 00 00 

//3.3.3
00 00 00 00 00 00 00 00 
01 - not battlemaster
20 00 00 00 -> 32 = all ?
50 minlevel ?
50 maxlevel ?
00 
8B 0E 00 00 
19 00 00 00
6D 02 00 00 
01 - is random
00 
8B 0E 00 00
19 00 00 00 
6D 02 00 00 
06 00 00 00 - count
09 00 00 00 
0A 00 00 00 
03 00 00 00
04 00 00 00
06 00 00 00 
01 00 00 00 

13329
{SERVER} Packet: (0x3858) UNKNOWN PacketSize = 43 TimeStamp = 27292390
20 00 00 00 = 32 = 100000
00 00 00 00 ?
00 00 ?
1E 00 00 00 ?
20 00 00 00 -> requested BG type
00 00 00 00 
05 00 00 00
00 
05 00 00 00 
1E 00 00 00
B9 11 00 00 B6 88 30 F1 - gossiper guid
14333
{SERVER} Packet: (0x5301) SMSG_BATTLEFIELD_LIST PacketSize = 36 TimeStamp = 10001208
00 30 
19 00 00 00 win arena 
2D 00 00 00 win honor ?
20 00 00 00 - all BG listed
0E 01 00 00 win honor rand ?
00 00 00 00 
19 00 00 00 win arena 
55 min level ?
2D 00 00 00 win honor ?
55 max level ?
0E 01 00 00 win honor rand ?

{SERVER} Packet: (0x5301) SMSG_BATTLEFIELD_LIST PacketSize = 36 TimeStamp = 10816313
00 30 
00 00 00 00 
0F 00 00 00 
03 00 00 00 
5A 00 00 00 
00 00 00 00 
00 00 00 00 
0A 
0F 00 00 00 
0E 
5A 00 00 00 
*/

	if( BattlegroundType == 0x01 )
	{
		WorldPacket data(SMSG_BATTLEFIELD_LIST, 200);
		data.CopyFromSnifferDump("00 30 19 00 00 00 2D 00 00 00 01 00 00 00 0E 01 00 00 00 00 00 00 19 00 00 00 55 2D 00 00 00 55 0E 01 00 00 ");
		m_session->SendPacket(&data);
		return;
	}
	else if( BattlegroundType == 0x02 )
	{
		WorldPacket data(SMSG_BATTLEFIELD_LIST, 200);
		data.CopyFromSnifferDump("00 30 19 00 00 00 2D 00 00 00 02 00 00 00 0E 01 00 00 00 00 00 00 19 00 00 00 55 2D 00 00 00 55 0E 01 00 00 ");
		m_session->SendPacket(&data);

/*
{SERVER} Packet: (0x0B21) UNKNOWN PacketSize = 37 TimeStamp = 27078622
02 00 00 06 3C 00 98 18 DD 86 FF 44 45 0B 92 C5 BC 4F FA 41 CE 81 00 03 CD CC 0C 41 AF 1C A6 3D 61 F5 51 95 CC 

{SERVER} Packet: (0xC743) UNKNOWN PacketSize = 37 TimeStamp = 27078622
1F 80 18 04 00 00 08 61 F5 51 95 03 67 66 F6 40 AF 1C A6 3D 18 81 CC 00 CE DD 86 FF 44 45 0B 92 C5 BC 4F FA 41 

{SERVER} Packet: (0x4765) UNKNOWN PacketSize = 37 TimeStamp = 27078622
C0 40 00 00 84 00 2E E7 FB 01 40 03 25 87 FF 44 44 0B 92 C5 BC 4F FA 41 18 00 81 62 F5 51 95 CE CC AF 1C A6 3D 

{SERVER} Packet: (0x0B21) UNKNOWN PacketSize = 37 TimeStamp = 27078622
00 00 00 06 38 00 98 18 44 88 FF 44 3E 0B 92 C5 BC 4F FA 41 CE 81 00 03 84 EB F1 40 AF 1C A6 3D 66 F5 51 95 CC 
*/
		data.Initialize( 0x0B21 );
		data.CopyFromSnifferDump("00 30 19 00 00 00 2D 00 00 00 02 00 00 00 0E 01 00 00 00 00 00 00 19 00 00 00 55 2D 00 00 00 55 0E 01 00 00 ");
		m_session->SendPacket(&data);
		data.Initialize( 0xC743 );
		data.CopyFromSnifferDump("1F 80 18 04 00 00 08 61 F5 51 95 03 67 66 F6 40 AF 1C A6 3D 18 81 CC 00 CE DD 86 FF 44 45 0B 92 C5 BC 4F FA 41 ");
		m_session->SendPacket(&data);
		data.Initialize( 0x4765 );
		data.CopyFromSnifferDump("C0 40 00 00 84 00 2E E7 FB 01 40 03 25 87 FF 44 44 0B 92 C5 BC 4F FA 41 18 00 81 62 F5 51 95 CE CC AF 1C A6 3D ");
		m_session->SendPacket(&data);
		data.Initialize( 0x0B21 );
		data.CopyFromSnifferDump("00 00 00 06 38 00 98 18 44 88 FF 44 3E 0B 92 C5 BC 4F FA 41 CE 81 00 03 84 EB F1 40 AF 1C A6 3D 66 F5 51 95 CC ");
		m_session->SendPacket(&data);

		return;
	}
	else if( BattlegroundType == 0x03 )
	{
		WorldPacket data(SMSG_BATTLEFIELD_LIST, 200);
		data.CopyFromSnifferDump("00 30 19 00 00 00 2D 00 00 00 03 00 00 00 0E 01 00 00 00 00 00 00 19 00 00 00 55 2D 00 00 00 55 0E 01 00 00 ");
		m_session->SendPacket(&data);
		return;
	}
	else if( BattlegroundType == 0x04 || BattlegroundType == 0x05 || BattlegroundType == 0x06 )
	{
		WorldPacket data(SMSG_BATTLEFIELD_LIST, 200);
		data.CopyFromSnifferDump("00 30 19 00 00 00 2D 00 00 00 20 00 00 00 0E 01 00 00 00 00 00 00 19 00 00 00 55 2D 00 00 00 55 0E 01 00 00 ");
		m_session->SendPacket(&data);
		return;
	}
	else if( BattlegroundType == 0x07 )
	{
		WorldPacket data(SMSG_BATTLEFIELD_LIST, 200);
		data.CopyFromSnifferDump("00 30 19 00 00 00 2D 00 00 00 07 00 00 00 0E 01 00 00 00 00 00 00 19 00 00 00 55 2D 00 00 00 55 0E 01 00 00 ");
		m_session->SendPacket(&data);
		return;
	}
	else if( BattlegroundType == 0x09 )
	{
		WorldPacket data(SMSG_BATTLEFIELD_LIST, 200);
		data.CopyFromSnifferDump("00 30 19 00 00 00 2D 00 00 00 09 00 00 00 0E 01 00 00 00 00 00 00 19 00 00 00 55 2D 00 00 00 55 0E 01 00 00 ");
		m_session->SendPacket(&data);
		return;
	}
	else if( BattlegroundType == 0x20 )
	{
		WorldPacket data(SMSG_BATTLEFIELD_LIST, 200);
		data.CopyFromSnifferDump("00 30 19 00 00 00 2D 00 00 00 20 00 00 00 0E 01 00 00 00 00 00 00 19 00 00 00 55 2D 00 00 00 55 0E 01 00 00 ");
		m_session->SendPacket(&data);
		return;
	}
	else if( BattlegroundType == 0x6C )
	{
		WorldPacket data(SMSG_BATTLEFIELD_LIST, 200);
		data.CopyFromSnifferDump("00 30 19 00 00 00 2D 00 00 00 6C 00 00 00 0E 01 00 00 00 00 00 00 19 00 00 00 55 2D 00 00 00 55 0E 01 00 00 ");
		m_session->SendPacket(&data);
		return;
	}
	else if( BattlegroundType == 0x78 )
	{
		WorldPacket data(SMSG_BATTLEFIELD_LIST, 200);
		data.CopyFromSnifferDump("00 30 19 00 00 00 2D 00 00 00 78 00 00 00 0E 01 00 00 00 00 00 00 19 00 00 00 55 2D 00 00 00 55 0E 01 00 00 ");
		m_session->SendPacket(&data);
		return;
	}
	else if( BattlegroundType == 0x1E )
	{
		WorldPacket data(SMSG_BATTLEFIELD_LIST, 200);
		data.CopyFromSnifferDump("00 30 19 00 00 00 2D 00 00 00 1E 00 00 00 0E 01 00 00 00 00 00 00 19 00 00 00 55 2D 00 00 00 55 0E 01 00 00 ");
		m_session->SendPacket(&data);
		return;
	}

/*	if( BattlegroundType >= BATTLEGROUND_NUM_TYPES )
		return;

	uint32 Count = 0;
	WorldPacket data(SMSG_BATTLEFIELD_LIST, 200);
	data << m_session->GetPlayer()->GetGUID();
	data << uint8(!battlemaster);	//before it was just 2 ?
	data << BattlegroundType;

	//seems like these have been added to maps.dbc in 3.3.3
	data << uint8(10);	//min level ?
	data << uint8(85);	//max level ?

    uint8 isRandom = (BattlegroundType == BATTLEGROUND_ALL_BG);
    data << uint8(isRandom);                               // 3.3.3 isRandom
    if(isRandom)
    {
		// Rewards
		data << uint8(0);                                      // 3.3.3 hasWin
		data << uint32( 3723 );                                // 3.3.3 winHonor
	    data << uint32( 25 );                                  // 3.3.3 winArena
	    data << uint32( 621 );                                 // 3.3.3 lossHonor

        // Rewards (random)
        data << uint8(0);										// 3.3.3 hasWin_Random
        data << uint32( 3723 );                                 // 3.3.3 winHonor_Random
        data << uint32( 25 );									// 3.3.3 winArena_Random
        data << uint32( 621 );									// 3.3.3 lossHonor_Random
    }
	else
	{
		data << uint8(0);										// 3.3.3 hasWin
		data << uint32( 0 );                                     // 3.3.3 winHonor
	    data << uint32( 0 );                                     // 3.3.3 winArena
	    data << uint32( 0 );                                     // 3.3.3 lossHonor
	}

	size_t CountPos = data.wpos();
	data << uint32(0);      // Count
	if(!IS_ARENA(BattlegroundType))
	{
		// Append the battlegrounds 
		m_instanceLock.Acquire();
		for(map<uint32, CBattleground* >::iterator itr = m_instances[BattlegroundType].begin(); itr != m_instances[BattlegroundType].end(); ++itr)
		{
			if( itr->second->CanPlayerJoin(m_session->GetPlayer(),BattlegroundType)  && !itr->second->HasEnded() )
			{
				data << itr->first;
				++Count;
			}
		}
		m_instanceLock.Release();
		*(uint32*)&data.contents()[CountPos] = Count;
	}
	m_session->SendPacket(&data);
	*/
}

void CBattlegroundManager::HandleBattlegroundJoin(WorldSession * m_session, WorldPacket & pck)
{
/*
14333
{CLIENT} Packet: (0xA860) CMSG_BATTLEFIELD_JOIN_QUEUE PacketSize = 9 TimeStamp = 15727974
C4 00 
00 00 00 00 
03 bg type
11 1E 
*/
	if( m_session->GetPlayer()->IsSpectator() )
	{
		m_session->GetPlayer()->BroadcastMessage("Can't queue while spectator");
		return;
	}

	uint32 pguid = m_session->GetPlayer()->GetLowGUID();
	uint32 lgroup = GetLevelGrouping(m_session->GetPlayer()->getLevel());
	uint8 bgtype;
	uint32 instance;

	uint16 unk1;
	uint32 unk2;
	pck >> unk1;
	pck >> unk2;
	pck >> bgtype;
//	pck >> instance; //not really ?
	instance = 0;

	bgtype = ObfuscateByte( bgtype );

	if(bgtype >= BATTLEGROUND_NUM_TYPES || !bgtype)
	{
//		sCheatLog.writefromsession(m_session,"tried to crash the server by joining battleground that does not exist (0)");
//		m_session->GetPlayer()->SoftDisconnect();
		m_session->GetPlayer()->BroadcastMessage("This BG is not yet implemented");
		return;      // cheater!
	}

	/* Check the instance id */
	if(instance)
	{
		/* We haven't picked the first instance. This means we've specified an instance to join. */
		m_instanceLock.Acquire();
		map<uint32, CBattleground*>::iterator itr = m_instances[bgtype].find(instance);

		if(itr == m_instances[bgtype].end())
		{
			sChatHandler.SystemMessage(m_session, m_session->LocalizedWorldSrv(51));
			m_instanceLock.Release();
			return;
		}

		m_instanceLock.Release();
	}
#ifdef BATTLEGRUND_REALM_BUILD
	int64 *queue_last_reward_stamp = m_session->GetPlayer()->GetCreateIn64Extension( EXTENSION_ID_BG_REALM_QUEU_REW_STAMP );
	*queue_last_reward_stamp = getMSTime();
#endif

	/* Queue him! */
	m_queueLock.Acquire();
	m_queuedPlayers[bgtype][lgroup].insert(pguid);
	Log.Success("BattlegroundManager", "Player %u is now in battleground queue for instance %u", m_session->GetPlayer()->GetLowGUID(), (instance + 1) );

	/*
{SERVER} Packet: (0x18B8) UNKNOWN PacketSize = 28 TimeStamp = 31365623
00 
E8 03 00 00 - maybe elapsed time ?
00 
00 00 00 00 
30 E8 00 00 - maybe estimated wait time ?
55 maxlevel ?
00 00 00 00 
02 00 00 00 warsong ?
00 00 10 1F -maybe instance id ?
00 
*/
/*	sStackWorldPacket( data, SMSG_BATTLEFIELD_QUEUE_STATUS, 28 + 10 );
	data << uint8(0);
	data << uint32( 1000 );	//elapsed time
	data << uint8(0);		//min level ?
	data << uint32( 0 );	//?
	data << uint32( 10*60*1000 );	//estimated time
	data << uint8(85);		//max level ?
	data << uint32( 0 );	//?
	data << uint32( bgtype );	//
	data << uint32( 0x1F100000 );
	data << uint8(0);		//
	m_session->SendPacket( &data ); */

	m_session->GetPlayer()->m_bgIsQueued = true;
	m_session->GetPlayer()->m_bgQueueInstanceId = instance;
	m_session->GetPlayer()->m_bgQueueType = bgtype;

	// Set battleground entry point 
	m_session->GetPlayer()->m_bgEntryPointX = m_session->GetPlayer()->GetPositionX();
	m_session->GetPlayer()->m_bgEntryPointY = m_session->GetPlayer()->GetPositionY();
	m_session->GetPlayer()->m_bgEntryPointZ = m_session->GetPlayer()->GetPositionZ();
	m_session->GetPlayer()->m_bgEntryPointMap = m_session->GetPlayer()->GetMapId();
	m_session->GetPlayer()->m_bgEntryPointInstance = m_session->GetPlayer()->GetInstanceID();

	// send the battleground status packet to show estimated queue time and stuff
	SendBattlefieldStatus(m_session->GetPlayer(), BF_STATUS_PACKET_QUEUE_PLAYER, bgtype, instance, 0, BGMapIds[bgtype],0);
	//
	NotifyPlayersAboutStatus( m_session->GetPlayer() );
	m_queueLock.Release();

	// We will get updated next few seconds =) 
}

#define IS_ARENA(x) ( (x) >= BATTLEGROUND_ARENA_2V2 && (x) <= BATTLEGROUND_ARENA_5V5 )

void ErasePlayerFromList(uint32 guid, set<uint32>* l)
{
	for(set<uint32>::iterator itr = l->begin(); itr != l->end(); ++itr)
	{
		if((*itr) == guid)
		{
			l->erase(itr);
			return;
		}
	}
}

uint8 GetBattlegroundCaption(BattleGroundTypes bgType)
{
	switch(bgType)
	{
	case BATTLEGROUND_ALTERAC_VALLEY:
		return 38;
	case BATTLEGROUND_WARSONG_GULCH:
		return 39;
	case BATTLEGROUND_TWIN_PEAKS:
		return 82;
	case BATTLEGROUND_GILNEAS:
		return 83;
	case BATTLEGROUND_ARATHI_BASIN:
		return 40;
	case BATTLEGROUND_ARENA_2V2:
		return 41;
	case BATTLEGROUND_ARENA_3V3:
		return 42;
	case BATTLEGROUND_ARENA_5V5:
		return 43;
	case BATTLEGROUND_EYE_OF_THE_STORM:
		return 44;
	case BATTLEGROUND_STRAND_OF_THE_ANCIENT:
		return 34;
	default:
		return 45;
	}
}

void CBattlegroundManager::HandleGetBattlegroundQueueCommand(WorldSession * m_session)
{
	std::stringstream ss;

	uint32 i,j;
	Player * plr;
	set<uint32>::iterator it3, it4;
	m_queueLock.Acquire();

	bool foundSomething = false;

	for(i = 0; i < BATTLEGROUND_NUM_TYPES; ++i)
	{
		for(j = 0; j < MAX_LEVEL_GROUP; ++j)
		{
			if(!m_queuedPlayers[i][j].size())
				continue;

			foundSomething = true;

			ss << m_session->LocalizedWorldSrv(GetBattlegroundCaption((BattleGroundTypes)i));

			switch(j)
			{
			case 0:
				ss << " (<10)";
				break;
			case 1:
				ss << " (<20)";
				break;
			case 2:
				ss << " (<30)";
				break;
			case 3:
				ss << " (<40)";
				break;
			case 4:
				ss << " (<50)";
				break;
			case 5:
				ss << " (<60)";
				break;
			case 6:
				ss << " (<70)";
				break;
			case 7:
				ss << " (<80)";
				break;
			}

			ss << ": ";

			ss << (uint32)m_queuedPlayers[i][j].size() << " players queued";

			if(!IS_ARENA(i))
			{
				int ally = 0, horde = 0;

				for(it3 = m_queuedPlayers[i][j].begin(); it3 != m_queuedPlayers[i][j].end();)
				{
					it4 = it3++;
					plr = objmgr.GetPlayer(*it4);

					if(!plr || GetLevelGrouping(plr->getLevel()) != j)
					{
						continue;
					}

					if(plr->GetTeam() == 0)
						ally++;
					if(plr->GetTeam() == 1)
						horde++;
				}

				ss << " (Alliance: " << ally << " Horde: " << horde;
				if((int)m_queuedPlayers[i][j].size() > (ally + horde))
					ss << " Unknown: " << ((int)m_queuedPlayers[i][j].size() - ally - horde);
				ss << ")";
			}

			m_session->SystemMessage( ss.str().c_str() );
			ss.rdbuf()->str("");
		}

		if(IS_ARENA(i))
		{
			if(m_queuedGroups[i].size())
			{
				foundSomething = true;

				ss << m_session->LocalizedWorldSrv(GetBattlegroundCaption((BattleGroundTypes)i)) << " (rated): ";
				ss << (uint32)m_queuedGroups[i].size() << " groups queued";

				m_session->SystemMessage( ss.str().c_str() );
				ss.rdbuf()->str("");
			}
		}
	}

	m_queueLock.Release();

	if(!foundSomething)
		m_session->SystemMessage( "There's nobody queued." );
}

void CBattlegroundManager::EventQueueUpdate()
{
	this->EventQueueUpdate(false);
}

uint32 CBattlegroundManager::GetArenaGroupQInfo(Group * group, int type, uint32 *avgRating)
{
	ArenaTeam *team;
	ArenaTeamMember *atm;
	Player *plr;
	uint32 count=0;
	uint32 rating=0;

	if (group == NULL || group->GetLeader() == NULL) 
	{ 
		return 0;
	}

	plr = group->GetLeader()->m_loggedInPlayer;
	if (plr == NULL) 
	{ 
		return 0;
	}

	team = plr->m_arenaTeams[type-BATTLEGROUND_ARENA_2V2];
	if (team == NULL) 
	{ 
		return 0;
	}

	GroupMembersSet::iterator itx;
	for(itx = group->GetSubGroup(0)->GetGroupMembersBegin(); itx != group->GetSubGroup(0)->GetGroupMembersEnd(); ++itx)
	{
		plr = (*itx)->m_loggedInPlayer;
		if(plr)
		{
			if (team == plr->m_arenaTeams[type-BATTLEGROUND_ARENA_2V2])
			{
				atm = team->GetMemberByGuid(plr->GetLowGUID());
				if (atm)
				{
					rating+= atm->PersonalRating;
					count++;
				}
			}
		}
	}

	*avgRating = count > 0 ? rating/count : 0;

	return team ? team->m_id : 0;
}

void CBattlegroundManager::AddGroupToArena(CBattleground * bg, Group * group, int nteam)
{
	ArenaTeam *team;
	Player *plr;

	if (group == NULL || group->GetLeader() == NULL) 
	{ 
		return;
	}

	plr = group->GetLeader()->m_loggedInPlayer;
	if (plr == NULL) 
	{ 
		return;
	}

	team = plr->m_arenaTeams[bg->GetType()-BATTLEGROUND_ARENA_2V2];
	if (team == NULL) 
	{ 
		return;
	}

	GroupMembersSet::iterator itx;
	for(itx = group->GetSubGroup(0)->GetGroupMembersBegin(); itx != group->GetSubGroup(0)->GetGroupMembersEnd(); ++itx)
	{
		plr = (*itx)->m_loggedInPlayer;
		if(plr && team == plr->m_arenaTeams[bg->GetType()-BATTLEGROUND_ARENA_2V2])
		{
			if( bg->HasFreeSlots(nteam,bg->GetType()) )
			{
				bg->AddPlayer(plr, nteam);
				plr->SetTeam(nteam);
			}
		}
	}
}

Arena *CBattlegroundManager::CreateArenaType(int type, Group * group1, Group * group2)
{
	Arena * ar = static_cast<Arena*>(CreateInstance(type, LEVEL_GROUP_70));
	if (ar == NULL)
	{
		Log.Error("BattlegroundMgr", "%s (%u): Couldn't create Arena Instance", __FILE__, __LINE__);
		m_queueLock.Release();
		m_instanceLock.Release();
		return NULL;
	}
	ar->rated_match=true;

	AddGroupToArena(ar, group1, 0);
	AddGroupToArena(ar, group2, 1);

	return ar;
}

void CBattlegroundManager::AddPlayerToBg(CBattleground * bg, deque<uint32> *playerVec, uint32 i, uint32 j)
{
	uint32 plrguid = *playerVec->begin();
	playerVec->pop_front();
	Player *plr = objmgr.GetPlayer(plrguid);
	if(plr) {
		if(bg->CanPlayerJoin(plr,bg->GetType()))
		{
			bg->AddPlayer(plr, plr->GetTeam());
			ErasePlayerFromList(plr->GetLowGUID(), &m_queuedPlayers[i][j]);
		}
		else
		{
			// Put again the player in the queue
			playerVec->push_back(plrguid);
		}
	}
	else
	{
		ErasePlayerFromList(plrguid, &m_queuedPlayers[i][j]);
	}
}

void CBattlegroundManager::AddPlayerToBgTeam(CBattleground * bg, deque<uint32> *playerVec, uint32 i, uint32 j, int Team)
{
	if (bg->HasFreeSlots(Team,bg->GetType()))
	{
		uint32 plrguid = *playerVec->begin();
		playerVec->pop_front();
		Player *plr = objmgr.GetPlayer(plrguid);
		if(plr)
		{
			plr->m_bgTeam=Team;
			bg->AddPlayer(plr, Team);
		}
		ErasePlayerFromList(plrguid, &m_queuedPlayers[i][j]);
	}
}

void CBattlegroundManager::EventQueueUpdate(bool forceStart)
{
	deque<uint32> tempPlayerVec[2];
	uint32 i,j,k;
	Player * plr;
	CBattleground * bg;
	set<uint32>::iterator it3, it4;
	map<uint32, CBattleground*>::iterator iitr;
	Arena * arena;
	int32 team;
	uint32 plrguid;
	m_queueLock.Acquire();
	m_instanceLock.Acquire();

	for(i = 0; i < BATTLEGROUND_NUM_TYPES; ++i)
	{
		for(j = 0; j < MAX_LEVEL_GROUP; ++j)
		{
			if( m_queuedPlayers[i][j].empty() )
				continue;

			tempPlayerVec[0].clear();
			tempPlayerVec[1].clear();

			for(it3 = m_queuedPlayers[i][j].begin(); it3 != m_queuedPlayers[i][j].end();)
			{
				it4 = it3++;
				plrguid = *it4;
				plr = objmgr.GetPlayer(plrguid);

				if(!plr || GetLevelGrouping(plr->getLevel()) != j || plr->m_bg )
				{
					if( plr && plr->m_bg )
						plr->BroadcastMessage("You are already inside of a BG, removing from queue");
					m_queuedPlayers[i][j].erase(it4);
					continue;
				}

#ifdef BATTLEGRUND_REALM_BUILD
				int64 *queue_last_reward_stamp = plr->GetCreateIn64Extension( EXTENSION_ID_BG_REALM_QUEU_REW_STAMP );
				if( *queue_last_reward_stamp + 10*60*1000 < getMSTime() )
				{
					*queue_last_reward_stamp = getMSTime();
					uint32 xp_this_level = 20 * plr->GetUInt32Value( PLAYER_NEXT_LEVEL_XP ) / 100;
					plr->BroadcastMessage( "BG queue loneliness gives you %d XP", xp_this_level );
					plr->GiveXP( xp_this_level, plr->GetGUID(), false );
				}
#endif
				// queued to a specific instance id?
				if(plr->m_bgQueueInstanceId != 0)
				{
					iitr = m_instances[i].find(plr->m_bgQueueInstanceId);
					if(iitr == m_instances[i].end())
					{
						// queue no longer valid
						plr->GetSession()->SystemMessage(plr->GetSession()->LocalizedWorldSrv(52), plr->m_bgQueueInstanceId);
						plr->m_bgIsQueued = false;
						plr->m_bgQueueType = 0;
						plr->m_bgQueueInstanceId = 0;
						m_queuedPlayers[i][j].erase(it4);
					}

					// can we join?
					bg = iitr->second;
					if(bg->CanPlayerJoin(plr,bg->GetType()))
					{
						bg->AddPlayer(plr, plr->GetTeam());
						m_queuedPlayers[i][j].erase(it4);
					}
				}
				else
				{
					if(IS_ARENA(i))
						tempPlayerVec[0].push_back(plrguid);
					else if (!plr->HasAura(BG_DESERTER))
						tempPlayerVec[plr->GetTeam()].push_back(plrguid);
				}
			}

			// try to join existing instances
			uint32 ListSize;
			uint32 BGTryJoinList[50];
			if( i == BATTLEGROUND_ALL_BG )
			{
				ListSize = 5;
				BGTryJoinList[0] = BATTLEGROUND_WARSONG_GULCH;
				BGTryJoinList[1] = BATTLEGROUND_ARATHI_BASIN;
				BGTryJoinList[2] = BATTLEGROUND_EYE_OF_THE_STORM;
				BGTryJoinList[3] = BATTLEGROUND_TWIN_PEAKS;
				BGTryJoinList[4] = BATTLEGROUND_GILNEAS;
			}
			else
			{
				ListSize = 1;
				BGTryJoinList[0] = i;
			}
			for( uint32 SearchIndex=0;SearchIndex<ListSize;SearchIndex++)
			{
				uint32 TryJoinBGType = BGTryJoinList[ SearchIndex ];
				for(iitr = m_instances[TryJoinBGType].begin(); iitr != m_instances[TryJoinBGType].end(); ++iitr)
				{
					if( iitr->second->HasEnded() || iitr->second->GetLevelGroup() != j )
						continue;

					if(IS_ARENA(i))
					{
						arena = static_cast<Arena*>(iitr->second);

						//we cannot join an already started rated match. Only skirmish
						if(arena->Rated())
							continue;

						team = arena->GetFreeTeam();
						while(team >= 0 && tempPlayerVec[0].empty() == false )
						{
							plrguid = *tempPlayerVec[0].begin();
							tempPlayerVec[0].pop_front();
							plr = objmgr.GetPlayer(plrguid);
							if(plr)
							{
								plr->m_bgTeam=team;
								arena->AddPlayer(plr, team);
								team = arena->GetFreeTeam();
							}
							ErasePlayerFromList(plrguid, &m_queuedPlayers[i][j]);
						}
					}
					else
					{
						bg = iitr->second;
						
						//let's try to adjust sides so player count is even 
						while (tempPlayerVec[0].empty() == false && bg->GetBalanceFreeSlots(0, bg->GetType()))
							AddPlayerToBgTeam(bg, &tempPlayerVec[0], i, j, 0);
						while (tempPlayerVec[1].empty() == false && bg->GetBalanceFreeSlots(1, bg->GetType()))
							AddPlayerToBgTeam(bg, &tempPlayerVec[1], i, j, 1);

						// if we still have open slots and players on both sides then we start adding them
						int size = (int)min(tempPlayerVec[0].size(),tempPlayerVec[1].size());
						for(int counter = 0; (counter < size) && (bg->IsFull() == false); counter++)
						{
							AddPlayerToBgTeam(bg, &tempPlayerVec[0], i, j, 0);
							AddPlayerToBgTeam(bg, &tempPlayerVec[1], i, j, 1);
						}
					}
				}
			}

			if(IS_ARENA(i))
			{
				// enough players to start a round?
				uint32 minPlayers = BattlegroundManager.GetMinimumPlayers(i);
				if(!forceStart && tempPlayerVec[0].size() < minPlayers)
					continue;

				if(CanCreateInstance(i,j))
				{
					arena = static_cast<Arena*>(CreateInstance(i, j));
					if ( arena == NULL )
					{
						Log.Error("BattlegroundMgr", "%s (%u): Couldn't create Arena Instance", __FILE__, __LINE__);
						m_queueLock.Release();
						m_instanceLock.Release();
 						return;
					}

					team = arena->GetFreeTeam();
					while(!arena->IsFull() && tempPlayerVec[0].size() && team >= 0)
					{
						plrguid = *tempPlayerVec[0].begin();
						tempPlayerVec[0].pop_front();
						plr = objmgr.GetPlayer(plrguid);

						if(plr)
						{
							plr->m_bgTeam=team;
							arena->AddPlayer(plr, team);
							team = arena->GetFreeTeam();
						}

						// remove from the main queue (painful!)
						ErasePlayerFromList(plrguid, &m_queuedPlayers[i][j]);
					}
				}
			}
			else
			{
				uint32 minPlayers = BattlegroundManager.GetMinimumPlayers(i);
//				minPlayers = minPlayers / 2;
				minPlayers = 5;
				if(forceStart || (tempPlayerVec[0].size() >= minPlayers && tempPlayerVec[1].size() >= minPlayers))
				{
					if(CanCreateInstance(i,j))
					{
						bg = CreateInstance(i,j);
						if ( bg == NULL )
						{
							m_queueLock.Release();
							m_instanceLock.Release();
 							return;
						}

						// push as many as possible in
						if (forceStart)
						{
							for(k = 0; k < 2; ++k)
							{
								while(tempPlayerVec[k].empty() == false && bg->HasFreeSlots(k, bg->GetType()))
								{
									AddPlayerToBgTeam(bg, &tempPlayerVec[k], i, j, k);
								}
							}
						}
						else
						{
							int size = (int)min(tempPlayerVec[0].size(),tempPlayerVec[1].size());
							for(int counter = 0; (counter < size) && (bg->IsFull() == false); counter++)
							{
								AddPlayerToBgTeam(bg, &tempPlayerVec[0], i, j, 0);
								AddPlayerToBgTeam(bg, &tempPlayerVec[1], i, j, 1);
							}
						}
					}
				}
			}
		}
	}

	/* Handle paired arena team joining */
	Group * group1, *group2;
	set<uint32>::iterator itz;
	for(i = BATTLEGROUND_ARENA_2V2; i <= BATTLEGROUND_ARENA_5V5; ++i)
	{
		uint32 GroupCount = m_queuedGroups[i].size();
		if(!forceStart && GroupCount < 2)      /* got enough to have an arena battle ;P */
		{
			continue;
		}
		if( forceStart && GroupCount == 1 )
		{
			itz = m_queuedGroups[i].begin();
			group1 = objmgr.GetGroupById(*itz);
			if (group1 == NULL)
			{
				continue;
			}
			if( CreateArenaType(i, group1, NULL) == NULL ) 
				return;
			m_queuedGroups[i].clear();
			continue;
		}

		if( GroupCount < 500 )
		{
			//get the rating of the group and get the best matching group
			struct GroupRatingPair
			{
				uint32	GroupID;
				uint32	TeamId;
				uint32	AvgRating;
				bool	Available;
			}; 
			GroupRatingPair Pairs[500];
			uint32	IndexCounter = 0;
			for(itz = m_queuedGroups[i].begin(); itz != m_queuedGroups[i].end();)
			{
				uint32 GroupID = *itz;
				itz++;
				group1 = objmgr.GetGroupById( GroupID );
				if (group1 == NULL)
				{
//					m_queuedGroups[i].remove( GroupID );
					continue;
				}
				Pairs[ IndexCounter ].GroupID = GroupID;
				Pairs[ IndexCounter ].TeamId = GetArenaGroupQInfo( group1, i, &Pairs[ IndexCounter ].AvgRating );
				IndexCounter++;
			}
			//get best matching teams
			for( uint32 ti=0; ti<GroupCount; ti++ )
				if( Pairs[ ti ].GroupID != 0 )
				{
					int32 BestDeltaRating = 0;
					int32 BestDeltaIndex = 0;
					for( uint32 tj= ti + 1; tj<GroupCount; tj++ )
						if( Pairs[ ti ].TeamId != Pairs[ tj ].TeamId && Pairs[ tj ].GroupID != 0 && Pairs[ tj ].GroupID != Pairs[ ti ].GroupID )
						{
							int32 DeltaRating = abs( (int32)Pairs[ ti ].AvgRating - (int32)Pairs[ tj ].AvgRating );
							if( DeltaRating < BestDeltaRating && DeltaRating > (int32)sWorld.ArenaQueueDiff )
							{
								BestDeltaRating = DeltaRating; 
								BestDeltaIndex = tj;
							}
						}
					if( Pairs[ ti ].TeamId != Pairs[ BestDeltaIndex ].TeamId && Pairs[ BestDeltaIndex ].GroupID != 0 && Pairs[ BestDeltaIndex ].GroupID != Pairs[ ti ].GroupID )
					{
						group1 = objmgr.GetGroupById( Pairs[ ti ].GroupID );
						group2 = objmgr.GetGroupById( Pairs[ BestDeltaIndex ].GroupID );
						if( CreateArenaType(i, group1, group2) == NULL )
							break;
						Pairs[ ti ].GroupID = 0;
						Pairs[ BestDeltaIndex ].GroupID = 0;
					}
				}
			//push back remaining groups in queue
			m_queuedGroups[i].clear();
			for( uint32 ti=0; ti<GroupCount; ti++ )
				if( Pairs[ ti ].GroupID != 0 )
					m_queuedGroups[i].insert( Pairs[ ti ].GroupID );
		}
#if 0
		else
		{
			uint32 teamids[2] = {0,0};
			uint32 avgRating[2] = {0,0};
			uint32 n;
			//get the rating of the group and get the best matching group
			for (j=0; j<(uint32)GroupCount; j++)
			{
				group1 = group2 = NULL;
				n =	RandomUInt((uint32)GroupCount) - 1;
				for(itz = m_queuedGroups[i].begin(); itz != m_queuedGroups[i].end() && n>0; ++itz)
					--n;

				if(itz == m_queuedGroups[i].end())
					itz=m_queuedGroups[i].begin();

				if(itz == m_queuedGroups[i].end())
				{
					Log.Error("BattlegroundMgr", "Internal error at %s:%u", __FILE__, __LINE__);
					m_queueLock.Release();
					m_instanceLock.Release();
					return;
				}

				group1 = objmgr.GetGroupById(*itz);
				if (group1 == NULL)
				{
					continue;
				}

				teamids[0] = GetArenaGroupQInfo(group1, i, &avgRating[0]);

				list<uint32> possibleGroups;
				for(itz = m_queuedGroups[i].begin(); itz != m_queuedGroups[i].end(); ++itz)
				{
					group2 = objmgr.GetGroupById(*itz);
					if (group2)
					{
						teamids[1] = GetArenaGroupQInfo(group2, i, &avgRating[1]);
						uint32 delta = abs((int32)avgRating[0] - (int32)avgRating[1]);
						if (teamids[0] != teamids[1] && delta <= sWorld.ArenaQueueDiff)
						{
							possibleGroups.push_back(group2->GetID());
						}
					}
				}

				if (possibleGroups.size() > 0)
				{
					n = RandomUInt((uint32)possibleGroups.size()) - 1;
					for(itz = possibleGroups.begin(); itz != possibleGroups.end() && n>0; ++itz)
						--n;

					if(itz == possibleGroups.end())
						itz=possibleGroups.begin();

					if(itz == possibleGroups.end())
					{
						Log.Error("BattlegroundMgr", "Internal error at %s:%u", __FILE__, __LINE__);
						m_queueLock.Release();
						m_instanceLock.Release();
						return;
					}

					group2 = objmgr.GetGroupById(*itz);
					if (group2)
					{
						if (CreateArenaType(i, group1, group2) == -1) return;
						m_queuedGroups[i].remove(group1->GetID());
						m_queuedGroups[i].remove(group2->GetID());
					}
				}
			}
		}
#endif
	}

	m_queueLock.Release();
	m_instanceLock.Release();
}

void CBattlegroundManager::NotifyPlayersAboutStatus(Player *plr_change)
{
	//sadly we need info about him
	if( !plr_change )
	{ 
		return;
	}
	set<uint32>::iterator itr;
	int32 sides[3]={0,0,0};	//actually 2. 1 is safety :p
	uint32 lgroup = GetLevelGrouping(plr_change->getLevel());
	itr = m_queuedPlayers[plr_change->m_bgQueueType][lgroup].begin();
	while(itr != m_queuedPlayers[plr_change->m_bgQueueType][lgroup].end())
	{
		Player *tplr = objmgr.GetPlayer(*itr);
		if( tplr )
			sides[ tplr->GetTeam() ]++;
		++itr;
	}
	char msg_to_send[500];
	uint32 minPlayers = BattlegroundManager.GetMinimumPlayers(plr_change->m_bgQueueType);
	sprintf( msg_to_send,"Battleground : Player %s changed the queue. Hordes in queue %u and Alliance in queue %u.Need %u to start",plr_change->GetName(),sides[1],sides[0],minPlayers);
	itr = m_queuedPlayers[plr_change->m_bgQueueType][lgroup].begin();
	while(itr != m_queuedPlayers[plr_change->m_bgQueueType][lgroup].end())
	{
		Player *tplr = objmgr.GetPlayer(*itr);
		if( tplr && tplr->IsInWorld() )
			tplr->BroadcastMessage( msg_to_send );
		++itr;
	}
}

void CBattlegroundManager::RemovePlayerFromQueues(Player * plr)
{
	m_queueLock.Acquire();

	ASSERT(plr->m_bgQueueType < BATTLEGROUND_NUM_TYPES);
	uint32 lgroup = GetLevelGrouping(plr->getLevel());
	set<uint32>::iterator itr;

	bool removed_from_list = false;
	itr = m_queuedPlayers[plr->m_bgQueueType][lgroup].begin();
	while(itr != m_queuedPlayers[plr->m_bgQueueType][lgroup].end())
	{
		if((*itr) == plr->GetLowGUID())
		{
			Log.Debug("BattlegroundManager", "Removing player %u from queue instance %u type %u", plr->GetLowGUID(), plr->m_bgQueueInstanceId, plr->m_bgQueueType);
			m_queuedPlayers[plr->m_bgQueueType][lgroup].erase(itr);
			plr->BroadcastMessage("You have left BattleGround queue.");
			removed_from_list = true;
			break;
		}
		++itr;
	}
	if( removed_from_list )
		NotifyPlayersAboutStatus( plr );

	plr->m_bgIsQueued = false;
	plr->m_bgTeam=plr->GetTeam();
	plr->m_pendingBattleground=0;
	SendBattlefieldStatus(plr,BF_STATUS_PACKET_REMOVE_PLAYER,0,0,0,0,0);
	m_queueLock.Release();

	Group* group;
	group = plr->GetGroup();
	if (group) //if da niggas in a group, boot dis bitch ass' group outa da q
	{
		Log.Debug("BattlegroundManager", "Player %u removed whilst in a group. Removing players group %u from queue", plr->GetLowGUID(), group->GetID());
		RemoveGroupFromQueues(group);
	}
}

void CBattlegroundManager::RemoveGroupFromQueues(Group * grp)
{
	m_queueLock.Acquire();
	for(uint32 i = BATTLEGROUND_ARENA_2V2; i < BATTLEGROUND_ARENA_5V5+1; ++i)
	{
		for(set<uint32>::iterator itr = m_queuedGroups[i].begin(); itr != m_queuedGroups[i].end(); )
		{
			if((*itr) == grp->GetID())
				itr = m_queuedGroups[i].erase(itr);
			else
				++itr;
		}
	}

	grp->m_isqueued = false;

	for(GroupMembersSet::iterator itr = grp->GetSubGroup(0)->GetGroupMembersBegin(); itr != grp->GetSubGroup(0)->GetGroupMembersEnd(); ++itr)
		if( (*itr)->m_loggedInPlayer && (*itr)->m_loggedInPlayer->m_bgIsQueued == true )
//			SendBattlefieldStatus((*itr)->m_loggedInPlayer, BF_STATUS_PACKET_REMOVE_PLAYER, 0, 0, 0, 0, 0);
			RemovePlayerFromQueues( (*itr)->m_loggedInPlayer );

	m_queueLock.Release();
}


bool CBattlegroundManager::CanCreateInstance(uint32 Type, uint32 LevelGroup)
{
	/*uint32 lc = 0;
	for(map<uint32, CBattleground*>::iterator itr = m_instances[Type].begin(); itr != m_instances[Type].end(); ++itr)
	{
	if(itr->second->GetLevelGroup() == LevelGroup)
	{
	lc++;
	if(lc >= MAXIMUM_BATTLEGROUNDS_PER_LEVEL_GROUP)
	return false;
	}
	}*/

	return true;
}

/* Returns the minimum number of players (Only valid for battlegrounds) */
uint32 CBattlegroundManager::GetMinimumPlayers(uint32 dbcIndex)
{
	if( dbcIndex == BATTLEGROUND_ARENA_2V2 )
		return 2;
	else if( dbcIndex == BATTLEGROUND_ARENA_3V3 )
		return 3;
	else if( dbcIndex == BATTLEGROUND_ARENA_5V5 )
		return 5;
	else if( dbcIndex == BGDBC_WARSONG_GULCH )
		return 5;
	else if( dbcIndex == BGDBC_TWIN_PEAKS )
		return 5;
	else if( dbcIndex == BGDBC_BATTLE_FOR_GILNEAS )
		return 5;
	else if( dbcIndex == BGDBC_ALTERAC_VALLEY )
		return 10;
	else if( dbcIndex == BGDBC_ARATHI_BASIN )
		return 15;
	else if( dbcIndex == BGDBC_EYE_OF_THE_STORM )
		return 10;
	else if( dbcIndex == BGDBC_STRAND_OF_THE_ANCIENT )
		return 10;
	else if( dbcIndex == BGDBC_RANDOM_BG )
		return 10;
	return 5;
}

uint32 CBattlegroundManager::GetMaximumPlayersDBC(uint32 dbcIndex)
{
	if( dbcIndex == BGDBC_ARENA_NAGRAND )
		return 5;
	else if( dbcIndex == BGDBC_ARENA_BLADES_EDGE )
		return 5;
	else if( dbcIndex == BGDBC_ARENA_ALLMAPS )
		return 5;
	else if( dbcIndex == BGDBC_DALARAN_SEWERS )
		return 5;
	else if( dbcIndex == BGDBC_RING_OF_VALOR )
		return 5;
	else 
		return GetMaximumPlayers( dbcIndex );
}

/* Returns the maximum number of players (Only valid for battlegrounds) */
uint32 CBattlegroundManager::GetMaximumPlayers(uint32 dbcIndex)
{
	if( dbcIndex == BATTLEGROUND_ARENA_2V2 )
		return 2;
	else if( dbcIndex == BATTLEGROUND_ARENA_3V3 )
		return 3;
	else if( dbcIndex == BATTLEGROUND_ARENA_5V5 )
		return 5;
	else if( dbcIndex == BGDBC_WARSONG_GULCH )
		return 10;
	else if( dbcIndex == BGDBC_TWIN_PEAKS )
		return 10;
	else if( dbcIndex == BGDBC_BATTLE_FOR_GILNEAS )
		return 10;
	else if( dbcIndex == BGDBC_ALTERAC_VALLEY )
		return 64;
	else if( dbcIndex == BGDBC_ARATHI_BASIN )
		return 15;
	else if( dbcIndex == BGDBC_EYE_OF_THE_STORM )
		return 15;
	else if( dbcIndex == BGDBC_STRAND_OF_THE_ANCIENT )
		return 15;
	return 10;
}

void CBattleground::SendWorldStates(Player * plr)
{ 
	m_mapMgr->SendInitialWorldState( plr ); 
}

void CBattleground::SetWorldState(uint32 Index, uint32 Value)
{ 
	m_mapMgr->SetWorldState( Index, Value ); 
}

CBattleground::CBattleground(MapMgr * mgr, uint32 id, uint32 levelgroup, uint32 type) : m_mapMgr(mgr), m_id(id), m_type(type), m_levelGroup(levelgroup)
{
	m_isRandomBG = 0;
	m_isWeekend = 0;
	m_nextPvPUpdateTime = 0;
//	m_countdownStage = 0;
	m_WarmupExpireStamp = 0;
	m_ended = false;
	m_started = false;
	m_winningteam = 0;
	m_startTime = (uint32)UNIXTIME;
	m_lastResurrect = (uint32)UNIXTIME;
	//m_invisGMs = 0;
	sEventMgr.AddEvent(this, &CBattleground::EventResurrectPlayers, EVENT_BATTLEGROUND_QUEUE_UPDATE, 30000, 0,0);

	/* create raid groups */
	for(uint32 i = 0; i < 2; ++i)
	{
		m_groups[i] = new Group(true);
		m_groups[i]->m_disbandOnNoMembers = false;
		if( IS_ARENA( GetType() ) == false )
			m_groups[i]->ExpandToRaid( true );
	}

	m_honorPerKill = HonorHandler::CalculateHonorPointsForKill(m_levelGroup * 10, m_levelGroup * 10);

	m_SkipRewards = false;
	m_PlayerResurrects = 0x00FFFFFF;
	m_TeamResurrects = 0x00FFFFFF;
	m_TeamResurrectsDone = 0;
	m_lastResurrect = 0;
	m_SuddenDeath = false;
	m_ScriptCPReward = 0;
	m_ArenaTeamNames[ 0 ] = m_ArenaTeamNames[ 1 ] = NULL;
}

CBattleground::~CBattleground()
{
	m_mainLock.Acquire();
	sEventMgr.RemoveEvents(this);
	for(uint32 i = 0; i < 2; ++i)
	{
		if( m_groups[i]->DisbandOnBGEnd() == false )
		{
			m_groups[i] = NULL;
			continue;
		}
		PlayerInfo *inf;
		for(uint32 j = 0; j < m_groups[i]->GetSubGroupCount(); ++j)
		{
			for(GroupMembersSet::iterator itr = m_groups[i]->GetSubGroup(j)->GetGroupMembersBegin(); itr != m_groups[i]->GetSubGroup(j)->GetGroupMembersEnd();)
			{
				inf = (*itr);
				++itr;
				m_groups[i]->RemovePlayer(inf);
			}
		}
		delete m_groups[i];
		m_groups[i] = NULL;
	}

	m_resurrectMap.clear();
	//make sure player have no link to us. No idea why tey would since we kicked them out already
	for(uint32 i = 0; i < 2; ++i)
		for(InRangePlayerSet::iterator itr = m_players[i].begin(); itr != m_players[i].end(); ++itr)
			if( (*itr)->m_bg == this )
				(*itr)->m_bg = NULL;
	m_players[0].clear();
	m_players[1].clear();	
	m_mainLock.Release();
}

void CBattleground::UpdatePvPData()
{
	if(m_type >= BATTLEGROUND_ARENA_2V2 && m_type <= BATTLEGROUND_ARENA_5V5)
	{
//		if(!m_ended)
/*		if( m_started == false ) //some say after the fight starts you should see others
		{
			return;
		} */
	}

	if(UNIXTIME >= m_nextPvPUpdateTime)
	{
		m_mainLock.Acquire();
		WorldPacket data(10*(m_players[0].size()+m_players[1].size())+50);
		BuildPvPUpdateDataPacket(&data);
		DistributePacketToAll(&data);
		m_mainLock.Release();

		m_nextPvPUpdateTime = UNIXTIME + 2;
	}
}

void CBattleground::BuildPvPUpdateDataPacket(WorldPacket * data)
{
	data->Initialize(MSG_PVP_LOG_DATA);
	data->reserve(10*(m_players[0].size()+m_players[1].size())+50);

	BGScore * bs;
//	bool is_arena = IS_ARENA( GetType() ) ;
	bool is_arena = false;
	// !!! arena guid /struct mask is different. Need to figure it out ( if it is worth it )
//	if(m_type >= BATTLEGROUND_ARENA_2V2 && m_type <= BATTLEGROUND_ARENA_5V5)
//		is_arena=true;
	{
		// bit 7 ended, calls some function, 
		// bit 6 also signals some boolean and makes client read an uint8
		// bit 5 signals presence of a structure at the end of the packet, 6*uint32
		uint8 HeaderBits = 0;
//		if( Rated() )
//			HeaderBits |= 0x20;	//signal rating presence packet
		if( m_ended )
			HeaderBits |= 0x40;	
		if( is_arena )
			HeaderBits |= 0x80;	//send player names

		*data << uint8( HeaderBits );      // If the game has ended - this will be 1

		if( HeaderBits & 0x80 ) 
		{
			if( m_ArenaTeamNames[ 0 ] != NULL )
				*data << m_ArenaTeamNames[ 0 ];	//Player1
			else
				*data << "Player1";	//Player1
			if( m_ArenaTeamNames[ 1 ] != NULL )
				*data << m_ArenaTeamNames[ 1 ];	//Player1
			else
				*data << "Player2";	//Player2
		}
		if( HeaderBits & 0x40 ) 
		{
			if( m_winningteam < 0 || m_winningteam > 1 )
				*data << uint8( 1 ); //debugging, copy paste from blizz
			else
				*data << uint8( 1 - m_winningteam ); //this just got to be the winning team id. I simply got 1 in the packets :(
		}

		//after match ends this number does not match. Maybe represents ingame team size ? Like group size
		*data << uint8( m_players[0].size() ); //seems to change when one of the team sizes changes. No idea which team size is represented here

//		uint32 count = ((uint32)m_players[0].size() + (uint32)m_players[1].size())-m_invisGMs;
		uint32 count = ((uint32)m_players[0].size() + (uint32)m_players[1].size());
		*data << uint32( count );

		//bit_7 signals presence of GO6( i think )
		//bit_10 signals team
		for(uint32 i = 0; i < 2; ++i)
			for(InRangePlayerSet::iterator itr = m_players[i].begin(); itr != m_players[i].end(); ++itr)
			{
				if( (*itr)->IsGMInvisible() )
					continue;
				if( is_arena == true )//this is not good, no longer shows team !
				{
					data->writeBits( 0x07, 8 );
					if( (*itr)->GetTeam() )
						data->writeBits( 0x6A >> (8-7), 7 );//Horde mask
					else
						data->writeBits( 0x6E >> (8-7), 7);//Aliance mask
				}
				else
				{
					data->writeBits( 0x27, 8 );
					if( (*itr)->GetTeam() )
						data->writeBits( 0x42 >> (8-7), 7 );//green team mask
					else
						data->writeBits( 0x46 >> (8-7), 7);//gold team mask
				}
			}
		data->flushBits();

		uint32 FieldCount = GetFieldCount( GetType() );
		for(uint32 i = 0; i < 2; ++i)
		{
			for(InRangePlayerSet::iterator itr = m_players[i].begin(); itr != m_players[i].end(); ++itr)
			{
				if( (*itr)->IsGMInvisible() )
					continue;
				bs = &(*itr)->m_bgScore;
				uint8 guid[8];
				*(uint64*)guid = (*itr)->GetGUID();
				
				*data << ObfuscateByte( guid[7] );
				*data << ObfuscateByte( guid[3] );
				*data << bs->HonorableKills;
				*data << bs->BonusHonor;
				if( is_arena == false )
				{
					*data << bs->Deaths;
					*data << bs->KillingBlows;
					*data << uint32( (*itr)->m_specs[ (*itr)->m_talentActiveSpec ].tree_lock );	//ex value 807
				}
				*data << FieldCount;
				*data << bs->HealingDone;
				*data << ObfuscateByte( guid[2] );
				*data << ObfuscateByte( guid[0] );
				if( is_arena )
					*data << uint32( ARENA_CURRENCY_WIN_AMT2 );	
				*data << ObfuscateByte(  guid[1] );
				*data << bs->DamageDone;
				for( uint32 x = 0; x < FieldCount; ++x )
					*data << bs->MiscData[x];
			}
		}
		//here 
//		for(uint i = 0; i < m_players[0].size()+m_players[1].size();i++) data << ??;
		//seems to be right
		*data << uint8( m_players[1].size() ); //?
		if( HeaderBits & 0x20 )
		{
/*
  if ( *(_BYTE *)(this1 + 216) )
  {
      Read_uint32(v75 + 8);
      Read_uint32(v76 + 4);
      Read_uint32(v77 + 12);
      Read_uint32(v78);
      Read_uint32(v79 + 20);
      Read_uint32(v80 + 16);
  }
00 00 00 00 
57 05 00 00 
00 00 00 00 
B2 04 00 00 
60 00 00 00 
00 00 00 00 
            *data << uint32(MatchmakerRating);              // Matchmaking Value
            *data << uint32(pointsLost);                    // Rating Lost
            *data << uint32(pointsGained);                  // Rating gained
*/
			int32 RequiredFieldsCount = 6;
/*			for(uint32 i = 0; i < 2; ++i)
				for(InRangePlayerSet::iterator itr = m_players[i].begin(); itr != m_players[i].end(); ++itr)
				{
					if( RequiredFieldsCount == 0 )
						break;
					if( (*itr)->IsGMInvisible() )
						continue;
					bs = &(*itr)->m_bgScore;
					*data << bs->RatingChange;
					RequiredFieldsCount--;
				}*/
			for( ;RequiredFieldsCount>0;RequiredFieldsCount--)
				*data << uint32( RequiredFieldsCount + 10 );
		}
	}
}
void CBattleground::AddPlayer(Player * plr, uint32 team, bool WithInit)
{
	if( WithInit )
	{
		plr->m_bgIsQueued = true;
		plr->m_bgQueueInstanceId = 0;
		plr->m_bgQueueType = GetType();
		plr->SetTeam( team );
	}

	//maybe we teleported since we queued
	if( plr->GetMapMgr() && plr->GetMapMgr()->GetMapInfo() && IS_STATIC_MAP( plr->GetMapMgr()->GetMapInfo() ) )
	{
		plr->m_bgEntryPointX = plr->GetPositionX();
		plr->m_bgEntryPointY = plr->GetPositionY();
		plr->m_bgEntryPointZ = plr->GetPositionZ();
		plr->m_bgEntryPointMap = plr->GetMapId();
	}

	m_mainLock.Acquire();

	/* This is called when the player is added, not when they port. So, they're essentially still queued, but not inside the bg yet */
	m_pendPlayers[team].insert(plr->GetLowGUID());

	/* Send a packet telling them that they can enter */
	BattlegroundManager.SendBattlefieldStatus(plr, BF_STATUS_PACKET_ALLOW_ENTER_PLAYER, m_type, m_id, 80000, m_mapMgr->GetMapId(),Rated());      // You will be removed from the queue in 2 minutes.

	/* Add an event to remove them in 1 minute 20 seconds time. */
	sEventMgr.AddEvent(this, &CBattleground::EventRemovePendingPlayer, plr->GetLowGUID(), EVENT_BATTLEGROUND_QUEUE_UPDATE, 80000, 1,0);
	plr->m_pendingBattleground = this;

	m_mainLock.Release();
}

void CBattleground::EventRemovePendingPlayer(uint32 lowguid)
{
	Player *plr = objmgr.GetPlayer( lowguid );
	if( plr )
		RemovePendingPlayer( plr );
}

void CBattleground::RemovePendingPlayer(Player * plr)
{
	m_mainLock.Acquire();
	m_pendPlayers[plr->GetTeam()].erase(plr->GetLowGUID());

	/* send a null bg update (so they don't join) */
	BattlegroundManager.SendBattlefieldStatus(plr, BF_STATUS_PACKET_REMOVE_PLAYER, 0, 0, 0, 0,0);
	plr->m_pendingBattleground =0;
	plr->m_bgTeam=plr->GetTeam();	//why on earth ? we use everywhere getteam() :S

	m_mainLock.Release();
}

void CBattleground::OnPlayerPushed(Player * plr)
{
	//no idea how this happens. Better be sure then sorry
	if( plr->m_bg != this 
		|| plr->GetSession() == NULL ) //maybe remove from BG also ?
		return;

	if( plr->GetGroup() && !Rated() && !m_SkipRewards )
		plr->GetGroup()->RemovePlayer( plr->getPlayerInfo(), true );

	plr->ProcessPendingUpdates();

	if( plr->GetGroup() == NULL )
	{
		if ( plr->IsGMInvisible() == false && m_groups[ plr->m_bgTeam ] ) //do not join invisible gm's into bg groups.
		{
//			if( m_groups[ plr->m_bgTeam ]->IsFull() )
//				m_groups[ plr->m_bgTeam ]->ExpandToRaid( true );
			m_groups[ plr->m_bgTeam ]->AddMember( plr->getPlayerInfo() );
		}
	}
//	else
//		plr->GetGroup()->ExpandToRaid( true );	//arenas use this also to avoid players quitting the group

	if( plr->GetSession() == NULL )
		return;

	//after new world has been sent
	if ( plr->IsGMInvisible() == false )
	{
//		BattlegroundManager.SendBattlefieldStatus(plr, BF_STATUS_PACKET_PLAYER_JOINED, m_type, m_id,(uint32)UNIXTIME - m_startTime, m_mapMgr->GetMapId(),Rated());   // Elapsed time is the last argument
		//Do not let everyone know an invisible gm has joined.
		WorldPacket data( SMSG_BATTLEGROUND_PLAYER_JOINED, 8);
		uint64 guid = plr->GetGUID();
		uint8 *guid_bytes = (uint8*)&guid;
		data << uint8( 0x4F );
        data << ObfuscateByte( guid_bytes[3] );   
        data << ObfuscateByte( guid_bytes[7] );   
        data << ObfuscateByte( guid_bytes[1] );   
        data << ObfuscateByte( guid_bytes[2] );   
        data << ObfuscateByte( guid_bytes[0] );   
		DistributePacketToTeam(&data,plr->m_bgTeam);
	}
	else
	{
//		m_invisGMs++;
	}

	//send the remaining warmup time( if there is any )
	if( getMSTime() < m_WarmupExpireStamp )
	{
		sStackWorldPacket( data, SMSG_BATTLEFIELD_TIMER, 12 + 10 );
		data << uint32( 0 );	//?
		data << uint32( WARMUP_WAITTIME / 1000 );	//timer bar total
		data << uint32( (m_WarmupExpireStamp-getMSTime()) / 1000 );	//timer bar remaining
		plr->GetSession()->SendPacket( &data );
	}
}

void CBattleground::PortPlayer(Player * plr, bool skip_teleport /* = false*/)
{
	m_mainLock.Acquire();
	if(m_ended)
	{
		sChatHandler.SystemMessage(plr->GetSession(), plr->GetSession()->LocalizedWorldSrv(53) );
		BattlegroundManager.SendBattlefieldStatus(plr, BF_STATUS_PACKET_REMOVE_PLAYER, 0, 0, 0, 0,0);
		plr->m_pendingBattleground = 0;
		m_mainLock.Release();
		return;
	}

	m_pendPlayers[plr->m_bgTeam].erase(plr->GetLowGUID());
	if(m_players[plr->m_bgTeam].find(plr) != m_players[plr->m_bgTeam].end())
	{
		m_mainLock.Release();
		return;
	}

	plr->FullHPMP();
	plr->SetTeam(plr->m_bgTeam);
	plr->SetByte(PLAYER_BYTES_3, 3, plr->GetTeam());
	if( plr->isCasting() )
		plr->GetCurrentSpell()->safe_cancel();
	//do not get stuck in channel anymation
	plr->SetChannelSpellId( 0 );
	plr->SetChannelSpellTargetGUID( 0 );

	if( plr->IsGMInvisible() == false && plr->IsSpectator() == false )
		m_players[plr->m_bgTeam].insert(plr);

	/* remove from any auto queue remove events */
	sEventMgr.RemoveEvents(plr, EVENT_BATTLEGROUND_QUEUE_UPDATE);

	if( !skip_teleport )
	{
		if( plr->IsInWorld() )
			plr->RemoveFromWorld();
	}

	plr->m_pendingBattleground = 0;
	plr->m_bg = this;

	if(!plr->IsPvPFlagged())
		plr->SetPvPFlag();
	if( IS_ARENA( GetType() ) || m_SkipRewards == true ) //wargames can be made between friendly chars
		plr->SetFFAPvPFlag();

	plr->RemoveAurasByInterruptFlag( AURA_INTERRUPT_ON_PVP_ENTER );

	// Reset the score 
	memset(&plr->m_bgScore, 0, sizeof(BGScore));

	// send him the world states - MAP already sends it automatically
//	SendWorldStates(plr);

	// update pvp data
	// can send after timer ?
//	UpdatePvPData();

	// add the player to the group
	if( plr->GetGroup() && !Rated() && m_SkipRewards == false )
	{
		// remove them from their group
		plr->GetGroup()->RemovePlayer( plr->getPlayerInfo(), true );
	}

//	if(!m_countdownStage)
	if(!m_WarmupExpireStamp)
	{
//		m_countdownStage = 1;
//		sEventMgr.AddEvent(this, &CBattleground::EventCountdown, EVENT_BATTLEGROUND_COUNTDOWN, 30000, 0,0);
//		sEventMgr.ModifyEventTimeLeft(this, EVENT_BATTLEGROUND_COUNTDOWN, 10000);
		m_WarmupExpireStamp = getMSTime() + WARMUP_WAITTIME;
		sEventMgr.RemoveEvents( this, EVENT_BATTLEGROUND_COUNTDOWN );
		sEventMgr.AddEvent(this, &CBattleground::EventCountdown, EVENT_BATTLEGROUND_COUNTDOWN, WARMUP_WAITTIME, 1,0);
	}

	sEventMgr.RemoveEvents(this, EVENT_BATTLEGROUND_CLOSE);

	if(!skip_teleport)
	{
		/* This is where we actually teleport the player to the battleground. */
		plr->SafeTeleport(m_mapMgr,GetStartingCoords(plr->m_bgTeam));
		BattlegroundManager.SendBattlefieldStatus(plr, BF_STATUS_PACKET_TEL_PLAYER, m_type, m_id, 0, m_mapMgr->GetMapId(),Rated());   // Elapsed time is the last argument
	}
	else
	{
		/* If we are not ported, call this immediatelly, otherwise its called after teleportation in Player::OnPushToWorld */
		OnAddPlayer( plr );
	}

	//we actually joined the BG. Sent about 3 seconds after BF_STATUS_PACKET_TEL_PLAYER and before SMSG_TRANSFER_PENDING
	uint32 BGType = GetType();
	uint32 InstanceID = GetMapMgr()->GetInstanceID();
	uint32 MapID = GetMapMgr()->GetMapId();
	uint32 Rated_ = Rated();
	BattlegroundManager.SendBattlefieldStatus(plr, BF_STATUS_PACKET_PLAYER_JOINED, BGType, InstanceID, (uint32)UNIXTIME - m_startTime, MapID, Rated_ );

	if( plr->m_bgTeam < 2 && m_ArenaTeamNames[ plr->m_bgTeam ] == NULL && GetType() - BATTLEGROUND_ARENA_2V2 < NUM_ARENA_TEAM_TYPES && plr->m_arenaTeams[ GetType() - BATTLEGROUND_ARENA_2V2 ] )
		m_ArenaTeamNames[ plr->m_bgTeam ] = (char*)plr->m_arenaTeams[ GetType() - BATTLEGROUND_ARENA_2V2 ]->m_name.c_str();

	m_mainLock.Release();
}

CBattleground * CBattlegroundManager::CreateInstance(uint32 Type, uint32 LevelGroup, uint32 MapID)
{
	//overwrite type before anything
	uint32 OriType = Type;
	if( Type == BATTLEGROUND_ALL_BG )
	{
//		uint32 AvailableTypes[ 6 ] = { BATTLEGROUND_ALTERAC_VALLEY, BATTLEGROUND_WARSONG_GULCH, BATTLEGROUND_ARATHI_BASIN, BATTLEGROUND_EYE_OF_THE_STORM, BATTLEGROUND_TWIN_PEAKS, BATTLEGROUND_GILNEAS };
//		Type = AvailableTypes[ RandomUInt() % 6 ];
		uint32 AvailableTypes[ 5 ] = { BATTLEGROUND_WARSONG_GULCH, BATTLEGROUND_ARATHI_BASIN, BATTLEGROUND_EYE_OF_THE_STORM, BATTLEGROUND_TWIN_PEAKS, BATTLEGROUND_GILNEAS };
		Type = AvailableTypes[ RandomUInt() % 5 ];
	}

	CreateBattlegroundFunc cfunc = BGCFuncs[Type];
	MapMgr * mgr = 0;
	CBattleground * bg;
	bool isWeekend = false;
	struct tm tm;
	uint32 iid;
	time_t t;
	int n;

	if(Type == BATTLEGROUND_ARENA_2V2 || Type == BATTLEGROUND_ARENA_3V3 || Type == BATTLEGROUND_ARENA_5V5)
	{
		/* arenas follow a different procedure. */
//		static const uint32 arena_map_ids[5] = { 559, 562, 572, 617, 618 };
//		static const uint32 arena_map_ids[6] = { 559, 562, 572, 617, 618, 980 }; //tol'vir arena migth not be in cataclysm clients. Remove it if you wish
//		static const uint32 arena_map_ids[1] = { 980 };
#ifdef _DEBUG
		static const uint32 arena_map_ids[5] = { 559, 562, 572, 617 };
		uint32 mapid = arena_map_ids[RandomUInt(3)];
#else
		static const uint32 arena_map_ids[5] = { 559, 562, 572, 617, 980 };
		uint32 mapid = arena_map_ids[RandomUInt(4)];
#endif
//		uint32 mapid = arena_map_ids[0];
		uint32 players_per_side;
		if( MapID != 0 ) 
			mapid = MapID;

		mgr = sInstanceMgr.CreateBattlegroundInstance(mapid);
		if(mgr == NULL)
		{
			Log.Error("BattlegroundManager", "Arena CreateInstance() call failed for map %u, type %u, level group %u", mapid, Type, LevelGroup);
			return NULL;      // Shouldn't happen
		}

		switch(Type)
		{
		case BATTLEGROUND_ARENA_2V2:
			players_per_side = 2;
			break;

		case BATTLEGROUND_ARENA_3V3:
			players_per_side = 3;
			break;

		case BATTLEGROUND_ARENA_5V5:
			players_per_side = 5;
			break;
		default:
			players_per_side = 0;
			break;
		}

		iid = ++m_maxBattlegroundId;
		bg = new Arena(mgr, iid, LevelGroup, Type, players_per_side);
		mgr->m_battleground = bg;
		Log.Success("BattlegroundManager", "Created arena battleground type %u for level group %u on map %u.", Type, LevelGroup, mapid);
		sEventMgr.AddEvent(bg, &CBattleground::EventCreate, EVENT_BATTLEGROUND_QUEUE_UPDATE, 1, 1,0);
		m_instanceLock.Acquire();
		m_instances[Type].insert( make_pair(iid, bg) );
		m_instanceLock.Release();
		return bg;
	}

	if(cfunc == NULL)
	{
		Log.Error("BattlegroundManager", "Could not find CreateBattlegroundFunc pointer for type %u level group %u", Type, LevelGroup);
		return NULL;
	}

	t = time(NULL);
#if (defined( WIN32 ) || defined( WIN64 ) )
//	localtime_s(&tm, &t);
	//zack : some luv for vs2k3 compiler
	tm = *localtime(&t);
#else
	localtime_r(&t, &tm);
#endif

	switch (Type)
	{
		case BATTLEGROUND_WARSONG_GULCH: n = 0; break;
		case BATTLEGROUND_ARATHI_BASIN: n = 1; break;
		case BATTLEGROUND_EYE_OF_THE_STORM: n = 2; break;
		case BATTLEGROUND_STRAND_OF_THE_ANCIENT: n = 3; break;
		default: n = 0; break;
	}
	if (((tm.tm_yday / 7) % 4) == n)
	{
		/* Set weekend from thursday night at midnight until tuesday morning */
		isWeekend = tm.tm_wday >= 5 || tm.tm_wday < 2;
	}

	/* Create Map Manager */
	uint32 mapid = BGMapIds[Type];
	if( MapID != 0 ) 
		mapid = MapID;
	mgr = sInstanceMgr.CreateBattlegroundInstance( mapid );
	if(mgr == NULL)
	{
		Log.Error("BattlegroundManager", "CreateInstance() call failed for map %u, type %u, level group %u", BGMapIds[Type], Type, LevelGroup);
		return NULL;      // Shouldn't happen
	}

	/* Call the create function */
	iid = ++m_maxBattlegroundId;
	bg = cfunc(mgr, iid, LevelGroup, Type);
	bg->SetIsWeekend(isWeekend);
	bg->SetIsRandomBG( OriType == BATTLEGROUND_ALL_BG );
	mgr->m_battleground = bg;
	sEventMgr.AddEvent(bg, &CBattleground::EventCreate, EVENT_BATTLEGROUND_QUEUE_UPDATE, 1, 1,0);
	Log.Success("BattlegroundManager", "Created battleground type %u for level group %u.", Type, LevelGroup);

	m_instanceLock.Acquire();
	m_instances[Type].insert( make_pair(iid, bg) );
	m_instanceLock.Release();

	return bg;
}

void CBattlegroundManager::DeleteBattleground(CBattleground * bg)
{
	uint32 i = bg->GetType();
	uint32 j = bg->GetLevelGroup();
	Player * plr;

	m_instanceLock.Acquire();
	m_queueLock.Acquire();
	m_instances[i].erase(bg->GetId());

	/* erase any queued players */
	set<uint32>::iterator itr = m_queuedPlayers[i][j].begin();
	set<uint32>::iterator it2;
	for(; itr != m_queuedPlayers[i][j].end();)
	{
		it2 = itr++;
		plr = objmgr.GetPlayer(*it2);
		if(!plr)
		{
			m_queuedPlayers[i][j].erase(it2);
			continue;
		}

		if(plr && plr->m_bgQueueInstanceId == bg->GetId())
		{
			sChatHandler.SystemMessageToPlr(plr, plr->GetSession()->LocalizedWorldSrv(54), bg->GetId());
			SendBattlefieldStatus(plr, BF_STATUS_PACKET_REMOVE_PLAYER, 0, 0, 0, 0,0);
			plr->m_bgIsQueued = false;
			m_queuedPlayers[i][j].erase(it2);
		}
	}

	m_queueLock.Release();
	m_instanceLock.Release();

	sLog.outDetail("Deleting battleground from queue %u, instance %u", bg->GetType(), bg->GetId());
	delete bg;
	bg = NULL;
}

GameObject * CBattleground::SpawnGameObject(uint32 entry,uint32 MapId , float x, float y, float z, float o, uint32 flags, uint32 faction, float scale)
{
	GameObject *go = m_mapMgr->CreateGameObject(entry);

	go->CreateFromProto(entry, MapId, x, y, z, o);

	go->SetUInt32Value(GAMEOBJECT_FACTION,faction);
	go->_setFaction();
	go->SetFloatValue(OBJECT_FIELD_SCALE_X,scale);
	go->SetUInt32Value(GAMEOBJECT_FLAGS, flags);
	go->SetInstanceID(m_mapMgr->GetInstanceID());
	go->m_battleground = this;

	return go;
}

Creature *CBattleground::SpawnCreature(uint32 entry, float x, float y, float z, float o)
{
	CreatureProto *cp = CreatureProtoStorage.LookupEntry(entry);
	Creature *c = m_mapMgr->CreateCreature(entry);

	c->Load(cp,x, y, z, o);
	c->PushToWorld(m_mapMgr);
	return c;
}

void CBattleground::SendChatMessage(uint32 Type, uint64 Guid, const char * Format, ...)
{
	char msg[500];
	va_list ap;
	va_start(ap, Format);
	vsnprintf(msg, 500, Format, ap);
	va_end(ap);
	WorldPacket * data = sChatHandler.FillMessageData(Type, 0, msg, Guid, 0);
	DistributePacketToAll(data);
	delete data;
	data = NULL;
}

void CBattleground::DistributePacketToAll(WorldPacket * packet)
{
	m_mainLock.Acquire();
	InRangePlayerSet::iterator itr2;
	for(int i = 0; i < 2; ++i)
     for(InRangePlayerSet::iterator itr = m_players[i].begin(); itr != m_players[i].end();)
	 {
		 itr2 = itr;
		 itr++;
#if defined(WIN32) && defined(HACKY_CRASH_FIXES)
		 try
		{
#endif
			if( (*itr2) && (*itr2)->GetSession() )
			  (*itr2)->GetSession()->SendPacket(packet);
			//player without session ? Not in a BG
			else 
				m_players[i].erase( itr2 );
#if defined(WIN32) && defined(HACKY_CRASH_FIXES)
		}
		 catch(int error )
		 {
			 //!!!! we are exiting loop, seems like this player list is messed up
			 m_players[i].erase( itr );
			 break;
		 }
#endif
	 }
	m_mainLock.Release();
}

void CBattleground::DistributePacketToTeam(WorldPacket * packet, uint32 Team)
{
	m_mainLock.Acquire();
	for(InRangePlayerSet::iterator itr = m_players[Team].begin(); itr != m_players[Team].end(); ++itr)
		if( (*itr) && (*itr)->GetSession() )
		  (*itr)->GetSession()->SendPacket(packet);
	m_mainLock.Release();
}

void CBattleground::PlaySoundToAll(uint32 Sound)
{
	WorldPacket data(SMSG_PLAY_SOUND, 4);
	data << Sound;
	DistributePacketToAll(&data);
}

void CBattleground::PlaySoundToTeam(uint32 Team, uint32 Sound)
{
	WorldPacket data(SMSG_PLAY_SOUND, 4);
	data << Sound;
	DistributePacketToTeam(&data, Team);
}

void CBattlegroundManager::SendBattlefieldStatus(Player * plr, BFStatus Status, uint32 Type, uint32 InstanceID, uint32 Time, uint32 MapId, uint8 RatedMatch)
{
	//waiting in queue
	if( Status == BF_STATUS_PACKET_QUEUE_PLAYER )
	{
/*
		14333
		{SERVER} Packet: (0x8707) SMSG_BATTLEFIELD_STATUS_UPDATE_QUEUE_TIME PacketSize = 24 TimeStamp = 15728286
		84 A0 1E 55 00 
		03 00 - bg type ?
		2E 1E 01 00 00 00 00 00 00 00 00 00 11 00 00 00 00 
		*/
		sStackWorldPacket( data, SMSG_BATTLEFIELD_STATUS_UPDATE_QUEUE_TIME, 24 + 10 );
		data << uint32( 0x551EA084 ); //was same for multiple chars
		data << uint8( 0 );			//
		data << uint16( ObfuscateByte( Type ) );		
		data << uint32( 0x0002D1F4 ); //seems to decrease over time slowly. Might be elapsed time
		data << uint32( 0 ); 
		data << uint32( 0 );		//elapsed time
		data << uint32( 0x11 );
		data << uint8( 0 );
		plr->GetSession()->SendPacket(&data);
	}
	//leave queue or battlefield
	else if( Status == BF_STATUS_PACKET_REMOVE_PLAYER )
	{
/*
		14333
		{SERVER} Packet: (0x0B07) SMSG_BATTLEFIELD_STATUS_LEAVE PacketSize = 4 TimeStamp = 15732045
		00 00 00 00 
		*/
		if( plr->GetSession() )
		{
			sStackWorldPacket( data, SMSG_BATTLEFIELD_STATUS_LEAVE, 4+10 );
			data << uint32( 0 ); 
			plr->GetSession()->SendPacket(&data);
		}
	}
	else if( Status == BF_STATUS_PACKET_ALLOW_ENTER_PLAYER )
	{
		/*
		14333
		{SERVER} Packet: (0x4F63) SMSG_BATTLEFIELD_STATUS3 PacketSize = 24 TimeStamp = 50501985
		61 00 
		55 90 5F 01 instance id ?
		00 
		03 type ?
		1E 00 ?
		E9 01 00 00 map
		00 00 00 00 
		00 00 00 00 
		00 ?
		11 ?
{SERVER} Packet: (0x4F63) SMSG_BATTLEFIELD_ALLOW_PORT PacketSize = 24 TimeStamp = 2012943
61 00 55 90 
5F 01 
00 
03 
1E 00 
E9 01 00 00 
00 00 00 00 
00 00 00 00 00 11 
		*/
		sStackWorldPacket( data, SMSG_BATTLEFIELD_ALLOW_PORT, 24 + 10 );
		data << uint32( 0x90550061 ); //?
		data << uint16( 0x015F );
		data << uint8( 0 );
		data << uint8( ObfuscateByte( Type ) );
		data << uint16( 0x001E ); //?
		data << uint32( MapId );
		data << uint32( 0 );
		data << uint32( 0 );
		data << uint8( 0 );
		data << uint8( 0x11 );
		plr->GetSession()->SendPacket(&data);
	}
	else if( Status == BF_STATUS_PACKET_TEL_PLAYER )
	{
		/*
		14333
		{SERVER} Packet: (0x0303) SMSG_BATTLEFIELD_STATUS_TEL_PLAYER PacketSize = 28 TimeStamp = 50503514
		4C 00 
		00 00 00 00 
		00 
		0A 
		90 5F 01 00 
		11 magic ?
		55 maxlevel
		03 obfuscated type ?
		00 
		02 type ?
		E9 01 00 00 map
		0A 05 00 00 elapsed time ?
		00 
		00 
		1E magic ?
		*/
		sStackWorldPacket( data, SMSG_BATTLEFIELD_STATUS_TEL_PLAYER, 28 + 10 );
		data << uint16( 0x004C );
		data << uint32( 0 );
		data << uint8( 0 );
		data << uint8( 0x0A );
		data << uint32( 90000 );
		data << uint8( 0x11 );
		data << uint8( 85 );	//maxlevel ?
		data << uint8( ObfuscateByte( Type ) );
		data << uint8( 0 );
		data << uint8( 0x02 );
		data << uint32( MapId );
		data << uint32( Time );
		data << uint8( 0 );
		data << uint8( 0 );
		data << uint8( 0x1E );
		plr->GetSession()->SendPacket(&data);
	}
	//shows an icon on minimap edge of ongoing fight type. At the end depending on the type we will see different score screen also
	//this is needed for gladius. It will show players from enemy team similar to our group
	else if( Status == BF_STATUS_PACKET_PLAYER_JOINED )
	{
		/*
		14333
		{SERVER} Packet: (0x4B05) SMSG_BATTLEFIELD_STATUS_PLAYER_JOINED PacketSize = 28 TimeStamp = 50503919
		0A 80 mask of streamed bytes + statuses ( Bit_1 Bit_3, Bit_7 )
		00 00 00 00 uint32
		55 uint8
		11 because mask2 bit 7 on pos 54
		1E because mask1 bit 3 on pos 52
		70 17 00 00 uint32 6000 time ?
		00 00 00 00 uint32
		03 obfuscated type ?
		00 uint8
		E9 01 00 00 map uint32
		00 00 00 00 uint32
		00 uint8
		{SERVER} Packet: (0x4B05) SMSG_BATTLEFIELD_STATUS_PLAYER_JOINED PacketSize = 28 TimeStamp = 5405746
		0B 80 mask of streamed bytes + statuses ( Bit_0 Bit_1 Bit_3, Bit_7 )
		00 00 00 00 
		55 
		11 
		1E 
		A0 0F 00 00
		00 00 00 00 
		03 
		00 
		E9 01 00 00 
		10 8C 04 00 - 298000 - 5 minutes
		00 
		{SERVER} Packet: (0x4B05) SMSG_BATTLEFIELD_STATUS_PLAYER_JOINED PacketSize = 28 TimeStamp = 5695643
		0B 80 
		00 00 00 00 
		55 
		11 
		1E 
		40 1F 00 00 
		00 00 00 00 
		02 
		00 
		11 02 00 00 arathi
		00 00 00 00 
		00 
		- after teleport
		{SERVER} Packet: (0x4B05) SMSG_BATTLEFIELD_STATUS_PLAYER_JOINED PacketSize = 30 TimeStamp = 14931822
		AE 80 
		00 00 00 00 
		55 
		11 
		1E 
		58 1B 00 00 
		00 00 00 00 
		07 
		00 
		03 
		6A 02 00 00 
		10 
		00 00 00 00 
		00 
		- after died
		{SERVER} Packet: (0x4B05) SMSG_BATTLEFIELD_STATUS_PLAYER_JOINED PacketSize = 30 TimeStamp = 15021741
		AE 80 00 00 00 00 55 11 1E E8 7A 01 00 00 00 00 00 07 00 03 
		6A 02 00 00 
		10 
		C0 D4 01 00 - time until close
		00 
		*/
		WorldPacket data( SMSG_BATTLEFIELD_STATUS_PLAYER_JOINED, 28 + 10 );
		//mask, if you touch it you break the rest of the packet
		uint8 FirstByte = 0;
		if( IS_ARENA( Type ) )
			FirstByte |= 0xA4;	//extra 2 mask bits = extra 2 bytes
		if( plr->GetTeam() == 0 )
			FirstByte |= 0x0A; //Horde 
		else
			FirstByte |= 0x0B; //bit 0 signals team type ? Aliance ? Same as for PVP_LOG_DATA
		data << uint8( FirstByte );
		data << uint8( 0x80 ); 
		data << uint32( 0 );
		data << uint8( 85 );	//maxlevel 85 ? 0x55 ?
		data << uint8( 0x11 );	//this is something obfuscated
		data << uint8( 0x1E );	//this is something obfuscated
		data << uint32( Time ); //milliseconds since started(as created started)
		data << uint32( 0 );	//?
//			data << uint8( ObfuscateByte( Type ) );	//maybe queue position ?
		data << uint8( 7 );	//maybe queue position ? seems to be semi constant : arathi 2, wsg 3, arena 7
		data << uint8( 0 );		//?
		if( IS_ARENA( Type ) )
			data << uint8( Type );
		data << uint32( MapId );
		if( IS_ARENA( Type ) )
			data << uint8( 2 );	//unk, seen c0 also
		if( plr->m_bg && plr->m_bg->HasEnded() )
			data << uint32( 120000 );	//timer until PVP icon near minimap is removed, Given in MS
		else
			data << uint32( 0 );	//timer until PVP icon near minimap is removed, Given in MS
		data << uint8( RatedMatch );
		if( plr->GetSession() )
			plr->GetSession()->SendPacket(&data); 
//		if( plr->m_bg )
//			plr->m_bg->DistributePacketToAll(&data);
	}
/*	else
	{
		WorldPacket data(SMSG_BATTLEFIELD_STATUS, 30);
		if(Type >= BATTLEGROUND_ARENA_2V2 && Type <= BATTLEGROUND_ARENA_5V5)
		{
			data << uint32(0);		// Queue Slot 0..2. Only the first slot is used in arcemu!
			switch(Type)
			{
			case BATTLEGROUND_ARENA_2V2:
				data << uint8(2);
				break;

			case BATTLEGROUND_ARENA_3V3:
				data << uint8(3);
				break;

			case BATTLEGROUND_ARENA_5V5:
				data << uint8(5);
				break;
			}
			data << uint8(0x0E);
			data << uint32(0x00000006);		// 6 for arena and 7 for bg = bgtype
			data << uint32(0x54501F90);		// mapinfo, action, unk
			data << uint32(0);	
			data << uint8(RatedMatch);      // 1 = rated match
		}
		else
		{
			data << uint32(0);				//arena type
			data << uint8(0);				//unk
			if( Type == BATTLEGROUND_ALL_BG )
				data << uint8(0);				//unk 3.3.3
			else
				data << uint8(7);				//bg type
			data << Type;
			data << uint32(0x50501F90);		//3.3.2 
			data << InstanceID;
			data << uint8(0);				//rated ?
		}
		data << (uint32)Status;
		plr->GetSession()->SendPacket(&data);
	} */
}

void CBattleground::RemovePlayer(Player * plr, bool logout)
{
	WorldPacket data(SMSG_BATTLEGROUND_PLAYER_LEFT, 30);
	uint64 guid = plr->GetGUID();
	uint8 *guid_bytes = (uint8*)&guid;
	data << uint8( 0xB7 );
    data << ObfuscateByte( guid_bytes[3] );   
    data << ObfuscateByte( guid_bytes[2] );   
    data << ObfuscateByte( guid_bytes[7] );   
    data << ObfuscateByte( guid_bytes[1] );   
    data << ObfuscateByte( guid_bytes[0] );   
    data << ObfuscateByte( guid_bytes[6] ); 
//	if ( plr->IsGMInvisible() == false )
	{
		//Dont show invisble gm's leaving the game.
		DistributePacketToAll(&data);
	}
//	else
	{
//		RemoveInvisGM();
	}
	m_mainLock.Acquire();
	m_players[plr->m_bgTeam].erase(plr);

	memset(&plr->m_bgScore, 0, sizeof(BGScore));
	OnRemovePlayer(plr);
	plr->m_bg = NULL;

	/* are we in the group? */
	if( plr->GetGroup() == m_groups[plr->m_bgTeam] )
		plr->GetGroup()->RemovePlayer( plr->getPlayerInfo() );

	// reset team
	plr->ResetTeam();

	/* revive the player if he is dead */
	if(!plr->isAlive())
	{
		plr->SetUInt32Value(UNIT_FIELD_HEALTH, plr->GetUInt32Value(UNIT_FIELD_MAXHEALTH));
		plr->ResurrectPlayer();
	}

	/* remove buffs */
	plr->RemoveAura(32727,0,AURA_SEARCH_POSITIVE); // Arena preparation
	plr->RemoveAura(44521,0,AURA_SEARCH_POSITIVE); // BG preparation
	plr->RemoveAura(44535,0,AURA_SEARCH_POSITIVE);
	plr->RemoveAura(21074,0,AURA_SEARCH_POSITIVE);

	/* teleport out */
	if(!logout)
	{
		if(!m_ended && plr->GetSession() && !plr->GetSession()->HasGMPermissions() && m_SkipRewards == false && plr->IsSpectator() == false )
		{
			plr->CastSpell(plr, BG_DESERTER, true);
			objmgr.PlayerAddDeserterCooldown( plr->GetGUID() );
		}

		if(!IS_INSTANCE(plr->m_bgEntryPointMap))
		{
			LocationVector vec(plr->m_bgEntryPointX, plr->m_bgEntryPointY, plr->m_bgEntryPointZ, plr->m_bgEntryPointO);
			plr->SafeTeleportDelayed(plr->m_bgEntryPointMap, plr->m_bgEntryPointInstance, vec);
		}
		else
		{
			LocationVector vec(plr->GetBindPositionX(), plr->GetBindPositionY(), plr->GetBindPositionZ());
			plr->SafeTeleportDelayed(plr->GetBindMapId(), 0, vec);
		}

		BattlegroundManager.SendBattlefieldStatus(plr, BF_STATUS_PACKET_REMOVE_PLAYER, 0, 0, 0, 0,0);

		// send some null world states 
/*		data.Initialize(SMSG_INIT_WORLD_STATES);
		data << uint32(plr->GetMapId()) << uint32(0) << uint32(0) << uint16( 0 );
		plr->GetSession()->SendPacket(&data); */
	}

	if(/*!m_ended && */m_players[0].empty() == true && m_players[1].empty() == true)
	{
		/* create an inactive event */
		sEventMgr.RemoveEvents(this, EVENT_BATTLEGROUND_CLOSE);                  // 10mins
		//sEventMgr.AddEvent(this, &CBattleground::Close, EVENT_BATTLEGROUND_CLOSE, DEFAULT_BG_CLOSE_TIME, 1,0); //this is BS..appears to be		the cause if the battleground crashes.
		this->Close();
	}

	plr->m_bgTeam=plr->GetTeam();
	plr->SetByte(PLAYER_BYTES_3, 3, 0);

	plr->FullHPMP(); //make sure to have this after removed negative auras and we are alive

	if( IS_ARENA( GetType() ) || m_SkipRewards == true  )
		plr->RemoveFFAPvPFlag();
	m_mainLock.Release();
}

void CBattleground::SendPVPData(Player * plr)
{
	m_mainLock.Acquire();

	WorldPacket data(10*(m_players[0].size()+m_players[1].size())+50);
	BuildPvPUpdateDataPacket(&data);
	plr->GetSession()->SendPacket(&data);

	m_mainLock.Release();
}

void CBattleground::EventCreate()
{
	OnCreate();
}

int32 CBattleground::event_GetInstanceID()
{
	return m_mapMgr->GetInstanceID();
}

void CBattleground::EventCountdown()
{
/*	if(m_countdownStage == 1)
	{
		m_countdownStage = 2;

		m_mainLock.Acquire();
		for(int i = 0; i < 2; ++i)
		{
			 for(InRangePlayerSet::iterator itr = m_players[i].begin(); itr != m_players[i].end(); ++itr)
				 if( (*itr) && (*itr)->GetSession() )
				 {
					(*itr)->GetSession()->SystemMessage((*itr)->GetSession()->LocalizedWorldSrv(46),(*itr)->GetSession()->LocalizedWorldSrv(GetNameID()));
				 }
		}
		m_mainLock.Release();

		// SendChatMessage( CHAT_MSG_BG_EVENT_NEUTRAL, 0, "One minute until the battle for %s begins!", GetName() );
	}
	else if(m_countdownStage == 2)
	{
		m_countdownStage = 3;

		m_mainLock.Acquire();
		for(int i = 0; i < 2; ++i)
		{
			 for(InRangePlayerSet::iterator itr = m_players[i].begin(); itr != m_players[i].end(); ++itr)
				 if( (*itr) && (*itr)->GetSession() ){
						(*itr)->GetSession()->SystemMessage((*itr)->GetSession()->LocalizedWorldSrv(47),(*itr)->GetSession()->LocalizedWorldSrv(GetNameID()));
					}
		}
		m_mainLock.Release();

		//SendChatMessage( CHAT_MSG_BG_EVENT_NEUTRAL, 0, "Thirty seconds until the battle for %s begins!", GetName() );
	}
	else if(m_countdownStage == 3)
	{
		m_countdownStage = 4;

		m_mainLock.Acquire();
		for(int i = 0; i < 2; ++i)
		{
			 for(InRangePlayerSet::iterator itr = m_players[i].begin(); itr != m_players[i].end(); ++itr)
				 if( (*itr) && (*itr)->GetSession() ){
						(*itr)->GetSession()->SystemMessage((*itr)->GetSession()->LocalizedWorldSrv(48),(*itr)->GetSession()->LocalizedWorldSrv(GetNameID()));
					}
		}
		m_mainLock.Release();

		//SendChatMessage( CHAT_MSG_BG_EVENT_NEUTRAL, 0, "Fifteen seconds until the battle for %s begins!", GetName() );
		sEventMgr.ModifyEventRepeatInterval(this, EVENT_BATTLEGROUND_COUNTDOWN, 150);
		sEventMgr.ModifyEventTimeLeft(this, EVENT_BATTLEGROUND_COUNTDOWN, 15000);
	}
	else
	{
		m_mainLock.Acquire();
		for(int i = 0; i < 2; ++i)
		{
			 for(InRangePlayerSet::iterator itr = m_players[i].begin(); itr != m_players[i].end(); ++itr)
				 if( (*itr) && (*itr)->GetSession() ){
						(*itr)->GetSession()->SystemMessage((*itr)->GetSession()->LocalizedWorldSrv(49),(*itr)->GetSession()->LocalizedWorldSrv(GetNameID()));
					}
		}
		m_mainLock.Release();
		//SendChatMessage( CHAT_MSG_BG_EVENT_NEUTRAL, 0, "The battle for %s has begun!", GetName() );
		sEventMgr.RemoveEvents(this, EVENT_BATTLEGROUND_COUNTDOWN);
		Start();
	}*/
	Start();
}

void CBattleground::Start()
{
	//update visibility of players so they can see other team players. This was required as you can see other arena team before match starts
	{
		for( PlayerStorageMap::iterator itr = GetMapMgr()->m_PlayerStorage.begin(); itr != GetMapMgr()->m_PlayerStorage.end(); itr++)
			itr->second->UpdateVisibility();
	}

	OnStart();
}

void CBattleground::Close()
{
	/* remove all players from the battleground */
	m_mainLock.Acquire();
	m_ended = true;

	if( m_groups[0] )
		m_groups[0]->ConvertToNormalFromBG();
	if( m_groups[1] )
		m_groups[1]->ConvertToNormalFromBG();

	for(uint32 i = 0; i < 2; ++i)
	{
		InRangePlayerSet::iterator itr;
		set<uint32>::iterator it2;
/*		{
			for(itr = m_players[i].begin(); itr != m_players[i].end();)
			{
				Player * plr = *itr;
				++itr;
				RemovePlayer(plr, false);
			}
		}/**/
		//all players including GMs and spectators or any other script added players
		{
			MapMgr *mgr = GetMapMgr();
			PlayerStorageMap::iterator itr, itr2;
			for( itr = mgr->m_PlayerStorage.begin(); itr != mgr->m_PlayerStorage.end(); )
			{
				Player *plr = itr->second;
				itr++;
				if( plr->m_bg == this )
					RemovePlayer(plr, false);

				//convert back groups to their normal form
				if( plr->GetGroup() && plr->GetGroup() != m_groups[0] && plr->GetGroup() != m_groups[1] )
					plr->GetGroup()->ShrinkToParty( );
			}
		}
		for(it2 = m_pendPlayers[i].begin(); it2 != m_pendPlayers[i].end();)
		{
			uint32 guid = *it2;
			++it2;
			Player * plr = objmgr.GetPlayer(guid);

			if(plr)
				RemovePendingPlayer(plr);
			else
				m_pendPlayers[i].erase(guid);
		}
	}

	/* call the virtual onclose for cleanup etc */
	OnClose();

	/* shut down the map thread. this will delete the battleground from the corrent context. */
	m_mapMgr->SetThreadState(THREADSTATE_TERMINATE);

	m_mainLock.Release();
}

Creature * CBattleground::SpawnSpiritGuide(float x, float y, float z, float o, uint32 horde)
{
	if(horde > 1)
		horde = 1;

	CreatureInfo * pInfo = CreatureNameStorage.LookupEntry(13116 + horde);
	if(pInfo == 0)
	{
		return NULL;
	}

	Creature * pCreature = m_mapMgr->CreateCreature(pInfo->Id);

	pCreature->Create(pInfo->Name, m_mapMgr->GetMapId(), x, y, z, o);

	pCreature->SetInstanceID(m_mapMgr->GetInstanceID());
	pCreature->SetUInt32Value(OBJECT_FIELD_ENTRY, 13116 + horde);
	pCreature->SetFloatValue(OBJECT_FIELD_SCALE_X, 1.0f);

	pCreature->SetUInt32Value(UNIT_FIELD_HEALTH, 100000);
	pCreature->SetPower(POWER_TYPE_MANA, 4868);
	pCreature->SetPower(POWER_TYPE_FOCUS, 200);
	pCreature->SetPower(POWER_TYPE_HAPPINESS, 2000000);

	pCreature->SetUInt32Value(UNIT_FIELD_MAXHEALTH, 10000);
	pCreature->SetMaxPower( POWER_TYPE_MANA, 4868);
//	pCreature->SetMaxPower( POWER_TYPE_FOCUS, 200);
//	pCreature->SetMaxPower( UNIT_FIELD_MAXPOWER5, 2000000);

	pCreature->SetUInt32Value(UNIT_FIELD_LEVEL, 60);
	pCreature->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, 84 - horde ); //important to have this or res packet is not sent by client
	pCreature->_setFaction();
//	pCreature->SetUInt32Value(UNIT_FIELD_BYTES_0, 0 | (2 << 8) | (1 << 16));
	pCreature->SetUInt32Value(UNIT_FIELD_BYTES_0, 512 );

	pCreature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, 12591); //item was taken from Horde. Maybe Aliance has other ?

	pCreature->SetUInt32Value(UNIT_FIELD_FLAGS, UNIT_FLAG_PLUS_MOB | UNIT_FLAG_NOT_ATTACKABLE_9 | UNIT_FLAG_PASSIVE); // 832
	pCreature->SetUInt32Value(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_ENABLE_POWER_REGEN ); // 2048

	pCreature->SetUInt32Value(UNIT_FIELD_BASEATTACKTIME, 2000);
	pCreature->SetUInt32Value(UNIT_FIELD_BASEATTACKTIME_1, 2000);
	pCreature->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 0.208f);
	pCreature->SetFloatValue(UNIT_FIELD_COMBATREACH, 1.5f);

	pCreature->SetUInt32Value(UNIT_FIELD_DISPLAYID, 13337 + horde*2);
	pCreature->SetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID, 13337 + horde*2);

	pCreature->SetUInt32Value(UNIT_CHANNEL_SPELL, 22011);
	pCreature->SetFloatValue(UNIT_MOD_CAST_SPEED, 1.0f);

	pCreature->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPIRITGUIDE ); // 32768
	pCreature->SetUInt32Value(UNIT_FIELD_BYTES_2, 1 );

	pCreature->DisableAI();
	pCreature->PushToWorld(m_mapMgr);

	pCreature->GetAIInterface()->disable_melee = true;
	pCreature->GetAIInterface()->m_canMove = false;
	pCreature->GetAIInterface()->disable_combat = true;
	pCreature->GetAIInterface()->disable_targeting = true;
	pCreature->bInvincible = true;

	return pCreature;
}

void CBattleground::QueuePlayerForResurrect(Player * plr, Creature * spirit_healer)
{
	//make sure we are not multi queued
	plr->RemovePlayerFromBGResurrect();

	m_mainLock.Acquire();
	map<Creature*,set<uint32> >::iterator itr = m_resurrectMap.find(spirit_healer);
	if(itr != m_resurrectMap.end())
		itr->second.insert(plr->GetLowGUID());
	plr->m_areaSpiritHealer_guid=spirit_healer->GetGUID();
	m_mainLock.Release();
}

void CBattleground::RemovePlayerFromResurrect(Player * plr, Creature * spirit_healer)
{
	m_mainLock.Acquire();
	map<Creature*,set<uint32> >::iterator itr = m_resurrectMap.find(spirit_healer);
	if(itr != m_resurrectMap.end())
		itr->second.erase(plr->GetLowGUID());
	plr->m_areaSpiritHealer_guid=0;
	m_mainLock.Release();
}

void CBattleground::AddSpiritGuide(Creature * pCreature)
{
	m_mainLock.Acquire();
	map<Creature*,set<uint32> >::iterator itr = m_resurrectMap.find(pCreature);
	if(itr == m_resurrectMap.end())
	{
		set<uint32> ti;
		m_resurrectMap.insert(make_pair(pCreature,ti));
	}
	m_mainLock.Release();
}

void CBattleground::RemoveSpiritGuide(Creature * pCreature)
{
	m_mainLock.Acquire();
	m_resurrectMap.erase(pCreature);
	m_mainLock.Release();
}

void CBattleground::EventResurrectPlayers()
{
	m_mainLock.Acquire();
	Player * plr;
	set<uint32>::iterator itr;
	map<Creature*,set<uint32> >::iterator i;
	for(i = m_resurrectMap.begin(); i != m_resurrectMap.end(); ++i)
	{
		for(itr = i->second.begin(); itr != i->second.end(); ++itr)
		{
			plr = m_mapMgr->GetPlayer(*itr);
			if( plr == NULL )
				continue;
			if( m_PlayerResurrects > 0 && (int32)plr->m_bgScore.Deaths > m_PlayerResurrects )
			{
				plr->BroadcastMessage( "You consumed all your resurrects" );
				continue;
			}
			if( m_TeamResurrectsDone > m_TeamResurrects )
			{
				plr->BroadcastMessage( "Team consumed all resurrects" );
				continue;
			}
			m_TeamResurrectsDone++;
			if(plr->IsDead())
			{
				plr->ResurrectPlayer();
				plr->SetUInt32Value(UNIT_FIELD_HEALTH, plr->GetUInt32Value(UNIT_FIELD_MAXHEALTH));
				plr->SetPower(POWER_TYPE_MANA, plr->GetMaxPower(POWER_TYPE_MANA ));
				plr->SetPower(POWER_TYPE_ENERGY, plr->GetMaxPower(POWER_TYPE_ENERGY));
				plr->CastSpell( plr, RESURRECT_SPELL, true );
				plr->CastSpell( plr, BG_REVIVE_PREPARATION, true );
				// custom to make players immune to avoid GY camping
//				plr->CastSpell(plr->GetGUID(),6724,true);					//immunity for us
//				plr->CastSpell( plr->GetGUID(), 42917, true );				//frost nova to avoid campers
/*				SpellEntry * sp = dbcSpell.LookupEntry(6724);
				Spell * s = SpellPool.PooledNew( __FILE__, __LINE__ );
				s->Init(plr, sp, true, 0);
				s->forced_duration = 7000;
				SpellCastTargets targets(plr->GetGUID());
				s->prepare(&targets);/**/
				//pets are always needed, bring them back
				if( plr->getClass() == HUNTER )
				{
//					plr->CastSpell(plr, 883, true );	//call pet
					plr->AutoResummonLastPet();
					plr->CastSpell(plr, 982, true );	//revive pet if we had any
					if( plr->GetSummon() )
						plr->GetSummon()->SetHealth( plr->GetSummon()->GetMaxHealth() );
				}
				//plr->SendAllAurasToPlayer( plr->GetLowGUID() );
//				sEventMgr.AddEvent( SafeUnitCast( plr ),&Unit::SendAllAurasToPlayer,plr->GetLowGUID(),EVENT_SEND_AURAS_TO_PLAYER,AURA_SEND_DELAY,1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT); 
			}
			if( m_SuddenDeath != 0 )
				plr->CastSpell( plr, 10848, true );
		}
		i->second.clear();
	}
	m_lastResurrect = (uint32)UNIXTIME;
	m_mainLock.Release();
}

void CBattlegroundManager::HandleArenaJoin(WorldSession * m_session, uint32 BattlegroundType, uint8 as_group, uint8 rated_match)
{
	uint32 pguid = m_session->GetPlayer()->GetLowGUID();
	uint32 lgroup = GetLevelGrouping(m_session->GetPlayer()->getLevel());
	if(as_group && m_session->GetPlayer()->GetGroup() == NULL)
	{ 
		sLog.outDebug("Arena : Player wanted to join arena as group but he has no group. Exiting");
		return;
	}

	Group * pGroup = m_session->GetPlayer()->GetGroup();
	if(as_group)
	{
		if(pGroup->GetSubGroupCount() != 1)
		{
			m_session->SystemMessage(m_session->LocalizedWorldSrv(55) );
			sLog.outDebug("Arena : Player group is raid. Exiting");
			return;
		}
		if(pGroup->GetLeader() != m_session->GetPlayer()->getPlayerInfo())
		{
			m_session->SystemMessage(m_session->LocalizedWorldSrv(56) );
			sLog.outDebug("Arena : Player in not group leader. Exiting");
			return;
		}

		GroupMembersSet::iterator itx;
		if(!rated_match)
		{
			/* add all players normally.. bleh ;P */
			pGroup->Lock();
			for(itx = pGroup->GetSubGroup(0)->GetGroupMembersBegin(); itx != pGroup->GetSubGroup(0)->GetGroupMembersEnd(); ++itx)
			{
				if((*itx)->m_loggedInPlayer && !(*itx)->m_loggedInPlayer->m_bgIsQueued && !(*itx)->m_loggedInPlayer->m_bg)
					HandleArenaJoin((*itx)->m_loggedInPlayer->GetSession(), BattlegroundType, 0, 0);
			}
			pGroup->Unlock();
			return;
		}
		else
		{
			/* make sure all players are 70 */
			uint32 maxplayers;
			uint32 type=BattlegroundType-BATTLEGROUND_ARENA_2V2;
			switch(BattlegroundType)
			{
			case BATTLEGROUND_ARENA_3V3:
				maxplayers=3;
				break;

			case BATTLEGROUND_ARENA_5V5:
				maxplayers=5;
				break;

			case BATTLEGROUND_ARENA_2V2:
			default:
				maxplayers=2;
				break;
			}

			if(pGroup->GetLeader()->m_loggedInPlayer && pGroup->GetLeader()->m_loggedInPlayer->m_arenaTeams[type] == NULL)
			{
				m_session->SystemMessage( m_session->LocalizedWorldSrv(57));
				return;
			}

			pGroup->Lock();
			for(itx = pGroup->GetSubGroup(0)->GetGroupMembersBegin(); itx != pGroup->GetSubGroup(0)->GetGroupMembersEnd(); ++itx)
			{
				if(maxplayers==0)
				{
//					m_session->SystemMessage(m_session->LocalizedWorldSrv(58));
					m_session->SystemMessage( "You have too many players in your party to join this type of arena." );
					pGroup->Unlock();
					return;
				}

				if( (*itx)->lastLevel < PLAYER_ARENA_MIN_LEVEL )
				{
//					m_session->SystemMessage(m_session->LocalizedWorldSrv(59));
					m_session->SystemMessage( "Sorry, some of your party members are not level 70." );
					pGroup->Unlock();
					return;
				}

				if((*itx)->m_loggedInPlayer)
				{
					if((*itx)->m_loggedInPlayer->m_bg || (*itx)->m_loggedInPlayer->m_bg || (*itx)->m_loggedInPlayer->m_bgIsQueued)
					{
//						m_session->SystemMessage(m_session->LocalizedWorldSrv(60));
						m_session->SystemMessage( "One or more of your party members are already queued or inside a battleground." );
						pGroup->Unlock();
						return;
					};
					if((*itx)->m_loggedInPlayer->m_arenaTeams[type] != pGroup->GetLeader()->m_loggedInPlayer->m_arenaTeams[type])
					{
//						m_session->SystemMessage(m_session->LocalizedWorldSrv(61));
						m_session->SystemMessage( "One or more of your party members are not members of your team." );
						pGroup->Unlock();
						return;
					}

					--maxplayers;
				}
			}
//			WorldPacket data(SMSG_GROUP_JOINED_BATTLEGROUND, 18);
//			{SERVER} Packet: (0xDB01) SMSG_GROUP_JOINED_BATTLEGROUND PacketSize = 18 TimeStamp = 2832354
//			data.CopyFromSnifferDump( "4D A6 06 00 00 00 00 02 95 39 DD 72 8E 12 FF FF FF FF" );

			//advertise it to other players
			objmgr.OnArenaTeamJoinedQueue( pGroup->GetLeader()->m_loggedInPlayer->m_arenaTeams[ type ], pGroup );

			for(itx = pGroup->GetSubGroup(0)->GetGroupMembersBegin(); itx != pGroup->GetSubGroup(0)->GetGroupMembersEnd(); ++itx)
			{
				if((*itx)->m_loggedInPlayer)
				{
					SendBattlefieldStatus((*itx)->m_loggedInPlayer, BF_STATUS_PACKET_QUEUE_PLAYER, BattlegroundType, 0 , 0, 0,1);
					(*itx)->m_loggedInPlayer->m_bgIsQueued = true;
					(*itx)->m_loggedInPlayer->m_bgQueueInstanceId = 0;
					(*itx)->m_loggedInPlayer->m_bgQueueType = BattlegroundType;
//					(*itx)->m_loggedInPlayer->GetSession()->SendPacket(&data);
					(*itx)->m_loggedInPlayer->m_bgEntryPointX=(*itx)->m_loggedInPlayer->GetPositionX();
					(*itx)->m_loggedInPlayer->m_bgEntryPointY=(*itx)->m_loggedInPlayer->GetPositionY();
					(*itx)->m_loggedInPlayer->m_bgEntryPointZ=(*itx)->m_loggedInPlayer->GetPositionZ();
					(*itx)->m_loggedInPlayer->m_bgEntryPointMap=(*itx)->m_loggedInPlayer->GetMapId();
				}
			}

			pGroup->Unlock();

			m_queueLock.Acquire();
			m_queuedGroups[BattlegroundType].insert(pGroup->GetID());
			m_queueLock.Release();
			Log.Success("BattlegroundMgr", "Group %u is now in battleground queue for arena type %u", pGroup->GetID(), BattlegroundType);

			/* send the battleground status packet */

			return;
		}
	}


	/* Queue him! */
	m_queueLock.Acquire();
	m_queuedPlayers[BattlegroundType][lgroup].insert(pguid);
	Log.Success("BattlegroundMgr", "Player %u is now in battleground queue for {Arena %u}", m_session->GetPlayer()->GetLowGUID(), BattlegroundType );

	/* send the battleground status packet */
	SendBattlefieldStatus(m_session->GetPlayer(), BF_STATUS_PACKET_QUEUE_PLAYER, BattlegroundType, 0 , 0, 0,0);
	m_session->GetPlayer()->m_bgIsQueued = true;
	m_session->GetPlayer()->m_bgQueueInstanceId = 0;
	m_session->GetPlayer()->m_bgQueueType = BattlegroundType;

	/* Set battleground entry point */
	m_session->GetPlayer()->m_bgEntryPointX = m_session->GetPlayer()->GetPositionX();
	m_session->GetPlayer()->m_bgEntryPointY = m_session->GetPlayer()->GetPositionY();
	m_session->GetPlayer()->m_bgEntryPointZ = m_session->GetPlayer()->GetPositionZ();
	m_session->GetPlayer()->m_bgEntryPointMap = m_session->GetPlayer()->GetMapId();
	m_session->GetPlayer()->m_bgEntryPointInstance = m_session->GetPlayer()->GetInstanceID();

	m_queueLock.Release();
}

bool CBattleground::CanPlayerJoin(Player * plr, uint32 type)
{
	return HasFreeSlots(plr->m_bgTeam,type)&&(GetLevelGrouping(plr->getLevel())==GetLevelGroup())&&(!plr->HasAura(BG_DESERTER));
}

void CBattleground::QueueAtNearestSpiritGuide(Player *plr, Creature *old)
{
	float dd;
	float dist = 999999.0f;
	Creature *cl = NULL;
	set<uint32> *closest = NULL;
	//make sure to not multi queue
	plr->RemovePlayerFromBGResurrect();
	m_lock.Acquire();
	map<Creature*, set<uint32> >::iterator itr = m_resurrectMap.begin();
	for(; itr != m_resurrectMap.end(); ++itr)
	{
		if( itr->first == old )
			continue;

		dd = plr->GetDistance2dSq(itr->first) < dist;
		if( dd < dist )
		{
			cl = itr->first;
			closest = &itr->second;
			dist = dd;
		}
	}

	if( closest != NULL )
	{
		//this spell removes us from queues in case we were in one
//		plr->CastSpell(plr,2584,true); //waiting to be resurrected

		closest->insert(plr->GetLowGUID());
		plr->m_areaSpiritHealer_guid=cl->GetGUID();
	}

	m_lock.Release();
}

uint32 CBattleground::GetFreeSlots(uint32 t, uint32 type)
{
	uint32 maxPlayers = BattlegroundManager.GetMaximumPlayers(type);

	m_mainLock.Acquire();
	size_t s = maxPlayers - m_players[t].size() - m_pendPlayers[t].size();
	m_mainLock.Release();
	return (uint32)s;
}

bool CBattleground::HasFreeSlots(uint32 Team, uint32 type)
{
	if( m_DenyNewJoins == true )
		return false;

	bool res;
	uint32 maxPlayers = BattlegroundManager.GetMaximumPlayers(type);

	m_mainLock.Acquire();
	if (type >= BATTLEGROUND_ARENA_2V2 && type <= BATTLEGROUND_ARENA_5V5)
	{
		res = (((uint32)m_players[Team].size() + m_pendPlayers[Team].size() ) < maxPlayers);
	}
	else
	{
		uint32 size[2];
		size[0] = uint32(m_players[0].size() + m_pendPlayers[0].size());
		size[1] = uint32(m_players[1].size() + m_pendPlayers[1].size());
		//if we have free slots and our team has less players then the other team
		res = (size[Team] < maxPlayers) && (((int)size[Team] - (int)size[1-Team]) <= 0 );
	}
	m_mainLock.Release();
	return res;
}

uint32 CBattleground::GetBalanceFreeSlots(uint32 Team, uint32 type)
{
	uint32 res;
	uint32 maxPlayers = BattlegroundManager.GetMaximumPlayers(type);

	m_mainLock.Acquire();
	if (type >= BATTLEGROUND_ARENA_2V2 && type <= BATTLEGROUND_ARENA_5V5)
	{
		res = maxPlayers - ((uint32)m_players[Team].size() + (uint32)m_pendPlayers[Team].size());
	}
	else
	{
		uint32 size[2];
		size[0] = uint32(m_players[0].size() + m_pendPlayers[0].size());
		size[1] = uint32(m_players[1].size() + m_pendPlayers[1].size());
		//if we have free slots and our team has less players then the other team
		if( size[Team] < maxPlayers && size[Team] <= size[1-Team] )
			res = MAX( 1, size[Team] - size[1-Team] );
		else
			res = 0;
	}
	m_mainLock.Release();
	return res;
}

void CBattleground::EventOnEndedPreCleanup()
{
	sHookInterface.OnBGEnded( this );

	if( m_ended == false || ( m_SkipRewards == true && m_ScriptCPReward == 0 ) )
		return;

	m_mainLock.Acquire();
	for(uint32 i = 0; i < 2; ++i)
	{
		InRangePlayerSet::iterator itr;
		set<uint32>::iterator it2;
		Player * plr;
		for(itr = m_players[i].begin(); itr != m_players[i].end();)
		{
			plr = *itr;
			++itr;
			if( m_isRandomBG && i == m_winningteam )
			{
				plr->ModCurrencyCount( CURRENCY_CONQUEST_POINT, float2int32( m_ScriptCPReward + 35 * sWorld.getRate(RATE_ARENAPOINTMULTIPLIER2X) ) ); //should be 50 for BG. Let's see how much they will exploit it
				plr->ModCurrencyCount( CURRENCY_HONOR_POINT, float2int32( 270 * sWorld.getRate(RATE_HONOR) ) );
			}
			else
				plr->ModCurrencyCount( CURRENCY_HONOR_POINT, float2int32( 45 * sWorld.getRate(RATE_HONOR) ) ); 
		}
	}
	m_mainLock.Release();
}

void CBattleground::AddGroupToTeam( Group *group, int8 Team, int32 RemainingAdds )
{
	if( Team != 0 && Team != 1 )
		return;
	//keep arena or BG teams after match ends
//	m_groups[ Team ]->DisbandOnBGEndDisable();

	if( IS_ARENA( GetType() ) == false )
		group->ExpandToRaid( true );

	uint32 count = group->GetSubGroupCount();
	// Loop through each raid group.
	for( uint8 k = 0; k < count; k++ )
	{
		SubGroup * subgroup = group->GetSubGroup( k );
		if( subgroup )
		{
			group->Lock();
			for( GroupMembersSet::iterator itr = subgroup->GetGroupMembersBegin(); itr != subgroup->GetGroupMembersEnd(); ++itr )
			{
				if( (*itr)->m_loggedInPlayer && (*itr)->m_loggedInPlayer->m_bg == NULL && (*itr)->m_loggedInPlayer->m_bgIsQueued == false )
				{
					AddPlayer( (*itr)->m_loggedInPlayer, Team, true );
					RemainingAdds--;
					if( RemainingAdds == 0 )
						break;
				}
			}
			group->Unlock();
		}
	}
}
