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
		sPath.SetAt(0, paths_GetCurrentDrive());
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
		sPath.SetAt(0, paths_GetCurrentDrive());
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

// get long name
CString paths_GetLongPath(const CString & sPath)
{
	int len = sPath.GetLength();
	// ensure it is not a root drive or a UNC path
	if (len < 3) return sPath;
	if (sPath[0]=='\\' && sPath[1]=='\\') return sPath;

	// Now get a working buffer and walk down each directory
	CString sBuffer=sPath; // original path
	sBuffer.Replace('/', '\\');
	CString sTemp; // used at each step to hold fully qualified short name
	CString sLong; // output

	if (sBuffer[0] == '\\')
	{
		// root directory, prepend current drive
		sTemp += paths_GetCurrentDrive();
		sBuffer = sTemp + ':' + sBuffer;
	}
	else if (sBuffer[1] == ':')
	{
		// looks like a fully qualified path
	}
	else
	{
		// treat as relative
		TCHAR curdir[_MAX_PATH];
		if (!GetCurrentDirectory(sizeof(curdir)/sizeof(curdir[0]), curdir))
			return sPath;
		sBuffer = (CString)curdir + '\\' + sBuffer;
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
		if (!end)
		{
			// on last component (probably filename)
			sTemp = sLong + '\\' + ptr;
			ptr = 0;
		}
		else
		{
			*end = 0;
			sTemp = sLong + _T("\\") + ptr;
			ptr = &end[1];
		}
		// (Couldn't get info for just the directory from CFindFile)
		WIN32_FIND_DATA ffd;
		HANDLE h = FindFirstFile(sTemp, &ffd);
		if (h == INVALID_HANDLE_VALUE)
		{
			sLong += '\\' + ptr;
			return sLong;
		}
		if (!sLong.IsEmpty())
			sLong += '\\';
		sLong += ffd.cFileName;
		FindClose(h);
	}
	return sLong;
}

TCHAR paths_GetCurrentDrive()
{
	TCHAR curdir[_MAX_PATH];
	if (!GetCurrentDirectory(sizeof(curdir)/sizeof(curdir[0]), curdir))
		return 'C';
	return curdir[0];
}

