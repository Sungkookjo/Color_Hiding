#include "stdafx.h"


O2GUIBase::O2GUIBase()
:m_vPos(0.0f,0.0f,0.0f)
,m_vRect(0,0,0,0)
,m_lpDevice(NULL)
,m_pResource(NULL)
{
	D3DXMatrixIdentity(&m_matWorld);
	m_matRot = m_matTrans = m_matScail = m_matWorld;
}

O2GUIBase::~O2GUIBase()
{

}

void O2GUIBase::SetMat()
{
	D3DXMatrixIdentity( &m_matWorld );
	m_matWorld = m_matScail * m_matRot * m_matTrans;
	(*m_lpDevice)->SetTransform( D3DTS_WORLD, &m_matWorld );
}

//////////////////////////////////////////////////////////////////////////
// O2GUIObject::O2GUIObject()
// 함수명 : O2GUIObject
// 설명 : O2GUIObject 맴버들을 초기화
// 작성자 : 조성국
// 작성일 : 08 / 07 / 07 
//////////////////////////////////////////////////////////////////////////
O2GUIObject::O2GUIObject()
:m_vAlpha(255)
,m_pTexture(NULL)
,m_lpVB(NULL)
{
}

O2GUIObject::O2GUIObject(tstring p_FileName)
:m_pTexture(NULL)
,m_vAlpha(255)
,m_lpVB(NULL)
{
	m_tFileName = p_FileName;
}

// 생성자 종료
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// O2GUIObject::~O2GUIObject()
// 함수명 : ~O2GUIObject
// 설명 : O2GUIObject 동적할당된 맴버들을 해제
// 작성자 : 조성국
// 작성일 : 08 / 07 / 07 
//////////////////////////////////////////////////////////////////////////
O2GUIObject::~O2GUIObject()
{
	SAFE_RELEASE(m_lpVB);
}

//////////////////////////////////////////////////////////////////////////
// void O2GUIObject::SetRect()
// 함수명 : SetRect
// 매개변수 : void
// 리턴값 : void
// 설명 : O2Rect를 텍스쳐 전체 범위로 설정
// 작성자 : 조성국
// 작성일 : 08 / 07 / 07 
//////////////////////////////////////////////////////////////////////////
void O2GUIObject::SetRect()
{
	if(m_pTexture)
	{
		// 텍스쳐의 이미지 사이즈를 불러와 렉트 설정
		D3DSURFACE_DESC ddsd;
		(m_pTexture)->GetLevelDesc(0,&ddsd);
		m_vRect.bottom = ddsd.Height;
		m_vRect.top = 0;
		m_vRect.left = 0;
		m_vRect.right = ddsd.Width;
		D3DXMatrixScaling(&m_matScail,m_vRect.GetWidth(),m_vRect.GetHeight(),1.0f);
	}
}

void O2GUIObject::SetRect( O2Rect p_Rect )
{
	m_vRect = p_Rect;
	D3DXMatrixScaling(&m_matScail,m_vRect.GetWidth(),m_vRect.GetHeight(),1.0f);
}
//////////////////////////////////////////////////////////////////////////
// HRESULT O2GUIObject::Init()
// 함수명 : Init
// 매개변수 : void
// 리턴값 : HRESULT
// 설명 : 텍스쳐 버퍼 할당
//			 실패시 E_FAIL, 성공시 S_OK 리턴
// 작성자 : 조성국
// 작성일 : 08 / 07 / 07 
//////////////////////////////////////////////////////////////////////////
HRESULT O2GUIObject::Init(tstring filename,ResourceManager* pResource,LPDIRECT3DDEVICE9* pDev,POINT imagesize,O2Rect pRect,D3DXVECTOR3 pPos,UINT _Center)
{
	m_tFileName = filename;
	m_pResource = pResource;
	m_lpDevice = pDev;
	m_pTexture = m_pResource->GetTexture(m_tFileName);
	m_vPos = pPos;
	if(pRect.bottom == 0){
		SetRect();
	}else{
		m_vRect.left = pRect.left;
		m_vRect.right = pRect.right;
		m_vRect.top = pRect.top;
		m_vRect.bottom = pRect.bottom;
	}

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

//////////////////////////////////////////////////////////////////////////
// bool O2GUIObject::IsInRect(POINT& pt)
// 함수명 : IsInRect
// 매개변수 : POINT& pt
// 리턴값 : bool
// 설명 : 마우스 포인터가 안에 들어있는지 체크.
// 작성자 : 조성국
// 작성일 : 08 / 07 / 07 
//////////////////////////////////////////////////////////////////////////
BOOL O2GUIObject::IsInRect(POINT& pt)
{
	return m_vRect.IsInRect(pt);
}

//////////////////////////////////////////////////////////////////////////
// void O2GUIObject::Render()
// 함수명 : Render
// 매개변수 : void
// 리턴값 : void
// 설명 : 그려준다.
// 작성자 : 조성국
// 작성일 : 08 / 07 / 07 
//////////////////////////////////////////////////////////////////////////
void O2GUIObject::Render()
{
	SetMat();
	(*m_lpDevice)->SetTexture( 0,(m_pTexture) );
	(*m_lpDevice)->SetStreamSource( 0, (m_lpVB), 0, sizeof( GUIVERTEX ) );
	(*m_lpDevice)->SetFVF( D3DFVF_GUIVERTEX );
	(*m_lpDevice)->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
}


void O2GUIObject::SetValidateObject()
{
	m_pTexture=m_pResource->GetTexture(m_tFileName);
}

UINT O2GUIObject::Update( POINT& pt,UINT _MsState )
{
	return 0;
}

HRESULT O2GUIObject::CreateVB( UINT _Center,POINT _ImgSize )
{
	m_uCenter = _Center;

	if( FAILED( (*m_lpDevice)->CreateVertexBuffer( 4 * sizeof( GUIVERTEX ),
		0, D3DFVF_GUIVERTEX,
		D3DPOOL_DEFAULT, &m_lpVB, NULL ) ) )
	{
		return E_FAIL;
	}
	GUIVERTEX* pVertices;
	if( FAILED( m_lpVB->Lock( 0, 0, ( void** )&pVertices, 0 ) ) )
		return E_FAIL;


	if( GUI_CENTER_LEFMID == m_uCenter){
		pVertices[0].position = D3DXVECTOR3( 0.0f , -0.5f , 0.0f );
		pVertices[0].color = 0xffffffff;
		pVertices[0].tu = m_vRect.left/(float)_ImgSize.x;
		pVertices[0].tv = m_vRect.top/(float)_ImgSize.y;

		pVertices[1].position = D3DXVECTOR3( 1.0f , -0.5f , 0.0f );
		pVertices[1].color = 0xffffffff;
		pVertices[1].tu = m_vRect.right/(float)_ImgSize.x;
		pVertices[1].tv = m_vRect.top/(float)_ImgSize.y;

		pVertices[2].position = D3DXVECTOR3( 0.0f , 0.5f , 0.0f );
		pVertices[2].color = 0xffffffff;
		pVertices[2].tu = m_vRect.left/(float)_ImgSize.x;
		pVertices[2].tv = m_vRect.bottom/(float)_ImgSize.y;

		pVertices[3].position = D3DXVECTOR3( 1.0f , 0.5f , 0.0f );
		pVertices[3].color = 0xffffffff;
		pVertices[3].tu = m_vRect.right/(float)_ImgSize.x;
		pVertices[3].tv = m_vRect.bottom/(float)_ImgSize.y;
	}else if( GUI_CENTER_RIGHTTOP == m_uCenter){
		pVertices[0].position = D3DXVECTOR3( -1.0f , 0.0f , 0.0f );
		pVertices[0].color = 0xffffffff;
		pVertices[0].tu = m_vRect.left/(float)_ImgSize.x;
		pVertices[0].tv = m_vRect.top/(float)_ImgSize.y;

		pVertices[1].position = D3DXVECTOR3( 0.0f , 0.0f , 0.0f );
		pVertices[1].color = 0xffffffff;
		pVertices[1].tu = m_vRect.right/(float)_ImgSize.x;
		pVertices[1].tv = m_vRect.top/(float)_ImgSize.y;

		pVertices[2].position = D3DXVECTOR3( -1.0f , 1.0f , 0.0f );
		pVertices[2].color = 0xffffffff;
		pVertices[2].tu = m_vRect.left/(float)_ImgSize.x;
		pVertices[2].tv = m_vRect.bottom/(float)_ImgSize.y;

		pVertices[3].position = D3DXVECTOR3( 0.0f , 1.0f , 0.0f );
		pVertices[3].color = 0xffffffff;
		pVertices[3].tu = m_vRect.right/(float)_ImgSize.x;
		pVertices[3].tv = m_vRect.bottom/(float)_ImgSize.y;
	}else if( GUI_CENTER_MIDTOP == m_uCenter){
		pVertices[0].position = D3DXVECTOR3( -0.5f , 0.0f , 0.0f );
		pVertices[0].color = 0xffffffff;
		pVertices[0].tu = m_vRect.left/(float)_ImgSize.x;
		pVertices[0].tv = m_vRect.top/(float)_ImgSize.y;

		pVertices[1].position = D3DXVECTOR3( 0.5f , 0.0f , 0.0f );
		pVertices[1].color = 0xffffffff;
		pVertices[1].tu = m_vRect.right/(float)_ImgSize.x;
		pVertices[1].tv = m_vRect.top/(float)_ImgSize.y;

		pVertices[2].position = D3DXVECTOR3( -0.5f , 1.0f , 0.0f );
		pVertices[2].color = 0xffffffff;
		pVertices[2].tu = m_vRect.left/(float)_ImgSize.x;
		pVertices[2].tv = m_vRect.bottom/(float)_ImgSize.y;

		pVertices[3].position = D3DXVECTOR3( 0.5f , 1.0f , 0.0f );
		pVertices[3].color = 0xffffffff;
		pVertices[3].tu = m_vRect.right/(float)_ImgSize.x;
		pVertices[3].tv = m_vRect.bottom/(float)_ImgSize.y;
	}else if( GUI_CENTER_MIDBOT == m_uCenter){
		pVertices[0].position = D3DXVECTOR3( -0.5f , -1.0f , 0.0f );
		pVertices[0].color = 0xffffffff;
		pVertices[0].tu = m_vRect.left/(float)_ImgSize.x;
		pVertices[0].tv = m_vRect.top/(float)_ImgSize.y;

		pVertices[1].position = D3DXVECTOR3( 0.5f , -1.0f , 0.0f );
		pVertices[1].color = 0xffffffff;
		pVertices[1].tu = m_vRect.right/(float)_ImgSize.x;
		pVertices[1].tv = m_vRect.top/(float)_ImgSize.y;

		pVertices[2].position = D3DXVECTOR3( -0.5f , 0.0f , 0.0f );
		pVertices[2].color = 0xffffffff;
		pVertices[2].tu = m_vRect.left/(float)_ImgSize.x;
		pVertices[2].tv = m_vRect.bottom/(float)_ImgSize.y;

		pVertices[3].position = D3DXVECTOR3( 0.5f , 0.0f , 0.0f );
		pVertices[3].color = 0xffffffff;
		pVertices[3].tu = m_vRect.right/(float)_ImgSize.x;
		pVertices[3].tv = m_vRect.bottom/(float)_ImgSize.y;
	}else{
		pVertices[0].position = D3DXVECTOR3( -0.5f , -0.5f , 0.0f );
		pVertices[0].color = 0xffffffff;
		pVertices[0].tu = m_vRect.left/(float)_ImgSize.x;
		pVertices[0].tv = m_vRect.top/(float)_ImgSize.y;

		pVertices[1].position = D3DXVECTOR3( 0.5f , -0.5f , 0.0f );
		pVertices[1].color = 0xffffffff;
		pVertices[1].tu = m_vRect.right/(float)_ImgSize.x;
		pVertices[1].tv = m_vRect.top/(float)_ImgSize.y;

		pVertices[2].position = D3DXVECTOR3( -0.5f , 0.5f , 0.0f );
		pVertices[2].color = 0xffffffff;
		pVertices[2].tu = m_vRect.left/(float)_ImgSize.x;
		pVertices[2].tv = m_vRect.bottom/(float)_ImgSize.y;

		pVertices[3].position = D3DXVECTOR3( 0.5f , 0.5f , 0.0f );
		pVertices[3].color = 0xffffffff;
		pVertices[3].tu = m_vRect.right/(float)_ImgSize.x;
		pVertices[3].tv = m_vRect.bottom/(float)_ImgSize.y;
	}
	m_lpVB->Unlock();

	return S_OK;
}