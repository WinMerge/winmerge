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
 * @file  PropRegistry.cpp
 *
 * @brief CPropRegistry implementation file
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "resource.h"
#include "PropRegistry.h"
#include "RegKey.h"
#include "coretools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// registry dir to WinMerge
static LPCTSTR f_RegDir = _T("Software\\Thingamahoochie\\WinMerge");

// registry values
static LPCTSTR f_RegValueEnabled = _T("ContextMenuEnabled");
static LPCTSTR f_RegValuePath = _T("Executable");

/////////////////////////////////////////////////////////////////////////////
// CPropRegistry dialog


CPropRegistry::CPropRegistry()
	: CPropertyPage(CPropRegistry::IDD)
{
	//{{AFX_DATA_INIT(CPropRegistry)
	m_bContextAdded = FALSE;
	m_bUseRecycleBin = TRUE;
	//}}AFX_DATA_INIT
}

void CPropRegistry::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropRegistry)
	DDX_Check(pDX, IDC_EXPLORER_CONTEXT, m_bContextAdded);
	DDX_Text(pDX, IDC_EXT_EDITOR_PATH, m_strEditorPath);
	DDX_Check(pDX, IDC_USE_RECYCLE_BIN, m_bUseRecycleBin);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPropRegistry, CDialog)
	//{{AFX_MSG_MAP(CPropRegistry)
	ON_BN_CLICKED(IDC_EXPLORER_CONTEXT, OnAddToExplorer)
	ON_BN_CLICKED(IDC_EXT_EDITOR_BROWSE, OnBrowseEditor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropRegistry message handlers

BOOL CPropRegistry::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	GetContextRegValues();
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/// Get registry values for ShellExtension
void CPropRegistry::GetContextRegValues()
{
	CRegKeyEx reg;
	if (reg.Open(HKEY_CURRENT_USER, f_RegDir) != ERROR_SUCCESS)
		return;

	// This will be bit mask, although now there is only one bit defined
	DWORD dwContextEnabled = reg.ReadDword(f_RegValueEnabled, 0);

	if (dwContextEnabled & 0x1)
		m_bContextAdded = TRUE;
}

/// Set registry values for ShellExtension
void CPropRegistry::OnAddToExplorer()
{
	UpdateData();

	CRegKeyEx reg;
	if (reg.Open(HKEY_CURRENT_USER, f_RegDir) != ERROR_SUCCESS)
		return;

	// This will be bit mask, although now there is only one bit defined
	DWORD dwContextEnabled = reg.ReadDword(f_RegValueEnabled, 0);

	if (m_bContextAdded)
		dwContextEnabled |= 0x01;
	else
		dwContextEnabled &= ~0x01;

	reg.WriteDword(f_RegValueEnabled, dwContextEnabled);
}

/// Saves given path to registry for ShellExtension
void CPropRegistry::SaveMergePath()
{
	TCHAR temp[MAX_PATH] = {0};
	GetModuleFileName(AfxGetInstanceHandle(), temp, MAX_PATH);

	CRegKeyEx reg;
	if (reg.Open(HKEY_CURRENT_USER, f_RegDir) != ERROR_SUCCESS)
		return;

	reg.WriteString(f_RegValuePath, temp);
}

/// Open file browse dialog to locate editor
void CPropRegistry::OnBrowseEditor()
{
	CString s;
	VERIFY(s.LoadString(IDS_PROGRAMFILES));
	DWORD flags = OFN_NOTESTFILECREATE | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
	CFileDialog pdlg(TRUE, NULL, _T(""), flags, s);
	CString title;
	VERIFY(title.LoadString(IDS_OPEN_TITLE));
	pdlg.m_ofn.lpstrTitle = (LPCTSTR)title;

	if (pdlg.DoModal() == IDOK)
	 	m_strEditorPath = pdlg.GetPathName();

	UpdateData(FALSE);
}
