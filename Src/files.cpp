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
 * @brief Reads different EOL formats and returns length of EOL
 * @note This function is safe: it first checks that there are unread bytes,
 * @note so that we do not read past EOF
 */
int files_readEOL(TBYTE *lpLineEnd, DWORD bytesLeft, BOOL bEOLSensitive)
{
	int eolBytes = 0;
	
	// EOL sensitive - ignore '\r' if '\r\n'
	if (bEOLSensitive)
	{
		// If >1 bytes left, there can be '\n' too
		if (*lpLineEnd == '\r' && bytesLeft > 1)
		{
			if (*(lpLineEnd+1) == '\n')
			{
				// '\r\n'
				//*(lpLineEnd+1) = '\0';
				eolBytes = 2;
			}
			else
			{
				//*(lpLineEnd) = '\0';
				eolBytes = 1;
			}
		}
		else
		{
			//*(lpLineEnd) = '\0';
			eolBytes = 1;
		}
	}
	else
	{
		if (bytesLeft > 1)
		{
			if ( (*lpLineEnd == '\r') && *(lpLineEnd+1) == '\n')
				eolBytes = 2;
			else
				eolBytes = 1;
		}
		else
			eolBytes = 1;

		//*(lpLineEnd) = '\0';
	}
	return eolBytes;
}

/**
 * @brief Checks Checks memory-mapped for binary data and counts lines
 * in textfile.
 * @note This does not work for UNICODE files
 * as WinMerge is not compiled UNICODE enabled
 */
int files_analyzeFile(MAPPEDFILEDATA *fileData, DWORD * dwLineCount)
{
	// Use unsigned type for binary compare
	TBYTE *lpByte = (TBYTE *)fileData->pMapBase;
	BOOL bBinary = FALSE;
	DWORD dwBytesRead = 0;
	*dwLineCount = 1;
	
	while ((dwBytesRead < fileData->dwSize - 1) && (bBinary == FALSE))
	{
		// Binary check
		if (*lpByte < 0x09)
		{
			bBinary = TRUE;
			lpByte++;
			dwBytesRead++;
		}
		else if (*lpByte == '\r' || *lpByte == '\n')
		{
			const int nEolBytes = files_readEOL(lpByte, fileData->dwSize - dwBytesRead, TRUE);
			lpByte += nEolBytes;
			dwBytesRead += nEolBytes;	// Skip EOL chars
			(*dwLineCount)++;
		}
		else
		{
			lpByte++;
			dwBytesRead++;
		}
	}

	if (bBinary)
		return FRESULT_BINARY;
	else
		return FRESULT_OK;
}