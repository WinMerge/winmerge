// MakePatchDirsDlg.h : header file
//

#if !defined(AFX_MAKEPATCHDIRSDLG_H__E35CCFFC_0CBB_424C_85F2_3CF36C2672F6__INCLUDED_)
#define AFX_MAKEPATCHDIRSDLG_H__E35CCFFC_0CBB_424C_85F2_3CF36C2672F6__INCLUDED_
#pragma once

/////////////////////////////////////////////////////////////////////////////
// CMakePatchDirsDlg dialog

#if !defined(AFX_DROPEDIT_H__1D8BBDC1_784C_11D1_8159_444553540000__INCLUDED_)
#include "DropEdit.h"
#endif
#ifndef CMoveConstraint_h
#include "CMoveConstraint.h"
#endif


class CMakePatchDirsDlg : public CDialog
{
// Construction
public:
	CMakePatchDirsDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMakePatchDirsDlg)
	enum { IDD = IDD_MAKEPATCHDIRS_DIALOG };
	CDropEdit	m_dir;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMakePatchDirsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	prdlg::CMoveConstraint m_constraint;

	// Generated message map functions
	//{{AFX_MSG(CMakePatchDirsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDirBrowse();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAKEPATCHDIRSDLG_H__E35CCFFC_0CBB_424C_85F2_3CF36C2672F6__INCLUDED_)
