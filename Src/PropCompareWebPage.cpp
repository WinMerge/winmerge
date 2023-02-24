/** 
 * @file  PropCompareWebPage.cpp
 *
 * @brief Implementation of PropCompareWebPage propertysheet
 */

#include "stdafx.h"
#include "PropCompareWebPage.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/** 
 * @brief Constructor.
 * @param [in] optionsMgr Pointer to COptionsMgr.
 */
PropCompareWebPage::PropCompareWebPage(COptionsMgr *optionsMgr) 
 : OptionsPanel(optionsMgr, PropCompareWebPage::IDD)
 , m_nUserDataFolderType(0)
 , m_bUserDataFolderPerPane(true)
{
}

void PropCompareWebPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropCompareWebPage)
	DDX_Text(pDX, IDC_COMPAREWEBPAGE_URLPATTERNTOINCLUDE, m_sURLPatternToInclude);
	DDX_Text(pDX, IDC_COMPAREWEBPAGE_URLPATTERNTOEXCLUDE, m_sURLPatternToExclude);
	DDX_CBIndex(pDX, IDC_COMPAREWEBPAGE_USERDATAFOLDER_TYPE, m_nUserDataFolderType);
	DDX_Check(pDX, IDC_COMPAREWEBPAGE_USERDATAFOLDER_PERPANE, m_bUserDataFolderPerPane);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropCompareWebPage, OptionsPanel)
	//{{AFX_MSG_MAP(PropCompareWebPage)
	ON_BN_CLICKED(IDC_COMPARE_DEFAULTS, OnDefaults)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 * Property sheet calls this before displaying GUI to load values
 * into members.
 */
void PropCompareWebPage::ReadOptions()
{
	m_sURLPatternToInclude = GetOptionsMgr()->GetString(OPT_CMP_WEB_URL_PATTERN_TO_INCLUDE);
	m_sURLPatternToExclude = GetOptionsMgr()->GetString(OPT_CMP_WEB_URL_PATTERN_TO_EXCLUDE);
	m_nUserDataFolderType = GetOptionsMgr()->GetInt(OPT_CMP_WEB_USERDATAFOLDER_TYPE);
	m_bUserDataFolderPerPane = GetOptionsMgr()->GetBool(OPT_CMP_WEB_USERDATAFOLDER_PERPANE);
}

/** 
 * @brief Writes options values from UI to storage.
 * Property sheet calls this after dialog is closed with OK button to
 * store values in member variables.
 */
void PropCompareWebPage::WriteOptions()
{
	GetOptionsMgr()->SaveOption(OPT_CMP_WEB_URL_PATTERN_TO_INCLUDE, m_sURLPatternToInclude);
	GetOptionsMgr()->SaveOption(OPT_CMP_WEB_URL_PATTERN_TO_EXCLUDE, m_sURLPatternToExclude);
	GetOptionsMgr()->SaveOption(OPT_CMP_WEB_USERDATAFOLDER_TYPE, m_nUserDataFolderType);
	GetOptionsMgr()->SaveOption(OPT_CMP_WEB_USERDATAFOLDER_PERPANE, m_bUserDataFolderPerPane);
}

/** 
 * @brief Called before propertysheet is drawn.
 */
BOOL PropCompareWebPage::OnInitDialog()
{
	CComboBox * combo = (CComboBox*) GetDlgItem(IDC_COMPAREWEBPAGE_USERDATAFOLDER_TYPE);
	for (const auto& item : { _("AppData folder"), _("Install folder") })
		combo->AddString(item.c_str());
	combo->SetCurSel(m_nUserDataFolderType);

	OptionsPanel::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
}

/** 
 * @brief Sets options to defaults
 */
void PropCompareWebPage::OnDefaults()
{
	m_sURLPatternToInclude = GetOptionsMgr()->GetDefault<String>(OPT_CMP_WEB_URL_PATTERN_TO_INCLUDE);
	m_sURLPatternToExclude = GetOptionsMgr()->GetDefault<String>(OPT_CMP_WEB_URL_PATTERN_TO_EXCLUDE);
	m_bUserDataFolderPerPane = GetOptionsMgr()->GetDefault<bool>(OPT_CMP_WEB_USERDATAFOLDER_PERPANE);
	m_nUserDataFolderType = GetOptionsMgr()->GetDefault<unsigned>(OPT_CMP_WEB_USERDATAFOLDER_TYPE);
	UpdateData(FALSE);
}

