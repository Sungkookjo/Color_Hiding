//////////////////////////////////////////////////////////////////////////
// JApp
// ���ø����̼� Ŭ����
#pragma once
#include "Manager.h"

class JApp{
private:
	BOOL			IsRun;		// ���α׷��� ���ư�?
	BOOL			m_bRender;	// �׷��ߵ�?

	static JApp*	pApp;		//	�̱��� �ν��Ͻ�
	HWND			hWnd;		//	hWnd
	HINSTANCE		m_hInst;	//	hInstance
	WNDCLASSEX wc;				//	Register the window class
	Manager* m_pMg;				//	MainManager
	O2Rect			m_rWinRect;	//	������ Rect


	HRESULT Render();			// �׷���
	HRESULT FrameMove();		// ���� ����
	void Cleanup();				// ����
	LRESULT SubProc( HWND phWnd, UINT& msg, WPARAM& wParam, LPARAM& lParam );	// MsgProc
	void CheckFocus();														// ����â Ȱ��ȭ����
public:
	JApp();
	~JApp();
	static LRESULT WINAPI MsgProc( HWND phWnd, UINT msg, WPARAM wParam, LPARAM lParam );
	HRESULT Init(UINT x,UINT y,UINT sx,UINT sy);
	int Run();
};