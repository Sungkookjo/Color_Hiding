#include "stdafx.h"

// ���ҽ� �Ŵ��� static���� �����ߴ�.
ResourceManager* O2Object::s_pResource = NULL;
VOID* O2Object::s_pManager = NULL;

//////////////////////////////////////////////////////////////////////////
//	Object
//	������
O2Object::O2Object()
:m_lpDevice(NULL)
,m_fpDelta(NULL)
,m_pParent(NULL)
,m_vPos(0.0f,0.0f,0.0f)
,m_vRot(0.0f,0.0f,0.0f)
,m_vSize(1.0f,1.0f,1.0f)
,m_vDir(0.0f,0.0f,1.0f)
,m_vRight(1.0f,0.0f,0.0f)
,m_vUp(0.0f,1.0f,0.0f)
,m_uType(OBJ_TYPE_BASIC)
,m_bIsDead(FALSE)
,m_bRendShadow(TRUE)
,m_bUseCellEffect(TRUE)
,m_bReflectShadow(TRUE)
,m_bLineRend(TRUE)
,m_bIsRend(TRUE)
{
	_tcscpy_s(m_tsName,_T("_NoName_"));
	D3DXMatrixIdentity(&m_matSize);
	m_matWorld = m_matRT = m_matRot = m_matTrans = m_matSize;
}

//////////////////////////////////////////////////////////////////////////
//	�Ҹ���
O2Object::~O2Object()
{
	m_fpDelta	= NULL;
	m_lpDevice	= NULL;
	m_pParent	= NULL;
	_tcscpy_s(m_tsName,_T("_Deleted_"));
}

//////////////////////////////////////////////////////////////////////////
//	SetMat
//	������� ����
VOID O2Object::SetMat()
{
	D3DXMatrixTranslation(&m_matTrans,m_vPos.x,m_vPos.y,m_vPos.z);
	m_matWorld = m_matSize * m_matRot * m_matTrans;

	// �θ������ �ִ� ��� �����ش�.
	if(m_pParent){
		m_matWorld *= m_pParent->GetMatWorld();
	}

	// ������� ����
	(*m_lpDevice)->SetTransform( D3DTS_WORLD, &m_matWorld );
}

//////////////////////////////////////////////////////////////////////////
//	SetSize
//	size ����.
//	�ּ� ������ �̻����θ� ������ ���� ����
VOID O2Object::SetSize( const D3DXVECTOR3& _size )
{
	m_vSize.x = ( _size.x < SIZE_MIN ? SIZE_MIN : _size.x );
	m_vSize.y = ( _size.y < SIZE_MIN ? SIZE_MIN : _size.y );
	m_vSize.z = ( _size.z < SIZE_MIN ? SIZE_MIN : _size.z );
	D3DXMatrixScaling(&m_matSize,m_vSize.x,m_vSize.y,m_vSize.z);
}

VOID O2Object::SetSize( const FLOAT& _size )
{
	m_vSize.x = m_vSize.y = m_vSize.z = ( _size < SIZE_MIN ? SIZE_MIN : _size );
	D3DXMatrixScaling(&m_matSize,m_vSize.x,m_vSize.y,m_vSize.z);
}

//////////////////////////////////////////////////////////////////////////
//	AddSize
//	���� ����� �߰������� ����� ����
VOID O2Object::AddSize( const FLOAT& _size )
{
	m_vSize.x += ( _size+m_vSize.x < SIZE_MIN ? 0.0f : _size );
	m_vSize.y += ( _size+m_vSize.y < SIZE_MIN ? 0.0f : _size );
	m_vSize.z += ( _size+m_vSize.z < SIZE_MIN ? 0.0f : _size );
	D3DXMatrixScaling(&m_matSize,m_vSize.x,m_vSize.y,m_vSize.z);
}

VOID O2Object::AddSize( const D3DXVECTOR3& _size )
{	
	m_vSize.x += ( _size.x+m_vSize.x < SIZE_MIN ? 0.0f : _size.x );
	m_vSize.y += ( _size.y+m_vSize.y < SIZE_MIN ? 0.0f : _size.y );
	m_vSize.z += ( _size.z+m_vSize.z < SIZE_MIN ? 0.0f : _size.z );
	D3DXMatrixScaling(&m_matSize,m_vSize.x,m_vSize.y,m_vSize.z);
}

//////////////////////////////////////////////////////////////////////////
//	Rot X,Y,Z
//	X,Y,Z ������ ȸ��
//	���� ȸ������ + ��
VOID O2Object::RotX( float angle )
{
	m_vRot.x += angle;

	SetMatRot();
}

VOID O2Object::RotY( float angle )
{
	m_vRot.y += angle;

	SetMatRot();
}

VOID O2Object::RotZ( float angle )
{
	m_vRot.z += angle;

	SetMatRot();
}
// Rot X,Y,Z
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//	SetMatRot()
//	ȸ�� ��� ��ȯ
VOID O2Object::SetMatRot()
{
	// ȸ�� ��� ��ȯ
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationX(&matRot,D3DXToRadian(m_vRot.x));
	m_matRot = matRot;
	D3DXMatrixRotationY(&matRot,D3DXToRadian(m_vRot.y));
	m_matRot *= matRot;
	D3DXMatrixRotationZ(&matRot,D3DXToRadian(m_vRot.z));
	m_matRot *= matRot;
	
	// ��ȯ�� ȸ�� ��Ŀ��� �� ���� ���͸� ����
	D3DXVec3TransformCoord(&m_vDir	,&D3DXVECTOR3(0.0f,0.0f,1.0f),&m_matRot);
	D3DXVec3TransformCoord(&m_vUp	,&D3DXVECTOR3(0.0f,1.0f,0.0f),&m_matRot);
	D3DXVec3TransformCoord(&m_vRight,&D3DXVECTOR3(1.0f,0.0f,0.0f),&m_matRot);
}