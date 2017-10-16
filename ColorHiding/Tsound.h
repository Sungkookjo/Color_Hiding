#pragma once

#define _LoopSound DSBPLAY_LOOPING

class O2TSound
{
private:

	LPDIRECTSOUND3DBUFFER		 m_3DSoundDSB;		//3D 사운드
	LPDIRECTSOUND3DLISTENER		 m_SoundListener;	//사운드 리스너


	DWORD	m_dwFlag;				//플레이 플래그
	BOOL	m_bPause;

	SNDOBJ*		 m_CSound;				//사운드 오브젝트
	O2SoundUtil* m_Soundutil;		//사운드 유틸 포인터

	// 재생 좌표
	D3DXVECTOR3		m_vPos;
	D3DXVECTOR3*	m_vpPos;
	// 듣는 좌표
	D3DXVECTOR3*	m_vListenerPos;


public:

	O2TSound();
	~O2TSound();
	void DestroyS();

//	사운드 생성
	HRESULT CreateSound(LPDIRECTSOUND _Sound,LPDIRECTSOUNDBUFFER _SoundDSB,
		LPDIRECTSOUND3DLISTENER _3DSoundListener,
		TCHAR* _Sname,UINT _Soundtype,BOOL _3Dtype);

	BOOL Update();

//	사운드 재생,정지,일시정지
	BOOL Play_Sound();
	BOOL Stop_Sound();
	BOOL Pause_Sound();
	BOOL IsPlay_Sound();
	BOOL IsPause() { return m_bPause; }


//	사운드객체 설정
	void SetSoundPos(D3DXVECTOR3 _SoundPos)			{ m_vPos = _SoundPos; m_vpPos=&m_vPos;}				//음원 위치
	void SetSoundPos(D3DXVECTOR3* _SoundPos)		{ m_vpPos = _SoundPos; }						
	void SetListenerPos(D3DXVECTOR3* _ListenerPos)  { m_vListenerPos = _ListenerPos; }	//리스너 위치
	void SetFlags(DWORD _flag)						{ m_dwFlag = _flag; }				//재생 플래그
	void SetSoundVolume(long p_Volum);													//볼륨

	//	속성 얻기
	DWORD GetFlags() { return m_dwFlag; }
	D3DXVECTOR3* GetSoundPos() { return &m_vPos; }

};