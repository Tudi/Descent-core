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

//
// Map.h
//

#ifndef __MAP_H
#define __MAP_H

class MapMgr;
class TemplateMgr;
struct MapInfo;
class TerrainMgr;
class CreatureProto;

#define WMO_VMAP_VALUE_NOT_INITIALIZED	( 0x00 )

struct Formation;

class CreatureSpawn
{
public:
	CreatureSpawn() { form = NULL; phase = 1;}
	uint32	id;//spawn ID
	uint32	entry;
//	CreatureProto	*proto;		//in case we need some info ?
	float	x;
	float	y;
	float	z;
	float	o;
	Formation* form;
	uint8 movetype;
	uint32 displayid;
	uint32 factionid;
	uint32 flags;
	uint32 bytes0;
	uint32 bytes1;
	uint32 bytes2;
	uint32 emote_state;
	//uint32 respawnNpcLink;
//	uint16 channel_spell;
//	uint32 channel_target_go;
//	uint32 channel_target_creature;
	uint16 stand_state;
	uint32 MountedDisplayID;
	uint32 Item1SlotDisplay;
	uint32 Item2SlotDisplay;
	uint32 Item3SlotDisplay;
	uint32 difficulty_spawnmask;	//instance difficulty will tell if this mob is spawned or not
	uint32 phase;
};

class GOSpawn
{
public:
	GOSpawn() { phase = 1; }
	uint32	id;//spawn ID
	uint32	entry;
	float	x;
	float	y;
	float	z;
	float	o;
	float	parent_rot1;	//not sure this is worth keeping considering we are not using parnet GO's
	float	parent_rot2;	//not sure this is worth keeping considering we are not using parnet GO's
	float	parent_rot3;	//not sure this is worth keeping considering we are not using parnet GO's
	float	parent_rot4;	//not sure this is worth keeping considering we are not using parnet GO's
	uint32	flags;
	uint32	state;
	uint32	faction;
	float	scale;
	uint32	phase;	//mask in whitch phases this mob is visible
	uint64  precise_facing;	//this is some weird format used in A9. Can be generated from orientation. But it is better if we have exact value from trusted sources
};

typedef std::vector<CreatureSpawn*> CreatureSpawnList;
typedef std::vector<GOSpawn*> GOSpawnList;
typedef SimpleVect<CreatureSpawn*> CreatureSpawnVect;
typedef SimpleVect<GOSpawn*> GOSpawnVect;

typedef struct
{
	CreatureSpawnList	CreatureSpawns;
	GOSpawnList			GOSpawns;
}CellSpawns;

typedef struct
{
	CreatureSpawnVect	CreatureSpawns;
	GOSpawnVect			GOSpawns;
}CellSpawnsVect;

class SERVER_DECL Map
{
public:
	Map(uint32 mapid, MapInfo * inf);
	~Map();

	ARCEMU_INLINE string GetNameString() { return name; }
	ARCEMU_INLINE const char* GetName() { return name.c_str(); }
	ARCEMU_INLINE MapEntry* GetDBCEntry() { return me; }

	ARCEMU_INLINE CellSpawnsVect *GetSpawnsList(uint32 cellx,uint32 celly)
	{
		ASSERT(cellx < _sizeX);
		ASSERT(celly < _sizeY);
		if(spawns[cellx]==NULL) return NULL;

		return spawns[cellx][celly];
	}
	ARCEMU_INLINE CellSpawnsVect * GetSpawnsListAndCreate(uint32 cellx, uint32 celly)
	{
		ASSERT(cellx < _sizeX);
		ASSERT(celly < _sizeY);
		if(spawns[cellx]==NULL)
		{
			spawns[cellx] = new CellSpawnsVect*[_sizeY+1];
			memset(spawns[cellx],0,sizeof(CellSpawnsVect*)*(_sizeY+1));
		}

		if(spawns[cellx][celly] == 0)
			spawns[cellx][celly] = new CellSpawnsVect;
		return spawns[cellx][celly];
	}

	void LoadSpawns(bool reload);//set to true to make clean up
	uint32 CreatureSpawnCount;
	uint32 GameObjectSpawnCount;

private:
	MapInfo *	   _mapInfo;
	uint32 _mapId;
	string name;
	MapEntry * me;

	//new stuff
	CellSpawnsVect **spawns[_sizeX+1];
public:
	CellSpawns staticSpawns;
};

#endif
