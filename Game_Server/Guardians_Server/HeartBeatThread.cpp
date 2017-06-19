#include "stdafx.h"
#include "HeartBeatThread.h"
#include "NetEngine.h"
#include "Memory.h"
#include "protocol.h"
#include "ObjectManager.h"
#include "ClientSession.h"

CHeartBeatThread::CHeartBeatThread()
{
}


CHeartBeatThread::~CHeartBeatThread()
{
}

void CHeartBeatThread::WaitThreads()
{
	m_pHeartBeatThread->join();
}

bool CHeartBeatThread::Create()
{
	m_pHeartBeatThread = new thread{ CHeartBeatThread::EntryPoint, this };
	return true;
}

void CHeartBeatThread::Release()
{
	Memory::SAFE_DELETE(m_pHeartBeatThread);
}

#define HEART_BEAT_DURATION 1000

UINT CHeartBeatThread::Run()
{
	auto& session_map = SESSION_MANAGER->GetSessionMap();
	while (1)
	{
		auto start = high_resolution_clock::now();
		for(auto& session : session_map)
		{
			auto& monster_list = session.second->GetPlayer()->GetOldViewMonsterList();

			for (auto& mon_id : monster_list)
			{
				CMonster *pMon = OBJECT_MANAGER->FindMonster(mon_id);
				
				if (pMon->IsExecute() || pMon->IsPet()) continue;
				
				pMon->SetExecute(true);

				MoveOverlappedEx *over_ex = new MoveOverlappedEx();
				over_ex->operation_type = OPERATION_TYPE::OP_NPC_MOVE;			
				PostQueuedCompletionStatus(NETWORK_ENGINE->GetIOCPHandle(), 0, mon_id, &over_ex->overlapped);
			}
		}
		auto duration = high_resolution_clock::now() - start;
		int sleep_time = 1000 - duration_cast<milliseconds>(duration).count();
		//if (sleep_time < 0) sleep_time = 1;			// 디버깅을 위해 임시로~
		//cout << "sleep : " << sleep_time << endl;
		Sleep(sleep_time);
	}

	return 0;
}

void CHeartBeatThread::EntryPoint(void * arg)
{
	CHeartBeatThread *thread = (CHeartBeatThread *)arg;
	thread->Run();
}
