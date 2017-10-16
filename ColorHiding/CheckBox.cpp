#include "stdafx.h"
#include "CheckBox.h"


CheckBox::CheckBox()
:m_pObjects(NULL)
,m_bIsCheck(NULL)
{

}

CheckBox::~CheckBox()
{
	SAFE_DELETE_ARRAY(m_pObjects);
}


HRESULT CheckBox::Init( tstring filename ,ResourceManager* pResource ,LPDIRECT3DDEVICE9* pDev ,POINT imagesize ,O2Rect pRect ,D3DXVECTOR3 pPos,UINT		_Center )
{
	float width,height;
	m_lpDevice = pDev;
	m_pResource = pResource;
	m_vPos = pPos;
	m_pImgSize = imagesize;
	m_vRect = pRect;

	width = m_vRect.GetWidth();
	height= m_vRect.GetHeight();

	m_vRect.left = 0.0f  - (width*0.5f) + m_vPos.x;
	m_vRect.right = width - (width*0.5f) + m_vPos.x;
	m_vRect.top = 0.0f - (height*0.5f) + m_vPos.y;
	m_vRect.bottom = height - (height*0.5f) + m_vPos.y;

	m_tFileName = filename;

	m_pObjects = new O2GUIObject[2];

	return S_OK;
}

void CheckBox::Render()
{
	if( m_pObjects && m_bIsCheck )
		m_pObjects[(*m_bIsCheck)].Render();
}

UINT CheckBox::Update( POINT& pt,UINT _MsState )
{
	return 0;
}

HRESULT CheckBox::InitCheck( tstring p_fileName ,BOOL p_State ,O2Rect p_Rect )
{
	return m_pObjects[p_State].Init(p_fileName,m_pResource,m_lpDevice,m_pImgSize,p_Rect,m_vPos);
}