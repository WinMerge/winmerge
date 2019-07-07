/** 
 * @file  SharedFilterDlg.h
 *
 * @brief Declaration file for CSharedFilterDlg.
 *
 */
#pragma once

#include "TrDialogs.h"

/////////////////////////////////////////////////////////////////////////////
// CSharedFilterDlg dialog

/**
 * @brief A dialog for selecting shared/private filter creation.
 * This dialog allows user to select if the new filter is a shared filter
 * (placed into WinMerge executable's subfolder) or private filter
 * (placed into profile folder).
 */
class CSharedFilterDlg : public CTrDialog
{
// Construction
public:
	enum FilterType : int { SHARED = 0, PRIVATE = 1 };
	explicit CSharedFilterDlg(FilterType type, CWnd* pParent = nullptr);
	FilterType GetSelectedFilterType() const { return m_selectedType; }

// Implementation
protected:

// Dialog Data
	//{{AFX_DATA(CSharedFilterDlg)
	enum { IDD = IDD_SHARED_FILTER };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSharedFilterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


	// Generated message map functions
	//{{AFX_MSG(CSharedFilterDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Implementation data
private:
	FilterType m_selectedType;
};
