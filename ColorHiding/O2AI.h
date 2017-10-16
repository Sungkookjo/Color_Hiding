#ifndef _MO2AI
#define _MO2AI

class O2AI
{
public:

	enum AI_STATE
	{
		AI_ROAMING = 0,					//	로밍
		AI_ALERT_LOW = 1,				//	낮은 경계 - Search
		AI_ALERT_HIGH  = 2,				//	높은 경계 - Follow
		AI_SLEEP	= 3,				//	정지
		AI_ATTACK	= 4,				//	공격	-	Attack
		AI_TURN = 5
	};

protected:

	VOID*		m_pGUIManager;					//	GUI Manager 
	float		m_fElapstime;					//델타타임
	float		m_fCurtime;						//지난시간
	BOOL		m_bTargetHideState;				//플레이어 숨은상태
	AI_STATE	m_AiState;						//ai 상태
	UINT		m_iRoamingState;				//로밍 지점 넘버

	float		m_fAttack;						//공격제어변수
	int			m_iAttackDelay;					//공격딜레이	(쿨타임)
	float		m_fAttackSpeed;					//공격속도		(대기시간)
	int			m_iTired;						//피로도	
	int			m_iTiredLevel;					//피로도 레벨
	float		m_fSearchDistance;				//서치거리
	float		m_fRoamSpeed;					//로밍스피드
	float		m_fSearchSpeed;					//탐색스피드
	float		m_fFollowSpeed;					//쫒아오는스피드
	float		m_fAttackRange;					//공격사거리
	float		m_fAttackScale;					//스플래쉬범위
	BOOL		m_bIsUpdateDir;					//방향 업데이트

	D3DXVECTOR3	m_vLastTargetPos;				//마지막으로 발견한지점
	D3DXVECTOR3 m_vSearchPos;					//탐색지점
	std::vector<D3DXVECTOR3> m_vRoamingPosList;

private:




public:


	O2AI();
	virtual ~O2AI();
	void			CleanUp();

	VOID			SetGUIMgr(VOID* pGUIMgr) { m_pGUIManager = pGUIMgr; }
	virtual void	ChangeState(AI_STATE _State);
	AI_STATE		GetAiState() { return m_AiState; }
////	로밍지점 지정하기
	virtual void	SetRoamingSpot(D3DXVECTOR3 _Pos);

////	UFO의 속성 초기화
	virtual void	SetStats(int _AttackDelay=200,
		int _TiredLevel=300,
		float _Roamspeed=1.0f,
		float _Searchspeed=0.5f,
		float _Followspeed=2.0f,
		float _SearchDist=15.0f,
		float _AttackRange=5.0f,
		float _AttackScale=1.0f,
		float _AttackSpd = 0.5f);

////	업데이트
	virtual void	Update(D3DXVECTOR3* _ThisPos,D3DXVECTOR3 _TargetPos,
		BOOL _TargetHideState,float _Elapstime,bool* _IsAttack,bool* _IsHit);

////	AI상태별 행동
	virtual void	Roaming(D3DXVECTOR3* _ThisPos);
	virtual void	SearchPlayer(D3DXVECTOR3* _ThisPos);
	virtual void	FollowPlayer(D3DXVECTOR3 _TargetPosition,D3DXVECTOR3* _ThisPos);
	virtual void	AttackPlayer(D3DXVECTOR3 _TargetPosition,D3DXVECTOR3* _ThisPos,
		bool* _IsAttack,bool* _IsHit);
//	virtual void	GetPlayer();

////	위치확인
	virtual bool	CheckPosition(D3DXVECTOR3 _ThisPos,D3DXVECTOR3 _TargetPos);


};




#endif 