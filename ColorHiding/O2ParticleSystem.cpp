#include "stdafx.h"
#include "O2ParticleSystem.h"

ResourceManager* O2ParticleSystem::sp_pResource = NULL;


float O2ParticleSystem::getRandomMinMax( float fMin, float fMax )
{
	float fRandNum = (float)rand () / RAND_MAX;
	return fMin + (fMax - fMin) * fRandNum;
}


D3DXVECTOR3 O2ParticleSystem::getRandomVector( void )
{
	D3DXVECTOR3 vVector;

	vVector.z = getRandomMinMax( -1.0f, 1.0f );

	float radius = (float)sqrt(1 - vVector.z * vVector.z);

	float t = getRandomMinMax( -D3DX_PI, D3DX_PI );

	vVector.x = (float)cosf(t) * radius;
	vVector.y = (float)sinf(t) * radius;

	return vVector;
}


int O2ParticleSystem::classifyPoint( D3DXVECTOR3 *vPoint, Plane *pPlane )
{
	D3DXVECTOR3 vDirection = pPlane->m_vPoint - *vPoint;
	float fResult = D3DXVec3Dot( &vDirection, &pPlane->m_vNormal );

	if( fResult < -0.001f )
		return CP_FRONT;

	if( fResult >  0.001f )
		return CP_BACK;

	return CP_ONPLANE;
}

O2ParticleSystem::O2ParticleSystem()
:CP_FRONT(0), CP_BACK(1), CP_ONPLANE(2), CR_BOUNCE(0), CR_STICK(1), CR_RECYCLE(2)
{
	m_dwVBOffset       = 0;    
	m_dwFlush          = 512;  
	m_dwDiscard        = 2048; 
	m_pActiveList      = NULL; // 작동하는 파티클의 헤더
	m_pFreeList        = NULL; // 재활용 대기 파티클 헤더
	m_pPlanes          = NULL;
	m_dwActiveCount    = 0;
	m_fCurrentTime     = 0.0f;
	m_fLastUpdate      = 0.0f;
	m_pVB              = NULL; 
	m_chTexFile        = NULL;
	m_ptexParticle     = NULL;
	m_dwMaxParticles   = 1;
	m_dwNumToRelease   = 1;
	m_fReleaseInterval = 1.0f;
	m_fLifeCycle       = 1.0f;
	m_fLifeTime		   = 1.0f;
	m_fSize            = 1.0f;
	m_clrColor         = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);
	m_vPosition        = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_vVelocity        = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_vGravity         = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_vWind            = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_bAirResistence   = true;
	m_fVelocityVar     = 1.0f;
	m_bRndColor		   = false;
	m_btwinkle		   = false;
	m_qTargetQuat	   = NULL;
	m_vCurPosition	   = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_vTargetPosition  = NULL;
	m_mTargetMat	   = NULL;
	m_vVarValue        = D3DXVECTOR3(0.0f,0.0f,0.0f);
	fTimePassed		   = 0.0f;
	m_vFirstPosition   = D3DXVECTOR3(0.0f,0.0f,0.0f);

	SetTexture(_T("particle/particle.bmp"));
}



O2ParticleSystem::~O2ParticleSystem()
{
	DestroyP();
}

void O2ParticleSystem::DestroyP()
{
//	InvalidateDeviceObjects();

	while( m_pPlanes ) 
	{
		Plane *pPlane = m_pPlanes;   
		m_pPlanes = pPlane->m_pNext; 
		SAFE_DELETE(pPlane);              
	}

	while( m_pActiveList )
	{
		Particle* pParticle = m_pActiveList;
		m_pActiveList = pParticle->m_pNext;
		SAFE_DELETE(pParticle);
	}
	m_pActiveList = NULL;

	while( m_pFreeList )
	{
		Particle *pParticle = m_pFreeList;
		m_pFreeList = pParticle->m_pNext;
		SAFE_DELETE(pParticle);
	}
	m_pFreeList = NULL;

	SAFE_DELETE_ARRAY(m_chTexFile);
//	SAFE_RELEASE(m_ptexParticle);
}


void O2ParticleSystem::SetTexture(TCHAR* chTexFile )
{

	SAFE_DELETE_ARRAY(m_chTexFile);

	m_chTexFile = new TCHAR[_tcslen( chTexFile ) + 1];

	if( m_chTexFile != NULL )
		_tcscpy( m_chTexFile, chTexFile );
}


LPDIRECT3DTEXTURE9 &O2ParticleSystem::GetTextureObject()
{
	return m_ptexParticle;
}


void O2ParticleSystem::SetCollisionPlane( D3DXVECTOR3 vPlaneNormal, D3DXVECTOR3 vPoint, 
										float fBounceFactor, int nCollisionResult )
{
	Plane *pPlane = new Plane;

	pPlane->m_vNormal          = vPlaneNormal;
	pPlane->m_vPoint           = vPoint;
	pPlane->m_fBounceFactor    = fBounceFactor;
	pPlane->m_nCollisionResult = nCollisionResult;

	pPlane->m_pNext = m_pPlanes; 
	m_pPlanes = pPlane;         
}


HRESULT O2ParticleSystem::Init( LPDIRECT3DDEVICE9 pd3dDevice )
{

	m_device=pd3dDevice;
	HRESULT hr;


	if( FAILED( hr = RestoreDeviceObjects( m_device ) ) )
		return hr;

	D3DCAPS9 d3dCaps;
	m_device->GetDeviceCaps( &d3dCaps );
	m_fMaxPointSize = d3dCaps.MaxPointSize;

	if( d3dCaps.FVFCaps & D3DFVFCAPS_PSIZE )
		m_bDeviceSupportsPSIZE = true;
	else
		m_bDeviceSupportsPSIZE = false;


	m_ptexParticle = sp_pResource->GetTexture(m_chTexFile);

	return S_OK;
}

HRESULT O2ParticleSystem::Update( FLOAT fElpasedTime ,D3DXVECTOR3 _vTarget,D3DXMATRIXA16* _Mat,D3DXQUATERNION* _Quat)
{
	SetupPosition();

	Particle  *pParticle;
	Particle **ppParticle;
	Plane     *pPlane;
	Plane    **ppPlane;
	D3DXVECTOR3 vOldPosition;

	m_fElapsedTime = fElpasedTime;
	m_fCurrentTime += fElpasedTime;    

	ppParticle = &m_pActiveList;

	while( *ppParticle )
	{
		pParticle = *ppParticle; 

		fTimePassed  = m_fCurrentTime - pParticle->m_fInitTime;

		if( fTimePassed > m_fLifeCycle )
		{
			*ppParticle = pParticle->m_pNext;
			pParticle->m_pNext = m_pFreeList;
			m_pFreeList = pParticle;

			--m_dwActiveCount;
		}
		else
		{
			pParticle->m_vCurVel += m_vGravity * fElpasedTime;
			if(pParticle->m_bColorstatus)
			{
				if(m_bRndColor) pParticle->m_cColor = D3DXCOLOR(getRandomMinMax(0,1.0f),getRandomMinMax(0,1.0f)
					,getRandomMinMax(0,1.0f),getRandomMinMax(0,1.0f));
				pParticle->m_bColorstatus = false;
			}else
			{
				if(!m_bRndColor) pParticle->m_cColor = m_clrColor;
			}
	
			if( m_bAirResistence == true )
				pParticle->m_vCurVel += (m_vWind - pParticle->m_vCurVel) * fElpasedTime;

			vOldPosition = pParticle->m_vCurPos;
			pParticle->m_vCurPos += pParticle->m_vCurVel * fElpasedTime;


			ppPlane = &m_pPlanes; 

			while( *ppPlane )
			{
				pPlane = *ppPlane;
				int result = classifyPoint( &pParticle->m_vCurPos, pPlane );

				if( result == CP_BACK /*|| result == CP_ONPLANE */ )
				{
					if( pPlane->m_nCollisionResult == CR_BOUNCE )
					{
						pParticle->m_vCurPos = vOldPosition;

						//-----------------------------------------------------------------
						//
						// The new velocity vector of a particle that is bouncing off
						// a plane is computed as follows:
						//
						// Vn = (N.V) * N
						// Vt = V - Vn
						// Vp = Vt - Kr * Vn
						//
						// Where:
						// 
						// .  = Dot product operation
						// N  = The normal of the plane from which we bounced
						// V  = Velocity vector prior to bounce
						// Vn = Normal force
						// Kr = The coefficient of restitution ( Ex. 1 = Full Bounce, 
						//      0 = Particle Sticks )
						// Vp = New velocity vector after bounce
						//
						//-----------------------------------------------------------------

						float Kr = pPlane->m_fBounceFactor;

						D3DXVECTOR3 Vn = D3DXVec3Dot( &pPlane->m_vNormal, 
							&pParticle->m_vCurVel ) * 
							pPlane->m_vNormal;
						D3DXVECTOR3 Vt = pParticle->m_vCurVel - Vn;
						D3DXVECTOR3 Vp = Vt - Kr * Vn;

						pParticle->m_vCurVel = Vp;
					}
					else if( pPlane->m_nCollisionResult == CR_RECYCLE )
					{
						pParticle->m_fInitTime -= m_fLifeCycle;
					}

					else if( pPlane->m_nCollisionResult == CR_STICK )
					{
						pParticle->m_vCurPos = vOldPosition;
						pParticle->m_vCurVel = D3DXVECTOR3(0.0f,0.0f,0.0f);
					}
				}

				ppPlane = &pPlane->m_pNext;
			}


			ppParticle = &pParticle->m_pNext;
		}
	}

	if( m_fCurrentTime - m_fLastUpdate > m_fReleaseInterval )
	{
		m_fLastUpdate = m_fCurrentTime;

		for( int i = 0; i < m_dwNumToRelease; ++i )
		{
			if( m_pFreeList )
			{
				pParticle = m_pFreeList;
				m_pFreeList = pParticle->m_pNext;
			}
			else
			{
				if( m_dwActiveCount < m_dwMaxParticles )
				{
					if( NULL == ( pParticle = new Particle ) )
						return E_OUTOFMEMORY;
				}
			}

			if( m_dwActiveCount < m_dwMaxParticles )
			{
				pParticle->m_pNext = m_pActiveList; 
				m_pActiveList = pParticle;

				pParticle->m_vCurVel = m_vVelocity;

				if( m_fVelocityVar != 0.0f )
				{
					D3DXVECTOR3 vRandomVec = getRandomVector();
					pParticle->m_vCurVel += vRandomVec * m_fVelocityVar;
				}

				pParticle->m_fInitTime  = m_fCurrentTime;
				pParticle->m_vCurPos    = m_vFirstPosition;

				++m_dwActiveCount;
			}
		}
	}

	return S_OK;
}

void O2ParticleSystem::RestartParticleSystem( void )
{
	Particle  *pParticle;
	Particle **ppParticle;

	ppParticle = &m_pActiveList; 

	while( *ppParticle )
	{
		pParticle = *ppParticle; 
		pParticle->m_bColorstatus=true;

		*ppParticle = pParticle->m_pNext;
		pParticle->m_pNext = m_pFreeList;
		m_pFreeList = pParticle;

		--m_dwActiveCount;
	}

	while( m_pPlanes ) 
	{
		Plane *pPlane = m_pPlanes;   
		m_pPlanes = pPlane->m_pNext; 
		delete pPlane;               
	}
}

HRESULT O2ParticleSystem::Render()
{   
	SetupWorld();
	HRESULT hr;

	m_device->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );       // Turn on point sprites
	m_device->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );       // Allow sprites to be scaled with distance
	m_device->SetRenderState( D3DRS_POINTSIZE,     FtoDW(m_fSize) ); // Float value that specifies the size to use for point size computation in cases where point size is not specified for each vertex.
	m_device->SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(1.0f) );    // Float value that specifies the minimum size of point primitives. Point primitives are clamped to this size during rendering. 
	m_device->SetRenderState( D3DRS_POINTSCALE_A,  FtoDW(0.0f) );    // Default 1.0
	m_device->SetRenderState( D3DRS_POINTSCALE_B,  FtoDW(0.0f) );    // Default 0.0
	m_device->SetRenderState( D3DRS_POINTSCALE_C,  FtoDW(1.0f) );    // Default 0.0

	Particle    *pParticle = m_pActiveList;
	PointVertex *pVertices;
	DWORD        dwNumParticlesToRender = 0;

	m_dwVBOffset += m_dwFlush;

	if( m_dwVBOffset >= m_dwDiscard )
		m_dwVBOffset = 0;

	if( FAILED( hr = m_pVB->Lock( m_dwVBOffset * sizeof(PointVertex), 
		m_dwFlush * sizeof(PointVertex),   
		(void**) &pVertices, 
		m_dwVBOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD)))
	{
		return hr;
	}

	while( pParticle )
	{
		D3DXVECTOR3 vPos= ( GetcurPos(pParticle) );
	//	D3DXVECTOR3 vVel(pParticle->m_vCurVel);



		pVertices->posit = vPos;
		
		(m_btwinkle)? pVertices->color = D3DXCOLOR(getRandomMinMax(0,1.0f),getRandomMinMax(0,1.0f)
			,getRandomMinMax(0,1.0f),getRandomMinMax(0,1.0f)) : pVertices->color = pParticle->m_cColor;
		pVertices++;

		if( ++dwNumParticlesToRender == m_dwFlush )
		{

			m_pVB->Unlock();

			m_device->SetStreamSource( 0, m_pVB, 0, sizeof(PointVertex) );
			m_device->SetFVF( PointVertex::FVF_Flags );

			if( FAILED(hr = m_device->DrawPrimitive( D3DPT_POINTLIST, 
				m_dwVBOffset, dwNumParticlesToRender)))
			{
				return hr;
			}

			m_dwVBOffset += m_dwFlush;

			if( m_dwVBOffset >= m_dwDiscard )
				m_dwVBOffset = 0;

			if( FAILED( hr = m_pVB->Lock( 
				m_dwVBOffset * sizeof(PointVertex), 
				m_dwFlush    * sizeof(PointVertex), 
				(void**) &pVertices, 
				m_dwVBOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD)))
			{
				return hr;
			}

			dwNumParticlesToRender = 0;
		}

		pParticle = pParticle->m_pNext;
	}

	m_pVB->Unlock();

	if( dwNumParticlesToRender )
	{
		m_device->SetStreamSource( 0, m_pVB, 0, sizeof(PointVertex) );
		m_device->SetFVF( PointVertex::FVF_Flags );

		if(FAILED(hr = m_device->DrawPrimitive( D3DPT_POINTLIST, m_dwVBOffset, 
			dwNumParticlesToRender )))
			return hr;
	}


	m_device->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
	m_device->SetRenderState( D3DRS_POINTSCALEENABLE,  FALSE );

	return S_OK;
}


HRESULT O2ParticleSystem::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	HRESULT hr;


	if( FAILED( hr = pd3dDevice->CreateVertexBuffer( 
		m_dwDiscard * sizeof(PointVertex), 
		D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY | D3DUSAGE_POINTS, 
		PointVertex::FVF_Flags, 
		D3DPOOL_DEFAULT, 
		&m_pVB, NULL )))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT O2ParticleSystem::InvalidateDeviceObjects()
{
	if( m_pVB != NULL )
		m_pVB->Release();

	return S_OK;
}

bool O2ParticleSystem::GetLifestatus()
{
	if(m_fCurrentTime>=m_fLifeTime-0.1f)
	{
		return true;
	}
	return false;
}

void O2ParticleSystem::Readtextfile()
{
	FILE* fp=_tfopen(Ftname,_T("r"));
	TCHAR line[256];
	int numplane,Colresult;
	float bounce;
	D3DXVECTOR3 vnormal,vpoint;
	bounce=1.0f;
	numplane=0;
	Colresult=0;
	_fgetts(line,256,fp);
	_stscanf(line,_T("%s %d %d %d"), m_tempTEXT,&m_dwMaxParticles,&m_dwNumToRelease,&numplane);
	_fgetts(line, 256, fp);
	_stscanf(line, _T("%f %f %f %f %f"), &m_fReleaseInterval,&m_fLifeCycle,&m_fLifeTime,&m_fSize,&m_fVelocityVar);
	_fgetts(line, 256, fp);
	_stscanf(line,_T("%f %f %f %f %d %d"),&m_clrColor.r,&m_clrColor.g,&m_clrColor.b,&m_clrColor.a,
		&m_bRndColor,&m_btwinkle);
	_fgetts(line, 256, fp);
	_stscanf(line,_T("%f %f %f"),&m_vPosition.x,&m_vPosition.y,&m_vPosition.z);
	_fgetts(line, 256, fp);
	_stscanf(line,_T("%f %f %f"),&m_vVelocity.x,&m_vVelocity.y,&m_vVelocity.z);
	_fgetts(line, 256, fp);
	_stscanf(line,_T("%f %f %f"),&m_vGravity.x,&m_vGravity.y,&m_vGravity.z);
	_fgetts(line, 256, fp);
	_stscanf(line,_T("%f %f %f %d"),&m_vWind.x,&m_vWind.y,&m_vWind.z,&m_bAirResistence);

	for(int i=0;i<numplane;i++)
	{
		_fgetts(line, 256, fp);
		_stscanf(line,_T("%f %f %f %f %f %f %f %d"),&vnormal.x,&vnormal.y,&vnormal.z,
			&vpoint.x,&vpoint.y,&vpoint.z,&bounce,&Colresult);
		SetCollisionPlane(vnormal,vpoint,bounce,Colresult);
	}

	fclose(fp);
}

void O2ParticleSystem::SetValues(LPDIRECT3DDEVICE9 _device)
{
	SetTexture( m_tempTEXT );						//텍스쳐이름
	SetMaxParticles( m_dwMaxParticles );			//파티클개수
	SetNumToRelease( m_dwNumToRelease );			//한번에 나오는개수
	SetReleaseInterval( m_fReleaseInterval );		//다시뿌려주는 딜레이
	SetLifeCycle( m_fLifeCycle );					//한번 뿌려지는 지속시간
	SetSize( m_fSize );								//파티클 크기
	SetVelocityVar( m_fVelocityVar );				//힘크기
	SetColor( m_clrColor );							//색깔
	SetRndcolor( m_bRndColor );					
	SetPosition( m_vPosition );						//파티클위치
	SetVelocity( m_vVelocity );						//힘방향
	SetGravity( m_vGravity );						//중력	
	SetWind( m_vWind);								//바람
	SetAirResistence(m_bAirResistence);				//바람저항 유무

	Init( _device );
}

void O2ParticleSystem::SetAttributes()
{
	Readtextfile();
	SetValues(m_device);
}

void O2ParticleSystem::SetupWorld()
{
	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity(&matWorld);
	m_device->SetTransform(D3DTS_WORLD,&matWorld);
}

void O2ParticleSystem::SetupPosition()
{
	m_vCurPosition = m_vPosition;
	m_vFirstPosition = m_vPosition;
}

D3DXVECTOR3 O2ParticleSystem::GetcurPos( Particle* part )
{
	return part->m_vCurPos;
}