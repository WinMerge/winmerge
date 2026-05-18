/**
 * @file  FilterMenuHelpers.h
 * @brief Common menu helper functions for filter menus
 */
#pragma once

#include <afxwin.h>

namespace FilterMenuHelpers
{
	/**
	 * @brief Traverse menu recursively and apply a function to each item
	 * @param pMenu Menu to traverse
	 * @param func Function to apply to each menu item (takes CMenu*, index, id)
	 */
	template <typename Func>
	inline void TraverseMenuRecursive(CMenu* pMenu, Func func)
	{
		if (!pMenu)
			return;
		int itemCount = pMenu->GetMenuItemCount();
		for (int i = itemCount - 1; i >= 0; --i)
		{
			UINT id = pMenu->GetMenuItemID(i);
			if (id == (UINT)-1)
			{
				CMenu* pSubMenu = pMenu->GetSubMenu(i);
				if (pSubMenu)
					TraverseMenuRecursive(pSubMenu, func);
			}
			func(pMenu, i, id);
		}
	}

	/**
	 * @brief Disable a menu item recursively in all submenus
	 */
	inline void DisableMenuItemRecursive(CMenu* pMenu, UINT idDisabled)
	{
		TraverseMenuRecursive(pMenu, [idDisabled](CMenu* pMenu, int index, UINT id)
			{
				if (id == idDisabled)
					pMenu->EnableMenuItem(index, MF_DISABLED | MF_BYPOSITION);
			});
	}

	/**
	 * @brief Check/uncheck a menu item recursively in all submenus
	 */
	inline void CheckMenuItemRecursive(CMenu* pMenu, UINT idChecked, bool checked)
	{
		TraverseMenuRecursive(pMenu, [idChecked, checked](CMenu* pMenu, int index, UINT id)
			{
				if (id == idChecked)
					pMenu->CheckMenuItem(index, (checked ? MF_CHECKED : 0) | MF_BYPOSITION);
			});
	}

	/**
	 * @brief Remove menu items in a range recursively
	 */
	inline void RemoveMenuItemsInRangeRecursive(CMenu* pMenu, UINT idStart, UINT idEnd)
	{
		TraverseMenuRecursive(pMenu, [idStart, idEnd](CMenu* pMenu, int index, UINT id)
			{
				if (id >= idStart && id <= idEnd)
					pMenu->RemoveMenu(index, MF_BYPOSITION);
			});
	}

	/**
	 * @brief Remove trailing separator from menu and all submenus
	 */
	inline void RemoveTrailingSeparator(CMenu* pMenu)
	{
		if (!pMenu)
			return;
		int count = pMenu->GetMenuItemCount();
		if (count > 0)
		{
			UINT state = pMenu->GetMenuState(count - 1, MF_BYPOSITION);
			if (state & MF_SEPARATOR)
				pMenu->RemoveMenu(count - 1, MF_BYPOSITION);
		}
		TraverseMenuRecursive(pMenu, [](CMenu* pMenu, int index, UINT id)
			{
				if (id == (UINT)-1)
				{
					CMenu* pSubMenu = pMenu->GetSubMenu(index);
					if (pSubMenu)
						RemoveTrailingSeparator(pSubMenu);
				}
			});
	}
}
