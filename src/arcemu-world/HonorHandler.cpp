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

void WorldSession::HandleSetVisibleRankOpcode( WorldPacket& recv_data )
{
	CHECK_PACKET_SIZE( recv_data, 4 );
	uint32 ChosenRank;
	recv_data >> ChosenRank; 
	if( ChosenRank == 0xFFFFFFFF )
		_player->SetUInt32Value( PLAYER_CHOSEN_TITLE, 0 );
	else if( _player->HasKnownTitle( ( RankTitles )( ChosenRank ) ) )
		_player->SetUInt32Value( PLAYER_CHOSEN_TITLE, ChosenRank );
}

void HonorHandler::AddHonorPointsToPlayer(Player *pPlayer, int32 uAmount)
{
	if( pPlayer->GetMapId() == 559 || pPlayer->GetMapId() == 562 || pPlayer->GetMapId() == 572)
	{ 
		return;
	}
	uAmount = uAmount + uAmount * pPlayer->HonorModBonusPCT / 100;

	if( uAmount <= 0 )
		return;

	pPlayer->m_honorToday += uAmount;
//	pPlayer->m_honorPoints += uAmount;
	uint32 Hpoints = pPlayer->GetCurrencyCount( CURRENCY_HONOR_POINT );
//	Hpoints += uAmount;
//	if( Hpoints > CURRENCY_LIMIT_HONOR_POINT ) 
//		Hpoints = CURRENCY_LIMIT_HONOR_POINT;
//	pPlayer->SetCurrencyCount( CURRENCY_HONOR_POINT, uAmount );
	if( Hpoints + uAmount > CURRENCY_LIMIT_HONOR_POINT )
		uAmount = MAX(0, CURRENCY_LIMIT_HONOR_POINT - (int32)Hpoints );
	pPlayer->ModCurrencyCount( CURRENCY_HONOR_POINT, uAmount );
	
	pPlayer->HandleProc(PROC_ON_GAIN_EXPIERIENCE, pPlayer, NULL);

	RecalculateHonorFields(pPlayer);
}

int32 HonorHandler::CalculateHonorPointsForKill( Player *pPlayer, Unit* pVictim )
{
	// this sucks.. ;p
	if( pVictim == NULL )
	{
		int32 pts = rand() % 100 + 100;
		return pts;
	}

	// Suicide lol
	if( pVictim == pPlayer )
	{ 
		return 0;
	}

	if( pVictim->GetTypeId() != TYPEID_PLAYER )
	{ 
		return 0;
	}

	//use Player::m_honorless, applied with Aura::SpellAuraNoPVPCredit
	// How dishonorable, you fiend!
	//if( pVictim->HasActiveAura( PLAYER_HONORLESS_TARGET_SPELL ) )
	//	return 0;

	if ( pVictim->GetTypeId() == TYPEID_PLAYER && SafePlayerCast(pVictim)->m_honorless )
	{ 
		return 0;
	}

	uint32 k_level = pPlayer->GetUInt32Value( UNIT_FIELD_LEVEL );
	uint32 v_level = pVictim->GetUInt32Value( UNIT_FIELD_LEVEL );

//	int k_honor = pPlayer->m_honorPoints;
	int k_honor = pPlayer->GetCurrencyCount( CURRENCY_HONOR_POINT );
//	int v_honor = SafePlayerCast( pVictim )->m_honorPoints;
	int v_honor = SafePlayerCast( pVictim )->GetCurrencyCount( CURRENCY_HONOR_POINT );

	uint32 k_grey = 0;

	if( k_level > 5 && k_level < 40 )
	{
		k_grey = k_level - 5 - float2int32( floor( ((float)k_level) / 10.0f ) );
	}
	else
	{
		k_grey = k_level - 1 - float2int32( floor( ((float)k_level) / 5.0f ) );
	}

	if( k_honor == 0 )
		k_honor = 1;

	float diff_level = ((float)v_level - k_grey) / ((float)k_level - k_grey);
	if( diff_level > 2 ) diff_level = 2.0f;
	if( diff_level < 0 ) diff_level = 0.0f;

	float diff_honor = ((float)v_honor) / ((float)k_honor);
	if( diff_honor > 3 ) diff_honor = 3.0f;
	if( diff_honor < 0 ) diff_honor = 0.0f;

	float honor_points = diff_level * ( 150.0f + diff_honor * 60 );
	honor_points *= ((float)k_level) / PLAYER_LEVEL_CAP;
	honor_points *= World::getSingleton().getRate( RATE_HONOR );

	return float2int32( honor_points );
}

void HonorHandler::OnPlayerKilledUnit( Player *pPlayer, Unit* pVictim )
{
	if( pVictim == NULL || pPlayer == NULL || pPlayer == pVictim )
	{ 
		return;
	}

	if( pPlayer->GetTypeId() != TYPEID_PLAYER || !pVictim->IsUnit() )
	{ 
		return;
	}

	if( !pVictim->IsPlayer() || SafePlayerCast( pVictim )->m_honorless )
	{ 
		return;
	}

    if( pVictim->IsPlayer() )
	{
		if( pPlayer->m_bg )
		{
			if( SafePlayerCast( pVictim )->m_bgTeam == pPlayer->m_bgTeam )
			{ 
				return;
			}

			// patch 2.4, players killed >50 times in battlegrounds won't be worth honor for the rest of that bg
			if( SafePlayerCast(pVictim)->m_bgScore.Deaths >= 50 )
			{ 
				return;
			}
		}
		else
		{
			if( pPlayer->GetTeam() == SafePlayerCast( pVictim )->GetTeam() )
			{ 
				return;
			}
		}
	}

	// Calculate points
	int32 Points = CalculateHonorPointsForKill(pPlayer, pVictim);
	if( Points > 0 )
	{
		pPlayer->Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_HONORABLE_KILL,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
		pPlayer->Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_HK_CLASS,pVictim->getClass(),ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
		pPlayer->Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_HK_RACE,pVictim->getRace(),ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
		//also has map filter. But areas do not repeat over maps(unique id)
		pPlayer->Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_HONORABLE_KILL_AT_AREA,pPlayer->GetAreaID(),ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
		if( pPlayer->m_bg )
		{
			// hackfix for battlegrounds (since the gorups there are disabled, we need to do this manually)
			vector<Player*> toadd;
			uint32 t = pPlayer->m_bgTeam;
			toadd.reserve(15);		// shouldnt have more than this
			pPlayer->m_bg->Lock();
			InRangePlayerSet * s = &pPlayer->m_bg->m_players[t];

			for(InRangePlayerSet::iterator itr = s->begin(); itr != s->end(); ++itr)
			{
				if((*itr) == pPlayer || (*itr)->isInRange(pPlayer,100.0f))
					toadd.push_back(*itr);
			}

			if( toadd.size() > 0 )
			{
				uint32 pts = Points / (uint32)toadd.size();
				for(vector<Player*>::iterator vtr = toadd.begin(); vtr != toadd.end(); ++vtr)
				{
					AddHonorPointsToPlayer(*vtr, pts);

					(*vtr)->m_killsToday++;
					(*vtr)->m_killsLifetime++;
					pPlayer->m_bg->HookOnHK(*vtr);
					if(pVictim)
					{
						// Send PVP credit
						WorldPacket data(SMSG_PVP_CREDIT, 12);
						uint32 pvppoints = pts * 10;	//because client divides this with 100 when showing in combat log
						data << pvppoints << pVictim->GetGUID() << uint32(SafePlayerCast(pVictim)->GetPVPRank());
						(*vtr)->GetSession()->SendPacket(&data);
					}
				}
			}

			pPlayer->m_bg->Unlock();
		}
		else
		{
			InRangePlayerSet contributors;
			contributors.insert( pPlayer ); //we killed him so we must get some reward 
			// First loop: Get all the people in the attackermap.
			pPlayer->AquireInrangeLock();
			InrangeLoopExitAutoCallback AutoLock;
			for(InRangeSetRecProt::iterator i = pPlayer->GetInRangeSetBegin( AutoLock ); i != pPlayer->GetInRangeSetEnd(); ++i)
				if( (*i)->IsPlayer() && isFriendly(pPlayer, (*i)) )
				{
					bool added = false;
					Player * plr = SafePlayerCast(*i);
					if(pVictim->CombatStatus.m_CombatTargets.find(plr->GetGUID()) != pVictim->CombatStatus.m_CombatTargets.end())
					{
						added = true;
						contributors.insert(plr);
					}

					if(added && plr->GetGroup())
					{
						Group * pGroup = plr->GetGroup();
						uint32 groups = pGroup->GetSubGroupCount();
						for(uint32 i = 0; i < groups; i++)
						{
							SubGroup * sg = pGroup->GetSubGroup(i);
							if(!sg) continue;

							for(GroupMembersSet::iterator itr2 = sg->GetGroupMembersBegin(); itr2 != sg->GetGroupMembersEnd(); itr2++)
							{
								PlayerInfo * pi = (*itr2);
								Player * gm = objmgr.GetPlayer(pi->guid);
								if(!gm) continue;

								if(gm->isInRange(pVictim, 100.0f))
									contributors.insert(gm);
							}
						}
					}
				}
			pPlayer->ReleaseInrangeLock();

			for(InRangePlayerSet::iterator itr = contributors.begin(); itr != contributors.end(); itr++)
			{
				Player * pAffectedPlayer = (*itr);
				if(!pAffectedPlayer) continue;

				pAffectedPlayer->m_killsToday++;
				pAffectedPlayer->m_killsLifetime++;
				if(pAffectedPlayer->m_bg)
					pAffectedPlayer->m_bg->HookOnHK(pAffectedPlayer);

				int32 contributorpts = Points / (int32)contributors.size();
				AddHonorPointsToPlayer(pAffectedPlayer, contributorpts);
				if(pVictim->IsPlayer())
				{
					sHookInterface.OnHonorableKill(pAffectedPlayer, SafePlayerCast(pVictim));

					WorldPacket data(SMSG_PVP_CREDIT, 12);
					uint32 pvppoints = contributorpts * 10; // Why *10?
					data << pvppoints << pVictim->GetGUID() << uint32(SafePlayerCast(pVictim)->GetPVPRank());
					pAffectedPlayer->GetSession()->SendPacket(&data);
				}

				if(pAffectedPlayer->GetZoneId() == 3518)
				{
					// Add Halaa Battle Token
					SpellEntry * pvp_token_spell = dbcSpell.LookupEntry(pAffectedPlayer->GetTeam()? 33004 : 33005);
					pAffectedPlayer->CastSpell(pAffectedPlayer, pvp_token_spell, true);
				}
				// If we are in Hellfire Peninsula
				if(pAffectedPlayer->GetZoneId() == 3483)
				{
					// Add Mark of Thrallmar/Honor Hold
					SpellEntry * pvp_token_spell = dbcSpell.LookupEntry(pAffectedPlayer->GetTeam()? 32158 : 32155);
					pAffectedPlayer->CastSpell(pAffectedPlayer, pvp_token_spell, true);
				}
			}
		}
	}
}

void HonorHandler::RecalculateHonorFields(Player *pPlayer)
{
	pPlayer->SetUInt32Value(PLAYER_FIELD_KILLS, uint16(pPlayer->m_killsToday) | ( pPlayer->m_killsYesterday << 16 ) );
	pPlayer->SetUInt32Value(PLAYER_FIELD_LIFETIME_HONORBALE_KILLS, pPlayer->m_killsLifetime);
}

bool ChatHandler::HandleAddKillCommand(const char* args, WorldSession* m_session)
{
	uint32 KillAmount = args ? atol(args) : 1;
	Player *plr = getSelectedChar(m_session, true);
	if(plr == 0)
	{ 
		return true;
	}

	BlueSystemMessage(m_session, "Adding %u kills to player %s.", KillAmount, plr->GetName());
	GreenSystemMessage(plr->GetSession(), "You have had %u honor kills added to your character.", KillAmount);

	for(uint32 i = 0; i < KillAmount; ++i)
		HonorHandler::OnPlayerKilledUnit(plr, 0);

	return true;
}

bool ChatHandler::HandleAddHonorCommand(const char* args, WorldSession* m_session)
{
	uint32 HonorAmount = args ? atol(args) : 1;
	Player *plr = getSelectedChar(m_session, true);
	if(plr == 0)
	{ 
		return true;
	}

	BlueSystemMessage(m_session, "Adding %u honor to player %s.", HonorAmount, plr->GetName());
	GreenSystemMessage(plr->GetSession(), "You have had %u honor points added to your character.", HonorAmount);

	HonorHandler::AddHonorPointsToPlayer(plr, HonorAmount);
	return true;
}

bool ChatHandler::HandlePVPCreditCommand(const char* args, WorldSession* m_session)
{
	uint32 Rank, Points;
	if(sscanf(args, "%u %u", (unsigned int*)&Rank, (unsigned int*)&Points) != 2)
	{
		RedSystemMessage(m_session, "Command must be in format <rank> <points>.");
		return true;
	}
	Points *= 10;
	uint64 Guid = m_session->GetPlayer()->GetSelection();
	if(Guid == 0)
	{
		RedSystemMessage(m_session, "A selection of a unit or player is required.");
		return true;
	}

	BlueSystemMessage(m_session, "Building packet with Rank %u, Points %u, GUID "I64FMT".", 
		Rank, Points, Guid);

	WorldPacket data(SMSG_PVP_CREDIT, 12);
	data << Points << Guid << Rank;
	m_session->SendPacket(&data);
	return true;
}

bool ChatHandler::HandleGlobalHonorDailyMaintenanceCommand(const char* args, WorldSession* m_session)
{
	return false;
}

bool ChatHandler::HandleNextDayCommand(const char* args, WorldSession* m_session)
{
	return false;
}

int32 HonorHandler::CalculateHonorPointsForKill( uint32 playerLevel, uint32 victimLevel )
{

   uint32 k_level = playerLevel;
   uint32 v_level = victimLevel;

   uint32 k_grey = 0;

   if( k_level > 5 && k_level < 40 )
   {
      k_grey = k_level - 5 - float2int32( floor( ((float)k_level) / 10.0f ) );
   }
   else
   {
      k_grey = k_level - 1 - float2int32( floor( ((float)k_level) / 5.0f ) );
   }

   if(v_level <= k_grey)
   { 
      return 0;
   }

   // Correct formula unknown. This one is correct for lvl 70 killing lvl 70 and scales down for lower levels
   uint32 diff_level = v_level - k_level; // Should somehow affect the result

   float honor_points = 20.9f;
   honor_points *= ((float)k_level) / PLAYER_LEVEL_CAP;
   honor_points *= World::getSingleton().getRate( RATE_HONOR );
   if(honor_points < 1.0f) // Make sure we get at least 1 point on low levels
   { 
      return 1;
   }
   return float2int32( honor_points );
}

