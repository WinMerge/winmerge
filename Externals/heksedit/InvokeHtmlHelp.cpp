/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  InvokeHtmlHelp.cpp
 *
 * @brief Implementation for helper function to start HtmlHelp.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: InvokeHtmlHelp.cpp 45 2008-09-04 14:05:26Z jtuc $

#include "precomp.h"
#include <htmlhelp.h>
#include "Constants.h"
#include "InvokeHtmlHelp.h"

/**
 * @brief Show HTML help.
 * This function shows HTML help, with given command and path.
 * @param [in] uCommand How/what to open in help (TOC etc).
 * @param [in] context Additional relative path inside help file to open.
 * @param [in] hParentWindow Window opening this help.
 */
void ShowHtmlHelp(UINT uCommand, LPCTSTR context, HWND hParentWindow)
{
	TCHAR path[MAX_PATH + 100];
	GetModuleFileName(0, path, MAX_PATH);
	PathRemoveFileSpec(path);
	PathAppend(path, HtmlHelpFile);
	switch (uCommand)
	{
	case HELP_CONTEXT:
		if (context)
		{
			lstrcat(path, _T("::/"));
			lstrcat(path, context);
		}
		uCommand = HH_DISPLAY_TOPIC;
		break;
	case HELP_FINDER:
		uCommand = HH_DISPLAY_SEARCH;
		break;
	case HELP_CONTENTS:
		uCommand = HH_DISPLAY_TOC;
		break;
	default:
		assert(FALSE);
		return;
	}
	if (HtmlHelp(hParentWindow, path, uCommand, NULL) == NULL)
	{
		TCHAR msg[500];
		msg[RTL_NUMBER_OF(msg) - 1] = '\0';
		_sntprintf(msg, RTL_NUMBER_OF(msg) - 1, _T("Help file\n%s\nnot found!"), HtmlHelpFile);
		MessageBox(hParentWindow, msg, ApplicationName, MB_ICONERROR);
	}
}
