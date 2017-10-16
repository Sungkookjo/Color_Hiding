#pragma once
#include "O2PassiveObject.h"

#define ITEM_BOOTS	0
#define ITEM_CAPE	1
#define ITEM_DRINK	2
#define ITEM_MAX	3

class O2Item:public O23DPassiveObject{
private:
	UINT m_uEffect;
public:
	O2Item();
	~O2Item(){};
		
	virtual	HRESULT		UpDate();
	void SetAbility(LPVOID	pObj);
};