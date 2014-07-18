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
// Class WorldSocket - Main network code functions, handles
// reading/writing of all packets.

#include "StdAfx.h"
#include "AuthCodes.h"

/* echo send/received packets to console */
//#define ECHO_PACKET_LOG_TO_CONSOLE 1

#ifndef CLUSTERING
#pragma pack(push, 1)
struct ClientPktHeader
{
	uint16 size;
	uint32 cmd;
};

struct ServerPktHeader
{
	uint16 size;
	uint16 cmd;
};

struct ServerPktHeaderSmall
{
	uint16 size;
};

struct ServerPktHeaderLarge
{
	uint8  size_extension;
	uint16 size;
	uint16 cmd;
};
#pragma pack(pop)

WorldSocket::WorldSocket(SOCKET fd) : Socket(fd, sWorld.SocketSendBufSize, sWorld.SocketRecvBufSize)
{
	Authed = false;
	mSize = mOpcode = mRemaining = 0;
	_latency = 0;
	mSession = NULL;
	mSeed = 0x9B6B99 + RandomUInt() % 0xFFFF;
	pAuthenticationPacket = NULL;
	mQueued = false;
	mRequestID = 0;
	m_nagleEanbled = false;
	m_fullAccountName = NULL;
#ifdef USE_PACKET_COMPRESSION
	m_CompressedPacketsInputBuffer.reserve( WORLDSOCKET_SENDBUF_SIZE );
	m_CompressedPacketsInBuffer = 0;
#endif
}

WorldSocket::~WorldSocket()
{
	WorldPacket * pck;
	while((pck = _queue.Pop()) != 0)
		delete pck;
	_queue.Clear();	//this is already empty !

	if(pAuthenticationPacket)
	{
		delete pAuthenticationPacket;
		pAuthenticationPacket = NULL;
	}

	if(mSession)
	{
		mSession->SetSocket(NULL);
		mSession=NULL;
	}

	if( m_fullAccountName != NULL )
	{
		delete m_fullAccountName;
		m_fullAccountName = NULL;
	}
}

void WorldSocket::OnDisconnect()
{
	if(mSession)
	{
		mSession->SetSocket( NULL );
		mSession=NULL;
	}

	if(mRequestID != 0)
	{
		sLogonCommHandler.UnauthedSocketClose(mRequestID);
		mRequestID = 0;
	}

	if(mQueued)
	{
		sWorld.RemoveQueuedSocket(this);	// Remove from queued sockets.
		mQueued=false;
	}
}

void WorldSocket::OutPacket(uint16 opcode, size_t len, const void* data, bool skip_compression)
{
	OUTPACKET_RESULT res;
	if( (len + 10) > WORLDSOCKET_SENDBUF_SIZE )
	{
		Log.Error("WorldSocket", "WARNING: Tried to send a packet of %u bytes (which is too large) to a socket. Opcode was: %u (0x%03X)\n", (unsigned int)len, (unsigned int)opcode, (unsigned int)opcode);
		return;
	}

#if defined( _DEBUG ) && !defined( X64 )
	if( opcode == 0 )
	{
		Log.Notice("WorldSocket", "Trying to send a packet without opcode(break here for debug)");
		return;
	}
#endif

	//can we/should we queue this as a compressed packet ?
#ifdef USE_PACKET_COMPRESSION
	if( 
//		len > 0 && 
		len <= (65535-2)	//because len is represented on 2 bytes
		&& len <= (WORLDSOCKET_SENDBUF_SIZE-10) 
//		&& GetLatency() >= USE_PACKET_COMPRESSION 
		&& mSession && mSession->GetPlayer() && mSession->GetPlayer()->IsInWorld()
//		&& mSession && mSession->GetPlayer()
		&& skip_compression == false 
		&& WorldPacketHandlers[ opcode ].skip_compress == 0
		)
	{
		queueLock.Acquire();
		//if appending this next buffer will exceed our socket send buffer before we enter update loop then we try to send first tehn recheck if we can add it
		if( m_CompressedPacketsInputBuffer.size() + (len + 10) > WORLDSOCKET_SENDBUF_COMPRESSED_SEND )
			UpdateCompressedPackets();
		//check if we can queue it now
		if( m_CompressedPacketsInputBuffer.size() + (len + 10) < WORLDSOCKET_SENDBUF_SIZE )
		{
			m_CompressedPacketsInputBuffer << uint16( len + 2 );
			m_CompressedPacketsInputBuffer << uint16( opcode );
			m_CompressedPacketsInputBuffer.append( (const char*)data, len );
			m_CompressedPacketsInBuffer++;
			queueLock.Release();
			return;
		}
		//if we got here it means that for some reason we was not able to queue it and we should try to send it normally or queue it normally
		queueLock.Release();
	}
#endif

	res = _OutPacket(opcode, len, data);

	//try to send it later
	if(res == OUTPACKET_RESULT_NO_ROOM_IN_BUFFER)
	{
		queueLock.Acquire();
		WorldPacket * pck = new WorldPacket(opcode, len);
		if(len) 
			pck->append((const uint8*)data, len);
		_queue.Push(pck);
		queueLock.Release();
	}
}

#ifdef USE_PACKET_COMPRESSION
void WorldSocket::UpdateCompressedPackets(bool nolock)
{
	if( nolock == false )
		queueLock.Acquire();

	if( 
//		m_CompressedPacketsInputBuffer.empty() == true || 
		m_CompressedPacketsInBuffer == 0 )
	{
		if( nolock == false )
			queueLock.Release();
		return;
	}


	//there is no point in compressing less then 200 bytes. Zlib simply cannot compress such a small size
//	if( m_CompressedPacketsInputBuffer.size() < 200 && m_CompressedPacketsInBuffer < 5 )
//	if( m_CompressedPacketsInputBuffer.size() * m_CompressedPacketsInBuffer < 200 )	//if there are multiple small packets then maybe it is wiser to just send them all as 1 packet. Not dead sure how this impacts network. I hope it has a positiv effect. In theory it has none.
	if( m_CompressedPacketsInputBuffer.size() < sWorld.compression_threshold )	//if there are multiple small packets then maybe it is wiser to just send them all as 1 packet. Not dead sure how this impacts network. I hope it has a positiv effect. In theory it has none.
	{
		//send the packets 1 by 1 as non compressed
		uint8 *next_content = (uint8 *)m_CompressedPacketsInputBuffer.contents(); //1 is size, 2 is opcode
		//might be outdated since size changed from 1 to 2 bytes
		if( m_CompressedPacketsInBuffer == 1 )
		{
			for(uint32 i=0;i<m_CompressedPacketsInBuffer;i++)
			{
				uint16 opcode = *((uint16*)(&next_content[2]));
				size_t len = (size_t)*((uint16*)(&next_content[0])) - 2;
				const void* data = (const void*)(next_content+4);
				OutPacket( opcode, len, data, true );
				next_content = next_content + len + 2 + 2;
			}/**/
		}
		else
		{
			OutPacket( SMSG_UNCOMPRESSED_MOVES, m_CompressedPacketsInputBuffer.size(), next_content );
		}
		m_CompressedPacketsInputBuffer.clear();
		m_CompressedPacketsInBuffer = 0;
		if( nolock == false )
			queueLock.Release();
		return;
	}
	m_CompressedPacketsInBuffer = 0;

	uint32 size = (uint32)m_CompressedPacketsInputBuffer.wpos();
	uint32 destsize = size + size/10 + 48;

	int rate = sWorld.getIntRate(INTRATE_COMPRESSION);
	if( rate > Z_BEST_COMPRESSION )
		rate = Z_BEST_COMPRESSION;
	else if( rate < Z_DEFAULT_COMPRESSION )
		rate = Z_DEFAULT_COMPRESSION;
	if( size <= sWorld.compression_threshold )
		rate = Z_NO_COMPRESSION;			// don't bother compressing packet smaller than this, zlib doesnt really handle them well

	// set up stream
	z_stream stream;
	stream.zalloc = 0;
	stream.zfree  = 0;
	stream.opaque = 0;

	if( deflateInit(&stream, rate) != Z_OK )
	{
		sLog.outError("deflateInit for compressed packets failed.");
		if( nolock == false )
			queueLock.Release();
		return;
	}

	uint8 *buffer = &m_CompressedPacketsOutBuffer[0];

	// set up stream pointers
	stream.next_out  = (Bytef*)buffer+4;
	stream.avail_out = destsize;
	stream.next_in   = (Bytef*)m_CompressedPacketsInputBuffer.contents();
	stream.avail_in  = size;

	// call the actual process
	if( deflate(&stream, Z_NO_FLUSH) != Z_OK 
		|| stream.avail_in != 0 
		|| deflate(&stream, Z_FINISH) != Z_STREAM_END	// finish the deflate
		|| deflateEnd(&stream) != Z_OK //finish up
		)
	{
		sLog.outError("deflate for compressed packets failed.");
		//no point entering an error loop, just dump the errored data :(
		m_CompressedPacketsInputBuffer.clear();
		if( nolock == false )
			queueLock.Release();
		return;
	}

	// fill in the full size of the compressed stream
	*(uint32*)&buffer[0] = size;

	// send it and skip readding it to compressed buffer ;)
	//no no no, use the other send way 
	//_OutPacket( SMSG_COMPRESSED_MOVES, (uint16)stream.total_out + 4, buffer );

	//we sent it, move on kk 
	m_CompressedPacketsInputBuffer.clear();

	if( nolock == false )
		queueLock.Release();

	//send it the normal way, in case we cannot add it to socket buffer then queue sending it
	//send it after releasing mutex to avoid looping lock..though nobody cares
	OutPacket( SMSG_COMPRESSED_MOVES, (uint16)stream.total_out + 4, buffer );
}
#endif

void WorldSocket::UpdateQueuedPackets()
{
	queueLock.Acquire();

#ifdef USE_PACKET_COMPRESSION
	UpdateCompressedPackets( true );
#endif

	if(!_queue.HasItems())
	{
		queueLock.Release();
		return;
	}

	WorldPacket * pck;
	while( (pck = _queue.front()) != 0 )
	{
		// try to push out as many as you can 
		switch(_OutPacket(pck->GetOpcode(), pck->size(), pck->size() ? pck->contents() : NULL))
		{
		case OUTPACKET_RESULT_SUCCESS:
			{
				delete pck;
				_queue.pop_front();
			}break;

		case OUTPACKET_RESULT_NO_ROOM_IN_BUFFER:
			{
				// still connected. Exit this function and wait some more
				queueLock.Release();
				return;
			}break;
		//OUTPACKET_RESULT_NOT_CONNECTED or OUTPACKET_RESULT_SOCKET_ERROR
		default:
			{
				// kill everything in the buffer if we are not connected anymore
				while((pck == _queue.Pop()))
					delete pck;
				queueLock.Release();
				return;
			}break;
		}
	}
	queueLock.Release();
}

OUTPACKET_RESULT WorldSocket::_OutPacket(uint16 opcode, size_t len, const void* data)
{
	bool rv;
	if( !IsConnected() )
	{
		return OUTPACKET_RESULT_NOT_CONNECTED;
	}

	BurstBegin();
	//if((m_writeByteCount + len + 4) >= m_writeBufferSize)
	if( GetWriteBuffer().GetSpace() < (len+10) )
	{
		BurstEnd();
		return OUTPACKET_RESULT_NO_ROOM_IN_BUFFER;
	}

	// Packet logger :)
	sWorldLog.LogPacket((uint32)len, opcode, (const uint8*)data, 1);

	// Encrypt the packet
	// First, create the header.
	// large packet is used when packet is compressed and the uncompressed size will exceed 32k sized byte buffer
	if( // opcode == SMSG_COMPRESSED_UPDATE_OBJECT && //actually any other compressed packet
		len > 0x7FFF )
	{
		ServerPktHeaderLarge Header;
		Header.cmd = opcode;
		Header.size = ntohs((uint16)len + 2);
		Header.size_extension = 0x80 | ( 0xFF & ( len>>16 ) );
		_crypt.EncryptSend((uint8*)&Header, sizeof (ServerPktHeaderLarge));
		// Pass the header to our send buffer
		rv = BurstSend((const uint8*)&Header, sizeof (ServerPktHeaderLarge));
	}
	else /**/
	if( opcode == SMSG_PACKET_WITHOUT_OPCODE )
	{
		ServerPktHeaderSmall Header;
		Header.size = ntohs((uint16)len);
		_crypt.EncryptSend((uint8*)&Header, sizeof (ServerPktHeaderSmall));
		// Pass the header to our send buffer
		rv = BurstSend((const uint8*)&Header, (const uint8*)data, sizeof (ServerPktHeaderSmall), len);
	}
	else
	{
		ServerPktHeader Header;
		Header.cmd = opcode;
		Header.size = ntohs((uint16)len + 2);
		_crypt.EncryptSend((uint8*)&Header, sizeof (ServerPktHeader));
		// Pass the header to our send buffer
		rv = BurstSend((const uint8*)&Header, (const uint8*)data, sizeof (ServerPktHeader), len);
	}

	//send the packet right now
	//!! this might lock for a short while!
	//sending packets 1 by 1 instead burst send at the end of the session update
	if(rv) 
		BurstPush();

	BurstEnd();
	return rv ? OUTPACKET_RESULT_SUCCESS : OUTPACKET_RESULT_NO_ROOM_IN_BUFFER;
}

void WorldSocket::OnConnect()
{
	sWorld.mAcceptedConnections++;
	_latency = getMSTime();

	WorldPacket textpacket(SMSG_PACKET_WITHOUT_OPCODE, 50 );	//!! this packet does not reserve space for opcode !
	//WORLD OF WARCRAFT CONNECTION - SERVER TO CLIENT 
	textpacket << "WORLD OF WARCRAFT CONNECTION - SERVER TO CLIENT";
	SendPacket(&textpacket);

	sStackWorldPacket(data,SMSG_AUTH_CHALLENGE,40+10);
#ifndef CATACLYSM_SUPPORT
    BigNumber seed1;
    seed1.SetRand(16 * 8);
    data.Write(seed1.AsByteArray(), 16);               // new encryption seeds

	data << uint8(1);
	data << mSeed;

    BigNumber seed2;
    seed2.SetRand(16 * 8);
    data.Write(seed2.AsByteArray(), 16);               // new encryption seeds
#else
	/*
	85 2F D9 5F D7 43 A8 48 ?
	01 connection index
	08 B9 C5 E0 seed
	03 67 61 40 8B 80 85 90 FA 14 51 6C 91 6D B1 CB 28 CC 2A 75 E5 DF B8 E9 ?

{SERVER} Packet: (0x8C10) UNKNOWN PacketSize = 37 TimeStamp = 4910693
66 E7 82 ED 90 55 37 54 56 8B B1 52 C4 7C 62 9D 
3D F8 F6 3D 
01 
EB D7 C6 99 B5 A8 87 12 DC 62 B8 27 59 31 98 7D 
	*/
//    BigNumber seed1;
//    seed1.SetRand(16 * 8);
	uint8 pad[16] =  { 0 };
//	data.Write(seed1.AsByteArray(), 16);               // new encryption seeds
	data.Write(pad, 16);               // new encryption seeds

	data << mSeed;
	data << uint8(1);	//connection index (secondary connection before world login is 1)

//    BigNumber seed2;
//    seed2.SetRand(24 * 8);
//    data.Write(seed2.AsByteArray(), 16);               // new encryption seeds		
    data.Write(pad, 16);               // new encryption seeds		
#endif
	SendPacket(&data);
}

void WorldSocket::_HandleAuthSession(WorldPacket* recvPacket)
{
	std::string account;
	_latency = getMSTime() - _latency;

#ifndef CATACLYSM_SUPPORT
	uint32 unk2,unk3;
	uint32	unk4,unk5,unk6,unk7,unk8;
	try
	{
		*recvPacket >> mClientBuild;
		*recvPacket >> unk2;
		*recvPacket >> account;
		*recvPacket >> unk3;
		*recvPacket >> mClientSeed;
		*recvPacket >> unk4;
		*recvPacket >> unk5;
		if( mClientBuild >= 12340 )
			*recvPacket >> unk6 >> unk7 >> unk8;
	}
	catch(ByteBuffer::error &)
	{
		sLog.outDetail("Incomplete copy of AUTH_SESSION Received.");
		return;
	}
#else
	if( recvPacket->size() < 48 )
	{
		sLog.outDetail("Incomplete copy of CMSG_AUTH_SESSION Received.");
		return;
	}
/*
D6 CC 22 E3 01 60 00 00 00 00 9E 00 AA 89 00 00 00 00 04 37 - 20 bytes
01 00 00 00 - 4 bytes
95 33 - client build 13205 
FD A4 8A 83 0C 00 00 00 00 00 00 00 00 E9  - 12 bytes
EC AA C0 78 seed ?
79 00 00 00 00 D4 65 51 - 8 crap
03 01 00 00 - addonsize (259 instead 149 ?)
3B 03 00 00 - addon uncompressed ( 827 ) -> next is compressed addon cause of ZIP header
78 9C 75 D2 C1 4E C3 30 0C 06 E0 F2 1E 7B 13 4E 6B 27 AA 8A F5 42 C3 19 B9 89 97 5A 4D E2 CA 4D 07 DD 7B F0 BE 50 21 21 26 DC 5C 3F C7 8A 7F E7 50 14 45 19 E8 76 03 71 6F F5 42 C1 BD 36 C5 EF B9 97 8A 53 16 0E 6A 41 93 E6 09 6D FE B6 87 F8 7E 38 DF D9 D1 0E 84 57 8C 98 76 5C EC 00 C8 81 FD AA F6 3E 0A 26 D0 6F 2E 36 13 27 D5 4A 90 1E 65 1E 78 DA E1 9C 03 5E 08 83 6B 29 51 84 49 2B A2 E4 28 79 B5 41 05 01 93 03 D1 88 63 0F F9 CC 7E D7 0C 7E 64 1D 27 B0 F9 05 C8 3D 09 44 9C 95 34 4E D8 2F DE 30 87 59 69 50 87 75 1A D4 E7 D6 6D 35 80 9E 7F DD 76 8B 5C 71 D5 71 DB 7C 09 69 54 B5 C9 18 3B B6 23 E6 BD 98 5A B0 C2 AA 6C 53 EA 80 17 16 FF D3 F0 FF FC 66 CB 5D 1F C4 50 C4 16 12 78 D4 B6 62 78 44 FD AB 18 01 87 DD 48 21 EC 31 25 94 BF F6 F8 79 7A FE 02 7E FB F2 B0 
54 55 44 49 00 account

	uint8 unkByte, unkByte2;
	p >> unkByte >> h[16] >> mClientBuild >> h[6] >> unkByte2;
    p >> clientSeed >> unkd >> h[14] >> h[11] >> h[2] >> unkd >> h[13] >> h[5];
	p >> h[19] >> h[9] >> unkd >> h[12] >> h[10] >> h[3] >> unkd;
	p >> h[7] >> h[17] >> unkd >> unkd >> h[0] >> h[8] >> h[1];
	p >> h[4] >> h[18] >> h[15] >> account;

	uint32 ByteSize, SizeUncompressed;
	p >> ByteSize >> SizeUncompressed;
	m_addonSize 
= SizeUncompressed;
	m_addonLenCompressed = ByteSize - 4;
	m_addonCompressed = new uint8[ByteSize - 4];
	p.read(m_addonCompressed, ByteSize - 4);
uint32 unkd and uint64 unkq
btw
uint8 h[20] is clienthash

{CLIENT} Packet: (0x29A0) CMSG_AUTH_SESSION PacketSize = 354 stamp = 26682333
|------------------------------------------------|----------------|
|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
|------------------------------------------------|----------------|
|00 5D 01 67 09 02 FD 37 FD 2A E0 00 00 00 00 0D |.].g...7.*......|
|34 EA 9E 00 00 00 00 00 00 00 00 00 4A 95 07 00 |4...........J...|
|00 00 B7 93 7F E2 5D E2 09 67 81 AD 00 00 00 00 |......]..g......|
|44 00 00 00 00 54 55 44 49 00 24 01 00 00 BE 03 |D....TUDI.$.....|
|00 00 78 9C 75 93 41 6E 84 30 0C 45 E9 3D BA E8 |..x.u.An.0.E.=..|
|55 0A 23 D0 A8 20 8D 06 A6 DB CA 24 06 2C 92 18 |U.#.. .....$.,..|
|99 40 3B A3 1E A7 07 2D D3 6E 5A C9 AC 9F 7F EC |.@;....-.nZ.....|
|FF BF F2 94 24 49 EA E8 76 03 B1 6F CF 66 20 5C |....$I..v..o.f \|
|D1 63 88 97 63 F2 E0 DF 1F CB E4 1F 5F 4C 24 0E |.c..c......._L$.|
|2A 4B 41 5A 94 79 E0 69 07 C7 E8 B0 23 74 B6 A2 |*KAZ.y.i....#t..|
|40 1E 26 65 28 73 B4 AD AE 61 45 FB 0A 42 D0 3A |@.&e(s...aE..B.:|
|9C B5 31 F6 2D C4 92 7B 9D 4D 60 E2 19 C8 E6 02 |..1.-..{.M`.....|
|5E D5 1F B0 5D FA 86 D9 69 B0 70 D7 69 50 2D 14 |^...]...i.p.iP-.|
|55 36 80 1E 4C B1 90 B3 2A 39 86 79 42 A3 AB 4A |U6..L...*9.yB..J|
|E6 91 42 9F B3 EC EB EF 46 54 D0 90 C7 0A 02 F4 |..B.....FT......|
|28 5A 53 62 06 40 76 DC 5F F5 26 05 03 E8 45 51 |(ZSb.@v._.&...EQ|
|B0 DB 51 2A CB C0 61 B0 A0 2D FC ED A4 C1 8F A8 |..Q*..a..-......|
|C1 4B 7E 12 EE 48 AF B3 A8 EA 45 56 D4 0F FD 49 |.K~..H....EV...I|
|26 85 30 EE D3 8C 43 14 76 7A FC 11 7D CD 66 C4 |&.0...C.vz..}.f.|
|B8 E7 A9 02 23 AC 13 5E F1 04 56 2B 05 3B 96 7E |....#..^..V+.;.~|
|EF C5 E6 9E 92 DE 78 C3 23 EA DF A7 11 B0 58 8F |......x.#.....X.|
|E4 74 1F 1B A6 80 F2 97 7D 7E 1D 5E BE 01 E5 EE |.t......}~.^....|
|22 92                                           |".              |
-------------------------------------------------------------------
*/
//	uint32 addonsize;
	recvPacket->read_skip( 6 );	//this is digest + crap 
	uint16 smallclientbuild;
	*recvPacket >> (uint16)(smallclientbuild);	mClientBuild = smallclientbuild;
	recvPacket->read_skip( 45 );
	*recvPacket >> account;
/*	recvPacket->read_skip( 4 );
	*recvPacket >> addonsize;
	if( addonsize > recvPacket->size() )
	{
		sLog.outDebug(" Error : Addon size is too large to fit in the packet ");
		return;
	}
	recvPacket->read_skip( addonsize ); /**/
#endif

	// Send out a request for this account.
#ifndef TRINITY_LOGONSERVER_COMPATIBLE
	mRequestID = sLogonCommHandler.ClientConnected(account, this);
#else
	if( sWorld.getIntRate( INTRATE_LOGON_TYPE ) & 1 )
		mRequestID = sLogonCommHandler.ClientConnected(account, this);
	if( mRequestID == 0xFFFFFFFF && ( sWorld.getIntRate( INTRATE_LOGON_TYPE ) & 2 ) )
		mRequestID = 1;
#endif
	
	if(mRequestID == 0xFFFFFFFF)
	{
		sLog.outDetail("Invalid Authrequest. Logonserver never received account %s connection.",account);
		Disconnect();
		return;
	}

	// shitty hash !
	m_fullAccountName = new string( account );

	// Set the authentication packet 
    pAuthenticationPacket = recvPacket;

#ifdef TRINITY_LOGONSERVER_COMPATIBLE
	if( sWorld.getIntRate( INTRATE_LOGON_TYPE ) == 2 )
	{
		WorldPacket data;
		data << uint32( 1 ); // error
		data << uint32( 10 ); // account id
		data << m_fullAccountName->c_str();
		data << "";
		data << uint8( 56 );
		InformationRetreiveCallback( data, mRequestID );
	}
#endif
}

void WorldSocket::InformationRetreiveCallback(WorldPacket & recvData, uint32 requestid)
{
	if(requestid != mRequestID)
		return;

	uint32 error;
	bool TrinityLogonserver = false;
	string AccountName;
	const string * ForcedPermissions;
	uint32 AccountID;
	string GMFlags;
	uint8 AccountFlags;
	string lang = "enUS";
	uint8 K[40];
	BigNumber BNK;
	
	recvData >> error;

#ifdef TRINITY_LOGONSERVER_COMPATIBLE
	//if a hacker tries to login to some other guy account, he will be unable to decode packet headers and server will not understand what packets client is sending
	// client side the message box will appear "connecting" and will be stuck until socket DC
	if( error != 0 && pAuthenticationPacket != NULL && ( sWorld.getIntRate( INTRATE_LOGON_TYPE ) & 2 ) != 0 && m_fullAccountName != NULL )
	{
		TrinityLogonserver = true;
		QueryResult * result = LogonDatabase.Query("SELECT sessionkey,username,id,expansion FROM account WHERE username like '%s'", m_fullAccountName->c_str() );
		if( result )
		{
			Field *fields = result->Fetch();
			if( fields == NULL )
			{
				Log.Debug("WorldSocket","Could not find account %s in DB", m_fullAccountName->c_str() );
				delete result;
				goto DC_PLAYER;
			}
			const char *ActName = fields[1].GetString();
			if( strstr( m_fullAccountName->c_str(), ActName ) == NULL )
			{
				Log.Debug("WorldSocket","Mismatch : Acount name from packet %s and acct name from DB %s ", m_fullAccountName->c_str(), ActName );
				delete result;
				goto DC_PLAYER;
			}
			const char *KhexStr = fields[0].GetString();
			AccountID = fields[2].GetInt32();
			int Expansion = fields[3].GetInt32();
			if( Expansion >= 0 )
				AccountFlags |= 2;  //vanilla
			if( Expansion >= 1 )
				AccountFlags |= 4;	//tbc
			if( Expansion >= 2 )
				AccountFlags |= 8;	//wotlk
			if( Expansion >= 3 )
				AccountFlags |= 16;	//cata
			if( Expansion >= 4 )
				AccountFlags |= 32;	//mop

			//should make a list of flags based on trinity GM rank system
			GMFlags="";

			BigNumber t;
			t.SetHexStr( KhexStr );

			_crypt.Init( t.AsByteArray() );
			BNK.SetBinary( t.AsByteArray(), 40 );

			AccountName = *m_fullAccountName;

			error = 0;
			delete result;

			//get GM rights
		}
		else
		{
			goto DC_PLAYER;
		}
	}else 
#endif
	if(error != 0 || pAuthenticationPacket == NULL)
	{
		// something happened wrong @ the logon server
		OutPacket(SMSG_AUTH_RESPONSE, 1, "\x0D");
		return;
	}

	// Extract account information from the packet.
	if( TrinityLogonserver == false )
	{
		recvData >> AccountID >> AccountName >> GMFlags >> AccountFlags;
		ForcedPermissions = sLogonCommHandler.GetForcedPermissions(AccountName);
		if( ForcedPermissions != NULL )
			GMFlags.assign(ForcedPermissions->c_str());

		// Pull the session key.
		recvData.read(K, 40);
		_crypt.Init(K);
		BNK.SetBinary(K, 40);

		//this is bad ? Should we not read from pAuthenticationPacket ?
		if(recvData.rpos() != recvData.wpos())
			recvData.read((uint8*)lang.data(), 4);
	}

	sLog.outDebug( " >> got information packet from logon: `%s` ID %u (request %u)", AccountName.c_str(), AccountID, mRequestID);
//	sLog.outColor(TNORMAL, "\n");

	mRequestID = 0;

	WorldSession *session = sWorld.FindSession( AccountID );
	if( session)
	{
		if(session->_player != NULL && session->_player->GetMapMgr() == NULL)
		{
DC_PLAYER:
			Log.Debug("WorldSocket","_player found without m_mapmgr during logon, trying to remove him [player %s, map %d, instance %d].", session->_player->GetName(), session->_player->GetMapId(), session->_player->GetInstanceID() );
			if(objmgr.GetPlayer(session->_player->GetLowGUID()))
				objmgr.RemovePlayer(session->_player);
			session->LogoutPlayer(false);
		}
		// we must send authentication failed here.
		// the stupid newb can relog his client.
		// otherwise accounts dupe up and disasters happen.
		OutPacket(SMSG_AUTH_RESPONSE, 1, "\x15");

		// AUTH_FAILED = 0x0D
		session->Disconnect();
		
		// clear the logout timer so he times out straight away
		session->SetLogoutTimer(1);

		return;
	}

#ifdef WE_BOTHERED_MAKING_SURE_WE_CAN_CALCULATE_DIGEST
	Sha1Hash sha;

	uint8 digest[20+5];
#ifdef CATACLYSM_SUPPORT
/*
CDS__WriteInt8(v2, v3->AuthSessionDigest[4]);
CDS__WriteInt8(v2, v3->AuthSessionDigest[8]);
CDS__WriteInt8(v2, v3->AuthSessionDigest[14]);
CDS__WriteInt8(v2, v3->AuthSessionDigest[17]);
CDS__WriteInt8(v2, v3->field_56C);
CDS__WriteInt8(v2, v3->AuthSessionDigest[3]);
CDS__WriteInt32(v2, v3->RealmId);
CDS__WriteInt8(v2, v3->AuthSessionDigest[5]);
CDS__WriteInt8(v2, v3->field_544);
CDS__WriteInt8(v2, v3->AuthSessionDigest[0]);
CDS__WriteInt8(v2, v3->AuthSessionDigest[10]);
CDS__WriteInt32(v2, v3->field_568);
CDS__WriteInt8(v2, v3->AuthSessionDigest[12]);
CDS__WriteInt8(v2, v3->AuthSessionDigest[6]);
CDS__WriteInt32(v2, v3->RealmSiteId);
CDS__WriteInt16(v2, v3->Build);
CDS__WriteInt8(v2, v3->AuthSessionDigest[7]);
CDS__WriteInt8(v2, v3->AuthSessionDigest[11]);
CDS__WriteInt8(v2, v3->AuthSessionDigest[16]);
CDS__WriteInt8(v2, v3->AuthSessionDigest[18]);
CDS__WriteInt8(v2, v3->AuthSessionDigest[13]);
CDS__WriteInt64(v2, v3->UnkAuthULONG);
CDS__WriteInt8(v2, v3->AuthSessionDigest[15]);
CDS__WriteInt32(v2, v3->ClientSeed);
CDS__WriteInt8(v2, v3->AuthSessionDigest[9]);
CDS__WriteInt32(v2, v3->RealmRegion);
CDS__WriteInt8(v2, v3->AuthSessionDigest[1]);
CDS__WriteInt8(v2, v3->AuthSessionDigest[2]);
CDS__WriteInt8(v2, v3->AuthSessionDigest[19]);

00 
5D 01 67 09 02 
FD 37  - client build
FD 2A 
E0 00 00 00 00
0D 34 EA 9E 
00 00 00 00 00 00 00 00 00 
4A 95 
07 00 00 00
B7 93 7F E2 
5D E2 09 67 clientseed
81 AD 
00 00 00 00 
44 
00 00 00 00 
54 55 44 49 00 - Tudi - account
24 01 00 00 
BE 03 00 00 - 958 bytes - addon size ?
78 9C 75 93 41 6E 84 30 0C 45 E9 3D BA E8 55 0A 23 D0 A8 20 8D 06 A6 DB CA 24 06 2C 92 18 99 40 3B A3 1E A7 07 2D D3 6E 5A C9 AC 9F 7F EC FF BF F2 94 24 49 EA E8 76 03 B1 6F CF 66 20 5C D1 63 88 97 63 F2 E0 DF 1F CB E4 1F 5F 4C 24 0E 2A 4B 41 5A 94 79 E0 69 07 C7 E8 B0 23 74 B6 A2 40 1E 26 65 28 73 B4 AD AE 61 45 FB 0A 42 D0 3A 9C B5 31 F6 2D C4 92 7B 9D 4D 60 E2 19 C8 E6 02 5E D5 1F B0 5D FA 86 D9 69 B0 70 D7 69 50 2D 14 55 36 80 1E 4C B1 90 B3 2A 39 86 79 42 A3 AB 4A E6 91 42 9F B3 EC EB EF 46 54 D0 90 C7 0A 02 F4 28 5A 53 62 06 40 76 DC 5F F5 26 05 03 E8 45 51 B0 DB 51 2A CB C0 61 B0 A0 2D FC ED A4 C1 8F A8 C1 4B 7E 12 EE 48 AF B3 A8 EA 45 56 D4 0F FD 49 26 85 30 EE D3 8C 43 14 76 7A FC 11 7D CD 66 C4 B8 E7 A9 02 23 AC 13 5E F1 04 56 2B 05 3B 96 7E EF C5 E6 9E 92 DE 78 C3 23 EA DF A7 11 B0 58 8F E4 74 1F 1B A6 80 F2 97 7D 7E 1D 5E BE 01 E5 EE 22 92 
  */
	pAuthenticationPacket->ResetRead();
	WorldPacket& p = *pAuthenticationPacket;
	uint8 h[20+5];
//	uint8 unkByte;
	uint16 clientbuild;
//	uint64 unkq;
	uint32 clientseed;

	p.read_skip( 1 );
	p >> h[0] >> h[1] >> h[2] >> h[3] >> h[4];
	p >> clientbuild;
	p >> h[5] >> h[6];
	p.read_skip( 5 );
	p >> h[7] >> h[8] >> h[9] >> h[10];
	p.read_skip( 9 );
	p >> h[11] >> h[12];
	p.read_skip( 4 );
	p >> h[13] >> h[14] >> h[15] >> h[16];
	p >> clientseed;
	p >> h[17] >> h[18];
	p.read_skip( 4 );
	p >> h[19];
//	p.read_skip( 4 );
//	p >> AccountName;
//	p.read_skip( 4 );
	memcpy(digest, h, 20);
#else
	pAuthenticationPacket->read(digest, 20);
#endif

	uint32 t = 0;
	if( m_fullAccountName == NULL )				// should never happen !
		sha.UpdateData(AccountName);
	else
	{
		sha.UpdateData(*m_fullAccountName);
		
		// this is unused now. we may as well free up the memory.
		delete m_fullAccountName;
		m_fullAccountName = NULL;
	}

	sha.UpdateData((uint8 *)&t, 4);
	sha.UpdateData((uint8 *)&mClientSeed, 4);
	sha.UpdateData((uint8 *)&mSeed, 4);
	sha.UpdateBigNumbers(&BNK, NULL);
	sha.Finalize();

	if( memcmp(sha.GetDigest(), digest, 20) && false )
	{
		// AUTH_UNKNOWN_ACCOUNT = 21
		OutPacket(SMSG_AUTH_RESPONSE, 1, "\x15");
		return;
	}
#endif

	// Allocate session
	WorldSession * pSession = new WorldSession(AccountID, AccountName, this);
	mSession = pSession;
	ASSERT(mSession);
	pSession->deleteMutex.Acquire();
	
	// Set session properties
	pSession->SetClientBuild(mClientBuild);
	pSession->LoadSecurity(GMFlags);
	pSession->SetAccountFlags(AccountFlags);
	pSession->language = sLocalizationMgr.GetLanguageId(lang);

	if(recvData.rpos() != recvData.wpos())
		recvData >> pSession->m_muted;
	if( pSession->GetSocket() )
	{
		uint32 local_mute_expire = objmgr.GetIPMuteExpire( pSession->GetSocket()->GetRemoteIP().c_str() );
		if( local_mute_expire > pSession->m_muted )
			pSession->m_muted = local_mute_expire;
	}

	for(uint32 i = 0; i < 8; ++i)
		pSession->SetAccountData(i, NULL, true, 0);

	// queue the account loading
	/*AsyncQuery * aq = new AsyncQuery( new SQLClassCallbackP1<World, uint32>(World::getSingletonPtr(), &World::LoadAccountDataProc, AccountID) );
	aq->AddQuery("SELECT * FROM account_data WHERE acct = %u", AccountID);
	CharacterDatabase.QueueAsyncQuery(aq);*/
	if(sWorld.m_useAccountData)
	{
		QueryResult * pResult = CharacterDatabase.Query("SELECT * FROM account_data WHERE acct = %u", AccountID);
		if( pResult == NULL )
			CharacterDatabase.Execute("INSERT INTO account_data VALUES(%u, '', '', '', '', '', '', '', '', '')", AccountID);
		else
		{
			size_t len;
			const char * data;
			char * d;
			for(int i = 0; i < 8; ++i)
			{
				data = pResult->Fetch()[1+i].GetString();
				len = data ? strlen(data) : 0;
				if(len > 1)
				{
					d = new char[len+1];
					memcpy(d, data, len+1);
					pSession->SetAccountData(i, d, true, (uint32)len);
				}
			}

			delete pResult;
			pResult = NULL;
		}
	}

	Log.Debug("Auth", "%s from %s:%u [%ums]", AccountName.c_str(), GetRemoteIP().c_str(), GetRemotePort(), _latency);
#ifdef SESSION_CAP
	if( sWorld.GetSessionCount() >= SESSION_CAP )
	{
		OutPacket(SMSG_AUTH_RESPONSE, 1, "\x0D");
		Disconnect();
		return;
	}
#endif

	// Check for queue.
	if( (sWorld.GetSessionCount() < sWorld.GetPlayerLimit()) || pSession->HasGMPermissions() ) 
	{
		Authenticate();
	}
	else 
	{
		// Queued, sucker.
		uint32 Position = sWorld.AddQueuedSocket(this);
		mQueued = true;
		Log.Debug("Queue", "%s added to queue in position %u", AccountName.c_str(), Position);

		// Send packet so we know what we're doing
		UpdateQueuePosition(Position);
	}

	pSession->deleteMutex.Release();
}

void WorldSocket::Authenticate()
{
	WorldSession * pSession = mSession;
	ASSERT(pAuthenticationPacket);
	mQueued = false;

	if(!pSession) 
		return;
	pSession->deleteMutex.Acquire();

	sStackWorldPacket(data,0,500);
	data.Initialize(SMSG_AUTH_RESPONSE);
    data << uint8 (AUTH_OK);
    data << uint32 (0);                                   // BillingTimeRemaining
    data << uint8 (0);                                    // BillingPlanFlags
    data << uint32 (0);                                   // BillingTimeRested
//	if(pSession->HasFlag(ACCOUNT_FLAG_XPACK_03) && sWorld.realmAllowTBCcharacters)
		data << uint8 ( 3 );                       // 0 - normal, 1 - TBC, must be set in database manually for each account
/*
	else if(pSession->HasFlag(ACCOUNT_FLAG_XPACK_02) && sWorld.realmAllowTBCcharacters)
//		OutPacket(SMSG_AUTH_RESPONSE, 11, "\x0C\x30\x78\x00\x00\x00\x00\x00\x00\x00\x02");
//		OutPacket(SMSG_AUTH_RESPONSE, 11, "\x0C\x14\xA3\x78\xEE\x02\x00\x00\x00\x00\x02");
//		OutPacket(SMSG_AUTH_RESPONSE, 11, "\x0C\x68\x10\x00\x00\x00\x00\x00\x00\x00\x02");
		data << uint8 ( 2 );                       // 0 - normal, 1 - TBC, must be set in database manually for each account
	else if(pSession->HasFlag(ACCOUNT_FLAG_XPACK_01) && sWorld.realmAllowTBCcharacters)
//		OutPacket(SMSG_AUTH_RESPONSE, 11, "\x0C\x14\xA3\x78\xEE\x02\x00\x00\x00\x00\x01");
		data << uint8 ( 1 );                       // 0 - normal, 1 - TBC, must be set in database manually for each account
	else
//		OutPacket(SMSG_AUTH_RESPONSE, 11, "\x0C\x14\xA3\x78\xEE\x02\x00\x00\x00\x00\x00");
		data << uint8 ( 0 );                       // 0 - normal, 1 - TBC, must be set in database manually for each account
	*/
	data << uint8 ( 0 );                       

	SendPacket(&data);

	data.Initialize( SMSG_CLIENTCACHE_VERSION );
	data << uint32( sWorld.ClientCacheVersion );
    SendPacket(&data);

	sAddonMgr.SendAddonInfoPacket(pAuthenticationPacket, (uint32)pAuthenticationPacket->rpos(), pSession);

	//new in 3.1.1. No idea what it is atm
	//OutPacket(UMSG_UNKNOWN_1195, 4, "\x01\x00\x00\x00");

	pSession->_latency = _latency;

	delete pAuthenticationPacket;
	pAuthenticationPacket = NULL;

	if(mSession)
	{
		sWorld.AddSession(mSession);
		sWorld.AddGlobalSession(mSession);

/*		if(pSession->HasFlag(ACCOUNT_FLAG_XTEND_INFO))
			sWorld.AddExtendedSession(pSession);*/

		if(pSession->HasGMPermissions() && mSession)
			sWorld.gmList.insert(pSession);
	}

	pSession->deleteMutex.Release();
}

void WorldSocket::UpdateQueuePosition(uint32 Position)
{
	WorldPacket QueuePacket(SMSG_AUTH_RESPONSE, 17);
	QueuePacket << uint8(0x1B) << uint8(0x2C) << uint8(0x73) << uint8(0) << uint8(0);
	QueuePacket << uint32(0) << uint8(0) << uint8(0);
	QueuePacket << Position;
	QueuePacket << uint8(1);
	SendPacket(&QueuePacket);
}

void WorldSocket::_HandlePing(WorldPacket* recvPacket)
{
	uint32 ping;
	if(recvPacket->size() < 4)
	{
		sLog.outString("Socket closed due to incomplete ping packet.");
		Disconnect();
		return;
	}

	*recvPacket >> _latency;	//not even sure that it is inverse:(
	*recvPacket >> ping;

	if(mSession)
	{
		int32 ping_timediff = (int32)UNIXTIME - (int32)mSession->m_lastPing;
//		sLog.outDebug( "Ping diff is %d ", ping_timediff);
		if( ping_timediff <= 29 && ping_timediff != 0 )	//client will send this packet every 30 seconds. Even if we mess it up once then it should get fixed on next send
		{
			mSession->m_ClientTimeSpeedHackDetectionCount++;
			if( mSession->m_ClientTimeSpeedHackDetectionCount > CLIENT_TIMESPEED_CHEAT_TRIGGER_DC )
			{
				if( mSession->_player && mSession->_player->deleted == false && mSession->_player->IsInWorld() )
				{
					mSession->_player->BroadcastMessage( "Time Speedhack detected. In case server was wrong then make a report how to reproduce this case. You will be logged out in 7 seconds." );
					sCheatLog.writefromsession( mSession, "Caught %s time speed hacking last occurence with speed: %d instead of 30", mSession->_player->GetName(), ping_timediff );
					sEventMgr.AddEvent( mSession->_player, &Player::_Kick, EVENT_PLAYER_KICK, 7000, 1, 0 );
				}
				else
					mSession->Disconnect();
				return;
			}
		}
		else
			mSession->m_ClientTimeSpeedHackDetectionCount = 0;

		mSession->_latency = _latency;
		mSession->m_lastPing = (uint32)UNIXTIME;
		mSession->m_lastRecvPacket = (uint32)UNIXTIME;	//ping is handled without packet queueing
	}

#ifdef USING_BIG_ENDIAN
	swap32(&ping);
#endif

	OutPacket(SMSG_PONG, 4, &ping);

#if (defined( WIN32 ) || defined( WIN64 ) ) && ENABLE_NAGLE_ALGORITHM > 0
	// Dynamically change nagle buffering status based on latency.
	//if(_latency >= 250)
	// I think 350 is better, in a MMO 350 latency isn't that big that we need to worry about reducing the number of packets being sent.
	if(_latency >= 350)
	{
		if(!m_nagleEanbled)
		{
			u_long arg = 0;
			setsockopt(GetFd(), 0x6, 0x1, (const char*)&arg, sizeof(arg));
			m_nagleEanbled = true;
		}
	}
	else
	{
		if(m_nagleEanbled)
		{
			u_long arg = 1;
			setsockopt(GetFd(), 0x6, 0x1, (const char*)&arg, sizeof(arg));
			m_nagleEanbled = false;
		}
	}
#endif
}

void WorldSocket::OnRead()
{
	for(;;)
	{
		// Check for the header if we don't have any bytes to wait for.
		if(mRemaining == 0)
		{
			if(GetReadBuffer().GetSize() < 6)
			{
				// No header in the packet, let's wait.
				return;
			}

			// Copy from packet buffer into header local var
			ClientPktHeader Header;
			GetReadBuffer().Read((uint8*)&Header, 6);

			// Decrypt the header
			_crypt.DecryptRecv((uint8*)&Header, sizeof (ClientPktHeader));
			mRemaining = mSize = ntohs(Header.size) - 4;
			mOpcode = Header.cmd;
		}

		if(mRemaining > 0)
		{
			//in case we cannot store this packet due to corrupted header then we try to drop it
			if( GetReadBuffer().GetSpace() < mRemaining )
			{
//				ASSERT( false ); //packet header was wrong and now we are dumping the packet
				mRemaining = mSize = mOpcode = 0;
				//disconnect or something
				if( mSession )
					mSession->Disconnect();
				OnDisconnect();
//printf("!!!!we have space %u and we would need %u \n",GetReadBuffer().GetSpace(),mRemaining);
				return;
			}
			if( GetReadBuffer().GetSize() < mRemaining )
			{
				// We have a fragmented packet. Wait for the complete one before proceeding.
				return;
			}
		}

		WorldPacket * Packet;
		Packet = new WorldPacket(mOpcode, mSize);
		Packet->resize(mSize);

		if(mRemaining > 0)
		{
			// Copy from packet buffer into our actual buffer.
			///Read(mRemaining, (uint8*)Packet->contents());
			GetReadBuffer().Read((uint8*)Packet->contents(), mRemaining);
		}

		sWorldLog.LogPacket(mSize, mOpcode, mSize ? Packet->contents() : NULL, 0);
		mRemaining = mSize = mOpcode = 0;

		// Check for packets that we handle
		switch(Packet->GetOpcode())
		{
		case CMSG_PING:
			{
				_HandlePing(Packet);
				delete Packet;
				Packet = NULL;
			}break;
		case CMSG_AUTH_SESSION:
			{
				_HandleAuthSession(Packet);
				//delete Packet;	-> do not delete it, we keep reference to it !
				Packet = NULL;
			}break;
		default:
			{
#if defined( _DEBUG ) && !defined( X64 )
				if( GetSession() )
					Log.Notice("WorldSocket", "Got packet with opcode: %04X and name %s for act %d", Packet->GetOpcode(),LookupName(Packet->GetOpcode(), g_worldOpcodeNames ), GetSession()->GetAccountId());
				else
					Log.Notice("WorldSocket", "Got packet with opcode: %04X and name %s", Packet->GetOpcode(),LookupName(Packet->GetOpcode(), g_worldOpcodeNames ));
#endif
				//this is to fix some old code bug where disconnect would have removed session socket but not remove socket session
				if( mSession && mSession->_socket == NULL )
					mSession = NULL;
				if( mSession )
					mSession->QueuePacket(Packet);
				else
				{
					delete Packet;
					Packet = NULL;
				}
			}break;
		}
	}
}

#endif

void WorldLog::LogPacket(uint32 len, uint16 opcode, const uint8* data, uint8 direction)
{
#ifdef ECHO_PACKET_LOG_TO_CONSOLE
	sLog.outString("[%s]: %s %s (0x%03X) of %u bytes.", direction ? "SERVER" : "CLIENT", direction ? "sent" : "received",
		LookupName(opcode, g_worldOpcodeNames), opcode, len);
#endif

	if(bEnabled)
	{
		mutex.Acquire();
		unsigned int line = 1;
		unsigned int countpos = 0;
		uint16 lenght = len;
		unsigned int count = 0;

		fprintf(m_file, "{%s} Packet: (0x%04X) %s PacketSize = %u stamp = %u\n", (direction ? "SERVER" : "CLIENT"), opcode,
			LookupName(opcode, g_worldOpcodeNames), lenght, getMSTime() );
		fprintf(m_file, "|------------------------------------------------|----------------|\n");
		fprintf(m_file, "|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|\n");
		fprintf(m_file, "|------------------------------------------------|----------------|\n");

		if(lenght > 0)
		{
			fprintf(m_file, "|");
			for (count = 0 ; count < lenght ; count++)
			{
				if (countpos == 16)
				{
					countpos = 0;

					fprintf(m_file, "|");

					for (unsigned int a = count-16; a < count;a++)
					{
						if ((data[a] < 32) || (data[a] > 126))
							fprintf(m_file, ".");
						else
							fprintf(m_file, "%c",data[a]);
					}

					fprintf(m_file, "|\n");

					line++;
					fprintf(m_file, "|");
				}

				fprintf(m_file, "%02X ",data[count]);

				//FIX TO PARSE PACKETS WITH LENGHT < OR = TO 16 BYTES.
				if (count+1 == lenght && lenght <= 16)
				{
					for (unsigned int b = countpos+1; b < 16;b++)
						fprintf(m_file, "   ");

					fprintf(m_file, "|");

					for (unsigned int a = 0; a < lenght;a++)
					{
						if ((data[a] < 32) || (data[a] > 126))
							fprintf(m_file, ".");
						else
							fprintf(m_file, "%c",data[a]);
					}

					for (unsigned int c = count; c < 15;c++)
						fprintf(m_file, " ");

					fprintf(m_file, "|\n");
				}

				//FIX TO PARSE THE LAST LINE OF THE PACKETS WHEN THE LENGHT IS > 16 AND ITS IN THE LAST LINE.
				if (count+1 == lenght && lenght > 16)
				{
					for (unsigned int b = countpos+1; b < 16;b++)
						fprintf(m_file, "   ");

					fprintf(m_file, "|");

					unsigned short print = 0;

					for (unsigned int a = line * 16 - 16; a < lenght;a++)
					{
						if ((data[a] < 32) || (data[a] > 126))
							fprintf(m_file, ".");
						else
							fprintf(m_file, "%c",data[a]);

						print++;
					}

					for (unsigned int c = print; c < 16;c++)
						fprintf(m_file, " ");

					fprintf(m_file, "|\n");
				}

				countpos++;
			}
		}
		fprintf(m_file, "-------------------------------------------------------------------\n\n");
		fflush(m_file);
		mutex.Release();
	}
}
