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
 * @file  SaveClosingDlg.h
 *
 * @brief Declaration file for SaveClosingDlg dialog
 */
// RCS ID line follows -- this is updated by CVS
// $Id$


#ifndef _SAVECLOSINGDLG_H_
#define _SAVECLOSINGDLG_H_

/////////////////////////////////////////////////////////////////////////////
// SaveClosingDlg dialog

/**
 * @brief Dialog asking if user wants to save modified left and/or right
 * files.
 *
 * The dialog has separate frames for both files and unneeded frame and
 * controls inside it are disabled. Asked file(s) are selected using
 * DoAskFor() function.
 */
class SaveClosingDlg : public CDialog
{
	DECLARE_DYNAMIC(SaveClosingDlg)

public:
	SaveClosingDlg(CWnd* pParent = NULL);   // standard constructor
	void DoAskFor(BOOL bLeft = FALSE, BOOL bRight = FALSE);

// Dialog Data
	//{{AFX_DATA(SaveClosingDlg)
	enum { IDD = IDD_SAVECLOSING };
	CString m_sLeftFile;
	CString m_sRightFile;
	int m_leftSave;
	int m_rightSave;
	BOOL m_bDisableCancel;
	//}}AFX_DATA

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(SaveClosingDlg)
	afx_msg BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL m_bAskForLeft; //*< Is left file modified? */
	BOOL m_bAskForRight; //*< Is right file modified? */
};

#endif // _SAVECLOSINGDLG_H_
