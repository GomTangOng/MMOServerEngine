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

HRESULT InitVB();		//���� ���� ����/�ʱ�ȭ �Լ�.��
int CompareVB( int index, DWORD color );
extern BOOL g_bShowWireFrm;		//���̾� ������ ��� �÷���.��

/********* end of "CreateDevice.h" *****************************************/