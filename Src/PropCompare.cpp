/** 
 * @file  PropColors.cpp
 *
 * @brief Implementation of CPropCompare propertysheet
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "merge.h"
#include "PropCompare.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropCompare property page

/** 
 * @brief Constructor
 */
CPropCompare::CPropCompare(COptionsMgr *optionsMgr) : CPropertyPage(CPropCompare::IDD)
{
	//{{AFX_DATA_INIT(CPropCompare)
	m_compareMethod = -1;
	m_bIgnoreCase = FALSE;
	m_bIgnoreBlankLines = FALSE;
	m_bEolSensitive = FALSE;
	m_nIgnoreWhite = -1;
	m_bMovedBlocks = FALSE;
	//}}AFX_DATA_INIT

	m_pOptionsMgr = optionsMgr;
}

void CPropCompare::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropCompare)
	DDX_CBIndex(pDX, IDC_COMPAREMETHODCOMBO, m_compareMethod);
	DDX_Check(pDX, IDC_IGNCASE_CHECK, m_bIgnoreCase);
	DDX_Check(pDX, IDC_IGNBLANKS_CHECK, m_bIgnoreBlankLines);
	DDX_Check(pDX, IDC_EOL_SENSITIVE, m_bEolSensitive);
	DDX_Radio(pDX, IDC_WHITESPACE, m_nIgnoreWhite);
	DDX_Check(pDX, IDC_MOVED_BLOCKS, m_bMovedBlocks);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropCompare, CPropertyPage)
	//{{AFX_MSG_MAP(CPropCompare)
	ON_BN_CLICKED(IDC_COMPARE_DEFAULTS, OnDefaults)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropCompare message handlers

/** 
 * @brief Called before propertysheet is drawn.
 */
BOOL CPropCompare::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	CComboBox * combo = (CComboBox*) GetDlgItem(IDC_COMPAREMETHODCOMBO);

	CString item;
	VERIFY(item.LoadString(IDS_COMPMETHOD_CONTENTS));
	combo->AddString(item);
	VERIFY(item.LoadString(IDS_COMPMETHOD_MODDATE));
	combo->AddString(item);
	combo->SetCurSel(m_compareMethod);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/** 
 * @brief Sets options to defaults
 */
void CPropCompare::OnDefaults()
{
	DWORD tmp;
	m_pOptionsMgr->GetDefault(OPT_CMP_METHOD, tmp);
	m_compareMethod = tmp;
	m_pOptionsMgr->GetDefault(OPT_CMP_IGNORE_WHITESPACE, tmp);
	m_nIgnoreWhite = tmp;
	m_pOptionsMgr->GetDefault(OPT_CMP_EOL_SENSITIVE, tmp);
	m_bEolSensitive = tmp;
	m_pOptionsMgr->GetDefault(OPT_CMP_IGNORE_BLANKLINES, tmp);
	m_bIgnoreBlankLines = tmp;
	m_pOptionsMgr->GetDefault(OPT_CMP_IGNORE_CASE, tmp);
	m_bIgnoreCase = tmp;
	m_pOptionsMgr->GetDefault(OPT_CMP_MOVED_BLOCKS, tmp);
	m_bMovedBlocks = tmp;
	UpdateData(FALSE);
}
