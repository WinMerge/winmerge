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
// DiffContext.cpp: implementation of the CDiffContext class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "merge.h"
#include "DiffContext.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDiffContext::CDiffContext(LPCTSTR pszLeft /*=NULL*/, LPCTSTR pszRight /*=NULL*/)
{
	m_bRecurse=FALSE;
	m_strLeft = pszLeft;
	m_strRight = pszRight;
	m_pList = &m_dirlist;

	pNamesLeft = NULL;
	pNamesRight = NULL;
}

CDiffContext::CDiffContext(LPCTSTR pszLeft, LPCTSTR pszRight, CDiffContext& src)
{
	m_bRecurse=src.m_bRecurse;
	m_strLeft = pszLeft;
	m_strRight = pszRight;
	m_pList = src.m_pList;
	SetRegExp(src.m_strRegExp);

	pNamesLeft = NULL;
	pNamesRight = NULL;
}

CDiffContext::~CDiffContext()
{
	if (pNamesLeft != NULL)
		free(pNamesLeft);
	if (pNamesRight != NULL)
		free(pNamesRight);
}
	

void CDiffContext::AddDiff(LPCTSTR pszFilename, LPCTSTR pszLeftDir, LPCTSTR pszRightDir, BYTE code)
{
	DIFFITEM di;
	strcpy(di.filename,pszFilename);
	strcpy(di.lpath, pszLeftDir);
	strcpy(di.rpath, pszRightDir);
	di.code = code;
	m_pList->AddTail(di);
}

void CDiffContext::SetRegExp(LPCTSTR pszExp)
{
	m_strRegExp = pszExp;
	m_rgx.RegComp( pszExp );
}
