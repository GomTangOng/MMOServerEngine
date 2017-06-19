#include "stdafx.h"
#include "AcceptThread.h"
#include "NetEngine.h"
#include "Memory.h"
#include "protocol.h"
#include "PacketManager.h"
#include "WorldManager.h"
#include "SessionManager.h"

CAcceptThread::CAcceptThread()
{
}


CAcceptThread::~CAcceptThread()
{
}

void CAcceptThread::WaitThreads()
{
	m_pAcceptThread->join();
}

bool CAcceptThread::Create()
{
	m_pAcceptThread = new thread{ CAcceptThread::EntryPoint, this };
	return false;
}

void CAcceptThread::Release()
{
	Memory::SAFE_DELETE(m_pAcceptThread);
}

UINT CAcceptThread::Run()
{
	
	return 0;
}

void CAcceptThread::EntryPoint(void * arg)
{
	CAcceptThread *thread = (CAcceptThread *)arg;
	thread->Run();
}
