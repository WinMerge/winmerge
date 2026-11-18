/**
 * @file  MergeDocResultPane.cpp
 *
 * @brief Implementation of the kdiff3-style merge result pane
 *        (CMergeDoc part + CMergeResultTextBuffer).
 *
 * The merge result is a separate text buffer that is not part of the
 * comparison: its content is generated from the compared buffers and a
 * per-difference resolution state (see MergeResultSegment). The result
 * segment table maps result buffer lines back to differences.
 */

#include "StdAfx.h"
#include "MergeResultPane.h"
#include "MergeDoc.h"
#include "MergeResultView.h"
#include "MergeResultTextBuffer.h"
#include "MergeEditView.h"
#include "MergeEditFrm.h"
#include "MainFrm.h"
#include "FileOrFolderSelect.h"
#include "FileTransform.h"
#include "Environment.h"
#include "paths.h"
#include "Merge.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"
#include "Logger.h"
#include "OptionsDiffOptions.h"
#include "resource.h"
#include "SemanticMergeAnalyzer.h"
#include <algorithm>
#include <iterator>
#include <unordered_set>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

struct MergePanes
{
	int nBasePane;
	int nTheirsPane;
	int nMinePane;
	int nMergeDestPane;
};

static const MergePanes& GetMergePaneMapping(int nBasePane)
{
	static constexpr std::array<MergePanes, 3> mappings = {{
		{ 0, 1, 2, 2 },
		{ 1, 0, 2, 1 },
		{ 2, 1, 0, 0 },
	}};
	ASSERT(nBasePane >= 0 && nBasePane < static_cast<int>(mappings.size()));
	return mappings[(nBasePane >= 0 && nBasePane < static_cast<int>(mappings.size())) ? nBasePane : 1];
}

static bool IsTrackedResultSegment(const MergeResultSegment& seg)
{
	return seg.diffIdx >= 0 || seg.state == ResultSegmentState::Conflict ||
		seg.state == ResultSegmentState::Unresolved;
}

/**
 * @brief Display-only line for a segment whose source has no lines
 * (kdiff3: "<No Lines>"). Like the compact conflict placeholders it
 * lives in the buffer as real text, so Undo/Redo and the segment table
 * snapshots need no special handling; it is omitted when saving.
 */
static String GetResultNoSrcLineText(const tchar_t * pszEol)
{
	return String(_("<No Lines>")) + pszEol;
}

std::array<String, 3> GetMergePaneMappingString(int nBasePane)
{
	const auto& panes = GetMergePaneMapping(nBasePane);
	std::array<String, 3> labels;
	labels[panes.nBasePane] = _("Base File");
	labels[panes.nTheirsPane] = _("Theirs File");
	labels[panes.nMinePane] = _("Mine File");
	return labels;
}

/////////////////////////////////////////////////////////////////////////////
// CMergeDoc merge result pane implementation

/**
 * @brief Is the bar hosting the result view shown?
 * Checks the bar's own visible style so this also works while the
 * frame itself is not visible yet.
 */
bool CMergeDoc::IsMergeResultPaneVisible() const
{
	if (m_pMergeResultView == nullptr || m_pMergeResultView->GetSafeHwnd() == nullptr)
		return false;
	const CWnd* pWndParent = m_pMergeResultView->GetParent();
	const CWnd* pBar = pWndParent != nullptr ? pWndParent->GetParent() : nullptr;
	return pBar != nullptr && (pBar->GetStyle() & WS_VISIBLE) != 0;
}

/**
 * @brief Update pane headers with merge-related labels
 * Assigns descriptive labels to panes based on their merge role (Base, Theirs, Mine)
 * and updates header text accordingly.
 */
void CMergeDoc::UpdateMergePaneHeaders(int nBasePane)
{
	// Give the compared buffers a descriptive name for the merge result
	const auto& panes = GetMergePaneMapping(nBasePane);
	const int panes2[] = { panes.nBasePane, panes.nTheirsPane, panes.nMinePane };
	const String labels[] = { _("Base File"), _("Theirs File"), _("Mine File") };
	for (int i = 0; i < 3; ++i)
	{
		const int pane = panes2[i];
		const String suffix = _T(" - ") + labels[i];

		if (m_strDesc[pane].empty())
		{
			m_strDesc[pane] = m_filePaths[pane] + suffix;
			m_nBufferType[pane] = BUFFERTYPE::NORMAL_NAMED;
		}
		else if (m_nBufferType[pane] == BUFFERTYPE::UNNAMED)
		{
			m_strDesc[pane] += suffix;
		}
		else
		{
			for (const auto& label : labels)
			{
				const String oldSuffix = _T(" - ") + label;
				if (m_strDesc[pane].length() >= oldSuffix.length() &&
					m_strDesc[pane].substr(m_strDesc[pane].length() - oldSuffix.length()) == oldSuffix)
				{
					m_strDesc[pane].replace(m_strDesc[pane].length() - oldSuffix.length(), oldSuffix.length(), suffix);
					break;
				}
			}
		}
		UpdateHeaderPath(pane);
	}
}

/**
 * @brief Start (or re-run) a merge session in the result pane.
 * @param [in] bAutoMerge Auto-resolve the non-conflicting differences.
 *
 * Used by the Merge menu's Start Merge Session command (no auto-merge)
 * and by the Auto Merge command (with auto-merge), which switches a
 * comparison to the merge result view.
 */
bool CMergeDoc::StartMergeSession(int nBasePane, bool bAutoMerge, bool bWithMessage)
{
	if (m_nBuffers < 2 || m_ptResultBuf == nullptr || m_bResultBuilt)
		return false;

	DIFFOPTIONS options = {};
	Options::DiffOptions::Load(GetOptionsMgr(), options);
	m_mergeSessionDiffOptions = options;

	m_bResultBuilt = false;
	m_nMergeBasePane = m_nBuffers == 2 ? 0 : nBasePane;
	if (CMergeEditFrame* pFrame = GetParentFrame())
		pFrame->ShowMergeResultPane();

	BuildMergeResult();
	CMainFrame::UpdateMergeViewMenu(GetParentFrame());

	for (int nBuffer = 0; nBuffer < m_nBuffers; ++nBuffer)
	{
		m_bResultSavedRO[nBuffer] = m_ptBuf[nBuffer]->GetReadOnly();
		m_strResultSavedDesc[nBuffer] = m_strDesc[nBuffer];
		m_nResultSavedBufferType[nBuffer] = m_nBufferType[nBuffer];
		m_ptBuf[nBuffer]->SetReadOnly(true);
	}

	auto& panes = GetMergePaneMapping(m_nMergeBasePane);
	int nMergeDestPane = m_nBuffers == 2 ? 0 : panes.nMergeDestPane;
	auto* def = GetView(0, nMergeDestPane)->m_CurSourceDef;
	m_pMergeResultView->SetTextType(def);

	// Update pane headers with merge-related labels
	if (m_nBuffers >= 3)
		UpdateMergePaneHeaders(m_nMergeBasePane);

	m_pMergeResultView->TakeFocus();

	if (bAutoMerge)
		ApplyAutoMergeToResult();
	const String msg = _("Merge session started.") + (m_nBuffers == 2 ? _T("") : (_T("\n\n") + GetMergePaneRoles()));
	if (bWithMessage)
		ShowMessageBox(msg.c_str(), MB_OK | MB_ICONINFORMATION | MB_DONT_DISPLAY_AGAIN, IDS_MERGE_SESSION_STARTED);
	else
		RootLogger::Info(msg);
	return true;
}

bool CMergeDoc::EndMergeSession()
{
	if (!m_bResultBuilt)
		return false;

	// Prompt to save if the merge result has unsaved changes
	if (IsMergeResultUnsaved())
	{
		String msg = _("The merge result has not been saved.\n\nSave the merge result?");
		int nResult = ShowMessageBox(msg.c_str(), MB_YESNOCANCEL | MB_ICONQUESTION);
		if (nResult == IDYES && !SaveMergeResult(false))
			return false;
		else if (nResult == IDCANCEL)
			return false;
	}

	// Restore original read-only states
	for (int nBuffer = 0; nBuffer < m_nBuffers; ++nBuffer)
	{
		m_ptBuf[nBuffer]->SetReadOnly(m_bResultSavedRO[nBuffer]);
		m_strDesc[nBuffer] = m_strResultSavedDesc[nBuffer];
		m_nBufferType[nBuffer] = m_nResultSavedBufferType[nBuffer];
		UpdateHeaderPath(nBuffer);
	}

	// Hide the merge result pane
	if (CMergeEditFrame* pFrame = GetParentFrame())
		pFrame->ShowMergeResultPane(false);

	// Reset merge session state
	m_bResultBuilt = false;
	m_ptResultBuf->SetModified(false);
	CMainFrame::UpdateMergeViewMenu(GetParentFrame());

	m_mergeSessionDiffOptions.reset();

	// Clear undo/redo targets for the result buffer, since the buffer is being closed
	size_t i = 0;
	while (i < undoTgt.size())
	{
		if (undoTgt[i] == -1)
		{
			if (i < static_cast<size_t>(curUndo))
				curUndo--;
			undoTgt.erase(undoTgt.begin() + i);
		}
		else
		{
			++i;
		}
	}

	FlushAndRescan(true);

	return true;
}

/**
 * @brief Extract the text of pane nPane for apparent lines
 * [nApparentBegin, nApparentEnd], skipping ghost lines.
 * @return Text; *pnLines receives the number of extracted lines.
 */
String CMergeDoc::GetPaneApparentLinesText(int nPane, int nApparentBegin,
	int nApparentEnd, int* pnLines) const
{
	String text;
	int nLines = 0;
	const CDiffTextBuffer* pBuf = m_ptBuf[nPane].get();
	const int nLineCount = pBuf->GetLineCount();
	for (int nLine = nApparentBegin; nLine <= nApparentEnd && nLine < nLineCount; ++nLine)
	{
		if (pBuf->GetLineFlags(nLine) & LF_GHOST)
			continue;
		const int nLength = pBuf->GetLineLength(nLine);
		if (nLength > 0)
			text.append(pBuf->GetLineChars(nLine), nLength);
		const tchar_t* pszEol = pBuf->GetLineEol(nLine);
		text += (pszEol != nullptr && *pszEol != _T('\0')) ? pszEol : _T("");
		++nLines;
	}
	if (pnLines != nullptr)
		*pnLines = nLines;
	return text;
}

/**
 * @brief (Re)generate the merge result buffer from the compared buffers,
 * auto-resolving all non-conflicting differences.
 *
 * Three-way comparisons use m_nMergeBasePane as their common ancestor.
 * Two-way conflict files instead use the left pane only to anchor common
 * text; every difference is left unresolved for explicit selection.
 */
void CMergeDoc::BuildMergeResult()
{
	if (m_nBuffers < 2 || m_ptResultBuf == nullptr || !m_ptBuf[m_nMergeBasePane]->IsInitialized())
		return;
	m_nSemanticSidesErrors = -1;

	CMergeResultTextBuffer::InternalOpGuard guard(*m_ptResultBuf);

	const bool bViewAttached = m_pMergeResultView != nullptr &&
		m_pMergeResultView->GetSafeHwnd() != nullptr &&
		m_pMergeResultView->LocateTextBuffer() != nullptr &&
		m_ptResultBuf->IsInitialized();
	if (bViewAttached)
		m_pMergeResultView->DetachFromBuffer();
	if (m_ptResultBuf->IsInitialized())
	{
		m_ptResultBuf->FreeAll();
		m_ptResultBuf->m_aUndoBuf.clear();
	}

	// The buffer's nominal style only provides the default EOL for typed
	// and placeholder lines; generated lines keep their source EOLs, and
	// saving preserves them per line unless the user chose a fixed style
	m_ptResultBuf->InitNew(PickResultCRLFStyle());
	PickResultEncoding();
	m_ptResultBuf->SetReadOnly(false);
	m_ptResultBuf->SetTempPath(env::GetTemporaryPath()); // needed by SaveToFile

	m_resultSegments.clear();
	OnResultUndoStackCleared(); // buffer undo history is gone as well
	m_resultDiffToSegment.assign(m_diffList.GetSize(), -1);

	String text;

	// The resume prompt is modal and pumps messages, so a rescan can have
	// replaced the diff list while it was open: size everything below off
	// the list as it is NOW
	const int nDiffCount = m_diffList.GetSize();

	int nCurLine = 0;
	int nApparent = 0;
	const int nApparentCount = m_ptBuf[m_nMergeBasePane]->GetLineCount();
	const int nMergeDestPane = m_nBuffers == 2 ? 1 :
		GetMergePaneMapping(m_nMergeBasePane).nMergeDestPane;

	auto appendCommon = [&](int nBegin, int nEndExcl)
	{
		if (nBegin >= nEndExcl)
			return;
		int nLines = 0;
		text += GetPaneApparentLinesText(m_nMergeBasePane, nBegin, nEndExcl - 1, &nLines);
		if (nLines > 0)
		{
			MergeResultSegment seg;
			seg.diffIdx = -1;
			seg.state = ResultSegmentState::Common;
			seg.srcPanes.push_back(m_nMergeBasePane);
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
		if (m_nBuffers == 2)
		{
			// Two-way conflict files have no common ancestor. Keep the two
			// compared panes intact and leave each differing block for the
			// user to resolve in the result pane.
			seg.state = pdi->op == OP_DIFF ?
				ResultSegmentState::Conflict : ResultSegmentState::Unresolved;
			seg.bWhiteSpaceOnly = seg.state == ResultSegmentState::Conflict &&
				IsResultDiffWhiteSpaceOnly(pdi);
			seg.blockText = GetResultConflictBlockText(nDiff, seg.bWhiteSpaceOnly,
				&seg.nBlockLines);
			text += GetResultSegmentDisplayText(seg, &seg.nLines);
			m_resultDiffToSegment[nDiff] = static_cast<int>(m_resultSegments.size());
			m_resultSegments.push_back(seg);
			nCurLine += seg.nLines;
			nApparent = pdi->dend + 1;
			continue;
		}
		// Without auto-merge every significant difference starts
		// unresolved; with it only true 3-way conflicts do. A difference
		// where the sides agree (or only one side changed) is not a
		// conflict even while it is still unresolved.
		const bool bConflict = (pdi->op == OP_DIFF);
		if (bConflict || pdi->op != OP_TRIVIAL)
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
				int srcPane = m_diffList.GetMergeableSrcIndex(nDiff, nMergeDestPane);
				if (srcPane == -1)
					srcPane = nMergeDestPane;
				seg.blockText = GetPaneApparentLinesText(srcPane, pdi->dbegin,
					pdi->dend, &seg.nBlockLines);
			}
			text += GetResultSegmentDisplayText(seg, &seg.nLines);
		}
		else
		{
			int srcPane = m_diffList.GetMergeableSrcIndex(nDiff, nMergeDestPane);
			if (srcPane == -1)
				srcPane = nMergeDestPane; // No mergeable source: use the merge destination pane.
			int nLines = 0;
			text += GetPaneApparentLinesText(srcPane, pdi->dbegin, pdi->dend, &nLines);
			seg.state = ResultSegmentState::Common;
			seg.srcPanes.push_back(srcPane);
			seg.srcPaneLines.push_back(nLines);
			seg.nLines = nLines;
			if (nLines == 0)
			{
				text += GetResultNoSrcLineText(m_ptResultBuf->GetDefaultEol());
				seg.bNoSrc = true;
				seg.nLines = 1;
			}
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
		for (int i = 0; i < m_ptResultBuf->GetLineCount(); ++i)
			m_ptResultBuf->m_aLines[i].m_dwRevisionNumber = 0;
	}
	m_ptResultBuf->SetModified(false);
	// the generated content is the baseline: no change markers on it
	m_ptResultBuf->AdoptCurrentRevision();
	for (auto& seg : m_resultSegments)
		seg.nBaseRevision = m_ptResultBuf->GetCurrentRevisionNumber();
	m_ptResultBuf->FinishLoading();
	m_bResultBuilt = true;
	// A generated result has not been written to the output path yet: an
	// automatically merged result still has to be saved even if the user
	// edits nothing.
	m_bResultSaved = false;

	if (m_pMergeResultView != nullptr && m_pMergeResultView->GetSafeHwnd() != nullptr)
	{
		m_pMergeResultView->AttachToBuffer(m_ptResultBuf.get());
		m_pMergeResultView->RefreshOptions();
		m_pMergeResultView->Invalidate();
	}
}

void CMergeDoc::ApplyAutoMergeToResult()
{
	if (!m_ptResultBuf || !m_bResultBuilt)
		return;

	// Optionally try safe semantic copy on the conflicts first: the line
	// merge resolves the one-sided fragments of a conflicting definition
	// independently, which can leave it semantically mangled
	ResetSemanticResolutionCounts();
	const int nSemantic = IsSemanticMergeEnabled() ? DoSemanticMergeConflictsToResult() : 0;

	CMergeResultTextBuffer::InternalOpGuard guard(*m_ptResultBuf);

	m_pMergeResultView->HideCursor();

	const int nMergeDestPane = m_nBuffers == 2 ? 1 :
		GetMergePaneMapping(m_nMergeBasePane).nMergeDestPane;

	// one undo step for the whole Auto Merge, semantic resolutions included
	m_ptResultBuf->BeginUndoGroup(nSemantic > 0);

	for (int i = static_cast<int>(m_resultSegments.size()) - 1; i >= 0; --i)
	{
		MergeResultSegment& seg = m_resultSegments[i];
		if (seg.state != ResultSegmentState::Unresolved)
			continue;

		const DIFFRANGE* pdi = m_diffList.DiffRangeAt(seg.diffIdx);
		if (!pdi || pdi->op == OP_DIFF)
			continue;

		int srcPane = m_diffList.GetMergeableSrcIndex(seg.diffIdx, nMergeDestPane);
		if (srcPane == -1)
			srcPane = nMergeDestPane;

		int nLines = 0;
		String resolvedText = GetPaneApparentLinesText(srcPane, pdi->dbegin, pdi->dend, &nLines);
		const bool bNoSrc = (nLines == 0);
		const int nNewLines = bNoSrc ? 1 : nLines;
		if (bNoSrc)
			resolvedText = GetResultNoSrcLineText(m_ptResultBuf->GetDefaultEol());

		const int nStartLine = seg.nStartLine;
		const int nEndLine = nStartLine + seg.nLines;   // exclusive

		int nEndLineAfter = 0, nEndCharAfter = 0;
		m_ptResultBuf->DeleteText(m_pMergeResultView, nStartLine, 0, nEndLine, 0, CE_ACTION_MERGE);
		m_ptResultBuf->InsertText(m_pMergeResultView, nStartLine, 0, resolvedText.c_str(), resolvedText.length(), nEndLineAfter, nEndCharAfter, CE_ACTION_MERGE, true /* history */);

		seg.state = ResultSegmentState::Auto;
		seg.srcPanes.clear();
		seg.srcPaneLines.clear();
		seg.srcPanes.push_back(srcPane);
		seg.srcPaneLines.push_back(nLines);
		seg.nLines = nNewLines;
		seg.bNoSrc = bNoSrc;
		seg.blockText.clear();
		seg.nBlockLines = 0;

		const int nDelta = nNewLines - (nEndLine - nStartLine);
		for (size_t j = i + 1; j < m_resultSegments.size(); ++j)
			m_resultSegments[j].nStartLine += nDelta;
	}

	m_ptResultBuf->FlushUndoGroup(m_pMergeResultView);

	m_pMergeResultView->ShowCursor();
	m_pMergeResultView->Invalidate();

	const String report = FormatSemanticResolutionReport();
	if (!report.empty())
	{
		RootLogger::Info(report);
		if (!theApp.GetNonInteractive())
			ShowMessageBox(report, MB_ICONINFORMATION);
	}
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
 * For three-way merges all versions are preserved; two-way merges use the
 * ordinary two-sided conflict format. Saved unresolved sections can be
 * finished later in an editor or recognized by other merge tools.
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
	int nLines = m_nBuffers == 2 ? 3 : 4;
	int nPaneLines = 0;
	if (m_nBuffers == 2)
	{
		String text = _T("<<<<<<< ") + label(1);
		if (bWhiteSpaceOnly)
			text += _T(" (whitespace only)");
		text += pszEol;
		text += GetPaneApparentLinesText(1, pdi->dbegin, pdi->dend, &nPaneLines);
		if (text.back() != _T('\n') && text.back() != _T('\r'))
			text += pszEol;
		nLines += nPaneLines;
		text += _T("=======") + String(pszEol);
		text += GetPaneApparentLinesText(0, pdi->dbegin, pdi->dend, &nPaneLines);
		if (text.back() != _T('\n') && text.back() != _T('\r'))
			text += pszEol;
		nLines += nPaneLines;
		text += _T(">>>>>>> ") + label(0) + pszEol;
		if (pnLines != nullptr)
			*pnLines = nLines;
		return text;
	}
	const auto& panes = GetMergePaneMapping(m_nMergeBasePane);
	int nBasePane = panes.nBasePane, nTheirsPane = panes.nTheirsPane, nMinePane = panes.nMinePane;
	String text = _T("<<<<<<< ") + label(nMinePane);
	if (bWhiteSpaceOnly)
		text += _T(" (whitespace only)");
	text += pszEol;
	text += GetPaneApparentLinesText(nMinePane, pdi->dbegin, pdi->dend, &nPaneLines);
	if (text.back() != _T('\n') && text.back() != _T('\r'))
		text += pszEol;
	nLines += nPaneLines;
	text += _T("||||||| ") + label(nBasePane) + pszEol;
	text += GetPaneApparentLinesText(nBasePane, pdi->dbegin, pdi->dend, &nPaneLines);
	if (text.back() != _T('\n') && text.back() != _T('\r'))
		text += pszEol;
	nLines += nPaneLines;
	text += _T("=======");
	text += pszEol;
	text += GetPaneApparentLinesText(nTheirsPane, pdi->dbegin, pdi->dend, &nPaneLines);
	if (text.back() != _T('\n') && text.back() != _T('\r'))
		text += pszEol;
	nLines += nPaneLines;
	text += _T(">>>>>>> ") + label(nTheirsPane) + pszEol;
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
	if (seg.diffIdx < 0)
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
 * @brief Counts of pending differences, conflicts, and whitespace-only
 * conflicts.
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
	// edit-protected. A segment without a linked difference
	// must stay editable: hand-editing is its only resolution path.
	if (pSegment != nullptr && pSegment->bNoSrc)
		return true; // "<No Lines>": choose a source to change it
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
	// segments without a linked difference
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
	case ResultSegmentState::Semantic:
		return _T('s');
	default:
		return 0;
	}
}

String SemanticRuleName(SemanticRule rule)
{
	switch (rule)
	{
	case SemanticRule::DuplicateAddition: return _("Duplicate addition");
	case SemanticRule::TokenMerge: return _("Token merge");
	case SemanticRule::MovedDefinition: return _("Moved definition");
	case SemanticRule::DefinitionChange: return _("One-sided definition change");
	case SemanticRule::LocalizedChange: return _("Small change onto refactored definition");
	case SemanticRule::CommentedOut: return _("Commented-out definition update");
	default: return String();
	}
}

namespace
{

String ResultPaneLabel(int nPane)
{
	switch (nPane)
	{
	case 0: return _("Left");
	case 1: return _("Middle");
	case 2: return _("Right");
	default: return String();
	}
}

}

int CMergeDoc::GetResultSemanticCount() const
{
	std::unordered_set<int> ids;
	for (const auto& seg : m_resultSegments)
	{
		if (seg.state == ResultSegmentState::Semantic)
			ids.insert(seg.semanticId);
	}
	return static_cast<int>(ids.size());
}

int CMergeDoc::FindResultSemanticDiff(int nFromLine, bool bForward) const
{
	// whole resolutions: skip the one at the line, and land on the first
	// segment of the one found
	const MergeResultSegment* pAt = GetResultSegmentByLine(nFromLine);
	const int nCurrentId = (pAt != nullptr && pAt->state == ResultSegmentState::Semantic) ? pAt->semanticId : 0;
	int nFoundId = 0;
	for (const auto& seg : m_resultSegments)
	{
		if (seg.state != ResultSegmentState::Semantic || seg.diffIdx < 0 || seg.semanticId == nCurrentId)
			continue;
		if (bForward ? seg.nStartLine > nFromLine : seg.nStartLine + (std::max)(seg.nLines, 1) - 1 < nFromLine)
		{
			nFoundId = seg.semanticId;
			if (bForward)
				break;
		}
	}
	if (nFoundId == 0)
		return -1;
	for (const auto& seg : m_resultSegments)
	{
		if (seg.state == ResultSegmentState::Semantic && seg.diffIdx >= 0 && seg.semanticId == nFoundId)
			return seg.diffIdx;
	}
	return -1;
}

String CMergeDoc::FormatSemanticResolutionReport() const
{
	const int nTotal = m_semanticCounts.Total();
	if (nTotal == 0)
		return String();
	String report = strutils::format_string1(_("Resolved semantically: %1"), strutils::to_str(nTotal));
	for (int nRule = 0; nRule < static_cast<int>(std::size(m_semanticCounts.counts)); ++nRule)
	{
		if (m_semanticCounts.counts[nRule] > 0)
			report += _T("\n  ") + SemanticRuleName(static_cast<SemanticRule>(nRule)) + _T(": ") +
				strutils::to_str(m_semanticCounts.counts[nRule]);
	}
	return report;
}

/**
 * @brief Record the linked segments before a semantic rule changes the result.
 */
SemanticResolutionSnapshot CMergeDoc::BeginSemanticResolution() const
{
	SemanticResolutionSnapshot snapshot;
	snapshot.nRevision = m_ptResultBuf != nullptr ? m_ptResultBuf->GetCurrentRevisionNumber() : 0;
	snapshot.entries.resize(m_resultDiffToSegment.size());
	for (const auto& seg : m_resultSegments)
	{
		if (seg.diffIdx < 0 || seg.diffIdx >= static_cast<int>(snapshot.entries.size()))
			continue;
		auto& entry = snapshot.entries[seg.diffIdx];
		entry.diffIdx = seg.diffIdx;
		entry.state = seg.state;
		entry.srcPanes = seg.srcPanes;
		entry.nLines = seg.nLines;
	}
	return snapshot;
}

/**
 * @brief Mark the linked segments a semantic rule changed since the
 * snapshot as resolved by that rule, count the resolution and log it.
 * @return Number of segments marked.
 */
int CMergeDoc::EndSemanticResolution(const SemanticResolutionSnapshot& before, SemanticRule rule, const String& reason)
{
	if (m_ptResultBuf == nullptr)
		return 0;
	const int nLineCount = m_ptResultBuf->GetLineCount();
	auto isChanged = [&](const MergeResultSegment& seg)
	{
		// a segment the snapshot does not know is not claimed for the rule
		if (seg.diffIdx >= static_cast<int>(before.entries.size()) || before.entries[seg.diffIdx].diffIdx != seg.diffIdx)
			return false;
		const auto& entry = before.entries[seg.diffIdx];
		if (entry.state != seg.state || entry.srcPanes != seg.srcPanes || entry.nLines != seg.nLines)
			return true;
		for (int nLine = seg.nStartLine; nLine < seg.nStartLine + seg.nLines && nLine < nLineCount; ++nLine)
		{
			if (m_ptResultBuf->GetLineRevisionNumber(nLine) > before.nRevision)
				return true;
		}
		return false;
	};

	const uint32_t nRevision = m_ptResultBuf->GetCurrentRevisionNumber();
	const int nId = m_nLastSemanticId + 1;
	int nMarked = 0;
	int nFirstLine = -1;
	for (auto& seg : m_resultSegments)
	{
		if (seg.diffIdx < 0 || !isChanged(seg))
			continue;
		seg.state = ResultSegmentState::Semantic;
		seg.semanticRule = rule;
		seg.semanticReason = reason;
		seg.semanticId = nId;
		// later hand edits are newer than this
		seg.nBaseRevision = nRevision;
		if (nFirstLine < 0)
			nFirstLine = seg.nStartLine;
		++nMarked;
	}
	if (nMarked == 0)
		return 0;

	m_nLastSemanticId = nId;
	++m_semanticCounts.counts[static_cast<int>(rule)];
	RootLogger::Info(strutils::format_string2(_("Resolved semantically at result line %1: %2"),
		strutils::to_str(nFirstLine + 1), reason));
	if (m_pMergeResultView != nullptr && m_pMergeResultView->GetSafeHwnd() != nullptr)
		m_pMergeResultView->Invalidate();
	return nMarked;
}

/**
 * @brief Do the compared versions of this difference only differ in white space?
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
	if (m_nBuffers == 2)
		return strippedText(0) == strippedText(1);
	const MergePanes panes = GetMergePaneMapping(m_nMergeBasePane);
	const String sBase = strippedText(m_nMergeBasePane);
	return strippedText(panes.nTheirsPane) == sBase && sBase == strippedText(panes.nMinePane);
}

/**
 * @brief Line-ending style for the result. For three-way merges, prefer
 * the style of the side that changed it (base = m_nMergeBasePane).
 */
CRLFSTYLE CMergeDoc::PickResultCRLFStyle() const
{
	if (m_nBuffers == 2)
	{
		CRLFSTYLE style = m_ptBuf[1]->GetCRLFMode();
		if (style == CRLFSTYLE::AUTOMATIC || style == CRLFSTYLE::MIXED)
			style = m_ptBuf[0]->GetCRLFMode();
		if (style == CRLFSTYLE::AUTOMATIC || style == CRLFSTYLE::MIXED)
			style = CRLFSTYLE::DOS;
		return style;
	}
	auto panes = GetMergePaneMapping(m_nMergeBasePane);
	int nBasePane = panes.nBasePane, nTheirsPane = panes.nTheirsPane, nMinePane = panes.nMinePane;
	const CRLFSTYLE sBase = m_ptBuf[nBasePane]->GetCRLFMode();
	const CRLFSTYLE sTheirs = m_ptBuf[nTheirsPane]->GetCRLFMode();
	const CRLFSTYLE sMine = m_ptBuf[nMinePane]->GetCRLFMode();
	CRLFSTYLE crlfStyle;
	if (sBase == sTheirs)
		crlfStyle = sMine;   // mine changed the style (or nobody did)
	else if (sBase == sMine)
		crlfStyle = sTheirs; // theirs changed the style
	else if (sTheirs == sMine)
		crlfStyle = sTheirs; // both sides agree against the base
	else
		crlfStyle = sBase;   // undecidable: keep the base style
	if (crlfStyle == CRLFSTYLE::AUTOMATIC || crlfStyle == CRLFSTYLE::MIXED)
		crlfStyle = sBase;
	if (crlfStyle == CRLFSTYLE::AUTOMATIC || crlfStyle == CRLFSTYLE::MIXED)
		crlfStyle = CRLFSTYLE::DOS;
	return crlfStyle;
}

/**
 * @brief Encoding for the result. For three-way merges, prefer the
 * encoding of the side that changed it (base = m_nMergeBasePane).
 */
void CMergeDoc::PickResultEncoding()
{
	if (m_nBuffers == 2)
	{
		m_ptResultBuf->setEncoding(m_ptBuf[1]->getEncoding());
		return;
	}
	auto panes = GetMergePaneMapping(m_nMergeBasePane);
	int nBasePane = panes.nBasePane, nTheirsPane = panes.nTheirsPane, nMinePane = panes.nMinePane;
	const FileTextEncoding& eBase = m_ptBuf[nBasePane]->getEncoding();
	const FileTextEncoding& eTheirs = m_ptBuf[nTheirsPane]->getEncoding();
	const FileTextEncoding& eMine = m_ptBuf[nMinePane]->getEncoding();
	auto sameEncoding = [](const FileTextEncoding& a, const FileTextEncoding& b)
		{
			return a.m_unicoding == b.m_unicoding && a.m_codepage == b.m_codepage;
		};
	if (sameEncoding(eBase, eTheirs) && !sameEncoding(eTheirs, eMine))
		m_ptResultBuf->setEncoding(eMine);   // mine changed the encoding
	else if (sameEncoding(eTheirs, eMine) && !sameEncoding(eBase, eTheirs))
		m_ptResultBuf->setEncoding(eTheirs); // theirs changed the encoding
	else
		m_ptResultBuf->setEncoding(eBase);   // agreement or undecidable: keep base
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
		if (bPlaceholder && seg.diffIdx >= 0)
			text += seg.blockText; // buffer shows the compact placeholder
		else if (!seg.bNoSrc) // "<No Lines>" is display only
			text += GetResultBufferLinesText(seg.nStartLine, seg.nLines);
		nCovered = (std::max)(nCovered, seg.nStartLine + seg.nLines);
	}
	text += GetResultBufferLinesText(nCovered,
		m_ptResultBuf->GetLineCount() - nCovered);
	return text;
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
	if (!m_bResultBuilt)
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

	const int nMergeDestPane = m_nBuffers == 2 ? 1 :
		GetMergePaneMapping(m_nMergeBasePane).nMergeDestPane;
	int nNewLines = 0;
	String text;
	bool bBackToUnresolved = false;
	std::vector<int> srcPaneLines;
	srcPaneLines.reserve(srcPanes.size());
	for (int srcPane : srcPanes)
	{
		int nPaneLines = 0;
		text += GetPaneApparentLinesText(srcPane, pdi->dbegin, pdi->dend, &nPaneLines);
		srcPaneLines.push_back(nPaneLines);
		nNewLines += nPaneLines;
	}
	// The selected source(s) have no lines for this difference: show the
	// display-only "<No Lines>" line instead of an empty segment
	const bool bNoSrc = !srcPanes.empty() && nNewLines == 0;
	if (bNoSrc)
	{
		text = GetResultNoSrcLineText(m_ptResultBuf->GetDefaultEol());
		nNewLines = 1;
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
			int srcPane = m_diffList.GetMergeableSrcIndex(nDiff, nMergeDestPane);
			if (srcPane == -1)
				srcPane = nMergeDestPane;
			seg.blockText = GetPaneApparentLinesText(srcPane, pdi->dbegin,
				pdi->dend, &seg.nBlockLines);
		}
		// state is updated below; only the state matters for the display
		MergeResultSegment tmpSeg = seg;
		tmpSeg.state = (pdi->op == OP_DIFF) ?
			ResultSegmentState::Conflict : ResultSegmentState::Unresolved;
		text = GetResultSegmentDisplayText(tmpSeg, &nNewLines);
		bBackToUnresolved = true;
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
	seg.state = !bBackToUnresolved ? ResultSegmentState::Chosen :
		(pdi->op == OP_DIFF ? ResultSegmentState::Conflict : ResultSegmentState::Unresolved);
	seg.srcPanes = srcPanes;
	seg.srcPaneLines = std::move(srcPaneLines);
	seg.nLines = nNewLines;
	seg.bNoSrc = bNoSrc;
	if (!bBackToUnresolved)
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
}

/**
 * @brief Toggle a source pane for the given difference (KDiff3 style).
 *
 * Pressing 1/2/3 adds that pane's block to the difference's selection,
 * or removes it when already selected. Blocks are concatenated in the
 * order they were selected. Removing the last selection restores the
 * unresolved conflict placeholder.
 */
void CMergeDoc::ResultToggleSource(int nDiff, int srcPane, bool bGroupWithPrevious /*= false*/)
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
	ResultChooseSources(nDiff, srcPanes, bGroupWithPrevious);
}

/**
 * @brief Resolve every difference that has nothing chosen for it yet by
 * taking the given pane. Covers both real conflicts and differences that
 * are simply still unresolved (the usual case without auto-merge).
 */
void CMergeDoc::ResultChooseAllConflicts(int srcPane)
{
	if (!m_bResultBuilt)
		return;

	m_pMergeResultView->HideCursor();

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

	m_pMergeResultView->ShowCursor();
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
	// (an unattended run cannot be asked; its unresolved conflicts are
	// written as conflict sections)
	if (nUnresolved > 0 && !theApp.GetNonInteractive())
	{
		const String msg = strutils::format_string1(
			_("There are still %1 unresolved difference(s) in the merge result.\n\nUnresolved conflicts are saved as conflict sections (<<<<<<<), which preserve all versions and can be resolved later in any editor; other unresolved differences are saved with their automatic merge content.\n\nSave the result anyway?"),
			strutils::format(_T("%d"), nUnresolved));
		if (ShowMessageBox(msg, MB_YESNO | MB_ICONWARNING) != IDYES)
			return false;
	}

	String strPath = m_strMergeResultSavePath;
	if (bSaveAs || strPath.empty())
	{
		String sDefault = m_strMergeResultSavePath;
		if (sDefault.empty())
		{
			const int nDefaultPane = m_nBuffers == 2 ? 1 :
				GetMergePaneMapping(m_nMergeBasePane).nMinePane;
			sDefault = m_filePaths.GetPath(nDefaultPane);
		}
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
		return buf.SaveToFile(strPath, false, sError, infoTempUnpacker);
	};

	// Compact placeholders are display-only: the file gets the full
	// conflict sections, written through a temporary buffer so the
	// visible one is not disturbed
	bool bNeedExpansion = false;
	for (const auto& seg : m_resultSegments)
	{
		if (seg.bNoSrc ||
			((seg.state == ResultSegmentState::Conflict ||
				seg.state == ResultSegmentState::Unresolved) &&
				seg.diffIdx >= 0))
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
	m_ptResultBuf->AdoptCurrentRevision();
	m_ptResultBuf->m_nSyncPosition = m_ptResultBuf->m_nUndoPosition;
	m_ptResultBuf->SetModified(false);
	m_pMergeResultView->Invalidate(false);
	m_strMergeResultSavePath = strPath;
	m_bResultSaved = true;
	return true;
}

/**
 * @brief Current difference changed: scroll the result pane to it.
 */
void CMergeDoc::OnResultPaneCurrentDiffChanged(int nDiff)
{
	if (!m_bResultBuilt || nDiff < 0)
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

	undoTgt.erase(undoTgt.begin() + curUndo, undoTgt.end());
	undoTgt.push_back(-1);
	curUndo = undoTgt.size();
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

bool CMergeDoc::AdjustResultSegmentsAfterLineDeletion(
	int nRemovedBegin, int nCount)
{
	if (nCount <= 0)
		return false;

	const int nRemovedEnd = nRemovedBegin + nCount - 1;
	bool bStateChanged = false;
	std::vector<int> emptySegments;

	// Adjust segment positions and sizes after the deletion.
	for (int i = 0; i < static_cast<int>(m_resultSegments.size()); ++i)
	{
		auto& seg = m_resultSegments[i];

		if (seg.nLines > 0)
		{
			const int nSegEnd = seg.nStartLine + seg.nLines - 1;

			if (nSegEnd < nRemovedBegin)
				continue;

			if (seg.nStartLine > nRemovedEnd)
			{
				seg.nStartLine -= nCount;
				continue;
			}

			const int nOverlapBegin =
				(std::max)(seg.nStartLine, nRemovedBegin);
			const int nOverlapEnd =
				(std::min)(nSegEnd, nRemovedEnd);
			const int nOverlap = nOverlapEnd - nOverlapBegin + 1;

			if (nOverlap > 0)
			{
				seg.nLines -= nOverlap;

				if (IsTrackedResultSegment(seg) &&
					seg.state != ResultSegmentState::Edited)
				{
					seg.state = ResultSegmentState::Edited;
					bStateChanged = true;
				}

				if (seg.nLines == 0 &&
					seg.diffIdx >= 0 &&
					seg.state != ResultSegmentState::Conflict &&
					seg.state != ResultSegmentState::Unresolved &&
					!seg.bNoSrc)
				{
					emptySegments.push_back(i);
				}
			}

			if (seg.nStartLine >= nRemovedBegin)
				seg.nStartLine = nRemovedBegin;
		}
		else
		{
			if (seg.nStartLine >= nRemovedBegin)
				seg.nStartLine = nRemovedBegin;
		}
	}

	if (emptySegments.empty())
		return bStateChanged;

	CMergeResultTextBuffer::InternalOpGuard guard(*m_ptResultBuf);

	CCrystalTextView* pView =
		(m_pMergeResultView != nullptr &&
			m_pMergeResultView->GetSafeHwnd() != nullptr)
		? m_pMergeResultView : nullptr;

	const String text =
		GetResultNoSrcLineText(m_ptResultBuf->GetDefaultEol());

	// Insert GHOST lines from top to bottom.
	//
	// Each empty result segment gets its own GHOST line.  Inserting
	// from top to bottom is important when multiple segments became
	// empty at the same deletion position:
	//
	//   m|<No Lines>       <- segment A
	//   m|<No Lines>       <- segment B
	//    |0123456789
	//
	// Inserting from bottom to top would reverse the relationship
	// between the physical lines and the result segments.
	for (const int i : emptySegments)
	{
		auto& seg = m_resultSegments[i];
		const int nInsertLine = seg.nStartLine;

		int nEndLine = 0;
		int nEndChar = 0;

		m_ptResultBuf->InsertText(
			pView,
			nInsertLine,
			0,
			text.c_str(),
			text.length(),
			nEndLine,
			nEndChar,
			CE_ACTION_UNKNOWN,
			true);

		seg.nStartLine = nInsertLine;
		seg.nLines = 1;
		seg.bNoSrc = true;

		// The inserted line shifts all subsequent result segments.
		for (size_t j = static_cast<size_t>(i) + 1;
			j < m_resultSegments.size();
			++j)
		{
			++m_resultSegments[j].nStartLine;
		}
	}

	return bStateChanged;
}

/**
 * @brief Lines [nFirstLine, nFirstLine + nCount) were removed whole by a
 * user edit (column-0-to-column-0 deletion); the following line survived
 * unchanged, so no line merge happened.
 */
void CMergeDoc::OnResultBufferDeletedWholeLines(int nFirstLine, int nCount)
{
	const bool bStateChanged = AdjustResultSegmentsAfterLineDeletion(
		nFirstLine, nCount);

	if (bStateChanged)
	{
		if (m_pMergeResultView != nullptr && m_pMergeResultView->GetSafeHwnd() != nullptr)
			m_pMergeResultView->Invalidate();
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
	AdjustResultSegmentsAfterLineDeletion(nRemovedBegin, nCount);

	// The segment containing the merge point was edited as well
	OnResultLineEdited(nStartLine);
}

/////////////////////////////////////////////////////////////////////////////
// Command handlers

void CMergeDoc::OnMergeChooseSource(UINT nID)
{
	const int srcPane = nID - ID_MERGE_RESULT_CHOOSE_1ST;
	if (srcPane < 0 || srcPane >= m_nBuffers)
		return;

	int firstDiff = -1, lastDiff = -1;
	if (!m_pMergeResultView->IsSelection() && GetCurrentDiff() != -1)
	{
		firstDiff = lastDiff = GetCurrentDiff();
	}
	else
	{
		m_pMergeResultView->GetSelectedDiffs(firstDiff, lastDiff);
		if (firstDiff < 0 || lastDiff < 0)
			return;
	}

	m_pMergeResultView->HideCursor();

	bool bGroupWithPrevious = false;
	for (int nDiff = firstDiff; nDiff <= lastDiff; ++nDiff)
	{
		if (!m_diffList.IsDiffSignificant(nDiff))
			continue;
		ResultToggleSource(nDiff, srcPane, bGroupWithPrevious);
		bGroupWithPrevious = true;
	}

	m_pMergeResultView->ShowCursor();
}

void CMergeDoc::OnUpdateMergeChooseSource(CCmdUI* pCmdUI)
{
	const bool bValidSource = m_nBuffers != 2 ||
		pCmdUI->m_nID == ID_MERGE_RESULT_CHOOSE_1ST ||
		pCmdUI->m_nID == ID_MERGE_RESULT_CHOOSE_2ND;
	if (!m_bResultBuilt || !bValidSource)
	{
		pCmdUI->Enable(FALSE);
		pCmdUI->SetCheck(FALSE);
		return;
	}

	int firstDiff = -1, lastDiff = -1;
	if (!m_pMergeResultView->IsSelection() && GetCurrentDiff() != -1)
	{
		firstDiff = lastDiff = GetCurrentDiff();
	}
	else
	{
		m_pMergeResultView->GetSelectedDiffs(firstDiff, lastDiff);

		if (firstDiff == -1 || lastDiff == -1)
		{
			pCmdUI->Enable(FALSE);
			pCmdUI->SetCheck(FALSE);
			return;
		}
	}

	pCmdUI->Enable(TRUE);

	// For single diff, show which panes are currently selected
	if (firstDiff == lastDiff)
	{
		const int nDiff = firstDiff;
		const MergeResultSegment* pSegment = GetResultSegmentByDiff(nDiff);
		const int srcPane = pCmdUI->m_nID - ID_MERGE_RESULT_CHOOSE_1ST;
		if (srcPane < 0 || srcPane >= m_nBuffers || !m_diffList.IsDiffSignificant(nDiff))
		{
			pCmdUI->Enable(FALSE);
			pCmdUI->SetCheck(FALSE);
			return;
		}
		const bool bChecked = pSegment != nullptr &&
			(pSegment->state == ResultSegmentState::Auto ||
			 pSegment->state == ResultSegmentState::Chosen) &&
			std::find(pSegment->srcPanes.begin(), pSegment->srcPanes.end(), srcPane) != pSegment->srcPanes.end();
		pCmdUI->SetCheck(bChecked);
	}
	else
	{
		// For multiple selection, don't show a specific checkbox state
		pCmdUI->SetCheck(FALSE);
	}
}

void CMergeDoc::OnMergeChooseAllConflicts(UINT nID)
{
	const int srcPane = nID - ID_MERGE_RESULT_CHOOSE_ALL_1ST;
	if (srcPane >= 0 && srcPane < m_nBuffers)
		ResultChooseAllConflicts(srcPane);
}

void CMergeDoc::OnUpdateMergeChooseAllConflicts(CCmdUI* pCmdUI)
{
	const bool bValidSource = m_nBuffers != 2 ||
		pCmdUI->m_nID == ID_MERGE_RESULT_CHOOSE_ALL_1ST ||
		pCmdUI->m_nID == ID_MERGE_RESULT_CHOOSE_ALL_2ND;
	pCmdUI->Enable(bValidSource && m_bResultBuilt && GetResultUnresolvedCount() > 0);
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
		m_bResultBuilt);
}

String CMergeDoc::GetMergePaneRoles() const
{
	if (m_nBuffers == 2)
		return _T("");
	std::array<String, 3> str = GetMergePaneMappingString(m_nMergeBasePane);
	return strutils::format_string3(_("Pane roles: Left=%1, Middle=%2, Right=%3"), str[0], str[1], str[2]);
}

void CMergeDoc::OnMergeStartSession()
{
	FlushAndRescan(true);

	if (auto* pView = GetActiveMergeView())
		StartMergeSession(2 - pView->m_nThisPane, false, true);
	else
		StartMergeSession(1, false, true);
}

void CMergeDoc::OnUpdateMergeStartSession(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetOptionsMgr()->GetBool(OPT_MERGE_RESULT_PANE_ENABLED) && !m_bResultBuilt);
}

/**
 * @brief End the current merge session by closing the result pane.
 * Prompts to save if the merge result has unsaved changes.
 * Restores the original read-only states and pane descriptions.
 */
void CMergeDoc::OnMergeEndSession()
{
	EndMergeSession();
}

/**
 * @brief Update UI for End Merge Session command.
 * Enable only when a merge session is active.
 */
void CMergeDoc::OnUpdateMergeEndSession(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bResultBuilt);
}

/////////////////////////////////////////////////////////////////////////////
// Safe semantic copy into the merge result

namespace
{

bool IsBlankLineText(const String& line)
{
	return line.find_first_not_of(_T(" \t\r\n")) == String::npos;
}

void SplitTextIntoLines(const String& text, std::vector<String>& lines)
{
	size_t pos = 0;
	while (pos <= text.length())
	{
		size_t end = text.find_first_of(_T("\r\n"), pos);
		if (end == String::npos)
		{
			lines.push_back(text.substr(pos));
			break;
		}
		lines.push_back(text.substr(pos, end - pos));
		pos = end + ((text[end] == _T('\r') && end + 1 < text.length() && text[end + 1] == _T('\n')) ? 2 : 1);
	}
}

void AppendNonBlankLines(const std::vector<String>& lines, std::vector<String>& nonBlank)
{
	for (const String& line : lines)
	{
		if (!IsBlankLineText(line))
			nonBlank.push_back(line);
	}
}

/** @brief One stretch of the region a suggestion rewrites: common text or a difference. */
struct SemanticRegionItem
{
	int nDiff = -1; /**< difference index, or -1 for common text */
	std::vector<String> commonLines; /**< non-blank destination lines of common text */
	std::vector<std::vector<int>> candidates; /**< pane selections to try for a difference */
	std::vector<std::vector<String>> candidateLines; /**< their non-blank lines */
	int nChosen = -1; /**< index of the matching candidate */
};

/**
 * @brief Find a pane selection per difference whose text reproduces the
 * wanted lines (blank lines aside). Differences are few, so plain
 * backtracking is enough.
 */
bool MatchSemanticRegion(std::vector<SemanticRegionItem>& items, size_t nItem,
	const std::vector<String>& wanted, size_t nPos, int& nBudget)
{
	if (nItem == items.size())
		return nPos == wanted.size();
	if (--nBudget < 0)
		return false;
	auto startsWith = [&](const std::vector<String>& lines)
	{
		if (lines.size() > wanted.size() - nPos)
			return false;
		return std::equal(lines.begin(), lines.end(), wanted.begin() + nPos);
	};
	SemanticRegionItem& item = items[nItem];
	if (item.nDiff < 0)
		return startsWith(item.commonLines) &&
			MatchSemanticRegion(items, nItem + 1, wanted, nPos + item.commonLines.size(), nBudget);
	for (size_t i = 0; i < item.candidates.size(); ++i)
	{
		if (!startsWith(item.candidateLines[i]))
			continue;
		item.nChosen = static_cast<int>(i);
		if (MatchSemanticRegion(items, nItem + 1, wanted, nPos + item.candidateLines[i].size(), nBudget))
			return true;
	}
	item.nChosen = -1;
	return false;
}

}

/**
 * @brief The pane the merge result stands for.
 */
int CMergeDoc::GetMergeDestPane() const
{
	return m_nBuffers == 2 ? 1 : GetMergePaneMapping(m_nMergeBasePane).nMergeDestPane;
}

/**
 * @brief Text of the non-ghost lines of a pane in an apparent line range.
 * @param [out] pApparent Apparent line number of each returned line.
 */
void CMergeDoc::GetPaneRealLines(int nPane, int nApparentBegin, int nApparentEnd,
	std::vector<String>& lines, std::vector<int>* pApparent) const
{
	const CDiffTextBuffer* pBuf = m_ptBuf[nPane].get();
	const int nLineCount = pBuf->GetLineCount();
	for (int nLine = (std::max)(nApparentBegin, 0); nLine <= nApparentEnd && nLine < nLineCount; ++nLine)
	{
		if (pBuf->GetLineFlags(nLine) & LF_GHOST)
			continue;
		const int nLength = pBuf->GetLineLength(nLine);
		lines.push_back(nLength > 0 ? String(pBuf->GetLineChars(nLine), nLength) : String());
		if (pApparent != nullptr)
			pApparent->push_back(nLine);
	}
}

/**
 * @brief Result buffer line that holds a line of a compared pane.
 *
 * Succeeds for common text and for differences whose result segment
 * currently consists of exactly that pane's text.
 */
bool CMergeDoc::MapPaneLineToResult(int nPane, int nApparentLine, int& nResultLine) const
{
	const CDiffTextBuffer* pBuf = m_ptBuf[nPane].get();
	if (nApparentLine < 0 || nApparentLine >= pBuf->GetLineCount() ||
		(pBuf->GetLineFlags(nApparentLine) & LF_GHOST))
		return false;
	const int nDiffCount = m_diffList.GetSize();
	if (static_cast<int>(m_resultDiffToSegment.size()) != nDiffCount)
		return false;

	auto countRealLines = [&](int nCountPane, int nBegin, int nEndExcl)
	{
		const CDiffTextBuffer* pCountBuf = m_ptBuf[nCountPane].get();
		int nCount = 0;
		for (int nLine = nBegin; nLine < nEndExcl && nLine < pCountBuf->GetLineCount(); ++nLine)
		{
			if (!(pCountBuf->GetLineFlags(nLine) & LF_GHOST))
				++nCount;
		}
		return nCount;
	};

	int nPrevDiff = -1;
	for (int nDiff = 0; nDiff < nDiffCount; ++nDiff)
	{
		const DIFFRANGE* pdi = m_diffList.DiffRangeAt(nDiff);
		if (nApparentLine > pdi->dend)
		{
			nPrevDiff = nDiff;
			continue;
		}
		if (nApparentLine < pdi->dbegin)
			break;
		const int nSegment = m_resultDiffToSegment[nDiff];
		if (nSegment < 0)
			return false;
		const MergeResultSegment& seg = m_resultSegments[nSegment];
		const bool bPaneText = !seg.bNoSrc && seg.srcPanes.size() == 1 && seg.srcPanes[0] == nPane &&
			(seg.state == ResultSegmentState::Common || seg.state == ResultSegmentState::Auto ||
			 seg.state == ResultSegmentState::Chosen);
		const int nOffset = countRealLines(nPane, pdi->dbegin, nApparentLine);
		if (!bPaneText || nOffset >= seg.nLines)
			return false;
		nResultLine = seg.nStartLine + nOffset;
		return true;
	}

	// common text, generated from the base pane
	int nApparentStart = 0;
	int nSegment = 0;
	if (nPrevDiff >= 0)
	{
		nApparentStart = m_diffList.DiffRangeAt(nPrevDiff)->dend + 1;
		if (m_resultDiffToSegment[nPrevDiff] < 0)
			return false;
		nSegment = m_resultDiffToSegment[nPrevDiff] + 1;
	}
	if (nSegment >= static_cast<int>(m_resultSegments.size()))
		return false;
	const MergeResultSegment& seg = m_resultSegments[nSegment];
	const int nOffset = countRealLines(m_nMergeBasePane, nApparentStart, nApparentLine);
	if (seg.diffIdx != -1 || seg.state != ResultSegmentState::Common || nOffset >= seg.nLines)
		return false;
	nResultLine = seg.nStartLine + nOffset;
	return true;
}

/**
 * @brief Is the common text of an apparent line range still in the
 * result exactly as it was generated?
 */
bool CMergeDoc::IsResultCommonTextIntact(int nApparentBegin, int nApparentEnd) const
{
	const CDiffTextBuffer* pBaseBuf = m_ptBuf[m_nMergeBasePane].get();
	for (int nLine = nApparentBegin; nLine <= nApparentEnd && nLine < pBaseBuf->GetLineCount(); ++nLine)
	{
		if (m_diffList.LineToDiff(nLine) != -1 || (pBaseBuf->GetLineFlags(nLine) & LF_GHOST))
			continue;
		int nResultLine = -1;
		if (!MapPaneLineToResult(m_nMergeBasePane, nLine, nResultLine) ||
			nResultLine >= m_ptResultBuf->GetLineCount())
			return false;
		const int nLength = pBaseBuf->GetLineLength(nLine);
		if (m_ptResultBuf->GetLineLength(nResultLine) != nLength ||
			(nLength > 0 && memcmp(m_ptResultBuf->GetLineChars(nResultLine),
				pBaseBuf->GetLineChars(nLine), nLength * sizeof(tchar_t)) != 0))
			return false;
	}
	return true;
}

/**
 * @brief Make the result read like a pane with some of its lines rewritten.
 *
 * The given differences take the pane's text, then the lines of the pane's
 * range that differ from the new lines are replaced like a hand edit, so
 * that the segment table follows. All of it is one undo group of its own,
 * which is taken back if the rewrite does not work out.
 *
 * @return 1 when the result changed, 0 when it already read that way,
 *         -1 when the rewrite is not possible.
 */
int CMergeDoc::RewritePaneLinesInResult(int nPane, int nBeginLine, int nEndLine,
	const std::vector<String>& newLines, const std::vector<int>& diffs, bool& bGroupWithPrevious)
{
	std::vector<String> oldLines;
	std::vector<int> oldApparent;
	GetPaneRealLines(nPane, nBeginLine, nEndLine, oldLines, &oldApparent);
	if (oldLines.empty())
		return -1;

	size_t nPrefix = 0;
	while (nPrefix < oldLines.size() && nPrefix < newLines.size() && oldLines[nPrefix] == newLines[nPrefix])
		++nPrefix;
	size_t nSuffix = 0;
	while (nSuffix < oldLines.size() - nPrefix && nSuffix < newLines.size() - nPrefix &&
		oldLines[oldLines.size() - 1 - nSuffix] == newLines[newLines.size() - 1 - nSuffix])
		++nSuffix;
	const size_t nOldEnd = oldLines.size() - nSuffix; // exclusive
	const size_t nNewEnd = newLines.size() - nSuffix; // exclusive
	const bool bRewrite = nPrefix < nOldEnd || nPrefix < nNewEnd;

	// A difference without lines of the pane shows as a placeholder line,
	// which the rewrite must not run across
	for (int nDiff : diffs)
	{
		const DIFFRANGE* pdi = m_diffList.DiffRangeAt(nDiff);
		std::vector<String> lines;
		GetPaneRealLines(nPane, pdi->dbegin, pdi->dend, lines, nullptr);
		if (lines.empty() && nPrefix < nOldEnd &&
			pdi->dbegin > oldApparent[nPrefix] && pdi->dend < oldApparent[nOldEnd - 1])
			return -1;
	}

	CCrystalTextView* pSource = (m_pMergeResultView != nullptr &&
		m_pMergeResultView->GetSafeHwnd() != nullptr) ? m_pMergeResultView : nullptr;
	const bool bOuterGroup = bGroupWithPrevious;
	bGroupWithPrevious = false;
	if (m_pMergeResultView != nullptr)
		m_pMergeResultView->HideCursor();

	bool bChanged = false;
	for (int nDiff : diffs)
	{
		const MergeResultSegment& seg = m_resultSegments[m_resultDiffToSegment[nDiff]];
		const bool bPaneText = seg.srcPanes.size() == 1 && seg.srcPanes[0] == nPane &&
			(seg.state == ResultSegmentState::Common || seg.state == ResultSegmentState::Auto ||
			 seg.state == ResultSegmentState::Chosen);
		if (bPaneText)
			continue;
		ResultChooseSources(nDiff, { nPane }, bGroupWithPrevious);
		bGroupWithPrevious = true;
		bChanged = true;
	}

	bool bOk = true;
	if (bRewrite)
	{
		int nInsertLine = -1, nLastOldLine = -1;
		if (nPrefix < nOldEnd)
		{
			bOk = MapPaneLineToResult(nPane, oldApparent[nPrefix], nInsertLine) &&
				MapPaneLineToResult(nPane, oldApparent[nOldEnd - 1], nLastOldLine) &&
				nLastOldLine - nInsertLine == static_cast<int>(nOldEnd - nPrefix) - 1 &&
				nLastOldLine + 1 < m_ptResultBuf->GetLineCount();
		}
		else if (nPrefix < oldLines.size())
			bOk = MapPaneLineToResult(nPane, oldApparent[nPrefix], nInsertLine);
		else
		{
			bOk = MapPaneLineToResult(nPane, oldApparent.back(), nInsertLine) &&
				++nInsertLine < m_ptResultBuf->GetLineCount();
		}

		if (bOk)
		{
			const tchar_t* pszEol = m_ptBuf[nPane]->GetLineEol(oldApparent.front());
			if (pszEol == nullptr || *pszEol == _T('\0'))
				pszEol = m_ptResultBuf->GetDefaultEol();
			String text;
			for (size_t i = nPrefix; i < nNewEnd; ++i)
				text += newLines[i] + pszEol;
			const int nInserted = static_cast<int>(nNewEnd - nPrefix);

			// the new lines go in first to keep the segment of the old ones alive
			m_ptResultBuf->BeginUndoGroup(bGroupWithPrevious);
			bool bInsertedText = false;
			if (!text.empty())
			{
				int nEndLine2 = 0, nEndChar = 0;
				bOk = m_ptResultBuf->InsertText(pSource, nInsertLine, 0, text.c_str(), text.length(),
					nEndLine2, nEndChar, CE_ACTION_UNKNOWN, true);
				bInsertedText = bOk;
			}
			if (bOk && nPrefix < nOldEnd)
			{
				bOk = m_ptResultBuf->DeleteText(pSource, nInsertLine + nInserted, 0,
					nLastOldLine + nInserted + 1, 0, CE_ACTION_UNKNOWN, true, false);
			}
			m_ptResultBuf->FlushUndoGroup(pSource);
			if (bOk || bInsertedText)
			{
				bGroupWithPrevious = true;
				bChanged = true;
			}
		}
	}

	if (m_pMergeResultView != nullptr)
	{
		m_pMergeResultView->ShowCursor();
		if (m_pMergeResultView->GetSafeHwnd() != nullptr)
			m_pMergeResultView->Invalidate();
	}
	if (!bOk && bChanged)
	{
		// (Undo takes a null view: it only updates the view's cursor when there is one)
		CEPoint ptCursorPos;
		m_ptResultBuf->Undo(pSource, ptCursorPos);
	}
	if (!bOk || !bChanged)
		bGroupWithPrevious = bOuterGroup;
	return !bOk ? -1 : bChanged ? 1 : 0;
}

/**
 * @brief Roles of the panes for the semantic analyzer, from the merge pane mapping (G1).
 */
SemanticMerge::MergeRoles CMergeDoc::GetSemanticMergeRoles() const
{
	const MergePanes& panes = GetMergePaneMapping(m_nMergeBasePane);
	SemanticMerge::MergeRoles roles;
	roles.base = panes.nBasePane;
	roles.mine = panes.nMinePane;
	roles.theirs = panes.nTheirsPane;
	return roles;
}

/**
 * @brief G4: the file of nPane with lines nBeginLine..nEndLine replaced by
 * newText must not have more syntax errors than the worse of the two sides'
 * files.
 * @param newText Whole lines, each ending with a line end; empty removes the lines.
 */
bool CMergeDoc::IsSemanticResultSyntaxOk(int nPane, int nBeginLine, int nEndLine, const String& newText)
{
	CTreeSitterParser* pParser = GetTreeSitterParser(nPane);
	const CTreeSitterLanguage* pLang = (pParser != nullptr && pParser->HasLanguage()) ? pParser->GetLanguage() : nullptr;
	if (pLang == nullptr)
		return false;
	// the compared panes are read-only during a merge session, so the sides'
	// counts hold until the result is built again
	if (m_nSemanticSidesErrors < 0)
	{
		const MergePanes& panes = GetMergePaneMapping(m_nMergeBasePane);
		auto wholeFile = [&](int nFilePane)
		{
			return GetPaneApparentLinesText(nFilePane, 0, m_ptBuf[nFilePane]->GetLineCount() - 1, nullptr);
		};
		m_nSemanticSidesErrors = (std::max)(pLang->CountSyntaxErrors(wholeFile(panes.nMinePane)),
			pLang->CountSyntaxErrors(wholeFile(panes.nTheirsPane)));
	}
	const int nLastLine = m_ptBuf[nPane]->GetLineCount() - 1;
	const String text = GetPaneApparentLinesText(nPane, 0, nBeginLine - 1, nullptr) + newText +
		GetPaneApparentLinesText(nPane, nEndLine + 1, nLastLine, nullptr);
	const int nErrors = pLang->CountSyntaxErrors(text);
	return nErrors >= 0 && nErrors <= m_nSemanticSidesErrors;
}

/**
 * @brief Apply a definition-level resolution (R4-R6) to the merge result.
 *
 * The result is to read like the suggestion's anchor pane with the
 * definition replaced. Where that text can be had by choosing panes for the
 * differences it covers, that is done, which keeps the provenance markers.
 * Otherwise those differences take the anchor pane's text and the lines
 * that differ are rewritten like a hand edit.
 *
 * @param [in,out] bGroupWithPrevious Undo grouping across several calls.
 */
bool CMergeDoc::ApplySemanticSuggestionToResult(const SemanticMerge::Suggestion& suggestion,
	bool& bGroupWithPrevious, String& message)
{
	const int nDiffCount = m_diffList.GetSize();
	if (!m_bResultBuilt || m_nBuffers != 3 || static_cast<int>(m_resultDiffToSegment.size()) != nDiffCount)
	{
		message = _("The merge result no longer matches the compared files.");
		return false;
	}

	const int nBasePane = m_nMergeBasePane;
	const int anchorPane = suggestion.anchorPane;
	const int srcPane = suggestion.changedPane;
	const String failed = _("The resolution cannot be applied to the merge result.");
	if (anchorPane < 0 || anchorPane >= m_nBuffers || srcPane < 0 || srcPane >= m_nBuffers)
	{
		message = failed;
		return false;
	}

	// Anchor lines the resolution rewrites, and their new text
	const CDiffTextBuffer* pAnchorBuf = m_ptBuf[anchorPane].get();
	const SemanticMerge::TagRange& anchorTag = suggestion.defs[anchorPane].tag;
	const int nEditBegin = anchorTag.startLine;
	const int nEditEnd = anchorTag.endLine;
	if (nEditBegin < 0 || nEditEnd < nEditBegin || nEditEnd >= pAnchorBuf->GetLineCount() ||
		(pAnchorBuf->GetLineFlags(nEditBegin) & LF_GHOST) || (pAnchorBuf->GetLineFlags(nEditEnd) & LF_GHOST) ||
		anchorTag.startChar < 0 || anchorTag.startChar > pAnchorBuf->GetLineLength(nEditBegin) ||
		anchorTag.endChar < 0 || anchorTag.endChar > pAnchorBuf->GetLineLength(nEditEnd))
	{
		message = failed;
		return false;
	}
	String newText(pAnchorBuf->GetLineChars(nEditBegin), anchorTag.startChar);
	newText += suggestion.replacementText;
	newText.append(pAnchorBuf->GetLineChars(nEditEnd) + anchorTag.endChar,
		pAnchorBuf->GetLineLength(nEditEnd) - anchorTag.endChar);
	std::vector<String> newLines;
	SplitTextIntoLines(newText, newLines);

	// G4
	{
		String newLinesText;
		for (const String& line : newLines)
			newLinesText += line + _T("\n");
		if (!IsSemanticResultSyntaxOk(anchorPane, nEditBegin, nEditEnd, newLinesText))
		{
			message = _("The resolution would add syntax errors.");
			return false;
		}
	}

	// The definition's lines in the other panes are accounted for by the
	// resolution: the differences holding them are involved, and their
	// lines must not end up in the result a second time
	std::vector<SemanticMerge::DefinitionInfo> sources;
	for (int nPane = 0; nPane < m_nBuffers; ++nPane)
	{
		if (nPane != anchorPane && suggestion.defs[nPane].pane == nPane)
			sources.push_back(suggestion.defs[nPane]);
	}
	auto isSourceLine = [&](int nPane, int nLine)
	{
		for (const auto& def : sources)
		{
			if (def.pane == nPane && nLine >= def.tag.startLine && nLine <= def.tag.endLine)
				return true;
		}
		return false;
	};
	auto isEditLine = [&](int nLine)
	{
		return nLine >= nEditBegin && nLine <= nEditEnd;
	};
	// What a difference holds besides the lines the resolution accounts
	// for. Taking the anchor pane's text for the difference must not decide
	// anything else on the way.
	auto hasOnlySuggestionLines = [&](const DIFFRANGE* pdi)
	{
		const bool bAnchorSide = pdi->dbegin <= nEditEnd && pdi->dend >= nEditBegin;
		std::vector<String> rest[3];
		bool bHasSource[3] = { false, false, false };
		for (int nPane = 0; nPane < m_nBuffers; ++nPane)
		{
			std::vector<String> lines;
			std::vector<int> apparent;
			GetPaneRealLines(nPane, pdi->dbegin, pdi->dend, lines, &apparent);
			for (size_t i = 0; i < lines.size(); ++i)
			{
				const bool bSource = nPane != anchorPane && isSourceLine(nPane, apparent[i]);
				bHasSource[nPane] = bHasSource[nPane] || bSource;
				if (!bSource && !isEditLine(apparent[i]) && !IsBlankLineText(lines[i]))
					rest[nPane].push_back(lines[i]);
			}
		}
		// A pane that is neither the anchor, the changed side nor the base
		// has changes of its own; the resolution must not run over them
		auto nonBlankLines = [&](int nPane)
		{
			std::vector<String> lines, nonBlank;
			GetPaneRealLines(nPane, pdi->dbegin, pdi->dend, lines, nullptr);
			AppendNonBlankLines(lines, nonBlank);
			return nonBlank;
		};
		for (int nPane = 0; nPane < m_nBuffers; ++nPane)
		{
			if (nPane == anchorPane || nPane == srcPane || nPane == nBasePane)
				continue;
			const std::vector<String> lines = nonBlankLines(nPane);
			if (lines != nonBlankLines(anchorPane) && lines != nonBlankLines(srcPane))
				return false;
		}
		// The other lines must read the same in every pane; where a pane
		// holds the definition somewhere else instead, it may have none
		for (int nPane = 0; nPane < m_nBuffers; ++nPane)
		{
			if (nPane == anchorPane || rest[nPane] == rest[anchorPane])
				continue;
			if (bAnchorSide || !bHasSource[nPane] || !rest[nPane].empty())
				return false;
		}
		return true;
	};
	const String otherChanges = _("The differences of this definition also contain other changes; resolve them in the merge result first.");
	std::vector<int> diffs;
	int nRegionBegin = nEditBegin, nRegionEnd = nEditEnd;
	for (int nDiff = 0; nDiff < nDiffCount; ++nDiff)
	{
		const DIFFRANGE* pdi = m_diffList.DiffRangeAt(nDiff);
		bool bInvolved = pdi->dbegin <= nEditEnd && pdi->dend >= nEditBegin;
		for (const auto& def : sources)
			bInvolved = bInvolved || (pdi->dbegin <= def.tag.endLine && pdi->dend >= def.tag.startLine);
		if (!bInvolved)
			continue;
		if (!hasOnlySuggestionLines(pdi))
		{
			message = otherChanges;
			return false;
		}
		if (m_resultDiffToSegment[nDiff] < 0)
		{
			message = failed;
			return false;
		}
		diffs.push_back(nDiff);
		nRegionBegin = (std::min)(nRegionBegin, pdi->dbegin);
		nRegionEnd = (std::max)(nRegionEnd, pdi->dend);
	}

	// G3: only pending differences, none decided by hand or by another rule
	bool bAnyPending = false, bAnyDecided = false;
	for (int nDiff : diffs)
	{
		const ResultSegmentState state = m_resultSegments[m_resultDiffToSegment[nDiff]].state;
		bAnyPending = bAnyPending || state == ResultSegmentState::Conflict || state == ResultSegmentState::Unresolved;
		bAnyDecided = bAnyDecided || state == ResultSegmentState::Chosen || state == ResultSegmentState::Edited ||
			state == ResultSegmentState::Semantic;
	}
	if (!bAnyPending || bAnyDecided)
	{
		message = _("The differences of this definition are already resolved in the merge result.");
		return false;
	}
	if (!IsResultCommonTextIntact(nRegionBegin, nRegionEnd))
	{
		message = _("The merge result was edited by hand around this definition.");
		return false;
	}

	// Whole lines of the region as the anchor pane should read afterwards
	std::vector<String> wanted;
	{
		std::vector<String> lines;
		GetPaneRealLines(anchorPane, nRegionBegin, nEditBegin - 1, lines, nullptr);
		lines.insert(lines.end(), newLines.begin(), newLines.end());
		GetPaneRealLines(anchorPane, nEditEnd + 1, nRegionEnd, lines, nullptr);
		AppendNonBlankLines(lines, wanted);
	}

	// 1. Try to get there by choosing panes for the differences
	std::vector<SemanticRegionItem> items;
	int nApparent = nRegionBegin;
	auto addCommon = [&](int nBegin, int nEndExcl)
	{
		if (nBegin >= nEndExcl)
			return;
		SemanticRegionItem item;
		std::vector<String> lines;
		GetPaneRealLines(anchorPane, nBegin, nEndExcl - 1, lines, nullptr);
		AppendNonBlankLines(lines, item.commonLines);
		items.push_back(std::move(item));
	};
	for (int nDiff : diffs)
	{
		const DIFFRANGE* pdi = m_diffList.DiffRangeAt(nDiff);
		addCommon(nApparent, pdi->dbegin);
		SemanticRegionItem item;
		item.nDiff = nDiff;
		const MergeResultSegment& seg = m_resultSegments[m_resultDiffToSegment[nDiff]];
		auto addCandidate = [&](const std::vector<int>& panes)
		{
			if (panes.empty() || std::find(item.candidates.begin(), item.candidates.end(), panes) != item.candidates.end())
				return;
			std::vector<String> lines, nonBlank;
			for (int nPane : panes)
				GetPaneRealLines(nPane, pdi->dbegin, pdi->dend, lines, nullptr);
			AppendNonBlankLines(lines, nonBlank);
			item.candidates.push_back(panes);
			item.candidateLines.push_back(std::move(nonBlank));
		};
		// keep what the result already has when it fits
		if (seg.state == ResultSegmentState::Common || seg.state == ResultSegmentState::Auto ||
			seg.state == ResultSegmentState::Chosen)
			addCandidate(seg.srcPanes);
		addCandidate({ srcPane });
		addCandidate({ anchorPane });
		for (int nPane = 0; nPane < m_nBuffers; ++nPane)
		{
			if (nPane != nBasePane || anchorPane == nBasePane)
				addCandidate({ nPane });
		}
		if (pdi->op == OP_DIFF)
		{
			// both sides added something: keep both (either order)
			const int nOther1 = anchorPane == 0 ? 1 : 0;
			const int nOther2 = anchorPane == 2 ? 1 : 2;
			addCandidate({ nOther1, nOther2 });
			addCandidate({ nOther2, nOther1 });
		}
		items.push_back(std::move(item));
		nApparent = pdi->dend + 1;
	}
	addCommon(nApparent, nRegionEnd + 1);

	auto isResolvedWith = [&](int nDiff, const std::vector<int>& panes)
	{
		const MergeResultSegment& seg = m_resultSegments[m_resultDiffToSegment[nDiff]];
		return seg.srcPanes == panes &&
			(seg.state == ResultSegmentState::Common || seg.state == ResultSegmentState::Auto ||
			 seg.state == ResultSegmentState::Chosen);
	};
	auto choose = [&](int nDiff, const std::vector<int>& panes)
	{
		if (isResolvedWith(nDiff, panes))
			return false;
		ResultChooseSources(nDiff, panes, bGroupWithPrevious);
		bGroupWithPrevious = true;
		return true;
	};

	const SemanticResolutionSnapshot before = BeginSemanticResolution();
	auto recordResolution = [&]()
	{
		SemanticRule rule = SemanticRule::DefinitionChange;
		String reason;
		const String& name = suggestion.displayName;
		const String changed = ResultPaneLabel(srcPane);
		const String anchor = ResultPaneLabel(anchorPane);
		switch (suggestion.kind)
		{
		case SemanticMerge::SuggestionKind::DefinitionChange:
			reason = strutils::format_string2(_("One-sided change of %1: only %2 changed it"), name, changed);
			break;
		case SemanticMerge::SuggestionKind::StringLiteral:
			rule = SemanticRule::LocalizedChange;
			reason = strutils::format_string3(_("%1: the string literal change of %2 replayed onto the version of %3"), name, changed, anchor);
			break;
		case SemanticMerge::SuggestionKind::ParameterRename:
			rule = SemanticRule::LocalizedChange;
			reason = strutils::format_string3(_("%1: the parameter rename of %2 replayed onto the version of %3"), name, changed, anchor);
			break;
		case SemanticMerge::SuggestionKind::IdentifierRename:
			rule = SemanticRule::LocalizedChange;
			reason = strutils::format_string3(_("%1: the identifier rename of %2 replayed onto the version of %3"), name, changed, anchor);
			break;
		case SemanticMerge::SuggestionKind::CommentedOut:
			rule = SemanticRule::CommentedOut;
			reason = strutils::format_string3(_("%1: commented out by %2, with the edit of %3"), name, anchor, changed);
			break;
		}
		EndSemanticResolution(before, rule, reason);
	};

	int nBudget = 20000;
	if (!diffs.empty() && MatchSemanticRegion(items, 0, wanted, 0, nBudget))
	{
		bool bChanged = false;
		for (const auto& item : items)
		{
			if (item.nDiff >= 0)
				bChanged = choose(item.nDiff, item.candidates[item.nChosen]) || bChanged;
		}
		if (bChanged)
			recordResolution();
		else
			message = _("The merge result already reads this way.");
		return bChanged;
	}

	// 2. Otherwise the differences take the anchor pane's text and the
	// lines that differ are rewritten
	const int nRewritten = RewritePaneLinesInResult(anchorPane, nEditBegin, nEditEnd, newLines, diffs, bGroupWithPrevious);
	if (nRewritten < 0)
		message = failed;
	else if (nRewritten == 0)
		message = _("The merge result already reads this way.");
	else
		recordResolution();
	return nRewritten > 0;
}

/**
 * @brief Resolve a difference in the merge result with a text that is
 * not the text of one pane (a merge of several).
 */
bool CMergeDoc::ResultResolveWithText(int nDiff, const String& resolvedText, bool bGroupWithPrevious)
{
	if (!m_bResultBuilt || nDiff < 0 || nDiff >= static_cast<int>(m_resultDiffToSegment.size()) ||
		nDiff >= m_diffList.GetSize())
		return false;
	const int nSegment = m_resultDiffToSegment[nDiff];
	if (nSegment < 0)
		return false;
	MergeResultSegment& seg = m_resultSegments[nSegment];
	const int nLineCount = m_ptResultBuf->GetLineCount();
	// the generated text ends every line with an EOL, so a line follows the segment
	if (seg.nStartLine < 0 || seg.nLines < 0 || seg.nStartLine + seg.nLines >= nLineCount)
		return false;

	String text = resolvedText;
	int nNewLines = 0;
	for (size_t pos = 0; pos < text.length(); ++pos)
	{
		if (text[pos] == _T('\n') || (text[pos] == _T('\r') && (pos + 1 == text.length() || text[pos + 1] != _T('\n'))))
			++nNewLines;
	}
	if (!text.empty() && text.back() != _T('\n') && text.back() != _T('\r'))
	{
		text += m_ptResultBuf->GetDefaultEol();
		++nNewLines;
	}
	const bool bNoSrc = nNewLines == 0;
	if (bNoSrc)
	{
		text = GetResultNoSrcLineText(m_ptResultBuf->GetDefaultEol());
		nNewLines = 1;
	}

	CMergeResultTextBuffer::InternalOpGuard guard(*m_ptResultBuf);
	CCrystalTextView* pSource = (m_pMergeResultView != nullptr &&
		m_pMergeResultView->GetSafeHwnd() != nullptr) ? m_pMergeResultView : nullptr;

	// lines written from here on are newer than the segment's baseline,
	// which marks them as not coming from a pane
	const uint32_t nBaseRevision = m_ptResultBuf->GetCurrentRevisionNumber();
	m_ptResultBuf->BeginUndoGroup(bGroupWithPrevious);
	if (seg.nLines > 0)
		m_ptResultBuf->DeleteText(pSource, seg.nStartLine, 0, seg.nStartLine + seg.nLines, 0,
			CE_ACTION_UNKNOWN, true, false);
	int nEndLine = 0, nEndChar = 0;
	m_ptResultBuf->InsertText(pSource, seg.nStartLine, 0, text.c_str(), text.length(),
		nEndLine, nEndChar, CE_ACTION_UNKNOWN, true);
	m_ptResultBuf->FlushUndoGroup(pSource);

	const int nDelta = nNewLines - seg.nLines;
	seg.state = ResultSegmentState::Edited;
	seg.srcPanes.clear();
	seg.srcPaneLines.clear();
	seg.nLines = nNewLines;
	seg.bNoSrc = bNoSrc;
	seg.blockText.clear();
	seg.nBlockLines = 0;
	seg.nBaseRevision = nBaseRevision;
	if (nDelta != 0)
	{
		for (size_t i = nSegment + 1; i < m_resultSegments.size(); ++i)
			m_resultSegments[i].nStartLine += nDelta;
	}
	if (m_pMergeResultView != nullptr && m_pMergeResultView->GetSafeHwnd() != nullptr)
		m_pMergeResultView->Invalidate();
	return true;
}

/**
 * @brief Try to resolve an unresolved conflict of the merge result by
 * merging what each side changed in it, token by token.
 *
 * The merged text is only used when the file still parses as well with
 * it as with either side's version of the conflict.
 */
/**
 * @brief R1 within one conflict: both sides added lines at the same place
 * and one side's addition holds the other's whole: take that side.
 */
bool CMergeDoc::TryContainedAdditionToResult(int nDiff, bool& bGroupWithPrevious)
{
	if (!m_bResultBuilt || m_nBuffers != 3 || nDiff < 0 || nDiff >= m_diffList.GetSize() ||
		static_cast<int>(m_resultDiffToSegment.size()) != m_diffList.GetSize() || !IsResultDiffPending(nDiff))
		return false;
	const DIFFRANGE* pdi = m_diffList.DiffRangeAt(nDiff);
	if (pdi == nullptr || pdi->op != OP_DIFF)
		return false;
	const MergePanes& panes = GetMergePaneMapping(m_nMergeBasePane);
	std::vector<String> baseLines, mineLines, theirsLines;
	GetPaneRealLines(panes.nBasePane, pdi->dbegin, pdi->dend, baseLines, nullptr);
	if (!SemanticMerge::SqueezeNonBlankLines(baseLines).empty())
		return false;
	GetPaneRealLines(panes.nMinePane, pdi->dbegin, pdi->dend, mineLines, nullptr);
	GetPaneRealLines(panes.nTheirsPane, pdi->dbegin, pdi->dend, theirsLines, nullptr);
	CTreeSitterParser* pParser = GetTreeSitterParser(panes.nMergeDestPane);
	const CTreeSitterLanguage* pLang = (pParser != nullptr && pParser->HasLanguage()) ? pParser->GetLanguage() : nullptr;
	if (pLang == nullptr)
		return false;
	const SemanticMerge::DuplicateDrop drop = SemanticMerge::ChooseContainedAdditionToDrop(mineLines, theirsLines,
		SemanticMerge::TraitsForLanguage(pLang->GetName()));
	if (drop == SemanticMerge::DuplicateDrop::None)
		return false;
	const int nKeep = drop == SemanticMerge::DuplicateDrop::Theirs ? panes.nMinePane : panes.nTheirsPane;
	const int nDrop = drop == SemanticMerge::DuplicateDrop::Theirs ? panes.nTheirsPane : panes.nMinePane;

	const SemanticResolutionSnapshot before = BeginSemanticResolution();
	ResultChooseSource(nDiff, nKeep, bGroupWithPrevious);
	bGroupWithPrevious = true;
	EndSemanticResolution(before, SemanticRule::DuplicateAddition,
		strutils::format_string2(_("Duplicate addition: %1 added the same lines as %2, and more; took %1"),
			ResultPaneLabel(nKeep), ResultPaneLabel(nDrop)));
	return true;
}

bool CMergeDoc::TryTokenMergeConflictToResult(int nDiff, bool& bGroupWithPrevious, String& why)
{
	if (!m_bResultBuilt || m_nBuffers != 3 || nDiff < 0 || nDiff >= m_diffList.GetSize() ||
		static_cast<int>(m_resultDiffToSegment.size()) != m_diffList.GetSize() || !IsResultDiffPending(nDiff))
	{
		why = _("the conflict is not pending.");
		return false;
	}
	const DIFFRANGE* pdi = m_diffList.DiffRangeAt(nDiff);
	if (pdi == nullptr || pdi->op != OP_DIFF)
	{
		why = _("the difference is not a conflict.");
		return false;
	}

	const MergePanes& panes = GetMergePaneMapping(m_nMergeBasePane);
	CTreeSitterParser* pParser = GetTreeSitterParser(panes.nMergeDestPane);
	const CTreeSitterLanguage* pLang = (pParser != nullptr && pParser->HasLanguage()) ? pParser->GetLanguage() : nullptr;
	if (pLang == nullptr)
	{
		why = _("there is no tree-sitter grammar for this file type.");
		return false;
	}

	const String baseText = GetPaneApparentLinesText(panes.nBasePane, pdi->dbegin, pdi->dend, nullptr);
	const String mineText = GetPaneApparentLinesText(panes.nMinePane, pdi->dbegin, pdi->dend, nullptr);
	const String theirsText = GetPaneApparentLinesText(panes.nTheirsPane, pdi->dbegin, pdi->dend, nullptr);
	String merged;
	if (!SemanticMerge::MergeTokensThreeWay(baseText, mineText, theirsText,
			SemanticMerge::TraitsForLanguage(pLang->GetName()), merged))
	{
		why = IsBlankLineText(baseText) ?
			_("both sides added different lines at the same place; which goes first is yours to decide.") :
			_("the two sides changed the same words, or one side changed too much to line the changes up.");
		return false;
	}

	// G4: compare the whole file with the conflict merged against the file
	// with either side's version of it
	const int nLastLine = m_ptBuf[panes.nBasePane]->GetLineCount() - 1;
	const String before = GetPaneApparentLinesText(panes.nBasePane, 0, pdi->dbegin - 1, nullptr);
	const String after = GetPaneApparentLinesText(panes.nBasePane, pdi->dend + 1, nLastLine, nullptr);
	const int nMergedErrors = pLang->CountSyntaxErrors(before + merged + after);
	const int nMineErrors = pLang->CountSyntaxErrors(before + mineText + after);
	const int nTheirsErrors = pLang->CountSyntaxErrors(before + theirsText + after);
	if (nMergedErrors < 0 || nMergedErrors > (std::max)(nMineErrors, nTheirsErrors))
	{
		why = _("the merged lines would add syntax errors.");
		return false;
	}

	const SemanticResolutionSnapshot snapshot = BeginSemanticResolution();
	if (!ResultResolveWithText(nDiff, merged, bGroupWithPrevious))
	{
		why = _("the merge result could not be changed.");
		return false;
	}
	bGroupWithPrevious = true;
	EndSemanticResolution(snapshot, SemanticRule::TokenMerge,
		strutils::format_string2(_("Token merge: %1 and %2 changed different parts of these lines"),
			ResultPaneLabel(panes.nMinePane), ResultPaneLabel(panes.nTheirsPane)));
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Definition-level merges in the merge result

namespace
{

OP_TYPE OnlyPaneOp(int nPane)
{
	return nPane == 0 ? OP_1STONLY : nPane == 1 ? OP_2NDONLY : OP_3RDONLY;
}

/**
 * @brief Definitions with their range in whole lines of their own text.
 *
 * Some grammars let a definition run on to where the next one starts,
 * which takes in the blank lines and the comment in front of the next
 * definition. Those lines are cut off the end.
 */
std::vector<CTreeSitterParser::TagRange> WholeLineTags(std::vector<CTreeSitterParser::TagRange> tags,
	const CDiffTextBuffer& buf, const std::vector<String>& lineCommentPrefixes)
{
	for (auto& tag : tags)
	{
		while (tag.endLine > tag.startLine && tag.endLine < buf.GetLineCount())
		{
			const tchar_t* pszLine = buf.GetLineChars(tag.endLine);
			const int nLineLength = buf.GetLineLength(tag.endLine);
			// only the part of the line that is inside the range counts
			const int nLength = (std::min)(tag.endChar, nLineLength);
			int nFirst = 0;
			while (nFirst < nLength && (pszLine[nFirst] == _T(' ') || pszLine[nFirst] == _T('\t')))
				++nFirst;
			bool bCut = nFirst == nLength;
			for (const String& prefix : lineCommentPrefixes)
			{
				bCut = bCut || (!prefix.empty() && nFirst + static_cast<int>(prefix.length()) <= nLineLength &&
					String(pszLine + nFirst, prefix.length()) == prefix);
			}
			if (!bCut)
				break;
			--tag.endLine;
			tag.endChar = buf.GetLineLength(tag.endLine);
		}
	}
	return tags;
}

/** @brief Name of the innermost definition around a definition, empty for none. */
std::string EnclosingTagName(const std::vector<CTreeSitterParser::TagRange>& tags, const CTreeSitterParser::TagRange& tag)
{
	const CTreeSitterParser::TagRange* pEnclosing = nullptr;
	for (const auto& other : tags)
	{
		if (other.startByte <= tag.startByte && other.endByte >= tag.endByte &&
			(other.startByte != tag.startByte || other.endByte != tag.endByte) &&
			(pEnclosing == nullptr || other.endByte - other.startByte < pEnclosing->endByte - pEnclosing->startByte))
			pEnclosing = &other;
	}
	return pEnclosing != nullptr ? pEnclosing->name : std::string();
}

/** @brief Does no other definition share lines with this one (nesting aside)? */
bool HasLinesToItself(const std::vector<CTreeSitterParser::TagRange>& tags, const CTreeSitterParser::TagRange& tag)
{
	for (const auto& other : tags)
	{
		const bool bNested = (other.startByte <= tag.startByte && other.endByte >= tag.endByte) ||
			(tag.startByte <= other.startByte && tag.endByte >= other.endByte);
		if (!bNested && other.startLine <= tag.endLine && other.endLine >= tag.startLine)
			return false;
	}
	return true;
}

}

/**
 * @brief The whole lines a definition spans.
 * @param [out] apparent Apparent line of each returned (non-ghost) line.
 */
bool CMergeDoc::GetDefinitionLines(int nPane, int nStartLine, int nEndLine,
	std::vector<String>& lines, std::vector<int>& apparent) const
{
	if (nStartLine < 0 || nEndLine < nStartLine || nEndLine >= m_ptBuf[nPane]->GetLineCount())
		return false;
	GetPaneRealLines(nPane, nStartLine, nEndLine, lines, &apparent);
	return !lines.empty();
}

/**
 * @brief A definition that one side moved while the other side changed
 * it where it was: put the changed definition where it was moved to.
 *
 * A line-based merge sees a block that one side removed and the other
 * changed, which is a conflict, plus an unrelated addition elsewhere. In
 * terms of what each side did there is no conflict: one moved the
 * definition, the other edited it.
 *
 * @param [in] nDiff An unresolved conflict inside the definition's old place.
 */
bool CMergeDoc::TryMovedDefinitionMergeToResult(int nDiff, bool& bGroupWithPrevious, String& why)
{
	if (!m_bResultBuilt || m_nBuffers != 3 || nDiff < 0 || nDiff >= m_diffList.GetSize() ||
		static_cast<int>(m_resultDiffToSegment.size()) != m_diffList.GetSize() || !IsResultDiffPending(nDiff))
	{
		why = _("the conflict is not pending.");
		return false;
	}
	const DIFFRANGE* pConflict = m_diffList.DiffRangeAt(nDiff);
	if (pConflict == nullptr || pConflict->op != OP_DIFF)
	{
		why = _("the difference is not a conflict.");
		return false;
	}

	const MergePanes& panes = GetMergePaneMapping(m_nMergeBasePane);
	const int nBase = panes.nBasePane;
	std::vector<CTreeSitterParser::TagRange> tags[3];
	const CTreeSitterLanguage* pLang = nullptr;
	for (int nPane = 0; nPane < 3; ++nPane)
	{
		CTreeSitterParser* pParser = GetTreeSitterParser(nPane);
		pLang = (pParser != nullptr && pParser->HasLanguage()) ? pParser->GetLanguage() : nullptr;
		if (pLang == nullptr)
		{
			why = _("there is no tree-sitter grammar for this file type.");
			return false;
		}
		tags[nPane] = WholeLineTags(pParser->GetTagRanges(), *m_ptBuf[nPane],
			SemanticMerge::TraitsForLanguage(pLang->GetName()).lineCommentPrefixes);
	}

	// The innermost base definition the conflict lies in
	const CTreeSitterParser::TagRange* pBaseTag = nullptr;
	for (const auto& tag : tags[nBase])
	{
		if (tag.startLine <= pConflict->dend && tag.endLine >= pConflict->dbegin &&
			(pBaseTag == nullptr || tag.endLine - tag.startLine < pBaseTag->endLine - pBaseTag->startLine))
			pBaseTag = &tag;
	}
	if (pBaseTag == nullptr)
	{
		why = _("the conflict is not inside a definition.");
		return false;
	}
	CTreeSitterParser::TagRange sideTags[3];
	for (int nPane = 0; nPane < 3; ++nPane)
	{
		if (!SemanticMerge::TryFindUniqueTagByName(tags[nPane], pBaseTag->name, sideTags[nPane]))
		{
			why = _("the definition is not there exactly once in every pane.");
			return false;
		}
	}
	const CTreeSitterParser::TagRange& baseTag = sideTags[nBase];
	const CTreeSitterParser::TagRange& mineTag = sideTags[panes.nMinePane];
	const CTreeSitterParser::TagRange& theirsTag = sideTags[panes.nTheirsPane];

	// One side (the mover) has the definition somewhere else, the other
	// (the editor) still has it in place
	const SemanticMerge::MovedSide moved = SemanticMerge::DetectMovedDefinition(baseTag.startLine, baseTag.endLine,
		mineTag.startLine, mineTag.endLine, theirsTag.startLine, theirsTag.endLine);
	if (moved == SemanticMerge::MovedSide::None)
	{
		why = _("neither side moved the definition while the other kept it in place.");
		return false;
	}
	const int nMover = moved == SemanticMerge::MovedSide::Mine ? panes.nMinePane : panes.nTheirsPane;
	const int nEditor = moved == SemanticMerge::MovedSide::Mine ? panes.nTheirsPane : panes.nMinePane;
	const CTreeSitterParser::TagRange& moverTag = sideTags[nMover];
	const CTreeSitterParser::TagRange& editorTag = sideTags[nEditor];

	std::vector<String> defLines[3];
	std::vector<int> defApparent[3];
	for (int nPane = 0; nPane < 3; ++nPane)
	{
		// (a rewrite replaces whole lines)
		if (!HasLinesToItself(tags[nPane], sideTags[nPane]) ||
			!GetDefinitionLines(nPane, sideTags[nPane].startLine, sideTags[nPane].endLine,
				defLines[nPane], defApparent[nPane]))
		{
			why = _("the definition shares lines with another definition.");
			return false;
		}
	}

	// The result is to read like the mover's file, with the definition in
	// its new place carrying the editor's change. For that the editor must
	// have changed nothing else in the differences this touches.
	auto paneLines = [&](int nPane, const DIFFRANGE* pdi, const CTreeSitterParser::TagRange* pWithout)
	{
		std::vector<String> lines, kept;
		std::vector<int> apparent;
		GetPaneRealLines(nPane, pdi->dbegin, pdi->dend, lines, &apparent);
		for (size_t n = 0; n < lines.size(); ++n)
		{
			if (pWithout == nullptr || apparent[n] < pWithout->startLine || apparent[n] > pWithout->endLine)
				kept.push_back(lines[n]);
		}
		return SemanticMerge::SqueezeNonBlankLines(kept);
	};
	std::vector<int> diffs;
	bool bConflictIsOld = false;
	int nRegionBegin = (std::min)(baseTag.startLine, moverTag.startLine);
	int nRegionEnd = (std::max)(baseTag.endLine, moverTag.endLine);
	for (int i = 0; i < m_diffList.GetSize(); ++i)
	{
		const DIFFRANGE* pdi = m_diffList.DiffRangeAt(i);
		// where the definition was (the editor still has it there) ...
		const bool bOld = (pdi->dbegin <= baseTag.endLine && pdi->dend >= baseTag.startLine) ||
			(pdi->dbegin <= editorTag.endLine && pdi->dend >= editorTag.startLine);
		// ... and where the mover put it
		const bool bNew = pdi->dbegin <= moverTag.endLine && pdi->dend >= moverTag.startLine;
		if (!bOld && !bNew)
			continue;
		const int nSegment = m_resultDiffToSegment[i];
		if (nSegment < 0)
		{
			why = _("the merge result no longer matches the compared files.");
			return false;
		}
		const ResultSegmentState state = m_resultSegments[nSegment].state;
		if (state == ResultSegmentState::Chosen || state == ResultSegmentState::Edited ||
			state == ResultSegmentState::Semantic)
		{
			why = _("a difference of the definition is already resolved.");
			return false;
		}
		if (paneLines(nEditor, pdi, bOld ? &editorTag : nullptr) != paneLines(nBase, pdi, bOld ? &baseTag : nullptr))
		{
			why = _("the editing side also changed other code around the definition.");
			return false;
		}
		bConflictIsOld = bConflictIsOld || (bOld && i == nDiff);
		diffs.push_back(i);
		nRegionBegin = (std::min)(nRegionBegin, pdi->dbegin);
		nRegionEnd = (std::max)(nRegionEnd, pdi->dend);
	}
	if (!bConflictIsOld)
	{
		why = _("the conflict is not at the definition's old place.");
		return false;
	}
	if (!IsResultCommonTextIntact(nRegionBegin, nRegionEnd))
	{
		why = _("the merge result was edited by hand around the definition.");
		return false;
	}

	// The definition as it should read: what the editor did to it, applied
	// to the mover's copy
	std::vector<String> mergedLines;
	if (!SemanticMerge::MergeMovedDefinition(defLines[nBase], defLines[nMover], defLines[nEditor],
			SemanticMerge::TraitsForLanguage(pLang->GetName()), mergedLines))
	{
		why = _("the editing side only re-laid the definition out, or both sides changed the same words of it.");
		return false;
	}

	// G4
	String mergedText;
	for (const String& line : mergedLines)
		mergedText += line + _T("\n");
	if (!IsSemanticResultSyntaxOk(nMover, moverTag.startLine, moverTag.endLine, mergedText))
	{
		why = _("the merged definition would add syntax errors.");
		return false;
	}

	const SemanticResolutionSnapshot before = BeginSemanticResolution();
	if (RewritePaneLinesInResult(nMover, moverTag.startLine, moverTag.endLine, mergedLines, diffs,
		bGroupWithPrevious) <= 0)
	{
		why = _("the merge result could not be changed.");
		return false;
	}
	EndSemanticResolution(before, SemanticRule::MovedDefinition,
		strutils::format_string3(_("Moved definition %1: %2 moved it, %3 edited it"),
			ucr::toTString(baseTag.name), ResultPaneLabel(nMover), ResultPaneLabel(nEditor)));
	return true;
}

/**
 * @brief Both sides added the same definition, each in its own place:
 * keep it once.
 *
 * To a line-based merge these are two unrelated additions, and it takes
 * both, which leaves the definition in the result twice.
 *
 * @return Number of additions left out.
 */
int CMergeDoc::ResolveDuplicateAdditionsInResult(bool& bGroupWithPrevious)
{
	if (!m_bResultBuilt || m_nBuffers != 3 ||
		static_cast<int>(m_resultDiffToSegment.size()) != m_diffList.GetSize())
		return 0;
	const MergePanes& panes = GetMergePaneMapping(m_nMergeBasePane);
	std::vector<CTreeSitterParser::TagRange> tags[3];
	bool bLayoutSensitive = false;
	for (int nPane = 0; nPane < 3; ++nPane)
	{
		CTreeSitterParser* pParser = GetTreeSitterParser(nPane);
		if (pParser == nullptr || !pParser->HasLanguage())
			return 0;
		const CTreeSitterLanguage* pLang = pParser->GetLanguage();
		if (pLang == nullptr)
			return 0;
		const SemanticMerge::LanguageTraits traits = SemanticMerge::TraitsForLanguage(pLang->GetName());
		bLayoutSensitive = traits.layoutSensitive;
		tags[nPane] = WholeLineTags(pParser->GetTagRanges(), *m_ptBuf[nPane], traits.lineCommentPrefixes);
	}

	// The pending difference that only this pane changed and that holds the whole definition
	auto additionDiff = [&](int nPane, const CTreeSitterParser::TagRange& tag)
	{
		const int nDiff = m_diffList.LineToDiff(tag.startLine);
		if (nDiff == -1 || m_resultDiffToSegment[nDiff] < 0 || !IsResultDiffPending(nDiff))
			return -1;
		const DIFFRANGE* pdi = m_diffList.DiffRangeAt(nDiff);
		// (a definition's range can run on into the blank lines after it)
		int nLastLine = (std::min)(tag.endLine, m_ptBuf[nPane]->GetLineCount() - 1);
		while (nLastLine > tag.startLine && ((m_ptBuf[nPane]->GetLineFlags(nLastLine) & LF_GHOST) ||
			IsBlankLineText(String(m_ptBuf[nPane]->GetLineChars(nLastLine), m_ptBuf[nPane]->GetLineLength(nLastLine)))))
			--nLastLine;
		return (pdi->op == OnlyPaneOp(nPane) && pdi->dend >= nLastLine) ? nDiff : -1;
	};
	auto paneLines = [&](int nPane, int nBegin, int nEnd)
	{
		std::vector<String> lines;
		GetPaneRealLines(nPane, nBegin, nEnd, lines, nullptr);
		return lines;
	};

	int nResolved = 0;
	const int nMine = panes.nMinePane, nTheirs = panes.nTheirsPane;
	for (const auto& tag : tags[nMine])
	{
		bool bInBase = false;
		for (const auto& other : tags[panes.nBasePane])
			bInBase = bInBase || other.name == tag.name;
		CTreeSitterParser::TagRange mineTag, theirsTag;
		if (bInBase ||
			!SemanticMerge::TryFindUniqueTagByName(tags[nMine], tag.name, mineTag) ||
			!SemanticMerge::TryFindUniqueTagByName(tags[nTheirs], tag.name, theirsTag))
			continue;
		// the same definition: same text, in the same enclosing definition
		// (G5: where indentation carries meaning it is part of the text)
		auto definitionText = [&](int nPane, const CTreeSitterParser::TagRange& defTag)
		{
			const std::vector<String> lines = paneLines(nPane, defTag.startLine, defTag.endLine);
			return bLayoutSensitive ? SemanticMerge::IndentedNonBlankLines(lines, true) :
				SemanticMerge::SqueezeNonBlankLines(lines);
		};
		if (EnclosingTagName(tags[nMine], mineTag) != EnclosingTagName(tags[nTheirs], theirsTag) ||
			definitionText(nMine, mineTag) != definitionText(nTheirs, theirsTag))
			continue;
		const int nMineDiff = additionDiff(nMine, mineTag);
		const int nTheirsDiff = additionDiff(nTheirs, theirsTag);
		if (nMineDiff == -1 || nTheirsDiff == -1 || nMineDiff == nTheirsDiff)
			continue;

		const DIFFRANGE* pMine = m_diffList.DiffRangeAt(nMineDiff);
		const DIFFRANGE* pTheirs = m_diffList.DiffRangeAt(nTheirsDiff);
		const std::vector<String> mineLines = paneLines(nMine, pMine->dbegin, pMine->dend);
		const std::vector<String> theirsLines = paneLines(nTheirs, pTheirs->dbegin, pTheirs->dend);
		SemanticMerge::DuplicateDrop drop = SemanticMerge::ChooseDuplicateAdditionToDrop(mineLines, theirsLines);
		// Leaving a copy out puts the base's lines there. A side that put its
		// copy in place of base lines removed those, so its copy is not the
		// one to leave out; the other side's is, when it holds nothing more.
		auto replacesBaseLines = [&](const DIFFRANGE* pdi)
		{
			return !SemanticMerge::SqueezeNonBlankLines(paneLines(panes.nBasePane, pdi->dbegin, pdi->dend)).empty();
		};
		if (drop == SemanticMerge::DuplicateDrop::Theirs && replacesBaseLines(pTheirs))
		{
			const bool bMineHasNothingMore =
				SemanticMerge::ChooseDuplicateAdditionToDrop(theirsLines, mineLines) == SemanticMerge::DuplicateDrop::Theirs;
			drop = bMineHasNothingMore ? SemanticMerge::DuplicateDrop::Mine : SemanticMerge::DuplicateDrop::None;
		}
		if (drop == SemanticMerge::DuplicateDrop::Mine && replacesBaseLines(pMine))
			drop = SemanticMerge::DuplicateDrop::None;
		if (drop == SemanticMerge::DuplicateDrop::None)
			continue;
		const bool bDropMine = drop == SemanticMerge::DuplicateDrop::Mine;
		const int nDropDiff = bDropMine ? nMineDiff : nTheirsDiff;
		const DIFFRANGE* pDrop = bDropMine ? pMine : pTheirs;

		// G4: the dropping side's file without its copy
		if (!IsSemanticResultSyntaxOk(bDropMine ? nMine : nTheirs, pDrop->dbegin, pDrop->dend, String()))
			continue;

		const SemanticResolutionSnapshot before = BeginSemanticResolution();
		ResultChooseSource(nDropDiff, panes.nBasePane, bGroupWithPrevious);
		bGroupWithPrevious = true;
		EndSemanticResolution(before, SemanticRule::DuplicateAddition,
			strutils::format_string3(_("Duplicate addition of %1: %2 and %3 both added it; kept once"),
				ucr::toTString(tag.name), ResultPaneLabel(nMine), ResultPaneLabel(nTheirs)));
		++nResolved;
	}
	return nResolved;
}
