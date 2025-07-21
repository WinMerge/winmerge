// SPDX-License-Identifier: GPL-2.0-or-later
/**
 * @file  FileFiltersDlg.h
 *
 * @brief Declaration file for File Filters dialog
 */
#pragma once

#include "TrDialogs.h"
#include "FileFilterHelper.h"
#include "SuperComboBox.h"
#include "ValidatingEdit.h"
#include "FileFilterHelperMenu.h"
#include <vector>
#include <memory>

class DirWatcher;

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
	void SetFileFilterHelper(FileFilterHelper* pFileFilterHelper);

// Implementation data
private:
	CPoint m_ptLastMousePos;
	String m_sMask;
	std::unique_ptr<FileFilterHelper> m_pFileFilterHelper;
	FileFilterHelper* m_pFileFilterHelperOrg;
	std::vector<FileFilterInfo> m_Filters;
	CSuperComboBox m_ctlMask;
	CValidatingEdit m_ctlMaskEdit;
	CFileFilterHelperMenu m_menu;
	std::unique_ptr<DirWatcher> m_pDirWatcher;

// Dialog Data
	//{{AFX_DATA(FileFiltersDlg)
	enum { IDD = IDD_FILTERS_FILEFILTERS };
	CListCtrl m_listFilters;
	//}}AFX_DATA

// Implementation methods
protected:
	void InitList();
	void SelectFilterByIndex(int index);
	void SelectFilterByFilePath(const String& path);
	void AddToGrid(int filterIndex);
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
	afx_msg void OnEndEditFilterfileMask(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFilterfileMaskMenu();
	afx_msg void OnEditChangeFilterfileMask();
	afx_msg void OnFiltersEditbtn();
	afx_msg void OnDblclkFiltersList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomDrawFiltersList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnItemchangedFilterfileList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnInfoTip(NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedFilterfileTestButton();
	afx_msg void OnBnClickedFilterfileNewbutton();
	afx_msg void OnBnClickedFilterfileDelete();
	afx_msg void OnHelp();
	afx_msg void OnBnClickedFilterfileInstall();
	afx_msg LRESULT OnFileFilterUpdated(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void SetButtonState();
};
