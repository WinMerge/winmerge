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
// SelectUnpackerDlg.cpp : implementation file
//
// $Id$
//

#include "stdafx.h"
#include "merge.h"
#include "SelectUnpackerDlg.h"
#include "Plugins.h"
#include "FileTransform.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CSelectUnpackerDlg dialog

void CSelectUnpackerDlg::Initialize()
{
	//{{AFX_DATA_INIT(CSelectUnpackerDlg)
	m_bNoExtensionCheck = FALSE;
	m_strDescription = _T("");
	m_strExtensions = _T("");
	//}}AFX_DATA_INIT

	// texts for the default unpackers
	noPlugin = new PluginInfo;
	noPlugin->lpDispatch = NULL;
	noPlugin->filters = NULL;
	noPlugin->name = theApp.LoadString(IDS_USERCHOICE_NONE);
	automaticPlugin = new PluginInfo;
	automaticPlugin->lpDispatch = NULL;
	automaticPlugin->filters = NULL;
	automaticPlugin->name = theApp.LoadString(IDS_USERCHOICE_AUTOMATIC);
	automaticPlugin->description = theApp.LoadString(ID_UNPACK_AUTO).c_str();

	m_pPlugin = noPlugin;

	PluginArray * piFileScriptArray = 
		CAllThreadsScripts::GetActiveSet()->GetAvailableScripts(L"FILE_PACK_UNPACK");
	PluginArray * piBufferScriptArray = 
		CAllThreadsScripts::GetActiveSet()->GetAvailableScripts(L"BUFFER_PACK_UNPACK");

	// add the default unpackers to the unpackers list
	m_UnpackerPlugins.Add(noPlugin);
	m_bWithFileFlags.Add(0);
	m_UnpackerPlugins.Add(automaticPlugin);
	m_bWithFileFlags.Add(0);
	// add the real unpackers to the unpackers list
	int i;
	for (i = 0 ; i < piFileScriptArray->GetSize() ; i++)
	{
		// during the dialog, we use a pointer to the scriptsOfThreads array
		m_UnpackerPlugins.Add(&piFileScriptArray->ElementAt(i));
		m_bWithFileFlags.Add(1);
	}
	for (i = 0 ; i < piBufferScriptArray->GetSize() ; i++)
	{
		// during the dialog, we use a pointer to the scriptsOfThreads array
		m_UnpackerPlugins.Add(&piBufferScriptArray->ElementAt(i));
		m_bWithFileFlags.Add(0);
	}
}


CSelectUnpackerDlg::CSelectUnpackerDlg(LPCTSTR filename, CWnd* pParent /*=NULL*/)
	: CDialog(CSelectUnpackerDlg::IDD, pParent)
{
	m_filteredFilenames = filename;
	Initialize();
}

CSelectUnpackerDlg::CSelectUnpackerDlg(LPCTSTR filename1, LPCTSTR filename2, CWnd* pParent /*=NULL*/)
	: CDialog(CSelectUnpackerDlg::IDD, pParent)
{
	m_filteredFilenames = filename1;
	m_filteredFilenames += "|";
	m_filteredFilenames += filename2;
	Initialize();
}


CSelectUnpackerDlg::~CSelectUnpackerDlg()
{
	delete noPlugin;
	delete automaticPlugin;
}


void CSelectUnpackerDlg::SetInitialInfoHandler(PackingInfo * infoHandler)
{
	// default value
	m_pPlugin = noPlugin;

	if (infoHandler && infoHandler->bToBeScanned)
		// automatic unpacker
		m_pPlugin = automaticPlugin;
	else if (infoHandler)
	{
		// find the initial unpacker
		int i;
		for (i = 0 ; i < m_UnpackerPlugins.GetSize() ; i++)
		{
			PluginInfo * pPlugin = static_cast<PluginInfo*> (m_UnpackerPlugins.GetAt(i));
			if (pPlugin->name == infoHandler->pluginName)
				m_pPlugin = pPlugin;
		}
	}
}

const PackingInfo CSelectUnpackerDlg::GetInfoHandler()
{
	if (m_pPlugin == noPlugin)
		return PackingInfo(PLUGIN_MANUAL);
	else if (m_pPlugin == automaticPlugin)
		return PackingInfo(PLUGIN_AUTO);
	else
	{
		// build a real plugin unpacker
		PackingInfo infoHandler;
		infoHandler.bToBeScanned = FALSE;
		infoHandler.pluginName = m_strPluginName;
		int i;
		for (i = 0 ; i < m_UnpackerPlugins.GetSize() ; i++)
		{
			PluginInfo * pPlugin = static_cast<PluginInfo*> (m_UnpackerPlugins.GetAt(i));
			if (m_pPlugin == pPlugin)
				break;
		}
		infoHandler.bWithFile = m_bWithFileFlags.GetAt(i);
		return infoHandler;
	}
}



void CSelectUnpackerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectUnpackerDlg)
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_UNPACKER_NAME, m_cboUnpackerName);
	DDX_Check(pDX, IDC_UNPACKER_ALLOW_ALL, m_bNoExtensionCheck);
	DDX_Text(pDX, IDC_UNPACKER_DESCRIPTION, m_strDescription);
	DDX_Text(pDX, IDC_UNPACKER_SUPPORTED_EXTENSIONS, m_strExtensions);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectUnpackerDlg, CDialog)
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
	AfxGetApp()->WriteProfileInt(_T("Plugins"), _T("UnpackDontCheckExtension"), m_bNoExtensionCheck);

	CDialog::OnOK();
}

BOOL CSelectUnpackerDlg::OnInitDialog() 
{
	theApp.TranslateDialog(m_hWnd);
	CDialog::OnInitDialog();

	m_bNoExtensionCheck = AfxGetApp()->GetProfileInt(_T("Plugins"), _T("UnpackDontCheckExtension"), FALSE);

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
		if (pPlugin == noPlugin || pPlugin == automaticPlugin 
				|| m_bNoExtensionCheck 
			  || pPlugin->TestAgainstRegList(m_filteredFilenames))
		{
			m_cboUnpackerName.AddString(pPlugin->name.c_str());
			if (pPlugin == m_pPlugin)
				sel = m_cboUnpackerName.GetCount()-1;
		}
	}

	if (sel == -1)
		m_cboUnpackerName.SelectString(-1, noPlugin->name.c_str());
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
		m_pPlugin = noPlugin;
	}
	else
	{
		// initialize with the default unpacker
		m_pPlugin = static_cast<PluginInfo*> (m_UnpackerPlugins.GetAt(0));
		PluginInfo * pPlugin;
		m_cboUnpackerName.GetWindowText(m_strPluginName);
		for (int j = 0 ; j < m_UnpackerPlugins.GetSize() ; j++)
		{
			pPlugin = static_cast<PluginInfo*> (m_UnpackerPlugins.GetAt(j));
			if (m_strPluginName == pPlugin->name.c_str())
			{
				m_pPlugin = pPlugin;
				break;
			}
		}
	}

	m_strPluginName = m_pPlugin->name.c_str();
	m_strDescription = m_pPlugin->description;
	m_strExtensions = m_pPlugin->filtersText;

	UpdateData (FALSE);
}
