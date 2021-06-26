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

void CSelectPluginDlg::Initialize(bool unpacker)
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

	std::vector<std::wstring> events = unpacker ?  FileTransform::UnpackerEventNames : FileTransform::PredifferEventNames;
	m_Plugins = FileTransform::CreatePluginMenuInfos(m_filteredFilenames, events, 0).second;
}


CSelectPluginDlg::CSelectPluginDlg(const String& pluginPipeline, const String& filename,
	bool unpacker /*= true */, bool argumentRequired/*= false  */, CWnd* pParent /*= nullptr*/)
	: CTrDialog(CSelectPluginDlg::IDD, pParent)
	, m_strPluginPipeline(pluginPipeline)
	, m_filteredFilenames(filename)
	, m_bUnpacker(unpacker)
	, m_bArgumentRequired(argumentRequired)
{
	Initialize(unpacker);
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
	m_ctlPluginPipeline.SaveState(m_bUnpacker ? _T("Files\\Unpacker") : _T("Files\\Prediffer"));

	CTrDialog::OnOK();
}

BOOL CSelectPluginDlg::OnInitDialog() 
{
	CTrDialog::OnInitDialog();

	m_bNoExtensionCheck = GetOptionsMgr()->GetBool(OPT_PLUGINS_UNPACK_DONT_CHECK_EXTENSION);
	m_bOpenInSameFrameType = GetOptionsMgr()->GetBool(OPT_PLUGINS_OPEN_IN_SAME_FRAME_TYPE);

	prepareListbox();
	m_ctlPluginPipeline.SetFileControlStates(true);
	m_ctlPluginPipeline.LoadState(m_bUnpacker ? _T("Files\\Unpacker") : _T("Files\\Prediffer"));

	EnableDlgItem(IDC_PLUGIN_OPEN_IN_SAME_FRAME_TYPE, m_bUnpacker);

	UpdateData(FALSE);

	if (m_bArgumentRequired)
	{
		SetWindowText(_("Specify plugin arguments").c_str());
		String args;
		CString pipeline;
		GetDlgItemText(IDC_PLUGIN_ARGUMENTS, args);
		m_ctlPluginPipeline.GetWindowText(pipeline);
		m_strPluginPipeline = pipeline + _T(" ") + args.c_str();
		m_ctlPluginPipeline.SetWindowText(m_strPluginPipeline.c_str());
		m_ctlPluginPipeline.SetFocus();
		return FALSE;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectPluginDlg::prepareListbox() 
{
	int sel = -1;
	int i = 0;
	String errorMessage;
	auto parseResult = PluginForFile::ParsePluginPipeline(m_strPluginPipeline, errorMessage);
	String lastPluginName = parseResult.empty() ? _T("") : parseResult.back().name;
	m_cboPluginName.AddString(noPlugin->m_name.c_str());
	m_cboPluginName.AddString(automaticPlugin->m_name.c_str());

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
				if (m_bNoExtensionCheck || plugin->TestAgainstRegList(m_filteredFilenames) || lastPluginName == name)
				{
					m_cboPluginName.AddString(name.c_str());
					if (lastPluginName == name)
						sel = m_cboPluginName.GetCount() - 1;
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
	int i = m_cboPluginName.GetCurSel();
	if (i == 0)
	{
		pPlugin = noPlugin.get();
		m_strPluginPipeline.clear();
	}
	else if (i == 1)
	{
		pPlugin = automaticPlugin.get();
		m_strPluginPipeline = _T("<Automatic>");
	}
	else
	{
		// initialize with the default unpacker
		CString cstrPluginName;
		m_cboPluginName.GetWindowText(cstrPluginName);
		String pluginName = cstrPluginName.Trim();
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
		m_strExtensions = pPlugin->m_filtersText;
		m_strArguments = pPlugin->m_arguments;
	}

	m_bOpenInSameFrameType = IsDlgButtonChecked(IDC_PLUGIN_OPEN_IN_SAME_FRAME_TYPE);

	UpdateData (FALSE);
}
