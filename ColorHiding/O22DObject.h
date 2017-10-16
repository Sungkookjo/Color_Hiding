#pragma once

struct OBJ2DVERTEX
{
	D3DXVECTOR3 position;
	D3DCOLOR color;
	FLOAT tu, tv;
};

#define D3DFVF_2DOBJVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

//////////////////////////////////////////////////////////////////////////
//	2D Object
class O22DObject : public O2Object{
protected:
	LPDIRECT3DTEXTURE9			m_lpTextures	;	// �ؽ�ó
	LPDIRECT3DVERTEXBUFFER9		m_lpVB			;	//	���ؽ� ����
	UINT						m_uCenter		;	//	�̹��� �߽�
	O2Rect						m_rImg			;	//	�̹��� ���� �׷��� ��Ʈ��

	HRESULT	CreateVB(UINT _Center,POINT	_ImgSize);

	// ������Ʈ ����
	virtual	VOID		CleanUp();
	virtual	VOID		SetMat();

public:
	O22DObject();
	virtual ~O22DObject();
	virtual HRESULT		RenderShadow(UINT pass) { return S_OK; }
	// �ʱ�ȭ
	virtual	HRESULT		Init(
		LPDIRECT3DDEVICE9*	pDev,
		TCHAR*				filename,
		O2Rect				pRect,
		POINT				imgsize,
		D3DXVECTOR3 pPos,
		D3DXVECTOR3 pSize=D3DXVECTOR3(1.0f,1.0f,1.0f),
		D3DXVECTOR3 pRot=D3DXVECTOR3(0.0f,0.0f,0.0f)
		);
	// ������
	virtual	HRESULT		UpDate();
	// �׸���
	virtual	HRESULT		Render();
	virtual HRESULT		RenderLine() {return S_OK;}
	virtual BOOL		IsRender(O2Frustum* pFrustrum){ m_bIsRend = TRUE; return m_bIsRend; }
};