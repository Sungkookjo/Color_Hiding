#include "stdafx.h"
#include "O23DObject.h"
NxSDK* NxSDK::m_pSingle = NULL;

NxSDK::NxSDK()
:m_pPhysicsSDK(NULL)
,m_pScene(NULL)
,m_pCooking(NULL)
{
	if(NULL == m_pSingle)
		m_pSingle = this;
}

NxSDK::~NxSDK()
{
	CleanUp();
	if(this == m_pSingle)
		m_pSingle = NULL;
}
//////////////////////////////////////////////////////////////////////////
// Init
// 피직스 SDK 생성및 Scene 생성 + cooking 생성
HRESULT NxSDK::Init()
{
	// PhysicsSDK 생성
	m_pPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION);
	if(m_pPhysicsSDK == NULL) 
	{
		printf("Failed-NxCreatePhysicsSDK\n");
		return E_FAIL;
	}

	// Cooking 생성
	m_pCooking = NxGetCookingLib(NX_PHYSICS_SDK_VERSION);

	if( NULL == m_pCooking )
		return E_FAIL;

	bool status = m_pCooking->NxInitCooking(NULL,NULL);
	if (!status) {
		printf("Unable to initialize the cooking library. Please make sure that you have correctly installed the latest version of the AGEIA PhysX SDK.");
		exit(1);
	}

	m_pPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.001f);				// 엡실론(이 이하의 차이는 충돌 X)


	// scene 생성
	NxSceneDesc sceneDesc;
	sceneDesc.gravity				= NxVec3(0.0f, -9.81f, 0.0f); // 중력

	// Enable Active Transform Notification
	sceneDesc.flags |= NX_SF_ENABLE_ACTIVETRANSFORMS;

	m_pScene = m_pPhysicsSDK->createScene(sceneDesc);
	if(m_pScene == NULL) 
	{
		printf("Failed-createScene\n");
		return E_FAIL;
	}

	// Set default material
	NxMaterial* defaultMaterial = m_pScene->getMaterialFromIndex(0);
	defaultMaterial->setRestitution(0.0f);		// 복원력
	defaultMaterial->setStaticFriction(0.5f);	// 정지 마찰력
	defaultMaterial->setDynamicFriction(0.5f);	// 운동 마찰력

	// 이벤트 처리
	m_pScene->setUserContactReport(&m_ContReport);

	return S_OK;
}

void NxSDK::CleanUp()
{

	if(m_pPhysicsSDK != NULL)
	{
		if(m_pCooking)
			m_pCooking->NxCloseCooking();

		if(m_pScene != NULL) m_pPhysicsSDK->releaseScene(*m_pScene);
		m_pScene = NULL;
		NxReleasePhysicsSDK(m_pPhysicsSDK);
		m_pPhysicsSDK = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
// onContactNotify
// 지정한 짝끼리 예약된 이벤트 발생시 돌아가는 콜백 함수
void O2ContactReport::onContactNotify( NxContactPair& pair, NxU32 events ){
	NxContactStreamIterator i(pair.stream);
	
	while(i.goNextPair()){
		while(i.goNextPatch()){
			// 충돌 이벤트시
			if( NX_NOTIFY_ON_TOUCH == events){
				// 오브젝트 받아옴
				O23DObject* Left = (O23DObject*)i.getShape(0)->userData;
				O23DObject* Right = (O23DObject*)i.getShape(1)->userData;
				// 충돌 함수 실행
				Left->CollEffect(Right);
				Right->CollEffect(Left);
			}
		}
	}
}
																		

																		