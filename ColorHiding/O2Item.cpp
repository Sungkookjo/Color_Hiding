#include "stdafx.h"
#include "O2Item.h"
#include "O2Hero.h"
#include "Manager.h"

O2Item::O2Item()
{
	m_uType = OBJ_TYPE_ITEM;
	m_uEffect = rand()%ITEM_MAX;
}

HRESULT O2Item::UpDate()
{
	UpdateNxtoDx();
	AddRot(D3DXVECTOR3(0.0f,30.0f*(*m_fpDelta),0.0f));
	return S_OK;
}

void O2Item::SetAbility( LPVOID pObj )
{
	if(m_bIsDead)
		return ;
	else
		m_bIsDead = TRUE;

	O2Hero* pHero = (O2Hero*)pObj;
	Manager* pManager = (Manager*)s_pManager;
	POINT pt={128,128};
	switch(m_uEffect){
		case ITEM_BOOTS:
			if(FAILED(pManager->GetGUIMgr()->CreateTimeSprite(_T("item_speed.png"),pt,D3DXVECTOR3(512.0f,368.0f,0.5f),O2Rect(0,128,0,128),1.0f))){
				_DEBUG_LOG(_T("FAILED\n"));
			}
			_DEBUG_LOG(_T("ITEM_BOOTS\n"));
			pHero->SetSpeed(SPEED_UP);
			break;
		case ITEM_CAPE:
			if(FAILED(pManager->GetGUIMgr()->CreateTimeSprite(_T("item_cape.png"),pt,D3DXVECTOR3(512.0f,368.0f,0.5f),O2Rect(0,128,0,128),1.0f))){
				_DEBUG_LOG(_T("FAILED\n"));
			}
			_DEBUG_LOG(_T("ITEM_CAPE\n"));
			pHero->SetSuperHide();
			break;
		case ITEM_DRINK:
			if(FAILED(pManager->GetGUIMgr()->CreateTimeSprite(_T("item_posion.png"),pt,D3DXVECTOR3(512.0f,368.0f,0.5f),O2Rect(0,128,0,128),1.0f))){
				_DEBUG_LOG(_T("FAILED\n"));
			}
			_DEBUG_LOG(_T("ITEM_DRINK\n"));
			{
				for(int i =0;i<COLOR_MAX;++i){
					pHero->SetColorGage(i,pHero->GetColorMaxGage(i));
				}
			}			
			break;
	}
}