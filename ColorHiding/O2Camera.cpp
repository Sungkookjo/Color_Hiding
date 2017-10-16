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
	float limitminx=-0.8f;	//제한값 -0.8
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


//	안쓰임 아직까진
void O2Camera::SetLookAt(D3DXVECTOR3 eye, D3DXVECTOR3 look, D3DXVECTOR3 up)
{
	D3DXMATRIX matTemp;		  // eye, lookat, up
	D3DXMatrixLookAtLH(&matTemp, &eye, &look, &up);
	m_vPos = look;

	D3DXQuaternionRotationMatrix(&m_qOrign, &matTemp);	// 방향 얻기 

}

bool O2Camera::Slerp(D3DXQUATERNION* quatTarget)
{

	// 두 방향 사이를 부드럽게 보간(sphere linear interpolation)
	if(m_qOrign == (*quatTarget))
	{
		return false;
	}

	// 두 사원수 orientation과 quatTarget을 m_fSlerp(0~1)사이의 값으로 Slerp 보간.
	D3DXQuaternionSlerp(&m_qOrign, &m_qOrign, quatTarget, m_fSlerp);
	D3DXQuaternionNormalize(&m_qOrign, &m_qOrign);


	return true;
}

//	오브젝트와 카메라간 충돌보간
bool O2Camera::SlerpCollsion(float _Raydist)
{
	float Lerpspeed = _Raydist*m_fSlerp*0.5f;

	m_fLRaydist += Lerpspeed;

//	현재거리가 제한값보다 작아질경우  
	if(m_fDistance+m_fLRaydist>=m_fDistminLimit )
	{
		m_fLRaydist = ( m_fDistminLimit-(m_fDistance) );
		return false;
	}

//	보간된 광선이 얻어온값보다 커질때
	if(m_fLRaydist>=_Raydist)
	{
		m_fLRaydist=_Raydist;
		return false;
	}

	return true;

}


//	쿼터니온 인자 ori로부터 인자 axis를 각각
//	(1,0,0) = right, (0,1,0) = up, (0,0,1) = look 
//	로된 방향 벡터를 얻을수 있다.
D3DXVECTOR3* O2Camera::TransformAxis(D3DXQUATERNION *_ori, D3DXVECTOR3 *_axis)
{

	// 카메라 회전을 나타내는 orientation으로부터 회전축을 먼저 구한다.
	D3DXMATRIX matRotation;
	D3DXMatrixRotationQuaternion(&matRotation, _ori);

	// 인자로 받은 축 벡터를 회전시켜 새로운 축을 만든다.
	D3DXVECTOR3 axisNew;
	axisNew.x = _axis->x * matRotation._11 + _axis->y * matRotation._21 + _axis->z * matRotation._31 + matRotation._41; 
	axisNew.y = _axis->x * matRotation._12 + _axis->y * matRotation._22 + _axis->z * matRotation._32 + matRotation._42;
	axisNew.z = _axis->x * matRotation._13 + _axis->y * matRotation._23 + _axis->z * matRotation._33 + matRotation._43;

	memcpy(_axis, &axisNew, sizeof(axisNew));

	return _axis;
	//	return &axisNew;
}

//	카메라 타입이 ghost일경우
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

//	카메라 타입이 around , follow일 경우
D3DXMATRIXA16* O2Camera::GetViewMatrix(D3DXVECTOR3* _pos,float _y,float _Raydist, bool _state)
{

//// 회전을 적용
	D3DXQUATERNION rot;
	D3DXQuaternionRotationAxis(&rot, TransformAxis(&m_qOrign, &D3DXVECTOR3(1.0f, 0.0f, 0.0f)), m_fRotX);
	m_qQuatout = m_qOrign * rot;
	D3DXQuaternionNormalize(&m_qQuatout, &m_qQuatout);

	D3DXMATRIX matRotation;
	D3DXMatrixRotationQuaternion(&matRotation, 
		&D3DXQUATERNION(-m_qQuatout.x, -m_qQuatout.y, -m_qQuatout.z, m_qQuatout.w));

	m_matView =  matRotation  ;
	
D3DXVECTOR3 look,eye,up;
//	룩벡터
	look.x=m_matView._13;
	look.y=m_matView._23;
	look.z=m_matView._33;
//	업벡터
	up.x =m_matView._12;
	up.y =m_matView._22;
	up.z =m_matView._32;
//	위치갱신 충돌( O , X )
	m_vPos = (*_pos) + look * (m_fDistance+m_fLRaydist) - (up * (m_fDistance+m_fLRaydist))/2.0f;
	m_vLPos = (*_pos) + look * m_fDistance - (up * m_fDistance)/2.0f;
//	위치갱신 - 플레이어 사이즈에 따라
	m_vPos.y += _y;
	m_vLPos.y += _y;

//	임시 - 바닥처리 08041011
//	히어로 위치로 변경
	if(m_vPos.y<=_pos->y+1.0f)
	{
		m_vPos.y=_pos->y+1.0f;
	}
	if(m_vLPos.y<=_pos->y+1.0f)
	{
		m_vLPos.y=_pos->y+1.0f;
	}

//	뷰행렬 설정 eye , lookat
	eye = m_vPos;
	look = (*_pos) + look * 25.0f ;
	look.y += _y;

//	플레이어와 카메라 거리 갱신
	D3DXVECTOR3 objPos=(*_pos);
	objPos.y += _y;
//	상:광선을 쏘는 카메라와 거리
//	하:플레이어가 보고있는 카메라와 거리
	m_fCODistance = D3DXVec3Length(&(m_vLPos-objPos));
	m_fCODistance2 = D3DXVec3Length(&(m_vPos-objPos));

//	뷰행렬 설정 up
	up=D3DXVECTOR3(0,1,0);
	TransformAxis(&m_qQuatout,&up);
//	뷰행렬 갱신
	D3DXMatrixLookAtLH(&m_matView,&eye,&look,&up);


	return &m_matView;
}

void O2Camera::Zoom(float fweight,D3DXVECTOR3 _look)
{

	m_fDistance += fweight;

	if(m_fDistance+m_fLRaydist>=(m_fDistminLimit))				//보고있는 카메라가 최소제한값보다 작아질때
	{
		m_fDistance = (	m_fDistminLimit-m_fLRaydist );
	}else if(m_fDistance+m_fLRaydist<=(m_fDistmaxLimit))		//보고있는 카메라가 최대제한값보다 커질때
	{
		m_fDistance = ( m_fDistmaxLimit-m_fLRaydist );
	}

//	카메라 타입이 ghost일경우에만 _look이 활성화되어 더해짐 default=0
	m_vPos += _look * fweight * CAM_MOVE_SPEED /10.0f;

}

void O2Camera::SetCamera(D3DXQUATERNION *_target_quat,float _Raydist,bool _Camerastate,bool _Slerpstate)
{

//	카메라-오브젝트간 충돌처리
//	(_Raydist<=0.0001f)? m_fLRaydist = _Raydist : SlerpCollsion(_Raydist);
	
	
//	카메라 타입 확인 / 보간 설정 확인
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

