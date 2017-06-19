#include "stdafx.h"

playerInfo::playerInfo()
{
	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0 )
		printf("WSAStartup() Error!!\n");
	m_bConnect = FALSE;
	m_bLogin = FALSE;
	InitializeCriticalSection(&cs);
}


BOOL playerInfo::Connect(int i)   
{
	m_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	HANDLE iocp = CreateIoCompletionPort((HANDLE) m_socket, g_iocp, i, 0);

	if ( (WSAConnect( m_socket, (SOCKADDR*)&clntAdr, sizeof(clntAdr), NULL, NULL, NULL, NULL )) == SOCKET_ERROR ) {
		printf("WSAConnect() Error!!\n");
		return false;
	}
	m_bConnect = true;
	return true;
}

BOOL playerInfo::SendPacket(CHAR *packet, WORD size)
{
	SEND_STRUCT *Send = new SEND_STRUCT();

	ZeroMemory(&Send->m_OverExSend, sizeof(OVERLAPPED_EX));
	Send->m_OverExSend.m_io_type = IO_SEND;

	CopyMemory(Send->m_SendBuffer, packet, size);

	Send->m_SendWSABuf.buf = Send->m_SendBuffer;
	Send->m_SendWSABuf.len = size;


	DWORD SendBytes;

	INT Result = WSASend(	m_socket,
							&Send->m_SendWSABuf,
							1,
							&SendBytes,
							0,
							reinterpret_cast<LPWSAOVERLAPPED> (&Send->m_OverExSend),
							NULL);

	if (Result == SOCKET_ERROR) {
		if (WSAGetLastError() != WSA_IO_PENDING) {
			printf("send() error!!\n");
			return FALSE;
		}
	}

	return TRUE;
}

BOOL playerInfo::DisConnect()
{
	// closesocket()
	if( m_socket )
		closesocket( m_socket );
	m_bConnect = FALSE;
	return TRUE;
}
