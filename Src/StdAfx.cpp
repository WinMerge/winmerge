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


// Get appropriate clipboard format for TCHAR text
int GetClipTcharTextFormat()
{
#ifdef _UNICODE
	return CF_UNICODETEXT;
#else
	return CF_TEXT;
#endif // _UNICODE
}

