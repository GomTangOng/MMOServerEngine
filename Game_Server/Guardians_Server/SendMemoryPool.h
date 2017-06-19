#pragma once

#include "MemoryPool.h"
#include "protocol.h"

class CSendMemoryPool : public CMemoryPool<CSendMemoryPool>
{
public :
	CSendMemoryPool();
	~CSendMemoryPool();
private :
	UCHAR size[MAX_CS_PACKET_SIZE];		// Warning : Ŭ�󿡼� �� ��쿡�� MAX_CS_PACKET_SIZE
										//			 �����϶��� MAX_SC_PACKET_SIZE
};

