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

Standing Player::GetReputationRankFromStanding( int32 Standing_ )
{
	if( Standing_ >= 42000 )  
	{ 
		return STANDING_EXALTED;  
	}
	else if( Standing_ >= 21000 )  
	{ 
		return STANDING_REVERED;  
	}
	else if( Standing_ >= 9000 )  
	{ 
		return STANDING_HONORED;  
	}
	else if( Standing_ >= 3000 )  
	{ 
		 return STANDING_FRIENDLY;  
	}
	else if( Standing_ >= 0 )  
	{ 
		return STANDING_NEUTRAL;  
	}
	else if( Standing_ > -3000 )  
	{ 
		return STANDING_UNFRIENDLY;  
	}
	else if( Standing_ > -6000 )  
	{ 
		return STANDING_HOSTILE;  
	}
	return STANDING_HATED;  
}

ARCEMU_INLINE bool CanToggleAtWar( uint8 flag ) { return ( flag & FACTION_FLAG_DISABLE_ATWAR ) == 0 ; }
/*
ARCEMU_INLINE bool CanToggleAtWar( uint32 Faction ) 
{ 
	FactionDBC * factdbc ;
	factdbc = dbcFaction.LookupEntryForced( Faction );
	if ( factdbc == NULL || factdbc->RepListId < 0 ) 
		continue;
	if( factdbc->repFlags & FACTION_FLAG_AT_WAR )
		return true;
	return false;
}*/
ARCEMU_INLINE bool AtWar( uint8 flag ) { return ( flag & FACTION_FLAG_AT_WAR ) != 0; }
ARCEMU_INLINE bool ForcedInvisible( uint8 flag ) { return ( flag & FACTION_FLAG_FORCED_INVISIBLE ) != 0; }
ARCEMU_INLINE bool Visible( uint8 flag ) { return ( flag & FACTION_FLAG_VISIBLE ) != 0; }
ARCEMU_INLINE bool Hidden( uint8 flag ) { return ( flag & FACTION_FLAG_HIDDEN ) != 0; }
ARCEMU_INLINE bool Inactive( uint8 flag ) { return ( flag & FACTION_FLAG_INACTIVE ) != 0; }

ARCEMU_INLINE bool SetFlagAtWar( uint8 &flag, bool set )
{
	if ( set && !AtWar( flag ) )
		flag |= FACTION_FLAG_AT_WAR;
	else if ( !set && AtWar( flag ) )
		flag &= ~FACTION_FLAG_AT_WAR;
	else
		return false;

	return true;
}

ARCEMU_INLINE bool SetFlagVisible( uint8 &flag, bool set )
{
	if ( ForcedInvisible( flag ) || Hidden( flag ) )
	{ 
		return false;
	}
	else if ( set && !Visible( flag ) )
		flag |= FACTION_FLAG_VISIBLE;
	else if ( !set && Visible( flag ) )
		flag &= ~FACTION_FLAG_VISIBLE;
	else
		return false;

	return true;
}

ARCEMU_INLINE bool SetFlagInactive( uint8 &flag, bool set )
{
	if ( set && !Inactive( flag ) )
		flag |= FACTION_FLAG_INACTIVE;
	else if ( !set && Inactive( flag ) )
		flag &= ~FACTION_FLAG_INACTIVE;
	else
		return false;

	return true;
}

ARCEMU_INLINE bool RankChanged( int32 Standing, int32 Change )
{
	return ( Player::GetReputationRankFromStanding( Standing ) != Player::GetReputationRankFromStanding( Standing + Change ) );
}

ARCEMU_INLINE bool RankChangedFlat( int32 Standing, int32 NewStanding )
{
	return ( Player::GetReputationRankFromStanding( Standing ) != Player::GetReputationRankFromStanding( NewStanding ) );
}

void Player::smsg_InitialFactions()
{
	WorldPacket data( SMSG_INITIALIZE_FACTIONS, 644 );
	data << uint32( 128 );
	FactionReputation * rep;
	for ( uint32 i = 0; i < 128; ++i )
	{
		rep = reputationByListId[i];
		if ( rep == NULL )
			data << uint8(0) << uint32(0);
		else
			data << rep->flag << rep->CalcStanding();
	}
	m_session->SendPacket(&data);
}

void Player::_InitialReputation()
{
	FactionDBC * f;
	for ( uint32 i = 0; i < dbcFaction.GetNumRows(); i++ )
	{
		f = dbcFaction.LookupRow( i );
		AddNewFaction( f, 0, true );
	}
}

int32 Player::GetStanding( uint32 Faction )
{
	ReputationMap::iterator itr = m_reputation.find( Faction );
	if ( itr != m_reputation.end() )
	{ 
		return itr->second->standing;
	}
	return 0;
}

int32 Player::GetBaseStanding( uint32 Faction )
{
	ReputationMap::iterator itr = m_reputation.find( Faction );
	if ( itr != m_reputation.end() )
	{ 
		return itr->second->baseStanding;
	}
	return 0;
}

void Player::SetStanding( uint32 Faction, int32 Value, int32 inloop )
{
	const int32 minReputation = -42000;      //   0/36000 Hated
	const int32 exaltedReputation = 42000;   //   0/1000  Exalted
	const int32 maxReputation = 42999;       // 999/1000  Exalted
	int32 newValue = Value;
	FactionDBC * f = dbcFaction.LookupEntryForced( Faction );
	if ( f == NULL
		|| f->RepListId < 0 // Zack : why can't we just have any faction ? These are mostly MOB related faction. #care
		)
	{
		sLog.outDebug("Cannot set standing. Faction is missing\n");
		return;
	}
	ReputationMap::iterator itr = m_reputation.find( Faction );

	if( newValue < minReputation )
		newValue = minReputation;
	else if( newValue > maxReputation )
		newValue = maxReputation;

	{
		uint32 standing = GetReputationRankFromStanding( newValue );
		Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_GAIN_REPUTATION,Faction,ACHIEVEMENT_UNUSED_FIELD_VALUE,newValue,ACHIEVEMENT_EVENT_ACTION_SET);
		Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_KNOWN_FACTIONS,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,(uint32)m_reputation.size(),ACHIEVEMENT_EVENT_ACTION_SET_MAX);
		if( standing >= STANDING_EXALTED )
		{
			ReputationMap::iterator itr1;
			uint32 exaltedcount=0;
			for( itr1 = m_reputation.begin();itr1 != m_reputation.end();itr1++)
				if( GetReputationRankFromStanding( itr1->second->standing ) >= STANDING_EXALTED )
					exaltedcount++;
			Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_GAIN_EXALTED_REPUTATION,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,exaltedcount,ACHIEVEMENT_EVENT_ACTION_SET_MAX);
		}
		if( standing >= STANDING_REVERED )
		{
			ReputationMap::iterator itr1;
			uint32 reveredcount=0;
			for( itr1 = m_reputation.begin();itr1 != m_reputation.end();itr1++)
				if( GetReputationRankFromStanding( itr1->second->standing ) >= STANDING_REVERED )
					reveredcount++;
			Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_GAIN_REVERED_REPUTATION,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,reveredcount,ACHIEVEMENT_EVENT_ACTION_SET_MAX);
		}
	}

	if( itr == m_reputation.end() )
	{
		if ( !AddNewFaction( f, newValue, false ) )
		{ 
			itr = m_reputation.find( Faction );
			if( itr != m_reputation.end() ) // wtf check
//				UpdateInrangeSetsBasedOnReputation();
				OnModStanding( f, itr->second, Value );
			if( inloop == 0 )
				ChainedModStanding( Faction, Value );
			return;
		}
	}
	else
	{
		// Increment it.
		if ( RankChangedFlat( itr->second->standing, newValue ) )
		{
			itr->second->standing = newValue;
//			UpdateInrangeSetsBasedOnReputation();
		}
		else
			itr->second->standing = newValue;
		
		OnModStanding( f, itr->second, Value );
		if( inloop == 0 )
			ChainedModStanding( Faction, Value );
	}
}

Standing Player::GetStandingRank( uint32 Faction )
{
	return Standing( GetReputationRankFromStanding( GetStanding( Faction ) ) );
}

bool Player::IsHostileBasedOnReputation( FactionDBC * dbc )
{
	if( dbc->RepListId < 0 || dbc->RepListId >= 128 )
	{ 
		return false;
	}

	FactionReputation * rep = reputationByListId[dbc->RepListId];
	if ( rep == NULL )
	{ 
		return false;
	}

	// forced reactions take precedence
	map<uint32, uint32>::iterator itr = m_forcedReactions.find( dbc->ID );
	if( itr != m_forcedReactions.end() )
	{ 
		return ( itr->second <= STANDING_HOSTILE );
	}

	return ( AtWar( rep->flag ) || GetReputationRankFromStanding( rep->standing ) <= STANDING_HOSTILE );
}

void Player::ModStanding( uint32 Faction, int32 Value, int32 inloop )
{
	int32 minReputation = -42000;			//   0/36000 Hated
	const int32 exaltedReputation = 42000;   //   0/1000  Exalted
	const int32 maxReputation = 42999;       // 999/1000  Exalted
	int32 newValue = Value;

	//spells may redirect reputaion gain in dungeons
	if( dungeon_factiongain_redirect && Value > 0 && ( IS_MAP_RAID( GetMapMgr()->GetMapInfo() ) || IS_MAP_DUNGEON( GetMapMgr()->GetMapInfo() ) ) )
		Faction = dungeon_factiongain_redirect;

	FactionDBC * f = dbcFaction.LookupEntry( Faction );
	if ( f == NULL 
		|| f->RepListId < 0 //Zack : mobs have these
		)
		return;
	ReputationMap::iterator itr = m_reputation.find( Faction );

	if ( itr == m_reputation.end() )
	{
		if( newValue < minReputation )
			newValue = minReputation;
		else if( newValue > maxReputation )
			newValue = maxReputation;

		if ( !AddNewFaction( f, newValue, false ) )
		{ 
			itr = m_reputation.find( Faction );
//			UpdateInrangeSetsBasedOnReputation();
			if( itr != m_reputation.end() )
				OnModStanding( f, itr->second, Value );
			if( inloop == 0 )
				ChainedModStanding( Faction, Value );
			return;
		}
	}
	else
	{
		if( pctReputationMod > 0 )
		{
			float d = float( float( pctReputationMod ) / 100.0f );
			newValue = newValue + float2int32( float( float( newValue ) * d ) );
		}

		// Increment it.
		if ( RankChanged( itr->second->standing, newValue ) )
		{
			itr->second->standing += newValue;
//			UpdateInrangeSetsBasedOnReputation();
		}
		else
		{
			itr->second->standing += newValue;
		}
		
		if( CanToggleAtWar( itr->second->flag ) == false )
			minReputation = 0;

		if(itr->second->standing < minReputation)
			itr->second->standing = minReputation;
		else if(itr->second->standing > maxReputation)
			itr->second->standing = maxReputation;

		OnModStanding( f, itr->second, Value );
		if( inloop == 0 )
			ChainedModStanding( Faction, Value );
   }
}

void Player::SetAtWar( uint32 Faction, bool Set )
{
	if( Faction >= 128 )
	{ 
		return;
	}

	FactionReputation * rep = reputationByListId[Faction];
	if( rep == NULL )
	{ 
		return;
	}

	if( GetReputationRankFromStanding( rep->standing ) <= STANDING_HOSTILE && !Set ) // At this point we have to be at war.
	{ 
		return;
	}
	
	if( !CanToggleAtWar( rep->flag ) )
	{ 
		return;
	}

	if( SetFlagAtWar( rep->flag, Set ) )
	{
//		UpdateInrangeSetsBasedOnReputation();
	}
}

void WorldSession::HandleSetAtWarOpcode( WorldPacket& recv_data )
{
	uint32 id;
	uint8 state;
	recv_data >> id >> state;

	_player->SetAtWar( id, ( state == 1 ) );
}

/*
void Player::UpdateInrangeSetsBasedOnReputation()
{
	// This function assumes that the opp faction set for player = the opp faction set for the unit.
	InRangeSet::iterator itr;
	Unit * pUnit;
	bool rep_value;
	bool enemy_current;
	for( itr = m_objectsInRange.begin(); itr != m_objectsInRange.end(); itr++ )
	{
		if( (*itr)->GetTypeId() != TYPEID_UNIT )
			continue;

		pUnit = SafeUnitCast( *itr );
		if(pUnit->m_factionDBC == NULL || pUnit->m_factionDBC->RepListId < 0)
			continue;

		rep_value = IsHostileBasedOnReputation( pUnit->m_factionDBC );
		enemy_current = IsInRangeOppFactSet( pUnit );

		if( rep_value && !enemy_current ) // We are now enemies.
			m_oppFactsInRange.insert( pUnit );
		else if( !rep_value && enemy_current )
			m_oppFactsInRange.erase( pUnit );
	}
}*/

void Player::Reputation_OnKilledUnit( Unit * pUnit, bool InnerLoop )
{
	// add rep for on kill
	if ( pUnit->GetTypeId() != TYPEID_UNIT || pUnit->IsPet() )
	{ 
		return;
	}

	Group * m_Group = m_playerInfo->m_Group;
	if ( !InnerLoop && m_Group )
	{
		/* loop the rep for group members */
		m_Group->getLock().Acquire();
		GroupMembersSet::iterator it;
		for ( uint32 i = 0; i < m_Group->GetSubGroupCount(); i++ )
		{
			for ( it = m_Group->GetSubGroup(i)->GetGroupMembersBegin(); it != m_Group->GetSubGroup(i)->GetGroupMembersEnd(); ++it )
			{
				if ( (*it)->m_loggedInPlayer && (*it)->m_loggedInPlayer->isInRange( this, 100.0f ) )
					(*it)->m_loggedInPlayer->Reputation_OnKilledUnit( pUnit, true );
			}
		}
		m_Group->getLock().Release();
		return;
	}

	int team = GetTeam();
	ReputationModifier * modifier = objmgr.GetReputationModifier( pUnit->GetEntry(), pUnit->m_factionDBC->ID );
	if( modifier != 0 )
	{
		// Apply this data.
		for( vector<ReputationMod>::iterator itr = modifier->mods.begin(); itr != modifier->mods.end(); itr++ )
		{
			if ( !(*itr).faction[team] )
				continue;

			/* rep limit? */
			if ( !IS_INSTANCE( GetMapId() ) || ( IS_INSTANCE( GetMapId() ) && IS_HEROIC_INSTANCE_DIFFICULTIE( GetInstanceDifficulty() ) ) )
			{
				if ( (*itr).replimit )
				{
					if ( GetStanding( (*itr).faction[team] ) >= (int32)(*itr).replimit )
						continue;
				}
			}
			ModStanding( itr->faction[team], int32( float( itr->value ) * sWorld.getRate( RATE_KILLREPUTATION ) ) );
		}
	}
	else
	{
		if ( IS_INSTANCE( GetMapId() ) && objmgr.HandleInstanceReputationModifiers( this, pUnit ) )
		{ 
			return;
		}

		if ( pUnit->m_factionDBC->RepListId < 0 )
		{ 
			return;
		}

		int32 change = int32( -5.0f * sWorld.getRate( RATE_KILLREPUTATION ) );
		ModStanding( pUnit->m_factionDBC->ID, change );
	}
}

void Player::Reputation_OnTalk( FactionDBC * dbc )
{
	// set faction visible if not visible
	if ( dbc == NULL || dbc->RepListId < 0 )
	{ 
		return;
	}

	FactionReputation * rep = reputationByListId[dbc->RepListId];
	if ( rep == NULL )
	{ 
		return;
	}

	if ( SetFlagVisible( rep->flag, true ) && IsInWorld() )
		m_session->OutPacket( SMSG_SET_FACTION_VISIBLE, 4, &dbc->RepListId );
}

void Player::SetFactionInactive( uint32 faction, bool set )
{
	FactionReputation * rep = reputationByListId[faction];
	if ( rep == NULL )
	{ 
		return;
	}

	if ( SetFlagInactive( rep->flag, set ) )
	{
	}
}

void WorldSession::HandleSetFactionInactiveOpcode( WorldPacket & recv_data )
{
	uint32 id;
	uint8 inactive;
	recv_data >> id >> inactive;

	_player->SetFactionInactive( id, ( inactive == 1 ) );
}

bool Player::AddNewFaction( FactionDBC * dbc, int32 standing, bool base ) // if ( base ) standing = baseRepValue
{
	if ( dbc == NULL 
		|| dbc->RepListId < 0 // Zack : mobs have these
		)
	{
		return false;
	}
	uint32 RaceMask = getRaceMask();
	uint32 ClassMask = getClassMask();
	for ( uint32 i = 0; i < 4; i++ )
	{
		if( ( (dbc->RaceMask[i] & RaceMask) || ( dbc->RaceMask[i] == 0 && dbc->ClassMask[i] != 0 ) ) &&
			( (dbc->ClassMask[i] & ClassMask) || dbc->ClassMask[i] == 0 ) )
		{
			FactionReputation * rep = new FactionReputation;
			rep->flag = dbc->repFlags[i];
			rep->baseStanding = dbc->baseRepValue[i];
			rep->standing = ( base ) ? dbc->baseRepValue[i] : standing;
			m_reputation[dbc->ID] = rep;
			if( dbc->RepListId >= 0 )
				reputationByListId[dbc->RepListId] = rep;

			if ( SetFlagVisible( rep->flag, true ) && IsInWorld() )
			{
				m_session->OutPacket( SMSG_SET_FACTION_VISIBLE, 4, &dbc->RepListId );
			}

			Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_KNOWN_FACTIONS,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
			return true;
		}
	}

	return false;
}

void Player::ChainedModStanding( uint32 Faction, int32 Value)
{
	//try to mod standing based on rep change chain list
	SimpleVect<ReputationToReputationMod *> *RepChangeChainList = objmgr.GetRepChainedModList( Faction );
	if( RepChangeChainList )
	{
		int32 old_value = GetStanding( Faction );
		for( uint32 i=0;i<RepChangeChainList->GetMaxSize();i++)
		{
			ReputationToReputationMod *mod = RepChangeChainList->GetValue( i );
			int32 new_mod = (int32)( Value * mod->mod_factor ) + mod->mod_flat;
			if( mod->target_rep_limit )
			{
				int32 new_value = old_value + new_mod;
				if( Value < 0 )
				{ 
					if( old_value <= mod->target_rep_limit )
						continue;		// value is already lower than allowed one
					else if( new_value < mod->target_rep_limit )
						new_mod = mod->target_rep_limit - old_value;	//presume both values are negative
				}
				else if( Value > 0 )
				{
					if( old_value >= mod->target_rep_limit )
						continue;		// value is already lower than allowed one
					else if( new_value > mod->target_rep_limit )
						new_mod = mod->target_rep_limit - old_value;
				}
			}
			ModStanding( mod->target_rep, new_mod, 1 );
		}
	}
}

void Player::OnModStanding( FactionDBC * dbc, FactionReputation * rep, int32 value )
{
	if ( SetFlagVisible( rep->flag, true ) && IsInWorld() )
	{
		m_session->OutPacket( SMSG_SET_FACTION_VISIBLE, 4, &dbc->RepListId );
	}

	SetFlagAtWar( rep->flag, ( GetReputationRankFromStanding( rep->standing ) <= STANDING_HOSTILE ) );

	if ( Visible( rep->flag ) && IsInWorld() && value )
	{
		sStackWorldPacket( data, SMSG_SET_FACTION_STANDING, 17+10 );
		data << uint32( 0 ) ; //?
		data << uint8( 1 ) ; //count
		data << uint32( rep->flag ) << dbc->RepListId << rep->CalcStanding();
		m_session->SendPacket( &data );
	}

	// PVP title as a reward for exalted reputations
	switch( dbc->RepListId )
	{
		case FACTION_STORMPIKE_GUARDS:
		case FACTION_SILVERWING_SENTINELS:
		case FACTION_THE_LEAGUE_OF_ARATHOR:
		{
			if( GetTeam() == 0 && 
				GetStandingRank( 730 ) == STANDING_EXALTED &&
				GetStandingRank( 890 ) == STANDING_EXALTED &&
				GetStandingRank( 509 ) == STANDING_EXALTED )
			{
				SetKnownTitle( PVPTITLE_JUSTICAR , true );
			}
			else
				SetKnownTitle( PVPTITLE_JUSTICAR , false );
		} break;
		case FACTION_THE_DEFILERS:
		case FACTION_FROSTWOLF_CLAN:
		case FACTION_WARSONG_OUTRIDERS:
		{
			if( GetTeam() == 1 && 
				GetStandingRank( 510 ) == STANDING_EXALTED &&
				GetStandingRank( 729 ) == STANDING_EXALTED &&
				GetStandingRank( 889 ) == STANDING_EXALTED )
			{
				SetKnownTitle( PVPTITLE_CONQUEROR , true );
			}
			else
				SetKnownTitle( PVPTITLE_CONQUEROR , false );
		} break;
	}
}
