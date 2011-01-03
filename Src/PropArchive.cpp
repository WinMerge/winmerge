/** 
 * @file  PropArchive.cpp
 *
 * @brief Implementation of PropArchive propertysheet
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include "Constants.h"
#include "Merge.h"
#include "PropArchive.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// PropArchive dialog

PropArchive::PropArchive(COptionsMgr *optionsMgr)
: OptionsPanel(optionsMgr, PropArchive::IDD)
, m_bEnableSupport(false)
, m_nInstallType(0)
, m_bProbeType(false)
{
}

/** 
 * @brief Sets update handlers for dialog controls.
 */
void PropArchive::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ARCHIVE_WWW, m_wwwLink);
	DDX_Check(pDX, IDC_ARCHIVE_ENABLE, m_bEnableSupport);
	DDX_Radio(pDX, IDC_ARCHIVE_INSTALSTANDALONE, m_nInstallType);
	DDX_Check(pDX, IDC_ARCHIVE_DETECTTYPE, m_bProbeType);
}


BEGIN_MESSAGE_MAP(PropArchive, CPropertyPage)
	ON_BN_CLICKED(IDC_ARCHIVE_ENABLE, OnEnableClicked)
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 */
void PropArchive::ReadOptions()
{
	int enable = GetOptionsMgr()->GetInt(OPT_ARCHIVE_ENABLE);
	m_bEnableSupport = enable > 0;
	m_nInstallType = enable > 1 ? enable - 1 : 0;
	m_bProbeType = GetOptionsMgr()->GetBool(OPT_ARCHIVE_PROBETYPE);
}

/** 
 * @brief Writes options values from UI to storage.
 */
void PropArchive::WriteOptions()
{
	if (m_bEnableSupport)
		GetOptionsMgr()->SaveOption(OPT_ARCHIVE_ENABLE, m_nInstallType + 1);
	else
		GetOptionsMgr()->SaveOption(OPT_ARCHIVE_ENABLE, (int)0);
	GetOptionsMgr()->SaveOption(OPT_ARCHIVE_PROBETYPE, m_bProbeType == TRUE);
}

/** 
 * @brief Called before propertysheet is drawn.
 */
BOOL PropArchive::OnInitDialog()
{
	theApp.TranslateDialog(m_hWnd);
	m_wwwLink.m_link = DownloadUrl;
	UpdateData(FALSE);

	UpdateControls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
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
	CButton *chkEnabled = (CButton *) GetDlgItem(IDC_ARCHIVE_ENABLE);
	CButton *btnLocal = (CButton *) GetDlgItem(IDC_ARCHIVE_INSTALLOCAL);
	CButton *btnStandAlone = (CButton *) GetDlgItem(IDC_ARCHIVE_INSTALSTANDALONE);
	CButton *chkProbe = (CButton *) GetDlgItem(IDC_ARCHIVE_DETECTTYPE);

	BOOL enableItems = FALSE;
	if (chkEnabled->GetCheck() == 1)
		enableItems = TRUE;
	btnLocal->EnableWindow(enableItems);
	btnStandAlone->EnableWindow(enableItems);
	chkProbe->EnableWindow(enableItems);
}
