#pragma once

//		���� Ÿ��(���,����Ʈ)
#define BGMSOUND 1
#define EFFECTSOUND 2
//		���� Ÿ��(2D , 3D)
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
	D3DXVECTOR3 m_vListenPos;						//������ ��ġ	
	long m_lEffectVolum;							//����Ʈ ����
	long m_lBGMVolum;								//BGM ����
	BOOL m_bSoundOnOff;								//On/Off ����

	O2TSound* m_SoundBGM;							//BGM ��ü  
	UINT	  m_IDBGM;								//BGM ID
	TCHAR*	  m_pcBGMname[S_BGM_ID_VOID];			//BGM ID �̸�����
	O2SoundUtil* m_SoundUtil;						//���� ��ƿ

	std::vector<O2TSound*> m_MenuSoundlist;			//�޴�(��ư������) ����Ʈ
	std::vector<O2TSound*> m_InGamelist;			//���ӳ�(���Ҹ�������) ����Ʈ
	std::map<TCHAR* , O2TSound*> m_Namedlist;		//���ӵ� ����(LoopSound�ε� ���ϴ¶��� ���ְ������) ����Ʈ

	LPDIRECTSOUND			     m_Sound;			//���̷�Ʈ ����
	LPDIRECTSOUNDBUFFER			 m_SoundDSB;		//1�� �������
	LPDIRECTSOUND3DLISTENER		 m_3DSoundListener;	//������
	DSBUFFERDESC				 m_DSBdesc;			//���ۼӼ�
public:

	O2SoundManager();
	~O2SoundManager();

	static O2SoundManager* GetInstance();

	void DestroyAll();
	void DestroyEffect();
	void DestroyBGM();

	HRESULT Init(HWND p_hWnd);
	void UpdateSound();

	//	������ ����
	void SetListen(D3DXVECTOR3* _Listen) { m_vListenPos=(*_Listen); }

	//	���� ��������
	void SetEffectVolum(long p_Volum);
	void SetBGMVolum(long p_Volum);

	//	���� �߰�
	HRESULT AddBGM(TCHAR* _Sname);
	HRESULT AddEffect(TCHAR* _index,TCHAR* _Sname,D3DXVECTOR3 _Pos,BOOL _Dimension=TRUE);
	void	PushEffect(O2TSound* _Sound,S_EFFECT_TYPE _Sub,TCHAR* _Sname);

	//	���� ����
	void StopEffect(S_EFFECT_TYPE _subject,TCHAR* _ObjName=NULL);
	void StopEffect();
	void StopBGM();
	void StopAll();

	//	���� �Ͻ�����
	void PauseEffect(S_EFFECT_TYPE _subject,TCHAR* _ObjName=NULL);
	void PauseEffect();
	void PauseBGM();
	void PauseAll();

	//	���� �÷���
	HRESULT PlayAll();
	HRESULT PlayEffect();
	HRESULT PlayBGM();					// ������ BGM �÷���
	HRESULT PlayBGM(S_BGM_ID _BGMID);	// ID�� �÷��� 
	//	���� �����ؼ� �÷���
	HRESULT PlayBGM(TCHAR* _Sname);		//	������ BGM �����ϰ� �÷���
	HRESULT PlayEffect(S_EFFECT_TYPE _Sub);
	HRESULT Play2DEffect(TCHAR* _Sname,S_EFFECT_TYPE _Sub=S_TYPE_INGAME,DWORD _flag=NULL,TCHAR* _Objname=NULL);	//2D
	HRESULT Play3DEffect(TCHAR* _Sname,S_EFFECT_TYPE _Sub=S_TYPE_INGAME,DWORD _flag=NULL		//3D
		,D3DXVECTOR3 _Pos=D3DXVECTOR3(0,0,0),D3DXVECTOR3* _pPos=NULL,TCHAR* _Objname=NULL);

	//	���� on/off
	void SoundOnOff();

};