#pragma once

#define GAGE_COVER	0
#define GAGE_BAR	1

class ColorGage : public O2GUIBase{
private:
	virtual BOOL IsInRect(POINT& pt) { return false; }

	O2GUIObject*		m_pGage;					//	������ Ŀ���� ������ ��
	LPDIRECT3DTEXTURE9	m_pTextures[COLOR_MAX];		//	������ ������ �ؽ�ó
	UINT				m_CurColor;					//	���� ��
	VOID*				m_pHero;					//	���ΰ� ������

	O2Rect				m_vBarRect;					//	������ ���� �⺻ ��Ʈ
	D3DXVECTOR3			m_vBarPos;					//	������ ���� �⺻ ��ġ
public:
	ColorGage();
	~ColorGage();
	// �׸���
	virtual void Render();
	virtual UINT Update(POINT& pt,UINT _MsState);
	// �ʱ�ȭ
	virtual HRESULT Init(tstring filename
		,ResourceManager* pResource
		,LPDIRECT3DDEVICE9* pDev
		,POINT	imagesize
		,O2Rect pRect = O2Rect(0,0,0,0)
		,D3DXVECTOR3 pPos = D3DXVECTOR3(0.0f,0.0f,0.0f)
		,UINT		_Center=GUI_CENTER_CENTER
		);

	VOID	SetHero(LPVOID	pHero) { m_pHero = pHero; }
	HRESULT	InitGageBar(POINT imagesize ,O2Rect pRect ,D3DXVECTOR3 pPos);
	VOID	SetGageTexture(UINT	_Color,tstring pTex);
};