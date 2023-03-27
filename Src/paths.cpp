/** 
 * @file  paths.cpp
 *
 * @brief Path handling routines
 */

#include "pch.h"
#include "paths.h"
#include <windows.h>
#include <cassert>
#include <cstring>
#include <direct.h>
#pragma warning (push)			// prevent "warning C4091: 'typedef ': ignored on left of 'tagGPFIDL_FLAGS' when no variable is declared"
#pragma warning (disable:4091)	// VC bug when using XP enabled toolsets.
#include <shlobj.h>
#pragma warning (pop)
#include <shlwapi.h>
#include "PathContext.h"
#include "coretools.h"
#include "TFile.h"

namespace paths
{

static bool IsSlash(const String& pszStart, size_t nPos);
static bool IsDirName(const String& sDir);

/** 
 * @brief Checks if char in string is slash.
 * @param [in] pszStart String to check.
 * @param [in] nPos of char in string to check (0-based index).
 * @return true if char is slash.
 */
static bool IsSlash(const String& pszStart, size_t nPos)
{
	return pszStart[nPos]=='/' || 
	       pszStart[nPos]=='\\';
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
PATH_EXISTENCE DoesPathExist(const String& szPath, bool (*IsArchiveFile)(const String&) /*= nullptr*/)
{
	if (szPath.empty())
		return DOES_NOT_EXIST;

	// Expand environment variables:
	// Convert "%userprofile%\My Documents" to "C:\Documents and Settings\username\My Documents"
	const tchar_t *lpcszPath = szPath.c_str();
	tchar_t expandedPath[MAX_PATH_FULL];

	if (tc::tcschr(lpcszPath, '%') != nullptr)
	{
		DWORD dwLen = ExpandEnvironmentStrings(lpcszPath, expandedPath, MAX_PATH_FULL);
		if (dwLen > 0 && dwLen < MAX_PATH_FULL)
			lpcszPath = expandedPath;
	}

	DWORD attr = GetFileAttributes(TFile(String(lpcszPath)).wpath().c_str());

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
	const tchar_t *filename = path.c_str();
	while (const tchar_t *slash = tc::tcspbrk(filename, _T("\\/")))
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

String RemoveExtension(const String& path)
{
	String ext = FindExtension(path);
	return path.substr(0, path.length() - ext.length());
}

/** 
 * @brief Strip trailing slas.
 * This function strips trailing slash from given path. Root paths are special
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
 * @brief Returns whether the given path is a directory name
 * @param [in] sDir Folder to handle.
 * @return true the given path is a directory name
 */
static bool IsDirName(const String& sDir)
{
	// FindFirstFile doesn't work for root:
	// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/fileio/fs/findfirstfile.asp
	// You cannot use root directories as the lpFileName input string for FindFirstFile - with or without a trailing backslash.
	size_t count = 0;
	if ((sDir[0] && sDir[1] == ':' && sDir[2] == '\0') ||
	    // \\host\share or \\host\share\ 
	    (sDir[0] == '\\' && sDir[1] == '\\' && 
	     (count = std::count(sDir.begin(), sDir.end(), ('\\'))) <= 4 &&
	     (count == 3 || (count == 4 && sDir.back() == '\\'))))
	{
		// I don't know if this work for empty root directories
		// because my first return value is not a dot directory, as I would have expected
		WIN32_FIND_DATA ffd;
		HANDLE h = FindFirstFile((sDir + (sDir.back() == '\\' ? _T("*") : _T("\\*"))).c_str(), &ffd);
		if (h == INVALID_HANDLE_VALUE)
			return false;
		FindClose(h);
		return true;
	}
	// (Couldn't get info for just the directory from CFindFile)
	WIN32_FIND_DATA ffd;
	
	HANDLE h = FindFirstFile(TFile(sDir).wpath().c_str(), &ffd);
	if (h == INVALID_HANDLE_VALUE)
		return false;
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

	tchar_t fullPath[MAX_PATH_FULL] = {0};
	tchar_t *pFullPath = &fullPath[0];
	tchar_t *lpPart;

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
	tchar_t expandedPath[MAX_PATH_FULL];
	const tchar_t *lpcszPath = sPath.c_str();
	if (bExpandEnvs && tc::tcschr(lpcszPath, '%') != nullptr)
	{
		DWORD dwLen = ExpandEnvironmentStrings(lpcszPath, expandedPath, MAX_PATH_FULL);
		if (dwLen > 0 && dwLen < MAX_PATH_FULL)
			lpcszPath = expandedPath;
	}
	
	String tPath = TFile(String(lpcszPath)).wpath();
	DWORD dwLen = GetFullPathName(tPath.c_str(), MAX_PATH_FULL, pFullPath, &lpPart);
	if (dwLen == 0 || dwLen >= MAX_PATH_FULL)
		tc::tcslcpy(pFullPath, MAX_PATH_FULL, tPath.c_str());

	// We are done if this is not a short name.
	if (tc::tcschr(pFullPath, _T('~')) == nullptr)
		return pFullPath;

	// We have to do it the hard way because GetLongPathName is not
	// available on Win9x and some WinNT 4

	// The file/directory does not exist, use as much long name as we can
	// and leave the invalid stuff at the end.
	String sLong;
	tchar_t *ptr = pFullPath;
	tchar_t *end = nullptr;

	// Skip to \ position     d:\abcd or \\host\share\abcd
	// indicated by ^           ^                    ^
	if (tc::tcslen(ptr) > 2)
		end = tc::tcschr(pFullPath+2, _T('\\'));
	if (end != nullptr && !tc::tcsncmp(pFullPath, _T("\\\\"),2))
		end = tc::tcschr(end+1, _T('\\'));

	if (end == nullptr)
		return pFullPath;

	*end = 0;
	sLong += ptr;
	ptr = &end[1];

	// now walk down each directory and do short to long name conversion
	while (ptr != nullptr)
	{
		end = tc::tcschr(ptr, '\\');
		// zero-terminate current component
		// (if we're at end, its already zero-terminated)
		if (end != nullptr)
			*end = 0;

		String sTemp(sLong);
		sTemp += '\\';
		sTemp += ptr;

		// advance to next component (or set ptr=`nullptr` to flag end)
		ptr = (end!=nullptr ? end+1 : nullptr);

		// (Couldn't get info for just the directory from CFindFile)
		WIN32_FIND_DATA ffd;
		HANDLE h = FindFirstFile(TFile(sTemp).wpath().c_str(), &ffd);
		if (h == INVALID_HANDLE_VALUE)
		{
			sLong = std::move(sTemp);
			if (ptr != nullptr)
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

	if (IsDirName(szPath))
		return true;

	if (szPath.length() >= MAX_PATH_FULL)
		return false;

	// Expand environment variables:
	// Convert "%userprofile%\My Documents" to "C:\Documents and Settings\username\My Documents"
	tchar_t fullPath[MAX_PATH_FULL];
	fullPath[0] = '\0';
	if (tc::tcschr(szPath.c_str(), '%') != nullptr)
	{
		DWORD dwLen = ExpandEnvironmentStrings(szPath.c_str(), fullPath, MAX_PATH_FULL);
		if (dwLen == 0 || dwLen >= MAX_PATH_FULL)
			tc::tcslcpy(fullPath, szPath.c_str());
	}
	else
		tc::tcslcpy(fullPath, szPath.c_str());
	// Now fullPath holds our desired path

	tchar_t *ptr = fullPath;
	tchar_t *end = nullptr;

	// Skip to \ position     d:\abcd or \\host\share\abcd
	// indicated by ^           ^                    ^
	if (tc::tcslen(ptr) > 2)
		end = tc::tcschr(fullPath+2, _T('\\'));
	if (end != nullptr && !tc::tcsncmp(fullPath, _T("\\\\"),2))
		end = tc::tcschr(end+1, _T('\\'));

	if (end == nullptr) return false;

	// check that first component exists
	*end = 0;
	if (!IsDirName(fullPath))
		return false;
	*end = '\\';

	ptr = end+1;

	while (ptr != nullptr)
	{
		end = tc::tcschr(ptr, '\\');
		// zero-terminate current component
		// (if we're at end, its already zero-terminated)
		if (end != nullptr)
			*end = 0;

		// advance to next component (or set ptr=`nullptr` to flag end)
		ptr = (end != nullptr ? end+1 : nullptr);

		if (!IsDirName(fullPath))
		{
			// try to create directory, and then double-check its existence
			if (!CreateDirectory(fullPath, 0) ||
				!IsDirName(fullPath))
			{
				return false;
			}
		}
		// if not finished, restore directory string we're working in
		if (ptr != nullptr)
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
PATH_EXISTENCE GetPairComparability(const PathContext & paths, bool (*IsArchiveFile)(const String&) /*= nullptr*/)
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
	const tchar_t ShortcutExt[] = _T(".lnk");
	tchar_t ext[_MAX_EXT] = {0};
	_wsplitpath_s(inPath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT);
	if (tc::tcsicmp(ext, ShortcutExt) == 0)
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
	assert(!inFile.empty());

	// No path, nothing to return
	if (inFile.empty())
		return _T("");

	String outFile;
	IShellLink* psl;
	HRESULT hres;

	// Create instance for shell link
	hres = ::CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
		IID_IShellLink, (LPVOID*) &psl);
	if (SUCCEEDED(hres))
	{
		// Get a pointer to the persist file interface
		IPersistFile* ppf;
		hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*) &ppf);
		if (SUCCEEDED(hres))
		{
			WCHAR wsz[MAX_PATH_FULL];
			tc::tcslcpy(wsz, inFile.c_str());

			// Load shortcut
			hres = ppf->Load(wsz, STGM_READ);

			if (SUCCEEDED(hres))
			{
				// find the path from that
				tchar_t buf[MAX_PATH_FULL] = {0};
				psl->GetPath(buf, MAX_PATH_FULL, nullptr, SLGP_UNCPRIORITY);
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
bool IsSlashOrColon(const tchar_t *pszChar, const tchar_t *begin)
{
		return (*pszChar == '/' || *pszChar == ':' || *pszChar == '\\');
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
	const tchar_t *pszChar = pathLeft.c_str() + pathLeft.length();
	const tchar_t *pend = pszChar;
	const tchar_t *extptr = 0;
	bool ext = false;

	while (pathLeft.c_str() < --pszChar)
	{
		if (*pszChar == '.')
		{
			if (!ext)
			{
				if (pExt != nullptr)
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

			if (pPath != nullptr)
			{
				// Grab directory (omit trailing slash)
				size_t len = pszChar - pathLeft.c_str();
				if (*pszChar == ':')
					++len; // Keep trailing colon ( eg, C:filename.txt)
				*pPath = pathLeft;
				pPath->erase(len); // Cut rest of path
			}

			if (pFile != nullptr)
			{
				// Grab file
				*pFile = pszChar + 1;
			}

			goto endSplit;
		}
	}

	// Never found a delimiter
	if (pFile != nullptr)
	{
		*pFile = pathLeft;
	}

endSplit:
	// if both filename & extension requested, remove extension from filename

	if (pFile != nullptr && pExt != nullptr && extptr != nullptr)
	{
		size_t extlen = pend - extptr;
		pFile->erase(pFile->length() - extlen);
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

bool IsURL(const String& path)
{
	size_t pos = path.find(':');
	return (pos != String::npos && pos > 1);
}

bool IsURLorCLSID(const String& path)
{
	return IsURL(path) || path.find(_T("::{")) != String::npos;
}

bool isFileURL(const String& path)
{
	return UrlIsFileUrl(path.c_str());
}

String FromURL(const String& url)
{
	std::vector<tchar_t> path((std::max)(size_t(MAX_PATH), url.length() + 1));
	DWORD size = static_cast<DWORD>(path.size());
	PathCreateFromUrl(url.c_str(), path.data(), &size, 0);
	return path.data();
}

bool IsDecendant(const String& path, const String& ancestor)
{
	return path.length() > ancestor.length() && 
		   strutils::compare_nocase(String(path.c_str(), path.c_str() + ancestor.length()), ancestor) == 0;
}

static void replace_char(tchar_t *s, int target, int repl)
{
	tchar_t *p;
	for (p=s; *p != _T('\0'); p = tc::tcsinc(p))
		if (*p == target)
			*p = (tchar_t)repl;
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

/**
 * @brief Return whether whether the given name can be used as a filename or directory name.
 * This function performs a test PathGetCharType() on each character in the specified name.
 * @param [in] name Filename or directory name to check.
 * @return true if the given name can be used as a filename or directory name.
 */
bool IsValidName(const String& name)
{
	for (String::const_iterator it = name.begin(); it != name.end(); ++it)
		if (!(PathGetCharType(*it) & GCT_LFNCHAR))
			return false;

	return true;
}

}
