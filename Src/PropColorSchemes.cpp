/** 
 * @file  PropColorSchemes.cpp
 *
 * @brief Implementation of PropColorSchemes propertysheet
 */

#include "stdafx.h"
#include "PropColorSchemes.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "IniOptionsMgr.h"
#include "OptionsPanel.h"
#include "SysColorHook.h"
#include "ColorSchemes.h"
#include "DarkModeLib.h"
#include "FileOrFolderSelect.h"
#include "OptionsDiffColors.h"
#include "OptionsDirColors.h"
#include "OptionsSyntaxColors.h"
#include "OptionsCustomColors.h"
#include "paths.h"
#include "SyntaxColors.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/** 
 * @brief Default constructor.
 */
PropColorSchemes::PropColorSchemes(COptionsMgr *optionsMgr)
 : OptionsPanel(optionsMgr, PropColorSchemes::IDD)
 , m_nColorMode(0)
{
	BindOption(OPT_COLOR_MODE, m_nColorMode, IDC_COLOR_MODE, DDX_CBIndex);
	BindOption(OPT_COLOR_SCHEME, m_sColorScheme, IDC_COLOR_SCHEME_LIGHT, DDX_CBString);
	BindOption(OPT_COLOR_SCHEME_DARK, m_sColorSchemeDark, IDC_COLOR_SCHEME_LIGHT, DDX_CBString);
}

void PropColorSchemes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropColorSchemes)
	//}}AFX_DATA_MAP
	DoDataExchangeBindOptions(pDX);
}


BEGIN_MESSAGE_MAP(PropColorSchemes, OptionsPanel)
	//{{AFX_MSG_MAP(PropColorSchemes)
	ON_CBN_SELCHANGE(IDC_COLOR_MODE, OnCbnSelchangeColorMode)
	ON_CBN_SELCHANGE(IDC_COLOR_SCHEME_LIGHT, OnCbnSelchangeColorScheme)
	ON_CBN_SELCHANGE(IDC_COLOR_SCHEME_DARK, OnCbnSelchangeColorScheme)
	ON_BN_CLICKED(IDC_COLOR_SCHEME_SAVE, OnSaveCurrentScheme)
	ON_BN_CLICKED(IDC_COLOR_SCHEME_DELETE, OnDeleteCurrentScheme)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 */
void PropColorSchemes::ReadOptions()
{
	ReadOptionBindings();
	if (!DarkMode::isExperimentalSupported())
		m_nColorMode = 0;
}

/** 
 * @brief Writes options values from UI to storage.
 */
void PropColorSchemes::WriteOptions()
{
	WriteOptionBindings();
	GetOptionsMgr()->SaveOption(OPT_COLOR_MODE_EFFECTIVE,
		WinMergeDarkMode::GetEffectiveColorMode(m_nColorMode));
}

/** 
 * @brief Called before propertysheet is drawn.
 */
BOOL PropColorSchemes::OnInitDialog()
{
	SetDlgItemComboBoxList(IDC_COLOR_MODE, { _("Light"), _("Dark"), _("Follow system") });

	ReloadColorSchemes();
	UpdateControls();

	OptionsPanel::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
}

void PropColorSchemes::ReloadColorSchemes()
{
	for (int id : { IDC_COLOR_SCHEME_LIGHT, IDC_COLOR_SCHEME_DARK })
	{
		CComboBox* combo = (CComboBox*)GetDlgItem(id);
		combo->ResetContent();
		for (auto& name : ColorSchemes::GetColorSchemeNames())
			combo->AddString(name.c_str());
	}
}

String PropColorSchemes::GetCurrentColorSchemePath() const
{
	const int colorMode = WinMergeDarkMode::GetEffectiveColorMode(m_nColorMode);
	const String colorScheme = colorMode == 0 ? m_sColorScheme : m_sColorSchemeDark;
	return ColorSchemes::GetColorSchemePath(colorScheme);
}

void PropColorSchemes::UpdateControls()
{
	EnableDlgItem(IDC_COLOR_SCHEME_LIGHT, m_nColorMode == 0 || m_nColorMode == 2);
	EnableDlgItem(IDC_COLOR_SCHEME_DARK, 
		DarkMode::isExperimentalSupported() && (m_nColorMode == 1 || m_nColorMode == 2));
	EnableDlgItem(IDC_COLOR_MODE, DarkMode::isExperimentalSupported());
	const bool canDelete = (strutils::compare_nocase(ColorSchemes::GetPrivateColorSchemesFolder(), paths::GetParentPath(GetCurrentColorSchemePath())) == 0);
	EnableDlgItem(IDC_COLOR_SCHEME_DELETE, canDelete);
}

void PropColorSchemes::UpdateColorScheme()
{
	DarkMode::DarkModeType dmType = WinMergeDarkMode::GetDarkModeType(m_nColorMode);
	String path = ColorSchemes::GetColorSchemePath(dmType == DarkMode::DarkModeType::dark ? m_sColorSchemeDark : m_sColorScheme);
	WriteOptions();
	SysColorHook::Unhook(AfxGetInstanceHandle());
	auto result = GetOptionsMgr()->ImportOptions(path);
	if (GetOptionsMgr()->GetBool(OPT_SYSCOLOR_HOOK_ENABLED))
		SysColorHook::Hook(AfxGetInstanceHandle());
	if (result != COption::OPT_OK)
	{
		I18n::MessageBox(IDS_OPT_IMPORT_ERR, MB_ICONWARNING);
		return;
	}
	GetParent()->GetParent()->PostMessage(WM_APP + IDC_COLOR_SCHEMES);
}

void PropColorSchemes::OnCbnSelchangeColorMode()
{
	UpdateData(TRUE);
	UpdateControls();
	UpdateColorScheme();
}

void PropColorSchemes::OnCbnSelchangeColorScheme()
{
	UpdateData(TRUE);
	UpdateControls();
	UpdateColorScheme();
}

void PropColorSchemes::OnSaveCurrentScheme()
{
	const String dir = ColorSchemes::GetPrivateColorSchemesFolder();
	paths::CreateIfNeeded(dir);
	String path;
	if (!SelectFile(GetSafeHwnd(), path, false, 
		dir.c_str(), _("Select file for export"),
		_("Options files (*.ini)|*.ini|All Files (*.*)|*.*||"), _T("ini")))
		return;
	CIniOptionsMgr optionsMgr(path);
	COLORSETTINGS diffColors;
	DIRCOLORSETTINGS dirColors;
	COLORREF customColors[16];
	SyntaxColors syntaxColors;
	Options::CustomColors::Init(&optionsMgr);
	Options::CustomColors::Load(GetOptionsMgr(), customColors);
	Options::CustomColors::Save(&optionsMgr, customColors);
	Options::DiffColors::Init(&optionsMgr);
	Options::DiffColors::Load(GetOptionsMgr(), diffColors);
	Options::DiffColors::Save(&optionsMgr, diffColors);
	Options::DirColors::Init(&optionsMgr);
	Options::DirColors::Load(GetOptionsMgr(), dirColors);
	Options::DirColors::Save(&optionsMgr, dirColors);
	Options::SyntaxColors::Init(&optionsMgr, &syntaxColors);
	Options::SyntaxColors::Load(GetOptionsMgr(), &syntaxColors);
	Options::SyntaxColors::Save(&optionsMgr, &syntaxColors);
	optionsMgr.InitOption(OPT_SYSCOLOR_HOOK_ENABLED, false);
	optionsMgr.SaveOption(OPT_SYSCOLOR_HOOK_ENABLED, GetOptionsMgr()->GetBool(OPT_SYSCOLOR_HOOK_ENABLED));
	optionsMgr.InitOption(OPT_SYSCOLOR_HOOK_COLORS, _T(""));
	optionsMgr.SaveOption(OPT_SYSCOLOR_HOOK_COLORS, GetOptionsMgr()->GetString(OPT_SYSCOLOR_HOOK_COLORS));

	ReloadColorSchemes();
	UpdateData(FALSE);
}

void PropColorSchemes::OnDeleteCurrentScheme()
{
	const String path = GetCurrentColorSchemePath();
	if (strutils::compare_nocase(ColorSchemes::GetPrivateColorSchemesFolder(), paths::GetParentPath(path)) != 0)
		return;
	const String sConfirm = strutils::format_string1(_("Are you sure you want to delete\n\n%1 ?"), path);
	const int res = AfxMessageBox(sConfirm.c_str(), MB_ICONWARNING | MB_YESNO);
	if (res != IDYES)
		return;
	if (!DeleteFile(path.c_str()))
	{
		const String sError = GetSysError();
		AfxMessageBox(sError.c_str(), MB_ICONWARNING);
		return;
	}
	ReloadColorSchemes();
	if (WinMergeDarkMode::GetEffectiveColorMode(m_nColorMode) == 0)
		m_sColorScheme = _T("Default");
	else
		m_sColorSchemeDark = _T("VS Dark");
	UpdateData(FALSE);
	WriteOptions();
	UpdateControls();
	UpdateColorScheme();
}
