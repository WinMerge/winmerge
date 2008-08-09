#ifndef idt_h
#define idt_h

class HexEditorWindow;

interface CDropTarget : public IDropTarget
{
private:
	ULONG m_cRefCount;

	DWORD LastKeyState;

	bool hdrop_present;
	IDataObject* pDataObj;
	HexEditorWindow &hexwnd;
public:
	//Members
	CDropTarget(HexEditorWindow &hexwnd);
	~CDropTarget( void );

	//IUnknown members
	STDMETHODIMP QueryInterface( REFIID iid, void** ppvObject );
	STDMETHODIMP_(ULONG) AddRef( void );
	STDMETHODIMP_(ULONG) Release( void );

	//IDropTarget methods
	STDMETHODIMP DragEnter( IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect );
	STDMETHODIMP DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect );
	STDMETHODIMP DragLeave( void );
	STDMETHODIMP Drop( IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect );
};

#endif // idt_h
