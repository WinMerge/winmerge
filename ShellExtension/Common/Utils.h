#pragma once

#include <Windows.h>
#include "UnicodeString.h"
#include <vector>
#include <utility>

std::vector<std::pair<int, int>> GetMenuList(DWORD dwMenuState);
BOOL GetWinMergeDir(String& strDir);
String FormatCmdLine(const String& winmergePath,
	const String& path1, const String& path2, const String& path3, BOOL bAlterSubFolders);
BOOL LaunchWinMerge(const String& winmergePath,
	const String& path1, const String& path2, const String& path3, BOOL bAlterSubFolders);
HRESULT InvokeCommand(DWORD verb, DWORD state, String paths[3], String previousPaths[3]);
