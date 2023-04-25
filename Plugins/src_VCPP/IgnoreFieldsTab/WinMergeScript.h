// WinMergeScript.h : Declaration of the CWinMergeScript

#ifndef __WINMERGESCRIPT_H_
#define __WINMERGESCRIPT_H_

#include "resource.h"       // main symbols

// change 1 : add this include
#include "typeinfoex.h"
#include <atlstr.h>

/////////////////////////////////////////////////////////////////////////////
// CWinMergeScript

// change 2 : add this
typedef CComTypeInfoHolderModule<1>  CComTypeInfoHolderFileOnly;

class ATL_NO_VTABLE CWinMergeScript : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CWinMergeScript, &CLSID_WinMergeScript>,
// change 3 : insert the text ", 1, 0, CComTypeInfoHolderFileOnly" 
	// public IDispatchImpl<IWinMergeScript, &IID_IWinMergeScript, &LIBID_IgnoreFieldsTabLib>
	public IDispatchImpl<IWinMergeScript, &IID_IWinMergeScript, &LIBID_IgnoreFieldsTabLib, 1, 0, CComTypeInfoHolderFileOnly>
{
public:
	CWinMergeScript() : m_bstrArguments(L"")
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
	STDMETHOD(PrediffBufferW)(/*[in]*/ BSTR * pText, /*[in]*/ INT * pSize, /*[in]*/ VARIANT_BOOL * pbChanged, /*[out, retval]*/ VARIANT_BOOL * pbHandled);
	STDMETHOD(ShowSettingsDialog)(/*[out, retval]*/ VARIANT_BOOL * pbHandled);
	STDMETHOD(PluginOnEvent)(/*[in]*/int iEventType, /*[in]*/IDispatch* pDispatch);
	STDMETHOD(get_PluginExtendedProperties)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_PluginArguments)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_PluginArguments)(/*[in]*/ BSTR val);
	STDMETHOD(get_PluginIsAutomatic)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(get_PluginFileFilters)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_PluginDescription)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_PluginEvent)(/*[out, retval]*/ BSTR *pVal);

protected:
	CString GetColumnRangeString();
	static INT_PTR CALLBACK DlgProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);
	CComPtr<IDispatch> m_pMergeApp;
	CComBSTR m_bstrArguments;
};

#endif //__WINMERGESCRIPT_H_
