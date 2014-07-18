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
//safe way to handle arrays in debug mode
#include <array>

#ifndef _CONTAINER_H
#define _CONTAINER_H

#define MAX_CONTAINER_SLOTS ( CONTAINER_FIELD_SLOT_1_71 - CONTAINER_FIELD_SLOT_1 )

class SERVER_DECL Container : public Item
{
public:
	friend class WorldSession;
	Container(uint32 high, uint32 low);
	~Container();

	void Create( uint32 itemid, Player *owner );
	void LoadFromDB( Field*fields);

	bool AddItem(uint16 slot, Item *item);
	bool AddItemToFreeSlot(Item *pItem, uint32 * r_slot);
	Item *GetItem(uint16 slot)
	{
		if( (uint16)slot < GetProto()->ContainerSlots 
//			&& m_Slot != NULL //wtf crashfix ? I bet something else is the problem here
			)
			return m_Slot[slot];
		else
			return 0;
	}
	
	uint32 GetNumSlots() { return GetUInt32Value(CONTAINER_FIELD_NUM_SLOTS); }
	int16 FindFreeSlot();
	bool HasItems();
	
	void SwapItems(int16 SrcSlot,int16 DstSlot);
	Item *SafeRemoveAndRetreiveItemFromSlot(int16 slot, bool destroy); //doesnt destroy item from memory
	bool SafeFullRemoveItemFromSlot(int16 slot); //destroys item fully
   
	void SaveBagToDB(int16 slot, bool first, QueryBuffer * buf);

protected:
	//Item **m_Slot;
	tr1::array< Item *, MAX_CONTAINER_SLOTS> m_Slot;

	uint32 __fields[CONTAINER_END];
};

#endif
