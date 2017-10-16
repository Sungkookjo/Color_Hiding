#pragma once

#include "O2InputManager.h"
#include "O2CameraManager.h"

class O2ParticleManager
{
public:


	enum PARTICLE_TYPE
	{
		PT_NATURAL= 0,
		PT_FOLLOWINGOBJ = 1,
		PT_STICKOBJECT = 2
	};

private:
	LPDIRECT3DDEVICE9		m_device;
	O2InputManager*			m_pInput;
	ResourceManager*		m_pResource	;

	int g_nActiveSystem;						//현재파티클
	int g_curpartnum;							//파티클 개수

	std::vector<O2ParticleSystem*> partlist;	//파티클 리스트

public:
	
	O2ParticleManager();
	~O2ParticleManager();


	HRESULT Init(LPDIRECT3DDEVICE9* _device,O2InputManager* input, ResourceManager* pResc);

	void Update(float g_ElpasedTime, D3DXVECTOR3 _vTarget,D3DXMATRIXA16* _Mat,D3DXQUATERNION* _Quat);	//프레임간 시간을 얻어와 업데이트

	void		Input();

	virtual void Render();
	void prerender();
	void postrender();

//////		파티클 추가 : 파일이름,대상 포지션, 파티클 타입, 조정위치값, 대상 매트릭스,대상 쿼터니온
	void AddParticle(TCHAR* _Fname,D3DXVECTOR3* _vPos=NULL,PARTICLE_TYPE _type=PT_NATURAL,
						D3DXVECTOR3 _Val=D3DXVECTOR3(0,0,0) ,D3DXMATRIXA16* _Mat=NULL, D3DXQUATERNION* _Quat=NULL);
	void DestroyAll();							//모든 파티클 제거

	void RestartP();							//라이프사이클에따라 루프시키기
	void ResetData();							//데이터 재설정




	O2ParticleSystem* GetCurPartilce() { return partlist[g_nActiveSystem]; }			//현재 파티클 얻어오기
	std::vector<O2ParticleSystem*>* GetParticlelist() { return &partlist; }			//파티클 리스트 얻기
	void ShowCurrentValue();														//현재 파티클 정보얻어오기				
	void SetType(int _t) { g_nActiveSystem= (_t%g_curpartnum);}						//보여지는 파티클 타입
	int GetType(){ return ++g_nActiveSystem; }

//파티클 정보 관련
	void SetMaxParticles( int dwMaxParticles ) { partlist[g_nActiveSystem]->SetMaxParticles(dwMaxParticles); }
	int GetMaxParticles( void ) { return partlist[g_nActiveSystem]->GetMaxParticles(); }

	void SetNumToRelease( int dwNumToRelease ) { partlist[g_nActiveSystem]->SetNumToRelease(dwNumToRelease); }
	int GetNumToRelease( void ) { return partlist[g_nActiveSystem]->GetNumToRelease(); }

	void SetReleaseInterval( float fReleaseInterval ) { partlist[g_nActiveSystem]->SetReleaseInterval(fReleaseInterval); }
	float GetReleaseInterval( void ) { return partlist[g_nActiveSystem]->GetReleaseInterval(); }

	void SetLifeCycle( float fLifeCycle ) { partlist[g_nActiveSystem]->SetLifeCycle(fLifeCycle); }
	float GetLifeCycle( void ) { return partlist[g_nActiveSystem]->GetLifeCycle(); }

	void SetLifeTime( float fLifetime ) { partlist[g_nActiveSystem]-> SetLifeTime(fLifetime); }
	float GetLifeTime( void ) { return partlist[g_nActiveSystem]->GetLifeTime(); }

	void SetSize( float fSize ) { partlist[g_nActiveSystem]-> SetSize(fSize); }
	float GetSize( void ) { return partlist[g_nActiveSystem]->GetSize(); }
	float GetMaxPointSize( void ) { return partlist[g_nActiveSystem]->GetMaxPointSize(); }

	void SetColor( D3DXCOLOR clrColor ) { partlist[g_nActiveSystem]-> SetColor(clrColor); }
	D3DXCOLOR GetColor( void ) { return partlist[g_nActiveSystem]->GetColor(); }

	void SetPosition( D3DXVECTOR3 vPosition ) { partlist[g_nActiveSystem]-> SetPosition(vPosition); }
	D3DXVECTOR3 GetPosition( void ) { return partlist[g_nActiveSystem]->GetPosition(); }

	void SetVelocity( D3DXVECTOR3 vVelocity ) { partlist[g_nActiveSystem]->SetVelocity(vVelocity); }
	D3DXVECTOR3 GetVelocity( void ) { return partlist[g_nActiveSystem]->GetVelocity(); }

	void SetGravity( D3DXVECTOR3 vGravity ) { partlist[g_nActiveSystem]-> SetGravity(vGravity); }
	D3DXVECTOR3 GetGravity( void ) { return partlist[g_nActiveSystem]->GetGravity(); }

	void SetWind( D3DXVECTOR3 vWind ) { partlist[g_nActiveSystem]-> SetWind(vWind); }
	D3DXVECTOR3 GetWind( void ) { return partlist[g_nActiveSystem]->GetWind(); }

	void SetAirResistence( bool bAirResistence ) { partlist[g_nActiveSystem]->SetAirResistence(bAirResistence); }
	bool GetAirResistence( void ) { return partlist[g_nActiveSystem]->GetAirResistence(); }

	void SetVelocityVar( float fVelocityVar ) { partlist[g_nActiveSystem]->SetVelocityVar(fVelocityVar); }
	float GetVelocityVar( void ) { return partlist[g_nActiveSystem]->GetVelocityVar(); }

	void SetCollisionPlane( D3DXVECTOR3 vPlaneNormal, D3DXVECTOR3 vPoint, 
		float fBounceFactor = 1.0f, int nCollisionResult = 0 );

	void SetRndcolor(bool _rndcolor) { partlist[g_nActiveSystem]-> SetRndcolor(_rndcolor); }
	bool GetRndcolor() { return partlist[g_nActiveSystem]->GetRndcolor(); }

	void SetTwinkle(bool _twinkle) { partlist[g_nActiveSystem]->SetTwinkle(_twinkle); }
	bool GetTwinkle() { return partlist[g_nActiveSystem]->GetTwinkle(); }


};


