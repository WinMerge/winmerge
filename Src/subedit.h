// subedit.h : main header file for the SUBEDIT application
//

#if !defined(AFX_SUBEDIT_H__071E4499_6320_49E5_B602_E6F7873ED145__INCLUDED_)
#define AFX_SUBEDIT_H__071E4499_6320_49E5_B602_E6F7873ED145__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CSubeditApp:
// See subedit.cpp for the implementation of this class
//

class CSubeditApp : public CWinApp
{
public:
	CSubeditApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSubeditApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CSubeditApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUBEDIT_H__071E4499_6320_49E5_B602_E6F7873ED145__INCLUDED_)
