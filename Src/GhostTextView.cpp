////////////////////////////////////////////////////////////////////////////
//  File:       GhostTextView.h
//  Version:    1.0.0.0
//  Created:    31-Jul-2003
//
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


#include "stdafx.h"
#include "GhostTextView.h"
#include "GhostTextBuffer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE (CGhostTextView, CCrystalEditViewEx)



void CGhostTextView::
AttachToBuffer (CCrystalTextBuffer * pBuf /*= NULL*/ )
{
	if (pBuf == NULL)
	{
		pBuf = LocateTextBuffer ();
		// ...
	}
	m_pGhostTextBuffer = dynamic_cast<CGhostTextBuffer*> (pBuf);
	CCrystalEditViewEx::AttachToBuffer(pBuf);
}

void CGhostTextView::
DetachFromBuffer ()
{
	if (m_pGhostTextBuffer != NULL)
		m_pGhostTextBuffer = NULL;
	CCrystalEditViewEx::DetachFromBuffer();
}








void CGhostTextView::
PopCursor ()
{
	CPoint ptCursorLast = m_ptCursorLast;
	ptCursorLast.y = m_pGhostTextBuffer->ComputeApparentLine(m_ptCursorLast.y, m_ptCursorLast_nGhost);
	if (ptCursorLast.y >= GetLineCount())
	{
		ptCursorLast.y = GetLineCount()-1;
		ptCursorLast.x = GetLineLength(ptCursorLast.y);
	}
	ASSERT_VALIDTEXTPOS (ptCursorLast);
	SetCursorPos (ptCursorLast);
	SetSelection (ptCursorLast, ptCursorLast);
	SetAnchor (ptCursorLast);
}

void CGhostTextView::
PushCursor ()
{
	m_ptCursorLast.x = m_ptCursorPos.x;
	m_ptCursorLast.y = m_pGhostTextBuffer->ComputeRealLineAndGhostAdjustment(m_ptCursorPos.y, m_ptCursorLast_nGhost);
}


int CGhostTextView::ComputeRealLine (int nApparentLine) const
{
	return m_pGhostTextBuffer->ComputeRealLine(nApparentLine);
}

int CGhostTextView::ComputeApparentLine (int nRealLine) const
{
	return m_pGhostTextBuffer->ComputeApparentLine(nRealLine);
}






