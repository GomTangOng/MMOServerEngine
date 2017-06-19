#pragma once

// ���߿� ���� CPP HEADER ������ ó��	

class CAcceptThread
{
public:
	CAcceptThread();
	~CAcceptThread();

	void WaitThreads();
	bool Create();
	void Release();

	UINT Run();

	static void EntryPoint(void* arg);

private :
	thread *m_pAcceptThread;
	SOCKET  m_listenSocket;
};

