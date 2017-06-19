#pragma once

#include "stdafx.h"
#include "CSingleton.h"

#define COLLISION_MANAGER CCollisionManager::GetInstance()

class CCollisionManager : public CSingleton<CCollisionManager>
{
public:
	CCollisionManager();
	~CCollisionManager();

	static CCollisionManager* GetInstance()
	{
		static CCollisionManager instance;
		return &instance;
	}

	bool IsCollision(const DirectX::BoundingSphere &a, const DirectX::BoundingSphere &b);
	bool IsCollision(const DirectX::BoundingBox &a, const DirectX::BoundingBox &b);
	bool IsCollision(const DirectX::BoundingOrientedBox &a, const DirectX::BoundingOrientedBox &b);

private :
};
