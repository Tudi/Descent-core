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

static const uint32 TeamCountToId[6] = {
	0,						// 0
	0,						// 1
	0,						// 2
	ARENA_TEAM_TYPE_2V2,	// 3
	ARENA_TEAM_TYPE_3V3,	// 4
	ARENA_TEAM_TYPE_5V5,	// 5
};

static const uint32 IdToTeamCount[6] = {
	3,
	4,
	5,
	0,
	0,
	0,
};

ArenaTeam::ArenaTeam(uint32 Type, uint32 Id)
{
	m_id = Id;
	m_type = Type;
	AllocateSlots(Type);
	m_leader = 0;
	m_emblemStyle = 0;
	m_emblemColour = 0;
	m_borderColour = 0;
	m_borderStyle = 0;
	m_backgroundColour = 0;
	m_stat_rating = ARENA_TEAM_START_RATING;
	m_stat_gamesplayedweek = 0;
	m_stat_gamesplayedseason = 0;
	m_stat_gameswonseason = 0;
	m_stat_gameswonweek = 0;
	m_stat_ranking = 0;
}

ArenaTeam::ArenaTeam(Field * f)
{
	uint32 z = 0, i, guid;
	const char * data;
	int ret;

	m_id = f[z++].GetUInt32();
	m_type = f[z++].GetUInt32();
	m_leader = f[z++].GetUInt32();
	m_name = f[z++].GetString();
	m_emblemStyle = f[z++].GetUInt32();
	m_emblemColour = f[z++].GetUInt32();
	m_borderStyle = f[z++].GetUInt32();
	m_borderColour = f[z++].GetUInt32();
	m_backgroundColour = f[z++].GetUInt32();
	m_stat_rating = f[z++].GetUInt32();
	if( m_stat_rating > ARENA_TEAM_MAX_RATING ) //bug introduced at some point
		m_stat_rating = ARENA_TEAM_START_RATING;
	else if( m_stat_rating < ARENA_TEAM_MIN_RATING )
		m_stat_rating = ARENA_TEAM_MIN_RATING;
	AllocateSlots(m_type);

	m_stat_gamesplayedweek = 0;
	m_stat_gamesplayedseason = 0;
	m_stat_gameswonseason = 0;
	m_stat_gameswonweek = 0;
	sscanf(f[z++].GetString(), "%u %u %u %u", &m_stat_gamesplayedweek, &m_stat_gameswonweek, &m_stat_gamesplayedseason, &m_stat_gameswonseason);

	m_stat_ranking = f[z++].GetUInt32();
	for(i = 0; i < m_slots; ++i)
	{
		data = f[z++].GetString();
		ret = sscanf(data, "%u %u %u %u %u %u", &guid, &m_members[i].Played_ThisWeek, &m_members[i].Won_ThisWeek,
			&m_members[i].Played_ThisSeason, &m_members[i].Won_ThisSeason, &m_members[i].PersonalRating);
		if(ret >= 5)
		{
			m_members[i].Info = objmgr.GetPlayerInfo(guid);
			if( m_members[i].Info )
			{
				if( m_members[i].Info->lastOnline + 2 * MONTHLY > UNIXTIME )
					++m_memberCount;
				else
					m_members[i].Info = NULL;
			}
			if (ret == 5)
			{
				// In case PersonalRating is not in the string just set the rating to the team rating
				m_members[i].PersonalRating = m_stat_rating;
				if( m_members[i].PersonalRating > ARENA_TEAM_MAX_RATING ) //bug introduced at some point
					m_members[i].PersonalRating = ARENA_PERSONAL_START_RATING; 
				else if( m_members[i].PersonalRating < ARENA_PERSONAL_MIN_RATING )
					m_members[i].PersonalRating = ARENA_PERSONAL_MIN_RATING; 
			}
		}
		else
			m_members[i].Info = NULL;
	}
}

void ArenaTeam::SendPacket(WorldPacket * data)
{
	PlayerInfo * info;
	for(uint32 i = 0; i < m_memberCount; ++i)
	{
		info = m_members[i].Info;
		if(info && info->m_loggedInPlayer)
			info->m_loggedInPlayer->GetSession()->SendPacket(data);
	}
}

void ArenaTeam::Destroy()
{
	char buffer[1024];
	WorldPacket * data;
	vector<PlayerInfo*> tokill;
	uint32 i;
	tokill.reserve(m_memberCount);
	snprintf(buffer,1024, "The arena team, '%s', disbanded.", m_name.c_str());
	data = sChatHandler.FillSystemMessageData(buffer);
	SendPacket(data);
	delete data;
	data = NULL;

	for(i=0; i < m_memberCount; ++i)
		if(m_members[i].Info)
			tokill.push_back(m_members[i].Info);

	for(vector<PlayerInfo*>::iterator itr = tokill.begin(); itr != tokill.end(); ++itr)
		RemoveMember(*itr);

	objmgr.RemoveArenaTeam(this);

	m_memberCount = 0;
	delete [] m_members;
	m_members = NULL;

	delete this;
}

void ArenaTeam::InitMemberPlayerFields(Player *plr)
{
	uint32 base_field = (m_type*ARENA_TEAM_INFO_SIZE) + PLAYER_FIELD_ARENA_TEAM_INFO_1_1;
	plr->SetUInt32Value(base_field + ARENA_TEAM_ID, m_id);
	plr->SetUInt32Value(base_field + ARENA_TEAM_TYPE, m_type);
	plr->SetUInt32Value(base_field + ARENA_TEAM_MEMBER, m_leader != plr->GetLowGUID() );
	plr->SetUInt32Value(base_field + ARENA_TEAM_GAMES_WEEK, m_stat_gamesplayedweek );
	plr->SetUInt32Value(base_field + ARENA_TEAM_GAMES_SEASON, m_stat_gamesplayedseason );
	plr->SetUInt32Value(base_field + ARENA_TEAM_WINS_SEASON, m_stat_gameswonseason );
	plr->SetUInt32Value(base_field + ARENA_TEAM_PERSONAL_RATING, m_stat_ranking );
}

bool ArenaTeam::AddMember(PlayerInfo * info)
{
	Player * plr = info->m_loggedInPlayer;
	if(m_memberCount >= m_slots)
	{ 
		return false;
	}

	memset(&m_members[m_memberCount], 0, sizeof(ArenaTeamMember));
	m_members[m_memberCount].PersonalRating = ARENA_PERSONAL_START_RATING;
	m_members[m_memberCount++].Info = info;
	SaveToDB();

	if(plr && m_type < NUM_ARENA_TEAM_TYPES )
	{
		InitMemberPlayerFields( plr );
        plr->m_arenaTeams[m_type]=this;
		plr->GetSession()->SystemMessage("You are now a member of the arena team, '%s'.", m_name.c_str());
	}
	return true;
}

bool ArenaTeam::RemoveMember(PlayerInfo * info)
{
	//Need to fix this differently or else will leave a bad pointer !

	//there is a bug that people leave a team before the match ends to not decrease their rating on loose. No idea how it happens
//	if( info->m_loggedInPlayer && info->m_loggedInPlayer->m_bg && IS_ARENA( info->m_loggedInPlayer->m_bg->GetType() ) )
//		return false;
	//maybe we are pending to get added ?
//	if( info->m_loggedInPlayer && info->m_loggedInPlayer->m_pendingBattleground && IS_ARENA( info->m_loggedInPlayer->m_pendingBattleground->GetType() ) )
//		return false;
	//remove from queue also
	if( info->m_loggedInPlayer )
		BattlegroundManager.RemovePlayerFromQueues( info->m_loggedInPlayer );

	for(uint32 i = 0; i < m_memberCount; ++i)
	{
		if(m_members[i].Info == info)
		{
			// memcpy all the blocks in front of him back (so we only loop O(members) instead of O(slots) 
			for(uint32 j = (i+1); j < m_memberCount; ++j)
				memcpy(&m_members[j-1], &m_members[j], sizeof(ArenaTeamMember));

			--m_memberCount;
			SaveToDB();

			if(info->m_loggedInPlayer && m_type < NUM_ARENA_TEAM_TYPES )
			{
				info->m_loggedInPlayer->SetUInt32Value(PLAYER_FIELD_ARENA_TEAM_INFO_1_1 + (m_type*ARENA_TEAM_INFO_SIZE) + ARENA_TEAM_ID, 0);
				info->m_loggedInPlayer->m_arenaTeams[m_type]=0;
			}
			return true;
		}
	}

	return false;
}

//this is what player will see when pushing 'h' and looking at arena tab
void ArenaTeam::Stat(WorldPacket & data)
{
/*
{SERVER} Packet: (0x9F5F) SMSG_ARENA_TEAM_STATS PacketSize = 28 TimeStamp = 22277707
A2 69 74 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00
00 00 00 00 
*/
	data.Initialize(SMSG_ARENA_TEAM_STATS);
	data << m_id;
	data << m_stat_rating;
	data << m_stat_gamesplayedweek;
	data << m_stat_gameswonweek;
	data << m_stat_gamesplayedseason;
	data << m_stat_gameswonseason;
	data << m_stat_ranking;
}

void ArenaTeam::Query(WorldPacket & data)
{
/*
13329
{SERVER} Packet: (0xC47F) SMSG_ARENA_TEAM_QUERY_RESPONSE PacketSize = 33 TimeStamp = 22277707
A2 69 74 00 
35 6D 61 6E 00 
05 00 00 00 
70 33 4A FF 
4F 00 00 00 
C9 FA 54 FF 
02 00 00 00 
91 BD D1 FF 
14333
{SERVER} Packet: (0xFE2F) SMSG_ARENA_TEAM_QUERY_RESPONSE PacketSize = 39 TimeStamp = 1742125
9C 45 59 00 
74 75 70 6C 61 70 6F 74 74 69 00 
02 00 00 00 
E8 7B 00 FF
2F 00 00 00 
29 00 D4 FF 
04 00 00 00 
1E 00 E6 FF 
*/
	data.Initialize(SMSG_ARENA_TEAM_QUERY_RESPONSE);
	data << m_id;
	data << m_name;
	data << GetPlayersPerTeam();
	data << m_emblemColour;
	data << m_emblemStyle;
	data << m_borderColour;
	data << m_borderStyle;
	data << m_backgroundColour;
}

void ArenaTeam::Roster(WorldPacket & data)
{
/*
13329
{SERVER} Packet: (0xAC5E) SMSG_ARENA_TEAM_ROSTER PacketSize = 56 TimeStamp = 22277707
A2 69 74 00 
00 
01 00 00 00 
05 00 00 00 
43 E7 38 03 00 00 00 01 
01 
47 75 6E 62 6F 6F 74 00 
00 00 00 00 
55 level
03 class
00 00 00 00 Played_ThisWeek
00 00 00 00 Won_ThisWeek
00 00 00 00 Played_ThisSeason
00 00 00 00 Won_ThisSeason
00 00 00 00 PersonalRating
14333
{SERVER} Packet: (0xA276) SMSG_ARENA_TEAM_ROSTER PacketSize = 95 TimeStamp = 2996061
6B 22 79 00 
00 
02 00 00 00 
02 00 00 00 
8F DC 38 03 00 00 00 01 
01 
48 6F 6C 6F 77 00 
00 00 00 00 
55 
09 
01 00 00 00 
00 00 00 00 
01 00 00 00 
00 00 00 00 
00 00 00 00 
E9 D7 38 03 00 00 00 01 
01 
55 64 72 65 61 00 
01 00 00 00 
55 
08 
01 00 00 00 
00 00 00 00 
01 00 00 00 
00 00 00 00 
00 00 00 00 

*/
	data.Initialize(SMSG_ARENA_TEAM_ROSTER);
	data.reserve(m_memberCount * 81 + 9);
	data << m_id;
	data << uint8(0); // 3.0.8
	data << m_memberCount;
	data << GetPlayersPerTeam();

	for(uint32 i = 0; i < m_memberCount; ++i)
	{
		PlayerInfo* info = m_members[i].Info;
		// TODO : burlex figure out why this became null
		if( info != NULL )
		{
			data << uint64(info->guid);
			data << uint8( (info->m_loggedInPlayer != NULL) ? 1 : 0 );
			data << info->name;
			data << uint32( m_members[i].Info->guid == m_leader ? 0 : 1); // Unk
			data << uint8( info->lastLevel );
			data << uint8( info->_class );
			data << m_members[i].Played_ThisWeek;
			data << m_members[i].Won_ThisWeek;
			data << m_members[i].Played_ThisSeason;
			data << m_members[i].Won_ThisSeason;
			data << m_members[i].PersonalRating;
		}
	}
}

void ArenaTeam::SaveToDB()
{
	std::stringstream ss;
	uint32 i;
	ss << "REPLACE INTO arenateams VALUES("
		<< m_id << ","
		<< m_type << ","
		<< m_leader << ",'"
		<< CharacterDatabase.EscapeString( m_name ) << "',"
		<< m_emblemStyle << ","
		<< m_emblemColour << ","
		<< m_borderStyle << ","
		<< m_borderColour << ","
		<< m_backgroundColour << ","
		<< m_stat_rating << ",'"
		<< m_stat_gamesplayedweek << " " << m_stat_gameswonweek << " "
		<< m_stat_gamesplayedseason << " " << m_stat_gameswonseason << "',"
		<< m_stat_ranking;

	for(i = 0; i < m_memberCount; ++i)
	{
		if(m_members[i].Info)
		{
			ss << ",'" << m_members[i].Info->guid << " " << m_members[i].Played_ThisWeek << " "
				<< m_members[i].Won_ThisWeek << " " << m_members[i].Played_ThisSeason << " "
				<< m_members[i].Won_ThisSeason << " " << m_members[i].PersonalRating << "'";
		}
		else
		{
			ss << ",'0 0 0 0 0 0'";
		}
	}

	for(; i < 10; ++i)
	{
		ss << ",'0 0 0 0 0 0'";
	}

	ss << ")";
	CharacterDatabase.Execute(ss.str().c_str());
}

bool ArenaTeam::HasMember(uint32 guid)
{
	for(uint32 i = 0; i < m_memberCount; ++i)
	{
		if(m_members[i].Info && m_members[i].Info->guid == guid)
		{ 
			return true;
		}
	}
	return false;
}

void ArenaTeam::SetLeader(PlayerInfo * info)
{
	uint32 old_leader = m_leader;
	char buffer[1024];
	WorldPacket * data;
	snprintf(buffer, 1024,"%s is now the captain of the arena team, '%s'.", info->name, m_name.c_str());
	data = sChatHandler.FillSystemMessageData(buffer);
	m_leader=info->guid;
    SendPacket(data);
	delete data;
	data = NULL;

	/* set the fields */
	for(uint32 i = 0; i < m_memberCount; ++i)
	{
		if(m_members[i].Info == info)		/* new leader */
		{
			if(m_members[i].Info->m_loggedInPlayer)
				m_members[i].Info->m_loggedInPlayer->SetUInt32Value(PLAYER_FIELD_ARENA_TEAM_INFO_1_1 + (m_type*ARENA_TEAM_INFO_SIZE) + ARENA_TEAM_MEMBER, 0);
		}
		else if(m_members[i].Info->guid == old_leader)
		{
			if(m_members[i].Info->m_loggedInPlayer)
				m_members[i].Info->m_loggedInPlayer->SetUInt32Value(PLAYER_FIELD_ARENA_TEAM_INFO_1_1 + (m_type*ARENA_TEAM_INFO_SIZE) + ARENA_TEAM_MEMBER, 1);
		}
	}
}

ArenaTeamMember * ArenaTeam::GetMember(PlayerInfo * info)
{
	for(uint32 i = 0; i < m_memberCount; ++i)
	{
		if(m_members[i].Info == info)
		{ 
			return &m_members[i];
		}
	}
	return NULL;
}

ArenaTeamMember * ArenaTeam::GetMemberByGuid(uint32 guid)
{
	for(uint32 i = 0; i < m_memberCount; ++i)
	{
		if(m_members[i].Info && m_members[i].Info->guid == guid)
		{ 
			return &m_members[i];
		}
	}
	return NULL;
}

void WorldSession::HandleArenaTeamRosterOpcode(WorldPacket & recv_data)
{
	uint8 slot;
	uint32 teamId;
	ArenaTeam * team;
	recv_data >> teamId;
	team = objmgr.GetArenaTeamById(teamId);
	if(team)
	{
		slot = TeamCountToId[team->m_type];
		WorldPacket data(1000);
		team->Roster(data);
		SendPacket(&data);
	}
}

void WorldSession::HandleArenaTeamQueryOpcode(WorldPacket & recv_data)
{
	ArenaTeam * team;
	uint32 team_id;
	recv_data >> team_id;

	team = objmgr.GetArenaTeamById(team_id);
	if(team != NULL)
	{
		WorldPacket data(1000);
		team->Query(data);
		SendPacket(&data);

		team->Stat(data);
		SendPacket(&data);
	}
}

void WorldSession::HandleArenaTeamAddMemberOpcode(WorldPacket & recv_data)
{
/*
{CLIENT} Packet: (0xEF7D) CMSG_ARENA_TEAM_INVITE PacketSize = 10 TimeStamp = 22824147
AA 69 74 00 55 64 72 65 61 00 
*/
	WorldPacket data(SMSG_ARENA_TEAM_INVITE, 40);
	string player_name;
	uint32 teamId;
	recv_data >> teamId >> player_name;

	ArenaTeam * pTeam = objmgr.GetArenaTeamById(teamId);
	if( !pTeam )
	{ 
		return;
	}

	if(!pTeam->HasMember(GetPlayer()->GetLowGUID()))
	{
		GetPlayer()->SoftDisconnect();
		return;
	}

	Player * plr = objmgr.GetPlayer(player_name.c_str(), false);
	if(plr == NULL)
	{
		SystemMessage("Player `%s` is non-existent or not online.", player_name.c_str());
		return;
	}

	if(pTeam->m_leader != _player->GetLowGUID())
	{
		SystemMessage("You are not the captain of this arena team.");
		return;
	}

    if( plr->getLevel() < PLAYER_ARENA_MIN_LEVEL )
    {
        SystemMessage( "Player must be level %u to join an arena team.", PLAYER_ARENA_MIN_LEVEL );
		return;
	}

	if(plr->m_arenaTeams[pTeam->m_type] != NULL)
	{
		SystemMessage("That player is already in an arena team of this type.");
		return;
	}

	if(plr->m_arenateaminviteguid != 0)
	{
		SystemMessage("That player is already invited to an arena team");
		return;
	}

	if(plr->GetTeam() != _player->GetTeam() && !HasGMPermissions())
	{
		SystemMessage("That player is a member of a different faction.");
		return;
	}

	plr->m_arenateaminviteguid = _player->m_arenaTeams[pTeam->m_type]->m_id;
	data << _player->GetName();
	data << _player->m_arenaTeams[pTeam->m_type]->m_name;
	plr->GetSession()->SendPacket(&data);
}

void WorldSession::HandleArenaTeamRemoveMemberOpcode(WorldPacket & recv_data)
{
	ArenaTeam * team;
	uint8 slot;
	uint32 teamId;
	string name;
	PlayerInfo * inf;
	recv_data >> teamId >> name;

	team = objmgr.GetArenaTeamById(teamId);
	if( !team || team->m_type >= NUM_ARENA_TEAM_TYPES )
	{
		GetPlayer()->SoftDisconnect();
		return;
	}

	slot = team->m_type;

	if( (team = _player->m_arenaTeams[slot]) == NULL )
	{
		SystemMessage("You are not in an arena team of this type.");
		return;
	}

	if(team->m_leader != _player->GetLowGUID())
	{
		SystemMessage("You are not the leader of this team.");
		return;
	}

	if( (inf = objmgr.GetPlayerInfoByName(name.c_str())) == NULL )
	{
		SystemMessage("That player cannot be found.");
		return;
	}

	if(!team->HasMember(inf->guid))
	{
		SystemMessage("That player is not in your arena team.");
		return;
	}

	//if there is anyone inside an arena match then do not destroy it
	if( inf->m_loggedInPlayer && inf->m_loggedInPlayer->m_bg && IS_ARENA( inf->m_loggedInPlayer->m_bg->GetType() ) )
	{
		SystemMessage("The player is playing an active match.");
		return;
	}

	if(team->RemoveMember(inf))
	{
		char buffer[1024];
		WorldPacket * data;
		snprintf(buffer,1024,"%s was removed from the arena team '%s'.", inf->name, team->m_name.c_str());
		data = sChatHandler.FillSystemMessageData(buffer);
		team->SendPacket(data);
		delete data;
		data = NULL;
		SystemMessage("Removed %s from the arena team '%s'.", inf->name, team->m_name.c_str());
	}
}

void WorldSession::HandleArenaTeamInviteAcceptOpcode(WorldPacket & recv_data)
{
	ArenaTeam * team;

	if(_player->m_arenateaminviteguid == 0)
	{
		SystemMessage("You have not been invited into another arena team.");
		return;
	}

	team = objmgr.GetArenaTeamById(_player->m_arenateaminviteguid);
	_player->m_arenateaminviteguid=0;
	if(team == 0)
	{
		SystemMessage("That arena team no longer exists.");
		return;
	}

	if(team->m_memberCount >= team->m_slots)
	{
		SystemMessage("That team is now full.");
		return;
	}

	if(_player->m_arenaTeams[team->m_type] != NULL)		/* shouldn't happen */
	{
		SystemMessage("You have already been in an arena team of that size.");
		return;
	}

	if(team->AddMember(_player->m_playerInfo))
	{
		char buffer[1024];
		WorldPacket * data;
		snprintf(buffer,1024,"%s joined the arena team, '%s'.", _player->GetName(), team->m_name.c_str());
		data = sChatHandler.FillSystemMessageData(buffer);
		team->SendPacket(data);
		delete data;
		data = NULL;
	}
	else
	{
		SendNotification("Internal error.");
	}
}

void WorldSession::HandleArenaTeamInviteDenyOpcode(WorldPacket & recv_data)
{
	ArenaTeam * team;
	if(_player->m_arenateaminviteguid == 0)
	{
		SystemMessage("You were not invited.");
		return;
	}

	team = objmgr.GetArenaTeamById(_player->m_arenateaminviteguid);
	_player->m_arenateaminviteguid=0;
	if(team == NULL)
	{ 
		return;
	}

	Player * plr = objmgr.GetPlayer(team->m_leader);
	if(plr != NULL)
		plr->GetSession()->SystemMessage("%s denied your arena team invitation for %s.", _player->GetName(), team->m_name.c_str());
}

void WorldSession::HandleArenaTeamLeaveOpcode(WorldPacket & recv_data)
{
	ArenaTeam * team;
	uint32 teamId;
	recv_data >> teamId;

	team = objmgr.GetArenaTeamById(teamId);

	if( !team || team->m_type >= NUM_ARENA_TEAM_TYPES )
	{
		GetPlayer()->SoftDisconnect();
		return;
	}

	team = _player->m_arenaTeams[team->m_type];
	if( team == NULL )
	{
		SystemMessage("You are not in an arena team of this type.");
		return;
	}

	//if there is anyone inside an arena match then do not destroy it
	if( _player->m_bg && IS_ARENA( _player->m_bg->GetType() ) )
	{
		SystemMessage("You cannot leave team while inside arena.");
		return;
	}

	if (team->m_leader == _player->GetLowGUID() && team->m_memberCount == 1)
	{
		team->Destroy();
		return;
	}

	if(team->m_leader == _player->GetLowGUID())
	{
		SystemMessage("You cannot leave the team yet, promote someone else to captain first.");
		return;
	}

	if(team->RemoveMember(_player->m_playerInfo))
	{
		char buffer[1024];
		WorldPacket * data;
		snprintf(buffer,1024,"%s left the arena team, '%s'.", _player->GetName(), team->m_name.c_str());
		data = sChatHandler.FillSystemMessageData(buffer);
		team->SendPacket(data);
		delete data;
		data = NULL;
		SystemMessage("You have left the arena team, '%s'.", team->m_name.c_str());
	}
}

void WorldSession::HandleArenaTeamDisbandOpcode(WorldPacket & recv_data)
{
	ArenaTeam * team;
	uint32 teamId;
	recv_data >> teamId;

	team = objmgr.GetArenaTeamById(teamId);
	if( !team || team->m_type >= NUM_ARENA_TEAM_TYPES )
	{
		GetPlayer()->SoftDisconnect();
		return;
	}

	if( (team = _player->m_arenaTeams[team->m_type]) == NULL )
	{
		SystemMessage("You are not in an arena team of this type.");
		return;
	}

	if(team->m_leader != _player->GetLowGUID())
	{
		SystemMessage("You aren't the captain of this team.");
		return;
	}

	//if there is anyone inside an arena match then do not destroy it
	for( uint32 i=0;i<team->m_memberCount;i++)
	{
		ArenaTeamMember *member = team->GetMemberBySlot( i );
		if( member && member->Info && member->Info->m_loggedInPlayer && member->Info->m_loggedInPlayer->m_bg &&	IS_ARENA( member->Info->m_loggedInPlayer->m_bg->GetType() ) )
		{
			SystemMessage("Someone is playing an active Arena match.");
			return;
		}
	}
	team->Destroy();
}

void WorldSession::HandleArenaTeamPromoteOpcode(WorldPacket & recv_data)
{
	uint32 teamId;
	uint8 slot;
	string name;
	ArenaTeam * team;
	PlayerInfo * inf;
	recv_data >> teamId >> name;

	team = objmgr.GetArenaTeamById(teamId);
	if(!team)
	{
		GetPlayer()->SoftDisconnect();
		return;
	}

	slot = team->m_type;

	if( slot >= NUM_ARENA_TEAM_TYPES )
	{ 
		return;
	}

	if( (team = _player->m_arenaTeams[slot]) == NULL )
	{
		SystemMessage("You are not in an arena team of this type.");
		return;
	}

	if(team->m_leader != _player->GetLowGUID())
	{
		SystemMessage("You aren't the captain of this team.");
		return;
	}

	if( (inf = objmgr.GetPlayerInfoByName(name.c_str())) == NULL )
	{
		SystemMessage("That player cannot be found.");
		return;
	}

	if(!team->HasMember(inf->guid))
	{
		SystemMessage("That player is not a member of your arena team.");
		return;
	}

	team->SetLeader(inf);
}


void WorldSession::HandleArenaTeamCreate(WorldPacket & recv_data)
{
/*
13329
{CLIENT} Packet: (0x0109) CMSG_ARENA_TEAM_CREATE PacketSize = 29 stamp = 3919291
05 00 00 00 - size
01 00 00 00 border
C9 7D F5 FF bordercolor
3E 00 00 00 icon
80 66 80 FF iconcolor
85 80 8A FF background
35 6D 61 6E 00  
144333
{CLIENT} Packet: (0x1050) CMSG_ARENA_TEAM_CREATE PacketSize = 33 stamp = 31580124
36 00 00 00 icon
BA 08 29 FF 
1F 6E AB FF 
E8 57 AD FF 
03 00 00 00 border
00 00 00 00 size 2
61 6C 6C 67 75 69 64 61 00                                              
*/
	uint32 icon, iconcolor, bordercolor, border, backgroundColour;
	recv_data >> icon >> iconcolor >> backgroundColour >> bordercolor >> border;
	uint32 type;
	recv_data >> type;
	switch(type)
	{
	case 0:
		type = ARENA_TEAM_TYPE_2V2;
		break;

	case 1:
		type = ARENA_TEAM_TYPE_3V3;
		break;

	case 2:
		type = ARENA_TEAM_TYPE_5V5;
		break;

	default:
		SendNotification("Internal Error");
		return;
	}
	string name;
	recv_data >> name;
	if( _player->m_arenaTeams[ type ] != NULL)
	{
		sChatHandler.SystemMessage(this, "You are already in an arena team.");
		return;
	}

	if( name.length() <= 1 )
	{
		_player->BroadcastMessage("Arena team name too short");
		return;
	}
	if( name.length() >= 20 )
	{
		_player->BroadcastMessage("Arena team name too long");
		return;
	}

	ArenaTeam * team;
	team = new ArenaTeam(type, objmgr.GenerateArenaTeamId());
	team->m_name = name;
	team->m_emblemColour = iconcolor;
	team->m_emblemStyle = icon;
	team->m_borderColour = bordercolor;
	team->m_borderStyle = border;
	team->m_backgroundColour = backgroundColour;
	team->m_leader = _player->GetLowGUID();
	team->m_stat_rating=ARENA_TEAM_START_RATING;
    
	objmgr.AddArenaTeam( team );
	objmgr.UpdateArenaTeamRankings();
	team->AddMember( _player->m_playerInfo );

	sStackWorldPacket( data, SMSG_ARENA_TEAM_COMMAND_RESULT, 19+20 );
	data << name;	
	data << uint32( CR_SUCCESS );	
	data << uint32( 0 );
	data << uint8( 0 );	//string for command result
	SendPacket( &data );

	data.Initialize( SMSG_ARENA_TEAM_COMMAND_RESULT );
	data << name;	
	data << uint32( CR_FOUNDING_OWNER );	
	data << uint32( 0 );
	data << uint8( 0 );	//string for command result
	SendPacket( &data );
/*
	{SERVER} Packet: (0x1898) SMSG_ARENA_TEAM_COMMAND_RESULT PacketSize = 13 TimeStamp = 21946407
	31 32 33 00 00 00 00 00 00 00 00 00 00
	{SERVER} Packet: (0x1898) SMSG_ARENA_TEAM_COMMAND_RESULT PacketSize = 13 TimeStamp = 21946610
	31 32 33 00 0E 00 00 00 00 00 00 00 00 
*/
}