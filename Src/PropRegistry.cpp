// PropRegistry.cpp : implementation file
//

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

// registry keys we use to be on Directory context menu
static LPCTSTR f_context_key = _T("Directory\\shell\\WinMerge");
static LPCTSTR f_context_key_cmd = _T("Directory\\shell\\WinMerge\\command");

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
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CPropRegistry::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropRegistry)
	DDX_Check(pDX, IDC_EXPLORER_CONTEXT, m_bContextAdded);
	DDX_Text(pDX, IDC_WINMERGE_PATH, m_strPath);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPropRegistry, CDialog)
	//{{AFX_MSG_MAP(CPropRegistry)
	ON_BN_CLICKED(IDC_ASSOC_DIRECTORY, OnAssocDirectory)
	ON_BN_CLICKED(IDC_EXPLORER_CONTEXT, OnAddToExplorer)
	ON_BN_CLICKED(IDC_WINMERGE_PATH_SAVE, OnSavePath)
	ON_BN_CLICKED(IDC_WINMERGE_PATH_BROWSE, OnBrowsePath)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropRegistry message handlers

BOOL CPropRegistry::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	UpdateButton();
	GetContextRegValues();
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// Are we registered as a Directory context handler ?
bool CPropRegistry::IsRegisteredForDirectory() const
{
	CRegKeyEx reg;
	REGSAM regsam = KEY_READ;
	if (reg.OpenNoCreateWithAccess(HKEY_CLASSES_ROOT, f_context_key, regsam) != ERROR_SUCCESS)
		return false;
	reg.Close();
	if (reg.OpenNoCreateWithAccess(HKEY_CLASSES_ROOT, f_context_key_cmd, regsam) != ERROR_SUCCESS)
		return false;
	return true;
}

// check registry and set button & member accordingly
void CPropRegistry::UpdateButton()
{
	m_enabled = IsRegisteredForDirectory();
	WPARAM wcheck = m_enabled ? BST_CHECKED : BST_UNCHECKED;
	SendDlgItemMessage(IDC_ASSOC_DIRECTORY, BM_SETCHECK, wcheck, 0);
}

// Set or clear our entries in registry for Directory context
void CPropRegistry::EnableContextHandler(bool enabling)
{
	if (enabling)
	{
		CRegKeyEx reg;
		if (reg.Open(HKEY_CLASSES_ROOT, f_context_key) != ERROR_SUCCESS)
		{
			return;
		}
		CString lbl;
		VERIFY(lbl.LoadString(IDS_WINMERGE_THIS_DIRECTORY));
		reg.WriteString(_T(""), lbl);
		reg.Close();
		if (reg.Open(HKEY_CLASSES_ROOT, f_context_key_cmd) != ERROR_SUCCESS)
			return;
		CString exe = GetModulePath() + '\\' + AfxGetApp()->m_pszExeName;
		CString cmd = exe + _T(" \"%1\"");
		reg.WriteString(_T(""), cmd);
	}
	else
	{
		::RegDeleteKey(HKEY_CLASSES_ROOT, f_context_key_cmd);
		::RegDeleteKey(HKEY_CLASSES_ROOT, f_context_key);
	}
}

void CPropRegistry::OnAssocDirectory()
{
	bool enabling = (SendDlgItemMessage(IDC_ASSOC_DIRECTORY, BM_GETCHECK, 0, 0) == BST_CHECKED);
	EnableContextHandler(enabling);
	UpdateButton();
}

void CPropRegistry::GetContextRegValues()
{
	CRegKeyEx reg;
	if (reg.Open(HKEY_CURRENT_USER, f_RegDir) != ERROR_SUCCESS)
		return;

	// This will be bit mask, although now there is only one bit defined
	DWORD dwContextEnabled = reg.ReadDword(f_RegValueEnabled, 0);
	
	if (dwContextEnabled & 0x1)
		m_bContextAdded = TRUE;

	m_strPath = reg.ReadString(f_RegValuePath, _T(""));
}

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

void CPropRegistry::OnSavePath()
{
	UpdateData();

	CRegKeyEx reg;
	if (reg.Open(HKEY_CURRENT_USER, f_RegDir) != ERROR_SUCCESS)
		return;

	reg.WriteString(f_RegValuePath, m_strPath);
}

void CPropRegistry::OnBrowsePath()
{
	CString s;           
                   
	VERIFY(s.LoadString(IDS_ALLFILES) );
	DWORD flags = OFN_NOTESTFILECREATE | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
	CFileDialog pdlg(TRUE, NULL, _T("WinMerge.exe"), flags, s);
	CString title;
	VERIFY(title.LoadString(IDS_OPEN_TITLE));
	pdlg.m_ofn.lpstrTitle = (LPCTSTR)title;
//	pdlg.m_ofn.lpstrInitialDir = (LPSTR)pszFolder;

	if (pdlg.DoModal()==IDOK)
	{
	 	m_strPath = pdlg.GetPathName(); 
	}

	UpdateData(FALSE);
}