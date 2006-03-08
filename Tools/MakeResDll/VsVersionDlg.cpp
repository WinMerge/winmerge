/**
 * @file  VsVersionDlg.cpp
 *
 * @brief Implementation of Visual Studio Version Dialog
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "resource.h"
#include "VsVersionDlg.h"
// Following files included from WinMerge/Src/Common
#include "RegKey.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVsVersionDlg dialog

static int mybuttons[] = 
{ // These must be laid out in the same order as VS5, VS6, VS2002, ...
	IDC_VS5_BTN,
	IDC_VS6_BTN,
	IDC_VS2002_BTN,
	IDC_VS2003_BTN,
	IDC_VS2005_BTN
};

static LPCTSTR myregvals[] =
{ // These must be laid out in the same order as VS5, VS6, VS2002, ...
	_T("5"),
	_T("6"),
	_T("Net"),
	_T("Net2003"),
	_T("Net2005")
};

static LPCTSTR mysettings = _T("Software\\Thingamahoochie\\MakeResDll\\Settings");

CVsVersionDlg::CVsVersionDlg(CWnd* pParent /*=NULL*/)
: CDialog(CVsVersionDlg::IDD, pParent)
, m_nVersion(VS_NONE)
, m_bestversion(VS_NONE)
{
	//{{AFX_DATA_INIT(CVsVersionDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CVsVersionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVsVersionDlg)
	DDX_Control(pDX, IDC_INSTALL_DIR_TEXT, m_txtInstallDir);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVsVersionDlg, CDialog)
	//{{AFX_MSG_MAP(CVsVersionDlg)
	ON_BN_CLICKED(IDC_VS2005_BTN, OnVs2005Btn)
	ON_BN_CLICKED(IDC_VS2003_BTN, OnVs2003Btn)
	ON_BN_CLICKED(IDC_VS2002_BTN, OnVs2002Btn)
	ON_BN_CLICKED(IDC_VS6_BTN, OnVs6Btn)
	ON_BN_CLICKED(IDC_VS5_BTN, OnVs5Btn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVsVersionDlg message handlers

BOOL CVsVersionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Note: LoadImage gets shared icon, its not needed to destroy
	HICON hMergeIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
			MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 16, 16,
			LR_DEFAULTSIZE | LR_SHARED);
	SetIcon(hMergeIcon, TRUE);

	// setup handler for resizing this dialog	
	m_constraint.InitializeCurrentSize(this);
	// configure how individual controls adjust when dialog resizes
	m_constraint.ConstrainItem(IDC_VSTUDIO_GROUP, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_VS2005_BTN, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_VS2003_BTN, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_VS2002_BTN, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_VS6_BTN, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_VS5_BTN, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_INSTALL_DIR_LABEL, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_INSTALL_DIR_TEXT, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDOK, 0.33, 0, 0, 0); // floats right
	m_constraint.ConstrainItem(IDCANCEL, 0.67, 0, 0, 0); // floats right
	m_constraint.DisallowHeightGrowth();
	m_constraint.SubclassWnd(); // install subclassing
	m_constraint.LoadPosition(_T("VsVersionDlg"), _T("OpenDlg"), false); // persist size via registry

	DisableUninstalledVersions();
	SelectInitialVersion();
	UpdateInstallDir();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief disable choices for any versions with no installation directories known
 */
void CVsVersionDlg::DisableUninstalledVersions()
{
	CheckVersion(VS2005);
	CheckVersion(VS2003);
	CheckVersion(VS2002);
	CheckVersion(VS6);
	CheckVersion(VS5);
}

/**
 * @brief Disable choice if not available (& keep track of best available)
 */
void CVsVersionDlg::CheckVersion(int vsnum)
{
	int vsbutton = mybuttons[vsnum];
	if (m_sBaseDir[vsnum].IsEmpty())
		GetDlgItem(vsbutton)->EnableWindow(FALSE);
	else if (vsnum > m_bestversion)
		m_bestversion = vsnum;
}

/**
 * @brief Select the user's choice, or first one available
 */
void CVsVersionDlg::SelectInitialVersion()
{
	m_nVersion = VS_NONE;
	// Check for user-configured overrides
	CRegKeyEx reg;
	if (RegOpenUser(reg, mysettings))
	{
		CString sVcVersion = reg.ReadString(_T("VcVersion"), _T(""));
		reg.Close();
		for (int i=0; i<VS_COUNT; ++i)
		{
			if (myregvals[i] == sVcVersion)
			{
				m_nVersion = i;
				break;
			}
		}
	}
	if (m_nVersion == VS_NONE)
	{
		m_nVersion = m_bestversion;
	}
	if (m_nVersion != VS_NONE)
	{
		CheckVersionButton(m_nVersion, true);
	}
}

/**
 * @brief check (or uncheck) specified version button (using VS5... enum)
 */
void CVsVersionDlg::CheckVersionButton(int nversion, bool checked)
{
	ASSERT(nversion>VS_NONE && nversion<VS_COUNT);
	int vsbutton = mybuttons[nversion];
	((CButton *)GetDlgItem(vsbutton))->SetCheck(checked ? 1 : 0);
}

/**
 * @brief Display installation dir for chosen version
 */
void CVsVersionDlg::UpdateInstallDir()
{
	// First uncheck all buttons (otherwise disabled ones don't get cleared)
	for (int i=0; i<VS_COUNT; ++i)
	{
		CheckVersionButton(i, false);
	}
	if (m_nVersion>=0 && m_nVersion<VS_COUNT)
	{
		CString txt = m_sBaseDir[m_nVersion];
		m_txtInstallDir.SetWindowText(txt);
		CheckVersionButton(m_nVersion, true);
	}
	else
	{
		m_txtInstallDir.SetWindowText(LoadResString(IDS_NO_VS_FOUND));
	}
}

void CVsVersionDlg::OnVs2005Btn() 
{
	m_nVersion = VS2005;
	UpdateInstallDir();
}

void CVsVersionDlg::OnVs2003Btn() 
{
	m_nVersion = VS2003;
	UpdateInstallDir();
}

void CVsVersionDlg::OnVs2002Btn() 
{
	m_nVersion = VS2002;
	UpdateInstallDir();
}

void CVsVersionDlg::OnVs6Btn() 
{
	m_nVersion = VS6;
	UpdateInstallDir();
}

void CVsVersionDlg::OnVs5Btn() 
{
	m_nVersion = VS5;
	UpdateInstallDir();
}


void CVsVersionDlg::OnOK() 
{
	if (m_nVersion != VS_NONE)
	{
		CString regval = myregvals[m_nVersion];
		CRegKeyEx reg;
		if (reg.OpenWithAccess(HKEY_CURRENT_USER, mysettings, KEY_WRITE) != ERROR_SUCCESS)
		{
			AfxMessageBox(IDS_ERROR_WRITING_PREF);
			return;
		}
		if (reg.WriteString(_T("VcVersion"), regval) != ERROR_SUCCESS)
		{
			AfxMessageBox(IDS_ERROR_WRITING_PREF);
			return;
		}
	}
	CDialog::OnOK();
}
