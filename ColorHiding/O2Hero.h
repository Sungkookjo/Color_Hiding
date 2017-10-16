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
// ���ΰ� Ŭ����
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

	LPDIRECT3DVERTEXBUFFER9	m_lpVertexBuffer	;	//	���ؽ� ����
	LPDIRECT3DINDEXBUFFER9	m_lpIndexBuffer		;	//	�ε��� ����

	D3DXVECTOR3				m_vDestPos			;	//	���� ĳ������ ��ġ��
													//	�̰����� �����Ұ���

	BOOL				m_bHide					;	//	ĳ���Ͱ� ��������?
	BOOL				m_bHideSound			;	//  ���̵� ���带 �÷����Ұ�����
													//	�浹�� �浹�� ��ü�� �� ��

	BOOL				m_bSuperHide			;	//	�������̵�
	float				m_fSuperHideTime		;	//	���� ���̵� Ÿ��
	BOOL				m_bDestUseLight			;	//	����Ʈ ȿ�� �޴���
	BOOL				m_bDestRendLine			;	//	���� �����ϴ���
	BOOL				m_bDestReflectShadow	;	//	�׸��� �޴���
	BOOL				m_bDestRendShadow		;	//	�׸��� �׸�����

	DWORD				m_adwAniId[STATE_MAX]	;	//	���ϸ��̼� �ε���
	float				m_afAniSpeed[STATE_MAX]	;	//	�ִϸ��̼� �ӵ�
	float				m_fColorDam				;	//	Į�� �پ��� ��

	O2InputManager*		m_pInput				;	//	��ǲ �Ŵ���

	void				UpDateAnim()			;	//���� ���¿� �´� �ִϷ� ����

	//////////////////////////////////////////////////////////////////////////
	// ĳ���� ����
	float				m_fMaxSpeed					;	//	ĳ���� �ִ� �ӵ�
	float				m_fBonusSpeed				;	//	���ʽ� �̵� �ӵ�

	float				m_faColorGage[COLOR_MAX]	;	//	Į�� ���� ������
	float				m_faMaxColGage[COLOR_MAX]	;	//	Į�� �ִ� ������
	BOOL				m_bIsVisible[COLOR_MAX]		;	//	Į�� Ȱ��ȭ
	float				m_fAdjustime				;	//	�浹ó�� �ð�����



public:
	O2Hero();
	~O2Hero();
	// �浹ó��
	virtual	HRESULT		CollEffect(O2Object* hit);

	HRESULT UpDate();
	// �ʱ�ȭ
	virtual	HRESULT		Init(LPDIRECT3DDEVICE9* lpDev
		,TCHAR* _filename
		,TCHAR* _objname
		,D3DXVECTOR3 pPos=D3DXVECTOR3(0.0f,0.0f,0.0f)
		,D3DXVECTOR3 pSize=D3DXVECTOR3(1.0f,1.0f,1.0f)
		,D3DXVECTOR3 pRot=D3DXVECTOR3(0.0f,0.0f,0.0f)
		);

	// ���ΰ��� Ű�Է� ó��
	void ProcInput(O2InputManager* pInput);
	void ProcInputHelp(O2InputManager* pInput);	//	�׽�Ʈ��

	// ĳ���� ������Ʈ(����) ����, �̶� �ִϸ��̼��� �ٲ��ٰ���
	VOID		ChangeState(UINT pState);
	VOID		SetState(UINT pState) { ChangeState(pState); }
	UINT		GetState() { return m_iState; }
	// ĳ���Ͱ� �������� ����
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