#pragma once

#include "stdafx.h"
#include "WorkerThreads.h"
#include "TimerThread.h"
#include "SessionManager.h"
#include "HeartBeatThread.h"
//#include "ClientSession.h"
#define NETWORK_ENGINE CNetEngine::GetInstance()

class CNetEngine
{
public:
	CNetEngine();
	~CNetEngine();

	static CNetEngine* GetInstance()
	{
		static CNetEngine instance;
		return &instance;
	}

	bool Start();
	void Run();
	void Shutdown();

	bool CreateListenSocket();
	bool Bind();
	bool Listen();
	bool Connect(char *ip, USHORT port);
	void Accept();	// Loop ����

	bool CreateIOCP(int nThread = 0);
	bool AssociateSocketWithIOCP(CClientSession *m_pClientSession);

	void SetNagle(SOCKET sock, bool flag);
private:
	HANDLE m_hIocp;
	SOCKET m_hListenSocket;

	unique_ptr<CWorkerThreads> m_pWorkerThreads;
	unique_ptr<CTimerThread>   m_pTimerThread;
	unique_ptr<CHeartBeatThread> m_pHeartBeatThread;

	SOCKET m_socket;		// �ٸ� ��ǻ�Ϳ��� ����� ���� ���� ����
public:

	// cuda
	

	// Getter , Setter
	HANDLE						GetIOCPHandle()		 const { return m_hIocp; }
	SOCKET						GetListenSock()		 const { return m_hListenSocket; }
	const unique_ptr<CTimerThread>& GetTimerThread() { return m_pTimerThread; }
};

// ���� �Լ� ���� ��� �� ����
void err_quit(TCHAR *msg);

// ���� �Լ� ���� ���
void err_display(TCHAR *msg);