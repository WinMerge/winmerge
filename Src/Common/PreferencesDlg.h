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
#include "PropEditor.h"
#include "PropRegistry.h"
#include "PropColors.h"
#include "PropTextColors.h"
#include "PropSyntaxColors.h"
#include "PropMarkerColors.h"
#include "PropDirColors.h"
#include "PropCodepage.h"
#include "PropArchive.h"
#include "PropBackups.h"
#include "PropShell.h"
#include "PropCompareFolder.h"
#include "PropCompareBinary.h"
#include "PropCompareImage.h"

#include "PropertyPageHost.h"

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

	void SetSyntaxColors(SyntaxColors *pColors);

protected:
// Dialog Data
	//{{AFX_DATA(CPreferencesDlg)
	CTreeCtrl m_tcPages;
	//}}AFX_DATA

	CPropertyPageHost m_pphost;
	
	PropGeneral m_pageGeneral;
	PropCompare m_pageCompare;
	PropEditor m_pageEditor;
	PropRegistry m_pageSystem;
	PropCodepage m_pageCodepage;
	PropMergeColors m_pageMergeColors;
	PropTextColors m_pageTextColors;
	PropSyntaxColors m_pageSyntaxColors;
	PropMarkerColors m_pageMarkerColors;
	PropDirColors m_pageDirColors;
	PropArchive m_pageArchive;
	PropBackups m_pageBackups;
	PropShell m_pageShell;
	PropCompareFolder m_pageCompareFolder;
	PropCompareBinary m_pageCompareBinary;
	PropCompareImage m_pageCompareImage;

	SyntaxColors *m_pSyntaxColors;

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
	afx_msg void OnDestroy();
	afx_msg void OnHelpButton();
	afx_msg void OnImportButton();
	afx_msg void OnExportButton();
	afx_msg void OnSelchangedPages(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void AddPage(CPropertyPage* pPage, UINT nResourceID);
	void AddPage(CPropertyPage* pPage, LPCTSTR szPath);
	void AddPage(CPropertyPage* pPage, UINT nTopHeading, UINT nSubHeading);
	void SetActivePage(int nPage);
	CString GetItemPath(HTREEITEM hti);
	void ReadOptions(bool bUpdate = false);
	void SaveOptions();
	void SafeUpdatePage(CPropertyPage* pPage, bool bSaveAndValidate);

private:
	COptionsMgr *m_pOptionsMgr;

};
