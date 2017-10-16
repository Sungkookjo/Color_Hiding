#include "stdafx.h"
#include "O2GUIManager.h"
O2AI::O2AI():
m_fAttack(0),
m_iTired(0),
m_iAttackDelay(200),
m_iTiredLevel(500),
m_AiState(AI_ROAMING),
m_fElapstime(0.001f),
m_fSearchDistance(15.0f),
m_bTargetHideState(TRUE),
m_vLastTargetPos(0,0,0),
m_vSearchPos(0,0,0),
m_iRoamingState(0),
m_fRoamSpeed(0.1f),
m_fSearchSpeed(0.1f),
m_fFollowSpeed(0.1f),
m_fAttackRange(5.0f),
m_fAttackScale(1.0f),
m_fAttackSpeed(0.5f),
m_fCurtime(0.0f),
m_bIsUpdateDir(FALSE),
m_pGUIManager(NULL)
{
}



O2AI::~O2AI()
{
	CleanUp();
	m_pGUIManager=NULL;
}

void O2AI::CleanUp()
{
	m_vRoamingPosList.clear();
}


/////	업데이트 함수
/////	적의위치:thispos   플레이어 위치 : targetpos    플레이어 숨은상태 : targethidestate
/////	시간 : Elapstime	공격상태 : IsAttack

void O2AI::Update(D3DXVECTOR3* _ThisPos,D3DXVECTOR3 _TargetPos,BOOL _TargetHideState,float _Elapstime,bool* _IsAttack,bool* _IsHit)
{
	if(m_AiState!=AI_SLEEP)
	{
		m_bTargetHideState = _TargetHideState;
		m_fElapstime = _Elapstime;
		m_fCurtime += m_fElapstime;


		//// 위치정보와 상태를 확인하여 Ai상태 업데이트
		if(m_AiState!=AI_ATTACK)
		{
			if(!m_bTargetHideState&&CheckPosition(*_ThisPos,_TargetPos))//숨은상태가 아니면
			{
				m_iTired = 0;
				ChangeState(AI_ALERT_HIGH);
				m_vSearchPos=m_vLastTargetPos=_TargetPos;
			}else if(m_AiState==AI_ALERT_HIGH)//숨은 상태이면
			{
				ChangeState(AI_ALERT_LOW);
			}else if(m_iTired>=m_iTiredLevel)
			{
				m_iTired = m_iTiredLevel;
				ChangeState(AI_ROAMING);
			}else
			{
				m_iTired++;
			}

		}

		//// Ai상태로 행동을 지정

		switch ( m_AiState )
		{
		case AI_ROAMING:
			Roaming(_ThisPos);
			break;
		case AI_ALERT_LOW:
			SearchPlayer(_ThisPos);
			break;
		case AI_ALERT_HIGH:
			FollowPlayer(_TargetPos,_ThisPos);
			break;
		case AI_ATTACK:
			AttackPlayer(_TargetPos,_ThisPos,_IsAttack,_IsHit);
			break;
		default:
			break;
		}



	}
}



//// 지정된 포인트로 이동한다

void O2AI::Roaming( D3DXVECTOR3* _ThisPos)
{
//	printf("Im Roaming\n");

//  UFO의 위치와 로밍포인트의 방향을 구함
	D3DXVECTOR3 vTemp(m_vRoamingPosList[m_iRoamingState]-(*_ThisPos));
	D3DXVec3Normalize(&vTemp,&vTemp);

//	UFO의 이동속도
	float mvdist=D3DXVec3Length(&(vTemp*m_fRoamSpeed*m_fElapstime));

//	UFO의 위치와 로밍포인트의 남은 거리를 계산
	D3DXVECTOR2 tempRoampos(m_vRoamingPosList[m_iRoamingState].x,m_vRoamingPosList[m_iRoamingState].z);
	D3DXVECTOR2 tempthispos(_ThisPos->x,_ThisPos->z);
	float Tdist = 	D3DXVec2Length(&(tempRoampos-tempthispos));
	int _size = m_vRoamingPosList.size();
//	해당지점에 도착할경우 로밍포인트를 다음지점으로 변경
	if(mvdist>Tdist||Tdist<0.1f)
	{
		m_bIsUpdateDir=TRUE;
		_ThisPos->x=m_vRoamingPosList[m_iRoamingState].x;
		_ThisPos->z=m_vRoamingPosList[m_iRoamingState].z;
		if(_size!=0) m_iRoamingState=(1+m_iRoamingState)%_size;
	}else
	{
		(*_ThisPos)+=vTemp*m_fRoamSpeed*m_fElapstime;
	}
}


//	마지막으로 발견된 지점을 탐색한다

void O2AI::SearchPlayer(D3DXVECTOR3* _ThisPos)
{

	D3DXVECTOR3 vTemp(m_vSearchPos-(*_ThisPos));
	vTemp.y=0;
	D3DXVec3Normalize(&vTemp,&vTemp);

	float mvdist=D3DXVec3Length(&(vTemp*m_fSearchSpeed*m_fElapstime));
	D3DXVECTOR2 tempLpos(m_vSearchPos.x,m_vSearchPos.z);
	D3DXVECTOR2 tempthispos(_ThisPos->x,_ThisPos->z);
	float Tdist = 	D3DXVec2Length(&(tempLpos-tempthispos));

//	printf("Im Searching U\n");

//	해당지점에 도착할경우 그 주위를 탐색
	if(mvdist>Tdist||Tdist<0.1f)
	{
		float fRandNum = (float)rand () / RAND_MAX;
		fRandNum = -5.0f + (5.0f - (-5.0f)) * fRandNum;
		m_vSearchPos.x=m_vLastTargetPos.x+fRandNum;
		printf("X : %f\t",fRandNum);
		fRandNum = (float)rand () / RAND_MAX;
		fRandNum = -5.0f + (5.0f - (-5.0f)) * fRandNum;
		m_vSearchPos.z=m_vLastTargetPos.z+fRandNum;
		printf("Z : %f\n",fRandNum);
	}else
	{
		(*_ThisPos)+=vTemp*m_fSearchSpeed*m_fElapstime;
	}

}

//// 주인공 위치로 이동

void O2AI::FollowPlayer(D3DXVECTOR3 _TargetPosition,D3DXVECTOR3* _ThisPos)
{

	D3DXVECTOR3 vTemp((_TargetPosition)-(*_ThisPos));
	vTemp.y=0;
	D3DXVec3Normalize(&vTemp,&vTemp);

	float mvdist=D3DXVec3Length(&(vTemp*m_fFollowSpeed*m_fElapstime));
	D3DXVECTOR2 temptargetpos(_TargetPosition.x,_TargetPosition.z);
	D3DXVECTOR2 tempthispos(_ThisPos->x,_ThisPos->z);
	float Tdist = 	D3DXVec2Length(&(temptargetpos-tempthispos));


//	printf("ImFOllowing U\n");
	///해당지점에 도착할경우 공격상태로
	if(mvdist>Tdist||Tdist<m_fAttackRange)
	{
		ChangeState(AI_ATTACK);
	}else
	{
		(*_ThisPos)+=vTemp*m_fFollowSpeed*m_fElapstime;
	}
}

//	플레이어 공격

void O2AI::AttackPlayer(D3DXVECTOR3 _TargetPosition,D3DXVECTOR3* _ThisPos,bool* _IsAttack,bool* _IsHit)
{
	D3DXVECTOR2 temptargetpos(_TargetPosition.x,_TargetPosition.z);
	D3DXVECTOR2 tempthispos(_ThisPos->x,_ThisPos->z);

	float Tdist = 	D3DXVec2Length(&(temptargetpos-tempthispos));

//	08041210
//	IsHit 추가 및 공격전 딜레이
	if(m_fAttack<=0 && Tdist>=m_fAttackRange)	//	공격 쿨타임이 돌아옴 AND 사정권에 없을 경우 높은경계상태로(Follow)
	{				
		ChangeState(AI_ALERT_HIGH);
	}else if(m_bTargetHideState)	//	캐릭터가 숨으면 낮은 경계상태로(Search)
	{
		ChangeState(AI_ALERT_LOW);
		(*_IsAttack)=FALSE;
		(*_IsHit) =FALSE;
	}
	else if(m_fAttack<=0)	//	(공격 쿨타임으로 돌아옴 AND 캐릭터를 봄 AND 사정권내에 있음) 공격모드로
	{
		m_fAttack = float(m_iAttackDelay);
	}	
	else if(m_fAttack<=float(m_iAttackDelay-m_fAttackSpeed)&&m_fAttack>float(m_iAttackDelay-m_fAttackSpeed*2.0f))	//	공격시 Tdist에따라 맞았는지 빗나갔는지 확인
	{
		printf("DIE\n");
		(*_IsHit) = ( (Tdist<=m_fAttackScale)? TRUE : FALSE );
		m_fAttack = float(m_iAttackDelay-m_fAttackSpeed*2.0f);
		(*_IsAttack)=TRUE;
	}else if(m_fAttack<=float(m_iAttackDelay-m_fAttackSpeed*2.0f-0.8f))	//	한프레임만 보이게 - 혀 애니메이션 마지막 구간까지 ( 1.90f - 1.0f)
	{
		m_fAttack-=m_fElapstime*50.0f;
		(*_IsAttack)=FALSE;
	}else 
	{
		m_fAttack-=m_fElapstime*1.0f;
	}
}

//	주인공과 자신의 위치를 계산하고 반환

bool O2AI::CheckPosition(D3DXVECTOR3 _ThisPos,D3DXVECTOR3 _TargetPos)
{
	D3DXVECTOR3 vTemp((_TargetPos)-(_ThisPos));
	vTemp.y=0;

	return ( (D3DXVec3Length(&(vTemp))<=m_fSearchDistance)? TRUE : FALSE );
}


void O2AI::ChangeState( AI_STATE _State )
{
	UINT m_Aitemp;
	if(m_AiState != _State){
		m_Aitemp = m_AiState;
		m_AiState = _State;
		switch(m_AiState)
		{
		case AI_ALERT_HIGH:
			{
				((O2GUIManager*)(m_pGUIManager))->SetOnWarnning();
				if(m_Aitemp!=AI_ATTACK)
					O2SOUNDINSTANCE->Play2DEffect(_T("Warning"),O2SoundManager::S_TYPE_INGAME);
			}
			break;
		default:
		    break;
		}
	}
}

void O2AI::SetRoamingSpot(D3DXVECTOR3 _Pos)
{
	D3DXVECTOR3 temp=_Pos;
	m_vRoamingPosList.push_back(temp);
}

void O2AI::SetStats(int _AttackDelay /*= 200*/, int _TiredLevel/* = 300*/, 
					float _Roamspeed /*= 1.000000*/, float _Searchspeed /*= 0.500000*/,
					float _Followspeed /*= 2.000000*/, float _SearchDist /*= 15.000000*/,
					float _AttackRange/* = 5.000000*/, float _AttackScale /*= 1.000000*/,
					float _AttackSpd/* = 0.5f*/)
{
	m_iAttackDelay=(_AttackDelay);
	m_iTiredLevel=(_TiredLevel);
	m_fRoamSpeed=(_Roamspeed);
	m_fSearchSpeed=(_Searchspeed);
	m_fFollowSpeed=(_Followspeed);
	m_fSearchDistance=(_SearchDist);
	m_fAttackRange=(_AttackRange);
	m_fAttackScale=(_AttackScale);
	m_fAttackSpeed=(_AttackSpd);

}