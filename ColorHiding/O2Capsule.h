#pragma once

#include "O2PassiveObject.h"

class O2Capsule : public O23DPassiveObject
{
private:
	static int s_iCount;
public:
	O2Capsule();
	~O2Capsule();
	virtual	HRESULT		UpDate();
	void SetAbility(LPVOID	pObj);
};