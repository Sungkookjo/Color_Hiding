#include "stdafx.h"

O2Camera::O2Camera()
:m_vPos(0,0,0)
,m_vLPos(0,0,0)
,m_fMoveSpeed(10.0f)
,m_fRotSpeed(5.0f)
,m_fSlerp(0.1f)
,m_fDistance(-50.0f)	//-50
,m_fDistminLimit(-25.0f)	//-15
,m_fDistmaxLimit(-80.0f)//-80
,m_fCODistance(0.0f)
,m_fCODistance2(0.0f)
,m_fLRaydist(0.0f)
,m_fRotX(0.0f)	
{
	D3DXQuaternionIdentity(&m_qOrign);
	D3DXQuaternionIdentity(&m_qQuatout);
	D3DXMatrixIdentity(&m_matView);
}

O2Camera::~O2Camera()
{

}

void O2Camera::RotX(float fangle)
{
	fangle *= (m_fRotSpeed / 60);
	m_fRotX += fangle;
	float limitminx=-0.8f;	//���Ѱ� -0.8
	float limitmaxx=0.4f;	//	0.4
	if(m_fRotX>=limitmaxx)
	{
		m_fRotX=limitmaxx;

	}else if(m_fRotX<=limitminx)
	{
		m_fRotX=limitminx;
	}

}

void O2Camera::RotY(float fangle)
{
	fangle *= (m_fRotSpeed/60);
	D3DXQUATERNION rot;	
	D3DXQuaternionRotationAxis(&rot, &D3DXVECTOR3(0.0f, 1.0f, .0f), fangle);
	m_qOrign *= rot;
	D3DXQuaternionNormalize(&m_qOrign, &m_qOrign);


}

void O2Camera::RotZ(float fangle)
{
	fangle *= (m_fRotSpeed/60);
	D3DXQUATERNION rot;
	D3DXQuaternionRotationAxis(&rot, TransformAxis(&m_qOrign, &D3DXVECTOR3(0.0f, 0.0f, 1.0f)), fangle);
	m_qOrign *= rot;	
	D3DXQuaternionNormalize(&m_qOrign, &m_qOrign);

}


//	�Ⱦ��� ��������
void O2Camera::SetLookAt(D3DXVECTOR3 eye, D3DXVECTOR3 look, D3DXVECTOR3 up)
{
	D3DXMATRIX matTemp;		  // eye, lookat, up
	D3DXMatrixLookAtLH(&matTemp, &eye, &look, &up);
	m_vPos = look;

	D3DXQuaternionRotationMatrix(&m_qOrign, &matTemp);	// ���� ��� 

}

bool O2Camera::Slerp(D3DXQUATERNION* quatTarget)
{

	// �� ���� ���̸� �ε巴�� ����(sphere linear interpolation)
	if(m_qOrign == (*quatTarget))
	{
		return false;
	}

	// �� ����� orientation�� quatTarget�� m_fSlerp(0~1)������ ������ Slerp ����.
	D3DXQuaternionSlerp(&m_qOrign, &m_qOrign, quatTarget, m_fSlerp);
	D3DXQuaternionNormalize(&m_qOrign, &m_qOrign);


	return true;
}

//	������Ʈ�� ī�޶� �浹����
bool O2Camera::SlerpCollsion(float _Raydist)
{
	float Lerpspeed = _Raydist*m_fSlerp*0.5f;

	m_fLRaydist += Lerpspeed;

//	����Ÿ��� ���Ѱ����� �۾������  
	if(m_fDistance+m_fLRaydist>=m_fDistminLimit )
	{
		m_fLRaydist = ( m_fDistminLimit-(m_fDistance) );
		return false;
	}

//	������ ������ ���°����� Ŀ����
	if(m_fLRaydist>=_Raydist)
	{
		m_fLRaydist=_Raydist;
		return false;
	}

	return true;

}


//	���ʹϿ� ���� ori�κ��� ���� axis�� ����
//	(1,0,0) = right, (0,1,0) = up, (0,0,1) = look 
//	�ε� ���� ���͸� ������ �ִ�.
D3DXVECTOR3* O2Camera::TransformAxis(D3DXQUATERNION *_ori, D3DXVECTOR3 *_axis)
{

	// ī�޶� ȸ���� ��Ÿ���� orientation���κ��� ȸ������ ���� ���Ѵ�.
	D3DXMATRIX matRotation;
	D3DXMatrixRotationQuaternion(&matRotation, _ori);

	// ���ڷ� ���� �� ���͸� ȸ������ ���ο� ���� �����.
	D3DXVECTOR3 axisNew;
	axisNew.x = _axis->x * matRotation._11 + _axis->y * matRotation._21 + _axis->z * matRotation._31 + matRotation._41; 
	axisNew.y = _axis->x * matRotation._12 + _axis->y * matRotation._22 + _axis->z * matRotation._32 + matRotation._42;
	axisNew.z = _axis->x * matRotation._13 + _axis->y * matRotation._23 + _axis->z * matRotation._33 + matRotation._43;

	memcpy(_axis, &axisNew, sizeof(axisNew));

	return _axis;
	//	return &axisNew;
}

//	ī�޶� Ÿ���� ghost�ϰ��
D3DXMATRIXA16* O2Camera::GetViewMatrix()
{
	D3DXMATRIX matTranslation;
	D3DXMatrixTranslation(&matTranslation, -m_vPos.x, -m_vPos.y, -m_vPos.z);

	D3DXQUATERNION rot;
	D3DXQuaternionRotationAxis(&rot, TransformAxis(&m_qOrign, &D3DXVECTOR3(1.0f, 0.0f, 0.0f)), m_fRotX);
	m_qQuatout = m_qOrign * rot;
	D3DXQuaternionNormalize(&m_qQuatout, &m_qQuatout);

	D3DXMATRIX matRotation;
	D3DXMatrixRotationQuaternion(&matRotation, 
		&D3DXQUATERNION(-m_qQuatout.x, -m_qQuatout.y, -m_qQuatout.z, m_qQuatout.w));

	D3DXMatrixMultiply(&m_matView, &matTranslation, &matRotation);

	return &m_matView;
}

//	ī�޶� Ÿ���� around , follow�� ���
D3DXMATRIXA16* O2Camera::GetViewMatrix(D3DXVECTOR3* _pos,float _y,float _Raydist, bool _state)
{

//// ȸ���� ����
	D3DXQUATERNION rot;
	D3DXQuaternionRotationAxis(&rot, TransformAxis(&m_qOrign, &D3DXVECTOR3(1.0f, 0.0f, 0.0f)), m_fRotX);
	m_qQuatout = m_qOrign * rot;
	D3DXQuaternionNormalize(&m_qQuatout, &m_qQuatout);

	D3DXMATRIX matRotation;
	D3DXMatrixRotationQuaternion(&matRotation, 
		&D3DXQUATERNION(-m_qQuatout.x, -m_qQuatout.y, -m_qQuatout.z, m_qQuatout.w));

	m_matView =  matRotation  ;
	
D3DXVECTOR3 look,eye,up;
//	�躤��
	look.x=m_matView._13;
	look.y=m_matView._23;
	look.z=m_matView._33;
//	������
	up.x =m_matView._12;
	up.y =m_matView._22;
	up.z =m_matView._32;
//	��ġ���� �浹( O , X )
	m_vPos = (*_pos) + look * (m_fDistance+m_fLRaydist) - (up * (m_fDistance+m_fLRaydist))/2.0f;
	m_vLPos = (*_pos) + look * m_fDistance - (up * m_fDistance)/2.0f;
//	��ġ���� - �÷��̾� ����� ����
	m_vPos.y += _y;
	m_vLPos.y += _y;

//	�ӽ� - �ٴ�ó�� 08041011
//	����� ��ġ�� ����
	if(m_vPos.y<=_pos->y+1.0f)
	{
		m_vPos.y=_pos->y+1.0f;
	}
	if(m_vLPos.y<=_pos->y+1.0f)
	{
		m_vLPos.y=_pos->y+1.0f;
	}

//	����� ���� eye , lookat
	eye = m_vPos;
	look = (*_pos) + look * 25.0f ;
	look.y += _y;

//	�÷��̾�� ī�޶� �Ÿ� ����
	D3DXVECTOR3 objPos=(*_pos);
	objPos.y += _y;
//	��:������ ��� ī�޶�� �Ÿ�
//	��:�÷��̾ �����ִ� ī�޶�� �Ÿ�
	m_fCODistance = D3DXVec3Length(&(m_vLPos-objPos));
	m_fCODistance2 = D3DXVec3Length(&(m_vPos-objPos));

//	����� ���� up
	up=D3DXVECTOR3(0,1,0);
	TransformAxis(&m_qQuatout,&up);
//	����� ����
	D3DXMatrixLookAtLH(&m_matView,&eye,&look,&up);


	return &m_matView;
}

void O2Camera::Zoom(float fweight,D3DXVECTOR3 _look)
{

	m_fDistance += fweight;

	if(m_fDistance+m_fLRaydist>=(m_fDistminLimit))				//�����ִ� ī�޶� �ּ����Ѱ����� �۾�����
	{
		m_fDistance = (	m_fDistminLimit-m_fLRaydist );
	}else if(m_fDistance+m_fLRaydist<=(m_fDistmaxLimit))		//�����ִ� ī�޶� �ִ����Ѱ����� Ŀ����
	{
		m_fDistance = ( m_fDistmaxLimit-m_fLRaydist );
	}

//	ī�޶� Ÿ���� ghost�ϰ�쿡�� _look�� Ȱ��ȭ�Ǿ� ������ default=0
	m_vPos += _look * fweight * CAM_MOVE_SPEED /10.0f;

}

void O2Camera::SetCamera(D3DXQUATERNION *_target_quat,float _Raydist,bool _Camerastate,bool _Slerpstate)
{

//	ī�޶�-������Ʈ�� �浹ó��
//	(_Raydist<=0.0001f)? m_fLRaydist = _Raydist : SlerpCollsion(_Raydist);
	
	
//	ī�޶� Ÿ�� Ȯ�� / ���� ���� Ȯ��
	if(!_Camerastate)
	{
		if(_Slerpstate)
		{
			Slerp(_target_quat);
		}else
		{
			m_qOrign = (*_target_quat) ;
		}
	}

}



void O2Camera::MoveX( float dist )
{
	D3DXVECTOR3 right;

	right.x = m_matView._11;
	right.y = m_matView._21;
	right.z = m_matView._31; 


	m_vPos += right * dist * CAM_MOVE_SPEED;
}

void O2Camera::MoveY( float dist )
{
	D3DXVECTOR3 up;

	up.x = m_matView._12;
	up.y = m_matView._22;
	up.z = m_matView._32; 


	m_vPos += up * dist * CAM_MOVE_SPEED;
}

void O2Camera::MoveZ( float dist )
{
	D3DXVECTOR3 look;

	look.x = m_matView._13;
	look.y = m_matView._23;
	look.z = m_matView._33; 

	m_vPos += look * dist * CAM_MOVE_SPEED;
}

