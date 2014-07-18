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

void WorldSession::HandleInitiateTrade(WorldPacket & recv_data)
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
#ifdef FORCED_GM_TRAINEE_MODE
	if( CanUseCommand('k') && !HasGMPermissions() )
	{
		GetPlayer()->BroadcastMessage( "You are not allowed to use this feature" );
		return;
	}
#endif
	if( sWorld.getIntRate( INTRATE_DISABLE_TRADE_FEATURE ) )
	{
		GetPlayer()->BroadcastMessage( "Trade feature is disabled right now" );
		return;
	}
//	CHECK_PACKET_SIZE(recv_data, 8);
//	uint64 guid;
//	recv_data >> guid;
	uint8 GUID_mask;
	uint8 GUID_bytes[8];
	memset( GUID_bytes, 0, sizeof( GUID_bytes ) );

	recv_data >> GUID_mask;

	if( GUID_mask & BIT_0x01 )
		recv_data >> GUID_bytes[2];
	if( GUID_mask & BIT_0x02 )
		recv_data >> GUID_bytes[1];
	if( GUID_mask & BIT_0x40 )
		recv_data >> GUID_bytes[3];
	if( GUID_mask & BIT_0x04 )
		recv_data >> GUID_bytes[0];

	uint8 GUID_bytes_obfuscated[8];
	*(uint64*)GUID_bytes_obfuscated = *(uint64*)GUID_bytes;

	GUID_un_obfuscate( GUID_bytes );
	Player * pTarget = _player->GetMapMgr()->GetPlayer( *(uint64*)GUID_bytes );
	uint32 TradeStatus = TRADE_STATUS_PROPOSED;

	if(pTarget == 0)
	{
		TradeStatus = TRADE_STATUS_PLAYER_NOT_FOUND;
		OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
		return;
	}

	// Handle possible error outcomes
	if(pTarget->CalcDistance(_player) > 10.0f)		// This needs to be checked
		TradeStatus = TRADE_STATUS_TOO_FAR_AWAY;
	else if(pTarget->IsDead())
		TradeStatus = TRADE_STATUS_DEAD;
	else if(pTarget->mTradeTarget != 0)
		TradeStatus = TRADE_STATUS_ALREADY_TRADING;
	else if(pTarget->GetTeam() != _player->GetTeam() && GetPermissionCount() == 0 && !sWorld.interfaction_trade)
		TradeStatus = TRADE_STATUS_WRONG_FACTION;

	if(TradeStatus == TRADE_STATUS_PROPOSED)
	{
		_player->ResetTradeVariables();
		pTarget->ResetTradeVariables();

		pTarget->mTradeTarget = _player->GetLowGUID();
		_player->mTradeTarget = pTarget->GetLowGUID();

		pTarget->mTradeStatus = TradeStatus;
		_player->mTradeStatus = TradeStatus;
	}
/*
13329
ME
{CLIENT} Packet: (0x4106) CMSG_INITIATE_TRADE PacketSize = 8 TimeStamp = 32130418
8A A9 79 02 00 00 00 04 
to me
{SERVER} Packet: (0x2830) SMSG_TRADE_STATUS PacketSize = 34 TimeStamp = 32131338
0E 69 2B 33 18 8A A9 79 02 00 00 00 04 03 00 00 00 AB 2A 00 00 90 BC 61 58 AB 01 00 00 00 00 00 00 00 -> trade proposed
to him
{SERVER} Packet: (0x2830) SMSG_TRADE_STATUS PacketSize = 34 TimeStamp = 32209838
80 B9 46 42 98 8A A9 79 02 00 00 00 04 90 BC 61 58 AB 2A 00 00 B3 47 A5 00 00 01 00 00 00 AB 2A 00 00 -> trade proposed
him
{CLIENT} Packet: (0x4107) CMSG_BEGIN_TRADE PacketSize = 0 TimeStamp = 32209854

to me
{SERVER} Packet: (0x2830) SMSG_TRADE_STATUS PacketSize = 34 TimeStamp = 32131338
01 00 00 00 00 10 BD 46 42 00 00 00 00 88 E3 29 0E 00 00 00 00 07 01 00 00 00 02 00 00 00 AB 2A 00 00 -> trade initiated
to him
{SERVER} Packet: (0x2830) SMSG_TRADE_STATUS PacketSize = 34 TimeStamp = 32209994
00 00 00 00 AE 00 00 00 00 00 00 00 00 01 00 00 00 00 00 00 00 09 01 00 00 00 02 00 00 00 00 00 00 00 -> trade initiated 
to me
{SERVER} Packet: (0x2830) SMSG_TRADE_STATUS PacketSize = 34 TimeStamp = 32379442
80 B9 46 42 98 10 BD 46 42 00 00 00 00 00 00 00 00 AA 2A 00 00 10 0A 6C 58 AB 08 00 00 00 00 00 00 00 ->trade complete
to him
{SERVER} Packet: (0x2830) SMSG_TRADE_STATUS PacketSize = 34 TimeStamp = 32456725
05 00 00 00 0A 20 B8 46 42 00 00 00 00 00 00 00 00 00 00 00 00 01 00 00 00 00 04 00 00 00 00 00 00 00 ->trade accepted
to him
{SERVER} Packet: (0x2830) SMSG_TRADE_STATUS PacketSize = 34 TimeStamp = 32460750
80 B9 46 42 98 10 BD 46 42 00 00 00 00 00 00 00 00 AA 2A 00 00 10 0A 6C 58 AB 08 00 00 00 00 00 00 00 ->trade complete

14333
/////////////////////////////////////////
E9 D7 38 03 00 00 00 01 GUID OF A
07 E2 91 03 00 00 00 01 GUID OF B
A asks B to trade
{CLIENT} Packet: (0x3863) CMSG_INITIATE_TRADE PacketSize = 6 TimeStamp = 14190240
67 90 E3 02 00 06 
server sends back to A confirmation of the received invite
{SERVER} Packet: (0xDB61) SMSG_TRADE_STATUS PacketSize = 33 TimeStamp = 14190989
5D 00 00 01 00 00 00 00 00 00 00 90 02 00 00 00 00 00 00 06 80 7F 0D 41 E3 03 00 00 00 29 B4 5E 2D 
//maybe some auto click by client on some popup
{CLIENT} Packet: (0x98CA) UNKNOWN PacketSize = 0 TimeStamp = 14191005

probably the other client accepted the trade
{SERVER} Packet: (0xDB61) SMSG_TRADE_STATUS PacketSize = 31 TimeStamp = 14191301
49 00 00 02 00 00 00 AB 2A 00 00 DB 00 09 00 00 00 0A 40 80 0D 41 0A 20 84 0D 41 2C 31 E9 00 
/////////////////////////////////////////
B asks A to trade
{SERVER} Packet: (0xDB61) SMSG_TRADE_STATUS PacketSize = 33 TimeStamp = 14208430
5D 00 2A 01 00 00 00 AB 2A 00 00 90 02 00 00 00 00 00 01 06 30 7F 0D 41 E3 07 E2 91 03 30 95 A8 4A compressed guid of the inviter
B accepts trade
{CLIENT} Packet: (0x0869) UNKNOWN PacketSize = 0 TimeStamp = 14208430

server lets B know that he received the previous packet
{SERVER} Packet: (0xDB61) SMSG_TRADE_STATUS PacketSize = 32 TimeStamp = 14208851
59 00 01 02 00 00 00 00 00 00 00 16 00 00 0A 00 00 00 DD E0 85 0D 41 BC B0 7D 0D 41 67 00 00 00 
/////////////////////////////////////////
*/
/*
we receive invite
{SERVER} Packet: (0xDB61) SMSG_TRADE_STATUS PacketSize = 33 TimeStamp = 14208430
5D 00 mask - 5 bits
2A junk
01 00 00 00 status ?
AB 2A 00 00 famous contant value or 0
90 - go2 B we are asking this guy to accept our trade
02 - go3 B we are asking this guy to accept our trade
00 00 00 00 00 00 - junk ?
06 - go0 B we are asking this guy to accept our trade
80 7F 0D 41 - magic float ? 
E3 - go1 B we are asking this guy to accept our trade
07 E2 91 03 lowguid of the other guy junk ?
30 95 A8 4A serializer ?
*/
	//right now trade initiator does not need this packet
/*	{
		sStackWorldPacket( data, SMSG_TRADE_STATUS, 44 + 10 );
		data << uint16( 0x59 ); // mask bit_2 might be go0, bit_0 might be go7
		data << uint8( 0 ); // junk
//		data << ObfuscateByte( GUID_bytes[6] );
		data << uint32( TRADE_STATUS_PROPOSED );	
		data << uint32( TRADE_ID_CONST ); //used a lot of times or 0
		data << ObfuscateByte( GUID_bytes[2] );
		data << ObfuscateByte( GUID_bytes[3] );
		data << uint8( 0 ); // junk
		data << uint32( 0x00000000 ); // highguid or junk
//		data << ObfuscateByte( GUID_bytes[7] );
		data << ObfuscateByte( GUID_bytes[0] );
		data << float( 0 ); // junk
		data << ObfuscateByte( GUID_bytes[1] );
		data << uint32( pTarget->GetLowGUID() ); // junk
		data << uint32( 0x00000000 ); // could be trade ID. Like A asks B, B refuses, A received back this number
		SendPacket( &data );
	}/**/
//	_player->SendTradeUpdate( 0 );
	//trade target does need this packet so he can auto reply with CMSG_BEGIN_TRADE
	{
		*(uint64*)GUID_bytes = _player->GetGUID();
		sStackWorldPacket( data, SMSG_TRADE_STATUS, 44 + 10 );
		data << uint16( 0x59 ); // mask bit_2 might be go0, bit_0 might be go7
		data << uint8( 0 ); // junk, can send 0 also
//		data << ObfuscateByte( GUID_bytes[6] );
		data << uint32( TRADE_STATUS_PROPOSED );	
		data << uint32( TRADE_ID_CONST ); //used a lot of times or 0, 
		data << ObfuscateByte( GUID_bytes[2] );
		data << ObfuscateByte( GUID_bytes[3] );
		data << uint8( 0 ); // junk
		data << uint32( 0x00000000 ); // highguid ? trade session counter ? junk ?
//		data << ObfuscateByte( GUID_bytes[7] );
		data << ObfuscateByte( GUID_bytes[0] );
		data << float( 0 ); // junk seems to be some float sometimes 
		data << ObfuscateByte( GUID_bytes[1] );
		data << uint32( _player->GetLowGUID() ); // lowguid ? junk ?
		data << uint32( 0 ); // could be trade ID. Like A asks B, B refuses, A received back this number. Or it could be time, global packet serializer
		pTarget->m_session->SendPacket( &data );
	}/**/
//	pTarget->SendTradeUpdate( 0 );
}


void WorldSession::HandleInitiateTradeACK(WorldPacket & recv_data)
{
}

void WorldSession::HandleBeginTrade(WorldPacket & recv_data)
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
#ifdef FORCED_GM_TRAINEE_MODE
	if( CanUseCommand('k') && !HasGMPermissions() )
	{
		GetPlayer()->BroadcastMessage( "You are not allowed to use this feature" );
		return;
	}
#endif
	if( sWorld.getIntRate( INTRATE_DISABLE_TRADE_FEATURE ) )
	{
		GetPlayer()->BroadcastMessage( "Trade feature is disabled right now" );
		return;
	}
	sLog.outDebug( "WORLD: got CMSG_BEGIN_TRADE from player %s.", _player->GetName() );
	packetSMSG_TRADE_STATUS data;
	memset( &data, 0, sizeof( data ) );
	data.trade_status = TRADE_STATUS_INITIATED;
	data.trade_id = TRADE_ID_CONST;	//this the constant used in status update also !

	Player * plr = _player->GetTradeTarget();
	if(_player->mTradeTarget == 0 || plr == 0)
	{
		data.trade_status = TRADE_STATUS_PLAYER_NOT_FOUND;
		OutPacket(SMSG_TRADE_STATUS,sizeof( packetSMSG_TRADE_STATUS ), &data);
		return;
	}
	// We're too far from target now?
	Player * plr2 = objmgr.GetPlayer(_player->mTradeTarget);
	if( plr2 == NULL )
	{
		return; //wtf ? player is inside the map and not in objectmanager ?
	}
	if( _player->CalcDistance( plr2 ) > 10.0f )
		data.trade_status = TRADE_STATUS_TOO_FAR_AWAY;

	//send to ourself
	OutPacket(SMSG_TRADE_STATUS,sizeof( packetSMSG_TRADE_STATUS ), &data);
	//send to our target
	plr->m_session->OutPacket(SMSG_TRADE_STATUS,sizeof( packetSMSG_TRADE_STATUS ), &data);

	plr->mTradeStatus = data.trade_status;
	_player->mTradeStatus = data.trade_status;

//	_player->SendTradeUpdate( 0 );
//	plr->SendTradeUpdate( 0 );
}
/*
void WorldSession::HandleBusyTrade(WorldPacket & recv_data)
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	uint32 TradeStatus = TRADE_STATUS_PLAYER_BUSY;

	Player * plr = _player->GetTradeTarget();
	if(_player->mTradeTarget == 0 || plr == 0)
	{
		TradeStatus = TRADE_STATUS_PLAYER_NOT_FOUND;
//		OutPacket(TRADE_STATUS_PLAYER_NOT_FOUND, 4, &TradeStatus);
		return;
	}

	OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
	plr->m_session->OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);

	plr->mTradeStatus = TradeStatus;
	_player->mTradeStatus = TradeStatus;

	plr->ResetTradeVariables();
	_player->ResetTradeVariables();
}*/

void WorldSession::HandleIgnoreTrade(WorldPacket & recv_data)
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	packetSMSG_TRADE_STATUS data;
	memset( &data, 0, sizeof( data ) );
	data.trade_status = TRADE_STATUS_PLAYER_IGNORED;

	Player * plr = _player->GetTradeTarget();
	if(_player->mTradeTarget == 0 || plr == 0)
	{
		data.trade_status = TRADE_STATUS_PLAYER_NOT_FOUND;
		OutPacket(SMSG_TRADE_STATUS,sizeof( packetSMSG_TRADE_STATUS ), &data);
		return;
	}

	//send to ourself
	OutPacket(SMSG_TRADE_STATUS,sizeof( packetSMSG_TRADE_STATUS ), &data);
	//send to our target
	plr->m_session->OutPacket(SMSG_TRADE_STATUS,sizeof( packetSMSG_TRADE_STATUS ), &data);

	plr->mTradeStatus = data.trade_status;
	_player->mTradeStatus = data.trade_status;

	plr->ResetTradeVariables();
	_player->ResetTradeVariables();
}

void WorldSession::HandleCancelTrade(WorldPacket & recv_data)
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}

	sLog.outDebug( "WORLD: got CMSG_CANCEL_TRADE from player %s.", _player->GetName() );
	if(_player->mTradeTarget == 0 || _player->mTradeStatus == TRADE_STATUS_COMPLETE)
	{ 
		return;
	}

	packetSMSG_TRADE_STATUS data;
	memset( &data, 0, sizeof( data ) );
	data.trade_status = TRADE_STATUS_CANCELLED;

	//send to ourself
	OutPacket(SMSG_TRADE_STATUS,sizeof( packetSMSG_TRADE_STATUS ), &data);

	Player * plr = _player->GetTradeTarget();
    if(plr)
    {
		//send to our target
        if(plr->m_session && plr->m_session->GetSocket())
			plr->m_session->OutPacket(SMSG_TRADE_STATUS,sizeof( packetSMSG_TRADE_STATUS ), &data);
	
		plr->ResetTradeVariables();
    }
	
	_player->ResetTradeVariables();
}

void WorldSession::HandleUnacceptTrade(WorldPacket & recv_data)
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	Player * plr = _player->GetTradeTarget();
	//_player->ResetTradeVariables();

	if(_player->mTradeTarget == 0 || plr == 0)
	{ 
		return;
	}

	packetSMSG_TRADE_STATUS data;
	memset( &data, 0, sizeof( data ) );
	data.trade_status = TRADE_STATUS_UNACCEPTED;

	//send to ourself
	OutPacket(SMSG_TRADE_STATUS,sizeof( packetSMSG_TRADE_STATUS ), &data);
	plr->m_session->OutPacket(SMSG_TRADE_STATUS,sizeof( packetSMSG_TRADE_STATUS ), &data);

	data.trade_status = TRADE_STATUS_STATE_CHANGED;

	OutPacket(SMSG_TRADE_STATUS,sizeof( packetSMSG_TRADE_STATUS ), &data);
	plr->m_session->OutPacket(SMSG_TRADE_STATUS,sizeof( packetSMSG_TRADE_STATUS ), &data);

	plr->mTradeStatus = data.trade_status;
	_player->mTradeStatus = data.trade_status;

  //cebernic,why target & self erased? we are not cancelled !
  //this merged from p2wow's svn.
	//plr->mTradeTarget = 0;
	//_player->mTradeTarget = 0;

/*	plr->mTradeTarget = 0;
	_player->mTradeTarget = 0;
	plr->ResetTradeVariables(); */
}

void WorldSession::HandleSetTradeItem(WorldPacket & recv_data)
{
#ifdef FORCED_GM_TRAINEE_MODE
	if( CanUseCommand('k') && !HasGMPermissions() )
	{
		GetPlayer()->BroadcastMessage( "You are not allowed to use this feature" );
		return;
	}
#endif
	if(_player->mTradeTarget == 0)
	{ 
		sLog.outDebug("HandleSetTradeItem: missing trade target\n");
		return;
	}

	if(!_player->IsInWorld()) 
	{ 
		sLog.outDebug("HandleSetTradeItem: not in world\n");
		return;
	}
	  
	CHECK_PACKET_SIZE(recv_data, 3);

	uint8 TradeSlot = recv_data.contents()[1];
	uint8 SourceSlot = recv_data.contents()[2];
	uint8 SourceBag = recv_data.contents()[0];
	Player * pTarget = _player->GetTradeTarget();

	Item * pItem = _player->GetItemInterface()->GetInventoryItem(SourceBag, SourceSlot);

	if( pTarget == NULL || pItem == 0 || TradeSlot >= TRADE_TOTAL_TRADE_SLOTS || ( TradeSlot < TRADE_MAX_TRADABLE_ITEMS && pItem->IsSoulbound() ) )
	{ 
		sLog.outDebug("HandleSetTradeItem: target missing or incorect item/slot. target(%u),item(%u),slot(%u))\n",pTarget != NULL,pItem != 0,TradeSlot );
 		return;
	}

	if( TradeSlot < TRADE_MAX_TRADABLE_ITEMS && pItem->IsAccountbound() )
	{
		PlayerInfo* pinfo = ObjectMgr::getSingleton().GetPlayerInfo(_player->mTradeTarget);
		if(pinfo == NULL || GetAccountId() != pinfo->acct) // can't trade account-based items
		{ 
			sLog.outDebug("HandleSetTradeItem: item is account bound\n");
			return;
		}
	}
/*	if( pItem->IsContainer() )
	{
		if(_player->GetItemInterface()->IsBagSlot(SourceSlot))
		return;*/
		
	packetSMSG_TRADE_STATUS data;
	memset( &data, 0, sizeof( data ) );
//	data.trade_status = TRADE_STATUS_STATE_CHANGED;
//	OutPacket(SMSG_TRADE_STATUS,sizeof( packetSMSG_TRADE_STATUS ), &data);
//	pTarget->m_session->OutPacket(SMSG_TRADE_STATUS,sizeof( packetSMSG_TRADE_STATUS ), &data);

	pTarget->mTradeStatus = TRADE_STATUS_STATE_CHANGED;
	_player->mTradeStatus = TRADE_STATUS_STATE_CHANGED;


	if( pItem->IsContainer() )
	{
		if( SafeContainerCast(pItem)->HasItems() )
		{
			_player->GetItemInterface()->BuildInventoryChangeError(pItem,0, INV_ERR_CAN_ONLY_DO_WITH_EMPTY_BAGS);

			//--trade cancel
			data.trade_status = TRADE_STATUS_CANCELLED;
			OutPacket(SMSG_TRADE_STATUS,sizeof( packetSMSG_TRADE_STATUS ), &data);
			
			Player * plr = _player->GetTradeTarget();
			if(plr)
			{
			    if(plr->m_session && plr->m_session->GetSocket())
					plr->m_session->OutPacket(SMSG_TRADE_STATUS,sizeof( packetSMSG_TRADE_STATUS ), &data);
			    plr->ResetTradeVariables();
			}

			_player->ResetTradeVariables();
			sLog.outDebug("HandleSetTradeItem: Container with items cannot be traded\n");
			return;
		}
	}			
		
	//well that covers all slots i think (MAX_TRADE_TRADABLE_ITEMS is temporal slot)
	if(TradeSlot < TRADE_MAX_TRADABLE_ITEMS)
	{
		if(pItem->IsSoulbound())
		{
			sCheatLog.writefromsession(this, "tried to cheat trade a soulbound item");
			sLog.outDebug("HandleSetTradeItem: Cannot trade soulbound item\n");
			Item *ti = _player->GetItemInterface()->SafeRemoveAndRetreiveItemFromSlot(SourceBag, SourceSlot, false);
			if( ti )
			{
				ti->DeleteFromDB();
				ti->DeleteMe();
				ti = NULL;
			}
			string sReason = "Soulboundtrade 1";
			uint32 uBanTime = (uint32)UNIXTIME + 60*60; //60 minutes ban
			_player->SetBanned( uBanTime, sReason );
			sEventMgr.AddEvent( _player, &Player::_Kick, EVENT_PLAYER_KICK, 7000, 1, 0 );
			return; //item is not valid anymore !
		}
		//this was checked earlier ? How the hack did we get here again ?
		else if(pItem->IsAccountbound() && pTarget->GetSession() && pTarget->GetSession()->GetAccountId() != GetAccountId() )
		{
			PlayerInfo* pinfo = ObjectMgr::getSingleton().GetPlayerInfo(_player->mTradeTarget);
			if(pinfo == NULL || GetAccountId() != pinfo->acct) // can't trade account-based items
			{
				sLog.outDebug("HandleSetTradeItem: Cannot trade account bound item\n");
				sCheatLog.writefromsession(this, "tried to cheat trade an accountbound item");
				Item *ti = _player->GetItemInterface()->SafeRemoveAndRetreiveItemFromSlot(SourceBag, SourceSlot, false);
				if( ti )
				{
					ti->DeleteFromDB();
					ti->DeleteMe();
					ti = NULL;
				}
				string sReason = "Account bound itemtrade";
				uint32 uBanTime = (uint32)UNIXTIME + 60*60; //60 minutes ban
				_player->SetBanned( uBanTime, sReason );
				sEventMgr.AddEvent( _player, &Player::_Kick, EVENT_PLAYER_KICK, 7000, 1, 0 );
				return; //item is not valid anymore !
			}
		}
	}

	for(uint32 i = 0; i < TRADE_TOTAL_TRADE_SLOTS; ++i)
	{
		// duping little shits
		if( TradeSlot != i && ( _player->mTradeItems[i] == pItem || pTarget->mTradeItems[i] == pItem ) )
		{
			sCheatLog.writefromsession(this, "tried to dupe an item through trade");
			Item *ti = _player->GetItemInterface()->SafeRemoveAndRetreiveItemFromSlot(SourceBag, SourceSlot, false);
			if( ti )
			{
				ti->DeleteFromDB();
				ti->DeleteMe();
				ti = NULL;
			}
			GetPlayer()->SoftDisconnect();
			sLog.outDebug("HandleSetTradeItem: item dupe detected\n");
			return;
		}
	}

	if( SourceBag <= INVENTORY_SLOT_NOT_SET && //we are removing from our direct character slot and not from a bag
		SourceSlot >= INVENTORY_SLOT_BAG_START && SourceSlot < INVENTORY_SLOT_BAG_END)
	{
		//More duping woohoo
        Item *ti = _player->GetItemInterface()->SafeRemoveAndRetreiveItemFromSlot(SourceBag, SourceSlot, false);
		if( ti )
		{
			ti->DeleteFromDB();
			ti->DeleteMe();
			ti = NULL;
		}
		sCheatLog.writefromsession(this, "tried to cheat trade a soulbound item");
		string sReason = "trading from bagslot";
		uint32 uBanTime = (uint32)UNIXTIME + 60*60; //60 minutes ban
		_player->SetBanned( uBanTime, sReason );
		sEventMgr.AddEvent( _player, &Player::_Kick, EVENT_PLAYER_KICK, 7000, 1, 0 );
		sLog.outDebug("HandleSetTradeItem: Cannot trade from bagslot\n");
		return; //item is not valid anymore !
	}

	_player->mTradeItems[TradeSlot] = pItem;
	_player->SendTradeUpdate();
}

void WorldSession::HandleSetTradeGold(WorldPacket & recv_data)
{
#ifdef FORCED_GM_TRAINEE_MODE
	if( CanUseCommand('k') && !HasGMPermissions() )
	{
		GetPlayer()->BroadcastMessage( "You are not allowed to use this feature" );
		return;
	}
#endif
	if(_player->mTradeTarget == 0)
	{ 
		return;
	}
  // cebernic: TradeGold sameway.
	Player * plr = _player->GetTradeTarget();
	if(!plr) 
	{ 
		return;
	}

//	packetSMSG_TRADE_STATUS data;
//	memset( &data, 0, sizeof( data ) );
//	data.trade_status = TRADE_STATUS_STATE_CHANGED;
//	OutPacket(SMSG_TRADE_STATUS,sizeof( packetSMSG_TRADE_STATUS ), &data);
//	plr->m_session->OutPacket(SMSG_TRADE_STATUS,sizeof( packetSMSG_TRADE_STATUS ), &data);

	plr->mTradeStatus = TRADE_STATUS_STATE_CHANGED;
	_player->mTradeStatus = TRADE_STATUS_STATE_CHANGED;


	uint64 Gold;
	recv_data >> Gold;

	if(_player->mTradeGold != Gold)
	{
		_player->mTradeGold = (Gold > _player->GetGold() ? _player->GetGold() : Gold);
		_player->SendTradeUpdate( );
	}
}

void WorldSession::HandleClearTradeItem(WorldPacket & recv_data)
{
	CHECK_PACKET_SIZE(recv_data, 1);
	if(_player->mTradeTarget == 0)
	{ 
		return;
	}

	uint8 TradeSlot = recv_data.contents()[0];
	if( TradeSlot >= TRADE_TOTAL_TRADE_SLOTS )
	{ 
		return;
	}

  // clean status
	Player * plr = _player->GetTradeTarget();
	if ( !plr ) 
	{ 
		return;
	}
//	packetSMSG_TRADE_STATUS data;
//	memset( &data, 0, sizeof( data ) );
//	data.trade_status = TRADE_STATUS_STATE_CHANGED;
//	OutPacket(SMSG_TRADE_STATUS,sizeof( packetSMSG_TRADE_STATUS ), &data);
//	plr->m_session->OutPacket(SMSG_TRADE_STATUS,sizeof( packetSMSG_TRADE_STATUS ), &data);

	plr->mTradeStatus = TRADE_STATUS_STATE_CHANGED;
	_player->mTradeStatus = TRADE_STATUS_STATE_CHANGED;


	_player->mTradeItems[TradeSlot] = 0;
	_player->SendTradeUpdate();
}

void WorldSession::HandleAcceptTrade(WorldPacket & recv_data) 
{
#ifdef FORCED_GM_TRAINEE_MODE
	if( CanUseCommand('k') && !HasGMPermissions() )
	{
		GetPlayer()->BroadcastMessage( "You are not allowed to use this feature" );
		return;
	}
#endif
	if( sWorld.getIntRate( INTRATE_DISABLE_TRADE_FEATURE ) )
	{
		GetPlayer()->BroadcastMessage( "Trade feature is disabled right now" );
		return;
	}
	Player * plr = _player->GetTradeTarget();
	if(_player->mTradeTarget == 0 || !plr)
	{ 
		return;
	}

	packetSMSG_TRADE_STATUS data;
	memset( &data, 0, sizeof( data ) );
	data.trade_status = TRADE_STATUS_ACCEPTED;
	OutPacket(SMSG_TRADE_STATUS,sizeof( packetSMSG_TRADE_STATUS ), &data);
	plr->m_session->OutPacket(SMSG_TRADE_STATUS,sizeof( packetSMSG_TRADE_STATUS ), &data);
	
	_player->mTradeStatus = TRADE_STATUS_ACCEPTED;	//when both have this status we can close the deal

	bool bag_hack_detected = false;
	if(plr->mTradeStatus == TRADE_STATUS_ACCEPTED)
	{
		// Ready!
		uint32 ItemCount = 0;
		uint32 TargetItemCount = 0;
		Player * pTarget = plr;

/*		// Calculate Item Count
		for(uint32 Index = 0; Index < 7; ++Index)
		{
			if(_player->mTradeItems[Index] != 0)	++ItemCount;
			if(pTarget->mTradeItems[Index] != 0)	++TargetItemCount;
		}*/
		

		// Calculate Count
		for(uint32 Index = 0; Index < TRADE_MAX_TRADABLE_ITEMS; ++Index) // cebernic: checking for MAX_TRADE_TRADABLE_ITEMS items ,untradable item check via others func.
		{
			Item * pItem;

			// safely trade checking
			pItem = _player->mTradeItems[Index];
			if( pItem )
			{
				if( ( pItem->IsContainer() && SafeContainerCast(pItem)->HasItems() )   || ( pItem->GetProto() && pItem->GetProto()->Bonding==ITEM_BIND_ON_PICKUP) )
				{
					ItemCount = 0;
					TargetItemCount = 0;
					bag_hack_detected = true;
					break;
				}
				else ++ItemCount;
			}					
			
			pItem = pTarget->mTradeItems[Index];
			if( pItem )
			{
				if( ( pItem->IsContainer() && SafeContainerCast(pItem)->HasItems() )   || ( pItem->GetProto() && pItem->GetProto()->Bonding==ITEM_BIND_ON_PICKUP) )
				{
					ItemCount = 0;
					TargetItemCount = 0;
					bag_hack_detected = true;
					break;
				}
				else 
					++TargetItemCount;
			}					

			//if(_player->mTradeItems[Index] != 0)	++ItemCount;
			//if(pTarget->mTradeItems[Index] != 0)	++TargetItemCount;
		}

		

		if( (_player->m_ItemInterface->CalculateFreeSlots(NULL) + ItemCount) < TargetItemCount ||
			(pTarget->m_ItemInterface->CalculateFreeSlots(NULL) + TargetItemCount) < ItemCount ||
			bag_hack_detected == true ||
			(ItemCount==0 && TargetItemCount==0 && !pTarget->mTradeGold && !_player->mTradeGold) )	// ceberwow added it
		{
			// Not enough slots on one end.
			data.trade_status = TRADE_STATUS_CANCELLED;
		}
		else
		{
			data.trade_status = TRADE_STATUS_COMPLETE;
			uint64 Guid;
			Item * pItem;
			
			// Remove all items from the players inventory
			for(uint32 Index = 0; Index < TRADE_MAX_TRADABLE_ITEMS; ++Index)
			{
				Guid = _player->mTradeItems[Index] ? _player->mTradeItems[Index]->GetGUID() : 0;
				if(Guid != 0)
				{
					if( _player->mTradeItems[Index]->GetProto()->Bonding == ITEM_BIND_ON_PICKUP ||
						_player->mTradeItems[Index]->GetProto()->Bonding  >=  ITEM_BIND_QUEST  )
					{
						_player->mTradeItems[Index] = NULL;
					}
					else
					{
						if(GetPermissionCount()>0)
							sGMLog.writefromsession(this, "traded item %s to %s", _player->mTradeItems[Index]->GetProto()->Name1, pTarget->GetName());
						pItem = _player->m_ItemInterface->SafeRemoveAndRetreiveItemByGuid(Guid, true);
					}
				}

				Guid = pTarget->mTradeItems[Index] ? pTarget->mTradeItems[Index]->GetGUID() : 0;
				if(Guid != 0)
				{
					if( pTarget->mTradeItems[Index]->GetProto()->Bonding == ITEM_BIND_ON_PICKUP ||  
						pTarget->mTradeItems[Index]->GetProto()->Bonding  >=  ITEM_BIND_QUEST )
					{
						pTarget->mTradeItems[Index] = NULL;
					}
					else
					{
						if(pTarget->GetSession() && pTarget->GetSession()->GetPermissionCount()>0)
							sGMLog.writefromsession(pTarget->GetSession(), "traded item %s to %s", pTarget->mTradeItems[Index]->GetProto()->Name1, _player->GetName());
						pTarget->m_ItemInterface->SafeRemoveAndRetreiveItemByGuid(Guid, true);
					}
				}
			}

			// Dump all items back into the opposite players inventory
			for(uint32 Index = 0; Index < TRADE_MAX_TRADABLE_ITEMS; ++Index)
			{
				pItem = _player->mTradeItems[Index];
				if(pItem != 0 && pTarget)
				{
					pItem->SetOwner(pTarget); // crash fixed.
					if( !pTarget->m_ItemInterface->AddItemToFreeSlot(&pItem) )
					{
						pItem->DeleteMe();
						pItem = NULL;
						_player->mTradeItems[Index] = NULL;
						continue;
					}
				}

				pItem = pTarget->mTradeItems[Index];
				if(pItem != 0 && _player)
				{
					pItem->SetOwner(_player);
					if( !_player->m_ItemInterface->AddItemToFreeSlot(&pItem) )
					{
						pItem->DeleteMe();
						pItem = NULL;
						pTarget->mTradeItems[Index] = NULL;
						continue;
					}
				}
			}

			// Trade Gold
			if(pTarget->mTradeGold && pTarget->GetGold() >= pTarget->mTradeGold )
			{
				_player->ModGold(pTarget->mTradeGold);
				pTarget->ModGold(-(int64)pTarget->mTradeGold);
				if(GetPermissionCount()>0)
					sGMLog.writefromsession(this, "traded(gave) %u gold to %s", pTarget->mTradeGold, pTarget->GetName());
			}

			if(_player->mTradeGold && _player->GetGold() >= _player->mTradeGold)
			{
				pTarget->ModGold( _player->mTradeGold);
				_player->ModGold( -(int64)_player->mTradeGold);
				if(pTarget->GetSession() && pTarget->GetSession()->GetPermissionCount()>0)
					sGMLog.writefromsession(pTarget->GetSession(), "traded(gave) %u gold to %s", _player->mTradeGold, _player->GetName());
			}

			// Close Window
			data.trade_status = TRADE_STATUS_COMPLETE;
			
		}

		OutPacket(SMSG_TRADE_STATUS,sizeof( packetSMSG_TRADE_STATUS ), &data);
		plr->m_session->OutPacket(SMSG_TRADE_STATUS,sizeof( packetSMSG_TRADE_STATUS ), &data);

		_player->mTradeStatus = TRADE_STATUS_COMPLETE;
		plr->mTradeStatus = TRADE_STATUS_COMPLETE;

		// Reset Trade Vars
		_player->ResetTradeVariables();		
		plr->ResetTradeVariables();

		//removed by zack. Hehe, just a way to spam db with saves. Not funny
		// added alternative way as item saves before 
		// Save for eachother
		//plr->SaveToDB(false);
		//_player->SaveToDB(false);
	}
}
