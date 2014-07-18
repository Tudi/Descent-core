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

#ifndef _PET_H
#define _PET_H

//safe way to handle arrays in debug mode
#include <array>

#define PET_SPELL_SPAM_COOLDOWN 2000 //applied only to spells that have no cooldown
#define PET_MAX_FAMILY_COUNT 60

/* Taken from ItemPetFood.dbc
 * Each value is equal to a flag
 * so 1 << PET_FOOD_BREAD for example
 * will result in a usable value.
 */
enum PET_FOOD
{
	PET_FOOD_NONE,
	PET_FOOD_MEAT,
	PET_FOOD_FISH,
	PET_FOOD_CHEESE,
	PET_FOOD_BREAD,
	PET_FOOD_FUNGUS,
	PET_FOOD_FRUIT,
	PET_FOOD_RAW_MEAT, // not used in pet diet
	PET_FOOD_RAW_FISH  // not used in pet diet
};

enum PetTalentType
{
    PET_TALENT_TYPE_NOT_HUNTER_PET = -1,
    PET_TALENT_TYPE_FEROCITY       = 0,
    PET_TALENT_TYPE_TENACITY       = 1,
    PET_TALENT_TYPE_CUNNING        = 2
};

//#define PET_HAPPINESS_UPDATE_VALUE 333000
//#define PET_HAPPINESS_UPDATE_TIMER 7500

#define PET_ACTION_NOTDEF   0x00
#define PET_ACTION_ACTION   0x07
#define PET_ACTION_STATE	0x06

//TODO: grep see the way pet spells contain the same flag?
#define PET_ACTION_SPELL	0xC1
#define PET_ACTION_SPELL_1  0x81
#define PET_ACTION_SPELL_2  0x01

#define DEFAULT_SPELL_STATE 0x81
#define PASSIVE_SPELL_STATE 0x01
#define AUTOCAST_SPELL_STATE 0xC1 

#define PET_ACTION_SPELL_IS_TALENT	0x20	//big hack to not add additional table :P. !!! look at DEFAULT_SPELL_STATE | PASSIVE_SPELL_STATE.... can't have these flags

#define Get_pet_number_from_guid(x) (( x & 0xFF000000 ) >> 24 )

enum PET_ACTION
{
	PET_ACTION_STAY,
	PET_ACTION_FOLLOW,
	PET_ACTION_ATTACK,
	PET_ACTION_DISMISS,
	PET_ACTION_GO_TO_POSITION,
//	PET_ACTION_CASTING,
};

enum PET_STATE
{
	PET_STATE_PASSIVE,
	PET_STATE_DEFENSIVE,
	PET_STATE_AGGRESSIVE
};

enum PetActionFeedback
{
	PET_FEEDBACK_NONE,
	PET_FEEDBACK_PET_DEAD,
	PET_FEEDBACK_NOTHING_TO_ATTACK,
	PET_FEEDBACK_CANT_ATTACK_TARGET
};

enum PET_SPELL
{
	PET_SPELL_PASSIVE = 0x06000000,
	PET_SPELL_DEFENSIVE,
	PET_SPELL_AGRESSIVE,
	PET_SPELL_STAY = 0x07000000,
	PET_SPELL_FOLLOW,
	PET_SPELL_ATTACK,
	PET_SPELL_ASSIST,
	PET_SPELL_GOTO = 0x07000004,
};

#define MAX_STABLE_SLOTS ( 20 + 5 ) // firt 5 are active pets
/*
enum StableState
{
	STABLE_STATE_ACTIVE		= 1,
	STABLE_STATE_PASSIVE	= 2
};
/*
enum HappinessState
{
	UNHAPPY		= 1,
	CONTENT		= 2,
	HAPPY		= 3
}; */

enum AutoCastEvents
{
	AUTOCAST_EVENT_NOT_AUTOCAST			,
	AUTOCAST_EVENT_ATTACK				,
	AUTOCAST_EVENT_ON_SPAWN				,
	AUTOCAST_EVENT_OWNER_ATTACKED		,
	AUTOCAST_EVENT_LEAVE_COMBAT			,
	AUTOCAST_EVENT_ENTER_COMBAT			,
	AUTOCAST_EVENT_COUNT				,
	AUTOCAST_EVENT_NONE					,
};

#define AUTOCAST_EVENT_LIST_MAXLEN		20
#define PET_ACTIONBAR_MAXSIZE			10

#define PET_DELAYED_REMOVAL_TIME 60000  // 1 min

typedef map<SpellEntry*, uint8> PetSpellMap;
struct PlayerPet;

class SERVER_DECL Pet : public Creature
{
	friend class Player;
	friend class Creature;
	friend class WorldSession;

public:
	Pet(uint64 guid);
	~Pet();
	virtual void Virtual_Destructor();

	void LoadFromDB(Player* owner, PlayerPet * pi);
	void CreateAsSummon(uint32 entry, CreatureInfo *ci, Creature *created_from_creature, Player* owner, SpellEntry *created_by_spell, uint32 type, uint32 expiretime, LocationVector* Vec = NULL);

	virtual void Update(uint32 time);
	void OnPushToWorld();

	ARCEMU_INLINE uint32 GetXP(void) 
	{
		INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
		return m_PetXP; 
	}

	void InitializeSpells();
	void ReInitializeSpells();
	void InitializeMe(bool first, PlayerPet * pi=NULL);
	void SendSpellsToOwner();
	void SendNullSpellsToOwner();
	void SendCastFailed( uint32 spellid, uint8 fail );
	void SendActionFeedback( PetActionFeedback value  );

	ARCEMU_INLINE void SetPetAction(uint32 act) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
		m_Action = act; 
	}
	ARCEMU_INLINE uint32 GetPetAction(void) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
		return m_Action; 
	}

	ARCEMU_INLINE void SetPetState(uint32 state);
	ARCEMU_INLINE uint32 GetPetState(void) { return m_State; }

	ARCEMU_INLINE void SetPetDiet(uint32 diet) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
		m_Diet = diet; 
	}
	ARCEMU_INLINE void SetPetDiet() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
		if( myFamily )
			m_Diet = myFamily->petdietflags; 
		else
			m_Diet = 0;
	}
	ARCEMU_INLINE uint32 GetPetDiet(void) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
		return m_Diet; 
	}
	
	void UpdatePetInfo(bool bSetToOffline);
	void Dismiss( bool bUpdate = true, bool bSetOffline = false, bool AutoResummon = false );
	void Abandon();

	void DelayedRemove(bool bTime, bool bDeath);

	ARCEMU_INLINE Player* GetPetOwner() { return m_Owner; }
	ARCEMU_INLINE void ClearPetOwner() { m_Owner = NULL; }
	bool CanGainXP();
	void GiveXP( uint32 xp );
	uint32 GetNextLevelXP(uint32 currentlevel);
	void ApplyStatsForLevel();
	void ApplySummonLevelAbilities();
	void ApplyPetLevelAbilities();
	void UpdateAP();
	void UpdateSP();
	void UpdateHaste();
	void PetSafeDelete();
	void LoadPetAuras(int32 id);
	void SetDefaultSpells();
	void SetDefaultActionbar();
	void SetActionBarSlot(uint32 slot, uint32 spell)
	{ 
		INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
		if( slot < PET_ACTIONBAR_MAXSIZE )
			ActionBar[ slot ] = spell; 
	}

	void LoadSpells();
	void AddSpell(SpellEntry * sp, bool learning,bool refreshclient=true);
	void RemoveSpell(SpellEntry * sp);
	void WipeTalent(SpellEntry * sp,uint32 loop_count);
	void WipeTalents();
	void fill_talent_info_in_packet( WorldPacket &data );
	void LearnTalent(uint32 talent_id, uint32 requested_rank, bool isPreviewed );
	void AddPetTalent(uint32 talent_entry, uint8 rank);
	uint32 GetUntrainCost();
	void SetSpellState(SpellEntry * sp, uint8 State);
	uint8 GetSpellState(SpellEntry * sp);
	bool HasSpell( uint32 SpellID )
	{ 
		INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
		SpellEntry * sp = dbcSpell.LookupEntry( SpellID );
		if( sp )
			return mSpells.find( sp ) != mSpells.end();
		return false;
	}
	ARCEMU_INLINE void RemoveSpell( uint32 SpellID )
	{
		INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
		SpellEntry * sp = dbcSpell.LookupEntry( SpellID );
		if( sp ) RemoveSpell( sp );
	}
	ARCEMU_INLINE void SetSpellState( uint32 SpellID, uint8 State )
	{
		INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
		SpellEntry * sp = dbcSpell.LookupEntry( SpellID );
		if( sp ) SetSpellState(sp, State);
	}
	ARCEMU_INLINE uint8 GetSpellState( uint32 SpellID )
	{
		INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
		if( SpellID == 0 )
			return DEFAULT_SPELL_STATE;

		SpellEntry * sp = dbcSpell.LookupEntry( SpellID );
		if( sp )
			return GetSpellState( sp );
		return DEFAULT_SPELL_STATE;
	}
	
	AI_Spell * CreateAISpell(SpellEntry * info);
	ARCEMU_INLINE PetSpellMap* GetSpells() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
		return &mSpells; 
	}
//	ARCEMU_INLINE bool IsSummon() 
//	{ 
//		INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
//		return Summon;
//	}

	void Rename(string NewName);
	ARCEMU_INLINE string* GetName() 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
		return &m_name; 
	}
	uint32 CanLearnSpell( SpellEntry* sp );
	void SkillUp(bool remove_lower_levels=true);
	
	// talents
	ARCEMU_INLINE uint8 GetTalentPointsForLevel( uint32 level ) 
	{ 
		INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
		return ( level >= 20 ) ? uint8( level - 16 ) >> 2 : 0; 
	}	// pet gain first talent point at lvl 20, then every 4 lvls another point
	void SetTalentPoints( int8 TP );
	void DecreaseTalentPoints();
	int8 GetUnspentTalentPoints();
	uint8 GetSpentTalentPoints();
	
	void HandleAutoCastEvent( AutoCastEvents Type );
	AI_Spell * HandleAutoCastEvent();
	void SetPetSpellState(uint32 spell, uint16 state);
//	float GetHappinessDmgMod() { return 0.25f * GetHappinessState() + 0.5f; };

	virtual Group *GetGroup();
	void SetPetNumber(uint8 pet_number);	//!! call this befor adding pet to world
	uint8 GetPetNumber() { return GetUInt32Value( UNIT_FIELD_PETNUMBER ); }
	void SetExpire( uint32 NewRemainingTime ) { m_ExpireTime = NewRemainingTime; }
	ARCEMU_INLINE AI_Spell* GetAISpellForSpellId(uint32 spellid);
	void RegenerateMana();
	uint32	OwnerStamContribPCT;
	uint32	OwnerAPContribPCT;
	uint32	OwnerSPContribPCT;
	void	Cooldown_Add( SpellEntry *sp );
	int32	CooldownCanCast( SpellEntry *sp );
	void	ClearSpellCooldowns();
	int32	AuraMeleeHasteSum;
protected:
	void SetAutoCast(AI_Spell * sp, bool on);

//	uint32	m_object_shield_for_prev_layer[200];	//only for debug purpuse to make sure creature object is not writing on us and corrupting our lists
	Player *m_Owner;
	uint32 m_PetXP;
	PetSpellMap mSpells;
	PlayerPet * mPi;
	//uint32 ActionBar[10];   // 10 slots
	tr1::array< uint32, PET_ACTIONBAR_MAXSIZE> ActionBar;
	
//	std::map<uint32, AI_Spell*> m_AISpellStore;			//this was nice and secsy but crashing. No idea why so i remake it :(
	SimplePointerList< AI_Spell >	m_AISpellStore2;

	uint32 m_AutoCombatSpell;

//	uint32 m_HappinessTimer;
	uint32 m_Action;
	uint32 m_State;
	uint32 m_ExpireTime;
	uint32 m_Diet;
	uint64 m_OwnerGuid;
	time_t reset_time;
	uint32 reset_cost;
	bool bSaveToDB;	//not sure if we need to save all summons into DB
	bool Summon;	//created by a spell not through taming
	string m_name;
//	HappinessState GetHappinessState();
	void SetNameForEntry( uint32 entry );

	//list<AI_Spell*> m_autoCastSpells[AUTOCAST_EVENT_COUNT];
	//tr1::array< list<AI_Spell*>, AUTOCAST_EVENT_COUNT > m_autoCastSpells;
	AI_Spell *m_autoCastSpells[AUTOCAST_EVENT_COUNT][AUTOCAST_EVENT_LIST_MAXLEN];

	//3.1.x talent changes
	std::map<uint32, uint8> m_talents;		//spells may belong to multiple talents, need to have this
	tr1::array< int32, 7> owner_inherited_SP;
};

#endif
