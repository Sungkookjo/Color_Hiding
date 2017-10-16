#include "stdafx.h"
#include "O2PassiveObject.h"

O23DPassiveObject::O23DPassiveObject()
{
	m_bRendShadow = TRUE;
	m_uType |= OBJ_TYPE_PASSIVE;
}

O23DPassiveObject::~O23DPassiveObject()
{
	CleanUp();
}

//////////////////////////////////////////////////////////////////////////
//	Passive Object
//////////////////////////////////////////////////////////////////////////
VOID O23DPassiveObject::CleanUp()
{

}

HRESULT O23DPassiveObject::LoadXfile( TCHAR* _filename )
{
	O2Mesh*	pMesh		= s_pResource->GetMesh(_filename);
	m_lpMesh			= pMesh->GetMesh();
	m_lpMeshMaterials	= pMesh->GetMaterials();
	m_lpMeshTextures	= pMesh->GetTextures();
	m_dwNumMaterials	= pMesh->GetNumMaterial();
	m_pColl				= pMesh->GetColl();
	m_uNumFaces			= pMesh->GetNumFaces();
	m_uNumVertices		= pMesh->GetNumVertices();
	m_lpShadowMesh		= pMesh->GetShadowMesh();


	return S_OK;
}

HRESULT O23DPassiveObject::UpDate()
{
	UpdateNxtoDx();
	return S_OK;
}

HRESULT O23DPassiveObject::Render()
{
	// 월드행렬 적용
	SetMat();
	m_pEffect->BeginPass(0);
	// 오브젝트 회전 * 이동 행렬 ( 노말 벡터 변환 )
	m_pEffect->SetMatrix(EFFECT_MAT_RT,m_matRT);
	// 오브젝트 전체 횡렬 ( 버텍스 좌표 이동 )
	m_pEffect->SetMatrix(EFFECT_MAT_WORLD,m_matWorld);

	for( DWORD i=0; i < m_dwNumMaterials; i++ )
	{
		// 오브젝트 색 설정		
		m_pEffect->SetVector(EFFECT_VEC_COLOR,((*m_lpMeshMaterials)[i]).Diffuse);
		// 본래 텍스처 맵핑
		m_pEffect->SetTexture(EFFECT_TEX_TEX1,m_lpMeshTextures[i]);
		m_pEffect->Commit();

		// Draw the mesh subset
		(*m_lpMesh)->DrawSubset( i );
	}

	m_pEffect->EndPass();

	return S_OK;
}

HRESULT O23DPassiveObject::CollEffect( O2Object* hit )
{
	return S_OK;
}

HRESULT O23DPassiveObject::Init( LPDIRECT3DDEVICE9* lpDev ,TCHAR* _filename ,TCHAR* _objname ,D3DXVECTOR3 pPos,D3DXVECTOR3 pSize,D3DXVECTOR3 pRot)
{
	m_lpDevice = lpDev;
	m_vPos = pPos;
	m_vSize = pSize;
	m_vRot = pRot;
	SetFileName(_filename);
	_objname ? SetName(_objname) : NULL ;
	LoadXfile(_filename);
	SetRot(m_vRot);
	D3DXMatrixScaling(&m_matSize,m_vSize.x,m_vSize.y,m_vSize.z);
	return S_OK;
}

HRESULT O23DPassiveObject::CreateActor( UINT _shapes,unsigned int _flag, BOOL _isstatic/*=FALSE*/ )
{
	if( NX_SHAPE_MESH != _shapes){
		return O23DObject::CreateActor(_shapes,_flag,_isstatic);
	}

	return GenerateTriangleMeshFromDXMesh(_flag, _isstatic);
}

HRESULT O23DPassiveObject::GenerateTriangleMeshFromDXMesh(unsigned int _flag,BOOL _isstatic)
{
	DWORD FVFSize = D3DXGetFVFVertexSize((*m_lpMesh)->GetFVF());
	D3DXMATRIXA16	tempMat;	//	임시 스케일링 행렬
	D3DXVECTOR4		tempPos;	//	임시 스케일링된 버텍스 위치
	NxVec3* verts = new NxVec3[m_uNumVertices];
	char *DXMeshPtr;

	// 버텍스 정보 복사
	(*m_lpMesh)->LockVertexBuffer(D3DLOCK_READONLY, (void**)&DXMeshPtr);
	if( m_vSize.x <= 0.99f || m_vSize.y <= 0.99f || m_vSize.z <= 0.99f ){
		// 스케일 행렬 생성
		D3DXMatrixScaling(&tempMat,m_vSize.x,m_vSize.y,m_vSize.z);
		for(int i = 0; i < m_uNumVertices; i++)
		{		
			Mesh_FVF *DXMeshFVF = (Mesh_FVF*)DXMeshPtr;
			// 스케일 행렬에 버텍스 위치 변환
			D3DXVec3Transform(&tempPos,&DXMeshFVF->VertexPos,&tempMat);
			verts[i] = NxVec3(tempPos.x, tempPos.y, tempPos.z);
			DXMeshPtr += FVFSize;
		}
	}else{
		for(int i = 0; i < m_uNumVertices; i++)
		{		
			Mesh_FVF *DXMeshFVF = (Mesh_FVF*)DXMeshPtr;
			verts[i] = NxVec3(DXMeshFVF->VertexPos.x, DXMeshFVF->VertexPos.y, DXMeshFVF->VertexPos.z);
			DXMeshPtr += FVFSize;
		}
	}
	(*m_lpMesh)->UnlockVertexBuffer();		

	// 인덱스 정보 복사
	IndexBufferStruct *tris = new IndexBufferStruct[m_uNumFaces];
	IndexBufferStruct *DXMeshIBPtr;
	(*m_lpMesh)->LockIndexBuffer(D3DLOCK_READONLY, (void**)&DXMeshIBPtr);

	for(int NumInd = 0; NumInd < m_uNumFaces; NumInd++)
	{		
		tris[NumInd].IBNumber[0] = DXMeshIBPtr[NumInd].IBNumber[0];	
		tris[NumInd].IBNumber[1] = DXMeshIBPtr[NumInd].IBNumber[1];	
		tris[NumInd].IBNumber[2] = DXMeshIBPtr[NumInd].IBNumber[2];
	}

	(*m_lpMesh)->UnlockIndexBuffer();


	// 메쉬 정보 설정
	NxTriangleMeshDesc TriMeshDesc;
	TriMeshDesc.numVertices = m_uNumVertices;
	TriMeshDesc.numTriangles = m_uNumFaces;
	TriMeshDesc.pointStrideBytes = sizeof(NxVec3);
	TriMeshDesc.triangleStrideBytes = 3*sizeof(NxU16);
	TriMeshDesc.points = verts;
	TriMeshDesc.triangles = tris;
	TriMeshDesc.flags = NX_MF_16_BIT_INDICES;

	NxTriangleMeshShapeDesc ShapeDesc;
	NxInitCooking();

	// Nx메쉬 생성
	MemoryWriteBuffer buf;	
	if(!NxCookTriangleMesh(TriMeshDesc, buf)) {
		return E_FAIL;
	}
	ShapeDesc.meshData = JNxSDK->createTriangleMesh(MemoryReadBuffer(buf.data));
	ShapeDesc.userData = this;
	

	// Actor 생성
	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&ShapeDesc);
	if(FALSE == _isstatic){
		// Create body
		//NxBodyDesc bodyDesc;
		//bodyDesc.angularDamping	= 0.5f;	// 감속력
		//bodyDesc.flags |= _flag;

		//actorDesc.body			= &bodyDesc;
		//actorDesc.density		= 10.0f;
	}
	actorDesc.globalPose.t = DxVec3toNxVec3(m_vPos);
	m_pActor = JNxScene->createActor(actorDesc);
	
	m_pActor->userData = this;

	// 회전 변환
	SetRot(m_vRot);

	SAFE_DELETE_ARRAY(verts);
	SAFE_DELETE_ARRAY(tris);


	m_pfSetMat = &O23DPassiveObject::SetMatWithActor;
	return S_OK;
}

HRESULT O23DPassiveObject::RenderShadow(UINT pass)
{
	if(!m_bRendShadow){
		return S_OK;
	}

	// 오브젝트 회전 * 이동 행렬 ( 노말 벡터 변환 )
	m_pEffect->SetMatrix(EFFECT_MAT_RT,m_matRT);
	// 오브젝트 전체 횡렬 ( 버텍스 좌표 이동 )
	m_pEffect->SetMatrix(EFFECT_MAT_WORLD,m_matWorld);

	for(UINT Cpass = 0;Cpass < pass;++Cpass)
	{
		m_pEffect->BeginPass(Cpass);
		m_pEffect->Commit();
		(*m_lpShadowMesh)->DrawSubset( 0 );
		m_pEffect->EndPass();
	}

	return S_OK;
}