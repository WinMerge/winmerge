/** 
 * @file  paths.cpp
 *
 * @brief Path handling routines
 */

#include "paths.h"
#include <windows.h>
#include <cassert>
#include <cstring>
#include <direct.h>
#include <mbctype.h> // MBCS (multibyte codepage stuff)
#pragma warning (push)			// prevent "warning C4091: 'typedef ': ignored on left of 'tagGPFIDL_FLAGS' when no variable is declared"
#pragma warning (disable:4091)	// VC bug when using XP enabled toolsets.
#include <shlobj.h>
#pragma warning (pop)
#include <shlwapi.h>
#include "PathContext.h"
#include "coretools.h"

namespace paths
{

static bool IsSlash(const String& pszStart, size_t nPos);
static bool GetDirName(const String& sDir, String& sName);

/** 
 * @brief Checks if char in string is slash.
 * @param [in] pszStart String to check.
 * @param [in] nPos of char in string to check (0-based index).
 * @return true if char is slash.
 */
static bool IsSlash(const String& pszStart, size_t nPos)
{
	return pszStart[nPos]=='/' || 
#ifdef _UNICODE
	       pszStart[nPos]=='\\';
#else
		// Avoid 0x5C (ASCII backslash) byte occurring as trail byte in MBCS
	       (pszStart[nPos]=='\\' && !_ismbstrail((unsigned char *)pszStart.c_str(), (unsigned char *)pszStart.c_str() + nPos));
#endif
}

/** 
 * @brief Checks if string ends with slash.
 * This function checks if given string ends with slash. In many places,
 * especially in GUI, we assume folder paths end with slash.
 * @param [in] s String to check.
 * @return true if last char in string is slash.
 */
bool EndsWithSlash(const String& s)
{
	if (size_t len = s.length())
		return IsSlash(s, (int)len - 1);
	return false;
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
PATH_EXISTENCE DoesPathExist(const String& szPath, bool (*IsArchiveFile)(const String&))
{
	if (szPath.empty())
		return DOES_NOT_EXIST;

	// Expand environment variables:
	// Convert "%userprofile%\My Documents" to "C:\Documents and Settings\username\My Documents"
	const TCHAR *lpcszPath = szPath.c_str();
	TCHAR expandedPath[_MAX_PATH];

	if (_tcschr(lpcszPath, '%'))
	{
		DWORD dwLen = ExpandEnvironmentStrings(lpcszPath, expandedPath, _MAX_PATH);
		if (dwLen > 0 && dwLen < _MAX_PATH)
			lpcszPath = expandedPath;
	}

	DWORD attr = GetFileAttributes(lpcszPath);

	if (attr == ((DWORD) -1))
	{
		if (IsArchiveFile && IsArchiveFile(szPath))
			return IS_EXISTING_DIR;
		return DOES_NOT_EXIST;
	}
	else if (attr & FILE_ATTRIBUTE_DIRECTORY)
		return IS_EXISTING_DIR;
	else
	{
		if (IsArchiveFile && IsArchiveFile(szPath))
			return IS_EXISTING_DIR;
		return IS_EXISTING_FILE;
	}
}

/**
 * @brief Like shlwapi's PathFindFileName(), but works with both \ and /.
 * @param [in] Path
 * @return Filename
 */
String FindFileName(const String& path)
{
	const TCHAR *filename = path.c_str();
	while (const TCHAR *slash = _tcspbrk(filename, _T("\\/")))
	{
		if (*(slash + 1) == '\0')
			break;
		filename = slash + 1;
	}
	return filename;
}

/**
 * @brief Like shlwapi's PathFindFileName(), but works with both \ and /.
 * @param [in] Path
 * @return Filename
 */
String FindExtension(const String& path)
{
	return ::PathFindExtension(path.c_str());
}

/** 
 * @brief Strip trailing slas.
 * This function strips trailing slas from given path. Root paths are special
 * case and they are left intact. Since C:\ is a valid path but C: is not.
 * @param [in,out] sPath Path to strip.
 */
void normalize(String & sPath)
{
	size_t len = sPath.length();
	if (!len)
		return;

	// prefix root with current drive
	sPath = GetLongPath(sPath);

	// Do not remove trailing slash from root directories
	if (len == 3 && sPath[1] == ':')
		return;

	// remove any trailing slash
	if (EndsWithSlash(sPath))
		sPath.resize(sPath.length() - 1);
}

/**
 * @brief Get canonical name of folder.
 * @param [in] sDir Folder to handle.
 * @param [out] sName Canonicalized folder name.
 * @return true if canonical name exists.
 * @todo Should we return empty string as sName when returning false?
 */
static bool GetDirName(const String& sDir, String& sName)
{
	// FindFirstFile doesn't work for root:
	// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/fileio/fs/findfirstfile.asp
	// You cannot use root directories as the lpFileName input string for FindFirstFile—with or without a trailing backslash.
	if (sDir[0] && sDir[1] == ':' && sDir[2] == '\0')
	{
		// I don't know if this work for empty root directories
		// because my first return value is not a dot directory, as I would have expected
		WIN32_FIND_DATA ffd;
		TCHAR sPath[8];
		StringCchPrintf(sPath, sizeof(sPath)/sizeof(sPath[0]), _T("%s\\*"), sDir.c_str());
		HANDLE h = FindFirstFile(sPath, &ffd);
		if (h == INVALID_HANDLE_VALUE)
			return false;
		FindClose(h);
		sName = sDir;
		return true;
	}
	// (Couldn't get info for just the directory from CFindFile)
	WIN32_FIND_DATA ffd;
	HANDLE h = FindFirstFile(sDir.c_str(), &ffd);
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
 * expanded if @p bExpandEnvs is true. If path does not exist, make canonical
 * the part that does exist, and leave the rest as is. Result, if a directory,
 * usually does not have a trailing backslash.
 * @param [in] sPath Path to convert.
 * @param [in] bExpandEnvs If true environment variables are expanded.
 * @return Converted path.
 */
String GetLongPath(const String& szPath, bool bExpandEnvs)
{
	String sPath = szPath;
	size_t len = sPath.length();
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
	TCHAR expandedPath[_MAX_PATH];
	const TCHAR *lpcszPath = sPath.c_str();
	if (bExpandEnvs && _tcschr(lpcszPath, '%'))
	{
		DWORD dwLen = ExpandEnvironmentStrings(lpcszPath, expandedPath, _MAX_PATH);
		if (dwLen > 0 && dwLen < _MAX_PATH)
			lpcszPath = expandedPath;
	}

	DWORD dwLen = GetFullPathName(lpcszPath, _MAX_PATH, fullPath, &lpPart);
	if (dwLen == 0 || dwLen >= _MAX_PATH)
		_tcscpy_safe(fullPath, lpcszPath);

	// We are done if this is not a short name.
	if (_tcschr(fullPath, _T('~')) == NULL)
		return fullPath;

	// We have to do it the hard way because GetLongPathName is not
	// available on Win9x and some WinNT 4

	// The file/directory does not exist, use as much long name as we can
	// and leave the invalid stuff at the end.
	String sLong;
	TCHAR *ptr = fullPath;
	TCHAR *end = NULL;

	// Skip to \ position     d:\abcd or \\host\share\abcd
	// indicated by ^           ^                    ^
	if (_tcslen(ptr) > 2)
		end = _tcschr(fullPath+2, _T('\\'));
	if (end && !_tcsncmp(fullPath, _T("\\\\"),2))
		end = _tcschr(end+1, _T('\\'));

	if (!end)
		return fullPath;

	*end = 0;
	sLong += ptr;
	ptr = &end[1];

	// now walk down each directory and do short to long name conversion
	while (ptr)
	{
		end = _tcschr(ptr, '\\');
		// zero-terminate current component
		// (if we're at end, its already zero-terminated)
		if (end)
			*end = 0;

		String sTemp(sLong);
		sTemp += '\\';
		sTemp += ptr;

		// advance to next component (or set ptr==0 to flag end)
		ptr = (end ? end+1 : 0);

		// (Couldn't get info for just the directory from CFindFile)
		WIN32_FIND_DATA ffd;
		HANDLE h = FindFirstFile(sTemp.c_str(), &ffd);
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
bool CreateIfNeeded(const String& szPath)
{
	if (szPath.empty())
		return false;

	String sTemp;
	if (GetDirName(szPath, sTemp))
		return true;

	if (szPath.length() >= _MAX_PATH)
		return false;

	// Expand environment variables:
	// Convert "%userprofile%\My Documents" to "C:\Documents and Settings\username\My Documents"
	TCHAR fullPath[_MAX_PATH];
	if (_tcschr(szPath.c_str(), '%'))
	{
		DWORD dwLen = ExpandEnvironmentStrings(szPath.c_str(), fullPath, _MAX_PATH);
		if (dwLen == 0 || dwLen >= _MAX_PATH)
			_tcscpy_safe(fullPath, szPath.c_str());
	}
	else
		_tcscpy_safe(fullPath, szPath.c_str());
	// Now fullPath holds our desired path

	TCHAR *ptr = fullPath;
	TCHAR *end = NULL;

	// Skip to \ position     d:\abcd or \\host\share\abcd
	// indicated by ^           ^                    ^
	if (_tcslen(ptr) > 2)
		end = _tcschr(fullPath+2, _T('\\'));
	if (end && !_tcsncmp(fullPath, _T("\\\\"),2))
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

		String sNextName;
		if (!GetDirName(fullPath, sNextName))
		{
			// try to create directory, and then double-check its existence
			if (!CreateDirectory(fullPath, 0) ||
				!GetDirName(fullPath, sNextName))
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

/** 
 * @brief Check if paths are both folders or files.
 * This function checks if paths are "compatible" as in many places we need
 * to have two folders or two files.
 * @param [in] paths Left and right paths.
 * @return One of:
 *  - IS_EXISTING_DIR : both are directories & exist
 *  - IS_EXISTING_FILE : both are files & exist
 *  - DOES_NOT_EXIST : in all other cases
*/
PATH_EXISTENCE GetPairComparability(const PathContext & paths, bool (*IsArchiveFile)(const String&))
{
	// fail if not both specified
	if (paths.GetSize() < 2 || paths[0].empty() || paths[1].empty())
		return DOES_NOT_EXIST;
	PATH_EXISTENCE p1 = DoesPathExist(paths[0], IsArchiveFile);
	// short circuit testing right if left doesn't exist
	if (p1 == DOES_NOT_EXIST)
		return DOES_NOT_EXIST;
	PATH_EXISTENCE p2 = DoesPathExist(paths[1], IsArchiveFile);
	if (p1 != p2)
	{
		p1 = DoesPathExist(paths[0]);
		p2 = DoesPathExist(paths[1]);
		if (p1 != p2)
			return DOES_NOT_EXIST;
	}
	if (paths.GetSize() < 3) return p1; 
	PATH_EXISTENCE p3 = DoesPathExist(paths[2], IsArchiveFile);
	if (p2 != p3)
	{
		p1 = DoesPathExist(paths[0]);
		p2 = DoesPathExist(paths[1]);
		p3 = DoesPathExist(paths[2]);
		if (p1 != p2 || p2 != p3)
			return DOES_NOT_EXIST;
	}
	return p1;
}

/**
 * @brief Check if the given path points to shotcut.
 * Windows considers paths having a filename with extension ".lnk" as
 * shortcuts. This function checks if the given path is shortcut.
 * We usually want to expand shortcuts with ExpandShortcut().
 * @param [in] inPath Path to check;
 * @return true if the path points to shortcut, false otherwise.
 */
bool IsShortcut(const String& inPath)
{
	const TCHAR ShortcutExt[] = _T(".lnk");
	TCHAR ext[_MAX_EXT] = {0};
	_tsplitpath_s(inPath.c_str(), NULL, 0, NULL, 0, NULL, 0, ext, _MAX_EXT);
	if (_tcsicmp(ext, ShortcutExt) == 0)
		return true;
	else
		return false;
}

bool IsDirectory(const String &path)
{
	return !!PathIsDirectory(path.c_str());
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
String ExpandShortcut(const String &inFile)
{
	assert(inFile != _T(""));

	// No path, nothing to return
	if (inFile == _T(""))
		return _T("");

	String outFile;
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
			WCHAR wsz[MAX_PATH];
#ifdef _UNICODE
			_tcscpy_safe(wsz, inFile.c_str());
#else
			::MultiByteToWideChar(CP_ACP, 0, inFile.c_str(), -1, wsz, MAX_PATH);
#endif

			// Load shortcut
			hres = ppf->Load(wsz, STGM_READ);

			if (SUCCEEDED(hres))
			{
				// find the path from that
				TCHAR buf[MAX_PATH] = {0};
				psl->GetPath(buf, MAX_PATH, NULL, SLGP_UNCPRIORITY);
				outFile = buf;
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
String ConcatPath(const String & path, const String & subpath)
{
	if (path.empty())
		return subpath;
	if (subpath.empty())
		return path;
	if (EndsWithSlash(path))
	{
		return String(path).append(subpath.c_str() + (IsSlash(subpath, 0) ? 1 : 0));
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
String GetParentPath(const String& path)
{
	String parentPath(path);
	size_t len = parentPath.length();

	// Remove last '\' from paths
	if (parentPath[len - 1] == '\\')
	{
		parentPath.resize(len - 1);
		--len;
	}

	// Remove last part of path
	size_t pos = parentPath.rfind('\\');

	if (pos != parentPath.npos)
	{
		// Do not remove trailing slash from root directories
		parentPath.resize(pos == 2 ? pos + 1 : pos);
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
String GetLastSubdir(const String & path)
{
	String parentPath(path);
	size_t len = parentPath.length();

	// Remove last '\' from paths
	if (parentPath[len - 1] == '\\')
	{
		parentPath.erase(len - 1, 1);
		--len;
	}

	// Find last part of path
	size_t pos = parentPath.find_last_of('\\');
	if (pos >= 2 && pos != String::npos)
		parentPath.erase(0, pos);
	return parentPath;
}

/** 
 * @brief Checks if path is an absolute path.
 * @param [in] path Path to check.
 * @return true if given path is absolute path.
 */
bool IsPathAbsolute(const String &path)
{
	if (path.length() < 3)
		return false;
	
	size_t pos = path.find_last_of('\\');

	// Absolute path must have "\" and cannot start with it.
	// Also "\\blahblah" is invalid.
	if (pos < 2 || pos == String::npos)
		return false;

	// Maybe "X:\blahblah"?
	if (path[1] == ':' && path[2] == '\\')
		return true;

	// So "\\blahblah\"?
	if (path[0] == '\\' && path[1] == '\\' && pos > 2)
		return true;
	else
		return false;
}

/**
 * @brief Checks if folder exists and creates it if needed.
 * This function checks if folder exists and creates it if not.
 * @param [in] sPath
 * @return Path if it exists or were created successfully. If
 * path points to file or folder failed to create returns empty
 * string.
 */
String EnsurePathExist(const String & sPath)
{
	int rtn = DoesPathExist(sPath);
	if (rtn == IS_EXISTING_DIR)
		return sPath;
	if (rtn == IS_EXISTING_FILE)
		return _T("");
	if (!CreateIfNeeded(sPath))
		return _T("");
	// Check creating folder succeeded
	if (DoesPathExist(sPath) == IS_EXISTING_DIR)
		return sPath;
	else
		return _T("");
}

/**
 * @brief Return true if *pszChar is a slash (either direction) or a colon
 *
 * begin points to start of string, in case multibyte trail test is needed
 */
bool IsSlashOrColon(const TCHAR *pszChar, const TCHAR *begin)
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
void SplitFilename(const String& pathLeft, String* pPath, String* pFile, String* pExt)
{
	const TCHAR *pszChar = pathLeft.c_str() + pathLeft.length();
	const TCHAR *pend = pszChar;
	const TCHAR *extptr = 0;
	bool ext = false;

	while (pathLeft.c_str() < --pszChar)
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
		else if (IsSlashOrColon(pszChar, pathLeft.c_str()))
		{
			// Ok, found last slash, so we collect any info desired
			// and we're done

			if (pPath)
			{
				// Grab directory (omit trailing slash)
				size_t len = pszChar - pathLeft.c_str();
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
void SplitViewName(const TCHAR *s, String * path, String * name, String * ext)
{
	String sViewName(s);
	size_t nOffset = sViewName.find(_T("@@"));
	if (nOffset != String::npos)
	{
		sViewName.erase(nOffset);
		SplitFilename(sViewName, path, name, ext);
	}
}

/**
 * @brief Return path component from full path.
 * @param [in] fullpath Full path to split.
 * @return Path without filename.
 */
String GetPathOnly(const String& fullpath)
{
	if (fullpath.empty()) return _T("");
	String spath;
	SplitFilename(fullpath, &spath, 0, 0);
	return spath;
}

bool IsURLorCLSID(const String& path)
{
	return (path.find(_T("://")) != String::npos || path.find(_T("::{")) != String::npos);
}

bool IsDecendant(const String& path, const String& ancestor)
{
	return path.length() > ancestor.length() && 
		   strutils::compare_nocase(String(path.c_str(), path.c_str() + ancestor.length()), ancestor) == 0;
}

static void replace_char(TCHAR *s, int target, int repl)
{
	TCHAR *p;
	for (p=s; *p != _T('\0'); p = _tcsinc(p))
		if (*p == target)
			*p = (TCHAR)repl;
}

String ToWindowsPath(const String& path)
{
	String winpath = path;
	replace_char(&*winpath.begin(), '/', '\\');
	return winpath;
}

String ToUnixPath(const String& path)
{
	String unixpath = path;
	replace_char(&*unixpath.begin(), '\\', '/');
	return unixpath;
}

}
