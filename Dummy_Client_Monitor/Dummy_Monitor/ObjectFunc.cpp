#include "stdafx.h"

bool CheckContainPlayer(int user_id)
{
	return (objectList.find(user_id) != objectList.end());
}
