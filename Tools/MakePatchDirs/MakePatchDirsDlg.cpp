/** 
 * @file  MakePatchDirsDlg.cpp
 *
 * @date  Created: 2003 (Perry)
 * @date  Edited:  2004-06-08 (Perry)
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
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMakePatchDirsDlg, CDialog)
	//{{AFX_MSG_MAP(CMakePatchDirsDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_DIR_BROWSE, OnDirBrowse)
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
