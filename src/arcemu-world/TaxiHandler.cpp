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

void WorldSession::HandleTaxiEnableOpcode( WorldPacket & recv_data )
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	sLog.outDebug( "WORLD: Received CMSG_ENABLETAXI" );

	uint64 guid;
	uint32 curloc;
	uint8 field;
	uint32 submask;

	recv_data >> guid;

	curloc = sTaxiMgr.GetNearestTaxiNode( GetPlayer( )->GetPositionX( ), GetPlayer( )->GetPositionY( ), GetPlayer( )->GetPositionZ( ), GetPlayer( )->GetMapId( ) );

	field = (uint8)((curloc - 1) / 32);
	submask = 1<<((curloc-1)%32);

	GetPlayer()->SetTaximask(field, (submask | GetPlayer( )->GetTaximask(field)) );

	OutPacket(SMSG_NEW_TAXI_PATH);

	WorldPacket data(9);
	data.Initialize( SMSG_TAXINODE_STATUS );
	data << guid;
	data << uint8( 1 );
	SendPacket( &data );
	sLog.outDebug( "WORLD: Sent SMSG_TAXINODE_STATUS" );
}

void WorldSession::HandleTaxiNodeStatusQueryOpcode( WorldPacket & recv_data )
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	sLog.outDebug( "WORLD: Received CMSG_TAXINODE_STATUS_QUERY" );

	uint64 guid;
	uint32 curloc;
	uint8 field;
	uint32 submask;

	recv_data >> guid;

	curloc = sTaxiMgr.GetNearestTaxiNode( GetPlayer( )->GetPositionX( ), GetPlayer( )->GetPositionY( ), GetPlayer( )->GetPositionZ( ), GetPlayer( )->GetMapId( ) );

	field = (uint8)((curloc - 1) / 32);
	submask = 1<<((curloc-1)%32);

	WorldPacket data(9);
	data.Initialize( SMSG_TAXINODE_STATUS );
	data << guid;

	// Check for known nodes
	if ( curloc !=0 && (GetPlayer( )->GetTaximask(field) & submask) != submask )
	{   
		data << uint8( 0 );	
	}
	else
	{
		data << uint8( 1 );
	}	

	SendPacket( &data );
	sLog.outDebug( "WORLD: Sent SMSG_TAXINODE_STATUS" );
}


void WorldSession::HandleTaxiQueryAvaibleNodesOpcode( WorldPacket & recv_data )
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	sLog.outDebug( "WORLD: Received CMSG_TAXIQUERYAVAILABLENODES" );
	uint64 guid;
	recv_data >> guid;
	Creature *pCreature = _player->GetMapMgr()->GetCreature( guid );
	if(!pCreature) 
	{ 
		return;
	}

	SendTaxiList(pCreature);
}

void WorldSession::SendTaxiList(Creature* pCreature)
{
	uint32 curloc;
	uint8 field;
	uint32 TaxiMask[TAXIMASK_SIZE];
	uint32 submask;
	uint64 guid = pCreature->GetGUID();

	curloc = pCreature->m_TaxiNode;
	if (!curloc)
	{ 
		return;
	}

	field = (uint8)((curloc - 1) / 32);
	submask = 1<<((curloc-1)%32);

	if( field >= TAXIMASK_SIZE )
	{
		ASSERT( false );
		return;
	}

	// Check for known nodes
	if (!(GetPlayer( )->GetTaximask(field) & submask))
	{
		GetPlayer()->SetTaximask(field, (submask | GetPlayer( )->GetTaximask(field)) );

		OutPacket(SMSG_NEW_TAXI_PATH);

		//Send packet
		WorldPacket update(SMSG_TAXINODE_STATUS, 9);
		update << guid << uint8( 1 );
		SendPacket( &update );
	}

	//Set Mask
	memset(TaxiMask, 0, sizeof(uint32)*TAXIMASK_SIZE);
	sTaxiMgr.GetGlobalTaxiNodeMask(curloc, TaxiMask);
	if( field < TAXIMASK_SIZE )
		TaxiMask[field] |= submask;

	//Remove nodes unknown to player
	for(int i = 0; i < TAXIMASK_SIZE; i++)
	{
		TaxiMask[i] &= GetPlayer( )->GetTaximask(i);
	}

/*
13329
{SERVER} Packet: (0xD654) SMSG_SHOWTAXINODES PacketSize = 97 TimeStamp = 8319595
01 00 00 00 
95 10 00 00 E7 A7 30 F1 
46 02 00 00 - 582 - goldshire - we should have byte 72, bit 6 set to 1
0A 
02 00 00 00 00 00 00 00 00 00 00 00 08 00 00 80 01 00 00 00 00 00 40 00 00 00 00 00 00 00 10 00 00 00 00 00 00 00 20 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 20 00 00 00 00 00 00 00 

14333
{SERVER} Packet: (0xAA7F) UNKNOWN PacketSize = 98 TimeStamp = 9522472
01 00 00 00 
78 13 00 00 E7 A7 30 F1 
46 02 00 00 
4E 00 00 00 = 78 
22 00 00 06 00 00 01 00 00 00 00 20 00 00 00 00 00 00 00 00 00 00 40 00 00 00 00 00 00 00 10 00 00 00 00 00 00 00 20 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 80 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 20 10 00 00 00 8C 
*/
	WorldPacket data( SMSG_SHOWTAXINODES, 1 + 8 + TAXIMASK_SIZE * 4 );
	data << uint32( 1 ) << guid;
//	data.CopyFromSnifferDump("46 02 00 00 4E 00 00 00 22 00 00 06 00 00 01 00 00 00 00 20 00 00 00 00 00 00 00 00 00 00 40 00 00 00 00 00 00 00 10 00 00 00 00 00 00 00 20 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 80 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 20 10 00 00 00 8C ");
	data << uint32( curloc );
	data << uint32( TAXIMASK_SIZE * 4 );	//bytecount
	for(int i = 0; i < TAXIMASK_SIZE; i++)
		data << TaxiMask[i];
	SendPacket( &data );

	sLog.outDebug( "WORLD: Sent SMSG_SHOWTAXINODES" );
}

void WorldSession::HandleActivateTaxiOpcode( WorldPacket & recv_data )
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	sLog.outDebug( "WORLD: Received CMSG_ACTIVATETAXI" );

	uint64 guid;
	uint32 sourcenode, destinationnode;
	int64 newmoney;
	uint32 curloc;
	uint8 field;
	uint32 submask;
	WorldPacket data(SMSG_ACTIVATETAXIREPLY, 4);

	recv_data >> guid >> sourcenode >> destinationnode;

	if(GetPlayer()->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_LOCK_PLAYER))
	{ 
		return;
	}

	TaxiPath* taxipath = sTaxiMgr.GetTaxiPath(sourcenode, destinationnode);
	TaxiNode* taxinode = sTaxiMgr.GetTaxiNode(sourcenode);

	if( !taxinode || !taxipath )
	{ 
		return;
	}

	curloc = taxinode->id;
	field = (uint8)((curloc - 1) / 32);
	submask = 1<<((curloc-1)%32);

	// Check for known nodes
	if ( (GetPlayer( )->GetTaximask(field) & submask) != submask )
	{   
		data << uint32( 1 );
		SendPacket( &data );
		return;
	}

	// Check for valid node
	if (!taxinode)
	{
		data << uint32( 1 );
		SendPacket( &data );
		return;
	}

	if (!taxipath || !taxipath->GetNodeCount())
	{
		data << uint32( 2 );
		SendPacket( &data );
		return;
	}

	// Check for gold
	newmoney = ((GetPlayer()->GetGold()) - taxipath->GetPrice());
	if(newmoney < 0 )
	{
		data << uint32( 3 );
		SendPacket( &data );
		return;
	}
	GetPlayer()->Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_GOLD_SPENT_FOR_TRAVELLING,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,taxipath->GetPrice(),ACHIEVEMENT_EVENT_ACTION_ADD);
	GetPlayer()->Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_FLIGHT_PATHS_TAKEN,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);

	// MOUNTDISPLAYID
	// bat: 1566
	// gryph: 1147
	// wyvern: 295
	// hippogryph: 479
	// fer0x: Incorrect system. Need take values from TaxiNodes.dbc

	uint32 modelid =0;
	if( _player->GetTeam() )
	{
		/*
		if( taxinode->horde_mount == 2224 )
			modelid =295; // In case it's a wyvern
		else
			modelid =1566; // In case it's a bat or a bad id
		*/

		CreatureInfo* ci = CreatureNameStorage.LookupEntry( taxinode->horde_mount );
		if(!ci) 
		{ 
			return;
		}
		modelid = ci->Male_DisplayID;
		if(!modelid) 
		{ 
			return;
		}
	}
	else
	{
		/*
		if( taxinode->alliance_mount == 3837 )
			modelid =479; // In case it's an hippogryph
		else
			modelid =1147; // In case it's a gryphon or a bad id
		*/

		CreatureInfo* ci = CreatureNameStorage.LookupEntry( taxinode->alliance_mount );
		if(!ci) 
		{ 
			return;
		}
		modelid = ci->Male_DisplayID;
		if(!modelid) 
		{ 
			return;
		}
	}

	//GetPlayer( )->setDismountCost( newmoney );

	data << uint32( 0 );
	// 0 Ok
	// 1 Unspecified Server Taxi Error
	// 2.There is no direct path to that direction
	// 3 Not enough Money
	SendPacket( &data );
	sLog.outDebug( "WORLD: Sent SMSG_ACTIVATETAXIREPLY" );

	// 0x001000 seems to make a mount visible
	// 0x002000 seems to make you sit on the mount, and the mount move with you
	// 0x000004 locks you so you can't move, no msg_move updates are sent to the server
	// 0x000008 seems to enable detailed collision checking

	// check for a summon -> if we do, remove.
	if(_player->GetSummon() != NULL)
	{
		_player->GetSummon()->Dismiss(true, true);					  // hunter pet -> just remove for later re-call
		_player->SetSummon( NULL );
	}
	if ( _player->GetCurrentSpell() != NULL )
		 _player->GetCurrentSpell()->safe_cancel();

	_player->taxi_model_id = modelid;
	GetPlayer()->TaxiStart(taxipath, modelid, 0);
	
	//sLog.outString("TAXI: Starting taxi trip. Next update in %d msec.", first_node_time);
}

void WorldSession::HandleMultipleActivateTaxiOpcode(WorldPacket & recvPacket)
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	sLog.outDebug( "WORLD: Received CMSG_ACTIVATETAXI" );

	uint64 guid;
	uint32 moocost;
	uint32 nodecount;
	vector<uint32> pathes;
	int64 newmoney;
	uint32 curloc;
	uint8 field;
	uint32 submask;
	WorldPacket data(SMSG_ACTIVATETAXIREPLY, 4);

	recvPacket >> guid >> moocost >> nodecount;
	if(nodecount < 2)
	{ 
		return;
	}

	if(nodecount>12)
	{
		GetPlayer()->SoftDisconnect();
		return;
	}

	for(uint32 i = 0; i < nodecount; ++i)
		pathes.push_back( recvPacket.read<uint32>() );

	if(GetPlayer()->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_LOCK_PLAYER))
	{ 
		return;
	}

	// get first trip
	TaxiPath* taxipath = sTaxiMgr.GetTaxiPath(pathes[0], pathes[1]);
	TaxiNode* taxinode = sTaxiMgr.GetTaxiNode(pathes[0]);

	curloc = taxinode->id;
	field = (uint8)((curloc - 1) / 32);
	submask = 1<<((curloc-1)%32);

	// Check for known nodes
	if ( (GetPlayer( )->GetTaximask(field) & submask) != submask )
	{   
		data << uint32( 1 );
		SendPacket( &data );
		return;
	}

	// Check for valid node
	if (!taxinode)
	{
		data << uint32( 1 );
		SendPacket( &data );
		return;
	}

	if (!taxipath || !taxipath->GetNodeCount())
	{
		data << uint32( 2 );
		SendPacket( &data );
		return;
	}

	if (taxipath->GetID() == 766 || taxipath->GetID() == 767 || taxipath->GetID() == 771 || taxipath->GetID() == 772)
	{
		data << uint32( 2 );
		SendPacket( &data );
		return;
	}

	int64 totalcost = taxipath->GetPrice();
	for(uint32 i = 2; i < nodecount; ++i)
	{
		TaxiPath * np = sTaxiMgr.GetTaxiPath(pathes[i-1], pathes[i]);
		if(!np) 
		{ 
			return;
		}
		totalcost += np->GetPrice();
	}

	// Check for gold
	newmoney = ((GetPlayer()->GetGold()) - totalcost);
	if(newmoney < 0 )
	{
		data << uint32( 3 );
		SendPacket( &data );
		return;
	}

	// MOUNTDISPLAYID
	// bat: 1566
	// gryph: 1147
	// wyvern: 295
	// hippogryph: 479

	uint32 modelid =0;
	if( _player->GetTeam() )
	{
		/*
		if( taxinode->horde_mount == 2224 )
			modelid =295; // In case it's a wyvern
		else
			modelid =1566; // In case it's a bat or a bad id
		*/

		CreatureInfo* ci = CreatureNameStorage.LookupEntry( taxinode->horde_mount );
		if(!ci) 
		{ 
			return;
		}
		modelid = ci->Male_DisplayID;
		if(!modelid) 
		{ 
			return;
		}
	}
	else
	{
		/*
		if( taxinode->alliance_mount == 3837 )
			modelid =479; // In case it's an hippogryph
		else
			modelid =1147; // In case it's a gryphon or a bad id
		*/

		CreatureInfo* ci = CreatureNameStorage.LookupEntry( taxinode->alliance_mount );
		if(!ci) 
		{ 
			return;
		}
		modelid = ci->Male_DisplayID;
		if(!modelid) 
		{ 
			return;
		}
	}

	//GetPlayer( )->setDismountCost( newmoney );

	data << uint32( 0 );
	// 0 Ok
	// 1 Unspecified Server Taxi Error
	// 2.There is no direct path to that direction
	// 3 Not enough Money
	SendPacket( &data );
	sLog.outDebug( "WORLD: Sent SMSG_ACTIVATETAXIREPLY" );

	// 0x001000 seems to make a mount visible
	// 0x002000 seems to make you sit on the mount, and the mount move with you
	// 0x000004 locks you so you can't move, no msg_move updates are sent to the server
	// 0x000008 seems to enable detailed collision checking

	// check for a summon -> if we do, remove.
	if(_player->GetSummon() != NULL)
	{
		_player->GetSummon()->Dismiss(true, true);					  // hunter pet -> just remove for later re-call
		_player->SetSummon( NULL );
	}

	_player->taxi_model_id = modelid;
	
	// build the rest of the path list
	for(uint32 i = 2; i < nodecount; ++i)
	{
		TaxiPath * np = sTaxiMgr.GetTaxiPath(pathes[i-1], pathes[i]);
		if(!np) 
		{ 
			return;
		}

/*		if (np->GetID() == 766 || np->GetID() == 767 || np->GetID() == 771 || np->GetID() == 772)
		{
			_player->m_taxiPaths.clear();
			return;
		}
*/
		// add to the list.. :)
		_player->m_taxiPaths.push_back(np);
	}

	// start the first trip :)
	GetPlayer()->TaxiStart(taxipath, modelid, 0);
}
