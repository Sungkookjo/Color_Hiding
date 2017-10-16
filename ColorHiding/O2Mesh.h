//////////////////////////////////////////////////////////////////////////
// Mesh
// 매쉬 클래스
#pragma once

struct SHADOWVERT
{
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
	D3DXVECTOR2 Tex;

	const static D3DVERTEXELEMENT9 Decl[4];
};

struct MESHVERT
{
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
	D3DXVECTOR2 Tex;

	const static D3DVERTEXELEMENT9 Decl[4];
};

struct CEdgeMapping
{
	int m_anOldEdge[2];  // vertex index of the original edge
	int m_aanNewEdge[2][2]; // vertex indexes of the new edge
	// First subscript = index of the new edge
	// Second subscript = index of the vertex for the edge

public:
	CEdgeMapping()
	{
		FillMemory( m_anOldEdge, sizeof( m_anOldEdge ), -1 );
		FillMemory( m_aanNewEdge, sizeof( m_aanNewEdge ), -1 );
	}
};

int FindEdgeInMappingTable( int nV1, int nV2, CEdgeMapping* pMapping, int nCount );

typedef struct {
	D3DXVECTOR3 VertexPos;
	D3DXVECTOR3 Normal;
	D3DXVECTOR2 TexCoord;
} Mesh_FVF;	//Used to copy indices

typedef struct {
	short IBNumber[3];
} IndexBufferStruct;

//메쉬 정보
class O2Mesh{
protected:
	Basefw_FS*				m_pFS;
	LPDIRECT3DDEVICE9*		m_lpDevice			;	//	디바이스
	LPD3DXMESH              m_pMesh				;	//	메쉬정보
	LPD3DXMESH				m_pShadowMesh		;	//	쉐도우 메쉬 정보
	D3DMATERIAL9*           m_pMeshMaterials	;	//	메쉬 재질
	LPDIRECT3DTEXTURE9*     m_pMeshTextures		;	//	메쉬 텍스처 재질
	UINT					m_uNumVertices		;	//	버텍스 개수
	UINT					m_uNumFaces			;	//	삼각형 개수
	UINT					m_uNumIndeces		;	//	삼각형 개수
	LPDIRECT3DVERTEXBUFFER9	m_lpVertexBuffer	;	//	버텍스 버퍼
	LPDIRECT3DINDEXBUFFER9	m_lpIndexBuffer		;	//	인덱스 버퍼

	DWORD                   m_dwNumMaterials	;	//	재질의 개수
	O2CollisionInfo*		m_pColl				;	//	메쉬의 구/박스 충돌 경계
protected:
	virtual void Cleanup();
	HRESULT GenerateShadowMesh();
public:
	O2Mesh(LPDIRECT3DDEVICE9* pDevice,Basefw_FS* FS);
	virtual ~O2Mesh();
	virtual HRESULT					Init(const TCHAR* filename,class ResourceManager* pResource);
	LPD3DXMESH*						GetMesh()														{ return &m_pMesh; }
	LPD3DXMESH*						GetShadowMesh()													{ return &m_pShadowMesh; }
	D3DMATERIAL9**					GetMaterials()													{ return &m_pMeshMaterials; }
	LPDIRECT3DINDEXBUFFER9			GetIndexBuffer()												{ return m_lpIndexBuffer; }
	LPDIRECT3DVERTEXBUFFER9			GetVertexBuffer()												{ return m_lpVertexBuffer; }
	LPDIRECT3DTEXTURE9*				GetTextures()													{ return m_pMeshTextures; }
	DWORD							GetNumMaterial()												{ return m_dwNumMaterials; }
	O2CollisionInfo				GetColl()															{ return *m_pColl;	}
	UINT							GetNumVertices()												{ return m_uNumVertices; }
	UINT							GetNumFaces()													{ return m_uNumFaces; }
};