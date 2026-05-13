/** 
 * @file  DirFilterBar.h
 *
 * @brief Declaration file for CDirFilterBar dialog bar
 */
#pragma once

#include "TrDialogs.h"
#include "SuperComboBox.h"
#include "ValidatingEdit.h"
#include "FileFilterHelperMenu.h"

/////////////////////////////////////////////////////////////////////////////
// CDirFilterBar dialog

class CDirFilterBar : public CTrDialogBar
{
// Construction
public:
	CDirFilterBar();   // standard constructor
	~CDirFilterBar();
	BOOL Create(CWnd* pParentWnd);
	String GetFilterText() { return m_sFilter; }
	void SaveFilterText();
	void ShowFilterMenu();

// Dialog Data
	//{{AFX_DATA(CDirFilterBar)
	enum { IDD = IDD_DISPLAY_FILTER_BAR };
	CSuperComboBox m_ctlFilter;
	CValidatingEdit m_ctlFilterEdit;
	CFileFilterHelperMenu m_menu;
	std::unique_ptr<FileFilterHelper> m_pFileFilterHelper;
	String m_sFilter;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CDirFilterBar)
	afx_msg void OnKillFocusFilter();
	afx_msg void OnEditChangeFilter();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
};

