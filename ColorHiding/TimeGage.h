#pragma once

class TimeGage : public O2GUIBase{
private:
	virtual BOOL IsInRect(POINT& pt) { return false; }

	O2GUIObject*		m_pGage;					//	게이지 커버랑 게이지 바

	float*				m_fpDelta	;				//	시간 게이지
	float				m_fMaxTime	;				//	최대 시간
	float				m_fCurTime	;				//	현재 시간

	O2Rect				m_vBarRect;					//	게이지 바의 기본 렉트
	D3DXVECTOR3			m_vBarPos;					//	게이지 바의 기본 위치
public:
	TimeGage();
	~TimeGage();
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

	HRESULT	InitGageBar(tstring _filename,POINT imagesize ,O2Rect pRect ,D3DXVECTOR3 pPos);

	VOID	SetDelta(float* pDelta) { m_fpDelta = pDelta; }
	VOID	SetTime(float	pTime)	{ m_fCurTime = m_fMaxTime = pTime; }
	VOID	InitTimer();
};