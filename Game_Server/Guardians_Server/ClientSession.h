#pragma once
#include "protocol.h"
#include "Player.h"
#include "MemoryPool.h"
#include "RingBuffer.h"
#include "SendMemoryPool.h"

#define ZERO_BUFFER

class CClientSession;



struct OverlappedEx
{
	WSAOVERLAPPED   overlapped;
	OPERATION_TYPE  operation_type;
	WSABUF          wsabuf;

	OverlappedEx()
	{
		ZeroMemory(&overlapped, sizeof(WSAOVERLAPPED));
		ZeroMemory(&wsabuf, sizeof(WSABUF));
	}
};

struct RecvOverlappedEx : public OverlappedEx, public CMemoryPool<RecvOverlappedEx>
{
	RecvOverlappedEx()
	{
		operation_type = OPERATION_TYPE::OP_RECV;
	}
};

struct SendOverlappedEx : public OverlappedEx, public CMemoryPool<SendOverlappedEx>
{
	SendOverlappedEx()
	{
		operation_type = OPERATION_TYPE::OP_SEND;
	}
};

struct ZeroRecvOverlappedEx : public OverlappedEx, CMemoryPool<ZeroRecvOverlappedEx, 10024>
{
	ZeroRecvOverlappedEx()
	{
		operation_type = OPERATION_TYPE::OP_ZERO_RECV;
	}
};

struct OnceSendOverlappedEx : public OverlappedEx, CMemoryPool<OnceSendOverlappedEx, 100240>
{
	OnceSendOverlappedEx()
	{
		operation_type = OPERATION_TYPE::OP_ONCE_SEND;
	}
};

struct MoveOverlappedEx : public OverlappedEx, CMemoryPool<MoveOverlappedEx>
{
	MoveOverlappedEx()
	{
		operation_type = OPERATION_TYPE::OP_NPC_MOVE;
	}
};


class CClientSession
{
public:
	CClientSession();
	~CClientSession();

	void Initalize();
	void Reset();							

	bool PreRecv();							// For Zero Recv
	bool PostRecv();		
	void RecvCompletion(const DWORD transferred);

	void OnceSend(const char *pkt);
	void PreSend(const char *pkt);
	bool PostSend();
	void SendCompletion(const DWORD transferred);
	
	bool PostAccept();						// AccepetEx 쓸때 사용
	bool AcceptCompletion();
	// AcceptEx를 위한 함수 따로 만들 예정
private :
	SOCKET        m_socket;
	UINT          m_index;
	SOCKADDR_IN   m_sockaddr;

	bool		  m_bConnect;	// atomic 처리 해야하나?

	CRingBuffer *m_pSendRingBuffer;
	CRingBuffer *m_pRecvRingBuffer;

	CPlayer *m_pPlayer;

	CSRWLock m_sendBufLock;
public :
	// Setter, Getter	
	void SetSocket(const SOCKET socket)						 { m_socket = socket; }
	void SetID(const int id)								 { m_pPlayer->SetID(id); }
	void SetConnect(const bool connect)						 { m_bConnect = connect; }
	void SetSockAddr(const SOCKADDR_IN &sockaddr)			 { m_sockaddr = sockaddr; }
	void SetIndex(const int index)							 { m_index = index; }

	CPlayer*	 GetPlayer()							 { return m_pPlayer; }
	SOCKET	     GetSocket()					   const { return m_socket; }
	UINT		 GetIndex()						   const { return m_index; }
	UINT		 GetID()						   const { return m_pPlayer->GetID(); }	
	bool		 IsConnected()					   const { return m_bConnect; }
	CRingBuffer* GetRecvRingBuffer()			   const { return m_pRecvRingBuffer; }
	CRingBuffer* GetSendRingBuffer()			   const { return m_pSendRingBuffer; }

	const SOCKADDR_IN& GetSockAddr() const { return m_sockaddr; }
};
