/////////////////////////////////////////////////////////////////////////////
//
//	CDropEdit
//	Copyright 1997 Chris Losinger
//
//	This code is freely distributable and modifiable, as long as credit
//	is given to where it's due. Watch, I'll demonstrate :
//
//	shortcut expansion code modified from :
//	CShortcut, 1996 Rob Warner
//

#if !defined(AFX_DROPEDIT_H__1D8BBDC1_784C_11D1_8159_444553540000__INCLUDED_)
#define AFX_DROPEDIT_H__1D8BBDC1_784C_11D1_8159_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// DropEdit.h : header file
//

#include <shlobj.h>

/////////////////////////////////////////////////////////////////////////////
// CDropEdit window

class CDropEdit : public CEdit
{
// Construction
public:
	CDropEdit();

// Attributes
public:

// Operations
public:
    void AllowFileDrop(BOOL bAllow = TRUE)      { m_bAllowFiles = bAllow; }
    BOOL AllowingFileDrop()                     { return m_bAllowFiles;   }
    void AllowDirDrop(BOOL bAllow = TRUE)       { m_bAllowDirs = bAllow;  }
    BOOL AllowingDirDrop()                      { return m_bAllowDirs;    }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDropEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDropEdit();

protected:
	CString ExpandShortcut(CString &inFile);

	// Generated message map functions
protected:
	//{{AFX_MSG(CDropEdit)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDropFiles(HDROP dropInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	BOOL m_bAllowFiles, m_bAllowDirs;
	BOOL m_bMustUninitOLE;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DROPEDIT_H__1D8BBDC1_784C_11D1_8159_444553540000__INCLUDED_)
