#include "stdafx.h"
#include "O2GUIButton.h"
#include "O2SoundManager.h"


//////////////////////////////////////////////////////////////////////////
// O2GUIButton::O2GUIButton()
// �Լ��� : O2GUIButton
// ���� : O2GUIButton �ɹ����� �ʱ�ȭ
// �ۼ��� : ������
// �ۼ��� : 08 / 07 / 07 
//////////////////////////////////////////////////////////////////////////
O2GUIButton::O2GUIButton()
:m_vCheckClick(BUTTONSTATE::NORMAL)
,m_bUpButtonSound(FALSE)
{
	for(int i=0;i<BUTTONMAX;++i){
		m_pButtons[i] = NULL;
	}
	m_vRect.left = m_vRect.right = m_vRect.top = m_vRect.bottom = 0L;
}
O2GUIButton::O2GUIButton(O2Rect p_Rect)
:m_vCheckClick(BUTTONSTATE::NORMAL)
,m_bUpButtonSound(FALSE)
{
	for(int i=0;i<BUTTONMAX;++i){
		m_pButtons[i] = NULL;
	}
	m_vRect = p_Rect;
}

O2GUIButton::O2GUIButton(O2GUIButton& p_Button)
:m_vCheckClick(BUTTONSTATE::NORMAL)
{
	for(int i=0;i<BUTTONMAX;++i){
		m_pButtons[i] = NULL;
	}
	*this = p_Button;
}

// ������ ����
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// O2GUIButton::~O2GUIButton()
// �Լ��� : ~O2GUIButton
// ���� : O2GUIButton �ɹ����� �ʱ�ȭ
// �ۼ��� : ������
// �ۼ��� : 08 / 07 / 07 
//////////////////////////////////////////////////////////////////////////
O2GUIButton::~O2GUIButton()
{
	for(int i=0;i<BUTTONMAX;++i){
		SAFE_DELETE(m_pButtons[i]);
	}
}


//////////////////////////////////////////////////////////////////////////
// void O2GUIButton::Render()
// �Լ��� : Render
// �Ű����� : void
// ���ϰ� : void
// ���� : ��ư���¿� �´� ��ư GUI ����
// �ۼ��� : ������
// �ۼ��� : 08 / 07 / 07 
//////////////////////////////////////////////////////////////////////////
void O2GUIButton::Render()
{
	m_pButtons[m_vCheckClick]->Render();
}


//////////////////////////////////////////////////////////////////////////
// bool O2GUIButton::IsInRect(POINT& pt)
// �Լ��� : IsInButton
// �Ű����� : POINT& pt
// ���ϰ� : bool
// ���� : ���콺 �����Ͱ� ��ư �ȿ� ����ִ��� üũ.
// �ۼ��� : ������
// �ۼ��� : 08 / 07 / 07 
//////////////////////////////////////////////////////////////////////////
BOOL O2GUIButton::IsInRect(POINT& pt)
{
	return m_vRect.IsInRect(pt);
}

//////////////////////////////////////////////////////////////////////////
// void O2GUIButton::L_SetOn(char* p_fileName,RECT p_Rect,D3DXVECTOR3 p_Pos)
// �Լ��� : L_SetOn
// �Ű����� : (char* p_fileName,RECT p_Rect,D3DXVECTOR3 p_Pos)
// ���ϰ� : void
// ���� : on / off ��ư ����
// �ۼ��� : ������
// �ۼ��� : 08 / 07 / 07 
//////////////////////////////////////////////////////////////////////////
HRESULT O2GUIButton::SetButton(tstring p_fileName,UINT p_State,O2Rect p_Rect)
{
	return m_pButtons[p_State]->Init(p_fileName
		,m_pResource
		,m_lpDevice
		,m_pImgSize
		,p_Rect
		,m_vPos
		);
}

//////////////////////////////////////////////////////////////////////////
// O2GUIButton::SetRect(RECT& p_Rect)
// �Լ��� : SetRect
// ���� : ��ư�� Rect ( Ŭ�� ��ȿ ���� ) ����
// �ۼ��� : ������
// �ۼ��� : 08 / 07 / 07 
//////////////////////////////////////////////////////////////////////////
void O2GUIButton::SetRect(O2Rect p_Rect)
{
	m_vRect = p_Rect;
}

void O2GUIButton::SetPos(D3DXVECTOR3 p_Pos)
{
	m_vPos = p_Pos;
}

HRESULT O2GUIButton::Init( tstring filename
						  ,ResourceManager* pResource
						  ,LPDIRECT3DDEVICE9* pDev
						  ,POINT	imgSize
						  ,O2Rect pRect
						  ,D3DXVECTOR3 pPos
						  ,UINT		_Center
						  )
{
	float width,height;
	m_lpDevice = pDev;
	m_pResource = pResource;
	m_vPos = pPos;

	m_vRect = pRect;

	width = m_vRect.GetWidth();
	height= m_vRect.GetHeight();

	m_vRect.left = 0.0f  - (width*0.5f) + m_vPos.x;
	m_vRect.right = width - (width*0.5f) + m_vPos.x;
	m_vRect.top = 0.0f - (height*0.5f) + m_vPos.y;
	m_vRect.bottom = height - (height*0.5f) + m_vPos.y;

	m_tFileName = filename;
	m_pImgSize = imgSize;
	for(int i=0;i<BUTTONMAX;++i){
		m_pButtons[i] = new O2GUIObject;
	}

	return S_OK;
}

UINT O2GUIButton::Update( POINT& pt,UINT _MsState )
{	
	if(IsInRect(pt)){
		switch(_MsState)
		{
		case MOUSE_CLICK:
			if( UI_SOUND == m_vIndex){
				SetOnOff(MOUSE_UP);
				ChangeTexture();
				return UI_SOUND;
			}
			O2SOUNDINSTANCE->Play2DEffect(_T("ButtonClick"),O2SoundManager::S_TYPE_MENU,NULL);
			return m_vIndex;
			break;
		case MOUSE_DOWN:
			SetOnOff(MOUSE_CLICK);
		    break;
		case MOUSE_UP:
			SetOnOff(MOUSE_DOWN);
			if(m_bUpButtonSound==FALSE)
			{
				m_bUpButtonSound=1;
				printf("Play Button Up \n");
				O2SOUNDINSTANCE->Play2DEffect(_T("ButtonUp"),O2SoundManager::S_TYPE_MENU,NULL);
			}
			break;
		default:
		    break;
		}
	}else{
		SetOnOff(MOUSE_UP);
		if(m_bUpButtonSound==TRUE)
		{
			m_bUpButtonSound=FALSE;
			printf("Play Button Off \n");
		}else
			m_bUpButtonSound=FALSE;
	}

	return 0;
}

void O2GUIButton::ChangeTexture()
{
	O2GUIObject* temp = m_pButtons[CLICK];
	m_pButtons[CLICK] = m_pButtons[NORMAL];
	m_pButtons[NORMAL] = temp;
}