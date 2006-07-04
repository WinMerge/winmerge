/** 
 * @file  DirColsDlg.h
 *
 * @brief Declaration file for CDirColsDlg
 *
 * @date  Created: 2003-08-19
 */
// RCS ID line follows -- this is updated by CVS
// $Id$


#if !defined(AFX_DIRCOLSDLG_H__2FCB576C_C609_4623_8C55_F3870F22CA0B__INCLUDED_)
#define AFX_DIRCOLSDLG_H__2FCB576C_C609_4623_8C55_F3870F22CA0B__INCLUDED_
#pragma once

#include "HScrollListBox.h"

/////////////////////////////////////////////////////////////////////////////
// CDirColsDlg dialog

/**
 * @brief Dialog to choose & order columns to be shown in dirview of differing files
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
	void AddColumn(CString name, CString desc, int log, int phy=-1)
		{ column c(name, desc, log, phy); m_cols.Add(c); }
	void AddDefColumn(CString name, int log, int phy=-1)
		{ column c(name, _T(""), log, phy); m_defCols.Add(c); }
	const ColumnArray & GetColumns() const { return m_cols; }

// Dialog Data
	//{{AFX_DATA(CDirColsDlg)
	enum { IDD = IDD_DIRCOLS };
	CHScrollListBox m_list_show;
	CHScrollListBox m_list_hide;
	BOOL		m_bReset;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirColsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation methods
protected:
	void LoadLists();
	void LoadDefLists();
	void MoveItems(CHScrollListBox * list1, CHScrollListBox * list2, bool top);
	void UpdateEnables();
	void SortArrayToLogicalOrder();
	static int cmpcols(const void * el1, const void * el2);

// Implementation data
private:
	ColumnArray m_cols;
	ColumnArray m_defCols;
	BOOL m_bFromKeyboard; /**< Is up/down movement originating from keyboard? */

	// Generated message map functions
	//{{AFX_MSG(CDirColsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnUp();
	afx_msg void OnDown();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	virtual void OnOK();
	afx_msg void OnDefaults();
	afx_msg void OnLbnSelchangeListShow();
	afx_msg void OnLbnSelchangeListHide();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLbnDblclkListShow();
	afx_msg void OnLbnDblclkListHide();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRCOLSDLG_H__2FCB576C_C609_4623_8C55_F3870F22CA0B__INCLUDED_)
