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

#ifndef __STATBAR_H__INCLUDED__
#define __STATBAR_H__INCLUDED__

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
    BOOL SetPaneFormattedText (int nIndex, BOOL bUpdate, LPCTSTR lpszFmt,...);
    BOOL SetPaneFormattedText (int nIndex, BOOL bUpdate, UINT nId,...);
    BOOL SetPaneText (int nIndex, LPCTSTR lpszNewText, BOOL bUpdate = TRUE);
    BOOL SetPaneText (int nIndex, UINT nId, BOOL bUpdate = TRUE);
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

#endif // __STATBAR_H__INCLUDED__
