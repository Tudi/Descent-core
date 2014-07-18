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

uint64 GUILD_LEVEL_XP_REQ[MAX_GUILD_LEVEL+2] =
{
	0,	//req for lvl 0
	16580000,	//req for level 1
	18240000,	//req for level 2
	19900000,
	21550000,
	23220000,
	24880000,
	26530000,
	28190000,
	29850000,
	31510000,
	33170000,
	34820000,
	36490000,
	38140000,
	39800000,
	41450000,
	43120000,
	44780000,
	46430000,
	48090000,
	49750000,
	51410000,
	53060000,
	54730000,
	54730000,
	54730000,
};

Guild::Guild()
{
	m_commandLogging=true;
	m_guildId=0;
	m_guildLeader=0;
	m_guildName=(char*)"Goose";
	m_guildInfo=NULL;
	m_motd=NULL;
	m_backgroundColor=0;
	m_emblemColor=0;
	m_emblemStyle=0;
	m_borderColor=0;
	m_borderStyle=0;
	m_creationTimeStamp=0;
	m_bankBalance =0;
	m_bankTabCount=0;
	creationDay=creationMonth=creationYear=0;
	m_hiLogId=1;
	memset(&m_ranks[0], 0, sizeof(GuildRank*)*MAX_GUILD_RANKS);
	m_level = 1;
	m_level_XP = 0;
	m_level_XP_limit_today = GUILD_DAILY_XP_LIMIT;
	m_level_XP_limit_today_stamp = (uint32)UNIXTIME;
	m_last_XP_save_stamp = getMSTime();
	GuildFinderSetFilter( true, GF_CFAF_QUESTING|GF_CFAF_DUNGEONS|GF_CFAF_RAIDS|GF_CFAF_PVP|GF_CFAF_PVE, GF_CFAF_WEEKDAYS|GF_CFAF_WEEKENDS, GF_CFRF_TANK|GF_CFRF_HEALER|GF_CFRF_DPS, GF_GFLF_ANY_LEVEL, "" );
}

Guild::~Guild()
{
	SaveGuildState();

	for(GuildMemberMap::iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
	{
		if(itr->second->szOfficerNote)
			free((void*)itr->second->szOfficerNote);
		if(itr->second->szPublicNote)
			free((void*)itr->second->szPublicNote);
		delete itr->second;
		itr->second = NULL;
	}
	m_members.clear();

	for(uint32 i = 0; i < MAX_GUILD_RANKS; ++i)
	{
		if(m_ranks[i] != NULL)
		{
			free(m_ranks[i]->szRankName);
			delete m_ranks[i];
			m_ranks[i] = NULL;
		}
	}

	for(list<GuildLogEvent*>::iterator itr = m_log.begin(); itr != m_log.end(); ++itr)
	{
		delete (*itr);
		(*itr) = NULL;
	}
	m_log.clear();

	for(list<GuildNewsEvent*>::iterator itr = m_NewsLog.begin(); itr != m_NewsLog.end(); ++itr)
	{
		delete (*itr);
		(*itr) = NULL;
	}
	m_NewsLog.clear();

	for( GuildBankTabVector::iterator itr = m_bankTabs.begin(); itr != m_bankTabs.end(); ++itr )
	{
		for( uint32 i = 0; i < MAX_GUILD_BANK_SLOTS; ++i )
			if( (*itr)->pSlots[i] != NULL )
			{
				(*itr)->pSlots[i]->DeleteMe();  //this will fail because pool destroy might destroy it before us
												// guilds are loaded only at server startup and unloaded on shutdown
				(*itr)->pSlots[i] = NULL;
			}

		for(list<GuildBankEvent*>::iterator it2 = (*itr)->lLog.begin(); it2 != (*itr)->lLog.end(); ++it2)
		{
			delete (*it2);
			(*it2) = NULL;
		}
		(*itr)->lLog.clear();
		
		free( (*itr)->szTabIcon );
		free( (*itr)->szTabInfo );
		free( (*itr)->szTabName );
		
		delete (*itr);
		(*itr) = NULL;
	}
	m_bankTabs.clear();

	for(list<GuildBankEvent*>::iterator it2 = m_moneyLog.begin(); it2 != m_moneyLog.end(); ++it2)
	{
		delete (*it2);
		(*it2) = NULL;
	}
	m_moneyLog.clear();
	if( m_motd )
	{
		free( m_motd );
		m_motd = NULL;
	}
	if(m_guildInfo)
	{
		free(m_guildInfo);
		m_guildInfo = NULL;
	}
	free(m_guildName);
	//guild finder
	{
		std::list< GuildFinderRequest *>::iterator itr;
		for( itr = m_guild_finder_requests.begin(); itr != m_guild_finder_requests.end(); itr++)
			(*itr)->DecRef();
		m_guild_finder_requests.clear();
	}
}

void Guild::SendGuildCommandResult(WorldSession * pClient, uint32 iCmd, const char * szMsg, uint32 iType)
{
	WorldPacket data(SMSG_GUILD_COMMAND_RESULT, (9+strlen(szMsg)));
	data << iCmd << szMsg << iType;
	pClient->SendPacket(&data);
}

GuildRank * Guild::FindLowestRank()
{
	for(uint32 i = MAX_GUILD_RANKS-1; i > 0; --i)
	{
		if(m_ranks[i] != NULL)
		{ 
			return m_ranks[i];
		}
	}

	return NULL;
}

GuildRank * Guild::FindHighestRank()
{
	for(uint32 i = 1; i < MAX_GUILD_RANKS; ++i)
	{
		if(m_ranks[i] != NULL)
		{ 
			return m_ranks[i];
		}
	}

	return NULL;
}

bool GuildRank::CanPerformCommand(uint32 t)
{
	return ((iRights & t) != 0 ? true : false);
}

bool GuildRank::CanPerformBankCommand(uint32 t, uint32 tab)
{
	int32 tab_flags = iTabPermissions[tab].iFlags;	//need it like this for debugging
	tab_flags = tab_flags & t;
	return (tab_flags != 0 ? true : false);
}

void Guild::LogGuildEvent(uint8 iEvent, uint64 TargetGUID, uint8 iStringCount, ...)
{
	if(!m_commandLogging)
	{ 
		return;
	}

	va_list ap;
	char * strs[4] = {NULL,NULL,NULL,NULL};

	va_start(ap, iStringCount);
	ASSERT(iStringCount <= 4);

	WorldPacket data(SMSG_GUILD_EVENT, 100);
	uint32 i;
	data << iEvent;
	data << iStringCount;

	for(i = 0; i < iStringCount; ++i)
	{
		strs[i] = va_arg(ap, char*);
		data << strs[i];
	}

//	if( TargetGUID != 0 )	//not always, but 99% of the case, MOTD does not require it
		data << TargetGUID;

	va_end(ap);
	SendPacket(&data);
}

void Guild::AddGuildLogEntry(uint8 iEvent, uint8 iParamCount, ...)
{
	if(!m_commandLogging)
	{ 
		return;
	}

	va_list ap;
	uint32 i;
	GuildLogEvent * ev;

	va_start(ap, iParamCount);
	ASSERT(iParamCount<=3);

	ev = new GuildLogEvent;
	ev->iLogId = GenerateGuildLogEventId();
	ev->iEvent = iEvent;
	ev->iTimeStamp = (uint32)UNIXTIME;

	for(i = 0; i < iParamCount; ++i)
		ev->iEventData[i] = va_arg(ap, uint32);

	for(; i < 3; ++i)
		ev->iEventData[i] = 0;

	CharacterDatabase.Execute("INSERT INTO guild_logs VALUES(%u, %u, %u, %u, %u, %u, %u)",
		ev->iLogId, m_guildId, ev->iTimeStamp, ev->iEvent, ev->iEventData[0], ev->iEventData[1], ev->iEventData[2]);

	m_lock.Acquire();
	if(m_log.size() >= 25)
	{
		// limit it to 250 events.
		// delete the first (oldest) event.
		CharacterDatabase.Execute("DELETE FROM guild_logs WHERE log_id = %u AND guildid = %u", (*(m_log.begin()))->iLogId, m_guildId);
		delete *(m_log.begin());
		*(m_log.begin()) = NULL;
		m_log.pop_front();
	}

	m_log.push_back(ev);
	m_lock.Release();
}

void Guild::SendPacket(WorldPacket * data)
{
	m_lock.Acquire();
	for(GuildMemberMap::iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
	{
		if(itr->first->m_loggedInPlayer != NULL && itr->first->m_loggedInPlayer->GetSession())
			itr->first->m_loggedInPlayer->GetSession()->SendPacket(data);
	}
	m_lock.Release();
}

GuildRank * Guild::CreateGuildRank(const char * szRankName, uint32 iPermissions, bool bFullGuildBankPermissions)
{
	// find a free id
	int32 i, j;
	m_lock.Acquire();
	for(i = MAX_GUILD_RANKS-1; i>=0; i--)
	{
		if(m_ranks[i] == NULL && ( i==0 || m_ranks[i-1] != NULL ) )
		{
			GuildRank * r = new GuildRank;
			memset(r, 0, sizeof(GuildRank));
			r->iId = i;
			r->iRights=iPermissions;
			if( szRankName == NULL || strlen( szRankName ) < 2 )
			{
				char tstr[50];
				sprintf( tstr,"New Rank %d",i+1);
				r->szRankName = strdup_local(tstr);
			}
			else
			{
				r->szRankName = strdup_local(szRankName);
			}
			r->iGoldLimitPerDay = bFullGuildBankPermissions ? -1 : 0;

			if(bFullGuildBankPermissions)
			{
				for(j = 0; j < MAX_GUILD_BANK_TABS; ++j)
				{
					r->iTabPermissions[j].iFlags = GR_RIGHT_GUILD_BANK_ALL;			// this is both use tab and withdraw
					r->iTabPermissions[j].iStacksPerDay = -1;	// -1 = unlimited
				}
			}
			m_ranks[i] = r;
			m_lock.Release();

			// save the rank into the database
			CharacterDatabase.Execute("INSERT INTO guild_ranks VALUES(%u, %u, \"%s\", %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
				m_guildId, i, CharacterDatabase.EscapeString(string(r->szRankName)).c_str(),
				r->iRights, r->iGoldLimitPerDay,
				r->iTabPermissions[0].iFlags, r->iTabPermissions[0].iStacksPerDay,
				r->iTabPermissions[1].iFlags, r->iTabPermissions[1].iStacksPerDay,
				r->iTabPermissions[2].iFlags, r->iTabPermissions[2].iStacksPerDay,
				r->iTabPermissions[3].iFlags, r->iTabPermissions[3].iStacksPerDay,
				r->iTabPermissions[4].iFlags, r->iTabPermissions[4].iStacksPerDay,
				r->iTabPermissions[5].iFlags, r->iTabPermissions[5].iStacksPerDay,
				r->iTabPermissions[6].iFlags, r->iTabPermissions[6].iStacksPerDay,
				r->iTabPermissions[7].iFlags, r->iTabPermissions[7].iStacksPerDay
				);

			Log.Debug("Guild", "Created rank %u on guild %u (%s)", i, m_guildId, szRankName);

			return r;
		}
		//new ranks are supposed to have lowest rankid ?
//		if( m_ranks[i] != NULL )
//			return NULL;
	}
	m_lock.Release();
	return NULL;
}

// creating from a charter
void Guild::CreateFromCharter(Charter * pCharter, WorldSession * pTurnIn)
{
	uint32 i;
	m_lock.Acquire();

	m_guildId = objmgr.GenerateGuildId();
	objmgr.AddGuild( this );
	m_guildName = strdup_local(pCharter->GuildName.c_str());
	m_guildLeader = pCharter->LeaderGuid;
	m_creationTimeStamp = (uint32)UNIXTIME;

	// create the guild in the database
	CreateInDB();

	// rest of the fields have been nulled out, create some default ranks.
	GuildRank * leaderRank = CreateGuildRank("Guild Master", GR_RIGHT_ALL, true);
	CreateGuildRank("Officer", GR_RIGHT_ALL, true);
	CreateGuildRank("Veteran", GR_RIGHT_DEFAULT, false);
	CreateGuildRank("Member", GR_RIGHT_DEFAULT, false);
	GuildRank * defRank = CreateGuildRank("Initiate", GR_RIGHT_DEFAULT, false);

	// turn off command logging, we don't wanna spam the logs
	m_commandLogging = false;

	// add the leader to the guild
	AddGuildMember(pTurnIn->GetPlayer()->m_playerInfo, NULL, leaderRank->iId);

	// add all the other people
	for(i = 0; i < pCharter->SignatureCount; ++i)
	{
		PlayerInfo * pi = objmgr.GetPlayerInfo(pCharter->Signatures[i]);
		if(pi)
			AddGuildMember(pi, NULL, defRank->iId);
	}

	// re-enable command logging
	m_commandLogging = true;

	GuildNewsAdd( GUILD_NEWS_LOG_GUILD_CREATED, 0, 0 );

	m_lock.Release();
}

bool Guild::IsValidRank(uint32 Rank )
{
	if( Rank > MAX_GUILD_RANKS )
		return false;

	if( m_ranks[ Rank ] == NULL )
		return false;

	return true;
}

void Guild::PromoteGuildMember(PlayerInfo * pMember, WorldSession * pClient, uint32 new_rank)
{
	if(pClient->GetPlayer()->m_playerInfo->guild != this || pMember->guild != this)
	{ 
		return;
	}

	int32 TargetPlayerGuildRank = MAX_GUILD_RANKS;
	if( pMember->guildRank )
		TargetPlayerGuildRank = pMember->guildRank->iId;
	if(	!pClient->GetPlayer()->m_playerInfo->guildRank->CanPerformCommand(GR_RIGHT_PROMOTE)
		|| ( TargetPlayerGuildRank - pClient->GetPlayer()->m_playerInfo->guildRank->iId) <= 1 
		)
	{
		SendGuildCommandResult(pClient, GUILD_PROMOTE_S, "", GUILD_PERMISSIONS);
		return;
	}

	// need a proper command result for this
	if(TargetPlayerGuildRank == 1)
	{
		pClient->SystemMessage("You cannot promote this member any further.");
		return;
	}

	// find the lowest rank that isnt his rank
	if( new_rank >= MAX_GUILD_RANKS )
	{
		pClient->SystemMessage("Could not find a rank to promote this member to.");
		return;
	}
	if( TargetPlayerGuildRank == new_rank )
	{
		pClient->SystemMessage("New rank and old rank matches.");
		return;
	}
	m_lock.Acquire();

	GuildRank * newRank = m_ranks[ new_rank ];

	if(newRank==NULL)
	{
		m_lock.Release();
		pClient->SystemMessage("Could not find a rank to promote this member to.");
		return;
	}

	GuildMemberMap::iterator itr = m_members.find(pMember);
	if(itr == m_members.end())
	{
		// shouldnt happen
		m_lock.Release();
		return;
	}

	itr->second->pRank = newRank;
	itr->second->pPlayer->guildRank = newRank;

	// log it
/*
{SERVER} Packet: (0x2858) UNKNOWN PacketSize = 21 TimeStamp = 10263984
80 
0C 40 5F 03 00 00 00 03 
03 00 00 00 
E5 64 E4 01 00 00 00 03 
14333
{SERVER} Packet: (0xC763) UNKNOWN PacketSize = 17 TimeStamp = 7152443
CD 2F 80 03 B6 05 8C 6A 03 00 76 A7 01 00 00 00 75 
{SERVER} Packet: (0xC763) UNKNOWN PacketSize = 17 TimeStamp = 7175796
4D 2F 80 03 B6 05 8C 6A 03 00 76 A7 03 00 00 00 75 
*/
	WorldPacket data(SMSG_GUILD_PROMOTE_DEMOTE_EVENT, 21 + 10 );
	data << uint8(0x80);	//promote event
	data << uint64( pClient->GetPlayer()->GetGUID() );	//promoter
	data << uint32( newRank->iId );
	data << uint64( GET_PLAYER_GUID( pMember->guid ) );	//target
	SendPacket( &data );

	AddGuildLogEntry(GUILD_LOG_EVENT_PROMOTION, 3, pClient->GetPlayer()->GetLowGUID(), pMember->guid, newRank->iId);

	// update in the database
	CharacterDatabase.Execute("UPDATE guild_data SET guildRank = %u WHERE playerid = %u AND guildid = %u", newRank->iId, pMember->guid, m_guildId);

	// if the player is online, update his guildrank
	if(pMember->m_loggedInPlayer)
		pMember->m_loggedInPlayer->SetGuildRank(newRank->iId);

	// release lock
	m_lock.Release();
}

void Guild::DemoteGuildMember(PlayerInfo * pMember, WorldSession * pClient, uint32 new_rank )
{
	if(pClient->GetPlayer()->m_playerInfo->guild != this || pMember->guild != this)
	{ 
		return;
	}

	uint32 TargetPlayerGuildRank = MAX_GUILD_RANKS;
	if( pMember->guildRank )
		TargetPlayerGuildRank = pMember->guildRank->iId;
	if(!pClient->GetPlayer()->m_playerInfo->guildRank->CanPerformCommand(GR_RIGHT_DEMOTE) ||
		pMember->guid == GetGuildLeader()
		|| pClient->GetPlayer()->m_playerInfo->guildRank->iId >= TargetPlayerGuildRank 
		)
	{
		SendGuildCommandResult(pClient, GUILD_PROMOTE_S, "", GUILD_PERMISSIONS);
		return;
	}

	m_lock.Acquire();

	GuildRank * newRank = m_ranks[ new_rank ];

	if(newRank==NULL)
	{
		m_lock.Release();
		pClient->SystemMessage("Could not find a rank to demote this member to.");
		return;
	}

	GuildMemberMap::iterator itr = m_members.find(pMember);
	if(itr == m_members.end())
	{
		// shouldnt happen
		m_lock.Release();
		return;
	}

	itr->second->pRank = newRank;
	itr->second->pPlayer->guildRank = newRank;

	// log it
/*
{SERVER} Packet: (0x2858) UNKNOWN PacketSize = 21 TimeStamp = 10263984
00 
0C 40 5F 03 00 00 00 03 
04 00 00 00 
E5 64 E4 01 00 00 00 03 
*/
	WorldPacket data(SMSG_GUILD_PROMOTE_DEMOTE_EVENT, 21 + 10 );
	data << uint8(0x00);	//demote event
	data << uint64( pClient->GetPlayer()->GetGUID() );	//promoter
	data << uint32( newRank->iId );
	data << uint64( GET_PLAYER_GUID( pMember->guid ) );	//target
	SendPacket( &data );
	AddGuildLogEntry(GUILD_LOG_EVENT_DEMOTION, 3, pClient->GetPlayer()->GetLowGUID(), pMember->guid, newRank->iId);

	// update in the database
	CharacterDatabase.Execute("UPDATE guild_data SET guildRank = %u WHERE playerid = %u AND guildid = %u", newRank->iId, pMember->guid, m_guildId);

	// if the player is online, update his guildrank
	if(pMember->m_loggedInPlayer)
		pMember->m_loggedInPlayer->SetGuildRank(newRank->iId);

	// release lock
	m_lock.Release();
}

void Guild::SaveGuildState()
{
	CharacterDatabase.Execute("UPDATE guilds SET level = %u,level_XP="I64FMTD",level_XP_limit_today="I64FMTD",level_xp_limit_today_stamp=%u WHERE guildId = %u", m_level,m_level_XP,m_level_XP_limit_today,m_level_XP_limit_today_stamp,m_guildId);
}

bool Guild::LoadFromDB(Field * f)
{
	m_guildId = f[0].GetUInt32();
	m_guildName = strdup_local(f[1].GetString());
	m_guildLeader = f[2].GetUInt32();
	m_emblemStyle = f[3].GetUInt32();
	m_emblemColor = f[4].GetUInt32();
	m_borderStyle = f[5].GetUInt32();
	m_borderColor = f[6].GetUInt32();
	m_backgroundColor = f[7].GetUInt32();
	m_guildInfo = strlen(f[8].GetString()) ? strdup_local(f[8].GetString()) : NULL;
	m_motd = strlen(f[9].GetString()) ? strdup_local(f[9].GetString()) : NULL;
	m_creationTimeStamp = f[10].GetUInt32();
	m_bankTabCount = f[11].GetUInt32();
	m_bankBalance = f[12].GetUInt64();
	m_level = MIN(MAX(1,f[13].GetUInt32()),MAX_GUILD_LEVEL);
	m_level_XP = f[14].GetUInt64();
	m_level_XP_limit_today = f[15].GetUInt64();
	m_level_XP_limit_today_stamp = f[16].GetUInt32();
	if( m_level_XP_limit_today_stamp + (60*60*24) < UNIXTIME )
	{
		m_level_XP_limit_today_stamp = (uint32)UNIXTIME;
		m_level_XP_limit_today = GUILD_DAILY_XP_LIMIT;
		SaveGuildState();
	}

	// load ranks
	uint32 j;
	QueryResult * result = CharacterDatabase.Query("SELECT * FROM guild_ranks WHERE guildId = %u ORDER BY rankId ASC", m_guildId);
	if(result==NULL)
	{ 
		return false;
	}

	uint32 sid = 0;

	do 
	{
		//morron protection
		if( sid >= MAX_GUILD_RANKS )
			break;
		GuildRank * r = new GuildRank;
		Field * f2 = result->Fetch();
		r->iId = f2[1].GetUInt32();
		bool need_insert = false;
		if(r->iId!=sid)
		{
			Log.Notice("Guild", "Renaming rank %u of guild %s to %u.", r->iId, m_guildName, sid);
//			CharacterDatabase.Execute("UPDATE guild_ranks SET rankId = %u WHERE guildId = %u AND rankName = \"%s\"", sid,
//				m_guildId, CharacterDatabase.EscapeString(string(f2[2].GetString())).c_str());
			// screw previous version, it kept complaining for duplicate keys, rank will be saved later or lost on server crash
			CharacterDatabase.Execute("delete from guild_ranks where rankId = %u and guildId = %u", sid, m_guildId);
			r->iId=sid;
			need_insert = true;
		}
		sid++;
		r->szRankName = strdup_local(f2[2].GetString());
		r->iRights = f2[3].GetUInt32();
		r->iGoldLimitPerDay = f2[4].GetUInt32();
		if( r->iGoldLimitPerDay < -1 )
			r->iGoldLimitPerDay = 0xEFFFFFFF;
		
		for(j = 0; j < MAX_GUILD_BANK_TABS; ++j)
		{
			r->iTabPermissions[j].iFlags = f2[5+(j*2)].GetUInt32();
			r->iTabPermissions[j].iStacksPerDay = f2[6+(j*2)].GetUInt32();
		}

		//m_ranks.push_back(r);
		ASSERT(m_ranks[r->iId] == NULL);
		m_ranks[r->iId] = r;
		if( need_insert == true )
		{
			CharacterDatabase.Execute("INSERT INTO guild_ranks VALUES(%u, %u, \"%s\", %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
				m_guildId, r->iId, CharacterDatabase.EscapeString(string(r->szRankName)).c_str(),
				r->iRights, r->iGoldLimitPerDay,
				r->iTabPermissions[0].iFlags, r->iTabPermissions[0].iStacksPerDay,
				r->iTabPermissions[1].iFlags, r->iTabPermissions[1].iStacksPerDay,
				r->iTabPermissions[2].iFlags, r->iTabPermissions[2].iStacksPerDay,
				r->iTabPermissions[3].iFlags, r->iTabPermissions[3].iStacksPerDay,
				r->iTabPermissions[4].iFlags, r->iTabPermissions[4].iStacksPerDay,
				r->iTabPermissions[5].iFlags, r->iTabPermissions[5].iStacksPerDay,
				r->iTabPermissions[6].iFlags, r->iTabPermissions[6].iStacksPerDay,
				r->iTabPermissions[7].iFlags, r->iTabPermissions[7].iStacksPerDay
				);
		}
		//guild master should always have all rights. This is a hackfix due to system changes that made master unable to edit cirtain parts of a guild
		if( r->iId == 0 && r->iRights != GR_RIGHT_ALL )
			r->iRights = GR_RIGHT_ALL;
	} while(result->NextRow());
	delete result;
	result = NULL;

	// load members
	result = CharacterDatabase.Query("SELECT * FROM guild_data WHERE guildid = %u", m_guildId);
	if(result==NULL)
	{ 
		return false;
	}

	do 
	{
		Field * f3 = result->Fetch();
		GuildMember * gm = new GuildMember;
		uint32 field_reader = 0;
		field_reader++;	//skip field 0 = guild id
		gm->pPlayer = objmgr.GetPlayerInfo(f3[field_reader++].GetUInt32());
		if(gm->pPlayer == NULL)
		{
			delete gm;
			gm = NULL;
			continue;
		}

		uint32 rankid = f3[field_reader++].GetUInt32();
		if( rankid >= MAX_GUILD_RANKS || m_ranks[ rankid ] == NULL) 
		{
			delete gm;
			gm = NULL;
			continue;
		}

		gm->pRank = m_ranks[ rankid ];
		if(gm->pRank==NULL)
			gm->pRank=FindLowestRank();
		gm->pPlayer->guild=this;
		gm->pPlayer->guildRank=gm->pRank;
		gm->pPlayer->guildMember=gm;

		const char *tstr = f3[field_reader++].GetString();
		if(strlen( tstr ))
			gm->szPublicNote = strdup_local( tstr );
		else
			gm->szPublicNote = NULL;

		tstr = f3[field_reader++].GetString();
		if(strlen( tstr ))
			gm->szOfficerNote = strdup_local( tstr );
		else
			gm->szOfficerNote = NULL;

		gm->uLastWithdrawReset = f3[field_reader++].GetUInt32();
		gm->uWithdrawlsSinceLastReset = f3[field_reader++].GetUInt32();
		for(j = 0; j < MAX_GUILD_BANK_TABS; ++j)
		{
			gm->uLastItemWithdrawReset[j] = f3[field_reader++].GetUInt32();
			gm->uItemWithdrawlsSinceLastReset[j] = f3[field_reader++].GetUInt32();
		}
		gm->xp_contrib = f3[field_reader++].GetUInt32();

		m_members.insert(make_pair(gm->pPlayer, gm));

	} while(result->NextRow());
	delete result;
	result = NULL;

	result = CharacterDatabase.Query("SELECT MAX(log_id) FROM guild_logs WHERE guildid = %u", m_guildId);
	m_hiLogId = 1;
	if(result != NULL)
	{
		m_hiLogId = result->Fetch()[0].GetUInt32() + 1;
		delete result;
		result = NULL;
	}

	result = CharacterDatabase.Query("SELECT MAX(log_id) FROM guild_banklogs WHERE guildid = %u", m_guildId);
	if(result)
	{
		if((result->Fetch()[0].GetUInt32() + 1) > m_hiLogId)
			m_hiLogId = result->Fetch()[0].GetUInt32() + 1;
		delete result;
		result = NULL;
	}

	// load log
	result = CharacterDatabase.Query("SELECT * FROM guild_logs WHERE guildid = %u ORDER BY timestamp ASC", m_guildId);
	if(result)
	{
		do 
		{
			GuildLogEvent * li = new GuildLogEvent;
			li->iLogId = result->Fetch()[0].GetUInt32();
			li->iEvent = result->Fetch()[3].GetUInt32();
			li->iTimeStamp = result->Fetch()[2].GetUInt32();
			li->iEventData[0] = result->Fetch()[4].GetUInt32();
			li->iEventData[1] = result->Fetch()[5].GetUInt32();
			li->iEventData[2] = result->Fetch()[6].GetUInt32();
			m_log.push_back(li);
		} while(result->NextRow());
		delete result;
		result = NULL;
	}

	result = CharacterDatabase.Query("SELECT * FROM guild_banktabs WHERE guildId = %u ORDER BY tabId ASC", m_guildId);
	sid = 0;
	if(result)
	{
		do 
		{
			if((sid++) != result->Fetch()[1].GetUInt32())
			{
#if (defined( WIN32 ) || defined( WIN64 ) )
				MessageBox(0, "Guild bank tabs are out of order!", "Internal error", MB_OK);
				TerminateProcess(GetCurrentProcess(), 0);
				return false;
#else
				printf("Guild bank tabs are out of order!\n");
				exit(0);
#endif
			}

			QueryResult * res2 = CharacterDatabase.Query("SELECT * FROM guild_bankitems WHERE guildId = %u AND tabId = %u", m_guildId, result->Fetch()[1].GetUInt32());
			GuildBankTab * pTab = new GuildBankTab;
			if( pTab == NULL )
				return false;
			pTab->iTabId = (uint8)result->Fetch()[1].GetUInt32();
			pTab->szTabName = (strlen(result->Fetch()[2].GetString()) > 0) ? strdup_local(result->Fetch()[2].GetString()) : NULL;
			pTab->szTabIcon = (strlen(result->Fetch()[3].GetString()) > 0) ? strdup_local(result->Fetch()[3].GetString()) : NULL;
			pTab->szTabInfo = (strlen(result->Fetch()[4].GetString()) > 0) ? strdup_local(result->Fetch()[4].GetString()) : NULL;
			
			memset(&pTab->pSlots[0], 0, sizeof(Item*) * MAX_GUILD_BANK_SLOTS);

			if(res2)
			{
				do 
				{
					Item * pItem = objmgr.LoadItem(res2->Fetch()[3].GetUInt64());
					if(pItem == NULL)
					{
						CharacterDatabase.Execute("DELETE FROM guild_bankitems WHERE itemGuid = %u AND guildId = %u AND tabId = %u", res2->Fetch()[3].GetUInt32(), m_guildId, (uint32)pTab->iTabId);
						continue;
					}

					pTab->pSlots[res2->Fetch()[2].GetUInt32()] = pItem;

				} while (res2->NextRow());
				delete res2;
				res2 = NULL;
			}

			res2 = CharacterDatabase.Query("SELECT * FROM guild_banklogs WHERE guildid = %u AND tabid = %u ORDER BY timestamp ASC", m_guildId, result->Fetch()[1].GetUInt32());
			if(res2 != NULL)
			{
				do 
				{
					GuildBankEvent * ev= new GuildBankEvent;
					ev->iLogId=res2->Fetch()[0].GetUInt32();
					ev->iAction=res2->Fetch()[3].GetUInt8();
					ev->uPlayer=res2->Fetch()[4].GetUInt32();
					ev->uEntry=res2->Fetch()[5].GetUInt32();
					ev->iStack=res2->Fetch()[6].GetUInt8();
					ev->uTimeStamp=res2->Fetch()[7].GetUInt32();

					pTab->lLog.push_back(ev);

				} while (res2->NextRow());
				delete res2;
				res2 = NULL;
			}

			m_bankTabs.push_back(pTab);

		} while (result->NextRow());
		delete result;
		result = NULL;
		if( sid < m_bankTabCount )
		{
			sLog.outDebug("Mismatch in saved banktabount and real banktab count. Should wipe guild tabs for guild %u'n",m_guildId);
			//panic here ?
		}
	}

	result = CharacterDatabase.Query("SELECT * FROM guild_banklogs WHERE guildid = %u ORDER BY timestamp desc limit 0,%d", m_guildId,MAX_GUILD_BANK_TABS);
	if(result != NULL)
	{
		do 
		{
			GuildBankEvent * ev= new GuildBankEvent;
			ev->iLogId=result->Fetch()[0].GetUInt32();
			ev->iAction=result->Fetch()[3].GetUInt8();
			ev->uPlayer=result->Fetch()[4].GetUInt32();
			ev->uEntry=result->Fetch()[5].GetUInt32();
			ev->iStack=result->Fetch()[6].GetUInt8();
			ev->uTimeStamp=result->Fetch()[7].GetUInt32();

			m_moneyLog.push_front(ev);

		} while (result->NextRow());
		delete result;
		result = NULL;
	}

	result = CharacterDatabase.Query("SELECT * FROM guild_newslogs WHERE guildid = %u ORDER BY timestamp desc limit 0,%d", m_guildId, MAX_GUILD_NEWS);
	if(result != NULL)
	{
		do 
		{
			Field *fields = result->Fetch();
			GuildNewsEvent * ev= new GuildNewsEvent;
			ev->Id=fields[0].GetUInt32();
			ev->Type=fields[2].GetUInt32();
			ev->OwnerGuid=fields[3].GetUInt32();
			ev->Value=fields[4].GetUInt32();
			ev->TimeStamp=fields[5].GetUInt32();
			ev->Flags=fields[6].GetUInt32();

			m_NewsLog.push_front(ev);

		} while (result->NextRow());
		delete result;
		result = NULL;
	}

	Log.Debug("Guild", "Loaded guild %s, %u members.", m_guildName, m_members.size());
	return true;
}

void Guild::SetMOTD(const char * szNewMotd, WorldSession * pClient)
{
	if(pClient->GetPlayer()->m_playerInfo->guild != this)
	{ 
		return;
	}

	if(!pClient->GetPlayer()->m_playerInfo->guildRank->CanPerformCommand(GR_RIGHT_SETMOTD))
	{
		Guild::SendGuildCommandResult(pClient, GUILD_INVITE_S, "", GUILD_PERMISSIONS);
		return;
	}

	if(m_motd)
		free(m_motd);

	if(strlen(szNewMotd))
	{
		m_motd = strdup_local(szNewMotd);
		CharacterDatabase.Execute("UPDATE guilds SET motd = \"%s\" WHERE guildId = %u", CharacterDatabase.EscapeString(string(szNewMotd)).c_str(), m_guildId);
	}
	else
	{
		m_motd= NULL;
		CharacterDatabase.Execute("UPDATE guilds SET motd = \"\" WHERE guildId = %u", m_guildId);
	}

	LogGuildEvent(GUILD_EVENT_MOTD, pClient->GetPlayer()->GetGUID(), 1, szNewMotd);
}

void Guild::SetGuildInformation(const char * szGuildInformation, WorldSession * pClient)
{
	if(pClient->GetPlayer()->m_playerInfo->guild != this)
	{ 
		return;
	}

	if(!pClient->GetPlayer()->m_playerInfo->guildRank->CanPerformCommand(GR_RIGHT_EGUILDINFO))
	{
		Guild::SendGuildCommandResult(pClient, GUILD_INVITE_S, "", GUILD_PERMISSIONS);
		return;
	}

	if(m_guildInfo)
		free(m_guildInfo);

	if(strlen(szGuildInformation))
	{
		m_guildInfo = strdup_local(szGuildInformation);
		CharacterDatabase.Execute("UPDATE guilds SET guildInfo = \"%s\" WHERE guildId = %u", CharacterDatabase.EscapeString(string(szGuildInformation)).c_str(), m_guildId);
	}
	else
	{
		m_guildInfo= NULL;
		CharacterDatabase.Execute("UPDATE guilds SET guildInfo = \"\" WHERE guildId = %u", m_guildId);
	}
}

// adding a member
void Guild::AddGuildMember(PlayerInfo * pMember, WorldSession * pClient, int32 ForcedRank /* = -1 */)
{
	if(pMember->guild != NULL)
	{ 
		return;
	}

	if(pClient && pClient->GetPlayer()->m_playerInfo->guild != this)
	{ 
		return;
	}

	if(pClient && !pClient->GetPlayer()->m_playerInfo->guildRank->CanPerformCommand(GR_RIGHT_INVITE))
	{ 
		return;
	}

	m_lock.Acquire();
	GuildRank * r;
	if(m_members.size())
	{
		if( ForcedRank > 0 ) r = m_ranks[ForcedRank];
		else if(ForcedRank == -2 ) r = FindHighestRank();
		else r = FindLowestRank();
	}
	else
	{
		if( ForcedRank >= 0 ) r = m_ranks[ForcedRank];
		else if(ForcedRank == -2 ) r = FindHighestRank();
		else r = FindLowestRank();
	}

	if(r==NULL)
		r=FindLowestRank();

	if(r==NULL)
	{
		// shouldnt happen
		m_lock.Release();
		return;
	}

	GuildMember * pm = new GuildMember;
	memset(pm, 0, sizeof(GuildMember));
	pm->pPlayer = pMember;
	pm->pRank = r;
	pm->szOfficerNote = pm->szPublicNote = NULL;
	m_members.insert(make_pair(pMember, pm));

	pMember->guild=this;
	pMember->guildRank=r;
	pMember->guildMember=pm;

	if(pMember->m_loggedInPlayer)
	{
		pMember->m_loggedInPlayer->SetGuildId(m_guildId);
		pMember->m_loggedInPlayer->SetGuildRank(r->iId);
		pMember->m_loggedInPlayer->SetStanding( PLAYER_GUILD_FACTION_ID, 0 );	//just make this visible
		pMember->m_loggedInPlayer->SetUInt32Value(PLAYER_GUILDLEVEL, 1 );
		pMember->m_loggedInPlayer->SetFlag( PLAYER_FLAGS, PLAYER_FLAGS_IS_IN_GUILD );

		if( pMember->m_loggedInPlayer->GetSession() )
			SendGuildStatusUnk( pMember->m_loggedInPlayer->GetSession() );

		pMember->m_loggedInPlayer->GuildFinderWipeRequests();
	}
	Player *p = objmgr.GetPlayer( GetGuildLeader() );
	if( p && p->GetSession() )
		p->GetSession()->OutPacket( SMSG_GUILD_FINDER_GUILD_REQUESTS_INTEROGATED );

	CharacterDatabase.Execute("INSERT INTO guild_data VALUES(%u, %u, %u, '', '', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)", m_guildId, pMember->guid, r->iId);

	LogGuildEvent(GUILD_EVENT_JOINED, GET_PLAYER_GUID( pMember->guid ), 1, pMember->name);
	AddGuildLogEntry(GUILD_LOG_EVENT_JOIN, 1, pMember->guid);

	m_lock.Release();
}

void Guild::RemoveGuildMember(PlayerInfo * pMember, WorldSession * pClient)
{
	if(pMember->guild != this)
	{ 
		return;
	}

	if(pClient && pClient->GetPlayer()->m_playerInfo->guild != this)
	{ 
		return;
	}

	if(pClient)
	{
		int RDiff = pMember->guildRank->iId - pClient->GetPlayer()->m_playerInfo->guildRank->iId;

		if(	pClient->GetPlayer()->m_playerInfo != pMember
			&& ( !pClient->GetPlayer()->m_playerInfo->guildRank->CanPerformCommand(GR_RIGHT_REMOVE) 
					|| RDiff <= 0 ) )
		{
			Guild::SendGuildCommandResult(pClient, GUILD_CREATE_S, "", GUILD_PERMISSIONS);
			return;
		}
	}

	if(pMember->guildRank->iId==0)
	{
		if(pClient)
			pClient->SystemMessage("You cannot remove the guild master.");

		return;
	}

	CharacterDatabase.Execute("DELETE FROM guild_data WHERE playerid = %u", pMember->guid);

	m_lock.Acquire();
	GuildMemberMap::iterator itr = m_members.find(pMember);
	if(itr != m_members.end())
	{
		// this should always exist.
		if(itr->second->szOfficerNote)
			free((void*)itr->second->szOfficerNote);
		if(itr->second->szPublicNote)
			free((void*)itr->second->szPublicNote);

		delete itr->second;
		itr->second = NULL;
		m_members.erase(itr);
	}

	if(pClient && pClient->GetPlayer()->m_playerInfo != pMember)
	{
		if(pMember->m_loggedInPlayer)
		{
			Player * plr = objmgr.GetPlayer(pMember->guid);
			if (plr)
				sChatHandler.SystemMessageToPlr(plr, "You has been kicked from the guild by %s", pClient->GetPlayer()->GetName());
		}
		LogGuildEvent(GUILD_EVENT_REMOVED, GET_PLAYER_GUID( pMember->guid ), 2, pMember->name, pClient->GetPlayer()->GetName());
		AddGuildLogEntry(GUILD_LOG_EVENT_REMOVAL, 2, pClient->GetPlayer()->GetLowGUID(), pMember->guid);
	}
	else
	{
		LogGuildEvent(GUILD_EVENT_LEFT, GET_PLAYER_GUID( pMember->guid ), 1, pMember->name);
		AddGuildLogEntry(GUILD_LOG_EVENT_LEFT, 1, pMember->guid);
	}

	m_lock.Release();

	pMember->guildRank=NULL;
	pMember->guild=NULL;
	pMember->guildMember=NULL;

	if(pMember->m_loggedInPlayer)
	{
		pMember->m_loggedInPlayer->SetGuildRank(0);
		pMember->m_loggedInPlayer->SetGuildId(0);
	}
}

void Guild::SetPublicNote(PlayerInfo * pMember, const char * szNewNote, WorldSession * pClient)
{
	if(pMember->guild != this)
	{ 
		return;
	}

	if(pClient->GetPlayer()->m_playerInfo->guild != this)
	{ 
		return;
	}

	if(!pClient->GetPlayer()->m_playerInfo->guildRank->CanPerformCommand(GR_RIGHT_EPNOTE))
	{
		Guild::SendGuildCommandResult(pClient, GUILD_MEMBER_S, "", GUILD_PERMISSIONS);
		return;
	}

	m_lock.Acquire();
	GuildMemberMap::iterator itr = m_members.find(pMember);
	if(itr != m_members.end())
	{
		if(itr->second->szPublicNote)
			free((void*)itr->second->szPublicNote);

		if(szNewNote && szNewNote[0] != '\0')
			itr->second->szPublicNote = strdup_local(szNewNote);
		else
			itr->second->szPublicNote = NULL;

			// Update the database
		if (itr->second->szPublicNote == NULL) 
			CharacterDatabase.Execute("UPDATE guild_data SET publicNote=\"\" WHERE playerid=%u", pMember->guid);
		else
			CharacterDatabase.Execute("UPDATE guild_data SET publicNote=\"%s\" WHERE playerid=%u", 
				CharacterDatabase.EscapeString(string(itr->second->szPublicNote)).c_str(),
				pMember->guid
			);
	}
	m_lock.Release();

//	Guild::SendGuildCommandResult(pClient, GUILD_PUBLIC_NOTE_CHANGED_S, pMember->name, 0);
	uint8 PlayerModderGuidBytes[8];
	*(uint64*)PlayerModderGuidBytes = pMember->guid;

	sStackWorldPacket(data, SMSG_GUILD_SET_NOTE, 200 );
	data << uint16( 0x00DE );
	data << uint8( ObfuscateByte( PlayerModderGuidBytes[7] ) );
	data << uint8( ObfuscateByte( PlayerModderGuidBytes[0] ) );
	data << uint8( ObfuscateByte( PlayerModderGuidBytes[2] ) );
	data << szNewNote;
	data << uint8( ObfuscateByte( PlayerModderGuidBytes[1] ) );
	data << uint8( ObfuscateByte( PlayerModderGuidBytes[3] ) );
	pClient->SendPacket( &data );
}

void Guild::SetOfficerNote(PlayerInfo * pMember, const char * szNewNote, WorldSession * pClient)
{
	if(pMember->guild != this)
	{ 
		return;
	}

	if(pClient->GetPlayer()->m_playerInfo->guild != this)
	{ 
		return;
	}

	if(!pClient->GetPlayer()->m_playerInfo->guildRank->CanPerformCommand(GR_RIGHT_EOFFNOTE))
	{
		Guild::SendGuildCommandResult(pClient, GUILD_MEMBER_S, "", GUILD_PERMISSIONS);
		return;
	}

	m_lock.Acquire();
	GuildMemberMap::iterator itr = m_members.find(pMember);
	if(itr != m_members.end())
	{
		if(itr->second->szOfficerNote)
			free((void*)itr->second->szOfficerNote);

		if(szNewNote && szNewNote[0] != '\0')
			itr->second->szOfficerNote = strdup_local(szNewNote);
		else
			itr->second->szOfficerNote = NULL;

		// Update the database
		if (itr->second->szOfficerNote == NULL) 
			CharacterDatabase.Execute("UPDATE guild_data SET officerNote=\"\" WHERE playerid=%u", pMember->guid);
		else
			CharacterDatabase.Execute("UPDATE guild_data SET officerNote=\"%s\" WHERE playerid=%u", 
				CharacterDatabase.EscapeString(string(itr->second->szOfficerNote)).c_str(),
				pMember->guid
			);
	}
	m_lock.Release();

//	Guild::SendGuildCommandResult(pClient, GUILD_OFFICER_NOTE_CHANGED_S, pMember->name, 0);
	uint8 PlayerModderGuidBytes[8];
	*(uint64*)PlayerModderGuidBytes = pMember->guid;

	sStackWorldPacket(data, SMSG_GUILD_SET_NOTE, 200 );
	data << uint16( 0x00CE );
	data << uint8( ObfuscateByte( PlayerModderGuidBytes[7] ) );
	data << uint8( ObfuscateByte( PlayerModderGuidBytes[0] ) );
	data << uint8( ObfuscateByte( PlayerModderGuidBytes[2] ) );
	data << szNewNote;
	data << uint8( ObfuscateByte( PlayerModderGuidBytes[1] ) );
	data << uint8( ObfuscateByte( PlayerModderGuidBytes[3] ) );
	pClient->SendPacket( &data );
}

void Guild::RemoveGuildRank(GuildRank * rank,WorldSession * pClient)
{
	m_lock.Acquire();

//	GuildRank * pLowestRank = FindLowestRank();
	GuildRank * pLowestRank = rank;

	if(pLowestRank == NULL || pLowestRank->iId >= MAX_GUILD_RANKS )		// cannot delete default ranks.
	{
		if( pClient )
			pClient->SystemMessage("Cannot find a rank to delete.");
		m_lock.Release();
		return;		
	}

	// check for players that need to be promoted
	GuildMemberMap::iterator itr = m_members.begin();
	for(; itr != m_members.end(); ++itr)
	{
		if(itr->second->pRank == pLowestRank)
		{
			if( pClient )
				pClient->SystemMessage("There are still members using this rank. You cannot delete it yet!");
			m_lock.Release();
			return;
		}
	}

	CharacterDatabase.Execute("DELETE FROM guild_ranks WHERE guildId = %u AND rankId = %u", m_guildId, pLowestRank->iId);
	m_ranks[pLowestRank->iId] = NULL;
	delete pLowestRank;
	pLowestRank = NULL;
	m_lock.Release();
}

void Guild::Disband()
{
	m_lock.Acquire();
	for(GuildMemberMap::iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
	{
		itr->first->guild=NULL;
		itr->first->guildRank=NULL;
		itr->first->guildMember=NULL;
		if(itr->first->m_loggedInPlayer != NULL)
		{
			itr->first->m_loggedInPlayer->SetGuildId(0);
			itr->first->m_loggedInPlayer->SetGuildRank(0);
		}

		delete itr->second;
		itr->second = NULL;
	}
	m_members.clear();
	objmgr.RemoveGuild(m_guildId);
	CharacterDatabase.Execute("DELETE FROM guilds WHERE guildId = %u", m_guildId);
	CharacterDatabase.Execute("DELETE FROM guild_logs WHERE guildid = %u", m_guildId);
	CharacterDatabase.Execute("DELETE FROM guild_ranks WHERE guildId = %u", m_guildId);
	CharacterDatabase.Execute("DELETE FROM guild_data WHERE guildid = %u", m_guildId);
	CharacterDatabase.Execute("DELETE FROM guild_bankitems WHERE guildId = %u", m_guildId);
	CharacterDatabase.Execute("DELETE FROM guild_banktabs WHERE guildId = %u", m_guildId);
	CharacterDatabase.Execute("DELETE FROM guild_newslogs WHERE guildId = %u", m_guildId);
	m_lock.Release();
	delete this;
}

void Guild::ChangeGuildMaster(PlayerInfo * pNewMaster, WorldSession * pClient)
{
	if(pClient->GetPlayer()->GetLowGUID() != m_guildLeader)
	{
		Guild::SendGuildCommandResult(pClient, GUILD_PROMOTE_S, "", GUILD_PERMISSIONS);
		return;
	}

	m_lock.Acquire();
	GuildRank * newRank = FindHighestRank();
	if(newRank==NULL)
	{
		m_lock.Release();
		return;
	}

	GuildMemberMap::iterator itr = m_members.find(pNewMaster);
	GuildMemberMap::iterator itr2 = m_members.find(pClient->GetPlayer()->m_playerInfo);
	ASSERT(m_ranks[0]!=NULL);
	if(itr==m_members.end())
	{
		Guild::SendGuildCommandResult(pClient, GUILD_PROMOTE_S, pNewMaster->name, GUILD_PLAYER_NOT_IN_GUILD_S);
		m_lock.Release();
		return;
	}
	if(itr2==m_members.end())
	{
		// wtf??
		Guild::SendGuildCommandResult(pClient, GUILD_PROMOTE_S, "", GUILD_INTERNAL);
		m_lock.Release();
		return;
	}

	itr->second->pRank = m_ranks[0];
	itr->first->guildRank = itr->second->pRank;
	itr2->second->pRank = newRank;
	itr2->first->guildRank = newRank;
	CharacterDatabase.Execute("UPDATE guild_data SET guildRank = 0 WHERE playerid = %u AND guildid = %u", itr->first->guid, m_guildId);
	CharacterDatabase.Execute("UPDATE guild_data SET guildRank = %u WHERE playerid = %u AND guildid = %u", newRank->iId, itr2->first->guid, m_guildId);
	CharacterDatabase.Execute("UPDATE guilds SET leaderGuid = %u WHERE guildId = %u", itr->first->guid, m_guildId);
	m_guildLeader = itr->first->guid;
	m_lock.Release();
	

	LogGuildEvent(GUILD_EVENT_LEADER_CHANGED, GET_PLAYER_GUID( pNewMaster->guid ), 2, pClient->GetPlayer()->GetName(), pNewMaster->name);
	//TODO: Figure out the GUILD_LOG_EVENT_LEADER_CHANGED code
}

uint32 Guild::GenerateGuildLogEventId()
{
	uint32 r;
	m_lock.Acquire();
	r = ++m_hiLogId;
	m_lock.Release();
	return r;
}

void Guild::GuildChat(const char * szMessage, WorldSession * pClient, uint32 iType)
{
	if(pClient->GetPlayer()->m_playerInfo->guild != this)
	{ 
		return;
	}

	if(!pClient->GetPlayer()->m_playerInfo->guildRank->CanPerformCommand(GR_RIGHT_GCHATSPEAK))
	{
		Guild::SendGuildCommandResult(pClient, GUILD_MEMBER_S, "", GUILD_PERMISSIONS);
		return;
	}

	WorldPacket * data = sChatHandler.FillMessageData( CHAT_MSG_GUILD, ((int32)iType)==CHAT_MSG_ADDON?-1:LANG_UNIVERSAL, szMessage,
		pClient->GetPlayer()->GetGUID(), pClient->GetPlayer()->bGMTagOn? CHAT_FLAG_GM_TAG:0);

	m_lock.Acquire();
	for(GuildMemberMap::iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
	{
		if(itr->second->pRank->CanPerformCommand(GR_RIGHT_GCHATLISTEN) && itr->first->m_loggedInPlayer)
			itr->first->m_loggedInPlayer->GetSession()->SendPacket(data);
	}
	m_lock.Release();

	delete data;
	data = NULL;
}

void Guild::OfficerChat(const char * szMessage, WorldSession * pClient, uint32 iType)
{
	if(pClient->GetPlayer()->m_playerInfo->guild != this)
	{ 
		return;
	}

	if(!pClient->GetPlayer()->m_playerInfo->guildRank->CanPerformCommand(GR_RIGHT_OFFCHATSPEAK))
	{
		Guild::SendGuildCommandResult(pClient, GUILD_MEMBER_S, "", GUILD_PERMISSIONS);
		return;
	}

	WorldPacket * data = sChatHandler.FillMessageData( CHAT_MSG_OFFICER, ((int32)iType)==CHAT_MSG_ADDON?-1:LANG_UNIVERSAL, szMessage,
		pClient->GetPlayer()->GetGUID());

	m_lock.Acquire();
	for(GuildMemberMap::iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
	{
		if(itr->second->pRank->CanPerformCommand(GR_RIGHT_OFFCHATLISTEN) && itr->first->m_loggedInPlayer)
			itr->first->m_loggedInPlayer->GetSession()->SendPacket(data);
	}
	m_lock.Release();

	delete data;
	data = NULL;
}

void Guild::SendGuildLog(WorldSession * pClient)
{
/*
06 - size
06 - type GUILD_LOG_EVENT_LEFT ( maybe joined ? )
13 13 A7 05 00 00 00 02 - GUID
1A 32 01 00 - time passed since event ?
03 - type GUILD_LOG_EVENT_PROMOTION
A6 8D 6B 01 00 00 00 02 who 
77 B7 74 04 00 00 00 02 to who
01 - rank ?
26 2F 01 00 - time
04 
A6 8D 6B 01 00 00 00 02 
77 B7 74 04 00 00 00 02 
03 
0F 2F 01 00 
06 
1E F5 A5 05 00 00 00 02 
EB 2B 01 00 
06 
58 55 7A 05 00 00 00 02 
A2 0C 01 00 
06 
77 B7 74 04 00 00 00 02 
3E 06 01 00 
06 06 13 13 A7 05 00 00 00 02 EA 33 01 00 03 A6 8D 6B 01 00 00 00 02 77 B7 74 04 00 00 00 02 01 F6 30 01 00 04 A6 8D 6B 01 00 00 00 02 77 B7 74 04 00 00 00 02 03 DF 30 01 00 06 1E F5 A5 05 00 00 00 02 BB 2D 01 00 06 58 55 7A 05 00 00 00 02 72 0E 01 00 06 77 B7 74 04 00 00 00 02 0E 08 01 00
*/
	WorldPacket data(MSG_GUILD_EVENT_LOG_QUERY, 18*m_log.size()+1);
	list<GuildLogEvent*>::iterator itr;
	uint32 count = 0;

//	data << uint8(m_log.size() >= 25 ? 25 : m_log.size());
	data << uint8( 0 );
	m_lock.Acquire();
	for(itr = m_log.begin(); itr != m_log.end(); ++itr)
	{
		//type, player1, player2, rank, year, month, day, hour = GetGuildEventInfo(i);
		data << uint8((*itr)->iEvent);
		switch((*itr)->iEvent)
		{
		case GUILD_LOG_EVENT_DEMOTION:
		case GUILD_LOG_EVENT_PROMOTION:
			{
				data << uint64((*itr)->iEventData[0]);
				data << uint64((*itr)->iEventData[1]);
				data << uint8((*itr)->iEventData[2]);
			}break;
			
		case GUILD_LOG_EVENT_INVITE:
		case GUILD_LOG_EVENT_REMOVAL:
			{
				data << uint64((*itr)->iEventData[0]);
				data << uint64((*itr)->iEventData[1]);
			}break;

		case GUILD_LOG_EVENT_JOIN:
		case GUILD_LOG_EVENT_LEFT:
			{
				data << uint64((*itr)->iEventData[0]);
			}break;
		}

		data << uint32(UNIXTIME - (*itr)->iTimeStamp);
		if( (++count) >= 25 )
			break;
	}
//	data << uint8( GUILD_LOG_EVENT_END_LIST ); // not required ?
	data.WriteAtPos( (uint8)(count), 0 );
	m_lock.Release();

	pClient->SendPacket(&data);
}

void Guild::SendGuildRoster(WorldSession * pClient)
{
	if(pClient->GetPlayer()->m_playerInfo->guild != this)
	{ 
		return;
	}

	//huje packet. I really mean it
	WorldPacket data(SMSG_GUILD_ROSTER_COMPRESSED, (60*10) + (100 * m_members.size()) + 100);
	GuildMemberMap::iterator itr;
	uint32 counter_pos;

	m_lock.Acquire();
	uint32 struct_counter= (uint32)m_members.size();

//values need to be used : achievement points, weekly activity, total activity
/*
14333
{SERVER} Packet: (0x1B47) SMSG_GUILD_ROSTER PacketSize = 126 TimeStamp = 14220380
6D 65 73 73 61 67 65 20 6F 66 20 74 68 65 20 64 61 79 00 - MOTD
01 00 00 00 - struct counter
FA 02 00 00 00 00 00 00 - xp contribution this week
04 rogue
00 bit per player ?
01 online
2A 03 00 00 achievement points
4C 6F 73 74 6D 69 6E 64 00 name
67 75 69 6C 64 20 69 6E 66 6F 00 - guild info
6F 66 66 69 63 65 72 20 6E 6F 74 65 00 - officer note
70 75 62 6C 69 63 20 6E 6F 74 65 00 public note
B6 00 00 00 SKILL_HERBALISM
01 00 00 00 
04 00 00 00 skill level
4D 01 00 00 SKILL_ENCHANTING
01 00 00 00 
02 00 00 00 skill level
00 00 00 00 ?
00 00 00 00 rank
00 00 00 00 ?
00 00 00 00 ?
17 11 00 00 ? 4375 remaining reputation gain this week
0E 00 00 00 lastzone
00 00 00 00 00 00 00 00 - xp contrib total
A6 8D 6B 01 00 00 00 02 - guid
46 last level
00 gender

{SERVER} Packet: (0x1B47) SMSG_GUILD_ROSTER PacketSize = 368 TimeStamp = 7176030
00 04 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 04 08 04 07 00 01 01 00 00 2A 03 00 00 69 0F 00 00 FF FF FF FF 8E 17 00 00 4C 6F 73 74 6D 69 6E 64 00 44 75 6B 6B 65 72 72 00 44 72 65 73 63 61 72 00 52 C3 A3 69 64 65 6E 00 00 00 00 00 00 00 00 00 00 B6 00 00 00 01 00 00 00 04 00 00 00 4D 01 00 00 01 00 00 00 02 00 00 00 C5 00 00 00 07 00 00 00 0D 02 00 00 BA 00 00 00 07 00 00 00 0D 02 00 00 F3 02 00 00 07 00 00 00 0D 02 00 00 4D 01 00 00 07 00 00 00 0D 02 00 00 05 03 00 00 07 00 00 00 0D 02 00 00 B6 00 00 00 07 00 00 00 0D 02 00 00 00 00 00 00 00 00 00 00 FF FF FF FF 00 00 00 00 00 00 00 00 03 00 00 00 03 00 00 00 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 03 00 00 00 00 00 00 00 00 00 00 17 11 00 00 17 11 00 00 17 11 00 00 17 11 00 00 65 06 00 00 36 11 00 00 65 06 00 00 65 06 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 A6 8D 6B 01 00 00 00 02 1E F5 A5 05 00 00 00 02 77 B7 74 04 00 00 00 02 58 55 7A 05 00 00 00 02 46 55 55 55 00 01 00 00 

00 
04 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
04 08 04 07 class
00 strangemask ? 
01 01 00 00 online ?
2A 03 00 00 69 0F 00 00 FF FF FF FF 8E 17 00 00 achi
4C 6F 73 74 6D 69 6E 64 00 Lostmind 
44 75 6B 6B 65 72 72 00 Dukkerr 
44 72 65 73 63 61 72 00 Drescar 
52 C3 A3 69 64 65 6E 00 Rãiden 
00 gi
00 00 00 00 pn
00 00 00 00 on
B6 00 00 00 01 00 00 00 04 00 00 00 4D 01 00 00 01 00 00 00 02 00 00 00 
C5 00 00 00 07 00 00 00 0D 02 00 00 BA 00 00 00 07 00 00 00 0D 02 00 00 
F3 02 00 00 07 00 00 00 0D 02 00 00 4D 01 00 00 07 00 00 00 0D 02 00 00 
05 03 00 00 07 00 00 00 0D 02 00 00 B6 00 00 00 07 00 00 00 0D 02 00 00 skills
00 00 00 00 00 00 00 00 FF FF FF FF 00 00 00 00 ?
00 00 00 00 03 00 00 00 03 00 00 00 03 00 00 00 ?
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ?
00 00 00 00 00 03 00 00 00 00 00 00 00 00 00 00 ?
17 11 00 00 17 11 00 00 17 11 00 00 17 11 00 00 faction gain remaining for this week
65 06 00 00 36 11 00 00 65 06 00 00 65 06 00 00 zones
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 xp
A6 8D 6B 01 00 00 00 02 1E F5 A5 05 00 00 00 02 
77 B7 74 04 00 00 00 02 58 55 7A 05 00 00 00 02 guids
46 55 55 55 levels
00 01 00 00 genders
*/

	if( m_motd )
		data << m_motd;
	else
		data << "";

	counter_pos = (uint32)data.wpos();
	data << uint32( struct_counter );

	//guild activity total from player
	for(itr = m_members.begin(); itr != m_members.end(); ++itr)
		if( itr->second->pPlayer->guildMember )
			data << uint64( itr->second->pPlayer->guildMember->xp_contrib + 1);	
		else
			data << uint64( 0 );

	for(itr = m_members.begin(); itr != m_members.end(); ++itr)
		data << uint8( itr->second->pPlayer->_class );

	//no idea what this is
	for( uint32 i=0;i<(struct_counter+7)/8;i++)
		data << uint8( 0 );	// 1 bit per player, i wonder what it means

	for(itr = m_members.begin(); itr != m_members.end(); ++itr)
		data << uint8( itr->second->pPlayer->m_loggedInPlayer != NULL );	//online mask

	for(itr = m_members.begin(); itr != m_members.end(); ++itr)
		if( itr->second->pPlayer->m_loggedInPlayer )
			data << uint32( itr->second->pPlayer->m_loggedInPlayer->m_achievement_points );
		else
			data << uint32( 0 );

	for(itr = m_members.begin(); itr != m_members.end(); ++itr)
		if( itr->second->pPlayer )
			data << itr->second->pPlayer->name;
		else
			data << "";
	
	if( m_guildInfo )
		data << m_guildInfo;
	else
		data << "";

	for(itr = m_members.begin(); itr != m_members.end(); ++itr)
	{
		if( itr->second->pPlayer && itr->second->pPlayer->guildMember && itr->second->pPlayer->guildMember->szOfficerNote )
			data << itr->second->pPlayer->guildMember->szOfficerNote;
		else
			data << "";
	}

	for(itr = m_members.begin(); itr != m_members.end(); ++itr)
	{
		if( itr->second->pPlayer->guildMember && itr->second->pPlayer->guildMember->szPublicNote )
			data << itr->second->pPlayer->guildMember->szPublicNote ;
		else
			data << "";
	}

	//not sure what are these atm. Most increase over time for players. Some decrease. Putting fixed value to maybe be able to guess from client gui
	for(itr = m_members.begin(); itr != m_members.end(); ++itr)
		if( itr->second->pPlayer->m_loggedInPlayer )
		{
			Player *player =  itr->second->pPlayer->m_loggedInPlayer;
			data << uint32( player->GetUInt32Value( PLAYER_PROFESSION_SKILL_LINE_1 ) );
			data << uint32( 1 );	//no idea : 0,1,7
			data << uint32( player->_GetSkillLineCurrent( player->GetUInt32Value( PLAYER_PROFESSION_SKILL_LINE_1 ) ) );	
			data << uint32( player->GetUInt32Value( PLAYER_PROFESSION_SKILL_LINE_2 ) );
			data << uint32( 1 );	//no idea : 0,1,7
			data << uint32( player->_GetSkillLineCurrent( player->GetUInt32Value( PLAYER_PROFESSION_SKILL_LINE_2 ) ) );	
		}
		else
		{
			data << uint32( 0 );	
			data << uint32( 0 );	
			data << uint32( 0 );	
			data << uint32( 0 );	
			data << uint32( 0 );	
			data << uint32( 0 );	
		}

	for( uint32 i=0;i<struct_counter;i++)
		data << uint32( 0 );	//all had 0 or -1

	for(itr = m_members.begin(); itr != m_members.end(); ++itr)
		if(  itr->second->pPlayer->guildMember && itr->second->pPlayer->guildMember->pRank )
			data << itr->second->pPlayer->guildMember->pRank->iId;
		else
			data << uint32( 0 );	//this should not ever happen. But it does ? :O

	for(itr = m_members.begin(); itr != m_members.end(); ++itr)
		data << float((UNIXTIME-itr->first->lastOnline)/(60.0f*60.0f*24.0f)); //values like : 2.x -> 0.x, our value vas 0 

	for( uint32 i=0;i<struct_counter;i++)
		data << uint32( 0 );	//all had 0

	//remaining faction to be gained this week
	for( uint32 i=0;i<struct_counter;i++)
		data << uint32( 0x00001117 );	//

	for(itr = m_members.begin(); itr != m_members.end(); ++itr)
		data << uint32( itr->second->pPlayer->lastZone );	//all players have a value here. Our value is : 1855 

	//guild activity weekly from player
	for(itr = m_members.begin(); itr != m_members.end(); ++itr)
		if(  itr->second->pPlayer->guildMember )
			data << uint64( itr->second->pPlayer->guildMember->xp_contrib );	
		else
			data << uint64( 0 );	

	for(itr = m_members.begin(); itr != m_members.end(); ++itr)
		data << GET_PLAYER_GUID( itr->second->pPlayer->guid );

	for(itr = m_members.begin(); itr != m_members.end(); ++itr)
		data << uint8( itr->second->pPlayer->lastLevel );	

	for(itr = m_members.begin(); itr != m_members.end(); ++itr)
		data << uint8( itr->second->pPlayer->gender );	//not sure, met 0, 1

	//guild activity weekly from player
//	for(itr = m_members.begin(); itr != m_members.end(); ++itr)
//		if(  itr->second->pPlayer->guildMember )
//			data << uint64( 0 );	

	m_lock.Release();

	pClient->SendPacketCompressed( &data );
	return;

	/*
	WorldPacket data(SMSG_GUILD_ROSTER, (60*10) + (100 * m_members.size()) + 100);
	GuildMemberMap::iterator itr;
	GuildRank * r;
	Player * pPlayer;
	uint32 i;
	uint32 c =0;
	uint32 pos;
	GuildRank * myRank;
	bool ofnote;
	if(pClient->GetPlayer()->m_playerInfo->guild != this)
	{ 
		return;
	}

	myRank = pClient->GetPlayer()->m_playerInfo->guildRank;
	ofnote = myRank->CanPerformCommand(GR_RIGHT_VIEWOFFNOTE);

	m_lock.Acquire();

	data << uint32(m_members.size());
	if(m_motd)
		data << m_motd;
	else
		data << uint8(0);

	if(m_guildInfo)
		data << m_guildInfo;
	else
		data << uint8(0);

	pos = (uint32)data.wpos();
	data << uint32(MAX_GUILD_RANKS);

	for(i = 0; i < MAX_GUILD_RANKS; ++i)
	{
		r = m_ranks[i];
		if(r != NULL)
		{
			data << r->iRights;
			data << r->iGoldLimitPerDay;

			// i would do this with one big memcpy but grr grr struct alignment
			data << r->iTabPermissions[0].iFlags;
			data << r->iTabPermissions[0].iStacksPerDay;
			data << r->iTabPermissions[1].iFlags;
			data << r->iTabPermissions[1].iStacksPerDay;
			data << r->iTabPermissions[2].iFlags;
			data << r->iTabPermissions[2].iStacksPerDay;
			data << r->iTabPermissions[3].iFlags;
			data << r->iTabPermissions[3].iStacksPerDay;
			data << r->iTabPermissions[4].iFlags;
			data << r->iTabPermissions[4].iStacksPerDay;
			data << r->iTabPermissions[5].iFlags;
			data << r->iTabPermissions[5].iStacksPerDay;

			++c;
		}
	}

	*(uint32*)&data.contents()[pos] = c;

	for(itr = m_members.begin(); itr != m_members.end(); ++itr)
	{
		pPlayer = itr->second->pPlayer->m_loggedInPlayer;

		data << itr->first->guid;
		data << uint32(0);			// highguid
		data << uint8( (pPlayer!=NULL) ? 1 : 0 );
		data << itr->first->name;
		data << itr->second->pRank->iId;
		data << uint8(itr->first->lastLevel);
		data << uint8(itr->first->cl);
		data << uint8(itr->first->gender);
		data << itr->first->lastZone;

		if(!pPlayer)
			data << float((UNIXTIME-itr->first->lastOnline)/86400.0);

		if(itr->second->szPublicNote)
			data << itr->second->szPublicNote;
		else
			data << uint8(0);

		if(ofnote && itr->second->szOfficerNote != NULL)
			data << itr->second->szOfficerNote;
		else
			data << uint8(0);

		//protection to crash the client
		if( data.wpos() >= 30000 )
			break;
	}

	m_lock.Release();

	pClient->SendPacket(&data);
*/
}

void Guild::SendGuildQuery(WorldSession * pClient)
{
/*
13329
{SERVER} Packet: (0x365C) SMSG_GUILD_QUERY_RESPONSE PacketSize = 228 TimeStamp = 146963328
DB 30 00 00 00 80 F1 1F guild GUID
45 6D 70 69 72 65 20 4F 66 20 46 65 6C 75 63 63 61 00 Empire Of Felucca 
47 75 69 6C 64 20 4D 61 73 74 65 72 00 Guild Master
42 61 6E 6B 20 4D 61 73 74 65 72 00 Bank Master
52 61 69 64 20 4F 72 67 61 6E 69 7A 65 72 00 Raid Organizer
52 61 69 64 20 52 65 63 72 75 69 74 65 72 00 Raid Recruiter 
43 6F 6F 72 64 69 6E 61 74 6F 72 00 Coordinator
4D 65 6D 62 65 72 20 32 00 Member 2
4D 65 6D 62 65 72 00 Member
52 6F 6E 69 6E 20 32 00 Ronin 2
52 6F 6E 69 6E 00 Ronin 
00 
00 00 00 00 
01 00 00 00 
02 00 00 00 
03 00 00 00 
04 00 00 00 
05 00 00 00 
06 00 00 00 
07 00 00 00 
08 00 00 00 
08 00 00 00 
00 00 00 00 
01 00 00 00 
02 00 00 00 
03 00 00 00 
04 00 00 00 
05 00 00 00 
06 00 00 00 
07 00 00 00 
08 00 00 00 
08 00 00 00 
A3 00 00 00 
01 00 00 00 
04 00 00 00 
0E 00 00 00 
2D 00 00 00 
09 00 00 00 

{SERVER} Packet: (0x365C) SMSG_GUILD_QUERY_RESPONSE PacketSize = 167 TimeStamp = 13564552
CA 7E A9 00 00 00 F3 1F 
52 61 74 73 00 
47 75 69 6C 64 20 4D 61 73 74 65 72 00 
4F 66 66 69 63 65 72 00 
56 65 74 65 72 61 6E 00 
4D 65 6D 62 65 72 00 
49 6E 69 74 69 61 74 65 00 
00 
00 00 00 00 
00 00 00 00 
01 00 00 00 
02 00 00 00 
03 00 00 00 
04 00 00 00 
04 00 00 00 
04 00 00 00 
04 00 00 00
04 00 00 00 
04 00 00 00 
00 00 00 00 
01 00 00 00 
02 00 00 00 
03 00 00 00 
04 00 00 00 
04 00 00 00 
04 00 00 00 
04 00 00 00 
04 00 00 00 
04 00 00 00 
71 00 00 00 m_emblemStyle
0E 00 00 00 m_emblemColor
05 00 00 00 m_borderStyle
05 00 00 00 m_borderColor
20 00 00 00 m_backgroundColor
05 00 00 00 

*/
	uint32 i = 0;
	GuildRank * r;
	WorldPacket data(SMSG_GUILD_QUERY_RESPONSE, 300);
	data << GET_GUILD_GUID( m_guildId );
	data << m_guildName;

	m_lock.Acquire();
	for(i = 0; i < MAX_GUILD_RANKS; ++i)
	{
		r = m_ranks[i];
		if(r != NULL)
			data << r->szRankName;
		else
			data << uint8(0);
	}

	//these 2 are the same, seems to be the rank IDs and then the last rankid is repeated N times until max rank ID
	for(int i = 0; i < MAX_GUILD_RANKS; i++)
		data << uint32( i );                                  // maybe rank IDs in rank slots ?
	for(int i = 0; i < MAX_GUILD_RANKS; i++)
		data << uint32( i );                                  // maybe rank IDs in rank slots ?

	data << m_emblemStyle;
	data << m_emblemColor;
	data << m_borderStyle;
	data << m_borderColor;
	data << m_backgroundColor;
	data << uint32( 9 );		//unk 403

	if(pClient != NULL)
	{
		pClient->SendPacket(&data);
	}
	else
	{
		for(GuildMemberMap::iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
		{
			if(itr->first->m_loggedInPlayer)
				itr->first->m_loggedInPlayer->GetSession()->SendPacket(&data);
		}
	}

	m_lock.Release();
}

void Guild::SendGuildRankQuery( WorldSession * pClient )
{
	if( pClient == NULL )
	{ 
		return;
	}

/*
!!!! not sure this is the right query packet !!!!
{CLIENT} Packet: (0x8B42) CMSG_GUILD_RANK_QUERY PacketSize = 8 TimeStamp = 14344681
01 00 00 00 00 00 F1 1F 

{SERVER} Packet: (0x2840) SMSG_GUILD_RANK_QUERY_RESPONSE PacketSize = 451 TimeStamp = 35976170
05 00 00 00 
47 75 69 6C 64 20 4D 61 73 74 65 72 00 guild rank : Guild Master
FF FF FF FF 
FF FF FF FF 
FF FF FF FF 
FF FF FF FF 
FF FF FF FF 
FF FF FF FF 
FF FF FF FF 
FF FF FF FF 
00 00 00 00 
BF FF 1F 00 
FF FF FF FF 
FF FF FF FF 
FF FF FF FF 
FF FF FF FF 
FF FF FF FF 
FF FF FF FF 
FF FF FF FF 
FF FF FF FF 
FF FF FF FF 
00 00 00 00 - rank 
43 61 70 74 61 69 6E 00 - Captain 
07 00 00 00 
03 00 00 00 
03 00 00 00 
03 00 00 00 
00 00 00 00
00 00 00 00 
00 00 00 00 
00 00 00 00 
01 00 00 00
FF F3 1D 00 
0A 00 00 00 
64 00 00 00 
05 00 00 00 
05 00 00 00 
05 00 00 00 
00 00 00 00 
00 00 00 00
00 00 00 00 
00 00 00 00
02 00 00 00 
41 73 73 74 2E 20 47 4D 00 Asst. GM 
07 00 00 00 
07 00 00 00 
07 00 00 00
07 00 00 00 
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
02 00 00 00
FF F1 1D 00 
A0 86 01 00
A0 86 01 00
A0 86 01 00
A0 86 01 00
A0 86 01 00
00 00 00 00
00 00 00 00
00 00 00 00 
00 00 00 00
01 00 00 00 
4F 66 66 69 63 65 72 00 Officer 
03 00 00 00 
03 00 00 00
03 00 00 00
03 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00
03 00 00 00
53 40 04 00
05 00 00 00 
02 00 00 00 
02 00 00 00 
02 00 00 00 
02 00 00 00 
00 00 00 00
00 00 00 00
00 00 00 00 
00 00 00 00
04 00 00 00
49 6E 69 74 69 61 74 65 00 Initiate 
03 00 00 00 
03 00 00 00 
03 00 00 00
03 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
04 00 00 00
53 40 00 00 
05 00 00 00
02 00 00 00
01 00 00 00
01 00 00 00
01 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
03 00 00 00

14333
05 00 00 00 
BF FF DD 00 
FF FF FF FF 
FF FF FF FF 
FF FF FF FF 
FF FF FF FF 
FF FF FF FF 
FF FF FF FF 
FF FF FF FF 
FF FF FF FF 
FF FF FF FF 
FF FF FF FF 
FF FF FF FF 
FF FF FF FF 
FF FF FF FF 
FF FF FF FF 
FF FF FF FF 
FF FF FF FF 
FF FF FF FF 
47 75 69 6C 64 20 4D 61 73 74 65 72 00 - guild master
00 00 00 00 
00 00 00 00 
FF F3 1D 00 
0A 00 00 00 
07 00 00 00 
03 00 00 00 
03 00 00 00 
03 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
64 00 00 00 
05 00 00 00 
05 00 00 00 
05 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
43 61 70 74 61 69 6E 00 
02 00 00 00 
01 00 00 00 
FF F1 1D 00 
A0 86 01 00 
07 00 00 00 
07 00 00 00 
07 00 00 00 
07 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
A0 86 01 00 
A0 86 01 00 
A0 86 01 00 
A0 86 01 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
41 73 73 74 2E 20 47 4D 00 
01 00 00 00 
02 00 00 00 
53 40 04 00 05 00 00 00 03 00 00 00 03 00 00 00 03 00 00 00 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 02 00 00 00 02 00 00 00 02 00 00 00 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
4F 66 66 69 63 65 72 00 
04 00 00 00 
03 00 00 00 
53 40 00 00 05 00 00 00 03 00 00 00 03 00 00 00 03 00 00 00 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 02 00 00 00 01 00 00 00 01 00 00 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
49 6E 69 74 69 61 74 65 00 
03 00 00 00 
04 00 00 00 

*/
	WorldPacket data(SMSG_GUILD_RANK_QUERY_RESPONSE, 1100);
	uint32 counter=0;

	data << uint32( counter );
	for(uint32 i=0;i< MAX_GUILD_RANKS;i++)
		if( m_ranks[i] )
		{
			GuildRank *gr = m_ranks[i];

			data << uint32( gr->iRights );	
			data << uint32( gr->iGoldLimitPerDay );

			//need to figure this out later
			//not tab permission, something else, does seem to be permission
			for(uint32 j=0;j<MIN(MAX_GUILD_BANK_TABS,8);j++)
				data << uint32( gr->iTabPermissions[ j ].iFlags );
			for(uint32 j=0;j<MIN(MAX_GUILD_BANK_TABS,8);j++)
				data << uint32( gr->iTabPermissions[ j ].iStacksPerDay ); //maybe the place is not right ? maybe this is flags also ?

			if( gr != NULL )
				data << gr->szRankName;
			else
				data << uint8(0);

			data << uint32( gr->iId ); 
			data << uint32( counter );

			counter++;
	}
	data.WriteAtPos( counter, 0 );
	pClient->SendPacket(&data);
}

void Guild::SendTurnInPetitionResult( WorldSession * pClient, uint32 result )
{
	if( pClient == NULL )
	{ 
		return;
	}

	WorldPacket data( SMSG_TURN_IN_PETITION_RESULTS, 4 );
	data << result;
	pClient->SendPacket( &data );
}

void Guild::CreateInDB()
{
	CharacterDatabase.Execute("INSERT INTO guilds VALUES(%u, \"%s\", %u, %u, %u, %u, %u, %u, '', '', %u, 0, 0, 0, 0, 0, 0)",
		m_guildId, CharacterDatabase.EscapeString(string(m_guildName)).c_str(), m_guildLeader, m_emblemStyle, m_emblemColor, m_borderColor, m_borderStyle,
		m_backgroundColor, m_creationTimeStamp);
}

Guild* Guild::Create()
{
	Guild *g = new Guild();
	return g;
}

void Guild::BuyBankTab(WorldSession * pClient)
{
	if(pClient && pClient->GetPlayer()->GetLowGUID() != m_guildLeader)
	{ 
		return;
	}

	if(m_bankTabCount>=MAX_GUILD_BANK_TABS)
	{ 
		return;
	}

	m_lock.Acquire();

	GuildBankTab * pTab = new GuildBankTab;
	pTab->iTabId = m_bankTabCount;
	memset(&pTab->pSlots[0], 0, sizeof(Item*)*MAX_GUILD_BANK_SLOTS);
	pTab->szTabName=NULL;
	pTab->szTabIcon=NULL;
	pTab->szTabInfo = NULL;

	m_bankTabs.push_back(pTab);
	m_bankTabCount++;

	CharacterDatabase.Execute("INSERT INTO guild_banktabs VALUES(%u, %u, 'Tab %u', '_orb_3', '')", m_guildId, (uint32)pTab->iTabId,(uint32)pTab->iTabId);
	CharacterDatabase.Execute("UPDATE guilds SET bankTabCount = %u WHERE guildId = %u", m_bankTabCount, m_guildId);
	m_lock.Release();
}

uint32 GuildMember::CalculateAllowedItemWithdraws(uint32 tab)
{
	if(pRank->iTabPermissions[tab].iStacksPerDay == -1)		// Unlimited
	{ 
		return 0xFFFFFFFF;
	}
	if(pRank->iTabPermissions[tab].iStacksPerDay == 0)		// none
	{ 
		return 0;
	}

	if((UNIXTIME - uLastItemWithdrawReset[tab]) >= TIME_DAY)
	{ 
		return pRank->iTabPermissions[tab].iStacksPerDay;
	}
	else
		return (pRank->iTabPermissions[tab].iStacksPerDay - uItemWithdrawlsSinceLastReset[tab]);
}

void GuildMember::OnItemWithdraw(uint32 tab)
{
	if(pRank->iTabPermissions[tab].iStacksPerDay <= 0)		// Unlimited
	{ 
		return;
	}

	// reset the counter if a day has passed
	if(((uint32)UNIXTIME - uLastItemWithdrawReset[tab]) >= TIME_DAY)
	{
		uLastItemWithdrawReset[tab] = (uint32)UNIXTIME;
		uItemWithdrawlsSinceLastReset[tab] = 1;
		CharacterDatabase.Execute("UPDATE guild_data SET lastItemWithdrawReset%u = %u, itemWithdrawlsSinceLastReset%u = 1 WHERE playerid = %u",
			tab, uLastItemWithdrawReset[tab], tab, pPlayer->guid);
	}
	else
	{
		// increment counter
		uItemWithdrawlsSinceLastReset[tab]++;
		CharacterDatabase.Execute("UPDATE guild_data SET itemWithdrawlsSinceLastReset%u = %u WHERE playerid = %u",
			tab, uItemWithdrawlsSinceLastReset[tab], pPlayer->guid);
	}
}

uint32 GuildMember::CalculateAvailableAmount()
{
	if(pRank->iGoldLimitPerDay == -1)		// Unlimited
	{ 
		return 0xEFFFFFFF;
	}

	if(pRank->iGoldLimitPerDay == 0)
	{ 
		return 0;
	}

	if((UNIXTIME - uLastWithdrawReset) >= TIME_DAY)
	{ 
		return pRank->iGoldLimitPerDay;
	}
	else
		return (pRank->iGoldLimitPerDay - uWithdrawlsSinceLastReset);
}

void GuildMember::OnMoneyWithdraw(uint64 amt)
{
	if(pRank->iGoldLimitPerDay <= 0)		// Unlimited
	{ 
		return;
	}

	// reset the counter if a day has passed
	if(((uint32)UNIXTIME - uLastWithdrawReset) >= TIME_DAY)
	{
		uLastWithdrawReset = (uint32)UNIXTIME;
		uWithdrawlsSinceLastReset = (uint32)amt;
		CharacterDatabase.Execute("UPDATE guild_data SET lastWithdrawReset = %u, withdrawlsSinceLastReset = %u WHERE playerid = %u",
			uLastWithdrawReset, uWithdrawlsSinceLastReset, pPlayer->guid);
	}
	else
	{
		// increment counter
		uWithdrawlsSinceLastReset+=(uint32)amt;
		CharacterDatabase.Execute("UPDATE guild_data SET withdrawlsSinceLastReset = %u WHERE playerid = %u",
			uWithdrawlsSinceLastReset, pPlayer->guid);
	}
}

void Guild::DepositMoney(WorldSession * pClient, int64 uAmount)
{
#ifdef PVP_AREANA_REALM
	pClient->GetPlayer()->BroadcastMessage("This feature is disabled" );
	return;
#endif
	if((int64)pClient->GetPlayer()->GetGold() < uAmount)
	{ 
		return;
	}

	// add to the bank balance
	m_bankBalance += uAmount;

	// update in db
	CharacterDatabase.Execute("UPDATE guilds SET bankBalance = %u WHERE guildId = %u", m_bankBalance, m_guildId);

	// take the money, oh noes gm pls gief gold mi hero poor
	pClient->GetPlayer()->ModGold( -uAmount );

	// broadcast guild event telling everyone the new balance
	char buf[20];
	snprintf(buf, 20, I64FMT, (uint64)m_bankBalance);
	LogGuildEvent(GUILD_EVENT_SETNEWBALANCE, pClient->GetPlayer()->GetGUID(), 1, buf);

	// log it!
	LogGuildBankActionMoney(GUILD_BANK_LOG_EVENT_DEPOSIT_MONEY, pClient->GetPlayer()->GetLowGUID(), uAmount);
}

void Guild::WithdrawMoney(WorldSession * pClient, int64 uAmount)
{
#ifdef PVP_AREANA_REALM
	pClient->GetPlayer()->BroadcastMessage("This feature is disabled" );
	return;
#endif
	GuildMember * pMember = pClient->GetPlayer()->m_playerInfo->guildMember;
	if(pMember==NULL)
	{ 
		return;
	}

	// sanity checks
	if(pMember->pRank->iGoldLimitPerDay > 0)
	{
		if(pMember->CalculateAvailableAmount() < uAmount)
		{
			pClient->SystemMessage("You have already withdrawn too much today.");
			return;
		}
	}

	//guild owner will have -1. 0 Means amount
	if(pMember->pRank->iGoldLimitPerDay == 0)
	{
		pClient->SystemMessage("You don't have permission to do that.");
		return;
	}

	if( (int64)m_bankBalance < uAmount)
	{ 
		return;
	}

	// update his bank state
	pMember->OnMoneyWithdraw(uAmount);

	// give the gold! GM PLS GOLD PLS 1 COIN
	pClient->GetPlayer()->ModGold( uAmount );

	// subtract the balance
	m_bankBalance -= uAmount;

	// update in db
	CharacterDatabase.Execute("UPDATE guilds SET bankBalance = %u WHERE guildId = %u", (m_bankBalance>0)?m_bankBalance:0, m_guildId);

	// notify everyone with the new balance
	char buf[20];
	snprintf(buf, 20, I64FMT, (uint64)m_bankBalance);
	LogGuildEvent(GUILD_EVENT_SETNEWBALANCE, pClient->GetPlayer()->GetGUID(), 1, buf);

	// log it!
	LogGuildBankActionMoney(GUILD_BANK_LOG_EVENT_WITHDRAW_MONEY, pClient->GetPlayer()->GetLowGUID(), uAmount);
}

void Guild::SendGuildBankLog(WorldSession * pClient, uint8 iSlot)
{
	uint32 count = 0;
	if(iSlot > MAX_GUILD_BANK_TABS)
	{ 
		return;
	}

	m_lock.Acquire();
	if( iSlot == MAX_GUILD_BANK_TABS )
	{
		// sending the money log
		WorldPacket data(MSG_GUILD_BANK_LOG_QUERY, (17*m_moneyLog.size()) + 2);
		uint32 lt = (uint32)UNIXTIME;
		data << uint8( iSlot );
		data << uint8((m_moneyLog.size() < 25) ? m_moneyLog.size() : 25);
		list<GuildBankEvent*>::iterator itr = m_moneyLog.begin();
		for(; itr != m_moneyLog.end(); ++itr)
		{
			data << (*itr)->iAction;
			data << uint64( (*itr)->uPlayer );
			data << uint64( (*itr)->uEntry );
			data << uint32(lt - (*itr)->uTimeStamp);

			if( (++count) >= 25 )
				break;
		}

		m_lock.Release();
		pClient->SendPacket(&data);
	}
	else
	{
		if(iSlot >= m_bankTabCount)
		{
			m_lock.Release();
			return;
		}

		GuildBankTab * pTab = m_bankTabs[iSlot];
		if(pTab == NULL)
		{
			m_lock.Release();
			return;
		}

		WorldPacket data(MSG_GUILD_BANK_LOG_QUERY, (21*m_moneyLog.size()) + 2);
		uint32 lt = (uint32)UNIXTIME;
		data << uint8(iSlot);
		data << uint8((pTab->lLog.size() < 25) ? pTab->lLog.size() : 25);

		list<GuildBankEvent*>::iterator itr = pTab->lLog.begin();
		for(; itr != pTab->lLog.end(); ++itr)
		{
			data << (*itr)->iAction;
			data << uint64( (*itr)->uPlayer );
			data << (*itr)->uEntry;
			data << (uint32)((*itr)->iStack);
			data << uint32(lt - (*itr)->uTimeStamp);

			if( (++count) >= 25 )
				break;
		}
		
		m_lock.Release();
		pClient->SendPacket(&data);
	}
}

void Guild::LogGuildBankAction(uint8 iAction, uint32 uGuid, uint32 uEntry, uint8 iStack, GuildBankTab * pTab)
{
	GuildBankEvent * ev = new GuildBankEvent;
	uint32 timest = (uint32)UNIXTIME;
	ev->iAction = iAction;
	ev->iStack = iStack;
	ev->uEntry = uEntry;
	ev->uPlayer = uGuid;
	ev->uTimeStamp = timest;

	m_lock.Acquire();

	while(pTab->lLog.size() >= 25)
	{
		// pop one off the end
		GuildBankEvent * ev2 = *(pTab->lLog.begin());
		CharacterDatabase.Execute("DELETE FROM guild_banklogs WHERE guildid = %u AND log_id = %u",
			m_guildId, ev2->iLogId);

		pTab->lLog.pop_front();
		delete ev2;
		ev2 = NULL;
	}

	ev->iLogId = GenerateGuildLogEventId();
	pTab->lLog.push_back(ev);
	m_lock.Release();

	CharacterDatabase.Execute("INSERT INTO guild_banklogs VALUES(%u, %u, %u, %u, %u, %u, %u, %u)",
		ev->iLogId, m_guildId, (uint32)pTab->iTabId, (uint32)iAction, uGuid, uEntry, (uint32)iStack, timest);
}

void Guild::LogGuildBankActionMoney(uint8 iAction, uint32 uGuid, int64 uAmount)
{
	GuildBankEvent * ev = new GuildBankEvent;
	uint32 timest = (uint32)UNIXTIME;
	ev->iAction = iAction;
	ev->iStack = 0;
	ev->uEntry = uAmount;
	ev->uPlayer = uGuid;
	ev->uTimeStamp = timest;

	m_lock.Acquire();

	if(m_moneyLog.size() >= 25)
	{
		// pop one off the end
		GuildBankEvent * ev2 = *(m_moneyLog.begin());
		CharacterDatabase.Execute("DELETE FROM guild_banklogs WHERE guildid = %u AND log_id = %u",
			m_guildId, ev2->iLogId);

		m_moneyLog.pop_front();
		delete ev2;
		ev2 = NULL;
	}

	ev->iLogId = GenerateGuildLogEventId();
	m_moneyLog.push_back(ev);
	m_lock.Release();

	CharacterDatabase.Execute("INSERT INTO guild_banklogs VALUES(%u, %u, %u, %u, %u, %u, 0, %u)",
		ev->iLogId, m_guildId, MAX_GUILD_BANK_TABS, (uint32)iAction, uGuid, uAmount, timest);
}

void Guild::SetTabardInfo(uint32 EmblemStyle, uint32 EmblemColor, uint32 BorderStyle, uint32 BorderColor, uint32 BackgroundColor)
{
	m_emblemStyle = EmblemStyle;
	m_emblemColor = EmblemColor;
	m_borderStyle = BorderStyle;
	m_borderColor = BorderColor;
	m_backgroundColor = BackgroundColor;

	// update in db
	CharacterDatabase.Execute("UPDATE guilds SET emblemStyle = %u, emblemColor = %u, borderStyle = %u, borderColor = %u, backgroundColor = %u WHERE guildId = %u",
		EmblemStyle, EmblemColor, BorderStyle, BorderColor, BackgroundColor, m_guildId);
}

void Guild::SendGuildInfo(WorldSession * pClient)
{
	WorldPacket data(SMSG_GUILD_INFO, 20 + 20 );

	time_t ct = (time_t)m_creationTimeStamp;
	tm * pTM = localtime(&ct);

	data << m_guildName;
	data << uint32(pTM->tm_year+1900);
	data << uint32(pTM->tm_mon);
	data << uint32(pTM->tm_mday);
	data << uint32(m_members.size());
	data << uint32(m_members.size());

	pClient->SendPacket(&data);	
}

void Guild::SwapGuildRank(uint32 RId1,uint32 RId2)
{
	GuildRank *r1 = GetGuildRank( RId1 );
	GuildRank *r2 = GetGuildRank( RId2 );
	GuildRank *t1 = r2;
	GuildRank *t2 = r1;

	if( r1 == NULL || r2 == NULL )
	{
		sLog.outDebug("Could not swap ranks since one of them does not exist");
		return;
	}

	//swap ID
	r1->iId = RId2;
	r2->iId = RId1;

	//swap in list
	m_ranks[ RId1 ] = r2;
	m_ranks[ RId2 ] = r1;

	//mod them to avoid the sql key collision and error
	if( r1->szRankName )
		CharacterDatabase.Execute("UPDATE guild_ranks SET rankId = %u WHERE guildId = %u AND rankName = \"%s\" and rankId = %u ", RId1 + MAX_GUILD_RANKS,
			GetGuildId(), CharacterDatabase.EscapeString(r1->szRankName).c_str(), RId1);
	if( r2->szRankName )
		CharacterDatabase.Execute("UPDATE guild_ranks SET rankId = %u WHERE guildId = %u AND rankName = \"%s\" and rankId = %u", RId2 + MAX_GUILD_RANKS,
			GetGuildId(), CharacterDatabase.EscapeString(r2->szRankName).c_str(), RId2);
	//shift down the one before us
	if( r1->szRankName )
		CharacterDatabase.Execute("UPDATE guild_ranks SET rankId = %u WHERE guildId = %u AND rankName = \"%s\" and rankId = %u", RId2,
			GetGuildId(), CharacterDatabase.EscapeString(r1->szRankName).c_str(), RId1 + MAX_GUILD_RANKS);
	if( r2->szRankName )
		CharacterDatabase.Execute("UPDATE guild_ranks SET rankId = %u WHERE guildId = %u AND rankName = \"%s\" and rankId = %u", RId1,
			GetGuildId(), CharacterDatabase.EscapeString(r2->szRankName).c_str(), RId2 + MAX_GUILD_RANKS);
}

void Guild::SendGuildRankChangeNotify( WorldSession * pClient )
{
	if( pClient == NULL )
	{ 
		return;
	}

/*
13329 -samples, no idea about the content
{SERVER} Packet: (0x0870) UNKNOWN PacketSize = 104 TimeStamp = 17021718
00 00 00 00 00 00 00 00 01 00 00 00 60 11 D0 C9 AA 2A 00 00 FF FF FF FF 00 00 00 00 25 A0 05 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 1A 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 20 12 D0 C9 00 00 00 00 B8 11 D0 C9 AA 2A 00 00 20 12 D0 C9 AA 2A 00 00 50 62 0A 3B 00 00 00 00 10 00 00 00 
{SERVER} Packet: (0x0870) UNKNOWN PacketSize = 104 TimeStamp = 17033406
00 00 00 00 00 00 00 00 00 00 00 00 B0 FF C7 BD AA 2A 00 00 20 A2 50 B4 AA 2A 00 00 11 A1 05 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 1A 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 70 FD 70 90 FF 7F 00 00 CD 3B 98 00 00 00 00 00 90 23 EE C2 AA 2A 00 00 D8 FF C7 BD AA 2A 00 00 10 00 00 00 
{SERVER} Packet: (0x0870) UNKNOWN PacketSize = 104 TimeStamp = 17036046
00 00 00 00 00 00 00 00 FF FF FF FF 16 00 00 00 16 00 00 00 40 FD 70 90 FF 7F 00 00 33 A1 05 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 1A 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 40 FC 6B 09 00 00 00 00 B0 FD 70 90 FF 7F 00 00 01 00 00 00 00 00 00 00 40 FC 6B 09 00 00 00 00 10 00 00 00 
{SERVER} Packet: (0x0870) UNKNOWN PacketSize = 104 TimeStamp = 17036765
00 00 00 00 00 00 00 00 00 00 00 00 B0 48 EF BF AA 2A 00 00 20 A2 50 B4 AA 2A 00 00 4A A1 05 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 1A 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 70 FD 70 90 FF 7F 00 00 CD 3B 98 00 00 00 00 00 70 FD 70 90 FF 7F 00 00 D8 48 EF BF AA 2A 00 00 10 00 00 00 
{SERVER} Packet: (0x0870) UNKNOWN PacketSize = 104 TimeStamp = 17040671
00 00 00 00 00 FD 70 90 FF 7F 00 00 0C FE 70 90 FF 7F 00 00 00 FE 70 90 FF 7F 00 00 A5 A1 05 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 1A 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 F8 FD 70 90 FF 7F 00 00 F8 FD 70 90 FF 7F 00 00 D0 FD 70 90 FF 7F 00 00 68 4C AB CA AA 2A 00 00 10 00 00 00 
{SERVER} Packet: (0x0870) UNKNOWN PacketSize = 104 TimeStamp = 17044640
00 00 00 00 00 00 00 00 00 00 00 00 01 00 00 00 00 00 00 00 50 FD 70 90 FF 7F 00 00 0A A2 05 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 1A 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 20 03 00 00 00 00 00 00 70 FE 70 90 FF 7F 00 00 03 00 00 00 00 00 00 00 1C 00 00 00 00 00 00 00 10 00 00 00 
{SERVER} Packet: (0x0870) UNKNOWN PacketSize = 104 TimeStamp = 17049687
00 00 00 00 00 E3 0E 01 00 00 00 00 6E FE 70 90 FF 7F 00 00 00 00 00 00 FF FF FF FF 83 A2 05 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 1A 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01 00 00 00 00 00 00 00 40 FC 6B 09 00 00 00 00 E0 FD 70 90 FF 7F 00 00 AF 69 9C 00 00 00 00 00 10 00 00 00 
{SERVER} Packet: (0x0870) UNKNOWN PacketSize = 104 TimeStamp = 17058093
00 00 00 00 00 22 0C 01 00 00 00 00 20 FE 70 90 FF 7F 00 00 17 00 03 03 00 00 00 00 25 A3 05 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 1A 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 20 FE 70 90 FF 7F 00 00 B0 C9 2D C3 AA 2A 00 00 60 FD 70 90 FF 7F 00 00 5D A0 E9 00 00 00 00 00 10 00 00 00 
14333
{SERVER} Packet: (0x1703) UNKNOWN PacketSize = 104 TimeStamp = 12569312
13 00 00 00 00 00 00 00 00 FC 40 D2 AA 2A 00 00 30 DC CC A9 FF 7F 00 00 B9 F3 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 1A 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 D8 DB CC A9 FF 7F 00 00 A5 13 00 00 00 00 00 00 20 87 FE B4 AA 2A 00 00 B5 F3 03 00 00 00 00 00 00 00 00 00 00 00 00 00 

*/
	WorldPacket data( SMSG_GUILD_RANK_CHANGE_NOTIFY, 104+10);
	//in some way it is sent : on which rank what actions are taken
	//sending all 0 will make client print out : created
	for(uint32 i=0;i<13;i++)
	{
		data << uint32( 0 );
		data << uint32( 0 );
	}
	pClient->SendPacket(&data);
}

void Guild::SendGuildStatusUnk(WorldSession * pClient)
{
	if( pClient == NULL )
	{ 
		return;
	}
	/*
13239
{SERVER} Packet: (0x3838) SMSG_GUILD_STATUS_UNK PacketSize = 516 TimeStamp = 19588749
40 00 00 00 24 89 00 0B DF DB B1 0A F6 70 B4 0A AD F2 01 0B DC 52 04 0B EA E4 04 0B 39 CD 05 0B 18 18 01 0B 6D 35 00 0B 39 CD 05 0B A7 B0 B2 0A 47 DC B1 0A CC 55 B3 0A CC 1D 01 0B B7 1D 01 0B D9 1D 01 0B B0 1D 01 0B 31 6D B2 0A 02 B5 B2 0A 12 8D 00 0B 58 8C 00 0B 0F 90 B1 0A 0F 90 B1 0A 30 1C B7 0A 30 1C B7 0A DE 35 B6 0A 41 DC B1 0A 9E 25 B2 0A FB 6C B2 0A 9C 6C B2 0A E3 B3 B2 0A 77 24 B2 0A 79 6B B2 0A C8 B0 B2 0A 02 B5 B2 0A 02 B5 B2 0A 33 AD 01 0B BA B5 B2 0A 11 DC 02 0B AD 62 B7 0A 24 13 06 0B 72 B5 B2 0A 6E B5 B2 0A 94 B5 B2 0A DB 6C B2 0A DE C2 B2 0A CD C0 B2 0A DE C2 B2 0A CC 55 B3 0A DF DB B1 0A AA 33 B6 0A FA 34 B6 0A B2 41 B6 0A 66 D5 B6 0A EF 1D 01 0B E2 25 B2 0A 8A C3 03 0B 2D 08 B3 0A 89 9B B3 0A 8A B1 B2 0A 97 60 01 0B D0 2B B4 0A A2 51 B3 0A 4E 33 11 0B FC 12 00 00 FD 12 00 00 4F 13 00 00 50 13 00 00 54 13 00 00 5B 13 00 00 79 13 00 00 7A 13 00 00 7B 13 00 00 7C 13 00 00 7D 13 00 00 85 13 00 00 91 13 00 00 98 13 00 00 99 13 00 00 9A 13 00 00 9E 13 00 00 AD 13 00 00 B0 13 00 00 BC 13 00 00 C0 13 00 00 07 14 00 00 08 14 00 00 09 14 00 00 0A 14 00 00 0C 14 00 00 0E 14 00 00 0F 14 00 00 10 14 00 00 11 14 00 00 12 14 00 00 13 14 00 00 14 14 00 00 15 14 00 00 16 14 00 00 17 14 00 00 18 14 00 00 19 14 00 00 27 14 00 00 2B 14 00 00 2C 14 00 00 35 14 00 00 36 14 00 00 37 14 00 00 38 14 00 00 39 14 00 00 3A 14 00 00 3B 14 00 00 40 14 00 00 44 14 00 00 4C 14 00 00 51 14 00 00 77 14 00 00 99 14 00 00 9B 14 00 00 2D 15 00 00 2E 15 00 00 31 15 00 00 53 15 00 00 54 15 00 00 59 15 00 00 5A 15 00 00 5B 15 00 00 51 13 00 00 
14333
{SERVER} Packet: (0x4B27) UNKNOWN PacketSize = 452 TimeStamp = 10442472
38 00 00 00 D0 A9 46 0B 89 24 35 0B 30 E3 36 0B 39 42 45 0B F6 43 56 0B 25 83 60 0B A4 45 45 0B 56 C5 41 0B B0 05 34 0B 79 C5 41 0B 79 C5 41 0B 6A A5 44 0B 6A A5 44 0B 6A A5 44 0B E3 0A 36 0B 28 44 32 0B DE D5 32 0B D5 6D 41 0B 41 25 41 0B 41 25 41 0B 1E 2D 43 0B 03 F3 33 0B 42 95 34 0B 44 8C 32 0B EC 0D 36 0B 98 D5 32 0B B7 5D 31 0B 90 ED 31 0B 1E 63 33 0B 28 44 32 0B EC 0D 36 0B DE D5 32 0B F6 A3 31 0B F8 A3 31 0B 04 F5 33 0B F8 A3 31 0B 8D 34 32 0B CC ED 31 0B 04 F5 33 0B 89 24 35 0B 09 53 36 0B 03 8D 45 0B BA 13 44 0B 0E 18 57 0B 8F 5D 31 0B C2 25 35 0B C9 A4 31 0B D1 03 51 0B E5 CC 43 0B 7A 32 45 0B 9B 72 37 0B 05 9C 36 0B 87 9B 36 0B 87 9B 36 0B 17 D8 51 0B 57 E8 61 0B FC 12 00 00 FD 12 00 00 4F 13 00 00 50 13 00 00 51 13 00 00 54 13 00 00 5B 13 00 00 79 13 00 00 7A 13 00 00 7B 13 00 00 7C 13 00 00 7D 13 00 00 85 13 00 00 91 13 00 00 AD 13 00 00 B0 13 00 00 07 14 00 00 08 14 00 00 09 14 00 00 0A 14 00 00 0C 14 00 00 0E 14 00 00 0F 14 00 00 10 14 00 00 11 14 00 00 12 14 00 00 13 14 00 00 14 14 00 00 15 14 00 00 16 14 00 00 17 14 00 00 19 14 00 00 35 14 00 00 36 14 00 00 37 14 00 00 38 14 00 00 39 14 00 00 3A 14 00 00 3B 14 00 00 40 14 00 00 43 14 00 00 51 14 00 00 99 14 00 00 9B 14 00 00 F2 14 00 00 31 15 00 00 53 15 00 00 54 15 00 00 59 15 00 00 5A 15 00 00 5B 15 00 00 8A 16 00 00 8B 16 00 00 94 16 00 00 95 16 00 00 99 16 00 00 

38 00 00 00 - counter * gametime ?
D0 A9 46 0B 89 24 35 0B 30 E3 36 0B 39 42 45 0B F6 43 56 0B 25 83 60 0B A4 45 45 0B 56 C5 41 0B B0 05 34 0B 79 C5 41 0B 79 C5 41 0B 6A A5 44 0B 6A A5 44 0B 6A A5 44 0B E3 0A 36 0B 28 44 32 0B DE D5 32 0B D5 6D 41 0B 41 25 41 0B 41 25 41 0B 1E 2D 43 0B 03 F3 33 0B 42 95 34 0B 44 8C 32 0B EC 0D 36 0B 98 D5 32 0B B7 5D 31 0B 90 ED 31 0B 1E 63 33 0B 28 44 32 0B EC 0D 36 0B DE D5 32 0B F6 A3 31 0B F8 A3 31 0B 04 F5 33 0B F8 A3 31 0B 8D 34 32 0B CC ED 31 0B 04 F5 33 0B 89 24 35 0B 09 53 36 0B 03 8D 45 0B BA 13 44 0B 0E 18 57 0B 8F 5D 31 0B C2 25 35 0B C9 A4 31 0B D1 03 51 0B E5 CC 43 0B 7A 32 45 0B 9B 72 37 0B 05 9C 36 0B 87 9B 36 0B 87 9B 36 0B 17 D8 51 0B 57 E8 61 0B FC 12 00 00 FD 12 00 00 4F 13 00 00 50 13 00 00 51 13 00 00 54 13 00 00 5B 13 00 00 79 13 00 00 7A 13 00 00 7B 13 00 00 7C 13 00 00 7D 13 00 00 85 13 00 00 91 13 00 00 AD 13 00 00 B0 13 00 00 07 14 00 00 08 14 00 00 09 14 00 00 0A 14 00 00 0C 14 00 00 0E 14 00 00 0F 14 00 00 10 14 00 00 11 14 00 00 12 14 00 00 13 14 00 00 14 14 00 00 15 14 00 00 16 14 00 00 17 14 00 00 19 14 00 00 35 14 00 00 36 14 00 00 37 14 00 00 38 14 00 00 39 14 00 00 3A 14 00 00 3B 14 00 00 40 14 00 00 43 14 00 00 51 14 00 00 99 14 00 00 9B 14 00 00 F2 14 00 00 31 15 00 00 53 15 00 00 54 15 00 00 59 15 00 00 5A 15 00 00 5B 15 00 00 8A 16 00 00 8B 16 00 00 94 16 00 00 95 16 00 00 99 16 00 00 
*/
	sLog.outDebug("Warning, sending a temp implemented packet with static content. Need to remake this later");
	WorldPacket data( SMSG_GUILD_STATUS_GAMETIMESTAMPS, 4+10);
	data << uint32( 0 );
//	data << uint32( 1 );
//	data << uint32( TimeToGametime( UNIXTIME ) );	//maybe login logs for our guild ?
	pClient->SendPacket(&data);
}

void Guild::GiveXP(uint64 amt)
{ 
	if( m_level_XP_limit_today > amt )
	{
		m_level_XP += amt; 
		m_level_XP_limit_today -= amt;
	}
	else
	{
		m_level_XP += m_level_XP_limit_today;
		m_level_XP_limit_today = 0;
	}
	//check if our guild leveled
	if( GUILD_LEVEL_XP_REQ[ m_level ] <= m_level_XP && MAX_GUILD_LEVEL > m_level )
	{
		m_level_XP -= GUILD_LEVEL_XP_REQ[ m_level ];
		m_level++;
		m_level_XP_limit_today = GUILD_LEVEL_XP_REQ[ m_level ] / 5; //5 days / guildlevel ?

		//avoid crash related QQ
		SaveGuildState();

		GuildNewsAdd( GUILD_NEWS_LOG_LEVELED, 0, m_level );

		//update players guild level
		GuildMemberMap::iterator itr;
		m_lock.Acquire();
		for(itr = m_members.begin(); itr != m_members.end(); ++itr)
			if( itr->second->pPlayer->m_loggedInPlayer )
				itr->second->pPlayer->m_loggedInPlayer->SetUInt32Value( PLAYER_GUILDLEVEL, GetLevel() );
		m_lock.Release();
	}
	//save XP to DB every N minutes or so
	else if( m_last_XP_save_stamp < getMSTime() + 5*60*1000 )
		SaveGuildState();
}

void Guild::SendGuildXPStatus(WorldSession * pClient)
{
	if( pClient == NULL )
		return;
/*
13329
{CLIENT} Packet: (0x8B82) UNKNOWN PacketSize = 8 TimeStamp = 3864160
CA 7E A9 00 00 00 F3 1F 
{SERVER} Packet: (0x3808) UNKNOWN PacketSize = 40 TimeStamp = 3864487
00 00 00 00 00 00 00 00 
A0 FD FC 00 00 00 00 00 - 16580000 
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 
{SERVER} Packet: (0x3808) UNKNOWN PacketSize = 40 TimeStamp = 642895
A6 1B E0 00 00 00 00 00 - 14687142 - XP contrib from us total
50 D5 74 01 00 00 00 00 - 24434000 - xp req til next level
70 4E 5F 00 00 00 00 00 -  6246000 - XP gain today
30 CE 06 00 00 00 00 00 -   446000 - total XP gain 
4F 85 01 00 00 00 00 00 -    99663 - XP contrib from us today
{SERVER} Packet: (0x3808) UNKNOWN PacketSize = 40 TimeStamp = 19016390
18 DA F3 00 00 00 00 00 15981080 
25 71 A2 00 00 00 00 00 10645797 
D9 4D 06 00 00 00 00 00   413145 
3B 35 8D 00 00 00 00 00  9254203 
5A 08 18 00 00 00 00 00  1575002 
{SERVER} Packet: (0x3808) UNKNOWN PacketSize = 40 TimeStamp = 13938578
D0 05 00 00 00 00 00 00     1488 
D6 E1 0A 00 00 00 00 00   713174 
FD 9B 16 00 00 00 00 00  1481725 
CA 1B F2 00 00 00 00 00 15866826 
D0 05 00 00 00 00 00 00     1488 
{SERVER} Packet: (0x3808) UNKNOWN PacketSize = 40 TimeStamp = 5732859
00 00 00 00 00 00 00 00 38 54 3E 00 00 00 00 00 74 77 16 00 00 00 00 00 68 A9 BE 00 00 00 00 00 00 00 00 00 00 00 00 00
{SERVER} Packet: (0x3808) UNKNOWN PacketSize = 40 TimeStamp = 5828156
00 00 00 00 00 00 00 00 
86 47 3E 00 00 00 00 00  4081542 
26 84 16 00 00 00 00 00  1475622 
1A B6 BE 00 00 00 00 00 12498458 
00 00 00 00 00 00 00 00
{SERVER} Packet: (0x3808) UNKNOWN PacketSize = 40 TimeStamp = 5847203
00 00 00 00 00 00 00 00 
E9 23 3E 00 00 00 00 00  4072425 
C3 A7 16 00 00 00 00 00  1484739 
B7 D9 BE 00 00 00 00 00 12507575 
00 00 00 00 00 00 00 00
{SERVER} Packet: (0x3808) UNKNOWN PacketSize = 40 TimeStamp = 5866046
44 00 00 00 00 00 00 00 F9 15 3E 00 00 00 00 00 B3 B5 16 00 00 00 00 00 A7 E7 BE 00 00 00 00 00 44 00 00 00 00 00 00 00
{SERVER} Packet: (0x3808) UNKNOWN PacketSize = 40 TimeStamp = 5884781
44 00 00 00 00 00 00 00 A1 15 3E 00 00 00 00 00 0B B6 16 00 00 00 00 00 FF E7 BE 00 00 00 00 00 44 00 00 00 00 00 00 00
{SERVER} Packet: (0x3808) UNKNOWN PacketSize = 40 TimeStamp = 5895468
44 00 00 00 00 00 00 00 55 11 3E 00 00 00 00 00 57 BA 16 00 00 00 00 00 4B EC BE 00 00 00 00 00 44 00 00 00 00 00 00 00
{SERVER} Packet: (0x3808) UNKNOWN PacketSize = 40 TimeStamp = 5903828
44 00 00 00 00 00 00 00 55 11 3E 00 00 00 00 00 57 BA 16 00 00 00 00 00 4B EC BE 00 00 00 00 00 44 00 00 00 00 00 00 00
{SERVER} Packet: (0x3808) UNKNOWN PacketSize = 40 TimeStamp = 5923281
52 00 00 00 00 00 00 00 97 0C 3E 00 00 00 00 00 15 BF 16 00 00 00 00 00 09 F1 BE 00 00 00 00 00 52 00 00 00 00 00 00 00
{SERVER} Packet: (0x3808) UNKNOWN PacketSize = 40 TimeStamp = 5942437
52 00 00 00 00 00 00 00 E7 07 3E 00 00 00 00 00 C5 C3 16 00 00 00 00 00 B9 F5 BE 00 00 00 00 00 52 00 00 00 00 00 00 00
{SERVER} Packet: (0x3808) UNKNOWN PacketSize = 40 TimeStamp = 5961468
52 00 00 00 00 00 00 00 51 05 3E 00 00 00 00 00 5B C6 16 00 00 00 00 00 4F F8 BE 00 00 00 00 00 52 00 00 00 00 00 00 00
{SERVER} Packet: (0x3808) UNKNOWN PacketSize = 40 TimeStamp = 5980265
52 00 00 00 00 00 00 00 DE 04 3E 00 00 00 00 00 CE C6 16 00 00 00 00 00 C2 F8 BE 00 00 00 00 00 52 00 00 00 00 00 00 00
{SERVER} Packet: (0x3808) UNKNOWN PacketSize = 40 TimeStamp = 5999250
52 00 00 00 00 00 00 00 92 FF 3D 00 00 00 00 00 1A CC 16 00 00 00 00 00 0E FE BE 00 00 00 00 00 52 00 00 00 00 00 00 00
{SERVER} Packet: (0x3808) UNKNOWN PacketSize = 40 TimeStamp = 6017890
52 00 00 00 00 00 00 00 CE FD 3D 00 00 00 00 00 DE CD 16 00 00 00 00 00 D2 FF BE 00 00 00 00 00 52 00 00 00 00 00 00 00

local currentXP, nextLevelXP, dailyXP, maxDailyXP = UnitGetGuildXP("player");
GuildXPBar_SetProgress(currentXP, nextLevelXP, maxDailyXP - dailyXP);

{SERVER} Packet: (0xDF21) UNKNOWN PacketSize = 40 TimeStamp = 16040880
FA 02 00 00 00 00 00 00 //xp limit today
FA 02 00 00 00 00 00 00 //xp made today
FA 02 00 00 00 00 00 00 //maybe total XP contrib from us 
FA 02 00 00 00 00 00 00 //total xp we made until next level
A6 FA FC 00 00 00 00 00 //xp for next level
*/
	sStackWorldPacket( data, SMSG_QUERY_GUILD_XP_STATUS_TAB, 40 + 10 );
	data << uint64( GUILD_DAILY_XP_LIMIT );								//good
	data << uint64( m_level_XP );										//current XP. Will be added to total
	data << uint64( 0 );	//?
	data << uint64( GUILD_DAILY_XP_LIMIT - m_level_XP_limit_today );	//XP gained today
	data << uint64( GUILD_LEVEL_XP_REQ[ m_level ] - m_level_XP );		//Total XP until level. Current XP will be added to it
	pClient->SendPacket(&data);
}

int64 Guild::EventPlayerLootedGold( Player *p, int64 gold )
{
	int64 bank_share = gold * 10 / 100; //10% of player loot goes to guild bank
	//bank share can be moded by some player spells ( guild rank and stuff )
	m_bankBalance += bank_share;
	//the combat log will be handled (for now) externally
	return bank_share;
}

void Guild::GuildFinderAddRequest( GuildFinderRequest *r )
{
	//let's make sure there is no duplicate
	if( GuildFinderHasPendingRequest( r->guild_id ) )
		return;

	r->AddRef();
	m_guild_finder_requests.push_front( r );
}

void Guild::GuildFinderDelRequest( uint64 player_guid )
{
	//let's make sure there is no duplicate
	std::list< GuildFinderRequest *>::iterator itr;
	bool found_duplicate = false;
	for( itr = m_guild_finder_requests.begin(); itr != m_guild_finder_requests.end(); itr++ )
		if( (*itr)->player_guid == player_guid )
		{
			if( (*itr)->ref_count >= 1 )
				(*itr)->DecRef();
			m_guild_finder_requests.erase( itr );
			break;
		}
}

bool Guild::GuildFinderHasPendingRequest( uint64 guild_guid )
{
	std::list< GuildFinderRequest *>::iterator itr;
	bool found_duplicate = false;
	for( itr = m_guild_finder_requests.begin(); itr != m_guild_finder_requests.end(); itr++ )
		if( (*itr)->guild_id == guild_guid )
			return true;
	return false;
}

void Guild::GuildFinderCleanOfflines( )
{
	std::list< GuildFinderRequest *>::iterator itr,itr2;
	for( itr = m_guild_finder_requests.begin(); itr != m_guild_finder_requests.end(); )
	{
		itr2 = itr;
		itr++;
		//this is a bug
		if( (*itr2)->ref_count < 1 )
			m_guild_finder_requests.erase( itr2 );
		//this happens when player logs out
		else if( (*itr2)->ref_count == 1 )
		{
			(*itr2)->DecRef();
			m_guild_finder_requests.erase( itr2 );
		}
	}
}


void Guild::GuildNewsAdd( uint8 Type, uint64 GUID, uint32 Value )
{
	GuildNewsEvent *gne = new GuildNewsEvent;
	gne->Id = GenerateGuildLogEventId();
	gne->Type = Type;
	gne->OwnerGuid = GUID;
	gne->Value = Value;
	gne->Flags = 0;
	gne->TimeStamp = UNIXTIME;
	m_NewsLog.push_front( gne );

	CharacterDatabase.Execute("INSERT INTO guild_newslogs VALUES (%u, %u, %u, %u, %u, %u, %u)",
		(uint32)gne->Id, (uint32)m_guildId, (uint32)gne->Type, (uint32)gne->OwnerGuid, (uint32)gne->Value, (uint32)gne->TimeStamp, (uint32)gne->Flags);
}

void Guild::GuildNewsSendAll( WorldSession * pClient )
{
/*
14333
{SERVER} Packet: (0x8F41) SMSG_QUERY_GUILD_NEWS_TAB_INFO PacketSize = 40 TimeStamp = 13556674
01 00 00 00 - counter ?
7A 94 64 0B - game time
00 00 00 00 
00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 
07 00 00 00 - type ? has been founded messege appeared
32 0F AE 00 00 00 00 00 

01 00 00 00 - c
10 0D 70 0B - t
E5 64 E4 01 00 00 00 03 
00 00 00 00 
F2 14 00 00 - Everyone Needs a Logo
00 00 00 00 
00 00 00 00 - achievement
CE E2 98 00 
01 00 00 00 - sticky

local isSticky, isHeader, newsType, text1, text2, id, data, data2, weekday, day, month, year = GetGuildNewsInfo(index);
01 00 00 00 -c
65 43 73 0B -t*c
23 A4 0F 01 00 00 80 01 - playerguid*c
00 00 00 00 somecounter*c - if i put 1 it said unknown achievement
B1 13 01 00 - item ID *c
00 00 00 00 ?*c
05 00 00 00 - GUILD_NEWS_LOG_ITEM_PURCHASED * c
C5 78 93 00 ?*c
00 00 00 00 Stickied(1)?*c

*/
	list<GuildNewsEvent*>::iterator itr;
	uint32 Counter =  MIN( MAX_GUILD_NEWS, m_NewsLog.size() );
	uint32 tcounter;

	WorldPacket data(SMSG_QUERY_GUILD_NEWS_TAB_INFO, 4 + ( 4 + 8 + 4 + 8 + 4 + 4 + 4 ) * MAX_GUILD_NEWS + 10);
	data << uint32( Counter );

	for( itr = m_NewsLog.begin(), tcounter=0; itr != m_NewsLog.end() && tcounter < Counter; itr++, tcounter++ )
		data << uint32( TimeToGametime( (*itr)->TimeStamp ) );

	for( itr = m_NewsLog.begin(), tcounter=0; itr != m_NewsLog.end() && tcounter < Counter; itr++, tcounter++ )
		data << uint64( (*itr)->OwnerGuid );

	for( itr = m_NewsLog.begin(), tcounter=0; itr != m_NewsLog.end() && tcounter < Counter; itr++, tcounter++ )
		data << uint32( 0 );	//some counter. Could be the header marker which means it can have sub events ( struct in struct )

	for( itr = m_NewsLog.begin(), tcounter=0; itr != m_NewsLog.end() && tcounter < Counter; itr++, tcounter++ )
		data << uint64( (*itr)->Value );

	for( itr = m_NewsLog.begin(), tcounter=0; itr != m_NewsLog.end() && tcounter < Counter; itr++, tcounter++ )
		data << uint32( (*itr)->Type );

	for( itr = m_NewsLog.begin(), tcounter=0; itr != m_NewsLog.end() && tcounter < Counter; itr++, tcounter++ )
		data << uint32( (*itr)->Id );

	for( itr = m_NewsLog.begin(), tcounter=0; itr != m_NewsLog.end() && tcounter < Counter; itr++, tcounter++ )
		data << uint32( (*itr)->Flags );

	pClient->SendPacket(&data);
}

void Guild::GuildNewsToggleSticky( uint32 EventId, WorldSession * pClient )
{
	for( list<GuildNewsEvent*>::iterator itr = m_NewsLog.begin(); itr != m_NewsLog.end(); itr++ )
		if( (*itr)->Id == EventId )
		{
			(*itr)->Flags = 1 - (*itr)->Flags;

			WorldPacket data(SMSG_QUERY_GUILD_NEWS_TAB_INFO, 4 + ( 4 + 8 + 4 + 8 + 4 + 4 + 4 ) * 25 + 10);
			data << uint32( 1 );
			data << uint32( TimeToGametime( (*itr)->TimeStamp ) );
			data << uint64( (*itr)->OwnerGuid );
			data << uint32( 0 );	//header marker
			data << uint64( (*itr)->Value );
			data << uint32( (*itr)->Type );
			data << uint32( (*itr)->Id );
			data << uint32( (*itr)->Flags );
			pClient->SendPacket(&data);
			break;
		}
}
