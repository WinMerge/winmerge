/** 
 * @file  ClipboardHistoryMenu.h
 *
 * @brief Declaration file for Clipboard history menu helper functions
 */
#pragma once

#include "ClipboardHistory.h"
#include <afxwin.h>

namespace ClipboardHistoryMenu
{
    /**
     * @brief Populate a menu with clipboard history items
     * @param [in] pMenu Target menu to populate
     * @param [in] items Clipboard history items to display
     * @param [in] idFirst First menu command ID
     * @param [in] idLast Last menu command ID
     * @return Number of items added to the menu
     */
    int PopulateMenu(CMenu* pMenu, const std::vector<ClipboardHistory::Item>& items, UINT idFirst, UINT idLast);

    /**
     * @brief Build a URL for a clipboard history item
     * @param [in] items Clipboard history items
     * @param [in] index Index of the item
     * @return URL string for the clipboard item
     */
	String BuildClipboardItemUrl(const std::vector<ClipboardHistory::Item>& items, size_t index);
}