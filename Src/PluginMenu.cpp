/**
 * @file PluginMenu.cpp
 * @brief Implementation of the PluginMenu namespace functions
 */
#include "stdafx.h"
#include "PluginMenu.h"
#include "Plugins.h"
#include "FileTransform.h"
#include "SelectPluginDlg.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"

void PluginMenu::AppendPluginMenus(CMenu *pMenu, const String& filteredFilenames,
	const std::vector<std::wstring>& events, unsigned flags, unsigned baseId)
{
	if (!GetOptionsMgr()->GetBool(OPT_PLUGINS_ENABLED))
		return;

	CWaitCursor waitstatus;

	auto [suggestedPlugins, allPlugins] = FileTransform::CreatePluginMenuInfos(filteredFilenames, events, baseId);

	if ((flags & MenuFlags::AddAllMenu) == 0)
	{
		pMenu->AppendMenu(MF_STRING, ID_SUGGESTED_PLUGINS, _("Suggested Plugins").c_str());
	}
	else
	{
		pMenu->AppendMenu(MF_SEPARATOR);
	}

	for (const auto& [caption, name, id, plugin] : suggestedPlugins)
		pMenu->AppendMenu(MF_STRING, id, caption.c_str());

	CMenu* pMenu2 = pMenu;
	CMenu popupAll;
	if ((flags & MenuFlags::AddAllMenu) != 0)
	{
		popupAll.CreatePopupMenu();
		pMenu->AppendMenu(MF_POPUP, reinterpret_cast<UINT_PTR>(popupAll.m_hMenu), _("Al&l").c_str());
		pMenu2 = &popupAll;
	}
	else
	{
		pMenu->AppendMenu(MF_SEPARATOR, 0);
		pMenu->AppendMenu(MF_STRING, ID_NOT_SUGGESTED_PLUGINS, _("All Plugins").c_str());
	}

	std::list<String> processTypes;
	for (const auto& [processType, pluginList] : allPlugins)
		processTypes.push_back(processType);
	auto it = std::find(processTypes.begin(), processTypes.end(), _("&Others"));
	if (it != processTypes.end())
	{
		processTypes.erase(it);
		processTypes.push_back(_("&Others"));
	}

	for (const auto& processType : processTypes)
	{
		if ((flags & MenuFlags::FlattenMenu) != 0)
		{
			for (const auto& [caption, name, id, plugin] : allPlugins[processType])
				pMenu2->AppendMenu(MF_STRING, id, caption.c_str());
		}
		else
		{
			if (processType.empty())
			{
				for (const auto& [caption, name, id, plugin] : allPlugins[processType])
					pMenu2->AppendMenu(MF_STRING, id, caption.c_str());
			}
			else
			{
				CMenu popup;
				popup.CreatePopupMenu();
				for (const auto& [caption, name, id, plugin] : allPlugins[processType])
					popup.AppendMenu(MF_STRING, id, caption.c_str());
				pMenu2->AppendMenu(MF_POPUP, reinterpret_cast<UINT_PTR>(popup.Detach()), processType.c_str());
			}
		}
	}

	if ((flags & MenuFlags::AddSelectMenu) != 0)
	{
		if (baseId == ID_UNPACKERS_FIRST)
			pMenu2->AppendMenu(MF_STRING, ID_OPEN_WITH_UNPACKER, _("&Select...").c_str());
		else if (baseId == ID_PREDIFFERS_FIRST)
			pMenu2->AppendMenu(MF_STRING, ID_APPLY_PREDIFFER, _("&Select...").c_str());
	}
	popupAll.Detach();
}

String PluginMenu::GetPluginPipelineByMenuId(unsigned idSearch, const std::vector<std::wstring>& events, unsigned baseId)
{
	PluginInfo* pluginFound = nullptr;
	String pluginName;
	const auto& allPlugins = FileTransform::CreatePluginMenuInfos(_T(""), events, baseId).second;
	for (const auto& [processType, pluginList] : allPlugins)
	{
		for (const auto& [caption, name, id, plugin] : pluginList)
		{
			if (id == idSearch)
			{
				pluginName = name;
				pluginFound = plugin;
				break;
			}
		}
	}
	if (pluginFound)
	{
		if (!pluginFound->GetExtendedPropertyValue(_T("ArgumentsRequired")).has_value() && 
		    !pluginFound->GetExtendedPropertyValue(pluginName + _T(".ArgumentsRequired")).has_value())
			return pluginName;
		CSelectPluginDlg dlg(pluginName, _T(""), 
			(baseId == ID_UNPACKERS_FIRST)  ? CSelectPluginDlg::PluginType::Unpacker    : (
			(baseId == ID_PREDIFFERS_FIRST) ? CSelectPluginDlg::PluginType::Prediffer   : 
			                                  CSelectPluginDlg::PluginType::EditorScript), true);
		if (dlg.DoModal() != IDOK)
			return {};
		return dlg.GetPluginPipeline();
	}
	return {};
}

void PluginMenu::ShowMenu(const String& filteredFilenames, const std::vector<std::wstring>& events, unsigned flags, unsigned baseId, int x, int y, CWnd* pParentWnd)
{
	CMenu menu;
	menu.CreatePopupMenu();
	AppendPluginMenus(&menu, filteredFilenames, events, flags, baseId);
	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, x, y, pParentWnd);
}
