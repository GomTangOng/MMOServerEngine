#include "d3d9.h"
#include "d3dx9.h"
#include <vector>
#pragma warning (disable : 4244)
///////////////////////////////////////////////////////////////////////////// 
//전역 변수

using namespace std;

LPDIRECT3D9				g_pD3D		= NULL;
LPDIRECT3DDEVICE9		g_pDevice	= NULL;

BOOL g_bShowWireFrm = FALSE;

LPDIRECT3DVERTEXBUFFER9	g_pVB;

//삼각형 구조체
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

//박스 구조체
struct BOX{
	 D3DXMATRIX mTrans;				//이동정보를 담을 행렬   <- 필요없음  ..nhjung
	 int bSelect;					//박스 색깔 선택지 플래그
};

//박스 갯수
#define ROW 100
#define COL 100

BOX g_pBox[ROW][COL];
CUSTOMVERTEX g_pVerticesList[6 * ROW * COL];

//박스 크기
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

	//정점 버퍼 생성.
	if(FAILED(g_pDevice->CreateVertexBuffer(	
				sizeof(g_pVerticesList),			//'정점 버퍼'의 크기 (바이트)
				0,													// 버퍼 처리 유형 
				D3DFVF_CUSTOMVERTEX,		//'정점' 스타일 
				D3DPOOL_MANAGED,			// 정점버퍼의 위치 MANAGED 추천
				&g_pVB,								// 성공시 리턴되는 버퍼 포인터ㅣ
				NULL											// 예약됨. 그냥 NULL.
				)))
	{
		printf("버텍스버퍼 생성 오류!!!\nㅜ");
		return -1;
	}
	
	for( int i =0; i < ROW; ++i ){
		for( int j = 0; j < COL; ++j ){

			BOX pBox;

			//박스 선택지 설정
			pBox.bSelect = 0;

			//박스행렬 초기화
			D3DXMatrixIdentity(&pBox.mTrans);

			//위치정보 설정
			D3DXMatrixTranslation(&pBox.mTrans, i*(WIDTH+1.0f),  j*(WIDTH+1.0f), 0.0f );

			//메모리 복사
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

	// view matrix를 매번 새로 생성하는 것은 비효율적   ..nhjung
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
	//D3D 객체를 생성한다.
	if(NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;

	//화면 설정 파라미터 구조체 초기화.
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));

	//화면 설정 파라미터 셋팅. 현재는 '창'모드 로 설정한다.
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect  = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	//'장치(Device)'를 생성한다.
	if(FAILED(g_pD3D->CreateDevice(
						D3DADAPTER_DEFAULT,		//0번 비디오 어뎁터.
						D3DDEVTYPE_HAL,					//하드웨어 '가속'
						hwnd,											//생성할 윈도 핸들.
						//D3DCREATE_SOFTWARE_VERTEXPROCESSING,		//정점 처리 방법.
						D3DCREATE_HARDWARE_VERTEXPROCESSING,		//정점 처리 방법.
						&d3dpp,																				//화면 설정 '옵션'
						&g_pDevice																		//생성된 장치의 포인터를 받을 포인터변수.
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

	//'장면(Scene) 그리기' 시작
	if(SUCCEEDED(g_pDevice->BeginScene()))	
	{	
		g_pDevice->SetFVF(D3DFVF_CUSTOMVERTEX);


	//버퍼 채우기.
	VOID*	pVertices;
	
	if(FAILED(g_pVB->Lock(0, sizeof(g_pVerticesList), (void**)&pVertices, 0 )))
	{
		printf("버텍스버퍼 Locking 오류!!\n");
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