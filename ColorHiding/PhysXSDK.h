#pragma once

#define DEFAULT_GRAVITY NxVec3(0.0f,-9.81f,0.0f)
#define JNxSDK	NxSDK::GetInstance()->GetSDK()
#define JNxScene NxSDK::GetInstance()->GetScene()
#define JNxCooking NxSDK::GetInstance()->GetCooking()

class O2ContactReport:public NxUserContactReport
{
public:
	/**
	Called for a pair in contact. The events parameter is a combination of:

	<ul>
	<li>NX_NOTIFY_ON_START_TOUCH,</li>
	<li>NX_NOTIFY_ON_END_TOUCH,</li>
	<li>NX_NOTIFY_ON_TOUCH,</li>
	<li>NX_NOTIFY_ON_START_TOUCH_FORCE_THRESHOLD,</li>
	<li>NX_NOTIFY_ON_END_TOUCH_FORCE_THRESHOLD,</li>
	<li>NX_NOTIFY_ON_TOUCH_FORCE_THRESHOLD,</li>
	<li>NX_NOTIFY_ON_IMPACT,	//unimplemented!</li>
	<li>NX_NOTIFY_ON_ROLL,		//unimplemented!</li>
	<li>NX_NOTIFY_ON_SLIDE,		//unimplemented!</li>
	</ul>
	
	*/
	virtual void  onContactNotify(NxContactPair& pair, NxU32 events);
	virtual ~O2ContactReport(){};
protected:
};


class NxSDK{
private:
	static NxSDK*		m_pSingle;			//	ΩÃ±€≈Ê ¿ŒΩ∫≈œΩ∫ ∆˜¿Œ≈Õ
	NxPhysicsSDK*		m_pPhysicsSDK;		//	SDK
	NxScene*			m_pScene;			//	Nx æ¿
	O2ContactReport		m_ContReport;		//	
	NxCookingInterface*	m_pCooking;		//	ƒÌ≈∑ ¿Œ≈Õ∆‰¿ÃΩ∫
	void CleanUp();
public:
	NxSDK();
	~NxSDK();
	HRESULT Init();
	static NxSDK*		GetInstance()	{ return m_pSingle; }
	NxPhysicsSDK*		GetSDK()		{ return m_pPhysicsSDK; }
	NxScene*			GetScene()		{ return m_pScene; }
	NxCookingInterface*	GetCooking()	{ return m_pCooking; }
};