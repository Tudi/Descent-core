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
#include <svn_revision.h>
#include "ConsoleCommands.h"

bool HandleInfoCommand(BaseConsole * pConsole, int argc, const char * argv[])
{
	uint32 clientsNum = (uint32)sWorld.GetSessionCount();

	int gm = 0;
	int count = 0;
	int avg = 0;
	PlayerStorageMap::const_iterator itr;
	objmgr._playerslock.AcquireReadLock();
	for (itr = objmgr._players.begin(); itr != objmgr._players.end(); itr++)
	{
		if( itr->second->GetSession() )
		{
			count++;
			avg += itr->second->GetSession()->GetLatency();
			if(itr->second->GetSession()->GetPermissionCount())
				gm++;
		}			
	}
	objmgr._playerslock.ReleaseReadLock();

	float AvgLatency;
	int ActiveMapCount, ActiveCreatureCount, ActiveGamobjects, ActiveCorpses;
	sInstanceMgr.GetServerStats( AvgLatency, ActiveMapCount, ActiveCreatureCount, ActiveGamobjects, ActiveCorpses );

#ifdef EXTRACT_REVISION_NUMBER 
	EXTRACT_REVISION_NUMBER();
#endif
	pConsole->Write("======================================================================\r\n");
	pConsole->Write("Server Information: \r\n");
	pConsole->Write("======================================================================\r\n");
	pConsole->Write("Server Revision: ArcEmu r%u/%s-%s-%s (www.arcemu.org)\r\n", BUILD_REVISION, CONFIG, PLATFORM_TEXT, ARCH);
	pConsole->Write("Server Uptime: %s\r\n", sWorld.GetUptimeString().c_str());
	pConsole->Write("Current Players: %d (%d GMs, %d queued)\r\n", clientsNum, gm,  0);
	pConsole->Write("Average Client Latency: %.3fms\r\n", count ?  ((float)((float)avg / (float)count)) : 0.0f);
	pConsole->Write("Average Server Latency: %.3fms\r\n", AvgLatency );
	pConsole->Write("Active Instances: %u\r\n", ActiveMapCount );
	pConsole->Write("Active Creatures: %u\r\n", ActiveCreatureCount );
	pConsole->Write("Active GameObjects: %u\r\n", ActiveGamobjects );
	pConsole->Write("Active Corpses: %u\r\n", ActiveCorpses );
	pConsole->Write("Delayed Deletes: %u\r\n", sGarbageCollection.size() );
	pConsole->Write("Pooled memory MB: %.4f ( %.4f unused )\r\n", ( ItemPool.GetPoolSizeMB() + AuraPool.GetPoolSizeMB() + SpellPool.GetPoolSizeMB() ) / 1024, ( ItemPool.GetPoolUnusedSizeMB() + AuraPool.GetPoolUnusedSizeMB() + SpellPool.GetPoolUnusedSizeMB() ) / 1024 );
	pConsole->Write("Active Thread Count: %u\r\n", ThreadPool.GetActiveThreadCount());
	pConsole->Write("Free Thread Count: %u\r\n", ThreadPool.GetFreeThreadCount());
	pConsole->Write("SQL Query Cache Size (World): %u queries delayed\r\n", WorldDatabase.GetQueueSize());
	pConsole->Write("SQL Query Cache Size (Character): %u queries delayed\r\n", CharacterDatabase.GetQueueSize());
	pConsole->Write("======================================================================\r\n\r\n");

	return true;
}

bool HandleGMsCommand(BaseConsole * pConsole, int argc, const char * argv[])
{
	WorldPacket data;
	//bool first = true;

	pConsole->Write("There are the following GM's online on this server: \r\n");
	pConsole->Write("======================================================\r\n");
	pConsole->Write("| %21s | %15s | % 03s  |\r\n" , "Name", "Permissions", "Latency");
	pConsole->Write("======================================================\r\n");

	PlayerStorageMap::const_iterator itr;
	objmgr._playerslock.AcquireReadLock();
	for (itr = objmgr._players.begin(); itr != objmgr._players.end(); itr++)
	{
		if(itr->second->GetSession()->GetPermissionCount())
		{
			pConsole->Write("| %21s | %15s | %03u ms |\r\n" , itr->second->GetName(), itr->second->GetSession()->GetPermissions(), itr->second->GetSession()->GetLatency());
		}
	}
	objmgr._playerslock.ReleaseReadLock();

	pConsole->Write("======================================================\r\n\r\n");
	return true;
}


bool HandleOnlinePlayersCommand(BaseConsole * pConsole, int argc, const char * argv[])
{
	WorldPacket data;
	//bool first = true;

	pConsole->Write("There following players online on this server: \r\n");
	pConsole->Write("======================================================\r\n");
	pConsole->Write("| %21s | %15s | % 03s  |\r\n" , "Name", "Level", "Latency");
	pConsole->Write("======================================================\r\n");

	PlayerStorageMap::const_iterator itr;
	objmgr._playerslock.AcquireReadLock();
	for (itr = objmgr._players.begin(); itr != objmgr._players.end(); itr++)
	{
		pConsole->Write("| %21s | %15s | %03u ms |\r\n" , itr->second->GetName(), itr->second->GetSession()->GetPlayer()->getLevel(), itr->second->GetSession()->GetLatency());
	}
	objmgr._playerslock.ReleaseReadLock();

	pConsole->Write("======================================================\r\n\r\n");
	return true;
}

void ConcatArgs(string & outstr, int argc, int startoffset, const char * argv[])
{
	for(int i = startoffset + 1; i < argc; ++i)
	{
		outstr += argv[i];
		if((i+1) != (argc))
			outstr += " ";
	}
}
bool HandleAnnounceCommand(BaseConsole * pConsole, int argc, const char * argv[])
{
	char pAnnounce[1024];
	string outstr;
	if(argc < 2)
		return false;

	ConcatArgs(outstr, argc, 0, argv);
	snprintf(pAnnounce, 1024, "%sConsole: |r%s", MSG_COLOR_LIGHTBLUE, outstr.c_str());
	sWorld.SendWorldText(pAnnounce); // send message
	pConsole->Write("Message sent.\r\n");
	return true;
}

bool HandleWAnnounceCommand(BaseConsole * pConsole, int argc, const char * argv[])
{
	char pAnnounce[1024];
	string outstr;

	if(argc < 2)
		return false;

	ConcatArgs(outstr, argc, 0, argv);
	snprintf(pAnnounce, 1024, "%sConsole: |r%s", MSG_COLOR_LIGHTBLUE, outstr.c_str());
	sWorld.SendWorldWideScreenText(pAnnounce); // send message
	pConsole->Write("Message sent.\r\n");
	return true;
}
bool HandleWhisperCommand(BaseConsole * pConsole, int argc, const char * argv[])
{
	char pAnnounce[1024];
	Player * pPlayer;
	string outstr;

	if(argc < 3)
		return false;

	pPlayer = objmgr.GetPlayer(argv[1]);

	if( pPlayer == NULL )
	{
		pConsole->Write("Could not find player, %s.\r\n", argv[1]);
		return true;
	}

	ConcatArgs(outstr, argc, 1, argv);
	snprintf(pAnnounce, 1024, "%sConsole whisper: |r%s", MSG_COLOR_MAGENTA, outstr.c_str());

	pPlayer->BroadcastMessage(pAnnounce);
	pConsole->Write("Message sent to %s.\r\n", pPlayer->GetName());
	return true;
}

bool HandleKickCommand(BaseConsole * pConsole, int argc, const char * argv[])
{
	if(argc < 3)
		return false;

	char pAnnounce[1024];
	Player * pPlayer;
	string outstr;

	pPlayer = objmgr.GetPlayer(argv[1]);
	if( pPlayer == NULL )
	{
		pConsole->Write("Could not find player, %s.\r\n", argv[1]);
		return true;
	}
	ConcatArgs(outstr, argc, 1, argv);
	snprintf(pAnnounce, 1024, "%sConsole:|r %s was kicked from the server for: %s.", MSG_COLOR_LIGHTBLUE, pPlayer->GetName(), outstr.c_str());
	pPlayer->BroadcastMessage("You were kicked by the console for: %s", outstr.c_str());
	sWorld.SendWorldText(pAnnounce, NULL);
	pPlayer->Kick(5000);
	pConsole->Write("Kicked player %s.\r\n", pPlayer->GetName());
	return true;
}

bool HandleShutDownCommand(BaseConsole * pConsole, int argc, const char * argv[])
{
	uint32 delay = 5;
	if(argc >= 2)
	{
		if (strcmp(argv[1], "fast") == 0)
		{
			PlayerStorageMap::const_iterator itr;
			uint32 stime = now();
			uint32 count = 0;
			objmgr._playerslock.AcquireReadLock();
			for (itr = objmgr._players.begin(); itr != objmgr._players.end(); itr++)
			{
				if(itr->second->GetSession())
				{
					itr->second->SaveToDB(false);
					count++;
				}
			}
			objmgr._playerslock.ReleaseReadLock();

			exit(0);
		}
		else
		{
			delay = atoi(argv[1]);
		}
	}

    sMaster.m_ShutdownTimer = delay * 1000;
	sMaster.m_ShutdownEvent = true;
	pConsole->Write("Shutdown initiated.\r\n");
	return true;
}

bool HandleCancelCommand(BaseConsole * pConsole, int argc, const char * argv[])
{
	pConsole->Write("Shutdown canceled.\r\n");
	sMaster.m_ShutdownTimer = 5000;
	sMaster.m_ShutdownEvent = false;
	return true;
}

bool HandleBanAccountCommand(BaseConsole * pConsole, int argc, const char * argv[])
{
	if(argc < 3)
		return false;

	int32 timeperiod = GetTimePeriodFromString(argv[2]);
	if(timeperiod < 0)
		return false;

	uint32 banned = (timeperiod ? (uint32)UNIXTIME+timeperiod : 1);

	char emptystring = 0;
	char * pReason;
	if( argc == 4 )
		pReason = (char *)argv[3];
	else 
		pReason = &emptystring;

	/// apply instantly in db
	sLogonCommHandler.Account_SetBanned(argv[1], banned, pReason);

	pConsole->Write("Account '%s' has been banned %s%s. The change will be effective imediatly.\r\n", argv[1], 
		timeperiod ? "until " : "forever", timeperiod ? ConvertTimeStampToDataTime(timeperiod+(uint32)UNIXTIME).c_str() : "");
	
	return true;
}

bool HandleUnbanAccountCommand(BaseConsole * pConsole, int argc, const char * argv[])
{
	if(argc < 2)
		return false;

	sLogonCommHandler.Account_SetBanned(argv[1], 0, "");

	pConsole->Write("Account '%s' has been unbanned.\r\n", argv[1]);
	return true;
}

bool HandleCreateAccountCommand(BaseConsole * pConsole, int argc, const char * argv[])
{
	if(argc < 5)
		return false;

	const char * username = argv[1];
	const char * password = argv[2];
	const char * email = argv[3];
//	uint32 flags = atoi(argv[4]);

	if(strlen(username) == 0 || strlen(password) == 0 || strlen(email) == 0)
		return false;

	/*string susername = CharacterDatabase.EscapeString(string(username));
	string spassword = CharacterDatabase.EscapeString(string(password));
	string semail = CharacterDatabase.EscapeString(string(email));

	sLogonCommHandler.LogonDatabaseSQLExecute("INSERT INTO accounts (login, password, email, flags) VALUES('%s','%s','%s',%u)",susername.c_str(), spassword.c_str(),
		semail.c_str(), flags);

	pConsole->Write("Account created.\r\n");*/
	return true;
}

bool HandleMOTDCommand(BaseConsole * pConsole, int argc, const char * argv[])
{
	if(argc < 2)
	{
		pConsole->Write( "The current motd is: '%s'.\r\n", sWorld.GetMotd() );
	}else
	{
		char set_motd[1024];
		string outstr;
		ConcatArgs( outstr, argc, 0, argv );
		snprintf( set_motd, 1024, "%s", outstr.c_str() );

		sWorld.SetMotd( set_motd );
		pConsole->Write( "The motd has been set to: '%s'.\r\n", sWorld.GetMotd() );
	}
	return true;
}

bool HandlePlayerInfoCommand(BaseConsole * pConsole, int argc, const char * argv[])
{
	if(argc < 2)
		return false;

	Player * plr = objmgr.GetPlayer(argv[1]);
	if( plr == NULL )
	{
		pConsole->Write("Player not found.\r\n");
		return true;
	}

	pConsole->Write("Player: %s\r\n", plr->GetName());
	pConsole->Write("Race: %s\r\n", plr->myRace->name);
	pConsole->Write("Class: %s\r\n", plr->myClass->name);
	pConsole->Write("IP: %s\r\n", plr->GetSession()->GetSocket() ? plr->GetSession()->GetSocket()->GetRemoteIP().c_str() : "disconnected");
	pConsole->Write("Level: %u\r\n", plr->getLevel());
	pConsole->Write("Account: %s\r\n", plr->GetSession()->GetAccountNameS());
	return true;
}

void TestConsoleLogin(string& username, string& password, uint32 requestno)
{
	sLogonCommHandler.TestConsoleLogon(username, password, requestno);
}

bool HandleRevivePlayer(BaseConsole * pConsole, int argc, const char * argv[])
{
	if ( !argc )
		return false;

	Player *plr = objmgr.GetPlayer(argv[1], false);
	if(!plr)
	{
		pConsole->Write( "Could not find player %s.\r\n", argv[1]);
		return true;
	}

	if(plr->IsDead())
	{
		plr->RemoteRevive();
		pConsole->Write("Revived player %s.\r\n", argv[1]);
	} else
	{
		pConsole->Write("Player %s is not dead.\r\n", argv[1]);
	}
	return true;
}


bool HandleRehashCommand(BaseConsole * pConsole, int argc, const char * argv[])
{
	pConsole->Write("Config file re-parsed.");
	sWorld.Rehash(true);
	return true;
}

bool HandleNameHashCommand(BaseConsole * pConsole, int argc, const char * argv[])
{
	if( !argc )
		return false;
	string spname;
		ConcatArgs(spname, argc, 0, argv);
	pConsole->Write( "Name Hash for %s is 0x%X" , spname.c_str() , crc32((const unsigned char*)spname.c_str(), (unsigned int)spname.length()) );
	sWorld.Rehash(true);
	return true;
}

bool HandleMemInfoCommand(BaseConsole * pConsole, int argc, const char * argv[])
{
	pConsole->Write("======================================================================\r\n");
	pConsole->Write("Server Mem usage Information: \r\n");
	pConsole->Write("======================================================================\r\n");
	pConsole->Write("Sessions active: %u\r\n", sWorld.GetSessionCount());
	pConsole->Write("Corpses active: %u\r\n", ObjectMgr::getSingleton().m_corpses.size());
	pConsole->Write("Groups active: %u\r\n", ObjectMgr::getSingleton().GetGroupCount());
	pConsole->Write("Players active: %u\r\n", ObjectMgr::getSingleton()._players.size());
	pConsole->Write("Arenateams active: %u\r\n", ObjectMgr::getSingleton().m_arenaTeams.size());
	pConsole->Write("Guilds active: %u\r\n", ObjectMgr::getSingleton().GetGuildCount());
	pConsole->Write("Items active: %u\r\n", ItemPool.GetPooledObjectCount());
	pConsole->Write("Auras active: %u\r\n", AuraPool.GetPooledObjectCount());
	pConsole->Write("Spells active: %u\r\n", SpellPool.GetPooledObjectCount());

	uint32 i,instance_count=0,map_count=0;
	InstanceMap::iterator itr;
	for(i = 0; i < NUM_MAPS; ++i)
	{
		if(sInstanceMgr.GetInstanceList()[i] != NULL)
			instance_count += (uint32)sInstanceMgr.GetInstanceList()[i]->size();

		if(sInstanceMgr.GetSingleMapList()[i] != NULL)
			map_count++;

		if(sInstanceMgr.GetMapList()[i] != NULL)
			map_count++;
	}
	pConsole->Write("Instances active: %u\r\n", instance_count);
	pConsole->Write("Maps active: %u\r\n", map_count);
	pConsole->Write("======================================================================\r\n");
	return true;
}

bool HandleModPlayerLevel(BaseConsole * pConsole, int argc, const char * argv[])
{
	if ( argc < 3 )
	{
		pConsole->Write( "ERROR:Not enough parameters.\r\n");
		return false;
	}

	const char	*PlayerName = argv[1];
	uint32	LevelCount = atoi( argv[2] );

	Player *plr = objmgr.GetPlayer(PlayerName, false);
	if( !plr || plr->IsInWorld() == false )
	{
		pConsole->Write( "ERROR:Could not find player %s.\r\n", argv[1]);
		return true;
	}

	for( uint32 i=0;i<LevelCount; i++)
		plr->GiveXP( plr->GetUInt32Value(PLAYER_NEXT_LEVEL_XP) + 10, plr->GetGUID(), false );

	plr->SaveToDB(false);
	plr->BroadcastMessage("You have been leveled(%u) by a GM command.Your character has been saved",LevelCount);
	pConsole->Write( "SUCCESS:Player has been leveled\r\n" );

	FILE *fp = fopen("ConsoleCommands.Log","at");
	if( fp )
	{
		fprintf(fp,"GUID = %u, name = %s, acct = %u has been leveled = %u times at %I64u\n",(uint32)plr->GetGUID(),plr->GetName(), plr->GetSession()->GetAccountId(),LevelCount, UNIXTIME);
		fclose( fp );
	}

	return true;
}

bool HandleModPlayerTalents(BaseConsole * pConsole, int argc, const char * argv[])
{
	if ( argc < 3 )
	{
		pConsole->Write( "ERROR:Not enough parameters.\r\n");
		return false;
	}

	const char	*PlayerName = argv[1];
	uint32	TalentCount = atoi( argv[2] );

	Player *plr = objmgr.GetPlayer(PlayerName, false);
	if( !plr || plr->IsInWorld() == false )
	{
		pConsole->Write( "ERROR:Could not find player %s.\r\n", argv[1]);
		return true;
	}

	plr->m_Talent_point_mods += TalentCount;
	plr->m_Talent_point_mods = MAX( 0, plr->m_Talent_point_mods );
	int32 tp = (int32) plr->CalcTalentPointsShouldHaveMax() - plr->CalcTalentPointsHaveSpent( plr->m_talentActiveSpec );
	if( tp < 0 )
		tp = 0;
	plr->SetUInt32Value( PLAYER_CHARACTER_POINTS, tp );
	plr->smsg_TalentsInfo( );

	plr->SaveToDB(false);
	plr->BroadcastMessage("You talentpoint count has been moded(%u) by a GM command.Your character has been saved",TalentCount);
	pConsole->Write( "SUCCESS:Player has received extra talentpoints\r\n" );
	FILE *fp = fopen("ConsoleCommands.Log","at");
	if( fp )
	{
		fprintf(fp,"GUID = %u, name = %s, acct = %u has received extra talents = %u at %I64u \n",(uint32)plr->GetGUID(),plr->GetName(), plr->GetSession()->GetAccountId(),TalentCount, UNIXTIME);
		fclose( fp );
	}
	return true;
}

bool HandleModPlayerProfessions(BaseConsole * pConsole, int argc, const char * argv[])
{
	if ( argc < 3 )
	{
		pConsole->Write( "ERROR:Not enough parameters.\r\n");
		return false;
	}

	const char	*PlayerName = argv[1];
	uint32	skilline = atoi( argv[2] );

	Player *plr = objmgr.GetPlayer(PlayerName, false);
	if( !plr || plr->IsInWorld() == false )
	{
		pConsole->Write( "ERROR:Could not find player %s.\r\n", argv[1]);
		return true;
	}

	bool can_learn = false;
	if( plr->GetUInt32Value( PLAYER_PROFESSION_SKILL_LINE_1 ) == skilline )
		can_learn = true;
	if( plr->GetUInt32Value( PLAYER_PROFESSION_SKILL_LINE_2 ) == skilline )
		can_learn = true;
	if( plr->GetUInt32Value( PLAYER_PROFESSION_SKILL_LINE_1 ) == 0 || plr->GetUInt32Value( PLAYER_PROFESSION_SKILL_LINE_2 ) == 0 )
	{
		can_learn = true;
		plr->LearnProfession( skilline );
	}

	if( can_learn == false )
	{
		pConsole->Write( "ERROR:Cannot learn more primary professions %s.\r\n", argv[1]);
		return true;
	}

	plr->_RemoveSkillLine(skilline);
	plr->RemoveSpellsFromLine(skilline);
	plr->_AddSkillLine(skilline, 525, 525);
	plr->LearnAllSpellsFromSkillLine( skilline, true, false );

	plr->SaveToDB(false);
	plr->BroadcastMessage("You have learned a new profession by a GM command.Your character has been saved");
	plr->BroadcastMessage("In case you have client User Interface issue, please use Taxi->Player tools and fixes->fix profession");
	pConsole->Write( "SUCCESS:Player has learned the profession\r\n" );
	FILE *fp = fopen("ConsoleCommands.Log","at");
	if( fp )
	{
		fprintf(fp,"GUID = %u, name = %s, acct = %u has learned profession = %u at %I64u \n",(uint32)plr->GetGUID(),plr->GetName(), plr->GetSession()->GetAccountId(),skilline, UNIXTIME);
		fclose( fp );
	}

	return true;
}

bool HandleModPlayerProfessionsSecondary(BaseConsole * pConsole, int argc, const char * argv[])
{
	if ( argc < 3 )
	{
		pConsole->Write( "ERROR:Not enough parameters.\r\n");
		return false;
	}

	const char	*PlayerName = argv[1];
	uint32	skilline = atoi( argv[2] );

	Player *plr = objmgr.GetPlayer(PlayerName, false);
	if( !plr || plr->IsInWorld() == false )
	{
		pConsole->Write( "ERROR:Could not find player %s.\r\n", argv[1]);
		return true;
	}

	plr->_RemoveSkillLine(skilline);
	plr->RemoveSpellsFromLine(skilline);
	plr->_AddSkillLine(skilline, 525, 525);
	plr->LearnAllSpellsFromSkillLine( skilline, true, false );

	plr->SaveToDB(false);
	plr->BroadcastMessage("You have learned a new profession by a GM command.Your character has been saved");
	plr->BroadcastMessage("In case you have client User Interface issue, please use Taxi->Player tools and fixes->fix profession");
	pConsole->Write( "SUCCESS:Player has learned the profession\r\n" );
	FILE *fp = fopen("ConsoleCommands.Log","at");
	if( fp )
	{
		fprintf(fp,"GUID = %u, name = %s, acct = %u has learned profession = %u at %I64u \n",(uint32)plr->GetGUID(),plr->GetName(), plr->GetSession()->GetAccountId(),skilline, UNIXTIME);
		fclose( fp );
	}

	return true;
}

bool HandlePlayerSave(BaseConsole * pConsole, int argc, const char * argv[])
{
	if ( argc < 2 )
	{
		pConsole->Write( "ERROR:Not enough parameters.\r\n");
		return false;
	}

	const char	*PlayerName = argv[1];

	Player *plr = objmgr.GetPlayer(PlayerName, false);
	if( !plr || plr->IsInWorld() == false )
	{
		pConsole->Write( "ERROR:Could not find player %s.\r\n", argv[1]);
		return true;
	}

	plr->SaveToDB(false);
	pConsole->Write( "SUCCESS:Player has been saved\r\n" );

	return true;
}

bool HandlePlayerAddItem(BaseConsole * pConsole, int argc, const char * argv[])
{
	if ( argc < 4 )
	{
		pConsole->Write( "ERROR:Not enough parameters.\r\n");
		return false;
	}

	const char	*PlayerName = argv[1];
	uint32	itemid = atoi( argv[2] );
	uint32	itemCount = atoi( argv[3] );

	Player *plr = objmgr.GetPlayer(PlayerName, false);
	if( !plr || plr->IsInWorld() == false )
	{
		pConsole->Write( "ERROR:Could not find player %s.\r\n", argv[1]);
		return true;
	}

	ItemPrototype *m_itemProto = ItemPrototypeStorage.LookupEntry( itemid );
	if( m_itemProto == NULL )
	{
		pConsole->Write( "ERROR:Could not find item.\r\n" );
		return true;
	}

	itemCount = MAX( 1, itemCount );	//morron protection :P
	uint32 max_stack = MAX( 1, m_itemProto->MaxCount );
	if( itemCount < max_stack )
		max_stack = itemCount;
	for( uint32 i=0; i< ( itemCount + max_stack - 1) / max_stack; i++ )	//roundup
	{
		Item *item = objmgr.CreateItem( itemid, plr );
		if( item == NULL )
		{
			pConsole->Write( "ERROR:Could not create new item.\r\n" );
			return true;
		}

		uint32 added_until_now = i*max_stack;
		uint32 need_to_add = itemCount - added_until_now;
		uint32 add_now = MAX(1, MIN( need_to_add, max_stack ));
		FILE *fp = fopen("ConsoleCommands.Log","at");
		if( fp )
		{
			fprintf(fp,"GUID = %u, name = %s, acct = %u has received item = %u count = %u guid = %u at %I64u \n",(uint32)plr->GetGUID(),plr->GetName(), plr->GetSession()->GetAccountId(),itemid, add_now,(uint32)item->GetGUID(), UNIXTIME);
			fclose( fp );
		}
		item->SetUInt32Value(ITEM_FIELD_STACK_COUNT, add_now );
		if( plr->GetItemInterface()->SafeAddItem(item, INVENTORY_SLOT_NOT_SET, INVENTORY_SLOT_NOT_SET ) == ADD_ITEM_RESULT_ERROR )
		{
			plr->RemoveAndMailItem( &item );
			plr->BroadcastMessage("You have received a new item from a GM command in mail.");
		}
		else
		{
			plr->BroadcastMessage("You have received a new item from a GM command in your backpack. Your item has been saved");
		}

	}
	plr->SaveToDB(false);

	pConsole->Write( "SUCCESS:Player has received the item\r\n" );

	return true;
}

bool HandleModPlayerGold(BaseConsole * pConsole, int argc, const char * argv[])
{
	if ( argc < 3 )
	{
		pConsole->Write( "ERROR:Not enough parameters.\r\n");
		return false;
	}

	const char	*PlayerName = argv[1];
	uint32	gold = atoi( argv[2] );

	Player *plr = objmgr.GetPlayer(PlayerName, false);
	if( !plr || plr->IsInWorld() == false )
	{
		pConsole->Write( "ERROR:Could not find player %s.\r\n", argv[1]);
		return true;
	}

	plr->ModGold( gold );
	plr->SaveToDB(false);

	plr->BroadcastMessage("You have received gold from a GM command.Your character has been saved");
	pConsole->Write( "SUCCESS:Player has received gold\r\n" );

	FILE *fp = fopen("ConsoleCommands.Log","at");
	if( fp )
	{
		fprintf(fp,"GUID = %u, name = %s, acct = %u has received gold = %u at %I64u \n",(uint32)plr->GetGUID(),plr->GetName(), plr->GetSession()->GetAccountId(),gold, UNIXTIME);
		fclose( fp );
	}
	return true;
}

bool HandlePlayerAddspell(BaseConsole * pConsole, int argc, const char * argv[])
{
	if ( argc < 3 )
	{
		pConsole->Write( "ERROR:Not enough parameters.\r\n");
		return false;
	}

	const char	*PlayerName = argv[1];
	uint32	spellid = atoi( argv[2] );

	Player *plr = objmgr.GetPlayer(PlayerName, false);
	if( !plr || plr->IsInWorld() == false )
	{
		pConsole->Write( "ERROR:Could not find player %s.\r\n", argv[1]);
		return true;
	}

	plr->addSpell( spellid, false );
	plr->SaveToDB(false);

	plr->BroadcastMessage("You have learned a new spell from a GM command.Your character has been saved");
	pConsole->Write( "SUCCESS:Player has learned spell\r\n" );

	FILE *fp = fopen("ConsoleCommands.Log","at");
	if( fp )
	{
		fprintf(fp,"GUID = %u, name = %s, acct = %u has learned spell = %u at %I64u \n",(uint32)plr->GetGUID(),plr->GetName(), plr->GetSession()->GetAccountId(),spellid, UNIXTIME);
		fclose( fp );
	}

	return true;
}

bool HandlePlayerCastSpell(BaseConsole * pConsole, int argc, const char * argv[])
{
	if ( argc < 3 )
	{
		pConsole->Write( "ERROR:Not enough parameters.\r\n");
		return false;
	}

	const char	*PlayerName = argv[1];
	uint32	spellid = atoi( argv[2] );

	Player *plr = objmgr.GetPlayer(PlayerName, false);
	if( !plr || plr->IsInWorld() == false )
	{
		pConsole->Write( "ERROR:Could not find player %s.\r\n", argv[1]);
		return true;
	}

	plr->CastSpell( plr, spellid, true );
	plr->SaveToDB(false);

	plr->BroadcastMessage("You have learned a new spell from a GM command.Your character has been saved");
	pConsole->Write( "SUCCESS:Player has learned spell\r\n" );
	FILE *fp = fopen("ConsoleCommands.Log","at");
	if( fp )
	{
		fprintf(fp,"GUID = %u, name = %s, acct = %u has learned spell = %u at %I64u \n",(uint32)plr->GetGUID(),plr->GetName(), plr->GetSession()->GetAccountId(),spellid, UNIXTIME);
		fclose( fp );
	}
	return true;
}

bool HandlePlayerAddTitle(BaseConsole * pConsole, int argc, const char * argv[])
{
	if ( argc < 3 )
	{
		pConsole->Write( "ERROR:Not enough parameters.\r\n");
		return false;
	}

	const char	*PlayerName = argv[1];
	uint32	Title = atoi( argv[2] );

	Player *plr = objmgr.GetPlayer(PlayerName, false);
	if( !plr || plr->IsInWorld() == false )
	{
		pConsole->Write( "ERROR:Could not find player %s.\r\n", argv[1]);
		return true;
	}

	plr->SetKnownTitle( static_cast< RankTitles >(Title), true );
	plr->SaveToDB(false);

	plr->BroadcastMessage("You have gained a new Title from a GM command.Your character has been saved");
	pConsole->Write( "SUCCESS:Player has learned spell\r\n" );

	FILE *fp = fopen("ConsoleCommands.Log","at");
	if( fp )
	{
		fprintf(fp,"GUID = %u, name = %s, acct = %u has earned title = %u at %I64u\n",(uint32)plr->GetGUID(),plr->GetName(), plr->GetSession()->GetAccountId(),Title, UNIXTIME );
		fclose( fp );
	}

	return true;
}