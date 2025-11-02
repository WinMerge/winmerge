#pragma once

#include "UnicodeString.h"
#include "I18n.h"

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
void* AppGetMainHWND();

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
