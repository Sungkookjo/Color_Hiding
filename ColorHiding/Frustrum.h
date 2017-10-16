#pragma  once

#define PLANE_EPSILON 1.0f 

class O2Frustum
{
private:
	D3DXVECTOR3 m_vtx[8];  //���������� ������ ���� 8��.
	D3DXVECTOR3 m_vCameraPos; // ���� ī�޶��� ���� ��ǥ.. ��Ȯ ���� �ʴ�.
	D3DXPLANE   m_plane[6]; // �������� 6�� ���.. 2���� ������� �ʴ´�.

public:
	O2Frustum();

	~O2Frustum();

	// ī�޶� (view ) *  �������� (projection) ���..
	bool Make(D3DXMATRIXA16* pmatViewProj);

	/// ���� v�� �������Ҿȿ� ������ TRUE�� ��ȯ, �ƴϸ� FALSE�� ��ȯ�Ѵ�.
	BOOL IsInPoint( D3DXVECTOR3* pv );


	/** �߽�(v)�� ������(radius)�� ���� ��豸(bounding sphere)�� �������Ҿȿ� ������
	*  TRUE�� ��ȯ, �ƴϸ� FALSE�� ��ȯ�Ѵ�.
	*/
	bool IsInSphere( D3DXVECTOR3* pv, float radius );
};