#include "StdAfx.h"
#include <stdio.h>
#include "PluginError.h"

static bool LoadResId(int id, LPTSTR buffer, unsigned int buflen);
static void PluginErrorDisplay(LPCTSTR message);
static void PluginErrorVarargs(LPCTSTR fmt, va_list args);

/**
 * @brief Display plugin error using a string resource
 */
void PluginError(int id)
{
	static TCHAR buffer[4096];
	unsigned int buflen = sizeof(buffer)/sizeof(buffer[0]);

	if (!LoadResId(id, buffer, buflen))
		_tcscpy(buffer, _T("Unknown Error (bad res id)"));

	PluginErrorDisplay(buffer);
}

/**
 * @brief Retrieve string resource from resources by id number
 */
static bool LoadResId(int id, LPTSTR buffer, unsigned int buflen)
{
	HINSTANCE hinst = _Module.GetModuleInstance();

	LPCTSTR namequiv = MAKEINTRESOURCE((id>>4)+1);

	if (::FindResource(hinst, namequiv, RT_STRING) != NULL &&
		::LoadString(hinst, id, buffer, buflen) != 0)
	{
		// found a non-zero string in app
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * @brief Display plugin error using a string resource and printf style arguments
 */
void PluginErrorFmt(int idfmt, ...)
{
	TCHAR buffer[4096];
	unsigned int buflen = sizeof(buffer)/sizeof(buffer[0]);

	if (!LoadResId(idfmt, buffer, buflen))
	{
		PluginErrorDisplay(_T("Unknown Error (bad res id)"));
		return;
	}
	
	va_list args;
	va_start(args, idfmt);
	PluginErrorVarargs(buffer, args);
	va_end(args);
}

/**
 * @brief Unpack varargs and display plugin error
 */
static void PluginErrorVarargs(LPCTSTR fmt, va_list args)
{
	TCHAR buffer[4096];
	unsigned int buflen = sizeof(buffer)/sizeof(buffer[0]);
	_vsntprintf(buffer, buflen, fmt, args);
	PluginErrorDisplay(buffer);
}

/**
 * @brief Actual workhorse that displays the message
 */
static void PluginErrorDisplay(LPCTSTR message)
{
	UINT uType = MB_OK + MB_ICONSTOP;
	LPCTSTR caption = _T("PluginErrorCaption");
	MessageBox(NULL, message, caption, uType);

}
