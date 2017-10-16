//////////////////////////////////////////////////////////////////////////
// Mesh
// �Ž� Ŭ����
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

//�޽� ����
class O2Mesh{
protected:
	Basefw_FS*				m_pFS;
	LPDIRECT3DDEVICE9*		m_lpDevice			;	//	����̽�
	LPD3DXMESH              m_pMesh				;	//	�޽�����
	LPD3DXMESH				m_pShadowMesh		;	//	������ �޽� ����
	D3DMATERIAL9*           m_pMeshMaterials	;	//	�޽� ����
	LPDIRECT3DTEXTURE9*     m_pMeshTextures		;	//	�޽� �ؽ�ó ����
	UINT					m_uNumVertices		;	//	���ؽ� ����
	UINT					m_uNumFaces			;	//	�ﰢ�� ����
	UINT					m_uNumIndeces		;	//	�ﰢ�� ����
	LPDIRECT3DVERTEXBUFFER9	m_lpVertexBuffer	;	//	���ؽ� ����
	LPDIRECT3DINDEXBUFFER9	m_lpIndexBuffer		;	//	�ε��� ����

	DWORD                   m_dwNumMaterials	;	//	������ ����
	O2CollisionInfo*		m_pColl				;	//	�޽��� ��/�ڽ� �浹 ���
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