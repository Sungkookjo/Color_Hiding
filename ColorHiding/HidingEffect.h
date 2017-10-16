#pragma once

#include "O2GUIOnce.h"
#include "O2Hero.h"


class HidingEffect : public O2GUIBase
{


private:
	float* m_fDelta;				//delta �ð�
	float  m_fDrawTime;				//�׸��� �ð�
	float  m_fCurTime;				//���� �ð�
	int	   m_iCurImage;				//���� �̹���
	int	   m_iNumImage;				//�̹��� ����
	BOOL   m_bSlide;				//�̹����� �׷�����
	BOOL   m_bHidingState;			//���̵����°� ��ȭ�Ͽ�����
	UINT   m_iColorState;			//Į�� ����
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
	
//	����Ʈ �߰��ϱ�
	HRESULT	AddEffect(tstring filename,POINT imagesize
		,O2Rect pRect = O2Rect(0,0,0,0)
		,D3DXVECTOR3 pPos = D3DXVECTOR3(0.0f,0.0f,0.0f)
		,UINT		_Center=GUI_CENTER_CENTER);

	void	SetHiding(BOOL _hiding) { m_bSlide = TRUE; }			//���̵��ÿ� true�� Ȱ��ȭ
	VOID	SetDelta(float* pDelta) { m_fDelta = pDelta; }			//��ŸŸ�� ����
	VOID	SetTime(float	pTime)	{ m_pEffectList[m_iNumImage++]->SetDrawTime(pTime); }		//�׷����� �ð�

	VOID	InitTime();						//	�ð� �ʱ�ȭ
	void	InitImage();					//	�̹��� �ʱ�ȭ
	void	SlideImage();					//	�����̵��̹��� �ʱ�ȭ


};