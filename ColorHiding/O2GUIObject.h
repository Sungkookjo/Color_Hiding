//////////////////////////////////////////////////////////////////////////
// O2GUIObject.h
// 설명 : O2GUIObject 클래스 헤더
// 작성자 : 조성국
// 작성일 : 08 / 07 / 01
//////////////////////////////////////////////////////////////////////////
#pragma once


#define GUI_CENTER_CENTER	0
#define GUI_CENTER_LEFMID	1
#define GUI_CENTER_MIDBOT	2
#define GUI_CENTER_RIGHTTOP	3
#define GUI_CENTER_MIDTOP	4

#define GAGE_COVER	0
#define GAGE_BAR	1

#define TIME_OVER	666

// UI 인덱스
#define UI_START	1
#define UI_CREATE	2
#define UI_END		3
#define UI_PAUSE	4
#define	UI_HELP		5
#define UI_OVER		6
#define UI_RESUME	UI_START
#define UI_QUIT		UI_END
#define UI_NEXT		7
#define UI_SOUND	10

struct GUIVERTEX
{
	D3DXVECTOR3 position;
	D3DCOLOR color;
	FLOAT tu, tv;
};

#define D3DFVF_GUIVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

class O2GUIBase{
protected:
	tstring					m_tFileName	;	//	파일 이름	
	O2Rect					m_vRect		;	//	텍스쳐에서 그림의 위치	
	D3DXVECTOR3				m_vPos		;	//	화면에서 그림의 위치
	LPDIRECT3DDEVICE9*		m_lpDevice	;	//	디바이스
	ResourceManager*		m_pResource	;	//	리소스 박스
	UINT					m_uType		;	//	gui 타입
	D3DXMATRIXA16			m_matWorld	;	//	월드행렬
	D3DXMATRIXA16			m_matRot	;	//	월드행렬
	D3DXMATRIXA16			m_matScail	;	//	스케일 행렬
	D3DXMATRIXA16			m_matTrans	;	//	포지션 행렬
	virtual BOOL IsInRect(POINT& pt) = 0;
	void					SetMat()	;	//	매트릭스 설정
public:
	O2GUIBase();
	virtual ~O2GUIBase();
	// 초기화
	virtual HRESULT Init(tstring filename
		,ResourceManager* pResource
		,LPDIRECT3DDEVICE9* pDev
		,POINT	imagesize
		,O2Rect pRect = O2Rect(0,0,0,0)
		,D3DXVECTOR3 pPos = D3DXVECTOR3(0.0f,0.0f,0.0f)
		,UINT		_Center=GUI_CENTER_CENTER
		) = 0;

	virtual void Render() = 0;
	virtual UINT Update(POINT& pt,UINT _MsState) = 0;	
};

//////////////////////////////////////////////////////////////////////////
// class : O2GUIObject
// 설명 : 기본 GUI 클래스
// 작성자 : 조성국
// 작성일 : 08 / 07 / 01
//////////////////////////////////////////////////////////////////////////
class O2GUIObject : public O2GUIBase
{
protected:
	
	LPDIRECT3DTEXTURE9			m_pTexture	;	//	텍스쳐	
	LPDIRECT3DVERTEXBUFFER9		m_lpVB		;	//	버텍스 버퍼
	UINT						m_vAlpha	;	//	그림 알파값
	UINT						m_uCenter	;	//	이미지 중심
	// 버튼안에 들어와있는지 설정
	virtual BOOL IsInRect(POINT& pt);
	HRESULT	CreateVB(UINT _Center,POINT	_ImgSize);
public:
	O2GUIObject();

	O2GUIObject(tstring p_FileName);
	virtual ~O2GUIObject();

	// GUI 생성
	HRESULT Init(tstring filename
		,ResourceManager* pResource
		,LPDIRECT3DDEVICE9* pDev
		,POINT	imagesize
		,O2Rect pRect = O2Rect(0,0,0,0)
		,D3DXVECTOR3 pPos = D3DXVECTOR3(0.0f,0.0f,0.0f)
		,UINT		_Center=GUI_CENTER_CENTER
		);
	
	// 그리기
	virtual void Render();
	virtual UINT Update(POINT& pt,UINT _MsState);

	// 텍스쳐에서의 그림위치 설정
	void SetRect(O2Rect p_Rect);
	void SetRect();
	// 화면에서의 그림 위치 설정
	void SetPos(D3DXVECTOR3 p_Pos) { m_vPos = p_Pos; D3DXMatrixTranslation(&m_matTrans,m_vPos.x,m_vPos.y,m_vPos.z); }
	void SetRotZ(float pRot)	{	D3DXMatrixRotationZ(&m_matRot,D3DXToRadian(pRot));	}
	// 파일 이름 설정
	void SetFileName(tstring p_Name) { m_tFileName = p_Name; }
	// 텍스쳐 알파값 설정
	void SetAlpha(UINT p_Alpha) { m_vAlpha = p_Alpha; }

	// 맴버 변수 반환 함수들
	const tstring GetFileName() const { return m_tFileName; }
	const O2Rect GetRect() const {return m_vRect; }
	const D3DXVECTOR3 GetPos() const { return m_vPos; }
	const UINT GetAlpha() const { return m_vAlpha; }

	void	SetTexture(LPDIRECT3DTEXTURE9 p_pTexture){ m_pTexture = p_pTexture;}
	LPDIRECT3DTEXTURE9	GetTexture(){ return m_pTexture;}

	void	SetValidateObject();
};