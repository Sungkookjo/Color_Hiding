#pragma once

#define GAGE_COVER	0
#define GAGE_BAR	1

class ColorGage : public O2GUIBase{
private:
	virtual BOOL IsInRect(POINT& pt) { return false; }

	O2GUIObject*		m_pGage;					//	게이지 커버랑 게이지 바
	LPDIRECT3DTEXTURE9	m_pTextures[COLOR_MAX];		//	색마다 게이지 텍스처
	UINT				m_CurColor;					//	현재 색
	VOID*				m_pHero;					//	주인공 포인터

	O2Rect				m_vBarRect;					//	게이지 바의 기본 렉트
	D3DXVECTOR3			m_vBarPos;					//	게이지 바의 기본 위치
public:
	ColorGage();
	~ColorGage();
	// 그리기
	virtual void Render();
	virtual UINT Update(POINT& pt,UINT _MsState);
	// 초기화
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