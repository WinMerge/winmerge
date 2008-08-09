#ifndef ido_h
#define ido_h


struct DataSpecifier
{
	STGMEDIUM medium;
	FORMATETC format;
};

class CEnumFORMATETC;

class CDataObject: public IDataObject
{
	friend CEnumFORMATETC;
private:
	ULONG m_cRefCount;
	bool deleteself;
	CDataObject** pthis;

	bool allowSetData;      //Allow the IDataObject::SetData call
	DataSpecifier* data;    //Pointer to an array of STGMEDIUM+FORMATETC pairs
	unsigned int numdata;   //Number of data objects in data
	CEnumFORMATETC** enums; //An array of pointers to IEnumFORMATETC objects
	unsigned int numenums;  //Number of pointers in enums

public:
	//Members
	CDataObject( bool delself = false, CDataObject** p = NULL );
	~CDataObject( void );
	void DisableSetData( void );
	void Empty( void );

	//IUnknown members
	STDMETHODIMP QueryInterface( REFIID iid, void** ppvObject );
	STDMETHODIMP_(ULONG) AddRef( void );
	STDMETHODIMP_(ULONG) Release( void );

	//IDataObject members
	STDMETHODIMP GetData( FORMATETC* pFormatetc, STGMEDIUM* pmedium );
	STDMETHODIMP GetDataHere( FORMATETC* pFormatetc, STGMEDIUM* pmedium );
	STDMETHODIMP QueryGetData( FORMATETC* pFormatetc );
	STDMETHODIMP GetCanonicalFormatEtc( FORMATETC* pFormatetcIn, FORMATETC* pFormatetcOut );
	STDMETHODIMP SetData( FORMATETC* pFormatetc, STGMEDIUM* pmedium, BOOL fRelease );
	STDMETHODIMP EnumFormatEtc( DWORD dwDirection, IEnumFORMATETC** ppenumFormatetc );
	STDMETHODIMP DAdvise( FORMATETC* pFormatetc, DWORD advf, IAdviseSink* pAdvSink, DWORD* pdwConnection );
	STDMETHODIMP DUnadvise( DWORD dwConnection );
	STDMETHODIMP EnumDAdvise( IEnumSTATDATA** ppenumAdvise );
};

class CEnumFORMATETC : public IEnumFORMATETC
{
private:
	DWORD m_cRefCount;
	bool deleteself;
	CEnumFORMATETC** pthis;

	CDataObject* parent; //Pointer to the parent IDataObject
	unsigned int index;  //The index of the data in the parent's data storage

public:
	//Members
	CEnumFORMATETC( CDataObject*par, bool delself = false, CEnumFORMATETC** p = NULL );
	~CEnumFORMATETC( void );

	//IUnknown members
	STDMETHODIMP QueryInterface( REFIID iid, void** ppvObject );
	STDMETHODIMP_(ULONG) AddRef( void );
	STDMETHODIMP_(ULONG) Release( void );

	//IEnumFORMATETC members
	STDMETHODIMP Next( ULONG celt, FORMATETC* rgelt, ULONG* pceltFetched );
	STDMETHODIMP Skip( ULONG celt );
	STDMETHODIMP Reset( void );
	STDMETHODIMP Clone( IEnumFORMATETC** ppenum );
};

#endif // ido_h