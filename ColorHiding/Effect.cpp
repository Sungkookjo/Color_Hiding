#include "stdafx.h"
#include "O2CameraManager.h"

Effect::Effect( LPDIRECT3DDEVICE9* pDev,O2CameraManager* pCam,Basefw_FS* FS)
:m_lpEffect(NULL)
,m_lpDeep(NULL)
,m_lpOriginTex(NULL)
,m_lpOriginSurf(NULL)
,m_pCam(pCam)
,m_lpDevice(pDev)
,m_pFS(FS)
{
}

Effect::~Effect()
{
	SAFE_RELEASE(m_lpDeep);
	SAFE_RELEASE(m_lpOriginTex);
	SAFE_RELEASE(m_lpOriginSurf);

	SAFE_RELEASE(m_lpEffect);
}

HRESULT Effect::Init(TCHAR* _file)
{
	// 리소스 파일 메모리 주소를 읽어옴.
	char FileName[MAX_PATH];
	UINT _len = _tcslen(_file);
#ifdef UNICODE
	wcstombs(FileName,_file,_len);
	FileName[_len] = NULL;
#else
	strcpy_s(FileName,_file);
#endif
	LPCSTR temp = (char*)(m_pFS->Read(FileName));

	HRESULT hr = 0;
	LPD3DXBUFFER _error = NULL;

	// 깊이버퍼
	if (FAILED((*m_lpDevice)->CreateDepthStencilSurface(
		MAP_WIDTH, MAP_HEIGHT, 
		D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &m_lpDeep, NULL)))
		return E_FAIL;
	// 색정보
	if (FAILED((*m_lpDevice)->CreateTexture(
		MAP_WIDTH, MAP_HEIGHT, 1, D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_lpOriginTex, NULL)))
		return E_FAIL;
	if (FAILED(m_lpOriginTex->GetSurfaceLevel(0, &m_lpOriginSurf)))
		return E_FAIL;

	if(temp){
		hr =D3DXCreateEffect(
			(*m_lpDevice),
			temp,
			m_pFS->ReadSize(FileName),
			NULL,
			NULL,
			D3DXSHADER_DEBUG | D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY , 
			NULL,
			&m_lpEffect,
			&_error
			);
	}else{
		return E_FAIL;
	}

	if(_error){
		::MessageBoxA(0, (char*)_error->GetBufferPointer(),0,0);
		SAFE_RELEASE(_error);
	}
	if(FAILED(hr)){
		MessageBox(0, _T("D3DXCreateEffectFromFile() - FAILED"),0,0);
		return E_FAIL;
	}

	// 핸들을 얻는다.
	m_hmWorld	 = m_lpEffect->GetParameterByName( NULL, "mWorld" );
	m_hmView	 = m_lpEffect->GetParameterByName( NULL, "mView" );
	m_hmProj	 = m_lpEffect->GetParameterByName( NULL, "mProj" );
	m_hmRT		 = m_lpEffect->GetParameterByName( NULL, "mRT" );

	return S_OK;
}

UINT Effect::PreRender()
{
	UINT Pass;

	m_lpEffect->SetMatrix(m_hmView,m_pCam->GetMatView());
	m_lpEffect->SetMatrix(m_hmProj,m_pCam->GetProjView());
	m_lpEffect->Begin( &Pass, 0);

	return Pass;
}

void Effect::PostRender()
{
	m_lpEffect->End();
}

void Effect::Commit()
{
	m_lpEffect->CommitChanges();
}

void Effect::BeginPass( UINT i )
{
	m_lpEffect->BeginPass(i);
}

void Effect::EndPass()
{
	m_lpEffect->EndPass();
}