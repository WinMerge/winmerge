/** 
 * @file  DirColsDlg.h
 *
 * @brief Declaration file for CDirColsDlg
 *
 * @date  Created: 2003-08-19
 */
#pragma once

#include <vector>
#include "TrDialogs.h"
#include "UnicodeString.h"

/////////////////////////////////////////////////////////////////////////////
// CDirColsDlg dialog

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
class CDirColsDlg : public CTrDialog
{
// Public types
public:
	/** @brief One column's information. */
	struct column
	{
		String name; /**< Column name */
		String desc; /**< Description for column */
		int log_col; /**< Logical (shown) order number */
		int phy_col; /**< Physical (in memory) order number */
		column(const String & colName, const String & dsc, int log, int phy)
			: name(colName), desc(dsc), log_col(log), phy_col(phy)
		{ } 
	};
	typedef std::vector<column> ColumnArray;

// Construction
public:
	explicit CDirColsDlg(CWnd* pParent = nullptr);   // standard constructor
	void AddColumn(const String & name, const String & desc, int log, int phy=-1)
		{ column c(name, desc, log, phy); m_cols.push_back(c); }
	void AddDefColumn(const String & name, int log, int phy=-1)
		{ column c(name, _T(""), log, phy); m_defCols.push_back(c); }
	const ColumnArray & GetColumns() const { return m_cols; }

// Dialog Data
	//{{AFX_DATA(CDirColsDlg)
	enum { IDD = IDD_DIRCOLS };
	CListCtrl m_listColumns;
	bool m_bReset;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirColsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation methods
protected:
	void InitList();
	void LoadLists();
	void SelectItem(int index);
	void LoadDefLists();
	void SortArrayToLogicalOrder();
	void MoveItem(int index, int newIndex);
	void MoveSelectedItems(bool bUp);
	void SanitizeOrder();

// Implementation data
private:
	ColumnArray m_cols; /**< Column list. */
	ColumnArray m_defCols; /**< Default columns. */
	static bool CompareColumnsByLogicalOrder( const column & el1, const column & el2 );

	// Generated message map functions
	//{{AFX_MSG(CDirColsDlg)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnUp();
	afx_msg void OnDown();
	virtual void OnOK() override;
	afx_msg void OnDefaults();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLvnItemchangedColdlgList(NMHDR *pNMHDR, LRESULT *pResult);
};
