#include "stdafx.h"

O2Input::O2Input()
:m_lpDxInput(NULL),m_lpKey(NULL),m_lpMouse(NULL)
{
}

O2Input::~O2Input()
{
	if(m_lpDxInput)
	{
		if(m_lpKey)
		{
			m_lpKey->Unacquire();
			m_lpKey->Release();
			m_lpKey = NULL;
		}
		if(m_lpMouse)
		{
			m_lpMouse->Unacquire();
			m_lpMouse->Release();
			m_lpMouse = NULL;
		}

		m_lpDxInput->Release();
		m_lpDxInput = NULL;
	}
}

HRESULT O2Input::InitDirectInput(HINSTANCE hInst, HWND hWnd, bool keyboard, bool mouse)
{
	hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_lpDxInput, NULL);
	if(FAILED(hr))
	{
		return hr;
	}


	if(keyboard)
	{
		if(FAILED(m_lpDxInput->CreateDevice(GUID_SysKeyboard, &m_lpKey, NULL)))
			return E_FAIL;
		if(FAILED(m_lpKey->SetDataFormat(&c_dfDIKeyboard)))
			return E_FAIL;
		if(FAILED(m_lpKey->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND | DISCL_NOWINKEY)))
			return E_FAIL;
		while(m_lpKey->Acquire() == DIERR_INPUTLOST);
	}

	if(mouse)
	{
		if(FAILED(m_lpDxInput->CreateDevice(GUID_SysMouse, &m_lpMouse, NULL)))
			return E_FAIL;
		if(FAILED(m_lpMouse->SetDataFormat(&c_dfDIMouse)))
			return E_FAIL;
		if(FAILED(m_lpMouse->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND)))
			return E_FAIL;
		while(m_lpMouse->Acquire() == DIERR_INPUTLOST);
	}


	return S_OK;
}



bool O2Input::KeyProcess(BYTE key)
{
	ZeroMemory(m_arrKeyState, sizeof(m_arrKeyState));

	hr = m_lpKey->GetDeviceState(sizeof(m_arrKeyState), m_arrKeyState);

	if(FAILED(hr))
	{
		hr = m_lpKey->Acquire();
		while(hr == DIERR_INPUTLOST)
			m_lpKey->Acquire();

		return false;
	}

	if(m_arrKeyState[key] & 0x80)
	{
		return true;
	}

	return false;
}

bool O2Input::KeyProcess(BYTE* key)
{
	ZeroMemory(key, 256);

	hr = m_lpKey->GetDeviceState(256, key);

	if(FAILED(hr))
	{
		hr = m_lpKey->Acquire();
		while(hr == DIERR_INPUTLOST)
			m_lpKey->Acquire();

		return false;
	}

	return true;
}

bool O2Input::MouseProcess(DIMOUSESTATE* MouseState)
{
	ZeroMemory(MouseState, sizeof(DIMOUSESTATE));

	hr = m_lpMouse->GetDeviceState(sizeof(DIMOUSESTATE), MouseState);

	if(FAILED(hr))
	{
		hr = m_lpMouse->Acquire();
		while(hr == DIERR_INPUTLOST)
			m_lpMouse->Acquire();

		return false;
	}

	return true;
}

void O2Input::DeviceAcquire()
{
	if(m_lpKey)
		m_lpKey->Acquire();
	if(m_lpMouse)
		m_lpMouse->Acquire();
}

void O2Input::DeviceUnacquire()
{
	if(m_lpKey)
		m_lpKey->Unacquire();
	if(m_lpMouse)
		m_lpMouse->Acquire();
}

BOOL O2Input::Update(BYTE* _Key,DIMOUSESTATE* _Mouse )
{
	KeyProcess((_Key?_Key:m_arrKeyState));
	MouseProcess((_Mouse?_Mouse:&m_iMouseState));
	return TRUE;
}