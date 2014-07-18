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

extern std::string LogFileName;
extern bool bLogChat;

static const uint32 LanguageSkills[NUM_LANGUAGES] = {
	0,				// UNIVERSAL		0x00
	109,			// ORCISH			0x01
	113,			// DARNASSIAN		0x02
	115,			// TAURAHE			0x03
	0,				// -				0x04
	0,				// -				0x05
	111,			// DWARVISH			0x06
	98,				// COMMON			0x07
	139,			// DEMON TONGUE		0x08
	140,			// TITAN			0x09
	137,			// THALSSIAN		0x0A
	138,			// DRACONIC			0x0B
	0,				// KALIMAG			0x0C
	313,			// GNOMISH			0x0D
	315,			// TROLL			0x0E
	0,				// -				0x0F
	0,				// -				0x10
	0,				// -				0x11
	0,				// -				0x12
	0,				// -				0x13
	0,				// -				0x14
	0,				// -				0x15
	0,				// -				0x16
	0,				// -				0x17
	0,				// -				0x18
	0,				// -				0x19
	0,				// -				0x1A
	0,				// -				0x1B
	0,				// -				0x1C
	0,				// -				0x1D
	0,				// -				0x1E
	0,				// -				0x1F
	0,				// -				0x20
	673,			// -				0x21
	0,				// -				0x22
	759,			// -				0x23
};

//this was made to remove colorization from chat messages
void ReplaceCharInStr(char *str,char what,char with)
{
	for(int i=0;i<1000 && str[i]!=0; i++)
		if( str[i] == what )
			str[i] = with;
}

//we search "|t" and "|T" -> positioning ?
//	"|r" "|R" -> reset color
void RemoveChatTextPossitioning(char *str)
{
	for(int i=0;i<1000 && str[i]!=0; i++)
		if( str[i] == '|' && ( str[i+1] == 't' || str[i+1] == 'T' || str[i+1] == 'r' || str[i+1] == 'R') )
			str[i] = '!';
}

int PunishChatTextFormatting( const char *str, Player *_player)
{
	if( _player && _player->GetSession() && _player->GetSession()->HasPermissions() )
		return 0;

	for(int i=0;i<1000 && str[i]!=0; i++)
		if( 
			//position text
			(str[i] == '|' && ( str[i+1] == 't' || str[i+1] == 'T' ))
//			|| ( str[i+1] == 'r' || str[i+1] == 'R' )	//reset color to previous one
			)
		{
			_player->BroadcastMessage( "Chat text formatting is not allowed. Temporary suspended" );
//			sLogonCommHandler.Account_SetBanned(_player->GetSession()->GetAccountNameS(), uint32(UNIXTIME + 60*10), "text formatting");
//			sWorld.DisconnectUsersWithAccount(_player->GetSession()->GetAccountNameS(), _player->GetSession() );
			return 1;
		}
	//icons have .blp extention
/*	if( str[0] != 0 && str[1] != 0 && str[2] != 0 && str[3] != 0 )
		for(int i=0;i<1000 && str[i+4]!=0; i++)
			if( str[i] == '.' && (str[i+1] == 'b' || str[i+1] == 'B' ) && (str[i+2] == 'l' || str[i+2] == 'L' ) && (str[i+3] == 'p' || str[i+3] == 'P' ) )
		{
			_player->BroadcastMessage( "Chat images are not allowed. Temporary suspended" );
//			sLogonCommHandler.Account_SetBanned(_player->GetSession()->GetAccountNameS(), uint32(UNIXTIME + 60*10), "text formatting");
//			sWorld.DisconnectUsersWithAccount(_player->GetSession()->GetAccountNameS(), _player->GetSession() );
			return 1;
		}*/
	return 0;
}

int CheckIsAdvertise(const char *str, WorldSession *sess )
{
	if( sess->GetPermissionCount() != 0 || sWorld.flood_lines > 0 )
		return 0;
	if( !str )
		return 0;
	char low_str[500];
	int i = 0;
	while( str[i] && i < 500 )
		low_str[i] = tolower( str[i++] );

	//if string is too short to contain anything important then we can early exit
	if( i < 4 )
		return 0;

	low_str[i] = 0;	//always 0 terminated
	//fucking advertisers -> first time we eat up the message, next time we ban the fuckers
		//not an email address
	if( !( strstr( low_str, "." ) != NULL && strstr( low_str, "@" ) != NULL )
		//not a friendly site
		&& !( strstr( low_str, "wowbeez" ) != NULL || strstr( low_str, "wowwiki" ) != NULL || strstr( low_str, "wowhead" ) != NULL || strstr( low_str, "thottbot" ) != NULL )
		//some other site
		&& ( strstr( low_str, "www." ) || strstr( low_str, ".com" ) || strstr( low_str, ".org" ) || strstr( low_str, ".net" ) )
		)
	{
		sess->floodLines += sWorld.flood_lines;	//add up
		return 1;
	}
	return 0;
}

bool WorldSession::CheckChatFloodProtection(const char *msg)
{
	if( msg == NULL )
		return false;

	//no need to overwrite GM command mute with our monitoring
	if( m_muted > UNIXTIME )
		return true;

	/* flood detection, wheeee! */
	if(UNIXTIME >= floodTime)
	{
		floodLines = 0;
		floodTime = UNIXTIME + sWorld.flood_seconds;
	}

	if((++floodLines) >= sWorld.flood_lines)
	{
		floodTime = UNIXTIME + sWorld.flood_seconds_ban; //extending flood protection. Learn to be patient
		m_muted = (uint32)(UNIXTIME + sWorld.flood_seconds_ban); //extending flood protection. Learn to be patient
		sLogonCommHandler.Account_SetMute( _player->GetSession()->GetAccountNameS() , m_muted );
		if(sWorld.flood_message && 
			(floodLines % sWorld.flood_lines) == 0 //don't spam back. Only send message once
			)
			_player->BroadcastMessage("Your message has triggered serverside flood protection. You can speak again in %u seconds.", floodTime - UNIXTIME);

		//fucking advertisers
		if( strstr( msg, "www." ) || strstr( msg, ".com" ) || strstr( msg, ".org" ) || strstr( msg, ".net" ) )
		{
			//1 hour ban ?
			sLogonCommHandler.Account_SetBanned(_player->GetSession()->GetAccountNameS(), uint32(UNIXTIME + 60*60), "advertising other sites while flooding chat");
			sWorld.DisconnectUsersWithAccount(_player->GetSession()->GetAccountNameS(), this);
		}
		return true;
	}
	return false;
}

void LogChatMessageFromPlayer( Player *p, const char *msg, uint32 Opcode, const char *wisper = "", const char *channel = "" )
{
	if( p == NULL )
		return;
	uint32 MapID = p->GetMapId();
	char *Type;
	if( Opcode == CMSG_CHAT_MSG_SAY )
		Type = "SAY";
	else if( Opcode == CMSG_CHAT_MSG_YELL )
		Type = "YELL";
	else if( Opcode == CMSG_CHAT_MSG_CHANNEL_SAY )
		Type = "CHANNEL";
	else if( Opcode == CMSG_CHAT_MSG_GUILD || Opcode == CMSG_CHAT_MSG_OFFICER )
		Type = "GUILD";
	else if( Opcode == CMSG_CHAT_MSG_WISPER )
		Type = "WISPER";
	else if( Opcode == CMSG_CHAT_MSG_AFK )
		Type = "AFK";
	else if( Opcode == CMSG_CHAT_MSG_DND )
		Type = "DND";
	else if( Opcode == CMSG_CHAT_MSG_EMOTE )
		Type = "EMOTE";
	else if( Opcode == CMSG_CHAT_MSG_PARTY )
		Type = "PARTY";
	else if( Opcode == CMSG_CHAT_MSG_PARTY_LEADER )
		Type = "PARTYLEADER";
	else if( Opcode == CMSG_CHAT_MSG_RAID )
		Type = "RAID";
	else if( Opcode == CMSG_CHAT_MSG_RAID_WARNING )
		Type = "RAIDWARNING";
	else if( Opcode == CMSG_CHAT_MSG_RAID_LEADER )
		Type = "RAIDLEADER";
	else if( Opcode == CMSG_CHAT_MSG_BATTLEGROUND )
		Type = "BG";
	else if( Opcode == CMSG_CHAT_MSG_BATTLEGROUND_LEADER )
		Type = "BGLEADER";
	else if( Opcode == CMSG_TEXT_EMOTE )
		Type = "TXTEMOTE";
	else if( Opcode == CMSG_CHAT_MSG_SAY )
		Type = "SAY";
	else 
		Type = "";
	if( ChatLog )
		ChatLog->write("%d %d %d %s %s %s %s %s", MapID, p->GetInstanceID(), p->GetLowGUID(), Type, p->GetName(), wisper, channel, msg );
}

void WorldSession::HandleMessagechatOpcode( WorldPacket & recv_data )
{
//	CHECK_PACKET_SIZE(recv_data, 5);
	WorldPacket *data = NULL;
	if(!_player->IsInWorld())
	{ 
		return;
	}

	int32 old_type = -2;
	int32 lang = LANG_UNIVERSAL;

	const char * pMisc = 0;
	const char * pMsg = 0;

	//when you break AFK
	if( recv_data.GetOpcode() == CMSG_CHAT_MSG_AFK || recv_data.GetOpcode() == CMSG_CHAT_MSG_DND || recv_data.GetOpcode() == CMSG_CHAT_MSG_EMOTE )
		lang = LANG_UNIVERSAL;
	else if( recv_data.wpos() > 4 )
		recv_data >> lang;

	if( lang >= NUM_LANGUAGES )
	{ 
		return;
	}

	if(GetPlayer()->IsBanned())
	{
		GetPlayer()->BroadcastMessage("You cannot do that when banned.");
		return;
	}

	//no chatting at all ?
	if( GetPlayer()->IsSpectator() && recv_data.GetOpcode() != CMSG_CHAT_MSG_SAY ) //gotto have a way to exit spectator mode anytime
		return;

	std::stringstream irctext;
	irctext.rdbuf()->str("");
	std::string msg;
	msg.reserve(256);

	//arghhh STFU. I'm not giving you gold or items NOOB
	switch( recv_data.GetOpcode() )
	{
		case CMSG_CHAT_MSG_WISPER:
#if GM_STATISTICS_UPDATE_INTERVAL > 0 
			if( GetPlayer() && GetPlayer()->m_GM_statistics )
				GetPlayer()->m_GM_statistics->wisper_chat_messages++;
#endif
//		case CHAT_MSG_EMOTE:
		case CMSG_CHAT_MSG_SAY:
		case CMSG_CHAT_MSG_YELL:
		case CMSG_CHAT_MSG_CHANNEL_SAY:
		{
			if( m_muted && m_muted >= (uint32)UNIXTIME )
			{
				SystemMessage("Your voice is currently muted by a moderator.");
				return;
			}
#if GM_STATISTICS_UPDATE_INTERVAL > 0 
			if( GetPlayer() && GetPlayer()->m_GM_statistics && recv_data.GetOpcode() != CHAT_MSG_WHISPER )
				GetPlayer()->m_GM_statistics->global_chat_messages++;
#endif
		}break;
	}

	switch( recv_data.GetOpcode() )
	{
	case CMSG_CHAT_MSG_EMOTE:
		{
			recv_data >> msg;
			LogChatMessageFromPlayer( GetPlayer(), msg.c_str(), recv_data.GetOpcode() );
			if( lang != -1 
				&& !GetPermissionCount() 
				&& sWorld.flood_lines
				&& CheckChatFloodProtection( msg.c_str() ) )
			{
				return;
			}

			if( PunishChatTextFormatting( msg.c_str(), _player ) || CheckIsAdvertise( msg.c_str(), this ) )
			{ 
				return;
			}

			if(sWorld.interfaction_chat && lang > 0)
				lang=0;

			if(GetPlayer()->m_modlanguage >=0)
				data = sChatHandler.FillMessageData( CHAT_MSG_EMOTE, GetPlayer()->m_modlanguage,  msg.c_str(), _player->GetGUID(), _player->bGMTagOn ? CHAT_FLAG_GM_TAG : 0, 0, _player );
			else if (lang==0 && sWorld.interfaction_chat)
				data = sChatHandler.FillMessageData( CHAT_MSG_EMOTE, CanUseCommand('0') ? LANG_UNIVERSAL : lang,  msg.c_str(), _player->GetGUID(), _player->bGMTagOn ? CHAT_FLAG_GM_TAG : 0, 0, _player );
			else	
				data = sChatHandler.FillMessageData( CHAT_MSG_EMOTE, CanUseCommand('c') ? LANG_UNIVERSAL : lang,  msg.c_str(), _player->GetGUID(), _player->bGMTagOn ? CHAT_FLAG_GM_TAG : 0, 0, _player );
			GetPlayer()->SendMessageToSet( data, true ,true );

			//sLog.outString("[emote] %s: %s", _player->GetName(), msg.c_str());
			delete data;
			data = NULL;
			
			pMsg=msg.c_str();
			pMisc=0;
			old_type = CHAT_MSG_EMOTE;
		}break; 
	case CMSG_CHAT_MSG_SAY:
		{
			recv_data >> msg;
			LogChatMessageFromPlayer( GetPlayer(), msg.c_str(), recv_data.GetOpcode() );
			if( lang != -1 
				&& !GetPermissionCount() 
				&& sWorld.flood_lines
				&& CheckChatFloodProtection( msg.c_str() ) )
			{
				return;
			}

			if( PunishChatTextFormatting( msg.c_str(), _player ) || CheckIsAdvertise( msg.c_str(), this ) )
			{ 
				return;
			}

			if(sWorld.interfaction_chat && lang > 0)
				lang=0;

			if (sChatHandler.ParseCommands(msg.c_str(), this) > 0)
				break;

			if(g_chatFilter->Parse(msg) == true)
			{
				SystemMessage("Your chat message was blocked by a server-side filter.");
				return;
			}

			if( GetPlayer()->IsSpectator() == true )
				return;

			if(GetPlayer()->m_modlanguage >=0)
			{
				data = sChatHandler.FillMessageData( CHAT_MSG_SAY, GetPlayer()->m_modlanguage,  msg.c_str(), _player->GetGUID(), _player->bGMTagOn ? CHAT_FLAG_GM_TAG : 0, 0, _player );
			}
			else
			{
				if(lang > 0 && LanguageSkills[lang] && _player->_HasSkillLine(LanguageSkills[lang]) == false)
				{ 
					return;
				}

				if(lang==0 && !CanUseCommand('c') && !sWorld.interfaction_chat)
				{ 
					return;
				}
				data = sChatHandler.FillMessageData( CHAT_MSG_SAY, lang, msg.c_str(), _player->GetGUID(), _player->bGMTagOn ? CHAT_FLAG_GM_TAG : 0, 0, _player );
				SendChatPacket(data, 1, lang, this);
				for(InRangePlayerSetRecProt::iterator itr = _player->m_inRangePlayers.begin(); itr != _player->m_inRangePlayers.end(); ++itr)
				{
					if( (*itr)->GetSession() )
						(*itr)->GetSession()->SendChatPacket(data, 1, lang, this);
				}
			}

			
			//sLog.outString("[say] %s: %s", _player->GetName(), msg.c_str());
			delete data;
			data = NULL;
			pMsg=msg.c_str();
			pMisc=0;
			old_type = CHAT_MSG_SAY;
		} break;
	case CMSG_CHAT_MSG_PARTY:
		old_type = CHAT_MSG_PARTY;
	case CMSG_CHAT_MSG_PARTY_LEADER:
		if( old_type == -2 ) 
		{
			if( _player->IsGroupLeader() == false )
				old_type = CHAT_MSG_PARTY;
			else
				old_type = CHAT_MSG_PARTY_LEADER;
		}
	case CMSG_CHAT_MSG_RAID:
		if( old_type == -2 ) old_type = CHAT_MSG_RAID;
	case CMSG_CHAT_MSG_RAID_LEADER:
		if( old_type == -2 ) 
		{
			if( _player->IsGroupLeader() == false )
				old_type = CHAT_MSG_RAID;
			else
				old_type = CHAT_MSG_RAID_LEADER;
		}
	case CMSG_CHAT_MSG_RAID_WARNING:
		if( old_type == -2 ) old_type = CHAT_MSG_RAID_WARNING;
		{
			recv_data >> msg;
			LogChatMessageFromPlayer( GetPlayer(), msg.c_str(), recv_data.GetOpcode() );
			if( PunishChatTextFormatting( msg.c_str(), _player ) 
//				|| CheckIsAdvertise( msg.c_str(), this ) 
				)
			{ 
				return;
			}

			if (sChatHandler.ParseCommands(msg.c_str(), this) > 0)
				break;

			if(sWorld.interfaction_chat && lang > 0)
				lang=0;

			if(g_chatFilter->Parse(msg) == true)
			{
				SystemMessage("Your chat message was blocked by a server-side filter.");
				return;
			}

			Group *pGroup = _player->GetGroup();
			if(pGroup == NULL) break;
			
			if(GetPlayer()->m_modlanguage >=0)
				data=sChatHandler.FillMessageData( old_type, GetPlayer()->m_modlanguage,  msg.c_str(), _player->GetGUID(), _player->bGMTagOn ? CHAT_FLAG_GM_TAG : 0, 0, _player );
			else if(lang==0 && sWorld.interfaction_chat)
				data=sChatHandler.FillMessageData( old_type, (CanUseCommand('0') && lang != -1) ? LANG_UNIVERSAL : lang, msg.c_str(), _player->GetGUID(), _player->bGMTagOn ? CHAT_FLAG_GM_TAG : 0, 0, _player );
			else
				data=sChatHandler.FillMessageData( old_type, (CanUseCommand('c') && lang != -1) ? LANG_UNIVERSAL : lang, msg.c_str(), _player->GetGUID(), _player->bGMTagOn ? CHAT_FLAG_GM_TAG : 0, 0, _player );
			if(old_type == CHAT_MSG_PARTY && pGroup->GetGroupType() == GROUP_TYPE_RAID)
			{
				// only send to that subgroup
				SubGroup * sgr = _player->GetGroup() ?
					_player->GetGroup()->GetSubGroup(_player->GetSubGroup()) : 0;

				if(sgr)
				{
					_player->GetGroup()->Lock();
					for(GroupMembersSet::iterator itr = sgr->GetGroupMembersBegin(); itr != sgr->GetGroupMembersEnd(); ++itr)
					{
						if((*itr)->m_loggedInPlayer)
							(*itr)->m_loggedInPlayer->GetSession()->SendChatPacket(data, 1, lang, this);
					}
					_player->GetGroup()->Unlock();
				}
			}
			else
			{
				SubGroup * sgr;
				for(uint32 i = 0; i < _player->GetGroup()->GetSubGroupCount(); ++i)
				{
					sgr = _player->GetGroup()->GetSubGroup(i);
					_player->GetGroup()->Lock();
					for(GroupMembersSet::iterator itr = sgr->GetGroupMembersBegin(); itr != sgr->GetGroupMembersEnd(); ++itr)
					{
						if((*itr)->m_loggedInPlayer)
							(*itr)->m_loggedInPlayer->GetSession()->SendChatPacket(data, 1, lang, this);
					}
					_player->GetGroup()->Unlock();
				}
			}
			//sLog.outString("[party] %s: %s", _player->GetName(), msg.c_str());
			delete data;
			data = NULL;
			pMsg=msg.c_str();
			pMisc=0;
		} break;
	case CMSG_CHAT_MSG_GUILD:
		{
			recv_data >> msg;
			LogChatMessageFromPlayer( GetPlayer(), msg.c_str(), recv_data.GetOpcode() );
			if( PunishChatTextFormatting( msg.c_str(), _player )
//				|| CheckIsAdvertise( msg.c_str(), this )
				)
			{ 
				return;
			}
			if (sChatHandler.ParseCommands(msg.c_str(), this) > 0)
			{
				break;
			}

			if(g_chatFilter->Parse(msg) == true)
			{
				SystemMessage("Your chat message was blocked by a server-side filter.");
				return;
			}

			if(_player->m_playerInfo->guild)
				_player->m_playerInfo->guild->GuildChat(msg.c_str(), this, lang);

			pMsg=msg.c_str();
			pMisc=0;
			old_type = CHAT_MSG_GUILD;
		} break;
	case CMSG_CHAT_MSG_OFFICER:
		{
			recv_data >> msg;
			LogChatMessageFromPlayer( GetPlayer(), msg.c_str(), recv_data.GetOpcode() );

			if (sChatHandler.ParseCommands(msg.c_str(), this) > 0)
				break;

			if(g_chatFilter->Parse(msg) == true)
			{
				SystemMessage("Your chat message was blocked by a server-side filter.");
				return;
			}

			if(_player->m_playerInfo->guild)
				_player->m_playerInfo->guild->OfficerChat(msg.c_str(), this, lang);

			pMsg=msg.c_str();
			pMisc=0;
			old_type = CHAT_MSG_OFFICER;
		} break;
	case CMSG_CHAT_MSG_YELL:
		{
			recv_data >> msg;
			LogChatMessageFromPlayer( GetPlayer(), msg.c_str(), recv_data.GetOpcode() );
			if( lang != -1 
				&& !GetPermissionCount() 
				&& sWorld.flood_lines
				&& CheckChatFloodProtection( msg.c_str() ) )
			{
				return;
			}
			if( PunishChatTextFormatting( msg.c_str(), _player ) || CheckIsAdvertise( msg.c_str(), this ) )
			{ 
				return;
			}

			if(sWorld.interfaction_chat && lang > 0)
				lang=0;

			if (sChatHandler.ParseCommands(msg.c_str(), this) > 0)
				break;

			if(g_chatFilter->Parse(msg) == true)
			{
				SystemMessage("Your chat message was blocked by a server-side filter.");
				return;
			}
			if(lang > 0 && LanguageSkills[lang] && _player->_HasSkillLine(LanguageSkills[lang]) == false)
			{ 
				return;
			}

			//Zack : this is overwritten in 3 rows below-> useless ?
//			if(lang==0 && sWorld.interfaction_chat)
//				data = sChatHandler.FillMessageData( CHAT_MSG_YELL, (CanUseCommand('0') && lang != -1) ? LANG_UNIVERSAL : lang,  msg.c_str(), _player->GetGUID(), _player->bGMTagOn ? CHAT_FLAG_GM_TAG : 0 );

			if(lang==0 && !CanUseCommand('c') && !sWorld.interfaction_chat)
			{ 
				return;
			}

			int32 NowLang;
			if( CanUseCommand('c') && lang != -1 )
				NowLang = LANG_UNIVERSAL;
			else if( GetPlayer()->m_modlanguage >= 0 )
				NowLang = GetPlayer()->m_modlanguage;
			else
				NowLang = lang;
			data = sChatHandler.FillMessageData( CHAT_MSG_YELL, NowLang,  msg.c_str(), _player->GetGUID(), _player->bGMTagOn ? CHAT_FLAG_GM_TAG : 0, 0, _player );

			//SendPacket(data);
			//sWorld.SendZoneMessage(data, GetPlayer()->GetZoneId(), this);
			_player->GetMapMgr()->SendChatMessageToCellPlayers(_player, data, 2, 1, lang, this);
			delete data;
			data = NULL;
			//sLog.outString("[yell] %s: %s", _player->GetName(), msg.c_str());
			pMsg=msg.c_str();
			pMisc=0;
			old_type = CHAT_MSG_YELL;
		} break;
	case CMSG_CHAT_MSG_WISPER:
		{
			std::string to = "",tmp;
			recv_data >> msg;
			recv_data >> to;
			LogChatMessageFromPlayer( GetPlayer(), msg.c_str(), recv_data.GetOpcode(), to.c_str() );
			if( PunishChatTextFormatting( msg.c_str(), _player )
//				|| CheckIsAdvertise( msg.c_str(), this ) 
				)
			{ 
				return;
			}

			if(sWorld.interfaction_chat && lang > 0)
				lang=0;

			if(g_chatFilter->Parse(msg) == true)
			{
				SystemMessage("Your chat message was blocked by a server-side filter.");
				return;
			}
		 
			Player *player = objmgr.GetPlayer(to.c_str(), false);
			if(!player)
			{
				sStackWorldPacket( data, SMSG_CHAT_PLAYER_NOT_FOUND, 2000 );
				data << to;
				SendPacket( &data );
				break;
			}

			// Check that the player isn't a gm with his status on
			if(!_player->GetSession()->GetPermissionCount() && player->bGMTagOn && player->gmTargets.count(_player) == 0)
			{
				// Build automated reply
				string Reply = "This Game Master does not currently have an open ticket from you and did not receive your whisper. Please submit a new GM Ticket request if you need to speak to a GM. This is an automatic message.";
				data = sChatHandler.FillMessageData( CHAT_MSG_WHISPER, LANG_UNIVERSAL, Reply.c_str(), player->GetGUID(), 3);
				SendPacket(data);
				delete data;
				data = NULL;
				break;
			}

			if(lang > 0 && LanguageSkills[lang] && _player->_HasSkillLine(LanguageSkills[lang]) == false)
			{ 
				return;
			}

			if(lang==0 && sWorld.interfaction_chat)
				data = sChatHandler.FillMessageData( CHAT_MSG_WHISPER, ((CanUseCommand('0') || player->GetSession()->CanUseCommand('0')) && lang != -1) ? LANG_UNIVERSAL : lang,  msg.c_str(), _player->GetGUID(), _player->bGMTagOn ? CHAT_FLAG_GM_TAG : 0, 0, _player );
			else if(lang==0 && !CanUseCommand('c'))
			{ 
				return;
			}

			if( player->Social_IsIgnoring( _player->GetLowGUID() ) )
			{
				if( data )
				{
					delete data;
					data = NULL;
				}
				data = sChatHandler.FillMessageData( CHAT_MSG_IGNORED, LANG_UNIVERSAL,  msg.c_str(), player->GetGUID(), _player->bGMTagOn ? CHAT_FLAG_GM_TAG : 0, 0, _player );
				SendPacket(data);
				delete data;
				data = NULL;
			}
			else
			{
				if( data )
				{
					delete data;
					data = NULL;
				}
				if(GetPlayer()->m_modlanguage >=0)
					data = sChatHandler.FillMessageData( CHAT_MSG_WHISPER, GetPlayer()->m_modlanguage,  msg.c_str(), _player->GetGUID(), _player->bGMTagOn ? CHAT_FLAG_GM_TAG : 0, 0, _player );
				else
					data = sChatHandler.FillMessageData( CHAT_MSG_WHISPER, ((CanUseCommand('c') || player->GetSession()->CanUseCommand('c')) && lang != -1) ? LANG_UNIVERSAL : lang,  msg.c_str(), _player->GetGUID(), _player->bGMTagOn ? CHAT_FLAG_GM_TAG : 0, 0, _player );

				player->GetSession()->SendPacket(data);
				delete data;
				data = NULL;
			}

			//Sent the to Users id as the channel, this should be fine as it's not used for wisper
		  
			if( data )
			{
				delete data;
				data = NULL;
			}
			data = sChatHandler.FillMessageData(CHAT_MSG_WHISPER_INFORM, LANG_UNIVERSAL,msg.c_str(), player->GetGUID(), player->bGMTagOn ? CHAT_FLAG_GM_TAG : 0, 0, _player  );
			SendPacket(data);
			delete data;
			data = NULL;

			if(player->HasFlag(PLAYER_FLAGS, PLAYER_FLAG_AFK))
			{
				// Has AFK flag, autorespond.
				data = sChatHandler.FillMessageData(CHAT_MSG_AFK, LANG_UNIVERSAL,  player->m_afk_reason.c_str(),player->GetGUID(), _player->bGMTagOn ? CHAT_FLAG_GM_TAG : 0, 0, _player );
				SendPacket(data);
				delete data;
				data = NULL;
			}
			else if(player->HasFlag(PLAYER_FLAGS, PLAYER_FLAG_DND))
			{
				// Has AFK flag, autorespond.
				if (player->GetTeamInitial() == _player->GetTeamInitial()) {
					data = sChatHandler.FillMessageData(CHAT_MSG_DND, LANG_UNIVERSAL, player->m_afk_reason.c_str(),player->GetGUID(), _player->bGMTagOn ? CHAT_FLAG_GM_TAG : 0, 0, _player );
				} else {
					data = sChatHandler.FillMessageData(CHAT_MSG_DND, LANG_UNIVERSAL, "",player->GetGUID(), _player->bGMTagOn ? CHAT_FLAG_GM_TAG : 0, 0, _player );
				}
				SendPacket(data);
				delete data;
				data = NULL;
			}

			//sLog.outString("[whisper] %s to %s: %s", _player->GetName(), to.c_str(), msg.c_str());
			pMsg=msg.c_str();
			pMisc=to.c_str();
			old_type = CHAT_MSG_WHISPER;
		} break;
	case CMSG_CHAT_MSG_CHANNEL_SAY:
		{
			std::string channel = "";
			recv_data >> msg;
			recv_data >> channel;
			LogChatMessageFromPlayer( GetPlayer(), msg.c_str(), recv_data.GetOpcode(), NULL, channel.c_str() );
			if( lang != -1 
				&& !GetPermissionCount() 
				&& sWorld.flood_lines
				&& CheckChatFloodProtection( msg.c_str() ) )
			{
				return;
			}
			if( PunishChatTextFormatting( msg.c_str(), _player ) || CheckIsAdvertise( msg.c_str(), this ) )
				return;
		 
			if(g_chatFilter->Parse(msg) == true)
			{
				SystemMessage("Your chat message was blocked by a server-side filter.");
				return;
			}

			if (sChatHandler.ParseCommands(msg.c_str(), this) > 0)
				break;

			Channel *chn = channelmgr.GetChannel(channel.c_str(),GetPlayer()); 
			if(chn) 
			{
				//hacks i tell you, Include gear score in message if this is global talk(for easier raid groupings)
				const char *channel_name = channel.c_str();
				if( channel_name[0]=='g' && channel_name[1]=='l' )
				{
					char tstr[3000];
					int32 Score = GetPlayer()->LFG_GetGearScore() + GetPlayer()->GetMaxPersonalRating();
					sprintf( tstr,"[%u]:%s", Score, msg.c_str());
					chn->Say(GetPlayer(),tstr, NULL, false);
				}
				else
					chn->Say(GetPlayer(),msg.c_str(), NULL, false);
			}

			//sLog.outString("[%s] %s: %s", channel.c_str(), _player->GetName(), msg.c_str());
			pMsg=msg.c_str();
			pMisc=channel.c_str();
			old_type = CHAT_MSG_CHANNEL;
		} break;
	case CMSG_CHAT_MSG_AFK:
		{
			if( GetPlayer()->CombatStatus.IsInCombat() )
			{
				SystemMessage("You are in combat.");
				return;
			}
			std::string reason;
			recv_data >> reason;
			LogChatMessageFromPlayer( GetPlayer(), reason.c_str(), recv_data.GetOpcode() );
			GetPlayer()->SetAFKReason(reason);

			if(g_chatFilter->Parse(msg) == true)
			{
				SystemMessage("Your chat message was blocked by a server-side filter.");
				return;
			}

			/* WorldPacket *data, WorldSession* session, uint32 type, uint32 language, const char *channelName, const char *message*/
			if(GetPlayer()->HasFlag(PLAYER_FLAGS, PLAYER_FLAG_AFK))
			{
				GetPlayer()->RemoveFlag(PLAYER_FLAGS, PLAYER_FLAG_AFK);
				if(sWorld.GetKickAFKPlayerTime())
					sEventMgr.RemoveEvents(GetPlayer(),EVENT_PLAYER_SOFT_DISCONNECT);
			}
			else
			{
				GetPlayer()->SetFlag(PLAYER_FLAGS, PLAYER_FLAG_AFK);

                if( GetPlayer()->m_bg )
                    GetPlayer()->m_bg->RemovePlayer( GetPlayer(), false );

				if(sWorld.GetKickAFKPlayerTime() && sEventMgr.HasEvent(GetPlayer(), EVENT_PLAYER_SOFT_DISCONNECT) == false )
					sEventMgr.AddEvent(GetPlayer(),&Player::SoftDisconnect,EVENT_PLAYER_SOFT_DISCONNECT,sWorld.GetKickAFKPlayerTime(),1,0);
			}			
			old_type = CHAT_MSG_AFK;
		} break;
	case CMSG_CHAT_MSG_DND:
		{
			if( GetPlayer()->CombatStatus.IsInCombat() )
			{
				SystemMessage("You are in combat.");
				return;
			}
			std::string reason;
			recv_data >> reason;
			LogChatMessageFromPlayer( GetPlayer(), reason.c_str(), recv_data.GetOpcode() );
			GetPlayer()->SetAFKReason(reason);

			if(g_chatFilter->Parse(msg) == true)
			{
				SystemMessage("Your chat message was blocked by a server-side filter.");
				return;
			}

			if(GetPlayer()->HasFlag(PLAYER_FLAGS, PLAYER_FLAG_DND))
				GetPlayer()->RemoveFlag(PLAYER_FLAGS, PLAYER_FLAG_DND);
			else
			{
				GetPlayer()->SetFlag(PLAYER_FLAGS, PLAYER_FLAG_DND);
			}		  
			old_type = CHAT_MSG_DND;
		} break;
	case CMSG_CHAT_MSG_BATTLEGROUND:
		old_type = CHAT_MSG_BATTLEGROUND;
	case CMSG_CHAT_MSG_BATTLEGROUND_LEADER:
		{
			if( old_type == -2 ) old_type = CHAT_MSG_BATTLEGROUND_LEADER;
			recv_data >> msg;
			LogChatMessageFromPlayer( GetPlayer(), msg.c_str(), recv_data.GetOpcode() );
			if( sChatHandler.ParseCommands( msg.c_str(), this ) > 0 )
				break;

			if( g_chatFilter->Parse( msg ) == true )
			{
				SystemMessage("Your chat message was blocked by a server-side filter.");
				return;
			}
			if( _player->m_bg != NULL && _player->GetTeam() != NULL )
			{
				data = sChatHandler.FillMessageData( old_type, lang, msg.c_str(), _player->GetGUID() );
				_player->m_bg->DistributePacketToTeam( data, _player->GetTeam() );
				delete data;
				data = NULL;
			}
		}break;
	default:
		sLog.outError("CHAT: unknown msg type(opcode) %u, lang: %u", recv_data.GetOpcode(), lang);
	}

	if(pMsg && old_type != -2 )
		sHookInterface.OnChat(_player, old_type, lang, pMsg, pMisc);
}

void WorldSession::HandleClearEmoteOpcode( WorldPacket &recv_data )
{
	//uint32 is sent that was 0 all the time. I guess it represents new emote state, cannot confirm though
	uint32 new_emote_state;
	recv_data >> new_emote_state;
	_player->SetUInt32Value( UNIT_NPC_EMOTESTATE, new_emote_state );
	_player->SetStandState( 0 );
	_player->Emote( EMOTE_ONESHOT_YES );	//this emote depends on the thing we activated last. Like say goodbye
/*
{SERVER} Packet: (0xE2F6) SMSG_STANDSTATE_UPDATE PacketSize = 1 TimeStamp = 33232799
00 
{SERVER} Packet: (0x76FE) SMSG_EMOTE PacketSize = 12 TimeStamp = 33232955
11 01 00 00 B2 00 00 00 49 0C 30 F1 
*/
}

void WorldSession::HandleTextEmoteOpcode( WorldPacket & recv_data )
{
	CHECK_PACKET_SIZE(recv_data, 16);
	if(!_player->IsInWorld() || !_player->isAlive())
	{ 
		return;
	}
	if( _player->IsSpectator() )
	{
		return;
	}
/*
13329
{CLIENT} Packet: (0x7F5C) CMSG_TEXT_EMOTE PacketSize = 16 stamp = 13948454
57 00 00 00 
FF FF FF FF 
00 00 00 00 00 00 00 00
*/
	uint64 guid;
	uint32
		text_emote,
		unk,
		namelen =1;
	const char* name ="";

	if( !GetPermissionCount() && sWorld.flood_lines && CheckChatFloodProtection( "" ) )
	{
		return;
	}

	recv_data >> text_emote;
	recv_data >> unk;
	recv_data >> guid;

	Unit * pUnit = _player->GetMapMgr()->GetUnit(guid);
	if(pUnit)
	{
		if( pUnit->IsPlayer() )
		{
			name = SafePlayerCast( pUnit )->GetName();
			namelen = (uint32)strlen(name) + 1;
		}
		else if ( pUnit->IsPet() )
		{
			name = SafePetCast( pUnit )->GetName()->c_str();
			namelen = (uint32)strlen(name) + 1;
		}
		else if(pUnit->GetTypeId() == TYPEID_UNIT)
		{
			Creature * p = SafeCreatureCast(pUnit);
			if(p->GetCreatureInfo())
			{
				name = p->GetCreatureInfo()->Name;
				namelen = (uint32)strlen(name) + 1;
			}
			else
			{
				name = "";
				namelen = 1;
			}
		}
	}

	emoteentry *em = dbcEmoteEntry.LookupEntry(text_emote);
	if(em)
	{
		sHookInterface.OnEmote(_player, (EmoteType)em->textid, pUnit);
		_player->Event_AchiementCriteria_Received( ACHIEVEMENT_CRITERIA_TYPE_DO_EMOTE,text_emote,ACHIEVEMENT_UNUSED_FIELD_VALUE,1,ACHIEVEMENT_EVENT_ACTION_ADD);
		if(pUnit)
			CALL_SCRIPT_EVENT(pUnit,OnEmote)(_player,(EmoteType)em->textid);

		sStackWorldPacket( data, SMSG_EMOTE, 28 + 50 );
        switch(em->textid)
        {
            case EMOTE_STATE_SLEEP:
				{
					_player->SetStandState( STANDSTATE_SLEEP );
				}break;
            case EMOTE_STATE_SIT:
				{
					_player->SetStandState( STANDSTATE_SIT );
				}break;
            case EMOTE_STATE_KNEEL:
				{
					_player->SetStandState( STANDSTATE_KNEEL );
				}break;
			case EMOTE_STATE_DANCE:
			case EMOTE_STATE_READ:
			case EMOTE_STATE_USESTANDING:
			case EMOTE_STATE_WORK:
			case EMOTE_STATE_READYRIFLE:
			case EMOTE_STATE_WORK_MINING:
			case EMOTE_STATE_WORK_CHOPWOOD:
			case EMOTE_STATE_APPLAUD:
			case EMOTE_STATE_TALK:
			case EMOTE_STATE_FISHING:
			case EMOTE_STATE_CANNIBALIZE:
			case EMOTE_STATE_DANCESPECIAL:
			case EMOTE_STATE_DANCE_CUSTOM:
			case EMOTE_STATE_SIT_CHAIR_MED:
			case EMOTE_STATE_SIT_CHAIR_HIGH:
				{
					_player->SetUInt32Value(UNIT_NPC_EMOTESTATE, em->textid);
				}break;
			default:
				{
					data << (uint32)em->textid;
					data << (uint64)GetPlayer()->GetGUID();
					GetPlayer()->SendMessageToSet(&data, true); //If player receives his own emote, his animation stops.
				}break;
		}

		data.Initialize(SMSG_TEXT_EMOTE);
		data << (uint64)GetPlayer()->GetGUID();
		data << (uint32)text_emote;
		data << unk;
		data << (uint32)namelen;
		data << name;
		GetPlayer()->SendMessageToSet(&data, true);
	}
}

void WorldSession::HandleReportSpamOpcode(WorldPacket & recvPacket)
{
/*	CHECK_PACKET_SIZE(recvPacket, 29);

    // the 0 in the out packet is unknown
    GetPlayer()->GetSession()->OutPacket(SMSG_REPORT_SPAM_RESPONSE, 1, 0 );

	uint8 unk1;
	uint64 reportedGuid;
	uint32 unk2;
	uint32 messagetype;
	uint32 unk3;
	uint32 unk4;
	std::string message;
	recvPacket >> unk1 >> reportedGuid >> unk2 >> messagetype >> unk3 >> unk4 >> message;

	Player * rPlayer = objmgr.GetPlayer((uint32)reportedGuid);
	if(!rPlayer)
		return;
*/
}
