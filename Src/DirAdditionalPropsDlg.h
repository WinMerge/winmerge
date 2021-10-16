/** 
 * @file  DirAdditionalPropsDlg.h
 *
 * @brief Declaration file for CDirAdditionalPropsDlg
 */
#pragma once

#include <vector>
#include "TrDialogs.h"
#include "UnicodeString.h"

/////////////////////////////////////////////////////////////////////////////
// CDirAdditionalPropsDlg dialog

/**
 * @brief A Dialog for choosing visible folder compare columns.
 * This class implements a dialog for choosing visible columns in folder
 * compare. Columns can be also re-ordered. There is one listview component
 * which lists all available columns. Every column name has a checkbox with
 * it. If the checkbox is checked, the column is visible.
 *
 * @note: Due to how columns handling code is implemented, hidden columns
 * must be always be last in the list with order number -1.
 * @todo: Allow hidden columns between visible columns.
 */
class CDirAdditionalPropsDlg : public CTrDialog
{
// Public types
public:

// Construction
public:
	explicit CDirAdditionalPropsDlg(const std::vector<String>& canonicalNames, CWnd* pParent = nullptr);   // standard constructor
	const std::vector<String>& GetSelectedCanonicalNames() const { return m_canonicalNames; }

// Dialog Data
	//{{AFX_DATA(CDirAdditionalPropsDlg)
	enum { IDD = IDD_DIRADDITIONALPROPS };
	CTreeCtrl m_treeProps;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirAdditionalPropsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation methods
protected:

// Implementation data
private:
	std::vector<String> m_canonicalNames;
	// Generated message map functions
	//{{AFX_MSG(CDirAdditionalPropsDlg)
	virtual BOOL OnInitDialog() override;
	virtual void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
