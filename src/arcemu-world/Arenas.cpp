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

#define ARENA_PREPARATION 32727
#define GREEN_TEAM 0
#define GOLD_TEAM 1
#define ARENA_WORLD_STATE_LORD_A_PLAYER_COUNT 3600
#define ARENA_WORLD_STATE_LORD_H_PLAYER_COUNT 3601
#define ARENA_WORLD_STATE_BLADES_A_PLAYER_COUNT 3600
#define ARENA_WORLD_STATE_BLADES_H_PLAYER_COUNT 3601
#define ARENA_WORLD_STATE_NAGRAND_A_PLAYER_COUNT 3600
#define ARENA_WORLD_STATE_NAGRAND_H_PLAYER_COUNT 3601

Arena::Arena(MapMgr * mgr, uint32 id, uint32 lgroup, uint32 t, uint32 players_per_side) : CBattleground(mgr, id, lgroup, t)
{
	int i;

	for (i=0; i<2; i++) {
		m_players[i].clear();
		m_pendPlayers[i].clear();
	}
	//m_worldStates.clear();
	m_pvpData.clear();
	m_resurrectMap.clear();

	m_started = false;
	m_playerCountPerTeam = players_per_side;
	m_buffs[0] = m_buffs[1] = NULL;
	m_playersCount[0] = m_playersCount[1] = 0;
	m_teams[0] = m_teams[1] = NULL;
	switch(t)
	{
	case BATTLEGROUND_ARENA_5V5:
		m_arenateamtype=2;
		break;

	case BATTLEGROUND_ARENA_3V3:
		m_arenateamtype=1;
		break;

	case BATTLEGROUND_ARENA_2V2:
		m_arenateamtype=0;
		break;

	default:
		m_arenateamtype=0;
		break;
	}
	rated_match=false;
	m_winningteam = -1;	//none
	m_EnoughTeamsJoined = false;
}

Arena::~Arena()
{
	int i;

	for(i = 0; i < 2; ++i)
	{
		// buffs may not be spawned, so delete them if they're not
		if(m_buffs[i] && m_buffs[i]->IsInWorld() == false)
		{
			sGarbageCollection.AddObject( m_buffs[i] );
			m_buffs[i] = NULL;
		}
	}

	for(set<GameObject*>::iterator itr = m_gates.begin(); itr != m_gates.end(); ++itr)
	{
		if((*itr) != NULL)
		{
			(*itr)->m_battleground = NULL;
			if( !(*itr)->IsInWorld() )
			{
				sGarbageCollection.AddObject( (*itr) );
				(*itr) = NULL;
			}
		}
	}
	m_gates.clear();

}

void Arena::OnAddPlayer(Player * plr)
{
	if (plr == NULL)
		return;

	//do not add GMs as part of some team. We are porting into the match due to a script
	if( plr->m_bgIsQueued == false )
		return;

	plr->SetDeathVision();

	// remove all buffs (exclude talents, include flasks)
	for(uint32 x=0;x<MAX_NEGATIVE_AURAS1(plr);x++)
	{
		Aura *a = plr->m_auras[x];
		if( a == NULL )
			continue;
		if( a->GetTimeLeft() == (uint32)-1 )
			continue;
		if( a->GetSpellProto()->AttributesExC & CAN_PERSIST_AND_CASTED_WHILE_DEAD )
			continue;
		if( a->IsPassive() )
			continue;
		a->Remove();
	}

	// On arena start all conjured items are removed
	plr->GetItemInterface()->RemoveAllConjured();

	// Before the arena starts all your cooldowns are reset
	if( !m_started  && plr->IsInWorld())
		plr->ResetAllCooldowns();

	// if( plr->m_isGmInvisible == false )
	// Make sure the player isn't a GM an isn't invisible (monitoring?)
	if ( !plr->IsGMInvisible() && !plr->IsSpectator() )
	{
		if( !m_started  && plr->IsInWorld())
			plr->CastSpell(plr, ARENA_PREPARATION, true);

		m_playersCount[plr->GetTeam()]++;
		UpdatePlayerCounts();

		/* Set FFA PvP Flag */
		plr->SetFFAPvPFlag();

		m_playersAlive.insert(plr->GetLowGUID());

	}
	// If they're still queued for the arena, remove them from the queue
	if (plr->m_bgIsQueued)
		plr->m_bgIsQueued = false;

	/* Add the green/gold team flag */
//	Aura * aura = AuraPool.PooledNew( __FILE__, __LINE__ );
//	aura->Init(dbcSpell.LookupEntry((plr->GetTeamInitial()) ? 35775-plr->m_bgTeam : 32725-plr->m_bgTeam), -1, plr, plr);
//	plr->AddAura(aura);
	if( plr->GetTeamInitial() )
	{
		if( plr->GetTeam() )
			plr->CastSpell( plr, SPELL_HORDE_GREEN_FLAG, true );
		else
			plr->CastSpell( plr, SPELL_HORDE_GOLD_FLAG, true );
	}
	else
	{
		if( plr->GetTeam() )
			plr->CastSpell( plr, SPELL_ALLIANCE_GREEN_FLAG, true );
		else
			plr->CastSpell( plr, SPELL_ALLIANCE_GOLD_FLAG, true );
	}

//{SERVER} Packet: (0xA66F) SMSG_ARENA_JOINED PacketSize = 13 TimeStamp = 4244522
//00 01 00 00 00 E9 D7 38 03 00 00 00 01 
	WorldPacket data( SMSG_ARENA_JOINED, 13 + 10 );
	data << uint8( 0 );
	data << uint32( 1 );
	data << uint64( plr->GetGUID() );
	DistributePacketToAll( &data );
//			SendPacketToAllButOne( &data, info->m_loggedInPlayer );
//	if( plr->GetGroup() )
//		plr->GetGroup()->Update();
	BattlegroundManager.SendBattlefieldStatus( plr, BF_STATUS_PACKET_PLAYER_JOINED, m_type, m_id,(uint32)UNIXTIME - m_startTime, m_mapMgr->GetMapId(), Rated() );   // Elapsed time is the last argument
}

void Arena::OnRemovePlayer(Player * plr)
{
	/* remove arena readyness buff */
	plr->RemDeathVision();

	//add deserter buff so they do not spam join
	if( m_ended == false && plr->isAlive() && ( plr->GetSession() == NULL || plr->GetSession()->HasPermissions() == false ) && m_SkipRewards == false )
		objmgr.PlayerAddDeserterCooldown( plr->GetGUID() );

	// All auras are removed on exit
	// plr->RemoveAura(ARENA_PREPARATION);
//	plr->RemoveAllAuras();	
///	plr->RemovePositiveAuras();	
//	plr->RemoveNegativeAuras();	
	plr->RemoveBGAuras();

	// Player has left arena, call HookOnPlayerDeath as if he died
	HookOnPlayerDeath(plr);

//	plr->RemoveAura(plr->GetTeamInitial() ? 35775-plr->m_bgTeam : 32725-plr->m_bgTeam,0,AURA_SEARCH_POSITIVE);
	plr->RemoveFFAPvPFlag();

	plr->m_bg = NULL;

	// Reset all their cooldowns and restore their HP/Mana/Energy to max
	if( plr->GetSession() )
		plr->ResetAllCooldowns();
	plr->FullHPMP();
}

void Arena::HookOnPlayerKill(Player * plr, Player * pVictim)
{
#ifdef ANTI_CHEAT
	if (!m_started)
	{
		plr->KillPlayer(); //cheater.
		return;
	}
#endif
	if ( pVictim->IsPlayer() )
	{
		plr->m_bgScore.KillingBlows++;
	}
}

void Arena::HookOnHK(Player * plr)
{
	plr->m_bgScore.HonorableKills++;
}

void Arena::HookOnPlayerDeath(Player * plr)
{
	ASSERT(plr != NULL);

	if( plr->IsGMInvisible() == true ) 
		return;

	if(m_playersAlive.find(plr->GetLowGUID()) != m_playersAlive.end())
	{
		m_playersCount[plr->GetTeam()]--;
		m_playersAlive.erase(plr->GetLowGUID());
		UpdatePlayerCounts();
//		m_playersAlive.erase(plr->GetLowGUID());
	}
}

void Arena::OnCreate()
{
	GameObject * obj;
	switch(m_mapMgr->GetMapId())
	{
		/* ruins of lordaeron */
	case 572: {
		obj = SpawnGameObject(185917, 572, 1278.647705f, 1730.556641f, 31.605574f, 1.684245f, 32, 1375, 1.0f);
		obj->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, GO_STATE_READY);
		obj->SetFloatValue(GAMEOBJECT_PARENTROTATION_2, 0.746058f);
		obj->SetFloatValue(GAMEOBJECT_PARENTROTATION_3, 0.665881f);
		m_gates.insert(obj);

		obj = SpawnGameObject(185918, 572, 1293.560791f, 1601.937988f, 31.605574f, -1.457349f, 32, 1375, 1.0f);
		obj->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, GO_STATE_READY);
		obj->SetFloatValue(GAMEOBJECT_PARENTROTATION_2, -0.665881f);
		obj->SetFloatValue(GAMEOBJECT_PARENTROTATION_3, 0.746058f);
		m_gates.insert(obj);

			  }break;

		/* blades edge arena */
	case 562: {
		obj = SpawnGameObject(183972, 562, 6177.707520f, 227.348145f, 3.604374f, -2.260201f, 32, 1375, 1.0f);
		obj->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, GO_STATE_READY);
		obj->SetFloatValue(GAMEOBJECT_PARENTROTATION_2, 0.90445f);
		obj->SetFloatValue(GAMEOBJECT_PARENTROTATION_3, -0.426569f);
		obj->PushToWorld(m_mapMgr);

		obj = SpawnGameObject(183973, 562, 6189.546387f, 241.709854f, 3.101481f, 0.881392f, 32, 1375, 1.0f);
		obj->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, GO_STATE_READY);
		obj->SetFloatValue(GAMEOBJECT_PARENTROTATION_2, 0.426569f);
		obj->SetFloatValue(GAMEOBJECT_PARENTROTATION_3, 0.904455f);
		m_gates.insert(obj);

		obj = SpawnGameObject(183970, 562, 6299.115723f, 296.549438f, 3.308032f, 0.881392f, 32, 1375, 1.0f);
		obj->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, GO_STATE_READY);
		obj->SetFloatValue(GAMEOBJECT_PARENTROTATION_2, 0.426569f);
		obj->SetFloatValue(GAMEOBJECT_PARENTROTATION_3, 0.904455f);
		obj->PushToWorld(m_mapMgr);

		obj = SpawnGameObject(183971, 562, 6287.276855f, 282.187714f, 3.810925f, -2.260201f, 32, 1375, 1.0f);
		obj->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, GO_STATE_READY);
		obj->SetFloatValue(GAMEOBJECT_PARENTROTATION_2, 0.904455f);
		obj->SetFloatValue(GAMEOBJECT_PARENTROTATION_3, -0.426569f);
		m_gates.insert(obj);
			  }break;

		/* nagrand arena */
	case 559: 
		{
			obj = SpawnGameObject(183979, 559, 4090.064453f, 2858.437744f, 10.236313f, 0.492805f, 32, 1375, 1.0f);
			obj->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, GO_STATE_READY);
			obj->SetFloatValue(GAMEOBJECT_PARENTROTATION_2, 0.243916f);
			obj->SetFloatValue(GAMEOBJECT_PARENTROTATION_3, 0.969796f);
			obj->PushToWorld(m_mapMgr);

			obj = SpawnGameObject(183980, 559, 4081.178955f, 2874.970459f, 12.391714f, 0.492805f, 32, 1375, 1.0f);
			obj->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, GO_STATE_READY);
			obj->SetFloatValue(GAMEOBJECT_PARENTROTATION_2, 0.243916f);
			obj->SetFloatValue(GAMEOBJECT_PARENTROTATION_3, 0.969796f);
			m_gates.insert(obj);

			obj = SpawnGameObject(183977, 559, 4023.709473f, 2981.776611f, 10.701169f, -2.648788f, 32, 1375, 1.0f);
			obj->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, GO_STATE_READY);
			obj->SetFloatValue(GAMEOBJECT_PARENTROTATION_2, 0.969796f);
			obj->SetFloatValue(GAMEOBJECT_PARENTROTATION_3, -0.243916f);
			obj->PushToWorld(m_mapMgr);

			obj = SpawnGameObject(183978, 559, 4031.854248f, 2966.833496f, 12.646200f, -2.648788f, 32, 1375, 1.0f);
			obj->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, GO_STATE_READY);
			obj->SetFloatValue(GAMEOBJECT_PARENTROTATION_2, 0.969796f);
			obj->SetFloatValue(GAMEOBJECT_PARENTROTATION_3, -0.243916f);
			m_gates.insert(obj);
	    }break;
	//Dalaran Sewers
	case 617: 
		{
			obj = SpawnGameObject(183979, 617, 1350.95f, 817.2f, 14.8096f, 3.14f/2, 32, 1375, 1.0f);
			obj->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, GO_STATE_READY);
			obj->SetFloatValue(GAMEOBJECT_PARENTROTATION_2, 0.99627f);
			obj->SetFloatValue(GAMEOBJECT_PARENTROTATION_3, 0.0862864f);
			m_gates.insert(obj);

			obj = SpawnGameObject(183980, 617, 1232.65f, 764.913f, 14.0729f, 3.14f/2, 32, 1375, 1.0f);
			obj->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, GO_STATE_READY);
			obj->SetFloatValue(GAMEOBJECT_PARENTROTATION_2, 0.0310211f);
			obj->SetFloatValue(GAMEOBJECT_PARENTROTATION_3, -0.999519f);
			m_gates.insert(obj);
	    }break;
	//Ring of Valor
	case 618: 
		{
			//collision
			obj = SpawnGameObject(192393, 618, 763.664551f, -261.872986f, 26.686588f, 0.000000f, 32, 1375, 1.0f);
			obj->PushToWorld(m_mapMgr);
			obj = SpawnGameObject(192394, 618, 763.578979f, -306.146149f, 26.665222f, 3.141593f, 32, 1375, 1.0f);
			obj->PushToWorld(m_mapMgr);
			obj = SpawnGameObject(192389, 618, 700.722290f, -283.990662f, 39.517582f, 3.141593f, 32, 1375, 1.0f);
			obj->PushToWorld(m_mapMgr);
			obj = SpawnGameObject(192390, 618, 826.303833f, -283.996429f, 39.517582f, 0.000000f, 32, 1375, 1.0f);
			obj->PushToWorld(m_mapMgr);

			obj = SpawnGameObject(194583, 618, 763.632385f, -306.162384f, 25.909504f, 3.141593f, 32, 1375, 1.0f);
			obj->PushToWorld(m_mapMgr);
			obj = SpawnGameObject(194584, 618, 723.644287f, -284.493256f, 24.648525f, 3.141593f, 32, 1375, 1.0f);
			obj->PushToWorld(m_mapMgr);
			obj = SpawnGameObject(194585, 618, 763.611145f, -261.856750f, 25.909504f, 0.000000f, 32, 1375, 1.0f);
			obj->PushToWorld(m_mapMgr);
			obj = SpawnGameObject(194587, 618, 802.211609f, -284.493256f, 24.648525f, 0.000000f, 32, 1375, 1.0f);
			obj->PushToWorld(m_mapMgr);

			obj = SpawnGameObject(194580, 618, 763.632385f, -306.162384f, 30.639660f, 3.141593f, 32, 1375, 1.0f);
			obj->PushToWorld(m_mapMgr);
			obj = SpawnGameObject(194579, 618, 723.644287f, -284.493256f, 32.382710f, 0.000000f, 32, 1375, 1.0f);
			obj->PushToWorld(m_mapMgr);
			obj = SpawnGameObject(194581, 618, 763.611145f, -261.856750f, 30.639660f, 0.000000f, 32, 1375, 1.0f);
			obj->PushToWorld(m_mapMgr);
			obj = SpawnGameObject(194578, 618, 802.211609f, -284.493256f, 32.382710f, 3.141593f, 32, 1375, 1.0f);
			obj->PushToWorld(m_mapMgr);

			obj = SpawnGameObject(192704, 618, 743.543457f, -283.799469f, 28.286655f, 3.131593f, 32, 1375, 1.0f);
			obj->SetUInt32Value(GAMEOBJECT_FLAGS, 64);
			obj->SetByte(GAMEOBJECT_BYTES_1, 0, 0);
			obj->PushToWorld(m_mapMgr);
			obj = SpawnGameObject(192705, 618, 782.971802f, -283.799469f, 28.286655f, 3.131593f, 32, 1375, 1.0f);
			obj->SetUInt32Value(GAMEOBJECT_FLAGS, 64);
			obj->SetByte(GAMEOBJECT_BYTES_1, 0, 0);
			obj->PushToWorld(m_mapMgr);
			obj = SpawnGameObject(192387, 618, 743.711060f, -284.099609f, 27.542587f, 3.141593f, 32, 1375, 1.0f);
			obj->SetUInt32Value(GAMEOBJECT_FLAGS, 64);
			obj->SetByte(GAMEOBJECT_BYTES_1, 0, 0);
			obj->PushToWorld(m_mapMgr);
			obj = SpawnGameObject(192388, 618, 783.221252f, -284.133362f, 27.535686f, 0.000000f, 32, 1375, 1.0f);
			obj->SetUInt32Value(GAMEOBJECT_FLAGS, 64);
			obj->SetByte(GAMEOBJECT_BYTES_1, 0, 0);
			obj->PushToWorld(m_mapMgr);

			//elevators
			obj = SpawnGameObject(194582, 618, 763.536377f, -294.535767f, 0.505383f, 3.141593f, 32, 1375, 1.0f);
			obj->PushToWorld(m_mapMgr);
			obj = SpawnGameObject(194586, 618, 763.506348f, -273.873352f, 0.505383f, 0.000000f, 32, 1375, 1.0f);
			obj->PushToWorld(m_mapMgr);

			//gates ! these are non blizzlike
			obj = SpawnGameObject(192390, 618, 708.6f, -269.34f, 28.0f, 5.749f, 32, 1375, 1.0f);
			obj->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, GO_STATE_READY);
			m_gates.insert(obj);
			obj = SpawnGameObject(192390, 618, 818.81f, -298.113f, 28.0f, 2.70f, 32, 1375, 1.0f);
			obj->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, GO_STATE_READY);
			m_gates.insert(obj);
	    }break;
	//Tol'vir
	case 980: 
		{
			obj = SpawnGameObject(183979, 980, -10660.0f, 428.0f, 24.4f, 3.11f+3.14f/2, 32, 1375, 1.0f);
			obj->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, GO_STATE_READY);
			m_gates.insert(obj);

			obj = SpawnGameObject(183980, 980, -10768.0f, 431.44f, 24.30f, 3.14f/2, 32, 1375, 1.0f);
			obj->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, GO_STATE_READY);
			m_gates.insert(obj);
	    }break;

	}

	/* push gates into world */
	for(set<GameObject*>::iterator itr = m_gates.begin(); itr != m_gates.end(); ++itr)
		(*itr)->PushToWorld(m_mapMgr);

	SetWorldState(0x08D4	,0x0000);
	SetWorldState(0x08D8	,0x0000);
	SetWorldState(0x08D7	,0x0000);
	SetWorldState(0x08D6	,0x0000);
	SetWorldState(0x08D5	,0x0000);
	SetWorldState(0x08D3	,0x0000);
	SetWorldState(0x0C0D	,0x017B);


	// Show players count
	switch(m_mapMgr->GetMapId())
	{
		/* ruins of lordaeron */
	case 572:
		SetWorldState(3002, 1);
		break;
		/* blades edge arena */
	case 562:
		SetWorldState(2547, 1);
		break;
		/* nagrand arena */
	case 559:
		SetWorldState(2577, 1);
		//Dalaran Sewers
	case 617:
		SetWorldState(3610, 1);
		break;
		//Ring of Valor
	case 618:
		SetWorldState(3610, 1);
		break;
		//Tol'vir
	case 980:
		SetWorldState(3610, 1);
		break;
	}
}

void Arena::HookOnShadowSight()
{
	switch(m_mapMgr->GetMapId())
	{
		/* ruins of lordaeron */
	case 572:
		m_buffs[0] = SpawnGameObject(184664, 572, 1328.729268f, 1632.738403f, 34.838585f, 2.611449f, 32, 1375, 1.0f);
		m_buffs[0]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, GO_STATE_READY);
		m_buffs[0]->SetFloatValue(GAMEOBJECT_PARENTROTATION_2, 0.904455f);
		m_buffs[0]->SetFloatValue(GAMEOBJECT_PARENTROTATION_3, -0.426569f);
		m_buffs[0]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_TYPEID, GAMEOBJECT_TYPE_TRAP);
		m_buffs[0]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_ANIMPROGRESS, 100);
		m_buffs[0]->PushToWorld(m_mapMgr);

		m_buffs[1] = SpawnGameObject(184664, 572, 1243.306763f, 1699.334351f, 34.837566f, 5.713773f, 32, 1375, 1.0f);
		m_buffs[1]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, GO_STATE_READY);
		m_buffs[1]->SetFloatValue(GAMEOBJECT_PARENTROTATION_2, 0.90445f);
		m_buffs[1]->SetFloatValue(GAMEOBJECT_PARENTROTATION_3, -0.426569f);
		m_buffs[1]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_TYPEID, GAMEOBJECT_TYPE_TRAP);
		m_buffs[1]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_ANIMPROGRESS, 100);
		m_buffs[1]->PushToWorld(m_mapMgr);

		m_buffs[0]->ConvertToCaster( 34709, SHADOW_SIGHT_TRIGGER_RADIUS, 10, 1, BUFF_RESPAWN_TIME );	//cast shadow sight to first player that reaches us
		m_buffs[1]->ConvertToCaster( 34709, SHADOW_SIGHT_TRIGGER_RADIUS, 10, 1, BUFF_RESPAWN_TIME );	//cast shadow sight to first player that reaches us
		break;

		/* blades edge arena */
	case 562:
		m_buffs[0] = SpawnGameObject(184664, 562, 6249.276855f, 275.187714f, 11.201481f, -2.260201f, 32, 1375, 1.0f);
		m_buffs[0]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, GO_STATE_READY);
		m_buffs[0]->SetFloatValue(GAMEOBJECT_PARENTROTATION_2, 0.904455f);
		m_buffs[0]->SetFloatValue(GAMEOBJECT_PARENTROTATION_3, -0.426569f);
		m_buffs[0]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_TYPEID, GAMEOBJECT_TYPE_TRAP);
		m_buffs[0]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_ANIMPROGRESS, 100);
		m_buffs[0]->PushToWorld(m_mapMgr);

		m_buffs[1] = SpawnGameObject(184664, 562, 6228.546387f, 249.709854f, 11.201481f, 0.881392f, 32, 1375, 1.0f);
		m_buffs[1]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, GO_STATE_READY);
		m_buffs[1]->SetFloatValue(GAMEOBJECT_PARENTROTATION_2, 0.90445f);
		m_buffs[1]->SetFloatValue(GAMEOBJECT_PARENTROTATION_3, -0.426569f);
		m_buffs[1]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_TYPEID, GAMEOBJECT_TYPE_TRAP);
		m_buffs[1]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_ANIMPROGRESS, 100);
		m_buffs[1]->PushToWorld(m_mapMgr);

		m_buffs[0]->ConvertToCaster( 34709, SHADOW_SIGHT_TRIGGER_RADIUS, 10, 1, BUFF_RESPAWN_TIME );	//cast shadow sight to first player that reaches us
		m_buffs[1]->ConvertToCaster( 34709, SHADOW_SIGHT_TRIGGER_RADIUS, 10, 1, BUFF_RESPAWN_TIME );	//cast shadow sight to first player that reaches us
		break;

		/* nagrand arena */
	case 559:
		m_buffs[0] = SpawnGameObject(184664, 559, 4011.113232f, 2896.879980f, 12.523950f, 0.486944f, 32, 1375, 1.0f);
		m_buffs[0]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, GO_STATE_READY);
		m_buffs[0]->SetFloatValue(GAMEOBJECT_PARENTROTATION_2, 0.904455f);
		m_buffs[0]->SetFloatValue(GAMEOBJECT_PARENTROTATION_3, -0.426569f);
		m_buffs[0]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_TYPEID, GAMEOBJECT_TYPE_TRAP);
		m_buffs[0]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_ANIMPROGRESS, 100);
		m_buffs[0]->PushToWorld(m_mapMgr);

		m_buffs[1] = SpawnGameObject(184664, 559, 4102.111426f, 2945.843262f, 12.662578f, 3.628544f, 32, 1375, 1.0f);
		m_buffs[1]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, GO_STATE_READY);
		m_buffs[1]->SetFloatValue(GAMEOBJECT_PARENTROTATION_2, 0.90445f);
		m_buffs[1]->SetFloatValue(GAMEOBJECT_PARENTROTATION_3, -0.426569f);
		m_buffs[1]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_TYPEID, GAMEOBJECT_TYPE_TRAP);
		m_buffs[1]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_ANIMPROGRESS, 100);
		m_buffs[1]->PushToWorld(m_mapMgr);

		m_buffs[0]->ConvertToCaster( 34709, SHADOW_SIGHT_TRIGGER_RADIUS, 10, 1, BUFF_RESPAWN_TIME );	//cast shadow sight to first player that reaches us
		m_buffs[1]->ConvertToCaster( 34709, SHADOW_SIGHT_TRIGGER_RADIUS, 10, 1, BUFF_RESPAWN_TIME );	//cast shadow sight to first player that reaches us
		break;
		// Dalaran Sewers
	case 617:
		m_buffs[0] = SpawnGameObject(184663, 617, 1291.7f, 813.424f, 7.11472f, 4.64562f, 32, 1375, 1.0f);
		m_buffs[0]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, GO_STATE_READY);
		m_buffs[0]->SetFloatValue(GAMEOBJECT_PARENTROTATION_2, 0.730314f);
		m_buffs[0]->SetFloatValue(GAMEOBJECT_PARENTROTATION_3, -0.683111f);
		m_buffs[0]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_TYPEID, GAMEOBJECT_TYPE_TRAP);
		m_buffs[0]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_ANIMPROGRESS, 100);
		m_buffs[0]->PushToWorld(m_mapMgr);

		m_buffs[1] = SpawnGameObject(184664, 617, 1291.7f, 768.911f, 7.11472f, 1.55194f, 32, 1375, 1.0f);
		m_buffs[1]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, GO_STATE_READY);
		m_buffs[1]->SetFloatValue(GAMEOBJECT_PARENTROTATION_2, 0.700409f);
		m_buffs[1]->SetFloatValue(GAMEOBJECT_PARENTROTATION_3, 0.713742f);
		m_buffs[1]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_TYPEID, GAMEOBJECT_TYPE_TRAP);
		m_buffs[1]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_ANIMPROGRESS, 100);
		m_buffs[1]->PushToWorld(m_mapMgr);

		m_buffs[0]->ConvertToCaster( 34709, SHADOW_SIGHT_TRIGGER_RADIUS, 10, 1, BUFF_RESPAWN_TIME );	//cast shadow sight to first player that reaches us
		m_buffs[1]->ConvertToCaster( 34709, SHADOW_SIGHT_TRIGGER_RADIUS, 10, 1, BUFF_RESPAWN_TIME );	//cast shadow sight to first player that reaches us
		break;
		// Ring of Valor
	case 618:
		m_buffs[0] = SpawnGameObject(184663, 618, 735.551819f, -284.794678f, 28.276682f, 0.034906f, 32, 1375, 1.0f);
		m_buffs[0]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, GO_STATE_READY);
		m_buffs[0]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_TYPEID, GAMEOBJECT_TYPE_TRAP);
		m_buffs[0]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_ANIMPROGRESS, 100);
		m_buffs[0]->PushToWorld(m_mapMgr);

		m_buffs[1] = SpawnGameObject(184664, 618, 791.224487f, -284.794464f, 28.276682f, 2.600535f, 32, 1375, 1.0f);
		m_buffs[1]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, GO_STATE_READY);
		m_buffs[1]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_TYPEID, GAMEOBJECT_TYPE_TRAP);
		m_buffs[1]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_ANIMPROGRESS, 100);
		m_buffs[1]->PushToWorld(m_mapMgr);

		m_buffs[0]->ConvertToCaster( 34709, SHADOW_SIGHT_TRIGGER_RADIUS, 10, 1, BUFF_RESPAWN_TIME );	//cast shadow sight to first player that reaches us
		m_buffs[1]->ConvertToCaster( 34709, SHADOW_SIGHT_TRIGGER_RADIUS, 10, 1, BUFF_RESPAWN_TIME );	//cast shadow sight to first player that reaches us
		break;
		// Tol'vir
	case 980:
		m_buffs[0] = SpawnGameObject(184663, 980, -10737.0f, 429.0f, 24.4f, 0.034906f, 32, 1375, 1.0f);
		m_buffs[0]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, GO_STATE_READY);
		m_buffs[0]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_TYPEID, GAMEOBJECT_TYPE_TRAP);
		m_buffs[0]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_ANIMPROGRESS, 100);
		m_buffs[0]->PushToWorld(m_mapMgr);

		m_buffs[1] = SpawnGameObject(184664, 980, -10695.0f, 429.0f, 24.4f, 2.600535f, 32, 1375, 1.0f);
		m_buffs[1]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_STATE, GO_STATE_READY);
		m_buffs[1]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_TYPEID, GAMEOBJECT_TYPE_TRAP);
		m_buffs[1]->SetByte(GAMEOBJECT_BYTES_1, GAMEOBJECT_BYTES_ANIMPROGRESS, 100);
		m_buffs[1]->PushToWorld(m_mapMgr);

		m_buffs[0]->ConvertToCaster( 34709, SHADOW_SIGHT_TRIGGER_RADIUS, 10, 1, BUFF_RESPAWN_TIME );	//cast shadow sight to first player that reaches us
		m_buffs[1]->ConvertToCaster( 34709, SHADOW_SIGHT_TRIGGER_RADIUS, 10, 1, BUFF_RESPAWN_TIME );	//cast shadow sight to first player that reaches us
		break;
	}
}

void Arena::OnStart()
{
	int i;
	/* remove arena readyness buff */
	for(i = 0; i < 2; ++i)
	{
		for(InRangePlayerSet::iterator itr = m_players[i].begin(); itr != m_players[i].end(); ++itr) 
		{
			Player *plr = *itr;
			if( plr->IsSpectator() )
				continue;
			//this will show minimap icon for the ongoing fight
			BattlegroundManager.SendBattlefieldStatus( plr, BF_STATUS_PACKET_PLAYER_JOINED, m_type, m_id,(uint32)UNIXTIME - m_startTime, m_mapMgr->GetMapId(), Rated() );   // Elapsed time is the last argument

			plr->RemoveAura(ARENA_PREPARATION,0,AURA_SEARCH_POSITIVE);
			plr->SetFFAPvPFlag();
			m_players2[i].insert(plr->GetLowGUID());

			// update arena team stats 
			if(rated_match && plr->m_arenaTeams[m_arenateamtype] != NULL)
			{
				m_teams[i] = plr->m_arenaTeams[m_arenateamtype];
				m_teamsID[i] = plr->m_arenaTeams[m_arenateamtype]->m_id;
			}
		}
	}

	m_EnoughTeamsJoined = true;
	for (i=0; i<2; i++)
	{
		if (m_teams[i] == NULL) 
		{
			sLog.outDebug("Starting arena match but team %u is NULL",i);
			m_EnoughTeamsJoined = false;
			continue;
		}
	}
	if( m_EnoughTeamsJoined )
	{
		for (i=0; i<2; i++)
			if (m_teams[i] != NULL) 
			{
				m_teams[i]->m_stat_gamesplayedseason++;
				m_teams[i]->m_stat_gamesplayedweek++;
				m_teams[i]->SaveToDB();
			}
		for(i = 0; i < 2; ++i)
		{
			for(InRangePlayerSet::iterator itr = m_players[i].begin(); itr != m_players[i].end(); ++itr) 
			{
				Player *plr = *itr;
				// update arena team stats 
				if(rated_match && plr->m_arenaTeams[m_arenateamtype] != NULL)
				{
					ArenaTeamMember * tp = m_teams[i]->GetMember(plr->getPlayerInfo());
					if(tp != NULL)
					{
						tp->Played_ThisWeek++;
						tp->Played_ThisSeason++;
					}
				}
			}
		}
	}

	/* open gates */
	for(set<GameObject*>::iterator itr = m_gates.begin(); itr != m_gates.end(); ++itr)
	{
		(*itr)->SetUInt32Value(GAMEOBJECT_FLAGS, 64);
		(*itr)->SetByte(GAMEOBJECT_BYTES_1, 0, 0);
	}

	m_started = true;

	/* Incase all players left */
	UpdatePlayerCounts();

	// WHEEEE
	PlaySoundToAll(SOUND_BATTLEGROUND_BEGIN);

	sEventMgr.RemoveEvents(this, EVENT_ARENA_SHADOW_SIGHT);
	sEventMgr.AddEvent(static_cast<CBattleground*>(this), &CBattleground::HookOnShadowSight, EVENT_ARENA_SHADOW_SIGHT, BUFF_RESPAWN_TIME, 1,0);

	//is this only added in 4.3 ? Once arena starts we can see other players
//	UpdatePvPData();

	//remove auras from players that have less then 30 second duration
	set<uint32>::iterator pitr;
	for( pitr = m_playersAlive.begin(); pitr != m_playersAlive.end(); pitr++ )
	{
		Player *p = GetMapMgr()->GetPlayer( *pitr );
		if( p )
		{
			for( uint32 i=POS_AURAS_START;i<MAX_PASSIVE_AURAS1(p);i++)
				if( p->m_auras[i] && p->m_auras[i]->GetTimeLeft() <= 30000 && p->m_auras[i]->GetDuration() > 0 )
					p->m_auras[i]->Remove();
			if( p->GetPowerType() == POWER_TYPE_RAGE )
				p->SetPower( POWER_TYPE_RAGE, 0 );
			else if( p->GetPowerType() == POWER_TYPE_RUNIC )
				p->SetPower( POWER_TYPE_RUNIC, 0 );
			if( p->GetSummon() )
				p->GetSummon()->SetPower( POWER_TYPE_MANA, p->GetSummon()->GetMaxPower( POWER_TYPE_MANA ) );
		}
	}
}

void Arena::UpdatePlayerCounts()
{
	if(m_ended)
		return;

	switch(m_mapMgr->GetMapId())
	{
		/* ruins of lordaeron */
	case 572:
		SetWorldState(ARENA_WORLD_STATE_LORD_A_PLAYER_COUNT, m_playersCount[0]);
		SetWorldState(ARENA_WORLD_STATE_LORD_H_PLAYER_COUNT, m_playersCount[1]);
		break;

		/* blades edge arena */
	case 562:
		SetWorldState(ARENA_WORLD_STATE_BLADES_A_PLAYER_COUNT, m_playersCount[0]);
		SetWorldState(ARENA_WORLD_STATE_BLADES_H_PLAYER_COUNT, m_playersCount[1]);
		break;

		/* nagrand arena */
	case 559:
		SetWorldState(ARENA_WORLD_STATE_NAGRAND_A_PLAYER_COUNT, m_playersCount[0]);
		SetWorldState(ARENA_WORLD_STATE_NAGRAND_H_PLAYER_COUNT, m_playersCount[1]);
		break;

		//Dalaran Sewers
	case 617:
		SetWorldState(ARENA_WORLD_STATE_NAGRAND_A_PLAYER_COUNT, m_playersCount[0]);
		SetWorldState(ARENA_WORLD_STATE_NAGRAND_H_PLAYER_COUNT, m_playersCount[1]);
		break;

		//Ring of Valor
	case 618:
		SetWorldState(ARENA_WORLD_STATE_NAGRAND_A_PLAYER_COUNT, m_playersCount[0]);
		SetWorldState(ARENA_WORLD_STATE_NAGRAND_H_PLAYER_COUNT, m_playersCount[1]);
		break;

		//Tol'vir
	case 980:
		SetWorldState(ARENA_WORLD_STATE_NAGRAND_A_PLAYER_COUNT, m_playersCount[0]);
		SetWorldState(ARENA_WORLD_STATE_NAGRAND_H_PLAYER_COUNT, m_playersCount[1]);
		break;
	}

	if(!m_started)
		return;

	if(m_playersCount[1] == 0 && m_playersCount[0] > 0)
		m_winningteam = 0;
	else if(m_playersCount[0] == 0 && m_playersCount[1] > 0)
		m_winningteam = 1;
	else
		return;

	Finish();
}

uint32 Arena::CalcDeltaRating(uint32 oldRating, uint32 opponentRating, bool outcome)
{
	// ---- Elo Rating System ----
	// Expected Chance to Win for Team A vs Team B
	//                     1
	// -------------------------------------------
	//                   (PB - PA)/400
	//              1 + 10

	double power = (int)(opponentRating - oldRating) / 400.0f;
	double divisor = pow(((double)(10.0)), power);
	divisor += 1.0;

	double winChance = 1.0 / divisor;

	// New Rating Calculation via Elo
	// New Rating = Old Rating + K * (outcome - Expected Win Chance)
	// outcome = 1 for a win and 0 for a loss (0.5 for a draw ... but we cant have that)
	// K is the maximum possible change
	// Through investigation, K was estimated to be 32 (same as chess)
	double multiplier = (outcome ? 1.0 : 0.0) - winChance;
	int32 ret = long2int32(32.0 * multiplier);
	if( ret < -ARENA_TEAM_MAX_RATING )
		ret = -ARENA_TEAM_MAX_RATING;
	if( ret > ARENA_TEAM_MAX_RATING )
		ret = ARENA_TEAM_MAX_RATING;
	return ret;
}

void Arena::Finish()
{
	//only finish it once
	if( m_ended )
		return;
	bool SkipRewardsDueToNoFightBug = false;
	if( m_EnoughTeamsJoined == false )
		SkipRewardsDueToNoFightBug = true;
	//check if any of the teams made dmg. Anti early quit protection
	if( SkipRewardsDueToNoFightBug == false ) 
	{
		int32 dmg_sum = 0;
		MapMgr *mgr = GetMapMgr();
		PlayerStorageMap::iterator itr;
		for( itr = mgr->m_PlayerStorage.begin(); itr != mgr->m_PlayerStorage.end(); itr++ )
		{
			dmg_sum += itr->second->m_bgScore.DamageDone;
			dmg_sum += itr->second->m_bgScore.KillingBlows;
		}
		if( dmg_sum == 0 )
			SkipRewardsDueToNoFightBug = true;
		//one of the team did not even join the fight
		if( m_teams[0] == NULL || m_teams[1] == NULL )
			SkipRewardsDueToNoFightBug = true;
	}
	if( m_winningteam != -1 && m_SkipRewards == true && m_ScriptCPReward > 0 )
	{
		int32 PlayersActive = 0;
		MapMgr *mgr = GetMapMgr();
		PlayerStorageMap::iterator itr;
		for( itr = mgr->m_PlayerStorage.begin(); itr != mgr->m_PlayerStorage.end(); itr++ )
		{
			if( itr->second->m_bgScore.DamageDone || itr->second->m_bgScore.HealingDone )
				PlayersActive++;
		}
		if( PlayersActive > 10 ) //half + half of team size. Need to make this dynamic
			for( set<uint32>::iterator itr=m_players2[m_winningteam].begin(); itr != m_players2[m_winningteam].end(); ++itr )
			{
				Player *p = objmgr.GetPlayer(*itr);
				if( p )
					p->ModCurrencyCount( CURRENCY_CONQUEST_POINT, m_ScriptCPReward );
			}
	}
	if( SkipRewardsDueToNoFightBug == true || m_SkipRewards == true )
	{
		//revert match stats
		if( m_EnoughTeamsJoined == true )
		{
			for (uint32 i=0; i<2; i++)
				if (m_teams[i] != NULL) 
				{
					m_teams[i]->m_stat_gamesplayedseason--;
					m_teams[i]->m_stat_gamesplayedweek--;
					m_teams[i]->SaveToDB();
				}
			for(uint32 i = 0; i < 2; ++i)
			{
				for(InRangePlayerSet::iterator itr = m_players[i].begin(); itr != m_players[i].end(); ++itr) 
				{
					Player *plr = *itr;
					// update arena team stats 
					if(rated_match && plr->m_arenaTeams[m_arenateamtype] != NULL)
					{
						ArenaTeamMember * tp = m_teams[i]->GetMember(plr->getPlayerInfo());
						if(tp != NULL)
						{
							tp->Played_ThisWeek--;
							tp->Played_ThisSeason--;
						}
					}
				}
			}
		}
		m_winningteam = -1;
	}

	m_ended = true;

	uint32 TeamSize = 0;
	if( m_arenateamtype == ARENA_TEAM_TYPE_5V5 ) 
		TeamSize = 5;
	else if( m_arenateamtype == ARENA_TEAM_TYPE_3V3 ) 
		TeamSize = 3;
	else if( m_arenateamtype == ARENA_TEAM_TYPE_2V2 ) 
		TeamSize = 2;

	//pls no rewards for non started matches : ex team 1 joins and exits quickly
	/* update arena team stats */
	if( rated_match && m_started == true && m_winningteam != -1 )
	{
		m_deltaRating[0] = m_deltaRating[1] = 0;
		for (uint32 i = 0; i < 2; ++i) 
		{
			uint32 j = i ? 0 : 1; // opposing side
			bool outcome;

			if( m_teams[i] == NULL || m_teams[j] == NULL )
			{
				sLog.outDebug("Arena is ending but team %u is NULL",i);
				continue;
			}

			ArenaTeam *TTeam;
			TTeam = objmgr.GetArenaTeamById( m_teamsID[i] );
			if( TTeam != m_teams[i] )
			{
				m_teams[i] = NULL;
				sLog.outDebug("Arena is ending but team %u is not the same as we started with. Maybe team disbanded while we were in queue ?",i);
				continue;
			}
			TTeam = objmgr.GetArenaTeamById( m_teamsID[j] );
			if( TTeam != m_teams[j] )
			{
				m_teams[j] = NULL;
				sLog.outDebug("Arena is ending but team %u is not the same as we started with. Maybe team disbanded while we were in queue ?",j);
				continue;
			}

			outcome = ( i == m_winningteam );
			if (outcome)
			{
				m_teams[i]->m_stat_gameswonseason++;
				m_teams[i]->m_stat_gameswonweek++;
			}

			m_deltaRating[i] = CalcDeltaRating(m_teams[i]->m_stat_rating, m_teams[j]->m_stat_rating, outcome);

			float ServerVarRatingMod = 1.0f;
			if(m_teams[i]->m_type == ARENA_TEAM_TYPE_2V2)
				ServerVarRatingMod = sWorld.getRate(RATE_ARENAPOINTMULTIPLIER2X);
			else if(m_teams[i]->m_type == ARENA_TEAM_TYPE_3V3)
				ServerVarRatingMod = sWorld.getRate(RATE_ARENAPOINTMULTIPLIER3X);
			else if(m_teams[i]->m_type == ARENA_TEAM_TYPE_5V5)
				ServerVarRatingMod = sWorld.getRate(RATE_ARENAPOINTMULTIPLIER5X);
			m_deltaRating[i] = float2int32( m_deltaRating[i] * ServerVarRatingMod );

#ifdef PVP_AREANA_REALM
			uint8 CanAwardPoints = 1;
			//do not award points for killing recently created arena team
			if( m_teams[j]->m_stat_gamesplayedseason == 0 )
				CanAwardPoints = 0;
			//do not award point for farming the same team over and over again
			float DiminishingCoeff = 1;
			uint32 MatchesPlayedTodayPair = objmgr.GetIncreasePVPDiminishing( m_teamsID[i], m_teamsID[j], i == 0 );
			for(uint32 i=0;i<MatchesPlayedTodayPair/4;i++)
				DiminishingCoeff = DiminishingCoeff / 2.0f;

			if( CanAwardPoints == 0 )
				DiminishingCoeff = 0;
			m_deltaRating[i] = m_deltaRating[i] * DiminishingCoeff;
#endif

			m_teams[i]->m_stat_rating += m_deltaRating[i];
			if (m_teams[i]->m_stat_rating < ARENA_TEAM_MIN_RATING)
				m_teams[i]->m_stat_rating = ARENA_TEAM_MIN_RATING;
			else if( m_teams[i]->m_stat_rating > ARENA_TEAM_MAX_RATING )
				m_teams[i]->m_stat_rating = ARENA_TEAM_MAX_RATING;

			for( set<uint32>::iterator itr=m_players2[i].begin(); itr != m_players2[i].end(); ++itr )
			{
				PlayerInfo * info = objmgr.GetPlayerInfo(*itr);
				if (info) 
				{
					//what the heck ? player switched arena team while playing the arena match ? How is this possible ?
					if( info->m_loggedInPlayer && info->m_loggedInPlayer->m_arenaTeams[ m_arenateamtype ] != m_teams[i] )
					{
//						ASSERT( 0 );
						continue;
					}

					ArenaTeamMember * tp = m_teams[i]->GetMember(info);

					if(tp != NULL)
					{
						int32 PlayerDeltaRating = CalcDeltaRating(tp->PersonalRating, m_teams[j]->m_stat_rating, outcome);
						PlayerDeltaRating = float2int32( PlayerDeltaRating * ServerVarRatingMod );
						tp->PersonalRating += PlayerDeltaRating;
						if (tp->PersonalRating < ARENA_PERSONAL_MIN_RATING)
							tp->PersonalRating = ARENA_PERSONAL_MIN_RATING;
						else if( tp->PersonalRating > ARENA_TEAM_MAX_RATING )
							tp->PersonalRating = ARENA_TEAM_MAX_RATING;
						info->m_loggedInPlayer->Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_REACH_PERSONAL_RATING,TeamSize,ACHIEVEMENT_UNUSED_FIELD_VALUE,tp->PersonalRating,ACHIEVEMENT_EVENT_ACTION_SET_MAX);

						if(outcome) 
						{
							++(tp->Won_ThisWeek);
							++(tp->Won_ThisSeason);
						}
						//reward winning team
						if( i == m_winningteam )
						{
							int32 CurrencyGainLimit = 0x0FFFFF;
							int32 CurrencyGainPlayer = 180;	//blizzlike

							if( m_arenateamtype == ARENA_TEAM_TYPE_5V5 ) // 5v5
								CurrencyGainLimit = ARENA_CURRENCY_WIN_AMT5;
							else if( m_arenateamtype == ARENA_TEAM_TYPE_3V3 ) // 3v3
								CurrencyGainLimit = ARENA_CURRENCY_WIN_AMT3;
							else if( m_arenateamtype == ARENA_TEAM_TYPE_2V2 ) // 2v2
								CurrencyGainLimit = ARENA_CURRENCY_WIN_AMT2;

							CurrencyGainLimit *= ServerVarRatingMod;
							
							CurrencyGainPlayer = MIN( CurrencyGainLimit, CurrencyGainPlayer );
							CurrencyGainPlayer = MAX( ARENA_CURRENCY_WIN_AMT_MIN, CurrencyGainPlayer );

							if( info->m_loggedInPlayer )
							{
								info->m_loggedInPlayer->m_bgScore.RatingChange = PlayerDeltaRating;
								info->m_loggedInPlayer->ModCurrencyCount( CURRENCY_CONQUEST_POINT, CurrencyGainPlayer );
							}
						}
#ifdef PVP_AREANA_REALM
						if( info->m_loggedInPlayer )
						{
							#define ALLOWED_GOLD_GAIN_PER_DAY 5000.0f
							float BaseGoldGain = ( 1000 / 6.0f );
							uint32 GoldGainTotal = objmgr.GetPVPGoldDiminishing( info->m_loggedInPlayer->GetGUID() );
							float DiminishingCoeff = 1.0f - (float)GoldGainTotal / ALLOWED_GOLD_GAIN_PER_DAY;
							if( i != m_winningteam )
								DiminishingCoeff /= 2.0f;
							if( DiminishingCoeff < 0.0f || DiminishingCoeff > 1.0f )
								DiminishingCoeff = 0.0f;
							uint32 ModGold = BaseGoldGain * DiminishingCoeff;
							objmgr.ModPVPGoldDiminishing( info->m_loggedInPlayer->GetGUID(), ModGold );
							info->m_loggedInPlayer->BroadcastMessage( "Arena diminishing coefficient is %f. Reward gold %d", DiminishingCoeff, (uint32)(ModGold) );
							info->m_loggedInPlayer->ModGold( ModGold * 10000 );
						}
#endif
					}
				}
			}

			m_teams[i]->SaveToDB();
		}
		objmgr.UpdateArenaTeamRankings();
	}

	m_nextPvPUpdateTime = 0;

//	if( m_teams[0] == NULL )
//		m_winningteam = 1;
//	else if( m_teams[1] == NULL )
//		m_winningteam = 0;

	UpdatePvPData(); //show leave arena menu
	PlaySoundToAll(m_winningteam ? SOUND_ALLIANCEWINS : SOUND_HORDEWINS);

	sEventMgr.RemoveEvents(this, EVENT_BATTLEGROUND_CLOSE);
	sEventMgr.AddEvent(static_cast<CBattleground*>(this), &CBattleground::Close, EVENT_BATTLEGROUND_CLOSE, 20000, 1,EVENT_FLAG_DELETES_OBJECT);
	//keep spamming this so people can see the "leave" button. This is blizzlike !
	sEventMgr.AddEvent(static_cast<CBattleground*>(this), &CBattleground::UpdatePvPData, EVENT_BATTLEGROUND_COUNTDOWN, 5000, 1, EVENT_FLAG_DELETES_OBJECT );

	for(int i = 0; i < 2; i++)
	{
		bool victorious = (i == m_winningteam);
		InRangePlayerSet::iterator itr = m_players[i].begin();
		for(; itr != m_players[i].end(); itr++)
		{
			Player * plr = SafePlayerCast(*itr);
			if (plr != NULL)
			{
				if( m_teams[i] != NULL )
				{
					plr->Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_PLAY_ARENA,plr->GetMapId(),ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
					plr->Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_REACH_TEAM_RATING,TeamSize,ACHIEVEMENT_UNUSED_FIELD_VALUE,m_teams[i]->m_stat_rating,ACHIEVEMENT_EVENT_ACTION_SET_MAX);
				}
				if( victorious )
				{
					plr->Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_WIN_ARENA,plr->GetMapId(),ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
					//there is a special filter here for "in row" wins
					if( rated_match )
						plr->Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_WIN_RATED_ARENA,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
				}
				sHookInterface.OnArenaFinish(plr, plr->m_arenaTeams[m_arenateamtype], victorious, rated_match);
				plr->ResetAllCooldowns();
			}
		}
	}
}

LocationVector Arena::GetStartingCoords(uint32 Team)
{
	// 559, 562, 572
	/*
	A start
	H start
	Repop
	572 1295.322388 1585.953369 31.605387
	572 1277.105103 1743.956177 31.603209
	572 1286.112061 1668.334961 39.289127

	562 6184.806641 236.643463 5.037095
	562 6292.032227 287.570343 5.003577
	562 6241.171875 261.067322 0.891833

	559 4085.861328 2866.750488 12.417445
	559 4027.004883 2976.964844 11.600499
	559 4057.042725 2918.686523 13.051933
	*/
	switch(m_mapMgr->GetMapId())
	{
		/* loraedeon */
	case 572:
		{
			if(Team)
				return LocationVector(1277.105103f, 1743.956177f, 31.603209f);
			else
				return LocationVector(1295.322388f, 1585.953369f, 31.605387f);
		}break;

		/* blades edge arena */
	case 562:
		{
			if(Team)
				return LocationVector(6292.032227f, 287.570343f, 5.003577f);
			else
				return LocationVector(6184.806641f, 236.643463f, 5.037095f);
		}break;

		/* nagrand arena */
	case 559:
		{
			if(Team)
				return LocationVector(4027.004883f, 2976.964844f, 11.600499f);
			else
				return LocationVector(4085.861328f, 2866.750488f, 12.417445f);
		}break;
		//Dalaran Sewers
	case 617:
		{
			if(Team)
				return LocationVector(1221.0f, 765.0f, 15.0f, 6.25f);
			else
				return LocationVector(1363.0f, 817.0f, 15.0f, 3.12f);
		}break;
		//Ring of Valor
	case 618:
		{
			if(Team)
//				return LocationVector(764.0f, -274.0f, 3.5f, 0.0f);
				return LocationVector(696.0f, -264.0f, 29.0f, 5.7f);
			else
//				return LocationVector(764.0f, -293.0f, 3.5f, 0.0f);
				return LocationVector(829.0f, -302.0f, 29.0f, 2.7f);
		}break;
		//Tol'vir
	case 980:
		{
			if(Team)
				return LocationVector(-10650.0f, 428.0f, 25.0f, 3.15f);
			else
				return LocationVector(-10779.0f, 431.0f, 25.0f, 6.20f);
		}break;
	}

	return LocationVector(0,0,0,0);
}

bool Arena::HookHandleRepop(Player * plr)
{
	// 559, 562, 572
	/*
	A start
	H start
	Repop
	572 1295.322388 1585.953369 31.605387
	572 1277.105103 1743.956177 31.603209
	572 1286.112061 1668.334961 39.289127

	562 6184.806641 236.643463 5.037095
	562 6292.032227 287.570343 5.003577
	562 6241.171875 261.067322 0.891833

	559 4085.861328 2866.750488 12.417445
	559 4027.004883 2976.964844 11.600499
	559 4057.042725 2918.686523 13.051933
	*/
	LocationVector dest(0,0,0,0);
	switch(m_mapMgr->GetMapId())
	{
		/* loraedeon */
	case 572: {
			dest.ChangeCoords(1286.112061f, 1668.334961f, 39.289127f);
		}break;

		/* blades edge arena */
	case 562: {
			dest.ChangeCoords(6241.171875f, 261.067322f, 0.891833f);
		}break;

		/* nagrand arena */
	case 559: {
			dest.ChangeCoords(4057.042725f, 2918.686523f, 13.051933f);
		}break;
		//Dalaran Sewers
	case 617: 
			dest.ChangeCoords(1299.046f, 784.825f, 9.338f, 2.422f);
		break;
		//Ring of Valor
	case 618: 
			dest.ChangeCoords(763.5f, -284, 28.276f, 2.422f);
		//Tol'vir
	case 980: 
			dest.ChangeCoords(-10717, 429, 25.0f, 2.422f);
		break;
	}

	plr->SafeTeleportDelayed(m_mapMgr->GetMapId(), m_mapMgr->GetInstanceID(), dest);
	return true;
}

void Arena::HookOnAreaTrigger(Player * plr, uint32 id)
{
	int32 buffslot = -1;

	ASSERT(plr != NULL);

/*
	//Tudi : Sadly not all maps have areatriggers. We will fix it by making GOs handle themself as separate modules
	switch (id)
	{
		case 4536:
		case 4538:
		case 4696:
			buffslot = 0;
			break;
		case 4537:
		case 4539:
		case 4697:
			buffslot = 1;
			break;
	} */

	if(buffslot >= 0)
	{
		if(m_buffs[buffslot] != NULL && m_buffs[buffslot]->IsInWorld())
		{
			/* apply the buff */
			SpellEntry * sp = dbcSpell.LookupEntry(m_buffs[buffslot]->GetInfo()->sound3);
			Spell * s = SpellPool.PooledNew( __FILE__, __LINE__ );

			ASSERT(sp != NULL);
			ASSERT(s != NULL);

			s->Init(plr, sp, true, 0);
			SpellCastTargets targets(plr->GetGUID());
			s->prepare(&targets);

			/* despawn the gameobject (not delete!) */
			m_buffs[buffslot]->Despawn(0, BUFF_RESPAWN_TIME);
		}
	}
}
void Arena::HookGenerateLoot( Player * plr, Object * pCorpse )// Not Used
{
}

void Arena::HookOnUnitKill(Player * plr, Unit * pVictim)
{
}

void Arena::HookOnFlagDrop(Player *plr)
{
}
