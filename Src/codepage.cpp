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
/** 
 * @file  codepage.cpp
 */
// RCS ID line follows -- this is updated by SVN
// $Id$

#include "StdAfx.h"
#include "Merge.h"
#include "codepage.h"
#include "DirScan.h" // for DirScan_InitializeDefaultCodepage

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static void initialize();

static int f_nDefaultCodepage; // store the default codepage as specified by user in options

static int f_bInitialized = false;

// map number to bit code
enum { CP_SUPPORTED_FLAG=0x01, CP_INSTALLED_FLAG=0x10 };
static CMap<int, int, int, int> f_codepage_status;

/**
 * @brief Update the appropriate default codepage
 */
void updateDefaultCodepage(int cpDefaultMode, int customCodepage)
{
	int wLangId;
	switch (cpDefaultMode)
	{
		case 0:
			f_nDefaultCodepage = GetACP();
			break;
		case 1:
			TCHAR buff[32];
			wLangId = theApp.GetLangId();
			if (GetLocaleInfo(wLangId, LOCALE_IDEFAULTANSICODEPAGE, buff, sizeof(buff)/sizeof(buff[0])))
				f_nDefaultCodepage = _ttol(buff);
			else
				f_nDefaultCodepage = GetACP();
			break;
		case 2:
			f_nDefaultCodepage = customCodepage;
			break;
		default:
			// no other valid option
			ASSERT (0);
			f_nDefaultCodepage = GetACP();
	}

	// Set default codepage
	DirScan_InitializeDefaultCodepage();
}


/**
 * @brief Get appropriate default codepage 
 */
int getDefaultCodepage()
{
	return f_nDefaultCodepage;
}

/**
 * @brief Callback used by initializeCodepages
 */
static BOOL CALLBACK EnumInstalledCodePagesProc(LPTSTR lpCodePageString)
{
	int codepage = _ttol(lpCodePageString);
	if (codepage)
	{
		int value = 0;
		f_codepage_status.Lookup(codepage, value);
		value |= CP_INSTALLED_FLAG; // installed
		f_codepage_status[codepage] = value;
	}
	return TRUE; // continue enumeratino
}

/**
 * @brief Callback used by initializeCodepages
 */
static BOOL CALLBACK EnumSupportedCodePagesProc(LPTSTR lpCodePageString)
{
	int codepage = _ttol(lpCodePageString);
	if (codepage)
	{
		int value = 0;
		f_codepage_status.Lookup(codepage, value);
		value |= CP_SUPPORTED_FLAG; // supported (but not necessarily installed)
		f_codepage_status[codepage] = value;
	}
	return TRUE; // continue enumeratino
}

/**
 * @brief Load information about codepages into local cache
 */
static void initializeCodepageStatuses()
{
	EnumSystemCodePages(EnumInstalledCodePagesProc,  CP_INSTALLED);
	EnumSystemCodePages(EnumSupportedCodePagesProc, CP_SUPPORTED);
}

/**
 * @brief Return true if specified codepage is installed on this system (according to EnumSystemCodePages)
 */
bool isCodepageInstalled(int codepage)
{
	initialize();
	int value=0;
	return f_codepage_status.Lookup(codepage, value) && (value & CP_INSTALLED_FLAG);
}

/**
 * @brief Initialize all local information stores (ie, maps)
 */
static void initialize()
{
	if (f_bInitialized) return;
	initializeCodepageStatuses();
	f_bInitialized = true;
}

