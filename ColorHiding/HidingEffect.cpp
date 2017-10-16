#include "stdafx.h"
#include "HidingEffect.h"

HidingEffect::HidingEffect()
:m_fDelta(NULL)
,m_fDrawTime(0)
,m_fCurTime(0)
,m_iCurImage(0)
,m_bSlide(FALSE)
,m_iNumImage(0)
,m_pResource(NULL)
,m_lpDevice(NULL)
,m_pHero(NULL)
,m_bHidingState(FALSE)
,m_iColorState(COLOR_NONE)
{

}

HidingEffect::~HidingEffect()
{
	std::vector<O2GUIOnce*>::iterator it=m_pEffectList.begin();
	int _end=m_pEffectList.size();
	for(int i=0;i<_end;i++,it++)
	{
		SAFE_DELETE(*it);
	}
	m_pEffectList.clear();
}

HRESULT HidingEffect::Init( ResourceManager *pResource, LPDIRECT3DDEVICE9 *pDev,O2Hero* _hero)
{
	m_pResource = pResource;
	if(m_pResource==NULL)
		return E_FAIL;
	m_lpDevice = pDev;
	if(m_lpDevice==NULL)
		return E_FAIL;
	m_pHero = _hero;
	if(m_pHero==NULL)
		return E_FAIL;

	return S_OK;
}

HRESULT HidingEffect::AddEffect(tstring filename, POINT imagesize, O2Rect pRect/* = O2Rect(0,0,0,0)*/, D3DXVECTOR3 pPos /*= D3DXVECTOR3(0.000000,0.000000,0.000000)*/, UINT _Center/* = 0*/)
{
	O2GUIOnce* temp = new O2GUIOnce;
	if(FAILED(temp->Init(filename,m_pResource,m_lpDevice,imagesize,pRect,pPos,_Center)))
		return E_FAIL;
	m_pEffectList.push_back(temp);
	return S_OK;
}


UINT HidingEffect::Update(POINT& pt,UINT _MsState)
{
	m_fCurTime+=(*m_fDelta);
	//юс╫ц
	if((*m_fDelta)<=0.0000000001f)
		m_bSlide=FALSE;

	int _size=m_pEffectList.size();
	if((m_fCurTime>=m_pEffectList[m_iCurImage]->GetDrawTime())&&m_bSlide)
	{
		m_iCurImage++;
		m_fCurTime=0;
	}

	if(m_iCurImage>=_size || m_bSlide==FALSE)
	{
		m_iCurImage=0;
		m_fCurTime=0;
		m_bSlide=FALSE;
	}
	
	if(m_pHero)
	{
		UINT _ColorState=m_pHero->GetColor();
		if(m_iColorState!=_ColorState)
		{
			m_iColorState=_ColorState;
			if(m_iColorState!=COLOR_NONE&&m_bHidingState==FALSE)
			{
				m_bHidingState=TRUE;
				SlideImage();
			}
		}else if(m_pHero->GetHide()==FALSE)
		{
			m_bHidingState=FALSE;
		}
	}

	return 0;
}

void HidingEffect::Render()
{
	if(m_bSlide)
		m_pEffectList[m_iCurImage]->Render();

}

void HidingEffect::InitTime()
{
	m_fCurTime=0;
}

void HidingEffect::SlideImage()
{
	InitTime();
	InitImage();
	SetHiding(TRUE);
}

void HidingEffect::InitImage()
{
	m_iCurImage=0;
}