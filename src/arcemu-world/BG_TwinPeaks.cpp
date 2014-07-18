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

TwinPeaks::TwinPeaks(MapMgr * mgr, uint32 id, uint32 lgroup, uint32 t) : CBattleground(mgr, id, lgroup, t)
{
	int i;

	for (i=0; i<2; i++) {
		m_players[i].clear();
		m_pendPlayers[i].clear();
	}
	//m_worldStates.clear();
	m_pvpData.clear();
	m_resurrectMap.clear();

	m_flagHolders[0] = m_flagHolders[1] = 0;
	m_lgroup = lgroup;
	
	/* create the buffs */
	for(i = 0; i < 6; ++i)
		SpawnBuff(i);

	/* take note: these are swapped around for performance bonus */
	// warsong flag - horde base
	m_homeFlags[0] = SpawnGameObject(179831, 726, 1578.34f, 344.045f, 2.41841f, 2.79252f, 0, 1997, 2.5f);
	m_homeFlags[0]->SetByte(GAMEOBJECT_BYTES_1, 0, 1);
	m_homeFlags[0]->SetByte(GAMEOBJECT_BYTES_1, 1, 24);
	m_homeFlags[0]->SetByte(GAMEOBJECT_BYTES_1, 3, 100);

	// silverwing flag - alliance base
	m_homeFlags[1] = SpawnGameObject(179830, 726, 2117.64f, 191.682f, 44.052f, 6.02139f, 0,1995, 2.5f);
	m_homeFlags[1]->SetByte(GAMEOBJECT_BYTES_1, 0, 1);
	m_homeFlags[1]->SetByte(GAMEOBJECT_BYTES_1, 1, 24);
	m_homeFlags[1]->SetByte(GAMEOBJECT_BYTES_1, 3, 100);

	// dropped flags
	m_dropFlags[1] = m_mapMgr->CreateGameObject(179786);
	if(!m_dropFlags[1]->CreateFromProto(179785, 726, 0, 0, 0, 0))
		Log.Warning("TwinPeaks", "Could not create dropped flag 1");

	m_dropFlags[0] = m_mapMgr->CreateGameObject(179786);
	if(!m_dropFlags[0]->CreateFromProto(179786, 726, 0, 0, 0, 0))
		Log.Warning("TwinPeaks", "Could not create dropped flag 0");

	for(i = 0; i < 2; ++i)
	{
		m_dropFlags[i]->SetUInt32Value(GAMEOBJECT_DYNAMIC, 1);
		m_dropFlags[i]->SetFloatValue(OBJECT_FIELD_SCALE_X, 2.5f);
	}

	m_scores[0] = m_scores[1] = 0;

}

TwinPeaks::~TwinPeaks()
{
	// gates are always spawned, so mapmgr will clean them up
	for(uint32 i = 0; i < 6; ++i)
	{
		// buffs may not be spawned, so delete them if they're not
		if( m_buffs[i] ) 
		{
			if( m_buffs[i]->IsInWorld() == true )
				m_buffs[i]->RemoveFromWorld( true );
			sGarbageCollection.AddObject( m_buffs[i] );
			m_buffs[i] = NULL;
		}
	}

	for(uint32 i = 0; i < 2; ++i)
	{
		if(m_dropFlags[i])
		{
			if( m_dropFlags[i]->IsInWorld() == true )
				m_dropFlags[i]->RemoveFromWorld( true );
			sGarbageCollection.AddObject( m_dropFlags[i] );
			m_dropFlags[i] = NULL;
		}

		if(m_homeFlags[i])
		{
			if( m_homeFlags[i]->IsInWorld() == true )
				m_homeFlags[i]->RemoveFromWorld( true );
			sGarbageCollection.AddObject( m_homeFlags[i] );
			m_homeFlags[i] = NULL;
		}
	}

	//we already despawned when BG started
	for(list<GameObject*>::iterator itr = m_gates.begin(); itr != m_gates.end(); ++itr)
		sGarbageCollection.AddObject( (*itr) );
	m_gates.clear();

	m_resurrectMap.clear();
	//m_worldStates.clear();
}

void TwinPeaks::HookOnAreaTrigger(Player * plr, uint32 id)
{
	int32 buffslot = -1;
	switch(id)
	{
	case 5906:	  // Speed
		buffslot = 0;
		break;
	case 5907:	  // Speed (Horde)
		buffslot = 1;
		break;
	case 5909:	  // Restoration
		buffslot = 2;
		break;
	case 5908:	  // Restoration (Horde)
		buffslot = 3;
		break;
	case 5911:	  // Berserking
		buffslot = 4;
		break;
	case 5910:	  // Berserking (Horde)
		buffslot = 5;
		break;
	}

	if(buffslot >= 0)
	{
		if(m_buffs[buffslot] != 0 && m_buffs[buffslot]->IsInWorld())
		{
			/* apply the buff */
			SpellEntry * sp = dbcSpell.LookupEntry(m_buffs[buffslot]->GetInfo()->sound3);
			Spell * s = SpellPool.PooledNew( __FILE__, __LINE__ );
			s->Init(plr, sp, true, 0);
			SpellCastTargets targets(plr->GetGUID());
			s->prepare(&targets);

			/* despawn the gameobject (not delete!) */
			m_buffs[buffslot]->Despawn(0, BUFF_RESPAWN_TIME);
		}
		return;
	}

	//Alliance is team 0 :P
	if(((id == 5904 && plr->GetTeam() == 0) || (id == 5905 && plr->GetTeam() == 1)) && (plr->m_bgHasFlag && m_flagHolders[plr->GetTeam()] == plr->GetLowGUID()))
	{
		if(m_flagHolders[plr->GetTeam() ? 0 : 1] != 0 || m_dropFlags[plr->GetTeam() ? 0 : 1]->IsInWorld())
		{
			/* can't cap while flag dropped */
			return;
		}
		float distance = (plr->GetTeam() == 0)? plr->CalcDistance(2118, 190, 44) : plr->CalcDistance(1578, 343, 2.5f);
		if (distance > 10.0f)
		{
			//50 yards from the spawn, gtfo hacker.
			sCheatLog.writefromsession(plr->GetSession(),"Tried to capture the flag in WSG while being more then 50 yards away. (%f yards)",plr->CalcDistance(915.367f, 1433.78f, 346.089f));
			plr->SoftDisconnect();
			return;
		}

		/* remove the bool from the player so the flag doesn't drop */
		m_flagHolders[plr->GetTeam()] = 0;
		plr->m_bgHasFlag = 0;
		
		/* remove flag aura from player */
		plr->RemoveAura(23333+(plr->GetTeam() * 2),0,AURA_SEARCH_POSITIVE);

		/* capture flag points */
		plr->m_bgScore.MiscData[BG_SCORE_WSG_FLAGS_CAPTURED]++;

		PlaySoundToAll( plr->GetTeam() ? SOUND_HORDE_SCORES : SOUND_ALLIANCE_SCORES );

		if( plr->GetTeam() == 1 )
			SendChatMessage( CHAT_MSG_BG_EVENT_HORDE, plr->GetGUID(), "%s captured the Alliance flag!", plr->GetName() );
		else
			SendChatMessage( CHAT_MSG_BG_EVENT_ALLIANCE, plr->GetGUID(), "%s captured the Horde flag!", plr->GetName() );

		SetWorldState( plr->GetTeam() ? WSG_ALLIANCE_FLAG_CAPTURED : WSG_HORDE_FLAG_CAPTURED, 1 );

		// Remove the Other Flag
		if (m_homeFlags[plr->GetTeam() ? 0 : 1]->IsInWorld())
			m_homeFlags[plr->GetTeam() ? 0 : 1]->RemoveFromWorld(false);

		// Add the Event to respawn the Flags
		if( sEventMgr.HasEvent( this, EVENT_BATTLEGROUND_WSG_AUTO_RETURN_FLAG ) == false )
			sEventMgr.AddEvent(this, &TwinPeaks::EventReturnFlags, EVENT_BATTLEGROUND_WSG_AUTO_RETURN_FLAG, 20000, 1, 0);

		/* give each player on that team bonus honor and reputation*/
		int32 honorToAdd = 2 * m_honorPerKill;
		uint32 repToAdd = m_isWeekend ? 45 : 35;
		uint32 fact = plr->GetTeam() ? 889 : 890; /*Warsong Outriders : Sliverwing Sentinels*/
		for(InRangePlayerSet::iterator itr = m_players[plr->GetTeam()].begin(); itr != m_players[plr->GetTeam()].end(); ++itr)
		{
			(*itr)->m_bgScore.BonusHonor += honorToAdd;
			HonorHandler::AddHonorPointsToPlayer((*itr), honorToAdd);
			plr->ModStanding(fact, repToAdd);
		}
		m_scores[plr->GetTeam()]++;
//m_scores[plr->GetTeam()] = 3;
		if(m_scores[plr->GetTeam()] == 3)
		{
			/* victory! */
			m_ended = true;
			m_winningteam = plr->GetTeam() ? 1 : 0;
			EventOnEndedPreCleanup();
			m_nextPvPUpdateTime = 0;

			sEventMgr.RemoveEvents(this, EVENT_BATTLEGROUND_CLOSE);
			sEventMgr.AddEvent(static_cast<CBattleground*>(this), &CBattleground::Close, EVENT_BATTLEGROUND_CLOSE, 120000, 1,0);

			/* add the marks of honor to all players */
//			SpellEntry * winner_spell = dbcSpell.LookupEntry(24951);
//			SpellEntry * loser_spell = dbcSpell.LookupEntry(24950);
			uint32 lostHonorToAdd = m_isWeekend ? 4 * m_honorPerKill : 2 * m_honorPerKill;
			uint32 winHonorToAdd = lostHonorToAdd + (m_isWeekend ? 3 * m_honorPerKill : m_honorPerKill);
			
			m_mainLock.Acquire();
			for(uint32 i = 0; i < 2; ++i)
			{
				for(InRangePlayerSet::iterator itr = m_players[i].begin(); itr != m_players[i].end(); ++itr)
				{
					(*itr)->Root();
					(*itr)->Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_BATTLEGROUND,(*itr)->GetMapId(),ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
					if(i == m_winningteam)
					{
						(*itr)->Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_WIN_BG,(*itr)->GetMapId(),ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
						(*itr)->m_bgScore.BonusHonor += winHonorToAdd;
						HonorHandler::AddHonorPointsToPlayer((*itr), winHonorToAdd);
//						(*itr)->CastSpell((*itr), winner_spell, true);
#ifdef BATTLEGRUND_REALM_BUILD
						(*itr)->GetItemInterface()->AddItemById( 20558, 3, 0 );
#endif
						if(i && (*itr)->GetQuestLogForEntry(11342))
							(*itr)->GetQuestLogForEntry(11342)->SendQuestComplete();
						else if((*itr)->GetQuestLogForEntry(11338))
							(*itr)->GetQuestLogForEntry(11338)->SendQuestComplete();
					}
					else
					{
						(*itr)->m_bgScore.BonusHonor += lostHonorToAdd;
						HonorHandler::AddHonorPointsToPlayer((*itr), lostHonorToAdd);
#ifdef BATTLEGRUND_REALM_BUILD
						(*itr)->GetItemInterface()->AddItemById( 20558, 1, 0 );
#endif
//						(*itr)->CastSpell((*itr), loser_spell, true);
					}
				}
			}
			m_mainLock.Release();
		}

		/* increment the score world state */
		SetWorldState(plr->GetTeam() ? WSG_CURRENT_HORDE_SCORE : WSG_CURRENT_ALLIANCE_SCORE, m_scores[plr->GetTeam()]);

		UpdatePvPData();
	}
}

void TwinPeaks::EventReturnFlags()
{
	for (uint32 x = 0; x < 2; x++)
		if(m_homeFlags[x] != NULL)
			m_homeFlags[x]->PushToWorld(m_mapMgr);
	SendChatMessage(CHAT_MSG_BG_EVENT_NEUTRAL, 0, "The Alliance's flag is now placed at her base.");
	SendChatMessage(CHAT_MSG_BG_EVENT_NEUTRAL, 0, "The Horde's flag is now placed at her base.");
}

void TwinPeaks::HookOnFlagDrop(Player * plr)
{
	if(!plr->m_bgHasFlag || m_dropFlags[plr->GetTeam()]->IsInWorld())
	{ 
		return;
	}

	/* drop the flag! */
	m_dropFlags[plr->GetTeam()]->SetPosition(plr->GetPosition());
	m_dropFlags[plr->GetTeam()]->PushToWorld(m_mapMgr);

	m_flagHolders[plr->GetTeam()] = 0;
	plr->m_bgHasFlag = false;
	plr->RemoveAura(23333 + (plr->GetTeam() * 2),0,AURA_SEARCH_POSITIVE);

	SetWorldState(plr->GetTeam() ? WSG_ALLIANCE_FLAG_CAPTURED : WSG_HORDE_FLAG_CAPTURED, 1);

	sEventMgr.AddEvent( this, &TwinPeaks::ReturnFlag, plr->GetTeam(), EVENT_BATTLEGROUND_WSG_AUTO_RETURN_FLAG + plr->GetTeam(), 5000, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );

	if( plr->GetTeam() == 1 )
		SendChatMessage( CHAT_MSG_BG_EVENT_ALLIANCE, plr->GetGUID(), "The Alliance flag was dropped by %s!", plr->GetName() );
	else
		SendChatMessage( CHAT_MSG_BG_EVENT_HORDE, plr->GetGUID(), "The Horde flag was dropped by %s!", plr->GetName() );
}

void TwinPeaks::HookFlagDrop(Player * plr, GameObject * obj)
{
	/* picking up a dropped flag */
	if(m_dropFlags[plr->GetTeam()] != obj)
	{
		/* are we returning it? */
		if( (obj->GetEntry() == 179785 && plr->GetTeam() == 0) ||
			(obj->GetEntry() == 179786 && plr->GetTeam() == 1) )
		{
			uint32 x = plr->GetTeam() ? 0 : 1;
			sEventMgr.RemoveEvents(this, EVENT_BATTLEGROUND_WSG_AUTO_RETURN_FLAG + ( plr->GetTeam() ? 0 : 1 ) );

			if( m_dropFlags[x]->IsInWorld() )
				m_dropFlags[x]->RemoveFromWorld(false);

			if(m_homeFlags[x]->IsInWorld() == false)
				m_homeFlags[x]->PushToWorld(m_mapMgr);

			plr->m_bgScore.MiscData[BG_SCORE_WSG_FLAGS_RETURNED]++;
			UpdatePvPData();

			if( plr->GetTeam() == 1 )
				SendChatMessage( CHAT_MSG_BG_EVENT_HORDE, plr->GetGUID(), "The Horde flag was returned to its base by %s!", plr->GetName() );
			else
				SendChatMessage( CHAT_MSG_BG_EVENT_ALLIANCE, plr->GetGUID(), "The Alliance flag was returned to its base by %s!", plr->GetName() );

			SetWorldState(plr->GetTeam() ? WSG_ALLIANCE_FLAG_CAPTURED : WSG_HORDE_FLAG_CAPTURED, 1);
			PlaySoundToAll(plr->GetTeam() ? SOUND_HORDE_RETURNED : SOUND_ALLIANCE_RETURNED);
		}
		return;
	}

	map<uint32,uint32>::iterator itr = plr->m_forcedReactions.find(1059);
	if (itr != plr->m_forcedReactions.end()) {
	{ 
		return;
	}
	}

	if( plr->GetTeam() == 0 )
		sEventMgr.RemoveEvents(this, EVENT_BATTLEGROUND_WSG_AUTO_RETURN_FLAG);
	else
		sEventMgr.RemoveEvents(this, EVENT_BATTLEGROUND_WSG_AUTO_RETURN_FLAG + 1);

	if( m_dropFlags[plr->GetTeam()]->IsInWorld() )
		m_dropFlags[plr->GetTeam()]->RemoveFromWorld(false);

	m_flagHolders[plr->GetTeam()] = plr->GetLowGUID();
	plr->m_bgHasFlag = true;

	/* This is *really* strange. Even though the A9 create sent to the client is exactly the same as the first one, if
	 * you spawn and despawn it, then spawn it again it will not show. So we'll assign it a new guid, hopefully that
	 * will work.
	 * - Burlex
	 */
	m_dropFlags[plr->GetTeam()]->SetNewGuid(m_mapMgr->GenerateGameobjectGuid());
	
	SpellEntry * pSp = dbcSpell.LookupEntry(23333 + (plr->GetTeam() * 2));
	Spell * sp = SpellPool.PooledNew( __FILE__, __LINE__ );
	sp->Init(plr, pSp, true, 0);
	SpellCastTargets targets(plr->GetGUID());
	sp->prepare(&targets);
	SetWorldState(plr->GetTeam() ? WSG_ALLIANCE_FLAG_CAPTURED : WSG_HORDE_FLAG_CAPTURED, 2);
	if( plr->GetTeam() == 1 )
		SendChatMessage( CHAT_MSG_BG_EVENT_HORDE, plr->GetGUID(), "The Alliance's flag has been taken by %s !", plr->GetName() );
	else
		SendChatMessage( CHAT_MSG_BG_EVENT_ALLIANCE, plr->GetGUID(), "The Horde's flag has been taken by %s !", plr->GetName() );
}

void TwinPeaks::ReturnFlag(uint32 team)
{
	if (m_dropFlags[team]->IsInWorld())
		m_dropFlags[team]->RemoveFromWorld(false);
	
	if( !m_homeFlags[team]->IsInWorld() )
		m_homeFlags[team]->PushToWorld(m_mapMgr);
	
	if( team )
		SendChatMessage( CHAT_MSG_BG_EVENT_NEUTRAL, 0, "The Alliance flag was returned to its base!" );
	else
		SendChatMessage( CHAT_MSG_BG_EVENT_NEUTRAL, 0, "The Horde flag was returned to its base!" );
}

void TwinPeaks::HookFlagStand(Player * plr, GameObject * obj)
{
#ifdef ANTI_CHEAT
	if(!m_started)
	{
		Anticheat_Log->writefromsession(plr->GetSession(), "%s tryed to hook the flag in warsong gluch before battleground (ID %u) started.", plr->GetName(), this->m_id);
		SendChatMessage(CHAT_MSG_BG_EVENT_NEUTRAL, plr->GetGUID(), "%s will be removed from the game for cheating.", plr->GetName());
		// Remove player from battleground.
		this->RemovePlayer(plr, false);
		// Kick player from server.
		plr->Kick(6000);
		return;
	}
#endif
	//both flags can be stolen anytime
	if(m_flagHolders[plr->GetTeam()] || m_homeFlags[plr->GetTeam()] != obj)
	//one flag stolen at a time
//	if(m_flagHolders[plr->GetTeam()] || m_homeFlags[plr->GetTeam()] != obj || m_flagHolders[plr->GetTeam() ? 0 : 1] != 0 || m_dropFlags[plr->GetTeam() ? 0 : 1]->IsInWorld())
	{
		// cheater!
		return;
	}

	map<uint32,uint32>::iterator itr = plr->m_forcedReactions.find(1059);
	if (itr != plr->m_forcedReactions.end()) {
	{ 
		return;
	}
	}

	SpellEntry * pSp = dbcSpell.LookupEntry(23333 + (plr->GetTeam() * 2));
	Spell * sp = SpellPool.PooledNew( __FILE__, __LINE__ );
	sp->Init(plr, pSp, true, 0);
	SpellCastTargets targets(plr->GetGUID());
	sp->prepare(&targets);

	/* set the flag holder */
	plr->m_bgHasFlag = true;
	m_flagHolders[plr->GetTeam()] = plr->GetLowGUID();
	if(m_homeFlags[plr->GetTeam()]->IsInWorld())
		m_homeFlags[plr->GetTeam()]->RemoveFromWorld(false);

	PlaySoundToAll(plr->GetTeam() ? SOUND_HORDE_CAPTURE : SOUND_ALLIANCE_CAPTURE);
	SetWorldState(plr->GetTeam() ? WSG_ALLIANCE_FLAG_CAPTURED : WSG_HORDE_FLAG_CAPTURED, 2);
}

void TwinPeaks::HookOnPlayerKill(Player * plr, Player * pVictim)
{
	plr->m_bgScore.KillingBlows++;
	UpdatePvPData();
}

void TwinPeaks::HookOnHK(Player * plr)
{
	plr->m_bgScore.HonorableKills++;
	UpdatePvPData();
}

void TwinPeaks::OnAddPlayer(Player * plr)
{
	if(!m_started)
	{
		plr->CastSpell(plr, BG_PREPARATION, true);
		plr->m_bgScore.MiscData[BG_SCORE_WSG_FLAGS_CAPTURED] = 0;
		plr->m_bgScore.MiscData[BG_SCORE_WSG_FLAGS_RETURNED] = 0;
	}
	UpdatePvPData();
}

void TwinPeaks::OnRemovePlayer(Player * plr)
{
	/* drop the flag if we have it */
	if(plr->m_bgHasFlag)
		HookOnMount(plr);

	plr->RemoveAura(BG_PREPARATION,0,AURA_SEARCH_POSITIVE);
}

LocationVector TwinPeaks::GetStartingCoords(uint32 Team)
{
	if(Team)		// Horde
	{ 
		return LocationVector(1555, 346, 3, 3.141593f);
	}
	else			// Alliance
		return LocationVector( 2133, 185 , 45, 0.0f);
}

void TwinPeaks::HookOnPlayerDeath(Player * plr)
{
	plr->m_bgScore.Deaths++;

	/* do we have the flag? */
	if(plr->m_bgHasFlag)
	    plr->RemoveAura( 23333 + (plr->GetTeam() * 2),0,AURA_SEARCH_POSITIVE );

	UpdatePvPData();
}

void TwinPeaks::HookOnMount(Player * plr)
{
	/* do we have the flag? */
	if(m_flagHolders[plr->GetTeam()] == plr->GetLowGUID())
		HookOnFlagDrop(plr);
}

bool TwinPeaks::HookHandleRepop(Player * plr)
{
    LocationVector dest;
	if(plr->GetTeam())
		dest.ChangeCoords(1814.27f, 154.594f, 1.98978f, 1.06465f);
	else
		dest.ChangeCoords(1879.16f, 441.913f, -3.81684f, 4.18879f);
	plr->SafeTeleportDelayed(plr->GetMapId(), plr->GetInstanceID(), dest);
	return true;
}

void TwinPeaks::SpawnBuff(uint32 x)
{
    switch(x)
	{
	case 0:
		m_buffs[x] = SpawnGameObject(179871, 726, 2175.87f, 226.622f, 43.7629f, 2.60053f, 0, 114, 1);
//		m_buffs[x]->SetFloatValue(GAMEOBJECT_PARENTROTATION_2,0.73135370016098f);
//		m_buffs[x]->SetFloatValue(GAMEOBJECT_PARENTROTATION_3,-0.681998312473297f);
		m_buffs[x]->SetByte(GAMEOBJECT_BYTES_1, 0, 1);
		m_buffs[x]->SetByte(GAMEOBJECT_BYTES_1, 1, 6);
		m_buffs[x]->SetByte(GAMEOBJECT_BYTES_1, 3, 100);
		break;
	case 1:
		m_buffs[x] = SpawnGameObject(179899, 726, 1544.55f, 303.852f, 0.692371f, 6.26573f, 0, 114, 1);
//		m_buffs[x]->SetFloatValue(GAMEOBJECT_PARENTROTATION_2,0.73135370016098f);
//		m_buffs[x]->SetFloatValue(GAMEOBJECT_PARENTROTATION_3,0.681998372077942f);
		m_buffs[x]->SetByte(GAMEOBJECT_BYTES_1, 0, 1);
		m_buffs[x]->SetByte(GAMEOBJECT_BYTES_1, 1, 6);
		m_buffs[x]->SetByte(GAMEOBJECT_BYTES_1, 3, 100);
		break;
	case 2:
		m_buffs[x] = SpawnGameObject(179904, 726, 1951.18f, 383.795f, -10.5257f, 4.06662f, 0, 114, 1);
//		m_buffs[x]->SetFloatValue(GAMEOBJECT_PARENTROTATION_2,0.130526319146156f);
//		m_buffs[x]->SetFloatValue(GAMEOBJECT_PARENTROTATION_3,-0.991444826126099f);
		m_buffs[x]->SetByte(GAMEOBJECT_BYTES_1, 0, 1);
		m_buffs[x]->SetByte(GAMEOBJECT_BYTES_1, 1, 6);
		m_buffs[x]->SetByte(GAMEOBJECT_BYTES_1, 3, 100);
		break;
	case 3:
		m_buffs[x] = SpawnGameObject(179906, 726, 1754.16f, 242.125f, -14.1316f, 1.15192f, 0, 114, 1);
//		m_buffs[x]->SetFloatValue(GAMEOBJECT_PARENTROTATION_2,0.333806991577148f);
//		m_buffs[x]->SetFloatValue(GAMEOBJECT_PARENTROTATION_3,-0.94264143705368f);
		m_buffs[x]->SetByte(GAMEOBJECT_BYTES_1, 0, 1);
		m_buffs[x]->SetByte(GAMEOBJECT_BYTES_1, 1, 6);
		m_buffs[x]->SetByte(GAMEOBJECT_BYTES_1, 3, 100);
		break;
	case 4:
		m_buffs[x] = SpawnGameObject(179905, 726, 1932.83f, 226.792f, -17.0598f, 2.44346f, 0, 114, 1);
//		m_buffs[x]->SetFloatValue(GAMEOBJECT_PARENTROTATION_2,0.559192895889282f);
//		m_buffs[x]->SetFloatValue(GAMEOBJECT_PARENTROTATION_3,0.829037606716156f);
		m_buffs[x]->SetByte(GAMEOBJECT_BYTES_1, 0, 1);
		m_buffs[x]->SetByte(GAMEOBJECT_BYTES_1, 1, 6);
		m_buffs[x]->SetByte(GAMEOBJECT_BYTES_1, 3, 100);
		break;
	case 5:
		m_buffs[x] = SpawnGameObject(179907, 726, 1737.57f, 435.845f, -8.08634f, 5.51524f, 0, 114, 1);
//		m_buffs[x]->SetFloatValue(GAMEOBJECT_PARENTROTATION_2,0.939692616462708f);
//		m_buffs[x]->SetFloatValue(GAMEOBJECT_PARENTROTATION_3,-0.342020124197006f);
		m_buffs[x]->SetByte(GAMEOBJECT_BYTES_1, 0, 1);
		m_buffs[x]->SetByte(GAMEOBJECT_BYTES_1, 1, 6);
		m_buffs[x]->SetByte(GAMEOBJECT_BYTES_1, 3, 100);
		break;
	}
}

void TwinPeaks::PeriodicUpdateClientRemainingTime()
{
	int64 time_remaining = DEFAULT_BG_CLOSE_TIME / 1000 / 60 - ( UNIXTIME - m_startTime ) / 60;
	if( time_remaining > 0 )
		SetWorldState( WSG_WS_TIME_REMAINING, (uint32)time_remaining );
}

void TwinPeaks::OnCreate()
{
	/* add the buffs to the world */
	for(int i = 0; i < 6; ++i)
	{
		if(!m_buffs[i]->IsInWorld())
			m_buffs[i]->PushToWorld(m_mapMgr);
	}
	GameObject *gate;
	// Alliance Gates
	gate = SpawnGameObject(206654, 726, 2156.0f, 219.206f, 43.6256f, 2.60926f, GAMEOBJECT_CLICKABLE|GAMEOBJECT_UNCLICKABLE, 1375, 1.0f);
	gate->SetFloatValue(GAMEOBJECT_PARENTROTATION_2,0.964787f);
	gate->SetFloatValue(GAMEOBJECT_PARENTROTATION_3,0.263031f);
	gate->SetUInt32Value( GAMEOBJECT_DYNAMIC, 0xFFFF0000 );
//	gate->SetUInt32Value( GAMEOBJECT_BYTES_1, 0xFF000000 ); //!! makes the GO invisible to players
	gate->PushToWorld(m_mapMgr);
	m_gates.push_back(gate);

	gate = SpawnGameObject(206655, 726, 2118.09f, 154.675f, 43.5709f, 2.60926f, GAMEOBJECT_CLICKABLE|GAMEOBJECT_UNCLICKABLE, 1375, 1.0f);
	gate->SetFloatValue(GAMEOBJECT_PARENTROTATION_2,0.964787f);
	gate->SetFloatValue(GAMEOBJECT_PARENTROTATION_3,0.263031f);
	gate->SetUInt32Value( GAMEOBJECT_DYNAMIC, 0xFFFF0000 );
	gate->PushToWorld(m_mapMgr);
	m_gates.push_back(gate);

	gate = SpawnGameObject(206653, 726, 2135.52f, 218.926f, 43.6095f, 5.75086f, GAMEOBJECT_CLICKABLE|GAMEOBJECT_UNCLICKABLE, 1375, 1.0f);
	gate->SetFloatValue(GAMEOBJECT_PARENTROTATION_2,0.964787f);
	gate->SetFloatValue(GAMEOBJECT_PARENTROTATION_3,0.263031f);
	gate->SetUInt32Value( GAMEOBJECT_DYNAMIC, 0xFFFF0000 );
	gate->PushToWorld(m_mapMgr);
	m_gates.push_back(gate);

	// Horde Gates
	gate = SpawnGameObject(208206, 726, 1574.61f, 321.242f, 1.58989f, 6.17847f, GAMEOBJECT_CLICKABLE|GAMEOBJECT_UNCLICKABLE, 1375, 1.0f);
	gate->SetFloatValue(GAMEOBJECT_PARENTROTATION_2,-0.0523358f);
	gate->SetFloatValue(GAMEOBJECT_PARENTROTATION_3,0.99863f);
	gate->SetUInt32Value( GAMEOBJECT_DYNAMIC, 0xFFFF0000 );
	gate->PushToWorld(m_mapMgr);
	m_gates.push_back(gate);

	gate = SpawnGameObject(208205, 726, 1556.66f, 314.713f, 1.589f, 6.17847f, GAMEOBJECT_CLICKABLE|GAMEOBJECT_UNCLICKABLE, 1375, 1.0f);
	gate->SetFloatValue(GAMEOBJECT_PARENTROTATION_2,-0.0523358f);
	gate->SetFloatValue(GAMEOBJECT_PARENTROTATION_3,0.99863f);
	gate->SetUInt32Value( GAMEOBJECT_DYNAMIC, 0xFFFF0000 );
	gate->PushToWorld(m_mapMgr);
	m_gates.push_back(gate);

	gate = SpawnGameObject(208207, 726, 1558.09f, 372.765f, 1.72373f, 6.17847f, GAMEOBJECT_CLICKABLE|GAMEOBJECT_UNCLICKABLE, 1375, 1.0f);   
	gate->SetFloatValue(GAMEOBJECT_PARENTROTATION_2,-0.0523358f);
	gate->SetFloatValue(GAMEOBJECT_PARENTROTATION_3,0.99863f);
	gate->SetUInt32Value( GAMEOBJECT_DYNAMIC, 0xFFFF0000 );
	gate->PushToWorld(m_mapMgr);
	m_gates.push_back(gate);

	gate = SpawnGameObject(203710, 726, 1558.62f, 379.16f, -6.40967f, 4.60767f, GAMEOBJECT_CLICKABLE|GAMEOBJECT_UNCLICKABLE, 1375, 1.0f);   
	gate->SetFloatValue(GAMEOBJECT_PARENTROTATION_2,-0.0523358f);
	gate->SetFloatValue(GAMEOBJECT_PARENTROTATION_3,0.99863f);
	gate->SetUInt32Value( GAMEOBJECT_DYNAMIC, 0xFFFF0000 );
	gate->PushToWorld(m_mapMgr);
	m_gates.push_back(gate);

	//Register the timer update function - maybe this can be used in all BGs ?
	sEventMgr.AddEvent( this, &TwinPeaks::PeriodicUpdateClientRemainingTime, EVENT_BATTLEGROUND_REMAINING_TIME, 60*1000, 0,0);

	/* set world states */
	SetWorldState(0x8D8, 0);
	SetWorldState(0x8D7, 0);
	SetWorldState(0x8D6, 0);
	SetWorldState(0x8D5, 0);
	SetWorldState(0x8D4, 0);
	SetWorldState(0x8D3, 0);
	SetWorldState(0x60B, 0);
	SetWorldState(0x60A, 0);
	SetWorldState(0x609, 0);
	SetWorldState(WSG_ALLIANCE_FLAG_CAPTURED, 1);
	SetWorldState(WSG_HORDE_FLAG_CAPTURED, 1);
	SetWorldState(WSG_MAX_SCORE, 3);
	SetWorldState(WSG_CURRENT_ALLIANCE_SCORE, 0);
	SetWorldState(WSG_CURRENT_HORDE_SCORE, 0);
	SetWorldState(WSG_WS_TIME_REMAINING, DEFAULT_BG_CLOSE_TIME / 1000 / 60 );
	//extracted from 14333 client
	SetWorldState(0x0000062D,0x00000000);
	SetWorldState(0x00001634,0x00000000);
	SetWorldState(0x0000162F,0x00000000);
	SetWorldState(0x0000162E,0x00000000);
	SetWorldState(0x0000162D,0x00000000);
	SetWorldState(0x00001300,0x00000064);
	SetWorldState(0x00000609,0x00000000);
	SetWorldState(0x000014E0,0x00000000);
	SetWorldState(0x00001584,0x00000000);
	SetWorldState(0x0000144B,0x00000000);
	SetWorldState(0x00001449,0x00000000);
	SetWorldState(0x000014F1,0x00000000);
	SetWorldState(0x000014F0,0x00000000);
	SetWorldState(0x000014D6,0x00000000);
	SetWorldState(0x000014D5,0x00000000);
	SetWorldState(0x000014D4,0x00000000);
	SetWorldState(0x00000FB8,0x00000000);
	SetWorldState(0x00000FB5,0x00000000);
	SetWorldState(0x00000795,0x00000000);
	SetWorldState(0x00001143,0x00000000);
	SetWorldState(0x00001141,0x00000001);
	SetWorldState(0x00001102,0x00000000);
	SetWorldState(0x00001117,0x00000000);
	SetWorldState(0x00001097,0x00000001);
	SetWorldState(0x00000D62,0x00000003);
	SetWorldState(0x00000CFF,0x00000000);
	SetWorldState(0x00000E7E,0x00000000);
	SetWorldState(0x00001099,0x00000000);
	SetWorldState(0x0000060B,0x00000002);
	SetWorldState(0x00000C77,0x00000004);
	SetWorldState(0x000013AD,0x00000006);
	SetWorldState(0x000012FF,0x0000012C);
	SetWorldState(0x000012FE,0x000003E8);
	SetWorldState(0x0000144C,0x00000000);
	SetWorldState(0x0000144A,0x00000000);
	SetWorldState(0x00001448,0x00000000);
	SetWorldState(0x000013FB,0x00000000);
	SetWorldState(0x000008D6,0x00000000);
	SetWorldState(0x000008D3,0x00000000);
	SetWorldState(0x00000923,0x00000001);
	SetWorldState(0x00001142,0x00000032);
	SetWorldState(0x00001023,0x0000003C);
	SetWorldState(0x000013CF,0x00000006);
	SetWorldState(0x00000B23,0x00000000);
	SetWorldState(0x00000FDE,0x00000000);
	SetWorldState(0x00000E1A,0x00000001);
	SetWorldState(0x00000E10,0x00000000);
	SetWorldState(0x0000062E,0x00000000);
	SetWorldState(0x0000060A,0x00000000);
	SetWorldState(0x00000E6F,0x00000001);
	SetWorldState(0x00000C0D,0x0000017B);
	SetWorldState(0x000010B1,0x00000000);
	SetWorldState(0x00000797,0x00000000);
	SetWorldState(0x00000796,0x00000000);
	SetWorldState(0x00000FB9,0x00000000);
	SetWorldState(0x00000FB7,0x00000000);
	SetWorldState(0x00000FB6,0x00000000);
	SetWorldState(0x00000FB4,0x00000000);
	SetWorldState(0x00000641,0x00000003);
	SetWorldState(0x00000E11,0x00000000);
	SetWorldState(0x000008D8,0x00000000);
	SetWorldState(0x000008D7,0x00000000);
	SetWorldState(0x000008D5,0x00000000);
	SetWorldState(0x00000922,0x00000001);
	SetWorldState(0x00000F3D,0x00000003);
	SetWorldState(0x00000EF2,0x00000001);
	SetWorldState(0x00000A05,0x0000008E);
	SetWorldState(0x00000EC5,0x00000000);
	SetWorldState(0x00000ED9,0x00000000);
	SetWorldState(0x000008D4,0x00000000);
	/* spawn spirit guides */
	AddSpiritGuide(SpawnSpiritGuide(1878.16f, 441.913f, -3.51684f, 4.18879f, 0));
	AddSpiritGuide(SpawnSpiritGuide(1815.27f, 154.594f, 1.98978f, 1.06465f, 1));
}

void TwinPeaks::OnStart()
{
	for(uint32 i = 0; i < 2; ++i) 
	{
		InRangePlayerSet::iterator itr,itr2;
		for(itr2 = m_players[i].begin(); itr2 != m_players[i].end();) 
		{
			itr = itr2;
			itr2++;
			(*itr)->RemoveAura(BG_PREPARATION,0,AURA_SEARCH_POSITIVE);
			Player *plr = (*itr);
			if( plr->m_bg == NULL )
			{
				m_players[i].erase( itr );
				RemovePlayer( plr, false );
				continue;
			}
			//we actually joined the BG
			BattlegroundManager.SendBattlefieldStatus(plr, BF_STATUS_PACKET_PLAYER_JOINED, plr->m_bg->GetType(), plr->m_bg->GetMapMgr()->GetInstanceID(), (uint32)UNIXTIME - m_startTime, plr->m_bg->GetMapMgr()->GetMapId(), Rated() );
		}
	}
	// update pvp data
//	UpdatePvPData();

	/* open the gates */
	for(list<GameObject*>::iterator itr = m_gates.begin(); itr != m_gates.end(); ++itr)
	{
		(*itr)->SetUInt32Value(GAMEOBJECT_FLAGS, 64);
		(*itr)->SetByte(GAMEOBJECT_BYTES_1, 0, 0);
		(*itr)->Despawn(5000, 0);
	}
	m_gates.clear();	//we did despawn them...

	/* add the flags to the world */
	for(int i = 0; i < 2; ++i)
	{
		if( !m_homeFlags[i]->IsInWorld() )
			m_homeFlags[i]->PushToWorld(m_mapMgr);
	}

	SendChatMessage(CHAT_MSG_BG_EVENT_NEUTRAL, 0, "The Alliance's flag is now placed at her base.");
	SendChatMessage(CHAT_MSG_BG_EVENT_NEUTRAL, 0, "The Horde's flag is now placed at her base.");

	/* correct? - burlex */
	PlaySoundToAll(SOUND_BATTLEGROUND_BEGIN);

	m_started = true;
}

void TwinPeaks::HookOnShadowSight() 
{
}
void TwinPeaks::HookGenerateLoot(Player *plr, Object * pOCorpse)
{
}

void TwinPeaks::HookOnUnitKill(Player * plr, Unit * pVictim)
{
}

void TwinPeaks::SetIsWeekend(bool isweekend) 
{
	m_isWeekend = isweekend;
}

