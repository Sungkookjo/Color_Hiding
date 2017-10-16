#pragma once
#include "O23DObject.h"
//////////////////////////////////////////////////////////////////////////
//	PassiveObject
class O23DPassiveObject : public O23DObject{
protected:
	UINT						m_uNumVertices		;	//	버텍스 개수
	UINT						m_uNumFaces			;	//	삼각형 개수

	// 오브젝트 정리
	virtual	VOID		CleanUp();
	// x파일 로드
	virtual HRESULT		LoadXfile(TCHAR* _filename);
public:
	O23DPassiveObject();
	~O23DPassiveObject();
	// 초기화
	virtual	HRESULT		Init(LPDIRECT3DDEVICE9* lpDev
		,TCHAR* _filename
		,TCHAR* _objname
		,D3DXVECTOR3 pPos=D3DXVECTOR3(0.0f,0.0f,0.0f)
		,D3DXVECTOR3 pSize=D3DXVECTOR3(1.0f,1.0f,1.0f)
		,D3DXVECTOR3 pRot=D3DXVECTOR3(0.0f,0.0f,0.0f)
		);
	// 움직임
	virtual	HRESULT		UpDate();
	// 그리기
	virtual	HRESULT		Render();
	// 충돌처리
	virtual	HRESULT		CollEffect(O2Object* hit);

	// 패시브 오브젝트에는 Mesh 가 추가 
	virtual	HRESULT		CreateActor(UINT _shapes,unsigned int _flag=0,BOOL _isstatic=FALSE);
	HRESULT GenerateTriangleMeshFromDXMesh(unsigned int _flag,BOOL _isstatic=FALSE);
	virtual HRESULT		RenderShadow(UINT pass);
};
