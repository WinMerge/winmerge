/** 
 * @file  Environment.h
 *
 * @brief Declaration file for Environment-related routines.
 */
#pragma once

#include "UnicodeString.h"

namespace env
{

void SetTemporaryPath(const String& path);
String GetTemporaryPath();
String GetTemporaryFileName(const String& lpPathName, const String& lpPrefixString,
		int * pnerr = nullptr);
String GetTempChildPath();
void SetProgPath(const String& path);
String GetProgPath();

String GetWindowsDirectory();
String GetMyDocuments();
String GetSystemTempPath();

String GetPerInstanceString(const String& name);

bool LoadRegistryFromFile(const String& sRegFilePath);
bool SaveRegistryToFile(const String& sRegFilePath, const String& sRegDir);

}
