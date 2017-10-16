#include "stdafx.h"

//-----------------------------------------------------------------------------
// DShowPlayer::RemoveUnconnectedRenderer
// Description: Remove a renderer filter from the graph if the filter is
//              not connected. 
//-----------------------------------------------------------------------------
HRESULT RemoveUnconnectedRenderer(IGraphBuilder *pGraph, IBaseFilter *pRenderer, BOOL *pbRemoved)
{
	IPin *pPin = NULL;

	BOOL bRemoved = FALSE;

	// Look for a connected input pin on the renderer.

	HRESULT hr = FindConnectedPin(pRenderer, PINDIR_INPUT, &pPin);
	SAFE_RELEASE(pPin);

	// If this function succeeds, the renderer is connected, so we don't remove it.
	// If it fails, it means the renderer is not connected to anything, so
	// we remove it.

	if (FAILED(hr))
	{
		hr = pGraph->RemoveFilter(pRenderer);
		bRemoved = TRUE;
	}

	if (SUCCEEDED(hr))
	{
		*pbRemoved = bRemoved;
	}

	return hr;
}


