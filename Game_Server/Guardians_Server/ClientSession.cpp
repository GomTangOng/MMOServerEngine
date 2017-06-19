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

void CClientSession::Initalize()	// Warning : id�� -1�� ����Ʈ �����ϱ⿡ �� �Լ�ȣ�� �� ID ���� �ʼ�
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
	//for (auto mon : m_pPlayer->GetOldViewMonsterList())		// �� ó�� �ʿ�??
	//{
	//	OBJECT_MANAGER->FindMonster(mon)->SetAlive(false);
	//}
	m_pPlayer->GetSector()->DelPlayer(GetID());	// �ڡڡ� ���߿� �꿡 ���ؼ��� �� ó�� �� �������
	
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
	// Post Recv���� �̹� �� �Լ����� �ؾߵ� ���� �ϱ� ������
	// �� �Լ��� �ϴ� �Ұ� ����
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

	// ToDo : OnceSend ���� �� Ŭ�� �����ش�.
	if (m_pSendRingBuffer->GetEmptySize() < pkt[0])
	{
		int old_size = m_pSendRingBuffer->GetCurrentSize();			// �� old_size�� ������ ����... �� �̷���..

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
	// Todo : ��Ŷ�� ����ִ� ���� �����۰� �� á���� �����ϴ� �ڵ嵵 ¥���Ѵ�.
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
	// lock �ʿ�
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

	if (m_pSendRingBuffer->GetEmptySize() <= 0)		// Note : �����۰� ���������� ~ �迭�ǳ��� ���� ����Ʈ���� �ʹ� ���ٸ� ���̸� �������� �� �������� �̵���Ű�� �����ϰ� ����       
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
