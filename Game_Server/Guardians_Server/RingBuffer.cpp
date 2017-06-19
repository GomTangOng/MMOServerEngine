#include "stdafx.h"
#include "RingBuffer.h"

CRingBuffer::CRingBuffer()
{
	m_pQueue = new char[MAX_QUEUE_SIZE];
	ZeroMemory(&m_pQueue[0], MAX_QUEUE_SIZE);

	m_head = m_tail = 0;

	m_nCurrentSize = 0;
}


CRingBuffer::~CRingBuffer()
{
	delete[] m_pQueue;
	m_pQueue = nullptr;
}

int CRingBuffer::Enqueue(const char * pBuf, const int nSize)
{
	for (int i = 0; i < nSize; ++i)
	{
		*(m_pQueue + m_tail) = *(pBuf + i);
		m_tail = (m_tail + 1) % MAX_QUEUE_SIZE;
	}
	m_nCurrentSize += nSize;
	return nSize;
}


int CRingBuffer::Dequeue(char * pOut, const int nSize)
{
	for (int i = 0; i < nSize; ++i)
	{
		*(pOut + i) = *(m_pQueue + m_head);
		*(m_pQueue + m_head) = 0;
		m_head = (m_head + 1) % MAX_QUEUE_SIZE;
	}

	m_nCurrentSize -= nSize;

	return nSize;
}

int CRingBuffer::Commit_Enqueue(const int nSize)
{
	m_nCurrentSize += nSize;
	m_tail = (m_tail + nSize) % MAX_QUEUE_SIZE;

	return nSize;
}

int CRingBuffer::Commit_Dequeue(const int nSize)
{
	m_nCurrentSize -= nSize;
	m_head = (m_head + nSize) % MAX_QUEUE_SIZE;
	
	return nSize;
}

void CRingBuffer::ReSize()
{
	// Note : �ϴ��� ����X... ����ũ�⸦ �ִ�� Ű����
	//		  ReSize�� ���� �ʰ� ������ -> ���� Copy Overhead�� ������ ����
}

void CRingBuffer::Clear()
{
	m_nCurrentSize = 0;
	m_head = m_tail = 0;
	//m_nTotalSize = MAX_QUEUE_SIZE;			// Warning : ReSize�� �����ϰ� �Ǹ� �� �ڵ� �����ؾ��Ѵ�.
}

bool CRingBuffer::IsPacket(const int packet_size)
{
	if (packet_size > m_nCurrentSize) return false;

	return true;
}

bool CRingBuffer::IsFull()
{
	if (m_nCurrentSize == MAX_QUEUE_SIZE - 1) return true;
	return false;
}

inline bool CRingBuffer::IsFull(int ioSize)
{
	if (m_nCurrentSize + ioSize >= MAX_QUEUE_SIZE - 1) return true;
	return false;
}

bool CRingBuffer::IsEmpty()
{
	if (!m_nCurrentSize) return true;
	return false;
}
