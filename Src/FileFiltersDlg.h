/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
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
	enum { IDD = IDD_FILEFILTERS };
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
