// SPDX-License-Identifier: GPL-2.0-or-later
/**
 * @file  ArchiveTool.cpp
 *
 * @brief archive creation for file comparison documents.
 */

#include "StdAfx.h"
#include "ArchiveTool.h"
#include "FileCmpReport.h"
#include "MainFrm.h"
#include "MergeDoc.h"
#include "PatchTool.h"
#include "TempFile.h"
#include "7zCommon.h"
#include "paths.h"
#include "Clipboard.h"

namespace
{
String GetArchiveFileName(const String& path, const String& prefix, size_t index)
{
	String folder;
	String name;
	String extension;
	paths::SplitFilename(path, &folder, &name, &extension);
	if (name.empty())
		name = strutils::format(_T("comparison_%d"), static_cast<int>(index + 1));
	return strutils::format(_T("%s%d_%s%s"), prefix.c_str(), static_cast<int>(index + 1),
		name.c_str(), extension.c_str());
}

bool AddTemporaryFile(std::vector<CompressibleItem>& archiveItems, const String& path, const String& name)
{
	if (paths::DoesPathExist(path) != paths::IS_EXISTING_FILE)
		return false;
	CompressibleItem item;
	item.name = name;
	item.fullPath = path;
	item.recurse = false;
	archiveItems.push_back(std::move(item));
	return true;
}

/*
enum class ArchiveRole { Left, Middle, Right };
enum class ArchiveType { File, Report, Patch, Project, Manifest };

struct ArchiveItem
{
	int compareIndex;
	String leftPath;
	String middlePath;
	String rightPath;
};

struct LayoutItem
{
	int compareIndex;
	ArchiveRole role;
	String sourcePath;
	// Relative path inside original/ or altered/
	String relativePath;
	// Final path inside ZIP
	String archivePath;
};

struct LayoutGroup
{
	String name;
	std::vector<LayoutItem> items;
};

struct ArchiveEntry
{
	ArchiveType type;
	int compareIndex;
	ArchiveRole role;
	String sourcePath;
	String archivePath;
};

std::vector<LayoutGroup>
CreateLayoutGroups(const std::vector<ArchiveItem>& archiveItems)
{
	LayoutGroup group;
	group.name = _T("");

	for (const auto& item : archiveItems)
	{
		if (!item.leftPath.empty())
		{
			group.items.push_back(
				{
					item.compareIndex,
					ArchiveRole::Left,
					item.leftPath
				});
		}

		if (!item.middlePath.empty())
		{
			group.items.push_back(
				{
					item.compareIndex,
					ArchiveRole::Middle,
					item.middlePath
				});
		}

		if (!item.rightPath.empty())
		{
			group.items.push_back(
				{
					item.compareIndex,
					ArchiveRole::Right,
					item.rightPath
				});
		}
	}

	return { std::move(group) };
}
std::vector<ArchiveEntry> LayoutArchive(const std::vector<ArchiveItem>& items, const ArchiveOptions& options)
{
	auto groups = CreateLayoutGroups(items);

	BuildRelativePaths(groups);

	ResolveConflicts(groups);

	auto entries = BuildArchiveEntries(groups);

	AddGeneratedEntries(entries, options);

	return entries;
}

*/
}

void ArchiveTool::AddDocument(IMergeDoc* document, bool checked, int diffStatus)
{
	if (document == nullptr)
		return;

	ArchiveItem item;
	item.document = document;
	const int fileCount = document->GetFileCount();
	for (int i = 0; i < fileCount; ++i)
		item.paths.SetPath(i, document->GetPath(i));
	item.title = document->GetDescription(0);
	if (item.title.empty())
		item.title = item.paths.GetLeft();
	item.checked = checked;
	item.diffStatus = diffStatus;
	m_items.push_back(std::move(item));
}

bool ArchiveTool::CreateArchive()
{
	if (!HasZipSupport())
	{
		I18n::MessageBox(IDS_NO_ZIP_SUPPORT, MB_ICONINFORMATION);
		return false;
	}

	ArchiveDlg dialog;
	dialog.SetItems(m_items);
	if (dialog.DoModal() != IDOK)
		return false;

	const ArchiveDlg::Options& options = dialog.GetOptions();
	String archivePath = options.archiveFile;
	TempFilePtr temporaryArchive;
	if (archivePath.empty())
	{
		temporaryArchive = std::make_shared<TempFile>();
		temporaryArchive->Create(_T("WM"), _T(".zip"));
		archivePath = temporaryArchive->GetPath();
	}

	std::vector<ArchiveItem> selectedItems;
	for (const size_t index : options.selectedItems)
	{
		if (index < m_items.size())
			selectedItems.push_back(m_items[index]);
	}
	if (selectedItems.empty())
		return false;

	std::vector<CompressibleItem> archiveItems;
	CPatchTool patchTool;
	for (size_t i = 0; i < selectedItems.size(); ++i)
	{
		const ArchiveItem& item = selectedItems[i];
		std::vector<String> roots;
		if (item.paths.GetSize() == 2)
		{
			roots.push_back(_T("original\\"));
			roots.push_back(_T("altered\\"));
		}
		else
		{
			roots.push_back(_T("1\\"));
			roots.push_back(_T("2\\"));
			roots.push_back(_T("3\\"));
		}
		for (int j = 0; j < item.paths.GetSize(); ++j)
		{
			const String& root = roots[j];
			const String& path = item.paths[j];
			if (!path.empty() && !paths::IsURL(path))
			{
				if (paths::DoesPathExist(path) == paths::IS_EXISTING_FILE)
				{
					CompressibleItem archiveItem;
					archiveItem.name = GetArchiveFileName(path, root, i);
					archiveItem.fullPath = path;
					archiveItems.push_back(std::move(archiveItem));
				}
			}
		}

		if (item.document->GetDocumentType() == IMergeDoc::DocumentType::Text ||
			item.document->GetDocumentType() == IMergeDoc::DocumentType::Table)
		{
			if (auto* document = dynamic_cast<CMergeDoc*>(item.document))
				patchTool.AddFiles(item.paths.GetLeft(), _T(""), item.paths.GetRight(), _T(""),
					item.title, true, item.diffStatus);
		}
	}

	std::vector<IMergeDoc*> documents;
	for (const auto& item : selectedItems)
	{
		if (item.document != nullptr)
			documents.push_back(item.document);
	}

	TempFile reportFile;
	TempFile patchFile;
	String error;
	if (options.includeReport)
	{
		reportFile.Create(_T("WM"), _T(".html"));
		if (!CFileCmpReport::GenerateDocumentReport(documents, reportFile.GetPath(), m_reportOptions, error))
		{
			AfxMessageBox(error.c_str(), MB_OK | MB_ICONSTOP);
			return false;
		}
		AddTemporaryFile(archiveItems, reportFile.GetPath(), _T("report.html"));
	}
	if (options.includePatch)
	{
		patchFile.Create(_T("WM"), _T(".diff"));
		if (!patchTool.CreatePatchFile(patchFile.GetPath()))
		{
			AfxMessageBox(_("Could not create the patch file.").c_str(), MB_OK | MB_ICONSTOP);
			return false;
		}
		AddTemporaryFile(archiveItems, patchFile.GetPath(), _T("patch.diff"));
	}

	if (archiveItems.empty())
	{
		AfxMessageBox(_("No files are available for packaging.").c_str(), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	if (!CompressibleItemEnumerator(std::move(archiveItems)).CompressArchive(archivePath.c_str()))
	{
		AfxMessageBox(_("Could not create the archive.").c_str(), MB_OK | MB_ICONSTOP);
		return false;
	}
	if (paths::DoesPathExist(archivePath) != paths::IS_EXISTING_FILE)
	{
		AfxMessageBox(_("Could not create the archive.").c_str(), MB_OK | MB_ICONSTOP);
		return false;
	}

	if (temporaryArchive != nullptr)
		GetMainFrame()->AddTempFile(temporaryArchive);

	if (options.copyToClipboard)
	{
		ClipboardUtils::PutFiles(std::vector<String>{ archivePath }, AfxGetMainWnd()->m_hWnd);
	}

	AfxMessageBox((_("Archive written.") + _T("\n") + archivePath).c_str(),
		MB_OK | MB_ICONINFORMATION);
	return true;
}
