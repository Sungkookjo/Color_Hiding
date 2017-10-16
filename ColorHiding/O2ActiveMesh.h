#pragma once

class O2ActiveMesh{
private:
	Basefw_FS*					m_pFS;
	LPDIRECT3DDEVICE9*			m_lpDevice			;	//	디바이스
	LPD3DXFRAME                 m_pFrameRoot;
	LPD3DXANIMATIONCONTROLLER   m_pAnimController;
	UINT                        m_NumBoneMatricesMax;
	DWORD						m_dwMaxTracks;
	LPD3DXMESH					m_pMesh;
	LPD3DXMESH					m_pShadowMesh;
	LPDIRECT3DVERTEXDECLARATION9 m_pDecl;

	HRESULT GenerateShadowMesh();
public:
	O2ActiveMesh(LPDIRECT3DDEVICE9* pDev,Basefw_FS* FS);
	~O2ActiveMesh();

	virtual HRESULT Init(const TCHAR* filename,class ResourceManager* pResource);

	// 본 행렬 세팅 ( 메쉬로 부터 행렬 포인터를 받아와 프레임에 저장 )
	HRESULT SetupBoneMatrixPointers( LPD3DXFRAME pFrame );
	HRESULT SetupBoneMatrixPointersOnMesh( LPD3DXMESHCONTAINER pMeshContainerBase );

	LPD3DXFRAME*				GetFrame() { return &m_pFrameRoot; }
	LPD3DXANIMATIONCONTROLLER	GetAniContr();
	UINT						GetBoneNum() { return m_NumBoneMatricesMax; }
	DWORD						GetMaxTrack() { return m_dwMaxTracks; }
	LPD3DXMESH*					GetShadowMesh() { return &m_pShadowMesh; }
	LPD3DXMESH*					GetMesh()	{ return &m_pMesh; }
};

HRESULT     SetVertexDecl( LPDIRECT3DDEVICE9 pd3dDevice, LPD3DXMESH* m_pMesh, const D3DVERTEXELEMENT9* pDecl,
						  bool bAutoComputeNormals = true, bool bAutoComputeTangents = true,
						  bool bSplitVertexForOptimalTangents = false );