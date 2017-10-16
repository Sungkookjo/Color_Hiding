//////////////////////////////////////////////////////////////////////////
// Manager.h
// ��ü������ ���� ���ִ� Ŭ����
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

// �� ������Ʈ ���� �ҷ��� �ε� ����
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

// ���� �޴��� ������Ʈ
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

	O2Light*			m_pLight	;	//	Light �Ѱ��� ������
	Effect*				m_pEffect	;	//	TestEffect
	ResourceManager*	m_pResource	;	//	���ҽ� �Ŵ���
	O2ObjectManager*	m_pObjMgr	;	//	������Ʈ �Ŵ���
	O2CameraManager*	m_pCamMgr	;	//	ī�޶� �Ŵ���
	O2InputManager*		m_pInputMgr	;	//	��ǲ �Ŵ���
	O2ParticleManager*  m_pPartMgr	;	//	��ƼŬ �Ŵ���
	O2SoundManager*		m_pSoundMgr ;	//	���� �Ŵ���
	O2GUIManager*		m_pGuiMgr	;	//	Gui �Ŵ���
	CTimer*			    m_pTimer	;	//  Ÿ�̸�
	Loading*			m_pLoading	;	//	�ε� Ŭ����

	O2VideoRenderer*	m_pVideo	;	//	���� ������
	LONGLONG			m_ldVideoTime;	//	���� ��� �ð�
	BOOL				m_bShowVideo;	//	���� �����?
	UINT				m_iNextState;	//	���� �� ����� ��������
	O2Rect				m_rVideoRect;	//	���� ��� ȭ��� �簢��
	BOOL				m_bWillPlayVideo;	//	������ ����� ������
	VOID				StopDrawVideo();

	O2Hero*				Hero		;	//	���ΰ� ������

	UINT				m_iState	;	//	Game State
	BOOL				m_bPause	;	//	Pause ����
	BOOL				m_bLoading	;	//	Loading �÷���
	BOOL				m_bFocus	;	//	���� â�� Ȱ��ȭ?

	HCURSOR				m_hCursor	;	//	Cursor
	HWND				m_hWnd		;	//	hWnd
	O2Rect				m_rWinRect	;	// Window Rect
	UINT				m_iStage	;	//	���� ���� ��������
	UINT				m_iTip		;	// ������
	float				m_fStageTime;	//	�������� �÷���Ÿ��
	int					m_iIsCreateMob;	//	�� ������?

	Basefw_FS*			m_pFS		;

	void			CreateMob();

	// ���� �ε�
	HRESULT			ReadData(FILE* data);
	HRESULT			ReadCmd(TCHAR* Cmd , FILE** _file);
	// ���� ���
	// ����� ���������ϸ� , ������ �÷��� �ð�, ������ ������ ���� ����� ���� State
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

	// ������Ʈ ����
	HRESULT			AddPassiveObjFromFile( FILE** pFile , O23DPassiveObject* pObject=NULL,unsigned int _flag = 0);
	HRESULT			AddItemObjFromFile( FILE** pFile );
	HRESULT			AddActiveObjFromFile(FILE** _pFile , O23DActiveObject* pObject=NULL);
	HRESULT			CreateHeroFromFile(FILE** _pFile);
	HRESULT			Add2DObjFromFile(FILE** _pFile);
	HRESULT			AddAlienFromFile(FILE** _pFile);

	// Play Video
	HRESULT			PlayVideoFromFile(FILE** pFile);

	// GUI ����
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
	float		m_fElapsed;				//	���� �ð�
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

	ResourceManager*	GetResMgr()		{ return m_pResource;	}	//	���ҽ� �Ŵ���
	O2ObjectManager*	GetObjMgr()		{ return m_pObjMgr;		}	//	������Ʈ �Ŵ���
	O2CameraManager*	GetCamMgr()		{ return m_pCamMgr;		}	//	ī�޶� �Ŵ���
	O2InputManager*		GetInputMgr()	{ return m_pInputMgr;	}	//	��ǲ �Ŵ���
	O2ParticleManager*  GetPartMgr()	{ return m_pPartMgr;	}	//	��ƼŬ �Ŵ���
	O2SoundManager*		GetSoundMgr()	{ return m_pSoundMgr;	}	//	���� �Ŵ���
	O2GUIManager*		GetGUIMgr()		{ return m_pGuiMgr;		}	//	Gui �Ŵ���

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