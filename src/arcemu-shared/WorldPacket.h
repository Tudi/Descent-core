#ifndef WOWSERVER_WORLDPACKET_H
#define WOWSERVER_WORLDPACKET_H

#include "Common.h"
#include "ByteBuffer.h"

class SERVER_DECL WorldPacket : public ByteBuffer
{
public:
    __inline WorldPacket() : ByteBuffer(), m_opcode(0) { }
    __inline WorldPacket(uint16 opcode, size_t res) : ByteBuffer(res), m_opcode(opcode) { bb_opcode = m_opcode; }
    __inline WorldPacket(size_t res) : ByteBuffer(res), m_opcode(0) { }
    __inline WorldPacket(const WorldPacket &packet) : ByteBuffer(packet), m_opcode(packet.m_opcode) { bb_opcode = m_opcode; }

    //! Clear packet and set opcode all in one mighty blow
    __inline void Initialize(uint16 opcode )
    {
        clear();
        m_opcode = opcode;
    }

    __inline uint16 GetOpcode() const { return m_opcode; }
    __inline void SetOpcode(uint16 opcode) { m_opcode = opcode; }

protected:
    uint16 m_opcode;
};

//simple example how to use it
#define sStackWorldPacket(name, opcode, size) uint8 local_stackbuffer[ size ]; StackWorldPacket name(opcode, local_stackbuffer, size);

#define EXTEND_STACK_BUFFER_HEAP_SIZE		500

//Make sure that stack buffer is large enough or there will be no magic for this whole optimization !
//do not worry much about too large size( make it less then 65k) cause there is no allocation hopefully
class SERVER_DECL StackWorldPacket
{
public:
	//Make sure that stack buffer is large enough or there will be no magic for this whole optimization !
	//do not worry much about too large size( make it less then 65k) cause there is no allocation hopefully
	__inline StackWorldPacket(uint16 opcode,uint8 *stack_buffer, uint32 stack_size)	: m_opcode(opcode), 
//		m_readPos(0), 
		m_writePos(0), m_bufferPointer(stack_buffer), m_heapBuffer(NULL), m_space(stack_size){}

	~StackWorldPacket() { if(m_heapBuffer) free(m_heapBuffer); }

	//! Clear packet and set opcode all in one mighty blow
	void Initialize(const uint16 opcode )
	{
//		m_readPos = 0
		m_writePos = 0;
		m_opcode = opcode;
	}
	ARCEMU_INLINE void clear()
	{
		m_writePos = 0;
//		m_readPos = 0;
	}

	ARCEMU_INLINE uint16 GetOpcode() { return m_opcode; }
	/** Re-allocates the buffer on the heap. This allows it to expand past the original specified size.
	 * This is only a failsafe and should be avoided at all costs, as it is quite heavy. 
	 */
	ARCEMU_INLINE void ReallocateOnHeap()
	{
		// the whole point of a stack buffer is to not go to heap
		ASSERT( false );
		bool need_copy = ( m_heapBuffer == NULL );
		// Reallocate with 200 bytes larger size
		m_space += EXTEND_STACK_BUFFER_HEAP_SIZE;
		m_heapBuffer = (uint8*)realloc(m_heapBuffer, m_space );
		if( need_copy )
			memcpy( m_heapBuffer, m_bufferPointer, m_space - EXTEND_STACK_BUFFER_HEAP_SIZE);
		m_bufferPointer = m_heapBuffer;
	}

	//ex fastpack guid needs this
	ARCEMU_INLINE void AdvanceWrite( int32 size )
	{
		m_writePos += size;
	}

	/** Gets the buffer pointer
	 * @return the buffer pointer
	 */
	ARCEMU_INLINE uint8 * GetBufferPointer() { return m_bufferPointer; }

	/** Gets the write position
	 * @return buffer size
	 */
	ARCEMU_INLINE uint32 GetSize() { return m_writePos; }

	/** Reads sizeof(T) bytes from the buffer
	 * @return the bytes read
	 */
/*	template<typename T>
		T Read()
	{
		if(m_readPos + sizeof(T) >= m_writePos)
			return (T)0;
		T ret = *(T*)&m_bufferPointer[m_readPos];
		m_readPos += sizeof(T);
		return ret;
	}*/

	/** Writes sizeof(T) bytes to the buffer, while checking for overflows.
	 * @param T data The data to be written
	 */
	template<typename T>
		void Write(T data)
	{
		EnsureBufferSize( sizeof(T) );

		*(T*)&m_bufferPointer[m_writePos] = data;
		m_writePos += sizeof(T);
	}

	//write a string to the buffer but make sure we are not going to crash due to bad string
	ARCEMU_INLINE void AppendSafeStr( const char *str, uint32 maxsize )
	{
		EnsureBufferSize( maxsize );
		if( str == NULL )
		{
			Write( uint8(0) );	//write a NULL string
			return;
		}
		uint32 i=0;
		while( str[i] != 0 && i < maxsize )
			i++;
		Write( (const uint8 *)str, i );
		//force terminate the string even if it is not supposed to end here
//		if( str[i] != 0 )
			Write( uint8(0) );
	}

	/** writes x bytes to the buffer, while checking for overflows
	 * @param ptr the data to be written
	 * @param size byte count
	 */
	ARCEMU_INLINE void Write(const uint8 * data, const size_t size)
	{
		EnsureBufferSize( size );

		memcpy(&m_bufferPointer[m_writePos], data, size);
		m_writePos += uint32(size);
	}

	//write 1 single value at some specific location
	ARCEMU_INLINE void WriteAtPos( const uint8 data, const uint32 pos )
	{
		if( pos < m_space )
			m_bufferPointer[ pos ] = data;
	}
	ARCEMU_INLINE void WriteAtPos( const uint32 data, const uint32 pos )
	{
		if( pos + 3 < m_space )
			*((uint32*)&m_bufferPointer[ pos ]) = data;
	}


	/** Ensures the buffer is big enough to fit the specified number of bytes.
	 * @param bytes number of bytes to fit
	 */
	ARCEMU_INLINE void EnsureBufferSize(const uint32 Bytes)
	{
		while( m_writePos + Bytes >= m_space )
			ReallocateOnHeap();
	}

	/** These are the default read/write operators.
	 */
//#define SWP_DEFINE_BUFFER_READ_OPERATOR(type) void operator >> (type& dest) { dest = Read<type>(); }
#define SWP_DEFINE_BUFFER_WRITE_OPERATOR(type) void operator << (type src) { Write<type>(src); }

	/** Fast read/write operators without using the templated read/write functions.
	 */
//#define SWP_DEFINE_FAST_READ_OPERATOR(type, size) StackWorldPacket& operator >> (type& dest) { if(m_readPos + size > m_writePos) { dest = (type)0; } else { dest = *(type*)&m_bufferPointer[m_readPos]; m_readPos += size; } return *this;  }
#define SWP_DEFINE_FAST_WRITE_OPERATOR(type, size) StackWorldPacket& operator << (type src) { while(m_writePos + size >= m_space) { ReallocateOnHeap(); } *(type*)&m_bufferPointer[m_writePos] = src; m_writePos += size; return *this; }

	/** Integer/float r/w operators
	 */
/*	SWP_DEFINE_FAST_READ_OPERATOR(uint64, 8);
	SWP_DEFINE_FAST_READ_OPERATOR(uint32, 4);
	SWP_DEFINE_FAST_READ_OPERATOR(int32, 4);
	SWP_DEFINE_FAST_READ_OPERATOR(uint16, 2);
	SWP_DEFINE_FAST_READ_OPERATOR(uint8, 1);
	SWP_DEFINE_FAST_READ_OPERATOR(float, 4);
	SWP_DEFINE_FAST_READ_OPERATOR(double, 8);*/

	SWP_DEFINE_FAST_WRITE_OPERATOR(uint64, sizeof(uint64));
	SWP_DEFINE_FAST_WRITE_OPERATOR(uint32, sizeof(uint32));
	SWP_DEFINE_FAST_WRITE_OPERATOR(int32, sizeof(int32));
	SWP_DEFINE_FAST_WRITE_OPERATOR(uint16, sizeof(uint16));
	SWP_DEFINE_FAST_WRITE_OPERATOR(uint8, sizeof(uint8));
	SWP_DEFINE_FAST_WRITE_OPERATOR(float, sizeof(float));
	SWP_DEFINE_FAST_WRITE_OPERATOR(double, sizeof(double));

	/** boolean (1-byte) read/write operators
	 */
	SWP_DEFINE_FAST_WRITE_OPERATOR(bool, sizeof(bool));
//	StackWorldPacket& operator >> (bool & dst) { dst = (Read<char>() > 0 ? true : false); return *this; }

	/** string (null-terminated) operators
	 */
	StackWorldPacket& operator << (std::string & value) 
	{ 
		uint32 len = (uint32)value.length() + 1;
		EnsureBufferSize( len ); 
		memcpy(&m_bufferPointer[m_writePos], value.c_str(), len); 
		m_writePos += len; 
		return *this; 
	}
/*	StackWorldPacket& operator >> (std::string & dest)
	{
		dest.clear();
		char c;
		for(;;)
		{
			c = Read<char>();
			if(c == 0) break;
			dest += c;
		}
		return *this;
	}*/

	/** C string (null-terminated) operators
	 */
	StackWorldPacket &operator<<(const char *str)
	{
		if( str == NULL )
			return *this; //maybe write something like 0 ?
		uint32 str_len = (uint32)strlen(str)+1;
		Write((const uint8*)str, str_len);
		return *this;
	}

	/** WoWGuid read/write operators
	 */
	StackWorldPacket& operator << (const WoWGuid & value)
	{
		EnsureBufferSize(value.GetNewGuidLen() + 1);
		Write<uint8>(value.GetNewGuidMask());
		memcpy(&m_bufferPointer[m_writePos], value.GetNewGuid(), value.GetNewGuidLen());
		m_writePos += value.GetNewGuidLen();
		return *this;
	}
	
/*	StackWorldPacket& operator >> (WoWGuid & value)
	{
		uint8 mask = Read<uint8>();
		value.Init(mask);
		uint8 masklen = BitCount8(mask);
		for(int32 i = 0; i < masklen; ++i)
			value.AppendField(Read<uint8>());
		return *this;
	}*/

	/** LocationVector read/write operators
	 */
	StackWorldPacket& operator << (LocationVector & val)
	{
		// espire: I would've done this as one memcpy.. but we don't know how the struct alignment is gonna come out :/
		Write<float>(val.x);
		Write<float>(val.y);
		Write<float>(val.z);
		return *this;
	}

/*	StackWorldPacket& operator >> (LocationVector & dst)
	{
		dst.x = Read<float>();
		dst.y = Read<float>();
		dst.z = Read<float>();
		return *this;
	}*/

protected:
	uint16	m_opcode;
//    uint32	m_readPos;
    uint32	m_writePos;
	uint8	*m_bufferPointer;		//this is a pointer on the stack or heap in case we failed to provide enough buffer fro write
	uint8	*m_heapBuffer;
	uint32	m_space;
};

#endif
