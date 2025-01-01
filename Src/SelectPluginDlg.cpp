/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/**
 * @file  SelectPluginDlg.cpp
 *
 * @brief Unpacker plugin selection dialog implementation.
 */

#include "stdafx.h"
#include "SelectPluginDlg.h"
#include "EditPluginDlg.h"
#include "Plugins.h"
#include "FileTransform.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"
#include "unicoder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const std::vector<String> *EventNamesArray[] = {
		&FileTransform::UnpackerEventNames,
		&FileTransform::PredifferEventNames,
		&FileTransform::EditorScriptEventNames };

/////////////////////////////////////////////////////////////////////////////
// CSelectPluginDlg dialog

void CSelectPluginDlg::Initialize(PluginType pluginType)
{
	//{{AFX_DATA_INIT(CSelectPluginDlg)
	m_bNoExtensionCheck = false;
	m_bOpenInSameFrameType = false;
	m_strDescription.clear();
	m_strExtensions.clear();
	m_strArguments.clear();
	//}}AFX_DATA_INIT

	m_bNoExtensionCheck = GetOptionsMgr()->GetBool(OPT_PLUGINS_UNPACK_DONT_CHECK_EXTENSION);
	m_bOpenInSameFrameType = GetOptionsMgr()->GetBool(OPT_PLUGINS_OPEN_IN_SAME_FRAME_TYPE);

	// texts for the default unpackers
	noPlugin.reset(new PluginInfo);
	noPlugin->m_lpDispatch = nullptr;
	noPlugin->m_name = _("<None>");
	automaticPlugin.reset(new PluginInfo);
	automaticPlugin->m_lpDispatch = nullptr;
	automaticPlugin->m_name = _("<Automatic>");
	automaticPlugin->m_description = _T("The adapted unpacker is applied to both files (one file only needs the extension).");

	const std::vector<std::wstring>& events = *EventNamesArray[static_cast<int>(pluginType)];
	m_Plugins = FileTransform::CreatePluginMenuInfos(m_filteredFilenames, events, 0).second;
}

CSelectPluginDlg::CSelectPluginDlg(const String& pluginPipeline, const String& filename,
	PluginType pluginType, bool argumentsRequired/*= false  */, CWnd* pParent /*= nullptr*/)
	: CTrDialog(CSelectPluginDlg::IDD, pParent)
	, m_strPluginPipeline(pluginPipeline)
	, m_filteredFilenames(filename)
	, m_pluginType(pluginType)
	, m_bArgumentsRequired(argumentsRequired)
{
	Initialize(pluginType);
}

CSelectPluginDlg::~CSelectPluginDlg()
{
}

void CSelectPluginDlg::DoDataExchange(CDataExchange* pDX)
{
	CTrDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectPluginDlg)
	DDX_Control(pDX, IDC_PLUGIN_NAME, m_cboPluginName);
	DDX_Control(pDX, IDC_PLUGIN_TARGETS, m_cboTargetFiles);
	DDX_Check(pDX, IDC_PLUGIN_ALLOW_ALL, m_bNoExtensionCheck);
	DDX_Check(pDX, IDC_PLUGIN_OPEN_IN_SAME_FRAME_TYPE, m_bOpenInSameFrameType);
	DDX_Text(pDX, IDC_PLUGIN_DESCRIPTION, m_strDescription);
	DDX_Text(pDX, IDC_PLUGIN_SUPPORTED_EXTENSIONS, m_strExtensions);
	DDX_Text(pDX, IDC_PLUGIN_ARGUMENTS, m_strArguments);
	DDX_Control(pDX, IDC_PLUGIN_PIPELINE, m_ctlPluginPipeline);
	DDX_CBStringExact(pDX, IDC_PLUGIN_PIPELINE, m_strPluginPipeline);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectPluginDlg, CTrDialog)
	//{{AFX_MSG_MAP(CSelectPluginDlg)
	ON_BN_CLICKED(IDC_PLUGIN_ALLOW_ALL, OnUnpackerAllowAll)
	ON_CBN_SELCHANGE(IDC_PLUGIN_NAME, OnSelchangePluginName)
	ON_CBN_SELENDOK(IDC_PLUGIN_NAME, OnSelchangePluginName)
	ON_CBN_SELCHANGE(IDC_PLUGIN_TARGETS, OnSelchangeTargets)
	ON_BN_CLICKED(IDC_PLUGIN_ALIAS, OnClickedAlias)
	ON_BN_CLICKED(IDC_PLUGIN_ADDPIPE, OnClickedAddPipe)
	ON_CBN_EDITCHANGE(IDC_PLUGIN_PIPELINE, OnChangePipeline)
	ON_BN_CLICKED(IDC_PLUGIN_SETTINGS, OnClickedSettings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectPluginDlg message handlers

void CSelectPluginDlg::OnOK()
{
	UpdateData(TRUE);

	GetOptionsMgr()->SaveOption(OPT_PLUGINS_UNPACK_DONT_CHECK_EXTENSION, m_bNoExtensionCheck);
	GetOptionsMgr()->SaveOption(OPT_PLUGINS_OPEN_IN_SAME_FRAME_TYPE, m_bOpenInSameFrameType);
	m_ctlPluginPipeline.SaveState(
		std::vector<const tchar_t *>{_T("Files\\Unpacker"), _T("Files\\Prediffer"), _T("Files\\EditorScript") }
			[static_cast<int>(m_pluginType)]);

	CTrDialog::OnOK();
}

BOOL CSelectPluginDlg::OnInitDialog() 
{
	CTrDialog::OnInitDialog();

	// setup handler for resizing this dialog	
	m_constraint.InitializeCurrentSize(this);
	m_constraint.SubclassWnd(); // install subclassing
	// persist size via registry
	m_constraint.LoadPosition(_T("ResizeableDialogs"), _T("SelectPluginDlg"), false);

	prepareListbox();
	String pipeline = m_strPluginPipeline;

	m_ctlPluginPipeline.SetFileControlStates(true);
	m_ctlPluginPipeline.LoadState(
		std::vector<const tchar_t *>{_T("Files\\Unpacker"), _T("Files\\Prediffer"), _T("Files\\EditorScript") }
			[static_cast<int>(m_pluginType)]);

	EnableDlgItem(IDC_PLUGIN_OPEN_IN_SAME_FRAME_TYPE, m_pluginType == PluginType::Unpacker);

	m_strPluginPipeline = pipeline;
	UpdateData(FALSE);

	const std::array<String, 3> pluginTypes = { _("Unpacker"), _("Prediffer"), _("Editor script") };
	String pluginTypeStr = pluginTypes[static_cast<int>(m_pluginType)];

	if (m_bArgumentsRequired)
	{
		SetTitleText(_("Specify plugin arguments") + _T(" [") + pluginTypeStr + _T("]"));
		String args;
		GetDlgItemText(IDC_PLUGIN_ARGUMENTS, args);
		GetDlgItemText(IDC_PLUGIN_PIPELINE, pipeline);
		m_strPluginPipeline = pipeline + _T(" ") + args;
		SetDlgItemText(IDC_PLUGIN_PIPELINE, m_strPluginPipeline);
		SetDlgItemFocus(IDC_PLUGIN_PIPELINE);
		return FALSE;
	}
	SetTitleText(GetTitleText() + _T(" [") + pluginTypeStr + _T("]"));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectPluginDlg::prepareListbox()
{
	int sel = -1;
	PluginInfo* pSelPlugin = nullptr;
	String errorMessage;
	auto parseResult = PluginForFile::ParsePluginPipeline(m_strPluginPipeline, errorMessage);
	String lastPluginName = parseResult.empty() ? _T("") : parseResult.back().name;
	uint8_t targetFlags = parseResult.empty() ? 0xff : parseResult.back().targetFlags;
	INT_PTR nameCount = 0;

	// Target files combobox
	SetDlgItemComboBoxList(IDC_PLUGIN_TARGETS,
		{
			{ _("All"), _T("255") },
			{ _("1st"), _T("1") },
			{ _("2nd"), _T("2") },
			{ _("3rd"), _T("4") },
			{ _("1st and 2nd"), _T("3") },
			{ _("1st and 3rd"), _T("5") },
			{ _("2nd and 3rd"), _T("6") },
		}, strutils::format(_T("%d"), targetFlags)
	);

	// Plugin name combobox
	m_cboPluginName.SetRedraw(false);
	m_cboPluginName.ResetContent();

	if (m_pluginType != PluginType::EditorScript)
	{
		COMBOBOXEXITEM item{ CBEIF_TEXT };
		item.iItem = nameCount++;
		item.pszText = const_cast<tchar_t*>(noPlugin->m_name.c_str());
		m_cboPluginName.InsertItem(&item);
		item.iItem = nameCount++;
		item.pszText = const_cast<tchar_t*>(automaticPlugin->m_name.c_str());
		m_cboPluginName.InsertItem(&item);
	}
	else
	{
		COMBOBOXEXITEM item{ CBEIF_TEXT };
		item.iItem = nameCount++;
		item.pszText = const_cast<tchar_t*>(noPlugin->m_name.c_str());
		m_cboPluginName.InsertItem(&item);
	}

	std::list<String> processTypes;
	for (const auto& [processType, pluginList] : m_Plugins)
		processTypes.push_back(processType);

	auto itFound = std::find(processTypes.begin(), processTypes.end(), _("&Others"));
	if (itFound != processTypes.end())
	{
		processTypes.erase(itFound);
		processTypes.push_back(_("&Others"));
	}

	for (const auto& processType : processTypes)
	{
		const auto& pluginList = m_Plugins[processType];
		String processType2 = strutils::strip_hot_key(processType);
		if (!processType2.empty())
		{
			String text = (_T("[") + processType2 + _T("]"));
			COMBOBOXEXITEM item{ CBEIF_TEXT };
			item.iItem = nameCount++;
			item.pszText = const_cast<tchar_t*>(text.c_str());
			m_cboPluginName.InsertItem(&item);
		}
		for (const auto& [caption, name, id, plugin] : pluginList)
		{
			if (!name.empty() && name != _T("<Automatic>"))
			{
				bool match = plugin->TestAgainstRegList(m_filteredFilenames);
				if (m_bNoExtensionCheck || match || lastPluginName == name)
				{
					COMBOBOXEXITEM item{ CBEIF_TEXT | CBEIF_INDENT | CBEIF_LPARAM };
					item.iItem = nameCount++;
					item.iIndent = 1;
					item.pszText = const_cast<tchar_t*>(name.c_str());
					item.lParam = reinterpret_cast<LPARAM>(plugin);
					m_cboPluginName.InsertItem(&item);
					if (lastPluginName.empty() && match)
					{
						if (sel == -1 || (!pSelPlugin->m_bAutomatic && plugin->m_bAutomatic))
						{
							sel = m_cboPluginName.GetCount() - 1;
							pSelPlugin = plugin;
						}
					}
					else if (lastPluginName == name)
					{
						sel = m_cboPluginName.GetCount() - 1;
						pSelPlugin = plugin;
					}
				}
			}
		}
	}
	if (lastPluginName == _T("<Automatic>"))
		sel = 1;
	if (sel == -1)
	{
		sel = m_cboPluginName.FindStringExact(0, noPlugin->m_name.c_str());
		if (sel >= 0)
			m_cboPluginName.SetCurSel(sel);
	}
	else
	{
		m_cboPluginName.SetCurSel(sel);
		OnSelchangePluginName();
	}

	m_cboPluginName.SetRedraw(true);
}

void CSelectPluginDlg::OnUnpackerAllowAll() 
{
	UpdateData ();

	prepareListbox();

	UpdateData (FALSE);
}

void CSelectPluginDlg::OnClickedAlias()
{
	UpdateData();

	PluginInfo* plugin = nullptr;
	String errmsg;
	auto parseResult = PluginForFile::ParsePluginPipeline(m_strPluginPipeline, errmsg);
	if (!parseResult.empty())
	{
		for (const auto& [processType, pluginList] : m_Plugins)
		{
			for (const auto& [caption, name, id, plugin2] : m_Plugins[processType])
			{
				if (name == parseResult.front().name)
					plugin = plugin2;
			}
		}
	}
	if (!plugin)
	{
		COMBOBOXEXITEM item{ CBEIF_LPARAM };
		item.iItem = m_cboPluginName.GetCurSel();
		m_cboPluginName.GetItem(&item);
		plugin = reinterpret_cast<PluginInfo*>(item.lParam);
	}

	const tchar_t* aliasEvents[] = { _T("ALIAS_PACK_UNPACK"), _T("ALIAS_PREDIFF"), _T("ALIAS_EDITOR_SCRIPT") };
	internal_plugin::Info info = internal_plugin::CreateAliasPluginExample(plugin, aliasEvents[static_cast<int>(m_pluginType)], m_strPluginPipeline);

	for (;;)
	{
		CEditPluginDlg dlg(info);
		if (dlg.DoModal() == IDCANCEL)
			return;
		if (internal_plugin::AddPlugin(info, errmsg))
			break;
		AfxMessageBox(errmsg.c_str(), MB_OK | MB_ICONEXCLAMATION);
	}

	const std::vector<std::wstring>& events = *EventNamesArray[static_cast<int>(m_pluginType)];
	m_Plugins = FileTransform::CreatePluginMenuInfos(m_filteredFilenames, events, 0).second;

	m_strPluginPipeline = info.m_name;

	prepareListbox();
}

void CSelectPluginDlg::OnClickedAddPipe()
{
	UpdateData();
	m_strPluginPipeline += _T("|");
	UpdateData(FALSE);
}

void CSelectPluginDlg::OnChangePipeline()
{
	UpdateData(TRUE);
}

void CSelectPluginDlg::OnSelchangePluginName() 
{
	PluginInfo* pPlugin = nullptr;
	String pluginName;
	int i = m_cboPluginName.GetCurSel();
	if (i == 0)
	{
		pPlugin = noPlugin.get();
		m_strPluginPipeline.clear();
	}
	else if (m_pluginType != PluginType::EditorScript && i == 1)
	{
		pPlugin = automaticPlugin.get();
		m_strPluginPipeline = _T("<Automatic>");
	}
	else
	{
		// initialize with the default unpacker
		tchar_t szBuf[256]{};
		COMBOBOXEXITEM item{CBEIF_TEXT};
		item.iItem = i;
		item.pszText = szBuf;
		item.cchTextMax = sizeof(szBuf) / sizeof(szBuf[0]);
		m_cboPluginName.GetItem(&item);
		CString cstrPluginName = item.pszText;
		pluginName = cstrPluginName.Trim();

		uint8_t targetFlags = 
			static_cast<uint8_t>(tc::ttoi(
			reinterpret_cast<const tchar_t*>(m_cboTargetFiles.GetItemDataPtr(m_cboTargetFiles.GetCurSel()))));

		for (const auto& [processType, pluginList] : m_Plugins)
		{
			for (const auto& [caption, name, id, plugin] : pluginList)
			{
				if (pluginName == name)
				{
					String pluginPipeline = strutils::trim_ws(m_strPluginPipeline);
					if (!pluginPipeline.empty() && pluginPipeline.back() == '|')
						pluginPipeline += _T("dummy");
					String errorMessage;
					auto parseResult = PluginForFile::ParsePluginPipeline(pluginPipeline, errorMessage);
					if (parseResult.empty())
						parseResult.push_back({ name, targetFlags, {}, '\0' });
					parseResult.back().name = name;
					parseResult.back().targetFlags = targetFlags;
					m_strPluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
					pPlugin = plugin;
					break;
				}
			}
		}
	}

	if (pPlugin)
	{
		const bool containsNonAsciiChars = std::any_of(pPlugin->m_description.begin(), pPlugin->m_description.end(), [](auto c) { return (c >= 0x80); });
		m_strDescription = containsNonAsciiChars ? pPlugin->m_description : tr(ucr::toUTF8(pPlugin->m_description));
		auto funcDescription = pPlugin->GetExtendedPropertyValue(pluginName + _T(".Description"));
		if (funcDescription.has_value())
			m_strDescription = tr(ucr::toUTF8(strutils::to_str(*funcDescription)));
		m_strExtensions = pPlugin->m_filtersText;
		m_strArguments = pPlugin->m_arguments;
		auto funcArguments = pPlugin->GetExtendedPropertyValue(pluginName + _T(".Arguments"));
		if (funcArguments.has_value())
			m_strArguments = strutils::to_str(*funcArguments);
	}

	m_bOpenInSameFrameType = IsDlgButtonChecked(IDC_PLUGIN_OPEN_IN_SAME_FRAME_TYPE);

	UpdateData (FALSE);
}

void CSelectPluginDlg::OnSelchangeTargets()
{
	OnSelchangePluginName();
}

void CSelectPluginDlg::OnClickedSettings() 
{
	COMBOBOXEXITEM item{CBEIF_LPARAM};
	item.iItem = m_cboPluginName.GetCurSel();
	m_cboPluginName.GetItem(&item);
	auto* plugin = reinterpret_cast<PluginInfo*>(item.lParam);
	if (plugin)
	{
		if (plugin->m_event.find(L"ALIAS_") == 0)
		{
			String errorMessage;
			auto parseResult = PluginForFile::ParsePluginPipeline(plugin->m_pipeline, errorMessage);
			for (const auto& [processType, pluginList] : m_Plugins)
			{
				for (const auto& [caption, name, id, plugin2] : m_Plugins[processType])
				{
					if (name == parseResult.front().name)
					{
						plugin = plugin2;
						break;
					}
				}
			}
		}
		plugin::InvokeShowSettingsDialog(plugin->m_lpDispatch);
	}
}
