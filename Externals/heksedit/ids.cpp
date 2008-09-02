#include "precomp.h"
#include "ids.h"
#include "hexwnd.h"

//Members
CDropSource::CDropSource()
: m_cRefCount(0)
{
}

CDropSource::~CDropSource()
{
	assert(m_cRefCount == 0);
}

//IUnknown members
STDMETHODIMP CDropSource::QueryInterface(REFIID iid, void **ppvObject)
{
	if (iid == IID_IUnknown || iid == IID_IDropSource)
	{
		*ppvObject = static_cast<IDropSource *>(this);
		AddRef();
		return S_OK;
	}
	*ppvObject = NULL;
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CDropSource::AddRef()
{
	return ++m_cRefCount;
}

STDMETHODIMP_(ULONG) CDropSource::Release()
{
	return --m_cRefCount;
}

//IDropSource members
STDMETHODIMP CDropSource::GiveFeedback(DWORD)
{
	return DRAGDROP_S_USEDEFAULTCURSORS;
}

STDMETHODIMP CDropSource::QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState)
{
	//Cancel when escape is pressed
	if (fEscapePressed)
		return DRAGDROP_S_CANCEL;
	//Complete if there are no mouse buttons pressed
	if(!(grfKeyState & (MK_LBUTTON|MK_MBUTTON|MK_RBUTTON)))
		return DRAGDROP_S_DROP;
	//Otherwise continue the drag operation
	return S_OK;
}
