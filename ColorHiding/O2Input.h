#pragma once

#define MOUSE_LBUTTON 0
#define MOUSE_RBUTTON 1

#define MOUSE_UP	0
#define MOUSE_DOWN	1
#define MOUSE_CLICK	2

class O2Input
{
public:
	O2Input();
	~O2Input();

	HRESULT			InitDirectInput(HINSTANCE hInst, HWND hWnd, bool keyboard, bool mouse);
	bool			KeyProcess(BYTE);
	bool			KeyProcess(BYTE*);						//키입력
	bool			MouseProcess(DIMOUSESTATE*);			//마우스입력
	void			DeviceAcquire();						
	void			DeviceUnacquire();
	BOOL			Update(BYTE* _Key=NULL,DIMOUSESTATE* _Mouse=NULL);

private:
	LPDIRECTINPUT8			m_lpDxInput;			//	Dx 인풋
	LPDIRECTINPUTDEVICE8	m_lpKey;				//	키보드
	LPDIRECTINPUTDEVICE8	m_lpMouse;				//	마우스
	BYTE					m_arrKeyState[256];		//	Key 상태
	DIMOUSESTATE			m_iMouseState;			//	Mouse 상태

	HRESULT					hr;					// temp 임시
};