
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
	

//	사운드 버퍼 로드
	IDirectSoundBuffer *DSLoadSoundBuffer(IDirectSound *pDS, LPCTSTR lpName);
//	3D사운드 버퍼 로드
	IDirectSoundBuffer *DSLoad3DSoundBuffer(IDirectSound *pDS, LPCTSTR lpName);

//	사운드버퍼 재로드
	BOOL DSReloadSoundBuffer(IDirectSoundBuffer *pDSB, LPCTSTR lpName);


//	웨이브 속성 읽어오기
	BOOL DSGetWaveResource(HMODULE hModule, LPCTSTR lpName,
		WAVEFORMATEX **ppWaveHeader, BYTE **ppbWaveData, DWORD *pdwWaveSize);

	BOOL DSGetWaveFile(HMODULE hModule, LPCTSTR lpName, WAVEFORMATEX **ppWaveHeader,
		BYTE **ppbWaveData, DWORD *pdwWaveSize, void **ppvBase);


//	사운드객체 생성
	HSNDOBJ SndObjCreate(IDirectSound *pDS, LPCTSTR lpName, int iConcurrent,BOOL _type=FALSE);

//	사운드객체 파괴
	void SndObjDestroy(HSNDOBJ hSO);

	BOOL SndObjPlay(HSNDOBJ hSO, DWORD dwPlayFlags);

	BOOL SndObjIsPlay(HSNDOBJ hSO, DWORD dwPlayFlags);

	BOOL SndObjStop(HSNDOBJ hSO);

	BOOL SndObjPause(HSNDOBJ hSO);

//	남은버퍼 얻기
	IDirectSoundBuffer *SndObjGetFreeBuffer(HSNDOBJ hSO);
//	버퍼 채우기
	BOOL DSFillSoundBuffer(IDirectSoundBuffer *pDSB, BYTE *pbWaveData, DWORD dwWaveSize);
//	웨이브 파싱
	BOOL DSParseWaveResource(void *pvRes, WAVEFORMATEX **ppWaveHeader, BYTE **ppbWaveData, DWORD *pdwWaveSize);



};