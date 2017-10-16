#include "stdafx.h"
#include "O2CameraManager.h"
#include "ToonEffect.h"

ToonEffect::ToonEffect( LPDIRECT3DDEVICE9* pDev,O2CameraManager* pCam,Basefw_FS* FS)
:Effect(pDev,pCam,FS)
{

}

ToonEffect::~ToonEffect()
{
}

void ToonEffect::SetVector( UINT _flag,D3DXVECTOR4& pVec )
{
	switch(_flag){
		case EFFECT_VEC_COLOR:
			m_lpEffect->SetVector(m_hvColor,&pVec);
			break;
		case EFFECT_VEC_LIGHT:
			m_lpEffect->SetVector(m_hvLight,&pVec);
			break;
		case EFFECT_VEC_SHADOW:
			m_lpEffect->SetVector(m_hvShadowColor,&pVec);
			break;
		case EFFECT_VEC_LIGHTPOSITION:
			D3DXVec4Transform( &pVec, &pVec, m_pCam->GetMatView() );
			m_lpEffect->SetVector(m_hmLight,&pVec);
			break;
		default:
			break;
	}
}

void ToonEffect::SetVector( UINT _flag,D3DCOLORVALUE& pVec )
{
	switch(_flag){
		case EFFECT_VEC_COLOR:
			m_lpEffect->SetVector(m_hvColor,(D3DXVECTOR4*)&pVec);
			break;
		case EFFECT_VEC_LIGHT:
			break;
		default:
			break;
	}
}
void ToonEffect::SetMatrix( UINT _flag,D3DXMATRIXA16& pVec )
{
	switch(_flag){
		case EFFECT_MAT_WORLD:
			m_lpEffect->SetMatrix(m_hmWorld,&pVec);
			break;
		case EFFECT_MAT_RT:
			m_lpEffect->SetMatrix(m_hmRT,&pVec);
			break;
		case EFFECT_MAT_VIEW:
			m_lpEffect->SetMatrix(m_hmView,&pVec);
			break;
		case EFFECT_MAT_PROJ:
			m_lpEffect->SetMatrix(m_hmProj,&pVec);
			break;
		case EFFECT_MAT_SCAIL:
			m_lpEffect->SetMatrix(m_hmScail,&pVec);
			break;
		default:
			break;
	}
}

void ToonEffect::SetTexture( UINT _flag,LPDIRECT3DTEXTURE9 pTex )
{
	switch(_flag){
		case EFFECT_TEX_TEX1:
			m_lpEffect->SetTexture(m_hpTex,pTex);
			break;
		default:
			break;
	}
}

HRESULT ToonEffect::Init( TCHAR* _file )
{
	D3DCAPS9 cap;

	if(FAILED(Effect::Init(_file))){
		return E_FAIL;
	}
	if(FAILED((*m_lpDevice)->GetDeviceCaps( &cap ))){
		return E_FAIL;
	}

	m_hTechCell = m_lpEffect->GetTechniqueByName( "Cell" );
	m_hTechLine = m_lpEffect->GetTechniqueByName( "Line" );
	m_hTechAmbient = m_lpEffect->GetTechniqueByName("RenderSceneAmbient");
	//if(cap.StencilCaps & D3DSTENCILCAPS_TWOSIDED){
	if(0){
		m_hTechShadow = m_lpEffect->GetTechniqueByName( "RenderShadowVolume2Sided" );
	}else{
		m_hTechShadow = m_lpEffect->GetTechniqueByName( "RenderShadowVolume" );
	}
	m_hvColor		= m_lpEffect->GetParameterByName( NULL, "vColor" );
	m_hvLight		= m_lpEffect->GetParameterByName( NULL, "vLight" );
	m_hpTex			= m_lpEffect->GetParameterByName( NULL, "tOrigin" );
	m_hmScail		= m_lpEffect->GetParameterByName( NULL, "mLine" );
	m_hfFarClip		= m_lpEffect->GetParameterByName( NULL, "g_fFarClip" );
	m_hvShadowColor	= m_lpEffect->GetParameterByName( NULL, "g_vShadowColor" );
	m_hmLight		= m_lpEffect->GetParameterByName( NULL, "g_vLightView" );

	m_lpEffect->SetVector(m_hvLight,&LIGHT_DIRECTION);

	return S_OK;
}

void ToonEffect::SetTechnique( UINT _flag )
{
	switch(_flag)
	{
	case EFFECT_TECH_CELL:
		m_lpEffect->SetTechnique(m_hTechCell);
		break;
	case EFFECT_TECH_LINE:
		m_lpEffect->SetTechnique(m_hTechLine);
		break;
	case EFFECT_TECH_SHADOW:
		m_lpEffect->SetTechnique(m_hTechShadow);
		break;
	case EFFECT_TECH_AMBIENT:
		m_lpEffect->SetTechnique(m_hTechAmbient);
		break;
	}
}

void ToonEffect::SetFloat( UINT _flag,float pfloat )
{
	switch(_flag){
		case EFFECT_FLOAT_FAR:
			m_lpEffect->SetFloat(m_hfFarClip,pfloat);
			break;
	}
}