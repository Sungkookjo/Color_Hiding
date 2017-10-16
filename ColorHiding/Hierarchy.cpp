#include "stdafx.h"

ResourceManager* CAllocateHierarchy::s_pResource = NULL;

//--------------------------------------------------------------------------------------
// Name: AllocateName()
// Desc: Allocates memory for a string to hold the name of a frame or mesh
//		 ���ڿ� ���� �Ҵ� �Ͽ� ����
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
// ��Ų �޽��� �ε��ϰų� �Լ��� �ٲ��� �ҷ�����.
// �� ���̺�� ���� �޽� ������ ȥ���Ͽ� ��Ű�׵� �޽� ������ �����Ѵ�.
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
// ������ ��ü ����
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
// Desc: ������ ���� ���븦 �����Ѵ�.
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
// Desc: D3DXLoadMeshHierarchyFromX �Լ� ȣ��� ȣ���
//		 �׷��� �޽� ���� ���� �� ��Ű�� ������ ����
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

	// �޽� ���� Ȯ��
	if( pMeshData->Type != D3DXMESHTYPE_MESH )
	{
		hr = E_FAIL;
		goto e_Exit;
	}

	// �޽� ����
	pMesh = pMeshData->pMesh;

	// FVF ���� Ȯ�� 
	if( pMesh->GetFVF() == 0 )
	{
		hr = E_FAIL;
		goto e_Exit;
	}

	// �޽� �����̳� ����
	pMeshContainer = new D3DXMESHCONTAINER_DERIVED;
	if( pMeshContainer == NULL )
	{
		hr = E_OUTOFMEMORY;
		goto e_Exit;
	}
	memset( pMeshContainer, 0, sizeof( D3DXMESHCONTAINER_DERIVED ) );

	// �̸� ����
	hr = AllocateName( Name, &pMeshContainer->Name );
	if( FAILED( hr ) )
		goto e_Exit;

	pMesh->GetDevice( &pd3dDevice );
	// �޽��� �ﰢ�� ���� �޾ƿ�
	NumFaces = pMesh->GetNumFaces();

	// �븻 ���Ͱ� ������ �븻 ���� �߰�
	if( !( pMesh->GetFVF() & D3DFVF_NORMAL ) )
	{
		pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

		// �븻 ���� ������ �߰��� FVF�� ����
		hr = pMesh->CloneMeshFVF( pMesh->GetOptions(),
			pMesh->GetFVF() | D3DFVF_NORMAL,
			pd3dDevice, &pMeshContainer->MeshData.pMesh );
		if( FAILED( hr ) )
			goto e_Exit;

		// �ٽ� �޽��� �޾ƿ�
		pMesh = pMeshContainer->MeshData.pMesh;

		// �븻���� ����
		D3DXComputeNormals( pMesh, NULL );
	}
	else  // if no normals, just add a reference to the mesh for the mesh container
	{
		pMeshContainer->MeshData.pMesh = pMesh;
		pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

		pMesh->AddRef();
	}

	// ���� ����
	// ������ ������ �ּ� 1���� �ɼ� �ְ�
	pMeshContainer->NumMaterials = max( (DWORD)1, NumMaterials );
	pMeshContainer->pMaterials = new D3DXMATERIAL[pMeshContainer->NumMaterials];
	pMeshContainer->ppTextures = new LPDIRECT3DTEXTURE9[pMeshContainer->NumMaterials];
	pMeshContainer->pAdjacency = new DWORD[NumFaces*3];

	// ���� �Ҵ� ����
	if( ( pMeshContainer->pAdjacency == NULL ) || ( pMeshContainer->pMaterials == NULL ) )
	{
		hr = E_OUTOFMEMORY;
		goto e_Exit;
	}

	// ���� ����
	memcpy( pMeshContainer->pAdjacency, pAdjacency, sizeof( DWORD ) * NumFaces*3 );
	memset( pMeshContainer->ppTextures, 0, sizeof( LPDIRECT3DTEXTURE9 ) * pMeshContainer->NumMaterials );

	// ���� ����
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
	else // ���׸����� �������� ���� ��� ( ������ ) , �Ʒ��� ����
	{
		pMeshContainer->pMaterials[0].pTextureFilename = NULL;
		memset( &pMeshContainer->pMaterials[0].MatD3D, 0, sizeof( D3DMATERIAL9 ) );
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.r = 1.0f;
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.g = 1.0f;
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.b = 1.0f;
		pMeshContainer->pMaterials[0].MatD3D.Specular = pMeshContainer->pMaterials[0].MatD3D.Diffuse;
	}

	// ��Ű�� ������ ������
	if( pSkinInfo != NULL )
	{
		// ��Ű�� ���� ����
		pMeshContainer->pSkinInfo = pSkinInfo;
		pSkinInfo->AddRef();

		// �������� �޽� ���� ����
		pMeshContainer->pOrigMesh = pMesh;
		pMesh->AddRef();

		// ���� ���� �޾ƿ� ������ŭ�� ������ ��� �Ҵ�
		cBones = pSkinInfo->GetNumBones();
		pMeshContainer->pBoneOffsetMatrices = new D3DXMATRIX[cBones];
		if( pMeshContainer->pBoneOffsetMatrices == NULL )
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}

		// �� ������ ��� ������ �޾ƿ� ����
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
	// ��Ų ������ ������
	if( pSkinInfo )
	{
		// �� ������ 
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