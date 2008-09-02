#include "precomp.h"
#include "ido.h"
#include "hexwnd.h"

/*The #ifndef __CYGWIN__s are there because cygwin/mingw doesn't yet have
certain APIs in their import libraries. Specifically _wremove, _wopen & GetEnhMetaFileBits.*/

//CDataObject
//Members
CDataObject::CDataObject()
{
	m_cRefCount = 0;
	allowSetData = true;
	data = NULL;
	enums = NULL;
	numdata = numenums = 0;
}

CDataObject::~CDataObject()
{
	assert(m_cRefCount == 0);
	Empty();
	if (enums)
	{
		while (numenums)
			delete enums[--numenums];
		free(enums);
	}
}

void CDataObject::DisableSetData()
{
	allowSetData = false;
}

void CDataObject::Empty()
{
	if (data)
	{
		while (numdata)
			ReleaseStgMedium(&data[--numdata].medium);
		free(data);
		data = NULL;
	}
}

//IUnknown members
STDMETHODIMP CDataObject::QueryInterface(REFIID iid, void **ppvObject)
{
	if (iid == IID_IUnknown || iid == IID_IDataObject)
	{
		*ppvObject = static_cast<IDataObject *>(this);
		AddRef();
		return S_OK;
	}
	*ppvObject = NULL;
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CDataObject::AddRef()
{
	return ++m_cRefCount;
}

STDMETHODIMP_(ULONG) CDataObject::Release()
{
	return --m_cRefCount;
}

//IDataObject members
STDMETHODIMP CDataObject::GetData( FORMATETC* pFormatetc, STGMEDIUM* pmedium )
{
	//Error handling
	if (pFormatetc == 0 || pmedium == 0)
		return E_INVALIDARG;
	if (pFormatetc->lindex != -1)
		return DV_E_LINDEX;
	if (pFormatetc->ptd != NULL)
		return DV_E_FORMATETC;

	HRESULT hr = DV_E_FORMATETC;
	//Search for the requested aspect of the requested format
	for (UINT i = 0; i < numdata ; i++)
	{
		if (data[i].format.cfFormat == pFormatetc->cfFormat)
		{
			hr = DV_E_DVASPECT;
			if (data[i].format.dwAspect == pFormatetc->dwAspect)
			{
				*pmedium = data[i].medium;
				hr = S_OK;
				break;
			}
		}
	}
	return hr;
}

STDMETHODIMP CDataObject::GetDataHere(FORMATETC *, STGMEDIUM *pmedium)
{
	pmedium->pUnkForRelease = NULL;
	return E_NOTIMPL;
}

STDMETHODIMP CDataObject::QueryGetData(FORMATETC *pFormatetc)
{
	for (UINT i = 0 ; i < numdata ; i++)
		if (data[i].format.cfFormat == pFormatetc->cfFormat)
			if (data[i].format.dwAspect == pFormatetc->dwAspect)
				return S_OK;
	return DV_E_FORMATETC;
}

STDMETHODIMP CDataObject::GetCanonicalFormatEtc(FORMATETC *, FORMATETC *)
{
	return E_NOTIMPL;
}

STDMETHODIMP CDataObject::SetData(FORMATETC *pFormatetc, STGMEDIUM *pmedium, BOOL fRelease)
{
	if (!allowSetData)
		return E_NOTIMPL;

	//Error handling
	//Make sure we can read from *pFormatetc & *pmedium & they have the same medium
	if (pFormatetc == 0 || pmedium == 0 || fRelease == FALSE)
		return E_INVALIDARG;

	if (pFormatetc->tymed != pmedium->tymed)
		return DV_E_TYMED;

	//Create a new entry
	UINT n = numdata + 1;
	DataSpecifier *t = (DataSpecifier *)realloc(data, n * sizeof *data);
	if (t == 0)
		return E_OUTOFMEMORY;
	t[numdata].medium = *pmedium;
	t[numdata].format = *pFormatetc;
	data = t;
	numdata = n;
	return S_OK;
}

STDMETHODIMP CDataObject::EnumFormatEtc( DWORD dwDirection, IEnumFORMATETC** ppenumFormatetc )
{
	//Don't support DATADIR_SET since we accept any format
	if(dwDirection!=DATADIR_GET) return E_NOTIMPL;

	*ppenumFormatetc = NULL;//FIXME: should we do this?

	/*Find an array member that is NULL (has been freed)
	  or resize the array to make space for one*/
	unsigned int i = numenums;
	if(enums) for( i = 0; i < numenums && enums[i] != NULL; i++ );
	CEnumFORMATETC **t;
	if (i == numenums)
		t = (CEnumFORMATETC**) realloc(enums, sizeof(CEnumFORMATETC *) * (numenums + 1));
	else
		t = enums;
	if (t == 0)
		return E_OUTOFMEMORY;
	enums = t;
	t[i] = new CEnumFORMATETC(this);
	if (t[i] == 0)
		return E_OUTOFMEMORY;
	HRESULT ret = t[i]->QueryInterface(IID_IEnumFORMATETC, (void**)ppenumFormatetc);
	if (i == numenums)
		numenums++;
	if (ret != S_OK || *ppenumFormatetc == NULL)
		//FIXME: Should we return E_INVALIDARG or E_OUTOFMEMORY or what?
		return E_OUTOFMEMORY;
	return S_OK;
}

//CEnumFORMATETC
//Members
CEnumFORMATETC::CEnumFORMATETC(CDataObject *par)
{
	m_cRefCount = 0;
	parent = par;
	index = 0;
}

CEnumFORMATETC::~CEnumFORMATETC()
{
	assert(m_cRefCount == 0);
}

//IUnknown members
STDMETHODIMP CEnumFORMATETC::QueryInterface(REFIID iid, void **ppvObject)
{
	if (iid == IID_IUnknown || iid == IID_IEnumFORMATETC)
	{
		*ppvObject = static_cast<IEnumFORMATETC *>(this);
		AddRef();
		return S_OK;
	}
	*ppvObject = NULL;
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CEnumFORMATETC::AddRef()
{
	return ++m_cRefCount;
}

STDMETHODIMP_(ULONG) CEnumFORMATETC::Release()
{
	return --m_cRefCount;
}

//IEnumFORMATETC members
STDMETHODIMP CEnumFORMATETC::Next(ULONG celt, FORMATETC *rgelt, ULONG *pceltFetched)
{
	if (rgelt == 0)
		return E_INVALIDARG;
	ULONG fetched = 0;
	unsigned int e=index+celt;
	while (index < e && index < parent->numdata)
	{
		rgelt[fetched++] = parent->data[index++].format;
	}
	if (pceltFetched)
		*pceltFetched = fetched;
	return fetched == celt ? S_OK : S_FALSE;
}

STDMETHODIMP CEnumFORMATETC::Skip(ULONG celt)
{
	index += celt;
	return S_FALSE;
}

STDMETHODIMP CEnumFORMATETC::Reset()
{
	index = 0;
	return S_OK;
}

STDMETHODIMP CEnumFORMATETC::Clone(IEnumFORMATETC **ppenum)
{
	return parent->EnumFormatEtc(DATADIR_GET, ppenum);
}

//Following methods not implemented yet
STDMETHODIMP CDataObject::DAdvise(FORMATETC *, DWORD, IAdviseSink *, DWORD *)
{
	return E_NOTIMPL;
}

STDMETHODIMP CDataObject::DUnadvise(DWORD)
{
	return E_NOTIMPL;
}

STDMETHODIMP CDataObject::EnumDAdvise(IEnumSTATDATA **)
{
	return E_NOTIMPL;
}
