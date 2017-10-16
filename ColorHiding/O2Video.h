#pragma once

#include <rpcsal.h>
#include <Vmr9.h>

HRESULT InitWindowlessVMR9(IBaseFilter *pVMR, HWND hwnd, IVMRWindowlessControl9 ** ppWc); 
HRESULT InitWindowlessVMR(IBaseFilter *pVMR, HWND hwnd, IVMRWindowlessControl** ppWc); 

//-----------------------------------------------------------------------------
// BaseVideoRenderer
//
// Abstract class to manage the video renderer filter.
// Specific implementations handle the VMR-7, VMR-9, or EVR filter.
//-----------------------------------------------------------------------------

class BaseVideoRenderer
{
public:
	virtual ~BaseVideoRenderer() { }

	virtual BOOL    HasVideo() const = 0;

	virtual HRESULT AddToGraph(IGraphBuilder *pGraph, HWND hwnd) = 0;
	virtual HRESULT FinalizeGraph(IGraphBuilder *pGraph) = 0;

	virtual HRESULT UpdateVideoWindow(HWND hwnd, const LPRECT prc) = 0;
	virtual HRESULT Repaint(HWND hwnd, HDC hdc) = 0;
	virtual HRESULT DisplayModeChanged() = 0;
};

//-----------------------------------------------------------------------------
// VMR7
//
// Manages the VMR-7 video renderer filter.
//-----------------------------------------------------------------------------

class VMR7 : public BaseVideoRenderer
{
	IVMRWindowlessControl	*m_pWindowless;

public:

	VMR7();
	~VMR7();

	BOOL    HasVideo() const { return (m_pWindowless != NULL); }

	HRESULT AddToGraph(IGraphBuilder *pGraph, HWND hwnd);
	HRESULT FinalizeGraph(IGraphBuilder *pGraph);

	HRESULT UpdateVideoWindow(HWND hwnd, const LPRECT prc);
	HRESULT Repaint(HWND hwnd, HDC hdc);
	HRESULT DisplayModeChanged();
};


//-----------------------------------------------------------------------------
// VMR9
//
// Manages the VMR-9 video renderer filter.
//-----------------------------------------------------------------------------

class VMR9 : public BaseVideoRenderer
{
	IVMRWindowlessControl9 *m_pWindowless;

public:

	VMR9();
	~VMR9();

	BOOL    HasVideo() const { return (m_pWindowless != NULL); }

	HRESULT AddToGraph(IGraphBuilder *pGraph, HWND hwnd);
	HRESULT FinalizeGraph(IGraphBuilder *pGraph);

	HRESULT UpdateVideoWindow(HWND hwnd, const LPRECT prc);
	HRESULT Repaint(HWND hwnd, HDC hdc);
	HRESULT DisplayModeChanged();
};