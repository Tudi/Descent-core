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

#include "StdAfx.h"

#define GROWL_RANK_1	2649
#define GROWL_RANK_2	14916
#define COWER_RANK_1	1742

#define WATER_ELEMENTAL	510
#define PET_IMP			416
#define PET_VOIDWALKER	1860
#define PET_SUCCUBUS	1863
#define PET_FELHUNTER	417
#define PET_FELGUARD	17252
#define SHADOWFIEND		19668
#define SPIRITWOLF		29264
#define DANCINGRUNEWEAPON 27893
AutoCastEvents GetAutoCastTypeForSpell( SpellEntry * ent )
{
	switch( ent->NameHash )
	{
	/************************************************************************/
	/* Warlock Pet Spells													*/
	/************************************************************************/
	case SPELL_HASH_BLOOD_PACT:			// Blood Pact
	case SPELL_HASH_AVOIDANCE:
	case SPELL_HASH_PARANOIA:
	case SPELL_HASH_LESSER_INVISIBILITY:
	case SPELL_HASH_QIRAJI_FORTITUDE:
	case SPELL_HASH_FEL_INTELLIGENCE:
	case SPELL_HASH_EMBRACE_OF_THE_SHALE_SPIDER:
		return AUTOCAST_EVENT_ON_SPAWN;
		break;

	case SPELL_HASH_FIRE_SHIELD:		// Fire Shield
		return AUTOCAST_EVENT_OWNER_ATTACKED;
		break;
		
	case SPELL_HASH_PHASE_SHIFT:		// Phase Shift
	case SPELL_HASH_CONSUME_SHADOWS:
//	case SPELL_HASH_LESSER_INVISIBILITY:
		return AUTOCAST_EVENT_LEAVE_COMBAT;
	case SPELL_HASH_SACRIFICE:
		return AUTOCAST_EVENT_NONE;
	case SPELL_HASH_SUFFERING:	
	case SPELL_HASH_FIREBOLT:	
		return AUTOCAST_EVENT_ATTACK;
		break;

	/************************************************************************/
	/* Hunter Pet Spells													*/
	/************************************************************************/
	case SPELL_HASH_PROWL:				// Prowl
		return AUTOCAST_EVENT_LEAVE_COMBAT;
	case SPELL_HASH_GROWL:				// growl
	case SPELL_HASH_THUNDERSTOMP:		// Thunderstomp
	case SPELL_HASH_SHELL_SHIELD:		// Shell Shield
	case SPELL_HASH_BITE:				// bite
	case SPELL_HASH_CLAW:				// claw
	case SPELL_HASH_SMACK:				// smack
		return AUTOCAST_EVENT_ATTACK;
	case SPELL_HASH_DASH:				// Dash
	case SPELL_HASH_CHARGE:				// Charge
	case SPELL_HASH_FURIOUS_HOWL:		// Furious Howl
	case SPELL_HASH_DIVE:				// Dive 
	case SPELL_HASH_SPIRIT_WALK:		
		return AUTOCAST_EVENT_ENTER_COMBAT;
		break;

	/************************************************************************/
	/* Mage Pet Spells														*/
	/************************************************************************/
	case SPELL_HASH_WATERBOLT:			// Waterbolt
		return AUTOCAST_EVENT_ATTACK;
		break;

	/************************************************************************/
	/* Shaman Pet Spells													*/
	/************************************************************************/
	case SPELL_HASH_SPIRIT_HUNT:
		return AUTOCAST_EVENT_ON_SPAWN;
		break;

	case SPELL_HASH_TWIN_HOWL:
	case SPELL_HASH_BASH:
		return AUTOCAST_EVENT_ATTACK;
		break;
	case SPELL_HASH_HEART_OF_THE_PHOENIX:
		return AUTOCAST_EVENT_NOT_AUTOCAST;
		break;

	}
	return AUTOCAST_EVENT_ATTACK;
}

void Pet::SetNameForEntry( uint32 entry )
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	switch( entry )
	{
		case WATER_ELEMENTAL:
			m_name = "Water Elemental";
			break;
		case SHADOWFIEND:
			m_name = "Shadowfiend";
			break;
		case SPIRITWOLF:
			m_name = "Spirit Wolf";
			break;
		case DANCINGRUNEWEAPON:
			m_name = "Rune Weapon";
			break;
		case PET_IMP:
		case PET_VOIDWALKER:
		case PET_SUCCUBUS:
		case PET_FELHUNTER:
		case PET_FELGUARD:
			{
				PlayerPet *pet = m_Owner->GetPlayerPet( -1, entry );
				if( pet )
				{
					m_name = pet->name;
				}
				else // no name found, generate one and save it
				{
					m_name = sWorld.GenerateName();
//					CharacterDatabase.Execute("INSERT INTO playersummons VALUES(%u, %u, '%s')",
//						m_Owner->GetLowGUID(), entry, CharacterDatabase.EscapeString( m_name ).c_str() );
				}
			}break;
		default: m_name = sWorld.GenerateName();
	}
}

void Pet::CreateAsSummon( uint32 entry, CreatureInfo *ci, Creature* created_from_creature, Player* owner, SpellEntry* created_by_spell, uint32 type, uint32 expiretime, LocationVector* Vec )
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	if( !ci || !owner )
	{
		sEventMgr.AddEvent( this, &Pet::PetSafeDelete, EVENT_CREATURE_SAFE_DELETE, 1, 1, EVENT_FLAG_DELETES_OBJECT | EVENT_FLAG_UNIQUE_EVENT_DELETE_NEWER );
		return;
	}

	if( owner->GetSummon() != NULL )
		owner->GetSummon()->Dismiss(); // to avoid problems caused by loosing reference to old pet

	bool LoadFromDb = ( type & 0x8 ) != 0;
	m_Owner = owner;
	m_OwnerGuid = m_Owner->GetGUID();
	m_phase = m_Owner->GetPhase();
	creature_info = ci;
	proto = CreatureProtoStorage.LookupEntry( GetEntry() );
	myFamily = dbcCreatureFamily.LookupEntryForced( ci->Family );

	switch( GetEntry() )
	{
		case 29264: //feral spirit
		{
			OwnerAPContribPCT = 30;
			if( GetPetOwner()->HasGlyphWithID( GLYPH_SHAMAN_FERAL_SPIRIT ) )
				OwnerAPContribPCT =  60;
		}break;
		default:
			break;
	};

	float x, y, z;
	if( Vec )
	{
		x = Vec->x;
		y = Vec->y;
		z = Vec->z;
	}
	else
	{
		x = owner->GetPositionX() + 2;
		y = owner->GetPositionY() + 2;
		z = owner->GetPositionZ();
	}

	// Create ourself	
	Create( m_name.c_str(), owner->GetMapId(), x, y, z, owner->GetOrientation() );
	
	// Hunter pet should be max 5 levels below owner
	uint32 level = owner->GetUInt32Value( UNIT_FIELD_LEVEL );
	if( ( type & 0x2 ) && created_from_creature != NULL )
//		level = created_from_creature->getLevel() < ( level - 5 ) ? level - 5 : created_from_creature->getLevel();
		level = owner->getLevel();

	SetUInt32Value( OBJECT_FIELD_ENTRY, entry );
	SetUInt32Value( UNIT_FIELD_LEVEL, level );
	SetUInt32Value( UNIT_FIELD_DISPLAYID, ci->Male_DisplayID );
	SetUInt32Value( UNIT_FIELD_NATIVEDISPLAYID, ci->Male_DisplayID );
	EventModelChange();
	SetUInt64Value( UNIT_FIELD_SUMMONEDBY, owner->GetGUID() );
	SetUInt64Value( UNIT_FIELD_CREATEDBY, owner->GetGUID() );
//	SetUInt32Value( UNIT_FIELD_BYTES_0, (owner->GetUInt32Value( UNIT_FIELD_BYTES_0 ) & 0x00FFFFFF) | (0 << 24) );
	SetUInt32Value( UNIT_FIELD_BASEATTACKTIME, 2000 );
	SetUInt32Value( UNIT_FIELD_BASEATTACKTIME_1, 2000 );
	SetUInt32Value( UNIT_FIELD_FACTIONTEMPLATE, owner->GetUInt32Value( UNIT_FIELD_FACTIONTEMPLATE ) );
	SetFloatValue( UNIT_MOD_CAST_SPEED, 1.0f );	// better set this one
	SetUInt32Value( UNIT_FIELD_FLAGS_2, UNIT_FLAG2_ENABLE_POWER_REGEN );

	if( type & 0x1 
//		|| created_from_creature == NULL	//but why ?
		)
	{
		Summon = true;
		SetNameForEntry( entry );
		if( created_by_spell != NULL )
			SetUInt64Value( UNIT_CREATED_BY_SPELL, created_by_spell->Id );

		SetUInt32Value( UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED );
//		SetUInt32Value( UNIT_FIELD_BYTES_2, (0x01 | (0x28 << 8) | (0x2 << 24) ) ); //totem had just 1
//		SetUInt32Value( UNIT_FIELD_BYTES_2, U_FIELD_BYTES_ANIMATION_FROZEN ); //totem had just 1
//		SetUInt32Value( UNIT_FIELD_FLAGS_2, UNIT_FLAG2_ENABLE_POWER_REGEN );
		SetByte( UNIT_FIELD_BYTES_2, 2, UNIT_BYTE2_FLAG_RENAME );
		SetFloatValue( UNIT_FIELD_BOUNDINGRADIUS, 0.5f );
		SetFloatValue( UNIT_FIELD_COMBATREACH, 0.75f );
		if( myFamily != NULL && myFamily->skilline == SKILL_PET_GHOUL )
		{
			PowerFields[ POWER_TYPE_ENERGY ] = 0;	//mana is stored in slot 0
			SetPowerType( POWER_TYPE_ENERGY );
			SetPower( POWER_TYPE_ENERGY, 100 );
			SetMaxPower( POWER_TYPE_ENERGY, 100 );
		}
		else
		{
			PowerFields[ POWER_TYPE_MANA ] = 0;	//mana is stored in slot 0
			SetPowerType( POWER_TYPE_MANA );
		}
	}
	else // Hunter pet
	{
		if( myFamily == NULL || myFamily->name == NULL )
			m_name = "Pet";
		else
			m_name = myFamily->name ;

		SetFloatValue( UNIT_FIELD_BOUNDINGRADIUS, proto->BoundingRadius );
		SetFloatValue( UNIT_FIELD_COMBATREACH, proto->CombatReach );

//		SetUInt32Value( UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED | UNIT_FLAG_COMBAT ); // why combat ??
		SetUInt32Value( UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED ); // why combat ??
//		SetUInt32Value( UNIT_FIELD_FLAGS_2, UNIT_FLAG2_ENABLE_POWER_REGEN );
//		SetPower( POWER_TYPE_HAPPINESS, PET_HAPPINESS_UPDATE_VALUE >> 1 );	//happiness
//		SetMaxPower( POWER_TYPE_HAPPINESS, 1000000 );
		SetUInt32Value( UNIT_FIELD_PETEXPERIENCE, 0 );
		SetUInt32Value( UNIT_FIELD_PETNEXTLEVELEXP, GetNextLevelXP( level ) );
		PowerFields[ POWER_TYPE_FOCUS ] = 0;	//mana is stored in slot 0
		SetPowerType( POWER_TYPE_FOCUS );
		SetPower( POWER_TYPE_FOCUS, 100 );// Focus
		SetMaxPower( POWER_TYPE_FOCUS, 100 );
//		SetUInt32Value( UNIT_FIELD_BYTES_2, 1 | (0x28 << 8) | (0x3 << 16) );// 0x3 -> Enable pet rename.
//		SetUInt32Value( UNIT_FIELD_BYTES_2, ((UNIT_BYTE2_FLAG_RENAME|UNIT_BYTE2_FLAG_PET_DETAILS) << 16) );// 0x3 -> Enable pet rename.
		SetByte( UNIT_FIELD_BYTES_2, 2, UNIT_BYTE2_FLAG_RENAME|UNIT_BYTE2_FLAG_PET_DETAILS );
	}
	
	BaseDamage[0] = 0;
	BaseDamage[1] = 0;
	BaseOffhandDamage[0 ] = 0;
	BaseOffhandDamage[1] = 0;
	BaseRangedDamage[0] = 0;
	BaseRangedDamage[1] = 0;
	m_base_runSpeed = m_runSpeed = owner->m_base_runSpeed;
	m_base_walkSpeed = m_walkSpeed = owner->m_base_walkSpeed;
	
	ApplyStatsForLevel();

	m_ExpireTime = expiretime;
	bSaveToDB = (( type & 0x10 )!=0);

	if( 
		bSaveToDB == true &&
//		LoadingHunterPet == false &&
		owner->GetPlayerPet( GetUInt32Value( UNIT_FIELD_PETNUMBER ) ) == NULL
		)
	{
		// Create PlayerPet struct (Rest done by UpdatePetInfo)
		PlayerPet *pp = new PlayerPet;
		pp->number = GetUInt32Value( UNIT_FIELD_PETNUMBER );
		pp->StableSlot = owner->GetUnstabledPetNumber();
		pp->spellid = created_by_spell ? created_by_spell->Id : 0;
		pp->name = m_name;
//		pp->active = true;
		pp->entry = GetEntry();
		pp->level = getLevel();
		pp->reset_cost = 0;
		pp->reset_time = UNIXTIME;
		pp->spellid = 0;
		pp->summon = false;
		pp->xp = 0;
		pp->HPPctOnDismiss = 100;
		owner->AddPlayerPet( pp, pp->number );
	}	

	SetStandState( STANDSTATE_STAND );

	//hack to wait for hunter pets to load from db
	if( LoadFromDb == false )
		InitializeMe( true );
}

//stupid pet number is byte[3,4,5] since we have highguid static we cannot use that :(
Pet::Pet( uint64 guid ) : Creature( guid )
{
	m_PetXP = 0;
	Summon = false;
	for( uint32 i=0;i<PET_ACTIONBAR_MAXSIZE;i++)
		ActionBar[i] = 0;
	internal_object_type = INTERNAL_OBJECT_TYPE_PET;

	m_AutoCombatSpell = 0;

//	m_HappinessTimer = PET_HAPPINESS_UPDATE_TIMER;

	m_State = PET_STATE_DEFENSIVE;
	m_Action = PET_ACTION_FOLLOW;
	m_ExpireTime = 0;
	bSaveToDB = false;
	m_Diet = 0;
	reset_time = 0;
	reset_cost = 0;

	for( uint8 i = 0; i < AUTOCAST_EVENT_COUNT; i++ )
		for( uint8 j = 0; j < AUTOCAST_EVENT_LIST_MAXLEN; j++ )
			m_autoCastSpells[i][j] = NULL;

	m_AISpellStore2.clear();
	mSpells.clear();

	//base value for new pets. This will get overwritten on loaded pets
	SetUInt32Value( UNIT_FIELD_PETNUMBER, Get_pet_number_from_guid( guid ) );

	//hackfix to make pets hold agro easier for noobs
	ModGeneratedThreatModifyerPCT( 0, 300 );	//a pet does around 200 - 500 dmg, owner does 17k DPS ?

	for( uint32 i=0;i<SCHOOL_COUNT;i++)
		owner_inherited_SP[ i ] = 0;

	OwnerAPContribPCT = 44; //should be 22 ?
	OwnerSPContribPCT = 60;
	OwnerStamContribPCT = 75;
	AuraMeleeHasteSum = 0;
}

Pet::~Pet()
{
	Virtual_Destructor();
}

void Pet::Virtual_Destructor()
{
	ObjectLock.Acquire();	//avoid list corruption while deleting
//	if( m_Owner )
//		m_Owner->RemoveFlag(PLAYER_FIELD_BYTES,PLAYER_BYTES_FLAG_HAS_PET);
	m_Owner = NULL;
	m_OwnerGuid = 0;
	sEventMgr.RemoveEvents( this );
	//fallthrough destructor(pet -> creature) would make acces higher level structures that got freed
	RemoveAllAuras();
/*	for(std::map<uint32, AI_Spell*>::iterator itr = m_AISpellStore.begin(); itr != m_AISpellStore.end(); ++itr)
	{
		delete itr->second;
		itr->second = NULL;
	}
	m_AISpellStore.clear(); */
	m_AISpellStore2.clear( 1 );

	for( uint8 i = 0; i < AUTOCAST_EVENT_COUNT; i++ )
		for( uint8 j = 0; j < AUTOCAST_EVENT_LIST_MAXLEN; j++ )
			m_autoCastSpells[i][j] = NULL;

	if(IsInWorld())
		this->Dismiss(true, true);

	mSpells.clear();
	m_talents.clear();
	ObjectLock.Release();
	//avoid lower destructor referencing to this object as higher level object
	//accesing already deleted data might lead to memory corruption
	Creature::Virtual_Destructor();
	m_objectTypeId = TYPEID_UNUSED;
	internal_object_type = INTERNAL_OBJECT_TYPE_CREATURE;
}

void Pet::Update( uint32 time )
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE

	//stupid idiot crash protection. Need to remake this
	if( m_Owner == NULL || GetTopOwner() == this )
	{
		m_Owner = NULL;	//wtf ?
		Dismiss();
		return;
	}

	Creature::Update( time ); // passthrough
	
/*	if( !Summon && !bExpires && isAlive() )
	{
		//ApplyPetLevelAbilities();
		//Happiness
		if( m_HappinessTimer == 0 )
		{	
			int32 burn = 1042;		//Based on WoWWiki pet looses 50 happiness over 6 min => 1042 every 7.5 s
			if( CombatStatus.IsInCombat() )
				burn >>= 1;			//in combat reduce burn by half (guessed) 
			ModPower( POWER_TYPE_HAPPINESS, -burn );
			m_HappinessTimer = PET_HAPPINESS_UPDATE_TIMER;// reset timer
		} 
		else if( !IsInBg() )
		{
			if( time > m_HappinessTimer )
				m_HappinessTimer = 0;
			else
				m_HappinessTimer -= time;
		}
	} */

	if( m_ExpireTime != 0 )
	{
		if( time >= m_ExpireTime )
		{
			// remove
			Dismiss();
			return;
		}
		else 
			m_ExpireTime -= time;
	}
}

void Pet::SendSpellsToOwner()
{
/*
13329
{SERVER} Packet: (0x5D7C) SMSG_PET_SPELLS PacketSize = 138 TimeStamp = 1114876
AA 6F 01 99 3B 8F 40 F1 guid
31 00 family
00 00 00 00 expiretime
00 petstate
01 petaction
00 00 actionbar flags
02 00 00 07 
01 00 00 07 
04 00 00 07 
73 82 00 01 
DB 7B 00 C1 
00 00 00 0A 
00 00 00 0B 
02 00 00 06 
01 00 00 06 
00 00 00 06 
09 73 82 00 
01 DB 7B 00 
C1 0F AE 00 
01 C4 FE 00 
01 49 8B 00 
01 4A 8B 00 
01 4B 8B 00 
01 4C 8B 00 
01 C8 75 01 
01 
03 EF F0 00 
00 00 
00 00 00 00 00 00 00 00
00 E5 07 01 
00 00 
00 00 00 00 00 00 00 00 
00 E9 07 01 
00 00 00 00 00 00 00 00 
00 00 
00
*/
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	if( m_Owner == NULL || !IsInWorld() )
	{ 
		return;
	}

	uint16 packetsize = ( uint16 )mSpells.size() * 4 + 61;
	WorldPacket * data = new WorldPacket( SMSG_PET_SPELLS, packetsize );
	*data << GetGUID();
	*data << uint16( myFamily != NULL ? myFamily->ID : 0 );	// pet family to determine talent tree
	*data << m_ExpireTime;
	*data << uint8( GetPetState() );	// 0x0 = passive, 0x1 = defensive, 0x2 = agressive
	*data << uint8( GetPetAction() );	// 0x0 = stay, 0x1 = follow, 0x2 = attack
	*data << uint16( 0 );				// flags: 0xFF = disabled pet bar (eg. when pet stunned)
																				
	// Send the actionbar
	for( uint8 i = 0; i < PET_ACTIONBAR_MAXSIZE; ++i )
	{
		if(ActionBar[i] & 0x4000000)		// Commands
			*data << uint32( ActionBar[i] );
		else
		{
			if( ActionBar[i] )
			{
				data->append_int24( ActionBar[i] );
				*data << uint8( GetSpellState( ActionBar[i] ) );
			}
			else
			{
				data->append_int24(0);
				*data << uint8(i+5);
			}
		}
	}

	// we don't send spells for the water elemental so it doesn't show up in the spellbook
	if( GetEntry() != WATER_ELEMENTAL && GetEntry() != SPIRITWOLF )
	{
		// Send the rest of the spells.
		*data << uint8( mSpells.size() );
		for( PetSpellMap::iterator itr = mSpells.begin(); itr != mSpells.end(); ++itr )
		{
			data->append_int24( itr->first->Id );
			*data << uint8( itr->second );
		}
	}

	uint32 HaveCooldowns = 0;
	SimplePointerListNode< AI_Spell > *itr;
	for( itr = m_AISpellStore2.begin(); itr != m_AISpellStore2.end(); itr = itr->next )
		if( itr->data->cooldowntime > getMSTime() )
			HaveCooldowns++;

	*data << uint8( HaveCooldowns ); 			// cooldowns count
	for( itr = m_AISpellStore2.begin(); itr != m_AISpellStore2.end(); itr = itr->next )
		if( itr->data->cooldowntime > getMSTime() )
		{
			*data << uint32( itr->data->spell->Id );			//spellid
			*data << uint16( itr->data->spell->Category );			//category
			*data << uint32( itr->data->cooldowntime - getMSTime() );			//spell cooldown
			*data << uint32( 0 );			//category cooldown

			//avoid sending more then 255 cooldowns
			HaveCooldowns--;
			if( HaveCooldowns == 0 )
				break;
		}
	//well we need this if we have pets for login else just use old session send
	{
		//m_Owner->delayedPackets.add( data );
	}
	{
		m_Owner->GetSession()->SendPacket( data );
		delete data;
	}

	smsg_TalentsInfo();
}

void Pet::SendNullSpellsToOwner()
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	if( m_Owner == NULL || m_Owner->GetSession() == NULL)
	{ 
		return;
	}

	WorldPacket data(8);
	data.SetOpcode( SMSG_PET_SPELLS );
	data << uint64(0);
	data << uint32(0);
	m_Owner->GetSession()->SendPacket( &data );
}

void Pet::SendCastFailed( uint32 spellid, uint8 fail )
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	if( m_Owner == NULL || m_Owner->GetSession() == NULL)
	{ 
		return;
	}

	WorldPacket data( SMSG_PET_CAST_FAILED, 6 );
	data << uint8( 0 );
	data << uint32( spellid );
	data << uint8( fail );
	m_Owner->GetSession()->SendPacket( &data );
}

void Pet::SendActionFeedback( PetActionFeedback value )
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	if( m_Owner == NULL || m_Owner->GetSession() == NULL)
	{ 
		return;
	}
	m_Owner->GetSession()->OutPacket( SMSG_PET_ACTION_FEEDBACK, 1, &value );
}

void Pet::InitializeSpells()
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	for( PetSpellMap::iterator itr = mSpells.begin(); itr != mSpells.end(); ++itr )
	{
		SpellEntry *info = itr->first;

		// Check that the spell isn't passive
		if( info->Attributes & ATTRIBUTES_PASSIVE )
		{
			// Cast on self..
			Spell * sp = SpellPool.PooledNew( __FILE__, __LINE__ );
			sp->Init( this, info, true, false );
			SpellCastTargets targets( this->GetGUID() );
			sp->prepare( &targets );

			continue;
		}

		AI_Spell * sp = CreateAISpell( info );
		if( itr->second == AUTOCAST_SPELL_STATE )
			SetAutoCast( sp, true );
		else
			SetAutoCast( sp, false );
	}
}

AI_Spell * Pet::CreateAISpell(SpellEntry * info)
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	ASSERT( info != NULL ); 

	// Create an AI_Spell
/*	map<uint32,AI_Spell*>::iterator itr = m_AISpellStore.find( info->Id );
	if( itr != m_AISpellStore.end() )
	{ 
		return itr->second;
	}*/
	AI_Spell *sp = GetAISpellForSpellId( info->Id );
	if( sp )
	{
		return sp;
	}

	sp = new AI_Spell;
	sp->agent = AGENT_SPELL;
	sp->entryId = GetEntry();
	sp->maxrange = GetMaxRange( dbcSpellRange.LookupEntry( info->rangeIndex ) );
	if( sp->maxrange < sqrt( info->base_range_or_radius_sqr ) )
		sp->maxrange = sqrt( info->base_range_or_radius_sqr );
	sp->minrange = GetMinRange( dbcSpellRange.LookupEntry( info->rangeIndex ) );
	sp->procChance = 0;
	sp->spell = info;
	sp->cooldown = objmgr.GetPetSpellCooldown(info->Id);
	if( sp->cooldown == 0 )
		sp->cooldown = info->StartRecoveryTime; //avoid spell spaming
	if( sp->cooldown == 0 )
		sp->cooldown = info->StartRecoveryCategory; //still 0 ?
	if( sp->cooldown == 0 )
		sp->cooldown = info->quick_duration_min; 
	if( sp->cooldown <= 500 || sp->cooldown == 1)
		sp->cooldown = PET_SPELL_SPAM_COOLDOWN; //omg, avoid spaming at least
	sp->cooldowntime = 0;
	
	sp->spelltargetType = info->ai_target_type;
	sp->autocast_type = GetAutoCastTypeForSpell( info );
	sp->procCount = 0;
//	m_AISpellStore[ info->Id ] = sp;
	m_AISpellStore2.push_front( sp );
	return sp;
}

void Pet::LoadFromDB( Player* owner, PlayerPet * pi )
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	m_Owner = owner;
	m_OwnerGuid = m_Owner->GetGUID();
	m_phase = m_Owner->GetPhase();
	mPi = pi;
//	creature_info = CreatureNameStorage.LookupEntry( mPi->entry );
//	Create( pi->name.c_str(), owner->GetMapId(), owner->GetPositionX() + 2 , owner->GetPositionY() + 2, owner->GetPositionZ(), owner->GetOrientation() );
//	LoadValues( mPi->fields.c_str() );

	SetPetNumber( mPi->number );
	m_PetXP = mPi->xp;
	m_name = mPi->name;
	Summon = mPi->summon;
//	m_HappinessTimer = mPi->happinessupdate;
	reset_time = mPi->reset_time;
	reset_cost = mPi->reset_cost;

	bSaveToDB = true;

	SetPetState( pi->State );

	// Setup actionbar
	if( mPi->actionbar.size() > 2 )
	{
		char * ab = strdup_local( mPi->actionbar.c_str() );
		char * p = strchr( ab, ',' );
		char * q = ab;
		uint32 spellid;
		uint32 spstate;
		uint32 i = 0;

		while( p && i < PET_ACTIONBAR_MAXSIZE )
		{
			*p = 0;

			if( sscanf( q, "%u %u", &spellid, &spstate ) != 2 )
				break;

			ActionBar[i] = spellid;
			//SetSpellState(dbcSpell.LookupEntry(spellid), spstate);
			if( !( ActionBar[i] & 0x4000000 ) && spellid )
				mSpells[ dbcSpell.LookupEntry( spellid ) ] = spstate;

			i++;

			q = p+1;
			p = strchr( q, ',' );
		}

		free(ab);
	}
	
	//Preventing overbuffs
	SetUInt32Value( UNIT_FIELD_ATTACK_POWER, 0 );
	SetUInt32Value( UNIT_FIELD_ATTACK_POWER_MOD_POS, 0 );
	SetUInt32Value( UNIT_FIELD_ATTACK_POWER_MOD_NEG, 0 );
	SetUInt32Value( UNIT_FIELD_BASEATTACKTIME, 2000 );

	if( m_Owner )
	{
		uint32 level = getLevel();
		if( level < m_Owner->getLevel() )
			level = m_Owner->getLevel();
		else if( level > m_Owner->getLevel() )
			level = m_Owner->getLevel();
		
		if( getLevel() != level )
		{
			SetUInt32Value( UNIT_FIELD_LEVEL, level );
			SetUInt32Value( UNIT_FIELD_PETEXPERIENCE, 0 );
			SetUInt32Value( UNIT_FIELD_PETNEXTLEVELEXP, GetNextLevelXP( level ) );
			if( !Summon )
				SetTalentPoints( GetUnspentTalentPoints() );
			ApplyStatsForLevel();
		}
		else if( Summon )
			ApplySummonLevelAbilities();
		else
			ApplyPetLevelAbilities();

		//this is a must !
		SetUInt64Value( UNIT_FIELD_CREATEDBY, owner->GetGUID() );
	}

	InitializeMe( false, pi );
	
	/*
	//if pet was dead on logout then it should be dead now too
	if( HasFlag( UNIT_FIELD_FLAGS, UNIT_FLAG_DEAD ) )
	{
		SetUInt32Value(UNIT_FIELD_HEALTH, 0); //this is probably already 0
		setDeathState( JUST_DIED );
		m_Owner->EventDismissPet();	//just in case in near future(or any other) on loading auras get applied then make sure we remove those
	} 
	/**/

	SetStandState( STANDSTATE_STAND );
}

void Pet::OnPushToWorld()
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	Creature::OnPushToWorld();
}

void Pet::InitializeMe( bool first, PlayerPet * pi )
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	GetAIInterface()->Init( this, AITYPE_PET, MOVEMENTTYPE_NONE, m_Owner );
	GetAIInterface()->SetUnitToFollow( m_Owner );
//	GetAIInterface()->SetFollowDistance( 3.0f );
	GetAIInterface()->m_spells.clear( 1 );	//how on earth are pets using normal mob spells ?

	SetCreatureInfo( CreatureNameStorage.LookupEntry( GetEntry() ) );
	proto = CreatureProtoStorage.LookupEntry( GetEntry() );
	
	m_Owner->SetSummon( this );
	
	//SetUInt32Value( UNIT_FIELD_PETNUMBER, GetUIdFromGUID() );
	//ex from blizz : guid : 7C AF 00 19 F6 47 40 F1 -> ptnr : 47 F6 19 ( 19 F6 47 )
	//that kinda means that our system will only support pet number 0 ? (blizzuses at least 1)
	//SetUInt32Value( UNIT_FIELD_PETNUMBER, m_PetNumber );
	SetUInt32Value( UNIT_FIELD_PET_NAME_TIMESTAMP, (uint32)UNIXTIME );
	
	if( GetCreatureInfo() )
		myFamily = dbcCreatureFamily.LookupEntryForced( GetCreatureInfo()->Family );
	else myFamily = NULL;
	
	SetInstanceID( m_Owner->GetInstanceID() );
	SetPetDiet();
	SetUInt32Value( UNIT_FIELD_FACTIONTEMPLATE, m_Owner->GetUInt32Value( UNIT_FIELD_FACTIONTEMPLATE ) );
	_setFaction();

	//inherit PVP settings from owner
	if( m_Owner->IsPvPFlagged() )
		SetPvPFlag();
	else
		RemovePvPFlag();
	if( m_Owner->IsFFAPvPFlagged() )
		SetFFAPvPFlag();
	else
		RemoveFFAPvPFlag();

	// Load our spells
	if( Summon ) // Summons - always
	{
		SetDefaultSpells();
		if( GetEntry() == PET_IMP )
			m_aiInterface->disable_melee = true;
		else if( GetEntry() == PET_FELGUARD )
			SetUInt32Value( UNIT_VIRTUAL_ITEM_SLOT_ID, 12784 );
	}
	if( first ) // Hunter pets - after taming
	{
		SetDefaultSpells();						// create our spells
		SetTalentPoints( GetTalentPointsForLevel( getLevel() ) );	// set talent points
	}
	else // Hunter / warlock pets - load from db
	{
		// Pull from database... :/
		QueryResult * query = CharacterDatabase.Query("SELECT * FROM playerpetspells WHERE ownerguid=%u and petnumber=%u",
			m_Owner->GetLowGUID(), GetUInt32Value( UNIT_FIELD_PETNUMBER ) );
		if( query )
		{
			do 
			{
				Field * f = query->Fetch();
				uint32 entry = f[2].GetUInt32();
				uint8 flags = f[3].GetUInt8();
				if( (flags & PET_ACTION_SPELL_IS_TALENT) == PET_ACTION_SPELL_IS_TALENT )
					AddPetTalent( entry, flags & ~PET_ACTION_SPELL_IS_TALENT );
				else
				{
					SpellEntry* spell = dbcSpell.LookupEntry( entry );
					if( spell != NULL )
					{
						mSpells.erase( spell ); //we had flags ? delete them, these were set automatically when we created the pet
						mSpells.insert( make_pair( spell, flags ) );
					}
				}

			} while( query->NextRow() );
			delete query;
			query = NULL;
		}
	}

	InitializeSpells(); 

	if( pi )
	{
		//restore cooldowns
		std::map<uint32,uint32>::iterator itr;
		for( itr = pi->Cooldowns.begin(); itr != pi->Cooldowns.end(); itr++ )
		{
			AI_Spell * sp = GetAISpellForSpellId( itr->first );
			if( sp != NULL )
			{
//				sp->cooldowntime = getMSTime() + itr->second;
				sp->cooldowntime = itr->second; //some say this keeps decreasing for non summoned pets
			}
		}

		//restore pre dismiss HP
		if( pi->HPPctOnDismiss > 0 && pi->HPPctOnDismiss < 100 )
		{
			uint32 NewHP = GetMaxHealth() * pi->HPPctOnDismiss / 100;
			SetHealth( NewHP );
		}
	}

	//let's relearn normal spells in case we lost them for some reason (offline talent reset). without packet spamming
//	SkillUp();	
	PushToWorld( m_Owner->GetMapMgr() );

	//no idea why we need this :S
	WorldPacket data(SMSG_PET_GUIDS, 12);
	data << uint32( 1 ); //not sure
	data << GetGUID();
	m_Owner->GetSession()->SendPacket(&data);
	
	if( first )
	{
		// Set up default actionbar
		SetDefaultActionbar();
	}

	SkillUp();	//need to be after pushtoworld so static auras get also applied
	SendSpellsToOwner();

	// set to active
	if( bSaveToDB == true )
		UpdatePetInfo( false );

	sEventMgr.AddEvent( this, &Pet::HandleAutoCastEvent, AUTOCAST_EVENT_ON_SPAWN,		EVENT_UNK, 1000, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
	sEventMgr.AddEvent( this, &Pet::HandleAutoCastEvent, AUTOCAST_EVENT_LEAVE_COMBAT,	EVENT_UNK, 1000, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );

	//needs to be after object has been created client side !
	if( m_Owner )
	{
//		sEventMgr.AddEvent( m_Owner, &Player::EventSummonPet, this, EVENT_UNK,5000,1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
		m_Owner->EventSummonPet( this );
	}

	//this is just an experiment : OBJECT_FIELD_DATA seems to be a huge number that groups objects. Like GROUP_GUID. like player and he's pet.
	//set guild also
	if( m_Owner->GetUInt32Value( OBJECT_FIELD_DATA ) )
	{
		SetUInt32Value( OBJECT_FIELD_DATA, m_Owner->GetUInt32Value( OBJECT_FIELD_DATA ) );
		SetUInt32Value( OBJECT_FIELD_DATA_1, m_Owner->GetUInt32Value( OBJECT_FIELD_DATA_1 ) );
		SetUInt32Value( OBJECT_FIELD_TYPE, GetUInt32Value( OBJECT_FIELD_TYPE ) | TYPE_IN_GUILD );
	}
}


void Pet::UpdatePetInfo( bool bSetToOffline )
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	if( bSaveToDB == false || m_Owner == NULL ) 	// don't update expiring pets
	{ 
		return;
	}

	PlayerPet *pi = m_Owner->GetPlayerPet( GetUInt32Value( UNIT_FIELD_PETNUMBER ) );
	//wtf bug check ? we are getting created as playerpet but owner does not have us anymore ?
	if( pi == NULL )
	{
		return;
	}
//	pi->active = !bSetToOffline;
	pi->entry = GetEntry();
	std::stringstream ss;
//	for( uint32 index = 0; index < UNIT_END; index ++ )
//		ss << GetUInt32Value(index) << " ";
//	pi->fields = ss.str();
	pi->name = GetName()->c_str();
	pi->number = GetUInt32Value( UNIT_FIELD_PETNUMBER );
	pi->xp = m_PetXP;
	pi->level = getLevel();
//	pi->happinessupdate = m_HappinessTimer;

	// save actionbar
	ss.rdbuf()->str("");
	for(uint32 i = 0; i < PET_ACTIONBAR_MAXSIZE; ++i)
	{
		if( ActionBar[i] & 0x4000000 )
			ss << ActionBar[i] << " 0";
		else if(ActionBar[i])
			ss << ActionBar[i] << " " << uint32(GetSpellState( ActionBar[i] ) );
		else
			ss << "0 0";

		ss << ",";
	}

	pi->actionbar = ss.str();
	pi->summon = Summon;
	pi->reset_cost = reset_cost;
	pi->reset_time = reset_time;
	pi->State = GetPetState();

	if( isAlive() )
		pi->HPPctOnDismiss = GetHealthPct();
	else
		pi->HPPctOnDismiss = 100;

	//cooldowns
	{
		pi->Cooldowns.clear();
		SimplePointerListNode< AI_Spell > *itr;
		for( itr = m_AISpellStore2.begin(); itr != m_AISpellStore2.end(); itr = itr->next )
			if( itr->data->cooldowntime > getMSTime() )
			{
				int32 RemainingMS = itr->data->cooldowntime - getMSTime();
				if( RemainingMS > 2000 )
				{
//					pi->Cooldowns[ itr->data->spell->Id ] = RemainingMS;
					pi->Cooldowns[ itr->data->spell->Id ] = itr->data->cooldowntime;	//some say this keeps decreasing for non summoned pets
					//save to DB also ?
				}
			}
	}
}

void Pet::Abandon() //Abandon pet
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	// Delete any petspells for us.
	if( m_Owner && bSaveToDB == true )
		m_Owner->RemovePlayerPet( GetUInt32Value( UNIT_FIELD_PETNUMBER ), -1 ); // find out playerpet entry, delete it
//	if( m_Owner )
//		m_Owner->RemoveFlag(PLAYER_FIELD_BYTES,PLAYER_BYTES_FLAG_HAS_PET);

	Dismiss( false, true );
}

void Pet::Dismiss( bool bUpdate, bool bSetOffline, bool AutoResummon )
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	RemoveAllAuras(); // Prevent pet overbuffing
	if( m_Owner )
	{
		if( AutoResummon && m_ExpireTime == 0 )
			m_Owner->SetAutoSummonPetNR( GetPetNumber() );
		m_Owner->EventDismissPet();
		if( bUpdate && bSaveToDB == true )
		{
			UpdatePetInfo( bSetOffline );
			m_Owner->_SavePet( NULL );
		}
		m_Owner->SetUInt64Value( UNIT_FIELD_SUMMON, 0 );
		if( m_Owner->GetSummon() == this )
			m_Owner->SetSummon( NULL );
		SendNullSpellsToOwner();
		ClearPetOwner();
	}
	
	// has to be next loop - reason because of RemoveFromWorld, iterator gets broken
	if( IsInWorld() && Active )
		Deactivate( m_mapMgr );
	
	if( sEventMgr.HasEvent( this, EVENT_CREATURE_SAFE_DELETE ) == false )
	{
		sEventMgr.RemoveEvents(this);
		sEventMgr.AddEvent( this, &Pet::PetSafeDelete, EVENT_CREATURE_SAFE_DELETE, 1, 1, EVENT_FLAG_DELETES_OBJECT | EVENT_FLAG_UNIQUE_EVENT_DELETE_NEWER );
	}
}

void Pet::PetSafeDelete()
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	// remove from world, and delete
	if( IsInWorld() )
	{
		RemoveFromWorld( false, false );
	}
	
	sEventMgr.RemoveEvents( this ); // to avoid crash of float delete
	sGarbageCollection.AddObject( this );
}

void Pet::DelayedRemove( bool bTime, bool bDeath )
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	// called when pet has died
	if( bTime )
	{
		if( !m_Owner )
			m_Owner = objmgr.GetPlayer( GET_LOWGUID_PART( m_OwnerGuid ) );

		Dismiss( true, false );
	}
	else
		sEventMgr.AddEvent(this, &Pet::DelayedRemove, true, bDeath, EVENT_PET_DELAYED_REMOVE, PET_DELAYED_REMOVAL_TIME, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
}

bool Pet::CanGainXP()
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	// only hunter pets which are below owner level can gain experience
	if( Summon || !m_Owner || getLevel() >= m_Owner->getLevel() )
	{ 
		return false;
	}
	else
		return true;
}

void Pet::GiveXP( uint32 xp )
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	if( ( m_Owner || !Summon ) && getLevel() < m_Owner->getLevel() ) 
	{
		xp += m_uint32Values[ UNIT_FIELD_PETEXPERIENCE ];
		uint32 nxp = m_uint32Values[ UNIT_FIELD_PETNEXTLEVELEXP ];

		if( xp >= nxp )
		{
			SetTalentPoints( GetUnspentTalentPoints() + 1 );
			ModUnsigned32Value( UNIT_FIELD_LEVEL, 1 );
			xp -= nxp;
			nxp = GetNextLevelXP( m_uint32Values[ UNIT_FIELD_LEVEL ] );
			SetUInt32Value( UNIT_FIELD_PETNEXTLEVELEXP, nxp );
			ApplyStatsForLevel();
			SkillUp();
		}

		SetUInt32Value( UNIT_FIELD_PETEXPERIENCE, xp );
	}
}

uint32 Pet::GetNextLevelXP(uint32 level)
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	// Pets need only 5% of xp to level up compared to players
	uint32 nextLvlXP = 0;
	if( level > 0 && level <= MAX_PREDEFINED_NEXTLEVELXP )
	{
		nextLvlXP = NextLevelXp[ level - 1 ];
	}
	else
	{
		nextLvlXP = ((int)((((float)(((8 * level) + ((level - 30) * 5)) * ((level * 5) + 45)))/100)+0.5))*100;
	}
	uint32 xp = nextLvlXP / 5;
	return xp;
}

void Pet::SetDefaultSpells()
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE

/*	if( Summon )
	{
		// this one's easy :p we just pull em from the owner.
		map<uint32, set<uint32> >::iterator it1;
		set<uint32>::iterator it2;
		it1 = m_Owner->SummonSpells.find(GetEntry());
		if(it1 != m_Owner->SummonSpells.end())
		{
			it2 = it1->second.begin();
			for(; it2 != it1->second.end(); ++it2)
			{
				AddSpell( dbcSpell.LookupEntry( *it2 ), false, false );
			}
		}
	}
	else  */
//		if( GetCreatureInfo() ) //Hunter pet
	{
		SkillUp();
/*		uint32 Line = GetCreatureInfo()->SpellDataID;
		if( Line == 0 )
			sLog.outError("DB Error: You miss spell data for creature id %u in your DB.", GetCreatureInfo()->Id );
		else
		{
			CreatureSpellDataEntry * SpellData = dbcCreatureSpellData.LookupEntry( Line );
			if( SpellData )
				for( uint32 i = 0; i < 3; ++i )
					if( SpellData->Spells[i] != 0 )
						AddSpell( dbcSpell.LookupEntry( SpellData->Spells[i] ), false ); //add spell to pet
		}*/
		if( Summon == false )
			AddSpell( dbcSpell.LookupEntry( GROWL_RANK_1 ), false, false );
	}
}

//learning means to add it to action bar or not. Summons may have multiple levels of same spells (not hunter pets)
void Pet::AddSpell( SpellEntry * sp, bool learning, bool refreshclient )
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	if( sp == NULL )
	{ 
		return;
	}

	// Cast on self if we're a passive spell
	if( sp->Attributes & ATTRIBUTES_PASSIVE )
	{
		if( IsInWorld() )
		{
			if( HasAura( sp->Id ) )
			{
				sLog.outDebug("Pet learn : we already have this aura. Exiting\n");
				return;
			}

			Spell * spell = SpellPool.PooledNew( __FILE__, __LINE__ );
			spell->Init(this, sp, true, false);
			SpellCastTargets targets(this->GetGUID());
			spell->prepare(&targets);
			mSpells[sp] = PASSIVE_SPELL_STATE;
		}

		//spells that are not shown in any way by client 
		//if( sp->activeIconID == 0 && sp->Rank == NULL && sp->SpellVisual == 0 )
		//	return; 
	}
	else
	{
		//why would we want to readd same spell ?
		if(mSpells.find(sp) == mSpells.end())
		{
			if( learning )
			{
				for(PetSpellMap::iterator itr = mSpells.begin(); itr != mSpells.end(); ++itr)
					if(sp->NameHash == itr->first->NameHash)
					{
						//no need to add lower level spell then the one we already have in spellbook
						if( itr->first->SpellLevel.baseLevel > sp->SpellLevel.baseLevel )
						{
							sLog.outDebug("Pet learn : we already have sronger spell. Exiting\n");
							return;
						}

						// replace the action bar
						for(int i = 0; i < PET_ACTIONBAR_MAXSIZE; ++i)
						{
							if(ActionBar[i] == itr->first->Id)
							{
								ActionBar[i] = sp->Id;
								break;
							}
						}

						RemoveAura( itr->first->Id ); //make sure there is no activa aura for this spell

						//warning this will change the list we are reading !
						if( itr->first->SpellLevel.baseLevel < sp->SpellLevel.baseLevel )
							RemoveSpell(itr->first);
						break;
					}

				AI_Spell * asp = CreateAISpell(sp);
				uint8 ss = (asp->autocast_type > 0) ? AUTOCAST_SPELL_STATE : DEFAULT_SPELL_STATE;
				mSpells[sp] = ss;
				if(ss==AUTOCAST_SPELL_STATE)
					SetAutoCast(asp,true);

				if(asp->autocast_type==AUTOCAST_EVENT_ON_SPAWN)
					CastSpell(this, sp, false);
			}
			else
				mSpells[sp] = DEFAULT_SPELL_STATE;
		}
		else
		{
			sLog.outDebug("Pet learn : we already have this spell. Exiting\n");
		}

	   // Active spell add to the actionbar.
		bool has=false;
		for(int i = 0; i < PET_ACTIONBAR_MAXSIZE; ++i)
			if(ActionBar[i] == sp->Id)
			{
				has=true;
				break;
			}

//		if( !has && !learning )
		if( !has )
			for(int i = 0; i < PET_ACTIONBAR_MAXSIZE; ++i)
				if(ActionBar[i] == 0)
				{
					ActionBar[i] = sp->Id;
					break;
				}
	}

	if( IsInWorld() && refreshclient == true )
		SendSpellsToOwner();
}

void Pet::SetSpellState(SpellEntry* sp, uint8 State)
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	PetSpellMap::iterator itr = mSpells.find(sp);
	if(itr == mSpells.end())
	{ 
		return;
	}

	uint8 oldstate = itr->second;
	itr->second = State;

	if(State == AUTOCAST_SPELL_STATE || oldstate == AUTOCAST_SPELL_STATE)
	{
		AI_Spell * sp2 = GetAISpellForSpellId(sp->Id);
		if(sp2)
		{
			if(State == AUTOCAST_SPELL_STATE)
				SetAutoCast(sp2, true);
			else
				SetAutoCast(sp2,false);
		}
	}		
}

uint8 Pet::GetSpellState(SpellEntry* sp)
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	PetSpellMap::iterator itr = mSpells.find(sp);
	if(itr == mSpells.end())
	{ 
		return DEFAULT_SPELL_STATE;
	}

	return itr->second;
}

void Pet::SetDefaultActionbar()
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	// Set up the default actionbar.
	ActionBar[0] = PET_SPELL_ATTACK;
	ActionBar[1] = PET_SPELL_FOLLOW;
	ActionBar[2] = PET_SPELL_STAY;

	// Fill up 4 slots with our spells
	if(mSpells.size() > 0)
	{
		PetSpellMap::iterator itr = mSpells.begin();
		uint32 pos = 0;
		for(; itr != mSpells.end() && pos < 4; ++itr, ++pos)
			ActionBar[3+pos] = itr->first->Id;
	}

	ActionBar[7] = PET_SPELL_AGRESSIVE;
	ActionBar[8] = PET_SPELL_DEFENSIVE;
	ActionBar[9] = PET_SPELL_PASSIVE;
}

AI_Spell* Pet::GetAISpellForSpellId(uint32 spellid)
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
//	std::map<uint32, AI_Spell*>::iterator itr = m_AISpellStore.find(spellid);
//	if(itr != m_AISpellStore.end())
//		return itr->second;
//	else
//		return NULL;
	SimplePointerListNode< AI_Spell > *itr;
	for( itr = m_AISpellStore2.begin(); itr != m_AISpellStore2.end(); itr = itr->next )
		if( itr->data->spell->Id == spellid )
			return itr->data;
	return NULL;
}

void Pet::RemoveSpell(SpellEntry * sp)
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	mSpells.erase(sp);

/*	map<uint32, AI_Spell*>::iterator itr = m_AISpellStore.find(sp->Id);
	if(itr != m_AISpellStore.end())
	{
		uint32 act = itr->second->autocast_type;
		if( act < AUTOCAST_EVENT_COUNT )
		{
			for(uint32 i3=0;i3<AUTOCAST_EVENT_LIST_MAXLEN;i3++)
				if( m_autoCastSpells[ act ][ i3 ] == itr->second )
					m_autoCastSpells[ act ][ i3 ] = NULL;
		}

		m_aiInterface->CheckNextSpell(itr->second);

		delete itr->second;
		itr->second = NULL;
		m_AISpellStore.erase(itr);
	}*/
	AI_Spell *spt = GetAISpellForSpellId( sp->Id );
	if( spt )
	{
		uint32 act = spt->autocast_type;
		if( act < AUTOCAST_EVENT_COUNT )
			for(uint32 i3=0;i3<AUTOCAST_EVENT_LIST_MAXLEN;i3++)
				if( m_autoCastSpells[ act ][ i3 ] == spt )
					m_autoCastSpells[ act ][ i3 ] = NULL;
		m_aiInterface->CheckNextSpell( spt );
		m_AISpellStore2.remove( spt, 1 );
	}

	//Remove spell from action bar as well
	for( uint32 pos = 0; pos < PET_ACTIONBAR_MAXSIZE; pos++ )
	{
		if( ActionBar[pos] == sp->Id )
			ActionBar[pos] = 0;
	}
	
	if( m_Owner != NULL && m_Owner->GetSession() != NULL )
		m_Owner->GetSession()->OutPacket( SMSG_PET_REMOVED_SPELL, 4, &sp->Id );
}

void Pet::Rename( string NewName )
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	m_name = NewName;
	// update petinfo
	UpdatePetInfo( false );

	// update timestamp to force a re-query
	SetUInt32Value( UNIT_FIELD_PET_NAME_TIMESTAMP, ( uint32 )UNIXTIME );

	// save new summoned name to db (.pet renamepet)
	if( m_Owner->getClass() == WARLOCK )
	{
		CharacterDatabase.Execute( "UPDATE `playerpets` SET `name`='%s' WHERE `ownerguid`=%u AND `entry`=%u",
			CharacterDatabase.EscapeString( m_name ).c_str(), m_Owner->GetLowGUID(), GetEntry() );
	}
}

void Pet::ApplySummonLevelAbilities()
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	uint32 level = m_uint32Values[UNIT_FIELD_LEVEL];
	float pet_level = (float)level;

	// Determine our stat index.
	int stat_index = -1;
	//float scale = 1;
	bool has_mana = true;

	switch( GetEntry() )
	{
	case 416: //Imp
		stat_index = 0;
		m_aiInterface->disable_melee = true;
		break;
	case 1860: //VoidWalker
		stat_index = 1;
		break;
	case 1863: //Succubus
		stat_index = 2;
		break;
	case 417: //Felhunter
		stat_index = 3;
		break;
	case 11859: // Doomguard
	case 89:	// Infernal
	case 17252: // Felguard
		stat_index = 4;
		break;
	/*case 11859: // Doomguard
		stat_index = 4;
		break;
	case 89:	// Infernal
		stat_index = 5;
		has_mana = false;
		break;
	case 17252: // Felguard
		stat_index = 6;
		break;*/
	case 510:	// Mage's water elemental
		stat_index = 5;
		m_aiInterface->disable_melee = true;
		break;
	case 19668:    // Priest's Shadowfiend, until someone knows the stats that real have
		stat_index = 5;
		break;
	case 26125:	//Ghoul
		stat_index = 6;
		break;
	case 29264: //feral spirit
		stat_index = 5;
		break;
	case 27893:
		stat_index = 4;
		break;
	}
	if(m_uint32Values[OBJECT_FIELD_ENTRY] == 89)
		has_mana = false;

	if(stat_index < 0)
	{
		sLog.outError("PETSTAT: No stat index found for entry %u, `%s`!", GetEntry(), GetCreatureInfo()->Name);
		return;
	}

	static float R_base_str[7] = {18.1884058f, -15.0f, -15.0f, -15.0f, -15.0f, -15.0f, -15.0f};
	static float R_mod_str[7] = {1.811594203f, 2.4f, 2.4f, 2.4f, 2.4f, 2.4f, 2.4f};
	static float R_base_agi[7] = {19.72463768f, -1.25f, -1.25f, -1.25f, -1.25f, -1.25f, -1.25f};
	static float R_mod_agi[7] = {0.275362319f, 1.575f, 1.575f, 1.575f, 1.575f, 1.575f, 1.575f};
	static float R_base_sta[7] = {18.82608695f, -3.5f, -17.75f, -17.75f, -17.75f, 0.0f, -17.75f};
	static float R_mod_sta[7] = {1.173913043f, 4.05f, 4.525f, 4.525f, 4.525f, 4.044f, 8.525f};
	static float R_base_int[7] = {19.44927536f, 12.75f, 12.75f, 12.75f, 12.75f, 20.0f, 12.75f};
	static float R_mod_int[7] = {4.550724638f, 1.875f, 1.875f, 1.875f, 1.875f, 2.8276f, 1.875f};
	static float R_base_spr[7] = {19.52173913f, -2.25f, -2.25f, -2.25f, -2.25f, 20.5f, -2.25f};
	static float R_mod_spr[7] = {3.47826087f, 1.775f, 1.775f, 1.775f, 1.775f, 3.5f, 1.775f};
	static float R_base_pwr[7] = {7.202898551f, -101.0f, -101.0f, -101.0f, -101.0f, -101.0f, -101.0f};
	static float R_mod_pwr[7] = {2.797101449f, 6.5f, 6.5f, 6.5f, 6.5f, 6.5f, 6.5f};
	static float R_base_armor[7] = {-11.69565217f, -702.0f, -929.4f, -1841.25f, -1157.55f, 0.0f, -702.0f};
	static float R_mod_armor[7] = {31.69565217f, 139.6f, 74.62f, 89.175f, 101.1316667f, 20.0f, 139.6f};
	static float R_pet_sta_to_hp[7] = {4.5f, 15.0f, 7.5f, 10.0f, 10.6f, 7.5f, 60.0f};
	static float R_pet_int_to_mana[7] = {15.0f, 15.0f, 15.0f, 15.0f, 15.0f, 5.0f, 15.0f};
	static float R_base_min_dmg[7] = {0.550724638f, 4.566666667f, 26.82f, 29.15f, 20.17888889f, 20.0f, 20.0f};
	static float R_mod_min_dmg[7] = {1.449275362f, 1.433333333f, 2.18f, 1.85f, 1.821111111f, 1.0f, 2.18f};
	static float R_base_max_dmg[7] = {1.028985507f, 7.133333333f, 36.16f, 39.6f, 27.63111111f, 20.0f, 36.16f};
	static float R_mod_max_dmg[7] = {1.971014493f, 1.866666667f, 2.84f, 2.4f, 2.368888889f, 1.1f, 2.84f};

	float base_str = R_base_str[stat_index];
	float mod_str = R_mod_str[stat_index];
	float base_agi = R_base_agi[stat_index];
	float mod_agi = R_mod_agi[stat_index];
	float base_sta = R_base_sta[stat_index];
	float mod_sta = R_mod_sta[stat_index];
	float base_int = R_base_int[stat_index];
	float mod_int = R_mod_int[stat_index];
	float base_spr = R_base_spr[stat_index];
	float mod_spr = R_mod_spr[stat_index];
	float base_pwr = R_base_pwr[stat_index];
	float mod_pwr = R_mod_pwr[stat_index];
	float base_armor = R_base_armor[stat_index];
	float mod_armor = R_mod_armor[stat_index];
	float base_min_dmg = R_base_min_dmg[stat_index];
	float mod_min_dmg = R_mod_min_dmg[stat_index];
	float base_max_dmg = R_base_max_dmg[stat_index];
	float mod_max_dmg = R_mod_max_dmg[stat_index];
	float pet_sta_to_hp = R_pet_sta_to_hp[stat_index];
	float pet_int_to_mana = R_pet_int_to_mana[stat_index];

	// Calculate bonuses
	float pet_str = base_str + pet_level * mod_str;
	float pet_agi = base_agi + pet_level * mod_agi;
	float pet_sta = base_sta + pet_level * mod_sta;
	float pet_int = base_int + pet_level * mod_int;
	float pet_spr = base_spr + pet_level * mod_spr;
	float pet_pwr = base_pwr + pet_level * mod_pwr;
	float pet_arm = base_armor + pet_level * mod_armor;

	uint32 dominant_power_value;
	uint8 owner_class = GetPetOwner()->getClass();
	if( owner_class == WARRIOR || owner_class == ROGUE || owner_class == DEATHKNIGHT )
		dominant_power_value = GetPetOwner()->GetAP();
	else if( owner_class == WARLOCK )
		dominant_power_value = GetPetOwner()->GetDamageDoneMod(SCHOOL_SHADOW);
	else if( owner_class == MAGE )
		dominant_power_value = MAX(MAX(GetPetOwner()->GetDamageDoneMod(SCHOOL_FIRE), GetPetOwner()->GetDamageDoneMod(SCHOOL_FROST)), GetDamageDoneMod(SCHOOL_ARCANE) );
	else if( owner_class == PRIEST )
		dominant_power_value = MAX(GetPetOwner()->GetDamageDoneMod(SCHOOL_HOLY), GetPetOwner()->GetDamageDoneMod(SCHOOL_SHADOW) );
	else if( owner_class == HUNTER )
		dominant_power_value = GetPetOwner()->GetRAP();
	else if( owner_class == PALADIN )
		dominant_power_value = MAX( GetPetOwner()->GetDamageDoneMod(SCHOOL_HOLY), GetPetOwner()->GetAP() );
	else if( owner_class == SHAMAN || owner_class == DRUID )
		dominant_power_value = MAX( GetPetOwner()->GetDamageDoneMod(SCHOOL_NATURE), GetPetOwner()->GetAP() );
	else
		dominant_power_value = 400 / float2int32( pet_level ); //240 dmg using 12% formula at lvl 80
	pet_pwr = MAX( pet_pwr, dominant_power_value / 2 );

	// Calculate values
	BaseStats[STAT_STRENGTH] = float2int32(pet_str);
	BaseStats[STAT_AGILITY] = float2int32(pet_agi);
	BaseStats[STAT_STAMINA] = float2int32(pet_sta);
	BaseStats[STAT_INTELLECT] = float2int32(pet_int);
	BaseStats[STAT_SPIRIT] = float2int32(pet_spr);

#define JUST_QQ_REMOVAL_EXTRA_DMG_ADD 1.5f
	float pet_min_dmg = base_min_dmg + pet_level * (mod_min_dmg+JUST_QQ_REMOVAL_EXTRA_DMG_ADD);
	float pet_max_dmg = base_max_dmg + pet_level * (mod_max_dmg+JUST_QQ_REMOVAL_EXTRA_DMG_ADD);
	BaseDamage[0] = float(pet_min_dmg + pet_pwr / 14);
	BaseDamage[1] = float(pet_max_dmg + pet_pwr / 14);

	// Apply attack power.
	SetUInt32Value(UNIT_FIELD_ATTACK_POWER, float2int32(pet_pwr));
		
	for( int32 i = 0; i < SCHOOL_COUNT; i++ )
		BaseResistance[ i ] = GetPetOwner()->GetResistance( i ) * 70 / 100;
//	BaseResistance[0] = float2int32(pet_arm);
//	CalcResistance(0);

	// Calculate health / mana
	float health = pet_sta * pet_sta_to_hp;
	float mana = has_mana ? (pet_int * pet_int_to_mana) : 0.0f;
	if( health == 0.0f )
	{
		sLog.outError("Pet with entry %u has 0 health !! \n",m_uint32Values[OBJECT_FIELD_ENTRY]);
		health = 100.0f;
	}
	SetUInt32Value(UNIT_FIELD_BASE_HEALTH, float2int32(health));
	SetUInt32Value(UNIT_FIELD_MAXHEALTH, float2int32(health));
	SetUInt32Value(UNIT_FIELD_BASE_MANA, float2int32(mana));
	SetMaxPower(POWER_TYPE_MANA, float2int32(mana));

	switch( GetEntry() )
	{
		case 29264: //feral spirit
		{
			BaseResistance[ 0 ] = GetPetOwner()->GetResistance( 0 ) * 35 / 100;
			CalcResistance(0);
			OwnerAPContribPCT = 30;
			if( GetPetOwner()->HasGlyphWithID( GLYPH_SHAMAN_FERAL_SPIRIT ) )
				OwnerAPContribPCT =  60;
			uint32 PetAP = GetPetOwner()->GetAP() * OwnerAPContribPCT / 100;
			SetUInt32Value( UNIT_FIELD_ATTACK_POWER, PetAP );
			BaseDamage[0] = float(pet_min_dmg + PetAP / 14 );
			BaseDamage[1] = float(pet_max_dmg + PetAP / 14 );
			OwnerStamContribPCT = 75 + 30;
			SetUInt32Value( UNIT_FIELD_BASE_HEALTH, GetPetOwner()->GetStat( STAT_STAMINA ) * 30 / 100 * 10 );
			SetUInt32Value( UNIT_FIELD_MAXHEALTH, GetPetOwner()->GetStat( STAT_STAMINA ) * 30 / 100 * 10 );
		}break;
		case 26125: //raise dead
		{
			if( GetPetOwner()->HasGlyphWithID( GLYPH_DEATHKNIGHT_RAISE_DEAD ) )
			{
				OwnerAPContribPCT = 44 + 40;
				uint32 OldBaseHP = GetUInt32Value( UNIT_FIELD_BASE_HEALTH );
				uint32 AddHP = GetPetOwner()->GetStat( STAT_STAMINA ) * 40 / 100 * 10;
				SetUInt32Value( UNIT_FIELD_BASE_HEALTH, OldBaseHP + AddHP );
				uint32 OldMaxHP = GetUInt32Value( UNIT_FIELD_MAXHEALTH );
				SetUInt32Value( UNIT_FIELD_MAXHEALTH, OldMaxHP + AddHP );
			}
			uint32 PetAP = GetPetOwner()->GetAP() * OwnerAPContribPCT / 100;
			SetUInt32Value( UNIT_FIELD_ATTACK_POWER, PetAP );
			BaseDamage[0] = float(pet_min_dmg + PetAP / 14 );
			BaseDamage[1] = float(pet_max_dmg + PetAP / 14 );
		}break;
		default:
			break;
	};

	for(uint32 x = 0; x < 5; ++x)
		CalcStat(x);
}

void Pet::ApplyPetLevelAbilities()
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	if( !GetCreatureInfo() )
		return;
	uint32 pet_family = GetCreatureInfo()->Family;

	uint32 level = getLevel();
	if( level > PLAYER_LEVEL_CAP )
		level = PLAYER_LEVEL_CAP;
	else if (level < 1)
		level = 1;

/*
	from wowwiki : 
	Pets receive attribute bonuses at 15% of their master's/mistress' stats:
    1 ranged attack power gives the pet 0.22 AP and 0.1287 spell damage .
    Hunter pets do not gain any health from their base stamina, and gain 10.5 health for each additional point of stamina (10 health per additional stamina before the inherent pet health bonus, which was standardized to 5% in Patch 3.1.0).
    1 resistance gives 0.4 resistance.
    1 armor gives 0.35 armor.
    1 point of spell penetration gives 1 point of spell penetration to your pet.[2]
    1 point of resilience gives 1 point of resilience rating to your pet as of Patch 3.3.0. Previously, it was 0.4 points of resilience per 1 point.[3]
	*/
	//static uint32 family_aura[PET_MAX_FAMILY_COUNT] = { 0, 17223, 17210, 17129, 17208, 7000, 17212, 17209, 17211, 17214, 0, 17217, 17220, 0, 0, 0, 0, 0, 0, 0, 17218, 17221, 0, 0, 17206, 17215, 17216, 17222, 0, 0, 34887, 35257, 35254, 35258, 35253, 35386, 50297, 54676, 0, 55192, 55729, 56634, 56635, 58598, 61199, 0 };
		
	//if( pet_family < PET_MAX_FAMILY_COUNT )
	//	RemoveAura( family_aura[ pet_family ] );//If the pet gained a level, we need to remove the auras to re-calculate everything.
//	LoadPetAuras(-1);//These too

	//Base attributes from http://petopia.brashendeavors.net/html/art...ttributes.shtml
//	static uint32 R_pet_base_armor[PLAYER_LEVEL_CAP] = { 15, 16, 41, 76, 120, 174, 239, 316, 406, 512, 538, 573, 608, 642, 677, 713, 748, 782, 817, 852, 888, 922, 957, 992, 1026, 1061, 1097, 1130, 1165, 1200, 1234, 1270, 1304, 1340, 1373, 1480, 1593, 1709, 1834, 1964, 2101, 2246, 2397, 2557, 2725, 2780, 2835, 2888, 2944, 2999, 3052, 3108, 3163, 3216, 3271, 3327, 3380, 3435, 3489, 3791, 4091, 4391, 4691, 4991, 5291, 5591, 5892, 6192, 6492, 6792, 7092, 7392, 7692, 7992, 8292, 8592, 8892, 9192, 9492, 9792,
//	10167,10642,11089,11351,11692,12025,12375,12638,13022,13426,13832,14148,14519,14980,15397,15780,16076,16478,16793,17091,17366,17616,18021,18294,18594,19000,19364,19759,20249,20639,21139,21512,21846,22198,22688,22982,23434,23750,24139,24509,24843,25274,25652,26087,26437,26905,27165,27615,28105,28431,28899,29151,29522,29802,30104,30405,30891};
	static uint32 R_pet_base_str[PLAYER_LEVEL_CAP] = { 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 37, 38, 40, 42, 44, 45, 47, 49, 50, 52, 53, 55, 56, 58, 60, 61, 63, 64, 66, 67, 69, 70, 72, 74, 76, 78, 81, 86, 91, 97, 102, 104, 106, 108, 110, 113, 115, 117, 119, 122, 124, 127, 129, 131, 134, 136, 139, 141, 144, 146, 149, 151, 154, 156, 159, 162, 165, 168, 171, 174, 177, 181, 184, 187, 190, 193, 
	197,201,205,207,211,213,217,219,222,224,227,231,234,236,238,242,244,246,248,252,255,257,260,262,265,267,269,271,273,276,278,280,282,284,287,291,295,299,301,305,309,311,313,316,320,322,325,327,330,332,334,336,338,341,343,346,349};
	static uint32 R_pet_base_agi[PLAYER_LEVEL_CAP] = { 15, 16, 16, 16, 17, 18, 18, 19, 20, 20, 20, 21, 23, 23, 24, 25, 26, 27, 28, 30, 30, 30, 32, 33, 34, 35, 36, 37, 38, 40, 40, 41, 43, 44, 45, 46, 47, 48, 49, 50, 52, 53, 54, 55, 57, 57, 59, 60, 61, 63, 64, 65, 67, 68, 70, 71, 72, 74, 75, 77, 82, 87, 92, 97, 102, 107, 112, 117, 122, 127, 131, 136, 141, 146, 151, 156, 161, 166, 171, 176, 
	180,186,189,193,198,203,208,211,217,223,228,232,236,242,247,252,258,264,270,275,278,282,288,291,294,300,304,308,311,314,319,322,325,328,334,340,345,350,353,357,362,367,371,374,379,385,388,394,399,403,407,413,416,421,427,430,436 };
	static uint32 R_pet_base_sta[PLAYER_LEVEL_CAP] = { 22, 24, 25, 27, 28, 30, 32, 34, 36, 38, 40, 43, 45, 48, 51, 54, 57, 60, 63, 66, 70, 74, 79, 83, 88, 93, 98, 103, 109, 114, 119, 124, 129, 134, 140, 146, 152, 158, 164, 170, 177, 183, 190, 196, 203, 210, 217, 224, 232, 240, 247, 255, 263, 271, 279, 288, 296, 305, 314, 323, 332, 342, 351, 361, 370, 380, 391, 401, 412, 423, 434, 445, 456, 467, 478, 489, 501, 512, 523, 534,
	546,556,569,578,588,599,612,624,635,647,659,669,676,685,692,701,713,726,735,749,759,771,780,789,803,813,822,835,843,853,862,871,885,895,907,918,925,935,942,951,961,971,985,992,1006,1016,1023,1036,1044,1052,1065,1072,1181,1395,1603,2116,2630,};
	static uint32 R_pet_base_int[PLAYER_LEVEL_CAP] = { 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 28, 28, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 30, 30, 31, 31, 31, 32, 32, 32, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43,
	45,46,47,48,49,50,52,54,55,57,59,60,61,63,65,67,69,70,72,73,74,75,76,78,79,81,82,84,85,86,87,88,89,91,93,95,96,97,98,100,102,104,106,108,110,112,113,114,115,116,117,119,120,121,123,124,126};
	static uint32 R_pet_base_spi[PLAYER_LEVEL_CAP] = { 20, 20, 21, 21, 21, 21, 22, 22, 22, 23, 23, 24, 25, 26, 27, 28, 29, 29, 31, 32, 32, 34, 34, 36, 37, 37, 39, 39, 41, 42, 42, 44, 44, 46, 47, 48, 49, 49, 51, 52, 53, 54, 55, 56, 58, 58, 60, 60, 62, 63, 64, 65, 66, 68, 69, 70, 71, 72, 73, 75, 78, 80, 84, 86, 88, 91, 93, 95, 96, 99, 102, 105, 108, 111, 114, 117, 120, 123, 126, 129,
	131,135,137,139,141,144,146,148,150,152,154,158,161,164,168,171,174,177,180,183,187,189,191,194,198,200,203,205,208,211,215,218,222,226,228,230,234,237,240,242,244,247,249,253,256,259,263,267,271,273,276,280,283,285,288,292,295};

//	BaseResistance[0] = R_pet_base_armor[ level - 1 ];
	BaseStats[STAT_STRENGTH] = R_pet_base_str[ level - 1 ];
	BaseStats[STAT_AGILITY] = R_pet_base_agi[ level - 1 ];
	BaseStats[STAT_STAMINA] = R_pet_base_sta[ level - 1 ];
	BaseStats[STAT_INTELLECT] = R_pet_base_int[ level - 1 ];
	BaseStats[STAT_SPIRIT] = R_pet_base_spi[ level - 1 ];

	BaseStats[STAT_STRENGTH] += GetPetOwner()->GetStat( STAT_STRENGTH ) * 15 / 100;
	BaseStats[STAT_AGILITY] += GetPetOwner()->GetStat( STAT_AGILITY ) * 15 / 100;
	BaseStats[STAT_STAMINA] += GetPetOwner()->GetStat( STAT_STAMINA ) * 15 / 100;
	BaseStats[STAT_INTELLECT] += GetPetOwner()->GetStat( STAT_INTELLECT ) * 15 / 100;
	BaseStats[STAT_SPIRIT] += GetPetOwner()->GetStat( STAT_SPIRIT ) * 15 / 100;

	BaseDamage[0] = GetPetOwner()->BaseDamage[0];
	BaseDamage[1] = GetPetOwner()->BaseDamage[1];

	for( int32 i = 0; i < SCHOOL_COUNT; i++ )
		BaseResistance[ i ] = GetPetOwner()->GetResistance( i ) * 70 / 100;

	//4.3.4 changes ? WOWWIKI says these are not valid ?
	if( myFamily )
	{
		if( myFamily->PetType == PET_TALENT_TYPE_FEROCITY ) //Ferocity Pet Scaling
		{
			BaseStats[2] = GetPetOwner()->GetStat( STAT_STAMINA ) * 67 / 100;
			for( int32 i = 0; i < SCHOOL_COUNT; i++ )
				BaseResistance[ i ] = GetPetOwner()->GetResistance( i ) * 60 / 100;
//			BaseDamage[0] = GetPetOwner()->BaseDamage[0] * 40/100;
//			BaseDamage[1] = GetPetOwner()->BaseDamage[1] * 40/100;
			BaseDamage[0] = GetPetOwner()->BaseDamage[0] * 120 /100;
			BaseDamage[1] = GetPetOwner()->BaseDamage[1] * 120 /100;
			OwnerAPContribPCT = 22 - 5;
		}
		else if( myFamily->PetType == PET_TALENT_TYPE_CUNNING )	//Cunning Pet Scaling
		{
			BaseStats[2] = GetPetOwner()->GetStat( STAT_STAMINA ) * 72.5 / 100;
			for( int32 i = 0; i < SCHOOL_COUNT; i++ )
				BaseResistance[ i ] = GetPetOwner()->GetResistance( i ) * 70 / 100;
//			BaseDamage[0] = GetPetOwner()->BaseDamage[0] * 30/100;
//			BaseDamage[1] = GetPetOwner()->BaseDamage[1] * 30/100;
			BaseDamage[0] = GetPetOwner()->BaseDamage[0] * 100 /100;
			BaseDamage[1] = GetPetOwner()->BaseDamage[1] * 100 /100;
			OwnerAPContribPCT = 22 - 0;
		}
		else	//Tenacity Pet Scaling
		{
			BaseStats[2] = GetPetOwner()->GetStat( STAT_STAMINA ) * 78 / 100;
			for( int32 i = 0; i < SCHOOL_COUNT; i++ )
				BaseResistance[ i ] = GetPetOwner()->GetResistance( i ) * 80 / 100;
//			BaseDamage[0] = GetPetOwner()->BaseDamage[0] * 25/100;
//			BaseDamage[1] = GetPetOwner()->BaseDamage[1] * 25/100;
			BaseDamage[0] = GetPetOwner()->BaseDamage[0] * 80 /100;
			BaseDamage[1] = GetPetOwner()->BaseDamage[1] * 80 /100;
			OwnerAPContribPCT = 22 + 5;
		}
	}

	uint32 base_hp = BaseStats[ STAT_STAMINA ] * 10;  //they said pet health is really low

	SetUInt32Value( UNIT_FIELD_BASE_HEALTH, base_hp );
	SetUInt32Value( UNIT_FIELD_MAXHEALTH, base_hp );

	//Family Aura
	if( pet_family >= PET_MAX_FAMILY_COUNT )
		sLog.outError( "PETSTAT: Creature family %i [%s] has missing data.", pet_family, myFamily==NULL?"":myFamily->name );
	//else if( family_aura[ pet_family ] != 0 )
	//	this->CastSpell( this, family_aura[ pet_family ], true );
		
	for( uint32 x = 0; x < 5; ++x )
		CalcStat( x );
	
	for( int32 i = 0; i < SCHOOL_COUNT; i++ )
		CalcResistance( i );

	UpdateSP();
	UpdateAP();
	UpdateSpeed();
	CalcDamage();
	UpdateHaste();
//	LoadPetAuras(-2);//Load all BM auras
}

void Pet::ApplyStatsForLevel()
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	if( Summon )
		ApplySummonLevelAbilities();
	else
		ApplyPetLevelAbilities();

	// Apply common stuff
	// Apply scale for this family.
	if( myFamily )
	{
		float pet_level = float( m_uint32Values[ UNIT_FIELD_LEVEL ] );
		float level_diff = float( myFamily->maxlevel - myFamily->minlevel );
		float scale_diff = float( myFamily->maxsize - myFamily->minsize );
		float factor = scale_diff / level_diff;
		float scale = factor * pet_level + myFamily->minsize; /**/
		scale *= 1.2f;	//people keep saying pets are small
		SetFloatValue( OBJECT_FIELD_SCALE_X, scale );
	}
	else if( proto )
		SetFloatValue(OBJECT_FIELD_SCALE_X,proto->Scale);

	// Apply health fields.
	SetUInt32Value( UNIT_FIELD_HEALTH, m_uint32Values[ UNIT_FIELD_MAXHEALTH ] );
	SetPower( POWER_TYPE_MANA, GetMaxPower( POWER_TYPE_MANA ) );
	SetPower( POWER_TYPE_FOCUS, GetMaxPower( POWER_TYPE_FOCUS ) );	
	SetFloatValue(UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER + POWER_TYPE_FOCUS, -1.0f);
	SetFloatValue(UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER + POWER_TYPE_FOCUS, -1.0f);
}
/*
void Pet::LoadPetAuras(int32 id)
{
	
//	   Talent			   Aura Id
//	Unleashed Fury			8875
//	Thick Hde				19580
//	Endurance Training		19581
//	Bestial Swiftness		19582
//	Bestial Discipline		19589
//	Ferocity				19591
//	Animal Handler			34666
//	Catlike Reflexes		34667
//	Serpent's Swiftness		34675
	
	
	static uint32 mod_auras[9] = { 8875, 19580, 19581, 19582, 19589, 19591, 34666, 34667, 34675 };//Beastmastery Talent's auras.
	InheritSMMods( m_Owner );
	
	if( id == -1 )//unload all
	{
		for( uint32 x = 0; x < 9; ++x )
			RemoveAura( mod_auras[x] );
	}
	else if( id == -2 )//load all
	{
		for( uint32 x = 0; x < 9; ++x )
			CastSpell( this, mod_auras[x], true );
	}
	else if( mod_auras[id] )//reload one
	{
		RemoveAura( mod_auras[id] );
		CastSpell( this, mod_auras[id], true );
	}
}

*/
void Pet::UpdateAP()
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	int32 AP = 0;
	if( m_Owner )
	{
		if( m_Owner->getClass() == WARLOCK )
		{
			int32 dominant_power = MAX( (int32)m_Owner->GetUInt32Value( PLAYER_PET_SPELL_POWER ), m_Owner->GetRAP() );
			dominant_power = MAX( dominant_power, m_Owner->GetAP() );
//			AP += dominant_power * 57 / 100;	//says wowwiki for 3.3.3
			AP = dominant_power;	//spell power will be picked here and that is already nerfed to x% of owner spell power
		}
		else
		{
			AP = MAX( m_Owner->GetRAP(), m_Owner->GetAP() ) * OwnerAPContribPCT / 100;
			//at least owner gets this. Not sure pet should calculate this also
			AP += GetStat( STAT_AGILITY ) * 2;
		}
	}
	if( AP < 0 ) 
		AP = 0;
	SetUInt32Value( UNIT_FIELD_ATTACK_POWER, AP );
}

void Pet::UpdateSP()
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	// hmm, only warlock class or all ?
	if( m_Owner == NULL || ( m_Owner->getClass() != WARLOCK && m_Owner->getClass() != MAGE ) )
		return;

	int32 max = 0;
	for( uint32 i=0;i<SCHOOL_COUNT;i++)
	{
		ModDamageDone[ i ] -= owner_inherited_SP[ i ];
//		owner_inherited_SP[ i ] = m_Owner->GetDamageDoneMod( i ) * 15 / 100;	//wowwiki said 15
//		owner_inherited_SP[ i ] = m_Owner->GetDamageDoneMod( i ) * 22 / 100;	//our spells always sucked :(
//		owner_inherited_SP[ i ] = m_Owner->GetDamageDoneMod( i ) * 40 / 100;	//our spells always sucked :(
		owner_inherited_SP[ i ] = m_Owner->GetDamageDoneMod( i ) * OwnerSPContribPCT / 100;	//our spells always sucked :(
		max = MAX( max, owner_inherited_SP[ i ] );
		ModDamageDone[ i ] += owner_inherited_SP[ i ];
	}
	m_Owner->SetUInt32Value( PLAYER_PET_SPELL_POWER, max );	//maybe we should put a min ?
}

void Pet::UpdateHaste()
{
	if( GetPetOwner() )
	{
		SetFloatValue( UNIT_MOD_CAST_SPEED, GetPetOwner()->GetFloatValue( UNIT_MOD_CAST_SPEED ) );
		SetFloatValue( UNIT_MOD_CAST_HASTE, GetPetOwner()->GetFloatValue( UNIT_MOD_CAST_HASTE ) );
		float PetOnlyMeleeHaste = 100.0f/(100.0f+AuraMeleeHasteSum);
		SetUInt32Value( UNIT_FIELD_BASEATTACKTIME, float2int32( PetOnlyMeleeHaste * 2000 * GetPetOwner()->GetFloatValue( PLAYER_FIELD_MOD_HASTE ) ) );
		SetUInt32Value( UNIT_FIELD_BASEATTACKTIME_1, float2int32( PetOnlyMeleeHaste * 2000 * GetPetOwner()->GetFloatValue( PLAYER_FIELD_MOD_HASTE ) ) );
		SetUInt32Value( UNIT_FIELD_RANGEDATTACKTIME, float2int32( PetOnlyMeleeHaste * 2000 * GetPetOwner()->GetFloatValue( PLAYER_FIELD_MOD_RANGED_HASTE ) ) );
	}
}

uint32 Pet::CanLearnSpell( SpellEntry * sp )
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	// level requirement
	if( getLevel() < sp->SpellLevel.spellLevel )
	{ 
		return SPELL_FAILED_LEVEL_REQUIREMENT;
	}
	
	return 0;
}
/*
HappinessState Pet::GetHappinessState() 
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	//gets happiness state from happiness points
	uint32 pts = GetPower( POWER_TYPE_HAPPINESS );
	if( pts < PET_HAPPINESS_UPDATE_VALUE )
	{ 
		return UNHAPPY;
	}
	else if( pts >= PET_HAPPINESS_UPDATE_VALUE << 1 )
	{ 
		return HAPPY;
	}
	return CONTENT;
}*/

AI_Spell * Pet::HandleAutoCastEvent()
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	list<AI_Spell*>::iterator itr,itr2;
	bool chance = true;
	uint32 size = 0;

	for(uint32 itr2=0;itr2<AUTOCAST_EVENT_LIST_MAXLEN;itr2++)
		if( m_autoCastSpells[ AUTOCAST_EVENT_ATTACK ][ itr2 ] != NULL )
			size++;

	if( size != 0 )
		for(uint32 itr2=0;itr2<AUTOCAST_EVENT_LIST_MAXLEN;itr2++)
			if( m_autoCastSpells[ AUTOCAST_EVENT_ATTACK ][ itr2 ] != NULL )
			{
				AI_Spell *sp = m_autoCastSpells[ AUTOCAST_EVENT_ATTACK ][ itr2 ];
				chance = RandChance( 100 / size );		
				if( chance 
					&& getMSTime() >= sp->cooldowntime 
					&& GetPower( sp->spell->powerType ) >= (int32)sp->spell->PowerEntry.manaCost 
					&& ( sp->spell->GetCasterAuraState() == 0 || HasAuraStateFlag( sp->spell->GetCasterAuraState() ) )
					)
					return m_autoCastSpells[ AUTOCAST_EVENT_ATTACK ][ itr2 ];
			}

	return NULL;
}

void Pet::HandleAutoCastEvent( AutoCastEvents Type )
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	list<AI_Spell*>::iterator itr, it2;
	AI_Spell * sp;
	if( m_Owner == NULL )
	{ 
		return;
	}

	if( Type >= AUTOCAST_EVENT_COUNT )
	{
		return;
	}

	if( Type == AUTOCAST_EVENT_ATTACK )
	{
		for( uint32 itr = 0; itr < AUTOCAST_EVENT_LIST_MAXLEN; itr++ )
			if( m_autoCastSpells[ Type ][ itr ]
				&& m_autoCastSpells[ Type ][ itr ]->cooldowntime < getMSTime()
			)
			{
				m_aiInterface->SetNextSpell( m_autoCastSpells[ Type ][ itr ] );
				break;
			}
		return;
	}

	for( uint32 itr = 0; itr < AUTOCAST_EVENT_LIST_MAXLEN; itr++  )
		if( m_autoCastSpells[ Type ][ itr ] 
			&& m_autoCastSpells[ Type ][ itr ]->cooldowntime < getMSTime()
		)
		{
			sp = m_autoCastSpells[ Type ][ itr ];

			if( sp->spelltargetType == TTYPE_OWNER )
			{
				if( !m_Owner->HasAura( sp->spell->Id ) )
					CastSpell( m_Owner, sp->spell, false );
			}
			else
			{
				//modified by Zack: Spell targetting will be generated in the castspell function now.You cannot force to target self all the time
				CastSpell( (Unit*)NULL , sp->spell, false);
			}

			sp->cooldowntime = getMSTime() + sp->cooldown;	//at least a GCD
		}
}

void Pet::SetAutoCast(AI_Spell * sp, bool on)
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	ASSERT(sp != NULL);
	ASSERT(sp->spell != NULL);

	if( sp->autocast_type < AUTOCAST_EVENT_COUNT )
	{
		if(!on)
		{
			for(uint32 itr2=0;itr2<AUTOCAST_EVENT_LIST_MAXLEN;itr2++)
				if( m_autoCastSpells[ sp->autocast_type ][ itr2 ] == sp )
				{
					m_autoCastSpells[ sp->autocast_type ][ itr2 ] = NULL;
					break;
				}
		}
		else
		{
			int32 firstempty = -1;
			for(uint32 itr2=0;itr2<AUTOCAST_EVENT_LIST_MAXLEN;itr2++)
				if( m_autoCastSpells[ sp->autocast_type ][ itr2 ] == sp )
					return;
				else if( m_autoCastSpells[ sp->autocast_type ][ itr2 ] == NULL && firstempty == -1 )
					firstempty = itr2;
			if( firstempty != -1 )
				m_autoCastSpells[sp->autocast_type][firstempty] = sp;
		}
	}
}
uint32 Pet::GetUntrainCost()
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	uint32 days = (uint32)( sWorld.GetGameTime() - reset_time ) / 60 * 60 * 24;

	if( reset_cost < 1000 || days > 0 )
		reset_cost = 1000;
	else if( reset_cost < 5000 )
		reset_cost = 5000;
	else if( reset_cost < 10000 )
		reset_cost = 10000;
	else
		reset_cost = reset_cost + 10000 > 100000 ? 100000 : reset_cost + 10000;

	return reset_cost;
}

Group *Pet::GetGroup()
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
 
	if( m_Owner ) 
	{ 
		return m_Owner->GetGroup();
	}
	return NULL;
}

void Pet::SkillUp(bool remove_lower_levels)
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	if( myFamily == NULL )
		return;

	if( IsInWorld() )
	{
		if( HasAuraWithNameHash( SPELL_HASH_AVOIDANCE, 0, AURA_SEARCH_POSITIVE | AURA_SEARCH_PASSIVE ) == NULL )
		{
			if( Summon == false )
				CastSpell( this, 65220, true ); //hunter
			else
				CastSpell( this, 32233, true );	//warlock
		}
	}

	uint32 rowcount = dbcSkillLineSpell.GetNumRows();
	skilllinespell* sls;
	// Pets increase in spell ranks as they level up
	for( uint32 idx = 0; idx < rowcount; ++idx )
	{
		sls = dbcSkillLineSpell.LookupRow( idx );
		if( sls->skillId == myFamily->skilline 
			&& sls->learnOnGetSkill == 2
			) 
		{
			SpellEntry* sp1 = dbcSpell.LookupEntryForced( sls->spellId );
			SpellEntry* sp2 = dbcSpell.LookupEntryForced( sls->forward_spellid );
			if( sp1 != NULL //outdated data ?
				&& ((sp1->c_is_flags & SPELL_FLAG_IS_CONDITIONAL_PASSIVE_CAST)==0)
				&& sp1->talent_entry[0] == NULL //do not learn talents 
				&& sp1->SpellLevel.baseLevel <= getLevel() //we should be able to have this spell
				&& ( sp2 == NULL //last or only rank
					|| sp2->SpellLevel.baseLevel>getLevel() ) // next level is not reachable
//				&& (sp1->RecoveryTime != 0 || sp1->CategoryRecoveryTime != 0 )	//seems to be always 0 :S
				&& mSpells.find( sp1 ) == mSpells.end()	//no need to float add it
				&& strstr( sp1->Name, "DND" ) == NULL
				&& strstr( sp1->Name, "Scaling" ) == NULL
				)
			{
				AddSpell( sp1, remove_lower_levels, false );
			}
//			else
//			{
//				printf("!\t t=%u,bl=%u,rt=%u,cr=%u,have it %u, name %s\n",sp1->talent_entry[0],sp1->SpellLevel.baseLevel,sp1->RecoveryTime,sp1->CategoryRecoveryTime,mSpells.find( sp1 ) != mSpells.end(),sp1->Name);
//				if(sp2)
//					printf("!\t\t bl=%u\n",sp2->SpellLevel.baseLevel);
//			}
		}
	}
}

// returns amount of spent talent points
uint8 Pet::GetSpentTalentPoints() 
{ 
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	uint32 total = 0;
	for( std::map<uint32, uint8>::iterator itr2 = m_talents.begin(); itr2 != m_talents.end(); ++itr2 )
		total += itr2->second;
	return total;
}	

int8 Pet::GetUnspentTalentPoints()
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
 
	if( m_Owner )
	{ 
		return MAX( 0, ( GetTalentPointsForLevel( getLevel() ) + m_Owner->pet_extra_talent_points) - GetSpentTalentPoints() );  
	}
	return GetByte( UNIT_FIELD_BYTES_1, 1 ); 
}

void Pet::DecreaseTalentPoints()
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
 
	//if we are out of
	if( GetByte( UNIT_FIELD_BYTES_1, 1 ) > 0 )
		SetByte( UNIT_FIELD_BYTES_1, 1, GetByte( UNIT_FIELD_BYTES_1, 1 ) - 1 );
	else if( m_Owner && m_Owner->pet_extra_talent_points > 0 )
		m_Owner->pet_extra_talent_points--;
}

// sets talent points
ARCEMU_INLINE void Pet::SetTalentPoints( int8 TP ) 
{ 
	if( Summon )
		return;
	if( TP >= 0 )
		SetByte( UNIT_FIELD_BYTES_1, 1, TP ); 
	else if( m_Owner && m_Owner->pet_extra_talent_points > -TP )
	{
		m_Owner->pet_extra_talent_points += TP;
		SetByte( UNIT_FIELD_BYTES_1, 1, 0 ); 
	}
	//both would go negative
	else if( m_Owner )
	{
		m_Owner->pet_extra_talent_points = 0;
		SetByte( UNIT_FIELD_BYTES_1, 1, 0 ); 
	}
}	

void Pet::fill_talent_info_in_packet(WorldPacket &data)
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	if( !GetPetOwner() )
	{ 
		return;
	}
/*
13329
{SERVER} Packet: (0x6676) SMSG_TALENTS_INFO PacketSize = 56 TimeStamp = 9443765
01 
00 00 00 00 free count
0A count ?
3B 08 00 00 01 41 08 00 00 01 50 08 00 00 01 
4D 08 00 00 02 3F 08 00 00 00 6C 08 00 00 00 
51 08 00 00 02 6B 08 00 00 00 69 08 00 00 00 
6D 08 00 00 00 
01 
00 00 00 00 
09 
3B 08 00 00 01 
3D 08 00 00 00 4D 08 00 00 02 50 08 00 00 00 67 08 00 00 00 3A 08 00 00 02 51 08 00 00 02 6D 08 00 00 00 6B 08 00 00 00
*/
	//the "1" is already sent !!!!
//	data << uint8(1);	//pets just use update type all the time
	uint8 count = 0;
	data << uint32( GetUnspentTalentPoints() ); // Unspent talents
	data << uint8(m_talents.size());
	for( std::map<uint32, uint8>::iterator itr2 = m_talents.begin(); itr2 != m_talents.end(); ++itr2 )
	{
		data << uint32(itr2->first); // talentid?
		data << uint8(itr2->second - 1); // rank?
	}
//	GetPetOwner()->GetSession()->SendPacket(&data);
}

void Pet::WipeTalent(SpellEntry * sp,uint32 loop_count)
{
	if( sp == NULL )
		return;
	if( loop_count > 0 )
		return;
	RemoveSpell( sp->Id );
	RemoveAura( sp->Id, 0, AURA_SEARCH_ALL, MAX_PASSIVE_AURAS );
	//some 
	if( m_Owner )
	{
		m_Owner->removeSpell( sp->Id, false, false, 0 );
		m_Owner->RemoveAura( sp->Id, 0, AURA_SEARCH_ALL, MAX_PASSIVE_AURAS );
	}
	for(uint32 i=0;i<MAX_SPELL_EFFECT_COUNT;i++)
		if( sp->eff[i].EffectTriggerSpell )
			WipeTalent( dbcSpell.LookupEntryForced( sp->eff[i].EffectTriggerSpell ), loop_count + 1);
	if( sp->chained_cast )
		WipeTalent( sp->chained_cast, loop_count + 1);
}

void Pet::WipeTalents()
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	std::map<uint32, uint8>::iterator itr;
	for(itr = m_talents.begin(); itr != m_talents.end(); ++itr)
	{
		TalentEntry *te = dbcTalent.LookupEntryForced( itr->first );
		if( !te )
			continue;
		WipeTalent( dbcSpell.LookupEntryForced( te->RankID[ itr->second ] ), 0 );
	}
	m_talents.clear();
	SetTalentPoints( GetUnspentTalentPoints() );
	SendSpellsToOwner();
}

void Pet::LearnTalent(uint32 talentid, uint32 requested_rank, bool isPreviewed )
{
	uint32 CurTalentPoints = GetUnspentTalentPoints();

	uint32 TP_needed;
	if( isPreviewed == false )
		TP_needed = 1;
	else 
		TP_needed = requested_rank + 1;

	if( CurTalentPoints < TP_needed )
	{ 
		sLog.outDebug("PetLearnTalent: Have not enough talentpoints to spend");
		return;
	}

	if (requested_rank > 4)
	{ 
		sLog.outDebug("PetLearnTalent: Requested rank is greater then 4");
		return;
	}

	//check previous rank
	if( isPreviewed == false && requested_rank > 0 && m_talents[ talentid ] == 0 )
	{
		sLog.outDebug("PetLearnTalent: Missing Lower Rank");
		return;
	}

	// find talent and spell
	TalentEntry *te = dbcTalent.LookupEntryForced( talentid );
	if( te == NULL )
	{ 
		sLog.outDebug("PetLearnTalent: Invalid Talent");
		return;
	}

	//check talent dependency
	TalentEntry *dep = dbcTalent.LookupEntryForced( te->DependsOn );
	if( te->DependsOn && dep == NULL )
	{ 
		sLog.outDebug("PetLearnTalent: Dependency talent does not exist");
		return;
	}
	else if( te->DependsOn )
	{
		if( m_talents[ te->DependsOn ] < te->DependsOnRank )
		{ 
			sLog.outDebug("PetLearnTalent: Dependency talent does not have required rank");
			return;
		}
	}

	SpellEntry* sp = dbcSpell.LookupEntry( te->RankID[ requested_rank ] );
	if( sp != NULL )
	{
		if( GetPetOwner() && GetPetOwner()->GetSession() )
			GetPetOwner()->GetSession()->OutPacket( SMSG_PET_LEARNED_SPELL, 4, &sp->Id );
		SetTalentPoints( CurTalentPoints - TP_needed );
		AddPetTalent( talentid, requested_rank + 1 ); //+1 to avoid 0 checks
		AddSpell( sp, true );
	}
}

void Pet::AddPetTalent(uint32 talent_entry, uint8 rank )
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	if( m_talents[ talent_entry ] )
	{
		TalentEntry *te = dbcTalent.LookupEntryForced( talent_entry );
		if( te )
		{
			for( uint32 i = 0; i < rank; i++ )
			{
				RemoveSpell( te->RankID[ i ] );
				RemoveAura( te->RankID[ i ] );
			}
		}
		m_talents[ talent_entry ] = rank;
	}
	else
//		m_talents.insert( make_pair( talent_entry, rank ) );
		m_talents[ talent_entry ] = rank;
}

//petnumber is bytes 3,4,5
void Pet::SetPetNumber(uint8 pet_number)
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	SetUInt32Value( UNIT_FIELD_PETNUMBER, pet_number );
	SetUInt32Value( OBJECT_FIELD_GUID, ( GetUInt32Value( OBJECT_FIELD_GUID ) & 0x00FFFFFF ) | ( (uint32)pet_number << 24 ) ); 
}

void Pet::SetPetState(uint32 state)
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
	m_State = state; 
	//make sure we guess what the owner wants. If he wants us to do nothing then let us do nothing
	if( GetAIInterface() && ( state == PET_STATE_PASSIVE || state == PET_STATE_DEFENSIVE ) )
	{
		GetAIInterface()->HandleEvent( EVENT_LEAVECOMBAT, NULL, 0);
	}
}

void Pet::RegenerateMana()
{
	INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE
   
	if( m_Owner == NULL )
	{
		return;
	}

	uint32 cur=GetPower( POWER_TYPE_MANA );
	uint32 mm=GetMaxPower( POWER_TYPE_MANA );
	if(cur>=mm)
	{ 
		return;
	}
	
	float amt;
	if (m_interruptRegen)
		amt = m_Owner->GetFloatValue(UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER) * 20 / 100;
	else
		amt = m_Owner->GetFloatValue(UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER) * 20 / 100;

	//Apply shit from conf file
	amt*=sWorld.getRate(RATE_POWER1);
	if(amt<=1.0f && amt>0.0f)//this fixes regen like 0.98
		cur++;
	else
		cur+=(uint32)amt;
	SetPower( POWER_TYPE_MANA,(cur>=mm)?mm:cur);
}

void Pet::Cooldown_Add( SpellEntry *pSpell )
{
	AI_Spell * sp = GetAISpellForSpellId( pSpell->Id );
	if( sp != NULL )
	{
//		int32 cool_time = pSpell->RecoveryTime;
		int32 cool_time = sp->cooldown;
		if( cool_time > 0 ) //some spells only have casttime as cooldown
		{
			if( Need_SM_Apply(pSpell) && SM_Mods)
			{
				SM_FIValue(SM_Mods->SM_FCooldownTime, &cool_time, pSpell->GetSpellGroupType());
				SM_PIValue(SM_Mods->SM_PCooldownTime, &cool_time, pSpell->GetSpellGroupType());
			}
		}
		sp->cooldowntime = getMSTime() + cool_time;
	}
}

int32 Pet::CooldownCanCast( SpellEntry *entry )
{
	AI_Spell * sp = GetAISpellForSpellId( entry->Id );
	if( sp != NULL )
	{
		// Check the cooldown
		if( sp->cooldowntime > 0 && getMSTime() < sp->cooldowntime )
			return ( sp->cooldowntime - getMSTime() );
	}
	return 0;
}

void Pet::ClearSpellCooldowns()
{
	SimplePointerListNode< AI_Spell > *itr;
	for( itr = m_AISpellStore2.begin(); itr != m_AISpellStore2.end(); itr = itr->next )
		if( itr->data->cooldowntime > itr->data->cooldown )
			itr->data->cooldowntime -= itr->data->cooldown;
	if( GetPetOwner() && GetPetOwner()->GetSession() )
		for( itr = m_AISpellStore2.begin(); itr != m_AISpellStore2.end(); itr = itr->next )
		{
//			GetPetOwner()->ClearCooldownForSpell( itr->data->spell->Id );
			sStackWorldPacket( data,SMSG_CLEAR_COOLDOWN, 16);
			data << uint32( itr->data->spell->Id ) << GetGUID();
			GetPetOwner()->GetSession()->SendPacket(&data);
		}
}
