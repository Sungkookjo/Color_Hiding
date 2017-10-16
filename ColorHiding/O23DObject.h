#pragma once

#include "O2SoundManager.h"

#define FORCE_LIMIT	20.0f
#define LINE_SCAIL_Y	1.005f
#define LINE_SCAIL		1.015f

//////////////////////////////////////////////////////////////////////////
//	3DObject
class O23DObject : public O2Object{
protected:
	LPD3DXMESH*				m_lpMesh				;	// �޽�
	LPD3DXMESH*				m_lpShadowMesh			;	// ������ �޽�
	D3DMATERIAL9**			m_lpMeshMaterials		;	// �޽��� ����
	LPDIRECT3DTEXTURE9*		m_lpMeshTextures		;	// �޽��� �ؽ�ó
	DWORD					m_dwNumMaterials		;	// �޽��� ���� ����
	NxActor*				m_pActor				;	// ������Ʈ�� ������ ����
	O2CollisionInfo			m_pColl					;	// ������Ʈ�� ��/�ڽ� �浹 ����
	UINT					m_iColor				;	//	���� Į���
	Effect*					m_pEffect				;	// Effect
	D3DXVECTOR4				m_vColor				;	//	������Ʈ ��
	float					m_fClWeight				;	//	������Ʈ ��

	O2SoundManager*			m_pSoundMgr				;	//	���带 ������ ������ ���
	VOID				(O23DObject::*m_pfSetMat)();

	// ������Ʈ ����
	virtual	VOID		CleanUp()	=	0;
	// x���� �ε�
	virtual HRESULT		LoadXfile(TCHAR* _filename)	=	0;
	// ������� ����
	virtual	VOID		SetMat();
	virtual VOID		SetMatWithActor();
	virtual VOID		SetMatWithDx();
	// Actor�� Shape ������ ����
	VOID				ChangeShapeSize();
	// Actor�� ������ Dx ������ ��ȯ�Ͽ� Object class �� ����
	VOID				UpdateNxtoDx();
	//////////////////////////////////////////////////////////////////////////
	// �ӽ�
public:
	virtual HRESULT		RenderLine();
	virtual HRESULT		RenderShadow(UINT pass) = 0;
	virtual BOOL		IsRender(O2Frustum* pFrustrum);
public:	
	O23DObject();
	virtual ~O23DObject();

	// �ʱ�ȭ
	virtual	HRESULT		Init(LPDIRECT3DDEVICE9* lpDev
		,TCHAR* _filename
		,TCHAR* _objname
		,D3DXVECTOR3 pPos=D3DXVECTOR3(0.0f,0.0f,0.0f)
		,D3DXVECTOR3 pSize=D3DXVECTOR3(1.0f,1.0f,1.0f)
		,D3DXVECTOR3 pRot=D3DXVECTOR3(0.0f,0.0f,0.0f)
		) = 0;

	// ������
	virtual	HRESULT		UpDate();
	// �׸���
	virtual	HRESULT		Render() = 0;
	// �浹ó��
	virtual	HRESULT		CollEffect(O2Object* hit)	=	0;
	// ���� ����
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

	//	����Ʈ
	VOID					SetEffect(Effect* pEffect)	{ m_pEffect = pEffect;		}
	Effect*					GetEffect()					{ return m_pEffect;			}

	// Į�� ����
	VOID			SetColor(UINT _col);									//	�� ����
	D3DXVECTOR4		GetColor(D3DCOLORVALUE& pVec)						;	//	pVec�� ���� ���� ���� ����
	UINT			GetColor()						{ return m_iColor; }	//	���� Į�� uint �� ����
	LPD3DXMESH*		GetMesh() { return m_lpMesh; }
	// �� ����
	void AddForce(D3DXVECTOR3 pForce);
	void AddForce(float x, float y, float z) { AddForce(D3DXVECTOR3(x,y,z)); }

	void SetLineRend(BOOL pBool)	{ m_bLineRend = pBool; }
	BOOL GetLineRend()				{ return m_bLineRend; }

	//	����Ŵ���
	void SetSoundManager(O2SoundManager* _sound) { m_pSoundMgr = _sound ;}
};