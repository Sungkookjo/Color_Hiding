#pragma once


#ifndef CPARTICLESYSTEM_H_INCLUDED
#define CPARTICLESYSTEM_H_INCLUDED

class ResourceManager;

class O2ParticleSystem
{

protected:

	struct Plane
	{
		D3DXVECTOR3 m_vNormal;           // The plane's normal
		D3DXVECTOR3 m_vPoint;            // A coplanar point within the plane
		float       m_fBounceFactor;     // Coefficient of restitution (or how bouncy the plane is)
		int         m_nCollisionResult;  // What will particles do when they strike the plane

		Plane      *m_pNext;             // Next plane in list
	};

	struct Particle
	{
		D3DXVECTOR3 m_vCurPos;    // Current position of particle
		D3DXVECTOR3 m_vCurVel;    // Current velocity of particle
		float       m_fInitTime;  // Time of creation of particle
		D3DXCOLOR	m_cColor;	  // 파티클 색깔
		bool		m_bColorstatus;  // 색깔을 얻었는지 확인

		Particle   *m_pNext;      // Next particle in list
	};

	// Custom vertex and FVF declaration for point sprite vertex points
	struct PointVertex
	{
		D3DXVECTOR3 posit;
		D3DCOLOR    color;

		enum FVF
		{
			FVF_Flags = D3DFVF_XYZ | D3DFVF_DIFFUSE
		};
	};

	// Classify Point
	const int CP_FRONT   ;
	const int CP_BACK    ;
	const int CP_ONPLANE ;

	// Collision Results
	const int CR_BOUNCE  ;
	const int CR_STICK   ;
	const int CR_RECYCLE ;


	TCHAR*				 Ftname;		//	파일이름
	LPDIRECT3DDEVICE9 m_device;		

public:

	O2ParticleSystem(void);
	virtual ~O2ParticleSystem(void);

	static ResourceManager*		sp_pResource		;


///파티클 정보 세팅 및 얻기
	void SetMaxParticles( int dwMaxParticles ) { m_dwMaxParticles = dwMaxParticles; }
	int GetMaxParticles( void ) { return m_dwMaxParticles; }

	void SetNumToRelease( int dwNumToRelease ) { m_dwNumToRelease = dwNumToRelease; }
	int GetNumToRelease( void ) { return m_dwNumToRelease; }

	void SetReleaseInterval( float fReleaseInterval ) { m_fReleaseInterval = fReleaseInterval; }
	float GetReleaseInterval( void ) { return m_fReleaseInterval; }

	void SetLifeCycle( float fLifeCycle ) { m_fLifeCycle = fLifeCycle; }
	float GetLifeCycle( void ) { return m_fLifeCycle; }

	void SetLifeTime( float fLifetime ) { m_fLifeTime = fLifetime; }
	float GetLifeTime( void ) { return m_fLifeTime; }

	void SetSize( float fSize ) { m_fSize = fSize; }
	float GetSize( void ) { return m_fSize; }
	float GetMaxPointSize( void ) { return m_fMaxPointSize; }

	void SetColor( D3DXCOLOR clrColor ) { m_clrColor = clrColor; }
	D3DXCOLOR GetColor( void ) { return m_clrColor; }

	void SetPosition( D3DXVECTOR3 vPosition ) { m_vPosition = vPosition; }
	D3DXVECTOR3 GetPosition( void ) { return m_vPosition; }
	void SetTargetPosition( D3DXVECTOR3* vPosition ) { m_vTargetPosition = vPosition; }

	void SetVelocity( D3DXVECTOR3 vVelocity ) { m_vVelocity = vVelocity; }
	D3DXVECTOR3 GetVelocity( void ) { return m_vVelocity; }

	void SetGravity( D3DXVECTOR3 vGravity ) { m_vGravity = vGravity; }
	D3DXVECTOR3 GetGravity( void ) { return m_vGravity; }

	void SetWind( D3DXVECTOR3 vWind ) { m_vWind = vWind; }
	D3DXVECTOR3 GetWind( void ) { return m_vWind; }

	void SetAirResistence( bool bAirResistence ) { m_bAirResistence = bAirResistence; }
	bool GetAirResistence( void ) { return m_bAirResistence; }

	void SetVelocityVar( float fVelocityVar ) { m_fVelocityVar = fVelocityVar; }
	float GetVelocityVar( void ) { return m_fVelocityVar; }

	void SetCollisionPlane( D3DXVECTOR3 vPlaneNormal, D3DXVECTOR3 vPoint, 
		float fBounceFactor = 1.0f, int nCollisionResult = 0 );

	void SetRndcolor(bool _rndcolor) { m_bRndColor = _rndcolor; }
	bool GetRndcolor() { return m_bRndColor; }

	void SetTwinkle(bool _twinkle) { m_btwinkle = _twinkle; }
	bool GetTwinkle() { return m_btwinkle; }

	void SetTexture(TCHAR* chTexFile ) ;
	void SetTexture( LPDIRECT3DTEXTURE9 _texParticle ) { m_ptexParticle = _texParticle ; }
	LPDIRECT3DTEXTURE9 &GetTextureObject();

	void SetFtName(TCHAR* _fname) { Ftname = _fname; }

/////float에서 dword , min~max 랜덤값, 랜덤vector
	inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }
	float getRandomMinMax( float fMin, float fMax );
	D3DXVECTOR3 getRandomVector( void );


	HRESULT Init( LPDIRECT3DDEVICE9 pd3dDevice );	//초기화 
//	업데이트 ( 위치값 필요없을듯 )
	HRESULT Update( float fElapsedTime , D3DXVECTOR3 _vTarget,D3DXMATRIXA16* _Mat,D3DXQUATERNION* _Quat);
	HRESULT Render();	//렌더


	HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );	//버퍼 초기화

	void DestroyP();												//파티클 제거
	HRESULT InvalidateDeviceObjects(void);							//다쓴객체 없애기

	void RestartParticleSystem(void);								//파티클 다시시작


	int classifyPoint( D3DXVECTOR3 *vPoint, Plane *pPlane );		//plane 위치에따라 리턴

	virtual void Init_Particles(LPDIRECT3DDEVICE9 _device) {}		//파티클 초기화
	virtual void SetAttributes();									//파티클 정보 읽어오고 세팅
	virtual void Readtextfile();									//텍스트파일읽기
	virtual void SetValues(LPDIRECT3DDEVICE9 _device);				//파티클 값 세팅


	void SetTargetQuat(D3DXQUATERNION* _Quat) { m_qTargetQuat = _Quat; }
	void SetTargetMat(D3DXMATRIXA16* _Mat) { m_mTargetMat = _Mat; }
	void SetVarValue(D3DXVECTOR3 _Val) { m_vVarValue = _Val; }
	void SetTargetPos(D3DXVECTOR3* _Pos) { m_vTargetPosition = _Pos; }

	virtual void SetupWorld();										//렌더하기전에 월드설정
	virtual void SetupPosition();									//업데이트전에 위치설정
	virtual D3DXVECTOR3 GetcurPos(Particle* part);
	bool GetLifestatus();											//라이프타임 확인

private:

	DWORD       m_dwVBOffset;
	DWORD       m_dwFlush;
	DWORD       m_dwDiscard;
	Particle   *m_pActiveList;
	Particle   *m_pFreeList;
	Plane      *m_pPlanes;
	int			m_dwActiveCount;
	float       m_fCurrentTime;				//생성~지금까지 시간
	float       m_fLastUpdate;				//마지막으로 갱신된 시간

	float       m_fMaxPointSize;
	bool        m_bDeviceSupportsPSIZE;

	LPDIRECT3DVERTEXBUFFER9 m_pVB;          // Vertex buffer for point sprites
	LPDIRECT3DTEXTURE9      m_ptexParticle; // Particle's texture


	// Particle Attributes
	int       m_dwMaxParticles;				//전체 파티클 개수
	int       m_dwNumToRelease;				//한번에 나오는 개수
	float       m_fReleaseInterval;			//다시 뿌려지는 시간
	float       m_fLifeCycle;				//반복되는 시간
	float		m_fLifeTime;				//살아있는 시간
	float       m_fSize;					//사이즈
	D3DXCOLOR   m_clrColor;					//색깔
	D3DXVECTOR3 m_vPosition;				//위치
	D3DXVECTOR3 m_vVelocity;				//나오는 방향
	D3DXVECTOR3 m_vGravity;					//중력
	D3DXVECTOR3 m_vWind;					//바람
	bool        m_bAirResistence;			//바람저항
	float       m_fVelocityVar;				//나오는 힘
	TCHAR       *m_chTexFile;				//텍스처 이름저장
	bool		m_bRndColor;				//랜덤칼라
	bool		m_btwinkle;					//반짝이효과


	float		fTimePassed;				//경과시간
	float		m_fElapsedTime;				//델타타임
	TCHAR      m_tempTEXT[256];				//텍스처 이름 임시저장


protected:

	D3DXQUATERNION*	m_qTargetQuat;			//회전 포인터
	D3DXVECTOR3		m_vCurPosition;			//갱신 위치
	D3DXVECTOR3		m_vFirstPosition;		//초기위치
	D3DXVECTOR3		m_vVarValue;			//위치 조정값
	D3DXVECTOR3*	m_vTargetPosition;		//고정될 위치값
	D3DXMATRIXA16*  m_mTargetMat;			//행렬 포인터
};

#endif 