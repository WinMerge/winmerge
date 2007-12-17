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
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include <sys/stat.h>
#include <sys/utime.h>
#include "UnicodeString.h"
#include "DiffFileInfo.h"
#include "files.h"
#include "paths.h"
#include "unicoder.h"

/**
 * @brief Open file as memory-mapped file.
 * @param [in,out] fileData Memory-mapped file's info.
 * @return TRUE if opening succeeded, FALSE otherwise.
 */
BOOL files_openFileMapped(MAPPEDFILEDATA *fileData)
{
	DWORD dwProtectFlag = 0;
	DWORD dwMapAccess = 0;
	DWORD dwOpenAccess = 0;
	DWORD dwFileSizeHigh = 0;
	DWORD dwSharedMode = FILE_SHARE_READ;
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
		if (fileData->dwSize == 0 && dwFileSizeHigh == 0)
			// Empty file (but should be accepted anyway)
			return bSuccess;

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
 * @brief Close memory-mapped file.
 * @param [in, out] fileData Memory-mapped file's info.
 * @param [in] newSize New size for the file.
 * @param [in] flush Flush buffers before closing the file.
 * @return TRUE if closing succeeded without errors, FALSE otherwise.
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
 * @brief Checks if file is read-only on disk.
 * Optionally returns also if file exists.
 * @param [in] file Full path to file to check.
 * @param [in, out] fileExists If non-NULL, function returns if file exists.
 * @return TRUE if file is read-only, FALSE otherwise.
 */
BOOL files_isFileReadOnly(const CString &file, BOOL *fileExists /*=NULL*/)
{
	struct _stati64 fstats = {0};
	BOOL bReadOnly = FALSE;
	BOOL bExists = FALSE;

	if (_tstati64(file, &fstats) == 0)
	{
		bExists = TRUE;

		if ((fstats.st_mode & _S_IWRITE) == 0)
			bReadOnly = TRUE;
	}
	else
		bExists = FALSE;
	
	if (fileExists != NULL)
		*fileExists = bExists;

	return bReadOnly;
}

/**
 * @brief Update file's modification time.
 * @param [in] info Contains filename, path and file times to update.
 */
void files_UpdateFileTime(const DiffFileInfo & info)
{
	String path = paths_ConcatPath(info.path, info.filename);
	_utimbuf times = {0};

	times.modtime = info.mtime;
	_tutime(path.c_str(), &times);
}
