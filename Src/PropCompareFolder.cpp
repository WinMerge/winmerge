/** 
 * @file  PropCompareFolder.cpp
 *
 * @brief Implementation of PropCompareFolder propertysheet
 */

#include "stdafx.h"
#include "PropCompareFolder.h"
#include "Merge.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"
#include "DDXHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int Mega = 1024 * 1024;

/** 
 * @brief Constructor.
 * @param [in] optionsMgr Pointer to COptionsMgr.
 */
PropCompareFolder::PropCompareFolder(COptionsMgr *optionsMgr) 
 : OptionsPanel(optionsMgr, PropCompareFolder::IDD)
 , m_compareMethod(-1)
 , m_bStopAfterFirst(FALSE)
 , m_bIgnoreSmallTimeDiff(FALSE)
 , m_bIncludeUniqFolders(FALSE)
 , m_bExpandSubdirs(FALSE)
 , m_bIgnoreReparsePoints(FALSE)
 , m_nQuickCompareLimit(4 * Mega)
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
	DDX_Check(pDX, IDC_EXPAND_SUBDIRS, m_bExpandSubdirs);
	DDX_Check(pDX, IDC_IGNORE_REPARSEPOINTS, m_bIgnoreReparsePoints);
	DDX_Text(pDX, IDC_COMPARE_QUICKC_LIMIT, m_nQuickCompareLimit);
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
	m_bExpandSubdirs = GetOptionsMgr()->GetBool(OPT_DIRVIEW_EXPAND_SUBDIRS);
	m_bIgnoreReparsePoints = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_REPARSE_POINTS);
	m_nQuickCompareLimit = GetOptionsMgr()->GetInt(OPT_CMP_QUICK_LIMIT) / Mega ;
}

/** 
 * @brief Writes options values from UI to storage.
 * Property sheet calls this after dialog is closed with OK button to
 * store values in member variables.
 */
void PropCompareFolder::WriteOptions()
{
	GetOptionsMgr()->SaveOption(OPT_CMP_METHOD, (int)m_compareMethod);
	GetOptionsMgr()->SaveOption(OPT_CMP_STOP_AFTER_FIRST, m_bStopAfterFirst);
	GetOptionsMgr()->SaveOption(OPT_IGNORE_SMALL_FILETIME, m_bIgnoreSmallTimeDiff);
	GetOptionsMgr()->SaveOption(OPT_CMP_WALK_UNIQUE_DIRS, m_bIncludeUniqFolders);
	GetOptionsMgr()->SaveOption(OPT_DIRVIEW_EXPAND_SUBDIRS, m_bExpandSubdirs);
	GetOptionsMgr()->SaveOption(OPT_CMP_IGNORE_REPARSE_POINTS, m_bIgnoreReparsePoints);

	if (m_nQuickCompareLimit > 2000)
		m_nQuickCompareLimit = 2000;
	GetOptionsMgr()->SaveOption(OPT_CMP_QUICK_LIMIT, m_nQuickCompareLimit * Mega);
}

/** 
 * @brief Called before propertysheet is drawn.
 */
BOOL PropCompareFolder::OnInitDialog() 
{
	theApp.TranslateDialog(m_hWnd);
	CPropertyPage::OnInitDialog();
	CComboBox * combo = (CComboBox*) GetDlgItem(IDC_COMPAREMETHODCOMBO);

	String item = _("Full Contents");
	combo->AddString(item.c_str());
	item = _("Quick Contents");
	combo->AddString(item.c_str());
	item = _("Binary Contents");
	combo->AddString(item.c_str());
	item = _("Modified Date");
	combo->AddString(item.c_str());
	item = _("Modified Date and Size");
	combo->AddString(item.c_str());
	item = _("Size");
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
	m_compareMethod = GetOptionsMgr()->GetDefault<unsigned>(OPT_CMP_METHOD);
	m_bStopAfterFirst = GetOptionsMgr()->GetDefault<bool>(OPT_CMP_STOP_AFTER_FIRST);
	m_bIncludeUniqFolders = GetOptionsMgr()->GetDefault<bool>(OPT_CMP_WALK_UNIQUE_DIRS);
	m_bExpandSubdirs = GetOptionsMgr()->GetDefault<bool>(OPT_DIRVIEW_EXPAND_SUBDIRS);
	m_bIgnoreReparsePoints = GetOptionsMgr()->GetDefault<bool>(OPT_CMP_IGNORE_REPARSE_POINTS);
	m_nQuickCompareLimit = GetOptionsMgr()->GetDefault<unsigned>(OPT_CMP_QUICK_LIMIT) / Mega;
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
