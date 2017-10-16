#pragma once
#include "O2PassiveObject.h"
#include "O2ActiveObject.h"
#include "O2Hero.h"
#include "O2AI_UFO.h"

class O2UFO:public O23DPassiveObject
{
protected:
	O2AI_UFO*	m_pAI;
	O2Hero*	m_pHero;
	O23DActiveObject*	m_pTongue;
	bool	m_bIsAttack;				//UFO의 공격상태
	bool	m_bIsHit;					//혀가 플레이어를 잡았는지 상태
	bool	m_bUfoSound;				//UFO가 지속적으로 내는 효과음


public:
	O2UFO(O2Hero* _hero);
	O2UFO();
	~O2UFO();
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
	virtual HRESULT		RenderLine();
	virtual HRESULT		RenderShadow(UINT pass);


	bool				GetIsHit() { return m_bIsHit; }						//충돌이 일어났는지 얻어오기
	void				SetIsHit(bool _Ishit) { m_bIsHit =  _Ishit; }
	void				SetRoamPos(D3DXVECTOR3 _Pos) { m_pAI->SetRoamingSpot(_Pos); }
	void				LoadAttributesFromFile(TCHAR* _fname);
	
};