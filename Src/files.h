#ifndef _FILES_H_INCLUDED
#define _FILES_H_INCLUDED

enum
{
	FRESULT_ERROR = 0,
	FRESULT_OK,
	FRESULT_BINARY
};

struct MAPPEDFILEDATA
{
	TCHAR fileName[_MAX_PATH];
	BOOL bWritable;
	DWORD dwOpenFlags;		// CreateFile()'s dwCreationDisposition
	DWORD dwSize;
	HANDLE hFile;
	HANDLE hMapping;
	LPVOID pMapBase;
};

BOOL files_openFileMapped(MAPPEDFILEDATA *fileData);
BOOL files_closeFileMapped(MAPPEDFILEDATA *fileData, DWORD newSize, BOOL flush);
int files_readEOL(TCHAR *lpLineEnd, DWORD bytesLeft, BOOL bEOLSensitive);
int files_binCheck(MAPPEDFILEDATA *fileData);

#endif // _FILES_H