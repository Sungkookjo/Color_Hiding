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
//	Shape�� ������ �纯��.
//	���� ����� �´� ���ο� shape �� ����.
VOID O23DObject::ChangeShapeSize()
{
	NxVec3 TempSize(0.0f);
	NxShapeDesc* Shape = NULL;

	if(m_pActor){
		// ���� ����� Ÿ���� �޾ƿ�
		switch((*(m_pActor->getShapes()))->getType()){
			// ���� ��� ���ο� �������� ���� ����
		case NX_SHAPE_SPHERE:
			Shape = new NxSphereShapeDesc;
			((NxSphereShapeDesc*)Shape)->radius = m_pColl._radius*m_vSize.x;
			break;
			// �ڽ��� ���
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
			// ĸ��
		case NX_SHAPE_CAPSULE:
			break;
			// ��(����)
		case NX_SHAPE_WHEEL:
			break;
		default:
			return;
		}
		// ���� �߰��� ����� user data ����
		Shape->userData = this;
		// ���� ��� ����
		m_pActor->releaseShape(*(*(m_pActor->getShapes())));
		// ���� ���� ��� �߰�
		m_pActor->createShape(*Shape);

		// �����Ҵ� ����
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
	// �� ȸ������ ���� ��ķ� �缳��
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
// ������� ����
VOID O23DObject::SetMat()
{
	(this->*m_pfSetMat)();

	// �θ������ �ִ� ��� �����ش�.
	if(m_pParent){
		m_matWorld *= m_pParent->GetMatWorld();
	}

	// ������� ����
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
// ���� ����
HRESULT O23DObject::CreateActor( UINT _shapes,unsigned int _flag,BOOL _isstatic/*=FALSE*/ )
{
	if(JNxScene == NULL) false;

	NxShapeDesc*	shapeDesc = NULL;
	D3DXVECTOR3	TempSize;

	// Create body
	NxBodyDesc bodyDesc;
	bodyDesc.angularDamping	= 0.5f;	// ���ӷ�
	bodyDesc.flags |= _flag;

	// ������ ���� ����� ����
	switch(_shapes){
		case NX_SHAPE_BOX:
			shapeDesc = new NxBoxShapeDesc;
			TempSize.x = max(abs(m_pColl._max.x),abs(m_pColl._min.x));
			TempSize.y = max(abs(m_pColl._max.y),abs(m_pColl._min.y));
			TempSize.z = max(abs(m_pColl._max.z),abs(m_pColl._min.z));

			TempSize *= m_vSize.x;
			TempSize.y *= 0.5f;

			// �簢�� ��� ����
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
// ������Ʈ�� �������� ���� ����
void O23DObject::SetForce( D3DXVECTOR3 pForce )
{
	if(m_pActor)
		m_pActor->setLinearVelocity(DxVec3toNxVec3(pForce));
}

//////////////////////////////////////////////////////////////////////////
// AddForce
// ������Ʈ�� ���� �������� ���� �ٸ� ���� ����
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
// ������ü(Actor)�� object class �� ����ȭ
VOID O23DObject::UpdateNxtoDx()
{
	if(m_pActor){
		m_vPos	=	NxVec3toDxVec3(m_pActor->getGlobalPosition());
	}
}



HRESULT O23DObject::RenderLine()
{

	// �����ϸ� �� ���� ��� ����
	(*m_lpDevice)->SetTransform( D3DTS_WORLD, &m_matLiner );


	// ���� ( �� )
	_D3DMATERIAL9 material;
	material.Diffuse.r = 1.0f;
	material.Diffuse.g = 1.0f;
	material.Diffuse.b = 1.0f;
	material.Diffuse.a = 1.0f;

	// �ؽ�ó �����
	(*m_lpDevice)->SetMaterial( &material );

	// �׸���
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
	// ������Ʈ�� ȸ��/�̵� ��� �޾ƿ� ����
	FLOAT RT[16];

	// Actor�� �ִ���(������ �����ϴ���) �˻�
	// ȸ�� ���
	m_pActor->getGlobalPose().M.getColumnMajorStride4(RT);
	m_matRot = D3DXMATRIXA16(RT);

	// ��ȯ�� ȸ�� ��Ŀ��� �� ���� ���͸� ����
	D3DXVec3TransformCoord(&m_vDir	,&D3DXVECTOR3(0.0f,0.0f,1.0f),&m_matRot);
	D3DXVec3TransformCoord(&m_vUp	,&D3DXVECTOR3(0.0f,1.0f,0.0f),&m_matRot);
	D3DXVec3TransformCoord(&m_vRight,&D3DXVECTOR3(1.0f,0.0f,0.0f),&m_matRot);

	// ȸ�� * �̵� ���
	m_pActor->getGlobalPose().getColumnMajor44(RT);

	// ȸ�� * �̵� * Invers(ȸ��) = �̵� ���
	m_matRT = D3DXMATRIXA16(RT);
	D3DXMatrixInverse(&m_matTrans,NULL,&m_matRot);
	m_matTrans = m_matRT * m_matTrans;

	//////////////////////////////////////////////////////////////////////////
	// �ӽ� �ܰ�
	D3DXMatrixScaling(&m_matLiner,m_vSize.x*LINE_SCAIL,m_vSize.y*LINE_SCAIL_Y,m_vSize.z*LINE_SCAIL);
	m_matLiner = m_matLiner * m_matRT;
	//////////////////////////////////////////////////////////////////////////

	// ������ * ȸ�� * �̵� ��� = ���� ���
	m_matWorld = m_matSize * m_matRT;
}

VOID O23DObject::SetMatWithDx()
{
	D3DXMatrixIdentity( &m_matWorld );
	D3DXMatrixTranslation(&m_matTrans,m_vPos.x,m_vPos.y,m_vPos.z);
	m_matRT = m_matRot * m_matTrans;
	m_matWorld = m_matSize * m_matRT;
	//////////////////////////////////////////////////////////////////////////
	// �ӽ� �ܰ�
	D3DXMatrixScaling(&m_matLiner,m_vSize.x*LINE_SCAIL,m_vSize.y*LINE_SCAIL_Y,m_vSize.z*LINE_SCAIL);
	m_matLiner = m_matLiner * m_matRT;
	//////////////////////////////////////////////////////////////////////////
}

BOOL O23DObject::IsRender( O2Frustum* pFrustrum )
{
	m_bIsRend = pFrustrum->IsInSphere(&m_vPos,m_pColl._radius);
	return m_bIsRend;
}
