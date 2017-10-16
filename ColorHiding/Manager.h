//////////////////////////////////////////////////////////////////////////
// Manager.h
// 전체적으로 관리 해주는 클래스
#pragma once
#include "O2ObjectManager.h"
#include "O2InputManager.h"
#include "O2CameraManager.h"
#include "O2ParticleManager.h"
#include "O2GUIManager.h"
#include "O2SoundManager.h"
#include "Effect.h"
#include "O2Hero.h"
#include "Loading.h"
#include "O2VideoRenderer.h"

#define OBJSIZE 3
const UINT WM_GRAPH_EVENT = WM_APP + 1;

// 각 스테이트 마다 불러올 로드 파일
#define SAVE_MAIN	_T("SAVE/Main.txt")
#define SAVE_STAGE1	_T("SAVE/Stage1.txt")
#define SAVE_STAGE2	_T("SAVE/Stage2.txt")
#define SAVE_STAGE3	_T("SAVE/Stage3.txt")
#define SAVE_ENDING	_T("SAVE/Ending.txt")
#define SAVE_HELP	_T("SAVE/Help.txt")
#define SAVE_CREATE	_T("SAVE/Create.txt")
#define SAVE_GUI	_T("SAVE/PlayGUI.txt")
#define SAVE_LOST	_T("SAVE/Lost.txt")
#define SAVE_WIN	_T("SAVE/Win.txt")
#define SAVE_HIDING _T("SAVE/Hiding.txt")
#define SAVE_TIP	_T("SAVE/Tip.txt")
#define	EFFECTFILE	_T("chfx.fx")
#define INTRO_FILE _T("opening.avi")
#define INTRO_TIME 48.0f

#define UFOFILE		_T("")
#define UFOOBJNAME	_T("")

// 게임 메니저 스테이트
enum {
	GS_MAIN =0 ,
	GS_STAGE1,
	GS_STAGE2,
	GS_STAGE3,
	GS_STAGE_END,
	GS_LOST,
	GS_WIN,
	GS_CREATE,
	GS_HELP,
	GS_END
};
class Manager{
private:
	JDevice*			m_pDevCls	;	//	DirectX
	NxSDK*				m_pNxSDK	;	//	PhysX
	LPDIRECT3DDEVICE9*	m_lpDevice	;	//	Device

	O2Light*			m_pLight	;	//	Light 한개만 쓸거임
	Effect*				m_pEffect	;	//	TestEffect
	ResourceManager*	m_pResource	;	//	리소스 매니저
	O2ObjectManager*	m_pObjMgr	;	//	오브젝트 매니저
	O2CameraManager*	m_pCamMgr	;	//	카메라 매니저
	O2InputManager*		m_pInputMgr	;	//	인풋 매니저
	O2ParticleManager*  m_pPartMgr	;	//	파티클 매니저
	O2SoundManager*		m_pSoundMgr ;	//	사운드 매니저
	O2GUIManager*		m_pGuiMgr	;	//	Gui 매니저
	CTimer*			    m_pTimer	;	//  타이머
	Loading*			m_pLoading	;	//	로딩 클래스

	O2VideoRenderer*	m_pVideo	;	//	비디오 랜더러
	LONGLONG			m_ldVideoTime;	//	비디오 재생 시간
	BOOL				m_bShowVideo;	//	비디오 재생중?
	UINT				m_iNextState;	//	비디오 다 재생후 스테이지
	O2Rect				m_rVideoRect;	//	비디오 재생 화면상 사각형
	BOOL				m_bWillPlayVideo;	//	비디오를 재생할 것인지
	VOID				StopDrawVideo();

	O2Hero*				Hero		;	//	주인공 포인터

	UINT				m_iState	;	//	Game State
	BOOL				m_bPause	;	//	Pause 변수
	BOOL				m_bLoading	;	//	Loading 플래그
	BOOL				m_bFocus	;	//	현재 창이 활성화?

	HCURSOR				m_hCursor	;	//	Cursor
	HWND				m_hWnd		;	//	hWnd
	O2Rect				m_rWinRect	;	// Window Rect
	UINT				m_iStage	;	//	게임 현재 스테이지
	UINT				m_iTip		;	// 팁상태
	float				m_fStageTime;	//	스테이지 플레이타임
	int					m_iIsCreateMob;	//	몹 생성중?

	Basefw_FS*			m_pFS		;

	void			CreateMob();

	// 파일 로드
	HRESULT			ReadData(FILE* data);
	HRESULT			ReadCmd(TCHAR* Cmd , FILE** _file);
	// 비디오 재생
	// 재생할 동영상파일명 , 동영상 플레이 시간, 동영상 끝나고 다음 재생할 게임 State
	HRESULT			PlayVideo(TCHAR* pFile,float pPlayTime,UINT pNextState=GS_MAIN);

	VOID			GetTcsFromData(TCHAR* _para,TCHAR* _Tcs);
	VOID			GetColorFromData(TCHAR* _para,UINT& _Color);
	VOID			GetVec3FromData(TCHAR* _para,D3DXVECTOR3& pVec3);
	VOID			GetFloatFromData(TCHAR* _para,float& pVec3);
	VOID			GetShapeFromData(TCHAR* _para,UINT& _shape);
	VOID			GetIntFromData(TCHAR* _para,INT& _int);
	VOID			GetUintFromData(TCHAR* _para,UINT& _int);
	VOID			GetBOOLFromData(TCHAR* _para,BOOL& _int);
	VOID			GetRectFromData(TCHAR* _para,O2Rect& _rect);
	VOID			GetPtFromData(TCHAR* _para,POINT& _rect);
	VOID			GetCenterFromData(TCHAR* _para,UINT& _Center);

	// 오브젝트 생성
	HRESULT			AddPassiveObjFromFile( FILE** pFile , O23DPassiveObject* pObject=NULL,unsigned int _flag = 0);
	HRESULT			AddItemObjFromFile( FILE** pFile );
	HRESULT			AddActiveObjFromFile(FILE** _pFile , O23DActiveObject* pObject=NULL);
	HRESULT			CreateHeroFromFile(FILE** _pFile);
	HRESULT			Add2DObjFromFile(FILE** _pFile);
	HRESULT			AddAlienFromFile(FILE** _pFile);

	// Play Video
	HRESULT			PlayVideoFromFile(FILE** pFile);

	// GUI 생성
	HRESULT			CreateSpriteFromFile(FILE** _pFile,BOOL _puase = FALSE);
	HRESULT			CreateTimeSpriteFromFile(FILE** pFile,BOOL _puase = FALSE);
	HRESULT			CreateButtonFromFile(FILE** pFile,BOOL _puase = FALSE);
	HRESULT			CreateCheckFromFile(FILE** pFile,BOOL _puase = FALSE);
	HRESULT			CreateColorGageFromFile(FILE** pFile,BOOL _puase = FALSE);
	HRESULT			CreateTimeGageFromFile(FILE** pFile,BOOL _puase = FALSE);
	HRESULT			CreateHidingEffectFromFile(FILE** pFile);
	HRESULT			CreateWarnningFromFile(FILE** pFile);
	HRESULT			CreateTipFromFile(FILE** pFile);

	// FrameMove
	HRESULT			(Manager::*m_pfFm)();
	HRESULT			Fm_PlayVideo();
	HRESULT			Fm_Create();
	HRESULT			Fm_Ending();
	HRESULT			Fm_Main();
	VOID			In_Main();
	HRESULT			Fm_Play();
	VOID			In_Play();
	HRESULT			Fm_Help();
	VOID			In_Help();
	HRESULT			Fm_WinLost();
	VOID			In_Void() {}
private:
	HRESULT		PreRend();
	HRESULT		Rend();
	HRESULT		PostRend();
	void		Cleanup();
	HRESULT		ChangeState(UINT pState);
	HRESULT		AddState(UINT pState);
	float		m_fElapsed;				//	보정 시간
	VOID		SetPause(BOOL pRendGUI = TRUE);
public:
	Manager();
	~Manager();
	HRESULT		LoadFile(TCHAR* _file);
	HRESULT		Init(HINSTANCE hInst, HWND hWnd);
	HRESULT		FrameMove();
	HRESULT		Render();
	void		Input();
	void		Flush();

	HRESULT		DrawVideo();
	BOOL		IsFocus() { return m_bFocus;	}

	ResourceManager*	GetResMgr()		{ return m_pResource;	}	//	리소스 매니저
	O2ObjectManager*	GetObjMgr()		{ return m_pObjMgr;		}	//	오브젝트 매니저
	O2CameraManager*	GetCamMgr()		{ return m_pCamMgr;		}	//	카메라 매니저
	O2InputManager*		GetInputMgr()	{ return m_pInputMgr;	}	//	인풋 매니저
	O2ParticleManager*  GetPartMgr()	{ return m_pPartMgr;	}	//	파티클 매니저
	O2SoundManager*		GetSoundMgr()	{ return m_pSoundMgr;	}	//	사운드 매니저
	O2GUIManager*		GetGUIMgr()		{ return m_pGuiMgr;		}	//	Gui 매니저

	VOID		SetWinRect(O2Rect pRect) {
		m_rWinRect = pRect;
		m_rVideoRect.left = 0;
		m_rVideoRect.right = m_rWinRect.GetWidth();
		m_rVideoRect.top = 0;
		m_rVideoRect.bottom = m_rWinRect.GetHeight()-24;
		if(m_pInputMgr)
			m_pInputMgr->SetWinRect(pRect);
		if(m_pVideo)
			m_pVideo->UpdateVideoWindow(&m_rVideoRect);
	}
};