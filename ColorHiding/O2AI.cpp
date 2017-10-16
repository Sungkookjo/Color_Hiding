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


/////	������Ʈ �Լ�
/////	������ġ:thispos   �÷��̾� ��ġ : targetpos    �÷��̾� �������� : targethidestate
/////	�ð� : Elapstime	���ݻ��� : IsAttack

void O2AI::Update(D3DXVECTOR3* _ThisPos,D3DXVECTOR3 _TargetPos,BOOL _TargetHideState,float _Elapstime,bool* _IsAttack,bool* _IsHit)
{
	if(m_AiState!=AI_SLEEP)
	{
		m_bTargetHideState = _TargetHideState;
		m_fElapstime = _Elapstime;
		m_fCurtime += m_fElapstime;


		//// ��ġ������ ���¸� Ȯ���Ͽ� Ai���� ������Ʈ
		if(m_AiState!=AI_ATTACK)
		{
			if(!m_bTargetHideState&&CheckPosition(*_ThisPos,_TargetPos))//�������°� �ƴϸ�
			{
				m_iTired = 0;
				ChangeState(AI_ALERT_HIGH);
				m_vSearchPos=m_vLastTargetPos=_TargetPos;
			}else if(m_AiState==AI_ALERT_HIGH)//���� �����̸�
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

		//// Ai���·� �ൿ�� ����

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



//// ������ ����Ʈ�� �̵��Ѵ�

void O2AI::Roaming( D3DXVECTOR3* _ThisPos)
{
//	printf("Im Roaming\n");

//  UFO�� ��ġ�� �ι�����Ʈ�� ������ ����
	D3DXVECTOR3 vTemp(m_vRoamingPosList[m_iRoamingState]-(*_ThisPos));
	D3DXVec3Normalize(&vTemp,&vTemp);

//	UFO�� �̵��ӵ�
	float mvdist=D3DXVec3Length(&(vTemp*m_fRoamSpeed*m_fElapstime));

//	UFO�� ��ġ�� �ι�����Ʈ�� ���� �Ÿ��� ���
	D3DXVECTOR2 tempRoampos(m_vRoamingPosList[m_iRoamingState].x,m_vRoamingPosList[m_iRoamingState].z);
	D3DXVECTOR2 tempthispos(_ThisPos->x,_ThisPos->z);
	float Tdist = 	D3DXVec2Length(&(tempRoampos-tempthispos));
	int _size = m_vRoamingPosList.size();
//	�ش������� �����Ұ�� �ι�����Ʈ�� ������������ ����
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


//	���������� �߰ߵ� ������ Ž���Ѵ�

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

//	�ش������� �����Ұ�� �� ������ Ž��
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

//// ���ΰ� ��ġ�� �̵�

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
	///�ش������� �����Ұ�� ���ݻ��·�
	if(mvdist>Tdist||Tdist<m_fAttackRange)
	{
		ChangeState(AI_ATTACK);
	}else
	{
		(*_ThisPos)+=vTemp*m_fFollowSpeed*m_fElapstime;
	}
}

//	�÷��̾� ����

void O2AI::AttackPlayer(D3DXVECTOR3 _TargetPosition,D3DXVECTOR3* _ThisPos,bool* _IsAttack,bool* _IsHit)
{
	D3DXVECTOR2 temptargetpos(_TargetPosition.x,_TargetPosition.z);
	D3DXVECTOR2 tempthispos(_ThisPos->x,_ThisPos->z);

	float Tdist = 	D3DXVec2Length(&(temptargetpos-tempthispos));

//	08041210
//	IsHit �߰� �� ������ ������
	if(m_fAttack<=0 && Tdist>=m_fAttackRange)	//	���� ��Ÿ���� ���ƿ� AND �����ǿ� ���� ��� ���������·�(Follow)
	{				
		ChangeState(AI_ALERT_HIGH);
	}else if(m_bTargetHideState)	//	ĳ���Ͱ� ������ ���� �����·�(Search)
	{
		ChangeState(AI_ALERT_LOW);
		(*_IsAttack)=FALSE;
		(*_IsHit) =FALSE;
	}
	else if(m_fAttack<=0)	//	(���� ��Ÿ������ ���ƿ� AND ĳ���͸� �� AND �����ǳ��� ����) ���ݸ���
	{
		m_fAttack = float(m_iAttackDelay);
	}	
	else if(m_fAttack<=float(m_iAttackDelay-m_fAttackSpeed)&&m_fAttack>float(m_iAttackDelay-m_fAttackSpeed*2.0f))	//	���ݽ� Tdist������ �¾Ҵ��� ���������� Ȯ��
	{
		printf("DIE\n");
		(*_IsHit) = ( (Tdist<=m_fAttackScale)? TRUE : FALSE );
		m_fAttack = float(m_iAttackDelay-m_fAttackSpeed*2.0f);
		(*_IsAttack)=TRUE;
	}else if(m_fAttack<=float(m_iAttackDelay-m_fAttackSpeed*2.0f-0.8f))	//	�������Ӹ� ���̰� - �� �ִϸ��̼� ������ �������� ( 1.90f - 1.0f)
	{
		m_fAttack-=m_fElapstime*50.0f;
		(*_IsAttack)=FALSE;
	}else 
	{
		m_fAttack-=m_fElapstime*1.0f;
	}
}

//	���ΰ��� �ڽ��� ��ġ�� ����ϰ� ��ȯ

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