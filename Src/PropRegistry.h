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
 * @file  PropRegistry.h
 *
 * @brief Declaration file CPropRegistry
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef PropRegistry_h_included
#define PropRegistry_h_included

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPropRegistry dialog

/**
 * @brief CPropRegistry dialog
 */
class CPropRegistry : public CPropertyPage
{
// Construction
public:

	CPropRegistry();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPropRegistry)
	enum { IDD = IDD_PROPPAGE_REGISTRY };
	BOOL m_bContextAdded;
	CString m_strPath;
	CString	m_strEditorPath;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropRegistry)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation methods
protected:
	bool IsRegisteredForDirectory() const;
	void UpdateButton();
	void EnableContextHandler(bool enabling);
	void GetContextRegValues();

	// Generated message map functions
	//{{AFX_MSG(CPropRegistry)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddToExplorer();
	afx_msg void OnSavePath();
	afx_msg void OnBrowsePath();
	afx_msg void OnBrowseEditor();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Implementation data
protected:
	bool m_enabled;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // PropRegistry_h_included
