#include "stdafx.h"
#include "files.h"

BOOL files_openFileMapped(MAPPEDFILEDATA *fileData)
{
	DWORD dwProtectFlag = 0;
	DWORD dwMapAccess = 0;
	DWORD dwOpenAccess = 0;
	DWORD dwFileSizeHigh = 0;
	BOOL bSuccess = TRUE;

	if (fileData->bWritable)
	{
		dwProtectFlag = PAGE_READWRITE;
		dwMapAccess = FILE_MAP_ALL_ACCESS;
		dwOpenAccess = GENERIC_READ;
	}
	else
	{
		dwProtectFlag = PAGE_READONLY;
		dwMapAccess = FILE_MAP_READ;
		dwOpenAccess = GENERIC_READ | GENERIC_WRITE;
	}

	fileData->hFile = CreateFile(fileData->fileName,
		dwOpenAccess, 0, NULL, fileData->dwOpenFlags,
		FILE_ATTRIBUTE_NORMAL, NULL);

	if (fileData->hFile == INVALID_HANDLE_VALUE)
		bSuccess = FALSE;
	else
	{
		fileData->dwSize = GetFileSize(fileData->hFile,
			 &dwFileSizeHigh);
		if (fileData->dwSize == 0xFFFFFFFF || dwFileSizeHigh)
		{
			fileData->dwSize = 0;
			bSuccess = FALSE;
		}
	}
		
	if (bSuccess)
	{
		fileData->hMapping = CreateFileMapping(fileData->hFile,
				NULL, dwProtectFlag, 0, fileData->dwSize, NULL);
	}

	if (fileData->hMapping)
		fileData->pMapBase = MapViewOfFile(fileData->hMapping,
			dwMapAccess, 0, 0, 0);
	else
		bSuccess = FALSE;


	if (!fileData->pMapBase)
		bSuccess = FALSE;

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

// Reads different EOL formats and returns length of EOL
// This function is safe: it first checks that there are unread bytes,
// so that we do not read past EOF
int files_readEOL(TCHAR *lpLineEnd, DWORD bytesLeft, BOOL bEOLSensitive)
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


// Replace file with another file
// Note order of parameters!
// pszReplaced - this is the file to delete
// pszReplacement - this file will be renamed to pszReplaced
BOOL files_safeReplaceFile(LPCTSTR pszReplaced,	LPCTSTR pszReplacement)
{
	BOOL bSuccess = FALSE;
	if (pszReplaced && pszReplacement)
	{
		// Delete the file we are replacing
		// This fails if file does not exist, that's ok
		::DeleteFile(pszReplaced);

		// Rename remaining file to removed file
		// This (renaming file) should not fail,
		if (::MoveFile(pszReplacement, pszReplaced))
			bSuccess = TRUE;
	}
	return bSuccess;
}

BOOL files_safeWriteFile(HANDLE hFile, LPVOID lpBuf, DWORD dwLength)
{
	DWORD dwWrittenBytes = 0;
	if (WriteFile(hFile, lpBuf, dwLength, &dwWrittenBytes, NULL))
	{
		if (dwLength == dwWrittenBytes)
			return TRUE;
	}
	return FALSE;
}