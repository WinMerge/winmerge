#include "StdAfx.h"
#include "MergeApp.h"
#include "Merge.h"
#include "VersionInfo.h"
#include "paths.h"
#include "Constants.h"
#include "unicoder.h"

// Get user language description of error, if available
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
		nullptr,
		nerr,
		0, // Default language
		(tchar_t*) &lpMsgBuf,
		0,
		nullptr 
		))
	{
		str = (const tchar_t*)lpMsgBuf;
		// Free the buffer.
		LocalFree( lpMsgBuf );
	}
	return str;
}

/**
 * @brief Get Options Manager.
 * @return Pointer to OptionsMgr.
 */
COptionsMgr * GetOptionsMgr()
{
	CMergeApp *pApp = static_cast<CMergeApp *>(AfxGetApp());
	return pApp->GetMergeOptionsMgr();
}

// Send message to log and debug window
void LogErrorString(const String& sz)
{
	if (sz.empty()) return;
	CString now = COleDateTime::GetCurrentTime().Format();
	TRACE(_T("%s: %s\n"), (const tchar_t*)now, sz.c_str());
}

// Send message to log and debug window
void LogErrorStringUTF8(const std::string& sz)
{
	if (sz.empty()) return;
	String str = ucr::toTString(sz);
	CString now = COleDateTime::GetCurrentTime().Format();
	TRACE(_T("%s: %s\n"), (const tchar_t*)now, str.c_str());
}

/**
 * @brief Load string resource and return as CString.
 * @param [in] id Resource string ID.
 * @return Resource string as CString.
 */
String LoadResString(unsigned id)
{
	return theApp.LoadString(id);
}

String tr(const std::string &str)
{
	String translated_str;
	theApp.TranslateString(str, translated_str);
	return translated_str;
}

String tr(const std::wstring &str)
{
	String translated_str;
	theApp.TranslateString(str, translated_str);
	return translated_str;
}

String tr(const char *msgctxt, const std::string &str)
{
	String translated_str;
	if (msgctxt)
		theApp.TranslateString("\x01\"" + std::string(msgctxt) + "\"" + str, translated_str);
	else
		theApp.TranslateString(str, translated_str);
	return translated_str;
}

void AppErrorMessageBox(const String& msg)
{
	AppMsgBox::error(msg);
}

namespace AppMsgBox
{

namespace detail
{
	int convert_to_winflags(int flags)
	{
		int newflags = 0;

		if ((flags & (YES | NO | CANCEL)) == (YES | NO | CANCEL)) newflags |= MB_YESNOCANCEL;
		else if ((flags & (YES | NO)) == (YES | NO)) newflags |= MB_YESNO;
		else if ((flags & (OK | CANCEL)) == (OK | CANCEL)) newflags |= MB_OKCANCEL;
		else if ((flags & OK) == OK) newflags |= MB_OK;
	
		if (flags & YES_TO_ALL) newflags |= MB_YES_TO_ALL;
		if (flags & DONT_DISPLAY_AGAIN) newflags |= MB_DONT_DISPLAY_AGAIN;

		return newflags;
	}

	int convert_resp(int resp)
	{
		switch (resp)
		{
		case IDOK:
			return OK;
		case IDCANCEL:
			return CANCEL;
		case IDNO:
			return NO;
		case IDYES:
			return YES;
		case IDYESTOALL:
			return YES_TO_ALL;
		default:
			return OK;
		}
	}
}

int error(const String& msg, int type)
{
	return detail::convert_resp(AfxMessageBox(msg.c_str(), detail::convert_to_winflags(type) | MB_ICONSTOP));
}

int warning(const String& msg, int type)
{
	return detail::convert_resp(AfxMessageBox(msg.c_str(), detail::convert_to_winflags(type) | MB_ICONWARNING));
}

int information(const String& msg, int type)
{
	return detail::convert_resp(AfxMessageBox(msg.c_str(), detail::convert_to_winflags(type) | MB_ICONINFORMATION));
}

}

AboutInfo::AboutInfo()
{
	CVersionInfo verinfo;
	version = strutils::format_string1(_("Version %1"), verinfo.GetProductVersion());
	private_build = verinfo.GetPrivateBuild();
	if (!private_build.empty())
	{
		version += _T(" + ") + private_build;
	}

	if (version.find(_T(" - ")) != String::npos)
	{
		strutils::replace(version, _T(" - "), _T("\n"));
		version += _T(" ");
	}
	else
	{
		version += _T("\n");
	}

#if defined _M_IX86
	version += _T(" ");
	version += _T("x86");
#elif defined _M_IA64
	version += _T(" IA64");
#elif defined _M_X64
	version += _T(" ");
	version += _("X64");
#elif defined _M_ARM
	version += _T(" ARM");
#elif defined _M_ARM64
	version += _T(" ARM64");
#endif

#if defined _DEBUG
	version += _T(" (");
	version += _T("Debug");
	version += _T(")");
#endif

	copyright = _("WinMerge comes with ABSOLUTELY NO WARRANTY. This is free software and you are welcome to redistribute it under certain circumstances; see the GNU General Public License in the Help menu for details.");
	copyright += _T("\n");
	copyright += verinfo.GetLegalCopyright();
	copyright += _T(" - All rights reserved.");

	website = WinMergeURL;
}
