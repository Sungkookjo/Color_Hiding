#pragma once
#include "O23DObject.h"

#define MOVE_DIGREE_SIDE	0.7f
#define MOVE_DIGREE_BACK	0.5f

#define ANIMINDEX_FAIL		0xffffffff

#define ANIM_LOOP	 0
#define ANIM_ONCE	 1

//////////////////////////////////////////////////////////////////////////
//	ActiveObject
//	�������� ���� ��ġ ���� �����ް� ȸ�� ���� �����Ѵ�
//	~�������� ����� ȸ����� �߱�� ��ġ ���� �޾� ��
class O23DActiveObject : public O23DObject{
protected:
	float				m_fSpeed;							//	�̵��ӵ�
	// ������Ʈ ����
	virtual	VOID		CleanUp();
	// x���� �ε�
	virtual HRESULT		LoadXfile(TCHAR* _filename);

	virtual VOID		SetMatWithActor();

	O2AI*				m_pAI;					//AI

	LPD3DXFRAME*                m_pFrameRoot			;	//	������
	LPD3DXANIMATIONCONTROLLER   m_pAnimController		;	//	�ִ� ��Ʈ�ѷ�
	D3DXMATRIXA16*              m_pBoneMatrices			;	//	�� ���
	D3DXMATRIXA16*				m_pLineBoneMatrices		;	//	�ܰ����� ���� �����
	UINT                        m_NumBoneMatricesMax	;	//	�� ��� ����
	DWORD						m_dwMaxTracks			;	//	�ִ� �ִ� Ʈ����

	DWORD						m_dwCurTrack			;	//	���� �ִ� Ʈ��
	UINT						m_iState				;	//	������Ʈ�� ���� ����
	float						m_fCurTime				;	//	���� ����
	float						m_fPreTime				;	//	���� �ð�

	BOOL						m_bIsAniStop			;	//	�ִϰ� ���� �ߴ���?
	INT							m_iCount				;	//	�ִϸ��̼� ��� Ƚ�� ����
	INT							m_iCurCount				;	//	���� ��� Ƚ��
	double						m_lfDestTime			;	//	�ִϸ��̼� ���۶� �ð�
	double						m_lfAniTime				;	//	���� �ִϸ��̼��� time

	// �ִϸ��̼� �ε����� ����
	DWORD				GetAniIndex( LPCSTR name);

	// �⺻ �޽� ���� ( Cell Shading )
	virtual void		DrawFrame( LPD3DXFRAME pFrame );
	virtual HRESULT		DrawMeshContainer( LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase );
	
	// �ܰ��� ����
	virtual void		DrawFrameLine( LPD3DXFRAME pFrame );
	virtual HRESULT		DrawMeshLiner( LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase );
	
	// �׸��� ����
	virtual void		DrawFrameShadow( LPD3DXFRAME pFrame );
	virtual HRESULT		DrawMeshShadow( LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase );

public:
	O23DActiveObject();
	~O23DActiveObject();
	// �ʱ�ȭ
	virtual	HRESULT		Init(LPDIRECT3DDEVICE9* lpDev
		,TCHAR* _filename
		,TCHAR* _objname
		,D3DXVECTOR3 pPos=D3DXVECTOR3(0.0f,0.0f,0.0f)
		,D3DXVECTOR3 pSize=D3DXVECTOR3(1.0f,1.0f,1.0f)
		,D3DXVECTOR3 pRot=D3DXVECTOR3(0.0f,0.0f,0.0f)
		);
	// ĳ���� ������ ����
	VOID				MoveFront(float _increement = 1.0f);
	VOID				MoveBack(float _increement = 1.0f);
	VOID				MoveLeft(float _increement = 1.0f);
	VOID				MoveRight(float _increement = 1.0f);
	VOID				MoveDown();
	VOID				MoveUp(float _incree = 1.0f);
	VOID				TurnLeft();
	VOID				TurnRight();

	// ������
	virtual	HRESULT		UpDate();
	// �׸���
	virtual	HRESULT		Render();
	// �ܰ��� ����
	virtual HRESULT		RenderLine();
	// �׶��� ����
	virtual HRESULT		RenderShadow(UINT pass);

	// �浹ó��
	virtual	HRESULT		CollEffect(O2Object* hit);

	// ������ ��Ʈ������ ������Ʈ
	void UpdateFrameMatrices( LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix );
	void UpdateSkinningMethod( LPD3DXFRAME pFrameBase );

	virtual VOID		ChangeState(UINT pState);
	virtual	void		StopAnim();
	virtual	void		PlayAnim(INT pCount = ANIM_LOOP,BOOL isInit=TRUE);

	BOOL				GetIsAniStop() { return m_bIsAniStop; }
	// �ִϸ��̼� ��� Ƚ�� ���� 
	void				SetAniCount(INT pCount=ANIM_LOOP) { 
		m_iCount = pCount;
		m_iCurCount=0;
	}

	void				SetSpeed(float pSpeed)	{ m_fSpeed = pSpeed;	}
	float				GetSpeed()				{ return m_fSpeed;		}
	virtual	VOID		UpDateAnim();
};
