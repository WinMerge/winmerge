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
/**
 *  @file cfindtextdlg.h
 *
 *  @brief Declaration Find-dialog.
 */

#pragma once

#include "resource.h"
#include "memcombo.h"
#include "FindTextHelper.h"

class CCrystalTextView;

/////////////////////////////////////////////////////////////////////////////
// CFindTextDlg dialog

class EDITPADC_CLASS CFindTextDlg : public CDialog
  {
private :
    void UpdateControls();
    CCrystalTextView * m_pBuddy;
    CFont m_font;

    // Construction
public :
    explicit CFindTextDlg (CCrystalTextView * pBuddy);
    void SetLastSearch (const tchar_t* sText, bool bMatchCase, bool bWholeWord, bool bRegExp, int nDirection);
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
    bool m_bMatchCase;
    CString m_sText;
    bool m_bWholeWord;
    bool m_bRegExp;
    bool m_bNoWrap;
    bool m_bNoClose; 
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
    void FindText (int nDirection);

    // Generated message map functions
    //{{AFX_MSG(CFindTextDlg)
    virtual void OnOK ();
    afx_msg void OnChangeEditText ();
    afx_msg void OnChangeSelected ();
    virtual BOOL OnInitDialog ();
    virtual void OnCancel ();
    afx_msg void OnRegExp ();
    afx_msg void OnFindPrev ();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP ()
  };
