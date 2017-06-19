#pragma once

#include "type.h"
#include "protocol.h"

class CCell
{
public:
	CCell();
	~CCell();
private :
	int m_index;
	float m_pivotX, m_pivotY;
	CellType m_type;
public :
	void SetIndex(const int index) { m_index = index; }
	void SetCellType(CellType type) { m_type = type; }
	// ¼¿ÀÇ Áß¾ÓÀ» ÇÇº¿À¸·Î ³õ±â À§ÇÑ ÀÛ¾÷
	void SetPivot(const float topLeftX, const float topLeftY) { m_pivotX = topLeftX + CELL_WIDTH/2.0f; m_pivotY = topLeftY + CELL_HEIGHT/2.0f; }

	const int GetIndex() const { return m_index; }
	const CellType GetType() const { return m_type; }

	const float GetPivotX() const { return m_pivotX; }
	const float GetPivotY() const { return m_pivotY; }
};

