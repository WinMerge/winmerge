#if !defined(AFX_SELECTUNPACKERDLG_H__C8FD4C3A_5ED5_43D3_ADAE_A2378369705C__INCLUDED_)
#define AFX_SELECTUNPACKERDLG_H__C8FD4C3A_5ED5_43D3_ADAE_A2378369705C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectUnpackerDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelectUnpackerDlgDlg dialog

class PackingInfo;
class PluginInfo;

class CSelectUnpackerDlg : public CDialog
{
private:
// Construction
	void Initialize();

public:
// Construction
	CSelectUnpackerDlg(LPCTSTR filename, CWnd* pParent /*=NULL*/);
	CSelectUnpackerDlg(LPCTSTR filename1, LPCTSTR filename2, CWnd* pParent /*=NULL*/);
	~CSelectUnpackerDlg();

	void SetInitialInfoHandler(PackingInfo * infoHandler);
	const PackingInfo GetInfoHandler();

// Dialog Data
	//{{AFX_DATA(CSelectUnpackerDlg)
	enum { IDD = IDD_SELECTUNPACKER };
	CButton	m_btnOK;
	CComboBox	m_cboUnpackerName;
	BOOL	m_bNoExtensionCheck;
	CString	m_strDescription;
	CString	m_strExtensions;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectUnpackerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	/// arrays for string describing the available plugins
	CPtrArray  m_UnpackerPlugins;
	CUIntArray m_bWithFileFlags;

	// const data "no plugin"
	PluginInfo * noPlugin;
	// const data "automatic plugin"
	PluginInfo * automaticPlugin;

	// input value
	CString m_filteredFilenames;

	/// current plugin choice
	PluginInfo * m_pPlugin;
	/// current plugin choice
	CString m_strPluginName;

	void prepareListbox();

	// Generated message map functions
	//{{AFX_MSG(CSelectUnpackerDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnUnpackerAllowAll();
	afx_msg void OnSelchangeUnpackerName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTUNPACKERDLG_H__C8FD4C3A_5ED5_43D3_ADAE_A2378369705C__INCLUDED_)
