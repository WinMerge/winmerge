#ifndef ids_h
#define ids_h


class CDropSource: public IDropSource
{
private:
	ULONG m_cRefCount;
	bool deleteself;
	CDropSource** pthis;

public:
	//Members
	CDropSource( bool delself = false, CDropSource** p = NULL);
	~CDropSource( void );

	//IUnknown members
	STDMETHODIMP QueryInterface( REFIID iid, void** ppvObject );
	STDMETHODIMP_(ULONG) AddRef( void );
	STDMETHODIMP_(ULONG) Release( void );

	//IDataObject members
	STDMETHODIMP GiveFeedback( DWORD dwEffect );
	STDMETHODIMP QueryContinueDrag( BOOL fEscapePressed,DWORD grfKeyState );
};


#endif // ids_h