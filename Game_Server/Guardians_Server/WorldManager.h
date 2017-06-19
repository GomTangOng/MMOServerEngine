#pragma once

#include "World.h"

#define WORLD_MANAGER CWorldManager::GetInstance()

class CClientSession;
class CWorldManager
{
public:
	CWorldManager();
	~CWorldManager();

	bool Start();
	void ShutDown();

	static CWorldManager* GetInstance()
	{
		static CWorldManager instance;
		return &instance;
	}

	bool UpdateSector(CClientSession *pOwnerSession);
	void ViewProcess(CClientSession *pOwnerSession);
	void PlayerViewProcess(CClientSession *pOwnerSession);
	void MonsterViewProcess(CClientSession *pOwnerSession);
	void ItemViewProcess(CClientSession *pOwnerSession);
private :
	tbb::concurrent_unordered_map<UINT, CWorld*> m_worlds;
public :
	// Getter
	CWorld* GetWorld(UINT world_type) { return m_worlds[world_type]; }
	template <class T>
	int GetNewSectorXWithObject(T *pObject) { return pObject->GetPositionX() / SECTOR_WIDTH; }
	template <class T>
	int GetNewSectorYWithObject(T *pObject) { return pObject->GetPositionY() / SECTOR_HEIGHT; }
};
