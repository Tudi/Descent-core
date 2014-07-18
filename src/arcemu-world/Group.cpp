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

Group::Group(bool Assign)
{
	m_GroupType = GROUP_TYPE_PARTY;	 // Always init as party

	//memset(&m_instanceIds[0], 0, sizeof(uint32) * NUM_MAPS * NUM_INSTANCE_MODES);

	m_Leader = NULL;
	m_Looter = NULL;
	m_LootMethod = PARTY_LOOT_GROUP;
	m_LootThreshold = 2;
	m_SubGroupCount = 1;
	m_MemberCount = 0;

	// Create initial subgroup
	for( uint32 i=0;i<m_SubGroupCount;i++)
		m_SubGroups[i] = new SubGroup(this, 0);
	for( uint32 i=m_SubGroupCount;i<8;i++)
		m_SubGroups[i] = NULL;


	if( Assign )
	{
		m_Id = objmgr.GenerateGroupId();
		m_GUID = m_Id | ((uint64)HIGHGUID_GROUP<<32);
		ObjectMgr::getSingleton().AddGroup(this);
	}

	m_dirty=false;
	m_updateblock=false;
	m_disbandOnNoMembers = true;

	memset(&m_targetIcons[0], 0, sizeof(uint64) * 8);
	m_isqueued=false;
	raid_difficulty=0;
	raid_difficulty=0;
	m_assistantLeader=m_mainAssist=m_mainTank=NULL;
	memset( m_markers, 0, sizeof( m_markers ) );
#ifdef VOICE_CHAT
	m_voiceChannelRequested = false;
	m_voiceChannelId = 0;
	m_voiceMemberCount = 0;
	memset(m_voiceMembersList, 0, sizeof(Player*)*41);
#endif
	m_DisbandOnBGEnd = true;
}

Group::~Group()
{
	ObjectMgr::getSingleton().RemoveGroup(this);
	for( uint32 j = 0; j < 8; ++j ) 
	{
		SubGroup * sub = GetSubGroup( j );
		if( sub )
		{
			delete sub;
			m_SubGroups[j] = NULL;
		}
	}

	RemoveSavedInstances();
	ClearRaidMarkers( MAX_RAID_MARKER_COUNT, true );
}

SubGroup::~SubGroup()
{

}

void SubGroup::RemovePlayer(PlayerInfo * info)
{
	m_GroupMembers.erase(info);
	info->subGroup=-1;
}

bool SubGroup::AddPlayer(PlayerInfo * info)
{
	if(IsFull())
	{ 
		return false;
	}

	info->subGroup=(int8)GetID();
	m_GroupMembers.insert(info);
	return true;
}

bool SubGroup::HasMember(uint32 guid)
{
	for( GroupMembersSet::iterator itr = m_GroupMembers.begin(); itr != m_GroupMembers.end(); ++itr )
		if( (*itr) != NULL )
			if( (*itr)->guid == guid )
			{ 
				return true;
			}

	return false;
}

SubGroup * Group::FindFreeSubGroup()
{
	for(uint32 i = 0; i < m_SubGroupCount; i++)
		if(m_SubGroups[i] && !m_SubGroups[i]->IsFull())
		{ 
			return m_SubGroups[i];
		}

	return NULL;
}

bool Group::AddMember(PlayerInfo * info, int32 subgroupid/* =-1 */)
{
	m_groupLock.Acquire();
	Player * pPlayer = info->m_loggedInPlayer;

	if(m_isqueued)
	{
		m_isqueued=false;
		BattlegroundManager.RemoveGroupFromQueues(this);
	}

	if(!IsFull())
	{
		SubGroup* subgroup = (subgroupid>0) ? m_SubGroups[subgroupid] : FindFreeSubGroup();
		if(subgroup == NULL)
		{
			m_groupLock.Release();
			return false;
		}

		if(subgroup->AddPlayer(info))
		{
            
			if(info->m_Group && info->m_Group != this)
				info->m_Group->RemovePlayer(info);

			if(pPlayer)
				sEventMgr.AddEvent(pPlayer,&Player::EventGroupFullUpdate,EVENT_PLAYER_UPDATE,2500,1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);

			m_dirty=true;
			++m_MemberCount;
			//achievement system
			{
				uint32 i = 0, j = 0;
				SubGroup *sg1 = NULL;
				GroupMembersSet::iterator itr1;
				for( i = 0; i < m_SubGroupCount; i++ )
				{
					sg1 = m_SubGroups[i];
					if( sg1 != NULL)
					{
						for( itr1 = sg1->GetGroupMembersBegin(); itr1 != sg1->GetGroupMembersEnd(); ++itr1 )
						{
							if( (*itr1) == NULL )
								continue;
							if( (*itr1)->m_loggedInPlayer == NULL )
								continue;
							(*itr1)->m_loggedInPlayer->Event_AchiementCriteria_Received(ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_RAID,m_MemberCount,ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_SET_MAX);
						}
					}
				}
			}
			Update();	// Send group update. 

			if(m_Leader==NULL && info->m_loggedInPlayer)
			{
				m_Leader=info;
				m_Leader->m_loggedInPlayer->SetFlag( PLAYER_FLAGS, PLAYER_FLAG_PARTY_LEADER );	//required for wargames
			}

			info->m_Group=this;
			info->subGroup = (int8)subgroup->GetID();

			m_groupLock.Release();
			return true;
		}
		else
		{
			m_groupLock.Release();
			info->m_Group=NULL;
			info->subGroup=-1;
			return false;
		}

	}
	else
	{
		info->m_Group = NULL;
		info->subGroup = -1;
		m_groupLock.Release();
		return false;
	}
}

void Group::SetLeader(Player* pPlayer, bool silent)
{
	if( m_Leader && m_Leader->m_loggedInPlayer )
	{
		m_Leader->m_loggedInPlayer->RemoveFlag( PLAYER_FLAGS, PLAYER_FLAG_PARTY_LEADER );	//required for wargames
	}
	if( pPlayer != NULL )
	{
		m_Leader = pPlayer->m_playerInfo;
		m_Leader->m_loggedInPlayer->SetFlag( PLAYER_FLAGS, PLAYER_FLAG_PARTY_LEADER );	//required for wargames
		m_dirty = true;
		if( !silent )
		{
			WorldPacket data( SMSG_GROUP_SET_LEADER, 100 );
			data << pPlayer->GetName();
			SendPacketToAll( &data );
		}
	}
	Update();
}

void Group::Update()
{
	if( m_updateblock )
	{ 
		return;
	}

	Player* pNewLeader = NULL;

	if( m_Leader == NULL || ( m_Leader != NULL && m_Leader->m_loggedInPlayer == NULL ) )
	{
		pNewLeader = FindFirstPlayer();
		if( pNewLeader != NULL )
		{
			m_Leader = pNewLeader->m_playerInfo;
			m_Leader->m_loggedInPlayer->SetFlag( PLAYER_FLAGS, PLAYER_FLAG_PARTY_LEADER );	//required for wargames
		}
	}

	if( m_Looter != NULL && m_Looter->m_loggedInPlayer == NULL )
	{
		if( pNewLeader == NULL )
			pNewLeader = FindFirstPlayer();
		if( pNewLeader != NULL )
			m_Looter = pNewLeader->m_playerInfo;
	}

	WorldPacket data( 50 + ( m_MemberCount * 20 ) );
	GroupMembersSet::iterator itr1, itr2;

	uint32 i = 0, j = 0;
	uint8 flags;
	SubGroup *sg1 = NULL;
	SubGroup *sg2 = NULL;
	m_groupLock.Acquire();

	for( i = 0; i < m_SubGroupCount; i++ )
	{
		sg1 = m_SubGroups[i];

		if( sg1 != NULL)
		{
			for( itr1 = sg1->GetGroupMembersBegin(); itr1 != sg1->GetGroupMembersEnd(); ++itr1 )
			{
				// should never happen but just in case
				if( (*itr1) == NULL )
					continue;

				/* skip offline players */
				if( (*itr1)->m_loggedInPlayer == NULL )
				{
#ifdef VOICE_CHAT
					if( (*itr1)->groupVoiceId >= 0 )
					{
						// remove from voice members since that player is now offline
						RemoveVoiceMember( (*itr1) );
					}

					continue;
				}

				if( (*itr1)->groupVoiceId < 0 && sVoiceChatHandler.CanUseVoiceChat() )
				{
					(*itr1)->groupVoiceId = 0;
					AddVoiceMember( (*itr1) );
				}
#else
					continue;
				}
#endif
/*
00 party
00 member
00 subg 0
00 ?
E1 B5 66 03 00 00 57 1F ??
02 00 00 00 membercount
01 00 00 00 packet ID
  53 67 63 00 name = Sgc
  CB 26 4B 02 00 00 00 07 guid
  01 online
  00 subg 0
  00 flags
  00 role
07 34 42 02 00 00 00 07 leader guid
03 loot method FFA
00 00 00 00 00 00 00 00 looter
02 treshhold
00 dungeon diff
00 raid diff
00 ?

02 group type raid
00 bg 
00 subgroup
00 ?
FA 73 67 04 00 00 51 1F -> maybe meeting stone guid ? does seem like a guid
21 00 00 00 - serializer
02 00 00 00 - membercount - 1

4B 77 6C 00 - name = Kwl 
09 D5 38 03 00 00 00 01 - guid
01 - online
00 - groupid
00 - flags = role
00 ?
50 61 6C 61 6E 61 6E 61 6E 61 00 -name 2 Palananana 
9E A0 3C 03 00 00 00 01 - guid
01 - online
00 - groupid
00 - flags = role 
00 ?
E9 D7 38 03 00 00 00 01 - leader
03 
00 00 00 00 
00 00 00 00 
02 - loot treshhold
01 - dungeon diff
03 - raid diff
00 ?

14333
03 GROUP_TYPE_BGRAID
01 leader is inside bg ? well it was so this is bad guess
00 subgroupid ?
08 my role DPS
AE D3 76 08 00 00 56 1F guid ? of who ?
14 00 00 00 serializer
09 00 00 00 member count
53 6C 6C 6F 6E 67 00 name
6C 3C A1 04 00 00 80 06 guid
01 online
00 subgroup
00 flags
00 ? 
57 66 63 61 6C 6C 00 09 93 8E 05 00 00 00 06 01 00 00 00 41 65 64 65 61 00 7C F0 8B 05 00 00 00 06 01 00 00 00 52 72 65 61 70 65 72 72 00 66 C8 96 04 00 00 80 03 01 00 00 00 43 72 61 6E 69 75 6D 73 68 6F 74 00 05 02 1B 04 00 00 80 01 01 00 00 00 4C 69 6B 65 61 62 C3 B8 73 73 00 4A EF 35 04 00 00 80 01 01 01 00 00 45 73 65 72 61 00 4D F6 F3 04 00 00 00 06 01 01 00 00 44 65 C3 A5 74 68 63 C3 B8 72 65 00 91 D0 79 04 00 00 80 03 01 01 00 00 46 72 65 65 7A 65 62 69 6F 74 63 68 00 2C 8C 36 04 00 00 80 01 01 01 00 00 6C 3C A1 04 00 00 80 06 03 00 00 00 00 00 00 00 00 02 00 00 

{SERVER} Packet: (0xBEB3) SMSG_GROUP_LIST PacketSize = 28 TimeStamp = 15199130
11 00 00 00 
73 A6 D9 09 00 00 51 1F 
05 00 00 00 
00 00 00 00 
00 00 00 00 00 00 00 00 

{SERVER} Packet: (0xBEB3) SMSG_GROUP_LIST PacketSize = 28 TimeStamp = 15039385
23 00 00 00 
F4 A3 D9 09 00 00 51 1F 
06 00 00 00 
00 00 00 00 
E9 D7 38 03 00 00 00 01 

- arena group
{SERVER} Packet: (0xBEB3) SMSG_GROUP_LIST PacketSize = 58 TimeStamp = 14936112
03 00 00 00 
F4 A3 D9 09 00 00 51 1F 
02 00 00 00 
01 00 00 00 

48 6F 6C 6F 77 00 - name
8F DC 38 03 00 00 00 01 - guid
01 00 00 00 - online / sub group / prefered role / selected role
8F DC 38 03 00 00 00 01 - leader
03 -lootmethod
00 00 00 00 00 00 00 00 - looter
02 00 00 m_LootThreshold / dungeon_difficulty / raid_difficulty / ?

-BG group Horde
{SERVER} Packet: (0xBEB3) SMSG_GROUP_LIST PacketSize = 181 TimeStamp = 2031226
03 01 00 00 
06 76 48 08 00 80 55 1F 
0C 00 00 00 
06 00 00 00 

44 75 64 65 6D 61 6E 67 75 79 00 1F B9 AB 00 00 00 00 05 01 00 00 00 
53 68 6F 63 6B 65 72 72 73 73 00 22 D6 F3 04 00 00 00 05 01 00 00 00 
48 75 72 74 73 61 6C 6F 74 00 2E D0 FB 04 00 00 00 04 01 00 00 00 
43 61 6E 64 79 6C 61 63 65 64 00 8E 24 E7 04 00 00 80 03 01 00 00 00 
55 6E 64 65 64 67 75 79 64 75 64 65 00 73 AE 35 04 00 00 80 01 01 00 00 00 
56 69 74 61 73 6D 65 72 6C 69 6E 69 00 B3 24 E7 04 00 00 80 03 01 01 00 00 
1F B9 AB 00 00 00 00 05 
03 
00 00 00 00 00 00 00 00 
02 00 00 

-BG group Alliance
{SERVER} Packet: (0xBEB3) SMSG_GROUP_LIST PacketSize = 118 TimeStamp = 11482749
03 00 00 00 
18 B1 B5 0B 00 00 53 1F 
08 00 00 00 
04 00 00 00 

4B 6F 64 69 74 68 00 76 E9 61 06 00 00 00 03 01 00 00 00 
57 69 73 73 69 65 00 EB 83 F2 04 00 00 80 03 01 00 00 00 
44 72 75 67 64 65 61 6C 65 72 00 E5 01 9F 04 00 00 00 01 01 00 00 00 
54 7A 79 6B 00 02 A5 63 06 00 00 00 03 01 00 00 00 
76 E9 61 06 00 00 00 03 
03 
00 00 00 00 00 00 00 00 
02 00 00 

*/
				data.Initialize( SMSG_GROUP_LIST );
				data << uint8(m_GroupType);	//0=party,2=raid,3=BG,0x10=null update ?, 0x11=?,0x23=?
				if( m_Leader != NULL && m_Leader->m_loggedInPlayer != NULL && m_Leader->m_loggedInPlayer->IsInBg() )
					data << uint8( m_Leader->m_loggedInPlayer->GetTeam() );   //not valid for arenas ?
				else
					data << uint8( 0 );
				data << uint8( sg1->GetID() );
				data << uint8( (*itr1)->m_loggedInPlayer->GroupPlayerRole );	
				if( m_GroupType & GROUP_TYPE_LFD )
				{
					data << uint8(0);
					data << uint32(0);
				}
				data << m_GUID;
				static uint32 group_packet_serializer = 0;
				data << uint32( group_packet_serializer++ );		// 3.3 this value increments every time SMSG_GROUP_LIST is sent, this is group specific
				data << uint32( m_MemberCount - 1 );	// we don't include self

				for( j = 0; j < m_SubGroupCount; j++ )
				{
					sg2 = m_SubGroups[j];

					if( sg2 != NULL)
					{
						for( itr2 = sg2->GetGroupMembersBegin(); itr2 != sg2->GetGroupMembersEnd(); ++itr2 )
						{
							if( (*itr1) == (*itr2) )
								continue;

							// should never happen but just in case
							if( (*itr2) == NULL )
								continue;

							data << (*itr2)->name << GET_PLAYER_GUID( (*itr2)->guid );	
							
							if( (*itr2)->m_loggedInPlayer != NULL )
								data << uint8( 1 );
							else
								data << uint8( 0 );

							data << uint8( sg2->GetID() );
							
							flags = 0;

							if( (*itr2) == m_assistantLeader )
								flags |= 1;
							if( (*itr2) == m_mainTank )
								flags |= 2;
							if( (*itr2) == m_mainAssist )
								flags |= 4;

							data << flags;
							if( (*itr2)->m_loggedInPlayer )
								data << uint8( (*itr2)->m_loggedInPlayer->GroupPlayerRole );   
							else
								data << uint8( 0 );                             
						}
					}
				}

				if( m_Leader != NULL )
					data << GET_PLAYER_GUID( m_Leader->guid );
				else
					data << uint64( 0 );

				data << uint8( m_LootMethod );

				if( m_Looter != NULL )
					data << GET_PLAYER_GUID( m_Looter->guid );
				else
					data << uint64( 0 );

				data << uint8( m_LootThreshold );
				data << uint8( dungeon_difficulty );
				data << uint8( raid_difficulty );
				data << uint8(0);                               // 3.3

				if( !(*itr1)->m_loggedInPlayer->IsInWorld() )
					(*itr1)->m_loggedInPlayer->CopyAndSendDelayedPacket( &data );
				else
					(*itr1)->m_loggedInPlayer->GetSession()->SendPacket( &data );
			}		
		}
	}

	if( m_dirty )
	{
		m_dirty = false;
		SaveToDB();
	}

	m_groupLock.Release();
}

void Group::Disband()
{
	m_groupLock.Acquire();
	m_updateblock=true;

	if(m_isqueued)
	{
		m_isqueued=false;
		WorldPacket * data = sChatHandler.FillSystemMessageData("A change was made to your group. Removing the arena queue.");
		SendPacketToAll(data);
		delete data;
		data = NULL;

		BattlegroundManager.RemoveGroupFromQueues(this);
	}

	uint32 i = 0;
	for(i = 0; i < m_SubGroupCount; i++)
	{
		SubGroup *sg = m_SubGroups[i];
		sg->Disband();
	}

	m_groupLock.Release();
	CharacterDatabase.Execute("DELETE FROM groups WHERE group_id = %u", m_Id);
	sInstanceMgr.OnGroupDestruction(this);
	delete this;	// destroy ourselves, the destructor removes from eventmgr and objectmgr.
}

void SubGroup::Disband()
{
	sStackWorldPacket( data, SMSG_GROUP_DESTROYED, 5 );
	WorldPacket data2( SMSG_PARTY_COMMAND_RESULT, 20);
	data2 << uint32(2) << uint8(0) ;
	data2 << uint32(m_Parent == NULL ? 0 : m_Parent->dungeon_difficulty) << uint32( 0 );	// you leave the group
	data2 << uint32(m_Parent == NULL ? 0 : m_Parent->raid_difficulty)<< uint32( 0 );

	GroupMembersSet::iterator itr = m_GroupMembers.begin();
//	GroupMembersSet::iterator it2;
	for(; itr != m_GroupMembers.end();)
	{
		if( (*itr) != NULL )
		{
			if( (*itr)->m_loggedInPlayer )
			{
				if( (*itr)->m_loggedInPlayer->GetSession() != NULL )
				{
					data2.put(5, uint32((*itr)->m_loggedInPlayer->dungeon_difficulty));
					data2.put(9, uint32((*itr)->m_loggedInPlayer->raid_difficulty));
					(*itr)->m_loggedInPlayer->GetSession()->SendPacket(&data2);
					(*itr)->m_loggedInPlayer->GetSession()->SendPacket(&data);
          (*itr)->m_Group->SendNullUpdate( (*itr)->m_loggedInPlayer ); // cebernic: panel refresh.
					
				}
			}

			(*itr)->m_Group = NULL;
			(*itr)->subGroup = -1;
		}

		m_Parent->m_MemberCount--;
//		it2 = itr;
		++itr;

//		m_GroupMembers.erase(it2);
	}
	m_GroupMembers.clear();

	m_Parent->m_SubGroups[m_Id] = NULL;
	delete this;
}

Player* Group::FindFirstPlayer()
{
	GroupMembersSet::iterator itr;
	m_groupLock.Acquire();

	for( uint32 i = 0; i < m_SubGroupCount; i++ )
	{
		if( m_SubGroups[i] != NULL )
		{
			for( itr = m_SubGroups[i]->GetGroupMembersBegin(); itr != m_SubGroups[i]->GetGroupMembersEnd(); ++itr )
			{
				if( (*itr) != NULL )
				{
					if( (*itr)->m_loggedInPlayer != NULL )
					{
						m_groupLock.Release();
						return (*itr)->m_loggedInPlayer;
					}
				}
			}
		}
	}

	m_groupLock.Release();
	return NULL;
}

void Group::RemovePlayer(PlayerInfo * info, bool DoNotKickFromBG)
{
	WorldPacket data(50);
	Player * pPlayer = info->m_loggedInPlayer;

	m_groupLock.Acquire();
	if(m_isqueued)
	{
		m_isqueued=false;
		BattlegroundManager.RemoveGroupFromQueues(this);
	}
	
	SubGroup *sg=NULL;
	if(info->subGroup >= 0 && info->subGroup <= 8)
		sg = m_SubGroups[info->subGroup];

	if(sg == NULL || sg->m_GroupMembers.find(info) == sg->m_GroupMembers.end())
	{
		for(uint32 i = 0; i < m_SubGroupCount; ++i)
		{
			if(m_SubGroups[i] != NULL)
			{
				if(m_SubGroups[i]->m_GroupMembers.find(info) != m_SubGroups[i]->m_GroupMembers.end())
				{
					sg = m_SubGroups[i];
					break;
				}
			}
		}
	}

	info->m_Group=NULL;
	info->subGroup=-1;
#ifdef VOICE_CHAT
	if( info->groupVoiceId <= 0 )
		RemoveVoiceMember(info);
#endif

	if(sg==NULL)
	{
		m_groupLock.Release();
		return;
	}

	m_dirty=true;
	sg->RemovePlayer(info);
	--m_MemberCount;

	if( info->m_loggedInPlayer != NULL )
	{
		sInstanceMgr.PlayerLeftGroup( this, info->m_loggedInPlayer );
		if( DoNotKickFromBG == false && info->m_loggedInPlayer->m_bg && BattlegroundManager.IsValidBGPointer( info->m_loggedInPlayer->m_bg ) )
			info->m_loggedInPlayer->m_bg->RemovePlayer( info->m_loggedInPlayer, false );
	}

	if( pPlayer != NULL )
	{
		if( pPlayer->GetSession() != NULL )
		{
			SendNullUpdate( pPlayer );

			data.SetOpcode( SMSG_GROUP_DESTROYED );
			pPlayer->GetSession()->SendPacket( &data );
/*
14333
{SERVER} Packet: (0xF2EF) SMSG_PARTY_COMMAND_RESULT PacketSize = 26 TimeStamp = 14961400
02 00 00 00 
55 64 72 65 61 00 
00 00 00 00 00 00 00 00 dungeon_difficulty
00 00 00 00 00 00 00 00 raid_difficulty
*/
			data.Initialize( SMSG_PARTY_COMMAND_RESULT );
			data << uint32(2) << uint8(0) << uint32(0) << uint32(0) << uint32(0) << uint32(0) ;  // you leave the group
			pPlayer->GetSession()->SendPacket( &data );
		}

		//Remove some party auras.
		for (uint32 i=0;i<MAX_POSITIVE_AURAS1(pPlayer);i++)
		{
			if (pPlayer->m_auras[i] && 
				pPlayer->m_auras[i]->IsAreaAura() && 
				pPlayer->m_auras[i]->GetUnitCaster() &&
				(!pPlayer->m_auras[i]->GetUnitCaster() ||(pPlayer->m_auras[i]->GetUnitCaster()->IsPlayer() && pPlayer!=pPlayer->m_auras[i]->GetUnitCaster())))
				pPlayer->m_auras[i]->Remove();
		}
	}

	if(m_MemberCount < 2)
	{
		if(m_disbandOnNoMembers)
		{
			m_groupLock.Release();
			Disband();
			return;
		}
	}

	// eek! ;P 
	Player *newPlayer = NULL;
	if(m_Looter == info)
	{
		newPlayer = FindFirstPlayer();
		if( newPlayer != NULL )
            m_Looter = newPlayer->m_playerInfo;
		else
			m_Looter = NULL;
	}

	if(m_Leader == info)
	{
		if( newPlayer==NULL )
			newPlayer=FindFirstPlayer();

		if( newPlayer != NULL )
			SetLeader(newPlayer, false);
		else
			m_Leader = NULL;
	}

	Update();
	m_groupLock.Release();
}

void Group::ExpandToRaid( bool IsBGRaid )
{
	if( m_GroupType & GROUP_TYPE_RAID )
		return;

	if(m_isqueued)
	{
		m_isqueued=false;
		WorldPacket * data = sChatHandler.FillSystemMessageData("A change was made to your group. Removing the arena queue.");
		SendPacketToAll(data);
		delete data;
		data = NULL;

		BattlegroundManager.RemoveGroupFromQueues(this);
	}
	// Very simple ;)

	uint32 i;
	m_groupLock.Acquire();
	m_SubGroupCount = 8;

	for( i = 1; i < m_SubGroupCount; i++)
		if( m_SubGroups[i] == NULL )
			m_SubGroups[i] = new SubGroup(this, i);

	m_GroupType = GROUP_TYPE_RAID;
	if( IsBGRaid )
		m_GroupType |= GROUP_TYPE_BG;
	m_dirty=true;
	Update();
	m_groupLock.Release();
}

void Group::ShrinkToParty( )
{
	if( m_GroupType == GROUP_TYPE_PARTY )
		return;

	if( MemberCount() > MAX_GROUP_SIZE_PARTY )
		return;

	if(m_isqueued)
	{
		m_isqueued=false;
		WorldPacket * data = sChatHandler.FillSystemMessageData("A change was made to your group. Removing the arena queue.");
		SendPacketToAll(data);
		delete data;
		data = NULL;

		BattlegroundManager.RemoveGroupFromQueues(this);
	}

	uint32 i;
	m_groupLock.Acquire();

	for( i = 1; i < m_SubGroupCount; i++)
		if( m_SubGroups[i] != NULL )
		{
			GroupMembersSet::iterator itr,itr2;
			for(itr = m_SubGroups[i]->GetGroupMembersBegin(); itr != m_SubGroups[i]->GetGroupMembersEnd();)
			{
				itr2 = itr;
				itr++;
				if( (*itr2)->m_loggedInPlayer != NULL )
					MovePlayer( (*itr2), 0 );
			}
		}
	m_SubGroupCount = 1;

	m_GroupType &= ~( GROUP_TYPE_RAID | GROUP_TYPE_BG );
	m_GroupType = GROUP_TYPE_PARTY;
	m_dirty=true;
	Update();
	m_groupLock.Release();
}

uint32 Group::MemberCountOnline()
{
	GroupMembersSet::iterator itr;
	uint32 Counter = 0;

	m_groupLock.Acquire();
	for( uint32 i = 0; i < m_SubGroupCount; i++ )
	{
		if( m_SubGroups[i] != NULL )
		{
			for( itr = m_SubGroups[i]->GetGroupMembersBegin(); itr != m_SubGroups[i]->GetGroupMembersEnd(); ++itr)
				if( (*itr)->m_loggedInPlayer != NULL )
					Counter++;
		}
	}
	m_groupLock.Release();

	return Counter;
}

void Group::SetLooter(Player *pPlayer, uint8 method, uint16 threshold)
{ 
	if( pPlayer != NULL )
	{
		m_LootMethod = method;
		m_Looter = pPlayer->m_playerInfo;
		m_LootThreshold  = threshold;
		m_dirty = true;
	}
	Update();
}

void Group::SendPacketToAllButOne(WorldPacket *packet, Player *pSkipTarget)
{
	GroupMembersSet::iterator itr;
	uint32 i = 0;
	m_groupLock.Acquire();
	for(; i < m_SubGroupCount; i++)
	{
		if( m_SubGroups[i] )
			for(itr = m_SubGroups[i]->GetGroupMembersBegin(); itr != m_SubGroups[i]->GetGroupMembersEnd(); ++itr)
				if((*itr)->m_loggedInPlayer != NULL && (*itr)->m_loggedInPlayer != pSkipTarget)
					(*itr)->m_loggedInPlayer->GetSession()->SendPacket(packet);
	}
	
	m_groupLock.Release();
}

void Group::OutPacketToAllButOne(uint16 op, uint16 len, const void* data, Player *pSkipTarget)
{
	GroupMembersSet::iterator itr;
	uint32 i = 0;
	m_groupLock.Acquire();
	for(; i < m_SubGroupCount; i++)
	{
		for(itr = m_SubGroups[i]->GetGroupMembersBegin(); itr != m_SubGroups[i]->GetGroupMembersEnd(); ++itr)
		{
			if((*itr)->m_loggedInPlayer != NULL && (*itr)->m_loggedInPlayer != pSkipTarget)
				(*itr)->m_loggedInPlayer->GetSession()->OutPacket( op, len, data );
		}
	}

	m_groupLock.Release();
}

bool Group::HasMember(Player * pPlayer)
{
	if( !pPlayer )
	{ 
		return false;
	}

	GroupMembersSet::iterator itr;
	m_groupLock.Acquire();

	for( uint32 i = 0; i < m_SubGroupCount; i++ )
	{
		if( m_SubGroups[i] != NULL )
		{
			if( m_SubGroups[i]->m_GroupMembers.find( pPlayer->m_playerInfo ) != m_SubGroups[i]->m_GroupMembers.end() )
			{
				m_groupLock.Release();
				return true;
			}
		}
	}

	m_groupLock.Release();
	return false;
}

bool Group::HasMember(PlayerInfo * info)
{
	GroupMembersSet::iterator itr;
	uint32 i = 0;

	m_groupLock.Acquire();

	for(; i < m_SubGroupCount; i++)
	{
		if(m_SubGroups[i]->m_GroupMembers.find(info) != m_SubGroups[i]->m_GroupMembers.end())
		{
			m_groupLock.Release();
			return true;
		}
	}

	m_groupLock.Release();
	return false;
}

void Group::MovePlayer(PlayerInfo *info, uint8 subgroup)
{
	if( subgroup >= m_SubGroupCount )
	{ 
		return;
	}

	if(m_SubGroups[subgroup]->IsFull())
	{ 
		return;
	}

	m_groupLock.Acquire();
	SubGroup *sg=NULL;

	if(info->subGroup > 0 && info->subGroup <= 8)
		sg = m_SubGroups[info->subGroup];

	if(sg == NULL || sg->m_GroupMembers.find(info) == sg->m_GroupMembers.end())
	{
		for(uint32 i = 0; i < m_SubGroupCount; ++i)
		{
			if(m_SubGroups[i] != NULL)
			{
				if(m_SubGroups[i]->m_GroupMembers.find(info) != m_SubGroups[i]->m_GroupMembers.end())
				{
					sg = m_SubGroups[i];
					break;
				}
			}
		}
	}

	if(!sg)
	{
		m_groupLock.Release();
		return;
	}
	
	sg->RemovePlayer(info);
    
	// Grab the new group, and insert
	sg = m_SubGroups[subgroup];
	if(!sg->AddPlayer(info))
	{
		RemovePlayer(info);
		info->m_Group=NULL;
	}
	else
	{
		info->subGroup=(int8)sg->GetID();
		info->m_Group=this;
	}

	Update();
	m_groupLock.Release();
}

void Group::SendNullUpdate( Player *pPlayer )
{
	// this packet is 24 bytes long.		// AS OF 2.1.0
	uint8 buffer[28];
	memset(buffer, 0, 28);
	buffer[0] = 0x10;
	pPlayer->GetSession()->OutPacket( SMSG_GROUP_LIST, 28, buffer );
}

// player is object class becouse its called from unit class
void Group::SendPartyKillLog( Object * player, Object * Unit )
{
	if( !player || !Unit || !HasMember( SafePlayerCast( player ) ) )
		return;

	WorldPacket data( SMSG_PARTYKILLLOG, 25 );
	data << player->GetGUID();
	data << Unit->GetGUID();
	SendPacketToAll( &data );
}

uint32 Group::GetInstanceDifficulty(MapInfo *new_instance)
{
/*	if( new_instance )
	{
	}
	else*/
	{
		if( GetGroupType() != GROUP_TYPE_RAID )
			return dungeon_difficulty;
		else
			return raid_difficulty+INSTANCE_MODE_RAID_10_MAN;
	}
}

void Group::LoadFromDB(Field *fields)
{
#define LOAD_ASSISTANT(__i, __d) g = fields[__i].GetUInt32(); if(g != 0) { __d = objmgr.GetPlayerInfo(g); }

	uint32 g;
	m_updateblock=true;
	m_Id = fields[0].GetUInt32();

	ObjectMgr::getSingleton().AddGroup( this );

	m_GroupType = fields[1].GetUInt8();
	m_SubGroupCount = fields[2].GetUInt8();
	m_LootMethod = fields[3].GetUInt8();
	m_LootThreshold = fields[4].GetUInt8();
	uint32 instance_difficulty = fields[5].GetUInt32();
	dungeon_difficulty = instance_difficulty & 0x0F;
	if( dungeon_difficulty >= TOTAL_DUNGEON_DIFFICULTIES )
		dungeon_difficulty = DUNGEON_DIFFICULTY_NORMAL;
	raid_difficulty = ( instance_difficulty >> 4 )& 0x0F;
	if( dungeon_difficulty >= TOTAL_RAID_DIFFICULTIES )
		dungeon_difficulty = RAID_DIFFICULTY_10MAN_NORMAL;

	LOAD_ASSISTANT(6, m_assistantLeader);
	LOAD_ASSISTANT(7, m_mainTank);
	LOAD_ASSISTANT(8, m_mainAssist);

	// create groups
	for(int i = 1; i < m_SubGroupCount; ++i)
		if( m_SubGroups[i] == NULL )
			m_SubGroups[i] = new SubGroup(this, i);

	// assign players into groups
	for(int i = 0; i < m_SubGroupCount; ++i)
	{
		for(int j = 0; j < 5; ++j)
		{
			uint32 guid = fields[9 + (i*5) + j].GetUInt32();
			if( guid == 0 )
				continue;

			PlayerInfo * inf = objmgr.GetPlayerInfo(guid);
			if(inf == NULL)
				continue;

			AddMember(inf);
			m_dirty=false;
		}
	}

	char *ids = strdup_local(fields[50].GetString());
	char *q = ids;
	char *p = strchr(q, ' ');
	while(p)
	{
		char *r = strchr(q, ':');
		if(r == NULL || r > p)
			continue;
		*p = 0;
		*r = 0;
		char *s = strchr(r+1, ':');
		if(s == NULL || s > p)
			continue;
		*s = 0;
		uint32 mapId = atoi(q);
		uint32 mode = atoi(r+1);
		uint32 instanceId = atoi(s+1);

		if(mapId >= NUM_MAPS)
			continue;

		AddSavedInstance(mapId,mode,instanceId);

		q = p+1;
		p = strchr(q, ' ');
	}
	free(ids);

	m_updateblock=false;
}

void Group::SaveToDB()
{
	if(!m_disbandOnNoMembers)	/* don't save bg groups */
	{ 
		return;
	}

	std::stringstream ss;
	//uint32 i = 0;
	uint32 fillers = 8 - m_SubGroupCount;
	uint32 instance_difficulty = dungeon_difficulty | ( raid_difficulty << 4 );

	ss << "REPLACE INTO groups VALUES("
		<< m_Id << ","
		<< uint32(m_GroupType) << ","
		<< uint32(m_SubGroupCount) << ","
		<< uint32(m_LootMethod) << ","
		<< uint32(m_LootThreshold) << ","
		<< uint32(instance_difficulty) << ",";

	if(m_assistantLeader)
		ss << m_assistantLeader->guid << ",";
	else
		ss << "0,";
	
	if(m_mainTank)
		ss << m_mainTank->guid << ",";
	else
		ss << "0,";

	if(m_mainAssist)
		ss << m_mainAssist->guid << ",";
	else
		ss << "0,";

	for(uint32 i = 0; i < m_SubGroupCount; ++i)
	{
		uint32 j = 0;
		for(GroupMembersSet::iterator itr = m_SubGroups[i]->GetGroupMembersBegin(); j<5 && itr != m_SubGroups[i]->GetGroupMembersEnd(); ++j, ++itr)
		{
			ss << (*itr)->guid << ",";
		}

		for(; j < 5; ++j)
			ss << "0,";
	}

	for(uint32 i = 0; i < fillers; ++i)
		ss << "0,0,0,0,0,";

	ss << (uint32)UNIXTIME << ",'";
	SimplePointerListNode<active_instance> *ai_itr;
	for( ai_itr = m_active_instances.begin(); ai_itr != m_active_instances.end(); ai_itr = ai_itr->Next() )
		ss << ai_itr->data->map_id << ":" << ai_itr->data->difficulty << ":" << ai_itr->data->difficulty << " ";
	ss << "')";
	/*printf("==%s==\n", ss.str().c_str());*/
	CharacterDatabase.Execute(ss.str().c_str());
}

void Group::UpdateOutOfRangePlayer(Player * pPlayer, uint32 Flags, bool Distribute, WorldPacket * Packet,bool full_update)
{
	/*
	3.3.5 - SMSG_PARTY_MEMBER_STATS_FULL
00 ?
8F FC C3 38 03 01 guid
EF 1B F4 7F flags 
01 00 GROUP_UPDATE_FLAG_ONLINE
00 A2 00 00  GROUP_UPDATE_FLAG_HEALTH
3F AD 00 00 GROUP_UPDATE_FLAG_MAXHEALTH
01 GROUP_UPDATE_FLAG_POWER_TYPE
E8 03 GROUP_UPDATE_FLAG_MAXPOWER
50 GROUP_UPDATE_FLAG_LEVEL
00 65 GROUP_UPDATE_FLAG_ZONEID
06 27 GROUP_UPDATE_FLAG_POSITION
06 C2 GROUP_UPDATE_FLAG_POSITION
EE FF FF 01 00 00 00 00 GROUP_UPDATE_FLAG_PLAYER_AURAS -> auramask  24 * 5 bytes ?
00 47 00 00 00 19 D7 7D 00 00 09 B5 14 00 00 09 7A 31 00 00 0F B4 31 00 00 0B FF 31 00 00
0B 9E 32 00 00 0B 9B E0 00 00 0F 69 B7 00 00 0B 9A 73 00 00 0B 43 32 00 00 09 45 08 01 00 
09 FA E5 00 00 09 DF 1A 01 00 09 07 F0 00 00 1B 06 B6 00 00 11 F0 EF 00 00 15 00 F9 F7 00 
00 00 00 00 00 00 00 
14333
{SERVER} Packet: (0x2ABA) SMSG_PARTY_MEMBER_STATS_FULL PacketSize = 163 TimeStamp = 10168051
00 full update ?
CF F9 2B 36 04 80 01 guid
FF FF EF 7F mask
03 00 GROUP_UPDATE_FLAG_ONLINE
2A 04 00 00 GROUP_UPDATE_FLAG_HEALTH
2A 04 00 00 GROUP_UPDATE_FLAG_MAXHEALTH
02 GROUP_UPDATE_FLAG_POWER_TYPE
64 00 GROUP_UPDATE_FLAG_POWER
64 00 GROUP_UPDATE_FLAG_MAXPOWER
18 00 GROUP_UPDATE_FLAG_LEVEL
5D 0D GROUP_UPDATE_FLAG_ZONEID - areatable.dbc ID
F7 02 
D7 03 GROUP_UPDATE_FLAG_POSITION
12 04 GROUP_UPDATE_FLAG_POSITION
D5 FF GROUP_UPDATE_FLAG_POSITION
{ GROUP_UPDATE_FLAG_PLAYER_AURAS
	01 - clear old auras
	FF FF FF FF FF FF FF FF - aura mask
	04 00 00 00 - values count ?
	6D 02 01 00 01 00 
	D1 5F 00 00 03 00 
	D7 34 01 00 73 00 
	05 00 00 00 0C 00 
	00 00 00 00 00 00 
	01 35 01 00 71 00 VISUALAURA_FLAG_HAS_DURATION | VISUALAURA_FLAG_SEND_VALUES
	12 00 00 00 
	00 00 00 00 
	00 00 00 00 
}
03 00 00 99 5A B1 40 F1 GROUP_UPDATE_FLAG_PET_GUID
43 61 72 72 69 6F 6E 20 42 69 72 64 00 GROUP_UPDATE_FLAG_PET_NAME
CC 04 GROUP_UPDATE_FLAG_PET_DISPLAYID
5B 03 00 00 GROUP_UPDATE_FLAG_PET_HEALTH
6F 03 00 00 GROUP_UPDATE_FLAG_PET_MAXHEALTH
02 GROUP_UPDATE_FLAG_PET_POWER_TYPE
64 00 GROUP_UPDATE_FLAG_PET_POWER
64 00 GROUP_UPDATE_FLAG_PET_MAXPOWER
{ GROUP_UPDATE_FLAG_PET_AURAS
	01 
	FF FF FF FF FF FF FF FF 
	01 00 00 00 
	D1 5F 00 00 1B 00 
}
08 00 00 00 
00 00 00 00 GROUP_UPDATE_FLAG_VEHICLE_SEAT
*/
	uint16 member_flags = 0x01;
	WorldPacket * data = Packet;
	if(!Packet)
		data = new WorldPacket(SMSG_PARTY_MEMBER_STATS, 500);

	if( full_update )
		data->Initialize(SMSG_PARTY_MEMBER_STATS_FULL);
	else
		data->Initialize(SMSG_PARTY_MEMBER_STATS);

//	if(pPlayer->GetPowerType() != POWER_TYPE_MANA )
//		Flags |= GROUP_UPDATE_FLAG_POWER_TYPE;

	/*Flags |= GROUP_UPDATE_FLAG_PET_NAME;
	Flags |= GROUP_UPDATE_FLAG_PET_UNK_1;*/

	//this is just a test ! Might need to remove it later !
	//players started reporting that when they die they repawn half way alive. Like ghost aura is missing
	if( pPlayer->isAlive() == false )
		Flags &= ~GROUP_UPDATE_FLAG_PLAYER_AURAS;

	if( full_update )
		*data << uint8(0);
	*data << pPlayer->GetNewGUID();
	*data << Flags;

	if(Flags & GROUP_UPDATE_FLAG_ONLINE)
	{
		if(pPlayer->IsPvPFlagged())
			member_flags |= 0x02;
		if(pPlayer->getDeathState() == CORPSE)
			member_flags |= 0x08;
		else if(pPlayer->IsDead())
			member_flags |= 0x10;

		*data << member_flags;
	}

	if(Flags & GROUP_UPDATE_FLAG_HEALTH)
		*data << uint32(pPlayer->GetUInt32Value(UNIT_FIELD_HEALTH));

	if(Flags & GROUP_UPDATE_FLAG_MAXHEALTH)
		*data << uint32(pPlayer->GetUInt32Value(UNIT_FIELD_MAXHEALTH));

	if(Flags & GROUP_UPDATE_FLAG_POWER_TYPE)
		*data << uint8(pPlayer->GetPowerType());

	if(Flags & GROUP_UPDATE_FLAG_POWER)
		*data << uint16(pPlayer->GetPower( pPlayer->GetPowerType() ) );

	if(Flags & GROUP_UPDATE_FLAG_MAXPOWER)
		*data << uint16(pPlayer->GetMaxPower( pPlayer->GetPowerType() ) );

	if(Flags & GROUP_UPDATE_FLAG_LEVEL)
		*data << uint16( pPlayer->getLevel() );

	if(Flags & GROUP_UPDATE_FLAG_ZONEID)
		*data << uint16( pPlayer->GetAreaID() );

	if(Flags & GROUP_UPDATE_FLAG_UNK)
		*data << uint16( 0 ); // ?? not 0 seems to have from huge to small values : ex : (31205 for 5170)

	if(Flags & GROUP_UPDATE_FLAG_POSITION)
	{
		//these are defenetly not int16
		*data << int16(pPlayer->GetPositionX()) << int16(pPlayer->GetPositionY()) << int16(pPlayer->GetPositionZ());			// wtf packed floats? O.o
		pPlayer->m_last_group_position = pPlayer->GetPosition();
	}

	if(Flags & GROUP_UPDATE_FLAG_PLAYER_AURAS)
	{
		*data << uint8(0); // if true client clears auras that are not covered by auramask
		*data << (uint32)(0xFFFFFFFF); //auramask, blizz sent this crap
		*data << (uint32)(0xFFFFFFFF); //auramask, blizz sent this crap
		uint32 counter_pos = (uint32)data->wpos();
		uint32 counter = 0;
		*data << uint32(counter);	  //number of structs client will read
		for(uint32 i = POS_AURAS_START; i < MIN( 32, MAX_POSITIVE_AURAS1( pPlayer ) ); ++i)
			if( pPlayer->m_auras[i] )
			{
				*data << (uint32) pPlayer->m_auras[i]->GetSpellId();
				*data << (uint16) (VISUALAURA_FLAG_SELF_CAST|VISUALAURA_FLAG_VISIBLE_1|VISUALAURA_FLAG_VISIBLE_2|VISUALAURA_FLAG_VISIBLE_3); //flags - !based on flags values might be needed to be sent
				counter++;
			}
		for(uint32 i = NEG_AURAS_START; i < MIN( 32, MAX_NEGATIVE_AURAS1( pPlayer ) ); ++i)
			if( pPlayer->m_auras[i] )
			{
				*data << (uint32) pPlayer->m_auras[i]->GetSpellId();
				*data << (uint16) (VISUALAURA_FLAG_NEGATIVE|VISUALAURA_FLAG_SELF_CAST|VISUALAURA_FLAG_VISIBLE_1|VISUALAURA_FLAG_VISIBLE_2|VISUALAURA_FLAG_VISIBLE_3); //flags - !based on flags values might be needed to be sent
				counter++;
			}
		data->put( counter_pos, counter );	
	}

	if(Flags & GROUP_UPDATE_FLAG_PET_GUID)
	{
		if( pPlayer->GetSummon() )
			*data << pPlayer->GetSummon()->GetGUID();
		else
			*data << uint64( 0 );
	}

	if(Flags & GROUP_UPDATE_FLAG_PET_NAME)
	{
		if( pPlayer->GetSummon() )
			*data << pPlayer->GetSummon()->GetName()->c_str();
		else
			*data << "";
	}

	if(Flags & GROUP_UPDATE_FLAG_PET_DISPLAYID)
	{
		if( pPlayer->GetSummon() )
			*data << uint16( pPlayer->GetSummon()->GetUInt32Value( UNIT_FIELD_DISPLAYID) );
		else
			*data << uint16( 0 );
	}

	if(Flags & GROUP_UPDATE_FLAG_PET_HEALTH)
	{
		if( pPlayer->GetSummon() )
			*data << uint32( pPlayer->GetSummon()->GetHealth() );
		else
			*data << uint32( 0 );
	}

	if(Flags & GROUP_UPDATE_FLAG_PET_MAXHEALTH)
	{
		if( pPlayer->GetSummon() )
			*data << uint32( pPlayer->GetSummon()->GetMaxHealth() );
		else
			*data << uint32( 0 );
	}

	if(Flags & GROUP_UPDATE_FLAG_PET_POWER_TYPE)
	{
		if( pPlayer->GetSummon() )
			*data << uint8( pPlayer->GetSummon()->GetPowerType() );
		else
			*data << uint8( POWER_TYPE_FOCUS );
	}

	if(Flags & GROUP_UPDATE_FLAG_PET_POWER)
	{
		if( pPlayer->GetSummon() )
			*data << uint16( pPlayer->GetSummon()->GetPower( pPlayer->GetSummon()->GetPowerType() ) );
		else
			*data << uint16( 0 );
	}

	if(Flags & GROUP_UPDATE_FLAG_PET_MAXPOWER)
	{
		if( pPlayer->GetSummon() )
			*data << uint16( pPlayer->GetSummon()->GetMaxPower( pPlayer->GetSummon()->GetPowerType() ) );
		else
			*data << uint16( 0 );
	}

	if(Flags & GROUP_UPDATE_FLAG_VEHICLE_SEAT)
		*data << uint32( pPlayer->GetCurrentSeatEntry() );
/*
	if(Flags & GROUP_UPDATE_TYPE_FULL_REQUEST_REPLY)
	{
		*data << uint64(0xFF00000000000000ULL);
		*data << uint8(0);
		*data << uint64(0xFF00000000000000ULL);
	}*/

	if(Distribute&&pPlayer->IsInWorld())
	{
		Player * plr;
		float dist = pPlayer->GetMapMgr()->m_UpdateDistance;
		m_groupLock.Acquire();
		for(uint32 i = 0; i < m_SubGroupCount; ++i)
		{
			if(m_SubGroups[i]==NULL)
				continue;

			for(GroupMembersSet::iterator itr = m_SubGroups[i]->GetGroupMembersBegin(); itr != m_SubGroups[i]->GetGroupMembersEnd();)
			{
				plr = (*itr)->m_loggedInPlayer;
				++itr;

				if(plr && plr != pPlayer)
				{
					if(plr->GetDistanceSq(pPlayer) > dist)
						plr->GetSession()->SendPacket(data);
				}
			}
		}
		m_groupLock.Release();
	}

	if(!Packet)
	{
		delete data;
		data = NULL;
	}
}

void Group::UpdateAllOutOfRangePlayersFor(Player * pPlayer)
{
	WorldPacket data(150);
	WorldPacket data2(150);

	if(m_SubGroupCount>8)
	{ 
		return;
	}

	/* tell the other players about us */
	UpdateOutOfRangePlayer(pPlayer, GROUP_UPDATE_TYPE_FULL_CREATE, true, &data2);

	/* tell us any other players we don't know about */
	Player * plr;
	bool u1, u2;
	UpdateMask myMask;
	myMask.SetCount(PLAYER_END);
	UpdateMask hisMask;
	hisMask.SetCount(PLAYER_END);

	m_groupLock.Acquire();
	for(uint32 i = 0; i < m_SubGroupCount; ++i)
	{
		if(m_SubGroups[i]==NULL)
			continue;

		for(GroupMembersSet::iterator itr = m_SubGroups[i]->GetGroupMembersBegin(); itr != m_SubGroups[i]->GetGroupMembersEnd(); ++itr)
		{
			plr = (*itr)->m_loggedInPlayer;
			if(!plr || plr == pPlayer) continue;

			if(!plr->IsVisible(pPlayer))
			{
				UpdateOutOfRangePlayer(plr, GROUP_UPDATE_TYPE_FULL_CREATE, false, &data);
				pPlayer->GetSession()->SendPacket(&data);
			}
			else
			{
				if(pPlayer->GetSubGroup() == plr->GetSubGroup())
				{
					/* distribute quest fields to other players */
					hisMask.Clear();
					myMask.Clear();
					u1 = u2 = false;
					for(uint32 i = PLAYER_QUEST_LOG_1_1; i < PLAYER_QUEST_LOG_25_1; ++i)
					{
						if(plr->GetUInt32Value(i))
						{
							hisMask.SetBit(i);
							u1 = true;
						}

						if(pPlayer->GetUInt32Value(i))
						{
							u2 = true;
							myMask.SetBit(i);
						}
					}

					if(u1)
					{
						data.clear();
                        plr->BuildValuesUpdateBlockForPlayer(&data, &hisMask);
						pPlayer->PushUpdateData(&data, 1);
					}

					if(u2)
					{
						data.clear();
						pPlayer->BuildValuesUpdateBlockForPlayer(&data, &myMask);
						plr->PushUpdateData(&data, 1);
					}
				}
			}
		}
	}

	m_groupLock.Release();
}

void Group::HandleUpdateFieldChange(uint32 Index, Player * pPlayer)
{
	uint32 Flags = 0;
	m_groupLock.Acquire();
	switch(Index)
	{
	case UNIT_FIELD_HEALTH:
		Flags = GROUP_UPDATE_FLAG_HEALTH;
		break;
		
	case UNIT_FIELD_MAXHEALTH:
		Flags = GROUP_UPDATE_FLAG_MAXHEALTH;
		break;

	case UNIT_FIELD_POWER1:
	case UNIT_FIELD_POWER2:
	case UNIT_FIELD_POWER3:
	case UNIT_FIELD_POWER4:
		Flags = GROUP_UPDATE_FLAG_POWER;
		break;

	case UNIT_FIELD_MAXPOWER1:
	case UNIT_FIELD_MAXPOWER2:
	case UNIT_FIELD_MAXPOWER3:
	case UNIT_FIELD_MAXPOWER4:
		Flags = GROUP_UPDATE_FLAG_MAXPOWER;
		break;

	case UNIT_FIELD_LEVEL:
		Flags = GROUP_UPDATE_FLAG_LEVEL;
		break;
	default:
		break;
	}

	if( pPlayer->GetVehicle() != NULL )
		Flags |= GROUP_UPDATE_FLAG_VEHICLE_SEAT;

	if( Flags != 0 )
		UpdateOutOfRangePlayer( pPlayer, Flags, true, 0 );

	m_groupLock.Release();
}

void Group::HandlePartialChange(uint32 Type, Player * pPlayer)
{
	m_groupLock.Acquire();

	if( Type & (GROUP_UPDATE_FLAG_POSITION | GROUP_UPDATE_FLAG_ZONEID) )
		UpdateOutOfRangePlayer(pPlayer, Type, true, 0);

	m_groupLock.Release();
}

void WorldSession::HandlePartyMemberStatsOpcode(WorldPacket & recv_data)
{
	if(!_player->IsInWorld())
	{ 
		return;
	}

	uint64 guid;
	recv_data >> guid;

	Player * plr = _player->GetMapMgr()->GetPlayer( guid );

	if(!_player->GetGroup() || !plr)
	{ 
		sLog.outDebug("!Warning : Client is asking for non existing player for party stats. Outdated packet ? ");
		return;
	}

	WorldPacket data(200);
	if(!_player->GetGroup()->HasMember(plr))
	{
		sLog.outDebug("!Warning : Client is asking for non existing player for party stats. Outdated packet ? ");
		return;			// invalid player
	}

/*	if(_player->IsVisible(plr))
	{ 
		sLog.outDebug("!Warning : Client is asking for for full party stats of player not visible ? ");
		return;
	} */

//	_player->GetGroup()->UpdateOutOfRangePlayer(plr, GROUP_UPDATE_TYPE_FULL_CREATE | GROUP_UPDATE_TYPE_FULL_REQUEST_REPLY, false, &data);
	_player->GetGroup()->UpdateOutOfRangePlayer(plr, GROUP_UPDATE_TYPE_FULL_CREATE, false, &data, true);
	SendPacket(&data);
}

Group* Group::Create()
{
	return new Group(true);
}

void Group::SetMainAssist(PlayerInfo * pMember)
{
	if(m_mainAssist==pMember)
	{ 
		return;
	}

	m_mainAssist = pMember;
	m_dirty = true;
	Update();
}

void Group::SetMainTank(PlayerInfo * pMember)
{
	if(m_mainTank==pMember)
	{ 
		return;
	}

	m_mainTank=pMember;
	m_dirty = true;
	Update();
}

void Group::SetAssistantLeader(PlayerInfo * pMember)
{
	if(m_assistantLeader == pMember)
	{ 
		return;
	}

	m_assistantLeader = pMember;
	m_dirty = true;
	Update();
}

void Group::ResetInstanceForPlayers(uint32 map, uint32 difficulty)
{
	Lock();
	for(uint32 i = 0; i < GetSubGroupCount(); ++i)
		for(GroupMembersSet::iterator itr = GetSubGroup(i)->GetGroupMembersBegin(); itr != GetSubGroup(i)->GetGroupMembersEnd(); ++itr)
			if((*itr)->m_loggedInPlayer)
				(*itr)->m_loggedInPlayer->SetPersistentInstanceId( map, difficulty, 0 );
	Unlock();
}

uint32	Group::GetSavedInstance( uint32 map, uint32 difficulty )
{
	SimplePointerListNode<active_instance> *ai_itr;
	for( ai_itr = m_active_instances.begin(); ai_itr != m_active_instances.end(); ai_itr = ai_itr->Next() )
		if( ai_itr->data->map_id == map && ai_itr->data->difficulty == difficulty )
			return ai_itr->data->instance_id;
	return GROUP_SAVED_INSTANCE_MISSING;
}

void	Group::RemoveSavedInstance( uint32 map, uint32 difficulty )
{
	SimplePointerListNode<active_instance> *ai_itr;
	for( ai_itr = m_active_instances.begin(); ai_itr != m_active_instances.end(); ai_itr = ai_itr->Next() )
		if( ai_itr->data->map_id == map && ai_itr->data->difficulty == difficulty )
		{
			m_active_instances.remove( ai_itr->data, 1 );
			return;
		}
}


void	Group::RemoveSavedInstances()
{
	m_active_instances.clear( 1 );
}

void Group::AddSavedInstance( uint32 map, uint32 difficulty,uint32 instance_id, uint8 check_exist )
{
	//make sure to not have it multiple times
	if( check_exist )
		RemoveSavedInstance( map, difficulty );

	active_instance *nai = new active_instance;
	nai->map_id = map;
	nai->difficulty = difficulty;
	nai->instance_id = instance_id;
	m_active_instances.push_front( nai );
}

bool Group::CanCast( Player *p, uint32 SpellID )
{
	if( m_Leader->m_loggedInPlayer != p )
		return false;
	//raid markers
	if( SpellID == 84996 || SpellID == 84997 || SpellID == 84998 || SpellID == 84999 || SpellID == 85000 )
		return true;
	return false;
}

void Group::SpawnRaidMarker( uint32 SpellID, DynamicObject *NewMarker )
{
	uint32 slot = SpellID - 84996;
	if( slot >= MAX_RAID_MARKER_COUNT )
		return;	//oh crap, cannot store it

	//try to remove old marker
	ClearRaidMarkers( slot, true );
	if( NewMarker )
	{
		m_markers[ slot ].object_guid = NewMarker->GetGUID();
		m_markers[ slot ].map_id = NewMarker->GetMapId();
		m_markers[ slot ].instance_id = NewMarker->GetInstanceID();
	}
	SendRaidMarkerStatus();
}

void Group::ClearRaidMarkers(uint8 pslot, bool silent)
{
	uint8 from,to;
	if( pslot >= MAX_RAID_MARKER_COUNT )
	{
		from = 0;
		to = MAX_RAID_MARKER_COUNT;
	}
	else
	{
		from = pslot;
		to = pslot+1;
	}
	//try to remove old marker
	for( uint32 slot = from; slot < to; slot++ )
		if( m_markers[ slot ].object_guid )
		{
			MapMgr *mgr = NULL;
			if( m_markers[ slot ].instance_id == -1 )
				mgr = sInstanceMgr.GetMapMgr( m_markers[ slot ].map_id );
			else 
			{
				Instance *inst = sInstanceMgr.GetInstanceByIds( m_markers[ slot ].map_id, m_markers[ slot ].instance_id );
				if( inst )
					mgr = inst->m_mapMgr;
			}
			if( mgr )
			{
				DynamicObject *go = mgr->GetDynamicObject( m_markers[ slot ].object_guid );
				if( go )
					go->Remove();
			}
			m_markers[ slot ].object_guid = 0;
		}
	if( silent == false )
		SendRaidMarkerStatus();
}

void Group::SendRaidMarkerStatus()
{
	uint32 status = 0;
	for( uint32 slot = 0; slot < MAX_RAID_MARKER_COUNT; slot++ )
		if( m_markers[ slot ].object_guid )
			status |= ( 1 << slot );
	WorldPacket data( SMSG_RAID_MARKER_STATUS, 4 + 1);
	data << status;
	SendPacketToAll( &data );
}
/************************************************************************/
/* Voicechat                                                            */
/************************************************************************/
#ifdef VOICE_CHAT

void Group::CreateVoiceSession()
{
	sVoiceChatHandler.CreateGroupChannel( this );
}

void Group::VoiceChannelCreated(uint16 id)
{
	Log.Debug("Group", "voicechannelcreated: id %u", (uint32)id);
	m_voiceChannelId = id;
	if( id != 0 )
	{
		// so we just got a channel. we better activate the slots that are in use so people can talk
		uint32 i;
		for( i = 0; i <= 40; ++i )
		{
			if( m_voiceMembersList[i] != NULL )
				sVoiceChatHandler.ActivateChannelSlot(id, (uint8)i );
		}

		SendVoiceUpdate();
	}
}

void Group::AddVoiceMember(PlayerInfo * pPlayer)
{
	m_groupLock.Acquire();
	Log.Debug("Group", "adding voice member %u to group %u", pPlayer->guid, GetID());
	uint32 i;

	// find him an id
	for( i = 1; i <= 40; ++i )
	{
		if( m_voiceMembersList[i] == NULL )
			break;
	}

	if( i == 41 )
	{
		// no free slots
		Log.Error("Group", "could not add voice member, no slots!");
		return;
	}

	pPlayer->groupVoiceId = i;
	m_voiceMembersList[i] = pPlayer;
	++m_voiceMemberCount;

	if( !m_voiceChannelRequested )
	{
		/*uint32 count = 0;
		for( i = 0; i < 41; ++i )
		{
			if( m_voiceMembersList[i] != NULL )
			{
				++count;
				if( count >= 2 )
					break;
			}
		}*/

		if( m_voiceMemberCount >= 2 )		// Don't create channels with only one member
		{
			CreateVoiceSession();
			m_voiceChannelRequested = true;
		}
	}
	else
	{
		if( m_voiceChannelId != 0 )
		{
			// activate his slot
			sVoiceChatHandler.ActivateChannelSlot(m_voiceChannelId, (uint8)i);
			SendVoiceUpdate();
		}
	}

	m_groupLock.Release();
}

void Group::RemoveVoiceMember(PlayerInfo * pPlayer)
{
	if( pPlayer->groupVoiceId <= 0 )
	{ 
		return;
	}

	Log.Debug("Group", "removing voice member %u from group %u", pPlayer->guid, GetID());

	m_groupLock.Acquire();
	if( m_voiceMembersList[pPlayer->groupVoiceId] == pPlayer )
	{
		--m_voiceMemberCount;
		m_voiceMembersList[pPlayer->groupVoiceId] = NULL;

		if( m_voiceChannelId != 0 )
		{
			// turn off the slot
			sVoiceChatHandler.DeactivateChannelSlot(m_voiceChannelId, pPlayer->groupVoiceId);
			SendVoiceUpdate();
		}

		if( m_voiceMemberCount < 2 )
		{
			// destroy the channel
			sVoiceChatHandler.DestroyGroupChannel(this);
			m_voiceChannelId = 0;
			m_voiceChannelRequested = false;
		}
	}

	pPlayer->groupVoiceId = -1;
	m_groupLock.Release();
}

void Group::SendVoiceUpdate()
{
	m_groupLock.Acquire();
	Log.Debug("Group", "sendgroupupdate id %u", (uint32)m_voiceChannelId);

	//static uint8 EncryptionKey[16] = { 0x14, 0x60, 0xcf, 0xaf, 0x9e, 0xa2, 0x78, 0x38, 0xce, 0xc7, 0xaf, 0x0b, 0x3a, 0x23, 0x61, 0x44 };
	static uint8 EncryptionKey[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	uint8 counter = 1;
	size_t pos;
	uint32 i,j;
	Player * pl;

	WorldPacket data(SMSG_VOICE_SESSION, 100);
	data << uint32( 0x00000E9D );
	data << uint32( 0xE2500000 );		// this appears to be constant :S

	data << uint16( m_voiceChannelId );		// voice channel id, used in udp packet
	data << uint8( 2 );						// party voice channel
	data << uint8( 0 );						// for channels this is name
	data.append( EncryptionKey, 16 );		// encryption key

	// IP
	// these dont appear to be in network byte order.. gg
	data << uint32(htonl(sVoiceChatHandler.GetVoiceServerIP()));
	data << uint16(sVoiceChatHandler.GetVoiceServerPort());
	
	data << uint8( m_voiceMemberCount );
	pos = data.wpos();

	for( i = 0; i <= 40; ++i )
	{
		if( m_voiceMembersList[i] == NULL )
			continue;

		if( m_voiceMembersList[i]->m_loggedInPlayer == NULL )
		{
			// shouldnt happen
			RemoveVoiceMember(m_voiceMembersList[i]);
			continue;
		}

		pl = m_voiceMembersList[i]->m_loggedInPlayer;

		// Append ourself first, always.
		data << uint64 ( pl->GetGUID() );
		data << uint8( i );

		if( pl->m_playerInfo == m_Leader )
		{
			data << uint8(0x06);
		}
		else
		{
			data << uint8(0x46);
		}

		for( j = 0; j <= 40; ++j )
		{
			if( i == j || m_voiceMembersList[j] == NULL )
				continue;

			if( m_voiceMembersList[j]->m_loggedInPlayer == NULL )
			{
				// shouldnt happen
				RemoveVoiceMember(m_voiceMembersList[j]);
				continue;
			}

			data << uint64( m_voiceMembersList[j]->guid );
			data << uint8( j );

			if( m_voiceMembersList[j] == m_Leader )
			{
				data << uint8(0x80) << uint8(0x47);
			}
			else
			{
				data << uint8(0xC8) << uint8(0x47);
			}
		}

		//data << uint8( 0x47 );

		pl->GetSession()->SendPacket( &data );
		data.wpos( pos );
	}

	m_groupLock.Release();
}

void Group::VoiceSessionDropped()
{
	Log.Debug("Group", "Voice session dropped");
	m_groupLock.Acquire();
	for(uint32 i = 0; i < 41; ++i)
	{
		if( m_voiceMembersList[i] != NULL )
		{
			m_voiceMembersList[i]->groupVoiceId = -1;
			m_voiceMembersList[i] = NULL;
		}
	}
	m_voiceChannelRequested = false;
	m_voiceChannelId = 0;
	m_voiceMemberCount = 0;
	m_groupLock.Release();
}

void Group::VoiceSessionReconnected()
{
	if( !sVoiceChatHandler.CanUseVoiceChat() )
	{ 
		return;
	}

	// try to recreate a group if one is needed
	GroupMembersSet::iterator itr1, itr2;
	Log.Debug("Group", "Attempting to recreate voice session for group %u", GetID());

	uint32 i = 0, j = 0;
	SubGroup *sg1 = NULL;
	SubGroup *sg2 = NULL;
	m_groupLock.Acquire();

	for( i = 0; i < m_SubGroupCount; i++ )
	{
		sg1 = m_SubGroups[i];

		if( sg1 != NULL)
		{
			for( itr1 = sg1->GetGroupMembersBegin(); itr1 != sg1->GetGroupMembersEnd(); ++itr1 )
			{
				// should never happen but just in case
				if( (*itr1) == NULL )
					continue;

				/* skip offline players */
				if( (*itr1)->m_loggedInPlayer == NULL )
				{
					if( (*itr1)->groupVoiceId >= 0 )
					{
						// remove from voice members since that player is now offline
						RemoveVoiceMember( (*itr1) );
					}

					continue;
				}

				if( (*itr1)->groupVoiceId < 0 )
				{
					(*itr1)->groupVoiceId = 0;
					AddVoiceMember( (*itr1) );
				}
			}
		}
	}

	m_groupLock.Release();
}
#endif
