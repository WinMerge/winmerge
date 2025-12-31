/** 
 * @file  CDirFilterBar.h
 *
 * @brief Declaration file for Directory compare statusdialog class
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
	String SaveAndGetFilterText();
	void ShowFilterMaskMenu();

// Dialog Data
	//{{AFX_DATA(CDirFilterBar)
	enum { IDD = IDD_DISPLAY_FILTER_BAR };
	CSuperComboBox m_ctlMask;
	CValidatingEdit m_ctlMaskEdit;
	CFileFilterHelperMenu m_menu;
	std::unique_ptr<FileFilterHelper> m_pFileFilterHelper;
	String m_sMask;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CDirFilterBar)
	afx_msg void OnKillFocusFilterfileMask();
	afx_msg void OnEditChangeFilterfileMask();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
};

