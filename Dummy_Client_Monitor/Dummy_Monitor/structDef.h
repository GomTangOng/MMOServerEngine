#pragma once
#include "MemoryPool.h"

#define MAX_BUFF_SIZE 4096

typedef struct {
	WSAOVERLAPPED	m_Overlapped;
	INT				m_io_type;
} OVERLAPPED_EX;

struct SEND_STRUCT{
	OVERLAPPED_EX	m_OverExSend;
	WSABUF			m_SendWSABuf;
	CHAR			m_SendBuffer[MAX_BUFF_SIZE];
};

struct location {
	int x;
	int y;
};

//void SetNameStr(ID_STR *name, wchar_t *new_name);
