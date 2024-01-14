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
FileFiltersDlg::FileFiltersDlg() : CTrPropertyPage(FileFiltersDlg::IDD)
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
	DDX_Control(pDX, IDC_FILTERFILE_LIST, m_listFilters);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(FileFiltersDlg, CTrPropertyPage)
	//{{AFX_MSG_MAP(FileFiltersDlg)
	ON_BN_CLICKED(IDC_FILTERFILE_EDITBTN, OnFiltersEditbtn)
	ON_NOTIFY(NM_DBLCLK, IDC_FILTERFILE_LIST, OnDblclkFiltersList)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_FILTERFILE_TEST_BTN, OnBnClickedFilterfileTestButton)
	ON_BN_CLICKED(IDC_FILTERFILE_NEWBTN, OnBnClickedFilterfileNewbutton)
	ON_BN_CLICKED(IDC_FILTERFILE_DELETEBTN, OnBnClickedFilterfileDelete)
	ON_COMMAND(ID_HELP, OnHelp)
	//}}AFX_MSG_MAP
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FILTERFILE_LIST, OnLvnItemchangedFilterfileList)
	ON_NOTIFY(LVN_GETINFOTIP, IDC_FILTERFILE_LIST, OnInfoTip)
	ON_BN_CLICKED(IDC_FILTERFILE_INSTALL, OnBnClickedFilterfileInstall)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFiltersDlg message handlers

/**
 * @brief Set array of filters.
 * @param [in] fileFilters Array of filters to show in the dialog.
 * @note Call this before actually showing the dialog.
 */
void FileFiltersDlg::SetFilterArray(const vector<FileFilterInfo>& fileFilters)
{
	m_Filters = fileFilters;
}

/**
 * @brief Returns path (cont. filename) of selected filter
 * @return Full path to selected filter file.
 */
String FileFiltersDlg::GetSelected()
{
	return m_sFileFilterPath;
}

/**
 * @brief Set path of selected filter.
 * @param [in] Path for selected filter.
 * @note Call this before actually showing the dialog.
 */
void FileFiltersDlg::SetSelected(const String & selected)
{
	m_sFileFilterPath = selected;
}

/**
 * @brief Initialise listcontrol containing filters.
 */
void FileFiltersDlg::InitList()
{
	// Show selection across entire row.
	// Also enable infotips.
	m_listFilters.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);

	const int lpx = CClientDC(this).GetDeviceCaps(LOGPIXELSX);
	auto pointToPixel = [lpx](int point) { return MulDiv(point, lpx, 72); };

	String title = _("Name");
	m_listFilters.InsertColumn(0, title.c_str(), LVCFMT_LEFT, pointToPixel(112));
	title = _("Description");
	m_listFilters.InsertColumn(1, title.c_str(), LVCFMT_LEFT, pointToPixel(262));
	title = _("Location");
	m_listFilters.InsertColumn(2, title.c_str(), LVCFMT_LEFT, pointToPixel(262));

	title = _("<None>");
	m_listFilters.InsertItem(1, title.c_str());
	m_listFilters.SetItemText(0, 1, title.c_str());
	m_listFilters.SetItemText(0, 2, title.c_str());

	const int count = (int) m_Filters.size();

	for (int i = 0; i < count; i++)
	{
		AddToGrid(i);
	}
}

/**
 * @brief Select filter by index in the listview.
 * @param [in] index Index of filter to select.
 */
void FileFiltersDlg::SelectFilterByIndex(int index)
{
	m_listFilters.SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);
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
 * @brief Called before dialog is shown.
 * @return Always TRUE.
 */
BOOL FileFiltersDlg::OnInitDialog()
{
	CTrPropertyPage::OnInitDialog();

	InitList();

	if (m_sFileFilterPath.empty())
	{
		SelectFilterByIndex(0);
		return TRUE;
	}

	int count = m_listFilters.GetItemCount();
	for (int i = 0; i < count; i++)
	{
		String desc = m_listFilters.GetItemText(i, 2);
		if (strutils::compare_nocase(desc, m_sFileFilterPath) == 0)
		{
			SelectFilterByIndex(i);
		}
	}

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
	const int item = filterIndex + 1;

	m_listFilters.InsertItem(item, filterinfo.name.c_str());
	m_listFilters.SetItemText(item, 1, filterinfo.description.c_str());
	m_listFilters.SetItemText(item, 2, filterinfo.fullpath.c_str());
}

/**
 * @brief Called when dialog is closed with "OK" button.
 */
void FileFiltersDlg::OnOK()
{
	int sel = m_listFilters.GetNextItem(-1, LVNI_SELECTED);
	m_sFileFilterPath = m_listFilters.GetItemText(sel, 2);

	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("FilterStartPage"), GetParentSheet()->GetActiveIndex());

	CDialog::OnOK();
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
	int sel =- 1;

	sel = m_listFilters.GetNextItem(sel, LVNI_SELECTED);

	// Can't edit first "None"
	if (sel > 0)
	{
		String path = m_listFilters.GetItemText(sel, 2);
		EditFileFilter(path);
	}
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
 * @brief Is item in list the <None> item?
 * @param [in] item Item to test.
 * @return true if item is <None> item.
 */
bool FileFiltersDlg::IsFilterItemNone(int item) const
{
	String txtNone = _("<None>");
	String txt = m_listFilters.GetItemText(item, 0);

	return (strutils::compare_nocase(txt, txtNone) == 0);
}

/**
 * @brief Called when item state is changed.
 *
 * Disable the "Test", "Edit" and "Remove" buttons when no item is selected or "None" filter is selected.
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

	int sel = m_listFilters.GetNextItem(-1, LVNI_SELECTED);
	if (sel == -1)
		return;
	if (IsFilterItemNone(sel))
		return;
	
	m_sFileFilterPath = m_listFilters.GetItemText(sel, 2);

	// Ensure filter is up-to-date (user probably just edited it)
	auto* pGlobalFileFilter = theApp.GetGlobalFileFilter();
	pGlobalFileFilter->ReloadUpdatedFilters();

	FileFilterMgr *pMgr = pGlobalFileFilter->GetManager();
	FileFilter * pFileFilter = pMgr->GetFilterByPath(m_sFileFilterPath);
	if (pFileFilter == nullptr)
		return;

	CTestFilterDlg dlg(this, pFileFilter, pMgr);
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
			_("User's filter file folder is not defined!\n\nPlease select filter folder in Options/System.").c_str(), MB_ICONSTOP);
		return;
	}

	// Format path to template file
	String templatePath = paths::ConcatPath(globalPath, FILE_FILTER_TEMPLATE);

	if (paths::DoesPathExist(templatePath) != paths::IS_EXISTING_FILE)
	{
		String msg = strutils::format_string2(
			_("Cannot find file filter template file!\n\nPlease copy file %1 to WinMerge/Filters folder:\n%2."),
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
	if (SelectFile(GetSafeHwnd(), s, false, path.c_str(), _("Select filename for new filter"),
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
				_( "Cannot copy filter template file to filter folder:\n%1\n\nPlease make sure the folder exists and is writable."),
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
			String selected;
			pGlobalFileFilter->LoadAllFileFilters();
			m_Filters = pGlobalFileFilter->GetFileFilters(selected);

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
	String path;
	int sel =- 1;

	sel = m_listFilters.GetNextItem(sel, LVNI_SELECTED);

	// Can't delete first "None"
	if (sel > 0)
	{
		path = m_listFilters.GetItemText(sel, 2);

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
				String selected;
				m_Filters = pGlobalFileFilter->GetFileFilters(selected);

				UpdateFiltersList();
			}
			else
			{
				String msg = strutils::format_string1(
					_("Failed to delete the filter file:\n%1\n\nMaybe the file is read-only?"),
					path);
				AfxMessageBox(msg.c_str(), MB_ICONSTOP);
			}
		}
	}
	SetButtonState();
}

/**
 * @brief Update filters to list.
 */
void FileFiltersDlg::UpdateFiltersList()
{
	int count = (int) m_Filters.size();

	m_listFilters.DeleteAllItems();

	String title = _("<None>");
	m_listFilters.InsertItem(1, title.c_str());
	m_listFilters.SetItemText(0, 1, title.c_str());
	m_listFilters.SetItemText(0, 2, title.c_str());

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

	if (SelectFile(GetSafeHwnd(), s, true, path.c_str(),_("Locate filter file to install"),
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
			String selected;
			m_Filters = pGlobalFileFilter->GetFileFilters(selected);

			UpdateFiltersList();
			SelectFilterByFilePath(userPath);
		}
	}
}

/**
 * @brief Disable the "Test", "Edit" and "Remove" buttons when no item is selected or "None" filter is selected.
 */
void FileFiltersDlg::SetButtonState()
{
	bool isNone = true;

	int sel = -1;
	sel = m_listFilters.GetNextItem(sel, LVNI_SELECTED);
	if (sel != -1)
	{
		String txtNone = _("<None>");
		String txt = m_listFilters.GetItemText(sel, 0);
		isNone = strutils::compare_nocase(txt, txtNone) == 0;
	}

	EnableDlgItem(IDC_FILTERFILE_TEST_BTN, !isNone);
	EnableDlgItem(IDC_FILTERFILE_EDITBTN, !isNone);
	EnableDlgItem(IDC_FILTERFILE_DELETEBTN, !isNone);
}
