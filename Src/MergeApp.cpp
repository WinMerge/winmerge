#include "StdAfx.h"
#include "MergeApp.h"
#include "Merge.h"
#include "VersionInfo.h"
#include "Constants.h"
#include "Logger.h"
#include "../Version.h"

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
	RootLogger::Error(sz);
}

// Send message to log and debug window
void LogErrorStringUTF8(const std::string& sz)
{
	RootLogger::Error(sz);
}

void AppErrorMessageBox(const String& msg)
{
	AppMsgBox::error(msg);
	RootLogger::Error(msg);
}

void* AppGetMainHWND()
{
	return AfxGetMainWnd()->GetSafeHwnd();
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
	version += _T(" (") + _(STRYEARMONTH) + _T(")");
	private_build = verinfo.GetPrivateBuild();
	if (!private_build.empty())
	{
		version += _T(" + ") + private_build;
	}

	if (version.find(_T(" - ")) != String::npos)
	{
		strutils::replace(version, _T(" - "), _T("\r\n"));
		version += _T(" ");
	}
	else
	{
		version += _T("\r\n");
	}

#if defined _M_IX86
	version += _T(" x86");
#elif defined _M_IA64
	version += _T(" IA64");
#elif defined _M_X64
	version += _T(" X64");
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

	copyright = _("WinMerge comes with ABSOLUTELY NO WARRANTY. It is free software and can be redistributed under the conditions of the GNU General Public License - see the Help menu for details.");
	copyright += _T("\n");
	copyright += verinfo.GetLegalCopyright();
	copyright += _(" - All rights reserved.");

	website = WinMergeURL;
}
