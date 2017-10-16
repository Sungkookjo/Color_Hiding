#include "stdafx.h"
#include "Loading.h"

Loading* Loading::s_pSingle = NULL;

Loading::Loading(Basefw_FS* FS)
:m_lpDevice(NULL)
,m_pIsFinish(NULL)
,m_lpVB(NULL)
,m_iCurTex(0)
,m_pFS(FS)
{
	s_pSingle = NULL;
}

Loading::~Loading(){
	for(int i = 0; i<TEX_MAX; ++i){
		SAFE_RELEASE(m_lpTexture[i]);
	}

	SAFE_RELEASE(m_lpVB);
	SAFE_RELEASE(m_lpDevice);
	s_pSingle = NULL;
}

void Loading::Run( BOOL* pFlag )
{
	QueryPerformanceCounter(&m_liCur);
	QueryPerformanceFrequency(&m_liFreq);
	m_liPre = m_liCur;
	m_fTime = 0.0f;
	m_pIsFinish = pFlag;
	m_iCurTex = 0;
	_beginthreadex(NULL,NULL,&(Loading::Update),this,NULL,NULL);
}

unsigned int __stdcall Loading::Update( VOID* arg )
{
	Loading* it = (Loading*)arg;
	
	it->m_lpDevice->SetRenderState(	D3DRS_CULLMODE,D3DCULL_NONE);
	it->m_lpDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	it->m_lpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	it->m_lpDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	it->m_lpDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
	it->m_lpDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	it->m_lpDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	it->m_lpDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	it->m_lpDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	it->m_lpDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	
	while( TRUE == (*(it->m_pIsFinish)) ){
		it->FrameMove();
		it->Render();
	}
	//it->m_lpDevice->SetRenderState(D3DRS_ZENABLE,TRUE);
	return 0;
}

VOID Loading::FrameMove()
{
	m_liPre = m_liCur;
	QueryPerformanceCounter(&m_liCur);
	m_fTime +=  (float) ((double(m_liCur.QuadPart - m_liPre.QuadPart)) / double(m_liFreq.QuadPart));

	D3DXVECTOR3 vEyePt( 0.0f, 0.0f,512.0f );
	D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	m_lpDevice->SetTransform( D3DTS_VIEW, &matView );

	if(m_fTime >= 0.1f){
		m_fTime = 0.0f;
		++m_iCurTex;
	}

	if(m_iCurTex >= TEX_MAX){
		m_iCurTex = 0;
		m_fTime = 0.0f;
	}
}

VOID Loading::Render()
{
	m_lpDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB( 255, 255, 255 ), 1.0f, 0 );

	if( SUCCEEDED( m_lpDevice->BeginScene() ) )
	{
		SetMat(m_vPos[m_iCurTex],m_fRot[m_iCurTex],m_vSize[m_iCurTex]);
		m_lpDevice->SetTexture( 0,m_lpTexture[m_iCurTex] );
		m_lpDevice->SetStreamSource( 0, m_lpVB, 0, sizeof( LOADVERTEX ) );
		m_lpDevice->SetFVF( D3DFVF_LOADVERTEX );
		m_lpDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

		m_lpDevice->EndScene();
	}
	m_lpDevice->Present( NULL, NULL, NULL, NULL );
}

HRESULT Loading::Init( HWND hWnd,LPDIRECT3D9 pDev )
{
	unsigned char* temp = NULL;

	for(int i = 0; i<TEX_MAX; ++i){
		m_vPos[i] = D3DXVECTOR3(0.0f,0.0f,0.0f);
		m_vSize[i] = D3DXVECTOR3(256.0f*1.11f,192.0f*1.11f,1.0f);
		m_fRot[i] = 0.0f;
	}

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof( d3dpp ) );
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

	if(FAILED(pDev->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &m_lpDevice))){
		return E_FAIL;
	}

	 temp = m_pFS->Read("laoding_1.png");
	 if(NULL != temp){
		 if(FAILED(D3DXCreateTextureFromFileInMemory(m_lpDevice,temp,m_pFS->ReadSize("laoding_1.png"),&m_lpTexture[0]))){
			 return E_FAIL;
		 }
	 }else{
		 printf("laoding_1\n");
		 MessageBox(NULL,_T("Failed"),_T("Error"),MB_OK);
		 return E_FAIL;
	 }
	 temp = NULL;

	 temp = m_pFS->Read("laoding_2.png");
	 if(NULL != temp){
		 if(FAILED(D3DXCreateTextureFromFileInMemory(m_lpDevice,temp,m_pFS->ReadSize("laoding_2.png"),&m_lpTexture[1]))){
			 return E_FAIL;
		 }
	 }else{
		 printf("laoding_2\n");
		 MessageBox(NULL,_T("Failed"),_T("Error"),MB_OK);
		 return E_FAIL;
	 }

	 temp = NULL;
	 temp = m_pFS->Read("laoding_3.png");
	 if(NULL != temp){
		 if(FAILED(D3DXCreateTextureFromFileInMemory(m_lpDevice,temp,m_pFS->ReadSize("laoding_3.png"),&m_lpTexture[2]))){
			 return E_FAIL;
		 }
	 }else{
		 printf("laoding_3\n");
	 }

	 temp = NULL;
	 temp = m_pFS->Read("laoding_4.png");
	 if(NULL != temp){
		 if(FAILED(D3DXCreateTextureFromFileInMemory(m_lpDevice,temp,m_pFS->ReadSize("laoding_4.png"),&m_lpTexture[3]))){
			 return E_FAIL;
		 }
	 }else{
		 printf("laoding_4\n");
		 MessageBox(NULL,_T("Failed"),_T("Error"),MB_OK);
		 return E_FAIL;
	 }

	 temp = NULL;
	 temp = m_pFS->Read("laoding_5.png");
	 if(NULL != temp){
		 if(FAILED(D3DXCreateTextureFromFileInMemory(m_lpDevice,temp,m_pFS->ReadSize("laoding_5.png"),&m_lpTexture[4]))){
			 return E_FAIL;
		 }
	 }else{
		 printf("laoding_5\n");
		 MessageBox(NULL,_T("Failed"),_T("Error"),MB_OK);
		 return E_FAIL;
	 }
	 temp = NULL;

	if( FAILED( m_lpDevice->CreateVertexBuffer( 4 * sizeof( LOADVERTEX ),
		0, D3DFVF_LOADVERTEX,
		D3DPOOL_DEFAULT, &m_lpVB, NULL ) ) )
	{
		return E_FAIL;
	}

	LOADVERTEX* pVertices;
	if( FAILED( m_lpVB->Lock( 0, 0, ( void** )&pVertices, 0 ) ) )
		return E_FAIL;


	pVertices[0].position = D3DXVECTOR3( -1.0f , 1.0f , 0.0f );
	pVertices[0].color = 0xffffffff;
	pVertices[0].tu = 1.0f;
	pVertices[0].tv = 0.0f;

	pVertices[1].position = D3DXVECTOR3( 1.0f , 1.0f , 0.0f );
	pVertices[1].color = 0xffffffff;
	pVertices[1].tu = 0.0f;
	pVertices[1].tv = 0.0f;

	pVertices[2].position = D3DXVECTOR3( -1.0f , -1.0f , 0.0f );
	pVertices[2].color = 0xffffffff;
	pVertices[2].tu = 1.0f;
	pVertices[2].tv = 1.0f;

	pVertices[3].position = D3DXVECTOR3( 1.0f , -1.0f , 0.0f );
	pVertices[3].color = 0xffffffff;
	pVertices[3].tu = 0.0f;
	pVertices[3].tv = 1.0f;

	m_lpVB->Unlock();

	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI / 4, 4.0f/3.0f, 1.0f, 1000.0f );
	m_lpDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	return S_OK;
}

void Loading::SetMat(D3DXVECTOR3 pPos,float pRot,D3DXVECTOR3 pSize)
{
	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matScail;
	D3DXMATRIXA16 matTrans;
	D3DXMatrixIdentity( &matWorld );
	D3DXMatrixTranslation(&matTrans,pPos.x,pPos.y,pPos.z);
	D3DXMatrixScaling(&matScail,pSize.x,pSize.y,pSize.z);
	D3DXMatrixRotationZ( &matWorld, pRot );
	matWorld = matScail * matWorld *  matTrans;
	m_lpDevice->SetTransform( D3DTS_WORLD, &matWorld );
}