#pragma once

#include <Windows.h>
#include <vector>
#include <utility>
#include <memory>
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
	/**
	 * @brief Flags for enabling and other settings of context menu.
	 */
	enum ExtensionFlags
	{
		EXT_ENABLED = 0x01, /**< ShellExtension enabled/disabled. */
		EXT_ADVANCED = 0x02, /**< Advanced menuitems enabled/disabled. */
		EXT_COMPARE_AS = 0x04, /**< Show Compare As menu. */
	};

	enum
	{
		CMD_COMPARE,
		CMD_COMPARE_ELLIPSIS,
		CMD_COMPARE_AS,
		CMD_SELECT_LEFT,
		CMD_SELECT_MIDDLE,
		CMD_RESELECT_LEFT,
		CMD_LAST = CMD_RESELECT_LEFT,
	};

	/**
	 * @brief The states in which the menu can be.
	 * These states define what items are added to the menu and how those
	 * items work.
	 */
	enum
	{
		MENU_HIDDEN = -1,
		MENU_SIMPLE = 0,  /**< Simple menu, only "Compare item" is shown. */
		MENU_ONESEL_NOPREV,  /**< One item selected, no previous selections. */
		MENU_ONESEL_PREV,  /**< One item selected, previous selection exists. */
		MENU_ONESEL_TWO_PREV,  /**< One item selected, two previous selections exist. */
		MENU_TWOSEL,  /**< Two items are selected. */
		MENU_THREESEL
	};

	explicit WinMergeContextMenu(HINSTANCE hInstance);
	bool UpdateMenuState(const std::vector<std::wstring>& paths);
	std::vector<MenuItem> GetMenuItemList() const;
	HRESULT InvokeCommand(DWORD verb);
	std::wstring GetResourceString(UINT id) const;
	DWORD GetMenuState() const { return m_dwMenuState; }
	DWORD GetContextMenuEnabled() const { return m_dwContextMenuEnabled; }
	void SetSite(IUnknown* pUnknown) { m_pSite = pUnknown; };
	const std::vector<std::wstring>& GetPaths() const { return m_strPaths; }

private:
	DWORD m_dwMenuState; /**< Shown menuitems */
	std::vector<std::wstring> m_strPaths; /**< Paths for selected items */
	std::wstring m_strPreviousPaths[2]; /**< Previously selected path */
	HINSTANCE m_hInstance;
	inline static CLanguageSelect* s_pLang;
	mutable LANGID m_langID; /**< Current Language Id */
	DWORD m_dwContextMenuEnabled; /**< 0, 2: context menu disabled 1: context menu enabled 3: Advanced menu enabled */
    IUnknown *m_pSite;
};

