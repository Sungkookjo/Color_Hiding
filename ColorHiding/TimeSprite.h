#pragma once

class TimeSprite:public O2GUIObject{
protected:
	virtual BOOL IsInRect(POINT& pt) { return false; }

	float*			m_fpDelta;
	float			m_fMaxTime;
	float			m_fCurTime;

	O2Rect				m_vBaseRect;	//	기본렉트
public:
	TimeSprite();
	~TimeSprite();

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

	VOID	SetDelta(float* pDelta) { m_fpDelta = pDelta; }
	VOID	SetTime(float	pTime)	{ m_fCurTime = m_fMaxTime = pTime; }
};