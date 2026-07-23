// SPDX-License-Identifier: GPL-2.0-or-later
/**
 * @file  ArchiveDlg.h
 *
 * @brief Declaration of the file comparison archive dialog.
 */
#pragma once

#include "TrDialogs.h"
#include "SuperComboBox.h"
#include "WindowListCtrl.h"
#include "IMergeDoc.h"
#include "PathContext.h"

struct ArchiveItem
{
	IMergeDoc* document = nullptr;
	PathContext paths;
	String title;
	bool checked = false;
	int diffStatus = -1;
};

class ArchiveDlg : public CTrDialog
{
	DECLARE_DYNAMIC(ArchiveDlg)

public:
	struct Options
	{
		String archiveFile;
		bool includeReport = true;
		bool includePatch = true;
		bool copyToClipboard = false;
		std::vector<size_t> selectedItems;
	};

	explicit ArchiveDlg(CWnd* pParent = nullptr);

	enum { IDD = IDD_ARCHIVE };

	void SetItems(const std::vector<ArchiveItem>& items) { m_items = items; }
	const Options& GetOptions() const { return m_options; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;
	virtual void OnOK() override;

	afx_msg void OnBrowseArchive();
	DECLARE_MESSAGE_MAP()

private:
	void CollectOptions();

	std::vector<ArchiveItem> m_items;
	Options m_options;

	CWindowListCtrl m_list;
	CSuperComboBox m_ctlArchiveFile;
};
