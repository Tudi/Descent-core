/*
 * Multiplatform Async Network Library
 * Copyright (c) 2007 Burlex
 *
 * SocketWin32.cpp - Windows-specific functions of Socket class are located here.
 *
 */


#include "Network.h"
#ifdef CONFIG_USE_IOCP
//called 1 by BurstPush() - this is our worldserver ( or other ) packet sender
//called 2 by HandleWriteComplete in case there is something to be sent to OS
void Socket::WriteCallback()
{
	if(m_deleted || !m_connected)
		return;

	//printf("\nSocket::Writecallback(): sendsize : %u\n", this->m_writeByteCount);
	// We don't want any writes going on while this is happening.
	m_writeMutex.Acquire();
	if(writeBuffer.GetSize())
	{
		DWORD w_length = 0;
		DWORD flags = 0;

		// attempt to push all the data out in a non-blocking fashion.
		WSABUF buf;
		buf.len = (u_long)writeBuffer.GetSize();
		buf.buf = (char*)writeBuffer.GetBufferStart();

		ASSERT( m_writeEvent.m_inUse == 0 );
		m_writeEvent.Mark();
		m_writeEvent.Reset(SOCKET_IO_EVENT_WRITE_END);
		int r = WSASend(m_fd, &buf, 1, &w_length, flags, &m_writeEvent.m_overlap, 0);
		if(r == SOCKET_ERROR)
		{
			if(WSAGetLastError() != WSA_IO_PENDING)
			{
				m_writeEvent.Unmark();
				DecSendLock();
				Disconnect();
			}
		}
	}
	else
	{
		// Write operation is completed.
		DecSendLock();
	}
	m_writeMutex.Release();
}

void Socket::SetupReadEvent()
{
	if(m_deleted || !m_connected)
		return;

	m_readMutex.Acquire();
	DWORD r_length = 0;
	DWORD flags = 0;
	WSABUF buf;
	buf.len = (u_long)readBuffer.GetSpace();
	ASSERT( buf.len > 6 );		//we should not try to block here since someone sent us a continues packet larger then our max buffer size
	buf.buf = (char*)readBuffer.GetBuffer();	

	// event that will trigger after data is receieved
	/*OverlappedStruct * ov = new OverlappedStruct(SOCKET_IO_EVENT_READ_COMPLETE);

	if(WSARecv(m_fd, &buf, 1, &r_length, &flags, &ov->m_overlap, 0) == SOCKET_ERROR)
	{
		if(WSAGetLastError() != WSA_IO_PENDING)
			Disconnect();
	}*/

	m_readEvent.Mark();
	m_readEvent.Reset(SOCKET_IO_EVENT_READ_COMPLETE);
	if(WSARecv(m_fd, &buf, 1, &r_length, &flags, &m_readEvent.m_overlap, 0) == SOCKET_ERROR)
	{
		if(WSAGetLastError() != WSA_IO_PENDING)
		{
			m_readEvent.Unmark();
			Disconnect();
		}
	}
	//m_readEvent = ov;
	m_readMutex.Release();
}

void Socket::ReadCallback(uint32 len)
{
	readBuffer.IncrementWritten( len );
	OnRead();
	SetupReadEvent();
}

void Socket::AssignToCompletionPort()
{
	/*HANDLE h = */CreateIoCompletionPort((HANDLE)m_fd, m_completionPort, (ULONG_PTR)this, 0);
	//__asm int 3;
}

void Socket::BurstPush()
{
	if(AcquireSendLock())
		WriteCallback();
}

#endif
