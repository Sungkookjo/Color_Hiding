#include "stdafx.h"
#include "O2Alien.h"
#include "O2Hero.h"
#include "Manager.h"


O2Alien::O2Alien()
{
	m_pAI = NULL;
	m_pHero = NULL;
	m_bIsAttack=FALSE;
	m_bIsHit=FALSE;
	m_eState=ALIEN_STATE_WALK;
	m_fAiupdatedelay=0;
	m_iState = 0;
	m_fSightDist = 35.0f;
	m_fSightRad = 25.0f;
	m_bAttkSound = FALSE;
}

O2Alien::O2Alien( O2Hero* _hero )
{
	m_pAI = NULL;
	m_pHero = _hero;
	m_bIsAttack=FALSE;
	m_bIsHit=FALSE;
	m_eState=ALIEN_STATE_WALK;
	m_fAiupdatedelay=0;
	m_iState = 0;
	m_fSightDist = 35.0f;
	m_fSightRad = 25.0f;
	m_bAttkSound = FALSE;
}

O2Alien::~O2Alien()
{
	CleanUp();
	m_pHero = NULL;
}

VOID O2Alien::CleanUp()
{
	O23DActiveObject::CleanUp();
	SAFE_DELETE(m_pAI);
}

// 애니메이션 및 Ai 업데이트
HRESULT O2Alien::UpDate()
{
	m_fAiupdatedelay+=(*m_fpDelta);
	UINT m_state=m_pHero->GetState();
	UINT state = ALIEN_STATE_WALK;

	if(m_state!=O2Hero::STATE_DIE&&m_state!=O2Hero::STATE_CLEAR&&((*m_fpDelta)>=0.0000000001f))
	{
		O23DActiveObject::UpDate();

		m_pAI->SetInSight(PickHero());

		m_pAI->Update(m_pHero->GetPos(),m_pHero->GetHide(),(*m_fpDelta),&m_bIsAttack,&m_bIsHit);
		RotY(m_pAI->GetRotRad());
		
		if(m_bIsAttack)
		{
			state=ALIEN_STATE_ATTACK;
		}else if(m_bIsHit)		//캐릭터가 맞으면 그려줌
		{
			state=ALIEN_STATE_ATTACK;
			m_pHero->SetState(O2Hero::STATE_DIE);
		}else					//끝나면 멈춤
		{
		//	StopAnim();
		}

		if(m_iState!=state)
		{
			m_iState=state;
			UpDateAnim();
			if(state==ALIEN_STATE_ATTACK)
				O2SOUNDINSTANCE->Play3DEffect(_T("Alien_Attack"),O2SoundManager::S_TYPE_INGAME,NULL,GetPos(),&GetPos());
		}

	}

	return S_OK;
}

HRESULT O2Alien::Render()
{
	O23DActiveObject::Render();


	return S_OK;
}

HRESULT O2Alien::Init( LPDIRECT3DDEVICE9* lpDev ,TCHAR* _filename ,TCHAR* _objname ,D3DXVECTOR3 pPos/*=D3DXVECTOR3(0.0f,0.0f,0.0f) */,D3DXVECTOR3 pSize/*=D3DXVECTOR3(1.0f,1.0f,1.0f) */,D3DXVECTOR3 pRot/*=D3DXVECTOR3(0.0f,0.0f,0.0f) */ )
{
	// Alien 초기화
	O23DActiveObject::Init(lpDev,_filename,_objname,pPos,pSize,pRot);
	// UFO 초기화
	m_pAI = new O2AI_Alien;
	if(m_pAI==NULL)
		return E_FAIL;
	m_pAI->SetGUIMgr(((Manager*)s_pManager)->GetGUIMgr());
	m_pAI->SetAlien(this);
	TCHAR ainame[32];
	_tcscpy(ainame,_T("Ai/"));
	_tcscat(ainame,_objname);
	_tcscat(ainame,_T(".txt"));
	LoadAttributesFromFile(ainame);
	

	SetDelta(m_fpDelta);
	SetEffect(m_pEffect);
	SetSpeed(1.0f);

	m_adwAniId[ALIEN_STATE_WALK] = GetAniIndex("ali");
	m_adwAniId[ALIEN_STATE_ATTACK] = GetAniIndex("ail_2");


	for(int i =0;i<ALIEN_STATE_MAX;++i){
		if(ANIMINDEX_FAIL == m_adwAniId[i]){
			return E_FAIL;
		}
	}

	m_vDestPos = m_vPos;
	UpDateAnim();

	return S_OK;
}

void O2Alien::LoadAttributesFromFile(TCHAR* _fname)
{
	FILE* fp=_tfopen(_fname,_T("r"));
	TCHAR line[256];
	int _Attdelay,_Tiredlevel,_RoamNum=0;
	float _Roamspd,_Searchspd,_Followspd;
	float _Searchrange,_Attrange,_Attscale,_Attspd;
	D3DXVECTOR3 _Roamspot;


	_fgetts(line,256,fp);
	_stscanf(line,_T("%d %d"),&_Attdelay,&_Tiredlevel);
	_fgetts(line,256,fp);
	_stscanf(line,_T("%f %f %f"),&_Roamspd,&_Searchspd,&_Followspd);
	_fgetts(line,256,fp);
	_stscanf(line,_T("%f %f %f %f"),&_Searchrange,&_Attrange,&_Attscale,&_Attspd);
	_fgetts(line,256,fp);
	_stscanf(line,_T("%d"),&_RoamNum);
	for(int i=0;i<_RoamNum;i++)
	{
		_fgetts(line,256,fp);
		_stscanf(line,_T("%f %f %f"),&_Roamspot.x,&_Roamspot.y,&_Roamspot.z);
		m_pAI->SetRoamingSpot(_Roamspot);
	}
	m_fSightDist = _Searchrange;

	m_pAI->SetStats(_Attdelay,_Tiredlevel,_Roamspd,_Searchspd,
		_Followspd,_Searchrange,_Attrange,_Attscale,_Attspd);
}

void O2Alien::UpDateAnim()
{
	DWORD dwNewTrack = ( m_dwCurTrack == 0 ? 1 : 0 );
	LPD3DXANIMATIONSET pAS;

	// 새로 설정할 애니셋 얻어옴
	m_pAnimController->GetAnimationSet( m_adwAniId[m_iState], &pAS );

	// 다른 트랙에 애니셋 설정
	m_pAnimController->SetTrackAnimationSet( dwNewTrack, pAS );
	// 현재 애니의 길이를 받아온다.
	m_lfAniTime = pAS->GetPeriod();
	// 현재 시간 저장
	m_lfDestTime = m_fCurTime;
	pAS->Release();

	// 트랙의 모든 이벤트 해제
	m_pAnimController->UnkeyAllTrackEvents( m_dwCurTrack );
	m_pAnimController->UnkeyAllTrackEvents( dwNewTrack );

	// 트랙의 속도, 가중치 설정
	m_pAnimController->KeyTrackEnable( m_dwCurTrack, FALSE, (m_fPreTime) + IDLE_TRANSITION_TIME );
	m_pAnimController->KeyTrackSpeed( m_dwCurTrack, 0.0f, (m_fPreTime), IDLE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
	m_pAnimController->KeyTrackWeight( m_dwCurTrack, 0.0f, (m_fPreTime), IDLE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
	//m_fCurTime = 0.0f;
	m_pAnimController->SetTrackEnable( dwNewTrack, TRUE );
	m_pAnimController->KeyTrackSpeed( dwNewTrack, 1.0f, (m_fCurTime), IDLE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
	m_pAnimController->KeyTrackWeight( dwNewTrack, 1.0f, (m_fCurTime), IDLE_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
	m_pAnimController->SetTrackPosition( dwNewTrack, 0.0 );

	m_dwCurTrack = dwNewTrack;
}

// BOOL O2Alien::PickObject(O23DObject* _vObj)
// {
// 
// 	D3DXVECTOR3 _Objpos = _vObj->GetPos();
// 	D3DXVECTOR3 _Alipos	= GetPos();
// 	D3DXVECTOR3 _Heropos = m_pHero->GetPos();
// 	
// 	_Alipos.y += (GetColl()._max.y-GetColl()._min.y)*(GetSize().y*0.75f);
// 	_Heropos.y += (m_pHero->GetColl()._max.y-m_pHero->GetColl()._min.y)*(m_pHero->GetSize().y*0.5f);
// 
// 	D3DXVECTOR3 _AliDir = _Heropos - _Alipos;
// 	D3DXVec3Normalize(&_AliDir,&_AliDir);
// 
// 	D3DXVECTOR3 V[8];
// 	D3DXVECTOR3 collx,colly,collz;
// 	collx = colly = collz =D3DXVECTOR3(0,0,0);
// 	V[0] = V[1] = V[2] = V[3] = V[4] = V[5] = V[6] = V[7] = _Objpos;
// 
// 	collx.x=(((_vObj->GetColl()._max.x)-(_vObj->GetColl()._min.x))*(_vObj->GetSize().x)/2.0f);
//  	colly.y=(((_vObj->GetColl()._max.y)-(_vObj->GetColl()._min.y))*(_vObj->GetSize().y)/2.0f);
//  	collz.z=(((_vObj->GetColl()._max.z)-(_vObj->GetColl()._min.z))*(_vObj->GetSize().z)/2.0f);
// 
// 	V[0] += (collx + collz + colly) ;
// 	V[1] += (collx + collz - colly) ; 
// 	V[2] += (collx - collz + colly) ;
// 	V[3] += (collx - collz - colly) ;
// 	V[4] += (-collx + collz + colly) ;
// 	V[5] += (-collx + collz - colly) ;
// 	V[6] += (-collx - collz + colly) ;
// 	V[7] += (-collx - collz - colly) ;
// 
// 	float tDist[12];
// 	if((D3DXIntersectTri(&V[0],&V[1],&V[3],&_Alipos,&_AliDir,NULL,NULL,&tDist[0]))
// 			||(D3DXIntersectTri(&V[0],&V[2],&V[3],&_Alipos,&_AliDir,NULL,NULL,&tDist[1]))
// 			||(D3DXIntersectTri(&V[0],&V[2],&V[6],&_Alipos,&_AliDir,NULL,NULL,&tDist[2]))
// 			||(D3DXIntersectTri(&V[0],&V[4],&V[6],&_Alipos,&_AliDir,NULL,NULL,&tDist[3]))
// 			||(D3DXIntersectTri(&V[0],&V[1],&V[5],&_Alipos,&_AliDir,NULL,NULL,&tDist[4]))
// 			||(D3DXIntersectTri(&V[0],&V[4],&V[5],&_Alipos,&_AliDir,NULL,NULL,&tDist[5]))
// 			||(D3DXIntersectTri(&V[7],&V[3],&V[1],&_Alipos,&_AliDir,NULL,NULL,&tDist[6]))
// 			||(D3DXIntersectTri(&V[7],&V[5],&V[1],&_Alipos,&_AliDir,NULL,NULL,&tDist[7]))
// 			||(D3DXIntersectTri(&V[7],&V[6],&V[4],&_Alipos,&_AliDir,NULL,NULL,&tDist[8]))
// 			||(D3DXIntersectTri(&V[7],&V[5],&V[4],&_Alipos,&_AliDir,NULL,NULL,&tDist[9]))
// 			||(D3DXIntersectTri(&V[7],&V[6],&V[2],&_Alipos,&_AliDir,NULL,NULL,&tDist[10]))
// 			||(D3DXIntersectTri(&V[7],&V[3],&V[2],&_Alipos,&_AliDir,NULL,NULL,&tDist[11])))
// 	{
// 		float _ObjDist=0;
// 		for(int j=0;j<12;j++)
// 		{
// 			if(tDist[j]>_ObjDist)
// 			{
// 				_ObjDist=tDist[j];
// 			}
// 		}
// 		float _HeroDist = D3DXVec3Length(&(GetPos()-m_pHero->GetPos()));
// 		if(_ObjDist<_HeroDist)
// 			return TRUE;
// 	}
// 
// 
// 	return FALSE;
// }

BOOL O2Alien::PickHero()
{
	D3DXVECTOR3 _Heropos = m_pHero->GetPos();
	D3DXVECTOR3 _Alipos	= GetPos();
	float		_Sightrad = m_fSightRad;
	D3DXVECTOR3 _AliDir[3];
	_AliDir[0] = -GetDir();
	D3DXVECTOR3 _AliRight;
	D3DXVec3Cross(&_AliRight,&_AliDir[0],&D3DXVECTOR3(0,1,0));
	_AliDir[1]=_AliDir[0]+_AliRight/2.0f;
	_AliDir[2]=_AliDir[0]-_AliRight/2.0f;
	D3DXVec3Normalize(&_AliDir[1],&_AliDir[1]);
	D3DXVec3Normalize(&_AliDir[2],&_AliDir[2]);


	D3DXVECTOR3 _CurSight=_Alipos;
	float		_SightDist = 0 ;
	float		_HeroDist = D3DXVec3Length(&(_CurSight-_Heropos));

	for(int i=0;i<3;i++)
	{
		while(_SightDist<m_fSightDist&&_HeroDist>_Sightrad)
		{
			_CurSight+=_AliDir[i];
			_SightDist = D3DXVec3Length(&(_CurSight-_Alipos));
			_HeroDist = D3DXVec3Length(&(_CurSight-_Heropos));
		}
		if(_HeroDist<=_Sightrad)
			return TRUE;
	}
	
	return FALSE;
}

HRESULT O2Alien::CollEffect(O2Object* hit)
{
	INT type = hit->GetObjType();

	if((OBJ_TYPE_3D | OBJ_TYPE_PASSIVE) == type )
	{
		SetForce(D3DXVECTOR3(0,0,0));
	//	MoveLeft(10.0f);	
	//	m_pAI->ChangeState(O2AI::AI_TURN);
	}
	return S_OK;
}
