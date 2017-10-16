#include "stdafx.h"
#include "ColorGage.h"
#include "O2Hero.h"


ColorGage::ColorGage()
:m_pHero(NULL)
,m_pGage(NULL)
,m_CurColor(COLOR_NONE)
{
}

ColorGage::~ColorGage()
{
	SAFE_DELETE_ARRAY(m_pGage);
}

void ColorGage::Render()
{
	m_pGage[GAGE_COVER].Render();
	if(COLOR_NONE != m_CurColor)
		m_pGage[GAGE_BAR].Render();
}

UINT ColorGage::Update( POINT& pt,UINT _MsState )
{
	O2Rect	TempRect = m_vBarRect;
	D3DXVECTOR3	TempPos = m_vBarPos;
	float	TempGage;
	// 칼라 얻어옴
	m_CurColor = ((O2Hero*)m_pHero)->GetColor();

	// 변신중이 아니라면
	if( COLOR_NONE == m_CurColor )
	{
	}else{
		TempGage = ((O2Hero*)m_pHero)->GetColorGage(m_CurColor)/((O2Hero*)m_pHero)->GetColorMaxGage(m_CurColor);
		TempRect.right  = TempRect.left + (TempRect.GetWidth() * TempGage);
		// 게이지바의 사이즈와 텍스처 변경
		m_pGage[GAGE_BAR].SetRect(TempRect);
		m_pGage[GAGE_BAR].SetPos(TempPos);
		m_pGage[GAGE_BAR].SetTexture(m_pTextures[m_CurColor]);
	}
	return 0;
}

HRESULT ColorGage::Init( tstring filename ,ResourceManager* pResource ,LPDIRECT3DDEVICE9* pDev ,POINT imagesize ,O2Rect pRect ,D3DXVECTOR3 pPos,UINT		_Center)
{
	float width,height;
	m_lpDevice = pDev;
	m_pResource = pResource;
	m_vPos = pPos;
	m_vRect = pRect;
	m_tFileName = filename;

	width = m_vRect.GetWidth();
	height= m_vRect.GetHeight();

	m_vRect.left = 0.0f  - (width*0.5f) + m_vPos.x;
	m_vRect.right = width - (width*0.5f) + m_vPos.x;
	m_vRect.top = 0.0f - (height*0.5f) + m_vPos.y;
	m_vRect.bottom = height - (height*0.5f) + m_vPos.y;

	m_pGage = new O2GUIObject[2];


	if(FAILED(m_pGage[GAGE_COVER].Init(filename,m_pResource,m_lpDevice,imagesize,pRect,pPos))){
		return E_FAIL;
	}

	return S_OK;
}

HRESULT ColorGage::InitGageBar(POINT imagesize ,O2Rect pRect,D3DXVECTOR3 pPos )
{
	HRESULT hr = m_pGage[GAGE_BAR].Init(m_tFileName,m_pResource,m_lpDevice,imagesize,pRect,pPos,GUI_CENTER_LEFMID);

	if(SUCCEEDED(hr)){
		m_pGage[GAGE_BAR].SetTexture(m_pTextures[COLOR_G]);
		m_vBarRect = m_pGage[GAGE_BAR].GetRect();
		m_vBarPos	=	pPos;
	}

	return hr;
}

VOID ColorGage::SetGageTexture( UINT _Color,tstring pTex )
{
	m_pTextures[_Color] = m_pResource->GetTexture(pTex);
}