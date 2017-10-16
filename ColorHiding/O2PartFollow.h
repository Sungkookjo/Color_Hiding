#pragma once

class O2PartFollow:public O2ParticleSystem
{
public:
	virtual void	SetupPosition();
	virtual D3DXVECTOR3 GetcurPos(Particle* part);

};