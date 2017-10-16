#include "stdafx.h"
#include "TIP.h"

O2TIP::O2TIP()
:m_fDelta(NULL)
,m_fDrawTime(0)
,m_fCurTime(0)
,m_iCurImage(0)
,m_iNumImage(0)
,m_pResource(NULL)
,m_lpDevice(NULL)
,m_pInputMgr(NULL)
,m_bSlide(TRUE)
,m_bOnce(FALSE)
,m_pBack(NULL)
{

}



O2TIP::~O2TIP()
{
	std::vector<O2GUIOnce*>::iterator it=m_pTiptList.begin();
	int _end=m_pTiptList.size();
	for(int i=0;i<_end;i++,it++)
	{
		SAFE_DELETE(*it);
	}
	m_pTiptList.clear();
	SAFE_DELETE(m_pBack);
}

HRESULT O2TIP::Init( ResourceManager *pResource, LPDIRECT3DDEVICE9 *pDev,O2InputManager* pInput)
{
	m_pResource = pResource;
	if(m_pResource==NULL)
		return E_FAIL;
	m_lpDevice = pDev;
	if(m_lpDevice==NULL)
		return E_FAIL;
	m_pInputMgr = pInput;
	if(m_pInputMgr==NULL)
		return E_FAIL;

	return S_OK;
}

HRESULT O2TIP::AddTip(UINT _index,tstring filename, POINT imagesize, O2Rect pRect/* = O2Rect(0,0,0,0)*/, D3DXVECTOR3 pPos /*= D3DXVECTOR3(0.000000,0.000000,0.000000)*/, UINT _Center/* = 0*/)
{
	O2GUIOnce* temp = new O2GUIOnce;
	if(FAILED(temp->Init(filename,m_pResource,m_lpDevice,imagesize,pRect,pPos,_Center)))
		return E_FAIL;
	if(_index==0)
		m_pBack=temp;
	else
		m_pTiptList.push_back(temp);
	return S_OK;
}


BOOL O2TIP::Update()
{
//	ÀÓ½Ã Å¸ÀÓ
//	PauseÇÒ¶§ µ¨Å¸°¡ 0µÊ
	m_fCurTime+=(0.016f);

	if(m_bOnce==TRUE)
		m_bOnce=FALSE;
	
	int _size=m_pTiptList.size();
	if(((m_fCurTime>=m_pTiptList[m_iCurImage]->GetDrawTime())&&m_bSlide)||m_pInputMgr->IsKeyPress(DIK_SPACE)||m_pInputMgr->IsKeyPress(DIK_RETURN)||m_pInputMgr->IsMouseClick(MOUSE_LBUTTON))
	{
		m_iCurImage++;
		m_fCurTime=0;
	}

	if(m_iCurImage>=_size)
	{
		m_iCurImage=0;
		m_fCurTime=0;
		m_bSlide=FALSE;
		m_bOnce=TRUE;
	}

	return FALSE;
}

void O2TIP::Render()
{
	if(m_bSlide)
	{
		if(m_pBack)
			m_pBack->Render();
		m_pTiptList[m_iCurImage]->Render();
	}
}

void O2TIP::SlideImage()
{
	
}