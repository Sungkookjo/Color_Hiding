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

	void				Update();					//카메라 업데이트  - 메인에서
	void				Update(O2Hero* _hero);		//카메라 업데이트  - 게임 실행

	void				Zoom(float fweight);	//카메라 줌-인,아웃
	void				Input();				//카메라 키입력

	D3DXMATRIXA16		DeadScene1();


//////카메라 회전
	inline void		RotX(float fangle) { m_pCam->RotX(fangle); }			
	inline void		RotY(float fangle) { m_pCam->RotY(fangle); }			
	inline void		RotZ(float fangle) { m_pCam->RotZ(fangle); }		
//////카메라 이동
	inline void		MoveX(float fdist) { m_pCam->MoveX(fdist); }
	inline void		MoveY(float fdist) { m_pCam->MoveY(fdist); }
	inline void		MoveZ(float fdist) { m_pCam->MoveZ(fdist); }

//////////////		카메라 타입 바꾸기
//////////////		C_GHOST  = 자유 시점
//////////////		C_FOLLOW = 3인칭 시선고정
//////////////		C_AROUND = 3인칭, 회전및 이동 가능
//////////////		C_FIXED  = 고정된시점 (카메라 타입을 바꾸고 SetFixedCamera 함수에 eye,lookat,up 벡터를 설정)
	void			Switch_CT(CAMERA_TYPE _cType) {m_uType= _cType;};			
	void			ResetCamera(UINT _State);



//////////////		카메라 매니저 속성
	D3DXMATRIXA16*	GetProjView() { return &m_matProj; }
	D3DXMATRIXA16*	GetMatView() { return &m_matView; }
	D3DXVECTOR3*    GetRayPos()  { return &m_RayPos; }
	D3DXVECTOR3*    GetCamDir()  { return &m_RayDir; }
	D3DXVECTOR3*	GetCamPos()	 { return m_pCam->GetPos(); }
	D3DXQUATERNION*	GetQuaternion() { return m_pCam->GetOrientation(); }
	CAMERA_TYPE		GetCameraType() { return m_uType; }	


	void			SetCollisionDist(float _fdist)  { m_Raydist = _fdist; }										//픽킹사용시 충돌거리세팅
	void			SetFixedCamera(D3DXVECTOR3 vEye , D3DXVECTOR3 vLookat, D3DXVECTOR3 vUp=D3DXVECTOR3(0,1,0));	//고정된지점 세팅
	float*			GetCoDist() { return m_pCam->GetCODistance(); }												//광선을소는 카메라와 플레이어의 거리얻기
	

private:
	O2Camera*			m_pCam;				//카메라 
	O2Hero*				m_Hero;				//플레이어
	O2InputManager*		m_pInput;			//인풋
	LPDIRECT3DDEVICE9*	m_lpDevice;			

	D3DXMATRIXA16		m_matProj;			//프로젝트 행렬
	D3DXMATRIXA16		m_matView;			//뷰 행렬
	CAMERA_TYPE			m_uType;			//카메라 타입


	D3DXVECTOR3			m_RayPos;			//카메라 위치
	D3DXVECTOR3			m_RayDir;			//카메라 시선방향
	D3DXVECTOR3			m_vFixedPos;		//고정된 지점
	D3DXVECTOR3			m_vFixedlook;		//고정된 시선
	D3DXVECTOR3			m_vFixedUp;			//고정된 업
	float				m_Raydist;			//부딪힌 물체거리
	float*				m_fDelta;			//delta time


};

