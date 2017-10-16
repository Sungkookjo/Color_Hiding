#include "stdafx.h"
#include "O2SoundManager.h"

O2SoundManager::O2SoundManager()
:
m_SoundUtil(NULL),
m_SoundBGM(NULL),
m_Sound(NULL),
m_SoundDSB(NULL),
m_3DSoundListener(NULL),
m_vListenPos(0,0,0),
m_lEffectVolum(0),
m_lBGMVolum(0),
m_bSoundOnOff(TRUE),
m_IDBGM(S_BGM_ID_VOID)
{
	for(int i=0;i<S_BGM_ID_VOID;i++)
	{
		m_pcBGMname[i]=NULL;
	}
}

O2SoundManager::~O2SoundManager()
{
	DestroyAll();
	std::vector<O2TSound*>::iterator _it=m_MenuSoundlist.begin();
	int _end = m_MenuSoundlist.size();

	for(int i=0;i<_end;i++,_it++)
	{
		SAFE_DELETE(*_it);
	}
	m_MenuSoundlist.clear();
}


void O2SoundManager::DestroyAll()
{
	DestroyEffect();
	DestroyBGM();
}

void O2SoundManager::DestroyBGM()
{
	SAFE_DELETE(m_SoundBGM);
}

void O2SoundManager::DestroyEffect()
{
	std::vector<O2TSound*>::iterator _it=m_InGamelist.begin();
	int _end = m_InGamelist.size();

	for(int i=0;i<_end;i++,_it++)
	{
		SAFE_DELETE(*_it);
	}
	m_InGamelist.clear();

	std::map<TCHAR* , O2TSound*>::iterator mit= m_Namedlist.begin();
	_end = m_Namedlist.size();

	for(int i=0;i<_end;i++,mit++)
	{
		SAFE_DELETE(mit->second);
	}
	m_Namedlist.clear();


}

HRESULT O2SoundManager::Init(HWND p_hWnd)
{

//	다이렉트사운드 얻기
	if ( DirectSoundCreate(NULL,&m_Sound,NULL) != DS_OK ) return E_FAIL;
	
	if (m_Sound->SetCooperativeLevel(p_hWnd,DSSCL_PRIORITY)!=DS_OK) return E_FAIL;
//	버퍼속성 설정
	memset(&m_DSBdesc,0,sizeof(DSBUFFERDESC));
	m_DSBdesc.dwSize = sizeof(DSBUFFERDESC);
	m_DSBdesc.dwFlags = DSBCAPS_PRIMARYBUFFER|DSBCAPS_CTRL3D;

//	1차버퍼 생성
	if (m_Sound->CreateSoundBuffer(&m_DSBdesc,&m_SoundDSB,NULL)!=DS_OK) return E_FAIL;

	m_SoundDSB -> SetVolume(0);
	m_SoundDSB -> SetPan(0);

//	3D 리스너 얻기
	if(m_SoundDSB->QueryInterface(IID_IDirectSound3DListener,(void**)&m_3DSoundListener)!=S_OK)
		return E_FAIL;
	m_3DSoundListener->SetDistanceFactor(0.001f,DS3D_IMMEDIATE);
	m_3DSoundListener->SetPosition(m_vListenPos.x,m_vListenPos.y,m_vListenPos.z,DS3D_IMMEDIATE);
	
//	BGM ID 입력

	m_pcBGMname[S_BGM_ID_OPENING]=_T("Opening");
	m_pcBGMname[S_BGM_ID_HELP]=_T("Opening");
	m_pcBGMname[S_BGM_ID_STAGE01]=_T("Stage01BGM");
	m_pcBGMname[S_BGM_ID_STAGE02]=_T("Stage02BGM");
	m_pcBGMname[S_BGM_ID_STAGE03]=_T("Stage03BGM");


	return S_OK;

}

void O2SoundManager::UpdateSound()
{
	std::vector<O2TSound*>::iterator _it;
	_it = m_MenuSoundlist.begin();
	int _end = m_MenuSoundlist.size();
	for(int i=0;i<_end;i++)
	{
		if((*_it)->Update())
			_it++;
		else
		{
			//테스트부분
			printf("m_MenuSoundlist delete\n");
			SAFE_DELETE(*_it);
			_it=m_MenuSoundlist.erase(_it);
		}
	}

 	_it = m_InGamelist.begin();
 	_end = m_InGamelist.size();
 	for(int i=0;i<_end;i++)
 	{
		if((*_it)->Update())
			_it++;
		else
		{
			//테스트부분
			printf("m_InGamelist delete\n");
			SAFE_DELETE(*_it);
			_it=m_InGamelist.erase(_it);
		}
	}

	std::map<TCHAR* , O2TSound*>::iterator mit;
	mit = m_Namedlist.begin();
	_end = m_Namedlist.size();

	 for(int i=0;i<_end;i++/*,mit++*/)
 	{
		
		if((mit->second)->Update())
			mit++;
		else
		{
			//테스트부분
			printf("m_Namedlist delete\n");
			SAFE_DELETE(mit->second);
			mit=m_Namedlist.erase(mit);
		}
	}

}

void O2SoundManager::SetEffectVolum(long p_Volum)
{
	p_Volum = ( p_Volum > DSBVOLUME_MAX ? DSBVOLUME_MAX : ( p_Volum < DSBVOLUME_MIN ? DSBVOLUME_MIN : p_Volum ));
	m_lEffectVolum = p_Volum;

	std::vector<O2TSound*>::iterator _it=m_MenuSoundlist.begin();
	int _end = m_MenuSoundlist.size();

	for(int i=0;i<_end;i++,_it++)
	{
		(*_it)->SetSoundVolume(p_Volum);
	}

	_it = m_InGamelist.begin();
	_end = m_InGamelist.size();

	for(int i=0;i<_end;i++,_it++)
	{
		(*_it)->SetSoundVolume(p_Volum);
	}

	std::map<TCHAR* , O2TSound*>::iterator mit= m_Namedlist.begin();
	_end = m_Namedlist.size();

	for(int i=0;i<_end;i++,mit++)
	{
		(mit->second)->SetSoundVolume(p_Volum);
	}
}

void O2SoundManager::SetBGMVolum(long p_Volum)
{
	p_Volum = ( p_Volum > DSBVOLUME_MAX ? DSBVOLUME_MAX : ( p_Volum < DSBVOLUME_MIN ? DSBVOLUME_MIN : p_Volum ));
	m_lBGMVolum = p_Volum;
	if(m_SoundBGM)
		m_SoundBGM->SetSoundVolume(p_Volum);
}




void O2SoundManager::StopEffect(S_EFFECT_TYPE _subject,TCHAR* _ObjName/*=NULL*/)
{
	if(_subject==S_TYPE_INGAME)
	{
		std::vector<O2TSound*>::iterator _it=m_InGamelist.begin();
		int _end=m_InGamelist.size();
		for(int i=0;i<_end;i++,_it++)
		{
			(*_it)->Stop_Sound();
		}
	}else if((_subject==S_TYPE_NAMED)&&(_ObjName!=NULL))
	{
		if(m_Namedlist[_ObjName])
			m_Namedlist[_ObjName]->Stop_Sound();
		else
			m_Namedlist.erase(_ObjName);
	}else if(_subject==S_TYPE_NAMED)
	{
		std::map<TCHAR* , O2TSound*>::iterator _it=m_Namedlist.begin();
		int _end=m_Namedlist.size();

		for(int i=0;i<_end;i++,_it++)
		{
			(_it->second)->Stop_Sound();
		}
	}
}

void O2SoundManager::StopEffect()
{
	std::vector<O2TSound*>::iterator _it;
	_it = m_MenuSoundlist.begin();
	int _end = m_MenuSoundlist.size();
	for(int i=0;i<_end;i++,_it++)
	{
		(*_it)->Stop_Sound();
	}

 	_it = m_InGamelist.begin();
 	_end = m_InGamelist.size();
 	for(int i=0;i<_end;i++,_it++)
 	{		
		(*_it)->Stop_Sound();
	}

	std::map<TCHAR* , O2TSound*>::iterator mit;
	mit = m_Namedlist.begin();
	_end = m_Namedlist.size();

	for(int i=0;i<_end;i++,mit++)
 	{
		(mit->second)->Stop_Sound();
	}
}


void O2SoundManager::StopBGM()
{
	if(!m_SoundBGM)
		printf("Stop : Cannot Found Sound Source BGM\n");
	else
		m_SoundBGM->Stop_Sound();
}


void O2SoundManager::StopAll()
{
	StopEffect();
	StopBGM();
}

void O2SoundManager::PauseEffect(S_EFFECT_TYPE _subject,TCHAR* _ObjName/*=NULL*/)
{
	if(_subject==S_TYPE_INGAME)
	{
		//테스트부분
		printf("\nINgame");
		std::vector<O2TSound*>::iterator _it=m_InGamelist.begin();
		int _end=m_InGamelist.size();
		for(int i=0;i<_end;i++,_it++)
		{
			(*_it)->Pause_Sound();
		}
	}else if((_subject==S_TYPE_NAMED)&&(_ObjName!=NULL))
	{
		printf("\nNAmed");
		m_Namedlist[_ObjName]->Pause_Sound();
	}else if(_subject==S_TYPE_NAMED)
	{
		printf("\nNAmed");
		std::map<TCHAR* , O2TSound*>::iterator _it=m_Namedlist.begin();
		int _end=m_Namedlist.size();

		for(int i=0;i<_end;i++,_it++)
		{
			(_it->second)->Pause_Sound();
		}
	}
}

void O2SoundManager::PauseEffect()
{
	std::vector<O2TSound*>::iterator _it;
	_it = m_MenuSoundlist.begin();
	int _end = m_MenuSoundlist.size();
	for(int i=0;i<_end;i++,_it++)
	{
		(*_it)->Pause_Sound();
	}

 	_it = m_InGamelist.begin();
 	_end = m_InGamelist.size();
 	for(int i=0;i<_end;i++,_it++)
 	{		
		(*_it)->Pause_Sound();
	}

	std::map<TCHAR* , O2TSound*>::iterator mit;
	mit = m_Namedlist.begin();
	_end = m_Namedlist.size();

	for(int i=0;i<_end;i++,mit++)
 	{
		(mit->second)->Pause_Sound();
	}
}

void O2SoundManager::PauseBGM()
{
	if(!m_SoundBGM)
		printf("Pause : Cannot Found Sound Source BGM\n");
	else
		m_SoundBGM->Pause_Sound();
}



void O2SoundManager::PauseAll()
{
	PauseEffect();
	PauseBGM();
}

void O2SoundManager::PushEffect(O2TSound* _Sound,S_EFFECT_TYPE _Sub,TCHAR* _Sname)
{
	switch (_Sub)
	{
	case S_TYPE_INGAME:
		m_InGamelist.push_back(_Sound);
		break;
	case S_TYPE_MENU:
		m_MenuSoundlist.push_back(_Sound);
		break;
	case S_TYPE_NAMED:
		SAFE_DELETE(m_Namedlist[_Sname]);
		m_Namedlist.erase(_Sname);
		m_Namedlist[_Sname]=_Sound;
	default:
		break;
	}
}

HRESULT O2SoundManager::PlayEffect()
{
	std::vector<O2TSound*>::iterator _it;
	_it = m_MenuSoundlist.begin();
	int _end = m_MenuSoundlist.size();
	for(int i=0;i<_end;i++,_it++)
	{
		(*_it)->Play_Sound();
	}

 	_it = m_InGamelist.begin();
 	_end = m_InGamelist.size();
 	for(int i=0;i<_end;i++,_it++)
 	{		
		(*_it)->Play_Sound();
	}

	std::map<TCHAR* , O2TSound*>::iterator mit;
	mit = m_Namedlist.begin();
	_end = m_Namedlist.size();

	for(int i=0;i<_end;i++,mit++)
 	{
		(mit->second)->Play_Sound();
	}

	return S_OK;
}

HRESULT O2SoundManager::PlayEffect(S_EFFECT_TYPE _Sub)
{
	std::vector<O2TSound*>::iterator _it;
	std::map<TCHAR* , O2TSound*>::iterator _mit;
	int _end,_mend;

	switch (_Sub)
	{
	case S_TYPE_INGAME:
		_it = m_InGamelist.begin();
		_end = m_InGamelist.size();
		for(int i=0;i<_end;i++,_it++)
		{
			(*_it)->Play_Sound();
		}
		break;
	case S_TYPE_MENU:
		_it = m_MenuSoundlist.begin();
		_end = m_MenuSoundlist.size();
		for(int i=0;i<_end;i++,_it++)
		{
			(*_it)->Play_Sound();
		}
		break;
	case S_TYPE_NAMED:
		_mit = m_Namedlist.begin();
		_mend = m_Namedlist.size();
		for(int i=0;i<_mend;i++,_mit++)
 		{
			(_mit->second)->Play_Sound();
		}
		break;
	default:
		break;
	}

	return S_OK;
}


HRESULT O2SoundManager::Play2DEffect(TCHAR *_Sname,S_EFFECT_TYPE _Sub,DWORD _flag,TCHAR* _Objname)
{
	O2TSound* _NewEffectSound = new O2TSound;
	TCHAR Sname[32];
	_tcscpy(Sname,_Sname);
	_tcscat(Sname,_T(".wav"));
	if(FAILED(_NewEffectSound->CreateSound(m_Sound,m_SoundDSB,m_3DSoundListener,Sname,EFFECTSOUND,FALSE)))
	{
		TCHAR fname[32];
		_tcscpy(fname,_T("sound/"));
		_tcscat(fname,Sname);
		if(FAILED(_NewEffectSound->CreateSound(m_Sound,m_SoundDSB,m_3DSoundListener,fname,EFFECTSOUND,FALSE)))
		{
			SAFE_DELETE(_NewEffectSound);
			printf("Failed Create Effect Sound %S\n",_Sname);
			return E_FAIL;
		}
	}
		
	_NewEffectSound->SetFlags(_flag);
	_NewEffectSound->SetSoundVolume( ( (m_bSoundOnOff==FALSE)? DSBVOLUME_MIN : m_lEffectVolum ) );
//	테스트 부분
	printf("%S Effect  ",Sname);
	_NewEffectSound->Play_Sound();
	PushEffect(_NewEffectSound,_Sub,_Objname);

	return S_OK;

}


HRESULT O2SoundManager::Play3DEffect(TCHAR* _Sname,S_EFFECT_TYPE _Sub/*=S_TYPE_INGAME*/,DWORD _flag/*=NULL*/
								   ,D3DXVECTOR3 _Pos/*(0,0,0)*/,D3DXVECTOR3* _pPos/*=NULL*/,TCHAR* _Objname)
{

	O2TSound* _NewEffectSound = new O2TSound;
	TCHAR Sname[32];
	_tcscpy(Sname,_Sname);
	_tcscat(Sname,_T(".wav"));
	if(FAILED(_NewEffectSound->CreateSound(m_Sound,m_SoundDSB,m_3DSoundListener,Sname,EFFECTSOUND,TRUE)))
	{
		TCHAR fname[32];
		_tcscpy(fname,_T("sound/"));
		_tcscat(fname,Sname);
		if(FAILED(_NewEffectSound->CreateSound(m_Sound,m_SoundDSB,m_3DSoundListener,fname,EFFECTSOUND,TRUE)))
		{
			SAFE_DELETE(_NewEffectSound);
			printf("Failed Create Effect Sound %S\n",_Sname);
			return E_FAIL;
		}
	}
	_NewEffectSound->SetFlags(_flag);
	_NewEffectSound->SetListenerPos(&m_vListenPos);
	_NewEffectSound->SetSoundVolume( ( (m_bSoundOnOff==FALSE)? DSBVOLUME_MIN : m_lEffectVolum ) );
	if(_pPos==NULL)
		_NewEffectSound->SetSoundPos(_Pos);
	else
		_NewEffectSound->SetSoundPos(_pPos);
//	테스트 부분
	printf("%S Effect  ",Sname);
	_NewEffectSound->Play_Sound();
	PushEffect(_NewEffectSound,_Sub,_Objname);

	return S_OK;
}

HRESULT O2SoundManager::PlayBGM()
{
	if(!m_SoundBGM)
	{
		printf("Cannot Play BGM Sound \n");
		return E_FAIL;
	}else
	{
		//	테스트 부분
		printf("BGM  ");
		m_SoundBGM->Play_Sound();
	}

	return S_OK;
}

HRESULT O2SoundManager::PlayBGM(TCHAR* _Sname)
{
	if(m_SoundBGM)
		SAFE_DELETE(m_SoundBGM);

	m_SoundBGM=new O2TSound;
	TCHAR Sname[32];
	_tcscpy(Sname,_Sname);
	_tcscat(Sname,_T(".wav"));
	if(FAILED(m_SoundBGM->CreateSound(m_Sound,m_SoundDSB,m_3DSoundListener,Sname,BGMSOUND,TWODIMENSION)))
	{
		TCHAR fname[32];
		_tcscpy(fname,_T("sound/"));
		_tcscat(fname,Sname);
		if(FAILED(m_SoundBGM->CreateSound(m_Sound,m_SoundDSB,m_3DSoundListener,fname,BGMSOUND,TWODIMENSION)))
		{
			SAFE_DELETE(m_SoundBGM);
			printf("Failed Create Sound %S",fname);
			return E_FAIL;
		}
	}
	m_SoundBGM->SetFlags(_LoopSound);
	//	테스트 부분
	printf("%S BGM  ",Sname);
	m_SoundBGM->Play_Sound();

	return S_OK;
}

HRESULT O2SoundManager::PlayBGM(S_BGM_ID _BGMID)
{
	if(_BGMID!=m_IDBGM)
	{	
		SAFE_DELETE(m_SoundBGM);
		m_IDBGM = _BGMID;
		m_SoundBGM=new O2TSound;

		TCHAR Sname[32];
		_tcscpy(Sname,m_pcBGMname[m_IDBGM]);
		_tcscat(Sname,_T(".wav"));

		if(FAILED(m_SoundBGM->CreateSound(m_Sound,m_SoundDSB,m_3DSoundListener,Sname,BGMSOUND,TWODIMENSION)))
		{
			TCHAR fname[32];
			_tcscpy(fname,_T("sound/"));
			_tcscat(fname,Sname);
			if(FAILED(m_SoundBGM->CreateSound(m_Sound,m_SoundDSB,m_3DSoundListener,fname,BGMSOUND,TWODIMENSION)))
			{
				SAFE_DELETE(m_SoundBGM);
				printf("Failed Create Sound %S",fname);
				return E_FAIL;
			}
		}
	
		m_SoundBGM->SetFlags(_LoopSound);
		//	테스트 부분
		printf("%S BGM  ",Sname);
		m_SoundBGM->Play_Sound();
	}

	return S_OK;
}

HRESULT O2SoundManager::PlayAll()
{
	PlayEffect();
	PlayBGM();
	return S_OK;
}

void O2SoundManager::SoundOnOff()
{
	if(m_bSoundOnOff)
	{
		m_bSoundOnOff=FALSE;
		long lEffect=m_lEffectVolum;
		long lBGM=m_lBGMVolum;
		SetEffectVolum(DSBVOLUME_MIN);
		SetBGMVolum(DSBVOLUME_MIN);
		m_lEffectVolum = lEffect;
		m_lBGMVolum    = lBGM;
	}else
	{
		m_bSoundOnOff=TRUE;
		SetEffectVolum(m_lEffectVolum);
		SetBGMVolum(m_lBGMVolum);
	}

}

O2SoundManager* O2SoundManager::GetInstance()
{
	static O2SoundManager* instance=NULL; 
	if(instance==NULL)
	{
		instance=new O2SoundManager;
	}
	return instance;
}