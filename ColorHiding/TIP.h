#pragma once
#include "O2GUIOnce.h"
#include "O2InputManager.h"

class O2TIP: public O2GUIBase
{
private:

	float* m_fDelta;				//delta 시간
	float  m_fDrawTime;				//그리는 시간
	float  m_fCurTime;				//현재 시간
	int	   m_iCurImage;				//현재 이미지
	int	   m_iNumImage;				//이미지 갯수
	BOOL   m_bSlide;				//이미지를 그려줄지
	BOOL   m_bOnce;					//한번만 보이기
	std::vector<O2GUIOnce*>		m_pTiptList;
	O2GUIOnce*					m_pBack;
	O2InputManager*			m_pInputMgr ;
	ResourceManager*		m_pResource	;
	LPDIRECT3DDEVICE9*		m_lpDevice	;

public:
	O2TIP();
	~O2TIP();

	virtual BOOL IsInRect(POINT& pt) { return false; }
	virtual HRESULT Init(tstring filename
		,ResourceManager* pResource
		,LPDIRECT3DDEVICE9* pDev
		,POINT	imagesize
		,O2Rect pRect = O2Rect(0,0,0,0)
		,D3DXVECTOR3 pPos = D3DXVECTOR3(0.0f,0.0f,0.0f)
		,UINT		_Center=GUI_CENTER_CENTER
		) { return S_OK;}



	virtual HRESULT Init(
		ResourceManager* pResource
		,LPDIRECT3DDEVICE9* pDev
		,O2InputManager* pInput
		);

	virtual UINT Update(POINT& pt,UINT _MsState) { return 0;}
	virtual void Render();
	virtual BOOL Update();


	HRESULT	AddTip(UINT _index,tstring filename,POINT imagesize
		,O2Rect pRect = O2Rect(0,0,0,0)
		,D3DXVECTOR3 pPos = D3DXVECTOR3(0.0f,0.0f,0.0f)
		,UINT		_Center=GUI_CENTER_CENTER);

	VOID	SetDelta(float* pDelta) { m_fDelta = pDelta; }			//델타타임 세팅
	VOID	SetTime(float	pTime)	{ m_pTiptList[m_iNumImage++]->SetDrawTime(pTime); }
	BOOL	GetIsOnce() { return m_bOnce; }
	VOID	SlideImage();
};