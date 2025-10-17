/**
 * @file PropertySystemMenu.cpp
 * @brief Implementation file for PropertySystemMenu
 */
#include "stdafx.h"
#include "PropertySystemMenu.h"
#include "PropertySystem.h"

static void CreatePropertyMenu(HMENU hMenu, const std::vector<String>& canonicalNames, UINT idFirst, const String& format)
{
	if (!hMenu) return;

	std::map<String, HMENU> menuMap;
	menuMap[L""] = hMenu;
	UINT id = idFirst;

	CMenu systemSubAtoI, systemSubJtoR, systemSubStoZ;
	bool systemMenuCreated = false;

	for (const auto& canonical : canonicalNames)
	{
		PropertySystem ps({ canonical });
		std::vector<String> displayNames;
		ps.GetDisplayNames(displayNames);
		if (!canonical.empty() && !displayNames.empty() && !displayNames[0].empty())
		{
			auto partstmp = strutils::split(canonical, L'.');
			String canonical2 = canonical;
			if (partstmp.size() == 2 && partstmp[0] == _T("System"))
			{
				tchar_t ch = canonical[7];
				if (ch >= 'A' && ch <= 'F')
					canonical2 = L"System.A-F." + canonical.substr(7);
				else if (ch >= 'G' && ch <= 'L')
					canonical2 = L"System.G-L." + canonical.substr(7);
				else if (ch >= 'M' && ch <= 'R')
					canonical2 = L"System.M-R." + canonical.substr(7);
				else if (ch >= 'S' && ch <= 'Z')
					canonical2 = L"System.S-Z." + canonical.substr(7);
			}

			String path;
			auto parts = strutils::split(canonical2, L'.');
			for (size_t i = 0; i < parts.size(); ++i)
			{
				if (!path.empty())
					path += L".";
				path += parts[i];

				String parentPath = (i == 0) ? L"" : path.substr(0, path.find_last_of(L'.'));

				if (menuMap.find(path) != menuMap.end())
					continue;

				HMENU hParent = menuMap[parentPath];
				if (!hParent) continue;

				if (i < parts.size() - 1)
				{
					HMENU hSubMenu = CreatePopupMenu();
					AppendMenu(hParent, MF_POPUP, (UINT_PTR)hSubMenu,
						I18n::tr(String(parts[i].data(), parts[i].length())).c_str());
					menuMap[path] = hSubMenu;
				}
				else
				{
					AppendMenu(hParent, MF_STRING, id,
						strutils::format_string1(format, displayNames[0]).c_str());
				}
			}
		}
		id++;
	}
}

std::optional<String> CPropertySystemMenu::ShowMenu(CWnd* pParentWnd, UINT idFirst, const String& format)
{
	std::optional<String> result;
	CMenu menu;
	menu.CreatePopupMenu();
	PropertySystem ps(PropertySystem::VIEWABLE);
	CreatePropertyMenu(menu.m_hMenu, ps.GetCanonicalNames(), idFirst, format);

	CPoint pt;
	GetCursorPos(&pt);
	const int command = menu.TrackPopupMenu(
		TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, pt.x, pt.y, pParentWnd);
	if (command == 0)
	{
		// User cancelled the menu
	}
	else
	{
		result = ps.GetCanonicalNames()[command - idFirst];
	}

	DestroyMenu();

	return result;
}
