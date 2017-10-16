#pragma once

class O2Light:public O2Object{
private:
	virtual	VOID		CleanUp() {}
	virtual	VOID		SetMat();
	float				Theta;
public:
	O2Light();
	~O2Light();
	virtual HRESULT		RenderLine() { return S_OK;}
	virtual	HRESULT		UpDate() { SetMat();return S_OK; }
	virtual	HRESULT		Render() { return S_OK; }
	virtual	HRESULT		RenderShadow(UINT pass) { return S_OK; }

	D3DXVECTOR4		GetLightPos() { return D3DXVECTOR4(m_vPos.x,m_vPos.y,m_vPos.z,1.0f); }
	D3DXVECTOR4		GetLightDir() { return D3DXVECTOR4(m_vDir.x,m_vDir.y,m_vDir.z,1.0f); }
	FLOAT			GetTheta() { return Theta; }
	virtual BOOL		IsRender(O2Frustum* pFrustrum) { return 0; }
};