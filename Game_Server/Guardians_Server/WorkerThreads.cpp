#include "stdafx.h"
#include "WorkerThreads.h"
#include "NetEngine.h"
#include "PacketManager.h"
#include "ClientSession.h"
#include "ObjectManager.h"

CWorkerThreads::CWorkerThreads()
{
	m_nWorkerThread = 0;
}


CWorkerThreads::~CWorkerThreads()
{
}

void CWorkerThreads::WaitThreads()
{
	for (auto thread : m_vWorkerThreads)
	{
		thread->join();
	}
}

bool CWorkerThreads::Create(const int nWorkerThread)
{
	RegisterProcessFunction();	// Operation 처리 함수들 해쉬테이블에 등록

	if (nWorkerThread == 0)
	{
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		m_nWorkerThread = si.dwNumberOfProcessors * 2;
	}
	else
	{
		m_nWorkerThread = nWorkerThread;
	}

	m_vWorkerThreads.reserve(m_nWorkerThread);

	for (int i = 0; i < m_nWorkerThread; ++i)
	{
		m_vWorkerThreads.push_back(new thread{ CWorkerThreads::EntryPoint, this });
	}
	return true;
}

void CWorkerThreads::Release()
{
	m_delOverlappedExFunc.clear();

	for (auto thread : m_vWorkerThreads)
	{
		Memory::SAFE_DELETE(thread);
	}
	m_vWorkerThreads.clear();
}

void CWorkerThreads::RegisterProcessFunction()
{
	m_operationFuncHashTable.reserve((int)OPERATION_TYPE::OP_END);

	m_operationFuncHashTable[(int)OPERATION_TYPE::OP_ZERO_RECV] = bind(&CWorkerThreads::ZeroRecvProcess, this, placeholders::_1, placeholders::_2);
	m_operationFuncHashTable[(int)OPERATION_TYPE::OP_RECV]      = bind(&CWorkerThreads::RecvProcess, this, placeholders::_1, placeholders::_2);
	m_operationFuncHashTable[(int)OPERATION_TYPE::OP_ONCE_SEND] = bind(&CWorkerThreads::OnceSendProcess, this, placeholders::_1, placeholders::_2);
	m_operationFuncHashTable[(int)OPERATION_TYPE::OP_SEND]      = bind(&CWorkerThreads::SendProcess, this, placeholders::_1, placeholders::_2);
	m_operationFuncHashTable[(int)OPERATION_TYPE::OP_NPC_MOVE]      = bind(&CWorkerThreads::MoveProcess, this, placeholders::_1, placeholders::_2);

	m_delOverlappedExFunc.reserve((int)OPERATION_TYPE::OP_END);

	m_delOverlappedExFunc[(int)OPERATION_TYPE::OP_ZERO_RECV] = std::bind(&CWorkerThreads::DeleteZeroRecvOverlappedEx, this, placeholders::_1);
	m_delOverlappedExFunc[(int)OPERATION_TYPE::OP_RECV] = std::bind(&CWorkerThreads::DeleteRecvOverlappedEx, this, placeholders::_1);
	m_delOverlappedExFunc[(int)OPERATION_TYPE::OP_ONCE_SEND] = std::bind(&CWorkerThreads::DeleteOnceSendOverlappedEx, this, placeholders::_1);
	m_delOverlappedExFunc[(int)OPERATION_TYPE::OP_SEND] = std::bind(&CWorkerThreads::DeleteZeroRecvOverlappedEx, this, placeholders::_1);
	m_delOverlappedExFunc[(int)OPERATION_TYPE::OP_NPC_MOVE] = std::bind(&CWorkerThreads::DeleteMoveOverlappedEx, this, placeholders::_1);
}


void CWorkerThreads::EntryPoint(void * arg)
{
	CWorkerThreads* wt = (CWorkerThreads *)arg;

	wt->Run();
}

void CWorkerThreads::ZeroRecvProcess(const DWORD transferred, const UINT id)
{
#if defined(ZERO_BUFFER)
	CClientSession *pClientSession = SESSION_MANAGER->FindSession(id);

	if (!pClientSession->PostRecv())
	{
		printf("[종료] ID : %d", pClientSession->GetID());
		SESSION_MANAGER->DeleteSession(pClientSession);		
	}
#endif
}

void CWorkerThreads::RecvProcess(const DWORD transferred, const UINT id)
{
	CClientSession *pClientSession = SESSION_MANAGER->FindSession(id);

	PACKET_MANAGER->Assemble(transferred, pClientSession);	// 패킷 조립 + 패킷 처리
#if defined(ZERO_BUFFER)
	pClientSession->PreRecv();
#else
	pClientSession->PostRecv();
#endif
}

void CWorkerThreads::OnceSendProcess(const DWORD transferred, const UINT id)
{
	// Warning : 워커쓰레드 안에서 cout 찍을때도 제대로 찍히려면... 락걸어야함
	//cout << dwTrasnferred << "바이트 전송했습니다." << endl;
}

void CWorkerThreads::SendProcess(const DWORD transferred, const UINT id)
{
	//pClientSession->SendCompletion(transferred);
}

void CWorkerThreads::MoveProcess(const DWORD transferred, const UINT id)
{
	CMonster *pMon = OBJECT_MANAGER->FindMonster(id);
	pMon->Move();
	pMon->SetExecute(false);
}

UINT CWorkerThreads::Run()
{
	int ret;
	DWORD dwError;

	while (1)
	{
		DWORD dwTrasnferred{ 0 };
		ULONG id{ 0 };
		OverlappedEx *over_ex{ nullptr };

		ret = GetQueuedCompletionStatus(NETWORK_ENGINE->GetIOCPHandle(), 
			                            &dwTrasnferred, 
			                            &id, 
										reinterpret_cast<LPOVERLAPPED *>(&over_ex), 
			                            INFINITE);

		if (ret == 0 || dwTrasnferred == 0)		// Note : 종료 처리 조건 확인해 볼 것
		{
			if (over_ex->operation_type == OPERATION_TYPE::OP_RECV || over_ex->operation_type == OPERATION_TYPE::OP_ONCE_SEND)
			{
				CClientSession *pClientSession = SESSION_MANAGER->FindSession(id);
				assert(pClientSession != nullptr);

				CPlayer* pPlayer = pClientSession->GetPlayer();

				sc_packet_remove_player remove_pkt;
				remove_pkt.size      = sizeof(sc_packet_remove_player);
				remove_pkt.type      = SC_REMOVE_PLAYER;
				remove_pkt.player_id = pClientSession->GetID();

#ifdef VIEW_PROCCESS	
				SESSION_MANAGER->BroadCastInView(reinterpret_cast<char *>(&remove_pkt), pClientSession->GetID());
#else
				SESSION_MANAGER->BroadCast(reinterpret_cast<char *>(&remove_pkt), pClientSession->GetID());
#endif
#ifdef _DEBUG
				printf("[종료] ID : %d\n", pClientSession->GetID());
				SESSION_MANAGER->DeleteSession(pClientSession);		
#endif			
				continue;
			}
		}

		assert(over_ex->operation_type < OPERATION_TYPE::OP_END);
	
		m_operationFuncHashTable[(int)over_ex->operation_type](dwTrasnferred, id);	// IO 처리
		m_delOverlappedExFunc[(int)over_ex->operation_type](over_ex);
#if defined(ONCE_SEND)

#else
		pClientSession->PostSend();
#endif
		
	}
	return 0;
}


void CWorkerThreads::DeleteZeroRecvOverlappedEx(OverlappedEx * overlappedEx)
{
	delete static_cast<ZeroRecvOverlappedEx *>(overlappedEx);
}

void CWorkerThreads::DeleteRecvOverlappedEx(OverlappedEx * overlappedEx)
{
	delete static_cast<RecvOverlappedEx *>(overlappedEx);
}

void CWorkerThreads::DeleteOnceSendOverlappedEx(OverlappedEx * overlappedEx)
{
	delete static_cast<OnceSendOverlappedEx *>(overlappedEx);
}

void CWorkerThreads::DeleteSendOverlappedEx(OverlappedEx * overlappedEx)
{
	delete static_cast<SendOverlappedEx *>(overlappedEx);
}

void CWorkerThreads::DeleteMoveOverlappedEx(OverlappedEx * overlappedEx)
{
	delete static_cast<MoveOverlappedEx *>(overlappedEx);
}
