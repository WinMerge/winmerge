/**
 * @file PluginMenu.h
 * @brief Declaration file for PluginMenu namespace functions
 */
#pragma once

#include "UnicodeString.h"
#include <vector>
#include <string>

class CMenu;
class PluginForFile;
class CCmdUI;
class CWnd;

namespace PluginMenu
{
	enum MenuFlags
	{
		None = 0,
		AddAllMenu = 1 << 0, /**< Add "All" menu item */
		AddSelectMenu = 1 << 1, /**< Add "Select..." menu item */
		FlattenMenu = 1 << 2 /**< Flatten the menu structure */
	};
	void AppendPluginMenus(CMenu* pMenu, const PluginForFile* pluginInfo, const String& filteredFilenames, const std::vector<std::wstring>& events, unsigned flags, unsigned baseId);
	String GetPluginPipelineByMenuId(const PluginForFile* pluginInfo, unsigned idSearch, const std::vector<std::wstring>& events, unsigned baseId);
	void ShowMenu(const PluginForFile* pluginInfo, const String& filteredFilenames, const std::vector<std::wstring>& events, unsigned flags, unsigned baseId, int x, int y, CWnd* pParentWnd);
	void UpdateMenu(CCmdUI* pCmdUI);
}
