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
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropColorSchemes, OptionsPanel)
	//{{AFX_MSG_MAP(PropColorSchemes)
	ON_CBN_SELCHANGE(IDC_COLOR_SCHEMES, OnCbnSelchangeColorSchemes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 */
void PropColorSchemes::ReadOptions()
{
	m_sColorScheme = GetOptionsMgr()->GetString(OPT_COLOR_SCHEME);
}

/** 
 * @brief Writes options values from UI to storage.
 */
void PropColorSchemes::WriteOptions()
{
	GetOptionsMgr()->SaveOption(OPT_COLOR_SCHEME, m_sColorScheme);
}

/** 
 * @brief Called before propertysheet is drawn.
 */
BOOL PropColorSchemes::OnInitDialog()
{
	CComboBox * combo = (CComboBox*) GetDlgItem(IDC_COLOR_SCHEMES);

	for (auto& name : ColorSchemes::GetColorSchemeNames())
	{
		combo->AddString(name.c_str());
		if (strutils::compare_nocase(name, m_sColorScheme) == 0)
			combo->SetCurSel(combo->GetCount() - 1);
	}
	if (combo->GetCurSel() == -1 && combo->GetCount() > 0)
		combo->SetCurSel(0);

	OptionsPanel::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
}

void PropColorSchemes::OnCbnSelchangeColorSchemes()
{
	String sColorScheme;
	GetDlgItemText(IDC_COLOR_SCHEMES, sColorScheme);
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

