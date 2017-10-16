#include "stdafx.h"


//////////////////////////////////////////////////////////////////////////
// Constructor
ResourceManager::ResourceManager(Basefw_FS* FS)
:m_lpDevice(NULL)
,m_pFS(FS)
{
	CAllocateHierarchy::s_pResource = this;
}

//////////////////////////////////////////////////////////////////////////
// Destructor
ResourceManager::~ResourceManager()
{
	Cleanup();
}

HRESULT ResourceManager::Init( LPDIRECT3DDEVICE9* lpDev )
{
	m_lpDevice = lpDev;

	return S_OK;
}

void ResourceManager::Cleanup()
{
	map<tstring,O2Mesh*>::iterator mesh_iter = m_pMeshMap.begin();
	map<tstring,O2Mesh*>::iterator mesh_end = m_pMeshMap.end();

	map<tstring,O2Texture*>::iterator texture_iter = m_pTextureMap.begin();
	map<tstring,O2Texture*>::iterator texture_end = m_pTextureMap.end();

	map<tstring,O2ActiveMesh*>::iterator Active_iter = m_pActiveMap.begin();
	map<tstring,O2ActiveMesh*>::iterator Active_end = m_pActiveMap.end();

	while(mesh_iter != mesh_end){
		SAFE_DELETE(mesh_iter->second);
		++mesh_iter;
	}
	m_pMeshMap.clear();

	while(texture_iter != texture_end){
		SAFE_DELETE(texture_iter->second);
		++texture_iter;
	}
	m_pTextureMap.clear();

	while(Active_iter != Active_end){
		SAFE_DELETE(Active_iter->second);
		++Active_iter;
	}
	m_pActiveMap.clear();

}

LPDIRECT3DTEXTURE9& ResourceManager::GetTexture( tstring pName )
{
	if( NULL == m_pTextureMap[pName]){
		if(FAILED(AddTexture(pName))){
			MessageBox(NULL,_T("Fail-LoadTexture"),_T("Error"),MB_OK);
		}
	}
	return m_pTextureMap[pName]->GetTexture();
}

O2Mesh* ResourceManager::GetMesh( tstring pName )
{
	if( NULL == m_pMeshMap[pName]){
		if(FAILED(AddMesh(pName))){
			return NULL;
		}
	}

	return m_pMeshMap[pName];
}

HRESULT ResourceManager::AddTexture( tstring pName )
{
	O2Texture* Temp = new O2Texture(m_lpDevice,m_pFS);
	if(FAILED(Temp->Init(pName.c_str()))){
		SAFE_DELETE(Temp);
		return E_FAIL;
	}
	m_pTextureMap[pName] = Temp;
	return S_OK;
}

HRESULT ResourceManager::AddMesh( tstring pName )
{
	O2Mesh* Temp = new O2Mesh(m_lpDevice,m_pFS);
	if(FAILED(Temp->Init(pName.c_str(),this))){
		SAFE_DELETE(Temp);
		_DEBUG_LOG(_T("Failed - InitMesh [ %s ]\n"),pName.c_str());
		return E_FAIL;
	}
	m_pMeshMap[pName] = Temp;
	return S_OK;
}

HRESULT ResourceManager::AddActiveMesh( tstring pName )
{
	O2ActiveMesh* Temp = new O2ActiveMesh(m_lpDevice,m_pFS);
	if(FAILED(Temp->Init(pName.c_str(),this))){
		_DEBUG_LOG(_T("Failed - InitActiveMesh [ %s ]\n"),pName.c_str());
		SAFE_DELETE(Temp);
		return E_FAIL;
	}
	m_pActiveMap[pName] = Temp;
	return S_OK;
}

O2ActiveMesh* ResourceManager::GetActiveMesh( tstring pName )
{
	if( NULL == m_pActiveMap[pName]){
		if(FAILED(AddActiveMesh(pName))){
			MessageBox(NULL,_T("Fail-LoadActiveMesh"),_T("Error"),MB_OK);
		}
	}
	return m_pActiveMap[pName];
}