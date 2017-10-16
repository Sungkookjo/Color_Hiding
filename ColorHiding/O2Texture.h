//////////////////////////////////////////////////////////////////////////
// Texture Buffer Ŭ����
// �ؽ��� ���� Ŭ����
#pragma once

// A class for our vertexbuffer
class O2Texture{
private:
	Basefw_FS*			m_pFS;
	LPDIRECT3DDEVICE9*	m_lpDevice;	// Device
	LPDIRECT3DTEXTURE9	m_lpTexture; // Buffer to hold vertices
private:
	void Cleanup();
public:
	O2Texture(LPDIRECT3DDEVICE9* pDevice,Basefw_FS* FS);
	~O2Texture();
	HRESULT Init(const TCHAR* filename);
	LPDIRECT3DTEXTURE9& GetTexture() { return m_lpTexture; }
};