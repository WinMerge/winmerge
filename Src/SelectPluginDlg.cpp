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

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



/////////////////////////////////////////////////////////////////////////////
// CSelectPluginDlg dialog

void CSelectPluginDlg::Initialize()
{
	//{{AFX_DATA_INIT(CSelectPluginDlg)
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

	PluginArray * piFileScriptArray = 
		CAllThreadsScripts::GetActiveSet()->GetAvailableScripts(L"FILE_PACK_UNPACK");
	PluginArray * piBufferScriptArray = 
		CAllThreadsScripts::GetActiveSet()->GetAvailableScripts(L"BUFFER_PACK_UNPACK");
	PluginArray * piFileFolderScriptArray = 
		CAllThreadsScripts::GetActiveSet()->GetAvailableScripts(L"FILE_FOLDER_PACK_UNPACK");

	// add the default unpackers to the unpackers list
	m_Plugins.Add(noPlugin.get());
	m_Plugins.Add(automaticPlugin.get());
	// add the real unpackers to the unpackers list
	size_t i;
	for (i = 0 ; i < piFileFolderScriptArray->size() ; i++)
	{
		// during the dialog, we use a pointer to the scriptsOfThreads array
		const PluginInfoPtr& plugin = piFileFolderScriptArray->at(i);
		if (!plugin->m_disabled)
		{
			m_Plugins.Add(plugin.get());
		}
	}
	for (i = 0 ; i < piFileScriptArray->size() ; i++)
	{
		// during the dialog, we use a pointer to the scriptsOfThreads array
		const PluginInfoPtr& plugin = piFileScriptArray->at(i);
		if (!plugin->m_disabled)
		{
			m_Plugins.Add(plugin.get());
		}
	}
	for (i = 0 ; i < piBufferScriptArray->size() ; i++)
	{
		// during the dialog, we use a pointer to the scriptsOfThreads array
		const PluginInfoPtr& plugin = piBufferScriptArray->at(i);
		if (!plugin->m_disabled)
		{
			m_Plugins.Add(plugin.get());
		}
	}
}


CSelectPluginDlg::CSelectPluginDlg(const String& filename, CWnd* pParent /*= nullptr*/)
	: CTrDialog(CSelectPluginDlg::IDD, pParent), m_filteredFilenames(filename)
{
	Initialize();
}

CSelectPluginDlg::CSelectPluginDlg(const String& filename1, const String& filename2, CWnd* pParent /*= nullptr*/)
	: CTrDialog(CSelectPluginDlg::IDD, pParent), m_filteredFilenames(filename1 + _T("|") + filename2)
{
	Initialize();
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
	DDX_Text(pDX, IDC_PLUGIN_DESCRIPTION, m_strDescription);
	DDX_Text(pDX, IDC_PLUGIN_SUPPORTED_EXTENSIONS, m_strExtensions);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectPluginDlg, CTrDialog)
	//{{AFX_MSG_MAP(CSelectPluginDlg)
	ON_BN_CLICKED(IDC_PLUGIN_ALLOW_ALL, OnUnpackerAllowAll)
	ON_CBN_SELCHANGE(IDC_PLUGIN_NAME, OnSelchangeUnpackerName)
	ON_CBN_SELENDOK(IDC_PLUGIN_NAME, OnSelchangeUnpackerName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectPluginDlg message handlers

void CSelectPluginDlg::OnOK() 
{
	GetOptionsMgr()->SaveOption(OPT_PLUGINS_UNPACK_DONT_CHECK_EXTENSION, m_bNoExtensionCheck);

	CTrDialog::OnOK();
}

BOOL CSelectPluginDlg::OnInitDialog() 
{
	CTrDialog::OnInitDialog();

	m_bNoExtensionCheck = GetOptionsMgr()->GetBool(OPT_PLUGINS_UNPACK_DONT_CHECK_EXTENSION);

	prepareListbox();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectPluginDlg::prepareListbox() 
{
	int sel = -1;
	int i;
	for (i = 0 ; i < m_Plugins.GetSize() ; i++)
	{
		PluginInfo * pPlugin = static_cast<PluginInfo*> (m_Plugins.GetAt(i));
		if (pPlugin == noPlugin.get() || pPlugin == automaticPlugin.get() 
				|| m_bNoExtensionCheck 
			  || pPlugin->TestAgainstRegList(m_filteredFilenames))
		{
			m_cboPluginName.AddString(pPlugin->m_name.c_str());
			if (m_strPluginPipeline == pPlugin->m_name)
				sel = m_cboPluginName.GetCount()-1;
		}
	}

	if (sel == -1)
		m_cboPluginName.SelectString(-1, noPlugin->m_name.c_str());
	else
		m_cboPluginName.SetCurSel(sel);

	OnSelchangeUnpackerName();
}

void CSelectPluginDlg::OnUnpackerAllowAll() 
{
	UpdateData ();

	m_cboPluginName.ResetContent();

	prepareListbox();

	UpdateData (FALSE);
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
		m_strPluginPipeline.clear();
		CString cstrPluginName;
		m_cboPluginName.GetWindowText(cstrPluginName);
		m_strPluginPipeline = cstrPluginName;
		for (int j = 0 ; j < m_Plugins.GetSize() ; j++)
		{
			PluginInfo *pPluginTmp = static_cast<PluginInfo*> (m_Plugins.GetAt(j));
			if (m_strPluginPipeline == pPluginTmp->m_name)
			{
				pPlugin = pPluginTmp;
				break;
			}
		}

	}

	if (pPlugin)
	{
		m_strDescription = pPlugin->m_description;
		m_strExtensions = pPlugin->m_filtersText;
	}

	UpdateData (FALSE);
}
