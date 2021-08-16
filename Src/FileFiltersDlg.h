// SPDX-License-Identifier: GPL-2.0-or-later
/**
 * @file  FileFiltersDlg.h
 *
 * @brief Declaration file for File Filters dialog
 */
#pragma once

#include "TrDialogs.h"
#include <vector>

struct FileFilterInfo;

/**
 * @brief Class for dialog allowing user to select
 * and edit used file filters
 */
class FileFiltersDlg : public CTrPropertyPage
{
	DECLARE_DYNCREATE(FileFiltersDlg)

// Construction
public:
	FileFiltersDlg();   // standard constructor
	void SetFilterArray(const std::vector<FileFilterInfo>& fileFilters);
	String GetSelected();
	void SetSelected(const String & selected);

// Implementation data
private:
	String m_sFileFilterPath;
	CPoint m_ptLastMousePos;
	std::vector<FileFilterInfo> m_Filters;

// Dialog Data
	//{{AFX_DATA(FileFiltersDlg)
	enum { IDD = IDD_FILTERS_FILEFILTERS };
	CListCtrl m_listFilters;
	//}}AFX_DATA

// Implementation methods
protected:
	void InitList();
	void SelectFilterByIndex(int index);
	void AddToGrid(int filterIndex);
	bool IsFilterItemNone(int item) const;
	void UpdateFiltersList();
	void EditFileFilter(const String& path);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(FileFiltersDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(FileFiltersDlg)
	virtual BOOL OnInitDialog() override;
	virtual void OnOK();
	afx_msg void OnFiltersEditbtn();
	afx_msg void OnDblclkFiltersList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnItemchangedFilterfileList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnInfoTip(NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedFilterfileTestButton();
	afx_msg void OnBnClickedFilterfileNewbutton();
	afx_msg void OnBnClickedFilterfileDelete();
	afx_msg void OnHelp();
	afx_msg void OnBnClickedFilterfileInstall();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
