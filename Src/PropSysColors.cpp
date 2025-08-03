/** 
 * @file  PropSysColors.cpp
 *
 * @brief Implementation of PropSysColors propertysheet
 */

#include "stdafx.h"
#include "PropSysColors.h"
#include "SysColorHook.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsInit.h"
#include "OptionsPanel.h"
#include "ColorSchemes.h"
#include "IniOptionsMgr.h"
#include "Merge.h"
#include "MyColorDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/** 
 * @brief Default constructor.
 */
PropSysColors::PropSysColors(COptionsMgr *optionsMgr)
 : OptionsPanel(optionsMgr, PropSysColors::IDD)
 , m_bEnableSysColorHook(false)
 , m_cSysColors()
{
}

void PropSysColors::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropSysColors)
	DDX_Check(pDX, IDC_SYSCOLOR_HOOK_ENABLED, m_bEnableSysColorHook);
	DDX_Control(pDX, IDC_SYSCOLOR_NAME, m_cboSysColorName);
	DDX_Control(pDX, IDC_SYSCOLOR, m_btnSysColor);
	UpdateControls();
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropSysColors, OptionsPanel)
	//{{AFX_MSG_MAP(PropSysColors)
	ON_BN_CLICKED(IDC_SYSCOLOR_HOOK_ENABLED, OnBnClickedSysColorHookEnabled)
	ON_CBN_SELCHANGE(IDC_SYSCOLOR_NAME, OnCbnSelchangeSysColorName)
	ON_BN_CLICKED(IDC_SYSCOLOR, OnBnClickedSysColor)
	ON_BN_CLICKED(IDC_COMPARE_DEFAULTS, OnDefaults)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 * (Property sheet calls this before displaying all property pages)
 */
void PropSysColors::ReadOptions()
{
	m_bEnableSysColorHook = GetOptionsMgr()->GetBool(OPT_SYSCOLOR_HOOK_ENABLED) ? true : false;
	m_oldColors = GetOptionsMgr()->GetString(OPT_SYSCOLOR_HOOK_COLORS);
	for (int i = 0; i < static_cast<int>(m_cSysColors.size()); i++)
		m_cSysColors[i] = SysColorHook::GetOrgSysColor(i);
	SysColorHook::Deserialize(m_oldColors, m_cSysColors.data());
}

/** 
 * @brief Writes options values from UI to storage.
 * (Property sheet calls this after displaying all property pages)
 */
void PropSysColors::WriteOptions()
{
	for (int i = 0; i < static_cast<int>(m_cSysColors.size()); i++)
		SysColorHook::SetSysColor(i, m_cSysColors[i]);
	String newColors = SysColorHook::Serialize();
	GetOptionsMgr()->SaveOption(OPT_SYSCOLOR_HOOK_ENABLED, m_bEnableSysColorHook);
	GetOptionsMgr()->SaveOption(OPT_SYSCOLOR_HOOK_COLORS, newColors);
}

/** 
 * @brief Let user browse common color dialog, and select a color
 */
void PropSysColors::BrowseColor(CColorButton & colorButton)
{
	CMyColorDialog dialog(colorButton.GetColor());
	if (dialog.DoModal() == IDOK)
	{
		colorButton.SetColor(dialog.GetColor());
		const int i = tc::ttoi(reinterpret_cast<const wchar_t*>(GetDlgItemDataCurSel(IDC_SYSCOLOR_NAME)));
		m_cSysColors[i] = dialog.GetColor();
	}
}

/** 
 * @brief Called Updates controls enabled/disables state.
 */
void PropSysColors::UpdateControls()
{
	const bool enabled = IsDlgButtonChecked(IDC_SYSCOLOR_HOOK_ENABLED);
	EnableDlgItem(IDC_SYSCOLOR_NAME, enabled);
	EnableDlgItem(IDC_SYSCOLOR, enabled);
	auto data = reinterpret_cast<intptr_t>(GetDlgItemDataCurSel(IDC_SYSCOLOR_NAME));
	const int i = (data != -1) ? tc::ttoi(reinterpret_cast<const wchar_t*>(data)) : 0;
	m_btnSysColor.SetColor(m_cSysColors[i]);
}

/** 
 * @brief Called before propertysheet is drawn.
 */
BOOL PropSysColors::OnInitDialog()
{
	OptionsPanel::OnInitDialog();

	SetDlgItemComboBoxList(IDC_SYSCOLOR_NAME,
		{
//			{ _T("COLOR_SCROLLBAR"),              L"0" },
//			{ _T("COLOR_DESKTOP"),                L"1" },
			{ _T("COLOR_ACTIVECAPTION"),          L"2" },
			{ _T("COLOR_INACTIVECAPTION"),        L"3" },
//			{ _T("COLOR_MENU"),                   L"4" },
			{ _T("COLOR_WINDOW"),                 L"5" },
//			{ _T("COLOR_WINDOWFRAME"),            L"6" },
//			{ _T("COLOR_MENUTEXT"),               L"7" },
			{ _T("COLOR_WINDOWTEXT"),             L"8" },
			{ _T("COLOR_CAPTIONTEXT"),            L"9" },
//			{ _T("COLOR_ACTIVEBORDER"),           L"10" },
//			{ _T("COLOR_INACTIVEBORDER"),         L"11" },
			{ _T("COLOR_APPWORKSPACE"),           L"12" },
			{ _T("COLOR_HIGHLIGHT"),              L"13" },
			{ _T("COLOR_HIGHLIGHTTEXT"),          L"14" },
			{ _T("COLOR_3DFACE"),                 L"15" },
			{ _T("COLOR_3DSHADOW"),               L"16" },
//			{ _T("COLOR_GRAYTEXT"),               L"17" },
			{ _T("COLOR_BTNTEXT"),                L"18" },
			{ _T("COLOR_INACTIVECAPTIONTEXT"),    L"19" },
			{ _T("COLOR_3DHIGHLIGHT"),            L"20" },
			{ _T("COLOR_3DDKSHADOW"),             L"21" },
//			{ _T("COLOR_3DLIGHT"),                L"22" },
//			{ _T("COLOR_INFOTEXT"),               L"23" },
//			{ _T("COLOR_INFOBK"),                 L"24" },
//			{ _T("COLOR_HOTLIGHT"),               L"26" },
			{ _T("COLOR_GRADIENTACTIVECAPTION"),  L"27" },
			{ _T("COLOR_GRADIENTINACTIVECAPTION"),L"28" },
//			{ _T("COLOR_MENUHILIGHT"),            L"29" },
//			{ _T("COLOR_MENUBAR"),                L"30" },
		}, L"2");

	OnCbnSelchangeSysColorName();
	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void PropSysColors::OnBnClickedSysColorHookEnabled()
{
	UpdateControls();
}

void PropSysColors::OnCbnSelchangeSysColorName()
{
	UpdateControls();
}

/** 
 * @brief User wants to change equal color
 */
void PropSysColors::OnBnClickedSysColor()
{
	BrowseColor(m_btnSysColor);
}

void PropSysColors::OnDefaults()
{
	for (int i = 0; i < static_cast<int>(m_cSysColors.size()); i++)
		m_cSysColors[i] = SysColorHook::GetOrgSysColor(i);
	String path = ColorSchemes::GetColorSchemePath(GetOptionsMgr()->GetString(OPT_COLOR_SCHEME));
	if (path.empty())
	{
		m_bEnableSysColorHook = GetOptionsMgr()->GetDefault<bool>(OPT_SYSCOLOR_HOOK_ENABLED);
	}
	else
	{
		CIniOptionsMgr iniOptions(path);
		Options::Init(&iniOptions);
		SysColorHook::Deserialize(iniOptions.GetString(OPT_SYSCOLOR_HOOK_COLORS), m_cSysColors.data());
		m_bEnableSysColorHook = iniOptions.GetBool(OPT_SYSCOLOR_HOOK_ENABLED);
	}
	UpdateData(false);
	UpdateControls();
}
