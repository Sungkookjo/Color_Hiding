#include "stdafx.h"
#include "TwinkleSprite.h"


TwinkleSprite::TwinkleSprite()
:m_fTwinkleTime(0.0f)
,m_bOn(FALSE)
,m_iIndex(0)
{

}

TwinkleSprite::~TwinkleSprite()
{

}

UINT TwinkleSprite::Update( POINT& pt,UINT _MsState )
{
	if( FALSE == m_bOn)
		return S_OK;
	
	m_fCurTime += (*m_fpDelta);

	if( m_fCurTime >= m_fMaxTime){
		m_bOn = FALSE;
	}else{
		m_iIndex = ((int)(m_fCurTime/m_fTwinkleTime))%2;
	}

	return S_OK;
}

void TwinkleSprite::Render()
{
	SetMat();
	if(m_bOn){
		(*m_lpDevice)->SetTexture( 0,(m_pTextureOn[m_iIndex]) );
		(*m_lpDevice)->SetStreamSource( 0, (m_lpVB), 0, sizeof( GUIVERTEX ) );
		(*m_lpDevice)->SetFVF( D3DFVF_GUIVERTEX );
		(*m_lpDevice)->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
	}else{
		(*m_lpDevice)->SetTexture( 0,(m_pTexture) );
		(*m_lpDevice)->SetStreamSource( 0, (m_lpVB), 0, sizeof( GUIVERTEX ) );
		(*m_lpDevice)->SetFVF( D3DFVF_GUIVERTEX );
		(*m_lpDevice)->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
	}
}

VOID TwinkleSprite::SetOnTexture( tstring ptex1,tstring ptex2 )
{
	m_pTextureOn[0] = m_pResource->GetTexture(ptex1);
	m_pTextureOn[1] = m_pResource->GetTexture(ptex2);
}

void TwinkleSprite::SetOn()
 {
	 if(!m_bOn)
	 {
		 m_bOn = TRUE;
		 m_fCurTime = 0.0f;
		 m_iIndex=0;
	 }else{
		 m_fCurTime -= ((int)(m_fCurTime/m_fTwinkleTime))*m_fTwinkleTime;
	 }
}

Twinkle2TexSprite::Twinkle2TexSprite()
{

}

Twinkle2TexSprite::~Twinkle2TexSprite()
{

}

void Twinkle2TexSprite::Render()
{
	if(m_bRend){
		SetMat();
		(*m_lpDevice)->SetTexture( 0,m_pTexture );
		(*m_lpDevice)->SetStreamSource( 0, (m_lpVB), 0, sizeof( GUIVERTEX ) );
		(*m_lpDevice)->SetFVF( D3DFVF_GUIVERTEX );
		(*m_lpDevice)->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
	}
}

UINT Twinkle2TexSprite::Update( POINT& pt,UINT _MsState )
{
	m_fCurTime += (*m_fpDelta);

	if( m_fCurTime > m_fMaxTime)
	{
		return TIME_OVER;
	}else{
		if(((int)(m_fCurTime / m_fTwinkleTime))%2){
			m_bRend = FALSE;
		}else{
			m_bRend = TRUE;
		}
	}

	return 0;
}