#include "stdafx.h"
#include "World.h"


CWorld::CWorld()
{
	SetRespownPosition(XMFLOAT3(0, 0, 0));	// юс╫ц

	for (int y = 0; y < SECTOR_Y_LENGTH; ++y)
	{
		for (int x = 0; x < SECTOR_X_LENGTH; ++x)
		{
			m_sectors[y][x].SetStartX(x * SECTOR_WIDTH);
			m_sectors[y][x].SetStartY(y * SECTOR_HEIGHT);
		}
	}
}


CWorld::~CWorld()
{
}
