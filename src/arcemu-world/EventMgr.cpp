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

#ifdef USE_OLD_EVENT_MANAGER
initialiseSingleton( EventMgr );

TimedEvent * TimedEvent::Allocate(void* object, CallbackBase* callback, uint32 flags, time_t time, uint32 repeat)
{
	return new TimedEvent(object, callback, flags, time, repeat, 0);
}
#endif

#ifndef USE_OLD_EVENT_MANAGER
initialiseSingleton( EventMgr );

TimedEvent * TimedEvent::Allocate(void* object, CallbackBase* callback, uint32 flags, uint32 time, uint32 repeat)
{
	return new TimedEvent(object, callback, flags, time, repeat, 0);
}

TimedEvent::TimedEvent(void* object, CallbackBase* callback, uint32 type, uint32 TriggerDelay, uint32 repeat, uint32 flags) : 
		obj(object), cb(callback), eventType(type), eventFlag(flags), repeats(repeat), TE_deleted(false),ref(0) 
{
	if( ( flags & EVENT_FLAG_TICK_ON_NEXT_UPDATE ) != 0 ) //tick on next object update instantly unless it is a one time tick
		TriggerAtStamp = getMSTime();
	else
		TriggerAtStamp = getMSTime() + TriggerDelay;
	TriggerTimeInterval = TriggerDelay;
}
#endif
