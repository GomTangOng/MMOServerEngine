#include "d3d9.h"
#include "d3dx9.h"
#include <vector>
#pragma warning (disable : 4244)
///////////////////////////////////////////////////////////////////////////// 
//���� ����

using namespace std;

LPDIRECT3D9				g_pD3D		= NULL;
LPDIRECT3DDEVICE9		g_pDevice	= NULL;

BOOL g_bShowWireFrm = FALSE;

LPDIRECT3DVERTEXBUFFER9	g_pVB;

//�ﰢ�� ����ü
struct CUSTOMVERTEX
{
	float x, y, z;
	DWORD color;
	
	CUSTOMVERTEX(){};
	CUSTOMVERTEX(float pX, float pY, float pZ, DWORD pColor) :
		x(pX), y(pY), z(pZ), color(pColor)
	{
	}
};

//�ڽ� ����ü
struct BOX{
	 D3DXMATRIX mTrans;				//�̵������� ���� ���   <- �ʿ����  ..nhjung
	 int bSelect;					//�ڽ� ���� ������ �÷���
};

//�ڽ� ����
#define ROW 100
#define COL 100

BOX g_pBox[ROW][COL];
CUSTOMVERTEX g_pVerticesList[6 * ROW * COL];

//�ڽ� ũ��
#define WIDTH 7.0f
#define HEIGHT 1.0f

#define D3DFVF_CUSTOMVERTEX ( D3DFVF_XYZ | D3DFVF_DIFFUSE )

HRESULT InitVB()
{	

	for (int i=0;i<100;++i)
		for (int j=0;j<100;++j) {
			int index = (i * 100 + j) * 6;
			g_pVerticesList[index++] = CUSTOMVERTEX( i * 7    , j * 7,     0.5, D3DCOLOR_ARGB(255,255,255,255));
			g_pVerticesList[index++] = CUSTOMVERTEX( i * 7 + 6, j * 7,     0.5, D3DCOLOR_ARGB(255,255,255,255));
			g_pVerticesList[index++] = CUSTOMVERTEX( i * 7    , j * 7 + 6, 0.5, D3DCOLOR_ARGB(255,255,255,255));
			g_pVerticesList[index++] = CUSTOMVERTEX( i * 7    , j * 7 + 6, 0.5, D3DCOLOR_ARGB(255,255,255,255));
			g_pVerticesList[index++] = CUSTOMVERTEX( i * 7 + 6, j * 7    , 0.5, D3DCOLOR_ARGB(255,255,255,255));
			g_pVerticesList[index++] = CUSTOMVERTEX( i * 7 + 6, j * 7 + 6, 0.5, D3DCOLOR_ARGB(255,255,255,255));
		}

	//���� ���� ����.
	if(FAILED(g_pDevice->CreateVertexBuffer(	
				sizeof(g_pVerticesList),			//'���� ����'�� ũ�� (����Ʈ)
				0,													// ���� ó�� ���� 
				D3DFVF_CUSTOMVERTEX,		//'����' ��Ÿ�� 
				D3DPOOL_MANAGED,			// ���������� ��ġ MANAGED ��õ
				&g_pVB,								// ������ ���ϵǴ� ���� �����ͤ�
				NULL											// �����. �׳� NULL.
				)))
	{
		printf("���ؽ����� ���� ����!!!\n��");
		return -1;
	}
	
	for( int i =0; i < ROW; ++i ){
		for( int j = 0; j < COL; ++j ){

			BOX pBox;

			//�ڽ� ������ ����
			pBox.bSelect = 0;

			//�ڽ���� �ʱ�ȭ
			D3DXMatrixIdentity(&pBox.mTrans);

			//��ġ���� ����
			D3DXMatrixTranslation(&pBox.mTrans, i*(WIDTH+1.0f),  j*(WIDTH+1.0f), 0.0f );

			//�޸� ����
			::memcpy( &g_pBox[i][j], &pBox, sizeof(BOX) );
		}
	}

	return S_OK;
	
}//end of HRESULT InitVB()

HRESULT UpdateVB( int row, int col, int input_bSelect )
{
	g_pBox[row][col].bSelect = input_bSelect;
	return S_OK;
	
}//end of HRESULT InitVB()

int CompareVB( int index, DWORD color )
{
	return 0;
}

void SetupCam(){

	// view matrix�� �Ź� ���� �����ϴ� ���� ��ȿ����   ..nhjung
	D3DXMATRIX	mView;
	D3DXMatrixIdentity(&mView);
	g_pDevice->SetTransform(D3DTS_VIEW, &mView);	

	D3DXMATRIX	mProj;
	D3DXMatrixOrthoOffCenterLH(&mProj, 0, (float)700, (float)700, 0.0f, 0.1f, 1000.0f);		
	g_pDevice->SetTransform(D3DTS_PROJECTION, &mProj);	

}

///////////////////////////////////////////////////////////////////////////// 
//
HRESULT InitialD3D(HWND hwnd)
{
	//D3D ��ü�� �����Ѵ�.
	if(NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;

	//ȭ�� ���� �Ķ���� ����ü �ʱ�ȭ.
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));

	//ȭ�� ���� �Ķ���� ����. ����� 'â'��� �� �����Ѵ�.
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect  = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	//'��ġ(Device)'�� �����Ѵ�.
	if(FAILED(g_pD3D->CreateDevice(
						D3DADAPTER_DEFAULT,		//0�� ���� ���.
						D3DDEVTYPE_HAL,					//�ϵ���� '����'
						hwnd,											//������ ���� �ڵ�.
						//D3DCREATE_SOFTWARE_VERTEXPROCESSING,		//���� ó�� ���.
						D3DCREATE_HARDWARE_VERTEXPROCESSING,		//���� ó�� ���.
						&d3dpp,																				//ȭ�� ���� '�ɼ�'
						&g_pDevice																		//������ ��ġ�� �����͸� ���� �����ͺ���.
						)))
	{
		return E_FAIL;
	}
	
	g_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	return S_OK;
}
///////////////////////////////////////////////////////////////////////////// 
//
void ReleaseD3D()
{
	if (g_pVB) g_pVB->Release();

	if(g_pDevice)
	{
		g_pDevice->Release();		
		g_pDevice = NULL;
	}

	if(g_pD3D)
	{
		g_pD3D->Release();
		g_pD3D = NULL;
	}

}

///////////////////////////////////////////////////////////////////////////// 
//
void Game()
{
	if(g_pDevice == NULL) return;

	g_pDevice->SetRenderState(D3DRS_FILLMODE, 
						(g_bShowWireFrm)?D3DFILL_WIREFRAME:D3DFILL_SOLID);
	 
	g_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);	

	SetupCam();


	for (int i=0;i<100;++i)
		for (int j=0;j<100;++j) {
			int index = (i * 100 + j) * 6;
			int count = g_pBox[i][j].bSelect;
			int intensity;
			if (count == 0) intensity = 0;
			else if (count == 1) intensity = 127;
			else {
				intensity = min(255, 127 + count*16);
			}
			intensity = 255 - intensity;
			int color = D3DCOLOR_ARGB(255, intensity, intensity, 255);
			g_pVerticesList[index++].color = color;
			g_pVerticesList[index++].color = color;
			g_pVerticesList[index++].color = color;
			g_pVerticesList[index++].color = color;
			g_pVerticesList[index++].color = color;
			g_pVerticesList[index++].color = color;
		}

	//'���(Scene) �׸���' ����
	if(SUCCEEDED(g_pDevice->BeginScene()))	
	{	
		g_pDevice->SetFVF(D3DFVF_CUSTOMVERTEX);


	//���� ä���.
	VOID*	pVertices;
	
	if(FAILED(g_pVB->Lock(0, sizeof(g_pVerticesList), (void**)&pVertices, 0 )))
	{
		printf("���ؽ����� Locking ����!!\n");
		return;
	}
	
	memcpy(pVertices, g_pVerticesList, sizeof(g_pVerticesList));

	g_pVB->Unlock();
		g_pDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
		D3DXMATRIX mTrans;
		D3DXMatrixIdentity(&mTrans);
		g_pDevice->SetTransform(D3DTS_WORLD, &mTrans);
		g_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 6 * 100 * 100);
		g_pDevice->EndScene();
	}
	g_pDevice->Present(NULL, NULL, NULL, NULL);

}

/***************** End of "CreateDevice.cpp" *******************************/