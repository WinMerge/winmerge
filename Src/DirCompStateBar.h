/** 
 * @file  DirCompStateBar.h
 *
 * @brief Declaration file for Directory compare statuspanel class
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#if !defined(AFX_DIRCOMPSTATEDLG_H__8F66C090_C232_429F_A4A2_18D43CCC6C38__INCLUDED_)
#define AFX_DIRCOMPSTATEDLG_H__8F66C090_C232_429F_A4A2_18D43CCC6C38__INCLUDED_


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
	void ClearStat();
	void AddElement(UINT diffcode);
	BOOL GetDefaultRect( LPRECT lpRect ) const;
	void UpdateText(CStatic * ctrl, int num) const;
	void FirstUpdate();

// Dialog Data
	//{{AFX_DATA(CDirCompStateBar)
	enum { IDD = IDD_DIRCOMPSTATE };
	int		m_nBinaryDiff;
	int		m_nBinarySame;
	int		m_nEqual;
	int		m_nFileSkip;
	int		m_nFolderSkip;
	int		m_nLFile;
	int		m_nLFolder;
	int		m_nNotEqual;
	int		m_nRFile;
	int		m_nRFolder;
	int		m_nUnknown;
	CButton	m_ctlStop;
	CStatic m_ctlLFolder;
	CStatic m_ctlRFolder;
	CStatic m_ctlEqual;
	CStatic m_ctlNotEqual;
	CStatic m_ctlBinarySame;
	CStatic m_ctlBinaryDiff;
	CStatic m_ctlLFile;
	CStatic m_ctlRFile;
	CStatic m_ctlFolderSkip;
	CStatic m_ctlLFileSkip;
	CStatic m_ctlLUnknown;
	//}}AFX_DATA

	CString strAbort;
	CString strClose;

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

	// Generated message map functions
	//{{AFX_MSG(CDirCompStateBar)
	afx_msg void OnStop();
	afx_msg void OnUpdateStop(CCmdUI* pCmdUI);
	afx_msg void OnWindowPosChanging( WINDOWPOS* lpwndpos );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL m_bFirstUpdate; /**< Is first update (all to zeros) done? */
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRCOMPSTATEDLG_H__8F66C090_C232_429F_A4A2_18D43CCC6C38__INCLUDED_)

