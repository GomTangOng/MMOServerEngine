#pragma once

#include "Entity.h"

class CMonster : public CEntity
{
public:
	CMonster();
	virtual ~CMonster();

	void Initalize();
	void Clear() override;
	
	void Move();		// ���߿� ���϶��� �����϶��� �����ؼ�...
	bool UpdateSector(int new_sector_x, int new_sector_y);

private :
	MonsterType   m_monsterType;
	UINT          m_index;

	atomic<bool>  m_bPet;
	atomic<bool>  m_bAlive;
	atomic<bool>  m_bExecute;	// AI ���������� �ƴ��� üũ�ϴ� �÷���

	// StateMachine , ������� ��� ���߿� �߰� ����
public :
	// Setter
	void SetPositionX(const float x) {
		m_position.x = x; m_collisionBoundingSphere.Center.x = x; m_viewBoundingSphere.Center.x = x; }	// Warning : x,y,z �� �������� ������� �� �ľ��ϰ�  �����... ������ �߾��̶�� �����ϰ� ���
	void SetPositionY(const float y) {
		m_position.y = y; m_collisionBoundingSphere.Center.y = y; m_viewBoundingSphere.Center.y = y; }	// 3D���� Z�� ���ľ���
	void SetPositionZ(const float z) {
		m_position.z = z; m_collisionBoundingSphere.Center.z = z; m_viewBoundingSphere.Center.z = z; }
	void SetPosition(const XMFLOAT3& position) {
		SetPositionX(position.x); SetPositionY(position.y); SetPositionZ(position.z);
	}
	void SetPosition(const float x, const float y, const float z) { 
		SetPositionX(x); SetPositionY(y); SetPositionZ(z); 
	}
	void SetMonsterType(const MonsterType type) { m_monsterType = type; }
	void SetIndex(const UINT index) { m_index = index; }
	void SetAlive(const bool alive);
	void SetExecute(const bool execute) { m_bExecute = execute; }
	// Getter
	const MonsterType	 GetMonsterType()         { return m_monsterType; }
	const UINT			 GetIndex()         const { return m_index; }
	const atomic<bool>&  GetAlive()		          { return m_bAlive; }
	const atomic<bool>&  IsExecute()        const { return m_bExecute; }
	const atomic<bool>&  IsPet()            const { return m_bPet; }
};

