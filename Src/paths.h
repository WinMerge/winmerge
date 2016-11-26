/** 
 * @file  paths.h
 *
 * @brief Declaration file for path routines
 */
#pragma once

#include "PathContext.h"
#include "UnicodeString.h"

/**
 * @brief Possible values when checking for file/folder existence.
 */
typedef enum
{
	DOES_NOT_EXIST, /**< File or folder does not exist. */
	IS_EXISTING_FILE, /**< It is existing file */
	IS_EXISTING_DIR, /**< It is existing folder */
	IS_EXISTING_DIR_ARCHIVE, /**< It is existing folder */
} PATH_EXISTENCE;

bool paths_EndsWithSlash(const String& s);

PATH_EXISTENCE paths_DoesPathExist(const String& szPath, bool (*IsArchiveFile)(const String&) = NULL);
String paths_FindFileName(const String& path);
String paths_FindExtension(const String& path);
void paths_normalize(String & sPath);
String paths_GetLongPath(const String& szPath, bool bExpandEnvs = true);
bool paths_CreateIfNeeded(const String& szPath);
PATH_EXISTENCE GetPairComparability(const PathContext & paths, bool (*IsArchiveFile)(const String&) = NULL);
bool paths_IsDirectory(const String& path);
bool paths_IsShortcut(const String& inPath);
String ExpandShortcut(const String &inFile);
String paths_ConcatPath(const String & path, const String & subpath);
String paths_GetParentPath(const String& path);
String paths_GetLastSubdir(const String & path);
bool paths_IsPathAbsolute(const String & path);
String paths_EnsurePathExist(const String & sPath);
void paths_SplitFilename(const String& s, String * path, String * name, String * ext);
void paths_SplitViewName(const TCHAR *s, String * path, String * name, String * ext);
String paths_GetPathOnly(const String& fullpath);
bool paths_IsURLorCLSID(const String& path);
bool paths_IsDecendant(const String& path, const String& ancestor);
inline String paths_AddTrailingSlash(const String& path) { return !paths_EndsWithSlash(path) ? path + _T("\\") : path; }
String paths_ToWindowsPath(const String& path);
String paths_ToUnixPath(const String& path);
