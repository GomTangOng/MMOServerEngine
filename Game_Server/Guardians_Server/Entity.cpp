#include "stdafx.h"
#include "Entity.h"
#include "World.h"
#include "Sector.h"

CEntity::CEntity()
{
	m_id   = -1;
	m_entityType = EntityType::ENTITY_END;
	m_name = nullptr;
}


CEntity::~CEntity()
{
	m_name.clear();
}

CEntity::CEntity(EntityType type)
{
	m_id   = 0;
	m_entityType = type;
}

void CEntity::Clear()
{
	m_id = 0;
	m_name.clear();
	m_entityType = EntityType::ENTITY_END;
	m_position   = XMFLOAT3(0, 0, 0);
	m_direction  = XMFLOAT3(0, 0, 0);
	m_speed = 0;
	m_hp = 0;
	m_sector_x = m_sector_y = 0;
}

void CEntity::SetName(char * name, const int size)
{
	assert(name != nullptr);
	assert(size != 0);
	
	m_name = name;
}

