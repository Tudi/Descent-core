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

//safe way to handle arrays in debug mode
#include <array>

#define SHADOW_SIGHT_TRIGGER_RADIUS 10
#ifndef BUFF_RESPAWN_TIME
	#define BUFF_RESPAWN_TIME 90000
#endif


class ArenaTeam;

class Arena : public CBattleground
{
	set<GameObject*>			m_gates;
	tr1::array<GameObject *,2>	m_buffs;
	tr1::array<ArenaTeam *,2>	m_teams;
	tr1::array<uint32,2>		m_teamsID; //this is needed when players queue, delete the team, then join the fight to create mem corruption
	uint32						m_arenateamtype;
	tr1::array<uint32,2>		m_playersCount;
	set<uint32>					m_players2[2];
	set<uint32>					m_playersAlive;
	bool						m_EnoughTeamsJoined;
public:
	bool rated_match;
	Arena(MapMgr * mgr, uint32 id, uint32 lgroup, uint32 t, uint32 players_per_side);
	virtual ~Arena();

	bool HookHandleRepop(Player * plr);
	void OnAddPlayer(Player * plr);
	void OnRemovePlayer(Player * plr);
	void OnCreate();
	void HookOnPlayerDeath(Player * plr);
	void HookOnPlayerKill(Player * plr, Player * pVictim);
	void HookOnUnitKill(Player * plr, Unit * pVictim);
	void HookOnFlagDrop(Player * plr);
	void HookOnHK(Player * plr);
	void HookOnShadowSight();
	void HookGenerateLoot( Player * plr, Object * pCorpse );
	void UpdatePlayerCounts();
	LocationVector GetStartingCoords(uint32 Team);
	uint32 GetNameID() { return 50; }
	void OnStart();
	bool CanPlayerJoin(Player * plr, uint32 type)
	{
		if(m_started)
			return false;
		else
			return CBattleground::CanPlayerJoin(plr,type);
	}

	bool CreateCorpse(Player * plr) { return false; }

	/* dummy stuff */
	void HookOnMount(Player * plr) {}
	void HookFlagDrop(Player * plr, GameObject * obj) {}
	void HookFlagStand(Player * plr, GameObject * obj) {}
	void HookOnAreaTrigger(Player * plr, uint32 id);

	int32 GetFreeTeam()
	{
		size_t c0 = m_players[0].size() + m_pendPlayers[0].size();
		size_t c1 = m_players[1].size() + m_pendPlayers[1].size();
		if(m_started) return -1;

		// Check if there is free room, if yes, return team with less members
		return ((c0 + c1 >= m_playerCountPerTeam * 2) ? -1 : (c0 > c1));

		/* We shouldn't reach here. */
	}

	void Finish();
	ARCEMU_INLINE uint8 Rated() { return rated_match; }
	ARCEMU_INLINE uint8 GetArenaTeamType() { return m_arenateamtype; }
//	ARCEMU_INLINE ArenaTeam ** GetTeams() { return m_teams; }
	ARCEMU_INLINE ArenaTeam * GetTeam(uint32 ind) { return m_teams[ind]; }
	uint32 CalcDeltaRating(uint32 oldRating, uint32 opponentRating, bool outcome);
	bool HasStarted() { return m_started; }
};
