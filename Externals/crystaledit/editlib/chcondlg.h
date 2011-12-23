///////////////////////////////////////////////////////////////////////////
//  File:    chcondlg.h
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Ferdinand Prantl
//  E-mail:     prantl@ff.cuni.cz
//
//  Character encoding dialog
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#ifndef __CHCONDLG_H__INCLUDED__
#define __CHCONDLG_H__INCLUDED__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "editcmd.h"

class CCrystalTextView;

/////////////////////////////////////////////////////////////////////////////
// CCharConvDlg dialog

class EDITPADC_CLASS CCharConvDlg : public CDialog
  {
    // Construction
public :
    CCharConvDlg ();

    // Dialog Data
    //{{AFX_DATA(CCharConvDlg)
	enum { IDD = IDD_EDIT_CHARCONV };
	CComboBox	m_ctlSource;
	CComboBox	m_ctlDest;
	int		m_nSource;
	int		m_nDest;
	BOOL	m_bAlpha;
	CString	m_sPreview;
	//}}AFX_DATA
	CString	m_sOriginal;

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CCharConvDlg)
protected :
    virtual void DoDataExchange (CDataExchange * pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected :

    // Generated message map functions
    //{{AFX_MSG(CCharConvDlg)
    virtual void OnOK ();
    virtual BOOL OnInitDialog ();
	afx_msg void OnPreview();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP ()
  };

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CHCONDLG_H__INCLUDED__
