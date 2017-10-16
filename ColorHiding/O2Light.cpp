#include "stdafx.h"
#include "O2Light.h"

O2Light::O2Light()
{
	m_vDir = D3DXVECTOR3(0.2f,-0.5f,-0.3f);

	Theta = D3DX_PI * 2.0f;
}

O2Light::~O2Light()
{

}

VOID O2Light::SetMat()
{
	D3DXMatrixTranslation(&m_matTrans,m_vPos.x,m_vPos.y,m_vPos.z);

	m_matWorld = m_matSize * m_matRot * m_matTrans;

	// 부모행렬이 있는 경우 곱해준다.
	if(m_pParent){
		m_matWorld *= m_pParent->GetMatWorld();
	}
}