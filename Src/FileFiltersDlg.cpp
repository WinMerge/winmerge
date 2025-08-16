// SPDX-License-Identifier: GPL-2.0-or-later
/**
 * @file  FileFiltersDlg.cpp
 *
 * @brief Implementation of FileFilters -dialog
 */

#include "stdafx.h"
#include "FileFiltersDlg.h"
#include <vector>
#include "UnicodeString.h"
#include "Merge.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"
#include "FileFilterMgr.h"
#include "FileFilterHelper.h"
#include "paths.h"
#include "SharedFilterDlg.h"
#include "TestFilterDlg.h"
#include "FileOrFolderSelect.h"
#include "UniFile.h"
#include "Constants.h"
#include "FilterErrorMessages.h"
#include "FileFilterHelperMenu.h"
#include "DirWatcher.h"

using std::vector;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/** @brief Template file used when creating new filefilter. */
static const tchar_t FILE_FILTER_TEMPLATE[] = _T("FileFilter.tmpl");

/////////////////////////////////////////////////////////////////////////////
// CFiltersDlg dialog
IMPLEMENT_DYNCREATE(FileFiltersDlg, CTrPropertyPage)

/**
 * @brief Constructor.
 */
FileFiltersDlg::FileFiltersDlg()
	: CTrPropertyPage(FileFiltersDlg::IDD)
	, m_pFileFilterHelper(new FileFilterHelper())
	, m_pFileFilterHelperOrg(nullptr)
{
	m_strCaption = theApp.LoadDialogCaption(m_lpszTemplateName).c_str();
	m_psp.pszTitle = m_strCaption;
	m_psp.dwFlags |= PSP_USETITLE;
	m_psp.hIcon = AfxGetApp()->LoadIcon(IDI_FILEFILTER);
	m_psp.dwFlags |= PSP_USEHICON;
}

void FileFiltersDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(FileFiltersDlg)
	DDX_CBStringExact(pDX, IDC_FILTERFILE_MASK, m_sMask);
	DDX_Control(pDX, IDC_FILTERFILE_LIST, m_listFilters);
	DDX_Control(pDX, IDC_FILTERFILE_MASK, m_ctlMask);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(FileFiltersDlg, CTrPropertyPage)
	//{{AFX_MSG_MAP(FileFiltersDlg)
	ON_CBN_KILLFOCUS(IDC_FILTERFILE_MASK, OnKillFocusFilterfileMask)
	ON_CBN_EDITCHANGE(IDC_FILTERFILE_MASK, OnEditChangeFilterfileMask)
	ON_CBN_SELCHANGE(IDC_FILTERFILE_MASK, OnEditChangeFilterfileMask)
	ON_BN_CLICKED(IDC_FILTERFILE_MASK_MENU, OnFilterfileMaskMenu)
	ON_BN_CLICKED(IDC_FILTERFILE_EDITBTN, OnFiltersEditbtn)
	ON_NOTIFY(NM_DBLCLK, IDC_FILTERFILE_LIST, OnDblclkFiltersList)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_FILTERFILE_LIST, OnCustomDrawFiltersList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FILTERFILE_LIST, OnLvnItemchangedFilterfileList)
	ON_NOTIFY(LVN_GETINFOTIP, IDC_FILTERFILE_LIST, OnInfoTip)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_FILTERFILE_TEST_BTN, OnBnClickedFilterfileTestButton)
	ON_BN_CLICKED(IDC_FILTERFILE_NEWBTN, OnBnClickedFilterfileNewbutton)
	ON_BN_CLICKED(IDC_FILTERFILE_DELETEBTN, OnBnClickedFilterfileDelete)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_BN_CLICKED(IDC_FILTERFILE_INSTALL, OnBnClickedFilterfileInstall)
	ON_MESSAGE(WM_USER+1, OnFileFilterUpdated)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFiltersDlg message handlers

void FileFiltersDlg::SetFileFilterHelper(FileFilterHelper* pFileFilterHelper)
{
	m_pFileFilterHelper->CloneFrom(pFileFilterHelper);
	m_pFileFilterHelperOrg = pFileFilterHelper;
	m_Filters = m_pFileFilterHelper->GetFileFilters();
}

/**
 * @brief Initialise listcontrol containing filters.
 */
void FileFiltersDlg::InitList()
{
	// Show selection across entire row.
	// Also enable infotips.
	m_listFilters.SetExtendedStyle(LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);

	const int lpx = CClientDC(this).GetDeviceCaps(LOGPIXELSX);
	auto pointToPixel = [lpx](int point) { return MulDiv(point, lpx, 72); };

	String title = _("Name");
	m_listFilters.InsertColumn(0, title.c_str(), LVCFMT_LEFT, pointToPixel(112));
	title = _("Description");
	m_listFilters.InsertColumn(1, title.c_str(), LVCFMT_LEFT, pointToPixel(262));
	title = _("Location");
	m_listFilters.InsertColumn(2, title.c_str(), LVCFMT_LEFT, pointToPixel(262));

	const int count = (int) m_pFileFilterHelper->GetFileFilters().size();
	for (int i = 0; i < count; i++)
		AddToGrid(i);
}

/**
 * @brief Select filter by index in the listview.
 * @param [in] index Index of filter to select.
 */
void FileFiltersDlg::SelectFilterByIndex(int index)
{
	m_listFilters.SetCheck(index, TRUE);
	bool bPartialOk = false;
	m_listFilters.EnsureVisible(index, bPartialOk);
}

/**
 * @brief Select filter by file path in the listview.
 * @param [in] path file path
 */
void FileFiltersDlg::SelectFilterByFilePath(const String& path)
{
	for (size_t i = 0; i < m_Filters.size(); ++i)
	{
		if (m_Filters[i].fullpath == path)
		{
			SelectFilterByIndex(static_cast<int>(i + 1));
			break;
		}
	}
}

/**
 * @brief Get preset filters from last group in filter expression.
 */
static std::vector<String> GetPresetFiltersFromLastGroup(const String& filterExpression)
{
	std::vector<String> presetFilters;
	std::vector<String> filterGroups = FileFilterHelper::SplitFilterGroups(filterExpression);
	String filterGroup = filterGroups.back();
	auto parts = strutils::split(filterGroup, ';');
	std::vector<String> result;
	for (const auto& part : parts)
	{
		const String partTrimmed = strutils::trim_ws(String(part.data(), part.length()));
		if (partTrimmed.substr(0, 3) == _T("pf:"))
			presetFilters.push_back(partTrimmed.substr(3));
	}
	return presetFilters;
}

/**
 * @brief Remove preset filters from filter expression and return the rest.
 */
static String RemovePresetFiltersFromLastGroup(const String& filterExpression)
{
	std::vector<String> filterGroups = FileFilterHelper::SplitFilterGroups(filterExpression);
	String filterGroup = filterGroups.back();
	auto parts = strutils::split(filterGroup, ';');
	std::vector<String> result;
	for (const auto& part : parts)
	{
		const String partTrimmed = strutils::trim_ws(String(part.data(), part.length()));
		if (partTrimmed.substr(0, 3) != _T("pf:"))
			result.push_back(partTrimmed);
	}
	filterGroups.back() = strutils::join(result.begin(), result.end(), _T(";"));
	return FileFilterHelper::JoinFilterGroups(filterGroups);
}

/**
 * @brief Add preset filters to last group in filter expression.
 */
static String AddPresetFiltersToLastGroup(const String& mask, const CListCtrl& list)
{
	std::vector<String> filterGroups = FileFilterHelper::SplitFilterGroups(mask);
	String result = filterGroups.back();
	const int count = list.GetItemCount();
	for (int i = 0; i < count; i++)
	{
		if (list.GetCheck(i))
		{
			if (!result.empty())
				result += _T(";");
			result += _T("pf:") + list.GetItemText(i, 0);
		}
	}
	filterGroups.back() = result;
	return FileFilterHelper::JoinFilterGroups(filterGroups);
}

static void SetCheckedState(CListCtrl& list, std::vector<String>& presetFilters)
{
	const int count = list.GetItemCount();
	for (int i = 0; i < count; i++)
	{
		String desc = list.GetItemText(i, 0);
		const bool isChecked = std::find(presetFilters.begin(), presetFilters.end(), desc) != presetFilters.end();
		list.SetCheck(i, isChecked);
	}
}

/**
 * @brief Called before dialog is shown.
 * @return Always TRUE.
 */
BOOL FileFiltersDlg::OnInitDialog()
{
	CTrPropertyPage::OnInitDialog();

	m_ctlMask.LoadState(_T("Files\\Ext"));

	InitList();

	SetDlgItemText(IDC_FILTERFILE_MASK, m_pFileFilterHelper->GetMaskOrExpression().c_str());

	std::vector<String> presetFilters = GetPresetFiltersFromLastGroup(m_pFileFilterHelper->GetMaskOrExpression());
	SetCheckedState(m_listFilters, presetFilters);

	SetButtonState();

	COMBOBOXINFO cbi{sizeof(COMBOBOXINFO)};
	GetComboBoxInfo(m_ctlMask.m_hWnd, &cbi);
	HWND hEdit = (HWND)m_ctlMask.SendMessage(CBEM_GETEDITCONTROL);
	m_ctlMaskEdit.SubclassWindow(cbi.hwndItem);
	m_ctlMaskEdit.m_validator = [this](const CString& text, CString& error) -> bool
		{
			m_pFileFilterHelper->SetMaskOrExpression((const tchar_t *)text);
			const bool bError = !m_pFileFilterHelper->GetErrorList().empty();
			if (bError)
			{
				for (const auto* errorItem : m_pFileFilterHelper->GetErrorList())
					error += (FormatFilterErrorSummary(*errorItem) + _T("\r\n")).c_str();
			}
			return !bError;
		};
	m_ctlMaskEdit.Validate();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief Add filter from filter-list index to dialog.
 * @param [in] filterIndex Index of filter to add.
 */
void FileFiltersDlg::AddToGrid(int filterIndex)
{
	const FileFilterInfo & filterinfo = m_Filters.at(filterIndex);
	const int item = filterIndex;

	m_listFilters.InsertItem(item, filterinfo.name.c_str());
	m_listFilters.SetItemText(item, 1, filterinfo.description.c_str());
	m_listFilters.SetItemText(item, 2, filterinfo.fullpath.c_str());
}

/**
 * @brief Called when dialog is closed with "OK" button.
 */
void FileFiltersDlg::OnOK()
{
	if (strutils::trim_ws(m_sMask).empty())
		m_sMask = _T("*.*");
	m_pFileFilterHelper->SetMaskOrExpression(m_sMask);
	m_pFileFilterHelperOrg->CloneFrom(m_pFileFilterHelper.get());

	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("FilterStartPage"), GetParentSheet()->GetActiveIndex());

	m_ctlMask.SetWindowTextW(m_sMask.c_str());
	m_ctlMask.SaveState(_T("Files\\Ext"));

	CDialog::OnOK();
}

void FileFiltersDlg::OnKillFocusFilterfileMask()
{
	UpdateData(TRUE);
	std::vector<String> presetFilters = GetPresetFiltersFromLastGroup(m_sMask);
	SetCheckedState(m_listFilters, presetFilters);
}

void FileFiltersDlg::OnEditChangeFilterfileMask()
{
	m_ctlMaskEdit.OnEnChange();
}

void FileFiltersDlg::OnFilterfileMaskMenu()
{
	UpdateData(TRUE);
	CRect rc;
	GetDlgItem(IDC_FILTERFILE_MASK_MENU)->GetWindowRect(&rc);
	const std::optional<String> filter = m_menu.ShowMenu(m_sMask, rc.left, rc.bottom, this);
	if (filter.has_value())
	{
		m_sMask = *filter;
		UpdateData(FALSE);
		std::vector<String> presetFilters = GetPresetFiltersFromLastGroup(m_sMask);
		SetCheckedState(m_listFilters, presetFilters);
		m_ctlMaskEdit.OnEnChange();
	}
}

/**
 * @brief Open selected filter for editing.
 *
 * This opens selected file filter file for user to edit. Other WinMerge UI is
 * not (anymore) blocked during editing. We let user continue working with
 * WinMerge while editing filter(s). Before opening this dialog and before
 * doing directory compare we re-load changed filter files from disk. So we
 * always compare with latest saved filters.
 * @sa CMainFrame::OnToolsFilters()
 * @sa CDirDoc::Rescan()
 * @sa FileFilterHelper::ReloadUpdatedFilters()
 */
void FileFiltersDlg::OnFiltersEditbtn()
{
	int sel = -1;
	sel = m_listFilters.GetNextItem(sel, LVNI_SELECTED);
	String path = m_listFilters.GetItemText(sel, 2);
	EditFileFilter(path);
	if (!m_pDirWatcher)
		m_pDirWatcher = std::make_unique<DirWatcher>();
	m_pDirWatcher->Remove(sel);
	m_pDirWatcher->Add(sel, false, path,
		[this](const String& path, DirWatcher::ACTION action)
		{
			PostMessage(WM_USER + 1);
		});
}

/**
 * @brief Edit file filter in external editor.
 * @param [in] path Full path to file filter to edit.
 */
void FileFiltersDlg::EditFileFilter(const String& path)
{
	CMergeApp::OpenFileToExternalEditor(path);
}

/**
 * @brief Edit selected filter when its double-clicked.
 * @param [in] pNMHDR List control item data.
 * @param [out] pResult Result of the action is returned in here.
 */
void FileFiltersDlg::OnDblclkFiltersList(NMHDR* pNMHDR, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pNMHDR);

	OnFiltersEditbtn();
	*pResult = 0;
}

void FileFiltersDlg::OnCustomDrawFiltersList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
	*pResult = CDRF_DODEFAULT;

	if (pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
	{
		int nItem = static_cast<int>(pLVCD->nmcd.dwItemSpec);
		if (auto pFilter = m_pFileFilterHelper->GetManager()->GetFilterByIndex(nItem))
		{
			if (!pFilter->errors.empty())
			{
				const COLORREF sysBk = GetSysColor(COLOR_WINDOW);
				const COLORREF bk = ((GetRValue(sysBk) + GetGValue(sysBk) + GetBValue(sysBk)) / 3 < 128) ? 
					RGB(80, 40, 40) : RGB(255, 200, 200);
				pLVCD->clrTextBk = bk;
			}
		}
		*pResult = CDRF_DODEFAULT;
	}
}

/**
 * @brief Called when item state is changed.
 *
 * Disable the "Test", "Edit" and "Remove" buttons when no item is selected.
 * @param [in] pNMHDR Listview item data.
 * @param [out] pResult Result of the action is returned in here.
 */
void FileFiltersDlg::OnLvnItemchangedFilterfileList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	// If item got selected or deselected
	if ((pNMLV->uNewState & LVIS_SELECTED) || (pNMLV->uOldState & LVIS_SELECTED))
	{
		SetButtonState();
	}
	if ((pNMLV->uChanged & LVIF_STATE) &&
		((pNMLV->uOldState & LVIS_STATEIMAGEMASK) != (pNMLV->uNewState & LVIS_STATEIMAGEMASK)))
	{
		m_sMask = RemovePresetFiltersFromLastGroup(m_sMask);
		m_sMask = AddPresetFiltersToLastGroup(m_sMask, m_listFilters);
		UpdateData(FALSE);
		m_ctlMaskEdit.OnEnChange();
	}
	*pResult = 0;
}

/**
 * @brief Called before infotip is shown to get infotip text.
 * @param [in] pNMHDR Listview item data.
 * @param [out] pResult Result of the action is returned in here.
 */
void FileFiltersDlg::OnInfoTip(NMHDR * pNMHDR, LRESULT * pResult)
{
	NMLVGETINFOTIP * pInfoTip = reinterpret_cast<NMLVGETINFOTIP*>(pNMHDR);
	ASSERT(pInfoTip != nullptr);

	auto* pFilter = m_pFileFilterHelper->GetManager()->GetFilterByIndex(pInfoTip->iItem);
	if (pFilter)
	{
		String strText;
		for (const auto& error : pFilter->errors)
			strText += FormatFilterErrorSummary(error) + _T("\r\n");
		_tcsncpy_s(pInfoTip->pszText, pInfoTip->cchTextMax, strText.c_str(), _TRUNCATE);
	}
	*pResult = 0;
}

/**
 * @brief Track mouse position for showing tooltips.
 * @param [in] nFlags Mouse movement flags.
 * @param [in] point Current mouse position.
 */
void FileFiltersDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	m_ptLastMousePos = point;
	CDialog::OnMouseMove(nFlags, point);
}

/**
 * @brief Called when user presses "Test" button.
 *
 * Asks filename for new filter from user (using standard
 * file picker dialog) and copies template file to that
 * name. Opens new filterfile for editing.
 * @todo (At least) Warn if user puts filter to outside
 * filter directories?
 */
void FileFiltersDlg::OnBnClickedFilterfileTestButton()
{
	UpdateData(TRUE);

	// Ensure filter is up-to-date (user probably just edited it)
	m_pFileFilterHelper->ReloadUpdatedFilters();

	const String mask = AddPresetFiltersToLastGroup(m_sMask, m_listFilters);
	m_pFileFilterHelper->SetMaskOrExpression(mask);

	CTestFilterDlg dlg(this, m_pFileFilterHelper.get());
	dlg.DoModal();
}

/**
 * @brief Called when user presses "New..." button.
 *
 * Asks filename for new filter from user (using standard
 * file picker dialog) and copies template file to that
 * name. Opens new filterfile for editing.
 * @todo (At least) Warn if user puts filter to outside
 * filter directories?
 * @todo Can global filter path be empty (I think not - Kimmo).
 */
void FileFiltersDlg::OnBnClickedFilterfileNewbutton()
{
	auto* pGlobalFileFilter = theApp.GetGlobalFileFilter();
	String globalPath = pGlobalFileFilter->GetGlobalFilterPathWithCreate();
	String userPath = pGlobalFileFilter->GetUserFilterPathWithCreate();

	if (globalPath.empty() && userPath.empty())
	{
		AfxMessageBox(
			_("User's filter folder is not defined!\n\nSelect filter folder in Options/System.").c_str(), MB_ICONSTOP);
		return;
	}

	// Format path to template file
	String templatePath = paths::ConcatPath(globalPath, FILE_FILTER_TEMPLATE);

	if (paths::DoesPathExist(templatePath) != paths::IS_EXISTING_FILE)
	{
		String msg = strutils::format_string2(
			_("Cannot find filter template!\n\nCopy %1 to WinMerge/Filters Folder:\n%2."),
			FILE_FILTER_TEMPLATE, templatePath);
		AfxMessageBox(msg.c_str(), MB_ICONERROR);
		return;
	}

	String path = globalPath.empty() ? userPath : globalPath;

	if (!globalPath.empty() && !userPath.empty())
	{
		CSharedFilterDlg dlg(
			GetOptionsMgr()->GetBool(OPT_FILEFILTER_SHARED) ? 
				CSharedFilterDlg::SHARED : CSharedFilterDlg::PRIVATE);
		if (dlg.DoModal() != IDOK)
			return;
		GetOptionsMgr()->SaveOption(OPT_FILEFILTER_SHARED, (dlg.GetSelectedFilterType() == CSharedFilterDlg::SHARED));
		path = dlg.GetSelectedFilterType() == CSharedFilterDlg::SHARED ? globalPath : userPath;
	}

	if (path.length())
		path = paths::AddTrailingSlash(path);
	
	String s;
	if (SelectFile(GetSafeHwnd(), s, false, path.c_str(), _("Select Filename for New Filter"),
		_("File Filters (*.flt)|*.flt|All Files (*.*)|*.*||")))
	{
		// Fix file extension
		tchar_t file[_MAX_FNAME] = {0};
		tchar_t ext[_MAX_EXT] = {0};
		tchar_t dir[_MAX_DIR] = {0};
		tchar_t drive[_MAX_DRIVE] = {0};
		_tsplitpath_s(s.c_str(), drive, _MAX_DRIVE, dir, _MAX_DIR, file, _MAX_FNAME, ext, _MAX_EXT);
		if (ext[0] == '\0')
		{
			s += FileFilterExt;
		}
		else if (tc::tcsicmp(ext, FileFilterExt) != 0)
		{
			s = drive;
			s += dir;
			s += file;
			s += FileFilterExt;
		}

		// Open-dialog asks about overwriting, so we can overwrite filter file
		// user has already allowed it.
		UniMemFile fileIn;
		UniStdioFile fileOut;
		if (!fileIn.OpenReadOnly(templatePath) || !fileOut.OpenCreate(s))
		{
			String msg = strutils::format_string1(
				_( "Cannot copy filter template:\n%1\n\nMake sure the folder exists and is writable."),
				templatePath);
			AfxMessageBox(msg.c_str(), MB_ICONERROR);
			return;
		}
		String lines;
		fileIn.ReadStringAll(lines);
		strutils::replace(lines, _T("${name}"), file);
		fileOut.WriteString(lines);
		fileIn.Close();
		fileOut.Close();

		EditFileFilter(s);
		FileFilterMgr *pMgr = pGlobalFileFilter->GetManager();
		int retval = pMgr->AddFilter(s);
		if (retval == FILTER_OK)
		{
			// Remove all from filterslist and re-add so we can update UI
			pGlobalFileFilter->LoadAllFileFilters();
			m_Filters = pGlobalFileFilter->GetFileFilters();

			UpdateFiltersList();
			SelectFilterByFilePath(s);
		}
	}
}

/**
 * @brief Delete selected filter.
 */
void FileFiltersDlg::OnBnClickedFilterfileDelete()
{
	int sel = -1;
	sel = m_listFilters.GetNextItem(sel, LVNI_SELECTED);
	const String path = m_listFilters.GetItemText(sel, 2);

	String sConfirm = strutils::format_string1(_("Are you sure you want to delete\n\n%1 ?"), path);
	int res = AfxMessageBox(sConfirm.c_str(), MB_ICONWARNING | MB_YESNO);
	if (res == IDYES)
	{
		if (DeleteFile(path.c_str()))
		{
			auto* pGlobalFileFilter = theApp.GetGlobalFileFilter();
			FileFilterMgr *pMgr = pGlobalFileFilter->GetManager();
			pMgr->RemoveFilter(path);
			
			// Remove all from filterslist and re-add so we can update UI
			m_Filters = pGlobalFileFilter->GetFileFilters();

			UpdateFiltersList();
		}
		else
		{
			String msg = strutils::format_string1(
				_("Failed to delete filter:\n%1\n\nFile may be read-only."),
				path);
			AfxMessageBox(msg.c_str(), MB_ICONSTOP);
		}
	}
	SetButtonState();
}

/**
 * @brief Update filters to list.
 */
void FileFiltersDlg::UpdateFiltersList()
{
	m_listFilters.DeleteAllItems();

	const int count = (int) m_Filters.size();
	for (int i = 0; i < count; i++)
	{
		AddToGrid(i);
	}
}

/**
 * @brief Open help from mainframe when user presses F1
 */
void FileFiltersDlg::OnHelp()
{
	theApp.ShowHelp(FilterHelpLocation);
}

/**
 * @brief Install new filter.
 * This function is called when user selects "Install" button from GUI.
 * Function allows easy installation of new filters for user. For example
 * when user has downloaded filter file from net. First we ask user to
 * select filter to install. Then we copy selected filter to private
 * filters folder.
 */
void FileFiltersDlg::OnBnClickedFilterfileInstall()
{
	auto* pGlobalFileFilter = theApp.GetGlobalFileFilter();
	String s;
	String path;

	if (SelectFile(GetSafeHwnd(), s, true, path.c_str(),_("Locate Filter File to Install"),
		_("File Filters (*.flt)|*.flt|All Files (*.*)|*.*||")))
	{
		String userPath = pGlobalFileFilter->GetUserFilterPathWithCreate();
		userPath = paths::ConcatPath(userPath, paths::FindFileName(s));
		if (!CopyFile(s.c_str(), userPath.c_str(), TRUE))
		{
			// If file already exists, ask from user
			// If user wants to, overwrite existing filter
			if (paths::DoesPathExist(userPath) == paths::IS_EXISTING_FILE)
			{
				int res = LangMessageBox(IDS_FILEFILTER_OVERWRITE, MB_YESNO |
					MB_ICONWARNING);
				if (res == IDYES)
				{
					if (!CopyFile(s.c_str(), userPath.c_str(), FALSE))
					{
						LangMessageBox(IDS_FILEFILTER_INSTALLFAIL, MB_ICONSTOP);
					}
				}
			}
			else
			{
				LangMessageBox(IDS_FILEFILTER_INSTALLFAIL, MB_ICONSTOP);
			}
		}
		else
		{
			FileFilterMgr *pMgr = pGlobalFileFilter->GetManager();
			pMgr->AddFilter(userPath);

			// Remove all from filterslist and re-add so we can update UI
			m_Filters = pGlobalFileFilter->GetFileFilters();

			UpdateFiltersList();
			SelectFilterByFilePath(userPath);
		}
	}
}

/**
 * @brief Disable the "Test", "Edit" and "Remove" buttons when no item is selected.
 */
void FileFiltersDlg::SetButtonState()
{
	int sel = -1;
	sel = m_listFilters.GetNextItem(sel, LVNI_SELECTED);
	const bool isNone = (sel == -1);

	EnableDlgItem(IDC_FILTERFILE_EDITBTN, !isNone);
	EnableDlgItem(IDC_FILTERFILE_DELETEBTN, !isNone);
}

LRESULT FileFiltersDlg::OnFileFilterUpdated(WPARAM wParam, LPARAM lParam)
{
	m_pFileFilterHelper->ReloadUpdatedFilters();
	m_listFilters.Invalidate();
	return 0;
}

