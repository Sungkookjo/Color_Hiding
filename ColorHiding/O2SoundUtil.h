
#pragma once


typedef struct
{
	BYTE *pbWaveData;               // pointer into wave resource (for restore)
	DWORD cbWaveSize;               // size of wave data (for restore)
	int iAlloc;                     // number of buffers.
	int iCurrent;                   // current buffer
	IDirectSoundBuffer* Buffers[10]; // list of buffers

} SNDOBJ, *HSNDOBJ;

class O2SoundUtil
{
	public:

		O2SoundUtil();
		~O2SoundUtil();
	

//	���� ���� �ε�
	IDirectSoundBuffer *DSLoadSoundBuffer(IDirectSound *pDS, LPCTSTR lpName);
//	3D���� ���� �ε�
	IDirectSoundBuffer *DSLoad3DSoundBuffer(IDirectSound *pDS, LPCTSTR lpName);

//	������� ��ε�
	BOOL DSReloadSoundBuffer(IDirectSoundBuffer *pDSB, LPCTSTR lpName);


//	���̺� �Ӽ� �о����
	BOOL DSGetWaveResource(HMODULE hModule, LPCTSTR lpName,
		WAVEFORMATEX **ppWaveHeader, BYTE **ppbWaveData, DWORD *pdwWaveSize);

	BOOL DSGetWaveFile(HMODULE hModule, LPCTSTR lpName, WAVEFORMATEX **ppWaveHeader,
		BYTE **ppbWaveData, DWORD *pdwWaveSize, void **ppvBase);


//	���尴ü ����
	HSNDOBJ SndObjCreate(IDirectSound *pDS, LPCTSTR lpName, int iConcurrent,BOOL _type=FALSE);

//	���尴ü �ı�
	void SndObjDestroy(HSNDOBJ hSO);

	BOOL SndObjPlay(HSNDOBJ hSO, DWORD dwPlayFlags);

	BOOL SndObjIsPlay(HSNDOBJ hSO, DWORD dwPlayFlags);

	BOOL SndObjStop(HSNDOBJ hSO);

	BOOL SndObjPause(HSNDOBJ hSO);

//	�������� ���
	IDirectSoundBuffer *SndObjGetFreeBuffer(HSNDOBJ hSO);
//	���� ä���
	BOOL DSFillSoundBuffer(IDirectSoundBuffer *pDSB, BYTE *pbWaveData, DWORD dwWaveSize);
//	���̺� �Ľ�
	BOOL DSParseWaveResource(void *pvRes, WAVEFORMATEX **ppWaveHeader, BYTE **ppbWaveData, DWORD *pdwWaveSize);



};