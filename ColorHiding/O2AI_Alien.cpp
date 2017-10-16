#include "stdafx.h"
#include "O2AI_Alien.h"

O2AI_Alien::O2AI_Alien()
:m_fRot(0)
,m_fWaveweight(0.4f)
,m_pAlien(NULL)
,m_bInSight(FALSE)
{
}

O2AI_Alien::~O2AI_Alien()
{

}

void O2AI_Alien::UpdateDirection(D3DXVECTOR3 _TargetPos)
{


//	hero - alien 방향벡터얻기
	D3DXVECTOR3 _ThisPos = m_pAlien->GetPos();
	D3DXVECTOR3 _ThisDir = m_pAlien->GetDir();
	D3DXVECTOR3 vDir = (_ThisPos-_TargetPos);	
	vDir.y=0.0f;
	D3DXVec3Normalize(&vDir,&vDir);
	D3DXVec3Normalize(&_ThisDir,&_ThisDir);

//	바라볼 벡터와 현재 alien 벡터 내적
	float _dot = D3DXVec3Dot (  &vDir,&_ThisDir );
	float rad = acos(_dot);

//	alien의 우향벡터 얻기
	D3DXVECTOR3 vRight;
	D3DXVec3Cross(&vRight,&_ThisDir,&D3DXVECTOR3(0,1,0));
//	우향벡터와 바라볼방향벡터 내적 
	if(D3DXVec3Dot(&vRight,&vDir)>=0)
	{
		m_fRot = D3DXToDegree(-rad);		//왼쪽
	}else
	{
		m_fRot = D3DXToDegree(rad);		//오른쪽
	}

//	방향삑나는거 방지 (임시)
	if(_dot>=0.9999999||_dot<=-0.9999999)
		m_fRot = 0;

	m_bIsUpdateDir=FALSE;
}

void O2AI_Alien::Update(D3DXVECTOR3 _TargetPos, BOOL _TargetHideState,
						float _Elapstime,bool* _IsAttack,bool* _IsHit )
{
	m_bTargetHideState = _TargetHideState;
	m_fElapstime = _Elapstime;


	//// 위치정보와 상태를 확인하여 Ai상태 업데이트
	if(m_AiState!=AI_ATTACK)
	{
		m_fCurtime += m_fElapstime;
		if(!m_bTargetHideState&&m_bInSight)//숨은상태가 아니고 시야에 들어왔을때
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
		Roaming();
		break;
	case AI_ALERT_LOW:
		SearchPlayer();
		break;
	case AI_ALERT_HIGH:
		FollowPlayer(_TargetPos);
		break;
	case AI_ATTACK:
		AttackPlayer(_TargetPos,_IsAttack,_IsHit);
		break;
	default:
		break;
	}

//	AI상태에 따라 바라볼 위치 설정
	D3DXVECTOR3 _Pos;
	switch ( m_AiState )
	{
	case AI_ROAMING:
		_Pos = m_vRoamingPosList[m_iRoamingState];
		break;
	case AI_ALERT_LOW:
		_Pos = m_vSearchPos;
		break;
	case AI_ALERT_HIGH:
		_Pos = _TargetPos;
		break;
	case AI_ATTACK:
		_Pos = _TargetPos;
		break;
	default:
		_Pos = D3DXVECTOR3(0,0,0);
		break;
	}

	UpdateDirection(_Pos);

}


//// 지정된 포인트로 이동한다

void O2AI_Alien::Roaming()
{

//  위치와 로밍포인트의 방향을 구함
	D3DXVECTOR3 _ThisPos = m_pAlien->GetPos();
	D3DXVECTOR3 vTemp(m_vRoamingPosList[m_iRoamingState]-_ThisPos);
//	vTemp.y=0;
	D3DXVec3Normalize(&vTemp,&vTemp);

//	UFO의 이동속도
	float mvdist=D3DXVec3Length(&(vTemp*m_fRoamSpeed*m_fElapstime));

//	UFO의 위치와 로밍포인트의 남은 거리를 계산
	D3DXVECTOR2 tempRoampos(m_vRoamingPosList[m_iRoamingState].x,m_vRoamingPosList[m_iRoamingState].z);
	D3DXVECTOR2 tempthispos(_ThisPos.x,_ThisPos.z);
	float Tdist = 	D3DXVec2Length(&(tempRoampos-tempthispos));
	int _size = m_vRoamingPosList.size();
//	해당지점에 도착할경우 로밍포인트를 다음지점으로 변경
	if(mvdist>Tdist||Tdist<0.1f)
	{
		m_bIsUpdateDir=TRUE;
		_ThisPos.x=m_vRoamingPosList[m_iRoamingState].x;
		_ThisPos.z=m_vRoamingPosList[m_iRoamingState].z;
		if(_size!=0) m_iRoamingState=(1+m_iRoamingState)%_size;
	}else
	{
		//vTemp.y+=m_fWaveweight*cos(m_fCurtime*5.0f);
		_ThisPos+=vTemp*m_fRoamSpeed*m_fElapstime;
		_ThisPos.y+=m_fWaveweight*cos(m_fCurtime*5.0f);
	}
// 	m_pAlien->SetPos(_ThisPos);
	m_pAlien->MoveBack(m_fRoamSpeed);
//	m_pAlien->MoveUp(m_fElapstime*vTemp.y);
//	m_pAlien->MoveUp(m_fWaveweight*cos(m_fCurtime*5.0f));
}


//	마지막으로 발견된 지점을 탐색한다

void O2AI_Alien::SearchPlayer()
{
	D3DXVECTOR3 _ThisPos = m_pAlien->GetPos();
	D3DXVECTOR3 vTemp(m_vSearchPos-_ThisPos);
	vTemp.y+=4.0f;
	D3DXVec3Normalize(&vTemp,&vTemp);

	float mvdist=D3DXVec3Length(&(vTemp*m_fSearchSpeed*m_fElapstime));
	D3DXVECTOR2 tempLpos(m_vSearchPos.x,m_vSearchPos.z);
	D3DXVECTOR2 tempthispos(_ThisPos.x,_ThisPos.z);
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
		//vTemp.y+=m_fWaveweight*cos(m_fCurtime*5.0f);
		_ThisPos+=vTemp*m_fSearchSpeed*m_fElapstime;
		_ThisPos.y+=m_fWaveweight*cos(m_fCurtime*5.0f);
	}
//	m_pAlien->SetPos(_ThisPos);
 	m_pAlien->MoveBack(m_fSearchSpeed);
// 	m_pAlien->MoveUp(m_fElapstime*vTemp.y);
// 	m_pAlien->MoveUp(m_fWaveweight*cos(m_fCurtime*5.0f));

}

//// 주인공 위치로 이동

void O2AI_Alien::FollowPlayer(D3DXVECTOR3 _TargetPosition)
{

	D3DXVECTOR3 _ThisPos = m_pAlien->GetPos();
	D3DXVECTOR3 vTemp((_TargetPosition)-_ThisPos);
	vTemp.y+=4.0f;
	D3DXVec3Normalize(&vTemp,&vTemp);

	float mvdist=D3DXVec3Length(&(vTemp*m_fFollowSpeed*m_fElapstime));
	D3DXVECTOR2 temptargetpos(_TargetPosition.x,_TargetPosition.z);
	D3DXVECTOR2 tempthispos(_ThisPos.x,_ThisPos.z);
	float Tdist = 	D3DXVec2Length(&(temptargetpos-tempthispos));


//	printf("ImFOllowing U\n");
	///해당지점에 도착할경우 공격상태로
	if(mvdist>Tdist||Tdist<m_fAttackRange-2.0f)
	{
		ChangeState(AI_ATTACK);
		_ThisPos.y+=m_fWaveweight*cos(m_fCurtime*5.0f);
	}else
	{
		//vTemp.y+=m_fWaveweight*cos(m_fCurtime*5.0f);
		_ThisPos+=vTemp*m_fFollowSpeed*m_fElapstime;
		_ThisPos.y+=m_fWaveweight*cos(m_fCurtime*5.0f);
	}
 //	m_pAlien->SetPos(_ThisPos);
//	m_pAlien->SetForce(0,0,0);
 	m_pAlien->MoveBack(m_fFollowSpeed);
// 	m_pAlien->MoveUp(m_fElapstime*vTemp.y);
// 	m_pAlien->MoveUp(m_fWaveweight*cos(m_fCurtime*5.0f));
}

//	플레이어 공격

void O2AI_Alien::AttackPlayer(D3DXVECTOR3 _TargetPosition,bool* _IsAttack,bool* _IsHit)
{
	D3DXVECTOR3 temptargetpos=_TargetPosition;
	temptargetpos.y=0;
	D3DXVECTOR3 tempthispos=m_pAlien->GetPos();
	tempthispos.y=0;
	D3DXVECTOR3 _ThisDir = m_pAlien->GetDir();

	float Tdist = 	D3DXVec3Length(&(temptargetpos-tempthispos));

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
		D3DXVECTOR3 _CurAttpos=tempthispos;
		float		_AttkRange = 0 ;
		float		_HeroDist = D3DXVec3Length(&(_CurAttpos-temptargetpos));
		while(_AttkRange<m_fAttackRange&&_HeroDist>m_fAttackScale)
		{
			_CurAttpos-=_ThisDir;
			_AttkRange = D3DXVec3Length(&(_CurAttpos-tempthispos));
			_HeroDist = D3DXVec3Length(&(_CurAttpos-temptargetpos));
		}
		(*_IsHit) = ( (_HeroDist<=m_fAttackScale)? TRUE : FALSE );
		m_fAttack = float(m_iAttackDelay-m_fAttackSpeed*2.0f);
		(*_IsAttack)=TRUE;
	}else if(m_fAttack<=float(m_iAttackDelay-m_fAttackSpeed*2.0f-0.55f))	//	한프레임만 보이게 - 혀 애니메이션 마지막 구간까지 ( 1.90f - 1.0f)
	{
		m_fAttack-=m_fElapstime*50.0f;
		(*_IsAttack)=FALSE;
	}else 
	{
		m_fAttack-=m_fElapstime*1.0f;
	}
}