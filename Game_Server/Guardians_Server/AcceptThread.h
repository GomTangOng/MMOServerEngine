#pragma once

// 나중에 따로 CPP HEADER 빼내서 처리	

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

