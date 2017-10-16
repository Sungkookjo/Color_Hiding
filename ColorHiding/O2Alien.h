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
	bool	m_bIsAttack;				//공격상태
	bool	m_bIsHit;					//플레이어를 잡았는지 상태
	bool	m_bAttkSound;

	ALIEN_STATE   m_eState;
	float	m_fAiupdatedelay;			//업데이트 딜레이
	float	m_fSightDist;				//시야 거리
	float	m_fSightRad;				//구형태의 시야 반경

	DWORD				m_adwAniId[ALIEN_STATE_MAX]	;	//	에니메이션 인덱스
	D3DXVECTOR3				m_vDestPos			;	//	이전 캐릭터의 위치값


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
	void				UpDateAnim()			;	//현재 상태에 맞는 애니로 변경


	bool				GetIsHit() { return m_bIsHit; }						//충돌이 일어났는지 얻어오기
	void				SetIsHit(bool _Ishit) { m_bIsHit =  _Ishit; }
	void				SetRoamPos(D3DXVECTOR3 _Pos) { m_pAI->SetRoamingSpot(_Pos); }
	void				LoadAttributesFromFile(TCHAR* _fname);
	BOOL				PickHero();
/*	BOOL				PickObject(O23DObject* _vObj);*/
	virtual	HRESULT		CollEffect(O2Object* hit);


};