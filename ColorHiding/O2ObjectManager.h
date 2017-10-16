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
	list<O2Object*>					m_lAllObjects	;	//	��� ������Ʈ��
	list<O23DObject*>				m_l3DObjects	;	//	������ ����� ������Ʈ��
	O2Hero*							m_pHero			;	//	���ΰ� ������Ʈ
	LPDIRECT3DDEVICE9*				m_lpDevice		;	//	����̽�
	ResourceManager*				m_pResource		;	//	���ҽ�
	FLOAT*							m_pfElaped		;	//	�����ð�
	Effect*							m_pEffect		;	//	ī��������
	O2ParticleManager*				m_pPartMgr		;	//	��ƼŬ �Ŵ���
	O2Light*						m_pLight		;	//	����Ʈ

	LPDIRECT3DTEXTURE9              m_pShadowMap;    // Texture to which the shadow map is rendered
	LPDIRECT3DSURFACE9              m_pDSShadow;     // Depth-stencil buffer for rendering to shadow map
	D3DXMATRIXA16					m_matShadowProj	;

	O2Frustum*						m_pFrustrum		;	//	������Ʈ��

	VOID		CleanUp();
	VOID		SetCollEvent(O23DObject* pObj); // �浹�� �̺�Ʈ�� �߻��ϰ� ����
public:
	O2ObjectManager();
	~O2ObjectManager();
	// �Ŵ��� ����
	VOID	Flush();

	// Updata
	HRESULT UpDate();
	// Render
	HRESULT Render();

	HRESULT	Init(LPDIRECT3DDEVICE9*	lpDev,ResourceManager* pResc,O2ParticleManager* pPart);
	// ������Ʈ ���� ( �׽�Ʈ�� )
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
	// ���ΰ� ����
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
	// �ܰ��� ����
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

	//ī�޶� �浹ó�� 
	//�÷��̾� �������� ���� ����� ��ü�� ī�޶��� �Ÿ��� ��ȯ
// 	float Picking(D3DXVECTOR3* _Pos,D3DXVECTOR3* _Dir,float* _Codist);
// 	float Picking_Mesh(D3DXVECTOR3* _Pos,D3DXVECTOR3* _Dir,float* _Codist);
	VOID			SetLight(O2Light* pLight)	{ m_pLight = pLight; }
	float			GetRandomFloat( float fMin, float fMax );

};