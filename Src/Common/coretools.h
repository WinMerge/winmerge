/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  coretools.h
 *
 * @brief Declaration file for Coretools.cpp
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef CORETOOLS_H
#define CORETOOLS_H

#include <io.h>
#include <stdlib.h>

/******** types & defines ********/
#define BYTES     1L
#define KBYTES    1024L
#define MBYTES    1048576L

// file types for GetFileType()
#define FTYPE_UNDETERMINED   0
#define FTYPE_EPS            1
#define FTYPE_PS             2
#define FTYPE_TIFF           3
#define FTYPE_TEXT           4
#define FTYPE_LAYOUT         5
#define FTYPE_PRESS          6
#define FTYPE_STEPPER        7
#define FTYPE_FOLD           8
#define FTYPE_TEMPLATE       9
#define FTYPE_MARKTEMP       10
#define FTYPE_DDES2			 11
#define FTYPE_PDF			 12

/******** macros ********/
// use this to get number of elements in a static array
#define ARRAY_SIZE(ar) (sizeof(ar)/sizeof(ar[0]))

// bitwise flag macros
#define FlagSet(var, flag)  (((flag) & (var)) == (flag))
#define AddFlag(var, flag)  ((var) |= (flag))
#define RemoveFlag(var, flag)  ((var) = (var) & (~(flag)))
#define ToggleFlag(var, flag, mask)  { RemoveFlag(var,mask); AddFlag(var,flag); }
#define GetFlag(var, mask)  (var & mask)



/******** function protos ********/

template <class T>
void swap(T *a,T *b)
{
 T tmp;
 tmp=*a;
 *a=*b;
 *b=tmp;
}

template <class T>
void selection_sort(T *ary, UINT cnt)
{
	register int i,j,smallest;
	T t;
	for (i=0; i < (int)cnt-1; i++)
	{
		smallest = i;
		for (j=i+1; (UINT)j < cnt; j++)
		{
			if (ary[j] < ary[smallest])
				smallest = j;
		}
		
		t = ary[smallest];
		ary[smallest] = ary[i];
		ary[i] = t;
	}
}

template <class T>
int linear_search(T *array, T target, int max)
{
  register int i=0;
  for (i=0; i < max; i++)
    if (array[i] == target)
      return i;

  return NONE_FOUND;
}

BOOL GetFileTimes(LPCTSTR szFilename, 
				  LPSYSTEMTIME pMod, 
				  LPSYSTEMTIME pCreate =NULL, 
				  LPSYSTEMTIME pAccess =NULL);
long GetFileModTime(LPCTSTR szPath);
DWORD GetFileSizeEx(LPCTSTR szFilename);
CString GetCDPath();
BOOL HaveAdminAccess();
BOOL DoModalProcess(CWnd *pWndParent, LPCTSTR szExeFile, 
					LPCTSTR szCmdLine, LPCTSTR szWindowCaption);

DWORD FPRINTF(HANDLE hf, LPCTSTR fmt, ... );
DWORD FPUTS(LPCTSTR s, HANDLE hf);
HANDLE FOPEN(LPCTSTR path, DWORD mode = GENERIC_READ, DWORD access = OPEN_EXISTING);
CString ConvertPath2PS(LPCSTR szPath);
void GetLocalDrives(LPTSTR letters);
//BOOL GetIP(LPTSTR straddr);
void replace_char(LPTSTR s, int target, int repl);
BOOL FileExtMatches(LPCTSTR filename, LPCTSTR ext);
void SplitFilename(LPCTSTR s, CString * path, CString * name, CString * ext);
void TestSplitFilename();
void AddExtension(LPTSTR name, LPCTSTR ext);
BOOL GetFreeSpaceString(LPCTSTR drivespec, ULONG mode, LPTSTR s);
int is_regfile(LPCTSTR buf,LPCTSTR path);
int is_dirfile(LPCTSTR buf,LPCTSTR path);
int is_regfile2(LPCTSTR path);
int is_dirfile2(LPCTSTR path);
int fcmp(float a,float b);
void aswap(LPTSTR a,LPTSTR b);
BOOL FindAnyFile(LPTSTR filespec, LPTSTR name);
long SwapEndian(long val);
short int SwapEndian(short int val);
BOOL MkDirEx(LPCTSTR filename);
CString GetModulePath(HMODULE hModule = NULL);
CString GetPathOnly(LPCTSTR fullpath);
float RoundMeasure(float measure, float units);

CString LegalizeFileName(LPCTSTR szFileName);
CString GetLocalizedNumberString(double dVal, int nPlaces =-1, BOOL bSeparate =FALSE, BOOL bTrailZeros =FALSE, LCID lcidNew =LOCALE_USER_DEFAULT);
void DDX_Float( CDataExchange* pDX, int nIDC, float& value );
void DDX_Double( CDataExchange* pDX, int nIDC, double& value );
int tcssubptr(LPCTSTR start, LPCTSTR end);
void ReadWcsToMbs(CFile* cf, LPTSTR szDest, DWORD dwCount);
void WriteWcsFromMbs(CFile* cf, LPCTSTR szSrc, DWORD dwCount);
void ReadWcsToMbs(HANDLE hf, LPTSTR szDest, DWORD dwCount);
void WriteWcsFromMbs(HANDLE hf, LPCTSTR szSrc, DWORD dwCount);
void ReadWcsToMbs(CArchive* ar, LPTSTR szDest, DWORD dwCount);
void WriteWcsFromMbs(CArchive* ar, LPCTSTR szSrc, DWORD dwCount);

HANDLE RunIt(LPCTSTR szExeFile, LPCTSTR szArgs, BOOL bMinimized = TRUE, BOOL bNewConsole = FALSE);
BOOL HasExited(HANDLE hProcess, DWORD *pCode = NULL);
BOOL IsLocalPath(LPCTSTR path);
BOOL GetAppDataPath(CString &sAppDataPath);
BOOL GetUserProfilePath(CString &sAppDataPath);
BOOL PutToClipboard(LPCTSTR pszText, HWND currentWindowHandle);
BOOL GetFromClipboard(CString & text, HWND currentWindowHandle);
BOOL TextInClipboard();

void GetDecoratedCmdLine(CString sCmdLine, CString &sDecoratedCmdLine,
	CString &sExecutable);
#endif
