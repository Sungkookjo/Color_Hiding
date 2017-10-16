//////////////////////////////////////////////////////////////////////////
// JApp
// 어플리케이션 클래스
#pragma once
#include "Manager.h"

class JApp{
private:
	BOOL			IsRun;		// 프로그램이 돌아감?
	BOOL			m_bRender;	// 그려야됨?

	static JApp*	pApp;		//	싱글톤 인스턴스
	HWND			hWnd;		//	hWnd
	HINSTANCE		m_hInst;	//	hInstance
	WNDCLASSEX wc;				//	Register the window class
	Manager* m_pMg;				//	MainManager
	O2Rect			m_rWinRect;	//	윈도우 Rect


	HRESULT Render();			// 그려줌
	HRESULT FrameMove();		// 내부 연산
	void Cleanup();				// 정리
	LRESULT SubProc( HWND phWnd, UINT& msg, WPARAM& wParam, LPARAM& lParam );	// MsgProc
	void CheckFocus();														// 현재창 활성화인지
public:
	JApp();
	~JApp();
	static LRESULT WINAPI MsgProc( HWND phWnd, UINT msg, WPARAM wParam, LPARAM lParam );
	HRESULT Init(UINT x,UINT y,UINT sx,UINT sy);
	int Run();
};