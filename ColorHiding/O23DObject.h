#pragma once

#include "O2SoundManager.h"

#define FORCE_LIMIT	20.0f
#define LINE_SCAIL_Y	1.005f
#define LINE_SCAIL		1.015f

//////////////////////////////////////////////////////////////////////////
//	3DObject
class O23DObject : public O2Object{
protected:
	LPD3DXMESH*				m_lpMesh				;	// 메쉬
	LPD3DXMESH*				m_lpShadowMesh			;	// 쉐도우 메쉬
	D3DMATERIAL9**			m_lpMeshMaterials		;	// 메쉬의 재질
	LPDIRECT3DTEXTURE9*		m_lpMeshTextures		;	// 메쉬의 텍스처
	DWORD					m_dwNumMaterials		;	// 메쉬의 재질 개수
	NxActor*				m_pActor				;	// 오브젝트의 물리적 성질
	O2CollisionInfo			m_pColl					;	// 오브젝트의 구/박스 충돌 범위
	UINT					m_iColor				;	//	변신 칼라색
	Effect*					m_pEffect				;	// Effect
	D3DXVECTOR4				m_vColor				;	//	오브젝트 색
	float					m_fClWeight				;	//	오브젝트 색

	O2SoundManager*			m_pSoundMgr				;	//	사운드를 가지고 있으면 사용
	VOID				(O23DObject::*m_pfSetMat)();

	// 오브젝트 정리
	virtual	VOID		CleanUp()	=	0;
	// x파일 로드
	virtual HRESULT		LoadXfile(TCHAR* _filename)	=	0;
	// 월드행렬 설정
	virtual	VOID		SetMat();
	virtual VOID		SetMatWithActor();
	virtual VOID		SetMatWithDx();
	// Actor의 Shape 사이즈 변경
	VOID				ChangeShapeSize();
	// Actor의 정보를 Dx 정보로 변환하여 Object class 에 대입
	VOID				UpdateNxtoDx();
	//////////////////////////////////////////////////////////////////////////
	// 임시
public:
	virtual HRESULT		RenderLine();
	virtual HRESULT		RenderShadow(UINT pass) = 0;
	virtual BOOL		IsRender(O2Frustum* pFrustrum);
public:	
	O23DObject();
	virtual ~O23DObject();

	// 초기화
	virtual	HRESULT		Init(LPDIRECT3DDEVICE9* lpDev
		,TCHAR* _filename
		,TCHAR* _objname
		,D3DXVECTOR3 pPos=D3DXVECTOR3(0.0f,0.0f,0.0f)
		,D3DXVECTOR3 pSize=D3DXVECTOR3(1.0f,1.0f,1.0f)
		,D3DXVECTOR3 pRot=D3DXVECTOR3(0.0f,0.0f,0.0f)
		) = 0;

	// 움직임
	virtual	HRESULT		UpDate();
	// 그리기
	virtual	HRESULT		Render() = 0;
	// 충돌처리
	virtual	HRESULT		CollEffect(O2Object* hit)	=	0;
	// 물리 적용
	virtual	HRESULT		CreateActor(UINT _shapes,unsigned int _falg=0,BOOL _isstatic=FALSE);


	//////////////////////////////////////////////////////////////////////////
	// Get	
	NxActor*					GetActor()	{	return m_pActor;	}
	O2CollisionInfo&			GetColl()	{	return m_pColl;		}

	//////////////////////////////////////////////////////////////////////////
	// Set
	virtual	VOID	SetPos(const D3DXVECTOR3& pPos);
	virtual	VOID	SetRot(const D3DXVECTOR3& pRot);
	virtual	VOID	SetSize(const D3DXVECTOR3& pSize);
	virtual	VOID	SetSize(const FLOAT& _size);
	virtual	void	SetForce(D3DXVECTOR3 pForce);
	void			SetForce(float x, float y, float z) { SetForce(D3DXVECTOR3(x,y,z)); }

	virtual	VOID	SetDir(const D3DXVECTOR3& pDir);
	virtual	VOID	SetUp(const D3DXVECTOR3& pUp);
	virtual	VOID	SetRight(const D3DXVECTOR3& pRight);

	//////////////////////////////////////////////////////////////////////////
	// Add
	virtual	VOID	AddPos(const D3DXVECTOR3& pPos);
	virtual	VOID	AddRot(const D3DXVECTOR3& pRot);
	virtual	VOID	AddSize(const FLOAT& _size);
	virtual VOID	AddSize(const D3DXVECTOR3& _size);

	//	이펙트
	VOID					SetEffect(Effect* pEffect)	{ m_pEffect = pEffect;		}
	Effect*					GetEffect()					{ return m_pEffect;			}

	// 칼라 설정
	VOID			SetColor(UINT _col);									//	색 변경
	D3DXVECTOR4		GetColor(D3DCOLORVALUE& pVec)						;	//	pVec에 현재 색을 섞어 리턴
	UINT			GetColor()						{ return m_iColor; }	//	현재 칼라 uint 값 리턴
	LPD3DXMESH*		GetMesh() { return m_lpMesh; }
	// 힘 설정
	void AddForce(D3DXVECTOR3 pForce);
	void AddForce(float x, float y, float z) { AddForce(D3DXVECTOR3(x,y,z)); }

	void SetLineRend(BOOL pBool)	{ m_bLineRend = pBool; }
	BOOL GetLineRend()				{ return m_bLineRend; }

	//	사운드매니저
	void SetSoundManager(O2SoundManager* _sound) { m_pSoundMgr = _sound ;}
};