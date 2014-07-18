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
initialiseSingleton( QuestMgr );

void WorldSession::HandleQuestgiverStatusQueryOpcode( WorldPacket & recv_data )
{
	sLog.outDebug( "WORLD: Received CMSG_QUESTGIVER_STATUS_QUERY." );
	if(!_player) return;
    if(!_player->IsInWorld()) return;

	uint64 guid;
	sStackWorldPacket( data, SMSG_QUESTGIVER_STATUS, 50 );
    Object *qst_giver = NULL;

	recv_data >> guid;
	uint32 guidtype = GET_TYPE_FROM_GUID(guid);
    if(guidtype==HIGHGUID_TYPE_UNIT)
    {
        Creature *quest_giver = _player->GetMapMgr()->GetCreature( guid );
        if(quest_giver)
			qst_giver = quest_giver;
		else
			return;

        if (!quest_giver->isQuestGiver())
	    {
		    sLog.outDebug("WORLD: Creature is not a questgiver.");
		    return;
	    }
    }
    else if(guidtype==HIGHGUID_TYPE_ITEM)
	{
		Item *quest_giver = GetPlayer()->GetItemInterface()->GetItemByGUID(guid);
#ifndef SMALL_ITEM_OBJECT
		if(quest_giver)
			qst_giver = quest_giver;
		else
			return;
#else
		if(quest_giver)
		{
			data << guid << sQuestMgr.CalcStatus(qst_giver, GetPlayer());
			SendPacket( &data );
		}
		return;
#endif
	}
    else if(guidtype==HIGHGUID_TYPE_GAMEOBJECT)
	{
		GameObject *quest_giver = _player->GetMapMgr()->GetGameObject( guid );
		if(quest_giver)
			qst_giver = quest_giver;
		else
			return;
	}

	if (!qst_giver)
	{
		sLog.outDebug("WORLD: Invalid questgiver GUID "I64FMT".", guid);
		return;
	}

	data << guid << sQuestMgr.CalcStatus(qst_giver, GetPlayer());
	SendPacket( &data );
}

void WorldSession::HandleQuestgiverHelloOpcode( WorldPacket & recv_data )
{
	sLog.outDebug( "WORLD: Received CMSG_QUESTGIVER_HELLO." );

//sLog.outDebug("Disabled until fixed.Crashed client");
//return;
    if(!_player) 
		return;
	if(!_player->IsInWorld()) 
		return;

	uint64 guid;

	recv_data >> guid;

	Creature *qst_giver = _player->GetMapMgr()->GetCreature( guid );

	if (!qst_giver)
	{
		sLog.outDebug("WORLD: Invalid questgiver GUID.");
		return;
	}

	if (!qst_giver->isQuestGiver())
	{
		sLog.outDebug("WORLD: Creature is not a questgiver.");
		return;
	}

	/*if(qst_giver->GetAIInterface()) // NPC Stops moving for 3 minutes
		qst_giver->GetAIInterface()->StopMovement(180000);*/

	//qst_giver->Emote(EMOTE_ONESHOT_TALK); // this doesnt work
	sQuestMgr.OnActivateQuestGiver(qst_giver, GetPlayer());
}

void WorldSession::HandleQuestGiverQueryQuestOpcode( WorldPacket & recv_data )
{
	sLog.outDebug( "WORLD: Received CMSG_QUESTGIVER_QUERY_QUEST." );
    if(!_player) return;
	if(!_player->IsInWorld()) return;

	WorldPacket data;
	uint64 guid;
	uint32 quest_id;
	uint32 status = 0;
	uint8 unk;

	recv_data >> guid;
	recv_data >> quest_id;
	recv_data >> unk;

	Object *qst_giver = NULL;

	bool bValid = false;
	Quest* qst = QuestStorage.LookupEntry(quest_id);
	
	if (!qst)
	{
		sLog.outDebug("WORLD: Invalid quest ID.");
		return;
	}

	uint32 guidtype = GET_TYPE_FROM_GUID(guid);
	if(guidtype==HIGHGUID_TYPE_UNIT)
	{
		Creature *quest_giver = _player->GetMapMgr()->GetCreature( guid );
		if(quest_giver)
			qst_giver = quest_giver;
		else
			return;
		bValid = quest_giver->isQuestGiver();
		if(bValid)
			status = sQuestMgr.CalcQuestStatus( GetPlayer(), qst, (uint8)quest_giver->GetQuestRelation(qst->id), false);
	} 
	else if(guidtype==HIGHGUID_TYPE_GAMEOBJECT)
	{
		GameObject *quest_giver = _player->GetMapMgr()->GetGameObject( guid );
		if(quest_giver)
			qst_giver = quest_giver;
		else
			return;
		bValid = quest_giver->isQuestGiver();
		if(bValid)
			status = sQuestMgr.CalcQuestStatus( GetPlayer(), qst, (uint8)quest_giver->GetQuestRelation(qst->id), false);
	} 
	else if(guidtype==HIGHGUID_TYPE_ITEM)
	{
		Item *quest_giver = GetPlayer()->GetItemInterface()->GetItemByGUID(guid);
		if(quest_giver)
			qst_giver = quest_giver;
		else
			return;
		bValid = true;
		status = sQuestMgr.CalcQuestStatus( GetPlayer(), qst, 1, false);
	}
	
	if (!qst_giver)
	{
		sLog.outDebug("WORLD: Invalid questgiver GUID.");
		return;
	}

	if (!bValid)
	{
		sLog.outDebug("WORLD: object is not a questgiver.");
		return;
	}

	/*if (!qst_giver->FindQuest(quest_id, QUESTGIVER_QUEST_START | QUESTGIVER_QUEST_END))
	{
		sLog.outDebug("WORLD: QuestGiver doesn't have that quest.");
		return;
	}*/	// bleh.. not needed.. maybe for antihack later on would be a good idea though
	
	if ((status == QMGR_QUEST_AVAILABLE) || (status == QMGR_QUEST_REPEATABLE) || (status == QMGR_QUEST_CHAT))
	{
		sQuestMgr.BuildQuestDetails(&data, qst,qst_giver,1, language, _player);	 // 0 because we want goodbye to function
		SendPacket(&data);

		if( qst->quest_flags & QUEST_FLAGS_AUTO_ACCEPT )
			sQuestMgr.TryAutoAcceptQuest( this, guid, quest_id );

		sLog.outDebug( "WORLD: Sent SMSG_QUESTGIVER_QUEST_DETAILS." );
	}
	/*else if (status == QMGR_QUEST_FINISHED)
	{
		sQuestMgr.BuildOfferReward(&data, qst, qst_giver, 1);
		SendPacket(&data);
		sLog.outDebug( "WORLD: Sent SMSG_QUESTGIVER_OFFER_REWARD." );
	}*/
	else if (status == QMGR_QUEST_NOT_FINISHED || status == QMGR_QUEST_FINISHED)
	{
		sQuestMgr.BuildRequestItems(&data, qst, qst_giver, status, language);
		SendPacket(&data);
		sLog.outDebug( "WORLD: Sent SMSG_QUESTGIVER_REQUEST_ITEMS." );
	}
}

void WorldSession::HandleQuestgiverAcceptQuestOpcode( WorldPacket & recv_data )
{
	sLog.outDebug( "WORLD: Received CMSG_QUESTGIVER_ACCEPT_QUEST" );
    if(!_player) return;
	if(!_player->IsInWorld()) return;

	//WorldPacket data;

	uint64 guid;
	uint32 quest_id;
	uint32 unk;

	recv_data >> guid;
	recv_data >> quest_id;
	recv_data >> unk;

	sQuestMgr.TryAutoAcceptQuest( this, guid, quest_id );
}

void WorldSession::HandleQuestgiverCancelOpcode(WorldPacket& recvPacket)
{
    OutPacket(SMSG_GOSSIP_COMPLETE, 0, NULL);

	sLog.outDebug("WORLD: Sent SMSG_GOSSIP_COMPLETE");
}

void WorldSession::HandleQuestlogRemoveQuestOpcode(WorldPacket& recvPacket)
{
	sLog.outDebug( "WORLD: Received CMSG_QUESTLOG_REMOVE_QUEST" );
    if(!_player) return;
	if(!_player->IsInWorld()) return;

	uint8 quest_slot;
	recvPacket >> quest_slot;
	if(quest_slot >= 25)
		return;

	QuestLogEntry *qEntry = GetPlayer()->GetQuestLogInSlot(quest_slot);
	if (!qEntry)
	{
		sLog.outDebug("WORLD: No quest in slot %d.", quest_slot);
		return;		
	}
	Quest *qPtr = qEntry->GetQuest();
	CALL_QUESTSCRIPT_EVENT(qEntry, OnQuestCancel)(GetPlayer());
	qEntry->Finish();

	// Remove all items given by the questgiver at the beginning
	for(uint32 i = 0; i < 4; ++i)
	{
		if(qPtr->receive_items[i])
			GetPlayer()->GetItemInterface()->RemoveItemAmt( qPtr->receive_items[i], 1 );
	}

	// Remove source item
	if(qPtr->srcitem)
		_player->GetItemInterface()->RemoveItemAmt( qPtr->srcitem, 1 );

	if(qPtr->time > 0)
		if (sEventMgr.HasEvent(_player,EVENT_TIMED_QUEST_EXPIRE))
			sEventMgr.RemoveEvents(_player, EVENT_TIMED_QUEST_EXPIRE); 	
	GetPlayer()->UpdateNearbyGameObjects();

	sHookInterface.OnQuestCancelled(_player, qPtr);
	GetPlayer()->Event_AchiementCriteria_Received( ACHIEVEMENT_CRITERIA_TYPE_QUEST_ABANDONED,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
}

void WorldSession::HandleQuestQueryOpcode( WorldPacket & recv_data )
{
    if(!_player) return;
	if(!_player->IsInWorld()) return;
	sLog.outDebug( "WORLD: Received CMSG_QUEST_QUERY" );

	uint32 quest_id;

	recv_data >> quest_id;

	Quest *qst = QuestStorage.LookupEntry(quest_id);

	if (!qst)
	{
		sLog.outDebug("WORLD: Invalid quest ID.");
		return;
	}

	WorldPacket *pkt = BuildQuestQueryResponse(qst);
	SendPacket(pkt);
	delete pkt;
	pkt = NULL;

	sLog.outDebug( "WORLD: Sent SMSG_QUEST_QUERY_RESPONSE." );
}

void WorldSession::HandleQuestgiverRequestRewardOpcode( WorldPacket & recv_data )
{
    if(!_player) return;
	if(!_player->IsInWorld()) return;
	sLog.outDebug( "WORLD: Received CMSG_QUESTGIVER_REQUESTREWARD_QUEST." );

	uint64 guid;
	uint32 quest_id;

	recv_data >> guid;
	recv_data >> quest_id;

	bool bValid = false;
	Quest *qst = NULL;
	Object *qst_giver = NULL;
	uint32 status = 0;
	uint32 guidtype = GET_TYPE_FROM_GUID(guid);

	if(guidtype==HIGHGUID_TYPE_UNIT)
	{
		Creature *quest_giver = _player->GetMapMgr()->GetCreature( guid );
		if(quest_giver)
			qst_giver = quest_giver;
		else
			return;
		bValid = quest_giver->isQuestGiver();
		if(bValid)
		{
			qst = quest_giver->FindQuest(quest_id, QUESTGIVER_QUEST_END);
			if(!qst)
				qst = quest_giver->FindQuest(quest_id, QUESTGIVER_QUEST_START);

			/*if(!qst) 
				qst = QuestStorage.LookupEntry(quest_id);*/
			if(!qst)
			{
				sLog.outError("WARNING: Cannot get reward for quest %u, as it doesn't exist at Unit %u.", quest_id, quest_giver->GetEntry());
				return;
			}
			status = sQuestMgr.CalcQuestStatus( GetPlayer(), qst, (uint8)quest_giver->GetQuestRelation(qst->id),false);
		}
	} 
	else if(guidtype==HIGHGUID_TYPE_GAMEOBJECT)
	{
		GameObject *quest_giver = _player->GetMapMgr()->GetGameObject( guid );
		if(quest_giver)
			qst_giver = quest_giver;
		else
			return; // oops..
		bValid = quest_giver->isQuestGiver();
		if(bValid)
		{
			qst = quest_giver->FindQuest(quest_id, QUESTGIVER_QUEST_END);
			/*if(!qst) sQuestMgr.FindQuest(quest_id);*/
			if(!qst)
			{
				sLog.outError("WARNING: Cannot get reward for quest %u, as it doesn't exist at GO %u.", quest_id, quest_giver->GetEntry());
				return;
			}
			status = sQuestMgr.CalcQuestStatus( GetPlayer(), qst, (uint8)quest_giver->GetQuestRelation(qst->id),false);
		}
	}

	if (!qst_giver)
	{
		sLog.outDebug("WORLD: Invalid questgiver GUID.");
		return;
	}

	if (!bValid || qst == NULL)
	{
		sLog.outDebug("WORLD: Creature is not a questgiver.");
		return;
	}

	if (status == QMGR_QUEST_FINISHED)
	{
        WorldPacket data;
		sQuestMgr.BuildOfferReward(&data, qst, qst_giver, 1, language, _player);
		SendPacket(&data);
		sLog.outDebug( "WORLD: Sent SMSG_QUESTGIVER_REQUEST_ITEMS." );
	}

	// if we got here it means we're cheating
}

void WorldSession::HandleQuestgiverCompleteQuestOpcode( WorldPacket & recvPacket )
{
    if(!_player) return;
	if(!_player->IsInWorld()) return;
	sLog.outDebug( "WORLD: Received CMSG_QUESTGIVER_COMPLETE_QUEST." );

	uint64 guid;
	uint32 quest_id;

	recvPacket >> guid;
	recvPacket >> quest_id;

	bool bValid = false;
	Quest *qst = NULL;
	Object *qst_giver = NULL;
	uint32 status = 0;
	uint32 guidtype = GET_TYPE_FROM_GUID(guid);

	if(guidtype==HIGHGUID_TYPE_UNIT)
	{
		Creature *quest_giver = _player->GetMapMgr()->GetCreature( guid );
		if(quest_giver)
			qst_giver = quest_giver;
		else
			return;
		bValid = quest_giver->isQuestGiver();
		if(bValid)
		{
			qst = quest_giver->FindQuest(quest_id, QUESTGIVER_QUEST_END);
			/*if(!qst) 
				sQuestMgr.FindQuest(quest_id);*/
			if(!qst)
			{
				sLog.outError("WARNING: Cannot complete quest %u, as it doesn't exist at Unit %u.", quest_id, quest_giver->GetEntry());
				return;
			}
			status = sQuestMgr.CalcQuestStatus( GetPlayer(), qst, (uint8)quest_giver->GetQuestRelation(qst->id),false);
		}
	} 
	else if(guidtype==HIGHGUID_TYPE_GAMEOBJECT)
	{
		GameObject *quest_giver = _player->GetMapMgr()->GetGameObject( guid );
		if(quest_giver)
			qst_giver = quest_giver;
		else
			return; // oops..
		bValid = quest_giver->isQuestGiver();
		if(bValid)
		{
			qst = quest_giver->FindQuest(quest_id, QUESTGIVER_QUEST_END);
			/*if(!qst) sQuestMgr.FindQuest(quest_id);*/
			if(!qst)
			{
				sLog.outError("WARNING: Cannot complete quest %u, as it doesn't exist at GO %u.", quest_id, quest_giver->GetEntry());
				return;
			}
			status = sQuestMgr.CalcQuestStatus( GetPlayer(), qst, (uint8)quest_giver->GetQuestRelation(qst->id),false);
		}
	}

	if (!qst_giver)
	{
		sLog.outDebug("WORLD: Invalid questgiver GUID.");
		return;
	}

	if (!bValid || qst == NULL)
	{
		sLog.outDebug("WORLD: Creature is not a questgiver.");
		return;
	}

	if (status == QMGR_QUEST_NOT_FINISHED || status == QMGR_QUEST_REPEATABLE)
	{
        WorldPacket data;
		sQuestMgr.BuildRequestItems(&data, qst, qst_giver, status, language);
		SendPacket(&data);
		sLog.outDebug( "WORLD: Sent SMSG_QUESTGIVER_REQUEST_ITEMS." );
	}

	if (status == QMGR_QUEST_FINISHED || status == QMGR_QUEST_REPEATABLE_FINISHED )
	{
		WorldPacket data;
		sQuestMgr.BuildOfferReward(&data, qst, qst_giver, 1, language, _player);
		SendPacket(&data);
		sLog.outDebug( "WORLD: Sent SMSG_QUESTGIVER_REQUEST_ITEMS." );
	}
	
	sHookInterface.OnQuestFinished(_player, qst, qst_giver);
}

void WorldSession::HandleQuestgiverChooseRewardOpcode(WorldPacket& recvPacket)
{
    if(!_player) return;
	if(!_player->IsInWorld()) return;
	sLog.outDebug( "WORLD: Received CMSG_QUESTGIVER_CHOOSE_REWARD." );

	uint64 guid;
	uint32 quest_id;
	uint32 reward_slot;

	recvPacket >> guid;
	recvPacket >> quest_id;
	recvPacket >> reward_slot;

	if( reward_slot >= 6 )
		return;

	bool bValid = false;
	Quest *qst = NULL;
	Object *qst_giver = NULL;
	uint32 guidtype = GET_TYPE_FROM_GUID(guid);

	if(guidtype==HIGHGUID_TYPE_UNIT)
	{
		Creature *quest_giver = _player->GetMapMgr()->GetCreature( guid );
		if(quest_giver)
			qst_giver = quest_giver;
		else
			return;
		bValid = quest_giver->isQuestGiver();
		if(bValid)
			qst = QuestStorage.LookupEntry(quest_id);
	} 
	else if(guidtype==HIGHGUID_TYPE_GAMEOBJECT)
	{
		GameObject *quest_giver = _player->GetMapMgr()->GetGameObject( guid );
		if(quest_giver)
			qst_giver = quest_giver;
		else
			return;
		//bValid = quest_giver->isQuestGiver();
		//if(bValid)
		bValid = true;
			qst = QuestStorage.LookupEntry(quest_id);
	}

	if (!qst_giver)
	{
		sLog.outDebug("WORLD: Invalid questgiver GUID.");
		return;
	}

	if (!bValid || qst == NULL)
	{
		sLog.outDebug("WORLD: Creature is not a questgiver.");
		return;
	}

	//FIXME: Some Quest givers talk in the end of the quest.
	//   qst_giver->SendChatMessage(CHAT_MSG_MONSTER_SAY,LANG_UNIVERSAL,qst->GetQuestEndMessage().c_str());
	QuestLogEntry *qle = _player->GetQuestLogForEntry(quest_id);

    if (!qle && !qst->is_repeatable)
	{
		sLog.outDebug("WORLD: QuestLogEntry not found.");
		return;
	}

	if (qle && !qle->CanBeFinished())
	{
		sLog.outDebug("WORLD: Quest not finished.");
		return;
	}

	// remove icon
	/*if(qst_giver->GetTypeId() == TYPEID_UNIT)
	{
		qst_giver->BuildFieldUpdatePacket(GetPlayer(), UNIT_DYNAMIC_FLAGS, qst_giver->GetUInt32Value(UNIT_DYNAMIC_FLAGS));
	}*/

    //check for room in inventory for all items
	if(!sQuestMgr.CanStoreReward(GetPlayer(),qst,reward_slot))
    {
        sQuestMgr.SendQuestFailed(FAILED_REASON_INV_FULL, qst, GetPlayer());
        return;
    }

	
	sQuestMgr.OnQuestFinished(GetPlayer(), qst, qst_giver, reward_slot);
	//if(qst_giver->GetTypeId() == TYPEID_UNIT) qst->LUA_SendEvent(SafeCreatureCast( qst_giver ),GetPlayer(),ON_QUEST_COMPLETEQUEST);

	if(qst->next_quest_id)
	{
        WorldPacket data(12);
		data.Initialize(CMSG_QUESTGIVER_QUERY_QUEST);
		data << guid;
		data << qst->next_quest_id;
		HandleQuestGiverQueryQuestOpcode(data);
	}
//	if(qst->time > 0)
//		GetPlayer()->timed_quest_slot = 0;
}

void WorldSession::HandlePushQuestToPartyOpcode(WorldPacket &recv_data)
{
    if(!_player) return;
	if(!_player->IsInWorld()) return;
	uint32 questid, status;
	recv_data >> questid;

	sLog.outDetail( "WORLD: Received CMSG_PUSHQUESTTOPARTY quest = %u", questid );

	Quest *pQuest = QuestStorage.LookupEntry(questid);
	if(pQuest)
	{
		Group *pGroup = _player->GetGroup();
		if(pGroup)
		{
			uint32 pguid = _player->GetLowGUID();
			SubGroup * sgr = _player->GetGroup() ?
				_player->GetGroup()->GetSubGroup(_player->GetSubGroup()) : 0;

			if(sgr)
			{
				_player->GetGroup()->Lock();
				GroupMembersSet::iterator itr;
				for(itr = sgr->GetGroupMembersBegin(); itr != sgr->GetGroupMembersEnd(); ++itr)
				{
					Player *pPlayer = (*itr)->m_loggedInPlayer;
					if(pPlayer && pPlayer->GetGUID() !=  pguid)
					{
						WorldPacket data( MSG_QUEST_PUSH_RESULT, 13 );
						data << pPlayer->GetGUID();
						data << uint32(QUEST_SHARE_MSG_SHARING_QUEST);
						data << uint8(0);
						_player->GetSession()->SendPacket(&data);

						uint32 response = 0;
						if(_player->GetDistance2dSq(pPlayer) > 100)
						{
							response = QUEST_SHARE_MSG_TOO_FAR;
							continue;
						}
						QuestLogEntry *qst = pPlayer->GetQuestLogForEntry(questid);
						if(qst)
						{
							response = QUEST_SHARE_MSG_HAVE_QUEST;
							continue;
						}
						status = sQuestMgr.PlayerMeetsReqs(pPlayer, pQuest, false);
						if(status != QMGR_QUEST_AVAILABLE && status != QMGR_QUEST_CHAT)
						{
							response = QUEST_SHARE_MSG_CANT_TAKE_QUEST;
							continue;
						}
						if(pPlayer->HasFinishedQuest(questid))
						{
							response = QUEST_SHARE_MSG_FINISH_QUEST;
							continue;
						}
						if(pPlayer->GetOpenQuestSlot() == -1)
						{
							response = QUEST_SHARE_MSG_LOG_FULL;
							continue;
						}
						//Anything more?
						if(pPlayer->DuelingWith)
						{
							response = QUEST_SHARE_MSG_BUSY;
							continue;
						}
						//CHECKS IF  ALREADY HAVE COMPLETED THE DAILY QUEST
//						if(pPlayer->HasFinishedDailyQuest(questid))
//						{
//							response = QUEST_SHARE_MSG_CANT_SHARE_TODAY;
//							continue;
//						}
						//CHECKS IF IS IN THE PARTY
//						if(!pGroup)
//						{
//							response = QUEST_SHARE_MSG_NOT_IN_PARTY;
//							continue;
//						}
						if(response > 0)
							sQuestMgr.SendPushToPartyResponse(_player, pPlayer, response);

						data.clear();
						sQuestMgr.BuildQuestDetails(&data, pQuest, pPlayer, 1, pPlayer->GetSession()->language, _player);
						pPlayer->GetSession()->SendPacket(&data);
						if( pQuest->quest_flags & QUEST_FLAGS_AUTO_ACCEPT )
						{
							uint64 guid = pPlayer->GetGUID();
							sQuestMgr.TryAutoAcceptQuest( this, guid, pQuest->id );
						}
						pPlayer->SetQuestSharer(pguid);
					}
				}
				_player->GetGroup()->Unlock();
			}
		}
	}
}

void WorldSession::HandleQuestPushResult(WorldPacket& recvPacket)
{
    if(!_player) return;
	if(!_player->IsInWorld()) return;
	uint64 guid;
	uint8 msg;
	recvPacket >> guid >> msg;

	sLog.outDetail( "WORLD: Received MSG_QUEST_PUSH_RESULT " );

	if(GetPlayer()->GetQuestSharer())
	{
		Player *pPlayer = objmgr.GetPlayer(GetPlayer()->GetQuestSharer());
		if(pPlayer)
		{
			WorldPacket data(MSG_QUEST_PUSH_RESULT, 13);
			data << guid;
			data << uint32(msg);
			data << uint8(0);
			pPlayer->GetSession()->SendPacket(&data);
			GetPlayer()->SetQuestSharer(0);
		}
	}
}


void WorldSession::HandleQuestQueryQuestFinishers( WorldPacket & recv_data )
{
/*
14333
{CLIENT} Packet: (0x98C8) CMSG_START_QUEST PacketSize = 8 TimeStamp = 1125235
01 00 00 00 1C 0C 00 00 
{SERVER} Packet: (0xCB21) UNKNOWN PacketSize = 16 TimeStamp = 1125594
01 00 00 00 01 00 00 00 8F 03 00 00 1C 0C 00 00 
*/
    if(!_player) 
		return;
	sLog.outDebug( "WORLD: Received CMSG_QUEST_QUERY_QUESTFINISHERS" );
	WorldPacket data( SMSG_QUEST_QUERY_QUESTFINISHERS, 2000 );
	WorldPacket data2( 0, 2000 );

	uint32 counter, tcounter;
	uint32 quest_id;

	recv_data >> counter;
	data << counter;
	tcounter = counter;
	while( tcounter-- && recv_data.size() > recv_data.rpos() )
	{
		recv_data >> quest_id;
		data2 << quest_id;

		if( sQuestMgr.m_quests_finisher_npc.find( quest_id ) != sQuestMgr.m_quests_finisher_npc.end() )
		{
			data << sQuestMgr.m_quests_finisher_npc[ quest_id ].size();
			list<uint32>::iterator itr2;
			for( itr2 = sQuestMgr.m_quests_finisher_npc[ quest_id ].begin(); itr2!=sQuestMgr.m_quests_finisher_npc[ quest_id ].end(); itr2++ )
				data << uint32( *itr2 );
		}
		else
			data << uint32( 0 );
	}
	data << data2;
	SendPacket( &data );
	sLog.outDebug( "WORLD: Sent SMSG_QUEST_QUERY_QUESTFINISHERS." );
}