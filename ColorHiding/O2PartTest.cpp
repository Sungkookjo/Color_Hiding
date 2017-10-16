#include "stdafx.h"
#include "O2PartTest.h"


void O2PartTest::Init_Particles(LPDIRECT3DDEVICE9 _device)
{
	m_device=_device;
	Ftname=_T("particle/test.txt");
	Readtextfile();
	SetValues(m_device);
}
