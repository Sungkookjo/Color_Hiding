#ifndef _MCAMERA
#define _MCAMERA
//////////////////////////////////////////////////////////////////////////
// Camera
// by. 이 동 준

// 임시 define 매크로
#define CAM_MOVE_SPEED 100.0f
#define CAM_WEIGHT		0.01f

class O2Camera
{
public:


	O2Camera();
	~O2Camera();

//	카메라 뷰행렬 얻기 및 카메라 속성 업데이트
//	상 : GHOST			하 : AROUND , FOLLOW
	D3DXMATRIXA16*	GetViewMatrix();			
	D3DXMATRIXA16*	GetViewMatrix(D3DXVECTOR3* _pos,float _y ,float _Raydist,
		bool _Camerastate=FALSE);

//	카메라 세팅 : 플레이어 쿼터니온 , 광선의 거리, 카메라타입 및 보간
	void			SetCamera(D3DXQUATERNION* _target_quat,float _Raydist,
		bool _state=FALSE,bool _Slerpstate=FALSE);			

//	쿼터니온으로부터 원하는 방향벡터얻기
	D3DXVECTOR3*	TransformAxis(D3DXQUATERNION* _ori, D3DXVECTOR3* _axis);


	bool			Slerp(D3DXQUATERNION* quatTarget);					//쿼터니온 보간
	bool			SlerpCollsion(float _Raydist);						//충돌 보간

	void			Zoom(float fweight,D3DXVECTOR3 _look=D3DXVECTOR3(0,0,0));	//줌-인,아웃


	void			SetViewMatrix(D3DXMATRIXA16* _view)		{ m_matView=(*_view); }	//카메라 행렬세팅

	D3DXVECTOR3*	GetPos()								{ return &m_vPos; }		//카메라 위치얻기
	void			SetPos(D3DXVECTOR3* _pos)				{ m_vPos=(*_pos); }		//카메라 위치세팅

	D3DXVECTOR3*	GetLPos()								{ return &m_vLPos; }		//충돌계산 전 위치
	void			SetLPos(D3DXVECTOR3* _pos)				{ m_vLPos=(*_pos); }		//

	D3DXQUATERNION*	GetOrientation()						{ return &m_qQuatout; }	//카메라 방향 얻기
	void			SetOrientation(D3DXQUATERNION _ori)		{ m_qOrign=_ori; }	//카메라 방향 세팅


	float			GetMoveSpeed()						{ return m_fMoveSpeed; }
	void			SetMoveSpeed(float _moveSpeed)		{ m_fMoveSpeed=_moveSpeed; }		//카메라 이동속도
	
	float			GetRotateSpeed()					{ return m_fRotSpeed; }
	void			SetRotateSpeed(float _rotateSpeed)	{ m_fRotSpeed=_rotateSpeed; }	//카메라 회전속도
	
	float			GetSlerpSpeed()						{ return m_fSlerp; }
	void			SetSlerpSpeed(float _slerpSpeed)	{ m_fSlerp=_slerpSpeed; }		//보간 속도

	void			RotX(float fangle);								//X축 회전,right
	void			RotY(float fangle);								//Y축 회전,up
	void			RotZ(float fangle);								//Z축 회전,look


	void			MoveX(float dist);								//X축 이동
	void			MoveY(float dist);								//Y축 이동
	void			MoveZ(float dist);								//Z축 이동


	void			SetDistance(float _dist) { m_fDistance = _dist; }		//거리 보정값 세팅
	void			SetRotX(float _rotx) { m_fRotX = _rotx; }
	float*			GetCODistance() { return &m_fCODistance; }				//광선을보내는 카메라와 플레이어간 거리얻기

//	안쓰임
	void			SetLookAt(D3DXVECTOR3 to, D3DXVECTOR3 from, D3DXVECTOR3 up);







protected:
private:
	D3DXVECTOR3		m_vPos;			//카메라 위치
	D3DXVECTOR3		m_vLPos;		//충돌처리X 위치
	D3DXQUATERNION	m_qOrign;			//카메라 회전값 Origin(플레이어 얻어오는 값,본래카메라값)
	D3DXQUATERNION  m_qQuatout;			//카메라 회전값 
	D3DXMATRIXA16	m_matView;			//카메라 행렬

	float			m_fMoveSpeed;		// 카메라 이동 속도
	float			m_fRotSpeed;		// 카메라 회전 속도
	float			m_fSlerp;			// 보간 (0과 1사이의 실수값으로 두 사원수 사이에서 어느쪽으로 가깝게 할지 결정)

	float			m_fDistance;			//플레이어-카메라 거리보정값
	float			m_fDistminLimit;		//플레이어-카메라 거리 최소 제한값
	float			m_fDistmaxLimit;		//플레이어-카메라 거리 최대 제한값
	float			m_fCODistance;			//플레이어-카메라0 거리 ( 광선을 쏘는 카메라 )
	float			m_fCODistance2;			//플레이어-카메라1 거리	( 사용자가보고있는 카메라 )
	float			m_fLRaydist;			//물체-카메라 간의 보정되는 광선값
	float			m_fRotX;			//X축 각도값
};

#endif 