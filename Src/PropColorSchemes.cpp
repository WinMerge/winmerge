/** 
 * @file  PropColorSchemes.cpp
 *
 * @brief Implementation of PropColorSchemes propertysheet
 */

#include "stdafx.h"
#include "PropColorSchemes.h"
#include "OptionsDef.h"
#include "OptionsInit.h"
#include "OptionsMgr.h"
#include "RegOptionsMgr.h"
#include "OptionsPanel.h"
#include "SysColorHook.h"
#include "ColorSchemes.h"
#include "MergeDarkMode.h"

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
}

void PropColorSchemes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropColorSchemes)
	DDX_CBIndex(pDX, IDC_COLOR_MODE, m_nColorMode);
	DDX_CBString(pDX, IDC_COLOR_SCHEME_LIGHT, m_sColorScheme);
	DDX_CBString(pDX, IDC_COLOR_SCHEME_DARK, m_sColorSchemeDark);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropColorSchemes, OptionsPanel)
	//{{AFX_MSG_MAP(PropColorSchemes)
	ON_CBN_SELCHANGE(IDC_COLOR_MODE, OnCbnSelchangeColorMode)
	ON_CBN_SELCHANGE(IDC_COLOR_SCHEME_LIGHT, OnCbnSelchangeColorScheme)
	ON_CBN_SELCHANGE(IDC_COLOR_SCHEME_DARK, OnCbnSelchangeColorScheme)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 */
void PropColorSchemes::ReadOptions()
{
	m_nColorMode = WinMergeDarkMode::IsDarkModeAvailable() ?
		GetOptionsMgr()->GetInt(OPT_COLOR_MODE) : 0;
	m_sColorScheme = GetOptionsMgr()->GetString(OPT_COLOR_SCHEME);
	m_sColorSchemeDark = GetOptionsMgr()->GetString(OPT_COLOR_SCHEME_DARK);
}

/** 
 * @brief Writes options values from UI to storage.
 */
void PropColorSchemes::WriteOptions()
{
	GetOptionsMgr()->SaveOption(OPT_COLOR_MODE, m_nColorMode);
	GetOptionsMgr()->SaveOption(OPT_COLOR_MODE_EFFECTIVE,
		WinMergeDarkMode::GetEffectiveColorMode(m_nColorMode));
	GetOptionsMgr()->SaveOption(OPT_COLOR_SCHEME, m_sColorScheme);
	GetOptionsMgr()->SaveOption(OPT_COLOR_SCHEME_DARK, m_sColorSchemeDark);
}

/** 
 * @brief Called before propertysheet is drawn.
 */
BOOL PropColorSchemes::OnInitDialog()
{
	SetDlgItemComboBoxList(IDC_COLOR_MODE, { _("Light"), _("Dark"), _("Follow system") });

	for (int id : { IDC_COLOR_SCHEME_LIGHT, IDC_COLOR_SCHEME_DARK })
	{
		CComboBox* combo = (CComboBox*)GetDlgItem(id);

		for (auto& name : ColorSchemes::GetColorSchemeNames())
			combo->AddString(name.c_str());
	}

	UpdateControls();

	OptionsPanel::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
}


void PropColorSchemes::UpdateControls()
{
	EnableDlgItem(IDC_COLOR_SCHEME_LIGHT, m_nColorMode == 0 || m_nColorMode == 2);
	EnableDlgItem(IDC_COLOR_SCHEME_DARK, 
		WinMergeDarkMode::IsDarkModeAvailable() && (m_nColorMode == 1 || m_nColorMode == 2));
	EnableDlgItem(IDC_COLOR_MODE, WinMergeDarkMode::IsDarkModeAvailable());
}

void PropColorSchemes::UpdateColorScheme()
{
#if defined(USE_DARKMODELIB)
	DarkMode::DarkModeType dmType = WinMergeDarkMode::GetDarkModeType(m_nColorMode);
	String path = ColorSchemes::GetColorSchemePath(dmType == DarkMode::DarkModeType::dark ? m_sColorSchemeDark : m_sColorScheme);
#else
	String path = ColorSchemes::GetColorSchemePath(m_sColorScheme);
#endif
	WriteOptions();
	SysColorHook::Unhook(AfxGetInstanceHandle());
	auto result = GetOptionsMgr()->ImportOptions(path);
	if (GetOptionsMgr()->GetBool(OPT_SYSCOLOR_HOOK_ENABLED))
		SysColorHook::Hook(AfxGetInstanceHandle());
	if (result != COption::OPT_OK)
	{
		LangMessageBox(IDS_OPT_IMPORT_ERR, MB_ICONWARNING);
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
	UpdateColorScheme();
}

