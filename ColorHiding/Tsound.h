#pragma once

#define _LoopSound DSBPLAY_LOOPING

class O2TSound
{
private:

	LPDIRECTSOUND3DBUFFER		 m_3DSoundDSB;		//3D ����
	LPDIRECTSOUND3DLISTENER		 m_SoundListener;	//���� ������


	DWORD	m_dwFlag;				//�÷��� �÷���
	BOOL	m_bPause;

	SNDOBJ*		 m_CSound;				//���� ������Ʈ
	O2SoundUtil* m_Soundutil;		//���� ��ƿ ������

	// ��� ��ǥ
	D3DXVECTOR3		m_vPos;
	D3DXVECTOR3*	m_vpPos;
	// ��� ��ǥ
	D3DXVECTOR3*	m_vListenerPos;


public:

	O2TSound();
	~O2TSound();
	void DestroyS();

//	���� ����
	HRESULT CreateSound(LPDIRECTSOUND _Sound,LPDIRECTSOUNDBUFFER _SoundDSB,
		LPDIRECTSOUND3DLISTENER _3DSoundListener,
		TCHAR* _Sname,UINT _Soundtype,BOOL _3Dtype);

	BOOL Update();

//	���� ���,����,�Ͻ�����
	BOOL Play_Sound();
	BOOL Stop_Sound();
	BOOL Pause_Sound();
	BOOL IsPlay_Sound();
	BOOL IsPause() { return m_bPause; }


//	���尴ü ����
	void SetSoundPos(D3DXVECTOR3 _SoundPos)			{ m_vPos = _SoundPos; m_vpPos=&m_vPos;}				//���� ��ġ
	void SetSoundPos(D3DXVECTOR3* _SoundPos)		{ m_vpPos = _SoundPos; }						
	void SetListenerPos(D3DXVECTOR3* _ListenerPos)  { m_vListenerPos = _ListenerPos; }	//������ ��ġ
	void SetFlags(DWORD _flag)						{ m_dwFlag = _flag; }				//��� �÷���
	void SetSoundVolume(long p_Volum);													//����

	//	�Ӽ� ���
	DWORD GetFlags() { return m_dwFlag; }
	D3DXVECTOR3* GetSoundPos() { return &m_vPos; }

};