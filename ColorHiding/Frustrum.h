#pragma  once

#define PLANE_EPSILON 1.0f 

class O2Frustum
{
private:
	D3DXVECTOR3 m_vtx[8];  //프러스텀을 구성할 정점 8개.
	D3DXVECTOR3 m_vCameraPos; // 현재 카메라의 월드 좌표.. 정확 하지 않다.
	D3DXPLANE   m_plane[6]; // 프러스텀 6개 평면.. 2개는 사용하지 않는다.

public:
	O2Frustum();

	~O2Frustum();

	// 카메라 (view ) *  프로젝션 (projection) 행렬..
	bool Make(D3DXMATRIXA16* pmatViewProj);

	/// 한점 v가 프러스텀안에 있으면 TRUE를 반환, 아니면 FALSE를 반환한다.
	BOOL IsInPoint( D3DXVECTOR3* pv );


	/** 중심(v)와 반지름(radius)를 갖는 경계구(bounding sphere)가 프러스텀안에 있으면
	*  TRUE를 반환, 아니면 FALSE를 반환한다.
	*/
	bool IsInSphere( D3DXVECTOR3* pv, float radius );
};