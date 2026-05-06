/** 
 * @file  LineFilterBar.h
 *
 * @brief Declaration file for CLineFilterBar dialog bar
 */
#pragma once

#include "TrDialogs.h"
#include "SuperComboBox.h"
#include "ValidatingEdit.h"
#include "LineFilterHelperMenu.h"

class LineFilterHelper;

/////////////////////////////////////////////////////////////////////////////
// CLineFilterBar dialog

class CLineFilterBar : public CTrDialogBar
{
// Construction
public:
	CLineFilterBar();   // standard constructor
	~CLineFilterBar();
	BOOL Create(CWnd* pParentWnd);
	String GetFilterText() { return m_sFilter; }
	void SaveFilterText();
	void ShowFilterMenu();

// Dialog Data
	//{{AFX_DATA(CLineFilterBar)
	enum { IDD = IDD_DISPLAY_FILTER_BAR };
	CSuperComboBox m_ctlFilter;
	CValidatingEdit m_ctlFilterEdit;
	CLineFilterHelperMenu m_menu;
	std::unique_ptr<LineFilterHelper> m_pLineFilterHelper;
	String m_sFilter;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CLineFilterBar)
	afx_msg void OnKillFocusFilter();
	afx_msg void OnEditChangeFilter();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
};

