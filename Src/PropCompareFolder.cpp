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
#include "paths.h"
#include "FileOrFolderSelect.h"
#include "DirItem.h"
#include "DirTravel.h"
#include "Environment.h"
#include "Shell.h"
#include "UniFile.h"
#include <Poco/Environment.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const int Mega = 1024 * 1024;
// Max number of replace lists shown in menu.
// NOTE: This relies on the resource ID ranges for string and regex replace lists
//       being consecutive and of equal size. If those ranges change in resource.h
//       (ID_RENAME_MOVE_KEY_MENU_STRING_REPLACE_LISTS_FIRST / _REGEX_...), update
//       MaxReplaceListSize accordingly.
static const int MaxReplaceListSize = 20;
static_assert(
    ID_RENAME_MOVE_KEY_MENU_REGEX_REPLACE_LISTS_FIRST - ID_RENAME_MOVE_KEY_MENU_STRING_REPLACE_LISTS_FIRST == MaxReplaceListSize,
    "MaxReplaceListSize must match the distance between the string and regex replace-list ID ranges."
);

// Get replace list folder
static String GetReplaceListFolder(int locationType, bool isRegex)
{
	String folder = locationType == 0 ? env::GetAppDataPath() : env::GetMyDocuments();
	return paths::ConcatPath(folder, isRegex ? _T("WinMerge\\RegexReplaceLists") : _T("WinMerge\\ReplaceLists"));
}

// Get files in replace list folder
static std::vector<String> GetReplaceLists(bool isRegex)
{
	int locationType = GetOptionsMgr()->GetInt(OPT_USERDATA_LOCATION);
	std::vector<String> list;
	String folder = GetReplaceListFolder(locationType, isRegex);

	if (folder.empty())
		return list;

	// Create folder if it doesn't exist
	if (paths::DoesPathExist(folder) != paths::IS_EXISTING_DIR)
	{
		if (!paths::CreateIfNeeded(folder))
			return list;
	}

	DirItemArray files, dirs;
	DirTravel::LoadFiles(folder, &dirs, &files, _T("*.*"));

	for (const auto& file : files)
		list.push_back(paths::ConcatPath(folder, file.filename.get()));

	DirItemArray files2, dirs2;
	folder = GetReplaceListFolder(1 - locationType, isRegex);
	DirTravel::LoadFiles(folder, &dirs2, &files2, _T("*.*"));

	for (const auto& file : files2)
		list.push_back(paths::ConcatPath(folder, file.filename.get()));

	return list;
}

// Create template file
static bool CreateTemplateFile(const String& filepath, bool isRegex)
{
	// Write template content
	String templateText = isRegex ?
			// Regex replacement list template
			_("# Regex replacement list\r\n"
			"# Format: regex<TAB>replacement\r\n"
			"# Backreferences like $1, $2 are supported\r\n"
			"\r\n"
			"(\\d{4})-(\\d{2})-(\\d{2})\t$1_$2_$3\r\n")
		:
			// Replacement list template
			_("# Replacement list\r\n"
			"# Format: search<TAB>replacement\r\n"
			"# Lines starting with # are ignored\r\n"
			"\r\n"
			"from1\tto1\r\n"
			"from2\tto2\r\n");

	UniStdioFile file;
	if (!file.OpenCreateUtf8(filepath))
		return false;
	file.WriteString(templateText);
	return true;
}

static String ReplaceAppDataFolderOrUserProfileFolder(const String& path)
{
	String result = path;
	String appData = env::GetAppDataPath();
	if (tc::tcsstr(path.c_str(), appData.c_str()))
	{
		strutils::replace(result, appData, _T("%APPDATA%"));
		return result;
	}
	String userProfile = env::ExpandEnvironmentVariables(_T("%USERPROFILE%"));
	if (tc::tcsstr(path.c_str(), userProfile.c_str()))
	{
		strutils::replace(result, userProfile, _T("%USERPROFILE%"));
		return result;
	}
	return result;
}

class CPropCompareFolderMenu : public CMenu
{
private:
	void PopulateReplaceLists(CMenu* pPopup)
	{
		// Find "Replace Lists" submenu
		for (int i = 0; i < pPopup->GetMenuItemCount(); i++)
		{
			CMenu* pSubMenu = pPopup->GetSubMenu(i);
			if (pSubMenu)
			{
				MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
				mii.fMask = MIIM_STRING;
				tchar_t buf[256];
				mii.dwTypeData = buf;
				mii.cch = 256;
				if (pPopup->GetMenuItemInfo(i, &mii, TRUE))
				{
					if (tc::tcsstr(buf, _("Replace &Lists").c_str()))
					{
						PopulateReplaceListsSubMenu(pSubMenu);
						break;
					}
				}
			}
		}
	}

	void PopulateReplaceListsSubMenu(CMenu* pReplaceListsMenu)
	{
		// Find String Replace Lists and Regex Replace Lists submenus
		for (int i = 0; i < pReplaceListsMenu->GetMenuItemCount(); i++)
		{
			CMenu* pSubMenu = pReplaceListsMenu->GetSubMenu(i);
			if (pSubMenu)
			{
				MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
				mii.fMask = MIIM_ID;
				if (pSubMenu->GetMenuItemInfo(0, &mii, TRUE))
				{
					if (mii.wID == ID_RENAME_MOVE_KEY_MENU_STRING_REPLACE_LISTS_FIRST)
					{
						PopulateListMenu(pSubMenu, false, ID_RENAME_MOVE_KEY_MENU_STRING_REPLACE_LISTS_FIRST);
					}
					else if (mii.wID == ID_RENAME_MOVE_KEY_MENU_REGEX_REPLACE_LISTS_FIRST)
					{
						PopulateListMenu(pSubMenu, true, ID_RENAME_MOVE_KEY_MENU_REGEX_REPLACE_LISTS_FIRST);
					}
				}
			}
		}
	}

	void PopulateListMenu(CMenu* pMenu, bool isRegex, UINT firstID)
	{
		// Delete existing "<None>" item
		pMenu->DeleteMenu(0, MF_BYPOSITION);

		// Get lists in folder
		std::vector<String> lists = GetReplaceLists(isRegex);

		if (lists.empty())
		{
			pMenu->AppendMenu(MF_STRING | MF_GRAYED, firstID, _("<None>").c_str());
		}
		else
		{
			for (size_t i = 0; i < lists.size() && i < MaxReplaceListSize; i++)
			{
				String filename = paths::FindFileName(lists[i]);
				pMenu->AppendMenu(MF_STRING, firstID + static_cast<UINT>(i), filename.c_str());
			}
		}
	}

	// Create and select replace list file
	std::optional<String> CreateAndSelectReplaceListFile(CWnd* pParentWnd, bool isRegex)
	{
		int locationType = GetOptionsMgr()->GetInt(OPT_USERDATA_LOCATION);
		String folder = GetReplaceListFolder(locationType, isRegex);
		if (folder.empty())
		{
			AfxMessageBox(_T("Failed to get ReplaceList folder."), MB_ICONERROR);
			return std::nullopt;
		}

		// Create folder if it doesn't exist
		if (paths::DoesPathExist(folder) != paths::IS_EXISTING_DIR)
		{
			if (!paths::CreateIfNeeded(folder))
			{
				AfxMessageBox((_T("Failed to create folder:\n") + folder).c_str(), MB_ICONERROR);
				return std::nullopt;
			}
		}

		String title = isRegex ?
			_("Create &Regex Replace List and Insert...") :
			_("&Create String Replace List and Insert...");
		strutils::replace(title, _T("&"), _T("")); // Remove & for file dialog title

		// Display file save dialog
		String sFilePath;
		String initialDir = folder;
		if (!SelectFile(pParentWnd->GetSafeHwnd(), sFilePath, false, initialDir.c_str(), title, 
			_("Tab-Separated Values (*.tsv;*.txt)|*.tsv;*.txt|All Files (*.*)|*.*||"), _T("tsv")))
			return std::nullopt;

		String filepath = sFilePath;

		// Create template file
		if (!CreateTemplateFile(filepath, isRegex))
		{
			AfxMessageBox((_T("Failed to create file:\n") + filepath).c_str(), MB_ICONERROR);
			return std::nullopt;
		}

		// Open with default editor
		shell::Edit(filepath.c_str());

		return filepath;
	}

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
			// Build menu dynamically
			if (menuid == IDR_POPUP_RENAMEMOVE_MENU)
			{
				PopulateReplaceLists(pPopup);
			}

			const int command = pPopup->TrackPopupMenu(
				TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, x, y, pParentWnd);
			if (command == 0)
			{
				// User cancelled the menu
			}
			else if (command == ID_ADDCMPMENU_CLEAR || command == ID_RENAME_MOVE_KEY_MENU_CLEAR)
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
				String newExpr = Exprs[command - ID_ADDCMPMENU_CMP_FIRST];
				result = expr.empty() ? newExpr : (expr + _T(" and ") + newExpr);
			}
			else if (command == ID_ADDCMPMENU_PROPS)
			{
				CPropertySystemMenu menuProps;
				auto resultProp = menuProps.ShowMenu(pParentWnd, ID_ADDCMPMENU_PROPS_FIRST, _("Compare %1"));
				if (resultProp.has_value())
				{
					String newExpr = _T("allequal(prop(\"") + *resultProp + _T("\"))");
					result = expr.empty() ? newExpr : (expr + _T(" and ") + newExpr);
				}
			}
			else if (command >= ID_RENAME_MOVE_KEY_MENU_FIRST && command <= ID_RENAME_MOVE_KEY_MENU_LAST)
			{
				static const String Exprs[] = {
					_T("Name"),
					_T("BaseName"),
					_T("normalizeUnicode(Name, \"NFC\")"),
					_T("RelPath"),
					_T("Size"),
					_T("Date"),
					_T("if(IsFolder, Name, prop(\"Hash.MD5\"))"),
				};
				String newExpr = Exprs[command - ID_RENAME_MOVE_KEY_MENU_FIRST];
				result = expr.empty() ? newExpr : (expr + _T(" + \"|\" + ") + newExpr);
			}
			else if (command == ID_RENAME_MOVE_KEY_MENU_PROPS)
			{
				CPropertySystemMenu menuProps;
				auto resultProp = menuProps.ShowMenu(pParentWnd, ID_RENAME_MOVE_KEY_MENU_PROPS_FIRST, _("%1"));
				if (resultProp.has_value())
				{
					String newExpr = _T("if(IsFolder, Name, prop(\"") + *resultProp + _T("\"))");
					result = expr.empty() ? newExpr : (expr + _T(" + \"|\" + ") + newExpr);
				}
			}
			else if (command >= ID_RENAME_MOVE_KEY_MENU_FUNC_FIRST && command <= ID_RENAME_MOVE_KEY_MENU_FUNC_LAST)
			{
				static const String Exprs[] = {
					_T("replace(%1, \"from\", \"to\")"),
					_T("regexReplace(%1, \"pattern\", \"replacement\")"),
					_T("toLower(%1)"),
					_T("toUpper(%1)"),
					_T("toHalfWidth(%1)"),
					_T("toFullWidth(%1)"),
					_T("toSimplifiedChinese(%1)"),
					_T("toTraditionalChinese(%1)"),
					_T("toHiragana(%1)"),
					_T("toKatakana(%1)"),
					_T("normalizeUnicode(%1, \"NFC\")"),
				};
				String newExpr = Exprs[command - ID_RENAME_MOVE_KEY_MENU_FUNC_FIRST];
				result = strutils::format_string1(newExpr, expr.empty() ? _T("Name") : expr);
			}
			else if (command == ID_RENAME_MOVE_KEY_MENU_CREATE_REPLACELIST)
			{
				auto filepath = CreateAndSelectReplaceListFile(pParentWnd, false);
				if (filepath.has_value())
				{
					String filepath2 = ReplaceAppDataFolderOrUserProfileFolder(*filepath);
					String newExpr = _T("replaceWithList(") + (expr.empty() ? _T("Name") : expr) + 
						_T(", \"") + filepath2 + _T("\")");
					result = newExpr;
				}
			}
			else if (command == ID_RENAME_MOVE_KEY_MENU_CREATE_REGEXREPLACELIST)
			{
				auto filepath = CreateAndSelectReplaceListFile(pParentWnd, true);
				if (filepath.has_value())
				{
					String filepath2 = ReplaceAppDataFolderOrUserProfileFolder(*filepath);
					String newExpr = _T("regexReplaceWithList(") + (expr.empty() ? _T("Name") : expr) + 
						_T(", \"") + filepath2 + _T("\")");
					result = newExpr;
				}
			}
			else if (command >= ID_RENAME_MOVE_KEY_MENU_STRING_REPLACE_LISTS_FIRST && 
					 command < ID_RENAME_MOVE_KEY_MENU_STRING_REPLACE_LISTS_FIRST + MaxReplaceListSize)
			{
				auto lists = GetReplaceLists(false);
				int index = command - ID_RENAME_MOVE_KEY_MENU_STRING_REPLACE_LISTS_FIRST;
				if (index < static_cast<int>(lists.size()))
				{
					String filepath = ReplaceAppDataFolderOrUserProfileFolder(lists[index]);
					String newExpr = _T("replaceWithList(") + (expr.empty() ? _T("Name") : expr) + 
						_T(", \"") + filepath + _T("\")");
					result = newExpr;
				}
			}
			else if (command >= ID_RENAME_MOVE_KEY_MENU_REGEX_REPLACE_LISTS_FIRST && 
					 command < ID_RENAME_MOVE_KEY_MENU_REGEX_REPLACE_LISTS_FIRST + MaxReplaceListSize)
			{
				auto lists = GetReplaceLists(true);
				int index = command - ID_RENAME_MOVE_KEY_MENU_REGEX_REPLACE_LISTS_FIRST;
				if (index < static_cast<int>(lists.size()))
				{
					String filepath = ReplaceAppDataFolderOrUserProfileFolder(lists[index]);
					String newExpr = _T("regexReplaceWithList(") + (expr.empty() ? _T("Name") : expr) + 
						_T(", \"") + filepath + _T("\")");
					result = newExpr;
				}
			}
			else if (command == ID_RENAME_MOVE_KEY_MENU_STRING_REPLACE_LISTS_FOLDER)
			{
				int locationType = GetOptionsMgr()->GetInt(OPT_USERDATA_LOCATION);
				String folder = GetReplaceListFolder(locationType, false);
				if (!folder.empty())
					shell::Open(folder.c_str());
			}
			else if (command == ID_RENAME_MOVE_KEY_MENU_REGEX_REPLACE_LISTS_FOLDER)
			{
				int locationType = GetOptionsMgr()->GetInt(OPT_USERDATA_LOCATION);
				String folder = GetReplaceListFolder(locationType, true);
				if (!folder.empty())
					shell::Open(folder.c_str());
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
 , m_pRenameMoveKey(new FilterExpression())
 , m_nRenameMoveDetection(0)
 , m_nRenameMoveMergeMode(0)
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
	BindOption(OPT_CMP_RENAME_MOVE_DETECTION, m_nRenameMoveDetection, IDC_RENAME_MOVE_DETECTION, DDX_CBIndex);
	BindOption(OPT_CMP_RENAME_MOVE_KEY, m_sRenameMoveKey, IDC_RENAME_MOVE_KEY, DDX_CBStringExact);
	BindOption(OPT_CMP_RENAME_MOVE_MERGE_MODE, m_nRenameMoveMergeMode, IDC_RENAME_MOVE_MERGE_MODE, DDX_CBIndex);
}

void PropCompareFolder::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropCompareFolder)
	DDX_Control(pDX, IDC_ADDTIONAL_COMPARE_CONDITION, m_ctlAdditionalCompareCondition);
	DDX_Control(pDX, IDC_RENAME_MOVE_KEY, m_ctlRenameMoveKey);
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
	ON_CBN_SELCHANGE(IDC_RENAME_MOVE_DETECTION, OnCbnSelchangeRenameMoveDetection)
	ON_CBN_EDITCHANGE(IDC_RENAME_MOVE_KEY, OnEditChangeRenameMoveKey)
	ON_CBN_SELCHANGE(IDC_RENAME_MOVE_KEY, OnEditChangeRenameMoveKey)
	ON_BN_CLICKED(IDC_RENAME_MOVE_KEY_MENU, OnBnClickedRenameMoveKeyMenu)
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
	if (m_ctlRenameMoveKey.GetSafeHwnd())
		m_ctlRenameMoveKey.SaveState(_T("Files\\RenameMoveKey"));
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
	SetDlgItemComboBoxList(IDC_RENAME_MOVE_DETECTION,
		{ I18n::tr("Options dialog|Compare|Folder|Detect renames and moves","Disabled"), _("Detect renames"), _("Detect renames and moves") });
	SetDlgItemComboBoxList(IDC_RENAME_MOVE_MERGE_MODE,
		{ I18n::tr("Options dialog|Compare|Folder|Detect renames and moves","Disabled"), _("Merge renames"), _("Merge renames and moves") });

	OptionsPanel::OnInitDialog();
	
	m_ctlAdditionalCompareCondition.LoadState(_T("Files\\AdditionalCompareCondition"));
	m_ctlAdditionalCompareCondition.SetWindowText(m_sAdditionalCompareCondition.c_str());
	m_ctlRenameMoveKey.LoadState(_T("Files\\RenameMoveKey"));
	m_ctlRenameMoveKey.SetWindowText(m_sRenameMoveKey.c_str());

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
	m_ctlAdditionalCompareConditionEdit.SetCueBanner(strutils::format_string1(_("e.g. %1"), _T("allequal(Date)")).c_str());

	GetComboBoxInfo(m_ctlRenameMoveKey.m_hWnd, &cbi);
	m_ctlRenameMoveKeyEdit.SubclassWindow(cbi.hwndItem);
	m_ctlRenameMoveKeyEdit.m_validator = [this](const CString& text, CString& error) -> bool
		{
			if (text.IsEmpty())
				return true;
			bool bError = !m_pRenameMoveKey->Parse(ucr::toUTF8((const tchar_t*)text));
			if (bError)
				error = FormatFilterErrorSummary(*m_pRenameMoveKey).c_str();
			return !bError;
		};
	m_ctlRenameMoveKeyEdit.Validate();
	m_ctlRenameMoveKeyEdit.SetCueBanner(strutils::format_string1(_("e.g. %1"), _T("BaseName + \"|\" + Size")).c_str());

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

void PropCompareFolder::OnCbnSelchangeRenameMoveDetection()
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

void PropCompareFolder::OnEditChangeRenameMoveKey()
{
	m_ctlRenameMoveKeyEdit.OnEnChange();
}

void PropCompareFolder::OnBnClickedRenameMoveKeyMenu()
{
	UpdateData(TRUE);
	CPropCompareFolderMenu menu;
	CRect rc;
	GetDlgItem(IDC_RENAME_MOVE_KEY_MENU)->GetWindowRect(&rc);
	const std::optional<String> expr = menu.ShowMenu(IDR_POPUP_RENAMEMOVE_MENU, m_sRenameMoveKey, rc.left, rc.bottom, this);
	if (expr.has_value())
	{
		m_sRenameMoveKey = *expr;
		UpdateData(FALSE);
		m_ctlRenameMoveKeyEdit.OnEnChange();
	}
}

void PropCompareFolder::UpdateControls()
{
	CComboBox * pCombo = (CComboBox*)GetDlgItem(IDC_COMPAREMETHODCOMBO);
	const int sel = pCombo->GetCurSel();
	CComboBox * pComboRenameMoveDetection = (CComboBox*)GetDlgItem(IDC_RENAME_MOVE_DETECTION);
	const int selRenameMoveDetection = pComboRenameMoveDetection->GetCurSel();
	EnableDlgItem(IDC_COMPARE_STOPFIRST, sel == 1);
	EnableDlgItem(IDC_IGNORE_SMALLTIMEDIFF, sel == 3 || sel == 4);
	EnableDlgItem(IDC_COMPARE_WALKSUBDIRS, IsDlgButtonChecked(IDC_RECURS_CHECK) == 1);
	EnableDlgItem(IDC_EXPAND_SUBDIRS, IsDlgButtonChecked(IDC_RECURS_CHECK) == 1);
	EnableDlgItem(IDC_COMPARE_THREAD_COUNT, sel <= 1 ? true : false); // true: fullcontent, quickcontent
	EnableDlgItem(IDC_COMPARE_QUICKC_LIMIT, sel == 0 ? true : false); // true: fullcontent
	EnableDlgItem(IDC_COMPARE_BINARYC_LIMIT, sel <= 1 ? true : false); // true: fullcontent, quickcontent
	EnableDlgItem(IDC_RENAME_MOVE_KEY, selRenameMoveDetection >= 1);
	EnableDlgItem(IDC_RENAME_MOVE_KEY_MENU, selRenameMoveDetection >= 1);
	EnableDlgItem(IDC_RENAME_MOVE_MERGE_MODE, selRenameMoveDetection >= 1);
}
