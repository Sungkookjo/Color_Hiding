#include "stdafx.h"
#include "O2ActiveObject.h"

//////////////////////////////////////////////////////////////////////////
// 생성자
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
// 소멸자
O23DActiveObject::~O23DActiveObject()
{
	CleanUp();
}

//////////////////////////////////////////////////////////////////////////
//	정리
VOID O23DActiveObject::CleanUp()
{
	SAFE_RELEASE(m_pAnimController);
	SAFE_DELETE_ARRAY(m_pBoneMatrices);
	SAFE_DELETE_ARRAY(m_pLineBoneMatrices);
}

//////////////////////////////////////////////////////////////////////////
// LoadXfile
// 리소스 불러옴
HRESULT O23DActiveObject::LoadXfile( TCHAR* _filename )
{
	O2Mesh*	pMesh	= NULL;
	O2ActiveMesh* pActive = NULL;
	// 기본 메쉬 정보
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

	// 애니 once 검사
	if( ANIM_LOOP != m_iCount ){
		// 애니가 멈춰야될 시간과 현재 시간 체크
		double lerp = m_fCurTime - m_lfDestTime;
		if( m_lfAniTime <=  lerp){
			// 카운트 증가
			++m_iCurCount;
			// 애니 검사 시간을 현재 시간 - 오바 시간
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
	// 월드행렬 적용
	SetMat();
	
	m_pAnimController->AdvanceTime( 0.0f, NULL );

	// 프레임 행렬 갱신
	UpdateFrameMatrices( *m_pFrameRoot, &m_matWorld );

	m_pEffect->BeginPass(0);
	// 오브젝트 회전 * 이동 행렬 ( 노말 벡터 변환 )
	//m_pEffect->SetMatrix(EFFECT_MAT_RT,m_matRT);
	// 오브젝트 전체 횡렬 ( 버텍스 좌표 이동 )
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
// 오브젝트의 움직임 관련 함수
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
// 현 프래임의 오브젝트 로컬 행렬 업데이트
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
// 스키닝 정보를 갱신한다.
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
// 프레임을 그린다.
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
//	메쉬를 그린다.
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

	// 스키닝 정보를 확인
	if( pMeshContainer->pSkinInfo != NULL )
	{
		D3DXMATRIXA16 Identity;
		DWORD cBones = pMeshContainer->pSkinInfo->GetNumBones();
		DWORD iBone;
		PBYTE pbVerticesSrc;
		PBYTE pbVerticesDest;

		// 본 행렬을 구한다.
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
		

		// 현재 그려질 버텍스 정보와 이전 프레임의 버텍스 정보를 가지고 현재의 버텍스 정보를 갱신
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

		// 스키닝 정보를 가지고 랜더
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
// 외곽선 랜더를 위한 함수
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

		// 외곽선용 행렬 정보를 갱신
		for( iBone = 0; iBone < cBones; ++iBone )
		{
			//  외곽선을 위한 스케일 행렬  * 현재 본 행렬
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

	// 트랙의 모든 이벤트 해제
	m_pAnimController->UnkeyAllTrackEvents( m_dwCurTrack );
	m_pAnimController->UnkeyAllTrackEvents( dwNewTrack );

	// 트랙의 속도, 가중치 설정
	m_pAnimController->KeyTrackEnable( m_dwCurTrack, FALSE, (m_fPreTime) );
	m_pAnimController->KeyTrackSpeed( m_dwCurTrack, 0.0f, (m_fPreTime), 0.0f, D3DXTRANSITION_LINEAR );
	m_pAnimController->KeyTrackWeight( m_dwCurTrack, 0.0f, (m_fPreTime), 0.0f, D3DXTRANSITION_LINEAR );
	m_pAnimController->SetTrackEnable( dwNewTrack, TRUE );
	m_pAnimController->KeyTrackSpeed( dwNewTrack, 1.0f, (m_fCurTime), 0.0f, D3DXTRANSITION_LINEAR );
	m_pAnimController->KeyTrackWeight( dwNewTrack, 1.0f, (m_fCurTime), 0.0f, D3DXTRANSITION_LINEAR );

	m_dwCurTrack = dwNewTrack;

	// 애니메이션 정보를 받아옴
	m_pAnimController->GetTrackAnimationSet( m_dwCurTrack, &pAS );
	// 현재 애니의 길이를 받아온다.
	m_lfAniTime = pAS->GetPeriod();
	// 현재 시간 저장
	m_lfDestTime = m_fCurTime;
	pAS->Release();
}

void O23DActiveObject::PlayAnim( INT pCount/*= ANIM_LOOP*/ ,BOOL isInit/*= TRUE*/ )
{
	LPD3DXANIMATIONSET pAS;
	m_pAnimController->SetTrackEnable( m_dwCurTrack, TRUE );

	// 애니메이션 정보를 받아옴
	m_pAnimController->GetTrackAnimationSet( m_dwCurTrack, &pAS );
	if(isInit)
		m_pAnimController->SetTrackPosition( m_dwCurTrack, 0.0 );
	// 현재 애니의 길이를 받아온다.
	m_lfAniTime = pAS->GetPeriod();
	// 현재 시간 저장
	m_lfDestTime = m_fCurTime;
	pAS->Release();

	SetAniCount(pCount);
	m_bIsAniStop = FALSE;
}

VOID O23DActiveObject::SetMatWithActor()
{
	// 오브젝트의 회전/이동 행렬 받아올 변수
	NxMat33 temp;
	temp.setColumnMajorStride4(m_matRot.m);
	D3DXMATRIXA16 temprot;

	if(m_pActor){
		// 이동 값 받아옴
		m_vPos = NxVec3toDxVec3(m_pActor->getGlobalPosition());
		// Nx의 회전 행렬을 Dx 꺼로 설정.
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
		// 월드행렬 적용
		SetMat();
		// 프레임 행렬 갱신
		UpdateFrameMatrices( *m_pFrameRoot, &m_matWorld );

		m_pEffect->BeginPass(Cpass);
		// 오브젝트 회전 * 이동 행렬 ( 노말 벡터 변환 )
		m_pEffect->SetMatrix(EFFECT_MAT_RT,m_matRT);
		// 오브젝트 전체 횡렬 ( 버텍스 좌표 이동 )
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

	// 스키닝 정보를 확인
	if( pMeshContainer->pSkinInfo != NULL )
	{
		D3DXMATRIXA16 Identity;
		DWORD cBones = pMeshContainer->pSkinInfo->GetNumBones();

		if(m_pEffect){
			m_pEffect->SetMatrix( EFFECT_MAT_WORLD, Identity );
		}else{
			(*m_lpDevice)->SetTransform( D3DTS_WORLD, &Identity );
		}

		// 스키닝 정보를 가지고 랜더
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