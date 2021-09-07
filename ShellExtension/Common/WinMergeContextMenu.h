#pragma once

#include <Windows.h>
#include <vector>
#include <utility>
#include <memory>
#include <tuple>
#include "UnicodeString.h"
#include "LanguageSelect.h"

struct MenuItem
{
	bool enabled;
	int icon;
	int verb;
	int strid;
	std::wstring text;
};

class WinMergeContextMenu
{
public:
	explicit WinMergeContextMenu(HINSTANCE hInstance);
	bool UpdateMenuState(const std::vector<std::wstring>& paths);
	std::vector<MenuItem> GetMenuItemList() const;
	HRESULT InvokeCommand(DWORD verb);
	std::wstring GetHelpText(DWORD verb) const;
	std::wstring GetResourceString(UINT id) const;
	DWORD GetMenuState() const { return m_dwMenuState; }
private:
	DWORD m_dwMenuState; /**< Shown menuitems */
	std::vector<std::wstring> m_strPaths; /**< Paths for selected items */
	std::wstring m_strPreviousPaths[2]; /**< Previously selected path */
	HINSTANCE m_hInstance;
	inline static std::unique_ptr<CLanguageSelect> s_pLang;
	mutable LANGID m_langID; /**< Current Language Id */
};

