#pragma once
#include "stdafx.h"
#include "../tbb/tbb.h"

template <class T, int ALLOC_BLOCK_COUNT = 10001>
class CMemoryPool
{
public :
	CMemoryPool()
	{
		
	}
	~CMemoryPool()
	{
			
	}

	static void* operator new(size_t allocLength)
	{
		if (m_freeList.empty()) AllocBlock();
		
		T* data;

		m_freeList.try_pop(data);

		return data;
	}

	static void operator delete(void *pDel)
	{
		m_freeList.push(reinterpret_cast<T *>(pDel));
	}

	static void AllocBlock()
	{
		for (int i = 0; i < ALLOC_BLOCK_COUNT; ++i)
		{
			T* data = (T *)malloc(sizeof(T));
			m_freeList.push(data);
		}
	}
private :
	static tbb::concurrent_queue<T *> m_freeList;
};

template <class T, int ALLOC_BLOCK_COUNT>
tbb::concurrent_queue<T *> CMemoryPool<T, ALLOC_BLOCK_COUNT>::m_freeList;
