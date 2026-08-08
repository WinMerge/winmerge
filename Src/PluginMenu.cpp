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

static int GetPluginPipelineOrder(const std::vector<PluginForFile::PipelineItem>& plugins, const String& pluginName)
{
	for (size_t i = 0; i < plugins.size(); ++i)
	{
		if (plugins[i].name == pluginName)
			return static_cast<int>(i);
	}
	return -1;
}

static void SetMenuItemData(CMenu* pMenu, unsigned id, int order)
{
	MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
	mii.fMask = MIIM_DATA;
	mii.dwItemData = order;
	pMenu->SetMenuItemInfo(id, &mii, FALSE);
}

static int GetMenuItemData(CMenu* pMenu, unsigned id)
{
	MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
	mii.fMask = MIIM_DATA;
	pMenu->GetMenuItemInfo(id, &mii, FALSE);
	return static_cast<int>(mii.dwItemData);
}

static void AddMenuItem(CMenu* pMenu, const std::vector<PluginForFile::PipelineItem>& plugins, const String& name, const String& caption, unsigned id)
{
	const int order = GetPluginPipelineOrder(plugins, name);
	const String caption2 = order >= 0 ? caption + _T(" (") + std::to_wstring(order + 1) + _T(")") : caption;
	pMenu->AppendMenu(MF_STRING, id, caption2.c_str());
	if (order >= 0)
		SetMenuItemData(pMenu, id, order + 1);
}

void PluginMenu::AppendPluginMenus(CMenu *pMenu, const PluginForFile* pluginInfo, const String& filteredFilenames,
	const std::vector<std::wstring>& events, unsigned flags, unsigned baseId)
{
	if (!GetOptionsMgr()->GetBool(OPT_PLUGINS_ENABLED))
		return;

	CWaitCursor waitstatus;

	String errorMessage;
	std::vector<PluginForFile::PipelineItem> plugins;
	if (pluginInfo)
		plugins = pluginInfo->ParsePluginPipeline(errorMessage);

	auto [suggestedPlugins, allPlugins] = FileTransform::CreatePluginMenuInfos(filteredFilenames, events, baseId);

	if ((flags & MenuFlags::AddAllMenu) == 0)
		pMenu->AppendMenu(MF_STRING, ID_SUGGESTED_PLUGINS, _("Suggested Plugins").c_str());
	else
		pMenu->AppendMenu(MF_SEPARATOR);

	for (const auto& [caption, name, id, plugin] : suggestedPlugins)
		AddMenuItem(pMenu, plugins, name, caption, id);

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
				AddMenuItem(pMenu2, plugins, name, caption, id);
		}
		else
		{
			if (processType.empty())
			{
				for (const auto& [caption, name, id, plugin] : allPlugins[processType])
					AddMenuItem(pMenu2, plugins, name, caption, id);
			}
			else
			{
				CMenu popup;
				popup.CreatePopupMenu();
				for (const auto& [caption, name, id, plugin] : allPlugins[processType])
					AddMenuItem(&popup, plugins, name, caption, id);
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
	if (pluginInfo)
		pMenu->AppendMenu(MF_STRING | MF_GRAYED, 0, _("(Ctrl+Click to add to pipeline)").c_str());
	popupAll.Detach();
}

String PluginMenu::GetPluginPipelineByMenuId(const PluginForFile* pluginInfo, unsigned idSearch, const std::vector<std::wstring>& events, unsigned baseId)
{
	bool bCtrlKey = (::GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
	PluginInfo* pluginFound = nullptr;
	String pluginName;
	[[maybe_unused]] auto [_, allPlugins] = FileTransform::CreatePluginMenuInfos(_T(""), events, baseId);
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
		String pipeline = pluginInfo ? pluginInfo->GetPluginPipeline() : _T("");
		if (bCtrlKey)
			pipeline = pipeline.empty() ? pluginName : (pipeline + _T("|") + pluginName);
		else
			pipeline = pluginName;
		if (!pluginFound->GetExtendedPropertyValue(_T("ArgumentsRequired")).has_value() &&
			!pluginFound->GetExtendedPropertyValue(pluginName + _T(".ArgumentsRequired")).has_value())
			return pipeline;
		CSelectPluginDlg dlg(pipeline, _T(""), 
			(baseId == ID_UNPACKERS_FIRST)  ? CSelectPluginDlg::PluginType::Unpacker    : (
			(baseId == ID_PREDIFFERS_FIRST) ? CSelectPluginDlg::PluginType::Prediffer   : 
			                                  CSelectPluginDlg::PluginType::EditorScript), true);
		if (dlg.DoModal() != IDOK)
			return {};
		return dlg.GetPluginPipeline();
	}
	return {};
}

void PluginMenu::ShowMenu(const PluginForFile* pluginInfo, const String& filteredFilenames, const std::vector<std::wstring>& events, unsigned flags, unsigned baseId, int x, int y, CWnd* pParentWnd)
{
	CMenu menu;
	menu.CreatePopupMenu();
	AppendPluginMenus(&menu, pluginInfo, filteredFilenames, events, flags, baseId);
	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, x, y, pParentWnd);
}

void PluginMenu::UpdateMenu(CCmdUI* pCmdUI)
{
	if (!pCmdUI || !pCmdUI->m_pMenu)
		return;
	pCmdUI->SetCheck(GetMenuItemData(pCmdUI->m_pMenu, pCmdUI->m_nID) > 0);
}
