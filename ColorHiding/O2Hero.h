#pragma once
#include "O2ActiveObject.h"

#define IDLE_TRANSITION_TIME 0.125f
#define MOVE_TRANSITION_TIME 0.25f

#define		COLOR_GAGE_MAX	50.0f
#define		TIME_SUPERHIDE	5.0f
#define		SPEED_NORMAL	15.0f
#define		SPEED_UP		15.0f
class O2InputManager;
//////////////////////////////////////////////////////////////////////////
// O2Hero
// 주인공 클래스
class O2Hero:public O23DActiveObject{
public:
	enum {STATE_IDLE=0
		,STATE_WALK
		,STATE_RUN
		,STATE_BACK
		,STATE_DANCE
		,STATE_JUMP1
		,STATE_JUMP2
		,STATE_JUMP3
		,STATE_DIE
		,STATE_CLEAR
		,STATE_MAX
	};
private:

	LPDIRECT3DVERTEXBUFFER9	m_lpVertexBuffer	;	//	버텍스 버퍼
	LPDIRECT3DINDEXBUFFER9	m_lpIndexBuffer		;	//	인덱스 버퍼

	D3DXVECTOR3				m_vDestPos			;	//	이전 캐릭터의 위치값
													//	이것으로 점프할거임

	BOOL				m_bHide					;	//	캐릭터가 숨었는지?
	BOOL				m_bHideSound			;	//  하이딩 사운드를 플레이할것인지
													//	충돌시 충돌한 물체랑 색 비교

	BOOL				m_bSuperHide			;	//	슈퍼하이딩
	float				m_fSuperHideTime		;	//	슈퍼 하이딩 타임
	BOOL				m_bDestUseLight			;	//	라이트 효과 받는지
	BOOL				m_bDestRendLine			;	//	라인 랜더하는지
	BOOL				m_bDestReflectShadow	;	//	그림자 받는지
	BOOL				m_bDestRendShadow		;	//	그림자 그리는지

	DWORD				m_adwAniId[STATE_MAX]	;	//	에니메이션 인덱스
	float				m_afAniSpeed[STATE_MAX]	;	//	애니메이션 속도
	float				m_fColorDam				;	//	칼라 줄어드는 값

	O2InputManager*		m_pInput				;	//	인풋 매니저

	void				UpDateAnim()			;	//현재 상태에 맞는 애니로 변경

	//////////////////////////////////////////////////////////////////////////
	// 캐릭터 정보
	float				m_fMaxSpeed					;	//	캐릭터 최대 속도
	float				m_fBonusSpeed				;	//	보너스 이동 속도

	float				m_faColorGage[COLOR_MAX]	;	//	칼라 현재 게이지
	float				m_faMaxColGage[COLOR_MAX]	;	//	칼라 최대 게이지
	BOOL				m_bIsVisible[COLOR_MAX]		;	//	칼라 활성화
	float				m_fAdjustime				;	//	충돌처리 시간조정



public:
	O2Hero();
	~O2Hero();
	// 충돌처리
	virtual	HRESULT		CollEffect(O2Object* hit);

	HRESULT UpDate();
	// 초기화
	virtual	HRESULT		Init(LPDIRECT3DDEVICE9* lpDev
		,TCHAR* _filename
		,TCHAR* _objname
		,D3DXVECTOR3 pPos=D3DXVECTOR3(0.0f,0.0f,0.0f)
		,D3DXVECTOR3 pSize=D3DXVECTOR3(1.0f,1.0f,1.0f)
		,D3DXVECTOR3 pRot=D3DXVECTOR3(0.0f,0.0f,0.0f)
		);

	// 주인공의 키입력 처리
	void ProcInput(O2InputManager* pInput);
	void ProcInputHelp(O2InputManager* pInput);	//	테스트용

	// 캐릭터 스테이트(상태) 변경, 이때 애니메이션을 바꿔줄거임
	VOID		ChangeState(UINT pState);
	VOID		SetState(UINT pState) { ChangeState(pState); }
	UINT		GetState() { return m_iState; }
	// 캐릭터가 숨었는지 리턴
	BOOL		GetHide() { return m_bHide; }
	VOID		SetHide(BOOL pHide) { m_bHide = pHide; }
	float		GetADTime() { return m_fAdjustime; }

	void		SetDam(float dam) { m_fColorDam = dam; }
	VOID		SetFullColor();
	VOID		SetColorMaxGage(UINT pColor,float Gage) { m_faMaxColGage[pColor]=Gage; }
	VOID		SetColorGage(UINT pColor,float Gage) { m_faColorGage[pColor]=Gage; }
	float		GetColorMaxGage(UINT pColor) { return m_faMaxColGage[pColor]; }
	float		GetColorGage(UINT pColor) { return m_faColorGage[pColor]; }
	BOOL*		GetColorVisible(UINT pColor) { return &m_bIsVisible[pColor]; }
	VOID		SetColorVisible(UINT pColor,BOOL pvisible = TRUE);
	VOID		SetMaxColor(float fcolor);

	VOID		SetSuperHide();
	BOOL		GetIsSuperHide() { return m_bSuperHide; }
};