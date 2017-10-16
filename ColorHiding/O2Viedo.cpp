#include "stdafx.h"
#include "DShowUlt.h"
#include "O2Video.h"

/// VMR-7 Wrapper

VMR7::VMR7() : m_pWindowless(NULL)
{

}


VMR7::~VMR7()
{
	SAFE_RELEASE(m_pWindowless);
}

HRESULT VMR7::AddToGraph(IGraphBuilder *pGraph, HWND hwnd)
{
	HRESULT hr = S_OK;

	IBaseFilter *pVMR = NULL;

	hr = AddFilterByCLSID(pGraph, CLSID_VideoMixingRenderer, &pVMR, L"VMR-7");

	// Set windowless mode on the VMR. This must be done before the VMR is connected.
	if (SUCCEEDED(hr))
	{
		hr = InitWindowlessVMR(pVMR, hwnd, &m_pWindowless);
	}

	return hr;
}


HRESULT VMR7::FinalizeGraph(IGraphBuilder *pGraph)
{
	if (m_pWindowless == NULL)
	{
		return S_OK;
	}

	HRESULT hr = S_OK;
	BOOL bRemoved = FALSE;

	IBaseFilter *pFilter = NULL;

	hr = m_pWindowless->QueryInterface(__uuidof(IBaseFilter), (void**)&pFilter);

	if (SUCCEEDED(hr))
	{
		hr = RemoveUnconnectedRenderer(pGraph, pFilter, &bRemoved);

		// If we removed the VMR, then we also need to release our 
		// pointer to the VMR's windowless control interface.
		if (bRemoved)
		{
			SAFE_RELEASE(m_pWindowless);
		}
	}

	SAFE_RELEASE(pFilter);
	return hr;
}

HRESULT VMR7::UpdateVideoWindow(HWND hwnd, const LPRECT prc)
{
	HRESULT hr = S_OK;

	if (m_pWindowless == NULL)
	{
		return S_OK; // no-op
	}

	if (prc)
	{
		hr = m_pWindowless->SetVideoPosition(NULL, prc);
	}
	else
	{

		RECT rc;
		GetClientRect(hwnd, &rc);
		hr = m_pWindowless->SetVideoPosition(NULL, &rc);
	}
	return hr;
}

HRESULT VMR7::Repaint(HWND hwnd, HDC hdc)
{
	HRESULT hr = S_OK;

	if (m_pWindowless)
	{
		hr = m_pWindowless->RepaintVideo(hwnd, hdc);
	}
	return hr;
}

HRESULT VMR7::DisplayModeChanged()
{
	HRESULT hr = S_OK;

	if (m_pWindowless)
	{
		hr = m_pWindowless->DisplayModeChanged();
	}
	return hr;


}


//-----------------------------------------------------------------------------
// InitWindowlessVMR
// Initialize the VMR-7 for windowless mode. 
//-----------------------------------------------------------------------------

HRESULT InitWindowlessVMR( 
						  IBaseFilter *pVMR,				// Pointer to the VMR
						  HWND hwnd,						// Clipping window
						  IVMRWindowlessControl** ppWC	// Receives a pointer to the VMR.
						  ) 
{ 

	IVMRFilterConfig* pConfig = NULL; 
	IVMRWindowlessControl *pWC = NULL;

	HRESULT hr = S_OK;

	// Set the rendering mode.  
	hr = pVMR->QueryInterface(IID_IVMRFilterConfig, (void**)&pConfig); 
	if (SUCCEEDED(hr)) 
	{ 
		hr = pConfig->SetRenderingMode(VMRMode_Windowless); 
	}

	// Query for the windowless control interface.
	if (SUCCEEDED(hr))
	{
		hr = pVMR->QueryInterface(IID_IVMRWindowlessControl, (void**)&pWC);
	}

	// Set the clipping window.
	if (SUCCEEDED(hr))
	{
		hr = pWC->SetVideoClippingWindow(hwnd);
	}

	// Preserve aspect ratio by letter-boxing
	if (SUCCEEDED(hr))
	{
		hr = pWC->SetAspectRatioMode(VMR_ARMODE_LETTER_BOX);
	}

	// Return the IVMRWindowlessControl pointer to the caller.
	if (SUCCEEDED(hr))
	{
		*ppWC = pWC;
		(*ppWC)->AddRef();
	}

	SAFE_RELEASE(pConfig);
	SAFE_RELEASE(pWC);

	return hr; 
} 


/// VMR-9 Wrapper

VMR9::VMR9() : m_pWindowless(NULL)
{

}


VMR9::~VMR9()
{
	SAFE_RELEASE(m_pWindowless);
}

HRESULT VMR9::AddToGraph(IGraphBuilder *pGraph, HWND hwnd)
{
	HRESULT hr = S_OK;

	IBaseFilter *pVMR = NULL;

	hr = AddFilterByCLSID(pGraph, CLSID_VideoMixingRenderer9, &pVMR, L"VMR-9");

	// Set windowless mode on the VMR. This must be done before the VMR is connected.
	if (SUCCEEDED(hr))
	{
		hr = InitWindowlessVMR9(pVMR, hwnd, &m_pWindowless);
	}


	return hr;
}


HRESULT VMR9::FinalizeGraph(IGraphBuilder *pGraph)
{
	if (m_pWindowless == NULL)
	{
		return S_OK;
	}

	HRESULT hr = S_OK;
	BOOL bRemoved = FALSE;

	IBaseFilter *pFilter = NULL;

	hr = m_pWindowless->QueryInterface(__uuidof(IBaseFilter), (void**)&pFilter);

	if (SUCCEEDED(hr))
	{
		hr = RemoveUnconnectedRenderer(pGraph, pFilter, &bRemoved);

		// If we removed the VMR, then we also need to release our 
		// pointer to the VMR's windowless control interface.
		if (bRemoved)
		{
			SAFE_RELEASE(m_pWindowless);
		}
	}

	SAFE_RELEASE(pFilter);
	return hr;
}




HRESULT VMR9::UpdateVideoWindow(HWND hwnd, const LPRECT prc)
{
	HRESULT hr = S_OK;

	if (m_pWindowless == NULL)
	{
		return S_OK; // no-op
	}

	if (prc)
	{
		hr = m_pWindowless->SetVideoPosition(NULL, prc);
	}
	else
	{
		RECT rc;
		GetClientRect(hwnd, &rc);
		hr = m_pWindowless->SetVideoPosition(NULL, &rc);
	}
	return hr;
}

HRESULT VMR9::Repaint(HWND hwnd, HDC hdc)
{
	HRESULT hr = S_OK;

	if (m_pWindowless)
	{
		hr = m_pWindowless->RepaintVideo(hwnd, hdc);
	}
	return hr;
}

HRESULT VMR9::DisplayModeChanged()
{
	HRESULT hr = S_OK;

	if (m_pWindowless)
	{
		hr = m_pWindowless->DisplayModeChanged();
	}
	return hr;


}


//-----------------------------------------------------------------------------
// InitWindowlessVMR9
// Initialize the VMR-9 for windowless mode. 
//-----------------------------------------------------------------------------

HRESULT InitWindowlessVMR9( 
						   IBaseFilter *pVMR,				// Pointer to the VMR
						   HWND hwnd,						// Clipping window
						   IVMRWindowlessControl9** ppWC	// Receives a pointer to the VMR.
						   ) 
{ 

	IVMRFilterConfig9 * pConfig = NULL; 
	IVMRWindowlessControl9 *pWC = NULL;

	HRESULT hr = S_OK;

	// Set the rendering mode.  
	hr = pVMR->QueryInterface(IID_IVMRFilterConfig9, (void**)&pConfig); 
	if (SUCCEEDED(hr)) 
	{ 
		hr = pConfig->SetRenderingMode(VMR9Mode_Windowless); 
	}

	// Query for the windowless control interface.
	if (SUCCEEDED(hr))
	{
		hr = pVMR->QueryInterface(IID_IVMRWindowlessControl9, (void**)&pWC);
	}

	// Set the clipping window.
	if (SUCCEEDED(hr))
	{
		hr = pWC->SetVideoClippingWindow(hwnd);
	}

	// Preserve aspect ratio by letter-boxing
	if (SUCCEEDED(hr))
	{
		hr = pWC->SetAspectRatioMode(VMR9ARMode_LetterBox);
	}

	// Return the IVMRWindowlessControl pointer to the caller.
	if (SUCCEEDED(hr))
	{
		*ppWC = pWC;
		(*ppWC)->AddRef();
	}

	SAFE_RELEASE(pConfig);
	SAFE_RELEASE(pWC);

	return hr; 
} 


