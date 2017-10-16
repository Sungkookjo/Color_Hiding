#include "stdafx.h"

JDevice::JDevice()
:m_pD3D(NULL)
,m_lpDevice(NULL)
{
}

JDevice::~JDevice()
{
	Cleanup();
}

//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT JDevice::InitD3D( HWND hWnd )
{
	// Create the D3D object.
	if( NULL == ( m_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return E_FAIL;

	if(FAILED( m_pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		&m_D3Dcaps ) )){
			MessageBox(NULL,_T("Fail to get D3D Caps"),_T("Error"),MB_OK);
		return E_FAIL;
	}


	if( m_D3Dcaps.PixelShaderVersion < D3DPS_VERSION( 2, 0 ) ){
		MessageBox(NULL,_T("Must support pixel shader 2.0"),_T("Error"),MB_OK);
		return E_FAIL;
	}


	// Set up the structure used to create the D3DDevice
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof( d3dpp ) );
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.BackBufferWidth = 1024;
	d3dpp.BackBufferHeight = 768;

	// Create the D3DDevice
	if( FAILED( m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &m_lpDevice ) ) )
	{
		return E_FAIL;
	}

	m_lpDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID JDevice::Cleanup()
{
	SAFE_RELEASE(m_lpDevice);
	SAFE_RELEASE(m_pD3D);
}
