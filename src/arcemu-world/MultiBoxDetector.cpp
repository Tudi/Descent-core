#include "StdAfx.h"

MultiBoxDetector::MultiBoxDetector()
{
	m_NextUpdateStamp = 0;
}

MultiBoxDetector::~MultiBoxDetector()
{
	m_IP_groups_buildup.clear();
	m_IP_groups_inspected.clear();
	std::map< ULONG, MultiBoxGroupStateStorage * >::iterator ggsitr;
	for( ggsitr = m_IP_groups_inspected.begin(); ggsitr != m_IP_groups_inspected.end(); ggsitr++)
		delete ggsitr->second;
}

void MultiBoxDetector::StartCycle()
{
	if( m_NextUpdateStamp > getMSTime() )
		return;
	m_IP_groups_buildup.clear();
	m_inspected_players.clear();
}


void MultiBoxDetector::AddPlayer(Player *p)
{
	if( m_NextUpdateStamp > getMSTime() )
		return;
	if( p == NULL || p->GetSession() == NULL || p->GetSession()->GetSocket() == NULL )
		return;

	ULONG IP = p->GetSession()->GetSocket()->GetRemoteAddress().S_un.S_addr;
	m_IP_groups_buildup[ IP ]++;

	m_inspected_players.push_front( p );
}

void MultiBoxDetector::EndCycle()
{
	if( m_NextUpdateStamp > getMSTime() )
		return;
	//let's see if we got IPs with multiple players 
	std::map<ULONG, uint32 >::iterator itr;
	for( itr = m_IP_groups_buildup.begin(); itr != m_IP_groups_buildup.end(); itr++ )
		if( itr->second > 1 )
		{
			//get a query result storage container for this group
			MultiBoxGroupStateStorage *gs;
			gs = m_IP_groups_inspected[ itr->first ];
			if( gs == NULL )
			{
				gs = new MultiBoxGroupStateStorage;
				gs->m_TimesInvestigated = 0;
				m_IP_groups_inspected[ itr->first ] = gs;
			}
			gs->m_LastInvestigatingStamp = getMSTime();
			gs->m_InvestigatedPlayers.clear();

			//get all player states with same IP
			std::list<Player *> t_list;
			std::list<Player *>::iterator itrp,itrp2;
			for( itrp = m_inspected_players.begin(); itrp != m_inspected_players.end(); itrp++ )
				if( (*itrp)->GetSession()->GetSocket()->GetRemoteAddress().S_un.S_addr == itr->first )
					t_list.push_front( (*itrp) );
			//now check the temp list to get players that are near each other 
			for( itrp = t_list.begin(); itrp != t_list.end(); itrp++ )
				for( itrp2 = t_list.begin(); itrp2 != t_list.end(); itrp2++ )
					if( *itrp != *itrp2 && (*itrp)->GetDistance2dSq( *itrp2 ) <= MB_PROXIMITY_REQUIRED_SQ )
					gs->m_InvestigatedPlayers.push_front( *itrp );
		}

	//cleanup non used query holders and boot players that can be booted
	std::map< ULONG, MultiBoxGroupStateStorage * >::iterator ggsitr,tggsitr;
	for( ggsitr = m_IP_groups_inspected.begin(); ggsitr != m_IP_groups_inspected.end(); )
	{
		tggsitr = ggsitr;
		ggsitr++;
		MultiBoxGroupStateStorage *gs = tggsitr->second;
		//if the list is outdated then we wipe the list and wait until we can create a new list
		if( gs->m_LastInvestigatingStamp != getMSTime() )
		{
			gs->m_InvestigatedPlayers.clear();
			delete gs;
			m_IP_groups_inspected.erase( tggsitr );
			continue;
		}
		else if( gs->m_TimesInvestigated > MB_MAX_PROXIMITY_CHECKS )
		{
			//boot all players from this list
			std::list<Player *>::iterator itrp;
			uint32 list_size = (uint32)gs->m_InvestigatedPlayers.size();
			for( itrp = gs->m_InvestigatedPlayers.begin(); itrp != gs->m_InvestigatedPlayers.end(); itrp++ )
			{
				Player *p = *itrp;
				if( sEventMgr.HasEvent( p, EVENT_PLAYER_KICK ) == false && p->GetSession()->HasPermissions() == true )
				{
					p->BroadcastMessage( "Possible Multiboxing detected. In case server was wrong then make a report how to reproduce this case. You will be logged out in 7 seconds." );
					sCheatLog.writefromsession( p->GetSession(), "Caught %s Multiboxing with %d instances", p->GetName(), list_size );
					uint32 uBanTime = (uint32)UNIXTIME + 5*60; //5 minutes ban
					//_player->SetBanned( uBanTime, sReason );
					sEventMgr.AddEvent( p, &Player::_Kick, EVENT_PLAYER_KICK, 7000, 1, 0 );
				}
			}
		}
	}
	//avoid spamming the updates. Players do not run all the times
	m_NextUpdateStamp =  getMSTime() + MB_CHECK_INTERVAL;
	//no need to eat memory
	m_inspected_players.clear();
}