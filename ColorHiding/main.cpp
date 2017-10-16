#include "stdafx.h"
#include "JApp.h"
#include <conio.h>


//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{

//	_CrtSetBreakAlloc(205);
	JApp pApp;
 	AllocConsole();
 	freopen("CONOUT$","wt",stdout);

	if(FAILED(pApp.Init(100,100,1024,768))){
		return 0;
	}

	return pApp.Run();
}