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
 * @file  files.cpp
 *
 * @brief Code file routines
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "files.h"
#include "unicoder.h"

/**
 * @brief Open file as memory-mapped file
 */
BOOL files_openFileMapped(MAPPEDFILEDATA *fileData)
{
	DWORD dwProtectFlag = 0;
	DWORD dwMapAccess = 0;
	DWORD dwOpenAccess = 0;
	DWORD dwFileSizeHigh = 0;
	DWORD dwSharedMode = 0; // exclusive
	HANDLE hTemplateFile = NULL; // for creating new file
	BOOL bSuccess = TRUE;

	if (fileData->bWritable)
	{
		dwProtectFlag = PAGE_READWRITE;
		dwMapAccess = FILE_MAP_ALL_ACCESS;
		dwOpenAccess = GENERIC_READ | GENERIC_WRITE;
	}
	else
	{
		dwProtectFlag = PAGE_READONLY;
		dwMapAccess = FILE_MAP_READ;
		dwOpenAccess = GENERIC_READ;
	}

	fileData->hFile = CreateFile(fileData->fileName,
		dwOpenAccess, dwSharedMode, NULL, fileData->dwOpenFlags,
		FILE_ATTRIBUTE_NORMAL, hTemplateFile);

	if (fileData->hFile == INVALID_HANDLE_VALUE)
	{
		bSuccess = FALSE;
		LogErrorString(Fmt(_T("CreateFile(%s) failed in files_openFileMapped: %s")
			, fileData->fileName, GetSysError(GetLastError())));
	}
	else
	{
		if (fileData->dwSize == 0)
		{
			fileData->dwSize = GetFileSize(fileData->hFile,
				 &dwFileSizeHigh);
			if (fileData->dwSize == 0xFFFFFFFF || dwFileSizeHigh)
			{
				fileData->dwSize = 0;
				bSuccess = FALSE;
			}
		}
	}
		
	if (bSuccess)
	{
		fileData->hMapping = CreateFileMapping(fileData->hFile,
				NULL, dwProtectFlag, 0, fileData->dwSize, NULL);
		if (!fileData->hMapping)
		{
			bSuccess = FALSE;
			LogErrorString(Fmt(_T("CreateFileMapping(%s) failed: %s")
				, fileData->fileName, GetSysError(GetLastError())));
		}
		else
		{
			fileData->pMapBase = MapViewOfFile(fileData->hMapping,
				dwMapAccess, 0, 0, 0);
			if (!fileData->pMapBase)
			{
				bSuccess = FALSE;
				LogErrorString(Fmt(_T("MapViewOfFile(%s) failed: %s")
					, fileData->fileName, GetSysError(GetLastError())));
			}
		}
	}

	if (!bSuccess)
	{
		UnmapViewOfFile(fileData->pMapBase);
		fileData->pMapBase = NULL;
		CloseHandle(fileData->hMapping);
		fileData->hMapping = NULL;
		CloseHandle(fileData->hFile);
		fileData->hFile = NULL;
	}
	return bSuccess;
}

/**
 * @brief Close memory-mapped file
 */
BOOL files_closeFileMapped(MAPPEDFILEDATA *fileData, DWORD newSize, BOOL flush)
{
	BOOL bSuccess = TRUE;

	if (fileData->pMapBase)
	{
		UnmapViewOfFile(fileData->pMapBase);
		fileData->pMapBase = NULL;
	}

	if (fileData->hMapping)
	{
		CloseHandle( fileData->hMapping );
		fileData->hMapping = NULL;
	}

	if (newSize != 0xFFFFFFFF)
	{
		SetFilePointer(fileData->hFile, newSize, NULL, FILE_BEGIN);
		SetEndOfFile(fileData->hFile);
	}

	if (flush)
		FlushFileBuffers(fileData->hFile);

	if (fileData->hFile)
	{
		CloseHandle(fileData->hFile);
		fileData->hFile = NULL;
	}
	return bSuccess;
}

/**
 * @brief Loads line array from memory-mapped file. Checks for Unicode BOMs and for binary file.
 */
int files_loadLines(MAPPEDFILEDATA *fileData, ParsedTextFile * parsedTextFile)
{
	// Use unsigned type for binary compare
	byte *lpByte = (byte *)fileData->pMapBase;
	BOOL bBinary = FALSE;
	DWORD dwBytesRead = 0;
	parsedTextFile->lines.RemoveAll();
	textline newline;
	newline.start = 0;
	// Manually grow line array exponentially
	int arraysize = 500;
	int lineno = 0;


	parsedTextFile->lines.SetSize(500);
	
	// Check for Unicode BOM (byte order mark)
	// (We don't check for UCS-4 marks)
	// Set stats and start appropriately
	if (fileData->dwSize >= 2)
	{
		if (lpByte[0] == 0xFF && lpByte[1] == 0xFE)
		{
			parsedTextFile->codeset = ucr::UCS2LE;
			parsedTextFile->charsize = 2;
			newline.start = 2;
		}
		else if (lpByte[0] == 0xFE && lpByte[1] == 0xFF)
		{
			parsedTextFile->codeset = ucr::UCS2BE;
			parsedTextFile->charsize = 2;
			newline.start = 2;
		}
	}
	if (fileData->dwSize >=3)
	{
		if (lpByte[0] == 0xEF && lpByte[1] == 0xBB && lpByte[2] == 0xBF)
		{
			parsedTextFile->codeset = ucr::UTF8;
			newline.start = 3;
		}
	}
	// skip over any BOM found
	dwBytesRead = newline.start;
	lpByte += dwBytesRead;

	// loop through every character in file
	while ((dwBytesRead +(parsedTextFile->charsize-1) < fileData->dwSize) && (bBinary == FALSE))
	{
		UINT ch=0;
		UINT utf8len=0;
	// shortcut cases, when file encoding is our native TCHAR encoding
#ifdef _UNICODE
		if (parsedTextFile->codeset == ucr::UCS2LE)
		{
			wchar_t wch = *(wchar_t *)lpByte;
			newline.sline += wch;
			ch = wch;
		}
#else
		if (parsedTextFile->codeset == ucr::NONE)
		{
			ch = *lpByte;
			newline.sline += ch;
		}
#endif
	// otherwise we convert via Unicode
		else 
		{
			if (parsedTextFile->codeset == ucr::UTF8)
			{
				// check for end in middle of UTF-8 character
				// or outside of UCS-2 (len>4)
				utf8len = ucr::Utf8len_fromLeadByte(*lpByte);
				if (dwBytesRead + utf8len > fileData->dwSize || utf8len>4)
				{
					bBinary = TRUE;
					break;
				}
				// Handle bad UTF-8
				// (Convert bad bytes individually to '?'
				if (utf8len<1)
				{
					utf8len=1;
					ch = '?';
				}
				else
				{
					ch = ucr::GetUtf8Char(lpByte);
				}
			}
			else
			{
				ch = ucr::get_unicode_char(lpByte, (ucr::UNICODESET)parsedTextFile->codeset);
			}
			// convert from Unicode codepoint to TCHAR string
			// could be multicharacter if decomposition took place, for example
			CString sch = ucr::maketchar(ch, parsedTextFile->lossy);
			newline.sline += sch;
			if (sch.GetLength() == 1)
				ch = sch[0];
			else
				ch = 0;
		}
		// Binary check
		if (ch < 0x09)
		{
			bBinary = TRUE;
			break;
		}
		if (ch == '\r')
		{
			bool crlf = false;
			// check for crlf pair
			if (dwBytesRead + 2 * parsedTextFile->charsize - 1 < fileData->dwSize)
			{
				// For UTF-8, this ch will be wrong if character is non-ASCII
				// but we only check it against \n here, so it doesn't matter
				UINT ch = ucr::get_unicode_char(lpByte+parsedTextFile->charsize, (ucr::UNICODESET)parsedTextFile->codeset);
				if (ch == '\n')
				{
					crlf = true;
					newline.sline += (TCHAR)ch;
				}
			}
			if (crlf)
			{
				++parsedTextFile->crlfs;
				newline.eoltype = textline::EOL_CRLF;
				// advance an extra character to skip the following lf
				lpByte += parsedTextFile->charsize;
				dwBytesRead += parsedTextFile->charsize;
				newline.end = dwBytesRead;
			}
			else
			{
				++parsedTextFile->crs;
				newline.eoltype = textline::EOL_CR;
				newline.end = dwBytesRead;
			}
		}
		else if (ch == '\n')
		{
			++parsedTextFile->lfs;
			newline.eoltype = textline::EOL_LF;
			newline.end = dwBytesRead;
		}
		// always advance to next character
		if (parsedTextFile->codeset == ucr::UTF8)
		{
			lpByte += utf8len;
			dwBytesRead += utf8len;
		}
		else
		{
			lpByte += parsedTextFile->charsize;
			dwBytesRead += parsedTextFile->charsize;
		}
		if (newline.end >= 0)
		{
			// Manually grow line array exponentially
			if (lineno == arraysize)
			{
				arraysize *= 2;
				parsedTextFile->lines.SetSize(arraysize);
				
			}
			parsedTextFile->lines[lineno] = newline;
			++lineno;
			newline.start = dwBytesRead;
			newline.end = -1;
			newline.sline = _T("");
		}
	}

	// fix array size (due to our manual exponential growth
	parsedTextFile->lines.SetSize(lineno);

	if (bBinary)
	{
		return FRESULT_BINARY;
	}
	else
	{
		// we always have a line after the last EOL
		// if the file has some text after its last EOL, it goes into this line
		newline.end = dwBytesRead;
		newline.eoltype = textline::EOL_NONE;
		parsedTextFile->lines.Add(newline);

		return FRESULT_OK;
	}
}
