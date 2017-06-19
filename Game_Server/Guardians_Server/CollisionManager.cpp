#include "stdafx.h"
#include "CollisionManager.h"


CCollisionManager::CCollisionManager()
{
}


CCollisionManager::~CCollisionManager()
{
}

bool CCollisionManager::IsCollision(const DirectX::BoundingSphere & a, const DirectX::BoundingSphere & b)
{
	return a.Intersects(b);
}

bool CCollisionManager::IsCollision(const DirectX::BoundingBox & a, const DirectX::BoundingBox & b)
{
	return a.Intersects(b);
}

bool CCollisionManager::IsCollision(const DirectX::BoundingOrientedBox & a, const DirectX::BoundingOrientedBox & b)
{
	return a.Intersects(b);
}
