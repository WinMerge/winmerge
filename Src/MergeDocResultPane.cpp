/**
 * @file  MergeDocResultPane.cpp
 *
 * @brief Implementation of the kdiff3-style merge result pane
 *        (CMergeDoc part + CMergeResultTextBuffer).
 *
 * The merge result is a 4th text buffer that is not part of the 3-way
 * diff: its content is generated from the three compared buffers and a
 * per-difference resolution state (see MergeResultSegment). The result
 * segment table maps result buffer lines back to differences.
 */

#include "StdAfx.h"
#include "MergeDoc.h"
#include "MergeResultPane.h"
#include "MergeResultView.h"
#include "MergeEditView.h"
#include "MergeEditFrm.h"
#include "MergeLineFlags.h"
#include "FileOrFolderSelect.h"
#include "FileTransform.h"
#include "UniFile.h"
#include "Environment.h"
#include "paths.h"
#include "Merge.h"
#include "resource.h"
#include <algorithm>

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

/////////////////////////////////////////////////////////////////////////////
// CMergeDoc merge result pane implementation

bool CMergeDoc::HasMergeResultPane() const
{
	return m_nBuffers == 3 && m_pMergeResultView != nullptr;
}

/**
 * @brief Is the result pane present, shown and populated?
 */
bool CMergeDoc::IsMergeResultPaneActive() const
{
	if (!HasMergeResultPane() || !m_bResultBuilt)
		return false;
	return IsMergeResultPaneVisible();
}

/**
 * @brief Is the bar hosting the result view shown?
 * Checks the bar's own visible style so this also works while the
 * frame itself is not visible yet.
 */
bool CMergeDoc::IsMergeResultPaneVisible() const
{
	if (m_pMergeResultView == nullptr || m_pMergeResultView->GetSafeHwnd() == nullptr)
		return false;
	const CWnd* pBar = m_pMergeResultView->GetParent();
	return pBar != nullptr && (pBar->GetStyle() & WS_VISIBLE) != 0;
}

/**
 * @brief Called when the result pane is shown or hidden.
 *
 * While the result pane is active the three compared buffers are forced
 * read-only (kdiff3 model: sources are inputs, result is the output).
 */
void CMergeDoc::SetMergeResultPaneVisible(bool bVisible)
{
	if (m_nBuffers < 3 || m_ptResultBuf == nullptr)
		return;
	if (bVisible)
	{
		if (!m_bResultBuilt)
			BuildMergeResult();
		if (!m_bResultROForced)
		{
			for (int nBuffer = 0; nBuffer < m_nBuffers; ++nBuffer)
			{
				m_bResultSavedRO[nBuffer] = m_ptBuf[nBuffer]->GetReadOnly();
				m_ptBuf[nBuffer]->SetReadOnly(true);
			}
			m_bResultROForced = true;
		}
	}
	else if (m_bResultROForced)
	{
		for (int nBuffer = 0; nBuffer < m_nBuffers; ++nBuffer)
			m_ptBuf[nBuffer]->SetReadOnly(m_bResultSavedRO[nBuffer]);
		m_bResultROForced = false;
	}
}

/**
 * @brief Show and populate the result pane because the document has a
 * merge output path (opened with -o, or from a conflict file).
 */
void CMergeDoc::ShowMergeResultPaneForOutput()
{
	if (!HasMergeResultPane())
		return;
	CMergeEditFrame* pFrame = GetParentFrame();
	if (pFrame == nullptr)
		return;
	pFrame->ShowMergeResultPane();
	SetMergeResultPaneVisible(true);
}

/**
 * @brief Start (or re-run) a merge session in the result pane.
 * @param [in] bAutoMerge Auto-resolve the non-conflicting differences.
 *
 * Used by the Merge menu's Start Merge Session command (no auto-merge)
 * and by the Auto Merge command (with auto-merge), which switches a
 * plain 3-way comparison to the 4-pane merge view.
 */
void CMergeDoc::StartMergeSession(bool bAutoMerge)
{
	if (!HasMergeResultPane())
		return;
	// Rebuild when the auto-merge mode changes, and also when the
	// segment <-> diff links were severed by a rescan: starting a new
	// session is the recovery path the sever notification promises
	const bool bLinksSevered = m_bResultBuilt && m_resultDiffSnapshot.empty();
	if (m_bResultBuilt && (bAutoMerge != m_bResultAutoMerge || bLinksSevered))
	{
		if (IsMergeResultModified() &&
			ShowMessageBox(_("Rebuilding the merge result discards the changes made in the Merge Result pane.\n\nContinue?"),
				MB_YESNO | MB_ICONWARNING) != IDYES)
			return;
		m_bResultBuilt = false; // rebuild with the new auto-merge mode
	}
	m_bResultAutoMerge = bAutoMerge;
	if (CMergeEditFrame* pFrame = GetParentFrame())
		pFrame->ShowMergeResultPane();
	SetMergeResultPaneVisible(true);
}

/**
 * @brief Extract the text of pane nPane for apparent lines
 * [nApparentBegin, nApparentEnd], skipping ghost lines.
 * Every extracted line is terminated with an EOL (the line's own EOL,
 * or the result buffer's default EOL if the line has none).
 * @return Text; *pnLines receives the number of extracted lines.
 */
String CMergeDoc::GetPaneApparentLinesText(int nPane, int nApparentBegin,
	int nApparentEnd, int* pnLines) const
{
	String text;
	int nLines = 0;
	const CDiffTextBuffer* pBuf = m_ptBuf[nPane].get();
	const tchar_t* pszDefaultEol = m_ptResultBuf->GetDefaultEol();
	const int nLineCount = pBuf->GetLineCount();
	for (int nLine = nApparentBegin; nLine <= nApparentEnd && nLine < nLineCount; ++nLine)
	{
		if (pBuf->GetLineFlags(nLine) & LF_GHOST)
			continue;
		const int nLength = pBuf->GetLineLength(nLine);
		if (nLength > 0)
			text.append(pBuf->GetLineChars(nLine), nLength);
		const tchar_t* pszEol = pBuf->GetLineEol(nLine);
		text += (pszEol != nullptr && *pszEol != _T('\0')) ? pszEol : pszDefaultEol;
		++nLines;
	}
	if (pnLines != nullptr)
		*pnLines = nLines;
	return text;
}

/**
 * @brief (Re)generate the merge result buffer from the three compared
 * buffers, auto-resolving all non-conflicting differences.
 *
 * The middle pane is treated as the common ancestor (base), matching
 * the semantics of WinMerge's existing 3-way auto-merge.
 */
void CMergeDoc::BuildMergeResult()
{
	if (m_nBuffers < 3 || m_ptResultBuf == nullptr || !m_ptBuf[1]->IsInitialized())
		return;

	CMergeResultTextBuffer::InternalOpGuard guard(*m_ptResultBuf);

	const bool bViewAttached = m_pMergeResultView != nullptr &&
		m_pMergeResultView->GetSafeHwnd() != nullptr &&
		m_pMergeResultView->LocateTextBuffer() != nullptr &&
		m_ptResultBuf->IsInitialized();
	if (bViewAttached)
		m_pMergeResultView->DetachFromBuffer();
	if (m_ptResultBuf->IsInitialized())
		m_ptResultBuf->FreeAll();

	// The buffer's nominal style only provides the default EOL for typed
	// and placeholder lines; generated lines keep their source EOLs, and
	// saving preserves them per line unless the user chose a fixed style
	m_ptResultBuf->InitNew(m_resultSaveEolStyle != CRLFSTYLE::AUTOMATIC ?
		m_resultSaveEolStyle : PickResultCRLFStyle());
	PickResultEncoding();
	m_ptResultBuf->SetReadOnly(false);
	m_ptResultBuf->SetTempPath(env::GetTemporaryPath()); // needed by SaveToFile

	m_resultSegments.clear();
	OnResultUndoStackCleared(); // buffer undo history is gone as well
	m_resultDiffToSegment.assign(m_diffList.GetSize(), -1);

	String text;

	// An existing output file with conflict sections (a previous partial
	// merge, or a version-control pre-populated merge target) can be
	// continued instead of starting over; only offered once per document
	bool bResumed = false;
	if (!m_bResultResumeAttempted && !m_strSaveAsPath.empty())
	{
		m_bResultResumeAttempted = true;
		bResumed = TryResumeMergeResultFromOutput(text);
	}
	// The resume prompt is modal and pumps messages, so a rescan can have
	// replaced the diff list while it was open: size everything below off
	// the list as it is NOW
	const int nDiffCount = m_diffList.GetSize();
	if (!bResumed)
	{
		// also discards anything a failed resume attempt filled in
		m_resultSegments.clear();
		m_resultDiffToSegment.assign(nDiffCount, -1);
		text.clear();
	}
	if (!bResumed)
	{
	int nCurLine = 0;
	int nApparent = 0;
	const int nApparentCount = m_ptBuf[1]->GetLineCount();

	auto appendCommon = [&](int nBegin, int nEndExcl)
	{
		if (nBegin >= nEndExcl)
			return;
		int nLines = 0;
		text += GetPaneApparentLinesText(1, nBegin, nEndExcl - 1, &nLines);
		if (nLines > 0)
		{
			MergeResultSegment seg;
			seg.diffIdx = -1;
			seg.state = ResultSegmentState::Common;
			seg.srcPanes.push_back(1);
			seg.srcPaneLines.push_back(nLines);
			seg.nStartLine = nCurLine;
			seg.nLines = nLines;
			m_resultSegments.push_back(seg);
			nCurLine += nLines;
		}
	};

	for (int nDiff = 0; nDiff < nDiffCount; ++nDiff)
	{
		const DIFFRANGE* pdi = m_diffList.DiffRangeAt(nDiff);
		appendCommon(nApparent, pdi->dbegin);

		MergeResultSegment seg;
		seg.diffIdx = nDiff;
		seg.nStartLine = nCurLine;
		// Without auto-merge every significant difference starts
		// unresolved; with it only true 3-way conflicts do. A difference
		// where the sides agree (or only one side changed) is not a
		// conflict even while it is still unresolved.
		const bool bConflict = (pdi->op == OP_DIFF);
		if (bConflict || (!m_bResultAutoMerge && pdi->op != OP_TRIVIAL))
		{
			seg.state = bConflict ?
				ResultSegmentState::Conflict : ResultSegmentState::Unresolved;
			seg.bWhiteSpaceOnly = bConflict && IsResultDiffWhiteSpaceOnly(pdi);
			if (bConflict)
				seg.blockText = GetResultConflictBlockText(nDiff, seg.bWhiteSpaceOnly,
					&seg.nBlockLines);
			else
			{
				// A non-conflicting difference must never put conflict
				// markers into the output; its stashed (and saved) form is
				// the content an automatic merge would pick for it
				int srcPane = m_diffList.GetMergeableSrcIndex(nDiff, 1);
				if (srcPane == -1)
					srcPane = 1;
				seg.blockText = GetPaneApparentLinesText(srcPane, pdi->dbegin,
					pdi->dend, &seg.nBlockLines);
			}
			text += GetResultSegmentDisplayText(seg, &seg.nLines);
		}
		else
		{
			int srcPane = m_diffList.GetMergeableSrcIndex(nDiff, 1);
			if (srcPane == -1)
				srcPane = 1; // trivial/ignored difference: keep base text
			int nLines = 0;
			text += GetPaneApparentLinesText(srcPane, pdi->dbegin, pdi->dend, &nLines);
			seg.state = ResultSegmentState::Auto;
			seg.srcPanes.push_back(srcPane);
			seg.srcPaneLines.push_back(nLines);
			seg.nLines = nLines;
		}
		m_resultDiffToSegment[nDiff] = static_cast<int>(m_resultSegments.size());
		m_resultSegments.push_back(seg);
		nCurLine += seg.nLines;
		nApparent = pdi->dend + 1;
	}
	appendCommon(nApparent, nApparentCount);
	} // if (!bResumed)

	if (!text.empty())
	{
		int nEndLine = 0, nEndChar = 0;
		m_ptResultBuf->InsertText(nullptr, 0, 0, text.c_str(), text.length(),
			nEndLine, nEndChar, CE_ACTION_UNKNOWN, false /* no history */);
	}
	m_ptResultBuf->SetModified(false);
	// the generated content is the baseline: no change markers on it
	m_ptResultBuf->AdoptCurrentRevision();
	for (auto& seg : m_resultSegments)
		seg.nBaseRevision = m_ptResultBuf->GetCurrentRevisionNumber();
	m_bResultBuilt = true;
	// A generated result has not been written to the output path yet: an
	// automatically merged result still has to be saved even if the user
	// edits nothing. A resumed result IS the output file's content.
	m_bResultSaved = bResumed;

	// Remember the diff list this result was generated from, so that a
	// rescan producing the identical list keeps the segment <-> diff links
	m_resultDiffSnapshot.clear();
	m_resultDiffSnapshot.reserve(nDiffCount);
	for (int nDiff = 0; nDiff < nDiffCount; ++nDiff)
	{
		const DIFFRANGE* pdi = m_diffList.DiffRangeAt(nDiff);
		m_resultDiffSnapshot.push_back({ static_cast<int>(pdi->dbegin),
			static_cast<int>(pdi->dend), static_cast<int>(pdi->op) });
	}
	m_bResultLinksDropNotified = false;

	if (m_pMergeResultView != nullptr && m_pMergeResultView->GetSafeHwnd() != nullptr)
	{
		m_pMergeResultView->AttachToBuffer(m_ptResultBuf.get());
		m_pMergeResultView->RefreshOptions();
		m_pMergeResultView->Invalidate();
	}
	UpdateMergeResultPaneCaption();
}

/**
 * @brief Called at the end of Rescan() to keep the result pane in sync.
 */
void CMergeDoc::UpdateMergeResultAfterRescan()
{
	if (m_nBuffers < 3 || m_ptResultBuf == nullptr || m_pMergeResultView == nullptr)
		return;
	if (!m_bResultBuilt)
		return; // nothing generated yet, it will be built from the new diffs
	if (!IsMergeResultPaneVisible() && !m_ptResultBuf->IsModified())
	{
		// Hidden and untouched: throw the stale result away so that showing
		// the pane again rebuilds it from the current differences
		m_bResultBuilt = false;
		return;
	}
	if (m_ptResultBuf->IsModified())
	{
		// The user already changed the result: don't discard their work.
		if (ResultDiffListUnchanged())
			return; // same differences as before: all links stay valid
		if (m_resultDiffSnapshot.empty())
		{
			// Links already severed by an earlier rescan: the segment
			// table is diff-independent now. Only keep the (unused)
			// lookup table sized to the current list.
			m_resultDiffToSegment.assign(m_diffList.GetSize(), -1);
			return;
		}
		// The diff list changed, so the segment <-> diff links are no
		// longer valid; drop them (Choose commands become unavailable).
		for (auto& seg : m_resultSegments)
		{
			if (seg.diffIdx >= 0)
			{
				seg.diffIdx = -1;
				if (seg.state != ResultSegmentState::Conflict &&
					seg.state != ResultSegmentState::Unresolved)
					seg.state = ResultSegmentState::Edited;
			}
		}
		m_resultDiffToSegment.assign(m_diffList.GetSize(), -1);
		m_resultDiffSnapshot.clear();
		// Unlinked unresolved segments can only be fixed by hand, so their
		// compact placeholders are expanded to the full conflict sections
		ReRenderResultConflictSegments();
		UpdateMergeResultPaneCaption();
		if (!m_bResultLinksDropNotified)
		{
			m_bResultLinksDropNotified = true;
			ShowMessageBox(
				_("The differences were recalculated and no longer match the Merge Result pane.\n\nYour result text is kept, but the Choose commands are disabled until a new merge session is started from the Merge menu. Remaining conflicts are shown as full conflict sections and can be edited directly."),
				MB_OK | MB_ICONINFORMATION);
		}
		return;
	}
	m_bResultBuilt = false;
	SetMergeResultPaneVisible(true);
}

bool CMergeDoc::IsMergeResultModified() const
{
	return m_ptResultBuf != nullptr && m_ptResultBuf->IsInitialized() &&
		m_ptResultBuf->IsModified();
}

/**
 * @brief Is there merge result content that has not been written out?
 *
 * True also for a freshly built result that the user has not edited: an
 * automatic merge produces the content, and it is still lost if the
 * window is closed without saving.
 */
bool CMergeDoc::IsMergeResultUnsaved() const
{
	if (m_ptResultBuf == nullptr || !m_ptResultBuf->IsInitialized() || !m_bResultBuilt)
		return false;
	return !m_bResultSaved || m_ptResultBuf->IsModified();
}

/**
 * @brief Conflict section shown (and saved) for a difference that has no
 * content chosen for it yet, in the familiar version-control format:
 *
 *   <<<<<<< left
 *   ||||||| middle (diff3 style: the base version is included)
 *   =======
 *   >>>>>>> right
 *
 * All three versions are preserved in the output, so a merge saved with
 * unresolved differences can be finished later in any editor, and other
 * tools recognize the file as conflicted.
 */
String CMergeDoc::GetResultConflictBlockText(int nDiff, bool bWhiteSpaceOnly,
	int* pnLines) const
{
	const DIFFRANGE* pdi = m_diffList.DiffRangeAt(nDiff);
	const tchar_t* pszEol = m_ptResultBuf->GetDefaultEol();
	auto label = [&](int nPane)
	{
		return !m_strDesc[nPane].empty() ?
			m_strDesc[nPane] : paths::FindFileName(m_filePaths[nPane]);
	};
	int nLines = 4;
	int nPaneLines = 0;
	String text = _T("<<<<<<< ") + label(0);
	if (bWhiteSpaceOnly)
		text += _T(" (whitespace only)");
	text += pszEol;
	text += GetPaneApparentLinesText(0, pdi->dbegin, pdi->dend, &nPaneLines);
	nLines += nPaneLines;
	text += _T("||||||| ") + label(1) + pszEol;
	text += GetPaneApparentLinesText(1, pdi->dbegin, pdi->dend, &nPaneLines);
	nLines += nPaneLines;
	text += _T("=======");
	text += pszEol;
	text += GetPaneApparentLinesText(2, pdi->dbegin, pdi->dend, &nPaneLines);
	nLines += nPaneLines;
	text += _T(">>>>>>> ") + label(2) + pszEol;
	if (pnLines != nullptr)
		*pnLines = nLines;
	return text;
}

/**
 * @brief Compact placeholder line shown for an unresolved difference when
 * full conflict sections are not displayed. Display only: the saved file
 * always contains the full conflict section (see BuildExpandedResultText).
 */
String CMergeDoc::GetResultPlaceholderText(ResultSegmentState state, bool bWhiteSpaceOnly)
{
	if (state == ResultSegmentState::Conflict)
		return bWhiteSpaceOnly ?
			_("<Merge Conflict (Whitespace only)>") : _("<Merge Conflict>");
	return _("<Unresolved Difference>");
}

/**
 * @brief Text an unresolved segment displays in the result buffer: the
 * full conflict section, or a one-line placeholder in compact mode.
 * Segments without a linked difference always display the full section:
 * Choose commands cannot resolve them, so their content must be editable.
 */
String CMergeDoc::GetResultSegmentDisplayText(const MergeResultSegment& seg,
	int* pnLines) const
{
	if (m_bResultShowFullConflicts || seg.diffIdx < 0)
	{
		if (pnLines != nullptr)
			*pnLines = seg.nBlockLines;
		return seg.blockText;
	}
	if (pnLines != nullptr)
		*pnLines = 1;
	return GetResultPlaceholderText(seg.state, seg.bWhiteSpaceOnly) +
		m_ptResultBuf->GetDefaultEol();
}

/**
 * @brief Number of differences with nothing chosen for them yet
 * (both real conflicts and non-conflicting differences).
 */
int CMergeDoc::GetResultUnresolvedCount() const
{
	int nUnresolved = 0, nConflicts = 0, nWhiteSpaceOnly = 0;
	GetResultUnresolvedCounts(nUnresolved, nConflicts, nWhiteSpaceOnly);
	return nUnresolved;
}

/**
 * @brief Counts of pending differences: all, true 3-way conflicts, and
 * conflicts where the sides differ only in white space.
 */
void CMergeDoc::GetResultUnresolvedCounts(int& nUnresolved, int& nConflicts,
	int& nWhiteSpaceOnly) const
{
	nUnresolved = nConflicts = nWhiteSpaceOnly = 0;
	for (const auto& seg : m_resultSegments)
	{
		if (seg.state == ResultSegmentState::Conflict)
		{
			++nUnresolved;
			++nConflicts;
			if (seg.bWhiteSpaceOnly)
				++nWhiteSpaceOnly;
		}
		else if (seg.state == ResultSegmentState::Unresolved)
			++nUnresolved;
	}
}

/**
 * @brief Difference that is still waiting for a decision?
 */
bool CMergeDoc::IsResultDiffPending(int nDiff) const
{
	const MergeResultSegment* pSegment = GetResultSegmentByDiff(nDiff);
	return pSegment != nullptr &&
		(pSegment->state == ResultSegmentState::Conflict ||
		 pSegment->state == ResultSegmentState::Unresolved);
}

/**
 * @brief Is the line a placeholder for an unresolved difference?
 * Placeholder lines are not editable: the difference must be resolved
 * with a Choose command first.
 */
bool CMergeDoc::IsResultPlaceholderLine(int nLine) const
{
	const MergeResultSegment* pSegment = GetResultSegmentByLine(nLine);
	// Only differences that can be resolved with a Choose command are
	// edit-protected. A segment without a linked difference (severed
	// links, or a resumed conflict section not matching any difference)
	// must stay editable: hand-editing is its only resolution path.
	return pSegment != nullptr && pSegment->diffIdx >= 0 &&
		(pSegment->state == ResultSegmentState::Conflict ||
		 pSegment->state == ResultSegmentState::Unresolved);
}

/**
 * @brief Does deleting the given range destroy or join a placeholder line?
 *
 * Deleting whole lines that merely end at a placeholder's start (col 0 to
 * col 0) leaves the placeholder itself untouched and is allowed.
 */
bool CMergeDoc::IsResultDeleteTouchingPlaceholder(int nStartLine, int nStartPos,
	int nEndLine, int nEndPos) const
{
	for (int nLine = nStartLine; nLine <= nEndLine; ++nLine)
	{
		if (!IsResultPlaceholderLine(nLine))
			continue;
		if (nLine == nEndLine && nEndPos == 0 && nStartPos == 0 && nStartLine < nEndLine)
			continue; // whole preceding lines removed, placeholder intact
		return true;
	}
	return false;
}

/**
 * @brief Margin provenance marker for a result line, kdiff3 style:
 * '1'/'2'/'3' = taken from that pane (matching the toolbar buttons),
 * '?' = unresolved placeholder, 'm' = modified by hand, 0 = common text.
 */
tchar_t CMergeDoc::GetResultLineMarker(int nLine) const
{
	const MergeResultSegment* pSegment = GetResultSegmentByLine(nLine);
	// Common text carries no marker; everything else does, including
	// segments without a linked difference (severed or resumed sections)
	if (pSegment == nullptr || pSegment->state == ResultSegmentState::Common)
		return 0;

	// attribute the line to the source pane its block came from
	auto srcPaneMarker = [&]() -> tchar_t
	{
		int nRel = nLine - pSegment->nStartLine;
		const size_t nSrcCount = (std::min)(pSegment->srcPanes.size(),
			pSegment->srcPaneLines.size());
		for (size_t i = 0; i < nSrcCount; ++i)
		{
			if (nRel < pSegment->srcPaneLines[i])
				return static_cast<tchar_t>(_T('1') + pSegment->srcPanes[i]);
			nRel -= pSegment->srcPaneLines[i];
		}
		return pSegment->srcPanes.empty() ? 0 :
			static_cast<tchar_t>(_T('1') + pSegment->srcPanes[0]);
	};

	switch (pSegment->state)
	{
	case ResultSegmentState::Conflict:
	case ResultSegmentState::Unresolved:
		return _T('?');
	case ResultSegmentState::Edited:
	{
		// Only the lines the user actually touched are marked 'm'
		// (kdiff3 behavior); the rest keep their source attribution.
		// A line edited after the segment's content was generated has a
		// newer revision than the segment's baseline.
		if (m_ptResultBuf != nullptr &&
			m_ptResultBuf->GetLineRevisionNumber(nLine) > pSegment->nBaseRevision)
			return _T('m');
		const tchar_t marker = srcPaneMarker();
		return marker != 0 ? marker : _T('m');
	}
	case ResultSegmentState::Auto:
	case ResultSegmentState::Chosen:
		return srcPaneMarker();
	default:
		return 0;
	}
}

/**
 * @brief Show the merge progress in the result bar's caption.
 */
void CMergeDoc::UpdateMergeResultPaneCaption()
{
	if (m_pMergeResultView == nullptr || m_pMergeResultView->GetSafeHwnd() == nullptr)
		return;
	CWnd* pBar = m_pMergeResultView->GetParent();
	if (pBar == nullptr)
		return;
	String sCaption = _("Merge Result Pane");
	if (m_bResultBuilt)
	{
		int nUnresolved = 0, nConflicts = 0, nWhiteSpaceOnly = 0;
		GetResultUnresolvedCounts(nUnresolved, nConflicts, nWhiteSpaceOnly);
		if (nUnresolved == 0)
			sCaption += _T(" - ") + _("All differences resolved");
		else
		{
			sCaption += _T(" - ") + strutils::format_string2(
				_("%1 unresolved difference(s), %2 conflict(s)"),
				strutils::format(_T("%d"), nUnresolved),
				strutils::format(_T("%d"), nConflicts));
			if (nWhiteSpaceOnly > 0)
				sCaption += strutils::format_string1(
					_(" (%1 whitespace only)"),
					strutils::format(_T("%d"), nWhiteSpaceOnly));
		}
	}
	pBar->SetWindowText(sCaption.c_str());
	// the bar draws its caption in the non-client area, which a plain
	// WM_SETTEXT does not repaint
	pBar->SetWindowPos(nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE |
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

/**
 * @brief Conflict where left, middle and right differ only in white space?
 */
bool CMergeDoc::IsResultDiffWhiteSpaceOnly(const DIFFRANGE* pdi) const
{
	auto strippedText = [&](int nPane)
	{
		const String text = GetPaneApparentLinesText(nPane, pdi->dbegin, pdi->dend, nullptr);
		String stripped;
		stripped.reserve(text.length());
		for (const tchar_t ch : text)
		{
			if (ch != _T(' ') && ch != _T('\t') && ch != _T('\r') && ch != _T('\n'))
				stripped += ch;
		}
		return stripped;
	};
	const String sMiddle = strippedText(1);
	return strippedText(0) == sMiddle && sMiddle == strippedText(2);
}

/**
 * @brief Line-ending style for the result, kdiff3 style: prefer the style
 * of the side that changed it (base = middle pane).
 */
CRLFSTYLE CMergeDoc::PickResultCRLFStyle() const
{
	const CRLFSTYLE s0 = m_ptBuf[0]->GetCRLFMode();
	const CRLFSTYLE s1 = m_ptBuf[1]->GetCRLFMode();
	const CRLFSTYLE s2 = m_ptBuf[2]->GetCRLFMode();
	CRLFSTYLE crlfStyle;
	if (s0 == s1)
		crlfStyle = s2; // right side changed the style (or nobody did)
	else if (s1 == s2)
		crlfStyle = s0; // left side changed the style
	else if (s0 == s2)
		crlfStyle = s0; // both sides agree against the base
	else
		crlfStyle = s1; // undecidable: keep the base style
	if (crlfStyle == CRLFSTYLE::AUTOMATIC || crlfStyle == CRLFSTYLE::MIXED)
		crlfStyle = s1;
	if (crlfStyle == CRLFSTYLE::AUTOMATIC || crlfStyle == CRLFSTYLE::MIXED)
		crlfStyle = CRLFSTYLE::DOS;
	return crlfStyle;
}

/**
 * @brief Encoding for the result, kdiff3 style: prefer the encoding of
 * the side that changed it (base = middle pane).
 */
void CMergeDoc::PickResultEncoding()
{
	const FileTextEncoding& e0 = m_ptBuf[0]->getEncoding();
	const FileTextEncoding& e1 = m_ptBuf[1]->getEncoding();
	const FileTextEncoding& e2 = m_ptBuf[2]->getEncoding();
	auto sameEncoding = [](const FileTextEncoding& a, const FileTextEncoding& b)
	{
		return a.m_unicoding == b.m_unicoding && a.m_codepage == b.m_codepage;
	};
	if (sameEncoding(e0, e1) && !sameEncoding(e1, e2))
		m_ptResultBuf->setEncoding(e2); // right side changed the encoding
	else if (sameEncoding(e1, e2) && !sameEncoding(e0, e1))
		m_ptResultBuf->setEncoding(e0); // left side changed the encoding
	else
		m_ptResultBuf->setEncoding(e1); // agreement or undecidable: keep base
}

/**
 * @brief Rewrite every unresolved segment's buffer lines to match its
 * current display form: compact placeholder, or the full conflict section
 * (always full for segments without a linked difference).
 *
 * The rewrites are history-less, so the undo history is dropped — the
 * positions recorded in it no longer match the text.
 */
void CMergeDoc::ReRenderResultConflictSegments()
{
	if (m_ptResultBuf == nullptr || !m_ptResultBuf->IsInitialized())
		return;
	CMergeResultTextBuffer::InternalOpGuard guard(*m_ptResultBuf);
	CCrystalTextView* pSource = (m_pMergeResultView != nullptr &&
		m_pMergeResultView->GetSafeHwnd() != nullptr) ? m_pMergeResultView : nullptr;
	int nDelta = 0;
	for (auto& seg : m_resultSegments)
	{
		seg.nStartLine += nDelta;
		if (seg.state != ResultSegmentState::Conflict &&
			seg.state != ResultSegmentState::Unresolved)
			continue;
		int nNewLines = 0;
		const String newText = GetResultSegmentDisplayText(seg, &nNewLines);
		const int nLineCount = m_ptResultBuf->GetLineCount();
		if (seg.nStartLine < 0 || seg.nStartLine >= nLineCount || seg.nLines < 0 ||
			seg.nStartLine + seg.nLines > nLineCount)
			continue; // defensive: leave inconsistent segments alone
		if (seg.nLines > 0)
		{
			if (seg.nStartLine + seg.nLines < nLineCount)
				m_ptResultBuf->DeleteText(pSource, seg.nStartLine, 0,
					seg.nStartLine + seg.nLines, 0, CE_ACTION_UNKNOWN, false, false);
			else
				m_ptResultBuf->DeleteText(pSource, seg.nStartLine, 0,
					nLineCount - 1, m_ptResultBuf->GetLineLength(nLineCount - 1),
					CE_ACTION_UNKNOWN, false, false);
		}
		if (!newText.empty())
		{
			int nEndLine = 0, nEndChar = 0;
			m_ptResultBuf->InsertText(pSource, seg.nStartLine, 0, newText.c_str(),
				newText.length(), nEndLine, nEndChar, CE_ACTION_UNKNOWN, false);
		}
		nDelta += nNewLines - seg.nLines;
		seg.nLines = nNewLines;
	}
	m_ptResultBuf->ClearUndoBuffer();
	OnResultUndoStackCleared();
	if (m_pMergeResultView != nullptr && m_pMergeResultView->GetSafeHwnd() != nullptr)
		m_pMergeResultView->Invalidate();
}

/**
 * @brief Switch between compact placeholders and full conflict sections
 * in the result pane.
 */
void CMergeDoc::SetResultShowFullConflicts(bool bShow)
{
	if (bShow == m_bResultShowFullConflicts)
		return;
	m_bResultShowFullConflicts = bShow;
	if (IsMergeResultPaneActive())
		ReRenderResultConflictSegments();
}

/**
 * @brief Text of the given result buffer lines, with each line's own EOL
 * (nothing is appended to a line that has none, i.e. the very last line).
 */
String CMergeDoc::GetResultBufferLinesText(int nStartLine, int nLines) const
{
	String text;
	const int nEndExcl = (std::min)(nStartLine + nLines, m_ptResultBuf->GetLineCount());
	for (int nLine = (std::max)(nStartLine, 0); nLine < nEndExcl; ++nLine)
	{
		if (m_ptResultBuf->GetLineFlags(nLine) & LF_GHOST)
			continue;
		const int nLength = m_ptResultBuf->GetLineLength(nLine);
		if (nLength > 0)
			text.append(m_ptResultBuf->GetLineChars(nLine), nLength);
		const tchar_t* pszEol = m_ptResultBuf->GetLineEol(nLine);
		if (pszEol != nullptr && *pszEol != _T('\0'))
			text += pszEol;
	}
	return text;
}

/**
 * @brief The text that saving writes: the result buffer's content, with
 * every compact placeholder replaced by its full conflict section.
 */
String CMergeDoc::BuildExpandedResultText() const
{
	String text;
	int nCovered = 0;
	for (const auto& seg : m_resultSegments)
	{
		if (seg.nStartLine > nCovered) // defensive: uncovered gap
			text += GetResultBufferLinesText(nCovered, seg.nStartLine - nCovered);
		const bool bPlaceholder = (seg.state == ResultSegmentState::Conflict ||
			seg.state == ResultSegmentState::Unresolved);
		if (bPlaceholder && seg.diffIdx >= 0 && !m_bResultShowFullConflicts)
			text += seg.blockText; // buffer shows the compact placeholder
		else
			text += GetResultBufferLinesText(seg.nStartLine, seg.nLines);
		nCovered = (std::max)(nCovered, seg.nStartLine + seg.nLines);
	}
	text += GetResultBufferLinesText(nCovered,
		m_ptResultBuf->GetLineCount() - nCovered);
	return text;
}

/**
 * @brief Initialize the merge result from an existing output file that
 * contains conflict sections — one written by a version control system
 * (e.g. git's pre-populated merge target) or by a previous, partially
 * finished session.
 *
 * Resolved text is kept verbatim. Every conflict section becomes an
 * unresolved segment; a section whose contents match a difference of the
 * current comparison is linked to it (Choose commands and pane sync work),
 * others stay unlinked and are resolved by editing them directly.
 *
 * @param [out] text Content for the result buffer.
 * @return true when the result was initialized from the file.
 */
bool CMergeDoc::TryResumeMergeResultFromOutput(String& text)
{
	if (paths::DoesPathExist(m_strSaveAsPath) != paths::IS_EXISTING_FILE)
		return false;

	// Read the file; without a BOM assume the encoding picked for the result
	struct FileLine { String text; String eol; };
	std::vector<FileLine> lines;
	{
		UniMemFile file;
		if (!file.OpenReadOnly(m_strSaveAsPath))
			return false;
		file.ReadBom();
		if (!file.HasBom())
		{
			const FileTextEncoding& enc = m_ptResultBuf->getEncoding();
			file.SetUnicoding(enc.m_unicoding);
			file.SetCodepage(enc.m_codepage);
		}
		String sLine, sEol;
		bool bLossy = false;
		while (file.ReadString(sLine, sEol, &bLossy))
			lines.push_back({ sLine, sEol });
	}

	// Parse the conflict sections (git format; the ||||||| base block of
	// diff3-style sections is optional)
	struct Section { size_t nBegin = 0, nEnd = 0; String ours, base, theirs; bool bHasBase = false; };
	std::vector<Section> sections;
	auto isMarker = [](const String& s, const tchar_t* pszMarker)
	{
		return s.compare(0, 7, pszMarker) == 0 &&
			(s.length() == 7 || s[7] == _T(' '));
	};
	enum class ParseState { Outside, Ours, Base, Theirs };
	ParseState state = ParseState::Outside;
	Section cur;
	for (size_t i = 0; i < lines.size(); ++i)
	{
		const String& s = lines[i].text;
		switch (state)
		{
		case ParseState::Outside:
			if (isMarker(s, _T("<<<<<<<")))
			{
				cur = Section();
				cur.nBegin = i;
				state = ParseState::Ours;
			}
			break;
		case ParseState::Ours:
			if (isMarker(s, _T("|||||||")))
			{
				cur.bHasBase = true;
				state = ParseState::Base;
			}
			else if (s == _T("======="))
				state = ParseState::Theirs;
			else
			{
				cur.ours += s;
				cur.ours += _T('\n');
			}
			break;
		case ParseState::Base:
			if (s == _T("======="))
				state = ParseState::Theirs;
			else
			{
				cur.base += s;
				cur.base += _T('\n');
			}
			break;
		case ParseState::Theirs:
			if (isMarker(s, _T(">>>>>>>")))
			{
				cur.nEnd = i;
				sections.push_back(cur);
				state = ParseState::Outside;
			}
			else
			{
				cur.theirs += s;
				cur.theirs += _T('\n');
			}
			break;
		}
	}
	if (state != ParseState::Outside || sections.empty())
		return false; // no (complete) conflict sections: start fresh

	const String msg = strutils::format_string2(
		_("The merge output file\n%1\nalready contains a merge with %2 conflict section(s).\n\nContinue from it? Choosing No starts a new merge from the compared files."),
		m_strSaveAsPath, strutils::format(_T("%d"), static_cast<int>(sections.size())));
	if (ShowMessageBox(msg, MB_YESNO | MB_ICONQUESTION) != IDYES)
		return false;

	// The modal prompt pumps messages: the diff list may have been
	// replaced while it was open. Everything below uses the current list.
	m_resultDiffToSegment.assign(m_diffList.GetSize(), -1);

	// Match each section to a difference by content (in order; both lists
	// are ordered, so a greedy scan is enough). EOL styles are ignored.
	auto normalizeEols = [](const String& s)
	{
		String r;
		r.reserve(s.length());
		for (size_t i = 0; i < s.length(); ++i)
		{
			if (s[i] == _T('\r'))
			{
				r += _T('\n');
				if (i + 1 < s.length() && s[i + 1] == _T('\n'))
					++i;
			}
			else
				r += s[i];
		}
		return r;
	};
	const int nDiffCount = m_diffList.GetSize();
	std::vector<int> sectionDiff(sections.size(), -1);
	int nSearchFrom = 0;
	for (size_t i = 0; i < sections.size(); ++i)
	{
		for (int nDiff = nSearchFrom; nDiff < nDiffCount; ++nDiff)
		{
			const DIFFRANGE* pdi = m_diffList.DiffRangeAt(nDiff);
			if (sections[i].ours == normalizeEols(GetPaneApparentLinesText(0, pdi->dbegin, pdi->dend, nullptr)) &&
				sections[i].theirs == normalizeEols(GetPaneApparentLinesText(2, pdi->dbegin, pdi->dend, nullptr)) &&
				(!sections[i].bHasBase ||
				 sections[i].base == normalizeEols(GetPaneApparentLinesText(1, pdi->dbegin, pdi->dend, nullptr))))
			{
				sectionDiff[i] = nDiff;
				nSearchFrom = nDiff + 1;
				break;
			}
		}
	}

	// Build the buffer text and the segment table from the file
	int nCurLine = 0;
	auto appendResolved = [&](size_t nBegin, size_t nEndExcl)
	{
		if (nBegin >= nEndExcl)
			return;
		MergeResultSegment seg;
		seg.diffIdx = -1;
		seg.state = ResultSegmentState::Common;
		seg.nStartLine = nCurLine;
		seg.nLines = static_cast<int>(nEndExcl - nBegin);
		for (size_t i = nBegin; i < nEndExcl; ++i)
		{
			text += lines[i].text;
			text += lines[i].eol;
		}
		m_resultSegments.push_back(seg);
		nCurLine += seg.nLines;
	};
	size_t iLine = 0;
	for (size_t i = 0; i < sections.size(); ++i)
	{
		const Section& sec = sections[i];
		appendResolved(iLine, sec.nBegin);
		MergeResultSegment seg;
		seg.diffIdx = sectionDiff[i];
		const DIFFRANGE* pdi = (seg.diffIdx >= 0) ?
			m_diffList.DiffRangeAt(seg.diffIdx) : nullptr;
		// a parsed section is always a conflict: whoever wrote the markers
		// declared it one, and only Conflict segments save as markers
		seg.state = ResultSegmentState::Conflict;
		seg.bWhiteSpaceOnly = (pdi != nullptr && pdi->op == OP_DIFF) &&
			IsResultDiffWhiteSpaceOnly(pdi);
		// stash the section exactly as it appears in the file, so saving
		// round-trips it even while the display is compact
		for (size_t nLine = sec.nBegin; nLine <= sec.nEnd; ++nLine)
		{
			seg.blockText += lines[nLine].text;
			seg.blockText += lines[nLine].eol;
		}
		seg.nBlockLines = static_cast<int>(sec.nEnd - sec.nBegin + 1);
		seg.nStartLine = nCurLine;
		text += GetResultSegmentDisplayText(seg, &seg.nLines);
		if (seg.diffIdx >= 0)
			m_resultDiffToSegment[seg.diffIdx] = static_cast<int>(m_resultSegments.size());
		m_resultSegments.push_back(seg);
		nCurLine += seg.nLines;
		iLine = sec.nEnd + 1;
	}
	appendResolved(iLine, lines.size());
	return true;
}

/**
 * @brief Is the current diff list identical to the one the result was
 * generated from?
 */
bool CMergeDoc::ResultDiffListUnchanged() const
{
	const int nDiffCount = m_diffList.GetSize();
	if (nDiffCount != static_cast<int>(m_resultDiffSnapshot.size()))
		return false;
	for (int nDiff = 0; nDiff < nDiffCount; ++nDiff)
	{
		const DIFFRANGE* pdi = m_diffList.DiffRangeAt(nDiff);
		const ResultDiffSnapshot& snap = m_resultDiffSnapshot[nDiff];
		if (pdi->dbegin != snap.dbegin || pdi->dend != snap.dend ||
			static_cast<int>(pdi->op) != snap.op)
			return false;
	}
	return true;
}

/**
 * @brief Find the segment containing the given result buffer line.
 */
const MergeResultSegment* CMergeDoc::GetResultSegmentByLine(int nLine) const
{
	const int nSegment = GetResultSegmentIndexByLine(nLine);
	return nSegment >= 0 ? &m_resultSegments[nSegment] : nullptr;
}

/**
 * @brief Find the segment belonging to the given difference.
 */
const MergeResultSegment* CMergeDoc::GetResultSegmentByDiff(int nDiff) const
{
	if (nDiff < 0 || nDiff >= static_cast<int>(m_resultDiffToSegment.size()))
		return nullptr;
	const int nSegment = m_resultDiffToSegment[nDiff];
	if (nSegment < 0 || nSegment >= static_cast<int>(m_resultSegments.size()))
		return nullptr;
	return &m_resultSegments[nSegment];
}

/**
 * @brief Replace the result segment of nDiff with the content of the
 * given source pane.
 */
void CMergeDoc::ResultChooseSource(int nDiff, int srcPane, bool bGroupWithPrevious /*= false*/)
{
	ResultChooseSources(nDiff, { srcPane }, bGroupWithPrevious);
}

/**
 * @brief Replace the result segment of nDiff with the concatenated
 * content of the given source panes, in order.
 *
 * Like KDiff3, several panes may be selected for one difference: the
 * typical case is a conflict where both sides added something (e.g. a
 * new function each) and the merge should keep both. An empty selection
 * restores the unresolved conflict placeholder (or removes the block
 * for a non-conflicting difference).
 */
void CMergeDoc::ResultChooseSources(int nDiff, const std::vector<int>& srcPanes,
	bool bGroupWithPrevious /*= false*/)
{
	if (!IsMergeResultPaneActive())
		return;
	for (int srcPane : srcPanes)
	{
		if (srcPane < 0 || srcPane >= m_nBuffers)
			return;
	}
	if (nDiff < 0 || nDiff >= static_cast<int>(m_resultDiffToSegment.size()))
		return;
	const int nSegment = m_resultDiffToSegment[nDiff];
	if (nSegment < 0)
		return;
	MergeResultSegment& seg = m_resultSegments[nSegment];
	// A failed rescan (file error, files turned binary) empties the diff
	// list without running the sever logic, leaving the links stale:
	// never trust nDiff against the current list
	const DIFFRANGE* pdi = (nDiff < m_diffList.GetSize()) ?
		m_diffList.DiffRangeAt(nDiff) : nullptr;
	if (pdi == nullptr)
		return;

	// Validate the segment against the buffer before touching it: the
	// buffer is freely editable, so never trust the segment table enough
	// to run an out-of-range delete (buffer bounds are only ASSERTed).
	const int nLineCount = m_ptResultBuf->GetLineCount();
	if (seg.nStartLine < 0 || seg.nLines < 0 ||
		seg.nStartLine + seg.nLines > nLineCount ||
		seg.nStartLine >= nLineCount)
	{
		seg.state = ResultSegmentState::Edited;
		return;
	}

	int nNewLines = 0;
	String text;
	bool bBackToConflict = false;
	std::vector<int> srcPaneLines;
	srcPaneLines.reserve(srcPanes.size());
	for (int srcPane : srcPanes)
	{
		int nPaneLines = 0;
		text += GetPaneApparentLinesText(srcPane, pdi->dbegin, pdi->dend, &nPaneLines);
		srcPaneLines.push_back(nPaneLines);
		nNewLines += nPaneLines;
	}
	if (srcPanes.empty())
	{
		// deselected everything: the difference is unresolved again
		if (pdi->op == OP_DIFF)
			seg.blockText = GetResultConflictBlockText(nDiff, seg.bWhiteSpaceOnly,
				&seg.nBlockLines);
		else
		{
			// non-conflict: never conflict markers, see BuildMergeResult
			int srcPane = m_diffList.GetMergeableSrcIndex(nDiff, 1);
			if (srcPane == -1)
				srcPane = 1;
			seg.blockText = GetPaneApparentLinesText(srcPane, pdi->dbegin,
				pdi->dend, &seg.nBlockLines);
		}
		// state is updated below; only the state matters for the display
		MergeResultSegment tmpSeg = seg;
		tmpSeg.state = (pdi->op == OP_DIFF) ?
			ResultSegmentState::Conflict : ResultSegmentState::Unresolved;
		text = GetResultSegmentDisplayText(tmpSeg, &nNewLines);
		bBackToConflict = true;
	}

	CMergeResultTextBuffer::InternalOpGuard guard(*m_ptResultBuf);
	CCrystalTextView* pSource = (m_pMergeResultView != nullptr &&
		m_pMergeResultView->GetSafeHwnd() != nullptr) ? m_pMergeResultView : nullptr;

	m_ptResultBuf->BeginUndoGroup(bGroupWithPrevious);
	if (seg.nLines > 0)
	{
		if (seg.nStartLine + seg.nLines < nLineCount)
		{
			// Normal case: a line exists after the segment (the generated
			// text ends every line with an EOL)
			m_ptResultBuf->DeleteText(pSource, seg.nStartLine, 0,
				seg.nStartLine + seg.nLines, 0, CE_ACTION_UNKNOWN, true, false);
		}
		else
		{
			// The segment reaches the physical end of the buffer (the
			// user removed the trailing newline): delete to the end of
			// the last line instead. The inserted text below ends with
			// an EOL, restoring the trailing empty line.
			m_ptResultBuf->DeleteText(pSource, seg.nStartLine, 0,
				nLineCount - 1, m_ptResultBuf->GetLineLength(nLineCount - 1),
				CE_ACTION_UNKNOWN, true, false);
		}
	}
	if (!text.empty())
	{
		int nEndLine = 0, nEndChar = 0;
		m_ptResultBuf->InsertText(pSource, seg.nStartLine, 0, text.c_str(),
			text.length(), nEndLine, nEndChar, CE_ACTION_UNKNOWN, true);
	}
	m_ptResultBuf->FlushUndoGroup(pSource);

	const int nDelta = nNewLines - seg.nLines;
	seg.state = !bBackToConflict ? ResultSegmentState::Chosen :
		(pdi->op == OP_DIFF ? ResultSegmentState::Conflict : ResultSegmentState::Unresolved);
	seg.srcPanes = srcPanes;
	seg.srcPaneLines = std::move(srcPaneLines);
	seg.nLines = nNewLines;
	if (!bBackToConflict)
	{
		seg.blockText.clear();
		seg.nBlockLines = 0;
	}
	// the freshly written content is this segment's baseline: only later
	// hand edits should show as modified in the margin
	seg.nBaseRevision = m_ptResultBuf->GetCurrentRevisionNumber();
	if (nDelta != 0)
	{
		for (size_t i = nSegment + 1; i < m_resultSegments.size(); ++i)
			m_resultSegments[i].nStartLine += nDelta;
	}

	if (m_pMergeResultView != nullptr && m_pMergeResultView->GetSafeHwnd() != nullptr)
		m_pMergeResultView->Invalidate();
	UpdateMergeResultPaneCaption();
}

/**
 * @brief Toggle a source pane for the given difference (KDiff3 style).
 *
 * Pressing 1/2/3 adds that pane's block to the difference's selection,
 * or removes it when already selected. Blocks are concatenated in the
 * order they were selected. Removing the last selection restores the
 * unresolved conflict placeholder.
 */
void CMergeDoc::ResultToggleSource(int nDiff, int srcPane)
{
	const MergeResultSegment* pSegment = GetResultSegmentByDiff(nDiff);
	if (pSegment == nullptr)
		return;
	std::vector<int> srcPanes;
	if (pSegment->state == ResultSegmentState::Auto ||
		pSegment->state == ResultSegmentState::Chosen)
		srcPanes = pSegment->srcPanes;
	// (a hand-edited or unresolved segment starts a fresh selection)
	const auto it = std::find(srcPanes.begin(), srcPanes.end(), srcPane);
	if (it != srcPanes.end())
		srcPanes.erase(it);
	else
		srcPanes.push_back(srcPane);
	ResultChooseSources(nDiff, srcPanes);
}

/**
 * @brief Resolve every difference that has nothing chosen for it yet by
 * taking the given pane. Covers both real conflicts and differences that
 * are simply still unresolved (the usual case without auto-merge).
 */
void CMergeDoc::ResultChooseAllConflicts(int srcPane)
{
	if (!IsMergeResultPaneActive())
		return;
	bool bGroupWithPrevious = false;
	for (int nDiff = 0; nDiff < static_cast<int>(m_resultDiffToSegment.size()); ++nDiff)
	{
		const MergeResultSegment* pSegment = GetResultSegmentByDiff(nDiff);
		if (pSegment != nullptr &&
			(pSegment->state == ResultSegmentState::Conflict ||
			 pSegment->state == ResultSegmentState::Unresolved))
		{
			ResultChooseSource(nDiff, srcPane, bGroupWithPrevious);
			bGroupWithPrevious = true;
		}
	}
}

/**
 * @brief Save the merge result.
 *
 * The default target path is the "save as" path given when the document
 * was opened (conflict file or /o output path). Otherwise (or when
 * bSaveAs) the user is prompted for a path.
 */
bool CMergeDoc::SaveMergeResult(bool bSaveAs)
{
	if (m_ptResultBuf == nullptr || !m_ptResultBuf->IsInitialized())
		return false;

	const int nUnresolved = GetResultUnresolvedCount();
	if (nUnresolved > 0)
	{
		const String msg = strutils::format_string1(
			_("There are still %1 unresolved difference(s) in the merge result.\n\nUnresolved conflicts are saved as conflict sections (<<<<<<<), which preserve all versions and can be resolved later in any editor; other unresolved differences are saved with their automatic merge content.\n\nSave the result anyway?"),
			strutils::format(_T("%d"), nUnresolved));
		if (ShowMessageBox(msg, MB_YESNO | MB_ICONWARNING) != IDYES)
			return false;
	}

	String strPath = m_strSaveAsPath;
	if (bSaveAs || strPath.empty())
	{
		String sDefault = !m_strSaveAsPath.empty() ? m_strSaveAsPath : m_filePaths.GetMiddle();
		HWND hwndParent = (m_pMergeResultView != nullptr) ? m_pMergeResultView->GetSafeHwnd() : nullptr;
		String strSelected;
		if (!SelectFile(hwndParent, strSelected, false, sDefault.c_str(),
				_("Save Merge Result As")))
			return false;
		strPath = strSelected;
	}

	// Same guards as DoSave for the compared files: offer to clear a
	// read-only target, honor the backup option, and repack the result
	// with the unpacker the files were opened with
	bool bApplyToAll = false;
	if (CMergeApp::HandleReadonlySave(strPath, false, bApplyToAll) == IDCANCEL)
		return false;
	if (!CMergeApp::CreateBackup(false, strPath))
		return false;

	PackingInfo infoTempUnpacker = m_infoUnpacker;
	String sError;
	auto saveBuffer = [&](CDiffTextBuffer& buf) -> int
	{
		if (m_resultSaveEolStyle == CRLFSTYLE::AUTOMATIC)
		{
			// Keep each line's own EOL, so the output differs from the
			// inputs as little as possible (e.g. minimal diffs in version
			// control). SaveToFile writes per-line EOLs when the buffer
			// reports MIXED; flip the mode only for the duration of the
			// write, keeping the nominal style as the default for typing.
			const CRLFSTYLE nOldMode = buf.GetCRLFMode();
			buf.SetCRLFMode(CRLFSTYLE::MIXED);
			const int nResult = buf.SaveToFile(strPath, false, sError, infoTempUnpacker);
			buf.SetCRLFMode(nOldMode);
			return nResult;
		}
		return buf.SaveToFile(strPath, false, sError, infoTempUnpacker,
			m_resultSaveEolStyle);
	};

	// Compact placeholders are display-only: the file gets the full
	// conflict sections, written through a temporary buffer so the
	// visible one is not disturbed
	bool bNeedExpansion = false;
	for (const auto& seg : m_resultSegments)
	{
		if ((seg.state == ResultSegmentState::Conflict ||
			 seg.state == ResultSegmentState::Unresolved) &&
			seg.diffIdx >= 0 && !m_bResultShowFullConflicts)
		{
			bNeedExpansion = true;
			break;
		}
	}
	int nRetVal;
	if (bNeedExpansion)
	{
		CDiffTextBuffer expandedBuf(this, 1);
		expandedBuf.InitNew(m_ptResultBuf->GetCRLFMode());
		expandedBuf.setEncoding(m_ptResultBuf->getEncoding());
		expandedBuf.SetTempPath(env::GetTemporaryPath());
		const String expanded = BuildExpandedResultText();
		if (!expanded.empty())
		{
			int nEndLine = 0, nEndChar = 0;
			expandedBuf.InsertText(nullptr, 0, 0, expanded.c_str(),
				expanded.length(), nEndLine, nEndChar, CE_ACTION_UNKNOWN, false);
		}
		nRetVal = saveBuffer(expandedBuf);
		expandedBuf.FreeAll();
	}
	else
		nRetVal = saveBuffer(*m_ptResultBuf);
	if (nRetVal != SAVE_DONE)
	{
		const String msg = strutils::format_string2(
			_("Saving the merge result to\n%1\nfailed.\n%2"), strPath, sError);
		ShowMessageBox(msg, MB_OK | MB_ICONERROR);
		return false;
	}
	m_ptResultBuf->SetModified(false);
	m_bResultSaved = true;
	return true;
}

/**
 * @brief Current difference changed: scroll the result pane to it.
 */
void CMergeDoc::OnResultPaneCurrentDiffChanged(int nDiff)
{
	if (!IsMergeResultPaneActive() || nDiff < 0)
		return;
	m_pMergeResultView->ScrollToDiff(nDiff);
}

/////////////////////////////////////////////////////////////////////////////
// Result buffer edit tracking (called from CMergeResultTextBuffer)

/**
 * @brief Index of segment containing the line, -1 if none.
 *
 * The segment table is sorted by start line (segments cover the buffer
 * contiguously; empty segments share their start line with the next
 * segment), so a binary search finds the candidate. This is called from
 * the paint path for every visible line, so it must not scan linearly.
 */
int CMergeDoc::GetResultSegmentIndexByLine(int nLine) const
{
	if (nLine < 0 || m_resultSegments.empty())
		return -1;
	// first segment starting after nLine
	auto it = std::upper_bound(m_resultSegments.begin(), m_resultSegments.end(), nLine,
		[](int nLine2, const MergeResultSegment& seg) { return nLine2 < seg.nStartLine; });
	// walk back over empty segments to the nearest non-empty candidate
	while (it != m_resultSegments.begin())
	{
		--it;
		if (it->nLines > 0)
		{
			if (nLine >= it->nStartLine && nLine < it->nStartLine + it->nLines)
				return static_cast<int>(it - m_resultSegments.begin());
			return -1;
		}
	}
	return -1;
}

/**
 * @brief A new undo group begins: snapshot the segment table (state
 * before the group's operations) and drop any stale redo snapshots.
 */
void CMergeDoc::OnResultUndoGroupStart(int nUndoPos)
{
	m_resultSegUndo.erase(m_resultSegUndo.lower_bound(nUndoPos), m_resultSegUndo.end());
	m_resultSegRedo.erase(m_resultSegRedo.lower_bound(nUndoPos), m_resultSegRedo.end());
	m_resultSegUndo[nUndoPos] = m_resultSegments;
}

/**
 * @brief An undo group was undone: restore the table from before the
 * group, keeping the current table for redo.
 */
void CMergeDoc::OnResultUndone(int nUndoPos)
{
	const auto it = m_resultSegUndo.find(nUndoPos);
	if (it == m_resultSegUndo.end())
		return;
	m_resultSegRedo[nUndoPos] = m_resultSegments;
	m_resultSegments = it->second;
	if (m_pMergeResultView != nullptr && m_pMergeResultView->GetSafeHwnd() != nullptr)
		m_pMergeResultView->Invalidate();
	UpdateMergeResultPaneCaption();
}

/**
 * @brief An undo group was redone: restore the table from after the group.
 */
void CMergeDoc::OnResultRedone(int nUndoPos)
{
	const auto it = m_resultSegRedo.find(nUndoPos);
	if (it == m_resultSegRedo.end())
		return;
	m_resultSegments = it->second;
	if (m_pMergeResultView != nullptr && m_pMergeResultView->GetSafeHwnd() != nullptr)
		m_pMergeResultView->Invalidate();
	UpdateMergeResultPaneCaption();
}

void CMergeDoc::OnResultUndoStackCleared()
{
	m_resultSegUndo.clear();
	m_resultSegRedo.clear();
}

void CMergeDoc::OnResultLineEdited(int nLine)
{
	const int nSegment = GetResultSegmentIndexByLine(nLine);
	if (nSegment < 0)
		return;
	MergeResultSegment& seg = m_resultSegments[nSegment];
	// Linked placeholder segments cannot get here (their lines reject
	// edits); editing an unlinked conflict section is its resolution.
	const bool bTracked = seg.diffIdx >= 0 ||
		seg.state == ResultSegmentState::Conflict ||
		seg.state == ResultSegmentState::Unresolved;
	if (bTracked && seg.state != ResultSegmentState::Edited)
	{
		seg.state = ResultSegmentState::Edited;
		if (m_pMergeResultView != nullptr && m_pMergeResultView->GetSafeHwnd() != nullptr)
			m_pMergeResultView->Invalidate();
		UpdateMergeResultPaneCaption();
	}
}

/**
 * @brief nCount lines were inserted after position nLine by a user edit.
 */
void CMergeDoc::OnResultBufferInsertedLines(int nLine, int nCount)
{
	const int nSegment = GetResultSegmentIndexByLine(nLine);
	if (nSegment >= 0)
	{
		MergeResultSegment& seg = m_resultSegments[nSegment];
		seg.nLines += nCount;
		if (seg.diffIdx >= 0)
			seg.state = ResultSegmentState::Edited;
		for (size_t i = nSegment + 1; i < m_resultSegments.size(); ++i)
			m_resultSegments[i].nStartLine += nCount;
	}
	else
	{
		for (auto& seg : m_resultSegments)
		{
			if (seg.nStartLine > nLine)
				seg.nStartLine += nCount;
		}
	}
}

/**
 * @brief Lines [nFirstLine, nFirstLine + nCount) were removed whole by a
 * user edit (column-0-to-column-0 deletion); the following line survived
 * unchanged, so no line merge happened.
 */
void CMergeDoc::OnResultBufferDeletedWholeLines(int nFirstLine, int nCount)
{
	const int nRemovedBegin = nFirstLine;
	const int nRemovedEnd = nFirstLine + nCount - 1;
	bool bStateChanged = false;
	for (auto& seg : m_resultSegments)
	{
		const int nSegEnd = seg.nStartLine + seg.nLines - 1;
		if (seg.nLines > 0 && nSegEnd < nRemovedBegin)
			continue; // fully before the removed range
		if (seg.nStartLine > nRemovedEnd)
		{
			seg.nStartLine -= nCount; // fully after the removed range
			continue;
		}
		if (seg.nLines <= 0)
		{
			// empty segment inside the removed range
			if (seg.nStartLine >= nRemovedBegin)
				seg.nStartLine = nRemovedBegin;
			continue;
		}
		// overlaps the removed range
		const int nOverlapBegin = (std::max)(seg.nStartLine, nRemovedBegin);
		const int nOverlapEnd = (std::min)(nSegEnd, nRemovedEnd);
		const int nOverlap = nOverlapEnd - nOverlapBegin + 1;
		if (nOverlap > 0)
		{
			seg.nLines -= nOverlap;
			const bool bTracked = seg.diffIdx >= 0 ||
				seg.state == ResultSegmentState::Conflict ||
				seg.state == ResultSegmentState::Unresolved;
			if (bTracked && seg.state != ResultSegmentState::Edited)
			{
				seg.state = ResultSegmentState::Edited;
				bStateChanged = true;
			}
		}
		// the surviving tail of a segment starting inside the removed
		// range now begins at the first line after the deletion point
		if (seg.nStartLine >= nRemovedBegin)
			seg.nStartLine = nRemovedBegin;
	}
	if (bStateChanged)
	{
		if (m_pMergeResultView != nullptr && m_pMergeResultView->GetSafeHwnd() != nullptr)
			m_pMergeResultView->Invalidate();
		UpdateMergeResultPaneCaption();
	}
}

/**
 * @brief Lines (nStartLine, nStartLine + nCount] were removed by a user edit.
 */
void CMergeDoc::OnResultBufferDeletedLines(int nStartLine, int nCount)
{
	// Line indices [nRemovedBegin, nRemovedEnd] no longer exist; the text
	// of lines nStartLine and nRemovedEnd merged into line nStartLine.
	const int nRemovedBegin = nStartLine + 1;
	const int nRemovedEnd = nStartLine + nCount;
	for (auto& seg : m_resultSegments)
	{
		const int nSegEnd = seg.nStartLine + seg.nLines - 1;
		if (seg.nLines > 0 && nSegEnd < nRemovedBegin)
			continue; // fully before the removed range
		if (seg.nStartLine > nRemovedEnd)
		{
			seg.nStartLine -= nCount; // fully after the removed range
			continue;
		}
		if (seg.nLines <= 0)
		{
			// empty segment inside the removed range
			if (seg.nStartLine >= nRemovedBegin)
				seg.nStartLine = nRemovedBegin;
			continue;
		}
		// overlaps the removed range
		const int nOverlapBegin = (std::max)(seg.nStartLine, nRemovedBegin);
		const int nOverlapEnd = (std::min)(nSegEnd, nRemovedEnd);
		const int nOverlap = nOverlapEnd - nOverlapBegin + 1;
		if (nOverlap > 0)
		{
			seg.nLines -= nOverlap;
			if (seg.diffIdx >= 0 ||
				seg.state == ResultSegmentState::Conflict ||
				seg.state == ResultSegmentState::Unresolved)
				seg.state = ResultSegmentState::Edited;
		}
		// a segment that started inside the removed range now starts at
		// the first surviving line after the merge point
		if (seg.nStartLine >= nRemovedBegin)
			seg.nStartLine = nRemovedBegin;
	}
	// The segment containing the merge point was edited as well
	OnResultLineEdited(nStartLine);
}

/////////////////////////////////////////////////////////////////////////////
// Command handlers

void CMergeDoc::OnMergeChooseSource(UINT nID)
{
	const int srcPane = nID - ID_MERGE_CHOOSE_LEFT;
	const int nDiff = GetCurrentDiff();
	if (nDiff < 0)
		return;
	ResultToggleSource(nDiff, srcPane);
}

void CMergeDoc::OnUpdateMergeChooseSource(CCmdUI* pCmdUI)
{
	const int nDiff = GetCurrentDiff();
	const MergeResultSegment* pSegment =
		(IsMergeResultPaneActive() && nDiff >= 0) ? GetResultSegmentByDiff(nDiff) : nullptr;
	pCmdUI->Enable(pSegment != nullptr);
	// show which panes are currently selected for this difference
	const int srcPane = pCmdUI->m_nID - ID_MERGE_CHOOSE_LEFT;
	const bool bChecked = pSegment != nullptr &&
		(pSegment->state == ResultSegmentState::Auto ||
		 pSegment->state == ResultSegmentState::Chosen) &&
		std::find(pSegment->srcPanes.begin(), pSegment->srcPanes.end(), srcPane) != pSegment->srcPanes.end();
	pCmdUI->SetCheck(bChecked);
}

void CMergeDoc::OnMergeChooseAllConflicts(UINT nID)
{
	ResultChooseAllConflicts(nID - ID_MERGE_CHOOSE_ALL_LEFT);
}

void CMergeDoc::OnUpdateMergeChooseAllConflicts(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(IsMergeResultPaneActive() && GetResultUnresolvedCount() > 0);
}

void CMergeDoc::OnMergeResultSave()
{
	SaveMergeResult(false);
}

void CMergeDoc::OnMergeResultSaveAs()
{
	SaveMergeResult(true);
}

void CMergeDoc::OnUpdateMergeResultSave(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_ptResultBuf != nullptr && m_ptResultBuf->IsInitialized() &&
		IsMergeResultPaneActive());
}

/**
 * @brief Select the line-ending style for saving the merge result.
 * "As in Sources" (the default) keeps each line's own EOL; a fixed style
 * normalizes the whole output and becomes the EOL for newly typed lines.
 */
void CMergeDoc::OnMergeResultEolStyle(UINT nID)
{
	static const CRLFSTYLE styles[] = { CRLFSTYLE::AUTOMATIC,
		CRLFSTYLE::DOS, CRLFSTYLE::UNIX, CRLFSTYLE::MAC };
	const UINT nIndex = nID - ID_MERGE_RESULT_EOL_ASIS;
	if (nIndex >= _countof(styles))
		return;
	m_resultSaveEolStyle = styles[nIndex];
	if (m_ptResultBuf != nullptr && m_ptResultBuf->IsInitialized())
		m_ptResultBuf->SetCRLFMode(m_resultSaveEolStyle != CRLFSTYLE::AUTOMATIC ?
			m_resultSaveEolStyle : PickResultCRLFStyle());
}

void CMergeDoc::OnUpdateMergeResultEolStyle(CCmdUI* pCmdUI)
{
	static const CRLFSTYLE styles[] = { CRLFSTYLE::AUTOMATIC,
		CRLFSTYLE::DOS, CRLFSTYLE::UNIX, CRLFSTYLE::MAC };
	const UINT nIndex = pCmdUI->m_nID - ID_MERGE_RESULT_EOL_ASIS;
	pCmdUI->Enable(IsMergeResultPaneActive());
	if (nIndex < _countof(styles))
		pCmdUI->SetRadio(styles[nIndex] == m_resultSaveEolStyle);
}

/**
 * @brief Toggle between compact placeholders and full conflict sections.
 */
void CMergeDoc::OnMergeResultShowSections()
{
	SetResultShowFullConflicts(!m_bResultShowFullConflicts);
}

void CMergeDoc::OnUpdateMergeResultShowSections(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(IsMergeResultPaneActive());
	pCmdUI->SetCheck(m_bResultShowFullConflicts);
}

void CMergeDoc::OnMergeStartSession()
{
	StartMergeSession(false);
}

void CMergeDoc::OnUpdateMergeStartSession(CCmdUI* pCmdUI)
{
	// Available when the pane is not shown yet, and as the recovery path
	// when the segment <-> diff links were severed by a rescan
	const bool bLinksSevered = m_bResultBuilt && m_resultDiffSnapshot.empty();
	pCmdUI->Enable(HasMergeResultPane() &&
		(!IsMergeResultPaneVisible() || bLinksSevered));
}
