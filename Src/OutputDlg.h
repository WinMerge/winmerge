// OutputDlg.h : header file
//

#if !defined(AFX_OUTPUTDLG_H__7F99AF10_24E5_49AF_8D97_9A86A80BAD48__INCLUDED_)
#define AFX_OUTPUTDLG_H__7F99AF10_24E5_49AF_8D97_9A86A80BAD48__INCLUDED_


/////////////////////////////////////////////////////////////////////////////
// COutputDlg dialog

void OutputBox(LPCTSTR szTitle, LPCTSTR szText);

class COutputDlg : public CDialog
{
// Construction
public:
	COutputDlg(const CString & sTitle, const CString & sText);

// Dialog Data
	//{{AFX_DATA(COutputDlg)
	enum { IDD = IDD_OUTPUT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutputDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COutputDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CString m_sTitle;
	CString m_sText;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OUTPUTDLG_H__7F99AF10_24E5_49AF_8D97_9A86A80BAD48__INCLUDED_)
