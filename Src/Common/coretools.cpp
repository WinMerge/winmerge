/**
 * @file  coretools.cpp
 *
 * @brief Common routines
 *
 */
// ID line follows -- this is updated by SVN
// $Id: coretools.cpp 7168 2010-05-16 13:45:23Z jtuc $

#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <mbctype.h> // MBCS (multibyte codepage stuff)
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <assert.h>
#include "UnicodeString.h"
#include "coretools.h"
#include "coretypes.h"

size_t linelen(const char *string, size_t maxlen)
{
	size_t stringlen = 0;
	while (stringlen < maxlen)
	{
		char c = string[stringlen];
		if (c == '\r' || c == '\n' || c == '\0')
			break;
		++stringlen;
	}
	return stringlen;
}

void replace_char(LPTSTR s, int target, int repl)
{
	TCHAR *p;
	for (p=s; *p != _T('\0'); p = _tcsinc(p))
		if (*p == target)
			*p = (TCHAR)repl;
}

/**
 * @brief Return true if *pszChar is a slash (either direction) or a colon
 *
 * begin points to start of string, in case multibyte trail test is needed
 */
bool IsSlashOrColon(LPCTSTR pszChar, LPCTSTR begin)
{
#ifdef _UNICODE
		return (*pszChar == '/' || *pszChar == ':' || *pszChar == '\\');
#else
		// Avoid 0x5C (ASCII backslash) byte occurring as trail byte in MBCS
		return (*pszChar == '/' || *pszChar == ':' 
			|| (*pszChar == '\\' && !_ismbstrail((unsigned char *)begin, (unsigned char *)pszChar)));
#endif
}

/**
 * @brief Extract path name components from given full path.
 * @param [in] pathLeft Original path.
 * @param [out] pPath Folder name component of full path, excluding
   trailing slash.
 * @param [out] pFile File name part, excluding extension.
 * @param [out] pExt Filename extension part, excluding leading dot.
 */
void SplitFilename(LPCTSTR pathLeft, String* pPath, String* pFile, String* pExt)
{
	LPCTSTR pszChar = pathLeft + _tcslen(pathLeft);
	LPCTSTR pend = pszChar;
	LPCTSTR extptr = 0;
	bool ext = false;

	while (pathLeft < --pszChar)
	{
		if (*pszChar == '.')
		{
			if (!ext)
			{
				if (pExt)
				{
					(*pExt) = pszChar + 1;
				}
				ext = true; // extension is only after last period
				extptr = pszChar;
			}
		}
		else if (IsSlashOrColon(pszChar, pathLeft))
		{
			// Ok, found last slash, so we collect any info desired
			// and we're done

			if (pPath)
			{
				// Grab directory (omit trailing slash)
				size_t len = pszChar - pathLeft;
				if (*pszChar == ':')
					++len; // Keep trailing colon ( eg, C:filename.txt)
				*pPath = pathLeft;
				pPath->erase(len); // Cut rest of path
			}

			if (pFile)
			{
				// Grab file
				*pFile = pszChar + 1;
			}

			goto endSplit;
		}
	}

	// Never found a delimiter
	if (pFile)
	{
		*pFile = pathLeft;
	}

endSplit:
	// if both filename & extension requested, remove extension from filename

	if (pFile && pExt && extptr)
	{
		size_t extlen = pend - extptr;
		pFile->erase(pFile->length() - extlen);
	}
}

// Split Rational ClearCase view name (file_name@@file_version).
void SplitViewName(LPCTSTR s, String * path, String * name, String * ext)
{
	String sViewName(s);
	size_t nOffset = sViewName.find(_T("@@"));
	if (nOffset != String::npos)
	{
		sViewName.erase(nOffset);
		SplitFilename(sViewName.c_str(), path, name, ext);
	}
}

HANDLE RunIt(LPCTSTR szExeFile, LPCTSTR szArgs, BOOL bMinimized /*= TRUE*/, BOOL bNewConsole /*= FALSE*/)
{
    STARTUPINFO si = {0};
	PROCESS_INFORMATION procInfo = {0};

    si.cb = sizeof(STARTUPINFO);
    si.lpDesktop = _T("");
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = (bMinimized) ? SW_MINIMIZE : SW_HIDE;

	TCHAR args[4096];
	if (szExeFile)
		_sntprintf(args, countof(args), _T("\"%s\" %s"), szExeFile, szArgs);
	else
		_sntprintf(args, countof(args), _T("%s"), szArgs);
    if (CreateProcess(szExeFile, args, NULL, NULL,
		FALSE, NORMAL_PRIORITY_CLASS|(bNewConsole? CREATE_NEW_CONSOLE:0),
                         NULL, _T(".\\"), &si, &procInfo))
	{
		CloseHandle(procInfo.hThread);
		return procInfo.hProcess;
	}

	return INVALID_HANDLE_VALUE;
}

BOOL HasExited(HANDLE hProcess, DWORD *pCode)
{
     DWORD code;
     if (GetExitCodeProcess(hProcess, &code)
         && code != STILL_ACTIVE)
     {
		 *pCode = code;
         return TRUE;
     }
     return FALSE;
}

/**
 * @brief Return module's path component (without filename).
 * @param [in] hModule Module's handle.
 * @return Module's path.
 */
String GetModulePath(HMODULE hModule /* = NULL*/)
{
	TCHAR temp[MAX_PATH] = {0};
	GetModuleFileName(hModule, temp, MAX_PATH);
	return GetPathOnly(temp);
}

/**
 * @brief Return path component from full path.
 * @param [in] fullpath Full path to split.
 * @return Path without filename.
 */
String GetPathOnly(LPCTSTR fullpath)
{
	if (!fullpath || !fullpath[0]) return _T("");
	String spath;
	SplitFilename(fullpath, &spath, 0, 0);
	return spath;
}

