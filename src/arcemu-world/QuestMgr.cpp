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

uint32 QuestMgr::CalcQuestStatus(Object* quest_giver, Player* plr, QuestRelation* qst)
{
	return CalcQuestStatus( plr, qst->qst, qst->type, false);
}

/*
bool QuestMgr::isRepeatableQuestFinished(Player *plr, Quest *qst)
{
	uint32 i;

	for(i = 0; i < 6; ++i)
	{
		if(qst->required_item[i])
		{
			if((int32)plr->GetItemInterface()->GetItemCount(qst->required_item[i]) < qst->required_itemcount[i])
			{
				return false;
			}
		}
	}
	//check for required currency
	for(i = 0; i < 4; ++i)
		if( qst->ReqCurrency[i].Type > 0 && plr->GetCurrencyCount( qst->ReqCurrency[i].Type ) < qst->ReqCurrency[i].Count )
		{
			return false;
		}

	return true;
}
*/

uint32 QuestMgr::PlayerMeetsReqs(Player* plr, Quest* qst, bool skiplevelcheck)
{
	std::list<uint32>::iterator itr;
	uint32 status;

	if (!sQuestMgr.IsQuestRepeatable(qst) && !sQuestMgr.IsQuestDaily(qst) && !sQuestMgr.IsQuestWeekly(qst))
		status = QMGR_QUEST_AVAILABLE;
	else
    {
		status = QMGR_QUEST_REPEATABLE;
		if(qst->is_repeatable == arcemu_QUEST_REPEATABLE_DAILY && plr->HasFinishedDaily(qst->id))
		{ 
			return QMGR_QUEST_NOT_AVAILABLE;
		}
		else if(qst->is_repeatable == arcemu_QUEST_REPEATABLE_WEEKLY && plr->HasFinishedWeekly(qst->id))
		{ 
			return QMGR_QUEST_NOT_AVAILABLE;
		}
    }

	if ((int32)plr->getLevel() < qst->min_level && !skiplevelcheck)
	{ 
		return QMGR_QUEST_AVAILABLELOW_LEVEL;
	}

	if(qst->required_class)
		if(!(qst->required_class & plr->getClassMask()))
		{ 
			return QMGR_QUEST_NOT_AVAILABLE;
		}

    if(qst->required_races)
    {
        if(!(qst->required_races & plr->getRaceMask()))
        { 
            return QMGR_QUEST_NOT_AVAILABLE;
        }
    }
	
	if(qst->required_tradeskill)
	{
		if(!plr->_HasSkillLine(qst->required_tradeskill))
		{ 
			return QMGR_QUEST_NOT_AVAILABLE;
		}
		if (qst->required_tradeskill_value && plr->_GetSkillLineCurrent(qst->required_tradeskill) < qst->required_tradeskill_value)
		{ 
			return QMGR_QUEST_NOT_AVAILABLE;
		}
	}

	// Check min reputation
	if(qst->required_min_rep_faction && qst->required_min_rep_value)
		if(plr->GetStanding(qst->required_min_rep_faction) < (int32)qst->required_min_rep_value)
		{ 
			return QMGR_QUEST_NOT_AVAILABLE;
		}


	// Check max reputation
	if(qst->required_max_rep_faction && qst->required_max_rep_value)
		if(plr->GetStanding(qst->required_max_rep_faction) > (int32)qst->required_max_rep_value)
		{ 
			return QMGR_QUEST_NOT_AVAILABLE;
		}

	if (plr->HasFinishedQuest(qst->id) && !sQuestMgr.IsQuestRepeatable(qst) && !sQuestMgr.IsQuestDaily(qst) && !sQuestMgr.IsQuestWeekly(qst))
	{ 
		return QMGR_QUEST_NOT_AVAILABLE;
	}

	for(uint32 i = 0; i < 4; ++i)
	{
		if (qst->required_quests[i] > 0)
		{
			bool finished = plr->HasFinishedQuest(qst->required_quests[i]);
			if(!finished)
				return QMGR_QUEST_NOT_AVAILABLE;
		}
	}

	// check quest level
	if( (int32)plr->getLevel() >= ( qst->max_level + 5 ) && ( status != (uint32)QMGR_QUEST_REPEATABLE ) )
	{ 
		return QMGR_QUEST_CHAT;
	}

	return status;
}

uint32 QuestMgr::CalcQuestStatus(Player* plr, Quest* qst, uint8 type, bool skiplevelcheck)
{
	QuestLogEntry* qle;

	qle = plr->GetQuestLogForEntry(qst->id);

	if (!qle)
	{
		if (type & QUESTGIVER_QUEST_START)
		{
			return PlayerMeetsReqs(plr, qst, skiplevelcheck);
		}
	}
	else
	{		
		if (!qle->CanBeFinished())
		{
			return QMGR_QUEST_NOT_FINISHED;
		}
		else
		{
			if (type & QUESTGIVER_QUEST_END) 
			{
				if (!sQuestMgr.IsQuestRepeatable(qst) && !sQuestMgr.IsQuestDaily(qst) && !sQuestMgr.IsQuestWeekly(qst))
					return QMGR_QUEST_FINISHED;					
				else
					return QMGR_QUEST_REPEATABLE_FINISHED;					
			}
			else
			{
				return QMGR_QUEST_NOT_AVAILABLE;
			}
		}
	}

	return QMGR_QUEST_NOT_AVAILABLE;
}

uint32 QuestMgr::CalcQuestStatus(Player* plr, uint32 qst)
{
	QuestLogEntry* qle;

	qle = plr->GetQuestLogForEntry(qst);

	if (qle)
	{		
		if (!qle->CanBeFinished())
		{
			return QMGR_QUEST_NOT_FINISHED;
		}
		else
		{
			return QMGR_QUEST_FINISHED;					
		}
	}

	return QMGR_QUEST_NOT_AVAILABLE;
}

uint32 QuestMgr::CalcStatus(Object* quest_giver, Player* plr)
{
	uint32 status = 0;
	std::list<QuestRelation *>::const_iterator itr;
	std::list<QuestRelation *>::const_iterator q_begin;
	std::list<QuestRelation *>::const_iterator q_end;
	bool bValid = false;

	if( quest_giver->GetTypeId() == TYPEID_GAMEOBJECT )
	{
        bValid = SafeGOCast(quest_giver)->HasQuests();
        if(bValid)
		{
			q_begin = SafeGOCast(quest_giver)->QuestsBegin();
			q_end = SafeGOCast(quest_giver)->QuestsEnd();
		}
	} 
	else if( quest_giver->GetTypeId() == TYPEID_UNIT )
	{
		bValid = SafeCreatureCast( quest_giver )->HasQuests();
		if(bValid)
		{
			q_begin = SafeCreatureCast(quest_giver)->QuestsBegin();
			q_end = SafeCreatureCast(quest_giver)->QuestsEnd();
		}
	}
#ifndef SMALL_ITEM_OBJECT
    else if( quest_giver->GetTypeId() == TYPEID_ITEM )
    {
        if( SafeItemCast( quest_giver )->GetProto()->QuestId )
            bValid = true;
    }
#endif
	//This will be handled at quest share so nothing important as status
	else if(quest_giver->GetTypeId() == TYPEID_PLAYER)
	{
		status = QMGR_QUEST_AVAILABLE;
	}

	if(!bValid)
	{
        //anoying msg that is not needed since all objects dont exactly have quests 
		//sLog.outDebug("QUESTS: Warning, invalid NPC "I64FMT" specified for CalcStatus. TypeId: %d.", quest_giver->GetGUID(), quest_giver->GetTypeId());
		return QMGR_QUEST_NOT_AVAILABLE;
	}
#ifndef SMALL_ITEM_OBJECT
    if(quest_giver->GetTypeId() == TYPEID_ITEM)
    {
        Quest *pQuest = QuestStorage.LookupEntry( SafeItemCast(quest_giver)->GetProto()->QuestId );
        QuestRelation qr;
        qr.qst = pQuest;
        qr.type = 1;

		status |= CalcQuestStatus(quest_giver,plr, &qr);
    }
#endif
	for(itr = q_begin; itr != q_end; ++itr)
	{
		status |= CalcQuestStatus(quest_giver, plr, *itr);	// save a call
	}

	//remove not avaialable flag if there are available quests
	if( status != 0 && status != QMGR_QUEST_NOT_AVAILABLE )
		status &= ~QMGR_QUEST_NOT_AVAILABLE;
	 
    return status;
}

#ifdef SMALL_ITEM_OBJECT
uint32 QuestMgr::CalcStatus(Item* quest_giver, Player* plr)
{
	uint32 status = QMGR_QUEST_NOT_AVAILABLE;
	std::list<QuestRelation *>::const_iterator itr;
	std::list<QuestRelation *>::const_iterator q_begin;
	std::list<QuestRelation *>::const_iterator q_end;

    Quest *pQuest = QuestStorage.LookupEntry( SafeItemCast(quest_giver)->GetProto()->QuestId );
    QuestRelation qr;
    qr.qst = pQuest;
    qr.type = 1;

//    uint32 tmp_status = CalcQuestStatus(quest_giver,plr, &qr);
	uint32 tmp_status = CalcQuestStatus(plr, qr.qst, qr.type, false );
    if(tmp_status > status)
        status = tmp_status;

	for(itr = q_begin; itr != q_end; ++itr)
	{
//		uint32 tmp_status = CalcQuestStatus(quest_giver, plr, *itr);	// save a call
		uint32 tmp_status = CalcQuestStatus(plr, (*itr)->qst, (*itr)->type, false );
		if (tmp_status > status)
			status = tmp_status;
	}

    return status;
}
#endif

uint32 QuestMgr::ActiveQuestsCount(Object* quest_giver, Player* plr)
{
	std::list<QuestRelation *>::const_iterator itr;
	map<uint32, uint8> tmp_map;
	uint32 questCount = 0;

	std::list<QuestRelation *>::const_iterator q_begin;
	std::list<QuestRelation *>::const_iterator q_end;
	bool bValid = false;

	if(quest_giver->GetTypeId() == TYPEID_GAMEOBJECT)
	{
        bValid = SafeGOCast(quest_giver)->HasQuests();
		if(bValid)
		{
			q_begin = SafeGOCast(quest_giver)->QuestsBegin();
			q_end   = SafeGOCast(quest_giver)->QuestsEnd();
			
		}
	} 
	else if(quest_giver->GetTypeId() == TYPEID_UNIT)
	{
		bValid = SafeCreatureCast(quest_giver)->HasQuests();
		if(bValid)
		{
			q_begin = SafeCreatureCast(quest_giver)->QuestsBegin();
			q_end   = SafeCreatureCast(quest_giver)->QuestsEnd();
		}
	}

	if(!bValid)
	{
		sLog.outDebug("QUESTS: Warning, invalid NPC "I64FMT" specified for ActiveQuestsCount. TypeId: %d.", quest_giver->GetGUID(), quest_giver->GetTypeId());
		return 0;
	}

	for(itr = q_begin; itr != q_end; ++itr)
	{
//		if (CalcQuestStatus(quest_giver, plr, *itr) >= QMGR_QUEST_CHAT)
		if (CalcQuestStatus(quest_giver, plr, *itr) > QMGR_QUEST_AVAILABLELOW_LEVEL)			
		{
			if (tmp_map.find((*itr)->qst->id) == tmp_map.end())
			{
				tmp_map.insert(std::map<uint32,uint8>::value_type((*itr)->qst->id, 1));
				questCount++;
			}
		}
	}

	return questCount;
}

void QuestMgr::BuildOfferReward(WorldPacket *data, Quest* qst, Object* qst_giver, uint32 menutype, uint32 language, Player * plr)
{
	LocalizedQuest * lq = (language>0) ? sLocalizationMgr.GetLocalizedQuest(qst->id,language):NULL;
	ItemPrototype * it;
/*
{SERVER} Packet: (0xAD54) SMSG_QUESTGIVER_OFFER_REWARD PacketSize = 436 TimeStamp = 649010
BE 07 00 00 C5 00 30 F1 
15 67 00 00 
42 6C 61 63 6B 72 6F 63 6B 20 49 6E 76 61 73 69 6F 6E 00 
54 68 65 20 42 6C 61 63 6B 72 6F 63 6B 20 6F 72 63 73 20 77 69 6C 6C 20 73 75 72 65 6C 79 20 66 61 6C 6C 20 6E 6F 77 20 74 68 61 74 20 74 68 65 69 72 20 6D 61 69 6E 20 66 6F 72 63 65 20 68 61 73 20 62 65 65 6E 20 64 65 66 65 61 74 65 64 21 20 4F 6E 6C 79 20 6F 6E 65 20 74 61 73 6B 20 72 65 6D 61 69 6E 73 2E 00 
00 00 00 00 4*strings
00 00 00 00 ?
00 00 00 00 ?
00 is there a next quest ?
00 00 08 00 flags
00 00 00 00 sugested players
01 00 00 00 emote count
00 00 00 00 emote
04 00 00 00 emote type
05 00 00 00 choice item count
CC 15 00 00 CB 15 00 00 B0 08 00 00 89 04 00 00 87 04 00 00 00 00 00 00 choice items
01 00 00 00 01 00 00 00 01 00 00 00 01 00 00 00 01 00 00 00 00 00 00 00 choice item count
41 4D 00 00 58 4C 00 00 20 19 00 00 08 06 00 00 82 13 00 00 00 00 00 00 choice item looks
00 00 00 00 reward item count
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
96 00 00 00 rew money
C2 01 00 00 rew xp
00 00 00 00 honor
00 00 00 00 unk_33_2
00 00 00 00 ?
00 00 00 00 rew spell
00 00 00 00 rew buff
00 00 00 00 rew title
48 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 rew factions 
05 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 rew rep A
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 rew rep B
00 00 00 00 
00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 rew currency ?
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 rew currency count ?
00 00 00 00 
00 00 00 00 

AA 07 00 00 C5 00 30 F1 
5A 70 00 00 
42 65 61 74 69 6E 67 20 74 68 65 6D 20 42 61 63 6B 21 00 
59 6F 75 27 76 65 20 62 6F 75 67 68 74 20 75 73 20 61 20 6C 69 74 74 6C 65 20 74 69 6D 65 2C 20 24 4E 2C 20 62 75 74 20 77 65 27 76 65 20 67 6F 74 20 65 76 65 6E 20 62 69 67 67 65 72 20 70 72 6F 62 6C 65 6D 73 20 74 6F 20 64 65 61 6C 20 77 69 74 68 20 6E 6F 77 2E 00 
54 68 69 73 20 69 73 20 61 20 42 6C 61 63 6B 72 6F 63 6B 20 42 61 74 74 6C 65 20 57 6F 72 67 2E 00 
42 6C 61 63 6B 72 6F 63 6B 20 42 61 74 74 6C 65 20 57 6F 72 67 00 
00 00 2 of the 4 texts
00 00 00 00 ?
00 00 00 00 ?
01 have next
00 00 08 00 flags
00 00 00 00 suggested players
02 00 00 00 
00 00 00 00
01 00 00 00
00 00 00 00 
01 00 00 00 emote
00 00 00 00 rew choice
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
01 00 00 00 rew
A7 DF 00 00 00 00 00 00 00 00 00 00 00 00 00 00
01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
89 20 01 00 00 00 00 00 00 00 00 00 00 00 00 00 
32 00 00 00 rew money
F4 01 00 00 rew xp
00 00 00 00
00 00 00 00 
00 00 00 00 
00 00 00 00
00 00 00 00 
00 00 00 00 
48 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 rew faction 1
05 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 a
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 b
00 00 00 00 
00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 
00 00 00 00
*/
	data->SetOpcode(SMSG_QUESTGIVER_OFFER_REWARD);
	*data << qst_giver->GetGUID();
	*data << qst->id;

	if(lq)
	{
		*data << lq->Title;
		*data << lq->CompletionText;
	}
	else
	{
		*data << qst->title;
		*data << qst->completiontext;
	}
	
	*data << uint8(0);                                        // unk string, 4.0.1
	*data << uint8(0);                                        // unk string, 4.0.1
	*data << uint8(0);                                        // unk string, 4.0.1
	*data << uint8(0);                                        // unk string, 4.0.1

	*data << uint32(0);                                      // unk uint32, 4.0.1
	*data << uint32(0);                                      // unk uint32, 4.0.1 
	//uint32 a = 0, b = 0, c = 1, d = 0, e = 1;

	*data << (qst->next_quest_id ? uint8(1) : uint8(0));	  // next quest shit
	*data << uint32(qst->quest_flags);	
	*data << uint32(qst->suggestedplayers);

	*data << uint32(1);										 // emotes count
	*data << uint32(0);										 // emote delay
	*data << uint32(1);										 // emote type

	*data << qst->count_reward_choiceitem;
//	if (qst->count_reward_choiceitem)
	{
		for(uint32 i = 0; i < 6; ++i)
//			if(qst->reward_choiceitem[i])
				*data << qst->reward_choiceitem[i];
		for(uint32 i = 0; i < 6; ++i)
//			if(qst->reward_choiceitem[i])
				*data << qst->reward_choiceitemcount[i];
		for(uint32 i = 0; i < 6; ++i)
//			if(qst->reward_choiceitem[i])
			{
				it = ItemPrototypeStorage.LookupEntry(qst->reward_choiceitem[i]);
				*data << (it ? it->DisplayInfoID : uint32(0));
			}
	}
   
	*data << qst->count_reward_item;
//	if (qst->count_reward_item)
	{
		for(uint32 i = 0; i < 4; ++i)
//			if(qst->reward_item[i])
			{
				*data << qst->reward_item[i];
			}
		for(uint32 i = 0; i < 4; ++i)
//			if(qst->reward_item[i])
			{
				*data << qst->reward_itemcount[i];
			}
		for(uint32 i = 0; i < 4; ++i)
//			if(qst->reward_item[i])
			{
				it = ItemPrototypeStorage.LookupEntry(qst->reward_item[i]);
				*data << (it ? it->DisplayInfoID : uint32(0));
			}
	}
	
	*data << GenerateRewardMoney( plr, qst );

	uint32 xp=0;
	if( plr->getLevel() < plr->GetUInt32Value(PLAYER_FIELD_MAX_LEVEL))
	{
		xp = float2int32(GenerateQuestXP(plr,qst) * sWorld.getRate(RATE_QUESTXP));
	}
	*data << uint32(xp); //VLack: The quest will give you this amount of XP
	*data << (qst->bonushonor * 10);  // bonus honor
	*data << qst->Honor2;
	*data << uint32(0);			//hmm was 8 for quest 179. Sadly no fields are 8 :O
	*data << qst->reward_spell;
	*data << qst->effect_on_player;
	*data << qst->rewardtitleid;  // reward title
//	*data << qst->rewardtalents;
//	*data << qst->unk_33_3;
//    *data << qst->unk_33_4;
//	*data << uint32(0); // unknow 4.0.1
//	*data << uint32(0); // unknow 4.0.1
//	*data << uint32(0); // unknow 4.0.1 
	for(uint32 i = 0; i < 5; ++i)
		*data << qst->rew_faction[i];		 
	for(uint32 i = 0; i < 5; ++i)
		*data << qst->rew_faction_repA[i];	
	for(uint32 i = 0; i < 5; ++i)
		*data << qst->rew_faction_repB[i];

	*data << uint32(0); // unknow 4.0.1
	*data << uint32(0); // unknow 4.0.1

	for(int i = 0; i < 4; i++)
		*data << uint32(qst->RewCurrency[i].Type);
	for(int i = 0; i < 4; i++)
		*data << uint32(qst->RewCurrency[i].Count);

	*data << uint32(0);
	*data << uint32(0);
}


void QuestMgr::TryAutoAcceptQuest(WorldSession *sess, uint64 &quest_giver_guid, uint32 quest_id)
{
	bool bValid = false;
	bool hasquest = true;
	bool bSkipLevelCheck = false;
	Quest *qst = NULL;
	Object *qst_giver = NULL;
	uint32 guidtype = GET_TYPE_FROM_GUID(quest_giver_guid);
	Player *_player = sess->GetPlayer();

	if(guidtype==HIGHGUID_TYPE_UNIT)
	{
		Creature *quest_giver = _player->GetMapMgr()->GetCreature( quest_giver_guid );
		if(quest_giver)
			qst_giver = quest_giver;
		else
			return;
		bValid = quest_giver->isQuestGiver();
		hasquest = quest_giver->HasQuest(quest_id, 1);
		if(bValid)
			qst = QuestStorage.LookupEntry(quest_id);
	} 
	else if(guidtype==HIGHGUID_TYPE_GAMEOBJECT)
	{
		GameObject *quest_giver = _player->GetMapMgr()->GetGameObject( quest_giver_guid );
		if(quest_giver)
			qst_giver = quest_giver;
		else
			return;
		//bValid = quest_giver->isQuestGiver();
		//if(bValid)
		bValid = true;
			qst = QuestStorage.LookupEntry(quest_id);
	} 
	else if(guidtype==HIGHGUID_TYPE_ITEM)
	{
		Item *quest_giver = _player->GetItemInterface()->GetItemByGUID(quest_giver_guid);
		if(quest_giver)
			qst_giver = quest_giver;
		else
			return;
		bValid = true;
		bSkipLevelCheck=true;
		qst = QuestStorage.LookupEntry(quest_id);
	}
	else if(guidtype==HIGHGUID_TYPE_PLAYER)
	{
		Player *quest_giver = _player->GetMapMgr()->GetPlayer( quest_giver_guid );
		if(quest_giver)
			qst_giver = quest_giver;
		else
			return;
		bValid = true;
		qst = QuestStorage.LookupEntry(quest_id);
	}

	if (!qst_giver)
	{
		sLog.outDebug("WORLD: Invalid questgiver GUID.");
		return;
	}

	if( !bValid || qst == NULL )
	{
		sLog.outDebug("WORLD: Creature is not a questgiver.");
		return;
	}

	if( _player->GetQuestLogForEntry( qst->id ) )
		return;

	if( qst_giver->GetTypeId() == TYPEID_UNIT && SafeCreatureCast( qst_giver )->m_escorter != NULL )
	{
		sess->SystemMessage("You cannot accept this quest at this time.");
		return;
	}

	// Check the player hasn't already taken this quest, or
	// it isn't available.
	uint32 status = sQuestMgr.CalcQuestStatus( _player,qst,3, bSkipLevelCheck);

	if((!sQuestMgr.IsQuestRepeatable(qst) && _player->HasFinishedQuest(qst->id)) || ( status != QMGR_QUEST_AVAILABLE && status != QMGR_QUEST_REPEATABLE && status != QMGR_QUEST_CHAT )
		|| !hasquest)
	{
		// We've got a hacker. Disconnect them.
		//sCheatLog.writefromsession(this, "tried to accept incompatible quest %u from %u.", qst->id, qst_giver->GetEntry());
		//Disconnect();
		return;
	}

	int32 log_slot = _player->GetOpenQuestSlot();

	if (log_slot == -1)
	{
		sQuestMgr.SendQuestLogFull(_player);
		return;
	}

	//FIXME
	/*if(Player Has Timed quest && qst->HasFlag(QUEST_FLAG_TIMED))
		sQuestMgr.SendQuestInvalid(INVALID_REASON_HAVE_TIMED_QUEST);*/

	if(qst->count_receiveitems || qst->srcitem)
	{
		uint32 slots_required = qst->count_receiveitems;

		if(_player->GetItemInterface()->CalculateFreeSlots(NULL) < slots_required)
		{
			_player->GetItemInterface()->BuildInventoryChangeError(NULL, NULL, INV_ERR_BAG_FULL);
			sQuestMgr.SendQuestFailed(FAILED_REASON_INV_FULL, qst, _player);
			return;
		}
	}	
	
/*	if(qst_giver->GetTypeId() == TYPEID_UNIT && !ScriptSystem->OnQuestRequireEvent(qst, SafeCreatureCast( qst_giver ), _player, QUEST_EVENT_CAN_ACCEPT))
		return;*/

	QuestLogEntry *qle = new QuestLogEntry();
	qle->Init(qst, _player, log_slot);
	qle->UpdatePlayerFields();

	// If the quest should give any items on begin, give them the items.
	for(uint32 i = 0; i < 4; ++i)
	{
		if(qst->receive_items[i])
		{
			Item *item = objmgr.CreateItem( qst->receive_items[i], _player);
			if(item == NULL)
				continue;
			if(!_player->GetItemInterface()->AddItemToFreeSlot(&item))
			{
				item->DeleteMe();
			}
			else
				sess->SendItemPushResult(item, false, true, false, true, 
				(uint8)_player->GetItemInterface()->LastSearchItemBagSlot(), _player->GetItemInterface()->LastSearchItemSlot(),
				1);
		}
	}

	if(qst->srcitem && qst->srcitem != qst->receive_items[0])
	{
		Item * item = objmgr.CreateItem( qst->srcitem, _player );
		if(item)
		{
			item->SetUInt32Value(ITEM_FIELD_STACK_COUNT, 1);
			if(!_player->GetItemInterface()->AddItemToFreeSlot(&item))
			{
				item->DeleteMe();
				item = NULL;
			}
		}
	}

	if(qst->count_required_item || qst_giver->GetTypeId() == TYPEID_GAMEOBJECT)	// gameobject quests deactivate
		_player->UpdateNearbyGameObjects();

	//ScriptSystem->OnQuestEvent(qst, SafeCreatureCast( qst_giver ), _player, QUEST_EVENT_ON_ACCEPT);

	sQuestMgr.OnQuestAccepted(_player,qst,qst_giver);

	sLog.outDebug("WORLD: Added new QLE.");
	sHookInterface.OnQuestAccept(_player, qst, qst_giver);
}

void QuestMgr::BuildQuestDetails(WorldPacket *data, Quest* qst, Object* qst_giver, uint32 menutype, uint32 language, Player * plr)
{
	LocalizedQuest * lq = (language>0) ? sLocalizationMgr.GetLocalizedQuest(qst->id,language):NULL;
	std::map<uint32, uint8>::const_iterator itr;
/*
//13329
{SERVER} Packet: (0xF65C) SMSG_QUESTGIVER_QUEST_DETAILS PacketSize = 962 TimeStamp = 36020578
B2 07 00 00 C5 00 30 F1 -guid
B2 07 00 00 C5 00 30 F1 -guid
5A 70 00 00 -id
42 65 61 74 69 6E 67 20 74 68 65 6D 20 42 61 63 6B 21 00 
53 6F 20 79 6F 75 27 72 65 20 74 68 65 20 6E 65 77 20 72 65 63 72 75 69 74 20 66 72 6F 6D 20 53 74 6F 72 6D 77 69 6E 64 2C 20 65 68 3F 20 49 27 6D 20 4D 61 72 73 68 61 6C 20 4D 63 42 72 69 64 65 2C 20 63 6F 6D 6D 61 6E 64 65 72 20 6F 66 20 74 68 69 73 20 67 61 72 72 69 73 6F 6E 2E 20 47 6C 61 64 20 74 6F 20 68 61 76 65 20 79 6F 75 20 6F 6E 20 62 6F 61 72 64 2E 2E 2E 24 42 24 42 3C 4D 63 42 72 69 64 65 20 6C 6F 6F 6B 73 20 74 68 72 6F 75 67 68 20 73 6F 6D 65 20 70 61 70 65 72 73 2E 3E 24 42 24 42 24 4E 2E 20 49 74 20 69 73 20 24 4E 2C 20 72 69 67 68 74 3F 24 42 24 42 59 6F 75 27 76 65 20 61 72 72 69 76 65 64 20 6A 75 73 74 20 69 6E 20 74 69 6D 65 2E 20 54 68 65 20 42 6C 61 63 6B 72 6F 63 6B 20 6F 72 63 73 20 68 61 76 65 20 6D 61 6E 61 67 65 64 20 74 6F 20 73 6E 65 61 6B 20 69 6E 74 6F 20 4E 6F 72 74 68 73 68 69 72 65 20 74 68 72 6F 75 67 68 20 61 20 62 72 65 61 6B 20 69 6E 20 74 68 65 20 6D 6F 75 6E 74 61 69 6E 2E 20 4D 79 20 73 6F 6C 64 69 65 72 73 20 61 72 65 20 64 6F 69 6E 67 20 74 68 65 20 62 65 73 74 20 74 68 61 74 20 74 68 65 79 20 63 61 6E 20 74 6F 20 70 75 73 68 20 74 68 65 6D 20 62 61 63 6B 2C 20 62 75 74 20 49 20 66 65 61 72 20 74 68 65 79 20 77 69 6C 6C 20 62 65 20 6F 76 65 72 77 68 65 6C 6D 65 64 20 73 6F 6F 6E 2E 24 42 24 42 48 65 61 64 20 6E 6F 72 74 68 77 65 73 74 20 69 6E 74 6F 20 74 68 65 20 66 6F 72 65 73 74 20 61 6E 64 20 6B 69 6C 6C 20 74 68 65 20 61 74 74 61 63 6B 69 6E 67 20 42 6C 61 63 6B 72 6F 63 6B 20 77 6F 72 67 73 21 20 48 65 6C 70 20 6D 79 20 73 6F 6C 64 69 65 72 73 21 00
4B 69 6C 6C 20 36 20 42 6C 61 63 6B 72 6F 63 6B 20 42 61 74 74 6C 65 20 57 6F 72 67 73 2E 00 - Kill 6 Blackrock Battle Worgs. 
54 68 69 73 20 69 73 20 61 20 42 6C 61 63 6B 72 6F 63 6B 20 42 61 74 74 6C 65 20 57 6F 72 67 2E 00 - This is a Blackrock Battle Worg.
42 6C 61 63 6B 72 6F 63 6B 20 42 61 74 74 6C 65 20 57 6F 72 67 00 - Blackrock Battle Worg 
00 00 00 00 
00 00 00 00 
00 00 00 00 00 
08 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01 00 00 00 A7 DF 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 89 20 01 00 00 00 00 00 00 00 00 00 00 00 00 00 32 00 00 00 F4 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 48 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 05 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 04 00 00 00 06 00 00 00 00 00 00 00 02 00 00 00 00 00 00 00 01 00 00 00 00 00 00 00 05 00 00 00 00 00 00 00 

3.3.3
28 08 00 92 02 00 30 F1 
00 00 00 00 00 00 00 00 
B3 00 00 00 id
44 77 61 72 76 65 6E 20 4F 75 74 66 69 74 74 65 72 73 00 
57 68 61 74 20 64 6F 20 77 65 20 68 61 76 65 20 68 65 72 65 3F 20 59 6F 75 20 6C 6F 6F 6B 20 61 73 20 74 68 6F 75 67 68 20 79 6F 75 20 6D 69 67 68 74 20 6E 65 65 64 20 73 6F 6D 65 74 68 69 6E 67 20 74 6F 20 6B 65 65 70 20 79 6F 75 72 20 68 61 6E 64 73 20 77 61 72 6D 2C 20 68 6D 3F 24 62 24 62 49 27 6C 6C 20 74 65 6C 6C 20 79 6F 75 20 77 68 61 74 20 77 6F 75 6C 64 20 68 65 6C 70 3A 20 61 20 70 61 69 72 20 6F 66 20 6E 69 63 65 2C 20 77 61 72 6D 20 67 6C 6F 76 65 73 2E 20 41 6E 64 2C 20 62 65 69 6E 67 20 74 68 65 20 6B 69 6E 64 20 73 6F 75 6C 20 74 68 61 74 20 49 20 61 6D 2C 20 49 27 64 20 62 65 20 6D 6F 72 65 20 74 68 61 6E 20 68 61 70 70 79 20 74 6F 20 70 72 6F 76 69 64 65 20 79 6F 75 20 77 69 74 68 20 61 20 73 75 69 74 61 62 6C 65 20 70 61 69 72 2E 20 49 27 76 65 20 6F 6E 65 20 63 6F 6E 64 69 74 69 6F 6E 2C 20 68 6F 77 65 76 65 72 2E 24 62 24 62 49 20 6E 65 65 64 20 79 6F 75 20 74 6F 20 67 6F 20 67 65 74 20 6D 65 20 73 6F 6D 65 20 77 6F 6C 66 20 6D 65 61 74 2E 20 4E 69 63 65 20 61 72 72 61 6E 67 65 6D 65 6E 74 2C 20 68 6D 3F 20 59 6F 75 20 62 72 69 6E 67 20 6D 65 20 73 6F 6D 65 20 77 6F 6C 66 20 6D 65 61 74 2C 20 61 6E 64 20 49 27 6C 6C 20 6D 61 6B 65 20 73 75 72 65 20 79 6F 75 20 64 6F 6E 27 74 20 6C 6F 73 65 20 61 6E 79 20 64 69 67 69 74 73 20 74 6F 20 66 72 6F 73 74 62 69 74 65 2E 20 57 65 6C 6C 2C 20 77 68 61 74 20 64 6F 20 79 6F 75 20 73 61 79 3F 00
53 74 65 6E 20 53 74 6F 75 74 61 72 6D 20 77 6F 75 6C 64 20 6C 69 6B 65 20 38 20 70 69 65 63 65 73 20 6F 66 20 54 6F 75 67 68 20 57 6F 6C 66 20 4D 65 61 74 2E 00
01 accept button
08 00 08 00 - flags
00 00 00 00 - suggested players
00 ?
03 00 00 00 - reward choiceitem count
CF 02 00 00 1
01 00 00 00 
4A 42 00 00
1B 18 00 00 2
01 00 00 00 
9E 24 00 00
F3 09 00 00 3
01 00 00 00
F9 1A 00 00 
00 00 00 00 reward items
00 00 00 00 rewardmoney
50 00 00 00 rewxp
00 00 00 00 honor
00 00 00 00 ?
00 00 00 00 spell
00 00 00 00 aura
00 00 00 00 title
00 00 00 00 talents
00 00 00 00 ?
03 00 00 00
2F 00 00 00 ref faction 1
36 00 00 00 ref faction 2
00 00 00 00
00 00 00 00 
00 00 00 00 5
05 00 00 00 rev rep 1
05 00 00 00 rev rep 2
00 00 00 00 
00 00 00 00
00 00 00 00 5
00 00 00 00
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 5
04 00 00 00 ?1-1
06 00 00 00 ?1-2
00 00 00 00 ?2
01 00 00 00 ?
00 00 00 00 ?3
00 00 00 00 ?
00 00 00 00	- emo
00 00 00 00
00 00 00 00 

28 08 00 92 02 00 30 F1
28 08 00 92 02 00 30 F1
23 0C 00 00 
43 6F 6E 73 65 63 72 61 74 65 64 20 52 75 6E 65 00 
57 68 69 6C 65 20 79 6F 75 20 77 65 72 65 20 68 65 6C 70 69 6E 67 20 6D 65 20 6F 75 74 2C 20 74 68 69 73 20 72 75 6E 65 20 77 61 73 20 67 69 76 65 6E 20 74 6F 20 6D 65 20 74 6F 20 70 61 73 73 20 6F 6E 20 74 6F 20 79 6F 75 2E 20 54 61 6B 65 20 73 6F 6D 65 20 74 69 6D 65 20 74 6F 20 72 65 61 64 20 69 74 20 77 68 65 6E 20 79 6F 75 20 68 61 76 65 20 61 20 63 68 61 6E 63 65 2E 20 49 27 6D 20 74 68 69 6E 6B 69 6E 27 20 69 74 20 63 61 6D 65 20 66 72 6F 6D 20 74 68 65 20 70 61 6C 61 64 69 6E 20 74 72 61 69 6E 65 72 20 42 72 6F 6D 6F 73 2E 20 54 61 6B 65 20 61 20 67 61 6E 64 65 72 20 61 74 20 69 74 20 61 6E 64 20 67 6F 20 66 69 6E 64 20 68 69 6D 20 69 6E 73 69 64 65 20 41 6E 76 69 6C 6D 61 72 20 77 68 65 6E 20 79 6F 75 27 76 65 20 61 20 63 68 61 6E 63 65 2E 00
52 65 61 64 20 74 68 65 20 43 6F 6E 73 65 63 72 61 74 65 64 20 52 75 6E 65 20 61 6E 64 20 73 70 65 61 6B 20 74 6F 20 42 72 6F 6D 6F 73 20 47 72 75 6D 6D 6E 65 72 20 69 6E 20 43 6F 6C 64 72 69 64 67 65 20 56 61 6C 6C 65 79 2E 00
00 - accept button - i think it is 
00 00 08 00 
00 00 00 00 
00 00 00 00 
00
00 00 00 00 
00 00 00 00
28 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01 00 00 00 2F 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 04 00 00 00 01 00 00 00 00 00 00 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 

52 10 00 0E 27 00 30 F1
00 00 00 00 00 00 00 00 
97 11 00 00 
53 68 69 7A 7A 6C 65 27 73 20 46 6C 79 65 72 00 
49 27 6D 20 6D 61 6B 69 6E 67 20 61 20 66 6C 79 69 6E 67 20 6D 61 63 68 69 6E 65 21 24 42 24 42 49 20 67 6F 74 20 73 74 72 61 6E 64 65 64 20 68 65 72 65 2C 20 61 6E 64 20 49 27 6D 20 61 62 73 6F 6C 75 74 65 6C 79 20 68 6F 72 72 69 62 6C 65 20 61 74 20 66 69 6E 64 69 6E 67 20 6D 79 20 77 61 79 20 61 72 6F 75 6E 64 2E 20 49 27 64 20 74 61 6B 65 20 61 20 67 72 79 70 68 6F 6E 20 6F 72 20 77 69 6E 64 20 72 69 64 65 72 20 62 61 63 6B 20 74 6F 20 47 61 64 67 65 74 7A 61 6E 2C 20 62 75 74 20 74 68 6F 73 65 20 62 65 61 73 74 73 20 6A 75 73 74 20 73 63 61 72 65 20 6D 65 20 74 6F 20 64 65 61 74 68 2E 24 42 24 42 53 6F 2C 20 77 69 6C 6C 20 79 6F 75 20 68 65 6C 70 20 6D 65 3F 20 41 6C 6C 20 49 20 68 61 76 65 20 6C 65 66 74 20 74 6F 20 62 75 69 6C 64 20 69 73 20 74 68 65 20 77 69 6E 67 73 2C 20 61 6E 64 20 49 20 77 61 73 20 74 68 69 6E 6B 69 6E 67 20 74 68 61 74 20 74 68 65 20 77 65 62 62 79 20 73 63 61 6C 65 73 20 66 72 6F 6D 20 74 68 65 20 70 74 65 72 72 6F 72 64 61 78 20 61 6E 64 20 64 69 65 6D 65 74 72 61 64 6F 6E 20 77 6F 75 6C 64 20 62 65 20 70 65 72 66 65 63 74 2E 20 54 68 65 72 65 27 73 20 6E 6F 20 77 61 79 20 49 27 6D 20 67 6F 69 6E 67 20 74 6F 20 67 6F 20 75 70 20 61 67 61 69 6E 73 74 20 6F 6E 65 20 6F 66 20 74 68 6F 73 65 2C 20 62 75 74 20 79 6F 75 20 6C 6F 6F 6B 20 70 72 65 74 74 79 20 74 6F 75 67 68 2E 20 57 68 79 20 64 6F 6E 27 74 20 79 6F 75 20 67 69 76 65 20 69 74 20 61 20 74 72 79 3F 00 
43 6F 6C 6C 65 63 74 20 38 20 57 65 62 62 65 64 20 44 69 65 6D 65 74 72 61 64 6F 6E 20 53 63 61 6C 65 73 20 61 6E 64 20 38 20 57 65 62 62 65 64 20 50 74 65 72 72 6F 72 64 61 78 20 53 63 61 6C 65 73 20 66 6F 72 20 53 68 69 7A 7A 6C 65 20 69 6E 20 4D 61 72 73 68 61 6C 27 73 20 52 65 66 75 67 65 2E 00 
01 - accept button - i think it is
08 00 00 00
00 00 00 00
00 
03 00 00 00 - reward list length
8B 2E 00 00 01 00 00 00 3E 49 00 00 8C 2E 00 00 01 00 00 00 5E 6E 00 00 8D 2E 00 00 01 00 00 00 5F 6E 00 00 00 00 00 00 00 00 00 00 8A 1B 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 04 00 00 00 04 00 00 00 00 00 00 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 

{SERVER} Packet: (0xF65C) SMSG_QUESTGIVER_QUEST_DETAILS PacketSize = 949 TimeStamp = 30377622
CA 02 00 00 76 63 30 F1 
00 00 00 00 00 00 00 00 
31 31 00 00 
49 6E 20 53 65 72 76 69 63 65 20 4F 66 20 54 68 65 20 4C 69 63 68 20 4B 69 6E 67 00 
41 6C 6C 20 74 68 61 74 20 49 20 61 6D 3A 20 61 6E 67 65 72 2C 20 63 72 75 65 6C 74 79 2C 20 76 65 6E 67 65 61 6E 63 65 20 2D 20 49 20 62 65 73 74 6F 77 20 75 70 6F 6E 20 79 6F 75 2C 20 6D 79 20 63 68 6F 73 65 6E 20 6B 6E 69 67 68 74 2E 20 49 20 68 61 76 65 20 67 72 61 6E 74 65 64 20 79 6F 75 20 69 6D 6D 6F 72 74 61 6C 69 74 79 20 73 6F 20 74 68 61 74 20 79 6F 75 20 6D 61 79 20 68 65 72 61 6C 64 20 69 6E 20 61 20 6E 65 77 2C 20 64 61 72 6B 20 61 67 65 20 66 6F 72 20 74 68 65 20 53 63 6F 75 72 67 65 2E 24 42 24 42 47 61 7A 65 20 6E 6F 77 20 75 70 6F 6E 20 74 68 65 20 6C 61 6E 64 73 20 62 65 6C 6F 77 20 75 73 2E 20 54 68 65 20 53 63 61 72 6C 65 74 20 43 72 75 73 61 64 65 20 73 63 75 72 72 69 65 73 20 74 6F 20 75 6E 64 6F 20 6D 79 20 77 6F 72 6B 2C 20 77 68 69 6C 65 20 4C 69 67 68 74 27 73 20 48 6F 70 65 20 73 74 61 6E 64 73 20 64 65 66 69 61 6E 74 6C 79 20 61 67 61 69 6E 73 74 20 75 73 20 2D 20 61 20 62 6C 65 6D 69 73 68 20 75 70 6F 6E 20 74 68 65 73 65 20 50 6C 61 67 75 65 6C 61 6E 64 73 2E 20 54 68 65 79 20 6D 75 73 74 20 61 6C 6C 20 62 65 20 73 68 6F 77 6E 20 74 68 65 20 70 72 69 63 65 20 6F 66 20 74 68 65 69 72 20 64 65 66 69 61 6E 63 65 2E 24 42 24 42 59 6F 75 20 77 69 6C 6C 20 62 65 63 6F 6D 65 20 6D 79 20 66 6F 72 63 65 20 6F 66 20 72 65 74 72 69 62 75 74 69 6F 6E 2E 20 57 68 65 72 65 20 79 6F 75 20 74 72 65 61 64 2C 20 64 6F 6F 6D 20 77 69 6C 6C 20 66 6F 6C 6C 6F 77 2E 20 47 6F 20 6E 6F 77 20 61 6E 64 20 63 6C 61 69 6D 20 79 6F 75 72 20 64 65 73 74 69 6E 79 2C 20 64 65 61 74 68 20 6B 6E 69 67 68 74 2E 00
52 65 70 6F 72 74 20 74 6F 20 49 6E 73 74 72 75 63 74 6F 72 20 52 61 7A 75 76 69 6F 75 73 20 69 6E 20 74 68 65 20 48 65 61 72 74 20 6F 66 20 41 63 68 65 72 75 73 2E 0D 0A 00
00 00 00 00 
00 00 00 00 
00 00 00 00 
01 
80 00 00 00 
00 00 
00 00 00 00 
00 00 00 00 choice item count
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 rew items
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 
00 00 00 00 
39 03 00 00 rew XP
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 04 00 00 00 8C 01 00 00 F4 01 00 00 8D 01 00 00 E8 03 00 00 8C 01 00 00 E8 03 00 00 8C 01 00 00 E8 03 00 00 

*/
	data->SetOpcode( SMSG_QUESTGIVER_QUEST_DETAILS );

	*data <<  qst_giver->GetGUID();
	*data << uint64( qst_giver->IsPlayer() ? qst_giver->GetGUID() : 0 );						// (questsharer?) guid
	*data <<  qst->id;
	if(lq)
	{
		*data << lq->Title;
		*data << lq->Details;
		*data << lq->Objectives;
	}
	else
	{
		*data <<  qst->title;
		*data <<  qst->details;
		*data <<  qst->objectives;
	}
	*data << uint32(0);										// 4.0.1, unknow -> 4 strings 
	*data << uint32(0);                                     // 4.0.1, unknow
	*data << uint32(0);										// 4.0.1, unknow 

	*data << uint8(1);						// show accept button - i think this is cancel button actually :P
	*data << uint32(qst->quest_flags);      // 3.3.3 questFlags
	*data << uint32(qst->suggestedplayers);	// "Suggested players"
	*data << uint8(0);                                       // 4.0.1, unknow 
	*data << uint8(0);						// IsFinished? value is sent back to server in quest accept packet
	*data << uint32(0);										// 4.0.1, unknow 

	*data << qst->count_reward_choiceitem;
	ItemPrototype *ip;
	uint32 i;
//	int32 count;
//	count = 0;
	for(i = 0; i < 6; ++i)
	{
//		if(!qst->reward_choiceitem[i]) 
//			continue;

//		count++;
//		if( count > qst->count_reward_choiceitem )
//			break;
		*data << qst->reward_choiceitem[i];
	}
	for(i = 0; i < 6; ++i)
		*data << qst->reward_choiceitemcount[i];
	for(i = 0; i < 6; ++i)
	{
		ip = ItemPrototypeStorage.LookupEntry(qst->reward_choiceitem[i]);
		if(ip)
			*data << ip->DisplayInfoID;
		else
			*data << uint32(0);
	}

	*data << qst->count_reward_item;

//	count = 0;
	for(i = 0; i < 4; ++i)
	{
//		if(!qst->reward_item[i]) 
//			continue;
//		count++;
//		if( count > qst->count_reward_item )
//			break;
		*data << qst->reward_item[i];
	}
	for(i = 0; i < 4; ++i)
		*data << qst->reward_itemcount[i];
	for(i = 0; i < 4; ++i)
	{
		ip = ItemPrototypeStorage.LookupEntry(qst->reward_item[i]);
		if(ip)
			*data << ip->DisplayInfoID;
		else
			*data << uint32(0);
	}

	*data << uint32(0); // unknow 4.0.1
	*data << uint32(0); // unknow 4.0.1 
	*data << GenerateRewardMoney( plr, qst );
	
	uint32 xp=0;
	if( plr->getLevel() < plr->GetUInt32Value(PLAYER_FIELD_MAX_LEVEL))
	{
		xp = float2int32(GenerateQuestXP(plr,qst) * sWorld.getRate(RATE_QUESTXP));
	}
	*data << uint32(xp);			//3.3 not sure this is here. test me as soon as there is a quest with data

	*data << qst->bonushonor;    // bonus honor
	*data << qst->Honor2;	
	*data << qst->reward_spell;
	*data << qst->effect_on_player;
	*data << qst->rewardtitleid;						// reward pvp title id
	*data << qst->rewardtalents;						// 3.0.2 reward talents

//    *data << qst->unk_33_3;								//3.3 not sure this is here
//    *data << qst->unk_33_4;								//3.3 not sure this is here
		
//	*data << uint32(0); // unknow 4.0.1
//	*data << uint32(0); // unknow 4.0.1
//	*data << uint32(0); // unknow 4.0.1 

	for(uint32 i = 0; i < 5; ++i)
		*data << qst->rew_faction[i];		 
	for(uint32 i = 0; i < 5; ++i)
		*data << qst->rew_faction_repA[i];	
	for(uint32 i = 0; i < 5; ++i)
		*data << qst->rew_faction_repB[i];	

	for(uint32 i = 0; i < 4; ++i)
		*data << uint32(qst->RewCurrency[i].Type);
	for(uint32 i = 0; i < 4; ++i)
		*data << uint32(qst->RewCurrency[i].Count);

//	*data << uint32(0); // unknow 4.0.1
//	*data << uint32(0); // unknow 4.0.1
	*data << uint32(1);						// emote count
	*data << uint32( EMOTE_ONESHOT_TALK );	// emote1 type
	*data << uint32(0);						// emote1 delay
}


void QuestMgr::BuildRequestItems(WorldPacket *data, Quest* qst, Object* qst_giver, uint32 status, uint32 language)
{
	LocalizedQuest * lq = (language>0) ? sLocalizationMgr.GetLocalizedQuest(qst->id,language):NULL;
	ItemPrototype * it;
/*
13329
{SERVER} Packet: (0xC45C) SMSG_QUESTGIVER_REQUEST_ITEMS PacketSize = 137 TimeStamp = 631226
BE 07 00 00 C5 00 30 F1 
15 67 00 00 = 26389 
42 6C 61 63 6B 72 6F 63 6B 20 49 6E 76 61 73 69 6F 6E 00 
48 61 76 65 20 79 6F 75 20 6B 69 6C 6C 65 64 20 74 68 65 20 72 61 6D 70 61 67 69 6E 67 20 6F 72 63 20 68 6F 72 64 65 73 3F 00 
00 00 00 00 
06 00 00 00 rewxplevel
01 00 00 00 ?
00 00 08 00 flags
00 00 00 00 
00 00 00 00 
01 00 00 00 required items
F9 E3 00 00 itemid
08 00 00 00 count
1C 10 01 00 looks
00 00 00 00 ? after items block
02 00 00 00 
04 00 00 00 
08 00 00 00 
10 00 00 00 
40 00 00 00 

{SERVER} Packet: (0xC45C) SMSG_QUESTGIVER_REQUEST_ITEMS PacketSize = 149 TimeStamp = 12372921
7C 06 00 00 68 8E 30 F1 44 38 00 00 4E 6F 74 20 51 75 69 74 65 20 53 68 69 70 73 68 61 70 65 00 44 6F 20 79 6F 75 20 68 61 76 65 20 6D 79 20 73 75 70 70 6C 69 65 73 2C 20 24 4E 3F 00 00 00 00 00 00 00 00 00 01 00 00 00 08 00 80 00 00 00 00 00 00 00 00 00 
03 00 00 00 
B9 C0 00 00 
01 00 00 00 
CA 08 01 00 
BA C0 00 00 
01 00 00 00 
B0 73 00 00 
BB C0 00 00 
01 00 00 00 
3D D1 00 00 
00 00 00 00 
02 00 00 00 04 00 00 00 08 00 00 00 10 00 00 00 40 00 00 00
*/
	data->SetOpcode( SMSG_QUESTGIVER_REQUEST_ITEMS );

	*data << qst_giver->GetGUID();
	*data << qst->id;

	if(lq)
	{
		*data << lq->Title;
		*data << ((lq->IncompleteText[0]) ? lq->IncompleteText : lq->Details);
	}
	else
	{
		*data << qst->title;
		*data << (qst->incompletetext[0] ? qst->incompletetext : qst->details);
	}
	
	*data << uint32(0);				 // ?
	*data << uint32( qst->rew_xp_rank );
	*data << uint32(1);				 // close on cancel ?
	*data << uint32(qst->quest_flags);                     // 3.3.3 questFlags
	*data << uint32(qst->suggestedplayers);          // SuggestedGroupNum
	*data << uint32( qst->rewardtalents );	   // Required Money

	// item count
	*data << qst->count_required_item;
	
	// (loop for each item)
	if( qst->count_required_item )
	{
		for(uint32 i = 0; i < 6; ++i)
		{
			if(qst->required_item[i] != 0)
			{
				*data << qst->required_item[i];
				*data << qst->required_itemcount[i];
				it = ItemPrototypeStorage.LookupEntry(qst->required_item[i]);
				*data << (it ? it->DisplayInfoID : uint32(0));
			}
		}
	}

	*data << uint32( 0 ); //?

	// wtf is this?
    if(status == QMGR_QUEST_NOT_FINISHED)
    {
	    *data << uint32(0); //incomplete button
    }
    else
    {
        *data << uint32(2);
    }

    *data << uint32(0x04);
    *data << uint32(0x08);
    *data << uint32(0x10);
    *data << uint32(0x40);
}

void QuestMgr::BuildQuestComplete(Player*plr, Quest* qst)
{
	uint32 xp ;
	if(plr->getLevel() >= plr->GetUInt32Value(PLAYER_FIELD_MAX_LEVEL))
	{
		//plr->ModUnsigned32Value(PLAYER_FIELD_COINAGE, qst->reward_xp_as_money);
		xp = 0;
	}else
	{
		xp = float2int32(GenerateQuestXP(plr,qst) * sWorld.getRate(RATE_QUESTXP));
		plr->GiveXP(xp, 0, false);
	}

	if( qst->rewardtalents > 0 )
	{
		//removed by zack to avoid exploits
		if( sWorld.getIntRate( INTRATE_ALLOW_QUEST_TALENTPOINTS ) )
		{
			plr->m_Talent_point_mods += qst->rewardtalents;
			plr->ModUnsigned32Value( PLAYER_CHARACTER_POINTS, qst->rewardtalents );
			plr->smsg_TalentsInfo();
		}
	}
	  
	// Reward title -> this is DBC row entry !
	if( qst->rewardtitleid > 0 )
	{
		CharTitlesEntry *ce = dbcCharTitle.LookupEntryForced( qst->rewardtitleid );
		if( ce )
			plr->SetKnownTitle( ( RankTitles )( ce->bit_index ), true );
	}
/*
13329
{SERVER} Packet: (0x08B0) UNKNOWN PacketSize = 25 TimeStamp = 19968580
80 00 00 00 
00 
3E 00 00 00 quest
00 00 00 00 
71 02 00 00 XP
00 00 00 00 
AF 00 00 00 gold
*/
	WorldPacket data( SMSG_QUESTGIVER_QUEST_COMPLETE,72 );

	data << uint8(0x80); // unk 4.0.1 flags
	data << uint8(0);   // ??
	data <<  qst->id;
	data << uint32(0);   // ??
//	data <<  uint32(3);
	//if(qst->reward_xp > 0)
	  //  data <<  uint32(qst->reward_xp);
	//else
	   // data <<  uint32(GenerateQuestXP(NULL,qst)); //xp
	data << xp;
	data << uint32(0);   // ??
	data <<  uint32( GenerateRewardMoney( plr, qst ) );
/*	data << uint32(0);   // bonus honor
	data << uint32(qst->rewardtalents);
	data <<  uint32(qst->count_reward_item); //Reward item count

	for(uint32 i = 0; i < 4; ++i)
	{
		if(qst->reward_item[i])
		{
			data << qst->reward_item[i];
			data << qst->reward_itemcount[i];
		}
	}*/
	plr->GetSession()->SendPacket(&data);
}

void QuestMgr::BuildQuestList(WorldPacket *data, Object* qst_giver, Player *plr, uint32 language)
{
	uint32 status;
	list<QuestRelation *>::iterator it;
	list<QuestRelation *>::iterator st;
	list<QuestRelation *>::iterator ed;
	map<uint32, uint8> tmp_map;
/*
13329
{SERVER} Packet: (0xD57E) SMSG_QUESTGIVER_QUEST_LIST PacketSize = 60 TimeStamp = 36018484
B2 07 00 00 C5 00 30 F1 guid
D0 0F 00 00 - menu id ?
4A 13 00 00 - text id ?
00 00 00 00 size
01 00 00 00 menu
5A 70 00 00 questid
02 00 00 00 status1
03 00 00 00 status2
00 
00 08 00 00 
42 65 61 74 69 6E 67 20 74 68 65 6D 20 42 61 63 6B 21 00 Beating them Back!

3.3.3
28 08 00 92 02 00 30 F1
41 68 2C 20 77 65 6C 6C 20 61 72 65 6E 27 74 20 79 6F 75 20 61 20 73 74 75 72 64 79 2D 6C 6F 6F 6B 69 6E 67 20 6F 6E 65 3F 20 50 65 72 68 61 70 73 20 79 6F 75 20 63 61 6E 20 61 73 73 69 73 74 20 6D 65 20 77 69 74 68 20 61 20 74 68 69 6E 67 20 6F 72 20 74 77 6F 2E 20 4E 6F 74 20 6D 75 63 68 20 68 65 6C 70 20 61 72 6F 75 6E 64 20 68 65 72 65 20 65 78 63 65 70 74 20 66 6F 72 20 67 72 65 65 6E 20 61 70 70 72 65 6E 74 69 63 65 73 2C 20 61 6E 64 20 74 68 65 79 27 76 65 20 6F 74 68 65 72 20 74 68 69 6E 67 73 20 74 6F 20 77 6F 72 72 79 20 61 62 6F 75 74 2E 00
00 00 00 00
01 00 00 00
02 
23 0C 00 00 - id
02 00 00 00 - status 1
01 00 00 00 - status 2
00 00 08 00 - flags 
00
43 6F 6E 73 65 63 72 61 74 65 64 20 52 75 6E 65 00 
E9 00 00 00 id
02 00 00 00 s1
03 00 00 00 s2
00 
00 08 00 00 
43 6F 6C 64 72 69 64 67 65 20 56 61 6C 6C 65 79 20 4D 61 69 6C 20 44 65 6C 69 76 65 72 79 00 
14333
{SERVER} Packet: (0x62E6) SMSG_QUESTGIVER_QUEST_LIST PacketSize = 24 TimeStamp = 9552846
C3 1B 00 00 46 81 30 F1 
36 28 00 00 
DA 37 00 00 
00 00 00 00 gossip menu counter
00 00 00 00 quest counter
{SERVER} Packet: (0x62E6) SMSG_QUESTGIVER_QUEST_LIST PacketSize = 50 TimeStamp = 7248960
3D 4A 00 00 FE 0E 30 F1 
CE 10 00 00 ?
62 15 00 00 npctext
01 00 00 00 gossip menu counter
00 00 00 00 quest counter
02 00 00 00 
00 00 
49 20 6E 65 65 64 20 61 20 72 69 64 65 2E 00 
00 00 00 00 00
14480
95 02 00 00 1D 25 13 F1 - guid
5C 2F 00 00 ?
86 42 00 00 - NPC text ID
00 00 00 00 gossip menu counter
02 00 00 00 quest counter
E5 71 00 00 id
02 00 00 00 status
55 00 00 00 maxlevel
80 00 84 00 flags
00 repeatable ?
54 68 65 20 5A 61 6E 64 61 6C 61 72 69 20 4D 65 6E 61 63 65 00 title
CC 72 00 00 entry
02 00 00 00 status
55 00 00 00 maxlevel
08 00 80 00 flags
00 repeatable
47 75 61 72 64 69 61 6E 73 20 6F 66 20 48 79 6A 61 6C 3A 20 46 69 72 65 6C 61 6E 64 73 20 49 6E 76 61 73 69 6F 6E 21 00 title
*/
	data->Initialize( SMSG_QUESTGIVER_QUEST_LIST );

	*data << qst_giver->GetGUID();
	*data << uint32( 0x00002F5C );	// no idea :(. Does change from time to time
	*data << uint32( 17030 );	//npc text 
	*data << uint32(0);//gossip menu counter

	bool bValid = false;
	if(qst_giver->GetTypeId() == TYPEID_GAMEOBJECT)
	{
		bValid = SafeGOCast(qst_giver)->HasQuests();
		if(bValid)
		{
			st = SafeGOCast(qst_giver)->QuestsBegin();
			ed = SafeGOCast(qst_giver)->QuestsEnd();
		}
	} 
	else if(qst_giver->GetTypeId() == TYPEID_UNIT)
	{
		bValid = SafeCreatureCast(qst_giver)->HasQuests();
		if(bValid)
		{
			st = SafeCreatureCast(qst_giver)->QuestsBegin();
			ed = SafeCreatureCast(qst_giver)->QuestsEnd();
		}
	}

	if(!bValid)
	{
		*data << uint32(0);	//quest counter
		return;
	}
	
	*data << uint32(sQuestMgr.ActiveQuestsCount(qst_giver, plr));

	for (it = st; it != ed; ++it)
	{
		status = sQuestMgr.CalcQuestStatus(qst_giver, plr, *it);
		if (status > QMGR_QUEST_AVAILABLELOW_LEVEL)
		{
			if (tmp_map.find((*it)->qst->id) == tmp_map.end())
			{
				tmp_map.insert(std::map<uint32,uint8>::value_type((*it)->qst->id, 1));
				LocalizedQuest * lq = (language>0) ? sLocalizationMgr.GetLocalizedQuest((*it)->qst->id,language):NULL;

				*data << (*it)->qst->id;
				/**data << sQuestMgr.CalcQuestStatus(qst_giver, plr, *it);
				*data << uint32(0);*/
				
				switch(status)
				{
				case QMGR_QUEST_NOT_FINISHED:
					*data << uint32(4);
					break;

				case QMGR_QUEST_FINISHED:
					*data << uint32(4);
					break;

				case QMGR_QUEST_CHAT:
					*data << uint32( QMGR_QUEST_AVAILABLE );
					break;

				default:
					*data << status;
				}
				
				*data << (*it)->qst->max_level;
				*data << uint32( (*it)->qst->quest_flags );
				*data << uint8( (*it)->qst->is_repeatable );                                   // 3.3.3 changes icon: blue question or yellow exclamation

				if(lq)
					*data << lq->Title;
				else
					*data << (*it)->qst->title;
			}
		}
	}
}

/*
void QuestMgr::BuildQuestUpdateAddItem(WorldPacket* data, uint32 itemid, uint32 count)
{
	data->Initialize(SMSG_QUESTUPDATE_ADD_ITEM);
	*data << itemid << count;
}*/

void QuestMgr::SendQuestUpdateAddKill(Player* plr, uint32 questid, uint32 entry, uint32 count, uint32 tcount, uint64 guid)
{
	/*
	13329
	{SERVER} Packet: (0x157F) SMSG_QUESTUPDATE_ADD_KILL PacketSize = 24 TimeStamp = 16685476
	5B 70 00 00 
	CF C2 00 00 
	01 00 00 00 
	06 00 00 00 
	7A F5 0D 00 CF C2 30 F1 
*/
	WorldPacket data(32);
	data.SetOpcode(SMSG_QUESTUPDATE_ADD_KILL);
	data << questid << entry << count << tcount << guid;
	plr->GetSession()->SendPacket(&data);
}

/*
void QuestMgr::BuildQuestUpdateComplete(WorldPacket* data, Quest* qst)
{
	data->Initialize(SMSG_QUESTUPDATE_COMPLETE);

	*data << qst->id;
}*/

void QuestMgr::SendPushToPartyResponse(Player *plr, Player* pTarget, uint32 response)
{
	WorldPacket data(MSG_QUEST_PUSH_RESULT, 13);
	data << pTarget->GetGUID();
	data << response;
	data << uint8(0);
	plr->GetSession()->SendPacket(&data);
}

bool QuestMgr::OnGameObjectActivate(Player *plr, GameObject *go)
{
	uint32 i, j;
	QuestLogEntry *qle;
	uint32 entry = go->GetEntry();

	for(i = 0; i < 25; ++i)
	{
		qle = plr->GetQuestLogInSlot( i );
		if( qle != NULL )
		{
			// dont waste time on quests without mobs
			if( qle->GetQuest()->count_required_mob == 0 )
				continue;

			for( j = 0; j < 4; ++j )
			{
				if( qle->GetQuest()->required_mob[j] == entry &&
					qle->GetQuest()->required_mobtype[j] == QUEST_MOB_TYPE_GAMEOBJECT &&
					(int32)qle->m_mobcount[j] < qle->GetQuest()->required_mobcount[j] )
				{
					// add another kill.
					// (auto-dirtys it)
					qle->SetMobCount( j, qle->m_mobcount[j] + 1 );
					qle->SendUpdateAddKill( j );
					CALL_QUESTSCRIPT_EVENT( qle, OnGameObjectActivate )( entry, plr, qle );

					if( qle->CanBeFinished() )
						qle->SendQuestComplete();

					qle->UpdatePlayerFields();
					return true;
				}
			}
		}
	}
	return false;
}


void QuestMgr::OnPlayerKill(Player* plr, Creature* victim, bool IsGroupKill)
{
	uint32 entry = victim->GetEntry();
	OnPlayerKill( plr, entry, IsGroupKill );

	// Extra credit (yay we wont have to script this anymore) - Shauren
	for( uint8 i = 0; i < 2; ++i )
	{
		uint32 extracredit = victim->GetCreatureInfo()->killcredit[i];
		if( extracredit != 0 )
		{
			if( CreatureNameStorage.LookupEntry(extracredit) )
				OnPlayerKill( plr, extracredit, IsGroupKill );
		}
	}
}

void QuestMgr::OnPlayerKill(Player* plr, uint32 entry, bool IsGroupKill)
{
	if(!plr)
	{ 
		return;
	}
	QuestLogEntry *qle;
	uint32 i, j;

	if (plr->HasQuestMob(entry))
	{
		for(i = 0; i < 25; ++i)
		{
			qle = plr->GetQuestLogInSlot( i );
			if( qle != NULL )
			{
				// dont waste time on quests without mobs
				if( qle->GetQuest()->count_required_mob == 0 )
					continue;

				for( j = 0; j < 4; ++j )
				{
					if( qle->GetQuest()->required_mob[j] == entry &&
						qle->GetQuest()->required_mobtype[j] == QUEST_MOB_TYPE_CREATURE &&
						(int32)qle->m_mobcount[j] < qle->GetQuest()->required_mobcount[j] )
					{
						// add another kill.(auto-dirtys it)
						qle->SetMobCount( j, qle->m_mobcount[j] + 1 );
						qle->SendUpdateAddKill( j );
						CALL_QUESTSCRIPT_EVENT( qle, OnCreatureKill)( entry, plr, qle );
						qle->UpdatePlayerFields();
						break;
					}
				}
			}
		}
	}

	// Shared kills
	Player *gplr = NULL;

	if( IsGroupKill && plr->InGroup() )
	{
		if(Group* pGroup = plr->GetGroup())
		{
//			removed by Zack How the hell will healers get the kills then ?
//			if(pGroup->GetGroupType() != GROUP_TYPE_PARTY) 
//				return;  // Raid's don't get shared kills.

			GroupMembersSet::iterator gitr;
			pGroup->Lock();
			for(uint32 k = 0; k < pGroup->GetSubGroupCount(); k++)
			{
				for(gitr = pGroup->GetSubGroup(k)->GetGroupMembersBegin(); gitr != pGroup->GetSubGroup(k)->GetGroupMembersEnd(); ++gitr)
				{
					gplr = (*gitr)->m_loggedInPlayer;
					if(gplr && gplr != plr && plr->isInRange(gplr,300) && gplr->HasQuestMob(entry)) // dont double kills also dont give kills to party members at another side of the world
					{
						for( i = 0; i < 25; ++i )
						{
							qle = gplr->GetQuestLogInSlot(i);
							if( qle != NULL )
							{
								// dont waste time on quests without mobs
								if( qle->GetQuest()->count_required_mob == 0 )
									continue;

								for( j = 0; j < 4; ++j )
								{
									if( qle->GetQuest()->required_mob[j] == entry &&
										qle->GetQuest()->required_mobtype[j] == QUEST_MOB_TYPE_CREATURE &&
										(int32)qle->m_mobcount[j] < qle->GetQuest()->required_mobcount[j] )
									{
										// add another kill.
										// (auto-dirtys it)
										qle->SetMobCount(j, qle->m_mobcount[j] + 1);
										qle->SendUpdateAddKill( j );
										CALL_QUESTSCRIPT_EVENT( qle, OnCreatureKill )( entry, plr, qle );
										qle->UpdatePlayerFields();

										if( qle->CanBeFinished() )
											qle->SendQuestComplete();
										break;
									}
								}
							}
						}
					}
				}
			}
			pGroup->Unlock();
		}
	}
}

void QuestMgr::OnPlayerCast(Player* plr, uint32 spellid, uint64& victimguid)
{
	if(!plr || !plr->HasQuestSpell(spellid))
	{ 
		return;
	}

	Unit * victim = plr->GetMapMgr() ? plr->GetMapMgr()->GetUnit(victimguid) : NULL;
	if(victim==NULL)
	{ 
		return;
	}

	uint32 i, j;
	uint32 entry = victim->GetEntry();
	QuestLogEntry *qle;
	for(i = 0; i < 25; ++i)
	{
		if((qle = plr->GetQuestLogInSlot(i)) != 0)
		{
			// dont waste time on quests without casts
			if(!qle->IsCastQuest())
				continue;

			for(j = 0; j < 4; ++j)
			{
				if(qle->GetQuest()->required_mob[j] == entry &&
					qle->GetQuest()->required_spell[j] == spellid &&
					(int32)qle->m_mobcount[j] < qle->GetQuest()->required_mobcount[j] &&
					!qle->IsUnitAffected(victim))
				{
					// add another kill.(auto-dirtys it)
					qle->AddAffectedUnit(victim);
					qle->SetMobCount(j, qle->m_mobcount[j] + 1);
					qle->SendUpdateAddKill(j);
					qle->UpdatePlayerFields();
					if( qle->CanBeFinished() )
						qle->SendQuestComplete();
					break;
				}
			}
		}
	}
}



void QuestMgr::OnPlayerItemPickup(Player* plr, Item* item)
{
	uint32 i, j;
	uint32 pcount;
	uint32 entry = item->GetEntry();
	QuestLogEntry *qle;
	for( i = 0; i < 25; ++i )
	{
		if( ( qle = plr->GetQuestLogInSlot( i ) )  != 0)
		{
			if( qle->GetQuest()->count_required_item == 0 )
				continue;

			for( j = 0; j < 6; ++j )
			{
				if( qle->GetQuest()->required_item[j] == entry )
				{
					pcount = plr->GetItemInterface()->GetItemCount(entry, true);
					CALL_QUESTSCRIPT_EVENT(qle, OnPlayerItemPickup)(entry, pcount, plr, qle);
					if((int32)pcount < qle->GetQuest()->required_itemcount[j])
					{
						if(qle->CanBeFinished())
						{
							plr->UpdateNearbyGameObjects();
							qle->SendQuestComplete();
						}
						break;
					}
				}
			}
		}
	}
}

void QuestMgr::OnPlayerExploreArea(Player* plr, uint32 AreaID)
{
	uint32 i, j;
	QuestLogEntry *qle;
	for( i = 0; i < 25; ++i )
	{
		if((qle = plr->GetQuestLogInSlot(i)) != 0)
		{
			// dont waste time on quests without triggers
			if( qle->GetQuest()->count_requiredtriggers == 0 )
				continue;

			for( j = 0; j < 4; ++j )
			{
				if(qle->GetQuest()->required_triggers[j] == AreaID &&
					!qle->m_explored_areas[j])
				{
					qle->SetTrigger(j);
					CALL_QUESTSCRIPT_EVENT(qle, OnExploreArea)(qle->m_explored_areas[j], plr, qle);
					qle->UpdatePlayerFields();
					if(qle->CanBeFinished())
					{
						plr->UpdateNearbyGameObjects();
						qle->SendQuestComplete();
					}
					break;
				}
			}
		}
	}
}

void QuestMgr::AreaExplored(Player* plr, uint32 QuestID)
{
	uint32 i, j;
	QuestLogEntry *qle;
	for( i = 0; i < 25; ++i )
	{
		if((qle = plr->GetQuestLogInSlot(i)) != 0)
		{
			// search for quest
			if( qle->GetQuest()->id == QuestID )
			for( j = 0; j < 4; ++j )
			{
				if(qle->GetQuest()->required_triggers[j] &&
					!qle->m_explored_areas[j])
				{
					qle->SetTrigger(j);
					CALL_QUESTSCRIPT_EVENT(qle, OnExploreArea)(qle->m_explored_areas[j], plr, qle);
					qle->UpdatePlayerFields();
					if(qle->CanBeFinished())
					{
						plr->UpdateNearbyGameObjects();
						qle->SendQuestComplete();
					}
					break;
				}
			}
		}
	}
}

void QuestMgr::GiveQuestRewardReputation(Player* plr, Quest* qst, Object *qst_giver)
{
	// Reputation reward
	for(int z = 0; z < 5; z++)
	{
		if(qst->rew_faction[z])
		{
			int32 amt;
			int fact = qst->rew_faction[z];
			if(qst->rew_faction_repB[z] == 0)
			{
				int amttype = qst->rew_faction_repA[z];
				QuestRewRepEntry* questRepReward = dbcQuestRewRep.LookupEntryForced(1);
				if(amttype < 0)
				{
					amttype *= -1;
					questRepReward = dbcQuestRewRep.LookupEntryForced(2);
				}
				if( questRepReward == NULL )
					return;
				switch(amttype)
				{
					case 1:
						amt = questRepReward->rewRep1;
						break;
					case 2:
						amt = questRepReward->rewRep2;
						break;
					case 3:
						amt = questRepReward->rewRep3;
						break;
					case 4:
						amt = questRepReward->rewRep4;
						break;
					case 5:
						amt = questRepReward->rewRep5;
						break;
					case 6:
						amt = questRepReward->rewRep6;
						break;
					case 7:
						amt = questRepReward->rewRep7;
						break;
					case 8:
						amt = questRepReward->rewRep8;
						break;
					case 9:
						amt = questRepReward->rewRep9;
						break;
				}
			}
			else
			{
				amt = float2int32( (float)qst->rew_faction_repB[z]/100);
			}
	  
			amt = float2int32( float( amt ) * sWorld.getRate( RATE_QUESTREPUTATION ) ); // reputation rewards 
			plr->ModStanding(fact, amt);
			//reward guild reputation for daily quests
			if( qst->is_repeatable == arcemu_QUEST_REPEATABLE_DAILY && plr->m_playerInfo && plr->m_playerInfo->guild )
				plr->ModStanding( PLAYER_GUILD_FACTION_ID, MAX( 1, amt / 10 ) );
		}
	}
}

void QuestMgr::OnQuestAccepted(Player* plr, Quest* qst, Object *qst_giver)
{
	
}

void QuestMgr::OnQuestFinished(Player* plr, Quest* qst, Object *qst_giver, uint32 reward_slot)
{
	//Re-Check for Gold Requirement (needed for possible xploit) - reward money < 0 means required money
	if( qst->reward_money < 0 )
	{ 
		if( plr->GetGold( ) < -qst->reward_money )
			return;
		plr->ModGold( qst->reward_money );	//take gold away
	}

	QuestLogEntry *qle = NULL;
	qle = plr->GetQuestLogForEntry(qst->id);
	if(!qle)
	{ 
		return;
	}
    
    BuildQuestComplete(plr, qst);
    CALL_QUESTSCRIPT_EVENT(qle, OnQuestComplete)(plr, qle);
	for (uint32 x=0;x<4;x++)
	{
		if (qst->required_spell[x]!=0)
		{
			if (plr->HasQuestSpell(qst->required_spell[x]))
				plr->RemoveQuestSpell(qst->required_spell[x]);
		}
		else if (qst->required_mob[x]!=0)
		{
			if (plr->HasQuestMob(qst->required_mob[x]))
				plr->RemoveQuestMob(qst->required_mob[x]);
		} 
	}
	qle->ClearAffectedUnits();
	qle->Finish();
	
	
	if(qst_giver->GetTypeId() == TYPEID_UNIT)
	{
		if(!SafeCreatureCast(qst_giver)->HasQuest(qst->id, 2))
		{
			//sCheatLog.writefromsession(plr->GetSession(), "tried to finish quest from invalid npc.");
			plr->SoftDisconnect();
			return;
		}
	}

	uint32 i;
	uint32 rew_money = GenerateRewardMoney( plr, qst );
	plr->ModGold( rew_money );
	plr->Event_AchiementCriteria_Received( ACHIEVEMENT_CRITERIA_TYPE_MONEY_FROM_QUEST_REWARD,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,rew_money,ACHIEVEMENT_EVENT_ACTION_ADD);
  
	// Reputation reward
	GiveQuestRewardReputation(plr, qst, qst_giver);
	// Static Item reward
	for(i = 0; i < 4; ++i)
	{
		if(qst->reward_item[i])
		{
			ItemPrototype *proto = ItemPrototypeStorage.LookupEntry(qst->reward_item[i]);
			if(!proto)
			{
				sLog.outError("Invalid item prototype in quest reward! ID %d, quest %d", qst->reward_item[i], qst->id);
			}
			else
			{   
				Item *add;
				SlotResult slotresult;
				add = plr->GetItemInterface()->FindItemLessMax(qst->reward_item[i], qst->reward_itemcount[i], false);
				if (!add)
				{
					slotresult = plr->GetItemInterface()->FindFreeInventorySlot(proto);
					if(!slotresult.Result)
					{
						plr->GetItemInterface()->BuildInventoryChangeError(NULL, NULL, INV_ERR_INVENTORY_FULL);
					}
					else
					{
						Item *itm = objmgr.CreateItem(qst->reward_item[i], plr);
						itm->SetUInt32Value(ITEM_FIELD_STACK_COUNT, uint32(qst->reward_itemcount[i]));
						if( plr->GetItemInterface()->SafeAddItem(itm,slotresult.ContainerSlot, slotresult.Slot) == ADD_ITEM_RESULT_ERROR )
						{
							itm->DeleteMe();
							itm = NULL;
						}
					}
				}
				else
				{
					add->SetCount(add->GetUInt32Value(ITEM_FIELD_STACK_COUNT) + qst->reward_itemcount[i]);
					add->m_isDirty = true;
				}
			}
		}
	}

	// Choice Rewards
	if(qst->reward_choiceitem[reward_slot])
	{
		ItemPrototype *proto = ItemPrototypeStorage.LookupEntry(qst->reward_choiceitem[reward_slot]);
		if(!proto)
		{
			sLog.outError("Invalid item prototype in quest reward! ID %d, quest %d", qst->reward_choiceitem[reward_slot], qst->id);
		}
		else
		{
			Item *add;
			SlotResult slotresult;
			add = plr->GetItemInterface()->FindItemLessMax(qst->reward_choiceitem[reward_slot], qst->reward_choiceitemcount[reward_slot], false);
			if (!add)
			{
				slotresult = plr->GetItemInterface()->FindFreeInventorySlot(proto);
				if(!slotresult.Result)
				{
					plr->GetItemInterface()->BuildInventoryChangeError(NULL, NULL, INV_ERR_INVENTORY_FULL);
				}
				else 
				{
					Item *itm = objmgr.CreateItem(qst->reward_choiceitem[reward_slot], plr);
					itm->SetUInt32Value(ITEM_FIELD_STACK_COUNT, uint32(qst->reward_choiceitemcount[reward_slot]));
					if( plr->GetItemInterface()->SafeAddItem(itm,slotresult.ContainerSlot, slotresult.Slot) == ADD_ITEM_RESULT_ERROR )
					{
						itm->DeleteMe();
						itm = NULL;
					}
				}
			}
			else
			{
				add->SetCount(add->GetUInt32Value(ITEM_FIELD_STACK_COUNT) + qst->reward_choiceitemcount[reward_slot]);
				add->m_isDirty = true;
			}
		}
	}

	//rew currency
	for(i = 0; i < 4; ++i)
		if( qst->RewCurrency[i].Type > 0 )
			plr->ModCurrencyCount( qst->RewCurrency[i].Type, qst->RewCurrency[i].Count );

	// Remove items
	for(uint32 i = 0; i < 6; ++i)
	{
		if(qst->required_item[i]) plr->GetItemInterface()->RemoveItemAmt(qst->required_item[i],qst->required_itemcount[i]);
	}

	// Remove srcitem
	if(qst->srcitem && qst->srcitem != qst->receive_items[0])
		plr->GetItemInterface()->RemoveItemAmt(qst->srcitem, 1);

	//remove currency
	for(i = 0; i < 4; ++i)
		if( qst->ReqCurrency[i].Type > 0 )
			plr->ModCurrencyCount( qst->ReqCurrency[i].Type, -((int32)(qst->ReqCurrency[i].Count)) );

	// cast learning spell
	if(qst->reward_spell)
	{
		if(!plr->HasSpell(qst->reward_spell))
		{
/*				// "Teaching" effect
			WorldPacket data(SMSG_SPELL_START, 42);
			data << qst_giver->GetNewGUID() << qst_giver->GetNewGUID();
			data << uint8(0);
			data << uint32(7763);
			data << uint32(0);
			data << uint32(0);
			data << uint16(2) << uint16(0);
			data << plr->GetNewGUID();
			plr->GetSession()->SendPacket( &data );

			data.Initialize( SMSG_SPELL_GO );
			data << qst_giver->GetNewGUID() << qst_giver->GetNewGUID();
			data << uint8(0); // cast number
			data << uint32(7763);		   // spellID
			data << uint8(0) << uint8(1)<<uint16(0);   // flags
			data << getMSTime();
			data << uint8(1);			   // amount of targets
			data << plr->GetGUID();		 // target
			data << uint8(0); //target mask
			data << uint16(2) << uint16( 0 ); //target flags
			data << plr->GetNewGUID();
			plr->GetSession()->SendPacket( &data ); */

			// Teach the spell
			plr->addSpell(qst->reward_spell);
		}
	}

	// cast Effect Spell
	if(qst->effect_on_player)
	{
		SpellEntry  * inf =dbcSpell.LookupEntry(qst->effect_on_player);
		if(inf)
		{
			Spell * spe = SpellPool.PooledNew( __FILE__, __LINE__ );
			spe->Init(qst_giver,inf,true,NULL);
			SpellCastTargets tgt;
			tgt.m_unitTarget = plr->GetGUID();
			spe->prepare(&tgt);
		}
	}

    //details: hmm as i can remember, repeatable quests give faction rep still after first completation
    if(IsQuestRepeatable(qst) || IsQuestDaily(qst) || IsQuestWeekly(qst))
    {
		// if daily then append to finished dailies
		if ( qst->is_repeatable == arcemu_QUEST_REPEATABLE_DAILY )
			plr->PushToFinishedDailies( qst->id );
		else if ( qst->is_repeatable == arcemu_QUEST_REPEATABLE_WEEKLY )
			plr->PushToFinishedWeeklies( qst->id );
	}
	else
	{
		//Add to finished quests
		if(qst->is_repeatable == arcemu_QUEST_REPEATABLE_DAILY)
			plr->PushToFinishedDailies(qst->id);
		else if(qst->is_repeatable == arcemu_QUEST_REPEATABLE_WEEKLY)
			plr->PushToFinishedDailies(qst->id);
		else if(!IsQuestRepeatable(qst))
			plr->AddToFinishedQuests(qst->id);

		//Remove any timed events
		if (sEventMgr.HasEvent(plr,EVENT_TIMED_QUEST_EXPIRE))
			sEventMgr.RemoveEvents(plr, EVENT_TIMED_QUEST_EXPIRE); 	
	}
}

/////////////////////////////////////
//		Quest Management		 //
/////////////////////////////////////

void QuestMgr::LoadNPCQuests(Creature *qst_giver)
{
	qst_giver->SetQuestList(GetCreatureQuestList(qst_giver->GetEntry()));
}

void QuestMgr::LoadGOQuests(GameObject *go)
{
	go->SetQuestList(GetGOQuestList(go->GetEntry()));
}

QuestRelationList* QuestMgr::GetGOQuestList(uint32 entryid)
{
	HM_NAMESPACE::hash_map<uint32, QuestRelationList* > &olist = _GetList<GameObject>();
	HM_NAMESPACE::hash_map<uint32, QuestRelationList* >::iterator itr = olist.find(entryid);
	return (itr == olist.end()) ? 0 : itr->second;
}

QuestRelationList* QuestMgr::GetCreatureQuestList(uint32 entryid)
{
	HM_NAMESPACE::hash_map<uint32, list<QuestRelation *>* > &olist = _GetList<Creature>();
	HM_NAMESPACE::hash_map<uint32, QuestRelationList* >::iterator itr = olist.find(entryid);
	return (itr == olist.end()) ? 0 : itr->second;
}

template <class T> void QuestMgr::_AddQuest(uint32 entryid, Quest *qst, uint8 type)
{
	HM_NAMESPACE::hash_map<uint32, list<QuestRelation *>* > &olist = _GetList<T>();
	std::list<QuestRelation *>* nlist;
	QuestRelation *ptr = NULL;

	if (olist.find(entryid) == olist.end())
	{
		nlist = new std::list<QuestRelation *>;

		olist.insert(HM_NAMESPACE::hash_map<uint32, list<QuestRelation *>* >::value_type(entryid, nlist));
	}
	else
	{
		nlist = olist.find(entryid)->second;
	}

	list<QuestRelation *>::iterator it;
	for (it = nlist->begin(); it != nlist->end(); ++it)
	{
		if ((*it)->qst == qst)
		{
			ptr = (*it);
			break;
		}
	}

	if (ptr == NULL)
	{
		ptr = new QuestRelation;
		ptr->qst = qst;
		ptr->type = type;

		nlist->push_back(ptr);
	}
	else
	{
		ptr->type |= type;
	}

	//the inverse list is quest having a list of finishers
	list<uint32>::iterator itr2;
	bool has_it = false;
	for( itr2 = m_quests_finisher_npc[ qst->id ].begin(); itr2!=m_quests_finisher_npc[ qst->id ].end(); itr2++ )
		if( *itr2 == entryid )
		{
			has_it = true;
			break;
		}
	if( has_it == false )
		m_quests_finisher_npc[ qst->id ].push_front( entryid );
}



void QuestMgr::_CleanLine(std::string *str) 
{
	_RemoveChar((char*)"\r", str);
	_RemoveChar((char*)"\n", str);

	while (str->c_str()[0] == 32) 
	{
		str->erase(0,1);
	}
}

void QuestMgr::_RemoveChar(char *c, std::string *str) 
{
	string::size_type pos = str->find(c,0);

	while (pos != string::npos)
	{
		str->erase(pos, 1);
		pos = str->find(c, 0);
	}	
}

uint32 QuestMgr::GenerateQuestXP(Player *plr, Quest *qst)	
{
	int questLevel = plr->getLevel();
	if(qst->max_level > 0)
		questLevel = qst->max_level;
	QuestXPEntry* questReward = dbcQuestXP.LookupEntryForced(questLevel);
	if( questReward == NULL )
		return 0;

	float modifier = 1.0f;

	if( plr->getLevel() == qst->max_level +  6 )
	{ 
		modifier = 0.8f;
	}
	if( plr->getLevel() == qst->max_level +  7 )
	{ 
		modifier = 0.6f;
	}
	if( plr->getLevel() == qst->max_level +  8 )
	{ 
		modifier = 0.4f;
	}
	if( plr->getLevel() == qst->max_level +  9 )
	{ 
		modifier = 0.2f;
	}
	switch(qst->rew_xp_rank)
	{
		case 1:
			return (uint32)(questReward->rewXP1 * modifier);
		case 2:
			return (uint32)(questReward->rewXP2 * modifier);
		case 3:
			return (uint32)(questReward->rewXP3 * modifier);
		case 4:
			return (uint32)(questReward->rewXP4 * modifier);
		case 5:
			return (uint32)(questReward->rewXP5 * modifier);
		case 6:
			return (uint32)(questReward->rewXP6 * modifier);
		case 7:
			return (uint32)(questReward->rewXP7 * modifier);
		case 8:
			return (uint32)(questReward->rewXP8 * modifier);
		default:
			return 0;
	}

}

uint32 QuestMgr::GenerateRewardMoney( Player * plr, Quest * qst )
{
//	if ( plr == NULL || !plr->IsInWorld() || plr->getLevel() >= plr->GetUInt32Value(PLAYER_FIELD_MAX_LEVEL) || ( !plr->GetSession()->HasFlag( ACCOUNT_FLAG_XPACK_01 ) && plr->getLevel() != 60 ) || plr->getLevel() != 70 || qst->is_repeatable != 0 )
	{
		return MAX( 0, qst->reward_money );
	}
//	else
	{
//		return qst->reward_money + float2int32( GenerateQuestXP( plr, qst ) * sWorld.getRate( RATE_QUESTXP ) ) * 6;
	}
}
/*
#define XP_INC 50
#define XP_DEC 10
#define XP_INC100 15
#define XP_DEC100 5
	double xp, pxp, mxp, mmx;

	// hack fix
	xp  = qst->max_level * XP_INC;
	if(xp <= 0)
		xp = 1;

	pxp  = xp + (xp / 100) * XP_INC100;

	xp   = XP_DEC;

	mxp  = xp + (xp / 100) * XP_DEC100;

	mmx = (pxp - mxp);

	if(qst->quest_flags & QUEST_FLAG_SPEAKTO)
		mmx *= 0.6;
	if(qst->quest_flags & QUEST_FLAG_TIMED)
		mmx *= 1.1;
	if(qst->quest_flags & QUEST_FLAG_EXPLORATION)
		mmx *= 1.2;

	if(mmx < 0)
		return 1;

	mmx *= sWorld.getRate(RATE_QUESTXP);
	return (int)mmx;*/


void QuestMgr::SendQuestInvalid(INVALID_REASON reason, Player *plyr)
{
	if(!plyr)
	{ 
		return;
	}
#ifdef USING_BIG_ENDIAN
	uint32 swapped = swap32((uint32)reason);
	plyr->GetSession()->OutPacket(SMSG_QUESTGIVER_QUEST_INVALID, 4, &reason);
#else
	plyr->GetSession()->OutPacket(SMSG_QUESTGIVER_QUEST_INVALID, 4, &reason);
#endif	
	sLog.outDebug("WORLD:Sent SMSG_QUESTGIVER_QUEST_INVALID");
}

void QuestMgr::SendQuestFailed(FAILED_REASON failed, Quest * qst, Player *plyr)
{
	if(!plyr)
	{ 
		return;
	}

    WorldPacket data(8);
    data.Initialize(SMSG_QUESTGIVER_QUEST_FAILED);
    data << uint32(qst->id);
    data << failed;
    plyr->GetSession()->SendPacket(&data);
	sLog.outDebug("WORLD:Sent SMSG_QUESTGIVER_QUEST_FAILED");
}

void QuestMgr::SendQuestUpdateFailedTimer(Quest *pQuest, Player *plyr)
{
	if(!plyr)
	{ 
		return;
	}

	plyr->GetSession()->OutPacket(SMSG_QUESTUPDATE_FAILEDTIMER, 4, &pQuest->id);
	sLog.outDebug("WORLD:Sent SMSG_QUESTUPDATE_FAILEDTIMER");
}

void QuestMgr::SendQuestUpdateFailed(Quest *pQuest, Player *plyr)
{
	if(!plyr)
	{ 
		return;
	}

	plyr->GetSession()->OutPacket(SMSG_QUESTUPDATE_FAILED, 4, &pQuest->id);
	sLog.outDebug("WORLD:Sent SMSG_QUESTUPDATE_FAILED");
}

void QuestMgr::SendQuestLogFull(Player *plyr)
{
	if(!plyr)
	{ 
		return;
	}

	plyr->GetSession()->OutPacket(SMSG_QUESTLOG_FULL);
	sLog.outDebug("WORLD:Sent QUEST_LOG_FULL_MESSAGE");
}

uint32 QuestMgr::GetGameObjectLootQuest(uint32 GO_Entry)
{
	HM_NAMESPACE::hash_map<uint32, uint32>::iterator itr = m_ObjectLootQuestList.find(GO_Entry);
	if(itr == m_ObjectLootQuestList.end()) 
	{ 
		return 0;
	}
	
	return itr->second;
}

void QuestMgr::SetGameObjectLootQuest(uint32 GO_Entry, uint32 Item_Entry)
{
	if(m_ObjectLootQuestList.find(GO_Entry) != m_ObjectLootQuestList.end())
	{
		//sLog.outError("WARNING: Gameobject %d has more than 1 quest item allocated in it's loot template!", GO_Entry);
	}

	// Find the quest that has that item
	uint32 QuestID = 0;
	uint32 i;
	StorageContainerIterator<Quest> * itr = QuestStorage.MakeIterator();
	while(!itr->AtEnd())
	{
		Quest *qst = itr->Get();
		for(i = 0; i < 6; ++i)
		{
			if(qst->required_item[i] == Item_Entry)
			{
				QuestID = qst->id;
				m_ObjectLootQuestList[GO_Entry] = QuestID;
				itr->Destruct();
				return;
			}
		}
		if(!itr->Inc())
			break;
	}
	itr->Destruct();

	//sLog.outError("WARNING: No coresponding quest was found for quest item %d", Item_Entry);
}

void QuestMgr::BuildQuestFailed(WorldPacket* data, uint32 questid)
{
	data->Initialize(SMSG_QUESTUPDATE_FAILEDTIMER);
	*data << questid;
}

bool QuestMgr::OnActivateQuestGiver(Object *qst_giver, Player *plr)
{
	if(qst_giver->GetTypeId() == TYPEID_GAMEOBJECT && !SafeGOCast(qst_giver)->HasQuests())
	{ 
		return false;
	}

	uint32 questCount = sQuestMgr.ActiveQuestsCount(qst_giver, plr);
	WorldPacket data(1001);	

	if (questCount == 0) 
	{
		sLog.outDebug("WORLD: Invalid NPC for CMSG_QUESTGIVER_HELLO.");
		return false;
	}
	else if (questCount == 1)
	{
		std::list<QuestRelation *>::const_iterator itr;
		std::list<QuestRelation *>::const_iterator q_begin;
		std::list<QuestRelation *>::const_iterator q_end;

		bool bValid = false;

		if(qst_giver->GetTypeId() == TYPEID_GAMEOBJECT)
		{
            bValid = SafeGOCast(qst_giver)->HasQuests();
            if(bValid)
            {
				q_begin = SafeGOCast(qst_giver)->QuestsBegin();
				q_end   = SafeGOCast(qst_giver)->QuestsEnd();
			}
		} 
		else if(qst_giver->GetTypeId() == TYPEID_UNIT)
		{
			bValid = SafeCreatureCast(qst_giver)->HasQuests();
			if(bValid)
			{
				q_begin = SafeCreatureCast(qst_giver)->QuestsBegin();
				q_end   = SafeCreatureCast(qst_giver)->QuestsEnd();
			}
		}

		if(!bValid)
		{
			sLog.outDebug("QUESTS: Warning, invalid NPC "I64FMT" specified for OnActivateQuestGiver. TypeId: %d.", qst_giver->GetGUID(), qst_giver->GetTypeId());
			return false;
		}
		
		for(itr = q_begin; itr != q_end; ++itr) 
//			if (sQuestMgr.CalcQuestStatus(qst_giver, plr, *itr) >= QMGR_QUEST_CHAT)
			if (sQuestMgr.CalcQuestStatus(qst_giver, plr, *itr) > QMGR_QUEST_AVAILABLELOW_LEVEL )
				break;

		if( ( sQuestMgr.CalcStatus(qst_giver, plr) & ( QMGR_QUEST_REPEATABLE_FINISHED | QMGR_QUEST_FINISHED | QMGR_QUEST_FINISHED_2 ) ) == 0 )
		{ 
			return false; 
		}

		ASSERT(itr != q_end);

		uint32 status = sQuestMgr.CalcStatus(qst_giver, plr);

		if( status & ( QMGR_QUEST_AVAILABLE | QMGR_QUEST_REPEATABLE | QMGR_QUEST_CHAT ) )
		{
			sQuestMgr.BuildQuestDetails(&data, (*itr)->qst, qst_giver, 1, plr->GetSession()->language, plr);		// 1 because we have 1 quest, and we want goodbye to function
			plr->GetSession()->SendPacket(&data);
			if( (*itr)->qst->quest_flags & QUEST_FLAGS_AUTO_ACCEPT )
			{
				uint64 guid = qst_giver->GetGUID();
				sQuestMgr.TryAutoAcceptQuest( plr->GetSession(), guid, (*itr)->qst->id );
			}
			sLog.outDebug( "WORLD: Sent SMSG_QUESTGIVER_QUEST_DETAILS." );
		}
		else if( status & ( QMGR_QUEST_FINISHED | QMGR_QUEST_FINISHED_2 ) )
		{
			sQuestMgr.BuildOfferReward(&data, (*itr)->qst, qst_giver, 1, plr->GetSession()->language, plr);
			plr->GetSession()->SendPacket(&data);
			//ss
			sLog.outDebug( "WORLD: Sent SMSG_QUESTGIVER_OFFER_REWARD." );
		}
		else if( status & QMGR_QUEST_NOT_FINISHED )
		{
			sQuestMgr.BuildRequestItems(&data, (*itr)->qst, qst_giver, status, plr->GetSession()->language);
			plr->GetSession()->SendPacket(&data);
			sLog.outDebug( "WORLD: Sent SMSG_QUESTGIVER_REQUEST_ITEMS." );
		}
	}
	else 
	{
		sQuestMgr.BuildQuestList(&data, qst_giver ,plr, plr->GetSession()->language);
		plr->GetSession()->SendPacket(&data);
		sLog.outDebug( "WORLD: Sent SMSG_QUESTGIVER_QUEST_LIST." );
	}
	return true;
}

QuestMgr::~QuestMgr()
{
	HM_NAMESPACE::hash_map<uint32, Quest*>::iterator itr1;
	HM_NAMESPACE::hash_map<uint32, list<QuestRelation *>* >::iterator itr2;
	list<QuestRelation*>::iterator itr3;
	HM_NAMESPACE::hash_map<uint32, list<QuestAssociation *>* >::iterator itr4;
	list<QuestAssociation *>::iterator itr5;

	// clear relations
	for(itr2 = m_obj_quests.begin(); itr2 != m_obj_quests.end(); ++itr2)
	{
		if(!itr2->second)
			continue;

		itr3 = itr2->second->begin();
		for(; itr3 != itr2->second->end(); ++itr3)
		{
			delete (*itr3);
			(*itr3) = NULL;
		}
		itr2->second->clear();
		delete itr2->second;
		itr2->second = NULL;
	}

	m_obj_quests.clear();

	for(itr2 = m_npc_quests.begin(); itr2 != m_npc_quests.end(); ++itr2)
	{
		if(!itr2->second)
			continue;

		itr3 = itr2->second->begin();
		for(; itr3 != itr2->second->end(); ++itr3)
		{
			delete (*itr3);
			(*itr3) = NULL;
		}
		itr2->second->clear();
		delete itr2->second;
		itr2->second = NULL;
	}

	m_npc_quests.clear();

	for(itr2 = m_itm_quests.begin(); itr2 != m_itm_quests.end(); ++itr2)
	{
		if(!itr2->second)
			continue;

		itr3 = itr2->second->begin();
		for(; itr3 != itr2->second->end(); ++itr3)
		{
			delete (*itr3);
			(*itr3) = NULL;
		}
		itr2->second->clear();
		delete itr2->second;
		itr2->second = NULL;
	}
	m_itm_quests.clear();
	for(itr4 = m_quest_associations.begin(); itr4 != m_quest_associations.end(); ++itr4)
	{
		if(!itr4->second)
			continue;

		itr5 = itr4->second->begin();
		for(; itr5 != itr4->second->end(); ++itr5)
		{
			delete (*itr5);
			(*itr5) = NULL;
		}
		itr4->second->clear();
		delete itr4->second;
		itr4->second = NULL;
	}
	// NTY.
	m_quest_associations.clear();
}


bool QuestMgr::CanStoreReward(Player *plyr, Quest *qst, uint32 reward_slot)
{
    uint32 available_slots = 0;
    uint32 slotsrequired = 0;
    available_slots = plyr->GetItemInterface()->CalculateFreeSlots(NULL);
    // Static Item reward
    for(uint32 i = 0; i < 4; ++i)
    {
        if(qst->reward_item[i])
        {
            slotsrequired++;
            ItemPrototype *proto = ItemPrototypeStorage.LookupEntry(qst->reward_item[i]);
            if(!proto)
                sLog.outError("Invalid item prototype in quest reward! ID %d, quest %d", qst->reward_item[i], qst->id);
            else if(plyr->GetItemInterface()->CanReceiveItem(proto, qst->reward_itemcount[i]))
            { 
				return false;
            }
        }
    }

    // Choice Rewards
    if(qst->reward_choiceitem[reward_slot])
    {
        slotsrequired++;
        ItemPrototype *proto = ItemPrototypeStorage.LookupEntry(qst->reward_choiceitem[reward_slot]);
        if(!proto)
            sLog.outError("Invalid item prototype in quest reward! ID %d, quest %d", qst->reward_choiceitem[reward_slot], qst->id);
        else if(plyr->GetItemInterface()->CanReceiveItem(proto, qst->reward_choiceitemcount[reward_slot]))
        { 
			return false;
        }
    }
    if(available_slots < slotsrequired)
	{
        return false;
	}

	return true;
}

void QuestMgr::LoadExtraQuestStuff()
{
	StorageContainerIterator<Quest> * it = QuestStorage.MakeIterator();
	Quest * qst;
	while(!it->AtEnd())
	{
		qst = it->Get();

		// 0 them out
		qst->count_required_item = 0;
		qst->count_required_mob = 0;
		qst->count_requiredtriggers = 0;
		qst->count_receiveitems = 0;
		qst->count_reward_item = 0;
		qst->count_reward_choiceitem = 0;

		qst->required_mobtype[0] = 0;
		qst->required_mobtype[1] = 0;
		qst->required_mobtype[2] = 0;
		qst->required_mobtype[3] = 0;

		qst->count_requiredquests = 0;

		for(int i = 0 ; i < 4; ++i)
		{
			if(qst->required_mob[i])
			{
				if(qst->required_mob[i] < 0)
				{
					qst->required_mobtype[i] = QUEST_MOB_TYPE_GAMEOBJECT;
					qst->required_mob[i] = abs(qst->required_mob[i]);
				}
				else
				{
					qst->required_mobtype[i] = QUEST_MOB_TYPE_CREATURE;
				}
				qst->count_required_mob++;
			}

			if(qst->required_item[i])
				qst->count_required_item++;

			if(qst->reward_item[i])
				qst->count_reward_item++;

			if(qst->required_triggers[i])
				qst->count_requiredtriggers++;

			if(qst->receive_items[i])
				qst->count_receiveitems++;

			if(qst->required_quests[i])
				qst->count_requiredquests++;
		}

		for(int i = 0; i < 6; ++i)
		{
			if(qst->reward_choiceitem[i])
				qst->count_reward_choiceitem++;
		}

		if(!it->Inc())
			break;
	}

	it->Destruct();

	// load creature starters
	uint32 creature, quest;

	QueryResult * pResult = WorldDatabase.Query("SELECT * FROM creature_quest_starter");
	uint32 pos = 0;
	uint32 total;
	if(pResult)
	{
		total = pResult->GetRowCount();
		do 
		{
			Field *data = pResult->Fetch();
			creature = data[0].GetUInt32();
			quest = data[1].GetUInt32();

			qst = QuestStorage.LookupEntry(quest);
			if(!qst)
			{
				//printf("Tried to add starter to npc %d for non-existant quest %d.\n", creature, quest);
			}
			else 
			{
				_AddQuest<Creature>(creature, qst, 1);  // 1 = starter
			}
		} while(pResult->NextRow());
		delete pResult;
		pResult = NULL;
	}

	pResult = WorldDatabase.Query("SELECT * FROM creature_quest_finisher");
	pos = 0;
	if(pResult)
	{
		total = pResult->GetRowCount();
		do 
		{
			Field *data = pResult->Fetch();
			creature = data[0].GetUInt32();
			quest = data[1].GetUInt32();

			qst = QuestStorage.LookupEntry(quest);
			if(!qst)
			{
				//printf("Tried to add finisher to npc %d for non-existant quest %d.\n", creature, quest);
			} 
			else 
			{
				_AddQuest<Creature>(creature, qst, 2);  // 1 = starter
			}
		} while(pResult->NextRow());
		delete pResult;
		pResult = NULL;
	}

	pResult = WorldDatabase.Query("SELECT * FROM gameobject_quest_starter");
	pos = 0;
	if(pResult)
	{
		total = pResult->GetRowCount();
		do 
		{
			Field *data = pResult->Fetch();
			creature = data[0].GetUInt32();
			quest = data[1].GetUInt32();

			qst = QuestStorage.LookupEntry(quest);
			if(!qst)
			{
				//printf("Tried to add starter to go %d for non-existant quest %d.\n", creature, quest);
			} 
			else
			{
				_AddQuest<GameObject>(creature, qst, 1);  // 1 = starter
			}
		} while(pResult->NextRow());
		delete pResult;
		pResult = NULL;
	}

	pResult = WorldDatabase.Query("SELECT * FROM gameobject_quest_finisher");
	pos = 0;
	if(pResult)
	{
		total = pResult->GetRowCount();
		do 
		{
			Field *data = pResult->Fetch();
			creature = data[0].GetUInt32();
			quest = data[1].GetUInt32();

			qst = QuestStorage.LookupEntry(quest);
			if(!qst)
			{
				//printf("Tried to add finisher to go %d for non-existant quest %d.\n", creature, quest);
			} 
			else 
			{
				_AddQuest<GameObject>(creature, qst, 2);  // 2 = finish
			}
		} while(pResult->NextRow());
		delete pResult;
		pResult = NULL;
	}
	objmgr.ProcessGameobjectQuests();

	//load item quest associations
	uint32 item;
	uint8 item_count;

	pResult = WorldDatabase.Query("SELECT * FROM item_quest_association");
	pos = 0;
	if( pResult != NULL)
	{
		total = pResult->GetRowCount();
		do 
		{
			Field *data = pResult->Fetch();
			item = data[0].GetUInt32();
			quest = data[1].GetUInt32();
			item_count = data[2].GetUInt8();

			qst = QuestStorage.LookupEntry(quest);
			if(!qst)
			{
				//printf("Tried to add association to item %d for non-existant quest %d.\n", item, quest);
			} 
			else 
			{
				AddItemQuestAssociation( item, qst, item_count );
			}
		} while( pResult->NextRow() );
		delete pResult;
		pResult = NULL;
	}
}

void QuestMgr::AddItemQuestAssociation( uint32 itemId, Quest *qst, uint8 item_count)
{
	HM_NAMESPACE::hash_map<uint32, list<QuestAssociation *>* > &associationList = GetQuestAssociationList();
	std::list<QuestAssociation *>* tempList;
	QuestAssociation *ptr = NULL;
	
	// look for the item in the associationList
	if (associationList.find( itemId ) == associationList.end() )
	{
		// not found. Create a new entry and QuestAssociationList
		tempList = new std::list<QuestAssociation *>;

		associationList.insert(HM_NAMESPACE::hash_map<uint32, list<QuestAssociation *>* >::value_type(itemId, tempList));
	}
	else
	{
		// item found, now we'll search through its QuestAssociationList
		tempList = associationList.find( itemId )->second;
	}
	
	// look through this item's QuestAssociationList for a matching quest entry
	list<QuestAssociation *>::iterator it;
	for (it = tempList->begin(); it != tempList->end(); ++it)
	{
		if ((*it)->qst == qst)
		{
			// matching quest found
			ptr = (*it);
			break;
		}
	}

	// did we find a matching quest?
	if (ptr == NULL)
	{
		// nope, create a new QuestAssociation for this item and quest
		ptr = new QuestAssociation;
		ptr->qst = qst;
		ptr->item_count = item_count;

		tempList->push_back( ptr );
	}
	else
	{
		// yep, update the QuestAssociation with the new item_count information 
		ptr->item_count = item_count;
		sLog.outDebug( "WARNING: Duplicate entries found in item_quest_association, updating item #%d with new item_count: %d.", itemId, item_count );
	}
}

QuestAssociationList* QuestMgr::GetQuestAssociationListForItemId (uint32 itemId)
{
	HM_NAMESPACE::hash_map<uint32, QuestAssociationList* > &associationList = GetQuestAssociationList();
	HM_NAMESPACE::hash_map<uint32, QuestAssociationList* >::iterator itr = associationList.find( itemId );
	if( itr == associationList.end() )
	{
		return 0;
	} else {
		return itr->second;
	}
}
