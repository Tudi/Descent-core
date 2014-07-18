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

#ifndef arcemu_ARENATEAMS_H
#define arcemu_ARENATEAMS_H

struct ArenaTeamMember
{
	PlayerInfo * Info;
	uint32 Played_ThisWeek;
	uint32 Won_ThisWeek;
	uint32 Played_ThisSeason;
	uint32 Won_ThisSeason;
	int32 PersonalRating;
};

enum ArenaTeamInfoType
{
	ARENA_TEAM_ID               = 0,
	ARENA_TEAM_TYPE             = 1,                        // new in 3.2
	ARENA_TEAM_MEMBER           = 2,                        // 0 - captain, 1 - member
	ARENA_TEAM_GAMES_WEEK       = 3,
	ARENA_TEAM_GAMES_SEASON     = 4,
	ARENA_TEAM_WINS_SEASON      = 5,
	ARENA_TEAM_PERSONAL_RATING  = 6,
	ARENA_TEAM_INFO_SIZE        = 7
};

enum ArenaTeamCommandResult
{
	CR_SUCCESS					= 0x00,
	CR_ERROR					= 0x01,
	CR_FOUNDING_OWNER			= 0x0E,
	//sub error codes ( when result is error )
	CR_ERROR_ALREADY_IN_TEAM	= 0x03,
	CR_ERROR_TOO_LOW_LEVEL		= 0x15,
};

enum ArenaTeamCommandLogType
{
	CL_JOINED					= 0x04,	//or maybe logged in ?
	CL_LEFT						= 0x09,	//or maybe logged out ?
};

//need to remake this to be calculated dynamic based on rating
#define ARENA_CURRENCY_WIN_AMT_MIN	5
#define ARENA_CURRENCY_WIN_AMT2		30
#define ARENA_CURRENCY_WIN_AMT3		60
#define ARENA_CURRENCY_WIN_AMT5		100
#define ARENA_TEAM_START_RATING		1500
#define ARENA_TEAM_MIN_RATING		0
#define ARENA_PERSONAL_START_RATING 0	
#define ARENA_PERSONAL_MIN_RATING	0	
#define ARENA_TEAM_MAX_RATING		6000	

class SERVER_DECL ArenaTeam
{
	void AllocateSlots(uint32 Type)
	{
		uint32 Slots = 0;
		if(Type == ARENA_TEAM_TYPE_2V2)
			Slots=4;
		else if(Type == ARENA_TEAM_TYPE_3V3)
			Slots=6;
		else if(Type == ARENA_TEAM_TYPE_5V5)
			Slots=10;
		ASSERT(Slots);
		m_members = new ArenaTeamMember[Slots];
		memset(m_members,0,sizeof(ArenaTeamMember)*Slots);
		m_slots = Slots;
		m_memberCount=0;
	}

public:

	uint32 m_id;
	uint32 m_type;
	uint32 m_leader;
	uint32 m_slots;
	uint32 m_memberCount;
	ArenaTeamMember * m_members;
	string m_name;

	uint32 m_emblemStyle;
	uint32 m_emblemColour;
	uint32 m_borderStyle;
	uint32 m_borderColour;
	uint32 m_backgroundColour;

	int32 m_stat_rating;
	uint32 m_stat_gamesplayedweek;
	uint32 m_stat_gameswonweek;
	uint32 m_stat_gamesplayedseason;
	uint32 m_stat_gameswonseason;
	uint32 m_stat_ranking;

	ArenaTeam(uint32 Type, uint32 Id);
	ArenaTeam(Field * f);
	~ArenaTeam()
	{
		delete [] m_members;
		m_members = NULL;
		m_slots = 0;
		m_memberCount = 0;
	}

	void SendPacket(WorldPacket * data);
	void Query(WorldPacket & data);
	void Stat(WorldPacket & data);
	void Roster(WorldPacket & data);
	void Inspect(WorldPacket & data);
	void Destroy();
	void SaveToDB();

	bool AddMember(PlayerInfo * info);
	bool RemoveMember(PlayerInfo * info);
	bool HasMember(uint32 guid);
	void SetLeader(PlayerInfo * info);
	void InitMemberPlayerFields(Player *plr);
	ArenaTeamMember * GetMember(PlayerInfo * info);
	ArenaTeamMember * GetMemberByGuid(uint32 guid);
	ArenaTeamMember * GetMemberBySlot(uint32 slot)
	{
		if( slot < m_slots && m_members )
			return &m_members[slot];
		return NULL;
	}

	uint32 GetPlayersPerTeam()
	{
		switch(m_type)
		{
		case ARENA_TEAM_TYPE_2V2:
			return 2;

		case ARENA_TEAM_TYPE_3V3:
			return 3;

		case ARENA_TEAM_TYPE_5V5:
			return 5;
		}

		// never reached
		return 2;
	}
};

#endif		// arcemu_ARENATEAMS_H
