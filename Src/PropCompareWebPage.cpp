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
	BindOption(OPT_CMP_WEB_URL_PATTERN_TO_INCLUDE, m_sURLPatternToInclude, IDC_COMPAREWEBPAGE_URLPATTERNTOINCLUDE, DDX_Text);
	BindOption(OPT_CMP_WEB_URL_PATTERN_TO_EXCLUDE, m_sURLPatternToExclude, IDC_COMPAREWEBPAGE_URLPATTERNTOEXCLUDE, DDX_Text);
	BindOption(OPT_CMP_WEB_USERDATAFOLDER_TYPE, m_nUserDataFolderType, IDC_COMPAREWEBPAGE_USERDATAFOLDER_TYPE, DDX_CBIndex);
	BindOption(OPT_CMP_WEB_USERDATAFOLDER_PERPANE, m_bUserDataFolderPerPane, IDC_COMPAREWEBPAGE_USERDATAFOLDER_PERPANE, DDX_Check);
}

void PropCompareWebPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropCompareWebPage)
	//}}AFX_DATA_MAP
	DoDataExchangeBindOptions(pDX);
}


BEGIN_MESSAGE_MAP(PropCompareWebPage, OptionsPanel)
	//{{AFX_MSG_MAP(PropCompareWebPage)
	ON_BN_CLICKED(IDC_COMPARE_DEFAULTS, OnDefaults)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Called before propertysheet is drawn.
 */
BOOL PropCompareWebPage::OnInitDialog()
{
	SetDlgItemComboBoxList(IDC_COMPAREWEBPAGE_USERDATAFOLDER_TYPE,
		{ _("AppData folder"), _("Install folder") });

	OptionsPanel::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
}
