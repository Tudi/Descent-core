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

#if !defined(FIELD_H)
#define FIELD_H

class Field
{
public:

	ARCEMU_INLINE void SetValue(char* value) { mValue = value; }

	ARCEMU_INLINE const char *GetString() { return mValue; }
	ARCEMU_INLINE float GetFloat() { return mValue ? (float)(atof(mValue)) : 0; }
	ARCEMU_INLINE bool GetBool() { return mValue ? atoi(mValue) > 0 : false; }
	ARCEMU_INLINE uint8 GetUInt8() { return mValue ? (uint8)(atol(mValue)) : 0; }
	ARCEMU_INLINE int8 GetInt8() { return mValue ? (int8)(atol(mValue)) : 0; }
	ARCEMU_INLINE uint16 GetUInt16() { return mValue ? (uint16)(atol(mValue)) : 0; }
	ARCEMU_INLINE uint16 GetInt16() { return mValue ? (int16)(atol(mValue)) : 0; }
	ARCEMU_INLINE uint32 GetUInt32() { return mValue ? (uint32)(atol(mValue)) : 0; }
	ARCEMU_INLINE uint32 GetInt32() { return mValue ? (int32)(atol(mValue)) : 0; }
	uint64 GetUInt64() 
	{
		if(mValue)
		{
			uint64 value;
			#if (!defined( WIN32 ) && !defined( WIN64 ) )	// Make GCC happy.
			sscanf(mValue,I64FMTD,(long long unsigned int*)&value);
			#else
			sscanf(mValue, I64FMTD, &value);
			#endif
			return value;
		}
		else
			return 0;
	}

private:
		char *mValue;
};

#endif
