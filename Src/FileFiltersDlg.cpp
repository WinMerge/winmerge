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
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "merge.h"
#include "FileFiltersDlg.h"
#include "coretools.h"
#include "dllver.h"
#include "FileFilterMgr.h"
#include "paths.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const TCHAR FILE_FILTER_TEMPLATE[] = _T("FileFilter.tmpl");

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
	ON_BN_CLICKED(IDC_FILTERFILE_NEWBTN, OnBnClickedFilterfileNewbutton)
	ON_BN_CLICKED(IDC_FILTERFILE_DELETEBTN, OnBnClickedFilterfileDelete)
	//}}AFX_MSG_MAP
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FILTERFILE_LIST, OnLvnItemchangedFilterfileList)
	ON_NOTIFY(LVN_GETINFOTIP, IDC_FILTERFILE_LIST, OnInfoTip)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFiltersDlg message handlers

/**
 * @brief Set array of filters.
 * @note Call this before actually showing the dialog.
 */
void FileFiltersDlg::SetFilterArray(FILEFILTER_INFOLIST * fileFilters)
{
	m_Filters = fileFilters;
}

/**
 * @brief Returns path (cont. filename) of selected filter
 */
CString FileFiltersDlg::GetSelected()
{
	return m_sFileFilterPath;
}

/**
 * @brief Set path of selected filter.
 * @note Call this before actually showing the dialog.
 */
void FileFiltersDlg::SetSelected(CString selected)
{
	m_sFileFilterPath = selected;
}

/**
 * @brief Initialise listcontrol containing filters.
 */
void FileFiltersDlg::InitList()
{
	CString title;
	// Show selection across entire row.
	DWORD newstyle = LVS_EX_FULLROWSELECT;
	// Also enable infotips if they have new enough version for our
	// custom draw code
	// LPNMLVCUSTOMDRAW->iSubItem not supported before comctl32 4.71
	if (GetDllVersion(_T("comctl32.dll")) >= PACKVERSION(4,71))
		newstyle |= LVS_EX_INFOTIP;
	m_listFilters.SetExtendedStyle(m_listFilters.GetExtendedStyle() | newstyle);

	VERIFY(title.LoadString(IDS_FILTERFILE_NAMETITLE));
	m_listFilters.InsertColumn(0, title,LVCFMT_LEFT, 150);
	VERIFY(title.LoadString(IDS_FILTERFILE_DESCTITLE));
	m_listFilters.InsertColumn(1, title, LVCFMT_LEFT, 350);
	VERIFY(title.LoadString(IDS_FILTERFILE_PATHTITLE));
	m_listFilters.InsertColumn(2, title,LVCFMT_LEFT, 350);

	VERIFY(title.LoadString(IDS_USERCHOICE_NONE));
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
 * @brief Called before dialog is shown.
 */
BOOL FileFiltersDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitList();

	CString desc;
	if (m_sFileFilterPath.IsEmpty())
	{
		m_listFilters.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
		m_listFilters.EnsureVisible(0, FALSE);
		return TRUE;
	}

	int count = m_listFilters.GetItemCount();
	for (int i = 0; i < count; i++)
	{
		m_listFilters.GetItemText(i, 2, desc.GetBuffer(MAX_PATH), MAX_PATH);
		desc.ReleaseBuffer();
		if (desc.CompareNoCase(m_sFileFilterPath) == 0)
		{
			m_listFilters.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
			m_listFilters.EnsureVisible(i, FALSE);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief Add filter from filter-list index to dialog
 */
void FileFiltersDlg::AddToGrid(int filterIndex)
{
	m_listFilters.InsertItem(filterIndex + 1,
		m_Filters->GetAt(filterIndex).name);
	m_listFilters.SetItemText(filterIndex + 1, 1,
		m_Filters->GetAt(filterIndex).description);
	m_listFilters.SetItemText(filterIndex + 1, 2,
		m_Filters->GetAt(filterIndex).fullpath);
}

/**
 * @brief Called when dialog is closed with "OK" button.
 */
void FileFiltersDlg::OnOK()
{
	CString path;
	int sel =- 1;

	sel = m_listFilters.GetNextItem(sel, LVNI_SELECTED);
	m_listFilters.GetItemText(sel, 2, path.GetBuffer(MAX_PATH),	MAX_PATH);
	path.ReleaseBuffer();
	m_sFileFilterPath = path;

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
	CString path;
	int sel =- 1;

	sel = m_listFilters.GetNextItem(sel, LVNI_SELECTED);

	// Can't edit first "None"
	if (sel > 0)
	{
		m_listFilters.GetItemText(sel, 2, path.GetBuffer(MAX_PATH),
			MAX_PATH);
		path.ReleaseBuffer();

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
		CButton *btn = (CButton *) GetDlgItem(IDC_FILTERFILE_EDITBTN);
		CButton *btnDel = (CButton *) GetDlgItem(IDC_FILTERFILE_DELETEBTN);
		VERIFY(txtNone.LoadString(IDS_USERCHOICE_NONE));
		CString txt = m_listFilters.GetItemText(pNMLV->iItem, 0);

		if (txt.CompareNoCase(txtNone) == 0)
		{
			btn->EnableWindow(FALSE);
			btnDel->EnableWindow(FALSE);
		}
		else
		{
			btn->EnableWindow(TRUE);
			btnDel->EnableWindow(TRUE);
		}
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

/// Reload selected filter from disk (in case its been modified etc)
void FileFiltersDlg::OnBnClickedReload()
{
	FileFilterMgr *pMgr = NULL;
	FileFilter *pFilter = NULL;
	CString path;
	int sel =- 1;

	sel = m_listFilters.GetNextItem(sel, LVNI_SELECTED);

	// Can't edit first "None"
	if (sel > 0)
	{
		m_listFilters.GetItemText(sel, 2, path.GetBuffer(MAX_PATH),	MAX_PATH);
		path.ReleaseBuffer();

		pMgr = theApp.m_globalFileFilter.GetManager();
		pFilter = pMgr->GetFilterByPath(path);

		if (pFilter)
		{
			pMgr->ReloadFilterFromDisk(pFilter);
		}
	}
}

/**
 * @brief Called when user presses "New..." button.
 *
 * Asks filename for new filter from user (using standard
 * file picker dialog) and copies template file to that
 * name. Opens new filterfile for editing.
 * @todo (At least) Warn if user puts filter to outside
 * filter directories?
 */
void FileFiltersDlg::OnBnClickedFilterfileNewbutton()
{
	CString title;
	CString path;
	CString s;
	CString tmplPath;
	TCHAR dir[_MAX_DIR] = {0};
	TCHAR drive[_MAX_DRIVE] = {0};

	VERIFY(title.LoadString(IDS_FILEFILTER_SAVENEW));
	path = theApp.m_globalFileFilter.GetNewFileFilterPath();
	if (path.GetLength() && path[path.GetLength()-1] != '\\')
		path += '\\';

	
	tmplPath = path + FILE_FILTER_TEMPLATE;
	
	if (SelectFile(s, path, title, IDS_FILEFILTER_FILEMASK, FALSE))
	{
		// Fix file extension
		TCHAR file[_MAX_FNAME] = {0};
		TCHAR ext[_MAX_EXT] = {0};
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

		CopyFile(tmplPath, s, TRUE);
		theApp.m_globalFileFilter.EditFileFilter(s);
		FileFilterMgr *pMgr = theApp.m_globalFileFilter.GetManager();
		pMgr->AddFilter(s);

		// Remove all from filterslist and re-add so we can update UI
		CString selected;
		m_Filters->RemoveAll();
		theApp.m_globalFileFilter.GetFileFilters(m_Filters, selected);

		UpdateFiltersList();
	}
}

/**
 * @brief Delete selected filter.
 * @todo Error message for failed file delete?
 */
void FileFiltersDlg::OnBnClickedFilterfileDelete()
{
	CString path;
	int sel =- 1;

	sel = m_listFilters.GetNextItem(sel, LVNI_SELECTED);

	// Can't edit first "None"
	if (sel > 0)
	{
		m_listFilters.GetItemText(sel, 2, path.GetBuffer(MAX_PATH),	MAX_PATH);
		path.ReleaseBuffer();

		CString sConfirm;
		AfxFormatString1(sConfirm, IDS_CONFIRM_DELETE_SINGLE, path);
		int res = AfxMessageBox(sConfirm, MB_ICONQUESTION | MB_YESNO);
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
		}
	}
}

/**
 * @brief Update filters to list.
 */
void FileFiltersDlg::UpdateFiltersList()
{
	int count = m_Filters->GetSize();
	int listItems = m_listFilters.GetItemCount();

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
