//////////////////////////////////////////////////////////////////////////
// JObject.h
// �⺻ ������Ʈ Ŭ����
#pragma once

// �浹 ���� ���� �ִ� ��
#define SKINWIDTH	0.02f
// �̸� �ִ� ����
#define MAX_OBJNAME		125
// size �ּ� ��
#define SIZE_MIN	0.01f


#define OBJ_TYPE_BASIC		0x000000
#define OBJ_TYPE_2D			0x000001
#define OBJ_TYPE_3D			0x000010
#define OBJ_TYPE_PASSIVE	0x000100
#define OBJ_TYPE_ACTIVE		0x001000
#define OBJ_TYPE_BACK		0x010000
#define OBJ_TYPE_COLL		Ox100000
#define OBJ_TYPE_ITEM		0x0000ff
#define	OBJ_TYPE_CAPSULE	0x000ff0


#define COLOR_NONE	5
#define COLOR_G		0
#define COLOR_R		1
#define COLOR_B		2
#define COLOR_P		3
#define COLOR_MAX	4

#define COLOR_R_VAL		D3DXVECTOR4(0.898f,0.094f,0.188f,1.0f)
#define COLOR_G_VAL		D3DXVECTOR4(0.407f,0.643f,0.2f,1.0f)
#define COLOR_B_VAL		D3DXVECTOR4(0.113f,0.450f,0.733f,1.0f)
#define COLOR_P_VAL		D3DXVECTOR4(0.996f,0.839f,0.0f,1.0f)
#define COLOR_S_VAL		D3DXVECTOR4(1.0f,1.0f,1.0f,0.1f)

// ���� ����
class ResourceManager;


//////////////////////////////////////////////////////////////////////////
//	�⺻ Object
class O2Object{
protected:
	UINT					m_uType					;	// ������Ʈ �Ӽ� Ÿ��
	FLOAT*					m_fpDelta				;	// ���� �ð� ( 1/�� )
	LPDIRECT3DDEVICE9*		m_lpDevice				;	// ����̽�
	TCHAR					m_tsName[MAX_OBJNAME]	;	// ������Ʈ �̸�
	TCHAR					m_tsFile[MAX_OBJNAME]	;	// ������Ʈ ���� �̸�

	D3DXVECTOR3				m_vPos					;	// ������Ʈ ��ġ
	D3DXVECTOR3				m_vRot					;	// ������Ʈ ȸ�� �� angle
	D3DXVECTOR3				m_vSize					;	// ������Ʈ ������

	D3DXVECTOR3				m_vDir					;	// ������Ʈ�� �ٶ󺸴� ����
	D3DXVECTOR3				m_vUp					;	// ������Ʈ�� ���� ����
	D3DXVECTOR3				m_vRight				;	// ������Ʈ�� ������ ����

	D3DXMATRIXA16			m_matTrans				;	// ������Ʈ�� �̵� ���
	D3DXMATRIXA16			m_matSize				;	// ������Ʈ�� ������ ���
	D3DXMATRIXA16			m_matRot				;	// ������Ʈ�� ȸ�� ���
	D3DXMATRIXA16			m_matRT					;	// ������Ʈ�� ȸ��/�̵� ���
	D3DXMATRIXA16			m_matWorld				;	// ������Ʈ�� ��ü ��ȯ ���(�������)
	BOOL					m_bIsDead				;	//	�׾����� �����ش�

	O2Object*				m_pParent				;	// ������Ʈ�� �θ� ������Ʈ

	BOOL					m_bRendShadow			;	//	�׸��ڸ� �׸�����
	BOOL					m_bReflectShadow		;	//	�׸��ڸ� �޴���
	BOOL					m_bUseCellEffect		;	//	�� ���̵��� ����ϴ���
	BOOL					m_bLineRend				;	//	�ܰ��� ������ �ϴ���

	BOOL					m_bIsRend				;	//	�� ������Ʈ�� �׷�������

	// ������Ʈ ����
	virtual	VOID		CleanUp()	=	0;
	// ���� ��� ����
	virtual	VOID		SetMat();
	virtual VOID		SetMatRot();

	//////////////////////////////////////////////////////////////////////////
	// �ӽ�
	D3DXMATRIXA16		m_matLiner;	//�ܰ��� ���� ���(�ӽ�)
public:
	// �ܰ��� �׸���
	virtual HRESULT		RenderLine()=0;
	virtual HRESULT		RenderShadow(UINT pass)=0;
	//////////////////////////////////////////////////////////////////////////
	virtual BOOL		IsRender(O2Frustum* pFrustrum)=0;
public:
	static	ResourceManager*	s_pResource		;	//	���ҽ��� �޾ƿ� Ŭ����
	static	VOID*				s_pManager		;	//	�Ŵ��� Ŭ����

	O2Object();
	virtual ~O2Object();

	//////////////////////////////////////////////////////////////////////////
	// Get
	FLOAT*						GetDelta()		{	return m_fpDelta;	}
	LPDIRECT3DDEVICE9*			GetDevice()		{	return m_lpDevice;	}
	TCHAR*						GetName()		{	return m_tsName;	}
	TCHAR*						GetFileName()	{	return m_tsFile;	}
	O2Object*					GetParent()		{	return m_pParent;	}
	UINT						GetObjType()	{	return m_uType;		}

	D3DXVECTOR3&				GetPos()		{	return m_vPos;		}
	D3DXVECTOR3&				GetRot()		{	return m_vRot;		}
	D3DXVECTOR3&				GetSize()		{	return m_vSize;		}

	D3DXVECTOR3&				GetDir()		{	return m_vDir;		}
	D3DXVECTOR3&				GetUp()			{	return m_vUp;		}
	D3DXVECTOR3&				GetRight()		{	return m_vRight;	}

	D3DXMATRIXA16&				GetMatTrans()	{	return m_matTrans;	}
	D3DXMATRIXA16&				GetMatSize()	{	return m_matSize;	}
	D3DXMATRIXA16&				GetMatRot()		{	return m_matRot;	}
	D3DXMATRIXA16&				GetMatRT()		{	return m_matRT;		}
	D3DXMATRIXA16&				GetMatWorld()	{	return m_matWorld;	}
	BOOL						GetIsDead()		{	return m_bIsDead;	}
	BOOL						GetIsRend()		{	return m_bIsRend;	}

	//////////////////////////////////////////////////////////////////////////
	// Set
	VOID			SetDelta(FLOAT* pDelta)				{	m_fpDelta	=	pDelta;		}
	VOID			SetDevice(LPDIRECT3DDEVICE9* lpDev)	{	m_lpDevice=lpDev;			}
	VOID			SetName(TCHAR* pName)				{	_tcscpy_s(m_tsName,pName);	}
	VOID			SetFileName(TCHAR* pFile)			{	_tcscpy_s(m_tsFile,pFile);	}
	VOID			SetParent(O2Object*	pObj)			{	m_pParent = pObj;			}
	VOID			SetIsDead(BOOL _IsDead)				{	m_bIsDead = _IsDead;		}
	VOID			SetIsRend(BOOL _IsRend)				{	m_bIsRend = _IsRend;		}

	virtual	VOID	SetPos(const D3DXVECTOR3& pPos)			{	m_vPos	=	pPos;	}
	virtual	VOID	SetRot(const D3DXVECTOR3& pRot)			{	m_vRot	=	pRot;	}
	virtual	VOID	SetSize(const D3DXVECTOR3& pSize);
	virtual	VOID	SetSize(const FLOAT& _size);

	virtual	VOID	SetDir(const D3DXVECTOR3& pDir)			{	m_vDir	=	pDir;	}
	virtual	VOID	SetUp(const D3DXVECTOR3& pUp)			{	m_vUp	=	pUp;	}
	virtual	VOID	SetRight(const D3DXVECTOR3& pRight)		{	m_vRight=	pRight;	}

	//////////////////////////////////////////////////////////////////////////
	// ȸ��
	VOID						RotX(float angle);
	VOID						RotY(float angle);
	VOID						RotZ(float angle);
	
	//////////////////////////////////////////////////////////////////////////
	// Add
	virtual	VOID	AddPos(const D3DXVECTOR3& pPos)			{	m_vPos	+=	pPos;	}
	virtual	VOID	AddRot(const D3DXVECTOR3& pRot)			{	m_vRot	+=	pRot;	}
	virtual	VOID	AddSize(const FLOAT& _size);
	virtual VOID	AddSize(const D3DXVECTOR3& _size);

	VOID					SetRendShadow(BOOL pBool)	{ m_bRendShadow = pBool;	}
	VOID					SetShadowReflect(BOOL pBool){ m_bReflectShadow = pBool; }
	VOID					SetUseCell(BOOL pBool)		{ m_bUseCellEffect = pBool; }


	BOOL GetShadowRend()				{ return m_bRendShadow;		}
	BOOL GetShadowReflect()				{ return m_bReflectShadow;	}
	BOOL GetCellRend()					{ return m_bUseCellEffect;	}
	BOOL GetRendLine()					{ return m_bLineRend;		}
public:
	// ������
	virtual	HRESULT		UpDate() = 0;
	// �׸���
	virtual	HRESULT		Render() = 0;
};