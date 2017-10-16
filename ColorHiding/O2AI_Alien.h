#pragma once

#include "O2ActiveObject.h"
class O2AI_Alien:public O2AI
{
public:

	O2AI_Alien();
	~O2AI_Alien();


	float	GetRotRad() { return m_fRot; }
	void	SetInSight(BOOL _Check) { m_bInSight = _Check; }
	void	SetAlien(O23DActiveObject* _pAlien) { m_pAlien = _pAlien; }

//	방향 업데이트
	void	UpdateDirection(D3DXVECTOR3 _TargetPos);

	virtual void	Update(D3DXVECTOR3 _TargetPos,
		BOOL _TargetHideState,float _Elapstime,bool* _IsAttack,bool* _IsHit);
	
	virtual void	Roaming();
	virtual void	SearchPlayer();
	virtual void	FollowPlayer(D3DXVECTOR3 _TargetPosition);
	virtual void	AttackPlayer(D3DXVECTOR3 _TargetPosition,
		bool* _IsAttack,bool* _IsHit);



protected:	
	BOOL		   m_bInSight;
	float		   m_fRot;
	float		   m_fWaveweight;
	O23DActiveObject*	   m_pAlien;
};