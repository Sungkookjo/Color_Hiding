#pragma once
#include "O2GUIOnce.h"
#include "O2InputManager.h"

class O2TIP: public O2GUIBase
{
private:

	float* m_fDelta;				//delta �ð�
	float  m_fDrawTime;				//�׸��� �ð�
	float  m_fCurTime;				//���� �ð�
	int	   m_iCurImage;				//���� �̹���
	int	   m_iNumImage;				//�̹��� ����
	BOOL   m_bSlide;				//�̹����� �׷�����
	BOOL   m_bOnce;					//�ѹ��� ���̱�
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

	VOID	SetDelta(float* pDelta) { m_fDelta = pDelta; }			//��ŸŸ�� ����
	VOID	SetTime(float	pTime)	{ m_pTiptList[m_iNumImage++]->SetDrawTime(pTime); }
	BOOL	GetIsOnce() { return m_bOnce; }
	VOID	SlideImage();
};