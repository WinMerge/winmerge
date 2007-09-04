/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or (at
//    your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/**
 * @file  FileFiltersDlg.cpp
 *
 * @brief Implementation of FileFilters -dialog
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include "UnicodeString.h"
#include "merge.h"
#include "MainFrm.h"
#include "FileFiltersDlg.h"
#include "coretools.h"
#include "dllver.h"
#include "FileFilterMgr.h"
#include "paths.h"
#include "SharedFilterDlg.h"
#include "TestFilterDlg.h"
#include "FileOrFolderSelect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/** @brief Template file used when creating new filefilter. */
static const TCHAR FILE_FILTER_TEMPLATE[] = _T("FileFilter.tmpl");

/** @brief Location for filters specific help to open. */
static const TCHAR FilterHelpLocation[] = _T("::/htmlhelp/Filters.html");

/////////////////////////////////////////////////////////////////////////////
// CFiltersDlg dialog
IMPLEMENT_DYNCREATE(FileFiltersDlg, CPropertyPage)

FileFiltersDlg::FileFiltersDlg() : CPropertyPage(FileFiltersDlg::IDD)
{
	//{{AFX_DATA_INIT(FileFiltersDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void FileFiltersDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(FileFiltersDlg)
	DDX_Control(pDX, IDC_FILTERFILE_LIST, m_listFilters);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(FileFiltersDlg, CDialog)
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
void FileFiltersDlg::SetFilterArray(FILEFILTER_INFOLIST * fileFilters)
{
	m_Filters = fileFilters;
}

/**
 * @brief Returns path (cont. filename) of selected filter
 * @return Full path to selected filter file.
 */
CString FileFiltersDlg::GetSelected()
{
	return m_sFileFilterPath;
}

/**
 * @brief Set path of selected filter.
 * @param [in] Path for selected filter.
 * @note Call this before actually showing the dialog.
 */
void FileFiltersDlg::SetSelected(const CString & selected)
{
	m_sFileFilterPath = selected;
}

/**
 * @brief Initialise listcontrol containing filters.
 */
void FileFiltersDlg::InitList()
{
	// Show selection across entire row.
	DWORD newstyle = LVS_EX_FULLROWSELECT;
	// Also enable infotips if they have new enough version for our
	// custom draw code
	// LPNMLVCUSTOMDRAW->iSubItem not supported before comctl32 4.71
	if (GetDllVersion(_T("comctl32.dll")) >= PACKVERSION(4,71))
		newstyle |= LVS_EX_INFOTIP;
	m_listFilters.SetExtendedStyle(m_listFilters.GetExtendedStyle() | newstyle);

	CString title = LoadResString(IDS_FILTERFILE_NAMETITLE);
	m_listFilters.InsertColumn(0, title,LVCFMT_LEFT, 150);
	title = LoadResString(IDS_FILTERFILE_DESCTITLE);
	m_listFilters.InsertColumn(1, title, LVCFMT_LEFT, 350);
	title = LoadResString(IDS_FILTERFILE_PATHTITLE);
	m_listFilters.InsertColumn(2, title,LVCFMT_LEFT, 350);

	title = LoadResString(IDS_USERCHOICE_NONE);
	m_listFilters.InsertItem(1, title);
	m_listFilters.SetItemText(0, 1, title);
	m_listFilters.SetItemText(0, 2, title);

	int count = m_Filters->GetSize();

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
	BOOL bPartialOk = FALSE;
	m_listFilters.EnsureVisible(index, bPartialOk);
}

/**
 * @brief Called before dialog is shown.
 */
BOOL FileFiltersDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitList();

	if (m_sFileFilterPath.IsEmpty())
	{
		SelectFilterByIndex(0);
		return TRUE;
	}

	int count = m_listFilters.GetItemCount();
	for (int i = 0; i < count; i++)
	{
		CString desc = m_listFilters.GetItemText(i, 2);
		if (desc.CompareNoCase(m_sFileFilterPath) == 0)
		{
			SelectFilterByIndex(i);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief Add filter from filter-list index to dialog.
 * @param [in] filterIndex Index of filter to add.
 */
void FileFiltersDlg::AddToGrid(int filterIndex)
{
	const FileFilterInfo & filterinfo = m_Filters->GetAt(filterIndex);
	const int item = filterIndex + 1;

	m_listFilters.InsertItem(item, filterinfo.name);
	m_listFilters.SetItemText(item, 1, filterinfo.description);
	m_listFilters.SetItemText(item, 2, filterinfo.fullpath);
}

/**
 * @brief Called when dialog is closed with "OK" button.
 */
void FileFiltersDlg::OnOK()
{
	int sel = m_listFilters.GetNextItem(-1, LVNI_SELECTED);
	m_sFileFilterPath = m_listFilters.GetItemText(sel, 2);

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
		CString path = m_listFilters.GetItemText(sel, 2);
		theApp.m_globalFileFilter.EditFileFilter(path);
	}
}

/**
 * @brief Edit selected filter when its double-clicked
 */
void FileFiltersDlg::OnDblclkFiltersList(NMHDR* pNMHDR, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pNMHDR);

	OnFiltersEditbtn();
	*pResult = 0;
}

/**
 * @brief Shortcut to enable or disable a control.
 * @param [in] parent Pointer to dialog.
 * @param [in] item Control's resourceID in dialog.
 * @param [in] enable TRUE if item is enabled, FALSE if disabled.
 */
static void EnableDlgItem(CWnd * parent, int item, bool enable)
{
	parent->GetDlgItem(item)->EnableWindow(!!enable);
}

/**
 * @brief Is item in list the <None> item?
 * @param [in] item Item to test.
 * @return true if item is <None> item.
 */
bool FileFiltersDlg::IsFilterItemNone(int item) const
{
	CString txtNone = LoadResString(IDS_USERCHOICE_NONE);
	CString txt = m_listFilters.GetItemText(item, 0);

	return (txt.CompareNoCase(txtNone) == 0);
}

/**
 * @brief Called when item state is changed.
 *
 * Disable Edit-button when "None" filter is selected.
 */
void FileFiltersDlg::OnLvnItemchangedFilterfileList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	// If item got selected
	if (pNMLV->uNewState & LVIS_SELECTED)
	{
		CString txtNone;
		VERIFY(txtNone.LoadString(IDS_USERCHOICE_NONE));
		CString txt = m_listFilters.GetItemText(pNMLV->iItem, 0);

		bool isNone = (txt.CompareNoCase(txtNone) == 0);

		EnableDlgItem(this, IDC_FILTERFILE_TEST_BTN, !isNone);
		EnableDlgItem(this, IDC_FILTERFILE_EDITBTN, !isNone);
		EnableDlgItem(this, IDC_FILTERFILE_DELETEBTN, !isNone);
	}
	*pResult = 0;
}

/// Called before infotip is shown to get infotip text
void FileFiltersDlg::OnInfoTip(NMHDR * pNMHDR, LRESULT * pResult)
{
	LVHITTESTINFO lvhti = {0};
	NMLVGETINFOTIP * pInfoTip = reinterpret_cast<NMLVGETINFOTIP*>(pNMHDR);
	ASSERT(pInfoTip);

	if (GetDllVersion(_T("comctl32.dll")) < PACKVERSION(4,71))
	{
		// LPNMLVCUSTOMDRAW->iSubItem not supported before comctl32 4.71
		return;
	}

	// Get subitem under mouse cursor
	lvhti.pt = m_ptLastMousePos;
	m_listFilters.SubItemHitTest(&lvhti);

	if (lvhti.iSubItem > 1)
	{
		// Check that we are over icon or label
		if ((lvhti.flags & LVHT_ONITEMICON) || (lvhti.flags & LVHT_ONITEMLABEL))
		{
			// Set item text to tooltip
			CString strText = m_listFilters.GetItemText(lvhti.iItem, lvhti.iSubItem);
			_tcscpy(pInfoTip->pszText, strText);
		}
	}
}

/// Track mouse position for showing tooltips
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
	if (sel == -1) return;
	if (IsFilterItemNone(sel)) return;
	
	m_sFileFilterPath = m_listFilters.GetItemText(sel, 2);

	// Ensure filter is up-to-date (user probably just edited it)
	theApp.m_globalFileFilter.ReloadUpdatedFilters();

	FileFilterMgr *pMgr = theApp.m_globalFileFilter.GetManager();
	FileFilter * pFileFilter = pMgr->GetFilterByPath(m_sFileFilterPath);
	if (!pFileFilter) return;

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
	CString title = LoadResString(IDS_FILEFILTER_SAVENEW);
	CString globalPath = theApp.m_globalFileFilter.GetGlobalFilterPathWithCreate();
	CString userPath = theApp.m_globalFileFilter.GetUserFilterPathWithCreate();

	if (globalPath.IsEmpty() && userPath.IsEmpty())
	{
		AfxMessageBox(IDS_FILEFILTER_NO_USERFOLDER, MB_ICONSTOP);
		return;
	}

	// Format path to template file
	CString templatePath(globalPath);
	if (templatePath[templatePath.GetLength()-1] != '\\')
		templatePath += "\\";
	templatePath += FILE_FILTER_TEMPLATE;

	if (paths_DoesPathExist(templatePath) != IS_EXISTING_FILE)
	{
		CString msg;
		AfxFormatString2(msg, IDS_FILEFILTER_TMPL_MISSING,
			FILE_FILTER_TEMPLATE, templatePath);
		AfxMessageBox(msg, MB_ICONERROR);
		return;
	}

	CString path = (globalPath.IsEmpty() ? userPath : globalPath);

	if (!globalPath.IsEmpty() && !userPath.IsEmpty())
	{
		path = CSharedFilterDlg::PromptForNewFilter(this, globalPath, userPath);
		if (path.IsEmpty()) return;
	}

	if (path.GetLength() && path[path.GetLength()-1] != '\\')
		path += '\\';
	
	CString s;
	if (SelectFile(GetSafeHwnd(), s, path, title, IDS_FILEFILTER_FILEMASK,
		FALSE))
	{
		// Fix file extension
		TCHAR file[_MAX_FNAME] = {0};
		TCHAR ext[_MAX_EXT] = {0};
		TCHAR dir[_MAX_DIR] = {0};
		TCHAR drive[_MAX_DRIVE] = {0};
		_tsplitpath(s, drive, dir, file, ext);
		if (_tcslen(ext) == 0)
		{
			s += FileFilterExt;
		}
		else if (_tcsicmp(ext, FileFilterExt) != 0)
		{
			s = drive;
			s += dir;
			s += file;
			s += FileFilterExt;
		}

		// Open-dialog asks about overwriting, so we can overwrite filter file
		// user has already allowed it.
		if (!CopyFile(templatePath, s, FALSE))
		{
			ResMsgBox1(IDS_FILEFILTER_TMPL_COPY, templatePath, MB_ICONERROR);
			return;
		}
		theApp.m_globalFileFilter.EditFileFilter(s);
		FileFilterMgr *pMgr = theApp.m_globalFileFilter.GetManager();
		int retval = pMgr->AddFilter(s);
		if (retval == FILTER_OK)
		{
			// Remove all from filterslist and re-add so we can update UI
			CString selected;
			m_Filters->RemoveAll();
			theApp.m_globalFileFilter.LoadAllFileFilters();
			theApp.m_globalFileFilter.GetFileFilters(m_Filters, selected);

			UpdateFiltersList();
		}
	}
}

/**
 * @brief Delete selected filter.
 */
void FileFiltersDlg::OnBnClickedFilterfileDelete()
{
	CString path;
	int sel =- 1;

	sel = m_listFilters.GetNextItem(sel, LVNI_SELECTED);

	// Can't delete first "None"
	if (sel > 0)
	{
		m_listFilters.GetItemText(sel, 2, path.GetBuffer(MAX_PATH),	MAX_PATH);
		path.ReleaseBuffer();

		CString sConfirm;
		AfxFormatString1(sConfirm, IDS_CONFIRM_DELETE_SINGLE, path);
		int res = AfxMessageBox(sConfirm, MB_ICONWARNING | MB_YESNO);
		if (res == IDYES)
		{
			if (DeleteFile(path))
			{
				FileFilterMgr *pMgr = theApp.m_globalFileFilter.GetManager();
				pMgr->RemoveFilter(path);
				
				// Remove all from filterslist and re-add so we can update UI
				CString selected;
				m_Filters->RemoveAll();
				theApp.m_globalFileFilter.GetFileFilters(m_Filters, selected);

				UpdateFiltersList();
			}
			else
			{
				ResMsgBox1(IDS_FILEFILTER_DELETE_FAIL, path, MB_ICONSTOP);
			}
			
		}
	}
}

/**
 * @brief Update filters to list.
 */
void FileFiltersDlg::UpdateFiltersList()
{
	int count = m_Filters->GetSize();

	m_listFilters.DeleteAllItems();

	CString title;
	VERIFY(title.LoadString(IDS_USERCHOICE_NONE));
	m_listFilters.InsertItem(1, title);
	m_listFilters.SetItemText(0, 1, title);
	m_listFilters.SetItemText(0, 2, title);

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
	GetMainFrame()->ShowHelp(FilterHelpLocation);
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
	CString s;
	CString path;
	CString userPath = theApp.m_globalFileFilter.GetUserFilterPathWithCreate();
	CString title = LoadResString(IDS_FILEFILTER_INSTALL);

	if (SelectFile(GetSafeHwnd(), s, path, title, IDS_FILEFILTER_FILEMASK,
		TRUE))
	{
		String sfile, sext;
		SplitFilename(s, NULL, &sfile, &sext);
		CString filename(sfile.c_str());
		CString ext(sext.c_str());
		filename += _T(".");
		filename += ext;
		userPath = paths_ConcatPath(userPath, filename);
		if (!CopyFile(s, userPath, TRUE))
		{
			// If file already exists, ask from user
			// If user wants to, overwrite existing filter
			if (paths_DoesPathExist(userPath) == IS_EXISTING_FILE)
			{
				int res = AfxMessageBox(IDS_FILEFILTER_OVERWRITE, MB_YESNO |
					MB_ICONWARNING);
				if (res == IDYES)
				{
					if (!CopyFile(s, userPath, FALSE))
					{
						AfxMessageBox(IDS_FILEFILTER_INSTALLFAIL, MB_ICONSTOP);
					}
				}
			}
			else
			{
				AfxMessageBox(IDS_FILEFILTER_INSTALLFAIL, MB_ICONSTOP);
			}
		}
		else
		{
			FileFilterMgr *pMgr = theApp.m_globalFileFilter.GetManager();
			pMgr->AddFilter(userPath);

			// Remove all from filterslist and re-add so we can update UI
			CString selected;
			m_Filters->RemoveAll();
			theApp.m_globalFileFilter.GetFileFilters(m_Filters, selected);

			UpdateFiltersList();
		}
	}
}
