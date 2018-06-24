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


#include "VSSHelper.h"
#include <windows.h>
#include <cassert>
#include "UnicodeString.h"
#include "paths.h"
#include "Environment.h"
#include "MergeApp.h"

String VSSHelper::GetProjectBase() const
{
	return m_strVssProjectBase;
}

bool VSSHelper::SetProjectBase(const String& strPath)
{
	if (strPath.size() < 2)
		return false;

	m_strVssProjectBase = strPath;
	strutils::replace(m_strVssProjectBase, _T("/"), _T("\\"));

	// Check if m_strVssProjectBase has leading $\\, if not put them in:
	if (m_strVssProjectBase[0] != '$' && m_strVssProjectBase[1] != '\\')
		m_strVssProjectBase.insert(0, _T("$\\"));
	
	if (paths::EndsWithSlash(m_strVssProjectBase))
		m_strVssProjectBase.resize(m_strVssProjectBase.size() - 1);
	return true;
}

bool VSSHelper::ReLinkVCProj(const String& strSavePath, String& sError)
{
	const UINT nBufferSize = 1024;

	String tempFile = env::GetTemporaryFileName(env::GetTemporaryPath(), _T("_LT"));
	if (tempFile.empty())
	{
		LogErrorString(_T("CMainFrame::ReLinkVCProj() - couldn't get tempfile"));
		return false;
	}

	String ext;
	paths::SplitFilename(strSavePath, NULL, NULL, &ext);
	String strExt = strutils::makelower(ext);
	if (strExt == _T("vcproj") || strExt == _T("sln"))
	{
		bool bVCPROJ = false;
		GetFullVSSPath(strSavePath, bVCPROJ);

		HANDLE hfile;
		HANDLE tfile;
		SetFileAttributes(strSavePath.c_str(), FILE_ATTRIBUTE_NORMAL);
		
		hfile = CreateFile(strSavePath.c_str(),
                GENERIC_ALL,              // open for writing
                FILE_SHARE_READ,           // share for reading 
                NULL,                      // no security 
                OPEN_EXISTING,             // existing file only 
                FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,     // normal file 
                NULL);
		tfile = CreateFile(tempFile.c_str(),
                GENERIC_ALL,              // open for writing
                FILE_SHARE_READ,           // share for reading 
                NULL,                      // no security 
                CREATE_ALWAYS,             // existing file only 
                FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,     // normal file 
                NULL);  
		
		if (hfile == INVALID_HANDLE_VALUE || tfile == INVALID_HANDLE_VALUE)
		{
			if (hfile == INVALID_HANDLE_VALUE)
			{
				sError = strutils::format(_T("CMainFrame::ReLinkVCProj() ")
					_T("- failed to open file: %s"), strSavePath.c_str());
				LogErrorString(sError);
				String errMsg = GetSysError(GetLastError());
				sError = strutils::format_string2(_("Cannot open file\n%1\n\n%2"), errMsg, strSavePath);
			}
			else
			{
				CloseHandle(hfile);
			}
			if (tfile == INVALID_HANDLE_VALUE)
			{
				sError = strutils::format(_T("CMainFrame::ReLinkVCProj() ")
					_T("- failed to open temporary file: %s"), tempFile.c_str());
				LogErrorString(sError);
				String errMsg = GetSysError(GetLastError());
				sError = strutils::format_string2(_("Cannot open file\n%1\n\n%2"), errMsg, strSavePath);
			}
			else
			{
				CloseHandle(tfile);
			}
			return false;
		}

		static TCHAR charset[] = _T(" \t\n\r=");
		TCHAR buffer[nBufferSize] = {0};
		DWORD numwritten = 0;
		bool succeed = true;
	
		while (succeed && GetWordFromFile(hfile, buffer, nBufferSize, charset))
		{
			if (!WriteFile(tfile, buffer, lstrlen(buffer), &numwritten, NULL))
				succeed = false;
			if (bVCPROJ)
			{
				if (_tcscmp(buffer, _T("SccProjectName")) == 0)
				{
					if (!GetVCProjName(hfile, tfile))
						succeed = false;
				}
			}
			else
			{//sln file
				//find sccprojectname inside this string
				if (_tcsstr(buffer, _T("SccProjectUniqueName")) == buffer)
				{
					if (!GetSLNProjUniqueName(hfile, tfile, buffer, nBufferSize))
						succeed = false;
				}
				else if (_tcsstr(buffer, _T("SccProjectName")) == buffer)
				{
					if (!GetSLNProjName(hfile, tfile, buffer))
						succeed = false;
				}
			}
		}
		CloseHandle(hfile);
		CloseHandle(tfile);

		if (succeed)
		{
			if (!CopyFile(tempFile.c_str(), strSavePath.c_str(), false))
			{
				sError = GetSysError(GetLastError()).c_str();
				DeleteFile(tempFile.c_str());
				return false;
			}
			else
				DeleteFile(tempFile.c_str());
		}
		else
		{
			String errMsg = GetSysError(GetLastError());
			sError = strutils::format_string2(_("Cannot open file\n%1\n\n%2"), errMsg, strSavePath);
			return false;
		}
	}
	return true;
}

void VSSHelper::GetFullVSSPath(const String& strSavePath, bool & bVCProj)
{
	String ext;
	String path;
	paths::SplitFilename(strSavePath, &path, NULL, &ext);
	String strExt = strutils::makelower(ext); 
	if (strExt == _T("vcproj"))
		bVCProj = true;

	String savepath(strSavePath);
	strutils::replace(savepath, _T("/"), _T("\\"));
	strutils::replace(m_strVssProjectBase, _T("/"), _T("\\"));

	//check if m_strVssProjectBase has leading $\\, if not put them in:
	if (m_strVssProjectBase[0] != '$' && m_strVssProjectBase[1] != '\\')
		m_strVssProjectBase.insert(0, _T("$\\"));

	savepath = strutils::makelower(savepath);
	m_strVssProjectBase = strutils::makelower(m_strVssProjectBase);

	//take out last '\\'
	size_t nLen = m_strVssProjectBase.size();
	if (paths::EndsWithSlash(m_strVssProjectBase))
		m_strVssProjectBase.resize(nLen - 1);

	String strSearch = m_strVssProjectBase.c_str() + 2; // Don't compare first 2
	size_t index = strSavePath.find(strSearch); //Search for project base path
	if (index != String::npos)
	{
		index++;
		m_strVssProjectFull = savepath.c_str() + index + strSearch.length();
		if (m_strVssProjectFull[0] == ':')
			m_strVssProjectFull.erase(0, 2);
	}

	paths::SplitFilename(m_strVssProjectFull, &path, NULL, NULL);
	m_strVssProjectBase = paths::AddTrailingSlash(m_strVssProjectBase);

	m_strVssProjectFull += m_strVssProjectBase + path;

	//if sln file, we need to replace ' '  with _T("\\u0020")
	if (!bVCProj)
		strutils::replace(m_strVssProjectFull, _T(" "), _T("\\u0020"));
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
bool VSSHelper::GetWordFromFile(HANDLE pfile, TCHAR * buffer,
		DWORD dwBufferSize, TCHAR * charset) const
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
		return false;

	return true;
}

int VSSHelper::GetWordFromBuffer(const TCHAR *inBuffer, DWORD dwInBufferSize,
		TCHAR * outBuffer, DWORD dwOutBufferSize, const TCHAR * charset) const
{
	TCHAR ctemp = '\0';
	const TCHAR * pcharset = NULL;
	UINT buffercount = 0;
	DWORD numread = sizeof(ctemp);
	bool delimword = false;
	bool firstRead = false; // First char read ?
	bool delimMatch = false;

	assert(inBuffer != NULL && outBuffer != NULL);

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
				delimword = true;
			firstRead = true;
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
		else if (delimword == false)
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
		else if (delimword == true)
		{
			delimMatch = false;
			for (pcharset = charset; *pcharset; pcharset++)
			{						
				//if next char is equal to a delimiter or we want delimwords stop the adding
				if (ctemp == *pcharset)
				{
					delimMatch = true;
					break;
				}
			}
			if (delimMatch == true)
			{
				*outBuffer = ctemp;
				buffercount++;
			}
			else
				break;
		}
	
		++inBuffer;
	}
	if (buffercount >= dwOutBufferSize || numread == 0)
		return 0;
	return buffercount;
}

bool VSSHelper::GetVCProjName(HANDLE hFile, HANDLE tFile) const
{
	TCHAR buffer[1024] = {0};
	DWORD dwNumWritten = 0;
	
	assert(hFile != NULL && hFile != INVALID_HANDLE_VALUE &&
		tFile != NULL && tFile != INVALID_HANDLE_VALUE);

	//nab the equals sign
	if (!GetWordFromFile(hFile, buffer, sizeof(buffer)/sizeof(buffer[0]), _T("=")))
		return false;
	if (!WriteFile(tFile, buffer, lstrlen(buffer),
			&dwNumWritten, NULL))
		return false;

	String stemp = _T("\"&quot;") + m_strVssProjectFull + 
		_T("&quot;");
	if (!WriteFile(tFile, stemp.c_str(), static_cast<DWORD>(stemp.size()),
			&dwNumWritten, NULL))
		return false;

	if (!GetWordFromFile(hFile, buffer, sizeof(buffer)/sizeof(buffer[0]), _T(",\n"))) //for junking
		return false;
	if (!GetWordFromFile(hFile, buffer, sizeof(buffer)/sizeof(buffer[0]), _T(",\n"))) //get the next delimiter
		return false;

	if (!_tcscmp(buffer, _T("\n")))
	{
		if (!WriteFile(tFile, _T("\""), 1, &dwNumWritten, NULL))
			return false;
	}
	if (!WriteFile(tFile, buffer, lstrlen(buffer), &dwNumWritten, NULL))
		return false;

	return true;
}

bool VSSHelper::GetSLNProjUniqueName(HANDLE hFile, HANDLE tFile, TCHAR * buf, size_t nBufSize) const
{
	TCHAR buffer[1024] = {0};
	DWORD dwNumWritten = 0;

	assert(hFile != NULL && hFile != INVALID_HANDLE_VALUE &&
		tFile != NULL && tFile != INVALID_HANDLE_VALUE);
	assert(buf != NULL);

	//nab until next no space, and no =
	if (!GetWordFromFile(hFile, buffer, sizeof(buffer)/sizeof(buffer[0]), _T(" =")))
		return false;
	if (!WriteFile(tFile, buffer, lstrlen(buffer), &dwNumWritten, NULL))
		return false;
	//nab word
	if (!GetWordFromFile(hFile, buffer, sizeof(buffer)/sizeof(buffer[0]), _T("\\\n.")))
		return false;
	while (!_tcsstr(buffer, _T(".")))
	{						
		if (buffer[0] != '\\')
			_tcsncat_s(buf, nBufSize, buffer, _tcslen(buffer));

		if (!WriteFile(tFile, buffer, lstrlen(buffer), &dwNumWritten, NULL))
			return false;
		if (!GetWordFromFile(hFile, buffer, sizeof(buffer)/sizeof(buffer[0]), _T("\\\n.")))
			return false;
	}
	if (!WriteFile(tFile, buffer, lstrlen(buffer), &dwNumWritten, NULL))
		return false;

	return true;
}

bool VSSHelper::GetSLNProjName(HANDLE hFile, HANDLE tFile, TCHAR * buf) const
{
	DWORD dwNumWritten = 0;

	assert(hFile != NULL && hFile != INVALID_HANDLE_VALUE &&
		tFile != NULL && tFile != INVALID_HANDLE_VALUE);
	assert(buf != NULL);
	
	String capp;
	if (*buf != '\\' && !_tcsstr(buf, _T(".")))
	{
		TCHAR buffer[1024] = {0};

		//write out \\u0020s for every space in buffer2
		for (TCHAR * pc = buf; *pc; pc++)
		{
			if (*pc == ' ') //insert \\u0020
				capp += _T("\\u0020");
			else
				capp += *pc;
		}
		capp = strutils::makelower(capp);

		//nab until the no space, and no =
		if (!GetWordFromFile(hFile, buffer, sizeof(buffer)/sizeof(buffer[0]), _T(" =")))
			return false;
		if (!WriteFile(tFile, buffer, lstrlen(buffer), &dwNumWritten, NULL))
			return false;
		String stemp = _T("\\u0022") + m_strVssProjectFull + capp + _T("\\u0022");
		if (!WriteFile(tFile, stemp.c_str(), static_cast<DWORD>(stemp.size()),
				&dwNumWritten, NULL))
			return false;
		
		//nab until the first backslash
		if (!GetWordFromFile(hFile, buffer, sizeof(buffer)/sizeof(buffer[0]), _T(",")))
			return false;
	}
	return true;
}
