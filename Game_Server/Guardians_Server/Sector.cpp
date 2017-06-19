#include "stdafx.h"
#include "Sector.h"
#include "SessionManager.h"
#include "ObjectManager.h"

CSector::CSector()
{
	for (int y = 0; y < SECTOR_HEIGHT / CELL_HEIGHT; ++y)
	{
		for (int x = 0; x < SECTOR_WIDTH / CELL_WIDTH; ++x)
		{
			m_cells[y][x].SetPivot(m_startX * x, m_startY * y);	// startX¿Í startY´Â ÁÂ»ó´Ü ±âÁØ ÁÂÇ¥
		}
	}
}


CSector::~CSector()
{
	m_playerListInSector.clear();
}

void CSector::AddPlayer(const UINT id)
{
	assert(id >= 0);
	//m_player_list_lock.WriteEnter();	
		m_playerListInSector.insert(id);
	//m_player_list_lock.WriteLeave();
}

void CSector::DelPlayer(const UINT id)
{
	assert(id >= 0);

	m_player_list_lock.WriteEnter();
		m_playerListInSector.unsafe_erase(id);
	m_player_list_lock.WriteLeave();
}

void CSector::AddMonster(const UINT id)
{
	assert(id >= 0);
	//m_monster_list_lock.WriteEnter();
		m_monsterListInSector.insert(id);
	//m_monster_list_lock.WriteLeave();
}

void CSector::DelMonster(const UINT id)
{
	assert(id >= 0);
	m_monster_list_lock.WriteEnter();
		m_monsterListInSector.unsafe_erase(id);
	m_monster_list_lock.WriteLeave();
}

void CSector::AddItem(const UINT id)
{
}

void CSector::DelItem(const UINT id)
{
}

void CSector::RegisterPlayerInRange(CPlayer * owner_player, CPlayer::ViewList& out_list)
{
	assert(owner_player != nullptr);

	//m_player_list_lock.ReadEnter();
	for (auto target_id : m_playerListInSector)
	{
		CClientSession *pSession = SESSION_MANAGER->FindSession(target_id);

		if (!pSession->IsConnected()) continue;
		if (target_id == owner_player->GetID()) continue;

		CPlayer *targetObject = SESSION_MANAGER->FindSession(target_id)->GetPlayer();
		
		if (owner_player->GetViewBoundingSphere().Contains(XMLoadFloat3(&targetObject->GetViewBoundingSphere().Center)))
		{	
			out_list.insert(target_id);
		}
	}
	//m_player_list_lock.ReadLeave();
}

void CSector::RegisterPlayerInRange(CMonster * monster, CPlayer::ViewList & out_set)
{
	assert(monster != nullptr);

	CPlayer*        target_object;
	//int nRef = 0;
	//m_player_list_lock.ReadEnter();
	for (auto target_id : m_playerListInSector)
	{
		target_object = SESSION_MANAGER->FindSession(target_id)->GetPlayer();
		
		if (target_object->GetViewBoundingSphere().Contains(XMLoadFloat3(&monster->GetViewBoundingSphere().Center))) 
		{
			out_set.insert(target_id);
		}
	}
	//monster->SetRef(nRef);
	//m_player_list_lock.ReadLeave();
}

void CSector::RegisterMonsterInRange(CPlayer * owner_player, ObjectList& out_set)
{
	assert(owner_player != nullptr);

	//m_monster_list_lock.ReadEnter();
	for (auto target_id : m_monsterListInSector)
	{
		CMonster *targetObject = OBJECT_MANAGER->FindMonster(target_id);

		if (owner_player->GetViewBoundingSphere().Contains(XMLoadFloat3(&targetObject->GetViewBoundingSphere().Center)))
		{
			out_set.insert(target_id);	
		}
	}
	//m_monster_list_lock.ReadLeave();
}

void CSector::RegisterItemInRange(CPlayer * player, ObjectList & out_set)
{
}
