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

initialiseSingleton( ObjectMgr );

const char * NormalTalkMessage = "What can I teach you, $N?";

ObjectMgr::ObjectMgr()
{
	m_hiPetGuid = 1;
	m_hiContainerGuid = 1;
	m_hiItemGuid = 1;
	m_hiGroupId = 1;
	m_mailid = 1;
	m_hiPlayerGuid = 1;
	m_hiCorpseGuid = 1;
	m_hiTransportGuid = 1;
//	m_hiVehicleGuid = 1;
	m_hiArenaTeamId=0;
	m_hiGuildId=1;
	m_ticketid = 1;
	memset(&m_InstanceBossInfoMap[0] , 0, sizeof(InstanceBossInfoMap*) * NUM_MAPS);
}


ObjectMgr::~ObjectMgr()
{
	Log.Notice("ObjectMgr", "Deleting Totem Spells...");
	TotemSpellMap::iterator tsitr;
	for (tsitr = m_totemSpells.begin(); tsitr != m_totemSpells.end();)
	{
		TotemSpells *c = tsitr->second;
		++tsitr;
		delete c;	
	}
	m_totemSpells.clear();

	Log.Notice("ObjectMgr", "Deleting Corpses...");
	CorpseMap::iterator itr;
	for (itr = m_corpses.begin(); itr != m_corpses.end();)
	{
		Corpse *c = itr->second;
		++itr;
		delete c;	//will callback function that will change this list!
	}
	m_corpses.clear();

	Log.Notice("ObjectMgr", "Deleting Itemsets...");
	for(ItemSetContentMap::iterator i = mItemSets.begin(); i != mItemSets.end(); ++i)
	{
		delete i->second;
		i->second = NULL;
	}
	mItemSets.clear();

	Log.Notice("ObjectMgr", "Deleting PlayerCreateInfo...");
	for( PlayerCreateInfoMap::iterator i = mPlayerCreateInfo.begin( ); i != mPlayerCreateInfo.end( ); ++ i )
	{
		delete i->second;
		i->second = NULL;
	}
	mPlayerCreateInfo.clear( );

	Log.Notice("ObjectMgr", "Deleting Guilds...");
	for ( GuildMap::iterator i = mGuild.begin(); i != mGuild.end(); ++i ) 
	{
		delete i->second;
		i->second = NULL;
	}

	Log.Notice("ObjectMgr", "Deleting Vendors...");
	for( VendorMap::iterator i = mVendors.begin( ); i != mVendors.end( ); ++ i ) 
	{
		delete i->second;
		i->second = NULL;
	}

	Log.Notice("ObjectMgr", "Deleting Spell Override...");
	for(OverrideIdMap::iterator i = mOverrideIdMap.begin(); i != mOverrideIdMap.end(); ++i)
	{
		delete i->second;
		i->second = NULL;
	}

	Log.Notice("ObjectMgr", "Deleting Trainers...");
	for( TrainerMap::iterator i = mTrainers.begin( ); i != mTrainers.end( ); ++ i)
	{
		Trainer * t = i->second;
		if(t->UIMessage && t->UIMessage != (char*)NormalTalkMessage)
		{
			delete [] t->UIMessage;
			t->UIMessage = NULL;
		}
		t->Spells.DestroyContent();
		delete t;
		t = NULL;
	}

/*	Log.Notice("ObjectMgr", "Deleting Level Information...");
	for( LevelInfoMap::iterator i = mLevelInfo.begin(); i != mLevelInfo.end(); ++i)
	{
		LevelMap * l = i->second;
		for(LevelMap::iterator i2 = l->begin(); i2 != l->end(); ++i2)
		{
			delete i2->second;
			i2->second = NULL;
		}
		l->clear();
		delete l;
		l = NULL;
	}*/

	Log.Notice("ObjectMgr", "Deleting Waypoint Cache...");
	for(HM_NAMESPACE::hash_map<uint32, WayPointVect*>::iterator i = m_waypoints.begin(); i != m_waypoints.end(); ++i)
	{
		i->second->DestroyContent();
		delete i->second;
		i->second = NULL;
	}

	Log.Notice("ObjectMgr", "Deleting timed emote Cache...");
	for(HM_NAMESPACE::hash_map<uint32, TimedEmoteList*>::iterator i = m_timedemotes.begin(); i != m_timedemotes.end(); ++i)
	{
		for(TimedEmoteList::iterator i2 = i->second->begin(); i2 != i->second->end(); ++i2)
			if((*i2))
			{
				delete [] (*i2)->msg;
				(*i2)->msg = NULL;
				delete (*i2); 
				(*i2) = NULL;
			}

		delete i->second;
		i->second = NULL;
	}

	Log.Notice("ObjectMgr", "Deleting NPC Say Texts...");
	for(uint32 i = 0 ; i < NUM_MONSTER_SAY_EVENTS ; ++i)
	{
		NpcMonsterSay * p;
		for(MonsterSayMap::iterator itr = mMonsterSays[i].begin(); itr != mMonsterSays[i].end(); ++itr)
		{
			p = itr->second;
			for(uint32 j = 0; j < p->TextCount; ++j)
				free((char*)p->Texts[j]);
			delete [] p->Texts;
			p->Texts = NULL;
			free((char*)p->MonsterName);
			delete p;
			itr->second = NULL;
		}

		mMonsterSays[i].clear();
	}

	Log.Notice("ObjectMgr", "Deleting Charters...");
	for(int i = 0; i < NUM_CHARTER_TYPES; ++i)
	{
		for(HM_NAMESPACE::hash_map<uint32, Charter*>::iterator itr =  m_charters[i].begin(); itr != m_charters[i].end(); ++itr)
		{
			delete itr->second;
			itr->second = NULL;
		}
	}

	Log.Notice("ObjectMgr", "Deleting Reputation Tables...");
	for(ReputationModMap::iterator itr = this->m_reputation_creature.begin(); itr != m_reputation_creature.end(); ++itr)
	{
		ReputationModifier * mod = itr->second;
		mod->mods.clear();
		delete mod;
		mod = NULL;
	}
	for(ReputationModMap::iterator itr = this->m_reputation_faction.begin(); itr != m_reputation_faction.end(); ++itr)
	{
		ReputationModifier * mod = itr->second;
		mod->mods.clear();
		delete mod;
		mod = NULL;
	}
	for(HM_NAMESPACE::hash_map<uint32, SimpleVect<ReputationToReputationMod *> *>::iterator itr = m_reputation_chains.begin(); itr != m_reputation_chains.end(); ++itr)
	{
		SimpleVect<ReputationToReputationMod *> *list = itr->second;
		list->DestroyContent();	//delete the pointers
		delete list;
	}
	m_reputation_chains.clear();

	for(HM_NAMESPACE::hash_map<uint32,InstanceReputationModifier*>::iterator itr = this->m_reputation_instance.begin(); itr != this->m_reputation_instance.end(); ++itr)
	{
		InstanceReputationModifier * mod = itr->second;
		mod->mods.clear();
		delete mod;
		mod = NULL;
	}

	Log.Notice("ObjectMgr", "Deleting Groups...");
	for(GroupMap::iterator itr = m_groups.begin(); itr != m_groups.end();)
	{
		Group* pGroup = itr->second;
		++itr;

		if( pGroup != NULL )
		{
			for( uint32 i = 0; i < pGroup->GetSubGroupCount(); ++i )
			{
				SubGroup* pSubGroup = pGroup->GetSubGroup( i );
				if( pSubGroup != NULL )
				{
					pSubGroup->Disband();
				}
			}
			delete pGroup;
		}
	}
	m_groups.clear();

	Log.Notice("ObjectMgr", "Deleting Player Information...");
	for(HM_NAMESPACE::hash_map<uint32, PlayerInfo*>::iterator itr = m_playersinfo.begin(); itr != m_playersinfo.end(); ++itr)
	{
		itr->second->m_Group=NULL;
		free(itr->second->name);
		delete itr->second;
		itr->second = NULL;
	}

	Log.Notice("ObjectMgr", "Deleting GM Tickets...");
	for(GmTicketList::iterator itr = GM_TicketList.begin(); itr != GM_TicketList.end(); ++itr)
	{
		delete (*itr);
		(*itr) = NULL;
	}

	Log.Notice("ObjectMgr", "Deleting Boss Information...");
	for(int i=0; i<NUM_MAPS; i++)
	{
		if(this->m_InstanceBossInfoMap[i] != NULL)
		{
			for(InstanceBossInfoMap::iterator itr = this->m_InstanceBossInfoMap[i]->begin(); itr != m_InstanceBossInfoMap[i]->end(); ++itr)
			{
				delete (*itr).second;
				(*itr).second = NULL;
			}
			delete this->m_InstanceBossInfoMap[i];
			this->m_InstanceBossInfoMap[i] = NULL;
		}
	}
	Log.Notice("ObjectMgr", "Deleting Arena teams...");
	HM_NAMESPACE::hash_map<uint32, ArenaTeam*>::iterator arenaitr;
	for( arenaitr = m_arenaTeams.begin(); arenaitr != m_arenaTeams.end(); arenaitr++)
	{
		delete arenaitr->second;
		arenaitr->second = NULL;
	}
	m_arenaTeams.clear();
	
	Log.Notice("ObjectMgr", "Deleting Profession Discoveries...");
	std::set<ProfessionDiscovery*>::iterator pditr = ProfessionDiscoveryTable.begin();
	for ( ; pditr != ProfessionDiscoveryTable.end(); pditr++ )
		delete (*pditr);

	Log.Notice("ObjectMgr", "Deleting Transporters...");
	TransportMap::iterator titr = mTransports.begin();
	for ( ; titr != mTransports.end(); titr++ )
		delete titr->second;
	mTransports.clear();

	Log.Notice("ObjectMgr", "Deleting Loots...");
	DestroyLootList( &m_reference_loot );
	DestroyLootList( &m_creature_loot_kill );
	DestroyLootList( &m_creature_loot_skin );
	DestroyLootList( &m_creature_loot_pickpocket );
	for( uint32 i = 0; i < m_currency_loot.GetMaxSize(); i++)
	{
		SimpleVect<LootCurencyStoreStruct*>	*loot_list = m_currency_loot.GetValue( i );
		loot_list->DestroyContent();
	}
	m_currency_loot.DestroyContent();	
}

//
// Groups
//

Group * ObjectMgr::GetGroupByLeader(Player* pPlayer)
{
	GroupMap::iterator itr;
	Group * ret=NULL;
	m_groupLock.AcquireReadLock();
	for(itr = m_groups.begin(); itr != m_groups.end(); ++itr)
	{
		if(itr->second->GetLeader()==pPlayer->m_playerInfo)
		{
			ret = itr->second;
			break;
		}
	}

	m_groupLock.ReleaseReadLock();
	return ret;
}

Group * ObjectMgr::GetGroupById(uint32 id)
{
	GroupMap::iterator itr;
	Group * ret=NULL;
	m_groupLock.AcquireReadLock();
	itr = m_groups.find(id);
	if(itr!=m_groups.end())
		ret=itr->second;

	m_groupLock.ReleaseReadLock();
	return ret;
}

//
// Player names
//
void ObjectMgr::DeletePlayerInfo( uint32 guid )
{
	PlayerInfo * pl;
	HM_NAMESPACE::hash_map<uint32,PlayerInfo*>::iterator i;
	PlayerNameStringIndexMap::iterator i2;
	playernamelock.AcquireWriteLock();
	i=m_playersinfo.find(guid);
	if(i==m_playersinfo.end())
	{
		playernamelock.ReleaseWriteLock();
		return;
	}
	
	pl=i->second;
	if(pl->m_Group)
	{
		pl->m_Group->RemovePlayer(pl);
		pl->m_Group = NULL;
	}

	if(pl->guild)
	{
		if(pl->guild->GetGuildLeader()==pl->guid)
			pl->guild->Disband();
		else
			pl->guild->RemoveGuildMember(pl,NULL);
	}

	string pnam = string(pl->name);
	arcemu_TOLOWER(pnam);
	i2 = m_playersInfoByName.find(pnam);
	if( i2 != m_playersInfoByName.end() && i2->second == pl )
		m_playersInfoByName.erase( i2 );

	free(pl->name);
	delete i->second;
	i->second = NULL;
	m_playersinfo.erase(i);

	playernamelock.ReleaseWriteLock();
}

PlayerInfo *ObjectMgr::GetPlayerInfo(const uint64 &guid )
{
	HM_NAMESPACE::hash_map<uint32,PlayerInfo*>::iterator i;
	PlayerInfo * rv;
	playernamelock.AcquireReadLock();
	i=m_playersinfo.find( (uint32) guid);
	if(i!=m_playersinfo.end())
		rv = i->second;
	else
		rv = NULL;
	playernamelock.ReleaseReadLock();
	return rv;
}

void ObjectMgr::AddPlayerInfo(PlayerInfo *pn)
{
	playernamelock.AcquireWriteLock();
	m_playersinfo[pn->guid] =  pn ;
	string pnam = string(pn->name);
	arcemu_TOLOWER(pnam);
	m_playersInfoByName[pnam] = pn;
	playernamelock.ReleaseWriteLock();
}

void ObjectMgr::RenamePlayerInfo(PlayerInfo * pn, const char * oldname, const char * newname)
{
	playernamelock.AcquireWriteLock();
	string oldn = string(oldname);
	arcemu_TOLOWER(oldn);

	PlayerNameStringIndexMap::iterator itr = m_playersInfoByName.find( oldn );
	if( itr != m_playersInfoByName.end() && itr->second == pn )
	{
		string newn = string(newname);
		arcemu_TOLOWER(newn);
		m_playersInfoByName.erase( itr );
		m_playersInfoByName[newn] = pn;
	}

	playernamelock.ReleaseWriteLock();
}

void ObjectMgr::LoadPlayersInfo()
{
	PlayerInfo * pn;
	QueryResult *result = CharacterDatabase.Query("SELECT guid,name,race,class,level,gender,zoneId,timestamp,acct FROM characters");
	uint32 period, c;
	if(result)
	{
		period = (result->GetRowCount() / 20) + 1;
		c = 0;
		do
		{
			Field *fields = result->Fetch();
			pn=new PlayerInfo;
			pn->guid = fields[0].GetUInt32();
			pn->name = strdup_local(fields[1].GetString());
			pn->race = fields[2].GetUInt8();
			pn->_class = fields[3].GetUInt8();
			pn->lastLevel = fields[4].GetUInt32();
			pn->gender = fields[5].GetUInt8();
			pn->lastZone=fields[6].GetUInt32();
			pn->lastOnline=fields[7].GetUInt32();
			pn->acct = fields[8].GetUInt32();
			pn->m_Group=0;
			pn->subGroup=0;
			pn->m_loggedInPlayer=NULL;
			pn->guild=NULL;
			pn->guildRank=NULL;
			pn->guildMember=NULL;
#ifdef VOICE_CHAT
			pn->groupVoiceId = -1;
#endif
			if(pn->race==RACE_HUMAN||pn->race==RACE_DWARF||pn->race==RACE_GNOME||pn->race==RACE_NIGHTELF||pn->race==RACE_DRAENEI)
				pn->team = 0;
			else 
				pn->team = 1;
		  
			if( GetPlayerInfoByName(pn->name) != NULL )
			{
				// gotta rename him
				char temp[300];
				snprintf(temp, 300, "%s__%X__", pn->name, pn->guid);
				Log.Notice("ObjectMgr", "Renaming duplicate player %s to %s. (%u)", pn->name,temp,pn->guid);
				CharacterDatabase.WaitExecute("UPDATE characters SET name = \"%s\", forced_rename_pending = 1 WHERE guid = %u",
					CharacterDatabase.EscapeString(string(temp)).c_str(), pn->guid);

				free(pn->name);
				pn->name = strdup_local(temp);
			}

			string lpn=string(pn->name);
			arcemu_TOLOWER(lpn);
			m_playersInfoByName[lpn] = pn;

			//this is startup -> no need in lock -> don't use addplayerinfo
			 m_playersinfo[(uint32)pn->guid]=pn;

			 if( !((++c) % period) )
				 Log.Notice("PlayerInfo", "Done %u/%u, %u%% complete.", c, result->GetRowCount(), float2int32( (float(c) / float(result->GetRowCount()))*100.0f ));
		} while( result->NextRow() );

		delete result;
		result = NULL;
	}
	Log.Notice("ObjectMgr", "%u players loaded.", m_playersinfo.size());

	Log.Notice("ObjectMgr", "Loading Player instances.");
	// Raid & heroic Instance IDs
	// Must be done before entering world...
	QueryResult *result2 = CharacterDatabase.Query("SELECT `instanceid`, `mode`, `mapid`,`playerguid` FROM `instanceids`");
	if(result2)
	{
		PlayerInstanceMap::iterator itr;
		do 
		{
			uint32 instanceId = result2->Fetch()[0].GetUInt32();
			uint32 mode = result2->Fetch()[1].GetUInt32();
			uint32 mapId = result2->Fetch()[2].GetUInt32();
			uint32 playerguid = result2->Fetch()[3].GetUInt32();
			PlayerInfo *pn = m_playersinfo[ playerguid ] ;

			if( pn == NULL || mode >= NUM_INSTANCE_MODES || mapId >= NUM_MAPS )
			{
				//delete trash from db then continue
				CharacterDatabase.Query("delete from instanceids where `playerguid`=%u ",playerguid);
				continue;
			}
			pn->savedInstanceIdsLock.Acquire();
			itr = pn->savedInstanceIds[mode].find(mapId);
			if(itr == pn->savedInstanceIds[mode].end())
				pn->savedInstanceIds[mode].insert(PlayerInstanceMap::value_type(mapId, instanceId));
			else
				(*itr).second = instanceId;
			
			//TODO: Instances not loaded yet ~.~
			//if(!sInstanceMgr.InstanceExists(mapId, pn->m_savedInstanceIds[mapId][mode]))
			//{
			//	pn->m_savedInstanceIds[mapId][mode] = 0;
			//	CharacterDatabase.Execute("DELETE FROM `instanceids` WHERE `mapId` = %u AND `instanceId` = %u AND `mode` = %u", mapId, instanceId, mode);
			//}
			
			pn->savedInstanceIdsLock.Release();
		} while (result2->NextRow());
		delete result2;
		result2 = NULL;
	}
	LoadGuilds();
}

PlayerInfo* ObjectMgr::GetPlayerInfoByName(const char * name)
{
	string lpn=string(name);
	arcemu_TOLOWER(lpn);
	PlayerNameStringIndexMap::iterator i;
	PlayerInfo *rv = NULL;
	playernamelock.AcquireReadLock();

	i=m_playersInfoByName.find(lpn);
	if( i != m_playersInfoByName.end() )
		rv = i->second;

	playernamelock.ReleaseReadLock();
	return rv;
}

void ObjectMgr::LoadPlayerCreateInfo()
{
	QueryResult *result = WorldDatabase.Query( "SELECT * FROM playercreateinfo" );

	if( !result )
	{
		Log.Error("MySQL","Query failed: SELECT * FROM playercreateinfo");
		return;
	}

	PlayerCreateInfo *pPlayerCreateInfo;

	do
	{
		Field *fields = result->Fetch();

		pPlayerCreateInfo = new PlayerCreateInfo;
		memset( &pPlayerCreateInfo->stats, 0, sizeof( CreateInfo_levelBaseStats ) * PLAYER_LEVEL_CAP );

		pPlayerCreateInfo->index = fields[0].GetUInt8();
		pPlayerCreateInfo->race = fields[1].GetUInt8();
		pPlayerCreateInfo->factiontemplate = fields[2].GetUInt32();
		pPlayerCreateInfo->class_ = fields[3].GetUInt8();
		pPlayerCreateInfo->mapId = fields[4].GetUInt32();
//		pPlayerCreateInfo->zoneId = fields[5].GetUInt32();
		pPlayerCreateInfo->positionX = fields[6].GetFloat();
		pPlayerCreateInfo->positionY = fields[7].GetFloat();
		pPlayerCreateInfo->positionZ = fields[8].GetFloat();
		pPlayerCreateInfo->displayId = fields[9].GetUInt16();

		QueryResult *sk_sql = WorldDatabase.Query(
			"SELECT * FROM playercreateinfo_skills WHERE indexid=%u",pPlayerCreateInfo->index);

		if(sk_sql)
		{
			do 
			{
				Field *fields = sk_sql->Fetch();
				CreateInfo_SkillStruct tsk;
				tsk.skillid = fields[1].GetUInt32();
				tsk.currentval = fields[2].GetUInt32();
				tsk.maxval = fields[3].GetUInt32();
				pPlayerCreateInfo->skills.push_back(tsk);
			} while(sk_sql->NextRow());
			delete sk_sql;
			sk_sql = NULL;
		}
		QueryResult *sp_sql = WorldDatabase.Query(
			"SELECT * FROM playercreateinfo_spells WHERE indexid=%u",pPlayerCreateInfo->index);

		if(sp_sql)
		{
			do 
			{
				pPlayerCreateInfo->spell_list.insert(sp_sql->Fetch()[1].GetUInt32());
			} while(sp_sql->NextRow());
			delete sp_sql;
			sp_sql = NULL;
		}
	  
		QueryResult *items_sql = WorldDatabase.Query(
			"SELECT * FROM playercreateinfo_items WHERE indexid=%u",pPlayerCreateInfo->index);
		
		if(items_sql)
		{
			do 
			{
				Field *fields = items_sql->Fetch();
				CreateInfo_ItemStruct itm;
				itm.protoid = fields[1].GetUInt32();
				itm.slot = fields[2].GetUInt8();
				itm.amount = fields[3].GetUInt32();
				pPlayerCreateInfo->items.push_back(itm);
			} while(items_sql->NextRow());
		   delete items_sql;
		   items_sql = NULL;
		}

		QueryResult *bars_sql = WorldDatabase.Query(
			"SELECT * FROM playercreateinfo_bars WHERE class=%u",pPlayerCreateInfo->class_ );

		if(bars_sql)
		{
			do 
			{
				Field *fields = bars_sql->Fetch();
				CreateInfo_ActionBarStruct bar;
				bar.button = fields[2].GetUInt32();
				bar.action = fields[3].GetUInt32();
				bar.type = fields[4].GetUInt32();
				bar.misc = fields[5].GetUInt32();
				pPlayerCreateInfo->actionbars.push_back(bar);
			} while(bars_sql->NextRow());			
			delete bars_sql;
			bars_sql = NULL;
		}
	
		//stats
		QueryResult *stats_sql = WorldDatabase.Query(
			"SELECT * FROM playercreateinfo_levelstats WHERE race=%u and class=%u",pPlayerCreateInfo->race,pPlayerCreateInfo->class_);
		
		if(stats_sql)
		{
			do 
			{
				Field *fields = stats_sql->Fetch();
				uint32 level = fields[2].GetUInt32();

				if( level >= PLAYER_LEVEL_CAP )
					continue;

				pPlayerCreateInfo->stats[ level ].Stat[0] = fields[3].GetUInt32();
				pPlayerCreateInfo->stats[ level ].Stat[1] = fields[4].GetUInt32();
				pPlayerCreateInfo->stats[ level ].Stat[2] = fields[5].GetUInt32();
				pPlayerCreateInfo->stats[ level ].Stat[3] = fields[6].GetUInt32();
				pPlayerCreateInfo->stats[ level ].Stat[4] = fields[7].GetUInt32();
				if( level < MAX_PREDEFINED_NEXTLEVELXP )
					pPlayerCreateInfo->stats[ level ].XPToNextLevel = NextLevelXp[ level ];
				else
					pPlayerCreateInfo->stats[ level ].XPToNextLevel = NextLevelXp[ MAX_PREDEFINED_NEXTLEVELXP - 1 ];

			} while(stats_sql->NextRow());
		   delete stats_sql;
		   stats_sql = NULL;
		}

		mPlayerCreateInfo[pPlayerCreateInfo->index] = pPlayerCreateInfo;
	} while( result->NextRow() );

	delete result;
	result = NULL;

	//load playercreateinfo health and mana
	result = WorldDatabase.Query( "SELECT * FROM playercreateinfo_classlevelstats" );

	if( !result )
	{
		Log.Error("MySQL","Query failed: SELECT * FROM playercreateinfo_classlevelstats");
		return;
	}

	do
	{
		Field *fields = result->Fetch();
		uint32 class_ = fields[0].GetUInt32();
		uint32 level = fields[1].GetUInt32();

		if( level >= PLAYER_LEVEL_CAP )
			continue;

		uint32	base_hp = fields[2].GetUInt32();
		uint32	base_mana = fields[3].GetUInt32();

		static uint32 player_races[] = {RACE_HUMAN,RACE_ORC,RACE_DWARF,RACE_NIGHTELF,RACE_UNDEAD,RACE_TAUREN,RACE_GNOME,RACE_TROLL,RACE_GOBLIN,RACE_BLOODELF,RACE_DRAENEI,RACE_WORGEN,0};
		for( uint32 i=0;player_races[i] != 0 ;i++)
		{
			PlayerCreateInfo *pc = GetPlayerCreateInfo( player_races[i], class_ );
			if( pc )
			{
				pc->stats[ level ].HP = base_hp;
				pc->stats[ level ].Mana = base_mana;
			}
		}
	}while( result->NextRow() );

	delete result;
	result = NULL;

	//noob protection. In case not all level stats are filled out then we try to fill them out manualy
	PlayerCreateInfoMap::const_iterator itr;
	for (itr = mPlayerCreateInfo.begin(); itr != mPlayerCreateInfo.end(); itr++)
	{
		PlayerCreateInfo *pc = itr->second;
		int32 last_valid_level;
		for( last_valid_level = PLAYER_LEVEL_CAP - 1; last_valid_level > 0; last_valid_level-- )
			if( pc->stats[ last_valid_level ].HP != 0 && pc->stats[ last_valid_level ].Stat[0] != 0 && pc->stats[ last_valid_level ].Stat[1] != 0 )
				break;
		//we have enough values
		if( last_valid_level == PLAYER_LEVEL_CAP - 1 )
			continue;
		//we will use a liniar formula to fill out values
		int32 val_count = last_valid_level-1;
		int32 val_delta;
		float liniar_increase;

		val_delta = pc->stats[ last_valid_level ].HP - pc->stats[ 1 ].HP;
		liniar_increase = float( val_delta ) / float( val_count );
		for( uint32 i=last_valid_level+1;i<PLAYER_LEVEL_CAP;i++)
			if( pc->stats[ i ].HP == 0 ) //support for partial DB support
				pc->stats[ i ].HP = pc->stats[ 1 ].HP + float2int32( liniar_increase*i );

		val_delta = pc->stats[ last_valid_level ].Mana - pc->stats[ 1 ].Mana;
		liniar_increase = float( val_delta ) / float( val_count );
		for( uint32 i=last_valid_level+1;i<PLAYER_LEVEL_CAP;i++)
			if( pc->stats[ i ].Mana == 0 ) //support for partial DB support
				pc->stats[ i ].Mana = pc->stats[ 1 ].Mana + float2int32( liniar_increase*i );

		val_delta = pc->stats[ last_valid_level ].XPToNextLevel - pc->stats[ 1 ].XPToNextLevel;
		liniar_increase = float( val_delta ) / float( val_count );
		for( uint32 i=last_valid_level+1;i<PLAYER_LEVEL_CAP;i++)
		{
			if( i < MAX_PREDEFINED_NEXTLEVELXP )
				pc->stats[ i ].XPToNextLevel = NextLevelXp[ i ];
			else
				pc->stats[ i ].XPToNextLevel = pc->stats[ 1 ].XPToNextLevel + float2int32( liniar_increase*i );
		}

		for(uint32 stat=0;stat<5;stat++)
		{
			val_delta = pc->stats[ last_valid_level ].Stat[ stat ] - pc->stats[ 1 ].Stat[ stat ];
			liniar_increase = float( val_delta ) / float( val_count );
			for( uint32 i=last_valid_level+1;i<PLAYER_LEVEL_CAP;i++)
				if( pc->stats[ i ].Stat[ stat ] == 0 ) //support for partial DB support
					pc->stats[ i ].Stat[ stat ] = pc->stats[ 1 ].Stat[ stat ] + float2int32( liniar_increase*i );
		}
	}
}

// DK:LoadGuilds()
void ObjectMgr::LoadGuilds()
{
	QueryResult *result = CharacterDatabase.Query( "SELECT * FROM guilds" );
	if(result)
	{
		uint32 period = (result->GetRowCount() / 20) + 1;
		uint32 c = 0;
		do 
		{
			Guild * pGuild = Guild::Create();
			if(!pGuild->LoadFromDB(result->Fetch()))
			{
				delete pGuild;
				pGuild = NULL;
			}
			else
				mGuild[ pGuild->GetGuildId() ] = pGuild;

			if( !((++c) % period) )
				Log.Notice("Guilds", "Done %u/%u, %u%% complete.", c, result->GetRowCount(), float2int32( (float(c) / float(result->GetRowCount()))*100.0f ));

		} while(result->NextRow());
		delete result;
		result = NULL;
	}
	Log.Notice("ObjectMgr", "%u guilds loaded.", mGuild.size());
}

Corpse* ObjectMgr::LoadCorpse(uint32 guid)
{
	Corpse *pCorpse;
	QueryResult *result = CharacterDatabase.Query("SELECT * FROM Corpses WHERE guid =%u ", guid );

	if( !result )
	{ 
		return NULL;
	}
	
	do
	{
		Field *fields = result->Fetch();
		pCorpse = new Corpse(HIGHGUID_TYPE_CORPSE,fields[0].GetUInt32());
		pCorpse->SetPosition(fields[1].GetFloat(), fields[2].GetFloat(), fields[3].GetFloat(), fields[4].GetFloat());
		pCorpse->SetZoneId(fields[5].GetUInt32());
		pCorpse->SetMapId(fields[6].GetUInt32());
		pCorpse->LoadValues( fields[7].GetString());
		if(pCorpse->GetUInt32Value(CORPSE_FIELD_DISPLAY_ID) == 0)
		{
			delete pCorpse;
			pCorpse = NULL;
			continue;
		}

		pCorpse->SetLoadedFromDB(true);
		pCorpse->SetInstanceID(fields[8].GetUInt32());
		pCorpse->AddToWorld();
	 } while( result->NextRow() );

	delete result;
	result = NULL;

	return pCorpse;
}


//------------------------------------------------------
// Live corpse retreival.
// comments: I use the same tricky method to start from the last corpse instead of the first
//------------------------------------------------------
Corpse *ObjectMgr::GetCorpseByOwner(uint32 ownerguid)
{
	CorpseMap::const_iterator itr;
	Corpse *rv = NULL;
	_corpseslock.Acquire();
	for (itr = m_corpses.begin();itr != m_corpses.end(); ++itr)
	{
		if(itr->second->GetUInt32Value(CORPSE_FIELD_OWNER) == ownerguid)
		{
			rv = itr->second;
			break;
		}
	}
	_corpseslock.Release();


	return rv;
}

void ObjectMgr::LoadGMTickets()
{
	QueryResult *result = CharacterDatabase.Query( "SELECT `guid`, `playerGuid`, `name`, `level`, `map`, `posX`, `posY`, `posZ`, `message`, `timestamp`, `deleted`, `assignedto`, `comment` FROM gm_tickets WHERE deleted=0" );

	GM_Ticket *ticket;
	if(result == 0)
	{ 
		return;
	}

	do
	{
		Field *fields = result->Fetch();

		ticket = new GM_Ticket;
		ticket->guid = fields[0].GetUInt64();
		ticket->playerGuid = fields[1].GetUInt64();
		ticket->name = fields[2].GetString();
		ticket->level = fields[3].GetUInt32();
		ticket->map = fields[4].GetUInt32();
		ticket->posX = fields[5].GetFloat();
		ticket->posY = fields[6].GetFloat();
		ticket->posZ = fields[7].GetFloat();
		ticket->message = fields[8].GetString();
		ticket->timestamp = fields[9].GetUInt32();
		ticket->deleted_by = fields[10].GetUInt64();
		ticket->assignedToPlayer = fields[11].GetUInt64();
		ticket->comment = fields[12].GetString();

		AddGMTicket(ticket, true);

	} while( result->NextRow() );

	Log.Notice("ObjectMgr", "%u active GM Tickets loaded.", result->GetRowCount());
	delete result;
	result = NULL;
}

void ObjectMgr::LoadInstanceBossInfos()
{
	char *p, *q, *trash;
	MapInfo * mapInfo;
	QueryResult * result = WorldDatabase.Query("SELECT `mapid`, `creatureid`, `trash`, `trash_respawn_override` FROM `instance_bosses`");

	if(result == NULL)
	{ 
		return;
	}

	uint32 cnt = 0;
	do
	{
		InstanceBossInfo * bossInfo = new InstanceBossInfo();
		if( bossInfo == NULL )
			return;
		bossInfo->mapid = (uint32)result->Fetch()[0].GetUInt32();

		mapInfo = WorldMapInfoStorage.LookupEntry(bossInfo->mapid);
		if(mapInfo == NULL || IS_STATIC_MAP( mapInfo ) )
		{
			sLog.outDetail("Not loading boss information for map %u! (continent or unknown map)", bossInfo->mapid);
			delete bossInfo;
			bossInfo = NULL;
			continue;
		}
		if(bossInfo->mapid >= NUM_MAPS)
		{
			sLog.outDetail("Not loading boss information for map %u! (map id out of range)", bossInfo->mapid);
			delete bossInfo;
			bossInfo = NULL;
			continue;
		}

		bossInfo->creatureid = (uint32)result->Fetch()[1].GetUInt32();
		trash = strdup_local(result->Fetch()[2].GetString());
		q = trash;
		p = strchr(q, ' ');
		while(p)
		{
			*p = 0;
			uint32 val = atoi(q);
			if(val)
				bossInfo->trash.insert(val);
			q = p + 1;
			p = strchr(q, ' ');
		}
		free(trash);
		bossInfo->trashRespawnOverride = (uint32)result->Fetch()[3].GetUInt32();

		
		if(this->m_InstanceBossInfoMap[bossInfo->mapid] == NULL)
			this->m_InstanceBossInfoMap[bossInfo->mapid] = new InstanceBossInfoMap;
		this->m_InstanceBossInfoMap[bossInfo->mapid]->insert(InstanceBossInfoMap::value_type(bossInfo->creatureid, bossInfo));
		cnt++;
	} while(result->NextRow());

	delete result;
	result = NULL;

	Log.Notice("ObjectMgr", "%u boss information loaded.", cnt);
}

void ObjectMgr::SaveGMTicket(GM_Ticket* ticket, QueryBuffer * buf)
{
	std::stringstream ss;
	ss << "REPLACE INTO gm_tickets (`guid`, `playerGuid`, `name`, `level`, `map`, `posX`, `posY`, `posZ`, `message`, `timestamp`, `deleted`, `assignedto`, `comment`) VALUES(";
	ss << ticket->guid << ", ";
	ss << ticket->playerGuid << ", '";
	ss << CharacterDatabase.EscapeString(ticket->name) << "', ";
	ss << ticket->level << ", ";
	ss << ticket->map << ", ";
	ss << ticket->posX << ", ";
	ss << ticket->posY << ", ";
	ss << ticket->posZ << ", '";
	ss << CharacterDatabase.EscapeString(ticket->message) << "', ";
	ss << ticket->timestamp << ", ";
	ss << ticket->deleted_by << ", ";
	ss << ticket->assignedToPlayer << ", '";
	ss << CharacterDatabase.EscapeString(ticket->comment) << "');";

	if(buf == NULL)
		CharacterDatabase.ExecuteNA(ss.str( ).c_str( ));
	else
		buf->AddQueryStr(ss.str());
}

void ObjectMgr::SetHighestGuids()
{
	QueryResult *result;

	result = CharacterDatabase.Query( "SELECT MAX(guid) FROM characters" );
	if( result )
	{
		m_hiPlayerGuid = result->Fetch()[0].GetUInt32();
		delete result;
		result = NULL;
	}

	result = CharacterDatabase.Query("SELECT MAX(guid) FROM playeritems");
	if( result )
	{
		m_hiItemGuid = (uint32)result->Fetch()[0].GetUInt32();
		delete result;
		result = NULL;
	}

	result = CharacterDatabase.Query( "SELECT MAX(guid) FROM corpses" );
	if( result )
	{
		m_hiCorpseGuid = result->Fetch()[0].GetUInt32();
		delete result;
		result = NULL;
	}

	result = WorldDatabase.Query("SELECT MAX(id) FROM creature_spawns");
	if(result)
	{
		m_hiCreatureSpawnId = result->Fetch()[0].GetUInt32();
		delete result;
		result = NULL;
	}

	result = WorldDatabase.Query("SELECT MAX(id) FROM gameobject_spawns");
	if(result)
	{
		m_hiGameObjectSpawnId = result->Fetch()[0].GetUInt32();
		delete result;
		result = NULL;
	}

	result = CharacterDatabase.Query("SELECT MAX(group_id) FROM groups");
	if(result)
	{
		m_hiGroupId = result->Fetch()[0].GetUInt32();
		delete result;
		result = NULL;
	}

	result = CharacterDatabase.Query("SELECT MAX(charterId) FROM charters");
	if(result)
	{
		m_hiCharterId = result->Fetch()[0].GetUInt32();
		delete result;
		result = NULL;
	}

	result = CharacterDatabase.Query("SELECT MAX(guildId) FROM guilds");
	if(result)
	{
		m_hiGuildId = result->Fetch()[0].GetUInt32();
		delete result;
		result = NULL;
	}

	result = CharacterDatabase.Query("SELECT MAX(guid) FROM gm_tickets");
	if(result)
	{
		m_ticketid = result->Fetch()[0].GetUInt64() + 1;
		delete result;
		result = NULL;
	}

	result = CharacterDatabase.Query("SELECT MAX(setguid) FROM character_equipmentsets");
	if(result)
	{
		m_hiEquipmentSetId = result->Fetch()[0].GetUInt32() + 1;
		delete result;
		result = NULL;
	}

	Log.Notice("ObjectMgr", "HighGuid(CORPSE) = %u", m_hiCorpseGuid);
	Log.Notice("ObjectMgr", "HighGuid(PLAYER) = %u", m_hiPlayerGuid);
	Log.Notice("ObjectMgr", "HighGuid(GAMEOBJ) = %u", m_hiGameObjectSpawnId);
	Log.Notice("ObjectMgr", "HighGuid(UNIT) = %u", m_hiCreatureSpawnId);
	Log.Notice("ObjectMgr", "HighGuid(ITEM) = %u", m_hiItemGuid);
	Log.Notice("ObjectMgr", "HighGuid(CONTAINER) = %u", m_hiContainerGuid);
	Log.Notice("ObjectMgr", "HighGuid(GROUP) = %u", m_hiGroupId);
	Log.Notice("ObjectMgr", "HighGuid(CHARTER) = %u", m_hiCharterId);
	Log.Notice("ObjectMgr", "HighGuid(GUILD) = %u", m_hiGuildId);
	Log.Notice("ObjectMgr", "HighGuid(TICKET) = %u", m_ticketid - 1);
	Log.Notice("ObjectMgr", "HighGuid(EQUIPMENTSET) = %u", m_hiEquipmentSetId - 1);
}

uint64 ObjectMgr::GenerateTicketID()
{
	return m_ticketid++;
}

uint32 ObjectMgr::GenerateMailID()
{
	return m_mailid++;
}

//do not generate player guids that have "0x01" in guid. Obfuscation will remove these bytes from client and will send misleading guids
//get bytes and check them to not be equal to 1
uint32 CataclysmCompatibleIncreaseGuid( uint32 OldGuid ) 
{
	OldGuid++;

	if( ( OldGuid & 0x000000FF ) == 0x00000001 )
		OldGuid += 0x00000001;
	if( ( OldGuid & 0x0000FF00 ) == 0x00000100 )
		OldGuid += 0x00000100;
	if( ( OldGuid & 0x00FF0000 ) == 0x00010000 )
		OldGuid += 0x00010000;
	if( ( OldGuid & 0xFF000000 ) == 0x01000000 )
		OldGuid += 0x01000000;	

	return OldGuid;
}

//!!! pets have petnumber in their low guid !
uint32 ObjectMgr::GenerateLowGuid(uint32 guidhigh)
{
	ASSERT(guidhigh == HIGHGUID_TYPE_ITEM 
		|| guidhigh == HIGHGUID_TYPE_CONTAINER 
		|| guidhigh == HIGHGUID_TYPE_PLAYER
		|| guidhigh == HIGHGUID_TYPE_PET
		|| guidhigh == HIGHGUID_TYPE_TRANSPORTER
		);

	guidhigh = HIGHGUID_TYPE_MASK & guidhigh;

	uint32 ret;
	if(guidhigh == HIGHGUID_TYPE_ITEM)
	{
		m_guidGenMutex.Acquire();
		m_hiItemGuid = CataclysmCompatibleIncreaseGuid( m_hiItemGuid );
		ret = m_hiItemGuid;
		m_guidGenMutex.Release();
	}
	else if(guidhigh==HIGHGUID_TYPE_PLAYER)
	{
		m_guidGenMutex.Acquire();
		m_hiPlayerGuid = CataclysmCompatibleIncreaseGuid( m_hiPlayerGuid );
		ret = m_hiPlayerGuid;
		m_guidGenMutex.Release();
	}
	else if(guidhigh==HIGHGUID_TYPE_PET)
	{
		m_guidGenMutex.Acquire();
		m_hiPetGuid = CataclysmCompatibleIncreaseGuid( m_hiPetGuid );
		ret = m_hiPetGuid;
		m_guidGenMutex.Release();
	}
	else if(guidhigh==HIGHGUID_TYPE_TRANSPORTER)
	{
		m_guidGenMutex.Acquire();
		m_hiTransportGuid = CataclysmCompatibleIncreaseGuid( m_hiTransportGuid );
		ret = m_hiTransportGuid;
		m_guidGenMutex.Release();
	}
/*	else if(guidhigh==HIGHGUID_TYPE_VEHICLE)
	{
		m_guidGenMutex.Acquire();
		m_hiVehicleGuid = CataclysmCompatibleIncreaseGuid( m_hiVehicleGuid );
		ret = m_hiVehicleGuid;
		m_guidGenMutex.Release();
	}*/
	else{
		m_guidGenMutex.Acquire();
		m_hiContainerGuid = CataclysmCompatibleIncreaseGuid( m_hiContainerGuid );
		ret = m_hiContainerGuid;
		m_guidGenMutex.Release();
	}
	return ret;
}

void ObjectMgr::ProcessGameobjectQuests()
{
	/*if(!mGameObjectNames.size())
		return;

	int total = mGameObjectNames.size();
	std::set<Quest*> tmp;
	for(HM_NAMESPACE::hash_map<uint32, Quest*>::iterator itr = sQuestMgr.Begin(); itr != sQuestMgr.End(); ++itr)
	{
		Quest *qst = itr->second;
		if(qst->count_required_item > 0 || 
			qst->required_mobtype[0] == QUEST_MOB_TYPE_GAMEOBJECT ||
			qst->required_mobtype[1] == QUEST_MOB_TYPE_GAMEOBJECT ||
			qst->required_mobtype[2] == QUEST_MOB_TYPE_GAMEOBJECT ||
			qst->required_mobtype[3] == QUEST_MOB_TYPE_GAMEOBJECT )
		{
			tmp.insert(qst);
		}
	}
	std::vector<GameObjectInfo*> gos;
	gos.reserve(5000);
	for(GameObjectNameMap::iterator it0 = mGameObjectNames.begin(); it0 != mGameObjectNames.end(); ++it0)
	{
		GameObjectInfo *gon = it0->second;
		gos.push_back(it0->second);
	}

	int c = 0;
	total = gos.size();
	for(std::vector<GameObjectInfo*>::iterator it0 = gos.begin(); it0 != gos.end(); ++it0)
	{
		GameObjectInfo *gon = (*it0);

		map<uint32, std::set<uint32> >* golootlist = &(LootMgr::getSingleton().quest_loot_go);
		map<uint32, std::set<uint32> >::iterator it2 = golootlist->find(gon->ID);
		//// QUEST PARSING
		for(std::set<Quest*>::iterator itr = tmp.begin(); itr != tmp.end(); ++itr)
		{
			Quest *qst = *itr;
			int max = qst->count_required_mob;
			if(qst->count_required_item > max)
				max = qst->count_required_item;

			if(max > 0)
			{
				for(int i = 0; i < max; ++i)
				{
					if(qst->required_mob[i] > 0)
					{
						if(qst->required_mob[i] == gon->ID &&
						qst->required_mobtype[i] == QUEST_MOB_TYPE_GAMEOBJECT)
						{
							gon->goMap.insert( GameObjectGOMap::value_type( qst, qst->required_mobcount[i] ) );
							sDatabase.WaitExecute("INSERT INTO gameobject_quest_pickup_binding VALUES(%u, %u, %u)",
								gon->ID, qst->id, qst->required_mobcount[i]);
						}
					}
					if(qst->required_itemcount[i] > 0 && it2 != golootlist->end())
					{
						// check our loot template for this item
						for(std::set<uint32>::iterator it4 = it2->second.begin(); it4 != it2->second.end(); ++it4)
						{
							if((*it4) == qst->required_item[i])
							{
								//GOQuestItem it;
								//it.itemid = qst->required_item[i];
								//it.requiredcount = qst->required_itemcount[i];
								//gon->itemMap.insert( GameObjectItemMap::value_type( qst, it ) );
								//gon->itemMap[qst].insert(it);
								gon->itemMap[qst].insert( std::map<uint32, uint32>::value_type( qst->required_item[i], qst->required_itemcount[i]) );
								sDatabase.WaitExecute("INSERT INTO gameobject_quest_item_binding VALUES(%u, %u, %u, %u)",
									gon->ID, qst->id, qst->required_item[i], qst->required_itemcount[i]);
							}
						}
					}
				}
			}
		}
		c++;
		if(!(c % 150))
			SetProgressBar(c, total, "Binding");
	}
	ClearProgressBar();*/
	QueryResult * result  = WorldDatabase.Query("SELECT * FROM gameobject_quest_item_binding");
	QueryResult * result2 = WorldDatabase.Query("SELECT * FROM gameobject_quest_pickup_binding");

	GameObjectInfo * gon;
	Quest * qst;

	if(result)
	{
		do 
		{
			Field * fields = result->Fetch();
			gon = GameObjectNameStorage.LookupEntry(fields[0].GetUInt32());
			qst = QuestStorage.LookupEntry(fields[1].GetUInt32());
			if(gon && qst)
				gon->itemMap[qst].insert( make_pair( fields[2].GetUInt32(), fields[3].GetUInt32() ) );			

		} while(result->NextRow());
		delete result;
		result = NULL;
	}


	if(result2)
	{
		do 
		{
			Field * fields = result2->Fetch();
			gon = GameObjectNameStorage.LookupEntry(fields[0].GetUInt32());
			qst = QuestStorage.LookupEntry(fields[1].GetUInt32());
			if(gon && qst)
				gon->goMap.insert( make_pair( qst, fields[2].GetUInt32() ) );

		} while(result2->NextRow());
		delete result2;
		result2 = NULL;
	}

	result = WorldDatabase.Query("SELECT * FROM npc_gossip_textid");
	if(result)
	{
		uint32 entry, text;
		do 
		{
			entry = result->Fetch()[0].GetUInt32();
			text  = result->Fetch()[1].GetUInt32();

			mNpcToGossipText[entry] = text;

		} while(result->NextRow());
		delete result;
		result = NULL;
	}
	Log.Notice("ObjectMgr", "%u NPC Gossip TextIds loaded.", mNpcToGossipText.size());
}

Player* ObjectMgr::GetPlayer(const char* name, bool caseSensitive)
{
	Player * rv = NULL;
	PlayerStorageMap::const_iterator itr;
	_playerslock.AcquireReadLock();	

	if(!caseSensitive)
	{
		std::string strName = name;
		arcemu_TOLOWER(strName);
		for (itr = _players.begin(); itr != _players.end(); ++itr)
		{
			if(!stricmp(itr->second->GetNameString()->c_str(), strName.c_str()))
			{
				rv = itr->second;
				break;
			}
		}
	}
	else
	{
		for (itr = _players.begin(); itr != _players.end(); ++itr)
		{
			if(!strcmp(itr->second->GetName(), name))
			{
				rv = itr->second;
				break;
			}
		}
	}
		
	_playerslock.ReleaseReadLock();

	return rv;
}

Player* ObjectMgr::GetPlayer(uint64 guid)
{
	if( guid == 0 )
		return NULL;

	Player * rv;
	
	_playerslock.AcquireReadLock();	
	PlayerStorageMap::const_iterator itr = _players.find( (uint32)guid );
	rv = (itr != _players.end()) ? itr->second : 0;
	_playerslock.ReleaseReadLock();

	return rv;
}

PlayerCreateInfo* ObjectMgr::GetPlayerCreateInfo(uint8 race, uint8 class_) const
{
	PlayerCreateInfoMap::const_iterator itr;
	for (itr = mPlayerCreateInfo.begin(); itr != mPlayerCreateInfo.end(); itr++)
	{
		if( (itr->second->race == race) && (itr->second->class_ == class_) )
			return itr->second;
	}
	return NULL;
}

void ObjectMgr::AddGuild(Guild *pGuild)
{
	ASSERT( pGuild );
	mGuild[pGuild->GetGuildId()] = pGuild;
}

uint32 ObjectMgr::GetTotalGuildCount()
{
	return (uint32)mGuild.size();
}

bool ObjectMgr::RemoveGuild(uint32 guildId)
{
	GuildMap::iterator i = mGuild.find(guildId);
	if (i == mGuild.end())
	{
		return false;
	}


	mGuild.erase(i);
	return true;
}

Guild* ObjectMgr::GetGuild(uint32 guildId)
{
	GuildMap::const_iterator itr = mGuild.find(guildId);
	if(itr == mGuild.end())
	{ 
		return NULL;
	}
	return itr->second;
}

Guild* ObjectMgr::GetGuildByLeaderGuid(const uint64 &leaderGuid)
{
	GuildMap::const_iterator itr;
	for (itr = mGuild.begin();itr != mGuild.end(); itr++)
	{
		if( itr->second->GetGuildLeader() == leaderGuid )
		{ 
			return itr->second;
		}
	}
	return NULL;
}

Guild* ObjectMgr::GetGuildByGuildName(std::string guildName)
{
	GuildMap::const_iterator itr;
	for (itr = mGuild.begin();itr != mGuild.end(); itr++)
	{
		if( itr->second->GetGuildName() == guildName )
		{ 
			return itr->second;
		}
	}
	return NULL;
}


void ObjectMgr::AddGMTicket(GM_Ticket *ticket, bool startup)
{
	ASSERT( ticket );
	GM_TicketList.push_back(ticket);

	// save
	if(!startup)
		SaveGMTicket(ticket, NULL);
}

void ObjectMgr::UpdateGMTicket(GM_Ticket *ticket)
{
	SaveGMTicket(ticket, NULL);
}

void ObjectMgr::DeleteGMTicketPermanently(const uint64 &ticketGuid)
{
	for(GmTicketList::iterator i = GM_TicketList.begin(); i != GM_TicketList.end();)
	{
		if((*i)->guid == ticketGuid)
		{
			i = GM_TicketList.erase(i);
		}
		else
		{
			++i;
		}
	}

	// kill from db
	CharacterDatabase.Execute("DELETE FROM gm_tickets WHERE guid=%u", ticketGuid);
}

void ObjectMgr::DeleteAllRemovedGMTickets()
{
	for(GmTicketList::iterator i = GM_TicketList.begin(); i != GM_TicketList.end();)
	{
		if((*i)->deleted_by)
		{
			i = GM_TicketList.erase(i);
		}
		else
		{
			++i;
		}
	}

	CharacterDatabase.Execute("DELETE FROM gm_tickets WHERE deleted=1");
}

void ObjectMgr::RemoveGMTicketByPlayer(const uint64 &playerGuid)
{
	for(GmTicketList::iterator i = GM_TicketList.begin(); i != GM_TicketList.end();)
	{
		if((*i)->playerGuid == playerGuid && !(*i)->deleted_by)
		{
			(*i)->deleted_by = playerGuid;
			SaveGMTicket((*i), NULL);
		}
		++i;
	}
}

void ObjectMgr::RemoveGMTicket(const uint64 &ticketGuid,const uint64 &GMGuid)
{
	for(GmTicketList::iterator i = GM_TicketList.begin(); i != GM_TicketList.end();)
	{
		if((*i)->guid == ticketGuid && !(*i)->deleted_by)
		{
			(*i)->deleted_by = GMGuid;
			SaveGMTicket((*i), NULL);
		}
		++i;
	}
}

GM_Ticket* ObjectMgr::GetGMTicketByPlayer(const uint64 &playerGuid)
{
	for(GmTicketList::iterator i = GM_TicketList.begin(); i != GM_TicketList.end();)
	{
		if((*i)->playerGuid == playerGuid && !(*i)->deleted_by)
		{
			return (*i);
		}
		++i;
	}
	return NULL;
}

GM_Ticket* ObjectMgr::GetGMTicket(const uint64 &ticketGuid)
{
	for(GmTicketList::iterator i = GM_TicketList.begin(); i != GM_TicketList.end();)
	{
		if((*i)->guid == ticketGuid)
		{
			return (*i);
		}
		++i;
	}
	return NULL;
}

//std::list<GM_Ticket*>* ObjectMgr::GetGMTicketsByPlayer(uint64 playerGuid)
//{
//	std::list<GM_Ticket*>* list = new std::list<GM_Ticket*>();
//	for(GmTicketList::iterator i = GM_TicketList.begin(); i != GM_TicketList.end();)
//	{
//		if((*i)->playerGuid == playerGuid)
//		{
//			list->push_back((*i));
//		}
//		++i;
//	}
//	return list;
//}

void ObjectMgr::LoadVendors()
{
	HM_NAMESPACE::hash_map<uint32, std::vector<CreatureItem>*>::const_iterator itr;
	std::vector<CreatureItem> *items;
	CreatureItem itm;
  
	for( uint32 i=0;i<2;i++)
	{
		//QueryResult *result = WorldDatabase.Query("SELECT * FROM vendors;");
		//crap this might lag :P, vendors sell in alphabetical order now
		QueryResult *result;
		if( i == 0 )
			result = WorldDatabase.Query("SELECT a.entry,a.item,a.amount,a.max_amount,a.inctime,a.extended_cost,a.item_or_currency FROM vendors a INNER JOIN  items b ON a.item = b.entry ORDER BY b.name1");
		else
			result = WorldDatabase.Query("SELECT entry,item,amount,max_amount,inctime,extended_cost,item_or_currency FROM vendors where item_or_currency=2");
		
		if( result != NULL )
		{
			if( result->GetFieldCount() < 7 )
			{
				Log.Notice("ObjectMgr", "Invalid format in vendors (%u/7) columns, not enough data to proceed.\n", result->GetFieldCount() );
				delete result;
				return;
			}
			else if( result->GetFieldCount() > 7)
			{
				Log.Notice("ObjectMgr", "Invalid format in vendors (%u/7) columns, loading anyway because we have enough data\n", result->GetFieldCount() );
			}

			ItemExtendedCostEntry * ec = NULL;
			do
			{
				Field* fields = result->Fetch();
				uint32 entry = fields[0].GetUInt32();

				itr = mVendors.find( entry );

				if( itr == mVendors.end() )
				{
					items = new std::vector<CreatureItem>;
					mVendors[entry] = items;
				}
				else
				{
					items = itr->second;
				}

				if( items->size() == 128 )
					sLog.outDebug("Vendor (%u) list warning : list contains more then 128 items. Client will crash.\n",fields[0].GetUInt32());

				itm.itemid = fields[1].GetUInt32();
				itm.buy_amount = fields[2].GetUInt32();
				itm.available_amount = fields[3].GetUInt32();
				itm.max_amount = fields[3].GetInt32();
				if( itm.max_amount == -1 )
					itm.max_amount = 0;
				itm.incrtime = fields[4].GetUInt32();
				itm.item_vendor_slot = (uint32)(items->size())+1;
				if ( fields[5].GetUInt32() > 0 )
				{
					ec = dbcItemExtendedCost.LookupEntryForced( fields[5].GetUInt32() );
					if ( ec == NULL )
					{
						Log.Warning("LoadVendors", "Extendedcost for item %u references nonexistant EC %u", fields[1].GetUInt32(), fields[5].GetUInt32() );
						continue; // do not add free gear to vendors if we forgot to update our DBC
					}
				}
				else
					ec = NULL;
				itm.extended_cost = ec;
				itm.item_or_currency = fields[6].GetUInt32();	//1 = item, 2 = currency
				items->push_back( itm );
			}
			while( result->NextRow() );

			delete result;
			result = NULL;
		}
	}
	Log.Notice("ObjectMgr", "%u vendors loaded.", mVendors.size());
}

void ObjectMgr::ReloadVendors()
{
	mVendors.clear();
	LoadVendors();
}

std::vector<CreatureItem>* ObjectMgr::GetVendorList(uint32 entry)
{
	return mVendors[entry];
}

void ObjectMgr::LoadTotemSpells()
{
	std::stringstream query;
	QueryResult *result = WorldDatabase.Query( "SELECT * FROM totemspells" );

	if(!result)
	{
		return;
	}

	//TotemSpells *ts = NULL;
	SpellEntry * sp;
	uint32 spellid;

	do
	{
		Field *fields = result->Fetch();
		bool has_value=false;
		TotemSpells *tspells = new TotemSpells;
		uint32 totem_spell_to_register = fields[0].GetUInt32();
		for(int i=0;i<MAX_NUMBER_OF_SPELLS_FOR_TOTEM;i++)
		{
			spellid = fields[i+1].GetUInt32();
			sp = dbcSpell.LookupEntry(spellid);
			if(!spellid || !sp) 
				continue;
			tspells->spells[i] = sp;
			has_value = true;
		}
		if( has_value == false )
		{
			delete tspells;
			continue;
		}
//		m_totemSpells.insert( TotemSpellMap::value_type( fields[0].GetUInt32(), tspells ));
		m_totemSpells[ totem_spell_to_register ] = tspells ;
	} while( result->NextRow() );

	delete result;
	result = NULL;
	Log.Notice("ObjectMgr", "%u totem spells loaded.", m_totemSpells.size());
}

TotemSpells* ObjectMgr::GetTotemSpell(uint32 spellId)
{
	return m_totemSpells[spellId];
}

void ObjectMgr::LoadAIThreatToSpellId()
{
	QueryResult *result = WorldDatabase.Query( "SELECT * FROM ai_threattospellid" );

	if(!result)
	{
		return;
	}

	SpellEntry * sp;

	do
	{
		Field *fields = result->Fetch();
		sp = dbcSpell.LookupEntryForced( fields[0].GetUInt32() );
		if( sp != NULL )
		{
			sp->ThreatForSpell = fields[1].GetUInt32();
			sp->ThreatForSpellCoef = fields[2].GetFloat();
		}
		else
		{
			Log.Warning("AIThreatSpell", "Cannot apply to spell %u; spell is nonexistant.", fields[0].GetUInt32());
			WorldDatabase.Query( "Delete from ai_threattospellid where spell=%d", fields[0].GetUInt32());
		}

	} while( result->NextRow() );

	delete result;
	result = NULL;
}

/*
void ObjectMgr::LoadSpellFixes()
{
	SpellEntry* sp;
	QueryResult * result = WorldDatabase.Query("SELECT * FROM spellfixes");
	if(result)
	{
		if( result->GetFieldCount() != 8 )
		{
			Log.LargeErrorMessage(LARGERRORMESSAGE_WARNING, "Incorrect column count at spellfixes, skipping, please fix it.",
				"arcemu has skipped loading this table in order to avoid crashing.", NULL);
			return;
		}
		sLog.outDetail("Loading %u spell fixes from database...",result->GetRowCount());
		do
		{
			Field * f = result->Fetch();
			uint32 sf_spellId = f[0].GetUInt32();
			uint32 sf_procFlags = f[1].GetUInt32();
			uint32 sf_SpellGroupType = f[2].GetUInt32();
			uint32 sf_procChance = f[3].GetUInt32();
			uint32 sf_procCharges = f[4].GetUInt32();
			uint64 sf_groupRelation0 = f[5].GetUInt64();
			uint64 sf_groupRelation1 = f[6].GetUInt64();
			uint64 sf_groupRelation2 = f[7].GetUInt64();

			if( sf_spellId )
			{
				sp = dbcSpell.LookupEntryForced( sf_spellId );
				if( sp != NULL )
				{
					if( sf_procFlags )
						sp->procFlags = sf_procFlags;

					if( sf_SpellGroupType )
					{
						sp->GetSpellGroupType()[0] = sf_SpellGroupType;
						if( sf_SpellGroupType != 0 )
							sp->c_is_flags |= SPELL_FLAG_IS_REQUIRING_SM_MOD;
					}

					if( sf_procChance )
						sp->procChance = sf_procChance;

					if ( sf_procCharges )
						sp->procCharges = sf_procCharges;

					if ( sf_groupRelation0)
					{
						sp->EffectItemType[0] = (uint32)sf_groupRelation0;
						sp->eff[0].EffectSpellGroupRelation[1] = (uint32)(sf_groupRelation0);
					}
					if ( sf_groupRelation1)
					{
						sp->EffectItemType[1] = (uint32)sf_groupRelation1;
						sp->eff[1].EffectSpellGroupRelation[1] = (uint32)(sf_groupRelation1);
					}
					if ( sf_groupRelation2)
					{
						sp->EffectItemType[2] = (uint32)sf_groupRelation2;
						sp->eff[2].EffectSpellGroupRelation[1] = (uint32)(sf_groupRelation2);
					}
				}
			}
		}while(result->NextRow());
		delete result;
	}	
}
*/

void ObjectMgr::LoadSpellProcs()
{
	SpellEntry* sp;
	QueryResult * result = WorldDatabase.Query("SELECT * FROM spell_proc");
	if(result)
	{
		do
		{
			Field * f = result->Fetch();
			uint32 spe_spellId = f[0].GetUInt32();
			uint32 spe_NameHash = f[1].GetUInt32();

			if( spe_spellId )
			{
				sp = dbcSpell.LookupEntryForced( spe_spellId );
				if( sp != NULL )
				{
					int x;
					for( x = 0; x < 3; ++x )
						if( sp->ProcOnNameHash[x] == 0 )
							break;

					if( x != 3 )
					{
						sp->ProcOnNameHash[x] = spe_NameHash;
					}
					else
						sLog.outError("Wrong ProcOnNameHash for Spell: %u!",spe_spellId);
				}
			}

		}while(result->NextRow());
		delete result;
		result = NULL;
	}	
}

void ObjectMgr::LoadSpellEffectsOverride()
{
	SpellEntry* sp;
	QueryResult * result = WorldDatabase.Query("SELECT * FROM spell_effects_override");
	if(result)
	{
		do
		{
			Field * f = result->Fetch();
			uint32 seo_SpellId = f[0].GetUInt32();
			uint32 seo_EffectId = f[1].GetUInt32();
			uint32 seo_Disable = f[2].GetUInt32();
			uint32 seo_Effect = f[3].GetUInt32();
			uint32 seo_BasePoints = f[4].GetUInt32();
			uint32 seo_ApplyAuraName = f[5].GetUInt32();
			uint32 seo_SpellGroupRelation = f[6].GetUInt32();
			uint32 seo_MiscValue = f[7].GetUInt32();
			uint32 seo_TriggerSpell = f[8].GetUInt32();
			uint32 seo_ImplicitTargetA = f[9].GetUInt32();
			uint32 seo_ImplicitTargetB = f[10].GetUInt32();

			if( seo_SpellId )
			{
				sp = dbcSpell.LookupEntryForced( seo_SpellId );
				if( sp != NULL )
				{
					if( seo_Disable )
						sp->eff[seo_EffectId].Effect = 0;

					if( seo_Effect )
						sp->eff[seo_EffectId].Effect = seo_Effect;

					if( seo_BasePoints )
						sp->eff[seo_EffectId].EffectBasePoints = seo_BasePoints;

					if( seo_ApplyAuraName )
						sp->eff[seo_EffectId].EffectApplyAuraName = seo_ApplyAuraName;

					if( seo_SpellGroupRelation )
						sp->eff[seo_EffectId].EffectItemType = seo_SpellGroupRelation;

					if( seo_MiscValue )
						sp->eff[seo_EffectId].EffectMiscValue = seo_MiscValue;

					if( seo_TriggerSpell )
						sp->eff[seo_EffectId].EffectTriggerSpell = seo_TriggerSpell;
					
					if( seo_ImplicitTargetA )
						sp->eff[seo_EffectId].EffectImplicitTargetA = seo_ImplicitTargetA;

					if( seo_ImplicitTargetB )
						sp->eff[seo_EffectId].EffectImplicitTargetB = seo_ImplicitTargetB;
				}
			}

		}while(result->NextRow());
		delete result;
		result = NULL;
	}	
}

Item * ObjectMgr::CreateItem(uint32 entry,Player * owner)
{
	ItemPrototype * proto = ItemPrototypeStorage.LookupEntry(entry);
	if(proto == 0) 
	{ 
		return 0;
	}

	if(proto->InventoryType == INVTYPE_BAG)
	{
		Container * pContainer = new Container(HIGHGUID_TYPE_CONTAINER,GenerateLowGuid(HIGHGUID_TYPE_CONTAINER));
		pContainer->Create( entry, owner);
		pContainer->SetUInt32Value(ITEM_FIELD_STACK_COUNT, 1);
		return pContainer;
	}
	else
	{
		Item * pItem = ItemPool.PooledNew( __FILE__, __LINE__ );
		pItem->Init(HIGHGUID_TYPE_ITEM,GenerateLowGuid(HIGHGUID_TYPE_ITEM));
		pItem->Create(entry, owner);
		pItem->SetUInt32Value(ITEM_FIELD_STACK_COUNT, 1);
		return pItem;
	}
}

Item * ObjectMgr::LoadItem(const uint64 &guid)
{
	QueryResult * result = CharacterDatabase.Query("SELECT * FROM playeritems WHERE guid = %u", GUID_LOPART(guid));
	Item * pReturn = 0;

	if(result)
	{
		ItemPrototype * pProto = ItemPrototypeStorage.LookupEntry(result->Fetch()[2].GetUInt32());
		if(!pProto)
		{ 
			return NULL;
		}

		if(pProto->InventoryType == INVTYPE_BAG)
		{
			Container * pContainer = new Container(HIGHGUID_TYPE_CONTAINER,(uint32)guid);
			pContainer->LoadFromDB(result->Fetch());
			pReturn = pContainer;
		}
		else
		{
			Item * pItem = ItemPool.PooledNew( __FILE__, __LINE__ );
			pItem->Init(HIGHGUID_TYPE_ITEM,(uint32)guid);
			pItem->LoadFromDB(result->Fetch(), 0, false);
			pReturn = pItem;
		}
		delete result;
		result = NULL;
	}
	
	return pReturn;
}

Item * ObjectMgr::LoadExternalItem(const uint64 &guid)
{
	QueryResult * result = CharacterDatabase.Query("SELECT * FROM playeritems_external WHERE guid = %u", GUID_LOPART(guid));
	Item * pReturn = 0;

	if(result)
	{
		ItemPrototype * pProto = ItemPrototypeStorage.LookupEntry(result->Fetch()[2].GetUInt32());
		if(!pProto)
		{ 
			return NULL;
		}

		if(pProto->InventoryType == INVTYPE_BAG)
		{
			Container * pContainer = new Container(HIGHGUID_TYPE_CONTAINER,(uint32)guid);
			pContainer->LoadFromDB(result->Fetch());
			pReturn = pContainer;
		}
		else
		{
			Item * pItem = ItemPool.PooledNew( __FILE__, __LINE__ );
			pItem->Init(HIGHGUID_TYPE_ITEM,(uint32)guid);
			pItem->LoadFromDB(result->Fetch(), 0, false);
			pReturn = pItem;
		}
		delete result;
		result = NULL;
	}

	return pReturn;
}

void ObjectMgr::LoadCorpses(MapMgr * mgr)
{
	Corpse *pCorpse = NULL;

	QueryResult *result = CharacterDatabase.Query("SELECT * FROM corpses WHERE instanceId = %u", mgr->GetInstanceID());

	if(result)
	{
		do
		{
			Field *fields = result->Fetch();
			pCorpse = new Corpse(HIGHGUID_TYPE_CORPSE,fields[0].GetUInt32());
			pCorpse->SetPosition(fields[1].GetFloat(), fields[2].GetFloat(), fields[3].GetFloat(), fields[4].GetFloat());
			pCorpse->SetZoneId(fields[5].GetUInt32());
			pCorpse->SetMapId(fields[6].GetUInt32());
			pCorpse->SetInstanceID(fields[7].GetUInt32());
			pCorpse->LoadValues( fields[8].GetString());
			if(pCorpse->GetUInt32Value(CORPSE_FIELD_DISPLAY_ID) == 0)
			{
				delete pCorpse;
				result = NULL;
				continue;
			}

			pCorpse->PushToWorld(mgr);
		} while( result->NextRow() );

		delete result;
		result = NULL;
	}
}

std::list<ItemPrototype*>* ObjectMgr::GetListForItemSet(uint32 setid)
{
	return mItemSets[setid];
}
/*
void ObjectMgr::DespawnCorpse(uint64 Guid)
{
	Corpse * pCorpse = objmgr.GetCorpse((uint32)Guid);
	if(pCorpse == 0)	// Already Deleted
		return;
	
	sEventMgr.AddEvent(pCorpse, &Corpse::Despawn, EVENT_CORPSE_DESPAWN, 600000, 1,0);
}*/

GossipMenu::GossipMenu(uint64 Creature_Guid, uint32 Text_Id) : TextId(Text_Id), CreatureGuid(Creature_Guid)
{
	unk240 = 0;
}

void GossipMenu::AddItem(uint8 Icon, const char* Text, int32 Id /* = -1 */, int8 Extra /* = 0 */)
{
	GossipMenuItem Item;
	Item.Icon = Icon;
	Item.Extra = Extra;
	Item.Text = Text;
	Item.m_gBoxMessage = "";
	Item.m_gBoxMoney = 0;
	Item.Id = (uint32)Menu.size()+1;
	if(Id > 0)
		Item.IntId = Id;
	else
		Item.IntId = Item.Id;		

	Menu.push_back(Item);
}

void GossipMenu::AddMenuItem(uint8 Icon, std::string Message, uint32 dtSender, uint32 dtAction, std::string BoxMessage, uint32 BoxMoney, bool Coded, uint32 ID)
{
	GossipMenuItem Item;
	Item.Icon = Icon;
	Item.Extra = Coded;
	Item.Text = Message.c_str();
	Item.m_gBoxMessage = BoxMessage;
	Item.m_gBoxMoney = BoxMoney;
	Item.m_gAction = dtAction;
	Item.Id = (uint32)Menu.size()+1;
	if(ID > 0)
		Item.IntId = ID;
	else
		Item.IntId = Item.Id;		

	Menu.push_back(Item);
}

void GossipMenu::AddItem(GossipMenuItem* GossipItem)
{
	Menu.push_back(*GossipItem);
}

void GossipMenu::BuildPacket(WorldPacket& Packet)
{

/*
13329
D4 04 00 00 CB 01 30 F1 
DF 05 00 00 maybe menu ID
7F 08 00 00 textid
01 00 00 00 size
00 00 00 00 menu id
03 icon
00 extra
00 00 00 00 money
49 20 61 6D 20 69 6E 74 65 72 65 73 74 65 64 20 69 6E 20 77 61 72 6C 6F 63 6B 20 74 72 61 69 6E 69 6E 67 2E 00 menu text
00 popup message
00 00 00 00 terminator
*/
	Packet << CreatureGuid;
	Packet << unk240;			// some new menu type in 2.4?
	Packet << TextId;
	Packet << uint32(Menu.size());

	for(std::vector<GossipMenuItem>::iterator iter = Menu.begin();
		iter != Menu.end(); ++iter)
	{
		Packet << iter->Id;
		Packet << iter->Icon;
		Packet << iter->Extra;					// requires passw to open it
		Packet << uint32(iter->m_gBoxMoney);    // money required to open menu, 2.0.3
		Packet << iter->Text;					// or message box title in case of confirmation menu
		Packet << iter->m_gBoxMessage;          // accept text (related to money) pop up box, 2.0.3
	}
}

void GossipMenu::SendTo(Player* Plr)
{
	WorldPacket data(SMSG_GOSSIP_MESSAGE, Menu.size() * 50 + 12);
	BuildPacket(data);
	data << uint32(0);  // 0 quests obviously
	Plr->GetSession()->SendPacket(&data);
}

void ObjectMgr::CreateGossipMenuForPlayer(GossipMenu** Location, uint64 Guid, uint32 TextID, Player* Plr)
{
	GossipMenu *Menu = new GossipMenu(Guid, TextID);
	ASSERT(Menu);

	if(Plr->CurrentGossipMenu != NULL)
	{
		delete Plr->CurrentGossipMenu;
		Plr->CurrentGossipMenu = NULL;
	}

	Plr->CurrentGossipMenu = Menu;
	*Location = Menu;
}

GossipMenuItem GossipMenu::GetItem(uint32 Id)
{
	if(Id >= Menu.size())
	{
		GossipMenuItem k;
		k.IntId = 1;
		return k;
	} else {
		return Menu[Id];
	}
}

uint32 ObjectMgr::GetGossipTextForNpc(uint32 ID)
{
	return mNpcToGossipText[ID];
}

void ObjectMgr::LoadTrainers()
{
	QueryResult * result = WorldDatabase.Query("SELECT * FROM trainer_defs");
	QueryResult * result2;
	Field * fields2;
	const char * temp;
	size_t len;

	LoadDisabledSpells();

	if(!result)
	{ 
		return;
	}


	do 
	{
		Field * fields = result->Fetch();
		uint32 entry = fields[0].GetUInt32();
		Trainer * tr = new Trainer;
		if( !tr )
			return;
		tr->RequiredSkill = fields[1].GetUInt32();
		tr->RequiredSkillLine = fields[2].GetUInt32();
		tr->RequiredClass = fields[3].GetUInt32();
		tr->TrainerType = fields[4].GetUInt32();
		tr->Can_Train_Gossip_TextId = fields[6].GetUInt32();
		tr->Cannot_Train_GossipTextId = fields[7].GetUInt32();
		tr->UIMessage = (char*)NormalTalkMessage;
		if(!tr->Can_Train_Gossip_TextId)
			tr->Can_Train_Gossip_TextId=1;
		if(!tr->Cannot_Train_GossipTextId)
			tr->Cannot_Train_GossipTextId=1;

		temp = fields[5].GetString();
		if( temp )
		{
			len=strlen(temp);
			if(len)
			{
				tr->UIMessage = new char[len+1];
				strcpy(tr->UIMessage, temp);
				tr->UIMessage[len] = 0;
			}
		}

		//now load the spells
		result2 = WorldDatabase.Query("SELECT * FROM trainer_spells where entry='%u'",entry);
		if(!result2)
		{
			if( tr->TrainerType != TRAINER_TYPE_PET )
			{
				Log.Error("LoadTrainers", "Trainer with no spells, entry %u.", entry);
				if( (char*)NormalTalkMessage != tr->UIMessage )
				{
					delete [] tr->UIMessage; //only delete custom train messages
					tr->UIMessage = NULL;
				}
				delete tr;
				tr = NULL; 
				continue;
			}
			else
			{
				mTrainers.insert( TrainerMap::value_type( entry, tr ) );
				continue;
			}
		}
		if(result2->GetFieldCount() != 10)
		{
			Log.LargeErrorMessage(LARGERRORMESSAGE_WARNING, "Trainers table format is invalid. Please update your database.");
			return;
		}
		else
		{
			do
			{
				fields2 = result2->Fetch();
				TrainerSpell *ts = new TrainerSpell;
				bool abrt=false;
				uint32 CastSpellID=fields2[1].GetUInt32();
				uint32 LearnSpellID=fields2[2].GetUInt32();

				ts->pCastSpell = NULL;
				ts->pLearnSpell = NULL;
				ts->pCastRealSpell = NULL;

				if( CastSpellID != 0 )
				{
					ts->pCastSpell = dbcSpell.LookupEntryForced( CastSpellID );
					if( ts->pCastSpell )
					{
						for(int k=0;k<3;k++)
						{
							if(ts->pCastSpell->eff[k].Effect==SPELL_EFFECT_LEARN_SPELL)
							{
								ts->pCastRealSpell = dbcSpell.LookupEntryForced(ts->pCastSpell->eff[k].EffectTriggerSpell);
								if( ts->pCastRealSpell == NULL )
								{
									Log.Warning("Trainers", "Trainer %u contains cast spell %u that is non-teaching\n", entry, CastSpellID);
									abrt=true;
								}
								break;
							}
						}
					}

					if(abrt)
					{
						delete ts;
						ts = NULL;
						continue;
					}
				}

				if( LearnSpellID != 0 )
					ts->pLearnSpell = dbcSpell.LookupEntryForced( LearnSpellID );

				if( ts->pCastSpell == NULL && ts->pLearnSpell == NULL )
				{
					//Log.Warning("LoadTrainers", "Trainer %u without valid spells (%u/%u).", entry, CastSpellID, LearnSpellID);
					delete ts;
					ts = NULL;
					continue; //omg a bad spell !
				}

				if( ts->pCastSpell && !ts->pCastRealSpell)
				{
					delete ts;
					ts = NULL;
					continue;
				}

				ts->Cost = fields2[3].GetUInt32();
				ts->RequiredSpell = fields2[4].GetUInt32();
				ts->RequiredSkillLine = fields2[5].GetUInt32();
				ts->RequiredSkillLineValue = fields2[6].GetUInt32();
				ts->RequiredLevel = fields2[7].GetUInt32();
				ts->DeleteSpell = fields2[8].GetUInt32();
				ts->AdsProfession = (fields2[9].GetUInt32() != 0) ? true : false;
				tr->Spells.push_back(ts);
			}
			while(result2->NextRow());
			delete result2;
			result2 = NULL;

			tr->SpellCount = (uint32)tr->Spells.GetMaxSize();

			//and now we insert it to our lookup table
			// we will insert empty trainer definition so we may script trainer event
			if(!tr->SpellCount)
			{
				if( (char*)NormalTalkMessage != tr->UIMessage )
				{
					delete [] tr->UIMessage;
					tr->UIMessage = NULL;
				}
				delete tr;
				tr = NULL;
				continue;
			}

			mTrainers.insert( TrainerMap::value_type( entry, tr ) );
		}
		
	} while(result->NextRow());
	delete result;
	result = NULL;
	Log.Notice("ObjectMgr", "%u trainers loaded.", mTrainers.size());
}

Trainer* ObjectMgr::GetTrainer(uint32 Entry)
{
	TrainerMap::iterator iter = mTrainers.find(Entry);
	if(iter == mTrainers.end())
	{ 
		return NULL;
	}

	return iter->second;
}
/*
void ObjectMgr::GenerateLevelUpInfo()
{
	// Generate levelup information for each class.
	PlayerCreateInfo * PCI;
	for(uint32 Class = WARRIOR; Class <= DRUID; ++Class)
	{
		// These are empty.
		if(Class == 10)
			continue;

		// Search for a playercreateinfo.
		for(uint32 Race = RACE_HUMAN; Race <= RACE_WORGEN; ++Race )
		{
			PCI = GetPlayerCreateInfo(Race, Class);

			if(PCI == 0)
				continue;   // Class not valid for this race.

			// Generate each level's information
			uint32 MaxLevel = sWorld.m_genLevelCap + 1;
			LevelInfo* lvl=0, lastlvl;
			lastlvl.HP = PCI->health;
			lastlvl.Mana = PCI->mana;
			lastlvl.Stat[0] = PCI->strength;
			lastlvl.Stat[1] = PCI->ability;
			lastlvl.Stat[2] = PCI->stamina;
			lastlvl.Stat[3] = PCI->intellect;
			lastlvl.Stat[4] = PCI->spirit;
			lastlvl.XPToNextLevel = 400;
			LevelMap * lMap = new LevelMap;
			if( !lMap )
				return;

			// Create first level.
			lvl = new LevelInfo;
			*lvl = lastlvl;

			// Insert into map
			lMap->insert( LevelMap::value_type( 1, lvl ) );

			uint32 val;
			for(uint32 Level = 2; Level < MaxLevel; ++Level)
			{
				lvl = new LevelInfo;

				// Calculate Stats
				for(uint32 s = 0; s < 5; ++s)
				{
					val = GainStat(Level, Class, s);
					lvl->Stat[s] = lastlvl.Stat[s] + val;
				}

				// Calculate HP/Mana
				uint32 TotalHealthGain = 0;
				uint32 TotalManaGain = 0;

				switch(Class)
				{
				case WARRIOR:
					if(Level<13)TotalHealthGain+=19;
					else if(Level <36) TotalHealthGain+=Level+6;
//					else if(Level >60) TotalHealthGain+=Level+100;
					else if(Level >60) TotalHealthGain+=Level+206;
					else TotalHealthGain+=2*Level-30;
					break;
				case HUNTER:
					if(Level<13)TotalHealthGain+=17;
//					else if(Level >60) TotalHealthGain+=Level+45;
					else if(Level >60) TotalHealthGain+=Level+161;
					else TotalHealthGain+=Level+4;

					if(Level<11)TotalManaGain+=29;
					else if(Level<27)TotalManaGain+=Level+18;
//					else if(Level>60)TotalManaGain+=Level+20;
					else if(Level>60)TotalManaGain+=Level+150;
					else TotalManaGain+=45;
					break;
				case ROGUE:
					if(Level <15)TotalHealthGain+=17;
//					else if(Level >60) TotalHealthGain+=Level+110;
					else if(Level >60) TotalHealthGain+=Level+191;
					else TotalHealthGain+=Level+2;
					break;
				case DRUID:
					if(Level < 17)TotalHealthGain+=17;
//					else if(Level >60) TotalHealthGain+=Level+55;
					else if(Level >60) TotalHealthGain+=Level+176;
					else TotalHealthGain+=Level;

					if(Level < 26)TotalManaGain+=Level+20;
//					else if(Level>60)TotalManaGain+=Level+25;
					else if(Level>60)TotalManaGain+=Level+150;
					else TotalManaGain+=45;
					break;
				case MAGE:
					if(Level < 23)TotalHealthGain+=15;
//					else if(Level >60) TotalHealthGain+=Level+40;
					else if(Level >60) TotalHealthGain+=Level+190;
					else TotalHealthGain+=Level-8;

					if(Level <28)TotalManaGain+=Level+23;
//					else if(Level>60)TotalManaGain+=Level+26;
					else if(Level>60)TotalManaGain+=Level+115;
					else TotalManaGain+=51;
					break;
				case SHAMAN:
					if(Level <16)TotalHealthGain+=17;
//					else if(Level >60) TotalHealthGain+=Level+75;
					else if(Level >60) TotalHealthGain+=Level+157;
					else TotalHealthGain+=Level+1;

					if(Level<22)TotalManaGain+=Level+19;
//					else if(Level>60)TotalManaGain+=Level+70;
					else if(Level>60)TotalManaGain+=Level+175;
					else TotalManaGain+=49;
					break;
				case WARLOCK:
					if(Level <17)TotalHealthGain+=17;
//					else if(Level >60) TotalHealthGain+=Level+50;
					else if(Level >60) TotalHealthGain+=Level+192;
					else TotalHealthGain+=Level-2;

					if(Level< 30)TotalManaGain+=Level+21;
//					else if(Level>60)TotalManaGain+=Level+25;
					else if(Level>60)TotalManaGain+=Level+121;
					else TotalManaGain+=51;
					break;
				case PALADIN:
					if(Level < 14)TotalHealthGain+=18;
//					else if(Level >60) TotalHealthGain+=Level+55;
					else if(Level >60) TotalHealthGain+=Level+167;
					else TotalHealthGain+=Level+4;

					if(Level<30)TotalManaGain+=Level+17;
//					else if(Level>60)TotalManaGain+=Level+100;
					else if(Level>60)TotalManaGain+=Level+131;
					else TotalManaGain+=42;
					break;
				case DEATHKNIGHT:
					if(Level >60) TotalHealthGain+=Level+167;
					else TotalHealthGain+=Level+4;
					break;
				case PRIEST:
					if(Level <21)TotalHealthGain+=15;
//					else if(Level >60) TotalHealthGain+=Level+40;
					else if(Level >60) TotalHealthGain+=Level+157;
					else TotalHealthGain+=Level-6;

					if(Level <22)TotalManaGain+=Level+22;
					else if(Level <32)TotalManaGain+=Level+37;
//					else if(Level>60)TotalManaGain+=Level+35;
					else if(Level>60)TotalManaGain+=Level+207;
					else TotalManaGain+=54;
					break;
				}

				// Apply HP/Mana
				lvl->HP = lastlvl.HP + TotalHealthGain;
				lvl->Mana = lastlvl.Mana + TotalManaGain;

				// Calculate next level XP
				uint32 nextLvlXP = 0;
				//this is a fixed table taken from 2.3.0 wow. This can;t get more blizzlike with the "if" cases ;)
				if( ( Level - 1 ) < MAX_PREDEFINED_NEXTLEVELXP )
					nextLvlXP = NextLevelXp[( Level - 1 )];
				if( nextLvlXP == 0 )
				{
					// 2.2
					//double MXP = 45 + ( 5 * level );
					// 2.3
					double MXP = 235 + ( 5 * Level );
					double DIFF = Level < 29 ? 0.0 : Level < 30 ? 1.0 : Level < 31 ? 3.0 : Level < 32 ? 6.0 : 5.0 * ( double( Level ) - 30.0 );
					double XP = ( ( 8.0 * double( Level ) ) + DIFF ) * MXP;
					nextLvlXP = (int)( ( XP / 100.0 ) + 0.5 ) * 100;
				}

				lvl->XPToNextLevel = nextLvlXP;
				lastlvl = *lvl;
				lastlvl.HP = lastlvl.HP;

				// Apply to map.
				lMap->insert( LevelMap::value_type( Level, lvl ) );
			}

			// Now that our level map is full, let's create the class/race pair
			pair<uint32, uint32> p;
			p.first = Race;
			p.second = Class;

			// Insert back into the main map.
			mLevelInfo.insert( LevelInfoMap::value_type( p, lMap ) );
		}
	}
	Log.Notice("ObjectMgr", "%u level up informations generated.", mLevelInfo.size());
}

LevelInfo* ObjectMgr::GetLevelInfo(uint32 Race, uint32 Class, uint32 Level)
{
	// Iterate levelinfo map until we find the right class+race.
	LevelInfoMap::iterator itr = mLevelInfo.begin();
	for(; itr != mLevelInfo.end(); ++itr )
	{
		if( itr->first.first == Race && itr->first.second == Class)
		{
			// We got a match.
			// Let's check that our level is valid first.
			if( Level > sWorld.m_genLevelCap ) // too far.
				Level = sWorld.m_genLevelCap;

			// Pull the level information from the second map.
			LevelMap::iterator it2 = itr->second->find( Level );
			ASSERT( it2 != itr->second->end() );

			return it2->second;
		}
	}

	return NULL;
}*/

void ObjectMgr::LoadDefaultPetSpells()
{
	QueryResult * result = WorldDatabase.Query("SELECT * FROM petdefaultspells");
	if(result)
	{
		do 
		{
			Field * f = result->Fetch();
			uint32 Entry = f[0].GetUInt32();
			uint32 spell = f[1].GetUInt32();
			SpellEntry * sp = dbcSpell.LookupEntry(spell);

			if(spell && Entry && sp)
			{
				PetDefaultSpellMap::iterator itr = mDefaultPetSpells.find(Entry);
				if(itr != mDefaultPetSpells.end())
					itr->second.insert(sp);
				else
				{
					set<SpellEntry*> s;
					s.insert(sp);
					mDefaultPetSpells[Entry] = s;
				}
			}
		} while(result->NextRow());

		delete result;
		result = NULL;
	}
}

set<SpellEntry*>* ObjectMgr::GetDefaultPetSpells(uint32 Entry)
{
	PetDefaultSpellMap::iterator itr = mDefaultPetSpells.find(Entry);
	if(itr == mDefaultPetSpells.end())
	{ 
		return 0;
	}

	return &(itr->second);
}

uint32 ObjectMgr::GetPetSpellCooldown(uint32 SpellId)
{
	SpellEntry* sp = dbcSpell.LookupEntry( SpellId );
//	return (sp->StartRecoveryTime + MAX(sp->CategoryRecoveryTime,sp->RecoveryTime));	//parrallel with recovery time
	return ( MAX( sp->StartRecoveryTime, MAX(sp->CategoryRecoveryTime, sp->RecoveryTime) ) );
}

void ObjectMgr::LoadSpellOverride()
{
//	int i = 0;
	std::stringstream query;
	QueryResult *result = WorldDatabase.Query( "SELECT DISTINCT overrideId FROM spelloverride" );

	if(!result)
	{
		return;
	}

//	int num = 0;
//	uint32 total =(uint32) result->GetRowCount();
	SpellEntry * sp;
	uint32 spellid;

	do
	{
		Field *fields = result->Fetch();
		query.rdbuf()->str("");
		query << "SELECT spellId FROM spelloverride WHERE overrideId = ";
		query << fields[0].GetUInt32();
		QueryResult *resultIn = WorldDatabase.Query(query.str().c_str());
		std::list<SpellEntry*>* list = new std::list<SpellEntry*>;
		if(resultIn)
		{
			do
			{
				Field *fieldsIn = resultIn->Fetch();
				spellid = fieldsIn[0].GetUInt32();
				sp = dbcSpell.LookupEntry(spellid);
				if(!spellid || !sp) 
					continue;
				list->push_back(sp);
			}while(resultIn->NextRow());
		}
		delete resultIn;
		resultIn = NULL;
		if(list->empty() == true)
		{
			delete list;
			list = NULL;
		}
		else
			mOverrideIdMap.insert( OverrideIdMap::value_type( fields[0].GetUInt32(), list ));
	} while( result->NextRow() );
	delete result;
	result = NULL;
	Log.Notice("ObjectMgr", "%u spell overrides loaded.", mOverrideIdMap.size());
}

void ObjectMgr::SetVendorList(uint32 Entry, std::vector<CreatureItem>* list_)
{
	mVendors[Entry] = list_;
}

void ObjectMgr::LoadCreatureClickSpells()
{
	QueryResult *result = WorldDatabase.Query("SELECT * FROM creature_clickspells");
	if(!result)
	{ 
		return;
	}
	do
	{
		Field *fields = result->Fetch();

		CreatureProto * cp = CreatureProtoStorage.LookupEntry( fields[0].GetUInt32() );
		if( !cp )
			continue;
		cp->vehicleId = fields[4].GetUInt32();
		cp->vehicle_use_required_quest = fields[5].GetUInt32();

		cp->on_click_spell = fields[1].GetUInt32();
		SpellEntry *sp = dbcSpell.LookupEntryForced( cp->on_click_spell );
		if( !sp )
		{
			cp->on_click_spell = 0;
			continue;
		}
		cp->on_click_spell_caster_is_player = fields[2].GetUInt8();
		cp->on_click_spell_target_is_clicker = fields[3].GetUInt8();
	}while( result->NextRow() );

	Log.Notice("ObjectMgr", "%u creature click spells loaded.", result->GetRowCount());
	delete result;
	result = NULL;
}

void ObjectMgr::LoadCreatureTimedEmotes()
{
	QueryResult *result = WorldDatabase.Query("SELECT * FROM creature_timed_emotes order by `rowid` asc");
	if(!result)
	{ 
		return;
	}

	do
	{
		Field *fields = result->Fetch();
		spawn_timed_emotes* te = new spawn_timed_emotes;
		if( !te ) 
			return;
		te->type = fields[2].GetUInt8();
		te->value = fields[3].GetUInt32();
		char *str = ( char* ) fields[4].GetString();
		if ( str )
		{
			uint32 len = ( int ) strlen ( str ) ;
			te->msg = new char[ len+1 ];
			memcpy ( te->msg, str, len+1 );
		}
		else te->msg = NULL;
		te->msg_type = fields[5].GetUInt32();
		te->msg_lang = fields[6].GetUInt32();
		te->expire_after = fields[7].GetUInt32();

		HM_NAMESPACE::hash_map<uint32,TimedEmoteList*>::const_iterator i;
		uint32 spawnid=fields[0].GetUInt32();
		i=m_timedemotes.find(spawnid);
		if(i==m_timedemotes.end())
		{
			TimedEmoteList* m=new TimedEmoteList;
			m->push_back( te );
			m_timedemotes[spawnid]=m;		
		}else
		{
			i->second->push_back( te );
		}
	}while( result->NextRow() );

	Log.Notice("ObjectMgr", "%u timed emotes cached.", result->GetRowCount());
	delete result;
	result = NULL;
}

TimedEmoteList*ObjectMgr::GetTimedEmoteList(uint32 spawnid)
{
	HM_NAMESPACE::hash_map<uint32,TimedEmoteList*>::const_iterator i;
	i=m_timedemotes.find(spawnid);
	if(i!=m_timedemotes.end())
	{
		TimedEmoteList * m=i->second;
		return m;
	}
	else 
	return NULL;
}

void ObjectMgr::LoadCreatureWaypoints()
{
	QueryResult *result = WorldDatabase.Query("SELECT * FROM creature_waypoints order by spawnid,waypointid asc");
	if(!result)
	{ 
		return;
	}
	SimpleVect< WayPoint* > tempvect( 5000 );
	int32 prev_spawn_id = -1;
//	uint32 spawn_iter=0;
	do
	{
		Field *fields = result->Fetch();
		WayPoint* wp = new WayPoint;
		wp->id = fields[1].GetUInt32();
		wp->x = fields[2].GetFloat();
		wp->y = fields[3].GetFloat();
		wp->z = fields[4].GetFloat();
		wp->waittime = fields[5].GetUInt32();
		wp->flags = fields[6].GetUInt32();
		wp->forwardemoteoneshot = fields[7].GetBool();
		wp->forwardemoteid = fields[8].GetUInt32();
		wp->backwardemoteoneshot = fields[9].GetBool();
		wp->backwardemoteid = fields[10].GetUInt32();
		wp->forwardskinid = fields[11].GetUInt32();
		wp->backwardskinid = fields[12].GetUInt32();

		if( prev_spawn_id == -1 )
		{
			prev_spawn_id = fields[0].GetUInt32();
			tempvect.ResetPushPos();
		}
		//time to store a fully loaded mob ?
		else if( prev_spawn_id != fields[0].GetUInt32() )
		{
			//create a new vector
			SimpleVect< WayPoint* > *spawn_paths;
			spawn_paths = new SimpleVect< WayPoint* >( tempvect.GetMaxSize()+1 );
			for( uint32 ti=0;ti<tempvect.GetMaxSize();ti++)
				spawn_paths->push_back( tempvect.GetValue( ti ) );
			//add it to our lookupmap
			m_waypoints[ prev_spawn_id ] = spawn_paths;
//			m_waypoints[ spawn_iter++ ] = spawn_paths;
			//start a new loop
			prev_spawn_id = fields[0].GetUInt32();
			tempvect.ResetPushPos();
		}
		tempvect.push_back( wp );
	}while( result->NextRow() );
	//last mob waypoints
	if( prev_spawn_id != -1 )
	{
		SimpleVect< WayPoint* > *spawn_paths;
		spawn_paths = new SimpleVect< WayPoint* >( tempvect.GetMaxSize()+1 );
		for( uint32 ti=0;ti<tempvect.GetMaxSize();ti++)
			spawn_paths->push_back( tempvect.GetValue( ti ) );
		//add it to our lookupmap
		m_waypoints[ prev_spawn_id ] = spawn_paths;
	}
	Log.Notice("ObjectMgr", "%u waypoints cached.", result->GetRowCount());
	delete result;
	result = NULL;
}

void ObjectMgr::DestroyLootList( std::map<uint32, SimpleVect< SimpleVect<LootStoreStruct*>* >* > *cur_global_store )
{
	std::map<uint32, SimpleVect< SimpleVect<LootStoreStruct*>* >* >::iterator itr;
	for( itr = cur_global_store->begin(); itr != cur_global_store->end(); itr++ )
	{
		SimpleVect< SimpleVect<LootStoreStruct*>* > *list_of_list = itr->second;
		if( list_of_list == NULL )
			continue;	//how the hell ?
		for( uint32 listitr = 0; listitr < list_of_list->GetMaxSize(); listitr++ )
		{
			SimpleVect<LootStoreStruct*> *group_list = list_of_list->GetValue( listitr );
			group_list->DestroyContent();	//only for pointers
			delete group_list;
		}
		delete list_of_list;
	}
	cur_global_store->clear();
}

void ObjectMgr::ProcessReferenceLootTable()
{
	std::map<uint32, SimpleVect< SimpleVect<LootStoreStruct*>* >* >::iterator itr;
	bool made_change;
	do{
		made_change=0;
		//for each list of group lists
		for( itr = m_reference_loot.begin(); itr != m_reference_loot.end(); itr++ )
		{
			//foreach of list of groups
			SimpleVect< SimpleVect<LootStoreStruct*>* > *list_of_gr_lists = itr->second;
			uint32 max_ind = list_of_gr_lists->GetMaxSize();
			for(uint32 ligli=0;ligli<max_ind;ligli++)
			{
				//for each group list
				SimpleVect<LootStoreStruct*> *gli=list_of_gr_lists->GetValue( ligli );
				LootStoreStruct *ls;
				uint32 max_ind2 = gli->GetMaxSize();
				for(uint32 glii=0;glii<max_ind2;glii++)
				{
					ls = gli->GetValue( glii );
					if( ls->MinCountOrRef < 0 && ls->ref_list == NULL )
					{
						uint32 RefEntry = -ls->MinCountOrRef;
						ls->ref_list = m_reference_loot[ RefEntry ];	//chaned linking
//						made_change = 1;	//it is enough to parse the list once.
					}
				}
			}
		}
	}while(made_change==1);
}

void ObjectMgr::LoadLootsAny(const char *table_name,std::map<uint32, SimpleVect< SimpleVect<LootStoreStruct*>* >* > *cur_global_store)
{
	//the async query since we load it multi threaded
	QueryResult *result = WorldDatabase.Query("SELECT * FROM %s order by entry,groupid asc",table_name);
	if(!result)
	{ 
		Log.Error("missing loot table %s !",table_name);
		return;
	}
	//make sure we support table reload
	DestroyLootList( cur_global_store );
	//define local vars
	SimpleVect< LootStoreStruct* > group_list( 5000 );	//represent a temporary group list
	SimpleVect< SimpleVect<LootStoreStruct*>* > list_group_list( 5000 );	//represent a temporary list of group lists
	int32 prev_list_id = -1;
	int32 prev_group_id = -1;
	do
	{
		Field *fields = result->Fetch();
		LootStoreStruct* ls = new LootStoreStruct;
		ls->entry = fields[0].GetUInt32();
		ls->ItemEntry = fields[1].GetUInt32();
		ls->DropChance = fields[2].GetFloat();
		ls->GroupID = fields[3].GetUInt32();	
		ls->MinCountOrRef = fields[4].GetInt32();
		ls->MaxCount = fields[5].GetUInt32();
		ls->ConditionType = fields[6].GetUInt32();
		ls->Cond[0] = fields[7].GetUInt32();
		ls->Cond[1] = fields[8].GetUInt32();
		ls->difficulty_mask = fields[9].GetUInt32();
		if( ls->MinCountOrRef < 0 )
		{
			uint32 RefEntry = -ls->MinCountOrRef;
			ls->ref_list = m_reference_loot[ RefEntry ];
		}
		else 
			ls->ref_list = NULL;

		ItemPrototype *itemproto = ItemPrototypeStorage.LookupEntry( ls->ItemEntry );
		if( itemproto )
		{
			ls->item.displayid = itemproto->DisplayInfoID;
			ls->item.itemproto = itemproto;
		}
		//!!!! does not mean item needs to be valid. Invalid item may be used for ref lists !!!
		else if( ls->ref_list == NULL )
		{
			//seems like an invalid item
			delete ls;
			continue;
		}

		//just for the first time
		if( prev_group_id == -1 )
		{
			prev_group_id = ls->GroupID;
			list_group_list.ResetPushPos();
			prev_list_id = ls->entry;
			group_list.ResetPushPos();
		}

		if( prev_group_id != ls->GroupID )
		{
			//create a group list
CREATE_GROUP_LIST:
			SimpleVect< LootStoreStruct* > *grouped_loot_list = new SimpleVect< LootStoreStruct* >( group_list.GetMaxSize()+1 );
			for( uint32 ti=0;ti<group_list.GetMaxSize();ti++)
				grouped_loot_list->push_back( group_list.GetValue( ti ) );
			//store the list in a list
			list_group_list.push_back( grouped_loot_list );
			//start a new temp list store
			prev_group_id = ls->GroupID;
			group_list.ResetPushPos();
		}
		//time to store a fully loaded mob ?
		if( prev_list_id != ls->entry )
		{
			//if there was only 1 group in prev entry list
			if( list_group_list.GetMaxSize() == 0 && group_list.GetMaxSize() > 0 )
				goto CREATE_GROUP_LIST;
			//new list to store the list
			SimpleVect< SimpleVect<LootStoreStruct*>* > *plist_list=new SimpleVect< SimpleVect<LootStoreStruct*>* >( list_group_list.GetMaxSize()+1 );;	//represent a temporary list of group lists
			for( uint32 ti=0;ti<list_group_list.GetMaxSize();ti++)
				plist_list->push_back( list_group_list.GetValue( ti ) );
			//add it to our lookupmap
			if( plist_list )
				cur_global_store[0][ prev_list_id ] = plist_list;
			//start a new loop
			prev_list_id = ls->entry;
			list_group_list.ResetPushPos();
		}
		group_list.push_back( ls );
	}while( result->NextRow() );
	//last list
//	if( prev_list_id != -1 )
	{
		if( group_list.GetMaxSize() > 0 )
		{
			//create a group list
			SimpleVect< LootStoreStruct* > *grouped_loot_list = new SimpleVect< LootStoreStruct* >( group_list.GetMaxSize()+1 );
			for( uint32 ti=0;ti<group_list.GetMaxSize();ti++)
				grouped_loot_list->push_back( group_list.GetValue( ti ) );
			//store the list in a list
			list_group_list.push_back( grouped_loot_list );
		}
		if( list_group_list.GetMaxSize() > 0 )
		{
			//new list to store the list
			SimpleVect< SimpleVect<LootStoreStruct*>* > *plist_list=new SimpleVect< SimpleVect<LootStoreStruct*>* >( list_group_list.GetMaxSize()+1 );;	//represent a temporary list of group lists
			for( uint32 ti=0;ti<list_group_list.GetMaxSize();ti++)
				plist_list->push_back( list_group_list.GetValue( ti ) );
			//add it to our lookupmap
			if( plist_list )
				cur_global_store[0][ prev_list_id ] = plist_list;
		}
	}
	Log.Notice("ObjectMgr", "%u loots from %s table cached.", result->GetRowCount(), table_name );
	delete result;
	result = NULL;
}

void ObjectMgr::LoadLootsCurrency()
{
	//the async query since we load it multi threaded
	QueryResult *result = WorldDatabase.Query("SELECT * FROM loot_currency group by entry,currency order by entry");
	if(!result)
	{ 
		Log.Error("ObjectMgr", "missing currency loot table !");
		return;
	}

	SimpleVect< LootCurencyStoreStruct* > group_list( 5000 );	//represent a temporary group list
//	SimpleVect< SimpleVect<LootCurencyStoreStruct*>* > list_group_list( 5000 );	//represent a temporary list of group lists
	int32 prev_list_id = -1;

	//set size to perfect
//	m_currency_loot.Resize( result->GetRowCount() );
	do
	{
		Field *fields = result->Fetch();
		uint32 entry = fields[0].GetUInt32();
		//first time initialization ?
		if( prev_list_id == -1 )
		{
			CreatureProto *proto = CreatureProtoStorage.LookupEntry( entry );
			if( proto == NULL )
				continue;
			prev_list_id = entry;
		}
		//it is time to start a new list fill
		else if( prev_list_id != entry )
		{
			//create a new list and copy content to it
			SimpleVect< LootCurencyStoreStruct* > *grouped_loot_list = new SimpleVect< LootCurencyStoreStruct* >( group_list.GetMaxSize()+1 );
			for( uint32 ti=0;ti<group_list.GetMaxSize();ti++)
				grouped_loot_list->push_back( group_list.GetValue( ti ) );
			//store the list that we will destroy on shutdown
			m_currency_loot.push_back( grouped_loot_list );
			//reset our temp holder
			group_list.ResetPushPos();
			prev_list_id = -1;	//we handled this list, time to move on
			CreatureProto *proto = CreatureProtoStorage.LookupEntry( entry );
			if( proto == NULL )
				continue;
			proto->loot_currency = grouped_loot_list;
			prev_list_id = entry;	//we are ready to handle the next list
		}
		LootCurencyStoreStruct* ls = new LootCurencyStoreStruct;
		ls->entry = entry;
		ls->currency_type = fields[1].GetUInt32();
		ls->currency_amt = fields[2].GetUInt32();
		ls->max_level = fields[3].GetUInt32();	
		ls->difficulty_mask = fields[4].GetInt32();
		group_list.push_back( ls );
	}while( result->NextRow() );
	Log.Notice("ObjectMgr", "%u loots from loot_currency table cached.", result->GetRowCount() );
	delete result;
	result = NULL;
}

WayPointVect *ObjectMgr::GetWayPointMap(uint32 spawnid)
{
	HM_NAMESPACE::hash_map<uint32,WayPointVect*>::const_iterator i;
	i=m_waypoints.find(spawnid);
	if(i!=m_waypoints.end())
	{
		WayPointVect *m=i->second;
		// we don't wanna erase from the map, becuase some are used more
		// than once (for instances)

		//m_waypoints.erase(i);
		return m;
	}
	else 
	return NULL;
}

Pet * ObjectMgr::CreatePet( uint32 entry, uint8 pet_number )
{
	uint64 guid;
	m_petlock.Acquire();
	m_hiPetGuid = CataclysmCompatibleIncreaseGuid( m_hiPetGuid );
	guid = m_hiPetGuid;
	m_petlock.Release();
	return new Pet(((uint64)HIGHGUID_TYPE_PET<<32) | ((uint64)pet_number << 24) | guid );	//pet number starts at byte 3 and might go up to byte 6
}

Player * ObjectMgr::CreatePlayer()
{
	uint32 guid;
	m_playerguidlock.Acquire();
	m_hiPlayerGuid = CataclysmCompatibleIncreaseGuid( m_hiPlayerGuid );
	guid = m_hiPlayerGuid;
	m_playerguidlock.Release();
	return new Player(guid);
}

void ObjectMgr::AddPlayer(Player * p)//add it to global storage
{
	_playerslock.AcquireWriteLock();
	_players[p->GetLowGUID()] = p;
	_playerslock.ReleaseWriteLock();
}

void ObjectMgr::RemovePlayer(Player * p)
{
	_playerslock.AcquireWriteLock();
	_players.erase(p->GetLowGUID());
	_playerslock.ReleaseWriteLock();

}

Corpse * ObjectMgr::CreateCorpse()
{
	uint32 guid;
	m_corpseguidlock.Acquire();
	m_hiCorpseGuid = CataclysmCompatibleIncreaseGuid( m_hiCorpseGuid );
	guid = m_hiCorpseGuid;
	m_corpseguidlock.Release();
	return new Corpse(HIGHGUID_TYPE_CORPSE,guid);
}

void ObjectMgr::AddCorpse(Corpse * p)//add it to global storage
{
	_corpseslock.Acquire();
	m_corpses[p->GetLowGUID()]=p;
	_corpseslock.Release();
}

void ObjectMgr::RemoveCorpse(Corpse * p)
{
	_corpseslock.Acquire();
	m_corpses.erase(p->GetLowGUID());
	_corpseslock.Release();
}

Corpse * ObjectMgr::GetCorpse(uint32 corpseguid)
{
	Corpse * rv;
	_corpseslock.Acquire();
	CorpseMap::const_iterator itr = m_corpses.find(corpseguid);
	rv = (itr != m_corpses.end()) ? itr->second : 0;
	_corpseslock.Release();
	return rv;
}

Transporter * ObjectMgr::GetTransporter(uint32 guid)
{
	Transporter * rv;
	_TransportLock.Acquire();
	HM_NAMESPACE::hash_map<uint32, Transporter*>::const_iterator itr = mTransports.find(guid);
	rv = (itr != mTransports.end()) ? itr->second : 0;
	_TransportLock.Release();
	return rv;
}

void ObjectMgr::AddTransport(Transporter *pTransporter)
{
	_TransportLock.Acquire();
	mTransports[pTransporter->GetUIdFromGUID()]=pTransporter;
 	_TransportLock.Release();
}

Transporter * ObjectMgr::GetTransporterByEntry(uint32 entry)
{
	Transporter * rv = 0;
	_TransportLock.Acquire();
	HM_NAMESPACE::hash_map<uint32, Transporter*>::iterator itr = mTransports.begin();
	for(; itr != mTransports.end(); ++itr)
	{
		if(itr->second->GetEntry() == entry)
		{
			rv = itr->second;
			break;
		}
	}
	_TransportLock.Release();
	return rv;
}

void ObjectMgr::LoadGuildCharters()
{
	m_hiCharterId = 0;
	QueryResult * result = CharacterDatabase.Query("SELECT * FROM charters");
	if(!result) 
	{ 
		return;
	}
	do 
	{
		Charter * c = new Charter(result->Fetch());
		m_charters[ c->CharterType ][ c->GetID() ] = c;
		if(c->GetID() > m_hiCharterId)
			m_hiCharterId = c->GetID();
	} while(result->NextRow());
	delete result;
	result = NULL;
	Log.Notice("ObjectMgr", "%u charters loaded.", m_charters[0].size());
}

Charter * ObjectMgr::GetCharter(uint32 CharterId, CharterTypes Type)
{
	Charter * rv;
	HM_NAMESPACE::hash_map<uint32,Charter*>::iterator itr;
	m_charterLock.AcquireReadLock();
	itr = m_charters[Type].find(CharterId);
	rv = (itr == m_charters[Type].end()) ? 0 : itr->second;
	m_charterLock.ReleaseReadLock();
	return rv;
}

Charter * ObjectMgr::CreateCharter(uint32 LeaderGuid, CharterTypes Type)
{
	m_charterLock.AcquireWriteLock();
	m_hiCharterId = CataclysmCompatibleIncreaseGuid( m_hiCharterId );
	Charter * c = new Charter(m_hiCharterId, LeaderGuid, Type);
	m_charters[c->CharterType][ c->GetID() ] = c;
	m_charterLock.ReleaseWriteLock();
	return c;
}

Charter::Charter(Field * fields)
{
	uint32 f = 0;
	CharterId = fields[f++].GetUInt32();
	CharterType = fields[f++].GetUInt32();
	LeaderGuid = fields[f++].GetUInt32();
	GuildName = fields[f++].GetString();
	ItemGuid = fields[f++].GetUInt64();
	SignatureCount = 0;
	Slots = GetNumberOfSlotsByType();
	Signatures = new uint32[Slots];

	for(uint32 i = 0; i < Slots; ++i)
	{
		Signatures[i] = fields[f++].GetUInt32();
		if(Signatures[i])
			++SignatureCount;
	}
}

void Charter::AddSignature(uint32 PlayerGuid)
{
	if(SignatureCount >= Slots)
	{ 
		return;
	}

	SignatureCount++;
	uint32 i;
	for(i = 0; i < Slots; ++i)
	{
		if(Signatures[i] == 0)
		{
			Signatures[i] = PlayerGuid;
			break;
		}
	}

	assert(i != Slots);
}

void Charter::RemoveSignature(uint32 PlayerGuid)
{
	for(uint32 i = 0; i < Slots; ++i)
	{
		if(Signatures[i] == PlayerGuid)
		{
			Signatures[i] = 0;
			SignatureCount--;
			SaveToDB();
			break;
		}
	}
}


void Charter::Destroy()
{
	if( Slots == 0 )			// ugly hack because of f*cked memory
	{ 
		return;
	}

	//meh remove from objmgr
	objmgr.RemoveCharter(this);
	// Kill the players with this (in db/offline)
	CharacterDatabase.Execute( "DELETE FROM charters WHERE charterId = %u", CharterId );
	Player * p;
#if (defined( WIN32 ) || defined( WIN64 ) )
	__try {
#endif
	for( uint32 i = 0; i < Slots; ++i )
	{
		if(!Signatures[i])
			continue;
		p =  objmgr.GetPlayer( Signatures[i] );
		if( p != NULL)
			p->m_charters[CharterType] = 0;
	}
#if (defined( WIN32 ) || defined( WIN64 ) )
	} __except(EXCEPTION_EXECUTE_HANDLER)
	{
		return;
	}
#endif

	// click, click, boom!
	delete this;
}

void Charter::SaveToDB()
{
	/*CharacterDatabase.Execute(
		"REPLACE INTO charters VALUES(%u,%u,'%s',"I64FMTD",%u,%u,%u,%u,%u,%u,%u,%u,%u)",
		CharterId,LeaderGuid,GuildName.c_str(),ItemGuid,Signatures[0],Signatures[1],
		Signatures[2],Signatures[3],Signatures[4],Signatures[5],
		Signatures[6],Signatures[7],Signatures[8]);*/
	std::stringstream ss;
	uint32 i;
	ss << "REPLACE INTO charters VALUES(" << CharterId << "," << CharterType << "," << LeaderGuid << ",'" << CharacterDatabase.EscapeString( GuildName ) << "'," << ItemGuid;

	for(i = 0; i < Slots; ++i)
		ss << "," << Signatures[i];

	for(; i < 9; ++i)
		ss << ",0";

	ss << ")";
	CharacterDatabase.Execute(ss.str().c_str());
}

Charter * ObjectMgr::GetCharterByItemGuid(const uint64 &guid)
{
	m_charterLock.AcquireReadLock();
	for(int i = 0; i < NUM_CHARTER_TYPES; ++i)
	{
		HM_NAMESPACE::hash_map<uint32, Charter*>::iterator itr = m_charters[i].begin();
		for(; itr != m_charters[i].end(); ++itr)
		{
			if(itr->second->ItemGuid == guid)
			{
				m_charterLock.ReleaseReadLock();
				return itr->second;
			}
		}
	}
	m_charterLock.ReleaseReadLock();
	return NULL;
}

Charter * ObjectMgr::GetCharterByGuid(const uint64 &playerguid, CharterTypes type)
{
	m_charterLock.AcquireReadLock();
	for(int i = 0; i < NUM_CHARTER_TYPES; ++i)
	{
		HM_NAMESPACE::hash_map<uint32, Charter*>::iterator itr = m_charters[i].begin();
		for(; itr != m_charters[i].end(); ++itr)
		{
			if(playerguid == itr->second->LeaderGuid)
			{
				m_charterLock.ReleaseReadLock();
				return itr->second;
			}

			for(uint32 j = 0; j < itr->second->SignatureCount; ++j)
			{
				if(itr->second->Signatures[j] == playerguid)
				{
					m_charterLock.ReleaseReadLock();
					return itr->second;
				}
			}
		}
	}
	m_charterLock.ReleaseReadLock();
	return NULL;
}

Charter * ObjectMgr::GetCharterByName(string &charter_name, CharterTypes Type)
{
	Charter * rv = 0;
	m_charterLock.AcquireReadLock();
	HM_NAMESPACE::hash_map<uint32, Charter*>::iterator itr = m_charters[Type].begin();
	for(; itr != m_charters[Type].end(); ++itr)
	{
		if(itr->second->GuildName == charter_name)
		{
			rv = itr->second;
			break;
		}
	}

	m_charterLock.ReleaseReadLock();
	return rv;
}

void ObjectMgr::RemoveCharter(Charter * c)
{
	if( c == NULL )
	{ 
		return;
	}
	if( c->CharterType > NUM_CHARTER_TYPES )
	{
		Log.Notice("ObjectMgr", "Charter %u cannot be destroyed as type %u is not a sane type value.", c->CharterId, c->CharterType );
		return;
	}
	m_charterLock.AcquireWriteLock();
	m_charters[c->CharterType].erase(c->CharterId);
	m_charterLock.ReleaseWriteLock();
}

void ObjectMgr::LoadReputationModifierTable(const char * tablename, ReputationModMap * dmap)
{
	QueryResult * result = WorldDatabase.Query("SELECT * FROM %s", tablename);
	ReputationModMap::iterator itr;
	ReputationModifier * modifier;
	ReputationMod mod;

	if(result)
	{
		do 
		{
			mod.faction[0] = result->Fetch()[1].GetUInt32();
			mod.faction[1] = result->Fetch()[2].GetUInt32();
			mod.value = result->Fetch()[3].GetInt32();
			mod.replimit = result->Fetch()[4].GetUInt32();

			itr = dmap->find(result->Fetch()[0].GetUInt32());
			if(itr == dmap->end())
			{
				modifier = new ReputationModifier;
				modifier->entry = result->Fetch()[0].GetUInt32();
				modifier->mods.push_back(mod);
				dmap->insert( ReputationModMap::value_type( result->Fetch()[0].GetUInt32(), modifier ) );
			}
			else
			{
				itr->second->mods.push_back(mod);
			}
		} while(result->NextRow());
		delete result;
		result = NULL;
	}
	Log.Notice("ObjectMgr", "%u reputation modifiers on %s.", dmap->size(), tablename);
}

void ObjectMgr::LoadChainedReputationModifiers()
{
	//changing one faction will change other factions
	QueryResult * result = WorldDatabase.Query("SELECT * FROM faction_dependencies");

	HM_NAMESPACE::hash_map<uint32, SimpleVect<ReputationToReputationMod *> *>::iterator itr;

	SimpleVect<ReputationToReputationMod *> *new_list;
	ReputationToReputationMod *mod;

	if(result)
	{
		do 
		{
			mod = new ReputationToReputationMod;
			mod->source_rep = result->Fetch()[0].GetUInt32();
			mod->target_rep = result->Fetch()[1].GetUInt32();
			mod->mod_factor = result->Fetch()[2].GetFloat();
			mod->mod_flat = result->Fetch()[3].GetInt32();
			mod->target_rep_limit = result->Fetch()[4].GetInt32();

			itr = m_reputation_chains.find( mod->source_rep );
			if( itr == m_reputation_chains.end() )
			{
				new_list = new SimpleVect<ReputationToReputationMod *>;
				new_list->push_back( mod );
				m_reputation_chains[ mod->source_rep ] = new_list;
			}
			else
			{
				itr->second->push_back( mod );
			}
		} while(result->NextRow());
		delete result;
		result = NULL;
	}
	Log.Notice("ObjectMgr", "%u reputation modifiers from faction_dependencies.", m_reputation_chains.size());
}

void ObjectMgr::LoadReputationModifiers()
{
	LoadReputationModifierTable("reputation_creature_onkill", &m_reputation_creature);
	LoadReputationModifierTable("reputation_faction_onkill", &m_reputation_faction);
	LoadInstanceReputationModifiers();
	LoadChainedReputationModifiers();
}

ReputationModifier * ObjectMgr::GetReputationModifier(uint32 entry_id, uint32 faction_id)
{
	// first, try fetching from the creature table (by faction is a fallback)
	ReputationModMap::iterator itr = m_reputation_creature.find(entry_id);
	if(itr != m_reputation_creature.end())
	{ 
		return itr->second;
	}

	// fetch from the faction table
	itr = m_reputation_faction.find(faction_id);
	if(itr != m_reputation_faction.end())
	{ 
		return itr->second;
	}

	// no data. fallback to default -5 value.
	return 0;
}

void ObjectMgr::LoadMonsterSay()
{
	QueryResult * result = WorldDatabase.Query("SELECT * FROM npc_monstersay");
	if(!result) 
	{ 
		return;
	}

	uint32 Entry, Event;
	Field * fields = result->Fetch();
	do 
	{
		Entry = fields[0].GetUInt32();
		Event = fields[1].GetUInt32();

		if(Event >= NUM_MONSTER_SAY_EVENTS)
			continue;

		if( mMonsterSays[Event].find( Entry ) != mMonsterSays[Event].end() )
		{
			sLog.outError("Duplicate monstersay event %u for entry %u, skipping", Event, Entry	);
			continue;
		}

		NpcMonsterSay * ms = new NpcMonsterSay;
		ms->Chance = fields[2].GetFloat();
		ms->Language = fields[3].GetUInt32();
		ms->Type = fields[4].GetUInt32();
		if( fields[5].GetString() )
		{
			ms->MonsterName = (char *)malloc( strlen( fields[5].GetString() ) + 1 );
			strcpy( (char *)ms->MonsterName, fields[5].GetString() );
		}
		else continue;

		char * texts[5];
		char * text;
		uint32 textcount = 0;

		for(uint32 i = 0; i < 5; ++i)
		{
			text = (char*)fields[6+i].GetString();
			if(!text) 
				continue;
			if(strlen( text ) < 5)
				continue;

			texts[textcount] = strdup_local(fields[6+i].GetString());

			// check for ;
			if(texts[textcount][strlen(texts[textcount])-1] == ';')
				texts[textcount][strlen(texts[textcount])-1] = 0;

			++textcount;
		}

		if(!textcount)
		{
			if( ms->MonsterName )
			{
				free(((char*)ms->MonsterName));
				ms->MonsterName = NULL;
			}
			delete ms;
			ms = NULL;
			continue;
		}

		ms->Texts = new char*[textcount];
		memcpy(ms->Texts, texts, sizeof(char*) * textcount);
		ms->TextCount = textcount;

		mMonsterSays[Event].insert( make_pair( Entry, ms ) );

	} while(result->NextRow());
	Log.Notice("ObjectMgr", "%u monster say events loaded.", result->GetRowCount());
	delete result;
	result = NULL;
}

void ObjectMgr::HandleMonsterSayEvent(Creature * pCreature, MONSTER_SAY_EVENTS Event)
{
	MonsterSayMap::iterator itr = mMonsterSays[Event].find(pCreature->GetEntry());
	if(itr == mMonsterSays[Event].end())
	{ 
		return;
	}

	NpcMonsterSay * ms = itr->second;
	if(RandChance(ms->Chance))
	{
		// chance successful.
		int choice = (ms->TextCount == 1) ? 0 : RandomUInt(ms->TextCount - 1);
		const char * text = ms->Texts[choice];
		pCreature->SendChatMessage(ms->Type, ms->Language, text);
	}
}

bool ObjectMgr::HasMonsterSay(uint32 Entry, MONSTER_SAY_EVENTS Event)
{
	MonsterSayMap::iterator itr = mMonsterSays[Event].find(Entry);
	if(itr == mMonsterSays[Event].end())
	{ 
		return false;
	}

	return true;
}

void ObjectMgr::LoadInstanceReputationModifiers()
{
	QueryResult * result = WorldDatabase.Query("SELECT * FROM reputation_instance_onkill");
	if(!result) 
	{ 
		return;
	}

	do 
	{
		Field * fields = result->Fetch();
		InstanceReputationMod mod;
		mod.mapid = fields[0].GetUInt32();
		mod.mob_rep_reward = fields[1].GetInt32();
		mod.mob_rep_limit = fields[2].GetUInt32();
		mod.boss_rep_reward = fields[3].GetInt32();
		mod.boss_rep_limit = fields[4].GetUInt32();
		mod.faction[0] = fields[5].GetUInt32();
		mod.faction[1] = fields[6].GetUInt32();

		HM_NAMESPACE::hash_map<uint32, InstanceReputationModifier*>::iterator itr = m_reputation_instance.find(mod.mapid);
		if(itr == m_reputation_instance.end())
		{
			InstanceReputationModifier * m = new InstanceReputationModifier;
			m->mapid = mod.mapid;
			m->mods.push_back(mod);
			m_reputation_instance.insert( make_pair( m->mapid, m ) );
		}
		else
			itr->second->mods.push_back(mod);

	} while(result->NextRow());
	delete result;
	result = NULL;
	Log.Notice("ObjectMgr", "%u instance reputation modifiers loaded.", m_reputation_instance.size());
}

bool ObjectMgr::HandleInstanceReputationModifiers(Player * pPlayer, Unit * pVictim)
{
	uint32 team = pPlayer->GetTeam();
	bool is_boss;
	if(pVictim->GetTypeId() != TYPEID_UNIT)
	{ 
		return false;
	}

	HM_NAMESPACE::hash_map<uint32, InstanceReputationModifier*>::iterator itr = m_reputation_instance.find(pVictim->GetMapId());
	if(itr == m_reputation_instance.end())
	{ 
		return false;
	}

	is_boss = 0;//SafeCreatureCast( pVictim )->GetCreatureInfo() ? ((Creature*)pVictim)->GetCreatureInfo()->Rank : 0;
	if( !is_boss && SafeCreatureCast( pVictim )->GetProto() && SafeCreatureCast( pVictim )->GetProto()->boss )
		is_boss = 1;

	// Apply the bonuses as normal.
	int32 replimit;
	int32 value;

	for(vector<InstanceReputationMod>::iterator i = itr->second->mods.begin(); i !=  itr->second->mods.end(); ++i)
	{
		if(!(*i).faction[team])
			continue;

		if(is_boss)
		{
			value = i->boss_rep_reward;
			replimit = i->boss_rep_limit;
		}
		else
		{
			value = i->mob_rep_reward;
			replimit = i->mob_rep_limit;
		}

		if(!value || (replimit && pPlayer->GetStanding(i->faction[team]) >= replimit))
			continue;

		//value *= sWorld.getRate(RATE_KILLREPUTATION);
		value = float2int32(float(value) * sWorld.getRate(RATE_KILLREPUTATION));
		pPlayer->ModStanding(i->faction[team], value);
	}

	return true;
}

void ObjectMgr::LoadDisabledSpells()
{
	QueryResult * result = WorldDatabase.Query("SELECT * FROM spell_disable");
	uint32 counter = 0;
	if(result)
	{
		do 
		{
			SpellEntry * sp = dbcSpell.LookupEntryForced( result->Fetch()[0].GetUInt32() );
			if( sp )
				sp->c_is_flags |= SPELL_FLAG_IS_DISABLED;
			counter++;
		} while(result->NextRow());
		delete result;
		result = NULL;
	}

	Log.Notice("ObjectMgr", "%u disabled spells.", counter);
}

void ObjectMgr::ReloadDisabledSpells()
{
	for(uint32 x=0; x < dbcSpell.GetNumRows(); x++)
	{
		SpellEntry * sp = dbcSpell.LookupRow(x);
		sp->c_is_flags &= ~SPELL_FLAG_IS_DISABLED;
	}	
	LoadDisabledSpells();
}

void ObjectMgr::LoadGroups()
{
	QueryResult * result = CharacterDatabase.Query("SELECT * FROM groups");
	if(result)
	{
		if(result->GetFieldCount() != 51)
		{
			Log.LargeErrorMessage(LARGERRORMESSAGE_WARNING, "groups table format is invalid. Please update your database.");
			return;
		}
		do 
		{
			Group * g = new Group(false);
			g->LoadFromDB(result->Fetch());
		} while(result->NextRow());
		delete result;
		result = NULL;
	}

	Log.Notice("ObjectMgr", "%u groups loaded.", this->m_groups.size());
}

void ObjectMgr::LoadArenaTeams()
{
	QueryResult * result = CharacterDatabase.Query("SELECT * FROM arenateams");
	if(result != NULL)
	{
		if(result->GetFieldCount() != 22)
		{
			Log.LargeErrorMessage(LARGERRORMESSAGE_WARNING, "arenateams table format is invalid. Please update your database.");
			return;
		}
		do 
		{
			ArenaTeam * team = new ArenaTeam(result->Fetch());
			if( team->m_memberCount <= 0 )
				team->Destroy();
			else
			{
				AddArenaTeam( team );
				if(team->m_id > m_hiArenaTeamId)
					m_hiArenaTeamId=team->m_id;
			}
			team = NULL; // decrease reference to pointer fro garbage collector ?
		} while(result->NextRow());
		delete result;
		result = NULL;
	}

	/* update the ranking */
	UpdateArenaTeamRankings();
}

ArenaTeam * ObjectMgr::GetArenaTeamByGuid(uint32 guid, uint32 Type)
{
	m_arenaTeamLock.Acquire();
	for(HM_NAMESPACE::hash_map<uint32,ArenaTeam*>::iterator itr = m_arenaTeamMap[Type].begin(); itr != m_arenaTeamMap[Type].end(); ++itr)
	{
		if(itr->second->HasMember(guid))
		{
			m_arenaTeamLock.Release();
			return itr->second;
		}
	}
	m_arenaTeamLock.Release();
	return NULL;
}

ArenaTeam * ObjectMgr::GetArenaTeamById(uint32 id)
{
	HM_NAMESPACE::hash_map<uint32, ArenaTeam*>::iterator itr;
	m_arenaTeamLock.Acquire();
	itr = m_arenaTeams.find(id);
	m_arenaTeamLock.Release();
	return (itr == m_arenaTeams.end()) ? NULL : itr->second;
}

ArenaTeam * ObjectMgr::GetArenaTeamByName(string & name, uint32 Type)
{
	m_arenaTeamLock.Acquire();
	for(HM_NAMESPACE::hash_map<uint32, ArenaTeam*>::iterator itr = m_arenaTeams.begin(); itr != m_arenaTeams.end(); ++itr)
	{
		if(!strnicmp(itr->second->m_name.c_str(), name.c_str(), name.size()))
		{
			m_arenaTeamLock.Release();
			return itr->second;
		}
	}
	m_arenaTeamLock.Release();
	return NULL;
}

void ObjectMgr::RemoveArenaTeam(ArenaTeam * team)
{
	m_arenaTeamLock.Acquire();
	m_arenaTeams.erase(team->m_id);
	m_arenaTeamMap[team->m_type].erase(team->m_id);
	m_arenaTeamLock.Release();
}

void ObjectMgr::AddArenaTeam(ArenaTeam * team)
{
	m_arenaTeamLock.Acquire();
	m_arenaTeams[team->m_id] = team;
	m_arenaTeamMap[team->m_type][team->m_id] = team;
	m_arenaTeamLock.Release();
}

class ArenaSorter
{
public:
	bool operator()(ArenaTeam* const & a,ArenaTeam* const & b)
	{
		return (a->m_stat_rating > b->m_stat_rating);
	}
		bool operator()(ArenaTeam*& a, ArenaTeam*& b)
		{
				return (a->m_stat_rating > b->m_stat_rating);
		}
};

void ObjectMgr::UpdateArenaTeamRankings()
{
	m_arenaTeamLock.Acquire();
	for(uint32 i = 0; i < NUM_ARENA_TEAM_TYPES; ++i)
	{
		vector<ArenaTeam*> ranking;
		
		for(HM_NAMESPACE::hash_map<uint32,ArenaTeam*>::iterator itr = m_arenaTeamMap[i].begin(); itr != m_arenaTeamMap[i].end(); ++itr)
			ranking.push_back(itr->second);

		std::sort(ranking.begin(), ranking.end(), ArenaSorter());
		uint32 rank = 1;
		for(vector<ArenaTeam*>::iterator itr = ranking.begin(); itr != ranking.end(); ++itr)
		{
			if((*itr)->m_stat_ranking != rank)
			{
				(*itr)->m_stat_ranking = rank;
				(*itr)->SaveToDB();
			}
			++rank;
		}
	}
	m_arenaTeamLock.Release();
}

void ObjectMgr::ResetArenaTeamRatings()
{
	m_arenaTeamLock.Acquire();
	for(uint32 i = 0; i < NUM_ARENA_TEAM_TYPES; ++i)
	{
		for(HM_NAMESPACE::hash_map<uint32,ArenaTeam*>::iterator itr = m_arenaTeamMap[i].begin(); itr != m_arenaTeamMap[i].end(); ++itr)
		{
			ArenaTeam *team = itr->second;
			if(team)
			{
				team->m_stat_gamesplayedseason = 0;
				team->m_stat_gamesplayedweek = 0;
				team->m_stat_gameswonseason = 0;
				team->m_stat_gameswonweek = 0;
				team->m_stat_rating = ARENA_TEAM_START_RATING;
				for(uint32 j = 0; j < team->m_memberCount; ++j)
				{
					team->m_members[j].Played_ThisSeason = 0;
					team->m_members[j].Played_ThisWeek = 0;
					team->m_members[j].Won_ThisSeason = 0;
					team->m_members[j].Won_ThisWeek = 0;
					team->m_members[j].PersonalRating = ARENA_PERSONAL_START_RATING;
				}
				team->SaveToDB();
			}
		}
	}
	m_arenaTeamLock.Release();

	UpdateArenaTeamRankings();
}

void ObjectMgr::UpdateArenaTeamWeekly()
{	// reset weekly matches count for all teams and all members
	m_arenaTeamLock.Acquire();
	for(uint32 i = 0; i < NUM_ARENA_TEAM_TYPES; ++i)
	{
		for(HM_NAMESPACE::hash_map<uint32,ArenaTeam*>::iterator itr = m_arenaTeamMap[i].begin(); itr != m_arenaTeamMap[i].end(); ++itr)
		{
			ArenaTeam *team = itr->second;
			if(team)
			{
				team->m_stat_gamesplayedweek = 0;
				team->m_stat_gameswonweek = 0;
				for(uint32 j = 0; j < team->m_memberCount; ++j)
				{
					team->m_members[j].Played_ThisWeek = 0;
					team->m_members[j].Won_ThisWeek = 0;
				}
				team->SaveToDB();
			}
		}
	}
	m_arenaTeamLock.Release();
}

void ObjectMgr::ResetDailies()
{
	_playerslock.AcquireReadLock();
	PlayerStorageMap::iterator itr = _players.begin();
	for(; itr != _players.end(); itr++)
	{
		Player * pPlayer = itr->second;
		pPlayer->DailyMutex.Acquire();
		pPlayer->m_finishedDailies.clear();
		pPlayer->DailyMutex.Release();
	}
	_playerslock.ReleaseReadLock();
#ifdef PVP_AREANA_REALM
	ResetDailyPVPArenaTeamFarm();
#endif
}

void ObjectMgr::ResetWeeklies()
{
	_playerslock.AcquireReadLock();
	PlayerStorageMap::iterator itr = _players.begin();
	for(; itr != _players.end(); itr++)
	{
		Player * pPlayer = itr->second;
		pPlayer->DailyMutex.Acquire();
		pPlayer->m_finishedWeeklies.clear();
		pPlayer->DailyMutex.Release();
	}
	_playerslock.ReleaseReadLock();
}

void ObjectMgr::LoadQuestPOI()
{
    uint32 count = 0;

	QueryResult *result = WorldDatabase.Query("SELECT entry,entry_POI,objective,mapid,unk1,unk2,unk3,unk4 FROM quest_poi order by entry");

    if(!result)
        return;

    do
    {
        Field *fields = result->Fetch();

        uint32 questId    = fields[0].GetUInt32();
        uint32 entry_poi  = fields[1].GetUInt32();
        int32 objIndex    = fields[2].GetInt32();
        uint32 mapId      = fields[3].GetUInt32();
        uint32 unk1       = fields[4].GetUInt32();
        uint32 unk2       = fields[5].GetUInt32();
        uint32 unk3       = fields[6].GetUInt32();
        uint32 unk4       = fields[7].GetUInt32();

        QuestPOI POI(objIndex, mapId, unk1, unk2, unk3, unk4);

        QueryResult *points = WorldDatabase.Query("SELECT x, y FROM quest_poi_points WHERE entry_quest='%u' AND entry_poi='%i'", questId, entry_poi);

        if(points)
        {
            do 
            {
                Field *pointFields = points->Fetch();
                int32 x = pointFields[0].GetInt32();
                int32 y = pointFields[1].GetInt32();
                QuestPOIPoint point(x, y);
                POI.points.push_back(point);
            } while (points->NextRow());

            delete points;
        }

        mQuestPOIMap[questId].push_back(POI);

        ++count;
    } while (result->NextRow());

    delete result;
	Log.Notice("ObjectMgr", "Loaded %u quest POI rows", count);
}


#ifdef VOICE_CHAT
void ObjectMgr::GroupVoiceReconnected()
{
	m_groupLock.AcquireReadLock();
	for(GroupMap::iterator itr = m_groups.begin(); itr != m_groups.end(); ++itr)
		itr->second->VoiceSessionReconnected();
	m_groupLock.ReleaseReadLock();
}
#endif

void ObjectMgr::AddIPMute( const char *IP, uint32 duration_seconds )
{
	unsigned char ipbytes[4*4 + 16];	//watch it, it is bytes and we are reading ints
	uint32 *mute_IP = (uint32 *)&ipbytes[0];
	if( IP == NULL )
		return;
	sscanf(IP, "%u.%u.%u.%u", &ipbytes[0], &ipbytes[1], &ipbytes[2], &ipbytes[3] ); 
	uint32 expiresat = (uint32)UNIXTIME + duration_seconds;
	if( IP_mute_expires[ *mute_IP ] > expiresat )
		return;
	IP_mute_expires[ *mute_IP ] = expiresat;
	QueryResult *result = CharacterDatabase.Query( "replace into IP_mutes (IP,expire_at) values( '%s',%d)",CharacterDatabase.EscapeString( IP ).c_str(),expiresat );
	if( result )
		delete result;
}
void ObjectMgr::RemoveIPMute( const char *IP )
{
	unsigned char ipbytes[4*4 + 16];	//watch it, it is bytes and we are reading ints
	uint32 *mute_IP = (uint32 *)&ipbytes[0];
	if( IP == NULL )
		return;
	sscanf(IP, "%u.%u.%u.%u", &ipbytes[0], &ipbytes[1], &ipbytes[2], &ipbytes[3] ); 
	IP_mute_expires[ *mute_IP ] = 0;
	QueryResult *result = CharacterDatabase.Query( "delete from IP_mutes where IP like '%s'",CharacterDatabase.EscapeString( IP ).c_str() );
	if( result )
		delete result;
}
uint32 ObjectMgr::GetIPMuteExpire( const char *IP )
{
	unsigned char ipbytes[4*4 + 16];	//watch it, it is bytes and we are reading ints
	uint32 *mute_IP = (uint32 *)&ipbytes[0];
	if( IP == NULL )
		return 0;
	sscanf(IP, "%u.%u.%u.%u", &ipbytes[0], &ipbytes[1], &ipbytes[2], &ipbytes[3] ); 
	return IP_mute_expires[ *mute_IP ];
}

void ObjectMgr::LoadIPMute( )
{
	QueryResult *result = CharacterDatabase.Query( "delete FROM IP_mutes where expire_at<=%u",UNIXTIME );
	result = CharacterDatabase.Query( "SELECT IP,expire_at FROM IP_mutes " );
	if(result == 0)
	{ 
		return;
	}

	do
	{
		Field *fields = result->Fetch();
		AddIPMute( fields[0].GetString(),fields[1].GetUInt32());
	} while( result->NextRow() );

	Log.Notice("ObjectMgr", "%u IP mutes loaded.", result->GetRowCount());
	delete result;
	result = NULL;
}

uint32 ObjectMgr::PlayerHasDeserterCooldown( const uint64 &GUID )
{
	uint32 EndingStamp = m_PlayerArenaDeserterCooldown[ (uint32)GUID ];
	if( EndingStamp <= getMSTime() )
		return 0;
	return ( EndingStamp - getMSTime() );
}

void ObjectMgr::PlayerAddDeserterCooldown( const uint64 &GUID )
{
#ifdef _DEBUG
	m_PlayerArenaDeserterCooldown[ (uint32)GUID ] = getMSTime() + 1 * 30 * 1000; // have mercy
#else
	m_PlayerArenaDeserterCooldown[ (uint32)GUID ] = getMSTime() + 15 * 60 * 1000; // blizzlike
//	m_PlayerArenaDeserterCooldown[ (uint32)GUID ] = getMSTime() + 5 * 60 * 1000; // have mercy
#endif
}

void ObjectMgr::OnArenaTeamJoinedQueue( ArenaTeam *team, Group *group )
{
	if( team == NULL )
		return;

//	uint32 AnnounceRatingLimit = team->m_stat_rating * 70 / 100;
	uint32 AnnounceRatingLimit = team->m_stat_rating;
	char Msg[500];
	sprintf( Msg, "Arenaqueu - join : %s(%d) : ", team->m_name.c_str(), team->m_stat_rating );
	uint32 count = group->GetSubGroupCount();
	for( uint8 k = 0; k < count; k++ )
	{
		SubGroup * subgroup = group->GetSubGroup( k );
		if( subgroup )
		{
			group->Lock();
			for( GroupMembersSet::iterator itr = subgroup->GetGroupMembersBegin(); itr != subgroup->GetGroupMembersEnd(); ++itr )
			{
				if( (*itr)->m_loggedInPlayer )
				{
					ArenaTeamMember *m = team->GetMemberByGuid( (*itr)->m_loggedInPlayer->GetGUID() );
					sprintf( Msg, "%s %s(%d) : ", Msg, (*itr)->m_loggedInPlayer->GetName(), m->PersonalRating );
				}
			}
			group->Unlock();
		}
	}

	HM_NAMESPACE::hash_map<uint32, ArenaTeam*>::iterator arenaitr;
	for( arenaitr = m_arenaTeams.begin(); arenaitr != m_arenaTeams.end(); arenaitr++)
	{
		if( 
//			arenaitr->second->m_stat_rating < AnnounceRatingLimit && 
			arenaitr->second->m_type == team->m_type )
		{
			int membercount = MIN( 10, arenaitr->second->m_memberCount );
			for( int32 i=0;i<membercount; i++ )
			{
				if( arenaitr->second->m_members[i].Info == NULL )
					continue;
				if( arenaitr->second->m_members[i].Info->m_loggedInPlayer )
					arenaitr->second->m_members[i].Info->m_loggedInPlayer->BroadcastMessage( Msg );
			}
		}
	}
}

void ObjectMgr::ListArenaMatches( Player *p, uint32 TypeFilter, ArenaTeam	**buffer, uint32 BufferSlots )
{
	uint32 SlotCounter = 0;
	std::set<CBattleground*> ListedBG;
	HM_NAMESPACE::hash_map<uint32, ArenaTeam*>::iterator arenaitr;
	for( arenaitr = m_arenaTeams.begin(); arenaitr != m_arenaTeams.end(); arenaitr++)
	{
		CBattleground *HasPlayersInArena = NULL;
		int membercount = MIN( 10, arenaitr->second->m_memberCount );
		for( int32 i=0;i<membercount; i++ )
		{
			if( arenaitr->second->m_members[i].Info == NULL )
				continue;
			if( arenaitr->second->m_members[i].Info->m_loggedInPlayer == NULL )
				continue;
			if( arenaitr->second->m_members[i].Info->m_loggedInPlayer->IsSpectator() )
				continue;
			if( arenaitr->second->m_members[i].Info->m_loggedInPlayer->m_bg == NULL )
				continue;
			uint32 BGType = arenaitr->second->m_members[i].Info->m_loggedInPlayer->m_bg->GetType();
			if( TypeFilter != 0 && BGType != TypeFilter )
				continue;
			if( BGType != BATTLEGROUND_ARENA_2V2 && BGType != BATTLEGROUND_ARENA_3V3 && BGType != BATTLEGROUND_ARENA_5V5 )
				continue;
			HasPlayersInArena = arenaitr->second->m_members[i].Info->m_loggedInPlayer->m_bg;
			break;
		}
		if( HasPlayersInArena != NULL )
		{
			if( buffer )
			{
				buffer[ SlotCounter ] = arenaitr->second;
				SlotCounter++;
				if( SlotCounter == BufferSlots )
					break;
				//only list each BG once
				if( ListedBG.find( HasPlayersInArena ) != ListedBG.end() )
					continue;
//				ListedBG.insert( HasPlayersInArena );
			}
			else
			{
				char Msg[500];
				sprintf( Msg, "Team %s : ", arenaitr->second->m_name.c_str() );
				for( int32 i=0;i<membercount; i++ )
					if( arenaitr->second->m_members[i].Info->m_loggedInPlayer 
						&& arenaitr->second->m_members[i].Info->m_loggedInPlayer->m_bg )
						sprintf( Msg, "%s %s ", Msg, arenaitr->second->m_members[i].Info->m_loggedInPlayer->GetName() );
				p->BroadcastMessage( Msg ); 
			}
		}
	}
}


#ifdef PVP_AREANA_REALM
uint32 ObjectMgr::GetIncreasePVPDiminishing( uint64 A1, uint64 A2, uint8 Increase )
{
	uint64 Key = A1 * A2;
	uint32 KillsToday = m_PVPKillsTodayArenaTeamPair[ Key ];
	if( Increase != 0 )
		KillsToday++;
	m_PVPKillsTodayArenaTeamPair[ Key ] = KillsToday;
	return KillsToday;
}

uint32 ObjectMgr::GetPVPGoldDiminishing( uint32 A1 )
{
	return m_PVPGoldGainPlayer[ A1 ];
}

uint32 ObjectMgr::ModPVPGoldDiminishing( uint32 A1, uint32 GoldGain )
{
	uint32 GoldToday = m_PVPGoldGainPlayer[ A1 ];
	GoldToday += GoldGain;
	m_PVPGoldGainPlayer[ A1 ] = GoldToday;
	return GoldToday;
}

void ObjectMgr::ResetDailyPVPArenaTeamFarm()
{
	m_PVPKillsTodayArenaTeamPair.clear();
	m_PVPGoldGainPlayer.clear();
}
#endif

#if 0
void ObjectMgr::GetCurrencyLoot( uint32 entry, Player *killer )
{
	if( killer == NULL || killer->GetMapMgr() == NULL )
		return;
//	bool mob_is_boss = false;
/*	uint8 level = killer->getLevel();
	uint32 difficulty_mask = 1 << (killer->GetMapMgr()->instance_difficulty + 2 );
	for(uint32 it=0;it<m_currency_loot.GetMaxSize();it++)
	{
		LootCurencyStoreStruct *ls = m_currency_loot.GetValue( it );
		if( ls->entry == entry && ls->max_level >= level && (ls->difficulty_mask & difficulty_mask) != 0 )
		{
			killer->ModCurrencyCount( ls->currency_type, ls->currency_amt );
			//!!!!!!!!!!!need to remake this !!!!!!!!!
//			mob_is_boss = true;
		}
	}
	//!!!!!!!!!!!need to remake this !!!!!!!!!
/*	if( mob_is_boss == true && killer->m_playerInfo != NULL && killer->m_playerInfo->guild != NULL )
	{
		uint32 guild_xp_share = killer->getLevel();
		killer->m_playerInfo->guild->GiveXP( guild_xp_share );
		sStackWorldPacket( data, SMSG_LOG_GUILD_XPGAIN, 8 + 10 );
		data << uint64( guild_xp_share );
		killer->GetSession()->SendPacket( &data );
	} */

	CreatureProto *proto = CreatureProtoStorage.LookupEntry( entry );
	if( proto == NULL )
		return;
	SimpleVect< LootCurencyStoreStruct* >		*loot_currency = proto->loot_currency;
	if( loot_currency == NULL )
		return;
	uint8 level = killer->getLevel();
	uint32 difficulty_mask = 1 << (killer->GetMapMgr()->instance_difficulty + 2 );
	uint32 max_size = loot_currency->GetMaxSize();
	for(uint32 it=0;it<max_size;it++)
	{
		LootCurencyStoreStruct *ls = loot_currency->GetValue( it );
		if( ls->entry == entry && ls->max_level >= level && (ls->difficulty_mask & difficulty_mask) != 0 )
		{
			killer->ModCurrencyCount( ls->currency_type, ls->currency_amt );
		}
	}
}
#endif