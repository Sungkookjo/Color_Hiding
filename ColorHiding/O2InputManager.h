#pragma once

class O2InputManager
{
private:
	O2Rect				m_rWinRect;		//	WindowRect

public:
	O2InputManager();
	~O2InputManager();

	HRESULT				Init(HINSTANCE hInst, HWND hWnd,UINT _Width=1024, UINT _Height=768);
	void				Update();
	inline BOOL			IsKeyDown(UINT Key) { return ((m_KeyState[Key]&0x80)?TRUE:FALSE);}
	BOOL				IsKeyPress(UINT key);
	inline BOOL			IsKeyUp(UINT Key) { return ((m_KeyState[Key]&0x80)?FALSE:TRUE);}

	const BYTE*			GetKeyState() const { return m_KeyState; }
	const DIMOUSESTATE&	GetMouseState() const { return m_iMouseState; }

	POINT				GetMousePos() { return m_pCursorPos; }
	float				GetRelativeX() { return m_iMouseState.lX * m_fMouseSensitivity; }
	float				GetRelativeY() { return m_iMouseState.lY * m_fMouseSensitivity; }
	float				GetRelativeZ() { return m_iMouseState.lZ * m_fMouseSensitivity; }

	void				SetMouseSensitivity(float _sensitivity) { m_fMouseSensitivity = _sensitivity; }


	
	BOOL				IsMouseClick(UINT Button);
	BOOL				IsMouseDown(UINT Button);
	inline BOOL			IsMouseUp(UINT Button) { return ((m_iMouseState.rgbButtons[Button]&0x80)?FALSE:TRUE);}

	HWND				GetHwnd() { return m_hWnd; }

	VOID				SetWinRect(O2Rect pRect) { m_rWinRect = pRect; }
	BOOL				MouseInClient();

private:
	BYTE				m_KeyState[256];
	BYTE				m_PreKeyState[256];
	BOOL				m_bMouseDown[2];
	DIMOUSESTATE		m_iMouseState;

	O2Input*			m_pInput;

	HWND				m_hWnd;
	POINT				m_pWinPos;

	bool				m_bMouseLeftDown;
	bool				m_bMouseRightDown;
	POINT				m_pCursorPos;			//	커서 위치
	float				m_fMouseSensitivity;	//마우스 감도
	UINT				m_uScreenWidth;			//스크린 넓이
	UINT				m_uScreenHeight;		//스크린 높이

};