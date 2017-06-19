#pragma once
#include "type.h"

class CWorld;
class CSector;
class CEntity
{
public:
	CEntity();
	~CEntity();
	CEntity(EntityType type);

	virtual void Clear();

private :
	UINT   m_id;
	int    m_entityType;
	string m_name;

protected :
	XMFLOAT3        m_position;
	XMFLOAT3        m_direction;
	float             m_speed;
	int				m_hp;

	CWorld*         m_pWorld;
	CSector*        m_pSector;
	int             m_sector_x, m_sector_y;

	BoundingSphere  m_collisionBoundingSphere;
	BoundingSphere  m_viewBoundingSphere;
	BoundingBox			m_collisionBoundingBox;
	BoundingOrientedBox m_collisionOrientedBox;
public :
	// Setter, Getter
	void SetPositionX(const float x) { m_position.x = x; }
	void SetPositionY(const float y) { m_position.y = y; }	// 3D때는 Z도 고쳐야함
	void SetPositionZ(const float z) { m_position.z = z; }
	void SetPosition(const XMFLOAT3& position) { m_position = position; }
	void SetDirectionX(const float dx) { m_direction.x = dx; }
	void SetDirectionY(const float dy) { m_direction.y = dy; }
	void SetDirectionZ(const float dz) { m_direction.z = dz; }
	void SetDirection(const float dx, const float dy, const float dz) { m_direction.x = dx; m_direction.y = dy; m_direction.z = dz; }
	void SetSpeed(const float speed) { m_speed = speed; }
	void SetType(EntityType type) { m_entityType = type; }
	void SetName(char *name, const int size);
	void SetID(const int id) { m_id = id; }
	void SetHP(const int hp) { m_hp = hp; }		// 락 필요???	
	void SetCollisionRadious(const float radius) { m_collisionBoundingSphere.Radius = radius; }
	void SetWorld(CWorld* pWorld) { m_pWorld = pWorld; }
	void SetSector(CSector* pSector) { m_pSector = pSector; }
	void SetSectorX(const int x) { m_sector_x = x; }
	void SetSectorY(const int y) { m_sector_y = y; }
	void SetSectorXY(const int x, const int y) { m_sector_x = x; m_sector_y = y; }
	void SetViewSphereRadious(const float radius) { m_viewBoundingSphere.Radius = radius; }

	const UINT			  GetID()   			  const { return m_id; }
	const int			  GetEntityType()		  const { return m_entityType; }
	int&				  GetHP()					    { return m_hp; }
	const string&		  GetName()					    { return m_name; }
	const float			  GetPositionX()	      const { return m_position.x; }
	const float			  GetPositionY()	      const { return m_position.y; }
	const float			  GetPositionZ()	      const { return m_position.z; }
	XMVECTOR              GetPositionXM()               { return XMLoadFloat3(&m_position); }
	const XMFLOAT3&		  GetPosition()		      const { return m_position; }
	XMFLOAT3&			  GetDirection()                { return m_direction; }
	const float			  GetSpeed()              const { return m_speed; }
	CWorld*               GetWorld()              const { return m_pWorld; }
	CSector*		      GetSector()			  const { return m_pSector; }
	const BoundingSphere& GetBoundingSphere()     const { return m_collisionBoundingSphere; }
	const BoundingSphere& GetViewBoundingSphere() const { return m_viewBoundingSphere; }
	const BoundingBox& GetBoundingBox() const { return m_collisionBoundingBox; }
	const BoundingOrientedBox& GetOrientedBoundingBox() const { return m_collisionOrientedBox; }
};

