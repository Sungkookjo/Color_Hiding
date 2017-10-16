#pragma once

#include "O2InputManager.h"
#include "O2Hero.h"

class O2CameraManager
{

public:
	enum CAMERA_TYPE
	{
		C_FOLLOW = 0,
		C_AROUND = 1,
		C_GHOST  = 2,
		C_FIXED	 = 3,
		C_DEADS1  = 4
	};




	O2CameraManager();
	~O2CameraManager();
	HRESULT				Init(LPDIRECT3DDEVICE9* _device,O2InputManager* input,float* _Delta);

	void				Update();					//ī�޶� ������Ʈ  - ���ο���
	void				Update(O2Hero* _hero);		//ī�޶� ������Ʈ  - ���� ����

	void				Zoom(float fweight);	//ī�޶� ��-��,�ƿ�
	void				Input();				//ī�޶� Ű�Է�

	D3DXMATRIXA16		DeadScene1();


//////ī�޶� ȸ��
	inline void		RotX(float fangle) { m_pCam->RotX(fangle); }			
	inline void		RotY(float fangle) { m_pCam->RotY(fangle); }			
	inline void		RotZ(float fangle) { m_pCam->RotZ(fangle); }		
//////ī�޶� �̵�
	inline void		MoveX(float fdist) { m_pCam->MoveX(fdist); }
	inline void		MoveY(float fdist) { m_pCam->MoveY(fdist); }
	inline void		MoveZ(float fdist) { m_pCam->MoveZ(fdist); }

//////////////		ī�޶� Ÿ�� �ٲٱ�
//////////////		C_GHOST  = ���� ����
//////////////		C_FOLLOW = 3��Ī �ü�����
//////////////		C_AROUND = 3��Ī, ȸ���� �̵� ����
//////////////		C_FIXED  = �����Ƚ��� (ī�޶� Ÿ���� �ٲٰ� SetFixedCamera �Լ��� eye,lookat,up ���͸� ����)
	void			Switch_CT(CAMERA_TYPE _cType) {m_uType= _cType;};			
	void			ResetCamera(UINT _State);



//////////////		ī�޶� �Ŵ��� �Ӽ�
	D3DXMATRIXA16*	GetProjView() { return &m_matProj; }
	D3DXMATRIXA16*	GetMatView() { return &m_matView; }
	D3DXVECTOR3*    GetRayPos()  { return &m_RayPos; }
	D3DXVECTOR3*    GetCamDir()  { return &m_RayDir; }
	D3DXVECTOR3*	GetCamPos()	 { return m_pCam->GetPos(); }
	D3DXQUATERNION*	GetQuaternion() { return m_pCam->GetOrientation(); }
	CAMERA_TYPE		GetCameraType() { return m_uType; }	


	void			SetCollisionDist(float _fdist)  { m_Raydist = _fdist; }										//��ŷ���� �浹�Ÿ�����
	void			SetFixedCamera(D3DXVECTOR3 vEye , D3DXVECTOR3 vLookat, D3DXVECTOR3 vUp=D3DXVECTOR3(0,1,0));	//���������� ����
	float*			GetCoDist() { return m_pCam->GetCODistance(); }												//�������Ҵ� ī�޶�� �÷��̾��� �Ÿ����
	

private:
	O2Camera*			m_pCam;				//ī�޶� 
	O2Hero*				m_Hero;				//�÷��̾�
	O2InputManager*		m_pInput;			//��ǲ
	LPDIRECT3DDEVICE9*	m_lpDevice;			

	D3DXMATRIXA16		m_matProj;			//������Ʈ ���
	D3DXMATRIXA16		m_matView;			//�� ���
	CAMERA_TYPE			m_uType;			//ī�޶� Ÿ��


	D3DXVECTOR3			m_RayPos;			//ī�޶� ��ġ
	D3DXVECTOR3			m_RayDir;			//ī�޶� �ü�����
	D3DXVECTOR3			m_vFixedPos;		//������ ����
	D3DXVECTOR3			m_vFixedlook;		//������ �ü�
	D3DXVECTOR3			m_vFixedUp;			//������ ��
	float				m_Raydist;			//�ε��� ��ü�Ÿ�
	float*				m_fDelta;			//delta time


};

