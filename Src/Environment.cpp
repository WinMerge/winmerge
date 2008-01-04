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

// Static string used by paths_GetTempPath() for storing temp path.
static String strTempPath;

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
		int cchTempPath = GetTempPath(0, 0);
		strTempPath.resize(cchTempPath - 1);
		if (!GetTempPath(cchTempPath, &*strTempPath.begin()))
		{
			int err = GetLastError();
			if (pnerr)
				*pnerr = err;
#ifdef _DEBUG
			CString sysErr = GetSysError(err); // for debugging
#endif
			return strTempPath.c_str(); // empty
		}
		strTempPath = paths_GetLongPath(strTempPath.c_str());
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
		CString sysErr = GetSysError(err); // for debugging
#endif
		return _T("");
	}
	return buffer;
}
