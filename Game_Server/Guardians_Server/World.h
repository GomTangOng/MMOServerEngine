#pragma once

#include "Sector.h"

class CWorld
{
public:
	CWorld();
	~CWorld();

private :
	CSector m_sectors[SECTOR_Y_LENGTH][ SECTOR_X_LENGTH];

	UINT     m_worldType;

	XMFLOAT3 m_respownPosition;		// 초기에 넣어할 녀석
public :
	// Setter
	void SetRespownPosition(const XMFLOAT3& res_pos) { m_respownPosition = res_pos; }
	void SetWorldType(const UINT world_type) { m_worldType = world_type; }

	// Getter
	CSector* GetSector(int sector_x, int sector_y) { return &m_sectors[sector_y][sector_x]; }
	const XMFLOAT3& GetRespownPosition() { return m_respownPosition; }
	const UINT GetWorldType() { return m_worldType; }
};

