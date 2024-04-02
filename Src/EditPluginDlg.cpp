// SPDX-License-Identifier: GPL-2.0-or-later
/**
 * @file  EditPluginDlg.cpp
 *
 * @brief Unpacker plugin selection dialog implementation.
 */

#include "stdafx.h"
#include "EditPluginDlg.h"
#include "Plugins.h"
#include "FileTransform.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"
#include "unicoder.h"
#include "Merge.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

enum { URL_PACK_UNPACK, FILE_PACK_UNPACK, FILE_FOLDER_PACK_UNPACK, PREDIFF_FILE, ALIAS_PACK_UNPACK, ALIAS_PRDIFF, ALIAS_EDITOR_SCRIPT };
enum { UnpackFile = 0, PackFile = 1, UnpackFolder = 2, PackFolder = 3, IsFolder = 4, PrediffFile = 0 };

/////////////////////////////////////////////////////////////////////////////
// CEditPluginDlg dialog

CEditPluginDlg::CEditPluginDlg(internal_plugin::Info& info, CWnd* pParent/* = nullptr*/)
	: CTrDialog(CEditPluginDlg::IDD, pParent)
	, m_info(info)
	, m_strEvent(info.m_event)
	, m_strPluginName(info.m_name)
	, m_strDescription(info.m_description)
	, m_strExtensions(info.m_fileFilters)
	, m_strArguments(info.m_arguments)
	, m_strPluginPipeline(info.m_pipeline)
{
	auto menuCaption = PluginInfo::GetExtendedPropertyValue(info.m_extendedProperties, _T("MenuCaption"));
	if (menuCaption.has_value())
		m_strMenuCaption = menuCaption.value();
	auto processType = PluginInfo::GetExtendedPropertyValue(info.m_extendedProperties, _T("ProcessType"));
	if (processType.has_value())
		m_strProcessType = processType.value();
	auto windowType = PluginInfo::GetExtendedPropertyValue(info.m_extendedProperties, _T("PreferredWindowType"));
	if (windowType.has_value())
		m_strWindowType = windowType.value();
	auto argumentsRequired = PluginInfo::GetExtendedPropertyValue(info.m_extendedProperties, _T("ArgumentsRequired"));
	m_bArgumentsRequired = argumentsRequired.has_value();
	auto generateEditorScript = PluginInfo::GetExtendedPropertyValue(info.m_extendedProperties, _T("GenerateEditorScript"));
	m_bGenerateEditorScript = generateEditorScript.has_value();
	if (info.m_unpackFile)
		LoadMethod(*info.m_unpackFile, 0);
	if (info.m_packFile)
		LoadMethod(*info.m_packFile, 1);
	if (info.m_isFolder)
		LoadMethod(*info.m_isFolder, 2);
	if (info.m_prediffFile)
		LoadMethod(*info.m_prediffFile, 0);
	m_strCommandline = m_strCommandlineAry[0];
	m_strScriptFileExtension = m_strScriptFileExtensionAry[0];
	m_strScriptBody = m_strScriptBodyAry[0];
}

CEditPluginDlg::~CEditPluginDlg()
{
}

void CEditPluginDlg::LoadMethod(const internal_plugin::Method& method, int index)
{
	m_strCommandlineAry[index] = method.m_command;
	if (method.m_script)
	{
		m_strScriptFileExtensionAry[index] = method.m_script->m_fileExtension;
		m_strScriptBodyAry[index] = method.m_script->m_body;
	}
}

void CEditPluginDlg::SaveMethod(internal_plugin::Method& method, int index)
{
	method.m_command = m_strCommandlineAry[index];
	if (!m_strScriptBodyAry[index].empty())
	{
		method.m_script = std::make_unique<internal_plugin::Script>();
		method.m_script->m_fileExtension = m_strScriptFileExtensionAry[index];
		method.m_script->m_body = m_strScriptBodyAry[index];
	}
}

void CEditPluginDlg::UpdateControls()
{
	m_ctlTab.DeleteAllItems();
	const int cursel = m_ctlEvent.GetCurSel();
	if (cursel == URL_PACK_UNPACK || cursel == FILE_PACK_UNPACK)
	{
		m_ctlTab.InsertItem(0, _T("UnpackFile"));
		m_ctlTab.InsertItem(1, _T("PackFile"));
	}
	else if (cursel == FILE_FOLDER_PACK_UNPACK)
	{
		m_ctlTab.InsertItem(0, _T("UnpackFile"));
		m_ctlTab.InsertItem(1, _T("PackFile"));
		m_ctlTab.InsertItem(2, _T("UnpackFolder"));
		m_ctlTab.InsertItem(3, _T("PackFolder"));
		m_ctlTab.InsertItem(4, _T("IsFolder"));
	}
	else if (cursel == PREDIFF_FILE)
	{
		m_ctlTab.InsertItem(0, _T("PrediffFile"));
	}
	const bool unpacker = (cursel < 3);
	const bool alias = (cursel > 3);
	const bool urlhandler = (cursel == 0);
	ShowDlgItem(IDC_PLUGIN_TAB, !alias);
	ShowDlgItem(IDC_PLUGIN_COMMAND_LINE_STATIC, !alias);
	ShowDlgItem(IDC_PLUGIN_COMMAND_LINE, !alias);
	ShowDlgItem(IDC_PLUGIN_COMMAND_LINE_MENU, !alias);
	ShowDlgItem(IDC_PLUGIN_SCRIPT_FILEEXTENSION_STATIC, !alias);
	ShowDlgItem(IDC_PLUGIN_SCRIPT_FILEEXTENSION, !alias);
	ShowDlgItem(IDC_PLUGIN_SCRIPT_BODY_STATIC, !alias);
	ShowDlgItem(IDC_PLUGIN_SCRIPT_BODY, !alias);
	ShowDlgItem(IDC_PLUGIN_PIPELINE_STATIC, alias);
	ShowDlgItem(IDC_PLUGIN_PIPELINE, alias);
	ShowDlgItem(IDC_PLUGIN_PIPELINE_MENU, alias);
	ShowDlgItem(IDC_PLUGIN_WINDOWTYPE_STATIC, unpacker);
	ShowDlgItem(IDC_PLUGIN_WINDOWTYPE, unpacker);
	ShowDlgItem(IDC_PLUGIN_GENERATESCRIPT, unpacker);
	ShowDlgItem(IDC_PLUGIN_MENUCAPTION_STATIC, !urlhandler);
	ShowDlgItem(IDC_PLUGIN_MENUCAPTION, !urlhandler);
	Invalidate();
}

static void RemoveMenuItems(CMenu* pPopup, unsigned menuid, int plugintype, int method)
{
	if (menuid == IDR_POPUP_PLUGIN_COMMAND_LINE_MENU)
	{
		if (plugintype == URL_PACK_UNPACK)
		{
			if (method == UnpackFile)
			{
				for (auto id : {
					ID_PLUGIN_COMMAND_LINE_SRC_FILE, ID_PLUGIN_COMMAND_LINE_SRC_FOLDER,
					ID_PLUGIN_COMMAND_LINE_DST_URL, ID_PLUGIN_COMMAND_LINE_DST_URL_PROTOCOL, ID_PLUGIN_COMMAND_LINE_DST_URL_SUFFIX, ID_PLUGIN_COMMAND_LINE_DST_FOLDER })
					pPopup->RemoveMenu(id, MF_BYCOMMAND);
			}
			else if (method == PackFile)
			{
				for (auto id : {
					ID_PLUGIN_COMMAND_LINE_DST_FILE, ID_PLUGIN_COMMAND_LINE_DST_FOLDER,
					ID_PLUGIN_COMMAND_LINE_SRC_URL, ID_PLUGIN_COMMAND_LINE_SRC_URL_PROTOCOL, ID_PLUGIN_COMMAND_LINE_SRC_URL_SUFFIX, ID_PLUGIN_COMMAND_LINE_SRC_FOLDER })
					pPopup->RemoveMenu(id, MF_BYCOMMAND);
			}
		}
		else if (plugintype == FILE_PACK_UNPACK || plugintype == PREDIFF_FILE)
		{
			for (auto id : {
				ID_PLUGIN_COMMAND_LINE_SRC_URL, ID_PLUGIN_COMMAND_LINE_SRC_URL_PROTOCOL, ID_PLUGIN_COMMAND_LINE_SRC_URL_SUFFIX, ID_PLUGIN_COMMAND_LINE_SRC_FOLDER,
				ID_PLUGIN_COMMAND_LINE_DST_URL, ID_PLUGIN_COMMAND_LINE_DST_URL_PROTOCOL, ID_PLUGIN_COMMAND_LINE_DST_URL_SUFFIX, ID_PLUGIN_COMMAND_LINE_DST_FOLDER })
				pPopup->RemoveMenu(id, MF_BYCOMMAND);
		}
		else if (plugintype == FILE_FOLDER_PACK_UNPACK)
		{
			if (method == UnpackFile || method == PackFile)
			{
				for (auto id : {
					ID_PLUGIN_COMMAND_LINE_SRC_URL, ID_PLUGIN_COMMAND_LINE_SRC_URL_PROTOCOL, ID_PLUGIN_COMMAND_LINE_SRC_URL_SUFFIX, ID_PLUGIN_COMMAND_LINE_SRC_FOLDER,
					ID_PLUGIN_COMMAND_LINE_DST_URL, ID_PLUGIN_COMMAND_LINE_DST_URL_PROTOCOL, ID_PLUGIN_COMMAND_LINE_DST_URL_SUFFIX, ID_PLUGIN_COMMAND_LINE_DST_FOLDER })
					pPopup->RemoveMenu(id, MF_BYCOMMAND);
			}
			else if (method == UnpackFolder)
			{
				for (auto id : {
					ID_PLUGIN_COMMAND_LINE_SRC_URL, ID_PLUGIN_COMMAND_LINE_SRC_URL_PROTOCOL, ID_PLUGIN_COMMAND_LINE_SRC_URL_SUFFIX, ID_PLUGIN_COMMAND_LINE_SRC_FOLDER,
					ID_PLUGIN_COMMAND_LINE_DST_URL, ID_PLUGIN_COMMAND_LINE_DST_URL_PROTOCOL, ID_PLUGIN_COMMAND_LINE_DST_URL_SUFFIX, ID_PLUGIN_COMMAND_LINE_DST_FILE})
					pPopup->RemoveMenu(id, MF_BYCOMMAND);
			}
			else if (method == PackFolder)
			{
				for (auto id : {
					ID_PLUGIN_COMMAND_LINE_SRC_URL, ID_PLUGIN_COMMAND_LINE_SRC_URL_PROTOCOL, ID_PLUGIN_COMMAND_LINE_SRC_URL_SUFFIX, ID_PLUGIN_COMMAND_LINE_SRC_FILE,
					ID_PLUGIN_COMMAND_LINE_DST_URL, ID_PLUGIN_COMMAND_LINE_DST_URL_PROTOCOL, ID_PLUGIN_COMMAND_LINE_DST_URL_SUFFIX, ID_PLUGIN_COMMAND_LINE_DST_FOLDER })
					pPopup->RemoveMenu(id, MF_BYCOMMAND);
			}
			else if (method == IsFolder)
			{
				for (auto id : {
					ID_PLUGIN_COMMAND_LINE_SRC_URL, ID_PLUGIN_COMMAND_LINE_SRC_URL_PROTOCOL, ID_PLUGIN_COMMAND_LINE_SRC_URL_SUFFIX, ID_PLUGIN_COMMAND_LINE_SRC_FOLDER,
					ID_PLUGIN_COMMAND_LINE_DST_URL, ID_PLUGIN_COMMAND_LINE_DST_URL_PROTOCOL, ID_PLUGIN_COMMAND_LINE_DST_URL_SUFFIX, ID_PLUGIN_COMMAND_LINE_DST_FILE, ID_PLUGIN_COMMAND_LINE_DST_FOLDER })
					pPopup->RemoveMenu(id, MF_BYCOMMAND);
			}
		}
	}
}

void CEditPluginDlg::ShowMenu(unsigned menuid, unsigned ctlid)
{
	auto pWnd = GetDlgItem(ctlid);
	if (!pWnd)
		return;
	CRect rcButton;
	pWnd->GetWindowRect(&rcButton);
	CMenu menu;
	menu.LoadMenu(menuid);
	CMenu *pPopup = menu.GetSubMenu(0);
	if (!pPopup)
		return;
	const int curselPluginType = m_ctlEvent.GetCurSel();
	const int curselMethod = m_ctlTab.GetCurSel();
	RemoveMenuItems(pPopup, menuid, curselPluginType, curselMethod);
	theApp.TranslateMenu(menu.m_hMenu);
	if (pPopup)
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, rcButton.left, rcButton.top, this);
}

void CEditPluginDlg::InsertMacro(unsigned menuid, unsigned id, unsigned ctlid)
{
	auto pEdit = static_cast<CEdit*>(GetDlgItem(ctlid));
	if (!pEdit)
		return;
	CMenu menu;
	menu.LoadMenu(menuid);
	CMenu *pPopup = menu.GetSubMenu(0);
	if (!pPopup)
		return;
	CString text;
	pPopup->GetMenuString(id, text, MF_BYCOMMAND);
	int pos = text.Find(_T("}"));
	text = text.Left(pos + 1);
	int nStartChar = 0, nEndChar = 0;
	pEdit->GetSel(nStartChar, nEndChar);
	if (nStartChar == 0 && nEndChar == 0)
		pEdit->SetSel(65535, 65535);
	pEdit->ReplaceSel(text);
}

void CEditPluginDlg::DoDataExchange(CDataExchange* pDX)
{
	CTrDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditPluginDlg)
	DDX_Control(pDX, IDC_PLUGIN_TYPE, m_ctlEvent);
	DDX_Text(pDX, IDC_PLUGIN_NAME, m_strPluginName);
	DDX_Text(pDX, IDC_PLUGIN_DESCRIPTION, m_strDescription);
	DDX_Text(pDX, IDC_PLUGIN_SUPPORTED_EXTENSIONS, m_strExtensions);
	DDX_Text(pDX, IDC_PLUGIN_ARGUMENTS, m_strArguments);
	DDX_Text(pDX, IDC_PLUGIN_PROCESSTYPE, m_strProcessType);
	DDX_Text(pDX, IDC_PLUGIN_MENUCAPTION, m_strMenuCaption);
	DDX_Text(pDX, IDC_PLUGIN_WINDOWTYPE, m_strWindowType);
	DDX_Check(pDX, IDC_PLUGIN_ARGUMENTSREQUIRED, m_bArgumentsRequired);
	DDX_Check(pDX, IDC_PLUGIN_GENERATESCRIPT, m_bGenerateEditorScript);
	DDX_Text(pDX, IDC_PLUGIN_PIPELINE, m_strPluginPipeline);
	DDX_Control(pDX, IDC_PLUGIN_TAB, m_ctlTab);
	DDX_Text(pDX, IDC_PLUGIN_COMMAND_LINE, m_strCommandline);
	DDX_Text(pDX, IDC_PLUGIN_SCRIPT_FILEEXTENSION, m_strScriptFileExtension);
	DDX_Text(pDX, IDC_PLUGIN_SCRIPT_BODY, m_strScriptBody);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditPluginDlg, CTrDialog)
	//{{AFX_MSG_MAP(CEditPluginDlg)
	ON_BN_CLICKED(IDC_PLUGIN_PIPELINE_MENU, OnBnClickedPipelineMenu)
	ON_BN_CLICKED(IDC_PLUGIN_COMMAND_LINE_MENU, OnBnClickedCommandLineMenu)
	ON_COMMAND_RANGE(ID_PLUGIN_PIPELINE_MACRO_FIRST, ID_PLUGIN_PIPELINE_MACRO_LAST, OnPluginPipelineMenu)
	ON_COMMAND_RANGE(ID_PLUGIN_COMMAND_LINE_MACRO_FIRST, ID_PLUGIN_COMMAND_LINE_MACRO_LAST, OnPluginCommandlineMenu)
	ON_CBN_SELCHANGE(IDC_PLUGIN_TYPE, OnSelchangePluginType)
	ON_NOTIFY(TCN_SELCHANGING, IDC_PLUGIN_TAB, OnTcnSelchangingTab)
	ON_NOTIFY(TCN_SELCHANGE, IDC_PLUGIN_TAB, OnTcnSelchangeTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditPluginDlg message handlers

void CEditPluginDlg::OnBnClickedPipelineMenu()
{
	ShowMenu(IDR_POPUP_PLUGIN_PIPELINE_MENU, IDC_PLUGIN_PIPELINE_MENU);
}

void CEditPluginDlg::OnBnClickedCommandLineMenu()
{
	ShowMenu(IDR_POPUP_PLUGIN_COMMAND_LINE_MENU, IDC_PLUGIN_COMMAND_LINE_MENU);
}

void CEditPluginDlg::OnPluginPipelineMenu(unsigned id)
{
	InsertMacro(IDR_POPUP_PLUGIN_PIPELINE_MENU, id, IDC_PLUGIN_PIPELINE);
}

void CEditPluginDlg::OnPluginCommandlineMenu(unsigned id)
{
	InsertMacro(IDR_POPUP_PLUGIN_COMMAND_LINE_MENU, id, IDC_PLUGIN_COMMAND_LINE);
}

void CEditPluginDlg::OnSelchangePluginType()
{
	UpdateControls();
}

void CEditPluginDlg::OnTcnSelchangingTab(NMHDR* pNMHDR, LRESULT* pResult)
{
	UpdateData(true);
	const int i = m_ctlTab.GetCurSel();
	m_strCommandlineAry[i] = m_strCommandline;
	m_strScriptFileExtensionAry[i] = m_strScriptFileExtension;
	m_strScriptBodyAry[i] = m_strScriptBody;
}

void CEditPluginDlg::OnTcnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult)
{
	const int i = m_ctlTab.GetCurSel();
	m_strCommandline = m_strCommandlineAry[i];
	m_strScriptFileExtension = m_strScriptFileExtensionAry[i];
	m_strScriptBody = m_strScriptBodyAry[i];
	UpdateData(false);
}

void CEditPluginDlg::OnOK()
{
	UpdateData(TRUE);
	const int cursel = m_ctlEvent.GetCurSel();
	m_info.m_event = reinterpret_cast<wchar_t*>(GetDlgItemDataCurSel(IDC_PLUGIN_TYPE));
	m_info.m_name = m_strPluginName;
	m_info.m_description = m_strDescription;
	m_info.m_fileFilters = m_strExtensions;
	m_info.m_arguments = m_strArguments;
	m_info.m_pipeline = m_strPluginPipeline;
	m_info.m_extendedProperties.clear();
	String& extendedProperties = m_info.m_extendedProperties;
	if (!m_strProcessType.empty())
		extendedProperties += _T("ProcessType=") + m_strProcessType + _T(";");
	if (!m_strMenuCaption.empty())
		extendedProperties += _T("MenuCaption=") + m_strMenuCaption + _T(";");
	if (!m_strWindowType.empty())
		extendedProperties += _T("PreferredWindowType=") + String(reinterpret_cast<wchar_t*>(GetDlgItemDataCurSel(IDC_PLUGIN_WINDOWTYPE))) + _T(";");
	if (m_bArgumentsRequired)
		extendedProperties += _T("ArgumentsRequired;");
	if (m_bGenerateEditorScript)
		extendedProperties += _T("GenerateEditorScript;");
	if (!m_info.m_extendedProperties.empty() && extendedProperties.back() == ';')
		extendedProperties.pop_back();

	if (cursel == URL_PACK_UNPACK || cursel == FILE_PACK_UNPACK)
	{
		SaveMethod(*m_info.m_unpackFile, 0);
		SaveMethod(*m_info.m_packFile, 1);
	}
	else if (cursel == FILE_FOLDER_PACK_UNPACK)
	{
		SaveMethod(*m_info.m_unpackFile, 0);
		SaveMethod(*m_info.m_packFile, 1);
		SaveMethod(*m_info.m_unpackFolder, 2);
		SaveMethod(*m_info.m_packFolder, 3);
		SaveMethod(*m_info.m_isFolder, 4);
	}
	else if (cursel == PREDIFF_FILE)
		SaveMethod(*m_info.m_prediffFile, 0);

	CTrDialog::OnOK();
}

BOOL CEditPluginDlg::OnInitDialog()
{
	CTrDialog::OnInitDialog();

	// setup handler for resizing this dialog	
	m_constraint.InitializeCurrentSize(this);
	m_constraint.SubclassWnd(); // install subclassing
	// persist size via registry
	m_constraint.LoadPosition(_T("ResizeableDialogs"), _T("EditPluginDlg"), false);

	SetDlgItemComboBoxList(IDC_PLUGIN_TYPE,
		{ 
			{ _("URL Handler"), L"URL_PACK_UNPACK" },
			{ _("File Unpacker"), L"FILE_PACK_UNPACK" }, 
			{ _("File or Folder Unpacker"), L"FILE_FOLDER_PACK_UNPACK" },
			{ _("Prediffer"), L"PREDIFF_FILE" }, 
			{ _("Alias for Unpacker"), L"ALIAS_PACK_UNPACK" },
			{ _("Alias for Prediffer"), L"ALIAS_PRDIFF" },
			{ _("Alias for Editor script"), L"ALIAS_EDITOR_SCRIPT" }
		}, m_strEvent);

	SetDlgItemComboBoxList(IDC_PLUGIN_WINDOWTYPE,
		{ 
			{ _("Unspecified"), L"" },
			{ _("Text"), L"Text" },
			{ _("Table"), L"Table" }, 
			{ _("Binary"), L"Binary" },
			{ _("Image"), L"Image" }, 
			{ _("Webpage"), L"Webpage" },
		}, m_strWindowType);

	UpdateControls();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

