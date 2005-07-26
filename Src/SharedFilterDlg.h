// SharedFilterDlg.h : header file
//

#if !defined(AFX_SHAREDFILTERDLG_H__94FD9E42_5C27_49DE_B2FB_77A0B0B03A87__INCLUDED_)
#define AFX_SHAREDFILTERDLG_H__94FD9E42_5C27_49DE_B2FB_77A0B0B03A87__INCLUDED_


/////////////////////////////////////////////////////////////////////////////
// CSharedFilterDlg dialog

class CSharedFilterDlg : public CDialog
{

public:
	static CString PromptForNewFilter(CWnd * Parent, CString SharedFolder, CString PrivateFolder);

// Construction
public:
	CSharedFilterDlg(CWnd* pParent);   // standard constructor

// Implementation
protected:

// Dialog Data
	//{{AFX_DATA(CSharedFilterDlg)
	enum { IDD = IDD_SHARED_FILTER };
	CString m_SharedFolder;
	CString	m_PrivateFolder;
	CButton m_SharedButton;
	CButton m_PrivateButton;
	//}}AFX_DATA


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
	CString m_ChosenFolder;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHAREDFILTERDLG_H__94FD9E42_5C27_49DE_B2FB_77A0B0B03A87__INCLUDED_)
