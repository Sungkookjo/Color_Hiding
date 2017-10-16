#include "stdafx.h"
#include "O2InputManager.h"

O2InputManager::O2InputManager()
:m_bMouseLeftDown(FALSE),m_bMouseRightDown(FALSE),
m_fMouseSensitivity(1.0f),m_uScreenWidth(1024),m_uScreenHeight(768)
{

}

O2InputManager::~O2InputManager()
{
	SAFE_DELETE(m_pInput);
}

HRESULT O2InputManager::Init(HINSTANCE hInst, HWND hWnd,UINT _Width, UINT _Height)
{
	m_pInput = new O2Input;
	m_hWnd = hWnd;
	m_uScreenWidth = _Width;
	m_uScreenHeight = _Height;

	if(FAILED(m_pInput->InitDirectInput(hInst,m_hWnd,true,true))){
		return E_FAIL;
	}
	return S_OK;
}

void O2InputManager::Update()
{
	memcpy(m_PreKeyState, m_KeyState, 256);
	m_pInput->Update(m_KeyState,&m_iMouseState);

	// 마우스 커서 좌표 받아옴
	GetCursorPos(&m_pCursorPos);
	//ScreenToClient(m_hWnd, &m_pCursorPos);
	m_pCursorPos.x -= m_rWinRect.left;
	m_pCursorPos.y -= (m_rWinRect.top+24);
	m_pCursorPos.y *= 1.05f;

	//O2Rect temp;
	//GetWindowRect(m_hWnd,&temp);
	//m_pCursorPos.x -= temp.left*2;
	//m_pCursorPos.y -= temp.top*2;

	// 마우스 클릭 체크
	if(m_bMouseLeftDown&&!m_iMouseState.rgbButtons[MOUSE_LBUTTON])
	{
		m_bMouseLeftDown=FALSE;
		m_bMouseDown[MOUSE_LBUTTON] = TRUE;
	}else
	{
		m_bMouseDown[MOUSE_LBUTTON] = FALSE;
	}
	if(m_bMouseRightDown&&!m_iMouseState.rgbButtons[MOUSE_RBUTTON])
	{
		m_bMouseRightDown=FALSE;
		m_bMouseDown[MOUSE_RBUTTON] = TRUE;
	}else
	{
		m_bMouseDown[MOUSE_RBUTTON] = FALSE;
	}

//	printf("X:%f\n Y:%f\n",m_fCursorX,m_fCursorY);
}

BOOL O2InputManager::IsMouseClick(UINT Button)
{
	return m_bMouseDown[Button];
}

BOOL O2InputManager::IsMouseDown(UINT Button)
{
	switch( Button )
	{
	case 0:
		return ((m_iMouseState.rgbButtons[Button]&0x80)?m_bMouseLeftDown=TRUE:FALSE);
	case 1:
		return ((m_iMouseState.rgbButtons[Button]&0x80)?m_bMouseRightDown=TRUE:FALSE);
	}

	return FALSE;
}

BOOL O2InputManager::IsKeyPress( UINT key )
{
	if ((m_KeyState[key] & 0x80) && !(m_PreKeyState[key] & 0x80))
	{
		return TRUE;
	}
	else
		return FALSE;

}

BOOL O2InputManager::MouseInClient()
{
	return m_rWinRect.IsInRect(m_pCursorPos);
}
