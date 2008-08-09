#include "precomp.h"
#include "ids.h"
#include "hexwnd.h"


//Members
CDropSource::CDropSource( bool delself, CDropSource** p )//When using C++ new need to call like so someptr = new CDataObject(1,someptr);
{
#ifdef _DEBUG
	printf("IDropSource::IDropSource\n");
#endif //_DEBUG
	m_cRefCount = 0;
	deleteself = delself;
	pthis = p;
}

CDropSource::~CDropSource( void )
{
#ifdef _DEBUG
	printf("IDropSource::~IDropSource\n");
	if( m_cRefCount != 0 )
		printf("Deleting %s too early 0x%x.m_cRefCount = %d\n", "IDropSource", this, m_cRefCount);
#endif //_DEBUG
	if( pthis ) *pthis = NULL;
}


//IUnknown members
STDMETHODIMP CDropSource::QueryInterface( REFIID iid, void** ppvObject )
{
#ifdef _DEBUG
	printf("IDropSource::QueryInterface\n");
#endif //_DEBUG

	*ppvObject = NULL;

	if ( iid == IID_IUnknown ) *ppvObject = (IUnknown*)this;
	else if ( iid == IID_IDropSource ) *ppvObject = (IDropSource*)this;

	if(*ppvObject){
		((IUnknown*)*ppvObject)->AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CDropSource::AddRef( void )
{
#ifdef _DEBUG
	printf("IDropSource::AddRef\n");
#endif //_DEBUG
	return ++m_cRefCount;
}

STDMETHODIMP_(ULONG) CDropSource::Release( void )
{
#ifdef _DEBUG
	printf("IDropSource::Release\n");
#endif //_DEBUG
	if( --m_cRefCount == 0 && deleteself ) delete this;
	return m_cRefCount;
}


//IDropSource members
STDMETHODIMP CDropSource::GiveFeedback( DWORD dwEffect )
{
	UNREFERENCED_PARAMETER( dwEffect );
#ifdef _DEBUG
	printf("IDropSource::GiveFeedback\n");
#endif //_DEBUG
	return DRAGDROP_S_USEDEFAULTCURSORS;
}

STDMETHODIMP CDropSource::QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState )
{
#ifdef _DEBUG
	printf("IDropSource::QueryContinueDrag\n");
#endif //_DEBUG
	//Cancel when escape is pressed
	if(fEscapePressed) return DRAGDROP_S_CANCEL;
	//Complete if there are no mouse buttons pressed
	if(!(grfKeyState&(MK_LBUTTON|MK_MBUTTON|MK_RBUTTON)))
		return DRAGDROP_S_DROP;
	//Otherwise continue the drag operation
	return S_OK;
}
