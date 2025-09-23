/** 
 * @file  PropArchive.cpp
 *
 * @brief Implementation of PropArchive propertysheet
 */

#include "stdafx.h"
#include "PropArchive.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// PropArchive dialog

PropArchive::PropArchive(COptionsMgr *optionsMgr)
: OptionsPanel(optionsMgr, PropArchive::IDD)
, m_bEnableSupport(false)
, m_bProbeType(false)
{
	BindOption(OPT_ARCHIVE_ENABLE, m_bEnableSupport, IDC_ARCHIVE_ENABLE, DDX_Check);
	BindOption(OPT_ARCHIVE_PROBETYPE, m_bProbeType, IDC_ARCHIVE_DETECTTYPE, DDX_Check);
}

/** 
 * @brief Sets update handlers for dialog controls.
 */
void PropArchive::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DoDataExchangeBindOptions(pDX);
	UpdateControls();
}


BEGIN_MESSAGE_MAP(PropArchive, OptionsPanel)
	ON_BN_CLICKED(IDC_COMPARE_DEFAULTS, OnDefaults)
	ON_BN_CLICKED(IDC_ARCHIVE_ENABLE, OnEnableClicked)
END_MESSAGE_MAP()

/** 
 * @brief Called when archive support is enabled or disabled.
 */
void PropArchive::OnEnableClicked()
{
	UpdateControls();
}

/** 
 * @brief Called Updates controls enabled/disables state.
 */
void PropArchive::UpdateControls()
{
	EnableDlgItem(IDC_ARCHIVE_DETECTTYPE, IsDlgButtonChecked(IDC_ARCHIVE_ENABLE) == 1);
}
