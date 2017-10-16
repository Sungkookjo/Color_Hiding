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
// ������ SDK ������ Scene ���� + cooking ����
HRESULT NxSDK::Init()
{
	// PhysicsSDK ����
	m_pPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION);
	if(m_pPhysicsSDK == NULL) 
	{
		printf("Failed-NxCreatePhysicsSDK\n");
		return E_FAIL;
	}

	// Cooking ����
	m_pCooking = NxGetCookingLib(NX_PHYSICS_SDK_VERSION);

	if( NULL == m_pCooking )
		return E_FAIL;

	bool status = m_pCooking->NxInitCooking(NULL,NULL);
	if (!status) {
		printf("Unable to initialize the cooking library. Please make sure that you have correctly installed the latest version of the AGEIA PhysX SDK.");
		exit(1);
	}

	m_pPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.001f);				// ���Ƿ�(�� ������ ���̴� �浹 X)


	// scene ����
	NxSceneDesc sceneDesc;
	sceneDesc.gravity				= NxVec3(0.0f, -9.81f, 0.0f); // �߷�

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
	defaultMaterial->setRestitution(0.0f);		// ������
	defaultMaterial->setStaticFriction(0.5f);	// ���� ������
	defaultMaterial->setDynamicFriction(0.5f);	// � ������

	// �̺�Ʈ ó��
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
// ������ ¦���� ����� �̺�Ʈ �߻��� ���ư��� �ݹ� �Լ�
void O2ContactReport::onContactNotify( NxContactPair& pair, NxU32 events ){
	NxContactStreamIterator i(pair.stream);
	
	while(i.goNextPair()){
		while(i.goNextPatch()){
			// �浹 �̺�Ʈ��
			if( NX_NOTIFY_ON_TOUCH == events){
				// ������Ʈ �޾ƿ�
				O23DObject* Left = (O23DObject*)i.getShape(0)->userData;
				O23DObject* Right = (O23DObject*)i.getShape(1)->userData;
				// �浹 �Լ� ����
				Left->CollEffect(Right);
				Right->CollEffect(Left);
			}
		}
	}
}
																		

																		