#pragma once

class O2GUIOnce:public O2GUIObject
{
protected:
	float		m_fdrawtime;
public:
	O2GUIOnce();
	~O2GUIOnce();

	void		SetDrawTime(float _time) { m_fdrawtime = _time; }
	float		GetDrawTime() { return m_fdrawtime; }
};