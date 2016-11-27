/** 
 * @file  Environment.cpp
 *
 * @brief Environment related routines.
 */

#define POCO_NO_UNWINDOWS 1
#include "Environment.h"
#include <windows.h>
#include <shlobj.h>
#include <sstream>
#include <Poco/Path.h>
#include <Poco/Process.h>
#include "paths.h"
#include "unicoder.h"

using Poco::Path;
using Poco::Process;

/**
 * @brief Temp path.
 * Static string used by env_GetTempPath() for storing temp path.
 */
static String strTempPath;
static String strProgPath;

void env_SetTempPath(const String& path)
{
	strTempPath = paths_AddTrailingSlash(paths_GetLongPath(path));
	paths_CreateIfNeeded(strTempPath);
}

/** 
 * @brief Get folder for temporary files.
 * This function returns system temp folder.
 * @return Temp path, or empty string if error happened.
 * @note Temp path is cached after first call.
 * @todo Should we return NULL for error case?
 */
String env_GetTempPath()
{
	if (strTempPath.empty())
	{
		strTempPath = env_GetSystemTempPath();
		if (strTempPath.empty())
			return strTempPath;

		paths_CreateIfNeeded(strTempPath);
	}
	return strTempPath;
}

/**
 * @brief Get filename for temporary file.
 * @param [in] lpPathName Temporary file folder.
 * @param [in] lpPrefixString Prefix to use for filename.
 * @param [out] pnerr Error code if error happened.
 * @return Full path for temporary file or empty string if error happened.
 */
String env_GetTempFileName(const String& lpPathName, const String& lpPrefixString, int * pnerr)
{
	TCHAR buffer[MAX_PATH] = {0};
	if (lpPathName.length() > MAX_PATH-14)
		return _T(""); // failure
	int rtn = GetTempFileName(lpPathName.c_str(), lpPrefixString.c_str(), 0, buffer);
	if (!rtn)
	{
		int err = GetLastError();
		if (pnerr)
			*pnerr = err;
		return _T("");
	}
	return buffer;
}

String env_GetTempChildPath()
{
	String path;
	do
	{
		path = paths_ConcatPath(env_GetTempPath(), string_format(_T("%08x"), rand()));
	} while (paths_IsDirectory(path) || !paths_CreateIfNeeded(path));
	return path;
}

void env_SetProgPath(const String& path)
{
	strProgPath = paths_AddTrailingSlash(path);
}

String env_GetProgPath()
{
	if (strProgPath.empty())
	{
		TCHAR temp[MAX_PATH] = {0};
		GetModuleFileName(NULL, temp, MAX_PATH);
		strProgPath = paths_GetPathOnly(temp);
	}
	return strProgPath;
}

/**
 * @brief Get Windows directory.
 * @return Windows directory.
 */
String env_GetWindowsDirectory()
{
	TCHAR path[MAX_PATH];
	path[0] = _T('\0');
	GetWindowsDirectory(path, MAX_PATH);
	return path;
}

/**
 * @brief Return User's My Documents Folder.
 * This function returns full path to User's My Documents -folder.
 * @return Full path to My Documents -folder.
 */
String env_GetMyDocuments()
{
	TCHAR path[MAX_PATH];
	path[0] = _T('\0');
	SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, path);
	return path;
}

/**
 * @brief Return unique string for the instance.
 * This function formats an unique string for WinMerge instance. The string
 * is quaranteed to be unique for instance asking it.
 * @param [in] name Additional name used as part of the string.
 * @return Unique string for the instance.
 */
String env_GetPerInstanceString(const String& name)
{
	std::basic_stringstream<TCHAR> stream;
	stream << name << Process::id();
	return stream.str();
}

/**
 * @brief Get system temporary directory.
 * @return System temporary director.
 */
String env_GetSystemTempPath()
{
	try
	{
		return ucr::toTString(Path::temp());
	}
	catch (...)
	{
		return _T("");
	}
}

static bool launchProgram(const String& sCmd, WORD wShowWindow)
{
	STARTUPINFO stInfo = { sizeof(STARTUPINFO) };
	stInfo.dwFlags = STARTF_USESHOWWINDOW;
	stInfo.wShowWindow = wShowWindow;
	PROCESS_INFORMATION processInfo;
	BOOL retVal = CreateProcess(NULL, (LPTSTR)sCmd.c_str(),
		NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL,
		&stInfo, &processInfo);
	if (!retVal)
		return false;
	CloseHandle(processInfo.hThread);
	CloseHandle(processInfo.hProcess);
	return true;
}

/**
 * @brief Load registry keys from .reg file if existing .reg file
 */
bool env_LoadRegistryFromFile(const String& sRegFilePath)
{
	if (paths_DoesPathExist(sRegFilePath) != IS_EXISTING_FILE)
		return false;
	return launchProgram(_T("reg.exe import \"") + sRegFilePath + _T("\""), SW_HIDE);
}

/** 
 * @brief Save registry keys to .reg file if existing .reg file
 */
bool env_SaveRegistryToFile(const String& sRegFilePath, const String& sRegDir)
{
	if (paths_DoesPathExist(sRegFilePath) != IS_EXISTING_FILE)
		return false;
	DeleteFile(sRegFilePath.c_str());
	return launchProgram(_T("reg.exe export HKCU\\") + sRegDir + _T(" \"") + sRegFilePath + _T("\""), SW_HIDE);
}
