#include "stdafx.h"

O2TSound::O2TSound()
:m_3DSoundDSB(NULL),
m_SoundListener(NULL),
m_Soundutil(NULL),
m_CSound(NULL),
m_dwFlag(NULL),
m_vListenerPos(NULL),
m_bPause(FALSE)
{
	m_vPos=D3DXVECTOR3(0,0,0);
	m_vpPos=&m_vPos;
}

O2TSound::~O2TSound()
{
	DestroyS();
}

void O2TSound::DestroyS()
{
	SAFE_RELEASE(m_3DSoundDSB);
	if(m_CSound)
	{
		m_Soundutil->SndObjStop(m_CSound);
		m_Soundutil->SndObjDestroy(m_CSound);
	}
	m_CSound=NULL;
}



HRESULT O2TSound::CreateSound(LPDIRECTSOUND _Sound,LPDIRECTSOUNDBUFFER _SoundDSB,LPDIRECTSOUND3DLISTENER _3DSoundListener,TCHAR* _Sname,UINT _Soundtype,BOOL _3Dtype)
{
	m_CSound = m_Soundutil->SndObjCreate(_Sound,_Sname,_Soundtype,_3Dtype);
	if(m_CSound==NULL)
		return E_FAIL;
	if(_3Dtype)
	{
		if(m_CSound->Buffers[0]->QueryInterface(IID_IDirectSound3DBuffer,(void**)&m_3DSoundDSB)!=S_OK)
			return E_FAIL;

		m_3DSoundDSB->SetMaxDistance(500.0f,DS3D_IMMEDIATE);
		m_3DSoundDSB->SetMinDistance(10.0f,DS3D_IMMEDIATE);
		m_SoundListener = _3DSoundListener;
	}
	return S_OK;
}

BOOL O2TSound::Play_Sound()
{
	//테스트 부분
	printf("Play Sound\n");
	m_bPause = FALSE;
	return (m_Soundutil->SndObjPlay( m_CSound, m_dwFlag ));
	
}

BOOL O2TSound::Stop_Sound()
{
	//테스트 부분
	printf("Stop Sound\n");
	return(m_Soundutil->SndObjStop(m_CSound));
}

BOOL O2TSound::Pause_Sound()
{
	//테스트 부분
	printf("Pause Sound\n");
	m_bPause = TRUE;
	return(m_Soundutil->SndObjPause(m_CSound));
}

BOOL O2TSound::IsPlay_Sound()
{
	return (m_Soundutil->SndObjIsPlay(m_CSound,m_dwFlag));
}

BOOL O2TSound::Update()
{
	BOOL result;
	result=IsPlay_Sound();

	if((m_3DSoundDSB!=NULL)&&result)
	{
		m_3DSoundDSB->SetPosition( m_vpPos->x , m_vpPos->y , m_vpPos->z , DS3D_IMMEDIATE );
		m_SoundListener->SetPosition(m_vListenerPos->x,m_vListenerPos->y,m_vListenerPos->z,DS3D_IMMEDIATE);
	}else if(m_bPause==FALSE&&!result)
		result = FALSE;
	else
		result = TRUE;

	return result;
}

void O2TSound::SetSoundVolume(long p_Volum)
{
	if(m_CSound)
		m_CSound->Buffers[0]->SetVolume(p_Volum);
}
