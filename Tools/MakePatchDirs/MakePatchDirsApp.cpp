// MakePatchDirs.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "MakePatchDirsApp.h"
#include "MakePatchDirsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMakePatchDirsApp

BEGIN_MESSAGE_MAP(CMakePatchDirsApp, CWinApp)
	//{{AFX_MSG_MAP(CMakePatchDirsApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMakePatchDirsApp construction

CMakePatchDirsApp::CMakePatchDirsApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMakePatchDirsApp object

CMakePatchDirsApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMakePatchDirsApp initialization

BOOL CMakePatchDirsApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// loop so dialog can exit & reenter to reload language
	while (1)
	{
		CMakePatchDirsDlg dlg;
		m_pMainWnd = &dlg;
		if (dlg.DoModal() != IDOK)
			break;
		// Remove the WM_QUIT message
		MSG msg;
		::PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE);
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
