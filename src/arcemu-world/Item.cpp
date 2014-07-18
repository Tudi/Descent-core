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

Item::Item()//this is called when constructing as container
{
	m_bufferPoolId = OBJECT_WAS_ALLOCATED_STANDARD_WAY;
	m_itemProto = NULL;
	m_owner = NULL;
	loot = NULL;
	locked = false;
	wrapped_item_id = 0;
	m_objectTypeId = TYPEID_ITEM;
	internal_object_type = INTERNAL_OBJECT_TYPE_ITEM;
	m_valuesCount = ITEM_END;
	m_uint32Values = _fields;
	m_updateMask.SetCount(ITEM_END);
	random_prop = 0;
	random_suffix = 0;
	stats_applied = false;

	Enchantments.clear();
}

//called instead of parametrized constructor
void Item::Init( uint32 high, uint32 low )
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	SetUInt32Value( OBJECT_FIELD_GUID, low );
	SetUInt32Value( OBJECT_FIELD_GUID + 1, high );
	m_wowGuid.Init( GetGUID() );
}

void Item::Virtual_Constructor()
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	memset( m_uint32Values, 0, (ITEM_END) * sizeof( uint32 ) );
	SetUInt32Value( OBJECT_FIELD_TYPE,TYPE_ITEM | TYPE_OBJECT );
	SetFloatValue( OBJECT_FIELD_SCALE_X, 1 );//always 1
	m_itemProto = NULL;
	m_owner = NULL;
	loot = NULL;
	locked = false;
	m_isDirty = true;
	random_prop = 0;
	random_suffix = 0;
	wrapped_item_id = 0;

	m_objectUpdated = false;

#ifndef SMALL_ITEM_OBJECT
	m_loadedFromDB = false;
	mSemaphoreTeleport = false;
	m_mapId = 0;
//	m_zoneId = 0;
	m_faction = NULL;
	m_factionDBC = NULL;
	m_instanceId = WORLD_INSTANCE;
	Active = false;
	m_inQueue = false;
	ExtensionSet::iterator itr;
	for( itr=m_extensions.begin(); itr!=m_extensions.end(); itr++)
		delete itr->second;
	m_extensions.clear();
#endif
	EventableObject::Virtual_Constructor();	//clear out event holder. There is a bug that if player logoud out in an instance and item was attached to it then item get invalid pointer to a holder
}

Item::~Item()
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	Virtual_Destructor();
	if( m_bufferPoolId != OBJECT_WAS_ALLOCATED_STANDARD_WAY )
		ASSERT( false ); //we are deleting a pooled object. This leads to mem corruptions
	m_bufferPoolId = OBJECT_WAS_DEALLOCATED;

	//avoid Unit destructor referencing to this object ass item after creature destructor call
	//if object destructor will acces player fields after destructor it could create memcorruptions
	m_objectTypeId = TYPEID_UNUSED;
	internal_object_type = INTERNAL_OBJECT_TYPE_NONE;
}

void Item::Virtual_Destructor()
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	if( loot != NULL )
	{
		delete loot;
		loot = NULL;
	}

	sEventMgr.RemoveEvents( this );

	EnchantmentMap::iterator itr;
	for( itr = Enchantments.begin(); itr != Enchantments.end(); ++itr )
	{
		delete itr->second;
		itr->second = NULL;
	}
	Enchantments.clear();

	//don't want to keep context ....
	//it makes a big difference where we mark Events deleted !
	EventableObject::Virtual_Destructor();

	if( IsInWorld() )
		RemoveFromWorld();

	m_owner = NULL;
	enchant_spell_book.clear();
}

void Item::Create( uint32 itemid, Player* owner )
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	SetUInt32Value( OBJECT_FIELD_ENTRY, itemid );
 
	if( owner != NULL )
	{
		SetUInt64Value( ITEM_FIELD_OWNER, owner->GetGUID() );
		SetUInt64Value( ITEM_FIELD_CONTAINED, owner->GetGUID() );
	}

	SetUInt32Value( ITEM_FIELD_STACK_COUNT, 1 );

	m_itemProto = ItemPrototypeStorage.LookupEntry( itemid );

	ASSERT( m_itemProto );
	 
	SetUInt32Value( ITEM_FIELD_SPELL_CHARGES, m_itemProto->Spells.Charges[0] );
	SetUInt32Value( ITEM_FIELD_SPELL_CHARGES_1, m_itemProto->Spells.Charges[1] );
	SetUInt32Value( ITEM_FIELD_SPELL_CHARGES_2, m_itemProto->Spells.Charges[2] );
	SetUInt32Value( ITEM_FIELD_SPELL_CHARGES_3, m_itemProto->Spells.Charges[3] );
	SetUInt32Value( ITEM_FIELD_SPELL_CHARGES_4, m_itemProto->Spells.Charges[4] );
	SetUInt32Value( ITEM_FIELD_MAXDURABILITY, m_itemProto->MaxDurability );
	SetUInt32Value( ITEM_FIELD_DURABILITY, m_itemProto->MaxDurability );

	m_owner = owner;
	if( m_itemProto->LockId > 1 )
		locked = true;
	else
		locked = false;
}

void Item::LoadFromDB(Field* fields, Player* plr, bool light )
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	uint32 itemid = fields[2].GetUInt32();
	uint32 random_prop, random_suffix;
	m_itemProto = ItemPrototypeStorage.LookupEntry( itemid );

	ASSERT( m_itemProto );
	
	if(m_itemProto->LockId > 1)
		locked = true;
	else
		locked = false;
	
	SetUInt32Value( OBJECT_FIELD_ENTRY, itemid );
	m_owner = plr;

	wrapped_item_id=fields[3].GetUInt32();
	m_uint32Values[ITEM_FIELD_GIFTCREATOR] = fields[4].GetUInt32();
	m_uint32Values[ITEM_FIELD_CREATOR] = fields[5].GetUInt32();

	int32 count = fields[6].GetUInt32();
	if ( count > (int32)m_itemProto->MaxCount )
		count = m_itemProto->MaxCount;
	SetUInt32Value( ITEM_FIELD_STACK_COUNT,  count );

	// Again another for that did not indent to make it do anything for more than 
	// one iteration x == 0 was the only one executed
	for( uint32 x = 0; x < 5; x++ )
	{
		if( m_itemProto->Spells.Id[x] )
		{
			SetUInt32Value( ITEM_FIELD_SPELL_CHARGES + x , fields[7].GetUInt32() );
			break;
		}
	}

	SetUInt32Value( ITEM_FIELD_FLAGS, fields[8].GetUInt32() );
	random_prop = fields[9].GetUInt32();
	random_suffix = fields[10].GetUInt32();

	SetRandomProperty( random_prop );
	SetRandomSuffix( random_suffix );

	int32 rprop = int32( random_prop );
	// If random properties point is negative that means the item uses random suffix as random enchantment
//	if( rprop < 0 )
//		SetRandomSuffix( random_suffix );
//	else
//		SetRandomSuffix( 0 );

//	SetUInt32Value( ITEM_FIELD_ITEM_TEXT_ID, fields[11].GetUInt32() );

	SetUInt32Value( ITEM_FIELD_MAXDURABILITY, m_itemProto->MaxDurability );
	SetUInt32Value( ITEM_FIELD_DURABILITY, fields[11].GetUInt32() );

	if( light )
	{ 
		return;
	}

	string enchant_field = fields[14].GetString();
	vector< string > enchants = StrSplit( enchant_field, ";" );
	uint32 enchant_id;
	EnchantEntry* entry;
	uint32 time_left;
	uint32 enchslot;

	for( vector<string>::iterator itr = enchants.begin(); itr != enchants.end(); ++itr )
	{
		if( sscanf( (*itr).c_str(), "%u,%u,%u", (unsigned int*)&enchant_id, (unsigned int*)&time_left, (unsigned int*)&enchslot) == 3 )
		{
			entry = dbcEnchant.LookupEntry( enchant_id );
			if( entry && entry->Id == enchant_id 
//				&& m_itemProto->SubClass != ITEM_SUBCLASS_WEAPON_THROWN //I wonder why this was added here
				)
			{
				AddEnchantment( entry, time_left, ( time_left == 0 ), false, false, enchslot );
				//(enchslot != 2) ? false : true, false);
			}
			else
			{
				/*
				EnchantEntry *pEnchant = new EnchantEntry;
				memset(pEnchant,0,sizeof(EnchantEntry));

				pEnchant->Id = enchant_id;
				if(enchslot != 2)
					AddEnchantment(pEnchant,0,true, false);
				else
					AddEnchantment(pEnchant,0,false,false);
				*/
			}
		}
	}	

	//this will add the enchantments from random properties
	//we will apply all enchantments when player will enter world
	ApplyRandomProperties( false );

	SetReforgeID( fields[15].GetUInt32() );

	// Charter stuff
	if(m_uint32Values[OBJECT_FIELD_ENTRY] == ITEM_ENTRY_GUILD_CHARTER)
	{
		SetUInt32Value( ITEM_FIELD_FLAGS, ITEM_FLAG_SOULBOUND );
		SetUInt32Value( ITEM_FIELD_STACK_COUNT, 1 );
		SetUInt32Value( ITEM_FIELD_PROPERTY_SEED, 57813883 );
		if( plr && plr->m_charters[CHARTER_TYPE_GUILD] )
			SetUInt32Value( ITEM_FIELD_ENCHANTMENT_1_1 , plr->m_charters[CHARTER_TYPE_GUILD]->GetID() );
	}

/*	if( m_uint32Values[OBJECT_FIELD_ENTRY] == ARENA_TEAM_CHARTER_2v2 )
	{
		SetUInt32Value( ITEM_FIELD_FLAGS, ITEM_FLAG_SOULBOUND );
		SetUInt32Value( ITEM_FIELD_STACK_COUNT, 1 );
		SetUInt32Value( ITEM_FIELD_PROPERTY_SEED, 57813883 );
		if( plr && plr->m_charters[CHARTER_TYPE_ARENA_2V2] )
			SetUInt32Value( ITEM_FIELD_ENCHANTMENT_1_1 , plr->m_charters[CHARTER_TYPE_ARENA_2V2]->GetID() );
	}

	if( m_uint32Values[OBJECT_FIELD_ENTRY] == ARENA_TEAM_CHARTER_3v3 )
	{
		SetUInt32Value( ITEM_FIELD_FLAGS, ITEM_FLAG_SOULBOUND );
		SetUInt32Value( ITEM_FIELD_STACK_COUNT, 1 );
		SetUInt32Value( ITEM_FIELD_PROPERTY_SEED, 57813883 );
		if( plr && plr->m_charters[CHARTER_TYPE_ARENA_3V3] )
			SetUInt32Value( ITEM_FIELD_ENCHANTMENT_1_1 , plr->m_charters[CHARTER_TYPE_ARENA_3V3]->GetID() );
	}

	if( m_uint32Values[OBJECT_FIELD_ENTRY] == ARENA_TEAM_CHARTER_5v5 )
	{
		SetUInt32Value( ITEM_FIELD_FLAGS, ITEM_FLAG_SOULBOUND );
		SetUInt32Value( ITEM_FIELD_STACK_COUNT, 1 );
		SetUInt32Value( ITEM_FIELD_PROPERTY_SEED, 57813883 );
		if( plr && plr->m_charters[CHARTER_TYPE_ARENA_5V5] )
			SetUInt32Value( ITEM_FIELD_ENCHANTMENT_1_1 , plr->m_charters[CHARTER_TYPE_ARENA_5V5]->GetID() );
	} */
}

void Item::ApplyRandomProperties( bool apply )
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	// apply random properties
//	if( m_uint32Values[ITEM_FIELD_RANDOM_PROPERTIES_ID] != 0 )
	{
//		if( m_int32Values[ITEM_FIELD_RANDOM_PROPERTIES_ID] > 0 )		// Random Property
		if( random_prop )
		{
//			RandomProps* rp= dbcRandomProps.LookupEntry( m_uint32Values[ITEM_FIELD_RANDOM_PROPERTIES_ID] );
			RandomProps* rp= dbcRandomProps.LookupEntry( random_prop );
			int32 Slot;
			for( int k = 0; k < RANDOM_PROPERTY_ENCHANT_COUNT; k++ )
			{
				if( rp->enchantments[k] != 0 )
				{	
					EnchantEntry* ee = dbcEnchant.LookupEntry( rp->enchantments[k] );
					Slot = HasEnchantment( ee->Id );
					if( Slot < 0 ) 
					{
						Slot = FindFreeEnchantSlot( ee, RANDOMPROPERTY );
						AddEnchantment( ee, 0, false, apply, true, Slot );
					}
					else if( apply )
						ApplyEnchantmentBonus( Slot, apply );
				}
			}
		}
//		else
		if( random_suffix )
		{
//			int32 random_suffix_id = -m_int32Values[ITEM_FIELD_RANDOM_PROPERTIES_ID];
//			ItemRandomSuffixEntry* rs = dbcItemRandomSuffix.LookupEntry( random_suffix_id );
			ItemRandomSuffixEntry* rs = dbcItemRandomSuffix.LookupEntry( random_suffix );
			int32 Slot;
			for( uint32 k = 0; k < SUFFIX_ENCHANT_COUNT; ++k )
			{
				if( rs->enchantments[k] != 0 )
				{
					EnchantEntry* ee = dbcEnchant.LookupEntry( rs->enchantments[k] );
					Slot = HasEnchantment( ee->Id );
					if( Slot < 0 ) 
					{
						Slot = FindFreeEnchantSlot( ee, RANDOMSUFFIX );
						AddEnchantment( ee, 0, false, apply, true, Slot, rs->prefixes[k] );
					}
					else if( apply )
						ApplyEnchantmentBonus( Slot, apply );
				}
			}
		}
	}
}

void Item::SaveToDB( int16 containerslot, int16 slot, bool firstsave, QueryBuffer* buf )
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	if( !m_isDirty && !firstsave )
	{ 
		return;
	}
	//conjured items dissapear on logout for more than 15 minutes
//	if( ( GetProto()->Flags & ITEM_FLAG_CONJURED ) 
		//!! Should mark them somehow else !
//		|| HasFlag( ITEM_FIELD_FLAGS, ITEM_FLAG_CONJURED ) //maybe this item is not default conjured but it is conjured by a script
//		)
	{
//		return;
	}

	std::stringstream ss;

	ss << "REPLACE INTO playeritems VALUES(";

	ss << m_uint32Values[ITEM_FIELD_OWNER] << ",";
    ss << m_uint32Values[OBJECT_FIELD_GUID] << ",";
	ss << m_uint32Values[OBJECT_FIELD_ENTRY] << ",";
	ss << wrapped_item_id << ",";
	ss << m_uint32Values[ITEM_FIELD_GIFTCREATOR] << ",";
	ss << m_uint32Values[ITEM_FIELD_CREATOR] << ",";

	ss << GetUInt32Value(ITEM_FIELD_STACK_COUNT) << ",";
	ss << GetChargesLeft() << ",";
	ss << GetUInt32Value(ITEM_FIELD_FLAGS) << ",";
	ss << random_prop << ", " << random_suffix << ", ";
//	ss << GetUInt32Value(ITEM_FIELD_ITEM_TEXT_ID) << ",";
	ss << GetUInt32Value(ITEM_FIELD_DURABILITY) << ",";
	ss << (int32)(containerslot) << ",";
	ss << (int32)(slot) << ",'";

	// Pack together enchantment fields
	if( Enchantments.empty() == false )
	{
		EnchantmentMap::iterator itr = Enchantments.begin();
		for(; itr != Enchantments.end(); ++itr)
		{
			if( itr->second->RemoveAtLogout )
				continue;

			uint32 elapsed_duration = uint32( UNIXTIME - itr->second->ApplyTime );
			int32 remaining_duration = itr->second->Duration - elapsed_duration;
			if( remaining_duration < 0 )
				remaining_duration = 0;
		
			/*
			if( !itr->second.RemoveAtLogout && (remaining_duration > 5 && itr->second.Slot != 2) || itr->second.Slot == 2)  // no point saving stuff with < 5 seconds... unless is perm enchant
			{
				ss << itr->second.Enchantment->Id << ",";
				ss << remaining_duration << ",";
				ss << itr->second.Slot << ";";
			}
			*/
		  
			if( itr->second->Enchantment && ( remaining_duration && remaining_duration > 5 || itr->second->Duration == 0 ) )
			{
				ss << itr->second->Enchantment->Id << ",";
				ss << remaining_duration << ",";
				ss << itr->second->Slot << ";";
			}
		}
	}
	ss << "',";
	ss << GetReforgeID() << ")";
	
	if( firstsave )
		CharacterDatabase.WaitExecute( ss.str().c_str() );
	else
	{
		if( buf == NULL )
			CharacterDatabase.Execute( ss.str().c_str() );
		else
			buf->AddQueryStr( ss.str() );
	}

	m_isDirty = false;
}

void Item::DeleteFromDB()
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	if( m_itemProto->ContainerSlots>0 && IsContainer() )
	{
		/* deleting a container */
		for( uint32 i = 0; i < m_itemProto->ContainerSlots; ++i )
		{
			if( SafeContainerCast( this )->GetItem( i ) != NULL )
			{
				/* abort the delete */
				return;
			}
		}
	}

	CharacterDatabase.Execute( "DELETE FROM playeritems WHERE guid = %u", m_uint32Values[OBJECT_FIELD_GUID] );
}

void Item::DeleteMe()
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	if( !IsItem() )
	{
		sLog.outDebug(" Item::DeleteMe : !!! omg invalid call on wrong object !\n");
		ASSERT(false);
		return;
	}

	//clear all pending updates and remove from update list
	if( IsInWorld() )
		RemoveFromWorld();

	//Don't inline me!
	if( IsContainer() )
	{
		INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
		delete SafeContainerCast(this);
	} 
	else 
	{
		ItemPool.PooledDelete( this, __FILE__, __LINE__ );
	}
}

uint32 GetSkillByProto( uint32 Class, uint32 SubClass )
{
	if( Class == ITEM_CLASS_ARMOR && SubClass < ITEM_SUBCLASS_WEAPON_SWORD )
	{
		return arm_skills[SubClass];
	} 
    else if( Class == ITEM_CLASS_WEAPON )
	{
		if( SubClass < 20 )//no skill for fishing
		{
			return weap_skills[SubClass];
		}
	}
	return 0;
}

//This map is used for profess.
//Prof packe strcut: {SMSG_SET_PROFICIENCY,(uint8)item_class,(uint32)1<<item_subclass}
//ie: for fishing (it's class=2--weapon, subclass ==20 -- fishing rod) permissive packet
// will have structure 0x2,524288
//this table is needed to get class/subclass by skill, valid classes are 2 and 4

const ItemProf* GetProficiencyBySkill( uint32 skill )
{
	switch( skill )
	{
		case SKILL_CLOTH:
			return &prof[0];
		case SKILL_LEATHER:
			return &prof[1];
		case SKILL_MAIL:
			return &prof[2];
		case SKILL_PLATE_MAIL:
			return &prof[3];
		case SKILL_SHIELD:
			return &prof[4];
		case SKILL_AXES:
			return &prof[5];
		case SKILL_2H_AXES:
			return &prof[6];
		case SKILL_BOWS:
			return &prof[7];
		case SKILL_GUNS:
			return &prof[8];
		case SKILL_MACES:
			return &prof[9];
		case SKILL_2H_MACES:
			return &prof[10];
		case SKILL_POLEARMS:
			return &prof[11];
		case SKILL_SWORDS:
			return &prof[12];
		case SKILL_2H_SWORDS:
			return &prof[13];
		case SKILL_STAVES:
			return &prof[14];
		case SKILL_FIST_WEAPONS:
			return &prof[15];
		case SKILL_DAGGERS:
			return &prof[16];
		case SKILL_THROWN:
			return &prof[17];
//		case SKILL_SPEARS:
//			return &prof[18];
		case SKILL_CROSSBOWS:
			return &prof[19];
		case SKILL_WANDS:
			return &prof[20];
		case SKILL_FISHING:
			return &prof[21];
		default:
			return NULL;
	}
}

uint32 GetSellPriceForItem( ItemPrototype *proto, uint32 count )
{
	int32 cost;
	cost = proto->SellPrice * ( ( count < 1 ) ? 1 : count );
	return cost;
}

uint32 GetBuyPriceForItem( ItemPrototype* proto, uint32 count, Player* plr, Creature* vendor )
{
	int32 cost = proto->BuyPrice;

	if( plr != NULL && vendor != NULL )
	{
		Standing plrstanding = plr->GetStandingRank( vendor->m_faction->Faction );
		cost = float2int32( ceilf( float( proto->BuyPrice ) * pricemod[plrstanding] ) );
	}

	return cost * count;
}

uint32 GetSellPriceForItem( uint32 itemid, uint32 count )
{
	if( ItemPrototype* proto = ItemPrototypeStorage.LookupEntry( itemid ) )
	{ 
		return GetSellPriceForItem(proto, count);
	}
	else
		return 1;
}

uint32 GetBuyPriceForItem( uint32 itemid, uint32 count, Player* plr, Creature* vendor )
{
	if( ItemPrototype* proto = ItemPrototypeStorage.LookupEntry( itemid ) )
	{ 
		return GetBuyPriceForItem( proto, count, plr, vendor );
	}
	else
		return 1;
}

void Item::RemoveFromWorld()
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	// if we have an owner->send destroy
	if( m_owner != NULL )
	{
//		DestroyForPlayer( m_owner );
		m_owner->PushOutOfRange( GetGUID() );
	}

	if( !IsInWorld() )
	{ 
		return;
	}

#ifndef SMALL_ITEM_OBJECT
	mSemaphoreTeleport = true;
#endif
//	m_mapMgr->RemoveObject( this, false );
	m_mapMgr = NULL;
  
	// update our event holder
	event_Relocate();
}

void Item::SetOwner( Player* owner )
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
 
	if( owner != NULL )
		SetUInt64Value( ITEM_FIELD_OWNER, owner->GetGUID() );
	else SetUInt64Value( ITEM_FIELD_OWNER, 0 );

	m_owner = owner; 
}


int32 Item::AddEnchantment( EnchantEntry* Enchantment, uint32 Duration, bool Perm /* = false */, bool apply /* = true */, bool RemoveAtLogout /* = false */, uint32 Slot_, uint32 RandomSuffixPrefix )
{
	int32 Slot = Slot_;
	m_isDirty = true;

/*
	if(Perm)
	{
		if(Slot_)
		{
			Slot=Slot_;
		}
		else
        {
			Slot = FindFreeEnchantSlot(Enchantment);
        }
	}
	else
	{
		if(Enchantment->EnchantGroups > 1) // replaceable temp enchants
		{
			Slot = 1;
			RemoveEnchantment(1);
		}
		else
		{
			Slot = FindFreeEnchantSlot(Enchantment);*/
			/*
			Slot = Enchantment->type ? 3 : 0;
			 //that's 's code
			for(uint32 Index = ITEM_FIELD_ENCHANTMENT_09; Index < ITEM_FIELD_ENCHANTMENT_32; Index += 3)
			{
				if(m_uint32Values[Index] == 0) break;;	
				++Slot;
			}

			//Slot = FindFreeEnchantSlot(Enchantment);
			// reach max of temp enchants
			if(Slot >= 11) return -1;
			*/
		/*}
	}   
*/

	//now why would this happen ?
	if( Slot >= (ITEM_FIELD_ENCHANTMENT_14_3-ITEM_FIELD_ENCHANTMENT_1_1)/3 )
	{
		sLog.outError("Error: something is trying to place an enchantement in a slot(%d) that would corrupt the item",Slot);
		return Slot; //oh no, what about error codes ?
	}

	//this should do nothing theoretically
	RemoveEnchantment( Slot );

	// Create the enchantment struct.
	EnchantmentInstance *Instance;
	Instance = new EnchantmentInstance;
	Instance->ApplyTime = UNIXTIME;
	Instance->BonusApplied = false;
	Instance->Slot = Slot;
	Instance->Enchantment = Enchantment;
	Instance->Duration = Duration;
	Instance->RemoveAtLogout = RemoveAtLogout;
	Instance->RandomSuffixPrefix = RandomSuffixPrefix;

	// Set the enchantment in the item fields.
	uint32 EnchantBase = Slot * 3 + ITEM_FIELD_ENCHANTMENT_1_1 ;
	if( Enchantment->custom_ScriptCreated == 0 )
	{
		SetUInt32Value( EnchantBase, Enchantment->Id );
		SetUInt32Value( EnchantBase + 1, (uint32)Instance->ApplyTime );
		SetUInt32Value( EnchantBase + 2, 0 ); // charges - valid for specific enchantments only
	}

	// Add it to our map.
	Enchantments[(uint32)Slot ] = Instance;

	if( m_owner == NULL )
	{ 
		return Slot;
	}

	// Add the removal event.
	if( Duration )
		sEventMgr.AddEvent( this, &Item::RemoveEnchantment, uint32(Slot), EVENT_REMOVE_ENCHANTMENT1 + Slot, Duration * 1000, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );

	// No need to send the log packet, if the owner isn't in world (we're still loading)
	if( !m_owner->IsInWorld() )
	{
		return Slot;
	}

	if( apply && Enchantment->custom_ScriptCreated == 0 )
	{
		WorldPacket EnchantLog( SMSG_ENCHANTMENTLOG, 25 );
		EnchantLog << m_owner->GetNewGUID();
		EnchantLog << m_owner->GetNewGUID();
		EnchantLog << m_uint32Values[OBJECT_FIELD_ENTRY];
		EnchantLog << Enchantment->Id;
		m_owner->GetSession()->SendPacket( &EnchantLog );

		if( m_owner->GetTradeTarget() )
			m_owner->SendTradeUpdate();
	
	}

	// Only apply the enchantment bonus if we're equipped 
	uint16 item_slot = m_owner->GetItemInterface()->GetInventorySlotByGuid( GetGUID() );
	if( item_slot >= EQUIPMENT_SLOT_START && item_slot < EQUIPMENT_SLOT_END )
	{
           ApplyEnchantmentBonus( Slot, apply );
		   if( apply == true && Enchantment->custom_ScriptCreated == 0 )
		   {
				uint32 mask;
				uint32 val;
				if( Slot % 2 == 0 )
				{
					mask = 0xFFFF0000; //clear lower part
					val = Enchantment->Id;
				}
				else
				{
					mask = 0x0000FFFF;//clear high part
					val = Enchantment->Id << 16;
				}
				uint32 old_value = m_owner->GetUInt32Value( PLAYER_VISIBLE_ITEM_1_ENCHANTMENT + item_slot * 2 );
				uint32 new_value = (old_value & mask) | val;
				m_owner->SetUInt32Value( PLAYER_VISIBLE_ITEM_1_ENCHANTMENT + item_slot * 2, new_value );
		   }
		   //never happens, this function only adds enchantsments
//		   else
//				m_owner->SetUInt32Value( PLAYER_VISIBLE_ITEM_1_ENCHANTMENT + item_slot * 2, 0 );
	}

	return Slot;
}

void Item::RemoveEnchantment( uint32 EnchantmentSlot )
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	// Make sure we actually exist.
	EnchantmentMap::iterator itr = Enchantments.find( EnchantmentSlot );
	if( itr == Enchantments.end() )
	{ 
		return;
	}

	m_isDirty = true;
	uint32 Slot = itr->first;
	if( itr->second->BonusApplied )
	{
		ApplyEnchantmentBonus( EnchantmentSlot, REMOVE );
		uint16 item_slot = m_owner->GetItemInterface()->GetInventorySlotByGuid( GetGUID() );
		if( item_slot >= EQUIPMENT_SLOT_START && item_slot < EQUIPMENT_SLOT_END )
		{
			uint32 mask;
			if( Slot % 2 == 0 )
				mask = 0xFFFF0000; //clear lower part
			else
				mask = 0x0000FFFF;//clear high part
			uint32 old_value = m_owner->GetUInt32Value( PLAYER_VISIBLE_ITEM_1_ENCHANTMENT + item_slot * 2 );
			uint32 new_value = old_value & mask;
			m_owner->SetUInt32Value( PLAYER_VISIBLE_ITEM_1_ENCHANTMENT + item_slot * 2, new_value );
		}
	}

	// Unset the item fields.
	uint32 EnchantBase = Slot * 3 + ITEM_FIELD_ENCHANTMENT_1_1 ;

	//never hurts to double check
	if( EnchantBase > ITEM_FIELD_ENCHANTMENT_12_3 )
	{ 
		return;
	}

	SetUInt32Value( EnchantBase + 0, 0 );
	SetUInt32Value( EnchantBase + 1, 0 );
	SetUInt32Value( EnchantBase + 2, 0 );

	// Remove the enchantment event for removal.
	event_RemoveEvents( EVENT_REMOVE_ENCHANTMENT1 + Slot );

	// delete the actual enchantment 
	delete itr->second;
	itr->second = NULL;	//just make sure it is not used anywhere else

	// Remove the enchantment instance.
	Enchantments.erase( itr );
}

//some gems can have effect applied only X times
uint32 Item::GetEnchantCountOfCustomGroup( uint32 custom_group )
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	uint32 ret=0;
	EnchantmentMap::iterator itr, itr2;
	for( itr = Enchantments.begin(); itr != Enchantments.end();  )
	{
		itr2 = itr++;
		if( itr2->second->BonusApplied == false )
			continue; //now why is that ?
		if( itr2->second->Enchantment->custom_enchant_group == custom_group )
			ret++;
	}
	return ret;
}

void Item::ApplyEnchantmentBonus( uint32 Slot, bool Apply )
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	if( m_owner == NULL 
		//this is exploit protection for enchanting broken items while equipped
		|| ( ( GetUInt32Value( ITEM_FIELD_DURABILITY ) == 0 && GetUInt32Value( ITEM_FIELD_MAXDURABILITY ) != 0 )
				&& Apply == true )	//we should be able to remove enchant when item "just broke down"
		)
	{ 
		return;
	}

	//this is not really an enchant but a row in a DBC ...
	if( Slot == REFORGE_ENCHANTMENT_SLOT )
		return;

	EnchantmentMap::iterator itr = Enchantments.find( Slot );
	if( itr == Enchantments.end() )
	{ 
		return;
	}

	if( itr->second->BonusApplied == Apply )
	{ 
		return;
	}

	EnchantEntry* Entry = itr->second->Enchantment;
	int32 ItemEquipSlot = -1;
	if( Apply )
	{
		if( Entry->required_skill && GetOwner() && GetOwner()->_GetSkillLineCurrent(Entry->required_skill,true) < Entry->required_skill_rank )
		{
			if( Entry->Name )
			{
				if( GetProto()->Name1 )
					GetOwner()->BroadcastMessage("Could not apply enchantment %s from %s", Entry->Name, GetProto()->Name1);
				else
					GetOwner()->BroadcastMessage("Could not apply enchantment: %s", Entry->Name);
			}
			else
				GetOwner()->BroadcastMessage("Could not apply enchantment from item");
			return;
		}
		// check if we are allowed to add any more of this enchant type. First appeared at prismatic gems
		if( Entry->custom_enchant_group && Entry->custom_enchant_maxstack && GetOwner() && GetOwner()->GetItemInterface() )
		{
			uint32 cur_grouped_enchant_count = 0;
			//get each equipped item 
			ItemInterface *ii = GetOwner()->GetItemInterface();
			for(int i=EQUIPMENT_SLOT_START;i<EQUIPMENT_SLOT_END;i++)
			{
				//see how many enchants of this type the player has already applied from this item
				Item *it = ii->GetInventoryItem( i );
				if( it )
					cur_grouped_enchant_count += it->GetEnchantCountOfCustomGroup( Entry->custom_enchant_group );
				if( it == this )
					ItemEquipSlot = i;

				//check if there is still a chance to apply the enchantment after checking this item
				if( cur_grouped_enchant_count >= Entry->custom_enchant_maxstack )
				{
					if( GetOwner() )
					{
						if( Entry->Name )
						{
							if( GetProto()->Name1 )
								GetOwner()->BroadcastMessage("Could not apply enchantment %s from %s", Entry->Name, GetProto()->Name1);
							else
								GetOwner()->BroadcastMessage("Could not apply enchantment: %s", Entry->Name);
						}
						else
							GetOwner()->BroadcastMessage("Could not apply enchantment from item");
					}
					return;
				}
			}
		}
		// Send the enchantment time update packet.
		if( itr->second->Duration ) 
			SendEnchantTimeUpdate( itr->second->Slot, itr->second->Duration );
		//increase player gear score if possible
		if( GetOwner() )
			GetOwner()->item_enchant_level_sum += Entry->custom_GearScore;
		if( ItemEquipSlot == - 1)
		{
			ItemInterface *ii = GetOwner()->GetItemInterface();
			for(int i=EQUIPMENT_SLOT_START;i<EQUIPMENT_SLOT_END;i++)
			{
				//see how many enchants of this type the player has already applied from this item
				Item *it = ii->GetInventoryItem( i );
				if( it == this )
					ItemEquipSlot = i;
			}
		}
	}
	else
	{
		//increase player gear score if possible
		if( GetOwner() && GetOwner()->item_enchant_level_sum >= Entry->custom_GearScore )
			GetOwner()->item_enchant_level_sum -= Entry->custom_GearScore;
	}

	itr->second->BonusApplied = Apply;

	// Apply the visual on the player.
	if( Slot == 0 )
	{
		uint32 ItemSlot = m_owner->GetItemInterface()->GetInventorySlotByGuid( GetGUID() ) * (PLAYER_VISIBLE_ITEM_2_ENTRYID - PLAYER_VISIBLE_ITEM_1_ENTRYID);
		uint32 VisibleBase = PLAYER_VISIBLE_ITEM_1_ENTRYID + ItemSlot;
		m_owner->SetUInt32Value( VisibleBase + 1 + Slot, Apply ? Entry->Id : 0 );
	}

	// Another one of those for loop that where not indented properly god knows what will break
	// but i made it actually affect the code below it
	uint32 RandomSuffixPrefix = itr->second->RandomSuffixPrefix;
	uint32 RandomSuffixFactor = GetItemRandomSuffixFactor();
	uint32 RandomSuffixVal = RANDOM_SUFFIX_MAGIC_CALCULATION( RandomSuffixPrefix, RandomSuffixFactor );
	// this is a huge crap!!, but this is what client shows for item level 359(epic) and factor 278 for suffix 135, 137...
	// item level 272(green), factor 161 the extra value was 51
	// need to figure out the real reason for this. Client adds some bonus to the factor
	RandomSuffixVal = RandomSuffixVal * 131 / 100;	

	for( uint32 c = 0; c < 3; c++ )
	{
		if( Entry->type[c] )
		{
			// Depending on the enchantment type, take the appropriate course of action.
			switch( Entry->type[c] )
			{
			case SPELL_ENCHANT_TYPE_ADD_SPELL:
				{
					//this spell will be added to item and client will let player trigger it when he wants it
					if( Entry->spell[c] != 0 )
					{
						if( Apply )
							AddTemporalSpellBookSpell( Entry->spell[c] );
						else
							RemoveTemporalSpellBookSpell( Entry->spell[c] );
					}

				}break;
			case SPELL_ENCHANT_TYPE_TRIGGER_SPELL_ON_MELEE_HIT:		 // Trigger spell on melee attack.
				{
					if( Apply && Entry->spell[c] != 0 )
					{
						// Create a proc trigger spell
						SpellEntry* sp = dbcSpell.LookupEntryForced( Entry->spell[c] );
						if( !sp )
							continue;
						ProcTriggerSpell *TS = new ProcTriggerSpell(sp, this);
						TS->caster = m_owner->GetGUID();
						TS->origId = 0;
						if( sp->procFlags == 0 || sp->procFlags == PROC_ON_MELEE_ATTACK )
						{
							//do not proc main hand enchants for offhand proc
							if( ItemEquipSlot != -1 )
							{
								if( ItemEquipSlot == EQUIPMENT_SLOT_MAINHAND )
									TS->procFlags = PROC_ON_MELEE_ATTACK;
								else if( ItemEquipSlot == EQUIPMENT_SLOT_OFFHAND )
									TS->procFlags = PROC_ON_MELEE_ATTACK | PROC_OFFHAND_WEAPON; //mark that this is an offhand proc
								else if( ItemEquipSlot == EQUIPMENT_SLOT_RANGED )
									TS->procFlags = PROC_ON_RANGED_ATTACK;
							}
							else
								TS->procFlags = PROC_ON_MELEE_ATTACK;	//for example Power Torrent procs on spell hit
						}
						else
							TS->procFlags = sp->procFlags;

						TS->procCharges = 0;
						/* This needs to be modified based on the attack speed of the weapon.
						 * Secondly, need to assign some static chance for instant attacks (ss,
						 * gouge, etc.) */
						if( Entry->min[c] )
							TS->procChance = Entry->min[c];
						else if( TS->procChance <= 1 )
							TS->procChance = 50;
						if( GetProto()->Class == ITEM_CLASS_WEAPON )
						{
							/////// procChance calc ///////
							if( sp->proc_interval != DEFAULT_SPELL_ITEM_PROC_INTERVAL && sp->proc_interval != DEFAULT_SPELL_PROC_INTERVAL )
								TS->procInterval = sp->proc_interval;
							else
							{
								float ppm = 0;
								switch( sp->NameHash )
								{
									case SPELL_HASH_FROSTBRAND_ATTACK:
										ppm = 60000.0f / 2000.0f;
									break;
									//removed since this is already set in spellfixes.cpp
/*									case SPELL_HASH_INSTANT_POISON:
										ppm = 8.53f * 0.9f;	//!!! can be moded by talent !
									break;
									case SPELL_HASH_WOUND_POISON:
										ppm = 21.43f * 0.9f;	//!!! can be moded by talent !
									break; */
								}
								if( ppm != 0 )
									TS->procInterval = uint32(60000 / ppm);	
								else if( Entry->min[c] == 0 )
								{
									float speed = (float)GetProto()->Delay;
		//								TS->procChance = (uint32)( speed / 600.0f );
									TS->procChance = (uint32)( speed / 60.0f );
								}
								if( TS->procInterval < 500 )
									TS->procInterval = 500;	//someone reported that deathfrost hit 500 times a second. I have a feeling that was a different exploit
							}
							///////////////////////////////
						}
						
						Log.Debug( "Enchant", "Setting procChance to %u%% with interval %u.", TS->procChance,TS->procInterval );
						TS->deleted = false;
						TS->spellId = Entry->spell[c];
						TS->LastTrigger = getMSTime()+ TS->procInterval;	//required that on login to have a reference time
						m_owner->RegisterProcStruct( TS );

						//cosmetic : see if we already have this proc spell, if so, then try to make it that we will proc NOT at the same time
						CommitPointerListNode<ProcTriggerSpell> *itr;
						for(itr = m_owner->m_procSpells.begin();itr != m_owner->m_procSpells.end();itr = itr->Next() )
							if( itr->data->origId == 0 && itr->data->spellId == Entry->spell[c] && itr->data->deleted == false && itr->data->caster == m_owner->GetGUID() )
							{
								TS->LastTrigger = itr->data->LastTrigger + TS->procInterval / 2;
//								break; 
							}
					}
					else
					{
						// Remove the proctriggerspell
						CommitPointerListNode<ProcTriggerSpell> *itr;
						for(itr = m_owner->m_procSpells.begin();itr != m_owner->m_procSpells.end();itr = itr->Next() )
							if( itr->data->owner == this && itr->data->origId == 0 && itr->data->spellId == Entry->spell[c] && itr->data->deleted == false && itr->data->caster == m_owner->GetGUID() )
							{
								itr->data->deleted = true;
//								break; //all or 1 ?
							}
					}
				}break;

			case SPELL_ENCHANT_TYPE_MOD_DMG:		 // Mod damage done.
				{
					int32 val = Entry->min[c];
					if( RandomSuffixVal )
						val = RandomSuffixVal;

//					if( m_owner->getClass() == DEATHKNIGHT )
//						continue;

					if( Apply )
						m_owner->ModUnsigned32Value( PLAYER_FIELD_MOD_DAMAGE_DONE_POS, val );
					else
						m_owner->ModUnsigned32Value( PLAYER_FIELD_MOD_DAMAGE_DONE_POS, -val );
					m_owner->CalcDamage();
				}break;

			case SPELL_ENCHANT_TYPE_CAST_SPELL:		 // Cast spell (usually means apply aura)
				{
					if( Apply )
					{
						SpellCastTargets targets( m_owner->GetGUID() );
						SpellEntry* sp;
						Spell* spell;
						
						if( Entry->spell[c] != 0 )
						{
							sp = dbcSpell.LookupEntry( Entry->spell[c] );
							if( sp == NULL )
								continue;

							spell = SpellPool.PooledNew( __FILE__, __LINE__ );
							spell->Init( m_owner, sp, true, 0 );
							spell->i_caster = this;
							spell->prepare( &targets );
						}
					}
					else
					{
						if( Entry->spell[c] != 0 )
								m_owner->RemoveAura( Entry->spell[c] );
					}

				}break;

			case SPELL_ENCHANT_TYPE_MOD_RESIST:		 // Modify physical resistance
				{
					int32 val = Entry->min[c];
					if( RandomSuffixVal )
						val = RandomSuffixVal;

					if( Apply )
					{
						m_owner->FlatResistanceModifierPos[Entry->spell[c]] += val;
					}
					else
					{
						m_owner->FlatResistanceModifierPos[Entry->spell[c]] -= val;
					}
					m_owner->CalcResistance( Entry->spell[c] );
				}break;

			case SPELL_ENCHANT_TYPE_MOD_ITEM_LIKE_STAT:	 //Modify rating ...order is PLAYER_FIELD_COMBAT_RATING_1 and above
				{
					//spellid is enum ITEM_STAT_TYPE
					//min=max is amount
					int32 val = Entry->min[c];
					if( RandomSuffixVal )
						val = RandomSuffixVal;

					m_owner->ModifyBonuses( Entry->spell[c], val, Apply );
					m_owner->UpdateStats();
				}break;

			case SPELL_ENCHANT_TYPE_MOD_DPS:	 // Rockbiter weapon (increase damage per second... how the hell do you calc that)
				{
					if( m_owner->getClass() == DEATHKNIGHT )
						continue;

					if( Apply )
					{
						//m_owner->ModUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS, Entry->min[c]);
						//if i'm not wrong then we should apply DMPS formula for this. This will have somewhat a larger value 28->34
						int32 val = Entry->min[c];
						if( RandomSuffixVal )
						val = RandomSuffixVal;

						int32 value = GetProto()->Delay * val / 1000;
						m_owner->ModUnsigned32Value( PLAYER_FIELD_MOD_DAMAGE_DONE_POS, value );
					}
					else
					{
						int32 val = Entry->min[c];
						if( RandomSuffixVal )
							val = RandomSuffixVal;

						int32 value =- (int32)(GetProto()->Delay * val / 1000 );
						m_owner->ModUnsigned32Value( PLAYER_FIELD_MOD_DAMAGE_DONE_POS, value );
					}
					m_owner->CalcDamage();
				}break;
			case SPELL_ENCHANT_TYPE_ADD_SOCKET:	 // add socket holder
				{
					//nothing to be done atm. Client will show socket. I think spell id is socket color mask
				}break;
			case SPELL_ENCHANT_TYPE_CUSTOM_PROC_ANY_DMG_DONE:		
			case SPELL_ENCHANT_TYPE_CUSTOM_PROC_ANY_DMG_TAKEN:		
				{
					if( Apply && Entry->spell[c] != 0 )
					{
						// Create a proc trigger spell
						SpellEntry* sp = dbcSpell.LookupEntryForced( Entry->spell[c] );
						if( !sp )
							continue;

						ProcTriggerSpell *TS = new ProcTriggerSpell(sp, this);
						TS->caster = m_owner->GetGUID();
						TS->origId = 0;
						TS->procCharges = 0;
						TS->procChance = 50;
						if( Entry->min[c] )
							TS->procChance = Entry->min[c];
						TS->procInterval = 30000;	
						if( Entry->max[c] )
							TS->procInterval = Entry->max[c];	
						Log.Debug( "Enchant", "Setting procChance to %u%% with interval %u.", TS->procChance, TS->procInterval );
						TS->deleted = false;
						TS->spellId = Entry->spell[c];
						TS->LastTrigger = getMSTime()+ TS->procInterval / 2 + RandomUInt() % ( TS->procInterval / 2 );	
						char SpellName[200];
						GetSpellLinkByProto( sp->Id, SpellName );
						char ItemName[200];
						GetItemLinkByProto( GetProto(), ItemName );
						if( Entry->type[c] == SPELL_ENCHANT_TYPE_CUSTOM_PROC_ANY_DMG_DONE )
						{
							TS->procFlags = PROC_ON_ANY_DAMAGE_DONE;
							GetOwner()->BroadcastMessage("%s added on Attack proc %s - chance %d - cooldown %d sec", ItemName, SpellName, TS->procChance, TS->procInterval / 1000 );
						}
						else
						{
							TS->procFlags = PROC_ON_ANY_DAMAGE_VICTIM;
							GetOwner()->BroadcastMessage("%s added on Hit proc : %s - chance %d - cooldown %d sec", ItemName, SpellName, TS->procChance, TS->procInterval / 1000 );
						}
						m_owner->RegisterProcStruct( TS );
					}
					else
					{
						// Remove the proctriggerspell
						CommitPointerListNode<ProcTriggerSpell> *itr;
						for(itr = m_owner->m_procSpells.begin();itr != m_owner->m_procSpells.end();itr = itr->Next() )
							if( itr->data->owner == this && itr->data->origId == 0 && itr->data->spellId == Entry->spell[c] && itr->data->deleted == false && itr->data->caster == m_owner->GetGUID() )
							{
								itr->data->deleted = true;
//								break; //all or 1 ?
							}
					}
				}break;
			case SPELL_ENCHANT_TYPE_CUSTOM_MOD_ITEM_STAT:	
				{
					int32 val = Entry->min[c];
					m_owner->ModifyBonuses( Entry->spell[c], val, Apply );
					m_owner->UpdateStats();
					if( Apply )
					{
						char ItemName[200];
						GetItemLinkByProto( GetProto(), ItemName );
						if( val > 0 )
							GetOwner()->BroadcastMessage("%s%s added stat mod %s %d|H", ItemName, MSG_COLOR_GREEN, Entry->Name, val );
						else
							GetOwner()->BroadcastMessage("%s%s added stat mod %s %d|H", ItemName, MSG_COLOR_LIGHTRED, Entry->Name, val );
					}
				}break;
			default:
				{
					sLog.outError( "Unknown enchantment type: %u (%u)", Entry->type[c], Entry->Id );
				}break;
			}
		}
	}
}

void Item::ApplyEnchantmentBonuses()
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	EnchantmentMap::iterator itr, itr2;
	EnchantmentMap tlist = Enchantments;	//somehow this list get emptied while updating enchantments
	for( itr = tlist.begin(); itr != tlist.end(); itr++ )
	{
		itr2 = Enchantments.find( itr->first );
		if( itr2 != Enchantments.end() )
			ApplyEnchantmentBonus( itr2->first, APPLY );
	}
	//this is not an enchant. But should be applied anyway
	ApplyReforging( true );
}

void Item::RemoveEnchantmentBonuses()
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	EnchantmentMap::iterator itr, itr2;
	for( itr = Enchantments.begin(); itr != Enchantments.end(); )
	{
		itr2 = itr++;
		ApplyEnchantmentBonus( itr2->first, REMOVE );
	}
	//this is not an enchant. But should be applied anyway
	ApplyReforging( false );
}

void Item::EventRemoveEnchantment( uint32 Slot )
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	// Remove the enchantment.
	RemoveEnchantment( Slot );
}

int32 Item::FindFreeEnchantSlot( EnchantEntry* Enchantment, uint32 random_type )
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	
	uint32 ItemSocketCount,EnchantSocketCount,TotalSocketCount;
	TotalSocketCount = GetSocketsCount( ItemSocketCount, EnchantSocketCount );
	uint32 GemSlotsReserve = TotalSocketCount;
	if( GetProto()->SocketBonus )
		GemSlotsReserve++;

	if( random_type == RANDOMPROPERTY )		// random prop
	{
		for( uint32 Slot = ITEM_ENCHANT_SLOT_RANDOM_PROP2; Slot <= ITEM_ENCHANT_SLOT_RANDOM_PROP4; ++Slot )
			if( m_uint32Values[ITEM_FIELD_ENCHANTMENT_1_1 + Slot * 3] == 0 )
			{ 
				return Slot;
			}
	}
	else if( random_type == RANDOMSUFFIX )	// random suffix
	{
		for( uint32 Slot = ITEM_ENCHANT_SLOT_RANDOM_PROP0; Slot <= ITEM_ENCHANT_SLOT_RANDOM_PROP4; ++Slot )
			if( m_uint32Values[ITEM_FIELD_ENCHANTMENT_1_1 + Slot * 3] == 0 )
			{ 
				return Slot;
			}
	}
	
	for( uint32 Slot = GemSlotsReserve + ITEM_ENCHANT_SLOT_GEM1; Slot <= ITEM_ENCHANT_SLOT_RANDOM_PROP4; Slot++ )
	{
		if( m_uint32Values[ITEM_FIELD_ENCHANTMENT_1_1 + Slot * 3] == 0 )
		{ 
			return Slot;	
		}
	}

	return -1;
}

int32 Item::HasEnchantment( uint32 Id )
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	for( uint32 Slot = 0; Slot <= ITEM_ENCHANT_SLOT_RANDOM_PROP4; Slot++ )
	{
		if( m_uint32Values[ITEM_FIELD_ENCHANTMENT_1_1 + Slot * 3] == Id )
		{ 
			return Slot;
		}
	}

	return -1;
}

bool Item::HasEnchantmentOnSlot( uint32 slot )
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	EnchantmentMap::iterator itr = Enchantments.find( slot );
	if( itr == Enchantments.end() )
	{ 
		return false;
	}

	return true;
}

void Item::ModifyEnchantmentTime( uint32 Slot, uint32 Duration )
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	EnchantmentMap::iterator itr = Enchantments.find( Slot );
	if( itr == Enchantments.end() )
	{ 
		return;
	}

	// Reset the apply time.
	itr->second->ApplyTime = UNIXTIME;
	itr->second->Duration = Duration;

	// Change the event timer.
	event_ModifyRepeatInterval( EVENT_REMOVE_ENCHANTMENT1 + Slot, Duration * 1000 );

	// Send update packet
	SendEnchantTimeUpdate( itr->second->Slot, Duration );
}

void Item::SendEnchantTimeUpdate( uint32 Slot, uint32 Duration )
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	/*
	{SERVER} Packet: (0x01EB) SMSG_ITEM_ENCHANT_TIME_UPDATE Size = 24
	|------------------------------------------------|----------------|
	|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
	|------------------------------------------------|----------------|
	|69 32 F0 35 00 00 00 40 01 00 00 00 08 07 00 00 |i2.5...@........|
	|51 46 35 00 00 00 00 00						 |QF5.....		|
	-------------------------------------------------------------------

	uint64 item_guid
	uint32 count?
	uint32 time_in_seconds
	uint64 player_guid
	*/

	WorldPacket* data = new WorldPacket(SMSG_ITEM_ENCHANT_TIME_UPDATE, 24 );
	*data << GetGUID();
	*data << Slot;
	*data << Duration;
	*data << m_owner->GetGUID();
	if( m_owner->IsInWorld() && m_owner->GetSession() )
	{
		m_owner->GetSession()->SendPacket( data );
		delete data;
	}
	else
		m_owner->delayedPackets.add( data );
}

void Item::RemoveAllEnchantments( bool OnlyTemporary )
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	EnchantmentMap::iterator itr, it2;
	for( itr = Enchantments.begin(); itr != Enchantments.end(); )
	{
		it2 = itr++;

		if( OnlyTemporary && it2->second->Duration == 0 ) 
			continue;
			
		RemoveEnchantment( it2->first );
	}
}

void Item::RemoveRelatedEnchants( EnchantEntry* newEnchant )
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	EnchantmentMap::iterator itr,itr2;
	for( itr = Enchantments.begin(); itr != Enchantments.end(); )
	{
		itr2 = itr++;
		
		if( itr2->second->Enchantment->Id == newEnchant->Id || ( itr2->second->Enchantment->EnchantGroups > 1 && newEnchant->EnchantGroups > 1 ) )
		{ 
			RemoveEnchantment( itr2->first );
		}
	}
}

void Item::RemoveProfessionEnchant(int32 slot)
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	EnchantmentMap::iterator itr;
	for( itr = Enchantments.begin(); itr != Enchantments.end(); itr++ )
	{
		if( itr->second->Duration != 0 )// not perm
			continue;
		if( IsGemRelated( itr->second->Enchantment ) )
			continue;
		//not dead sure this is good, but enginiering enchants stack with enchanting enchants 
		if( slot != -1 && itr->second->Slot != slot )
			continue;

		RemoveEnchantment( itr->first );
		return;
	}
}

void Item::RemoveSocketBonusEnchant()
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	EnchantmentMap::iterator itr;
	
	for( itr = Enchantments.begin(); itr != Enchantments.end(); itr++ )
	{
		if( itr->second->Enchantment->Id == GetProto()->SocketBonus )
		{
			RemoveEnchantment( itr->first );
			return;
		}	
	}
}

EnchantmentInstance* Item::GetEnchantment( uint32 slot )
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	EnchantmentMap::iterator itr = Enchantments.find( slot );
	if( itr != Enchantments.end() )
	{ 
		return itr->second;
	}
	else
		return NULL;
}

bool Item::IsGemRelated( EnchantEntry* Enchantment )
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	if( GetProto()->SocketBonus == Enchantment->Id )
	{ 
		return true;
	}
	
	return( Enchantment->GemEntry != 0 );
}

uint32 Item::GetSocketsCount( uint32 &ItemSocketCount, uint32 &EnchantSocketCount )
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	ItemSocketCount = EnchantSocketCount = 0;
	if( IsContainer() ) // no sockets on containers.
	{ 
		return 0;
	}

	for( uint32 x = 0; x < 3; x++ )
		if( GetProto()->Sockets.SocketColor[x] )
			ItemSocketCount++;

	//Enchantments may add extra sockets
	EnchantmentMap::iterator itr;
	for( itr = Enchantments.begin(); itr != Enchantments.end(); itr++ )
		for(int i=0;i<3;i++)
			if( itr->second->Enchantment->type[i] == SPELL_ENCHANT_TYPE_ADD_SOCKET )
				EnchantSocketCount++;

	return (ItemSocketCount + EnchantSocketCount);
}

uint32 Item::GenerateRandomSuffixFactor( ItemPrototype* m_itemProto )
{
	ItemPrototype *itemProto = m_itemProto;

    if (!itemProto)
        return 0;
    if (!itemProto->RandomSuffixId)
        return 0;

    RandomPropertiesPointsEntry const *randomProperty = dbcRandomPropertyPoints.LookupEntry( itemProto->ItemLevel );
    if (!randomProperty)
        return 0;

    uint32 suffixFactor;
    switch( itemProto->InventoryType )
    {
        // Items of that type don`t have points
        case INVTYPE_NON_EQUIP:
        case INVTYPE_BAG:
        case INVTYPE_TABARD:
        case INVTYPE_AMMO:
        case INVTYPE_QUIVER:
        case INVTYPE_RELIC:
            return 0;
            // Select point coefficient
        case INVTYPE_HEAD:
        case INVTYPE_BODY:
        case INVTYPE_CHEST:
        case INVTYPE_LEGS:
        case INVTYPE_2HWEAPON:
        case INVTYPE_ROBE:
            suffixFactor = 0;
            break;
        case INVTYPE_SHOULDERS:
        case INVTYPE_WAIST:
        case INVTYPE_FEET:
        case INVTYPE_HANDS:
        case INVTYPE_TRINKET:
            suffixFactor = 1;
            break;
        case INVTYPE_NECK:
        case INVTYPE_WRISTS:
        case INVTYPE_FINGER:
        case INVTYPE_SHIELD:
        case INVTYPE_CLOAK:
        case INVTYPE_HOLDABLE:
            suffixFactor = 2;
            break;
        case INVTYPE_WEAPON:
        case INVTYPE_WEAPONMAINHAND:
        case INVTYPE_WEAPONOFFHAND:
            suffixFactor = 3;
            break;
        case INVTYPE_RANGED:
        case INVTYPE_THROWN:
        case INVTYPE_RANGEDRIGHT:
            suffixFactor = 4;
            break;
        default:
            return 0;
    }
    // Select rare/epic modifier
    switch (itemProto->Quality)
    {
        case ITEM_QUALITY_UNCOMMON_GREEN:
            return randomProperty->UncommonPropertiesPoints[suffixFactor];
        case ITEM_QUALITY_RARE_BLUE:
            return randomProperty->RarePropertiesPoints[suffixFactor];
        case ITEM_QUALITY_EPIC_PURPLE:
            return randomProperty->EpicPropertiesPoints[suffixFactor];
        case ITEM_QUALITY_LEGENDARY_ORANGE:
        case ITEM_QUALITY_ARTIFACT_LIGHT_YELLOW:
            return 0;                                       // not have random properties
        default:
            break;
    }
    return 0; 
}

string Item::GetItemLink(uint32 language = NULL)
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	return GetItemLinkByProto(GetProto(), language);
}

string GetItemLinkByProto(ItemPrototype * iProto, uint32 language = NULL)
{
	const char * ItemLink;
	char buffer[256];
	std::string colour;

	switch(iProto->Quality)
	{
		case ITEM_QUALITY_POOR_GREY:
			colour = "cff9d9d9d";
		break;
		case ITEM_QUALITY_NORMAL_WHITE:
			colour = "cffffffff";
		break;
		case ITEM_QUALITY_UNCOMMON_GREEN:
			colour = "cff1eff00";
		break;
		case ITEM_QUALITY_RARE_BLUE:
			colour = "cff0070dd";
		break;
		case ITEM_QUALITY_EPIC_PURPLE:
			colour = "cffa335ee";
		break;
		case ITEM_QUALITY_LEGENDARY_ORANGE:
			colour = "cffff8000";
		break;
		case ITEM_QUALITY_ARTIFACT_LIGHT_YELLOW:
			colour = "c00fce080";
		break;
		case ITEM_QUALITY_HEIRLOOM_LIGHT_YELLOW:
			colour = "c00fce080";
		break;
		default:
			colour = "cff9d9d9d";
	}
	
	// try to get localized version
	LocalizedItem *lit	= (language>0) ? sLocalizationMgr.GetLocalizedItem(iProto->ItemId, language) : NULL;

	if(lit)
		snprintf(buffer, 256, "|%s|Hitem:%u:0:0:0:0:0:0:0|h[%s]|h|r", colour.c_str(), iProto->ItemId, lit->Name);
	else
		snprintf(buffer, 256, "|%s|Hitem:%u:0:0:0:0:0:0:0|h[%s]|h|r", colour.c_str(), iProto->ItemId, iProto->Name1);
	

	ItemLink	= reinterpret_cast<const char*>(buffer);

	return ItemLink;
}

void GetItemLinkByProto( ItemPrototype * iProto, char *buffer )
{
	string str = GetItemLinkByProto( iProto, LANG_ENGLISH );
	strcpy( buffer, str.c_str() );
}

int32 Item::event_GetInstanceID() 
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	if( m_owner )
	{ 
		return m_owner->event_GetInstanceID();
	}
	return WORLD_INSTANCE;
}

void Item::AddTemporalSpellBookSpell(uint32 new_spell)
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	if( HasTemporalSpellBookSpell( new_spell ) )
		return;
	enchant_spell_book.push_back( new_spell );
}

void Item::RemoveTemporalSpellBookSpell(uint32 new_spell)
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	enchant_spell_book.remove( new_spell );

/*	list<uint32>::iterator itr;
	for(itr=enchant_spell_book.begin();itr!=enchant_spell_book.end();itr++)
		if( *itr == new_spell )
		{
			enchant_spell_book.remove( *itr );
			return;
		}*/
}

bool Item::HasTemporalSpellBookSpell(uint32 spellId)
{
	INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE
	list<uint32>::iterator itr;
	for(itr=enchant_spell_book.begin();itr!=enchant_spell_book.end();itr++)
		if( *itr == spellId )
		{
			return true;
		}
	return false;
}

void Item::SendItemDuration()
{
    if (!GetUInt32Value(ITEM_FIELD_DURATION))
        return;

	sStackWorldPacket( data ,SMSG_ITEM_TIME_UPDATE, 8+4+2);
    data << (uint64)GetGUID();
    data << (uint32)GetUInt32Value(ITEM_FIELD_DURATION);
    m_owner->GetSession()->SendPacket(&data);
}

// converts one stat into another based on some factor
void Item::ApplyReforging( bool apply )
{
	uint32 reid = GetUInt32Value( ITEM_FIELD_ENCHANTMENT_1_1 + REFORGE_ENCHANTMENT_SLOT * 3 );
	if( reid == 0 )
		return;
	ItemReforgeEntry *re = dbcItemReforge.LookupEntryForced( reid );
	if( re == NULL )
		return;
	int32 ammount = 0;
	for( uint32 i=0;i<10;i++)
		if( m_itemProto->Stats.Type[i] == re->Stat_Source )
		{
			ammount = float2int32( m_itemProto->Stats.Value[i] * re->conversion_factor );
		}
	//wtf ?
	if( ammount == 0 )
		return;

	GetOwner()->ModifyBonuses( re->Stat_Source, -ammount ,apply);
	GetOwner()->ModifyBonuses( re->Stat_Dest, ammount ,apply);
}

void Item::SetReforgeID( uint32 ID )
{
	SetUInt32Value( ITEM_FIELD_ENCHANTMENT_1_1 + REFORGE_ENCHANTMENT_SLOT * 3, ID );
	m_isDirty = true;	//save to db
}

void Item::Randomize( )
{
	int32 randomprop = 0;

//	if( ( it->RandomPropId != 0 ) && ( it->RandomSuffixId != 0 ) )
//		sLog.outError("Item %u ( %s ) has both RandomPropId and RandomSuffixId.", itemid, it->Name1 );

	ItemPrototype *it = GetProto();
	if( it->RandomPropId != 0 )
	{
		RandomProps *rp = lootmgr.GetRandomProperties( it );
		if( rp != NULL )
			randomprop = rp->ID;
		else
			sLog.outError( "Item %u ( %s ) has unknown RandomPropId %u", it->ItemId, it->Name1, it->RandomPropId );
	}
	if( randomprop == 0 && it->RandomSuffixId != 0 )
	{
		ItemRandomSuffixEntry *rs = lootmgr.GetRandomSuffix( it );

		if( rs != NULL )
			randomprop = -1 * rs->id;
		else
			sLog.outError( "Item %u ( %s ) has unknown RandomSuffixId %u", it->ItemId, it->Name1, it->RandomSuffixId );
	}
	if( randomprop != 0 )
	{
		if( randomprop < 0 )
			SetRandomSuffix( -randomprop );
		else
			SetRandomProperty( randomprop );
		ApplyRandomProperties( false );
	}

	RandomizeEPLItem( this ); //this is custom, should make it a build option :(
}

uint32 ScalingStatGetStatMultiplier( ScalingStatValuesEntry *ssv, uint32 inventoryType)
{
	switch (inventoryType)
	{
		case INVTYPE_NON_EQUIP:
		case INVTYPE_BODY:
		case INVTYPE_BAG:
		case INVTYPE_TABARD:
		case INVTYPE_AMMO:
		case INVTYPE_QUIVER:
			return 0;
		case INVTYPE_HEAD:
		case INVTYPE_CHEST:
		case INVTYPE_LEGS:
		case INVTYPE_2HWEAPON:
		case INVTYPE_ROBE:
			return ssv->StatMultiplier[0];
		case INVTYPE_SHOULDERS:
		case INVTYPE_WAIST:
		case INVTYPE_FEET:
		case INVTYPE_HANDS:
		case INVTYPE_TRINKET:
			return ssv->StatMultiplier[1];
		case INVTYPE_NECK:
		case INVTYPE_WRISTS:
		case INVTYPE_FINGER:
		case INVTYPE_SHIELD:
		case INVTYPE_CLOAK:
		case INVTYPE_HOLDABLE:
			return ssv->StatMultiplier[2];
		case INVTYPE_RANGED:
		case INVTYPE_THROWN:
		case INVTYPE_RANGEDRIGHT:
		case INVTYPE_RELIC:
			return ssv->StatMultiplier[3];
		case INVTYPE_WEAPON:
		case INVTYPE_WEAPONMAINHAND:
		case INVTYPE_WEAPONOFFHAND:
			return ssv->StatMultiplier[4];
		default:
			break;
	}
	return 0;
}

uint32 ScalingStatGetArmor( ScalingStatValuesEntry *ssv, uint32 inventoryType, uint32 armorType )
{
	if (inventoryType <= INVTYPE_ROBE && armorType < 4)
	{
		switch (inventoryType)
		{
			case INVTYPE_NON_EQUIP:
			case INVTYPE_NECK:
			case INVTYPE_BODY:
			case INVTYPE_FINGER:
			case INVTYPE_TRINKET:
			case INVTYPE_WEAPON:
			case INVTYPE_SHIELD:
			case INVTYPE_RANGED:
			case INVTYPE_2HWEAPON:
			case INVTYPE_BAG:
			case INVTYPE_TABARD:
				break;
			case INVTYPE_SHOULDERS:
				return ssv->Armor[0][armorType];
			case INVTYPE_CHEST:
			case INVTYPE_ROBE:
				return ssv->Armor[1][armorType];
			case INVTYPE_HEAD:
				return ssv->Armor[2][armorType];
			case INVTYPE_LEGS:
				return ssv->Armor[3][armorType];
			case INVTYPE_FEET:
				return ssv->Armor[4][armorType];
			case INVTYPE_WAIST:
				return ssv->Armor[5][armorType];
			case INVTYPE_HANDS:
				return ssv->Armor[6][armorType];
			case INVTYPE_WRISTS:
				return ssv->Armor[7][armorType];
			case INVTYPE_CLOAK:
				return ssv->CloakArmor;
			default:
				break;
		}
	}
	return 0;
}

uint32 ScalingStatGetDPSAndDamageMultiplier( ScalingStatValuesEntry *ssv, uint32 subClass, bool isCasterWeapon, float *damageMultiplier )
{
	if (!isCasterWeapon)
	{
		switch (subClass)
		{
			case ITEM_SUBCLASS_WEAPON_AXE:
			case ITEM_SUBCLASS_WEAPON_MACE:
			case ITEM_SUBCLASS_WEAPON_SWORD:
			case ITEM_SUBCLASS_WEAPON_DAGGER:
			case ITEM_SUBCLASS_WEAPON_THROWN:
				*damageMultiplier = 0.3f;
				return ssv->dpsMod[0];
			case ITEM_SUBCLASS_WEAPON_TWOHAND_AXE:
			case ITEM_SUBCLASS_WEAPON_TWOHAND_MACE:
			case ITEM_SUBCLASS_WEAPON_POLEARM:
			case ITEM_SUBCLASS_WEAPON_TWOHAND_SWORD:
			case ITEM_SUBCLASS_WEAPON_STAFF:
			case ITEM_SUBCLASS_WEAPON_FISHING_POLE:
				*damageMultiplier = 0.2f;
				return ssv->dpsMod[1];
			case ITEM_SUBCLASS_WEAPON_BOW:
			case ITEM_SUBCLASS_WEAPON_GUN:
			case ITEM_SUBCLASS_WEAPON_CROSSBOW:
				*damageMultiplier = 0.3f;
				return ssv->dpsMod[4];
			case ITEM_SUBCLASS_WEAPON_Obsolete:
			case ITEM_SUBCLASS_WEAPON_EXOTIC:
			case ITEM_SUBCLASS_WEAPON_EXOTIC2:
			case ITEM_SUBCLASS_WEAPON_FIST_WEAPON:
			case ITEM_SUBCLASS_WEAPON_MISC_WEAPON:
			case ITEM_SUBCLASS_WEAPON_SPEAR:
			case ITEM_SUBCLASS_WEAPON_WAND:
				break;
		}
	}
	else
	{
		if (subClass <= ITEM_SUBCLASS_WEAPON_WAND)
		{
			uint32 mask = 1 << subClass;
			// two-handed weapons
			if (mask & 0x562)
			{
				*damageMultiplier = 0.2f;
				return ssv->dpsMod[3];
			}

			if (mask & (1 << ITEM_SUBCLASS_WEAPON_WAND))
			{
				*damageMultiplier = 0.3f;
				return ssv->dpsMod[5];
			}
		}
		*damageMultiplier = 0.3f;
		return ssv->dpsMod[2];
	}
	return 0;
}

#ifdef SMALL_ITEM_OBJECT
uint32 Item::GetZoneId()
{
	if( GetOwner() == NULL )
		return (uint32)(-1);
	return GetOwner()->GetZoneId();
}

uint32 Item::GetMapId()
{
	if( GetOwner() == NULL )
		return (uint32)(-1);
	return GetOwner()->GetMapId();
}

void Item::PushToWorld(MapMgr*mgr)
{
	if( !mgr )
		return; //instance add failed

//	m_mapId=mgr->GetMapId();
	m_instanceId = mgr->GetInstanceID();

	m_mapMgr = mgr;
//	OnPrePushToWorld();

//	mgr->PushObject(this);

	// correct incorrect instance id's
//	mSemaphoreTeleport = false;
//	m_inQueue = false;
   
	event_Relocate();
	
	// call virtual function to handle stuff.. :P
//	OnPushToWorld();
}

void Item::DestroyForPlayer(Player *target, uint8 anim)
{
	if(target->GetSession() == 0) 
	{ 
		return;
	}

	ASSERT(target);

	sStackWorldPacket( data, SMSG_DESTROY_OBJECT, 20 );
	data << GetGUID();
	data << uint8(anim);
	target->GetSession()->SendPacket( &data );
}

/*
Debug A9Reader : block(0)->type = 1 start at 7 
Debug A9Reader : Got into create object at pos = 7 
Debug A9Reader : read guid at pos = 13 
Debug A9Reader : object type = 4 
Moveblock (010): Read F1 = 0061 
Moveblock (020): Has flag 0x20 Flags1 , flags2 = 0 -> reading 10 bytes
Moveblock (030): Has flag 0x40 -> reading 4 floats 
Moveblock (0491): moveinfo ? -> reading 20 bytes
Moveblock (100): In speed block -> reading 9 floats = 36 bytes
Debug A9Reader : read Move Block end at pos = 98 
Debug Datareader : Masklen(12) for mask 0x03 at position 99 (expecting 0x2E)
Debug Datareader : The mask : C4 FF A0 CF 07 3C 00 00 40 EC 09 21 
Debug Datareader : updatemask requires 144 bytes and we have 39361 in packet (total 39472 and used 111 )
Debug A9Reader : read data end at pos = 255 
Debug A9Reader : block(0)->type = 1 end at 255 
Debug A9Reader : block content :  01 8F 56 C9 5E 03 01 04 61 00 00 00 00 00 00 88 4D 5C CA 49 F6 65 9B 44 F5 45 89 C5 6C 92 E0 41 A8 DA B6 40 21 00 00 00 00 00 00 00 00 00 20 40 CD CC 00 41 00 00 90 40 6D 1C 97 40 00 00 20 40 67 66 F6 40 00 00 90 40 DB 0F 49 40 DB 0F 49 40 2E 77 00 00 6E C1 F7 05 20 00 E3 7B 03 C4 FF A0 CF 07 3C 00 00 40 EC 09 21 84 50 4A 08 A1 84 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 A0 AA AA AA EA C0 FF FF FF FF F3 FF FF FF FF FF FF FF FF FF FF FF FF FF 03 00 00 C0 03 00 00 00 00 00 00 00 9C 2D C9 B6 2D 4B 96 6D C9 B2 7D 5B B6 25 D9 96 6C C9 96 64 5B B2 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 C0 F3 7F FF FF FF FF FF FF FF FF 3D FF FB FF F8 E3 FF FF FF 0F E4 07 FC 9F 00 00 00 28 FA B1
*/
uint32 Item::BuildCreateUpdateBlockForPlayer(ByteBuffer *data, Player *target)
{
	uint16 flags = 0;
	uint32 flags2 = 0;

	uint8 updatetype = UPDATETYPE_CREATE_OBJECT;

	if( target == GetOwner() )
	{
		// player creating self
		flags |= CREATE_FLAG_FOR_SELF;
		updatetype = UPDATETYPE_CREATE_OBJECT_SELF;
	}

	// build our actual update
	*data << updatetype;

	// we shouldn't be here, under any cercumstances, unless we have a wowguid..
	*data << m_wowGuid;
	
	*data << (uint8)(m_objectTypeId);

	//cromon says these got removed in 4.x client
	_BuildMovementUpdate(data, flags, flags2, target);

	// we have dirty data, or are creating for ourself.
	UpdateMask updateMask;
	updateMask.SetCount( m_valuesCount );
	_SetCreateBits( &updateMask, target );

	// this will cache automatically if needed
	_BuildValuesUpdate( data, &updateMask, target );

	// update count: 1 ;)
	return 1;
}

void Item::_SetCreateBits(UpdateMask *updateMask, Player *target) 
{
	for(uint32 i = 0; i < m_valuesCount; ++i)
		if(m_uint32Values[i] != 0)
			updateMask->SetBit(i);
}

void Item::_BuildMovementUpdate(ByteBuffer * data, uint16 flags, uint32 flags2, Player* target )
{
	*data << (uint16)flags;

	if (flags & 0x20)
	{
		*data << (uint32)0;
		*data << (uint16)0;
		*data << getMSTime(); // this appears to be time in ms but can be any thing. Maybe packet serializer ?
	}
	if (flags & 0x40)
	{
		*data << (float)0;
		*data << (float)0;
		*data << (float)0;
		*data << (float)0;
	}
	if (flags & 0x20)
	{
		*data << (uint32)0; //last fall time
		*data << 1.0f;
		*data << 2.0f;
		*data << 3.0f;
		*data << 4.0f;
		*data << 5.0f;
		*data << 6.0f;
		*data << 7.0f;
		*data << 8.0f;
		*data << 9.0f;
	}
}

void Item::_BuildValuesUpdate(ByteBuffer * data, UpdateMask *updateMask, Player* target)
{
	uint32 bc;
	uint32 values_count;
	uint8 *m = NULL;
	if( updateMask->GetBit(OBJECT_FIELD_GUID) == 0 )//it is an update not a create
	{
		if( target == GetOwner() )
		{
			bc = updateMask->GetUpdateBlockCount();
			values_count = min<uint32>( bc * 32, m_valuesCount );
		}
		else
		{
			m = (uint8*)sWorld.create_update_for_other_masks[ TYPEID_ITEM ]->GetMask();
			bc = updateMask->GetUpdateBlockCount( (uint32*)m );
			values_count = min<uint32>( bc * 32, m_valuesCount );
		}
	}
	else 
	{
		bc=updateMask->GetBlockCount();
		values_count=m_valuesCount;
	}

	if( bc == 0 )
	{
		//just send an empty mask. Shit happens i guess a lot of times( really lot of times ) on live servers
		*data << (uint8)1;	
		*data << (uint32)0;
	}
	else
	{
		*data << (uint8)bc;
		//this is probably create for self or just create for others ( in this case we made a temp update mask already )
		if( m == NULL )
		{
			data->append( updateMask->GetMask(), bc*4 );
			for( uint32 index = 0; index < values_count; index ++ )
				if( updateMask->GetBit( index ) )
					*data << m_uint32Values[ index ];
		}
		//this is update
		else
		{
			//add a mask that filters visibility bits only
			uint32 *m32 = (uint32 *)m;
			for( uint32 index = 0; index < bc; index ++ )
				*data << (updateMask->GetBlock( index ) & m32[ index ]);
			//only add values filtered by visibility bits
			for( uint32 index = 0; index < values_count; index ++ )
				if( updateMask->GetBit( index, m ) )
					*data << m_uint32Values[ index ];
		}
	}
}

#endif