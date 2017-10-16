#include "stdafx.h"

O2Frustum::O2Frustum()
{

}
O2Frustum::~O2Frustum()
{

}

// ī�޶� (vier ) *  �������� (projection) ���..
bool O2Frustum::Make(D3DXMATRIXA16* pmatViewProj)
{
	int				i;
	D3DXMATRIXA16	matInv;

	// ������ı��� ��ġ�� ��� 3���� ������ǥ�� ���� (-1,-1,0) ~ (1,1,1)������ ������ �ٲ��.
	// m_vtx�� �� ���������� ��谪�� �־�д�.
	m_vtx[0].x = -1.0f;	m_vtx[0].y = -1.0f;	m_vtx[0].z = 0.0f;
	m_vtx[1].x =  1.0f;	m_vtx[1].y = -1.0f;	m_vtx[1].z = 0.0f;
	m_vtx[2].x =  1.0f;	m_vtx[2].y = -1.0f;	m_vtx[2].z = 1.0f;
	m_vtx[3].x = -1.0f;	m_vtx[3].y = -1.0f;	m_vtx[3].z = 1.0f;
	m_vtx[4].x = -1.0f;	m_vtx[4].y =  1.0f;	m_vtx[4].z = 0.0f;
	m_vtx[5].x =  1.0f;	m_vtx[5].y =  1.0f;	m_vtx[5].z = 0.0f;
	m_vtx[6].x =  1.0f;	m_vtx[6].y =  1.0f;	m_vtx[6].z = 1.0f;
	m_vtx[7].x = -1.0f;	m_vtx[7].y =  1.0f;	m_vtx[7].z = 1.0f;

	//View * proj�� �����
	D3DXMatrixInverse(&matInv, NULL, pmatViewProj );

	for( i = 0; i < 8; i++ )
		D3DXVec3TransformCoord( &m_vtx[i], &m_vtx[i], &matInv );

	m_vCameraPos = ( m_vtx[0] + m_vtx[5] ) / 2.0f;

	// ����� ������ǥ�� �������� ����� �����
	// ���Ͱ� �������� ���ʿ��� �ٱ������� ������ �����̴�.
	D3DXPlaneFromPoints(&m_plane[0], &m_vtx[4], &m_vtx[7], &m_vtx[6]);	// �� ���(top)
	D3DXPlaneFromPoints(&m_plane[1], &m_vtx[0]  , &m_vtx[1], &m_vtx[2]);	// �� ���(bottom)
	D3DXPlaneFromPoints(&m_plane[2], &m_vtx[0]  , &m_vtx[4], &m_vtx[5]);	// �� ���(near)
	//		D3DXPlaneFromPoints(&m_plane[3], m_vtx[2], m_vtx[6], m_vtx[7]);	// �� ���(far)
	D3DXPlaneFromPoints(&m_plane[4], &m_vtx[0]  , &m_vtx[3], &m_vtx[7]);	// �� ���(left)
	D3DXPlaneFromPoints(&m_plane[5], &m_vtx[1], &m_vtx[5], &m_vtx[6]);	// �� ���(right)

	return true;
}

/// ���� v�� �������Ҿȿ� ������ TRUE�� ��ȯ, �ƴϸ� FALSE�� ��ȯ�Ѵ�.
BOOL O2Frustum::IsInPoint( D3DXVECTOR3* pv )
{
	float		fDist;

	fDist = D3DXPlaneDotCoord( &m_plane[0], pv );  //��
	if( fDist > PLANE_EPSILON ) return FALSE;	

	fDist = D3DXPlaneDotCoord( &m_plane[1], pv ); //��
	if( fDist > PLANE_EPSILON ) return FALSE;	

	fDist = D3DXPlaneDotCoord( &m_plane[2], pv ); //��
	if( fDist > PLANE_EPSILON ) return FALSE;	

	fDist = D3DXPlaneDotCoord( &m_plane[4], pv );  //��
	if( fDist > PLANE_EPSILON ) return FALSE;

	fDist = D3DXPlaneDotCoord( &m_plane[5], pv );  //��
	if( fDist > PLANE_EPSILON ) return FALSE;


	return TRUE;
}

/** �߽�(v)�� ������(radius)�� ���� ��豸(bounding sphere)�� �������Ҿȿ� ������
*  TRUE�� ��ȯ, �ƴϸ� FALSE�� ��ȯ�Ѵ�.
*/
bool O2Frustum::IsInSphere( D3DXVECTOR3* pv, float radius )
{
	float		fDist;

	fDist = D3DXPlaneDotCoord( &m_plane[0], pv );  //�����.
	if( fDist > (radius+PLANE_EPSILON) ) return FALSE;	

	fDist = D3DXPlaneDotCoord( &m_plane[1], pv ); //�� 
	if( fDist > (radius+PLANE_EPSILON) ) return FALSE;

	fDist = D3DXPlaneDotCoord( &m_plane[2], pv ); //�� 
	if( fDist > (radius+PLANE_EPSILON) ) return FALSE;

	fDist = D3DXPlaneDotCoord( &m_plane[4], pv ); //��
	if( fDist > (radius+PLANE_EPSILON) ) return FALSE;

	fDist = D3DXPlaneDotCoord( &m_plane[5], pv );  //��
	if( fDist > (radius+PLANE_EPSILON) ) return FALSE;


	return TRUE;
}