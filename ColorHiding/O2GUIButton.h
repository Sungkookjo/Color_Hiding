//////////////////////////////////////////////////////////////////////////
// O2GUIButton.h
// ���� : ��ư Ŭ����
//			 ��ư ON / OFF ������ enum
// �ۼ��� : ������
// �ۼ��� : 08 / 07 / 01
//////////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////////////////////////////////////////////
// class : O2GUIButton
// ���� : ��ưGUI Ŭ����
// �ۼ��� : ������
// �ۼ��� : 08 / 07 / 01
//////////////////////////////////////////////////////////////////////////
enum BUTTONSTATE{NORMAL=0
,ON
,CLICK
,BUTTONMAX};

class Manager;

class O2GUIButton : public O2GUIBase
{
protected:
	UINT m_vIndex;
	// ��ư üũ
	UINT m_vCheckClick;
	// ��ư 0 �������� 1 ��������
	O2GUIObject* m_pButtons[BUTTONMAX];
	// ��ư ������Ʈ.
	UINT m_vNextState;
	// ��ư�ȿ� �����ִ��� ����
	virtual BOOL IsInRect(POINT& pt);
	POINT	m_pImgSize;
	// ��ư �ö󰡰� �������� ���� ��� 
	BOOL m_bUpButtonSound;
public:
	O2GUIButton();
	O2GUIButton(O2Rect p_Rect);
	O2GUIButton(O2GUIButton& p_Button);
	~O2GUIButton();

	virtual HRESULT Init(tstring filename
		,ResourceManager* pResource
		,LPDIRECT3DDEVICE9* pDev
		,POINT	imgSize
		,O2Rect pRect = O2Rect(0,0,0,0)
		,D3DXVECTOR3 pPos = D3DXVECTOR3(0.0f,0.0f,0.0f)
		,UINT		_Center=GUI_CENTER_CENTER
		);
	// ��ư���¿� �´� ����
	virtual void Render();

	virtual UINT Update(POINT& pt,UINT _MsState);

	// Index
	void SetIndex(UINT p_Index) { m_vIndex = p_Index; }
	UINT GetIndex() const { return m_vIndex; }

	UINT GetOnOffState() {return m_vCheckClick;}

	// ��ư On/Off�϶� GUI����
	HRESULT SetButton(tstring p_fileName
		,UINT p_State
		,O2Rect p_Rect=O2Rect(100,0,0,0)
		);

	// On / Off ����
	void SetOnOff(UINT p_check) { m_vCheckClick = p_check; }
	// ��ư Rect ���� / �ҷ�����
	void SetRect(O2Rect p_Rect);

	const O2Rect GetRect() const { return m_vRect; }
	// ��ǥ ������
	void SetPos(D3DXVECTOR3 p_Pos);
	const D3DXVECTOR3 GetPos() const { return m_vPos; }
	// On / Off ����
	const UINT GetCheck() const { return m_vCheckClick; }
	void ChangeTexture();
};