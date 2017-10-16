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
// �Լ��� : O2GUIObject
// ���� : O2GUIObject �ɹ����� �ʱ�ȭ
// �ۼ��� : ������
// �ۼ��� : 08 / 07 / 07 
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

// ������ ����
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// O2GUIObject::~O2GUIObject()
// �Լ��� : ~O2GUIObject
// ���� : O2GUIObject �����Ҵ�� �ɹ����� ����
// �ۼ��� : ������
// �ۼ��� : 08 / 07 / 07 
//////////////////////////////////////////////////////////////////////////
O2GUIObject::~O2GUIObject()
{
	SAFE_RELEASE(m_lpVB);
}

//////////////////////////////////////////////////////////////////////////
// void O2GUIObject::SetRect()
// �Լ��� : SetRect
// �Ű����� : void
// ���ϰ� : void
// ���� : O2Rect�� �ؽ��� ��ü ������ ����
// �ۼ��� : ������
// �ۼ��� : 08 / 07 / 07 
//////////////////////////////////////////////////////////////////////////
void O2GUIObject::SetRect()
{
	if(m_pTexture)
	{
		// �ؽ����� �̹��� ����� �ҷ��� ��Ʈ ����
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
// �Լ��� : Init
// �Ű����� : void
// ���ϰ� : HRESULT
// ���� : �ؽ��� ���� �Ҵ�
//			 ���н� E_FAIL, ������ S_OK ����
// �ۼ��� : ������
// �ۼ��� : 08 / 07 / 07 
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
// �Լ��� : IsInRect
// �Ű����� : POINT& pt
// ���ϰ� : bool
// ���� : ���콺 �����Ͱ� �ȿ� ����ִ��� üũ.
// �ۼ��� : ������
// �ۼ��� : 08 / 07 / 07 
//////////////////////////////////////////////////////////////////////////
BOOL O2GUIObject::IsInRect(POINT& pt)
{
	return m_vRect.IsInRect(pt);
}

//////////////////////////////////////////////////////////////////////////
// void O2GUIObject::Render()
// �Լ��� : Render
// �Ű����� : void
// ���ϰ� : void
// ���� : �׷��ش�.
// �ۼ��� : ������
// �ۼ��� : 08 / 07 / 07 
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