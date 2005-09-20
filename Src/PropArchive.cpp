/** 
 * @file  PropArchive.cpp
 *
 * @brief Implementation of CPropArchive propertysheet
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "Merge.h"
#include "PropArchive.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CPropArchive dialog

IMPLEMENT_DYNAMIC(CPropArchive, CPropertyPage)
CPropArchive::CPropArchive()
	: CPropertyPage(CPropArchive::IDD)
	, m_bEnableSupport(false)
	, m_nInstallType(0)
	, m_bProbeType(false)
{
}

CPropArchive::~CPropArchive()
{
}

/** 
 * @brief Sets update handlers for dialog controls.
 */
void CPropArchive::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ARCHIVE_WWW, m_wwwLink);
	DDX_Check(pDX, IDC_ARCHIVE_ENABLE, m_bEnableSupport);
	DDX_Radio(pDX, IDC_ARCHIVE_INSTALSTANDALONE, m_nInstallType);
	DDX_Check(pDX, IDC_ARCHIVE_DETECTTYPE, m_bProbeType);
}


BEGIN_MESSAGE_MAP(CPropArchive, CPropertyPage)
	ON_BN_CLICKED(IDC_ARCHIVE_ENABLE, OnEnableClicked)
END_MESSAGE_MAP()


/** 
 * @brief Called before propertysheet is drawn.
 */
BOOL CPropArchive::OnInitDialog()
{
	m_wwwLink.m_link = _T("http://winmerge.org/downloads.php");
	UpdateData(FALSE);

	UpdateControls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/** 
 * @brief Called when archive support is enabled or disabled.
 */
void CPropArchive::OnEnableClicked()
{
	UpdateControls();
}

/** 
 * @brief Called Updates controls enabled/disables state.
 */
void CPropArchive::UpdateControls()
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
