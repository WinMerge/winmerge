/** 
 * @file  paths.h
 *
 * @brief Declaration file for path routines
 */
// ID line follows -- this is updated by SVN
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
PATH_EXISTENCE GetPairComparability(LPCTSTR pszLeft, LPCTSTR pszRight);
String ExpandShortcut(const String &inFile);
String paths_ConcatPath(const String & path, const String & subpath);
String paths_GetParentPath(LPCTSTR path);
String paths_GetLastSubdir(const String & path);
BOOL paths_IsPathAbsolute(const String & path);
String paths_EnsurePathExist(const String & sPath);
String paths_GetWindowsDirectory();
String paths_GetMyDocuments(HWND hWindow);

#endif // paths_h_included
