#pragma once

#ifdef _DEBUG
#pragma comment(lib,"tbb_debug.lib")
#else
#pragma comment(lib,"tbb.lib")
#endif


#define _CRT_RAND_S
#define IO_RECV 0
#define IO_SEND 1
//#define MAX_PLAYER_NUM		1000
//#define TEST_PLAYER_NUM		100
//#define MAX_BUFF_SIZE		1024
//#define READ				3
//#define WRITE				5
//#define PORT				9000
#define MAX_WORKER_THREAD	4
//#define MAP_X_LENGTH		100
//#define MAP_Y_LENGTH		100
//#define HOT_SPOT_OFF

#include <iostream>
#include <list>
#include <map>
#include <algorithm>  
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <process.h>
#include <WinSock2.h>
#include <WinBase.h>
#include <Windows.h>
#include <time.h>
#include <d3d9types.h>
#include <tchar.h>

using namespace std;

#include "structDef.h"
#include "playerInfo.h"
#include "global.h"
#pragma comment(lib, "ws2_32.lib")
#include "Device.h"
#include "resource.h"
#pragma comment (linker , "/entry:WinMainCRTStartup /subsystem:console")  
#pragma warning (disable : 4244)