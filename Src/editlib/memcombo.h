///////////////////////////////////////////////////////////////////////////
//  File:    memcombo.h
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Ferdinand Prantl
//  E-mail:     prantl@ff.cuni.cz
//
//  Combo-box saving last typed expressions
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#ifndef __MEMCOMBO_H__INCLUDED__
#define __MEMCOMBO_H__INCLUDED__

////////////////////////////////////////////////////////////////////////////////
// Includes

////////////////////////////////////////////////////////////////////////////////
// All Classes and Class Templates

class CMemComboBox;

////////////////////////////////////////////////////////////////////////////////
// Includes

/////////////////////////////////////////////////////////////////////////////
// CMemComboBox Class Declaration

class EDITPADC_CLASS CMemComboBox : public CComboBox
  {
    // Construction
public :
    CMemComboBox ();

    // Attributes
public :
    static CMap < CString, LPCTSTR, CString, LPCTSTR > groups;
    BOOL m_bFirstFocus;
    CString m_sGroup;

    // Operations
public :
    void FillCurrent ();
    void Fill (LPCTSTR text);
    static void LoadSettings ();
    static void SaveSettings ();

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMemComboBox)
    //}}AFX_VIRTUAL

    // Implementation
public :
    virtual ~CMemComboBox ();

    // Generated message map functions
public :
    //{{AFX_MSG(CMemComboBox)
    afx_msg void OnSetfocus ();
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP ()
  };

/////////////////////////////////////////////////////////////////////////////

#ifndef _DEBUG
#include "memcombo.inl"
#endif // _DEBUG

/////////////////////////////////////////////////////////////////////////////

#endif // __MEMCOMBO_H__INCLUDED__
