#pragma once

#define CHECK_TRUE	1
#define CHECK_FALSE 0

class CheckBox : public O2GUIBase{
private:
	O2GUIObject*	m_pObjects	;
	BOOL*			m_bIsCheck	;
	POINT			m_pImgSize	;
	virtual BOOL IsInRect(POINT& pt) { return false; }
public:
	CheckBox();
	~CheckBox();
	// �׸���
	virtual void Render();
	virtual UINT Update(POINT& pt,UINT _MsState);
	// �ʱ�ȭ
	virtual HRESULT Init(tstring filename
		,ResourceManager* pResource
		,LPDIRECT3DDEVICE9* pDev
		,POINT	imagesize
		,O2Rect pRect = O2Rect(0,0,0,0)
		,D3DXVECTOR3 pPos = D3DXVECTOR3(0.0f,0.0f,0.0f)
		,UINT		_Center=GUI_CENTER_CENTER
		);

	HRESULT InitCheck(tstring p_fileName
		,BOOL p_State
		,O2Rect p_Rect
		);

	VOID	SetCheck(BOOL*	pcheck) { m_bIsCheck = pcheck; }
	BOOL*	GetCheck() { return m_bIsCheck; }
};