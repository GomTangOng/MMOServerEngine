#pragma once

template <class OwnerType>
class CState
{
public:
	//CState();
	//~CState();

	virtual void Enter(OwnerType *) = 0;
	virtual void Execute(OwnerType *) = 0;
	virtual void Exit(OwnerType *) = 0;
};

