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

bool IsSlash(LPCTSTR pszStart, int nPos)
{
	return pszStart[nPos]=='/' || 
#ifdef _UNICODE
	       pszStart[nPos]=='\\';
#else
		// Avoid 0x5C (ASCII backslash) byte occurring as trail byte in MBCS
	       (pszStart[nPos]=='\\' && !_ismbstrail((unsigned char *)pszStart, (unsigned char *)pszStart + nPos));
#endif
}

bool EndsWithSlash(const CString & s)
{
	return !s.IsEmpty() && IsSlash(s, s.GetLength()-1);
}

PATH_EXISTENCE paths_DoesPathExist(LPCTSTR szPath)
{
	if (!szPath || !szPath[0]) return DOES_NOT_EXIST;

	DWORD attr = GetFileAttributes(szPath);

	if (attr == ((DWORD) -1))
		return DOES_NOT_EXIST;
	else if (attr & FILE_ATTRIBUTE_DIRECTORY)
		return IS_EXISTING_DIR;
	else
		return IS_EXISTING_FILE;
}

// strip trailing slashes (except from root paths)
void paths_normalize(CString & sPath)
{
	int len = sPath.GetLength();
	if (!len) return;

	// prefix root with current drive
	sPath = paths_GetLongPath(sPath, DIRSLASH);

	// Do not remove trailing slash from root directories
	if (len == 3 && sPath[1] == ':')
		return;

	// remove any trailing slash
	if (EndsWithSlash(sPath))
		sPath.Delete(sPath.GetLength()-1);
}

// get long name (optionally terminate directories with slash)
CString paths_GetLongPath(const CString & sPath, DIRSLASH_TYPE dst)
{
	int len = sPath.GetLength();
	if (len < 1) return sPath;

	TCHAR fullPath[_MAX_PATH];
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
	if (!GetFullPathName(sPath, _MAX_PATH, fullPath, &lpPart))
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

	if (!end) return fullPath;

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
				sLong += (CString)_T("\\") + ptr;
			// No trailing slash no matter dst setting
			// because it (requested path) doesn't exist
			return sLong;
		}
		sLong += '\\';
		sLong += ffd.cFileName;
		if (dst == DIRSLASH && !ptr &&
			(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			sLong += '\\';
		}
		FindClose(h);
	}
	return sLong;
}

// return IS_EXISTING_DIR if both are directories & exist
// return IS_EXISTING_FILE if both are files & exist
// return DOES_NOT_EXIST in all other cases
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
CString ExpandShortcut(CString &inFile)
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

// Append subpath to path
// Skip empty arguments
// Ensure exactly one backslash between them in result
CString paths_ConcatPath(const CString & path, const CString & subpath)
{
	if (path.IsEmpty()) return subpath;
	if (subpath.IsEmpty()) return path;
	if (EndsWithSlash(path))
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
 * @brief Get parent path
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
 * @brief Checks if path is absolute path
 */
BOOL paths_IsPathAbsolute(CString path)
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
