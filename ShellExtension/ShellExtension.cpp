/////////////////////////////////////////////////////////////////////////////
// ShellExtension.cpp : Implementation of DLL Exports.
//
/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//    You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
// Look at http://www.codeproject.com/shell/ for excellent guide
// to Windows Shell programming by Michael Dunn.
// 
// $Id$

// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f ShellExtensionps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "ShellExtension.h"

#include "ShellExtension_i.c"
#include "WinMergeShell.h"


CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_WinMergeShell, CWinMergeShell)
END_OBJECT_MAP()

class CShellExtensionApp : public CWinApp
{
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShellExtensionApp)
	public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CShellExtensionApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CShellExtensionApp, CWinApp)
	//{{AFX_MSG_MAP(CShellExtensionApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CShellExtensionApp theApp;

BOOL CShellExtensionApp::InitInstance()
{
    _Module.Init(ObjectMap, m_hInstance, &LIBID_SHELLEXTENSIONLib);
    return CWinApp::InitInstance();
}

int CShellExtensionApp::ExitInstance()
{
    _Module.Term();
    return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return (AfxDllCanUnloadNow()==S_OK && _Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	// If we're on NT, add ourselves to the list of approved shell extensions.

	// Note that you should *NEVER* use the overload of CRegKey::SetValue with
	// 4 parameters.  It lets you set a value in one call, without having to 
	// call CRegKey::Open() first.  However, that version of SetValue() has a
	// bug in that it requests KEY_ALL_ACCESS to the key.  That will fail if the
	// user is not an administrator.  (The code should request KEY_WRITE, which
	// is all that's necessary.)

	if ((GetVersion() & 0x80000000UL) == 0)
	{
		CRegKey reg;
		LONG lRet;

		lRet = reg.Open(HKEY_LOCAL_MACHINE,
				_T("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"),
				KEY_SET_VALUE );

		if (ERROR_SUCCESS != lRet)
			return E_ACCESSDENIED;

		lRet = reg.SetValue(_T("WinMerge_Shell Extension"), 
				_T("{4E716236-AA30-4C65-B225-D68BBA81E9C2}"));

		if (ERROR_SUCCESS != lRet)
			return E_ACCESSDENIED;
	}

	// registers object, typelib and all interfaces in typelib
    return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	// If we're on NT, remove ourselves from the list of approved shell extensions.
	// Note that if we get an error along the way, I don't bail out since I want
	// to do the normal ATL unregistration stuff too.

	if ((GetVersion() & 0x80000000UL) == 0)
	{
		CRegKey reg;
		LONG lRet;

		lRet = reg.Open(HKEY_LOCAL_MACHINE,
				_T("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"),
				KEY_SET_VALUE);

		if (ERROR_SUCCESS == lRet)
		{
			lRet = reg.DeleteValue( _T("{4E716236-AA30-4C65-B225-D68BBA81E9C2}"));
		}
	}
	
	return _Module.UnregisterServer(TRUE);
}


