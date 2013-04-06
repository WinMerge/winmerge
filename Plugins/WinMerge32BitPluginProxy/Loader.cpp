// Loader.cpp : CLoader ‚ÌŽÀ‘•

#include "stdafx.h"
#include "Loader.h"
#include "lwdisp.h"


// CLoader



STDMETHODIMP CLoader::Load(BSTR bstrPath, BSTR bstrProgId, IDispatch** ppDispatch)
{
	*ppDispatch = CreateDispatchBySource(OLE2T(bstrPath), OLE2W(bstrProgId));

	return S_OK;
}
