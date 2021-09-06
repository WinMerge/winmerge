#include "Utils.h"
#include "RegKey.h"
#include "Constants.h"
#include "../ShellExtension/Resource.h"
#include <Shlwapi.h>

std::vector<std::pair<int, int>> GetMenuList(DWORD dwMenuState)
{
	std::vector<std::pair<int, int>> verbIdStringIdList;
	switch (dwMenuState)
	{
	case MENU_SIMPLE:
		verbIdStringIdList.emplace_back(CMD_COMPARE, IDS_CONTEXT_MENU);
		break;

		// No items selected earlier
		// Select item as first item to compare
	case MENU_ONESEL_NOPREV:
		verbIdStringIdList.emplace_back(CMD_SELECT_LEFT, IDS_SELECT_LEFT);
		verbIdStringIdList.emplace_back(CMD_COMPARE_ELLIPSE, IDS_COMPARE_ELLIPSIS);
		break;

		// One item selected earlier:
		// Allow re-selecting first item or selecting second item
	case MENU_ONESEL_PREV:
		verbIdStringIdList.emplace_back(CMD_COMPARE, IDS_COMPARE);
		verbIdStringIdList.emplace_back(CMD_SELECT_MIDDLE, IDS_SELECT_MIDDLE);
		verbIdStringIdList.emplace_back(CMD_RESELECT_LEFT, IDS_RESELECT_LEFT);
		break;

		// Two items are selected earlier:
		// Allow re-selecting first item or selecting second item
	case MENU_ONESEL_TWO_PREV:
		verbIdStringIdList.emplace_back(CMD_COMPARE, IDS_COMPARE);
		verbIdStringIdList.emplace_back(CMD_RESELECT_LEFT, IDS_RESELECT_LEFT);
		break;

		// Two items selected
		// Select both items for compare
	case MENU_TWOSEL:
	case MENU_THREESEL:
	default:
		verbIdStringIdList.emplace_back(CMD_COMPARE, IDS_COMPARE);
		break;
	}
	return verbIdStringIdList;
}

/// Reads WinMerge path from registry
BOOL GetWinMergeDir(String &strDir)
{
	CRegKeyEx reg;
	if (!reg.QueryRegUser(f_RegDir))
		return FALSE;

	// Try first reading debug/test value
	strDir = reg.ReadString(f_RegValuePriPath, _T(""));
	if (strDir.empty())
	{
		strDir = reg.ReadString(f_RegValuePath, _T(""));
		if (strDir.empty())
			return FALSE;
	}

	return TRUE;
}

/// Format commandline used to start WinMerge
String FormatCmdLine(const String &winmergePath,
		const String &path1, const String &path2, const String &path3, BOOL bAlterSubFolders)
{
	String strCommandline = winmergePath.empty() ? _T("") : _T("\"") + winmergePath + _T("\"");

	// Check if user wants to use context menu
	BOOL bSubfoldersByDefault = FALSE;
	CRegKeyEx reg;
	if (reg.Open(HKEY_CURRENT_USER, f_RegSettingsDir) == ERROR_SUCCESS)
		bSubfoldersByDefault = reg.ReadBool(f_Recurse, FALSE);

	if (bAlterSubFolders && !bSubfoldersByDefault)
		strCommandline += _T(" /r");
	else if (!bAlterSubFolders && bSubfoldersByDefault)
		strCommandline += _T(" /r");

	strCommandline += _T(" \"") + path1 + _T("\"");

	if (!path2.empty())
		strCommandline += _T(" \"") + path2 + _T("\"");

	if (!path3.empty())
		strCommandline += _T(" \"") + path3 + _T("\"");

	return strCommandline;
}

BOOL LaunchWinMerge(const String &winmergePath,
		const String &path1, const String &path2, const String &path3, BOOL bAlterSubFolders)
{
	String strCommandLine = FormatCmdLine(winmergePath,
		path1, path2, path3, bAlterSubFolders);

	// Finally start a new WinMerge process
	BOOL retVal = FALSE;
	STARTUPINFO stInfo = { 0 };
	stInfo.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION processInfo = { 0 };

	retVal = CreateProcess(NULL, (LPTSTR)strCommandLine.c_str(),
		NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL,
		&stInfo, &processInfo);

	if (retVal)
	{
		CloseHandle(processInfo.hThread);
		CloseHandle(processInfo.hProcess);
	}
	else if (GetLastError() == ERROR_ELEVATION_REQUIRED)
	{
		String strCommandLine = FormatCmdLine(_T(""),
			path1, path2, path3, bAlterSubFolders);
		HINSTANCE hInstance = ShellExecute(nullptr, _T("runas"), winmergePath.c_str(), strCommandLine.c_str(), 0, SW_SHOWNORMAL);
		if (reinterpret_cast<intptr_t>(hInstance) < 32)
			return FALSE;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

/// Runs WinMerge with given paths
HRESULT InvokeCommand(DWORD verb, DWORD state, String paths[3], String previousPaths[3])
{
	CRegKeyEx reg;
	String strWinMergePath;
	BOOL bCompare = FALSE;
	BOOL bAlterSubFolders = FALSE;

	// Read WinMerge location from registry
	if (!GetWinMergeDir(strWinMergePath))
		return S_FALSE;

	// Check that file we are trying to execute exists
	if (!PathFileExists(strWinMergePath.c_str()))
		return S_FALSE;

	if (verb == CMD_COMPARE)
	{
		switch (state)
		{
		case MENU_SIMPLE:
			bCompare = TRUE;
			break;

		case MENU_ONESEL_PREV:
			paths[1] = paths[0];
			paths[0] = previousPaths[0];
			bCompare = TRUE;

			// Forget previous selection
			if (reg.Open(HKEY_CURRENT_USER, f_RegDir) == ERROR_SUCCESS)
			{
				reg.WriteString(f_FirstSelection, _T(""));
				reg.WriteString(f_SecondSelection, _T(""));
			}
			break;

		case MENU_ONESEL_TWO_PREV:
			paths[2] = paths[0];
			paths[0] = previousPaths[0];
			paths[1] = previousPaths[1];
			bCompare = TRUE;

			// Forget previous selection
			if (reg.Open(HKEY_CURRENT_USER, f_RegDir) == ERROR_SUCCESS)
			{
				reg.WriteString(f_FirstSelection, _T(""));
				reg.WriteString(f_SecondSelection, _T(""));
			}
			break;

		case MENU_TWOSEL:
		case MENU_THREESEL:
			// "Compare" - compare paths
			bCompare = TRUE;
			previousPaths[0].erase();
			previousPaths[1].erase();
			break;
		}
	}
	else if (verb == CMD_COMPARE_ELLIPSE)
	{
		// "Compare..." - user wants to compare this single item and open WinMerge
		paths[1].erase();
		paths[2].erase();
		bCompare = TRUE;
	}
	else if (verb == CMD_SELECT_LEFT)
	{
		// Select Left
		previousPaths[0] = paths[0];
		if (reg.Open(HKEY_CURRENT_USER, f_RegDir) == ERROR_SUCCESS)
			reg.WriteString(f_FirstSelection, previousPaths[0].c_str());
	}
	else if (verb == CMD_SELECT_MIDDLE)
	{
		// Select Middle
		previousPaths[1] = paths[0];
		if (reg.Open(HKEY_CURRENT_USER, f_RegDir) == ERROR_SUCCESS)
			reg.WriteString(f_SecondSelection, previousPaths[1].c_str());
	}
	else if (verb == CMD_RESELECT_LEFT)
	{
		// Re-select Left
		previousPaths[0] = paths[0];
		previousPaths[1].clear();
		if (reg.Open(HKEY_CURRENT_USER, f_RegDir) == ERROR_SUCCESS)
		{
			reg.WriteString(f_FirstSelection, previousPaths[0].c_str());
			reg.WriteString(f_SecondSelection, _T(""));
		}
	}
	else
		return E_INVALIDARG;

	if (bCompare == FALSE)
		return S_FALSE;

	if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0)
		bAlterSubFolders = TRUE;

	return LaunchWinMerge(strWinMergePath,
		paths[0], paths[1], paths[2], bAlterSubFolders) ? S_OK : S_FALSE;
}

