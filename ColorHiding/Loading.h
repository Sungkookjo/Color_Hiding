#pragma once

struct LOADVERTEX
{
	D3DXVECTOR3 position;
	D3DCOLOR color;
	FLOAT tu, tv;
};

#define D3DFVF_LOADVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

#define TEX_MAX 5

class Loading{
private:
	LARGE_INTEGER		m_liPre;
	LARGE_INTEGER		m_liCur;
	LARGE_INTEGER		m_liFreq;
	float				m_fTime;
	static Loading*		s_pSingle;
	LPDIRECT3DDEVICE9	m_lpDevice;
	BOOL*				m_pIsFinish;
	LPDIRECT3DTEXTURE9	m_lpTexture[TEX_MAX];
	LPDIRECT3DVERTEXBUFFER9	m_lpVB;
	D3DXVECTOR3			m_vPos[TEX_MAX];
	D3DXVECTOR3			m_vSize[TEX_MAX];
	float				m_fRot[TEX_MAX];

	UINT				m_iCurTex;
	Basefw_FS*			m_pFS;

	static unsigned int __stdcall Update(VOID* arg);
	VOID FrameMove();
	VOID Render();
private:
	void SetMat(D3DXVECTOR3 pPos,float pRot,D3DXVECTOR3 pSize);
public:
	Loading(Basefw_FS* FS);
	~Loading();

	void Run(BOOL* pFlag);
	HRESULT Init(
		HWND hWnd
		,LPDIRECT3D9 pDev
		);
};