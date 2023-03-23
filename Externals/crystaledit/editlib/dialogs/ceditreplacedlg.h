////////////////////////////////////////////////////////////////////////////
//  File:       ceditreplacedlg.h
//  Version:    1.0.0.0
//  Created:    29-Dec-1998
//
//  Author:     Stcherbatchenko Andrei
//  E-mail:     windfall@gmx.de
//
//  Declaration of the CEditReplaceDlg dialog, a part of Crystal Edit -
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

#pragma once
// ceditreplacedlg.h : header file
//

#include "resource.h"
#include "memcombo.h"
#include "cfindtextdlg.h" // for structure LastSearchInfos
#include "FindTextHelper.h"
#include "cepoint.h"

class CCrystalEditView;

/////////////////////////////////////////////////////////////////////////////
// CEditReplaceDlg dialog

class EDITPADC_CLASS CEditReplaceDlg : public CDialog
  {
private :
    CCrystalEditView * m_pBuddy;
    bool m_bFound;
    CEPoint m_ptFoundAt;
    bool DoHighlightText ( bool bNotifyIfNotFound, bool bUpdateView = true );
    bool DoReplaceText (const tchar_t* pszNewText, findtext_flags_t dwSearchFlags);
    void UpdateControls();
    void FindNextPrev (bool bNext);
    bool AdjustSearchPos (CEPoint& ptFoundAt) const;

    // Construction
public :
    explicit CEditReplaceDlg (CCrystalEditView * pBuddy);
    void SetLastSearch (const tchar_t* sText, bool bMatchCase, bool bWholeWord, bool bRegExp, int nScope, int nDirection);
    void UseLastSearch ();
    LastSearchInfos * GetLastSearchInfos (); 
    void SetScope(bool bWithSelection);


    bool m_bEnableScopeSelection;
    CEPoint m_ptCurrentPos;
    CEPoint m_ptBlockBegin, m_ptBlockEnd;
    LastSearchInfos lastSearch;

    // Dialog Data
    //{{AFX_DATA(CEditReplaceDlg)
  enum { IDD = IDD_EDIT_REPLACE };
    CMemComboBox m_ctlFindText;
    CMemComboBox m_ctlReplText;
    CButton m_ctlWholeWord;
    bool m_bMatchCase;
    bool m_bWholeWord;
    bool m_bRegExp;
    CString m_sText;
    CString m_sNewText;
    int m_nScope;
    bool m_bDontWrap;
    int m_nDirection;
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CEditReplaceDlg)
  public:
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  protected:
    virtual void DoDataExchange (CDataExchange * pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected :

    void UpdateRegExp ();
    void UpdateLastSearch ();

    // Generated message map functions
    //{{AFX_MSG(CEditReplaceDlg)
    afx_msg void OnChangeEditText ();
    afx_msg void OnChangeSelected ();
    virtual void OnCancel ();
    virtual BOOL OnInitDialog ();
    afx_msg void OnEditReplace ();
    afx_msg void OnEditReplaceAll ();
    afx_msg void OnEditSkip ();
    afx_msg void OnEditFindPrev();
    afx_msg void OnRegExp ();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP ()
  };
