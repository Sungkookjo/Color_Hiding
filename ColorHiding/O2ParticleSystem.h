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
		D3DXCOLOR	m_cColor;	  // ��ƼŬ ����
		bool		m_bColorstatus;  // ������ ������� Ȯ��

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


	TCHAR*				 Ftname;		//	�����̸�
	LPDIRECT3DDEVICE9 m_device;		

public:

	O2ParticleSystem(void);
	virtual ~O2ParticleSystem(void);

	static ResourceManager*		sp_pResource		;


///��ƼŬ ���� ���� �� ���
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

/////float���� dword , min~max ������, ����vector
	inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }
	float getRandomMinMax( float fMin, float fMax );
	D3DXVECTOR3 getRandomVector( void );


	HRESULT Init( LPDIRECT3DDEVICE9 pd3dDevice );	//�ʱ�ȭ 
//	������Ʈ ( ��ġ�� �ʿ������ )
	HRESULT Update( float fElapsedTime , D3DXVECTOR3 _vTarget,D3DXMATRIXA16* _Mat,D3DXQUATERNION* _Quat);
	HRESULT Render();	//����


	HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );	//���� �ʱ�ȭ

	void DestroyP();												//��ƼŬ ����
	HRESULT InvalidateDeviceObjects(void);							//�پ���ü ���ֱ�

	void RestartParticleSystem(void);								//��ƼŬ �ٽý���


	int classifyPoint( D3DXVECTOR3 *vPoint, Plane *pPlane );		//plane ��ġ������ ����

	virtual void Init_Particles(LPDIRECT3DDEVICE9 _device) {}		//��ƼŬ �ʱ�ȭ
	virtual void SetAttributes();									//��ƼŬ ���� �о���� ����
	virtual void Readtextfile();									//�ؽ�Ʈ�����б�
	virtual void SetValues(LPDIRECT3DDEVICE9 _device);				//��ƼŬ �� ����


	void SetTargetQuat(D3DXQUATERNION* _Quat) { m_qTargetQuat = _Quat; }
	void SetTargetMat(D3DXMATRIXA16* _Mat) { m_mTargetMat = _Mat; }
	void SetVarValue(D3DXVECTOR3 _Val) { m_vVarValue = _Val; }
	void SetTargetPos(D3DXVECTOR3* _Pos) { m_vTargetPosition = _Pos; }

	virtual void SetupWorld();										//�����ϱ����� ���弳��
	virtual void SetupPosition();									//������Ʈ���� ��ġ����
	virtual D3DXVECTOR3 GetcurPos(Particle* part);
	bool GetLifestatus();											//������Ÿ�� Ȯ��

private:

	DWORD       m_dwVBOffset;
	DWORD       m_dwFlush;
	DWORD       m_dwDiscard;
	Particle   *m_pActiveList;
	Particle   *m_pFreeList;
	Plane      *m_pPlanes;
	int			m_dwActiveCount;
	float       m_fCurrentTime;				//����~���ݱ��� �ð�
	float       m_fLastUpdate;				//���������� ���ŵ� �ð�

	float       m_fMaxPointSize;
	bool        m_bDeviceSupportsPSIZE;

	LPDIRECT3DVERTEXBUFFER9 m_pVB;          // Vertex buffer for point sprites
	LPDIRECT3DTEXTURE9      m_ptexParticle; // Particle's texture


	// Particle Attributes
	int       m_dwMaxParticles;				//��ü ��ƼŬ ����
	int       m_dwNumToRelease;				//�ѹ��� ������ ����
	float       m_fReleaseInterval;			//�ٽ� �ѷ����� �ð�
	float       m_fLifeCycle;				//�ݺ��Ǵ� �ð�
	float		m_fLifeTime;				//����ִ� �ð�
	float       m_fSize;					//������
	D3DXCOLOR   m_clrColor;					//����
	D3DXVECTOR3 m_vPosition;				//��ġ
	D3DXVECTOR3 m_vVelocity;				//������ ����
	D3DXVECTOR3 m_vGravity;					//�߷�
	D3DXVECTOR3 m_vWind;					//�ٶ�
	bool        m_bAirResistence;			//�ٶ�����
	float       m_fVelocityVar;				//������ ��
	TCHAR       *m_chTexFile;				//�ؽ�ó �̸�����
	bool		m_bRndColor;				//����Į��
	bool		m_btwinkle;					//��¦��ȿ��


	float		fTimePassed;				//����ð�
	float		m_fElapsedTime;				//��ŸŸ��
	TCHAR      m_tempTEXT[256];				//�ؽ�ó �̸� �ӽ�����


protected:

	D3DXQUATERNION*	m_qTargetQuat;			//ȸ�� ������
	D3DXVECTOR3		m_vCurPosition;			//���� ��ġ
	D3DXVECTOR3		m_vFirstPosition;		//�ʱ���ġ
	D3DXVECTOR3		m_vVarValue;			//��ġ ������
	D3DXVECTOR3*	m_vTargetPosition;		//������ ��ġ��
	D3DXMATRIXA16*  m_mTargetMat;			//��� ������
};

#endif 