/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
/////////////////////////////////////////////////////////////////////////////
// OpenDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Merge.h"
#include "OpenDlg.h"
#include "coretools.h"
#include "StringEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define DIRSEL_TAG   _T("Directory Selection")


/////////////////////////////////////////////////////////////////////////////
// COpenDlg dialog


COpenDlg::COpenDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COpenDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COpenDlg)
	m_strLeft = _T("");
	m_strRight = _T("");
	m_bRecurse = FALSE;
	m_strExt = _T("*.*");
	//}}AFX_DATA_INIT
	
	m_strParsedExt = _T(".*");
}


void COpenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COpenDlg)
	DDX_Control(pDX, IDC_EXT_COMBO, m_ctlExt);
	DDX_Control(pDX, IDOK, m_ctlOk);
	DDX_Control(pDX, IDC_RECURS_CHECK, m_ctlRecurse);
	DDX_Control(pDX, IDC_RIGHT_COMBO, m_ctlRight);
	DDX_Control(pDX, IDC_LEFT_COMBO, m_ctlLeft);
	DDX_CBString(pDX, IDC_LEFT_COMBO, m_strLeft);
	DDX_CBString(pDX, IDC_RIGHT_COMBO, m_strRight);
	DDX_Check(pDX, IDC_RECURS_CHECK, m_bRecurse);
	DDX_CBString(pDX, IDC_EXT_COMBO, m_strExt);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COpenDlg, CDialog)
	//{{AFX_MSG_MAP(COpenDlg)
	ON_BN_CLICKED(IDC_LEFT_BUTTON, OnLeftButton)
	ON_BN_CLICKED(IDC_RIGHT_BUTTON, OnRightButton)
	ON_CBN_SELCHANGE(IDC_LEFT_COMBO, OnSelchangeLeftCombo)
	ON_CBN_SELCHANGE(IDC_RIGHT_COMBO, OnSelchangeRightCombo)
	ON_CBN_EDITCHANGE(IDC_LEFT_COMBO, UpdateButtonStates)
	ON_CBN_SELENDCANCEL(IDC_LEFT_COMBO, UpdateButtonStates)
	ON_CBN_EDITCHANGE(IDC_RIGHT_COMBO, UpdateButtonStates)
	ON_CBN_SELENDCANCEL(IDC_RIGHT_COMBO, UpdateButtonStates)
	ON_CBN_KILLFOCUS(IDC_LEFT_COMBO, OnKillfocusLeftCombo)
	ON_CBN_KILLFOCUS(IDC_RIGHT_COMBO, OnKillfocusRightCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenDlg message handlers

void COpenDlg::OnLeftButton() 
{
	CString s;
	TCHAR folder[MAX_PATH]=_T(""), name[MAX_PATH];
	CFileStatus status;
	UpdateData(TRUE); 

	if (CFile::GetStatus(m_strLeft, status)
		&& (status.m_attribute & CFile::Attribute::directory))
			_tcscpy(folder, m_strLeft);
	else
		split_filename(m_strLeft, folder, NULL, NULL);
	if (SelectFile(s, folder))
	{
		split_filename(s, folder, name, NULL);
		if (!_tcscmp(name, DIRSEL_TAG))
		{
			m_strLeft = folder;
		}
		else
			m_strLeft = s;
		UpdateData(FALSE);
		UpdateButtonStates();
	}	
}

void COpenDlg::OnRightButton() 
{
	CString s;
	TCHAR folder[MAX_PATH]=_T(""), name[MAX_PATH];
	CFileStatus status;
	UpdateData(TRUE);

	if (CFile::GetStatus(m_strRight, status)
		&& (status.m_attribute & CFile::Attribute::directory))
			_tcscpy(folder, m_strRight);
	else 
		split_filename(m_strRight, folder, NULL, NULL);
	if (SelectFile(s, folder))
	{
		split_filename(s, folder, name, NULL);
		if (!_tcscmp(name, DIRSEL_TAG))
			m_strRight = folder;
		else
			m_strRight = s;
		UpdateData(FALSE);
		UpdateButtonStates();
	}	
}

void COpenDlg::OnOK() 
{
	UpdateData(TRUE);

	if (m_strLeft.Right(1)==_T(':'))
		m_strLeft += _T("\\");
	if (m_strRight.Right(1)==_T(':'))
		m_strRight += _T("\\");
	UpdateData(FALSE);

	// parse the extensions
	// replace all *. with .*\\.
	int idx=0;
	LPCTSTR pszSeps = _T("; |*%^&.,\\/<>:\"'`?\t\r\n");
	CStringEx strExt(m_strExt);
	strExt.TrimLeft();
	strExt.TrimRight();

	TCHAR ext[2048];
	_tcscpy(ext, strExt);
	LPTSTR p;
	CString strPattern(_T("^.*\\.("));

	p = _tcstok(ext, pszSeps);
	if (p == NULL)
		m_strParsedExt = _T(".*");
	else
	{
		while (p != NULL)
		{
			strPattern += p;		
			p = _tcstok(NULL, pszSeps);
			if (p != NULL)
				strPattern += _T('|');
		}
		strPattern += _T(")$");
		m_strParsedExt = strPattern;
	}

	m_ctlLeft.SaveState(_T("Files\\Left"));
	m_ctlRight.SaveState(_T("Files\\Right"));
	m_ctlExt.SaveState(_T("Files\\Ext"));

	CDialog::OnOK();
}

BOOL COpenDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	
	m_ctlLeft.LoadState(_T("Files\\Left"));
	m_ctlRight.LoadState(_T("Files\\Right"));
	m_ctlExt.LoadState(_T("Files\\Ext"));
	UpdateData(m_strLeft.IsEmpty() && m_strRight.IsEmpty());
	UpdateButtonStates();

	return TRUE;  
}

void COpenDlg::UpdateButtonStates()
{
	BOOL bLIsDir, bRIsDir;
	UpdateData(TRUE);

	// only enable OK button if both file exist
	BOOL bEnableOK = (IsFileOk(m_strLeft, &bLIsDir) && IsFileOk(m_strRight, &bRIsDir));
	m_ctlOk.EnableWindow(bEnableOK);
	m_ctlRecurse.EnableWindow(bEnableOK && bLIsDir && bRIsDir);
}

BOOL COpenDlg::SelectFile(CString& path, LPCTSTR pszFolder) 
{
	CString s;           
                   
	s.LoadString(IDS_ALLFILES); 
	CFileDialog *pdlg = new CFileDialog(TRUE, NULL, NULL, 
				    OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, s);
	if (NULL != pdlg)
	{
		TCHAR buf[MAX_PATH] = DIRSEL_TAG;
		CString title;
		VERIFY(title.LoadString(IDS_OPEN_TITLE));
		pdlg->m_ofn.lpstrTitle = (LPCTSTR)title;
		pdlg->m_ofn.lpstrInitialDir = (LPSTR)pszFolder;
		pdlg->m_ofn.nMaxFile = MAX_PATH;
		pdlg->m_ofn.lpstrFile = buf;

		if (pdlg->DoModal()==IDOK)
		{
	 		path = pdlg->GetPathName(); 
			delete pdlg;
	 		return TRUE;
		}
		path.Empty();
		delete pdlg;
	}
	return FALSE;	   
}

void COpenDlg::OnSelchangeLeftCombo() 
{
	int sel = m_ctlLeft.GetCurSel();
	if (sel != CB_ERR)
	{
		m_ctlLeft.GetLBText(sel, m_strLeft);
		m_ctlLeft.SetWindowText(m_strLeft);
		UpdateData(TRUE);
	}
	UpdateButtonStates();
	
}

void COpenDlg::OnSelchangeRightCombo() 
{
	int sel = m_ctlRight.GetCurSel();
	if (sel != CB_ERR)
	{
		m_ctlRight.GetLBText(sel, m_strRight);
		m_ctlRight.SetWindowText(m_strRight);
		UpdateData(TRUE);
	}
	UpdateButtonStates();
}

void COpenDlg::OnKillfocusLeftCombo() 
{
	UpdateData(TRUE);
	// remove trailing slashes
	RemoveTrailingSlash(m_strLeft);
	m_ctlLeft.SetWindowText(m_strLeft);
	UpdateButtonStates();
}

void COpenDlg::OnKillfocusRightCombo() 
{
	UpdateData(TRUE);
	RemoveTrailingSlash(m_strRight);
	m_ctlRight.SetWindowText(m_strRight);
	UpdateButtonStates();
}

BOOL COpenDlg::IsFileOk(CString & strFile, BOOL *pbDir /*= NULL*/)
{
	BOOL bResult=FALSE;
	CFileStatus status;
	CString s(strFile);

	while (s.Right(1) == _T('\\') || s.Right(1) == _T('/'))
		s = s.Left(s.GetLength()-1);

	// fix bug #121116
	// I guess GetStatus doesn't like stuff like "F:"
	if (s.GetLength()==2 && s.Right(1) == _T(':'))
	{
		TCHAR temp[100];
		TCHAR drive = toupper(s.GetAt(0));

		if (GetLogicalDriveStrings(100,temp))
		{
			LPTSTR p;
			for (p=temp; *p != _T('\0'); )
			{
				if (toupper(*p) == drive)
				{
					bResult=TRUE;
					break;
				}
				p = _tcsninc(p,_tcslen(p)+1);
			}
		}

		if (pbDir != NULL)
			*pbDir = TRUE;
	}
	else
	{
		bResult = CFile::GetStatus(s, status);
		if (pbDir != NULL)
			*pbDir = (status.m_attribute & CFile::Attribute::directory);
	}

	return bResult;
}

void COpenDlg::RemoveTrailingSlash(CString & s)
{
	while (s.Right(1) == _T('\\') || s.Right(1) == _T('/'))
		s = s.Left(s.GetLength()-1);
}
