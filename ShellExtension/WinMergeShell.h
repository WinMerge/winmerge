// WinMergeShell.h : Declaration of the CWinMergeShell

#ifndef __WINMERGESHELL_H_
#define __WINMERGESHELL_H_

#include <shlobj.h>
#include <comdef.h>
#include <atlconv.h>  // for ATL string conversion macros
#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CWinMergeShell
class ATL_NO_VTABLE CWinMergeShell : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CWinMergeShell, &CLSID_WinMergeShell>,
	public IDispatchImpl<IWinMergeShell, &IID_IWinMergeShell, &LIBID_SHELLEXTENSIONLib>,
	public IShellExtInit,
	public IContextMenu
{
public:
	CWinMergeShell();

DECLARE_REGISTRY_RESOURCEID(IDR_WINMERGESHELL)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CWinMergeShell)
	COM_INTERFACE_ENTRY(IWinMergeShell)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IShellExtInit)
	COM_INTERFACE_ENTRY(IContextMenu)
END_COM_MAP()

// IWinMergeShell
protected:
    CString m_strPaths[2];
	HBITMAP m_hMergeBmp;
	UINT	m_nSelectedItems;

	BOOL GetWinMergeDir(CString &strDir);

public:
    // IShellExtInit
    STDMETHOD(Initialize)(LPCITEMIDLIST, LPDATAOBJECT, HKEY);

    // IContextMenu
    STDMETHOD(GetCommandString)(UINT, UINT, UINT*, LPSTR, UINT);
    STDMETHOD(InvokeCommand)(LPCMINVOKECOMMANDINFO);
    STDMETHOD(QueryContextMenu)(HMENU, UINT, UINT, UINT, UINT);
};

#endif //__WINMERGESHELL_H_
