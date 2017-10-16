#include "stdafx.h"
#include "O2PartStick.h"
#include "O2PartFollow.h"
#include "Manager.h"

O2ParticleManager::O2ParticleManager()
:g_nActiveSystem(0)
,g_curpartnum(0)
{
}

O2ParticleManager::~O2ParticleManager()
{
	DestroyAll();
}

void O2ParticleManager::DestroyAll()
{
	std::vector<O2ParticleSystem*>::iterator it = partlist.begin();
	int _size = partlist.size();
	for(int i=0;i<_size;i++,it++)
	{
		SAFE_DELETE(*it);
		--g_curpartnum;
	}
	partlist.clear();
}

HRESULT O2ParticleManager::Init(LPDIRECT3DDEVICE9* _device,O2InputManager* input, ResourceManager* pResc)
{
	m_device=(*_device);
	m_pInput=input;
	m_pResource = pResc;
	O2ParticleSystem::sp_pResource = m_pResource;

	return S_OK;
}




void O2ParticleManager::Update(float g_ElpasedTime, D3DXVECTOR3 _vTarget,D3DXMATRIXA16* _Mat,D3DXQUATERNION* _Quat)
{

	
	std::vector<O2ParticleSystem*>::iterator it = partlist.begin();
	int _size=partlist.size();

	for(int i=0;i<_size;i++)
	{
		if((*it)->GetLifestatus())	//	시간이 다된거 없애기
		{
			SAFE_DELETE(*it);
			it=partlist.erase(it);
		}else
		{
			(*it)->Update(g_ElpasedTime,_vTarget,_Mat,_Quat);
			++it;
		}
	}

}

void O2ParticleManager::Render()
{
	prerender();

	std::vector<O2ParticleSystem*>::iterator it = partlist.begin();
	std::vector<O2ParticleSystem*>::iterator end = partlist.end();
	while(it!=end)
	{
		m_device->SetTexture( 0, (*it)->GetTextureObject() );
		(*it)->Render();
		++it;
	}

	postrender();
}

void O2ParticleManager::prerender()
{
	m_device->SetRenderState( D3DRS_LIGHTING, FALSE );
	m_device->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

	m_device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	m_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
}

void O2ParticleManager::postrender()
{
	m_device->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	m_device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	m_device->SetRenderState( D3DRS_LIGHTING, TRUE );
}




//텍스트파일로 불러오기
void O2ParticleManager::AddParticle(TCHAR* _Fname,D3DXVECTOR3* _vPos/*0,0,0*/,O2ParticleManager::PARTICLE_TYPE _type,
									D3DXVECTOR3 _Val,D3DXMATRIXA16* _Mat, D3DXQUATERNION* _Quat)
{
	O2ParticleSystem* temp;
	switch(_type)
	{
	case PT_NATURAL:
		temp = new O2ParticleSystem;
		break;
	case PT_FOLLOWINGOBJ:
		temp = new O2PartFollow;
		break;
	case PT_STICKOBJECT:
		temp = new O2PartStick;
		break;
	default:
		break;

	}

	temp->SetFtName(_Fname);
	temp->Readtextfile();
	temp->SetValues(m_device);
	temp->SetPosition(*_vPos);
	temp->SetTargetPosition(_vPos);
	temp->SetVarValue(_Val);
	temp->SetTargetMat(_Mat);
	temp->SetTargetQuat(_Quat);

	partlist.push_back(temp);
	++g_curpartnum;
}

void O2ParticleManager::RestartP()
{
	int i = partlist.size();
	if(i>0)	
	{
		partlist[g_nActiveSystem]->RestartParticleSystem();
		ResetData();
	}
}

void O2ParticleManager::ResetData()
{
//텍스트파일로 정보갱신
//소스파일일경우 원래 속성으로 초기화 
	partlist[g_nActiveSystem]->SetAttributes();

//최신 텍스트 파일 데이터로 초기화
	//partlist[g_nActiveSystem]->Readtextfile();
	//partlist[g_nActiveSystem]->SetValues(&m_device);
}

void O2ParticleManager::ShowCurrentValue()
{
		printf("MAX_Particle %d\n",partlist[g_nActiveSystem]->GetMaxParticles());
		printf("NumToRelease %d\n",partlist[g_nActiveSystem]->GetNumToRelease());
		printf("ReleaseInterval %f\n",partlist[g_nActiveSystem]->GetReleaseInterval());
		printf("LifeCycle %f\n",partlist[g_nActiveSystem]->GetLifeCycle());
		printf("GetSize %f\n",partlist[g_nActiveSystem]->GetSize());
		printf("GetMaxPointSize %f\n",partlist[g_nActiveSystem]->GetMaxPointSize());
		printf("GetColor %f %f %f %f \n",partlist[g_nActiveSystem]->GetColor().r,partlist[g_nActiveSystem]->GetColor().g,
			partlist[g_nActiveSystem]->GetColor().b,partlist[g_nActiveSystem]->GetColor().a);
		printf("GetRndColor %d\n GetTwinkle %d\n",partlist[g_nActiveSystem]->GetRndcolor(),partlist[g_nActiveSystem]->GetTwinkle());
		printf("GetPosition %f %f %f\n",partlist[g_nActiveSystem]->GetPosition().x,partlist[g_nActiveSystem]->GetPosition().y,
			partlist[g_nActiveSystem]->GetPosition().z);
		printf("GetVelocity %f %f %f\n",partlist[g_nActiveSystem]->GetVelocity().x,partlist[g_nActiveSystem]->GetVelocity().y,
			partlist[g_nActiveSystem]->GetVelocity().z);
		printf("GetGravity %f %f %f\n",partlist[g_nActiveSystem]->GetGravity().x,partlist[g_nActiveSystem]->GetGravity().y,
			partlist[g_nActiveSystem]->GetGravity().z);
		printf("GetWind %f %f %f\n",partlist[g_nActiveSystem]->GetWind().x,partlist[g_nActiveSystem]->GetWind().y,
			partlist[g_nActiveSystem]->GetWind().z);
		printf("GetAirResistence %d\n",partlist[g_nActiveSystem]->GetAirResistence());
		printf("GetVelocityVar %f\n\n\n\n\n\n\n",partlist[g_nActiveSystem]->GetVelocityVar());
}



void O2ParticleManager::Input()
{

	if(m_pInput->IsKeyPress(DIK_LCONTROL))
	{
		RestartP();
	}

}






////	충돌 처리되는 벽
//// Create a series of planes to collide with
//temp->SetCollisionPlane( D3DXVECTOR3( 0.0f, 1.0f, 0.0f ), 
//	D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) ); // Floor

//temp->SetCollisionPlane( D3DXVECTOR3( 1.0f, 0.0f, 0.0f ), 
//	D3DXVECTOR3(-3.0f, 0.0f, 0.0f ) ); // Left Wall

//temp->SetCollisionPlane( D3DXVECTOR3(-1.0f, 0.0f, 0.0f ), 
//	D3DXVECTOR3( 3.0f, 0.0f, 0.0f ) ); // Right Wall

//temp->SetCollisionPlane( D3DXVECTOR3( 0.0f, 0.0f, 1.0f ), 
//	D3DXVECTOR3( 0.0f, 0.0f,-3.0f ) ); // Front Wall

//temp->SetCollisionPlane( D3DXVECTOR3( 0.0f, 0.0f,-1.0f ), 
//	D3DXVECTOR3( 0.0f, 0.0f, 3.0f ) ); // Back Wall

//temp->SetCollisionPlane( D3DXVECTOR3( 0.0f,-1.0f, 0.0f ), 
//	D3DXVECTOR3( 0.0f, 5.0f, 0.0f ) ); // Ceiling

//temp->Init( m_device );










//void M_Testmngr::AddDust(char* p_Name,D3DXVECTOR3 origin, int numParticles,float size, D3DXVECTOR3 direction, D3DXVECTOR3 _colormin, D3DXVECTOR3 _colormax,float _altha, float width, float lifetime, float speed )
//{
//	m_device=DEVICE;
//	CParticle* temp=new CDust();
//
//	temp->init(p_Name,&m_device);
//	temp->SetParticle(origin,numParticles,size,direction, _colormin, _colormax, _altha, width, lifetime, speed ); 
//	Particle_List.push_back(temp);
//}
//
//void M_Testmngr::FrameMove()
//{
//	std::list<CParticle*>::iterator start1=Particle_List.begin();
//	std::list<CParticle*>::iterator end1=Particle_List.end();
//
//	while(start1 != end1)
//	{
//		(*start1)->update(0.0001f);
//		if((*start1)->isDead())
//		{
//			if(*start1)
//			{
//				delete (*start1);
//				(*start1) = NULL;
//			}
//			Particle_List.erase(start1);
//			break;
//		}
//		++start1;
//	}
//}