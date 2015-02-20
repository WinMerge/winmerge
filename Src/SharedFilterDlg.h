/** 
 * @file  SharedFilterDlg.h
 *
 * @brief Declaration file for CSharedFilterDlg.
 *
 */
#pragma once

/////////////////////////////////////////////////////////////////////////////
// CSharedFilterDlg dialog

/**
 * @brief A dialog for selecting shared/private filter creation.
 * This dialog allows user to select if the new filter is a shared filter
 * (placed into WinMerge executable's subfolder) or private filter
 * (placed into profile folder).
 */
class CSharedFilterDlg : public CDialog
{
public:
	static String PromptForNewFilter(CWnd * Parent, const String &SharedFolder,
		const String &PrivateFolder);

// Construction
public:
	CSharedFilterDlg(CWnd* pParent);   // standard constructor

// Implementation
protected:

// Dialog Data
	//{{AFX_DATA(CSharedFilterDlg)
	enum { IDD = IDD_SHARED_FILTER };
	CButton m_SharedButton;
	CButton m_PrivateButton;
	//}}AFX_DATA
	String m_SharedFolder;  /**< Folder for shared filters. */
	String m_PrivateFolder; /**< Folder for private filters. */


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSharedFilterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


	// Generated message map functions
	//{{AFX_MSG(CSharedFilterDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Implementation data
private:
	String m_ChosenFolder;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
