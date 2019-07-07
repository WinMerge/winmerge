/** 
 * @file  TestFilterDlg.h
 *
 * @brief Declaration file for CTestFilterDlg class
 *
 */
#pragma once

#include "TrDialogs.h"

struct FileFilter;
class FileFilterMgr;

/**
 * @brief Dialog allowing user to test out file filter strings.
 * The user can type a text and test if selected file filter matches to it.
 */
class CTestFilterDlg : public CTrDialog
{
// Construction
public:
	CTestFilterDlg(CWnd* pParent, FileFilter * pFileFilter, FileFilterMgr *pFilterMgr);

// Implementation data
private:
	FileFilter * m_pFileFilter; /**< Selected file filter. */
	FileFilterMgr * m_pFileFilterMgr; /**< File filter manager. */

// Implementation methods
private:
	bool CheckText(String text) const;
	void AppendResult(const String& result);

// Dialog Data
	//{{AFX_DATA(CTestFilterDlg)
	enum { IDD = IDD_TEST_FILTER };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestFilterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


	// Generated message map functions
	//{{AFX_MSG(CTestFilterDlg)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnTestBtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
