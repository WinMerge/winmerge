/** 
 * @file  PropCompareFolder.cpp
 *
 * @brief Implementation of PropCompareFolder propertysheet
 */

#include "stdafx.h"
#include "PropCompareFolder.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"
#include <Poco/Environment.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const int Mega = 1024 * 1024;

/** 
 * @brief Constructor.
 * @param [in] optionsMgr Pointer to COptionsMgr.
 */
PropCompareFolder::PropCompareFolder(COptionsMgr *optionsMgr) 
 : OptionsPanel(optionsMgr, PropCompareFolder::IDD)
 , m_compareMethod(-1)
 , m_bStopAfterFirst(false)
 , m_bIgnoreSmallTimeDiff(false)
 , m_bIncludeUniqFolders(false)
 , m_bIncludeSubdirs(false)
 , m_nExpandSubdirs(0)
 , m_bIgnoreReparsePoints(false)
 , m_nQuickCompareLimit(4 * Mega)
 , m_nBinaryCompareLimit(64 * Mega)
 , m_nCompareThreads(-1)
 , m_nCompareThreadsPrev(-1)
{
	BindOption(OPT_CMP_METHOD, m_compareMethod, IDC_COMPAREMETHODCOMBO, DDX_CBIndex);
	BindOption(OPT_CMP_STOP_AFTER_FIRST, m_bStopAfterFirst, IDC_COMPARE_STOPFIRST, DDX_Check);
	BindOption(OPT_IGNORE_SMALL_FILETIME, m_bIgnoreSmallTimeDiff, IDC_IGNORE_SMALLTIMEDIFF, DDX_Check);
	BindOption(OPT_CMP_WALK_UNIQUE_DIRS, m_bIncludeUniqFolders, IDC_COMPARE_WALKSUBDIRS, DDX_Check);
	BindOption(OPT_CMP_INCLUDE_SUBDIRS, m_bIncludeSubdirs, IDC_RECURS_CHECK, DDX_Check);
	BindOption(OPT_DIRVIEW_EXPAND_SUBDIRS, m_nExpandSubdirs, IDC_EXPAND_SUBDIRS, DDX_CBIndex);
	BindOption(OPT_CMP_IGNORE_REPARSE_POINTS, m_bIgnoreReparsePoints, IDC_IGNORE_REPARSEPOINTS, DDX_Check);
	auto readconv = +[](int v) -> unsigned { return v / Mega; };
	auto writeconv = +[](unsigned v) -> int { return v * Mega; };
	BindOptionCustom<unsigned, int>(OPT_CMP_QUICK_LIMIT, m_nQuickCompareLimit, IDC_COMPARE_QUICKC_LIMIT, DDX_Text, readconv, writeconv);
	BindOptionCustom<unsigned, int>(OPT_CMP_BINARY_LIMIT, m_nBinaryCompareLimit, IDC_COMPARE_BINARYC_LIMIT, DDX_Text, readconv, writeconv);
	BindOption(OPT_CMP_COMPARE_THREADS, m_nCompareThreads, IDC_COMPARE_THREAD_COUNT, DDX_Text);
}

void PropCompareFolder::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropCompareFolder)
	//}}AFX_DATA_MAP
	DoDataExchangeBindOptions(pDX);
	UpdateControls();
}


BEGIN_MESSAGE_MAP(PropCompareFolder, OptionsPanel)
	//{{AFX_MSG_MAP(PropCompareFolder)
	ON_BN_CLICKED(IDC_COMPARE_DEFAULTS, OnDefaults)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_COMPAREMETHODCOMBO, OnCbnSelchangeComparemethodcombo)
	ON_BN_CLICKED(IDC_RECURS_CHECK, OnBnClickedRecursCheck)
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 * Property sheet calls this before displaying GUI to load values
 * into members.
 */
void PropCompareFolder::ReadOptions()
{
	ReadOptionBindings();
	m_nCompareThreadsPrev = m_nCompareThreads;
	if (m_nCompareThreads <= 0)
		m_nCompareThreads = Poco::Environment::processorCount() + m_nCompareThreads;
	m_nCompareThreads = std::clamp(m_nCompareThreads, 1, static_cast<int>(Poco::Environment::processorCount()));
}

/** 
 * @brief Writes options values from UI to storage.
 * Property sheet calls this after dialog is closed with OK button to
 * store values in member variables.
 */
void PropCompareFolder::WriteOptions()
{
	m_nQuickCompareLimit = std::clamp(m_nQuickCompareLimit, 1u, 2000u);
	m_nBinaryCompareLimit = std::clamp(m_nBinaryCompareLimit, 1u, 2000u);
	WriteOptionBindings();
	if ((m_nCompareThreadsPrev >  0 && m_nCompareThreads != m_nCompareThreadsPrev) ||
	    (m_nCompareThreadsPrev <= 0 && m_nCompareThreads != static_cast<int>(Poco::Environment::processorCount() + m_nCompareThreadsPrev)))
		GetOptionsMgr()->SaveOption(OPT_CMP_COMPARE_THREADS, m_nCompareThreads);
}

/** 
 * @brief Called before propertysheet is drawn.
 */
BOOL PropCompareFolder::OnInitDialog() 
{
	SetDlgItemComboBoxList(IDC_COMPAREMETHODCOMBO,
		{ _("Full Contents"), _("Quick Contents"), _("Binary Contents"), _("Modified Date"), _("Modified Date and Size"), _("Size") });
	SetDlgItemComboBoxList(IDC_EXPAND_SUBDIRS,
		{ _("Do not expand"), _("Expand all subfolders"), _("Expand different subfolders"), _("Expand identical subfolders") });

	OptionsPanel::OnInitDialog();
	
	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/** 
 * @brief Sets options to defaults
 */
void PropCompareFolder::OnDefaults()
{
	ResetOptionBindings();
	if (m_nCompareThreads <= 0)
		m_nCompareThreads = Poco::Environment::processorCount() + m_nCompareThreads;
	UpdateData(FALSE);
}

/** 
 * @brief Called when compare method dropdown selection is changed.
 * Enables / disables "Stop compare after first difference" checkbox.
 * That checkbox is valid only for quick contents compare method.
 */
void PropCompareFolder::OnCbnSelchangeComparemethodcombo()
{
	UpdateControls();
}

void PropCompareFolder::OnBnClickedRecursCheck()
{
	UpdateControls();
}

void PropCompareFolder::UpdateControls()
{
	CComboBox * pCombo = (CComboBox*)GetDlgItem(IDC_COMPAREMETHODCOMBO);
	EnableDlgItem(IDC_COMPARE_STOPFIRST, pCombo->GetCurSel() == 1);
	EnableDlgItem(IDC_COMPARE_WALKSUBDIRS, IsDlgButtonChecked(IDC_RECURS_CHECK) == 1);
	EnableDlgItem(IDC_EXPAND_SUBDIRS, IsDlgButtonChecked(IDC_RECURS_CHECK) == 1);
	EnableDlgItem(IDC_COMPARE_THREAD_COUNT, pCombo->GetCurSel() <= 1 ? true : false); // true: fullcontent, quickcontent
	EnableDlgItem(IDC_COMPARE_QUICKC_LIMIT, pCombo->GetCurSel() == 0 ? true : false); // true: fullcontent
	EnableDlgItem(IDC_COMPARE_BINARYC_LIMIT, pCombo->GetCurSel() <= 1 ? true : false); // true: fullcontent, quickcontent
}
