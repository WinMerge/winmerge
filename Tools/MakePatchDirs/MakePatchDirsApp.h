// MakePatchDirs.h : main header file for the MAKEPATCHDIRS application
//

#if !defined(AFX_MAKEPATCHDIRS_H__52906114_7009_41C2_AC82_A92A68F0EE58__INCLUDED_)
#define AFX_MAKEPATCHDIRS_H__52906114_7009_41C2_AC82_A92A68F0EE58__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMakePatchDirsApp:
// See MakePatchDirs.cpp for the implementation of this class
//

class CMakePatchDirsApp : public CWinApp
{
public:
	CMakePatchDirsApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMakePatchDirsApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMakePatchDirsApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAKEPATCHDIRS_H__52906114_7009_41C2_AC82_A92A68F0EE58__INCLUDED_)
