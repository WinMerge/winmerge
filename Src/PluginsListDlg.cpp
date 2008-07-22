/**
 * @file  PluginsListDlg.cpp
 *
 * @brief Implementation file for PluginsList dialog
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include "UnicodeString.h"
#include "Merge.h"
#include "Plugins.h"
#include "dllver.h"
#include "PluginsListDlg.h"
#include "OptionsDef.h"

IMPLEMENT_DYNAMIC(PluginsListDlg, CDialog)

BEGIN_MESSAGE_MAP(PluginsListDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

/**
 * @brief Constructor.
 */
PluginsListDlg::PluginsListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PluginsListDlg::IDD, pParent)
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
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PLUGINSLIST_LIST, m_list);
}

/**
 * @brief Initialize the dialog.
 */
BOOL PluginsListDlg::OnInitDialog()
{
	theApp.TranslateDialog(m_hWnd);
	CDialog::OnInitDialog();
	
	InitList();
	AddPlugins();

	BOOL pluginsEnabled = GetOptionsMgr()->GetBool(OPT_PLUGINS_ENABLED);
	if (pluginsEnabled)
	{
		CButton *btn = (CButton *)GetDlgItem(IDC_PLUGINS_ENABLE);
		btn->SetCheck(BST_CHECKED);
	}

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief Set-up the list control.
 */
void PluginsListDlg::InitList()
{
	// Show selection across entire row.
	DWORD newstyle = LVS_EX_FULLROWSELECT;
	// Also enable infotips if they have new enough version for our
	// custom draw code
	// LPNMLVCUSTOMDRAW->iSubItem not supported before comctl32 4.71
	if (GetDllVersion(_T("comctl32.dll")) >= PACKVERSION(4,71))
		newstyle |= LVS_EX_INFOTIP;
	m_list.SetExtendedStyle(m_list.GetExtendedStyle() | newstyle);

	String title = LoadResString(IDS_PLUGINSLIST_NAME);
	m_list.InsertColumn(0, title.c_str(), LVCFMT_LEFT, 150);
	title = LoadResString(IDS_PLUGINSLIST_TYPE);
	m_list.InsertColumn(1, title.c_str(), LVCFMT_LEFT, 100);
	title = LoadResString(IDS_PLUGINSLIST_DESC);
	m_list.InsertColumn(2, title.c_str(), LVCFMT_LEFT, 300);
}

/**
 * @brief Add found plugins to the list.
 */
void PluginsListDlg::AddPlugins()
{
	String type = LoadResString(IDS_PLUGINS_TYPE_UNPACKER);
	AddPluginsToList(L"FILE_PACK_UNPACK", type.c_str());
	AddPluginsToList(L"BUFFER_PACK_UNPACK", type.c_str());
	type = LoadResString(IDS_PLUGINS_TYPE_PREDIFFER);
	AddPluginsToList(L"FILE_PREDIFF", type.c_str());
	AddPluginsToList(L"BUFFER_PREDIFF", type.c_str());
	type = LoadResString(IDS_PLUGINS_TYPE_EDITSCRIPT);
	AddPluginsToList(L"EDITOR_SCRIPT", type.c_str());
}

/**
 * @brief Add plugins of given event type to the list.
 * @param [in] pluginEvent Event type for plugins to add.
 * @param [in] pluginType String to use as type in the list.
 */
void PluginsListDlg::AddPluginsToList(LPCWSTR pluginEvent, LPCTSTR pluginType)
{
	PluginArray * piPluginArray = 
		CAllThreadsScripts::GetActiveSet()->GetAvailableScripts(pluginEvent);

	for (int iPlugin = 0 ; iPlugin < piPluginArray->GetSize() ; iPlugin++)
	{
		PluginInfo & plugin = piPluginArray->ElementAt(iPlugin);
		int ind = m_list.InsertItem(m_list.GetItemCount(), plugin.m_name.c_str());
		m_list.SetItemText(ind, 1, pluginType);
		m_list.SetItemText(ind, 2, plugin.m_description.c_str());
	}
}

/**
 * @brief Save plugins enabled setting when closing the dialog.
 */
void PluginsListDlg::OnBnClickedOk()
{
	CButton *btn = (CButton *)GetDlgItem(IDC_PLUGINS_ENABLE);
	int check = btn->GetCheck();
	if (check == BST_CHECKED)
	{
		GetOptionsMgr()->SaveOption(OPT_PLUGINS_ENABLED, true);
	}
	else
	{
		GetOptionsMgr()->SaveOption(OPT_PLUGINS_ENABLED, false);
	}
	OnOK();
}
