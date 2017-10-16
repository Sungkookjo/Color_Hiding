//////////////////////////////////////////////////////////////////////////
// O2Texture 
#include "stdafx.h"


//////////////////////////////////////////////////////////////////////////
// constructor
O2Texture::O2Texture(LPDIRECT3DDEVICE9* pDevice,Basefw_FS* FS)
:m_lpDevice(pDevice)
,m_lpTexture(NULL)
,m_pFS(FS)
{

}
//////////////////////////////////////////////////////////////////////////
// destructor
O2Texture::~O2Texture()
{
	Cleanup();
}

HRESULT O2Texture::Init( const TCHAR* filename)
{
	// 府家胶 颇老 皋葛府 林家甫 佬绢咳.
	char FileName[MAX_PATH];
	UINT _len = _tcslen(filename);
#ifdef UNICODE
	wcstombs(FileName,filename,_len);
	FileName[_len] = NULL;
#else
	strcpy_s(FileName,filename);
#endif
	unsigned char* temp = m_pFS->Read(FileName);

	if(temp)
	{
		// 府家胶 积己
		if( FAILED( D3DXCreateTextureFromFileInMemory((*m_lpDevice),
			temp,
			m_pFS->ReadSize(FileName),
			&m_lpTexture ) ))
		{  
			MessageBox(NULL, _T("FAILED : D3DXCreateTexture"), filename, MB_OK);
			return E_FAIL;
		}
	}else
	{
		MessageBox(NULL, _T("FAILED : ReadFile"), filename , MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

void O2Texture::Cleanup()
{
	SAFE_RELEASE(m_lpTexture);
}
