#ifndef __MERGEAPP_H__
#define __MERGEAPP_H__

#include "UnicodeString.h"

#define _(x) tr(x)
#define N_(x) (x)

class COptionsMgr;

/** @brief Retrieve error description from Windows; uses FormatMessage */
String GetSysError(int nerr = -1);

COptionsMgr * GetOptionsMgr();
void LogErrorString(const String& sz);
void LogErrorStringUTF8(const std::string& sz);
void AppErrorMessageBox(const String& msg);

/** @brief Load string from string resources; shortcut for CString::LoadString */
String LoadResString(unsigned id);

String tr(const std::string &str);

#endif /* __MERGEAPP_H__ */