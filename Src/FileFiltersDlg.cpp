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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFiltersDlg message handlers

/**
 * @brief Set array of filters.
 * @note Call this before actually showing the dialog.
 */
void FileFiltersDlg::SetFilterArray(StringPairArray * fileFilters)
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
	m_listFilters.SetExtendedStyle(m_listFilters.GetExtendedStyle() |
			LVS_EX_FULLROWSELECT );

	VERIFY(title.LoadString(IDS_FILTERFILE_NAMETITLE));
	m_listFilters.InsertColumn(0, title,LVCFMT_LEFT, 150);
	VERIFY(title.LoadString(IDS_FILTERFILE_PATHTITLE));
	m_listFilters.InsertColumn(1, title,LVCFMT_LEFT, 350);
//	m_listFilters.InsertColumn(2, _T("File"),LVCFMT_LEFT, 150);

	VERIFY(title.LoadString(IDS_USERCHOICE_NONE));
	m_listFilters.InsertItem(1, title);
	m_listFilters.SetItemText(0, 1, title);

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
	int count = m_listFilters.GetItemCount();
	for (int i = 0; i < count; i++)
	{
		m_listFilters.GetItemText(i, 1, desc.GetBuffer(MAX_PATH), MAX_PATH);
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
/*	CString file;
	CString path;
	CString ext;
	CString appDataPath;
	CString profilePath;
	CString profilePath2;
	CString winmergePath;
*/
	int i = filterIndex;

/*	winMergePath = GetModulePath() + _T("\\Filters\\");
	GetAppDataPath(appDataPath);
	appDataPath += _T("\\WinMerge\\Filters\\");
	GetUserProfilePath(profilePath);
	profilePath2 = profilePath;
	profilePath += _T("\\Local Settings\\Application Data\\WinMerge\\Filters\\");
	profilePath2 += _T("\\Application Data\\WinMerge\\Filters\\");

	SplitFileName(m_Filters->GetAt(i).first, &path, &file, &ext);

	if (path.CompareNoCase(winMergePath) == 0)
	{
		path = _T("ProgramDir");
	}
*/
	m_listFilters.InsertItem(i + 1, m_Filters->GetAt(i).second);

	m_listFilters.SetItemText(i + 1, 1, m_Filters->GetAt(i).first);

}

/**
 * @brief Called when dialog is closed with "OK" button.
 */
void FileFiltersDlg::OnOK()
{
	CString path;
	int sel =- 1;

	sel = m_listFilters.GetNextItem(sel, LVNI_SELECTED);
	m_listFilters.GetItemText(sel, 1, path.GetBuffer(MAX_PATH),
		MAX_PATH);
	path.ReleaseBuffer();
	m_sFileFilterPath = path;

	CDialog::OnOK();
}

/**
 * @brief Open selected filter for editing.
 * @note This function waits until editing is finished.
 */
void FileFiltersDlg::OnFiltersEditbtn()
{
	CString path;
	int sel =- 1;

	sel = m_listFilters.GetNextItem(sel, LVNI_SELECTED);

	// Can't edit first "None"
	if (sel > 0)
	{
		m_listFilters.GetItemText(sel, 1, path.GetBuffer(MAX_PATH),
			MAX_PATH);
		path.ReleaseBuffer();

		theApp.EditFileFilter(path);
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
