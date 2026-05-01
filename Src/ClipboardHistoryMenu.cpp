/** 
 * @file  ClipboardHistoryMenu.cpp
 *
 * @brief Implementation for Clipboard history menu helper functions
 */

#include "StdAfx.h"
#include "ClipboardHistoryMenu.h"
#include "UnicodeString.h"
#include <algorithm>

namespace ClipboardHistoryMenu
{
	/**
	 * @brief Populate a menu with clipboard history items
	 */
	int PopulateMenu(CMenu* pMenu, const std::vector<ClipboardHistory::Item>& items, UINT idFirst, UINT idLast)
	{
		if (!pMenu)
			return 0;

		CMenu clipboardMenu;
		clipboardMenu.CreatePopupMenu();

		if (items.empty())
			clipboardMenu.AppendMenu(MF_STRING | MF_GRAYED, 0, _("< Empty >").c_str());

		int itemsAdded = 0;
		UINT id = idFirst;

		for (size_t i = 0; i < items.size() && id <= idLast; ++i, ++id)
		{
			const auto& item = items[i];

			String displayName = item.previewText;

			// Truncate long text
			if (displayName.length() > 60)
				displayName = displayName.substr(0, 57) + _T("...");

			// Replace newlines with spaces for menu display
			std::replace(displayName.begin(), displayName.end(), '\n', ' ');
			std::replace(displayName.begin(), displayName.end(), '\r', ' ');

			// Format menu text with shortcut key
			String menuText = strutils::format(_T("&%c %s"), "123456789abcdefghijklmnopqrstuvwxyz"[i], displayName.c_str());

			clipboardMenu.AppendMenu(MF_STRING, id, menuText.c_str());
			itemsAdded++;
		}

		pMenu->AppendMenu(MF_POPUP, reinterpret_cast<UINT_PTR>(clipboardMenu.m_hMenu), _("Clipboard &History").c_str());

		clipboardMenu.Detach();

		return itemsAdded;
	}
}
