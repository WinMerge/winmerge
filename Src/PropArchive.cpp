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
}

/** 
 * @brief Sets update handlers for dialog controls.
 */
void PropArchive::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_ARCHIVE_ENABLE, m_bEnableSupport);
	DDX_Check(pDX, IDC_ARCHIVE_DETECTTYPE, m_bProbeType);
	UpdateControls();
}


BEGIN_MESSAGE_MAP(PropArchive, OptionsPanel)
	ON_BN_CLICKED(IDC_COMPARE_DEFAULTS, OnDefaults)
	ON_BN_CLICKED(IDC_ARCHIVE_ENABLE, OnEnableClicked)
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 */
void PropArchive::ReadOptions()
{
	m_bEnableSupport = GetOptionsMgr()->GetBool(OPT_ARCHIVE_ENABLE);
	m_bProbeType = GetOptionsMgr()->GetBool(OPT_ARCHIVE_PROBETYPE);
}

/** 
 * @brief Writes options values from UI to storage.
 */
void PropArchive::WriteOptions()
{
	GetOptionsMgr()->SaveOption(OPT_ARCHIVE_ENABLE, m_bEnableSupport);
	GetOptionsMgr()->SaveOption(OPT_ARCHIVE_PROBETYPE, m_bProbeType);
}

/**
 * @brief Sets options to defaults
 */
void PropArchive::OnDefaults()
{
	m_bEnableSupport = GetOptionsMgr()->GetDefault<bool>(OPT_ARCHIVE_ENABLE);
	m_bProbeType = GetOptionsMgr()->GetDefault<bool>(OPT_ARCHIVE_PROBETYPE);
	UpdateData(FALSE);
}

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
