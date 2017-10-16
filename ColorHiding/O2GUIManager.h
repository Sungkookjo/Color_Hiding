#pragma once
#include "O2GUISprite.h"
#include "O2GUIButton.h"
#include "O2InputManager.h"
#include "O2Hero.h"
#include "HidingEffect.h"
#include "O2SoundManager.h"
#include "TIP.h"


enum {
	GUI_SPRITE = 0 ,
	GUI_BUTTON,
	GUI_CHECKBOX
};
class TwinkleSprite;

class O2GUIManager{
private:
	list<O2GUIBase*>	m_lGuiList;
	list<O2GUIBase*>	m_lPauseList;
	LPDIRECT3DDEVICE9*	m_lpDevice;
	ResourceManager*	m_pResource;
	LPD3DXSPRITE		m_lpSprite;
	O2InputManager*		m_pInput;
	BOOL				m_bPause;
	O2Hero*				m_pHero;
	float*				m_fpDelta;
	UINT				m_IHidingNum;
	HidingEffect*		m_pCurHiding;
	O2SoundManager*		m_pSoundMgr;
	O2TIP*				m_pTip;
	TwinkleSprite*			m_pWarnning;

	LPDIRECT3DVERTEXBUFFER9	m_lpVB;

	VOID CleanUp();
	BOOL				m_bSoundOnOff;

public:
	O2GUIManager();
	~O2GUIManager();

	VOID Flush();
	UINT UpDate();
	VOID Render();
	HRESULT CreateButton(UINT _index
		,tstring _file
		,POINT imgSize
		,D3DXVECTOR3 _pos
		,O2Rect _rect1
		,O2Rect _rect2
		,O2Rect _rect3 = O2Rect(0,0,0,0)
		,BOOL isPause = FALSE
		);
	HRESULT CreateSprite(tstring _file,POINT imgSize,D3DXVECTOR3 _pos,O2Rect _rect,BOOL isPause = FALSE);
	HRESULT	CreateTimeSprite(tstring _file,
		POINT imgSize,
		D3DXVECTOR3 _pos,
		O2Rect _rect,
		float	pTime,
		BOOL isPause = FALSE,
		UINT _Center = GUI_CENTER_CENTER
		);

	HRESULT	CreateTwinkleSprite(tstring _file,
		tstring _fileOn1,
		tstring _fileOn2,
		POINT imgSize,
		D3DXVECTOR3 _pos,
		O2Rect _rect,
		float	pMaxTime,
		float	pTwinkleTime,
		BOOL isPause = FALSE,
		UINT _Center = GUI_CENTER_CENTER
		);

	HRESULT	CreateTwinkleSprite(tstring _file,
		POINT imgSize,
		D3DXVECTOR3 _pos,
		O2Rect _rect,
		float	pMaxTime,
		float	pTwinkleTime,
		BOOL isPause = FALSE,
		UINT _Center = GUI_CENTER_CENTER
		);


	HRESULT CreateCheck(tstring _file,
		POINT imgSize,
		D3DXVECTOR3 _pos,
		O2Rect _rect,
		O2Rect _rect2,
		UINT index
		);
	HRESULT	CreateColorGage(tstring _CoverFile,
		tstring _GreenFile,
		tstring _RedFile,
		tstring _BlueFile,
		tstring _PuppleFile,
		POINT	_CoverSize,
		POINT	_BarSize,
		O2Rect	_CoverRect,
		O2Rect	_BarRect,
		D3DXVECTOR3 _GagePos,
		D3DXVECTOR3 _BarPos);

	HRESULT	CreateTimeGage(tstring _CoverFile,
		tstring _BarFile,
		POINT	_CoverSize,
		POINT	_BarSize,
		O2Rect	_CoverRect,
		O2Rect	_BarRect,
		D3DXVECTOR3 _GagePos,
		D3DXVECTOR3 _BarPos,
		float		_Time
		);

	HRESULT	CreateHidingEffect(UINT _index,tstring _file,
		POINT imgSize,
		D3DXVECTOR3 _pos,
		O2Rect _rect,
		float	_Time);

	HRESULT	CreateTIP(UINT _index,tstring _file,
		POINT imgSize,
		D3DXVECTOR3 _pos,
		O2Rect _rect,
		float	_Time);

	VOID	SetOnWarnning();
	HRESULT Init(O2InputManager* input,LPDIRECT3DDEVICE9* pDev,ResourceManager* pResource,O2SoundManager* pSound);
	VOID	SetPause(BOOL _pause) { m_bPause = _pause; }
	VOID	SetPause()	{ m_bPause = (!m_bPause); }

	VOID	SetHero(O2Hero* pHero) { m_pHero = pHero; }
	VOID	SetDelta(float* pDelta) { m_fpDelta = pDelta;}
	BOOL	GetTipOver() {if(m_pTip) { return m_pTip->GetIsOnce();}
									else{return FALSE;}}

};