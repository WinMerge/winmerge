// SPDX-License-Identifier: GPL-2.0-or-later
/**
 * @file  ArchiveDlg.cpp
 *
 * @brief Implementation of the file comparison archive dialog.
 */

#include "StdAfx.h"
#include "ArchiveDlg.h"
#include "FileOrFolderSelect.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "DiffImageListUtils.h"
#include "resource.h"

IMPLEMENT_DYNAMIC(ArchiveDlg, CTrDialog)

ArchiveDlg::ArchiveDlg(CWnd* pParent)
	: CTrDialog(ArchiveDlg::IDD, pParent)
{
}

void ArchiveDlg::DoDataExchange(CDataExchange* pDX)
{
	CTrDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ARCHIVE_LIST, m_list);
	DDX_Control(pDX, IDC_ARCHIVE_FILE, m_ctlArchiveFile);
	DDX_Check(pDX, IDC_ARCHIVE_REPORT, m_options.includeReport);
	DDX_Check(pDX, IDC_ARCHIVE_PATCH, m_options.includePatch);
	DDX_Check(pDX, IDC_ARCHIVE_COPYCLIPBOARD, m_options.copyToClipboard);
	DDX_CBString(pDX, IDC_ARCHIVE_FILE, m_options.archiveFile);
}

BEGIN_MESSAGE_MAP(ArchiveDlg, CTrDialog)
	ON_BN_CLICKED(IDC_ARCHIVE_BROWSE, OnBrowseArchive)
END_MESSAGE_MAP()

BOOL ArchiveDlg::OnInitDialog()
{
	__super::OnInitDialog();

	m_options.includeReport = GetOptionsMgr()->GetBool(OPT_ARCHIVE_INCLUDEREPORT);
	m_options.includePatch = GetOptionsMgr()->GetBool(OPT_ARCHIVE_INCLUDEPATCH);
	m_options.copyToClipboard = GetOptionsMgr()->GetBool(OPT_ARCHIVE_COPYTOCLIPBOARD);
	m_ctlArchiveFile.SetFileControlStates(true);
	m_ctlArchiveFile.LoadState(_T("Archives"));

	m_list.Initialize();
	DiffImageListUtils::InitializeDiffImageList(m_list.GetImageList());
	m_list.SetImageList(&m_list.GetImageList(), LVSIL_SMALL);
	std::vector<CWindowListCtrl::Item> listItems;
	listItems.reserve(m_items.size());
	for (size_t i = 0; i < m_items.size(); ++i)
	{
		CWindowListCtrl::Item item;
		item.title = m_items[i].title;
		item.data = static_cast<uintptr_t>(i);
		item.checked = m_items[i].checked;
		item.iImage = m_items[i].diffStatus;
		listItems.push_back(item);
	}
	m_list.SetItems(listItems);

	CString cstrArchiveFile;
	m_ctlArchiveFile.GetWindowText(cstrArchiveFile);
	m_options.archiveFile = cstrArchiveFile;

	UpdateData(FALSE);
	return TRUE;
}

void ArchiveDlg::CollectOptions()
{
	UpdateData(TRUE);
	m_options.selectedItems.clear();
	for (const auto data : m_list.GetCheckedData())
		m_options.selectedItems.push_back(static_cast<size_t>(data));
}

void ArchiveDlg::OnBrowseArchive()
{
	String chosenPath;
	String initialPath = m_options.archiveFile;
	if (SelectFile(GetSafeHwnd(), chosenPath, false, initialPath.c_str(), _T(""),
		I18n::LoadString(IDS_ARCHIVEFILES), _T("zip")))
	{
		m_options.archiveFile = chosenPath;
		UpdateData(FALSE);
	}
}

void ArchiveDlg::OnOK()
{
	CollectOptions();
	m_ctlArchiveFile.SaveState(_T("Archives"));
	GetOptionsMgr()->SaveOption(OPT_ARCHIVE_INCLUDEREPORT, m_options.includeReport);
	GetOptionsMgr()->SaveOption(OPT_ARCHIVE_INCLUDEPATCH, m_options.includePatch);
	GetOptionsMgr()->SaveOption(OPT_ARCHIVE_COPYTOCLIPBOARD, m_options.copyToClipboard);
	__super::OnOK();
}
