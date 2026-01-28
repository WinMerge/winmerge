#include "pch.h"
#include "ColorSchemes.h"
#include "UnicodeString.h"
#include "DirItem.h"
#include "DirTravel.h"
#include "paths.h"
#include "Environment.h"
#include <set>

namespace ColorSchemes
{

std::vector<String> GetColorSchemesFolders()
{
	std::vector<String> folders;
	folders.push_back(paths::ConcatPath(env::GetProgPath(), _T("ColorSchemes")));
	folders.push_back(paths::ConcatPath(env::GetAppDataPath(), _T("WinMerge\\ColorSchemes")));
	folders.push_back(paths::ConcatPath(env::GetMyDocuments(), _T("WinMerge\\ColorSchemes")));
	return folders;
}

String GetPrivateColorSchemesFolder(bool useMyDocument)
{
	const auto folders = GetColorSchemesFolders();
	return folders[useMyDocument ? 2 : 1];
}

bool IsPrivateColorSchemePath(const String& path)
{
	const String parentFolder = paths::GetParentPath(path);
	int i = 0;
	for (const auto& folder : GetColorSchemesFolders())
	{
		if (i > 0)
		{
			if (strutils::compare_nocase(folder, parentFolder) == 0)
				return true;
		}
		++i;
	}
	return false;
}

String GetColorSchemePath(const String& name)
{
	for (const String& folder : GetColorSchemesFolders())
	{
		const String path = paths::ConcatPath(folder, name + _T(".ini"));
		if (paths::DoesPathExist(path))
			return path;
	}
	return _T("");
}

std::vector<String> GetColorSchemeNames()
{
	DirItemArray files;
	std::set<String> names;

	for (const String& folder : GetColorSchemesFolders())
	{
		DirItemArray dirs, filesTmp;
		DirTravel::LoadAndSortFiles(folder, &dirs, &filesTmp, false);
		files.insert(files.end(), filesTmp.begin(), filesTmp.end());
	}

	for (DirItem& item : files)
	{
		String filename;
		String ext;
		paths::SplitFilename(item.filename, nullptr, &filename, &ext);
		if (strutils::compare_nocase(ext, _T("ini")) == 0)
			names.insert(filename);
	}

	return std::vector<String>(names.begin(), names.end());
}

}
