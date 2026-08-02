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
#include "Merge.h"
#include "MergeDoc.h"
#include "MergeTextFormatter.h"
#include "PatchTool.h"
#include "TempFile.h"
#include "7zCommon.h"
#include "paths.h"
#include "Clipboard.h"
#include "ProjectFile.h"
#include "FileTransform.h"
#include "OptionsProject.h"
#include "FileFilterHelper.h"

namespace
{

// Returns the longest common directory of the given paths.
String GetCommonRoot(const std::vector<String>& paths)
{
	if (paths.empty())
		return _T("");

	// Split the first path into components.
	std::vector<StringView> common = strutils::split(paths.front(), _T('\\'));
	// Remove the filename.
	if (!common.empty())
		common.pop_back();

	for (size_t i = 1; i < paths.size() && !common.empty(); ++i)
	{
		std::vector<StringView> parts = strutils::split(paths[i], _T('\\'));
		// Remove the filename.
		if (!parts.empty())
			parts.pop_back();

		size_t n = std::min(common.size(), parts.size());

		size_t j = 0;
		while (j < n && strutils::compare_nocase(
			String(common[j].data(), common[j].length()),
			String(parts[j].data(), parts[j].length())) == 0)
			++j;

		common.resize(j);
	}

	return strutils::join(common.begin(), common.end(), _T("\\"));
}

// Returns the longest common directory for each side.
// The returned vector has the same size as PathContext.
std::vector<String> GetCommonRoots(const std::vector<ArchiveItem>& selectedItems)
{
	std::vector<String> commonRoots;

	if (selectedItems.empty())
		return commonRoots;

	size_t sideCount = 0;
	for (const auto& item : selectedItems)
		sideCount = std::max(sideCount, static_cast<size_t>(item.paths.GetSize()));

	commonRoots.resize(sideCount);

	for (int side = 0; side < sideCount; ++side)
	{
		std::vector<String> paths;

		// Collect all existing paths for this side.
		for (const auto& item : selectedItems)
		{
			if (side >= item.paths.GetSize())
				continue;

			if (!item.paths[side].empty())
				paths.push_back(item.paths[side]);
		}

		if (paths.empty())
			continue;

		commonRoots[side] = GetCommonRoot(paths);
	}

	return commonRoots;
}

String GetArchiveFileName(const String& path, const String& prefix, const String& commonRoot)
{
	String relativePath = path.substr(commonRoot.length());
	strutils::replace(relativePath, _T(":"), _T(""));
	strutils::replace(relativePath, _T("\\\\"), _T("\\"));
	return paths::ConcatPath(prefix, relativePath);
}

void AddArchiveItem(std::vector<CompressibleItem>& archiveItems, const String& path, const String& name, bool recurse = false)
{
	CompressibleItem item;
	item.name = name;
	item.fullPath = path;
	item.recurse = recurse;
	archiveItems.push_back(std::move(item));
}

void SetProjectFileItem(ProjectFileItem& projItem, const PathContext& paths, const IMergeDoc* pDoc)
{
	bool m_nIgnoreWhite = GetOptionsMgr()->GetInt(OPT_CMP_IGNORE_WHITESPACE);
	bool m_bIgnoreBlankLines = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_BLANKLINES);
	bool m_bIgnoreCase = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_CASE);
	bool m_bIgnoreEol = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_EOL);
	bool m_bIgnoreNumbers = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_NUMBERS);
	bool m_bIgnoreCodepage = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_CODEPAGE);
	bool m_bFilterCommentsLines = GetOptionsMgr()->GetBool(OPT_CMP_FILTER_COMMENTLINES);
	bool m_bIgnoreMissingTrailingEol = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_MISSING_TRAILING_EOL);
	bool m_bIgnoreLineBreaks = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_LINE_BREAKS);
	bool m_nCompareMethod = GetOptionsMgr()->GetInt(OPT_CMP_METHOD);
	std::vector<String> m_hiddenItems;
	bool m_bRecurse = false;
	String m_strExt;
	bool m_bReadOnly[3] = { false, false, false };
	String m_strDesc[3];
	String m_strPath[3];
	for (int i = 0; i < paths.GetSize(); ++i)
	{
		m_strPath[i] = paths[i];
		m_bReadOnly[i] = pDoc->GetReadOnly(i);
		m_strDesc[i] = pDoc->GetDescription(i);
	}
	String m_strUnpackerPipeline;
	String m_strPredifferPipeline;
	if (pDoc->GetUnpacker())
		m_strUnpackerPipeline = pDoc->GetUnpacker()->GetPluginPipeline();
	if (pDoc->GetPrediffer())
		m_strPredifferPipeline = pDoc->GetPrediffer()->GetPluginPipeline();
	int m_nWindowType = pDoc->GetDocumentType();
	CMergeDoc* pMergeDoc = dynamic_cast<CMergeDoc*>(const_cast<IMergeDoc*>(pDoc));
	tchar_t m_cTableDelimiter = pMergeDoc ? pMergeDoc->m_ptBuf[0]->GetFieldDelimiter() : 0;
	tchar_t m_cTableQuote = pMergeDoc ? pMergeDoc->m_ptBuf[0]->GetFieldEnclosure() : 0;
	bool m_bTableAllowNewLinesInQuotes = pMergeDoc ? pMergeDoc->m_ptBuf[0]->GetAllowNewlinesInQuotes() : false;

	// from COpenView::OnSaveProject
	bool bSaveFileFilter = false;
	bool bSaveIncludeSubfolders = false;
	bool bSavePlugin = Options::Project::Get(GetOptionsMgr(), Options::Project::Operation::Save, Options::Project::Item::Plugin);
	bool bSaveCompareOptions = Options::Project::Get(GetOptionsMgr(), Options::Project::Operation::Save, Options::Project::Item::CompareOptions);
	bool bSaveHiddenItems = false;
	bool bSaveCompareMethod = false;

	projItem.SetSaveFilter(bSaveFileFilter);
	projItem.SetSaveSubfolders(bSaveIncludeSubfolders);
	projItem.SetSaveUnpacker(bSavePlugin);
	projItem.SetSavePrediffer(bSavePlugin);
	projItem.SetSaveIgnoreWhite(bSaveCompareOptions);
	projItem.SetSaveIgnoreBlankLines(bSaveCompareOptions);
	projItem.SetSaveIgnoreCase(bSaveCompareOptions);
	projItem.SetSaveIgnoreEol(bSaveCompareOptions);
	projItem.SetSaveIgnoreNumbers(bSaveCompareOptions);
	projItem.SetSaveIgnoreCodepage(bSaveCompareOptions);
	projItem.SetSaveIgnoreMissingTrailingEol(bSaveCompareOptions);
	projItem.SetSaveIgnoreBreakLines(bSaveCompareOptions);
	projItem.SetSaveFilterCommentsLines(bSaveCompareOptions);
	projItem.SetSaveCompareMethod(bSaveCompareOptions);
	projItem.SetSaveHiddenItems(bSaveHiddenItems);
	projItem.SetSaveCompareMethod(bSaveCompareMethod);

	if (!m_strPath[0].empty())
		projItem.SetLeft(m_strPath[0], &m_bReadOnly[0]);
	if (!m_strDesc[0].empty())
		projItem.SetLeftDesc(m_strDesc[0]);
	if (m_strPath[2].empty())
	{
		if (!m_strPath[1].empty())
			projItem.SetRight(m_strPath[1], &m_bReadOnly[1]);
		if (!m_strDesc[1].empty())
			projItem.SetRightDesc(m_strDesc[1]);
	}
	else
	{
		if (!m_strPath[1].empty())
			projItem.SetMiddle(m_strPath[1], &m_bReadOnly[1]);
		if (!m_strPath[2].empty())
			projItem.SetRight(m_strPath[2], &m_bReadOnly[2]);
		if (!m_strDesc[1].empty())
			projItem.SetMiddleDesc(m_strDesc[1]);
		if (!m_strDesc[2].empty())
			projItem.SetRightDesc(m_strDesc[2]);
	}
	if (bSaveFileFilter && !m_strExt.empty())
	{
		// Remove possbile prefix from the filter name
		String prefix = _T("[F] ");
		String strExt = m_strExt;
		size_t ind = strExt.find(prefix, 0);
		if (ind == 0)
		{
			strExt.erase(0, prefix.length());
		}
		strExt = strutils::trim_ws_begin(strExt);
		projItem.SetFilter(strExt);
	}
	if (bSaveIncludeSubfolders)
		projItem.SetSubfolders(m_bRecurse);
	if (bSavePlugin)
	{
		if (!m_strUnpackerPipeline.empty())
			projItem.SetUnpacker(m_strUnpackerPipeline);
		if (!m_strPredifferPipeline.empty())
			projItem.SetPrediffer(m_strPredifferPipeline);
	}
	if (m_nWindowType != -1)
		projItem.SetWindowType(m_nWindowType);
	if (m_nWindowType == 2 /* table */)
	{
		projItem.SetTableDelimiter(m_cTableDelimiter);
		projItem.SetTableQuote(m_cTableQuote);
		projItem.SetTableAllowNewLinesInQuotes(m_bTableAllowNewLinesInQuotes);
	}

	if (bSaveCompareOptions)
	{
		projItem.SetIgnoreWhite(m_nIgnoreWhite);
		projItem.SetIgnoreBlankLines(m_bIgnoreBlankLines);
		projItem.SetIgnoreCase(m_bIgnoreCase);
		projItem.SetIgnoreEol(m_bIgnoreEol);
		projItem.SetIgnoreNumbers(m_bIgnoreNumbers);
		projItem.SetIgnoreCodepage(m_bIgnoreCodepage);
		projItem.SetIgnoreMissingTrailingEol(m_bIgnoreMissingTrailingEol);
		projItem.SetIgnoreLineBreaks(m_bIgnoreLineBreaks);
		projItem.SetFilterCommentsLines(m_bFilterCommentsLines);
		projItem.SetCompareMethod(m_nCompareMethod);
	}

	if (bSaveHiddenItems)
		projItem.SetHiddenItems(m_hiddenItems);
}

}

void ArchiveTool::AddDocument(IMergeDoc* document, bool checked, int diffStatus)
{
	if (document == nullptr)
		return;

	ArchiveItem item;
	item.document = document;
	const int fileCount = document->GetFileCount();
	for (int i = 0; i < fileCount; ++i)
		item.paths.SetPath(i, document->GetPath(i), false);
	item.title = MergeTextFormatter::GetTitleString(*document);
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

	ProjectFile project;

	std::vector<String> commonRoots = GetCommonRoots(selectedItems);

	std::vector<CompressibleItem> archiveItems;
	CPatchTool patchTool;
	for (size_t i = 0; i < selectedItems.size(); ++i)
	{
		IMergeDoc* pDoc = selectedItems[i].document;
		if (pDoc && pDoc->IsModified())
		{
			I18n::MessageBox(IDS_ARCHIVE_SAVEFILES, MB_ICONSTOP);
			return false;
		}

		ProjectFileItem projItem;
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
		PathContext paths;
		for (int j = 0; j < item.paths.GetSize(); ++j)
		{
			const String& root = roots[j];
			const String& path = item.paths[j];
			if (!path.empty() && !paths::IsURL(path))
			{
				String archiveFileName = GetArchiveFileName(path, root, commonRoots[j]);
				paths.SetPath(j, archiveFileName, false);
				if (paths::DoesPathExist(path) == paths::IS_EXISTING_FILE)
				{
					CompressibleItem archiveItem;
					archiveItem.name = archiveFileName;
					archiveItem.fullPath = path;
					archiveItems.push_back(std::move(archiveItem));
				}
			}
			else
			{
				paths.SetPath(j, path, false);
			}
		}
		if (paths.GetSize() >= 2)
		{
			SetProjectFileItem(projItem, paths, item.document);
			project.Items().push_back(projItem);
		}

		if (pDoc->GetDocumentType() == IMergeDoc::DocumentType::Text ||
			pDoc->GetDocumentType() == IMergeDoc::DocumentType::Table)
		{
			if (auto* document = dynamic_cast<CMergeDoc*>(pDoc))
			{
				String left = item.paths.GetLeft();
				String right = item.paths.GetRight();
				if (!paths::IsURL(left) && !paths::IsURL(right) && (!left.empty() || !right.empty()))
				{
					patchTool.AddFiles(left, _T(""), right, _T(""),
						item.title, true, item.diffStatus);
				}
				else
				{
					I18n::MessageBox(IDS_SAVEFILES_FORPATCH, MB_ICONSTOP);
					return false;
				}
			}
		}
	}

	std::vector<IMergeDoc*> documents;
	for (const auto& item : selectedItems)
	{
		if (item.document != nullptr)
			documents.push_back(item.document);
	}

	TempFile patchFile;
	TempFile projectFile;
	String error;
	if (options.includeReport)
	{
		std::shared_ptr<TempFolder> reportFolder = std::make_shared<TempFolder>();
		reportFolder->Create();
		String reportFile = paths::ConcatPath(reportFolder->GetPath(), _T("report.html"));
		if (!CFileCmpReport::GenerateDocumentReport(documents, reportFile, m_reportOptions, error))
		{
			AfxMessageBox(error.c_str(), MB_OK | MB_ICONSTOP);
			return false;
		}
		AddArchiveItem(archiveItems, reportFile, _T("report.html"));

		String reportFilesDir = paths::RemoveExtension(reportFile) + _T(".files");
		if (paths::DoesPathExist(reportFilesDir) == paths::IS_EXISTING_DIR)
			AddArchiveItem(archiveItems, reportFilesDir, _T("report.files"), true);

		GetMainFrame()->AddTempFolder(reportFolder);
	}
	if (options.includePatch)
	{
		patchFile.Create(_T("WM"), _T(".diff"));
		if (!patchTool.GetFileList().empty() && !patchTool.CreatePatchFile(patchFile.GetPath()))
		{
			AfxMessageBox(strutils::format_string1(_("Could not write to file %1."), patchFile.GetPath()).c_str(), MB_OK | MB_ICONSTOP);
			return false;
		}
		AddArchiveItem(archiveItems, patchFile.GetPath(), _T("patch.diff"));
	}
	if (options.includeProject)
	{
		projectFile.Create(_T("WM"), _T(".WinMerge"));
		if (!CMergeApp::SaveProjectFile(projectFile.GetPath(), project))
			return false;
		AddArchiveItem(archiveItems, projectFile.GetPath(), _T("project.WinMerge"));
	}

	if (archiveItems.empty())
	{
		AfxMessageBox(_("No files are available for packaging.").c_str(), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	if (!CompressibleItemEnumerator(std::move(archiveItems)).CompressArchive(archivePath.c_str()))
	{
		AfxMessageBox(strutils::format_string1(_("Could not write to file %1."), archivePath).c_str(), MB_OK | MB_ICONSTOP);
		return false;
	}

	if (temporaryArchive != nullptr)
		GetMainFrame()->AddTempFile(temporaryArchive);

	if (options.copyToClipboard)
		ClipboardUtils::PutFiles(std::vector<String>{ archivePath }, AfxGetMainWnd()->m_hWnd);

	AfxMessageBox((_("Archive file written.") + _T("\n") + archivePath).c_str(), MB_OK | MB_ICONINFORMATION);
	return true;
}
