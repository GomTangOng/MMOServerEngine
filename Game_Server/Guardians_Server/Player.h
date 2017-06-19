#pragma once

#include "protocol.h"
#include "Entity.h"
#include "SRWLock.h"

class CPlayer : public CEntity 
{
public:
	//typedef unordered_set<UINT> ViewList;
	typedef tbb::concurrent_unordered_set<UINT> ViewList;

	CPlayer();
	~CPlayer();

	void Clear() override;

	void Move(const float delta_time);
	void MoveX(const float delta_time);
	void MoveY(const float delta_time);
	void MoveZ(const float delta_time);	

	void IncreaseHP(const int delta_hp);
	void DecreaseHP(const int delta_hp);

	// 나중에 XML에서 플레이어 데이터 가져옴

	void AddPlayerInList(UINT player_id);
	void DelPlayerInList(UINT player_id);
	void AddMonsterInList(UINT monster_id);
	void DelMonsterInList(UINT monster_id);

	// 기존의 섹터와 새로운 섹터가 같으면 true 아니면 false
	bool UpdateSector(int new_sector_x, int new_sector_y);
private :
	ViewList        m_oldViewPlayerList;
	ViewList        m_oldViewMonsterList;
	ViewList        m_oldItemList;

	char            m_chatBuffer[MAX_CHAT_SIZE];

	CSRWLock        m_playerViewLock;
	CSRWLock		m_monsterViewLock;
	CSRWLock		m_itemViewLock;
	CSRWLock        m_sectorLock;
public :
	// Setter
	void SetPositionX(const float x) { m_position.x = x; m_collisionBoundingSphere.Center.x = x; m_viewBoundingSphere.Center.x = x; m_collisionBoundingBox.Center.x = x; m_collisionOrientedBox.Center.x = x; }
	void SetPositionY(const float y) { m_position.y = y; m_collisionBoundingSphere.Center.y = y; m_viewBoundingSphere.Center.y = y; m_collisionBoundingBox.Center.y = y;	m_collisionOrientedBox.Center.y = y; }	// 3D때는 Z도 고쳐야함
	void SetPositionZ(const float z) { m_position.z = z;}	
	void SetPosition(const XMFLOAT3& position) { SetPositionX(position.x); SetPositionY(position.y); SetPositionZ(position.z); }
	void SetPosition(const float x, const float y, const float z) { SetPositionX(x); SetPositionY(y); SetPositionZ(z);}

	void SetChatBuffer(char *chatBuf, const int size);
	// Getter
	      char*			  GetChatBuffer()               { return m_chatBuffer; }
		  CSRWLock&       GetPlayerViewLock()           { return m_playerViewLock; }
		  CSRWLock&		  GetMonsterViewLock()			{ return m_monsterViewLock; }
		  CSRWLock&		  GetItemViewLock()				{ return m_itemViewLock; }
		  CSRWLock&       GetSectorLock()               { return m_sectorLock; }
		  ViewList&       GetOldViewPlayerList()        { return m_oldViewPlayerList; }
		  ViewList&       GetOldViewMonsterList()	    { return m_oldViewMonsterList; }
		  ViewList&       GetOldViewItemList()			{ return m_oldItemList; }
	const int			  GetSectorX()			  const { return m_sector_x; }
	const int			  GetSectorY()			  const { return m_sector_y; }
};

