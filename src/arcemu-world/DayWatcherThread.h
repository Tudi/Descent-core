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

#ifndef _DAYWATCHERTHREAD_
#define _DAYWATCHERTHREAD_

//in seconds the amount of time when next update should trigger
enum DAYWATCHERSETTINGS
{
	WEEKLY		= (60*60)*24*7,
	DAILY		= (60*60)*24,
	MONTHLY		= (60*60)*24*31,
	HOURLY		= (60*60),
};

class DayWatcherThread : public CThread
{
	bool m_running;
	bool m_busy;
	bool m_dirty;

	time_t last_arena_time;
	uint32 arena_period;

	time_t last_daily_time;
	uint32 daily_period;

	time_t last_weekly_time;
	time_t next_weekly_time;

public:
	DayWatcherThread();
	~DayWatcherThread();

	bool run();
	void terminate();
	void maintenence();
	void load_settings();
	void update_settings();
	uint32 get_timeout_from_string(const char * string, uint32 def);
	void update_arena();
	void update_daily();
	void update_weekly();
};

#endif
