#pragma once


class CHeartBeatThread 
{
public:
	CHeartBeatThread();
	~CHeartBeatThread();

	void WaitThreads();
	bool Create();
	void Release();

	UINT Run();

	static void EntryPoint(void* arg);
private :
	thread *m_pHeartBeatThread;
};

