#include "stdafx.h"
#include "O2Capsule.h"
#include "O2Hero.h"
#include "Manager.h"

int O2Capsule::s_iCount = 0;

O2Capsule::O2Capsule()
{
	m_uType = OBJ_TYPE_CAPSULE;
	m_iColor = COLOR_NONE;
	++s_iCount;
}

O2Capsule::~O2Capsule()
{
	--s_iCount;
}

HRESULT O2Capsule::UpDate()
{
	return S_OK;
}

void O2Capsule::SetAbility( LPVOID pObj )
{
	if(m_bIsDead){
		return ;
	}else{
		m_bIsDead = TRUE;
	}

	O2Hero* pHero = (O2Hero*)pObj;
	if(s_iCount-1){
		pHero->SetColorVisible(m_iColor);
	}else{
		pHero->SetColorVisible(m_iColor);
		pHero->SetState(O2Hero::STATE_CLEAR);
	}
	((Manager*)s_pManager)->GetSoundMgr()->Play2DEffect(_T("GetCapsule"),O2SoundManager::S_TYPE_MENU);
}