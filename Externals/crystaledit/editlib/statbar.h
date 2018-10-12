///////////////////////////////////////////////////////////////////////////
//  File:    statbar.h
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Ferdinand Prantl, portions by Stcherbatchenko Andrei
//  E-mail:     prantl@ff.cuni.cz
//
//  Status bar extension
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#pragma once

class EDITPADC_CLASS CEditStatusBar : public CStatusBar
  {
    DECLARE_DYNCREATE (CEditStatusBar)
public :
    CEditStatusBar ();
    ~CEditStatusBar ();
    virtual BOOL Create (CWnd * pParentWnd, DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_BOTTOM, UINT nID = AFX_IDW_STATUS_BAR);

protected :
    CString m_strClockFormat;

public :
    bool SetPaneFormattedText (int nIndex, bool bUpdate, LPCTSTR lpszFmt,...);
    bool SetPaneFormattedText (int nIndex, bool bUpdate, UINT nId,...);
    bool SetPaneText (int nIndex, LPCTSTR lpszNewText, bool bUpdate = true);
    bool SetPaneText (int nIndex, UINT nId, bool bUpdate = true);
    void SetClockFormat (LPCTSTR strClockFormat);

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CEditStatusBar)
    //}}AFX_VIRTUAL

    // Generated message map functions
    //{{AFX_MSG(CEditStatusBar)
    afx_msg void OnDestroy ();
    afx_msg void OnUpdateIndicatorTime (CCmdUI * pCmdUI);
    afx_msg int OnCreate (LPCREATESTRUCT lpCreateStruct);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP ()
  };
