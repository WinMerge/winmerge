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
/** 
 * @file  OpenDlg.h
 *
 * @brief Declaration file for COpenDlg dialog
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$
#if !defined(AFX_OPENDLG_H__69FB0D77_2A05_11D1_BA92_00A024706EDC__INCLUDED_)
#define AFX_OPENDLG_H__69FB0D77_2A05_11D1_BA92_00A024706EDC__INCLUDED_
#pragma once

/////////////////////////////////////////////////////////////////////////////
// COpenDlg dialog
#include "SuperComboBox.h"

#ifndef CMoveConstraint_h
#include "CMoveConstraint.h"
#endif
#include "FileTransform.h"

/** @brief File open dialog displayed for user to choose directories or files */
class COpenDlg : public CDialog
{
// Construction
public:

	void UpdateButtonStates();
	COpenDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL SelectFile(CString& path, LPCTSTR pszFolder);

// Dialog Data
	//{{AFX_DATA(COpenDlg)
	enum { IDD = IDD_OPEN };
	CButton	m_ctlSelectUnpacker;
	CEdit	m_ctlUnpacker;
	CSuperComboBox	m_ctlExt;
	CButton	m_ctlOk;
	CButton	m_ctlRecurse;
	CSuperComboBox	m_ctlRight;
	CSuperComboBox	m_ctlLeft;
	CString	m_strLeft;
	CString	m_strRight;
	BOOL	m_bRecurse;
	CString	m_strExt;
	CString	m_strUnpacker;
	//}}AFX_DATA

// other public data
	int m_pathsType; // enum from PATH_EXISTENCE in paths.h
	/// unpacker info
	PackingInfo m_infoHandler;
	BOOL m_bFileFilterSelected;
	BOOL m_bOverwriteRecursive;  /**< If TRUE overwrite last used value of recursive */

// Implementation data
private:
	prdlg::CMoveConstraint m_constraint; 


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpenDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void RemoveTrailingSlash(CString& s);
	void SetStatus(UINT msgID);
	void SetUnpackerStatus(UINT msgID);
	void CenterToMainFrame();

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
	afx_msg void OnSelectUnpacker();
	afx_msg void OnSelectFilter();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPENDLG_H__69FB0D77_2A05_11D1_BA92_00A024706EDC__INCLUDED_)
