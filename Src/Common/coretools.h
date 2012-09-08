/**
 * @file  coretools.h
 *
 * @brief Declaration file for Coretools.cpp
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef CORETOOLS_H
#define CORETOOLS_H

#include "UnicodeString.h"

void replace_char(LPTSTR s, int target, int repl);
void SplitFilename(LPCTSTR s, String * path, String * name, String * ext);
void SplitViewName(LPCTSTR s, String * path, String * name, String * ext);
String GetModulePath(HMODULE hModule = NULL);
String GetPathOnly(LPCTSTR fullpath);

size_t linelen(const char *string, size_t maxlen);

HANDLE RunIt(LPCTSTR szExeFile, LPCTSTR szArgs, BOOL bMinimized = TRUE, BOOL bNewConsole = FALSE);
BOOL HasExited(HANDLE hProcess, DWORD *pCode = NULL);

#endif
