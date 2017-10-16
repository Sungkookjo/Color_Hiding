#pragma once

//--------------------------------------------------------------------------------------
// Name: struct D3DXFRAME_DERIVED
// Desc: Structure derived from D3DXFRAME so we can add some app-specific
//       info that will be stored with each frame
//--------------------------------------------------------------------------------------
struct D3DXFRAME_DERIVED : public D3DXFRAME
{
	D3DXMATRIXA16	CombinedTransformationMatrix;
	DWORD			m_dwBoneIndex;
};


//--------------------------------------------------------------------------------------
// Name: struct D3DXMESHCONTAINER_DERIVED
// Desc: Structure derived from D3DXMESHCONTAINER so we can add some app-specific
//       info that will be stored with each mesh
//--------------------------------------------------------------------------------------
struct D3DXMESHCONTAINER_DERIVED : public D3DXMESHCONTAINER
{
	LPDIRECT3DTEXTURE9* ppTextures;       // array of textures, entries are NULL if no texture specified    

	// SkinMesh info             
	LPD3DXMESH pOrigMesh;
	LPD3DXATTRIBUTERANGE pAttributeTable;
	DWORD NumAttributeGroups;
	DWORD NumInfl;
	LPD3DXBUFFER pBoneCombinationBuf;
	D3DXMATRIX** ppBoneMatrixPtrs;
	D3DXMATRIX* pBoneOffsetMatrices;
	DWORD NumPaletteEntries;
	bool UseSoftwareVP;
	DWORD iAttributeSW;     // used to denote the split between SW and HW if necessary for non-indexed skinning

	HRESULT SetupBonePtrs( D3DXFRAME* pFrameRoot );
};

HRESULT AllocateName( LPCSTR Name, LPSTR* pNewName );
HRESULT GenerateSkinnedMesh( IDirect3DDevice9* pd3dDevice, D3DXMESHCONTAINER_DERIVED* pMeshContainer );
void ReleaseAttributeTable( LPD3DXFRAME pFrameBase );

//--------------------------------------------------------------------------------------
// Name: class CAllocateHierarchy
// Desc: Custom version of ID3DXAllocateHierarchy with custom methods to create
//       frames and meshcontainers.
//--------------------------------------------------------------------------------------
class CAllocateHierarchy : public ID3DXAllocateHierarchy
{
public:
	static class ResourceManager* s_pResource;
	STDMETHOD( CreateFrame )( THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame );
	STDMETHOD( CreateMeshContainer )( THIS_
		LPCSTR Name,
		CONST D3DXMESHDATA *pMeshData,
		CONST D3DXMATERIAL *pMaterials,
		CONST D3DXEFFECTINSTANCE *pEffectInstances,
		DWORD NumMaterials,
		CONST DWORD *pAdjacency,
		LPD3DXSKININFO pSkinInfo,
		LPD3DXMESHCONTAINER *ppNewMeshContainer );
	STDMETHOD( DestroyFrame )( THIS_ LPD3DXFRAME pFrameToFree );
	STDMETHOD( DestroyMeshContainer )( THIS_ LPD3DXMESHCONTAINER pMeshContainerBase );

public:
	DWORD	m_dwNumBones;
	LPD3DXMESHCONTAINER m_pMeshContainer;
};