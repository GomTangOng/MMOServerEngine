#pragma once

#include "Cell.h"
#include "Player.h"
#include "Monster.h"
#include "SRWLock.h"

class CSector
{
public:
	//typedef unordered_set<UINT> ObjectList;
	typedef tbb::concurrent_unordered_set<UINT> ObjectList;

	CSector();
	~CSector();

	void AddPlayer(const UINT id);
	void DelPlayer(const UINT id);

	void AddMonster(const UINT id);
	void DelMonster(const UINT id);

	void AddItem(const UINT id);
	void DelItem(const UINT id);

	void RegisterPlayerInRange(CPlayer * owner_player, CPlayer::ViewList& out_set);
	void RegisterPlayerInRange(CMonster *monster, CPlayer::ViewList& out_set);
	void RegisterMonsterInRange(CPlayer * owner_player, ObjectList& out_set);
	void RegisterItemInRange(CPlayer * owner_player, ObjectList& out_set);
private:
	float m_startX, m_startY;
	CCell m_cells[SECTOR_HEIGHT / CELL_HEIGHT][SECTOR_WIDTH / CELL_WIDTH];

	ObjectList m_playerListInSector;
	ObjectList m_monsterListInSector;
	ObjectList m_itemListInSector;

	CSRWLock   m_monster_list_lock;
	CSRWLock   m_player_list_lock;
	CSRWLock   m_item_list_lock;
public:
	// Setter
	void SetStartX(const float x) { m_startX = x; }
	void SetStartY(const float y) { m_startY = y; }
	// Getter
	ObjectList& GetPlayerList()  { return m_playerListInSector; }
	ObjectList& GetNPCList()     { return m_monsterListInSector; }
	ObjectList& GetItemList()    { return m_itemListInSector; }
	CCell* GetCell(int x, int y) { return &m_cells[y][x]; }
};

