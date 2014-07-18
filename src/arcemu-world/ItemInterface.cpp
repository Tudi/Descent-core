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
#include "Item.h"
#include "Container.h"
#include "ItemPrototype.h"
#include "Player.h"
//
//-------------------------------------------------------------------//
ItemInterface::ItemInterface( Player *pPlayer )
{
	m_pOwner = pPlayer;
	memset(&m_pItems[0], 0, sizeof(Item*)*MAX_INVENTORY_SLOT);
	memset(&m_pBuyBack[0], 0, sizeof(Item*)*MAX_BUYBACK_SLOT);
}

//-------------------------------------------------------------------//
ItemInterface::~ItemInterface()
{
	for(int i = 0; i < MAX_INVENTORY_SLOT; i++)
	{
		if( m_pItems[i] != NULL )
		{
			//wtf fix is this ?
			if( m_pItems[i]->GetOwner() != m_pOwner )
				continue;
			m_pItems[i]->DeleteMe();
			m_pItems[i] = NULL;
		}
	}
}

void ItemInterface::EventItemAdd(Item *pItem)
{
	if( pItem == NULL )
		return;
	uint32 stack_to_add = pItem->GetUInt32Value(ITEM_FIELD_STACK_COUNT) ? pItem->GetUInt32Value(ITEM_FIELD_STACK_COUNT) : 1;

	m_pOwner->Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_OWN_ITEM,pItem->GetUInt32Value(OBJECT_FIELD_ENTRY),ACHIEVEMENT_UNUSED_FIELD_VALUE, stack_to_add, ACHIEVEMENT_EVENT_ACTION_ADD);
	if( pItem->GetProto()->Quality >= ITEM_QUALITY_EPIC_PURPLE )
		m_pOwner->Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_RECEIVE_EPIC_ITEM,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE, stack_to_add, ACHIEVEMENT_EVENT_ACTION_ADD);

	//these should be scripted to the bone
	for( int k = 0; k < 5; k++ )
	{
		// stupid fucked dbs
		if( pItem->GetProto()->Spells.Id[k] == 0 )
			continue;

		if( pItem->GetProto()->Spells.Trigger[k] == ON_PICKUP_QUEST )
		{
			SpellEntry* spells = dbcSpell.LookupEntry( pItem->GetProto()->Spells.Id[k] );
			if( spells )
			{
				Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
				spell->Init( m_pOwner, spells ,true, NULL );
				SpellCastTargets targets;
				targets.m_unitTarget = m_pOwner->GetGUID();
				spell->castedItemId = pItem->GetEntry();
				spell->prepare( &targets );
			}
		}
	}
}

void ItemInterface::EventItemRemove(Item *pItem)
{
	if( pItem == NULL )
		return;

	if( m_pOwner == NULL )
		return;

	//these should be scripted to the bone
	for( int k = 0; k < 5; k++ )
	{
		// stupid fucked dbs
		if( pItem->GetProto()->Spells.Id[k] == 0 )
			continue;
		if( pItem->GetProto()->Spells.Trigger[k] == ON_PICKUP_QUEST )
			m_pOwner->RemoveAura( pItem->GetProto()->Spells.Id[k] ); 
	}
}

//-------------------------------------------------------------------// 100%
uint32 ItemInterface::m_CreateForPlayer(ByteBuffer *data)
{
	ASSERT(m_pOwner != NULL);
	uint32 count = 0;

	for(int i = 0; i < MAX_INVENTORY_SLOT; i++)
	{
		if(m_pItems[i])
		{
			if(m_pItems[i]->IsContainer())
			{
				count += SafeContainerCast(m_pItems[i])->BuildCreateUpdateBlockForPlayer(data, m_pOwner);

				if(m_pItems[i]->GetProto())
				{
					for(uint32 e=0; e < m_pItems[i]->GetProto()->ContainerSlots; e++)
					{
						Item *pItem = SafeContainerCast(m_pItems[i])->GetItem(e);
						if(pItem)
						{
							if(pItem->IsContainer())
							{
								count += SafeContainerCast(pItem)->BuildCreateUpdateBlockForPlayer( data, m_pOwner );
							}
							else
							{
								count += pItem->BuildCreateUpdateBlockForPlayer( data, m_pOwner );
							}
						}
					}
				}
			}
			else
			{
				count += m_pItems[i]->BuildCreateUpdateBlockForPlayer(data, m_pOwner);
			}
		}
	}
	return count;
}

//-------------------------------------------------------------------// 100%
void ItemInterface::m_DestroyForPlayer()
{
	ASSERT(m_pOwner != NULL);

	for(int i = 0; i < MAX_INVENTORY_SLOT; i++)
	{
		if(m_pItems[i])
		{
			if(m_pItems[i]->IsContainer())
			{
				if(m_pItems[i]->GetProto())
				{
					for(uint32 e=0; e < m_pItems[i]->GetProto()->ContainerSlots; e++)
					{
						Item *pItem = SafeContainerCast((m_pItems[i]))->GetItem(e);
						if(pItem)
						{
							if(pItem->IsContainer())
							{
//								SafeContainerCast((pItem))->DestroyForPlayer( m_pOwner );
								m_pOwner->PushOutOfRange( pItem->GetGUID() );
							}
							else
							{
//								pItem->DestroyForPlayer( m_pOwner );
								m_pOwner->PushOutOfRange( pItem->GetGUID() );
							}
						}
					}
				}
				SafeContainerCast((m_pItems[i]))->DestroyForPlayer( m_pOwner );
			}
			else
			{
				m_pItems[i]->DestroyForPlayer( m_pOwner );
			}
		}
	}
}

//-------------------------------------------------------------------//
//Description: Creates and adds a item that can be manipulated after		  
//-------------------------------------------------------------------//
Item *ItemInterface::SafeAddItem(uint32 ItemId, int8 ContainerSlot, int16 slot)
{
	Item *pItem;
	ItemPrototype *pProto = ItemPrototypeStorage.LookupEntry(ItemId);
	if(!pProto) { return NULL; }

	if(pProto->InventoryType == INVTYPE_BAG)
	{
		pItem = (Item*)new Container(HIGHGUID_TYPE_CONTAINER,objmgr.GenerateLowGuid(HIGHGUID_TYPE_CONTAINER));
		SafeContainerCast(pItem)->Create( ItemId, m_pOwner);
		if(m_AddItem(pItem, ContainerSlot, slot))
		{
			return pItem;
		}
		else
		{
			pItem->DeleteMe();
			return NULL;
		}
	}
	else
	{
		pItem = ItemPool.PooledNew( __FILE__, __LINE__ );
		pItem->Init(HIGHGUID_TYPE_ITEM,objmgr.GenerateLowGuid(HIGHGUID_TYPE_ITEM));
		pItem->Create( ItemId, m_pOwner);
		if(m_AddItem(pItem, ContainerSlot, slot))
		{
			return pItem;
		}
		else
		{
			ItemPool.PooledDelete( pItem, __FILE__, __LINE__ );
			return NULL;
		}
	}
}

//-------------------------------------------------------------------//
//Description: Creates and adds a item that can be manipulated after		  
//-------------------------------------------------------------------//
AddItemResult ItemInterface::SafeAddItem(Item *pItem, int8 ContainerSlot, int16 slot)
{
	return m_AddItem( pItem, ContainerSlot, slot );
}

//-------------------------------------------------------------------//
//Description: adds items to player inventory, this includes all types of slots.		  
//-------------------------------------------------------------------//
AddItemResult ItemInterface::m_AddItem(Item *item, int8 ContainerSlot, int16 slot)
{
	if ( slot >= MAX_INVENTORY_SLOT )
	{
		sLog.outString("%s: slot (%d) >= MAX_INVENTORY_SLOT (%d)", __FUNCTION__, slot, MAX_INVENTORY_SLOT);
		return ADD_ITEM_RESULT_ERROR;
	}
	if ( ContainerSlot >= MAX_INVENTORY_SLOT )
	{
		sLog.outString("%s: ContainerSlot (%d) >= MAX_INVENTORY_SLOT (%d)", __FUNCTION__, ContainerSlot, MAX_INVENTORY_SLOT);
		return ADD_ITEM_RESULT_ERROR;
	}

	if( item == NULL || !item->GetProto() )
	{ 
		return ADD_ITEM_RESULT_ERROR;
	}

	if( slot < 0 )
	{
		result = this->FindFreeInventorySlot(item->GetProto());
		if( result.Result )
		{
			slot = result.Slot;
			ContainerSlot = result.ContainerSlot;
		}
		else
			return ADD_ITEM_RESULT_ERROR;
	}

	item->m_isDirty = true;

	// doublechecking
	uint32 i, j, k;
	Item * tempitem;
	for(i = 0; i < MAX_INVENTORY_SLOT; ++i)
	{
		tempitem = m_pItems[i];
		if( tempitem != NULL )
		{
			if( tempitem == item )
			{
#if (defined( WIN32 ) || defined( WIN64 ) )
				//OutputCrashLogLine("item duplication, callstack:");
				//printf("item duplication, callstack: ");
				//CStackWalker ws;
				//ws.ShowCallstack();
#endif
				return ADD_ITEM_RESULT_DUPLICATED;
			}

			if( tempitem->IsContainer() )
			{
				k = tempitem->GetProto()->ContainerSlots;
				for(j = 0; j < k; ++j)
				{
					if( SafeContainerCast(tempitem)->GetItem( j ) == item )
					{
#if (defined( WIN32 ) || defined( WIN64 ) )
						//OutputCrashLogLine("item duplication in container, callstack:");
						//printf("item duplication in container, callstack: ");
						//CStackWalker ws;
						//ws.ShowCallstack();
#endif
						return ADD_ITEM_RESULT_DUPLICATED;
					}
				}
			}
		}
	}

	if(item->GetProto())
	{
		//case 1, item is from backpack container
		if(ContainerSlot == INVENTORY_SLOT_NOT_SET)
		{
			//ASSERT(m_pItems[slot] == NULL);
			if( GetInventoryItem(slot) != NULL 
				//this would be only required when we login to avoid item swaps accidentally
				//|| CanEquipItemInSlot(ContainerSlot,slot,item->GetProto(),true,false) != INV_ERR_OK
				//|| (slot == EQUIPMENT_SLOT_OFFHAND && !m_pOwner->HasSkillLine(118))
				)
			{
				//sLog.outError("bugged inventory: %u %u", m_pOwner->GetName(), item->GetGUID());
				SlotResult result = this->FindFreeInventorySlot(item->GetProto());
				
				// send message to player
				sChatHandler.BlueSystemMessageToPlr(m_pOwner, "A duplicated item, `%s` was found in your inventory. We've attempted to add it to a free slot in your inventory, if there is none this will fail. It will be attempted again the next time you log on.",
					item->GetProto()->Name1);
				if(result.Result == true)
				{
					// Found a new slot for that item.
					slot = result.Slot;
					ContainerSlot = result.ContainerSlot;
				}
				else
				{
					// don't leak memory! -> we are not, function that called us should handle return values properly
					/*if(item->IsContainer())
						delete  SafeContainerCast(item);
					else
						delete item;*/

					return ADD_ITEM_RESULT_ERROR;
				}
			}

			if(!GetInventoryItem(slot)) //slot is free, add item.
			{
				item->SetOwner( m_pOwner );
				item->SetUInt64Value(ITEM_FIELD_CONTAINED, m_pOwner->GetGUID());
				m_pItems[(int)slot] = item;

				if (item->GetProto()->Bonding == ITEM_BIND_ON_PICKUP) 
				{
					if(item->GetProto()->Flags & ITEM_FLAG_ACCOUNTBOUND) // don't "Soulbind" account-bound items
						item->AccountBind();
					else
						item->SoulBind();
				}

				if( m_pOwner->IsInWorld() && !item->IsInWorld())
				{
					//item->AddToWorld();
					item->PushToWorld(m_pOwner->GetMapMgr());
					ByteBuffer buf(2500);
					uint32 count = item->BuildCreateUpdateBlockForPlayer( &buf, m_pOwner );
					m_pOwner->PushCreationData( &buf, count, item );
				}
				m_pOwner->SetUInt64Value(PLAYER_FIELD_INV_SLOT_HEAD + (slot*2), item->GetGUID());
			}
			else
			{
				return ADD_ITEM_RESULT_ERROR;
			}
		}
		else //case 2: item is from a bag container
		{
			if( GetInventoryItem(ContainerSlot) && GetInventoryItem(ContainerSlot)->IsContainer() &&
				slot < (int32)GetInventoryItem(ContainerSlot)->GetProto()->ContainerSlots) //container exists
			{
				bool result = SafeContainerCast(m_pItems[(int)ContainerSlot])->AddItem(slot, item);
				if( !result )
				{
					return ADD_ITEM_RESULT_ERROR;
				}
			}
			else
			{
				item->DeleteFromDB(); //wpe dupefix ..we dont want it reappearing on the next relog now do we?
				return ADD_ITEM_RESULT_ERROR;
			}
		}
	}
	else
	{
		return ADD_ITEM_RESULT_ERROR;
	}

	if ( slot < EQUIPMENT_SLOT_END && ContainerSlot == INVENTORY_SLOT_NOT_SET )
	{
		int VisibleBase = PLAYER_VISIBLE_ITEM_1_ENTRYID + (slot * (PLAYER_VISIBLE_ITEM_2_ENTRYID - PLAYER_VISIBLE_ITEM_1_ENTRYID));
		if( VisibleBase > PLAYER_VISIBLE_ITEM_19_ENTRYID )
		{
			printf("Slot warning: slot: %d\n", slot);
			OutputCrashLogLine("Slot warning: slot: %d\n", slot);
		}
		else
		{
			m_pOwner->SetUInt32Value( VisibleBase, item->GetUInt32Value( OBJECT_FIELD_ENTRY ) );
			uint32 enchant_val;
			enchant_val = item->GetUInt32Value( ITEM_FIELD_ENCHANTMENT_1_1 ) | (item->GetUInt32Value( ITEM_FIELD_ENCHANTMENT_1_2 ) << 16 );
			m_pOwner->SetUInt32Value( VisibleBase + 1, enchant_val );
/*			m_pOwner->SetUInt32Value( VisibleBase + 2, item->GetUInt32Value( ITEM_FIELD_ENCHANTMENT_1_1 + 3 ) );
			m_pOwner->SetUInt32Value( VisibleBase + 3, item->GetUInt32Value( ITEM_FIELD_ENCHANTMENT_1_1 + 6 ) );
			m_pOwner->SetUInt32Value( VisibleBase + 4, item->GetUInt32Value( ITEM_FIELD_ENCHANTMENT_1_1 + 9 ) );
			m_pOwner->SetUInt32Value( VisibleBase + 5, item->GetUInt32Value( ITEM_FIELD_ENCHANTMENT_1_1 + 12 ) );
			m_pOwner->SetUInt32Value( VisibleBase + 6, item->GetUInt32Value( ITEM_FIELD_ENCHANTMENT_1_1 + 15 ) );
			m_pOwner->SetUInt32Value( VisibleBase + 7, item->GetUInt32Value( ITEM_FIELD_ENCHANTMENT_1_1 + 18 ) );
			m_pOwner->SetUInt32Value( VisibleBase + 8, item->GetUInt32Value( ITEM_FIELD_RANDOM_PROPERTIES_ID ) ); */
		}
	}

	if( m_pOwner->IsInWorld() && slot < INVENTORY_SLOT_BAG_END && ContainerSlot == INVENTORY_SLOT_NOT_SET )
	{
		m_pOwner->ApplyItemMods( item, slot, true );
	}
	sHookInterface.OnAddRemovePlayerItem( m_pOwner, item, true );

	if( ContainerSlot == INVENTORY_SLOT_NOT_SET && slot == EQUIPMENT_SLOT_OFFHAND && item->GetProto()->Class == ITEM_CLASS_WEAPON )
		m_pOwner->SetDualWield( true );

	EventItemAdd( item );

	return ADD_ITEM_RESULT_OK;
}

//-------------------------------------------------------------------//
//Description: Checks if the slot is a Bag slot
//-------------------------------------------------------------------//
bool ItemInterface::IsBagSlot(int16 slot)
{
	if((slot >= INVENTORY_SLOT_BAG_START && slot < INVENTORY_SLOT_BAG_END) || (slot >= BANK_SLOT_BAG_START && slot < BANK_SLOT_BAG_END))
	{
		return true;
	}
	return false;
}

//-------------------------------------------------------------------//
//Description: removes the item safely and returns it back for usage
//-------------------------------------------------------------------//
Item *ItemInterface::SafeRemoveAndRetreiveItemFromSlot(int8 ContainerSlot, int16 slot, bool destroy)
{
	ASSERT(slot < MAX_INVENTORY_SLOT);
	ASSERT(ContainerSlot < MAX_INVENTORY_SLOT);
	Item *pItem = NULL;

	if(ContainerSlot == INVENTORY_SLOT_NOT_SET)
	{
		pItem = GetInventoryItem(ContainerSlot,slot);

		if (pItem == NULL) 
		{ 
			return NULL; 
		}

		if(pItem->GetProto()->ContainerSlots > 0 && pItem->IsContainer() && SafeContainerCast(pItem)->HasItems())
		{
			/* sounds weird? no. this will trigger a callstack display due to my other debug code. */
			pItem->DeleteFromDB();
			return NULL;
		}

		m_pItems[(int)slot] = NULL;

		EventItemRemove( pItem );

		if(pItem->GetOwner() == m_pOwner)
		{
			pItem->m_isDirty = true;

			m_pOwner->SetUInt64Value(PLAYER_FIELD_INV_SLOT_HEAD  + (slot*2), 0 );

			if ( slot < EQUIPMENT_SLOT_END )
			{
				m_pOwner->ApplyItemMods( pItem, slot, false );
				int VisibleBase = PLAYER_VISIBLE_ITEM_1_ENTRYID + (slot * (PLAYER_VISIBLE_ITEM_2_ENTRYID - PLAYER_VISIBLE_ITEM_1_ENTRYID));
				for (int i = VisibleBase; i < VisibleBase + PLAYER_VISIBLE_ITEM_2_ENTRYID - PLAYER_VISIBLE_ITEM_1_ENTRYID; ++i)
					m_pOwner->SetUInt32Value(i, 0);
			}
			sHookInterface.OnAddRemovePlayerItem( m_pOwner, pItem, false );

			if(slot == EQUIPMENT_SLOT_OFFHAND)
				m_pOwner->SetDualWield( false );

			if(destroy)
			{
				if (pItem->IsInWorld())
				{
					pItem->RemoveFromWorld();
				}
				pItem->DeleteFromDB();
			}
		}
		else
		{
			//wtf ?
			EventItemRemove( pItem );
			pItem = NULL;
		}
	}
	else if( IsBagSlot( ContainerSlot ) )
	{
		Item *pContainer = GetInventoryItem(ContainerSlot);
		if(pContainer && pContainer->IsContainer())
			pItem = SafeContainerCast(pContainer)->SafeRemoveAndRetreiveItemFromSlot(slot, destroy);
	}

	return pItem;
}

//-------------------------------------------------------------------//
//Description: removes the item safely by guid and returns it back for usage, supports full inventory
//-------------------------------------------------------------------//
Item *ItemInterface::SafeRemoveAndRetreiveItemByGuid(uint64 guid, bool destroy)
{
	int16 i = 0;

	for(i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; i++)
	{
		Item *item = GetInventoryItem(i);
		if (item && item->GetGUID() == guid)
		{
			return this->SafeRemoveAndRetreiveItemFromSlot(INVENTORY_SLOT_NOT_SET, i, destroy);
		}
	}

	for(i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; i++)
	{
		Item *item = GetInventoryItem(i);
		if (item && item->GetGUID() == guid)
		{
			return this->SafeRemoveAndRetreiveItemFromSlot(INVENTORY_SLOT_NOT_SET, i, destroy);
		}
	}

/*	for(i = INVENTORY_KEYRING_START; i < INVENTORY_KEYRING_END; i++)
	{
		Item *item = GetInventoryItem(i);

		if (item && item->GetGUID() == guid)
		{
			return this->SafeRemoveAndRetreiveItemFromSlot(INVENTORY_SLOT_NOT_SET, i, destroy);
		}
	}*/

	for(i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
	{
		Item *item = GetInventoryItem(i);
		if(item && item->GetGUID() == guid)
		{
			return this->SafeRemoveAndRetreiveItemFromSlot(INVENTORY_SLOT_NOT_SET, i, destroy);
		}
		else
		{
			if(item && item->IsContainer() && item->GetProto())
			{
				for (uint32 j =0; j < item->GetProto()->ContainerSlots; j++)
				{
					Item *item2 = SafeContainerCast(item)->GetItem(j);
					if (item2 && item2->GetGUID() == guid)
					{
						return SafeContainerCast(item)->SafeRemoveAndRetreiveItemFromSlot(j, destroy);
					}
				}
			}
		}
	}

	for(i = BANK_SLOT_ITEM_START; i < BANK_SLOT_ITEM_END; i++)
	{
		Item *item = GetInventoryItem(i);
		if (item && item->GetGUID() == guid)
		{
			return this->SafeRemoveAndRetreiveItemFromSlot(INVENTORY_SLOT_NOT_SET, i, destroy);
		}
	}

	for(i = BANK_SLOT_BAG_START; i < BANK_SLOT_BAG_END; i++)
	{
		Item *item = GetInventoryItem(i);
		if (item && item->GetGUID() == guid)
		{
			return this->SafeRemoveAndRetreiveItemFromSlot(INVENTORY_SLOT_NOT_SET, i, destroy);
		}
		else
		{
			if(item && item->IsContainer() && item->GetProto())
			{
				for (uint32 j =0; j < item->GetProto()->ContainerSlots; j++)
				{
					Item *item2 = SafeContainerCast(item)->GetItem(j);
					if (item2 && item2->GetGUID() == guid)
					{
						return SafeContainerCast(item)->SafeRemoveAndRetreiveItemFromSlot(j, destroy);
					}
				}
			}
		}
	}

	return NULL;
}

//-------------------------------------------------------------------//
//Description: completely removes item from player
//Result: true if item removal was succefull
//-------------------------------------------------------------------//
bool ItemInterface::SafeFullRemoveItemFromSlot(int8 ContainerSlot, int16 slot)
{
	ASSERT(slot < MAX_INVENTORY_SLOT);
	ASSERT(ContainerSlot < MAX_INVENTORY_SLOT);

	if(ContainerSlot == INVENTORY_SLOT_NOT_SET)
	{
		Item *pItem = GetInventoryItem(slot);

		if (pItem == NULL) { return false; }

		if(pItem->GetProto()->ContainerSlots > 0 && pItem->IsContainer() && SafeContainerCast(pItem)->HasItems())
		{
			/* sounds weird? no. this will trigger a callstack display due to my other debug code. */
			pItem->DeleteFromDB();
			return false;
		}

		m_pItems[(int)slot] = NULL;

		EventItemRemove( pItem );

		// hacky crashfix
		if( pItem->GetOwner() == m_pOwner )
		{
			pItem->m_isDirty = true;

			m_pOwner->SetUInt64Value(PLAYER_FIELD_INV_SLOT_HEAD  + (slot*2), 0 );

			if ( slot < EQUIPMENT_SLOT_END )
			{
				m_pOwner->ApplyItemMods(pItem, slot, false );
				int VisibleBase = PLAYER_VISIBLE_ITEM_1_ENTRYID + (slot * (PLAYER_VISIBLE_ITEM_2_ENTRYID - PLAYER_VISIBLE_ITEM_1_ENTRYID));
//				for (int i = VisibleBase; i < VisibleBase + PLAYER_VISIBLE_ITEM_2_ENTRYID - PLAYER_VISIBLE_ITEM_1_ENTRYID; ++i)
					m_pOwner->SetUInt32Value(VisibleBase, 0); //entry
					m_pOwner->SetUInt32Value(VisibleBase + 1, 0); //enchant
			}

			sHookInterface.OnAddRemovePlayerItem( m_pOwner, pItem, false );

			if(slot == EQUIPMENT_SLOT_OFFHAND)
				m_pOwner->SetDualWield( false );

			if (pItem->IsInWorld())
			{
				pItem->RemoveFromWorld();
			}

			pItem->DeleteFromDB();
			ItemPool.PooledDelete( pItem, __FILE__, __LINE__ );
		}
	}
	else
	{
		Item *pContainer = GetInventoryItem(ContainerSlot);
		if(pContainer && pContainer->IsContainer())
		{
			SafeContainerCast(pContainer)->SafeFullRemoveItemFromSlot(slot);
		}
	}
	return true;
}

//-------------------------------------------------------------------//
//Description: removes the item safely by guid, supports full inventory
//-------------------------------------------------------------------//
bool ItemInterface::SafeFullRemoveItemByGuid(uint64 guid)
{
	int16 i = 0;

	for(i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; i++)
	{
		Item *item = GetInventoryItem(i);
		if (item && item->GetGUID() == guid)
		{
			return this->SafeFullRemoveItemFromSlot(INVENTORY_SLOT_NOT_SET, i);
		}
	}

	for(i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; i++)
	{
		Item *item = GetInventoryItem(i);
		if (item && item->GetGUID() == guid)
		{
			return this->SafeFullRemoveItemFromSlot(INVENTORY_SLOT_NOT_SET, i);
		}
	}

/*	for(i = INVENTORY_KEYRING_START; i < INVENTORY_KEYRING_END; i++)
	{
		Item *item = GetInventoryItem(i);

		if (item && item->GetGUID() == guid)
		{
			return this->SafeFullRemoveItemFromSlot(INVENTORY_SLOT_NOT_SET, i);
		}
	}*/

	for(i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
	{
		Item *item = GetInventoryItem(i);
		if(item && item->GetGUID() == guid)
		{
			return this->SafeFullRemoveItemFromSlot(INVENTORY_SLOT_NOT_SET, i);
		}
		else
		{
			if(item && item->IsContainer() && item->GetProto())
			{
				for (uint32 j =0; j < item->GetProto()->ContainerSlots; j++)
				{
					Item *item2 = SafeContainerCast(item)->GetItem(j);
					if (item2 && item2->GetGUID() == guid)
					{
						return SafeContainerCast(item)->SafeFullRemoveItemFromSlot(j);
					}
				}
			}
		}
	}

	for(i = BANK_SLOT_ITEM_START; i < BANK_SLOT_ITEM_END; i++)
	{
		Item *item = GetInventoryItem(i);

		if (item && item->GetGUID() == guid)
		{
			return this->SafeFullRemoveItemFromSlot(INVENTORY_SLOT_NOT_SET, i);
		}
	}

	for(i = BANK_SLOT_BAG_START; i < BANK_SLOT_BAG_END; i++)
	{
		Item *item = GetInventoryItem(i);
		if(item && item->GetGUID() == guid)
		{
			return this->SafeFullRemoveItemFromSlot(INVENTORY_SLOT_NOT_SET, i);
		}
		else
		{
			if(item && item->IsContainer() && item->GetProto())
			{
				for (uint32 j =0; j < item->GetProto()->ContainerSlots; j++)
				{
					Item *item2 = SafeContainerCast(item)->GetItem(j);
					if (item2 && item2->GetGUID() == guid)
					{
						return SafeContainerCast(item)->SafeFullRemoveItemFromSlot(j);
					}
				}
			}
		}
	}
	return false;
}

//-------------------------------------------------------------------//
//Description: Gets a item from Inventory
//-------------------------------------------------------------------//
Item *ItemInterface::GetInventoryItem(int16 slot)
{
	if(slot < 0 || slot >= MAX_INVENTORY_SLOT)
	{ 
		return NULL;
	}

	return m_pItems[(int)slot];
}

//-------------------------------------------------------------------//
//Description: Gets a Item from inventory or container
//-------------------------------------------------------------------//
Item *ItemInterface::GetInventoryItem( int8 ContainerSlot, int16 slot )
{
	if( ContainerSlot <= INVENTORY_SLOT_NOT_SET )
	{
		if( slot < 0 || slot >= MAX_INVENTORY_SLOT )
		{ 
			return NULL;
		}

		return m_pItems[ (int)slot ];
	}
	else
	{
		if( IsBagSlot( ContainerSlot ) )
		{
			if( m_pItems[ (int)ContainerSlot ] )
			{
				return SafeContainerCast( m_pItems[ (int)ContainerSlot ] )->GetItem( slot );
			}
		}
	}
	return NULL;
}

//-------------------------------------------------------------------//
//Description: checks for stacks that didnt reached max capacity
//-------------------------------------------------------------------//
Item* ItemInterface::FindItemLessMax(uint32 itemid, uint32 cnt, bool IncBank)
{
	uint32 i = 0;
	for(i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; i++)
	{
		Item *item = GetInventoryItem(i);
		if (item)
		{
			if (item->GetProto())
			{
				if((item->GetEntry() == itemid && item->wrapped_item_id==0) && (item->GetProto()->MaxCount >= (item->GetUInt32Value(ITEM_FIELD_STACK_COUNT) + cnt)))
				{
					return item; 
				}
			}
			else
			{
				sLog.outError("%s: item with no proto :S entry=%d", __FUNCTION__, item->GetEntry());
			}
		}
	}

	for(i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
	{
		Item *item = GetInventoryItem(i);
		if(item && item->IsContainer())
		{
			  for (uint32 j =0; j < item->GetProto()->ContainerSlots; j++)
				{
					Item *item2 = SafeContainerCast(item)->GetItem(j);
					if (item2)
					{
						if((item2->GetProto()->ItemId == itemid && item2->wrapped_item_id==0) && (item2->GetProto()->MaxCount >= (item2->GetUInt32Value(ITEM_FIELD_STACK_COUNT) + cnt)))
						{
							return item2;
						}
					}
				}
			
		}
	}

	if(IncBank)
	{
		for(i = BANK_SLOT_ITEM_START; i < BANK_SLOT_ITEM_END; i++)
		{
			Item *item = GetInventoryItem(i);
			if (item)
			{
				if((item->GetEntry() == itemid && item->wrapped_item_id==0) && (item->GetProto()->MaxCount >= (item->GetUInt32Value(ITEM_FIELD_STACK_COUNT) + cnt)))
				{
					return item; 
				}
			}
		}

		for(i = BANK_SLOT_BAG_START; i < BANK_SLOT_BAG_END; i++)
		{
			Item *item = GetInventoryItem(i);
			if(item && item->IsContainer())
			{
			   
					for (uint32 j =0; j < item->GetProto()->ContainerSlots; j++)
					{
						Item *item2 = SafeContainerCast(item)->GetItem(j);
						if (item2)
						{
							if((item2->GetProto()->ItemId == itemid && item2->wrapped_item_id==0) && (item2->GetProto()->MaxCount >= (item2->GetUInt32Value(ITEM_FIELD_STACK_COUNT) + cnt)))
							{
								return item2;
							}
						}
					}
				
			}
		}
	}

	return NULL;
}

//-------------------------------------------------------------------//
//Description: finds item ammount on inventory, banks not included
//-------------------------------------------------------------------//
uint32 ItemInterface::GetItemCount(uint32 itemid, bool IncBank)
{
	uint32 cnt = 0;
	uint32 i = 0;
	for(i = EQUIPMENT_SLOT_START; i < INVENTORY_SLOT_ITEM_END; i++)
	{
		Item *item = GetInventoryItem(i);

		if (item)
		{
			if(item->GetEntry() == itemid && item->wrapped_item_id==0)
			{
				cnt += item->GetUInt32Value(ITEM_FIELD_STACK_COUNT) ? item->GetUInt32Value(ITEM_FIELD_STACK_COUNT) : 1; 
			}
		}
	}

	for(i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
	{
		Item *item = GetInventoryItem(i);
		if(item && item->IsContainer())
		{
				for (uint32 j =0; j < item->GetProto()->ContainerSlots;j++)
				{
					Item *item2 = SafeContainerCast(item)->GetItem(j);
					if (item2)
					{
						if (item2->GetEntry() == itemid && item->wrapped_item_id==0)
						{
							cnt += item2->GetUInt32Value(ITEM_FIELD_STACK_COUNT) ? item2->GetUInt32Value(ITEM_FIELD_STACK_COUNT) : 1; 
						}
					}
				}
			
		}
	}

/*	for(i = INVENTORY_KEYRING_START; i < INVENTORY_KEYRING_END; i++)
	{
		Item *item = GetInventoryItem(i);

		if (item)
		{
			if(item->GetProto()->ItemId == itemid && item->wrapped_item_id==0)
			{
				cnt += item->GetUInt32Value(ITEM_FIELD_STACK_COUNT) ? item->GetUInt32Value(ITEM_FIELD_STACK_COUNT) : 1;
			}
		}
	} */

	if(IncBank)
	{
		for(i = BANK_SLOT_ITEM_START; i < BANK_SLOT_BAG_END; i++)
		{
			Item *item = GetInventoryItem(i);
			if (item)
			{
				if(item->GetProto()->ItemId == itemid && item->wrapped_item_id==0)
				{
					cnt += item->GetUInt32Value(ITEM_FIELD_STACK_COUNT) ? item->GetUInt32Value(ITEM_FIELD_STACK_COUNT) : 1;
				}
			}
		}

		for(i = BANK_SLOT_BAG_START; i < BANK_SLOT_BAG_END; i++)
		{
			Item *item = GetInventoryItem(i);
			if(item)
			{
				if(item->IsContainer())
				{
					for (uint32 j =0; j < item->GetProto()->ContainerSlots; j++)
					{
						Item *item2 = SafeContainerCast(item)->GetItem(j);
						if (item2)
						{
							if(item2->GetProto()->ItemId == itemid && item->wrapped_item_id==0)
							{
								cnt += item2->GetUInt32Value(ITEM_FIELD_STACK_COUNT) ? item2->GetUInt32Value(ITEM_FIELD_STACK_COUNT) : 1;
							}
						}
					}
				}
			}
		}
	}
	return cnt;
}

//-------------------------------------------------------------------//
//Description: Removes a ammount of items from inventory
//-------------------------------------------------------------------//
uint32 ItemInterface::RemoveItemAmt(uint32 id, uint32 amt, bool IncBank)
{
	//this code returns shit return value is fucked
//	if (GetItemCount(id,IncBank) < amt)
	{
//		return 0;
	}
	uint32 i;

	for(i = EQUIPMENT_SLOT_START; i < INVENTORY_SLOT_ITEM_END; i++)
	{
		Item *item = GetInventoryItem(i);
		if (item)
		{
			if(item->GetEntry() == id && item->wrapped_item_id==0)
			{
				if(item->GetProto()->ContainerSlots > 0 && item->IsContainer() && SafeContainerCast(item)->HasItems())
				{
					/* sounds weird? no. this will trigger a callstack display due to my other debug code. */
					item->DeleteFromDB();
					continue;
				}

				if (item->GetUInt32Value(ITEM_FIELD_STACK_COUNT) > amt)
				{
					item->SetCount(item->GetUInt32Value(ITEM_FIELD_STACK_COUNT) - amt);
					item->m_isDirty = true;
					return amt;
				}
				else if (item->GetUInt32Value(ITEM_FIELD_STACK_COUNT)== amt)
				{
					bool result = SafeFullRemoveItemFromSlot(INVENTORY_SLOT_NOT_SET, i);
					if(result)
					{
						return amt;
					}
					else
					{
						return 0;
					}
				}
				else
				{
					amt -= item->GetUInt32Value(ITEM_FIELD_STACK_COUNT);
					SafeFullRemoveItemFromSlot(INVENTORY_SLOT_NOT_SET, i);
				}
			}
		}
	}

	for(int32 player_slot = INVENTORY_SLOT_BAG_START; player_slot < INVENTORY_SLOT_BAG_END; player_slot++)
	{
		Item *item = GetInventoryItem( player_slot );
		if(item && item->IsContainer())
		{
			for (uint32 inside_container_slot =0; inside_container_slot < item->GetProto()->ContainerSlots;inside_container_slot++)
			{
				Item *item2 = SafeContainerCast(item)->GetItem( inside_container_slot );
				if (item2)
				{
					if (item2->GetProto()->ItemId == id && item->wrapped_item_id==0)
					{
						if (item2->GetUInt32Value(ITEM_FIELD_STACK_COUNT) > amt)
						{
							item2->SetCount(item2->GetUInt32Value(ITEM_FIELD_STACK_COUNT) - amt);
							item2->m_isDirty = true;
							return amt;
						}
						else if (item2->GetUInt32Value(ITEM_FIELD_STACK_COUNT)== amt)
						{
							bool result = SafeFullRemoveItemFromSlot( player_slot, inside_container_slot );
							if(result)
							{
								return amt;
							}
							else
							{
								return 0;
							}
						}
						else
						{
							amt -= item2->GetUInt32Value(ITEM_FIELD_STACK_COUNT);
							SafeFullRemoveItemFromSlot( player_slot, inside_container_slot );
					  
						}
					}
				}
			}
		}
	}

/*	for(i = INVENTORY_KEYRING_START; i < INVENTORY_KEYRING_END; i++)
	{
		Item *item = GetInventoryItem(i);
		if (item)
		{
			if(item->GetProto()->ItemId == id && item->wrapped_item_id==0)
			{
				if (item->GetUInt32Value(ITEM_FIELD_STACK_COUNT) > amt)
				{
					item->SetCount(item->GetUInt32Value(ITEM_FIELD_STACK_COUNT) - amt);
					item->m_isDirty = true;
					return amt;
				}
				else if (item->GetUInt32Value(ITEM_FIELD_STACK_COUNT)== amt)
				{
					bool result = SafeFullRemoveItemFromSlot(INVENTORY_SLOT_NOT_SET, i);
					if(result)
					{
						return amt;
					}
					else
					{
						return 0;
					}
				}
				else
				{
					amt -= item->GetUInt32Value(ITEM_FIELD_STACK_COUNT);
					SafeFullRemoveItemFromSlot(INVENTORY_SLOT_NOT_SET, i);
				}
			}
		}
	} */

	if(IncBank)
	{
		for(i = BANK_SLOT_ITEM_START; i < BANK_SLOT_ITEM_END; i++)
		{
			Item *item = GetInventoryItem(i);
			if (item)
			{
				if(item->GetEntry() == id && item->wrapped_item_id==0) 					
				{
					if( item->GetUInt32Value(ITEM_FIELD_STACK_COUNT) > amt )
					{
						item->SetCount(item->GetUInt32Value(ITEM_FIELD_STACK_COUNT) - amt);
						item->m_isDirty = true;
						return amt;
					}
					else if (item->GetUInt32Value(ITEM_FIELD_STACK_COUNT)== amt)
					{
						bool result = SafeFullRemoveItemFromSlot(INVENTORY_SLOT_NOT_SET, i);
						if(result)
						{
							return amt;
						}
						else
						{
							return 0;
						}
					}
					else
					{
						amt -= item->GetUInt32Value(ITEM_FIELD_STACK_COUNT);
						SafeFullRemoveItemFromSlot(INVENTORY_SLOT_NOT_SET, i);
					}
				}
			}
		}

		for(int32 player_slot = BANK_SLOT_BAG_START; player_slot < BANK_SLOT_BAG_END; player_slot++)
		{
			Item *item = GetInventoryItem( player_slot );
			if(item && item->IsContainer())
			{
			   
					for (uint32 inside_container_slot =0; inside_container_slot < item->GetProto()->ContainerSlots; inside_container_slot++)
					{
						Item *item2 = SafeContainerCast(item)->GetItem( inside_container_slot );
						if( item2 && item2->GetEntry() == id && item2->wrapped_item_id==0) 					
						{
							if( item2->GetUInt32Value(ITEM_FIELD_STACK_COUNT) > amt )
							{
								item2->SetCount(item->GetUInt32Value(ITEM_FIELD_STACK_COUNT) - amt);
								item2->m_isDirty = true;
								return amt;
							}
							else if (item2->GetUInt32Value(ITEM_FIELD_STACK_COUNT)== amt)
							{
								bool result = SafeFullRemoveItemFromSlot( player_slot, inside_container_slot );
								if(result)
								{
									return amt;
								}
								else
								{
									return 0;
								}
							}
							else
							{
								amt -= item2->GetUInt32Value(ITEM_FIELD_STACK_COUNT);
								SafeFullRemoveItemFromSlot( player_slot, inside_container_slot );
							}
						}
					}
				
			}
		}
	}

	return 0;
}

uint32 ItemInterface::RemoveItemAmt_ProtectPointer(uint32 id, uint32 amt, Item** pointer)
{
	//this code returns shit return value is fucked
	if (GetItemCount(id) < amt)
	{
		return 0;
	}
	uint32 i;

	for(i = EQUIPMENT_SLOT_START; i < INVENTORY_SLOT_ITEM_END; i++)
	{
		Item *item = GetInventoryItem(i);
		if (item)
		{
			if(item->GetEntry() == id && item->wrapped_item_id==0)
			{
				if(item->GetProto()->ContainerSlots > 0 && item->IsContainer() && SafeContainerCast(item)->HasItems())
				{
					/* sounds weird? no. this will trigger a callstack display due to my other debug code. */
					item->DeleteFromDB();
					continue;
				}

				if (item->GetUInt32Value(ITEM_FIELD_STACK_COUNT) > amt)
				{
					item->SetCount(item->GetUInt32Value(ITEM_FIELD_STACK_COUNT) - amt);
					item->m_isDirty = true;
					return amt;
				}
				else if (item->GetUInt32Value(ITEM_FIELD_STACK_COUNT)== amt)
				{
					bool result = SafeFullRemoveItemFromSlot(INVENTORY_SLOT_NOT_SET, i);
					if( pointer != NULL && *pointer != NULL && *pointer == item )
						*pointer = NULL;

					if(result)
					{
						return amt;
					}
					else
					{
						return 0;
					}
				}
				else
				{
					amt -= item->GetUInt32Value(ITEM_FIELD_STACK_COUNT);
					SafeFullRemoveItemFromSlot(INVENTORY_SLOT_NOT_SET, i);

					if( pointer != NULL && *pointer != NULL && *pointer == item )
						*pointer = NULL;
				}
			}
		}
	}

	for(int32 player_slot = INVENTORY_SLOT_BAG_START; player_slot < INVENTORY_SLOT_BAG_END; player_slot++)
	{
		Item *item = GetInventoryItem( player_slot );
		if(item && item->IsContainer())
		{
			for (uint32 inside_container_slot =0; inside_container_slot < item->GetProto()->ContainerSlots;inside_container_slot++)
			{
				Item *item2 = SafeContainerCast(item)->GetItem( inside_container_slot );
				if (item2)
				{
					if (item2->GetProto()->ItemId == id && item->wrapped_item_id==0)
					{
						if (item2->GetUInt32Value(ITEM_FIELD_STACK_COUNT) > amt)
						{
							item2->SetCount(item2->GetUInt32Value(ITEM_FIELD_STACK_COUNT) - amt);
							item2->m_isDirty = true;
							return amt;
						}
						else if (item2->GetUInt32Value(ITEM_FIELD_STACK_COUNT)== amt)
						{
							bool result = SafeFullRemoveItemFromSlot( player_slot, inside_container_slot );
							if( pointer != NULL && *pointer != NULL && *pointer == item2 )
								*pointer = NULL;

							if(result)
							{
								return amt;
							}
							else
							{
								return 0;
							}
						}
						else
						{
							amt -= item2->GetUInt32Value(ITEM_FIELD_STACK_COUNT);
							SafeFullRemoveItemFromSlot( player_slot, inside_container_slot );

							if( pointer != NULL && *pointer != NULL && *pointer == item2 )
								*pointer = NULL;
						}
					}
				}
			}
		}
	}

/*	for(i = INVENTORY_KEYRING_START; i < INVENTORY_KEYRING_END; i++)
	{
		Item *item = GetInventoryItem(i);
		if (item)
		{
			if(item->GetProto()->ItemId == id && item->wrapped_item_id==0)
			{
				if (item->GetUInt32Value(ITEM_FIELD_STACK_COUNT) > amt)
				{
					item->SetCount(item->GetUInt32Value(ITEM_FIELD_STACK_COUNT) - amt);
					item->m_isDirty = true;
					return amt;
				}
				else if (item->GetUInt32Value(ITEM_FIELD_STACK_COUNT)== amt)
				{
					bool result = SafeFullRemoveItemFromSlot(INVENTORY_SLOT_NOT_SET, i);
					if( pointer != NULL && *pointer != NULL && *pointer == item )
						*pointer = NULL;

					if(result)
					{
						return amt;
					}
					else
					{
						return 0;
					}
				}
				else
				{
					amt -= item->GetUInt32Value(ITEM_FIELD_STACK_COUNT);
					SafeFullRemoveItemFromSlot(INVENTORY_SLOT_NOT_SET, i);

					if( pointer != NULL && *pointer != NULL && *pointer == item )
						*pointer = NULL;
				}
			}
		}
	}*/
	return 0;
}

void ItemInterface::RemoveAllConjured()
{
	for(uint32 x = INVENTORY_SLOT_BAG_START; x < INVENTORY_SLOT_ITEM_END; ++x)
	{
		if (m_pItems[x]!= NULL)
		{
			if(IsBagSlot(x) && m_pItems[x]->IsContainer())
			{
				Container * bag = SafeContainerCast(m_pItems[x]);
 
				for(uint32 i = 0; i < bag->GetProto()->ContainerSlots; i++)
				{
					if (bag->GetItem(i) != NULL && 
						( bag->GetItem(i)->GetProto() && (bag->GetItem(i)->GetProto()->Flags & ITEM_FLAG_CONJURED) ) || m_pItems[x]->HasFlag( ITEM_FIELD_FLAGS, ITEM_FLAG_CONJURED ) )
						bag->SafeFullRemoveItemFromSlot(i);
				}
			}
			else
			{
				if( ( m_pItems[x]->GetProto() && ( m_pItems[x]->GetProto()->Flags & ITEM_FLAG_CONJURED ) ) || m_pItems[x]->HasFlag( ITEM_FIELD_FLAGS, ITEM_FLAG_CONJURED ) )
					SafeFullRemoveItemFromSlot(INVENTORY_SLOT_NOT_SET, x);
			}
		}
	}
}


//-------------------------------------------------------------------//
//Description: Gets slot number by itemid, banks not included
//-------------------------------------------------------------------//
int16 ItemInterface::GetInventorySlotById(uint32 ID)
{
	for(uint16 i=0;i<INVENTORY_SLOT_ITEM_END;i++)
	{
		if(m_pItems[i])
		{
			if(m_pItems[i]->GetProto()->ItemId == ID)
			{
				return i;
			}
		}
	}
	return ITEM_NO_SLOT_AVAILABLE;
}

//-------------------------------------------------------------------//
//Description: Gets slot number by item guid, banks not included
//-------------------------------------------------------------------//
int16 ItemInterface::GetInventorySlotByGuid(uint64 guid)
{
	for(uint16 i=EQUIPMENT_SLOT_START ;i<INVENTORY_SLOT_ITEM_END;i++)
	{
		if(m_pItems[i])
		{
			if(m_pItems[i]->GetGUID() == guid)
			{
				return i;
			}
		}
	}

/*	for(uint16 i=INVENTORY_KEYRING_START; i<INVENTORY_KEYRING_END; i++)
	{
		if(m_pItems[i])
		{
			if(m_pItems[i]->GetGUID() == guid)
			{
				return i;
			}
		}
	}*/
	return ITEM_NO_SLOT_AVAILABLE; //was changed from 0 cuz 0 is the slot for head
}

int16 ItemInterface::GetBagSlotByGuid(uint64 guid)
{
	for(uint16 i=EQUIPMENT_SLOT_START ;i<INVENTORY_SLOT_ITEM_END;i++)
	{
		if(m_pItems[i])
		{
			if(m_pItems[i]->GetGUID() == guid)
			{
				return i;
			}
		}
	}

/*	for(uint16 i=INVENTORY_KEYRING_START; i<INVENTORY_KEYRING_END; i++)
	{
		if(m_pItems[i])
		{
			if(m_pItems[i]->GetGUID() == guid)
			{
				return i;
			}
		}
	}*/
	for(uint16 i=INVENTORY_SLOT_BAG_START; i<INVENTORY_SLOT_BAG_END; ++i)
	{
		if( m_pItems[i]&&m_pItems[i]->IsContainer() )
		{
			for(uint32 j = 0; j < m_pItems[i]->GetProto()->ContainerSlots; ++j)
			{
				Item * inneritem = SafeContainerCast(m_pItems[i])->GetItem(j);
				if(inneritem && inneritem->GetGUID()==guid)
				{ 
					return i;
				}
			}
		}
	}

	return ITEM_NO_SLOT_AVAILABLE; //was changed from 0 cuz 0 is the slot for head
}


//-------------------------------------------------------------------//
//Description: Adds a Item to a free slot
//-------------------------------------------------------------------//
AddItemResult ItemInterface::AddItemToFreeSlot(Item **item)
{
	if( (*item) == NULL )
	{ 
		return ADD_ITEM_RESULT_ERROR;
	}

	if( (*item)->GetProto() == NULL )
	{ 
		return ADD_ITEM_RESULT_ERROR;
	}

	uint32 i = 0;
	bool result2;
	AddItemResult result3;

	//detect special bag item
	if( (*item)->GetProto()->BagFamily )
	{
/*		if( (*item)->GetProto()->BagFamily & ITEM_TYPE_KEYRING || (*item)->GetProto()->Class == ITEM_CLASS_KEY )
		{
			for(i=INVENTORY_KEYRING_START; i<INVENTORY_KEYRING_END; i++ )
			{
				if(m_pItems[i] == NULL)
				{
					result3 = SafeAddItem( (*item), INVENTORY_SLOT_NOT_SET, i );
					if( result3 == ADD_ITEM_RESULT_OK )
					{
						result.ContainerSlot = INVENTORY_SLOT_NOT_SET;
						result.Slot = i;
						result.Result = true;
						return ADD_ITEM_RESULT_OK;
					}
				}
			}
		}
		else */
		{
			for(i=INVENTORY_SLOT_BAG_START;i<INVENTORY_SLOT_BAG_END;i++)
			{
				if(m_pItems[i])
				{
					if (m_pItems[i]->GetProto()->BagFamily & (*item)->GetProto()->BagFamily)
					{
						if(m_pItems[i]->IsContainer())
						{
							uint32 r_slot;
							result2 = SafeContainerCast(m_pItems[i])->AddItemToFreeSlot((*item), &r_slot);
							if (result2) 
							{
								result.ContainerSlot = i;
								result.Slot = r_slot;
								result.Result = true;
								return ADD_ITEM_RESULT_OK;
							}
						}
					}
				}
			}
		}
	}

	//INVENTORY
	for(i=INVENTORY_SLOT_ITEM_START;i<INVENTORY_SLOT_ITEM_END;i++)
	{
		if(m_pItems[i] == NULL)
		{
			result3 = SafeAddItem((*item), INVENTORY_SLOT_NOT_SET, i);
			if( result3 == ADD_ITEM_RESULT_OK )
			{
				result.ContainerSlot = INVENTORY_SLOT_NOT_SET;
				result.Slot = i;
				result.Result = true;
				return ADD_ITEM_RESULT_OK;
			}
		}
		else if(m_pItems[i]->GetProto()->ItemId == (*item)->GetProto()->ItemId &&
				m_pItems[i]->GetUInt32Value(ITEM_FIELD_STACK_COUNT) + (*item)->GetUInt32Value( ITEM_FIELD_STACK_COUNT ) <= m_pItems[i]->GetProto()->MaxCount)
			{
				m_pItems[i]->SetUInt32Value( ITEM_FIELD_STACK_COUNT,m_pItems[i]->GetUInt32Value( ITEM_FIELD_STACK_COUNT ) + (*item)->GetUInt32Value( ITEM_FIELD_STACK_COUNT ) );
				result.Slot=i;
				result.Result=true;
				//we merged item, we delete the old one !
				(*item)->DeleteFromDB();
				(*item)->DeleteMe();
				//(*item) = NULL; //make sure we do not use this item from where we called the function
				(*item) = m_pItems[i]; //wow, cocky move, we change pointer to the item we merged with !
				return ADD_ITEM_RESULT_OK;
			}
	}

	//INVENTORY BAGS
	for(i=INVENTORY_SLOT_BAG_START;i<INVENTORY_SLOT_BAG_END;i++)
	{
		if(m_pItems[i] != NULL && m_pItems[i]->GetProto()->BagFamily == 0 && m_pItems[i]->IsContainer()) //special bags ignored
		{
			for (uint32 j =0; j < m_pItems[i]->GetProto()->ContainerSlots;j++)
			{
				Item *item2 = SafeContainerCast(m_pItems[i])->GetItem(j);
				if (item2 == NULL)
				{
					result3 = SafeAddItem((*item), i, j);
					if( result3 == ADD_ITEM_RESULT_OK ) 
					{
						result.ContainerSlot = i;
						result.Slot = j;
						result.Result = true;
						return ADD_ITEM_RESULT_OK;
					}
				}
				else if(item2->GetProto()->ItemId == (*item)->GetProto()->ItemId &&
					item2->GetUInt32Value(ITEM_FIELD_STACK_COUNT) + (*item)->GetUInt32Value( ITEM_FIELD_STACK_COUNT ) <= item2->GetProto()->MaxCount)
				{
					item2->SetUInt32Value( ITEM_FIELD_STACK_COUNT,item2->GetUInt32Value( ITEM_FIELD_STACK_COUNT ) + (*item)->GetUInt32Value( ITEM_FIELD_STACK_COUNT ) );
					result.Slot=i;
					result.Result=true;
					//we merged item, we delete the old one !
					(*item)->DeleteFromDB();
					(*item)->DeleteMe();
					//(*item) = NULL; //make sure we do not use this item from where we called the function
					(*item) = item2; //wow, cocky move, we change pointer to the item we merged with !
					return ADD_ITEM_RESULT_OK;
				}
			}
		}
	}
	return ADD_ITEM_RESULT_ERROR;
}

//-------------------------------------------------------------------//
//Description: Calculates inventory free slots, bag inventory slots not included
//-------------------------------------------------------------------//
uint32 ItemInterface::CalculateFreeSlots(ItemPrototype *proto)
{
	uint32 count = 0;
	uint32 i;

	if(proto)
	{
		if(proto->BagFamily)
		{
			if(proto->BagFamily & ITEM_TYPE_KEYRING || proto->Class == ITEM_CLASS_KEY)
			{
/*				for(uint32 i = INVENTORY_KEYRING_START; i < INVENTORY_KEYRING_END; i++)
				{
					if(m_pItems[i] == NULL)
					{
						count++;
					}
				}*/
			}
			else
			{
				for(uint32 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END;i++)
				{
					if(m_pItems[i] && m_pItems[i]->IsContainer())
					{
						if (m_pItems[i]->GetProto()->BagFamily & proto->BagFamily)
						{
							int16 slot = SafeContainerCast(m_pItems[i])->FindFreeSlot();
							if(slot != ITEM_NO_SLOT_AVAILABLE) 
							{
								count++;
							}
						}
					}
				}
			}
		}
	}

	for(i=INVENTORY_SLOT_ITEM_START;i<INVENTORY_SLOT_ITEM_END;i++)
	{
		if(m_pItems[i] == NULL)
		{
			count++;
		}
	}

	for(i=INVENTORY_SLOT_BAG_START;i<INVENTORY_SLOT_BAG_END;i++)
	{
		if(m_pItems[i] != NULL )
		{
			if(m_pItems[i]->IsContainer() && !m_pItems[i]->GetProto()->BagFamily)
			{

				for (uint32 j =0; j < m_pItems[i]->GetProto()->ContainerSlots;j++)
				{
					Item *item2 = SafeContainerCast(m_pItems[i])->GetItem(j);
					if (item2 == NULL)
					{
						count++;
					}
				}
			}
		}
	}
	return count;
}

//-------------------------------------------------------------------//
//Description: finds a free slot on the backpack
//-------------------------------------------------------------------//
int16 ItemInterface::FindFreeBackPackSlot()
{
	//search for backpack slots
	for(int16 i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; i++)
	{
		Item *item = GetInventoryItem(i);
		if(!item) 
		{ 
			return i; 
		}
	}

	return ITEM_NO_SLOT_AVAILABLE; //no slots available
}

//-------------------------------------------------------------------//
//Description: converts bank bags slot ids into player bank byte slots(0-5)
//-------------------------------------------------------------------//
int16 ItemInterface::GetInternalBankSlotFromPlayer(int16 islot)
{
	switch(islot)
	{
	case BANK_SLOT_BAG_1:
		{
			return 1;
		}
	case BANK_SLOT_BAG_2:
		{
			return 2;
		}
	case BANK_SLOT_BAG_3:
		{
			return 3;
		}
	case BANK_SLOT_BAG_4:
		{
			return 4;
		}
	case BANK_SLOT_BAG_5:
		{
			return 5;
		}
	case BANK_SLOT_BAG_6:
		{
			return 6;
		}
	case BANK_SLOT_BAG_7:
		{
			return 7;
		}
	default:
		return 8;
	}
}

//-------------------------------------------------------------------//
//Description: checks if the item can be equiped on a specific slot
//             this will check unique-equipped gems as well
//-------------------------------------------------------------------//
int8 ItemInterface::CanEquipItemInSlot2(int16 DstInvSlot, int16 slot, Item* item, bool ignore_combat /* = false */, bool skip_2h_check /* = false */)
{
	ItemPrototype *proto=item->GetProto();
	uint32 count;
	int8 ret;

	ret = CanEquipItemInSlot(DstInvSlot, slot, proto, ignore_combat, skip_2h_check);
	if (ret) 
	{ 
		return ret;
	}

	if((slot < INVENTORY_SLOT_BAG_END && DstInvSlot == INVENTORY_SLOT_NOT_SET) || (slot >= BANK_SLOT_BAG_START && slot < BANK_SLOT_BAG_END && DstInvSlot == INVENTORY_SLOT_NOT_SET))
	{
		for (count=ITEM_ENCHANT_SLOT_GEM1; count<=ITEM_ENCHANT_SLOT_GEM3; count++)
		{
			EnchantmentInstance *ei = item->GetEnchantment( count );
			if (ei 
				&& ei->Enchantment->GemEntry //huh ? Gem without entry ?
				)
			{
				ItemPrototype * ip = ItemPrototypeStorage.LookupEntry(ei->Enchantment->GemEntry);

				if (
					ip //maybe gem got removed from db due to update ?
					&& ( 
//						( (ip->Flags & ITEM_FLAG_UNIQUE_EQUIP) && IsEquipped(ip->ItemId) ) ||
//						(ip->ItemLimitCategory && IsCategoryEquipped( ip->ItemLimitCategory ) ) ||
						CanCustomCategoryEquip( ip ) == false
						)
					)
				{
					return INV_ERR_CANT_CARRY_MORE_OF_THIS;
				}
			}
		}
	}

	return 0;
}

int8 ItemInterface::CanEquipItem(ItemPrototype *proto, Player *m_pOwner)
{
	// Check to see if we have the correct race
	if(!(proto->AllowableRace& m_pOwner->getRaceMask()))
	{ 
		return INV_ERR_YOU_CAN_NEVER_USE_THAT_ITEM;
	}

	// Check to see if we have the correct class
	if(!(proto->AllowableClass & m_pOwner->getClassMask()))
	{ 
		return INV_ERR_YOU_CAN_NEVER_USE_THAT_ITEM2;
	}

	// Check to see if we have the reqs for that reputation
	if(proto->RequiredFaction)
	{
		Standing current_standing = Player::GetReputationRankFromStanding(m_pOwner->GetStanding(proto->RequiredFaction));
		if(current_standing < (Standing)proto->RequiredFactionStanding)	   // Not enough rep rankage..
		{ 
			return INV_ERR_ITEM_REPUTATION_NOT_ENOUGH;
		}
	}

	// Check to see if we have the correct level.
	if(proto->RequiredLevel>m_pOwner->GetUInt32Value(UNIT_FIELD_LEVEL))
	{ 
		return INV_ERR_YOU_MUST_REACH_LEVEL_N;
	}

	if( proto->Class == ITEM_CLASS_ARMOR )
	{

		if( proto->SubClass != ITEM_SUBCLASS_ARMOR_COSMETIC_CLOTH_LEATHER_MAIL_PLATE && !(m_pOwner->GetArmorProficiency()&(((uint32)(1))<<proto->SubClass)))
		{ 
			return INV_ERR_NO_REQUIRED_PROFICIENCY;
		}

	}
	else if( proto->Class == ITEM_CLASS_WEAPON )
	{
		if(!(m_pOwner->GetWeaponProficiency()&(((uint32)(1))<<proto->SubClass)))
		{ 
			return INV_ERR_NO_REQUIRED_PROFICIENCY;
		}
	}

	if(proto->RequiredSkill)
		if (proto->RequiredSkillRank > m_pOwner->_GetSkillLineCurrent(proto->RequiredSkill,true))
		{ 
			return INV_ERR_SKILL_ISNT_HIGH_ENOUGH;
		}

	if(proto->RequiredSpell)
		if (!m_pOwner->HasSpell(proto->RequiredSpell))
		{ 
			return INV_ERR_NO_REQUIRED_PROFICIENCY;
		}
	return 0;
}

//-------------------------------------------------------------------//
//Description: checks if the item can be equiped on a specific slot
//-------------------------------------------------------------------//
int8 ItemInterface::CanEquipItemInSlot(int16 DstInvSlot, int16 slot, ItemPrototype* proto, bool ignore_combat /* = false */, bool skip_2h_check /* = false */)
{
	uint32 type=proto->InventoryType;
	
	if(slot >= INVENTORY_SLOT_BAG_START && slot < INVENTORY_SLOT_BAG_END && DstInvSlot == -1)
		if(proto->ContainerSlots == 0)
		{ 
			return INV_ERR_ITEMS_CANT_BE_SWAPPED;
		}

	if((slot < INVENTORY_SLOT_BAG_END && DstInvSlot == INVENTORY_SLOT_NOT_SET) || (slot >= BANK_SLOT_BAG_START && slot < BANK_SLOT_BAG_END && DstInvSlot == INVENTORY_SLOT_NOT_SET))
	{
		if (!ignore_combat && m_pOwner->CombatStatus.IsInCombat() && (slot < EQUIPMENT_SLOT_MAINHAND || slot > EQUIPMENT_SLOT_RANGED))
		{ 
			return INV_ERR_CANT_DO_IN_COMBAT;
		}

		if( 
	//			( (proto->Unique || ( proto->Flags & ITEM_FLAG_UNIQUE_EQUIP )) && IsEquipped(proto->ItemId)) ||
	//			(proto->ItemLimitCategory && IsCategoryEquipped( proto->ItemLimitCategory ) ) ||
			CanCustomCategoryEquip( proto ) == false
			)
		{ 
			return INV_ERR_CANT_CARRY_MORE_OF_THIS;
		}

		int8 tret = CanEquipItem( proto, m_pOwner );
		if( tret != 0 )
			return tret;

		// You are dead !
		if(m_pOwner->getDeathState() != ALIVE)
		{ 
			return INV_ERR_YOU_ARE_DEAD;
		}
	}

	switch(slot)
	{
	case EQUIPMENT_SLOT_HEAD:
		{
			if(type == INVTYPE_HEAD)
			{ 
				return 0;
			}
			else
				return INV_ERR_ITEM_DOESNT_GO_TO_SLOT;
		}
	case EQUIPMENT_SLOT_NECK:
		{
			if(type == INVTYPE_NECK)
			{ 
				return 0;
			}
			else
				return INV_ERR_ITEM_DOESNT_GO_TO_SLOT;
		}
	case EQUIPMENT_SLOT_SHOULDERS:
		{
			if(type == INVTYPE_SHOULDERS)
			{ 
				return 0;
			}
			else
				return INV_ERR_ITEM_DOESNT_GO_TO_SLOT;
		}
	case EQUIPMENT_SLOT_BODY:
		{
			if(type == INVTYPE_BODY)
			{ 
				return 0;
			}
			else
				return INV_ERR_ITEM_DOESNT_GO_TO_SLOT;
		}
	case EQUIPMENT_SLOT_CHEST:
		{
			if(type == INVTYPE_CHEST || type == INVTYPE_ROBE)
			{ 
				return 0;
			}
			else
				return INV_ERR_ITEM_DOESNT_GO_TO_SLOT;
		}
	case EQUIPMENT_SLOT_WAIST:
		{
			if(type == INVTYPE_WAIST)
			{ 
				return 0;
			}
			else
				return INV_ERR_ITEM_DOESNT_GO_TO_SLOT;
		}
	case EQUIPMENT_SLOT_LEGS:
		{
			if(type == INVTYPE_LEGS)
			{ 
				return 0;
			}
			else
				return INV_ERR_ITEM_DOESNT_GO_TO_SLOT;
		}
	case EQUIPMENT_SLOT_FEET:
		{
			if(type == INVTYPE_FEET)
			{ 
				return 0;
			}
			else
				return INV_ERR_ITEM_DOESNT_GO_TO_SLOT;
		}
	case EQUIPMENT_SLOT_WRISTS:
		{
			if(type == INVTYPE_WRISTS)
			{ 
				return 0;
			}
			else
				return INV_ERR_ITEM_DOESNT_GO_TO_SLOT;
		}
	case EQUIPMENT_SLOT_HANDS:
		{
			if(type == INVTYPE_HANDS)
			{ 
				return 0;
			}
			else
				return INV_ERR_ITEM_DOESNT_GO_TO_SLOT;
		}
	case EQUIPMENT_SLOT_FINGER1:
	case EQUIPMENT_SLOT_FINGER2:
		{
			if(type == INVTYPE_FINGER)
			{ 
				return 0;
			}
			else
				return INV_ERR_ITEM_DOESNT_GO_TO_SLOT;
		}
	case EQUIPMENT_SLOT_TRINKET1:
	case EQUIPMENT_SLOT_TRINKET2:
		{
			if(type == INVTYPE_TRINKET)
			{ 
				return 0;
			}
			else
				return INV_ERR_ITEM_DOESNT_GO_TO_SLOT;
		}
	case EQUIPMENT_SLOT_BACK:
		{
			if(type == INVTYPE_CLOAK)
			{ 
				return 0;
			}
			else
				return INV_ERR_ITEM_DOESNT_GO_TO_SLOT;
		}
	case EQUIPMENT_SLOT_MAINHAND:
		{
			//you cannot dual weild with polearms
			Item *offhand = GetInventoryItem(EQUIPMENT_SLOT_OFFHAND);
			if( offhand && offhand->GetProto() && offhand->GetProto()->Class == ITEM_CLASS_WEAPON && offhand->GetProto()->SubClass == ITEM_SUBCLASS_WEAPON_POLEARM )
				return INV_ERR_CANT_DUAL_WIELD;
			if( proto->SubClass == ITEM_SUBCLASS_WEAPON_POLEARM && GetInventoryItem(EQUIPMENT_SLOT_OFFHAND) )
				return INV_ERR_CANT_DUAL_WIELD;

			if(type == INVTYPE_WEAPON || type == INVTYPE_WEAPONMAINHAND ||
				(type == INVTYPE_2HWEAPON && (!GetInventoryItem(EQUIPMENT_SLOT_OFFHAND) || skip_2h_check || 
					( GetOwner()->HasSpellwithNameHash( SPELL_HASH_TITAN_S_GRIP ) ))))
				return INV_ERR_OK;
			else
				return INV_ERR_ITEM_DOESNT_GO_TO_SLOT;
		}
	case EQUIPMENT_SLOT_OFFHAND:
		{
			//you cannot dual weild with polearms
			if( GetInventoryItem(EQUIPMENT_SLOT_MAINHAND) && GetInventoryItem(EQUIPMENT_SLOT_MAINHAND)->GetProto() && GetInventoryItem(EQUIPMENT_SLOT_MAINHAND)->GetProto()->SubClass == ITEM_SUBCLASS_WEAPON_POLEARM )
				return INV_ERR_CANT_DUAL_WIELD;
			if( proto->Class == ITEM_CLASS_WEAPON && proto->SubClass == ITEM_SUBCLASS_WEAPON_POLEARM && GetInventoryItem(EQUIPMENT_SLOT_MAINHAND) )
				return INV_ERR_CANT_DUAL_WIELD;

			if( GetOwner()->HasSpellwithNameHash( SPELL_HASH_TITAN_S_GRIP ) )
			{
				if( type == INVTYPE_2HWEAPON )
					return INV_ERR_OK; //sure we can do it
				skip_2h_check = true;
			}

			if(type == INVTYPE_WEAPON || type == INVTYPE_WEAPONOFFHAND)
			{
				Item* mainweapon = GetInventoryItem(EQUIPMENT_SLOT_MAINHAND);
				if(mainweapon) //item exists
				{
					if(mainweapon->GetProto())
					{
						if(mainweapon->GetProto()->InventoryType != INVTYPE_2HWEAPON)
						{
							if(m_pOwner->_HasSkillLine(SKILL_DUAL_WIELD))
							{ 
								return 0;
							}
							else
								return INV_ERR_CANT_DUAL_WIELD;
						}
						else
						{
							if(!skip_2h_check)
							{ 
								return INV_ERR_CANT_EQUIP_WITH_TWOHANDED;
							}
							else
								return 0;
						}
					}
				}
				else
				{
					if(m_pOwner->_HasSkillLine(SKILL_DUAL_WIELD))
					{ 
						return 0;
					}
					else
						return INV_ERR_CANT_DUAL_WIELD;
				}
			}
			else if(type == INVTYPE_SHIELD || type == INVTYPE_HOLDABLE)
			{
				Item* mainweapon = GetInventoryItem(EQUIPMENT_SLOT_MAINHAND);
				if(mainweapon) //item exists
				{
					if(mainweapon->GetProto())
					{
						if(mainweapon->GetProto()->InventoryType != INVTYPE_2HWEAPON)
						{
							return 0;
						}
						else
						{
							if(!skip_2h_check)
							{ 
								return INV_ERR_CANT_EQUIP_WITH_TWOHANDED;
							}
							else
								return 0;
						}
					}
				}
				else
				{
					return 0;
				}
			}
			else
				return INV_ERR_ITEM_DOESNT_GO_TO_SLOT;
		}
	case EQUIPMENT_SLOT_RANGED:
		{
			if(type == INVTYPE_RANGED || type == INVTYPE_THROWN || type == INVTYPE_RANGEDRIGHT || type == INVTYPE_RELIC)
			{ 
				return 0;
			}
			else
				return INV_ERR_ITEM_DOESNT_GO_TO_SLOT;//6;
		}
	case EQUIPMENT_SLOT_TABARD:
		{
			if(type == INVTYPE_TABARD)
			{ 
				return 0;
			}
			else
				return INV_ERR_ITEM_DOESNT_GO_TO_SLOT; // 6;
		}
	case BANK_SLOT_BAG_1:
	case BANK_SLOT_BAG_2:
	case BANK_SLOT_BAG_3:
	case BANK_SLOT_BAG_4:
	case BANK_SLOT_BAG_5:
	case BANK_SLOT_BAG_6:
	case BANK_SLOT_BAG_7:
		{
			int32 bytes,slots;
			int16 islot;

			if(!GetInventoryItem(INVENTORY_SLOT_NOT_SET,slot))
			{
				//check if player got that slot.
				bytes = m_pOwner->GetUInt32Value(PLAYER_BYTES_2);
				slots =(uint16) (bytes >> 16);
				islot = GetInternalBankSlotFromPlayer(slot);
				if(slots < islot)
				{
					return INV_ERR_MUST_PURCHASE_THAT_BAG_SLOT;
				}

				//in case bank slot exists, check if player can put the item there
				if(type == INVTYPE_BAG)
				{
					return 0;
				}
				else
				{
					return INV_ERR_NOT_A_BAG;
				}
			}
			else
			{
				if(GetInventoryItem(INVENTORY_SLOT_NOT_SET,slot)->GetProto()->BagFamily)
				{
					if((IsBagSlot(slot) && DstInvSlot == INVENTORY_SLOT_NOT_SET))
					{
						if(proto->InventoryType == INVTYPE_BAG )
						{
							return 0;
						}
					}

					if(proto->BagFamily & GetInventoryItem(INVENTORY_SLOT_NOT_SET,slot)->GetProto()->BagFamily)
					{
						return 0;
					}
					else
					{
						return INV_ERR_ITEM_DOESNT_GO_INTO_BAG;
					}
				}
				else
				{
					return 0;
				}
			}
		}
	case INVENTORY_SLOT_BAG_1:	
	case INVENTORY_SLOT_BAG_2:
	case INVENTORY_SLOT_BAG_3:
	case INVENTORY_SLOT_BAG_4:
		{
			//this chunk of code will limit you to equip only 1 Ammo Bag. Later i found out that this is not blizzlike so i will remove it when it's blizzlike
			//we are trying to equip an Ammo Bag
			if(proto->Class==ITEM_CLASS_QUIVER)
			{
				//check if we already have an AB equiped
				FindAmmoBag();
				//we do have amo bag but we are not swaping them then we send error
				if(result.Slot!=ITEM_NO_SLOT_AVAILABLE && result.Slot != slot)
				{
					return INV_ERR_CAN_EQUIP_ONLY1_AMMOPOUCH;
				}
			}
			if(GetInventoryItem(INVENTORY_SLOT_NOT_SET,slot))
			{
				if(GetInventoryItem(INVENTORY_SLOT_NOT_SET,slot)->GetProto()->BagFamily)
				{
					if((IsBagSlot(slot) && DstInvSlot == INVENTORY_SLOT_NOT_SET))
					{
						if(proto->InventoryType == INVTYPE_BAG )
						{
							return 0;
						}
					}
					
					if(proto->BagFamily & GetInventoryItem(INVENTORY_SLOT_NOT_SET,slot)->GetProto()->BagFamily)
					{
						return 0;
					}
					else
					{
						return INV_ERR_ITEM_DOESNT_GO_INTO_BAG;
					}
				}
				else
				{
					return 0;
				}
			}
			else
			{
				if(type == INVTYPE_BAG)
				{
					return 0;
				}
				else
				{
					return INV_ERR_NOT_A_BAG;
				}
			}
		}
	case INVENTORY_SLOT_ITEM_1:
	case INVENTORY_SLOT_ITEM_2:
	case INVENTORY_SLOT_ITEM_3:
	case INVENTORY_SLOT_ITEM_4:
	case INVENTORY_SLOT_ITEM_5:
	case INVENTORY_SLOT_ITEM_6:
	case INVENTORY_SLOT_ITEM_7:
	case INVENTORY_SLOT_ITEM_8:
	case INVENTORY_SLOT_ITEM_9:
	case INVENTORY_SLOT_ITEM_10:
	case INVENTORY_SLOT_ITEM_11:
	case INVENTORY_SLOT_ITEM_12:
	case INVENTORY_SLOT_ITEM_13:
	case INVENTORY_SLOT_ITEM_14:
	case INVENTORY_SLOT_ITEM_15:
	case INVENTORY_SLOT_ITEM_16:
		{
			return 0;
		}
/*	case INVENTORY_KEYRING_1:
	case INVENTORY_KEYRING_2:
	case INVENTORY_KEYRING_3:
	case INVENTORY_KEYRING_4:
	case INVENTORY_KEYRING_5:
	case INVENTORY_KEYRING_6:
	case INVENTORY_KEYRING_7:
	case INVENTORY_KEYRING_8:
	case INVENTORY_KEYRING_9:
	case INVENTORY_KEYRING_10:
	case INVENTORY_KEYRING_11:
	case INVENTORY_KEYRING_12:
	case INVENTORY_KEYRING_13:
	case INVENTORY_KEYRING_14:
	case INVENTORY_KEYRING_15:
	case INVENTORY_KEYRING_16:
	case INVENTORY_KEYRING_17:
	case INVENTORY_KEYRING_18:
	case INVENTORY_KEYRING_19:
	case INVENTORY_KEYRING_20:
	case INVENTORY_KEYRING_21:
	case INVENTORY_KEYRING_22:
	case INVENTORY_KEYRING_23:
	case INVENTORY_KEYRING_24:
	case INVENTORY_KEYRING_25:
	case INVENTORY_KEYRING_26:
	case INVENTORY_KEYRING_27:
	case INVENTORY_KEYRING_28:
	case INVENTORY_KEYRING_29:
	case INVENTORY_KEYRING_30:
	case INVENTORY_KEYRING_31:
	case INVENTORY_KEYRING_32:
		{
			if(proto->BagFamily & ITEM_TYPE_KEYRING || proto->Class == ITEM_CLASS_KEY)
			{
				return 0;
			}
			else
			{
				return INV_ERR_ITEM_DOESNT_GO_INTO_BAG;
			}
		}*/
/*	case INVENTORY_CURRENCYTOKEN_SLOT_1:
	case INVENTORY_CURRENCYTOKEN_SLOT_2:	
	case INVENTORY_CURRENCYTOKEN_SLOT_3:	
	case INVENTORY_CURRENCYTOKEN_SLOT_4:		
	case INVENTORY_CURRENCYTOKEN_SLOT_5:	
	case INVENTORY_CURRENCYTOKEN_SLOT_6:		
	case INVENTORY_CURRENCYTOKEN_SLOT_7:		
	case INVENTORY_CURRENCYTOKEN_SLOT_8:		
	case INVENTORY_CURRENCYTOKEN_SLOT_9:		
	case INVENTORY_CURRENCYTOKEN_SLOT_10:		
	case INVENTORY_CURRENCYTOKEN_SLOT_11:		
	case INVENTORY_CURRENCYTOKEN_SLOT_12:		
	case INVENTORY_CURRENCYTOKEN_SLOT_13:		
	case INVENTORY_CURRENCYTOKEN_SLOT_14:		
	case INVENTORY_CURRENCYTOKEN_SLOT_15:		
	case INVENTORY_CURRENCYTOKEN_SLOT_16:		
	case INVENTORY_CURRENCYTOKEN_SLOT_17:	
	case INVENTORY_CURRENCYTOKEN_SLOT_18:	
	case INVENTORY_CURRENCYTOKEN_SLOT_19:	
	case INVENTORY_CURRENCYTOKEN_SLOT_20:	
	case INVENTORY_CURRENCYTOKEN_SLOT_21:		
	case INVENTORY_CURRENCYTOKEN_SLOT_22:	
	case INVENTORY_CURRENCYTOKEN_SLOT_23:		
	case INVENTORY_CURRENCYTOKEN_SLOT_24:		
	case INVENTORY_CURRENCYTOKEN_SLOT_25:		
	case INVENTORY_CURRENCYTOKEN_SLOT_26:		
	case INVENTORY_CURRENCYTOKEN_SLOT_27:		
	case INVENTORY_CURRENCYTOKEN_SLOT_28:		
	case INVENTORY_CURRENCYTOKEN_SLOT_29:		
	case INVENTORY_CURRENCYTOKEN_SLOT_30:		
	case INVENTORY_CURRENCYTOKEN_SLOT_31:		
	case INVENTORY_CURRENCYTOKEN_SLOT_32:		
		{
			if( proto->BagFamily & ITEM_TYPE_CURRENCY || proto->Class == ITEM_CLASS_MONEY )
			{
				return 0;
			}
			else
			{
				return INV_ERR_ITEM_DOESNT_GO_INTO_BAG;
			}
		}*/
	default:
		return 0;
	}
}

//-------------------------------------------------------------------//
//Description: Checks if player can receive the item
//-------------------------------------------------------------------//
uint8 ItemInterface::CanReceiveItem(ItemPrototype * item, uint32 amount)
{
	if(!item)
	{
		return (int16)NULL;
	}

	int32 CanHaveMaxCount = MAX_INT;

	if( item->Unique > 0 )
		CanHaveMaxCount = MIN( CanHaveMaxCount, (int32)item->Unique );

	if( item->ItemLimitCategory != 0 )
	{
		ItemLimitCategory *ilc = dbcItemLimitCategory.LookupEntryForced( item->ItemLimitCategory );
		if( ilc )
			CanHaveMaxCount = MIN( CanHaveMaxCount, (int32)ilc->MaxStack );
	}

	if( CanHaveMaxCount != MAX_INT )
	{
		uint32 count = GetItemCount(item->ItemId, true);
		if( (int32)(count + amount) > CanHaveMaxCount )
		{
			return INV_ERR_CANT_CARRY_MORE_OF_THIS;
		}
	}
	return (uint8)NULL;
}

void ItemInterface::BuyItem(ItemPrototype *item, uint32 total_amount, Creature * pVendor)
{
	if(item->BuyPrice)
	{
		uint32 itemprice = GetBuyPriceForItem(item, total_amount, m_pOwner, pVendor);
		if(itemprice>m_pOwner->GetGold())
			m_pOwner->SetGold(0);
		else
			m_pOwner->ModGold( -(int32)itemprice);
	}
	ItemExtendedCostEntry * ex = pVendor->GetItemExtendedCostByItemId( item->ItemId );
	if( ex != NULL )
	{
		for(int i=0;i<5;i++)
		{
			if(ex->item[i])
				m_pOwner->GetItemInterface()->RemoveItemAmt( ex->item[i], total_amount * ex->count[i] );
			//this refers to season or weekly required count and not the count you currently have
			//ex : http://www.wowhead.com/item=70224 rquires 3400 honor and 7500 season honor. We do not store season honor atm
			if( i > 0 && ex->currency[i] == ex->currency[i-1] && ex->currency_count[i] > ex->currency_count[i-1] )
				continue;
			if(ex->currency[i])
				m_pOwner->ModCurrencyCount( ex->currency[i], -((int32)(total_amount * ex->currency_count[i])) );
		}
		//check depracated currency cost
		if( ex->arena )
			m_pOwner->ModCurrencyCount( CURRENCY_ARENA_POINT, -((int32)(total_amount * ex->arena)) );
//		if( ex->honor )
//			m_pOwner->ModCurrencyCount( CURRENCY_HONOR_POINT, -((int32)(total_amount * ex->arena)) );
	}
	//add guild logs
	if( GetOwner()->GetGuild() )
	{
		if( item->Quality >= ITEM_QUALITY_EPIC_PURPLE && item->ItemLevel * 110 / 100 >= GetOwner()->GetItemAvgLevelBlizzlike() )
			GetOwner()->GetGuild()->GuildNewsAdd( GUILD_NEWS_LOG_ITEM_PURCHASED, GetOwner()->GetGUID(), item->ItemId );
	}
}

void ItemInterface::BuyCurrency(uint32 cur_id, uint32 total_amount, Creature * pVendor)
{
	ItemExtendedCostEntry * ex = pVendor->GetItemExtendedCostByItemId( cur_id );
	if( ex != NULL )
	{
		for(int i = 0;i<5;i++)
		{
			if(ex->item[i])
				m_pOwner->GetItemInterface()->RemoveItemAmt( ex->item[i], total_amount * ex->count[i] );
			if(ex->currency[i])
				m_pOwner->ModCurrencyCount( ex->currency[i], -((int32)(total_amount * ex->currency_count[i])) );
		}
		//check depracated currency cost
		if( ex->arena )
			m_pOwner->ModCurrencyCount( CURRENCY_ARENA_POINT, -((int32)(total_amount * ex->arena)) );
//		if( ex->honor )
//			m_pOwner->ModCurrencyCount( CURRENCY_HONOR_POINT, -((int32)(total_amount * ex->arena)) );
	}
}

uint8 ItemInterface::CanAffordItem(ItemPrototype * item,uint32 amount, Creature * pVendor)
{
	ItemExtendedCostEntry * ex = pVendor->GetItemExtendedCostByItemId( item->ItemId );
	amount = MAX( 1, amount );
	if( ex != NULL )
	{
		for(int i = 0;i<5;i++)
		{
			uint32 req_count = MAX( ex->count[i], 1 ) * amount;
			if(ex->item[i] && m_pOwner->GetItemInterface()->GetItemCount(ex->item[i], false) < req_count )
			{ 
				return CAN_AFFORD_ITEM_ERROR_NOT_ENOUGH_MONEY;
			}
			if( ex->currency[i] )
			{
				//this refers to season or weekly required count and not the count you currently have
				//ex : http://www.wowhead.com/item=70224 rquires 3400 honor and 7500 season honor. We do not store season honor atm
				if( i > 0 && ex->currency[i] == ex->currency[i-1] && ex->currency_count[i] > ex->currency_count[i-1] )
					continue;
				uint32 has_count = m_pOwner->GetCurrencyCount( ex->currency[i] );
				uint32 need_count = MAX( 1, ex->currency_count[i] );
				if( need_count > has_count )
					return CAN_AFFORD_ITEM_ERROR_NOT_ENOUGH_MONEY;
			}
		}
		//check depracated currency cost
		if( ex->arena && m_pOwner->GetCurrencyCount( CURRENCY_ARENA_POINT ) < ex->arena )
			return CAN_AFFORD_ITEM_ERROR_NOT_ENOUGH_MONEY;
//		if( ex->honor && m_pOwner->GetCurrencyCount( CURRENCY_HONOR_POINT ) < ex->honor )
//			return CAN_AFFORD_ITEM_ERROR_NOT_ENOUGH_MONEY;
		//not sure this is still used in Cata. I just hope DBC will be updated 
		if(m_pOwner->GetMaxPersonalRating() < ex->personalrating)
		{ 
			return CAN_AFFORD_ITEM_ERROR_NOT_REQUIRED_RANK;
		}
	}

	if(item->BuyPrice)
	{
		int32 price = GetBuyPriceForItem(item, amount, m_pOwner, pVendor) * amount;
		if((int64)m_pOwner->GetGold() < price)
		{
			return CAN_AFFORD_ITEM_ERROR_NOT_ENOUGH_MONEY;
		}
	}
	if(item->RequiredFaction)
	{
		FactionDBC *factdbc = dbcFaction.LookupEntry(item->RequiredFaction);
		if(!factdbc || factdbc->RepListId < 0)
		{ 
			return CAN_AFFORD_ITEM_ERROR_REPUTATION;
		}
		
		if( m_pOwner->GetReputationRankFromStanding( m_pOwner->GetStanding( item->RequiredFaction )) < (int32)item->RequiredFactionStanding )
		{
			return CAN_AFFORD_ITEM_ERROR_REPUTATION;
		}
	}
	return 0;
}

uint8 ItemInterface::CanAffordCurrency(uint32 cur_id,uint32 amount, Creature * pVendor)
{
	ItemExtendedCostEntry * ex = pVendor->GetItemExtendedCostByItemId( cur_id );
	amount = MAX( 1, amount );
	if( ex != NULL )
	{
		for(int i = 0;i<5;i++)
		{
			uint32 req_count = MAX( ex->count[i], 1 ) * amount;
			if(ex->item[i] && m_pOwner->GetItemInterface()->GetItemCount(ex->item[i], false) < req_count )
			{ 
				return CAN_AFFORD_ITEM_ERROR_NOT_ENOUGH_MONEY;
			}
			if( ex->currency[i] )
			{
				uint32 has_count = m_pOwner->GetCurrencyCount( ex->currency[i] );
				uint32 need_count = MAX( 1, ex->currency_count[i] );
				if( need_count > has_count )
					return CAN_AFFORD_ITEM_ERROR_NOT_ENOUGH_MONEY;
			}
		}
		//check depracated currency cost
		if( ex->arena && m_pOwner->GetCurrencyCount( CURRENCY_ARENA_POINT ) < ex->arena )
			return CAN_AFFORD_ITEM_ERROR_NOT_ENOUGH_MONEY;
//		if( ex->honor && m_pOwner->GetCurrencyCount( CURRENCY_HONOR_POINT ) < ex->honor )
//			return CAN_AFFORD_ITEM_ERROR_NOT_ENOUGH_MONEY;
		//not sure this is still used in Cata. I just hope DBC will be updated 
		if(m_pOwner->GetMaxPersonalRating() < ex->personalrating)
		{ 
			return CAN_AFFORD_ITEM_ERROR_NOT_REQUIRED_RANK;
		}
	}
	return 0;
}

//-------------------------------------------------------------------//
//Description: Gets the Item slot by item type
//-------------------------------------------------------------------//
int16 ItemInterface::GetItemSlotByType(uint32 type)
{
	switch(type)
	{
	case INVTYPE_NON_EQUIP:
		return ITEM_NO_SLOT_AVAILABLE; 
	case INVTYPE_HEAD:
		{
			return EQUIPMENT_SLOT_HEAD;
		}
	case INVTYPE_NECK:
		{
			return EQUIPMENT_SLOT_NECK;
		}
	case INVTYPE_SHOULDERS:
		{
			return EQUIPMENT_SLOT_SHOULDERS;
		}
	case INVTYPE_BODY:
		{
			return EQUIPMENT_SLOT_BODY;
		}
	case INVTYPE_CHEST:
		{
			return EQUIPMENT_SLOT_CHEST;
		}
	case INVTYPE_ROBE: // ???
		{
			return EQUIPMENT_SLOT_CHEST;
		}
	case INVTYPE_WAIST:
		{
			return EQUIPMENT_SLOT_WAIST;
		}
	case INVTYPE_LEGS:
		{
			return EQUIPMENT_SLOT_LEGS;
		}
	case INVTYPE_FEET:
		{
			return EQUIPMENT_SLOT_FEET;
		}
	case INVTYPE_WRISTS:
		{
			return EQUIPMENT_SLOT_WRISTS;
		}
	case INVTYPE_HANDS:
		{
			return EQUIPMENT_SLOT_HANDS;
		}
	case INVTYPE_FINGER:
		{
			if (!GetInventoryItem(EQUIPMENT_SLOT_FINGER1))
			{ 
				return EQUIPMENT_SLOT_FINGER1;
			}
			else if (!GetInventoryItem(EQUIPMENT_SLOT_FINGER2))
			{ 
				return EQUIPMENT_SLOT_FINGER2;
			}
			return EQUIPMENT_SLOT_FINGER1; //auto equips always in finger 1
		}
	case INVTYPE_TRINKET:
		{
			if (!GetInventoryItem(EQUIPMENT_SLOT_TRINKET1))
			{ 
				return EQUIPMENT_SLOT_TRINKET1;
			}
			else if (!GetInventoryItem(EQUIPMENT_SLOT_TRINKET2))
			{ 
				return EQUIPMENT_SLOT_TRINKET2;
			}
			return EQUIPMENT_SLOT_TRINKET1; //auto equips always on trinket 1
		}
	case INVTYPE_CLOAK:
		{
			return EQUIPMENT_SLOT_BACK;
		}
	case INVTYPE_WEAPON:
		{
			if (!GetInventoryItem(EQUIPMENT_SLOT_MAINHAND) )
			{ 
				return EQUIPMENT_SLOT_MAINHAND;
			}
			else if(!GetInventoryItem(EQUIPMENT_SLOT_OFFHAND))
			{ 
				return EQUIPMENT_SLOT_OFFHAND;
			}
		}
	case INVTYPE_SHIELD:
		{
			return EQUIPMENT_SLOT_OFFHAND;
		}
	case INVTYPE_RANGED:
		{
			return EQUIPMENT_SLOT_RANGED;
		}
	case INVTYPE_2HWEAPON:
		{
			return EQUIPMENT_SLOT_MAINHAND;
		}
	case INVTYPE_TABARD:
		{
			return EQUIPMENT_SLOT_TABARD;
		}
	case INVTYPE_WEAPONMAINHAND:
		{
			return EQUIPMENT_SLOT_MAINHAND;
		}
	case INVTYPE_WEAPONOFFHAND:
		{
			return EQUIPMENT_SLOT_OFFHAND;
		}
	case INVTYPE_HOLDABLE:
		{
			return EQUIPMENT_SLOT_OFFHAND;
		}
	case INVTYPE_THROWN:
		return EQUIPMENT_SLOT_RANGED; // ?
	case INVTYPE_RANGEDRIGHT:
		return EQUIPMENT_SLOT_RANGED; // ?
	case INVTYPE_RELIC:
		return EQUIPMENT_SLOT_RANGED;
	case INVTYPE_BAG:
		{
			for (int16 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
			{
				if (!GetInventoryItem(i))
				{ 
					return i;
				}
			}
			return ITEM_NO_SLOT_AVAILABLE; //bags are not suposed to be auto-equiped when slots are not free
		}
	default:
		return ITEM_NO_SLOT_AVAILABLE;
	}
}

//-------------------------------------------------------------------//
//Description: Gets a Item by guid
//-------------------------------------------------------------------//
Item* ItemInterface::GetItemByGUID(uint64 Guid)
{
	uint32 i ;

	//EQUIPMENT
	for(i=EQUIPMENT_SLOT_START;i<EQUIPMENT_SLOT_END;i++)
	{
		if(m_pItems[i] != 0)
		{
			if( m_pItems[i]->GetGUID() == Guid)
			{
				result.ContainerSlot = INVALID_BACKPACK_SLOT;//not a containerslot. In 1.8 client marked wrong slot like this
				result.Slot = i;
				return m_pItems[i];
			}
		}
	}

	//INVENTORY BAGS
	for(i=INVENTORY_SLOT_BAG_START;i<INVENTORY_SLOT_BAG_END;i++)
	{
		if(m_pItems[i] != NULL && m_pItems[i]->IsContainer())
		{
			if(m_pItems[i]->GetGUID()==Guid) 
			{
				result.ContainerSlot = INVALID_BACKPACK_SLOT;
				result.Slot = i;
				return m_pItems[i]; 
			}

			for (uint32 j =0; j < m_pItems[i]->GetProto()->ContainerSlots;j++)
			{
				Item *item2 = SafeContainerCast(m_pItems[i])->GetItem(j);
				if (item2)
				{
					if (item2->GetGUID() == Guid)
					{
						result.ContainerSlot = i;
						result.Slot = j;
						return item2;
					}
				}
			}
		}
	}

	//INVENTORY
	for(i=INVENTORY_SLOT_ITEM_START;i<INVENTORY_SLOT_ITEM_END;i++)
	{
		if(m_pItems[i] != 0)
		{
			if(m_pItems[i]->GetGUID() == Guid)
			{
				result.ContainerSlot = INVALID_BACKPACK_SLOT;
				result.Slot = i;
				return m_pItems[i];
			}
		}
	}

	//Keyring
/*	for(i=INVENTORY_KEYRING_START;i<INVENTORY_KEYRING_END;i++)
	{
		if(m_pItems[i] != 0)
		{
			if(m_pItems[i]->GetGUID() == Guid)
			{
				result.ContainerSlot = INVALID_BACKPACK_SLOT;
				result.Slot = i;
				return m_pItems[i];
			}
		}
	}*/
	return NULL;
}

//-------------------------------------------------------------------//
//Description: Inventory Error report
//-------------------------------------------------------------------//
void ItemInterface::BuildInventoryChangeError(Item *SrcItem, Item *DstItem, uint8 Error)
{

	if( !m_pOwner->GetSession() )
		return; // low chances for this

	WorldPacket data(22);

	data.Initialize( SMSG_INVENTORY_CHANGE_FAILURE );
	data << Error;
	data << (SrcItem ? SrcItem->GetGUID() : uint64(0));
	data << (DstItem ? DstItem->GetGUID() : uint64(0));
	data << uint8(0);
	if(Error == INV_ERR_YOU_MUST_REACH_LEVEL_N && SrcItem) 
		data << SrcItem->GetProto()->RequiredLevel;

	m_pOwner->GetSession()->SendPacket( &data );
}

void ItemInterface::EmptyBuyBack()
{
	 for (uint32 j = 0;j < MAX_BUYBACK_SLOT;j++)
	 {
		 if (m_pBuyBack[j] != NULL)
		 {
			m_pBuyBack[j]->DestroyForPlayer(m_pOwner);
			m_pBuyBack[j]->DeleteFromDB();

			if (m_pBuyBack[j]->IsInWorld())
				m_pBuyBack[j]->RemoveFromWorld();

			m_pBuyBack[j]->DeleteMe();

			m_pOwner->SetUInt64Value(PLAYER_FIELD_VENDORBUYBACK_SLOT_1 + (2*j),0);
			m_pOwner->SetUInt32Value(PLAYER_FIELD_BUYBACK_PRICE_1 + j,0);
			m_pOwner->SetUInt32Value(PLAYER_FIELD_BUYBACK_TIMESTAMP_1 + j,0);
			m_pBuyBack[j] = NULL;
		 }
		 else
			 break;
	 }
}

void ItemInterface::AddBuyBackItem(Item *it,uint32 price)
{
	int i;
	if ((m_pBuyBack[MAX_BUYBACK_SLOT-1] != NULL) && (m_pOwner->GetUInt64Value(PLAYER_FIELD_VENDORBUYBACK_SLOT_1 + (MAX_BUYBACK_SLOT-1)*2) != 0))
	{
		if(m_pBuyBack[0] != NULL)
		{		   
			 m_pBuyBack[0]->DestroyForPlayer(m_pOwner);
			 m_pBuyBack[0]->DeleteFromDB();

			 if(m_pBuyBack[0]->IsContainer())
			 {
				if (SafeContainerCast(m_pBuyBack[0])->IsInWorld())
					SafeContainerCast(m_pBuyBack[0])->RemoveFromWorld();
				
				delete SafeContainerCast(m_pBuyBack[0]);
			 }
			 else
			 {
				if (m_pBuyBack[0]->IsInWorld())
					m_pBuyBack[0]->RemoveFromWorld();
				ItemPool.PooledDelete( m_pBuyBack[0], __FILE__, __LINE__ );
			 }

			m_pBuyBack[0] = NULL;
		}

		for (int j = 0;j < MAX_BUYBACK_SLOT-1;j++)
		{
			//SetUInt64Value(PLAYER_FIELD_VENDORBUYBACK_SLOT_1 + (2*j),buyback[j+1]->GetGUID());
			m_pOwner->SetUInt64Value(PLAYER_FIELD_VENDORBUYBACK_SLOT_1 + (2*j),m_pOwner->GetUInt64Value(PLAYER_FIELD_VENDORBUYBACK_SLOT_1 + ((j+1)*2) ) );
			m_pOwner->SetUInt32Value(PLAYER_FIELD_BUYBACK_PRICE_1 + j,m_pOwner->GetUInt32Value(PLAYER_FIELD_BUYBACK_PRICE_1 + j+1));
			m_pOwner->SetUInt32Value(PLAYER_FIELD_BUYBACK_TIMESTAMP_1 + j,m_pOwner->GetUInt32Value(PLAYER_FIELD_BUYBACK_TIMESTAMP_1 + j+1));
			m_pBuyBack[j] = m_pBuyBack[j+1];
		}
		m_pBuyBack[MAX_BUYBACK_SLOT-1] = it;

		m_pOwner->SetUInt64Value(PLAYER_FIELD_VENDORBUYBACK_SLOT_1 + (2*(MAX_BUYBACK_SLOT-1)),m_pBuyBack[MAX_BUYBACK_SLOT-1]->GetGUID());
		m_pOwner->SetUInt32Value(PLAYER_FIELD_BUYBACK_PRICE_1 + MAX_BUYBACK_SLOT-1,price);
		m_pOwner->SetUInt32Value(PLAYER_FIELD_BUYBACK_TIMESTAMP_1 + MAX_BUYBACK_SLOT-1,(uint32)UNIXTIME);
		return;
	}

	for(i=0; i <= (MAX_BUYBACK_SLOT - 1)*2;i+=2) //at least 1 slot is empty
	{
		if((m_pOwner->GetUInt32Value(PLAYER_FIELD_VENDORBUYBACK_SLOT_1 + i) == 0) || (m_pBuyBack[i/2] == NULL))
		{
			sLog.outDetail("setting buybackslot %u\n",i/2);
			m_pBuyBack[i >> 1] = it;

			m_pOwner->SetUInt64Value(PLAYER_FIELD_VENDORBUYBACK_SLOT_1 + i,m_pBuyBack[i >> 1]->GetGUID());
			//SetUInt64Value(PLAYER_FIELD_VENDORBUYBACK_SLOT_1 + i,it->GetGUID());
			m_pOwner->SetUInt32Value(PLAYER_FIELD_BUYBACK_PRICE_1 + (i >> 1),price);
			m_pOwner->SetUInt32Value(PLAYER_FIELD_BUYBACK_TIMESTAMP_1 + (i >> 1),(uint32)UNIXTIME);
			return;
		}
	}
}

void ItemInterface::RemoveBuyBackItem(uint32 index)
{
	int32 j = 0;
	for( j = index;j < MAX_BUYBACK_SLOT-1; j++ )
	{
		if (m_pOwner->GetUInt64Value( PLAYER_FIELD_VENDORBUYBACK_SLOT_1 + ( j * 2 ) ) != 0 )
		{
			m_pOwner->SetUInt64Value( PLAYER_FIELD_VENDORBUYBACK_SLOT_1 + ( 2 * j ), m_pOwner->GetUInt64Value( PLAYER_FIELD_VENDORBUYBACK_SLOT_1 + ( ( j + 1 ) * 2 ) ) );
			m_pOwner->SetUInt32Value( PLAYER_FIELD_BUYBACK_PRICE_1 + j, m_pOwner->GetUInt32Value( PLAYER_FIELD_BUYBACK_PRICE_1 + j+1));
			m_pOwner->SetUInt32Value( PLAYER_FIELD_BUYBACK_TIMESTAMP_1 + j, m_pOwner->GetUInt32Value( PLAYER_FIELD_BUYBACK_TIMESTAMP_1 + j + 1 ) );
			
			if( m_pBuyBack[j+1] != NULL && ( m_pOwner->GetUInt64Value( PLAYER_FIELD_VENDORBUYBACK_SLOT_1 + ( ( j + 1 ) * 2 ) ) != 0 ) )
			{
				m_pBuyBack[j] = m_pBuyBack[j+1];
			}
			else
			{
				if( m_pBuyBack[j] != NULL )
				{
						m_pBuyBack[j] = NULL;
				}

				sLog.outDetail( "nulling %u\n", j );
			}
		}
		else
			return;
	}
	j = 11;
	m_pOwner->SetUInt64Value( PLAYER_FIELD_VENDORBUYBACK_SLOT_1 + ( 2 * j ), m_pOwner->GetUInt64Value( PLAYER_FIELD_VENDORBUYBACK_SLOT_1 + ( ( j + 1 ) * 2 ) ) );
	m_pOwner->SetUInt32Value( PLAYER_FIELD_BUYBACK_PRICE_1 + j, m_pOwner->GetUInt32Value( PLAYER_FIELD_BUYBACK_PRICE_1 + j + 1 ) );
	m_pOwner->SetUInt32Value( PLAYER_FIELD_BUYBACK_TIMESTAMP_1 + j, m_pOwner->GetUInt32Value( PLAYER_FIELD_BUYBACK_TIMESTAMP_1 + j + 1 ) );
	if( m_pBuyBack[MAX_BUYBACK_SLOT-1] )
	{
		m_pBuyBack[MAX_BUYBACK_SLOT-1] = NULL;
	}
	
}

//-------------------------------------------------------------------//
//Description: swap inventory slots
//-------------------------------------------------------------------//
void ItemInterface::SwapItemSlots(int16 srcslot, int16 dstslot)
{
	// srcslot and dstslot are int... NULL might not be an int depending on arch where it is compiled
	if( srcslot >= MAX_INVENTORY_SLOT || srcslot < 0 )
	{ 
		return;
	}

	if( dstslot >= MAX_INVENTORY_SLOT || dstslot < 0 )
	{ 
		return;
	}

	Item* SrcItem = GetInventoryItem( srcslot );
	Item* DstItem = GetInventoryItem( dstslot );

	sLog.outDebug( "ItemInterface::SwapItemSlots(%u, %u);" , srcslot , dstslot );
	//Item * temp = GetInventoryItem( srcslot );
	//if( temp )
	//	sLog.outDebug( "Source item: %s (inventoryType=%u, realslot=%u);" , temp->GetProto()->Name1 , temp->GetProto()->InventoryType , GetItemSlotByType( temp->GetProto()->InventoryType ) );
	//	temp = GetInventoryItem( dstslot );
	//if( temp )
	//	sLog.outDebug( "Destination: Item: %s (inventoryType=%u, realslot=%u);" , temp->GetProto()->Name1 , temp->GetProto()->InventoryType , GetItemSlotByType( temp->GetProto()->InventoryType ) );
	//else
	//	sLog.outDebug( "Destination: Empty" );

	if( SrcItem != NULL && DstItem != NULL && SrcItem->GetEntry()==DstItem->GetEntry() && SrcItem->GetProto()->MaxCount > 1 && SrcItem->wrapped_item_id == 0 && DstItem->wrapped_item_id == 0 )
	{
		uint32 total = SrcItem->GetUInt32Value( ITEM_FIELD_STACK_COUNT ) + DstItem->GetUInt32Value( ITEM_FIELD_STACK_COUNT );
		if( total <= DstItem->GetProto()->MaxCount )
		{
			DstItem->ModUnsigned32Value( ITEM_FIELD_STACK_COUNT, SrcItem->GetUInt32Value( ITEM_FIELD_STACK_COUNT ) );
			SafeFullRemoveItemFromSlot( INVENTORY_SLOT_NOT_SET, srcslot );
			DstItem->m_isDirty = true;
			return;
		}
		else
		{
			if( DstItem->GetUInt32Value( ITEM_FIELD_STACK_COUNT ) == DstItem->GetProto()->MaxCount )
			{

			}
			else
			{
				int32 delta=DstItem->GetProto()->MaxCount-DstItem->GetUInt32Value( ITEM_FIELD_STACK_COUNT );
				DstItem->SetUInt32Value( ITEM_FIELD_STACK_COUNT, DstItem->GetProto()->MaxCount );
				SrcItem->ModUnsigned32Value( ITEM_FIELD_STACK_COUNT, -delta );
				SrcItem->m_isDirty = true;
				DstItem->m_isDirty = true;
				return;
			}
		}
	}

	//src item was equiped previously
	if( srcslot < INVENTORY_SLOT_BAG_END ) 
	{
		if( m_pItems[(int)srcslot] != NULL )		
			m_pOwner->ApplyItemMods( m_pItems[(int)srcslot], srcslot, false );
	}

	//dst item was equiped previously
	if( dstslot < INVENTORY_SLOT_BAG_END ) 
	{
		if( m_pItems[(int)dstslot] != NULL )		
			m_pOwner->ApplyItemMods( m_pItems[(int)dstslot], dstslot, false );
	}

	//sLog.outDebug( "Putting items into slots..." );



    m_pItems[(int)dstslot] = SrcItem;

    // Moving a bag with items to a empty bagslot
    if ( (DstItem == NULL) && (SrcItem->IsContainer()) )
    {
        Item *tSrcItem = NULL;

        for ( uint32 Slot = 0; Slot < SrcItem->GetProto()->ContainerSlots; Slot++ )
        {
            tSrcItem = SafeContainerCast((m_pItems[(int)srcslot]))->GetItem(Slot);
            
            m_pOwner->GetItemInterface()->SafeRemoveAndRetreiveItemFromSlot(srcslot, Slot, false);

            if ( tSrcItem != NULL )
            {
                m_pOwner->GetItemInterface()->SafeAddItem(tSrcItem, dstslot, Slot);
            }
        }
    }

   m_pItems[(int)srcslot] = DstItem;

    // swapping 2 bags filled with items
    if ( DstItem && SrcItem->IsContainer() && DstItem->IsContainer() )
    {
        Item *tDstItem = NULL;
        Item *tSrcItem = NULL;
        uint32 TotalSlots = 0;

        // Determine the max amount of slots to swap
        if ( SrcItem->GetProto()->ContainerSlots > DstItem->GetProto()->ContainerSlots )
            TotalSlots = SrcItem->GetProto()->ContainerSlots;
        else
            TotalSlots = DstItem->GetProto()->ContainerSlots;

        // swap items in the bags
        for( uint32 Slot = 0; Slot < TotalSlots; Slot++ )
        {
            tSrcItem = SafeContainerCast((m_pItems[(int)srcslot]))->GetItem(Slot);
            tDstItem = SafeContainerCast((m_pItems[(int)dstslot]))->GetItem(Slot);

            if ( tSrcItem != NULL )
                m_pOwner->GetItemInterface()->SafeRemoveAndRetreiveItemFromSlot(srcslot, Slot, false);
            if ( tDstItem != NULL )
                m_pOwner->GetItemInterface()->SafeRemoveAndRetreiveItemFromSlot(dstslot, Slot, false);

            if ( tSrcItem != NULL )
                SafeContainerCast((DstItem))->AddItem(Slot, tSrcItem);
            if ( tDstItem != NULL )
                SafeContainerCast((SrcItem))->AddItem(Slot, tDstItem);
        }
    }
	
	if( DstItem != NULL )
		DstItem->m_isDirty = true;
	if( SrcItem != NULL )
		SrcItem->m_isDirty = true;

	if( m_pItems[(int)dstslot] != NULL )
	{
		//sLog.outDebug( "(SrcItem) PLAYER_FIELD_INV_SLOT_HEAD + %u is now %u" , dstslot * 2 , m_pItems[(int)dstslot]->GetGUID() );
		m_pOwner->SetUInt64Value( PLAYER_FIELD_INV_SLOT_HEAD + (dstslot*2),  m_pItems[(int)dstslot]->GetGUID() );
	}
	else
	{
		//sLog.outDebug( "(SrcItem) PLAYER_FIELD_INV_SLOT_HEAD + %u is now 0" , dstslot * 2 );
		m_pOwner->SetUInt64Value( PLAYER_FIELD_INV_SLOT_HEAD + (dstslot*2), 0 );
	}

	if( m_pItems[(int)srcslot] != NULL )
	{
		//sLog.outDebug( "(DstItem) PLAYER_FIELD_INV_SLOT_HEAD + %u is now %u" , dstslot * 2 , m_pItems[(int)srcslot]->GetGUID() );
		m_pOwner->SetUInt64Value( PLAYER_FIELD_INV_SLOT_HEAD + (srcslot*2), m_pItems[(int)srcslot]->GetGUID() );
	}
	else
	{
		//sLog.outDebug( "(DstItem) PLAYER_FIELD_INV_SLOT_HEAD + %u is now 0" , dstslot * 2 );
		m_pOwner->SetUInt64Value( PLAYER_FIELD_INV_SLOT_HEAD + (srcslot*2), 0 );
	}

	if( srcslot < INVENTORY_SLOT_BAG_END )	// source item is equiped
	{
		if( m_pItems[(int)srcslot] ) // dstitem goes into here.
		{
			// Bags aren't considered "visible".
			if( srcslot < EQUIPMENT_SLOT_END )
			{
				int VisibleBase = PLAYER_VISIBLE_ITEM_1_ENTRYID + (srcslot * (PLAYER_VISIBLE_ITEM_2_ENTRYID - PLAYER_VISIBLE_ITEM_1_ENTRYID));
				m_pOwner->SetUInt32Value( VisibleBase, m_pItems[(int)srcslot]->GetEntry() );
				uint32 enchant_val;
				enchant_val = m_pItems[(int)srcslot]->GetUInt32Value( ITEM_FIELD_ENCHANTMENT_1_1 ) | (m_pItems[(int)srcslot]->GetUInt32Value( ITEM_FIELD_ENCHANTMENT_1_1 ) << 16 );
				m_pOwner->SetUInt32Value( VisibleBase + 1, enchant_val );
			}

			// handle bind on equip
			if( m_pItems[(int)srcslot]->GetProto()->Bonding == ITEM_BIND_ON_EQUIP )
				m_pItems[(int)srcslot]->SoulBind();

			//anything that moves cancels spell casting
			if( GetOwner() && GetOwner()->GetCurrentSpell() )
				GetOwner()->GetCurrentSpell()->safe_cancel();
		} 
		else 
		{
			// Bags aren't considered "visible".
			if( srcslot < EQUIPMENT_SLOT_END )
			{
				int VisibleBase = PLAYER_VISIBLE_ITEM_1_ENTRYID + (srcslot * (PLAYER_VISIBLE_ITEM_2_ENTRYID - PLAYER_VISIBLE_ITEM_1_ENTRYID));
				m_pOwner->SetUInt32Value( VisibleBase, 0 );
				m_pOwner->SetUInt32Value( VisibleBase + 1, 0 );
			}
		}
	}  

	if( dstslot < INVENTORY_SLOT_BAG_END )   // source item is inside inventory
	{
		if( m_pItems[(int)dstslot] != NULL ) // srcitem goes into here.
		{	
			// Bags aren't considered "visible".
			if( dstslot < EQUIPMENT_SLOT_END )
			{
				int VisibleBase = PLAYER_VISIBLE_ITEM_1_ENTRYID + (dstslot * (PLAYER_VISIBLE_ITEM_2_ENTRYID-PLAYER_VISIBLE_ITEM_1_ENTRYID));
				m_pOwner->SetUInt32Value( VisibleBase, m_pItems[(int)dstslot]->GetEntry() );
				uint32 enchant_val;
				enchant_val = m_pItems[(int)dstslot]->GetUInt32Value( ITEM_FIELD_ENCHANTMENT_1_1 ) | (m_pItems[(int)dstslot]->GetUInt32Value( ITEM_FIELD_ENCHANTMENT_1_1 ) << 16 );
				m_pOwner->SetUInt32Value( VisibleBase + 1, enchant_val );
			}

			// handle bind on equip
			if( m_pItems[(int)dstslot]->GetProto()->Bonding == ITEM_BIND_ON_EQUIP )
				m_pItems[(int)dstslot]->SoulBind();

		}
		else
		{

			// bags aren't considered visible
			if( dstslot < EQUIPMENT_SLOT_END )
			{
				int VisibleBase = PLAYER_VISIBLE_ITEM_1_ENTRYID + (dstslot * (PLAYER_VISIBLE_ITEM_2_ENTRYID-PLAYER_VISIBLE_ITEM_1_ENTRYID));
				m_pOwner->SetUInt32Value( VisibleBase, 0 );
				m_pOwner->SetUInt32Value( VisibleBase + 1, 0 );
			}
		}
	}

	// handle dual wield
	if( dstslot == EQUIPMENT_SLOT_OFFHAND || srcslot == EQUIPMENT_SLOT_OFFHAND )
	{
		if( m_pItems[EQUIPMENT_SLOT_OFFHAND] != NULL && m_pItems[EQUIPMENT_SLOT_OFFHAND]->GetProto()->Class == ITEM_CLASS_WEAPON )
			m_pOwner->SetDualWield( true );
		else
			m_pOwner->SetDualWield( false );
	}

	//src item is equiped now
	if( srcslot < INVENTORY_SLOT_BAG_END ) 
 	{
		if( m_pItems[(int)srcslot] != NULL )		
			m_pOwner->ApplyItemMods( m_pItems[(int)srcslot], srcslot, true );
 	}

	//dst item is equiped now
	if( dstslot < INVENTORY_SLOT_BAG_END ) 
	{
		if( m_pItems[(int)dstslot] != NULL )		
			m_pOwner->ApplyItemMods( m_pItems[(int)dstslot], dstslot, true );
	}

	// recalculate damage. How come this was not called before ?
	if( srcslot == EQUIPMENT_SLOT_MAINHAND || srcslot == EQUIPMENT_SLOT_OFFHAND || dstslot == EQUIPMENT_SLOT_MAINHAND || dstslot == EQUIPMENT_SLOT_OFFHAND )
		m_pOwner->CalcDamage();

}

//-------------------------------------------------------------------//
//Description: Item Loading
//-------------------------------------------------------------------//
void ItemInterface::mLoadItemsFromDatabase(QueryResult * result)
{
	int8 containerslot, slot;
	Item* item;
	ItemPrototype* proto;

	if( result )
	{
		do
		{
			Field* fields = result->Fetch();

			containerslot = fields[12].GetInt32();
			slot = fields[13].GetInt32();
			proto = ItemPrototypeStorage.LookupEntry(fields[2].GetUInt32());

			if( proto != NULL )
			{
				if( proto->InventoryType == INVTYPE_BAG )
				{
					item = new Container( HIGHGUID_TYPE_CONTAINER, fields[1].GetUInt32() );
					SafeContainerCast( item )->LoadFromDB( fields );

				}
				else
				{
					item = ItemPool.PooledNew( __FILE__, __LINE__ );
					item->Init( HIGHGUID_TYPE_ITEM, fields[1].GetUInt32() );
					item->LoadFromDB( fields, m_pOwner, false);

				}
				AddItemResult res = SafeAddItem( item, containerslot, slot );
				if( res == ADD_ITEM_RESULT_OK )
				    item->m_isDirty = false;
				else if( res == ADD_ITEM_RESULT_ERROR )
					item->DeleteMe();
			}
		}
		while( result->NextRow() );
	}
}

//-------------------------------------------------------------------//
//Description: Item saving
//-------------------------------------------------------------------//
void ItemInterface::mSaveItemsToDatabase(bool first, QueryBuffer * buf)
{
	uint32 x;

	for( x = EQUIPMENT_SLOT_START; x < MAX_INVENTORY_SLOT; ++x )
	{
		if( GetInventoryItem( x ) != NULL )
		{
			if( GetInventoryItem( x )->GetProto() != NULL )
			{
				if( IsBagSlot( x ) && GetInventoryItem( x )->IsContainer() )
				{
					SafeContainerCast( GetInventoryItem( x ) )->SaveBagToDB( x, first, buf );
				}
				else
				{
					GetInventoryItem( x )->SaveToDB( INVENTORY_SLOT_NOT_SET, x, first, buf );
				}
			}
		}
	}
}

AddItemResult ItemInterface::AddItemToFreeBankSlot(Item *item)
{
	//special items first
	for( uint32 i = BANK_SLOT_BAG_START; i < BANK_SLOT_BAG_END; i++ )
	{
		if( m_pItems[i] != NULL )
		{
			if( m_pItems[i]->GetProto()->BagFamily & item->GetProto()->BagFamily )
			{
				if( m_pItems[i]->IsContainer() )
				{
					bool result = SafeContainerCast( m_pItems[i] )->AddItemToFreeSlot( item, NULL );
					if( result )
					{ 
						return ADD_ITEM_RESULT_OK;
					}
				}
			}
		}
	}

	for( uint32 i= BANK_SLOT_ITEM_START; i < BANK_SLOT_ITEM_END; i++ )
	{
		if( m_pItems[i] == NULL )
		{
			return SafeAddItem( item, INVENTORY_SLOT_NOT_SET, i );
		}
	}

	for( uint32 i = BANK_SLOT_BAG_START; i < BANK_SLOT_BAG_END; i++ )
	{
		if( m_pItems[i] != NULL && m_pItems[i]->GetProto()->BagFamily == 0 && m_pItems[i]->IsContainer() ) //special bags ignored
		{
			for( uint32 j =0; j < m_pItems[i]->GetProto()->ContainerSlots; j++ )
			{
				Item *item2 = SafeContainerCast( m_pItems[i] )->GetItem(j);
				if( item2 == NULL )
				{
					return SafeAddItem( item, i, j );
				}
			}
		}
	}
	return ADD_ITEM_RESULT_ERROR;
}

int16 ItemInterface::FindSpecialBag(Item *item)
{
	for( uint16 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++ )
	{
		if( m_pItems[i] != NULL )
		{
			if( m_pItems[i]->GetProto()->BagFamily & item->GetProto()->BagFamily )
			{
				return i;
			}
		}
	}
	return ITEM_NO_SLOT_AVAILABLE;
}
/*
int16 ItemInterface::FindFreeKeyringSlot()
{
	for( uint16 i = INVENTORY_KEYRING_START; i < INVENTORY_KEYRING_END; i++ )
	{
		if( m_pItems[i] == NULL )
		{
			return i;
		}
	}
	return ITEM_NO_SLOT_AVAILABLE;
}*/

SlotResult ItemInterface::FindFreeInventorySlot(ItemPrototype *proto)
{
	//special item
	//special slots will be ignored of item is not set
	if( proto != NULL )
	{
		//sLog.outDebug( "ItemInterface::FindFreeInventorySlot called for item %s" , proto->Name1 );
		if( proto->BagFamily)
		{
/*			if( proto->BagFamily & ITEM_TYPE_KEYRING || proto->Class == ITEM_CLASS_KEY )
			{
				for(uint32 i = INVENTORY_KEYRING_START; i < INVENTORY_KEYRING_END; i++ )
				{
					if( m_pItems[i] == NULL )
					{
						result.ContainerSlot = ITEM_NO_SLOT_AVAILABLE;
						result.Slot = i;
						result.Result = true;
						return result;
					}
				}
			}
			else */
			{
				for( uint32 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++ )
				{
					if( m_pItems[i] != NULL && m_pItems[i]->IsContainer() )
					{
						if( m_pItems[i]->GetProto()->BagFamily & proto->BagFamily )
						{
							int32 slot = SafeContainerCast( m_pItems[i] )->FindFreeSlot();
							if( slot != ITEM_NO_SLOT_AVAILABLE ) 
							{
								result.ContainerSlot = i;
								result.Slot = slot;
								result.Result = true;
								return result;
							}
						}
					}
				}
			}
		}
	}

	//backpack
	for( uint32 i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; i++ )
	{
		Item *item = GetInventoryItem( i );
		if( item == NULL ) 
		{
			result.ContainerSlot = ITEM_NO_SLOT_AVAILABLE;
			result.Slot = i;
			result.Result = true;
			return result;
		}
	}

	//bags
	for( uint32 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++ )
	{
		Item* item = GetInventoryItem(i);
		if( item != NULL )
		{
			if( item->IsContainer() && !item->GetProto()->BagFamily )
			{
				int32 slot = SafeContainerCast( m_pItems[i] )->FindFreeSlot();
				if( slot != ITEM_NO_SLOT_AVAILABLE ) 
				{ 
					result.ContainerSlot = i;
					result.Slot = slot;
					result.Result = true;
					return result;
				}
			}
		}
	}

	result.ContainerSlot = ITEM_NO_SLOT_AVAILABLE;
	result.Slot = ITEM_NO_SLOT_AVAILABLE;
	result.Result = false;

	return result;
}

SlotResult ItemInterface::FindFreeBankSlot(ItemPrototype *proto)
{
	//special item
	//special slots will be ignored of item is not set
	if( proto != NULL )
	{
		if( proto->BagFamily )
		{
			for( uint32 i = BANK_SLOT_BAG_START; i < BANK_SLOT_BAG_END; i++ )
			{
				if( m_pItems[i] != NULL && m_pItems[i]->IsContainer() )
				{
					if( m_pItems[i]->GetProto()->BagFamily & proto->BagFamily )
					{
						int32 slot = SafeContainerCast( m_pItems[i] )->FindFreeSlot();
						if( slot != ITEM_NO_SLOT_AVAILABLE ) 
						{
							result.ContainerSlot = i;
							result.Slot = slot;
							result.Result = true;
							return result;
						}
					}
				}
			}
		}
	}

	//backpack
	for( uint32 i = BANK_SLOT_ITEM_START; i < BANK_SLOT_ITEM_END; i++ )
	{
		Item* item = GetInventoryItem( i );
		if( item == NULL ) 
		{
			result.ContainerSlot = ITEM_NO_SLOT_AVAILABLE;
			result.Slot = i;
			result.Result = true;
			return result;
		}
	}

	//bags
	for( uint32 i = BANK_SLOT_BAG_START; i < BANK_SLOT_BAG_END; i++ )
	{
		Item* item = GetInventoryItem(i);
		if( item != NULL )
		{
			if( item->IsContainer() && !item->GetProto()->BagFamily )
			{
				int32 slot = SafeContainerCast( m_pItems[i] )->FindFreeSlot();
				if( slot != ITEM_NO_SLOT_AVAILABLE) 
				{ 
					result.ContainerSlot = i;
					result.Slot = slot;
					result.Result = true;
					return result;
				}
			}
		}
	}

	result.ContainerSlot = ITEM_NO_SLOT_AVAILABLE;
	result.Slot = ITEM_NO_SLOT_AVAILABLE;
	result.Result = false;

	return result;
}

SlotResult ItemInterface::FindAmmoBag()
{
	for( uint32 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++ )
		if( m_pItems[i] != NULL && m_pItems[i]->IsAmmoBag())
		{
			result.ContainerSlot = ITEM_NO_SLOT_AVAILABLE;
			result.Slot = i;
			result.Result = true;
			return result;
		}

	result.ContainerSlot = ITEM_NO_SLOT_AVAILABLE;
	result.Slot = ITEM_NO_SLOT_AVAILABLE;
	result.Result = false;

	return result;
}

void ItemInterface::ReduceItemDurability()
{
	uint32 f = RandomUInt(100);
	if( f <= 10 ) //10% chance to loose 1 dur from a random valid item.
	{
		int32 slot = RandomUInt( EQUIPMENT_SLOT_END );
		Item* pItem = GetInventoryItem( INVENTORY_SLOT_NOT_SET, slot );
		if( pItem != NULL )
		{
			if( pItem->GetUInt32Value( ITEM_FIELD_DURABILITY) && pItem->GetUInt32Value( ITEM_FIELD_MAXDURABILITY ) )
			{
				pItem->SetUInt32Value( ITEM_FIELD_DURABILITY, ( pItem->GetUInt32Value( ITEM_FIELD_DURABILITY ) - 1 ) );
				pItem->m_isDirty = true;
				//check final durabiity
				if( !pItem->GetUInt32Value( ITEM_FIELD_DURABILITY ) ) //no dur left
				{
					this->GetOwner()->ApplyItemMods( pItem, slot, false, true );
				}
			}
		}
	}
}

bool ItemInterface::IsEquipped(uint32 itemid)
{
	for( uint32 x = EQUIPMENT_SLOT_START; x < EQUIPMENT_SLOT_END; ++x )
	{
		Item * it = m_pItems[x];

		if( it != NULL )
		{
			if( it->GetProto()->ItemId == itemid )
			{ 
 				return true;
			}

			// check gems as well
			for( uint32 count=ITEM_ENCHANT_SLOT_GEM1; count<=ITEM_ENCHANT_SLOT_GEM3; count++ )
			{
				EnchantmentInstance *ei = it->GetEnchantment(count);
				
				if (ei && ei->Enchantment && ei->Enchantment->GemEntry)
				{
					ItemPrototype * ip = ItemPrototypeStorage.LookupEntry(ei->Enchantment->GemEntry);
					
					if (ip && ip->ItemId == itemid)
					{ 
						return true;
					}
				}
			}
		}
	}
	return false;
}

/*
bool ItemInterface::IsCategoryEquipped(uint32 category)
{
	uint32 equip_count = 0;
	for( uint32 x = EQUIPMENT_SLOT_START; x < EQUIPMENT_SLOT_END; ++x )
	{
		Item * it = m_pItems[x];

		if( it != NULL )
		{
			if( it->GetProto()->ItemLimitCategory == category )
				equip_count++;

			// check gems as well
			for( uint32 count=0; count<it->GetSocketsCount(); count++ )
			{
				EnchantmentInstance *ei = it->GetEnchantment(2+count);
				
				if (ei && ei->Enchantment && ei->Enchantment->GemEntry)
				{
					ItemPrototype * ip = ItemPrototypeStorage.LookupEntry(ei->Enchantment->GemEntry);
					
					if (ip && ip->ItemLimitCategory == category)
						equip_count++;
				}
			}
		}
	}
	ItemLimitCategory *ilc = dbcItemLimitCategory.LookupEntryForced( category );
	if( ilc->MaxStack <= equip_count )
		return false;
	return true;
}
*/

bool ItemInterface::CanCustomCategoryEquip(ItemPrototype *newequip)
{
	bool IgnoreSameEntry = ( newequip->Unique == 0 && (newequip->Flags & ITEM_FLAG_UNIQUE_EQUIP)==0 && newequip->ItemLimitCategory == 0 );
	int32 gems_found = 1;
	int32 gems_count_limit = 66;
	for( uint32 x = EQUIPMENT_SLOT_START; x < EQUIPMENT_SLOT_END; ++x )
	{
		Item * it = m_pItems[x];

		if( it != NULL )
		{
			if( (IgnoreSameEntry == false || it->GetProto()->ItemId != newequip->ItemId) 
				&& it->GetProto()->CustomCategoryId == newequip->CustomCategoryId )
				return false;

			// check gems as well
			for( uint32 count=ITEM_ENCHANT_SLOT_GEM1; count<=ITEM_ENCHANT_SLOT_GEM3; count++ )
			{
				EnchantmentInstance *ei = it->GetEnchantment(count);
				
				if (ei && ei->Enchantment && ei->Enchantment->GemEntry)
				{
					ItemPrototype * ip = ItemPrototypeStorage.LookupEntry(ei->Enchantment->GemEntry);
					
					if (ip && (IgnoreSameEntry == false || ip->ItemId != newequip->ItemId) 
						&& ip->CustomCategoryId == newequip->CustomCategoryId 
//						&& ei->Enchantment->custom_enchant_maxstack <= 1	//larger numbers are complicated to check, we will simply ignore the gem bonus on equip
						)
					{
						gems_found++;
						gems_count_limit = ei->Enchantment->custom_enchant_maxstack;
//						return true;
					}
				}
			}
		}
	}
	if( gems_count_limit < gems_found )
		return false;
	return true;
}

void ItemInterface::CheckAreaItems()
{
	for( uint32 x = EQUIPMENT_SLOT_START; x < INVENTORY_SLOT_ITEM_END; ++x )
	{
		if( m_pItems[x] != NULL )
		{
			if( IsBagSlot(x) && m_pItems[x]->IsContainer() )
			{
				Container* bag = SafeContainerCast(m_pItems[x]);
 
				for( uint32 i = 0; i < bag->GetProto()->ContainerSlots; i++ )
				{
					if( bag->GetItem(i) != NULL && bag->GetItem(i)->GetProto() && bag->GetItem(i)->GetProto()->MapID && bag->GetItem(i)->GetProto()->MapID != GetOwner()->GetMapId() )
						bag->SafeFullRemoveItemFromSlot( i );
				}
			}
			else
			{
				if( m_pItems[x]->GetProto() != NULL && m_pItems[x]->GetProto()->MapID && m_pItems[x]->GetProto()->MapID != GetOwner()->GetMapId() )
					SafeFullRemoveItemFromSlot( INVENTORY_SLOT_NOT_SET, x );
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
//
//  Adds an item to the Player by itemid
//  refactored from Level1.cpp
//
/////////////////////////////////////////////////////////////////////////////
Item *ItemInterface::AddItemById( uint32 itemid, uint32 count, int32 randomprop )
{
	if( count == 0 )
		return NULL;

    Player *chr = GetOwner();

    // checking if the iteminterface has owner, impossible to not have one
    if( chr == NULL )
        return NULL;

	ItemPrototype* it = ItemPrototypeStorage.LookupEntry(itemid);
	if(it == NULL )
		return NULL;

//	uint32 maxStack = chr->ItemStackCheat ? 0x7fffffff : it->MaxCount;
	uint32 maxStack = it->MaxCount;
	uint32 toadd;
	bool freeslots = true;
	Item *item = NULL;
	
	count = MIN( count, 5000 ); //well, maybe you can add more, but let's not deadlock pls
	while( count > 0 && freeslots )
	{
		if( count < maxStack )
		{
			// find existing item with free stack
			Item* free_stack_item = FindItemLessMax( itemid, count, false );
			if( free_stack_item != NULL )
			{
				// increase stack by new amount
				free_stack_item->ModUnsigned32Value( ITEM_FIELD_STACK_COUNT, count );
				free_stack_item->m_isDirty = true;
				return free_stack_item;
			}
		}
		
		// create new item
		item = objmgr.CreateItem( itemid, chr );
		if( item == NULL )
			return NULL;

		if( it->Bonding == ITEM_BIND_ON_PICKUP )
			if( it->Flags & ITEM_FLAG_ACCOUNTBOUND ) // don't "Soulbind" account-bound items
				item->AccountBind();
			else
				item->SoulBind();
		
		// Let's try to autogenerate randomprop / randomsuffix
		if( randomprop == 0 )
			item->Randomize();
		
		toadd = count > maxStack ? maxStack : count;
		toadd = MAX( 1, toadd );

		item->SetUInt32Value( ITEM_FIELD_STACK_COUNT, toadd );
		if( AddItemToFreeSlot( &item ) )
		{
			SlotResult *lr = LastSearchResult();
            
            chr->GetSession()->SendItemPushResult( item, false, true, false, true, lr->ContainerSlot, lr->Slot, toadd );
			chr->Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_OWN_ITEM,item->GetUInt32Value(OBJECT_FIELD_ENTRY),ACHIEVEMENT_UNUSED_FIELD_VALUE, toadd, ACHIEVEMENT_EVENT_ACTION_ADD);
			count -= toadd;
		}
		else
		{
			freeslots = false;
            chr->GetSession()->SendNotification("No free slots were found in your inventory!");
			item->DeleteMe();
		}
	}
	return item;
}

bool ItemInterface::HasItemWithTotemCategory(uint32 category)
{
	uint32 i = 0;
	for(i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; i++)
	{
		Item *item = GetInventoryItem(i);
		if( item && item->GetProto()->TotemCategory == category )
			return true; 
	}

	for(i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
	{
		Item *item = GetInventoryItem(i);
		if(item && item->IsContainer())
		{
			  for (uint32 j =0; j < item->GetProto()->ContainerSlots; j++)
				{
					Item *item2 = SafeContainerCast(item)->GetItem(j);
					if( item2 && item2->GetProto()->TotemCategory == category )
						return true;
				}
			
		}
	}
	return false;
}

uint32 ItemInterface::GetInventoryItemDisplay(int16 slot)
{
//	Item *it = GetInventoryItem( slot );
//	if(it && it->GetProto() )
//		return it->GetProto()->DisplayInfoID;
//	return 0;
	uint32 Entry = m_pOwner->GetUInt32Value( PLAYER_VISIBLE_ITEM_1_ENTRYID + slot * ( PLAYER_VISIBLE_ITEM_2_ENTRYID - PLAYER_VISIBLE_ITEM_1_ENTRYID ) );
	if( Entry == 0 )
		return 0;
	ItemPrototype* it = ItemPrototypeStorage.LookupEntry( Entry );
	if(it == NULL )
		return NULL;
	return it->DisplayInfoID;
}
