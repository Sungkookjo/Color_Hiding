#include "stdafx.h"

bool ComputeBounding(LPD3DXMESH* mesh, O2CollisionInfo* Coll)
{
	HRESULT hr = 0;

	BYTE* v = 0;

	DWORD size = (*mesh)->GetNumVertices();
	UINT FvFsize = D3DXGetFVFVertexSize((*mesh)->GetFVF());

	(*mesh)->LockVertexBuffer(0, (void**)&v);

	// 경계 구 얻어오기
	hr = D3DXComputeBoundingSphere(
		(D3DXVECTOR3*)v,
		size,
		FvFsize,
		&Coll->_center,
		&Coll->_radius);

	// 실패시
	if( FAILED(hr) ){
		(*mesh)->UnlockVertexBuffer();
		return false;
	}

	// 경계 박스 얻어오기
	hr = D3DXComputeBoundingBox(
		(D3DXVECTOR3*)v,
		size,
		FvFsize,
		&Coll->_min,
		&Coll->_max);

	(*mesh)->UnlockVertexBuffer();
	if( FAILED(hr) )
		return false;

	return true;
}


//////////////////////////////////////////////////////////////////////////
// CollisionInfo
// 생성자
O2CollisionInfo::O2CollisionInfo()
{
	_center = D3DXVECTOR3(0.0f,0.0f,0.0f);
	_radius = 0.0f;
	_min = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
	_max = D3DXVECTOR3(FLT_MIN,FLT_MIN,FLT_MIN);
}

//////////////////////////////////////////////////////////////////////////
// IsPointInside
// 포인트(p)가 collisonBox 안에 있는지 검사
// 충돌 true
bool O2CollisionInfo::isPointInside( D3DXVECTOR3& p )
{
	if( p.x >= _min.x && p.y >= _min.y && p.z >= _min.z &&
		p.x <= _max.x && p.y <= _max.y && p.z <= _max.z )
	{
		return true;
	}
	else
	{
		return false;
	}
}



bool operator<(wstring& _Left, wstring& _Right)
{
	return false;
}

D3DXVECTOR3 NxVec3toDxVec3( NxVec3& _Right )
{
	return D3DXVECTOR3(_Right.x,_Right.y,_Right.z);
}

D3DXVECTOR3 NxVec3toDxVec3( const NxVec3& _Right )
{
	return D3DXVECTOR3(_Right.x,_Right.y,_Right.z);
}
//NxVec3 DxVec3toNxVec3( D3DXVECTOR3& _Right )
//{
//	return NxVec3(_Right.x,_Right.y,_Right.z);
//}
//
//NxVec3 DxVec3toNxVec3( const D3DXVECTOR3& _Right )
//{
//	return NxVec3(_Right.x,_Right.y,_Right.z);
//}

O2Rect::O2Rect()
{
	left = 0;
	right = 0;
	top = 0;
	bottom = 0;
}

O2Rect::O2Rect(LONG p_left,LONG p_right,LONG p_top,LONG p_bottom)
{
	left = p_left;
	right = p_right;
	top = p_top;
	bottom = p_bottom;
}

O2Rect::~O2Rect()
{
}

BOOL O2Rect::IsInRect(int px, int py)
{
	return !(px < left || px > right || py < top || py > bottom);
}

BOOL O2Rect::IsInRect(POINT pt)
{
	return PtInRect(this,pt);
}
