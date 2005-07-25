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
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
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

// map number to bit code, 0x01 = supported, 0x10 = installed
static CMap<int, int, int, int> f_codepage_status;

// map name to number
// eg, "CP-1252" => 1252
static CMap<CString, LPCTSTR, int, int> f_codepageAliases;

// map codepage number to index in f_codepageinfo
static CMap<int, int, int, int> f_codepage_info_index;

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
 * @brief Record a codepage alias string (ok if already recorded)
 */
static void RecordAlias(LPCTSTR alias, int codepage)
{
	f_codepageAliases[alias] = codepage;
}

/**
 * @brief Record all the obvious codepage aliases (eg, "CP-1252" for 1252)
 */
static void RecordAliases(LPCTSTR lpCodePageString, int codepage)
{
	if (codepage > 400 && codepage < 1000)
	{
		RecordAlias((CString)_T("CP-") + lpCodePageString, codepage);
		RecordAlias((CString)_T("CP") + lpCodePageString, codepage);
	}
	if (codepage > 800 && codepage < 1300)
	{
		RecordAlias((CString)_T("CP-") + lpCodePageString, codepage);
		RecordAlias((CString)_T("CP") + lpCodePageString, codepage);
		RecordAlias((CString)_T("windows-") + lpCodePageString, codepage);
	}
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
		value |= 0x10; // installed
		f_codepage_status[codepage] = value;
		RecordAliases(lpCodePageString, codepage);
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
		value |= 0x01; // installed
		f_codepage_status[codepage] = value;
		RecordAliases(lpCodePageString, codepage);
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
	return f_codepage_status.Lookup(codepage, value) && (value & 0x01);
}

/**
 * @brief Return true if specified codepage is supported on this system (according to EnumSystemCodePages)
 */
bool isCodepageSupported(int codepage)
{
	initialize();
	int value=0;
	return f_codepage_status.Lookup(codepage, value) && (value & 0x10);
}

static codepage_wminfo f_codepageinfo[] = {
	 { 37, IDS_CP_EBCDIC, 0, IDS_CPG_WESTERN_EUROPEAN, IDS_CPP_IBM }
	, { 437, IDS_CP_DOS_US, 0, IDS_CPG_WESTERN_EUROPEAN, IDS_CPP_DOS }
	, { 708, IDS_CP_DOS_ARABIC, 0, IDS_CPG_MIDDLE_EASTERN, IDS_CPP_DOS }
	, { 737, IDS_CP_DOS_GREEK, 0, IDS_CPG_EASTERN_EUROPEAN, IDS_CPP_DOS }
	, { 775, IDS_CP_DOS_BALTIC, 0, IDS_CPG_EASTERN_EUROPEAN, IDS_CPP_DOS }
	, { 850, IDS_CP_DOS_LATIN1, 0, IDS_CPG_WESTERN_EUROPEAN, IDS_CPP_DOS }
	, { 851, IDS_CP_DOS_GREEK1, 0, IDS_CPG_EASTERN_EUROPEAN, IDS_CPP_DOS }
	, { 852, IDS_CP_DOS_LATIN2, 0, IDS_CPG_EASTERN_EUROPEAN, IDS_CPP_DOS }
	, { 855, IDS_CP_DOS_CYRILLIC, 0, IDS_CPG_EASTERN_EUROPEAN, IDS_CPP_DOS }
	, { 857, IDS_CP_DOS_TURKISH, 0, IDS_CPG_EASTERN_EUROPEAN, IDS_CPP_DOS }
	, { 860, IDS_CP_DOS_PORTUGUESE, 0, IDS_CPG_WESTERN_EUROPEAN, IDS_CPP_DOS }
	, { 861, IDS_CP_DOS_ICELANDIC, 0, IDS_CPG_WESTERN_EUROPEAN, IDS_CPP_DOS }
	, { 862, IDS_CP_DOS_HEBREW, 0, IDS_CPG_MIDDLE_EASTERN, IDS_CPP_DOS }
	, { 863, IDS_CP_DOS_FRENCH_CANADA, 0, IDS_CPG_WESTERN_EUROPEAN, IDS_CPP_DOS }
	, { 865, IDS_CP_DOS_NORDIC, 0, IDS_CPG_WESTERN_EUROPEAN, IDS_CPP_DOS }
	, { 866, IDS_CP_DOS_CYRILLIC_CIS1, 0, IDS_CPG_EASTERN_EUROPEAN, IDS_CPP_DOS }
	, { 869, IDS_CP_DOS_GREEK2, 0, IDS_CPG_EASTERN_EUROPEAN, IDS_CPP_DOS }
	, { 874, IDS_CP_WINDOWS_THAI, 0, IDS_CPG_SOUTH_ASIAN, IDS_CPP_WINDOWS }
	, { 932, IDS_CP_WINDOWS_JAPANESE, IDS_CP_WINDOWS_JAPANESE_EXTRA, IDS_CPG_EAST_ASIAN, IDS_CPP_WINDOWS }
	, { 936, IDS_CP_WINDOWS_CHINESE_SIMP, IDS_CP_WINDOWS_CHINESE_SIMP_EXTRA, IDS_CPG_EAST_ASIAN, IDS_CPP_WINDOWS }
	, { 949, IDS_CP_WINDOWS_KOREAN, IDS_CP_WINDOWS_KOREAN_EXTRA, IDS_CPG_EAST_ASIAN, IDS_CPP_WINDOWS }
	, { 950, IDS_CP_WINDOWS_CHINESE_TRAD, IDS_CP_WINDOWS_CHINESE_TRAD_EXTRA, IDS_CPG_EAST_ASIAN, IDS_CPP_WINDOWS }
	, { 1250, IDS_CP_WINDOWS_LATIN2, IDS_CP_WINDOWS_LATIN2_EXTRA, IDS_CPG_EASTERN_EUROPEAN, IDS_CPP_WINDOWS }
	, { 1251, IDS_CP_WINDOWS_CYRILLIC, IDS_CP_WINDOWS_CYRILLIC_EXTRA, IDS_CPG_EASTERN_EUROPEAN, IDS_CPP_WINDOWS }
	, { 1252, IDS_CP_WINDOWS_LATIN1, IDS_CP_WINDOWS_LATIN1_EXTRA, IDS_CPG_WESTERN_EUROPEAN, IDS_CPP_WINDOWS }
	, { 1253, IDS_CP_WINDOWS_GREEK, IDS_CP_WINDOWS_GREEK_EXTRA, IDS_CPG_EASTERN_EUROPEAN, IDS_CPP_WINDOWS }
	, { 1254, IDS_CP_WINDOWS_TURKISH, IDS_CP_WINDOWS_TURKISH_EXTRA, IDS_CPG_EASTERN_EUROPEAN, IDS_CPP_WINDOWS }
	, { 1255, IDS_CP_WINDOWS_HEBREW, IDS_CP_WINDOWS_HEBREW_EXTRA, IDS_CPG_MIDDLE_EASTERN, IDS_CPP_WINDOWS }
	, { 1256, IDS_CP_WINDOWS_ARABIC, 0, IDS_CPG_MIDDLE_EASTERN, IDS_CPP_WINDOWS }
	, { 1257, IDS_CP_WINDOWS_BALTIC, 0, IDS_CPG_EASTERN_EUROPEAN, IDS_CPP_WINDOWS }
	, { 1258, IDS_CP_WINDOWS_VIETNAM, 0, IDS_CPG_SOUTH_ASIAN, IDS_CPP_WINDOWS }
	, { 10000, IDS_CP_MACINTOSH_ROMAN, 0, IDS_CPG_WESTERN_EUROPEAN, IDS_CPP_MACINTOSH }
	, { 10006, IDS_CP_MACINTOSH_GREEK, 0, IDS_CPG_EASTERN_EUROPEAN, IDS_CPP_MACINTOSH }
	, { 10007, IDS_CP_MACINTOSH_CYRILLIC, 0, IDS_CPG_WESTERN_EUROPEAN, IDS_CPP_MACINTOSH }
	, { 10029, IDS_CP_MACINTOSH_CENTRAL_EUROPE, 0, IDS_CPG_EASTERN_EUROPEAN, IDS_CPP_MACINTOSH }
	, { 10079, IDS_CP_MACINTOSH_ICELANDIC, 0, IDS_CPG_WESTERN_EUROPEAN, IDS_CPP_MACINTOSH }
	, { 20880, IDS_CP_IBM_EBCDIC_CYRILLIC, 0, IDS_CPG_EASTERN_EUROPEAN, IDS_CPP_IBM }
	, { 10081, IDS_CP_MACINTOSH_TURKISH, 0, IDS_CPG_MIDDLE_EASTERN, IDS_CPP_MACINTOSH }
	, { 65000, IDS_CP_UTF7, 0, IDS_CPG_NA, IDS_CPP_NA }
	, { 65001, IDS_CP_UTF8, 0, IDS_CPG_NA, IDS_CPP_NA }
};


const codepage_wminfo * getCodepageInfo(int codepage)
{
	initialize();
	int index=-1;
	if (!f_codepage_info_index.Lookup(codepage, index))
		return NULL;
	return &f_codepageinfo[index];
}

/**
 * @brief Load map used for finding codepage in f_codepageinfo
 */
static void initializeCodepageInfo()
{
	for (int i=0; i<sizeof(f_codepageinfo)/sizeof(f_codepageinfo[0]); ++i)
	{
		codepage_wminfo * info = &f_codepageinfo[i];
		f_codepage_info_index[info->codepage] = i;
	}
}

/**
 * @brief Initialize all local information stores (ie, maps)
 */
static void initialize()
{
	if (f_bInitialized) return;
	initializeCodepageStatuses();
	initializeCodepageInfo();
	f_bInitialized = true;
}

/**
 * @brief Populate caller's array with all installed codepages (or all supported codepages)
 */
void getCodepages(CArray<int, int> * pages, bool includeUninstalled)
{
	initialize();

	pages->SetSize(0, f_codepage_status.GetCount()); // make first allocation big enough

	for (POSITION pos = f_codepage_status.GetStartPosition(); pos; )
	{
		int codepage=0, status=0;
		f_codepage_status.GetNextAssoc(pos, codepage, status);
		if (status & 0x01)
		{
			pages->Add(codepage);
		}
		else if (includeUninstalled && (status & 0x10))
		{
			pages->Add(codepage);
		}
	}
}
