/** 
 * @file  PreferencesDlg.h
 *
 * @brief Declaration of CPreferencesDlg class
 *
 * @note This code originates from AbstractSpoon / TodoList
 * (http://www.abstractspoon.com/) but is modified to use in
 * WinMerge.
 */
#pragma once

#include "TrDialogs.h"
#include "PropGeneral.h"
#include "PropCompare.h"
#include "PropMessageBoxes.h"
#include "PropEditor.h"
#include "PropEditorSyntax.h"
#include "PropRegistry.h"
#include "PropColorSchemes.h"
#include "PropMergeColors.h"
#include "PropTextColors.h"
#include "PropSyntaxColors.h"
#include "PropMarkerColors.h"
#include "PropDirColors.h"
#include "PropCodepage.h"
#include "PropArchive.h"
#include "PropProject.h"
#include "PropBackups.h"
#include "PropShell.h"
#include "PropCompareFolder.h"
#include "PropCompareTable.h"
#include "PropCompareBinary.h"
#include "PropCompareImage.h"
#include "PropCompareWebPage.h"

#include "PropertyPageHost.h"
#include "CMoveConstraint.h"

class COptionsMgr;
class SyntaxColors;

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDlg dialog

class CPreferencesDlg : public CTrDialog
{
// Construction
public:
	//CPreferencesDlg(UINT nMenuID = 0, CWnd* pParent = nullptr);   // standard constructor
	CPreferencesDlg(COptionsMgr *optionsMgr, SyntaxColors *colors,
		UINT nMenuID = 0, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPreferencesDlg();

protected:
// Dialog Data
	//{{AFX_DATA(CPreferencesDlg)
	CTreeCtrl m_tcPages;
	//}}AFX_DATA

	CPropertyPageHost m_pphost;
	
	PropGeneral m_pageGeneral;
	PropCompare m_pageCompare;
	PropMessageBoxes m_pageMessageBoxes;
	PropEditor m_pageEditor;
	PropEditorSyntax m_pageEditorSyntax;
	PropRegistry m_pageSystem;
	PropCodepage m_pageCodepage;
	PropColorSchemes m_pageColorSchemes;
	PropMergeColors m_pageMergeColors;
	PropTextColors m_pageTextColors;
	PropSyntaxColors m_pageSyntaxColors;
	PropMarkerColors m_pageMarkerColors;
	PropDirColors m_pageDirColors;
	PropArchive m_pageArchive;
	PropProject m_pageProject;
	PropBackups m_pageBackups;
	PropShell m_pageShell;
	PropCompareFolder m_pageCompareFolder;
	PropCompareTable m_pageCompareTable;
	PropCompareBinary m_pageCompareBinary;
	PropCompareImage m_pageCompareImage;
	PropCompareWebPage m_pageCompareWebPage;

	CMapPtrToPtr m_mapPP2HTI;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesDlg)
	public:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual void OnOK();

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPreferencesDlg)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHelpButton();
	afx_msg void OnImportButton();
	afx_msg void OnExportButton();
	afx_msg LRESULT OnColorSchemeChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSelchangedPages(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void AddPage(CPropertyPage* pPage, UINT nResourceID);
	void AddPage(CPropertyPage* pPage, const tchar_t* szPath);
	void AddPage(CPropertyPage* pPage, UINT nTopHeading, UINT nSubHeading);
	void SetActivePage(int nPage);
	CString GetItemPath(HTREEITEM hti);
	void ReadOptions(bool bUpdate = false);
	void SaveOptions();
	void SafeUpdatePage(CPropertyPage* pPage, bool bSaveAndValidate);

private:
	COptionsMgr *m_pOptionsMgr;
	SyntaxColors *m_pSyntaxColors;
	prdlg::CMoveConstraint m_constraint; /**< Resizes dialog controls when dialog resized */
};
