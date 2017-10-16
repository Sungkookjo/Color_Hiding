#ifndef _MCAMERA
#define _MCAMERA
//////////////////////////////////////////////////////////////////////////
// Camera
// by. �� �� ��

// �ӽ� define ��ũ��
#define CAM_MOVE_SPEED 100.0f
#define CAM_WEIGHT		0.01f

class O2Camera
{
public:


	O2Camera();
	~O2Camera();

//	ī�޶� ����� ��� �� ī�޶� �Ӽ� ������Ʈ
//	�� : GHOST			�� : AROUND , FOLLOW
	D3DXMATRIXA16*	GetViewMatrix();			
	D3DXMATRIXA16*	GetViewMatrix(D3DXVECTOR3* _pos,float _y ,float _Raydist,
		bool _Camerastate=FALSE);

//	ī�޶� ���� : �÷��̾� ���ʹϿ� , ������ �Ÿ�, ī�޶�Ÿ�� �� ����
	void			SetCamera(D3DXQUATERNION* _target_quat,float _Raydist,
		bool _state=FALSE,bool _Slerpstate=FALSE);			

//	���ʹϿ����κ��� ���ϴ� ���⺤�;��
	D3DXVECTOR3*	TransformAxis(D3DXQUATERNION* _ori, D3DXVECTOR3* _axis);


	bool			Slerp(D3DXQUATERNION* quatTarget);					//���ʹϿ� ����
	bool			SlerpCollsion(float _Raydist);						//�浹 ����

	void			Zoom(float fweight,D3DXVECTOR3 _look=D3DXVECTOR3(0,0,0));	//��-��,�ƿ�


	void			SetViewMatrix(D3DXMATRIXA16* _view)		{ m_matView=(*_view); }	//ī�޶� ��ļ���

	D3DXVECTOR3*	GetPos()								{ return &m_vPos; }		//ī�޶� ��ġ���
	void			SetPos(D3DXVECTOR3* _pos)				{ m_vPos=(*_pos); }		//ī�޶� ��ġ����

	D3DXVECTOR3*	GetLPos()								{ return &m_vLPos; }		//�浹��� �� ��ġ
	void			SetLPos(D3DXVECTOR3* _pos)				{ m_vLPos=(*_pos); }		//

	D3DXQUATERNION*	GetOrientation()						{ return &m_qQuatout; }	//ī�޶� ���� ���
	void			SetOrientation(D3DXQUATERNION _ori)		{ m_qOrign=_ori; }	//ī�޶� ���� ����


	float			GetMoveSpeed()						{ return m_fMoveSpeed; }
	void			SetMoveSpeed(float _moveSpeed)		{ m_fMoveSpeed=_moveSpeed; }		//ī�޶� �̵��ӵ�
	
	float			GetRotateSpeed()					{ return m_fRotSpeed; }
	void			SetRotateSpeed(float _rotateSpeed)	{ m_fRotSpeed=_rotateSpeed; }	//ī�޶� ȸ���ӵ�
	
	float			GetSlerpSpeed()						{ return m_fSlerp; }
	void			SetSlerpSpeed(float _slerpSpeed)	{ m_fSlerp=_slerpSpeed; }		//���� �ӵ�

	void			RotX(float fangle);								//X�� ȸ��,right
	void			RotY(float fangle);								//Y�� ȸ��,up
	void			RotZ(float fangle);								//Z�� ȸ��,look


	void			MoveX(float dist);								//X�� �̵�
	void			MoveY(float dist);								//Y�� �̵�
	void			MoveZ(float dist);								//Z�� �̵�


	void			SetDistance(float _dist) { m_fDistance = _dist; }		//�Ÿ� ������ ����
	void			SetRotX(float _rotx) { m_fRotX = _rotx; }
	float*			GetCODistance() { return &m_fCODistance; }				//������������ ī�޶�� �÷��̾ �Ÿ����

//	�Ⱦ���
	void			SetLookAt(D3DXVECTOR3 to, D3DXVECTOR3 from, D3DXVECTOR3 up);







protected:
private:
	D3DXVECTOR3		m_vPos;			//ī�޶� ��ġ
	D3DXVECTOR3		m_vLPos;		//�浹ó��X ��ġ
	D3DXQUATERNION	m_qOrign;			//ī�޶� ȸ���� Origin(�÷��̾� ������ ��,����ī�޶�)
	D3DXQUATERNION  m_qQuatout;			//ī�޶� ȸ���� 
	D3DXMATRIXA16	m_matView;			//ī�޶� ���

	float			m_fMoveSpeed;		// ī�޶� �̵� �ӵ�
	float			m_fRotSpeed;		// ī�޶� ȸ�� �ӵ�
	float			m_fSlerp;			// ���� (0�� 1������ �Ǽ������� �� ����� ���̿��� ��������� ������ ���� ����)

	float			m_fDistance;			//�÷��̾�-ī�޶� �Ÿ�������
	float			m_fDistminLimit;		//�÷��̾�-ī�޶� �Ÿ� �ּ� ���Ѱ�
	float			m_fDistmaxLimit;		//�÷��̾�-ī�޶� �Ÿ� �ִ� ���Ѱ�
	float			m_fCODistance;			//�÷��̾�-ī�޶�0 �Ÿ� ( ������ ��� ī�޶� )
	float			m_fCODistance2;			//�÷��̾�-ī�޶�1 �Ÿ�	( ����ڰ������ִ� ī�޶� )
	float			m_fLRaydist;			//��ü-ī�޶� ���� �����Ǵ� ������
	float			m_fRotX;			//X�� ������
};

#endif 