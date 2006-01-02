/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or (at
//    your option) any later version.
//    
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  VSSHelper.cpp
 *
 * @brief Implementation file for VSSHelper class
 */
// RCS ID line follows -- this is updated by CVS
// $Id$


#include "stdafx.h"
#include "VSSHelper.h"
#include "coretools.h"
#include "paths.h"

CString VSSHelper::GetProjectBase()
{
	return m_strVssProjectBase;
}

BOOL VSSHelper::SetProjectBase(CString strPath)
{
	if (strPath.GetLength() < 2)
		return FALSE;

	m_strVssProjectBase = strPath;
	m_strVssProjectBase.Replace('/', '\\');

	// Check if m_strVssProjectBase has leading $\\, if not put them in:
	if (m_strVssProjectBase[0] != '$' && m_strVssProjectBase[1] != '\\')
		m_strVssProjectBase.Insert(0, _T("$\\"));
	
	if (m_strVssProjectBase[m_strVssProjectBase.GetLength() - 1] == '\\')
		m_strVssProjectBase.Delete(m_strVssProjectBase.GetLength() - 1, 1);
	return TRUE;
}

BOOL VSSHelper::ReLinkVCProj(CString strSavePath, CString * psError)
{
	const UINT nBufferSize = 1024;
	TCHAR buffer[nBufferSize] = {0};
	CString spath;
	BOOL bVCPROJ = FALSE;

	int nerr;
	CString tempPath = paths_GetTempPath(&nerr);
	if (tempPath.IsEmpty())
	{
		LogErrorString(Fmt(_T("CMainFrame::ReLinkVCProj() - couldn't get temppath: %s")
			, GetSysError(nerr)));
		return FALSE;
	}
	CString tempFile = paths_GetTempFileName(tempPath, _T("_LT"), &nerr);
	if (tempFile.IsEmpty())
	{
		LogErrorString(Fmt(_T("CMainFrame::ReLinkVCProj() - couldn't get tempfile: %s")
			, GetSysError(nerr)));
		return FALSE;
	}

	CString strExt;
	SplitFilename(strSavePath, NULL, NULL, &strExt);
	if (strExt.CompareNoCase(_T("vcproj")) == 0 || strExt.CompareNoCase(_T("sln")) == 0)
	{
		GetFullVSSPath(strSavePath, bVCPROJ);

		HANDLE hfile;
		HANDLE tfile;
		SetFileAttributes(strSavePath, FILE_ATTRIBUTE_NORMAL);
		
		hfile = CreateFile(strSavePath,
                GENERIC_ALL,              // open for writing
                FILE_SHARE_READ,           // share for reading 
                NULL,                      // no security 
                OPEN_EXISTING,             // existing file only 
                FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,     // normal file 
                NULL);
		tfile = CreateFile(tempFile,
                GENERIC_ALL,              // open for writing
                FILE_SHARE_READ,           // share for reading 
                NULL,                      // no security 
                CREATE_ALWAYS,             // existing file only 
                FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,     // normal file 
                NULL);  
		
		if (hfile == INVALID_HANDLE_VALUE || tfile == INVALID_HANDLE_VALUE)
		{
			CString msg;
			if (hfile == INVALID_HANDLE_VALUE)
			{
				msg.Format(_T("CMainFrame::ReLinkVCProj() ")
					_T("- failed to open file: %s"), strSavePath);
				LogErrorString(msg);
				AfxFormatString2(msg, IDS_ERROR_FILEOPEN,
						GetSysError(GetLastError()), strSavePath);
				*psError = msg;
			}
			if (tfile == INVALID_HANDLE_VALUE)
			{
				msg.Format(_T("CMainFrame::ReLinkVCProj() ")
					_T("- failed to open temporary file: %s"), tempFile);
				LogErrorString(msg);
				AfxFormatString2(msg, IDS_ERROR_FILEOPEN,
						GetSysError(GetLastError()), tempFile);
				*psError = msg;
			}
			return FALSE;
		}

		static TCHAR charset[] = _T(" \t\n\r=");
		DWORD numwritten = 0;
		BOOL succeed = TRUE;
	
		while (succeed && GetWordFromFile(hfile, buffer, nBufferSize, charset))
		{
			if (!WriteFile(tfile, buffer, _tcslen(buffer), &numwritten, NULL))
				succeed = FALSE;
			if (bVCPROJ)
			{
				if (_tcscmp(buffer, _T("SccProjectName")) == 0)
				{
					if (!GetVCProjName(hfile, tfile))
						succeed = FALSE;
				}
			}
			else
			{//sln file
				//find sccprojectname inside this string
				if (_tcsstr(buffer, _T("SccProjectUniqueName")) == buffer)
				{
					if (!GetSLNProjUniqueName(hfile, tfile, buffer))
						succeed = FALSE;
				}
				else if (_tcsstr(buffer, _T("SccProjectName")) == buffer)
				{
					if (!GetSLNProjName(hfile, tfile, buffer))
						succeed = FALSE;
				}
			}
		}
		CloseHandle(hfile);
		CloseHandle(tfile);

		if (succeed)
		{
			if (!CopyFile(tempFile, strSavePath, FALSE))
			{
				*psError = GetSysError(GetLastError());
				DeleteFile(tempFile);
				return FALSE;
			}
			else
				DeleteFile(tempFile);
		}
		else
		{
			CString msg;
			AfxFormatString2(msg, IDS_ERROR_FILEOPEN,
					strSavePath, GetSysError(GetLastError()));
			*psError = msg;
			return FALSE;
		}
	}
	return TRUE;
}

void VSSHelper::GetFullVSSPath(CString strSavePath, BOOL & bVCProj)
{
	CString strExt;
	CString spath;

	SplitFilename(strSavePath, NULL, NULL, &strExt);
	if (strExt.CompareNoCase(_T("vcproj")))
		bVCProj = TRUE;
	SplitFilename(strSavePath, &spath, NULL, NULL);

	strSavePath.Replace('/', '\\');
	m_strVssProjectBase.Replace('/', '\\');

	//check if m_strVssProjectBase has leading $\\, if not put them in:
	if (m_strVssProjectBase[0] != '$' && m_strVssProjectBase[1] != '\\')
		m_strVssProjectBase.Insert(0, _T("$\\"));

	strSavePath.MakeLower();
	m_strVssProjectBase.MakeLower();

	//take out last '\\'
	int nLen = m_strVssProjectBase.GetLength();
	if (m_strVssProjectBase[nLen - 1] == '\\')
		m_strVssProjectBase.Delete(nLen - 1, 1);

	CString strSearch = m_strVssProjectBase.Mid(2); // Don't compare first 2
	int index = strSavePath.Find(strSearch); //Search for project base path
	if (index > -1)
	{
		index++;
		m_strVssProjectFull = strSavePath.Mid(index + strSearch.GetLength());
		if (m_strVssProjectFull[0] == ':')
			m_strVssProjectFull.Delete(0, 2);
	}

	SplitFilename(m_strVssProjectFull, &spath, NULL, NULL);
	if (m_strVssProjectBase[m_strVssProjectBase.GetLength() - 1] != '\\')
		m_strVssProjectBase += "\\";

	m_strVssProjectFull = m_strVssProjectBase + spath;

	//if sln file, we need to replace ' '  with _T("\\u0020")
	if (!bVCProj)
		m_strVssProjectFull.Replace( _T(" "), _T("\\u0020"));
}

/**
 * @brief Reads words from a file deliminated by charset
 *
 * Reads words from a file deliminated by charset with one slight twist.
 * If the next char in the file to be read is one of the characters inside
 * the delimiter, then the word returned will be a word consisting only
 * of delimiters.
 * @param [in] pfile Opened handle to file
 * @param [in] buffer pointer to buffer read data is put
 * @param [in] dwBufferSize size of buffer
 * @param [in] charset pointer to string containing delimiter chars
 */
BOOL VSSHelper::GetWordFromFile(HANDLE pfile, TCHAR * buffer,
		DWORD dwBufferSize, TCHAR * charset)
{
	TCHAR buf[1024] = {0};
	const DWORD bytesToRead = sizeof(buf);
	DWORD bytesRead = 0;

	if (ReadFile(pfile, (LPVOID)buf, bytesToRead, &bytesRead, NULL))
	{
		int charsRead = GetWordFromBuffer(buf, bytesRead, buffer,
			dwBufferSize, charset);
		if (charsRead > 0)
			SetFilePointer(pfile, -1, NULL, FILE_CURRENT);
	}
	else
		return FALSE;

	return TRUE;
}

int VSSHelper::GetWordFromBuffer(TCHAR *inBuffer, DWORD dwInBufferSize,
		TCHAR * outBuffer, DWORD dwOutBufferSize, TCHAR * charset)
{
	TCHAR ctemp = '\0';
	TCHAR * pcharset = NULL;
	UINT buffercount = 0;
	DWORD numread = sizeof(ctemp);
	BOOL delimword = FALSE;
	BOOL firstRead = FALSE; // First char read ?
	BOOL delimMatch = FALSE;

	ASSERT(inBuffer != NULL && outBuffer != NULL);

	while (buffercount < dwInBufferSize && buffercount < dwOutBufferSize)
	{
		ctemp = *inBuffer;
		if (!firstRead && charset)
		{
			for (pcharset = charset; *pcharset; pcharset++)
			{
				if (ctemp == *pcharset)
					break;
			}
			if (*pcharset != NULL) // Means that cbuffer[0] is a delimiter character
				delimword = TRUE;
			firstRead = TRUE;
		}

		if (!charset)
		{
			if (ctemp != ' ' && ctemp != '\n' && ctemp != '\t' && ctemp != '\r')
			{
				*outBuffer = ctemp;
				buffercount++;
			}
			else
				break;
		}
		else if (delimword == FALSE)
		{
			for (pcharset = charset; *pcharset; pcharset++)
			{
				//if next char is equal to a delimiter or we want delimwords stop the adding
				if (ctemp == *pcharset)
					break;
			}
			if (*pcharset == NULL)
			{
				*outBuffer = ctemp;
				buffercount++;
			}
			else
				break;
		}
		else if (delimword == TRUE)
		{
			delimMatch = FALSE;
			for (pcharset = charset; *pcharset; pcharset++)
			{						
				//if next char is equal to a delimiter or we want delimwords stop the adding
				if (ctemp == *pcharset)
				{
					delimMatch = TRUE;
					break;
				}
			}
			if (delimMatch == TRUE)
			{
				*outBuffer = ctemp;
				buffercount++;
			}
			else
				break;
		}
	
		inBuffer += sizeof(TCHAR);
	}
	if (buffercount >= dwOutBufferSize || numread == 0)
		return 0;
	return buffercount;
}

BOOL VSSHelper::GetVCProjName(HANDLE hFile, HANDLE tFile)
{
	TCHAR buffer[1024] = {0};
	DWORD dwNumWritten = 0;
	
	ASSERT(hFile != NULL && hFile != INVALID_HANDLE_VALUE &&
		tFile != NULL && tFile != INVALID_HANDLE_VALUE);

	//nab the equals sign
	if (!GetWordFromFile(hFile, buffer, countof(buffer), _T("=")))
		return FALSE;
	if (!WriteFile(tFile, buffer, _tcslen(buffer),
			&dwNumWritten, NULL))
		return FALSE;

	CString stemp = _T("\"&quot;") + m_strVssProjectFull + 
		_T("&quot;");
	if (!WriteFile(tFile, stemp, stemp.GetLength(),
			&dwNumWritten, NULL))
		return FALSE;

	if (!GetWordFromFile(hFile, buffer, countof(buffer), _T(",\n"))) //for junking
		return FALSE;
	if (!GetWordFromFile(hFile, buffer, countof(buffer), _T(",\n"))) //get the next delimiter
		return FALSE;

	if (!_tcscmp(buffer, _T("\n")))
	{
		if (!WriteFile(tFile, _T("\""), 1, &dwNumWritten, NULL))
			return FALSE;
	}
	if (!WriteFile(tFile, buffer, _tcslen(buffer), &dwNumWritten, NULL))
		return FALSE;

	return TRUE;
}

BOOL VSSHelper::GetSLNProjUniqueName(HANDLE hFile, HANDLE tFile, TCHAR * buf)
{
	TCHAR buffer[1024] = {0};
	DWORD dwNumWritten = 0;

	ASSERT(hFile != NULL && hFile != INVALID_HANDLE_VALUE &&
		tFile != NULL && tFile != INVALID_HANDLE_VALUE);
	ASSERT(buf != NULL);

	//nab until next no space, and no =
	if (!GetWordFromFile(hFile, buffer, countof(buffer), _T(" =")))
		return FALSE;
	if (!WriteFile(tFile, buffer, _tcslen(buffer), &dwNumWritten, NULL))
		return FALSE;
	//nab word
	if (!GetWordFromFile(hFile, buffer, countof(buffer), _T("\\\n.")))
		return FALSE;
	while (!_tcsstr(buffer, _T(".")))
	{						
		if (buffer[0] != '\\')
			_tcsncat(buf, buffer, _tcslen(buffer));

		if (!WriteFile(tFile, buffer, _tcslen(buffer), &dwNumWritten, NULL))
			return FALSE;
		if (!GetWordFromFile(hFile, buffer, countof(buffer), _T("\\\n.")))
			return FALSE;
	}
	if (!WriteFile(tFile, buffer, _tcslen(buffer), &dwNumWritten, NULL))
		return FALSE;

	return TRUE;
}

BOOL VSSHelper::GetSLNProjName(HANDLE hFile, HANDLE tFile, TCHAR * buf)
{
	TCHAR buffer[1024] = {0};
	DWORD dwNumWritten = 0;

	ASSERT(hFile != NULL && hFile != INVALID_HANDLE_VALUE &&
		tFile != NULL && tFile != INVALID_HANDLE_VALUE);
	ASSERT(buf != NULL);
	
	CString capp;
	if (*buf != '\\' && !_tcsstr(buf, _T(".")))
	{
		//write out \\u0020s for every space in buffer2
		for (TCHAR * pc = buf; *pc; pc++)
		{
			if (*pc == ' ') //insert \\u0020
				capp += _T("\\u0020");
			else
				capp += *pc;
		}
		capp.MakeLower();

		//nab until the no space, and no =
		if (!GetWordFromFile(hFile, buffer, countof(buffer), _T(" =")))
			return FALSE;
		if (!WriteFile(tFile, buffer, _tcslen(buffer), &dwNumWritten, NULL))
			return FALSE;
		CString stemp = _T("\\u0022") + m_strVssProjectFull + capp + _T("\\u0022");
		if (!WriteFile(tFile, stemp, stemp.GetLength(),
				&dwNumWritten, NULL))
			return FALSE;
		
		//nab until the first backslash
		if (!GetWordFromFile(hFile, buffer, countof(buffer), _T(",")))
			return FALSE;
	}
	return TRUE;
}
