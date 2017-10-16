#include "stdafx.h"
#include "O2PartFollow.h"

void O2PartFollow::SetupPosition()
{
	m_vCurPosition = (*m_vTargetPosition) ;
	m_vFirstPosition = D3DXVECTOR3(0,0,0);
}

D3DXVECTOR3 O2PartFollow::GetcurPos( Particle* part )
{
	return (part->m_vCurPos + m_vCurPosition);
}