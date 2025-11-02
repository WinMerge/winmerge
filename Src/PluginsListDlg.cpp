/**
 * @file  PluginsListDlg.cpp
 *
 * @brief Implementation file for PluginsList dialog
 */

#include "stdafx.h"
#include "PluginsListDlg.h"
#include "EditPluginDlg.h"
#include "WildcardDropList.h"
#include "UnicodeString.h"
#include "Plugins.h"
#include "InternalPlugins.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "unicoder.h"
#include "Merge.h"
#include "Constants.h"
#include "Win_VersionHelper.h"
#include "DarkModeLib.h"

#ifndef BCN_DROPDOWN
#define BCN_DROPDOWN            (BCN_FIRST + 0x0002)
#endif

IMPLEMENT_DYNAMIC(PluginsListDlg, CTrDialog)

BEGIN_MESSAGE_MAP(PluginsListDlg, CTrDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_PLUGIN_ADD, OnBnClickedPluginAdd)
	ON_NOTIFY(BCN_DROPDOWN, IDC_PLUGIN_ADD, OnDropDownAdd)
	ON_BN_CLICKED(IDC_PLUGIN_EDIT, OnBnClickedPluginEdit)
	ON_BN_CLICKED(IDC_PLUGIN_REMOVE, OnBnClickedPluginRemove)
	ON_BN_CLICKED(IDC_PLUGIN_SETTINGS, OnBnClickedPluginSettings)
	ON_BN_CLICKED(IDC_PLUGIN_DEFAULTS, OnBnClickedFileFiltesDefaults)
	ON_CBN_DROPDOWN(IDC_PLUGIN_FILEFILTERS, OnDropDownPatterns)
	ON_CBN_CLOSEUP(IDC_PLUGIN_FILEFILTERS, OnCloseUpPatterns)
	ON_CBN_SELCHANGE(IDC_PLUGIN_TYPE, OnSelchangePluginType)
	ON_NOTIFY(NM_DBLCLK, IDC_PLUGINSLIST_LIST, OnNMDblclkList)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_PLUGINSLIST_LIST, OnLVNItemChanging)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PLUGINSLIST_LIST, OnLVNItemChanged)
	ON_COMMAND(ID_HELP, OnHelp)
END_MESSAGE_MAP()

/**
 * @brief Constructor.
 */
PluginsListDlg::PluginsListDlg(CWnd* pParent /*= nullptr*/)
	: CTrDialog(PluginsListDlg::IDD, pParent)
{
}

/**
 * @brief Destructor.
 */
PluginsListDlg::~PluginsListDlg()
{
}

void PluginsListDlg::DoDataExchange(CDataExchange* pDX)
{
	CTrDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PLUGIN_TYPE, m_comboType);
	DDX_Control(pDX, IDC_PLUGINSLIST_LIST, m_list);
	DDX_Control(pDX, IDC_PLUGIN_FILEFILTERS, m_comboPatterns);
}

/**
 * @brief Initialize the dialog.
 */
BOOL PluginsListDlg::OnInitDialog()
{
	CTrDialog::OnInitDialog();

	if (!IsVista_OrGreater())
	{
		// fallback for XP 
		SendDlgItemMessage(IDC_PLUGIN_ADD, BM_SETSTYLE, BS_PUSHBUTTON, TRUE);
	}

	
	InitList();
	SetDlgItemComboBoxList(IDC_PLUGIN_TYPE, { _("Unpacker"), _("Prediffer"), _("Editor script") });
	m_comboType.SetCurSel(0);
	SetPlugins(0);
	m_list.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);

	CheckDlgButton(IDC_PLUGINS_ENABLE, 
		GetOptionsMgr()->GetBool(OPT_PLUGINS_ENABLED) ? BST_CHECKED : BST_UNCHECKED);

	m_constraint.InitializeCurrentSize(this);
	m_constraint.SubclassWnd(); // install subclassing
	// persist size via registry
	m_constraint.LoadPosition(_T("ResizeableDialogs"), _T("PluginsListDlg"), false);

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief Set-up the list control.
 */
void PluginsListDlg::InitList()
{
	// Show selection across entire row.
	// Also enable infotips.
	m_list.SetExtendedStyle(LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);

	HWND hList = m_list.GetSafeHwnd();
	if (hList != nullptr)
		DarkMode::setDarkListViewCheckboxes(hList);

	const int lpx = CClientDC(this).GetDeviceCaps(LOGPIXELSX);
	auto pointToPixel = [lpx](int point) { return MulDiv(point, lpx, 72); };

	String title = _("Name");
	m_list.InsertColumn(0, title.c_str(), LVCFMT_LEFT, pointToPixel(150));
	title = _("Type");
	m_list.InsertColumn(1, title.c_str(), LVCFMT_LEFT, pointToPixel(100));
	title = _("Description");
	m_list.InsertColumn(2, title.c_str(), LVCFMT_LEFT, pointToPixel(600));
}

/**
 * @brief Add found plugins to the list.
 */
void PluginsListDlg::SetPlugins(int sel)
{
	m_list.DeleteAllItems();
	if (sel == 0)
	{
		AddPluginsToList(L"URL_PACK_UNPACK");
		for (const auto& event : plugin::UnpackerEventNames)
			AddPluginsToList(event.c_str());
	}
	else if (sel == 1)
	{
		for (const auto& event : plugin::PredifferEventNames)
			AddPluginsToList(event.c_str());
	}
	else if (sel == 2)
	{
		for (const auto& event : plugin::EditorScriptEventNames)
			AddPluginsToList(event.c_str() );
	}
}

/**
 * @brief Add plugins of given event type to the list.
 * @param [in] pluginEvent Event type for plugins to add.
 */
void PluginsListDlg::AddPluginsToList(const wchar_t *pluginEvent)
{
	PluginArray * piPluginArray = 
		CAllThreadsScripts::GetActiveSet()->GetAvailableScripts(pluginEvent);

	m_list.SetRedraw(false);
	for (size_t iPlugin = 0 ; iPlugin < piPluginArray->size() ; iPlugin++)
	{
		const PluginInfoPtr& plugin = piPluginArray->at(iPlugin);
		auto processType = plugin->GetExtendedPropertyValue(_T("ProcessType"));
		String processType2 = processType.has_value() ? strutils::to_str(*processType) : _T("&Others");
		processType2 = strutils::strip_hot_key(I18n::tr(ucr::toUTF8(processType2)));
		int ind = m_list.InsertItem(m_list.GetItemCount(), plugin->m_name.c_str());
		const bool containsNonAsciiChars  = std::any_of(plugin->m_description.begin(), plugin->m_description.end(), [](auto c) { return (c >= 0x80); });
		String desc = containsNonAsciiChars  ? plugin->m_description : I18n::tr(ucr::toUTF8(plugin->m_description));
		strutils::replace(desc, _T("\r"), _T(""));
		strutils::replace(desc, _T("\n"), _T(" "));
		m_list.SetItemText(ind, 1, processType2.c_str());
		m_list.SetItemText(ind, 2, desc.c_str());
		m_list.SetCheck(ind, !plugin->m_disabled);
		m_list.SetItemData(ind, reinterpret_cast<DWORD_PTR>(plugin.get()));
	}
	m_list.SetRedraw(true);
}

PluginInfo *PluginsListDlg::GetSelectedPluginInfo() const
{
	int ind = m_list.GetNextItem(-1, LVNI_SELECTED);
	if (ind < 0)
		return nullptr;
	return reinterpret_cast<PluginInfo *>(m_list.GetItemData(ind));
}

internal_plugin::Info* PluginsListDlg::GetSelectedInternalPluginInfo() const
{
	PluginInfo* plugin = GetSelectedPluginInfo();
	if (!plugin)
		return nullptr;
	return internal_plugin::GetInternalPluginInfo(plugin);
}

void PluginsListDlg::RefreshList()
{
	CRect rect;
	const int topIndex = m_list.GetTopIndex();
	m_list.GetItemRect(topIndex, &rect, LVIR_BOUNDS);
	const int itemheight = rect.bottom - rect.top;
	int index = -1;
	auto pos = m_list.GetFirstSelectedItemPosition();
	if (pos)
		index = m_list.GetNextSelectedItem(pos);
	SetPlugins(m_comboType.GetCurSel());
	if (index >= m_list.GetItemCount())
		index = m_list.GetItemCount() - 1;
	if (index > 0)
	{
		m_list.SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);
		m_list.Scroll(CSize{ 0, itemheight * topIndex });
	}
}

void PluginsListDlg::AddPlugin(unsigned id)
{
	auto info = std::make_unique<internal_plugin::Info>(
		(id == ID_PLUGIN_ADD_UNPACKER) ?
		internal_plugin::CreateUnpackerPluginExample() : internal_plugin::CreatePredifferPluginExample());
	for (;;)
	{
		CEditPluginDlg dlg(*info);
		if (dlg.DoModal() == IDCANCEL || !info->m_userDefined)
			return;
		String errmsg;
		if (internal_plugin::AddPlugin(*info, errmsg))
		{
			m_comboType.SetCurSel(id == ID_PLUGIN_ADD_UNPACKER ? 0 : 1);
			break;
		}
		AfxMessageBox(errmsg.c_str(), MB_OK | MB_ICONEXCLAMATION);
	}
	RefreshList();
}

void PluginsListDlg::EditPlugin()
{
	auto* info = GetSelectedInternalPluginInfo();
	if (!info)
		return;
	String nameOrg = info->m_name;
	for (;;)
	{
		CEditPluginDlg dlg(*info);
		if (dlg.DoModal() == IDCANCEL || !info->m_userDefined)
			return;
		String errmsg;
		if (info->m_name == nameOrg)
		{
			if (internal_plugin::UpdatePlugin(*info, errmsg))
				break;
		}
		else
		{
			if (internal_plugin::AddPlugin(*info, errmsg))
			{
				internal_plugin::Info infoOld(nameOrg);
				infoOld.m_userDefined = true;
				internal_plugin::RemovePlugin(infoOld, errmsg);
				break;
			}
		}
		AfxMessageBox(errmsg.c_str(), MB_OK | MB_ICONEXCLAMATION);
	}
	RefreshList();
}

void PluginsListDlg::DuplicatePlugin()
{
	auto* info = GetSelectedInternalPluginInfo();
	if (!info)
		return;
	internal_plugin::Info info2(*info);
	info2.m_userDefined = true;
	for (;;)
	{
		info2.m_name += _T("Copy");
		if (!internal_plugin::FindPluginNameConflict(info2))
			break;
	}
	for (;;)
	{
		CEditPluginDlg dlg(info2);
		if (dlg.DoModal() == IDCANCEL || !info2.m_userDefined)
			return;
		String errmsg;
		if (internal_plugin::AddPlugin(info2, errmsg))
			break;
		AfxMessageBox(errmsg.c_str(), MB_OK | MB_ICONEXCLAMATION);
	}
	RefreshList();
}

void PluginsListDlg::RemovePlugin()
{
	auto* info = GetSelectedInternalPluginInfo();
	if (!info)
		return;
	String errmsg;
	if (!internal_plugin::RemovePlugin(*info, errmsg))
	{
		AfxMessageBox(errmsg.c_str(), MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	RefreshList();
}
/**
 * @brief Save plugins enabled setting when closing the dialog.
 */
void PluginsListDlg::OnBnClickedOk()
{
	GetOptionsMgr()->SaveOption(OPT_PLUGINS_ENABLED, 
		(IsDlgButtonChecked(IDC_PLUGINS_ENABLE) == 1));

	OnLVNItemChanging(nullptr, nullptr);

	for (int i = 0; i < m_list.GetItemCount(); ++i)
	{
		PluginInfo * plugin = reinterpret_cast<PluginInfo *>(m_list.GetItemData(i));
		if (plugin)
			plugin->m_disabled = !m_list.GetCheck(i);
	}

	CAllThreadsScripts::GetActiveSet()->SaveSettings();
	CAllThreadsScripts::ReloadCustomSettings();
	OnOK();
}

void PluginsListDlg::OnBnClickedPluginAdd()
{
	AddPlugin(ID_PLUGIN_ADD_UNPACKER);
}

void PluginsListDlg::OnDropDownAdd(NMHDR *pNMHDR, LRESULT *pResult)
{
	CRect rcButton;
	GetDlgItem(IDC_PLUGIN_ADD)->GetWindowRect(&rcButton);
	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_POPUP_PLUGIN_ADD_MENU));
	I18n::TranslateMenu(menu.m_hMenu);
	CMenu* pPopup = menu.GetSubMenu(0);
	auto* info = GetSelectedInternalPluginInfo();
	if (!info)
		pPopup->EnableMenuItem(ID_PLUGIN_DUPLICATE, MF_GRAYED);
	int command = pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
			rcButton.left, rcButton.bottom, this);
	switch (command)
	{
	case ID_PLUGIN_ADD_UNPACKER:
	case ID_PLUGIN_ADD_PREDIFFER:
		AddPlugin(command);
		break;
	case ID_PLUGIN_DUPLICATE:
		DuplicatePlugin();
		break;
	default:
		break;
	}
	*pResult = 0;
}

void PluginsListDlg::OnBnClickedPluginEdit()
{
	EditPlugin();
}

void PluginsListDlg::OnBnClickedPluginRemove()
{
	RemovePlugin();
}

void PluginsListDlg::OnBnClickedPluginSettings()
{
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	if (pos != nullptr)
	{
		String pluginName = static_cast<const tchar_t *>(m_list.GetItemText(m_list.GetNextSelectedItem(pos), 0));
		for (int i = 0; TransformationCategories[i] != nullptr; ++i)
		{
			PluginInfo * plugin = CAllThreadsScripts::GetActiveSet()->GetPluginByName(TransformationCategories[i], pluginName);
			if (plugin != nullptr)
			{
				EnableWindow(false);
				plugin::InvokeShowSettingsDialog(plugin->m_lpDispatch);
				EnableWindow(true);
				SetForegroundWindow();
				break;
			}
		}
	}
}

void PluginsListDlg::OnBnClickedFileFiltesDefaults()
{
	PluginInfo *plugin = GetSelectedPluginInfo();
	if (plugin)
	{
		SetDlgItemText(IDC_PLUGIN_FILEFILTERS, plugin->m_filtersTextDefault);
		SetDlgItemText(IDC_PLUGIN_ARGUMENTS, plugin->m_argumentsDefault);
		CheckDlgButton(IDC_PLUGIN_AUTOMATIC, plugin->m_bAutomaticDefault);
	}
}

void PluginsListDlg::OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnBnClickedPluginSettings();
}

void PluginsListDlg::OnLVNItemChanging(NMHDR *pNMHDR, LRESULT *pResult)
{
	PluginInfo *plugin = GetSelectedPluginInfo();
	if (plugin)
	{
		GetDlgItemText(IDC_PLUGIN_FILEFILTERS, plugin->m_filtersText);
		WildcardRemoveDuplicatePatterns(plugin->m_filtersText);
		plugin->LoadFilterString();
		GetDlgItemText(IDC_PLUGIN_ARGUMENTS, plugin->m_arguments);
		plugin->m_bAutomatic = !!IsDlgButtonChecked(IDC_PLUGIN_AUTOMATIC);
	}
}

void PluginsListDlg::OnLVNItemChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	PluginInfo *plugin = GetSelectedPluginInfo();
	if (plugin)
	{
		SetDlgItemText(IDC_PLUGIN_FILEFILTERS, plugin->m_filtersText);
		SetDlgItemText(IDC_PLUGIN_ARGUMENTS, plugin->m_arguments);
		CheckDlgButton(IDC_PLUGIN_AUTOMATIC, plugin->m_bAutomatic);
		auto* info = internal_plugin::GetInternalPluginInfo(plugin);
		EnableDlgItem(IDC_PLUGIN_EDIT, info != nullptr);
		EnableDlgItem(IDC_PLUGIN_REMOVE, (info && info->m_userDefined));
	}
}

/**
 * @brief Prepares multi-selection drop list 
 */
void PluginsListDlg::OnSelchangePluginType()
{
	SetPlugins(m_comboType.GetCurSel());
}

/**
 * @brief Prepares multi-selection drop list 
 */
void PluginsListDlg::OnDropDownPatterns()
{
	PluginInfo *plugin = GetSelectedPluginInfo();
	if (plugin)
	{
		String patterns = plugin->m_filtersTextDefault;
		WildcardDropList::OnDropDown(m_comboPatterns, 3, patterns.c_str());
	}
}

/**
 * @brief Finishes drop list multi-selection
 */
void PluginsListDlg::OnCloseUpPatterns()
{
	WildcardDropList::OnCloseUp(m_comboPatterns);
}

/**
 * @brief Open help from mainframe when user presses F1.
 */
void PluginsListDlg::OnHelp()
{
	CMergeApp::ShowHelp(PluginsHelpLocation);
}

