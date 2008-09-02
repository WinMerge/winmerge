#ifndef ids_h
#define ids_h


class CDropSource: public IDropSource
{
private:
	ULONG m_cRefCount;

public:
	//Members
	CDropSource();
	~CDropSource();

	//IUnknown members
	STDMETHODIMP QueryInterface(REFIID iid, void **ppvObject);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	//IDataObject members
	STDMETHODIMP GiveFeedback(DWORD dwEffect);
	STDMETHODIMP QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState);
};


#endif // ids_h