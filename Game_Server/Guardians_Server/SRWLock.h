#pragma once
#include "stdafx.h"

class CSRWLock
{
public:
	CSRWLock()
	{
		InitializeSRWLock(&m_srwLock);
	}
	~CSRWLock()
	{

	}

	inline void ReadEnter()
	{
		AcquireSRWLockShared(&m_srwLock);
	}

	inline void ReadLeave()
	{
		ReleaseSRWLockShared(&m_srwLock);
	}

	inline void WriteEnter()
	{
		AcquireSRWLockExclusive(&m_srwLock);
	}

	inline void WriteLeave()
	{
		ReleaseSRWLockExclusive(&m_srwLock);
	}
private :
	SRWLOCK m_srwLock;
};

template <class T>
class CSRWLockGaurd
{
	friend class SRWLockAutoGuard;
public :
	struct SRWLockReadAutoGuard
	{
		SRWLockReadAutoGuard()
		{
			T::m_lock.ReadEnter();
		}
		~SRWLockReadAutoGuard()
		{
			T::m_lock.ReadLeave();
		}
	};

	struct SRWLockWriteAutoGuard
	{
		SRWLockWriteAutoGuard()
		{
			T::m_lock.WriteEnter();
		}
		~SRWLockWriteAutoGuard()
		{
			T::m_lock.WriteLeave();
		}
	};
private :
	static CSRWLock m_lock;
};

template <class T>
CSRWLock CSRWLockGaurd<T>::m_lock;