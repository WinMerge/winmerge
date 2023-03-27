////////////////////////////////////////////////////////////////////////////
//  File:       GhostTextBuffer.cpp
//  Version:    1.0.0.0
//  Created:    31-Jul-2003
//
/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  GhostTextBuffer.cpp
 *
 * @brief Implementation of GhostTextBuffer class.
 */

#include "StdAfx.h"
#include "GhostTextBuffer.h"
#include "ccrystaltextview.h"
#include "MergeLineFlags.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using std::vector;

/**
 * @brief Constructor.
 */
CGhostTextBuffer::CGhostTextBuffer()
{
}

#if 0
/**
 * @brief Insert a ghost line.
 * @param [in] pSource View into which to insert the line.
 * @param [in] nLine Line (apparent/screen) where to insert the ghost line.
 * @return true if the insertion succeeded, false otherwise.
 */
bool CGhostTextBuffer::InternalInsertGhostLine (CCrystalTextView * pSource,
		int nLine)
{
	ASSERT (m_bInit);             //  Text buffer not yet initialized.
	//  You must call InitNew() or LoadFromFile() first!

	ASSERT (nLine >= 0 && nLine <= static_cast<intptr_t>(m_aLines.size ()));

	CInsertContext context;
	context.m_ptStart.x = 0;
	context.m_ptStart.y = nLine;
	context.m_ptEnd.x = 0;
	context.m_ptEnd.y = nLine + 1;

	CCrystalTextBuffer::InsertLine (_T(""), 0, nLine);
	if (pSource != nullptr)
		UpdateViews (pSource, &context, UPDATE_HORZRANGE | UPDATE_VERTRANGE, nLine);

	return true;
}
#endif

/** InternalDeleteGhostLine accepts only apparent line numbers */
/**
 * @brief Delete a group of ghost lines.
 * @param [in] pSource View from which to delete the lines.
 * @param [in] nLine Line index where to delete the first ghost line.
 * @param [in] nCount the number of ghost lines to delete
 * @return true if the deletion succeeded, false otherwise.
 * @note @p nLine must be an apparent line number (ghost lines added).
 */
bool CGhostTextBuffer::InternalDeleteGhostLine (CCrystalTextView * pSource,
		int nLine, int nCount)
{
	ASSERT (m_bInit);             //  Text buffer not yet initialized.
	//  You must call InitNew() or LoadFromFile() first!
	ASSERT (nCount >= 0);
	ASSERT (nLine >= 0 && (nLine + nCount) <= static_cast<intptr_t>(m_aLines.size ()));

	if (nCount == 0)
		return true;

	for (int i = nLine ; i < nLine + nCount; i++)
	{
		ASSERT ( (GetLineFlags(i) & LF_GHOST) != 0 );
		m_aLines[i].Clear();
	}

	vector<LineInfo>::iterator iterBegin = m_aLines.begin() + nLine;
	vector<LineInfo>::iterator iterEnd = iterBegin + nCount;
	m_aLines.erase(iterBegin, iterEnd);

	if (pSource != nullptr)
	{
		CDeleteContext context;
		context.m_ptStart.y = nLine;
		context.m_ptStart.x = 0;
		context.m_ptEnd.y = nLine + nCount;
		context.m_ptEnd.x = 0;

		if (nLine == GetLineCount())
			nLine--;
		// The last parameter is optimization  
		//   - don't recompute lines preceding the removed line.
		UpdateViews (pSource, &context, UPDATE_HORZRANGE | UPDATE_VERTRANGE,
				nLine);
	}

	return true;
}

/**
 * @brief Get text of specified lines (ghost lines will not contribute to text).
 * 
 * @param nCrlfStyle determines the EOL type in the returned buffer.
 * If nCrlfStyle equals CRLFSTYLE::AUTOMATIC, we read the EOL from the line buffer
 * 
 * @note This function has its base in CrystalTextBuffer
 * CrystalTextBuffer::GetTextWithoutEmptys() is for a buffer with no ghost lines.
 * CrystalTextBuffer::GetText() returns text including ghost lines.
 * These two base functions never read the EOL from the line buffer, they
 * use CRLFSTYLE::DOS when nCrlfStyle equals CRLFSTYLE::AUTOMATIC.
 */
void CGhostTextBuffer::			/* virtual override */
GetTextWithoutEmptys(int nStartLine, int nStartChar, 
                 int nEndLine, int nEndChar, 
                 String &text, CRLFSTYLE nCrlfStyle /*= CRLFSTYLE::AUTOMATIC */,
                 bool bExcludeInvisibleLines /*= true*/) const
{
	const size_t lines = m_aLines.size();
	ASSERT(nStartLine >= 0 && nStartLine < static_cast<intptr_t>(lines));
	ASSERT(nStartChar >= 0 && nStartChar <= GetLineLength(nStartLine));
	ASSERT(nEndLine >= 0 && nEndLine < static_cast<intptr_t>(lines));
	ASSERT(nEndChar >= 0 && nEndChar <= GetFullLineLength(nEndLine));
	ASSERT(nStartLine < nEndLine || nStartLine == nEndLine && nStartChar <= nEndChar);
	// some edit functions (copy...) should do nothing when there is no selection.
	// assert to be sure to catch these 'do nothing' cases.
//	ASSERT(nStartLine != nEndLine || nStartChar != nEndChar);

	// estimate size (upper bound)
	size_t nBufSize = 0;
	int i = 0;
	for (i = nStartLine; i <= nEndLine; ++i)
		nBufSize += (GetFullLineLength(i) + 2); // in case we insert EOLs
	text.resize(nBufSize);
	tchar_t* pszBuf = text.data();

	if (nCrlfStyle != CRLFSTYLE::AUTOMATIC)
	{
		// we must copy this EOL type only
		const CString sEol = GetStringEol (nCrlfStyle);

		for (i = nStartLine; i <= nEndLine; ++i)
		{
			// exclude ghost lines
			if ((GetLineFlags(i) & LF_GHOST) || (bExcludeInvisibleLines && (GetLineFlags(i) & LF_INVISIBLE)))
				continue;

			// copy the line, excluding the EOL
			int soffset = (i == nStartLine ? nStartChar : 0);
			int eoffset = (i == nEndLine ? nEndChar : GetLineLength(i));
			int chars = eoffset - soffset;
			const tchar_t* szLine = m_aLines[i].GetLine(soffset);
			CopyMemory(pszBuf, szLine, chars * sizeof(tchar_t));
			pszBuf += chars;

			// copy the EOL of the requested type
			if (i != ApparentLastRealLine())
			{
				CopyMemory(pszBuf, sEol, sEol.GetLength() * sizeof(tchar_t));
				pszBuf += sEol.GetLength();
			}
		}
	} 
	else 
	{
		for (i = nStartLine; i <= nEndLine; ++i)
		{
			// exclude ghost lines
			if ((GetLineFlags(i) & LF_GHOST) || (bExcludeInvisibleLines && GetLineFlags(i) & LF_INVISIBLE))
				continue;

			// copy the line including the EOL
			int soffset = (i == nStartLine ? nStartChar : 0);
			int eoffset = (i == nEndLine ? nEndChar : GetFullLineLength(i));
			int chars = eoffset - soffset;
			const tchar_t* szLine = m_aLines[i].GetLine(soffset);
			CopyMemory(pszBuf, szLine, chars * sizeof(tchar_t));
			pszBuf += chars;

			// check that we really have an EOL
			if (i != ApparentLastRealLine() && GetLineLength(i) == GetFullLineLength(i))
			{
				// Oops, real line lacks EOL
				// (If this happens, editor probably has bug)
				ASSERT(false);
				CString sEol = GetStringEol (nCrlfStyle);
				CopyMemory(pszBuf, sEol, sEol.GetLength() * sizeof(tchar_t));
				pszBuf += sEol.GetLength();
			}
		}
	}
	text.resize(pszBuf - text.data());
}

////////////////////////////////////////////////////////////////////////////
// edition functions

/**
 * @brief Insert text to the buffer.
 * @param [in] pSource View into which to insert the text.
 * @param [in] nLine Line number (apparent/screen) where the insertion starts.
 * @param [in] nPos Character position where the insertion starts.
 * @param [in] pszText The text to insert.
 * @param [in] cchText The length of text in pszText.
 * @param [out] nEndLine Line number of last added line in the buffer.
 * @param [out] nEndChar Character position of the end of the added text
 *   in the buffer.
 * @param [in] nAction Edit action.
 * @param [in] bHistory Save insertion for undo/redo?
 * @return true if the insertion succeeded, false otherwise.
 * @note Line numbers are apparent (screen) line numbers, not real
 * line numbers in the file.
 * @note @p nEndLine and @p nEndChar are valid as long as you do not call
 *   FlushUndoGroup. If you need to call FlushUndoGroup, just store them in a
 *   variable which is preserved with real line number during Rescan
 *   (m_ptCursorPos, m_ptLastChange for example).
 */
bool CGhostTextBuffer::			/* virtual override */
InsertText (CCrystalTextView * pSource, int nLine,
		int nPos, const tchar_t* pszText, size_t cchText, int &nEndLine, int &nEndChar,
		int nAction /*= CE_ACTION_UNKNOWN*/, bool bHistory /*= true*/)
{
	bool bGroupFlag = false;
	bool bFirstLineGhost = ((GetLineFlags(nLine) & LF_GHOST) != 0);
	bool bSpecialLastLineHandling = bFirstLineGhost && (nLine == GetLineCount()-1);

	if (bFirstLineGhost && cchText > 0)
	{
		CString text = GetStringEol(GetCRLFMode());
		if (bHistory && !m_bUndoGroup)
		{
			BeginUndoGroup();
			bGroupFlag = true;
		}
		auto reverseFindRealLine = [&](int nLine) {
			for (; nLine >= 0; --nLine) { if ((GetLineFlags(nLine) & LF_GHOST) == 0) break; }
			return nLine;
		};
		int i = reverseFindRealLine(nLine);
		if (i >= 0 && !m_aLines[i].HasEol())
			CCrystalTextBuffer::InsertText(pSource, i, GetLineLength(i), text, text.GetLength(), nEndLine, nEndChar, 0, bHistory);
		else if (!LineInfo::IsEol(pszText[cchText - 1]))
		{
			auto findRealLine = [&](int nLine) {
				for (; nLine < GetLineCount(); ++nLine) { if ((GetLineFlags(nLine) & LF_GHOST) == 0) break; }
				if (nLine == GetLineCount())
					return -1;
				return nLine;
			};
			if (findRealLine(nLine) != -1)
				CCrystalTextBuffer::InsertText(pSource, nLine, 0, text, text.GetLength(), nEndLine, nEndChar, 0, bHistory);
		}
	}

	if (!CCrystalTextBuffer::InsertText (pSource, nLine, nPos, pszText,
		cchText, nEndLine, nEndChar, nAction, bHistory))
	{
		return false;
	}

	// when inserting an EOL terminated text into a ghost line,
	// there is a discrepancy between nInsertedLines and nEndLine-nRealLine
	bool bDiscrepancyInInsertedLines;
	if (bFirstLineGhost && nEndChar == 0 && ApparentLastRealLine() >= nEndLine)
		bDiscrepancyInInsertedLines = true;
	else
		bDiscrepancyInInsertedLines = false;

	if (bSpecialLastLineHandling)
	{
		// The special case of inserting text into the very last line of a file when
		//	that last line is marked as LF_GHOST.  Effectively, the new text is 
		//	supposed to go "before" the Ghost, but mechanically the text is inserted
		//	into the Ghost itself, with a new Ghost line appearing at the end of the
		//	file.  Later (below), the Ghost status of both the first and last inserted 
		//	lines will get straightened out, with the trailing Ghost line becomming  
		//	a NULL line.
		if ((GetLineFlags(nLine) & LF_GHOST) != 0)	// first line still marked GHOST
			bSpecialLastLineHandling = false;
		else
			bDiscrepancyInInsertedLines = false;
	}

	// compute the number of real lines created (for undo)
	int nRealLinesCreated = nEndLine - nLine;
	if (bFirstLineGhost && (nEndChar > 0 || ApparentLastRealLine() < nEndLine))
		// we create one more real line
		nRealLinesCreated ++;

	int i;
	for (i = nLine ; i < nEndLine ; i++)
	{
		// update line revision numbers of modified lines
		m_aLines[i].m_dwRevisionNumber = m_dwCurrentRevisionNumber;
		OnNotifyLineHasBeenEdited(i);
	}
	if (!bDiscrepancyInInsertedLines)
	{
		m_aLines[i].m_dwRevisionNumber = m_dwCurrentRevisionNumber;
		OnNotifyLineHasBeenEdited(i);
	}

	// when inserting into a ghost line block, we want to replace ghost lines
	// with our text, so delete some ghost lines below the inserted text
	if (bFirstLineGhost)
	{
		// where is the first line after the inserted text ?
		int nInsertedTextLinesCount = nEndLine - nLine + (bDiscrepancyInInsertedLines ? 0 : 1);
		int nLineAfterInsertedBlock = nLine + nInsertedTextLinesCount;
		// delete at most nInsertedTextLinesCount - 1 ghost lines
		// as the first ghost line has been reused
		int nMaxGhostLineToDelete = min(nInsertedTextLinesCount - 1, GetLineCount()-nLineAfterInsertedBlock);
		if (nEndChar == 0 && ApparentLastRealLine() < nEndLine)
			nMaxGhostLineToDelete --;
		for (i = 0 ; i < nMaxGhostLineToDelete ; i++)
			if ((GetLineFlags(nLineAfterInsertedBlock+i) & LF_GHOST) == 0)
				break;
		InternalDeleteGhostLine(pSource, nLineAfterInsertedBlock, i);
	}

	for (i = nLine ; i < nEndLine ; i++)
		SetLineFlag (i, LF_GHOST, false, false, false);
	if (!bDiscrepancyInInsertedLines)
		// if there is no discrepancy, the final cursor line is real
		// as either some text was inserted in it, or it inherits the real status from the first line
		SetLineFlag (i, LF_GHOST, false, false, false);
	else
		// if there is a discrepancy, the final cursor line was not changed during insertion so we do nothing
		;
		
	if (bSpecialLastLineHandling)
	{
		// By setting the last line (in this special case, see above) to `nullptr`, 
		//	the line will eventually be removed or become an actual LF_GHOST line.
		int nLastLine = GetLineCount()-1;
		ASSERT(m_aLines[nLastLine].FullLength() == 0);
		m_aLines[nLastLine].Clear();
	}

	// now we can recompute
	if ((nEndLine > nLine) || bFirstLineGhost)
	{
		// TODO: Be smarter, and don't recompute if it is easy to see what changed
		RecomputeRealityMapping();
	}

	if (bGroupFlag)
		FlushUndoGroup (pSource);

	// nEndLine may have changed during Rescan
	nEndLine = m_ptLastChange.y;

	return true;
}

std::vector<uint32_t> *CGhostTextBuffer::			/* virtual override */
CopyRevisionNumbers(int nStartLine, int nEndLine) const
{
	std::vector<uint32_t> *paSavedRevisionNumbers = CCrystalTextBuffer::CopyRevisionNumbers(nStartLine, nEndLine);
	for (int nLine = nEndLine; nLine >= nStartLine; --nLine)
	{
		if ((GetLineFlags(nLine) & LF_GHOST) != 0)
			paSavedRevisionNumbers->erase(paSavedRevisionNumbers->begin() + (nLine - nStartLine));
	}
	if ((GetLineFlags(nEndLine) & LF_GHOST) != 0)
	{
		for (int nLine = nEndLine + 1; nLine < GetLineCount(); ++nLine)
			if ((GetLineFlags(nLine) & LF_GHOST) == 0)
			{
				paSavedRevisionNumbers->push_back(GetLineFlags(nLine));
				break;
			}
	}
	return paSavedRevisionNumbers;
}

void CGhostTextBuffer::			/* virtual override */
RestoreRevisionNumbers(int nStartLine, std::vector<uint32_t> *paSavedRevisionNumbers)
{
	for (int i = 0, j = 0; i < static_cast<int>(paSavedRevisionNumbers->size()); j++)
	{
		if ((GetLineFlags(nStartLine + j) & LF_GHOST) == 0)
		{
			m_aLines[nStartLine + j].m_dwRevisionNumber = (*paSavedRevisionNumbers)[i];
			++i;
		}
	}
}

bool CGhostTextBuffer::			/* virtual override */
DeleteText2 (CCrystalTextView * pSource, int nStartLine, int nStartChar,
            int nEndLine, int nEndChar, int nAction /*= CE_ACTION_UNKNOWN*/, bool bHistory /*= true*/)
{
	int const nLineCount = GetLineCount();
	while (nEndLine < nLineCount - 1 && GetLineFlags(nEndLine) & LF_GHOST)
		++nEndLine;
	if (!CCrystalTextBuffer::DeleteText2(pSource, nStartLine, nStartChar,
		nEndLine, nEndChar, nAction, bHistory))
	{
		return false;
	}

	if (nStartChar != 0 || nEndChar != 0)
		OnNotifyLineHasBeenEdited(nStartLine);

	// now we can recompute
	if (nStartLine != nEndLine)
	{
		// TODO: Be smarter, and don't recompute if it is easy to see what changed
		RecomputeRealityMapping();
	}
		
	return true;
}

#if 0
/**
 * @brief Insert a ghost line to the buffer (and view).
 * @param [in] pSource The view to which to add the ghost line.
 * @param [in] Line index (apparent/screen) where to add the ghost line.
 * @return true if the addition succeeded, false otherwise.
 */
bool CGhostTextBuffer::InsertGhostLine (CCrystalTextView * pSource, int nLine)
{
	if (!InternalInsertGhostLine (pSource, nLine))
		return false;

	// Set WinMerge flags  
	SetLineFlag (nLine, LF_GHOST, true, false, false);
	RecomputeRealityMapping();

	// Don't need to recompute EOL as real lines are unchanged.
	// Never AddUndoRecord as Rescan clears the ghost lines.
	return true;
}
#endif

/**
 * @brief Remove all the ghost lines from the buffer.
 */
void CGhostTextBuffer::RemoveAllGhostLines()
{
	int nlines = GetLineCount();
	int nFirstGhost = -1;
	// Free the buffer of ghost lines, 
	// remember where the first ghost line occurs
	for(int ct = 0; ct < nlines; ct++)
	{
		if (GetLineFlags(ct) & LF_GHOST)
		{
			m_aLines[ct].FreeBuffer();
			if (nFirstGhost < 0)
				nFirstGhost = ct;
		}
	}
	if (nFirstGhost >= 0)
	{
		// Compact non-ghost lines, starting at the first ghost.
		// (we copy the buffer address, so the buffer doesn't move and we don't free it)
		int newnl = nFirstGhost;
		for (int ct = nFirstGhost; ct < nlines; ct++)
		{
			if ((GetLineFlags(ct) & LF_GHOST) == 0)
				m_aLines[newnl++] = m_aLines[ct];
		}

		// Discard unused entries in one shot
		m_aLines.resize(newnl);
		RecomputeRealityMapping();
	}
}

////////////////////////////////////////////////////////////////////////////
// apparent <-> real line conversion

/**
 * @brief Get last apparent (screen) line index.
 * @return Last apparent line, or -1 if no lines in the buffer.
 */
int CGhostTextBuffer::ApparentLastRealLine() const
{
	if (m_RealityBlocks.size() == 0)
		return -1;
	const RealityBlock &block = m_RealityBlocks.back();
	return block.nStartApparent + block.nCount - 1;
}

/**
 * @brief Get a real line for the apparent (screen) line.
 * This function returns the real line for the given apparent (screen) line.
 * For ghost lines we return next real line. For trailing ghost line we return
 * last real line + 1). Ie, lines 0->0, 1->2, 2->4, for argument of 3,
 * return 2.
 * @param [in] nApparentLine Apparent line for which to get the real line.
 * @return The real line for the apparent line.
 */
int CGhostTextBuffer::ComputeRealLine(int nApparentLine) const
{
	int decToReal;
	return ComputeRealLineAndGhostAdjustment(nApparentLine, decToReal);
}

/**
 * @brief Get an apparent (screen) line for the real line.
 * @param [in] nRealLine Real line for which to get the apparent line.
 * @return The apparent line for the real line. If real line is out of bounds
 *   return last valid apparent line + 1.
 */
int CGhostTextBuffer::ComputeApparentLine(int nRealLine) const
{
	const int size = static_cast<int>(m_RealityBlocks.size());
	if (size == 0)
		return 0;

	// after last block ?
	const RealityBlock & maxblock = m_RealityBlocks.back();
	if (nRealLine >= maxblock.nStartReal + maxblock.nCount)
		return GetLineCount();

	// binary search to find correct (or nearest block)
	int blo = 0;
	int bhi = size - 1;
	while (blo <= bhi)
	{
		int i = (blo + bhi) / 2;
		const RealityBlock & block = m_RealityBlocks[i];
		if (nRealLine < block.nStartReal)
			bhi = i - 1;
		else if (nRealLine >= block.nStartReal + block.nCount)
			blo = i + 1;
		else
			return (nRealLine - block.nStartReal) + block.nStartApparent;
	}
	// Should have found it; all real lines should be in a block
	ASSERT(false);
	return -1;
}

/**
 * @brief Get a real line for apparent (screen) line.
 * This function returns the real line for the given apparent (screen) line.
 * For ghost lines we return next real line. For trailing ghost line we return
 * last real line + 1; i.e. lines 0->0, 1->2, 2->4, for argument of 3,
 * return 2 and decToReal would be 1.
 * @param [in] nApparentLine Apparent line for which to get the real line.
 * @param [out] decToReal Difference of the apparent and real line.
 * @return The real line for the apparent line.
 */
int CGhostTextBuffer::ComputeRealLineAndGhostAdjustment(int nApparentLine,
		int& decToReal) const
{
	const int size = static_cast<int>(m_RealityBlocks.size());
	if (size == 0) 
	{
		decToReal = 0;
		return 0;
	}

	// after last apparent line ?
	ASSERT(nApparentLine < GetLineCount());

	// after last block ?
	const RealityBlock & maxblock = m_RealityBlocks.back();
	if (nApparentLine >= maxblock.nStartApparent + maxblock.nCount)
	{
		decToReal = GetLineCount() - nApparentLine;
		return maxblock.nStartReal + maxblock.nCount;
	}

	// binary search to find correct (or nearest block)
	int blo = 0;
	int bhi = size - 1;
	while (blo <= bhi)
	{
		int i = (blo + bhi) / 2;
		const RealityBlock & block = m_RealityBlocks[i];
		if (nApparentLine < block.nStartApparent)
			bhi = i - 1;
		else if (nApparentLine >= block.nStartApparent + block.nCount)
			blo = i + 1;
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
 * @brief Get an apparent (screen) line for the real line.
 * @param [in] nRealLine Real line for which to get the apparent line.
 * @param [in] decToReal Difference of the apparent and real line.
 * @return The apparent line for the real line. If real line is out of bounds
 *   return last valid apparent line + 1.
 */
int CGhostTextBuffer::ComputeApparentLine(int nRealLine, int decToReal) const
{
	int nPreviousBlock;
	int nApparent;

	const int size = (int) m_RealityBlocks.size();
	int blo = 0;
	int bhi = size - 1;
	int i;
	if (size == 0)
		return 0;

	// after last block ?
	const RealityBlock & maxblock = m_RealityBlocks.back();
	if (nRealLine >= maxblock.nStartReal + maxblock.nCount)
	{
		nPreviousBlock = size - 1;
		nApparent = GetLineCount() - 1;
		goto limitWithPreviousBlock;
	}

	// binary search to find correct (or nearest block)
	while (blo <= bhi)
	{
		i = (blo + bhi) / 2;
		const RealityBlock & block = m_RealityBlocks[i];
		if (nRealLine < block.nStartReal)
			bhi = i - 1;
		else if (nRealLine >= block.nStartReal + block.nCount)
			blo = i + 1;
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
	ASSERT(false);
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
			return nApparent + 1;
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
	m_RealityBlocks.clear();
	int reality = -1; // last encountered real line
	int i = 0; // current line
	int nLineCount = GetLineCount();
	RealityBlock block; // current block being traversed (in state 2)

	// This is a state machine with 2 states

	// state 1, i-1 not real line
passingGhosts:
	ASSERT( i <= nLineCount );
	if (i == nLineCount)
	{
		checkFlagsFromReality();
		return;
	}
	if (GetLineFlags(i) & LF_GHOST)
	{
		++i;
		goto passingGhosts;
	}
	// this is the first line of a reality block
	block.nStartApparent = i;
	block.nStartReal = reality + 1;
	block.nCount = -1;
	++reality;
	++i;
	// fall through to other state

	// state 2, i - 1 is real line
inReality:
	ASSERT( i <= nLineCount );
	if (i == nLineCount || (GetLineFlags(i) & LF_GHOST))
	{
		// i-1 is the last line of a reality block
		ASSERT(reality >= 0);
		block.nCount = i - block.nStartApparent;
		ASSERT(block.nCount > 0);
		ASSERT(reality + 1 - block.nStartReal == block.nCount);
		
		// Optimize memory allocation
		if (m_RealityBlocks.capacity() == m_RealityBlocks.size())
		{
			if (m_RealityBlocks.size() == 0)
				m_RealityBlocks.reserve(16);
			else
				// TODO: grow more slowly with really large RealityBlocks
				m_RealityBlocks.reserve(m_RealityBlocks.size() * 2);
		}
		m_RealityBlocks.push_back(block);
		if (i == nLineCount)
		{
			checkFlagsFromReality();
			return;
		}
		++i;
		goto passingGhosts;
	}
	++reality;
	++i;
	goto inReality;
}

/** 
Check all lines, and ASSERT if reality blocks differ from flags. 
This means that this only has effect in DEBUG build
*/
void CGhostTextBuffer::checkFlagsFromReality() const
{
#ifdef _DEBUG
	const int size = static_cast<int>(m_RealityBlocks.size());
	int i = 0;
	for (int b = 0 ; b < size ; b ++)
	{
		const RealityBlock & block = m_RealityBlocks[b];
		for ( ; i < block.nStartApparent ; i++)
			ASSERT ((GetLineFlags(i) & LF_GHOST) != 0);
		for ( ; i < block.nStartApparent+block.nCount ; i++)
			ASSERT ((GetLineFlags(i) & LF_GHOST) == 0);
	}

	for ( ; i < GetLineCount() ; i++)
		ASSERT ((GetLineFlags(i) & LF_GHOST) != 0);
#endif 
}

void CGhostTextBuffer::			/* virtual base */
OnNotifyLineHasBeenEdited(int nLine)
{
	return;
}

void CGhostTextBuffer::
CountEolAndLastLineLength(const CEPoint& ptStartPos, const tchar_t* pszText, size_t cchText, int &nLastLineLength, int &nEol)
{
	nLastLineLength = 0;
	nEol = 0;
	if (m_bTableEditing && m_bAllowNewlinesInQuotes)
	{
		bool bInQuote = false;
		const tchar_t* pszLine = m_aLines[ptStartPos.y].GetLine();
		for (int j = 0; j < ptStartPos.x; ++j)
		{
			if (pszLine[j] == m_cFieldEnclosure)
				bInQuote = !bInQuote;
		}
		for (size_t nTextPos = 0; nTextPos < cchText; ++nTextPos)
		{
			if (pszText[nTextPos] == m_cFieldEnclosure)
				bInQuote = !bInQuote;
			if (!bInQuote && LineInfo::IsEol(pszText[nTextPos]))
			{
				if (nTextPos + 1 < cchText && LineInfo::IsDosEol(&pszText[nTextPos]))
					++nTextPos;
				++nEol;
				nLastLineLength = 0;
			}
			else
				++nLastLineLength;
		}
	}
	else
	{
		for (size_t nTextPos = 0; nTextPos < cchText; ++nTextPos)
		{
			if (LineInfo::IsEol(pszText[nTextPos]))
			{
				if (nTextPos + 1 < cchText && LineInfo::IsDosEol(&pszText[nTextPos]))
					++nTextPos;
				++nEol;
				nLastLineLength = 0;
			}
			else
				++nLastLineLength;
		}
	}
}

void CGhostTextBuffer::			/* virtual override */
AddUndoRecord(bool bInsert, const CEPoint & ptStartPos,
	const CEPoint & ptEndPos, const tchar_t* pszText, size_t cchText,
	int nActionType /*= CE_ACTION_UNKNOWN*/,
	std::vector<uint32_t> *paSavedRevisionNumbers /*= nullptr*/)
{
	CEPoint real_ptStartPos(ptStartPos.x, ComputeRealLine(ptStartPos.y));
	int nLastLineLength, nEol;
	CountEolAndLastLineLength(ptStartPos, pszText, cchText, nLastLineLength, nEol);
	CEPoint real_ptEndPos(ptEndPos.x, real_ptStartPos.y + nEol);
	if (ptEndPos.x == 0 && cchText > 0 && !LineInfo::IsEol(pszText[cchText - 1]))
		real_ptEndPos.x = nLastLineLength;
	CCrystalTextBuffer::AddUndoRecord(bInsert, real_ptStartPos, real_ptEndPos, pszText,
		cchText, nActionType, paSavedRevisionNumbers);
}

UndoRecord CGhostTextBuffer::			/* virtual override */
GetUndoRecord(int nUndoPos) const
{
	UndoRecord ur = m_aUndoBuf[nUndoPos];
	ur.m_ptStartPos.y = ComputeApparentLine(ur.m_ptStartPos.y, 0);
	ur.m_ptEndPos.y = ComputeApparentLine(ur.m_ptEndPos.y, 0);
	return ur;
}

bool CGhostTextBuffer::		/* virtual override */
UndoInsert(CCrystalTextView * pSource, CEPoint & ptCursorPos, const CEPoint apparent_ptStartPos, CEPoint const apparent_ptEndPos, const UndoRecord & ur)
{    
    // Check that text in the undo buffer matches text in file buffer.  
	// If not, then rescan() has moved lines and undo fails.

    // we need to put the cursor before the deleted section
    String text;
    const size_t size = m_aLines.size();
    if ((apparent_ptStartPos.y < static_cast<LONG>(size)) &&
        (apparent_ptStartPos.x <= static_cast<LONG>(m_aLines[apparent_ptStartPos.y].Length())) &&
        (apparent_ptEndPos.y < static_cast<LONG>(size)) &&
        (apparent_ptEndPos.x <= static_cast<LONG>(m_aLines[apparent_ptEndPos.y].Length())))
    {
		//  Try to ensure that we are undoing correctly...
		//  Just compare the text as it was before Undo operation
        GetTextWithoutEmptys (apparent_ptStartPos.y, apparent_ptStartPos.x, apparent_ptEndPos.y, apparent_ptEndPos.x, text, CRLFSTYLE::AUTOMATIC, false);
        if (text.length() == ur.GetTextLength() && memcmp(text.c_str(), ur.GetText(), text.length() * sizeof(tchar_t)) == 0)
        {
			if (CCrystalTextBuffer::UndoInsert(pSource, ptCursorPos, apparent_ptStartPos, apparent_ptEndPos, ur))
			{
				// ptCursorPos = apparent_ptStartPos;
				return true;
			}
        }
		else
		// It is possible that the ptEndPos is at the last line of the file and originally pointed
		//	at an LF_GHOST line that followed (and has since been discarded).  Lets try to reconstruct
		//	that situation before we fail entirely...
		if (apparent_ptEndPos.y + 1 == static_cast<LONG>(size) && apparent_ptEndPos.x == 0)
		{
			CEPoint apparentEnd2 = apparent_ptEndPos;
			apparentEnd2.x = static_cast<LONG>(m_aLines[apparentEnd2.y].FullLength());
			text.clear();
			GetTextWithoutEmptys(apparent_ptStartPos.y, apparent_ptStartPos.x, apparent_ptEndPos.y, apparentEnd2.x, text, CRLFSTYLE::AUTOMATIC, false);
			if (text.length() == ur.GetTextLength() && memcmp(text.c_str(), ur.GetText(), text.length() * sizeof(tchar_t)) == 0)
			{
				if (CCrystalTextBuffer::UndoInsert(pSource, ptCursorPos, apparent_ptStartPos, apparentEnd2, ur))
				{
					// ptCursorPos = apparent_ptStartPos;
					const size_t nLastLine = m_aLines.size() - 1;
					if (m_aLines[nLastLine].Length() == 0)
					{
						m_aLines[nLastLine].Clear();
						if (static_cast<size_t>(ptCursorPos.y) == nLastLine)
							ptCursorPos.y--;
					}
					return true;
				}
			}
		}
    }
	ASSERT(false);
	return false;
}

