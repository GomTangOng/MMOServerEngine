// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "mswsock.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "cudart.lib")
//#pragma comment(lib, "d3dx11.lib")

#if defined(_DEBUG)
	#pragma comment(lib, "tbb_debug.lib")
#else
	#pragma comment(lib,"tbb.lib")
#endif

#include "targetver.h"

#include <WinSock2.h>
#include <MSWSock.h>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <assert.h>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <thread>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <thread>
#include <chrono>
#include <process.h>
#include <functional>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <queue>
using namespace std;
using namespace std::chrono;

#include "simulationTest.h"

// DirectX
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
using namespace DirectX;

// TBB
#include "../tbb/tbb.h"
#include "cuda.h"
// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
