#include "stdafx.h"
#include "Arrow.h"

Arrow::Arrow()
:m_fCurSize(1.0f)
,m_fDir(-1.0f)
{

}

Arrow::~Arrow()
{

}

HRESULT Arrow::UpDate()
{
	m_fCurSize += (m_fDir * (*m_fpDelta));
	if( m_fCurSize <= 0.9f )
	{
		m_fDir = 1.0f;
		m_fCurSize = 0.9f;
	}else if(m_fCurSize >= 1.1f){
		m_fDir = -1.0f;
		m_fCurSize = 1.1f;
	}
	SetSize(m_fCurSize);

	return S_OK;
}