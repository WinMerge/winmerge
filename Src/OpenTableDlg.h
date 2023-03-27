/** 
 * @file  OpenTableDlg.h
 *
 * @brief Declaration of the dialog used to select codepages
 */
#pragma once

#include "TrDialogs.h"
#include "UnicodeString.h"

/////////////////////////////////////////////////////////////////////////////
// COpenTableDlg dialog

class COpenTableDlg : public CTrDialog
{
public:
// Construction
	explicit COpenTableDlg(CWnd* pParent = nullptr);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COpenTableDlg)
	enum { IDD = IDD_OPEN_TABLE };
	int m_nFileType;
	String m_sCSVDelimiterChar;
	String m_sDSVDelimiterChar;
	String m_sDelimiterChar;
	String m_sQuoteChar;
	bool m_bAllowNewlinesInQuotes;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpenTableDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK() override;
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COpenTableDlg)
	afx_msg void OnClickFileType(UINT nID);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
