/** 
 * @file  Environment.cpp
 *
 * @brief Environment related routines.
 */

#include "pch.h"
#define POCO_NO_UNWINDOWS 1
#include "Environment.h"
#include <windows.h>
#pragma warning (push)			// prevent "warning C4091: 'typedef ': ignored on left of 'tagGPFIDL_FLAGS' when no variable is declared"
#pragma warning (disable:4091)	// VC bug when using XP enabled toolsets.
#include <shlobj.h>
#pragma warning (pop)
#include <sstream>
#include <Poco/Path.h>
#include <Poco/Process.h>
#include "paths.h"
#include "unicoder.h"

using Poco::Path;
using Poco::Process;

namespace env
{

/**
 * @brief Temp path.
 * Static string used by GetTemporaryPath() for storing temp path.
 */
static String strTempPath;
static String strProgPath;

void SetTemporaryPath(const String& path)
{
	strTempPath = paths::AddTrailingSlash(paths::GetLongPath(path));
	paths::CreateIfNeeded(strTempPath);
}

/** 
 * @brief Get folder for temporary files.
 * This function returns system temp folder.
 * @return Temp path, or empty string if error happened.
 * @note Temp path is cached after first call.
 * @todo Should we return `nullptr` for error case?
 */
String GetTemporaryPath()
{
	if (strTempPath.empty())
	{
		strTempPath = GetSystemTempPath();
		if (strTempPath.empty())
			return strTempPath;

		paths::CreateIfNeeded(strTempPath);
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
String GetTemporaryFileName(const String& lpPathName, const String& lpPrefixString, int * pnerr /*= nullptr*/)
{
	tchar_t buffer[MAX_PATH] = {0};
	if (lpPathName.length() > MAX_PATH-14)
		return _T(""); // failure
	int rtn = ::GetTempFileName(lpPathName.c_str(), lpPrefixString.c_str(), 0, buffer);
	if (rtn == 0)
	{
		paths::CreateIfNeeded(lpPathName);
		rtn = ::GetTempFileName(lpPathName.c_str(), lpPrefixString.c_str(), 0, buffer);
		if (rtn == 0)
		{
			int err = GetLastError();
			if (pnerr != nullptr)
				*pnerr = err;
			return _T("");
		}
	}
	return buffer;
}

String GetTempChildPath()
{
	String path;
	do
	{
		path = paths::ConcatPath(GetTemporaryPath(), strutils::format(_T("%08x"), rand()));
	} while (paths::IsDirectory(path) || !paths::CreateIfNeeded(path));
	return path;
}

void SetProgPath(const String& path)
{
	strProgPath = paths::AddTrailingSlash(path);
}

String GetProgPath()
{
	if (strProgPath.empty())
	{
		tchar_t temp[MAX_PATH] = {0};
		GetModuleFileName(nullptr, temp, MAX_PATH);
		strProgPath = paths::GetPathOnly(temp);
	}
	return strProgPath;
}

/**
 * @brief Get Windows directory.
 * @return Windows directory.
 */
String GetWindowsDirectory()
{
	tchar_t path[MAX_PATH];
	path[0] = _T('\0');
	::GetWindowsDirectory(path, MAX_PATH);
	return path;
}

/**
 * @brief Return User's My Documents Folder.
 * This function returns full path to User's My Documents -folder.
 * @return Full path to My Documents -folder.
 */
String GetMyDocuments()
{
	tchar_t path[MAX_PATH];
	path[0] = _T('\0');
	SHGetFolderPath(nullptr, CSIDL_PERSONAL, nullptr, 0, path);
	return path;
}

/**
 * @brief Return unique string for the instance.
 * This function formats an unique string for WinMerge instance. The string
 * is quaranteed to be unique for instance asking it.
 * @param [in] name Additional name used as part of the string.
 * @return Unique string for the instance.
 */
String GetPerInstanceString(const String& name)
{
	std::basic_stringstream<tchar_t> stream;
	stream << name << Process::id();
	return stream.str();
}

/**
 * @brief Get system temporary directory.
 * @return System temporary director.
 */
String GetSystemTempPath()
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
	bool retVal = !!CreateProcess(nullptr, (tchar_t*)sCmd.c_str(),
		nullptr, nullptr, FALSE, CREATE_DEFAULT_ERROR_MODE, nullptr, nullptr,
		&stInfo, &processInfo);
	if (!retVal)
		return false;
	CloseHandle(processInfo.hThread);
	CloseHandle(processInfo.hProcess);
	return true;
}

String ExpandEnvironmentVariables(const String& text)
{
	tchar_t buf[512];
	buf[0] = 0;
	const unsigned size = sizeof(buf) / sizeof(buf[0]);
	const unsigned expandedSize = ::ExpandEnvironmentStrings(text.c_str(), buf, size);
	if (expandedSize <= size)
		return buf;
	std::vector<tchar_t> newbuf(expandedSize);
	::ExpandEnvironmentStrings(text.c_str(), newbuf.data(), expandedSize);
	return newbuf.data();
}

/**
 * @brief Load registry keys from .reg file if existing .reg file
 */
bool LoadRegistryFromFile(const String& sRegFilePath)
{
	if (paths::DoesPathExist(sRegFilePath) != paths::IS_EXISTING_FILE)
		return false;
	return launchProgram(_T("reg.exe import \"") + sRegFilePath + _T("\""), SW_HIDE);
}

/** 
 * @brief Save registry keys to .reg file if existing .reg file
 */
bool SaveRegistryToFile(const String& sRegFilePath, const String& sRegDir)
{
	if (paths::DoesPathExist(sRegFilePath) != paths::IS_EXISTING_FILE)
		return false;
	DeleteFile(sRegFilePath.c_str());
	return launchProgram(_T("reg.exe export HKCU\\") + sRegDir + _T(" \"") + sRegFilePath + _T("\""), SW_HIDE);
}

}
