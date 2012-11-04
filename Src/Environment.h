/** 
 * @file  Environment.h
 *
 * @brief Declaration file for Environment-related routines.
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _ENVIRONMENT_H_
#define _ENVIRONMENT_H_

#include "UnicodeString.h"

void env_SetTempPath(const String& path);
String env_GetTempPath();
String env_GetTempFileName(const String& lpPathName, const String& lpPrefixString,
		int * pnerr = NULL);
void env_SetProgPath(const String& path);
String env_GetProgPath();

String env_GetWindowsDirectory();
String env_GetMyDocuments();
String env_GetSystemTempPath();

String env_GetPerInstanceString(const String& name);

#endif // _ENVIRONMENT_H_
