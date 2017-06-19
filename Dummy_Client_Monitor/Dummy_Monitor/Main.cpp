#include "stdafx.h"



//worker ������ ����
void CreateWorkerThread() 
{
	// �� ������ �������� update, connect Thread�� ���� ����
	for (int i=0; i<g_threadCount-2; ++i)
	{
		h_threadHandle[i] = CreateThread(NULL, 0, WorkerThread, 0, 0, NULL);
	}
}

//connect ������ ����
void CreateConnectThread() 
{
	h_threadHandle[g_threadCount-2] = CreateThread(NULL, 0, ConnectThread, 0, 0, NULL);	
}

//move ������ ����
void CreateUpdateThread() 
{
	h_threadHandle[g_threadCount-1] = CreateThread(NULL, 0, UpdateThread, 0, 0, NULL);	
}

/////////////////////////////////////////////////////////////////////////////
//
// ���� �Լ�.
//
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	//------------------------------------------------------------------//
	// �⺻���� ������ ������ ����..                                    //
 	//------------------------------------------------------------------//
	if(!InitWindow(800, 800)) return 0;

	//srand( (unsigned)time( NULL ) ); 
	//------------------------------------------------------------------//
	// D3D �ʱ�ȭ...�����ϸ� �ٷ� ���ø����̼��� �����Ѵ�.              //
 	//------------------------------------------------------------------//
 	if(FAILED(InitialD3D(g_hWnd)))
		return 0;

	//------------------------------------------------------------------//
	// ���� ���� ����.��                                                //
	//------------------------------------------------------------------//
	
 	if(FAILED(InitVB()) )
	{
		ReleaseD3D();			//�����ϸ� �޸� �����ϰ� �����Ѵ�.
		return 0;
	}	

	clntAdr.sin_family = AF_INET;
	clntAdr.sin_addr.s_addr = inet_addr(c_ip);
	clntAdr.sin_port = htons(SERVER_PORT);	

	memset(display_array, 0, sizeof(display_array));
	
	g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);

	// ����� ������ ���� = �����ھ�*2(workerThread) + 2(Connect, Update Thread)
	g_threadCount = GetPhysicalProcessorsNum()*2 + 2;
	h_threadHandle = new HANDLE[g_threadCount];

	Sleep(1000);
	CreateWorkerThread();  
	Sleep(1000);	
	CreateConnectThread();
	Sleep(1000);
	CreateUpdateThread();
	                    
	//------------------------------------------------------------------//
	// ����  ����														//
	//------------------------------------------------------------------//
	while(1) 
	{
		if(!MessagePump())		// �޼��� ����.
			break;

		/* ���� ���� ���� */
		Game();

	}
 	
	//------------------------------------------------------------------//
	// ���ø����̼� �����, D3D ���� �޸𸮸� �����Ѵ�.					//	
 	//------------------------------------------------------------------//
 	
	WaitForMultipleObjects(g_threadCount, h_threadHandle, TRUE, INFINITE); 

	WSACleanup();

	delete h_threadHandle;

	printf("Dummy Client ����\n");

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
		case VK_SPACE :				//���̾� ������ ����� ���� Ű���� üũ.��
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
		//�޼��� ť�� �޼����� ������...ó���ϰ�..
		if(::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)			//������ ���Ḧ ���� ����.
				return FALSE;		

			//������ �޼��� ó��.
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else  //Ư���� �޼����� ���ٸ� ����(������)�� �����Ѵ�..
		{
			return TRUE;		
		}
	}

	return FALSE;
}


void ResizeWindow(HWND hWnd, UINT NewWidth, UINT NewHeight)
{	
	//���� �������� ��Ÿ�� ���ϱ�.
	RECT oldrc;
	DWORD sytle  = (DWORD) ::GetWindowLong(hWnd, GWL_STYLE);
	DWORD exstyle = (DWORD) ::GetWindowLong(hWnd, GWL_EXSTYLE);

	//���� ������ '��ü' ũ�� (��ũ����ǥ) �� ��´�.
	::GetWindowRect(hWnd, &oldrc);

	//���� ������ �������� 'Ŭ���̾�Ʈ' ���� ũ�� ����ϱ�.
	RECT newrc;
	newrc.left = 0;  
	newrc.top = 0;
	newrc.right = NewWidth;
	newrc.bottom = NewHeight;

	::AdjustWindowRectEx(&newrc, sytle, NULL, exstyle);


	//������ �������� �ʺ�� ���� ���Ѵ�. 
	int width = (newrc.right - newrc.left);
	int height = (newrc.bottom - newrc.top);
	 

	//���ο� ũ�⸦ �����쿡 �����Ѵ�.
	::SetWindowPos(hWnd, HWND_NOTOPMOST, 
					oldrc.left, oldrc.top, 
 					width, height, SWP_SHOWWINDOW);

}


/*************** end of "main.cpp" *****************************/
