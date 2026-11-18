// SPDX-License-Identifier: GPL-2.0-or-later
#ifndef __WINMERGESCRIPT_H_
#define __WINMERGESCRIPT_H_

#include "resource.h"
#include "typeinfoex.h"
#include <atlstr.h>

typedef CComTypeInfoHolderModule<1>
    CComTypeInfoHolderFileOnly;

class ATL_NO_VTABLE CWinMergeScript :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<
        CWinMergeScript,
        &CLSID_WinMergeScript>,
    public IDispatchImpl<
        IWinMergeScript,
        &IID_IWinMergeScript,
        &LIBID_IgnoreAccentsLib,
        1, 0,
        CComTypeInfoHolderFileOnly>
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

    STDMETHOD(PrediffFile)(
        BSTR fileSrc,
        BSTR fileDst,
        VARIANT_BOOL* pbChanged,
        VARIANT_BOOL* pbSuccess
    );

    STDMETHOD(ShowSettingsDialog)(
        VARIANT_BOOL* pbHandled
    );

    STDMETHOD(PluginOnEvent)(
        int iEventType,
        IDispatch* pDispatch
    );

    STDMETHOD(get_PluginExtendedProperties)(BSTR* pVal);
    STDMETHOD(get_PluginArguments)(BSTR* pVal);
    STDMETHOD(put_PluginArguments)(BSTR val);
    STDMETHOD(get_PluginIsAutomatic)(VARIANT_BOOL* pVal);
    STDMETHOD(get_PluginFileFilters)(BSTR* pVal);
    STDMETHOD(get_PluginDescription)(BSTR* pVal);
    STDMETHOD(get_PluginEvent)(BSTR* pVal);

protected:

    CComPtr<IDispatch> m_pMergeApp;
    CComBSTR m_bstrArguments;
};

#endif