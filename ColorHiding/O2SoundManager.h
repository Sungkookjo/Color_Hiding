#pragma once

//		사운드 타입(배경,이펙트)
#define BGMSOUND 1
#define EFFECTSOUND 2
//		사운드 타입(2D , 3D)
#define THREEDIMENSION TRUE
#define TWODIMENSION FALSE

#define O2SOUNDINSTANCE O2SoundManager::GetInstance()

class O2SoundManager
{
public:
	enum S_EFFECT_TYPE
	{
		S_TYPE_INGAME= 0,
		S_TYPE_MENU = 1,
		S_TYPE_NAMED = 2
	};

	enum S_BGM_ID
	{
		S_BGM_ID_OPENING= 0,
		S_BGM_ID_HELP = 1,
		S_BGM_ID_STAGE01 = 2,
		S_BGM_ID_STAGE02 = 3,
		S_BGM_ID_STAGE03 = 4,
		S_BGM_ID_VOID=5
	};
private:
	D3DXVECTOR3 m_vListenPos;						//리스너 위치	
	long m_lEffectVolum;							//이펙트 볼륨
	long m_lBGMVolum;								//BGM 볼륨
	BOOL m_bSoundOnOff;								//On/Off 상태

	O2TSound* m_SoundBGM;							//BGM 객체  
	UINT	  m_IDBGM;								//BGM ID
	TCHAR*	  m_pcBGMname[S_BGM_ID_VOID];			//BGM ID 이름저장
	O2SoundUtil* m_SoundUtil;						//사운드 유틸

	std::vector<O2TSound*> m_MenuSoundlist;			//메뉴(버튼같은거) 리스트
	std::vector<O2TSound*> m_InGamelist;			//게임내(적소리같은거) 리스트
	std::map<TCHAR* , O2TSound*> m_Namedlist;		//네임드 사운드(LoopSound인데 원하는때에 없애고싶을때) 리스트

	LPDIRECTSOUND			     m_Sound;			//다이렉트 사운드
	LPDIRECTSOUNDBUFFER			 m_SoundDSB;		//1차 사운드버퍼
	LPDIRECTSOUND3DLISTENER		 m_3DSoundListener;	//리스너
	DSBUFFERDESC				 m_DSBdesc;			//버퍼속성
public:

	O2SoundManager();
	~O2SoundManager();

	static O2SoundManager* GetInstance();

	void DestroyAll();
	void DestroyEffect();
	void DestroyBGM();

	HRESULT Init(HWND p_hWnd);
	void UpdateSound();

	//	리스너 설정
	void SetListen(D3DXVECTOR3* _Listen) { m_vListenPos=(*_Listen); }

	//	사운드 볼륨설정
	void SetEffectVolum(long p_Volum);
	void SetBGMVolum(long p_Volum);

	//	사운드 추가
	HRESULT AddBGM(TCHAR* _Sname);
	HRESULT AddEffect(TCHAR* _index,TCHAR* _Sname,D3DXVECTOR3 _Pos,BOOL _Dimension=TRUE);
	void	PushEffect(O2TSound* _Sound,S_EFFECT_TYPE _Sub,TCHAR* _Sname);

	//	사운드 정지
	void StopEffect(S_EFFECT_TYPE _subject,TCHAR* _ObjName=NULL);
	void StopEffect();
	void StopBGM();
	void StopAll();

	//	사운드 일시정지
	void PauseEffect(S_EFFECT_TYPE _subject,TCHAR* _ObjName=NULL);
	void PauseEffect();
	void PauseBGM();
	void PauseAll();

	//	사운드 플레이
	HRESULT PlayAll();
	HRESULT PlayEffect();
	HRESULT PlayBGM();					// 기존의 BGM 플레이
	HRESULT PlayBGM(S_BGM_ID _BGMID);	// ID로 플레이 
	//	새로 생성해서 플레이
	HRESULT PlayBGM(TCHAR* _Sname);		//	기존의 BGM 삭제하고 플레이
	HRESULT PlayEffect(S_EFFECT_TYPE _Sub);
	HRESULT Play2DEffect(TCHAR* _Sname,S_EFFECT_TYPE _Sub=S_TYPE_INGAME,DWORD _flag=NULL,TCHAR* _Objname=NULL);	//2D
	HRESULT Play3DEffect(TCHAR* _Sname,S_EFFECT_TYPE _Sub=S_TYPE_INGAME,DWORD _flag=NULL		//3D
		,D3DXVECTOR3 _Pos=D3DXVECTOR3(0,0,0),D3DXVECTOR3* _pPos=NULL,TCHAR* _Objname=NULL);

	//	사운드 on/off
	void SoundOnOff();

};