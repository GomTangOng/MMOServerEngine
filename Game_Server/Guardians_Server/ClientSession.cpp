#include "stdafx.h"
#include "ClientSession.h"
#include "NetEngine.h"
#include "WorldManager.h"
#include "ObjectManager.h"

CClientSession::CClientSession()
{
	m_socket = NULL;
	m_bConnect = false;

	m_pSendRingBuffer = nullptr;
	m_pRecvRingBuffer = nullptr;
	m_pPlayer         = nullptr;
}


CClientSession::~CClientSession()
{
	Memory::SAFE_DELETE_ARRAY(m_pRecvRingBuffer);
	Memory::SAFE_DELETE_ARRAY(m_pSendRingBuffer);
	Memory::SAFE_DELETE(m_pPlayer);
}

void CClientSession::Initalize()	// Warning : id를 -1로 디폴트 설정하기에 이 함수호출 뒤 ID 설정 필수
{
	ZeroMemory(&m_sockaddr, sizeof(m_sockaddr));

	m_socket   = NULL;
	m_bConnect = false;

	if (!m_pPlayer) m_pPlayer = new CPlayer();
	if(!m_pSendRingBuffer) m_pSendRingBuffer = new CRingBuffer();
	if(!m_pRecvRingBuffer) m_pRecvRingBuffer = new CRingBuffer();

	m_pPlayer->Clear();
}

void CClientSession::Reset()
{
	//for (auto mon : m_pPlayer->GetOldViewMonsterList())		// 이 처리 필요??
	//{
	//	OBJECT_MANAGER->FindMonster(mon)->SetAlive(false);
	//}
	m_pPlayer->GetSector()->DelPlayer(GetID());	// ★★★ 나중에 펫에 대해서도 이 처리 꼭 해줘야함
	
	m_pPlayer->Clear();
	
	m_pRecvRingBuffer->Clear(); 
	m_pSendRingBuffer->Clear();

	m_bConnect = false;

	if (m_socket != NULL) closesocket(m_socket);
}

bool CClientSession::PreRecv()
{
	ZeroRecvOverlappedEx *overlapped = new ZeroRecvOverlappedEx();
	
	DWORD recvBytes = 0;
	DWORD flags     = 0;
	overlapped->wsabuf.buf = nullptr;
	overlapped->wsabuf.len = 0;

	if (SOCKET_ERROR == WSARecv(m_socket, &overlapped->wsabuf, 1, &recvBytes, &flags, &overlapped->overlapped, NULL))
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			delete overlapped;
			printf("CClientSession::PreRecv Error\n");
			return false;
		}
	}

	return true;
}

bool CClientSession::PostRecv()
{
	RecvOverlappedEx *overlapped = new RecvOverlappedEx();
	
	DWORD recvBytes = 0;
	DWORD flags = 0;
	
	overlapped->wsabuf.buf = m_pRecvRingBuffer->GetEuqueuePosition();	
	overlapped->wsabuf.len = m_pRecvRingBuffer->GetEmptySize();

	if (SOCKET_ERROR == WSARecv(m_socket, &overlapped->wsabuf, 1, &recvBytes, &flags, &overlapped->overlapped, NULL))
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			delete overlapped;
			err_display(L"CClientSession::PostRecv Error\n");
			return false;
		}
	}
	return true;
}

void CClientSession::RecvCompletion(const DWORD transferred)
{
	// Post Recv에서 이미 이 함수에서 해야될 일을 하기 때문에
	// 이 함수는 일단 할게 없음
}

void CClientSession::OnceSend(const char * pkt)
{
	OnceSendOverlappedEx *overlapped = new OnceSendOverlappedEx();

	m_sendBufLock.WriteEnter();

	DWORD sendBytes = 0;
	DWORD flags     = 0;
	
	if (pkt[0] < 0)
	{
		cout << "pkt[0] < 0\n";
	}

	// ToDo : OnceSend 만든 후 클라에 보내준다.
	if (m_pSendRingBuffer->GetEmptySize() < pkt[0])
	{
		int old_size = m_pSendRingBuffer->GetCurrentSize();			// ★ old_size가 음수가 나옴... 왜 이럴까..

		if (0 > old_size) {
			cout << "old_size < 0\n";
		}

		memcpy(m_pSendRingBuffer->GetBufStartPosition(), m_pSendRingBuffer->GetDequeuePosttion(), old_size);
		m_pSendRingBuffer->Clear();
		m_pSendRingBuffer->Commit_Enqueue(old_size);
	}
	m_pSendRingBuffer->Enqueue(pkt, pkt[0]);

	overlapped->wsabuf.buf = m_pSendRingBuffer->GetDequeuePosttion();
	overlapped->wsabuf.len = pkt[0];

	if (SOCKET_ERROR == WSASend(m_socket, &overlapped->wsabuf, 1, &sendBytes, flags, &overlapped->overlapped, NULL))
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			m_sendBufLock.WriteLeave();
			
			delete overlapped;
			
			err_display(L"CClientSession::OnceSend Error\n");
			
			return;
		}
	}
	m_pSendRingBuffer->Commit_Dequeue(sendBytes);
	m_sendBufLock.WriteLeave();
}

void CClientSession::PreSend(const char * pkt)
{
	int type = pkt[1];
	// Todo : 패킷을 집어넣는 도중 링버퍼가 꽉 찼을때 대응하는 코드도 짜야한다.
	m_sendBufLock.WriteEnter();

	assert(type >= 0);
	int retval = m_pSendRingBuffer->Enqueue(pkt, pkt[0]);
	if (retval == -1)
	{
		cout << "CClientSession::PreSend Faiied : IsFull()\n";
	}
	m_sendBufLock.WriteLeave();	
}

bool CClientSession::PostSend()
{
	// lock 필요
	m_sendBufLock.WriteEnter();

	int buf_size = m_pSendRingBuffer->GetCurrentSize();

	if (buf_size == 0)
	{
		m_sendBufLock.WriteLeave();
		return true;
	}

	SendOverlappedEx *overlapped = new SendOverlappedEx();
	
	DWORD sendBytes = 0;
	DWORD flags     = 0;

	overlapped->wsabuf.buf = m_pSendRingBuffer->GetDequeuePosttion();
	overlapped->wsabuf.len = buf_size;

	if (SOCKET_ERROR == WSASend(m_socket, &overlapped->wsabuf, 1, &sendBytes, flags, &overlapped->overlapped, NULL))
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			m_sendBufLock.WriteLeave();
			delete overlapped;
			printf("CClientSession::PostSend Error %d\n", WSAGetLastError());
			return false;
		}
	}
	m_sendBufLock.WriteLeave();
	return true;
}

void CClientSession::SendCompletion(const DWORD transferred)
{
	int curr_size{ 0 };

	m_sendBufLock.WriteEnter();
	m_pSendRingBuffer->Commit_Dequeue(transferred);

	if (m_pSendRingBuffer->GetEmptySize() <= 0)		// Note : 링버퍼가 현재포인터 ~ 배열의끝의 남은 바이트수가 너무 적다면 데이를 링버퍼의 맨 앞쪽으로 이동시키고 깨끗하게 만듬       
	{
		curr_size = m_pSendRingBuffer->GetCurrentSize();
		memcpy(m_pSendRingBuffer->GetBufStartPosition(), m_pSendRingBuffer->GetDequeuePosttion(), curr_size);
		m_pSendRingBuffer->Clear();
		m_pSendRingBuffer->Commit_Enqueue(curr_size);
	}
		
	m_sendBufLock.WriteLeave();
}

bool CClientSession::PostAccept()
{
	return false;
}

bool CClientSession::AcceptCompletion()
{
	int opt = 0;
#if defined(ZERO_BUFFER)
	//setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (const char *)&opt, sizeof(int));	// for zero recv
#endif

	NETWORK_ENGINE->SetNagle(m_socket, false);

	if (!NETWORK_ENGINE->AssociateSocketWithIOCP(this))
	{
		err_display(L"[DEBUG] Accpet() - CreateIoCompletionPort() Error\n");
		return false;
	}


#if defined(ZERO_BUFFER)
	if (!PreRecv())
	{
		printf("[DEBUG] CClientSession::AcceptCompletion() - PreRecv() Error\n");
		return false;
	}
#else
	if (!PostRecv())
	{
		printf("[DEBUG] CClientSession::AcceptCompletion() - PostRecv() Error\n");
		return false;
	}
#endif
	return true;
}
