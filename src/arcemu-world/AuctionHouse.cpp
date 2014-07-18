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

void Auction::DeleteFromDB()
{
	CharacterDatabase.WaitExecute("DELETE FROM auctions WHERE auctionId = %u", Id);
}

void Auction::SaveToDB(uint32 AuctionHouseId)
{
	CharacterDatabase.Execute("INSERT INTO auctions VALUES(%u, %u, "I64FMTD", %u, %u, %u, %u, %u, %u, %u)",Id, AuctionHouseId, pItem->GetGUID(), Owner, StartingPrice, BuyoutPrice, ExpiryTime, HighestBidder, HighestBid, DepositAmount );
}

void Auction::UpdateInDB()
{
	CharacterDatabase.Execute("UPDATE auctions SET bidder = %u, bid = %u WHERE auctionId = %u", HighestBidder, HighestBid, Id);
}

AuctionHouse::AuctionHouse(uint32 ID)
{
	dbc = dbcAuctionHouse.LookupEntry(ID);
	assert(dbc);

	cut_percent = float( float(dbc->tax) / 100.0f );
	deposit_percent = float( float(dbc->fee ) / 100.0f );
}

AuctionHouse::~AuctionHouse()
{
	HM_NAMESPACE::hash_map<uint32, Auction*>::iterator itr = auctions.begin();
	for(; itr != auctions.end(); ++itr)
	{
		if( itr->second->pItem )
		{
			itr->second->pItem->DeleteMe();
			itr->second->pItem = NULL;
		}
		delete itr->second;
		itr->second = NULL;
	}
	auctions.clear();
}

void AuctionHouse::QueueDeletion(Auction * auct, uint32 Reason)
{
	if(auct->Deleted)
	{ 
		return;
	}

	auct->Deleted = true;
	auct->DeletedReason = Reason;
	removalLock.Acquire();
	removalList.push_back(auct);
	removalLock.Release();
}

void AuctionHouse::UpdateDeletionQueue()
{
	removalLock.Acquire();
	Auction * auct;

	list<Auction*>::iterator it = removalList.begin();
	for(; it != removalList.end(); ++it)
	{
		auct = *it;
		assert(auct->Deleted);
		RemoveAuction(auct);
	}

	removalList.clear();
	removalLock.Release();
}

void AuctionHouse::UpdateAuctions()
{
	auctionLock.AcquireReadLock();
	removalLock.Acquire();

	uint32 t = (uint32)UNIXTIME;
	HM_NAMESPACE::hash_map<uint32, Auction*>::iterator itr = auctions.begin();
	Auction * auct;
	for(; itr != auctions.end();)
	{
		auct = itr->second;
		++itr;

		if(t >= auct->ExpiryTime)
		{
			if(auct->HighestBidder == 0)
			{
				auct->DeletedReason = AUCTION_REMOVE_EXPIRED;
				this->SendAuctionExpiredNotificationPacket(auct);
			}
			else
			{
				auct->DeletedReason = AUCTION_REMOVE_WON;
			}

			auct->Deleted = true;
			removalList.push_back(auct);
		}
	}

	removalLock.Release();
	auctionLock.ReleaseReadLock();
}

void AuctionHouse::AddAuction(Auction * auct)
{
	// add to the map
	auctionLock.AcquireWriteLock();
	auctions.insert( HM_NAMESPACE::hash_map<uint32, Auction*>::value_type( auct->Id , auct ) );
	auctionLock.ReleaseWriteLock();

	Log.Debug("AuctionHouse", "%u: Add auction %u, expire@ %u.", dbc->id, auct->Id, auct->ExpiryTime);
}

Auction * AuctionHouse::GetAuction(uint32 Id)
{
	Auction * ret;
	HM_NAMESPACE::hash_map<uint32, Auction*>::iterator itr;
	auctionLock.AcquireReadLock();
	itr = auctions.find(Id);
	ret = (itr == auctions.end()) ? 0 : itr->second;
	auctionLock.ReleaseReadLock();
	return ret;
}

void AuctionHouse::RemoveAuction(Auction * auct)
{
	Log.Debug("AuctionHouse", "%u: Removing auction %u, reason %u.", dbc->id, auct->Id, auct->DeletedReason);

	char subject[100];
	char body[200];
	switch(auct->DeletedReason)
	{
	case AUCTION_REMOVE_EXPIRED:
		{
			// ItemEntry:0:3
			snprintf(subject, 100, "%u:0:3", (unsigned int)auct->pItem->GetEntry());

			// Auction expired, resend item, no money to owner.
			sMailSystem.SendAutomatedMessage(AUCTION, dbc->id, auct->Owner, subject, "", 0, 0, auct->pItem->GetGUID(), MAIL_STATIONERY_AUCTION);
		}break;

	case AUCTION_REMOVE_WON:
		{
			// ItemEntry:0:1
			snprintf(subject, 100, "%u:0:1", (unsigned int)auct->pItem->GetEntry());

			// <owner player guid>:bid:buyout
			snprintf(body, 200, "%X:%u:%u", (unsigned int)auct->Owner, (unsigned int)auct->HighestBid, (unsigned int)auct->BuyoutPrice);

			// Auction won by highest bidder. He gets the item.
			sMailSystem.SendAutomatedMessage(AUCTION, dbc->id, auct->HighestBidder, subject, body, 0, 0, auct->pItem->GetGUID(), MAIL_STATIONERY_AUCTION);

			// Send a mail to the owner with his cut of the price.
			int64 auction_cut = float2int32(float(cut_percent * float(auct->HighestBid)));
			int64 amount = auct->HighestBid - auction_cut + auct->DepositAmount;
			if(amount < 0)
				amount = 0;

			Player *seller = objmgr.GetPlayer( auct->Owner );
			if( seller )
			{
				seller->Event_AchiementCriteria_Received( ACHIEVEMENT_CRITERIA_TYPE_GOLD_EARNED_BY_AUCTIONS,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,amount,ACHIEVEMENT_EVENT_ACTION_ADD);
				seller->Event_AchiementCriteria_Received( ACHIEVEMENT_CRITERIA_TYPE_CREATE_AUCTION,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
				seller->Event_AchiementCriteria_Received( ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_GOLD_VALUE_OWNED,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,amount,ACHIEVEMENT_EVENT_ACTION_SET_MAX);
			}
			Player *buyer = objmgr.GetPlayer( auct->HighestBidder );
			if( buyer )
			{
				buyer->Event_AchiementCriteria_Received( ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_AUCTION_BID,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,amount,ACHIEVEMENT_EVENT_ACTION_SET_MAX);
				buyer->Event_AchiementCriteria_Received( ACHIEVEMENT_CRITERIA_TYPE_WON_AUCTIONS,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
			}

			// ItemEntry:0:2
			snprintf(subject, 100, "%u:0:2", (unsigned int)auct->pItem->GetEntry());

			// <hex player guid>:bid:0:deposit:cut
			if(auct->HighestBid == auct->BuyoutPrice)	   // Buyout
				snprintf(body, 200, "%X:%u:%u:%u:%u", (unsigned int)auct->HighestBidder, (unsigned int)auct->HighestBid, (unsigned int)auct->BuyoutPrice, (unsigned int)auct->DepositAmount, (unsigned int)auction_cut);
			else
				snprintf(body, 200, "%X:%u:0:%u:%u", (unsigned int)auct->HighestBidder, (unsigned int)auct->HighestBid, (unsigned int)auct->DepositAmount, (unsigned int)auction_cut);

			// send message away.
			sMailSystem.SendAutomatedMessage(AUCTION, dbc->id, auct->Owner, subject, body, amount, 0, 0, MAIL_STATIONERY_AUCTION);

			// If it's not a buyout (otherwise the players has been already notified)
			if(auct->HighestBid < auct->BuyoutPrice || auct->BuyoutPrice == 0)
			{
				this->SendAuctionBuyOutNotificationPacket(auct);
			}
		}break;
	case AUCTION_REMOVE_CANCELLED:
		{
			snprintf(subject, 100, "%u:0:5", (unsigned int)auct->pItem->GetEntry());
			uint32 cut = uint32(float(cut_percent * auct->HighestBid));
			Player * plr = objmgr.GetPlayer(auct->Owner);
			if(cut && plr && plr->GetGold() >= cut)
				plr->ModGold(-((int32)cut));

			sMailSystem.SendAutomatedMessage(AUCTION, GetID(), auct->Owner, subject, "", 0, 0, auct->pItem->GetGUID(), MAIL_STATIONERY_AUCTION);

			// return bidders money
			if(auct->HighestBidder)
			{
				sMailSystem.SendAutomatedMessage(AUCTION, GetID(), auct->HighestBidder, subject, "", auct->HighestBid, 0, 0, MAIL_STATIONERY_AUCTION);
			}

		}break;
	}

	// Remove the auction from the hashmap.
	auctionLock.AcquireWriteLock();
	auctions.erase(auct->Id);
	auctionLock.ReleaseWriteLock();

	// Destroy the item from memory (it still remains in the db)
	auct->pItem->DeleteMe();
	auct->pItem = NULL;

	// Finally destroy the auction instance.
	auct->DeleteFromDB();
	delete auct;
	auct = NULL;
}

void WorldSession::HandleAuctionListBidderItems( WorldPacket & recv_data )
{
	if(!_player->IsInWorld())
	{ 
		return;
	}

	uint64 guid;
	recv_data >> guid;

	Creature * pCreature = _player->GetMapMgr()->GetCreature( guid );
	if(!pCreature || !pCreature->auctionHouse)
	{ 
		return;
	}

	pCreature->auctionHouse->SendBidListPacket(_player, &recv_data);
}

void Auction::AddToPacket(WorldPacket & data, uint32 counter)
{
	/*
	13329
{SERVER} Packet: (0xAF57) SMSG_AUCTION_LIST_RESULT PacketSize = 200 TimeStamp = 111406756
01 00 00 00 count
16 C2 A4 3E auction
9B 04 00 00 entry
00 00 00 00 00 00 00 00 00 00 00 00 enchant + enchant time + charges
00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 prismatic socket
00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 randomprop
01 00 00 00 stack ?
FF FF FF FF charges
00 00 03 00 ?
8E 95 EF 03 00 00 00 01 owner
27 23 00 00 start bid 8999 
00 00 00 00 is there a highest bid ?
B8 01 00 00 cancel cost ? 440 
00 00 00 00 ?
0F 27 00 00 ? buyout cost ? 9999 
00 00 00 00 ?
1F 02 22 0A time left ?
86 D0 2A 03 00 00 00 01 highestbidder guid
27 23 00 00 best bid
00 00 00 00 ?
01 00 00 00 
2C 01 00 00

0F AE 98 3E 67 DF 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 24 2C 31 01 00 00 00 FF FF FF FF 20 00 03 00 37 A0 1B 01 00 00 00 01 5D 93 07 00 00 00 00 00 00 00 00 00 00 00 00 00 0E BC 07 00 00 00 00 00 AB 8E A4 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
13 AE 98 3E 67 DF 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 24 2C 31 01 00 00 00 FF FF FF FF 20 00 03 00 37 A0 1B 01 00 00 00 01 5D 93 07 00 00 00 00 00 00 00 00 00 00 00 00 00 0E BC 07 00 00 00 00 00 BB 90 A4 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
18 AE 98 3E 
67 DF 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 
00 00 00 00 00 24 2C 31 
01 00 00 00 stack
FF FF FF FF 
20 00 03 00 37 A0 1B 01 00 00 00 01 5D 93 07 00 00 00 00 00 00 00 00 00 00 00 00 00 0E BC 07 00 00 00 00 00 ED 91 A4 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
14333
{SERVER} Packet: (0xEEB7) SMSG_AUCTION_LIST_RESULT PacketSize = 388 TimeStamp = 18361364
01 00 00 00 count - global
3E 6C 37 4A auction
9F 00 00 00 entry
00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 enchants
00 00 00 00 00 00 00 00 00 00 00 00 prismatic
00 00 00 00 randprop
00 A1 21 45 seed 2586.0625 
01 00 00 00 stack
FF FF FF FF charges
20 80 03 00 ? 229408 
C3 40 C2 03 00 00 80 01 guid of the item ?
74 27 00 00 00 00 00 00 current bid 10100 
F4 01 00 00 00 00 00 00 next bid min amt 500 
20 4E 00 00 00 00 00 00 buyout price
E5 09 8B 09 UNIXTIME
23 A4 0F 01 00 00 80 01 highest bidder
74 27 00 00 00 00 00 00 highest bid
01 00 00 00 - uaction orderer ( list sort )
2C 01 00 00 - 300 - global - maybe refresh timer limiter ?
*/
	data << Id;
	data << pItem->GetEntry();

	for (uint32 i = 0; i < 8; i++)
	{
		data << pItem->GetUInt32Value(ITEM_FIELD_ENCHANTMENT_1_1 + (3 * i));   // Enchantment ID
		data << uint32(pItem->GetEnchantmentApplytime(i));						 // Unknown / maybe ApplyTime
		data << pItem->GetUInt32Value(ITEM_FIELD_SPELL_CHARGES + i);	   // Charges
	}

	// TODO: merge into the loop above when arcemu adds support for prismatic socket enchants
	for (uint32 i = 0; i < 3; i++)
		data << uint32( 0 );

	data << pItem->GetUInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID);		 // -ItemRandomSuffix / random property	 : If the value is negative its ItemRandomSuffix if its possitive its RandomItemProperty
	data << pItem->GetUInt32Value(ITEM_FIELD_PROPERTY_SEED);			  // when ItemRandomSuffix is used this is the modifier

	/******************** ItemRandomSuffix***************************
	* For what I have seen ItemRandomSuffix is like RandomItemProperty
	* The only difference is has is that it has a modifier.
	* That is the result of jewelcrafting, the effect is that the
	* enchantment is variable. That means that a enchantment can be +1 and 
	* with more Jem's +12 or so.
	* Decription for lookup: You get the enchantmentSuffixID and search the
	* DBC for the last 1 - 3 value's(depending on the enchantment).
	* That value is what I call EnchantmentValue. You guys might find a 
	* better name but for now its good enough. The formula to calculate
	* The ingame "GAIN" is:
	* (Modifier / 10000) * enchantmentvalue = EnchantmentGain;	
	*/

	data << pItem->GetStackCount();         // Amount
	data << pItem->GetChargesLeft();        // Charges Left
	data << uint32(0);                      // Unknown
	data << uint64(Owner);                  // Owner guid
	data << uint64(StartingPrice);          // Starting bid
	data << uint64((HighestBid > 0 ? (StartingPrice*5/100) : 0));      // Next bid value modifier, like current bid + this value
	data << uint64(BuyoutPrice);            // Buyout
	data << uint32((ExpiryTime - UNIXTIME) * 1000);         // Time left
	data << uint64(HighestBidder);          // Last bidder
	data << uint64(HighestBid);             // The bid of the last bidder
}

void AuctionHouse::SendBidListPacket(Player * plr, WorldPacket * packet)
{
	uint32 count = 0;

	WorldPacket data( SMSG_AUCTION_BIDDER_LIST_RESULT, 1024 );
	data << uint32(0);										  // Placeholder

	Auction * auct;
	auctionLock.AcquireReadLock();
	HM_NAMESPACE::hash_map<uint32, Auction*>::iterator itr = auctions.begin();
	for(; itr != auctions.end(); ++itr)
	{
		auct = itr->second;
		if(auct->HighestBidder == plr->GetGUID())
		{
			if(auct->Deleted) 
				continue;

			auct->AddToPacket(data, count);
			++count;
		}			
	}
	(*(uint32*)&data.contents()[0]) = count;
	data << uint32( count );	//show count
	data << uint32( auctions.size() );	//total count
	auctionLock.ReleaseReadLock();
	plr->GetSession()->SendPacket(&data);
}

void AuctionHouse::UpdateOwner(uint32 oldGuid, uint32 newGuid)
{
	auctionLock.AcquireWriteLock();
	HM_NAMESPACE::hash_map<uint32, Auction*>::iterator itr = auctions.begin();
	Auction * auction;
	for(; itr != auctions.end(); ++itr)
	{
		auction = itr->second;
		if(auction->Owner == oldGuid)
			auction->Owner = newGuid;
		if(auction->HighestBidder == oldGuid)
		{
			auction->HighestBidder = newGuid;
			auction->UpdateInDB();
		}
	}
	auctionLock.ReleaseWriteLock();
}

void AuctionHouse::SendOwnerListPacket(Player * plr, WorldPacket * packet)
{
	uint32 count = 0;

	WorldPacket data( SMSG_AUCTION_OWNER_LIST_RESULT, 1024);
	data << uint32(0);										  // Placeholder

	Auction * auct;
	auctionLock.AcquireReadLock();
	HM_NAMESPACE::hash_map<uint32, Auction*>::iterator itr = auctions.begin();
	for(; itr != auctions.end(); ++itr)
	{
		auct = itr->second;
		if(auct->Owner == plr->GetGUID())
		{
			if(auct->Deleted) 
				continue;

			auct->AddToPacket(data,count);
			++count;
		}			
	}
	(*(uint32*)&data.contents()[0]) = count;
	data << uint32( count );	//show count
	data << uint32( auctions.size() );	//total count
	auctionLock.ReleaseReadLock();
	plr->GetSession()->SendPacket(&data);
}

void AuctionHouse::SendPendingSalesListPacket(Player * plr, WorldPacket * packet)
{
	uint32 count = 0;

	WorldPacket data( SMSG_AUCTION_LIST_PENDING_SALES, 1024);
	data << uint32(0);										  // Placeholder

	Auction * auct;
	auctionLock.AcquireReadLock();
	HM_NAMESPACE::hash_map<uint32, Auction*>::iterator itr = auctions.begin();
	for(; itr != auctions.end(); ++itr)
	{
		auct = itr->second;
		if( auct->Owner == plr->GetGUID() 
			&& auct->HighestBid != 0
			&& auct->Deleted == false
			)
		{
			auct->AddToPacket(data, count);
			++count;
		}			
	}
	(*(uint32*)&data.contents()[0]) = count;
	data << uint32( count );	//show count
	data << uint32( auctions.size() );	//total count
	auctionLock.ReleaseReadLock();
	plr->GetSession()->SendPacket(&data);
}

void AuctionHouse::SendAuctionOutBidNotificationPacket(Auction * auct, uint64 newBidder, uint64 newHighestBid)
{
	Player *bidder = objmgr.GetPlayer(auct->HighestBidder);
	if(bidder != NULL && bidder->IsInWorld())
	{
		uint64 outbid = (auct->HighestBid / 100) * 5;
		if(!outbid)
			outbid = 1;

		//TODO: Check this code, when a user has been bid out by instant buy out
		sStackWorldPacket( data, SMSG_AUCTION_BIDDER_NOTIFICATION, 150 );
		data << GetID();
		data << auct->Id;
		data << uint64(newBidder);
		data << uint64(newHighestBid);
		data << uint64(outbid);
		data << auct->pItem->GetEntry();
		data << uint32(0);
		bidder->GetSession()->SendPacket(&data);
	}
}

void AuctionHouse::SendAuctionBuyOutNotificationPacket(Auction * auct)
{
	Player *bidder = objmgr.GetPlayer((uint32)auct->HighestBidder);
	if(bidder != NULL && bidder->IsInWorld())
	{
		uint64 outbid = (auct->HighestBid / 100) * 5;
		if(!outbid)
			outbid = 1;

		sStackWorldPacket( data, SMSG_AUCTION_BIDDER_NOTIFICATION, 150 );
		data << GetID();
		data << auct->Id;
		data << uint64(auct->HighestBidder);
		data << uint64(0);
		data << uint64(outbid);
		data << auct->pItem->GetEntry();
		data << uint32(0);
		bidder->GetSession()->SendPacket(&data);
	}

	Player *owner = objmgr.GetPlayer((uint32)auct->Owner);
	if(owner != NULL && owner->IsInWorld())
	{
		sStackWorldPacket( ownerData, SMSG_AUCTION_OWNER_NOTIFICATION, 50);
		ownerData << GetID(); 
		ownerData << auct->Id;
		ownerData << uint64(0);
		ownerData << uint64(0);
		ownerData << uint64(0);
		ownerData << auct->pItem->GetEntry();
		ownerData << uint32(0);
		owner->GetSession()->SendPacket(&ownerData);
	}
}

void AuctionHouse::SendAuctionExpiredNotificationPacket(Auction * auct)
{
	//TODO: I don't know the net code... so: TODO ;-)

	//Player *owner = objmgr.GetPlayer((uint32)auct->Owner);
	//if(owner != NULL && owner->IsInWorld())
	//{
	//  WorldPacket data(SMSG_AUCTION_REMOVED_NOTIFICATION, ??);
	//  data << GetID();     
	//  data << auct->Id;    
	//  data << uint32(0);   // I don't have an active blizz account..so I can't get the netcode by myself.
	//  data << uint32(0);
	//  data << uint32(0);
	//  data << auct->pItem->GetEntry();
	//  data << uint32(0);
	//  owner->GetSession()->SendPacket(&data);
	//}
}

void WorldSession::SendAuctionPlaceBidResultPacket(uint32 itemId, uint32 error)
{
	sStackWorldPacket( data, SMSG_AUCTION_COMMAND_RESULT, 50 );
	data << itemId;
	data << uint32(AUCTION_BID);
	data << error;
	data << uint32(0);
	data << uint32(0);
	SendPacket(&data);
}

void WorldSession::HandleAuctionPlaceBid( WorldPacket & recv_data )
{
	if(!_player->IsInWorld())
	{ 
		return;
	}

	uint64 guid;
	recv_data >> guid;

	uint32 auction_id;
	uint64 price;
	recv_data >> auction_id >> price;

	Creature * pCreature = _player->GetMapMgr()->GetCreature( guid );
	if(!pCreature || !pCreature->auctionHouse)
	{ 
		return;
	}

	// Find Item
	AuctionHouse * ah = pCreature->auctionHouse;
	Auction * auct = ah->GetAuction(auction_id);
	if(auct == 0 || !auct->Owner || !_player)
	{
		SendAuctionPlaceBidResultPacket(0, AUCTION_ERROR_INTERNAL);
		return;
	}

	if(auct->Owner == _player->GetGUID())
	{
		SendAuctionPlaceBidResultPacket(0, AUCTION_ERROR_BID_OWN_AUCTION);
		return;
	}
	if(auct->HighestBid > price && price != auct->BuyoutPrice)
	{
		//HACK: Don't know the correct error code... 
		SendAuctionPlaceBidResultPacket(0, AUCTION_ERROR_INTERNAL);
		return;
	}

	if(_player->GetGold() < price)
	{
		SendAuctionPlaceBidResultPacket(0, AUCTION_ERROR_MONEY);
		return;
	}

	_player->ModGold( -((int32)price));
	if(auct->HighestBidder != 0)
	{
		// Return the money to the last highest bidder.
		char subject[100];
		snprintf(subject, 100, "%u:0:0", (int)auct->pItem->GetEntry());
		sMailSystem.SendAutomatedMessage(AUCTION, ah->GetID(), auct->HighestBidder, subject, "", auct->HighestBid, 0, 0, MAIL_STATIONERY_AUCTION );

		// Do not send out bid notification, when current highest bidder and new bidder are the same player..
		if(auct->HighestBidder != (uint32)_player->GetLowGUID())
			ah->SendAuctionOutBidNotificationPacket(auct, _player->GetGUID(), price);
	}

	if(auct->BuyoutPrice == price)
	{
		auct->HighestBidder = _player->GetLowGUID();
		auct->HighestBid = price;

		// we used buyout on the item.
		ah->QueueDeletion(auct, AUCTION_REMOVE_WON);

		SendAuctionPlaceBidResultPacket(auct->Id, AUCTION_ERROR_NONE);
		ah->SendAuctionBuyOutNotificationPacket(auct);
	}
	else
	{
		// update most recent bid
		auct->HighestBidder = _player->GetLowGUID();
		auct->HighestBid = price;
		auct->UpdateInDB();

		SendAuctionPlaceBidResultPacket(auct->Id, AUCTION_ERROR_NONE);
	}
}

void WorldSession::HandleCancelAuction( WorldPacket & recv_data)
{
	if(!_player->IsInWorld())
	{ 
		return;
	}

	uint64 guid;
	recv_data >> guid;

	uint32 auction_id;
	recv_data >> auction_id;

	Creature * pCreature = _player->GetMapMgr()->GetCreature( guid );
	if(!pCreature || !pCreature->auctionHouse)
	{ 
		return;
	}

	// Find Item
	Auction * auct = pCreature->auctionHouse->GetAuction(auction_id);
	if(auct == 0) 
	{ 
		return;
	}

	pCreature->auctionHouse->QueueDeletion(auct, AUCTION_REMOVE_CANCELLED);

	// Send response packet.
	sStackWorldPacket( data, SMSG_AUCTION_COMMAND_RESULT, 50 );
	data << auction_id << uint32(AUCTION_CANCEL) << uint32(0) << uint32( 0 );
	SendPacket(&data);

	// Re-send the owner list.
	pCreature->auctionHouse->SendOwnerListPacket(_player, 0);
}

void WorldSession::HandleAuctionSellItem( WorldPacket & recv_data )
{
	if (!_player->IsInWorld())
	{ 
		return;
	}

#ifdef FORCED_GM_TRAINEE_MODE
	if(	CanUseCommand('k') )
	{
		_player->BroadcastMessage("You are not allowed to use this feature");
		return;
	}
#endif

	uint64 guid,item,bid,buyout;
	uint32 etime, unk1, stack_count;	// etime is in minutes

	recv_data >> guid >> unk1 >> item;
	recv_data >> stack_count;	//need to implement !
	recv_data >> bid >> buyout >> etime;

	Creature * pCreature = _player->GetMapMgr()->GetCreature( guid );
	if(  !pCreature || !pCreature->auctionHouse )
		return;		// NPC doesnt exist or isnt an auctioneer

	// Get item
	Item * pItem = _player->GetItemInterface()->GetItemByGUID(item);
	if( !pItem || pItem->IsSoulbound() || pItem->HasFlag(ITEM_FIELD_FLAGS, ITEM_FLAG_CONJURED ) )
	{
		sStackWorldPacket( data,SMSG_AUCTION_COMMAND_RESULT, 50);
		data << uint32(0);
		data << uint32(AUCTION_CREATE);
		data << uint32(AUCTION_ERROR_ITEM);
		data << uint32( 0 );
		SendPacket(&data);
		return;
	};

	AuctionHouse * ah = pCreature->auctionHouse;

	uint32 item_worth = pItem->GetProto()->SellPrice * pItem->GetUInt32Value(ITEM_FIELD_STACK_COUNT);
	uint32 item_deposit = (uint32)(item_worth * ah->deposit_percent) * (uint32)(etime / 240.0f); // deposit is per 4 hours

	if (_player->GetGold() < item_deposit)	// player cannot afford deposit
	{
		sStackWorldPacket( data,SMSG_AUCTION_COMMAND_RESULT, 50);
		data << uint32(0);
		data << uint32(AUCTION_CREATE);
		data << uint32(AUCTION_ERROR_MONEY);
		data << uint32( 0 );
		SendPacket(&data);
		return;
	}

	pItem = _player->GetItemInterface()->SafeRemoveAndRetreiveItemByGuid(item, false);
	if (!pItem){
		sStackWorldPacket( data, SMSG_AUCTION_COMMAND_RESULT, 50);
		data << uint32(0);
		data << uint32(AUCTION_CREATE);
		data << uint32(AUCTION_ERROR_ITEM);
		data << uint32( 0 );
		SendPacket(&data);
		return;
	};

	if( pItem->IsInWorld() )
	{
		pItem->RemoveFromWorld();
	}

	pItem->SetOwner(NULL);
	pItem->m_isDirty = true;
	pItem->SaveToDB(INVENTORY_SLOT_NOT_SET, 0, true, NULL);

	// Create auction
	Auction * auct = new Auction;
	auct->BuyoutPrice = buyout;
	auct->ExpiryTime = (uint32)UNIXTIME + (etime * 60);
	auct->StartingPrice = bid;
	auct->HighestBid = 0;
	auct->HighestBidder = 0;	// hm
	auct->Id = sAuctionMgr.GenerateAuctionId();
	auct->Owner = _player->GetLowGUID();
	auct->pItem = pItem;
	auct->Deleted = false;
	auct->DeletedReason = 0;
	auct->DepositAmount = item_deposit;

	// remove deposit
	_player->ModGold(-(int32)item_deposit);

	// Add and save auction to DB
	ah->AddAuction(auct);
	auct->SaveToDB(ah->GetID());

	// Send result packet
/*
13329
{SERVER} Packet: (0xEF76) SMSG_AUCTION_COMMAND_RESULT PacketSize = 12 TimeStamp = 109880942
82 3B A5 3E 
00 00 00 00 create
00 00 00 00 error
*/
	sStackWorldPacket( data,SMSG_AUCTION_COMMAND_RESULT, 50);
	data << auct->Id;
	data << uint32(AUCTION_CREATE);
	data << uint32(AUCTION_ERROR_NONE);
	data << uint32( 0 );
	SendPacket(&data);
}

void WorldSession::HandleAuctionListOwnerItems( WorldPacket & recv_data )
{
	if(!_player->IsInWorld())
	{ 
		return;
	}

	uint64 guid;
	recv_data >> guid;

	Creature * pCreature = _player->GetMapMgr()->GetCreature( guid );
	if(!pCreature || !pCreature->auctionHouse)
	{ 
		return;
	}

	pCreature->auctionHouse->SendOwnerListPacket(_player, &recv_data);
}

void WorldSession::HandleAuctionQueryPendingSales( WorldPacket & recv_data )
{
	if(!_player->IsInWorld())
	{ 
		return;
	}

	uint64 guid;
	recv_data >> guid;

	Creature * pCreature = _player->GetMapMgr()->GetCreature( guid );
	if(!pCreature || !pCreature->auctionHouse)
	{ 
		return;
	}

	pCreature->auctionHouse->SendPendingSalesListPacket(_player, &recv_data);
}

void AuctionHouse::SendAuctionList(Player * plr, WorldPacket * packet)
{
	uint32 start_index, current_index = 0;
	uint32 counted_items = 0;
	std::string auctionString;
	uint8 levelRange1, levelRange2, usableCheck;
	int32 inventory_type, itemclass, itemsubclass, rarityCheck;

/*
13329
{CLIENT} Packet: (0xD455) CMSG_AUCTION_LIST_ITEMS PacketSize = 62 TimeStamp = 109697719
5D 06 00 00 DF 21 30 F1 
00 00 00 00 
48 65 61 6C 69 6E 67 20 50 6F 74 69 6F 6E 00 
00 
00 
FF FF FF FF 
FF FF FF FF 
FF FF FF FF 
FF FF FF FF 
00 
00 
07 01 00 00 01 05 00 06 00 09 01 08 00 03 00  ??
*/
	*packet >> start_index;
	*packet >> auctionString;
	*packet >> levelRange1 >> levelRange2;
	*packet >> inventory_type >> itemclass >> itemsubclass;
	*packet >> rarityCheck >> usableCheck;

	// convert auction string to lowercase for faster parsing.
	if(auctionString.length() > 0)
	{
		for(uint32 j = 0; j < auctionString.length(); ++j)
			auctionString[j] = tolower(auctionString[j]);
	}

/*
{SERVER} Packet: (0xBFA8) SMSG_AUCTION_LIST_RESULT_COMPRESSED PacketSize = 9241 TimeStamp = 109697968
08 24 00 00 78 01 D5 99 6D 50 54 55 18 C7 CF B2 ....
uncompressed
31 00 00 00 
0F AE 98 3E 67 DF 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 24 2C 31 01 00 00 00 FF FF FF FF 20 00 03 00 37 A0 1B 01 00 00 00 01 5D 93 07 00 00 00 00 00 00 00 00 00 00 00 00 00 0E BC 07 00 00 00 00 00 AB 8E A4 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
13 AE 98 3E 67 DF 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 24 2C 31 01 00 00 00 FF FF FF FF 20 00 03 00 37 A0 1B 01 00 00 00 01 5D 93 07 00 00 00 00 00 00 00 00 00 00 00 00 00 0E BC 07 00 00 00 00 00 BB 90 A4 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
18 AE 98 3E 67 DF 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 24 2C 31 01 00 00 00 FF FF FF FF 20 00 03 00 37 A0 1B 01 00 00 00 01 5D 93 07 00 00 00 00 00 00 00 00 00 00 00 00 00 0E BC 07 00 00 00 00 00 ED 91 A4 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
07 AE 98 3E 67 DF 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 24 2C 31 01 00 00 00 FF FF FF FF 20 00 03 00 37 A0 1B 01 00 00 00 01 5D 93 07 00 00 00 00 00 C0 5D 00 00 00 00 00 00 0E BC 07 00 00 00 00 00 8F 41 A8 02 58 12 8D 03 00 00 00 01 5D 93 07 00 00 00 00 00 
10 AE 98 3E 67 DF 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 24 2C 31 01 00 00 00 FF FF FF FF 20 00 03 00 37 A0 1B 01 00 00 00 01 5D 93 07 00 00 00 00 00 C0 5D 00 00 00 00 00 00 0E BC 07 00 00 00 00 00 28 DB A8 02 58 12 8D 03 00 00 00 01 5D 93 07 00 00 00 00 00 
0A AE 98 3E 67 DF 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 24 2C 31 01 00 00 00 FF FF FF FF 20 00 03 00 37 A0 1B 01 00 00 00 01 5D 93 07 00 00 00 00 00 C0 5D 00 00 00 00 00 00 0E BC 07 00 00 00 00 00 0A EA A8 02 58 12 8D 03 00 00 00 01 5D 93 07 00 00 00 00 00 
9B 8F 91 3E A7 82 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 20 10 56 18 03 00 00 00 FF FF FF FF 00 00 03 00 3E C2 D4 03 00 00 00 01 EC D6 00 00 00 00 00 00 00 00 00 00 00 00 00 00 3B E2 00 00 00 00 00 00 4C 5D 71 05 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
D0 EF 79 3E A7 82 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 C0 44 36 59 06 00 00 00 FF FF FF FF 20 80 03 00 27 23 53 02 00 00 00 01 98 73 03 00 00 00 00 00 00 00 00 00 00 00 00 00 D0 A4 03 00 00 00 00 00 4A 2C 04 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
AA 8C 91 3E A7 82 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 F7 45 7B 14 00 00 00 FF FF FF FF 00 00 03 00 3E C2 D4 03 00 00 00 01 CB 98 05 00 00 00 00 00 00 00 00 00 00 00 00 00 34 E4 05 00 00 00 00 00 43 D1 70 05 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
B3 DE 8F 3E F7 9A 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 80 C8 90 47 0A 00 00 00 FF FF FF FF 00 00 03 00 D3 AF D4 03 00 00 00 01 80 1A 06 00 00 00 00 00 00 00 00 00 00 00 00 00 20 A1 07 00 00 00 00 00 EB 4A 08 05 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
C7 6F 95 3E 2D 59 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 80 E0 8B 8A 01 00 00 00 FF FF FF FF 20 00 03 00 1B E1 9E 03 00 00 00 01 04 29 00 00 00 00 00 00 00 00 00 00 00 00 00 00 BC 34 00 00 00 00 00 00 40 95 33 06 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
F7 D6 93 3E 2D 59 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 B4 5C 35 01 00 00 00 FF FF FF FF 00 00 03 00 CB 57 AA 03 00 00 00 01 CD 43 00 00 00 00 00 00 00 00 00 00 00 00 00 00 5B 58 00 00 00 00 00 00 08 7F E3 05 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
36 F2 8C 3E 2D 59 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 80 C9 92 70 09 00 00 00 FF FF FF FF 20 80 03 00 24 6A C5 02 00 00 00 01 7C E5 07 00 00 00 00 00 00 00 00 00 00 00 00 00 60 3D 08 00 00 00 00 00 52 56 61 04 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
A5 E2 7B 3E C4 6D 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 FD 40 48 03 00 00 00 FF FF FF FF 20 00 03 00 D0 37 0B 01 00 00 00 01 01 26 00 00 00 00 00 00 00 00 00 00 00 00 00 00 0A 32 00 00 00 00 00 00 B7 6A 55 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
BE 55 9E 3E 86 34 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 80 60 50 75 01 00 00 00 FF FF FF FF 00 00 03 00 5E DB ED 00 00 00 00 01 0B 93 00 00 00 00 00 00 00 00 00 00 00 00 00 00 FC A5 00 00 00 00 00 00 79 D4 FD 08 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
65 DE 7B 3E 86 34 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 C0 E2 9B 50 02 00 00 00 FF FF FF FF 20 00 03 00 D0 37 0B 01 00 00 00 01 1C 29 01 00 00 00 00 00 00 00 00 00 00 00 00 00 54 4F 01 00 00 00 00 00 7A BB 54 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
03 10 A4 3E 58 0F 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 80 46 BC AE 01 00 00 00 FF FF FF FF 20 00 03 00 C8 3A 24 03 00 00 00 01 27 09 00 00 00 00 00 00 00 00 00 00 00 00 00 00 D2 0A 00 00 00 00 00 00 1D E1 16 0A 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
8E 1F 83 3E 58 0F 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 80 A6 A3 20 03 00 00 00 FF FF FF FF 20 00 03 00 F3 C9 BD 02 00 00 00 01 1C 11 00 00 00 00 00 00 00 00 00 00 00 00 00 00 F1 14 00 00 00 00 00 00 24 14 06 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
1B D7 A4 3E 58 0F 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 B4 3F 3E 04 00 00 00 FF FF FF FF 20 00 03 00 E1 A9 10 03 00 00 00 01 80 16 00 00 00 00 00 00 00 00 00 00 00 00 00 00 A4 1B 00 00 00 00 00 00 A0 E9 3B 0A 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
C7 D6 A4 3E 58 0F 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 80 2E B1 4F 14 00 00 00 FF FF FF FF 20 00 03 00 E1 A9 10 03 00 00 00 01 80 70 00 00 00 00 00 00 00 00 00 00 00 00 00 00 34 8A 00 00 00 00 00 00 61 D9 3B 0A 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
DB D6 A4 3E 58 0F 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 68 12 68 12 14 00 00 00 FF FF FF FF 20 00 03 00 E1 A9 10 03 00 00 00 01 80 70 00 00 00 00 00 00 00 00 00 00 00 00 00 00 34 8A 00 00 00 00 00 00 32 DC 3B 0A 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
E8 D6 A4 3E 58 0F 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 80 DF C6 88 14 00 00 00 FF FF FF FF 20 00 03 00 E1 A9 10 03 00 00 00 01 80 70 00 00 00 00 00 00 00 00 00 00 00 00 00 00 34 8A 00 00 00 00 00 00 62 DE 3B 0A 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
F6 D6 A4 3E 58 0F 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 E0 00 2B 3B 14 00 00 00 FF FF FF FF 20 00 03 00 E1 A9 10 03 00 00 00 01 80 70 00 00 00 00 00 00 00 00 00 00 00 00 00 00 34 8A 00 00 00 00 00 00 DB E0 3B 0A 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
3A A1 95 3E 58 0F 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 10 DE 10 0C 14 00 00 00 FF FF FF FF 20 00 03 00 CA B6 4C 02 00 00 00 01 00 B9 00 00 00 00 00 00 00 00 00 00 00 00 00 00 98 DA 00 00 00 00 00 00 32 1F 3F 06 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
A9 F6 A3 3E AE 06 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 80 50 5B 5E 02 00 00 00 FF FF FF FF 20 00 03 00 C8 3A 24 03 00 00 00 01 10 24 00 00 00 00 00 00 00 00 00 00 00 00 00 00 B8 28 00 00 00 00 00 00 D0 DD 12 0A 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
8A A9 9F 3E AE 06 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 80 0E E4 96 04 00 00 00 FF FF FF FF 20 00 03 00 9D 5D C7 03 00 00 00 01 3C 30 00 00 00 00 00 00 00 00 00 00 00 00 00 00 90 33 00 00 00 00 00 00 A2 E7 3B 09 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
34 7C 7F 3E AE 06 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 80 4A 2D 22 05 00 00 00 FF FF FF FF 00 00 03 00 D4 EE FB 03 00 00 00 01 44 52 00 00 00 00 00 00 00 00 00 00 00 00 00 00 D5 66 00 00 00 00 00 00 23 FB E3 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
3B C9 A4 3E A1 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 80 06 C5 3A 01 00 00 00 FF FF FF FF 20 00 03 00 DD 46 F5 02 00 00 00 01 9E 09 00 00 00 00 00 00 00 00 00 00 00 00 00 00 E2 0A 00 00 00 00 00 00 09 5F 39 0A 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
50 FC A3 3E A1 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 80 35 62 3A 08 00 00 00 FF FF FF FF 20 00 03 00 C8 3A 24 03 00 00 00 01 80 32 00 00 00 00 00 00 00 00 00 00 00 00 00 00 A8 39 00 00 00 00 00 00 75 AE 13 0A 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
2E FC 91 3E A1 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 36 99 29 37 07 00 00 00 FF FF FF FF 20 80 03 00 83 0D 1C 03 00 00 00 01 2C EA 00 00 00 00 00 00 00 00 00 00 00 00 00 00 8B 05 01 00 00 00 00 00 33 E8 88 05 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
FE CB 7B 3E 5A 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 80 6D 75 01 00 00 00 FF FF FF FF 20 00 03 00 2F 0B F2 03 00 00 00 01 E8 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 E8 03 00 00 00 00 00 00 3B DF 51 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
02 CC 7B 3E 5A 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 80 6D 75 01 00 00 00 FF FF FF FF 20 00 03 00 2F 0B F2 03 00 00 00 01 E8 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 E8 03 00 00 00 00 00 00 87 E0 51 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
0E CC 7B 3E 5A 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 80 6D 75 01 00 00 00 FF FF FF FF 20 00 03 00 2F 0B F2 03 00 00 00 01 E8 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 E8 03 00 00 00 00 00 00 EE E1 51 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
19 CC 7B 3E 5A 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 80 6D 75 01 00 00 00 FF FF FF FF 20 00 03 00 2F 0B F2 03 00 00 00 01 E8 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 E8 03 00 00 00 00 00 00 C0 E3 51 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
1F CC 7B 3E 5A 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 80 6D 75 01 00 00 00 FF FF FF FF 20 00 03 00 2F 0B F2 03 00 00 00 01 E8 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 E8 03 00 00 00 00 00 00 8D E4 51 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
28 CC 7B 3E 5A 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 80 6D 75 01 00 00 00 FF FF FF FF 20 00 03 00 2F 0B F2 03 00 00 00 01 E8 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 E8 03 00 00 00 00 00 00 4D E6 51 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
33 CC 7B 3E 5A 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 80 6D 75 01 00 00 00 FF FF FF FF 20 00 03 00 2F 0B F2 03 00 00 00 01 E8 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 E8 03 00 00 00 00 00 00 4E E8 51 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
3C CC 7B 3E 5A 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 80 6D 75 01 00 00 00 FF FF FF FF 20 00 03 00 2F 0B F2 03 00 00 00 01 E8 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 E8 03 00 00 00 00 00 00 B9 E9 51 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
42 CC 7B 3E 5A 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 80 6D 75 01 00 00 00 FF FF FF FF 20 00 03 00 2F 0B F2 03 00 00 00 01 E8 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 E8 03 00 00 00 00 00 00 00 EB 51 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
50 CC 7B 3E 5A 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 80 6D 75 01 00 00 00 FF FF FF FF 20 00 03 00 2F 0B F2 03 00 00 00 01 E8 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 E8 03 00 00 00 00 00 00 25 ED 51 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
5A CC 7B 3E 5A 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 80 6D 75 01 00 00 00 FF FF FF FF 20 00 03 00 2F 0B F2 03 00 00 00 01 E8 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 E8 03 00 00 00 00 00 00 A2 EE 51 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
64 CC 7B 3E 5A 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 80 6D 75 01 00 00 00 FF FF FF FF 20 00 03 00 2F 0B F2 03 00 00 00 01 E8 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 E8 03 00 00 00 00 00 00 DF EF 51 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
37 1F 83 3E 5A 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 7F 91 15 05 00 00 00 FF FF FF FF 20 00 03 00 F3 C9 BD 02 00 00 00 01 2B 11 00 00 00 00 00 00 00 00 00 00 00 00 00 00 83 13 00 00 00 00 00 00 FC C2 05 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
3A 1F 83 3E 5A 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 7F 91 15 05 00 00 00 FF FF FF FF 20 00 03 00 F3 C9 BD 02 00 00 00 01 2B 11 00 00 00 00 00 00 00 00 00 00 00 00 00 00 83 13 00 00 00 00 00 00 6A C5 05 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
5F 65 94 3E 5A 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 80 DC 33 27 07 00 00 00 FF FF FF FF 00 00 03 00 20 6A F4 03 00 00 00 01 8A 1D 00 00 00 00 00 00 00 00 00 00 00 00 00 00 D4 30 00 00 00 00 00 00 57 7E D8 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
C9 EC 9C 3E 5A 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 DF C4 36 13 00 00 00 FF FF FF FF 20 00 03 00 D4 EE FB 03 00 00 00 01 5D 48 00 00 00 00 00 00 00 00 00 00 00 00 00 00 5D 48 00 00 00 00 00 00 31 B9 BC 08 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
57 EC 9C 3E 5A 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 86 DB 67 14 00 00 00 FF FF FF FF 20 00 03 00 D4 EE FB 03 00 00 00 01 2C 4C 00 00 00 00 00 00 00 00 00 00 00 00 00 00 2C 4C 00 00 00 00 00 00 36 A1 BC 08 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
B2 64 94 3E 76 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 60 83 9B 1A 0A 00 00 00 FF FF FF FF 00 00 03 00 20 6A F4 03 00 00 00 01 88 13 00 00 00 00 00 00 00 00 00 00 00 00 00 00 10 27 00 00 00 00 00 00 C5 4C D8 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
FB 25 92 3E 76 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 80 ED 23 5A 0D 00 00 00 FF FF FF FF 20 00 03 00 83 0D 1C 03 00 00 00 01 15 50 00 00 00 00 00 00 00 00 00 00 00 00 00 00 46 59 00 00 00 00 00 00 21 58 90 05 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
31 00 00 00 2C 01 00 00 

{SERVER} Packet: (0xAF57) SMSG_AUCTION_LIST_RESULT PacketSize = 200 TimeStamp = 111406756
01 00 00 00 
16 C2 A4 3E 9B 04 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01 00 00 00 FF FF FF FF 00 00 03 00 8E 95 EF 03 00 00 00 01 27 23 00 00 00 00 00 00 B8 01 00 00 00 00 00 00 0F 27 00 00 00 00 00 00 1F 02 22 0A 86 D0 2A 03 00 00 00 01 27 23 00 00 00 00 00 00 
01 00 00 00 
2C 01 00 00
*/
	WorldPacket data(SMSG_AUCTION_LIST_RESULT, 7000);
	data << uint32(0);

	auctionLock.AcquireReadLock();
	HM_NAMESPACE::hash_map<uint32, Auction*>::iterator itr = auctions.begin();
	ItemPrototype * proto;
	for(; itr != auctions.end(); ++itr)
	{
		if(itr->second->Deleted) 
			continue;
		proto = itr->second->pItem->GetProto();

		// Check the auction for parameters

		// inventory type
		if(inventory_type != -1 && inventory_type != (int32)proto->InventoryType)
			continue;

		// class
		if(itemclass != -1 && itemclass != (int32)proto->Class)
			continue;

		// subclass
		if(itemsubclass != -1 && itemsubclass != (int32)proto->SubClass)
			continue;

		// this is going to hurt. - name
		if(auctionString.length() > 0 && !FindXinYString(auctionString, proto->lowercase_name))
			continue;

		// rarity
		if(rarityCheck != -1 && rarityCheck != (int32)proto->Quality)
			continue;

		// level range check - lower boundary
		if(levelRange1 && proto->RequiredLevel < levelRange1)
			continue;

		// level range check - high boundary
		if(levelRange2 && proto->RequiredLevel > levelRange2)
			continue;

		// usable check - this will hurt too :(
		if(usableCheck)
		{
			// allowed class
			if(proto->AllowableClass && !(plr->getClassMask() & proto->AllowableClass))
				continue;

			if(proto->RequiredLevel && proto->RequiredLevel > plr->getLevel())
				continue;

			if(proto->AllowableRace && !(plr->getRaceMask() & proto->AllowableRace))
				continue;

			if(proto->Class == 4 && proto->SubClass && !(plr->GetArmorProficiency()&(((uint32)(1))<<proto->SubClass)))
				continue;

			if(proto->Class == 2 && proto->SubClass && !(plr->GetWeaponProficiency()&(((uint32)(1))<<proto->SubClass)))
				continue;

			if(proto->RequiredSkill && (!plr->_HasSkillLine(proto->RequiredSkill) || proto->RequiredSkillRank > plr->_GetSkillLineCurrent(proto->RequiredSkill, true)))
				continue;
		}

		// Page system.
		current_index++;
		if(start_index && current_index < start_index) 
			continue;
		if(counted_items >= start_index + 50)
			break;

		// all checks passed -> add to packet.
		itr->second->AddToPacket(data,counted_items);
		++counted_items;
	}

	// total count
	(*(uint32*)&data.contents()[0]) = counted_items;
	data << uint32( current_index );	//show count
	data << uint32( auctions.size() );	//total count

	auctionLock.ReleaseReadLock();
	if( counted_items )
	{
		data.SetOpcode(SMSG_AUCTION_LIST_RESULT_COMPRESSED);
		plr->GetSession()->SendPacketCompressed(&data);
	}
	else
		plr->GetSession()->SendPacket(&data);
}

void WorldSession::HandleAuctionListItems( WorldPacket & recv_data )
{
	CHECK_INWORLD_RETURN
	uint64 guid;
	recv_data >> guid;

	Creature * pCreature = _player->GetMapMgr()->GetCreature( guid );
	if(!pCreature || !pCreature->auctionHouse)
	{ 
		return;
	}

	pCreature->auctionHouse->SendAuctionList(_player, &recv_data);
}

void AuctionHouse::LoadAuctions()
{
	QueryResult *result = CharacterDatabase.Query("SELECT * FROM auctions WHERE auctionhouse =%u", GetID());

	if( !result )
	{ 
		return;
	}

	Auction * auct;
	Field * fields;

	do
	{
		fields = result->Fetch();
		auct = new Auction;
		auct->Id = fields[0].GetUInt32();

		Item * pItem = objmgr.LoadItem(fields[2].GetUInt64());
		if(!pItem)
		{
			CharacterDatabase.Execute("DELETE FROM auctions WHERE auctionId=%u",auct->Id);
			delete auct;
			auct = NULL;
			continue;
		}
		auct->pItem = pItem;
		auct->Owner = fields[3].GetUInt32();
		auct->StartingPrice = fields[4].GetUInt32();
		auct->BuyoutPrice = fields[5].GetUInt32();
		auct->ExpiryTime = fields[6].GetUInt32();
		auct->HighestBidder = fields[7].GetUInt32();
		auct->HighestBid = fields[8].GetUInt32();
		auct->DepositAmount = fields[9].GetUInt32();

		auct->DeletedReason = 0;
		auct->Deleted = false;

		auctions.insert( HM_NAMESPACE::hash_map<uint32, Auction*>::value_type( auct->Id, auct ) );
	} while (result->NextRow());
	delete result;
	result = NULL;
}
