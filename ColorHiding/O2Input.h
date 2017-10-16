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
	bool			KeyProcess(BYTE*);						//Ű�Է�
	bool			MouseProcess(DIMOUSESTATE*);			//���콺�Է�
	void			DeviceAcquire();						
	void			DeviceUnacquire();
	BOOL			Update(BYTE* _Key=NULL,DIMOUSESTATE* _Mouse=NULL);

private:
	LPDIRECTINPUT8			m_lpDxInput;			//	Dx ��ǲ
	LPDIRECTINPUTDEVICE8	m_lpKey;				//	Ű����
	LPDIRECTINPUTDEVICE8	m_lpMouse;				//	���콺
	BYTE					m_arrKeyState[256];		//	Key ����
	DIMOUSESTATE			m_iMouseState;			//	Mouse ����

	HRESULT					hr;					// temp �ӽ�
};