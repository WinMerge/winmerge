// MakePatchDirs_sat.cpp : Defines the initialization routines for the DLL.
// Used for a satellite resource dll (that is, resources for one language)
//

#include "stdafx.h"
#include <afxdllx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static AFX_EXTENSION_MODULE MakePatchDirs_satDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	TCHAR name[MAX_PATH];
	GetModuleFileName(hInstance, name, sizeof(name)/sizeof(name[0]));

	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE(_T("%s Initializing!\n"), name);
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(MakePatchDirs_satDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(MakePatchDirs_satDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE(_T("%s Terminating!\n"), name);
		// Terminate the library before destructors are called
		AfxTermExtensionModule(MakePatchDirs_satDLL);
	}
	return 1;   // ok
}
