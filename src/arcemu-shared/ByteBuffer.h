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

#ifndef _BYTEBUFFER_H
#define _BYTEBUFFER_H

#include "Common.h"
#include "WoWGuid.h"
#include "LocationVector.h"

//enable this only if you suspect some packet is creating memory corruptions due that content is larger then buffer
//#define ENABLE_INTERNAL_BOUNCE_CHECKER
#ifdef ENABLE_INTERNAL_BOUNCE_CHECKER
	#define BBwprintf(opcode,pos,maxsize) \
	{	\
		FILE *Bounce_checker_out; \
		Bounce_checker_out = fopen("bouncecheckercatches.txt","a"); \
		if( Bounce_checker_out ) \
		{ \
			fprintf(Bounce_checker_out,"opcode %03X writing at pos %u when size is %u\n",opcode,pos,maxsize); \
			fclose( Bounce_checker_out ); \
		} \
	}
	#define BBrprintf(opcode,pos,maxsize) \
	{	\
		FILE *Bounce_checker_out; \
		Bounce_checker_out = fopen("bouncecheckercatches.txt","a"); \
		if( Bounce_checker_out ) \
		{ \
			fprintf(Bounce_checker_out,"opcode %03X reading at pos %u when size is %u\n",opcode,pos,maxsize); \
			fclose( Bounce_checker_out ); \
		} \
	}
#else
	#define BBwprintf
	#define BBrprintf
#endif

class SERVER_DECL ByteBuffer {
public:
	class error {
	};

	const static size_t DEFAULT_SIZE = 0x1000;
	const static size_t MIN_RESIZE = 500; //practical results show that packets that resize resize a lot of times. Avoid spamming these

	ByteBuffer(): _rpos(0), _wpos(0)
	{
		_storage.reserve(DEFAULT_SIZE);
		bb_opcode = 0;
		_bitpos = 8;
		_curbitval = 0;
	}
	ByteBuffer(size_t res): _rpos(0), _wpos(0)
	{
		_storage.reserve(res);
		bb_opcode = 0;
		_bitpos = 8;
		_curbitval = 0;
	}
	ByteBuffer(const ByteBuffer &buf): _rpos(buf._rpos), _wpos(buf._wpos), _storage(buf._storage), _bitpos(buf._bitpos), _curbitval(buf._curbitval) { }
	virtual ~ByteBuffer() {}

	void clear() 
	{
		_storage.clear();
		_rpos = _wpos = 0;
		_bitpos = 8;
		_curbitval = 0;
	}

	//template <typename T> void insert(size_t pos, T value) {
	//  insert(pos, (uint8 *)&value, sizeof(value));
	//}
	template <typename T> void append(T value)
	{
		flushBits();
		append((uint8 *)&value, sizeof(value));
	}
    void flushBits()
    {
        if (_bitpos == 8)
            return;

        append((uint8 *)&_curbitval, sizeof(uint8));
        _curbitval = 0;
        _bitpos = 8;
    }

    bool writeBit(uint32 bit)
    {
        --_bitpos;
        if (bit)
            _curbitval |= (1 << (_bitpos));

        if (_bitpos == 0)
        { 
            _bitpos = 8;
            append((uint8 *)&_curbitval, sizeof(_curbitval));
            _curbitval = 0;
        }

        return (bit != 0);
    }
 
    template <typename T> void writeBits(T value, size_t bits)
    {
        for (int32 i = bits-1; i >= 0; --i)
            writeBit((value >> i) & 1);
    }

    bool readBit()
    {
        ++_bitpos;
        if (_bitpos > 7)
        { 
            _bitpos = 0;
            _curbitval = read<uint8>();
        }
        bool bit = ((_curbitval >> (7-_bitpos)) & 1) != 0;
        return bit;
    }
    void ReadByteMask(uint8& b)
    {
        b = readBit() ? 1 : 0;
    }
    void ReadByteSeq(uint8& b)
    {
        if (b != 0)
            b ^= read<uint8>();
    }

    void WriteByteMask(uint8 b)
    {
        writeBit(b);
    }
    void WriteByteSeq(uint8 b)
    {
        if (b != 0)
            append<uint8>(b ^ 1);
    }

    uint32 readBits(size_t bits)
    {
        uint32 value = 0;
        for (int32 i = bits-1; i >= 0; --i)
        {
            if(readBit())
            {
                value |= (1 << i);
            }
        }
        return value;
    }

	template <typename T> void put(size_t pos,T value)
	{
		put(pos,(uint8 *)&value,sizeof(value));
	}

	// stream like operators for storing data
	ByteBuffer &operator<<(bool value) 
	{
		append<char>((char)value);
		return *this;
	}
	// unsigned
	ByteBuffer &operator<<(uint8 value)
	{
		append<uint8>(value);
		return *this;
	}
	ByteBuffer &operator<<(uint16 value)
	{
		append<uint16>(value);
		return *this;
	}
	ByteBuffer &operator<<(uint32 value)
	{
		append<uint32>(value);
		return *this;
	}
	ByteBuffer &operator<<(uint64 value) 
	{
		append<uint64>(value);
		return *this;
	}
	// signed as in 2e complement
	ByteBuffer &operator<<(int8 value)
	{
		append<int8>(value);
		return *this;
	}
	ByteBuffer &operator<<(int16 value)
	{
		append<int16>(value);
		return *this;
	}
	ByteBuffer &operator<<(int32 value)
	{
		append<int32>(value);
		return *this;
	}
	ByteBuffer &operator<<(int64 value)
	{
		append<int64>(value);
		return *this;
	}
	ByteBuffer &operator<<(float value)
	{
		append<float>(value);
		return *this;
	}
	ByteBuffer &operator<<(double value)
	{
		append<double>(value);
		return *this;
	}
	ByteBuffer &operator<<(const std::string &value)
	{
		append((uint8 *)value.c_str(), value.length());
		append((uint8)0);
		return *this;
	}
	ByteBuffer &operator<<(const char *str)
	{
		append((uint8 *)str, strlen(str));
		append((uint8)0);
		return *this;
	}
	ByteBuffer &operator<<(const WoWGuid &value)
	{
		append<uint8>(value.GetNewGuidMask());
		append((uint8 *)value.GetNewGuid(), value.GetNewGuidLen());
		return *this;
	}
	ByteBuffer &operator<<(ByteBuffer &src)
	{
		if( src.wpos() )
			append( src.contents(), src.wpos() );
		return *this;
	}

	// stream like operators for reading data
	ByteBuffer &operator>>(bool &value)
	{
		value = read<char>() > 0 ? true : false;
		return *this;
	}
	//unsigned
	ByteBuffer &operator>>(uint8 &value)
	{
		value = read<uint8>();
		return *this;
	}
	ByteBuffer &operator>>(uint16 &value)
	{
		value = read<uint16>();
		return *this;
	}
	ByteBuffer &operator>>(uint32 &value)
	{
		value = read<uint32>();
		return *this;
	}
	ByteBuffer &operator>>(uint64 &value)
	{
		value = read<uint64>();
		return *this;
	}
	//signed as in 2e complement
	ByteBuffer &operator>>(int8 &value)
	{
		value = read<int8>();
		return *this;
	}
	ByteBuffer &operator>>(int16 &value)
	{
		value = read<int16>();
		return *this;
	}
	ByteBuffer &operator>>(int32 &value)
	{
		value = read<int32>();
		return *this;
	}
	ByteBuffer &operator>>(int64 &value)
	{
		value = read<int64>();
		return *this;
	}
	ByteBuffer &operator>>(float &value)
	{
		value = read<float>();
		return *this;
	}
	ByteBuffer &operator>>(double &value)
	{
		value = read<double>();
		return *this;
	}
	ByteBuffer &operator>>(std::string& value)
	{
		size_t _Size = _storage.size();
		if (_rpos < _Size)
		{
			const char *_Str = (const char *)&_storage[_rpos];
			char *_Ptr = (char *)_Str, *_End = (_Ptr - _rpos) + _Size;
			while (true)
			{
				if (_Ptr < _End)
				{
					if (*_Ptr++ != 0)
						continue;
					else
						break;
				}
				else
				{
					printf("string read with no null terminator\n");
					++_Ptr;
					break;
				}
			}
			value.assign(_Str, size_t(_Ptr - _Str) - 1);
			_rpos += size_t(_Ptr - _Str);
			return *this;
		}
		else
		{
			value.clear();
			return *this;
		}
	}
	//! Only does X,Y,Z!
	ByteBuffer & operator << (const LocationVector & vec)
	{
		append<float>(vec.x);
		append<float>(vec.y);
		append<float>(vec.z);
		return *this;
	}

	//! Only does X,Y,Z!
	ByteBuffer & operator >> (LocationVector & vec)
	{
		vec.x = read<float>();
		vec.y = read<float>();
		vec.z = read<float>();
		return * this;
	}

	ByteBuffer &operator>>(WoWGuid &value) 
	{
		uint8 field, mask = read<uint8>();
		value.Init((uint8)mask);
		for(int i = 0; i < BitCount8(mask); i++)
		{
			field = read<uint8>();
			value.AppendField(field);
		}
		return *this;
	}

	uint8 operator[](size_t pos) 
	{
		return read<uint8>(pos);
	}

	size_t rpos()
	{
		return _rpos;
	};

	size_t rpos(size_t rpos) 
	{
		_rpos = rpos;
		return _rpos;
	};

	size_t wpos() 
	{
		return _wpos;
	}

	size_t wpos(size_t wpos)
	{
		_wpos = wpos;
		return _wpos;
	}

	template <typename T> T read()
	{
		T r=read<T>(_rpos);
		_rpos += sizeof(T);
		return r;
	};
	template <typename T> T read(size_t pos) const
	{
		//ASSERT(pos + sizeof(T) <= size());
		if(pos + sizeof(T) > size())
		{
			BBrprintf(bb_opcode,_rpos,size());
			return (T)0;
		} 
		else 
		{
			return *((T*)&_storage[pos]);
		}
	}

	void read(uint8 *dest, size_t len)
	{
		if (_rpos + len <= size())
		{
			memcpy(dest, &_storage[_rpos], len);
		} else 
		{
			//throw error();
			BBrprintf(bb_opcode,_rpos,size());
			memset(dest, 0, len);
		}
		_rpos += len;
	}

	const uint8 *contents() const { return &_storage[0]; };

	ARCEMU_INLINE size_t size() const { return _storage.size(); };
	ARCEMU_INLINE size_t wsize() const { return _wpos; };
	// one should never use resize probably
	void resize(size_t newsize)
	{
		_storage.resize( newsize );
		_rpos = 0;
		_wpos = size();
	};
	void reserve(size_t ressize)
	{
		if (ressize > size()) _storage.reserve(ressize);
	};

		// appending to the end of buffer
	void append(const std::string& str)
	{
		append((uint8 *)str.c_str(),str.size() + 1);
	}
	void append(const char *src, size_t cnt)
	{
		return append((const uint8 *)src, cnt);
	}
	void append(const uint8 *src, size_t cnt)
	{
		if (!cnt) return;

		// noone should even need uint8buffer longer than 10mb
		// if you DO need, think about it
		// then think some more
		// then use something else
		// -- qz
		ASSERT(size() < 10000000);

		if (_storage.size() < _wpos + cnt)
		{
			BBwprintf(bb_opcode,_wpos,_storage.size());
			_storage.resize(_wpos + cnt);
		}
		memcpy(&_storage[_wpos], src, cnt);
		_wpos += cnt;
	}
	void append(const ByteBuffer& buffer)
	{
		if(buffer.size() > 0) append(buffer.contents(),buffer.size());
	}
	void append_int24(const uint32 val)
	{
		append((const char*)&val,3);
	}

	void appendPackGUID(uint64 guid)
        {
			if (_storage.size() < _wpos + 8)
			{
				BBwprintf(bb_opcode,_wpos,_storage.size());
				_storage.resize(_wpos + 8);
			}
            size_t mask_position = wpos();
            *this << uint8(0);
            for(uint8 i = 0; i < 8; i++)
            {
                if(guid & 0xFF)
                {
                    _storage[mask_position] |= (1<<i);
                    *this << ((uint8)(guid & 0xFF));
                }

                guid >>= 8;
            }
        }

	void put(size_t pos, const uint8 *src, size_t cnt)
	{
		ASSERT(pos + cnt <= size());
		if (_storage.size() < pos + cnt)
		{
			BBwprintf(bb_opcode,pos,_storage.size());
			_storage.resize(_storage.size() + cnt);
		}
		memcpy(&_storage[pos], src, cnt);
	}

	//write 1 single value at some specific location
	ARCEMU_INLINE void WriteAtPos( const uint8 data, const uint32 pos )
	{
		put( pos, &data, 1 );
	}
	ARCEMU_INLINE void WriteAtPos( const uint32 data, const uint32 pos )
	{
		put( pos, (const uint8 *)&data, 4 );
	}

	//void insert(size_t pos, const uint8 *src, size_t cnt) {
	//  std::copy(src, src + cnt, inserter(_storage, _storage.begin() + pos));
	//}

	void hexlike()
	{
		uint32 j = 1, k = 1;
		printf("STORAGE_SIZE: %u\n", (unsigned int)size() );
		for(uint32 i = 0; i < size(); i++)
		{
			if ((i == (j*8)) && ((i != (k*16))))
			{
				if (read<uint8>(i) <= 0x0F)
				{
					printf("| 0%X ", read<uint8>(i) );
				}
				else
				{
					printf("| %X ", read<uint8>(i) );
				}

				j++;
			}
			else if (i == (k*16))
			{
				rpos(rpos()-16);	// move read pointer 16 places back
				printf(" | ");	  // write split char

				for (int x = 0; x < 16; x++)
				{
					printf("%c", read<uint8>(i-16 + x) );
				}

				if (read<uint8>(i) <= 0x0F)
				{
					printf("\n0%X ", read<uint8>(i) );
				}
				else
				{
					printf("\n%X ", read<uint8>(i) );
				}

				k++;
				j++;
			}
			else
			{
				if (read<uint8>(i) <= 0x0F)
				{
					printf("0%X ", read<uint8>(i) );
				}
				else
				{
					printf("%X ", read<uint8>(i) );
				}
			}
		}
		printf("\n");
	}

	/* input is a string containing hex bytes separated by " ". Null terminated */
	void CopyFromSnifferDump(char *content)
	{
		uint32 pindex = 0;
		while( content[ pindex ] != 0 )
		{
			//convert hex to decimal
			uint8 dec = (uint8)ByteHexStrToDec( &content[ pindex ] );
			*this << dec;
			pindex++;	//hex symbol 1
			if( content[ pindex ] == 0 ) break;
			pindex++;	//hex symbol 2
			if( content[ pindex ] == 0 ) break;
			pindex++;	//space to separate
		}
	}

	ARCEMU_INLINE void reverse()
	{
		std::reverse(_storage.begin(), _storage.end());
	}

	ARCEMU_INLINE void ResetRead()
	{
		_rpos = 0;
	}
	ARCEMU_INLINE void read_skip(uint32 byte_count)
	{
		_rpos += byte_count;
	}

	uint32 bb_opcode; //this is only used for debuging
protected:
	// read and write positions
	size_t _rpos, _wpos, _bitpos;
	uint8 _curbitval;
	std::vector<uint8> _storage;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <typename T> ByteBuffer &operator<<(ByteBuffer &b, std::vector<T> v)
{
	b << (uint32)v.size();
	for (typename std::vector<T>::iterator i = v.begin(); i != v.end(); i++) {
		b << *i;
	}
	return b;
}

template <typename T> ByteBuffer &operator>>(ByteBuffer &b, std::vector<T> &v)
{
	uint32 vsize;
	b >> vsize;
	v.clear();
	while(vsize--)
	{
		T t;
		b >> t;
		v.push_back(t);
	}
	return b;
}

template <typename T> ByteBuffer &operator<<(ByteBuffer &b, std::list<T> v)
{
	b << (uint32)v.size();
	for (typename std::list<T>::iterator i = v.begin(); i != v.end(); i++) 
	{
		b << *i;
	}
	return b;
}

template <typename T> ByteBuffer &operator>>(ByteBuffer &b, std::list<T> &v)
{
	uint32 vsize;
	b >> vsize;
	v.clear();
	T t;
	while(vsize--)
	{
		b >> t;
		v.push_back(t);
	}
	return b;
}

template <typename K, typename V> ByteBuffer &operator<<(ByteBuffer &b, std::map<K, V> &m)
{
	b << (uint32)m.size();
	for (typename std::map<K, V>::iterator i = m.begin(); i != m.end(); i++)
	{
		b << i->first << i->second;
	}
	return b;
}

template <typename K, typename V> ByteBuffer &operator>>(ByteBuffer &b, std::map<K, V> &m)
{
	uint32 msize;
	b >> msize;
	m.clear();
	while(msize--) 
	{
		K k;
		V v;
		b >> k >> v;
		m.insert(make_pair(k, v));
	}
	return b;
}

#endif
