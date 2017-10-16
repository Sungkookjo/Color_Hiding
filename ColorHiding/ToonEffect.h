#pragma once

//#define LIGHT_DIRECTION		D3DXVECTOR4(0.0f,200.0f,0.0f,0.0f)
#define LIGHT_DIRECTION		D3DXVECTOR4(-0.4f,0.4f,-0.2f,0.0f)

class ToonEffect:public Effect{
private:
	D3DXHANDLE			m_hTechAmbient	;	//	셀 쉐이딩
	D3DXHANDLE			m_hTechCell		;	//	셀 쉐이딩
	D3DXHANDLE			m_hTechLine		;	//	외곽선 쉐이딩
	D3DXHANDLE			m_hTechShadow	;	//	볼륨 쉐이딩
	D3DXHANDLE			m_hvColor		;	//	재질 색
	D3DXHANDLE			m_hvLight		;	//	라이트 방향
	D3DXHANDLE			m_hpTex			;	//	본래 텍스처
	D3DXHANDLE			m_hmScail		;	//	외곽을 위한 스케일 행렬
	D3DXHANDLE			m_hfFarClip		;	//	z of far clip plane
	D3DXHANDLE			m_hvShadowColor	;	//	ShadowColor
	D3DXHANDLE			m_hmLight		;	//	Light matrix

	LPDIRECT3DTEXTURE9	m_pTexture	;	//	음영 텍스처 버퍼
public:
	ToonEffect(LPDIRECT3DDEVICE9* pDev,O2CameraManager* pCam,Basefw_FS* FS);
	~ToonEffect();
	HRESULT Init(TCHAR* _file);
	void SetFloat(UINT _flag,float pfloat);
	void SetVector(UINT _flag,D3DXVECTOR4& pVec);
	void SetVector(UINT _flag,D3DCOLORVALUE& pVec);
	void SetMatrix(UINT _flag,D3DXMATRIXA16& pVec);
	void SetTexture(UINT _flag,LPDIRECT3DTEXTURE9 pTex);
	void SetTechnique(UINT _flag);

};