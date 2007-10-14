/** 
 * @file  paths.h
 *
 * @brief Declaration file for path routines
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef paths_h_included
#define paths_h_included

#include "UnicodeString.h"

/**
 * @brief Possible values when checking for file/folder existence.
 */
typedef enum
{
	DOES_NOT_EXIST, /**< File or folder does not exist. */
	IS_EXISTING_FILE, /**< It is existing file */
	IS_EXISTING_DIR, /**< It is existing folder */
} PATH_EXISTENCE;

bool paths_EndsWithSlash(LPCTSTR s);

PATH_EXISTENCE paths_DoesPathExist(LPCTSTR szPath);
void paths_normalize(String & sPath);
String paths_GetLongPath(LPCTSTR szPath, BOOL bExpandEnvs = TRUE);
bool paths_CreateIfNeeded(LPCTSTR szPath);
LPCTSTR paths_GetTempPath(int * pnerr=NULL);
String paths_GetTempFileName(LPCTSTR lpPathName, LPCTSTR lpPrefixString, int * pnerr=NULL);
PATH_EXISTENCE GetPairComparability(LPCTSTR pszLeft, LPCTSTR pszRight);
CString ExpandShortcut(const CString &inFile);
String paths_ConcatPath(const String & path, const String & subpath);
String paths_GetParentPath(LPCTSTR path);
CString paths_GetLastSubdir(const CString & path);
BOOL paths_IsPathAbsolute(const CString & path);
String paths_EnsurePathExist(const String & sPath);
CString paths_GetWindowsDirectory();
CString paths_GetMyDocuments(HWND hWindow);

#endif // paths_h_included
