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
 * @file  MergeDoc.cpp
 *
 * @brief Implementation file for CMergeDoc
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"

#include "MainFrm.h"
#include "MergeDoc.h"
#include "LoadSaveCodepageDlg.h"
#include "unicoder.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/**
 * @brief Display file encoding dialog to user & handle user's choices
 */
void CMergeDoc::DoFileEncodingDialog()
{
	if (!PromptAndSaveIfNeeded(TRUE))
		return;
	
	CLoadSaveCodepageDlg dlg;
	dlg.SetCodepages(m_ptBuf[0]->getCodepage());
	if (IDOK != dlg.DoModal())
		return;

	bool doLeft = dlg.DoesAffectLeft();
	bool doRight = dlg.DoesAffectRight();
	FileLocation filelocLeft, filelocRight;
	BOOL bROLeft = m_ptBuf[0]->GetReadOnly();
	BOOL bRORight = m_ptBuf[1]->GetReadOnly();
	if (doLeft)
	{
		filelocLeft.encoding.m_unicoding = ucr::NONE;
		filelocLeft.encoding.m_codepage = dlg.GetLoadCodepage();
	}
	else
	{
		filelocLeft.encoding.m_unicoding = m_ptBuf[0]->getUnicoding();
		filelocLeft.encoding.m_codepage = m_ptBuf[0]->getCodepage();
	}
	if (doRight)
	{
		filelocRight.encoding.m_unicoding = ucr::NONE;
		filelocRight.encoding.m_codepage = dlg.GetLoadCodepage();
	}
	else
	{
		filelocRight.encoding.m_unicoding = m_ptBuf[1]->getUnicoding();
		filelocRight.encoding.m_codepage = m_ptBuf[1]->getCodepage();
	}
	filelocLeft.setPath(m_filePaths.GetLeft().c_str());
	filelocRight.setPath(m_filePaths.GetRight().c_str());
	GetMainFrame()->m_strDescriptions[0] = m_strDesc[0];
	GetMainFrame()->m_strDescriptions[1] = m_strDesc[1];
	OpenDocs(filelocLeft, filelocRight, bROLeft, bRORight);
}

