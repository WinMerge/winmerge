/**
 * @file  VsVersionDlg.cpp
 *
 * @brief Declaration of Visual Studio Version Dialog
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$


#ifndef VsVersionDlg_h_included
#define VsVersionDlg_h_included
#pragma once

/////////////////////////////////////////////////////////////////////////////
// CVsVersionDlg dialog

#ifndef CMoveConstraint_h
#include "CMoveConstraint.h"
#endif

class CVsVersionDlg : public CDialog
{
// Construction
public:
	CVsVersionDlg(const CStringArray & VsBaseDirs, CWnd* pParent = NULL);   // standard constructor

	VS_VERSION m_nVersion;

	static VS_VERSION MapRegistryValue(const CString & val);

// Implementation
private:
	VS_VERSION m_bestversion;
	prdlg::CMoveConstraint m_constraint;
	const CStringArray & m_VsBaseDirs;

private:

	void DisableUninstalledVersions();
	void CheckVersion(VS_VERSION vsnum);
	void SelectInitialVersion();
	void UpdateInstallDir();
	void CheckVersionButton(VS_VERSION nversion, bool checked);

// Dialog Data
	//{{AFX_DATA(CVsVersionDlg)
	enum { IDD = IDD_VSVERSION };
	CEdit	m_txtInstallDir;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVsVersionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	// Generated message map functions
	//{{AFX_MSG(CVsVersionDlg)
	afx_msg void OnVs2005Btn();
	afx_msg void OnVs2003Btn();
	afx_msg void OnVs2002Btn();
	afx_msg void OnVs6Btn();
	afx_msg void OnVs5Btn();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // VsVersionDlg_h_included
