#pragma once

#include "Monster.h"

#define OBJECT_MANAGER CObjectManager::GetInstance()

class CObjectManager
{
public:
	typedef tbb::concurrent_queue<CMonster *> MonsterPool;
	typedef tbb::concurrent_hash_map<UINT /* ID */, UINT /* INDEX */> MonsterMappingHashMap;
	typedef tbb::concurrent_unordered_map<UINT /* INDEX */, CMonster *> MonsterMap;

	typedef tbb::concurrent_queue<CMonster *> ItemPool;
	typedef tbb::concurrent_hash_map<UINT /* ID */, UINT /* INDEX */> ItemMappingHashMap;
	typedef tbb::concurrent_unordered_map<UINT /* INDEX */, CMonster *> ItemMap;

	CObjectManager();
	~CObjectManager();

	static CObjectManager* GetInstance()
	{
		static CObjectManager instance;
		return &instance;
	}
	bool	  Start();
	void	  ShutDown();

	void      DeleteMonster(const UINT id);
	void      DeleteMonster(CMonster *pMonster);

	CMonster* FindMonster(const UINT id);
	UINT      FindIndexFromMappingTable(const UINT id);

	CMonster* GetNewMonster(const int mon_type);
private :
	void	  InsertMonsterToMap(const UINT index, CMonster *pMon);
	void	  InsertMappingData(const UINT id, const UINT idx);
	void	  InsertMonsterToPool(CMonster *pMon);
private :
	atomic<UINT> m_nextAttackerMonsterID;
	atomic<UINT> m_nextDefenderMonsterID;
	atomic<UINT> m_nextSupporterMonsterID;
	atomic<UINT> m_nextItemID;

	MonsterPool           m_monsterPool;
	MonsterMappingHashMap m_monsterMappingHashMap;
	MonsterMap            m_monsterMap;

	ItemPool              m_itemPool;				// 나중에 함수 추가
	ItemMappingHashMap    m_itemMappingHashMap;
	ItemMap               m_itemMap;
public :
	const MonsterMap& GetMonsterMap() { return m_monsterMap; }
};

