#pragma once

#include "O2PassiveObject.h"

class Arrow : public O23DPassiveObject{
private:
	float			m_fCurSize;
	float			m_fDir;
public:
	Arrow();
	~Arrow();

	virtual HRESULT UpDate();
};