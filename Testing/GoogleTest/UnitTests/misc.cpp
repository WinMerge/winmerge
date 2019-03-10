#include "pch.h"
#include "MergeApp.h"
#include "unicoder.h"
#include "RegOptionsMgr.h"
#include "OptionsDef.h"

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

CRegOptionsMgr m_option;

COptionsMgr * GetOptionsMgr()
{
	m_option.InitOption(OPT_PLUGINS_DISABLED_LIST, _T(""));
	return &m_option;
}
