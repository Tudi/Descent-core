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

//Pakcet Building
/////////////////

WorldPacket* WorldSession::BuildQuestQueryResponse(Quest *qst)
{
	// 2048 bytes should be more than enough. The fields cost ~200 bytes.
	// better to allocate more at startup than have to realloc the buffer later on.

	WorldPacket *data = new WorldPacket(SMSG_QUEST_QUERY_RESPONSE, 248);
	LocalizedQuest * lci = (language>0) ? sLocalizationMgr.GetLocalizedQuest(qst->id, language) : NULL;
   
	*data << uint32(qst->id);					   // Quest ID
	*data << uint32(qst->IsActiveFlags);	 	   // Unknown, always seems to be 2
	*data << uint32(qst->max_level);				// Quest level
	*data << uint32(qst->min_level);

	*data << uint32(qst->zone_id);			  // Positive if pointing to a zone.

	*data << uint32(qst->type);					 // Info ID / Type
	*data << qst->suggestedplayers;								// suggested players
	*data << uint32(qst->objective_rep_faction);	 // Faction ID
	*data << uint32(qst->objective_rep_value);	   // Faction Amount
	*data << uint32(qst->objective_rep_faction2);	
	*data << uint32(qst->objective_rep_value2);		
	*data << uint32(qst->next_quest_id);			// Next Quest ID
	*data << uint32(qst->rew_xp_rank);					//
	*data << uint32(qst->reward_money);				// Reward Money
	*data << uint32(qst->rew_money_at_max_level);		   // Money at max level
	*data << uint32(qst->reward_spell);			 // Spell added to spellbook upon completion
	*data << uint32(qst->effect_on_player);		 // Spell casted on player upon completion
	*data << qst->bonushonor;								// 2.3.0 - bonus honor
	*data << qst->Honor2;
	*data << uint32(qst->srcitem);				  // Item given at the start of a quest (srcitem)
	*data << uint32(qst->quest_flags);			  // Quest Flags
	*data << qst->QuestFlags2;				
	*data << qst->rewardtitleid;								// 2.4.0 unk
	*data << qst->req_playerkillcount;								// playerkillcount
	*data << qst->rewardtalents;								// 3.0.2
	*data << uint32(qst->BonusArenaPoints);
	*data << uint32(qst->RewardSkillLineID);
	*data << uint32(qst->RewardSkillPoints);	
	*data << uint32(qst->RewardFactionMask);	
	*data << uint32(qst->QuestGiverPortraitID);	
	*data << uint32(qst->QuestTurnInPortraitID);

	for(uint32 i = 0; i < 4; ++i)
	{
		*data << qst->reward_item[i];			   // Forced Reward Item [i]
		*data << qst->reward_itemcount[i];		  // Forced Reward Item Count [i]
	}
	for(uint32 i = 0; i < 6; ++i)
	{
		*data << qst->reward_choiceitem[i];		 // Choice Reward Item [i]
		*data << qst->reward_choiceitemcount[i];	// Choice Reward Item Count [i]
	}
	for(uint32 i = 0; i < 5; ++i)
		*data << qst->rew_faction[i];		 
	for(uint32 i = 0; i < 5; ++i)
		*data << qst->rew_faction_repA[i];	
	for(uint32 i = 0; i < 5; ++i)
		*data << qst->rew_faction_repB[i];	

	*data << qst->point_mapid;					  // Unknown
	*data << qst->point_x;						  // Unknown
	*data << qst->point_y;						  // Unknown
	*data << qst->point_opt;						// Unknown
	
	if(lci)
	{
		*data << lci->Title;
		*data << lci->Objectives;
		*data << lci->Details;
		*data << lci->EndText;
		*data << uint8(0);						 
	}
	else
	{
		*data << qst->title;							// Title / name of quest
		*data << qst->objectives;					   // Objectives / description
		*data << qst->details;						  // Details
		*data << qst->endtext;						  // Subdescription
		*data << qst->CompletionText;						 
	}

	for(uint32 i = 0; i < 4; ++i)
	{
		*data << qst->required_mob[i];			 // Kill mob entry ID [i]
		*data << qst->required_mobcount[i];		 // Kill mob count [i]
		*data << qst->linkItemId[i];			 // required item to be used on target
		*data << qst->linkItemIdCount[i];			 // required item to be used on target
	}

	for(uint32 i = 0; i < 6; ++i)
	{
		*data << qst->required_item[i];			 // Collect item [i]
		*data << qst->required_itemcount[i];		// Collect item count [i]
	}

	*data << uint32(qst->CriteriaSpellID);		// 4.0.3 unk
	if(lci)
	{
		*data << lci->ObjectiveText[0];
		*data << lci->ObjectiveText[1];
		*data << lci->ObjectiveText[2];
		*data << lci->ObjectiveText[3];
	}
	else
	{
		*data << qst->objectivetexts[0];				// Objective 1 - Used as text if mob not set
		*data << qst->objectivetexts[1];				// Objective 2 - Used as text if mob not set
		*data << qst->objectivetexts[2];				// Objective 3 - Used as text if mob not set
		*data << qst->objectivetexts[3];				// Objective 4 - Used as text if mob not set
	}

	for(uint32 i = 0; i < 4; ++i)                               // 4.0.0 currency reward id and count
	{
		*data << qst->RewCurrency[i].Type;
		*data << qst->RewCurrency[i].Count;
	}

	for(uint32 i = 0; i < 4; ++i)                               // 4.0.0 currency required id and count
	{
		*data << qst->ReqCurrency[i].Type;
		*data << qst->ReqCurrency[i].Count;
	}

	for(uint32 i = 0; i < 2; ++i)                             
		*data << qst->npcframe_accept_text[i];		
	for(uint32 i = 0; i < 2; ++i)                             
		*data << qst->npcframe_handin_text[i];		

	*data << uint32(qst->soundID1);										// 4.0.1 unk
	*data << uint32(qst->soundID2);										// 4.0.1 unk

	 return data;
}

void WorldSession::HandleQuestPOIQuery(WorldPacket & recv_data)
{
    uint32 count;
    recv_data >> count;                                     // quest count, max=25

    if( count >= MAX_QUEST_LOG_SIZE )
        return;

    WorldPacket data(SMSG_QUEST_POI_QUERY_RESPONSE, 4+(4+4)*count);
    data << uint32(count);                                  // count

    for(uint32 i = 0; i < count; ++i)
    {
        uint32 questId;
        recv_data >> questId;                               // quest id

		QuestPOIVector const *POI = objmgr.GetQuestPOIVector(questId);

        if(POI)
        {
            data << uint32(questId);                    // quest ID
			uint32 size = (uint32)POI->size();
            data << uint32(size);						// POI count

            int index = 0;
            for(QuestPOIVector::const_iterator itr = POI->begin(); itr != POI->end(); ++itr)
            {
                data << uint32(index);						// POI index
                data << int32(itr->ObjectiveIndex);			// objective index
                data << uint32(itr->MapId);					// mapid
                data << uint32(itr->WorldMapAreaDBCIndex);  
                data << uint32(itr->FloorLevel);
                data << uint32(itr->Unk3);
                data << uint32(itr->Unk4);
				uint32 size = (uint32)itr->points.size();
                data << uint32(size);						// POI points count

                for(std::vector<QuestPOIPoint>::const_iterator itr2 = itr->points.begin(); itr2 != itr->points.end(); ++itr2)
                {
                    data << int32(itr2->x);             // POI point x
                    data << int32(itr2->y);             // POI point y
                }
                ++index;
            }
        }
        else
        {
            data << uint32(questId);                    // quest ID
            data << uint32(0);                          // POI count
        }
    }
	//on live servers if packet is too large it will not be sent and client blocks on it. We will tell client there are no POI messages and 
	if( data.size() >= WORLDSOCKET_SENDBUF_SIZE )
	{
		GetPlayer()->BroadcastMessage("You have too many active quests and minimap data cannot be sent. Abandon a few quests and relog to see minimap data markers");
		WorldPacket data(SMSG_QUEST_POI_QUERY_RESPONSE, 4+(4+4)*count);
		data << uint32(count);                // count
		for(uint32 i = 0; i < count; ++i)
		{
			uint32 questId;
			recv_data >> questId;             // quest id
			data << uint32(questId);          // quest ID
			data << uint32(0);                // POI count
		}
	    SendPacket(&data);
		return;
	}
    SendPacket(&data);
}

//!!not confirmed
void WorldSession::HandleQuestsCompleted(WorldPacket &recv_data)
{
	uint32 count = 0;
    WorldPacket data(SMSG_QUERY_QUESTS_COMPLETED_RESPONSE, 4*MAX_QUEST_LOG_SIZE);
    data << uint32(count);                                  // count

    for(uint32 i = 0; i < MAX_QUEST_LOG_SIZE; ++i)
	{
		QuestLogEntry* qle = GetPlayer()->GetQuestLogInSlot( i );
		if( qle == NULL )
			continue;
		if (qle->CanBeFinished())
		{
			data << uint32( qle->GetQuest()->id );
			count++;
		}
	}
	data.wpos(0);
	data << count;
	SendPacket(&data);
}

/*****************
* QuestLogEntry *
*****************/
QuestLogEntry::QuestLogEntry()
{
	mInitialized = false;
	m_quest = NULL;
	mDirty = false;
	m_slot = -1;
	completed=0;
}

QuestLogEntry::~QuestLogEntry()
{

}

void QuestLogEntry::Init(Quest* quest, Player* plr, uint32 slot)
{
	ASSERT(quest);
	ASSERT(plr);

	m_quest = quest;
	m_plr = plr;
	m_slot = slot;

	iscastquest = false;
	for (uint32 i=0;i<4;++i)
	{
		if (quest->required_spell[i]!=0)
		{
			iscastquest=true;
			if (!plr->HasQuestSpell(quest->required_spell[i]))
				plr->quest_spells.insert(quest->required_spell[i]);
		}
		else if (quest->required_mob[i]!=0)
		{
			if (!plr->HasQuestMob(quest->required_mob[i]))
				plr->quest_mobs.insert(quest->required_mob[i]);
		}
	}


	// update slot
	plr->SetQuestLogSlot(this, slot);
	
	mDirty = true;

	memset(&m_mobcount[0], 0, 4*4);
	memset(&m_explored_areas[0], 0, 4*4);

	if(m_quest->time)
		m_time_left = m_quest->time;
	else
		m_time_left = 0;

	if (!plr->GetSession()->m_loggingInPlayer) //quest script should not be called on login
		CALL_QUESTSCRIPT_EVENT(this, OnQuestStart)(plr, this);
}

void QuestLogEntry::ClearAffectedUnits()
{
	if (m_affected_units.size()>0)
		m_affected_units.clear();
}
void QuestLogEntry::AddAffectedUnit(Unit* target)
{
	if (!target)
	{ 
		return;
	}
	if (!IsUnitAffected(target))
		m_affected_units.insert(target->GetGUID());
}
bool QuestLogEntry::IsUnitAffected(Unit* target)
{
	if (!target)
	{ 
		return true;
	}
	if (m_affected_units.find(target->GetGUID()) != m_affected_units.end())
	{ 
		return true;
	}
	return false;
}

void QuestLogEntry::SaveToDB(QueryBuffer * buf)
{
	ASSERT(m_slot != -1);
	if(!mDirty)
	{ 
		return;
	}

	//Made this into a replace not an insert
	//CharacterDatabase.Execute("DELETE FROM questlog WHERE player_guid=%u AND quest_id=%u", m_plr->GetGUIDLow(), m_quest->id);
	std::stringstream ss;
	ss << "REPLACE INTO questlog VALUES(";
	ss << m_plr->GetLowGUID() << "," << m_quest->id << "," << m_slot << "," << m_time_left;
	for(int i = 0; i < 4; ++i)
		ss << "," << m_explored_areas[i];
	
	for(int i = 0; i < 4; ++i)
		ss << "," << m_mobcount[i];

	ss << ")";
	
	if( buf == NULL )
		CharacterDatabase.Execute( ss.str().c_str() );
	else
		buf->AddQueryStr(ss.str());
}

bool QuestLogEntry::LoadFromDB(Field *fields)
{
	// playerguid,questid,timeleft,area0,area1,area2,area3,kill0,kill1,kill2,kill3
	int f = 3;
	ASSERT(m_plr && m_quest);
	m_time_left = fields[f].GetUInt32();	f++;
	for(int i = 0; i < 4; ++i)
	{
		m_explored_areas[i] = fields[f].GetUInt32();	f++;
		CALL_QUESTSCRIPT_EVENT(this, OnExploreArea)(m_explored_areas[i], m_plr, this);
	}

	for(int i = 0; i < 4; ++i)
	{
		m_mobcount[i] = fields[f].GetUInt32();	f++;
		if(GetQuest()->required_mobtype[i] == QUEST_MOB_TYPE_CREATURE)
			CALL_QUESTSCRIPT_EVENT(this, OnCreatureKill)(GetQuest()->required_mob[i], m_plr, this);
		else
			CALL_QUESTSCRIPT_EVENT(this, OnGameObjectActivate)(GetQuest()->required_mob[i], m_plr, this);
	}
	mDirty = false;
	return true;
}

bool QuestLogEntry::CanBeFinished()
{
	uint32 i;
	for(i = 0; i < 4; ++i)
	{
		if(m_quest->required_mob[i])
		{
			if((int32)m_mobcount[i] < m_quest->required_mobcount[i])
			{
				return false;
			}
		}
	}

	for(i = 0; i < 6; ++i)
	{
		if(m_quest->required_item[i])
		{
			if((int32)m_plr->GetItemInterface()->GetItemCount(m_quest->required_item[i]) < m_quest->required_itemcount[i])
			{
				return false;
			}
		}
	}

	//Check for Gold & AreaTrigger Requirements
	if ( m_quest->reward_money < 0 && m_plr->GetGold( ) < -m_quest->reward_money )
	{ 
		return false;
	}

	for(i = 0; i < 4; ++i)
	{
		if(m_quest->required_triggers[i])
		{
			if(m_explored_areas[i] == 0)
			{ 
				return false;
			}
		}
	}

	//check for required currency
	for(i = 0; i < 4; ++i)
		if( m_quest->ReqCurrency[i].Type > 0 && m_plr->GetCurrencyCount( m_quest->ReqCurrency[i].Type ) < m_quest->ReqCurrency[i].Count )
		{
			return false;
		}

	return true;
}


void QuestLogEntry::HackComplete()
{
	uint32 i;
	for(i = 0; i < 4; ++i)
		m_mobcount[i] = m_quest->required_mobcount[i];

	for(i = 0; i < 6; ++i)
		if(m_quest->required_item[i])
		{
			uint32 count = m_plr->GetItemInterface()->GetItemCount( m_quest->required_item[i] );
			if( (int32)count < m_quest->required_itemcount[i] )
				m_plr->GetItemInterface()->AddItemById( m_quest->required_item[i], m_quest->required_itemcount[i] - count, 0 );
		}

	for(i = 0; i < 4; ++i)
		if( m_quest->ReqCurrency[i].Type > 0 )
			m_plr->ModCurrencyCount( m_quest->ReqCurrency[i].Type, -((int32)m_quest->ReqCurrency[i].Count) );

	//Check for Gold
	if ( m_quest->reward_money < 0 && m_plr->GetGold( ) < -m_quest->reward_money )
	{
		m_plr->ModGold( m_quest->reward_money );
		m_plr->Event_AchiementCriteria_Received( ACHIEVEMENT_CRITERIA_TYPE_MONEY_FROM_QUEST_REWARD,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,m_quest->reward_money,ACHIEVEMENT_EVENT_ACTION_ADD);
	}
	// Check for  AreaTrigger Requirements
	for(i = 0; i < 4; ++i)
		if(m_quest->required_triggers[i])
			m_explored_areas[i] = 1; // what to put here ?
}

void QuestLogEntry::SubtractTime(uint32 value)
{
	if(this->m_time_left  <=value)
		m_time_left = 0;
	else
		m_time_left-=value;
}

void QuestLogEntry::SetMobCount(uint32 i, uint32 count)
{
	ASSERT(i<4);
	m_mobcount[i] = count;
	mDirty = true;
}

void QuestLogEntry::SetTrigger(uint32 i)
{
	ASSERT(i<4);
	m_explored_areas[i] = 1;
	mDirty = true;
}

void QuestLogEntry::SetSlot(int32 i)
{
	ASSERT(i!=-1);
	m_slot = i;
}

void QuestLogEntry::Finish()
{
	uint32 base = GetBaseField(m_slot);
	m_plr->SetUInt32Value(base + 0, 0);
	m_plr->SetUInt32Value(base + 1, 0);
	m_plr->SetUInt32Value(base + 2, 0);
	m_plr->SetUInt32Value(base + 3, 0);

	// clear from player log
	m_plr->SetQuestLogSlot(NULL, m_slot);
	m_plr->PushToRemovedQuests(m_quest->id);
	// delete ourselves


	delete this;
}

void QuestLogEntry::UpdatePlayerFields()
{
	if( !m_plr || m_plr->deleted )
	{ 
		return;
	}

	uint32 base = GetBaseField(m_slot);
	m_plr->SetUInt32Value(base + 0, m_quest->id);

	// next field is kills and shit like that
	uint32 field1[3] = { 0, 0, 0 };	//required mobcount / 2 = 4 / 2 = 2 ?
	int i;
	
	// explored areas
	if(m_quest->count_requiredtriggers)
	{
		uint32 count = 0;
		for(i = 0; i < 4; ++i)
		{
			if(m_quest->required_triggers[i])
			{
				if(m_explored_areas[i] == 1)
				{
					count++;
				}
			}
		}

		if(count == m_quest->count_requiredtriggers)
		{
			field1[1] |= 0x01000000;
		}
	}

	// mob hunting
	if(m_quest->count_required_mob)
	{
		// optimized this - burlex
//		uint8 * p = (uint8*)&field1;
		uint16 * p = (uint16*)&field1;	//as of 3.3.2 ? Maybe byte[0] -> MOB, Byte[1] -> GO ?
		for(int i = 0; i < 4; ++i)
		{
			if( m_quest->required_mob[i] && m_mobcount[i] > 0 )
				p[i] |= (uint8)m_mobcount[i];
		}
	}

	{
		m_plr->SetUInt32Value(base + 1, 0);
		m_plr->SetUInt32Value(base + 2, field1[0]);
		m_plr->SetUInt32Value(base + 3, field1[1]);
		m_plr->SetUInt32Value(base + 4, ( m_time_left ? (uint32)(UNIXTIME+m_time_left/1000) : 0 ) );
	}

	// Timed quest handler.
	if(m_time_left && !sEventMgr.HasEvent( m_plr,EVENT_TIMED_QUEST_EXPIRE ))
		m_plr->EventTimedQuestExpire(m_quest, this , m_slot);
}

void QuestLogEntry::SendQuestComplete()
{
	m_plr->Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_QUEST,m_quest->id,ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
	m_plr->Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_QUEST_COUNT,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
	m_plr->Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_QUESTS_IN_ZONE,m_plr->GetZoneId(),ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
	if( m_quest->is_repeatable == arcemu_QUEST_REPEATABLE_DAILY )
	{
		//Complete a daily quest every day for five consecutive days -> We are doing it wrong !!
		m_plr->Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_DAILY_QUEST_DAILY,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
		m_plr->Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_DAILY_QUEST,ACHIEVEMENT_UNUSED_FIELD_VALUE,ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
	}
	WorldPacket data(4);
	data.SetOpcode(SMSG_QUESTUPDATE_COMPLETE);
	data << m_quest->id;
	m_plr->GetSession()->SendPacket(&data);
	CALL_QUESTSCRIPT_EVENT(this, OnQuestComplete)(m_plr, this);
}

void QuestLogEntry::SendUpdateAddKill(uint32 i)
{
	sQuestMgr.SendQuestUpdateAddKill(m_plr, m_quest->id, m_quest->required_mob[i], m_mobcount[i], m_quest->required_mobcount[i], 0);
}



