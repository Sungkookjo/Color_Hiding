#pragma once
#include "O23DObject.h"

#define MOVE_DIGREE_SIDE	0.7f
#define MOVE_DIGREE_BACK	0.5f

#define ANIMINDEX_FAIL		0xffffffff

#define ANIM_LOOP	 0
#define ANIM_ONCE	 1

//////////////////////////////////////////////////////////////////////////
//	ActiveObject
//	피직스로 부터 위치 값을 제공받고 회전 값을 제공한다
//	~피직스에 적용된 회전행렬 쌩까고 위치 값만 받아 씀
class O23DActiveObject : public O23DObject{
protected:
	float				m_fSpeed;							//	이동속도
	// 오브젝트 정리
	virtual	VOID		CleanUp();
	// x파일 로드
	virtual HRESULT		LoadXfile(TCHAR* _filename);

	virtual VOID		SetMatWithActor();

	O2AI*				m_pAI;					//AI

	LPD3DXFRAME*                m_pFrameRoot			;	//	프레임
	LPD3DXANIMATIONCONTROLLER   m_pAnimController		;	//	애니 컨트롤러
	D3DXMATRIXA16*              m_pBoneMatrices			;	//	본 행렬
	D3DXMATRIXA16*				m_pLineBoneMatrices		;	//	외곽선을 위한 본행렬
	UINT                        m_NumBoneMatricesMax	;	//	본 행렬 개수
	DWORD						m_dwMaxTracks			;	//	최대 애니 트랙수

	DWORD						m_dwCurTrack			;	//	현재 애니 트렉
	UINT						m_iState				;	//	오브젝트의 현재 상태
	float						m_fCurTime				;	//	현재 시작
	float						m_fPreTime				;	//	이전 시간

	BOOL						m_bIsAniStop			;	//	애니가 정지 했는지?
	INT							m_iCount				;	//	애니메이션 재생 횟수 제한
	INT							m_iCurCount				;	//	현재 재생 횟수
	double						m_lfDestTime			;	//	애니메이션 시작때 시간
	double						m_lfAniTime				;	//	현재 애니메이션의 time

	// 애니메이션 인덱스를 얻어옴
	DWORD				GetAniIndex( LPCSTR name);

	// 기본 메쉬 랜더 ( Cell Shading )
	virtual void		DrawFrame( LPD3DXFRAME pFrame );
	virtual HRESULT		DrawMeshContainer( LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase );
	
	// 외곽선 랜더
	virtual void		DrawFrameLine( LPD3DXFRAME pFrame );
	virtual HRESULT		DrawMeshLiner( LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase );
	
	// 그림자 랜더
	virtual void		DrawFrameShadow( LPD3DXFRAME pFrame );
	virtual HRESULT		DrawMeshShadow( LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase );

public:
	O23DActiveObject();
	~O23DActiveObject();
	// 초기화
	virtual	HRESULT		Init(LPDIRECT3DDEVICE9* lpDev
		,TCHAR* _filename
		,TCHAR* _objname
		,D3DXVECTOR3 pPos=D3DXVECTOR3(0.0f,0.0f,0.0f)
		,D3DXVECTOR3 pSize=D3DXVECTOR3(1.0f,1.0f,1.0f)
		,D3DXVECTOR3 pRot=D3DXVECTOR3(0.0f,0.0f,0.0f)
		);
	// 캐릭터 움직임 관련
	VOID				MoveFront(float _increement = 1.0f);
	VOID				MoveBack(float _increement = 1.0f);
	VOID				MoveLeft(float _increement = 1.0f);
	VOID				MoveRight(float _increement = 1.0f);
	VOID				MoveDown();
	VOID				MoveUp(float _incree = 1.0f);
	VOID				TurnLeft();
	VOID				TurnRight();

	// 움직임
	virtual	HRESULT		UpDate();
	// 그리기
	virtual	HRESULT		Render();
	// 외곽선 랜더
	virtual HRESULT		RenderLine();
	// 그람자 랜더
	virtual HRESULT		RenderShadow(UINT pass);

	// 충돌처리
	virtual	HRESULT		CollEffect(O2Object* hit);

	// 프레임 매트릭스를 업데이트
	void UpdateFrameMatrices( LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix );
	void UpdateSkinningMethod( LPD3DXFRAME pFrameBase );

	virtual VOID		ChangeState(UINT pState);
	virtual	void		StopAnim();
	virtual	void		PlayAnim(INT pCount = ANIM_LOOP,BOOL isInit=TRUE);

	BOOL				GetIsAniStop() { return m_bIsAniStop; }
	// 애니메이션 재생 횟수 설정 
	void				SetAniCount(INT pCount=ANIM_LOOP) { 
		m_iCount = pCount;
		m_iCurCount=0;
	}

	void				SetSpeed(float pSpeed)	{ m_fSpeed = pSpeed;	}
	float				GetSpeed()				{ return m_fSpeed;		}
	virtual	VOID		UpDateAnim();
};
