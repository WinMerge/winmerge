#include "pch.h"
#include "MergeApp.h"
#include "unicoder.h"
#include "RegOptionsMgr.h"
#include "OptionsDef.h"

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

CRegOptionsMgr m_option(_T("Thingamahoochie\\WinMerge\\"));

COptionsMgr * GetOptionsMgr()
{
	m_option.InitOption(OPT_PLUGINS_CUSTOM_SETTINGS_LIST, _T(""));
	return &m_option;
}
