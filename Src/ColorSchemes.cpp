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

String GetColorSchemesFolder()
{
	return paths::ConcatPath(env::GetProgPath(), _T("ColorSchemes"));
}

String GetPrivateColorSchemesFolder()
{
	return paths::ConcatPath(env::GetMyDocuments(), _T("WinMerge\\ColorSchemes"));
}

String GetColorSchemePath(const String& name)
{
	const String path = paths::ConcatPath(GetColorSchemesFolder(), name + _T(".ini"));
	if (paths::DoesPathExist(path))
		return path;
	const String pathPrivate = paths::ConcatPath(GetPrivateColorSchemesFolder(), name + _T(".ini"));
	if (paths::DoesPathExist(pathPrivate))
		return pathPrivate;
	return _T("");
}

std::vector<String> GetColorSchemeNames()
{
	DirItemArray dirs, files, filesPrivate;
	std::set<String> names;

	DirTravel::LoadAndSortFiles(GetColorSchemesFolder(), &dirs, &files, false);
	DirTravel::LoadAndSortFiles(GetPrivateColorSchemesFolder(), &dirs, &filesPrivate, false);
	files.insert(files.end(), filesPrivate.begin(), filesPrivate.end());

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
