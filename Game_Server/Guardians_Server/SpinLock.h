#pragma once
#include "stdafx.h"

/* 
 *  
 * 
 * 
 */

class CSpinLock
{
public :
	CSpinLock(DWORD dwSpinCount = 4000)
	{
		InitializeCriticalSectionAndSpinCount(&m_cs, dwSpinCount);
	}
	~CSpinLock()
	{
		DeleteCriticalSection(&m_cs);
	}

	inline void Enter()
	{
		EnterCriticalSection(&m_cs);
	}

	inline void	Leave()
	{
		LeaveCriticalSection(&m_cs);
	}
private :
	CRITICAL_SECTION m_cs;
};

template <class T>
class CSpinLockGuard
{
	friend class SpinLockAutoGaurd;
public :
	struct SpinLockAutoGaurd
	{
	public :
		SpinLockAutoGaurd()
		{
			T::m_spinLock.Enter();
		}
		~SpinLockAutoGaurd()
		{
			T::m_spinLock.Leave();
		}
	};
private :
	static CSpinLock m_spinLock;
};

template <class T>
CSpinLock CSpinLockGuard<T>::m_spinLock;