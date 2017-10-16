//////////////////////////////////////////////////////////////////////////
// ResoureManager.h
// 리소스 관리 클래스
#pragma once

class ResourceManager{
private:
	LPDIRECT3DDEVICE9* m_lpDevice;
	map<tstring,O2Texture*> m_pTextureMap;
	map<tstring,O2Mesh*> m_pMeshMap;
	map<tstring,O2ActiveMesh*> m_pActiveMap;
	Basefw_FS*			m_pFS;			//	압축 파일
private:
	void Cleanup();
public:
	ResourceManager(Basefw_FS* FS);
	~ResourceManager();
	HRESULT Init(LPDIRECT3DDEVICE9* lpDev);
	LPDIRECT3DTEXTURE9&	GetTexture( tstring pName);
	O2Mesh*	GetMesh( tstring pName);
	O2ActiveMesh* GetActiveMesh(tstring pName);

	HRESULT AddTexture(tstring pName);
	HRESULT AddMesh(tstring pName);
	HRESULT AddActiveMesh(tstring pName);
};