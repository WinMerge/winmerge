/** 
 * @file  MakePatchDirsDlg.cpp
 *
 * @date  Created: 2003 (Perry)
 * @date  Edited:  2004-08-01 (Perry)
 * @brief Code for CMakePatchDirsDlg (main dialog) & CAboutDlg classes
 */
// RCS ID line follows -- this is updated by CVS
// $Id$


#include "stdafx.h"
#include "MakePatchDirsApp.h"
#include "MakePatchDirsDlg.h"
#include "MakeDirs.h"
#include "CDirDialog.h"
#include "AppVersion.h"
#include "Satellites.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString	m_sVersion;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_sVersion = _T("");
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_VERSION, m_sVersion);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMakePatchDirsDlg dialog

CMakePatchDirsDlg::CMakePatchDirsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMakePatchDirsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMakePatchDirsDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMakePatchDirsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMakePatchDirsDlg)
	DDX_Control(pDX, IDC_DIR, m_dir);
	DDX_Control(pDX, IDC_LANGS, m_langs);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMakePatchDirsDlg, CDialog)
	//{{AFX_MSG_MAP(CMakePatchDirsDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_DIR_BROWSE, OnDirBrowse)
	ON_CBN_SELCHANGE(IDC_LANGS, OnSelchangeLangs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMakePatchDirsDlg message handlers

BOOL CMakePatchDirsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_dir.AllowDirDrop();
	m_dir.AllowFileDrop(FALSE);
	m_constraint.InitializeCurrentSize(this);
	m_constraint.ConstrainItem(IDC_DIR_GROUP, 0, 1, 0, 1);
	m_constraint.ConstrainItem(IDC_DIR, 0, 1, 0, 1);
	m_constraint.ConstrainItem(IDC_DIR_BROWSE, 1, 0, 1, 0);
	m_constraint.ConstrainItem(IDOK, .5, 0, 1, 0);
	m_constraint.DisallowHeightGrowth();
	m_constraint.SubclassWnd();

	GetDlgItem(IDC_EXCLUDE_VC_STUFF)->SendMessage(BM_SETCHECK, BST_CHECKED, 0);

	if (!PopulateDllList((WORD)-1))
	{
		// language changed, need to reload
		EndDialog(IDOK);
		return TRUE;
	}
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMakePatchDirsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.m_sVersion = appv::GetVersionReport();
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMakePatchDirsDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMakePatchDirsDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMakePatchDirsDlg::OnDirBrowse() 
{
	UpdateData(TRUE);
	prdlg::CDirDialog dlg; 

	CString dir;
	m_dir.GetWindowText(dir);

	dlg.m_strTitle = LoadResString(IDS_CHOOSE_PATCH_PROMPT);
	dlg.m_strSelDir = dir;

	if (dlg.DoBrowse()) 
	{
		m_dir.SetWindowText(dlg.m_strPath);
	}
}


/** @brief file/directory filter to pass to MakeDirs module */
class PatchDlgFilter : public MakeDirFilter
{
public:
	PatchDlgFilter() : m_excludeVc6(false), m_excludeCvs(false) { }
	virtual bool handle(bool dir, const CString & subdir, const CString & filebase, const CString & ext);

public:
	bool m_excludeVc6;
	bool m_excludeCvs;
};

bool PatchDlgFilter::handle(bool dir, const CString & subdir, const CString & filebase, const CString & ext)
{
	if (m_excludeVc6)
	{
		if (ext.CompareNoCase(_T("ncb"))==0
			|| ext.CompareNoCase(_T("opt"))==0
			|| ext.CompareNoCase(_T("aps"))==0
			|| ext.CompareNoCase(_T("plg"))==0
			|| ext.CompareNoCase(_T("clw"))==0
			)
		{
			return false;
		}
		if (filebase.CompareNoCase(_T("Build")) == 0)
		{
			return false;
		}
		if (filebase.CompareNoCase(_T("BuildTmp")) == 0)
		{
			return false;
		}

	}
	if (m_excludeCvs)
	{
		if (dir && filebase.CompareNoCase(_T("CVS")) == 0 && ext.IsEmpty())
		{
			return false;
		}
	}
	return true;
}

static bool IsButtonChecked(CWnd * pwnd, int id)
{
	return (pwnd->SendDlgItemMessage(id, BM_GETCHECK, 0, 0) == BST_CHECKED);
}

/** @brief User clicked Go */
void CMakePatchDirsDlg::OnOK() 
{
	CString dir;
	m_dir.GetWindowText(dir);
	if (dir.IsEmpty()) return;


	try {
		CWaitCursor wait;

		PatchDlgFilter filter;
		filter.m_excludeVc6 = IsButtonChecked(this, IDC_EXCLUDE_VC_STUFF);;
		filter.m_excludeCvs = IsButtonChecked(this, IDC_EXCLUDE_RCS_STUFF);;
		CString summary = MakeDirs::DoIt(dir, &filter);
		MessageBox(summary);

	} catch(CException * pExc) {
		pExc->ReportError();
		pExc->Delete();
	}
}

static CString
GetModuleFilepath()
{
	TCHAR buff[MAX_PATH+4] = _T("");
	GetModuleFileName(AfxGetInstanceHandle(), buff, countof(buff));
	return buff;
}

/**
 * @brief Create descriptive string for language
 */
static CString
GetLangDesc(WORD lang)
{
	CString desc;

	LCID lcid = MAKELCID(MAKELANGID(lang, SUBLANG_DEFAULT), SORT_DEFAULT);
	TCHAR buff[512];
	if (!GetLocaleInfo(lcid, LOCALE_SENGLANGUAGE, buff, countof(buff)))
		_tcscpy(buff, _T("?"));
	desc += buff;
	desc += _T(": ");
	if (!GetLocaleInfo(lcid, LOCALE_SLANGUAGE , buff, countof(buff)))
		_tcscpy(buff, _T("?"));
	desc += buff;
	return desc;
}

/**
 * @brief Return langid for a given locale (or -1)
 */
static WORD
GetLangFromLocale(LCID lcid)
{
		TCHAR buff[8];
		if (GetLocaleInfo(lcid, LOCALE_IDEFAULTLANGUAGE, buff, countof(buff)))
		{
			LANGID langid = 0;
			if (1 == _stscanf(buff, _T("%x"), &langid) && langid)
				return langid;
		}
		return -1;
}

/**
 * @brief Look for language for user in array of available languages, fill in langdll
 */
static bool
GetDefaultLang(const LangDllArray &langdlls, LangDll & langdll)
{
	CString mydll;

	WORD Lang1 = GetLangFromLocale(GetThreadLocale());
	if (Lang1 != (WORD)-1)
	{
		for (int i=0; i<langdlls.GetSize(); ++i)
		{
			if (langdlls[i].m_lang == Lang1)
			{
				langdll = langdlls[i];
				return true;
			}
		}
	}
	WORD Lang2 = GetLangFromLocale(LOCALE_USER_DEFAULT);
	if (Lang2 != (WORD)-1 && Lang2 != Lang1)
	{
		for (int i=0; i<langdlls.GetSize(); ++i)
		{
			if (langdlls[i].m_lang == Lang2)
			{
				langdll = langdlls[i];
				return true;
			}
		}
	}
	WORD Lang3 = GetLangFromLocale(LOCALE_SYSTEM_DEFAULT);
	if (Lang3 != (WORD)-1 && Lang3 != Lang2 && Lang3 != Lang1)
	{
		for (int i=0; i<langdlls.GetSize(); ++i)
		{
			if (langdlls[i].m_lang == Lang3)
			{
				langdll = langdlls[i];
				return true;
			}
		}
	}
	return false;
}

static HINSTANCE f_inst = 0;
static WORD f_lang = MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT);
/**
 * @brief Actually change language, returning true if successful
 */
static bool
SetLang(const LangDllArray &langdlls, WORD langid)
{
	LangDll langdll;
	for (int i=0; i<langdlls.GetSize(); ++i)
	{
		if (langdlls[i].m_lang == langid)
		{
			langdll = langdlls[i];
			break;
		}
	}
	if (langdll.dllpath.IsEmpty())
		return false;
	// reset the resource handle to point to the current file
	AfxSetResourceHandle(AfxGetInstanceHandle( ));
	if (f_inst)
	{
		FreeLibrary(f_inst);
		f_inst = 0;
	}
	if (langdll.dllpath != _T("!"))
	{
		f_inst = LoadLibrary(langdll.dllpath);
		if (!f_inst)
			return false;
		AfxSetResourceHandle(f_inst);
	}
	SetThreadLocale(MAKELCID(langdll.m_lang, SORT_DEFAULT));
	// dig out info for _tsetlocale(LC_ALL, ...)
	f_lang = langid;
	return true;
}

/**
 * @brief Fill in array of available languages
 */
static void
LoadSatList(LangDllArray &langdlls)
{
	CString filepath = GetModuleFilepath();
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_PATH];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	_tsplitpath(filepath, drive, dir, fname, ext);
	CString spec = (CString)drive + dir + fname + _T("_*.dll");
	CString spath = (CString)drive + dir;

	Sats_LoadList(spath, spec, langdlls);
	langdlls.Add(LangDll(MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), _T("!")));
}

/**
 * @brief Populate the droplist of available languages (except, return false if we need to reload)
 */
bool // return false if language changed
CMakePatchDirsDlg::PopulateDllList(WORD curlang)
{
	int loadedlang = f_lang;

	LangDllArray langdlls;
	LoadSatList(langdlls);

	if (curlang != (WORD)-1)
	{
		if (!SetLang(langdlls, curlang))
			curlang = (WORD)-1;
	}
	if (curlang == (WORD)-1)
	{
		LangDll langdll;
		if (GetDefaultLang(langdlls, langdll))
		{
			if (SetLang(langdlls, langdll.m_lang))
				curlang = langdll.m_lang;
		}
	}

	if (loadedlang != f_lang)
	{
		// To implement new language, must close & reopen main dialog
		return false;
	}

	m_langs.ResetContent();
	for (int i=0; i<langdlls.GetSize(); ++i)
	{
		const LangDll & langdll = langdlls[i];
		CString langname = GetLangDesc(langdll.m_lang);
		int index = m_langs.AddString(langname);
		m_langs.SetItemData(index, langdll.m_lang);
	}
	int isel = -1;
	for (i=0; i<langdlls.GetSize(); ++i)
	{
		if (m_langs.GetItemData(i) == curlang)
			isel = i;
	}

	m_langs.SetCurSel(isel);
	return true;
}

void CMakePatchDirsDlg::OnSelchangeLangs() 
{
	int index = m_langs.GetCurSel();
	if (index == -1) return;
	WORD newlang = (WORD)m_langs.GetItemData(index);
	if (newlang == f_lang) return;

	// User has chosen a new language
	// Need dll array
	LangDllArray langdlls;
	LoadSatList(langdlls);

	if (SetLang(langdlls, newlang))
	{
		// To implement new language, must close & reopen main dialog
		EndDialog(IDOK);
	}
}
