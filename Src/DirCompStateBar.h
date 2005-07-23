/** 
 * @file  DirCompStateBar.h
 *
 * @brief Declaration file for Directory compare statuspanel class
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#if !defined(AFX_DIRCOMPSTATEDLG_H__8F66C090_C232_429F_A4A2_18D43CCC6C38__INCLUDED_)
#define AFX_DIRCOMPSTATEDLG_H__8F66C090_C232_429F_A4A2_18D43CCC6C38__INCLUDED_

#ifndef _COMPARESTATS_H_
#include "CompareStats.h"
#endif

class CDirFrame;

/////////////////////////////////////////////////////////////////////////////
// CDirCompStateBar dialog

/**
 * @brief Class for directory compare statuspanel
 * 
 * Implements floating statuspanel during directory compares.
 * Shows statistics from compare and allows stopping compare.
 */
class CDirCompStateBar : public CDialogBar
{
// Construction
public:
	CDirCompStateBar(CWnd* pParent = NULL);   // standard constructor
	BOOL Create(CWnd* pParentWnd);
	BOOL GetDefaultRect( LPRECT lpRect ) const;
	void UpdateText(CStatic * ctrl, int num) const;
	void Reset();
	void UpdateElements();
	void SetCompareStat(CompareStats * pCompareStats);
	void StartUpdating();
	void EndUpdating();
	UINT GetIDFromResult(CompareStats::RESULT res);

// Dialog Data
	//{{AFX_DATA(CDirCompStateBar)
	enum { IDD = IDD_DIRCOMPSTATE };
	CButton	m_ctlStop;
	//}}AFX_DATA

// Overrides
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirCompStateBar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void ClearStat();

	// Generated message map functions
	//{{AFX_MSG(CDirCompStateBar)
	afx_msg void OnStop();
	afx_msg void OnUpdateStop(CCmdUI* pCmdUI);
	afx_msg void OnWindowPosChanging( WINDOWPOS* lpwndpos );
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CString strAbort; /**< 'Stop' text for button */
	CString strClose; /**< 'Close' text for button */
	long m_lElapsed; /**< Elapsed time for compare */
	CompareStats *m_pCompareStats; /**< Pointer to comparestats */
	BOOL m_bStopText; /**< Button has 'Stop' text? */
	CompareStats::CMP_STATE m_prevState; /**< Previous state for compare (to track changes) */
	BOOL m_bCompareReady; /**< Compare ready, waiting for closing? */
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRCOMPSTATEDLG_H__8F66C090_C232_429F_A4A2_18D43CCC6C38__INCLUDED_)

