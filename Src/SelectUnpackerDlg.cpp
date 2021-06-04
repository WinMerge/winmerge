/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/**
 * @file  SelectUnpackerDlg.cpp
 *
 * @brief Unpacker plugin selection dialog implementation.
 */

#include "stdafx.h"
#include "SelectUnpackerDlg.h"
#include "Plugins.h"
#include "FileTransform.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



/////////////////////////////////////////////////////////////////////////////
// CSelectUnpackerDlg dialog

void CSelectUnpackerDlg::Initialize()
{
	//{{AFX_DATA_INIT(CSelectUnpackerDlg)
	m_bNoExtensionCheck = false;
	m_strDescription = _T("");
	m_strExtensions = _T("");
	//}}AFX_DATA_INIT

	// texts for the default unpackers
	noPlugin.reset(new PluginInfo);
	noPlugin->m_lpDispatch = nullptr;
	noPlugin->m_name = _("<None>");
	automaticPlugin.reset(new PluginInfo);
	automaticPlugin->m_lpDispatch = nullptr;
	automaticPlugin->m_name = _("<Automatic>");
	automaticPlugin->m_description = _("The adapted unpacker is applied to both files (one file only needs the extension).");

	m_pPlugins.clear();

	PluginArray * piFileScriptArray = 
		CAllThreadsScripts::GetActiveSet()->GetAvailableScripts(L"FILE_PACK_UNPACK");
	PluginArray * piBufferScriptArray = 
		CAllThreadsScripts::GetActiveSet()->GetAvailableScripts(L"BUFFER_PACK_UNPACK");
	PluginArray * piFileFolderScriptArray = 
		CAllThreadsScripts::GetActiveSet()->GetAvailableScripts(L"FILE_FOLDER_PACK_UNPACK");

	// add the default unpackers to the unpackers list
	m_UnpackerPlugins.Add(noPlugin.get());
	m_UnpackerPlugins.Add(automaticPlugin.get());
	// add the real unpackers to the unpackers list
	size_t i;
	for (i = 0 ; i < piFileFolderScriptArray->size() ; i++)
	{
		// during the dialog, we use a pointer to the scriptsOfThreads array
		const PluginInfoPtr& plugin = piFileFolderScriptArray->at(i);
		if (!plugin->m_disabled)
		{
			m_UnpackerPlugins.Add(plugin.get());
		}
	}
	for (i = 0 ; i < piFileScriptArray->size() ; i++)
	{
		// during the dialog, we use a pointer to the scriptsOfThreads array
		const PluginInfoPtr& plugin = piFileScriptArray->at(i);
		if (!plugin->m_disabled)
		{
			m_UnpackerPlugins.Add(plugin.get());
		}
	}
	for (i = 0 ; i < piBufferScriptArray->size() ; i++)
	{
		// during the dialog, we use a pointer to the scriptsOfThreads array
		const PluginInfoPtr& plugin = piBufferScriptArray->at(i);
		if (!plugin->m_disabled)
		{
			m_UnpackerPlugins.Add(plugin.get());
		}
	}
}


CSelectUnpackerDlg::CSelectUnpackerDlg(const String& filename, CWnd* pParent /*= nullptr*/)
	: CTrDialog(CSelectUnpackerDlg::IDD, pParent), m_filteredFilenames(filename)
{
	Initialize();
}

CSelectUnpackerDlg::CSelectUnpackerDlg(const String& filename1, const String& filename2, CWnd* pParent /*= nullptr*/)
	: CTrDialog(CSelectUnpackerDlg::IDD, pParent), m_filteredFilenames(filename1 + _T("|") + filename2)
{
	Initialize();
}


CSelectUnpackerDlg::~CSelectUnpackerDlg()
{
}


void CSelectUnpackerDlg::SetInitialInfoHandler(PackingInfo * infoHandler)
{
	// default value
	m_pPlugins.clear();

	if (infoHandler != nullptr && infoHandler->m_PluginOrPredifferMode != PLUGIN_MODE::PLUGIN_MANUAL)
		// automatic unpacker
		m_pPlugins.push_back(automaticPlugin.get());
	else if (infoHandler)
	{
		// find the initial unpacker
		int i;
		for (i = 0 ; i < m_UnpackerPlugins.GetSize() ; i++)
		{
			PluginInfo * pPlugin = static_cast<PluginInfo*> (m_UnpackerPlugins.GetAt(i));
			for (const auto& pluginName : infoHandler->m_PluginNames)
			{
				if (pPlugin->m_name == pluginName)
					m_pPlugins.push_back(pPlugin);
			}
		}
	}
}

const PackingInfo CSelectUnpackerDlg::GetInfoHandler()
{
	if (m_pPlugins.empty())
		return PackingInfo(PLUGIN_MODE::PLUGIN_MANUAL);
	else if (m_pPlugins[0] == automaticPlugin.get())
		return PackingInfo(PLUGIN_MODE::PLUGIN_AUTO);
	else
	{
		// build a real plugin unpacker
		PackingInfo infoHandler;
		infoHandler.m_PluginOrPredifferMode = PLUGIN_MODE::PLUGIN_MANUAL;
		infoHandler.m_PluginNames = m_strPluginNames;
		return infoHandler;
	}
}



void CSelectUnpackerDlg::DoDataExchange(CDataExchange* pDX)
{
	CTrDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectUnpackerDlg)
	DDX_Control(pDX, IDC_UNPACKER_NAME, m_cboUnpackerName);
	DDX_Check(pDX, IDC_UNPACKER_ALLOW_ALL, m_bNoExtensionCheck);
	DDX_Text(pDX, IDC_UNPACKER_DESCRIPTION, m_strDescription);
	DDX_Text(pDX, IDC_UNPACKER_SUPPORTED_EXTENSIONS, m_strExtensions);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectUnpackerDlg, CTrDialog)
	//{{AFX_MSG_MAP(CSelectUnpackerDlg)
	ON_BN_CLICKED(IDC_UNPACKER_ALLOW_ALL, OnUnpackerAllowAll)
	ON_CBN_SELCHANGE(IDC_UNPACKER_NAME, OnSelchangeUnpackerName)
	ON_CBN_SELENDOK(IDC_UNPACKER_NAME, OnSelchangeUnpackerName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectUnpackerDlg message handlers

void CSelectUnpackerDlg::OnOK() 
{
	GetOptionsMgr()->SaveOption(OPT_PLUGINS_UNPACK_DONT_CHECK_EXTENSION, m_bNoExtensionCheck);

	CTrDialog::OnOK();
}

BOOL CSelectUnpackerDlg::OnInitDialog() 
{
	CTrDialog::OnInitDialog();

	m_bNoExtensionCheck = GetOptionsMgr()->GetBool(OPT_PLUGINS_UNPACK_DONT_CHECK_EXTENSION);

	prepareListbox();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectUnpackerDlg::prepareListbox() 
{
	int sel = -1;
	int i;
	for (i = 0 ; i < m_UnpackerPlugins.GetSize() ; i++)
	{
		PluginInfo * pPlugin = static_cast<PluginInfo*> (m_UnpackerPlugins.GetAt(i));
		if (pPlugin == noPlugin.get() || pPlugin == automaticPlugin.get() 
				|| m_bNoExtensionCheck 
			  || pPlugin->TestAgainstRegList(m_filteredFilenames))
		{
			m_cboUnpackerName.AddString(pPlugin->m_name.c_str());
			if (!m_pPlugins.empty() && pPlugin == m_pPlugins[0])
				sel = m_cboUnpackerName.GetCount()-1;
		}
	}

	if (sel == -1)
		m_cboUnpackerName.SelectString(-1, noPlugin->m_name.c_str());
	else
		m_cboUnpackerName.SetCurSel(sel);

	OnSelchangeUnpackerName();
}

void CSelectUnpackerDlg::OnUnpackerAllowAll() 
{
	UpdateData ();

	m_cboUnpackerName.ResetContent();

	prepareListbox();

	UpdateData (FALSE);
}

void CSelectUnpackerDlg::OnSelchangeUnpackerName() 
{
	int i = m_cboUnpackerName.GetCurSel();
	if (i <= 0)
	{
		m_pPlugins.clear();
		m_strPluginNames.clear();
	}
	else
	{
		// initialize with the default unpacker
		m_pPlugins.clear();
		m_strPluginNames.clear();
		PluginInfo* pPlugin;
		CString cstrPluginName;
		m_cboUnpackerName.GetWindowText(cstrPluginName);
		m_strPluginNames.push_back((const TCHAR *)cstrPluginName);
		for (int j = 0 ; j < m_UnpackerPlugins.GetSize() ; j++)
		{
			pPlugin = static_cast<PluginInfo*> (m_UnpackerPlugins.GetAt(j));
			if (m_strPluginNames[0] == pPlugin->m_name)
			{
				m_pPlugins.push_back(pPlugin);
				break;
			}
		}
	}

	if (!m_pPlugins.empty())
	{
		m_strPluginNames[0] = m_pPlugins[0]->m_name;
		m_strDescription = m_pPlugins[0]->m_description;
		m_strExtensions = m_pPlugins[0]->m_filtersText;
	}

	UpdateData (FALSE);
}
