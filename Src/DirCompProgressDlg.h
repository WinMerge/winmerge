/** 
 * @file  DirCompProgressDlg.h
 *
 * @brief Declaration file for Directory compare statusdialog class
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#if !defined(AFX_DIRCOMPPROGRESSDLG_H__8F66C090_C232_429F_A4A2_18D43CCC6C38__INCLUDED_)
#define AFX_DIRCOMPPROGRESSDLG_H__8F66C090_C232_429F_A4A2_18D43CCC6C38__INCLUDED_

#ifndef _COMPARESTATS_H_
#include "CompareStats.h"
#endif

class CDirFrame;
class CDirDoc;

/////////////////////////////////////////////////////////////////////////////
// DirCompProgressDlg dialog

/**
 * @brief Class for directory compare statusdialog
 * 
 * Implements non-modal dialog directory compares.
 * Shows compare progress and allows stopping compare.
 */
class DirCompProgressDlg : public CDialog
{
	DECLARE_DYNAMIC(DirCompProgressDlg)

// Construction
public:
	DirCompProgressDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL Create(UINT nIDTemplate, CWnd* pParentWnd);
	void SetCompareStat(CompareStats * pCompareStats);
	void StartUpdating();
	void EndUpdating();
	void CenterToParent();
	void CloseDialog();
	void SetDirDoc(CDirDoc *pDirDoc);

// Dialog Data
	//{{AFX_DATA(DirCompProgressDlg)
	enum { IDD = IDD_DIRCOMP_PROGRESS };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DirCompProgressDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void ClearStat();

	// Generated message map functions
	//{{AFX_MSG(DirCompProgressDlg)
	afx_msg BOOL OnInitDialog();
	afx_msg void OnUpdateStop(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedComparisonStop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	long m_lElapsed; /**< Elapsed time for compare */
	CompareStats *m_pCompareStats; /**< Pointer to comparestats */
	CompareStats::CMP_STATE m_prevState; /**< Previous state for compare (to track changes) */
	BOOL m_bCompareReady; /**< Compare ready, waiting for closing? */
	CDirDoc * m_pDirDoc;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRCOMPPROGRESSDLG_H__8F66C090_C232_429F_A4A2_18D43CCC6C38__INCLUDED_)

