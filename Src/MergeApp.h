#pragma once

#include "UnicodeString.h"

#define _(x) tr(x)
#define N_(x) (x)
#define NC_(c, x) (x)

class COptionsMgr;
class FileFilterHelper;

struct AboutInfo
{
	AboutInfo();
	String copyright;
	String version;
	String private_build;
	String website;
};

/** @brief Retrieve error description from Windows; uses FormatMessage */
String GetSysError(int nerr = -1);

COptionsMgr * GetOptionsMgr();
void LogErrorString(const String& sz);
void LogErrorStringUTF8(const std::string& sz);
void AppErrorMessageBox(const String& msg);

namespace AppMsgBox
{
	enum
	{
		YES = 1,
		OK = 2,
		NO = 4,
		CANCEL = 8,
		YES_TO_ALL = 16,
		DONT_DISPLAY_AGAIN = 32,
	};
	int error(const String& msg, int type = OK);
	int warning(const String& msg, int type = OK);
	int information(const String& msg, int type = OK);
};

/** @brief Load string from string resources; shortcut for String::LoadString */
String LoadResString(unsigned id);

String tr(const std::string &str);
String tr(const std::wstring &str);
String tr(const char *msgctxt, const std::string &str);