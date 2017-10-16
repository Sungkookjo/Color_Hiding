//////////////////////////////////////////////////////////////////////////
// O2GUIObject.h
// ���� : O2GUIObject Ŭ���� ���
// �ۼ��� : ������
// �ۼ��� : 08 / 07 / 01
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

// UI �ε���
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
	tstring					m_tFileName	;	//	���� �̸�	
	O2Rect					m_vRect		;	//	�ؽ��Ŀ��� �׸��� ��ġ	
	D3DXVECTOR3				m_vPos		;	//	ȭ�鿡�� �׸��� ��ġ
	LPDIRECT3DDEVICE9*		m_lpDevice	;	//	����̽�
	ResourceManager*		m_pResource	;	//	���ҽ� �ڽ�
	UINT					m_uType		;	//	gui Ÿ��
	D3DXMATRIXA16			m_matWorld	;	//	�������
	D3DXMATRIXA16			m_matRot	;	//	�������
	D3DXMATRIXA16			m_matScail	;	//	������ ���
	D3DXMATRIXA16			m_matTrans	;	//	������ ���
	virtual BOOL IsInRect(POINT& pt) = 0;
	void					SetMat()	;	//	��Ʈ���� ����
public:
	O2GUIBase();
	virtual ~O2GUIBase();
	// �ʱ�ȭ
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
// ���� : �⺻ GUI Ŭ����
// �ۼ��� : ������
// �ۼ��� : 08 / 07 / 01
//////////////////////////////////////////////////////////////////////////
class O2GUIObject : public O2GUIBase
{
protected:
	
	LPDIRECT3DTEXTURE9			m_pTexture	;	//	�ؽ���	
	LPDIRECT3DVERTEXBUFFER9		m_lpVB		;	//	���ؽ� ����
	UINT						m_vAlpha	;	//	�׸� ���İ�
	UINT						m_uCenter	;	//	�̹��� �߽�
	// ��ư�ȿ� �����ִ��� ����
	virtual BOOL IsInRect(POINT& pt);
	HRESULT	CreateVB(UINT _Center,POINT	_ImgSize);
public:
	O2GUIObject();

	O2GUIObject(tstring p_FileName);
	virtual ~O2GUIObject();

	// GUI ����
	HRESULT Init(tstring filename
		,ResourceManager* pResource
		,LPDIRECT3DDEVICE9* pDev
		,POINT	imagesize
		,O2Rect pRect = O2Rect(0,0,0,0)
		,D3DXVECTOR3 pPos = D3DXVECTOR3(0.0f,0.0f,0.0f)
		,UINT		_Center=GUI_CENTER_CENTER
		);
	
	// �׸���
	virtual void Render();
	virtual UINT Update(POINT& pt,UINT _MsState);

	// �ؽ��Ŀ����� �׸���ġ ����
	void SetRect(O2Rect p_Rect);
	void SetRect();
	// ȭ�鿡���� �׸� ��ġ ����
	void SetPos(D3DXVECTOR3 p_Pos) { m_vPos = p_Pos; D3DXMatrixTranslation(&m_matTrans,m_vPos.x,m_vPos.y,m_vPos.z); }
	void SetRotZ(float pRot)	{	D3DXMatrixRotationZ(&m_matRot,D3DXToRadian(pRot));	}
	// ���� �̸� ����
	void SetFileName(tstring p_Name) { m_tFileName = p_Name; }
	// �ؽ��� ���İ� ����
	void SetAlpha(UINT p_Alpha) { m_vAlpha = p_Alpha; }

	// �ɹ� ���� ��ȯ �Լ���
	const tstring GetFileName() const { return m_tFileName; }
	const O2Rect GetRect() const {return m_vRect; }
	const D3DXVECTOR3 GetPos() const { return m_vPos; }
	const UINT GetAlpha() const { return m_vAlpha; }

	void	SetTexture(LPDIRECT3DTEXTURE9 p_pTexture){ m_pTexture = p_pTexture;}
	LPDIRECT3DTEXTURE9	GetTexture(){ return m_pTexture;}

	void	SetValidateObject();
};