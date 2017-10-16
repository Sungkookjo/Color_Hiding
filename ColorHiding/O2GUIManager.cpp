#include "stdafx.h"
#include "O2GUIManager.h"
#include "CheckBox.h"
#include "ColorGage.h"
#include "TImeGage.h"
#include "TimeSprite.h"
#include "TwinkleSprite.h"

O2GUIManager::O2GUIManager()
:m_lpDevice(NULL)
,m_lpSprite(NULL)
,m_pResource(NULL)
,m_bPause(FALSE)
,m_lpVB(NULL)
,m_pHero(NULL)
,m_bSoundOnOff(TRUE)
,m_IHidingNum(0)
,m_pCurHiding(NULL)
,m_pSoundMgr(NULL)
,m_pTip(NULL)
,m_pWarnning(NULL)
{

}

O2GUIManager::~O2GUIManager()
{
	CleanUp();
	SAFE_RELEASE(m_lpSprite);
	SAFE_RELEASE(m_lpVB);
	m_pSoundMgr=NULL;
}

UINT O2GUIManager::UpDate()
{
	POINT pt={0,0};
	UINT msState=0;
	UINT index=0;
	UINT TempIndex = 0;
	list<O2GUIBase*>::iterator _it = m_lGuiList.begin();
	list<O2GUIBase*>::iterator _end = m_lGuiList.end();

	pt = m_pInput->GetMousePos();

	if(m_pInput->IsMouseClick(MOUSE_LBUTTON)){
		msState = MOUSE_CLICK;
	}else if(m_pInput->IsMouseDown(MOUSE_LBUTTON)){
		msState = MOUSE_DOWN;
	}else{
		msState = MOUSE_UP;
	}

	while(_it != _end){
		TempIndex = (*_it)->Update(pt,msState);
		if(TempIndex){
			if( TIME_OVER == TempIndex ){
				SAFE_DELETE(*_it);
				_it = m_lGuiList.erase(_it);
			}else if(UI_SOUND == TempIndex){
				m_bSoundOnOff = !m_bSoundOnOff;
				index = TempIndex;
				++_it;
			}else{
				index = TempIndex;
				++_it;
			}
		}else{
			++_it;
		}
	}

	if (m_bPause)
	{
		_it = m_lPauseList.begin();
		_end = m_lPauseList.end();
		while(_it != _end){
			TempIndex = (*_it)->Update(pt,msState);
			if(TempIndex){
				index = TempIndex;
			}
			++_it;
		}
	}
	if(m_pTip)
		m_pTip->Update();

	return index;
}

VOID O2GUIManager::Render()
{
	//(*m_lpDevice)->SetRenderState(D3DRS_ZENABLE,FALSE);
	m_lpSprite->Begin( D3DXSPRITE_ALPHABLEND );

	list<O2GUIBase*>::iterator _it = m_lGuiList.begin();
	list<O2GUIBase*>::iterator _end = m_lGuiList.end();
	while(_it != _end){
		(*_it)->Render();
		++_it;
	}

	if(m_bPause)
	{
		_it = m_lPauseList.begin();
		_end = m_lPauseList.end();
		while(_it != _end){
			(*_it)->Render();
			++_it;
		}
	
	}

	if(m_pTip)
		m_pTip->Render();

	m_lpSprite->End();
	//(*m_lpDevice)->SetRenderState(D3DRS_ZENABLE,TRUE);
}

VOID O2GUIManager::CleanUp()
{
	m_IHidingNum=0;
	m_pCurHiding=NULL;

	list<O2GUIBase*>::iterator _it = m_lGuiList.begin();
	list<O2GUIBase*>::iterator _end = m_lGuiList.end();
	while(_it != _end){
		SAFE_DELETE((*_it));
		++_it;
	}
	m_lGuiList.clear();

	_it = m_lPauseList.begin();
	_end = m_lPauseList.end();
	while(_it != _end){
		SAFE_DELETE((*_it));
		++_it;
	}
	m_lPauseList.clear();

	
	SAFE_DELETE(m_pTip);
	m_pHero = NULL;
	
}

HRESULT O2GUIManager::Init( O2InputManager* input,LPDIRECT3DDEVICE9* pDev,ResourceManager* pResource,O2SoundManager* pSound )
{
	HRESULT hr;
	m_lpDevice = pDev;
	m_pResource = pResource;
	m_pInput = input;
	m_pSoundMgr = pSound;

	hr = D3DXCreateSprite((*m_lpDevice), &m_lpSprite);
	if(hr){
		return E_FAIL;
	}

	if( FAILED( (*m_lpDevice)->CreateVertexBuffer( 4 * sizeof( GUIVERTEX ),
		0, D3DFVF_GUIVERTEX,
		D3DPOOL_DEFAULT, &m_lpVB, NULL ) ) )
	{
		return E_FAIL;
	}
	GUIVERTEX* pVertices;
	if( FAILED( m_lpVB->Lock( 0, 0, ( void** )&pVertices, 0 ) ) )
		return E_FAIL;


	pVertices[0].position = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
	pVertices[0].color = 0xffffffff;
	pVertices[0].tu = 0.0f;
	pVertices[0].tv = 0.0f;

	pVertices[1].position = D3DXVECTOR3( 1.0f , 0.0f , 0.0f );
	pVertices[1].color = 0xffffffff;
	pVertices[1].tu = 1.0f;
	pVertices[1].tv = 0.0f;

	pVertices[2].position = D3DXVECTOR3( 0.0f , 1.0f , 0.0f );
	pVertices[2].color = 0xffffffff;
	pVertices[2].tu = 0.0f;
	pVertices[2].tv = 1.0f;

	pVertices[3].position = D3DXVECTOR3( 1.0f , 1.0f , 0.0f );
	pVertices[3].color = 0xffffffff;
	pVertices[3].tu = 1.0f;
	pVertices[3].tv = 1.0f;

	m_lpVB->Unlock();

	return S_OK;
}

HRESULT O2GUIManager::CreateSprite( tstring _file,POINT	imgSize,D3DXVECTOR3 _pos,O2Rect _rect,BOOL _pause )
{
	HRESULT hr;
	O2GUISprite* _sprite = new O2GUISprite;
	hr = _sprite->Init(_file,m_pResource,m_lpDevice,imgSize,_rect,_pos);

	if(_pause)
		m_lPauseList.push_back(_sprite);
	else
		m_lGuiList.push_back(_sprite);

	return hr;
}

HRESULT O2GUIManager::CreateButton( UINT _index ,
								   tstring _file ,
								   POINT	imgSize,
								   D3DXVECTOR3 _pos ,
								   O2Rect _rect1 ,
								   O2Rect _rect2 ,
								   O2Rect _rect3 ,
								   BOOL _pause )
{
	HRESULT hr;
	O2GUIButton* _button = new O2GUIButton;
	hr = _button->Init(_file,m_pResource,m_lpDevice,imgSize,_rect1,_pos);
	if(FAILED(hr)){
		return hr;
	}
	_button->SetIndex(_index);

	hr = _button->SetButton(_file,BUTTONSTATE::NORMAL,_rect1);
	hr = _button->SetButton(_file,BUTTONSTATE::ON,_rect2);
	hr = _button->SetButton(_file,BUTTONSTATE::CLICK,_rect3);

	if(_pause)
		m_lPauseList.push_back(_button);
	else
		m_lGuiList.push_back(_button);

	if( UI_SOUND == _index ){
		if(!m_bSoundOnOff){
			_button->ChangeTexture();
		}
	}

	return hr;
}

VOID O2GUIManager::Flush()
{
	m_IHidingNum=0;
	m_pCurHiding=NULL;
	m_pWarnning = NULL;

	list<O2GUIBase*>::iterator _it = m_lGuiList.begin();
	list<O2GUIBase*>::iterator _end = m_lGuiList.end();
	while(_it != _end){
		SAFE_DELETE((*_it));
		++_it;
	}
	m_lGuiList.clear();

	_it = m_lPauseList.begin();
	_end = m_lPauseList.end();
	while(_it != _end){
		SAFE_DELETE((*_it));
		++_it;
	}
	m_lPauseList.clear();


	SAFE_DELETE(m_pTip);
	m_bPause = FALSE;

	
}

HRESULT O2GUIManager::CreateCheck( tstring _file, POINT imgSize, D3DXVECTOR3 _pos, O2Rect _rect , O2Rect _rect2,UINT index)
{
	CheckBox* _gui = new CheckBox;
	if(FAILED(_gui->Init(_file,m_pResource,m_lpDevice,imgSize,_rect,_pos)))
		return E_FAIL;
	_gui->InitCheck(_file,TRUE,_rect);
	_gui->InitCheck(_file,FALSE,_rect2);
	_gui->SetCheck(m_pHero->GetColorVisible(index));

	m_lGuiList.push_back(_gui);

	return S_OK;
}

HRESULT O2GUIManager::CreateColorGage( tstring _CoverFile, tstring _GreenFile, tstring _RedFile, tstring _BlueFile, tstring _PuppleFile, POINT _CoverSize, POINT _BarSize, O2Rect _CoverRect, O2Rect _BarRect, D3DXVECTOR3 _GagePos, D3DXVECTOR3 _BarPos )
{
	ColorGage* _gui = new ColorGage;
	if(FAILED(_gui->Init(_CoverFile,m_pResource,m_lpDevice,_CoverSize,_CoverRect,_GagePos)))
		return E_FAIL;

	_gui->SetGageTexture(COLOR_R,_RedFile);
	_gui->SetGageTexture(COLOR_G,_GreenFile);
	_gui->SetGageTexture(COLOR_B,_BlueFile);
	_gui->SetGageTexture(COLOR_P,_PuppleFile);

	if(FAILED(_gui->InitGageBar(_BarSize,_BarRect,_BarPos)))
		return E_FAIL;

	_gui->SetHero(m_pHero);

	m_lGuiList.push_back(_gui);

	return S_OK;
}

HRESULT O2GUIManager::CreateTimeGage( tstring _CoverFile, tstring _BarFile, POINT _CoverSize, POINT _BarSize, O2Rect _CoverRect, O2Rect _BarRect, D3DXVECTOR3 _GagePos, D3DXVECTOR3 _BarPos, float _Time )
{
	TimeGage* _gui = new TimeGage;
	if(FAILED(_gui->Init(_CoverFile,m_pResource,m_lpDevice,_CoverSize,_CoverRect,_GagePos)))
		return E_FAIL;

	if(FAILED(_gui->InitGageBar(_BarFile,_BarSize,_BarRect,_BarPos)))
		return E_FAIL;

	_gui->SetDelta(m_fpDelta);
	_gui->SetTime(_Time);

	m_lGuiList.push_back(_gui);

	return S_OK;
}

HRESULT O2GUIManager::CreateHidingEffect(UINT _index, tstring _file,POINT imgSize,D3DXVECTOR3 _pos,O2Rect _rect,float	_Time )
{
	if(_index>=m_IHidingNum)
	{	
		HidingEffect* _HidingEffect = new HidingEffect;
		if(_HidingEffect==NULL)
			return E_FAIL;
		_HidingEffect->Init(m_pResource,m_lpDevice,m_pHero);
		_HidingEffect->SetDelta(m_fpDelta);
		m_lGuiList.push_back(_HidingEffect);
		m_pCurHiding=_HidingEffect;
		m_IHidingNum++;
	}

	if(FAILED(m_pCurHiding->AddEffect(_file,imgSize,_rect,_pos)))
		return E_FAIL;

	m_pCurHiding->SetTime(_Time);

	return S_OK;
}


HRESULT O2GUIManager::CreateTimeSprite( tstring _file, POINT imgSize, D3DXVECTOR3 _pos, O2Rect _rect, float pTime, BOOL isPause,UINT _Center /*= FALSE */ )
{
	TimeSprite* _sprite = new TimeSprite;
	if(FAILED(_sprite->Init(_file,m_pResource,m_lpDevice,imgSize,_rect,_pos,_Center)))
		return E_FAIL;

	_sprite->SetTime(pTime);
	_sprite->SetDelta(m_fpDelta);

	m_lGuiList.push_back(_sprite);

	return S_OK;
}

HRESULT O2GUIManager::CreateTwinkleSprite( tstring _file, tstring _fileOn1, tstring _fileOn2,POINT imgSize, D3DXVECTOR3 _pos, O2Rect _rect, float pMaxTime, float pTwinkleTime, BOOL isPause /*= FALSE*/, UINT _Center /*= GUI_CENTER_CENTER */ )
{
	TwinkleSprite* _sprite = new TwinkleSprite;

	if(NULL == m_pWarnning)
		m_pWarnning = _sprite;

	if(FAILED(_sprite->Init(_file,m_pResource,m_lpDevice,imgSize,_rect,_pos,_Center))){
		return E_FAIL;
	}

	_sprite->SetTwinkle(pMaxTime,pTwinkleTime);
	_sprite->SetDelta(m_fpDelta);
	_sprite->SetOnTexture(_fileOn1,_fileOn2);

	m_lGuiList.push_back(_sprite);

	return S_OK;
}

HRESULT O2GUIManager::CreateTwinkleSprite( tstring _file, POINT imgSize, D3DXVECTOR3 _pos, O2Rect _rect, float pMaxTime, float pTwinkleTime, BOOL isPause /*= FALSE*/, UINT _Center /*= GUI_CENTER_CENTER */ )
{
	Twinkle2TexSprite* _sprite = new Twinkle2TexSprite;

	if(FAILED(_sprite->Init(_file,m_pResource,m_lpDevice,imgSize,_rect,_pos,_Center))){
		return E_FAIL;
	}

	_sprite->SetTwinkle(pMaxTime,pTwinkleTime);
	_sprite->SetDelta(m_fpDelta);

	m_lGuiList.push_back(_sprite);

	return S_OK;
}

HRESULT O2GUIManager::CreateTIP(UINT _index,tstring _file,POINT imgSize,D3DXVECTOR3 _pos,O2Rect _rect,float	_Time )
{
	if(m_pTip==NULL)
	{
		m_pTip = new O2TIP;
		if(m_pTip==NULL)
			return E_FAIL;
		m_pTip->Init(m_pResource,m_lpDevice,m_pInput);
		m_pTip->SetDelta(m_fpDelta);
	}

	if(FAILED(m_pTip->AddTip(_index,_file,imgSize,_rect,_pos)))
		return E_FAIL;

	if(_index!=0)
		m_pTip->SetTime(_Time);

	return S_OK;
}

VOID O2GUIManager::SetOnWarnning()
{
	if(m_pWarnning)
		m_pWarnning->SetOn();
}