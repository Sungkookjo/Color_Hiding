#include "stdafx.h"

ResourceManager* CAllocateHierarchy::s_pResource = NULL;

//--------------------------------------------------------------------------------------
// Name: AllocateName()
// Desc: Allocates memory for a string to hold the name of a frame or mesh
//		 문자열 동적 할당 하여 복사
//--------------------------------------------------------------------------------------
HRESULT AllocateName( LPCSTR Name, LPSTR* pNewName )
{
	UINT cbLength;

	if( Name != NULL )
	{
		cbLength = ( UINT )strlen( Name ) + 1;
		*pNewName = new CHAR[cbLength];
		if( *pNewName == NULL )
			return E_OUTOFMEMORY;
		memcpy( *pNewName, Name, cbLength * sizeof( CHAR ) );
	}
	else
	{
		*pNewName = NULL;
	}

	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
//	GenerateSkinnedMesh
// 스킨 메쉬를 로드하거나 함수가 바뀔떄 불려진다.
// 본 테이블과 기존 메쉬 정보를 혼합하여 스키닝된 메쉬 정보를 생성한다.
//////////////////////////////////////////////////////////////////////////
HRESULT GenerateSkinnedMesh( IDirect3DDevice9* pd3dDevice, D3DXMESHCONTAINER_DERIVED* pMeshContainer )
{
	HRESULT hr;

	hr = pMeshContainer->pOrigMesh->CloneMeshFVF( D3DXMESH_MANAGED, pMeshContainer->pOrigMesh->GetFVF(),
		pd3dDevice, &pMeshContainer->MeshData.pMesh );
	if( FAILED( hr ) )
		goto e_Exit;

	hr = pMeshContainer->MeshData.pMesh->GetAttributeTable( NULL, &pMeshContainer->NumAttributeGroups );
	if( FAILED( hr ) )
		goto e_Exit;

	delete[] pMeshContainer->pAttributeTable;
	pMeshContainer->pAttributeTable = new D3DXATTRIBUTERANGE[pMeshContainer->NumAttributeGroups];
	if( pMeshContainer->pAttributeTable == NULL )
	{
		hr = E_OUTOFMEMORY;
		goto e_Exit;
	}

	hr = pMeshContainer->MeshData.pMesh->GetAttributeTable( pMeshContainer->pAttributeTable, NULL );
	if( FAILED( hr ) )
		goto e_Exit;

e_Exit:
	return hr;
}

//////////////////////////////////////////////////////////////////////////
// ReleaseAttributeTable
// 프레임 전체 해제
//////////////////////////////////////////////////////////////////////////
void ReleaseAttributeTable( LPD3DXFRAME pFrameBase )
{
	D3DXFRAME_DERIVED* pFrame = ( D3DXFRAME_DERIVED* )pFrameBase;
	D3DXMESHCONTAINER_DERIVED* pMeshContainer;

	pMeshContainer = ( D3DXMESHCONTAINER_DERIVED* )pFrame->pMeshContainer;

	while( pMeshContainer != NULL )
	{
		delete[] pMeshContainer->pAttributeTable;

		pMeshContainer = ( D3DXMESHCONTAINER_DERIVED* )pMeshContainer->pNextMeshContainer;
	}

	if( pFrame->pFrameSibling != NULL )
	{
		ReleaseAttributeTable( pFrame->pFrameSibling );
	}

	if( pFrame->pFrameFirstChild != NULL )
	{
		ReleaseAttributeTable( pFrame->pFrameFirstChild );
	}
}


//--------------------------------------------------------------------------------------
// Name: CAllocateHierarchy::CreateFrame()
// Desc: 프레임 생성 뼈대를 생성한다.
//--------------------------------------------------------------------------------------
HRESULT CAllocateHierarchy::CreateFrame( LPCSTR Name, LPD3DXFRAME* ppNewFrame )
{
	HRESULT hr = S_OK;
	D3DXFRAME_DERIVED* pFrame;

	*ppNewFrame = NULL;

	pFrame = new D3DXFRAME_DERIVED;
	if( pFrame == NULL )
	{
		hr = E_OUTOFMEMORY;
		goto e_Exit;
	}

	hr = AllocateName( Name, &pFrame->Name );
	if( FAILED( hr ) )
		goto e_Exit;

	D3DXMatrixIdentity( &pFrame->TransformationMatrix );
	D3DXMatrixIdentity( &pFrame->CombinedTransformationMatrix );

	pFrame->pMeshContainer = NULL;
	pFrame->pFrameSibling = NULL;
	pFrame->pFrameFirstChild = NULL;

	*ppNewFrame = pFrame;
	pFrame = NULL;

e_Exit:
	delete pFrame;
	return hr;
}




//--------------------------------------------------------------------------------------
// Name: CAllocateHierarchy::CreateMeshContainer()
// Desc: D3DXLoadMeshHierarchyFromX 함수 호출시 호출됨
//		 그려질 메쉬 정보 복사 및 스키닝 정보를 복사
//--------------------------------------------------------------------------------------
HRESULT CAllocateHierarchy::CreateMeshContainer(
	LPCSTR Name,
	CONST D3DXMESHDATA *pMeshData,
	CONST D3DXMATERIAL *pMaterials,
	CONST D3DXEFFECTINSTANCE *pEffectInstances,
	DWORD NumMaterials,
	CONST DWORD *pAdjacency,
	LPD3DXSKININFO pSkinInfo,
	LPD3DXMESHCONTAINER *ppNewMeshContainer )
{
	HRESULT hr;
	D3DXMESHCONTAINER_DERIVED *pMeshContainer = NULL;
	UINT NumFaces;
	UINT iMaterial;
	UINT iBone, cBones;
	LPDIRECT3DDEVICE9 pd3dDevice = NULL;

	LPD3DXMESH pMesh = NULL;

	*ppNewMeshContainer = NULL;

	// 메쉬 인지 확인
	if( pMeshData->Type != D3DXMESHTYPE_MESH )
	{
		hr = E_FAIL;
		goto e_Exit;
	}

	// 메쉬 얻어옴
	pMesh = pMeshData->pMesh;

	// FVF 정보 확인 
	if( pMesh->GetFVF() == 0 )
	{
		hr = E_FAIL;
		goto e_Exit;
	}

	// 메쉬 컨테이너 생성
	pMeshContainer = new D3DXMESHCONTAINER_DERIVED;
	if( pMeshContainer == NULL )
	{
		hr = E_OUTOFMEMORY;
		goto e_Exit;
	}
	memset( pMeshContainer, 0, sizeof( D3DXMESHCONTAINER_DERIVED ) );

	// 이름 복사
	hr = AllocateName( Name, &pMeshContainer->Name );
	if( FAILED( hr ) )
		goto e_Exit;

	pMesh->GetDevice( &pd3dDevice );
	// 메쉬의 삼각형 개수 받아옴
	NumFaces = pMesh->GetNumFaces();

	// 노말 벡터가 없으면 노말 벡터 추가
	if( !( pMesh->GetFVF() & D3DFVF_NORMAL ) )
	{
		pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

		// 노말 벡터 포멧이 추가된 FVF로 설정
		hr = pMesh->CloneMeshFVF( pMesh->GetOptions(),
			pMesh->GetFVF() | D3DFVF_NORMAL,
			pd3dDevice, &pMeshContainer->MeshData.pMesh );
		if( FAILED( hr ) )
			goto e_Exit;

		// 다시 메쉬를 받아옴
		pMesh = pMeshContainer->MeshData.pMesh;

		// 노말벡터 생성
		D3DXComputeNormals( pMesh, NULL );
	}
	else  // if no normals, just add a reference to the mesh for the mesh container
	{
		pMeshContainer->MeshData.pMesh = pMesh;
		pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

		pMesh->AddRef();
	}

	// 재질 설정
	// 재질의 개수가 최소 1개가 될수 있게
	pMeshContainer->NumMaterials = max( (DWORD)1, NumMaterials );
	pMeshContainer->pMaterials = new D3DXMATERIAL[pMeshContainer->NumMaterials];
	pMeshContainer->ppTextures = new LPDIRECT3DTEXTURE9[pMeshContainer->NumMaterials];
	pMeshContainer->pAdjacency = new DWORD[NumFaces*3];

	// 동적 할당 실패
	if( ( pMeshContainer->pAdjacency == NULL ) || ( pMeshContainer->pMaterials == NULL ) )
	{
		hr = E_OUTOFMEMORY;
		goto e_Exit;
	}

	// 정보 복사
	memcpy( pMeshContainer->pAdjacency, pAdjacency, sizeof( DWORD ) * NumFaces*3 );
	memset( pMeshContainer->ppTextures, 0, sizeof( LPDIRECT3DTEXTURE9 ) * pMeshContainer->NumMaterials );

	// 재질 생성
	if( NumMaterials > 0 )
	{
		memcpy( pMeshContainer->pMaterials, pMaterials, sizeof( D3DXMATERIAL ) * NumMaterials );

		for( iMaterial = 0; iMaterial < NumMaterials; iMaterial++ )
		{
			if( pMeshContainer->pMaterials[iMaterial].pTextureFilename != NULL )
			{
#ifdef UNICODE
				TCHAR wszBuf[MAX_PATH];
				MultiByteToWideChar( CP_ACP, 0, pMeshContainer->pMaterials[iMaterial].pTextureFilename, -1, wszBuf, MAX_PATH );
				wszBuf[MAX_PATH - 1] = L'\0';
				pMeshContainer->ppTextures[iMaterial] = s_pResource->GetTexture(wszBuf);
#else
				pMeshContainer->ppTextures[iMaterial] = s_pResource->GetTexture(pMeshContainer->pMaterials[iMaterial].pTextureFilename);
#endif
				pMeshContainer->pMaterials[iMaterial].pTextureFilename = NULL;
			}
		}
	}
	else // 메테리얼이 제공되지 않은 경우 ( 없으면 ) , 아래로 설정
	{
		pMeshContainer->pMaterials[0].pTextureFilename = NULL;
		memset( &pMeshContainer->pMaterials[0].MatD3D, 0, sizeof( D3DMATERIAL9 ) );
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.r = 1.0f;
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.g = 1.0f;
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.b = 1.0f;
		pMeshContainer->pMaterials[0].MatD3D.Specular = pMeshContainer->pMaterials[0].MatD3D.Diffuse;
	}

	// 스키닝 정보가 있으면
	if( pSkinInfo != NULL )
	{
		// 스키닝 정보 저장
		pMeshContainer->pSkinInfo = pSkinInfo;
		pSkinInfo->AddRef();

		// 오리지날 메쉬 정보 저장
		pMeshContainer->pOrigMesh = pMesh;
		pMesh->AddRef();

		// 본의 개수 받아와 개수만큼의 본들의 행렬 할당
		cBones = pSkinInfo->GetNumBones();
		pMeshContainer->pBoneOffsetMatrices = new D3DXMATRIX[cBones];
		if( pMeshContainer->pBoneOffsetMatrices == NULL )
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}

		// 각 본들의 행렬 정보를 받아와 저장
		for( iBone = 0; iBone < cBones; iBone++ )
		{
			pMeshContainer->pBoneOffsetMatrices[iBone] = *( pMeshContainer->pSkinInfo->GetBoneOffsetMatrix( iBone ) );
		}

		hr = GenerateSkinnedMesh( pd3dDevice, pMeshContainer );
		if( FAILED( hr ) )
			goto e_Exit;
	}

	m_dwNumBones = pMeshContainer->pSkinInfo->GetNumBones();
	m_pMeshContainer = pMeshContainer;
	*ppNewMeshContainer = pMeshContainer;
	pMeshContainer = NULL;
e_Exit:
	SAFE_RELEASE( pd3dDevice );

	// call Destroy function to properly clean up the memory allocated 
	if( pMeshContainer != NULL )
	{
		DestroyMeshContainer( pMeshContainer );
	}

	return hr;
}




//--------------------------------------------------------------------------------------
// Name: CAllocateHierarchy::DestroyFrame()
// Desc: 
//--------------------------------------------------------------------------------------
HRESULT CAllocateHierarchy::DestroyFrame( LPD3DXFRAME pFrameToFree )
{
	SAFE_DELETE_ARRAY( pFrameToFree->Name );
	SAFE_DELETE( pFrameToFree );
	return S_OK;
}

//--------------------------------------------------------------------------------------
// Name: CAllocateHierarchy::DestroyMeshContainer()
// Desc: 
//--------------------------------------------------------------------------------------
HRESULT CAllocateHierarchy::DestroyMeshContainer( LPD3DXMESHCONTAINER pMeshContainerBase )
{
	UINT iMaterial;
	D3DXMESHCONTAINER_DERIVED* pMeshContainer = ( D3DXMESHCONTAINER_DERIVED* )pMeshContainerBase;

	SAFE_DELETE_ARRAY( pMeshContainer->Name );
	SAFE_DELETE_ARRAY( pMeshContainer->pAdjacency );
	SAFE_DELETE_ARRAY( pMeshContainer->pMaterials );
	SAFE_DELETE_ARRAY( pMeshContainer->pBoneOffsetMatrices );

	SAFE_DELETE_ARRAY( pMeshContainer->ppTextures );
	SAFE_DELETE_ARRAY( pMeshContainer->ppBoneMatrixPtrs );
	SAFE_RELEASE( pMeshContainer->pBoneCombinationBuf );
	SAFE_RELEASE( pMeshContainer->MeshData.pMesh );
	SAFE_RELEASE( pMeshContainer->pSkinInfo );
	SAFE_RELEASE( pMeshContainer->pOrigMesh );
	SAFE_DELETE( pMeshContainer );
	return S_OK;
}

HRESULT D3DXMESHCONTAINER_DERIVED::SetupBonePtrs( D3DXFRAME* pFrameRoot )
{
	// 스킨 정보가 있으면
	if( pSkinInfo )
	{
		// 본 포인터 
		SAFE_DELETE_ARRAY(ppBoneMatrixPtrs);

		DWORD dwNumBones = pSkinInfo->GetNumBones();

		ppBoneMatrixPtrs = new D3DXMATRIX* [ dwNumBones ];
		if( ppBoneMatrixPtrs == NULL )
			return E_OUTOFMEMORY;

		for( DWORD i = 0; i < dwNumBones; ++ i )
		{
			D3DXFRAME_DERIVED* pFrame = ( D3DXFRAME_DERIVED* )D3DXFrameFind( pFrameRoot, pSkinInfo->GetBoneName( i ) );
			if( pFrame == NULL ){
				return E_FAIL;
			}else{
				pFrame->m_dwBoneIndex = i;
			}

			ppBoneMatrixPtrs[ i ] = &pFrame->TransformationMatrix;
		}
	}

	return S_OK;
}