/**
 * @file  ReplaceListHelper.cpp
 * @brief Implementation of replace-list UI helper functions
 */
#include "StdAfx.h"
#include "ReplaceListHelper.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"
#include "paths.h"
#include "FileOrFolderSelect.h"
#include "DirItem.h"
#include "DirTravel.h"
#include "Environment.h"
#include "Shell.h"
#include "UniFile.h"

namespace ReplaceListHelper
{

String GetReplaceListFolder(int locationType, bool isRegex)
{
	String folder = locationType == 0 ? env::GetAppDataPath() : env::GetMyDocuments();
	return paths::ConcatPath(folder, isRegex ? _T("WinMerge\\RegexReplaceLists") : _T("WinMerge\\ReplaceLists"));
}

std::vector<String> GetReplaceLists(bool isRegex)
{
	int locationType = GetOptionsMgr()->GetInt(OPT_USERDATA_LOCATION);
	std::vector<String> list;
	String folder = GetReplaceListFolder(locationType, isRegex);

	if (folder.empty())
		return list;

	// Create folder if it doesn't exist
	if (paths::DoesPathExist(folder) != paths::IS_EXISTING_DIR)
	{
		if (!paths::CreateIfNeeded(folder))
			return list;
	}

	DirItemArray files, dirs;
	DirTravel::LoadFiles(folder, &dirs, &files, _T("*.*"));

	for (const auto& file : files)
		list.push_back(paths::ConcatPath(folder, file.filename.get()));

	DirItemArray files2, dirs2;
	folder = GetReplaceListFolder(1 - locationType, isRegex);
	DirTravel::LoadFiles(folder, &dirs2, &files2, _T("*.*"));

	for (const auto& file : files2)
		list.push_back(paths::ConcatPath(folder, file.filename.get()));

	return list;
}

bool CreateTemplateFile(const String& filepath, bool isRegex)
{
	// Write template content
	String templateText = isRegex ?
			// Regex replacement list template
			_("# Regex replacement list\r\n"
			"# Format: regex<TAB>replacement\r\n"
			"# Backreferences like $1, $2 are supported\r\n"
			"\r\n"
			"(\\d{4})-(\\d{2})-(\\d{2})\t$1_$2_$3\r\n")
		:
			// Replacement list template
			_("# Replacement list\r\n"
			"# Format: search<TAB>replacement\r\n"
			"# Lines starting with # are ignored\r\n"
			"\r\n"
			"from1\tto1\r\n"
			"from2\tto2\r\n");

	UniStdioFile file;
	if (!file.OpenCreateUtf8(filepath))
		return false;
	file.WriteString(templateText);
	return true;
}

String ReplaceAppDataFolderOrUserProfileFolder(const String& path)
{
	String result = path;
	String appData = env::GetAppDataPath();
	if (tc::tcsstr(path.c_str(), appData.c_str()))
	{
		strutils::replace(result, appData, _T("%APPDATA%"));
		return result;
	}
	String userProfile = env::ExpandEnvironmentVariables(_T("%USERPROFILE%"));
	if (tc::tcsstr(path.c_str(), userProfile.c_str()))
	{
		strutils::replace(result, userProfile, _T("%USERPROFILE%"));
		return result;
	}
	return result;
}

std::optional<String> CreateAndSelectReplaceListFile(CWnd* pParentWnd, bool isRegex)
{
	int locationType = GetOptionsMgr()->GetInt(OPT_USERDATA_LOCATION);
	String folder = GetReplaceListFolder(locationType, isRegex);
	if (folder.empty())
	{
		AfxMessageBox(_T("Failed to get ReplaceList folder."), MB_ICONERROR);
		return std::nullopt;
	}

	// Create folder if it doesn't exist
	if (paths::DoesPathExist(folder) != paths::IS_EXISTING_DIR)
	{
		if (!paths::CreateIfNeeded(folder))
		{
			AfxMessageBox((_T("Failed to create folder:\n") + folder).c_str(), MB_ICONERROR);
			return std::nullopt;
		}
	}

	String title = isRegex ?
		_("Create &Regex Replace List and Insert...") :
		_("&Create String Replace List and Insert...");
	strutils::replace(title, _T("&"), _T("")); // Remove & for file dialog title

	// Display file save dialog
	String sFilePath;
	String initialDir = folder;
	if (!SelectFile(pParentWnd->GetSafeHwnd(), sFilePath, false, initialDir.c_str(), title, 
		_("Tab-Separated Values (*.tsv;*.txt)|*.tsv;*.txt|All Files (*.*)|*.*||"), _T("tsv")))
		return std::nullopt;

	String filepath = sFilePath;

	// Create template file
	if (!CreateTemplateFile(filepath, isRegex))
	{
		AfxMessageBox((_T("Failed to create file:\n") + filepath).c_str(), MB_ICONERROR);
		return std::nullopt;
	}

	// Open with default editor
	shell::Edit(filepath.c_str());

	return filepath;
}

} // namespace ReplaceListHelper
