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

initialiseSingleton(LootMgr);

struct loot_tb
{
	uint32 itemid;
	float chance;
};

template <class T>  // works for anything that has the field 'chance' and is stored in plain array
const T& RandomChoice( const T* variant, int count )
{
  float totalChance = 0;
  for( int i = 0; i < count; i++)
    totalChance += variant[i].chance;
  float val = RandomFloat(totalChance);
  for( int i = 0; i < count; i++)
  {
    val -= variant[i].chance;
    if (val <= 0) return variant[i];
  }
  // should not come here, buf if it does, we should return something reasonable
  return variant[count-1];
}

template <class T>  // works for anything that has the field 'chance' and is stored in plain array
T* RandomChoiceVector( vector<pair<T*, float> > & variant )
{
	float totalChance = 0;
	float val;
	typename vector<pair<T*,float> >::iterator itr;

	if(variant.empty() == true)
		return NULL;

	for(itr = variant.begin(); itr != variant.end(); ++itr)
		totalChance += itr->second;

	val = RandomFloat(totalChance);
	
	for(itr = variant.begin(); itr != variant.end(); ++itr)
	{
		val -= itr->second;
		if (val <= 0) return itr->first;
	}
	// should not come here, buf if it does, we should return something reasonable
	return variant.begin()->first;
}

LootMgr::LootMgr()
{
	is_loading = false;
}

void LootMgr::LoadLoot()
{
	//THIS MUST BE CALLED AFTER LOADING OF ITEMS
	is_loading = true;
	LoadLootProp();
//	LoadLootTables("loot_creatures",&CreatureLoot);
	LoadLootTables("loot_gameobjects",&GOLoot);
//	LoadLootTables("loot_skinning",&SkinningLoot);
	LoadLootTables("loot_fishing",&FishingLoot);
	LoadLootTables("loot_items", &ItemLoot);
//	LoadLootTables("loot_prospecting", &ProspectingLoot);
//	LoadLootTables("loot_milling", &MillingLoot);
//	LoadLootTables("loot_disenchanting", &DisenchantingLoot);
//	LoadLootTables("loot_pickpocketing", &PickpocketingLoot);
	is_loading = false;
}

RandomProps * LootMgr::GetRandomProperties(ItemPrototype * proto)
{
	map<uint32,RandomPropertyVector>::iterator itr;

	if(proto->RandomPropId==0)
	{ 
		return NULL;
	}

    itr = _randomprops.find(proto->RandomPropId);
	if(itr==_randomprops.end())
	{ 
		return NULL;
	}

	//pick a random enchantment from the vector of enchantments
	uint32 total_size = (uint32)itr->second.size();
	uint32 selected_index = RandomUInt() % total_size;

//	return RandomChoiceVector<RandomProps>(itr->second);
	return itr->second[ selected_index ].first;
}

ItemRandomSuffixEntry * LootMgr::GetRandomSuffix(ItemPrototype * proto)
{
	map<uint32,RandomSuffixVector>::iterator itr;

	if(proto->RandomSuffixId==0)
	{ 
		return NULL;
	}

//	itr = _randomsuffix.find(proto->RandomSuffixId);
	itr = _randomsuffix.find( proto->ItemId );
	if(itr==_randomsuffix.end())
	{ 
		return NULL;
	}

	uint32 max_index = (uint32)itr->second.size();
	uint32 picked_index = RandomUInt() % max_index;
	return itr->second[picked_index].first;
}


void LootMgr::LoadLootProp()
{	
	QueryResult * result = WorldDatabase.Query("SELECT * FROM item_randomprop_groups");
	uint32 id, eid;
	RandomProps * rp;
	ItemRandomSuffixEntry * rs;
	float ch;

	if(result)
	{
		map<uint32, RandomPropertyVector>::iterator itr;
		do 
		{
			id = result->Fetch()[0].GetUInt32();
			eid = result->Fetch()[1].GetUInt32();
			ch = result->Fetch()[2].GetFloat();

			rp = dbcRandomProps.LookupEntryForced(eid);
			if(rp == NULL)
			{
				Log.Error("LoadLootProp", "RandomProp group %u references non-existant randomprop %u.", id, eid);
				continue;
			}

            itr = _randomprops.find(id);
			if(itr == _randomprops.end())
			{
				RandomPropertyVector v;
				v.push_back(make_pair(rp, ch));
				_randomprops.insert(make_pair(id, v));
			}
			else
			{
				itr->second.push_back(make_pair(rp,ch));
			}
		} while(result->NextRow());
		delete result;
		result = NULL;
	}

	result = WorldDatabase.Query("SELECT * FROM item_randomsuffix_groups");
	if(result)
	{
		map<uint32, RandomSuffixVector>::iterator itr;
		do 
		{
			id = result->Fetch()[0].GetUInt32();
			eid = result->Fetch()[1].GetUInt32();
			ch = result->Fetch()[2].GetFloat();

			rs = dbcItemRandomSuffix.LookupEntryForced(eid);
			if(rs == NULL)
			{
				Log.Error("LoadLootProp", "RandomSuffix group %u references non-existant randomsuffix %u.", id, eid);
				continue;
			}

			itr = _randomsuffix.find(id);
			if(itr == _randomsuffix.end())
			{
				RandomSuffixVector v;
				v.push_back(make_pair(rs, ch));
				_randomsuffix.insert(make_pair(id, v));
			}
			else
			{
				itr->second.push_back(make_pair(rs,ch));
			}
		} while(result->NextRow());
		delete result;
		result = NULL;
	}   
}

LootMgr::~LootMgr()
{
  sLog.outString("  Deleting Loot Tables...");
  for(LootStore::iterator iter=CreatureLoot.begin(); iter != CreatureLoot.end(); ++iter)
  {
  	delete [] iter->second.items;
	iter->second.items = NULL;
  }

  for(LootStore::iterator iter=FishingLoot.begin(); iter != FishingLoot.end(); ++iter)
  {
  	delete [] iter->second.items;
	iter->second.items = NULL;
  }

  for(LootStore::iterator iter=SkinningLoot.begin(); iter != SkinningLoot.end(); ++iter)
  {
  	delete [] iter->second.items;
	iter->second.items = NULL;
  }

  for(LootStore::iterator iter=GOLoot.begin(); iter != GOLoot.end(); ++iter)
  {
  	delete [] iter->second.items;
	iter->second.items = NULL;
  }

  for(LootStore::iterator iter=ItemLoot.begin(); iter != ItemLoot.end(); ++iter)
  {
	delete [] iter->second.items;
	iter->second.items = NULL;
  }

/*  for(LootStore::iterator iter=ProspectingLoot.begin(); iter != ProspectingLoot.end(); ++iter)
  {
  	delete [] iter->second.items;
	iter->second.items = NULL;
  }

  for(LootStore::iterator iter=MillingLoot.begin(); iter != MillingLoot.end(); ++iter)
  {
  	delete [] iter->second.items;
	iter->second.items = NULL;
  }

  for(LootStore::iterator iter=DisenchantingLoot.begin(); iter != DisenchantingLoot.end(); ++iter)
  {
	delete [] iter->second.items;
	iter->second.items = NULL;
  }*/

  for(LootStore::iterator iter=PickpocketingLoot.begin(); iter != PickpocketingLoot.end(); ++iter)
  {
	delete [] iter->second.items;
	iter->second.items = NULL;
  }
}

void LootMgr::LoadLootTables(const char * szTableName,LootStore * LootTable)
{
  /*  DBCFile *dbc = new DBCFile();
	dbc->open("DBC/ItemRandomProperties.dbc");
	_propCount = dbc->getRecordCount();
	delete dbc;*/
	//HM_NAMESPACE::hash_map<uint32, std::vector<loot_tb> > loot_db;
	//HM_NAMESPACE::hash_map<uint32, std::vector<loot_tb> >::iterator itr;
	vector< pair< uint32, vector< tempy > > > db_cache;
	vector< pair< uint32, vector< tempy > > >::iterator itr;
	db_cache.reserve(10000);
	LootStore::iterator tab;
	QueryResult *result =WorldDatabase.Query("SELECT * FROM %s ORDER BY entryid ASC",szTableName);
	if(!result)
	{
		Log.Error("LootMgr", "Loading loot from table %s failed.", szTableName);
		return;
	}
	uint32 entry_id = 0;
	uint32 last_entry = 0;

	uint32 total =(uint32) result->GetRowCount();
	int pos = 0;
	vector< tempy > ttab;
	tempy t;
	do 
	{	 
		Field *fields = result->Fetch();
		entry_id = fields[1].GetUInt32();
		if(entry_id < last_entry)
		{
			Log.Error("LootMgr", "WARNING: Out of order loot table being loaded.\n");
			return;
		}
		if(entry_id != last_entry)
		{
			if(last_entry != 0)
				db_cache.push_back( make_pair( last_entry, ttab) );
			ttab.clear();
		}

		t.itemid = fields[2].GetUInt32();
		t.chance = fields[3].GetFloat();
		t.chance_2 = fields[4].GetFloat();
		t.mincount = fields[5].GetUInt32();
		t.maxcount = fields[6].GetUInt32();
		t.ffa_loot = fields[7].GetUInt32();

		ttab.push_back( t );

		last_entry = entry_id;
	} while( result->NextRow() );
	//last list was not pushed in
	if(last_entry != 0 && ttab.size())
		db_cache.push_back( make_pair( last_entry, ttab) );
	pos = 0;
	total = (uint32)db_cache.size();
	ItemPrototype* proto;
	uint32 itemid;

	//for(itr=loot_db.begin();itr!=loot_db.end();++itr)
	for( itr = db_cache.begin(); itr != db_cache.end(); ++itr)
	{
		entry_id = (*itr).first;
		if(LootTable->end()==LootTable->find(entry_id))
		{
			StoreLootList list;
			//list.count = itr->second.size();			
			list.count = (uint32)(*itr).second.size();
			list.items=new StoreLootItem[list.count];
		
			uint32 ind=0;
			//for(std::vector<loot_tb>::iterator itr2=itr->second.begin();itr2!=itr->second.end();++itr2)
			for(vector< tempy >::iterator itr2 = (*itr).second.begin(); itr2 != (*itr).second.end(); ++itr2)
			{
				//Omit items that are not in db to prevent future bugs
				//uint32 itemid=(*itr2).itemid;
				itemid = itr2->itemid;
				proto=ItemPrototypeStorage.LookupEntry(itemid);
				if(!proto)
				{
					list.items[ind].item.itemproto=NULL;
					Log.Warning("LootMgr", "Loot for %u contains non-existant item. (%s)",entry_id, szTableName);
				}
				else
				{
					list.items[ind].item.itemproto=proto;
					list.items[ind].item.displayid=proto->DisplayInfoID;
					//list.items[ind].chance=(*itr2).chance;
					list.items[ind].chance= itr2->chance;
					list.items[ind].chance2 = itr2->chance_2;
					list.items[ind].mincount = itr2->mincount;
					list.items[ind].maxcount = itr2->maxcount;
					list.items[ind].ffa_loot = itr2->ffa_loot;

					if(LootTable == &GOLoot)
					{
						if(proto->Class == ITEM_CLASS_QUEST)
						{
							//printf("Quest item \"%s\" allocated to quest ", proto->Name1.c_str());
							sQuestMgr.SetGameObjectLootQuest(itr->first, itemid);
							quest_loot_go[entry_id].insert(proto->ItemId);
						}
					}
				}
				ind++;
			}
			(*LootTable)[entry_id]=list;
		}
	}

	//sLog.outString("  %d loot templates loaded from %s", db_cache.size(), szTableName);
 //   loot_db.clear();
	delete result;
	result = NULL;
}

void LootMgr::PushLoot(StoreLootList *list,Loot * loot, uint32 instance_difficulty)
{
	uint32 i;
	uint32 count;

	for( uint32 x = 0; x < list->count; x++ )
	{
		if( list->items[x].item.itemproto )// this check is needed until loot DB is fixed
		{
			float chance;

			if( instance_difficulty == INSTANCE_MODE_RAID_25_MAN_HEROIC )
				chance = list->items[x].chance2 ;
			else if( instance_difficulty == INSTANCE_MODE_RAID_10_MAN_HEROIC )
				chance = list->items[x].chance2 / 2.5f;
			else if( instance_difficulty == INSTANCE_MODE_RAID_10_MAN )
				chance = list->items[x].chance / 2.5f;
//			else if( instance_difficulty == INSTANCE_MODE_DUNGEON_HEROIC )
//				chance = list->items[x].chance * 2;
			else 
				chance = list->items[x].chance;

			if(chance == 0.0f) 
				continue;
			
			ItemPrototype *itemproto = list->items[x].item.itemproto;
			if( RandChance( chance * sWorld.getRate( RATE_DROP0 + itemproto->Quality ) ) )//|| itemproto->Class == ITEM_CLASS_QUEST)
			{
				if( list->items[x].mincount == list->items[x].maxcount )
					count = list->items[x].maxcount;
				else
					count = RandomUInt(list->items[x].maxcount - list->items[x].mincount) + list->items[x].mincount;

				for( i = 0; i < loot->items.size(); ++i )
				{
					//itemid rand match a already placed item, if item is stackable and unique(stack), increment it, otherwise skips
					if((loot->items[i].item.itemproto == list->items[x].item.itemproto) && itemproto->MaxCount && ((loot->items[i].iItemsCount + count) < itemproto->MaxCount))
					{
						if(itemproto->Unique && ((loot->items[i].iItemsCount+count) < itemproto->Unique))
						{
							loot->items[i].iItemsCount += count;
							break;
						}
						else if (!itemproto->Unique)
						{
							loot->items[i].iItemsCount += count;
							break;
						}
					}
				}

				if( i != loot->items.size() )
					continue;

				__LootItem itm;
				itm.item =list->items[x].item;
				itm.iItemsCount = count;
				itm.roll = NULL;
				itm.passed = false;
				itm.ffa_loot = list->items[x].ffa_loot;
				itm.has_looted.clear();
				
				if( itemproto->Quality > 1 && itemproto->ContainerSlots == 0 )
				{
					itm.iRandomProperty=GetRandomProperties( itemproto );
					itm.iRandomSuffix=GetRandomSuffix( itemproto );
				}
				else
				{
					// save some calls :P
					itm.iRandomProperty = NULL;
					itm.iRandomSuffix = NULL;
				}

				loot->items.push_back(itm);
			}
		}
	}
	if( loot->items.size() > sWorld.MaxLootListSize )
	{
		std::vector<__LootItem>::iterator item_to_remove;
		std::vector<__LootItem>::iterator itr;
		uint32 item_quality;
		bool quest_item;
		while( loot->items.size() > sWorld.MaxLootListSize )
		{
			item_to_remove = loot->items.begin();
			item_quality = 0;
			quest_item = false;
			for( itr = loot->items.begin(); itr != loot->items.end(); ++itr )
			{
				item_quality = (*itr).item.itemproto->Quality;
				quest_item = (*itr).item.itemproto->Class == ITEM_CLASS_QUEST;
				if( (*item_to_remove).item.itemproto->Quality > item_quality && !quest_item )
				{
					item_to_remove = itr;
					break;
				}
			}
			loot->items.erase( item_to_remove );
		}
	}

}

void LootMgr::AddLoot(Loot * loot, uint32 itemid, uint32 mincount, uint32 maxcount, uint32 ffa_loot)
{
	uint32 i;
	uint32 count;
	ItemPrototype *itemproto = ItemPrototypeStorage.LookupEntry(itemid);

	if( itemproto )// this check is needed until loot DB is fixed
	{
		if( mincount == maxcount )
			count = maxcount;
		else
			count = RandomUInt(maxcount - mincount) + mincount;

		for( i = 0; i < loot->items.size(); ++i )
		{
			//itemid rand match a already placed item, if item is stackable and unique(stack), increment it, otherwise skips
			if((loot->items[i].item.itemproto == itemproto) && itemproto->MaxCount && ((loot->items[i].iItemsCount + count) < itemproto->MaxCount))
			{
				if(itemproto->Unique && ((loot->items[i].iItemsCount+count) < itemproto->Unique))
				{
					loot->items[i].iItemsCount += count;
					break;
				}
				else if (!itemproto->Unique)
				{
					loot->items[i].iItemsCount += count;
					break;
				}
			}
		}

		if( i != loot->items.size() )
		{ 
			return;
		}

		_LootItem item;
		item.itemproto = itemproto;
		item.displayid = itemproto->DisplayInfoID;

		__LootItem itm;
		itm.item = item;
		itm.iItemsCount = count;
		itm.roll = NULL;
		itm.passed = false;
		itm.ffa_loot = ffa_loot;
		itm.has_looted.clear();
		
		if( itemproto->Quality > ITEM_QUALITY_NORMAL_WHITE && itemproto->ContainerSlots == 0 )
		{
			itm.iRandomProperty=GetRandomProperties( itemproto );
			itm.iRandomSuffix=GetRandomSuffix( itemproto );
		}
		else
		{
			// save some calls :P
			itm.iRandomProperty = NULL;
			itm.iRandomSuffix = NULL;
		}

		loot->items.push_back(itm);
	}
	if( loot->items.size() > sWorld.MaxLootListSize )
	{
		std::vector<__LootItem>::iterator item_to_remove;
		std::vector<__LootItem>::iterator itr;
		uint32 item_quality;
		bool quest_item;
		while( loot->items.size() > sWorld.MaxLootListSize )
		{
			item_to_remove = loot->items.begin();
			item_quality = 0;
			quest_item = false;
			for( itr = loot->items.begin(); itr != loot->items.end(); ++itr )
			{
				item_quality = (*itr).item.itemproto->Quality;
				quest_item = (*itr).item.itemproto->Class == ITEM_CLASS_QUEST;
				if( (*item_to_remove).item.itemproto->Quality > item_quality && !quest_item )
				{
					item_to_remove = itr;
					break;
				}
			}
			loot->items.erase( item_to_remove );
		}
	}
}

void LootMgr::FillCreatureLoot(Loot * loot,uint32 loot_id, uint32 instance_difficulty)
{
	loot->items.clear();
	loot->currencies.clear();
	loot->gold =0;
	
	LootStore::iterator tab =CreatureLoot.find(loot_id);
	if( CreatureLoot.end()==tab)
	{ 
		return;
	}
	else PushLoot(&tab->second,loot, instance_difficulty);
}

void LootMgr::FillGOLoot(Loot * loot,uint32 loot_id, uint32 instance_difficulty)
{
	loot->items.clear ();
	loot->gold =0;

	LootStore::iterator tab =GOLoot.find(loot_id);
	if( GOLoot.end()==tab)
	{ 
		return;
	}
	else PushLoot(&tab->second,loot, instance_difficulty);
}

void LootMgr::FillFishingLoot(Loot * loot,uint32 loot_id)
{
    loot->items.clear();
	loot->currencies.clear();
    loot->gold = 0;

    LootStore::iterator tab = FishingLoot.find(loot_id);
    if( FishingLoot.end() == tab) 
    { 
    	return;
    }
    else PushLoot(&tab->second, loot, 0);
}
/*
void LootMgr::FillSkinningLoot(Loot * loot,uint32 loot_id)
{
 loot->items.clear();
 loot->gold = 0;

 LootStore::iterator tab = SkinningLoot.find(loot_id);
 if( SkinningLoot.end() == tab)
 { 
 	return;
 }
 else PushLoot(&tab->second, loot, 0);
}

void LootMgr::FillPickpocketingLoot( Loot * loot,uint32 loot_id )
{
	 loot->items.clear();
	 loot->gold = 0;

	 LootStore::iterator tab = PickpocketingLoot.find( loot_id );
	 if( PickpocketingLoot.end() == tab )
	 { 
		 return;
	 }
	 else 
		 PushLoot( &tab->second, loot, 0 );
}

void LootMgr::FillDisenchantingLoot( Loot *loot, uint32 loot_id )
{
	loot->items.clear();
	loot->gold = 0;

	LootStore::iterator tab = DisenchantingLoot.find( loot_id );
	if( DisenchantingLoot.end() == tab )
	{ 
		return;
	}
	else 
		PushLoot( &tab->second, loot, 0 );
}

void LootMgr::FillProspectingLoot( Loot *loot, uint32 loot_id )
{
	loot->items.clear();
	loot->gold = 0;

	LootStore::iterator tab = ProspectingLoot.find( loot_id );
	if( ProspectingLoot.end() == tab )
	{ 
		return;
	}
	else
		PushLoot( &tab->second, loot, 0 );
}

void LootMgr::FillMillingLoot( Loot *loot, uint32 loot_id )
{
	loot->items.clear();
	loot->gold = 0;

	LootStore::iterator tab = MillingLoot.find( loot_id );
	if( MillingLoot.end() == tab )
	{ 
		return;
	}
	else
		PushLoot( &tab->second, loot, 0 );
}
*/
bool LootMgr::CanGODrop(uint32 LootId,uint32 itemid)
{
	LootStore::iterator tab =GOLoot.find(LootId);
	if( GOLoot.end()==tab)
	{ 
	return false;
	}
	StoreLootList *list=&(tab->second);
	for(uint32 x=0;x<list->count;x++)
		if(list->items[x].item.itemproto->ItemId==itemid)
		{ 
			return true;
		}
	return false;
}

//THIS should be cached 
bool LootMgr::IsPickpocketable(uint32 creatureId)
{
	LootStore::iterator tab =PickpocketingLoot.find(creatureId);
	if( PickpocketingLoot.end()==tab)
	{ 
		return false;
	}
	return true;
}

//THIS should be cached 
/*
bool LootMgr::IsSkinnable(uint32 creatureId)
{
	LootStore::iterator tab =SkinningLoot.find(creatureId);
	if( SkinningLoot.end()==tab)
	{ 
		return false;
	}
	return true;
} */

//THIS should be cached 
bool LootMgr::IsFishable(uint32 zoneid)
{
	LootStore::iterator tab =FishingLoot.find(zoneid);
	return tab!=FishingLoot.end();
}

LootRoll::LootRoll(uint32 timer, uint32 groupcount, uint64 guid, uint32 slotid, uint32 itemid, uint32 itemunk1, uint32 itemunk2, MapMgr * mgr) : EventableObject()
{
	_mgr = mgr;
	//self delete after N minutes -> !! this might fail horibly sometimes due to backlink to some list !!!!
	sEventMgr.AddEvent(this, &LootRoll::Finalize, EVENT_LOOT_ROLL_FINALIZE, 60*1000, 1, EVENT_FLAG_DELETES_OBJECT);
	_groupcount = groupcount;
	_guid = guid;
	_slotid = slotid;
	_itemid = itemid;
	random_suffix_factor = itemunk1;
	random_suffix_id = itemunk2;
	_remaining = groupcount;
	unk_33_1 = 0;
}

LootRoll::~LootRoll()
{
	sEventMgr.RemoveEvents( this );
}

void LootRoll::Finalize()
{
	sEventMgr.RemoveEvents(this);

	// this we will have to finalize with groups types.. for now
	// we'll just assume need before greed. person with highest roll
	// in need gets the item.

	uint32 highest = 0;
	int8 hightype = -1;
	uint64 player = 0;

	WorldPacket data(34);

/*
	Player * gplr = NULL;
	for(std::map<uint64, uint32>::iterator itr = NeedRolls.begin(); itr != NeedRolls.end(); ++itr)
	{
		gplr = _mgr->GetPlayer((uint32)itr->first);
		if(gplr) break;
	}
	
	if(!gplr)
	{
		for(std::map<uint64, uint32>::iterator itr = GreedRolls.begin(); itr != GreedRolls.end(); ++itr)
		{
			gplr = _mgr->GetPlayer((uint32)itr->first);
			if(gplr) break;
		}
	}
*/
	for(std::map<uint32, uint32>::iterator itr = m_NeedRolls.begin(); itr != m_NeedRolls.end(); ++itr)
	{
		if(itr->second > highest)
		{
			highest = itr->second;
			player = itr->first;
			hightype = LOOT_ROLL_TYPE_NEED;
		}
		/*
		data.Initialize(SMSG_LOOT_ROLL);
		data << _guid << _slotid << itr->first;
		data << _itemid << random_suffix_factor << random_suffix_id;
		data << uint8(itr->second) << uint8(LOOT_ROLL_TYPE_NEED);
		if(gplr && gplr->GetGroup())
			gplr->GetGroup()->SendPacketToAll(&data);
		*/
	}

	if(!highest)
	{
		for(std::map<uint32, uint32>::iterator itr = m_GreedRolls.begin(); itr != m_GreedRolls.end(); ++itr)
		{
			if(itr->second > highest)
			{
				highest = itr->second;
				player = itr->first;
				hightype = LOOT_ROLL_TYPE_GREED;
			}
		/*
		data.Initialize(SMSG_LOOT_ROLL);
		data << _guid << _slotid << itr->first;
		data << _itemid << random_suffix_factor << random_suffix_id;
		data << uint8(itr->second) << uint8(LOOT_ROLL_TYPE_GREED);
		if(gplr && gplr->GetGroup())
			gplr->GetGroup()->SendPacketToAll(&data);
		*/
		}
	}

	Loot * pLoot = 0;
	uint32 guidtype = GET_TYPE_FROM_GUID(_guid);
	if( guidtype == HIGHGUID_TYPE_UNIT )
	{
		Creature * pc = _mgr->GetCreature( _guid );
		if(pc) pLoot = &pc->loot;
	}
	else if( guidtype == HIGHGUID_TYPE_GAMEOBJECT )
	{
		GameObject * go = _mgr->GetGameObject( _guid );
		if(go) pLoot = &go->loot;
	}

	if(!pLoot)
	{
		delete this;
		return;
	}

	if(_slotid >= pLoot->items.size())
	{
		delete this;
		return;
	}

	pLoot->items.at(_slotid).roll = NULL;

	uint32 itemid = pLoot->items.at(_slotid).item.itemproto->ItemId;
	uint32 amt = pLoot->items.at(_slotid).iItemsCount;
	if(!amt)
	{
		delete this;
		return;
	}

	Player * _player = (player) ? _mgr->GetPlayer( player ) : 0;
	if(!player || !_player)
	{
		/* all passed */
		data.Initialize(SMSG_LOOT_ALL_PASSED);
		data << _guid << _groupcount << _itemid << random_suffix_factor << random_suffix_id;
		set<uint32>::iterator pitr = m_passRolls.begin();
		while(_player == NULL && pitr != m_passRolls.end())
			_player = _mgr->GetPlayer( (*(pitr++)) );

		if( _player != NULL )
		{
			if(_player->InGroup())
				_player->GetGroup()->SendPacketToAll(&data);
			else
				_player->GetSession()->SendPacket(&data);
		}

		/* item can now be looted by anyone :) */
		pLoot->items.at(_slotid).passed = true;
		delete this;
		return;
	}

	/*
	0C BA 0E D0 0E 00 30 F1 - guid
	00 00 00 00 slot
	7A 16 00 00 itemid
	00 26 81 8E randomfactor
	00 00 00 00 randomid
	07 34 42 02 00 00 00 07 -guid
	0C roll
	01 roll type
	*/
    pLoot->items.at(_slotid).roll = NULL;
	data.Initialize(SMSG_LOOT_ROLL_WON);
	data << _guid << _slotid << _itemid << random_suffix_factor << random_suffix_id;
	data << _player->GetGUID() << uint8(highest) << uint8(hightype);
	if(_player->InGroup())
		_player->GetGroup()->SendPacketToAll(&data);
	else
		_player->GetSession()->SendPacket(&data);

	ItemPrototype* it = ItemPrototypeStorage.LookupEntry(itemid);

	int8 error;
	if((error = _player->GetItemInterface()->CanReceiveItem(it, 1)) != 0)
	{
		_player->GetItemInterface()->BuildInventoryChangeError(NULL, NULL, error);
		return;
	}

	Item * add = _player->GetItemInterface()->FindItemLessMax(itemid, amt, false);

	if (!add)
	{
		SlotResult slotresult = _player->GetItemInterface()->FindFreeInventorySlot(it);
		if(!slotresult.Result)
		{
			_player->GetItemInterface()->BuildInventoryChangeError(NULL, NULL, INV_ERR_INVENTORY_FULL);
			return;
		}

		sLog.outDebug("AutoLootItem MISC");
		Item *item = objmgr.CreateItem( itemid, _player);

		item->SetUInt32Value(ITEM_FIELD_STACK_COUNT,amt);
		if(pLoot->items.at(_slotid).iRandomProperty!=NULL)
		{
			item->SetRandomProperty(pLoot->items.at(_slotid).iRandomProperty->ID);
			item->ApplyRandomProperties(false);
		}
		else if(pLoot->items.at(_slotid).iRandomSuffix != NULL)
		{
			item->SetRandomSuffix(pLoot->items.at(_slotid).iRandomSuffix->id);
			item->ApplyRandomProperties(false);
		}
		RandomizeEPLItem( item );


		AddItemResult res = _player->GetItemInterface()->SafeAddItem(item,slotresult.ContainerSlot, slotresult.Slot);
		if( res == ADD_ITEM_RESULT_OK )
		{
			_player->GetSession()->SendItemPushResult(item,false,true,true,true,slotresult.ContainerSlot,slotresult.Slot,1);
			sQuestMgr.OnPlayerItemPickup(_player,item);
		}
		else if( res == ADD_ITEM_RESULT_ERROR )
		{
			item->DeleteMe();
			item = NULL;
		}
	}
	else 
	{	
		add->SetCount(add->GetUInt32Value(ITEM_FIELD_STACK_COUNT) + amt);
		add->m_isDirty = true;
		sQuestMgr.OnPlayerItemPickup(_player,add);
		_player->GetSession()->SendItemPushResult(add, false, true, true, false, _player->GetItemInterface()->GetBagSlotByGuid(add->GetGUID()), 0xFFFFFFFF, 1);
	}

	pLoot->items.at(_slotid).iItemsCount=0;
	// this gets sent to all looters
	data.Initialize(SMSG_LOOT_REMOVED);
	data << uint8(_slotid);
	Player * plr;
	for(LooterSet::iterator itr = pLoot->looters.begin(); itr != pLoot->looters.end(); ++itr)
	{
		if((plr = _player->GetMapMgr()->GetPlayer(*itr)) != 0)
			plr->GetSession()->SendPacket(&data);
	}

	/*WorldPacket idata(45);
	_player->GetSession()->BuildItemPushResult(&idata, _player->GetGUID(), ITEM_PUSH_TYPE_LOOT, amt, itemid, pLoot->items.at(_slotid).iRandomProperty ? pLoot->items.at(_slotid).iRandomProperty->ID : 0);

	if(_player->InGroup())
		_player->GetGroup()->SendPacketToAll(&idata);
	else
		_player->GetSession()->SendPacket(&idata);*/

	delete this;
}

void LootRoll::PlayerRolled(Player *player, uint8 choice)
{
	if(m_NeedRolls.find(player->GetLowGUID()) != m_NeedRolls.end() || m_GreedRolls.find(player->GetLowGUID()) != m_GreedRolls.end())
		return; // dont allow cheaters

	/*
	0C BA 0E D0 0E 00 30 F1 Guid
	00 00 00 00 slot
	CB 26 4B 02 00 00 00 07 guid
	7A 16 00 00 item
	00 26 81 8E random factor
	00 00 00 00 F9 random id
	02 roll
	00 roll type

	4C 2A 05 0F 09 00 30 F1 
	01 00 00 00
	F0 BB 3E 02	00 00 00 07 
	C8 1C 00 00 
	60 7B 01 3D 
	08 04 00 00 F9
	02 
	00

	9B 29 05 C9 37 00 30 F1 
	02 00 00 00
	F0 BB 3E 02	00 00 00 07
	B2 19 00 00 
	80 B4 AD 9D 
	A8 03 00 00 F9
	02 
	00
	*/
	int roll = RandomUInt(99)+1;
	// create packet
	WorldPacket data(34);
	data.SetOpcode(SMSG_LOOT_ROLL);
	data << _guid << _slotid << player->GetGUID();
	data << _itemid << random_suffix_factor << random_suffix_id;

	//anti ninja - don't need on things you cannot wear
	if(choice == LOOT_ROLL_TYPE_NEED) 
	{
		ItemPrototype *ip = ItemPrototypeStorage.LookupEntry( _itemid );
		if( player->GetItemInterface()->CanEquipItem( ip, player ) != 0 )
		{
			choice = LOOT_ROLL_TYPE_GREED;
			player->BroadcastMessage("You cannot need on this item.Changing to greed roll");
		}
	}

	if(choice == LOOT_ROLL_TYPE_NEED) 
	{
		player->Event_AchiementCriteria_Received( ACHIEVEMENT_CRITERIA_TYPE_ROLL_NEED_ON_LOOT,roll,ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_SET);
		player->Event_AchiementCriteria_Received( ACHIEVEMENT_CRITERIA_TYPE_ROLL_NEED,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
		m_NeedRolls.insert( std::make_pair(player->GetLowGUID(), roll) );
		data << uint8(roll) << uint8(LOOT_ROLL_TYPE_NEED);
	} 
	else if(choice == LOOT_ROLL_TYPE_GREED || choice == LOOT_ROLL_TYPE_DISENCHANT)
	{
		if( choice == LOOT_ROLL_TYPE_GREED )
		{
			player->Event_AchiementCriteria_Received( ACHIEVEMENT_CRITERIA_TYPE_ROLL_GREED_ON_LOOT,roll,ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_SET);
			player->Event_AchiementCriteria_Received( ACHIEVEMENT_CRITERIA_TYPE_ROLL_GREED,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
		}
		else
		{
			player->Event_AchiementCriteria_Received( ACHIEVEMENT_CRITERIA_TYPE_ROLL_DISENCHANT,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
		}
		m_GreedRolls.insert( std::make_pair(player->GetLowGUID(), roll) );
		data << uint8(roll) << uint8(choice);

	}
	else
	{
		m_passRolls.insert( player->GetLowGUID() );
		data << uint8(128) << uint8(128);
	}
	data << uint8(unk_33_1);
	
	if(player->InGroup())
		player->GetGroup()->SendPacketToAll(&data);
	else
		player->GetSession()->SendPacket(&data);
	
	// check for early completion
	if(!--_remaining)
	{
		// kill event early
		//sEventMgr.RemoveEvents(this);
		Finalize();
	}
}

void LootMgr::FillItemLoot(Loot *loot, uint32 loot_id)
{
	loot->items.clear ();
	loot->gold =0;

	LootStore::iterator tab =ItemLoot.find(loot_id);
	if( ItemLoot.end()==tab)
	{ 
		return;
	}
	else PushLoot(&tab->second,loot,false);
}

//no test, just a push
void LootMgr::FillObjectLoot(Object *obj,LootStoreStruct *ls )
{
	__LootItem itm;
	itm.item = ls->item;

	itm.iItemsCount = ls->MinCountOrRef;
	int32 randomcount = ls->MaxCount - ls->MinCountOrRef;
	if( randomcount > 1 )
		itm.iItemsCount += RandomUInt() % randomcount;

	itm.roll = NULL;
	itm.passed = false;
	//everyone can loot it. Set it to 0 for everyone to receive 1 copy
//	if( ls->item.itemproto->Class == ITEM_CLASS_QUEST || (ls->item.itemproto->Flags & ITEM_FLAG_QUEST) )
	if( ls->item.itemproto->Flags & ITEM_FLAG_GROUP )
		itm.ffa_loot = 1;	
	else
		itm.ffa_loot = 0;	
	itm.has_looted.clear();
	
	if( ls->item.itemproto->Quality > ITEM_QUALITY_NORMAL_WHITE && ls->item.itemproto->ContainerSlots == 0 )
	{
		itm.iRandomProperty=GetRandomProperties( ls->item.itemproto );
		itm.iRandomSuffix=GetRandomSuffix( ls->item.itemproto );
	}
	else
	{
		// save some calls :P
		itm.iRandomProperty = NULL;
		itm.iRandomSuffix = NULL;
	}

	if( obj->IsUnit() )
		SafeUnitCast(obj)->loot.items.push_back(itm);
//	else if( obj->IsItem() )
//		SafeItemCast(obj)->loot.items.push_back(itm);
//	else if( obj->IsGameObject() )
//		SafeGOCast(obj)->loot.items.push_back(itm);
}

//check if we could pick this loot 
bool LootMgr::FillObjectLoot(Object *obj,LootStoreStruct *ls, LootStoreStruct *reference )
{
	float drop_chance = 0;

	//quick exit test
	if( reference != NULL )
	{
		if( reference->GroupID && reference->GroupID != ls->GroupID )
			return false;
//		if( reference->DropChance ) 
//			drop_chance = reference->DropChance;
	}

	//normal loot condition
	if( ls->DropChance > 0 && ls->MinCountOrRef > 0 )
	{
//		if( drop_chance == 0 )
			drop_chance = ls->DropChance;
	}

	//equal-chanced entries are processed separatly
	else if( ls->DropChance == 0 )
	{
		return false;
	}

	//need to check if we have this quest active before we can use it as loot
	else if( ls->DropChance < 0 && ls->MinCountOrRef > 0 && obj->IsPlayer() )
	{
//		Player *pobj = SafePlayerCast(obj);
//		return false;
//		if( drop_chance == 0 )
			drop_chance = -ls->DropChance;
	}

	//redirect loot generation from this list to some other list
	else if( ls->ref_list != NULL )
	{
		if( RandChance( drop_chance * sWorld.getRate( RATE_DROP0 )) == true )
			return FillObjectLoot( obj, ls->ref_list, reference );
		return false;
	}

	//forced dropchance means that one of the items must drop ?  
	if( reference == NULL 
//		|| reference->DropChance != 0 
		)
		drop_chance = drop_chance * sWorld.getRate( RATE_DROP0 + ls->item.itemproto->Quality );

	if( RandChance( drop_chance ) == false )
	{
		return false;
	}

	uint32 diff_mask;
	if( obj && obj->GetMapMgr() )
		diff_mask = 1 << ( obj->GetMapMgr()->instance_difficulty + 2 );
	else
		diff_mask = 65535;
	if( ( ls->difficulty_mask & diff_mask ) == 0 )
	{
		return false;	//this item does not drop in this instance difficulty mode
	}

	FillObjectLoot( obj, ls );

	return true;
}

//parse a group list and pick 1 item out of it
bool LootMgr::FillObjectLoot(Object *obj,SimpleVect<LootStoreStruct*> *source_list, LootStoreStruct *reference )
{
	uint32 max_ind = source_list->GetMaxSize();
	if( max_ind == 0 )
		return false;	//noway
	uint32 drops_added = 0;
	uint32 must_drop_counter = 0;
	float chance_sum = 0;
	//this should make list parsing "randomized". On highdroprate mods mobs would drop always the first 16 items if this would not exist
	uint32 random_start_index = RandomUInt() % max_ind;
//	uint32 random_increment_index = RandomUInt() % max_ind;
	for(uint32 li=random_start_index;li<max_ind+random_start_index;li++)
	{
		uint32 ti = li % max_ind;
//		uint32 ti = (li * random_increment_index ) % max_ind;	//need to avoid parsing the same item twice
		LootStoreStruct *ls = source_list->GetValue( ti );
		//reference list is parsed only if we manage to roll for it
		if( ls->ref_list )
		{
			//theoretically loot with reference list will have only 1 item, but this whole process is called N times
			if( ls->DropChance<=0 || RandChance( ls->DropChance * sWorld.getRate( RATE_DROP0 )) == true ) 
			{	
				if( FillObjectLoot( obj, ls->ref_list, ls ) )
				{
					drops_added++;
//					return true;
				}
			}
		}
		//explicit drops are parsed first
		else if( ls->DropChance >0 )
		{
			if( FillObjectLoot( obj, ls, reference ) == true )
			{
				if( ls->GroupID != 0 )
					return true;
				drops_added++;
			}
			//if explicit drops chance is >= 100 then we drop something from here
			chance_sum += ls->DropChance;
		}
		//we count equal chance drops and pick 1 at the end
		else if( ls->DropChance == 0 )
			must_drop_counter++;
	}
	//pick an equal-chanced drop (if there is any)
	if( must_drop_counter > 0 )
	{
		uint32 picked_value = RandomUInt() % must_drop_counter;
		uint32 counter = 0;
		for(uint32 li=0;li<max_ind;li++)
			if( source_list->GetValue( li )->DropChance == 0 && source_list->GetValue( li )->ref_list == NULL )
			{
				if( counter == picked_value )
				{
					FillObjectLoot( obj, source_list->GetValue( li ) );
					return true;
				}
				counter++;
			}
	}
	//if we still have no items rule says that if sum of chances is at least 100 then 1 item will be picked for sure
	if( drops_added == 0 && chance_sum >= 100 )
	{
		uint32 explicit_chanced_items = max_ind - must_drop_counter;
		uint32 picked_value = RandomUInt() % explicit_chanced_items;
		uint32 counter = 0;
		for(uint32 li=0;li<max_ind;li++)
			if( source_list->GetValue( li )->DropChance >= 0 && source_list->GetValue( li )->ref_list == NULL )
			{
				if( counter == picked_value )
				{
					FillObjectLoot( obj, source_list->GetValue( li ) );
					return true;
				}
				counter++;
			}
	}
	return false;
}

// parse a list of groups and pick multiple items
// this function can be called recursively for linked lists !
bool LootMgr::FillObjectLoot(Object *obj,SimpleVect< SimpleVect<LootStoreStruct*>* > *source_list, LootStoreStruct *reference )
{
	uint32 max_ind = source_list->GetMaxSize();
	if( max_ind == 0 )
		return false;	//empty list
	bool ret = false; 
	uint32 loops_max;
	if( reference )
		loops_max = reference->MaxCount;
	else
		loops_max = 1;
	for(uint32 loop=0;loop<loops_max;loop++)
	{
		//this should make list parsing "randomized". On highdroprate mods mobs would drop always the first 16 items if this would not exist
		uint32 random_start_index = RandomUInt() % max_ind;
		for(uint32 li=random_start_index;li<max_ind+random_start_index;li++)
		{
			//check for current loot list size and abort if too long
			if( obj->IsUnit() && SafeUnitCast(obj)->loot.items.size() >= sWorld.MaxLootListSize )
				return false;
			uint32 ti = li % max_ind;
			ret = ret | FillObjectLoot( obj, source_list->GetValue( ti ), reference );
		}
	}
	return ret;
}

int32 LootRoll::event_GetInstanceID()
{
	return _mgr->GetInstanceID();
}
