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

	String BuildClipboardItemUrl(const std::vector<ClipboardHistory::Item>& items, size_t index)
	{
		if (index < 0 || index >= items.size())
			return _T("");

		const auto& item = items[index];

		// Generate clipboard URL (1-based index for user-facing URL)
		String url = _T("clipboard://");
		if (index == 0)
			url += _T("latest");
		else if (index == 1)
			url += _T("previous");
		else
			url += strutils::to_str(index + 1);

		// Add format parameter for images
		if (item.pBitmapTempFile)
			url += _T("?format=png#type=image");

		return url;
	}
}
