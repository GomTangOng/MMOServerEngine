// Guardians_Server.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "NetEngine.h"
#include "MiniDump.h"

int main()
{
	MINIDUMP->Begin();
	if (NETWORK_ENGINE->Start())
	{
		NETWORK_ENGINE->Run();
		MINIDUMP->End();
	}
	NETWORK_ENGINE->Shutdown();

	return 0;
}

