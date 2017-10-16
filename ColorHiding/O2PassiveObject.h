#pragma once
#include "O23DObject.h"
//////////////////////////////////////////////////////////////////////////
//	PassiveObject
class O23DPassiveObject : public O23DObject{
protected:
	UINT						m_uNumVertices		;	//	���ؽ� ����
	UINT						m_uNumFaces			;	//	�ﰢ�� ����

	// ������Ʈ ����
	virtual	VOID		CleanUp();
	// x���� �ε�
	virtual HRESULT		LoadXfile(TCHAR* _filename);
public:
	O23DPassiveObject();
	~O23DPassiveObject();
	// �ʱ�ȭ
	virtual	HRESULT		Init(LPDIRECT3DDEVICE9* lpDev
		,TCHAR* _filename
		,TCHAR* _objname
		,D3DXVECTOR3 pPos=D3DXVECTOR3(0.0f,0.0f,0.0f)
		,D3DXVECTOR3 pSize=D3DXVECTOR3(1.0f,1.0f,1.0f)
		,D3DXVECTOR3 pRot=D3DXVECTOR3(0.0f,0.0f,0.0f)
		);
	// ������
	virtual	HRESULT		UpDate();
	// �׸���
	virtual	HRESULT		Render();
	// �浹ó��
	virtual	HRESULT		CollEffect(O2Object* hit);

	// �нú� ������Ʈ���� Mesh �� �߰� 
	virtual	HRESULT		CreateActor(UINT _shapes,unsigned int _flag=0,BOOL _isstatic=FALSE);
	HRESULT GenerateTriangleMeshFromDXMesh(unsigned int _flag,BOOL _isstatic=FALSE);
	virtual HRESULT		RenderShadow(UINT pass);
};
