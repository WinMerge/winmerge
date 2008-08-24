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
 * @file  CCPrompt.h
 *
 * @brief Declaration file for ClearCase dialog.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#if !defined(AFX_CCPROMPT_H__7ED564F2_CCB8_11D4_92BB_00B0D0221937__INCLUDED_)
#define AFX_CCPROMPT_H__7ED564F2_CCB8_11D4_92BB_00B0D0221937__INCLUDED_

/**
 * @brief A dialog for ClearCase checkout/checkin.
 */
class CCCPrompt : public CDialog
{
// Construction
public:
	CCCPrompt(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCCPrompt)
	enum { IDD = IDD_CLEARCASE };
	CString	m_comments;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCCPrompt)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL OnInitDialog();

	// Generated message map functions
	//{{AFX_MSG(CCCPrompt)
	afx_msg void OnSaveas();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	BOOL m_bMultiCheckouts;
	BOOL m_bCheckin;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CCPROMPT_H__7ED564F2_CCB8_11D4_92BB_00B0D0221937__INCLUDED_)
