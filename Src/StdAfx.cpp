/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
/////////////////////////////////////////////////////////////////////////////
// stdafx.cpp : source file that includes just the standard includes
//	Merge.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information
//
// RCS ID line follows -- this is updated by CVS
// $Id: StdAfx.cpp 6457 2009-02-15 14:08:50Z kimmov $

#include "stdafx.h"
#include "LogFile.h"
#include "Merge.h"

/**
 * @brief Wrapper around CMergeApp::TranslateDialog()
 */
void NTAPI LangTranslateDialog(HWND h)
{
	theApp.TranslateDialog(h);
}

/**
 * @brief Lang aware version of AfxMessageBox()
 */
int NTAPI LangMessageBox(UINT nIDPrompt, UINT nType, UINT nIDHelp)
{
	String string = theApp.LoadString(nIDPrompt);
	if (nIDHelp == (UINT)-1)
		nIDHelp = nIDPrompt;
	return AfxMessageBox(string.c_str(), nType, nIDHelp);
}

/**
 * @brief Show messagebox with resource string having parameter.
 * @param [in] msgid Resource string ID.
 * @param [in] arg Argument string.
 * @param [in] nType Messagebox type flags (e.g. MB_OK).
 * @param [in] nIDHelp Help string ID.
 * @return User choice from the messagebox (see MessageBox()).
 */
int ResMsgBox1(UINT msgid, LPCTSTR arg, UINT nType, UINT nIDHelp)
{
	String msg = LangFormatString1(msgid, arg);
	if (!nIDHelp)
		nIDHelp = msgid;
	return AfxMessageBox(msg.c_str(), nType, nIDHelp);
}

// Get appropriate clipboard format for TCHAR text
int GetClipTcharTextFormat()
{
#ifdef _UNICODE
	return CF_UNICODETEXT;
#else
	return CF_TEXT;
#endif // _UNICODE
}

/**
 * @brief Return true if Unicode (16-bit) build
 */
bool IsUnicodeBuild()
{
#ifdef UNICODE
	return true;
#else
	return false;
#endif
}

#if _MSC_VER <= 1310
/**
* @brief Calculates a number absolute value.
*/
__int64 _abs64_patch(__int64 n)
{
	return ((n >= 0) ? n : -n);
}
#endif
