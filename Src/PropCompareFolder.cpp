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
#include "FilterErrorMessages.h"
#include "PropertySystemMenu.h"
#include "unicoder.h"
#include <Poco/Environment.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const int Mega = 1024 * 1024;

class CPropCompareFolderMenu : public CMenu
{
public:
	std::optional<String> ShowMenu(int menuid, const String& expr, int x, int y, CWnd* pParentWnd)
	{
		std::optional<String> result;
		VERIFY(LoadMenu(menuid));
		I18n::TranslateMenu(m_hMenu);
		CMenu* pPopup = GetSubMenu(0);
		if (pPopup)
		{
#ifndef _WIN64
			pPopup->EnableMenuItem(ID_ADDCMPMENU_PROPS, MF_GRAYED);
#endif
			const int command = pPopup->TrackPopupMenu(
				TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, x, y, pParentWnd);
			if (command == 0)
			{
				// User cancelled the menu
			}
			else if (command == ID_ADDCMPMENU_CLEAR || command == ID_MOVEDETECTIONMENU_CLEAR)
			{
				result = _T("");
			}
			else if (command >= ID_ADDCMPMENU_CMP_FIRST && command <= ID_ADDCMPMENU_CMP_LAST)
			{
				static const String Exprs[] = {
					_T("allequal(Size)"),
					_T("allequal(Date)"),
					_T("allequal(Attributes)"),
					_T("allequal(Content)"),
				};
				result = expr.empty() ? expr : expr + _T(" and ");
				*result += Exprs[command - ID_ADDCMPMENU_CMP_FIRST];
			}
			else if (command == ID_ADDCMPMENU_PROPS)
			{
				CPropertySystemMenu menuProps;
				auto resultProp = menuProps.ShowMenu(pParentWnd, ID_ADDCMPMENU_PROPS_FIRST, _("Compare %1"));
				if (resultProp.has_value())
				{
					result = expr.empty() ? expr : expr + _T(" and ");
					*result += _T("allequal(prop(\"") + *resultProp + _T("\"))");
				}
			}
			else if (command >= ID_MOVEDETECTIONMENU_FIRST && command <= ID_MOVEDETECTIONMENU_LAST)
			{
				static const String Exprs[] = {
					_T("allequal(Name)"),
					_T("allequal(Size)"),
					_T("allequal(Date)"),
					_T("allequal(Content)"),
				};
				result = expr.empty() ? expr : expr + _T(" and ");
				*result += Exprs[command - ID_MOVEDETECTIONMENU_FIRST];
			}
			else if (command == ID_MOVEDETECTIONMENU_PROPS)
			{
				CPropertySystemMenu menuProps;
				auto resultProp = menuProps.ShowMenu(pParentWnd, ID_MOVEDETECTIONMENU_PROPS_FIRST, _("Match %1"));
				if (resultProp.has_value())
				{
					result = expr.empty() ? expr : expr + _T(" and ");
					*result += _T("allequal(prop(\"") + *resultProp + _T("\"))");
				}
			}
		}
		DestroyMenu();

		return result;
	}
};

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
 , m_pAdditionalCompareCondition(new FilterExpression())
 , m_pMoveDetectionCondition(new FilterExpression())
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
	BindOption(OPT_CMP_ADDITIONAL_CONDITION, m_sAdditionalCompareCondition, IDC_ADDTIONAL_COMPARE_CONDITION, DDX_CBStringExact);
	BindOption(OPT_CMP_MOVE_DETECTION_CONDITION, m_sMoveDetectionCondition, IDC_MOVE_DETECTION_CONDITION, DDX_CBStringExact);
}

void PropCompareFolder::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropCompareFolder)
	DDX_Control(pDX, IDC_ADDTIONAL_COMPARE_CONDITION, m_ctlAdditionalCompareCondition);
	DDX_Control(pDX, IDC_MOVE_DETECTION_CONDITION, m_ctlMoveDetectionCondition);
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
	ON_CBN_EDITCHANGE(IDC_ADDTIONAL_COMPARE_CONDITION, OnEditChangeAdditionalCompareCondition)
	ON_CBN_SELCHANGE(IDC_ADDTIONAL_COMPARE_CONDITION, OnEditChangeAdditionalCompareCondition)
	ON_BN_CLICKED(IDC_ADDTIONAL_COMPARE_CONDITION_MENU, OnBnClickedAdditionalCompareConditionMenu)
	ON_CBN_EDITCHANGE(IDC_MOVE_DETECTION_CONDITION, OnEditChangeMoveDetectionCondition)
	ON_CBN_SELCHANGE(IDC_MOVE_DETECTION_CONDITION, OnEditChangeMoveDetectionCondition)
	ON_BN_CLICKED(IDC_MOVE_DETECTION_CONDITION_MENU, OnBnClickedMoveDetectionConditionMenu)
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
	if (m_ctlAdditionalCompareCondition.GetSafeHwnd())
		m_ctlAdditionalCompareCondition.SaveState(_T("Files\\AdditionalCompareCondition"));
	if (m_ctlMoveDetectionCondition.GetSafeHwnd())
		m_ctlMoveDetectionCondition.SaveState(_T("Files\\MoveDetectionCondition"));
}

/** 
 * @brief Called before propertysheet is drawn.
 */
BOOL PropCompareFolder::OnInitDialog() 
{
	SetDlgItemComboBoxList(IDC_COMPAREMETHODCOMBO,
		{ _("Full Contents"), _("Quick Contents"), _("Binary Contents"), _("Modified Date"), _("Modified Date and Size"), _("Size"), _("Existence")});
	SetDlgItemComboBoxList(IDC_EXPAND_SUBDIRS,
		{ _("Do not expand"), _("Expand all subfolders"), _("Expand different subfolders"), _("Expand identical subfolders") });

	OptionsPanel::OnInitDialog();
	
	m_ctlAdditionalCompareCondition.LoadState(_T("Files\\AdditionalCompareCondition"));
	m_ctlAdditionalCompareCondition.SetWindowText(m_sAdditionalCompareCondition.c_str());

	COMBOBOXINFO cbi{ sizeof(COMBOBOXINFO) };
	GetComboBoxInfo(m_ctlAdditionalCompareCondition.m_hWnd, &cbi);
	m_ctlAdditionalCompareConditionEdit.SubclassWindow(cbi.hwndItem);
	m_ctlAdditionalCompareConditionEdit.m_validator = [this](const CString& text, CString& error) -> bool
		{
			if (text.IsEmpty())
				return true;
			bool bError = !m_pAdditionalCompareCondition->Parse(ucr::toUTF8((const tchar_t*)text));
			if (bError)
				error = FormatFilterErrorSummary(*m_pAdditionalCompareCondition).c_str();
			return !bError;
		};
	m_ctlAdditionalCompareConditionEdit.Validate();

	GetComboBoxInfo(m_ctlMoveDetectionCondition.m_hWnd, &cbi);
	m_ctlMoveDetectionConditionEdit.SubclassWindow(cbi.hwndItem);
	m_ctlMoveDetectionConditionEdit.m_validator = [this](const CString& text, CString& error) -> bool
		{
			if (text.IsEmpty())
				return true;
			bool bError = !m_pMoveDetectionCondition->Parse(ucr::toUTF8((const tchar_t*)text));
			if (bError)
				error = FormatFilterErrorSummary(*m_pMoveDetectionCondition).c_str();
			return !bError;
		};
	m_ctlMoveDetectionConditionEdit.Validate();

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

void PropCompareFolder::OnEditChangeAdditionalCompareCondition()
{
	m_ctlAdditionalCompareConditionEdit.OnEnChange();
}

void PropCompareFolder::OnBnClickedAdditionalCompareConditionMenu()
{
	UpdateData(TRUE);
	CPropCompareFolderMenu menu;
	CRect rc;
	GetDlgItem(IDC_ADDTIONAL_COMPARE_CONDITION_MENU)->GetWindowRect(&rc);
	const std::optional<String> expr = menu.ShowMenu(IDR_POPUP_ADDCMPMENU, m_sAdditionalCompareCondition, rc.left, rc.bottom, this);
	if (expr.has_value())
	{
		m_sAdditionalCompareCondition = *expr;
		UpdateData(FALSE);
		m_ctlAdditionalCompareConditionEdit.OnEnChange();
	}
}

void PropCompareFolder::OnEditChangeMoveDetectionCondition()
{
	m_ctlMoveDetectionConditionEdit.OnEnChange();
}

void PropCompareFolder::OnBnClickedMoveDetectionConditionMenu()
{
	UpdateData(TRUE);
	CPropCompareFolderMenu menu;
	CRect rc;
	GetDlgItem(IDC_MOVE_DETECTION_CONDITION_MENU)->GetWindowRect(&rc);
	const std::optional<String> expr = menu.ShowMenu(IDR_POPUP_MOVEDETECTIONMENU, m_sMoveDetectionCondition, rc.left, rc.bottom, this);
	if (expr.has_value())
	{
		m_sMoveDetectionCondition = *expr;
		UpdateData(FALSE);
		m_ctlMoveDetectionConditionEdit.OnEnChange();
	}
}

void PropCompareFolder::UpdateControls()
{
	CComboBox * pCombo = (CComboBox*)GetDlgItem(IDC_COMPAREMETHODCOMBO);
	const int sel = pCombo->GetCurSel();
	EnableDlgItem(IDC_COMPARE_STOPFIRST, sel == 1);
	EnableDlgItem(IDC_IGNORE_SMALLTIMEDIFF, sel == 3 || sel == 4);
	EnableDlgItem(IDC_COMPARE_WALKSUBDIRS, IsDlgButtonChecked(IDC_RECURS_CHECK) == 1);
	EnableDlgItem(IDC_EXPAND_SUBDIRS, IsDlgButtonChecked(IDC_RECURS_CHECK) == 1);
	EnableDlgItem(IDC_COMPARE_THREAD_COUNT, sel <= 1 ? true : false); // true: fullcontent, quickcontent
	EnableDlgItem(IDC_COMPARE_QUICKC_LIMIT, sel == 0 ? true : false); // true: fullcontent
	EnableDlgItem(IDC_COMPARE_BINARYC_LIMIT, sel <= 1 ? true : false); // true: fullcontent, quickcontent
}
