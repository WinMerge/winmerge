/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
// stdafx.cpp : source file that includes just the standard includes
//	Merge.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include "Merge.h"

int CWinMergeProfile::level = 0;
CMapStringToPtr CWinMergeProfile::map;
LARGE_INTEGER CWinMergeProfile::origin;

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
