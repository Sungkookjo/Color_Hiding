//////////////////////////////////////////////////////////////////////////
// JDevice
// Device Å¬·¡½º
#pragma once

class JDevice{
private:
	LPDIRECT3D9             m_pD3D; // Used to create the D3DDevice
	LPDIRECT3DDEVICE9       m_lpDevice; // Our rendering device
	D3DCAPS9				m_D3Dcaps;
public:
	JDevice();
	~JDevice();
	HRESULT InitD3D( HWND hWnd );
	VOID Cleanup();
	LPDIRECT3DDEVICE9*	GetDevice() { return &m_lpDevice; }
	LPDIRECT3D9			GetD3D9()	{ return m_pD3D; }
	D3DCAPS9*			GetD3DCaps() { return &m_D3Dcaps; }
};