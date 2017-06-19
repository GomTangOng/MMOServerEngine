// Guardians_Server.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
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

