// SPDX-License-Identifier: GPL-2.0-or-later
/**
 * @file  ArchiveTool.h
 *
 * @brief archive creation for file comparison documents.
 */
#pragma once

#include "ArchiveDlg.h"
#include "FileCmpReport.h"

class ArchiveTool
{
public:
	void AddDocument(IMergeDoc* document, bool checked, int diffStatus);
	void SetReportOptions(const CFileCmpReport::Options& options) { m_reportOptions = options; }
	bool CreateArchive();

private:
	std::vector<ArchiveItem> m_items;
	CFileCmpReport::Options m_reportOptions;
};
