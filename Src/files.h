#ifndef _FILES_H_INCLUDED
#define _FILES_H_INCLUDED

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
BOOL files_safeWriteFile(HANDLE hFile, LPVOID lpBuf, DWORD dwLength);
BOOL files_safeReadFile(HANDLE hFile, LPVOID lpBuf, DWORD dwLength);

#endif // _FILES_H