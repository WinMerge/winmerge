#include "pch.h"
#include "Shell.h"
#include "UnicodeString.h"
#include "paths.h"
#include <Windows.h>

namespace shell
{

void Open(const TCHAR * szFile)
{
	ShellExecute(::GetDesktopWindow(), _T("open"), szFile, 0, 0, SW_SHOWNORMAL);
}

void Edit(const TCHAR * szFile)
{
	HINSTANCE rtn = ShellExecute(::GetDesktopWindow(), _T("edit"), szFile, 0, 0, SW_SHOWNORMAL);
	if (reinterpret_cast<uintptr_t>(rtn) == SE_ERR_NOASSOC)
		rtn = ShellExecute(::GetDesktopWindow(), _T("open"), szFile, 0, 0, SW_SHOWNORMAL);
	if (reinterpret_cast<uintptr_t>(rtn) == SE_ERR_NOASSOC)
		OpenWith(szFile);
}

void OpenWith(const TCHAR * szFile)
{
	TCHAR sysdir[MAX_PATH]{};
	if (!GetSystemDirectory(sysdir, MAX_PATH)) return;
	String arg = String(_T("shell32.dll,OpenAs_RunDLL ")) + szFile;
	ShellExecute(::GetDesktopWindow(), 0, _T("RUNDLL32.EXE"), arg.c_str(), sysdir, SW_SHOWNORMAL);
}

/**
 * @brief Open file, if it exists, else open url
 */
void OpenFileOrUrl(const TCHAR * szFile, const TCHAR * szUrl)
{
	if (paths::DoesPathExist(szFile) == paths::IS_EXISTING_FILE)
		ShellExecute(nullptr, _T("open"), _T("notepad.exe"), szFile, nullptr, SW_SHOWNORMAL);
	else
		Open(szUrl);
}

/**
 * @brief Open parent folder
 */
void OpenParentFolder(const TCHAR * szFile)
{
	Open(paths::GetParentPath(szFile).c_str());
}

}