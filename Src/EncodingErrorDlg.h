// EncodingErrorDlg.h : header file
//

#if !defined(AFX_ENCODINGERRORDLG_H__7F99AF10_24E5_49AF_8D97_9A86A80BAD48__INCLUDED_)
#define AFX_ENCODINGERRORDLG_H__7F99AF10_24E5_49AF_8D97_9A86A80BAD48__INCLUDED_


/////////////////////////////////////////////////////////////////////////////
// CEncodingErrorDlg dialog

class CEncodingErrorDlg : public CDialog
{
// Construction
public:
	CEncodingErrorDlg(const CString & sText);

// Dialog Data
	//{{AFX_DATA(CEncodingErrorDlg)
	enum { IDD = IDD_ENCODINGERROR };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEncodingErrorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEncodingErrorDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CString m_sText;
public:
	afx_msg void OnBnClickedEncoding();
	afx_msg void OnBnClickedPlugin();
	afx_msg void OnBnClickedHexView();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENCODINGERRORDLG_H__7F99AF10_24E5_49AF_8D97_9A86A80BAD48__INCLUDED_)
