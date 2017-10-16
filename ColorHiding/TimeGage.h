#pragma once

class TimeGage : public O2GUIBase{
private:
	virtual BOOL IsInRect(POINT& pt) { return false; }

	O2GUIObject*		m_pGage;					//	������ Ŀ���� ������ ��

	float*				m_fpDelta	;				//	�ð� ������
	float				m_fMaxTime	;				//	�ִ� �ð�
	float				m_fCurTime	;				//	���� �ð�

	O2Rect				m_vBarRect;					//	������ ���� �⺻ ��Ʈ
	D3DXVECTOR3			m_vBarPos;					//	������ ���� �⺻ ��ġ
public:
	TimeGage();
	~TimeGage();
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

	HRESULT	InitGageBar(tstring _filename,POINT imagesize ,O2Rect pRect ,D3DXVECTOR3 pPos);

	VOID	SetDelta(float* pDelta) { m_fpDelta = pDelta; }
	VOID	SetTime(float	pTime)	{ m_fCurTime = m_fMaxTime = pTime; }
	VOID	InitTimer();
};