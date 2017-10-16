#pragma once
#include "O2PassiveObject.h"

class O2AI_UFO:public O2AI
{
public:

	O2AI_UFO();
	~O2AI_UFO();

	virtual void	Roaming();
	virtual void	SearchPlayer();
	virtual void	FollowPlayer(D3DXVECTOR3 _TargetPosition);
	virtual void	AttackPlayer(D3DXVECTOR3 _TargetPosition,
		bool* _IsAttack,bool* _IsHit);
	void	SetUFO(O23DPassiveObject* _pUFO) { m_pUFO = _pUFO; }
	virtual void	Update(D3DXVECTOR3 _TargetPos,
		BOOL _TargetHideState,float _Elapstime,bool* _IsAttack,bool* _IsHit);

protected:
	O23DPassiveObject*	   m_pUFO;

};