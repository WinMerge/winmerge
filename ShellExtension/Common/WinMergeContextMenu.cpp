#include "WinMergeContextMenu.h"
#include "RegKey.h"
#include "LanguageSelect.h"
#include "../ShellExtension/Resource.h"
#include <Shlwapi.h>

/// Max. filecount to select
static const int MaxFileCount = 3;

/// Registry path to WinMerge
#define REGDIR _T("Software\\Thingamahoochie\\WinMerge")
static const TCHAR f_RegDir[] = REGDIR;
static const TCHAR f_RegLocaleDir[] = REGDIR _T("\\Locale");
static const TCHAR f_RegSettingsDir[] = REGDIR _T("\\Settings");

/**
 * @name Registry valuenames.
 */
 /*@{*/
 /** Shell context menuitem enabled/disabled */
static const TCHAR f_RegValueEnabled[] = _T("ContextMenuEnabled");
/** 'Saved' path in advanced mode */
static const TCHAR f_FirstSelection[] = _T("FirstSelection");
/** 'Saved' path in advanced mode */
static const TCHAR f_SecondSelection[] = _T("SecondSelection");
/** Path to WinMergeU.exe */
static const TCHAR f_RegValuePath[] = _T("Executable");
/** Path to WinMergeU.exe, overwrites f_RegValuePath if present. */
static const TCHAR f_RegValuePriPath[] = _T("PriExecutable");
/** LanguageId */
static const TCHAR f_LanguageId[] = _T("LanguageId");
/** Recurse */
static const TCHAR f_Recurse[] = _T("Recurse");
/*@}*/


/// Reads WinMerge path from registry
static BOOL GetWinMergeDir(String &strDir)
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
static String FormatCmdLine(const String &winmergePath,
		const std::vector<String>& paths, BOOL bAlterSubFolders)
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

	strCommandline += _T(" \"") + paths[0] + _T("\"");

	if (paths.size() > 1)
		strCommandline += _T(" \"") + paths[1] + _T("\"");

	if (paths.size() > 2)
		strCommandline += _T(" \"") + paths[2] + _T("\"");

	return strCommandline;
}

static BOOL LaunchWinMerge(const String &winmergePath,
		const std::vector<String>& paths, BOOL bAlterSubFolders)
{
	String strCommandLine = FormatCmdLine(winmergePath, paths, bAlterSubFolders);

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
		String strCommandLine = FormatCmdLine(_T(""), paths, bAlterSubFolders);
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

WinMergeContextMenu::WinMergeContextMenu(HINSTANCE hInstance)
	: m_hInstance(hInstance)
	, m_dwMenuState(MENU_HIDDEN)
	, m_dwContextMenuEnabled(0)
	, m_langID(0)
{
	CRegKeyEx reg;
	if (reg.Open(HKEY_CURRENT_USER, f_RegDir) == ERROR_SUCCESS)
		m_dwContextMenuEnabled = reg.ReadDword(f_RegValueEnabled, 0);
}

bool WinMergeContextMenu::UpdateMenuState(const std::vector<std::wstring>& paths)
{
	m_strPaths = paths;

	// Check if user wants to use context menu
	CRegKeyEx reg;
	if (reg.Open(HKEY_CURRENT_USER, f_RegDir) != ERROR_SUCCESS)
	{
		m_dwMenuState = MENU_HIDDEN;
		return false;
	}

	DWORD dwContextMenuEnabled = reg.ReadDword(f_RegValueEnabled, 0);
	m_strPreviousPaths[0] = reg.ReadString(f_FirstSelection, _T("")).c_str();
	m_strPreviousPaths[1] = reg.ReadString(f_SecondSelection, _T("")).c_str();

	if (dwContextMenuEnabled & EXT_ENABLED) // Context menu enabled
	{
		// Check if advanced menuitems enabled
		if ((dwContextMenuEnabled & EXT_ADVANCED) == 0)
		{
			m_dwMenuState = MENU_SIMPLE;
		}
		else
		{
			if (m_strPaths.size() == 1 && m_strPreviousPaths[0].empty())
				m_dwMenuState = MENU_ONESEL_NOPREV;
			else if (m_strPaths.size() == 1 && !m_strPreviousPaths[0].empty() && m_strPreviousPaths[1].empty())
				m_dwMenuState = MENU_ONESEL_PREV;
			else if (m_strPaths.size() == 1 && !m_strPreviousPaths[0].empty() && !m_strPreviousPaths[1].empty())
				m_dwMenuState = MENU_ONESEL_TWO_PREV;
			else if (m_strPaths.size() == 2)
				m_dwMenuState = MENU_TWOSEL;
			else if (m_strPaths.size() >= 3)
				m_dwMenuState = MENU_THREESEL;
		}
	}
	else
	{
		m_dwMenuState = MENU_HIDDEN;
	}
	return true;
}

std::vector<MenuItem> WinMergeContextMenu::GetMenuItemList() const
{
	std::vector<MenuItem> list;
	list.reserve(3);
	bool isdir = m_strPaths.empty() ? false : !!PathIsDirectory(m_strPaths[0].c_str());
	bool enabled = (m_strPaths.size() <= MaxFileCount);
	int icon = isdir ? IDI_WINMERGEDIR : IDI_WINMERGE;

	switch (m_dwMenuState)
	{
	case MENU_HIDDEN:
		break;

	case MENU_SIMPLE:
		list.push_back({ enabled, icon, CMD_COMPARE, IDS_CONTEXT_MENU, GetResourceString(IDS_CONTEXT_MENU) });
		break;

		// No items selected earlier
		// Select item as first item to compare
	case MENU_ONESEL_NOPREV:
		list.push_back({ enabled, icon, CMD_SELECT_LEFT, IDS_SELECT_LEFT, GetResourceString(IDS_SELECT_LEFT) });
		list.push_back({ enabled, icon, CMD_COMPARE_ELLIPSIS, IDS_COMPARE_ELLIPSIS, GetResourceString(IDS_COMPARE_ELLIPSIS) });
		break;

		// One item selected earlier:
		// Allow re-selecting first item or selecting second item
	case MENU_ONESEL_PREV:
		list.push_back({ enabled, icon, CMD_COMPARE, IDS_COMPARE, GetResourceString(IDS_COMPARE) });
		list.push_back({ enabled, icon, CMD_SELECT_MIDDLE, IDS_SELECT_MIDDLE, GetResourceString(IDS_SELECT_MIDDLE) });
		list.push_back({ enabled, icon, CMD_RESELECT_LEFT, IDS_RESELECT_LEFT, GetResourceString(IDS_RESELECT_LEFT) });
		break;

		// Two items are selected earlier:
		// Allow re-selecting first item or selecting second item
	case MENU_ONESEL_TWO_PREV:
		list.push_back({ enabled, icon, CMD_COMPARE, IDS_COMPARE, GetResourceString(IDS_COMPARE) });
		list.push_back({ enabled, icon, CMD_RESELECT_LEFT, IDS_RESELECT_LEFT, GetResourceString(IDS_RESELECT_LEFT) });
		break;

		// Two items selected
		// Select both items for compare
	case MENU_TWOSEL:
	case MENU_THREESEL:
	default:
		list.push_back({ enabled, icon, CMD_COMPARE, IDS_COMPARE, GetResourceString(IDS_COMPARE) });
		break;
	}
	return list;
}

/// Runs WinMerge with given paths
HRESULT WinMergeContextMenu::InvokeCommand(DWORD verb)
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
		bCompare = TRUE;
		switch (m_dwMenuState)
		{
		case MENU_ONESEL_PREV:
			m_strPaths.resize(2);
			m_strPaths[1] = m_strPaths[0];
			m_strPaths[0] = m_strPreviousPaths[0];

			// Forget previous selection
			if (reg.Open(HKEY_CURRENT_USER, f_RegDir) == ERROR_SUCCESS)
			{
				reg.WriteString(f_FirstSelection, _T(""));
				reg.WriteString(f_SecondSelection, _T(""));
			}
			break;

		case MENU_ONESEL_TWO_PREV:
			m_strPaths.resize(3);
			m_strPaths[2] = m_strPaths[0];
			m_strPaths[0] = m_strPreviousPaths[0];
			m_strPaths[1] = m_strPreviousPaths[1];

			// Forget previous selection
			if (reg.Open(HKEY_CURRENT_USER, f_RegDir) == ERROR_SUCCESS)
			{
				reg.WriteString(f_FirstSelection, _T(""));
				reg.WriteString(f_SecondSelection, _T(""));
			}
			break;

		case MENU_TWOSEL:
		case MENU_THREESEL:
			// "Compare" - compare m_strPaths
			m_strPreviousPaths[0].erase();
			m_strPreviousPaths[1].erase();
			break;
		}
	}
	else if (verb == CMD_COMPARE_ELLIPSIS)
	{
		// "Compare..." - user wants to compare this single item and open WinMerge
		m_strPaths.resize(1);
		bCompare = TRUE;
	}
	else if (verb == CMD_SELECT_LEFT)
	{
		// Select Left
		m_strPreviousPaths[0] = m_strPaths[0];
		if (reg.Open(HKEY_CURRENT_USER, f_RegDir) == ERROR_SUCCESS)
			reg.WriteString(f_FirstSelection, m_strPreviousPaths[0].c_str());
	}
	else if (verb == CMD_SELECT_MIDDLE)
	{
		// Select Middle
		m_strPreviousPaths[1] = m_strPaths[0];
		if (reg.Open(HKEY_CURRENT_USER, f_RegDir) == ERROR_SUCCESS)
			reg.WriteString(f_SecondSelection, m_strPreviousPaths[1].c_str());
	}
	else if (verb == CMD_RESELECT_LEFT)
	{
		// Re-select Left
		m_strPreviousPaths[0] = m_strPaths[0];
		m_strPreviousPaths[1].clear();
		if (reg.Open(HKEY_CURRENT_USER, f_RegDir) == ERROR_SUCCESS)
		{
			reg.WriteString(f_FirstSelection, m_strPreviousPaths[0].c_str());
			reg.WriteString(f_SecondSelection, _T(""));
		}
	}
	else
		return E_INVALIDARG;

	if (bCompare == FALSE)
		return S_FALSE;

	if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0)
		bAlterSubFolders = TRUE;

	return LaunchWinMerge(strWinMergePath, m_strPaths, bAlterSubFolders) ? S_OK : S_FALSE;
}

/**
 * @brief Load a string from resource.
 * @param [in] Resource string ID.
 * @return String loaded from resource.
 */
std::wstring WinMergeContextMenu::GetResourceString(UINT id) const
{
	if (!s_pLang)
		s_pLang = new CLanguageSelect();
	if (m_langID == 0)
	{
		CRegKeyEx reg;
		if (reg.Open(HKEY_CURRENT_USER, f_RegLocaleDir) == ERROR_SUCCESS)
			m_langID = static_cast<LANGID>(reg.ReadDword(f_LanguageId, m_langID));
	}
	if (s_pLang->GetLangId() != m_langID)
	{
		TCHAR szFileName[1024] = {0};
		GetModuleFileName(m_hInstance, szFileName, sizeof(szFileName) / sizeof(TCHAR));
		PathRemoveFileSpec(szFileName);
		String languagesFolder = String(szFileName) + _T("\\Languages\\ShellExtension");
		s_pLang->LoadLanguageFile(m_langID, languagesFolder);
	}
	return s_pLang->GetResourceString(m_hInstance, id);
}
