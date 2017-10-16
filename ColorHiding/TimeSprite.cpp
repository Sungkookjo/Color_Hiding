#include "stdafx.h"
#include "TimeSprite.h"

TimeSprite::TimeSprite()
:m_fpDelta(NULL)
,m_fMaxTime(0)
,m_fCurTime(0)
{

}

TimeSprite::~TimeSprite()
{

}

UINT TimeSprite::Update( POINT& pt,UINT _MsState )
{
	O2Rect TempRect = m_vBaseRect;
	m_fCurTime -= (*m_fpDelta);

	if(m_fCurTime <= 0.01f){
		return TIME_OVER;
	}else if(m_fCurTime >= 0.5f){
		TempRect.bottom = TempRect.bottom * 1.5f;
		TempRect.right = TempRect.right * 1.5f;
		TempRect.bottom = (m_vBaseRect.bottom*((1.0f-(m_fCurTime/m_fMaxTime))*2.0f));
		TempRect.right = (m_vBaseRect.right*((1.0f-(m_fCurTime/m_fMaxTime))*2.0f));
		SetRect(TempRect);
	}

	return 0;
}

HRESULT TimeSprite::Init( tstring filename ,ResourceManager* pResource ,LPDIRECT3DDEVICE9* pDev ,POINT imagesize ,O2Rect pRect /*= O2Rect(0,0,0,0) */,D3DXVECTOR3 pPos /*= D3DXVECTOR3(0.0f,0.0f,0.0f) */,UINT _Center/*=GUI_CENTER_CENTER */ )
{
	m_tFileName = filename;
	m_pResource = pResource;
	m_lpDevice = pDev;
	m_pTexture = m_pResource->GetTexture(m_tFileName);
	m_vPos = pPos;
	m_vBaseRect = m_vRect = pRect;

	if(FAILED(CreateVB(_Center,imagesize)))
		return E_FAIL;

	float	width = m_vRect.GetWidth();
	float	height = m_vRect.GetHeight();
	
	m_vRect.left = 0  - (width*0.5f);
	m_vRect.right = width - (width*0.5f);
	m_vRect.top = 0 - (height*0.5f);
	m_vRect.bottom = height - (height*0.5f);

	D3DXMatrixTranslation(&m_matTrans,m_vPos.x,m_vPos.y,m_vPos.z);
	D3DXMatrixScaling(&m_matScail,m_vRect.GetWidth(),m_vRect.GetHeight(),1.0f);

	return S_OK;
}