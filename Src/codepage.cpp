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
#include "OptionsMgr.h"
#include "Merge.h"
#include "OptionsDef.h"
#include "codepage.h"
#include "DirScan.h" // for DirScan_InitializeDefaultCodepage

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static int f_nDefaultCodepage; // store the default codepage as specified by user in options

static int f_bInitialized = false;

// map number to bit code, 0x01 = supported, 0x10 = installed
static CMap<int, int, int, int> f_codepages;

// map name to number
// eg, "CP-1252" => 1252
static CMap<CString, LPCTSTR, int, int> f_codepageAliases;

/**
 * @brief Update the appropriate default codepage
 */
void updateDefaultCodepage(CRegOptions * options)
{
	int nDefaultCPMode = options->GetInt(OPT_CP_DEFAULT_MODE);

	int wLangId;
	switch (nDefaultCPMode)
	{
		case 0:
			f_nDefaultCodepage = GetACP();
			break;
		case 1:
			TCHAR buff[32];
			wLangId = theApp.m_lang.GetLangId();
			if (GetLocaleInfo(wLangId, LOCALE_IDEFAULTANSICODEPAGE, buff, sizeof(buff)/sizeof(buff[0])))
				f_nDefaultCodepage = _ttol(buff);
			else
				f_nDefaultCodepage = GetACP();
			break;
		case 2:
			f_nDefaultCodepage = options->GetInt(OPT_CP_DEFAULT_CUSTOM);
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
		f_codepages.Lookup(codepage, value);
		value |= 0x10; // installed
		f_codepages[codepage] = value;
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
		f_codepages.Lookup(codepage, value);
		value |= 0x01; // installed
		f_codepages[codepage] = value;
		RecordAliases(lpCodePageString, codepage);
	}
	return TRUE; // continue enumeratino
}

/**
 * @brief Load information about codepages into local cache
 */
static void initializeCodepages()
{
	if (f_bInitialized) return;
	EnumSystemCodePages(EnumInstalledCodePagesProc,  CP_INSTALLED);
	EnumSystemCodePages(EnumSupportedCodePagesProc, CP_SUPPORTED);
	f_bInitialized = true;
}

/**
 * @brief Return true if specified codepage is installed on this system (according to EnumSystemCodePages)
 */
bool isCodepageInstalled(int codepage)
{
	initializeCodepages();
	int value=0;
	return f_codepages.Lookup(codepage, value) && (value & 0x01);
}

/**
 * @brief Return true if specified codepage is supported on this system (according to EnumSystemCodePages)
 */
bool isCodepageSupported(int codepage)
{
	initializeCodepages();
	int value=0;
	return f_codepages.Lookup(codepage, value) && (value & 0x10);
}
