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

void WorldSession::HandleBattlefieldJoinLeaveSelect(WorldPacket &recv_data)
{
	uint8 mapinfo[3];
	uint8 action;
	uint8 bgtype;
/*
14333
{CLIENT} Packet: (0x88C8) CMSG_BATTLEFIELD_LEAVE_JOIN_REPLY PacketSize = 5 TimeStamp = 15715306
70 00 1E 11 ?
03 - type
{CLIENT} Packet: (0x88C8) CMSG_BATTLEFIELD_LEAVE_JOIN_REPLY PacketSize = 5 TimeStamp = 50502375
78 00 1E 11 
03 
*/
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	recv_data >> action >> mapinfo[0] >> mapinfo[1] >> mapinfo[2];
	recv_data >> bgtype;

	if( (action & (0x0F)) == 0)
	{
		BattlegroundManager.RemovePlayerFromQueues(_player);
	}
	else
	{
		if( _player->m_pendingBattleground )
		{
			if( BattlegroundManager.IsValidBGPointer( _player->m_pendingBattleground ) )
				_player->m_pendingBattleground->PortPlayer(_player);
			//omef mem corruption or i forgot to clear this variable ?
			else
			{
				_player->m_pendingBattleground = NULL; 
				BattlegroundManager.RemovePlayerFromQueues(_player);
			}
		}
	}
}
/*
void WorldSession::HandleBattlefieldStatusOpcode(WorldPacket &recv_data)
{
	// This is done based on whether we are queued, inside, or not in a battleground.
	if(_player->m_pendingBattleground)		// Ready to port
	{
		if( BattlegroundManager.IsValidBGPointer( _player->m_pendingBattleground ) )
			BattlegroundManager.SendBattlefieldStatus(_player, BF_STATUS_PACKET_ALLOW_ENTER_PLAYER, _player->m_pendingBattleground->GetType(), _player->m_pendingBattleground->GetId(), 120000, 0, _player->m_pendingBattleground->Rated());
		else
			_player->m_pendingBattleground = NULL;
	}
	else if(_player->m_bg)					// Inside a bg
		BattlegroundManager.SendBattlefieldStatus(_player, BF_STATUS_PACKET_TEL_PLAYER, _player->m_bg->GetType(), _player->m_bg->GetId(), (uint32)UNIXTIME - _player->m_bg->GetStartTime(), _player->GetMapId(), _player->m_bg->Rated());
	else									// None
		BattlegroundManager.SendBattlefieldStatus(_player, BF_STATUS_PACKET_REMOVE_PLAYER, 0, 0, 0, 0, 0);	
}*/

void WorldSession::HandleBattlefieldListOpcode(WorldPacket &recv_data)
{
/*	uint64 guid;
	recv_data >> guid;

	CHECK_INWORLD_RETURN;
	Creature * pCreature = _player->GetMapMgr()->GetCreature( GET_LOWGUID_PART(guid) );
	if( pCreature == NULL )
		return;

	SendBattlegroundList( pCreature, 0 );*/
/*
13329
{CLIENT} Packet: (0x4136) CMSG_BATTLEFIELD_LIST PacketSize = 4 TimeStamp = 22579303
20 00 00 00 
14333
{CLIENT} Packet: (0x38E8) CMSG_BATTLEFIELD_LIST PacketSize = 4 TimeStamp = 1961650
02 00 00 00 
*/
	uint32 battlegroundType;
//	uint8 requestType; // 0 = ShowBattlefieldList, 1 = RequestBattlegroundInstanceInfo
	recv_data >> battlegroundType;
	CHECK_INWORLD_RETURN;
	BattlegroundManager.HandleBattlegroundListPacket(this, battlegroundType, false);
}

void WorldSession::SendBattlegroundList(Creature* pCreature, uint32 mapid)
{
	if(!pCreature)
	{ 
		return;
	}

	/* we should have a bg id selection here. */
	uint32 t = BATTLEGROUND_WARSONG_GULCH;
	if (mapid == 0)
	{
		if(pCreature->GetCreatureInfo())
		{
			if(strstr(pCreature->GetCreatureInfo()->SubName, "Arena") != NULL)
				t = BATTLEGROUND_ARENA_2V2;
			else if(strstr(pCreature->GetCreatureInfo()->SubName, "Arathi") != NULL)
				t = BATTLEGROUND_ARATHI_BASIN;
			else if(strstr(pCreature->GetCreatureInfo()->SubName, "Eye of the Storm") != NULL)
				t = BATTLEGROUND_EYE_OF_THE_STORM;
			else if(strstr(pCreature->GetCreatureInfo()->SubName, "Warsong") != NULL)
				t = BATTLEGROUND_WARSONG_GULCH;
			else if(strstr(pCreature->GetCreatureInfo()->SubName, "Twin") != NULL)
				t = BATTLEGROUND_TWIN_PEAKS;
			else if(strstr(pCreature->GetCreatureInfo()->SubName, "Alterac") != NULL)
				t = BATTLEGROUND_ALTERAC_VALLEY;
			else if(strstr(pCreature->GetCreatureInfo()->SubName, "Strand") != NULL)
				t = BATTLEGROUND_STRAND_OF_THE_ANCIENT;
		}
	}
	else
		t = mapid;

    BattlegroundManager.HandleBattlegroundListPacket( this, t, true );
}

void WorldSession::HandleBattleMasterHelloOpcode(WorldPacket &recv_data)
{
	CHECK_PACKET_SIZE(recv_data, 8);
	if( !_player->IsInWorld() )
	{ 
		return;
	}

	uint64 guid;
	recv_data >> guid;
	sLog.outDebug("Received CMSG_BATTLEMASTER_HELLO from " I64FMT, guid);

	Creature * bm = _player->GetMapMgr()->GetCreature( guid );

	if(!bm)
	{ 
		return;
	}

	if(!bm->isBattleMaster())		// Not a Battlemaster
	{ 
		return;
	}

	SendBattlegroundList(bm, 0);
}

void WorldSession::HandleLeaveBattlefieldOpcode(WorldPacket &recv_data)
{
	if(_player->m_bg && _player->IsInWorld())
	{
		if( BattlegroundManager.IsValidBGPointer( _player->m_bg ) )
			_player->m_bg->RemovePlayer(_player, false);
		else
			_player->m_bg = NULL;
	}
}

void WorldSession::HandleAreaSpiritHealerQueryOpcode(WorldPacket &recv_data)
{
	if(!_player->IsInWorld() || !_player->m_bg) 
	{ 
		return;
	}
	uint64 guid;
	recv_data >> guid;

	Creature * psg = _player->GetMapMgr()->GetCreature( guid );
	if(psg == NULL)
	{ 
		return;
	}
	
	uint32 restime = _player->m_bg->GetLastResurrect() + 30;
	if((uint32)UNIXTIME > restime)
		restime = 1000;
	else
		restime = (restime - (uint32)UNIXTIME) * 1000;

	WorldPacket data(SMSG_AREA_SPIRIT_HEALER_TIME, 12);
	data << guid << restime;
	SendPacket(&data);
}

void WorldSession::HandleAreaSpiritHealerQueueOpcode(WorldPacket &recv_data)
{
	if(!_player->IsInWorld() || !_player->m_bg) 
	{ 
		return;
	}
	uint64 guid;
	recv_data >> guid;
	Creature * psg = _player->GetMapMgr()->GetCreature( guid );
	if(psg == NULL)
	{ 
		return;
	}

	//this spell removes us from queues in case we were in one
//	_player->CastSpell(_player,2584,true);	//waiting to be resurrected
	if( sEventMgr.HasEvent( _player, EVENT_PLAYER_DELAYED_BG_RES_QUEUE ) == false )
	{
		//sadly if we teleport and repop too quickly this will bug out and mix up update packet order
		//we can end up dead and not moving or alive and half dead....
		uint32 NextResIn = sEventMgr.GetTimeLeft( _player->m_bg, EVENT_BATTLEGROUND_QUEUE_UPDATE );
		if( NextResIn >= 3000 )
			_player->m_bg->QueuePlayerForResurrect( _player, psg );
		else
			sEventMgr.AddEvent( _player, &Player::DelayedBGResurrect, guid, EVENT_PLAYER_DELAYED_BG_RES_QUEUE, NextResIn + 500, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
	}
}

void WorldSession::HandleBattlegroundPlayerPositionsOpcode(WorldPacket &recv_data)
{
	// empty opcode
	CBattleground *bg = _player->m_bg;
    if(!_player->IsInWorld() || !bg)
        return;

	if(bg->GetType() == BATTLEGROUND_WARSONG_GULCH)
    {
/*
14333
{SERVER} Packet: (0x5763) UNKNOWN PacketSize = 22 TimeStamp = 2214839
00 00 00 00 01 00 00 00 9B 23 67 2A C0 44 B5 24 B9 44 04 D7 05 F2 
{SERVER} Packet: (0x5763) UNKNOWN PacketSize = 22 TimeStamp = 2219722
00 00 00 00 01 00 00 00 9B 23 81 2B BB 44 84 F1 BA 44 04 D7 05 F2 

23 D7 F2 05 00 00 00 04  -> obfuscated full guid
00 00 00 00 
01 00 00 00 
9B 10011011 - mask ?
23 go0
81 2B BB 44 1497.3594970703 
84 F1 BA 44 1495.5473632812 
04 go7
D7 go1
05 go3
F2 go2
{SERVER} Packet: (0x5763) UNKNOWN PacketSize = 36 TimeStamp = 2259783
00 00 00 00 02 00 00 00 9B 9B 17 FD F5 82 44 46 84 A2 44 03 55 04 A0 23 4A 8F A5 44 A6 2A AA 44 04 D7 05 F2 
*/
        uint32 count1 = 0;
		uint32 count2 = 0;

        Player *ap = objmgr.GetPlayer(static_cast<WarsongGulch*>(bg)->GetAllianceFlagHolderGUID());
        if(ap) ++count2;

        Player *hp = objmgr.GetPlayer(static_cast<WarsongGulch*>(bg)->GetHordeFlagHolderGUID());
        if(hp) ++count2;

        WorldPacket data(SMSG_BATTLEGROUND_PLAYER_POSITIONS, (4+4+16*count1+16*count2));
        data << count1;
        data << count2;
		if(ap)
		{
			uint64 guid = ap->GetGUID();
			uint8 *guid_bytes = (uint8*)&guid;
			data << uint8( 0x9B );	//obfuscated guid mask
			data << ObfuscateByte( guid_bytes[0] );
			data << (float)ap->GetPositionX();
			data << (float)ap->GetPositionY();
			data << ObfuscateByte( guid_bytes[7] );
			data << ObfuscateByte( guid_bytes[1] );
			data << ObfuscateByte( guid_bytes[3] );
			data << ObfuscateByte( guid_bytes[2] );
		}
		if(hp)
		{
			uint64 guid = hp->GetGUID();
			uint8 *guid_bytes = (uint8*)&guid;
			data << uint8( 0x9B );	//obfuscated guid mask
			data << ObfuscateByte( guid_bytes[0] );
			data << (float)hp->GetPositionX();
			data << (float)hp->GetPositionY();
			data << ObfuscateByte( guid_bytes[7] );
			data << ObfuscateByte( guid_bytes[1] );
			data << ObfuscateByte( guid_bytes[3] );
			data << ObfuscateByte( guid_bytes[2] );
		}

		SendPacket(&data);
	}
	else if(bg->GetType() == BATTLEGROUND_TWIN_PEAKS)
    {
        uint32 count1 = 0;
		uint32 count2 = 0;

        Player *ap = objmgr.GetPlayer(static_cast<TwinPeaks*>(bg)->GetAllianceFlagHolderGUID());
        if(ap) ++count2;

        Player *hp = objmgr.GetPlayer(static_cast<TwinPeaks*>(bg)->GetHordeFlagHolderGUID());
        if(hp) ++count2;

        WorldPacket data(SMSG_BATTLEGROUND_PLAYER_POSITIONS, (4+4+16*count1+16*count2));
        data << count1;
        data << count2;
		if(ap)
		{
			uint64 guid = ap->GetGUID();
			uint8 *guid_bytes = (uint8*)&guid;
			data << uint8( 0x9B );	//obfuscated guid mask
			data << ObfuscateByte( guid_bytes[0] );
			data << (float)ap->GetPositionX();
			data << (float)ap->GetPositionY();
			data << ObfuscateByte( guid_bytes[7] );
			data << ObfuscateByte( guid_bytes[1] );
			data << ObfuscateByte( guid_bytes[3] );
			data << ObfuscateByte( guid_bytes[2] );
		}
		if(hp)
		{
			uint64 guid = hp->GetGUID();
			uint8 *guid_bytes = (uint8*)&guid;
			data << uint8( 0x9B );	//obfuscated guid mask
			data << ObfuscateByte( guid_bytes[0] );
			data << (float)hp->GetPositionX();
			data << (float)hp->GetPositionY();
			data << ObfuscateByte( guid_bytes[7] );
			data << ObfuscateByte( guid_bytes[1] );
			data << ObfuscateByte( guid_bytes[3] );
			data << ObfuscateByte( guid_bytes[2] );
		}

		SendPacket(&data);
	} 
	else if(bg->GetType() == BATTLEGROUND_EYE_OF_THE_STORM)
	{
		uint32 count1 = 0;
		uint32 count2 = 0;

		Player *ap = objmgr.GetPlayer(static_cast<EyeOfTheStorm*>(bg)->GetFlagHolderGUID());
		if(ap) ++count2;

		WorldPacket data(SMSG_BATTLEGROUND_PLAYER_POSITIONS, (4+4+16*count1+16*count2));
		data << count1;
		data << count2;
		if(ap)
		{
			data << (uint64)ap->GetGUID();
			data << (float)ap->GetPositionX();
			data << (float)ap->GetPositionY();
		}

		SendPacket(&data);
	}
	else
	{
		//{SERVER} Packet: (0x5763) SMSG_BATTLEGROUND_PLAYER_POSITIONS PacketSize = 8 TimeStamp = 2949791
		//00 00 00 00 00 00 00 00 
		sStackWorldPacket( data, SMSG_BATTLEGROUND_PLAYER_POSITIONS, 8 + 1 );
		data << uint32( 0 );
		data << uint32( 0 );
		SendPacket(&data);
	}
}

void WorldSession::HandleBattleMasterJoinOpcode(WorldPacket &recv_data)
{
	CHECK_INWORLD_RETURN

	if( GetPlayer()->IsSpectator() )
	{
		_player->BroadcastMessage("Can't queue while spectator");
		return;
	}

	if(_player->HasAura(BG_DESERTER))
	{
/*
		//needs update for 4.2 client !!
		WorldPacket data(SMSG_GROUP_JOINED_BATTLEGROUND, 4);
		data << (uint32) 0xFFFFFFFE;
		_player->GetSession()->SendPacket(&data); */
		return;
	}

	if(_player->GetGroup() && _player->GetGroup()->m_isqueued)
	{
		SystemMessage("You are in a group that is already queued for a battleground or inside a battleground. Leave this first.");
		return;
	}

	/* are we already in a queue? */
	if(_player->m_bgIsQueued)
		BattlegroundManager.RemovePlayerFromQueues(_player);

	if(_player->IsInWorld())
		BattlegroundManager.HandleBattlegroundJoin(this, recv_data);
}

void WorldSession::HandleArenaJoinOpcode(WorldPacket &recv_data)
{
	CHECK_INWORLD_RETURN
	if( _player->IsSpectator() )
	{
		_player->BroadcastMessage("Can't queue while spectator");
		return;
	}
	uint32 RemainingMS = objmgr.PlayerHasDeserterCooldown( _player->GetGUID() );
	if( RemainingMS > 0 )
	{
		_player->BroadcastMessage( "Player %s has %d seconds Deserter buff cooldown", _player->GetName(), RemainingMS / 1000 );
		return;
	}
	if(_player->GetGroup() && _player->GetGroup()->m_isqueued)
	{
		SystemMessage("You are in a group that is already queued for a battleground or inside a battleground. Leave this first.");
		return;
	}

	/* are we already in a queue? */
    if(_player->m_bgIsQueued)
		BattlegroundManager.RemovePlayerFromQueues(_player);

	uint32 bgtype=0;
	uint8 arenacategory;
	uint8 as_group = 1;
	uint8 rated_match = 1;
//    recv_data >> guid >> arenacategory >> as_group >> rated_match;
    recv_data >> arenacategory;
	switch(arenacategory)
	{
	case 0:		// 2v2
		bgtype = BATTLEGROUND_ARENA_2V2;
		break;

	case 1:		// 3v3
		bgtype = BATTLEGROUND_ARENA_3V3;
		break;

	case 2:		// 5v5
		bgtype = BATTLEGROUND_ARENA_5V5;
		break;
	}

	//check deserter buff
	if( as_group && _player->GetGroup() )
	{
		Group *pGroup = _player->GetGroup();
		bool GroupCanJoin = true;
		GroupMembersSet::iterator itx;
		pGroup->Lock();
		for(itx = pGroup->GetSubGroup(0)->GetGroupMembersBegin(); itx != pGroup->GetSubGroup(0)->GetGroupMembersEnd(); ++itx)
		{
			if( (*itx)->m_loggedInPlayer )
			{
				uint32 RemainingMS = objmgr.PlayerHasDeserterCooldown( (*itx)->m_loggedInPlayer->GetGUID() );
				if( RemainingMS > 0 )
				{
					_player->BroadcastMessage( "Player %s has %d seconds Deserter buff cooldown", (*itx)->m_loggedInPlayer->GetName(), RemainingMS / 1000 );
					GroupCanJoin = false;
				}
			}
		}
		pGroup->Unlock();
		if( GroupCanJoin == false )
			return;
	}

	if(bgtype != 0)
	{
		BattlegroundManager.HandleArenaJoin(this, bgtype, as_group, rated_match);
	}
}

void WorldSession::HandleInspectHonorStatsOpcode( WorldPacket &recv_data )
{
	CHECK_INWORLD_RETURN

    uint8 guid_mask;
	uint8 guid[8];
	*(uint64*)guid = 0;
    recv_data >> guid_mask;
//	if( guid_mask & BIT_X )
//		recv_data >> guid[7];
	if( guid_mask & BIT_2 )
		recv_data >> guid[0];
	if( guid_mask & BIT_5 )
		recv_data >> guid[3];
//	if( guid_mask & BIT_X )
//		recv_data >> guid[6];
	if( guid_mask & BIT_1 )
		recv_data >> guid[1];
	if( guid_mask & BIT_4 )
		recv_data >> guid[2];

	GUID_un_obfuscate( guid );
    Player* player =  _player->GetMapMgr()->GetPlayer( *(uint64*)guid );
  	if( player == NULL )
	{
		sLog.outError( "CMSG_INSPECT_HONOR_STATS : _player was not found for guid" );
		return;
	}

	//this packet was made for 14333. I doubt this is correct packet though :(
    WorldPacket data( SMSG_INSPECT_HONOR_STATS, 13 );
	data << uint8( 0xE7 );
	data << ObfuscateByte( guid[6] );
	data << ObfuscateByte( guid[7] );
	data << ObfuscateByte( guid[3] );
	data << uint16( 0x0001 );	//?
	data << ObfuscateByte( guid[0] );
	data << uint8( 0 );	//?
	data << ObfuscateByte( guid[1] );
	data << uint16( 0x0000 );	//?
//    data << player->GetUInt32Value( PLAYER_FIELD_KILLS );
    data << player->GetUInt32Value( PLAYER_FIELD_LIFETIME_HONORBALE_KILLS );
	data << ObfuscateByte( guid[2] );

    SendPacket( &data );
}

void WorldSession::HandleInspectArenaStatsOpcode( WorldPacket & recv_data )
{
    CHECK_PACKET_SIZE( recv_data, 8 );
	CHECK_INWORLD_RETURN

    uint64 guid;
    recv_data >> guid;

    Player* player =  _player->GetMapMgr()->GetPlayer( guid );
	if( player == NULL )
	{
		sLog.outError( "HandleInspectHonorStatsOpcode : guid was null" );
		return;
	}

	uint32 id;

    for( uint8 i = 0; i < 3; i++ )
    {
		id = player->GetUInt32Value( PLAYER_FIELD_ARENA_TEAM_INFO_1_1 + ( i * ARENA_TEAM_INFO_SIZE ) + ARENA_TEAM_ID );
        if( id > 0 )
        {
            ArenaTeam* team = objmgr.GetArenaTeamById( id );
            if( team != NULL )
			{
/*
14333
{SERVER} Packet: (0x72A3) MSG_INSPECT_ARENA_TEAMS PacketSize = 33 TimeStamp = 1742031
22 31 72 00 00 00 80 01 guid
00 type 2v2
9C 45 59 00 = 5850524  team ID
F6 04 00 00 = 1270 team rating / rating
1C 00 00 00 games played
10 00 00 00 win
1C 00 00 00 games played
F6 04 00 00 team rating / rating
*/
				//when you inspect someone, these will be shown "under" the team type line ( multiple stats )
				WorldPacket data( MSG_INSPECT_ARENA_TEAMS, 8 + 1 + 4 * 5 );
				data << player->GetGUID();
				data << (uint8)(team->m_type);
				data << team->m_id;
				data << team->m_stat_rating;					//team rating
				data << team->m_stat_gamesplayedseason;			//team season games
				data << team->m_stat_gameswonseason;			//season win
				data << team->m_stat_gamesplayedseason;			//season games
				data << team->m_stat_rating;					//season rating
				SendPacket( &data );

				//blizz sent this, useless since inspect already contains all the data ?
//				team->Stat( data );
//				SendPacket( &data );
			}
        }
    }
}


void WorldSession::HandlePVPLogDataOpcode(WorldPacket &recv_data)
{
	CHECK_INWORLD_RETURN
	if(_player->m_bg)
		_player->m_bg->SendPVPData(_player);
}

void WorldSession::HandleInspectRatedBG( WorldPacket & recv_data )
{
	CHECK_INWORLD_RETURN

    uint8 guid_mask;
	uint8 guid[8];
	*(uint64*)guid = 0;
    recv_data >> guid_mask;
//	if( guid_mask & BIT_X )
//		recv_data >> guid[6];
	if( guid_mask & BIT_1 )
		recv_data >> guid[1];
	if( guid_mask & BIT_2 )
		recv_data >> guid[3];
	if( guid_mask & BIT_7 )
		recv_data >> guid[0];
	if( guid_mask & BIT_5 )
		recv_data >> guid[2];
//	if( guid_mask & BIT_X )
//		recv_data >> guid[7];

	GUID_un_obfuscate( guid );
    Player* player =  _player->GetMapMgr()->GetPlayer( *(uint64*)guid );
	if( player == NULL )
	{
		sLog.outError( "CMSG_INSPECT_RATEG_BG : could not find player for guid" );
		return;
	}

	AchievementVal *achi;
	uint32 win_count = 0; 
	uint32 total_matches = 0; 
	uint32 rating = 0;

	//these are temp values until we implement rated BG
	achi = player->m_sub_achievement_criterias[ 140 ]; //WSG victories
	if( achi )
		win_count = achi->cur_value;
	achi = player->m_sub_achievement_criterias[ 111 ]; //total deaths achievements : 111
	if( achi )
		total_matches = achi->cur_value;
	rating = player->GetUInt32Value( PLAYER_FIELD_LIFETIME_HONORBALE_KILLS );

	sStackWorldPacket( data, SMSG_INSPECT_RATEG_BG, 3*4+9+10 );
	data << uint8( 0x3F );		
	data << uint32( win_count );
	data << ObfuscateByte( guid[0] );
	data << ObfuscateByte( guid[6] );
	data << ObfuscateByte( guid[1] );
	data << ObfuscateByte( guid[7] );
	data << uint32( total_matches );
	data << uint32( rating );
	data << ObfuscateByte( guid[3] );
	data << ObfuscateByte( guid[2] );
	SendPacket( &data );

}

void WorldSession::HandleBattlefieldRequestRatedArenaRewards(WorldPacket &recv_data)
{
	if( !_player->IsInWorld() )
	{ 
		return;
	}

	sLog.outDebug("Received CMSG_REQUEST_RATED_ARENA_INFO from ");
/*
!! i have absolutly no idea how to name these packets, there are 4 requested and 4 send, this could anything !
{SERVER} Packet: (0x8723) CMSG_REQUEST_RATED_ARENA_INFO PacketSize = 24 TimeStamp = 15739315
72 06 00 00 
00 00 00 00 
72 06 00 00 
00 00 00 00 
46 05 00 00 
00 00 00 00 
{SERVER} Packet: (0x1F07) SMSG_REQUEST_RATED_ARENA_INFO PacketSize = 29 TimeStamp = 2996311
03 
00 00 00 00 
00 00 00 00 
90 01 00 00 400 
72 06 00 00 1650
00 00 00 00 
00 00 00 00 
72 06 00 00 1650
{SERVER} Packet: (0x1F07) SMSG_REQUEST_RATED_ARENA_INFO PacketSize = 29 TimeStamp = 2799827
00 
00 00 00 00 
00 00 00 00 
B4 00 00 00 180 
72 06 00 00 1650 
00 00 00 00 
00 00 00 00 
46 05 00 00 1350 
*/
	uint8 Type;
	recv_data >> Type;

	sStackWorldPacket( data, SMSG_REQUEST_RATED_ARENA_INFO, 24+10);
	data << uint8( Type );
	data << uint32( 0 );
	data << uint32( 0 );
	data << uint32( 180 );
	data << uint32( 1650 );
	data << uint32( 0 );
	data << uint32( 0 );
	data << uint32( 1350 );
	SendPacket( &data );
}

void WorldSession::HandleBattlefieldRequestRatedBGRewards(WorldPacket &recv_data)
{
	if( !_player->IsInWorld() )
	{ 
		return;
	}

	sLog.outDebug("Received CMSG_REQUEST_RATED_BG_INFO from ");
/*
{SERVER} Packet: (0x1F07) SMSG_REQUEST_RATED_BG_INFO PacketSize = 29 TimeStamp = 15739471
03 
00 00 00 00 
00 00 00 00
00 00 00 00 
72 06 00 00 
00 00 00 00 
00 00 00 00 
72 06 00 00 
14333
{SERVER} Packet: (0x8723) SMSG_REQUEST_RATED_BG_INFO PacketSize = 24 TimeStamp = 2996124
72 06 00 00 00 00 00 00 
72 06 00 00 00 00 00 00 
46 05 00 00 00 00 00 00 
*/
	uint8 type;
	recv_data >> type;

	sStackWorldPacket( data, SMSG_REQUEST_RATED_BG_INFO, 24+10);
	data << uint64( 1650 );
	data << uint64( 1650 );
	data << uint64( 1350 );
	SendPacket( &data );
}

void WorldSession::HandleBattlefieldRequestWargameScore(WorldPacket &recv_data)
{
	if( !_player->IsInWorld() )
	{ 
		return;
	}

	sLog.outDebug("Received CMSG_BATTLEFIELD_GROUP_WARGAME_SCORE from ");
/*
{SERVER} Packet: (0x4743) SMSG_BATTLEFIELD_GROUP_FIGHT_SCORE PacketSize = 72 TimeStamp = 15739050
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
*/
	sStackWorldPacket( data, SMSG_BATTLEFIELD_GROUP_WARGAME_SCORE, 72+10);
	for( uint32 i=0;i<18;i++)
		data << uint32( 0 );
	SendPacket( &data );
}

void WorldSession::HandleBattlefieldQueryEnableTypes(WorldPacket &recv_data)
{
	if( !_player->IsInWorld() )
	{ 
		return;
	}

	sLog.outDebug("Received CMSG_PVP_TYPES_ENABLED from ");
/*
{SERVER} Packet: (0x0725) SMSG_PVP_TYPES_ENABLED PacketSize = 1 TimeStamp = 15739471
F8 
*/
	sStackWorldPacket( data, SMSG_PVP_TYPES_ENABLED, 1+10);
	data << uint8( 0xF8 );
	SendPacket( &data );
}

void PrintWarGameStatus( Player *Inviter, Player *p1, Player *p2 )
{
	char PrintBuff[2000];
	PrintBuff[0]=0;

	Inviter->WargamesInitialize();

	sprintf_s( PrintBuff, 2000, "%s Group Size : %d \n", PrintBuff, p2->GetGroup()->MemberCountOnline() ); 

	sprintf_s( PrintBuff, 2000, "%s Group Players : ", PrintBuff ); 
	Group *group = p2->GetGroup();
	uint32 count = group->GetSubGroupCount();
	// Loop through each raid group.
	for( uint8 k = 0; k < count; k++ )
	{
		SubGroup * subgroup = group->GetSubGroup( k );
		if( subgroup )
		{
			group->Lock();
			for( GroupMembersSet::iterator itr = subgroup->GetGroupMembersBegin(); itr != subgroup->GetGroupMembersEnd(); ++itr )
				if( (*itr)->m_loggedInPlayer )
					sprintf_s( PrintBuff, 2000, "%s, %s(%d)", PrintBuff, (*itr)->m_loggedInPlayer->GetName(), (*itr)->m_loggedInPlayer->GetMaxPersonalRating() ); 
			group->Unlock();
		}
	}
	sprintf_s( PrintBuff, 2000, "%s \n", PrintBuff ); 
	sprintf_s( PrintBuff, 2000, "%s Allow Unbalanced Teams : %d \n", PrintBuff, Inviter->m_Wargames->SkipPartySizeCheck ); 
	sprintf_s( PrintBuff, 2000, "%s Use Sudden Death : %d \n", PrintBuff, Inviter->m_Wargames->SuddenDeath ); 
	sprintf_s( PrintBuff, 2000, "%s Player Death Limit : %d \n", PrintBuff, Inviter->m_Wargames->PlayerLives ); 
	sprintf_s( PrintBuff, 2000, "%s Team Death Limit : %d \n", PrintBuff, Inviter->m_Wargames->TeamLives ); 

	WorldPacket *data = sChatHandler.FillMessageData( CHAT_MSG_WHISPER, LANG_UNIVERSAL, PrintBuff, p2->GetGUID(), 0, 0, p1  );
	p1->GetSession()->SendPacket(data);
	delete data;
	data = NULL;
}

void WorldSession::HandleWargamesInviteParty(WorldPacket &recv_data)
{
	if( !_player->IsInWorld() )
	{ 
		return;
	}

	sLog.outDebug("Received CMSG_WARGAMES_INVITE_PARTY_LEADER from ");
	
	Player *p = GetPlayer()->GetMapMgr()->GetPlayer( GetPlayer()->GetSelection() );
	if( p == NULL || p->GetGroup() == NULL || p->GetGroup()->GetLeader() != p->getPlayerInfo() )
	{
		GetPlayer()->BroadcastMessage("You need to select a party leader");
		return;
	}

	p->WargamesInitialize();

	if( p->m_Wargames->OtherPartyLeader != 0 )
	{
		GetPlayer()->BroadcastMessage("That player already has a pending request");
		return;
	}
/*
{CLIENT} Packet: (0x0011) CMSG_WARGAMES_INVITE_PARTY_LEADER PacketSize = 12 TimeStamp = 4599237
2F E5 mask
02 go3
00 
03 
42 go0
39 go2
02 1E 11 
E6 go1
04 BattlegroundDbcIndex
{CLIENT} Packet: (0x0011) UNKNOWN PacketSize = 11 stamp = 16009446
27 E5 
21 go3
03 unk_0
27 go0
23 go2
02 unk_1
1E unk_2
11 unk_3
25 go1
05 BattlegroundDbcIndex
{CLIENT} Packet: (0x0011) CMSG_WARGAMES_INVITE_PARTY_LEADER PacketSize = 11 stamp = 9260156
27 E5 
08 -pg3
03 
02 
06 -pg2
02 -pg0
1E 11 
04 -pg1
05
*/
	uint16	guid_mask;	//could be party ID mask
	uint8	target_guid[8];	//obfuscated !
	uint8	unk_byte[4];
	uint8	BattlegroundDbcIndex;
	*(uint64*)target_guid = 0;
	unk_byte[0] = 0;

	recv_data >> guid_mask;
	if( guid_mask & 0x0002 )
		recv_data >> target_guid[3];
	if( guid_mask & 0x0008 )
		recv_data >> target_guid[7];	//might not be byte 7
	if( guid_mask & 0x2000 )
		recv_data >> unk_byte[0];	//seen values : 3
	if( guid_mask & 0x0400 )
		recv_data >> target_guid[0];
	if( guid_mask & 0x8000 )
		recv_data >> target_guid[2];
	if( guid_mask & 0x0025 )
	{
		recv_data >> unk_byte[1];	//seen values : 2	always
		recv_data >> unk_byte[2];	//seen values : 1E	always
		recv_data >> unk_byte[3];	//seen values : 11	always
	}
	if( guid_mask & 0x4000 )
		recv_data >> target_guid[1];
	recv_data >> BattlegroundDbcIndex;
	if( BattlegroundDbcIndex > BGDBC_ROWS )
	{
		GetPlayer()->BroadcastMessage("You managed to request a not yet implemented map. Report to devs id %d and your name and target name",BattlegroundDbcIndex);
		return;
	}

	GetPlayer()->WargamesInitialize();
	p->WargamesInitialize();

	//tell players how the groups look like
	PrintWarGameStatus( GetPlayer(), GetPlayer(), p );
	PrintWarGameStatus( GetPlayer(), p, GetPlayer() );

	//store state so we know who to blame if we need to blame
	if( GetPlayer()->m_Wargames->OtherPartyLeader != 0 )
	{
		Player *p2 = GetPlayer()->GetMapMgr()->GetPlayer( GetPlayer()->m_Wargames->OtherPartyLeader );
		if( p2 )
		{
			p2->WargamesInitialize(); //this did crash once ...
			if( p2->m_Wargames->OtherPartyLeader == GetPlayer()->GetGUID() )
				p2->m_Wargames->OtherPartyLeader = 0;
		}
	}
	p->m_Wargames->OtherPartyLeader = GetPlayer()->GetGUID();
	GetPlayer()->m_Wargames->OtherPartyLeader = p->GetGUID();
	GetPlayer()->m_Wargames->BGType = BattlegroundDbcIndex;

	//tell our client we tried to initiate invite
/*
{SERVER} Packet: (0x0B65) SMSG_WARGAMES_INVITE_PARTY_LEADER_ACK PacketSize = 6 TimeStamp = 4599408
DA - mask ?
39 go2
42 go0
02 go3
E6 go1
00 go7
*/
	{
		*(uint64*)target_guid = p->GetGUID();
		GUID_un_obfuscate( target_guid );
		sStackWorldPacket( data, SMSG_WARGAMES_INVITE_PARTY_LEADER_ACK, 6 + 10 );
		data << uint8( 0xDA );	//guid mask
		data << uint8( target_guid[2] );	
		data << uint8( target_guid[0] );	
		data << uint8( target_guid[3] );	
		data << uint8( target_guid[1] );	
		data << uint8( target_guid[7] );	
		SendPacket( &data );
	}
	//send gui popup to the target player
/*
{SERVER} Packet: (0x0707) SMSG_WARGAMES_INVITE_PARTY_LEADER_OTHER PacketSize = 16 TimeStamp = 4229358
E3 F2 mask ?
39 go2
00 go7
5A 00 00 00 ? = 90 - maybe timeout time ?
1E 11 02 ? - magic const
E6 go1
05 BattlegroundDbcIndex
03 ? - magic const 2
42 go0
02 go3
*/
	{
		*(uint64*)target_guid = GetPlayer()->GetGUID();
		GUID_obfuscate( target_guid );
		sStackWorldPacket( data, SMSG_WARGAMES_INVITE_PARTY_LEADER_OTHER, 16 + 10 );
		data << uint16( 0xF2E3 );	//guid mask
		data << uint8( target_guid[2] );	
		data << uint8( target_guid[7] );	
		data << uint32( 0x0000005A );	//unk = 90
		data << uint8( unk_byte[2] );	//unk 1E
		data << uint8( unk_byte[3] );	//unk 11
		data << uint8( target_guid[3] );	
		data << uint8( target_guid[1] );	
		data << uint8( BattlegroundDbcIndex );	//unk
		data << uint8( unk_byte[0] );	//unk
		data << uint8( target_guid[0] );	
		data << uint8( unk_byte[1] );	//unk 02
		p->GetSession()->SendPacket( &data );
		p->BroadcastMessage("%s has Invited you to join wargames", GetPlayer()->GetName() );
	}
}

void WorldSession::HandleWargamesInvitePartyReply(WorldPacket &recv_data)
{
	if( !_player->IsInWorld() )
	{ 
		return;
	}

	sLog.outDebug("Received CMSG_WARGAMES_INVITE_PARTY_LEADER_OTHER_REPLY from ");
	
	GetPlayer()->WargamesInitialize();

	Player *Inviter = GetPlayer()->GetMapMgr()->GetPlayer( GetPlayer()->m_Wargames->OtherPartyLeader );
	if( Inviter == NULL || Inviter->GetGroup() == NULL || Inviter->GetGroup()->GetLeader() != Inviter->getPlayerInfo() )
	{
		GetPlayer()->BroadcastMessage("Wargames inviter is missing or not group leader");
		return;
	}
	if( GetPlayer() == NULL || GetPlayer()->GetGroup() == NULL || GetPlayer()->GetGroup()->GetLeader() != GetPlayer()->getPlayerInfo() )
	{
		GetPlayer()->BroadcastMessage("Wargames target is missing or not group leader");
		return;
	}
	
	Inviter->WargamesInitialize();

/*
{CLIENT} Packet: (0x4081) CMSG_WARGAMES_INVITE_PARTY_LEADER_OTHER_REPLY PacketSize = 13 TimeStamp = 4231059
D3 4F mask
80 - reject
00 go7
1E unk_2
05 BattlegroundDbcIndex
11 unk_3
03 unk_0
39 go2
42 go0
E6 go1
02 go3 ?
02 unk_1 ?
{CLIENT} Packet: (0x4081) CMSG_WARGAMES_INVITE_PARTY_LEADER_OTHER_REPLY PacketSize = 9 stamp = 11997537
51 0D = 1010001 00001101 
00 ?
1E ?
05 BattlegroundDbcIndex
11 ?
03 
02 pg
02 ?
*/
	uint16	guid_mask;	//could be party ID mask
	uint8	unk;
	uint8	target_guid[8];	//obfuscated !
	uint8	unk_byte[4];
	*(uint64*)target_guid = 0;
	unk_byte[0] = 0;

	recv_data >> guid_mask;
	recv_data >> unk;
/*
	if( guid_mask & 0x0000 )
		recv_data >> target_guid[7];
	if( guid_mask & 0x0000 )
		recv_data >> unk_byte[2];	//seen values : 1E	always
	recv_data >> BattlegroundDbcIndex;
	if( guid_mask & 0x0000 )
		recv_data >> unk_byte[3];	//seen values : 11	always
	if( guid_mask & 0x0000 )
		recv_data >> unk_byte[0];	//seen values : 3
	if( guid_mask & 0x0000 )
		recv_data >> target_guid[2];	//might not be byte 7
	if( guid_mask & 0x0000 )
		recv_data >> target_guid[0];
	if( guid_mask & 0x0000 )
		recv_data >> target_guid[1];
	if( guid_mask & 0x0000 )
		recv_data >> target_guid[3];
	if( guid_mask & 0x0000 )
		recv_data >> unk_byte[1];	//seen values : 2	always
*/
	bool TargetAccepted = ( guid_mask & 0x0200 ) != 0;
	uint32 PlayerCount1 = 0, PlayerCount2 = 0;

	//check if we have the same setup as we need for the wargame
	if( TargetAccepted == true )
	{
		BattlemasterListEntry *bm = dbcBattlemasterListStore.LookupEntryForced( Inviter->m_Wargames->BGType );
		if( bm == NULL )
		{
			GetPlayer()->BroadcastMessage("Internal Error.Could not find this map. Report to devs" );
			Inviter->BroadcastMessage("Internal Error.Could not find this map. Report to devs" );
			TargetAccepted = false;
		}
//		uint32 MinPlayerCount = BattlegroundManager.GetMinimumPlayersDBC( Inviter->m_Wargames->BGType );
		uint32 MinPlayerCount = 1;
		if( Inviter->m_Wargames->SkipPartySizeCheck == 0 )
		{
			PlayerCount1 = GetPlayer()->GetGroup()->MemberCountOnline();
			PlayerCount2 = Inviter->GetGroup()->MemberCountOnline();
			if( PlayerCount1 < MinPlayerCount || PlayerCount2 < MinPlayerCount || PlayerCount1 != PlayerCount2 )
			{
				GetPlayer()->BroadcastMessage("Group size is wrong for this BG type. Need %d, have %d - %d", MinPlayerCount, PlayerCount1, PlayerCount2 );
				Inviter->BroadcastMessage("Group size is wrong for this BG type. Need %d, have %d - %d", MinPlayerCount, PlayerCount1, PlayerCount2 );
				TargetAccepted = false;
			}
		}
	}

	if( TargetAccepted == false )
	{
/*
{SERVER} Packet: (0xDB01) SMSG_GROUP_JOINED_BATTLEGROUND PacketSize = 19 TimeStamp = 57052030
4D 
E6 
40 12 00 00 00 00 02 7D 
39 D6 19 E8 4D FF FF FF FF 
*/
		Inviter->BroadcastMessage("%s rejected your wargame request",GetPlayer()->GetName());
		GetPlayer()->m_Wargames->OtherPartyLeader = 0;
		Inviter->m_Wargames->OtherPartyLeader = 0;
		//seems like this packet is almost the same all the time. No idea about content :(
		WorldPacket data( SMSG_GROUP_JOINED_BATTLEGROUND, 19 );
		data.CopyFromSnifferDump("4D E6 40 12 00 00 00 00 02 8D 39 D6 19 E8 5D FF FF FF FF");
		SendPacket( &data );
		//packet for the guy that initialized the request. Note that content varies a bit
		data.Initialize( SMSG_GROUP_JOINED_BATTLEGROUND );
		data.CopyFromSnifferDump("4D E6 40 12 00 00 00 00 02 7D 39 D6 19 E8 4D FF FF FF FF ");
		Inviter->GetSession()->SendPacket( &data );
	}
	//create a new BG or arena for players to fight in
	else
	{
//Inviter->m_Wargames->BGType = 2; //testing wsg
//		GetPlayer()->BroadcastMessage("Feature pending to be implemented");
//		Inviter->BroadcastMessage("Feature pending to be implemented");
		BattlemasterListEntry *bm = dbcBattlemasterListStore.LookupEntryForced( Inviter->m_Wargames->BGType );
		uint32 ValidMapIdCount = 0;
		while( bm->maps[ ValidMapIdCount ] > 0 && ValidMapIdCount < 7 )
			ValidMapIdCount++;
		ValidMapIdCount = MAX( ValidMapIdCount, 1 );
		uint32 MapId = bm->maps[ RandomUInt() % ValidMapIdCount ];
		if( MapId <= 0 || MapId == 607 || MapId == 628 )
		{
			GetPlayer()->BroadcastMessage("This map is not supported yet");
			Inviter->BroadcastMessage("This map is not supported yet");
			return;
		}
		int32 BGType = Inviter->m_Wargames->BGType;
		if( bm->instance_type == 4 ) //arena
		{
			if( PlayerCount1 == 2 && PlayerCount2 == 2 )
				BGType = BATTLEGROUND_ARENA_2V2;
			else if( PlayerCount1 == 3 && PlayerCount2 == 3 )
				BGType = BATTLEGROUND_ARENA_3V3;
			else 
				BGType = BATTLEGROUND_ARENA_5V5;
		}
		CBattleground *a = BattlegroundManager.CreateInstance( BGType, MAX_LEVEL_GROUP, MapId );
		if ( a == NULL )
		{
			GetPlayer()->BroadcastMessage("Failed to create instance");
			Inviter->BroadcastMessage("Failed to create instance");
			return;
		}

//			a->rated_match = false;
		a->m_SkipRewards = true;
		a->m_DenyNewJoins = true;
		a->m_TeamResurrects = Inviter->m_Wargames->TeamLives;
		a->m_PlayerResurrects = Inviter->m_Wargames->PlayerLives;

		//get player group
		a->AddGroupToTeam( Inviter->GetGroup(), 0 );
		a->AddGroupToTeam( GetPlayer()->GetGroup(), 1 );

		//allow queueing next time
		GetPlayer()->m_Wargames->OtherPartyLeader = 0;
		Inviter->m_Wargames->OtherPartyLeader = 0;
	}
}