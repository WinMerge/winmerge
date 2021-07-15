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
#include "Plugins.h"
#include "FileTransform.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"
#include "unicoder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectPluginDlg dialog

void CSelectPluginDlg::Initialize(PluginType pluginType)
{
	//{{AFX_DATA_INIT(CSelectPluginDlg)
	m_bNoExtensionCheck = false;
	m_bOpenInSameFrameType = false;
	m_strDescription = _T("");
	m_strExtensions = _T("");
	m_strArguments = _T("");
	//}}AFX_DATA_INIT

	// texts for the default unpackers
	noPlugin.reset(new PluginInfo);
	noPlugin->m_lpDispatch = nullptr;
	noPlugin->m_name = _("<None>");
	automaticPlugin.reset(new PluginInfo);
	automaticPlugin->m_lpDispatch = nullptr;
	automaticPlugin->m_name = _("<Automatic>");
	automaticPlugin->m_description = _T("The adapted unpacker is applied to both files (one file only needs the extension).");

	const std::vector<String> *eventNamesArray[] = {
			&FileTransform::UnpackerEventNames,
			&FileTransform::PredifferEventNames,
			&FileTransform::EditorScriptEventNames };
	const std::vector<std::wstring>& events = *eventNamesArray[static_cast<int>(pluginType)];
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
	ON_CBN_SELCHANGE(IDC_PLUGIN_NAME, OnSelchangeUnpackerName)
	ON_CBN_SELENDOK(IDC_PLUGIN_NAME, OnSelchangeUnpackerName)
	ON_BN_CLICKED(IDC_PLUGIN_ADDPIPE, OnClickedAddPipe)
	ON_EN_CHANGE(IDC_PLUGIN_PIPELINE, OnChangePipeline)
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
		std::vector<const TCHAR *>{_T("Files\\Unpacker"), _T("Files\\Prediffer"), _T("Files\\EditorScript") }
			[static_cast<int>(m_pluginType)]);

	CTrDialog::OnOK();
}

BOOL CSelectPluginDlg::OnInitDialog() 
{
	CTrDialog::OnInitDialog();

	m_bNoExtensionCheck = GetOptionsMgr()->GetBool(OPT_PLUGINS_UNPACK_DONT_CHECK_EXTENSION);
	m_bOpenInSameFrameType = GetOptionsMgr()->GetBool(OPT_PLUGINS_OPEN_IN_SAME_FRAME_TYPE);

	prepareListbox();
	m_ctlPluginPipeline.SetFileControlStates(true);
	m_ctlPluginPipeline.LoadState(
		std::vector<const TCHAR *>{_T("Files\\Unpacker"), _T("Files\\Prediffer"), _T("Files\\EditorScript") }
			[static_cast<int>(m_pluginType)]);

	EnableDlgItem(IDC_PLUGIN_OPEN_IN_SAME_FRAME_TYPE, m_pluginType == PluginType::Unpacker);

	UpdateData(FALSE);

	const std::vector<String> pluginTypes = { _("Unpacker"), _("Prediffer"), _("Editor script") };
	String pluginTypeStr = pluginTypes[static_cast<int>(m_pluginType)];

	if (m_bArgumentsRequired)
	{
		SetWindowText((_("Specify plugin arguments") + _T(" [") + pluginTypeStr + _T("]")).c_str());
		String args;
		CString pipeline;
		GetDlgItemText(IDC_PLUGIN_ARGUMENTS, args);
		m_ctlPluginPipeline.GetWindowText(pipeline);
		m_strPluginPipeline = pipeline + _T(" ") + args.c_str();
		m_ctlPluginPipeline.SetWindowText(m_strPluginPipeline.c_str());
		m_ctlPluginPipeline.SetFocus();
		return FALSE;
	}
	CString title;
	GetWindowText(title);
	SetWindowText(title + _T(" [") + pluginTypeStr.c_str() + _T("]"));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectPluginDlg::prepareListbox() 
{
	int sel = -1;
	PluginInfo* pSelPlugin = nullptr;
	int i = 0;
	String errorMessage;
	auto parseResult = PluginForFile::ParsePluginPipeline(m_strPluginPipeline, errorMessage);
	String lastPluginName = parseResult.empty() ? _T("") : parseResult.back().name;

	if (m_pluginType != PluginType::EditorScript)
	{
		m_cboPluginName.AddString(noPlugin->m_name.c_str());
		m_cboPluginName.AddString(automaticPlugin->m_name.c_str());
	}

	std::vector<String> processTypes;
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
		String processType2 = processType;
		auto it = processType2.find(_("(&"));
		if (it != String::npos)
			processType2.erase(it, it + 2);
		strutils::replace(processType2, _T("&"), _T(""));
		if (!processType2.empty())
			m_cboPluginName.AddString((_T("[") + processType2 + _T("]")).c_str());
		for (const auto& [caption, name, id, plugin] : pluginList)
		{
			if (!name.empty() && name != _T("<Automatic>"))
			{
				bool match = plugin->TestAgainstRegList(m_filteredFilenames);
				if (m_bNoExtensionCheck || match || lastPluginName == name)
				{
					m_cboPluginName.AddString(name.c_str());
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
		m_cboPluginName.SelectString(-1, noPlugin->m_name.c_str());
	}
	else
	{
		m_cboPluginName.SetCurSel(sel);
		OnSelchangeUnpackerName();
	}
}

void CSelectPluginDlg::OnUnpackerAllowAll() 
{
	UpdateData ();

	m_cboPluginName.ResetContent();

	prepareListbox();

	UpdateData (FALSE);
}

void CSelectPluginDlg::OnClickedAddPipe()
{
	m_strPluginPipeline += _T("|");
	UpdateData(FALSE);
}

void CSelectPluginDlg::OnChangePipeline()
{
	UpdateData(TRUE);
}

void CSelectPluginDlg::OnSelchangeUnpackerName() 
{
	PluginInfo* pPlugin = nullptr;
	String pluginName;
	int i = m_cboPluginName.GetCurSel();
	if (m_pluginType != PluginType::EditorScript && i == 0)
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
		CString cstrPluginName;
		m_cboPluginName.GetWindowText(cstrPluginName);
		pluginName = cstrPluginName.Trim();
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
						parseResult.push_back({ name, {}, '\0' });
					parseResult.back().name = name;
					m_strPluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
					pPlugin = plugin;
					break;
				}
			}
		}
	}

	if (pPlugin)
	{
		m_strDescription = tr(ucr::toUTF8(pPlugin->m_description));
		auto funcDescription = pPlugin->GetExtendedPropertyValue(pluginName + _T(".Description"));
		if (funcDescription.has_value())
			m_strDescription = tr(ucr::toUTF8({ funcDescription.value().data(), funcDescription.value().length() }));
		m_strExtensions = pPlugin->m_filtersText;
		m_strArguments = pPlugin->m_arguments;
		auto funcArguments = pPlugin->GetExtendedPropertyValue(pluginName + _T(".Arguments"));
		if (funcArguments.has_value())
			m_strArguments = { funcArguments.value().data(), funcArguments.value().length() };
	}

	m_bOpenInSameFrameType = IsDlgButtonChecked(IDC_PLUGIN_OPEN_IN_SAME_FRAME_TYPE);

	UpdateData (FALSE);
}
