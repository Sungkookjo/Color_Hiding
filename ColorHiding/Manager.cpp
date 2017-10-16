#include "stdafx.h"
#include "Manager.h"
#include "ToonEffect.h"
#include "Arrow.h"
#include "O2Capsule.h"

//////////////////////////////////////////////////////////////////////////
// Constructor
Manager::Manager()
:m_pDevCls(NULL)
,m_lpDevice(NULL)
,m_pResource(NULL)
,m_pNxSDK(NULL)
,m_pObjMgr(NULL)
,m_pInputMgr(NULL)
,m_pCamMgr(NULL)
,m_pPartMgr(NULL)
,m_pSoundMgr(NULL)
,m_pEffect(NULL)
,m_pGuiMgr(NULL)
,m_iState(GS_MAIN)
,m_pfFm(NULL)
,m_pLoading(NULL)
,m_bPause(FALSE)
,m_pTimer(NULL)
,m_bLoading(FALSE)
,m_pLight(NULL)
,m_pVideo(NULL)
,m_bShowVideo(FALSE)
,m_iNextState(GS_MAIN)
,m_bWillPlayVideo(TRUE)
,m_iTip(0)
,m_pFS(NULL)
{
	srand((unsigned)time(NULL));
}

//////////////////////////////////////////////////////////////////////////
// Destructor
Manager::~Manager()
{
	Cleanup();
	SetCursor(LoadCursor(NULL,IDC_ARROW));
}

//////////////////////////////////////////////////////////////////////////
// Cleaner
void Manager::Cleanup()
{
	// 로딩중에 뻑갈수 있으므로
	m_bLoading = FALSE;
	Sleep(50);
	
	SAFE_DELETE(m_pVideo);

	SAFE_DELETE(m_pObjMgr);
	SAFE_DELETE(m_pGuiMgr);
	SAFE_DELETE(m_pResource);
	SAFE_DELETE(m_pInputMgr);
	SAFE_DELETE(m_pCamMgr);
	SAFE_DELETE(m_pPartMgr);
	SAFE_DELETE(m_pSoundMgr);
	SAFE_DELETE(m_pLoading);

	SAFE_DELETE(m_pEffect);

	SAFE_DELETE(m_pNxSDK);
	SAFE_DELETE(m_pDevCls);
	SAFE_DELETE(m_pTimer);
	SAFE_DELETE(m_pLight);
	SAFE_DELETE(m_pFS);
}

//////////////////////////////////////////////////////////////////////////
// Initialize
HRESULT Manager::Init(HINSTANCE hInst, HWND hWnd )
{	
	srand((unsigned)time(NULL));

	m_hCursor = GetCursor();
	m_hWnd = hWnd;
	m_bLoading = TRUE;
	// Create DirectX
	m_pDevCls = new JDevice;	
	if(FAILED(m_pDevCls->InitD3D(hWnd))){
		_DEBUG_LOG(_T("Failed-InitD3D\n"));
		//MessageBox(NULL,_T("Failed-Init()"),_T("DirectX"),MB_OK);
		return E_FAIL;
	}
	m_lpDevice = m_pDevCls->GetDevice();
	
	// 압축파일 링크
	m_pFS = new Basefw_FS;
	if(FS_OK != m_pFS->Link(RESOURCEFILE)){
		return E_FAIL;
	}

	// 로딩 클래스
	m_pLoading = new Loading(m_pFS);
	if(FAILED(m_pLoading->Init(
		hWnd,
		m_pDevCls->GetD3D9()
		))){
			_DEBUG_LOG(_T("Failed-Init Loading\n"));
			return E_FAIL;
	}
	m_pLoading->Run(&m_bLoading);

	// 비디오 생성
	m_pVideo = new O2VideoRenderer(hWnd);

	// Light 
	m_pLight = new O2Light;

	// Create Timer
	m_pTimer = new CTimer;
	m_pTimer->SetTimer();

	// Create PhysX
	m_pNxSDK = new NxSDK;
	if(FAILED(m_pNxSDK->Init())){
		_DEBUG_LOG(_T("Failed-InitNxPhysX\n"));
		return E_FAIL;
	}

	// 인풋 매니저
	m_pInputMgr = new O2InputManager;
	if(FAILED(m_pInputMgr->Init(hInst,hWnd))){
		_DEBUG_LOG(_T("Failed-InitInput\n"));
		return E_FAIL;
	}
	m_pInputMgr->SetWinRect(m_rWinRect);

	// 카메라 매니저
	m_pCamMgr = new O2CameraManager;
	if(FAILED(m_pCamMgr->Init(m_lpDevice,m_pInputMgr,&m_fElapsed))){
		_DEBUG_LOG(_T("Failed-InitCamera\n"));
		return E_FAIL;
	}

	// 리소스매니저
	m_pResource = new ResourceManager(m_pFS);
	if(FAILED(m_pResource->Init(m_lpDevice))){
		_DEBUG_LOG(_T("Failed-InitResourceManager\n"));
		return E_FAIL;
	}

	// 파티클 매니저
	m_pPartMgr = new O2ParticleManager;
	if(FAILED(m_pPartMgr->Init(m_lpDevice,m_pInputMgr,m_pResource))){
		_DEBUG_LOG(_T("Failed-InitParticle\n"));
		return E_FAIL;
	}

	// 사운드 매니저
	m_pSoundMgr = O2SoundManager::GetInstance();
	if(FAILED(m_pSoundMgr->Init(hWnd)))
	{
		_DEBUG_LOG(_T("Failed-InitSound\n"));
		return E_FAIL;
	}

	// GUI 매니저
	m_pGuiMgr = new O2GUIManager;
	if(FAILED(m_pGuiMgr->Init(m_pInputMgr,m_lpDevice,m_pResource,m_pSoundMgr))){
		_DEBUG_LOG(_T("Failed-InitGUIMgr\n"));
		return E_FAIL;
	}
	m_pGuiMgr->SetDelta(&m_fElapsed);

	// 오브젝트 매니저
	m_pObjMgr = new O2ObjectManager;
	if(FAILED(m_pObjMgr->Init(m_lpDevice,m_pResource,m_pPartMgr))){
		_DEBUG_LOG(_T("Failed-InitObjMgr\n"));
		return E_FAIL;
	}
	m_pObjMgr->SetLight(m_pLight);

	// 이펙트 생성
	m_pEffect = new ToonEffect(m_lpDevice,m_pCamMgr,m_pFS);
	if(FAILED(m_pEffect->Init(EFFECTFILE))){
		_DEBUG_LOG(_T("Failed-InitEffect\n"));
		return E_FAIL;
	}

	m_pObjMgr->SetEffect(m_pEffect);


	m_pObjMgr->SetElaped(&m_fElapsed);
	m_bLoading = FALSE;
	Sleep(50);
	ChangeState(m_iState);
	O2Object::s_pManager = this;
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
// Frame Move
HRESULT Manager::FrameMove()
{
	float FPS;
	TCHAR Msg[30];
	m_fElapsed = m_pTimer->GetElapsedTime();
	FPS = 1.0f/m_fElapsed;

	wsprintf(Msg,_T("FPS = %d"),(int)FPS);

	SetWindowText(m_pInputMgr->GetHwnd(),_T("ColorHiding"));
	// 인풋 정보 갱신
	m_pInputMgr->Update();


	// 임시
	m_pPartMgr->Input();
	Input();

	m_bFocus = (GetFocus() == m_hWnd ? TRUE:FALSE);

	// 현재 상태 FrameMove 진행
	return (this->*m_pfFm)();
}
//////////////////////////////////////////////////////////////////////////
// Render : 그려줌
HRESULT Manager::Render()
{
	if(m_bShowVideo){
		return S_OK;
	}

	if(FAILED(PreRend()))
		return E_FAIL;

	Rend();

	if(FAILED(PostRend()))
		return E_FAIL;

	return S_OK;
}

HRESULT Manager::PreRend()
{
	// Clear the backbuffer to a blue color
	(*m_lpDevice)->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );

	// Begin the scene
	if( FAILED( (*m_lpDevice)->BeginScene() ) )
		return E_FAIL;

	return S_OK;
}

HRESULT Manager::Rend()
{
	m_pObjMgr->Render();
	m_pPartMgr->Render();
	m_pGuiMgr->Render();
	return S_OK;
}

HRESULT Manager::PostRend()
{

	if(FAILED((*m_lpDevice)->EndScene()))
		return E_FAIL;

	// Present the backbuffer contents to the display
	(*m_lpDevice)->Present( NULL, NULL, NULL, NULL );

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// 키인풋. 임시
void Manager::Input()
{
	if(m_pInputMgr->IsKeyDown(DIK_R)){
		ChangeState(m_iState);
	}

	if(m_pInputMgr->IsKeyDown(DIK_M)){
		ChangeState(GS_MAIN);
	}
}

//////////////////////////////////////////////////////////////////////////
// LoadFile
// File 로드함
HRESULT Manager::LoadFile( TCHAR* _filename )
{	
	// 리소스 파일 메모리 주소를 읽어옴.
	char FileName[MAX_PATH];
	UINT _len = _tcslen(_filename);
#ifdef UNICODE
	wcstombs(FileName,_filename,_len);
	FileName[_len] = NULL;
#else
	strcpy_s(FileName,_filename);
#endif
	//unsigned char* temp; = m_pFS->Read(FileName);
	HRESULT hr = S_OK;
	FILE* file=NULL;

	_DEBUG_LOG(_T("  ===           Load [ %s ]                === \n"),_filename);
	//remove(FileName);
	//_tfopen_s(&file,_filename,_T("wt"));
	//if(temp)
//		fwrite(temp,m_pFS->ReadSize(FileName),1,file);
	//fclose(file);
	_tfopen_s(&file,_filename,_T("rt"));

	if( NULL == file){
		_DEBUG_LOG(_T("Failed-Load. [ %s ]\n"),_filename);
		return E_FAIL;
	}

	hr = ReadData(file);

	fclose(file);
	//remove(FileName);

	return hr;
}

//////////////////////////////////////////////////////////////////////////
// ReadData
// 파일을 읽음
HRESULT Manager::ReadData( FILE* file)
{
	TCHAR cmd[256];
	UINT	len = 0;
	HRESULT hr;

	while(_fgetts(cmd,256,file)){
		len = _tcslen(cmd);
		cmd[len-1] = NULL;
		hr = ReadCmd(cmd,&file);
		if(FAILED(hr)){
			return hr;
		}
	}

	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
// ReadCmd
// 파일의 한줄한줄 명령 읽음
HRESULT Manager::ReadCmd( TCHAR* Cmd , FILE** _file )
{
	HRESULT hr = S_OK;

	//	오브젝트 추가
	if(_tcscmp(Cmd,_T("[AddObject]")) == 0){
		hr = AddPassiveObjFromFile(_file);
	}
	// 액티브 오브젝트 추가
	else if(_tcscmp(Cmd,_T("[AddActObject]")) == 0){
		hr = AddActiveObjFromFile(_file);
	}
	// 2D오브젝트 추가
	else if(_tcscmp(Cmd,_T("[Add2DObject]")) == 0){
		hr = Add2DObjFromFile(_file);
	}
	//	UFO
	else if(_tcscmp(Cmd,_T("[AddUFO]")) == 0){
		hr = AddPassiveObjFromFile(_file,new O2UFO(Hero),NX_BF_FROZEN_POS_Y|NX_BF_FROZEN_ROT);
	}
	//	캡슐
	else if(_tcscmp(Cmd,_T("[AddCapsule]")) == 0){
		hr = AddPassiveObjFromFile(_file,new O2Capsule);
	}
	// Alien
	else if(_tcscmp(Cmd,_T("[AddAlien]")) == 0){
		hr = AddAlienFromFile(_file);
	}
	// Arrow
	else if(_tcscmp(Cmd,_T("[AddArrow]"))==0){
		hr = AddPassiveObjFromFile(_file,new Arrow);
	}
	// 히어로 추가
	else if(_tcscmp(Cmd,_T("[CreateHero]")) == 0){
		hr = CreateHeroFromFile(_file);
	}
	// 아이템 추가
	else if(_tcscmp(Cmd,_T("[CreateItem]")) == 0){
		hr = AddItemObjFromFile(_file);
	}
	// 화면 지우기
	else if(_tcscmp(Cmd,_T("Clear()")) == 0){
		Flush();
	}
	// Sprite
	else if(_tcscmp(Cmd,_T("[CreateSprite]")) == 0){
		hr = CreateSpriteFromFile(_file);
	}
	// Time Sprite
	else if(_tcscmp(Cmd,_T("[CreateTimeSprite]")) == 0){
		hr = CreateTimeSpriteFromFile(_file);
	}
	// Button
	else if(_tcscmp(Cmd,_T("[CreateButton]")) == 0){
		hr = CreateButtonFromFile(_file);
	}
	// 체크박스
	else if(_tcscmp(Cmd,_T("[CreateCheck]")) == 0){
		hr = CreateCheckFromFile(_file);
	}
	// 칼라게이지
	else if(_tcscmp(Cmd,_T("[CreateColorGage]")) == 0){
		hr = CreateColorGageFromFile(_file);
	}
	// 타이머 게이지
	else if(_tcscmp(Cmd,_T("[CreateTimeGage]")) == 0){
		hr = CreateTimeGageFromFile(_file);
	}
	// Pause Button
	else if(_tcscmp(Cmd,_T("[CreatePauseButton]")) == 0){
		hr = CreateButtonFromFile(_file,TRUE);
	}
	// PauseSprite
	else if(_tcscmp(Cmd,_T("[CreatePauseSprite]")) == 0){
		hr = CreateSpriteFromFile(_file,TRUE);
	}
	// HidingEffect
	else if(_tcscmp(Cmd,_T("[CreateHidingEffect]")) == 0){
		hr = CreateHidingEffectFromFile(_file);
	}
	// Warnning
	else if(_tcscmp(Cmd,_T("[CreateWarnning]")) == 0){
		hr = CreateWarnningFromFile(_file);
	}
	// Play Video
	else if(_tcscmp(Cmd,_T("[PlayVideo]")) == 0){
		hr = PlayVideoFromFile(_file);
	}
	// TutoTip
	else if(_tcscmp(Cmd,_T("[CreateTip]")) == 0){
		hr = CreateTipFromFile(_file);
	}
	else{
		_DEBUG_LOG(_T("Error - ReadCmd value : %s \n"),Cmd);
		return hr;
	}

	_DEBUG_LOG(_T("Finished Load %s \n"),Cmd);
	return hr;
}
//////////////////////////////////////////////////////////////////////////
// CreateHeroFromFile
// 파일에서 주인공 생성 정보에 맞게 주인공 생성
HRESULT Manager::CreateHeroFromFile( FILE** pFile )
{
	TCHAR Data[256];
	TCHAR* _val;
	TCHAR* _flag;
	TCHAR _filename[MAX_OBJNAME] = _T("NONE");
	D3DXVECTOR3 _pos(0.0f,0.0f,0.0f);
	D3DXVECTOR3 _rot(0.0f,0.0f,0.0f);
	D3DXVECTOR3 _size(1.0f,1.0f,1.0f);
	BOOL _isUseCellEffect = TRUE;
	BOOL _isRendLine = TRUE;
	BOOL _isRendShadow = TRUE;
	BOOL _isReflectShadow = TRUE;
	UINT len;

	while(_fgetts(Data,256,(*pFile))){
		len = _tcslen(Data);
		Data[len-1] = NULL;

		if( 0 == _tcscmp(Data,_T("[END]"))){
			break;
		}
		_flag = _tcstok_s(Data,_T("="),&_val);
		if(_val != NULL){
			_val = _tcstok_s(_val,_T(";"),&_val); 
			if(NULL == _val){
				continue;
			}
			if( 0 == _tcscmp(_flag,_T("FileName"))){
				GetTcsFromData(_val,_filename);
			}else if(0 == _tcscmp(_flag,_T("Pos"))){
				GetVec3FromData(_val,_pos);
			}else if(0 == _tcscmp(_flag,_T("Rot"))){
				GetVec3FromData(_val,_rot);
			}else if(0 == _tcscmp(_flag,_T("Size"))){
				GetVec3FromData(_val,_size);
			}else if(0 == _tcscmp(_flag,_T("RendShadow"))){
				GetBOOLFromData(_val,_isRendShadow);
			}else if(0 == _tcscmp(_flag,_T("ReflectShadow"))){
				GetBOOLFromData(_val,_isReflectShadow);
			}else if(0 == _tcscmp(_flag,_T("UseLightEffect"))){
				GetBOOLFromData(_val,_isUseCellEffect);
			}else if(0 == _tcscmp(_flag,_T("RendLine"))){
				GetBOOLFromData(_val,_isRendLine);
			}else{
				_DEBUG_LOG(_T("Error - Readflag value [ %s ]\n"),_flag);
			}
		}
	}

	// 주인공 등록 후 포인터 받아옴
	if(FAILED(m_pObjMgr->CreateHero(_filename,_pos,_rot,_size,_isRendShadow,_isReflectShadow,_isUseCellEffect,_isRendLine))){
		return E_FAIL;
	}
	Hero = m_pObjMgr->GetHero();
	Hero->SetSoundManager(m_pSoundMgr);

	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
// AddObjectFromFile
// 파일에서 오브젝트 추가 함수에 맞춰서 새로운 오브젝트 추가
HRESULT Manager::AddPassiveObjFromFile( FILE** pFile , O23DPassiveObject* pObj , unsigned int _bodyflag )
{
	TCHAR Data[256];
	TCHAR* _val;
	TCHAR* _flag;
	TCHAR _filename[MAX_OBJNAME] = _T("NONE");
	TCHAR _objname[MAX_OBJNAME] = _T("NONE");
	D3DXVECTOR3 _pos(0.0f,0.0f,0.0f);
	D3DXVECTOR3 _rot(0.0f,0.0f,0.0f);
	D3DXVECTOR3 _size(1.0f,1.0f,1.0f);
	UINT _shape = 0;
	BOOL _static=FALSE;
	UINT _objColor=COLOR_NONE ;
	BOOL _isUseCellEffect = TRUE;
	BOOL _isRendLine = TRUE;
	BOOL _isRendShadow = TRUE;
	BOOL _isReflectShadow = TRUE;
	UINT	len;
	vector<D3DXVECTOR3> RandomList;
	D3DXVECTOR3		RandomPos;

	while(_fgetts(Data,256,(*pFile))){
		len = _tcslen(Data);
		Data[len-1] = NULL;
		if( 0 == _tcscmp(Data,_T("[END]"))){
			break;
		}
		_flag = _tcstok_s(Data,_T("="),&_val);
		if(_val != NULL){
			_val = _tcstok_s(_val,_T(";"),&_val); 
			if(NULL == _val){
				continue;
			}
			if( 0 == _tcscmp(_flag,_T("FileName"))){
				GetTcsFromData(_val,_filename);
			}else if(0 == _tcscmp(_flag,_T("CollShape"))){
				GetShapeFromData(_val,_shape);
			}else if(0 == _tcscmp(_flag,_T("IsStatic"))){
				GetBOOLFromData(_val,_static);
			}else if(0 == _tcscmp(_flag,_T("Pos"))){
				GetVec3FromData(_val,_pos);
			}else if(0 == _tcscmp(_flag,_T("Rot"))){
				GetVec3FromData(_val,_rot);
			}else if(0 == _tcscmp(_flag,_T("Size"))){
				GetVec3FromData(_val,_size);
			}else if(0 == _tcscmp(_flag,_T("ObjectName"))){
				GetTcsFromData(_val,_objname);
			}else if(0 == _tcscmp(_flag,_T("ObjectColor"))){
				GetColorFromData(_val,_objColor);
			}else if(0 == _tcscmp(_flag,_T("RendShadow"))){
				GetBOOLFromData(_val,_isRendShadow);
			}else if(0 == _tcscmp(_flag,_T("ReflectShadow"))){
				GetBOOLFromData(_val,_isReflectShadow);
			}else if(0 == _tcscmp(_flag,_T("UseLightEffect"))){
				GetBOOLFromData(_val,_isUseCellEffect);
			}else if(0 == _tcscmp(_flag,_T("RendLine"))){
				GetBOOLFromData(_val,_isRendLine);
			}else if(0 == _tcscmp(_flag,_T("RandomPos"))){
				GetVec3FromData(_val,RandomPos);
				RandomList.push_back(RandomPos);
			}else{
				_DEBUG_LOG(_T("Error - Readflag value [ %s ]\n"),_flag);
			}
		}
	}

	// 등록
	if(pObj!=NULL)
		pObj->SetSoundManager(m_pSoundMgr);


	int i = RandomList.size();
	if( 0 < i){
		i = rand()%i;
		return m_pObjMgr->AddPassiveObject(_filename,_shape,_static,RandomList[i],_rot,_size,_objname,_objColor,_isRendShadow,_isReflectShadow,_isUseCellEffect,_isRendLine,pObj,_bodyflag);
	}else{
		return m_pObjMgr->AddPassiveObject(_filename,_shape,_static,_pos,_rot,_size,_objname,_objColor,_isRendShadow,_isReflectShadow,_isUseCellEffect,_isRendLine,pObj,_bodyflag);
	}

	return E_FAIL;
}
//////////////////////////////////////////////////////////////////////////
// CreateSpriteFromFile
// 파일 읽어 Sprite 추가
HRESULT Manager::CreateSpriteFromFile( FILE** pFile ,BOOL _puase)
{
	TCHAR Data[256];
	TCHAR* _val;
	TCHAR* _flag;
	TCHAR _filename[MAX_OBJNAME] = _T("NONE");
	POINT	_imgsize = { 1024 , 768 };
	O2Rect	_imgrect(0,1024,0,768);
	D3DXVECTOR3 _pos(0.0f,0.0f,0.0f);
	UINT len;

	while(_fgetts(Data,256,(*pFile))){
		len = _tcslen(Data);
		Data[len-1] = NULL;

		if( 0 == _tcscmp(Data,_T("[END]"))){
			break;
		}
		_flag = _tcstok_s(Data,_T("="),&_val);
		if(_val != NULL){
			_val = _tcstok_s(_val,_T(";"),&_val);
			if(NULL == _val){
				continue;
			}
			if( 0 == _tcscmp(_flag,_T("FileName"))){
				GetTcsFromData(_val,_filename);
			}else if(0 == _tcscmp(_flag,_T("Pos"))){
				GetVec3FromData(_val,_pos);
			}else if(0 == _tcscmp(_flag,_T("ImgSize"))){
				GetPtFromData(_val,_imgsize);
			}else if(0 == _tcscmp(_flag,_T("ImgRect"))){
				GetRectFromData(_val,_imgrect);
			}else{
				_DEBUG_LOG(_T("Error - Readflag value [ %s ]\n"),_flag);
			}
		}
	}

	// GUI 등록
	return m_pGuiMgr->CreateSprite(_filename,_imgsize,_pos,_imgrect,_puase);
}

//////////////////////////////////////////////////////////////////////////
// CreateButtonFromFile
// 파일 읽어 Button 추가
HRESULT Manager::CreateButtonFromFile( FILE** pFile ,BOOL _puase)
{
	TCHAR Data[256];
	TCHAR* _val;
	TCHAR* _flag;
	TCHAR _filename[MAX_OBJNAME] = _T("NONE");
	UINT	_Index=0;
	POINT	_imgsize = { 1024 , 768 };
	O2Rect	_NormalRect(0,1024,0,768);
	O2Rect	_DownRect(0,1024,0,768);
	O2Rect	_ClickRect(0,1024,0,768);
	D3DXVECTOR3 _pos(0,0,0);
	UINT len;

	while(_fgetts(Data,256,(*pFile))){
		len = _tcslen(Data);
		Data[len-1] = NULL;

		if( 0 == _tcscmp(Data,_T("[END]"))){
			break;
		}
		_flag = _tcstok_s(Data,_T("="),&_val);
		if(_val != NULL){
			_val = _tcstok_s(_val,_T(";"),&_val); 
			if(NULL == _val){
				continue;
			}
			if( 0 == _tcscmp(_flag,_T("FileName"))){
				GetTcsFromData(_val,_filename);
			}else if(0 == _tcscmp(_flag,_T("Pos"))){
				GetVec3FromData(_val,_pos);
			}else if(0 == _tcscmp(_flag,_T("Index"))){
				GetUintFromData(_val,_Index);
			}else if(0 == _tcscmp(_flag,_T("ImgSize"))){
				GetPtFromData(_val,_imgsize);
			}else if(0 == _tcscmp(_flag,_T("NormalRect"))){
				GetRectFromData(_val,_NormalRect);
			}else if(0 == _tcscmp(_flag,_T("DownRect"))){
				GetRectFromData(_val,_DownRect);
			}else if(0 == _tcscmp(_flag,_T("ClickRect"))){
				GetRectFromData(_val,_ClickRect);
			}else{
				_DEBUG_LOG(_T("Error - Readflag value [ %s ]\n"),_flag);
			}
		}
	}

	// 버튼 등록
	return m_pGuiMgr->CreateButton(_Index,_filename,_imgsize,_pos,_NormalRect,_DownRect,_ClickRect,_puase);
}
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// GetTcsFromData
// file data 로 부터 "" 안의 내용을 읽어옴
VOID Manager::GetTcsFromData( TCHAR* _para , TCHAR* Tcs )
{
	_para = _tcstok_s(_para,_T("\""),&_para);
	_tcscpy_s(Tcs,MAX_OBJNAME,_para);
}
//////////////////////////////////////////////////////////////////////////
// GetShapeFromData
// file data 로 부터 어떤 모양인지 읽어옴
VOID Manager::GetShapeFromData( TCHAR* _para , UINT& _shape )
{
	if(0 == _tcscmp(_para,_T("BOX"))){
		_shape = NX_SHAPE_BOX;
	}else if(0 == _tcscmp(_para,_T("SPHERE"))){
		_shape = NX_SHAPE_SPHERE;
	}else if(0 == _tcscmp(_para,_T("CAPSULE"))){
		_shape = NX_SHAPE_CAPSULE;
	}else if(0 == _tcscmp(_para,_T("MESH"))){
		_shape = NX_SHAPE_MESH;
	}else if(0 == _tcscmp(_para,_T("NONE"))){
		_shape = 0;
	}else{
		_DEBUG_LOG(_T("Error - GetShapeFromData value [ %s ]\n"),_para);
		_shape = 0;
	}
}
//////////////////////////////////////////////////////////////////////////
// GetBOOLFromData
// file data 로 부터 BOOL 형 읽어옴
VOID Manager::GetBOOLFromData( TCHAR* _para,BOOL& _bool )
{
	if(0 == _tcscmp(_para,_T("TRUE"))){
		_bool = TRUE;
	}else if(0 == _tcscmp(_para,_T("FALSE"))){
		_bool = FALSE;
	}else{
		_DEBUG_LOG(_T("Error - GetBOOLFromData value [ %s ]\n"),_para);
		_bool = 0;
	}
}
//////////////////////////////////////////////////////////////////////////
// GetCenterFromData
// file data 로 부터 중심이 어디인지 읽어옴
VOID Manager::GetCenterFromData( TCHAR* _para,UINT& _Center )
{
	if(0 == _tcscmp(_para,_T("MIDBOT"))){
		_Center = GUI_CENTER_MIDBOT;
	}else if(0 == _tcscmp(_para,_T("MIDTOP"))){
		_Center = GUI_CENTER_MIDTOP;
	}else if(0 == _tcscmp(_para,_T("CENTER"))){
		_Center = GUI_CENTER_CENTER;
	}else{
		_DEBUG_LOG(_T("Error - GetCenterFromData value [ %s ]\n"),_para);
		_Center = GUI_CENTER_CENTER;
	}
}
//////////////////////////////////////////////////////////////////////////
// GetShapeFromData
// file data 로 부터 어떤 색상인지 읽어옴
VOID Manager::GetColorFromData( TCHAR* _para,UINT& _Color )
{
	if(0 == _tcscmp(_para,_T("RED"))){
		_Color = COLOR_R;
	}else if(0 == _tcscmp(_para,_T("GREEN"))){
		_Color = COLOR_G;
	}else if(0 == _tcscmp(_para,_T("BLUE"))){
		_Color = COLOR_B;
	}else if(0 == _tcscmp(_para,_T("YELLOW"))){
		_Color = COLOR_P;
	}else if(0 == _tcscmp(_para,_T("NONE"))){
		_Color = COLOR_NONE;
	}else{
		_DEBUG_LOG(_T("Error - GetColorFromData value [ %s ]\n"),_para);
		_Color = COLOR_NONE;
	}
}

//////////////////////////////////////////////////////////////////////////
// GetIntFromData
// file data 로부터 int 값을 읽어옴
VOID Manager::GetIntFromData( TCHAR* _para , INT& _int )
{
	_int = _ttoi(_para);
}
//////////////////////////////////////////////////////////////////////////
// GetUintFromData
// file data 로부터 UINT 값을 읽어옴
VOID Manager::GetUintFromData( TCHAR* _para , UINT& _int )
{
	_int = _ttoi(_para);
}

//////////////////////////////////////////////////////////////////////////
// GetVec3FromData
// D3DXVECTOR3 를 읽어오 <x,y,z> 형태
VOID Manager::GetVec3FromData( TCHAR* _para , D3DXVECTOR3& _vec )
{
	TCHAR* _val;
	// <> 제거
	++_para;
	_para = _tcstok_s(_para,_T(">"),&_para);

	_val = _tcstok_s(_para,_T(","),&_para);
	_vec.x = _tstof(_val);
	_val = _tcstok_s(_para,_T(","),&_para);
	_vec.y = _tstof(_val);
	_val = _tcstok_s(_para,_T(","),&_para);
	_vec.z = _tstof(_val);
}

//////////////////////////////////////////////////////////////////////////
// GetFloatFromData
// float 를 읽어옴
VOID Manager::GetFloatFromData( TCHAR* _para,float& _pf )
{
	_pf = _tstof(_para);
}
//////////////////////////////////////////////////////////////////////////
// GetRectFromData
// Rect 를 읽어옴 형식은 <left,right,top,bottom>
VOID Manager::GetRectFromData( TCHAR* _para,O2Rect& _rect )
{
	TCHAR* _val;
	// <> 제거
	++_para;
	_para = _tcstok_s(_para,_T(">"),&_para);

	_val = _tcstok_s(_para,_T(","),&_para);
	_rect.left		= _ttoi(_val);
	_val = _tcstok_s(_para,_T(","),&_para);
	_rect.right		= _ttoi(_val);
	_val = _tcstok_s(_para,_T(","),&_para);
	_rect.top		= _ttoi(_val);
	_val = _tcstok_s(_para,_T(","),&_para);
	_rect.bottom	= _ttoi(_val);
}
//////////////////////////////////////////////////////////////////////////
// GetPointFromData
// Point 를 읽어옴 형식은 <x,y>
VOID Manager::GetPtFromData( TCHAR* _para,POINT& _pt )
{
	TCHAR* _val;
	// <> 제거
	++_para;
	_para = _tcstok_s(_para,_T(">"),&_para);
	_val = _tcstok_s(_para,_T(","),&_para);
	_pt.x = _ttoi(_val);
	_pt.y = _ttoi(_para);
}

//////////////////////////////////////////////////////////////////////////
// ChangeState
// 메인 매니저 상태 변경
HRESULT Manager::ChangeState( UINT pState )
{
	HRESULT hr = S_OK;
	m_bLoading = TRUE;
	m_pLoading->Run(&m_bLoading);

	SetCursor(LoadCursor(NULL,IDC_ARROW));

	//if( m_iState != pState )
	{
		m_iState = pState;
		switch(m_iState)
		{
		case GS_MAIN:
			m_pfFm = &Manager::Fm_Main;
			hr = LoadFile(SAVE_MAIN);
			break;
		case GS_STAGE1:
			m_pLight->SetPos(D3DXVECTOR3(100.0f,500.0f,100.0f));
			m_fStageTime = 0.0f;
			m_iIsCreateMob = 0;
			m_pfFm = &Manager::Fm_Play;
			m_pResource->GetActiveMesh(_T("ali.X"));
			m_pResource->GetMesh(_T("ali.X"));
			hr = LoadFile(SAVE_STAGE1);
			m_pGuiMgr->SetHero(Hero);
			Hero->SetMaxColor(25.0f);
			Hero->SetFullColor();

			if(SUCCEEDED(hr)){
				hr = LoadFile(SAVE_GUI);
			}
			
			if(SUCCEEDED(hr)){
				hr = LoadFile(SAVE_HIDING);
			}

			if(SUCCEEDED(hr)){
				hr = LoadFile(SAVE_TIP);
			}
			
			SetCursor(NULL);
			m_pCamMgr->Switch_CT(O2CameraManager::C_FOLLOW);
			break;
		case GS_STAGE2:
			m_pLight->SetPos(D3DXVECTOR3(100.0f,500.0f,100.0f));
			m_fStageTime = 0.0f;
			m_iIsCreateMob = 0;
			m_bWillPlayVideo = FALSE;
			m_pfFm = &Manager::Fm_Play;
			hr = LoadFile(SAVE_STAGE2);
			m_pGuiMgr->SetHero(Hero);
			Hero->SetMaxColor(40.0f);
			Hero->SetFullColor();
			Hero->SetColorVisible(COLOR_R);
			if(SUCCEEDED(hr)){
				hr = LoadFile(SAVE_GUI);
			}
			
			if(SUCCEEDED(hr)){
				hr = LoadFile(SAVE_HIDING);
			}

			SetCursor(NULL);
			m_pCamMgr->Switch_CT(O2CameraManager::C_FOLLOW);
			break;
		case GS_STAGE3:
			m_pLight->SetPos(D3DXVECTOR3(0.0f,300.0f,0.0f));
			m_fStageTime = 0.0f;
			m_iIsCreateMob = 0;
			m_bWillPlayVideo = FALSE;
			m_pfFm = &Manager::Fm_Play;
			hr = LoadFile(SAVE_STAGE3);
			m_pGuiMgr->SetHero(Hero);
			Hero->SetColorVisible(COLOR_R);
			Hero->SetColorVisible(COLOR_B);
			Hero->SetMaxColor(30.0f);
			Hero->SetFullColor();
			if(SUCCEEDED(hr)){
				hr = LoadFile(SAVE_GUI);
			}

			if(SUCCEEDED(hr)){
				hr = LoadFile(SAVE_HIDING);
			}

			SetCursor(NULL);
			m_pCamMgr->Switch_CT(O2CameraManager::C_FOLLOW);
			break;
		case GS_STAGE_END:
			{
				m_pfFm = &Manager::Fm_Ending;
				hr = LoadFile(SAVE_ENDING);
			}
			break;
		case GS_CREATE:
			{
				m_bWillPlayVideo = FALSE;
				m_pfFm = &Manager::Fm_Create;
				hr = LoadFile(SAVE_CREATE);
				m_pCamMgr->Switch_CT(O2CameraManager::C_FIXED);
				m_pCamMgr->SetFixedCamera(D3DXVECTOR3(0.0f,0.0f,512.0f),D3DXVECTOR3(0.0f,0.0f,0.0f));
			}
			break;
		case GS_HELP:
			m_pLight->SetPos(D3DXVECTOR3(100.0f,0.0f,600.0f));
			m_bWillPlayVideo = FALSE;
			m_pfFm = &Manager::Fm_Help;
			hr = LoadFile(SAVE_HELP);
			m_pSoundMgr->PlayBGM(O2SoundManager::S_BGM_ID_OPENING);
			m_pCamMgr->Switch_CT(O2CameraManager::C_FIXED);
			m_pCamMgr->SetFixedCamera(D3DXVECTOR3(0.0f,0.0f,512.0f),D3DXVECTOR3(0.0f,0.0f,0.0f));
			break;
		default:
			break;
		}
	}
	m_pTimer->GetElapsedTime();
	m_fElapsed = 0.0f;
	m_bLoading = FALSE;
	Sleep(50);

	if(FAILED(hr)){
		SendMessage(m_hWnd,WM_DESTROY,0,0);
	}
	return hr;
}

//////////////////////////////////////////////////////////////////////////
// Flush
// 모든 오브젝트 해제
void Manager::Flush()
{
	m_pVideo->Pause();
	m_bShowVideo = FALSE;
	m_pSoundMgr->DestroyAll();
	m_pGuiMgr->Flush();
	m_pObjMgr->Flush();
	m_pPartMgr->DestroyAll();
	Hero = NULL;
	m_bPause = FALSE;
	m_iTip = 0;
	m_pCamMgr->ResetCamera(m_iState);
}

//////////////////////////////////////////////////////////////////////////
// Fm_Main
// Main 상태에서의 FrameMove
HRESULT Manager::Fm_Main()
{
	m_pSoundMgr->PlayBGM(O2SoundManager::S_BGM_ID_OPENING);

	UINT flag=0;
	HRESULT hr=S_OK;
	// 메인에서 관리하는 키 인풋
	In_Main();

	m_pCamMgr->Update();

	// 오브젝트들 업데이트
	m_pObjMgr->UpDate();
	// 파티클 업데이트
	m_pPartMgr->Update(m_fElapsed,*m_pCamMgr->GetCamPos(),m_pCamMgr->GetMatView(),m_pCamMgr->GetQuaternion());
	// GUI 업데이트
	flag = m_pGuiMgr->UpDate();

	// 눌려진 버튼에 따라 수행할 항목
	m_bWillPlayVideo = TRUE;
	switch(flag){
		case UI_START:
			hr = ChangeState(GS_STAGE1);
			break;
		case UI_CREATE:
			hr = ChangeState(GS_CREATE);
			break;
		case UI_HELP:
			hr = ChangeState(GS_HELP);
			break;
		case UI_END:
			return E_FAIL;
			break;
		case UI_SOUND:
			m_pSoundMgr->SoundOnOff();
			break;
		default:
			break;
	}
	return hr;
}
//////////////////////////////////////////////////////////////////////////
// Fm_Play
// Play 상태에서의 프레임 무브 ( 내부연산 )
HRESULT Manager::Fm_Play()
{
	UINT flag=0;
	HRESULT hr=S_OK;

	
	if(m_iState==GS_STAGE1)
	{
		m_pSoundMgr->PlayBGM(O2SoundManager::S_BGM_ID_STAGE01);
		if(m_iTip==0)
		{
			SetPause(FALSE);
			m_iTip=1;
		}else if(m_pGuiMgr->GetTipOver()&&m_iTip==1)
		{
			SetPause(FALSE);
			m_iTip=2;
		}
	}else if(m_iState==GS_STAGE2)
	{	
		m_pSoundMgr->PlayBGM(O2SoundManager::S_BGM_ID_STAGE02);
		m_iTip=2;
	}else if(m_iState==GS_STAGE3)
	{
		m_pSoundMgr->PlayBGM(O2SoundManager::S_BGM_ID_STAGE03);
		m_iTip=2;
	}else
		m_iTip=2;
	
	// 현재 포즈 모드인지 검사
	if(m_bPause)
	{
		m_fElapsed = 0.0f;
		SetCursor(LoadCursor(NULL,IDC_ARROW));
	}else{
		if(m_bFocus){
			SetCursor(NULL);
			SetCursorPos(m_rWinRect.left+m_rWinRect.GetWidth()*0.5f,m_rWinRect.top+m_rWinRect.GetHeight()*0.5f);
		}
	}

	// PhysX 시뮬레이션
	if(Hero&&Hero->GetADTime()<=0)
		Hero->SetHide(FALSE);
	JNxScene->simulate(m_fElapsed);
	JNxScene->flushStream();
	JNxScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
	// 시뮬레이션 끝

	// 오브젝트들 업데이트
	flag = m_pObjMgr->UpDate();
	m_pCamMgr->Update(Hero);

	switch(flag){
		case O2Hero::STATE_CLEAR:
			if( m_iState < GS_STAGE_END){
				m_iNextState = m_iState + 1;
			}else{
				m_iNextState = GS_MAIN;
			}
			hr = AddState(GS_WIN);
			break;
		case O2Hero::STATE_DIE:
			hr = AddState(GS_LOST);
			break;
	}
	if(FAILED(hr)){
		return hr;
	}


	// 파티클 업데이트
	m_pPartMgr->Update(m_fElapsed,*m_pCamMgr->GetCamPos(),m_pCamMgr->GetMatView(),m_pCamMgr->GetQuaternion());
	m_pSoundMgr->SetListen(&(Hero->GetPos()));
	m_pSoundMgr->UpdateSound();
	// GUI 업데이트
	flag = m_pGuiMgr->UpDate();

	// 플레이 인풋
	In_Play();


	// 눌려진 버튼 인덱스에 따라 
	switch(flag){
		case UI_RESUME:
			hr = ChangeState(m_iState);
			break;
		case UI_QUIT:
			hr = ChangeState(GS_MAIN);
			break;
		case UI_PAUSE:
			SetPause();
			break;
		case UI_OVER:
			hr = AddState(GS_LOST);
			break;
		case UI_SOUND:
			//MessageBox(0,_T("Sound On/Off!"),0,0);
			m_pSoundMgr->SoundOnOff();
			break;
		default:
			break;
	}

	CreateMob();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// In_Main()
// Main 상태에서의 Input
VOID Manager::In_Main()
{

}

//////////////////////////////////////////////////////////////////////////
// In_Play()
// Play 상태에서의 Input
VOID Manager::In_Play()
{
	// 게임포즈
	if(m_pInputMgr->IsKeyPress(DIK_ESCAPE)&&(m_iTip==2)){
		SetPause();
	}

	// 회전
	if(!m_bPause){
		Hero->ProcInput(m_pInputMgr);
		// 카메라의 키입력 처리(마우스에 따른 화면인동 등등 )
		m_pCamMgr->Input();
	}
}

HRESULT Manager::AddActiveObjFromFile( FILE** pFile,O23DActiveObject* pObject/*=NULL*/ )
{
	TCHAR Data[256];
	TCHAR* _val;
	TCHAR* _flag;
	TCHAR _filename[MAX_OBJNAME] = _T("NONE");
	TCHAR _objname[MAX_OBJNAME] = _T("NONE");
	D3DXVECTOR3 _pos(0.0f,0.0f,0.0f);
	D3DXVECTOR3 _rot(0.0f,0.0f,0.0f);
	D3DXVECTOR3 _size(1.0f,1.0f,1.0f);
	UINT _shape = 0;
	BOOL _static=FALSE;
	UINT _objColor=COLOR_NONE ;
	BOOL _isUseCellEffect = TRUE;
	BOOL _isRendLine = TRUE;
	BOOL _isRendShadow = TRUE;
	BOOL _isReflectShadow = TRUE;
	UINT len;
	vector<D3DXVECTOR3> RandomList;
	D3DXVECTOR3		RandomPos;

	while(_fgetts(Data,256,(*pFile))){
		len = _tcslen(Data);
		Data[len-1] = NULL;
		if( 0 == _tcscmp(Data,_T("[END]"))){
			break;
		}
		_flag = _tcstok_s(Data,_T("="),&_val);
		if(_val != NULL){
			_val = _tcstok_s(_val,_T(";"),&_val); 
			if( 0 == _tcscmp(_flag,_T("FileName"))){
				GetTcsFromData(_val,_filename);
			}else if(0 == _tcscmp(_flag,_T("CollShape"))){
				GetShapeFromData(_val,_shape);
			}else if(0 == _tcscmp(_flag,_T("IsStatic"))){
				GetBOOLFromData(_val,_static);
			}else if(0 == _tcscmp(_flag,_T("Pos"))){
				GetVec3FromData(_val,_pos);
			}else if(0 == _tcscmp(_flag,_T("Rot"))){
				GetVec3FromData(_val,_rot);
			}else if(0 == _tcscmp(_flag,_T("Size"))){
				GetVec3FromData(_val,_size);
			}else if(0 == _tcscmp(_flag,_T("ObjectName"))){
				GetTcsFromData(_val,_objname);
			}else if(0 == _tcscmp(_flag,_T("ObjectColor"))){
				GetColorFromData(_val,_objColor);
			}else if(0 == _tcscmp(_flag,_T("RendShadow"))){
				GetBOOLFromData(_val,_isRendShadow);
			}else if(0 == _tcscmp(_flag,_T("ReflectShadow"))){
				GetBOOLFromData(_val,_isReflectShadow);
			}else if(0 == _tcscmp(_flag,_T("UseLightEffect"))){
				GetBOOLFromData(_val,_isUseCellEffect);
			}else if(0 == _tcscmp(_flag,_T("RendLine"))){
				GetBOOLFromData(_val,_isRendLine);
			}else if(0 == _tcscmp(_flag,_T("RandomPos"))){
				GetVec3FromData(_val,RandomPos);
				RandomList.push_back(RandomPos);
			}else{
				_DEBUG_LOG(_T("Error - Readflag value [ %s ]\n"),_flag);
			}
		}
	}

	int i = RandomList.size();
	if( 0 < i){
		i = rand()%i;
		return m_pObjMgr->AddActiveObject(_filename,_shape,_static,RandomList[i],_rot,_size,_objname,_objColor,_isRendShadow,_isReflectShadow,_isUseCellEffect,_isRendLine,pObject);
	}else{
		return m_pObjMgr->AddActiveObject(_filename,_shape,_static,_pos,_rot,_size,_objname,_objColor,_isRendShadow,_isReflectShadow,_isUseCellEffect,_isRendLine,pObject);
	}

	return E_FAIL;
}

VOID Manager::SetPause( BOOL pRendGUI /*= TRUE*/ )
{
	m_bPause = (!m_bPause);
	if(pRendGUI){
		m_pGuiMgr->SetPause(m_bPause);
		if(m_bPause==TRUE)
		{
			m_pSoundMgr->PauseEffect(O2SoundManager::S_TYPE_INGAME);
			m_pSoundMgr->PauseEffect(O2SoundManager::S_TYPE_NAMED);
		}else
		{
			m_pSoundMgr->PlayEffect(O2SoundManager::S_TYPE_INGAME);
			m_pSoundMgr->PlayEffect(O2SoundManager::S_TYPE_NAMED);
		}
	}
}

HRESULT Manager::Fm_Help()
{
	UINT flag=0;
	HRESULT hr=S_OK;

	// 오브젝트들 업데이트
	m_pObjMgr->UpDate();

	// GUI 업데이트
	flag = m_pGuiMgr->UpDate();

	// 플레이 인풋
	In_Help();

	m_pCamMgr->Update(Hero);

	// 파티클 업데이트
	m_pPartMgr->Update(m_fElapsed,*m_pCamMgr->GetCamPos(),m_pCamMgr->GetMatView(),m_pCamMgr->GetQuaternion());

	// 눌려진 버튼 인덱스에 따라 
	switch(flag){
		case UI_QUIT:
			hr = ChangeState(GS_MAIN);
			break;
		case UI_SOUND:
			m_pSoundMgr->SoundOnOff();
			break;
		default:
			break;
	}
	return hr;
}

VOID Manager::In_Help()
{
	Hero->ProcInputHelp(m_pInputMgr);
}

HRESULT Manager::CreateCheckFromFile( FILE** pFile,BOOL _puase /*= FALSE*/ )
{
	TCHAR Data[256];
	TCHAR* _val;
	TCHAR* _flag;
	TCHAR _filename[MAX_OBJNAME] = _T("NONE");
	POINT	_imgsize = { 1024 , 768 };
	O2Rect	_CheckRect(0,1024,0,768);
	O2Rect	_NormalRect(0,1024,0,768);
	D3DXVECTOR3 _pos(0.0f,0.0f,0.0f);
	UINT	_Index = 0;
	UINT len;

	while(_fgetts(Data,256,(*pFile))){
		len = _tcslen(Data);
		Data[len-1] = NULL;
		if( 0 == _tcscmp(Data,_T("[END]"))){
			break;
		}
		_flag = _tcstok_s(Data,_T("="),&_val);
		if(_val != NULL){
			_val = _tcstok_s(_val,_T(";"),&_val); 
			if(NULL == _val){
				continue;
			}
			if( 0 == _tcscmp(_flag,_T("FileName"))){
				GetTcsFromData(_val,_filename);
			}else if(0 == _tcscmp(_flag,_T("Pos"))){
				GetVec3FromData(_val,_pos);
			}else if(0 == _tcscmp(_flag,_T("ImgSize"))){
				GetPtFromData(_val,_imgsize);
			}else if(0 == _tcscmp(_flag,_T("NormalRect"))){
				GetRectFromData(_val,_NormalRect);
			}else if(0 == _tcscmp(_flag,_T("CheckRect"))){
				GetRectFromData(_val,_CheckRect);
			}else if(0 == _tcscmp(_flag,_T("Index"))){
				GetUintFromData(_val,_Index);
			}else{
				_DEBUG_LOG(_T("Error - Readflag value [ %s ]\n"),_flag);
			}
		}
	}

	// 등록
	return m_pGuiMgr->CreateCheck(_filename,_imgsize,_pos,_NormalRect,_CheckRect,_Index);
}

HRESULT Manager::CreateColorGageFromFile( FILE** pFile,BOOL _puase /*= FALSE*/ )
{
	TCHAR Data[256];
	TCHAR* _val;
	TCHAR* _flag;
	TCHAR _CoverFileName[MAX_OBJNAME] = _T("NONE");
	TCHAR _GreenFileName[MAX_OBJNAME] = _T("NONE");
	TCHAR _RedFileName[MAX_OBJNAME] = _T("NONE");
	TCHAR _BlueFileName[MAX_OBJNAME] = _T("NONE");
	TCHAR _YellowFileName[MAX_OBJNAME] = _T("NONE");
	POINT	_CoverSize = { 1024 , 768 };
	POINT	_BarSize = { 1024 , 768 };
	O2Rect	_CoverRect(0,1024,0,768);
	O2Rect	_BarRect(0,0,1024,768);
	D3DXVECTOR3 _CoverPos(0.0f,0.0f,0.0f);
	D3DXVECTOR3 _BarPos(0.0f,0.0f,0.0f);
	UINT len;

	while(_fgetts(Data,256,(*pFile))){
		len = _tcslen(Data);
		Data[len-1] = NULL;

		if( 0 == _tcscmp(Data,_T("[END]"))){
			break;
		}
		_flag = _tcstok_s(Data,_T("="),&_val);
		if(_val != NULL){
			_val = _tcstok_s(_val,_T(";"),&_val); 
			if(NULL == _val){
				continue;
			}
			if( 0 == _tcscmp(_flag,_T("CoverFile"))){
				GetTcsFromData(_val,_CoverFileName);
			}else if( 0 == _tcscmp(_flag,_T("GreenFile"))){
				GetTcsFromData(_val,_GreenFileName);
			}else if( 0 == _tcscmp(_flag,_T("RedFile"))){
				GetTcsFromData(_val,_RedFileName);
			}else if( 0 == _tcscmp(_flag,_T("BlueFile"))){
				GetTcsFromData(_val,_BlueFileName);
			}else if( 0 == _tcscmp(_flag,_T("YellowFile"))){
				GetTcsFromData(_val,_YellowFileName);
			}else if( 0 == _tcscmp(_flag,_T("CoverRect"))){
				GetRectFromData(_val,_CoverRect);
			}else if( 0 == _tcscmp(_flag,_T("BarRect"))){
				GetRectFromData(_val,_BarRect);
			}else if( 0 == _tcscmp(_flag,_T("CoverSize"))){
				GetPtFromData(_val,_CoverSize);
			}else if( 0 == _tcscmp(_flag,_T("BarSize"))){
				GetPtFromData(_val,_BarSize);
			}else if( 0 == _tcscmp(_flag,_T("CoverPos"))){
				GetVec3FromData(_val,_CoverPos);
			}else if( 0 == _tcscmp(_flag,_T("BarPos"))){
				GetVec3FromData(_val,_BarPos);
			}else{
				_DEBUG_LOG(_T("Error - Readflag value [ %s ]\n"),_flag);
			}
		}
	}

	// 등록
	return m_pGuiMgr->CreateColorGage(_CoverFileName,_GreenFileName,_RedFileName,_BlueFileName,_YellowFileName,
		_CoverSize,_BarSize,
		_CoverRect,_BarRect,
		_CoverPos,_BarPos);
}

HRESULT Manager::Add2DObjFromFile( FILE** pFile )
{
	TCHAR Data[256];
	TCHAR* _val;
	TCHAR* _flag;
	TCHAR _filename[MAX_OBJNAME] = _T("NONE");
	POINT	_imgsize = { 1024 , 768 };
	O2Rect	_imgrect(0,1024,0,768);
	D3DXVECTOR3 _pos(0.0f,0.0f,0.0f);
	D3DXVECTOR3 _rot(0.0f,0.0f,0.0f);
	D3DXVECTOR3 _size(1.0f,1.0f,1.0f);
	UINT	len;

	while(_fgetts(Data,256,(*pFile))){
		len = _tcslen(Data);
		Data[len-1] = NULL;

		if( 0 == _tcscmp(Data,_T("[END]"))){
			break;
		}
		_flag = _tcstok_s(Data,_T("="),&_val);
		if(_val != NULL){
			_val = _tcstok_s(_val,_T(";"),&_val); 
			if(NULL == _val){
				continue;
			}
			if( 0 == _tcscmp(_flag,_T("FileName"))){
				GetTcsFromData(_val,_filename);
			}else if(0 == _tcscmp(_flag,_T("Pos"))){
				GetVec3FromData(_val,_pos);
			}else if(0 == _tcscmp(_flag,_T("Rot"))){
				GetVec3FromData(_val,_rot);
			}else if(0 == _tcscmp(_flag,_T("Size"))){
				GetVec3FromData(_val,_size);
			}else if(0 == _tcscmp(_flag,_T("ImgSize"))){
				GetPtFromData(_val,_imgsize);
			}else if(0 == _tcscmp(_flag,_T("ImgRect"))){
				GetRectFromData(_val,_imgrect);
			}else{
				_DEBUG_LOG(_T("Error - Readflag value [ %s ]\n"),_flag);
			}
		}
	}

	return m_pObjMgr->Add2DObject(_filename,_imgrect,_imgsize,_pos,_size,_rot);
}

HRESULT Manager::CreateTimeGageFromFile( FILE** pFile,BOOL _puase /*= FALSE*/ )
{
	TCHAR Data[256];
	TCHAR* _val;
	TCHAR* _flag;
	TCHAR _CoverFile[MAX_OBJNAME]= _T("NONE");
	TCHAR _BarFile[MAX_OBJNAME] = _T("NONE");
	POINT	_CoverSize={1024,768};
	POINT	_BarSize={1024,768};
	O2Rect	_CoverRect(0,1024,0,768);
	O2Rect	_BarRect(0,1024,0,768);
	D3DXVECTOR3 _CoverPos(0.0f,0.0f,0.0f);
	D3DXVECTOR3 _BarPos(0.0f,0.0f,0.0f);
	float	_time = 1.0f;
	UINT len;

	while(_fgetts(Data,256,(*pFile))){
		len = _tcslen(Data);
		Data[len-1] = NULL;
		if( 0 == _tcscmp(Data,_T("[END]"))){
			break;
		}
		_flag = _tcstok_s(Data,_T("="),&_val);
		if(_val != NULL){
			_val = _tcstok_s(_val,_T(";"),&_val); 
			if(NULL == _val){
				continue;
			}
			if( 0 == _tcscmp(_flag,_T("CoverFile"))){
				GetTcsFromData(_val,_CoverFile);
			}else if( 0 == _tcscmp(_flag,_T("BarFile"))){
				GetTcsFromData(_val,_BarFile);
			}else if(0 == _tcscmp(_flag,_T("CoverSize"))){
				GetPtFromData(_val,_CoverSize);
			}else if(0 == _tcscmp(_flag,_T("BarSize"))){
				GetPtFromData(_val,_BarSize);
			}else if(0 == _tcscmp(_flag,_T("CoverRect"))){
				GetRectFromData(_val,_CoverRect);
			}else if(0 == _tcscmp(_flag,_T("BarRect"))){
				GetRectFromData(_val,_BarRect);
			}else if(0 == _tcscmp(_flag,_T("CoverPos"))){
				GetVec3FromData(_val,_CoverPos);
			}else if(0 == _tcscmp(_flag,_T("BarPos"))){
				GetVec3FromData(_val,_BarPos);
			}else if(0 == _tcscmp(_flag,_T("Time"))){
				GetFloatFromData(_val,_time);
			}else{
				_DEBUG_LOG(_T("Error - Readflag value [ %s ]\n"),_flag);
			}
		}
	}
	// 등록
	return m_pGuiMgr->CreateTimeGage(_CoverFile,_BarFile,_CoverSize,_BarSize,_CoverRect,_BarRect,_CoverPos,_BarPos,_time);
}

//////////////////////////////////////////////////////////////////////////
// CreateHidingEffectFromFile
// 파일 읽어 HidingEffect 추가
HRESULT Manager::CreateHidingEffectFromFile( FILE** pFile)
{
	TCHAR Data[256];
	TCHAR* _val;
	TCHAR* _flag;
	UINT _index = 0;
	TCHAR _filename[MAX_OBJNAME] = _T("NONE");
	D3DXVECTOR3 _pos(0.0f,0.0f,0.0f);
	O2Rect	_rect(0,1024,0,768);
	POINT _imgsize={1024,768};
	float _drawtime=1.0f;
	UINT len;

	while(_fgetts(Data,256,(*pFile))){
		len = _tcslen(Data);
		Data[len-1] = NULL;
		if( 0 == _tcscmp(Data,_T("[END]"))){
			break;
		}
		_flag = _tcstok_s(Data,_T("="),&_val);
		if(_val != NULL){
			_val = _tcstok_s(_val,_T(";"),&_val); 
			if(NULL == _val){
				continue;
			}
			if( 0 == _tcscmp(_flag,_T("FileName"))){
				GetTcsFromData(_val,_filename);
			}else if( 0 == _tcscmp(_flag,_T("Index"))){
				GetUintFromData(_val,_index);
			}else if( 0 == _tcscmp(_flag,_T("Pos"))){
				GetVec3FromData(_val,_pos);
			}else if( 0 == _tcscmp(_flag,_T("ImgSize"))){
				GetPtFromData(_val,_imgsize);
			}else if( 0 == _tcscmp(_flag,_T("ImgRect"))){
				GetRectFromData(_val,_rect);
			}else if( 0 == _tcscmp(_flag,_T("Time"))){
				GetFloatFromData(_val,_drawtime);
			}else{
				_DEBUG_LOG(_T("Error - Readflag value [ %s ]\n"),_flag);
			}
		}
	}

	// GUI 등록
	return m_pGuiMgr->CreateHidingEffect(_index,_filename,_imgsize,_pos,_rect,_drawtime);

}
//
//////////////////////////////////////////////////////////////////////////

HRESULT Manager::CreateTimeSpriteFromFile( FILE** pFile,BOOL _puase /*= FALSE*/ )
{
	TCHAR Data[256];
	TCHAR* _val;
	TCHAR* _flag;
	TCHAR _filename[MAX_OBJNAME] = _T("NONE");
	POINT	_imgsize = { 1024 , 768 };
	O2Rect	_imgRect(0,1024,0,768);
	float	_Time = 1.0f;
	UINT	_Center = GUI_CENTER_CENTER;
	D3DXVECTOR3 _pos(0.0f,0.0f,0.0f);
	UINT len;

	while(_fgetts(Data,256,(*pFile))){
		len = _tcslen(Data);
		Data[len-1] = NULL;
		if( 0 == _tcscmp(Data,_T("[END]"))){
			break;
		}
		_flag = _tcstok_s(Data,_T("="),&_val);
		if(_val != NULL){
			_val = _tcstok_s(_val,_T(";"),&_val); 
			if( 0 == _tcscmp(_flag,_T("FileName"))){
				GetTcsFromData(_val,_filename);
			}else if(0 == _tcscmp(_flag,_T("Pos"))){
				GetVec3FromData(_val,_pos);
			}else if(0 == _tcscmp(_flag,_T("ImgSize"))){
				GetPtFromData(_val,_imgsize);
			}else if(0 == _tcscmp(_flag,_T("ImgRect"))){
				GetRectFromData(_val,_imgRect);
			}else if(0 == _tcscmp(_flag,_T("Time"))){
				GetFloatFromData(_val,_Time);
			}else if(0 == _tcscmp(_flag,_T("Pivot"))){
				GetCenterFromData(_val,_Center);
			}else{
				_DEBUG_LOG(_T("Error - Readflag value [ %s ]\n"),_flag);
			}
		}
	}

	// GUI 등록
	return m_pGuiMgr->CreateTimeSprite(_filename,_imgsize,_pos,_imgRect,_Time,_puase,_Center);

}

HRESULT Manager::Fm_WinLost()
{
	UINT flag=0;
	HRESULT hr=S_OK;

	// 파티클 업데이트
	m_pPartMgr->Update(m_fElapsed,*m_pCamMgr->GetCamPos(),m_pCamMgr->GetMatView(),m_pCamMgr->GetQuaternion());
	m_pSoundMgr->SetListen(&(Hero->GetPos()));
	m_pSoundMgr->UpdateSound();

	// 오브젝트들 업데이트
	m_pObjMgr->UpDate();

	m_pCamMgr->Update(Hero);
	m_fElapsed = 0.0f;
	// GUI 업데이트
	flag = m_pGuiMgr->UpDate();
	// 눌려진 버튼 인덱스에 따라 
	switch(flag){
		case UI_RESUME:
			hr = ChangeState(m_iState);
			break;
		case UI_QUIT:
			hr = ChangeState(GS_MAIN);
			break;
		case UI_NEXT:
			m_bWillPlayVideo = TRUE;
			hr = ChangeState(m_iNextState);
			break;
		case UI_SOUND:
			m_pSoundMgr->SoundOnOff();
			break;
		default:
			break;
	}

	return hr;
}

HRESULT Manager::AddState( UINT pState )
{
	HRESULT hr = S_OK;
	SetCursor(LoadCursor(NULL,IDC_ARROW));

	switch(pState)
	{
	case GS_LOST:
		m_pfFm = &Manager::Fm_WinLost;
		hr = LoadFile(SAVE_LOST);
		O2SOUNDINSTANCE->Play2DEffect(_T("GameOver"),O2SoundManager::S_TYPE_MENU,NULL);
		break;
	case GS_WIN:
		m_pfFm = &Manager::Fm_WinLost;
		hr = LoadFile(SAVE_WIN);
		break;
	default:
		break;
	}
	m_pTimer->GetElapsedTime();
	m_fElapsed = 0.0f;

	return hr;
}

HRESULT Manager::DrawVideo()
{
	PAINTSTRUCT ps;
	HDC hdc;

	if(m_pVideo){
		hdc = BeginPaint(m_hWnd, &ps);

		if (m_pVideo->State() != STATE_CLOSED && m_pVideo->HasVideo() && m_pVideo->State() != STATE_STOPPED)
		{
			// The player has video, so ask the player to repaint. 
			m_pVideo->Repaint(hdc);
		}else{
			m_bShowVideo = FALSE;
			ChangeState(m_iNextState);
			return E_FAIL;
		}
	}

	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
// 비디오 재생동안의 프레임 무브
// 동영상 스킵 / 동영상 종료 여부 판단.
//////////////////////////////////////////////////////////////////////////
HRESULT Manager::Fm_PlayVideo()
{
	LONGLONG CurPos=0;
	
	if(m_pVideo->CanSeek()){
		m_pVideo->GetCurrentPosition(&CurPos);
	}
	// 재생 시간이 끝나거나 비디오 재생이 멈추면 비디오 재생 끝
	if( m_ldVideoTime <= CurPos || FILTER_STATE::State_Stopped == m_pVideo->State() ){
		StopDrawVideo();
	}

	// ESC 키 누루면 재생 끝
	if(m_pInputMgr->IsKeyPress(DIK_ESCAPE)){
		StopDrawVideo();
	}else if(m_pInputMgr->IsKeyPress(DIK_RETURN)){
		StopDrawVideo();
	}else if(m_pInputMgr->IsMouseDown(MOUSE_LBUTTON)){
		if(m_pInputMgr->MouseInClient())
			StopDrawVideo();
	}else if(m_pInputMgr->IsMouseDown(MOUSE_RBUTTON)){
		if(m_pInputMgr->MouseInClient())
			StopDrawVideo();
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// 동영상 스킵 또는 동영상 재생이 끝났을 경우 호출
// 동영상 재생을 정지 하고 다음 스테이지 로드
//////////////////////////////////////////////////////////////////////////
VOID Manager::StopDrawVideo()
{
	m_pVideo->Pause();
	m_bShowVideo = FALSE;
	SetCursor(LoadCursor(NULL,IDC_ARROW));

	m_iStage = m_iNextState;

	switch(m_iNextState)
	{
	case GS_MAIN:
		m_pfFm = &Manager::Fm_Main;
		break;
	case GS_STAGE1:  
	case GS_STAGE2:
	case GS_STAGE3:
		m_fStageTime = 0.0f;
		m_iIsCreateMob = 0;
		m_pfFm = &Manager::Fm_Play;
		break;
	case GS_STAGE_END:
		m_pfFm = &Manager::Fm_Ending;
		break;
	case GS_CREATE:
		m_pfFm = &Manager::Fm_Create;
		break;
	case GS_HELP:
		m_pfFm = &Manager::Fm_Help;
		break;
	default:
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
// 비디오 재생
// 파일명 , 동영상 재생 시간, 동영상 재생 끝난후 바뀔 상태 스테이트
//////////////////////////////////////////////////////////////////////////
HRESULT Manager::PlayVideo( TCHAR* pFile,float pPlayTime,UINT pNextState )
{
	HRESULT hr;
	
	if(FALSE == m_bWillPlayVideo){
		return S_OK;
	}

	m_ldVideoTime = pPlayTime*10000000;
	m_iNextState = pNextState;

	m_bLoading = FALSE;
	Sleep(50);

	InvalidateRect(m_hWnd, NULL, FALSE);
	// 비디오 파일 열기 / 동영상 파일의 드로우(재생) 가능 여부 판단
#ifdef UNICODE
	hr = m_pVideo->OpenFile(pFile);
#else
	WCHAR	file[MAX_PATH];
	int len = strlen(pFile);
	mbstowcs(file,pFile,_tcslen(pFile));
	file[len] = NULL;
	hr = m_pVideo->OpenFile(file);
#endif
	if(FAILED(hr)){
		_DEBUG_LOG(_T("Failed Open Video [ %s ] \n"),pFile);
		MessageBox(NULL,_T("Failed Load Video"),_T("ERROR"),MB_OK);
		return hr;
	}

	// 비디오를 그려줄 화면 위치 설정 ( 실행중인 창에서 )
	hr = m_pVideo->UpdateVideoWindow(&m_rVideoRect);
	if(FAILED(hr)){
		_DEBUG_LOG(_T("Failed updata Video\n"));
		return hr;
	}

	hr = m_pVideo->Play();
	if( FAILED(hr)){
		_DEBUG_LOG(_T("Failed Play Video [ %s ] \n"),pFile);
		return hr;
	}
	m_bShowVideo = TRUE;
	m_pfFm = &Manager::Fm_PlayVideo;
	m_bWillPlayVideo = FALSE;

	return S_OK;
}

HRESULT Manager::PlayVideoFromFile( FILE** pFile )
{
	TCHAR Data[256];
	TCHAR* _val;
	TCHAR* _flag;
	TCHAR _filename[MAX_OBJNAME] = _T("NONE");
	float _Time = 0.0f;
	UINT len;

	while(_fgetts(Data,256,(*pFile))){
		len = _tcslen(Data);
		Data[len-1] = NULL;

		if( 0 == _tcscmp(Data,_T("[END]"))){
			break;
		}
		_flag = _tcstok_s(Data,_T("="),&_val);
		if(_val != NULL){
			_val = _tcstok_s(_val,_T(";"),&_val);
			if(NULL == _val){
				continue;
			}
			if( 0 == _tcscmp(_flag,_T("FileName"))){
				GetTcsFromData(_val,_filename);
			}else if(0 == _tcscmp(_flag,_T("Time"))){
				GetFloatFromData(_val,_Time);
			}else{
				_DEBUG_LOG(_T("Error - Readflag value [ %s ]\n"),_flag);
			}
		}
	}

	// GUI 등록
	return PlayVideo(_filename,_Time,m_iState);
}

HRESULT Manager::Fm_Create()
{
	UINT flag=0;
	HRESULT hr=S_OK;

	// 오브젝트들 업데이트
	m_pObjMgr->UpDate();

	// GUI 업데이트
	flag = m_pGuiMgr->UpDate();

	m_pCamMgr->Update();

	// 눌려진 버튼 인덱스에 따라 
	switch(flag){
		case UI_QUIT:
			hr = ChangeState(GS_MAIN);
			break;
		case UI_SOUND:
			m_pSoundMgr->SoundOnOff();
			break;
		default:
			break;
	}
	return hr;
}

//	Alien 추가
HRESULT Manager::AddAlienFromFile( FILE** pFile)
{
	TCHAR Data[256];
	TCHAR* _val;
	TCHAR* _flag;
	TCHAR _filename[MAX_OBJNAME] = _T("NONE");
	TCHAR _objname[MAX_OBJNAME] = _T("NONE");
	D3DXVECTOR3 _pos(0.0f,0.0f,0.0f);
	D3DXVECTOR3 _rot(0.0f,0.0f,0.0f);
	D3DXVECTOR3 _size(1.0f,1.0f,1.0f);
	UINT _shape = 0;
	BOOL _static=FALSE;
	UINT _objColor=COLOR_NONE ;
	BOOL _isUseCellEffect = TRUE;
	BOOL _isRendLine = TRUE;
	BOOL _isRendShadow = TRUE;
	BOOL _isReflectShadow = TRUE;
	UINT len;

	while(_fgetts(Data,256,(*pFile))){
		len = _tcslen(Data);
		Data[len-1] = NULL;
		if( 0 == _tcscmp(Data,_T("[END]"))){
			break;
		}
		_flag = _tcstok_s(Data,_T("="),&_val);
		if(_val != NULL){
			_val = _tcstok_s(_val,_T(";"),&_val); 
			if(NULL == _val){
				continue;
			}
			if( 0 == _tcscmp(_flag,_T("FileName"))){
				GetTcsFromData(_val,_filename);
			}else if(0 == _tcscmp(_flag,_T("CollShape"))){
				GetShapeFromData(_val,_shape);
			}else if(0 == _tcscmp(_flag,_T("IsStatic"))){
				GetBOOLFromData(_val,_static);
			}else if(0 == _tcscmp(_flag,_T("Pos"))){
				GetVec3FromData(_val,_pos);
			}else if(0 == _tcscmp(_flag,_T("Rot"))){
				GetVec3FromData(_val,_rot);
			}else if(0 == _tcscmp(_flag,_T("Size"))){
				GetVec3FromData(_val,_size);
			}else if(0 == _tcscmp(_flag,_T("ObjectName"))){
				GetTcsFromData(_val,_objname);
			}else if(0 == _tcscmp(_flag,_T("ObjectColor"))){
				GetColorFromData(_val,_objColor);
			}else if(0 == _tcscmp(_flag,_T("RendShadow"))){
				GetBOOLFromData(_val,_isRendShadow);
			}else if(0 == _tcscmp(_flag,_T("ReflectShadow"))){
				GetBOOLFromData(_val,_isReflectShadow);
			}else if(0 == _tcscmp(_flag,_T("UseLightEffect"))){
				GetBOOLFromData(_val,_isUseCellEffect);
			}else if(0 == _tcscmp(_flag,_T("RendLine"))){
				GetBOOLFromData(_val,_isRendLine);
			}else{
				_DEBUG_LOG(_T("Error - Readflag value [ %s ]\n"),_flag);
			}
		}
	}

	return m_pObjMgr->AddAlien(_filename,_shape,_static,_pos,_rot,_size,_objname,_objColor,_isRendShadow,_isReflectShadow,_isUseCellEffect,_isRendLine);
}

HRESULT Manager::AddItemObjFromFile( FILE** pFile )
{
	TCHAR Data[256];
	TCHAR* _val;
	TCHAR* _flag;
	TCHAR _filename[MAX_OBJNAME] = _T("NONE");
	TCHAR _objname[MAX_OBJNAME] = _T("NONE");
	D3DXVECTOR3 _pos(0.0f,0.0f,0.0f);
	D3DXVECTOR3 _rot(0.0f,0.0f,0.0f);
	D3DXVECTOR3 _size(1.0f,1.0f,1.0f);
	UINT _shape = 0;
	BOOL _static=FALSE;
	BOOL _isUseCellEffect = TRUE;
	BOOL _isRendLine = TRUE;
	BOOL _isRendShadow = TRUE;
	BOOL _isReflectShadow = TRUE;
	UINT	len;
	vector<D3DXVECTOR3> RandomList;
	D3DXVECTOR3		RandomPos;


	while(_fgetts(Data,256,(*pFile))){
		len = _tcslen(Data);
		Data[len-1] = NULL;
		if( 0 == _tcscmp(Data,_T("[END]"))){
			break;
		}
		_flag = _tcstok_s(Data,_T("="),&_val);
		if(_val != NULL){
			_val = _tcstok_s(_val,_T(";"),&_val); 
			if( NULL == _val)
				continue;
			if( 0 == _tcscmp(_flag,_T("FileName"))){
				GetTcsFromData(_val,_filename);
			}else if(0 == _tcscmp(_flag,_T("CollShape"))){
				GetShapeFromData(_val,_shape);
			}else if(0 == _tcscmp(_flag,_T("IsStatic"))){
				GetBOOLFromData(_val,_static);
			}else if(0 == _tcscmp(_flag,_T("Pos"))){
				GetVec3FromData(_val,_pos);
			}else if(0 == _tcscmp(_flag,_T("Rot"))){
				GetVec3FromData(_val,_rot);
			}else if(0 == _tcscmp(_flag,_T("Size"))){
				GetVec3FromData(_val,_size);
			}else if(0 == _tcscmp(_flag,_T("ObjectName"))){
				GetTcsFromData(_val,_objname);
			}else if(0 == _tcscmp(_flag,_T("RendShadow"))){
				GetBOOLFromData(_val,_isRendShadow);
			}else if(0 == _tcscmp(_flag,_T("ReflectShadow"))){
				GetBOOLFromData(_val,_isReflectShadow);
			}else if(0 == _tcscmp(_flag,_T("UseLightEffect"))){
				GetBOOLFromData(_val,_isUseCellEffect);
			}else if(0 == _tcscmp(_flag,_T("RandomPos"))){
				GetVec3FromData(_val,RandomPos);
				RandomList.push_back(RandomPos);
			}else if(0 == _tcscmp(_flag,_T("RendLine"))){
				GetBOOLFromData(_val,_isRendLine);
			}else{
				_DEBUG_LOG(_T("Error - Readflag value [ %s ]\n"),_flag);
			}
		}
	}

	int i = RandomList.size();
	if(i){
		i = rand()%i;
		return m_pObjMgr->AddItem(_filename,_shape,_static,RandomList[i],_rot,_size,_objname,_isRendShadow,_isReflectShadow,_isUseCellEffect,_isRendLine);
	}else{
		return m_pObjMgr->AddItem(_filename,_shape,_static,_pos,_rot,_size,_objname,_isRendShadow,_isReflectShadow,_isUseCellEffect,_isRendLine);
	}

	return E_FAIL;
}

//////////////////////////////////////////////////////////////////////////
// CreateHidingEffectFromFile
// 파일 읽어 HidingEffect 추가
HRESULT Manager::CreateTipFromFile( FILE** pFile)
{
	TCHAR Data[256];
	TCHAR* _val;
	TCHAR* _flag;
	TCHAR _filename[MAX_OBJNAME] = _T("NONE");
	D3DXVECTOR3 _pos(0.0f,0.0f,0.0f);
	O2Rect	_rect(0,1024,0,768);
	POINT _imgsize={1024,768};
	float _drawtime=1.0f;
	UINT len,_index=0;

	while(_fgetts(Data,256,(*pFile))){
		len = _tcslen(Data);
		Data[len-1] = NULL;
		if( 0 == _tcscmp(Data,_T("[END]"))){
			break;
		}
		_flag = _tcstok_s(Data,_T("="),&_val);
		if(_val != NULL){
			_val = _tcstok_s(_val,_T(";"),&_val); 
			if( 0 == _tcscmp(_flag,_T("FileName"))){
				GetTcsFromData(_val,_filename);
			}else if( 0 == _tcscmp(_flag,_T("Pos"))){
				GetVec3FromData(_val,_pos);
			}else if( 0 == _tcscmp(_flag,_T("Index"))){
				GetUintFromData(_val,_index);
			}else if( 0 == _tcscmp(_flag,_T("ImgSize"))){
				GetPtFromData(_val,_imgsize);
			}else if( 0 == _tcscmp(_flag,_T("ImgRect"))){
				GetRectFromData(_val,_rect);
			}else if( 0 == _tcscmp(_flag,_T("Time"))){
				GetFloatFromData(_val,_drawtime);
			}else{
				_DEBUG_LOG(_T("Error - Readflag value [ %s ]\n"),_flag);
			}
		}
	}

	// GUI 등록
	return m_pGuiMgr->CreateTIP(_index,_filename,_imgsize,_pos,_rect,_drawtime);

}

void Manager::CreateMob()
{
	int index =0;
	POINT pt = { 66, 55 };
	tstring filename = _T("NONE");
	m_fStageTime += m_fElapsed;

	if( m_iIsCreateMob > 0 )
	{
		// 개수 제한
		if(m_iIsCreateMob >= 5){
			return ;
		}

		if( m_fStageTime > 20.0f ){
			index = rand()%2;
			m_fStageTime = 0.0f;
			++m_iIsCreateMob;
			switch(m_iState){
				case GS_STAGE1:
					{
						m_pObjMgr->CreateAddtionalMob(index,0);
					}
					break;
				case GS_STAGE2:
					{
						m_pObjMgr->CreateAddtionalMob(index,1);
					}
					break;
				case GS_STAGE3:
					{
						m_pObjMgr->CreateAddtionalMob(index,2);
					}
					break;
			}	
			if(index){
				filename = _T("ali_plus.png");
			}else{
				filename = _T("ufo_plus.png");
			}
			m_pGuiMgr->CreateTwinkleSprite(filename,pt,D3DXVECTOR3(100,155,0),O2Rect(0,66,0,55),3.0f,0.5f);
		}
	}else{
		if( 30.0f < m_fStageTime ){
			index = rand()%2;
			m_fStageTime = 0.0f;
			m_iIsCreateMob = 1;
			switch(m_iState){
				case GS_STAGE1:
					{
						m_pObjMgr->CreateAddtionalMob(index,0);
					}
					break;
				case GS_STAGE2:
					{
						m_pObjMgr->CreateAddtionalMob(index,1);
					}
					break;
				case GS_STAGE3:
					{
						m_pObjMgr->CreateAddtionalMob(index,2);
					}
					break;
			}
			if(index){
				filename = _T("ali_plus.png");
			}else{
				filename = _T("ufo_plus.png");
			}
			m_pGuiMgr->CreateTwinkleSprite(filename,pt,D3DXVECTOR3(100,155,0),O2Rect(0,66,0,55),3.0f,0.5f);
		}
	}
}

HRESULT Manager::CreateWarnningFromFile( FILE** pFile )
{
	TCHAR Data[256];
	TCHAR* _val;
	TCHAR* _flag;
	TCHAR _file[MAX_OBJNAME] = _T("NONE");
	TCHAR _fileOn1[MAX_OBJNAME] = _T("NONE");
	TCHAR _fileOn2[MAX_OBJNAME] = _T("NONE");
	D3DXVECTOR3 _pos(0,0,0);
	O2Rect _rect(0,0,0,0);
	POINT ImgSize = {0,0};
	float _mtime = 0.0f;
	float _ttime = 0.0f;

	UINT len;

	while(_fgetts(Data,256,(*pFile))){
		len = _tcslen(Data);
		Data[len-1] = NULL;
		if( 0 == _tcscmp(Data,_T("[END]"))){
			break;
		}
		_flag = _tcstok_s(Data,_T("="),&_val);
		if(_val != NULL){
			_val = _tcstok_s(_val,_T(";"),&_val);
			if(NULL == _val){
				continue;
			}
			if( 0 == _tcscmp(_flag,_T("FileName"))){
				GetTcsFromData(_val,_file);
			}else if( 0 == _tcscmp(_flag,_T("OnFileName1"))){
				GetTcsFromData(_val,_fileOn1);
			}else if( 0 == _tcscmp(_flag,_T("OnFileName2"))){
				GetTcsFromData(_val,_fileOn2);			
			}else if( 0 == _tcscmp(_flag,_T("Pos"))){
				GetVec3FromData(_val,_pos);			
			}else if( 0 == _tcscmp(_flag,_T("ImgSize"))){
				GetPtFromData(_val,ImgSize);
			}else if( 0 == _tcscmp(_flag,_T("ImgRect"))){
				GetRectFromData(_val,_rect);
			}else if( 0 == _tcscmp(_flag,_T("Time"))){
				GetFloatFromData(_val,_mtime);
			}else if( 0 == _tcscmp(_flag,_T("ChangeTime"))){
				GetFloatFromData(_val,_ttime);
			}else{
				_DEBUG_LOG(_T("Error - Readflag value [ %s ]\n"),_flag);
			}
		}
	}

	return m_pGuiMgr->CreateTwinkleSprite(_file,_fileOn1,_fileOn2,ImgSize,_pos,_rect,_mtime,_ttime);
}

HRESULT Manager::Fm_Ending()
{
	ChangeState(GS_MAIN);

	return S_OK;
}