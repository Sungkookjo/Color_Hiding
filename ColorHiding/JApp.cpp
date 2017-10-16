//////////////////////////////////////////////////////////////////////////
// JApp
// Application class
#include "stdafx.h"
#include "JApp.h"
#include "resource.h"

JApp* JApp::pApp = NULL;

//-----------------------------------------------------------------------------
// Name: JApp()
// Desc: Constructor
//-----------------------------------------------------------------------------
JApp::JApp()
:m_pMg(NULL)
,IsRun(TRUE)
,m_bRender(TRUE)
{
	srand((unsigned)time(NULL));
	if(NULL == pApp)
		pApp = this;
}
//-----------------------------------------------------------------------------
// Name: ~JApp()
// Desc: Destructor
//-----------------------------------------------------------------------------
JApp::~JApp()
{
	CoUninitialize();
	Cleanup();
}


//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI JApp::MsgProc( HWND phWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	if(JApp::pApp)
		JApp::pApp->SubProc(phWnd,msg,wParam,lParam);

	switch( msg )
	{
	case WM_DESTROY:
		PostQuitMessage( 0 );
		return 0;
	}

	return DefWindowProc( phWnd, msg, wParam, lParam );
}
//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize
//-----------------------------------------------------------------------------
HRESULT JApp::Init(UINT x,UINT y,UINT sx,UINT sy)
{
	m_pMg = new Manager;
	if(NULL == m_pMg){
		return E_OUTOFMEMORY;
	}

	HRESULT hr;
	// Register the window class
	WNDCLASSEX temp = {
		 sizeof( WNDCLASSEX ), CS_CLASSDC, &JApp::MsgProc, 0L, 0L,
		GetModuleHandle( NULL ), NULL, NULL, NULL, NULL,
		WIN_NAME, NULL
	};
	//temp.hIcon = LoadIcon(NULL,MAKEINTRESOURCE(IDI_ICON1));
	wc = temp;

	ATOM atom = RegisterClassEx( &wc );

	if( 0 == atom ){
		return __HRESULT_FROM_WIN32(GetLastError());
	}

	m_rWinRect.left = x;
	m_rWinRect.right = x + sx;
	m_rWinRect.top = y;
	m_rWinRect.bottom = y+sy;

	hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		_DEBUG_LOG(_T("Failed - CoInitalize() \n"));
		return E_FAIL;
	}

	// Create the application's window
	hWnd = CreateWindow( WIN_NAME, WIN_CAPTION,
		WS_OVERLAPPEDWINDOW
		/*WS_POPUP*/, m_rWinRect.left , m_rWinRect.top, m_rWinRect.GetWidth(), m_rWinRect.GetHeight(),
		NULL, NULL, wc.hInstance, NULL );
	
	if ( 0 == hWnd )
	{
		return  __HRESULT_FROM_WIN32(GetLastError());
	}

	m_hInst = wc.hInstance;

	// Show the window
	ShowWindow( hWnd, SW_SHOWDEFAULT );
	UpdateWindow( hWnd );

	if( SUCCEEDED( m_pMg->Init( m_hInst, hWnd ) ) )
	{
		// Initialize Direct3D
		m_pMg->SetWinRect(m_rWinRect);
	}else{
		return E_FAIL;
	}

	return S_OK;
}
//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Process
//-----------------------------------------------------------------------------
HRESULT JApp::FrameMove()
{
	if(FALSE == IsRun)
		return E_FAIL;

	if(FAILED(m_pMg->FrameMove()))
		return E_FAIL;
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
HRESULT JApp::Render()
{
	if(FALSE == m_pMg->IsFocus()){
		Sleep(50);
		return S_OK;
	}
	if(FAILED(m_pMg->Render()))
		return E_FAIL;
	return S_OK;
}

void JApp::Cleanup()
{
	SAFE_DELETE(m_pMg);
	UnregisterClass( WIN_NAME, wc.hInstance );
}

int JApp::Run()
{
	// Enter the message loop
	MSG msg;
	ZeroMemory( &msg, sizeof( msg ) );
	while( msg.message != WM_QUIT )
	{
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else{
			if(FAILED(FrameMove()))
				break;

			if(FAILED(Render()))
				break;
		}
	}
	Sleep(100000);
	return 0;
}
//////////////////////////////////////////////////////////////////////////
// WinProc 에서 돌릴 JApp의 프록시저 함수
LRESULT JApp::SubProc( HWND phWnd, UINT& msg, WPARAM& wParam, LPARAM& lParam )
{
	switch( msg )
	{
	case WM_KEYDOWN:
		//if( VK_ESCAPE == wParam )
		//	IsRun = FALSE;
		//return 0;
		break;
	case WM_MOVE:
		GetWindowRect(phWnd,&m_rWinRect);
		if(m_pMg)
			m_pMg->SetWinRect(m_rWinRect);
		break;
	case WM_GETMINMAXINFO:
		{
			// 사이즈 차단!!!

			// 윈도우에서는 *대신 P를 앞에 붙이는 경우가 많다.
			PMINMAXINFO p            = (PMINMAXINFO)lParam;

			// 최대로 커질수 있는 값을 지정한다
			p->ptMaxTrackSize.x = m_rWinRect.GetWidth();
			p->ptMaxTrackSize.y = m_rWinRect.GetHeight();

			p->ptMinTrackSize.x = m_rWinRect.GetWidth();
			p->ptMinTrackSize.y = m_rWinRect.GetHeight();
		}
		break;
	case WM_SIZE:	
		if( SIZE_MINIMIZED == wParam )
		{
			m_bRender = FALSE;
		}
		else
		{
			if( SIZE_MAXIMIZED == wParam )
			{
				m_bRender = TRUE;
			}
			else if( SIZE_RESTORED == wParam )
			{
				m_bRender = TRUE;
			}
		}
		break;
	}

	return 0;
}