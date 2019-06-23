/** 
 * @file  PropShell.cpp
 *
 * @brief Implementation file for Shell Options dialog.
 *
 */

#include "stdafx.h"
#include "PropShell.h"
#include "RegKey.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"
#include "Constants.h"
#include "Environment.h"
#include "paths.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/// Flags for enabling and mode of extension
#define CONTEXT_F_ENABLED 0x01
#define CONTEXT_F_ADVANCED 0x02

// registry values
static LPCTSTR f_RegValueEnabled = _T("ContextMenuEnabled");
static LPCTSTR f_RegValuePath = _T("Executable");

static bool IsShellExtensionRegistered(bool peruser)
{
	HKEY hKey;
#ifdef _WIN64
	DWORD ulOptions = KEY_QUERY_VALUE;
#else
	auto Is64BitWindows = []() { BOOL f64 = FALSE; return IsWow64Process(GetCurrentProcess(), &f64) && f64; };
	DWORD ulOptions = KEY_QUERY_VALUE | (Is64BitWindows() ? KEY_WOW64_64KEY : 0);
#endif
	if (ERROR_SUCCESS == RegOpenKeyEx(peruser ? HKEY_CURRENT_USER : HKEY_LOCAL_MACHINE, _T("Software\\Classes\\CLSID\\{4E716236-AA30-4C65-B225-D68BBA81E9C2}"), 0, ulOptions, &hKey))
	{
		RegCloseKey(hKey);
		return true;
	}
	return false;
}

static bool RegisterShellExtension(bool unregister, bool peruser)
{
	TCHAR szSystem32[260] = { 0 };
	TCHAR szSysWow64[260] = { 0 };
	GetSystemDirectory(szSystem32, sizeof(szSystem32) / sizeof(szSystem32[0]));
	GetSystemWow64Directory(szSysWow64, sizeof(szSysWow64) / sizeof(szSysWow64[0]));

	String progpath = env::GetProgPath();
	String regsvr32 = paths::ConcatPath(szSystem32, _T("regsvr32.exe"));
	String args;
	String options = (unregister ? _T("/s /u") : _T("/s"));
	options += peruser ? _T(" /n /i:user") : _T("");
	SHELLEXECUTEINFO sei = { sizeof(sei) };
	if (!peruser)
		sei.lpVerb = _T("runas");
	if (szSysWow64[0])
	{
		args = options + _T(" \"") + paths::ConcatPath(progpath, _T("ShellExtensionX64.dll")) + _T("\"");

		sei.lpFile = regsvr32.c_str();
		sei.lpParameters = args.c_str();
		ShellExecuteEx(&sei);

		regsvr32 = paths::ConcatPath(szSysWow64, _T("regsvr32.exe"));
		args = options + _T("\"") + paths::ConcatPath(progpath, _T("ShellExtensionU.dll")) + _T("\"");
		sei.lpFile = regsvr32.c_str();
		sei.lpParameters = args.c_str();
		return !!ShellExecuteEx(&sei);
	}
	else
	{
		args = options + _T(" \"") + paths::ConcatPath(progpath, _T("ShellExtensionU.dll")) + _T("\"");
		sei.lpFile = regsvr32.c_str();
		sei.lpParameters = args.c_str();
		return !!ShellExecuteEx(&sei);
	}
}

PropShell::PropShell(COptionsMgr *optionsMgr) 
: OptionsPanel(optionsMgr, PropShell::IDD)
, m_bContextAdded(false)
, m_bContextAdvanced(false)
{
}

BOOL PropShell::OnInitDialog()
{
	OptionsPanel::OnInitDialog();

#ifndef BCM_SETSHIELD
#define BCM_SETSHIELD            (0x1600/*BCM_FIRST*/ + 0x000C)
#endif

	SendDlgItemMessage(IDC_REGISTER_SHELLEXTENSION, BCM_SETSHIELD, 0, TRUE);
	SendDlgItemMessage(IDC_UNREGISTER_SHELLEXTENSION, BCM_SETSHIELD, 0, TRUE);

	// Update shell extension checkboxes
	UpdateButtons();
	GetContextRegValues();
	AdvancedContextMenuCheck();
	UpdateData(FALSE);

	SetTimer(0, 1000, nullptr);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void PropShell::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropShell)
	DDX_Check(pDX, IDC_EXPLORER_CONTEXT, m_bContextAdded);
	DDX_Check(pDX, IDC_EXPLORER_ADVANCED, m_bContextAdvanced);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(PropShell, CPropertyPage)
	//{{AFX_MSG_MAP(PropShell)
	ON_BN_CLICKED(IDC_EXPLORER_CONTEXT, OnAddToExplorer)
	ON_BN_CLICKED(IDC_REGISTER_SHELLEXTENSION, OnRegisterShellExtension)
	ON_BN_CLICKED(IDC_UNREGISTER_SHELLEXTENSION, OnUnregisterShellExtension)
	ON_BN_CLICKED(IDC_REGISTER_SHELLEXTENSION_PERUSER, OnRegisterShellExtensionPerUser)
	ON_BN_CLICKED(IDC_UNREGISTER_SHELLEXTENSION_PERUSER, OnUnregisterShellExtensionPerUser)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 */
void PropShell::ReadOptions()
{
	GetContextRegValues();
}

/** 
 * @brief Writes options values from UI to storage.
 */
void PropShell::WriteOptions()
{
	SaveMergePath(); // saves context menu settings as well
}

/// Get registry values for ShellExtension
void PropShell::GetContextRegValues()
{
	CRegKeyEx reg;
	LONG retVal = 0;
	retVal = reg.Open(HKEY_CURRENT_USER, RegDir);
	if (retVal != ERROR_SUCCESS)
	{
		String msg = strutils::format(_T("Failed to open registry key HKCU/%s:\n\t%d : %s"),
			RegDir, retVal, GetSysError(retVal));
		LogErrorString(msg);
		return;
	}

	// Read bitmask for shell extension settings
	DWORD dwContextEnabled = reg.ReadDword(f_RegValueEnabled, 0);

	if (dwContextEnabled & CONTEXT_F_ENABLED)
		m_bContextAdded = true;

	if (dwContextEnabled & CONTEXT_F_ADVANCED)
		m_bContextAdvanced = true;
}

/// Set registry values for ShellExtension
void PropShell::OnAddToExplorer()
{
	AdvancedContextMenuCheck();
	UpdateButtons();
}

/// Saves given path to registry for ShellExtension, and Context Menu settings
void PropShell::SaveMergePath()
{
	TCHAR temp[MAX_PATH] = {0};
	LONG retVal = 0;
	GetModuleFileName(AfxGetInstanceHandle(), temp, MAX_PATH);

	CRegKeyEx reg;
	retVal = reg.Open(HKEY_CURRENT_USER, RegDir);
	if (retVal != ERROR_SUCCESS)
	{
		String msg = strutils::format(_T("Failed to open registry key HKCU/%s:\n\t%d : %s"),
			RegDir, retVal, GetSysError(retVal));
		LogErrorString(msg);
		return;
	}

	// Save path to WinMerge(U).exe
	retVal = reg.WriteString(f_RegValuePath, temp);
	if (retVal != ERROR_SUCCESS)
	{
		String msg = strutils::format(_T("Failed to set registry value %s:\n\t%d : %s"),
			f_RegValuePath, retVal, GetSysError(retVal));
		LogErrorString(msg);
	}

	// Determine bitmask for shell extension
	DWORD dwContextEnabled = reg.ReadDword(f_RegValueEnabled, 0);
	if (m_bContextAdded)
		dwContextEnabled |= CONTEXT_F_ENABLED;
	else
		dwContextEnabled &= ~CONTEXT_F_ENABLED;

	if (m_bContextAdvanced)
		dwContextEnabled |= CONTEXT_F_ADVANCED;
	else
		dwContextEnabled &= ~CONTEXT_F_ADVANCED;

	retVal = reg.WriteDword(f_RegValueEnabled, dwContextEnabled);
	if (retVal != ERROR_SUCCESS)
	{
		String msg = strutils::format(_T("Failed to set registry value %s to %d:\n\t%d : %s"),
			f_RegValueEnabled, dwContextEnabled, retVal, GetSysError(retVal));
		LogErrorString(msg);
	}
}

/// Enable/Disable "Advanced menu" checkbox.
void PropShell::AdvancedContextMenuCheck()
{
	if (!IsDlgButtonChecked(IDC_EXPLORER_CONTEXT))
	{
		CheckDlgButton(IDC_EXPLORER_ADVANCED, FALSE);
		m_bContextAdvanced = false;
	}
}

void PropShell::UpdateButtons()
{
	bool registered = IsShellExtensionRegistered(false);
	bool registeredPerUser = IsShellExtensionRegistered(true);
	EnableDlgItem(IDC_EXPLORER_CONTEXT, registered || registeredPerUser);
	EnableDlgItem(IDC_REGISTER_SHELLEXTENSION, !registered);
	EnableDlgItem(IDC_UNREGISTER_SHELLEXTENSION, registered);
	EnableDlgItem(IDC_REGISTER_SHELLEXTENSION_PERUSER, !registeredPerUser);
	EnableDlgItem(IDC_UNREGISTER_SHELLEXTENSION_PERUSER, registeredPerUser);
	EnableDlgItem(IDC_EXPLORER_ADVANCED, 
		(registered || registeredPerUser) && IsDlgButtonChecked(IDC_EXPLORER_CONTEXT));
}

void PropShell::OnRegisterShellExtension()
{
	RegisterShellExtension(false, false);
}

void PropShell::OnUnregisterShellExtension()
{
	RegisterShellExtension(true, false);
}

void PropShell::OnRegisterShellExtensionPerUser()
{
	RegisterShellExtension(false, true);
}

void PropShell::OnUnregisterShellExtensionPerUser()
{
	RegisterShellExtension(true, true);
}

void PropShell::OnTimer(UINT_PTR nIDEvent)
{
	UpdateButtons();
}
