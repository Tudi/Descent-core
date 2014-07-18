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

#include "StdAfx.h"
initialiseSingleton( World );

DayWatcherThread* dw = NULL;

World::World()
{
	m_playerLimit = 0;
	m_allowMovement = true;
	m_gmTicketSystem = true;

	GmClientChannel = "";

	m_StartTime = 0;
	eventholder = new EventableObjectHolder(-1);
	m_holder = eventholder;

	mQueueUpdateInterval = 10000;
	PeakSessionCount = 0;
	mInWorldPlayerCount = 0;
	mAcceptedConnections = 0;
	HordePlayers = 0;
	AlliancePlayers = 0;
	gm_skip_attunement = false;
	show_gm_in_who_list = true;
	interfaction_chat = false;
	interfaction_group = false;
	interfaction_guild = false;
	interfaction_trade = false;
	interfaction_friend = false;
	interfaction_misc = false;
	crossover_chars = true;
	gamemaster_listOnlyActiveGMs = false;
	gamemaster_hidePermissions = false;
	GMAdminTag = true;
	NameinAnnounce = false;
	NameinWAnnounce = false;
	announce_output = true;
	map_unload_time=0;
	antiMasterLootNinja = false;
#ifndef CLUSTERING
	SocketSendBufSize = WORLDSOCKET_SENDBUF_SIZE;
	SocketRecvBufSize = WORLDSOCKET_RECVBUF_SIZE;
#endif
	m_levelCap=PLAYER_LEVEL_CAP;
	m_genLevelCap=PLAYER_LEVEL_CAP;
	start_level=1;
	m_limitedNames=false;
	m_banTable = NULL;
#ifdef DEBUG_SPELL_CASTS
	memset( casted_spells, 0, sizeof( uint8 ) * MAX_USABLE_SPELL_ID );
	memset( last_N_spells, 0, sizeof( uint8 ) * LOG_LAST_N_SPELLS );
	next_dump = getMSTime();
	last_N_spells_index = NULL;
	filename_index = next_dump;
	char fname[100];
	sprintf(fname,"last_n_spells_%u.txt",filename_index);
	last_N_spells_FILE = fopen(fname,"w");
#endif
	Global_Pet_id = GetTickCount();		//nevah to have 0 !
}

void CleanupRandomNumberGenerators();
void World::LogoutPlayers()
{
	Log.Notice("World", "Logging out players...");
	for(SessionMap::iterator i=m_sessions.begin();i!=m_sessions.end();i++)
	{
		(i->second)->LogoutPlayer(true);
	}

	Log.Notice("World", "Deleting sessions...");
	WorldSession * p;
	for(SessionMap::iterator i=m_sessions.begin();i!=m_sessions.end();)
	{
		p = i->second;
		++i;

		DeleteSession(p);
		//delete p;
	}
}

World::~World()
{
	Log.Notice("LocalizationMgr", "~LocalizationMgr()");
	sLocalizationMgr.Shutdown();

	Log.Notice("WorldLog", "~WorldLog()");
	delete WorldLog::getSingletonPtr();

	Log.Notice("ObjectMgr", "~ObjectMgr()");
	delete ObjectMgr::getSingletonPtr();
	
	Log.Notice("LootMgr", "~LootMgr()");
	delete LootMgr::getSingletonPtr();
	
	Log.Notice("LfgMgr", "~LfgMgr()");
	delete LfgMgr::getSingletonPtr();

	Log.Notice("ChannelMgr", "~ChannelMgr()");
	delete ChannelMgr::getSingletonPtr();

	Log.Notice("QuestMgr", "~QuestMgr()");
	delete QuestMgr::getSingletonPtr();
  
	Log.Notice("WeatherMgr", "~WeatherMgr()");
	delete WeatherMgr::getSingletonPtr();

	Log.Notice("TaxiMgr", "~TaxiMgr()");
	delete TaxiMgr::getSingletonPtr();
	
	Log.Notice("BattlegroundMgr", "~BattlegroundMgr()");
	delete CBattlegroundManager::getSingletonPtr();

	Log.Notice("InstanceMgr", "~InstanceMgr()");
	sInstanceMgr.Shutdown();

#ifdef ENABLE_STATISTICS_GENERATED_HEIGHTMAP
	Log.Notice("SVMapMgr", "~SVMapMgr()");
	delete VMapMgr::getSingletonPtr();
#endif
	Log.Notice("GarbageCollector", "~DelayedObjectDeletor()");
	delete DelayedObjectDeletor::getSingletonPtr();

	Log.Notice("TerrainMgr2", "~TerrainMgr2()");
	delete TerrainMgr2::getSingletonPtr();

	//sLog.outString("Deleting Thread Manager..");
	//delete ThreadMgr::getSingletonPtr();
	Log.Notice("WordFilter", "~WordFilter()");
	delete g_chatFilter;
	g_chatFilter = NULL;
	delete g_characterNameFilter;
	g_characterNameFilter = NULL;

	Log.Notice("Rnd", "~Rnd()");
	CleanupRandomNumberGenerators();

	for( AreaTriggerMap::iterator i = m_AreaTrigger.begin( ); i != m_AreaTrigger.end( ); ++ i ) 
	{
		delete i->second;
		i->second = NULL;
	}
	m_AreaTrigger.clear();

	//eventholder = 0;
	delete eventholder;
	eventholder = NULL;

	Storage_Cleanup();
/*	for(list<SpellEntry*>::iterator itr = dummyspells.begin(); itr != dummyspells.end(); ++itr)
	{
		delete *itr;
		*itr = NULL;
	}
	dummyspells.clear();*/
}


WorldSession* World::FindSession(uint32 id)
{
	m_sessionlock.AcquireReadLock();
	WorldSession * ret = 0;
	SessionMap::const_iterator itr = m_sessions.find(id);

	if(itr != m_sessions.end())
		ret = itr->second;
	
	m_sessionlock.ReleaseReadLock();

	return ret;
}

void World::RemoveSession(uint32 id)
{
	m_sessionlock.AcquireWriteLock();

	SessionMap::iterator itr = m_sessions.find(id);

	if(itr != m_sessions.end())
	{
		itr->second->Delete( false );
		m_sessions.erase(itr);
	}

	m_sessionlock.ReleaseWriteLock();
}

void World::AddSession(WorldSession* s)
{
	if(!s)
		return;

	m_sessionlock.AcquireWriteLock();

	ASSERT(s);
	m_sessions[s->GetAccountId()] = s;

	if(m_sessions.size() >  PeakSessionCount)
		PeakSessionCount = (uint32)m_sessions.size();

	s->SendAccountDataTimes(GLOBAL_CACHE_MASK);

	m_sessionlock.ReleaseWriteLock();
}

void World::AddGlobalSession(WorldSession *session)
{
	if(!session)
		return;

	SessionsMutex.Acquire();
	Sessions.insert(session);
	SessionsMutex.Release();
}

void World::RemoveGlobalSession(WorldSession *session)
{
	SessionsMutex.Acquire();
	Sessions.erase(session);
	SessionsMutex.Release();
}

bool BasicTaskExecutor::run()
{
	/* Set thread priority, this is a bitch for multiplatform :P */
#if (defined( WIN32 ) || defined( WIN64 ) )
	switch(priority)
	{
		case BTE_PRIORITY_LOW:
			::SetThreadPriority( ::GetCurrentThread(), THREAD_PRIORITY_LOWEST );
			break;

		case BTW_PRIORITY_HIGH:
			::SetThreadPriority( ::GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL );
			break;

		default:		// BTW_PRIORITY_MED
			::SetThreadPriority( ::GetCurrentThread(), THREAD_PRIORITY_NORMAL );
			break;
	}
#else
	struct sched_param param;
	switch(priority)
	{
	case BTE_PRIORITY_LOW:
		param.sched_priority = 0;
		break;

	case BTW_PRIORITY_HIGH:
		param.sched_priority = 10;
		break;

	default:		// BTW_PRIORITY_MED
		param.sched_priority = 5;
		break;
	}
	pthread_setschedparam(pthread_self(), SCHED_OTHER, &param);
#endif

	// Execute the task in our new context.
	cb->execute();
#if (defined( WIN32 ) || defined( WIN64 ) )
	::SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
#else
	param.sched_priority = 5;
	pthread_setschedparam(pthread_self(), SCHED_OTHER, &param);
#endif

	return true;
}

void Apply112SpellFixes();
void ApplyNormalFixes();
void LoadSpellForcedTargetting();
void ApplyItemEnchantFixes();
void ApplyAreaFixes();

void GenerateBoosetedAchievementLookupTable()
{
    for (uint32 i=0; i<dbcAchievementCriteriaStore.GetNumRows(); i++)
    {
		AchievementCriteriaEntry *criteria = dbcAchievementCriteriaStore.LookupRow(i);
		if( !criteria ) continue; //nope, this will never happen ;)
		if( criteria->requiredType >= ACHIEVEMENT_CRITERIA_TYPE_TOTAL )
		{
			sLog.outDebug("On generating lookuptable for criterias. We found a type greater then the ones we known before : %u ",criteria->requiredType);
			continue;
		}
		dbcAchievementCriteriaTypeLookup[ criteria->requiredType ].push_back( criteria );
	}
	//no idea how to get required instance difficulty mode from DBC atm
	std::list<AchievementCriteriaEntry*>::iterator itr;
    for (itr = dbcAchievementCriteriaTypeLookup[ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE].begin(); itr != dbcAchievementCriteriaTypeLookup[ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE].end(); itr++)
    {
		uint32 achientry = (*itr)->ID;
		AchievementEntry *achi = dbcAchievementStore.LookupEntry( (*itr)->referredAchievement );
		if( !achi )
			continue;
		if( strstr( achi->name,"Heroic" ) || strstr( achi->name,"heroic" ) )
			(*itr)->requires_heroic_instance = 1;
		if( strstr( achi->name,"player" ) || strstr( achi->name,"Player" ) )
		{
			if( strstr( achi->name,"5" ) )
				(*itr)->requires_player_count = 5;
			else if( strstr( achi->name,"10" ) )
				(*itr)->requires_player_count = 10;
			else if( strstr( achi->name,"25" ) )
				(*itr)->requires_player_count = 25;
		}
	}
    for (itr = dbcAchievementCriteriaTypeLookup[ACHIEVEMENT_CRITERIA_TYPE_WIN_ARENA].begin(); itr != dbcAchievementCriteriaTypeLookup[ACHIEVEMENT_CRITERIA_TYPE_WIN_ARENA].end(); itr++)
    {
		uint32 achientry = (*itr)->ID;
		AchievementEntry *achi = dbcAchievementStore.LookupEntry( (*itr)->referredAchievement );
		if( !achi )
			continue;
		if( strstr( achi->name,"2v2" ) )
			(*itr)->requires_arenta_team_type_check = ARENA_TEAM_TYPE_2V2;
		else if( strstr( achi->name,"3v3" ) )
			(*itr)->requires_arenta_team_type_check = ARENA_TEAM_TYPE_3V3;
		else if( strstr( achi->name,"5v5" ) )
			(*itr)->requires_arenta_team_type_check = ARENA_TEAM_TYPE_5V5;
	}
}

bool World::SetInitialWorldSettings()
{
	Log.Line();
//	Player::InitVisibleUpdateBits();
	GenerateStaticUpdateMasks();

	CharacterDatabase.WaitExecute("UPDATE characters SET online = 0 WHERE online = 1");
	//CharacterDatabase.WaitExecute("UPDATE characters SET level = 70 WHERE level > 70");
	CharacterDatabase.WaitExecute("UPDATE characters SET banned=0,banReason='' WHERE banned > 100 AND banned < %u", UNIXTIME);
	//cleanup DB cause this seems to bug out client
	CharacterDatabase.Execute("DELETE FROM social_friends WHERE character_guid not in ( select GUID from characters )");
	CharacterDatabase.Execute("DELETE FROM social_friends WHERE friend_guid not in ( select GUID from characters )");
	CharacterDatabase.Execute("DELETE FROM social_ignores WHERE character_guid not in ( select GUID from characters )");
	CharacterDatabase.Execute("DELETE FROM social_ignores WHERE ignore_guid not in ( select GUID from characters )");

	m_lastTick = UNIXTIME;

	// TODO: clean this
	time_t tiempo;
	char hour[3];
	char minute[3];
	char second[3];
	struct tm *tmPtr;
	tiempo = UNIXTIME;
	tmPtr = localtime(&tiempo);
	strftime( hour, 3, "%H", tmPtr );
	strftime( minute, 3, "%M", tmPtr );
	strftime( second, 3, "%S", tmPtr );
	m_gameTime = (3600*atoi(hour))+(atoi(minute)*60)+(atoi(second)); // server starts at noon

	// Start

	uint32 start_time = getMSTime();
	printmem("mem usage : %f\n",GetMemUsage() );
	Log.Notice( "World", "Loading DBC files..." );
	if( !LoadDBCs() )
	{
		Log.LargeErrorMessage(LARGERRORMESSAGE_ERROR, "One or more of the DBC files are missing.", "These are absolutely necessary for the server to function.", "The server will not start without them.", NULL);
		return false;
	}
	printmem("mem usage : %f\n",GetMemUsage() );
	Log.Notice( "World", "Generting achievemnt lookup boost..." );
	GenerateBoosetedAchievementLookupTable();
	printmem("mem usage : %f\n",GetMemUsage() );

	new ObjectMgr;
	new QuestMgr;
	new LootMgr;
	new LfgMgr;
	new WeatherMgr;
	new TaxiMgr;
	new AddonMgr;
	new WorldLog;
	new ChatHandler;
#ifdef ENABLE_STATISTICS_GENERATED_HEIGHTMAP
	new VMapMgr;
#endif
	new DelayedObjectDeletor;
	new TerrainMgr2;

	// grep: this only has to be done once between version updates
	// to re-fill the table.

	/*sLog.outString("Filling spell replacements table...");
	FillSpellReplacementsTable();
	sLog.outString("");*/

	Apply112SpellFixes();
	ApplyNormalFixes();
	LoadSpellForcedTargetting();
	RegisterProcHandlers();			//needs to be after loading spells
	RegisterChargeHandlers();		//needs to be after loading spells
	RegisterCanCastOverrides();		//needs to be after loading spells
	RegisterEffectOverrides();		//needs to be after loading spells
	RegisterTargetingOverride();	//needs to be after loading spells
	RegisterAuraHooks();			//needs to be after loading spells
	RegisterAuraInterruptHandlers();//needs to be after loading spells
	RegisterAuraPeriodicDummyTickHandlers();//needs to be after loading spells
	RegisterCritChanceHandlers();	//needs to be after loading spells
	RegisterEffectRedirectHandlers();	//needs to be after loading spells
	RegisterDefaultClassOverrides();	//needs to be after loading spells
	RegisterAchivementCriteriaConditionHandlers();	//needs to be after loading achievement criteria DBC

#define MAKE_TASK(sp, ptr) tl.AddTask(new Task(new CallbackP0<sp>(sp::getSingletonPtr(), &sp::ptr)))
	// Fill the task list with jobs to do.
	TaskList tl;
	printmem("mem usage : %f\n",GetMemUsage() );
	Storage_FillTaskList(tl);

	// spawn worker threads (2 * number of cpus)
	tl.spawn();

	/* storage stuff has to be loaded first */
	tl.wait();
	printmem("mem usage : %f\n",GetMemUsage() );
	Storage_LoadAdditionalTables();
	printmem("mem usage : %f\n",GetMemUsage() );

	MAKE_TASK(ObjectMgr, LoadPlayerCreateInfo);
	MAKE_TASK(ObjectMgr, LoadPlayersInfo);
	MAKE_TASK(ObjectMgr, LoadIPMute);

	tl.wait();
	printmem("mem usage : %f\n",GetMemUsage() );

	//needs to be after load creature proto and before load extra creature proto
	ObjectMgr::getSingleton().LoadReferenceLoot();	//not threaded cause we need it for the other loots
	ObjectMgr::getSingleton().ProcessReferenceLootTable();	//linking lists to lists
	MAKE_TASK(ObjectMgr, LoadLootsCreatureKill);
	MAKE_TASK(ObjectMgr, LoadLootsCreatureSkin);
	MAKE_TASK(ObjectMgr, LoadLootsCreaturePickPocket);
	MAKE_TASK(ObjectMgr, LoadLootsCurrency);
	tl.wait();

	MAKE_TASK(ObjectMgr,  LoadInstanceBossInfos);
	MAKE_TASK(ObjectMgr,  LoadCreatureWaypoints);
	MAKE_TASK(ObjectMgr,  LoadCreatureTimedEmotes);
	MAKE_TASK(ObjectMgr,  LoadCreatureClickSpells);
	MAKE_TASK(ObjectMgr,  LoadTrainers);
	MAKE_TASK(ObjectMgr,  LoadTotemSpells);
//	MAKE_TASK(ObjectMgr,  LoadSpellSkills);
	MAKE_TASK(ObjectMgr,  LoadSpellOverride);
	MAKE_TASK(ObjectMgr,  LoadVendors);
	MAKE_TASK(ObjectMgr,  LoadAIThreatToSpellId);
//	MAKE_TASK(ObjectMgr,  LoadSpellFixes);
	MAKE_TASK(ObjectMgr,  LoadSpellProcs);
	MAKE_TASK(ObjectMgr,  LoadSpellEffectsOverride);
	MAKE_TASK(ObjectMgr,  LoadDefaultPetSpells);
	MAKE_TASK(ObjectMgr,  LoadGuildCharters);
	MAKE_TASK(ObjectMgr,  LoadGMTickets);
	MAKE_TASK(AddonMgr,   LoadFromDB);
	MAKE_TASK(ObjectMgr,  SetHighestGuids);
	MAKE_TASK(ObjectMgr,  LoadReputationModifiers);
	MAKE_TASK(ObjectMgr,  LoadMonsterSay);
	MAKE_TASK(WeatherMgr, LoadFromDB);
	MAKE_TASK(ObjectMgr,  LoadGroups);

	MAKE_TASK(ObjectMgr, LoadExtraCreatureProtoStuff);
	MAKE_TASK(ObjectMgr, LoadExtraItemStuff);
	MAKE_TASK(QuestMgr, LoadExtraQuestStuff);
	MAKE_TASK(ObjectMgr, LoadArenaTeams);
	MAKE_TASK(ObjectMgr, LoadProfessionDiscoveries);
	MAKE_TASK(ObjectMgr, LoadQuestPOI);
	printmem("mem usage : %f\n",GetMemUsage() );

#undef MAKE_TASK

	// wait for all loading to complete.
	tl.wait();

	//this needs both dbc and item proto to be loaded
	ApplyItemEnchantFixes();

	//cause we suck at decoding flags and fields :(
	ApplyAreaFixes();

	sLocalizationMgr.Reload(false);

	CommandTableStorage::getSingleton().Load();
	Log.Notice("WordFilter", "Loading...");
	
	g_characterNameFilter = new WordFilter();
	g_chatFilter = new WordFilter();
	g_characterNameFilter->Load("wordfilter_character_names");
	g_chatFilter->Load("wordfilter_chat");

	Log.Notice("WordFilter", "Done.");

	sLog.outString("");
	Log.Notice("World", "Database loaded in %ums.", getMSTime() - start_time);
	sLog.outString("");

	printmem("mem usage : %f\n",GetMemUsage() );
	if (Collision) 
	{
		CollideInterface.Init();
	}
	printmem("mem usage : %f\n",GetMemUsage() );

	// calling this puts all maps into our task list.
	new FormationMgr;
	sInstanceMgr.Load(&tl);

	// wait for the events to complete.
	tl.wait();

	// wait for them to exit, now.
	tl.kill();
	tl.waitForThreadsToExit();
	sLog.outString("");
	LoadNameGenData();
	LoadWMOAreaData();

	Log.Notice("World", "Object size: %u bytes", sizeof(Object));
	Log.Notice("World", "Unit size: %u bytes", sizeof(Unit) + sizeof(AIInterface));
	Log.Notice("World", "Creature size: %u bytes", sizeof(Creature) + sizeof(AIInterface));
	Log.Notice("World", "Player size: %u bytes", sizeof(Player) + sizeof(ItemInterface) + 50000 + 30000 + 1000 + sizeof(AIInterface));
	Log.Notice("World", "GameObject size: %u bytes", sizeof(GameObject));

// ------------------------------------------------------------------------------------------------

	Log.Notice("World","Starting Transport System...");
	printmem("mem usage : %f\n",GetMemUsage() );
	objmgr.LoadTransporters();
	printmem("mem usage : %f\n",GetMemUsage() );
	// start mail system
	MailSystem::getSingleton().StartMailSystem();
	printmem("mem usage : %f\n",GetMemUsage() );
	Log.Notice("World", "Starting Auction System...");
	new AuctionMgr;
	sAuctionMgr.LoadAuctionHouses();
	printmem("mem usage : %f\n",GetMemUsage() );
	m_queueUpdateTimer = mQueueUpdateInterval;
	if(Config.MainConfig.GetBoolDefault("Startup", "BackgroundLootLoading", true))
	{
		Log.Notice("World", "Backgrounding loot loading...");

		// loot background loading in a lower priority thread.
		ThreadPool.ExecuteTask(new BasicTaskExecutor(new CallbackP0<LootMgr>(LootMgr::getSingletonPtr(), &LootMgr::LoadLoot), 
			BTE_PRIORITY_LOW));
	}
	else
	{
		Log.Notice("World", "Loading loot in foreground...");
		lootmgr.LoadLoot();
	}
	printmem("mem usage : %f\n",GetMemUsage() );
	Channel::LoadConfSettings();
	Log.Notice("BattlegroundManager", "Starting...");
	new CBattlegroundManager;

	dw = new DayWatcherThread();
	ThreadPool.ExecuteTask( dw );

	ThreadPool.ExecuteTask( new CharacterLoaderThread() );

	sEventMgr.AddEvent(this, &World::CheckForExpiredInstances, EVENT_WORLD_UPDATEAUCTIONS, 120000, 0, 0);
	WorldDatabase.StartQueryLogging("IngameWorldQuerys.sql");

	return true;
}

void World::Update(time_t diff)
{
	eventholder->Update((uint32)diff);
	sAuctionMgr.Update();
	_UpdateGameTime();
	UpdateQueuedSessions((uint32)diff);
#ifdef SESSION_CAP
	if( GetSessionCount() >= SESSION_CAP )
		TerminateProcess(GetCurrentProcess(),0);
#endif
}


void World::SendGlobalMessage(WorldPacket *packet, WorldSession *self)
{
	m_sessionlock.AcquireReadLock();

	SessionMap::iterator itr;
	for (itr = m_sessions.begin(); itr != m_sessions.end(); itr++)
	{
		if (itr->second->GetPlayer() &&
			itr->second->GetPlayer()->IsInWorld()
			&& itr->second != self)  // dont send to self!
		{
			itr->second->SendPacket(packet);
		}
	}

	m_sessionlock.ReleaseReadLock();
}
void World::SendFactionMessage(WorldPacket *packet, uint8 teamId)
{
	m_sessionlock.AcquireReadLock();
	SessionMap::iterator itr;
	Player * plr;
	for(itr = m_sessions.begin(); itr != m_sessions.end(); itr++)
	{
		plr = itr->second->GetPlayer();
		if(!plr || !plr->IsInWorld())
			continue;

		if(plr->GetTeam() == teamId)
			itr->second->SendPacket(packet);
	}
	m_sessionlock.ReleaseReadLock();
}

void World::SendGamemasterMessage(WorldPacket *packet, WorldSession *self)
{
	m_sessionlock.AcquireReadLock();
	SessionMap::iterator itr;
	for(itr = m_sessions.begin(); itr != m_sessions.end(); itr++)
	{
	  if (itr->second->GetPlayer() &&
	  itr->second->GetPlayer()->IsInWorld()
	  && itr->second != self)  // dont send to self!
	  {
		if(itr->second->CanUseCommand('u'))
		itr->second->SendPacket(packet);
	  }
	}
	m_sessionlock.ReleaseReadLock();
}

void World::SendZoneMessage(WorldPacket *packet, uint32 zoneid, WorldSession *self)
{
	m_sessionlock.AcquireReadLock();

	SessionMap::iterator itr;
	for (itr = m_sessions.begin(); itr != m_sessions.end(); itr++)
	{
		if (itr->second->GetPlayer() &&
			itr->second->GetPlayer()->IsInWorld()
			&& itr->second != self)  // dont send to self!
		{
			if (itr->second->GetPlayer()->GetZoneId() == zoneid)
				itr->second->SendPacket(packet);
		}
	}

	m_sessionlock.ReleaseReadLock();
}

void World::SendInstanceMessage(WorldPacket *packet, uint32 instanceid, WorldSession *self)
{
	m_sessionlock.AcquireReadLock();

	SessionMap::iterator itr;
	for (itr = m_sessions.begin(); itr != m_sessions.end(); itr++)
	{
		if (itr->second->GetPlayer() &&
			itr->second->GetPlayer()->IsInWorld()
			&& itr->second != self)  // dont send to self!
		{
			if (itr->second->GetPlayer()->GetInstanceID() == (int32)instanceid)
				itr->second->SendPacket(packet);
		}
	}

	m_sessionlock.ReleaseReadLock();
}

void World::SendWorldText(const char* text, WorldSession *self)
{
    uint32 textLen = (uint32)strlen((char*)text) + 1;

    WorldPacket data(textLen + 40);

	data.Initialize(SMSG_MESSAGECHAT);
	data << uint8(CHAT_MSG_SYSTEM);
	data << uint32(LANG_UNIVERSAL);
	
	data << (uint64)0; // Who cares about guid when there's no nickname displayed heh ?
	data << (uint32)0;
	data << (uint64)0;

	data << textLen;
	data << text;
	data << uint8(0);

	SendGlobalMessage(&data, self);

	if(announce_output)
		sLog.outString("> %s", text);
}

void World::SendGMWorldText(const char* text, WorldSession *self)
{
    uint32 textLen = (uint32)strlen((char*)text) + 1;

    WorldPacket data(textLen + 40);

	data.Initialize(SMSG_MESSAGECHAT);
	data << uint8(CHAT_MSG_SYSTEM);
	data << uint32(LANG_UNIVERSAL);
	
	data << (uint64)0;
	data << (uint32)0;
	data << (uint64)0;

	data << textLen;
	data << text;
	data << uint8(0);
	SendGamemasterMessage(&data, self);
}

void World::SendWorldWideScreenText(const char *text, WorldSession *self)
{
	WorldPacket data(256);
	data.Initialize(SMSG_AREA_TRIGGER_MESSAGE);
	data << (uint32)0 << text << (uint8)0x00;
	SendGlobalMessage(&data, self);
}

void World::UpdateSessions(uint32 diff)
{
	SessionSet::iterator itr, it2;
	WorldSession *session;
	for(itr = Sessions.begin(); itr != Sessions.end();)
	{
		session = (*itr);
		it2 = itr;
		++itr;

		//this will only do something if we are removed from world
		session->Update( WORLD_INSTANCE );

		if( session->bDeleted == true )
			Sessions.erase(it2);
	}
}

std::string World::GenerateName(uint32 type)
{
	if( _namegendata[type].empty() == true )
		return "ERR";

	uint32 ent = RandomUInt((uint32)_namegendata[type].size()-1);
	return _namegendata[type].at(ent).name;
}

void World::DeleteSession(WorldSession *session)
{
	if( session )
		RemoveSession( session->GetAccountId() );
}

uint32 World::GetNonGmSessionCount()
{
	m_sessionlock.AcquireReadLock();

	uint32 total = (uint32)m_sessions.size();

	SessionMap::const_iterator itr = m_sessions.begin();
	for( ; itr != m_sessions.end(); itr++ )
	{
		if( (itr->second)->HasGMPermissions() )
			total--;
	}

	m_sessionlock.ReleaseReadLock();

	return total;
}

uint32 World::AddQueuedSocket(WorldSocket* Socket)
{
	// Since we have multiple socket threads, better guard for this one,
	// we don't want heap corruption ;)
	queueMutex.Acquire();

	// Add socket to list
	mQueuedSessions.push_back(Socket);
	queueMutex.Release();
	// Return queue position
	return (uint32)mQueuedSessions.size();
}

void World::RemoveQueuedSocket(WorldSocket* Socket)
{
	// Since we have multiple socket threads, better guard for this one,
	// we don't want heap corruption ;)
	queueMutex.Acquire();

	// Find socket in list
	QueueSet::iterator iter = mQueuedSessions.begin();
	for(; iter != mQueuedSessions.end(); ++iter)
	{
		if((*iter) == Socket)
		{
			// Remove from the queue and abort.
			// This will be slow (Removing from middle of a vector!) but it won't
			// get called very much, so it's not really a big deal.

			mQueuedSessions.erase(iter);
			queueMutex.Release();
			return;
		}
	}
	queueMutex.Release();
}

uint32 World::GetQueuePos(WorldSocket* Socket)
{
	// Since we have multiple socket threads, better guard for this one,
	// we don't want heap corruption ;)
	queueMutex.Acquire();

	// Find socket in list
	QueueSet::iterator iter = mQueuedSessions.begin();
	uint32 QueuePos = 1;
	for(; iter != mQueuedSessions.end(); ++iter, ++QueuePos)
	{
		if((*iter) == Socket)
		{
			queueMutex.Release();
			// Return our queue position.
			return QueuePos;
		}
	}
	queueMutex.Release();
	// We shouldn't get here..
	return 1;
}

void World::UpdateQueuedSessions(uint32 diff)
{
#ifndef CLUSTERING
	if(diff >= m_queueUpdateTimer) 
	{
		m_queueUpdateTimer = mQueueUpdateInterval;
		queueMutex.Acquire();

		if( mQueuedSessions.empty() == true )
		{
			queueMutex.Release();
			return;
		}
		
		while(m_sessions.size() < m_playerLimit && mQueuedSessions.size())
		{
			// Yay. We can let another player in now.
			// Grab the first fucker from the queue, but guard of course, since
			// this is in a different thread again.

			QueueSet::iterator iter = mQueuedSessions.begin();
			WorldSocket * QueuedSocket = *iter;
			mQueuedSessions.erase(iter);

			// Welcome, sucker.
			if(QueuedSocket->GetSession())
			{
				QueuedSocket->GetSession()->deleteMutex.Acquire();
				QueuedSocket->Authenticate();
				QueuedSocket->GetSession()->deleteMutex.Release();
			}
		}

		if(mQueuedSessions.empty() == true )
		{
			queueMutex.Release();
			return;
		}

		// Update the remaining queue members.
		QueueSet::iterator iter = mQueuedSessions.begin();
		uint32 Position = 1;
		while(iter != mQueuedSessions.end())
		{
			(*iter)->UpdateQueuePosition(Position++);
			if(iter==mQueuedSessions.end())
				break;
			else
				++iter;
		}
		queueMutex.Release();
	} 
	else 
	{
		m_queueUpdateTimer -= diff;
	}
#endif
}

void World::SaveAllPlayers()
{
	if(!(ObjectMgr::getSingletonPtr()))
		return;

	sLog.outString("Saving all players to database...");
	uint32 count = 0;
	PlayerStorageMap::const_iterator itr;
		// Servers started and obviously runing. lets save all players.
	uint32 mt;
	objmgr._playerslock.AcquireReadLock();   
	for (itr = objmgr._players.begin(); itr != objmgr._players.end(); itr++)
		{
			if(itr->second->GetSession())
			{
				mt = getMSTime();
				itr->second->SaveToDB(false);
				sLog.outString("Saved player `%s` (level %u) in %ums.", itr->second->GetName(), itr->second->GetUInt32Value(UNIT_FIELD_LEVEL), getMSTime() - mt);
				++count;
			}
		}
	objmgr._playerslock.ReleaseReadLock();
	sLog.outString("Saved %u players.", count);
}

WorldSession* World::FindSessionByName(const char * Name)//case insensetive
{
	m_sessionlock.AcquireReadLock();

	// loop sessions, see if we can find him
	SessionMap::iterator itr = m_sessions.begin();
	for(; itr != m_sessions.end(); ++itr)
	{
	  if(!stricmp(itr->second->GetAccountName().c_str(),Name))
	  {
		  m_sessionlock.ReleaseReadLock();
			return itr->second;
	  }
	}
	m_sessionlock.ReleaseReadLock();
	return 0;
}

void World::MuteSessionsByIP(const char *IP,uint32 end_stamp)
{
	unsigned char ipbytes[4*4 + 16];	//watch it, it is bytes and we are reading ints
	uint32 *mute_IP = (uint32 *)&ipbytes[0];
	if( IP == NULL )
		return;
	sscanf(IP, "%u.%u.%u.%u", &ipbytes[0], &ipbytes[1], &ipbytes[2], &ipbytes[3] ); 

	m_sessionlock.AcquireReadLock();

	// loop sessions, see if we can find him
	SessionMap::iterator itr = m_sessions.begin();
	for(; itr != m_sessions.end(); ++itr)
		if( itr->second->m_muted < end_stamp && itr->second->GetSocket() )
		{
			if( *mute_IP == itr->second->GetSocket()->GetRemoteStruct().sin_addr.S_un.S_addr )
			{
				itr->second->m_muted = end_stamp;
				itr->second->SystemMessage("Your voice has been muted for %u seconds by a GM. Until this time, you will not be able to speak in any form.", end_stamp - UNIXTIME);
			}
		}
	m_sessionlock.ReleaseReadLock();
}

void World::UnMuteSessionsByIP(const char *IP)
{
	unsigned char ipbytes[4*4 + 16];	//watch it, it is bytes and we are reading ints
	uint32 *mute_IP = (uint32 *)&ipbytes[0];
	if( IP == NULL )
		return;
	sscanf(IP, "%u.%u.%u.%u", &ipbytes[0], &ipbytes[1], &ipbytes[2], &ipbytes[3] ); 

	m_sessionlock.AcquireReadLock();

	// loop sessions, see if we can find him
	SessionMap::iterator itr = m_sessions.begin();
	for(; itr != m_sessions.end(); ++itr)
		if( itr->second->GetSocket() )
		{
			if( *mute_IP == itr->second->GetSocket()->GetRemoteStruct().sin_addr.S_un.S_addr )
			{
				itr->second->m_muted = 0;
				itr->second->SystemMessage("You have been unmuted");
			}
		}
	m_sessionlock.ReleaseReadLock();
}
void World::ShutdownClasses()
{
	Log.Notice("AddonMgr", "~AddonMgr()");
	sAddonMgr.SaveToDB();
	delete AddonMgr::getSingletonPtr();

	Log.Notice("AuctionMgr", "~AuctionMgr()");
	delete AuctionMgr::getSingletonPtr();
	Log.Notice("LootMgr", "~LootMgr()");
	delete LootMgr::getSingletonPtr();

	Log.Notice("MailSystem", "~MailSystem()");
	delete MailSystem::getSingletonPtr();

	for(int i=0;i<TYPEID_UNUSED;i++)
		if( create_update_for_other_masks[i] )
		{
			delete create_update_for_other_masks[i];
			create_update_for_other_masks[i] = NULL;
		}
}

void World::GetStats(uint32 * GMCount, float * AverageLatency)
{
	int gm = 0;
	int count = 0;
	int avg = 0;
	PlayerStorageMap::const_iterator itr;
	objmgr._playerslock.AcquireReadLock();
	for (itr = objmgr._players.begin(); itr != objmgr._players.end(); itr++)
	{
		if(itr->second->GetSession())
		{
			count++;
			avg += itr->second->GetSession()->GetLatency();
			if(itr->second->GetSession()->GetPermissionCount())
				gm++;
		}			
	}
	objmgr._playerslock.ReleaseReadLock();

	*AverageLatency = count ? (float)((float)avg / (float)count) : 0;
	*GMCount = gm;
}

void TaskList::AddTask(Task * task)
{
	queueLock.Acquire();
	tasks.insert(task);
	queueLock.Release();
}

Task * TaskList::GetTask()
{
	queueLock.Acquire();

	Task* t = 0;
	for(set<Task*>::iterator itr = tasks.begin(); itr != tasks.end(); ++itr)
	{
		if(!(*itr)->in_progress)
		{
			t = (*itr);
			t->in_progress = true;
			break;
		}
	}
	queueLock.Release();
	return t;
}

void TaskList::spawn()
{
	running = true;
	thread_count = 0;

	uint32 threadcount;
	if(Config.MainConfig.GetBoolDefault("Startup", "EnableMultithreadedLoading", true))
	{
		// get processor count
#if (!defined( WIN32 ) && !defined( WIN64 ) )
#if UNIX_FLAVOUR == UNIX_FLAVOUR_LINUX
#ifdef X64
		threadcount = 2;
#else
		long affmask;
		sched_getaffinity(0, 4, (cpu_set_t*)&affmask);
		threadcount = (BitCount8(affmask)) * 2;
		if(threadcount > 8) threadcount = 8;
		else if(threadcount <= 0) threadcount = 1;
#endif
#else
		threadcount = 2;
#endif
#else
		SYSTEM_INFO s;
		GetSystemInfo(&s);
		threadcount = s.dwNumberOfProcessors * 2;
		if(threadcount>8)
			threadcount=8;
#endif
	}
	else
		threadcount = 1;

	Log.Line();
	Log.Notice("World", "Beginning %s server startup with %u threads.", (threadcount == 1) ? "progressive" : "parallel", threadcount);
	Log.Line();

	for(uint32 x = 0; x < threadcount; ++x)
		ThreadPool.ExecuteTask(new TaskExecutor(this));
}

void TaskList::wait()
{
	bool has_tasks = true;
	while(has_tasks)
	{
		queueLock.Acquire();
		has_tasks = false;
		for(set<Task*>::iterator itr = tasks.begin(); itr != tasks.end(); ++itr)
		{
			if(!(*itr)->completed)
			{
				has_tasks = true;
				break;
			}
		}
		queueLock.Release();
		Sleep(20);
	}
}

void TaskList::kill()
{
	running = false;
}

void Task::execute()
{
	_cb->execute();
}

bool TaskExecutor::run()
{
	Task * t;
	THREAD_TRY_EXECUTION
	{
		while(starter->running)
		{
			t = starter->GetTask();
			if(t)
			{
				t->execute();
				t->completed = true;
				starter->RemoveTask(t);
				delete t;
			}
			else
				Sleep(20);
		}
	}
	THREAD_HANDLE_CRASH
	return true;
}

void TaskList::waitForThreadsToExit()
{
	while(thread_count)
	{
		Sleep(20);
	}
}

void World::Rehash(bool load)
{
	if(load)
	{
		#if (defined( WIN32 ) || defined( WIN64 ) )
		Config.MainConfig.SetSource("configs/arcemu-world.conf", true);
		Config.OptionalConfig.SetSource("configs/arcemu-optional.conf", true);
		#else
		Config.MainConfig.SetSource((char*)CONFDIR "/arcemu-world.conf", true);
		Config.OptionalConfig.SetSource((char*)CONFDIR "/arcemu-optional.conf", true);
		#endif
	}
	if(!ChannelMgr::getSingletonPtr())
		new ChannelMgr;

	if(!MailSystem::getSingletonPtr())
		new MailSystem;

	channelmgr.seperatechannels = Config.MainConfig.GetBoolDefault("Server", "SeperateChatChannels", false);
	MapPath = Config.MainConfig.GetStringDefault("Terrain", "MapPath", "maps");
	vMapPath = Config.MainConfig.GetStringDefault("Terrain", "vMapPath", "vmaps");
	UnloadMapFiles = Config.MainConfig.GetBoolDefault("Terrain", "UnloadMapFiles", true);
	BreathingEnabled = Config.MainConfig.GetBoolDefault("Server", "EnableBreathing", true);
	SendStatsOnJoin = Config.MainConfig.GetBoolDefault("Server", "SendStatsOnJoin", true);
	compression_threshold = Config.MainConfig.GetIntDefault("Server", "CompressionThreshold", 400);

	// load regeneration rates.
	setRate(RATE_HEALTH,Config.MainConfig.GetFloatDefault("Rates", "Health",1));
	setRate(RATE_POWER1,Config.MainConfig.GetFloatDefault("Rates", "Power1",1));
	setRate(RATE_POWER2,Config.MainConfig.GetFloatDefault("Rates", "Power2",1));
	setRate(RATE_POWER3,Config.MainConfig.GetFloatDefault("Rates", "Power3",1));
	setRate(RATE_POWER4,Config.MainConfig.GetFloatDefault("Rates", "Power4",1));
	setRate(RATE_POWER5,Config.MainConfig.GetFloatDefault("Rates", "Power5",1));
	setRate(RATE_DROP0,Config.MainConfig.GetFloatDefault("Rates", "DropGrey",1));
	setRate(RATE_DROP1,Config.MainConfig.GetFloatDefault("Rates", "DropWhite",1));
	setRate(RATE_DROP2,Config.MainConfig.GetFloatDefault("Rates", "DropGreen",1));
	setRate(RATE_DROP3,Config.MainConfig.GetFloatDefault("Rates", "DropBlue",1));
	setRate(RATE_DROP4,Config.MainConfig.GetFloatDefault("Rates", "DropPurple",1));
	setRate(RATE_DROP5,Config.MainConfig.GetFloatDefault("Rates", "DropOrange",1));
	setRate(RATE_DROP6,Config.MainConfig.GetFloatDefault("Rates", "DropArtifact",1));
	setRate(RATE_XP,Config.MainConfig.GetFloatDefault("Rates", "XP",1));
	setRate(RATE_RESTXP,Config.MainConfig.GetFloatDefault("Rates", "RestXP", 1));
	setRate(RATE_QUESTXP,Config.MainConfig.GetFloatDefault("Rates", "QuestXP", 1));
	setRate(RATE_EXPLOREXP,Config.MainConfig.GetFloatDefault("Rates", "ExploreXP", 1));
	setIntRate(INTRATE_SAVE, Config.MainConfig.GetIntDefault("Rates", "Save", 30000));	// every 30 seconds to not spam DB
	setRate(RATE_MONEY, Config.MainConfig.GetFloatDefault("Rates", "DropMoney", 1.0f));
	setRate(RATE_QUESTREPUTATION, Config.MainConfig.GetFloatDefault("Rates", "QuestReputation", 1.0f));
	setRate(RATE_KILLREPUTATION, Config.MainConfig.GetFloatDefault("Rates", "KillReputation", 1.0f));
	setRate(RATE_HONOR, Config.MainConfig.GetFloatDefault("Rates", "Honor", 1.0f));
	setRate(RATE_SKILLCHANCE, Config.MainConfig.GetFloatDefault("Rates", "SkillChance", 1.0f));
	setRate(RATE_SKILLRATE, Config.MainConfig.GetFloatDefault("Rates", "SkillRate", 1.0f));
	setIntRate(INTRATE_COMPRESSION, Config.MainConfig.GetIntDefault("Rates", "Compression", 1));
	setIntRate(INTRATE_PVPTIMER, Config.MainConfig.GetIntDefault("Rates", "PvPTimer", 300000));
//	ArenaQueueDiff = Config.MainConfig.GetIntDefault("Rates", "ArenaQueueDiff", 150);
	ArenaQueueDiff = Config.MainConfig.GetIntDefault("Rates", "ArenaQueueDiff", 500);
	setRate(RATE_ARENAPOINTMULTIPLIER2X, Config.MainConfig.GetFloatDefault("Rates", "ArenaMultiplier2x", 1.0f));
	setRate(RATE_ARENAPOINTMULTIPLIER3X, Config.MainConfig.GetFloatDefault("Rates", "ArenaMultiplier3x", 1.0f));
	setRate(RATE_ARENAPOINTMULTIPLIER5X, Config.MainConfig.GetFloatDefault("Rates", "ArenaMultiplier5x", 1.0f));
	SetPlayerLimit(Config.MainConfig.GetIntDefault("Server", "PlayerLimit", 1000));
	SetMotd(Config.MainConfig.GetStringDefault("Server", "Motd", "Arcemu Default MOTD").c_str());
	mQueueUpdateInterval = Config.MainConfig.GetIntDefault("Server", "QueueUpdateInterval", 5000);
	SetKickAFKPlayerTime(Config.MainConfig.GetIntDefault("Server", "KickAFKPlayers", 0));
	sLog.SetScreenLoggingLevel(Config.MainConfig.GetIntDefault("LogLevel", "Screen", 1));
	sLog.SetFileLoggingLevel(Config.MainConfig.GetIntDefault("LogLevel", "File", -1));
	Log.log_level = Config.MainConfig.GetIntDefault("LogLevel", "Screen", 1);
	gm_skip_attunement = Config.MainConfig.GetBoolDefault("Server", "SkipAttunementsForGM", true);
	Collision = Config.MainConfig.GetBoolDefault("Server", "Collision", 0);
	setIntRate( INTRATE_START_EXTRA_TP, Config.OptionalConfig.GetIntDefault("Optional", "ExtraTalentPoints", 0));
	setRate( RATE_MOVE_SPEED, Config.OptionalConfig.GetFloatDefault("Optional", "MoveSpeedMod", 0.0f));
	setIntRate( INTRATE_LOGIN_BUFF1, Config.OptionalConfig.GetIntDefault("Optional", "LoginBuff1", 0));
	setIntRate( INTRATE_RESPAWN_COOLDOWN, Config.OptionalConfig.GetIntDefault("Optional", "RespawnCooldown", 0));
	setIntRate( INTRATE_RANDOMIZEITEMS1, Config.OptionalConfig.GetIntDefault("Optional", "RandomizeItems1", 0));
	setIntRate( INTRATE_DISABLEITEMCONSUME, Config.OptionalConfig.GetIntDefault("Optional", "InfiniteItemCharges", 0));
//	setRate( RATE_CAST_AND_COOLDOWN_SPEED, Config.OptionalConfig.GetFloatDefault("Optional", "CastAndCooldownSpeed", 0.0f));
	setRate( RATE_WEEKENDXP, Config.OptionalConfig.GetFloatDefault("Optional", "WeekendXPRateExtra", 0.0f));
	setIntRate( INTRATE_ALLOW_QUEST_TALENTPOINTS, Config.OptionalConfig.GetIntDefault("Optional", "AllowQuestTalentPoints", 0));
	setIntRate( INTRATE_NO_CURRENCY_LIMITS, Config.OptionalConfig.GetIntDefault("Optional", "NoCurrencyLimit", 0));	
	setIntRate( INTRATE_DISABLE_MAIL_FEATURE, Config.OptionalConfig.GetIntDefault("Optional", "DisableMail", 0));	
	setIntRate( INTRATE_DISABLE_TRADE_FEATURE, Config.OptionalConfig.GetIntDefault("Optional", "DisableTrade", 0));	
	setIntRate( INTRATE_LOGON_TYPE, Config.MainConfig.GetIntDefault( "LogonDatabase", "ServerType", 1 ) );	// 1 arcemu, 2 Trinity, 3 Both(arcemu first)

	time_t TimeNow = time(0);
	tm *gmtm = gmtime(&TimeNow);
	if( gmtm->tm_wday == 0 || gmtm->tm_wday == 6 )
	{
		setRate(RATE_XP, getRate( RATE_XP ) + getRate( RATE_WEEKENDXP ) );
		setRate(RATE_RESTXP, getRate( RATE_RESTXP ) + getRate( RATE_WEEKENDXP ) );
		setRate(RATE_QUESTXP, getRate( RATE_QUESTXP ) + getRate( RATE_WEEKENDXP ) );
		setRate(RATE_EXPLOREXP, getRate( RATE_EXPLOREXP ) + getRate( RATE_WEEKENDXP ) );
	}
#ifndef CLUSTERING
	SocketRecvBufSize = Config.MainConfig.GetIntDefault("WorldSocket", "RecvBufSize", WORLDSOCKET_RECVBUF_SIZE);
	SocketSendBufSize = Config.MainConfig.GetIntDefault("WorldSocket", "SendBufSize", WORLDSOCKET_SENDBUF_SIZE);
#endif

	bool log_enabled = Config.MainConfig.GetBoolDefault("Log", "Cheaters", false);
	if(Anticheat_Log->IsOpen())
	{
		if(!log_enabled)
			Anticheat_Log->Close();
	}
	else
		if(log_enabled)
			Anticheat_Log->Open();

	log_enabled = Config.MainConfig.GetBoolDefault("Log", "GMCommands", false);
	if(GMCommand_Log->IsOpen())
	{
		if(!log_enabled)
			GMCommand_Log->Close();
	}
	else
		if(log_enabled)
			GMCommand_Log->Open();

	log_enabled = Config.MainConfig.GetBoolDefault("Log", "Player", false);
	if(Player_Log->IsOpen())
	{
		if(!log_enabled)
			Player_Log->Close();
	}
	else
	{
		if(log_enabled)
			Player_Log->Open();
	}

#if (defined( WIN32 ) || defined( WIN64 ) )
	DWORD current_priority_class = GetPriorityClass( GetCurrentProcess() );
	bool high = Config.MainConfig.GetBoolDefault( "Server", "AdjustPriority", false );

	if( high )
	{
		if( current_priority_class != HIGH_PRIORITY_CLASS )
			SetPriorityClass( GetCurrentProcess(), HIGH_PRIORITY_CLASS );
	}
	else
	{
		if( current_priority_class != NORMAL_PRIORITY_CLASS )
			SetPriorityClass( GetCurrentProcess(), NORMAL_PRIORITY_CLASS );
	}
#endif

	if(!Config.MainConfig.GetString("GMClient", "GmClientChannel", &GmClientChannel))
	{
		GmClientChannel = "";
	}

	m_reqGmForCommands = !Config.MainConfig.GetBoolDefault("Server", "AllowPlayerCommands", false);
	m_lfgForNonLfg = Config.MainConfig.GetBoolDefault("Server", "EnableLFGJoin", false);

	realmtype = Config.MainConfig.GetBoolDefault("Server", "RealmType", false);
	TimeOut= uint32(1000* Config.MainConfig.GetIntDefault("Server", "ConnectionTimeout", 180) );
	GMTTimeZone = Config.MainConfig.GetIntDefault("Server", "TimeZone", 0);

	uint32 config_flags = 0;
	if(Config.MainConfig.GetBoolDefault("Mail", "DisablePostageCostsForGM", true))
		config_flags |= MAIL_FLAG_NO_COST_FOR_GM;

	if(Config.MainConfig.GetBoolDefault("Mail", "DisablePostageCosts", false))
		config_flags |= MAIL_FLAG_DISABLE_POSTAGE_COSTS;

	if(Config.MainConfig.GetBoolDefault("Mail", "DisablePostageDelayItems", true))
		config_flags |= MAIL_FLAG_DISABLE_HOUR_DELAY_FOR_ITEMS;

	if(Config.MainConfig.GetBoolDefault("Mail", "DisableMessageExpiry", false))
		config_flags |= MAIL_FLAG_NO_EXPIRY;

	if(Config.MainConfig.GetBoolDefault("Mail", "EnableInterfactionMail", true))
		config_flags |= MAIL_FLAG_CAN_SEND_TO_OPPOSITE_FACTION;

	if(Config.MainConfig.GetBoolDefault("Mail", "EnableInterfactionForGM", true))
		config_flags |= MAIL_FLAG_CAN_SEND_TO_OPPOSITE_FACTION_GM;

	sMailSystem.config_flags = config_flags;
	flood_lines = Config.MainConfig.GetIntDefault("FloodProtection", "Lines", 0);
	flood_seconds = Config.MainConfig.GetIntDefault("FloodProtection", "Seconds", 0);
	flood_seconds_ban = Config.MainConfig.GetIntDefault("FloodProtection", "BanforSeconds", 0);
	flood_message = Config.MainConfig.GetBoolDefault("FloodProtection", "SendMessage", false);
	show_gm_in_who_list = Config.MainConfig.GetBoolDefault("Server", "ShowGMInWhoList", false);
	interfaction_chat = Config.OptionalConfig.GetBoolDefault("Interfaction", "InterfactionChat", false);
	interfaction_group = Config.OptionalConfig.GetBoolDefault("Interfaction", "InterfactionGroup", false);
	interfaction_guild = Config.OptionalConfig.GetBoolDefault("Interfaction", "InterfactionGuild", false);
	interfaction_trade = Config.OptionalConfig.GetBoolDefault("Interfaction", "InterfactionTrade", false);
	interfaction_friend= Config.OptionalConfig.GetBoolDefault("Interfaction", "InterfactionFriends", false);
	interfaction_misc = Config.OptionalConfig.GetBoolDefault("Interfaction", "InterfactionMisc", false);
	crossover_chars = Config.OptionalConfig.GetBoolDefault("Interfaction", "CrossOverCharacters", false);
	gamemaster_listOnlyActiveGMs = Config.OptionalConfig.GetBoolDefault("GameMaster", "ListOnlyActiveGMs", false);
	gamemaster_hidePermissions = Config.OptionalConfig.GetBoolDefault("GameMaster", "HidePermissions", false);
	start_level = Config.OptionalConfig.GetIntDefault("Optional", "StartingLevel", 1);
	AlianceExtraXPRate = Config.OptionalConfig.GetFloatDefault("Optional", "AlianceExtraXPRate", 1);
	HordeExtraXPRate = Config.OptionalConfig.GetFloatDefault("Optional", "HordeExtraXPRate", 1);
	if(start_level > PLAYER_LEVEL_CAP) {start_level = PLAYER_LEVEL_CAP;}
	antiMasterLootNinja = Config.OptionalConfig.GetBoolDefault("Optional", "AntiMasterLootNinja", true);
	realmAllowTBCcharacters = Config.OptionalConfig.GetBoolDefault("Optional", "AllowTBC", true);
	GoldStartAmount = Config.OptionalConfig.GetIntDefault("Optional", "StartingGold", 0);
	MaxLootListSize = Config.OptionalConfig.GetIntDefault("Optional", "MaxLootItemCount", 16);	//16 because client limits it that way for the moment
	ClientCacheVersion = Config.MainConfig.GetIntDefault("Server", "ClientCacheVersion", 12345678);

	announce_tag = Config.MainConfig.GetStringDefault("Announce", "Tag", "Staff");
	GMAdminTag = Config.MainConfig.GetBoolDefault("Announce", "GMAdminTag", false);
	NameinAnnounce = Config.MainConfig.GetBoolDefault("Announce", "NameinAnnounce", true);
	NameinWAnnounce = Config.MainConfig.GetBoolDefault("Announce", "NameinWAnnounce", true);
	announce_output = Config.MainConfig.GetBoolDefault("Announce", "ShowInConsole", true);
	announce_tagcolor = Config.OptionalConfig.GetIntDefault("Color", "AnnTagColor", 2);
	announce_gmtagcolor = Config.OptionalConfig.GetIntDefault("Color", "AnnGMTagColor", 1);
	announce_namecolor = Config.OptionalConfig.GetIntDefault("Color", "AnnNameColor", 4);
	announce_msgcolor = Config.OptionalConfig.GetIntDefault("Color", "AnnMsgColor", 10);
	AnnounceColorChooser(announce_tagcolor, announce_gmtagcolor, announce_namecolor, announce_msgcolor);

	if(!flood_lines || !flood_seconds)
		flood_lines = flood_seconds = flood_seconds_ban = 0;

	map_unload_time=Config.MainConfig.GetIntDefault("Server", "MapUnloadTime", 0);

	antihack_teleport = Config.MainConfig.GetBoolDefault("AntiHack", "Teleport", true);
	antihack_speed = Config.MainConfig.GetBoolDefault("AntiHack", "Speed", true);
	antihack_flight = Config.MainConfig.GetBoolDefault("AntiHack", "Flight", true);
	flyhack_threshold = Config.MainConfig.GetIntDefault("AntiHack", "FlightThreshold", 8);
	no_antihack_on_gm = Config.MainConfig.GetBoolDefault("AntiHack", "DisableOnGM", false);
	SpeedhackProtection = antihack_speed;
	m_levelCap = Config.OptionalConfig.GetIntDefault("Optional", "LevelCap", PLAYER_LEVEL_CAP);
	m_genLevelCap = Config.OptionalConfig.GetIntDefault("Optional", "GenLevelCap", PLAYER_LEVEL_CAP);
	m_limitedNames = Config.MainConfig.GetBoolDefault("Server", "LimitedNames", true);
	m_useAccountData = Config.MainConfig.GetBoolDefault("Server", "UseAccountData", false);

	mob_NormalHealSpellFactor = Config.OptionalConfig.GetFloatDefault("WorldDifficulty", "NormalHealSpellFactor", 1.0f);
	mob_NormalDmgSpellFactor = Config.OptionalConfig.GetFloatDefault("WorldDifficulty", "NormalDmgSpellFactor", 1.0f);
	mob_HeroicHealSpellFactor = Config.OptionalConfig.GetFloatDefault("WorldDifficulty", "HeroicHealSpellFactor", 1.5f);
	mob_HeroicDmgSpellFactor = Config.OptionalConfig.GetFloatDefault("WorldDifficulty", "HeroicDmgSpellFactor", 1.5f);
	mob_HeroicSpellCostPerLVL = Config.OptionalConfig.GetFloatDefault("WorldDifficulty", "HeroicSpellCostPerLVL", 0.05f);
	mob_HeroicHPEXPPerLVL = Config.OptionalConfig.GetFloatDefault("WorldDifficulty", "HeroicHPEXPPerLVL", 1.001f);
	mob_HeroicArmorEXPPerLVL = Config.OptionalConfig.GetFloatDefault("WorldDifficulty", "HeroicArmorEXPPerLVL", 1.011f);
	mob_HeroicArmorFlatPerLVL = Config.OptionalConfig.GetFloatDefault("WorldDifficulty", "HeroicArmorFlatPerLVL", 83.3f);
	mob_HeroicResFlatPerLVL = Config.OptionalConfig.GetFloatDefault("WorldDifficulty", "HeroicResFlatPerLVL", 3.75f);
	mob_HeroicSpellResEXPPerLVL = Config.OptionalConfig.GetFloatDefault("WorldDifficulty", "HeroicSpellResEXPPerLVL", 1.047f);
	mob_HeroicAttackFlatSpeedPerLVL = Config.OptionalConfig.GetFloatDefault("WorldDifficulty", "HeroicAttackFlatSpeedPerLVL", 6.25f);
	mob_HeroicDMGEXPPerLVL = Config.OptionalConfig.GetFloatDefault("WorldDifficulty", "HeroicDMGEXPPerLVL", 1.006f);
	mob_HeroicSpellCooldownPCT = Config.OptionalConfig.GetFloatDefault("WorldDifficulty", "HeroicSpellCooldownPCT", 10.0f );
	mob_HeroicSpellCooldownPCT = (100 - mob_HeroicSpellCooldownPCT) / 100;
	mob_SpellKillerBlowBelow = Config.OptionalConfig.GetFloatDefault("WorldDifficulty", "SpellKillerBlowBelow", 100000.0f);
	mob_SpellKillerBlowReduction = Config.OptionalConfig.GetFloatDefault("WorldDifficulty", "SpellKillerBlowReduction", 3.0f);
	AnyKillerBlowBelow = Config.OptionalConfig.GetFloatDefault("WorldDifficulty", "AnyKillerBlowBelow", 1000000.0f);
	AnyKillerBlowReduction = Config.OptionalConfig.GetFloatDefault("WorldDifficulty", "AnyKillerBlowReduction", 0.0f);

	instance_TakeGroupLeaderID = Config.MainConfig.GetBoolDefault("InstanceHandling", "TakeGroupLeaderID", true);
	instance_SlidingExpiration = Config.MainConfig.GetBoolDefault("InstanceHandling", "SlidingExpiration", false);
	instance_DailyHeroicInstanceResetHour = Config.MainConfig.GetIntDefault("InstanceHandling", "DailyHeroicInstanceResetHour", 5);

	if(instance_DailyHeroicInstanceResetHour < 0)
		instance_DailyHeroicInstanceResetHour = 0;
	if(instance_DailyHeroicInstanceResetHour > 23)
		instance_DailyHeroicInstanceResetHour = 23;

	if( m_banTable != NULL )
		free( m_banTable );

	m_banTable = NULL;
	string s = Config.MainConfig.GetStringDefault( "Server", "BanTable", "" );
	if( !s.empty() )
		m_banTable = strdup_local( s.c_str() );

	if( load )
		Channel::LoadConfSettings();
}

void World::LoadNameGenData()
{
	DBCFile dbc;

	if( !dbc.open( "DBC/NameGen.dbc" ) )
	{
		Log.Error( "World", "Cannot find file ./DBC/NameGen.dbc" );
		return;
	}

	for(uint32 i = 0; i < dbc.getRecordCount(); ++i)
	{
		NameGenData d;
		if(dbc.getRecord(i).getString(1)==NULL)
			continue;

		d.name = string(dbc.getRecord(i).getString(1));
		d.type = dbc.getRecord(i).getUInt(3);
		_namegendata[d.type].push_back(d);
	}
}

void World::CharacterEnumProc(QueryResultVector& results, uint32 AccountId)
{
	WorldSession * s = FindSession(AccountId);
	if(s == NULL)
		return;

	s->CharacterEnumProc(results[0].result);
}

void World::AnnounceColorChooser(int tagcolor, int gmtagcolor, int namecolor, int msgcolor)
{
	switch(tagcolor)
	{
		case 1:
			ann_tagcolor = "|cffff6060"; //lightred
			break;
		case 2:
			ann_tagcolor = "|cff00ccff"; //lightblue
			break;
		case 3:
			ann_tagcolor = "|cff0000ff"; //blue
			break;
		case 4:
			ann_tagcolor = "|cff00ff00"; //green
			break;
		case 5:
			ann_tagcolor = "|cffff0000"; //red
			break;
		case 6:
			ann_tagcolor = "|cffffcc00"; //gold
			break;
		case 7:
			ann_tagcolor = "|cff888888"; //grey
			break;
		case 8:
			ann_tagcolor = "|cffffffff"; //white
			break;
		case 9:
			ann_tagcolor = "|cffff00ff"; //magenta
			break;
		case 10:
			ann_tagcolor = "|cffffff00"; //yellow
			break;
	}
	switch(gmtagcolor)
	{
		case 1:
			ann_gmtagcolor = "|cffff6060"; //lightred
			break;
		case 2:
			ann_gmtagcolor = "|cff00ccff"; //lightblue
			break;
		case 3:
			ann_gmtagcolor = "|cff0000ff"; //blue
			break;
		case 4:
			ann_gmtagcolor = "|cff00ff00"; //green
			break;
		case 5:
			ann_gmtagcolor = "|cffff0000"; //red
			break;
		case 6:
			ann_gmtagcolor = "|cffffcc00"; //gold
			break;
		case 7:
			ann_gmtagcolor = "|cff888888"; //grey
			break;
		case 8:
			ann_gmtagcolor = "|cffffffff"; //white
			break;
		case 9:
			ann_gmtagcolor = "|cffff00ff"; //magenta
			break;
		case 10:
			ann_gmtagcolor = "|cffffff00"; //yellow
			break;
	}
	switch(namecolor)
	{
		case 1:
			ann_namecolor = "|cffff6060"; //lightred
			break;
		case 2:
			ann_namecolor = "|cff00ccff"; //lightblue
			break;
		case 3:
			ann_namecolor = "|cff0000ff"; //blue
			break;
		case 4:
			ann_namecolor = "|cff00ff00"; //green
			break;
		case 5:
			ann_namecolor = "|cffff0000"; //red
			break;
		case 6:
			ann_namecolor = "|cffffcc00"; //gold
			break;
		case 7:
			ann_namecolor = "|cff888888"; //grey
			break;
		case 8:
			ann_namecolor = "|cffffffff"; //white
			break;
		case 9:
			ann_namecolor = "|cffff00ff"; //magenta
			break;
		case 10:
			ann_namecolor = "|cffffff00"; //yellow
			break;
	}
	switch(msgcolor)
	{
		case 1:
			ann_msgcolor = "|cffff6060"; //lightred
			break;
		case 2:
			ann_msgcolor = "|cff00ccff"; //lightblue
			break;
		case 3:
			ann_msgcolor = "|cff0000ff"; //blue
			break;
		case 4:
			ann_msgcolor = "|cff00ff00"; //green
			break;
		case 5:
			ann_msgcolor = "|cffff0000"; //red
			break;
		case 6:
			ann_msgcolor = "|cffffcc00"; //gold
			break;
		case 7:
			ann_msgcolor = "|cff888888"; //grey
			break;
		case 8:
			ann_msgcolor = "|cffffffff"; //white
			break;
		case 9:
			ann_msgcolor = "|cffff00ff"; //magenta
			break;
		case 10:
			ann_msgcolor = "|cffffff00"; //yellow
			break;
	}
	printf("\nAnnounce colors initialized.\n");
}

void World::LoadAccountDataProc(QueryResultVector& results, uint32 AccountId)
{
	WorldSession * s = FindSession(AccountId);
	if(s == NULL)
		return;

	s->LoadAccountDataProc(results[0].result);
}

void World::CleanupCheaters()
{
	/*uint32 guid;
	string name;
	uint32 cl;
	uint32 level;
	uint32 talentpts;
	char * start, *end;
	Field * f;
	uint32 should_talents;
	uint32 used_talents;
	SpellEntry * sp;

	QueryResult * result = CharacterDatabase.Query("SELECT guid, name, class, level, available_talent_points, spells FROM characters");
	if(result == NULL)
		return;

	do 
	{
		f = result->Fetch();
		guid = f[0].GetUInt32();
		name = string(f[1].GetString());
		cl = f[2].GetUInt32();
		level = f[3].GetUInt32();
		talentpts = f[4].GetUInt32();
		start = f[5].GetString();
		should_talents = (level<10 ? 0 : level - 9);
		used_talents -= 
        		

		start = (char*)get_next_field.GetString();//buff;
		while(true) 
		{
			end = strchr(start,',');
			if(!end)break;
			*end=0;
			sp = dbcSpell.LookupEntry(atol(start));
			start = end +1;

			if(sp->talent_tree)

		}

	} while(result->NextRow());*/

}

void World::CheckForExpiredInstances()
{
	sInstanceMgr.CheckForExpiredInstances();
}

struct insert_playeritem
{
	uint32 ownerguid;
	uint32 entry;
	uint32 wrapped_item_id;
	uint32 wrapped_creator;
	uint32 creator;
	uint32 count;
	uint32 charges;
	uint32 flags;
	uint32 randomprop;
	uint32 randomsuffix;
	uint32 itemtext;
	uint32 durability;
	int32 containerslot;
	int32 slot;
	string enchantments;
};

#define LOAD_THREAD_SLEEP 180

void CharacterLoaderThread::OnShutdown()
{
	running=false;
#if (defined( WIN32 ) || defined( WIN64 ) )
	SetEvent(hEvent);
#else
	pthread_cond_signal(&cond);
#endif
}

CharacterLoaderThread::CharacterLoaderThread()
{

}

CharacterLoaderThread::~CharacterLoaderThread()
{
#if (defined( WIN32 ) || defined( WIN64 ) )
	CloseHandle(hEvent);
#else
	pthread_cond_destroy(&cond);
	pthread_mutex_destroy(&mutex);
#endif
}

bool CharacterLoaderThread::run()
{
#if (defined( WIN32 ) || defined( WIN64 ) )
	hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
#else
	struct timeval now;
	struct timespec tv;

	pthread_mutex_init(&mutex,NULL);
	pthread_cond_init(&cond,NULL);
#endif
	running=true;
	for(;;)
	{
		// Get a single connection to maintain for the whole process.
		DatabaseConnection * con = CharacterDatabase.GetFreeConnection();

		// this hasn't been updated in some time, enable it if you want to fix/use it
#if 0
		sWorld.PollCharacterInsertQueue(con);
#endif

		sWorld.PollMailboxInsertQueue(con);
		/* While this looks weird, it ensures the system doesn't waste time switching to these contexts.
		   WaitForSingleObject will suspend the thread,
		   and on unix, select will as well. - Burlex
			*/
		con->Busy.Release();
#if (defined( WIN32 ) || defined( WIN64 ) )
		if (hEvent)
			WaitForSingleObject(hEvent,LOAD_THREAD_SLEEP*1000);
#else
		gettimeofday(&now, NULL);
		tv.tv_sec = now.tv_sec + LOAD_THREAD_SLEEP;
		tv.tv_nsec = now.tv_usec * 1000;
		pthread_mutex_lock(&mutex);
		pthread_cond_timedwait(&cond, &mutex, &tv);
		pthread_mutex_unlock(&mutex);
#endif
		if(!running)
			break;
	}

	return true;
}

void World::PollMailboxInsertQueue(DatabaseConnection * con)
{
	QueryResult * result;
	Field * f;
	Item * pItem;
	uint32 itemid;
	uint32 stackcount;

	result = CharacterDatabase.FQuery("SELECT * FROM mailbox_insert_queue", con);
	if( result != NULL )
	{
		Log.Notice("MailboxQueue", "Sending queued messages....");
		do 
		{
			f = result->Fetch();
			itemid = f[6].GetUInt32();
			stackcount = f[7].GetUInt32();
			
			if( itemid != 0 )
			{
				pItem = objmgr.CreateItem( itemid, NULL );
				if( pItem != NULL )
				{
					pItem->SetUInt32Value( ITEM_FIELD_STACK_COUNT, stackcount );
					pItem->SaveToDB( 0, 0, true, NULL );
				}
			}
			else
				pItem = NULL;

			Log.Notice("MailboxQueue", "Sending message to %u (item: %u)...", f[1].GetUInt32(), itemid);
			sMailSystem.SendAutomatedMessage( 0, f[0].GetUInt64(), f[1].GetUInt64(), f[2].GetString(), f[3].GetString(), f[5].GetUInt32(),
				0, pItem ? pItem->GetGUID() : 0, f[4].GetUInt32() );

			if( pItem != NULL )
			{
				pItem->DeleteMe();
				pItem = NULL;
			}
		} while ( result->NextRow() );
		delete result;
		result = NULL;
		Log.Notice("MailboxQueue", "Done.");
		CharacterDatabase.FWaitExecute("DELETE FROM mailbox_insert_queue", con);
	}
}

void World::PollCharacterInsertQueue(DatabaseConnection * con)
{
	// Our local stuff..
	bool has_results = false;
	map<uint32, vector<insert_playeritem> > itemMap;
	map<uint32,vector<insert_playeritem> >::iterator itr;
	Field * f;
	insert_playeritem ipi;                          
	static const char * characterTableFormat = "uSuuuuuussuuuuuuuuuuuuuuffffuususuufffuuuuusuuuUssuuuuuuffffuuuuufffssssssuuuuuuuuu";

	// Lock the table to prevent any more inserts
	CharacterDatabase.FWaitExecute("LOCK TABLES `playeritems_insert_queue` WRITE", con);

	// Cache all items in memory. This will save us doing additional queries and slowing down the db.
	QueryResult * result = CharacterDatabase.FQuery("SELECT * FROM playeritems_insert_queue", con);
	if(result)
	{
		do 
		{
			f = result->Fetch();
			
			ipi.ownerguid = f[0].GetUInt32();
			ipi.entry = f[1].GetUInt32();
			ipi.wrapped_item_id = f[2].GetUInt32();
			ipi.wrapped_creator = f[3].GetUInt32();
			ipi.creator = f[4].GetUInt32();
			ipi.count = f[5].GetUInt32();
			ipi.charges = f[6].GetUInt32();
			ipi.flags = f[7].GetUInt32();
			ipi.randomprop = f[8].GetUInt32();
			ipi.randomsuffix = f[9].GetUInt32();
			ipi.itemtext = f[10].GetUInt32();
			ipi.durability = f[11].GetUInt32();
			ipi.containerslot = f[12].GetInt32();
			ipi.slot = f[13].GetInt32();
			ipi.enchantments = string(f[14].GetString());

			itr = itemMap.find(ipi.ownerguid);
			if(itr == itemMap.end())
			{
				vector<insert_playeritem> to_insert;
				to_insert.push_back(ipi);
				itemMap.insert(make_pair(ipi.ownerguid,to_insert));
			}
			else
			{
				itr->second.push_back(ipi);
			}
		
		} while(result->NextRow());
		delete result;
		result = NULL;
	}

	// Unlock the item table
	CharacterDatabase.FWaitExecute("UNLOCK TABLES", con);

	// Lock the character table
	CharacterDatabase.FWaitExecute("LOCK TABLES `characters_insert_queue` WRITE", con);

	// Load the characters, and assign them their new guids, and insert them into the main db.
	result = CharacterDatabase.FQuery("SELECT * FROM characters_insert_queue", con);

	// Can be unlocked now.
	CharacterDatabase.FWaitExecute("UNLOCK TABLES", con);

	if(result)
	{
		uint32 guid;
		std::stringstream ss;
		do 
		{
			f = result->Fetch();
			char * p = (char*)characterTableFormat;
			uint32 i = 1;
			guid = f[0].GetUInt32();
			uint32 new_guid = objmgr.GenerateLowGuid(HIGHGUID_TYPE_PLAYER);
			uint32 new_item_guid;
			ss << "INSERT INTO characters VALUES(" << new_guid;

			// create his playerinfo in the server
			PlayerInfo * inf = new PlayerInfo();
			inf->acct = f[1].GetUInt32();
#ifdef VOICE_CHAT
			inf->groupVoiceId = -1;
#endif

			while(*p != 0)
			{
				switch(*p)
				{
				case 's':
					ss << ",'" << CharacterDatabase.EscapeString(f[i].GetString(), con) << "'";
					break;

				case 'f':
					ss << ",'" << f[i].GetFloat() << "'";
					break;

				case 'S':
					{
						// this is the character name, append a hex version of the guid to it to prevent name clashes.
						char newname[100];
						snprintf(newname,20,"%5s%X",f[i].GetString(),new_guid);
						ss << ",'" << CharacterDatabase.EscapeString(newname,con) << "'";
						inf->name = strdup_local(newname);
					}break;

				case 'U':
					{
						// this is our forced rename field. force it to one.
						ss << ",1";
					}break;

				default:
					ss << "," << f[i].GetUInt32();
					break;
				}

				++i;
				++p;
			}
			ss << ",''";
			ss << ")";
			CharacterDatabase.FWaitExecute(ss.str().c_str(),con);

			inf->_class = f[4].GetUInt32();
			inf->gender = f[5].GetUInt32();
			inf->guid = new_guid;
			inf->lastLevel = f[7].GetUInt32();
			inf->lastOnline = UNIXTIME;
			inf->lastZone = 0;
			inf->m_Group=NULL;
			inf->m_loggedInPlayer=NULL;
			inf->guild=NULL;
			inf->guildRank=NULL;
			inf->guildMember=NULL;
			inf->race=f[3].GetUInt32();
			inf->subGroup=0;
			switch(inf->race)
			{
			case RACE_HUMAN:
			case RACE_GNOME:
			case RACE_DWARF:
			case RACE_NIGHTELF:
			case RACE_DRAENEI:
				inf->team=0;
				break;

			default:
				inf->team=1;
				break;
			}
			
			// add playerinfo to objectmgr
			objmgr.AddPlayerInfo(inf);

			// grab all his items, assign them their new guids and insert them
			itr = itemMap.find(guid);
			if(itr != itemMap.end())
			{
				for(vector<insert_playeritem>::iterator vtr = itr->second.begin(); vtr != itr->second.end(); ++vtr)
				{
					ss.rdbuf()->str("");
					ss << "INSERT INTO playeritems VALUES(";
					new_item_guid = objmgr.GenerateLowGuid(HIGHGUID_TYPE_ITEM);
					ss << new_guid << ","
						<< new_item_guid << ","
						<< (*vtr).entry << ","
						<< (*vtr).wrapped_item_id << ","
						<< (*vtr).wrapped_creator << ","
						<< (*vtr).creator << ","
						<< (*vtr).count << ","
						<< (*vtr).charges << ","
						<< (*vtr).flags << ","
						<< (*vtr).randomprop << ","
						<< (*vtr).randomsuffix << ","
						<< (*vtr).itemtext << ","
						<< (*vtr).durability << ","
						<< (*vtr).containerslot << ","
						<< (*vtr).slot << ",'"
						<< (*vtr).enchantments << "')";
					CharacterDatabase.FWaitExecute(ss.str().c_str(),con);
				}
			}
			ss.rdbuf()->str("");
		} while(result->NextRow());
		has_results = true;
		delete result;
		result = NULL;
	}

	// Clear all the data in the tables.
	if(has_results)
	{
		CharacterDatabase.FWaitExecute("DELETE FROM characters_insert_queue", con);
		CharacterDatabase.FWaitExecute("DELETE FROM playeritems_insert_queue", con);
	}
}

void World::DisconnectUsersWithAccount(const char * account, WorldSession * m_session)
{
	SessionMap::iterator itr;
	WorldSession * session;
	m_sessionlock.AcquireReadLock();
	bool FoundUser = false;
	for(itr = m_sessions.begin(); itr != m_sessions.end();)
	{
		session = itr->second;
		++itr;

		if(!stricmp(account, session->GetAccountNameS()))
		{
			FoundUser = true;
			m_session->SystemMessage("Disconnecting user with account `%s` IP `%s` Player `%s`.", session->GetAccountNameS(), 
				session->GetSocket() ? session->GetSocket()->GetRemoteIP().c_str() : "noip", session->GetPlayer() ? session->GetPlayer()->GetName() : "noplayer");

			if( session->GetPlayer() )
				session->GetPlayer()->SoftDisconnect();
		}
	}
	m_sessionlock.ReleaseReadLock();
	if ( FoundUser == false )
		m_session->SystemMessage("There is nobody online with account [%s]",account);
}

void World::DisconnectUsersWithIP(const char * ip, WorldSession * m_session)
{
	SessionMap::iterator itr;
	WorldSession * session;
	m_sessionlock.AcquireReadLock();
	bool FoundUser = false;
	for(itr = m_sessions.begin(); itr != m_sessions.end();)
	{
		session = itr->second;
		++itr;

		if(!session->GetSocket())
			continue;

		string ip2 = session->GetSocket()->GetRemoteIP().c_str();
		if(!stricmp(ip, ip2.c_str()))
		{
			FoundUser = true;
			m_session->SystemMessage("Disconnecting user with account `%s` IP `%s` Player `%s`.", session->GetAccountNameS(), 
				ip2.c_str(), session->GetPlayer() ? session->GetPlayer()->GetName() : "noplayer");

			if( session->GetPlayer() )
				session->GetPlayer()->SoftDisconnect();
		}
	}
	if ( FoundUser == false )
		m_session->SystemMessage("There is nobody online with ip [%s]",ip);
	m_sessionlock.ReleaseReadLock();
}

void World::DisconnectUsersWithPlayerName(const char * plr, WorldSession * m_session)
{
	SessionMap::iterator itr;
	WorldSession * session;
	m_sessionlock.AcquireReadLock();
	bool FoundUser = false;
	for(itr = m_sessions.begin(); itr != m_sessions.end();)
	{
		session = itr->second;
		++itr;

		if(!session->GetPlayer())
			continue;

		if(!stricmp(plr, session->GetPlayer()->GetName()))
		{
			m_session->SystemMessage("Disconnecting user with account `%s` IP `%s` Player `%s`.", session->GetAccountNameS(), 
				session->GetSocket() ? session->GetSocket()->GetRemoteIP().c_str() : "noip", session->GetPlayer() ? session->GetPlayer()->GetName() : "noplayer");

			session->GetPlayer()->SoftDisconnect();
		}
	}
	if (FoundUser == false)
		m_session->SystemMessage("There is no body online with the name [%s]",plr);
	m_sessionlock.ReleaseReadLock();
}

string World::GetUptimeString()
{
	char str[300];
	time_t pTime = (time_t)UNIXTIME - m_StartTime;
	tm * tmv = gmtime(&pTime);

	snprintf(str, 300, "%u days, %u hours, %u minutes, %u seconds.", tmv->tm_yday, tmv->tm_hour, tmv->tm_min, tmv->tm_sec);
	return string(str);
}

void World::GenerateStaticUpdateMasks()
{
	for(int i=0;i<TYPEID_UNUSED;i++)
		create_update_for_other_masks[i] = NULL;
	/////////////////////////////////////////////////////////////////////
	create_update_for_other_masks[ TYPEID_OBJECT ] = new UpdateMask;
	create_update_for_other_masks[ TYPEID_OBJECT ]->SetCount( OBJECT_END );
	create_update_for_other_masks[ TYPEID_OBJECT ]->SetBit( OBJECT_FIELD_GUID );
	create_update_for_other_masks[ TYPEID_OBJECT ]->SetBit( OBJECT_FIELD_GUID_01 );
	create_update_for_other_masks[ TYPEID_OBJECT ]->SetBit( OBJECT_FIELD_TYPE );
	create_update_for_other_masks[ TYPEID_OBJECT ]->SetBit( OBJECT_FIELD_ENTRY );
	create_update_for_other_masks[ TYPEID_OBJECT ]->SetBit( OBJECT_FIELD_SCALE_X ); 
	create_update_for_other_masks[ TYPEID_OBJECT ]->SetBit( OBJECT_FIELD_DATA );
	create_update_for_other_masks[ TYPEID_OBJECT ]->SetBit( OBJECT_FIELD_DATA_1 );
	/////////////////////////////////////////////////////////////////////
	create_update_for_other_masks[ TYPEID_ITEM ] = new UpdateMask;
	create_update_for_other_masks[ TYPEID_ITEM ]->SetCount( ITEM_END );
	create_update_for_other_masks[ TYPEID_ITEM ]->Inherit( create_update_for_other_masks[ TYPEID_OBJECT ] );
	create_update_for_other_masks[ TYPEID_ITEM ]->SetBit( ITEM_FIELD_OWNER );
	create_update_for_other_masks[ TYPEID_ITEM ]->SetBit( ITEM_FIELD_CONTAINED );
	create_update_for_other_masks[ TYPEID_ITEM ]->SetBit( ITEM_FIELD_CREATOR );
	create_update_for_other_masks[ TYPEID_ITEM ]->SetBit( ITEM_FIELD_GIFTCREATOR );
	create_update_for_other_masks[ TYPEID_ITEM ]->SetBit( ITEM_FIELD_FLAGS );
	create_update_for_other_masks[ TYPEID_ITEM ]->SetBit( ITEM_FIELD_ENCHANTMENT_1_1 );
	create_update_for_other_masks[ TYPEID_ITEM ]->SetBit( ITEM_FIELD_ENCHANTMENT_1_2 );
	create_update_for_other_masks[ TYPEID_ITEM ]->SetBit( ITEM_FIELD_ENCHANTMENT_1_3 );
	create_update_for_other_masks[ TYPEID_ITEM ]->SetBit( ITEM_FIELD_ENCHANTMENT_2_1 );
	create_update_for_other_masks[ TYPEID_ITEM ]->SetBit( ITEM_FIELD_ENCHANTMENT_2_3 );
	create_update_for_other_masks[ TYPEID_ITEM ]->SetBit( ITEM_FIELD_ENCHANTMENT_3_1 );
	create_update_for_other_masks[ TYPEID_ITEM ]->SetBit( ITEM_FIELD_ENCHANTMENT_3_3 );
	create_update_for_other_masks[ TYPEID_ITEM ]->SetBit( ITEM_FIELD_ENCHANTMENT_4_1 );
	create_update_for_other_masks[ TYPEID_ITEM ]->SetBit( ITEM_FIELD_ENCHANTMENT_4_3 );
	create_update_for_other_masks[ TYPEID_ITEM ]->SetBit( ITEM_FIELD_ENCHANTMENT_5_1 );
	create_update_for_other_masks[ TYPEID_ITEM ]->SetBit( ITEM_FIELD_ENCHANTMENT_5_3 );
	create_update_for_other_masks[ TYPEID_ITEM ]->SetBit( ITEM_FIELD_ENCHANTMENT_6_1 );
	create_update_for_other_masks[ TYPEID_ITEM ]->SetBit( ITEM_FIELD_ENCHANTMENT_6_3 );
	create_update_for_other_masks[ TYPEID_ITEM ]->SetBit( ITEM_FIELD_ENCHANTMENT_7_1 );
	create_update_for_other_masks[ TYPEID_ITEM ]->SetBit( ITEM_FIELD_ENCHANTMENT_7_3 );
	create_update_for_other_masks[ TYPEID_ITEM ]->SetBit( ITEM_FIELD_ENCHANTMENT_8_1 );
	create_update_for_other_masks[ TYPEID_ITEM ]->SetBit( ITEM_FIELD_ENCHANTMENT_8_3 );
	create_update_for_other_masks[ TYPEID_ITEM ]->SetBit( ITEM_FIELD_ENCHANTMENT_9_1 );
	create_update_for_other_masks[ TYPEID_ITEM ]->SetBit( ITEM_FIELD_ENCHANTMENT_9_3 );
	create_update_for_other_masks[ TYPEID_ITEM ]->SetBit( ITEM_FIELD_ENCHANTMENT_10_1 );
	create_update_for_other_masks[ TYPEID_ITEM ]->SetBit( ITEM_FIELD_ENCHANTMENT_10_3 );
	create_update_for_other_masks[ TYPEID_ITEM ]->SetBit( ITEM_FIELD_ENCHANTMENT_11_1 );
	create_update_for_other_masks[ TYPEID_ITEM ]->SetBit( ITEM_FIELD_ENCHANTMENT_11_3 );
	create_update_for_other_masks[ TYPEID_ITEM ]->SetBit( ITEM_FIELD_ENCHANTMENT_12_1 );
	create_update_for_other_masks[ TYPEID_ITEM ]->SetBit( ITEM_FIELD_ENCHANTMENT_12_3 );
	create_update_for_other_masks[ TYPEID_ITEM ]->SetBit( ITEM_FIELD_PROPERTY_SEED );
	create_update_for_other_masks[ TYPEID_ITEM ]->SetBit( ITEM_FIELD_RANDOM_PROPERTIES_ID );
	/////////////////////////////////////////////////////////////////////
	create_update_for_other_masks[ TYPEID_CONTAINER ] = new UpdateMask;
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetCount( CONTAINER_END );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->Inherit( create_update_for_other_masks[ TYPEID_ITEM ] );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_NUM_SLOTS );
/*	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_1 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_2 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_3 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_4 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_5 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_6 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_7 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_8 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_9 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_10 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_11 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_12 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_13 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_14 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_15 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_16 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_17 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_18 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_19 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_20 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_21 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_22 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_23 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_24 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_25 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_26 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_27 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_28 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_29 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_30 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_31 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_32 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_33 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_34 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_35 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_36 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_37 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_38 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_39 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_40 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_41 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_42 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_43 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_44 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_45 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_46 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_47 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_48 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_49 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_50 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_51 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_52 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_53 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_54 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_55 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_56 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_57 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_58 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_59 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_60 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_61 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_62 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_63 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_64 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_65 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_66 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_67 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_68 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_69 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_70 );
	create_update_for_other_masks[ TYPEID_CONTAINER ]->SetBit( CONTAINER_FIELD_SLOT_1_71 ); */
	/////////////////////////////////////////////////////////////////////
	create_update_for_other_masks[ TYPEID_UNIT ] = new UpdateMask;
	create_update_for_other_masks[ TYPEID_UNIT ]->SetCount( UNIT_END );
	create_update_for_other_masks[ TYPEID_UNIT ]->Inherit( create_update_for_other_masks[ TYPEID_OBJECT ] );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_CHARM );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_CHARM + 1);
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_SUMMON );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_SUMMON + 1);
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_CHARMEDBY );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_CHARMEDBY + 1);
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_SUMMONEDBY );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_SUMMONEDBY + 1);
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_CREATEDBY );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_CREATEDBY + 1);
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_TARGET );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_TARGET + 1);
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_CHANNEL_OBJECT );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_CHANNEL_OBJECT + 1);
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_BYTES_0 );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_HEALTH );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_POWER1 );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_POWER2 );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_POWER3 );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_POWER4 );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_POWER5 );
//	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_POWER6 );
//	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_POWER7 );
//	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_POWER8 );
//	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_POWER9 );
//	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_POWER10 );
//	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_POWER11 );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_MAXHEALTH );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_MAXPOWER1 );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_MAXPOWER2 );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_MAXPOWER3 );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_MAXPOWER4 );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_MAXPOWER5 );
//	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_MAXPOWER6 );
//	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_MAXPOWER7 );
//	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_MAXPOWER8 );
//	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_MAXPOWER9 );
//	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_MAXPOWER10 );
//	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_MAXPOWER11 );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_LEVEL );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_FACTIONTEMPLATE );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_VIRTUAL_ITEM_SLOT_ID );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_VIRTUAL_ITEM_SLOT_ID_1 );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_VIRTUAL_ITEM_SLOT_ID_2 );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_FLAGS );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_FLAGS_2 );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_AURASTATE );
//	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_BASEATTACKTIME );
//	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_BASEATTACKTIME_1 );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_BOUNDINGRADIUS );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_COMBATREACH );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_DISPLAYID );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_NATIVEDISPLAYID );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_MOUNTDISPLAYID );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_BYTES_1 );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_PETNUMBER );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_PET_NAME_TIMESTAMP );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_DYNAMIC_FLAGS );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_CHANNEL_SPELL );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_MOD_CAST_SPEED );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_CREATED_BY_SPELL );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_NPC_FLAGS );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_NPC_EMOTESTATE );
//	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_BASE_MANA );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_BYTES_2 );
	create_update_for_other_masks[ TYPEID_UNIT ]->SetBit( UNIT_FIELD_HOVERHEIGHT ); 
	/////////////////////////////////////////////////////////////////////
	create_update_for_other_masks[ TYPEID_PLAYER ] = new UpdateMask;
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetCount( PLAYER_END );
/*	create_update_for_other_masks[ TYPEID_PLAYER ]->Inherit( create_update_for_other_masks[ TYPEID_UNIT ] );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_DUEL_ARBITER );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_FLAGS );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_GUILDID );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_GUILDRANK );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_BYTES );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_BYTES_2 );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_BYTES_3 );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_DUEL_TEAM );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_GUILD_TIMESTAMP );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_1_ENTRYID );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_1_ENCHANTMENT );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_2_ENTRYID );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_2_ENCHANTMENT );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_3_ENTRYID );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_3_ENCHANTMENT );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_4_ENTRYID );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_4_ENCHANTMENT );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_5_ENTRYID );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_5_ENCHANTMENT );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_6_ENTRYID );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_6_ENCHANTMENT );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_7_ENTRYID );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_7_ENCHANTMENT );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_8_ENTRYID );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_8_ENCHANTMENT );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_9_ENTRYID );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_9_ENCHANTMENT );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_10_ENTRYID );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_10_ENCHANTMENT );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_11_ENTRYID );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_11_ENCHANTMENT );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_12_ENTRYID );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_12_ENCHANTMENT );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_13_ENTRYID );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_13_ENCHANTMENT );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_14_ENTRYID );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_14_ENCHANTMENT );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_15_ENTRYID );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_15_ENCHANTMENT );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_16_ENTRYID );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_16_ENCHANTMENT );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_17_ENTRYID );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_17_ENCHANTMENT );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_18_ENTRYID );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_18_ENCHANTMENT );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_19_ENTRYID );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_19_ENCHANTMENT );
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_CHOSEN_TITLE ); 
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_FAKE_INEBRIATION ); */
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(OBJECT_FIELD_GUID);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(OBJECT_FIELD_TYPE);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(OBJECT_FIELD_SCALE_X);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(OBJECT_FIELD_DATA);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(OBJECT_FIELD_DATA_1);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_SUMMON);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_SUMMON+1);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_TARGET);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_TARGET+1);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_HEALTH);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_POWER1);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_POWER2);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_POWER3);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_POWER4);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_POWER5);
//	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_POWER7);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_MAXHEALTH);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_MAXPOWER1);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_MAXPOWER2);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_MAXPOWER3);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_MAXPOWER4);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_MAXPOWER5);
//	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_MAXPOWER7);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_LEVEL);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_FACTIONTEMPLATE);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_BYTES_0);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_FLAGS);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_FLAGS_2);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_AURASTATE);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_BASEATTACKTIME);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_BASEATTACKTIME+1);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_BOUNDINGRADIUS);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_COMBATREACH);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_DISPLAYID);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_NATIVEDISPLAYID);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_MOUNTDISPLAYID);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_BYTES_1);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_MOUNTDISPLAYID);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_PETNUMBER);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_PET_NAME_TIMESTAMP);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_CHANNEL_OBJECT);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_CHANNEL_OBJECT+1);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_CHANNEL_SPELL);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_DYNAMIC_FLAGS);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_NPC_FLAGS);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_NPC_EMOTESTATE);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_HOVERHEIGHT);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(PLAYER_FLAGS);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(PLAYER_BYTES);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(PLAYER_BYTES_2);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(PLAYER_BYTES_3);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(PLAYER_GUILD_TIMESTAMP);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(PLAYER_DUEL_TEAM);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(PLAYER_DUEL_ARBITER);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(PLAYER_DUEL_ARBITER+1);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(PLAYER_GUILDRANK);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(PLAYER_CHOSEN_TITLE);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(UNIT_FIELD_BYTES_2);
	create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit(PLAYER_FIELD_BYTES2);
	for(uint16 i = 0; i < EQUIPMENT_SLOT_END; i++)
	{
		create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_1_ENTRYID + 2*i ); // visual items for other players
		create_update_for_other_masks[ TYPEID_PLAYER ]->SetBit( PLAYER_VISIBLE_ITEM_1_ENCHANTMENT + 2*i ); // visual items for other players
	}
	/////////////////////////////////////////////////////////////////////
	create_update_for_other_masks[ TYPEID_GAMEOBJECT ] = new UpdateMask;
	create_update_for_other_masks[ TYPEID_GAMEOBJECT ]->SetCount( GAMEOBJECT_END );
	create_update_for_other_masks[ TYPEID_GAMEOBJECT ]->Inherit( create_update_for_other_masks[ TYPEID_OBJECT ] );
	create_update_for_other_masks[ TYPEID_GAMEOBJECT ]->SetBit( OBJECT_FIELD_CREATED_BY );
	create_update_for_other_masks[ TYPEID_GAMEOBJECT ]->SetBit( GAMEOBJECT_DISPLAYID );
	create_update_for_other_masks[ TYPEID_GAMEOBJECT ]->SetBit( GAMEOBJECT_FLAGS );
	create_update_for_other_masks[ TYPEID_GAMEOBJECT ]->SetBit( GAMEOBJECT_PARENTROTATION );
	create_update_for_other_masks[ TYPEID_GAMEOBJECT ]->SetBit( GAMEOBJECT_PARENTROTATION_1 );
	create_update_for_other_masks[ TYPEID_GAMEOBJECT ]->SetBit( GAMEOBJECT_PARENTROTATION_2 );
	create_update_for_other_masks[ TYPEID_GAMEOBJECT ]->SetBit( GAMEOBJECT_PARENTROTATION_3 );
	create_update_for_other_masks[ TYPEID_GAMEOBJECT ]->SetBit( GAMEOBJECT_DYNAMIC );
	create_update_for_other_masks[ TYPEID_GAMEOBJECT ]->SetBit( GAMEOBJECT_FACTION );
	create_update_for_other_masks[ TYPEID_GAMEOBJECT ]->SetBit( GAMEOBJECT_LEVEL );
	create_update_for_other_masks[ TYPEID_GAMEOBJECT ]->SetBit( GAMEOBJECT_BYTES_1 );
	/////////////////////////////////////////////////////////////////////
	create_update_for_other_masks[ TYPEID_DYNAMICOBJECT ] = new UpdateMask;
	create_update_for_other_masks[ TYPEID_DYNAMICOBJECT ]->SetCount( DYNAMICOBJECT_END );
	create_update_for_other_masks[ TYPEID_DYNAMICOBJECT ]->Inherit( create_update_for_other_masks[ TYPEID_OBJECT ] );
	create_update_for_other_masks[ TYPEID_DYNAMICOBJECT ]->SetBit( DYNAMICOBJECT_CASTER );
	create_update_for_other_masks[ TYPEID_DYNAMICOBJECT ]->SetBit( DYNAMICOBJECT_BYTES );
	create_update_for_other_masks[ TYPEID_DYNAMICOBJECT ]->SetBit( DYNAMICOBJECT_SPELLID );
	create_update_for_other_masks[ TYPEID_DYNAMICOBJECT ]->SetBit( DYNAMICOBJECT_RADIUS );
	create_update_for_other_masks[ TYPEID_DYNAMICOBJECT ]->SetBit( DYNAMICOBJECT_CASTTIME ); 
	/////////////////////////////////////////////////////////////////////
	create_update_for_other_masks[ TYPEID_CORPSE ] = new UpdateMask;
	create_update_for_other_masks[ TYPEID_CORPSE ]->SetCount( CORPSE_END );
	create_update_for_other_masks[ TYPEID_CORPSE ]->Inherit( create_update_for_other_masks[ TYPEID_OBJECT ] );
	create_update_for_other_masks[ TYPEID_CORPSE ]->SetBit( CORPSE_FIELD_OWNER );
	create_update_for_other_masks[ TYPEID_CORPSE ]->SetBit( CORPSE_FIELD_OWNER_1 );
	create_update_for_other_masks[ TYPEID_CORPSE ]->SetBit( CORPSE_FIELD_PARTY );
	create_update_for_other_masks[ TYPEID_CORPSE ]->SetBit( CORPSE_FIELD_PARTY_1 );
	create_update_for_other_masks[ TYPEID_CORPSE ]->SetBit( CORPSE_FIELD_DISPLAY_ID );
	create_update_for_other_masks[ TYPEID_CORPSE ]->SetBit( CORPSE_FIELD_ITEM );
	create_update_for_other_masks[ TYPEID_CORPSE ]->SetBit( CORPSE_FIELD_ITEM_1 );
	create_update_for_other_masks[ TYPEID_CORPSE ]->SetBit( CORPSE_FIELD_ITEM_2 );
	create_update_for_other_masks[ TYPEID_CORPSE ]->SetBit( CORPSE_FIELD_ITEM_3 );
	create_update_for_other_masks[ TYPEID_CORPSE ]->SetBit( CORPSE_FIELD_ITEM_4 );
	create_update_for_other_masks[ TYPEID_CORPSE ]->SetBit( CORPSE_FIELD_ITEM_5 );
	create_update_for_other_masks[ TYPEID_CORPSE ]->SetBit( CORPSE_FIELD_ITEM_6 );
	create_update_for_other_masks[ TYPEID_CORPSE ]->SetBit( CORPSE_FIELD_ITEM_7 );
	create_update_for_other_masks[ TYPEID_CORPSE ]->SetBit( CORPSE_FIELD_ITEM_8 );
	create_update_for_other_masks[ TYPEID_CORPSE ]->SetBit( CORPSE_FIELD_ITEM_9 );
	create_update_for_other_masks[ TYPEID_CORPSE ]->SetBit( CORPSE_FIELD_ITEM_10 );
	create_update_for_other_masks[ TYPEID_CORPSE ]->SetBit( CORPSE_FIELD_ITEM_11 );
	create_update_for_other_masks[ TYPEID_CORPSE ]->SetBit( CORPSE_FIELD_ITEM_12 );
	create_update_for_other_masks[ TYPEID_CORPSE ]->SetBit( CORPSE_FIELD_ITEM_13 );
	create_update_for_other_masks[ TYPEID_CORPSE ]->SetBit( CORPSE_FIELD_ITEM_14 );
	create_update_for_other_masks[ TYPEID_CORPSE ]->SetBit( CORPSE_FIELD_ITEM_15 );
	create_update_for_other_masks[ TYPEID_CORPSE ]->SetBit( CORPSE_FIELD_ITEM_16 );
	create_update_for_other_masks[ TYPEID_CORPSE ]->SetBit( CORPSE_FIELD_ITEM_17 );
	create_update_for_other_masks[ TYPEID_CORPSE ]->SetBit( CORPSE_FIELD_ITEM_18 );
	create_update_for_other_masks[ TYPEID_CORPSE ]->SetBit( CORPSE_FIELD_BYTES_1 );
	create_update_for_other_masks[ TYPEID_CORPSE ]->SetBit( CORPSE_FIELD_BYTES_2 );
#ifndef CATACLYSM_SUPPORT
	create_update_for_other_masks[ TYPEID_CORPSE ]->SetBit( CORPSE_FIELD_GUILD );
#endif
	create_update_for_other_masks[ TYPEID_CORPSE ]->SetBit( CORPSE_FIELD_FLAGS ); 
	create_update_for_other_masks[ TYPEID_CORPSE ]->SetBit( CORPSE_FIELD_DYNAMIC_FLAGS ); 
}