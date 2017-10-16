#include "stdafx.h"

O2Frustum::O2Frustum()
{

}
O2Frustum::~O2Frustum()
{

}

// 카메라 (vier ) *  프로젝션 (projection) 행렬..
bool O2Frustum::Make(D3DXMATRIXA16* pmatViewProj)
{
	int				i;
	D3DXMATRIXA16	matInv;

	// 투영행렬까지 거치면 모든 3차원 월드좌표의 점은 (-1,-1,0) ~ (1,1,1)사이의 값으로 바뀐다.
	// m_vtx에 이 동차공간의 경계값을 넣어둔다.
	m_vtx[0].x = -1.0f;	m_vtx[0].y = -1.0f;	m_vtx[0].z = 0.0f;
	m_vtx[1].x =  1.0f;	m_vtx[1].y = -1.0f;	m_vtx[1].z = 0.0f;
	m_vtx[2].x =  1.0f;	m_vtx[2].y = -1.0f;	m_vtx[2].z = 1.0f;
	m_vtx[3].x = -1.0f;	m_vtx[3].y = -1.0f;	m_vtx[3].z = 1.0f;
	m_vtx[4].x = -1.0f;	m_vtx[4].y =  1.0f;	m_vtx[4].z = 0.0f;
	m_vtx[5].x =  1.0f;	m_vtx[5].y =  1.0f;	m_vtx[5].z = 0.0f;
	m_vtx[6].x =  1.0f;	m_vtx[6].y =  1.0f;	m_vtx[6].z = 1.0f;
	m_vtx[7].x = -1.0f;	m_vtx[7].y =  1.0f;	m_vtx[7].z = 1.0f;

	//View * proj의 역행렬
	D3DXMatrixInverse(&matInv, NULL, pmatViewProj );

	for( i = 0; i < 8; i++ )
		D3DXVec3TransformCoord( &m_vtx[i], &m_vtx[i], &matInv );

	m_vCameraPos = ( m_vtx[0] + m_vtx[5] ) / 2.0f;

	// 얻어진 월드좌표로 프러스텀 평면을 만든다
	// 벡터가 프러스텀 안쪽에서 바깥쪽으로 나가는 평면들이다.
	D3DXPlaneFromPoints(&m_plane[0], &m_vtx[4], &m_vtx[7], &m_vtx[6]);	// 상 평면(top)
	D3DXPlaneFromPoints(&m_plane[1], &m_vtx[0]  , &m_vtx[1], &m_vtx[2]);	// 하 평면(bottom)
	D3DXPlaneFromPoints(&m_plane[2], &m_vtx[0]  , &m_vtx[4], &m_vtx[5]);	// 근 평면(near)
	//		D3DXPlaneFromPoints(&m_plane[3], m_vtx[2], m_vtx[6], m_vtx[7]);	// 원 평면(far)
	D3DXPlaneFromPoints(&m_plane[4], &m_vtx[0]  , &m_vtx[3], &m_vtx[7]);	// 좌 평면(left)
	D3DXPlaneFromPoints(&m_plane[5], &m_vtx[1], &m_vtx[5], &m_vtx[6]);	// 우 평면(right)

	return true;
}

/// 한점 v가 프러스텀안에 있으면 TRUE를 반환, 아니면 FALSE를 반환한다.
BOOL O2Frustum::IsInPoint( D3DXVECTOR3* pv )
{
	float		fDist;

	fDist = D3DXPlaneDotCoord( &m_plane[0], pv );  //상
	if( fDist > PLANE_EPSILON ) return FALSE;	

	fDist = D3DXPlaneDotCoord( &m_plane[1], pv ); //하
	if( fDist > PLANE_EPSILON ) return FALSE;	

	fDist = D3DXPlaneDotCoord( &m_plane[2], pv ); //근
	if( fDist > PLANE_EPSILON ) return FALSE;	

	fDist = D3DXPlaneDotCoord( &m_plane[4], pv );  //좌
	if( fDist > PLANE_EPSILON ) return FALSE;

	fDist = D3DXPlaneDotCoord( &m_plane[5], pv );  //우
	if( fDist > PLANE_EPSILON ) return FALSE;


	return TRUE;
}

/** 중심(v)와 반지름(radius)를 갖는 경계구(bounding sphere)가 프러스텀안에 있으면
*  TRUE를 반환, 아니면 FALSE를 반환한다.
*/
bool O2Frustum::IsInSphere( D3DXVECTOR3* pv, float radius )
{
	float		fDist;

	fDist = D3DXPlaneDotCoord( &m_plane[0], pv );  //상평면.
	if( fDist > (radius+PLANE_EPSILON) ) return FALSE;	

	fDist = D3DXPlaneDotCoord( &m_plane[1], pv ); //하 
	if( fDist > (radius+PLANE_EPSILON) ) return FALSE;

	fDist = D3DXPlaneDotCoord( &m_plane[2], pv ); //근 
	if( fDist > (radius+PLANE_EPSILON) ) return FALSE;

	fDist = D3DXPlaneDotCoord( &m_plane[4], pv ); //좌
	if( fDist > (radius+PLANE_EPSILON) ) return FALSE;

	fDist = D3DXPlaneDotCoord( &m_plane[5], pv );  //우
	if( fDist > (radius+PLANE_EPSILON) ) return FALSE;


	return TRUE;
}