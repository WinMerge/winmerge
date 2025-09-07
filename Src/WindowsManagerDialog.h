// WindowsManagerDialog.h : header file

#pragma once

class CWindowsManager;

#define WMD_LISTCOLOR_BKG					RGB(255, 255, 213)
#define WMD_LISTCOLOR_BKGSEL				RGB(255, 165, 0)

#ifndef WMU_ISOPEN
#define WMU_ISOPEN							(WM_APP + 33)
#endif
#ifndef WMU_SELECTNEXT
#define WMU_SELECTNEXT						(WM_APP + 34)
#endif
/////////////////////////////////////////////////////////////////////////////
// CWindowsManagerDialog dialog

class CWindowsManagerDialog : public CDialog
{
// Construction
public:
	CWindowsManagerDialog(CWindowsManager& manager, CWnd* pParent = NULL);   // standard constructor
	BOOL Create(UINT nID, CWnd* pWnd = NULL, BOOL bAutoCleanup = TRUE)
	{
		m_bAutoCleanup = bAutoCleanup;
		m_pFrame = DYNAMIC_DOWNCAST(CMDIFrameWnd, pWnd);

		return CDialog::Create(nID, pWnd);
	}

// Dialog Data
	//{{AFX_DATA(CWindowsManagerDialog)
	enum { IDD = IDD_DIALOG_WINDOWSMANAGER };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

public:
	void PopulateList();
	void SetParentWnd(CWnd* pWnd);
	CWnd* GetParentWnd() { return DYNAMIC_DOWNCAST(CWnd, m_pFrame); }
	BOOL GetAutoCleanup() const { return m_bAutoCleanup; }
	void SetAutoCleanup(const BOOL bSet) { m_bAutoCleanup = bSet; }

protected:
	CListCtrl m_List;
	CImageList* m_pIL;
	BOOL m_bAutoCleanup;
	CMDIFrameWnd* m_pFrame;
	CWindowsManager& m_manager;

protected:
	void AdjustSize();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWindowsManagerDialog)
public:
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual ~CWindowsManagerDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWindowsManagerDialog)
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnDestroy();
	afx_msg void OnNMCustomdrawListFile(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMDblclkListFile(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	afx_msg LRESULT OnIsOpen(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSelectNext(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
