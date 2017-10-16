#include "stdafx.h"
#include "O2UFO.h"
#include "O2Hero.h"
#include "Manager.h"

O2UFO::O2UFO()
{
	m_pHero = NULL;
	m_pTongue=NULL;
	m_pAI=NULL;
	m_bIsAttack=FALSE;
	m_bIsHit=FALSE;
	m_bUfoSound=FALSE;
}

O2UFO::O2UFO(O2Hero* _hero)
{
	m_pHero = _hero;
	m_pTongue=NULL;
	m_pAI=NULL;
	m_bIsAttack=FALSE;
	m_bIsHit=FALSE;
	m_bUfoSound=FALSE;
}

O2UFO::~O2UFO()
{
	CleanUp();
	m_pHero=NULL;
	m_bUfoSound=FALSE;
}

VOID O2UFO::CleanUp()
{
	O23DPassiveObject::CleanUp();
	SAFE_DELETE(m_pAI);
	SAFE_DELETE(m_pTongue);
}


// 애니메이션 및 Ai 업데이트
HRESULT O2UFO::UpDate()
{
	O23DPassiveObject::UpDate();
	UINT m_state=m_pHero->GetState();

	if(m_state!=O2Hero::STATE_DIE&&m_state!=O2Hero::STATE_CLEAR)
	{
		m_pAI->Update(m_pHero->GetPos(),m_pHero->GetHide(),(*m_fpDelta),&m_bIsAttack,&m_bIsHit);

		// 위치 설정
		D3DXVECTOR3 temp=GetPos();
		temp.y+=10.0f;
		m_pTongue->SetPos(temp);

		if(m_bUfoSound==FALSE)
		{
			m_bUfoSound=TRUE;
			O2SOUNDINSTANCE->Play3DEffect(_T("UFO_Sound"),O2SoundManager::S_TYPE_INGAME,_LoopSound,GetPos(),&GetPos());
		}

		if(m_bIsAttack)
		{
			if(m_pTongue->GetIsAniStop())
			{
				m_pTongue->PlayAnim(ANIM_ONCE);
				O2SOUNDINSTANCE->Play3DEffect(_T("UFO_Attack"),O2SoundManager::S_TYPE_INGAME,NULL,GetPos(),&GetPos());
			}
			m_pTongue->UpDate();
			m_pTongue->SetRot(m_pHero->GetRot());
		}else if(m_bIsHit)		//캐릭터가 맞으면 그려줌
		{
			m_pTongue->StopAnim();
			m_pTongue->UpDate();
			m_pHero->SetState(O2Hero::STATE_DIE);
		}else					//끝나면 멈춤
		{
			m_pTongue->StopAnim();
		}
	}

	return S_OK;
}

HRESULT O2UFO::Render()
{
	O23DPassiveObject::Render();


// 공격중이거나 플레이어가 맞은상태이면 그려줌
	if((m_bIsAttack||m_bIsHit)&&!(m_pHero->GetState()==O2Hero::STATE_CLEAR))
	{
		m_pTongue->Render();		
	}

	return S_OK;
}

HRESULT O2UFO::Init( LPDIRECT3DDEVICE9* lpDev ,TCHAR* _filename ,TCHAR* _objname ,D3DXVECTOR3 pPos/*=D3DXVECTOR3(0.0f,0.0f,0.0f) */,D3DXVECTOR3 pSize/*=D3DXVECTOR3(1.0f,1.0f,1.0f) */,D3DXVECTOR3 pRot/*=D3DXVECTOR3(0.0f,0.0f,0.0f) */ )
{
	// UFO 초기화
	O23DPassiveObject::Init(lpDev,_filename,_objname,pPos,pSize,pRot);
	// AI 초기화
	m_pAI = new O2AI_UFO;
	if(m_pAI==NULL)
		return E_FAIL;
	TCHAR ainame[32];
	_tcscpy(ainame,_T("Ai/"));
	_tcscat(ainame,_objname);
	_tcscat(ainame,_T(".txt"));
	LoadAttributesFromFile(ainame);

	m_pAI->SetGUIMgr(((Manager*)s_pManager)->GetGUIMgr());
	m_pAI->SetUFO(this);
	// UFO 혀 초기화
	m_pTongue = new O23DActiveObject();
	if(FAILED(m_pTongue->Init(m_lpDevice,_T("Tongue.x"),_T("_Tongue_"),D3DXVECTOR3(0,0,0),D3DXVECTOR3(0.5f,1.0f,0.5f)))){
		SAFE_DELETE(m_pTongue);
		return E_FAIL;
	}
	m_pTongue->SetDelta(m_fpDelta);
	m_pTongue->SetEffect(m_pEffect);
	m_pTongue->StopAnim();
	

	return S_OK;
}

HRESULT O2UFO::RenderLine()
{

//	UFO 렌더라인
	(*m_lpDevice)->SetTransform( D3DTS_WORLD, &m_matLiner );

	// 텍스처 비워줌
	(*m_lpDevice)->SetTexture( 0, NULL );
	// 재질 ( 색 )
	_D3DMATERIAL9 material;
	material.Diffuse.r = 0.0f;
	material.Diffuse.g = 0.0f;
	material.Diffuse.b = 0.0f;
	material.Diffuse.a = 1.0f;
	(*m_lpDevice)->SetMaterial( &material );

	// 그리기
	for( DWORD i=0; i < m_dwNumMaterials; i++ )
	{
		(*m_lpMesh)->DrawSubset( i );
	}


//	혀 렌더라인 
	if((m_bIsAttack||m_bIsHit)&&!(m_pHero->GetState()==O2Hero::STATE_CLEAR)){
		if(FAILED(m_pTongue->RenderLine())){
			return E_FAIL;
		}
	}

	return S_OK;
}

void O2UFO::LoadAttributesFromFile(TCHAR* _fname)
{
	FILE* fp=_tfopen(_fname,_T("r"));
	TCHAR line[256];
	int _Attdelay=10,_Tiredlevel=10,_RoamNum=0;
	float _Roamspd=10,_Searchspd=10,_Followspd=20;
	float _Searchrange=25,_Attrange=20,_Attscale=5,_Attspd=0.5;
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

	m_pAI->SetStats(_Attdelay,_Tiredlevel,_Roamspd,_Searchspd,
		_Followspd,_Searchrange,_Attrange,_Attscale,_Attspd);
}
HRESULT O2UFO::RenderShadow(UINT pass)
{
	if(FAILED(O23DPassiveObject::RenderShadow(pass))){
		return E_FAIL;
	}

	if(m_bIsAttack||m_bIsHit){
		if(FAILED(m_pTongue->RenderShadow(pass))){
			return E_FAIL;
		}
	}

	return S_OK;
}