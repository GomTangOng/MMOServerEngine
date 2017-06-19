#pragma once

HRESULT		InitialD3D(HWND hwnd);
void		ReleaseD3D();
void		Game();	

struct CUSTOMVERTEX
{
	float x, y, z;
	float rhw;
	DWORD color;
};

HRESULT InitVB();		//정점 버퍼 생성/초기화 함수.★
int CompareVB( int index, DWORD color );
extern BOOL g_bShowWireFrm;		//와이어 프레임 출력 플래그.★

/********* end of "CreateDevice.h" *****************************************/