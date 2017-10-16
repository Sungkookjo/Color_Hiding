#pragma once

#define MAP_WIDTH	512
#define MAP_HEIGHT	512

#define EFFECT_MAT_WORLD	0
#define EFFECT_MAT_RT		1
#define EFFECT_MAT_VIEW		2
#define EFFECT_MAT_PROJ		3
#define EFFECT_MAT_SCAIL	4

#define EFFECT_VEC_COLOR	0
#define EFFECT_VEC_LIGHT	1
#define EFFECT_VEC_SHADOW	2
#define	EFFECT_VEC_LIGHTPOSITION	3

#define EFFECT_TEX_TEX1		0
#define EFFECT_TEX_TEX2		1
#define EFFECT_TEX_TEX3		2

#define EFFECT_TECH_CELL	0
#define EFFECT_TECH_LINE	1
#define EFFECT_TECH_SHADOW	2
#define EFFECT_TECH_AMBIENT	3

#define EFFECT_FLOAT_FAR	0
// ���� ����
class O2CameraManager;

class Effect{
protected:
	LPDIRECT3DDEVICE9*		m_lpDevice	;	//	����̽�
	O2CameraManager*		m_pCam		;	// ī�޶� �Ŵ��� ( ��/�������� ��� �޾ƿ� )

	// ���̴�
	LPD3DXEFFECT		    m_lpEffect;		//	����Ʈ

	D3DXHANDLE				m_hTechnique;	//	��ũ��
	D3DXHANDLE				m_hmWorld;		//	���� ���
	D3DXHANDLE				m_hmRT;			//	���� ���
	D3DXHANDLE				m_hmView;		//	�� ���
	D3DXHANDLE				m_hmProj;		//	���� ���

	// ������Ÿ��
	LPDIRECT3DSURFACE9		m_lpDeep;		// ���̹���
	LPDIRECT3DTEXTURE9		m_lpOriginTex;	// �ؽ�ó
	LPDIRECT3DSURFACE9		m_lpOriginSurf;	// ǥ��

	Basefw_FS*				m_pFS;
public:
	Effect(LPDIRECT3DDEVICE9* pDev,O2CameraManager* m_pCam,Basefw_FS* FS);
	virtual ~Effect();

	LPD3DXEFFECT			GetEffect() { return m_lpEffect; }

	virtual HRESULT Init(TCHAR* _file);
	virtual UINT	PreRender();
	virtual void	PostRender();
	virtual	void	BeginPass(UINT i);
	virtual void	EndPass();
	virtual void	Commit();
	virtual void	SetFloat(UINT _flag,float pfloat) = 0;
	virtual void	SetVector(UINT _flag,D3DXVECTOR4& pVec)=0;
	virtual void	SetVector(UINT _flag,D3DCOLORVALUE& pVec)=0;
	virtual void	SetMatrix(UINT _flag,D3DXMATRIXA16& pVec)=0;
	virtual void	SetTexture(UINT _flag,LPDIRECT3DTEXTURE9 pTex)=0;
	virtual void	SetTechnique(UINT _flag) = 0;

	O2CameraManager* GetCam() { return m_pCam; }
};