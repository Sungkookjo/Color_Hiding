#include "stdafx.h"
#include "O22DObject.h"

//////////////////////////////////////////////////////////////////////////
//	2D Object
//////////////////////////////////////////////////////////////////////////
O22DObject::O22DObject()
:m_lpVB(NULL)
,m_lpTextures(NULL)
{
	m_uType |= OBJ_TYPE_2D;
}

O22DObject::~O22DObject()
{
	SAFE_RELEASE(m_lpVB);
}

VOID O22DObject::CleanUp()
{

}

HRESULT O22DObject::Init( LPDIRECT3DDEVICE9* pDev, TCHAR* filename, O2Rect pRect, POINT imgsize, D3DXVECTOR3 pPos, D3DXVECTOR3 pSize/*=D3DXVECTOR3(1.0f,1.0f,1.0f)*/, D3DXVECTOR3 pRot/*=D3DXVECTOR3(0.0f,0.0f,0.0f) */ )
{
	m_lpDevice = pDev;
	SetFileName(filename);
	m_lpTextures = s_pResource->GetTexture(m_tsFile);
	m_rImg = pRect;

	SetPos(pPos);
	SetSize(pSize);
	SetRot(pRot);
	SetMatRot();

	if(FAILED(CreateVB(GUI_CENTER_CENTER,imgsize)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT O22DObject::UpDate()
{
	return S_OK;
}

HRESULT O22DObject::Render()
{
	(*m_lpDevice)->SetRenderState( D3DRS_LIGHTING , FALSE );
	SetMat();
	(*m_lpDevice)->SetTexture( 0,(m_lpTextures) );
	(*m_lpDevice)->SetStreamSource( 0, (m_lpVB), 0, sizeof( OBJ2DVERTEX ) );
	(*m_lpDevice)->SetFVF( D3DFVF_2DOBJVERTEX );
	(*m_lpDevice)->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
	(*m_lpDevice)->SetRenderState( D3DRS_LIGHTING , TRUE );

	return S_OK;
}

VOID O22DObject::SetMat()
{
	D3DXMatrixTranslation(&m_matTrans,m_vPos.x,m_vPos.y,m_vPos.z);

	m_matWorld = m_matSize * m_matRot * m_matTrans;

	// 부모행렬이 있는 경우 곱해준다.
	if(m_pParent){
		m_matWorld *= m_pParent->GetMatWorld();
	}

	// 월드행렬 설정
	(*m_lpDevice)->SetTransform( D3DTS_WORLD, &m_matWorld );
}

HRESULT O22DObject::CreateVB( UINT _Center,POINT _ImgSize )
{
	m_uCenter = _Center;

	if( FAILED( (*m_lpDevice)->CreateVertexBuffer( 4 * sizeof( OBJ2DVERTEX ),
		0, D3DFVF_2DOBJVERTEX,
		D3DPOOL_DEFAULT, &m_lpVB, NULL ) ) )
	{
		return E_FAIL;
	}
	OBJ2DVERTEX* pVertices;
	if( FAILED( m_lpVB->Lock( 0, 0, ( void** )&pVertices, 0 ) ) )
		return E_FAIL;


	if( GUI_CENTER_LEFMID == m_uCenter){
		pVertices[0].position = D3DXVECTOR3( 0.0f , -0.5f , 0.0f );
		pVertices[0].color = 0xffffffff;
		pVertices[0].tu = m_rImg.left/(float)_ImgSize.x;
		pVertices[0].tv = m_rImg.top/(float)_ImgSize.y;

		pVertices[1].position = D3DXVECTOR3( 1.0f , -0.5f , 0.0f );
		pVertices[1].color = 0xffffffff;
		pVertices[1].tu = m_rImg.right/(float)_ImgSize.x;
		pVertices[1].tv = m_rImg.top/(float)_ImgSize.y;

		pVertices[2].position = D3DXVECTOR3( 0.0f , 0.5f , 0.0f );
		pVertices[2].color = 0xffffffff;
		pVertices[2].tu = m_rImg.left/(float)_ImgSize.x;
		pVertices[2].tv = m_rImg.bottom/(float)_ImgSize.y;

		pVertices[3].position = D3DXVECTOR3( 1.0f , 0.5f , 0.0f );
		pVertices[3].color = 0xffffffff;
		pVertices[3].tu = m_rImg.right/(float)_ImgSize.x;
		pVertices[3].tv = m_rImg.bottom/(float)_ImgSize.y;
	}else if(GUI_CENTER_MIDBOT == m_uCenter){
		pVertices[0].position = D3DXVECTOR3( -0.5f , 0.0f , 0.0f );
		pVertices[0].color = 0xffffffff;
		pVertices[0].tu = m_rImg.left/(float)_ImgSize.x;
		pVertices[0].tv = m_rImg.top/(float)_ImgSize.y;

		pVertices[1].position = D3DXVECTOR3( 0.5f , 0.0f , 0.0f );
		pVertices[1].color = 0xffffffff;
		pVertices[1].tu = m_rImg.right/(float)_ImgSize.x;
		pVertices[1].tv = m_rImg.top/(float)_ImgSize.y;

		pVertices[2].position = D3DXVECTOR3( -0.5f , 1.0f , 0.0f );
		pVertices[2].color = 0xffffffff;
		pVertices[2].tu = m_rImg.left/(float)_ImgSize.x;
		pVertices[2].tv = m_rImg.bottom/(float)_ImgSize.y;

		pVertices[3].position = D3DXVECTOR3( 0.5f , 1.0f , 0.0f );
		pVertices[3].color = 0xffffffff;
		pVertices[3].tu = m_rImg.right/(float)_ImgSize.x;
		pVertices[3].tv = m_rImg.bottom/(float)_ImgSize.y;
	}else{
		pVertices[0].position = D3DXVECTOR3( -0.5f , -0.5f , 0.0f );
		pVertices[0].color = 0xffffffff;
		pVertices[0].tu = m_rImg.left/(float)_ImgSize.x;
		pVertices[0].tv = m_rImg.top/(float)_ImgSize.y;

		pVertices[1].position = D3DXVECTOR3( 0.5f , -0.5f , 0.0f );
		pVertices[1].color = 0xffffffff;
		pVertices[1].tu = m_rImg.right/(float)_ImgSize.x;
		pVertices[1].tv = m_rImg.top/(float)_ImgSize.y;

		pVertices[2].position = D3DXVECTOR3( -0.5f , 0.5f , 0.0f );
		pVertices[2].color = 0xffffffff;
		pVertices[2].tu = m_rImg.left/(float)_ImgSize.x;
		pVertices[2].tv = m_rImg.bottom/(float)_ImgSize.y;

		pVertices[3].position = D3DXVECTOR3( 0.5f , 0.5f , 0.0f );
		pVertices[3].color = 0xffffffff;
		pVertices[3].tu = m_rImg.right/(float)_ImgSize.x;
		pVertices[3].tv = m_rImg.bottom/(float)_ImgSize.y;
	}
	m_lpVB->Unlock();

	return S_OK;
}