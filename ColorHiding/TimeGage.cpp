#include "stdafx.h"
#include "TimeGage.h"

TimeGage::TimeGage()
:m_fpDelta(NULL)
,m_pGage(NULL)
{

}

TimeGage::~TimeGage()
{
	SAFE_DELETE_ARRAY(m_pGage);
}

void TimeGage::Render()
{
	m_pGage[GAGE_COVER].Render();
	m_pGage[GAGE_BAR].Render();
}

UINT TimeGage::Update( POINT& pt,UINT _MsState )
{
	m_fCurTime -= (*m_fpDelta);
	if(m_fCurTime <= 0.01f){
		m_fCurTime = 0.0f;
		return UI_OVER;
	}

	m_pGage[GAGE_BAR].SetRotZ((m_fCurTime/m_fMaxTime * 90.0f)-90);

	return 0;
}

HRESULT TimeGage::Init( tstring filename ,ResourceManager* pResource ,LPDIRECT3DDEVICE9* pDev ,POINT imagesize ,O2Rect pRect /*= O2Rect(0,0,0,0) */,D3DXVECTOR3 pPos /*= D3DXVECTOR3(0.0f,0.0f,0.0f) */,UINT _Center/*=GUI_CENTER_CENTER */ )
{
	m_lpDevice = pDev;
	m_pResource = pResource;
	m_vPos = pPos;
	m_vRect = pRect;
	m_tFileName = filename;

	m_pGage = new O2GUIObject[2];

	if(FAILED(m_pGage[GAGE_COVER].Init(filename,m_pResource,m_lpDevice,imagesize,pRect,pPos,GUI_CENTER_RIGHTTOP))){
		return E_FAIL;
	}

	return S_OK;
}

HRESULT TimeGage::InitGageBar( tstring _filename, POINT imagesize ,O2Rect pRect ,D3DXVECTOR3 pPos )
{
	return m_pGage[GAGE_BAR].Init(_filename,m_pResource,m_lpDevice,imagesize,pRect,pPos,GUI_CENTER_RIGHTTOP);
}

VOID TimeGage::InitTimer()
{
	m_fCurTime = m_fMaxTime;
}