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


/////////////////////////////////////////////////////////////////////////////
// CDirColsDlg dialog

/**
 * @brief Dialog to choose & order columns to be shown in dirview of differing files
 */
class CDirColsDlg : public CDialog
{
// Public types
public:
	struct column {
		CString name;
		int log_col;
		int phy_col;
		column() : log_col(-1), phy_col(-1) { } /**< default constructor for use in CArray */
		column(LPCTSTR sz, int log, int phy) : name(sz), log_col(log), phy_col(phy) { } 
	};
	typedef CArray<column, column> ColumnArray;

// Construction
public:
	CDirColsDlg(CWnd* pParent = NULL);   // standard constructor
	void AddColumn(CString name, int log, int phy=-1)
		{ column c(name, log, phy); m_cols.Add(c); }
	void AddDefColumn(CString name, int log, int phy=-1)
		{ column c(name, log, phy); m_defCols.Add(c); }
	const ColumnArray & GetColumns() const { return m_cols; }

// Dialog Data
	//{{AFX_DATA(CDirColsDlg)
	enum { IDD = IDD_DIRCOLS };
	CListBox	m_list_show;
	CListBox	m_list_hide;
	BOOL		m_bReset;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirColsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation methods
protected:
	void LoadLists();
	void LoadDefLists();
	void MoveItems(CListBox * list1, CListBox * list2, bool top);
	void UpdateEnables();
	void SortArrayToLogicalOrder();
	static int cmpcols(const void * el1, const void * el2);

// Implementation data
private:
	ColumnArray m_cols;
	ColumnArray m_defCols;

	// Generated message map functions
	//{{AFX_MSG(CDirColsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnUp();
	afx_msg void OnDown();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	virtual void OnOK();
	afx_msg void OnDefaults();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRCOLSDLG_H__2FCB576C_C609_4623_8C55_F3870F22CA0B__INCLUDED_)
