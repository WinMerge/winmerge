/** 
 * @file  paths.h
 *
 * @brief Declaration file for path routines
 */
#pragma once

#include "PathContext.h"
#include "UnicodeString.h"

#ifndef MAX_PATH_FULL
#  define MAX_PATH_FULL 32767
#endif

namespace paths
{

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

bool EndsWithSlash(const String& s);

PATH_EXISTENCE DoesPathExist(const String& szPath, bool (*IsArchiveFile)(const String&) = nullptr);
String FindFileName(const String& path);
String FindExtension(const String& path);
void normalize(String & sPath);
String GetLongPath(const String& szPath, bool bExpandEnvs = true);
bool CreateIfNeeded(const String& szPath);
PATH_EXISTENCE GetPairComparability(const PathContext & paths, bool (*IsArchiveFile)(const String&) = nullptr);
bool IsDirectory(const String& path);
bool IsShortcut(const String& inPath);
String ExpandShortcut(const String &inFile);
String ConcatPath(const String & path, const String & subpath);
String GetParentPath(const String& path);
String GetLastSubdir(const String & path);
bool IsPathAbsolute(const String & path);
String EnsurePathExist(const String & sPath);
void SplitFilename(const String& s, String * path, String * name, String * ext);
String GetPathOnly(const String& fullpath);
bool IsURLorCLSID(const String& path);
bool IsDecendant(const String& path, const String& ancestor);
inline String AddTrailingSlash(const String& path) { return !EndsWithSlash(path) ? path + _T("\\") : path; }
String ToWindowsPath(const String& path);
String ToUnixPath(const String& path);

}	
