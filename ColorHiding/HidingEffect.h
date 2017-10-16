#pragma once

#include "O2GUIOnce.h"
#include "O2Hero.h"


class HidingEffect : public O2GUIBase
{


private:
	float* m_fDelta;				//delta 시간
	float  m_fDrawTime;				//그리는 시간
	float  m_fCurTime;				//현재 시간
	int	   m_iCurImage;				//현재 이미지
	int	   m_iNumImage;				//이미지 갯수
	BOOL   m_bSlide;				//이미지를 그려줄지
	BOOL   m_bHidingState;			//하이딩상태가 변화하였는지
	UINT   m_iColorState;			//칼라 상태
	std::vector<O2GUIOnce*>		m_pEffectList;
	ResourceManager*		m_pResource	;
	LPDIRECT3DDEVICE9*		m_lpDevice	;
	O2Hero*					m_pHero;

public:
	HidingEffect();
	~HidingEffect();

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
		,O2Hero* _hero
		);

	virtual UINT Update(POINT& pt,UINT _MsState);
	virtual void Render();
	
//	이펙트 추가하기
	HRESULT	AddEffect(tstring filename,POINT imagesize
		,O2Rect pRect = O2Rect(0,0,0,0)
		,D3DXVECTOR3 pPos = D3DXVECTOR3(0.0f,0.0f,0.0f)
		,UINT		_Center=GUI_CENTER_CENTER);

	void	SetHiding(BOOL _hiding) { m_bSlide = TRUE; }			//하이딩시에 true로 활성화
	VOID	SetDelta(float* pDelta) { m_fDelta = pDelta; }			//델타타임 세팅
	VOID	SetTime(float	pTime)	{ m_pEffectList[m_iNumImage++]->SetDrawTime(pTime); }		//그려지는 시간

	VOID	InitTime();						//	시간 초기화
	void	InitImage();					//	이미지 초기화
	void	SlideImage();					//	슬라이드이미지 초기화


};