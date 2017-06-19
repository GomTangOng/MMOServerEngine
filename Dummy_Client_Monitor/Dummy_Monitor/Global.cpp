#include "stdafx.h"

LONG display_array[101][101];
HANDLE g_iocp;
SYSTEM_INFO sysInfo;
SOCKADDR_IN clntAdr;
DWORD recvBytes, flags = 0;
unsigned int randNum;
HINSTANCE g_hInst;
playerInfo player[MAX_CLIENT];
Objlist objectList;
HWND g_hWnd = NULL;
CRITICAL_SECTION cs;
wchar_t *g_ClassName = L"D3D Tutorial";
wchar_t *g_WindowName = L"D3D Tutorial 03 : 정점 Vertex";
HWND hIp;
TCHAR tc_ip[20];
char c_ip[20];
int g_threadCount;
HANDLE *h_threadHandle;


LRESULT CALLBACK	MsgProc(HWND, UINT, WPARAM, LPARAM);
BOOL InitWindow(int width, int height);
void ResizeWindow(HWND hWnd, UINT NewWidth, UINT NewHeight);
BOOL MessagePump();
void DisplayUpdateVB(int display_array_index);
void InitializeDisplay();
void AddRecvObjectList(int user_id);
void AddDisplayArray(int user_id);
bool CheckContainPlayer(int user_id);
int GetPhysicalProcessorsNum(void);
//void RandomMove(CS_Move_Packet* MovePacket, int move_flag);
HRESULT UpdateVB( int row, int col, int input_bSelect );

void ProcessMovePacket(char *packet);
void ProcessLoginOkPacket(int user_id, char *packet);
void ProcessPlayerInfoPacket(int user_id, char *packet);
void ProcessPutObjectPacket(int user_id, char *packet);
bool ProcessPacket(int user_id, char *packet);

DWORD WINAPI ConnectThread(LPVOID para);
DWORD WINAPI WorkerThread(LPVOID para);
DWORD WINAPI UpdateThread(LPVOID para);

BOOL CALLBACK InputServerIpDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);//서버주소입력 대화 상자
//
//void SetNameStr(ID_STR *name, wchar_t *new_name)
//{
//	wcsncpy_s(name->str, new_name, _countof(name->str));
//}
