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

#ifndef WOWSERVER_DEFINITION_H
#define WOWSERVER_DEFINITION_H

#define NUM_MAPS (980+100)

#define MAP_MGR_UPDATE_PERIOD 100

//instance refers to both dungeon and raid !
enum INSTANCE_MODE
{
/*    MODE_NORMAL = 0,
    MODE_HEROIC = 1,
    MODE_EPIC   = 2,*/
	//maybe use flags ?
	INSTANCE_MODE_DUNGEON_NORMAL			= 0,	// 5 man
	INSTANCE_MODE_DUNGEON_HEROIC			= 1,	// 5 man
	INSTANCE_MODE_RAID_10_MAN				= 2,	// 10 man
	INSTANCE_MODE_RAID_25_MAN				= 3,	// 25 man	- in some packets this is first and not the 10 man version
	INSTANCE_MODE_RAID_10_MAN_HEROIC		= 4,	// 10 man
	INSTANCE_MODE_RAID_25_MAN_HEROIC		= 5,	// 25 man
	//pfff crap, there are 20 man modes also but actually merged with one of the below modes
	NUM_INSTANCE_MODES						= 6
};
//need to match above. Do not change unless you also update DB !
/*enum INSTANCE_TYPE
{
	INSTANCE_NULL,
	INSTANCE_RAID,
	INSTANCE_DUNGEON,
	INSTANCE_PVP,
	INSTANCE_MULTIMODE,
};*/
enum INSTANCE_TYPE_FLAGS
{
	INSTANCE_FLAG_STATIC_MAP			= 1,		//cannot combine with instance flags !
	INSTANCE_FLAG_PVP					= 2,
	INSTANCE_FLAG_DUNGEON_5_MEN			= 4,
	INSTANCE_FLAG_DUNGEON_5_MEN_HEROIC	= 8,
	INSTANCE_FLAG_RAID_10_MEN			= 16,
	INSTANCE_FLAG_RAID_25_MEN			= 32,
	INSTANCE_FLAG_RAID_10_MEN_HEROIC	= 64,
	INSTANCE_FLAG_RAID_25_MEN_HEROIC	= 128,
	INSTANCE_FLAG_DISABLE_FLIGHT		= 256,
};

#define IS_MAP_RAID(m) ((m->type_flags & (INSTANCE_FLAG_RAID_10_MEN|INSTANCE_FLAG_RAID_10_MEN_HEROIC|INSTANCE_FLAG_RAID_25_MEN|INSTANCE_FLAG_RAID_25_MEN_HEROIC))!=0)
#define IS_MAP_DUNGEON(m) ((m->type_flags & (INSTANCE_FLAG_DUNGEON_5_MEN|INSTANCE_FLAG_DUNGEON_5_MEN_HEROIC))!=0)
//pvp maps are also instances ?
#define IS_MAP_INSTANCE(m) ((m->type_flags & (INSTANCE_FLAG_PVP|INSTANCE_FLAG_DUNGEON_5_MEN|INSTANCE_FLAG_DUNGEON_5_MEN_HEROIC|INSTANCE_FLAG_RAID_10_MEN|INSTANCE_FLAG_RAID_10_MEN_HEROIC|INSTANCE_FLAG_RAID_25_MEN|INSTANCE_FLAG_RAID_25_MEN_HEROIC))!=0)

#define IS_HEROIC_INSTANCE_DIFFICULTIE( i ) ( i == INSTANCE_MODE_DUNGEON_HEROIC || i == INSTANCE_MODE_RAID_10_MAN_HEROIC || i == INSTANCE_MODE_RAID_25_MAN_HEROIC )
#define IS_NORMAL_INSTANCE_DIFFICULTIE( i ) ( i == INSTANCE_MODE_DUNGEON_NORMAL || i == INSTANCE_MODE_RAID_10_MAN || i == INSTANCE_MODE_RAID_25_MAN )
#define IS_RAID_INSTANCE_DIFFICULTIE( i ) ( i == INSTANCE_MODE_RAID_10_MAN || i == INSTANCE_MODE_RAID_10_MAN_HEROIC || i == INSTANCE_MODE_RAID_25_MAN || i == INSTANCE_MODE_RAID_25_MAN_HEROIC )

inline const char * GetDifficultyString(int difficulty)
{
	switch(difficulty)
	{
	case INSTANCE_MODE_DUNGEON_NORMAL:
		return "5 man normal";
	case INSTANCE_MODE_DUNGEON_HEROIC:
		return "5 man heroic";
	case INSTANCE_MODE_RAID_10_MAN:
		return "10 man normal";
	case INSTANCE_MODE_RAID_10_MAN_HEROIC:
		return "10 man heroic";
	case INSTANCE_MODE_RAID_25_MAN:
		return "25 man normal";
	case INSTANCE_MODE_RAID_25_MAN_HEROIC:
		return "25 man heroic";
//	case MODE_EPIC:
//		return "epic";
	default:
		return "unknown";
	}
}

enum DungeonDifficulties
{
	DUNGEON_DIFFICULTY_NORMAL = 0,
	DUNGEON_DIFFICULTY_HEROIC = 1,
	TOTAL_DUNGEON_DIFFICULTIES
};

enum RaidDifficulties
{
	RAID_DIFFICULTY_10MAN_NORMAL = 0,
	RAID_DIFFICULTY_10MAN_HEROIC = 1,
	RAID_DIFFICULTY_25MAN_NORMAL = 2,
	RAID_DIFFICULTY_25MAN_HEROIC = 3,
	TOTAL_RAID_DIFFICULTIES
};

#endif
