/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
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
	automaticPlugin->m_description = _("The adapted unpacker is applied to both files (one file only needs the extension)");

	m_pPlugin = noPlugin.get();

	PluginArray * piFileScriptArray = 
		CAllThreadsScripts::GetActiveSet()->GetAvailableScripts(L"FILE_PACK_UNPACK");
	PluginArray * piBufferScriptArray = 
		CAllThreadsScripts::GetActiveSet()->GetAvailableScripts(L"BUFFER_PACK_UNPACK");
	PluginArray * piFileFolderScriptArray = 
		CAllThreadsScripts::GetActiveSet()->GetAvailableScripts(L"FILE_FOLDER_PACK_UNPACK");

	// add the default unpackers to the unpackers list
	m_UnpackerPlugins.Add(noPlugin.get());
	m_bWithFileFlags.push_back(false);
	m_UnpackerPlugins.Add(automaticPlugin.get());
	m_bWithFileFlags.push_back(true);
	// add the real unpackers to the unpackers list
	size_t i;
	for (i = 0 ; i < piFileScriptArray->size() ; i++)
	{
		// during the dialog, we use a pointer to the scriptsOfThreads array
		const PluginInfoPtr& plugin = piFileScriptArray->at(i);
		if (!plugin->m_disabled)
		{
			m_UnpackerPlugins.Add(plugin.get());
			m_bWithFileFlags.push_back(true);
		}
	}
	for (i = 0 ; i < piBufferScriptArray->size() ; i++)
	{
		// during the dialog, we use a pointer to the scriptsOfThreads array
		const PluginInfoPtr& plugin = piBufferScriptArray->at(i);
		if (!plugin->m_disabled)
		{
			m_UnpackerPlugins.Add(plugin.get());
			m_bWithFileFlags.push_back(false);
		}
	}
	for (i = 0 ; i < piFileFolderScriptArray->size() ; i++)
	{
		// during the dialog, we use a pointer to the scriptsOfThreads array
		const PluginInfoPtr& plugin = piFileFolderScriptArray->at(i);
		if (!plugin->m_disabled)
		{
			m_UnpackerPlugins.Add(plugin.get());
			m_bWithFileFlags.push_back(true);
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
	m_pPlugin = noPlugin.get();

	if (infoHandler != nullptr && infoHandler->m_PluginOrPredifferMode != PLUGIN_MANUAL)
		// automatic unpacker
		m_pPlugin = automaticPlugin.get();
	else if (infoHandler)
	{
		// find the initial unpacker
		int i;
		for (i = 0 ; i < m_UnpackerPlugins.GetSize() ; i++)
		{
			PluginInfo * pPlugin = static_cast<PluginInfo*> (m_UnpackerPlugins.GetAt(i));
			if (pPlugin->m_name == infoHandler->m_PluginName)
				m_pPlugin = pPlugin;
		}
	}
}

const PackingInfo CSelectUnpackerDlg::GetInfoHandler()
{
	if (m_pPlugin == noPlugin.get())
		return PackingInfo(PLUGIN_MANUAL);
	else if (m_pPlugin == automaticPlugin.get())
		return PackingInfo(PLUGIN_AUTO);
	else
	{
		// build a real plugin unpacker
		PackingInfo infoHandler;
		infoHandler.m_PluginOrPredifferMode = PLUGIN_MANUAL;
		infoHandler.m_PluginName = m_strPluginName;
		int i;
		for (i = 0 ; i < m_UnpackerPlugins.GetSize() ; i++)
		{
			PluginInfo * pPlugin = static_cast<PluginInfo*> (m_UnpackerPlugins.GetAt(i));
			if (m_pPlugin == pPlugin)
				break;
		}
		infoHandler.m_bWithFile = m_bWithFileFlags.at(i);
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
			if (pPlugin == m_pPlugin)
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
	if (i == -1)
	{
		m_pPlugin = noPlugin.get();
	}
	else
	{
		// initialize with the default unpacker
		m_pPlugin = static_cast<PluginInfo*> (m_UnpackerPlugins.GetAt(0));
		PluginInfo * pPlugin;
		CString cstrPluginName;
		m_cboUnpackerName.GetWindowText(cstrPluginName);
		m_strPluginName = cstrPluginName;
		for (int j = 0 ; j < m_UnpackerPlugins.GetSize() ; j++)
		{
			pPlugin = static_cast<PluginInfo*> (m_UnpackerPlugins.GetAt(j));
			if (m_strPluginName == pPlugin->m_name)
			{
				m_pPlugin = pPlugin;
				break;
			}
		}
	}

	m_strPluginName = m_pPlugin->m_name;
	m_strDescription = m_pPlugin->m_description;
	m_strExtensions = m_pPlugin->m_filtersText;

	UpdateData (FALSE);
}
