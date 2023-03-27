#include "pch.h"
#include "Shell.h"
#include "UnicodeString.h"
#include "paths.h"
#include <Windows.h>

namespace shell
{

void Open(const tchar_t * szFile)
{
	ShellExecute(::GetDesktopWindow(), _T("open"), szFile, 0, 0, SW_SHOWNORMAL);
}

void Edit(const tchar_t * szFile)
{
	HINSTANCE rtn = ShellExecute(::GetDesktopWindow(), _T("edit"), szFile, 0, 0, SW_SHOWNORMAL);
	if (reinterpret_cast<uintptr_t>(rtn) == SE_ERR_NOASSOC)
		rtn = ShellExecute(::GetDesktopWindow(), _T("open"), szFile, 0, 0, SW_SHOWNORMAL);
	if (reinterpret_cast<uintptr_t>(rtn) == SE_ERR_NOASSOC)
		OpenWith(szFile);
}

void OpenWith(const tchar_t * szFile)
{
	tchar_t sysdir[MAX_PATH]{};
	if (!GetSystemDirectory(sysdir, MAX_PATH)) return;
	String arg = String(_T("shell32.dll,OpenAs_RunDLL ")) + szFile;
	ShellExecute(::GetDesktopWindow(), 0, _T("RUNDLL32.EXE"), arg.c_str(), sysdir, SW_SHOWNORMAL);
}

/**
 * @brief Open file, if it exists, else open url
 */
void OpenFileOrUrl(const tchar_t * szFile, const tchar_t * szUrl)
{
	if (paths::DoesPathExist(szFile) == paths::IS_EXISTING_FILE)
		ShellExecute(nullptr, _T("open"), _T("notepad.exe"), szFile, nullptr, SW_SHOWNORMAL);
	else
		Open(szUrl);
}

/**
 * @brief Open parent folder
 */
void OpenParentFolder(const tchar_t * szFile)
{
	String param = _T("/select,\"") + String(szFile) + _T("\"");
	ShellExecute(nullptr, _T("open"), _T("explorer.exe"), param.c_str(), nullptr, SW_SHOWNORMAL);
}

}