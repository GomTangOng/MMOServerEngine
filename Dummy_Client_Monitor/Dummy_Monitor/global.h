#pragma once

#include "protocol.h"

extern LONG display_array[101][101];
extern HANDLE g_iocp;
extern SYSTEM_INFO sysInfo;
extern SOCKADDR_IN clntAdr;
extern DWORD recvBytes, flags;
extern unsigned int randNum;
extern HINSTANCE g_hInst;
extern playerInfo player[MAX_CLIENT];
typedef map<int, location> Objlist;
extern Objlist objectList;
extern HWND g_hWnd;
extern CRITICAL_SECTION cs;
extern wchar_t *g_ClassName;
extern wchar_t *g_WindowName;
extern HWND hIp;
extern TCHAR tc_ip[20];
extern char c_ip[20];
extern int g_threadCount;
extern HANDLE *h_threadHandle;

extern LRESULT CALLBACK	MsgProc(HWND, UINT, WPARAM, LPARAM);
extern BOOL InitWindow(int width, int height);
extern void ResizeWindow(HWND hWnd, UINT NewWidth, UINT NewHeight);
extern BOOL MessagePump();
extern void DisplayUpdateVB(int display_array_index);
extern void InitializeDisplay();
extern void AddRecvObjectList(int user_id);
extern void AddDisplayArray(int user_id);
extern bool CheckContainPlayer(int user_id);
extern int GetPhysicalProcessorsNum(void);
//extern void RandomMove(CS_Move_Packet* MovePacket, int move_flag);
extern HRESULT UpdateVB( int row, int col, int input_bSelect );

extern void ProcessMovePacket(char *packet);
extern void ProcessLoginOkPacket(int user_id, char *packet);
extern void ProcessPlayerInfoPacket(int user_id, char *packet); 
extern void ProcessPutObjectPacket(int user_id, char *packet);
extern bool ProcessPacket(int user_id, char *packet);

extern DWORD WINAPI ConnectThread(LPVOID para);
extern DWORD WINAPI WorkerThread(LPVOID para);
extern DWORD WINAPI UpdateThread(LPVOID para);

extern BOOL CALLBACK InputServerIpDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);