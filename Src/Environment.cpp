/** 
 * @file  Environment.cpp
 *
 * @brief Environment related routines.
 */
// ID line follows -- this is updated by SVN
// $Id$

#define POCO_NO_UNWINDOWS 1
#include "Environment.h"
#include <windows.h>
#include <shlobj.h>
#include <cassert>
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
	strTempPath = path;
	if (!paths_EndsWithSlash(strTempPath))
		strTempPath += '\\';
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

		strTempPath = paths_ConcatPath(strTempPath, env_GetPerInstanceString(_T("WM_")));

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

void env_SetProgPath(const String& path)
{
	strProgPath = path;
	if (!paths_EndsWithSlash(strProgPath))
		strProgPath += '\\';
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
	TCHAR buf[MAX_PATH] = {0};
	GetWindowsDirectory(buf, MAX_PATH);
	return buf;
}

/**
 * @brief Return User's My Documents Folder.
 * This function returns full path to User's My Documents -folder.
 * @return Full path to My Documents -folder.
 */
String env_GetMyDocuments()
{
	LPITEMIDLIST pidl;
	LPMALLOC pMalloc;
	String path;

	HRESULT rv = SHGetSpecialFolderLocation(NULL, CSIDL_PERSONAL, &pidl);
	if (rv == S_OK)
	{
		TCHAR szPath[MAX_PATH] = {0};
		if (SHGetPathFromIDList(pidl, szPath))
		{
			path = szPath;
		}

		SHGetMalloc(&pMalloc);
		pMalloc->Free(pidl);
		pMalloc->Release();
	}
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
