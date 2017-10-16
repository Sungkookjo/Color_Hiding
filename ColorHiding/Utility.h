#pragma once

struct O2CollisionInfo
{
	O2CollisionInfo();

	D3DXVECTOR3 _center;
	float       _radius;

	bool isPointInside(D3DXVECTOR3& p);

	D3DXVECTOR3 _min;
	D3DXVECTOR3 _max;
};


bool operator<(wstring& _Left, wstring& _Right);


#define DxVec3toNxVec3(Vec) ((NxVec3)Vec)

//#define NxVec3toDxVec3(Vec) ((D3DXVECTOR3)Vec)

//NxVec3 DxVec3toNxVec3(D3DXVECTOR3& _Right);
//NxVec3 DxVec3toNxVec3(const D3DXVECTOR3& _Right);
D3DXVECTOR3 NxVec3toDxVec3(NxVec3& _Right);
D3DXVECTOR3 NxVec3toDxVec3(const NxVec3& _Right);

bool ComputeBounding(LPD3DXMESH* mesh, O2CollisionInfo* sphere);

struct O2Rect : public RECT
{
public:
	O2Rect();
	O2Rect(LONG p_left,LONG p_right,LONG p_top,LONG p_bottom);
	~O2Rect();

	LONG GetHeight() { return bottom - top; }
	LONG GetWidth() { return right-left; }
	//RECT GetRect() {return (RECT)(*this); };

	BOOL IsInRect(int px, int py);
	BOOL IsInRect(POINT pt);
};

////////////////////////////////////////
#if !defined (__AutoDetectMemoryLeak_h__)
#define __AutoDetectMemoryLeak_h__

#if defined(_MSC_VER) && defined (_DEBUG)

#define _CRTDBG_MAP_ALLOC // �޸� ������ Ž���ϱ� ���� ���� ���־�� �Ѵ�.
#include <crtdbg.h>
#if !defined (_CONSOLE)
#include <cstdlib> // for Consol Application
#endif

class __AutoDetectMemoryLeak
{
public:
	__AutoDetectMemoryLeak ()
	{
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | 
			_CRTDBG_LEAK_CHECK_DF);

		// Consol Application�� ���
#if defined (_CONSOLE)
		// Send all reports to STDOUT
		_CrtSetReportMode( _CRT_WARN,   
			_CRTDBG_MODE_FILE   );
		_CrtSetReportFile( _CRT_WARN,   
			_CRTDBG_FILE_STDOUT );
		_CrtSetReportMode( _CRT_ERROR,  
			_CRTDBG_MODE_FILE   );
		_CrtSetReportFile( _CRT_ERROR,  
			_CRTDBG_FILE_STDOUT );
		_CrtSetReportMode( _CRT_ASSERT, 
			_CRTDBG_MODE_FILE   );
		_CrtSetReportFile( _CRT_ASSERT, 
			_CRTDBG_FILE_STDOUT );


#define DEBUG_NORMALBLOCK   new ( _NORMAL_BLOCK, __FILE__, __LINE__ )
#ifdef new
#undef new
#endif
#define new DEBUG_NORMALBLOCK

#else

		// Send all reports to DEBUG window
		_CrtSetReportMode( _CRT_WARN,   
			_CRTDBG_MODE_DEBUG  );
		_CrtSetReportMode( _CRT_ERROR,  
			_CRTDBG_MODE_DEBUG  );
		_CrtSetReportMode( _CRT_ASSERT, 
			_CRTDBG_MODE_DEBUG  );

#endif

#ifdef malloc
#undef malloc
#endif
		/*
		* malloc���� �Ҵ�� �޸𸮿� ������ ���� ��� ��ġ
		�� ����
		* CONSOLE ����� ��� crtdbg.h�� malloc�� ���ǵǾ� 
		������,
		* _AXFDLL ����� ��쿡�� �ణ �ٸ�������� ���� ��
		�Եȴ�.
		* date: 2001-01-30
		*/
#define malloc(s) (_malloc_dbg( s, _NORMAL_BLOCK, __FILE__, __LINE__ ))
	}
};

// �� ���� �ʱ�ȭ�� �����ڸ� ���� �ڵ����� ���ֱ� ���� �������� �����Ѵ�.
static __AutoDetectMemoryLeak __autoDetectMemoryLeak;

#endif // if defined(_MSC_VER) && defined (_DEBUG)

#endif // __AutoDetectMemoryLeak_h__