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


static int m_nDefaultCodepage;

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
			m_nDefaultCodepage = GetACP();
			break;
		case 1:
			TCHAR buff[32];
			wLangId = theApp.m_lang.GetLangId();
			if (GetLocaleInfo(wLangId, LOCALE_IDEFAULTANSICODEPAGE, buff, sizeof(buff)/sizeof(buff[0])))
				m_nDefaultCodepage = _ttol(buff);
			else
				m_nDefaultCodepage = GetACP();
			break;
		case 2:
			m_nDefaultCodepage = options->GetInt(OPT_CP_DEFAULT_CUSTOM);
			break;
		default:
			// no other valid option
			ASSERT (0);
			m_nDefaultCodepage = GetACP();
	}

	// Set default codepage
	DirScan_InitializeDefaultCodepage();
}


/**
 * @brief Get appropriate default codepage 
 */
int getDefaultCodepage()
{
	return m_nDefaultCodepage;
}

