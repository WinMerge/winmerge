/**
 * @file  MergeResultTextBuffer.cpp
 *
 * @brief Implementation of CMergeResultTextBuffer class (kdiff3-style merge result buffer)
 */

#include "StdAfx.h"
#include "MergeResultTextBuffer.h"
#include "MergeDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////////////
// CMergeResultTextBuffer

CMergeResultTextBuffer::CMergeResultTextBuffer(CMergeDoc* pDoc)
: CDiffTextBuffer(pDoc, 1 /* only used by code paths not taken for the result buffer */)
, m_pResultOwnerDoc(pDoc)
, m_nInternalOp(0)
{
}

/**
 * @brief Track the modified state on the document, without clobbering it:
 * set the document flag when the result becomes dirty, but only clear it
 * when no compared buffer is modified either.
 */
void CMergeResultTextBuffer::SetModified(bool bModified /*= true*/)
{
	CCrystalTextBuffer::SetModified(bModified);
	if (bModified)
		m_pResultOwnerDoc->SetModifiedFlag(TRUE);
	else if (!m_pResultOwnerDoc->IsModified())
		m_pResultOwnerDoc->SetModifiedFlag(FALSE);
}

/**
 * @brief Record undo, but do not register the result buffer in the
 * document's pane undo-target list (the result pane has its own undo).
 *
 * When a new undo group starts, snapshot the segment table so that
 * Undo/Redo can restore it exactly; deriving the table from the replayed
 * buffer operations is ambiguous (e.g. a Choose is delete+insert, whose
 * generic bookkeeping cannot know what the segment state used to be).
 */
void CMergeResultTextBuffer::AddUndoRecord(bool bInsert, const CEPoint & ptStartPos,
	const CEPoint & ptEndPos, const tchar_t* pszText, size_t cchText,
	int nActionType /*= CE_ACTION_UNKNOWN*/,
	std::vector<uint32_t> *paSavedRevisionNumbers /*= nullptr*/)
{
	CGhostTextBuffer::AddUndoRecord(bInsert, ptStartPos, ptEndPos, pszText,
		cchText, nActionType, paSavedRevisionNumbers);
	if (m_aUndoBuf[m_nUndoPosition - 1].m_dwFlags & UNDO_BEGINGROUP)
		m_pResultOwnerDoc->OnResultUndoGroupStart(m_nUndoPosition - 1);
}

/**
 * @brief Undo, restoring the segment table from its snapshot.
 * The internal-op guard keeps the generic edit hooks from adjusting the
 * table during the replay; the snapshot restore is exact.
 */
bool CMergeResultTextBuffer::Undo(CCrystalTextView * pSource, CEPoint & ptCursorPos)
{
	InternalOpGuard guard(*this);
	const bool bResult = CGhostTextBuffer::Undo(pSource, ptCursorPos);
	if (bResult)
		m_pResultOwnerDoc->OnResultUndone(m_nUndoPosition);
	else
		m_pResultOwnerDoc->OnResultUndoStackCleared(); // failed undo clears the stack
	return bResult;
}

bool CMergeResultTextBuffer::Redo(CCrystalTextView * pSource, CEPoint & ptCursorPos)
{
	InternalOpGuard guard(*this);
	const int nGroupStart = m_nUndoPosition;
	const bool bResult = CGhostTextBuffer::Redo(pSource, ptCursorPos);
	if (bResult)
		m_pResultOwnerDoc->OnResultRedone(nGroupStart);
	return bResult;
}

void CMergeResultTextBuffer::OnNotifyLineHasBeenEdited(int nLine)
{
	CGhostTextBuffer::OnNotifyLineHasBeenEdited(nLine);
	if (!IsInternalOp())
		m_pResultOwnerDoc->OnResultLineEdited(nLine);
}

bool CMergeResultTextBuffer::InsertText(CCrystalTextView * pSource, int nLine, int nPos,
	const tchar_t* pszText, size_t cchText, int &nEndLine, int &nEndChar,
	int nAction /*= CE_ACTION_UNKNOWN*/, bool bHistory /*= true*/)
{
	// Placeholder lines are not editable (kdiff3 rule): a difference must
	// be resolved with a Choose command before its text can be changed.
	// Without this, editing the placeholder would silently remove the
	// difference from the unresolved count while its text remains.
	if (!IsInternalOp() && m_pResultOwnerDoc->IsResultPlaceholderLine(nLine))
		return false;
	const bool bResult = CGhostTextBuffer::InsertText(pSource, nLine, nPos,
		pszText, cchText, nEndLine, nEndChar, nAction, bHistory);
	if (bResult && !IsInternalOp() && nEndLine > nLine)
		m_pResultOwnerDoc->OnResultBufferInsertedLines(nLine, nEndLine - nLine);
	return bResult;
}

bool CMergeResultTextBuffer::DeleteText2(CCrystalTextView * pSource, int nStartLine,
	int nStartPos, int nEndLine, int nEndPos,
	int nAction /*= CE_ACTION_UNKNOWN*/, bool bHistory /*= true*/)
{
	// Placeholder lines cannot be deleted or joined with their neighbors;
	// see the corresponding check in InsertText
	if (!IsInternalOp() && m_pResultOwnerDoc->IsResultDeleteTouchingPlaceholder(
			nStartLine, nStartPos, nEndLine, nEndPos))
		return false;
	// Deliberately skip CDiffTextBuffer::DeleteText2: it maintains sync
	// points by pane index, which does not apply to the result buffer.
	const bool bResult = CGhostTextBuffer::DeleteText2(pSource, nStartLine,
		nStartPos, nEndLine, nEndPos, nAction, bHistory);
	if (bResult && !IsInternalOp() && nEndLine > nStartLine)
	{
		// A column-0-to-column-0 deletion removes lines [nStartLine,
		// nEndLine-1] whole; line nEndLine survives unchanged, so no text
		// merge happens and the merge-point bookkeeping does not apply
		if (nStartPos == 0 && nEndPos == 0)
			m_pResultOwnerDoc->OnResultBufferDeletedWholeLines(nStartLine, nEndLine - nStartLine);
		else
			m_pResultOwnerDoc->OnResultBufferDeletedLines(nStartLine, nEndLine - nStartLine);
	}
	return bResult;
}

