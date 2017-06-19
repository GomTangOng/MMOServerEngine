#pragma once

#include "State.h"

template <class OwnerType>
class CStateMachine
{
public:
	//CStateMachine();
	//~CStateMachine();

	//void ChangeState(UINT new_state);
	//void RevertToPreviousState();
private :
	OwnerType* m_pOwner;

	CState<OwnerType> *m_pCurrentState;
	CState<OwnerType> *m_pPreviousState;
	CState<OwnerType> *m_pGolbalState;
public :
};

