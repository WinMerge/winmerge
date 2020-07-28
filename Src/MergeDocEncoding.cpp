/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  MergeDocEncoding.cpp
 *
 * @brief Implementation file for CMergeDoc
 *
 */

#include "stdafx.h"
#include "MergeDoc.h"
#include "LoadSaveCodepageDlg.h"
#include "unicoder.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
 * @brief Display file encoding dialog to user & handle user's choices
 */
bool CMergeDoc::DoFileEncodingDialog()
{
	if (!PromptAndSaveIfNeeded(true))
		return false;
	
	CLoadSaveCodepageDlg dlg(m_nBuffers);
	dlg.EnableSaveCodepage(true);
	dlg.SetCodepages(m_ptBuf[0]->getCodepage());
	if (IDOK != dlg.DoModal())
		return false;

	bool doLeft = dlg.DoesAffectLeft();
	bool doMiddle = dlg.DoesAffectMiddle();
	bool doRight = dlg.DoesAffectRight();
	FileLocation fileloc[3];
	bool bRO[3];
	for (int pane = 0; pane < m_nBuffers; pane++)
	{
		bRO[pane] = m_ptBuf[pane]->GetReadOnly();
		if ((pane == 0 && doLeft) ||
		    (pane == 1 && doRight  && m_nBuffers <  3) ||
		    (pane == 1 && doMiddle && m_nBuffers == 3) ||
		    (pane == 2 && doRight  && m_nBuffers == 3))
		{
			fileloc[pane].encoding.m_unicoding = ucr::NONE;
			fileloc[pane].encoding.m_codepage = dlg.GetLoadCodepage();
		}
		else
		{
			fileloc[pane].encoding.m_unicoding = m_ptBuf[pane]->getUnicoding();
			fileloc[pane].encoding.m_codepage = m_ptBuf[pane]->getCodepage();
		}
		fileloc[pane].setPath(m_filePaths[pane]);
	}
	OpenDocs(m_nBuffers, fileloc, bRO, m_strDesc);
	
	if (dlg.GetSaveCodepage() != dlg.GetLoadCodepage())
	{
		int nSaveCodepage = dlg.GetSaveCodepage();
		for (int pane = 0; pane < m_nBuffers; pane++)
		{
			bRO[pane] = m_ptBuf[pane]->GetReadOnly();
			if (!bRO[pane] && (
				(pane == 0 && doLeft) ||
				(pane == 1 && doRight  && m_nBuffers <  3) ||
				(pane == 1 && doMiddle && m_nBuffers == 3) ||
				(pane == 2 && doRight  && m_nBuffers == 3)
				))
			{
				m_ptBuf[pane]->setCodepage(nSaveCodepage);
				m_ptBuf[pane]->SetModified();
				UpdateHeaderPath(pane);
			}
		}
	}
	return true;
}

