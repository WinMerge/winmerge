#include "pch.h"
#include <iostream>
#include <Windows.h>
#include "UnicodeString.h"
#include "unicoder.h"
#include "OptionsMgr.h"
#include "RegOptionsMgr.h"

CRegOptionsMgr m_optionsMgr(_T("Thingamahoochie\\WinMerge\\"));

COptionsMgr * GetOptionsMgr()
{
	return &m_optionsMgr;
}

String GetSysError(int nerr /* =-1 */)
{
	if (nerr == -1)
		nerr = GetLastError();
	LPVOID lpMsgBuf;
	String str = _T("?");
	if (FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		nerr,
		0, // Default language
		(tchar_t*) &lpMsgBuf,
		0,
		NULL 
		))
	{
		str = (const tchar_t*)lpMsgBuf;
	}
	// Free the buffer.
	LocalFree( lpMsgBuf );
	return str;
}

String LoadResString(unsigned id)
{
	return _T("Nothing");
}

void AppErrorMessageBox(const String& msg)
{
	MessageBox(NULL, msg.c_str(), NULL, MB_ICONSTOP);
}

String tr(const std::string& str)
{
	return ucr::toTString(str);
}

String tr(const std::wstring& str)
{
	return ucr::toTString(str);
}

void NTAPI LangTranslateDialog(HWND h)
{
}

void* AppGetMainHWND()
{
	return nullptr;
}

