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
 * @file  PatchDlg.h
 *
 * @brief Declaration file for patch creation dialog
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#if !defined(AFX_PATCHDLG_H__AB3CE671_1328_11D7_B088_005004D9D386__INCLUDED_)
#define AFX_PATCHDLG_H__AB3CE671_1328_11D7_B088_005004D9D386__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// PatchDlg dialog

/** 
 * @brief Filepair to create patch
 */
typedef struct tagPATCHFILES
{
	CString lfile;
	CString rfile;
	time_t ltime, rtime;
} PATCHFILES;


class CPatchDlg : public CDialog
{
// Construction
public:
	CPatchDlg(CWnd* pParent = NULL);   // standard constructor
	
	// Functions to add and get selected files (as PATCHFILEs)
	void AddItem(PATCHFILES pf);
	int GetItemCount();
	POSITION GetFirstItem();
	PATCHFILES GetNextItem(POSITION &pos);
	void SetItemAt(POSITION pos, PATCHFILES pf);
	void ClearItems();

// Dialog Data
	//{{AFX_DATA(CPatchDlg)
	enum { IDD = IDD_GENERATE_PATCH };
	CComboBox	m_comboStyle;
	CComboBox	m_comboContext;
	BOOL	m_caseSensitive;
	CString	m_file1;
	CString	m_file2;
	CString	m_fileResult;
	BOOL	m_ignoreBlanks;
	int		m_whitespaceCompare;
	BOOL	m_appendFile;
	//}}AFX_DATA

	enum output_style m_outputStyle;
	int m_contextLines;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPatchDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CList<PATCHFILES, PATCHFILES> m_fileList;
	
	void AddNewFile(CString sFile, BOOL bLeft);

	// Generated message map functions
	//{{AFX_MSG(CPatchDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDiffBrowseFile1();
	afx_msg void OnDiffBrowseFile2();
	afx_msg void OnDiffBrowseResult();
	afx_msg void OnSelchangeDiffStyle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PATCHDLG_H__AB3CE671_1328_11D7_B088_005004D9D386__INCLUDED_)
