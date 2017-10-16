#include "stdafx.h"
#include "O2ActiveObject.h"

//////////////////////////////////////////////////////////////////////////
// ������
O23DActiveObject::O23DActiveObject()
:m_fSpeed(0.0f)
,m_pFrameRoot(NULL)
,m_pAnimController(NULL)
,m_pBoneMatrices(NULL)
,m_pLineBoneMatrices(NULL)
,m_NumBoneMatricesMax(0)
,m_dwCurTrack(0)
,m_dwMaxTracks(0)
,m_fCurTime(0.0f)
,m_iCount(ANIM_LOOP)
{
	m_uType |= OBJ_TYPE_ACTIVE;
}

//////////////////////////////////////////////////////////////////////////
// �Ҹ���
O23DActiveObject::~O23DActiveObject()
{
	CleanUp();
}

//////////////////////////////////////////////////////////////////////////
//	����
VOID O23DActiveObject::CleanUp()
{
	SAFE_RELEASE(m_pAnimController);
	SAFE_DELETE_ARRAY(m_pBoneMatrices);
	SAFE_DELETE_ARRAY(m_pLineBoneMatrices);
}

//////////////////////////////////////////////////////////////////////////
// LoadXfile
// ���ҽ� �ҷ���
HRESULT O23DActiveObject::LoadXfile( TCHAR* _filename )
{
	O2Mesh*	pMesh	= NULL;
	O2ActiveMesh* pActive = NULL;
	// �⺻ �޽� ����
	pMesh = s_pResource->GetMesh(_filename);
	if(NULL == pMesh){
		_DEBUG_LOG(_T("Failed-GetMesh() [%s]"),_filename);
		return E_FAIL;
	}

	m_lpMeshMaterials	= pMesh->GetMaterials();
	m_lpMeshTextures	= pMesh->GetTextures();
	m_dwNumMaterials	= pMesh->GetNumMaterial();
	m_pColl				= pMesh->GetColl();

	pActive					= s_pResource->GetActiveMesh(_filename);
	if(NULL == pActive){
		_DEBUG_LOG(_T("Failed-GetActiveMesh() [%s]"),_filename);
		return E_FAIL;
	}

	m_lpMesh				= pActive->GetMesh();
	m_lpShadowMesh			= pActive->GetShadowMesh();
	m_NumBoneMatricesMax	= pActive->GetBoneNum();
	m_pAnimController		= pActive->GetAniContr();
	m_pFrameRoot			= pActive->GetFrame();
	m_dwMaxTracks			= pActive->GetMaxTrack();

	for( UINT i = 0; i < m_dwMaxTracks; ++ i ){
		m_pAnimController->SetTrackEnable( i, FALSE );
	}

	m_pBoneMatrices = new D3DXMATRIXA16[m_NumBoneMatricesMax];
	m_pLineBoneMatrices = new D3DXMATRIXA16[m_NumBoneMatricesMax];

	return S_OK;
}

HRESULT O23DActiveObject::UpDate()
{
	UpdateNxtoDx();
	
	m_pAnimController->AdvanceTime( (*m_fpDelta), NULL );

	// �ִ� once �˻�
	if( ANIM_LOOP != m_iCount ){
		// �ִϰ� ����ߵ� �ð��� ���� �ð� üũ
		double lerp = m_fCurTime - m_lfDestTime;
		if( m_lfAniTime <=  lerp){
			// ī��Ʈ ����
			++m_iCurCount;
			// �ִ� �˻� �ð��� ���� �ð� - ���� �ð�
			m_lfDestTime = m_fCurTime - (lerp - m_lfAniTime);
			if( m_iCurCount >= m_iCount ){
				StopAnim();
			}
		}
	}

	m_fPreTime = m_fCurTime;
	m_fCurTime += (*m_fpDelta);

	return S_OK;
}

HRESULT O23DActiveObject::Render()
{
	// ������� ����
	SetMat();
	
	m_pAnimController->AdvanceTime( 0.0f, NULL );

	// ������ ��� ����
	UpdateFrameMatrices( *m_pFrameRoot, &m_matWorld );

	m_pEffect->BeginPass(0);
	// ������Ʈ ȸ�� * �̵� ��� ( �븻 ���� ��ȯ )
	//m_pEffect->SetMatrix(EFFECT_MAT_RT,m_matRT);
	// ������Ʈ ��ü Ⱦ�� ( ���ؽ� ��ǥ �̵� )
	m_pEffect->SetMatrix(EFFECT_MAT_WORLD,m_matWorld);

	DrawFrame( *m_pFrameRoot );

	
	m_pEffect->EndPass();

	return S_OK;
}

HRESULT O23DActiveObject::CollEffect( O2Object* hit )
{
	return S_OK;
}
HRESULT O23DActiveObject::Init( LPDIRECT3DDEVICE9* lpDev ,TCHAR* _filename ,TCHAR* _objname ,D3DXVECTOR3 pPos,D3DXVECTOR3 pSize,D3DXVECTOR3 pRot)
{
	m_lpDevice = lpDev;
	m_vPos = pPos;
	m_vSize = pSize;
	m_vRot = pRot;
	SetFileName(_filename);
	_objname ? SetName(_objname) : NULL ;
	if(FAILED(LoadXfile(_filename)))
		return E_FAIL;
	D3DXMatrixScaling(&m_matSize,m_vSize.x,m_vSize.y,m_vSize.z);
	PlayAnim(ANIM_LOOP);
	SetMatRot();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// ������Ʈ�� ������ ���� �Լ�
VOID O23DActiveObject::MoveFront(float _increement)
{
	if(COLOR_NONE == m_iColor)
		AddPos(m_vDir * m_fSpeed * _increement * (*m_fpDelta));
	else
		AddPos(m_vDir * m_fSpeed * 0.8f * _increement * (*m_fpDelta));
}

VOID O23DActiveObject::MoveBack(float _increement)
{
	if(COLOR_NONE == m_iColor)
		AddPos(-m_vDir * m_fSpeed * MOVE_DIGREE_BACK * (*m_fpDelta) * _increement);
	else
		AddPos(-m_vDir * m_fSpeed * 0.8f * MOVE_DIGREE_BACK * (*m_fpDelta) * _increement);
	
}

VOID O23DActiveObject::MoveLeft(float _increement)
{
	if(COLOR_NONE == m_iColor)
		AddPos(-m_vRight * m_fSpeed * (*m_fpDelta) * MOVE_DIGREE_SIDE * _increement);
	else
		AddPos(-m_vRight * m_fSpeed*0.8f * (*m_fpDelta) * MOVE_DIGREE_SIDE * _increement);
}

VOID O23DActiveObject::MoveRight(float _increement)
{
	if(COLOR_NONE == m_iColor)
		AddPos(m_vRight * m_fSpeed * (*m_fpDelta) * MOVE_DIGREE_SIDE * _increement);
	else
		AddPos(m_vRight * m_fSpeed*0.8f * (*m_fpDelta) * MOVE_DIGREE_SIDE * _increement);
}

VOID O23DActiveObject::MoveDown()
{
	AddPos(-m_vUp * m_fSpeed * (*m_fpDelta));
}

VOID O23DActiveObject::MoveUp(float _incree)
{
	AddForce(m_vUp * 9.8f * _incree);
}

VOID O23DActiveObject::TurnLeft()
{
	RotY(-30.0f * (*m_fpDelta));
}

VOID O23DActiveObject::TurnRight()
{
	RotY(30.0f * (*m_fpDelta));
}

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// UpdateFrameMatrices
// �� �������� ������Ʈ ���� ��� ������Ʈ
//////////////////////////////////////////////////////////////////////////
void O23DActiveObject::UpdateFrameMatrices( LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix )
{
	D3DXFRAME_DERIVED* pFrame = ( D3DXFRAME_DERIVED* )pFrameBase;

	if( pParentMatrix != NULL )
		D3DXMatrixMultiply( &pFrame->CombinedTransformationMatrix, &pFrame->TransformationMatrix, pParentMatrix );
	else
		pFrame->CombinedTransformationMatrix = pFrame->TransformationMatrix;

	if( pFrame->pFrameSibling != NULL )
	{
		UpdateFrameMatrices( pFrame->pFrameSibling, pParentMatrix );
	}

	if( pFrame->pFrameFirstChild != NULL )
	{
		UpdateFrameMatrices( pFrame->pFrameFirstChild, &pFrame->CombinedTransformationMatrix );
	}
}

//////////////////////////////////////////////////////////////////////////
// UpdateSkinningMethod
// ��Ű�� ������ �����Ѵ�.
//////////////////////////////////////////////////////////////////////////
void O23DActiveObject::UpdateSkinningMethod( LPD3DXFRAME pFrameBase )
{
	D3DXFRAME_DERIVED* pFrame = ( D3DXFRAME_DERIVED* )pFrameBase;
	D3DXMESHCONTAINER_DERIVED* pMeshContainer;

	pMeshContainer = ( D3DXMESHCONTAINER_DERIVED* )pFrame->pMeshContainer;

	while( pMeshContainer != NULL )
	{
		GenerateSkinnedMesh( (*m_lpDevice), pMeshContainer );

		pMeshContainer = ( D3DXMESHCONTAINER_DERIVED* )pMeshContainer->pNextMeshContainer;
	}

	if( pFrame->pFrameSibling != NULL )
	{
		UpdateSkinningMethod( pFrame->pFrameSibling );
	}

	if( pFrame->pFrameFirstChild != NULL )
	{
		UpdateSkinningMethod( pFrame->pFrameFirstChild );
	}
}
//////////////////////////////////////////////////////////////////////////
// DrawFrame
// �������� �׸���.
//////////////////////////////////////////////////////////////////////////
void O23DActiveObject::DrawFrame( LPD3DXFRAME pFrame )
{
	LPD3DXMESHCONTAINER pMeshContainer;

	pMeshContainer = pFrame->pMeshContainer;
	while( pMeshContainer != NULL )
	{
		DrawMeshContainer( pMeshContainer, pFrame );

		pMeshContainer = pMeshContainer->pNextMeshContainer;
	}

	if( pFrame->pFrameSibling != NULL )
	{
		DrawFrame( pFrame->pFrameSibling );
	}

	if( pFrame->pFrameFirstChild != NULL )
	{
		DrawFrame( pFrame->pFrameFirstChild );
	}
}
//////////////////////////////////////////////////////////////////////////
//	DrawMeshContainer
//	�޽��� �׸���.
//////////////////////////////////////////////////////////////////////////
HRESULT O23DActiveObject::DrawMeshContainer( LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase )
{
	D3DXMESHCONTAINER_DERIVED* pMeshContainer = ( D3DXMESHCONTAINER_DERIVED* )pMeshContainerBase;
	D3DXFRAME_DERIVED* pFrame = ( D3DXFRAME_DERIVED* )pFrameBase;
	UINT iMaterial;
	UINT iAttrib;

	D3DXMATRIXA16 matTemp;
	D3DCAPS9 d3dCaps;
	(*m_lpDevice)->GetDeviceCaps( &d3dCaps );

	// ��Ű�� ������ Ȯ��
	if( pMeshContainer->pSkinInfo != NULL )
	{
		D3DXMATRIXA16 Identity;
		DWORD cBones = pMeshContainer->pSkinInfo->GetNumBones();
		DWORD iBone;
		PBYTE pbVerticesSrc;
		PBYTE pbVerticesDest;

		// �� ����� ���Ѵ�.
		for( iBone = 0; iBone < cBones; ++iBone )
		{
			D3DXMatrixMultiply
				(
				&m_pBoneMatrices[iBone],                 // output
				&pMeshContainer->pBoneOffsetMatrices[iBone],
				pMeshContainer->ppBoneMatrixPtrs[iBone]
			);
		}

		// set world transform
		if(NULL != m_pParent){
			Identity = m_pParent->GetMatWorld();
		}else{
			D3DXMatrixIdentity( &Identity );
		}
		if(m_pEffect){
			m_pEffect->SetMatrix( EFFECT_MAT_WORLD, Identity );
		}else{
			(*m_lpDevice)->SetTransform( D3DTS_WORLD, &Identity );
		}
		

		// ���� �׷��� ���ؽ� ������ ���� �������� ���ؽ� ������ ������ ������ ���ؽ� ������ ����
		if(FAILED(( pMeshContainer->pOrigMesh->LockVertexBuffer( D3DLOCK_READONLY, ( LPVOID* )&pbVerticesSrc ) )))
			return E_FAIL;
		if(FAILED(( pMeshContainer->MeshData.pMesh->LockVertexBuffer( 0, ( LPVOID* )&pbVerticesDest ) )))
			return E_FAIL;

		// generate skinned mesh
		pMeshContainer->pSkinInfo->UpdateSkinnedMesh( m_pBoneMatrices, NULL, pbVerticesSrc, pbVerticesDest );

		if(FAILED(( pMeshContainer->pOrigMesh->UnlockVertexBuffer() )))
			return E_FAIL;
		if(FAILED( pMeshContainer->MeshData.pMesh->UnlockVertexBuffer() ))
			return E_FAIL;

		// ��Ű�� ������ ������ ����
		for( iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++ )
		{
			if(NULL != m_pEffect){
				m_pEffect->SetVector(EFFECT_VEC_COLOR,GetColor(pMeshContainer->pMaterials[pMeshContainer->pAttributeTable[iAttrib].AttribId].MatD3D.Diffuse));
				m_pEffect->SetTexture(EFFECT_TEX_TEX1,pMeshContainer->ppTextures[pMeshContainer->pAttributeTable[iAttrib].AttribId]);
				m_pEffect->Commit();
			}else{
				(*m_lpDevice)->SetMaterial( &(
					pMeshContainer->pMaterials[pMeshContainer->pAttributeTable[iAttrib].AttribId].MatD3D ) );
				(*m_lpDevice)->SetTexture( 0,
					pMeshContainer->ppTextures[pMeshContainer->pAttributeTable[iAttrib].AttribId] ) ;
			}
			pMeshContainer->MeshData.pMesh->DrawSubset( pMeshContainer->pAttributeTable[iAttrib].AttribId );
		}
	}
	else  // standard mesh, just draw it after setting material properties
	{
		if(NULL != m_pParent){
			matTemp = m_pParent->GetMatWorld();
			D3DXMatrixMultiply(&matTemp,&matTemp,&pFrame->CombinedTransformationMatrix);
		}else{
			matTemp = pFrame->CombinedTransformationMatrix;
		}

		if(m_pEffect){
			m_pEffect->SetMatrix( EFFECT_MAT_WORLD, matTemp );
		}else{
			(*m_lpDevice)->SetTransform( D3DTS_WORLD, &matTemp ) ;
		}

		for( iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++ )
		{
			if(NULL != m_pEffect){
				m_pEffect->SetVector(EFFECT_VEC_COLOR,pMeshContainer->pMaterials[iMaterial].MatD3D.Diffuse);
				m_pEffect->SetTexture(EFFECT_TEX_TEX1,pMeshContainer->ppTextures[iMaterial]);
				m_pEffect->Commit();
			}else{
				(*m_lpDevice)->SetMaterial( &pMeshContainer->pMaterials[iMaterial].MatD3D ) ;
				(*m_lpDevice)->SetTexture( 0, pMeshContainer->ppTextures[iMaterial] ) ;
			}
			pMeshContainer->MeshData.pMesh->DrawSubset( iMaterial ) ;
		}
	}

	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
// DrawMeshLiner
// �ܰ��� ������ ���� �Լ�
//////////////////////////////////////////////////////////////////////////
HRESULT O23DActiveObject::DrawMeshLiner( LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase )
{
	D3DXMESHCONTAINER_DERIVED* pMeshContainer = ( D3DXMESHCONTAINER_DERIVED* )pMeshContainerBase;
	D3DXFRAME_DERIVED* pFrame = ( D3DXFRAME_DERIVED* )pFrameBase;
	UINT iMaterial;
	UINT iAttrib;

	D3DXMATRIXA16 matTemp;
	D3DXMATRIXA16 matScail;
	D3DCAPS9 d3dCaps;
	(*m_lpDevice)->GetDeviceCaps( &d3dCaps );

	D3DXMatrixIdentity(&matScail);
	D3DXMatrixScaling(&matScail,LINE_SCAIL,LINE_SCAIL_Y,LINE_SCAIL);
	// first check for skinning
	if( pMeshContainer->pSkinInfo != NULL )
	{
		D3DXMATRIX Identity;
		DWORD cBones = pMeshContainer->pSkinInfo->GetNumBones();
		DWORD iBone;
		PBYTE pbVerticesSrc;
		PBYTE pbVerticesDest;

		// �ܰ����� ��� ������ ����
		for( iBone = 0; iBone < cBones; ++iBone )
		{
			//  �ܰ����� ���� ������ ���  * ���� �� ���
			D3DXMatrixMultiply
				(
				&m_pLineBoneMatrices[iBone],                 // output
				&matScail,
				&m_pBoneMatrices[iBone]
			);
		}

		// set world transform
		D3DXMatrixIdentity( &Identity );
		(*m_lpDevice)->SetTransform( D3DTS_WORLD, &Identity );

		if(FAILED(( pMeshContainer->pOrigMesh->LockVertexBuffer( D3DLOCK_READONLY, ( LPVOID* )&pbVerticesSrc ) )))
			return E_FAIL;
		if(FAILED(( pMeshContainer->MeshData.pMesh->LockVertexBuffer( 0, ( LPVOID* )&pbVerticesDest ) )))
			return E_FAIL;

		// generate skinned mesh
		pMeshContainer->pSkinInfo->UpdateSkinnedMesh( m_pLineBoneMatrices, NULL, pbVerticesSrc, pbVerticesDest );

		if(FAILED(( pMeshContainer->pOrigMesh->UnlockVertexBuffer() )))
			return E_FAIL;
		if(FAILED( pMeshContainer->MeshData.pMesh->UnlockVertexBuffer() ))
			return E_FAIL;


		for( iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++ )
		{
			pMeshContainer->MeshData.pMesh->DrawSubset( pMeshContainer->pAttributeTable[iAttrib].AttribId );
		}
	}
	else  // standard mesh, just draw it after setting material properties
	{
		D3DXMatrixMultiply(&m_matLiner,&matScail,&pFrame->CombinedTransformationMatrix);
		(*m_lpDevice)->SetTransform( D3DTS_WORLD, &m_matLiner ) ;

		for( iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++ )
		{
			pMeshContainer->MeshData.pMesh->DrawSubset( iMaterial ) ;
		}
	}

	return S_OK;
}

void O23DActiveObject::DrawFrameLine( LPD3DXFRAME pFrame )
{
	LPD3DXMESHCONTAINER pMeshContainer;

	pMeshContainer = pFrame->pMeshContainer;
	while( pMeshContainer != NULL )
	{
		DrawMeshLiner( pMeshContainer, pFrame );

		pMeshContainer = pMeshContainer->pNextMeshContainer;
	}

	if( pFrame->pFrameSibling != NULL )
	{
		DrawFrameLine( pFrame->pFrameSibling );
	}

	if( pFrame->pFrameFirstChild != NULL )
	{
		DrawFrameLine( pFrame->pFrameFirstChild );
	}
}

HRESULT O23DActiveObject::RenderLine()
{	
	m_pAnimController->AdvanceTime( 0.0f, NULL );

	DrawFrameLine(*m_pFrameRoot);

	return S_OK;
}

DWORD O23DActiveObject::GetAniIndex( LPCSTR name )
{
	HRESULT hr;
	LPD3DXANIMATIONSET pAS;
	DWORD dwRet = ANIMINDEX_FAIL;

	for( DWORD i = 0; i < m_pAnimController->GetNumAnimationSets(); ++ i )
	{
		hr = m_pAnimController->GetAnimationSet( i, &pAS );
		if( FAILED( hr ) )
			continue;

		if( pAS->GetName() &&
			0 == strcmp(pAS->GetName(),name) )
		{
			dwRet = i;
			pAS->Release();
			break;
		}

		pAS->Release();
	}

	return dwRet;
}

VOID O23DActiveObject::ChangeState( UINT pState )
{
	m_iState = pState;
}

void O23DActiveObject::StopAnim()
{
	for( UINT i = 0; i < m_dwMaxTracks; ++ i ){
		m_pAnimController->SetTrackEnable( i, FALSE );
	}
	m_bIsAniStop = TRUE;
}

VOID O23DActiveObject::UpDateAnim()
{
	DWORD dwNewTrack = ( m_dwCurTrack == 0 ? 1 : 0 );
	LPD3DXANIMATIONSET pAS;

	// Ʈ���� ��� �̺�Ʈ ����
	m_pAnimController->UnkeyAllTrackEvents( m_dwCurTrack );
	m_pAnimController->UnkeyAllTrackEvents( dwNewTrack );

	// Ʈ���� �ӵ�, ����ġ ����
	m_pAnimController->KeyTrackEnable( m_dwCurTrack, FALSE, (m_fPreTime) );
	m_pAnimController->KeyTrackSpeed( m_dwCurTrack, 0.0f, (m_fPreTime), 0.0f, D3DXTRANSITION_LINEAR );
	m_pAnimController->KeyTrackWeight( m_dwCurTrack, 0.0f, (m_fPreTime), 0.0f, D3DXTRANSITION_LINEAR );
	m_pAnimController->SetTrackEnable( dwNewTrack, TRUE );
	m_pAnimController->KeyTrackSpeed( dwNewTrack, 1.0f, (m_fCurTime), 0.0f, D3DXTRANSITION_LINEAR );
	m_pAnimController->KeyTrackWeight( dwNewTrack, 1.0f, (m_fCurTime), 0.0f, D3DXTRANSITION_LINEAR );

	m_dwCurTrack = dwNewTrack;

	// �ִϸ��̼� ������ �޾ƿ�
	m_pAnimController->GetTrackAnimationSet( m_dwCurTrack, &pAS );
	// ���� �ִ��� ���̸� �޾ƿ´�.
	m_lfAniTime = pAS->GetPeriod();
	// ���� �ð� ����
	m_lfDestTime = m_fCurTime;
	pAS->Release();
}

void O23DActiveObject::PlayAnim( INT pCount/*= ANIM_LOOP*/ ,BOOL isInit/*= TRUE*/ )
{
	LPD3DXANIMATIONSET pAS;
	m_pAnimController->SetTrackEnable( m_dwCurTrack, TRUE );

	// �ִϸ��̼� ������ �޾ƿ�
	m_pAnimController->GetTrackAnimationSet( m_dwCurTrack, &pAS );
	if(isInit)
		m_pAnimController->SetTrackPosition( m_dwCurTrack, 0.0 );
	// ���� �ִ��� ���̸� �޾ƿ´�.
	m_lfAniTime = pAS->GetPeriod();
	// ���� �ð� ����
	m_lfDestTime = m_fCurTime;
	pAS->Release();

	SetAniCount(pCount);
	m_bIsAniStop = FALSE;
}

VOID O23DActiveObject::SetMatWithActor()
{
	// ������Ʈ�� ȸ��/�̵� ��� �޾ƿ� ����
	NxMat33 temp;
	temp.setColumnMajorStride4(m_matRot.m);
	D3DXMATRIXA16 temprot;

	if(m_pActor){
		// �̵� �� �޾ƿ�
		m_vPos = NxVec3toDxVec3(m_pActor->getGlobalPosition());
		// Nx�� ȸ�� ����� Dx ���� ����.
		m_pActor->setGlobalOrientation(temp);
	}

	D3DXMatrixIdentity( &m_matWorld );
	D3DXMatrixTranslation(&m_matTrans,m_vPos.x,m_vPos.y,m_vPos.z);
	D3DXMatrixScaling(&m_matLiner,m_vSize.x*LINE_SCAIL,m_vSize.y*LINE_SCAIL_Y,m_vSize.z*LINE_SCAIL);
	m_matRT = m_matRot * m_matTrans;
	m_matWorld = m_matSize * m_matRT;
	m_matLiner = m_matLiner * m_matRT;
}

HRESULT O23DActiveObject::RenderShadow(UINT pass)
{
	m_pAnimController->AdvanceTime( 0.0f, NULL );

	for( UINT Cpass=0;Cpass < pass ; ++Cpass){
		// ������� ����
		SetMat();
		// ������ ��� ����
		UpdateFrameMatrices( *m_pFrameRoot, &m_matWorld );

		m_pEffect->BeginPass(Cpass);
		// ������Ʈ ȸ�� * �̵� ��� ( �븻 ���� ��ȯ )
		m_pEffect->SetMatrix(EFFECT_MAT_RT,m_matRT);
		// ������Ʈ ��ü Ⱦ�� ( ���ؽ� ��ǥ �̵� )
		m_pEffect->SetMatrix(EFFECT_MAT_WORLD,m_matWorld);

		DrawFrame( *m_pFrameRoot );

		m_pEffect->EndPass();
	}

	return S_OK;
}

void O23DActiveObject::DrawFrameShadow( LPD3DXFRAME pFrame )
{
	LPD3DXMESHCONTAINER pMeshContainer;

	pMeshContainer = pFrame->pMeshContainer;
	while( pMeshContainer != NULL )
	{
		DrawMeshShadow( pMeshContainer, pFrame );

		pMeshContainer = pMeshContainer->pNextMeshContainer;
	}

	if( pFrame->pFrameSibling != NULL )
	{
		DrawFrameLine( pFrame->pFrameSibling );
	}

	if( pFrame->pFrameFirstChild != NULL )
	{
		DrawFrameLine( pFrame->pFrameFirstChild );
	}
}

HRESULT O23DActiveObject::DrawMeshShadow( LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase )
{
	D3DXMESHCONTAINER_DERIVED* pMeshContainer = ( D3DXMESHCONTAINER_DERIVED* )pMeshContainerBase;
	D3DXFRAME_DERIVED* pFrame = ( D3DXFRAME_DERIVED* )pFrameBase;


	D3DXMATRIXA16 matTemp;
	D3DCAPS9 d3dCaps;
	(*m_lpDevice)->GetDeviceCaps( &d3dCaps );

	// ��Ű�� ������ Ȯ��
	if( pMeshContainer->pSkinInfo != NULL )
	{
		D3DXMATRIXA16 Identity;
		DWORD cBones = pMeshContainer->pSkinInfo->GetNumBones();

		if(m_pEffect){
			m_pEffect->SetMatrix( EFFECT_MAT_WORLD, Identity );
		}else{
			(*m_lpDevice)->SetTransform( D3DTS_WORLD, &Identity );
		}

		// ��Ű�� ������ ������ ����
		m_pEffect->Commit();

		(*m_lpShadowMesh)->DrawSubset(0);
	}
	else  // standard mesh, just draw it after setting material properties
	{
		if(NULL != m_pParent){
			matTemp = m_pParent->GetMatWorld();
			D3DXMatrixMultiply(&matTemp,&matTemp,&pFrame->CombinedTransformationMatrix);
		}else{
			matTemp = pFrame->CombinedTransformationMatrix;
		}

		if(m_pEffect){
			m_pEffect->SetMatrix( EFFECT_MAT_WORLD, matTemp );
		}else{
			(*m_lpDevice)->SetTransform( D3DTS_WORLD, &matTemp ) ;
		}

		m_pEffect->Commit();
		
		(*m_lpShadowMesh)->DrawSubset(0);
	}

	return S_OK;
}