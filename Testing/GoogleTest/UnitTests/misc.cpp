#include "MergeApp.h"
#include "unicoder.h"

void LogErrorStringUTF8(const std::string& sz)
{
}

void LogErrorString(const String& sz)
{
}

void AppErrorMessageBox(const String& msg)
{
}

String LoadResString(unsigned id)
{
	return _T("");
}

String GetSysError(int nerr)
{
	return _T("");
}

String tr(const std::string& str)
{
	return ucr::toTString(str);
}
