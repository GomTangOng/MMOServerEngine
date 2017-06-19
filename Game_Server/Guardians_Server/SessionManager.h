#pragma once

#include "ClientSession.h"

#define SESSION_MANAGER CSessionManager::GetInstance()

class CSessionManager
{
public:
	typedef tbb::concurrent_queue<CClientSession *> SessionPool;
	typedef tbb::concurrent_hash_map<UINT /* ID */, UINT /* INDEX */> SessionMappingHashMap;
	typedef tbb::concurrent_unordered_map<UINT /* INDEX */, CClientSession *> SessionMap;

	CSessionManager();
	~CSessionManager();

	static CSessionManager* GetInstance()
	{
		static CSessionManager instance;
		return &instance;
	}

	bool Start();
	void ShutDown();
						
	CClientSession* GetNewClientFromPool(SOCKET sock, const SOCKADDR_IN& sockaddr);	// WSAAccept Version
	
	void DeleteSession(CClientSession *pSession);
	void DeleteSession(UINT id);

	CClientSession* FindSession(UINT id);
	CPlayer*        FindPlayer(UINT id);

	void BroadCast(char *pkt, const UINT id = 0);
	void BroadCastInView(char *pkt, const UINT id);
private:
	UINT FindIndexFromMappingTable(UINT id);

	void InsertSessionToMap(const UINT index, CClientSession *pSession);
	void InsertMappingData(const UINT id, const UINT idx);
	void InsertSessionToPool(CClientSession *pSession);
private :
	SessionMap            m_sessionsMap;
	SessionPool			  m_sessionsPool;
	SessionMappingHashMap m_sessionMappingHashMap;	// <  PLAYER ID , PLAYER INDEX >

	atomic<UINT> m_nextPlayerID;
public :
	// Getter, Setter
	const SessionMap& GetSessionMap() { return m_sessionsMap; }
};

