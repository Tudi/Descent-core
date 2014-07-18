#ifndef _FEATURE_DEFINITIONS_H_
#define _FEATURE_DEFINITIONS_H_

#ifdef _DEBUG
	#ifdef _CRTDBG_MAP_ALLOC
//		#define _CRTDBG_MAP_ALLOC_TEST_MEM		//some temp mem checking
		//use these only if you are not using external memory debuggers
		#ifndef SCRIPTLIB
			#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
			#define new DEBUG_NEW
		#endif
	#endif
	#if !defined(_CRTDBG_MAP_ALLOC) && !defined(WIN64)  && !defined(X64)
		#define USE_INTERNAL_MEM_LEAK_TRACKER
		#if !defined(SCRIPTLIB) && !defined( ARCEMU_SHARED_LIB )
			#include "mmgr.h"
		#else
//			#include "../arcemu-shared/MemManager/mmgr.h"
		#endif
	#endif
#else
#endif

#define PRINTMEMUSAGEONLOAD
#define DISABLE_COLLISION_LOAD	1	//has a lot of mem leaks :)
#define DISABLE_GUID_RECYCLING	1	//this will leak memory like hell. BUT i needed it to debug some guid collision issue

#ifdef PRINTMEMUSAGEONLOAD
	#define	printmem(x,y) sLog.outDebug(x,y);
#else
	#define	printmem(x,y) //
#endif

#define FORCED_GM_TRAINEE_MODE				1 //this means he cannot switch off GM mode. He has fixed DMG and health. Forced faction. No spell casts
#define FORCED_GM_BAN_LIMITATION			2*7*24*60*60 //given in seconds. Noob GMs are not supposed to give bigger bans then this. This is to avoid GM revenges
//#define MINIMIZE_NETWORK_TRAFIC			//this is used for very high player concentrations to send spell packets only to duelling players
//#define GENERATE_CLASS_BALANCING_STATISTICS	1	//used to bust urban legends of QQ players that say that some class is uber imba due to emu bugs
//! you need to search in files and additionall enable this !
//#define USE_OLD_EVENT_MANAGER			//the new event manager is using pools for the event lists. Stats say alloc and dealoc eats up 2 GB RAM / 5 minutes

//put 0 to disable this feature, else put a few minute size in milliseconds to monitor when GM goes AFK
#define GM_STATISTICS_UPDATE_INTERVAL		(1*60*1000)

#define USE_PACKET_COMPRESSION				0		//counted in milliseconds. undefine to disable this feature. This feature saves 7%-14% bandwidth avg on blizz servers
#define ENABLE_NAGLE_ALGORITHM				0		//nagle can reduce huge overhead for some packets. Might also create a lot of lag in case he has nothing else to group up packets with
#define CLIENT_TIMESPEED_CHEAT_TRIGGER_DC	3		//if ping packets will come with a higher rate then normal then there is a chance that client is using timespeed cheat ?

#define CATACLYSM_SUPPORT					1		//disable this in case i forget about it. This will be used to force emu to try to support basic cataclysm support

//#define USE_HACK_TO_ENABLE_ACHIEVEMENTS		1
#define FORCE_DISABLE_CREATURE_PROTO_PERIODIC_AURAS 1//because i met the case when some fire was casting every few seconds a spell. And there were lots of fires...= doomed
#define SOCIAL_BONUSES_ADDON				1		// when people chat a lot, or get in places with lots of places, they receive buffs. The idea is to award people that socialize a lot
#define SOCIAL_BONUS_DROPRATE_CHANCE_PER_5_MAN	5	//for every 5 man drop rate chance increases by N%

//FAILURE !! Do not ever use this :P
//#define SMALL_ITEM_OBJECT					1		//there are a few million item objects cached. Even if we save a few bytes it might have a considerable impact on mem usage

//#define USE_MULTIBOXING_DETECTOR					// check if multiple players are present on same map.
													// can trigger falsely easelly. Some cities connect with the same IP

#define ENABLE_STATISTICS_GENERATED_HEIGHTMAP		// this height map includes the height of anything players can step on, trees, rocks, fences...
#define JayZMoveHandler								// really nice, but only he can update it in the future
#endif

#define DISABLE_CLIENT_GM_SHOW						// this is required since client is expecting you to have special GM addon...
//#define BATTLEGRUND_REALM_BUILD						// some special functionalities for PVP only realm. Most probably you want this to be disabled
//#define USE_SPELL_MISSILE_DODGE		
#define CLEAR_DUEL_COOLDOWNS						// non blizzlike, record spells casted while duelling and clear cooldowns after duel ends

#define SPECIFIC_14333_CLIENT_FIXES					// like spell power coeffs

//#define USE_MERSENE_TWISTER_RANDOM_GEN				// good old trusted multi threaded implementation of a random number generator
#define USE_STATIC_FILE_RANDOM_GEN					// good enough random number generator. Was 13.5 times faster than mersene
#define GM_COMMAND_TO_LOG_PLAYER_INCOMMING_PACKETS	//had to add this one because of a hacker that kept crashing client of other players
//#define ANYONE_CAN_USE_ALL_COMMANDS					// used only to build for some nubs server so they can experimetn on it
//#define PVP_AREANA_REALM							// special realm build. You can only play arena games here
#define TRINITY_LOGONSERVER_COMPATIBLE				// trinity server can handle authentification as long as table "account" is in character DB and worldserver can get the required info