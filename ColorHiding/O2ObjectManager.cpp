#include "stdafx.h"
#include "Manager.h"
#include "O22DObject.h"

O2ObjectManager::O2ObjectManager()
:m_lpDevice(NULL)
,m_pResource(NULL)
,m_pHero(NULL)
,m_pEffect(NULL)
,m_pPartMgr(NULL)
,m_pLight(NULL)
,m_pShadowMap(NULL)
,m_pDSShadow(NULL)
,m_pFrustrum(NULL)
{

}

O2ObjectManager::~O2ObjectManager()
{
	CleanUp();
}

HRESULT O2ObjectManager::AddPassiveObject( TCHAR* _file ,
										  UINT _shape ,
										  BOOL _static ,
										  D3DXVECTOR3 _pos ,
										  D3DXVECTOR3 _rot ,
										  D3DXVECTOR3 _size ,
										  TCHAR* _ObjName,
										  UINT _ObjColor,
										  BOOL _IsRendShadow,
										  BOOL _IsReflectShadow,
										  BOOL _IsEffect,
										  BOOL _IsLine,
										  O23DPassiveObject* pObj,
										  unsigned int _flag)
{
	O23DPassiveObject* obj = (NULL == pObj ? new O23DPassiveObject : pObj);

	// 칼라 저장
	obj->SetColor(_ObjColor);

	// 일렙스 타임 저장
	obj->SetDelta(m_pfElaped);

	// 이펙트 사용여부와 이펙트 저장
	obj->SetRendShadow(_IsRendShadow);
	obj->SetShadowReflect(_IsReflectShadow);
	obj->SetUseCell(_IsEffect);
	obj->SetEffect(m_pEffect);
	obj->SetLineRend(_IsLine);

	// 초기화
	if(FAILED(obj->Init(m_lpDevice,_file,_ObjName,_pos,_size,_rot))){
		SAFE_DELETE(obj);
		return E_FAIL;
	}

	// 물리 모양이 있으면 물리 성질 추가
	if( 0 != _shape ){
		// Actor 생성
		if(FAILED(obj->CreateActor(_shape,_flag,_static))){
			SAFE_DELETE(obj);
			return E_FAIL;
		}
		// 충돌 이벤트리스트에 등록
		SetCollEvent(obj);
		m_l3DObjects.push_back(obj);
	}

	// 오브젝트 리스트에 추가
	m_lAllObjects.push_back(obj);
	return S_OK;
}

HRESULT O2ObjectManager::Init( LPDIRECT3DDEVICE9* lpDev,ResourceManager* pResc,O2ParticleManager* pPart )
{
	HRESULT hr;
	m_lpDevice = lpDev;
	m_pResource = pResc;
	m_pPartMgr	= pPart;
	O2Object::s_pResource = m_pResource;

	// Create the shadow map texture
	V_RETURN( (*m_lpDevice)->CreateTexture( ShadowMap_SIZE, ShadowMap_SIZE,
		1, D3DUSAGE_RENDERTARGET,
		D3DFMT_R32F,
		D3DPOOL_DEFAULT,
		&m_pShadowMap,
		NULL ) );

	V_RETURN( (*m_lpDevice)->CreateDepthStencilSurface( ShadowMap_SIZE,
		ShadowMap_SIZE,
		D3DFMT_D24S8,
		D3DMULTISAMPLE_NONE,
		0,
		TRUE,
		&m_pDSShadow,
		NULL ) );
	m_pFrustrum = new O2Frustum;

	D3DXMatrixPerspectiveFovLH( &m_matShadowProj, D3DX_PI * 0.5f , 1, 0.01f, 100.0f );

	return hr;
}

VOID O2ObjectManager::CleanUp()
{
	list<O2Object*>::iterator it = m_lAllObjects.begin();
	int _size = m_lAllObjects.size();
	for(int i = 0;i<_size; ++i ){
		SAFE_DELETE(*it);
		++it;
	}
	m_lAllObjects.clear();
	m_l3DObjects.clear();
	SAFE_DELETE(m_pHero);
	m_pLight = NULL;

	SAFE_RELEASE(m_pShadowMap);
	SAFE_RELEASE(m_pDSShadow);
	SAFE_DELETE(m_pFrustrum);
}

HRESULT O2ObjectManager::CreateHero( TCHAR* _file
									,D3DXVECTOR3 _pos
									,D3DXVECTOR3 _rot
									,D3DXVECTOR3 _size
									,BOOL _IsShadow
									,BOOL _IsReflectShadow
									,BOOL _IsEffect
									,BOOL _IsLine )
{
	if(NULL != m_pHero){
		return E_FAIL;
	}

	O2Hero* obj = NULL;
	obj = new O2Hero;
	m_pHero = obj;

	if(FAILED(obj->Init(m_lpDevice,_file,_T("_Hero_"),_pos,_size,_rot))){
		SAFE_DELETE(obj);
		return E_FAIL;
	}

	if(FAILED(obj->CreateActor(NX_SHAPE_CAPSULE))){
		SAFE_DELETE(obj);
		return E_FAIL;
	}

	m_pHero->SetDelta(m_pfElaped);
	m_pHero->SetEffect(m_pEffect);
	m_pHero->SetRendShadow(_IsShadow);
	m_pHero->SetShadowReflect(_IsReflectShadow);
	m_pHero->SetLineRend(_IsLine);
	m_pHero->SetUseCell(_IsEffect);

	return S_OK;
}

HRESULT O2ObjectManager::UpDate()
{
	D3DXMATRIXA16	matVP;
	D3DXMatrixMultiply(&matVP,m_pEffect->GetCam()->GetMatView(),m_pEffect->GetCam()->GetProjView());
	if(m_pEffect){
		m_pFrustrum->Make(&matVP);
	}

	list<O2Object*>::iterator it = m_lAllObjects.begin();
	int _size =  m_l3DObjects.size();

	list<O23DObject*>::iterator it3D = m_l3DObjects.begin();
	
	for(int i=0;i<_size;i++)
	{
		if((*it3D)->GetIsDead())
		{
			CreateParticle(*it3D);
			it3D=m_l3DObjects.erase(it3D);
		}else{
			(*it3D)->IsRender(m_pFrustrum);
			it3D++;
		}
	}

	_size =m_lAllObjects.size();
	for(int i = 0;i<_size; ++i){
		if((*it)->GetIsDead())
		{
			SAFE_DELETE(*it);
			it=m_lAllObjects.erase(it);
		}else
		{
			(*it)->UpDate();
			++it;
		}	
	}

	if(m_pHero){
		m_pHero->UpDate();
		if(O2Hero::STATE_CLEAR == m_pHero->GetState()){
			return O2Hero::STATE_CLEAR;
		}else if(O2Hero::STATE_DIE == m_pHero->GetState()){
			return O2Hero::STATE_DIE;
		}
	}

	return S_OK;
}

HRESULT O2ObjectManager::Render()
{
	list<O2Object*>::iterator it = m_lAllObjects.begin();
	int _size = m_lAllObjects.size();
	UINT pass;

	m_pEffect->SetFloat(EFFECT_FLOAT_FAR,1000.0f - 0.1f);
	m_pEffect->SetVector(EFFECT_VEC_SHADOW,D3DXVECTOR4(0.0f,0.1f,0.0f,0.2f));
	m_pEffect->SetVector(EFFECT_VEC_LIGHTPOSITION,m_pLight->GetLightPos());

	(*m_lpDevice)->SetSamplerState( 0, D3DSAMP_MAGFILTER , D3DTEXF_LINEAR );
	(*m_lpDevice)->SetSamplerState( 0, D3DSAMP_MINFILTER , D3DTEXF_LINEAR );

	//////////////////////////////////////////////////////////////////////////
	// 전체 랜더
	m_pEffect->SetTechnique(EFFECT_TECH_AMBIENT);
	m_pEffect->PreRender();
	m_pEffect->GetEffect()->SetVector("Ambient",&D3DXVECTOR4(0.3f,0.3f,0.3f,1.0f));
	it = m_lAllObjects.begin();

	for(int i = 0;i<_size; ++i,++it ){
		if((*it)->GetIsRend()){
			if((*it)->GetShadowReflect()){
				(*it)->Render();	
			}
		}
	}

	m_pEffect->GetEffect()->SetVector("Ambient",&D3DXVECTOR4(0.3f,0.3f,0.3f,0.5f));

	if(m_pHero && FALSE == m_pHero->GetIsSuperHide()){
		if(m_pHero->GetShadowReflect())
			m_pHero->Render();
	}
	m_pEffect->PostRender();
	//////////////////////////////////////////////////////////////////////////
	// 그림자 랜더
	(*m_lpDevice)->Clear( 0, NULL, D3DCLEAR_STENCIL, D3DCOLOR_ARGB( 0, 0, 0, 0 ), 1.0f, 0 );

	m_pEffect->SetTechnique(EFFECT_TECH_SHADOW);
	pass = m_pEffect->PreRender();

	it = m_lAllObjects.begin();

	for(int i = 0;i<_size; ++i,++it ){
		if((*it)->GetShadowRend()){
			(*it)->RenderShadow(pass);
		}
	}

	if(m_pHero){
		if(m_pHero->GetShadowRend()){
			m_pHero->RenderShadow(pass);
		}
	}

	m_pEffect->PostRender();

	//////////////////////////////////////////////////////////////////////////
	// 그림자 안받는놈들 랜더
	m_pEffect->SetTechnique(EFFECT_TECH_AMBIENT);
	m_pEffect->GetEffect()->SetVector("Ambient",&D3DXVECTOR4(1.0f,1.0f,1.0f,1.0f));
	m_pEffect->PreRender();

	it = m_lAllObjects.begin();

	for(int i = 0;i<_size; ++i,++it ){
		if((*it)->GetIsRend()){
			if(!(*it)->GetShadowReflect() ){
				(*it)->Render();	
			}
		}
	}

	if(m_pHero){
		if(!m_pHero->GetShadowReflect())
			m_pHero->Render();
	}
	m_pEffect->PostRender();
	//////////////////////////////////////////////////////////////////////////
	// 슈퍼 하이딩
	if(m_pHero && m_pHero->GetIsSuperHide()){
		m_pEffect->GetEffect()->SetTechnique("RenderSceneAmbientWithAlpha");
	}

	m_pEffect->PreRender();
	m_pEffect->GetEffect()->SetVector("Ambient",&D3DXVECTOR4(0.3f,0.3f,0.3f,0.1f));
	if(m_pHero){
		m_pHero->Render();
	}
	m_pEffect->PostRender();

	//////////////////////////////////////////////////////////////////////////
	// 셀 쉐이딩
	m_pEffect->SetTechnique(EFFECT_TECH_CELL);
	m_pEffect->PreRender();

//	(*m_lpDevice)->SetVertexDeclaration(   );
	it = m_lAllObjects.begin();
	for(int i = 0;i<_size; ++i,++it ){
		if((*it)->GetIsRend()){
			if((*it)->GetCellRend()){
				(*it)->Render();
			}
		}
	}

	if(m_pHero){
		if(m_pHero->GetCellRend()){
			m_pHero->Render();
		}
	}

	m_pEffect->PostRender();


	//////////////////////////////////////////////////////////////////////////
	// 외곽선
	(*m_lpDevice)->SetRenderState(D3DRS_LIGHTING,TRUE);
	(*m_lpDevice)->SetSamplerState( 0, D3DSAMP_MAGFILTER , D3DTEXF_LINEAR );
	(*m_lpDevice)->SetSamplerState( 0, D3DSAMP_MINFILTER , D3DTEXF_LINEAR );

	it = m_lAllObjects.begin();
	(*m_lpDevice)->SetRenderState(D3DRS_CULLMODE,D3DCULL_CW);
	(*m_lpDevice)->SetTexture( 0, NULL );
	(*m_lpDevice)->Clear( 0, NULL, D3DCLEAR_STENCIL, D3DCOLOR_ARGB( 0, 0, 0, 0 ), 1.0f, 0 );

	for(int i = 0;i<_size; ++i,++it ){
		if((*it)->GetIsRend()){
			if((*it)->GetRendLine()){
				(*it)->RenderLine();
			}
		}
	}

	if(m_pHero){
		if(m_pHero->GetRendLine())
			m_pHero->RenderLine();
	}

	(*m_lpDevice)->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	(*m_lpDevice)->SetRenderState(D3DRS_LIGHTING,FALSE);
	/// 임시
	//////////////////////////////////////////////////////////////////////////


	return S_OK;
}

VOID O2ObjectManager::Flush()
{
	list<O2Object*>::iterator it = m_lAllObjects.begin();
	int _size = m_lAllObjects.size();
	for(int i = 0;i<_size; ++i ){
		SAFE_DELETE(*it);
		++it;
	}
	m_lAllObjects.clear();
	m_l3DObjects.clear();
	SAFE_DELETE(m_pHero);
}

VOID O2ObjectManager::SetCollEvent( O23DObject* pObj )
{
	list<O23DObject*>::iterator _it = m_l3DObjects.begin();
	list<O23DObject*>::iterator _end = m_l3DObjects.end();

	if(m_pHero){
		JNxScene->setActorPairFlags(*(m_pHero->GetActor()),*(pObj->GetActor()),NX_NOTIFY_ON_TOUCH);
	}

	while(_it != _end){
		JNxScene->setActorPairFlags(*((*_it)->GetActor()),*(pObj->GetActor()),NX_NOTIFY_ON_TOUCH);
		++_it;
	}
}

// float O2ObjectManager::Picking(D3DXVECTOR3* _Pos,D3DXVECTOR3* _Dir,float* _Codist)
// {
// 	D3DXVECTOR3 V[8];
// 	D3DXVECTOR3 collx,colly,collz;
// 	collx = colly = collz =D3DXVECTOR3(0,0,0);
// 	float tDist[12],CoDist,Adist=0;
// 	
// 
// 	// 카메라 - 플레이어 간 거리를 측정
// 	CoDist = (*_Codist);
// 	list<O23DObject*>::iterator it = m_l3DObjects.begin();
// 	int _size = m_l3DObjects.size();
// 
// //	printf("COdist %f \n",CoDist);
// 	for(int i = 0;i<_size; ++i,++it ){
// 
// 	//임시로 배경과 충돌 X
// 	//	if(i==0) continue;
// 		
// 
// 	//정점 8개로 오브젝트의 크기에 따라 정육면체를 만듬
// 		V[0] = V[1] = V[2] = V[3] = V[4] = V[5] = V[6] = V[7] = (*it)->GetPos();	
// 		collx.x=((((*it)->GetColl()._max.x)-((*it)->GetColl()._min.x))*((*it)->GetSize().x)/2.0f);
// 		colly.y=((((*it)->GetColl()._max.y)-((*it)->GetColl()._min.y))*((*it)->GetSize().y)/2.0f);
// 		collz.z=((((*it)->GetColl()._max.z)-((*it)->GetColl()._min.z))*((*it)->GetSize().z)/2.0f);
// 		
// 
// 		
// 		V[0] += (collx + collz + colly) ;
// 		V[1] += (collx + collz - colly) ; 
// 		V[2] += (collx - collz + colly) ;
// 		V[3] += (collx - collz - colly) ;
// 		V[4] += (-collx + collz + colly) ;
// 		V[5] += (-collx + collz - colly) ;
// 		V[6] += (-collx - collz + colly) ;
// 		V[7] += (-collx - collz - colly) ;
// 
// 	//정점8개를 이용해 삼각형 12개의 면을 검사
// 		if((D3DXIntersectTri(&V[0],&V[1],&V[3],_Pos,_Dir,NULL,NULL,&tDist[0]))
// 			||(D3DXIntersectTri(&V[0],&V[2],&V[3],_Pos,_Dir,NULL,NULL,&tDist[1]))
// 			||(D3DXIntersectTri(&V[0],&V[2],&V[6],_Pos,_Dir,NULL,NULL,&tDist[2]))
// 			||(D3DXIntersectTri(&V[0],&V[4],&V[6],_Pos,_Dir,NULL,NULL,&tDist[3]))
// 			||(D3DXIntersectTri(&V[0],&V[1],&V[5],_Pos,_Dir,NULL,NULL,&tDist[4]))
// 			||(D3DXIntersectTri(&V[0],&V[4],&V[5],_Pos,_Dir,NULL,NULL,&tDist[5]))
// 			||(D3DXIntersectTri(&V[7],&V[3],&V[1],_Pos,_Dir,NULL,NULL,&tDist[6]))
// 			||(D3DXIntersectTri(&V[7],&V[5],&V[1],_Pos,_Dir,NULL,NULL,&tDist[7]))
// 			||(D3DXIntersectTri(&V[7],&V[6],&V[4],_Pos,_Dir,NULL,NULL,&tDist[8]))
// 			||(D3DXIntersectTri(&V[7],&V[5],&V[4],_Pos,_Dir,NULL,NULL,&tDist[9]))
// 			||(D3DXIntersectTri(&V[7],&V[6],&V[2],_Pos,_Dir,NULL,NULL,&tDist[10]))
// 			||(D3DXIntersectTri(&V[7],&V[3],&V[2],_Pos,_Dir,NULL,NULL,&tDist[11])))
// 		{
// 			//카메라-플레이어 간 사이에 껴있고
// 			//카메라-오브젝트간 거리가 가장 큰것을 얻는다
// 			for(int j=0;j<12;j++)
// 			{
// 				if(tDist[j]<CoDist&&tDist[j]>Adist)
// 				{
// 					Adist=tDist[j];
// 				}
// 			}
// 		}
// 	}
// 
// 	return Adist;
// }
// 
// float O2ObjectManager::Picking_Mesh(D3DXVECTOR3* _Pos,D3DXVECTOR3* _Dir,float* _Codist)
// {
// 	D3DXVECTOR3 collx,colly,collz;
// 	collx = colly = collz =D3DXVECTOR3(0,0,0);
// 	float tDist,CoDist,Adist=0;
// 	BOOL hit;
// 	D3DXIntersect(*(m_pHero->GetMesh()),_Pos,_Dir,&hit,NULL,NULL,NULL,&tDist,NULL,NULL);
// 	CoDist = (*_Codist);
// //	CoDist = tDist;
// 	printf("Codist %f\n",CoDist);
// 	list<O23DObject*>::iterator it = m_l3DObjects.begin();
// 	int _size = m_l3DObjects.size();
// 	//	printf("COdist %f \n",CoDist);
// 	for(int i = 0;i<_size; ++i,++it ){
// 
// 
// 		//정점8개를 이용해 삼각형 12개의 면을 검사
// 		if((D3DXIntersect(*((*it)->GetMesh()),_Pos,_Dir,&hit,NULL,NULL,NULL,&tDist,NULL,NULL)))
// 		{
// 			//카메라-플레이어 간 사이에 껴있고
// 			//카메라-오브젝트간 거리가 가장 큰것을 얻는다
// 			printf("Adist %f \n",Adist);
// 			if(hit&&tDist<CoDist&&tDist>Adist)
// 			{
// 				Adist=tDist;
// 				printf("Adist %f \n",Adist);
// 			}
// 		}
// 	}
// 
// 	return Adist;
// }

HRESULT O2ObjectManager::AddActiveObject( TCHAR* _file
										 ,UINT _shape
										 ,BOOL _static
										 ,D3DXVECTOR3 _pos
										 ,D3DXVECTOR3 _rot
										 ,D3DXVECTOR3 _size
										 ,TCHAR* _ObjName
										 ,UINT _ObjColor
										 ,BOOL _IsRendShadow
										 ,BOOL _IsReflectShadow
										 ,BOOL _IsEffect
										 ,BOOL _IsLine
										 ,O23DActiveObject* pObj
										 )
{
	O23DActiveObject* obj = (NULL == pObj ? new O23DActiveObject : pObj);

	// 칼라 저장
	obj->SetColor(_ObjColor);

	// 일렙스 타임 저장
	obj->SetDelta(m_pfElaped);

	// 이펙트 사용여부와 이펙트 저장
	obj->SetRendShadow(_IsRendShadow);
	obj->SetShadowReflect(_IsReflectShadow);
	obj->SetUseCell(_IsEffect);
	obj->SetEffect(m_pEffect);
	obj->SetLineRend(_IsLine);

	// 초기화
	if(FAILED(obj->Init(m_lpDevice,_file,_ObjName,_pos,_size,_rot))){
		SAFE_DELETE(obj);
		return E_FAIL;
	}

	// 물리 모양이 있으면 물리 성질 추가
	if( 0 != _shape ){
		// Actor 생성
		if(FAILED(obj->CreateActor(_shape,_static))){
			SAFE_DELETE(obj);
			return E_FAIL;
		}
		// 충돌 이벤트리스트에 등록
		SetCollEvent(obj);
		m_l3DObjects.push_back(obj);
	}

	// 오브젝트 리스트에 추가
	m_lAllObjects.push_back(obj);

	return S_OK;
}

HRESULT O2ObjectManager::Add2DObject(TCHAR* filename, O2Rect pRect, POINT imgsize, D3DXVECTOR3 pPos, D3DXVECTOR3 pSize/*=D3DXVECTOR3(1.0f,1.0f,1.0f)*/, D3DXVECTOR3 pRot/*=D3DXVECTOR3(0.0f,0.0f,0.0f) */ )
{
	O22DObject* obj = new O22DObject;

	if(FAILED(obj->Init(m_lpDevice,filename,pRect,imgsize,pPos,pSize,pRot))){
		SAFE_DELETE(obj);
		return E_FAIL;
	}
	obj->SetDelta(m_pfElaped);

	m_lAllObjects.push_back(obj);

	return S_OK;
}

void O2ObjectManager::CreateParticle( O23DObject* _it )
{
	D3DXVECTOR3 vTemp = _it->GetPos();
	UINT type = _it->GetObjType();
	

	if(type==OBJ_TYPE_CAPSULE)
	{
		UINT _color = _it->GetColor();
		vTemp.y-=_it->GetSize().y*2.0f;

		switch (_color)
		{
		case COLOR_G:
			m_pPartMgr->AddParticle(_T("particle/Cap_Green.txt"),&vTemp);
			break;
		case COLOR_R:
			m_pPartMgr->AddParticle(_T("particle/Cap_Pink.txt"),&vTemp);
			break;
		case COLOR_B:
			m_pPartMgr->AddParticle(_T("particle/Cap_Blue.txt"),&vTemp);
			break;
		case COLOR_P:
			m_pPartMgr->AddParticle(_T("particle/Cap_Yellow.txt"),&vTemp);
			break;
		default:
			break;
		}
	}else if(type==OBJ_TYPE_ITEM)
	{
		m_pPartMgr->AddParticle(_T("particle/Item.txt"),&vTemp);
	}
}



HRESULT O2ObjectManager::AddAlien( TCHAR* _file
										 ,UINT _shape
										 ,BOOL _static
										 ,D3DXVECTOR3 _pos
										 ,D3DXVECTOR3 _rot
										 ,D3DXVECTOR3 _size
										 ,TCHAR* _ObjName
										 ,UINT _ObjColor
										 ,BOOL _IsRendShadow
										 ,BOOL _IsReflectShadow
										 ,BOOL _IsEffect
										 ,BOOL _IsLine
										 )
{
	O2Alien* obj = new O2Alien(m_pHero) ;

	// 칼라 저장
	obj->SetColor(_ObjColor);

	// 일렙스 타임 저장
	obj->SetDelta(m_pfElaped);

	// 이펙트 사용여부와 이펙트 저장
	obj->SetRendShadow(_IsRendShadow);
	obj->SetShadowReflect(_IsReflectShadow);
	obj->SetUseCell(_IsEffect);
	obj->SetEffect(m_pEffect);
	obj->SetLineRend(_IsLine);

	// 초기화
	if(FAILED(obj->Init(m_lpDevice,_file,_ObjName,_pos,_size,_rot))){
		SAFE_DELETE(obj);
		return E_FAIL;
	}

	// 물리 모양이 있으면 물리 성질 추가
	if( 0 != _shape ){
		// Actor 생성
		if(FAILED(obj->CreateActor(_shape,NX_BF_FROZEN_POS_Y,_static))){
			SAFE_DELETE(obj);
			return E_FAIL;
		}
		// 충돌 이벤트리스트에 등록
		SetCollEvent(obj);
		m_l3DObjects.push_back(obj);
	}

	// 오브젝트 리스트에 추가
	m_lAllObjects.push_back(obj);

	return S_OK;
}

HRESULT O2ObjectManager::AddItem( TCHAR* _file ,UINT _shape,BOOL _static,D3DXVECTOR3 _pos,D3DXVECTOR3 _rot,D3DXVECTOR3 _size,TCHAR* _ObjName,BOOL _IsShadow /*= FALSE */,BOOL _IsReflectShadow/*=FALSE */,BOOL _IsEffect/*=TRUE */,BOOL _IsLine/*=TRUE */ )
{
	O2Item* obj =new O2Item;

	// 일렙스 타임 저장
	obj->SetDelta(m_pfElaped);

	// 이펙트 사용여부와 이펙트 저장
	obj->SetRendShadow(_IsShadow);
	obj->SetShadowReflect(_IsReflectShadow);
	obj->SetUseCell(_IsEffect);
	obj->SetEffect(m_pEffect);
	obj->SetLineRend(_IsLine);

	// 초기화
	if(FAILED(obj->Init(m_lpDevice,_file,_ObjName,_pos,_size,_rot))){
		SAFE_DELETE(obj);
		return E_FAIL;
	}

	// 물리 모양이 있으면 물리 성질 추가
	if( 0 != _shape ){
		// Actor 생성
		if(FAILED(obj->CreateActor(_shape,_static))){
			SAFE_DELETE(obj);
			return E_FAIL;
		}
		// 충돌 이벤트리스트에 등록
		SetCollEvent(obj);
		m_l3DObjects.push_back(obj);
	}

	// 오브젝트 리스트에 추가
	m_lAllObjects.push_back(obj);
	return S_OK;
}

// 1 ali
// 0 ufo
HRESULT O2ObjectManager::CreateAddtionalMob( UINT _index,UINT _stage )
{
	UINT num =rand()%3;
	TCHAR* tname=NULL;
	float ay=0;
	D3DXVECTOR3 _TempPos = m_pHero->GetPos();
	D3DXVECTOR3 _TempRoam = _TempPos;
	if(_index==0)
	{
		O2UFO* _NewUFO = new O2UFO(m_pHero);
		_NewUFO->SetColor(COLOR_NONE);

		// 일렙스 타임 저장
		_NewUFO->SetDelta(m_pfElaped);

		// 이펙트 사용여부와 이펙트 저장
		_NewUFO->SetRendShadow(TRUE);
		_NewUFO->SetShadowReflect(TRUE);
		_NewUFO->SetUseCell(TRUE);
		_NewUFO->SetEffect(m_pEffect);
		_NewUFO->SetLineRend(TRUE);

		

		_TempPos.x+=GetRandomFloat(10,40);
		_TempPos.y+=GetRandomFloat(0,20);
		_TempPos.z+=GetRandomFloat(10,40);
		// 초기화
		if(num==0)
			tname = _T("AddUFO01");
		else if(num==1)
			tname = _T("AddUFO02");
		else
			tname = _T("AddUFO03");

		if(_stage==0)
			ay = 90.0f;
		else if(_stage==1)
			ay = 140.0f;
		else
			ay = 160.0f;
		_TempPos.y+=ay;
		if(FAILED(_NewUFO->Init(m_lpDevice,_T("UFO.X"),tname,_TempPos,D3DXVECTOR3(1,1,1),D3DXVECTOR3(1,1,1)))){
			SAFE_DELETE(_NewUFO);
			return E_FAIL;
		}

		
		// Actor 생성
 		if(FAILED(_NewUFO->CreateActor(NX_SHAPE_BOX,NX_BF_FROZEN_POS_Y|NX_BF_FROZEN_ROT,0))){
 			SAFE_DELETE(_NewUFO);
 			return E_FAIL;
 		}
 		// 충돌 이벤트리스트에 등록
 		SetCollEvent(_NewUFO);
 		m_l3DObjects.push_back(_NewUFO);
		
		for(int i=0 ; i<3 ; i++)
		{
			_TempRoam.x = _TempPos.x + GetRandomFloat(-40,40);
			_TempRoam.y = ay;
			_TempRoam.z = _TempPos.z +GetRandomFloat(-40,40);
			_NewUFO->SetRoamPos(_TempRoam);
		}

		// 오브젝트 리스트에 추가
		m_lAllObjects.push_back(_NewUFO);


	}else
	{
		O2Alien* _NewAli = new O2Alien(m_pHero);
		_NewAli->SetColor(COLOR_NONE);

		// 일렙스 타임 저장
		_NewAli->SetDelta(m_pfElaped);

		// 이펙트 사용여부와 이펙트 저장
		_NewAli->SetRendShadow(TRUE);
		_NewAli->SetShadowReflect(TRUE);
		_NewAli->SetUseCell(TRUE);
		_NewAli->SetEffect(m_pEffect);
		_NewAli->SetLineRend(TRUE);


		_TempPos.x+=GetRandomFloat(10,40);
		_TempPos.y+=GetRandomFloat(10,15);
		_TempPos.z+=GetRandomFloat(10,40);
		// 초기화
		if(num==0)
			tname = _T("AddAlien01");
		else if(num==1)
			tname = _T("AddAlien02");
		else
			tname = _T("AddAlien03");

		if(_stage==0)
			ay = 5.0f;
		else if(_stage==1)
			ay = 55.0f;
		else
			ay = 5.0f;

		_TempPos.y=ay;

		if(FAILED(_NewAli->Init(m_lpDevice,_T("ali.X"),tname,_TempPos,D3DXVECTOR3(1,1,1),D3DXVECTOR3(1,1,1)))){
			SAFE_DELETE(_NewAli);
			return E_FAIL;
		}

		printf("Here\n");
		// Actor 생성
		if(FAILED(_NewAli->CreateActor(NX_SHAPE_BOX,0))){
			SAFE_DELETE(_NewAli);
			return E_FAIL;
		}
		// 충돌 이벤트리스트에 등록
		SetCollEvent(_NewAli);
		m_l3DObjects.push_back(_NewAli);

		for(int i=0 ; i<5 ; i++)
		{
			_TempRoam.x = _TempPos.x + GetRandomFloat(-60,60);
			_TempRoam.y = ay;
			_TempRoam.z = _TempPos.z +GetRandomFloat(-60,60);
			_NewAli->SetRoamPos(_TempRoam);
		}

		// 오브젝트 리스트에 추가
		m_lAllObjects.push_back(_NewAli);
	}

	return S_OK;
}

float	O2ObjectManager::GetRandomFloat( float fMin, float fMax )
{
	float fRandNum = (float)rand () / RAND_MAX;
	return fMin + (fMax - fMin) * fRandNum;
}