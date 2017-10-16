//////////////////////////////////////////////////////////////////////////
// O2GUIButton.h
// 설명 : 버튼 클래스
//			 버튼 ON / OFF 상태의 enum
// 작성자 : 조성국
// 작성일 : 08 / 07 / 01
//////////////////////////////////////////////////////////////////////////
#pragma once


//////////////////////////////////////////////////////////////////////////
// class : O2GUIButton
// 설명 : 버튼GUI 클래스
// 작성자 : 조성국
// 작성일 : 08 / 07 / 01
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
	// 버튼 체크
	UINT m_vCheckClick;
	// 버튼 0 꺼졌을때 1 눌렀을때
	O2GUIObject* m_pButtons[BUTTONMAX];
	// 버튼 스테이트.
	UINT m_vNextState;
	// 버튼안에 들어와있는지 설정
	virtual BOOL IsInRect(POINT& pt);
	POINT	m_pImgSize;
	// 버튼 올라가고 떨어질때 사운드 출력 
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
	// 버튼상태에 맞는 랜더
	virtual void Render();

	virtual UINT Update(POINT& pt,UINT _MsState);

	// Index
	void SetIndex(UINT p_Index) { m_vIndex = p_Index; }
	UINT GetIndex() const { return m_vIndex; }

	UINT GetOnOffState() {return m_vCheckClick;}

	// 버튼 On/Off일때 GUI설정
	HRESULT SetButton(tstring p_fileName
		,UINT p_State
		,O2Rect p_Rect=O2Rect(100,0,0,0)
		);

	// On / Off 설정
	void SetOnOff(UINT p_check) { m_vCheckClick = p_check; }
	// 버튼 Rect 설정 / 불러오기
	void SetRect(O2Rect p_Rect);

	const O2Rect GetRect() const { return m_vRect; }
	// 좌표 얻어오기
	void SetPos(D3DXVECTOR3 p_Pos);
	const D3DXVECTOR3 GetPos() const { return m_vPos; }
	// On / Off 얻어옴
	const UINT GetCheck() const { return m_vCheckClick; }
	void ChangeTexture();
};