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
 * @file  PatchDlg.cpp
 *
 * @brief Implementation of Patch creation dialog
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "merge.h"
#include "PatchDlg.h"
#include "diff.h"
#include "coretools.h"
#include <sys/types.h>	// struct stat
#include <sys/stat.h>	// struct stat & _fstat()

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int recursive;

/////////////////////////////////////////////////////////////////////////////
// CPatchDlg dialog


CPatchDlg::CPatchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPatchDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPatchDlg)
	m_caseSensitive = FALSE;
	m_file1 = _T("");
	m_file2 = _T("");
	m_fileResult = _T("");
	m_ignoreBlanks = FALSE;
	m_whitespaceCompare = -1;
	//}}AFX_DATA_INIT
}


void CPatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPatchDlg)
	DDX_Control(pDX, IDC_DIFF_STYLE, m_comboStyle);
	DDX_Control(pDX, IDC_DIFF_CONTEXT, m_comboContext);
	DDX_Check(pDX, IDC_DIFF_CASESENSITIVE, m_caseSensitive);
	DDX_Text(pDX, IDC_DIFF_FILE1, m_file1);
	DDX_Text(pDX, IDC_DIFF_FILE2, m_file2);
	DDX_Text(pDX, IDC_DIFF_FILERESULT, m_fileResult);
	DDX_Check(pDX, IDC_DIFF_WHITESPACE_IGNOREBLANKS, m_ignoreBlanks);
	DDX_Radio(pDX, IDC_DIFF_WHITESPACE_COMPARE, m_whitespaceCompare);
	DDX_Check(pDX, IDC_DIFF_APPENDFILE, m_appendFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPatchDlg, CDialog)
	//{{AFX_MSG_MAP(CPatchDlg)
	ON_BN_CLICKED(IDC_DIFF_BROWSE_FILE1, OnDiffBrowseFile1)
	ON_BN_CLICKED(IDC_DIFF_BROWSE_FILE2, OnDiffBrowseFile2)
	ON_BN_CLICKED(IDC_DIFF_BROWSE_RESULT, OnDiffBrowseResult)
	ON_CBN_SELCHANGE(IDC_DIFF_STYLE, OnSelchangeDiffStyle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPatchDlg message handlers

/** 
 * @brief OK button pressed: check options and filenames and close dialog
 */
void CPatchDlg::OnOK()
{
	int contextSel = 0;
	TCHAR contextText[50] = {0};
	BOOL file1Ok = TRUE;
	BOOL file2Ok = TRUE;
	BOOL fileResultOk = FALSE;
	int overWrite = 0;
	int selectCount = 0;
	struct _stat fileStat = {0};

	UpdateData(TRUE);
	selectCount = m_fileList.GetCount();

	// There are two different cases: single files or
	// multiple files.  Multiple files are selected from DirView.
	// Only if single files selected, filenames are checked here.
	// Filenames read from Dirview must be valid ones.
	if (selectCount == 1)
	{
		file1Ok = is_regfile2(m_file1);
		file2Ok = is_regfile2(m_file2);

		if (file1Ok == FALSE)
			AfxMessageBox(IDS_DIFF_ITEM1NOTFOUND, MB_ICONSTOP);

		if (file2Ok == FALSE)
			AfxMessageBox(IDS_DIFF_ITEM2NOTFOUND, MB_ICONSTOP);
	}

	fileResultOk = is_regfile2(m_fileResult);

	// Result file already exists and append not selected
	if (fileResultOk == TRUE && m_appendFile == FALSE)
	{
		overWrite = AfxMessageBox(IDS_DIFF_FILEOVERWRITE, MB_YESNO);
		if (overWrite == IDNO)
			fileResultOk = FALSE;
	}
	else	// It's ok to write new file
		fileResultOk = TRUE;

	if (file1Ok == TRUE && file2Ok == TRUE && fileResultOk == TRUE)
	{
		m_outputStyle = (enum output_style) m_comboStyle.GetCurSel();

		contextSel = m_comboContext.GetCurSel();
		if (contextSel > 0)
		{
			m_comboContext.GetLBText(contextSel, contextText);
			m_contextLines = (int) contextText[0] - '0';
		}
		else
			m_contextLines = 0;

		CDialog::OnOK();
	}
}

/** 
 * @brief Initialise dialog data
 */
BOOL CPatchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ignoreBlanks = TRUE;
	m_caseSensitive = TRUE;
	m_whitespaceCompare = 0;
	int count = m_fileList.GetCount();
	
	// If one file added, show filenames on dialog
	if (count == 1)
	{
		PATCHFILES files = m_fileList.GetHead();
		if (m_file1.IsEmpty())
			m_file1 = files.lfile;

		if (m_file2.IsEmpty())
			m_file2 = files.rfile;
	}
	else	// Multiple files added, show number of files
	{
		CString msg;
		CString num;
		num.Format(_T("%d"), count);
		AfxFormatString1(msg, IDS_DIFF_SELECTEDFILES, num);
		m_file1 = msg;
		m_file2 = msg;
	}
	
	UpdateData(FALSE);

	// Add patch styles to combobox
	CString str;
	VERIFY(str.LoadString(IDS_DIFF_NORMAL));
	m_comboStyle.AddString(str);
	VERIFY(str.LoadString(IDS_DIFF_CONTEXT));
	m_comboStyle.AddString(str);
	VERIFY(str.LoadString(IDS_DIFF_UNIFIED));
	m_comboStyle.AddString(str);

	m_outputStyle = OUTPUT_NORMAL;
	m_comboStyle.SetCurSel(0);

	// Add context line counts to combobox
	m_comboContext.AddString(_T("0"));
	m_comboContext.AddString(_T("1"));
	m_comboContext.AddString(_T("3"));
	m_comboContext.AddString(_T("5"));
	m_comboContext.AddString(_T("7"));
	m_comboContext.AddString(_T("11"));
	
	m_contextLines = 0;
	m_comboContext.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/** 
 * @brief Select left file
 */
void CPatchDlg::OnDiffBrowseFile1()
{
	CString s;
	CString folder;
	CString name;

	if (SelectFile(s, folder))
	{
		PATCHFILES pf;
		int count = GetItemCount();

		// If there are multiple files already, empty list
		if (count > 1)
		{
			ClearItems();
			pf.lfile = s;
			AddItem(pf);
			m_file1 = s;
			m_file2 = "";
		}
		else if (count == 0)
		{
			pf.lfile = s;
			AddItem(pf);
			m_file1 = s;
		}
		else
		{
			POSITION pos = GetFirstItem();
			POSITION current = pos;
			pf = GetNextItem(pos);
			//CString fileName = pf.lfile;
			pf.lfile = s;
			m_file1 = s;

			SetItemAt(current, pf);
		}
		UpdateData(FALSE);
	}
}

/** 
 * @brief Select right file
 */
void CPatchDlg::OnDiffBrowseFile2()
{
	CString s;
	CString folder;
	CString name;

	if (SelectFile(s, folder))
	{
		PATCHFILES pf;
		int count = GetItemCount();

		// If there are multiple files already, empty list
		if (count > 1)
		{
			ClearItems();
			pf.rfile = s;
			AddItem(pf);
			m_file1 = "";
			m_file2 = s;
		}
		else if (count == 0)
		{
			pf.rfile = s;
			AddItem(pf);
			m_file2 = s;
		}
		else
		{
			POSITION pos = GetFirstItem();
			POSITION current = pos;
			pf = GetNextItem(pos);
			//CString fileName = pf.rfile;
			pf.rfile = s;
			m_file2 = s;

			SetItemAt(current, pf);
		}
		UpdateData(FALSE);
	}
}

/** 
 * @brief Select patch file
 */
void CPatchDlg::OnDiffBrowseResult()
{
	CString s;
	CString folder;
	CString name;

	if (SelectFile(s, folder))
	{
		SplitFilename(s, &folder, &name, NULL);
		m_fileResult = s;
		UpdateData(FALSE);
	}
}

/** 
 * @brief Select file using Windows common dialog
 */
BOOL CPatchDlg::SelectFile(CString& path, LPCTSTR pszFolder)
{
	CString s;

	VERIFY(s.LoadString(IDS_ALLFILES));
	CFileDialog pdlg(TRUE, NULL, "", OFN_NOTESTFILECREATE | OFN_PATHMUSTEXIST, s);

	CString title;
	VERIFY(title.LoadString(IDS_OPEN_TITLE));
	pdlg.m_ofn.lpstrTitle = (LPCTSTR)title;
	pdlg.m_ofn.lpstrInitialDir = (LPSTR)pszFolder;

	if (pdlg.DoModal() == IDOK)
	{
	 	path = pdlg.GetPathName();
	 	return TRUE;
	}
	else
		return FALSE;
}

/** 
 * @brief Change diff style, enable/disable context selection
 */
void CPatchDlg::OnSelchangeDiffStyle()
{
	int selection = -1;

	selection = m_comboStyle.GetCurSel();

	// Only context and unified formats allow context lines
	if ((selection == OUTPUT_CONTEXT) ||
			(selection == OUTPUT_UNIFIED))
	{
		m_comboContext.EnableWindow(TRUE);
		// 3 lines is default context for Difftools too
		m_comboContext.SetCurSel(2);
	}
	else
	{
		m_contextLines = 0;
		m_comboContext.SetCurSel(0);
		m_comboContext.EnableWindow(FALSE);
	}
}

/** 
 * @brief Add file to internal list
 */
void CPatchDlg::AddItem(PATCHFILES pf)
{
	m_fileList.AddTail(pf);
}

/** 
 * @brief Returns amount of files in internal list
 */
int CPatchDlg::GetItemCount()
{
	return m_fileList.GetCount();
}

/** 
 * @brief Return ref to first files in internal list
 */
POSITION CPatchDlg::GetFirstItem()
{
	return m_fileList.GetHeadPosition();
}

/** 
 * @brief Return next files in internal list
 */
PATCHFILES CPatchDlg::GetNextItem(POSITION &pos)
{
	return m_fileList.GetNext(pos);
}

/** 
 * @brief Set files in given pos of internal list
 */
void CPatchDlg::SetItemAt(POSITION pos, PATCHFILES pf)
{
	m_fileList.SetAt(pos, pf);
}

/** 
 * @brief Empties internal file list
 */
void CPatchDlg::ClearItems()
{
	m_fileList.RemoveAll();
}

