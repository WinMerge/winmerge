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
 * @file  WMGotoDlg.h
 *
 * @brief Declaration file for WMGotoDlg dialog.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#if !defined(AFX_WMGOTODLG_H__A9D2366D_6358_4A74_9A45_6681D22EC786__INCLUDED_)
#define AFX_WMGOTODLG_H__A9D2366D_6358_4A74_9A45_6681D22EC786__INCLUDED_


/**
 * @brief Class for Goto-dialog.
 * This dialog allows user to go to certain line or or difference in the file
 * compare. As there are two panels with different line numbers, there is a
 * choice for target panel. When dialog is opened, its values are initialized
 * for active file's line number.
 */
class WMGotoDlg : public CDialog
{
// Construction
public:
	WMGotoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(WMGotoDlg)
	enum { IDD = IDD_WMGOTO };
	CString m_strParam;  /**< Line/difference number. */
	int m_nFile;         /**< Target file number. */
	int m_nGotoWhat;     /**< Goto line or difference? */
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(WMGotoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(WMGotoDlg)
    virtual BOOL OnInitDialog();
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WMGOTODLG_H__A9D2366D_6358_4A74_9A45_6681D22EC786__INCLUDED_)
