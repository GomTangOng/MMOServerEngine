#include "stdafx.h"
#include "SessionManager.h"
#include "protocol.h"

CSessionManager::CSessionManager()
{
	m_nextPlayerID = 1;
}


CSessionManager::~CSessionManager()
{
}

bool CSessionManager::Start()
{
	for (int i = 0; i < MAX_CLIENT; ++i)
	{
		CClientSession *pSession = new CClientSession();
		pSession->Initalize();			// Warning : Initalize -> Clear -> SetIndex 순서 바뀌면 안됨		
		pSession->SetIndex(i);
		
		InsertSessionToMap(i, pSession);

		if (0 == i) continue;			// 0은 더미 클라라서 풀에 넣을 필요없음
		m_sessionsPool.push(pSession);
	}
	return true;
}

void CSessionManager::ShutDown()
{
	for (auto client : m_sessionsMap)
	{
		Memory::SAFE_DELETE(client.second);
	}
	m_sessionMappingHashMap.clear();
	m_sessionsPool.clear();
	m_sessionsMap.clear();
}


CClientSession * CSessionManager::GetNewClientFromPool(SOCKET sock, const SOCKADDR_IN& sockaddr)	
{
	CClientSession *pNewSession;

	if (m_sessionsPool.try_pop(pNewSession))
	{
		pNewSession->SetSocket(sock);
		pNewSession->SetSockAddr(sockaddr);

		pNewSession->SetID(m_nextPlayerID++);			// ★ 이 부분에서 ID 새로 할당 

		InsertMappingData(pNewSession->GetID(), pNewSession->GetIndex());
														// 미리 세션맵에 세션들을 할당해놨기 때문에 따로 넣어줄 필요는없음

		return pNewSession;
	}
	
	return nullptr;
}

void CSessionManager::DeleteSession(CClientSession * pSession)
{
	if (m_sessionMappingHashMap.erase(pSession->GetID()))
	{
#ifdef _DEBUG
		cout << "Session Mapping HashMap -> Player Erase : " << pSession->GetID() << endl;
#endif
	}
	InsertSessionToPool(pSession);

	pSession->Reset();	
}

void CSessionManager::DeleteSession(UINT id)
{
	CClientSession *pSession = FindSession(id);

	if (m_sessionMappingHashMap.erase(id))
	{
#ifdef _DEBUG
		cout << "Session Mapping HashMap -> Player Erase : " << pSession->GetID() << endl;
#endif
	}
	InsertSessionToPool(pSession);

	pSession->Reset();
}


CClientSession * CSessionManager::FindSession(UINT id)
{
	UINT index = FindIndexFromMappingTable(id);

	auto session_data = m_sessionsMap.find(index);
	
	return session_data->second;			// Error : 플레이어를 껐을때 어느 순간 nullptr 에러가남
}

CPlayer * CSessionManager::FindPlayer(UINT id)
{
	return FindSession(id)->GetPlayer();
}

UINT CSessionManager::FindIndexFromMappingTable(UINT id)
{
	SessionMappingHashMap::const_accessor citer;
	if (!m_sessionMappingHashMap.find(citer, id))
	{
#ifdef _DEBUG
		cout << "CSessionManager::FindIndexFromMappingTable : Session Not Find\n";
#endif
		return 0;		// 0은 더미 객체의 인덱스
	}
	return citer->second;
}

void CSessionManager::InsertSessionToMap(const UINT index, CClientSession *pSession)
{
	m_sessionsMap[index] = pSession;
	//m_sessionsMap.insert(make_pair(index, pSession));
}

void CSessionManager::InsertMappingData(const UINT id, const UINT idx)
{
	SessionMappingHashMap::accessor a;
	if (!m_sessionMappingHashMap.insert(a, make_pair(id, idx)))
	{
		//cout << "CSessionManager::InsertMappingData : Insert Fail\n";
	}
}

void CSessionManager::InsertSessionToPool(CClientSession *pSession)
{
	m_sessionsPool.push(pSession);
}

void CSessionManager::BroadCast(char * pkt, const UINT id)
{
	for (auto& client : m_sessionsMap)
	{
#if defined(ONCE_SEND) 
		if (client.second->GetID() == id) continue;
		if (false == client.second->IsConnected()) continue;

		client.second->OnceSend(pkt);
#else
		client.second->PreSend(pkt);
#endif
	}
}

void CSessionManager::BroadCastInView(char * pkt, const UINT id)
{
	CClientSession *pClient = FindSession(id);
	CPlayer        *pPlayer = pClient->GetPlayer();

	CPlayer::ViewList& view_list = pPlayer->GetOldViewPlayerList();
	
	if (view_list.empty()) return;

	pPlayer->GetPlayerViewLock().ReadEnter();
	for (auto other_id : view_list)
	{
		FindSession(other_id)->OnceSend(pkt);
	}pPlayer->GetPlayerViewLock().ReadLeave();
}
