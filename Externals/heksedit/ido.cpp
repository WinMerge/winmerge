#include "precomp.h"
#include "ido.h"
#include "hexwnd.h"
#include <shlwapi.h>

/*The #ifndef __CYGWIN__s are there because cygwin/mingw doesn't yet have
certain APIs in their import libraries. Specifically _wremove, _wopen & GetEnhMetaFileBits.*/

//CDataObject
//Members
CDataObject::CDataObject( bool delself, CDataObject** p )//When using C++ new need to call like so someptr = new CDataObject(1,someptr);
{
#ifdef _DEBUG
	printf("IDataObject::IDataObject\n");
#endif //_DEBUG
	m_cRefCount = 0;
	deleteself = delself;
	pthis = p;

	allowSetData = true;
	data = NULL;
	enums = NULL;
	numdata = numenums = 0;
}

CDataObject::~CDataObject( void )
{
#ifdef _DEBUG
	printf("IDataObject::~IDataObject\n");
	if( m_cRefCount != 0 )
		printf("Deleting %s too early 0x%x.m_cRefCount = %d\n", "IDataObject", this, m_cRefCount);
#endif //_DEBUG
	Empty();
	if(enums){
		for(UINT i = 0;i<numenums;i++)
			if(enums[i]) delete enums[i];
		free(enums);
		enums = NULL; numenums = 0;
	}
	if( pthis ) *pthis = NULL;
}

void CDataObject::DisableSetData( void ){
	allowSetData = false;
}

void CDataObject::Empty( void ){
	if(data){
		for( UINT i = 0;i<numdata;i++)
			ReleaseStgMedium(&data[i].medium);
		free(data);
		data = NULL;
	}
}


//IUnknown members
STDMETHODIMP CDataObject::QueryInterface( REFIID iid, void** ppvObject )
{
#ifdef _DEBUG
	printf("IDataObject::QueryInterface\n");
#endif //_DEBUG

	*ppvObject = NULL;

	if ( iid == IID_IUnknown ) *ppvObject = (IUnknown*)this;
	else if ( iid == IID_IDataObject ) *ppvObject = (IDataObject*)this;

	if(*ppvObject){
		((IUnknown*)*ppvObject)->AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CDataObject::AddRef( void )
{
#ifdef _DEBUG
	printf("IDataObject::AddRef\n");
#endif //_DEBUG
	return ++m_cRefCount;
}

STDMETHODIMP_(ULONG) CDataObject::Release( void )
{
#ifdef _DEBUG
	printf("IDataObject::Release\n");
#endif //_DEBUG
	if( --m_cRefCount == 0 && deleteself ) delete this;
	return m_cRefCount;
}


//IDataObject members
STDMETHODIMP CDataObject::GetData( FORMATETC* pFormatetc, STGMEDIUM* pmedium )
{
#ifdef _DEBUG
	printf("IDataObject::GetData\n");
#endif //_DEBUG
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

STDMETHODIMP CDataObject::GetDataHere( FORMATETC* pFormatetc, STGMEDIUM* pmedium )
{
	UNREFERENCED_PARAMETER( pFormatetc );
	UNREFERENCED_PARAMETER( pmedium );
#ifdef _DEBUG
	printf("IDataObject::GetDataHere\n");
#endif //_DEBUG
	pmedium->pUnkForRelease = NULL;
	return E_NOTIMPL;
}

STDMETHODIMP CDataObject::QueryGetData( FORMATETC* pFormatetc )
{
#ifdef _DEBUG
	printf("IDataObject::QueryGetData\n");
#endif //_DEBUG
	for( UINT i = 0; i < numdata; i++ ){
		if( data[i].format.cfFormat == pFormatetc->cfFormat ){
			if( data[i].format.dwAspect == pFormatetc->dwAspect){
				return S_OK;
			}
		}
	}
	return DV_E_FORMATETC;
}

STDMETHODIMP CDataObject::GetCanonicalFormatEtc( FORMATETC* pFormatetcIn, FORMATETC* pFormatetcOut )
{
	UNREFERENCED_PARAMETER( pFormatetcIn );
	UNREFERENCED_PARAMETER( pFormatetcOut );
#ifdef _DEBUG
	printf("IDataObject::GetCanonicalFormatEtc\n");
#endif //_DEBUG
	return E_NOTIMPL;
}

STDMETHODIMP CDataObject::SetData(FORMATETC* pFormatetc, STGMEDIUM* pmedium, BOOL fRelease)
{
#ifdef _DEBUG
	printf("IDataObject::SetData\n");
#endif //_DEBUG

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
#ifdef _DEBUG
	printf("IDataObject::EnumFormatEtc\n");
#endif //_DEBUG

	//Don't support DATADIR_SET since we accept any format
	if(dwDirection!=DATADIR_GET) return E_NOTIMPL;

	*ppenumFormatetc = NULL;//FIXME: should we do this?

	/*Find an array member that is NULL (has been freed)
	  or resize the array to make space for one*/
	unsigned int i = numenums;
	if(enums) for( i = 0; i < numenums && enums[i] != NULL; i++ );
	CEnumFORMATETC** t;
	if( i == numenums ) t = (CEnumFORMATETC**) realloc(enums,sizeof(CEnumFORMATETC*)*(numenums+1));
	else t = enums;
	if(t){
		enums = t; t[i] = new CEnumFORMATETC(this,true,&t[i]);
		if(t[i]){
			HRESULT ret = t[i]->QueryInterface(IID_IEnumFORMATETC,(void**)ppenumFormatetc);
			if( i == numenums ) numenums++;
			if( ret != S_OK || *ppenumFormatetc == NULL )
				//FIXME: Should we return E_INVALIDARG or E_OUTOFMEMORY or what?
				return E_OUTOFMEMORY;
		}
		else return E_OUTOFMEMORY;
	} else return E_OUTOFMEMORY;
	return S_OK;
}

//CEnumFORMATETC
//Members
CEnumFORMATETC::CEnumFORMATETC( CDataObject*par, bool delself, CEnumFORMATETC** p )
{
#ifdef _DEBUG
	printf("IEnumFORMATETC::IEnumFORMATETC\n");
#endif //_DEBUG
	m_cRefCount = 0;
	deleteself = delself;
	pthis = p;

	parent = par;
	index = 0;
}

CEnumFORMATETC::~CEnumFORMATETC( void )
{
#ifdef _DEBUG
	printf("IEnumFORMATETC::~IEnumFORMATETC\n");
	if( m_cRefCount != 0 )
		printf("Deleting %s too early 0x%x.m_cRefCount = %d\n", "IEnumFORMATETC", this, m_cRefCount);
#endif //_DEBUG
	if( pthis ) *pthis = NULL;
}

//IUnknown members
STDMETHODIMP CEnumFORMATETC::QueryInterface( REFIID iid, void** ppvObject )
{
#ifdef _DEBUG
	printf("IEnumFORMATETC::QueryInterface\n");
#endif //_DEBUG

	*ppvObject = NULL;

	if ( iid == IID_IUnknown ) *ppvObject = (IUnknown*)this;
	else if ( iid == IID_IEnumFORMATETC ) *ppvObject = (IEnumFORMATETC*)this;

	if(*ppvObject){
		((IUnknown*)*ppvObject)->AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CEnumFORMATETC::AddRef( void )
{
#ifdef _DEBUG
	printf("IEnumFORMATETC::AddRef\n");
#endif //_DEBUG
	return ++m_cRefCount;
}

STDMETHODIMP_(ULONG) CEnumFORMATETC::Release( void )
{
#ifdef _DEBUG
	printf("IEnumFORMATETC::Release\n");
#endif //_DEBUG
	if( --m_cRefCount == 0 && deleteself ) delete this;
	return m_cRefCount;
}

//IEnumFORMATETC members
STDMETHODIMP CEnumFORMATETC::Next( ULONG celt, FORMATETC* rgelt, ULONG* pceltFetched ){
#ifdef _DEBUG
	printf("IEnumFORMATETC::Next\n");
#endif //_DEBUG
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

STDMETHODIMP CEnumFORMATETC::Skip( ULONG celt ){
#ifdef _DEBUG
	printf("IEnumFORMATETC::Skip\n");
#endif //_DEBUG
	index += celt;
	return S_FALSE;
}

STDMETHODIMP CEnumFORMATETC::Reset( void ){
#ifdef _DEBUG
	printf("IEnumFORMATETC::Reset\n");
#endif //_DEBUG
	index = 0;
	return S_OK;
}

STDMETHODIMP CEnumFORMATETC::Clone( IEnumFORMATETC** ppenum ){
#ifdef _DEBUG
	printf("IEnumFORMATETC::Clone\n");
#endif //_DEBUG
	return parent->EnumFormatEtc( DATADIR_GET, ppenum );
}


//Following methods not implemented yet
STDMETHODIMP CDataObject::DAdvise( FORMATETC* pFormatetc, DWORD advf, IAdviseSink* pAdvSink, DWORD* pdwConnection )
{
	UNREFERENCED_PARAMETER( pFormatetc );
	UNREFERENCED_PARAMETER( advf );
	UNREFERENCED_PARAMETER( pAdvSink );
	UNREFERENCED_PARAMETER( pdwConnection );
#ifdef _DEBUG
	printf("IDataObject::DAdvise\n");
#endif //_DEBUG
	return E_NOTIMPL;
}

STDMETHODIMP CDataObject::DUnadvise( DWORD dwConnection )
{
	UNREFERENCED_PARAMETER( dwConnection );
#ifdef _DEBUG
	printf("IDataObject::DUnadvise\n");
#endif //_DEBUG
	return E_NOTIMPL;
}

STDMETHODIMP CDataObject::EnumDAdvise( IEnumSTATDATA** ppenumAdvise )
{
	UNREFERENCED_PARAMETER( ppenumAdvise );
#ifdef _DEBUG
	printf("IDataObject::EnumDAdvise\n");
#endif //_DEBUG
	return E_NOTIMPL;
}
