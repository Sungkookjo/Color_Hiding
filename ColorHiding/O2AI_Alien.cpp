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


//	hero - alien ���⺤�;��
	D3DXVECTOR3 _ThisPos = m_pAlien->GetPos();
	D3DXVECTOR3 _ThisDir = m_pAlien->GetDir();
	D3DXVECTOR3 vDir = (_ThisPos-_TargetPos);	
	vDir.y=0.0f;
	D3DXVec3Normalize(&vDir,&vDir);
	D3DXVec3Normalize(&_ThisDir,&_ThisDir);

//	�ٶ� ���Ϳ� ���� alien ���� ����
	float _dot = D3DXVec3Dot (  &vDir,&_ThisDir );
	float rad = acos(_dot);

//	alien�� ���⺤�� ���
	D3DXVECTOR3 vRight;
	D3DXVec3Cross(&vRight,&_ThisDir,&D3DXVECTOR3(0,1,0));
//	���⺤�Ϳ� �ٶ󺼹��⺤�� ���� 
	if(D3DXVec3Dot(&vRight,&vDir)>=0)
	{
		m_fRot = D3DXToDegree(-rad);		//����
	}else
	{
		m_fRot = D3DXToDegree(rad);		//������
	}

//	����೪�°� ���� (�ӽ�)
	if(_dot>=0.9999999||_dot<=-0.9999999)
		m_fRot = 0;

	m_bIsUpdateDir=FALSE;
}

void O2AI_Alien::Update(D3DXVECTOR3 _TargetPos, BOOL _TargetHideState,
						float _Elapstime,bool* _IsAttack,bool* _IsHit )
{
	m_bTargetHideState = _TargetHideState;
	m_fElapstime = _Elapstime;


	//// ��ġ������ ���¸� Ȯ���Ͽ� Ai���� ������Ʈ
	if(m_AiState!=AI_ATTACK)
	{
		m_fCurtime += m_fElapstime;
		if(!m_bTargetHideState&&m_bInSight)//�������°� �ƴϰ� �þ߿� ��������
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

//	AI���¿� ���� �ٶ� ��ġ ����
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


//// ������ ����Ʈ�� �̵��Ѵ�

void O2AI_Alien::Roaming()
{

//  ��ġ�� �ι�����Ʈ�� ������ ����
	D3DXVECTOR3 _ThisPos = m_pAlien->GetPos();
	D3DXVECTOR3 vTemp(m_vRoamingPosList[m_iRoamingState]-_ThisPos);
//	vTemp.y=0;
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
		//vTemp.y+=m_fWaveweight*cos(m_fCurtime*5.0f);
		_ThisPos+=vTemp*m_fRoamSpeed*m_fElapstime;
		_ThisPos.y+=m_fWaveweight*cos(m_fCurtime*5.0f);
	}
// 	m_pAlien->SetPos(_ThisPos);
	m_pAlien->MoveBack(m_fRoamSpeed);
//	m_pAlien->MoveUp(m_fElapstime*vTemp.y);
//	m_pAlien->MoveUp(m_fWaveweight*cos(m_fCurtime*5.0f));
}


//	���������� �߰ߵ� ������ Ž���Ѵ�

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
		//vTemp.y+=m_fWaveweight*cos(m_fCurtime*5.0f);
		_ThisPos+=vTemp*m_fSearchSpeed*m_fElapstime;
		_ThisPos.y+=m_fWaveweight*cos(m_fCurtime*5.0f);
	}
//	m_pAlien->SetPos(_ThisPos);
 	m_pAlien->MoveBack(m_fSearchSpeed);
// 	m_pAlien->MoveUp(m_fElapstime*vTemp.y);
// 	m_pAlien->MoveUp(m_fWaveweight*cos(m_fCurtime*5.0f));

}

//// ���ΰ� ��ġ�� �̵�

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
	///�ش������� �����Ұ�� ���ݻ��·�
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

//	�÷��̾� ����

void O2AI_Alien::AttackPlayer(D3DXVECTOR3 _TargetPosition,bool* _IsAttack,bool* _IsHit)
{
	D3DXVECTOR3 temptargetpos=_TargetPosition;
	temptargetpos.y=0;
	D3DXVECTOR3 tempthispos=m_pAlien->GetPos();
	tempthispos.y=0;
	D3DXVECTOR3 _ThisDir = m_pAlien->GetDir();

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
	}else if(m_fAttack<=float(m_iAttackDelay-m_fAttackSpeed*2.0f-0.55f))	//	�������Ӹ� ���̰� - �� �ִϸ��̼� ������ �������� ( 1.90f - 1.0f)
	{
		m_fAttack-=m_fElapstime*50.0f;
		(*_IsAttack)=FALSE;
	}else 
	{
		m_fAttack-=m_fElapstime*1.0f;
	}
}