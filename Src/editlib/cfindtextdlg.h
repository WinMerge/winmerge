////////////////////////////////////////////////////////////////////////////
//  File:       cfindtextdlg.h
//  Version:    1.0.0.0
//  Created:    29-Dec-1998
//
//  Author:     Stcherbatchenko Andrei
//  E-mail:     windfall@gmx.de
//
//  Declaration of the CFindTextDlg dialog, a part of Crystal Edit -
//  syntax coloring text editor.
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//  19-Jul-99
//      Ferdinand Prantl:
//  +   FEATURE: see cpps ...
//
//  ... it's being edited very rapidly so sorry for non-commented
//        and maybe "ugly" code ...
////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CFINDTEXTDLG_H__F59009E3_7B01_11D2_8C4F_0080ADB86836__INCLUDED_)
#define AFX_CFINDTEXTDLG_H__F59009E3_7B01_11D2_8C4F_0080ADB86836__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "resource.h"
#include "memcombo.h"

class CCrystalTextView;


/////////////////////////////////////////////////////////////////////////////

#ifndef STRUCT_LAST_SEARCH_INFO
#define STRUCT_LAST_SEARCH_INFO

// this structure is also defined in the file CEditReplaceDlg.h
struct LastSearchInfos
  {
    int m_nDirection;
    BOOL m_bMatchCase;
    CString m_sText;
    BOOL m_bWholeWord;
    BOOL m_bRegExp;
  };

#endif // STRUCT_LAST_SEARCH_INFO

/////////////////////////////////////////////////////////////////////////////
// CFindTextDlg dialog

class EDITPADC_CLASS CFindTextDlg : public CDialog
  {
private :
    void UpdateControls();
    CCrystalTextView * m_pBuddy;

    // Construction
public :
    CFindTextDlg (CCrystalTextView * pBuddy);
    void SetLastSearch (LPCTSTR sText, BOOL bMatchCase, BOOL bWholeWord, BOOL bRegExp, int nDirection);
    void UseLastSearch ();
    LastSearchInfos * GetLastSearchInfos (); 

    CPoint m_ptCurrentPos;
    LastSearchInfos lastSearch;

    // Dialog Data
    //{{AFX_DATA(CFindTextDlg)
  enum { IDD = IDD_EDIT_FIND };
    CMemComboBox m_ctlFindText;
    CButton m_ctlWholeWord;
    int m_nDirection;
    BOOL m_bMatchCase;
    CString m_sText;
    BOOL m_bWholeWord;
    BOOL m_bRegExp;
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CFindTextDlg)
protected :
    virtual void DoDataExchange (CDataExchange * pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected :

    void UpdateRegExp ();
    void UpdateLastSearch ();


    // Generated message map functions
    //{{AFX_MSG(CFindTextDlg)
    virtual void OnOK ();
    afx_msg void OnChangeEditText ();
    afx_msg void OnChangeSelected ();
    virtual BOOL OnInitDialog ();
    virtual void OnCancel ();
    afx_msg void OnRegExp ();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP ()
  };

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CFINDTEXTDLG_H__F59009E3_7B01_11D2_8C4F_0080ADB86836__INCLUDED_)
