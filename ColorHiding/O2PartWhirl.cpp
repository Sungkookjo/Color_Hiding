#include "stdafx.h"
#include "O2PartWhirl.h"

HRESULT O2PartWhirl::Render(LPDIRECT3DDEVICE9 pd3dDevice)
{
	O2ParticleSystem::Render();
	return S_OK;
}