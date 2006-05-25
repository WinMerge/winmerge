/** 
 * @file  paths.h
 *
 * @brief Declaration file for path routines
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef paths_h_included
#define paths_h_included

/**
 * @brief Possible values when checking for file/folder existence.
 */
typedef enum
{
	DOES_NOT_EXIST, /**< File or folder does not exist. */
	IS_EXISTING_FILE, /**< It is existing file */
	IS_EXISTING_DIR, /**< It is existing folder */
} PATH_EXISTENCE;

bool paths_EndsWithSlash(const CString & s);
PATH_EXISTENCE paths_DoesPathExist(LPCTSTR szPath);
void paths_normalize(CString & sPath);
CString paths_GetLongPath(const CString & sPath);
bool paths_CreateIfNeeded(const CString & sPath);
LPCTSTR paths_GetTempPath(int * pnerr=NULL);
CString paths_GetTempFileName(LPCTSTR lpPathName, LPCTSTR lpPrefixString, int * pnerr=NULL);
PATH_EXISTENCE GetPairComparability(LPCTSTR pszLeft, LPCTSTR pszRight);
CString ExpandShortcut(const CString &inFile);
CString paths_ConcatPath(const CString & path, const CString & subpath);
CString paths_GetParentPath(CString path);
CString paths_GetLastSubdir(CString path);
BOOL paths_IsPathAbsolute(const CString &path);
CString paths_EnsurePathExist(const CString & sPath);
CString paths_GetWindowsDirectory();
CString paths_GetMyDocuments(HWND hWindow);

#endif // paths_h_included
