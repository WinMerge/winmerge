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
#if !defined(AFX_DIRFRAME_H__95565903_35C4_11D1_BAA7_00A024706EDC__INCLUDED_)
#define AFX_DIRFRAME_H__95565903_35C4_11D1_BAA7_00A024706EDC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DirFrame.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CDirFrame frame

class CDirFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CDirFrame)
protected:
	CDirFrame();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
	void SetStatus(LPCTSTR szStatus);
	CStatusBar  m_wndStatusBar;
	void SetClosableCallback(bool (*canclose)(void *), void * param);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirFrame)
	public:
	virtual void ActivateFrame(int nCmdShow = -1);
	//}}AFX_VIRTUAL

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual ~CDirFrame();

	// Generated message map functions
	//{{AFX_MSG(CDirFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdateStatusNum(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	bool (*m_picanclose)(void *);
	void * m_param;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRFRAME_H__95565903_35C4_11D1_BAA7_00A024706EDC__INCLUDED_)
