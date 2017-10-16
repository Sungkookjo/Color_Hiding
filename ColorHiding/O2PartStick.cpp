#include "stdafx.h"
#include "O2PartStick.h"


void O2PartStick::SetupWorld()
{
	D3DXMATRIX matWorld,matRotation;
	D3DXMatrixTranslation(&matWorld,m_vCurPosition.x,m_vCurPosition.y,m_vCurPosition.z);
	if(m_qTargetQuat!=NULL)
	{
			D3DXMatrixRotationQuaternion(&matRotation, 
		&D3DXQUATERNION(m_qTargetQuat->x, m_qTargetQuat->y, m_qTargetQuat->z, m_qTargetQuat->w));

	}else
	{
		D3DXMatrixIdentity(&matRotation);
	}
	
	matWorld =  matRotation * matWorld;

	m_device->SetTransform(D3DTS_WORLD,&matWorld);
}

void O2PartStick::SetupPosition()
{
	if(m_mTargetMat!=NULL)
	{
		D3DXVECTOR3 vLook(m_mTargetMat->_13,m_mTargetMat->_23,m_mTargetMat->_33);
		D3DXVECTOR3 vUp(m_mTargetMat->_12,m_mTargetMat->_22,m_mTargetMat->_32);
		D3DXVECTOR3 vRight(m_mTargetMat->_11,m_mTargetMat->_21,m_mTargetMat->_31);
		m_vCurPosition = (*m_vTargetPosition) + (vLook*m_vVarValue.z) + (vUp*m_vVarValue.y) + (vRight*m_vVarValue.x);
	//	printf("%f %f %f\n",m_vCurPosition.x,m_vCurPosition.y,m_vCurPosition.z);
	}	
	m_vFirstPosition = D3DXVECTOR3(0,0,0);
}
