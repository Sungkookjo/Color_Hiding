#pragma once

class O2PartWhirl:public O2ParticleSystem
{
public:
	virtual void	Update();
	virtual HRESULT Render( LPDIRECT3DDEVICE9 pd3dDevice );

};