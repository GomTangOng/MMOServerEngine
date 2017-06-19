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
	void Accept();	// Loop 돌림

	bool CreateIOCP(int nThread = 0);
	bool AssociateSocketWithIOCP(CClientSession *m_pClientSession);

	void SetNagle(SOCKET sock, bool flag);
private:
	HANDLE m_hIocp;
	SOCKET m_hListenSocket;

	unique_ptr<CWorkerThreads> m_pWorkerThreads;
	unique_ptr<CTimerThread>   m_pTimerThread;
	unique_ptr<CHeartBeatThread> m_pHeartBeatThread;

	SOCKET m_socket;		// 다른 컴퓨터와의 통신을 위해 쓰는 소켓
public:

	// cuda
	

	// Getter , Setter
	HANDLE						GetIOCPHandle()		 const { return m_hIocp; }
	SOCKET						GetListenSock()		 const { return m_hListenSocket; }
	const unique_ptr<CTimerThread>& GetTimerThread() { return m_pTimerThread; }
};

// 소켓 함수 오류 출력 후 종료
void err_quit(TCHAR *msg);

// 소켓 함수 오류 출력
void err_display(TCHAR *msg);