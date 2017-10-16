#include "stdafx.h"
#include "O2CameraManager.h"

O2CameraManager::O2CameraManager()
:m_pCam(NULL),
m_Hero(NULL),
m_pInput(NULL),
m_lpDevice(NULL),
m_uType(C_FOLLOW),
m_Raydist(0),
m_RayPos(0,0,0),
m_RayDir(0,0,0),
m_vFixedPos(0,0,0),
m_vFixedlook(0,0,0),
m_vFixedUp(0,1,0),
m_fDelta(NULL)
{
	D3DXMatrixIdentity(&m_matProj);
	D3DXMatrixIdentity(&m_matView);
}

O2CameraManager::~O2CameraManager()
{
	SAFE_DELETE(m_pCam);
	m_lpDevice = NULL;
	m_Hero = NULL;
	m_pInput = NULL;
}

HRESULT O2CameraManager::Init(LPDIRECT3DDEVICE9* _device,O2InputManager* input,float* _Delta)
{
	m_lpDevice	=	_device;
	m_pInput	=	input;
	m_fDelta	=	_Delta;


	m_pCam = new O2Camera;

	if(m_pCam==NULL)
		return E_FAIL;

	m_matView=*(m_pCam->GetViewMatrix());
	(*m_lpDevice)->SetTransform( D3DTS_VIEW, &m_matView );

	D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI * 0.25f , 4.0f/3.0f, 1.0f, 1000.0f );
	(*m_lpDevice)->SetTransform( D3DTS_PROJECTION, &m_matProj );


	return S_OK;
}

void O2CameraManager::Update()
{

	switch (m_uType)
	{
	case C_GHOST:
		m_matView=*(m_pCam->GetViewMatrix());
		break;
	case C_FIXED:
		D3DXMatrixLookAtLH(&m_matView,&m_vFixedPos,&m_vFixedlook,&m_vFixedUp);
		break;
	default:
		m_matView=*(m_pCam->GetViewMatrix());
		break;
	}

	(*m_lpDevice)->SetTransform( D3DTS_VIEW, &m_matView );
}


void O2CameraManager::Update(O2Hero* _hero)
{
//	임시
//	캐릭터의 회전행렬얻기
	m_Hero = _hero;
	D3DXQUATERNION temp;
	D3DXQuaternionRotationMatrix(&temp,&(_hero->GetMatRT()));

//	카메라 타입에 따라 업데이트

	switch (m_uType)
	{
	case C_GHOST:
		m_matView=*(m_pCam->GetViewMatrix());
		break;
	case C_FOLLOW:
		m_pCam->SetCamera(&temp,m_Raydist,FALSE,TRUE);
		m_matView=*(m_pCam->GetViewMatrix(&(_hero->GetPos()),
			(_hero->GetColl()._radius*_hero->GetSize().y)*1.5f,m_Raydist,FALSE));
		break;
	case C_AROUND:
		m_pCam->SetCamera(&temp,m_Raydist,TRUE,FALSE);
		m_matView=*(m_pCam->GetViewMatrix(&(_hero->GetPos()),
			(_hero->GetColl()._radius*_hero->GetSize().y)*1.5f,m_Raydist,TRUE));
		break;
	case C_FIXED:
		D3DXMatrixLookAtLH(&m_matView,&m_vFixedPos,&m_vFixedlook,&m_vFixedUp);
		break;
	case C_DEADS1:
		m_matView=DeadScene1();
		break;
	default:
		m_matView=*(m_pCam->GetViewMatrix());
		break;
	}

	(*m_lpDevice)->SetTransform( D3DTS_VIEW, &m_matView );


//  픽킹관련 업데이트
	m_RayDir = D3DXVECTOR3(m_matView._13,m_matView._23,m_matView._33);
	D3DXVec3Normalize(&m_RayDir,&m_RayDir);
	m_RayPos = *(m_pCam -> GetLPos());
}




void O2CameraManager::Zoom(float fweight)
{
	if(m_uType==C_GHOST)
	{
		D3DXVECTOR3 look;
		look.x = m_matView._13;
		look.y = m_matView._23;
		look.z = m_matView._33; 
		m_pCam->Zoom(fweight,look);
	}else
	{
		m_pCam->Zoom(fweight);
	}
}

void O2CameraManager::Input()
{
	float fweight = 0.01f;
	D3DXVECTOR3 temp(0,0,0);
	D3DXVECTOR3 dir(1,0,0);


	if(m_pInput->IsKeyDown(DIK_DELETE))
		RotX(fweight*10);
	else if(m_pInput->IsKeyDown(DIK_INSERT))
		RotX(-fweight*10);

	if(m_pInput->IsKeyDown(DIK_END))
		RotY(fweight*10);
	else if(m_pInput->IsKeyDown(DIK_HOME))
		RotY(-fweight*10);


	if(m_pInput->IsKeyDown(DIK_D))
		MoveX(fweight);
	else if(m_pInput->IsKeyDown(DIK_A))
		MoveX(-fweight);

	if(m_pInput->IsKeyDown(DIK_E))
		MoveY(fweight);
	else if(m_pInput->IsKeyDown(DIK_Q))
		MoveY(-fweight);

	if(m_pInput->IsKeyDown(DIK_W))
		MoveZ(fweight);
	else if(m_pInput->IsKeyDown(DIK_S))
		MoveZ(-fweight);

	if(m_pInput->IsKeyPress(DIK_MINUS))
	{
		Switch_CT(C_DEADS1);
	}

	//마우스

	DIMOUSESTATE MouseState = m_pInput->GetMouseState();

	if(m_uType==C_GHOST)
	{
		if(MouseState.rgbButtons[0])
		{
			if(MouseState.lY>0)
				RotX(MouseState.lY*fweight*10);
			else if(MouseState.lY<0)
				RotX(MouseState.lY*fweight*10);

			if(MouseState.lX>0)
				RotY(MouseState.lX*fweight*10);
			else if(MouseState.lX<0)
				RotY(MouseState.lX*fweight*10);
		}
	}else if(m_uType==C_AROUND || m_uType==C_FOLLOW)
	{
		if(MouseState.rgbButtons[0])
		{
			m_uType = C_AROUND;

			if(MouseState.lX>0)
				RotY(MouseState.lX*fweight*10);
			else if(MouseState.lX<0)
				RotY(MouseState.lX*fweight*10);
		}else
		{
			m_uType = C_FOLLOW;
		}
	}
	
	
	if(MouseState.lY<0)
		RotX(MouseState.lY*fweight*(10));
	else if(MouseState.lY>0)
		RotX(MouseState.lY*fweight*(10));

	if(MouseState.lZ>0)
		Zoom(fweight*300);
	else if(MouseState.lZ<0)
		Zoom(-fweight*300);
	
}

void O2CameraManager::SetFixedCamera(D3DXVECTOR3 vEye , D3DXVECTOR3 vLookat, D3DXVECTOR3 vUp/* 0 , 1, 0 */)
{
	m_vFixedPos = vEye;
	m_vFixedlook = vLookat;
	m_vFixedUp = vUp;
}

D3DXMATRIXA16 O2CameraManager::DeadScene1()
{
	return m_matView;
}


void O2CameraManager::ResetCamera(UINT _State)
{
	m_pCam->SetDistance(-80.0f);
	m_pCam->SetRotX(0);
	D3DXQUATERNION temp;
	D3DXQuaternionIdentity(&temp);
	if(_State==2)
	{
		D3DXQUATERNION rot;	
		D3DXQuaternionRotationAxis(&rot, &D3DXVECTOR3(0.0f, 1.0f, .0f), D3DXToRadian(90));
		temp *= rot;
		D3DXQuaternionNormalize(&temp, &temp);
	}else if(_State==3)
	{
		D3DXQUATERNION rot;	
		D3DXQuaternionRotationAxis(&rot, &D3DXVECTOR3(0.0f, 1.0f, .0f), D3DXToRadian(180));
		temp *= rot;
		D3DXQuaternionNormalize(&temp, &temp);
	}
	m_pCam->SetOrientation(temp);
}

