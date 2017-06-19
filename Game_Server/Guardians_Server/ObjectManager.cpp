#include "stdafx.h"
#include "ObjectManager.h"
#include "protocol.h"
#include "Memory.h"
#include "SessionManager.h"
#include <cstdlib>

CObjectManager::CObjectManager()
{
	m_nextAttackerMonsterID  = ATTACKER_MONSTER_START;
	m_nextDefenderMonsterID  = DEFENDER_MONSTER_START;
	m_nextSupporterMonsterID = SURPPORT_MONSTER_START;

	m_nextItemID    = 1;

	m_monsterMappingHashMap.clear();
	m_itemMappingHashMap.clear();
}


CObjectManager::~CObjectManager()
{
}

bool CObjectManager::Start()
{
	for (int i = 0; i < MAX_MONSTER; ++i)
	{
		CMonster *pMonster = new CMonster();
		pMonster->Initalize();			// Warning : Initalize -> Clear -> SetIndex 순서 바뀌면 안됨		
		pMonster->SetIndex(i);

		InsertMonsterToMap(i, pMonster);

		if (0 == i)
		{
			pMonster->SetMonsterType(MonsterType::MONSTER_TYPE_END);
			continue;			// 0은 더미 NPC라서 풀에 넣을 필요없음
		}
		else if (i < DEFENDER_MONSTER_START)
		{
			pMonster->SetMonsterType(MonsterType::ATTACKER);
		}
		else if(i < SURPPORT_MONSTER_START)
		{
			pMonster->SetMonsterType(MonsterType::DEFENDER);
		}
		else
		{
			pMonster->SetMonsterType(MonsterType::SUPPORTER);
		}

		m_monsterPool.push(pMonster);
	}

	return true;
}

void CObjectManager::ShutDown()
{
	for (auto mon : m_monsterMap)
	{
		Memory::SAFE_DELETE(mon.second);
	}
	m_monsterMap.clear();
	m_monsterPool.clear();
	m_monsterMappingHashMap.clear();
}

void CObjectManager::InsertMonsterToMap(const UINT index, CMonster * pMon)
{
	m_monsterMap[index] = pMon;
}

void CObjectManager::InsertMappingData(const UINT id, const UINT idx)
{
	MonsterMappingHashMap::accessor a;
	if (!m_monsterMappingHashMap.insert(a, make_pair(id, idx)))
	{
#if defined(_DEBUG)
		cout << "CObjectManager::InsertMappingData : Insert  Failed\n";
#endif
	}
}

void CObjectManager::InsertMonsterToPool(CMonster * pMon)
{
	m_monsterPool.push(pMon);
}

void CObjectManager::DeleteMonster(const UINT id)
{
	CMonster* pMon = FindMonster(id);

	if (m_monsterMappingHashMap.erase(id))
	{
#ifdef _DEBUG
		cout << "CObjectManager Mapping HashMap -> Monster Erase : " << id << endl;
#endif
	}
	InsertMonsterToPool(pMon);

	pMon->Clear();
}

void CObjectManager::DeleteMonster(CMonster * pMonster)
{
	if (m_monsterMappingHashMap.erase(pMonster->GetID()))
	{
#ifdef _DEBUG
		cout << "CObjectManager Mapping HashMap -> Monster Erase : " << pMonster->GetID() << endl;
#endif
	}
	InsertMonsterToPool(pMonster);

	pMonster->Clear();
}

CMonster* CObjectManager::FindMonster(const UINT id)
{
	UINT index = FindIndexFromMappingTable(id);

	auto npc_data = m_monsterMap.find(index);

	return npc_data->second;			
}

UINT CObjectManager::FindIndexFromMappingTable(const UINT id)
{
	MonsterMappingHashMap::const_accessor citer;
	if (!m_monsterMappingHashMap.find(citer, id))
	{
#ifdef _DEBUG
		//cout << "CObjectManager::FindIndexFromMappingTable : Monster Not Find\n";
#endif
		return 0;		// 0은 더미 객체의 인덱스
	}
	return citer->second;
}

CMonster * CObjectManager::GetNewMonster(const int mon_type)
{
	if (mon_type == MonsterType::MONSTER_TYPE_END) return m_monsterMap[0];

	CMonster* pMon;

	if (m_monsterPool.try_pop(pMon))
	{
		if (mon_type == MonsterType::ATTACKER)
		{
			pMon->SetID(m_nextAttackerMonsterID++);
		}
		else if (mon_type == MonsterType::DEFENDER)
		{
			pMon->SetID(m_nextDefenderMonsterID++);
		}
		else
		{
			pMon->SetID(m_nextSupporterMonsterID++);
		}
	
		pMon->SetMonsterType((MonsterType)mon_type);

		InsertMappingData(pMon->GetID(), pMon->GetIndex());

		return pMon;
	}
	return m_monsterMap[0];
}
