////////////////////////////////////////////////////////////////////////////
//  File:       GhostTextBuffer.cpp
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
#include "GhostTextBuffer.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
#define _ADVANCED_BUGCHECK  1
#endif



BEGIN_MESSAGE_MAP (CGhostTextBuffer, CCrystalTextBuffer)
//{{AFX_MSG_MAP(CGhostTextBuffer)
//}}AFX_MSG_MAP
END_MESSAGE_MAP ()

IMPLEMENT_DYNCREATE (CGhostTextBuffer, CCrystalTextBuffer)

CGhostTextBuffer::CGhostTextBuffer()
{
	m_bUndoGroup = FALSE;
	CCrystalTextBuffer::m_bUndoBeginGroup = m_bUndoBeginGroup = FALSE;
}

BOOL CGhostTextBuffer::
InitNew (int nCrlfStyle /*= CRLF_STYLE_DOS*/ )
{
	m_bUndoBeginGroup = FALSE;
	return CCrystalTextBuffer::InitNew(nCrlfStyle);
}


/** InternalInsertGhostLine accepts only apparent line numbers */
BOOL CGhostTextBuffer::
InternalInsertGhostLine (CCrystalTextView * pSource, int nLine)
{
	ASSERT (m_bInit);             //  Text buffer not yet initialized.
	//  You must call InitNew() or LoadFromFile() first!

	ASSERT (nLine >= 0 && nLine <= m_aLines.GetSize ());
	if (m_bReadOnly)
		return FALSE;

	CInsertContext context;
	context.m_ptStart.x = 0;
	context.m_ptStart.y = nLine;

	InsertLine (_T(""), 0, nLine);

	context.m_ptEnd.x = 0;
	context.m_ptEnd.y = nLine+1;

	if (pSource!=NULL)
		UpdateViews (pSource, &context, UPDATE_HORZRANGE | UPDATE_VERTRANGE, nLine);

	if (!m_bModified)
		SetModified (TRUE);

	OnNotifyLineHasBeenEdited(nLine);

	return TRUE;
}




/**
Get text of specified lines (ghost lines will not contribute text).

CrystalTextBuffer::GetTextWithoutEmptys() is for a buffer with no ghost lines.

CrystalTextBuffer::GetText() returns text including ghost lines.
*/
UINT CGhostTextBuffer::GetTextWithoutEmptys(int nStartLine, int nStartChar, 
                 int nEndLine, int nEndChar, 
                 CString &text, int nCrlfStyle /* CRLF_STYLE_AUTOMATIC */)
{
	int lines = m_aLines.GetSize();
	ASSERT(nStartLine >= 0 && nStartLine < lines);
	ASSERT(nStartChar >= 0 && nStartChar <= GetLineLength(nStartLine));
	ASSERT(nEndLine >= 0 && nEndLine < lines);
	ASSERT(nEndChar >= 0 && nEndChar <= GetFullLineLength(nEndLine));
	ASSERT(nStartLine < nEndLine || nStartLine == nEndLine && 
		nStartChar < nEndChar);

	// estimate size (upper bound)
	int nBufSize = 0;
	for (int i=nStartLine; i<=nEndLine; ++i)
		nBufSize += (GetFullLineLength(i) + 2); // in case we insert EOLs
	LPTSTR pszBuf = text.GetBuffer(nBufSize);

	for (i=nStartLine; i<=nEndLine; ++i)
	{
		int soffset = (i==nStartLine ? nStartChar : 0);
		int eoffset = (i==nEndLine ? nEndChar : GetFullLineLength(i));
		int chars = eoffset - soffset;
		// (Exclude ghost lines, also exclude last line if at position 0)
		if (chars>0)
		{
			LPCTSTR szLine = m_aLines[i].m_pcLine + soffset;
			CopyMemory(pszBuf, szLine, chars * sizeof(TCHAR));
			pszBuf += chars;
			if (i!=ApparentLastRealLine() && GetLineLength(i)==GetFullLineLength(i)
				&& i < nEndLine)
			{
				// Oops, real line lacks EOL
				// (If this happens, editor probably has bug)
				// We don't check it on nEndLine
				ASSERT(0);
				CString sEol = GetStringEol (nCrlfStyle);
				CopyMemory(pszBuf, sEol, sEol.GetLength());
				pszBuf += sEol.GetLength();
			}
		}
	}
	pszBuf[0] = 0;
	text.ReleaseBuffer();
	text.FreeExtra();
	return text.GetLength();
}

////////////////////////////////////////////////////////////////////////////
// undo/redo functions



void CGhostTextBuffer::SUndoRecord::
SetText (LPCTSTR pszText)
{
	FreeText();
	if (pszText != NULL && pszText[0] != _T ('\0'))
	{
		int nLength = _tcslen (pszText);
		if (nLength > 1)
		{
			m_pszText = new TCHAR[(nLength + 1) * sizeof (TCHAR)];
			_tcscpy (m_pszText, pszText);
		}
		else
		{
			m_szText[0] = pszText[0];
		}
	}
}

void CGhostTextBuffer::SUndoRecord::
FreeText ()
{
	// see the m_szText/m_pszText definition about the use of HIWORD
	if (HIWORD ((DWORD) m_pszText) != 0)
		delete[] m_pszText;
	m_pszText = NULL;
}



BOOL CGhostTextBuffer::
Undo (CCrystalTextView * pSource, CPoint & ptCursorPos)
{
	ASSERT (CanUndo ());
	ASSERT ((m_aUndoBuf[0].m_dwFlags & UNDO_BEGINGROUP) != 0);
	BOOL failed = FALSE;
	int tmpPos = m_nUndoPosition;

	while (!failed)
	{
		tmpPos--;;
		SUndoRecord ur = m_aUndoBuf[tmpPos];
		// Undo records are stored in file line numbers
		// and must be converted to apparent (screen) line numbers for use
		CPoint apparent_ptStartPos = ur.m_ptStartPos;
		apparent_ptStartPos.y = ComputeApparentLine(ur.m_ptStartPos.y, ur.m_ptStartPos_nGhost);
		CPoint apparent_ptEndPos = ur.m_ptEndPos;
		apparent_ptEndPos.y = ComputeApparentLine(ur.m_ptEndPos.y, ur.m_ptEndPos_nGhost);

		if (ur.m_ptStartPos_nGhost > 0)
			// if we need a ghost line at position apparent_ptStartPos.y
			if (apparent_ptStartPos.y >= m_aLines.GetSize() || (GetLineFlags(apparent_ptStartPos.y) & LF_GHOST) == 0)
			{
				// if we don't find it, we insert it 
				InsertGhostLine (pSource, apparent_ptStartPos.y);
				// and recompute apparent_ptEndPos
				apparent_ptEndPos.y = ComputeApparentLine (ur.m_redo_ptEndPos.y, ur.m_redo_ptEndPos_nGhost);
			} 

		// EndPos defined only for UNDO_INSERT (when we delete)
		if (ur.m_dwFlags & UNDO_INSERT && ur.m_ptEndPos_nGhost > 0)
			// if we need a ghost line at position apparent_ptStartPos.y
			if (apparent_ptEndPos.y >= m_aLines.GetSize() || (GetLineFlags(apparent_ptEndPos.y) & LF_GHOST) == 0)
			{
				// if we don't find it, we insert it
				InsertGhostLine (pSource, apparent_ptEndPos.y);
			}

		if (ur.m_dwFlags & UNDO_INSERT)
		{
			// WINMERGE -- Check that text in undo buffer matches text in
			// file buffer. If not, then rescan() has moved lines and undo
			// is skipped.

			// we need to put the cursor before the deleted section
			CString text;
			ur.m_redo_ptEndPos.x = apparent_ptEndPos.x;
			ur.m_redo_ptEndPos.y = ComputeRealLineAndGhostAdjustment (apparent_ptEndPos.y, ur.m_redo_ptEndPos_nGhost);

			if ((apparent_ptStartPos.y < m_aLines.GetSize ()) &&
					(apparent_ptStartPos.x <= m_aLines[apparent_ptStartPos.y].m_nLength) &&
					(apparent_ptEndPos.y < m_aLines.GetSize ()) &&
					(apparent_ptEndPos.x <= m_aLines[apparent_ptEndPos.y].m_nLength))
			{
				GetTextWithoutEmptys (apparent_ptStartPos.y, apparent_ptStartPos.x, apparent_ptEndPos.y, apparent_ptEndPos.x, text);
				if (_tcscmp(text, ur.GetText()) == 0)
				{
					VERIFY (CCrystalTextBuffer::DeleteText (pSource, 
						apparent_ptStartPos.y, apparent_ptStartPos.x, apparent_ptEndPos.y, apparent_ptEndPos.x,
						0, TRUE, FALSE));
					ptCursorPos = apparent_ptStartPos;
				}
				else
				{
					//..Try to ensure that we are undoing correctly...
					//  Just compare the text as it was before Undo operation
#ifdef _ADVANCED_BUGCHECK
					ASSERT(0);
#endif
					failed = TRUE;
					break;
				}

			}
			else
			{
				failed = TRUE;
				break;
			}

			OnNotifyLineHasBeenEdited(apparent_ptStartPos.y);

			// set WinMerge flags for first line
			if (ur.m_dwFlags & UNDO_VALID_FIRST)
				SetLineFlag(apparent_ptStartPos.y, LF_GHOST, FALSE, FALSE, FALSE);
			else
				SetLineFlag(apparent_ptStartPos.y, LF_GHOST, TRUE, FALSE, FALSE);

			// it is not easy to know when Recompute so we do it always
			RecomputeRealityMapping();

			RecomputeEOL (pSource, apparent_ptStartPos.y, apparent_ptStartPos.y);
		}
		else
		{
			int nEndLine, nEndChar;
			VERIFY(CCrystalTextBuffer::InsertText (pSource, 
				apparent_ptStartPos.y, apparent_ptStartPos.x, ur.GetText (), nEndLine, nEndChar, 
				0, TRUE, FALSE));
			ptCursorPos = m_ptLastChange;

			// for the flags, the logic is nearly the same as in insertText
			int bFirstLineGhost = ((GetLineFlags(apparent_ptStartPos.y) & LF_GHOST) != 0);
			// when inserting an EOL terminated text into a ghost line,
			// there is a dicrepancy between nInsertedLines and nEndLine-nRealLine
			int bDiscrepancyInInsertedLines;
			if (bFirstLineGhost && nEndChar == 0)
				bDiscrepancyInInsertedLines = TRUE;
			else
				bDiscrepancyInInsertedLines = FALSE;

			int i;
			for (i = apparent_ptStartPos.y ; i < nEndLine ; i++)
				OnNotifyLineHasBeenEdited(i);
			if (bDiscrepancyInInsertedLines == 0)
				OnNotifyLineHasBeenEdited(i);

			// set WinMerge flags for first line
			if (apparent_ptStartPos.y < nEndLine)
				SetLineFlag (apparent_ptStartPos.y, LF_GHOST, FALSE, FALSE, FALSE);
			// set WinMerge flags for last line
			if (bDiscrepancyInInsertedLines == 0)
				// if there is no discrepancy, the cursor line after undo is the same
				// as the last line of the deleted selection
				// because of exception (see deleteText), maybe this line was a ghost one
				// we use UNDO_VALID_LAST to know its original status
				if (ur.m_dwFlags & UNDO_VALID_LAST)
					SetLineFlag(m_ptLastChange.y, LF_GHOST, FALSE, FALSE, FALSE);
				else
					SetLineFlag(m_ptLastChange.y, LF_GHOST, TRUE, FALSE, FALSE);
			else
				// if there is a discrepancy, the final cursor line was not changed during insertion so we do nothing
				;

			// it is not easy to know when Recompute so we do it always
			RecomputeRealityMapping();

			RecomputeEOL (pSource, apparent_ptStartPos.y, nEndLine);
		}

		// store infos for redo
		ur.m_redo_ptStartPos.x = apparent_ptStartPos.x;
		ur.m_redo_ptStartPos.y = ComputeRealLineAndGhostAdjustment( apparent_ptStartPos.y, ur.m_redo_ptStartPos_nGhost);
		if (ur.m_dwFlags & UNDO_INSERT)
			ur.m_redo_ptEndPos = CPoint( -1, 0 );
		else
		{
			ur.m_redo_ptEndPos.x = m_ptLastChange.x;
			ur.m_redo_ptEndPos.y = ComputeRealLineAndGhostAdjustment (m_ptLastChange.y, ur.m_redo_ptEndPos_nGhost);
		}


		m_aUndoBuf[tmpPos] = ur;

		if (ur.m_dwFlags & UNDO_BEGINGROUP)
			break;
	}
	if (m_bModified && m_nSyncPosition == tmpPos)
		SetModified (FALSE);
	if (!m_bModified && m_nSyncPosition != tmpPos)
		SetModified (TRUE);
	if (failed)
	{
		// If the Undo failed, clear the entire Undo/Redo stack
		// Not only can we not Redo the failed Undo, but the Undo
		// may have partially completed (if in a group)
		m_nUndoPosition = 0;
		m_aUndoBuf.SetSize (m_nUndoPosition);
	}
	else
	{
		m_nUndoPosition = tmpPos;
	}
	return !failed;
}

BOOL CGhostTextBuffer::
Redo (CCrystalTextView * pSource, CPoint & ptCursorPos)
{
	ASSERT (CanRedo ());
	ASSERT ((m_aUndoBuf[0].m_dwFlags & UNDO_BEGINGROUP) != 0);
	ASSERT ((m_aUndoBuf[m_nUndoPosition].m_dwFlags & UNDO_BEGINGROUP) != 0);

	while(1)
	{
		SUndoRecord ur = m_aUndoBuf[m_nUndoPosition];
		CPoint apparent_ptStartPos = ur.m_redo_ptStartPos;
		apparent_ptStartPos.y = ComputeApparentLine (ur.m_redo_ptStartPos.y, ur.m_redo_ptStartPos_nGhost);
		CPoint apparent_ptEndPos = ur.m_redo_ptEndPos;
		apparent_ptEndPos.y = ComputeApparentLine (ur.m_redo_ptEndPos.y, ur.m_redo_ptEndPos_nGhost);

		if (ur.m_redo_ptStartPos_nGhost > 0) 
			// we need a ghost line at position apparent_ptStartPos.y
			if (apparent_ptStartPos.y >= m_aLines.GetSize() || (GetLineFlags(apparent_ptStartPos.y) & LF_GHOST) == 0)
			{
				// if we don't find it, we insert it 
				InsertGhostLine (pSource, apparent_ptStartPos.y);
				// and recompute apparent_ptEndPos
				apparent_ptEndPos.y = ComputeApparentLine (ur.m_redo_ptEndPos.y, ur.m_redo_ptEndPos_nGhost);
			} 

		// EndPos defined only for UNDO_DELETE (when we delete)
		if ((ur.m_dwFlags & UNDO_INSERT) == 0 && ur.m_redo_ptEndPos_nGhost > 0)
			// we need a ghost line at position apparent_ptStartPos.y
			if (apparent_ptEndPos.y >= m_aLines.GetSize() || (GetLineFlags(apparent_ptEndPos.y) & LF_GHOST) == 0)
			{
				// if we don't find it, we insert it
				InsertGhostLine (pSource, apparent_ptEndPos.y);
			}

		// now we can use normal (CGhostTextBuffer::) insertTxt or deleteText
		if (ur.m_dwFlags & UNDO_INSERT)
		{
			int nEndLine, nEndChar;
			VERIFY(InsertText (pSource, apparent_ptStartPos.y, apparent_ptStartPos.x,
				ur.GetText(), nEndLine, nEndChar, 0, TRUE, FALSE));
			ptCursorPos = m_ptLastChange;
		}
		else
		{
#ifdef _ADVANCED_BUGCHECK
			CString text;
			GetTextWithoutEmptys (apparent_ptStartPos.y, apparent_ptStartPos.x, apparent_ptEndPos.y, apparent_ptEndPos.x, text);
			ASSERT (lstrcmp (text, ur.GetText ()) == 0);
#endif
			VERIFY(DeleteText(pSource, apparent_ptStartPos.y, apparent_ptStartPos.x, 
				apparent_ptEndPos.y, apparent_ptEndPos.x, 0, TRUE, FALSE));
			ptCursorPos = apparent_ptStartPos;
		}
		m_nUndoPosition++;
		if (m_nUndoPosition == m_aUndoBuf.GetSize ())
			break;
		if ((m_aUndoBuf[m_nUndoPosition].m_dwFlags & UNDO_BEGINGROUP) != 0)
			break;
	}

	if (m_bModified && m_nSyncPosition == m_nUndoPosition)
		SetModified (FALSE);
	if (!m_bModified && m_nSyncPosition != m_nUndoPosition)
		SetModified (TRUE);
	return TRUE;
}


/** 
we must set both our m_bUndoBeginGroup and the one of CCrystalTextBuffer
*/
void CGhostTextBuffer::
BeginUndoGroup (BOOL bMergeWithPrevious /*= FALSE*/ )
{
	ASSERT (!m_bUndoGroup);
	m_bUndoGroup = TRUE;
	m_bUndoBeginGroup = m_nUndoPosition == 0 || !bMergeWithPrevious;
	CCrystalTextBuffer::m_bUndoBeginGroup = m_bUndoBeginGroup;
}

/** Use ou own flushing function as we need to use our own m_aUndoBuf */
void CGhostTextBuffer::
FlushUndoGroup (CCrystalTextView * pSource)
{
	ASSERT (m_bUndoGroup);
	if (pSource != NULL)
	{
		ASSERT (m_nUndoPosition == m_aUndoBuf.GetSize ());
		if (m_nUndoPosition > 0)
		{
			pSource->OnEditOperation (m_aUndoBuf[m_nUndoPosition - 1].m_nAction, m_aUndoBuf[m_nUndoPosition - 1].GetText ());
		}
	}
	m_bUndoGroup = FALSE;
}


/** The CPoint received parameters are apparent (on screen) line numbers */
void CGhostTextBuffer::
AddUndoRecord (BOOL bInsert, const CPoint & ptStartPos, const CPoint & ptEndPos, LPCTSTR pszText, int flags, int nActionType)
{
	//  Forgot to call BeginUndoGroup()?
	ASSERT (m_bUndoGroup);
	ASSERT (m_aUndoBuf.GetSize () == 0 || (m_aUndoBuf[0].m_dwFlags & UNDO_BEGINGROUP) != 0);

	//  Strip unnecessary undo records (edit after undo wipes all potential redo records)
	int nBufSize = m_aUndoBuf.GetSize ();
	if (m_nUndoPosition < nBufSize)
	{
		m_aUndoBuf.SetSize (m_nUndoPosition);
	}

	//  If undo buffer size is close to critical, remove the oldest records
	ASSERT (m_aUndoBuf.GetSize () <= m_nUndoBufSize);
	nBufSize = m_aUndoBuf.GetSize ();
	if (nBufSize >= m_nUndoBufSize)
	{
		int nIndex = 0;
		for (;;)
		{
			nIndex++;
			if (nIndex == nBufSize || (m_aUndoBuf[nIndex].m_dwFlags & UNDO_BEGINGROUP) != 0)
				break;
		}
		m_aUndoBuf.RemoveAt (0, nIndex);

//<jtuc 2003-06-28>
//- Keep m_nSyncPosition in sync.
//- Ensure first undo record is flagged UNDO_BEGINGROUP since part of the code
//..relies on this condition.
		if (m_nSyncPosition >= 0)
		{
			m_nSyncPosition -= nIndex;		// за c'est bien...mais non, test inutile ? Ou Apres !
		}
		if (nIndex < nBufSize)
		{
			// Not really necessary as long as groups are discarded as a whole.
			// Just in case some day the loop above should be changed to limit
			// the number of discarded undo records to some reasonable value...
			m_aUndoBuf[0].m_dwFlags |= UNDO_BEGINGROUP;		// за c'est sale
		}
		else
		{
			// No undo records left - begin a new group:
			m_bUndoBeginGroup = TRUE;
		}
//</jtuc>

	}
	ASSERT (m_aUndoBuf.GetSize () < m_nUndoBufSize);

	//  Add new record
	SUndoRecord ur;
	ur.m_dwFlags = bInsert ? UNDO_INSERT : 0;
	ur.m_dwFlags |= flags; 
	ur.m_nAction = nActionType;
	if (m_bUndoBeginGroup)
	{
		ur.m_dwFlags |= UNDO_BEGINGROUP;
		m_bUndoBeginGroup = FALSE;
	}
	ur.m_ptStartPos = ptStartPos;
	ur.m_ptEndPos = ptEndPos;
	ur.m_ptStartPos.y = ComputeRealLineAndGhostAdjustment( ptStartPos.y, ur.m_ptStartPos_nGhost);
	ur.m_ptEndPos.y = ComputeRealLineAndGhostAdjustment( ptEndPos.y, ur.m_ptEndPos_nGhost);
	ur.SetText (pszText);

	m_aUndoBuf.Add (ur);
	m_nUndoPosition = m_aUndoBuf.GetSize ();

	ASSERT (m_aUndoBuf.GetSize () <= m_nUndoBufSize);
}




////////////////////////////////////////////////////////////////////////////
// edition functions

BOOL CGhostTextBuffer::
InsertText (CCrystalTextView * pSource, int nLine, int nPos, LPCTSTR pszText,
            int &nEndLine, int &nEndChar, int nAction, BOOL bUpdate /*=TRUE*/, BOOL bHistory /*=TRUE*/)
{
	BOOL bGroupFlag = FALSE;
	if (bHistory)
	{
		if (!m_bUndoGroup)
		{
			BeginUndoGroup ();
			bGroupFlag = TRUE;
		} 
	}

	if (!CCrystalTextBuffer::InsertText (pSource, nLine, nPos, pszText, nEndLine, nEndChar, nAction, bUpdate, bHistory))
		return FALSE;

	// set WinMerge flags
	int bFirstLineGhost = ((GetLineFlags(nLine) & LF_GHOST) != 0);

	// when inserting an EOL terminated text into a ghost line,
	// there is a dicrepancy between nInsertedLines and nEndLine-nRealLine
	int bDiscrepancyInInsertedLines;
	if (bFirstLineGhost && nEndChar == 0)
		bDiscrepancyInInsertedLines = TRUE;
	else
		bDiscrepancyInInsertedLines = FALSE;

	int i;
	for (i = nLine ; i < nEndLine ; i++)
		OnNotifyLineHasBeenEdited(i);
	if (bDiscrepancyInInsertedLines == 0)
		OnNotifyLineHasBeenEdited(i);

	int bCursorLineAfterUndoIsGhost;
	if (bDiscrepancyInInsertedLines)
		// if there is a discrepancy, the final cursor line didn't change during insertion
		// so we shall preserve the status of this line during undo
		// (this line will be the cursor line after undo)
		bCursorLineAfterUndoIsGhost = ((GetLineFlags(nEndLine) & LF_GHOST) != 0);
	else
		// no discrepancy, the cursor line after undo must be the same as
		// the insertion point line
		bCursorLineAfterUndoIsGhost = bFirstLineGhost;

	for (i = nLine ; i < nEndLine ; i++)
		SetLineFlag (i, LF_GHOST, FALSE, FALSE, FALSE);
	if (bDiscrepancyInInsertedLines == 0)
		// if there is no discrepancy, the final cursor line is real
		// as either some text was inserted in it, or it inherits the real status from the first line
		SetLineFlag (i, LF_GHOST, FALSE, FALSE, FALSE);
	else
		// if there is a discrepancy, the final cursor line was not changed during insertion so we do nothing
		;

	// now we can recompute
	if ((nEndLine > nLine) || bFirstLineGhost)
	{
		// TODO: Be smarter, and don't recompute if it is easy to see what changed
		RecomputeRealityMapping();
	}

	RecomputeEOL (bUpdate? pSource:NULL, nLine, nEndLine);


	if (bHistory == false)
		return TRUE;


	// little trick as we share the m_nUndoPosition with the base class
	ASSERT (  m_nUndoPosition > 0);
	m_nUndoPosition --;
	AddUndoRecord (TRUE, CPoint (nPos, nLine), CPoint (nEndChar, nEndLine),
                 pszText, bCursorLineAfterUndoIsGhost ? 0 : UNDO_VALID_FIRST, nAction);

	if (bGroupFlag)
		FlushUndoGroup (pSource);
	return TRUE;
}

BOOL CGhostTextBuffer::
DeleteText (CCrystalTextView * pSource, int nStartLine, int nStartChar,
            int nEndLine, int nEndChar, int nAction, BOOL bUpdate /*=TRUE*/, BOOL bHistory /*=TRUE*/)
{
	BOOL bGroupFlag = FALSE;
	if (bHistory)
	{
		if (!m_bUndoGroup)
		{
			BeginUndoGroup ();
			bGroupFlag = TRUE;
		} 
	}


	// flags are going to be deleted so we store them now
	int bLastLineGhost = ((GetLineFlags(nEndLine) & LF_GHOST) != 0);
	int bFirstLineGhost = ((GetLineFlags(nStartLine) & LF_GHOST) != 0);

	CString sTextToDelete;
	GetTextWithoutEmptys (nStartLine, nStartChar, nEndLine, nEndChar, sTextToDelete);
	if (!CCrystalTextBuffer::DeleteText (pSource, nStartLine, nStartChar, nEndLine, nEndChar, nAction, bUpdate, bHistory))
		return FALSE;

	OnNotifyLineHasBeenEdited(nStartLine);

	// the first line inherits the status of the last one 
	// but exception... if the last line is a ghost, we preserve the status of the first line
	// (then if we use backspace in a ghost line, we don't delete the previous line)
	int bFlagException;
	if (bLastLineGhost)
		bFlagException = (bFirstLineGhost == 0);
	if (bLastLineGhost == 0 || bFlagException)
		SetLineFlag(nStartLine, LF_GHOST, FALSE, FALSE, FALSE);
	else
		SetLineFlag(nStartLine, LF_GHOST, TRUE, FALSE, FALSE);

	// now we can recompute
	if (nStartLine != nEndLine)
	{
		// TODO: Be smarter, and don't recompute if it is easy to see what changed
		RecomputeRealityMapping();
	}

	RecomputeEOL (bUpdate? pSource:NULL, nStartLine, nStartLine);


	if (bHistory == false)
		return TRUE;

	// little trick as we share the m_nUndoPosition with the base class
	ASSERT (  m_nUndoPosition > 0);
	m_nUndoPosition --;
	AddUndoRecord (FALSE, CPoint (nStartChar, nStartLine), CPoint (0, -1),
                 sTextToDelete, bLastLineGhost ? 0 : UNDO_VALID_LAST, nAction);

	if (bGroupFlag)
		FlushUndoGroup (pSource);
	return TRUE;
}




BOOL CGhostTextBuffer::
InsertGhostLine (CCrystalTextView * pSource, int nLine)
{
	if (!InternalInsertGhostLine (pSource, nLine))
		return FALSE;

	// set WinMerge flags  
	SetLineFlag (nLine, LF_GHOST, TRUE, FALSE, FALSE);

	RecomputeRealityMapping();

	// don't need to recompute EOL as real lines are unchanged

	// never AddUndoRecord as Rescan clears the ghost lines

	return TRUE;
}

void CGhostTextBuffer::
RemoveAllGhostLines()
{
	for(int ct=GetLineCount()-1; ct>=0; --ct)
		if (GetLineFlags(ct) & LF_GHOST)
			DeleteLine(ct);

	RecomputeRealityMapping();
}



////////////////////////////////////////////////////////////////////////////
// apparent <-> real line conversion

/**
Return apparent line of highest real (file) line. 
Return -1 if no lines.
*/
int CGhostTextBuffer::ApparentLastRealLine() const
{
	int bmax = m_RealityBlocks.GetUpperBound();
	if (bmax<0) return -1;
	const RealityBlock & block = m_RealityBlocks[bmax];
	return block.nStartApparent + block.nCount - 1;
}



/**
Return underlying real line. 
For ghost lines, return NEXT HIGHER real line (for trailing ghost line, return last real line + 1). 
If nApparentLine is greater than the last valid apparent line, ASSERT

ie, lines 0->0, 1->2, 2->4, 
for argument of 3, return 2
*/
int CGhostTextBuffer::ComputeRealLine(int nApparentLine) const
{
	int bmax = m_RealityBlocks.GetUpperBound();
	// first get the degenerate cases out of the way
	// empty file ?
	if (bmax<0)
		return 0;

	// after last apparent line ?
	ASSERT(nApparentLine < GetLineCount());

	// after last block ?
	const RealityBlock & maxblock = m_RealityBlocks[bmax];
	if (nApparentLine >= maxblock.nStartApparent + maxblock.nCount)
		return maxblock.nStartReal + maxblock.nCount;

	// binary search to find correct (or nearest block)
	int blo=0, bhi=bmax;
	int i;
	while (blo<=bhi)
	{
		i = (blo+bhi)/2;
		const RealityBlock & block = m_RealityBlocks[i];
		if (nApparentLine < block.nStartApparent)
			bhi = i-1;
		else if (nApparentLine >= block.nStartApparent + block.nCount)
			blo = i+1;
		else // found it inside this block
			return (nApparentLine - block.nStartApparent) + block.nStartReal;
	}
	// it is a ghost line just before block blo
	return m_RealityBlocks[blo].nStartReal;
}

/**
Return apparent line for this underlying real line. 
If real line is out of bounds, return last valid apparent line + 1
*/
int CGhostTextBuffer::ComputeApparentLine(int nRealLine) const
{
	int bmax = m_RealityBlocks.GetUpperBound();
	// first get the degenerate cases out of the way
	// empty file ?
	if (bmax<0)
		return 0;
	// after last block ?
	const RealityBlock & maxblock = m_RealityBlocks[bmax];
	if (nRealLine >= maxblock.nStartReal + maxblock.nCount)
		return GetLineCount();

	// binary search to find correct (or nearest block)
	int blo=0, bhi=bmax;
	int i;
	while (blo<=bhi)
	{
		i = (blo+bhi)/2;
		const RealityBlock & block = m_RealityBlocks[i];
		if (nRealLine < block.nStartReal)
			bhi = i-1;
		else if (nRealLine >= block.nStartReal + block.nCount)
			blo = i+1;
		else
			return (nRealLine - block.nStartReal) + block.nStartApparent;
	}
	// Should have found it; all real lines should be in a block
	ASSERT(0);
	return -1;
}

/**
Return underlying real line and ghost adjustment 
as nApparentLine = apparent(nRealLine) - nGhostAdjustment 

nRealLine for ghost lines is the NEXT HIGHER real line (for trailing ghost line, last real line + 1).
If nApparentLine is greater than the last valid apparent line, ASSERT

ie, lines 0->0, 1->2, 2->4,  
for argument of 3, return 2, and decToReal = 1
*/
int CGhostTextBuffer::ComputeRealLineAndGhostAdjustment(int nApparentLine, int& decToReal) const
{
	int bmax = m_RealityBlocks.GetUpperBound();
	// first get the degenerate cases out of the way
	// empty file ?
	if (bmax<0) 
	{
		decToReal = 0;
		return 0;
	}

	// after last apparent line ?
	ASSERT(nApparentLine < GetLineCount());

	// after last block ?
	const RealityBlock & maxblock = m_RealityBlocks[bmax];
	if (nApparentLine >= maxblock.nStartApparent + maxblock.nCount)
	{
		decToReal = GetLineCount() - nApparentLine;
		return maxblock.nStartReal + maxblock.nCount;
	}

	// binary search to find correct (or nearest block)
	int blo=0, bhi=bmax;
	int i;
	while (blo<=bhi)
	{
		i = (blo+bhi)/2;
		const RealityBlock & block = m_RealityBlocks[i];
		if (nApparentLine < block.nStartApparent)
			bhi = i-1;
		else if (nApparentLine >= block.nStartApparent + block.nCount)
			blo = i+1;
		else // found it inside this block
		{
			decToReal = 0;
			return (nApparentLine - block.nStartApparent) + block.nStartReal;
		}
	}
	// it is a ghost line just before block blo
	decToReal = m_RealityBlocks[blo].nStartApparent - nApparentLine;
	return m_RealityBlocks[blo].nStartReal;
}

/**
Return apparent line for this underlying real line, with adjustment : 
nApparent = apparent(nReal) - decToReal

If the previous real line has apparent number   apparent(nReal) - dec, with dec < decToReal, 
return apparent(nReal) - dec + 1
*/
int CGhostTextBuffer::ComputeApparentLine(int nRealLine, int decToReal) const
{
	int blo, bhi;
	int nPreviousBlock;
	int nApparent;
	int bmax = m_RealityBlocks.GetUpperBound();
	// first get the degenerate cases out of the way
	// empty file ?
	if (bmax<0)
		return 0;
	// after last block ?
	const RealityBlock & maxblock = m_RealityBlocks[bmax];
	if (nRealLine >= maxblock.nStartReal + maxblock.nCount)
	{
		nPreviousBlock = bmax;
		nApparent = GetLineCount();
		goto limitWithPreviousBlock;
	}

	// binary search to find correct (or nearest block)
	blo=0;
	bhi=bmax;
	int i;
	while (blo<=bhi)
	{
		i = (blo+bhi)/2;
		const RealityBlock & block = m_RealityBlocks[i];
		if (nRealLine < block.nStartReal)
			bhi = i-1;
		else if (nRealLine >= block.nStartReal + block.nCount)
			blo = i+1;
		else
		{
			if (nRealLine > block.nStartReal)
				// limited by the previous line in this block
				return (nRealLine - block.nStartReal) + block.nStartApparent;
			nPreviousBlock = i - 1;
			nApparent = (nRealLine - block.nStartReal) + block.nStartApparent;
			goto limitWithPreviousBlock;
		}
	}
	// Should have found it; all real lines should be in a block
	ASSERT(0);
	return -1;

limitWithPreviousBlock:
	// we must keep above the value lastApparentInPreviousBlock
	int lastApparentInPreviousBlock;
	if (nPreviousBlock == -1)
		lastApparentInPreviousBlock = -1;
	else
	{
		const RealityBlock & previousBlock = m_RealityBlocks[nPreviousBlock];
		lastApparentInPreviousBlock = previousBlock.nStartApparent + previousBlock.nCount - 1;
	}

	while (decToReal --) 
	{
		nApparent --;
		if (nApparent == lastApparentInPreviousBlock)
			return nApparent+1;
	}
	return nApparent;
}


/** Do what we need to do just after we've been reloaded */
void CGhostTextBuffer::FinishLoading()
{
	if (!m_bInit) return;
	RecomputeRealityMapping();
}

/** Recompute the reality mapping (this is fairly naive) */
void CGhostTextBuffer::RecomputeRealityMapping()
{
	m_RealityBlocks.RemoveAll();
	int reality=-1; // last encountered real line
	int i=0; // current line
	RealityBlock block; // current block being traversed (in state 2)

	// This is a state machine with 2 states

	// state 1, i-1 not real line
passingGhosts:
	if (i==GetLineCount())
		return;
	if (GetLineFlags(i) & LF_GHOST)
	{
		++i;
		goto passingGhosts;
	}
	// this is the first line of a reality block
	block.nStartApparent = i;
	block.nStartReal = reality+1;
	++reality;
	++i;
	// fall through to other state

	// state 2, i-1 is real line
inReality:
	if (i==GetLineCount() || (GetLineFlags(i) & LF_GHOST))
	{
		// i-1 is the last line of a reality block
		ASSERT(reality >= 0);
		block.nCount = i - block.nStartApparent;
		ASSERT(block.nCount > 0);
		ASSERT(reality+1-block.nStartReal == block.nCount);
		m_RealityBlocks.Add(block);
		if (i==GetLineCount())
			return;
		++i;
		goto passingGhosts;
	}
	++reality;
	++i;
	goto inReality;
}

/** we recompute EOL from the real line before nStartLine to nEndLine */
void CGhostTextBuffer::RecomputeEOL(CCrystalTextView * pSource, int nStartLine, int nEndLine)
{
	if (ApparentLastRealLine() <= nEndLine)
	{
		// EOL may have to change on the real line before nStartLine
		int nRealBeforeStart;
		for (nRealBeforeStart = nStartLine-1 ; nRealBeforeStart >= 0 ; nRealBeforeStart--)
			if ((GetLineFlags(nRealBeforeStart) & LF_GHOST) == 0)
				break;
		if (nRealBeforeStart >= 0)
			nStartLine = nRealBeforeStart;
	}
	int bLastRealLine = (ApparentLastRealLine() <= nEndLine);
	int i;
	for (i = nEndLine ; i >= nStartLine ; i --)
	{
		if ((GetLineFlags(i) & LF_GHOST) == 0)
		{
			if (bLastRealLine)
			{
				bLastRealLine = 0;
				if (m_aLines[i].m_nEolChars != 0) 
				{
					// if the last real line has an EOL, remove it
					m_aLines[i].m_pcLine[m_aLines[i].m_nLength] = '\0';
					m_aLines[i].m_nEolChars = 0;
					if (pSource!=NULL)
						UpdateViews (pSource, NULL, UPDATE_HORZRANGE | UPDATE_SINGLELINE, i);
				}
			}
			else
			{
				if (m_aLines[i].m_nEolChars == 0) 
				{
					// if a real line (not the last) has no EOL, add one
					AppendLine (i, GetDefaultEol(), _tcslen(GetDefaultEol()));
					if (pSource!=NULL)
						UpdateViews (pSource, NULL, UPDATE_HORZRANGE | UPDATE_SINGLELINE, i);
				}
			}
		}
		else 
		{
			if (m_aLines[i].m_nEolChars != 0) 
			{
				// if a ghost line has an EOL, remove it
				m_aLines[i].m_pcLine[m_aLines[i].m_nLength] = '\0';
				m_aLines[i].m_nEolChars = 0;
				if (pSource!=NULL)
					UpdateViews (pSource, NULL, UPDATE_HORZRANGE | UPDATE_SINGLELINE, i);
			}
		}
	}
}

/** 
Check all lines, and ASSERT if reality blocks differ from flags. 
This means that this only has effect in DEBUG build
*/
void CGhostTextBuffer::checkFlagsFromReality(BOOL bFlag) const
{
	int bmax = m_RealityBlocks.GetUpperBound();
	int b;
	int i = 0;
	for (b = 0 ; b <= bmax ; b ++)
	{
		const RealityBlock & block = m_RealityBlocks[b];
		for ( ; i < block.nStartApparent ; i++)
			ASSERT ((GetLineFlags(i) & LF_GHOST) != 0);
		for ( ; i < block.nStartApparent+block.nCount ; i++)
			ASSERT ((GetLineFlags(i) & LF_GHOST) == 0);
	}

	for ( ; i < GetLineCount() ; i++)
		ASSERT ((GetLineFlags(i) & LF_GHOST) != 0);
}


void CGhostTextBuffer::OnNotifyLineHasBeenEdited(int nLine)
{
	return;
}


