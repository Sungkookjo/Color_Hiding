#include "stdafx.h"
#include "O23DObject.h"

//////////////////////////////////////////////////////////////////////////
//	3D Object
//////////////////////////////////////////////////////////////////////////
O23DObject::O23DObject()
:m_lpMesh(NULL)
,m_lpMeshMaterials(NULL)
,m_lpMeshTextures(NULL)
,m_dwNumMaterials(0)
,m_pActor(NULL)
,m_pEffect(NULL)
,m_iColor(COLOR_NONE)
,m_pSoundMgr(NULL)
,m_pfSetMat(NULL)
,m_lpShadowMesh(NULL)
{
	m_uType |= OBJ_TYPE_3D;
	m_pfSetMat = &O23DObject::SetMatWithDx;
}

O23DObject::~O23DObject()
{
	if(m_pActor)
		JNxScene->releaseActor(*m_pActor);
	m_pSoundMgr=NULL;
}

//////////////////////////////////////////////////////////////////////////
//	ChangeShapeSize
//	Shape의 사이즈 재변경.
//	현재 사이즈에 맞는 새로운 shape 를 생성.
VOID O23DObject::ChangeShapeSize()
{
	NxVec3 TempSize(0.0f);
	NxShapeDesc* Shape = NULL;

	if(m_pActor){
		// 현재 모양의 타입을 받아옴
		switch((*(m_pActor->getShapes()))->getType()){
			// 구일 경우 새로운 사이즈의 구를 생성
		case NX_SHAPE_SPHERE:
			Shape = new NxSphereShapeDesc;
			((NxSphereShapeDesc*)Shape)->radius = m_pColl._radius*m_vSize.x;
			break;
			// 박스일 경우
		case NX_SHAPE_BOX:
			Shape = new NxBoxShapeDesc;
			TempSize.x = max(abs(m_pColl._max.x),abs(m_pColl._min.x));
			TempSize.y = max(abs(m_pColl._max.y),abs(m_pColl._min.y));
			TempSize.z = max(abs(m_pColl._max.z),abs(m_pColl._min.z));

			TempSize *= m_vSize.x;
			TempSize.y *= 0.5f;

			((NxBoxShapeDesc*)Shape)->dimensions.set(TempSize.x,TempSize.y,TempSize.z);
			((NxBoxShapeDesc*)Shape)->localPose.t = NxVec3(0.0f,TempSize.y,0.0f);
			break;
			// 캡슐
		case NX_SHAPE_CAPSULE:
			break;
			// 휠(바퀴)
		case NX_SHAPE_WHEEL:
			break;
		default:
			return;
		}
		// 새로 추가할 모양의 user data 설정
		Shape->userData = this;
		// 이전 모양 해제
		m_pActor->releaseShape(*(*(m_pActor->getShapes())));
		// 새로 만든 모양 추가
		m_pActor->createShape(*Shape);

		// 동적할당 해제
		SAFE_DELETE(Shape);
	}
}

//////////////////////////////////////////////////////////////////////////
// Set
VOID O23DObject::SetPos( const D3DXVECTOR3& pPos )
{
	m_vPos = pPos;
	if(m_pActor){
		m_pActor->setGlobalPosition(DxVec3toNxVec3(m_vPos));
	}
}

VOID O23DObject::SetRot( const D3DXVECTOR3& pRot )
{
	// 각 회전값을 곱한 행렬로 재설정
	m_vRot = pRot;
	if(m_pActor){
		NxMat33 _mat,_temp;
		_mat.id();
		_temp.rotX(D3DXToRadian(m_vRot.x));
		_mat = _temp;
		_temp.rotY(D3DXToRadian(m_vRot.y));
		_mat *= _temp;
		_temp.rotZ(D3DXToRadian(m_vRot.z));
		_mat *= _temp;
		m_pActor->setGlobalOrientation(_mat);
	}else{
		SetMatRot();
	}
}

VOID O23DObject::SetSize( const D3DXVECTOR3& _size )
{
	m_vSize.x =	(_size.x < 0.1f ? 0.1f : _size.x );
	m_vSize.y =	(_size.y < 0.1f ? 0.1f : _size.y );
	m_vSize.z = (_size.z < 0.1f ? 0.1f : _size.z );
	D3DXMatrixScaling(&m_matSize,m_vSize.x,m_vSize.y,m_vSize.z);
	ChangeShapeSize();
}

VOID O23DObject::SetSize( const FLOAT& _size )
{
	m_vSize.x=m_vSize.y=m_vSize.z = (_size < 0.1f ? 0.1f : _size );
	D3DXMatrixScaling(&m_matSize,m_vSize.x,m_vSize.y,m_vSize.z);
	ChangeShapeSize();
}

VOID O23DObject::SetDir( const D3DXVECTOR3& pDir )
{
	m_vDir = pDir;
}

VOID O23DObject::SetUp( const D3DXVECTOR3& pUp )
{
	m_vUp = pUp;
}

VOID O23DObject::SetRight( const D3DXVECTOR3& pRight )
{
	m_vRight = pRight;
}
//////////////////////////////////////////////////////////////////////////
// SetMat
// 월드행렬 설정
VOID O23DObject::SetMat()
{
	(this->*m_pfSetMat)();

	// 부모행렬이 있는 경우 곱해준다.
	if(m_pParent){
		m_matWorld *= m_pParent->GetMatWorld();
	}

	// 월드행렬 설정
	(*m_lpDevice)->SetTransform( D3DTS_WORLD, &m_matWorld );
}

// Set
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Add
VOID O23DObject::AddPos( const D3DXVECTOR3& _pos )
{
	m_vPos += _pos;
	if(m_pActor)
		m_pActor->setGlobalPosition(DxVec3toNxVec3(m_vPos));
}
VOID O23DObject::AddRot( const D3DXVECTOR3& _rot )
{
	m_vRot += _rot;
	NxMat33 _mat,_temp;
	if(m_pActor){
		_mat.id();
		_temp.rotX(D3DXToRadian(_rot.x));
		_mat = _temp;
		_temp.rotY(D3DXToRadian(_rot.y));
		_mat *= _temp;
		_temp.rotZ(D3DXToRadian(_rot.z));
		_mat *= _temp;
		m_pActor->setGlobalOrientation(m_pActor->getGlobalPose().M * _mat);
	}else{
		SetMatRot();
	}
}
VOID O23DObject::AddSize( const FLOAT& _size )
{
	m_vSize.x += ( _size+m_vSize.x < SIZE_MIN ? 0.0f : _size );
	m_vSize.y += ( _size+m_vSize.y < SIZE_MIN ? 0.0f : _size );
	m_vSize.z += ( _size+m_vSize.z < SIZE_MIN ? 0.0f : _size );
	D3DXMatrixScaling(&m_matSize,m_vSize.x,m_vSize.y,m_vSize.z);
	ChangeShapeSize();
}

VOID O23DObject::AddSize( const D3DXVECTOR3& _size )
{
	SetSize(m_vSize + _size);
}

// Add
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CreateActor
// 물리 적용
HRESULT O23DObject::CreateActor( UINT _shapes,unsigned int _flag,BOOL _isstatic/*=FALSE*/ )
{
	if(JNxScene == NULL) false;

	NxShapeDesc*	shapeDesc = NULL;
	D3DXVECTOR3	TempSize;

	// Create body
	NxBodyDesc bodyDesc;
	bodyDesc.angularDamping	= 0.5f;	// 감속력
	bodyDesc.flags |= _flag;

	// 물리적 성질 모양을 설정
	switch(_shapes){
		case NX_SHAPE_BOX:
			shapeDesc = new NxBoxShapeDesc;
			TempSize.x = max(abs(m_pColl._max.x),abs(m_pColl._min.x));
			TempSize.y = max(abs(m_pColl._max.y),abs(m_pColl._min.y));
			TempSize.z = max(abs(m_pColl._max.z),abs(m_pColl._min.z));

			TempSize *= m_vSize.x;
			TempSize.y *= 0.5f;

			// 사각형 모양 생성
			((NxBoxShapeDesc*)shapeDesc)->dimensions.set(TempSize.x,TempSize.y,TempSize.z);
			((NxBoxShapeDesc*)shapeDesc)->localPose.t = NxVec3(0.0f,TempSize.y,0.0f);
			((NxBoxShapeDesc*)shapeDesc)->userData = this;
			break;
		case NX_SHAPE_SPHERE:
			shapeDesc = new NxSphereShapeDesc;
			((NxSphereShapeDesc*)shapeDesc)->radius = m_pColl._radius*m_vSize.x;
			((NxSphereShapeDesc*)shapeDesc)->userData = this;
			break;
		case NX_SHAPE_CAPSULE:
			shapeDesc = new NxCapsuleShapeDesc;
			((NxCapsuleShapeDesc*)shapeDesc)->radius = m_pColl._max.x * 0.5f;
			((NxCapsuleShapeDesc*)shapeDesc)->height = m_pColl._max.y-m_pColl._max.x;
			((NxCapsuleShapeDesc*)shapeDesc)->userData = this;
			((NxBoxShapeDesc*)shapeDesc)->localPose.t = NxVec3(0.0f,(m_pColl._max.y*0.5f),0.0f);
			break;		
		default:
			return E_FAIL;
	}

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(shapeDesc);
	actorDesc.userData			= this;
	if(FALSE == _isstatic){
			actorDesc.body			= &bodyDesc;
	}
	actorDesc.density		= 10.0f;
	actorDesc.globalPose.t  = DxVec3toNxVec3(m_vPos);

	m_pActor = JNxScene->createActor(actorDesc);

	SAFE_DELETE(shapeDesc);

	if( NULL == m_pActor)
		return E_FAIL;

	SetRot(m_vRot);
	m_pfSetMat = &O23DObject::SetMatWithActor;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// SetForce
// 오브젝트에 가해지는 힘을 설정
void O23DObject::SetForce( D3DXVECTOR3 pForce )
{
	if(m_pActor)
		m_pActor->setLinearVelocity(DxVec3toNxVec3(pForce));
}

//////////////////////////////////////////////////////////////////////////
// AddForce
// 오브젝트에 현재 가해지는 힘에 다른 힘을 더함
void O23DObject::AddForce( D3DXVECTOR3 pForce )
{
	if(m_pActor){
		m_pActor->addForce(DxVec3toNxVec3(pForce),NX_VELOCITY_CHANGE);
	}
}

HRESULT O23DObject::UpDate()
{
	UpdateNxtoDx();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// UpdateNxtoDx
// 물리객체(Actor)와 object class 의 동기화
VOID O23DObject::UpdateNxtoDx()
{
	if(m_pActor){
		m_vPos	=	NxVec3toDxVec3(m_pActor->getGlobalPosition());
	}
}



HRESULT O23DObject::RenderLine()
{

	// 스케일링 된 월드 행렬 설정
	(*m_lpDevice)->SetTransform( D3DTS_WORLD, &m_matLiner );


	// 재질 ( 색 )
	_D3DMATERIAL9 material;
	material.Diffuse.r = 1.0f;
	material.Diffuse.g = 1.0f;
	material.Diffuse.b = 1.0f;
	material.Diffuse.a = 1.0f;

	// 텍스처 비워줌
	(*m_lpDevice)->SetMaterial( &material );

	// 그리기
	for( DWORD i=0; i < m_dwNumMaterials; i++ )
	{
		(*m_lpMesh)->DrawSubset( i );
	}

	return S_OK;
}

void O23DObject::SetColor( UINT cl )
{
	if(m_iColor != cl){
		m_iColor = cl;
		m_fClWeight = 0.0f;
	}else{
		m_iColor = COLOR_NONE;
	}
}

D3DXVECTOR4 O23DObject::GetColor( D3DCOLORVALUE& pVec )
{
	D3DXVECTOR4 TempColor;
	TempColor.x = pVec.r;
	TempColor.y = pVec.g;
	TempColor.z = pVec.b;
	TempColor.w = pVec.a;
	if(m_iColor!= COLOR_NONE){
		m_fClWeight += 1.0f*(*m_fpDelta);
		if(m_fClWeight >= 1.0f){
			m_fClWeight = 1.0f;
		}
	}
	switch(m_iColor)
	{
	case COLOR_R:
		TempColor -= COLOR_R_VAL;
		break;
	case COLOR_G:			
		TempColor -= COLOR_G_VAL;
		break;
	case COLOR_B:
		TempColor -= COLOR_B_VAL;
		break;
	case COLOR_P:
		TempColor -= COLOR_P_VAL;
		break;
	case COLOR_NONE:
		return TempColor;
	}
	TempColor = D3DXVECTOR4(1.0f,1.0f,1.0f,1.0f) - (TempColor * m_fClWeight);

	return TempColor;
}

VOID O23DObject::SetMatWithActor()
{
	// 오브젝트의 회전/이동 행렬 받아올 변수
	FLOAT RT[16];

	// Actor가 있는지(물리를 적용하는지) 검사
	// 회전 행렬
	m_pActor->getGlobalPose().M.getColumnMajorStride4(RT);
	m_matRot = D3DXMATRIXA16(RT);

	// 변환된 회전 행렬에서 각 방향 벡터를 구함
	D3DXVec3TransformCoord(&m_vDir	,&D3DXVECTOR3(0.0f,0.0f,1.0f),&m_matRot);
	D3DXVec3TransformCoord(&m_vUp	,&D3DXVECTOR3(0.0f,1.0f,0.0f),&m_matRot);
	D3DXVec3TransformCoord(&m_vRight,&D3DXVECTOR3(1.0f,0.0f,0.0f),&m_matRot);

	// 회전 * 이동 행렬
	m_pActor->getGlobalPose().getColumnMajor44(RT);

	// 회전 * 이동 * Invers(회전) = 이동 행렬
	m_matRT = D3DXMATRIXA16(RT);
	D3DXMatrixInverse(&m_matTrans,NULL,&m_matRot);
	m_matTrans = m_matRT * m_matTrans;

	//////////////////////////////////////////////////////////////////////////
	// 임시 외곽
	D3DXMatrixScaling(&m_matLiner,m_vSize.x*LINE_SCAIL,m_vSize.y*LINE_SCAIL_Y,m_vSize.z*LINE_SCAIL);
	m_matLiner = m_matLiner * m_matRT;
	//////////////////////////////////////////////////////////////////////////

	// 사이즈 * 회전 * 이동 행렬 = 월드 행렬
	m_matWorld = m_matSize * m_matRT;
}

VOID O23DObject::SetMatWithDx()
{
	D3DXMatrixIdentity( &m_matWorld );
	D3DXMatrixTranslation(&m_matTrans,m_vPos.x,m_vPos.y,m_vPos.z);
	m_matRT = m_matRot * m_matTrans;
	m_matWorld = m_matSize * m_matRT;
	//////////////////////////////////////////////////////////////////////////
	// 임시 외곽
	D3DXMatrixScaling(&m_matLiner,m_vSize.x*LINE_SCAIL,m_vSize.y*LINE_SCAIL_Y,m_vSize.z*LINE_SCAIL);
	m_matLiner = m_matLiner * m_matRT;
	//////////////////////////////////////////////////////////////////////////
}

BOOL O23DObject::IsRender( O2Frustum* pFrustrum )
{
	m_bIsRend = pFrustrum->IsInSphere(&m_vPos,m_pColl._radius);
	return m_bIsRend;
}
