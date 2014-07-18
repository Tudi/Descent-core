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

void WorldSession::HandleSplitOpcode(WorldPacket& recv_data)
{
	if( !_player || !_player->IsInWorld() )
	{ 
		return;
	}
	CHECK_PACKET_SIZE(recv_data, 5);
	int8 DstInvSlot=0, DstSlot=0, SrcInvSlot=0, SrcSlot=0;
	uint8 count=0;

	AddItemResult result;

	recv_data >> SrcInvSlot >> SrcSlot >> DstInvSlot >> DstSlot >> count;

	if( count==0 || count >= 127 || (SrcInvSlot <= 0 && SrcSlot < INVENTORY_SLOT_ITEM_START) || (DstInvSlot <= 0 && DstSlot < INVENTORY_SLOT_ITEM_START))
	{
		/* exploit fix */
		return;
	}

	if( !_player->GetItemInterface()->VerifyBagSlots(SrcInvSlot, SrcSlot) )
	{ 
		return;
	}

	if( !_player->GetItemInterface()->VerifyBagSlots(DstInvSlot, DstSlot) )
	{ 
		return;
	}

	int32 c=count;
	Item *i1 =_player->GetItemInterface()->GetInventoryItem(SrcInvSlot,SrcSlot);
	if(!i1)
	{ 
		return;
	}

	Item *i2=_player->GetItemInterface()->GetInventoryItem(DstInvSlot,DstSlot);

	if( (i1 && i1->wrapped_item_id) || (i2 && i2->wrapped_item_id) || ( i1 && i1->GetProto()->MaxCount < 2 ) || ( i2 && i2->GetProto()->MaxCount < 2 ) || count < 1 )
	{
		GetPlayer()->GetItemInterface()->BuildInventoryChangeError(i1, i2, INV_ERR_ITEM_CANT_STACK);
        return;
	}

	if(i2)//smth already in this slot
	{
		if(i1->GetEntry()==i2->GetEntry() )
		{
			//check if player has the required stacks to avoid exploiting.
			//safe exploit check
			if(c < (int32)i1->GetUInt32Value(ITEM_FIELD_STACK_COUNT))
			{
				//check if there is room on the other item.
				if(((c + i2->GetUInt32Value(ITEM_FIELD_STACK_COUNT)) <= i2->GetProto()->MaxCount))
				{
					i1->ModUnsigned32Value(ITEM_FIELD_STACK_COUNT, -c);
					i2->ModUnsigned32Value(ITEM_FIELD_STACK_COUNT, +c);
					i1->m_isDirty = true;
					i2->m_isDirty = true;
				}
				else
				{
					 GetPlayer()->GetItemInterface()->BuildInventoryChangeError(i1, i2, INV_ERR_ITEM_CANT_STACK);
				}
			}
			else
			{
				//error cant split item
				_player->GetItemInterface()->BuildInventoryChangeError(i1, i2, INV_ERR_COULDNT_SPLIT_ITEMS);
			}
		}
		else
		{
			 GetPlayer()->GetItemInterface()->BuildInventoryChangeError(i1, i2, INV_ERR_ITEM_CANT_STACK);
		}
	}
	else
	{
		if(c < (int32)i1->GetUInt32Value(ITEM_FIELD_STACK_COUNT))
		{
			i1->ModUnsigned32Value(ITEM_FIELD_STACK_COUNT,-c);

			i2=objmgr.CreateItem(i1->GetEntry(),_player);
			i2->SetUInt32Value(ITEM_FIELD_STACK_COUNT,c);
			i1->m_isDirty = true;
			i2->m_isDirty = true;

			if(DstSlot == -1)
			{
				// Find a free slot
				SlotResult res = _player->GetItemInterface()->FindFreeInventorySlot(i2->GetProto());
				if(!res.Result)
				{
					SendNotification("Internal Error");
					return;
				}
				else
				{
					DstSlot = res.Slot;
					DstInvSlot = res.ContainerSlot;
				}
			}
			result = _player->GetItemInterface()->SafeAddItem(i2,DstInvSlot,DstSlot);
			if( result == ADD_ITEM_RESULT_ERROR )
			{
				printf("HandleBuyItemInSlot: Error while adding item to dstslot bagslot %u and slot %u for char %s\n",DstInvSlot,DstSlot,_player->GetName());
				i2->DeleteFromDB();
				i2->DeleteMe();
				i2 = NULL;
				string sReason = "ItemDupe";
				uint32 uBanTime = (uint32)UNIXTIME + 60*60; //60 minutes ban
				_player->SetBanned( uBanTime, sReason );
				sEventMgr.AddEvent( _player, &Player::_Kick, EVENT_PLAYER_KICK, 7000, 1, 0 );
			}
		}
		else
		{
			_player->GetItemInterface()->BuildInventoryChangeError(i1, i2, INV_ERR_COULDNT_SPLIT_ITEMS);
		}
	}
}

void WorldSession::HandleSwapItemOpcode(WorldPacket& recv_data)
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	CHECK_PACKET_SIZE(recv_data, 4);
	WorldPacket data;
	WorldPacket packet;
	Item *SrcItem = NULL;
	Item *DstItem = NULL;

	//Item *SrcTemp = NULL;
	//Item *DstTemp = NULL;

	int8 DstInvSlot=0, DstSlot=0, SrcInvSlot=0, SrcSlot=0, error=0;
	//	 20		   5			255	  26

	if(!GetPlayer())
	{ 
		return;
	}
	
	recv_data >> DstInvSlot >> DstSlot >> SrcInvSlot >> SrcSlot;

	sLog.outDetail("ITEM: swap, DstInvSlot %i DstSlot %i SrcInvSlot %i SrcSlot %i", DstInvSlot, DstSlot, SrcInvSlot, SrcSlot);

	if(DstInvSlot == SrcSlot && SrcInvSlot == -1) // player trying to add self container to self container slots
	{
		GetPlayer()->GetItemInterface()->BuildInventoryChangeError(NULL, NULL, INV_ERR_ITEMS_CANT_BE_SWAPPED);
		return;
	}

	if( ( DstInvSlot <= 0 && DstSlot < 0 ) || DstInvSlot < -1 )
	{ 
		return;
	}
	
	if( ( SrcInvSlot <= 0 && SrcSlot < 0 ) || SrcInvSlot < -1 )
	{ 
		return;
	}

	SrcItem=_player->GetItemInterface()->GetInventoryItem(SrcInvSlot,SrcSlot);
	if(!SrcItem)
	{ 
		return;
	}

	DstItem=_player->GetItemInterface()->GetInventoryItem(DstInvSlot,DstSlot);

	if(DstItem)
	{   //check if it will go to equipment slot
		if(SrcInvSlot==INVENTORY_SLOT_NOT_SET)//not bag
		{
			if(DstItem->IsContainer())
			{
				if(SafeContainerCast(DstItem)->HasItems())
				{
					if(SrcSlot < INVENTORY_SLOT_BAG_START || SrcSlot >= INVENTORY_SLOT_BAG_END || SrcSlot < BANK_SLOT_BAG_START || SrcSlot >= BANK_SLOT_BAG_END)
					{
						_player->GetItemInterface()->BuildInventoryChangeError(SrcItem, DstItem, INV_ERR_NONEMPTY_BAG_OVER_OTHER_BAG);
						return;
					}
				}
			}

			if(SrcSlot <  MAX_INVENTORY_SLOT)
			{
				if((error=GetPlayer()->GetItemInterface()->CanEquipItemInSlot2(SrcInvSlot, SrcSlot, DstItem)) != 0)
				{
					_player->GetItemInterface()->BuildInventoryChangeError(SrcItem, DstItem, error);
					return;
				}
			}
		}
		else
		{
			if(DstItem->IsContainer())
			{
				if(SafeContainerCast(DstItem)->HasItems())
				{
					_player->GetItemInterface()->BuildInventoryChangeError(SrcItem, DstItem, INV_ERR_NONEMPTY_BAG_OVER_OTHER_BAG);
					return;
				}
			}

			if((error=GetPlayer()->GetItemInterface()->CanEquipItemInSlot2(SrcInvSlot, SrcInvSlot, DstItem)) != 0)
			{
				_player->GetItemInterface()->BuildInventoryChangeError(SrcItem, DstItem, error);
				return;
			}
		}
	}

	if(SrcItem)
	{   //check if it will go to equipment slot
		if(DstInvSlot==INVENTORY_SLOT_NOT_SET)//not bag
		{
			if(SrcItem->IsContainer())
			{
				if(SafeContainerCast(SrcItem)->HasItems())
				{
					if(DstSlot < INVENTORY_SLOT_BAG_START || DstSlot >= INVENTORY_SLOT_BAG_END || DstSlot < BANK_SLOT_BAG_START || DstSlot >= BANK_SLOT_BAG_END)
					{
						_player->GetItemInterface()->BuildInventoryChangeError(SrcItem, DstItem, INV_ERR_NONEMPTY_BAG_OVER_OTHER_BAG);
						return;
					}
				}
			}

			if(DstSlot <  MAX_INVENTORY_SLOT)
			{
				if((error=GetPlayer()->GetItemInterface()->CanEquipItemInSlot2(DstInvSlot, DstSlot, SrcItem)) != 0)
				{
					_player->GetItemInterface()->BuildInventoryChangeError(SrcItem, DstItem, error);
					return;
				}
			} 
		}
		else
		{
			if(SrcItem->IsContainer())
			{
				if(SafeContainerCast(SrcItem)->HasItems())
				{
					_player->GetItemInterface()->BuildInventoryChangeError(SrcItem, DstItem, INV_ERR_NONEMPTY_BAG_OVER_OTHER_BAG);
					return;
				}
			}

			if((error=GetPlayer()->GetItemInterface()->CanEquipItemInSlot2(DstInvSlot, DstInvSlot, SrcItem)) != 0)
			{
				_player->GetItemInterface()->BuildInventoryChangeError(SrcItem, DstItem, error);
				return;
			}
		}
	}

	/*if(GetPlayer()->CombatStatus.IsInCombat())
	{
		GetPlayer()->GetItemInterface()->BuildInventoryChangeError(SrcItem, NULL, INV_ERR_CANT_DO_IN_COMBAT);
		return;
	}*/

	if( DstSlot < INVENTORY_SLOT_BAG_START && DstInvSlot == INVENTORY_SLOT_NOT_SET ) //equip
	{
		if( SrcItem->GetProto()->Bonding == ITEM_BIND_ON_EQUIP )
			SrcItem->SoulBind();
	}

	if( SrcInvSlot == DstInvSlot )//in 1 bag
	{
		if( SrcInvSlot == INVENTORY_SLOT_NOT_SET ) //in backpack
		{
			_player->GetItemInterface()->SwapItemSlots( SrcSlot, DstSlot );
		}
		else//in bag
		{
			SafeContainerCast( _player->GetItemInterface()->GetInventoryItem( SrcInvSlot ) )->SwapItems( SrcSlot, DstSlot );
		}
	}
	else
	{
		// this is done in CanEquipItemInSlot ;)

		/*if (DstInvSlot != INVENTORY_SLOT_NOT_SET) 
		{
			uint32 DstInvSubClass = _player->GetItemInterface()->GetInventoryItem(DstInvSlot)->GetProto()->SubClass;
			uint32 SrcItemClass = SrcItem->GetProto()->Class;
			uint32 SrcItemSubClass = SrcItem->GetProto()->SubClass;
			uint32 DstInvClass = _player->GetItemInterface()->GetInventoryItem(DstInvSlot)->GetProto()->Class;

			// if its not ammo/arrows it shouldnt go there
			if( DstInvSubClass != 0  && SrcItemSubClass != DstInvSubClass || 
				( SrcItemClass == 11 && DstInvClass == 11 ) ) 
			{
				_player->GetItemInterface()->BuildInventoryChangeError(SrcItem, NULL, INV_ERR_ONLY_AMMO_CAN_GO_HERE);
				return;			
			}
		}*/

		//Check for stacking
		if(DstItem && SrcItem->GetEntry()==DstItem->GetEntry() && SrcItem->GetProto()->MaxCount>1 && SrcItem->wrapped_item_id == 0 && DstItem->wrapped_item_id == 0)
		{
			uint32 total=SrcItem->GetUInt32Value(ITEM_FIELD_STACK_COUNT)+DstItem->GetUInt32Value(ITEM_FIELD_STACK_COUNT);
			if(total<=DstItem->GetProto()->MaxCount)
			{
				DstItem->ModUnsigned32Value(ITEM_FIELD_STACK_COUNT,SrcItem->GetUInt32Value(ITEM_FIELD_STACK_COUNT));
				DstItem->m_isDirty = true;
				bool result = _player->GetItemInterface()->SafeFullRemoveItemFromSlot(SrcInvSlot,SrcSlot);
				if(!result)
				{
					GetPlayer()->GetItemInterface()->BuildInventoryChangeError(SrcItem, DstItem, INV_ERR_ITEM_CANT_STACK);
				}
				return;
			}
			else
			{
				if(DstItem->GetUInt32Value(ITEM_FIELD_STACK_COUNT) == DstItem->GetProto()->MaxCount)
				{

				}
				else
				{
					int32 delta=DstItem->GetProto()->MaxCount-DstItem->GetUInt32Value(ITEM_FIELD_STACK_COUNT);
					DstItem->SetUInt32Value(ITEM_FIELD_STACK_COUNT,DstItem->GetProto()->MaxCount);
					SrcItem->ModUnsigned32Value(ITEM_FIELD_STACK_COUNT,-delta);
					SrcItem->m_isDirty = true;
					DstItem->m_isDirty = true;
					return;
				}
			}
		}
	   
		if(SrcItem)
			SrcItem = _player->GetItemInterface()->SafeRemoveAndRetreiveItemFromSlot(SrcInvSlot,SrcSlot, false);

		if(DstItem)
			DstItem = _player->GetItemInterface()->SafeRemoveAndRetreiveItemFromSlot(DstInvSlot,DstSlot, false);

		if(SrcItem)
		{
			AddItemResult result =_player->GetItemInterface()->SafeAddItem(SrcItem,DstInvSlot,DstSlot);
			if( result == ADD_ITEM_RESULT_ERROR )
			{
				printf("HandleSwapItem: Error while adding item to dstslot bagslot %u and slot %u for char %s\n",DstInvSlot,DstSlot,_player->GetName());
				SrcItem->DeleteFromDB();
				SrcItem->DeleteMe();
				SrcItem = NULL;
				string sReason = "ItemDupe";
				uint32 uBanTime = (uint32)UNIXTIME + 60*60; //60 minutes ban
				_player->SetBanned( uBanTime, sReason );
				sEventMgr.AddEvent( _player, &Player::_Kick, EVENT_PLAYER_KICK, 7000, 1, 0 );
				if (DstItem && _player->GetItemInterface()->SafeAddItem(DstItem, DstInvSlot, DstSlot) == ADD_ITEM_RESULT_ERROR )
				{
					DstItem->DeleteFromDB();
					DstItem->DeleteMe();
					DstItem = NULL;
				}
				return;
			}
		}

		if(DstItem)
		{
			AddItemResult result = _player->GetItemInterface()->SafeAddItem(DstItem,SrcInvSlot,SrcSlot);
			if( result == ADD_ITEM_RESULT_ERROR )
			{
				printf("HandleSwapItem: Error while adding item to srcslot bagslot %u and slot %u for char %s\n",SrcInvSlot,SrcSlot,_player->GetName());
				DstItem->DeleteFromDB();
				DstItem->DeleteMe();
				DstItem = NULL;
				string sReason = "ItemDupe";
				uint32 uBanTime = (uint32)UNIXTIME + 60*60; //60 minutes ban
				_player->SetBanned( uBanTime, sReason );
				sEventMgr.AddEvent( _player, &Player::_Kick, EVENT_PLAYER_KICK, 7000, 1, 0 );
				if (SrcItem && _player->GetItemInterface()->SafeAddItem(SrcItem, SrcInvSlot, SrcSlot) == ADD_ITEM_RESULT_ERROR )
				{
					SrcItem->DeleteFromDB();
					SrcItem->DeleteMe();
					SrcItem = NULL;
				}
				return;
			}
		}
	}
}

void WorldSession::HandleSwapInvItemOpcode( WorldPacket & recv_data )
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	CHECK_PACKET_SIZE(recv_data, 2);
	WorldPacket data;
	int8 srcslot=0, dstslot=0;
	int8 error=0;

//	recv_data >> srcslot >> dstslot;
	recv_data >> dstslot >> srcslot;

	if(!GetPlayer())
	{ 
		return;
	}

	sLog.outDetail("ITEM: swap, src slot: %u dst slot: %u", (uint32)srcslot, (uint32)dstslot);

	if(dstslot == srcslot) // player trying to add item to the same slot
	{
		GetPlayer()->GetItemInterface()->BuildInventoryChangeError(NULL, NULL, INV_ERR_ITEMS_CANT_BE_SWAPPED);
		return;
	}

	Item * srcitem = _player->GetItemInterface()->GetInventoryItem(srcslot);

	//in case you auto store item from char
	if( srcitem && dstslot == INVENTORY_SLOT_NOT_SET )
		dstslot = _player->GetItemInterface()->FindFreeInventorySlot( srcitem->GetProto() ).Slot;
	
	Item * dstitem = _player->GetItemInterface()->GetInventoryItem(dstslot);
	// allow weapon switching in combat
	bool skip_combat = false;
	if( srcslot < EQUIPMENT_SLOT_END || dstslot < EQUIPMENT_SLOT_END )	  // We're doing an equip swap.
	{
		if( _player->CombatStatus.IsInCombat() )
		{
			if( srcslot < EQUIPMENT_SLOT_MAINHAND || dstslot < EQUIPMENT_SLOT_MAINHAND )	// These can't be swapped
			{
				_player->GetItemInterface()->BuildInventoryChangeError(srcitem, dstitem, INV_ERR_CANT_DO_IN_COMBAT);
				return;
			}
			skip_combat= true;
		}
	}

	if( !srcitem )
	{
		_player->GetItemInterface()->BuildInventoryChangeError( srcitem, dstitem, INV_ERR_YOU_CAN_NEVER_USE_THAT_ITEM );
		return;
	}

	if( srcslot == dstslot )
	{
		_player->GetItemInterface()->BuildInventoryChangeError( srcitem, dstitem, INV_ERR_ITEM_DOESNT_GO_TO_SLOT );
		return;
	}
	
	if( ( error = _player->GetItemInterface()->CanEquipItemInSlot2( INVENTORY_SLOT_NOT_SET, dstslot, srcitem, skip_combat ) )  != 0)
	{
		if( dstslot < MAX_INVENTORY_SLOT )
		{
			_player->GetItemInterface()->BuildInventoryChangeError( srcitem, dstitem, error );
			return;
		}
	}

	if(dstitem)
	{
		if((error=_player->GetItemInterface()->CanEquipItemInSlot2(INVENTORY_SLOT_NOT_SET, srcslot, dstitem, skip_combat)) != 0)
		{
			if(srcslot < MAX_INVENTORY_SLOT)
			{
				_player->GetItemInterface()->BuildInventoryChangeError( srcitem, dstitem, error );
				return;
			}
		}
	}

	if(srcitem->IsContainer())
	{
		//source has items and dst is a backpack or bank
		if(SafeContainerCast(srcitem)->HasItems())
			if(!_player->GetItemInterface()->IsBagSlot(dstslot))
			{
				_player->GetItemInterface()->BuildInventoryChangeError(srcitem,dstitem, INV_ERR_NONEMPTY_BAG_OVER_OTHER_BAG);
				return;
			}

		if(dstitem)
		{
			//source is a bag and dst slot is a bag inventory and has items
			if(dstitem->IsContainer())
			{
				if(SafeContainerCast(dstitem)->HasItems() && !_player->GetItemInterface()->IsBagSlot(srcslot))
				{
					_player->GetItemInterface()->BuildInventoryChangeError(srcitem,dstitem, INV_ERR_NONEMPTY_BAG_OVER_OTHER_BAG);
					return;
				}
			}
			else
			{
				//dst item is not a bag, swap impossible
				_player->GetItemInterface()->BuildInventoryChangeError(srcitem,dstitem,INV_ERR_NONEMPTY_BAG_OVER_OTHER_BAG);
				return;
			}
		}

		//dst is bag inventory
		if(dstslot < INVENTORY_SLOT_BAG_END)
		{
			if(srcitem->GetProto()->Bonding==ITEM_BIND_ON_EQUIP)
				srcitem->SoulBind();
		}
	}

	// swap items
  if( _player->IsDead() ) {
  	_player->GetItemInterface()->BuildInventoryChangeError(srcitem,NULL,INV_ERR_YOU_ARE_DEAD);
	  return;
  }

	_player->GetItemInterface()->SwapItemSlots(srcslot, dstslot);
}

void WorldSession::HandleDestroyItemOpcode( WorldPacket & recv_data )
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	CHECK_PACKET_SIZE(recv_data, 2);
	//Player *plyr = GetPlayer();

	int8 SrcInvSlot, SrcSlot;

	recv_data >> SrcInvSlot >> SrcSlot;

	sLog.outDetail( "ITEM: destroy, SrcInv Slot: %i Src slot: %i", SrcInvSlot, SrcSlot );
	Item *it = _player->GetItemInterface()->GetInventoryItem(SrcInvSlot,SrcSlot);

	if(it)
	{
		if(it->IsContainer())
		{
			if(SafeContainerCast(it)->HasItems())
			{
				_player->GetItemInterface()->BuildInventoryChangeError(
				it, NULL, INV_ERR_CAN_ONLY_DO_WITH_EMPTY_BAGS);
				return;
			}
		}

		if(it->GetProto()->ItemId == ITEM_ENTRY_GUILD_CHARTER)
		{
			Charter *gc = _player->m_charters[CHARTER_TYPE_GUILD];
			if(gc)
				gc->Destroy();

			_player->m_charters[CHARTER_TYPE_GUILD] = NULL;
		}

/*		if(it->GetProto()->ItemId == ARENA_TEAM_CHARTER_2v2)
		{
			Charter *gc = _player->m_charters[CHARTER_TYPE_ARENA_2V2];
			if(gc)
				gc->Destroy();

			_player->m_charters[CHARTER_TYPE_ARENA_2V2] = NULL;
		}

		if(it->GetProto()->ItemId == ARENA_TEAM_CHARTER_5v5)
		{
			Charter *gc = _player->m_charters[CHARTER_TYPE_ARENA_5V5];
			if(gc)
				gc->Destroy();

			_player->m_charters[CHARTER_TYPE_ARENA_5V5] = NULL;
		}

		if(it->GetProto()->ItemId == ARENA_TEAM_CHARTER_3v3)
		{
			Charter *gc = _player->m_charters[CHARTER_TYPE_ARENA_3V3];
			if(gc)
				gc->Destroy();

			_player->m_charters[CHARTER_TYPE_ARENA_3V3] = NULL;
		} */

		/*
		3.3 is missing this
		uint32 mail_id = it->GetUInt32Value(ITEM_FIELD_ITEM_TEXT_ID);
		if(mail_id)
			sMailSystem.RemoveMessageIfDeleted(mail_id, _player);
			*/
		


		/*bool result =  _player->GetItemInterface()->SafeFullRemoveItemFromSlot(SrcInvSlot,SrcSlot);
		if(!result)
		{
			sLog.outDetail("ITEM: Destroy, SrcInv Slot: %u Src slot: %u Failed", (uint32)SrcInvSlot, (uint32)SrcSlot);
		}*/
		Item * pItem = _player->GetItemInterface()->SafeRemoveAndRetreiveItemFromSlot(SrcInvSlot,SrcSlot,false);
		if(!pItem)
		{ 
			return;
		}

		if(_player->GetCurrentSpell() && _player->GetCurrentSpell()->i_caster==pItem)
		{
			_player->GetCurrentSpell()->i_caster=NULL;
			_player->GetCurrentSpell()->safe_cancel();
		}

		pItem->DeleteFromDB();
		pItem->DeleteMe();
		pItem = NULL;
	}
}

void WorldSession::HandleAutoEquipItemOpcode( WorldPacket & recv_data )
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	CHECK_PACKET_SIZE(recv_data, 2);
	WorldPacket data;

	AddItemResult result;
	int8 SrcInvSlot, SrcSlot, error=0;
	
	if(!GetPlayer())
	{ 
		return;
	}

	recv_data >> SrcInvSlot >> SrcSlot;

	sLog.outDetail("ITEM: autoequip, Inventory slot: %i Source Slot: %i", SrcInvSlot, SrcSlot); 

	Item *eitem=_player->GetItemInterface()->GetInventoryItem(SrcInvSlot,SrcSlot);

	if(!eitem) 
	{
		_player->GetItemInterface()->BuildInventoryChangeError(eitem, NULL, INV_ERR_ITEM_NOT_FOUND);
		return;
	}

	int16 Slot = _player->GetItemInterface()->GetItemSlotByType(eitem->GetProto()->InventoryType);
	if(Slot == ITEM_NO_SLOT_AVAILABLE)
	{
		_player->GetItemInterface()->BuildInventoryChangeError(eitem,NULL,INV_ERR_ITEM_CANT_BE_EQUIPPED);
		return;
	}

	// handle equipping of 2h when we have two items equipped! :) special case.
	if(Slot == EQUIPMENT_SLOT_MAINHAND || Slot == EQUIPMENT_SLOT_OFFHAND)
	{
		Item * mainhandweapon = _player->GetItemInterface()->GetInventoryItem(INVENTORY_SLOT_NOT_SET, EQUIPMENT_SLOT_MAINHAND);
		if( mainhandweapon != NULL && mainhandweapon->GetProto()->InventoryType == INVTYPE_2HWEAPON )
		{
			if( Slot == EQUIPMENT_SLOT_OFFHAND && eitem->GetProto()->InventoryType == INVTYPE_WEAPON )
			{
				Slot = EQUIPMENT_SLOT_MAINHAND;
			}
		}

		if((error = _player->GetItemInterface()->CanEquipItemInSlot2(INVENTORY_SLOT_NOT_SET, Slot, eitem, true, true)) != 0)
		{
			_player->GetItemInterface()->BuildInventoryChangeError(eitem,NULL, error);
			return;
		}

		if(eitem->GetProto()->InventoryType == INVTYPE_2HWEAPON)
		{
			// see if we have a weapon equipped in the offhand, if so we need to remove it
			Item * offhandweapon = _player->GetItemInterface()->GetInventoryItem(INVENTORY_SLOT_NOT_SET, EQUIPMENT_SLOT_OFFHAND);
			if( offhandweapon != NULL )
			{
				// we need to de-equip this
				SlotResult result = _player->GetItemInterface()->FindFreeInventorySlot(offhandweapon->GetProto());
				if( !result.Result )
				{
					// no free slots for this item
					_player->GetItemInterface()->BuildInventoryChangeError(eitem,NULL, INV_ERR_BAG_FULL);
					return;
				}

				offhandweapon = _player->GetItemInterface()->SafeRemoveAndRetreiveItemFromSlot(INVENTORY_SLOT_NOT_SET, EQUIPMENT_SLOT_OFFHAND, false);
				if( offhandweapon == NULL )
					return;		// should never happen

				if( _player->GetItemInterface()->SafeAddItem(offhandweapon, result.ContainerSlot, result.Slot) == ADD_ITEM_RESULT_ERROR )
					if( !_player->GetItemInterface()->AddItemToFreeSlot(&offhandweapon) )		// shouldn't happen either.
					{
						offhandweapon->DeleteMe();
						offhandweapon = NULL;
					}
			}
		}
		else
		{
			// can't equip a non-two-handed weapon with a two-handed weapon
			mainhandweapon = _player->GetItemInterface()->GetInventoryItem(INVENTORY_SLOT_NOT_SET, EQUIPMENT_SLOT_MAINHAND);
			if( mainhandweapon != NULL && mainhandweapon->GetProto()->InventoryType == INVTYPE_2HWEAPON )
			{
				// we need to de-equip this
				SlotResult result = _player->GetItemInterface()->FindFreeInventorySlot(mainhandweapon->GetProto());
				if( !result.Result )
				{
					// no free slots for this item
					_player->GetItemInterface()->BuildInventoryChangeError(eitem,NULL, INV_ERR_BAG_FULL);
					return;
				}

				mainhandweapon = _player->GetItemInterface()->SafeRemoveAndRetreiveItemFromSlot(INVENTORY_SLOT_NOT_SET, EQUIPMENT_SLOT_MAINHAND, false);
				if( mainhandweapon == NULL )
					return;		// should never happen

				if( _player->GetItemInterface()->SafeAddItem(mainhandweapon, result.ContainerSlot, result.Slot) == ADD_ITEM_RESULT_ERROR )
					if( !_player->GetItemInterface()->AddItemToFreeSlot(&mainhandweapon) )		// shouldn't happen either.
					{
						mainhandweapon->DeleteMe();
						mainhandweapon = NULL;
					}
			}
		}
	}
	else
	{
		if((error = _player->GetItemInterface()->CanEquipItemInSlot2(INVENTORY_SLOT_NOT_SET, Slot, eitem)) != 0)
		{
			_player->GetItemInterface()->BuildInventoryChangeError(eitem,NULL, error);
			return;
		}
	}

	if( Slot <= INVENTORY_SLOT_BAG_END )
	{
		if((error = _player->GetItemInterface()->CanEquipItemInSlot2(INVENTORY_SLOT_NOT_SET, Slot, eitem, false, false)) != 0)
		{
			_player->GetItemInterface()->BuildInventoryChangeError(eitem,NULL, error);
			return;
		}
	}

	Item * oitem = NULL;

	if( SrcInvSlot == INVENTORY_SLOT_NOT_SET )
	{
		_player->GetItemInterface()->SwapItemSlots( SrcSlot, Slot );
	}
	else
	{
		eitem=_player->GetItemInterface()->SafeRemoveAndRetreiveItemFromSlot(SrcInvSlot,SrcSlot, false);
		oitem=_player->GetItemInterface()->SafeRemoveAndRetreiveItemFromSlot(INVENTORY_SLOT_NOT_SET, Slot, false);
		if(oitem)
		{
			result = _player->GetItemInterface()->SafeAddItem(oitem,SrcInvSlot,SrcSlot);
			if( result == ADD_ITEM_RESULT_ERROR )
			{
				printf("HandleAutoEquip: Error while adding item to SrcSlot");
				if( oitem )
				{
					oitem->DeleteMe();
					oitem = NULL;
				}
			}
		}
		result = _player->GetItemInterface()->SafeAddItem(eitem, INVENTORY_SLOT_NOT_SET, Slot);
		if( result == ADD_ITEM_RESULT_ERROR )
		{
			printf("HandleAutoEquip: Error while adding item to Slot");
			if( eitem )
			{
				eitem->DeleteMe();
				eitem = NULL;
			}
		}
		
	}

	if(eitem && eitem->GetProto()->Bonding==ITEM_BIND_ON_EQUIP)
		eitem->SoulBind();	   
}

void WorldSession::HandleItemQuerySingleOpcode( WorldPacket & recv_data )
{
	CHECK_PACKET_SIZE(recv_data, 4);
 
//return;	//disabled since it crashes the client atm:(

	uint32 itemid=0;
	uint32 DB2FileID;
	recv_data.read_skip( 8 );	//seems to be all 0
	recv_data >> itemid;
	recv_data >> DB2FileID;	//not sure what it is, but server sent it back. Maybe it is some sort of hash on a ABD row ?
	BuildSendItemQueryReply( itemid, DB2FileID, 0 );
}

void WorldSession::HandleItemQueryMultipleOpcode( WorldPacket & recv_data )
{
	CHECK_PACKET_SIZE(recv_data, 4);
 
//return;	//disabled since it crashes the client atm:(

	uint32 counter;
	uint32 itemid;
	uint32 DB2FileID;
	recv_data >> DB2FileID;	//not sure what it is, but server sent it back. Maybe it is some sort of hash on a ABD row ?
	recv_data >> counter;
	recv_data.read_skip( counter );	//some masks i think, all 0
	for( uint32 i=0;i<counter && recv_data.rpos()<recv_data.wpos();i++)
	{
		recv_data >> itemid;
		BuildSendItemQueryReply( itemid, DB2FileID, 0 );
	}
}

void WorldSession::HandleBuyBackOpcode( WorldPacket & recv_data )
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	CHECK_PACKET_SIZE(recv_data, 8);
	WorldPacket data(16);
	uint64 guid;
	int32 stuff;
	Item* add ;
	AddItemResult result;
	uint8 error;

	sLog.outDetail( "WORLD: Received CMSG_BUYBACK_ITEM" );

	recv_data >> guid >> stuff;
	stuff -= 74;

	Item *it = _player->GetItemInterface()->GetBuyBack(stuff);
	if (it)
	{
		// Find free slot and break if inv full
		uint32 amount = it->GetUInt32Value(ITEM_FIELD_STACK_COUNT);
		uint32 itemid = it->GetUInt32Value(OBJECT_FIELD_ENTRY);
	  
		add = _player->GetItemInterface()->FindItemLessMax(itemid,amount, false);
	 
		   uint32 FreeSlots = _player->GetItemInterface()->CalculateFreeSlots(it->GetProto());
		if ((FreeSlots == 0) && (!add))
		{
			_player->GetItemInterface()->BuildInventoryChangeError(NULL, NULL, INV_ERR_INVENTORY_FULL);
			return;
		}
		
		// Check for gold
		int32 cost =_player->GetUInt32Value(PLAYER_FIELD_BUYBACK_PRICE_1 + stuff);
		if((int64)_player->GetGold() < cost )
		{
//			SendBuyFailed(guid,itemid,CAN_AFFORD_ITEM_ERROR_NOT_ENOUGH_MONEY);
			_player->GetItemInterface()->BuildInventoryChangeError( 0, 0, INV_ERR_NOT_ENOUGH_MONEY );
			return;
		}
		// Check for item uniqueness
		if ((error = _player->GetItemInterface()->CanReceiveItem(it->GetProto(), amount)) != 0)
		{
			_player->GetItemInterface()->BuildInventoryChangeError(NULL, NULL, error);
			return;
		}
		_player->ModGold( -cost );
		_player->GetItemInterface()->RemoveBuyBackItem(stuff);

		if (!add)
		{
			it->m_isDirty = true;			// save the item again on logout
			result = _player->GetItemInterface()->AddItemToFreeSlot(&it);
			if(!result)
			{
				printf("HandleBuyBack: Error while adding item to free slot");
				it->DeleteMe();
				it = NULL;
			}
		}
		else
		{
			add->SetCount(add->GetUInt32Value(ITEM_FIELD_STACK_COUNT) + amount);
			add->m_isDirty = true;

			// delete the item
			it->DeleteFromDB();

			// free the pointer
			//it->DestroyForPlayer( _player );
			it->DeleteMe();
			it = NULL;
		}

/*		data.Initialize( SMSG_BUY_ITEM );
		data << uint64(guid);
		data << uint32(itemid) << uint32(amount);
		SendPacket( &data );*/
	}
}

void WorldSession::HandleSellItemOpcode( WorldPacket & recv_data )
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	CHECK_PACKET_SIZE(recv_data, 17);
	sLog.outDetail( "WORLD: Received CMSG_SELL_ITEM" );
	if(!GetPlayer())
	{ 
		return;
	}

	uint64 vendorguid=0, itemguid=0;
	int8 amount=0;
	//uint8 slot = INVENTORY_NO_SLOT_AVAILABLE;
	//uint8 bagslot = INVENTORY_NO_SLOT_AVAILABLE;
	//int check = 0;

	recv_data >> vendorguid;
	recv_data >> itemguid;
	recv_data >> amount;

	if(_player->isCasting())
		_player->InterruptSpell();

	// Check if item exists
	if(!itemguid)
	{
		SendSellItem(vendorguid, itemguid, 1);
		return;
	}

	Creature *unit = _player->GetMapMgr()->GetCreature( vendorguid );
	// Check if Vendor exists
	if (unit == NULL)
	{
		SendSellItem(vendorguid, itemguid, 3);
		return;
	}

	Item* item = _player->GetItemInterface()->GetItemByGUID(itemguid);
	if(!item)
	{
		SendSellItem(vendorguid, itemguid, 1);
		return; //our player doesn't have this item
	}

	ItemPrototype *it = item->GetProto();
	if(!it)
	{
		SendSellItem(vendorguid, itemguid, 2);
		return; //our player doesn't have this item
	}

	if(item->IsContainer() && SafeContainerCast(item)->HasItems())
	{
		SendSellItem(vendorguid, itemguid, 6);
		return;
	}

	// Check if item can be sold
	if (it->SellPrice == 0 || item->wrapped_item_id != 0 || it->BuyPrice == 0)
	{
		SendSellItem(vendorguid, itemguid, 2);
		return;
	}
	
	uint32 stackcount = item->GetUInt32Value(ITEM_FIELD_STACK_COUNT);
	uint32 quantity = 0;

	if (amount != 0)
	{
		quantity = amount;
	}
	else
	{
		quantity = stackcount; //allitems
	}

	if(quantity > stackcount) quantity = stackcount; //make sure we don't over do it
	
	uint32 price = GetSellPriceForItem(it, quantity);

	_player->Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_GOLD_FROM_VENDORS,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,price,ACHIEVEMENT_EVENT_ACTION_ADD);
	_player->ModGold(price);
 
	if(quantity < stackcount)
	{
		item->SetCount(stackcount - quantity);
		item->m_isDirty = true;
	}
	else
	{
		//removing the item from the char's inventory
		item = _player->GetItemInterface()->SafeRemoveAndRetreiveItemByGuid(itemguid, false); //again to remove item from slot
		if(item)
		{
			_player->GetItemInterface()->AddBuyBackItem(item,(it->SellPrice) * quantity);
			item->DeleteFromDB();
		}
	}

	WorldPacket data(SMSG_SELL_ITEM, 12);
	data << vendorguid << itemguid << uint8(0); 
	SendPacket( &data );

	sLog.outDetail( "WORLD: Sent SMSG_SELL_ITEM" );
}

void WorldSession::HandleBuyItemInSlotOpcode( WorldPacket & recv_data ) // drag & drop
{
	if( !_player->IsInWorld() )
	{ 
		return;
	}

	CHECK_PACKET_SIZE( recv_data, 22 );

	sLog.outDetail( "WORLD: Received CMSG_BUY_ITEM_IN_SLOT" );

	if( GetPlayer() == NULL )
	{ 
		return;
	}

	uint64 srcguid, bagguid;
	uint32 itemid;
	int8 slot;
	uint32 amount = 0;
	uint8 error;
	int8 bagslot = INVENTORY_SLOT_NOT_SET;
	uint32 vendorslot;
	uint8 ItemOrCurrency;

	recv_data >> srcguid;
	recv_data >> ItemOrCurrency;
	recv_data >> itemid;
	recv_data >> vendorslot;
	recv_data >> amount;
	recv_data >> bagguid; 
	recv_data >> slot;

	if(amount < 1)
		amount = 1;

	if( _player->isCasting() )
		_player->InterruptSpell();

	Creature* unit = _player->GetMapMgr()->GetCreature( srcguid );
	if( unit == NULL || !unit->HasItems() )
	{ 
		return;
	}

	if( unit->GetUInt32Value(UNIT_FIELD_CHARM ) )
		return; //mob is controled and we can buy cross faction items

	Container* c = NULL;

	CreatureItem ci;
	unit->GetSellItemByItemId( itemid, ci );

	if( ci.itemid == 0 )
	{ 
		return;
	}

	if( ci.buy_amount != 0 )
		amount = amount / ci.buy_amount; //in 4.0.3 client will multiply stack * count

	if( ci.max_amount > 0 && ci.available_amount < amount )
	{
		_player->GetItemInterface()->BuildInventoryChangeError( 0, 0, INV_ERR_ITEM_IS_CURRENTLY_SOLD_OUT );
		return;
	}

	uint32 count_per_stack = ci.buy_amount * amount;

	if( ci.item_or_currency == VENDOR_SLOT_TYPE_ITEM )
	{ 
		ItemPrototype* it = ItemPrototypeStorage.LookupEntry( itemid );
		if( it == NULL )
		{
			return;
		}

		if( unit->IsCreature() && SafeCreatureCast(unit)->GetScript() != NULL )
		{
			uint32 BuyCountLimit = SafeCreatureCast(unit)->GetScript()->OnVendorItemSell( GetPlayer(), it, 0, amount );
			if( BuyCountLimit == 0 )
				return;
			amount = MIN( amount, BuyCountLimit );
		}

		if( it->MaxCount > 0 && ci.buy_amount*amount > it->MaxCount)
		{
	//		sLog.outDebug( "SUPADBG can't carry #1 (%u>%u)" , ci.amount*amount , it->MaxCount );
			_player->GetItemInterface()->BuildInventoryChangeError( 0, 0, INV_ERR_CANT_CARRY_MORE_OF_THIS );
			return;
		}

		//if slot is diferent than -1, check for validation, else continue for auto storing.
		if( slot != INVENTORY_SLOT_NOT_SET )
		{
			if( bagguid == 0 || bagguid == GetPlayer()->GetGUID() )//buy to bakcpack
			{
				if(slot > INVENTORY_SLOT_ITEM_END || slot < INVENTORY_SLOT_ITEM_START)
				{
					//hackers!
					_player->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_ITEM_DOESNT_GO_TO_SLOT);
					return;
				}
			}
			else
			{
				c=SafeContainerCast(_player->GetItemInterface()->GetItemByGUID(bagguid));
				if(!c)
				{ 
					return;
				}
				bagslot = _player->GetItemInterface()->GetBagSlotByGuid(bagguid);

				if(bagslot == INVENTORY_SLOT_NOT_SET || (c->GetProto() && (uint32)slot >= c->GetProto()->ContainerSlots))
				{
					_player->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_ITEM_DOESNT_GO_TO_SLOT);
					return;
				}
			}
		}
		else
		{
			if( bagguid == 0 || bagguid == GetPlayer()->GetGUID() )
			{
//				slot=_player->GetItemInterface()->FindFreeBackPackSlot();
				SlotResult sr = _player->GetItemInterface()->FindFreeInventorySlot( it );
				slot = sr.Slot;
				bagslot = sr.ContainerSlot;
			}
			else
			{
				c=SafeContainerCast(_player->GetItemInterface()->GetItemByGUID(bagguid));
				if(!c)
				{
					_player->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_ITEM_NOT_FOUND);
					return;//non empty
				}

				bagslot = _player->GetItemInterface()->GetBagSlotByGuid(bagguid);
				slot = c->FindFreeSlot();
			}
		}

		if((error = _player->GetItemInterface()->CanReceiveItem(it, amount)) != 0)
		{
			_player->GetItemInterface()->BuildInventoryChangeError(NULL, NULL, error);
			return;
		}

		if((error = _player->GetItemInterface()->CanAffordItem(it,amount,unit)) != 0)
		{
	//		SendBuyFailed(srcguid, ci.itemid, error);
			_player->GetItemInterface()->BuildInventoryChangeError( 0, 0, INV_ERR_NOT_ENOUGH_MONEY );
			return;
		}

		if(slot==INVENTORY_SLOT_NOT_SET)
		{
			_player->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_BAG_FULL);
			return;
		}

		// ok our z and slot are set.
		Item * oldItem= NULL;
		Item * pItem=NULL;
		if(slot != INVENTORY_SLOT_NOT_SET)
			oldItem = _player->GetItemInterface()->GetInventoryItem(bagslot, slot);

		if(oldItem != NULL)
		{
			if (oldItem->wrapped_item_id != 0)
			{
				_player->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_CANT_CARRY_MORE_OF_THIS);
				return;
			}

			// try to add to the existing items stack
			if(oldItem->GetProto() != it)
			{
				_player->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_ITEM_DOESNT_GO_TO_SLOT);
				return;
			}

			if((oldItem->GetUInt32Value(ITEM_FIELD_STACK_COUNT) + count_per_stack) > it->MaxCount)
			{
	//			sLog.outDebug( "SUPADBG can't carry #2" );
				_player->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_CANT_CARRY_MORE_OF_THIS);
				return;
			}

			oldItem->ModUnsigned32Value(ITEM_FIELD_STACK_COUNT, count_per_stack);
			oldItem->m_isDirty = true;
			pItem=oldItem;
		}
		else
		{
			// create new item
			if(slot == INVENTORY_SLOT_NOT_SET)
				slot = c->FindFreeSlot();
			
			if(slot==ITEM_NO_SLOT_AVAILABLE)
			{
				_player->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_BAG_FULL);
				return;
			}

			pItem = objmgr.CreateItem(it->ItemId, _player);
			if(pItem)
			{
				pItem->SetUInt32Value(ITEM_FIELD_STACK_COUNT, count_per_stack);
				pItem->m_isDirty = true;
	//			sLog.outDebug( "SUPADBG bagslot=%u, slot=%u" , bagslot, slot );
				if( _player->GetItemInterface()->SafeAddItem(pItem, bagslot, slot) == ADD_ITEM_RESULT_ERROR )
				{
					pItem->DeleteMe();
					return;
				}
				else if( pItem != NULL )
				{
					pItem->Randomize( ); //add some randomization if possible
				}
			}
			else
				return;
		}

		SendItemPushResult(pItem, false, true, false, (pItem==oldItem) ? false : true, bagslot, slot, amount*ci.buy_amount);
		_player->GetItemInterface()->BuyItem(it,amount,unit);
	}
	else if( ci.item_or_currency == VENDOR_SLOT_TYPE_CURRENCY )
	{

		if( unit->IsCreature() && SafeCreatureCast(unit)->GetScript() != NULL )
		{
			uint32 BuyCountLimit = SafeCreatureCast(unit)->GetScript()->OnVendorItemSell( GetPlayer(), NULL, ci.itemid, amount );
			if( BuyCountLimit == 0 )
				return;
			amount = MIN( amount, BuyCountLimit );
		}

		//check if we can carry more
		uint32 can_store = GetPlayer()->GetCurrencyCanStore( ci.itemid );
		if( can_store == 0 )
		{
			_player->GetItemInterface()->BuildInventoryChangeError( 0, 0, INV_ERR_CANT_CARRY_MORE_OF_THIS );
			return;
		}
		//check if can afford
		if((error = _player->GetItemInterface()->CanAffordCurrency(ci.itemid,amount,unit)) != 0)
		{
	//		SendBuyFailed(srcguid, ci.itemid, error);
			_player->GetItemInterface()->BuildInventoryChangeError( 0, 0, INV_ERR_NOT_ENOUGH_MONEY );
			return;
		}
		_player->GetItemInterface()->BuyCurrency(ci.itemid,amount,unit);
		uint32 AmtToBuy = amount * ci.buy_amount / 100; ///100 is not a mistake, blizz divides this automatically in the interface
		_player->ModCurrencyCount( ci.itemid, AmtToBuy );
	}


	if( ci.max_amount > 0 )
	{
		unit->ModAvItemAmount(ci.itemid,ci.buy_amount*amount);

		// there is probably a proper opcode for this. - burlex
//		SendInventoryList(unit);
	}
//	else
	{
		WorldPacket data(SMSG_BUY_ITEM, 12);
		data << uint64(srcguid);
		data << ci.item_vendor_slot;
		if( ci.max_amount == 0 )
			data << int32(-1); //no idea. maybe a slot that should be valid on us
		else
			data << ci.available_amount; //this is ammount left at vendor
		data << uint32(amount);
	 
		SendPacket( &data );
	}
	sLog.outDetail( "WORLD: Sent SMSG_BUY_ITEM" );
}
/*
void WorldSession::HandleBuyItemOpcode( WorldPacket & recv_data ) // right-click on item
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	CHECK_PACKET_SIZE(recv_data, 14);
	sLog.outDetail( "WORLD: Received CMSG_BUY_ITEM" );

	if(!GetPlayer())
	{ 
		return;
	}

	WorldPacket data(45);
	uint64 srcguid=0;
	uint32 itemid=0;
	int32 vendor_slot=0;
	uint32 amount=0;
//	int8 playerslot = 0;
//	int8 bagslot = 0;
	Item *add = NULL;
	uint8 error = 0;
	SlotResult slotresult;
	AddItemResult result;
	uint8 bag_slot;
	uint8 unk403;

	recv_data >> srcguid;
	recv_data >> unk403;
	recv_data >> itemid;
	recv_data >> vendor_slot >> amount >> unk1;


	Creature *unit = _player->GetMapMgr()->GetCreature( srcguid );
	if (unit == NULL || !unit->HasItems())
	{ 
		return;
	}

	if( unit->GetUInt32Value(UNIT_FIELD_CHARM ) )
		return; //mob is controled and we can buy cross faction items

	if(amount < 1)
		amount = 1;

	CreatureItem item;
	unit->GetSellItemByItemId(itemid, item);

	if(item.itemid == 0)
	{
		// vendor does not sell this item.. bitch about cheaters?
		_player->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_DONT_OWN_THAT_ITEM);
		return;
	}

	if (item.max_amount>0 && item.available_amount<amount)
	{
		_player->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_ITEM_IS_CURRENTLY_SOLD_OUT);
		return;
	}

	ItemPrototype *it = ItemPrototypeStorage.LookupEntry(itemid);
	if(!it) 
	{
		_player->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_DONT_OWN_THAT_ITEM);
		return;
	}

	if( amount > it->MaxCount )
	{
		_player->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_ITEM_CANT_STACK);
		return;
	}

	if((error = _player->GetItemInterface()->CanReceiveItem(it, amount*item.buy_amount)) != 0)
	{
		_player->GetItemInterface()->BuildInventoryChangeError(NULL, NULL, error);
		return;
	}

   if((error = _player->GetItemInterface()->CanAffordItem(it, amount, unit)) != 0)
   {
      SendBuyFailed(srcguid, itemid, error);
      return;
   }
 
	// Find free slot and break if inv full
	add = _player->GetItemInterface()->FindItemLessMax(itemid,amount*item.buy_amount, false);
	if (!add)
	{
		slotresult = _player->GetItemInterface()->FindFreeInventorySlot(it);
	}
	if ((!slotresult.Result) && (!add))
	{
		//Our User doesn't have a free Slot in there bag
		_player->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_INVENTORY_FULL);
		return;
	}

	if(!add)
	{
		Item *itm = objmgr.CreateItem(item.itemid, _player);
		if(!itm)
		{
			_player->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_DONT_OWN_THAT_ITEM);
			return;
		}

		itm->m_isDirty=true;
		itm->SetUInt32Value(ITEM_FIELD_STACK_COUNT, min(amount*item.buy_amount,itm->GetProto()->MaxCount));

		if(slotresult.ContainerSlot == ITEM_NO_SLOT_AVAILABLE)
		{
			result = _player->GetItemInterface()->SafeAddItem(itm, INVENTORY_SLOT_NOT_SET, slotresult.Slot);
			if( result == ADD_ITEM_RESULT_ERROR )
			{
				itm->DeleteMe();
				itm = NULL;
			}
			else
				SendItemPushResult(itm, false, true, false, true, (uint8)(INVENTORY_SLOT_NOT_SET), slotresult.Result, amount*item.buy_amount);
		}
		else 
		{
			if(Item *bag = _player->GetItemInterface()->GetInventoryItem(slotresult.ContainerSlot))
			{
				if( !SafeContainerCast(bag)->AddItem(slotresult.Slot, itm) )
				{
					itm->DeleteMe();
					itm = NULL;
				}
				else
					SendItemPushResult(itm, false, true, false, true, slotresult.ContainerSlot, slotresult.Result, 1);
			}
		}
	}
	else
	{
		add->ModUnsigned32Value(ITEM_FIELD_STACK_COUNT, min(amount*item.buy_amount,add->GetProto()->MaxCount));
		add->m_isDirty = true;
		SendItemPushResult(add, false, true, false, false, _player->GetItemInterface()->GetBagSlotByGuid(add->GetGUID()), 1, amount*item.buy_amount);
	}
		
	 if( item.max_amount > 0 ) // -1 ,eams there will be no change in the ammount
	 {
		 unit->ModAvItemAmount(item.itemid,item.buy_amount*amount);
		
		 // there is probably a proper opcode for this. - burlex
//		SendInventoryList(unit);
	 }
//	 else
	 {
		data.Initialize( SMSG_BUY_ITEM );
		data << uint64(srcguid);
		data << item.item_vendor_slot;
		if( item.max_amount == 0 )
			data << int32(-1); //no idea. maybe a slot that should be valid on us
		else
			data << item.available_amount; //this is ammount left at vendor
		data << uint32(amount*item.buy_amount);
		SendPacket( &data );
	 }
	_player->GetItemInterface()->BuyItem(it,amount,unit);
}
*/

void WorldSession::HandleListInventoryOpcode( WorldPacket & recv_data )
{
	if(!_player->IsInWorld())
	{ 
		return;
	}

	CHECK_PACKET_SIZE(recv_data, 8);
	sLog.outDetail( "WORLD: Recvd CMSG_LIST_INVENTORY" );
	uint64 guid;

	recv_data >> guid;

	Creature *unit = _player->GetMapMgr()->GetCreature( guid );
	if (unit == NULL)
	{ 
		return;
	}

	//this is a blizzlike check
	if( _player->GetDistanceSq( unit ) > 100 )
		return; //avoid talking to anyone by guid hacking. Like sell farmed items anytime ? Low chance hack

	if(unit->GetAIInterface())
		unit->GetAIInterface()->StopMovement(180000);

	_player->Reputation_OnTalk(unit->m_factionDBC);
	SendInventoryList(unit);
}

void WorldSession::SendInventoryList(Creature* unit)
{
	if(!unit->HasItems())
	{
		sChatHandler.BlueSystemMessageToPlr(_player, "No sell template found. Report this to devs: %d (%s)", unit->GetEntry(), unit->GetCreatureInfo()->Name);
		return;
	}

	if( unit->GetUInt32Value( UNIT_FIELD_CHARMEDBY ) )
		return; //mob is controled and we can buy cross faction items
/*
13329
18 AE 00 00 BD 04 30 F1 
08 item count
01 00 00 00 slot
01 00 00 00 ammount ?
4B 09 00 00 itemid
A7 08 00 00 display
FF FF FF FF available count
49 00 00 00 buyprice
3C 00 00 00 sellprice ? or durability ?
01 00 00 00 ammount ?
00 00 00 00 extended cost
00 ?
02 00 00 00 
01 00 00 00 
4D 09 00 00 
A9 08 00 00 
FF FF FF FF 
49 00 00 00 
2D 00 00 00 
01 00 00 00 
00 00 00 00 
00 
03 00 00 00 
01 00 00 00
4C 09 00 00 
F6 1A 00 00 
FF FF FF FF 
25 00 00 00 
14 00 00 00 
01 00 00 00 
00 00 00 00 
00 
04 00 00 00 01 00 00 00 4F 09 00 00 F7 1A 00 00 FF FF FF FF 38 00 00 00 1E 00 00 00 01 00 00 00 00 00 00 00 00 05 00 00 00 01 00 00 00 50 09 00 00 F8 1A 00 00 FF FF FF FF 25 00 00 00 14 00 00 00 01 00 00 00 00 00 00 00 00 06 00 00 00 01 00 00 00 51 09 00 00 F9 1A 00 00 FF FF FF FF 25 00 00 00 14 00 00 00 01 00 00 00 00 00 00 00 00 07 00 00 00 01 00 00 00 20 43 00 00 30 48 00 00 FF FF FF FF 23 00 00 00 19 00 00 00 01 00 00 00 00 00 00 00 00 08 00 00 00 01 00 00 00 51 08 00 00 E6 48 00 00 FF FF FF FF 4A 00 00 00 23 00 00 00 01 00 00 00 00 00 00 00 00 

48 0F 00 00 98 00 30 F1 
03 
01 00 00 00 
01 00 00 00 
BC 11 00 00 
FF 18 00 00 
FF FF FF FF - infinit ammount available
18 00 00 00 
FF FF FF FF 
05 00 00 00 - buy stack count
00 00 00 00 
00 
02 00 00 00 01 00 00 00 9F 00 00 00 A4 46 00 00 FF FF FF FF 18 00 00 00 FF FF FF FF 05 00 00 00 00 00 00 00 00 03 00 00 00 01 00 00 00 90 11 00 00 4F 20 00 00 FF FF FF FF DB 01 00 00 FF FF FF FF 01 00 00 00 00 00 00 00 00 
*/

	WorldPacket data(((unit->GetSellItemCount() * 28) + 9));	   // allocate

	uint8	obfuscated_guid_mask = 0;
	uint32	counter_placeholder;
	uint64	old_guid = unit->GetGUID();
	uint8	*guid_bytes = (uint8*)&old_guid;

	if( guid_bytes[7] )
		obfuscated_guid_mask |= BIT_6;
	if( guid_bytes[4] )
		obfuscated_guid_mask |= BIT_7;
	if( guid_bytes[0] )
		obfuscated_guid_mask |= BIT_5;
	if( guid_bytes[1] )
		obfuscated_guid_mask |= BIT_0;
	if( guid_bytes[5] )
		obfuscated_guid_mask |= BIT_3;
	if( guid_bytes[3] )
		obfuscated_guid_mask |= BIT_2;
	if( guid_bytes[2] )
		obfuscated_guid_mask |= BIT_4;
	if( guid_bytes[6] )
		obfuscated_guid_mask |= BIT_1;

	data.SetOpcode( SMSG_LIST_INVENTORY );
	data << obfuscated_guid_mask;

	if( guid_bytes[7] )
		data << ObfuscateByte( guid_bytes[7] );
	if( guid_bytes[4] )
		data << ObfuscateByte( guid_bytes[4] );
	if( guid_bytes[0] )
		data << ObfuscateByte( guid_bytes[0] );
	if( guid_bytes[1] )
		data << ObfuscateByte( guid_bytes[1] );
	if( guid_bytes[5] )
		data << ObfuscateByte( guid_bytes[5] );
	if( guid_bytes[3] )
		data << ObfuscateByte( guid_bytes[3] );
	if( guid_bytes[2] )
		data << ObfuscateByte( guid_bytes[2] );

	counter_placeholder = (uint32)data.wpos();
	data << uint32( 0 ); // placeholder for item count
	data << uint8( 0 );	//unk

	if( guid_bytes[6] )
		data << ObfuscateByte( guid_bytes[6] );

	ItemPrototype * curItem;
	uint32 counter = 0;

	for(std::vector<CreatureItem>::iterator itr = unit->GetSellItemBegin(); itr != unit->GetSellItemEnd(); ++itr)
	{
		if(itr->itemid && (itr->max_amount == 0 || (itr->max_amount>0 && itr->available_amount >0)))
		{
			int32 available_am = (itr->max_amount>0)?itr->available_amount:-1;
			int32 gold_price = 0;
			int32 available_durability = 0;
			int32 display = 0;
			if( itr->item_or_currency == 2 )
				curItem = NULL;
			else
			{
				curItem = ItemPrototypeStorage.LookupEntry(itr->itemid);
				if( curItem == NULL )
					continue;
//			if((curItem = ItemPrototypeStorage.LookupEntry(itr->itemid)) != 0)
//				if(curItem->AllowableClass && !(_player->getClassMask() & curItem->AllowableClass) && !_player->GetSession()->HasGMPermissions()) // GM looking up for everything.
//					continue;
//				if(curItem->AllowableRace && !(_player->getRaceMask() & curItem->AllowableRace) && !_player->GetSession()->HasGMPermissions())
//					continue;
				if( GetPlayer()->GetItemInterface()->CanEquipItem( curItem, GetPlayer() ) != 0 && !_player->GetSession()->HasGMPermissions() )
					continue;
				gold_price =  GetBuyPriceForItem(curItem, 1, _player, unit); //gold
				available_durability = (curItem->MaxDurability>0)?curItem->MaxDurability:-1;
				display = curItem->DisplayInfoID;
			}

			if( unit->IsCreature() && SafeCreatureCast(unit)->GetScript() != NULL && SafeCreatureCast(unit)->GetScript()->OnVendorInventoryShow( GetPlayer(), curItem, itr->itemid ) == false )
					continue;

			data << uint32( gold_price );	//no gold price for currency, we really should have extended cost

			if( itr->extended_cost != NULL )
				data << itr->extended_cost->costid;
			else
				data << int32(0);

			data << itr->buy_amount;		//buy stack

			data << uint32( available_durability );		//durability ?
			data << int32( 0 );			// never seen anything else then 0, maybe time until it will be available ?
			data << uint32( itr->item_or_currency );		//1 = item, 2 = currency
			data << available_am;
			data << itr->item_vendor_slot;
			data << uint32( display );	//currency has no display
			data << itr->itemid;

			++counter;
			if( counter >= 10*15 )
			{
				sLog.outDebug("Vendor list warning : list contains more then 128 items. Client will crash.Exiting\n");
				break;
			} 
		}
	}

	data.WriteAtPos( counter, counter_placeholder );	// set count

	SendPacket( &data );
	sLog.outDetail( "WORLD: Sent SMSG_LIST_INVENTORY" );
}

void WorldSession::SendListInventory(Creature* unit)
{
	SendInventoryList(unit);
}


void WorldSession::HandleAutoStoreBagItemOpcode( WorldPacket & recv_data )
{
	CHECK_PACKET_SIZE(recv_data, 3);
	sLog.outDetail( "WORLD: Recvd CMSG_AUTO_STORE_BAG_ITEM" );
	
	if(!GetPlayer())
	{ 
		return;
	}

	//WorldPacket data;
	WorldPacket packet;
	int8 SrcInv=0, Slot=0, DstInv=0;
//	Item *item= NULL;
	Item *srcitem = NULL;
	Item *dstitem= NULL;
	int8 NewSlot = 0;
	int8 error;
	AddItemResult result;

	recv_data >> SrcInv >> Slot >> DstInv;

	srcitem = _player->GetItemInterface()->GetInventoryItem(SrcInv, Slot);

	//source item exists
	if(srcitem)
	{
		//src containers cant be moved if they have items inside
		if(srcitem->IsContainer() && SafeContainerCast(srcitem)->HasItems())
		{
			_player->GetItemInterface()->BuildInventoryChangeError(srcitem, 0, INV_ERR_NONEMPTY_BAG_OVER_OTHER_BAG);
			return;
		}
		//check for destination now before swaping.
		//destination is backpack
		if(DstInv == INVENTORY_SLOT_NOT_SET)
		{
			//check for space
			NewSlot = _player->GetItemInterface()->FindFreeBackPackSlot();
			if(NewSlot == ITEM_NO_SLOT_AVAILABLE)
			{
				_player->GetItemInterface()->BuildInventoryChangeError(srcitem, 0, INV_ERR_BAG_FULL);
				return;
			}
			else
			{
				//free space found, remove item and add it to the destination
				srcitem = _player->GetItemInterface()->SafeRemoveAndRetreiveItemFromSlot(SrcInv, Slot, false);
				if( srcitem )
				{
					result = _player->GetItemInterface()->SafeAddItem(srcitem, INVENTORY_SLOT_NOT_SET, NewSlot);
					if( result == ADD_ITEM_RESULT_ERROR )
					{
						printf("HandleAutoStoreBagItem: Error while adding item to newslot");
						srcitem->DeleteMe();
						return;
					}
				}
			}
		}
		else
		{
			if((error=_player->GetItemInterface()->CanEquipItemInSlot2(DstInv,  DstInv, srcitem)) != 0)
			{
				if(DstInv < MAX_INVENTORY_SLOT)
				{
					_player->GetItemInterface()->BuildInventoryChangeError(srcitem,dstitem, error);
					return;
				}
			}

			//destination is a bag
			dstitem = _player->GetItemInterface()->GetInventoryItem(DstInv);
			if(dstitem)
			{
				//dstitem exists, detect if its a container
				if(dstitem->IsContainer())
				{
					NewSlot = SafeContainerCast(dstitem)->FindFreeSlot();
					if(NewSlot == ITEM_NO_SLOT_AVAILABLE)
					{
						_player->GetItemInterface()->BuildInventoryChangeError(srcitem, 0, INV_ERR_BAG_FULL);
						return;
					}
					else
					{
						srcitem = _player->GetItemInterface()->SafeRemoveAndRetreiveItemFromSlot(SrcInv, Slot, false);
						if( srcitem != NULL )
						{
							result = _player->GetItemInterface()->SafeAddItem(srcitem, DstInv, NewSlot);
							if( result == ADD_ITEM_RESULT_ERROR )
							{
								printf("HandleBuyItemInSlot: Error while adding item to newslot");
								srcitem->DeleteFromDB();
								srcitem->DeleteMe();
								srcitem = NULL;
								string sReason = "ItemDupe";
								uint32 uBanTime = (uint32)UNIXTIME + 60*60; //60 minutes ban
								_player->SetBanned( uBanTime, sReason );
								sEventMgr.AddEvent( _player, &Player::_Kick, EVENT_PLAYER_KICK, 7000, 1, 0 );
								return;
							}		
						}
					}
				}
				else
				{
					_player->GetItemInterface()->BuildInventoryChangeError(srcitem, 0,  INV_ERR_ITEM_DOESNT_GO_TO_SLOT);
					return;
				}
			}
			else
			{
				_player->GetItemInterface()->BuildInventoryChangeError(srcitem, 0, INV_ERR_ITEM_DOESNT_GO_TO_SLOT);
				return;
			}
		}
	}
	else
	{
		_player->GetItemInterface()->BuildInventoryChangeError(srcitem, 0, INV_ERR_ITEM_NOT_FOUND);
		return;
	}
}

void WorldSession::HandleReadItemOpcode(WorldPacket &recvPacket)
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	CHECK_PACKET_SIZE(recvPacket, 2);
	int8 uslot=0, slot=0;
	recvPacket >> uslot >> slot;

	if(!GetPlayer())
	{ 
		return;
	}

	Item *item = _player->GetItemInterface()->GetInventoryItem(uslot, slot);
	sLog.outDebug("Received CMSG_READ_ITEM %d", slot);

	if(item)
	{
		// Check if it has pagetext
	   
		if(item->GetProto()->PageId)
		{
			WorldPacket data(SMSG_READ_ITEM_OK, 4);
			data << item->GetGUID();
			SendPacket(&data);
			sLog.outDebug("Sent SMSG_READ_OK %d", item->GetGUID());
		}	
	}
}

ARCEMU_INLINE int64 RepairItemCost(Player * pPlayer, Item * pItem)
{
	DurabilityCostsEntry * dcosts = dbcDurabilityCosts.LookupEntry(pItem->GetProto()->ItemLevel);
	if(!dcosts)
	{
		sLog.outError("Repair: Unknown item level (%u)", dcosts);
		return 0;
	}

	DurabilityQualityEntry * dquality = dbcDurabilityQuality.LookupEntry((pItem->GetProto()->Quality + 1) * 2);
	if(!dquality)
	{
		sLog.outError("Repair: Unknown item quality (%u)", dquality);
		return 0;
	}

	int64 dmodifier = dcosts->modifier[pItem->GetProto()->Class == ITEM_CLASS_WEAPON ? pItem->GetProto()->SubClass : pItem->GetProto()->SubClass + 21];
	int64 cost = long2int32((pItem->GetDurabilityMax() - pItem->GetDurability()) * dmodifier * double(dquality->quality_modifier));
	return cost;
}

ARCEMU_INLINE bool RepairItem(Player * pPlayer, Item * pItem)
{
	//int32 cost = (int32)pItem->GetUInt32Value( ITEM_FIELD_MAXDURABILITY ) - (int32)pItem->GetUInt32Value( ITEM_FIELD_DURABILITY );
	int64 cost = RepairItemCost(pPlayer, pItem);
	if( cost <= 0 )
	{ 
		return FALSE;
	}

	if( cost > (int64)pPlayer->GetGold() )
	{ 
		return FALSE;
	}

	pPlayer->ModGold( -cost );
	pItem->SetDurabilityToMax();
	pItem->m_isDirty = true;
	return TRUE;
}

void WorldSession::HandleRepairItemOpcode(WorldPacket &recvPacket)
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	CHECK_PACKET_SIZE(recvPacket, 12);

	if(!GetPlayer())
	{ 
		return;
	}

	uint64 npcguid;
	uint64 itemguid;
	Item * pItem;
	Container * pContainer;
	uint32 j, i;

	recvPacket >> npcguid >> itemguid;

	Creature * pCreature = _player->GetMapMgr()->GetCreature( npcguid );
	if( pCreature == NULL )
	{ 
		return;
	}

	if( !pCreature->HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_ARMORER ) )
	{ 
		return;
	}

	//this is a blizzlike check
	if( _player->GetDistanceSq( pCreature ) > 100 )
		return; //avoid talking to anyone by guid hacking. Like repair items anytime in raid ? Low chance hack

	if( !itemguid ) 
	{
		for( i = 0; i < MAX_INVENTORY_SLOT; i++ )
		{
			pItem = _player->GetItemInterface()->GetInventoryItem( i );
			if( pItem != NULL )
			{
				if( pItem->IsContainer() )
				{
					pContainer = SafeContainerCast( pItem );
					for( j = 0; j < pContainer->GetProto()->ContainerSlots; ++j )
					{
						pItem = pContainer->GetItem( j );
						if( pItem != NULL )
							RepairItem( _player, pItem );
					}
				}
				else
				{
					if( i < INVENTORY_SLOT_BAG_END )
					{
						if( pItem->GetDurability() == 0 && RepairItem( _player, pItem ) )
							_player->ApplyItemMods( pItem, i, true );
						else
							RepairItem( _player, pItem );
					}
				}
			}
		}
	}
	else 
	{
		Item *item = _player->GetItemInterface()->GetItemByGUID(itemguid);
		if(item)
		{
			SlotResult *searchres=_player->GetItemInterface()->LastSearchResult();//this never gets null since we get a pointer to the inteface internal var
			uint32 dDurability = item->GetDurabilityMax() - item->GetDurability();

			if (dDurability)
			{
                uint32 cDurability = item->GetDurability();
                //only apply item mods if they are on char equiped
                if( RepairItem( _player, item ) && cDurability == 0 && searchres->ContainerSlot==INVALID_BACKPACK_SLOT && searchres->Slot<INVENTORY_SLOT_BAG_END)
                    _player->ApplyItemMods(item, searchres->Slot, true);
			}
		}
	}
	sLog.outDebug("Received CMSG_REPAIR_ITEM %d", itemguid);
}

void WorldSession::HandleBuyBankSlotOpcode(WorldPacket& recvPacket) 
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	//CHECK_PACKET_SIZE(recvPacket, 12);
	uint32 bytes,slots;
	int32 price;
	sLog.outDebug("WORLD: CMSG_BUY_bytes_SLOT");

	bytes = GetPlayer()->GetUInt32Value(PLAYER_BYTES_2);
	slots =(uint8) (bytes >> 16);

	sLog.outDetail("PLAYER: Buy bytes bag slot, slot number = %d", slots);
	BankSlotPrice* bsp = dbcBankSlotPrices.LookupEntry(slots+1);
	price = (bsp != NULL ) ? bsp->Price : 99999999;

	if ((int64)_player->GetGold() >= price) 
	{
	   _player->SetUInt32Value(PLAYER_BYTES_2, (bytes&0xff00ffff) | ((slots+1) << 16) );
	   _player->ModGold( -price);
	}
	_player->Event_AchiementCriteria_Received( ACHIEVEMENT_CRITERIA_TYPE_BUY_BANK_SLOT, ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE, 1, ACHIEVEMENT_EVENT_ACTION_ADD );
}

void WorldSession::HandleAutoBankItemOpcode(WorldPacket &recvPacket)
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	CHECK_PACKET_SIZE(recvPacket, 2);
	sLog.outDebug("WORLD: CMSG_AUTO_BANK_ITEM");

	//WorldPacket data;

	SlotResult slotresult;
	int8 SrcInvSlot, SrcSlot;//, error=0;

	if(!GetPlayer())
	{ 
		return;
	}

	recvPacket >> SrcInvSlot >> SrcSlot;

	sLog.outDetail("ITEM: Auto Bank, Inventory slot: %u Source Slot: %u", (uint32)SrcInvSlot, (uint32)SrcSlot);

	Item *eitem=_player->GetItemInterface()->GetInventoryItem(SrcInvSlot,SrcSlot);

	if(!eitem) 
	{
		_player->GetItemInterface()->BuildInventoryChangeError(eitem, NULL, INV_ERR_ITEM_NOT_FOUND);
		return;
	}

	slotresult =  _player->GetItemInterface()->FindFreeBankSlot(eitem->GetProto());

	if(!slotresult.Result)
	{
		_player->GetItemInterface()->BuildInventoryChangeError(eitem, NULL, INV_ERR_BANK_FULL);
		return;
	}
	else
	{
        eitem = _player->GetItemInterface()->SafeRemoveAndRetreiveItemFromSlot(SrcInvSlot,SrcSlot, false);
		if( _player->GetItemInterface()->SafeAddItem(eitem, slotresult.ContainerSlot, slotresult.Slot) == ADD_ITEM_RESULT_ERROR )
		{
			sLog.outDebug("[ERROR]AutoBankItem: Error while adding item to bank bag!\n");
			if( eitem && _player->GetItemInterface()->SafeAddItem(eitem, slotresult.ContainerSlot, slotresult.Slot) == ADD_ITEM_RESULT_ERROR && eitem )
				eitem->DeleteMe();
		}
	}
}

void WorldSession::HandleAutoStoreBankItemOpcode(WorldPacket &recvPacket)
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	CHECK_PACKET_SIZE(recvPacket, 2);
	sLog.outDebug("WORLD: CMSG_AUTOSTORE_BANK_ITEM");

	//WorldPacket data;

	int8 SrcInvSlot, SrcSlot;//, error=0, slot=-1, specialbagslot=-1;

	if(!GetPlayer())
	{ 
		return;
	}

	recvPacket >> SrcInvSlot >> SrcSlot;

	sLog.outDetail("ITEM: AutoStore Bank Item, Inventory slot: %u Source Slot: %u", (uint32)SrcInvSlot, (uint32)SrcSlot);

	Item *eitem=_player->GetItemInterface()->GetInventoryItem(SrcInvSlot,SrcSlot);

	if(!eitem) 
	{
		_player->GetItemInterface()->BuildInventoryChangeError(eitem, NULL, INV_ERR_ITEM_NOT_FOUND);
		return;
	}

	SlotResult slotresult = _player->GetItemInterface()->FindFreeInventorySlot(eitem->GetProto());

	if(!slotresult.Result)
	{
		_player->GetItemInterface()->BuildInventoryChangeError(eitem, NULL, INV_ERR_INVENTORY_FULL);
		return;
	}
	else
	{
        eitem = _player->GetItemInterface()->SafeRemoveAndRetreiveItemFromSlot(SrcInvSlot, SrcSlot, false);
		if( eitem && _player->GetItemInterface()->AddItemToFreeSlot(&eitem) == ADD_ITEM_RESULT_ERROR )
		{
			sLog.outDebug("[ERROR]AutoStoreBankItem: Error while adding item from one of the bank bags to the player bag!\n");
           if( eitem && _player->GetItemInterface()->SafeAddItem(eitem, SrcInvSlot, SrcSlot) == ADD_ITEM_RESULT_ERROR )
			   eitem->DeleteMe();
		}
	}
}

void WorldSession::HandleCancelTemporaryEnchantmentOpcode(WorldPacket &recvPacket)
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	uint32 inventory_slot;
	recvPacket >> inventory_slot;

	Item * item = _player->GetItemInterface()->GetInventoryItem(inventory_slot);
	if(!item) 
	{ 
		return;
	}

	item->RemoveAllEnchantments(true);
}

void WorldSession::HandleInsertGemOpcode(WorldPacket &recvPacket)
{
	uint64 itemguid;
	uint64 gemguid;
	ItemInterface *itemi = _player->GetItemInterface();
	GemPropertyEntry * gp = NULL;
	EnchantEntry * Enchantment;
	recvPacket >> itemguid ;

	Item * TargetItem = itemi->GetItemByGUID(itemguid);

	if(!TargetItem)
	{ 
		return;
	}

	int slot = itemi->GetInventorySlotByGuid(itemguid);
	bool apply = (slot>=EQUIPMENT_SLOT_START && slot <EQUIPMENT_SLOT_END);

	bool found_slot = false;
	uint32 ItemSocketCount,EnchantSocketCount,TotalSocketCount;
	TotalSocketCount = TargetItem->GetSocketsCount( ItemSocketCount, EnchantSocketCount );
	EnchantSocketCount = MIN( 1, EnchantSocketCount ); //need to update this script to support more then 1 Enchant socket
	TotalSocketCount = MIN( 3, ItemSocketCount + EnchantSocketCount );
	for(int32 i = 0;i<(int32)TotalSocketCount;i++)
	{
		recvPacket >> gemguid;

		if(gemguid)//add or replace gem
		{

			int32 SocketColor;
			int32 SocketIndexToGem;
			SocketIndexToGem = ITEM_ENCHANT_SLOT_GEM1 + i;
			if( i < (int32)ItemSocketCount )
				SocketColor = TargetItem->GetProto()->Sockets.SocketColor[ i ];
			else
			{
				EnchantmentInstance *EI = TargetItem->GetEnchantment(PRISMATIC_ENCHANTMENT_SLOT);
				if( EI )//replace gem
					SocketColor = EI->Enchantment->min[0]; //should have value 1 ( prismatic )
				else
					SocketColor = META_SOCKET;
			}
			//strange, but might happen
			if( SocketColor == 0 )
				continue;

			Item *GemItem;
			ItemPrototype *GemProto;

			GemItem = itemi->GetItemByGUID( gemguid );
			if(!GemItem)
				continue;

			GemProto = GemItem->GetProto();
			if ( (GemProto->Flags & ITEM_FLAG_UNIQUE_EQUIP ) && itemi->IsEquipped(GemProto->ItemId))
			{
				itemi->BuildInventoryChangeError(GemItem, TargetItem, INV_ERR_CANT_CARRY_MORE_OF_THIS);
				continue;
			}
			// Skill requirement
			if( GemProto->RequiredSkill && GemProto->RequiredSkillRank > _player->_GetSkillLineCurrent( GemProto->RequiredSkill, true ) )
			{
				itemi->BuildInventoryChangeError( GemItem, TargetItem, INV_ERR_SKILL_ISNT_HIGH_ENOUGH );
				return;
			}
			//make sure to match color
			gp = dbcGemProperty.LookupEntry( GemProto->GemProperties );
	
			if( gp == NULL )
				continue;

			if( !(gp->SocketMaskMustMatch == 0 || ( gp->SocketMaskMustMatch & SocketColor ))
//				|| !(gp->SocketMask & SocketColor)
				)
				continue;

			if( !gp->EnchantmentID )//this is ok in few cases
				continue;

			found_slot = true;
			if( GemItem->GetStackCount() == 1 )
			{
				GemItem = itemi->SafeRemoveAndRetreiveItemByGuid(gemguid,true);
				if( !GemItem ) 
					return; //someone sending hacked packets to crash server
				GemProto = GemItem->GetProto();

				GemItem->DeleteMe();
				GemItem = NULL;
			}
			else
			{
				GemProto = GemItem->GetProto();
				GemItem->SetStackCount( GemItem->GetStackCount() - 1 );
			}
				  
			EnchantmentInstance *EI = TargetItem->GetEnchantment(SocketIndexToGem);
			if( EI )//replace gem
				TargetItem->RemoveEnchantment(SocketIndexToGem);//remove previous

			Enchantment = dbcEnchant.LookupEntry(gp->EnchantmentID);
			if(Enchantment && TargetItem->GetProto()->SubClass != ITEM_SUBCLASS_WEAPON_THROWN)
				TargetItem->AddEnchantment(Enchantment, 0, true,apply,false,SocketIndexToGem);
//			break;
		}
	}
	//Add color match bonus
	if(TargetItem->GetProto()->SocketBonus)
	{
		//check for color match
		bool ColorMatch = true;
		uint32 FilledSlots=0;
		for(uint32 i=0; i<ItemSocketCount; i++)
		{
			EnchantmentInstance * EI= TargetItem->GetEnchantment(ITEM_ENCHANT_SLOT_GEM1+i);
			if(EI)
			{
				FilledSlots++;
				ItemPrototype *GemProto = ItemPrototypeStorage.LookupEntry(EI->Enchantment->GemEntry);
				if(!GemProto)
					gp = 0;
				else
					gp = dbcGemProperty.LookupEntry(GemProto->GemProperties);
		
				if(gp && !(gp->SocketMask & TargetItem->GetProto()->Sockets.SocketColor[i]))
				{
					ColorMatch=false;
					break;
				}
			}
		}
		if(	ColorMatch && ( FilledSlots >= ItemSocketCount ) )
		{
			if( TargetItem->HasEnchantment( TargetItem->GetProto()->SocketBonus ) > 0 )
			{ 
				return;
			}

			Enchantment = dbcEnchant.LookupEntry( TargetItem->GetProto()->SocketBonus );
			if( Enchantment && TargetItem->GetProto()->SubClass != ITEM_SUBCLASS_WEAPON_THROWN )
			{
				uint32 Slot = TargetItem->FindFreeEnchantSlot(Enchantment,0);
				TargetItem->AddEnchantment( Enchantment, 0, true, apply, false, Slot );
			}
		}
		else //remove
			TargetItem->RemoveSocketBonusEnchant();
	}

	TargetItem->m_isDirty = true;
}

void WorldSession::HandleWrapItemOpcode( WorldPacket& recv_data )
{
	int8 sourceitem_bagslot, sourceitem_slot;
	int8 destitem_bagslot, destitem_slot;
	uint32 source_entry;
	uint32 itemid;
	Item * src,*dst;

	recv_data >> sourceitem_bagslot >> sourceitem_slot;
	recv_data >> destitem_bagslot >> destitem_slot;

	if( !_player->IsInWorld() )
	{ 
		return;
	}

	src = _player->GetItemInterface()->GetInventoryItem( sourceitem_bagslot, sourceitem_slot );
	dst = _player->GetItemInterface()->GetInventoryItem( destitem_bagslot, destitem_slot );

	if( !src || !dst )
	{ 
		return;
	}

	if(src == dst || !(src->GetProto()->Class == 0 && src->GetProto()->SubClass == 8))
	{
		_player->GetItemInterface()->BuildInventoryChangeError( src, dst, INV_ERR_WRAPPED_CANT_BE_WRAPPED );
		return;
	}

	if( dst->GetUInt32Value( ITEM_FIELD_STACK_COUNT ) > 1 )
	{
		_player->GetItemInterface()->BuildInventoryChangeError( src, dst, INV_ERR_STACKABLE_CANT_BE_WRAPPED );
		return;
	}

	if( dst->GetProto()->MaxCount > 1 )
	{
		_player->GetItemInterface()->BuildInventoryChangeError( src, dst, INV_ERR_STACKABLE_CANT_BE_WRAPPED );
		return;
	}

	if( dst->IsSoulbound() || dst->GetProto()->Bonding != 0)
	{
		_player->GetItemInterface()->BuildInventoryChangeError( src, dst, INV_ERR_BOUND_CANT_BE_WRAPPED );
		return;
	}

	if( dst->wrapped_item_id || src->wrapped_item_id )
	{
		_player->GetItemInterface()->BuildInventoryChangeError( src, dst, INV_ERR_WRAPPED_CANT_BE_WRAPPED );
		return;
	}

	if( dst->GetProto()->Unique )
	{
		_player->GetItemInterface()->BuildInventoryChangeError( src, dst, INV_ERR_UNIQUE_CANT_BE_WRAPPED );
		return;
	}

	if( dst->IsContainer() )
	{
		_player->GetItemInterface()->BuildInventoryChangeError( src, dst, INV_ERR_BAGS_CANT_BE_WRAPPED );
		return;
	}

	if( dst->HasEnchantments() )
	{
		_player->GetItemInterface()->BuildInventoryChangeError( src, dst, INV_ERR_ITEM_LOCKED );
		return;
	}
	if( destitem_bagslot == (int8)0xFF && ( destitem_slot >= EQUIPMENT_SLOT_START && destitem_slot <= INVENTORY_SLOT_BAG_END ) )
	{
		_player->GetItemInterface()->BuildInventoryChangeError( src, dst, INV_ERR_EQUIPPED_CANT_BE_WRAPPED );
		return;
	}

	// all checks passed ok
	source_entry = src->GetEntry();
	itemid = source_entry;
	switch( source_entry )
	{
	case 5042:
		itemid = 5043;
		break;

	case 5048:
		itemid = 5044;
		break;

	case 17303:
		itemid = 17302;
		break;

	case 17304:
		itemid = 17305;
		break;

	case 17307:
		itemid = 17308;
		break;

	case 21830:
		itemid = 21831;
		break;

	default:
		_player->GetItemInterface()->BuildInventoryChangeError( src, dst, INV_ERR_WRAPPED_CANT_BE_WRAPPED );
		return;
		break;
	}

	dst->SetProto( src->GetProto() );

	if( src->GetUInt32Value( ITEM_FIELD_STACK_COUNT ) <= 1 )
	{
		// destroy the source item
		_player->GetItemInterface()->SafeFullRemoveItemByGuid( src->GetGUID() );
	}
	else
	{
		// reduce stack count by one
		src->ModUnsigned32Value( ITEM_FIELD_STACK_COUNT, -1 );
		src->m_isDirty = true;
	}

	// change the dest item's entry
	dst->wrapped_item_id = dst->GetEntry();
	dst->SetUInt32Value( OBJECT_FIELD_ENTRY, itemid );

	// set the giftwrapper fields
	dst->SetUInt32Value( ITEM_FIELD_GIFTCREATOR, _player->GetLowGUID() );
	dst->SetUInt32Value( ITEM_FIELD_DURABILITY, 0 );
	dst->SetUInt32Value( ITEM_FIELD_MAXDURABILITY, 0 );
	dst->SetUInt32Value( ITEM_FIELD_FLAGS, ITEM_FLAG_HEROIC | ITEM_FLAG_UNKNOWN_16 );

	// save it
	dst->m_isDirty = true;
	dst->SaveToDB( destitem_bagslot, destitem_slot, false, NULL );
}


void WorldSession::HandleEquipmentSetSave(WorldPacket &recv_data)
{
    sLog.outDebug("CMSG_EQUIPMENT_SET_SAVE");

	WoWGuid		packedGUID;
	uint64		unpackedGUID;
    uint32		index;
    std::string name;
    std::string iconName;

    recv_data >> packedGUID;
    recv_data >> index;

    if( index >= MAX_EQUIPMENT_SET_INDEX )                    // client set slots amount
	{
		if( index < MAX_EQUIPMENT_SET_INDEX_CLIENT )
			_player->BroadcastMessage( "Current item set count limitation Exceeded" );
		return;
	}

	EquipmentSet *eqSet = NULL;
	CommitPointerListNode<EquipmentSet>	*itr;
	_player->m_EquipmentSets.BeginLoop();
	uint32 active_set_count = 0;
	unpackedGUID = packedGUID.GetOldGuid();
    for(itr = _player->m_EquipmentSets.begin(); itr != _player->m_EquipmentSets.end(); itr = itr->Next() )
    {
		if( itr->data->Guid == unpackedGUID )
			eqSet = itr->data;
        ++active_set_count;                                            
    }
	if( active_set_count >= MAX_EQUIPMENT_SET_INDEX )
	{
		_player->BroadcastMessage( "Current item set count limitation Exceeded" );
		_player->m_EquipmentSets.EndLoopAndCommit();
		return;
	}
	if( eqSet == NULL )
	{
		eqSet = new EquipmentSet;
		unpackedGUID = objmgr.GenerateEquipmentSetGuid();
		_player->m_EquipmentSets.push_front( eqSet );
	}
	eqSet->need_save = true;
	eqSet->Guid = unpackedGUID;

    recv_data >> eqSet->Name;
    recv_data >> eqSet->IconName;

    for(uint32 i = 0; i < EQUIPMENT_SLOT_END; ++i)
    {
        recv_data >> packedGUID;
        eqSet->ItemGUIDS[i] = packedGUID.GetOldGuid();
    }
	_player->m_EquipmentSets.EndLoopAndCommit();

	sStackWorldPacket( data, SMSG_EQUIPMENT_SET_SAVED, 4 + 9 + 1 );
	data << uint32(index);
	FastGUIDPack( data, eqSet->Guid );
	SendPacket(&data);
}

void WorldSession::HandleEquipmentSetDelete(WorldPacket &recv_data)
{
    sLog.outDebug("CMSG_EQUIPMENT_SET_DELETE");
    WoWGuid		packedGUID;
    recv_data >> packedGUID;
    _player->DeleteEquipmentSet( packedGUID.GetOldGuid() );
}

void WorldSession::HandleEquipmentSetUse(WorldPacket &recv_data)
{
    sLog.outDebug("CMSG_EQUIPMENT_SET_USE");

    for(uint32 item_slot = 0; item_slot < EQUIPMENT_SLOT_END; ++item_slot)
    {
		WoWGuid		packedGUID;
		uint64		unpackedGUID;
        uint8		srcbag, srcslot;

        recv_data >> packedGUID;
        recv_data >> srcbag >> srcslot;

		unpackedGUID = packedGUID.GetOldGuid();
        sLog.outDebug("Item (%u): srcbag %u, srcslot %u", (uint32)unpackedGUID, srcbag, srcslot);

		//special value used by client to be able to mark skipped slots
		if( unpackedGUID == 1 )
			continue;

        Item *new_item = NULL;
		
		if( unpackedGUID )
			new_item = _player->GetItemInterface()->GetItemByGUID( unpackedGUID );

        if(!new_item)
            new_item = _player->GetItemInterface()->GetInventoryItem( srcbag, srcslot );

		Item *equipped_item = _player->GetItemInterface()->GetInventoryItem( item_slot );

		//no change is required. Slot is either empty in both setups or has the same item
		if( new_item == equipped_item )
			continue;

		//remove the 2 items
		if( new_item )
			new_item = _player->GetItemInterface()->SafeRemoveAndRetreiveItemByGuid( new_item->GetGUID(), false );
		if( equipped_item )
			equipped_item = _player->GetItemInterface()->SafeRemoveAndRetreiveItemByGuid( equipped_item->GetGUID(), false );

		//try to make the swap, if not then put it back from where it came if possible. Worst case it gets mailed
		if( new_item )
		{
			int8 resultEQ = _player->GetItemInterface()->CanEquipItemInSlot2( INVENTORY_SLOT_NOT_SET, item_slot, new_item );
			AddItemResult result;
			if( resultEQ == INV_ERR_OK )
				result = _player->GetItemInterface()->SafeAddItem( new_item, INVENTORY_SLOT_NOT_SET, item_slot );
			else
				result = ADD_ITEM_RESULT_ERROR;
			if( resultEQ != INV_ERR_OK || result != ADD_ITEM_RESULT_OK )
			{
				//try to put it back in inventory
				result = _player->GetItemInterface()->AddItemToFreeSlot( &new_item );
				//wow, just mail it to the player
				if( new_item && result == ADD_ITEM_RESULT_ERROR )
					_player->RemoveAndMailItem( &new_item );
			}
		}
		if( equipped_item )
		{
			AddItemResult result =_player->GetItemInterface()->AddItemToFreeSlot( &equipped_item );
			//wow, just mail it to the player
			if( equipped_item && result == ADD_ITEM_RESULT_ERROR )
				_player->RemoveAndMailItem( &equipped_item );
		}
    }
    sStackWorldPacket( data, SMSG_EQUIPMENT_SET_USE_RESULT, 2 );
    data << uint8(0);                                       // 4 - equipment swap failed - inventory is full
    SendPacket(&data);
}

void WorldSession::HandleItemRefundInfoRequest(WorldPacket& recv_data)
{
    sLog.outDebug("WORLD: CMSG_ITEM_REFUND_INFO");

    uint64 guid;
    recv_data >> guid;                                      // item guid

    Item *item = _player->GetItemInterface()->GetItemByGUID( guid );
    if (!item)
    {
        sLog.outDebug("Item refund: item not found!");
        return;
    }

	sLog.outDebug("!! Item refunding not supported atm !!");
//    GetPlayer()->SendRefundInfo(item);
}

void WorldSession::HandleItemRefund(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_ITEM_REFUND");
    uint64 guid;
    recv_data >> guid;                                      // item guid

    Item *item = _player->GetItemInterface()->GetItemByGUID(guid);
    if (!item)
    {
        sLog.outDebug("Item refund: item not found!");
        return;
    }

	sLog.outDebug("!! Item refunding not supported atm !!");
//    GetPlayer()->RefundItem(item);
} 

void WorldSession::SendReforgeWindowOpen(uint64 reforgerGUID)
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
/*
55 6C 00 00 65 B4 30 F1 real guid 
F3 55 6C 65 B4 30 F1 cguid
54 6D 00 00 64 B5 31 F0
{SERVER} Packet: (0xDB23) UNKNOWN PacketSize = 7 TimeStamp = 17299668
EB B5 64 54 6D F0 31 
M  5  4  0  1  7  6
*/
	WorldPacket data(8);
	data.Initialize( SMSG_REFORGING_WINDOW_OPEN );
	uint8 guid_mask = 0;
	uint8 *guid_bytes = (uint8 *)&reforgerGUID;
/*	if( guid_bytes[0] )
		guid_mask |= BIT_0;
	if( guid_bytes[1] )
		guid_mask |= BIT_0;
	if( guid_bytes[2] )
		guid_mask |= BIT_0;
	if( guid_bytes[3] )
		guid_mask |= BIT_0;
	if( guid_bytes[4] )
		guid_mask |= BIT_0;
	if( guid_bytes[5] )
		guid_mask |= BIT_0;
	if( guid_bytes[6] )
		guid_mask |= BIT_0; */

	guid_mask = 0xEB;
	data << guid_mask;
//	if( guid_bytes[5] )
		data << ObfuscateByte( guid_bytes[5] );
//	if( guid_bytes[4] )
		data << ObfuscateByte( guid_bytes[4] );
//	if( guid_bytes[0] )
		data << ObfuscateByte( guid_bytes[0] );
//	if( guid_bytes[1] )
		data << ObfuscateByte( guid_bytes[1] );
//	if( guid_bytes[7] )
		data << ObfuscateByte( guid_bytes[7] );
//	if( guid_bytes[6] )
		data << ObfuscateByte( guid_bytes[6] );
	SendPacket( &data );
} 

void WorldSession::HandleReforgeItem(WorldPacket &recv_data)
{
	if(_player == NULL || !_player->IsInWorld()) 
	{ 
		return;
	}
/*
13329
{CLIENT} Packet: (0x4117) UNKNOWN PacketSize = 20 TimeStamp = 51406728
26 41 00 00 65 B4 30 F1 - reforger NPC
FF 00 00 00 - bagslot 
04 00 00 00 - item slot
86 00 00 00 - something about refoge - 134 
{CLIENT} Packet: (0x4117) UNKNOWN PacketSize = 20 TimeStamp = 60195388
26 41 00 00 65 B4 30 F1 
16 00 00 00 - slot of bag
04 00 00 00 - slot in bag
8E 00 00 00 - 142 
ITEM_FIELD_ENCHANTMENT_9_1 stores reforge value

14333
{CLIENT} Packet: (0x2869) UNKNOWN PacketSize = 19 TimeStamp = 17338278
7E guid mask
13 00 00 00 
00 00 00 00 
9A 00 00 00 
64 F0 6D 54 31 B5 

*/
	uint8 guid_mask;
//	uint64 obfuscated_compressed_guid;
	int32 bagslot;
	int32 itemslot;
	uint32 reforgeid;

	recv_data >> guid_mask;
	recv_data >> bagslot;
	recv_data >> itemslot;
	recv_data >> reforgeid;
///	recv_data >> obfuscated_compressed_guid;	//this is bitmask based !

	//cause every frikkin client version messed this up in he's own way
	if( bagslot <= INVENTORY_SLOT_BAG_START || bagslot >= INVENTORY_SLOT_BAG_END )
		bagslot = INVENTORY_SLOT_NOT_SET;

	Item *dst_item = _player->GetItemInterface()->GetInventoryItem( bagslot, itemslot );

	if( dst_item )
	{
		//check for proper item
		if( dst_item->GetProto()->ItemLevel < 200 )
		{
			_player->GetItemInterface()->BuildInventoryChangeError( 0, 0, INV_ERR_PURCHASE_LEVEL_TOO_LOW );
			return;
		}

		//check if we have enough gold for action
		int32 cost = dst_item->GetProto()->SellPrice;
		if( dst_item->GetProto()->SellPrice > _player->GetGold() )
		{
			_player->GetItemInterface()->BuildInventoryChangeError( 0, 0, INV_ERR_NOT_ENOUGH_MONEY );
			return;
		}
		_player->ModGold( -cost );

		if( bagslot <= INVENTORY_SLOT_NOT_SET && itemslot < EQUIPMENT_SLOT_END )
			dst_item->ApplyReforging( false );
		dst_item->SetReforgeID( reforgeid );
		if( bagslot <= INVENTORY_SLOT_NOT_SET && itemslot < EQUIPMENT_SLOT_END )
			dst_item->ApplyReforging( true );

		//if we got here we can signal success
		WorldPacket data(8);
		data.Initialize( SMSG_REFORGING_REFORGE_FEEDBACK );
		data << uint8(1);
		SendPacket( &data );
	}
} 