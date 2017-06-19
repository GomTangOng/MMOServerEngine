#include "stdafx.h"



//worker 스레드 생성
void CreateWorkerThread() 
{
	// 총 스레드 갯수에서 update, connect Thread를 빼준 갯수
	for (int i=0; i<g_threadCount-2; ++i)
	{
		h_threadHandle[i] = CreateThread(NULL, 0, WorkerThread, 0, 0, NULL);
	}
}

//connect 스레드 생성
void CreateConnectThread() 
{
	h_threadHandle[g_threadCount-2] = CreateThread(NULL, 0, ConnectThread, 0, 0, NULL);	
}

//move 스레드 생성
void CreateUpdateThread() 
{
	h_threadHandle[g_threadCount-1] = CreateThread(NULL, 0, UpdateThread, 0, 0, NULL);	
}

/////////////////////////////////////////////////////////////////////////////
//
// 메인 함수.
//
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	//------------------------------------------------------------------//
	// 기본적인 윈도우 프레임 생성..                                    //
 	//------------------------------------------------------------------//
	if(!InitWindow(800, 800)) return 0;

	//srand( (unsigned)time( NULL ) ); 
	//------------------------------------------------------------------//
	// D3D 초기화...실패하면 바로 어플리케이션을 종료한다.              //
 	//------------------------------------------------------------------//
 	if(FAILED(InitialD3D(g_hWnd)))
		return 0;

	//------------------------------------------------------------------//
	// 정점 버퍼 생성.★                                                //
	//------------------------------------------------------------------//
	
 	if(FAILED(InitVB()) )
	{
		ReleaseD3D();			//실패하면 메모리 제거하고 종료한다.
		return 0;
	}	

	clntAdr.sin_family = AF_INET;
	clntAdr.sin_addr.s_addr = inet_addr(c_ip);
	clntAdr.sin_port = htons(SERVER_PORT);	

	memset(display_array, 0, sizeof(display_array));
	
	g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);

	// 사용할 스레드 갯수 = 물리코어*2(workerThread) + 2(Connect, Update Thread)
	g_threadCount = GetPhysicalProcessorsNum()*2 + 2;
	h_threadHandle = new HANDLE[g_threadCount];

	Sleep(1000);
	CreateWorkerThread();  
	Sleep(1000);	
	CreateConnectThread();
	Sleep(1000);
	CreateUpdateThread();
	                    
	//------------------------------------------------------------------//
	// 메인  루프														//
	//------------------------------------------------------------------//
	while(1) 
	{
		if(!MessagePump())		// 메세지 펌프.
			break;

		/* 이하 게임 로직 */
		Game();

	}
 	
	//------------------------------------------------------------------//
	// 어플리케이션 종료시, D3D 관련 메모리를 제거한다.					//	
 	//------------------------------------------------------------------//
 	
	WaitForMultipleObjects(g_threadCount, h_threadHandle, TRUE, INFINITE); 

	WSACleanup();

	delete h_threadHandle;

	printf("Dummy Client 종료\n");

	ReleaseD3D();

	return 0;
}
 
/////////////////////////////////////////////////////////////////////////////
//
LRESULT CALLBACK MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_CREATE:
		DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG), HWND_DESKTOP, InputServerIpDlgProc);
		return 0;
	case WM_KEYUP:
		switch(wParam)
		{
		case VK_SPACE :				//와이어 프레임 출력을 위해 키보드 체크.★
			g_bShowWireFrm ^= TRUE;
			break;
		}
		case VK_TAB :{
			static int i = 0;		
			UpdateVB( i, i, 2 );
			i++;			
			break;
		}
		return 0;

 	case WM_DESTROY:
		PostQuitMessage(0); 
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}


BOOL InitWindow(int width, int height)
{
	WNDCLASSEX wc = { 
		sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0, 0,
			::GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
			g_ClassName, NULL 
	};

	RegisterClassEx(&wc);

	HWND hWnd = ::CreateWindow( g_ClassName, g_WindowName, 
								WS_OVERLAPPEDWINDOW, 0, 0, 
								width, height, 
								GetDesktopWindow(), NULL, 
								wc.hInstance, NULL
								);
	if(hWnd == NULL) return FALSE;

	g_hWnd = hWnd;

	::ShowWindow(hWnd, SW_SHOWDEFAULT);
	::UpdateWindow(hWnd);

	ResizeWindow(hWnd, width, height);

	return TRUE;
}

int MessagePump()
{
	MSG msg;	::ZeroMemory(&msg, sizeof(msg));

	while(1)
	{
		//메세지 큐에 메세지가 있으면...처리하고..
		if(::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)			//윈도우 종료를 위해 리턴.
				return FALSE;		

			//나머지 메세지 처리.
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else  //특별한 메세지가 없다면 게임(렌더링)을 진행한다..
		{
			return TRUE;		
		}
	}

	return FALSE;
}


void ResizeWindow(HWND hWnd, UINT NewWidth, UINT NewHeight)
{	
	//현재 윈도우의 스타일 구하기.
	RECT oldrc;
	DWORD sytle  = (DWORD) ::GetWindowLong(hWnd, GWL_STYLE);
	DWORD exstyle = (DWORD) ::GetWindowLong(hWnd, GWL_EXSTYLE);

	//현재 윈도우 '전체' 크기 (스크린좌표) 를 얻는다.
	::GetWindowRect(hWnd, &oldrc);

	//새로 생성될 윈도우의 '클라이언트' 영역 크기 계산하기.
	RECT newrc;
	newrc.left = 0;  
	newrc.top = 0;
	newrc.right = NewWidth;
	newrc.bottom = NewHeight;

	::AdjustWindowRectEx(&newrc, sytle, NULL, exstyle);


	//보정된 윈도우의 너비와 폭을 구한다. 
	int width = (newrc.right - newrc.left);
	int height = (newrc.bottom - newrc.top);
	 

	//새로운 크기를 윈도우에 설정한다.
	::SetWindowPos(hWnd, HWND_NOTOPMOST, 
					oldrc.left, oldrc.top, 
 					width, height, SWP_SHOWWINDOW);

}


/*************** end of "main.cpp" *****************************/
