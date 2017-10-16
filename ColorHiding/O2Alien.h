#pragma once
#include "O2ActiveObject.h"
#include "O2AI_Alien.h"
#include "O2Hero.h"

class O2Alien:public O23DActiveObject
{
public:
	enum ALIEN_STATE
	{
		ALIEN_STATE_WALK=0,
		ALIEN_STATE_ATTACK=1,
		ALIEN_STATE_MAX=2
	};
protected:
	O2AI_Alien*	m_pAI;
	O2Hero*	m_pHero;
	bool	m_bIsAttack;				//���ݻ���
	bool	m_bIsHit;					//�÷��̾ ��Ҵ��� ����
	bool	m_bAttkSound;

	ALIEN_STATE   m_eState;
	float	m_fAiupdatedelay;			//������Ʈ ������
	float	m_fSightDist;				//�þ� �Ÿ�
	float	m_fSightRad;				//�������� �þ� �ݰ�

	DWORD				m_adwAniId[ALIEN_STATE_MAX]	;	//	���ϸ��̼� �ε���
	D3DXVECTOR3				m_vDestPos			;	//	���� ĳ������ ��ġ��


public:
	O2Alien();
	O2Alien(O2Hero* _hero);
	~O2Alien();
	virtual	VOID		CleanUp();
	virtual	HRESULT		Init(LPDIRECT3DDEVICE9* lpDev
		,TCHAR* _filename
		,TCHAR* _objname
		,D3DXVECTOR3 pPos=D3DXVECTOR3(0.0f,0.0f,0.0f)
		,D3DXVECTOR3 pSize=D3DXVECTOR3(1.0f,1.0f,1.0f)
		,D3DXVECTOR3 pRot=D3DXVECTOR3(0.0f,0.0f,0.0f)
		);

	virtual	HRESULT		UpDate();
 	virtual	HRESULT		Render();
	void				UpDateAnim()			;	//���� ���¿� �´� �ִϷ� ����


	bool				GetIsHit() { return m_bIsHit; }						//�浹�� �Ͼ���� ������
	void				SetIsHit(bool _Ishit) { m_bIsHit =  _Ishit; }
	void				SetRoamPos(D3DXVECTOR3 _Pos) { m_pAI->SetRoamingSpot(_Pos); }
	void				LoadAttributesFromFile(TCHAR* _fname);
	BOOL				PickHero();
/*	BOOL				PickObject(O23DObject* _vObj);*/
	virtual	HRESULT		CollEffect(O2Object* hit);


};