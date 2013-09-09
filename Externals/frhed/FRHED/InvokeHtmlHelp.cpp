/*
Frhed - Free hex editor
Copyright (C) 2000 Raihan Kibria

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.

Last change: 2013-02-24 by Jochen Neubeck
*/
/** 
 * @file  InvokeHtmlHelp.cpp
 *
 * @brief Implementation for helper function to start HtmlHelp.
 *
 */
#include "precomp.h"
#include <htmlhelp.h>
#include <tchar.h>
#include "Constants.h"
#include "resource.h"
#include "StringTable.h"
#include "hexwnd.h"
#include "hexwdlg.h"

/**
 * @brief Show HTML help.
 * This function shows HTML help, with given command and path.
 * @param [in] uCommand How/what to open in help (TOC etc).
 * @param [in] path Additional relative path inside help file to open.
 * @param [in] hParentWindow Window opening this help.
 */
void ShowHtmlHelp(UINT uCommand, LPCTSTR path, HWindow *pParentWindow)
{
	TCHAR fullHelpPath[MAX_PATH];
	GetModuleFileName(NULL, fullHelpPath, MAX_PATH);
	PathRemoveFileSpec(fullHelpPath);
	PathAppend(fullHelpPath, HtmlHelpFile);
	if (!PathFileExists(fullHelpPath))
	{
		TCHAR msg[500];
		_sntprintf(msg, RTL_NUMBER_OF(msg), GetLangString(IDS_ERR_HELP_NOT_FOUND), fullHelpPath);
		MessageBox(pParentWindow, msg, MB_ICONERROR);
		return;
	}

	if (uCommand == HELP_CONTEXT)
	{
		if (path != NULL)
		{
			TCHAR fullpath[MAX_PATH] = {0};
			_sntprintf(fullpath, MAX_PATH, _T("%s::/%s"), fullHelpPath, path);
			HtmlHelp(NULL, fullpath, HH_DISPLAY_TOPIC, NULL);
		}
		else
		{
			HtmlHelp(NULL, fullHelpPath, HH_DISPLAY_TOPIC, NULL);
		}
	}
	else if (uCommand == HELP_FINDER)
	{
		HtmlHelp(NULL, fullHelpPath, HH_DISPLAY_SEARCH, NULL);
	}
	else if (uCommand == HELP_CONTENTS)
	{
		HtmlHelp(NULL, fullHelpPath, HH_DISPLAY_TOC, NULL);
	}
}
