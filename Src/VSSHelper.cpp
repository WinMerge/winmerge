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

CString VSSHelper::GetProjectBase()
{
	return m_strVssProjectBase;
}

void VSSHelper::SetProjectBase(CString strPath)
{
	m_strVssProjectBase = strPath;
	m_strVssProjectBase.Replace('/', '\\');

	// Check if m_strVssProjectBase has leading $\\, if not put them in:
	if (m_strVssProjectBase[0] != '$' && m_strVssProjectBase[1] != '\\')
		m_strVssProjectBase.Insert(0, _T("$\\"));
	
	if (m_strVssProjectBase[m_strVssProjectBase.GetLength() - 1] == '\\')
		m_strVssProjectBase.Delete(m_strVssProjectBase.GetLength() - 1, 1);
}

BOOL VSSHelper::ReLinkVCProj(CString strSavePath, CString * psError)
{
	const UINT nBufferSize = 1024;
	static TCHAR buffer[nBufferSize];
	static TCHAR buffer1[nBufferSize];
	static TCHAR buffer2[nBufferSize];
	TCHAR tempPath[MAX_PATH] = {0};
	TCHAR tempFile[MAX_PATH] = {0};
	CString spath;
	BOOL bVCPROJ = FALSE;

	if (::GetTempPath(MAX_PATH, tempPath))
	{
		if (!::GetTempFileName(tempPath, _T ("_LT"), 0, tempFile))
		{
			LogErrorString(_T("CMainFrame::ReLinkVCProj() - couldn't get tempfile!"));
			return FALSE;
		}
	}
	else
	{
		LogErrorString(_T("CMainFrame::ReLinkVCProj() - couldn't get temppath!"));
		return FALSE;
	}

	CString strExt;
	SplitFilename(strSavePath, NULL, NULL, &strExt);
	if (strExt.CompareNoCase(_T("vcproj")) == 0 || strExt.CompareNoCase(_T("sln")))
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
	
		ZeroMemory(&buffer2, nBufferSize * sizeof(TCHAR));
		while (GetWordFile(hfile, buffer, charset))
		{
			WriteFile(tfile, buffer, _tcslen(buffer), &numwritten, NULL);
			if (bVCPROJ)
			{
				if (!_tcscmp(buffer, _T("SccProjectName")))
				{
					//nab the equals sign
					GetWordFile(hfile, buffer, _T("="));
					WriteFile(tfile, buffer, _tcslen(buffer), &numwritten, NULL);
					CString stemp = _T("\"&quot;") + m_strVssProjectFull + 
						_T("&quot;");
					WriteFile(tfile, stemp, stemp.GetLength(),
						&numwritten, NULL);
					GetWordFile(hfile, buffer, _T(",\n"));//for junking
					GetWordFile(hfile, buffer, _T(",\n"));//get the next delimiter
					if (!_tcscmp(buffer, _T("\n")))
					{
						WriteFile(tfile, _T("\""), 1, &numwritten, NULL);						
					}
					WriteFile(tfile, buffer, _tcslen(buffer), &numwritten, NULL);
				}
			}
			else
			{//sln file
				//find sccprojectname inside this string
				if (_tcsstr(buffer, _T("SccProjectUniqueName")) == buffer)
				{
					//nab until next no space, and no =
					GetWordFile(hfile, buffer, _T(" ="));
					WriteFile(tfile, buffer, _tcslen(buffer), &numwritten, NULL);
					//nab word
					GetWordFile(hfile, buffer, _T("\\\n."));
					while (!_tcsstr(buffer, _T(".")))
					{						
						if (buffer[0] != '\\')
						{
							_stprintf(buffer1, _T("%s/%s"), buffer2, buffer);//put append word to file
							_tcscpy(buffer2,buffer1);
						}
						WriteFile(tfile, buffer, _tcslen(buffer), &numwritten, NULL);
						GetWordFile(hfile, buffer, _T("\\\n."));
					}
					WriteFile(tfile, buffer, _tcslen(buffer), &numwritten, NULL);
				}
				else if (_tcsstr(buffer, _T("SccProjectName")) == buffer)
				{
					
					//buffer2 appends
					CString capp;
					if (buffer2[0] != '\\' && !_tcsstr(buffer2, _T(".")))
					{
						//write out \\u0020s for every space in buffer2
						ZeroMemory(&buffer1, nBufferSize * sizeof(TCHAR));
						ZeroMemory(&buffer, nBufferSize * sizeof(TCHAR));
						for (TCHAR * pc = buffer2; *pc; pc++)
						{
							if (*pc == ' ')//insert \\u0020
							{
								_stprintf(buffer, _T("%s\\u0020"), buffer1);
								_tcscpy(buffer1, buffer);
							}
							else
							{
								int slb2 = _tcslen(buffer1);
								buffer1[slb2] = *pc;
								buffer1[slb2+1] = '\0';
							}
						}
						_tcslwr(buffer1);
						capp = buffer1;
						
						//nab until the no space, and no =
						GetWordFile(hfile, buffer, _T(" ="));
						WriteFile(tfile, buffer, _tcslen(buffer), &numwritten, NULL);
						CString stemp =  _T("\\u0022") + m_strVssProjectFull + capp + _T("\\u0022");
						WriteFile(tfile, stemp, stemp.GetLength(),
							&numwritten, NULL);
						
						//nab until the first backslash
						GetWordFile(hfile, buffer, _T(","));
						ZeroMemory(&buffer2, nBufferSize * sizeof(TCHAR));						
					}
				}
			}
		}
		CloseHandle(hfile);
		CloseHandle(tfile);
		if (!CopyFile(tempFile, strSavePath, FALSE))
		{
			*psError = GetSysError(GetLastError());
			DeleteFile(tempFile);
			return FALSE;
		}
		else
		{
			DeleteFile(tempFile);
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
 * If the next char in the file to be read is one of the characters inside the delimiter,
 * then the word returned will be a word consisting only of delimiters.
 * 
 * @note pfile is not incremented past the word returned
 */
BOOL VSSHelper::GetWordFile(HANDLE pfile, TCHAR * buffer, TCHAR * charset)
{
	TCHAR cbuffer[1024];
	TCHAR ctemp = '\0';
	TCHAR * pcharset;
	int buffercount = 0;
	DWORD numread = sizeof(ctemp);
	BOOL delimword = FALSE;
	BOOL FirstRead = FALSE;
	BOOL delimMatch = FALSE;

	ASSERT(pfile != NULL && pfile != INVALID_HANDLE_VALUE);
	ZeroMemory(&cbuffer, sizeof(cbuffer));
	
	while (numread == sizeof(ctemp) && buffercount < sizeof(cbuffer))
	{
		if (ReadFile(pfile, (LPVOID)&ctemp, sizeof(ctemp), &numread, NULL) == TRUE)
		{
			//first read:
			if (!FirstRead && charset)
			{
				for (pcharset = charset; *pcharset; pcharset++)
				{
					if (ctemp == *pcharset)
						break;
				}
				if (*pcharset != NULL)//means that cbuffer[0] is a delimiter character
					delimword = TRUE;
				FirstRead = TRUE;
			}

			if (numread == sizeof(ctemp))
			{
				if (!charset)
				{
					if (ctemp != ' ' && ctemp != '\n' && ctemp != '\t' && ctemp != '\r')
					{
						cbuffer[buffercount] = ctemp;
						buffercount++;
					}
					else
					{
						SetFilePointer(pfile,-1,NULL,FILE_CURRENT);
						break;
					}
				}
				else if (delimword == FALSE)
				{
					for (pcharset = charset;*pcharset;pcharset++)
					{						
						//if next char is equal to a delimiter or we want delimwords stop the adding
						if (ctemp == *pcharset)
						{
							SetFilePointer(pfile,-1,NULL,FILE_CURRENT);
							break;
						}
					}
					if (*pcharset == NULL)
					{
						cbuffer[buffercount] = ctemp;
						buffercount++;
					}
					else
						break;
				}
				else if (delimword == TRUE)
				{
					delimMatch = FALSE;
					for (pcharset = charset;*pcharset;pcharset++)
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
						cbuffer[buffercount] = ctemp;
						buffercount++;
					}
					else
					{
						SetFilePointer(pfile,-1,NULL,FILE_CURRENT);
						break;
					}
				}
			}
		}
		else
		{
			DWORD err = GetLastError();
			return FALSE;
		}
	}
	_tcscpy(buffer, cbuffer);
	if (buffercount >= sizeof(cbuffer) || numread == 0)
		return FALSE;
	return TRUE;
}
