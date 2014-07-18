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

#include "Common.h"
#include <Psapi.h>
#pragma comment(lib, "Psapi")

using namespace std;

vector<string> StrSplit(const string &src, const string &sep)
{
	vector<string> r;
	string s;
	for (string::const_iterator i = src.begin(); i != src.end(); i++) {
		if (sep.find(*i) != string::npos) {
			if (s.length()) r.push_back(s);
			s = "";
		} else {
			s += *i;
		}
	}
	if (s.length()) r.push_back(s);
	return r;
}

void SetThreadName(const char* format, ...)
{
	// This isn't supported on nix?
	va_list ap;
	va_start(ap, format);

#if (defined( WIN32 ) || defined( WIN64 ) )

	char thread_name[200];
	vsnprintf(thread_name, 200, format, ap);

	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.dwThreadID = GetCurrentThreadId();
	info.dwFlags = 0;
	info.szName = thread_name;

	__try
	{
#ifdef _WIN64
		RaiseException(0x406D1388, 0, sizeof(info)/sizeof(DWORD), (ULONG_PTR*)&info);
#else
		RaiseException(0x406D1388, 0, sizeof(info)/sizeof(DWORD), (DWORD*)&info);
#endif
	}
	__except(EXCEPTION_CONTINUE_EXECUTION)
	{

	}

#endif

	va_end(ap);
}

time_t convTimePeriod ( uint32 dLength, char dType )
{
	time_t rawtime = 0;
	if (dLength == 0)
		return rawtime;
	struct tm * ti = localtime( &rawtime );
	switch(dType)
	{
		case 'h':		// hours
			ti->tm_hour += dLength;
			break;
		case 'd':		// days
			ti->tm_mday += dLength;
			break;
		case 'w':		// weeks
			ti->tm_mday += 7 * dLength;
			break;
		case 'm':		// months
			ti->tm_mon += dLength;
			break;
		case 'y':		// years
			// are leap years considered ? do we care ?
			ti->tm_year += dLength;
			break;
		default:		// minutes
			ti->tm_min += dLength;
			break;
	}
	return mktime(ti);
}
int32 GetTimePeriodFromString(const char * str)
{
	uint32 time_to_ban = 0;
	char * p = (char*)str;
	uint32 multiplier;
	string number_temp;
	uint32 multipliee;
	number_temp.reserve(10);

	while(*p != 0)
	{
		// always starts with a number.
		if(!isdigit(*p))
			break;

		number_temp.clear();
		while(isdigit(*p) && *p != 0)
		{
			number_temp += *p;
			++p;
		}

		// try and find a letter
		if(*p == 0)
			break;

		// check the type
		switch(tolower(*p))
		{
		case 'y':
			multiplier = TIME_YEAR;		// eek!
			break;

		case 'm':
			multiplier = TIME_MONTH;
			break;

		case 'd':
			multiplier = TIME_DAY;
			break;

		case 'h':
			multiplier = TIME_HOUR;
			break;

		default:
			return -1;
			break;
		}

		++p;
		multipliee = atoi(number_temp.c_str());
		time_to_ban += (multiplier * multipliee);
	}

	return time_to_ban;
}

const char * szDayNames[7] = {
	"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};

const char * szMonthNames[12] = {
	"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"
};

void MakeIntString(char * buf, int num)
{
	if(num<10)
	{
		buf[0] = '0';
		//itoa(num, &buf[1], 10);
		sprintf(&buf[1], "%u", num);
	}
	else
	{
		//itoa(num,buf,10);
		sprintf(buf,"%u",num);
	}
}

void MakeIntStringNoZero(char * buf, int num)
{
	//itoa(num,buf,10);
	sprintf(buf,"%u",num);
}

string ConvertTimeStampToString(uint32 timestamp)
{
	int seconds = (int)timestamp;
	int mins=0;
	int hours=0;
	int days=0;
	int months=0;
	int years=0;
	if(seconds >= 60)
	{
		mins = seconds / 60;
		if(mins)
		{
			seconds -= mins*60;
			if(mins >= 60)
			{
				hours = mins / 60;
				if(hours)
				{
					mins -= hours*60;
					if(hours >= 24)
					{
						days = hours/24;
						if(days)
						{
							hours -= days*24;
							if(days >= 30)
							{
								months = days / 30;
								if(months)
								{
									days -= months*30;
									if(months >= 12)
									{
										years = months / 12;
										if(years)
										{
											months -= years*12;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	char szTempBuf[100];
	string szResult;

	if(years) {
		MakeIntStringNoZero(szTempBuf, years);
		szResult += szTempBuf;
		szResult += " years, ";
	}

	if(months) {
		MakeIntStringNoZero(szTempBuf, months);
		szResult += szTempBuf;
		szResult += " months, ";
	}

	if(days) {
		MakeIntStringNoZero(szTempBuf, days);
		szResult += szTempBuf;
		szResult += " days, ";
	}

	if(hours) {
		MakeIntStringNoZero(szTempBuf, hours);
		szResult += szTempBuf;
		szResult += " hours, ";
	}

	if(mins) {
		MakeIntStringNoZero(szTempBuf, mins);
		szResult += szTempBuf;
		szResult += " minutes, ";
	}

	if(seconds) {
		MakeIntStringNoZero(szTempBuf, seconds);
		szResult += szTempBuf;
		szResult += " seconds";
	}

	return szResult;
}

string ConvertTimeStampToDataTime(uint32 timestamp)
{
	char szTempBuf[100];
	time_t t = (time_t)timestamp;
	tm * pTM = localtime(&t);

	string szResult;
	szResult += szDayNames[pTM->tm_wday];
	szResult += ", ";

	MakeIntString(szTempBuf, pTM->tm_mday);
	szResult += szTempBuf;
	szResult += " ";

	szResult += szMonthNames[pTM->tm_mon];
	szResult += " ";

	MakeIntString(szTempBuf, pTM->tm_year+1900);
	szResult += szTempBuf;
	szResult += ", ";
	MakeIntString(szTempBuf, pTM->tm_hour);
	szResult += szTempBuf;
	szResult += ":";
	MakeIntString(szTempBuf, pTM->tm_min);
	szResult += szTempBuf;
	szResult += ":";
	MakeIntString(szTempBuf, pTM->tm_sec);
	szResult += szTempBuf;

	return szResult;
}

//time_t TimeToGametime(time_t unixtime)	-> cayse time_t is not always 32 bit ?
unsigned int TimeToGametime(time_t unixtime)
{
    /* Some minor documentation about the time field
    // MOVE THIS DOCUMENTATION TO THE WIKI
    
    minute's = 0x0000003F                  00000000000000000000000000111111
    hour's   = 0x000007C0                  00000000000000000000011111000000
    weekdays = 0x00003800                  00000000000000000011100000000000
    days     = 0x000FC000                  00000000000011111100000000000000
    months   = 0x00F00000                  00000000111100000000000000000000
    years    = 0x1F000000                  00011111000000000000000000000000
    unk	     = 0xE0000000                  11100000000000000000000000000000
    */

//	time_t gameTime;
	uint32 gameTime;
	time_t basetime = unixtime;

//	time_t minutes = sWorld.GetGameTime( ) / 60;
//	time_t hours = minutes / 60; 
//	minutes %= 60;
	time_t minutes = ((unixtime / 60) % 60); //unixtime is in seconds
	time_t hours = ((unixtime / (60*60)) % 60);

	struct tm * timeinfo = localtime(&basetime);
	uint32 DayOfTheWeek;
	if(timeinfo->tm_wday == 0)
		DayOfTheWeek = 6;					//	It's Sunday
	else
		DayOfTheWeek = timeinfo->tm_wday - 1;		//	0b111 = (any) day, 0 = Monday ect)
	uint32 DayOfTheMonth = timeinfo->tm_mday - 1;	//	Day - 1 (0 is actual 1) its now the 20e here.
	uint32 CurrentMonth = timeinfo->tm_mon;			//	Month - 1 (0 is actual 1) same as above.
	uint32 CurrentYear = timeinfo->tm_year - 100;	//	2000 + this number results in a correct value for this crap.

	#define MINUTE_BITMASK      0x0000003F
    #define HOUR_BITMASK        0x000007C0
    #define WEEKDAY_BITMASK     0x00003800
    #define DAY_BITMASK         0x000FC000
    #define MONTH_BITMASK       0x00F00000
    #define YEAR_BITMASK        0x1F000000
    #define UNK_BITMASK         0xE0000000

    #define MINUTE_SHIFTMASK    0
    #define HOUR_SHIFTMASK      6
    #define WEEKDAY_SHIFTMASK   11
    #define DAY_SHIFTMASK       14
    #define MONTH_SHIFTMASK     20
    #define YEAR_SHIFTMASK      24
    #define UNK_SHIFTMASK       29

    gameTime = ((minutes << MINUTE_SHIFTMASK) & MINUTE_BITMASK);
    gameTime|= ((hours << HOUR_SHIFTMASK) & HOUR_BITMASK);
    gameTime|= ((DayOfTheWeek << WEEKDAY_SHIFTMASK) & WEEKDAY_BITMASK);
    gameTime|= ((DayOfTheMonth << DAY_SHIFTMASK) & DAY_BITMASK);
    gameTime|= ((CurrentMonth << MONTH_SHIFTMASK) & MONTH_BITMASK);
    gameTime|= ((CurrentYear << YEAR_SHIFTMASK) & YEAR_BITMASK);
	return gameTime;
}

float GetMemUsage()
{
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
	float Last_Ram_Mb_usage = (float)pmc.PagefileUsage;
	Last_Ram_Mb_usage = Last_Ram_Mb_usage / 1024.0f / 1024.0f;
	return Last_Ram_Mb_usage;
}

