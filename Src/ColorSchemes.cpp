#include "pch.h"
#include "ColorSchemes.h"
#include "UnicodeString.h"
#include "DirItem.h"
#include "DirTravel.h"
#include "paths.h"
#include "Environment.h"

namespace ColorSchemes
{

String GetColorSchemesFolder()
{
	return paths::ConcatPath(env::GetProgPath(), _T("ColorSchemes"));
}

String GetColorSchemePath(const String& name)
{
	const String path = paths::ConcatPath(GetColorSchemesFolder(), name + _T(".ini"));
	if (!paths::DoesPathExist(path))
		return _T("");
	return path;
}

std::vector<String> GetColorSchemeNames()
{
	DirItemArray dirs, files;
	std::vector<String> names;

	LoadAndSortFiles(GetColorSchemesFolder(), &dirs, &files, false);

	for (DirItem& item : files)
	{
		String filename;
		String ext;
		paths::SplitFilename(item.filename, nullptr, &filename, &ext);
		if (strutils::compare_nocase(ext, _T("ini")) == 0)
			names.push_back(filename);
	}

	return names;
}

}
