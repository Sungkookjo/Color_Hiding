#pragma once

//#define LIGHT_DIRECTION		D3DXVECTOR4(0.0f,200.0f,0.0f,0.0f)
#define LIGHT_DIRECTION		D3DXVECTOR4(-0.4f,0.4f,-0.2f,0.0f)

class ToonEffect:public Effect{
private:
	D3DXHANDLE			m_hTechAmbient	;	//	�� ���̵�
	D3DXHANDLE			m_hTechCell		;	//	�� ���̵�
	D3DXHANDLE			m_hTechLine		;	//	�ܰ��� ���̵�
	D3DXHANDLE			m_hTechShadow	;	//	���� ���̵�
	D3DXHANDLE			m_hvColor		;	//	���� ��
	D3DXHANDLE			m_hvLight		;	//	����Ʈ ����
	D3DXHANDLE			m_hpTex			;	//	���� �ؽ�ó
	D3DXHANDLE			m_hmScail		;	//	�ܰ��� ���� ������ ���
	D3DXHANDLE			m_hfFarClip		;	//	z of far clip plane
	D3DXHANDLE			m_hvShadowColor	;	//	ShadowColor
	D3DXHANDLE			m_hmLight		;	//	Light matrix

	LPDIRECT3DTEXTURE9	m_pTexture	;	//	���� �ؽ�ó ����
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