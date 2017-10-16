#ifndef _MO2AI
#define _MO2AI

class O2AI
{
public:

	enum AI_STATE
	{
		AI_ROAMING = 0,					//	�ι�
		AI_ALERT_LOW = 1,				//	���� ��� - Search
		AI_ALERT_HIGH  = 2,				//	���� ��� - Follow
		AI_SLEEP	= 3,				//	����
		AI_ATTACK	= 4,				//	����	-	Attack
		AI_TURN = 5
	};

protected:

	VOID*		m_pGUIManager;					//	GUI Manager 
	float		m_fElapstime;					//��ŸŸ��
	float		m_fCurtime;						//�����ð�
	BOOL		m_bTargetHideState;				//�÷��̾� ��������
	AI_STATE	m_AiState;						//ai ����
	UINT		m_iRoamingState;				//�ι� ���� �ѹ�

	float		m_fAttack;						//���������
	int			m_iAttackDelay;					//���ݵ�����	(��Ÿ��)
	float		m_fAttackSpeed;					//���ݼӵ�		(���ð�)
	int			m_iTired;						//�Ƿε�	
	int			m_iTiredLevel;					//�Ƿε� ����
	float		m_fSearchDistance;				//��ġ�Ÿ�
	float		m_fRoamSpeed;					//�ιֽ��ǵ�
	float		m_fSearchSpeed;					//Ž�����ǵ�
	float		m_fFollowSpeed;					//�i�ƿ��½��ǵ�
	float		m_fAttackRange;					//���ݻ�Ÿ�
	float		m_fAttackScale;					//���÷�������
	BOOL		m_bIsUpdateDir;					//���� ������Ʈ

	D3DXVECTOR3	m_vLastTargetPos;				//���������� �߰�������
	D3DXVECTOR3 m_vSearchPos;					//Ž������
	std::vector<D3DXVECTOR3> m_vRoamingPosList;

private:




public:


	O2AI();
	virtual ~O2AI();
	void			CleanUp();

	VOID			SetGUIMgr(VOID* pGUIMgr) { m_pGUIManager = pGUIMgr; }
	virtual void	ChangeState(AI_STATE _State);
	AI_STATE		GetAiState() { return m_AiState; }
////	�ι����� �����ϱ�
	virtual void	SetRoamingSpot(D3DXVECTOR3 _Pos);

////	UFO�� �Ӽ� �ʱ�ȭ
	virtual void	SetStats(int _AttackDelay=200,
		int _TiredLevel=300,
		float _Roamspeed=1.0f,
		float _Searchspeed=0.5f,
		float _Followspeed=2.0f,
		float _SearchDist=15.0f,
		float _AttackRange=5.0f,
		float _AttackScale=1.0f,
		float _AttackSpd = 0.5f);

////	������Ʈ
	virtual void	Update(D3DXVECTOR3* _ThisPos,D3DXVECTOR3 _TargetPos,
		BOOL _TargetHideState,float _Elapstime,bool* _IsAttack,bool* _IsHit);

////	AI���º� �ൿ
	virtual void	Roaming(D3DXVECTOR3* _ThisPos);
	virtual void	SearchPlayer(D3DXVECTOR3* _ThisPos);
	virtual void	FollowPlayer(D3DXVECTOR3 _TargetPosition,D3DXVECTOR3* _ThisPos);
	virtual void	AttackPlayer(D3DXVECTOR3 _TargetPosition,D3DXVECTOR3* _ThisPos,
		bool* _IsAttack,bool* _IsHit);
//	virtual void	GetPlayer();

////	��ġȮ��
	virtual bool	CheckPosition(D3DXVECTOR3 _ThisPos,D3DXVECTOR3 _TargetPos);


};




#endif 