#pragma once

#include "O2Hero.h"
#include "O2UFO.h"
#include "O2Alien.h"
#include "O2PassiveObject.h"
#include "O2ParticleManager.h"
#include "O2Light.h"
#include "O2Item.h"

#define ShadowMap_SIZE 512

class O2ObjectManager{
private:
	list<O2Object*>					m_lAllObjects	;	//	모든 오브젝트들
	list<O23DObject*>				m_l3DObjects	;	//	물리가 적용된 오브젝트들
	O2Hero*							m_pHero			;	//	주인공 오브젝트
	LPDIRECT3DDEVICE9*				m_lpDevice		;	//	디바이스
	ResourceManager*				m_pResource		;	//	리소스
	FLOAT*							m_pfElaped		;	//	보정시간
	Effect*							m_pEffect		;	//	카툰랜더링
	O2ParticleManager*				m_pPartMgr		;	//	파티클 매니저
	O2Light*						m_pLight		;	//	라이트

	LPDIRECT3DTEXTURE9              m_pShadowMap;    // Texture to which the shadow map is rendered
	LPDIRECT3DSURFACE9              m_pDSShadow;     // Depth-stencil buffer for rendering to shadow map
	D3DXMATRIXA16					m_matShadowProj	;

	O2Frustum*						m_pFrustrum		;	//	프러스트럼

	VOID		CleanUp();
	VOID		SetCollEvent(O23DObject* pObj); // 충돌시 이벤트가 발생하게 설정
public:
	O2ObjectManager();
	~O2ObjectManager();
	// 매니저 정리
	VOID	Flush();

	// Updata
	HRESULT UpDate();
	// Render
	HRESULT Render();

	HRESULT	Init(LPDIRECT3DDEVICE9*	lpDev,ResourceManager* pResc,O2ParticleManager* pPart);
	// 오브젝트 생성 ( 테스트용 )
	HRESULT AddPassiveObject(TCHAR* _file
		,UINT _shape=NX_SHAPE_BOX
		,BOOL _static=FALSE
		,D3DXVECTOR3 _pos=D3DXVECTOR3(0.0f,0.0f,0.0f)
		,D3DXVECTOR3 _rot=D3DXVECTOR3(0.0f,0.0f,0.0f)
		,D3DXVECTOR3 _size=D3DXVECTOR3(1.0f,1.0f,1.0f)
		,TCHAR* _ObjName=NULL
		,UINT _ObjColor=COLOR_NONE
		,BOOL _IsShadow = FALSE
		,BOOL _IsReflectShadow=FALSE
		,BOOL _IsEffect=TRUE
		,BOOL _IsLine=TRUE
		,O23DPassiveObject* pObj=NULL	
		,unsigned int _flag = 0
		);
	HRESULT AddActiveObject(TCHAR* _file
		,UINT _shape=NX_SHAPE_BOX
		,BOOL _static=FALSE
		,D3DXVECTOR3 _pos=D3DXVECTOR3(0.0f,0.0f,0.0f)
		,D3DXVECTOR3 _rot=D3DXVECTOR3(0.0f,0.0f,0.0f)
		,D3DXVECTOR3 _size=D3DXVECTOR3(1.0f,1.0f,1.0f)
		,TCHAR* _ObjName=NULL
		,UINT _ObjColor=COLOR_NONE
		,BOOL _IsShadow = FALSE
		,BOOL _IsReflectShadow=FALSE
		,BOOL _IsEffect=TRUE
		,BOOL _IsLine=TRUE
		,O23DActiveObject* pObj=NULL		
		);
	// 주인공 생성
	HRESULT CreateHero(TCHAR* _file
		,D3DXVECTOR3 _pos=D3DXVECTOR3(0.0f,0.0f,0.0f)
		,D3DXVECTOR3 _rot=D3DXVECTOR3(0.0f,0.0f,0.0f)
		,D3DXVECTOR3 _size=D3DXVECTOR3(1.0f,1.0f,1.0f)
		,BOOL _IsShadow = FALSE
		,BOOL _IsReflectShadow=FALSE
		,BOOL _IsEffect=TRUE
		,BOOL _IsLine=TRUE
		);
	HRESULT Add2DObject(
		TCHAR*				filename,
		O2Rect				pRect,
		POINT				imgsize,
		D3DXVECTOR3 pPos,
		D3DXVECTOR3 pSize=D3DXVECTOR3(1.0f,1.0f,1.0f),
		D3DXVECTOR3 pRot=D3DXVECTOR3(0.0f,0.0f,0.0f)
		);
	// 외계인 생성
	HRESULT AddAlien(TCHAR* _file
		,UINT _shape=NX_SHAPE_BOX
		,BOOL _static=FALSE
		,D3DXVECTOR3 _pos=D3DXVECTOR3(0.0f,0.0f,0.0f)
		,D3DXVECTOR3 _rot=D3DXVECTOR3(0.0f,0.0f,0.0f)
		,D3DXVECTOR3 _size=D3DXVECTOR3(1.0f,1.0f,1.0f)
		,TCHAR* _ObjName=NULL
		,UINT _ObjColor=COLOR_NONE
		,BOOL _IsShadow = FALSE
		,BOOL _IsReflectShadow=FALSE
		,BOOL _IsEffect=TRUE
		,BOOL _IsLine=TRUE		
		);

	HRESULT AddItem(TCHAR* _file
		,UINT _shape=NX_SHAPE_BOX
		,BOOL _static=FALSE
		,D3DXVECTOR3 _pos=D3DXVECTOR3(0.0f,0.0f,0.0f)
		,D3DXVECTOR3 _rot=D3DXVECTOR3(0.0f,0.0f,0.0f)
		,D3DXVECTOR3 _size=D3DXVECTOR3(1.0f,1.0f,1.0f)
		,TCHAR* _ObjName=NULL
		,BOOL _IsShadow = FALSE
		,BOOL _IsReflectShadow=FALSE
		,BOOL _IsEffect=TRUE
		,BOOL _IsLine=TRUE
		);
	HRESULT AddCapsule(TCHAR* _file
		,UINT _shape=NX_SHAPE_BOX
		,BOOL _static=FALSE
		,D3DXVECTOR3 _pos=D3DXVECTOR3(0.0f,0.0f,0.0f)
		,D3DXVECTOR3 _rot=D3DXVECTOR3(0.0f,0.0f,0.0f)
		,D3DXVECTOR3 _size=D3DXVECTOR3(1.0f,1.0f,1.0f)
		,TCHAR* _ObjName=NULL
		,BOOL _IsShadow = FALSE
		,BOOL _IsReflectShadow=FALSE
		,BOOL _IsEffect=TRUE
		,BOOL _IsLine=TRUE
		);
	HRESULT CreateAddtionalMob(UINT _index,UINT _stage);

	O2Hero*			GetHero() { return m_pHero; }
	VOID			SetElaped(FLOAT* pTime) { m_pfElaped = pTime; }

	VOID			SetEffect(Effect* pEffect) { m_pEffect = pEffect; }
	Effect*			GetEffect() { return m_pEffect; }

	void			CreateParticle(O23DObject* _it);

	//카메라 충돌처리 
	//플레이어 기준으로 가장 가까운 물체와 카메라의 거리를 반환
// 	float Picking(D3DXVECTOR3* _Pos,D3DXVECTOR3* _Dir,float* _Codist);
// 	float Picking_Mesh(D3DXVECTOR3* _Pos,D3DXVECTOR3* _Dir,float* _Codist);
	VOID			SetLight(O2Light* pLight)	{ m_pLight = pLight; }
	float			GetRandomFloat( float fMin, float fMax );

};