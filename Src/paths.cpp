/** 
 * @file  paths.cpp
 *
 * @brief Path handling routines
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "paths.h"

bool IsSlash(TCHAR ch)
{
	return ch=='\\' || ch=='/';
}

bool EndsWithSlash(const CString & s)
{
	return !s.IsEmpty() && IsSlash(s[s.GetLength()-1]);
}

PATH_EXISTENCE paths_DoesPathExist(LPCTSTR szPath)
{
	if (!szPath || !szPath[0]) return DOES_NOT_EXIST;

	CFileStatus status;
	CString sPath = szPath;

	// handle "\"
	if (sPath.GetLength()==1 && IsSlash(sPath[0]))
	{
		// Prefix current drive to "\"
		sPath.SetAt(0, paths_GetCurrentDriveUpper());
		sPath += _T(":\\");
	}

	// Only allow colon in 2nd char, when first char is an English letter
	// (This disallows urls)
	int colon = sPath.Find(':');
	if (colon>=0)
	{
		if (colon != 1)
			return DOES_NOT_EXIST;
		// handle "c:" or "c:\"
		if (sPath.GetLength() == 2 || (sPath.GetLength() == 3 && EndsWithSlash(sPath)))
		{
			// validate root directory
			if (!EndsWithSlash(sPath))
				sPath += '\\';
			CFileFind filefind;
			if (filefind.FindFile(sPath + _T("*.*")))
				return IS_EXISTING_DIR;
			else
				return DOES_NOT_EXIST;
		}
	}

	// strip trailing slashes (but for UNC, we need to know if there was one)
	BOOL bDir = EndsWithSlash(sPath);
	if (bDir)
		sPath = sPath.Left(sPath.GetLength()-1);

	// disallow multiple trailing slashes
	if (EndsWithSlash(sPath))
		return DOES_NOT_EXIST;

	// Check for UNC directories (CFile::GetStatus doesn't open them)
	if (bDir && sPath.Left(2) == _T("\\\\"))
	{
		// validate UNC directory
		CFileFind filefind;
		if (filefind.FindFile(sPath + _T("\\*.*")))
			return IS_EXISTING_DIR;
		else
			return DOES_NOT_EXIST;
	}

	// Now for normal handling

	if (!CFile::GetStatus(sPath, status))
		return DOES_NOT_EXIST;

	if (status.m_attribute & CFile::Attribute::directory)
		return IS_EXISTING_DIR;

	return IS_EXISTING_FILE;
}

// strip trailing slashes (except from root paths)
void paths_normalize(CString & sPath)
{
	int len = sPath.GetLength();
	if (!len) return;

	// prefix root with current drive
	if (sPath == _T("\\"))
	{
		// Prefix current drive to "\"
		sPath.SetAt(0, paths_GetCurrentDriveUpper());
		sPath += _T(":\\");
		return;
	}

	// Append slash to root directory lacking slash
	if (len == 2 && sPath[1] == ':')
	{
		sPath += '\\';
		return;
	}

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
	// ensure it is not a root drive or a UNC path
	if (len < 1) return sPath;
	if (sPath[0]=='\\' && len>1 && sPath[1]=='\\') return sPath;

	// Now get a working buffer and walk down each directory
	CString sBuffer=sPath; // original path
	sBuffer.Replace('/', '\\');
	CString sTemp; // used at each step to hold fully qualified short name
	CString sLong; // output

	if (sBuffer[0] == '\\')
	{
		// root directory, prepend current drive
		sTemp += paths_GetCurrentDriveUpper();
		sBuffer = sTemp + ':' + sBuffer;
	}
	else if (len>1 && sBuffer[1] == ':')
	{
		if (!_istalpha(sBuffer[0]))
			return sPath; // not a valid drive, give up
		if (len == 2 || sBuffer[2] != '\\')
		{
			// relative path
			TCHAR chdrv = sBuffer[0];
			if (_istlower(chdrv)) chdrv = _totupper(chdrv);
			if (chdrv == paths_GetCurrentDriveUpper())
			{
				// relative on current drive, so prepend current directory
				CString sTemp = paths_GetCurrentDirectory() + _T("\\") + sBuffer.Mid(2);
				sBuffer = sTemp;
			}
			else
			{
				// relative on other drive; don't know how to find current dir
				// so treat it as absolute
				CString sTemp = sBuffer.Left(2) + _T("\\") + sBuffer.Mid(2);
				sBuffer = sTemp;
			}
		}
		else
		{
			// looks like a fully qualified path
		}
	}
	else
	{
		// treat as relative
		CString curdir = paths_GetCurrentDirectory();
		sBuffer = curdir + '\\' + sBuffer;
	}
	LPTSTR ptr = sBuffer.GetBuffer(0), end;
	// skip over root slash
	end = _tcschr(ptr, '\\');
	if (!end) return sPath;
	*end = 0;
	sLong += ptr;
	ptr = &end[1];
	// now walk down each directory
	// using CFileFind to get its long name
	while (ptr)
	{
		end = _tcschr(ptr, '\\');
		// zero-terminate current component
		// (if we're at end, its already zero-terminated)
		if (end)
			*end = 0;
		sTemp = sLong + '\\' + ptr;
		// special handling for . and .. components
		if (0 == _tcscmp(ptr, _T(".")))
		{
			// advance to next component (or set ptr==0 to flag end)
			ptr = (end ? end+1 : 0);
			continue;
		}
		if (0 == _tcscmp(ptr, _T("..")))
		{
			// back up one component
			for (int i = sLong.GetLength()-1; i>=0 && sLong[i]!='\\' && sLong[i]!=':'; --i)
				;
			if (i==-1)
			{
				return _T("");
			}
			sLong = sLong.Left(i);
			// advance to next component (or set ptr==0 to flag end)
			ptr = (end ? end+1 : 0);
			continue;
		}

		// advance to next component (or set ptr==0 to flag end)
		ptr = (end ? end+1 : 0);

		// (Couldn't get info for just the directory from CFindFile)
		WIN32_FIND_DATA ffd;
		HANDLE h = FindFirstFile(sTemp, &ffd);
		if (h == INVALID_HANDLE_VALUE)
		{
			sLong += '\\';
			if (ptr)
				sLong += ptr;
			return sLong;
		}
		if (!sLong.IsEmpty())
			sLong += '\\';
		sLong += ffd.cFileName;
		if (dst == DIRSLASH && !ptr 
			&& (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			sLong += '\\';
		}
		FindClose(h);
	}
	return sLong;
}

CString paths_GetCurrentDirectory()
{
		TCHAR curdir[_MAX_PATH] = _T("");
		GetCurrentDirectory(sizeof(curdir)/sizeof(curdir[0]), curdir);
		return curdir;
}

TCHAR paths_GetCurrentDriveUpper()
{
	TCHAR curdir[_MAX_PATH];
	if (!GetCurrentDirectory(sizeof(curdir)/sizeof(curdir[0]), curdir))
		return 'C';
	if (_istascii(curdir[0]) && _istlower(curdir[0]))
		curdir[0] = _totupper(curdir[0]);
	return curdir[0];
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
		if (IsSlash(subpath[0]))
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
		if (IsSlash(subpath[0]))
		{
			return path + subpath;
		}
		else
		{
			return path + _T("\\") + subpath;
		}
	}
}
