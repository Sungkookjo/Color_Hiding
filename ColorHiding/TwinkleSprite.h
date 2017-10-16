#pragma once
#include "TimeSprite.h"

class TwinkleSprite : public TimeSprite{
private:
	float	m_fTwinkleTime;
	bool	m_bOn;
	int		m_iIndex;
	LPDIRECT3DTEXTURE9	m_pTextureOn[2];
public:
	TwinkleSprite();
	~TwinkleSprite();

	virtual void Render();

	virtual UINT Update(POINT& pt,UINT _MsState);
	
	VOID	SetTwinkle(float	pTime,float pTwinkTime)	{
		m_fCurTime = m_fMaxTime = pTime; 
		m_fTwinkleTime = pTwinkTime;
	}

	VOID	SetOnTexture(tstring ptex1,tstring ptex2);

	void SetOn();
};

class Twinkle2TexSprite: public TimeSprite{
private:
	float m_fTwinkleTime;
	bool m_bRend;
public:
	Twinkle2TexSprite();
	~Twinkle2TexSprite();

	virtual void Render();

	virtual UINT Update(POINT& pt,UINT _MsState);

	VOID	SetTwinkle(float	pTime,float pTwinkTime)	{
		m_fCurTime = 0.0f;
		m_fMaxTime = pTime; 
		m_fTwinkleTime = pTwinkTime;
	}
};