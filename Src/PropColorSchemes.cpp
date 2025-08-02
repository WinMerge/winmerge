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

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/** 
 * @brief Default constructor.
 */
PropColorSchemes::PropColorSchemes(COptionsMgr *optionsMgr)
 : OptionsPanel(optionsMgr, PropColorSchemes::IDD)
{
}

void PropColorSchemes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropColorSchemes)
	DDX_CBIndex(pDX, IDC_COLOR_MODE, m_nColorMode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropColorSchemes, OptionsPanel)
	//{{AFX_MSG_MAP(PropColorSchemes)
	ON_CBN_SELCHANGE(IDC_COLOR_MODE, OnCbnSelchangeColorMode)
	ON_CBN_SELCHANGE(IDC_COLOR_SCHEME_LIGHT, OnCbnSelchangeColorSchemeLight)
	ON_CBN_SELCHANGE(IDC_COLOR_SCHEME_DARK, OnCbnSelchangeColorSchemeDark)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 */
void PropColorSchemes::ReadOptions()
{
	m_nColorMode = GetOptionsMgr()->GetInt(OPT_COLOR_MODE);
	m_sColorScheme = GetOptionsMgr()->GetString(OPT_COLOR_SCHEME);
	m_sColorSchemeDark = GetOptionsMgr()->GetString(OPT_COLOR_SCHEME_DARK);
}

/** 
 * @brief Writes options values from UI to storage.
 */
void PropColorSchemes::WriteOptions()
{
	GetOptionsMgr()->SaveOption(OPT_COLOR_MODE, m_nColorMode);
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
		String scheme = (id == IDC_COLOR_SCHEME_LIGHT) ? m_sColorScheme : m_sColorSchemeDark;

		for (auto& name : ColorSchemes::GetColorSchemeNames())
		{
			combo->AddString(name.c_str());
			if (strutils::compare_nocase(name, scheme) == 0)
				combo->SetCurSel(combo->GetCount() - 1);
		}
		if (combo->GetCurSel() == -1 && combo->GetCount() > 0)
			combo->SetCurSel(0);
	}

	OptionsPanel::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
}

void PropColorSchemes::OnCbnSelchangeColorMode()
{
}

void PropColorSchemes::OnCbnSelchangeColorSchemeLight()
{
	OnCbnSelchangeColorScheme(IDC_COLOR_SCHEME_LIGHT);
}

void PropColorSchemes::OnCbnSelchangeColorSchemeDark()
{
	OnCbnSelchangeColorScheme(IDC_COLOR_SCHEME_DARK);
}

void PropColorSchemes::OnCbnSelchangeColorScheme(int id)
{
	String sColorScheme;
	GetDlgItemText(id, sColorScheme);
	m_sColorScheme = sColorScheme;
	WriteOptions();
	String path = ColorSchemes::GetColorSchemePath(sColorScheme);
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

