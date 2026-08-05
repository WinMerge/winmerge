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
	// Deliberately skip CDiffTextBuffer::DeleteText2: it maintains sync
	// points by pane index, which does not apply to the result buffer.
	const bool bResult = CGhostTextBuffer::DeleteText2(pSource, nStartLine,
		nStartPos, nEndLine, nEndPos, nAction, bHistory);
	if (bResult && !IsInternalOp() && nEndLine > nStartLine)
		m_pResultOwnerDoc->OnResultBufferDeletedLines(nStartLine, nEndLine - nStartLine);
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
	if (m_bResultBuilt && bAutoMerge != m_bResultAutoMerge)
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

	CRLFSTYLE crlfStyle = m_ptBuf[1]->GetCRLFMode();
	if (crlfStyle == CRLFSTYLE::AUTOMATIC || crlfStyle == CRLFSTYLE::MIXED)
		crlfStyle = CRLFSTYLE::DOS;
	m_ptResultBuf->InitNew(crlfStyle);
	m_ptResultBuf->setEncoding(m_ptBuf[1]->getEncoding());
	m_ptResultBuf->SetReadOnly(false);
	m_ptResultBuf->SetTempPath(env::GetTemporaryPath()); // needed by SaveToFile

	m_resultSegments.clear();
	OnResultUndoStackCleared(); // buffer undo history is gone as well
	const int nDiffCount = m_diffList.GetSize();
	m_resultDiffToSegment.assign(nDiffCount, -1);

	String text;
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
		// unresolved; with it only true 3-way conflicts do
		if (pdi->op == OP_DIFF ||
			(!m_bResultAutoMerge && pdi->op != OP_TRIVIAL))
		{
			seg.state = ResultSegmentState::Conflict;
			seg.nLines = 1;
			text += _("<Merge Conflict>");
			text += m_ptResultBuf->GetDefaultEol();
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
			seg.nLines = nLines;
		}
		m_resultDiffToSegment[nDiff] = static_cast<int>(m_resultSegments.size());
		m_resultSegments.push_back(seg);
		nCurLine += seg.nLines;
		nApparent = pdi->dend + 1;
	}
	appendCommon(nApparent, nApparentCount);

	if (!text.empty())
	{
		int nEndLine = 0, nEndChar = 0;
		m_ptResultBuf->InsertText(nullptr, 0, 0, text.c_str(), text.length(),
			nEndLine, nEndChar, CE_ACTION_UNKNOWN, false /* no history */);
	}
	m_ptResultBuf->SetModified(false);
	// the generated content is the baseline: no change markers on it
	m_ptResultBuf->AdoptCurrentRevision();
	m_bResultBuilt = true;

	if (m_pMergeResultView != nullptr && m_pMergeResultView->GetSafeHwnd() != nullptr)
	{
		m_pMergeResultView->AttachToBuffer(m_ptResultBuf.get());
		m_pMergeResultView->RefreshOptions();
		m_pMergeResultView->Invalidate();
	}
}

/**
 * @brief Called at the end of Rescan() to keep the result pane in sync.
 */
void CMergeDoc::UpdateMergeResultAfterRescan()
{
	if (m_nBuffers < 3 || m_ptResultBuf == nullptr || m_pMergeResultView == nullptr)
		return;
	if (!IsMergeResultPaneVisible())
		return;
	if (m_bResultBuilt && m_ptResultBuf->IsModified())
	{
		// The user already changed the result: don't discard their work.
		// The diff list changed, so the segment <-> diff links are no
		// longer valid; drop them (Choose commands become unavailable).
		for (auto& seg : m_resultSegments)
		{
			if (seg.diffIdx >= 0)
			{
				seg.diffIdx = -1;
				if (seg.state != ResultSegmentState::Conflict)
					seg.state = ResultSegmentState::Edited;
			}
		}
		m_resultDiffToSegment.assign(m_diffList.GetSize(), -1);
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
 * @brief Number of still unresolved conflict segments.
 */
int CMergeDoc::GetResultUnresolvedCount() const
{
	int nCount = 0;
	for (const auto& seg : m_resultSegments)
	{
		if (seg.state == ResultSegmentState::Conflict)
			++nCount;
	}
	return nCount;
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
	const DIFFRANGE* pdi = m_diffList.DiffRangeAt(nDiff);

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
	for (int srcPane : srcPanes)
	{
		int nPaneLines = 0;
		text += GetPaneApparentLinesText(srcPane, pdi->dbegin, pdi->dend, &nPaneLines);
		nNewLines += nPaneLines;
	}
	if (srcPanes.empty() && pdi->op == OP_DIFF)
	{
		// deselected everything: the conflict is unresolved again
		text = _("<Merge Conflict>");
		text += m_ptResultBuf->GetDefaultEol();
		nNewLines = 1;
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
	seg.state = bBackToConflict ? ResultSegmentState::Conflict : ResultSegmentState::Chosen;
	seg.srcPanes = srcPanes;
	seg.nLines = nNewLines;
	if (nDelta != 0)
	{
		for (size_t i = nSegment + 1; i < m_resultSegments.size(); ++i)
			m_resultSegments[i].nStartLine += nDelta;
	}

	if (m_pMergeResultView != nullptr && m_pMergeResultView->GetSafeHwnd() != nullptr)
		m_pMergeResultView->Invalidate();
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
 * @brief Resolve all remaining conflicts by taking the given pane.
 */
void CMergeDoc::ResultChooseAllConflicts(int srcPane)
{
	if (!IsMergeResultPaneActive())
		return;
	bool bGroupWithPrevious = false;
	for (int nDiff = 0; nDiff < static_cast<int>(m_resultDiffToSegment.size()); ++nDiff)
	{
		const MergeResultSegment* pSegment = GetResultSegmentByDiff(nDiff);
		if (pSegment != nullptr && pSegment->state == ResultSegmentState::Conflict)
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
			_("There are still %1 unresolved conflicts in the merge result.\n\nSave the result anyway?"),
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

	PackingInfo infoUnpacker;
	String sError;
	const int nRetVal = m_ptResultBuf->SaveToFile(strPath, false, sError, infoUnpacker);
	if (nRetVal != SAVE_DONE)
	{
		const String msg = strutils::format_string2(
			_("Saving the merge result to\n%1\nfailed.\n%2"), strPath, sError);
		ShowMessageBox(msg, MB_OK | MB_ICONERROR);
		return false;
	}
	m_ptResultBuf->SetModified(false);
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
	if (seg.diffIdx >= 0 && seg.state != ResultSegmentState::Edited)
	{
		seg.state = ResultSegmentState::Edited;
		if (m_pMergeResultView != nullptr && m_pMergeResultView->GetSafeHwnd() != nullptr)
			m_pMergeResultView->Invalidate();
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
			if (seg.diffIdx >= 0)
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

void CMergeDoc::OnMergeStartSession()
{
	StartMergeSession(false);
}

void CMergeDoc::OnUpdateMergeStartSession(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(HasMergeResultPane() && !IsMergeResultPaneVisible());
}
