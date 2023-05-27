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
#include "Win_VersionHelper.h"
#include "MergeCmdLineInfo.h"
#include "JumpList.h"
#include "Merge.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/// Flags for enabling and mode of extension
enum
{
	CONTEXT_F_ENABLED = 0x01,
	CONTEXT_F_ADVANCED = 0x02,
	CONTEXT_F_COMPARE_AS = 0x04
};

// registry values
static const tchar_t* f_RegValueEnabled = _T("ContextMenuEnabled");
static const tchar_t* f_RegValuePath = _T("Executable");
static const tchar_t* f_RegValueUserTasksFlags = _T("UserTasksFlags");

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

static bool IsWinMergeContextMenuRegistered()
{
	HKEY hKey;
#ifdef _WIN64
	DWORD ulOptions = KEY_QUERY_VALUE;
#else
	auto Is64BitWindows = []() { BOOL f64 = FALSE; return IsWow64Process(GetCurrentProcess(), &f64) && f64; };
	DWORD ulOptions = KEY_QUERY_VALUE | (Is64BitWindows() ? KEY_WOW64_64KEY : 0);
#endif
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\AppHost\\IndexedDB\\WinMerge_83g614hpn1ttr"), 0, ulOptions, &hKey))
	{
		RegCloseKey(hKey);
		return true;
	}
	return false;
}

static bool RegisterShellExtension(bool unregister, bool peruser)
{
	tchar_t szSystem32[260] = { 0 };
	tchar_t szSysWow64[260] = { 0 };
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
#if defined _M_ARM64
		args = options + _T(" \"") + paths::ConcatPath(progpath, _T("ShellExtensionARM64.dll")) + _T("\"");

		sei.lpFile = regsvr32.c_str();
		sei.lpParameters = args.c_str();
		return !!ShellExecuteEx(&sei);
#else
		args = options + _T(" \"") + paths::ConcatPath(progpath, _T("ShellExtensionX64.dll")) + _T("\"");

		sei.lpFile = regsvr32.c_str();
		sei.lpParameters = args.c_str();
		ShellExecuteEx(&sei);

		regsvr32 = paths::ConcatPath(szSysWow64, _T("regsvr32.exe"));
		args = options + _T("\"") + paths::ConcatPath(progpath, _T("ShellExtensionU.dll")) + _T("\"");
		sei.lpFile = regsvr32.c_str();
		sei.lpParameters = args.c_str();
		return !!ShellExecuteEx(&sei);
#endif
	}
	else
	{
#if defined _M_ARM
		args = options + _T(" \"") + paths::ConcatPath(progpath, _T("ShellExtensionARM.dll")) + _T("\"");
#else
		args = options + _T(" \"") + paths::ConcatPath(progpath, _T("ShellExtensionU.dll")) + _T("\"");
#endif
		sei.lpFile = regsvr32.c_str();
		sei.lpParameters = args.c_str();
		return !!ShellExecuteEx(&sei);
	}
}

static bool RegisterWinMergeContextMenu(bool unregister)
{
	String cmd;
	String progpath = env::GetProgPath();
	String packagepath = paths::ConcatPath(progpath, _T("WinMergeContextMenuPackage.msix"));
	if (!unregister)
		cmd = strutils::format(_T("powershell -c \"Add-AppxPackage '%s' -ExternalLocation '%s'\""), packagepath, progpath);
	else
		cmd = _T("powershell -c \"Get-AppxPackage -name WinMerge | Remove-AppxPackage\"");

	STARTUPINFO stInfo = { sizeof(STARTUPINFO) };
	stInfo.dwFlags = STARTF_USESHOWWINDOW;
	stInfo.wShowWindow = SW_SHOW;
	PROCESS_INFORMATION processInfo;
	bool retVal = !!CreateProcess(nullptr, (tchar_t*)cmd.c_str(),
		nullptr, nullptr, FALSE, CREATE_DEFAULT_ERROR_MODE, nullptr, nullptr,
		&stInfo, &processInfo);
	if (!retVal)
		return false;
	CloseHandle(processInfo.hThread);
	CloseHandle(processInfo.hProcess);
	return true;
}

static void LoadListView(CListCtrl& list)
{
	CRect rc;
	list.DeleteAllItems();
	list.SetExtendedStyle(LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);
	list.GetClientRect(&rc);
	list.InsertColumn(0, _T(""), LVCFMT_LEFT, rc.Width() - GetSystemMetrics(SM_CXVSCROLL));
	CRegKeyEx reg;
	reg.Open(HKEY_CURRENT_USER, RegDir);
	DWORD dwFlags = reg.ReadDword(f_RegValueUserTasksFlags, 0);
	int i = 0;
	for (auto [mask, text] : std::vector<std::pair<MergeCmdLineInfo::usertasksflags_t, String>>(
		{
			{ MergeCmdLineInfo::NEW_TEXT_COMPARE,   _("New Text Compare") },
			{ MergeCmdLineInfo::NEW_TABLE_COMPARE,  _("New Table Compare") },
			{ MergeCmdLineInfo::NEW_BINARY_COMPARE, _("New Binary Compare") },
			{ MergeCmdLineInfo::NEW_IMAGE_COMPARE,  _("New Image Compare") },
			{ MergeCmdLineInfo::NEW_WEBPAGE_COMPARE,  _("New Webpage Compare") },
			{ MergeCmdLineInfo::CLIPBOARD_COMPARE,  _("Clipboard Compare") },
			{ MergeCmdLineInfo::SHOW_OPTIONS_DIALOG,  _("Options") },
		}))
	{
		list.InsertItem(LVIF_TEXT | LVIF_PARAM, i, text.c_str(), 0, 0, 0, mask);
		list.SetCheck(i, dwFlags & mask);
		++i;
	}
}

static void SaveListView(CListCtrl& list)
{
	CRegKeyEx reg;
	MergeCmdLineInfo::usertasksflags_t dwFlags = 0;
	reg.Open(HKEY_CURRENT_USER, RegDir);
	int itemCount = list.GetItemCount();
	for (int i = 0; i < itemCount; ++i)
		dwFlags |= list.GetCheck(i) ? list.GetItemData(i) : 0;
	if (dwFlags != reg.ReadDword(f_RegValueUserTasksFlags, 0))
	{
		reg.WriteDword(f_RegValueUserTasksFlags, dwFlags);
		JumpList::AddUserTasks(CMergeApp::CreateUserTasks(dwFlags));
	}
}

PropShell::PropShell(COptionsMgr *optionsMgr) 
: OptionsPanel(optionsMgr, PropShell::IDD)
, m_bContextAdded(false)
, m_bContextAdvanced(false)
, m_bContextCompareAs(false)
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

	LoadListView(m_list);

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
	DDX_Check(pDX, IDC_EXPLORER_COMPARE_AS, m_bContextCompareAs);
	DDX_Control(pDX, IDC_JUMP_LIST, m_list);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(PropShell, OptionsPanel)
	//{{AFX_MSG_MAP(PropShell)
	ON_BN_CLICKED(IDC_EXPLORER_CONTEXT, OnAddToExplorer)
	ON_BN_CLICKED(IDC_EXPLORER_ADVANCED, OnAddToExplorerAdvanced)
	ON_BN_CLICKED(IDC_REGISTER_SHELLEXTENSION, OnRegisterShellExtension)
	ON_BN_CLICKED(IDC_UNREGISTER_SHELLEXTENSION, OnUnregisterShellExtension)
	ON_BN_CLICKED(IDC_REGISTER_SHELLEXTENSION_PERUSER, OnRegisterShellExtensionPerUser)
	ON_BN_CLICKED(IDC_UNREGISTER_SHELLEXTENSION_PERUSER, OnUnregisterShellExtensionPerUser)
	ON_BN_CLICKED(IDC_REGISTER_WINMERGECONTEXTMENU, OnRegisterWinMergeContextMenu)
	ON_BN_CLICKED(IDC_UNREGISTER_WINMERGECONTEXTMENU, OnUnregisterWinMergeContextMenu)
	ON_BN_CLICKED(IDC_CLEAR_ALL_RECENT_ITEMS, OnClearAllRecentItems)
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
	bool registered = IsShellExtensionRegistered(false);
	bool registeredPerUser = IsShellExtensionRegistered(true);
	bool registeredWinMergeContextMenu = IsWinMergeContextMenuRegistered();
	if (registered || registeredPerUser || registeredWinMergeContextMenu)
		SaveMergePath(); // saves context menu settings as well
	if (m_list.m_hWnd)
		SaveListView(m_list);
}

/// Get registry values for ShellExtension
void PropShell::GetContextRegValues()
{
	CRegKeyEx reg;
	LONG retVal = 0;
	retVal = reg.OpenNoCreateWithAccess(HKEY_CURRENT_USER, RegDir, KEY_READ);
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

	if (dwContextEnabled & CONTEXT_F_COMPARE_AS)
		m_bContextCompareAs = true;
}

/// Set registry values for ShellExtension
void PropShell::OnAddToExplorer()
{
	AdvancedContextMenuCheck();
	UpdateButtons();
}

void PropShell::OnAddToExplorerAdvanced()
{
	CompareAsContextMenuCheck();
	UpdateButtons();
}

/// Saves given path to registry for ShellExtension, and Context Menu settings
void PropShell::SaveMergePath()
{
	tchar_t temp[MAX_PATH] = {0};
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

	if (m_bContextCompareAs)
		dwContextEnabled |= CONTEXT_F_COMPARE_AS;
	else
		dwContextEnabled &= ~CONTEXT_F_COMPARE_AS;

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
		CompareAsContextMenuCheck();
	}
}

void PropShell::CompareAsContextMenuCheck()
{
	if (!IsDlgButtonChecked(IDC_EXPLORER_ADVANCED))
	{
		CheckDlgButton(IDC_EXPLORER_COMPARE_AS, FALSE);
		m_bContextCompareAs = false;
	}
}

void PropShell::UpdateButtons()
{
	bool registered = IsShellExtensionRegistered(false);
	bool registeredPerUser = IsShellExtensionRegistered(true);
	bool registerdWinMergeContextMenu = IsWinMergeContextMenuRegistered();
	bool win11 = IsWin11_OrGreater();
	bool win7 = IsWin7_OrGreater();
	EnableDlgItem(IDC_EXPLORER_CONTEXT, registered || registeredPerUser || registerdWinMergeContextMenu);
	EnableDlgItem(IDC_REGISTER_SHELLEXTENSION, !registered);
	EnableDlgItem(IDC_UNREGISTER_SHELLEXTENSION, registered);
	EnableDlgItem(IDC_REGISTER_SHELLEXTENSION_PERUSER, !registeredPerUser);
	EnableDlgItem(IDC_UNREGISTER_SHELLEXTENSION_PERUSER, registeredPerUser);
	EnableDlgItem(IDC_REGISTER_WINMERGECONTEXTMENU, !registerdWinMergeContextMenu && win11);
	EnableDlgItem(IDC_UNREGISTER_WINMERGECONTEXTMENU, registerdWinMergeContextMenu && win11);
	EnableDlgItem(IDC_EXPLORER_ADVANCED, 
		(registered || registeredPerUser || registerdWinMergeContextMenu) && IsDlgButtonChecked(IDC_EXPLORER_CONTEXT));
	EnableDlgItem(IDC_EXPLORER_COMPARE_AS,
		(registered || registeredPerUser || registerdWinMergeContextMenu) && IsDlgButtonChecked(IDC_EXPLORER_ADVANCED));
	EnableDlgItem(IDC_JUMP_LIST, win7);
	EnableDlgItem(IDC_CLEAR_ALL_RECENT_ITEMS, win7);
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

void PropShell::OnRegisterWinMergeContextMenu()
{
	RegisterWinMergeContextMenu(false);
}

void PropShell::OnUnregisterWinMergeContextMenu()
{
	RegisterWinMergeContextMenu(true);
}

void PropShell::OnClearAllRecentItems()
{
	JumpList::RemoveRecentDocs();
}

void PropShell::OnTimer(UINT_PTR nIDEvent)
{
	UpdateButtons();
}
