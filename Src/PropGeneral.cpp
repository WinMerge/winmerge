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
	BindOption(OPT_SCROLL_TO_FIRST, m_bScroll, IDC_SCROLL_CHECK, DDX_Check);
	BindOption(OPT_SCROLL_TO_FIRST_INLINE_DIFF, m_bScrollToFirstInlineDiff, IDC_SCROLL_TO_FIRST_INLINE_DIFF_CHECK, DDX_Check);
	BindOption(OPT_SINGLE_INSTANCE, m_nSingleInstance, IDC_SINGLE_INSTANCE, DDX_CBIndex);
	BindOption(OPT_VERIFY_OPEN_PATHS, m_bVerifyPaths, IDC_VERIFY_OPEN_PATHS, DDX_Check);
	BindOption(OPT_CLOSE_WITH_ESC, m_nCloseWindowWithEsc, IDC_ESC_CLOSES_WINDOW, DDX_CBIndex);
	BindOption(OPT_ASK_MULTIWINDOW_CLOSE, m_bAskMultiWindowClose, IDC_ASK_MULTIWINDOW_CLOSE, DDX_Check);
	BindOption(OPT_AUTO_COMPLETE_SOURCE, m_nAutoCompleteSource, IDC_AUTO_COMPLETE_SOURCE, DDX_CBIndex);
	BindOption(OPT_PRESERVE_FILETIMES, m_bPreserveFiletime, IDC_PRESERVE_FILETIME, DDX_Check);
	BindOption(OPT_SHOW_SELECT_FILES_AT_STARTUP, m_bShowSelectFolderOnStartup, IDC_STARTUP_FOLDER_SELECT, DDX_Check);
	BindOption(OPT_CLOSE_WITH_OK, m_bCloseWithOK, IDC_CLOSE_WITH_OK, DDX_Check);
	BindOption(OPT_AUTO_RELOAD_MODIFIED_FILES, m_nFileReloadMode, IDC_AUTO_RELOAD_MODIFIED_FILES, DDX_CBIndex);
}

PropGeneral::~PropGeneral()
{
}

BOOL PropGeneral::OnInitDialog()
{
	SetDlgItemComboBoxList(IDC_AUTO_COMPLETE_SOURCE,
		{ I18n::tr("Options dialog|General|Open dialog Auto completion","Disabled"), _("From file system"), _("From Most Recently Used list") });
	SetDlgItemComboBoxList(IDC_ESC_CLOSES_WINDOW,
		{ I18n::tr("Options dialog|General|Close windows with Esc","Disabled"), _("MDI child window or main window"), _("MDI child window only"), _("Close main window if only one MDI child window") });
	SetDlgItemComboBoxList(IDC_SINGLE_INSTANCE,
		{ I18n::tr("Options dialog|General|Single instance mode","Disabled"), _("Allow only one instance to run"), _("Allow only one instance; wait for termination") });
	SetDlgItemComboBoxList(IDC_AUTO_RELOAD_MODIFIED_FILES,
		{ I18n::tr("Options dialog|General|Auto-reload modified files","Disabled"), _("Only on window activated"), _("Immediately") });

	OptionsPanel::OnInitDialog();

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
	DDX_Control(pDX, IDC_LANGUAGE_LIST, m_ctlLangList);
	//}}AFX_DATA_MAP
	DoDataExchangeBindOptions(pDX);
}


BEGIN_MESSAGE_MAP(PropGeneral, OptionsPanel)
	//{{AFX_MSG_MAP(PropGeneral)
	ON_BN_CLICKED(IDC_COMPARE_DEFAULTS, OnDefaults)
	ON_MESSAGE(WM_APP, OnLoadLanguages)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Writes options values from UI to storage.
 */
void PropGeneral::WriteOptions()
{
	WriteOptionBindings();

	int index = m_ctlLangList.GetCurSel();
	if (index >= 0)
	{
		WORD lang = (WORD)m_ctlLangList.GetItemData(index);
		GetOptionsMgr()->SaveOption(OPT_SELECTED_LANGUAGE, (int)lang);
	}
}

/**
 * @brief Sets options to defaults.
 */
void PropGeneral::OnDefaults()
{
	ResetOptionBindings();

	LANGID lang = static_cast<LANGID>(GetOptionsMgr()->GetDefault<unsigned>(OPT_SELECTED_LANGUAGE));
	SetCursorSelectForLanguage(lang);

	UpdateData(FALSE);
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

/**
 * @brief Select the item specified by the language ID in the "Language" combo box.
 * @param [in] lang The language ID of the selected item.
 */
void PropGeneral::SetCursorSelectForLanguage(LANGID lang)
{
	int itemCount = m_ctlLangList.GetCount();
	for (int i = 0; i < itemCount; i++)
	{
		if (m_ctlLangList.GetItemData(i) == lang)
		{
			m_ctlLangList.SetCurSel(i);
			break;
		}
	}
}
