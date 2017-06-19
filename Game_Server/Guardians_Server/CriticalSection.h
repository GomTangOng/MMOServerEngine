#pragma once
#include <Windows.h>

class CCritcalSection
{
public :
	CCritcalSection()
	{
		InitializeCriticalSection(&m_cs);
	}
	~CCritcalSection()
	{
		DeleteCriticalSection(&m_cs);
	}
	inline void Enter()
	{
		EnterCriticalSection(&m_cs);
	}

	inline void Leave()
	{
		LeaveCriticalSection(&m_cs);
	}
private :
	CRITICAL_SECTION m_cs;
};
