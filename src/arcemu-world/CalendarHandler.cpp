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

void WorldSession::HandleCalendarGetCalendar(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_GET_CALENDAR");

	WorldPacket data( SMSG_CALENDAR_SEND_CALENDAR,4+4*0+4+4*0+4+4+50);

    // TODO: calendar invite event output
	data << (uint32) 0;                                     //invite node count
    // TODO: calendar event output
	data << (uint32) 0;                                     //event count

	data << (uint32) 0;                                     //wtf??
	data << (uint32) TimeToGametime( UNIXTIME );         // current time

    uint32 counter = 0;
    size_t p_counter = data.wpos();
    data << uint32(counter);                                // instance save count

	_player->m_playerInfo->savedInstanceIdsLock.Acquire();
	for(int i = 0; i < NUM_INSTANCE_MODES; ++i)
    {
		PlayerInstanceMap::iterator itr;
        for (itr = _player->m_playerInfo->savedInstanceIds[i].begin(); itr != _player->m_playerInfo->savedInstanceIds[i].end(); ++itr)
        {
			Instance  *inst = sInstanceMgr.GetInstanceByIds(itr->first, itr->second );
			if( !inst )
				continue; //well i have no idea why :P
			data << uint32( inst->m_mapId );					//map
			data << uint32( inst->instance_difficulty );				//difficulty
			data << uint32( inst->m_expiration - UNIXTIME );	//time until it expires
			data << uint64( inst->m_instanceId );				// instance save id as unique instance copy id
            ++counter;
        }
    }
	_player->m_playerInfo->savedInstanceIdsLock.Release();

	data.put<uint32>(p_counter,counter);

	data << (uint32) 1135753200;                            //wtf?? (28.12.2005 12:00)
	data << (uint32) 0;
	data << (uint32) 0;
	SendPacket(&data);  
}

void WorldSession::HandleCalendarGetEvent(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_GET_EVENT");
}

void WorldSession::HandleCalendarGuildFilter(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_GUILD_FILTER");
}

void WorldSession::HandleCalendarArenaTeam(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_ARENA_TEAM");
}

void WorldSession::HandleCalendarAddEvent(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_ADD_EVENT");
}

void WorldSession::HandleCalendarUpdateEvent(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_UPDATE_EVENT");
}

void WorldSession::HandleCalendarRemoveEvent(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_REMOVE_EVENT");
}

void WorldSession::HandleCalendarCopyEvent(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_COPY_EVENT");
}

void WorldSession::HandleCalendarEventInvite(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_EVENT_INVITE");
}

void WorldSession::HandleCalendarEventRsvp(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_EVENT_RSVP");
}

void WorldSession::HandleCalendarEventRemoveInvite(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_EVENT_REMOVE_INVITE");
}

void WorldSession::HandleCalendarEventStatus(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_EVENT_STATUS");
}

void WorldSession::HandleCalendarEventModeratorStatus(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_EVENT_MODERATOR_STATUS");
}

void WorldSession::HandleCalendarComplain(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_COMPLAIN");
}

void WorldSession::HandleCalendarGetNumPending(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_GET_NUM_PENDING");
	sStackWorldPacket( data, SMSG_CALENDAR_SEND_NUM_PENDING, 4 + 10 );
	data << uint32( 0 );	//this is event counter
	SendPacket( &data );
}