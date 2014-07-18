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

#ifndef __STDAFX_H
#define __STDAFX_H

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#define MAX_INT (1 << 31)

#define _GAME

#ifdef _DEBUG
//	#define _CRTDBG_MAP_ALLOC //eable this or add it as compile option for leak detection
#endif

#include "Definitions.h"

#include <cstring>
#include <cstdlib>
#include <list>
#include <vector>
#include <map>
#include <sstream>
#include <string>
//#include <fstream>
//safe way to handle arrays in debug mode
#include <array>

//needs to be before FeatureDefinitions.h to avoid replacing allocators 
#ifndef SCRIPTLIB
#include "../g3dlite/G3D/Vector3.h"
#include "../g3dlite/G3D/Matrix3.h"
#include "../Detour/DetourNavMesh.h"
#include "../Detour/DetourNavMeshQuery.h"
#include "../vmaplib/IVMapManager.h"
#include "../vmaplib/VMapFactory.h"
#endif

#define USE_STATIC_SET
class Object;
class Player;
#ifndef USE_STATIC_SET
	//this is working fine and tested
	#include <set>
	typedef std::set<Object *> InRangeSet;
	typedef std::set<Player *> InRangePlayerSet;
	typedef std::set<Object *> InRangeSetRecProt;
	typedef std::set<Player *> InRangePlayerSetRecProt;
#else
	#include "../arcemu-shared/SetRecursiveProtected.h"
	#include <set>
	//this is because a lot of the crashes are in iterator allocators
	typedef std::set<Object *>						InRangeSet;
	typedef std::set<Player *>						InRangePlayerSet;
	typedef RecursiveProtectedStaticSet<Object *>	InRangeSetRecProt;
	typedef RecursiveProtectedStaticSet<Player *>	InRangePlayerSetRecProt;
#endif

#define BUFFER_POOL_REMEMBER_HISTORY

#include "../arcemu-shared/Common.h"
#include "FeatureDefinitions.h"

//#define CAST_DEBUGGING
//this will be probably a 1 time usage thing to make sure we did not forcecast some pointer to invalid type
#if defined(CAST_DEBUGGING) || defined(_DEBUG)
/*	#define SafePlayerCast(x)		((x)->IsPlayer()?(static_cast<Player*>(x)):(NULL))
	#define SafePetCast(x)			((x)->IsPet()?(static_cast<Pet*>(x)):(NULL))
	#define SafeCreatureCast(x)		((x)->IsCreature()?(static_cast<Creature*>(x)):(NULL))
	#define SafeCorpseCast(x)		((x)->IsCorpse()?(static_cast<Corpse*>(x)):(NULL))
	#define SafeUnitCast(x)			((x)->IsUnit()?(static_cast<Unit*>(x)):(NULL))
	#define SafeItemCast(x)			((x)->IsItem()?(static_cast<Item*>(x)):(NULL))
	#define SafeContainerCast(x)	((x)->IsContainer()?(static_cast<Container*>(x)):(NULL))
	#define SafeGOCast(x)			((x)->IsGameObject()?(static_cast<GameObject*>(x)):(NULL))
	#define SafeDynOCast(x)			((x)->IsDynObj()?(static_cast<DynamicObject*>(x)):(NULL))
*/
	#define SafePlayerCast(x)		((x)->IsPlayer()?(dynamic_cast<Player*>(x)):(NULL))
	#define SafePetCast(x)			((x)->IsPet()?(dynamic_cast<Pet*>(x)):(NULL))
	#define SafeCreatureCast(x)		((x)->IsCreature()?(dynamic_cast<Creature*>(x)):(NULL))
	#define SafeCorpseCast(x)		((x)->IsCorpse()?(dynamic_cast<Corpse*>(x)):(NULL))
	#define SafeUnitCast(x)			((x)->IsUnit()?(dynamic_cast<Unit*>(x)):(NULL))
	#define SafeItemCast(x)			((x)->IsItem()?(dynamic_cast<Item*>(x)):(NULL))
	#define SafeContainerCast(x)	((x)->IsContainer()?(dynamic_cast<Container*>(x)):(NULL))
	#define SafeGOCast(x)			((x)->IsGameObject()?(dynamic_cast<GameObject*>(x)):(NULL))
	#define SafeDynOCast(x)			((x)->IsDynObj()?(dynamic_cast<DynamicObject*>(x)):(NULL))
//	#define SafeVehicleCast(x)		((x)->IsVehicle()?(static_cast<Vehicle*>(x)):(NULL))
	#define INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE ASSERT( m_Unit->IsUnit() );
	#define INSTRUMENT_TYPECAST_CHECK_CONTAINER_OBJECT_TYPE ASSERT( IsContainer() );
	#define INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE ASSERT( IsCreature() );
	#define INSTRUMENT_TYPECAST_CHECK_DYNO_OBJECT_TYPE ASSERT( IsDynObj() );
	#define INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE ASSERT( IsGameObject() );
	#define INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE ASSERT( IsItem() );
	#define INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE ASSERT( IsPet() );
	#define INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE ASSERT( IsPlayer() );
	#define INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE ASSERT( IsUnit() );
#else
	#define SafePlayerCast(x)		static_cast<Player*>(x)
	#define SafePetCast(x)			static_cast<Pet*>(x)
	#define SafeCreatureCast(x)		static_cast<Creature*>(x)
	#define SafeCorpseCast(x)		static_cast<Corpse*>(x)
	#define SafeUnitCast(x)			static_cast<Unit*>(x)
	#define SafeItemCast(x)			static_cast<Item*>(x)
	#define SafeContainerCast(x)	static_cast<Container*>(x)
	#define SafeGOCast(x)			static_cast<GameObject*>(x)
	#define SafeDynOCast(x)			static_cast<DynamicObject*>(x)
//	#define SafeVehicleCast(x)		static_cast<Vehicle*>(x)
	#define INSTRUMENT_TYPECAST_CHECK_AI_INTERFACE_OBJECT_TYPE 
	#define INSTRUMENT_TYPECAST_CHECK_CONTAINER_OBJECT_TYPE 
	#define INSTRUMENT_TYPECAST_CHECK_CREATURE_OBJECT_TYPE 
	#define INSTRUMENT_TYPECAST_CHECK_DYNO_OBJECT_TYPE 
	#define INSTRUMENT_TYPECAST_CHECK_GO_OBJECT_TYPE 
	#define INSTRUMENT_TYPECAST_CHECK_ITEM_OBJECT_TYPE 
	#define INSTRUMENT_TYPECAST_CHECK_PET_OBJECT_TYPE 
	#define INSTRUMENT_TYPECAST_CHECK_PLAYER_OBJECT_TYPE 
	#define INSTRUMENT_TYPECAST_CHECK_UNIT_OBJECT_TYPE 
#endif

//this was made to be able to monitor memory allocations by leak manager
static char * strdup_local(const char *src)
{
	if( src == NULL )
		return NULL;
	uint32 len = (uint32)strlen( src ) + 1;
	char *ret = (char*)malloc( len );
	memcpy( ret, src, len );
	return ret;
}

enum ObjectDeletedStates
{
	OBJ_AVAILABLE			= 0,
	OBJ_POOLED_FOR_DELETE	= 1,
	OBJ_DELETED				= 2,		//this cannot be tested. Memory is already dealocated
};

#include "../arcemu-shared/MemManager/GarbageCollector.h"
#include "../arcemu-shared/MersenneTwister.h"
#include "../arcemu-shared/WorldPacket.h"
#include "../arcemu-shared/Log.h"
#include "../arcemu-shared/NGLog.h"
#include "../arcemu-shared/ByteBuffer.h"
#include "../arcemu-shared/StackBuffer.h"
#include "../arcemu-shared/Config/ConfigEnv.h"
#include "../arcemu-shared/crc32.h"
#include "../arcemu-shared/LocationVector.h"
#include "../arcemu-shared/Timer.h"
#include "../arcemu-shared/CallStackMon.h"

#include <zlib.h>

#include "../arcemu-shared/Database/DatabaseEnv.h"
#include "../arcemu-shared/Database/DBCStores.h"
#include "../arcemu-shared/Database/dbcfile.h"

#include <Network/Network.h>

#include "../arcemu-shared/Auth/MD5.h"
#include "../arcemu-shared/Auth/BigNumber.h"
#include "../arcemu-shared/Auth/Sha1.h"
#include "../arcemu-shared/Auth/WowCrypt.h"
#include "../arcemu-shared/CrashHandler.h"
#include "../arcemu-shared/FastQueue.h"
#include "../arcemu-shared/CircularQueue.h"
#include "../arcemu-shared/Threading/RWLock.h"
#include "../arcemu-shared/Threading/Condition.h"
#include "../arcemu-shared/arcemu_getopt.h"

#include "UpdateFields.h"
#include "UpdateMask.h"
#include "Opcodes.h"

#ifdef CLUSTERING
	#include "../arcemu-realmserver/WorkerOpcodes.h"
#endif

#include "Packets.h"
#include "ExtensionNames.h"

#include "../arcemu-shared/CallBack.h"
#include "WordFilter.h"
#include "EventMgr.h"
#include "EventableObject.h"
#include "Object.h"
#include "LootMgr.h"
#include "Spell.h"
#include "Unit.h"

#include "SpellProcHandler.h"
#include "AddonMgr.h"
#include "AIInterface.h"
#include "AreaTrigger.h"
#include "BattlegroundMgr.h"
#include "CellHandler.h"
#include "Chat.h"
#include "Corpse.h"
#include "Quest.h"
#include "QuestMgr.h"
#include "Creature.h"
#include "DynamicObject.h"
#include "GameObject.h"
#include "Group.h"
#include "Guild.h"
#include "HonorHandler.h"
#include "ItemPrototype.h"
#include "Item.h"
#include "Container.h"
#include "AuctionHouse.h"
#include "AuctionMgr.h"
#include "LfgMgr.h"
#include "MailSystem.h"
#include "Map.h"
#include "MapCell.h"
#include "TerrainMgr2.h"
#include "MiscHandler.h"
#include "NameTables.h"
#include "NPCHandler.h"
#include "Pet.h"
#include "WorldSocket.h"
#include "WorldSession.h"
#include "MapScriptInterface.h"
#include "Player.h"
#include "MapMgr.h"
#include "faction.h"
#include "Skill.h"
//#include "SkillNameMgr.h"
#include "SpellNameHashes.h"
#include "SpellClassOverride.h"
#include "SpellCanCastOverride.h"
#include "SpellAuras.h"
#include "TaxiMgr.h"
#include "TransporterHandler.h"
#include "World.h"
#include "ItemInterface.h"
#include "Stats.h"
#include "Vehicle.h"
#include "WorldCreator.h"
#include "WeatherMgr.h"
#include "StrandOfTheAncient.h"
#include "AlteracValley.h"
#include "ArathiBasin.h"
#include "EyeOfTheStorm.h"
#include "WarsongGulch.h"
#include "BG_TwinPeaks.h"
#include "BG_BattleForGilneas.h"

#include "ObjectMgr.h"

#include "CThreads.h"
#include "ScriptMgr.h"

#include "Channel.h"
#include "ChannelMgr.h"
#include "ArenaTeam.h"
#include "Arenas.h"

#include "LogonCommClient.h"
#include "LogonCommHandler.h"
#include "MainServerDefines.h"
#include "WorldRunnable.h"

#include "../arcemu-shared/Storage.h"
#include "ObjectStorage.h"
#include "DatabaseCleaner.h"
#include "DayWatcherThread.h"
#include "VoiceChatHandler.h"
#include "LocalizationMgr.h"

#ifdef CLUSTERING
	#include "WorkerServerClient.h"
	#include "ClusterInterface.h"
#endif

#include "CollideInterface.h"

#include "Master.h"
#include "BaseConsole.h"
#include "CConsole.h"
//#define COLLECTION_OF_UNTESTED_STUFF_AND_TESTERS 1
#include "SpeedDetector.h"
#include "MultiBoxDetector.h"
//#define PVP_REALM_MEANS_CONSTANT_PVP
#include "BufferPools/BufferPools.h"
#include "HeightMapMgr.h"
#include "ProcCondHandler.h"
#include "AiAHealsupport.h"

#if (defined( WIN32 ) || defined( WIN64 ) )
	#define HACKY_CRASH_FIXES 1		// SEH stuff
#endif

#endif
