/** 
 * @file  DirColsDlg.h
 *
 * @brief Declaration file for CDirColsDlg
 *
 * @date  Created: 2003-08-19
 */
// ID line follows -- this is updated by SVN
// $Id$


#if !defined(AFX_DIRCOLSDLG_H__2FCB576C_C609_4623_8C55_F3870F22CA0B__INCLUDED_)
#define AFX_DIRCOLSDLG_H__2FCB576C_C609_4623_8C55_F3870F22CA0B__INCLUDED_
#pragma once


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
class CDirColsDlg : public CDialog
{
// Public types
public:
	/** @brief One column's information. */
	struct column
	{
		CString name; /**< Column name */
		CString desc; /**< Description for column */
		int log_col; /**< Logical (shown) order number */
		int phy_col; /**< Physical (in memory) order number */
		column() : log_col(-1), phy_col(-1) { } /**< default constructor for use in CArray */
		column(LPCTSTR sz, LPCTSTR dsc, int log, int phy) : name(sz), desc(dsc), log_col(log), phy_col(phy) { } 
	};
	typedef CArray<column, column> ColumnArray;

// Construction
public:
	CDirColsDlg(CWnd* pParent = NULL);   // standard constructor
	void AddColumn(LPCTSTR name, LPCTSTR desc, int log, int phy=-1)
		{ column c(name, desc, log, phy); m_cols.Add(c); }
	void AddDefColumn(LPCTSTR name, int log, int phy=-1)
		{ column c(name, _T(""), log, phy); m_defCols.Add(c); }
	const ColumnArray & GetColumns() const { return m_cols; }

// Dialog Data
	//{{AFX_DATA(CDirColsDlg)
	enum { IDD = IDD_DIRCOLS };
	CListCtrl m_listColumns;
	BOOL m_bReset;
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
	void LoadDefLists();
	void SortArrayToLogicalOrder();
	void MoveItem(int index, int newIndex);
	void MoveSelectedItems(BOOL bUp);
	void SanitizeOrder();
	static int cmpcols(const void * el1, const void * el2);

// Implementation data
private:
	ColumnArray m_cols; /**< Column list. */
	ColumnArray m_defCols; /**< Default columns. */

	// Generated message map functions
	//{{AFX_MSG(CDirColsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnUp();
	afx_msg void OnDown();
	virtual void OnOK();
	afx_msg void OnDefaults();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLvnItemchangedColdlgList(NMHDR *pNMHDR, LRESULT *pResult);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRCOLSDLG_H__2FCB576C_C609_4623_8C55_F3870F22CA0B__INCLUDED_)
