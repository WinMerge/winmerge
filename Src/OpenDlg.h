/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997  Dean P. Grimm
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_OPENDLG_H__69FB0D77_2A05_11D1_BA92_00A024706EDC__INCLUDED_)
#define AFX_OPENDLG_H__69FB0D77_2A05_11D1_BA92_00A024706EDC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// OpenDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COpenDlg dialog
#include "SuperComboBox.h"

class COpenDlg : public CDialog
{
// Construction
public:
	CString m_strParsedExt;
	void UpdateButtonStates();
	COpenDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL SelectFile(CString& path, LPCTSTR pszFolder);

// Dialog Data
	//{{AFX_DATA(COpenDlg)
	enum { IDD = IDD_OPEN };
	CSuperComboBox	m_ctlExt;
	CButton	m_ctlOk;
	CButton	m_ctlRecurse;
	CSuperComboBox	m_ctlRight;
	CSuperComboBox	m_ctlLeft;
	CString	m_strLeft;
	CString	m_strRight;
	BOOL	m_bRecurse;
	CString	m_strExt;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpenDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void RemoveTrailingSlash(CString& s);
	BOOL IsFileOk(const CString& strFile, BOOL *pbDir = NULL) const;
	BOOL AreComparable(BOOL * pbDirs) const;

	// Generated message map functions
	//{{AFX_MSG(COpenDlg)
	afx_msg void OnLeftButton();
	afx_msg void OnRightButton();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeLeftCombo();
	afx_msg void OnSelchangeRightCombo();
	afx_msg void OnEditEvent();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPENDLG_H__69FB0D77_2A05_11D1_BA92_00A024706EDC__INCLUDED_)
