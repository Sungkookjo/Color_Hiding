#include "stdafx.h"
#include "O2AI_UFO.h"


O2AI_UFO::O2AI_UFO()
{
}


O2AI_UFO::~O2AI_UFO()
{
}


//	���������� �߰ߵ� ������ Ž���Ѵ�

void O2AI_UFO::Roaming()
{
	//	printf("Im Roaming\n");

	//  UFO�� ��ġ�� �ι�����Ʈ�� ������ ����
	D3DXVECTOR3 _ThisPos=m_pUFO->GetPos();
	D3DXVECTOR3 vTemp(m_vRoamingPosList[m_iRoamingState]-_ThisPos);
	D3DXVec3Normalize(&vTemp,&vTemp);

	//	UFO�� �̵��ӵ�
	float mvdist=D3DXVec3Length(&(vTemp*m_fRoamSpeed*m_fElapstime));

	//	UFO�� ��ġ�� �ι�����Ʈ�� ���� �Ÿ��� ���
	D3DXVECTOR2 tempRoampos(m_vRoamingPosList[m_iRoamingState].x,m_vRoamingPosList[m_iRoamingState].z);
	D3DXVECTOR2 tempthispos(_ThisPos.x,_ThisPos.z);
	float Tdist = 	D3DXVec2Length(&(tempRoampos-tempthispos));
	int _size = m_vRoamingPosList.size();
	//	�ش������� �����Ұ�� �ι�����Ʈ�� ������������ ����
	if(mvdist>Tdist||Tdist<0.1f)
	{
		m_bIsUpdateDir=TRUE;
		_ThisPos.x=m_vRoamingPosList[m_iRoamingState].x;
		_ThisPos.z=m_vRoamingPosList[m_iRoamingState].z;
		if(_size!=0) m_iRoamingState=(1+m_iRoamingState)%_size;
	}else
	{
		_ThisPos+=vTemp*m_fRoamSpeed*m_fElapstime;
	}
	m_pUFO->SetPos(_ThisPos);
}

void O2AI_UFO::SearchPlayer()
{

	D3DXVECTOR3 _ThisPos=m_pUFO->GetPos();
	D3DXVECTOR3 vTemp(m_vSearchPos);
	vTemp.y+=60.0f;
	vTemp-=_ThisPos;
	D3DXVec3Normalize(&vTemp,&vTemp);

	float mvdist=D3DXVec3Length(&(vTemp*m_fSearchSpeed*m_fElapstime));
	D3DXVECTOR2 tempLpos(m_vSearchPos.x,m_vSearchPos.z);
	D3DXVECTOR2 tempthispos(_ThisPos.x,_ThisPos.z);
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
		_ThisPos+=vTemp*m_fSearchSpeed*m_fElapstime;
	}
	m_pUFO->SetPos(_ThisPos);

}

//// ���ΰ� ��ġ�� �̵�

void O2AI_UFO::FollowPlayer(D3DXVECTOR3 _TargetPosition)
{

	D3DXVECTOR3 _ThisPos=m_pUFO->GetPos();
	D3DXVECTOR3 vTemp(_TargetPosition);
	vTemp.y+=60.0f;
	vTemp-=_ThisPos;
	D3DXVec3Normalize(&vTemp,&vTemp);

	float mvdist=D3DXVec3Length(&(vTemp*m_fFollowSpeed*m_fElapstime));
	D3DXVECTOR2 temptargetpos(_TargetPosition.x,_TargetPosition.z);
	D3DXVECTOR2 tempthispos(_ThisPos.x,_ThisPos.z);
	float Tdist = 	D3DXVec2Length(&(temptargetpos-tempthispos));


	//	printf("ImFOllowing U\n");
	///�ش������� �����Ұ�� ���ݻ��·�
	if(mvdist>Tdist||Tdist<m_fAttackRange-4.0f)
	{
		ChangeState(AI_ATTACK);
	}else
	{
		_ThisPos+=vTemp*m_fFollowSpeed*m_fElapstime;
	}
	m_pUFO->SetPos(_ThisPos);
}

void O2AI_UFO::AttackPlayer(D3DXVECTOR3 _TargetPosition,bool* _IsAttack,bool* _IsHit)
{
	D3DXVECTOR3 temptargetpos(_TargetPosition);
	temptargetpos.y+=60;
	D3DXVECTOR3 tempthispos=m_pUFO->GetPos();

	float Tdist = 	D3DXVec3Length(&(temptargetpos-tempthispos));

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

void O2AI_UFO::Update(D3DXVECTOR3 _TargetPos,BOOL _TargetHideState,float _Elapstime,bool* _IsAttack,bool* _IsHit)
{
	if(m_AiState!=AI_SLEEP)
	{
		m_bTargetHideState = _TargetHideState;
		m_fElapstime = _Elapstime;
		m_fCurtime += m_fElapstime;


		//// ��ġ������ ���¸� Ȯ���Ͽ� Ai���� ������Ʈ
		if(m_AiState!=AI_ATTACK)
		{
			if(!m_bTargetHideState&&CheckPosition(m_pUFO->GetPos(),_TargetPos))//�������°� �ƴϸ�
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



	}
}