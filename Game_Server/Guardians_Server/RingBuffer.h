#pragma once
#include "SRWLock.h"

#define MAX_QUEUE_SIZE 4096	

class CRingBuffer
{
public:
	CRingBuffer();
	~CRingBuffer();

	int Enqueue(const char *pBuf, const int nSize);
	int Dequeue(char *pOut, const int nSize);

	int Commit_Enqueue(const int nSize);	// Enqueue�� Size��ŭ Tail�� ��ġ�� �̵���Ų��.
	int Commit_Dequeue(const int nSize);	// Dequeue�� Size��ŭ head�� ��ġ�� �̵���Ų��.

	void ReSize();	// Note : ���� ���� ���� �ǰ�
	void Clear();

	inline bool IsPacket(const int packet_size);
	inline bool IsFull();
	inline bool IsFull(int ioSize);
	inline bool IsEmpty();
private:
	char *m_pQueue;

	USHORT m_tail;
	USHORT m_head;

	int m_nCurrentSize;
public:
	// Setter, Getter
	int GetCurrentSize() const { return m_nCurrentSize; }
	int GetEmptySize()   const { return MAX_QUEUE_SIZE - m_tail - 1; }

	char* GetBufStartPosition() { return &m_pQueue[0]; }
	char* GetBufEndPosition()   { return &m_pQueue[MAX_QUEUE_SIZE - 1]; }
	char* GetEuqueuePosition()  { return &m_pQueue[m_tail]; }
	char* GetDequeuePosttion()  { return &m_pQueue[m_head]; }
};

