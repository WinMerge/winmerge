/** 
 * @file  PropCompareFolder.cpp
 *
 * @brief Implementation of PropCompareFolder propertysheet
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include "merge.h"
#include "PropCompareFolder.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/** 
 * @brief Constructor.
 * @param [in] optionsMgr Pointer to COptionsMgr.
 */
PropCompareFolder::PropCompareFolder(COptionsMgr *optionsMgr) 
 : OptionsPanel(optionsMgr, PropCompareFolder::IDD)
 , m_compareMethod(-1)
 , m_bStopAfterFirst(FALSE)
 , m_bIgnoreSmallTimeDiff(FALSE)
 , m_bIncludeUniqFolders(TRUE)
{
}

void PropCompareFolder::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropCompareFolder)
	DDX_CBIndex(pDX, IDC_COMPAREMETHODCOMBO, m_compareMethod);
	DDX_Check(pDX, IDC_COMPARE_STOPFIRST, m_bStopAfterFirst);
	DDX_Check(pDX, IDC_IGNORE_SMALLTIMEDIFF, m_bIgnoreSmallTimeDiff);
	DDX_Check(pDX, IDC_COMPARE_WALKSUBDIRS, m_bIncludeUniqFolders);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropCompareFolder, CPropertyPage)
	//{{AFX_MSG_MAP(PropCompareFolder)
	ON_BN_CLICKED(IDC_COMPAREFOLDER_DEFAULTS, OnDefaults)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_COMPAREMETHODCOMBO, OnCbnSelchangeComparemethodcombo)
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 * Property sheet calls this before displaying GUI to load values
 * into members.
 */
void PropCompareFolder::ReadOptions()
{
	m_compareMethod = GetOptionsMgr()->GetInt(OPT_CMP_METHOD);
	m_bStopAfterFirst = GetOptionsMgr()->GetBool(OPT_CMP_STOP_AFTER_FIRST);
	m_bIgnoreSmallTimeDiff = GetOptionsMgr()->GetBool(OPT_IGNORE_SMALL_FILETIME);
	m_bIncludeUniqFolders = GetOptionsMgr()->GetBool(OPT_CMP_WALK_UNIQUE_DIRS);
}

/** 
 * @brief Writes options values from UI to storage.
 * Property sheet calls this after dialog is closed with OK button to
 * store values in member variables.
 */
void PropCompareFolder::WriteOptions()
{
	GetOptionsMgr()->SaveOption(OPT_CMP_METHOD, (int)m_compareMethod);
	GetOptionsMgr()->SaveOption(OPT_CMP_STOP_AFTER_FIRST, m_bStopAfterFirst == TRUE);
	GetOptionsMgr()->SaveOption(OPT_IGNORE_SMALL_FILETIME, m_bIgnoreSmallTimeDiff == TRUE);
	GetOptionsMgr()->SaveOption(OPT_CMP_WALK_UNIQUE_DIRS, m_bIncludeUniqFolders == TRUE);
}

/** 
 * @brief Called before propertysheet is drawn.
 */
BOOL PropCompareFolder::OnInitDialog() 
{
	theApp.TranslateDialog(m_hWnd);
	CPropertyPage::OnInitDialog();
	CComboBox * combo = (CComboBox*) GetDlgItem(IDC_COMPAREMETHODCOMBO);

	String item = theApp.LoadString(IDS_COMPMETHOD_FULL_CONTENTS);
	combo->AddString(item.c_str());
	item = theApp.LoadString(IDS_COMPMETHOD_QUICK_CONTENTS);
	combo->AddString(item.c_str());
	item = theApp.LoadString(IDS_COMPMETHOD_MODDATE);
	combo->AddString(item.c_str());
	item = theApp.LoadString(IDS_COMPMETHOD_DATESIZE);
	combo->AddString(item.c_str());
	item = theApp.LoadString(IDS_COMPMETHOD_SIZE);
	combo->AddString(item.c_str());
	combo->SetCurSel(m_compareMethod);

	CButton * pBtn = (CButton*) GetDlgItem(IDC_COMPARE_STOPFIRST);
	if (m_compareMethod == 1)
		pBtn->EnableWindow(TRUE);
	else
		pBtn->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/** 
 * @brief Sets options to defaults
 */
void PropCompareFolder::OnDefaults()
{
	DWORD tmp;
	GetOptionsMgr()->GetDefault(OPT_CMP_METHOD, tmp);
	m_compareMethod = tmp;
	GetOptionsMgr()->GetDefault(OPT_CMP_STOP_AFTER_FIRST, tmp);
	m_bStopAfterFirst = tmp;
	GetOptionsMgr()->GetDefault(OPT_CMP_WALK_UNIQUE_DIRS, tmp);
	m_bIncludeUniqFolders = tmp;
	UpdateData(FALSE);
}

/** 
 * @brief Called when compare method dropdown selection is changed.
 * Enables / disables "Stop compare after first difference" checkbox.
 * That checkbox is valid only for quick contents compare method.
 */
void PropCompareFolder::OnCbnSelchangeComparemethodcombo()
{
	CComboBox * pCombo = (CComboBox*) GetDlgItem(IDC_COMPAREMETHODCOMBO);
	CButton * pBtn = (CButton*) GetDlgItem(IDC_COMPARE_STOPFIRST);
	if (pCombo->GetCurSel() == 1)
		pBtn->EnableWindow(TRUE);
	else
		pBtn->EnableWindow(FALSE);
}
