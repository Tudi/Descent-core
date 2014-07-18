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

/*
SetWorldState(0x00001748,0x00000000);
SetWorldState(0x00001749,0x00000000);
SetWorldState(0x00000700,0x00000000);
SetWorldState(0x00000701,0x00000000);
SetWorldState(0x00000702,0x00000000);
SetWorldState(0x00000703,0x00000000);
SetWorldState(0x000006E7,0x00000000);
SetWorldState(0x000006E8,0x00000000);
SetWorldState(0x000006E9,0x00000001);
SetWorldState(0x000006EA,0x00000000);
SetWorldState(0x000006EC,0x00000000);
SetWorldState(0x000006ED,0x00000000);
SetWorldState(0x000006EE,0x00000001);
SetWorldState(0x000006EF,0x00000000);
SetWorldState(0x000006F0,0x00000064);
SetWorldState(0x000006F1,0x000000D2);
SetWorldState(0x000006F2,0x00000000);
SetWorldState(0x000006F3,0x00000001);
SetWorldState(0x000006F4,0x000007D0);
SetWorldState(0x000006F6,0x00000001);
SetWorldState(0x000006F7,0x00000000);
SetWorldState(0x000006F8,0x00000000);
SetWorldState(0x000006F9,0x00000000);
SetWorldState(0x000006FB,0x00000000);
SetWorldState(0x000006FC,0x00000000);
SetWorldState(0x000006FD,0x00000000);
SetWorldState(0x000006FE,0x00000000);
SetWorldState(0x00000732,0x00000000);
SetWorldState(0x00000733,0x00000001);
SetWorldState(0x00000734,0x00000001);
SetWorldState(0x00000735,0x00000000);
SetWorldState(0x00000736,0x00000000);
SetWorldState(0x00000745,0x00000002);
SetWorldState(0x000007A3,0x00000708);
*/
#define BASE_RESOURCES_GAIN			10
#define RESOURCES_WARNING_THRESHOLD 1800
#define RESOURCES_WINVAL			2000
uint32 BFGbuffentrys[3] = {180380,180362,180146}; //berserk, food, speed
// BFG define's
#define BFG_CAPTURED_LIGHTHOUSE_ALLIANCE			0x6E7	//1767
#define BFG_CAPTURED_LIGHTHOUSE_HORDE				0x6E8	//1768
#define BFG_CAPTURING_LIGHTHOUSE_ALLIANCE			0x6E9	//1769
#define BFG_CAPTURING_LIGHTHOUSE_HORDE				0x6EA	//1770

#define BFG_CAPTURED_MINE_ALLIANCE					0x6EC	// 1772 1 is captured by the alliance
#define BFG_CAPTURED_MINE_HORDE						0x6ED	// 1773 1 is captured by the horde
#define BFG_CAPTURING_MINE_ALLIANCE					0x6EE	// 1774 1 is capturing by the alliance
#define BFG_CAPTURING_MINE_HORDE					0x6EF	// 1775 1 is capturing by the horde

#define BFG_CAPTURED_WATERWORKS_ALLIANCE			0x6F6	// 1782
#define BFG_CAPTURED_WATERWORKS_HORDE				0x6F7	// 1783
#define BFG_CAPTURING_WATERWORKS_ALLIANCE			0x6F8	// 1784
#define BFG_CAPTURING_WATERWORKS_HORDE				0x6F9	// 1785

#define BFG_SHOW_LIGHTHOUSE_ICON					0x732	//1842
#define BFG_SHOW_MINE_ICON							0x735	//1845
#define BFG_SHOW_WATERWORKS_ICON					0x736	//1846

/* BFG Battleground Data */

	static float GraveyardLocations[BFG_NUM_CONTROL_POINTS][3] =
	{
		{ 1252.22998047f, 836.54699707f, 28.7894992828f },											// MINE
		{ 1034.81994629f, 1335.57995605f, 13.0094995499f },											// LIGHTHOUSE
		{ 887.57800293f, 937.336975098f, 24.7737007141f }											// WATERWORKS
	};

	static float NoBaseGYLocations[2][3] = 
	{
		{ 909.460021973f, 1337.35998535f, 28.6448993683f },											// ALLIANCE
		{ 1430.43994141f, 983.861022949f, 1.225734993815f }											// HORDE
	};

	static const char * ControlPointNames[BFG_NUM_CONTROL_POINTS] =
	{
		"Mine",																						// MINE
		"LightHouse",																				// LIGHTHOUSE
		"Waterworks"																				// WATERWORKS
	};

	static uint32 ControlPointGoIds[BFG_NUM_CONTROL_POINTS][BFG_NUM_SPAWN_TYPES] = {
		  // NEUTRAL    ALLIANCE-ATTACK    HORDE-ATTACK    ALLIANCE-CONTROLLED    HORDE_CONTROLLED
		{ 180087,       180085,            180086,         180076,                180078 },			// MINE	
		{ 180089,       180085,            180086,         180076,                180078 },			// LIGHTHOUSE
		{ 180088,       180085,            180086,         180076,                180078 }			// WATERWORKS
	};

	static float ControlPointCoordinates[BFG_NUM_CONTROL_POINTS][4] = 
	{
		{ 1251.02f, 958.359f, 5.67407f, 2.77507f },									// MINE
		{ 1057.91f, 1278.33f, 3.27763f, 4.97419f },									// LIGHTHOUSE
		{ 980.08f, 948.707f, 12.7478f, 2.74016f }									// WATERWORKS
	};

	static float ControlPointRotations[BFG_NUM_CONTROL_POINTS][2] = 
	{
		{ 0, 0 },																	// MINE
		{ 0, 0 },																	// LIGHTHOUSE
		{ 0, 0 }																	// WATERWORKS
	};

	static float BuffCoordinates[BFG_NUM_BUFFS][4] = 
	{
		{ 990.297f, 983.373f, 12.9826f, 4.55531f },			// 6265
		{ 1111.43f, 921.616f, 30.1085f, 5.5676f },			// 6266
		{ 966.458f, 1043.57f, 13.1467f, 0.90757f },			// 6267
		{ 1195.73f, 1020.30f, 7.97874f, 5.77704f },			// 6268
		{ 1063.56f, 1308.98f, 4.91169f, 4.03171f },			// 6269
	};

	static float BuffRotations[BFG_NUM_BUFFS][2] = 
	{
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
	};

	static uint32 AssaultFields[BFG_NUM_CONTROL_POINTS][2] = 
	{
		{ BFG_CAPTURING_MINE_ALLIANCE, BFG_CAPTURING_MINE_HORDE },									// MINE
		{ BFG_CAPTURING_LIGHTHOUSE_ALLIANCE, BFG_CAPTURING_LIGHTHOUSE_HORDE },						// LIGHTHOUSE
		{ BFG_CAPTURING_WATERWORKS_ALLIANCE, BFG_CAPTURING_WATERWORKS_HORDE },						// WATERWORKS
	};

	static uint32 OwnedFields[BFG_NUM_CONTROL_POINTS][2] = 
	{
		{ BFG_CAPTURED_MINE_ALLIANCE, BFG_CAPTURED_MINE_HORDE },									// MINE
		{ BFG_CAPTURED_LIGHTHOUSE_ALLIANCE, BFG_CAPTURED_LIGHTHOUSE_HORDE },						// LIGHTHOUSE
		{ BFG_CAPTURED_WATERWORKS_ALLIANCE, BFG_CAPTURED_WATERWORKS_HORDE },						// WATERWORKS
	};

	static uint32 NeutralFields[BFG_NUM_CONTROL_POINTS] = 
	{
		BFG_SHOW_MINE_ICON,
		BFG_SHOW_LIGHTHOUSE_ICON,
		BFG_SHOW_WATERWORKS_ICON,
	};

	//this might not work as expected
	static uint32 ResourceUpdateIntervals[4] = 
	{
		0,
		12000,
		9000,
		6000,
	};

	static uint32 PointBonusPerUpdate[4] = 
	{
		0,
		10,
		15,
		25,
	};

//								<10 <20 <30 <40 <50 <60 <70 70
static int resHonorTable[8] = { 0,  0,  4,  7,  11, 19, 20, 20 };
static int winHonorTable[8] = { 0,  0,  4,  7,  11, 19, 20, 20 };

static uint32 resourcesToGainBH = 330;
static uint32 resourcesToGainBR = 200;

/* End BG Data */

void BattleForGilneas::SpawnBuff(uint32 x)
{
	uint32 chosen_buffid;
	chosen_buffid = BFGbuffentrys[RandomUInt(2)];
	GameObjectInfo * goi = GameObjectNameStorage.LookupEntry(chosen_buffid);
	if(goi == NULL)
		return;

	if( m_buffs[x] != NULL )
	{
		m_buffs[x]->Despawn( 0, 0 );
		m_buffs[x] = NULL;
	}

	if(m_buffs[x] == NULL)
	{
		m_buffs[x] = SpawnGameObject(chosen_buffid, m_mapMgr->GetMapId(), BuffCoordinates[x][0], BuffCoordinates[x][1], BuffCoordinates[x][2],
			BuffCoordinates[x][3], 0, 114, 1);

		m_buffs[x]->SetFloatValue(GAMEOBJECT_PARENTROTATION_2, BuffRotations[x][0]);
		m_buffs[x]->SetFloatValue(GAMEOBJECT_PARENTROTATION_3, BuffRotations[x][1]);
		m_buffs[x]->SetByte(GAMEOBJECT_BYTES_1, 0, 1);
		m_buffs[x]->SetByte(GAMEOBJECT_BYTES_1, 1, 6);
		m_buffs[x]->SetByte(GAMEOBJECT_BYTES_1, 3, 100);
		m_buffs[x]->PushToWorld(m_mapMgr);
	}
	else
	{
		// only need to reassign guid if the entry changes.
		if(m_buffs[x]->IsInWorld())
			m_buffs[x]->RemoveFromWorld(false);

		if(chosen_buffid != m_buffs[x]->GetEntry())
		{
			m_buffs[x]->SetNewGuid(m_mapMgr->GenerateGameobjectGuid());
			m_buffs[x]->SetUInt32Value(OBJECT_FIELD_ENTRY, chosen_buffid);
			m_buffs[x]->SetInfo(goi);
		}

		m_buffs[x]->PushToWorld(m_mapMgr);
	}
}

void BattleForGilneas::SpawnControlPoint(uint32 Id, uint32 Type)
{
	GameObjectInfo * gi, * gi_aura;
	gi = GameObjectNameStorage.LookupEntry(ControlPointGoIds[Id][Type]);
	if(gi == NULL)
		return;

	gi_aura = gi->sound3 ? GameObjectNameStorage.LookupEntry(gi->sound3) : NULL;

	if( m_controlPoints[Id] != NULL )
	{
		m_controlPoints[Id]->Despawn( 0, 0 );
		m_controlPoints[Id] = NULL;
	}

	if(m_controlPoints[Id] == NULL)
	{
		m_controlPoints[Id] = SpawnGameObject(gi->ID, m_mapMgr->GetMapId(), ControlPointCoordinates[Id][0], ControlPointCoordinates[Id][1],
			ControlPointCoordinates[Id][2], ControlPointCoordinates[Id][3], 0, 35, 1.0f);

		m_controlPoints[Id]->SetFloatValue(GAMEOBJECT_PARENTROTATION_2, ControlPointRotations[Id][0]);
		m_controlPoints[Id]->SetFloatValue(GAMEOBJECT_PARENTROTATION_3, ControlPointRotations[Id][1]);
		m_controlPoints[Id]->SetByte(GAMEOBJECT_BYTES_1, 0, 1);
		m_controlPoints[Id]->SetByte(GAMEOBJECT_BYTES_1, 1, gi->Type);
		m_controlPoints[Id]->SetByte(GAMEOBJECT_BYTES_1, 3, 100);
		m_controlPoints[Id]->SetUInt32Value(GAMEOBJECT_DYNAMIC, 1);
		m_controlPoints[Id]->SetUInt32Value(GAMEOBJECT_DISPLAYID, gi->DisplayID);

		switch(Type)
		{
		case BFG_SPAWN_TYPE_ALLIANCE_ASSAULT:
		case BFG_SPAWN_TYPE_ALLIANCE_CONTROLLED:
			m_controlPoints[Id]->SetUInt32Value(GAMEOBJECT_FACTION, 2);
			break;

		case BFG_SPAWN_TYPE_HORDE_ASSAULT:
		case BFG_SPAWN_TYPE_HORDE_CONTROLLED:
			m_controlPoints[Id]->SetUInt32Value(GAMEOBJECT_FACTION, 1);
			break;

		default:
			m_controlPoints[Id]->SetUInt32Value(GAMEOBJECT_FACTION, 35);		// neutral
			break;
		}

		m_controlPoints[Id]->bannerslot = Id;
		m_controlPoints[Id]->PushToWorld(m_mapMgr);
//		m_mapMgr->PushStaticObject( m_controlPoints[Id], true );
	}
	else
	{
		if(m_controlPoints[Id]->IsInWorld())
		{
			m_controlPoints[Id]->RemoveFromWorld(false);
//			m_mapMgr->RemoveStaticObject( m_controlPoints[Id], true );
		}

		// assign it a new guid (client needs this to see the entry change?)
		m_controlPoints[Id]->SetNewGuid(m_mapMgr->GenerateGameobjectGuid());
		m_controlPoints[Id]->SetUInt32Value(OBJECT_FIELD_ENTRY, gi->ID);
		m_controlPoints[Id]->SetUInt32Value(GAMEOBJECT_DISPLAYID, gi->DisplayID);
		m_controlPoints[Id]->SetByte(GAMEOBJECT_BYTES_1, 1, gi->Type);

		switch(Type)
		{
		case BFG_SPAWN_TYPE_ALLIANCE_ASSAULT:
		case BFG_SPAWN_TYPE_ALLIANCE_CONTROLLED:
			m_controlPoints[Id]->SetUInt32Value(GAMEOBJECT_FACTION, 2);
			break;

		case BFG_SPAWN_TYPE_HORDE_ASSAULT:
		case BFG_SPAWN_TYPE_HORDE_CONTROLLED:
			m_controlPoints[Id]->SetUInt32Value(GAMEOBJECT_FACTION, 1);
			break;

		default:
			m_controlPoints[Id]->SetUInt32Value(GAMEOBJECT_FACTION, 35);		// neutral
			break;
		}

		m_controlPoints[Id]->SetInfo(gi);
		m_controlPoints[Id]->PushToWorld(m_mapMgr);
	}

	if(gi_aura==NULL)
	{
		// remove it if it exists
		if(m_controlPointAuras[Id]!=NULL && m_controlPointAuras[Id]->IsInWorld())
			m_controlPointAuras[Id]->RemoveFromWorld(false);
			
		return;
	}

	if( m_controlPointAuras[Id] != NULL )
	{
		m_controlPointAuras[Id]->Despawn( 0, 0 );
		m_controlPointAuras[Id] = NULL;
	}

	if(m_controlPointAuras[Id] == NULL)
	{
		m_controlPointAuras[Id] = SpawnGameObject(gi_aura->ID, m_mapMgr->GetMapId(), ControlPointCoordinates[Id][0], ControlPointCoordinates[Id][1],
			ControlPointCoordinates[Id][2], ControlPointCoordinates[Id][3], 0, 35, 1.0f);

		m_controlPointAuras[Id]->SetFloatValue(GAMEOBJECT_PARENTROTATION_2, ControlPointRotations[Id][0]);
		m_controlPointAuras[Id]->SetFloatValue(GAMEOBJECT_PARENTROTATION_3, ControlPointRotations[Id][1]);
		m_controlPointAuras[Id]->SetByte(GAMEOBJECT_BYTES_1, 0, 1);
		m_controlPointAuras[Id]->SetByte(GAMEOBJECT_BYTES_1, 1, 6);
		m_controlPointAuras[Id]->SetByte(GAMEOBJECT_BYTES_1, 3, 100);
		m_controlPointAuras[Id]->bannerauraslot = Id;
		m_controlPointAuras[Id]->PushToWorld(m_mapMgr);
	}
	else
	{
		if(m_controlPointAuras[Id]->IsInWorld())
			m_controlPointAuras[Id]->RemoveFromWorld(false);

		// re-spawn the aura
		m_controlPointAuras[Id]->SetNewGuid(m_mapMgr->GenerateGameobjectGuid());
		m_controlPointAuras[Id]->SetUInt32Value(OBJECT_FIELD_ENTRY, gi_aura->ID);
		m_controlPointAuras[Id]->SetUInt32Value(GAMEOBJECT_DISPLAYID, gi_aura->DisplayID);
		m_controlPointAuras[Id]->SetInfo(gi_aura);
		m_controlPointAuras[Id]->PushToWorld(m_mapMgr);
	}	
}

void BattleForGilneas::OnCreate()
{
	GameObject *gate;
	// Alliance Gate
	gate = SpawnGameObject(207177, 761, 918.391f, 1336.64f, 27.4252f, 2.84488f, 32, 114, 0.34f);
	gate->SetByte(GAMEOBJECT_BYTES_1, 3, 100);
	gate->PushToWorld(m_mapMgr);
	m_gates.push_back(gate);
	gate = SpawnGameObject(205496, 761, 918.299f, 1336.49f, 20.455f, 2.82743f, 32, 114, 1.00f);
	gate->SetByte(GAMEOBJECT_BYTES_1, 3, 100);
	gate->PushToWorld(m_mapMgr);
	m_gates.push_back(gate);

	// horde gate
	gate = SpawnGameObject(207178, 761, 1395.97f, 977.09f, 7.63597f, 6.27446f, 32, 114, 1.05f);
	gate->SetByte(GAMEOBJECT_BYTES_1, 3, 100);
	gate->PushToWorld(m_mapMgr);
	m_gates.push_back(gate);
	gate = SpawnGameObject(205495, 761, 1395.96f, 977.257f, -13.7897f, 6.26573f, 32, 114, 1.00f);
	gate->SetByte(GAMEOBJECT_BYTES_1, 3, 100);
	gate->PushToWorld(m_mapMgr);
	m_gates.push_back(gate);

	// spawn (default) control points
	SpawnControlPoint(BFG_CONTROL_POINT_LIGHTHOUSE,		BFG_SPAWN_TYPE_NEUTRAL);
	SpawnControlPoint(BFG_CONTROL_POINT_WATERWORKS,		BFG_SPAWN_TYPE_NEUTRAL);
	SpawnControlPoint(BFG_CONTROL_POINT_MINE,			BFG_SPAWN_TYPE_NEUTRAL);

	// spawn buffs
	for( uint32 i=0;i<BFG_NUM_BUFFS;i++)
		SpawnBuff( i );

	// spawn the h/a base spirit guides
	AddSpiritGuide(SpawnSpiritGuide(NoBaseGYLocations[0][0],NoBaseGYLocations[0][1],NoBaseGYLocations[0][2], 0.0f, 0));
	AddSpiritGuide(SpawnSpiritGuide(NoBaseGYLocations[1][0],NoBaseGYLocations[1][1],NoBaseGYLocations[1][2], 0.0f, 1));

	// urrrgh worldstates. Copy pasted from packet, need to research
	SetWorldState(0x00001749,0x00000000);
	SetWorldState(0x00001748,0x00000000);
	SetWorldState(0x00000702,0x00000000);
	SetWorldState(0x000006FD,0x00000000);
	SetWorldState(0x000006F7,0x00000000);
	SetWorldState(0x000006F0,0x00000064);
	SetWorldState(0x00000703,0x00000000);
	SetWorldState(0x000006ED,0x00000000);
	SetWorldState(0x000006EA,0x00000000);
	SetWorldState(0x000006E7,0x00000000);
	SetWorldState(0x00000701,0x00000000);
	SetWorldState(0x00000700,0x00000000);
	SetWorldState(0x000006FE,0x00000000);
	SetWorldState(0x000006FC,0x00000000);
	SetWorldState(0x000006FB,0x00000000);
	SetWorldState(0x000006F8,0x00000000);
	SetWorldState(0x000006F6,0x00000001);
	SetWorldState(0x000006F2,0x00000000);
	SetWorldState(0x000006EF,0x00000000);
	SetWorldState(0x000006F9,0x00000000);
	SetWorldState(0x000006F1,0x000000D2);
	SetWorldState(0x000006E9,0x00000001);
	SetWorldState(0x000006EE,0x00000001);
	SetWorldState(0x000006EC,0x00000000);
	SetWorldState(0x000006E8,0x00000000);
	SetWorldState(0x000007A3,RESOURCES_WARNING_THRESHOLD);
	SetWorldState(0x00000732,0x00000000);
	SetWorldState(0x00000733,0x00000001);
	SetWorldState(0x00000734,0x00000001);
	SetWorldState(0x00000735,0x00000000);
	SetWorldState(0x00000736,0x00000000);
	SetWorldState(0x00000745,0x00000002); //pvp map ?

	// BFG world state's

	// Icon stuff for on the map
	SetWorldState(BFG_SHOW_WATERWORKS_ICON, 			 0x01);
	SetWorldState(BFG_SHOW_MINE_ICON, 				     0x01);
	SetWorldState(BFG_SHOW_LIGHTHOUSE_ICON, 			 0x01);

	// WATERWORKS
	SetWorldState(BFG_CAPTURING_WATERWORKS_HORDE, 	     0x00);
	SetWorldState(BFG_CAPTURING_WATERWORKS_ALLIANCE, 	 0x00);
	SetWorldState(BFG_CAPTURED_WATERWORKS_HORDE, 		 0x00);
	SetWorldState(BFG_CAPTURED_WATERWORKS_ALLIANCE, 	 0x00);

	SetWorldState(BFG_MAX_SCORE, 						 RESOURCES_WINVAL);
	SetWorldState(BFG_ALLIANCE_CAPTUREBASE, 			 0x00);
	SetWorldState(BFG_HORDE_CAPTUREBASE, 				 0x00);
	SetWorldState(BFG_HORDE_RESOURCES, 				     0x00);
	SetWorldState(BFG_ALLIANCE_RESOURCES, 			     0x00);

	// MINE
	SetWorldState(BFG_CAPTURING_MINE_ALLIANCE, 		     0x00);
	SetWorldState(BFG_CAPTURING_MINE_HORDE, 			 0x00);
	SetWorldState(BFG_CAPTURED_MINE_HORDE, 			     0x00);
	SetWorldState(BFG_CAPTURED_MINE_ALLIANCE, 		     0x00);

	// LIGHTHOUSE
	SetWorldState(BFG_CAPTURING_LIGHTHOUSE_HORDE, 		  0x00);
	SetWorldState(BFG_CAPTURING_LIGHTHOUSE_ALLIANCE, 	  0x00);
	SetWorldState(BFG_CAPTURED_LIGHTHOUSE_HORDE, 		  0x00);
	SetWorldState(BFG_CAPTURED_LIGHTHOUSE_ALLIANCE, 	  0x00);
}

void BattleForGilneas::OnStart()
{
	for(uint32 i = 0; i < 2; ++i) 
		for(InRangePlayerSet::iterator itr = m_players[i].begin(); itr != m_players[i].end(); ++itr)
			(*itr)->RemoveAura(BG_PREPARATION,0,AURA_SEARCH_POSITIVE);

	// open gates
	for(list<GameObject*>::iterator itr = m_gates.begin(); itr != m_gates.end(); ++itr)
	{
		(*itr)->SetUInt32Value(GAMEOBJECT_FLAGS, 64);
		(*itr)->SetByte(GAMEOBJECT_BYTES_1, 0, 0);
	}

	/* correct? - burlex */
	PlaySoundToAll(SOUND_BATTLEGROUND_BEGIN);

	m_started = true;
}

BattleForGilneas::BattleForGilneas(MapMgr * mgr, uint32 id, uint32 lgroup, uint32 t) : CBattleground(mgr,id,lgroup,t)
{
	int i;

	for (i=0; i<2; i++) 
	{
		m_players[i].clear();
		m_pendPlayers[i].clear();
	}
	//m_worldStates.clear();
	m_pvpData.clear();
	m_resurrectMap.clear();

	for(i = 0; i < BFG_NUM_BUFFS; ++i)
	{
		m_buffs[i] = NULL;
	}
	for(i = 0; i < BFG_NUM_CONTROL_POINTS; ++i)
	{
		m_controlPointAuras[i] = NULL;
		m_controlPoints[i] = NULL;
		m_spiritGuides[i] = NULL;
		m_basesAssaultedBy[i] = -1;
		m_basesOwnedBy[i] = -1;
		m_basesLastOwnedBy[i] = -1;
	}

	for(i = 0; i < 2; ++i)
	{
		m_resources[i] = 0;
		m_capturedBases[i] = 0;
		m_lastHonorGainResources[i] = 0;
		m_lastRepGainResources[i] = 0;
		m_nearingVictory[i] = false;
	}

	for(i = 0; i < BFG_NUM_CONTROL_POINTS; ++i)
	{
		DefFlag[i][0] = false;
		DefFlag[i][1] = true;
	}

	m_lgroup = lgroup;
}

BattleForGilneas::~BattleForGilneas()
{
	// gates are always spawned, so mapmgr will clean them up
	for(uint32 i = 0; i < BFG_NUM_CONTROL_POINTS; ++i)
	{
		// buffs may not be spawned, so delete them if they're not
		if(m_buffs[i] != NULL)
		{
			m_buffs[i]->m_battleground = NULL;
			if( !m_buffs[i]->IsInWorld() )
			{
				sGarbageCollection.AddObject( m_buffs[i] );
				m_buffs[i] = NULL;
			}
		}

		if(m_controlPoints[i] != NULL)
		{
			m_controlPoints[i]->m_battleground = NULL;
			if( !m_controlPoints[i]->IsInWorld() )
			{
				sGarbageCollection.AddObject( m_controlPoints[i] );
				m_controlPoints[i] = NULL;
			}
		}

		if(m_controlPointAuras[i] != NULL)
		{
			m_controlPointAuras[i]->m_battleground = NULL;
			if( !m_controlPointAuras[i]->IsInWorld() )
			{
				sGarbageCollection.AddObject( m_controlPointAuras[i] );
				m_controlPointAuras[i] = NULL;
			}
		}

		if(m_spiritGuides[i])
		{
			if( !m_spiritGuides[i]->IsInWorld() )
			{
				sGarbageCollection.AddObject( m_spiritGuides[i] );
				m_spiritGuides[i] = NULL;
			}
		}
	}

	for(list<GameObject*>::iterator itr = m_gates.begin(); itr != m_gates.end(); ++itr)
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

	m_resurrectMap.clear();
	//m_worldStates.clear();
}

void BattleForGilneas::EventUpdateResources(uint32 Team)
{
	//there is an exploit to not quit BG
	if( m_ended == true )
		return;
	uint32 resource_fields[2] = { BFG_ALLIANCE_RESOURCES, BFG_HORDE_RESOURCES };

	uint32 current_resources = m_resources[Team];
	uint32 current_bases = m_capturedBases[Team];

	if(current_bases>5)
		current_bases=5;

	// figure out how much resources we have to add to that team based on the number of captured bases.
	current_resources += (PointBonusPerUpdate[current_bases]);

	// did it change?
	if(current_resources == m_resources[Team])
		return;

	// check for overflow
	if(current_resources > RESOURCES_WINVAL)
		current_resources = RESOURCES_WINVAL;

	m_resources[Team] = current_resources;
	if((current_resources - m_lastRepGainResources[Team]) >= resourcesToGainBR)
	{
/*		m_mainLock.Acquire();
		for(InRangePlayerSet::iterator itr = m_players[Team].begin(); itr != m_players[Team].end(); ++itr)
		{
			uint32 fact = (*itr)->GetTeam() ? 510 : 509; //The Defilers : The League of Arathor
			(*itr)->ModStanding(fact, 10);
		}
		m_mainLock.Release(); */
		m_lastRepGainResources[Team] += resourcesToGainBR;
	}
	if((current_resources - m_lastHonorGainResources[Team]) >= resourcesToGainBH)
	{
		uint32 honorToAdd = m_honorPerKill;

		m_mainLock.Acquire();
		for(InRangePlayerSet::iterator itr = m_players[Team].begin(); itr != m_players[Team].end(); ++itr)
		{
			(*itr)->m_bgScore.BonusHonor += honorToAdd;
			HonorHandler::AddHonorPointsToPlayer((*itr), honorToAdd);
		}

		UpdatePvPData();
		m_mainLock.Release();
		m_lastHonorGainResources[Team] += resourcesToGainBH;
	}

	// update the world states
	SetWorldState(resource_fields[Team], current_resources);

	if(current_resources >= RESOURCES_WARNING_THRESHOLD && !m_nearingVictory[Team])
	{
		m_nearingVictory[Team] = true;
		SendChatMessage(Team ? CHAT_MSG_BG_EVENT_HORDE : CHAT_MSG_BG_EVENT_ALLIANCE, (uint64)0, "The %s has gathered %u resources and is nearing victory!", Team ? "Horde" : "Alliance", current_resources);
		uint32 sound = SOUND_ALLIANCE_BGALMOSTEND - Team;
		PlaySoundToAll(sound);
	}

	// check for winning condition
	if(current_resources == RESOURCES_WINVAL)
	{
		m_ended = true;
		m_winningteam = Team;
		EventOnEndedPreCleanup();
		m_nextPvPUpdateTime = 0;

		sEventMgr.RemoveEvents(this);
		sEventMgr.AddEvent(static_cast<CBattleground*>(this), &CBattleground::Close, EVENT_BATTLEGROUND_CLOSE, 120000, 1,EVENT_FLAG_DELETES_OBJECT);

		/* add the marks of honor to all players */
		SpellEntry * winner_spell = dbcSpell.LookupEntry(24953);
		SpellEntry * loser_spell = dbcSpell.LookupEntry(24952);
		uint32 lostHonorToAdd = m_honorPerKill;
		uint32 winHonorToAdd = 2 * lostHonorToAdd;
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
					(*itr)->CastSpell((*itr), winner_spell, true);
					if(i && (*itr)->GetQuestLogForEntry(11339))
						(*itr)->GetQuestLogForEntry(11339)->SendQuestComplete();
					else if((*itr)->GetQuestLogForEntry(11335))
						(*itr)->GetQuestLogForEntry(11335)->SendQuestComplete();
				}
				else
				{
					(*itr)->m_bgScore.BonusHonor += lostHonorToAdd;
					HonorHandler::AddHonorPointsToPlayer((*itr), lostHonorToAdd);
					(*itr)->CastSpell((*itr), loser_spell, true);
				}
			}
		}
		m_mainLock.Release();
		UpdatePvPData();
	}
}

void BattleForGilneas::HookOnPlayerDeath(Player * plr)
{
	// nothing in this BG
	plr->m_bgScore.Deaths++;
	UpdatePvPData();
}

void BattleForGilneas::HookOnMount(Player * plr)
{
	// nothing in this BG
}

void BattleForGilneas::HookOnPlayerKill(Player * plr, Player* pVictim)
{
	plr->m_bgScore.KillingBlows++;
	UpdatePvPData();
}

void BattleForGilneas::HookOnHK(Player * plr)
{
	plr->m_bgScore.HonorableKills++;
	UpdatePvPData();
}

void BattleForGilneas::OnAddPlayer(Player * plr)
{
	if(!m_started)
	{
		plr->CastSpell(plr, BG_PREPARATION, true);
		plr->m_bgScore.MiscData[BG_SCORE_BFG_BASES_ASSAULTED] = 0;
		plr->m_bgScore.MiscData[BG_SCORE_BFG_BASES_CAPTURED] = 0;
	}
	UpdatePvPData();
}

void BattleForGilneas::OnRemovePlayer(Player * plr)
{
	plr->RemoveAura(BG_PREPARATION,0,AURA_SEARCH_POSITIVE);
}

void BattleForGilneas::HookFlagDrop(Player * plr, GameObject * obj)
{
	// nothing?
}

void BattleForGilneas::HookFlagStand(Player * plr, GameObject * obj)
{
	// nothing?
}

LocationVector BattleForGilneas::GetStartingCoords(uint32 Team)
{
	if(Team)
		return LocationVector(1401.38000488f, 977.125f, 7.44215011597f, 0);
	else
		return LocationVector(908.273986816f, 1338.59997559f, 27.6448993683f, 0);
}

void BattleForGilneas::HookOnAreaTrigger(Player * plr, uint32 id)
{
	uint32 spellid=0;
	int32 buffslot = -1;
	switch(id)
	{
	case 6265:			
		buffslot=BFG_BUFF_1;
		break;
	case 6266:			
		buffslot=BFG_BUFF_2;
		break;
	case 6267:			
		buffslot=BFG_BUFF_3;
		break;
	case 6268:
		buffslot=BFG_BUFF_4;
		break;
	case 6269:
		buffslot=BFG_BUFF_5;
		break;

	default:
		Log.Error("BattleForGilneas", "Encountered unhandled areatrigger id %u", id);
		return;
		break;
	}

	if(plr->IsDead())		// dont apply to dead players... :P
		return;	

	uint32 x = (uint32)buffslot;
	if(m_buffs[x] && m_buffs[x]->IsInWorld())
	{
		// apply the spell
		spellid = m_buffs[x]->GetInfo()->sound3;
		m_buffs[x]->RemoveFromWorld(false);

		// respawn it in buffrespawntime
		sEventMgr.AddEvent(this,&BattleForGilneas::SpawnBuff,x,EVENT_BFG_RESPAWN_BUFF,BFG_BUFF_RESPAWN_TIME,1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);

		// cast the spell on the player
		SpellEntry * sp = dbcSpell.LookupEntryForced(spellid);
		if(sp)
		{
			Spell * pSpell = SpellPool.PooledNew( __FILE__, __LINE__ );
			pSpell->Init(plr, sp, true, NULL);
			SpellCastTargets targets(plr->GetGUID());
			pSpell->prepare(&targets);
		}
	}
}

bool BattleForGilneas::HookHandleRepop(Player * plr)
{
	/* our uber leet ab graveyard handler */
	LocationVector dest( NoBaseGYLocations[plr->m_bgTeam][0], NoBaseGYLocations[plr->m_bgTeam][1], NoBaseGYLocations[plr->m_bgTeam][2], 0.0f );
	float current_distance = 999999.0f;
	float dist;

	for(uint32 i = 0; i < BFG_NUM_CONTROL_POINTS; ++i)
	{
		if(m_basesOwnedBy[2] == (int32)plr->m_bgTeam)
		{
			dest.ChangeCoords(GraveyardLocations[2][0], GraveyardLocations[2][1], GraveyardLocations[2][2]);
		}
		else if(m_basesOwnedBy[i] == (int32)plr->m_bgTeam)
		{
			dist = plr->GetPositionV()->Distance2DSq(GraveyardLocations[i][0], GraveyardLocations[i][1]);
			if(dist < current_distance)
			{
				current_distance = dist;
				dest.ChangeCoords(GraveyardLocations[i][0], GraveyardLocations[i][1], GraveyardLocations[i][2]);
			}
		}
	}

	// port us there.
	plr->SafeTeleportDelayed(plr->GetMapId(),plr->GetInstanceID(),dest);
	return true;
}

void BattleForGilneas::CaptureControlPoint(uint32 Id, uint32 Team)
{
	if(m_basesOwnedBy[Id] != -1)
	{
		// there is a very slim chance of this happening, 2 teams evnets could clash..
		// just in case...
		return;
	}

	// anticheat, not really necessary because this is a server method but anyway
	if(m_basesAssaultedBy[Id] != (int32)Team)
		return;

	m_basesOwnedBy[Id] = Team;
	m_basesAssaultedBy[Id]=-1;
	m_basesLastOwnedBy[Id] = -1;

	// remove the other spirit guide (if it exists) // burlex: shouldnt' happen
	if(m_spiritGuides[Id] != NULL)
	{
		RemoveSpiritGuide(m_spiritGuides[Id]);
		m_spiritGuides[Id]->Despawn(0,0);
		m_spiritGuides[Id] = NULL;
	}

	// spawn the spirit guide for our faction
	m_spiritGuides[Id] = SpawnSpiritGuide(GraveyardLocations[Id][0], GraveyardLocations[Id][1], GraveyardLocations[Id][2], 0.0f, Team);
	AddSpiritGuide(m_spiritGuides[Id]);

	// send the chat message/sounds out
	PlaySoundToAll(Team ? SOUND_HORDE_CAPTURE : SOUND_ALLIANCE_CAPTURE);
	SendChatMessage(Team ? CHAT_MSG_BG_EVENT_HORDE : CHAT_MSG_BG_EVENT_ALLIANCE, 0, "The %s has taken the %s!", Team ? "Horde" : "Alliance", ControlPointNames[Id]);
	DefFlag[Id][0] = false;
	DefFlag[Id][1] = false;
	
	// update the overhead display on the clients (world states)
	m_capturedBases[Team]++;
	SetWorldState(Team ? BFG_HORDE_CAPTUREBASE : BFG_ALLIANCE_CAPTUREBASE, m_capturedBases[Team]);

	if(m_capturedBases[Team] >= 4)
	{
/*		m_mainLock.Acquire();
		for(InRangePlayerSet::iterator itr = m_players[Team].begin(); itr != m_players[Team].end(); ++itr)
		{
			if(Team)
			{
				if(m_capturedBases[Team] >= 4 && (*itr)->GetQuestLogForEntry(8121))
					(*itr)->GetQuestLogForEntry(8121)->SendQuestComplete();
				if(m_capturedBases[Team] == 5 && (*itr)->GetQuestLogForEntry(8122))
					(*itr)->GetQuestLogForEntry(8122)->SendQuestComplete();
			}
			else
			{
				if(m_capturedBases[Team] >= 4 && (*itr)->GetQuestLogForEntry(8114))
					(*itr)->GetQuestLogForEntry(8114)->SendQuestComplete();
				if(m_capturedBases[Team] == 5 && (*itr)->GetQuestLogForEntry(8115))
					(*itr)->GetQuestLogForEntry(8115)->SendQuestComplete();
			}
		}
		m_mainLock.Release(); */
	}

	// respawn the control point with the correct aura
	SpawnControlPoint(Id, Team ? BFG_SPAWN_TYPE_HORDE_CONTROLLED : BFG_SPAWN_TYPE_ALLIANCE_CONTROLLED);

	// update the map
	if( Id < BFG_NUM_CONTROL_POINTS && Team < 2 )
	{
		SetWorldState(AssaultFields[Id][Team], 0);
		SetWorldState(OwnedFields[Id][Team], 1);
	}

	// resource update event. :)
	sEventMgr.RemoveEvents( this, EVENT_BFG_RESOURCES_UPDATE_TEAM_0+Team );
	sEventMgr.AddEvent(this,&BattleForGilneas::EventUpdateResources, (uint32)Team, EVENT_BFG_RESOURCES_UPDATE_TEAM_0+Team, ResourceUpdateIntervals[m_capturedBases[Team]], 0, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
}

void BattleForGilneas::AssaultControlPoint(Player * pPlayer, uint32 Id)
{
#ifdef ANTI_CHEAT
	if(!m_started)
	{
		Anticheat_Log->writefromsession(pPlayer->GetSession(), "%s tryed to assault control point in arathi basin before battleground (ID %u) started.", pPlayer->GetName(), this->m_id);
		SendChatMessage(CHAT_MSG_BG_EVENT_NEUTRAL, pPlayer->GetGUID(), "%s will be removed from the game for cheating.", pPlayer->GetName());
		// Remove player from battleground.
		this->RemovePlayer(pPlayer, false);
		// Kick player from server.
		pPlayer->Kick(6000);
		return;
	}
#endif

	uint32 Team = pPlayer->m_bgTeam;
	uint32 Owner;

	pPlayer->m_bgScore.MiscData[BG_SCORE_BFG_BASES_ASSAULTED]++;

	if(m_basesOwnedBy[Id]==-1 && m_basesAssaultedBy[Id]==-1)
	{
		// omgwtfbbq our flag is a virgin?
		SetWorldState(NeutralFields[Id], 0);
	}

	if(m_basesOwnedBy[Id] != -1)
	{
		Owner = m_basesOwnedBy[Id];

		// set it to uncontrolled for now
		m_basesOwnedBy[Id] = -1;
		m_basesLastOwnedBy[Id] = Owner;

		// this control point just got taken over by someone! oh noes!
		if( m_spiritGuides[Id] != NULL )
		{
			map<Creature*,set<uint32> >::iterator itr = m_resurrectMap.find(m_spiritGuides[Id]);
			if( itr != m_resurrectMap.end() )
			{
				for( set<uint32>::iterator it2 = itr->second.begin(); it2 != itr->second.end(); ++it2 )
				{
					Player* r_plr = m_mapMgr->GetPlayer( *it2 );
					if( r_plr != NULL && r_plr->IsDead() )
						HookHandleRepop( r_plr );
				}
			}
			m_resurrectMap.erase( itr );
			m_spiritGuides[Id]->Despawn( 0, 0 );
			m_spiritGuides[Id] = NULL;
		}

		// detract one from the teams controlled points
		m_capturedBases[Owner] -= 1;
		SetWorldState(Owner ? BFG_HORDE_CAPTUREBASE : BFG_ALLIANCE_CAPTUREBASE, m_capturedBases[Owner]);

		// reset the world states
		if( Id < 5 && Team < 2 )
			SetWorldState(OwnedFields[Id][Owner], 0);

		// modify the resource update time period
		if(m_capturedBases[Owner]==0)
			this->event_RemoveEvents(EVENT_BFG_RESOURCES_UPDATE_TEAM_0+Owner);
		else
			this->event_ModifyRepeatInterval(EVENT_BFG_RESOURCES_UPDATE_TEAM_0 + Owner, ResourceUpdateIntervals[m_capturedBases[Owner]]);
	}

	// nigga stole my flag!
	if(m_basesAssaultedBy[Id] != -1)
	{
		Owner = m_basesAssaultedBy[Id];

		// woah! vehicle hijack!
		m_basesAssaultedBy[Id] = -1;
		SetWorldState(AssaultFields[Id][Owner], 0);

		// make sure the event does not trigger
		sEventMgr.RemoveEvents(this, EVENT_BFG_CAPTURE_CP_1 + Id);
		if (m_basesLastOwnedBy[Id] == (int32)Team)
		{
			m_basesAssaultedBy[Id] = (int32)Team;
			CaptureControlPoint(Id, Team);
			return;
		}

		// no need to remove the spawn, SpawnControlPoint will do this.
	}

	m_basesAssaultedBy[Id] = Team;

	// spawn the new control point gameobject
	SpawnControlPoint(Id, Team ? BFG_SPAWN_TYPE_HORDE_ASSAULT : BFG_SPAWN_TYPE_ALLIANCE_ASSAULT);

	// update the client's map with the new assaulting field
	if( Id < 5 && Team < 2 )
		SetWorldState(AssaultFields[Id][Team], 1);

	// create the 60 second event
	// Check Assault/Defense, the time of capture is not the same.
	if(DefFlag[Id][0] && !DefFlag[Id][1] )
	{
		DefFlag[Id][0] = false;
		SendChatMessage(Team ? CHAT_MSG_BG_EVENT_HORDE : CHAT_MSG_BG_EVENT_ALLIANCE, pPlayer->GetGUID(), "$N defend %s", ControlPointNames[Id]);
		sEventMgr.AddEvent(this, &BattleForGilneas::CaptureControlPoint, Id, Team, EVENT_BFG_CAPTURE_CP_1 + Id, 1000, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
		pPlayer->m_bgScore.MiscData[BG_SCORE_BFG_BASES_CAPTURED]++;
		UpdatePvPData();
	}
	else if(!DefFlag[Id][0] && !DefFlag[Id][1] )
	{
		DefFlag[Id][0] = true;
		SendChatMessage(Team ? CHAT_MSG_BG_EVENT_HORDE : CHAT_MSG_BG_EVENT_ALLIANCE, pPlayer->GetGUID(), "$N assault %s !", ControlPointNames[Id]);
		PlaySoundToAll(Team ? 8212 : 8174);
/*		if(Team)
		{
			QuestLogEntry *en = pPlayer->GetQuestLogForEntry(8120);
			switch(Id)
			{
				case BFG_CONTROL_POINT_MINE:
				{
					if(en && (int32)en->GetMobCount(0) < en->GetQuest()->required_mobcount[0])
					{
						en->SetMobCount(0, en->GetMobCount(0) + 1);
						en->SendUpdateAddKill(0);
						en->UpdatePlayerFields();
					}
				}break;
				case BFG_CONTROL_POINT_LUMBERMILL:
				{
					if(en && (int32)en->GetMobCount(1) < en->GetQuest()->required_mobcount[1])
					{
						en->SetMobCount(1, en->GetMobCount(1) + 1);
						en->SendUpdateAddKill(1);
						en->UpdatePlayerFields();
					}
				}break;
				case BFG_CONTROL_POINT_WATERWORKS:
				{
					if(en && (int32)en->GetMobCount(2) < en->GetQuest()->required_mobcount[2])
					{
						en->SetMobCount(2, en->GetMobCount(2) + 1);
						en->SendUpdateAddKill(2);
						en->UpdatePlayerFields();
					}
				}break;
				case BFG_CONTROL_POINT_LIGHTHOUSE:
				{
					if(en && (int32)en->GetMobCount(3) < en->GetQuest()->required_mobcount[3])
					{
						en->SetMobCount(3, en->GetMobCount(3) + 1);
						en->SendUpdateAddKill(3);
						en->UpdatePlayerFields();
					}
				}break;
			}
		}
		else
		{
			QuestLogEntry *en = pPlayer->GetQuestLogForEntry(8105);
			switch(Id)
			{
				case BFG_CONTROL_POINT_MINE:
				{
					if(en && (int32)en->GetMobCount(0) < en->GetQuest()->required_mobcount[0])
					{
						en->SetMobCount(0, en->GetMobCount(0) + 1);
						en->SendUpdateAddKill(0);
						en->UpdatePlayerFields();
					}
				}break;
				case BFG_CONTROL_POINT_LUMBERMILL:
				{
					if(en && (int32)en->GetMobCount(1) < en->GetQuest()->required_mobcount[1])
					{
						en->SetMobCount(1, en->GetMobCount(1) + 1);
						en->SendUpdateAddKill(1);
						en->UpdatePlayerFields();
					}
				}break;
				case BFG_CONTROL_POINT_WATERWORKS:
				{
					if(en && (int32)en->GetMobCount(2) < en->GetQuest()->required_mobcount[2])
					{
						en->SetMobCount(2, en->GetMobCount(2) + 1);
						en->SendUpdateAddKill(2);
						en->UpdatePlayerFields();
					}
				}break;
				case BFG_CONTROL_POINT_MINE:
				{
					if(en && (int32)en->GetMobCount(3) < en->GetQuest()->required_mobcount[3])
					{
						en->SetMobCount(3, en->GetMobCount(3) + 1);
						en->SendUpdateAddKill(3);
						en->UpdatePlayerFields();
					}
				}break;
			}
		} */
		sEventMgr.AddEvent(this, &BattleForGilneas::CaptureControlPoint, Id, Team, EVENT_BFG_CAPTURE_CP_1 + Id, 60000, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
		pPlayer->m_bgScore.MiscData[BG_SCORE_BFG_BASES_ASSAULTED]++;
		UpdatePvPData();
	}
	else
	{
		DefFlag[Id][0] = true;
		SendChatMessage(Team ? CHAT_MSG_BG_EVENT_HORDE : CHAT_MSG_BG_EVENT_ALLIANCE, pPlayer->GetGUID(), "$N claims the %s! If left unchallenged, the %s will control it in 1 minute!", ControlPointNames[Id], Team ? "Horde" : "Alliance");
		PlaySoundToAll(8192);
		sEventMgr.AddEvent(this, &BattleForGilneas::CaptureControlPoint, Id, Team, EVENT_BFG_CAPTURE_CP_1 + Id, 60000, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
	}
}

bool BattleForGilneas::HookSlowLockOpen(GameObject * pGo, Player * pPlayer, Spell * pSpell)
{
	// burlex todo: find a cleaner way to do this that doesnt waste memory.
	if(pGo->bannerslot >= 0 && pGo->bannerslot < BFG_NUM_CONTROL_POINTS)
	{
		//Stealthed / invisible players can't cap
		//if(pPlayer->GetStealthLevel() > 0 || pPlayer->HasAuraWithNameHash(SPELL_HASH_PROWL) || pPlayer->HasAuraWithNameHash(SPELL_HASH_SHADOWMELD))
		if(pPlayer->IsStealth() || pPlayer->IsInvisible() )
			return false;

		AssaultControlPoint(pPlayer,pGo->bannerslot);
		return true;
	}

	return false;
}

void BattleForGilneas::HookOnShadowSight() 
{
}
void BattleForGilneas::HookGenerateLoot(Player *plr, Object * pOCorpse)
{
}

void BattleForGilneas::HookOnUnitKill(Player * plr, Unit * pVictim)
{
}

void BattleForGilneas::HookOnFlagDrop(Player * plr)
{
}

void BattleForGilneas::SetIsWeekend(bool isweekend) 
{
	if (isweekend)
	{
		resourcesToGainBH = 200;
		resourcesToGainBR = 150;
	}
	else
	{
		resourcesToGainBH = 330;
		resourcesToGainBR = 200;
	}
}

