//////////////////////////////////////////////////////////////////////////
// JObject.h
// 기본 오브젝트 클래스
#pragma once

// 충돌 무시 가능 최대 값
#define SKINWIDTH	0.02f
// 이름 최대 길이
#define MAX_OBJNAME		125
// size 최소 값
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

// 전방 선언
class ResourceManager;


//////////////////////////////////////////////////////////////////////////
//	기본 Object
class O2Object{
protected:
	UINT					m_uType					;	// 오브젝트 속성 타입
	FLOAT*					m_fpDelta				;	// 보정 시간 ( 1/초 )
	LPDIRECT3DDEVICE9*		m_lpDevice				;	// 디바이스
	TCHAR					m_tsName[MAX_OBJNAME]	;	// 오브젝트 이름
	TCHAR					m_tsFile[MAX_OBJNAME]	;	// 오브젝트 파일 이름

	D3DXVECTOR3				m_vPos					;	// 오브젝트 위치
	D3DXVECTOR3				m_vRot					;	// 오브젝트 회전 값 angle
	D3DXVECTOR3				m_vSize					;	// 오브젝트 사이즈

	D3DXVECTOR3				m_vDir					;	// 오브젝트가 바라보는 방향
	D3DXVECTOR3				m_vUp					;	// 오브젝트의 수직 방향
	D3DXVECTOR3				m_vRight				;	// 오브젝트의 오른쪽 방향

	D3DXMATRIXA16			m_matTrans				;	// 오브젝트의 이동 행렬
	D3DXMATRIXA16			m_matSize				;	// 오브젝트의 사이즈 행렬
	D3DXMATRIXA16			m_matRot				;	// 오브젝트의 회전 행렬
	D3DXMATRIXA16			m_matRT					;	// 오브젝트의 회전/이동 행렬
	D3DXMATRIXA16			m_matWorld				;	// 오브젝트의 전체 변환 행렬(월드행렬)
	BOOL					m_bIsDead				;	//	죽었으면 지워준다

	O2Object*				m_pParent				;	// 오브젝트의 부모 오브젝트

	BOOL					m_bRendShadow			;	//	그림자를 그리는지
	BOOL					m_bReflectShadow		;	//	그림자를 받는지
	BOOL					m_bUseCellEffect		;	//	셀 쉐이딩을 사용하는지
	BOOL					m_bLineRend				;	//	외곽선 랜더를 하는지

	BOOL					m_bIsRend				;	//	이 오브젝트가 그려지는지

	// 오브젝트 정리
	virtual	VOID		CleanUp()	=	0;
	// 월드 행렬 설정
	virtual	VOID		SetMat();
	virtual VOID		SetMatRot();

	//////////////////////////////////////////////////////////////////////////
	// 임시
	D3DXMATRIXA16		m_matLiner;	//외곽을 위한 행렬(임시)
public:
	// 외곽선 그리기
	virtual HRESULT		RenderLine()=0;
	virtual HRESULT		RenderShadow(UINT pass)=0;
	//////////////////////////////////////////////////////////////////////////
	virtual BOOL		IsRender(O2Frustum* pFrustrum)=0;
public:
	static	ResourceManager*	s_pResource		;	//	리소스를 받아올 클래스
	static	VOID*				s_pManager		;	//	매니저 클래스

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
	// 회전
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
	// 움직임
	virtual	HRESULT		UpDate() = 0;
	// 그리기
	virtual	HRESULT		Render() = 0;
};