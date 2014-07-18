/*
 * ArcEmu MMORPG Server
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

#ifndef _THREADING_STARTER_H
#define _THREADING_STARTER_H

class SERVER_DECL ThreadBase
{
public:
	ThreadBase() { last_updated = 0; }
	virtual ~ThreadBase() {}
	virtual bool run() = 0;
	virtual void OnShutdown() { last_updated = 0; }
#if (defined( WIN32 ) || defined( WIN64 ) )
	HANDLE THREAD_HANDLE;
#else
	pthread_t THREAD_HANDLE;
#endif
	uint32	last_updated;		//used to monitor if it got deadlocked
};

#endif

