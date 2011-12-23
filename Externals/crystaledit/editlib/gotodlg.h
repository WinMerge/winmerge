///////////////////////////////////////////////////////////////////////////
//  File:    gotodlg.h
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Ferdinand Prantl
//  E-mail:     prantl@ff.cuni.cz
//
//  Go to line dialog
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#ifndef __GOTODLG_H__INCLUDED__
#define __GOTODLG_H__INCLUDED__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "editcmd.h"

class CCrystalTextView;

/////////////////////////////////////////////////////////////////////////////
// CGotoDlg dialog

class EDITPADC_CLASS CGotoDlg : public CDialog
  {
private :
    CCrystalTextView * m_pBuddy;

    // Construction
public :
    CGotoDlg (CCrystalTextView * pBuddy);

    CPoint m_ptCurrentPos;
    // Dialog Data
    //{{AFX_DATA(CGotoDlg)
	enum { IDD = IDD_EDIT_GOTO };
    CEdit m_ctlNumber;
    CString m_sNumber;
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CGotoDlg)
protected :
    virtual void DoDataExchange (CDataExchange * pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected :

    // Generated message map functions
    //{{AFX_MSG(CGotoDlg)
    virtual void OnOK ();
    afx_msg void OnChangeNumber ();
    virtual BOOL OnInitDialog ();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP ()
  };

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __GOTODLG_H__INCLUDED__
