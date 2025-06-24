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
	DDX_Text(pDX, IDC_FILTERFILE_MASK, m_sMask);
	DDX_Control(pDX, IDC_FILTERFILE_LIST, m_listFilters);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(FileFiltersDlg, CTrPropertyPage)
	//{{AFX_MSG_MAP(FileFiltersDlg)
	ON_EN_KILLFOCUS(IDC_FILTERFILE_MASK, OnKillFocusFilterfileMask)
	ON_BN_CLICKED(IDC_FILTERFILE_EDITBTN, OnFiltersEditbtn)
	ON_NOTIFY(NM_DBLCLK, IDC_FILTERFILE_LIST, OnDblclkFiltersList)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_FILTERFILE_TEST_BTN, OnBnClickedFilterfileTestButton)
	ON_BN_CLICKED(IDC_FILTERFILE_NEWBTN, OnBnClickedFilterfileNewbutton)
	ON_BN_CLICKED(IDC_FILTERFILE_DELETEBTN, OnBnClickedFilterfileDelete)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FILTERFILE_LIST, OnLvnItemchangedFilterfileList)
	ON_NOTIFY(LVN_GETINFOTIP, IDC_FILTERFILE_LIST, OnInfoTip)
	ON_BN_CLICKED(IDC_FILTERFILE_INSTALL, OnBnClickedFilterfileInstall)
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
 * @brief Remove preset filters from filter expression and return the rest.
 */
static String RemovePresetFilters(const String& filterExpression, std::vector<String>& presetFilters)
{
	auto parts = strutils::split(filterExpression, ';');
	std::vector<String> result;
	for (const auto& part : parts)
	{
		const String partTrimmed = strutils::trim_ws(String(part.data(), part.length()));
		if (partTrimmed.substr(0, 3) == _T("fp:"))
			presetFilters.push_back(partTrimmed.substr(3));
		else
			result.push_back(partTrimmed);
	}
	return strutils::join(result.begin(), result.end(), _T(";"));
}

static void SetCheckedState(CListCtrl& list, std::vector<String>& presetFilters)
{
	const int count = list.GetItemCount();
	for (int i = 0; i < count; i++)
	{
		String desc = list.GetItemText(i, 0);
		const bool isChecked = std::find(presetFilters.begin(), presetFilters.end(), desc) != presetFilters.end();
		if (isChecked)
			list.SetCheck(i, true);
	}
}

/**
 * @brief Called before dialog is shown.
 * @return Always TRUE.
 */
BOOL FileFiltersDlg::OnInitDialog()
{
	CTrPropertyPage::OnInitDialog();

	InitList();

	std::vector<String> presetFilters;
	String filterExpression = RemovePresetFilters(m_pFileFilterHelper->GetFilterNameOrMask(), presetFilters);
	SetDlgItemText(IDC_FILTERFILE_MASK, filterExpression.c_str());

	SetCheckedState(m_listFilters, presetFilters);

	SetButtonState();

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
	String mask = m_sMask;
	const int count = m_listFilters.GetItemCount();
	for (int i = 0; i < count; i++)
	{
		const bool checked = m_listFilters.GetCheck(i);
		if (checked)
		{
			if (!mask.empty())
				mask += _T(";");
			mask += _T("fp:") + m_listFilters.GetItemText(i, 0);
		}
	}

	m_pFileFilterHelper->SetMask(mask);
	m_pFileFilterHelperOrg->CloneFrom(m_pFileFilterHelper.get());

	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("FilterStartPage"), GetParentSheet()->GetActiveIndex());

	CDialog::OnOK();
}

void FileFiltersDlg::OnKillFocusFilterfileMask()
{
	String filterExpressionOld;
	GetDlgItemText(IDC_FILTERFILE_MASK, filterExpressionOld);
	std::vector<String> presetFilters;
	String filterExpression = RemovePresetFilters(filterExpressionOld, presetFilters);
	if (filterExpression != filterExpressionOld)
	{
		SetDlgItemText(IDC_FILTERFILE_MASK, filterExpression.c_str());
		SetCheckedState(m_listFilters, presetFilters);
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
	*pResult = 0;
}

/**
 * @brief Called before infotip is shown to get infotip text.
 * @param [in] pNMHDR Listview item data.
 * @param [out] pResult Result of the action is returned in here.
 */
void FileFiltersDlg::OnInfoTip(NMHDR * pNMHDR, LRESULT * pResult)
{
	LVHITTESTINFO lvhti = {0};
	NMLVGETINFOTIP * pInfoTip = reinterpret_cast<NMLVGETINFOTIP*>(pNMHDR);
	ASSERT(pInfoTip != nullptr);

	// Get subitem under mouse cursor
	lvhti.pt = m_ptLastMousePos;
	m_listFilters.SubItemHitTest(&lvhti);

	if (lvhti.iSubItem > 1)
	{
		// Check that we are over icon or label
		if ((lvhti.flags & LVHT_ONITEMICON) || (lvhti.flags & LVHT_ONITEMLABEL))
		{
			// Set item text to tooltip
			String strText = m_listFilters.GetItemText(lvhti.iItem, lvhti.iSubItem);
			_tcscpy_s(pInfoTip->pszText, pInfoTip->cchTextMax, strText.c_str());
		}
	}
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

	EnableDlgItem(IDC_FILTERFILE_TEST_BTN, !isNone);
	EnableDlgItem(IDC_FILTERFILE_EDITBTN, !isNone);
	EnableDlgItem(IDC_FILTERFILE_DELETEBTN, !isNone);
}
