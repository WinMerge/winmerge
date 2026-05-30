/**
 * @file  FilterMenuHelpers.cpp
 * @brief Implementation of common menu helper functions for filter menus
 */
#include "StdAfx.h"
#include "FilterMenuHelpers.h"
#include "ReplaceListHelper.h"
#include "paths.h"
#include "unicoder.h"

namespace FilterMenuHelpers
{
	/**
	 * @brief Populate a replace list submenu with available list files
	 */
	void PopulateReplaceListMenu(CMenu* pMenu, bool isRegex, UINT firstID)
	{
		// Delete existing "<None>" item
		pMenu->DeleteMenu(0, MF_BYPOSITION);

		// Get lists in folder
		std::vector<String> lists = ReplaceListHelper::GetReplaceLists(isRegex);

		if (lists.empty())
		{
			pMenu->AppendMenu(MF_STRING | MF_GRAYED, firstID, _("<None>").c_str());
		}
		else
		{
			for (size_t i = 0; i < lists.size() && i < ReplaceListHelper::MaxReplaceListSize; i++)
			{
				String filename = paths::FindFileName(lists[i]);
				pMenu->AppendMenu(MF_STRING, firstID + static_cast<UINT>(i), filename.c_str());
			}
		}
	}

	/**
	 * @brief Populate "Replace Lists" submenus (both String and Regex)
	 */
	void PopulateReplaceListsSubMenus(CMenu* pReplaceListsMenu, UINT stringListFirstID, UINT regexListFirstID)
	{
		// Find String Replace Lists and Regex Replace Lists submenus
		for (int i = 0; i < pReplaceListsMenu->GetMenuItemCount(); i++)
		{
			CMenu* pSubMenu = pReplaceListsMenu->GetSubMenu(i);
			if (pSubMenu)
			{
				MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
				mii.fMask = MIIM_ID;
				if (pSubMenu->GetMenuItemInfo(0, &mii, TRUE))
				{
					if (mii.wID == stringListFirstID)
					{
						PopulateReplaceListMenu(pSubMenu, false, stringListFirstID);
					}
					else if (mii.wID == regexListFirstID)
					{
						PopulateReplaceListMenu(pSubMenu, true, regexListFirstID);
					}
				}
			}
		}
	}

	/**
	 * @brief Find and populate "Replace Lists" menu recursively
	 */
	void PopulateReplaceLists(CMenu* pPopup, UINT stringListFirstID, UINT regexListFirstID)
	{
		if (!pPopup)
			return;

		// Search "Replace Lists" submenu recursively
		for (int i = 0; i < pPopup->GetMenuItemCount(); i++)
		{
			CMenu* pSubMenu = pPopup->GetSubMenu(i);
			if (pSubMenu)
			{
				MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
				mii.fMask = MIIM_STRING;
				tchar_t buf[256];
				mii.dwTypeData = buf;
				mii.cch = 256;
				if (pPopup->GetMenuItemInfo(i, &mii, TRUE))
				{
					if (tc::tcsstr(buf, _("Replace &Lists").c_str()))
					{
						PopulateReplaceListsSubMenus(pSubMenu, stringListFirstID, regexListFirstID);
						return;
					}
				}
				// Recursively search in submenu
				PopulateReplaceLists(pSubMenu, stringListFirstID, regexListFirstID);
			}
		}
	}
}
