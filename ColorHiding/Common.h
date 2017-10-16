#pragma once

// ������ �̸�
#define WIN_NAME _T("ColorHiding")
#define WIN_CAPTION _T("ColorHiding")

// �Ҵ� ���� ��ũ��
#define SAFE_DELETE(x) if( NULL != x){ delete (x);} x = NULL;
#define SAFE_DELETE_ARRAY(x) if( NULL != x){ delete [] (x);} x = NULL;
#define SAFE_RELEASE(x) if( NULL != x){ (x)->Release();} x = NULL;

#ifndef V
#define V(x)           { hr = (x); if( FAILED(hr) ) { return hr; } }
#endif
#ifndef V_RETURN
#define V_RETURN(x)    { hr = (x); if( FAILED(hr) ) { return hr; } }
#endif

#ifdef UNICODE
#define _DEBUG_LOG wprintf
#else
#define _DEBUG_LOG printf
#endif

// ���ҽ� ���� ���ϸ�
#define RESOURCEFILE "Data.zip"