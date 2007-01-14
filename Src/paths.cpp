/** 
 * @file  paths.cpp
 *
 * @brief Path handling routines
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "paths.h"
#include <direct.h>
#include <mbctype.h> // MBCS (multibyte codepage stuff)
#include <shlobj.h>

static bool IsSlash(LPCTSTR pszStart, int nPos);
static bool GetDirName(const CString & sDir, CString& sName);

/** 
 * @brief Checks if char in string is slash.
 * @param [in] pszStart String to check.
 * @param [in] nPos of char in string to check (0-based index).
 * @return true if char is slash.
 */
static bool IsSlash(LPCTSTR pszStart, int nPos)
{
	return pszStart[nPos]=='/' || 
#ifdef _UNICODE
	       pszStart[nPos]=='\\';
#else
		// Avoid 0x5C (ASCII backslash) byte occurring as trail byte in MBCS
	       (pszStart[nPos]=='\\' && !_ismbstrail((unsigned char *)pszStart, (unsigned char *)pszStart + nPos));
#endif
}

/** 
 * @brief Checks if string ends with slash.
 * This function checks if given string ends with slash. In many places,
 * especially in GUI, we assume folder paths end with slash.
 * @param [in] s String to check.
 * @return true if last char in string is slash.
 */
bool paths_EndsWithSlash(const CString & s)
{
	return !s.IsEmpty() && IsSlash(s, s.GetLength()-1);
}

/** 
 * @brief Checks if path exists and if it points to folder or file.
 * This function first checks if path exists. If path exists
 * then function checks if path points to folder or file.
 * @param [in] szPath Path to check.
 * @return One of:
 * - DOES_NOT_EXIST : path does not exists
 * - IS_EXISTING_DIR : path points to existing folder
 * - IS_EXISTING_FILE : path points to existing file
 */
PATH_EXISTENCE paths_DoesPathExist(LPCTSTR szPath)
{
	if (!szPath || !szPath[0])
		return DOES_NOT_EXIST;

	// Expand environment variables:
	// Convert "%userprofile%\My Documents" to "C:\Documents and Settings\username\My Documents"
	LPCTSTR lpcszPath;
	TCHAR expandedPath[_MAX_PATH] = {0};

	if (_tcschr(szPath, '%') &&
		ExpandEnvironmentStrings(szPath, expandedPath, _MAX_PATH))
	{
		lpcszPath = expandedPath;
	}
	else
		lpcszPath = szPath;

	DWORD attr = GetFileAttributes(lpcszPath);

	if (attr == ((DWORD) -1))
		return DOES_NOT_EXIST;
	else if (attr & FILE_ATTRIBUTE_DIRECTORY)
		return IS_EXISTING_DIR;
	else
		return IS_EXISTING_FILE;
}

/** 
 * @brief Strip trailing slas.
 * This function strips trailing slas from given path. Root paths are special
 * case and they are left intact. Since C:\ is a valid path but C: is not.
 * @param [in,out] sPath Path to strip.
 */
void paths_normalize(CString & sPath)
{
	int len = sPath.GetLength();
	if (!len)
		return;

	// prefix root with current drive
	sPath = paths_GetLongPath(sPath);

	// Do not remove trailing slash from root directories
	if (len == 3 && sPath[1] == ':')
		return;

	// remove any trailing slash
	if (paths_EndsWithSlash(sPath))
		sPath.Delete(sPath.GetLength()-1);
}

/**
 * @brief Get canonical name of folder.
 * @param [in] sDir Folder to handle.
 * @param [out] sName Canonicalized folder name.
 * @return true if canonical name exists.
 * @todo Should we return empty string as sName when returning false?
 */
static bool GetDirName(const CString & sDir, CString& sName)
{
	// FindFirstFile doesn't work for root:
	// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/fileio/fs/findfirstfile.asp
	// You cannot use root directories as the lpFileName input string for FindFirstFile—with or without a trailing backslash.
	if (sDir.GetLength() == 2 && sDir[1] == ':')
	{
		// I don't know if this work for empty root directories
		// because my first return value is not a dot directory, as I would have expected
		WIN32_FIND_DATA ffd;
		HANDLE h = FindFirstFile(sDir + _T("\\*"), &ffd);
		if (h == INVALID_HANDLE_VALUE)
			return false;
		FindClose(h);
		sName = sDir;
		return true;
	}
	// (Couldn't get info for just the directory from CFindFile)
	WIN32_FIND_DATA ffd;
	HANDLE h = FindFirstFile(sDir, &ffd);
	if (h == INVALID_HANDLE_VALUE)
		return false;
	sName = ffd.cFileName;
	FindClose(h);
	return true;
}

/**
 * Convert path to canonical long path.
 * This function converts given path to canonical long form. For example
 * foldenames with ~ short names are expanded. Also environment strings are
 * expanded if @p bExpandEnvs is TRUE. If path does not exist, make canonical
 * the part that does exist, and leave the rest as is. Result, if a directory,
 * usually does not have a trailing backslash.
 * @param [in] sPath Path to convert.
 * @param [in] bExpandEnvs If TRUE environment variables are expanded.
 * @return Converted path.
 */
CString paths_GetLongPath(const CString & sPath, BOOL bExpandEnvs)
{
	int len = sPath.GetLength();
	if (len < 1)
		return sPath;

	TCHAR fullPath[_MAX_PATH] = {0};
	TCHAR *lpPart;

	//                                         GetFullPathName  GetLongPathName
	// Convert to fully qualified form              Yes               No
	//    (Including .)
	// Convert /, //, \/, ... to \                  Yes               No
	// Handle ., .., ..\..\..                       Yes               No
	// Convert 8.3 names to long names              No                Yes
	// Fail when file/directory does not exist      No                Yes
	//
	// Fully qualify/normalize name using GetFullPathName.

	// Expand environment variables:
	// Convert "%userprofile%\My Documents" to "C:\Documents and Settings\username\My Documents"
	TCHAR expandedPath[_MAX_PATH] = {0};
	LPCTSTR lpcszPath = NULL;

	if (bExpandEnvs && _tcschr(sPath, '%'))
	{
		if (ExpandEnvironmentStrings(sPath, expandedPath, _MAX_PATH))
		{
			lpcszPath = expandedPath;
		}
	}
	else
		lpcszPath = sPath;

	if (!GetFullPathName(lpcszPath, _MAX_PATH, fullPath, &lpPart))
		_tcscpy(fullPath, sPath);

	// We are done if this is not a short name.
	if (_tcschr(fullPath, _T('~')) == NULL)
		return fullPath;

	// We have to do it the hard way because GetLongPathName is not
	// available on Win9x and some WinNT 4

	// The file/directory does not exist, use as much long name as we can
	// and leave the invalid stuff at the end.
	CString sLong;
	TCHAR *ptr = fullPath;
	TCHAR *end = NULL;

	// Skip to \ position     d:\abcd or \\host\share\abcd
	// indicated by ^           ^                    ^
	if (_tcslen(ptr) > 2)
		end = _tcschr(fullPath+2, _T('\\'));
	if (end && !_tcsnicmp(fullPath, _T("\\\\"),2))
		end = _tcschr(end+1, _T('\\'));

	if (!end)
		return fullPath;

	*end = 0;
	sLong += ptr;
	ptr = &end[1];
	CString sTemp; // used at each step to hold fully qualified short name

	// now walk down each directory and do short to long name conversion
	while (ptr)
	{
		end = _tcschr(ptr, '\\');
		// zero-terminate current component
		// (if we're at end, its already zero-terminated)
		if (end)
			*end = 0;

		sTemp = sLong + '\\' + ptr;

		// advance to next component (or set ptr==0 to flag end)
		ptr = (end ? end+1 : 0);

		// (Couldn't get info for just the directory from CFindFile)
		WIN32_FIND_DATA ffd;
		HANDLE h = FindFirstFile(sTemp, &ffd);
		if (h == INVALID_HANDLE_VALUE)
		{
			sLong = sTemp;
			if (ptr)
			{
				sLong += '\\';
				sLong += ptr;
			}
			return sLong;
		}
		sLong += '\\';
		sLong += ffd.cFileName;
		FindClose(h);
	}
	return sLong;
}

/**
 * @brief Check if the path exist and create the folder if needed.
 * This function checks if path exists. If path does not yet exist
 * function created needed folder structure. So this function is the
 * easy way to create a needed folder structure. Environment strings are
 * expanded when handling paths.
 * @param [in] sPath Path to check/create.
 * @return true if path exists or if we successfully created it.
 */
bool paths_CreateIfNeeded(const CString & sPath)
{
	if (sPath.IsEmpty()) return false;

	CString sTemp;
	if (GetDirName(sPath, sTemp)) return true;

	if (sPath.GetLength() >= _MAX_PATH) return false;

	// Expand environment variables:
	// Convert "%userprofile%\My Documents" to "C:\Documents and Settings\username\My Documents"
	TCHAR fullPath[_MAX_PATH] = _T("");
	if (!_tcschr(sPath, '%') || !ExpandEnvironmentStrings(sPath, fullPath, _MAX_PATH))
	{
		_tcscpy(fullPath, sPath);
	}
	// Now fullPath holds our desired path

	CString sLong;
	TCHAR *ptr = fullPath;
	TCHAR *end = NULL;

	// Skip to \ position     d:\abcd or \\host\share\abcd
	// indicated by ^           ^                    ^
	if (_tcslen(ptr) > 2)
		end = _tcschr(fullPath+2, _T('\\'));
	if (end && !_tcsnicmp(fullPath, _T("\\\\"),2))
		end = _tcschr(end+1, _T('\\'));

	if (!end) return false;

	// check that first component exists
	*end = 0;
	if (!GetDirName(fullPath, sTemp))
		return false;
	*end = '\\';

	ptr = end+1;

	while (ptr)
	{
		end = _tcschr(ptr, '\\');
		// zero-terminate current component
		// (if we're at end, its already zero-terminated)
		if (end)
			*end = 0;

		// advance to next component (or set ptr==0 to flag end)
		ptr = (end ? end+1 : 0);

		CString sNextName;
		if (!GetDirName(fullPath, sNextName))
		{
			// try to create directory, and then double-check its existence
			if (!CreateDirectory(fullPath, 0)
				|| !GetDirName(fullPath, sNextName))
			{
				return false;
			}
		}
		// if not finished, restore directory string we're working in
		if (ptr)
			*end = '\\';
	}
	return true;
}

// Static string used by paths_GetTempPath() for storing temp path.
static CString strTempPath;

/** 
 * @brief Get folder for temporary files.
 * This function returns system temp folder.
 * @param [out] pnerr Error code if erorr happened.
 * @return Temp path, or empty string if error happened.
 * @note Temp path is cached after first call.
 * @todo Should we return NULL for error case?
 */
LPCTSTR paths_GetTempPath(int * pnerr)
{
	if (strTempPath.IsEmpty())
	{
		int cchTempPath = GetTempPath(0, 0);
		if (!GetTempPath(cchTempPath, strTempPath.GetBufferSetLength(cchTempPath - 1)))
		{
			int err = GetLastError();
			if (pnerr)
				*pnerr = err;
#ifdef _DEBUG
			CString sysErr = GetSysError(err); // for debugging
#endif
			return strTempPath; // empty
		}
		strTempPath = paths_GetLongPath(strTempPath);
	}
	return strTempPath;
}

/** 
 * @brief Check if paths are both folders or files.
 * This function checks if paths are "compatible" as in many places we need
 * to have two folders or two files.
 * @param [in] pszLeft Left path.
 * @param [in] pszRight Right path.
 * @return One of:
 *  - IS_EXISTING_DIR : both are directories & exist
 *  - IS_EXISTING_FILE : both are files & exist
 *  - DOES_NOT_EXIST : in all other cases
*/
PATH_EXISTENCE GetPairComparability(LPCTSTR pszLeft, LPCTSTR pszRight)
{
	// fail if not both specified
	if (!pszLeft || !pszLeft[0] || !pszRight || !pszRight[0])
		return DOES_NOT_EXIST;
	PATH_EXISTENCE p1 = paths_DoesPathExist(pszLeft);
	// short circuit testing right if left doesn't exist
	if (p1 == DOES_NOT_EXIST) return DOES_NOT_EXIST;
	PATH_EXISTENCE p2 = paths_DoesPathExist(pszRight);
	if (p1 != p2) return DOES_NOT_EXIST;
	return p1;
}

//////////////////////////////////////////////////////////////////
//	use IShellLink to expand the shortcut
//	returns the expanded file, or "" on error
//
//	original code was part of CShortcut 
//	1996 by Rob Warner
//	rhwarner@southeast.net
//	http://users.southeast.net/~rhwarner

/** 
 * @brief Expand given shortcut to full path.
 * @param [in] inFile Shortcut to expand.
 * @return Full path or empty string if error happened.
 */
CString ExpandShortcut(const CString &inFile)
{
	CString outFile;

    // Make sure we have a path
    ASSERT(inFile != _T(""));

    IShellLink* psl;
    HRESULT hres;

    // Create instance for shell link
    hres = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
        IID_IShellLink, (LPVOID*) &psl);
    if (SUCCEEDED(hres))
    {
        // Get a pointer to the persist file interface
        IPersistFile* ppf;
        hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*) &ppf);
        if (SUCCEEDED(hres))
        {
	     USES_CONVERSION;
	     LPCTSTR szFile = inFile;
            // Load shortcut
            hres = ppf->Load(T2CW(szFile), STGM_READ);

            if (SUCCEEDED(hres)) {
				WIN32_FIND_DATA wfd;
				// find the path from that
				HRESULT hres = psl->GetPath(outFile.GetBuffer(MAX_PATH), 
								MAX_PATH,
								&wfd, 
								SLGP_UNCPRIORITY);

				outFile.ReleaseBuffer();
            }
            ppf->Release();
        }
        psl->Release();
    }

	// if this fails, outFile == ""
    return outFile;
}

/** 
 * @brief Append subpath to path.
 * This function appends subpath to given path. Function ensures there
 * is only one backslash between path parts.
 * @param [in] path "Base" path where other part is appended.
 * @param [in] subpath Path part to append to base part.
 * @return Formatted path. If one of arguments is empty then returns
 * non-empty argument. If both argumets are empty empty string is returned.
 */
CString paths_ConcatPath(const CString & path, const CString & subpath)
{
	if (path.IsEmpty()) return subpath;
	if (subpath.IsEmpty()) return path;
	if (paths_EndsWithSlash(path))
	{
		if (IsSlash(subpath, 0))
		{
			return path + subpath.Mid(1);
		}
		else
		{
			return path + subpath;
		}
	}
	else
	{
		if (IsSlash(subpath, 0))
		{
			return path + subpath;
		}
		else
		{
			return path + _T("\\") + subpath;
		}
	}
}

/** 
 * @brief Get parent path.
 * This function returns parent path for given path. For example for
 * path "c:\folder\subfolder" we return "c:\folder".
 * @param [in] path Path to get parent path for.
 * @return Parent path.
 */
CString paths_GetParentPath(CString path)
{
	CString parentPath;
	int len = path.GetLength();

	// Remove last '\' from paths
	if (path[len - 1] == '\\')
	{
		path.Delete(len - 1, 1);
		--len;
	}

	// Remove last part of path
	int pos = path.ReverseFind('\\');

	if (pos > -1)
	{
		path.Delete(pos, len - pos);
		parentPath = path;
	}
	return parentPath;
}

/** 
 * @brief Get last subdirectory of path.
 *
 * Returns last subdirectory name (if one exists) from given path.
 * For example:
 * - C:\work\myproject returns \myproject
 * @param [in] path Original path.
 * @return Last subdirectory in path.
 */
CString paths_GetLastSubdir(CString path)
{
	CString parentPath;
	int len = path.GetLength();

	// Remove last '\' from paths
	if (path[len - 1] == '\\')
	{
		path.Delete(len - 1, 1);
		--len;
	}

	// Find last part of path
	int pos = path.ReverseFind('\\');

	if (pos > 2)
	{
		path.Delete(0, pos);
		parentPath = path;
	}
	else
		return path;

	return parentPath;
}

/** 
 * @brief Checks if path is an absolute path.
 * @param [in] path Path to check.
 * @return TRUE if given path is absolute path.
 */
BOOL paths_IsPathAbsolute(const CString &path)
{
	if (path.GetLength() < 3)
		return FALSE;
	
	int pos = path.ReverseFind('\\');

	// Absolute path must have "\" and cannot start with it.
	// Also "\\blahblah" is invalid.
	if (pos < 2)
		return FALSE;

	// Maybe "X:\blahblah"?
	if (path[1] == ':' && path[2] == '\\')
		return TRUE;

	// So "\\blahblah\"?
	if (path[0] == '\\' && path[1] == '\\' && pos > 2)
		return TRUE;
	else
		return FALSE;
}

/**
 * @brief Get filename for temporary file.
 * @param [in] lpPathName Temporary file folder.
 * @param [in] lpPrefixString Prefix to use for filename.
 * @param [out] pnerr Error code if error happened.
 * @return Full path for temporary file or empty string if error happened.
 */
CString paths_GetTempFileName(LPCTSTR lpPathName, LPCTSTR lpPrefixString, int * pnerr)
{
	TCHAR buffer[MAX_PATH] = {0};
	if (_tcslen(lpPathName) > MAX_PATH-14) return _T(""); // failure
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

/**
 * @brief Checks if folder exists and creates it if needed.
 * This function checks if folder exists and creates it if not.
 * @param [in] sPath
 * @return Path if it exists or were created successfully. If
 * path points to file or folder failed to create returns empty
 * string.
 */
CString paths_EnsurePathExist(const CString & sPath)
{
	int rtn = paths_DoesPathExist(sPath);
	if (rtn == IS_EXISTING_DIR)
		return sPath;
	if (rtn == IS_EXISTING_FILE)
		return _T("");
	if (!paths_CreateIfNeeded(sPath))
		return _T("");
	// Check creating folder succeeded
	if (paths_DoesPathExist(sPath) == IS_EXISTING_DIR)
		return sPath;
	else
		return _T("");
}

/**
 * @brief Get Windows directory.
 * @return Windows directory.
 */
CString paths_GetWindowsDirectory()
{
	CString str;
	GetWindowsDirectory(str.GetBuffer(MAX_PATH), MAX_PATH);
	str.ReleaseBuffer();
	return str;
}

/**
 * @brief Return User's My Documents Folder.
 * This function returns full path to User's My Documents -folder.
 * @param [in] hWindow Parent window.
 * @return Full path to My Documents -folder.
 */
CString paths_GetMyDocuments(HWND hWindow)
{
	LPITEMIDLIST pidl;
	LPMALLOC pMalloc;
	CString path;
	TCHAR szPath[MAX_PATH] = {0};

	HRESULT rv = SHGetSpecialFolderLocation(hWindow, CSIDL_PERSONAL, &pidl);
	if (rv == S_OK)
	{
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
