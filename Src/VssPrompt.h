/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  VssPrompt.h
 *
 * @brief Declaration file for CVssPrompt
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#if !defined(AFX_VSSPROMPT_H__F767E53B_90F4_11D1_BB11_00A024706EDC__INCLUDED_)
#define AFX_VSSPROMPT_H__F767E53B_90F4_11D1_BB11_00A024706EDC__INCLUDED_

#include "SuperComboBox.h"
#include "afxwin.h"

/**
 * @brief Class for VSS dialog
 */
class CVssPrompt : public CDialog
{
// Construction
public:
	CVssPrompt(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CVssPrompt)
	enum { IDD = IDD_VSS };
	CSuperComboBox	m_ctlProject;
	CString	m_strProject;
	CString	m_strUser;
	CString	m_strPassword;
	CString	m_strMessage;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVssPrompt)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVssPrompt)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSaveas();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CString m_strSelectedDatabase;
	CComboBox m_ctlDBCombo;
	BOOL m_bMultiCheckouts;
	CButton m_ctlMultiCheckouts;
	BOOL m_bVCProjSync;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VSSPROMPT_H__F767E53B_90F4_11D1_BB11_00A024706EDC__INCLUDED_)
