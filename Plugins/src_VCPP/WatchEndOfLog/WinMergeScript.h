// WinMergeScript.h : Declaration of the CWinMergeScript

#ifndef __WINMERGESCRIPT_H_
#define __WINMERGESCRIPT_H_

#include "resource.h"       // main symbols

// change 1 : add this include
#include "typeinfoex.h"

/////////////////////////////////////////////////////////////////////////////
// CWinMergeScript

// change 2 : add this
typedef CComTypeInfoHolderModule<1>  CComTypeInfoHolderFileOnly;

class ATL_NO_VTABLE CWinMergeScript : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CWinMergeScript, &CLSID_WinMergeScript>,
// change 3 : insert the text ", 1, 0, CComTypeInfoHolderFileOnly" 
	// public IDispatchImpl<IWinMergeScript, &IID_IWinMergeScript, &LIBID_HIDEFIRSTLETTERLib>
	public IDispatchImpl<IWinMergeScript, &IID_IWinMergeScript, &LIBID_HIDEFIRSTLETTERLib, 1, 0, CComTypeInfoHolderFileOnly>
{
public:
	CWinMergeScript()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_WINMERGESCRIPT)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CWinMergeScript)
	COM_INTERFACE_ENTRY(IWinMergeScript)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IWinMergeScript
public:
	STDMETHOD(PackFile)(/*[in]*/ BSTR fileSrc, /*[in]*/ BSTR fileDst, VARIANT_BOOL * pbChanged, INT pSubcode, /*[out, retval]*/ VARIANT_BOOL * pbSuccess);
	STDMETHOD(UnpackFile)(/*[in]*/ BSTR fileSrc, /*[in]*/ BSTR fileDst, VARIANT_BOOL * pbChanged, INT * pSubcode, /*[out, retval]*/ VARIANT_BOOL * pbSuccess);
	STDMETHOD(PackBufferA)(/*[in]*/ SAFEARRAY ** pBuffer, /*[in]*/ INT * pSize, /*[in]*/ VARIANT_BOOL * pbChanged, /*[in]*/ INT subcode, /*[out, retval]*/ VARIANT_BOOL * pbSuccess);
	STDMETHOD(UnpackBufferA)(/*[in]*/ SAFEARRAY ** pBuffer, /*[in]*/ INT * pSize, /*[in]*/ VARIANT_BOOL * pbChanged, /*[in]*/ INT * pSubcode, /*[out, retval]*/ VARIANT_BOOL * pbSuccess);
	STDMETHOD(get_PluginIsAutomatic)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(get_PluginFileFilters)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_PluginDescription)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_PluginEvent)(/*[out, retval]*/ BSTR *pVal);
};

#endif //__WINMERGESCRIPT_H_
