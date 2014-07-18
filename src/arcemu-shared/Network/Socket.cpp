/*
 * Multiplatform Async Network Library
 * Copyright (c) 2007 Burlex
 *
 * Socket implementable class.
 *
 */

#include "Network.h"

initialiseSingleton(SocketGarbageCollector);

Socket::Socket(SOCKET fd, uint32 sendbuffersize, uint32 recvbuffersize) : m_fd(fd), m_connected(false),	m_deleted(false)
{
	// Allocate Buffers
	readBuffer.Allocate(recvbuffersize);
	writeBuffer.Allocate(sendbuffersize);

	// IOCP Member Variables
#ifdef CONFIG_USE_IOCP
	m_writeLock = 0;
	m_completionPort = 0;
#else
	m_writeLock = 0;
#endif

	// Check for needed fd allocation.
	if(m_fd == 0)
		m_fd = SocketOps::CreateTCPFileDescriptor();
	m_empty_reads = 0;
}

Socket::~Socket()
{
	if( m_fd )
	{
		SocketOps::CloseSocket(m_fd);
		m_fd = 0;	//this is closed already ?
	}
}

bool Socket::TryRudeDisconnect()
{
	if( m_fd )
	{
		SocketOps::CloseSocket(m_fd);
		m_fd = 0;	//this is closed already ?
		return true;
	}
	return false;
}

bool Socket::Connect(const char * Address, uint32 Port)
{
	struct hostent * ci = gethostbyname(Address);
	if(ci == 0)
		return false;

	m_client.sin_family = ci->h_addrtype;
	m_client.sin_port = ntohs((u_short)Port);
	memcpy(&m_client.sin_addr.s_addr, ci->h_addr_list[0], ci->h_length);

	SocketOps::Blocking(m_fd);
	if(connect(m_fd, (const sockaddr*)&m_client, sizeof(m_client)) == -1)
		return false;

	// at this point the connection was established
#ifdef CONFIG_USE_IOCP
	m_completionPort = sSocketMgr.GetCompletionPort();
#endif
	_OnConnect();
	return true;
}

void Socket::Accept(sockaddr_in * address)
{
//	memcpy(&m_client, address, sizeof(*address));
	memcpy(&m_client, address, sizeof(sockaddr_in));
	_OnConnect();
}

void Socket::_OnConnect()
{
	// set common parameters on the file descriptor
	SocketOps::Nonblocking(m_fd);
	SocketOps::DisableBuffering(m_fd);
/*	SocketOps::SetRecvBufferSize(m_fd, m_writeBufferSize);
	SocketOps::SetSendBufferSize(m_fd, m_writeBufferSize);*/
	m_connected = true;

	// IOCP stuff
#ifdef CONFIG_USE_IOCP
	AssignToCompletionPort();
	//wait until client will send us a sign that we can send login packet
	//this might time out and then we might queue this socket for deletion leading to a deleted socket usage.
	SetupReadEvent();	
#endif
	//small chance that SetupReadEvent will delete this socket
	if( IsConnected() )
	{
		sSocketMgr.AddSocket(this);
		// Call virtual onconnect
		OnConnect();
	}
}

bool Socket::Send(const uint8 * Bytes, uint32 Size)
{
	bool rv;

	// This is really just a wrapper for all the burst stuff.
	BurstBegin();
	rv = BurstSend(Bytes, Size);
	if(rv)
		BurstPush();
	BurstEnd();

	return rv;
}

bool Socket::BurstSend(const uint8 * Bytes, uint32 Size)
{
	return writeBuffer.Write(Bytes, Size);
}

bool Socket::BurstSend(const uint8 * Bytes, const uint8 * Bytes1, uint32 Size, uint32 Size1)
{
	return writeBuffer.Write(Bytes, Bytes1, Size, Size1);
}

string Socket::GetRemoteIP()
{
	char* ip = (char*)inet_ntoa( m_client.sin_addr );
	if( ip != NULL )
		return string( ip );
	else
		return string( "noip" );
}

void Socket::Disconnect()
{
	if( m_connected )
	{

		//make sure this is not a DC while we are still queued for read or write
		//! this will trigger a lot since we add ourself to the OS and wait for a read / write event !
		// the key is to not delete the structure until the event does not time out !
/*		m_readEvent.Mark();
		m_writeEvent.Mark();
		m_readMutex.Acquire();
		m_writeMutex.Acquire();*/
		
		m_connected = false;

		// remove from mgr
		sSocketMgr.RemoveSocket(this);

		SocketOps::CloseSocket(m_fd);
		m_fd = 0;	//this is closed already ?

/*		m_readEvent.Unmark();
		m_writeEvent.Unmark();
		m_readMutex.Release();
		m_writeMutex.Release();*/

		// Call virtual ondisconnect
		OnDisconnect();
	}

	if(!m_deleted) 
		Delete();
}

void Socket::Delete()
{
	if(m_deleted) 
		return;
	m_deleted = true;

	if(m_connected) 
		Disconnect();
	sLog.outDebug("Queue socket for deletion : %p",this);
	sSocketGarbageCollector.QueueSocket(this);
}

