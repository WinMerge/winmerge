/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  PropGeneral.h
 *
 * @brief Implementation file for PropGeneral propertyheet
 *
 */

#include "stdafx.h"
#include "PropGeneral.h"
#include "Merge.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"
#include "LanguageSelect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/** 
 * @brief Constructor initialising members.
 */
PropGeneral::PropGeneral(COptionsMgr *optionsMgr) 
	: OptionsPanel(optionsMgr, PropGeneral::IDD)
	, m_bScroll(false)
	, m_bScrollToFirstInlineDiff(false)
	, m_nSingleInstance(0)
	, m_bVerifyPaths(false)
	, m_nCloseWindowWithEsc(1)
	, m_bAskMultiWindowClose(false)
	, m_nAutoCompleteSource(0)
	, m_bPreserveFiletime(false)
	, m_bShowSelectFolderOnStartup(false)
	, m_bCloseWithOK(true)
	, m_nFileReloadMode(0)
{
}

PropGeneral::~PropGeneral()
{
}

BOOL PropGeneral::OnInitDialog()
{
	OptionsPanel::OnInitDialog();

	CComboBox *pWnd = (CComboBox*)GetDlgItem(IDC_AUTO_COMPLETE_SOURCE);
	ASSERT(pWnd != nullptr);

	for (const auto& item : { _("Disabled"), _("From file system"), _("From Most Recently Used list") })
		pWnd->AddString(item.c_str());

	pWnd->SetCurSel(m_nAutoCompleteSource);

	pWnd = (CComboBox*)GetDlgItem(IDC_ESC_CLOSES_WINDOW);
	ASSERT(pWnd != nullptr);

	for (const auto& item : { _("Disabled"), _("MDI child window or main window"), _("MDI child window only"), _("Close main window if there is only one MDI child window") })
		pWnd->AddString(item.c_str());

	pWnd->SetCurSel(m_nCloseWindowWithEsc);

	pWnd = (CComboBox*)GetDlgItem(IDC_SINGLE_INSTANCE);
	ASSERT(pWnd != nullptr);

	for (const auto& item : { _("Disabled"), _("Allow only one instance to run"), _("Allow only one instance to run and wait for the instance to terminate") })
		pWnd->AddString(item.c_str());

	pWnd->SetCurSel(m_nSingleInstance);

	pWnd = (CComboBox*)GetDlgItem(IDC_AUTO_RELOAD_MODIFIED_FILES);
	ASSERT(pWnd != nullptr);

	for (const auto& item : { _("Disabled"), _("Only on window activated"), _("Immediately") })
		pWnd->AddString(item.c_str());

	pWnd->SetCurSel(m_nFileReloadMode);

	m_ctlLangList.SetDroppedWidth(600);
	m_ctlLangList.EnableWindow(FALSE);
	m_asyncLanguagesLoader = Concurrent::CreateTask([hwnd = m_hWnd] {
			std::vector<std::pair<LANGID, String>> langs = theApp.m_pLangDlg->GetAvailableLanguages();
			::PostMessage(hwnd, WM_APP, 0, 0);
			return langs;
		});
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void PropGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropGeneral)
	DDX_Check(pDX, IDC_SCROLL_CHECK, m_bScroll);
	DDX_Check(pDX, IDC_SCROLL_TO_FIRST_INLINE_DIFF_CHECK, m_bScrollToFirstInlineDiff);
	DDX_CBIndex(pDX, IDC_SINGLE_INSTANCE, m_nSingleInstance);
	DDX_Check(pDX, IDC_VERIFY_OPEN_PATHS, m_bVerifyPaths);
	DDX_CBIndex(pDX, IDC_ESC_CLOSES_WINDOW, m_nCloseWindowWithEsc);
	DDX_Check(pDX, IDC_ASK_MULTIWINDOW_CLOSE, m_bAskMultiWindowClose);
	DDX_CBIndex(pDX, IDC_AUTO_COMPLETE_SOURCE, m_nAutoCompleteSource);
	DDX_Check(pDX, IDC_PRESERVE_FILETIME, m_bPreserveFiletime);
	DDX_Check(pDX, IDC_STARTUP_FOLDER_SELECT, m_bShowSelectFolderOnStartup);
	DDX_Check(pDX, IDC_CLOSE_WITH_OK, m_bCloseWithOK);
	DDX_CBIndex(pDX, IDC_AUTO_RELOAD_MODIFIED_FILES, m_nFileReloadMode);
	DDX_Control(pDX, IDC_LANGUAGE_LIST, m_ctlLangList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropGeneral, OptionsPanel)
	//{{AFX_MSG_MAP(PropGeneral)
	ON_MESSAGE(WM_APP, OnLoadLanguages)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 */
void PropGeneral::ReadOptions()
{
	m_bScroll = GetOptionsMgr()->GetBool(OPT_SCROLL_TO_FIRST);
	m_bScrollToFirstInlineDiff = GetOptionsMgr()->GetBool(OPT_SCROLL_TO_FIRST_INLINE_DIFF);
	m_nSingleInstance = GetOptionsMgr()->GetInt(OPT_SINGLE_INSTANCE);
	m_bVerifyPaths = GetOptionsMgr()->GetBool(OPT_VERIFY_OPEN_PATHS);
	m_nCloseWindowWithEsc = GetOptionsMgr()->GetInt(OPT_CLOSE_WITH_ESC);
	m_bAskMultiWindowClose = GetOptionsMgr()->GetBool(OPT_ASK_MULTIWINDOW_CLOSE);
	m_nAutoCompleteSource = GetOptionsMgr()->GetInt(OPT_AUTO_COMPLETE_SOURCE);
	m_bPreserveFiletime = GetOptionsMgr()->GetBool(OPT_PRESERVE_FILETIMES);
	m_bShowSelectFolderOnStartup = GetOptionsMgr()->GetBool(OPT_SHOW_SELECT_FILES_AT_STARTUP);
	m_bCloseWithOK = GetOptionsMgr()->GetBool(OPT_CLOSE_WITH_OK);
	m_nFileReloadMode = GetOptionsMgr()->GetInt(OPT_AUTO_RELOAD_MODIFIED_FILES);
}

/** 
 * @brief Writes options values from UI to storage.
 */
void PropGeneral::WriteOptions()
{
	GetOptionsMgr()->SaveOption(OPT_SCROLL_TO_FIRST, m_bScroll);
	GetOptionsMgr()->SaveOption(OPT_SCROLL_TO_FIRST_INLINE_DIFF, m_bScrollToFirstInlineDiff);
	GetOptionsMgr()->SaveOption(OPT_SINGLE_INSTANCE, m_nSingleInstance);
	GetOptionsMgr()->SaveOption(OPT_VERIFY_OPEN_PATHS, m_bVerifyPaths);
	GetOptionsMgr()->SaveOption(OPT_CLOSE_WITH_ESC, m_nCloseWindowWithEsc);
	GetOptionsMgr()->SaveOption(OPT_ASK_MULTIWINDOW_CLOSE, m_bAskMultiWindowClose);
	GetOptionsMgr()->SaveOption(OPT_AUTO_COMPLETE_SOURCE, m_nAutoCompleteSource);
	GetOptionsMgr()->SaveOption(OPT_PRESERVE_FILETIMES, m_bPreserveFiletime);
	GetOptionsMgr()->SaveOption(OPT_SHOW_SELECT_FILES_AT_STARTUP, m_bShowSelectFolderOnStartup);
	GetOptionsMgr()->SaveOption(OPT_CLOSE_WITH_OK, m_bCloseWithOK);
	GetOptionsMgr()->SaveOption(OPT_AUTO_RELOAD_MODIFIED_FILES, m_nFileReloadMode);
	int index = m_ctlLangList.GetCurSel();
	if (index >= 0)
	{
		WORD lang = (WORD)m_ctlLangList.GetItemData(index);
		GetOptionsMgr()->SaveOption(OPT_SELECTED_LANGUAGE, (int)lang);
	}
}

LRESULT PropGeneral::OnLoadLanguages(WPARAM, LPARAM)
{
	m_ctlLangList.SetRedraw(false);
	for (auto&& i : m_asyncLanguagesLoader.Get())
	{
		m_ctlLangList.AddString(i.second.c_str());
		m_ctlLangList.SetItemData(m_ctlLangList.GetCount() - 1, i.first);
		if (i.first == theApp.m_pLangDlg->GetLangId())
			m_ctlLangList.SetCurSel(m_ctlLangList.GetCount() - 1);
	}
	m_ctlLangList.EnableWindow(TRUE);
	m_ctlLangList.SetRedraw(true);
	return 0;
}
