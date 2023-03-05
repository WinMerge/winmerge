// SPDX-License-Identifier: GPL-2.0-or-later
/** 
 * @file  PatchDlg.cpp
 *
 * @brief Implementation of Patch creation dialog
 */

#include "StdAfx.h"
#include "PatchDlg.h"
#include "PatchTool.h"
#include "diff.h"
#include "paths.h"
#include "CompareOptions.h"
#include "FileOrFolderSelect.h"
#include "Environment.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using std::swap;

/////////////////////////////////////////////////////////////////////////////
// CPatchDlg dialog

/** 
 * @brief Constructor, initializes members.
 */
CPatchDlg::CPatchDlg(CWnd* pParent /*= nullptr*/)
	: CTrDialog(CPatchDlg::IDD, pParent)
	, m_copyToClipboard(false)
	, m_appendFile(false)
	, m_openToEditor(false)
	, m_includeCmdLine(false)
	, m_outputStyle(OUTPUT_NORMAL)
	, m_contextLines(0)
{
}

/**
 * @brief Map dialog controls and class member variables.
 */
void CPatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CTrDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPatchDlg)
	DDX_Control(pDX, IDC_DIFF_STYLE, m_comboStyle);
	DDX_Control(pDX, IDC_DIFF_CONTEXT, m_comboContext);
	DDX_Check(pDX, IDC_DIFF_COPYCLIPBOARD, m_copyToClipboard);
	DDX_Check(pDX, IDC_DIFF_APPENDFILE, m_appendFile);
	DDX_Control(pDX, IDC_DIFF_FILE1, m_ctlFile1);
	DDX_Control(pDX, IDC_DIFF_FILE2, m_ctlFile2);
	DDX_Control(pDX, IDC_DIFF_FILERESULT, m_ctlResult);
	DDX_CBStringExact(pDX, IDC_DIFF_FILE1, m_file1);
	DDX_CBStringExact(pDX, IDC_DIFF_FILE2, m_file2);
	DDX_CBStringExact(pDX, IDC_DIFF_FILERESULT, m_fileResult);
	DDX_Check(pDX, IDC_DIFF_OPENTOEDITOR, m_openToEditor);
	DDX_Check(pDX, IDC_DIFF_INCLCMDLINE, m_includeCmdLine);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPatchDlg, CTrDialog)
	//{{AFX_MSG_MAP(CPatchDlg)
	ON_BN_CLICKED(IDC_DIFF_BROWSE_FILE1, OnDiffBrowseFile1)
	ON_BN_CLICKED(IDC_DIFF_BROWSE_FILE2, OnDiffBrowseFile2)
	ON_BN_CLICKED(IDC_DIFF_BROWSE_RESULT, OnDiffBrowseResult)
	ON_BN_CLICKED(IDC_DIFF_DEFAULTS, OnDefaultSettings)
	ON_CBN_SELCHANGE(IDC_DIFF_STYLE, OnSelchangeDiffStyle)
	ON_BN_CLICKED(IDC_DIFF_SWAPFILES, OnDiffSwapFiles)
	ON_CBN_SELCHANGE(IDC_DIFF_FILE1, OnSelchangeFile1)
	ON_CBN_SELCHANGE(IDC_DIFF_FILE2, OnSelchangeFile2)
	ON_CBN_EDITCHANGE(IDC_DIFF_FILE1, OnEditchangeFile1)
	ON_CBN_EDITCHANGE(IDC_DIFF_FILE2, OnEditchangeFile2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPatchDlg message handlers

/** 
 * @brief Called when dialog is closed with OK.
 * Check options and filenames given and close the dialog.
 */
void CPatchDlg::OnOK()
{
	UpdateData(TRUE);

	// There are two different cases: single files or
	// multiple files.  Multiple files are selected from DirView.
	// Only if single files selected, filenames are checked here.
	// Filenames read from Dirview must be valid ones.
	size_t selectCount = m_fileList.size();
	if (selectCount == 0)
	{
		PATCHFILES tFiles;
		tFiles.lfile = m_file1;
		tFiles.rfile = m_file2;
		AddItem(tFiles);
		selectCount = 1;
	}
	if (selectCount == 1)
	{
		bool file1Ok = (paths::DoesPathExist(m_file1) != paths::DOES_NOT_EXIST);
		bool file2Ok = (paths::DoesPathExist(m_file2) != paths::DOES_NOT_EXIST);

		if (!file1Ok || !file2Ok)
		{
			if (!file1Ok)
				LangMessageBox(IDS_DIFF_ITEM1NOTFOUND, MB_ICONSTOP);

			if (!file2Ok)
				LangMessageBox(IDS_DIFF_ITEM2NOTFOUND, MB_ICONSTOP);
			return;
		}

		PATCHFILES tFiles = m_fileList[0];
		if (tFiles.lfile != m_file1 && !tFiles.pathLeft.empty())
			tFiles.pathLeft.clear();
		if (tFiles.rfile != m_file2 && !tFiles.pathRight.empty())
			tFiles.pathRight.clear();
		tFiles.lfile = m_file1;
		tFiles.rfile = m_file2;
		m_fileList[0] = tFiles;
	}

	// Check that result (patch) file is absolute path
	if (!paths::IsPathAbsolute(m_fileResult))
	{
		if (m_fileResult.length() == 0)
		{
			tchar_t szTempFile[MAX_PATH];
			::GetTempFileName(env::GetTemporaryPath().c_str(), _T("pat"), 0, szTempFile);
			m_fileResult = szTempFile;
			m_ctlResult.SetWindowText(m_fileResult.c_str());
			DeleteFile(m_fileResult.c_str());
		}
		if (!paths::IsPathAbsolute(m_fileResult))
		{
			String msg = strutils::format_string1(_("The specified output path is not an absolute path: %1"),
				m_fileResult);
			AfxMessageBox(msg.c_str(), MB_ICONSTOP);
			m_ctlResult.SetFocus();
			return;
		}
	}
	
	bool fileExists = (paths::DoesPathExist(m_fileResult) == paths::IS_EXISTING_FILE);

	// Result file already exists and append not selected
	if (fileExists && !m_appendFile)
	{
		if (LangMessageBox(IDS_DIFF_FILEOVERWRITE,
				MB_YESNO | MB_ICONWARNING | MB_DONT_ASK_AGAIN,
				IDS_DIFF_FILEOVERWRITE) != IDYES)
		{
			return;
		}
	}
	// else it's OK to write new file

	switch (m_comboStyle.GetCurSel())
	{
	case 1: m_outputStyle = (enum output_style)OUTPUT_CONTEXT; break;
	case 2: m_outputStyle = (enum output_style)OUTPUT_UNIFIED; break;
	case 3: m_outputStyle = (enum output_style)OUTPUT_HTML; break;
	default: m_outputStyle = (enum output_style)OUTPUT_NORMAL; break;
	}

	m_contextLines = GetDlgItemInt(IDC_DIFF_CONTEXT);

	SaveSettings();

	// Save combobox history
	m_ctlResult.SaveState(_T("Files\\DiffFileResult"));
	m_comboContext.SaveState(_T("PatchCreator\\DiffContext"));
	// Don't save filenames if multiple file selected (as editbox reads
	// [X files selected])
	if (selectCount <= 1)
	{
		m_ctlFile1.SaveState(_T("Files\\DiffFile1"));
		m_ctlFile2.SaveState(_T("Files\\DiffFile2"));
	}
	
	CTrDialog::OnOK();
}

/** 
 * @brief Initialise dialog data.
 *
 * There are two cases for filename editboxes:
 * - if one file was added to list then we show that filename
 * - if multiple files were added we show text [X files selected]
 */
BOOL CPatchDlg::OnInitDialog()
{
	CTrDialog::OnInitDialog();

	// Load combobox history
	m_ctlFile1.LoadState(_T("Files\\DiffFile1"));
	m_ctlFile2.LoadState(_T("Files\\DiffFile2"));
	m_comboContext.LoadState(_T("PatchCreator\\DiffContext"));
	m_ctlResult.LoadState(_T("Files\\DiffFileResult"));

	size_t count = m_fileList.size();

	// If one file added, show filenames on dialog
	if (count == 1)
	{
        const PATCHFILES& tFiles = m_fileList.front();
		m_file1 = tFiles.lfile;
		m_ctlFile1.SetWindowText(tFiles.lfile.c_str());
		m_file2 = tFiles.rfile;
		m_ctlFile2.SetWindowText(tFiles.rfile.c_str());
	}
	else if (count > 1)	// Multiple files added, show number of files
	{
		m_file1 = m_file2 = strutils::format_string1(_("[%1 files selected]"), strutils::to_str(count));
	}
	UpdateData(FALSE);

	// Add patch styles to combobox
	m_comboStyle.AddString(_("Normal").c_str());
	m_comboStyle.AddString(_("Context").c_str());
	m_comboStyle.AddString(_("Unified").c_str());
	m_comboStyle.AddString(_("HTML").c_str());

	m_outputStyle = OUTPUT_NORMAL;
	m_comboStyle.SetCurSel(0);

	// Add default context line counts to combobox if its empty
	if (m_comboContext.GetCount() == 0)
	{
		m_comboContext.AddString(_T("0"));
		m_comboContext.AddString(_T("1"));
		m_comboContext.AddString(_T("3"));
		m_comboContext.AddString(_T("5"));
		m_comboContext.AddString(_T("7"));
	}
	
	LoadSettings();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/** 
 * @brief Select the left file.
 */
void CPatchDlg::OnDiffBrowseFile1()
{
	String s;
	String folder;

	folder = m_file1;
	if (SelectFileOrFolder(GetSafeHwnd(), s, folder.c_str()))
	{
		m_ctlFile1.SetWindowText(s.c_str());
		if (m_fileList.size() > 1)
		{
			m_ctlFile2.SetWindowText(_T(""));
			ClearItems();
		}
	}
}

/** 
 * @brief Select the right file.
 */
void CPatchDlg::OnDiffBrowseFile2()
{
	String s;
	String folder;

	folder = m_file2;
	if (SelectFileOrFolder(GetSafeHwnd(), s, folder.c_str()))
	{
		m_ctlFile2.SetWindowText(s.c_str());
		if (m_fileList.size() > 1)
		{
			m_ctlFile1.SetWindowText(_T(""));
			ClearItems();
		}
	}
}

/** 
 * @brief Select the patch file.
 */
void CPatchDlg::OnDiffBrowseResult()
{
	String s;
	String folder;

	folder = m_fileResult;
	if (SelectFile(GetSafeHwnd(), s, false, folder.c_str()))
		m_ctlResult.SetWindowText(s.c_str());
}

/** 
 * @brief Called when diff style dropdown selection is changed.
 * Called when diff style dropdown selection is changed.
 * If the new selection is context patch or unified patch format then
 * enable context lines selection control. Otherwise context lines selection
 * is disabled.
 */
void CPatchDlg::OnSelchangeDiffStyle()
{
	int selection = m_comboStyle.GetCurSel();

	// Only context and unified formats allow context lines
	if (selection != OUTPUT_NORMAL)
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
 * @brief Swap filenames on file1 and file2.
 */
void CPatchDlg::OnDiffSwapFiles()
{
	CString cstrFile1 = m_file1.c_str();
	CString cstrFile2 = m_file2.c_str();
	m_ctlFile1.GetWindowText(cstrFile1);
	m_ctlFile2.GetWindowText(cstrFile2);

	m_ctlFile1.SetWindowText(cstrFile2);
	m_ctlFile2.SetWindowText(cstrFile1);

	//  swapped files
	Swap();
}

/** 
 * @brief Updates patch dialog settings from member variables.
 */
void CPatchDlg::UpdateSettings()
{
	UpdateData(FALSE);

	switch (m_outputStyle)
	{
	case DIFF_OUTPUT_NORMAL:
		m_comboStyle.SelectString(-1, _("Normal").c_str());
		break;
	case DIFF_OUTPUT_CONTEXT:
		m_comboStyle.SelectString(-1, _("Context").c_str());
		break;
	case DIFF_OUTPUT_UNIFIED:
		m_comboStyle.SelectString(-1, _("Unified").c_str());
		break;
	case DIFF_OUTPUT_HTML:
		m_comboStyle.SelectString(-1, _("HTML").c_str());
		break;
	}

	m_comboContext.SelectString(-1, strutils::to_str(m_contextLines).c_str());

	if (m_outputStyle == OUTPUT_CONTEXT || m_outputStyle == OUTPUT_UNIFIED || m_outputStyle == OUTPUT_HTML)
		m_comboContext.EnableWindow(TRUE);
	else
		m_comboContext.EnableWindow(FALSE);
}

/** 
 * @brief Loads patch dialog settings from registry.
 */
void CPatchDlg::LoadSettings()
{
	int patchStyle = GetOptionsMgr()->GetInt(OPT_PATCHCREATOR_PATCH_STYLE);
	if ((patchStyle < DIFF_OUTPUT_NORMAL || patchStyle > DIFF_OUTPUT_UNIFIED) &&  patchStyle != DIFF_OUTPUT_HTML)
		patchStyle = DIFF_OUTPUT_NORMAL;
	m_outputStyle = (enum output_style) patchStyle;
	
	m_contextLines = GetOptionsMgr()->GetInt(OPT_PATCHCREATOR_CONTEXT_LINES);
	if (m_contextLines < 0 || m_contextLines > 50)
		m_contextLines = 0;

	m_openToEditor = GetOptionsMgr()->GetBool(OPT_PATCHCREATOR_OPEN_TO_EDITOR);
	m_includeCmdLine = GetOptionsMgr()->GetBool(OPT_PATCHCREATOR_INCLUDE_CMD_LINE);
	m_copyToClipboard = GetOptionsMgr()->GetBool(OPT_PATCHCREATOR_COPY_TO_CLIPBOARD);

	UpdateSettings();
}

/** 
 * @brief Saves patch dialog settings to registry.
 */
void CPatchDlg::SaveSettings()
{
	COptionsMgr *pOptions = GetOptionsMgr();
	pOptions->SaveOption(OPT_PATCHCREATOR_PATCH_STYLE, m_outputStyle);
	pOptions->SaveOption(OPT_PATCHCREATOR_CONTEXT_LINES, m_contextLines);
	pOptions->SaveOption(OPT_PATCHCREATOR_OPEN_TO_EDITOR, m_openToEditor);
	pOptions->SaveOption(OPT_PATCHCREATOR_INCLUDE_CMD_LINE, m_includeCmdLine);
	pOptions->SaveOption(OPT_PATCHCREATOR_COPY_TO_CLIPBOARD, m_copyToClipboard);
}

/** 
 * @brief Resets patch dialog settings to defaults.
 */
void CPatchDlg::OnDefaultSettings()
{
	m_outputStyle = (enum output_style) DIFF_OUTPUT_NORMAL;
	m_contextLines = 0;
	m_openToEditor = false;
	m_includeCmdLine = false;
	m_copyToClipboard = false;

	UpdateSettings();
}

void CPatchDlg::OnSelchangeFile1()
{
	if (m_fileList.size() > 1)
	{
		m_ctlFile2.SetWindowText(_T(""));
		ClearItems();
	}
}

void CPatchDlg::OnSelchangeFile2()
{
	if (m_fileList.size() > 1)
	{
		m_ctlFile1.SetWindowText(_T(""));
		ClearItems();
	}
}

void CPatchDlg::OnEditchangeFile1()
{
	if (m_fileList.size() > 1)
	{
		m_ctlFile2.SetWindowText(_T(""));
		ClearItems();
	}
}

void CPatchDlg::OnEditchangeFile2()
{
	if (m_fileList.size() > 1)
	{
		m_ctlFile1.SetWindowText(_T(""));
		ClearItems();
	}
}


/**
 * @brief Swap sides.
 */
void CPatchDlg::Swap()
{
	std::vector<PATCHFILES>::iterator iter = m_fileList.begin();
	std::vector<PATCHFILES>::const_iterator iterEnd = m_fileList.end();
	while (iter != iterEnd)
	{
		(*iter).swap_sides();
		++iter;
	}
}
