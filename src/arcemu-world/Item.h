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

#ifndef WOWSERVER_ITEM_H
#define WOWSERVER_ITEM_H

#include "Skill.h"
#include "FeatureDefinitions.h"

enum RandomEnchantmentTypes
{
	RANDOMPROPERTY = 1,
	RANDOMSUFFIX   = 2
};

struct EnchantmentInstance
{
	EnchantEntry * Enchantment;
	bool BonusApplied;
	uint32 Slot;
	time_t ApplyTime;
	uint32 Duration;
	bool RemoveAtLogout;
	uint32 RandomSuffixPrefix;	//this is needed to be multiplied with random suffix factor
};

enum scalingstatmodtypes 
{
	SCALINGSTATSTAT,
	SCALINGSTATARMOR,
	SCALINGSTATDAMAGE,
	SCALINGSTATSPELLPOWER,

	SCALINGSTAT_NUM_TYPES	//just to see if new values poped up
};

const static ItemProf prof[22] = { {4, 2}, {4, 4}, {4, 8}, {4, 16}, {4, 64},
								   {2, 1}, {2, 2}, {2, 4}, {2, 8}, {2, 16}, {2, 32}, {2, 64}, {2, 128}, {2, 256}, {2, 1024}, {2, 8192}, {2, 32768}, {2, 65536}, {2, 131072},
								   {2, 262144}, {2, 524288}, {2, 1048576} };

const static uint32 arm_skills[7] = { 0,
									  SKILL_CLOTH,
									  SKILL_LEATHER,
									  SKILL_MAIL,
									  SKILL_PLATE_MAIL,
									  0,
									  SKILL_SHIELD };

const static uint32 weap_skills[21] = {	SKILL_AXES,
										SKILL_2H_AXES,
										SKILL_BOWS,
										SKILL_GUNS,
										SKILL_MACES,
										SKILL_2H_MACES,
										SKILL_POLEARMS,
										SKILL_SWORDS,
										SKILL_2H_SWORDS,
										0,
										SKILL_STAVES,
										0,
										0,
										SKILL_FIST_WEAPONS,
										0,//<-13
										SKILL_DAGGERS,
										SKILL_THROWN,
										0,	//old spears
										SKILL_CROSSBOWS,
										SKILL_WANDS,
										SKILL_FISHING };
                                      
const static float pricemod[9] = { 1.0f, // HATED
								   1.0f, // HOSTILE
								   1.0f, // UNFRIENDLY
								   1.0f, // NEUTRAL
								   0.95f, // FRIENDLY
								   0.90f, // HONORED
								   0.85f, // REVERED
								   0.80f }; // EXHALTED

const static float SuffixMods[NUM_INVENTORY_TYPES] = { 0.0f,
														0.46f,		// HEAD
														0.26f,		// NECK
														0.35f,		// SHOULDERS
														0.46f,		// BODY
														0.46f,		// CHEST
														0.35f,		// WAIST
														0.46f,		// LEGS
														0.34f,		// FEET
														0.26f,		// WRISTS
														0.35f,		// HANDS
														0.26f,		// FINGER
//														0.0f,		// TRINKET
														0.26f,		// TRINKET
														0.19f,		// WEAPON
														0.25f,		// SHEILD
														0.14f,		// RANGED
														0.26f,		// CLOAK
														0.46f,		// 2H-WEAPON
														0.0f,		// BAG
														0.0f,		// TABARD
														0.46f,		// ROBE
														0.19f,		// MAIN-HAND WEAPON
														0.19f,		// OFF-HAND WEAPON
														0.26f,		// HOLDABLE
														0.0f,		// AMMO
														0.26f,		// THROWN
														0.14f,		// RANGED
														0.0f,		// QUIVER
														0.26f, };	// RELIC

typedef std::map<uint32, EnchantmentInstance *> EnchantmentMap;

#define APPLY true
#define REMOVE false

//ex : 2803 * 264 / 1334.0f = 739992 / 1334 = 554 -> kinda good for avg max value
//#define RANDOM_SUFFIX_MAGIC_CALCULATION( __suffix, __scale ) int32( float( ( float( ( __suffix ) ) * float( ( __scale ) ) ) ) / 13340.0f );
#define RANDOM_SUFFIX_MAGIC_CALCULATION( __suffix, __scale ) int32( (__suffix * __scale + 9999) / 10000 )

class SERVER_DECL Item 
#ifndef SMALL_ITEM_OBJECT
	: public Object
#else
	: public EventableObject
#endif
{
public:
	Item();
	void Init( uint32 high, uint32 low );
	void Virtual_Constructor();		//when using object pool contructor is not good to be called again sometimes. Use this instead
	virtual ~Item();
	void Virtual_Destructor();		//this makes sure we do not leave events on objects that are supposed to be deleted
	int32 m_bufferPoolId;
#ifdef BUFFER_POOL_REMEMBER_HISTORY
	int	  m_bufferPoolPrevAllocLine;
	char  *m_bufferPoolPrevAllocFILE;
	int	  m_bufferPoolPrevDeAllocLine;
	char  *m_bufferPoolPrevDeAllocFILE;
#endif
	void Create( uint32 itemid, Player* owner );

	ARCEMU_INLINE ItemPrototype* GetProto() 
	{ 
		return m_itemProto; 
	}
	ARCEMU_INLINE void SetProto( ItemPrototype* pr ) 
	{ 
		m_itemProto = pr; 
	}

	ARCEMU_INLINE Player* GetOwner() 
	{ 
		return m_owner; 
	}
	void SetOwner( Player* owner );

	//! DB Serialization
	void LoadFromDB( Field *fields, Player* plr, bool light );
	void SaveToDB( int16 containerslot, int16 slot, bool firstsave, QueryBuffer* buf );
	bool LoadAuctionItemFromDB( uint64 guid );
	void DeleteFromDB();
	void DeleteMe();
	
	ARCEMU_INLINE void SoulBind()
	{
		this->SetFlag( ITEM_FIELD_FLAGS, ITEM_FLAG_SOULBOUND );
	}

	ARCEMU_INLINE bool IsSoulbound()
	{
		return this->HasFlag( ITEM_FIELD_FLAGS, ITEM_FLAG_GROUP | ITEM_FLAG_SOULBOUND );
	}

	ARCEMU_INLINE void AccountBind()
	{
		this->SetFlag( ITEM_FIELD_FLAGS, ITEM_FLAG_ACCOUNTBOUND );
	}

	ARCEMU_INLINE bool IsAccountbound()
	{
		return this->HasFlag( ITEM_FIELD_FLAGS, ITEM_FLAG_ACCOUNTBOUND );  // 134217728 = 0x8000000
	}

	ARCEMU_INLINE uint32 GetChargesLeft()
	{
		for( uint32 x = 0; x < 5; x++ )
			if( m_itemProto->Spells.Id[x] )
				return GetUInt32Value( ITEM_FIELD_SPELL_CHARGES + x );
		return 0;
	}

	ARCEMU_INLINE time_t GetEnchantmentApplytime( uint32 slot )
	{
		EnchantmentMap::iterator itr = Enchantments.find( slot );
		if( itr == Enchantments.end() )
			return 0;
		else
			return itr->second->ApplyTime;
	}

	//! Adds an enchantment to the item.
	int32 AddEnchantment( EnchantEntry* Enchantment, uint32 Duration, bool Perm = false, bool apply = true, bool RemoveAtLogout = false, uint32 Slot_ = 0, uint32 RandomSuffixPrefix = 0 );
	uint32 GetSocketsCount( uint32 &ItemSocketCount, uint32 &EnchantSocketCount );

	//! Removes an enchantment from the item.
	void RemoveEnchantment( uint32 EnchantmentSlot );
	
	// Removes related temporary enchants
	void RemoveRelatedEnchants( EnchantEntry* newEnchant );

	//! Adds the bonus on an enchanted item.
	void ApplyEnchantmentBonus( uint32 Slot, bool Apply );

	//! Applies all enchantment bonuses (use on equip)
	void ApplyEnchantmentBonuses();

	//! Removes all enchantment bonuses (use on dequip)
	void RemoveEnchantmentBonuses();

	//! Event to remove an enchantment.
	void EventRemoveEnchantment( uint32 Slot );

	//! Check if we have an enchantment of this id?
	int32 HasEnchantment( uint32 Id );

	//! Check if we have an enchantment on that slot
	bool HasEnchantmentOnSlot( uint32 slot );

	//! Modify the time of an existing enchantment.
	void ModifyEnchantmentTime( uint32 Slot, uint32 Duration );

	//! Find free enchantment slot.
	int32 FindFreeEnchantSlot( EnchantEntry* Enchantment, uint32 random_type );

	//! Removes all enchantments.
	void RemoveAllEnchantments( bool OnlyTemporary );

	//! Sends SMSG_ITEM_UPDATE_ENCHANT_TIME
	void SendEnchantTimeUpdate( uint32 Slot, uint32 Duration );

	//! Applies any random properties the item has.
	void ApplyRandomProperties( bool apply );

	// converts one stat into another based on some factor
	void ApplyReforging( bool apply );
	void SetReforgeID( uint32 ID );
	uint32 GetReforgeID( ){ return GetUInt32Value( ITEM_FIELD_ENCHANTMENT_1_1 + REFORGE_ENCHANTMENT_SLOT * 3 ); };

	void RemoveProfessionEnchant(int32 slot=-1);
	void RemoveSocketBonusEnchant();

	// gets the itemlink for a message to the player
	string GetItemLink(uint32 language);

	ARCEMU_INLINE void SetCount( uint32 amt ) { SetUInt32Value( ITEM_FIELD_STACK_COUNT, amt ); }
	ARCEMU_INLINE void SetDurability( uint32 Value ) { SetUInt32Value(ITEM_FIELD_DURABILITY, Value ); };
	ARCEMU_INLINE void SetDurabilityToMax() { SetUInt32Value( ITEM_FIELD_DURABILITY, GetUInt32Value( ITEM_FIELD_MAXDURABILITY ) ); }
	ARCEMU_INLINE uint32 GetDurability() { return GetUInt32Value( ITEM_FIELD_DURABILITY ); }
	ARCEMU_INLINE uint32 GetDurabilityMax() { return GetUInt32Value( ITEM_FIELD_MAXDURABILITY ); }
	
    ARCEMU_INLINE void SetStackCount( uint32 amt ) { SetUInt32Value( ITEM_FIELD_STACK_COUNT,  amt ); }
    ARCEMU_INLINE uint32 GetStackCount(){ return GetUInt32Value( ITEM_FIELD_STACK_COUNT ); }
    ARCEMU_INLINE void ModStackCount( int32 val ){ ModUnsigned32Value( ITEM_FIELD_STACK_COUNT, val ); }
    
    ARCEMU_INLINE void SetDuration( uint32 durationseconds ){ SetUInt32Value( ITEM_FIELD_DURATION, durationseconds ); }
    ARCEMU_INLINE uint32 GetDuration(){ return GetUInt32Value( ITEM_FIELD_DURATION ); }
    
    ARCEMU_INLINE void SetCharges( uint32 index, uint32 charges ){ SetUInt32Value( ITEM_FIELD_SPELL_CHARGES + index, charges ); }
    ARCEMU_INLINE void ModCharges( uint32 index, int32 val ){ ModUnsigned32Value( ITEM_FIELD_SPELL_CHARGES + index, val ); }
    ARCEMU_INLINE uint32 GetCharges( uint32 index ){ return GetUInt32Value( ITEM_FIELD_SPELL_CHARGES + index ); } 

	ARCEMU_INLINE bool IsAmmoBag() 
	{ 
		return (m_itemProto->Class == ITEM_CLASS_QUIVER); 
	}

	void RemoveFromWorld();

	Loot* loot;
	bool locked;
	bool m_isDirty;
	bool stats_applied;

	EnchantmentInstance* GetEnchantment( uint32 slot );
	bool IsGemRelated( EnchantEntry* Enchantment );

	ARCEMU_INLINE uint32 GetItemRandomPropertyId() const { return m_uint32Values[ITEM_FIELD_RANDOM_PROPERTIES_ID]; }
	ARCEMU_INLINE uint32 GetItemRandomSuffixFactor() { return m_uint32Values[ITEM_FIELD_PROPERTY_SEED]; }
	static uint32 GenerateRandomSuffixFactor( ItemPrototype* m_itemProto );

	ARCEMU_INLINE void SetRandomProperty( uint32 id )
	{
		SetUInt32Value( ITEM_FIELD_RANDOM_PROPERTIES_ID, id );
		random_prop = id;
	}

	ARCEMU_INLINE void SetRandomSuffix( uint32 id )
	{
		int32 r_id = -(int32(id));
		SetInt32Value( ITEM_FIELD_RANDOM_PROPERTIES_ID, r_id );
		if( r_id != 0 )
		{
			uint32 v = Item::GenerateRandomSuffixFactor( m_itemProto );
			SetUInt32Value( ITEM_FIELD_PROPERTY_SEED, v );
		}
		random_suffix = id;
	}

	bool HasEnchantments() { return Enchantments.empty() == false; }

	uint32 GetEnchantCountOfCustomGroup( uint32 custom_group );//some gems can have effect applied only X times

	uint32 wrapped_item_id;

	int32 event_GetInstanceID();

	void AddTemporalSpellBookSpell(uint32 new_spell);		//item proto does not have this spell
	void RemoveTemporalSpellBookSpell(uint32 new_spell);		//item proto does not have this spell
	bool HasTemporalSpellBookSpell(uint32 spellId);
	list<uint32>	enchant_spell_book;	//some enchantments add spells on use.
	void SendItemDuration();
	EnchantmentMap Enchantments;

	void Randomize();	//RandomSuffixId + RandomPropId
#ifdef SMALL_ITEM_OBJECT
	void __fastcall SetFlag( const uint32 index, uint32 newFlag );
	void __fastcall RemoveFlag( const uint32 index, uint32 oldFlag );

	ARCEMU_INLINE bool HasFlag( const uint32 index, uint32 flag ) const
	{
		ASSERT( index < m_valuesCount );
		return (m_uint32Values[ index ] & flag) != 0;
	}
	void __fastcall SetUInt32Value( const uint32 index, const uint32 value );
	void __fastcall SetInt32Value( const uint32 index, const int32 value );
	void __fastcall SetUInt64Value( const uint32 index, const uint64 value );
	void __fastcall SetFloatValue( const uint32 index, const float value );
	ARCEMU_INLINE const uint32& GetUInt32Value( uint32 index ) const
	{
		ASSERT( index < m_valuesCount );
		return m_uint32Values[ index ];
	}
	ARCEMU_INLINE const int32& GetInt32Value( uint32 index ) const
	{
		ASSERT( index < m_valuesCount );
		return m_int32Values[ index ];
	}
	ARCEMU_INLINE const uint64& GetUInt64Value( uint32 index ) const
	{
		ASSERT( index + uint32(1) < m_valuesCount );
		return *((uint64*)&(m_uint32Values[ index ]));
	}
	void ModUnsigned32Value(uint32 index, int32 mod);
	void ModSignedInt32Value(uint32 index, int32 value);
	
	////////////////////////////////////////
	void ClearUpdateMask( )
	{
		m_updateMask.Clear();
		m_objectUpdated = false;
	}
	UpdateMask m_updateMask;
	bool m_objectUpdated;
	uint16 m_valuesCount;
	union {
		uint32* m_uint32Values;
		int32*	m_int32Values;
		float*	m_floatValues;
	};
	uint8 m_objectTypeId;
	uint8 internal_object_type;
	ARCEMU_INLINE bool IsContainer() const { return ( m_objectTypeId == TYPEID_CONTAINER); }
	ARCEMU_INLINE const uint64& GetGUID() const { return *((uint64*)&m_uint32Values[ OBJECT_FIELD_GUID ] ); }
	ARCEMU_INLINE const WoWGuid& GetNewGUID() const { return m_wowGuid; }
	WoWGuid m_wowGuid;
	ARCEMU_INLINE uint32 GetEntry(){return m_uint32Values[ OBJECT_FIELD_ENTRY ];}
	ARCEMU_INLINE bool IsItem() const { return true; }
	uint32 GetZoneId();
	uint32 GetMapId();
	ARCEMU_INLINE bool IsInWorld() { return m_mapMgr != NULL; }
	MapMgr *m_mapMgr;
	ARCEMU_INLINE const uint32 GetLowGUID() const { return (m_uint32Values[ OBJECT_FIELD_GUID ]); }
	void PushToWorld(MapMgr*mgr);
	void DestroyForPlayer(Player *target, uint8 anim = 0);
	uint32 m_instanceId;
	uint32 BuildCreateUpdateBlockForPlayer(ByteBuffer *data, Player *target);
	void _BuildMovementUpdate(ByteBuffer * data, uint16 flags, uint32 flags2, Player* target );
	void _BuildValuesUpdate(ByteBuffer * data, UpdateMask *updateMask, Player* target);
	void _SetCreateBits(UpdateMask *updateMask, Player *target);
#endif
protected:

	ItemPrototype* m_itemProto;
	uint32 _fields[ITEM_END];//this mem is wasted in case of container... but this will be fixed in future
	Player* m_owner; // let's not bother the manager with unneeded requests
	uint32 random_prop;
	uint32 random_suffix;
};

uint32 GetSkillByProto( uint32, uint32 );

uint32 GetSellPriceForItem( ItemPrototype* proto, uint32 count );
uint32 GetBuyPriceForItem( ItemPrototype* proto, uint32 count, Player* plr, Creature* vendor );

uint32 GetSellPriceForItem( uint32 itemid, uint32 count);
uint32 GetBuyPriceForItem( uint32 itemid, uint32 count, Player* plr, Creature* vendor );

string GetItemLinkByProto(ItemPrototype * iProto, uint32 language);
//!!buffer len should be large enough ! at least 250 chars ?
SERVER_DECL void GetItemLinkByProto(ItemPrototype * iProto, char *buffer );

uint32 ScalingStatGetStatMultiplier( ScalingStatValuesEntry *ssv, uint32 inventoryType);
uint32 ScalingStatGetArmor( ScalingStatValuesEntry *ssv, uint32 inventoryType, uint32 armorType );
uint32 ScalingStatGetDPSAndDamageMultiplier( ScalingStatValuesEntry *ssv, uint32 subClass, bool isCasterWeapon, float *damageMultiplier );

void RandomizeEPLItem( Item *it, float ChanceModPos = 0.0f, float ChanceModNeg = 0.0f );

#endif
