#pragma once

#include "protocol.h"
#include "RingBuffer.h"
#include "SendMemoryPool.h"

#define SERVER

#define NETWORK_ENGINE CClientNetEngine::GetInstance()

class CClientNetEngine
{
public:
	CClientNetEngine();
	~CClientNetEngine();

	static CClientNetEngine* GetInstance()
	{
		static CClientNetEngine instance;
		return &instance;
	}

	bool Start(const HWND hWnd);
	void ShutDown();

	void CreateSocket();
	void Connect(const char *ip, const USHORT port);	

	void RegisterAsyncSelect(const HWND hWnd);

	void SendPacket(char *packet);
	void RecvPacket();

	void IsNagle(SOCKET sock, bool flag);
private:
	SOCKET  m_socket;
	int     m_id;
	WSABUF  m_wsaSendBuf;
	WSABUF  m_wsaRecvBuf;

	CSendMemoryPool *m_pSendMemoryPool;
	CRingBuffer     *m_pRecvRingBuffer;
public:
	// Setter, Getter
	void         SetID(const int id) { m_id = id; }

	SOCKET		 GetSocket()   const { return m_socket; }
	int			 GetID()	   const { return m_id; }
	//CRingBuffer* GetSendRingBuffer() { return m_pSendRingBuffer; }
	CRingBuffer*     GetRecvRingBuffer() const { return m_pRecvRingBuffer; }
	CSendMemoryPool* GetSendMemoryPool() const { return m_pSendMemoryPool; }
};

// ���� �Լ� ���� ��� �� ����
void err_quit(TCHAR *msg);

// ���� �Լ� ���� ���
void err_display(TCHAR *msg);