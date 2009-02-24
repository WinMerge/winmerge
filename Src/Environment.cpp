/** 
 * @file  Environment.cpp
 *
 * @brief Environment related routines.
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include "paths.h"
#include "Environment.h"
#include "OptionsDef.h"
#include "Merge.h"

/**
 * @brief Temp path.
 * Static string used by paths_GetTempPath() for storing temp path.
 */
static String strTempPath;

/** @brief Per-instance part of the temp path. */
static String strTempPathInstance;

/** 
 * @brief Get folder for temporary files.
 * This function returns system temp folder.
 * @param [out] pnerr Error code if erorr happened.
 * @return Temp path, or empty string if error happened.
 * @note Temp path is cached after first call.
 * @todo Should we return NULL for error case?
 */
LPCTSTR env_GetTempPath(int * pnerr)
{
	if (strTempPath.empty())
	{
		if (GetOptionsMgr()->GetBool(OPT_USE_SYSTEM_TEMP_PATH))
		{
			int cchTempPath = GetTempPath(0, 0);
			strTempPath.resize(cchTempPath - 1);
			if (!GetTempPath(cchTempPath, &*strTempPath.begin()))
			{
				int err = GetLastError();
				if (pnerr)
					*pnerr = err;
#ifdef _DEBUG
				String sysErr = GetSysError(err); // for debugging
#endif
				return strTempPath.c_str(); // empty
			}
		}
		else
		{
			strTempPath = GetOptionsMgr()->GetString(OPT_CUSTOM_TEMP_PATH);
			if (!paths_EndsWithSlash(strTempPath.c_str()))
				strTempPath += '\\';
		}

		if (!strTempPathInstance.empty())
			strTempPath = paths_ConcatPath(strTempPath, strTempPathInstance);
		strTempPath = paths_GetLongPath(strTempPath.c_str());
		paths_CreateIfNeeded(strTempPath.c_str());
	}
	return strTempPath.c_str();
}

/**
 * @brief Get filename for temporary file.
 * @param [in] lpPathName Temporary file folder.
 * @param [in] lpPrefixString Prefix to use for filename.
 * @param [out] pnerr Error code if error happened.
 * @return Full path for temporary file or empty string if error happened.
 */
String env_GetTempFileName(LPCTSTR lpPathName, LPCTSTR lpPrefixString, int * pnerr)
{
	TCHAR buffer[MAX_PATH] = {0};
	if (_tcslen(lpPathName) > MAX_PATH-14)
		return _T(""); // failure
	int rtn = GetTempFileName(lpPathName, lpPrefixString, 0, buffer);
	if (!rtn)
	{
		int err = GetLastError();
		if (pnerr)
			*pnerr = err;
#ifdef _DEBUG
		String sysErr = GetSysError(err); // for debugging
#endif
		return _T("");
	}
	return buffer;
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
 * @param [in] hWindow Parent window.
 * @return Full path to My Documents -folder.
 */
String env_GetMyDocuments(HWND hWindow)
{
	LPITEMIDLIST pidl;
	LPMALLOC pMalloc;
	String path;

	HRESULT rv = SHGetSpecialFolderLocation(hWindow, CSIDL_PERSONAL, &pidl);
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
String env_GetPerInstanceString(LPCTSTR name)
{
	// Get processId as string
	TCHAR buffer[65] = {0};
	_itot(GetCurrentProcessId(), buffer, 10);

	String cPId(buffer);
	String folder(name);
	folder += cPId;
	return folder;
}

/**
 * @brief Set per-instance part of the temp folder.
 * @param [in] lpPathname Per-instance part of the folder name.
 */
void env_SetInstanceFolder(LPCTSTR lpPathName)
{
	// Instance folder must not be changed once set.
	ASSERT(strTempPathInstance.empty());
	strTempPathInstance = lpPathName;
}
