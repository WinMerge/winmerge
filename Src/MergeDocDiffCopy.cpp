/**
 * @file  MergeDocDiffCopy.cpp
 *
 * @brief The functions to copy the selected differences to the specified pane.
 *
 */

#include "StdAfx.h"
#include "MergeDoc.h"
#include "UnicodeString.h"
#include "DiffTextBuffer.h"
#include "MergeEditView.h"
#include "DiffList.h"
#include "MergeLineFlags.h"
#include "MergeFrameCommon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

 /**
  * @brief An instance of RescanSuppress prevents rescan during its lifetime
  * (or until its Clear method is called, which ends its effect).
  */
class RescanSuppress
{
public:
	explicit RescanSuppress(CMergeDoc& doc) : m_doc(doc)
	{
		m_bSuppress = true;
		m_bPrev = doc.m_bEnableRescan;
		m_doc.m_bEnableRescan = false;
	}
	void Clear()
	{
		if (m_bSuppress)
		{
			m_bSuppress = false;
			m_doc.m_bEnableRescan = m_bPrev;
		}
	}
	~RescanSuppress()
	{
		Clear();
	}
private:
	CMergeDoc& m_doc;
	bool m_bPrev;
	bool m_bSuppress;
};

/**
 * @brief Copy all diffs from one side to side.
 * @param [in] srcPane Source side from which diff is copied
 * @param [in] dstPane Destination side
 */
void CMergeDoc::CopyAllList(int srcPane, int dstPane)
{
	CopyMultipleList(srcPane, dstPane, 0, m_diffList.GetSize() - 1);
}

/**
 * @brief Copy range of diffs from one side to side.
 * This function copies given range of differences from side to another.
 * Ignored differences are skipped, and not copied.
 * @param [in] srcPane Source side from which diff is copied
 * @param [in] dstPane Destination side
 * @param [in] firstDiff First diff copied (0-based index)
 * @param [in] lastDiff Last diff copied (0-based index)
 */
void CMergeDoc::CopyMultipleList(int srcPane, int dstPane, int firstDiff, int lastDiff, int firstWordDiff, int lastWordDiff)
{
#ifdef _DEBUG
	if (firstDiff > lastDiff)
		_RPTF0(_CRT_ERROR, "Invalid diff range (firstDiff > lastDiff)!");
	if (firstDiff < 0)
		_RPTF0(_CRT_ERROR, "Invalid diff range (firstDiff < 0)!");
	if (lastDiff > m_diffList.GetSize() - 1)
		_RPTF0(_CRT_ERROR, "Invalid diff range (lastDiff < diffcount)!");
#endif

	lastDiff = (std::min)(m_diffList.GetSize() - 1, lastDiff);
	firstDiff = (std::max)(0, firstDiff);
	if (firstDiff > lastDiff)
		return;

	RescanSuppress suppressRescan(*this);

	// Note we don't care about m_nDiffs count to become zero,
	// because we don't rescan() so it does not change

	SetCurrentDiff(lastDiff);

	bool bGroupWithPrevious = false;
	if (firstWordDiff <= 0 && lastWordDiff == -1)
	{
		if (!ListCopy(srcPane, dstPane, -1, bGroupWithPrevious, true))
			return; // sync failure
	}
	else
	{
		if (!InlineDiffListCopy(srcPane, dstPane, lastDiff,
			(firstDiff == lastDiff) ? firstWordDiff : 0, lastWordDiff, nullptr, bGroupWithPrevious, true))
			return; // sync failure
	}

	SetEditedAfterRescan(dstPane);

	int nGroup = GetActiveMergeView()->m_nThisGroup;
	CMergeEditView* pViewDst = m_pView[nGroup][dstPane];
	CEPoint currentPosDst{ 0, pViewDst->GetCursorPos().y };

	CEPoint pt(0, 0);
	pViewDst->SetCursorPos(pt);
	pViewDst->SetNewSelection(pt, pt, false);
	pViewDst->SetNewAnchor(pt);

	// copy from bottom up is more efficient
	for (int i = lastDiff - 1; i >= firstDiff; --i)
	{
		if (m_diffList.IsDiffSignificant(i))
		{
			SetCurrentDiff(i);
			const DIFFRANGE* pdi = m_diffList.DiffRangeAt(i);
			if (currentPosDst.y > pdi->dend)
			{
				if (pdi->blank[dstPane] >= 0)
					currentPosDst.y -= pdi->dend - pdi->blank[dstPane] + 1;
				else if (pdi->blank[srcPane] >= 0)
					currentPosDst.y -= pdi->dend - pdi->blank[srcPane] + 1;
			}
			// Group merge with previous (merge undo data to one action)
			bGroupWithPrevious = true;
			if (i > firstDiff || firstWordDiff <= 0)
			{
				if (!ListCopy(srcPane, dstPane, -1, bGroupWithPrevious, false))
					break; // sync failure
			}
			else
			{
				if (!InlineDiffListCopy(srcPane, dstPane, firstDiff, firstWordDiff, -1, nullptr, bGroupWithPrevious, false))
					break; // sync failure
			}
		}
	}

	ForEachView(dstPane, [currentPosDst](auto& pView) {
		pView->SetCursorPos(currentPosDst);
		pView->SetNewSelection(currentPosDst, currentPosDst, false);
		pView->SetNewAnchor(currentPosDst);
		});

	suppressRescan.Clear(); // done suppress Rescan
	FlushAndRescan();
}

void CMergeDoc::CopyMultiplePartialList(int srcPane, int dstPane, int activePane,
	int firstDiff, int lastDiff, const CEPoint& ptStart, const CEPoint& ptEnd, bool bCharacter)
{
	lastDiff = (std::min)(m_diffList.GetSize() - 1, lastDiff);
	firstDiff = (std::max)(0, firstDiff);
	if (firstDiff > lastDiff)
		return;
	const int firstLineDiff = ptStart.y;
	const int lastLineDiff = ptEnd.x == 0 ? ptEnd.y - 1 : ptEnd.y;

	RescanSuppress suppressRescan(*this);

	bool bGroupWithPrevious = false;
	if (firstLineDiff <= 0 && lastLineDiff == -1)
	{
		if (!ListCopy(srcPane, dstPane, -1, bGroupWithPrevious, true))
			return; // sync failure
	}
	else
	{
		if (!bCharacter)
		{
			if (!LineListCopy(srcPane, dstPane, lastDiff,
				(firstDiff == lastDiff) ? firstLineDiff : 0, lastLineDiff, bGroupWithPrevious, true))
				return; // sync failure
		}
		else
		{
			if (!CharacterListCopy(srcPane, dstPane, activePane, lastDiff,
				ptStart, ptEnd, bGroupWithPrevious, true))
				return; // sync failure
		}
	}

	SetEditedAfterRescan(dstPane);

	int nGroup = GetActiveMergeView()->m_nThisGroup;
	CMergeEditView* pViewDst = m_pView[nGroup][dstPane];
	CEPoint currentPosDst{ 0, pViewDst->GetCursorPos().y };

	CEPoint pt(0, 0);
	pViewDst->SetCursorPos(pt);
	pViewDst->SetNewSelection(pt, pt, false);
	pViewDst->SetNewAnchor(pt);

	// copy from bottom up is more efficient
	for (int i = lastDiff - 1; i >= firstDiff; --i)
	{
		if (m_diffList.IsDiffSignificant(i))
		{
			SetCurrentDiff(i);
			const DIFFRANGE* pdi = m_diffList.DiffRangeAt(i);
			if (currentPosDst.y > pdi->dend)
			{
				if (pdi->blank[dstPane] >= 0)
					currentPosDst.y -= pdi->dend - pdi->blank[dstPane] + 1;
				else if (pdi->blank[srcPane] >= 0)
					currentPosDst.y -= pdi->dend - pdi->blank[srcPane] + 1;
			}
			// Group merge with previous (merge undo data to one action)
			bGroupWithPrevious = true;
			if (i > firstDiff || firstLineDiff <= 0)
			{
				if (!ListCopy(srcPane, dstPane, -1, bGroupWithPrevious, false))
					break; // sync failure
			}
			else
			{
				if (!bCharacter)
				{
					if (!LineListCopy(srcPane, dstPane, firstDiff, firstLineDiff, -1, bGroupWithPrevious, false))
						break; // sync failure
				}
				else
				{
					CEPoint ptEndAjusted = CEPoint(0, pdi->dend + 1);
					if (!CharacterListCopy(srcPane, dstPane, activePane, firstDiff, ptStart, ptEndAjusted, bGroupWithPrevious, false))
						break; // sync failure
				}
			}
		}
	}

	ForEachView(dstPane, [currentPosDst](auto& pView) {
		pView->SetCursorPos(currentPosDst);
		pView->SetNewSelection(currentPosDst, currentPosDst, false);
		pView->SetNewAnchor(currentPosDst);
		});

	suppressRescan.Clear(); // done suppress Rescan
	FlushAndRescan();
}

enum MergeResult { NoMergeNeeded, Merged, Conflict };

template<class Type>
static std::pair<MergeResult, Type> DoMergeValue(Type left, Type middle, Type right, int dstPane)
{
	bool equal_all = middle == left && middle == right && left == right;
	if (equal_all)
		return std::pair<MergeResult, Type>(NoMergeNeeded, left);
	bool conflict = middle != left && middle != right && left != right;
	if (conflict)
		return std::pair<MergeResult, Type>(Conflict, left);
	switch (dstPane)
	{
	case 0:
		if (left == right)
			return std::pair<MergeResult, Type>(Merged, middle);
		break;
	case 1:
		if (left == middle)
			return std::pair<MergeResult, Type>(Merged, right);
		else
			return std::pair<MergeResult, Type>(Merged, left);
		break;
	case 2:
		if (left == right)
			return std::pair<MergeResult, Type>(Merged, middle);
		break;
	}
	return std::pair<MergeResult, Type>(NoMergeNeeded, left);
}

/**
 * @brief Do auto-merge.
 * @param [in] dstPane Destination side
 */
void CMergeDoc::DoAutoMerge(int dstPane)
{
	if (m_nBuffers < 3)
		return;
	const int lastDiff = m_diffList.GetSize() - 1;
	const int firstDiff = 0;
	bool bGroupWithPrevious = false;
	int autoMergedCount = 0;
	int unresolvedConflictCount = 0;

	std::pair<MergeResult, FileTextEncoding> mergedEncoding =
		DoMergeValue(m_ptBuf[0]->getEncoding(), m_ptBuf[1]->getEncoding(), m_ptBuf[2]->getEncoding(), dstPane);
	if (mergedEncoding.first == Merged)
	{
		ShowMessageBox(_("Codepage change merged."), MB_ICONINFORMATION);
		m_ptBuf[dstPane]->setEncoding(mergedEncoding.second);
	}
	else if (mergedEncoding.first == Conflict)
		ShowMessageBox(_("Codepage changes are conflicting."), MB_ICONINFORMATION);

	std::pair<MergeResult, CRLFSTYLE> mergedEOLStyle =
		DoMergeValue(m_ptBuf[0]->GetCRLFMode(), m_ptBuf[1]->GetCRLFMode(), m_ptBuf[2]->GetCRLFMode(), dstPane);
	if (mergedEOLStyle.first == Merged)
	{
		ShowMessageBox(_("EOL change merged."), MB_ICONINFORMATION);
		m_ptBuf[dstPane]->SetCRLFMode(mergedEOLStyle.second);
	}
	else if (mergedEOLStyle.first == Conflict)
		ShowMessageBox(_("EOL changes are conflicting."), MB_ICONINFORMATION);

	RescanSuppress suppressRescan(*this);

	// Note we don't care about m_nDiffs count to become zero,
	// because we don't rescan() so it does not change

	SetCurrentDiff(lastDiff);

	SetEditedAfterRescan(dstPane);

	int nGroup = GetActiveMergeView()->m_nThisGroup;
	CMergeEditView* pViewDst = m_pView[nGroup][dstPane];
	CEPoint currentPosDst = pViewDst->GetCursorPos();
	currentPosDst.x = 0;

	CEPoint pt(0, 0);
	pViewDst->SetCursorPos(pt);
	pViewDst->SetNewSelection(pt, pt, false);
	pViewDst->SetNewAnchor(pt);

	// copy from bottom up is more efficient
	for (int i = lastDiff; i >= firstDiff; --i)
	{
		const DIFFRANGE* pdi = m_diffList.DiffRangeAt(i);
		const int srcPane = m_diffList.GetMergeableSrcIndex(i, dstPane);
		if (srcPane != -1)
		{
			SetCurrentDiff(i);
			if (currentPosDst.y > pdi->dend)
			{
				if (pdi->blank[dstPane] >= 0)
					currentPosDst.y -= pdi->dend - pdi->blank[dstPane] + 1;
				else if (pdi->blank[srcPane] >= 0)
					currentPosDst.y -= pdi->dend - pdi->blank[srcPane] + 1;
			}
			// Group merge with previous (merge undo data to one action)
			if (!ListCopy(srcPane, dstPane, -1, bGroupWithPrevious, false))
				break; // sync failure
			if (!bGroupWithPrevious)
				bGroupWithPrevious = true;
			++autoMergedCount;
		}
		if (pdi->op == OP_DIFF)
			++unresolvedConflictCount;
	}

	ForEachView(dstPane, [currentPosDst](auto& pView) {
		pView->SetCursorPos(currentPosDst);
		pView->SetNewSelection(currentPosDst, currentPosDst, false);
		pView->SetNewAnchor(currentPosDst);
		});

	suppressRescan.Clear(); // done suppress Rescan
	FlushAndRescan();
	UpdateHeaderPath(dstPane);

	if (autoMergedCount > 0)
		m_bAutoMerged = true;

	// move to first conflict 
	const int nDiff = m_diffList.FirstSignificant3wayDiff(THREEWAYDIFFTYPE_CONFLICT);
	if (nDiff != -1)
		pViewDst->SelectDiff(nDiff, true, false);

	ShowMessageBox(
		strutils::format_string2(
			_("Automatic merges: %1\nUnresolved conflicts: %2"),
			strutils::format(_T("%d"), autoMergedCount),
			strutils::format(_T("%d"), unresolvedConflictCount)),
		MB_ICONINFORMATION);
}

/**
 * @brief Sanity check difference.
 *
 * Checks that lines in difference are inside difference in both files.
 * If file is edited, lines added or removed diff lines get out of sync and
 * merging fails miserably.
 *
 * @param [in] dr Difference to check.
 * @return true if difference lines match, false otherwise.
 */
bool CMergeDoc::SanityCheckDiff(const DIFFRANGE& dr) const
{
	const int cd_dbegin = dr.dbegin;
	const int cd_dend = dr.dend;

	for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
	{
		// Must ensure line number is in range before getting line flags
		if (cd_dend >= m_ptBuf[nBuffer]->GetLineCount())
			return false;

		// Optimization - check last line first so we don't need to
		// check whole diff for obvious cases
		lineflags_t dwFlags = m_ptBuf[nBuffer]->GetLineFlags(cd_dend);
		if (!(dwFlags & LF_WINMERGE_FLAGS))
			return false;
	}

	for (int line = cd_dbegin; line < cd_dend; line++)
	{
		for (int nBuffer = 0; nBuffer < m_nBuffers; nBuffer++)
		{
			lineflags_t dwFlags = m_ptBuf[nBuffer]->GetLineFlags(cd_dend);
			if (!(dwFlags & LF_WINMERGE_FLAGS))
				return false;
		}
	}
	return true;
}

bool CMergeDoc::TransformText(String& text)
{ 
	if (m_editorScriptInfo.GetPluginPipeline().empty())
		return false;
	const auto pwndActiveWindow = GetActiveMergeView();
	if (!pwndActiveWindow)
		return false;
	const int nActivePane = pwndActiveWindow->m_nThisPane;
	bool bChanged = false;
	m_editorScriptInfo.TransformText(nActivePane, text, { m_filePaths[nActivePane] }, bChanged);
	return bChanged;
}

/// Replace line (removing any eol, and only including one if in strText)
void CMergeDoc::ReplaceFullLines(CDiffTextBuffer& dbuf, CDiffTextBuffer& sbuf, CCrystalTextView * pSource, int nLineBegin, int nLineEnd, int nAction /*=CE_ACTION_UNKNOWN*/)
{
	String strText;
	if (nLineBegin != nLineEnd || sbuf.GetLineLength(nLineEnd) > 0)
		sbuf.GetTextWithoutEmptys(nLineBegin, 0, nLineEnd, sbuf.GetLineLength(nLineEnd), strText);
	strText += sbuf.GetLineEol(nLineEnd);

	if (nLineBegin != nLineEnd || dbuf.GetFullLineLength(nLineEnd) > 0)
	{
		int nLineEndSource = nLineEnd < dbuf.GetLineCount() ? nLineEnd : dbuf.GetLineCount();
		if (nLineEnd+1 < dbuf.GetLineCount())
			dbuf.DeleteText(pSource, nLineBegin, 0, nLineEndSource + 1, 0, nAction);
		else
			dbuf.DeleteText(pSource, nLineBegin, 0, nLineEndSource, dbuf.GetLineLength(nLineEndSource), nAction); 
	}

	TransformText(strText);
	if (size_t cchText = strText.length())
	{
		int endl,endc;
		dbuf.InsertText(pSource, nLineBegin, 0, strText.c_str(), cchText, endl, endc, nAction);
	}
}

 /**
  * @brief Copy selected (=current) difference from from side to side.
  * @param [in] srcPane Source side from which diff is copied
  * @param [in] dstPane Destination side
  * @param [in] nDiff Diff to copy, if -1 function determines it.
  * @param [in] bGroupWithPrevious Adds diff to same undo group with
  * @return true if ok, false if sync failure & need to abort copy
  * previous action (allows one undo for copy all)
  */
bool CMergeDoc::ListCopy(int srcPane, int dstPane, int nDiff /* = -1*/,
	bool bGroupWithPrevious /*= false*/, bool bUpdateView /*= true*/)
{
	int nGroup = GetActiveMergeView()->m_nThisGroup;
	CMergeEditView* pViewSrc = m_pView[nGroup][srcPane];
	CMergeEditView* pViewDst = m_pView[nGroup][dstPane];
	CCrystalTextView* pSource = bUpdateView ? pViewDst : nullptr;

	// suppress Rescan during this method
	// (Not only do we not want to rescan a lot of times, but
	// it will wreck the line status array to rescan as we merge)
	RescanSuppress suppressRescan(*this);

	// If diff-number not given, determine it from active view
	if (nDiff == -1)
	{
		nDiff = GetCurrentDiff();

		// No current diff, but maybe cursor is in diff?
		if (nDiff == -1 && (pViewSrc->IsCursorInDiff() ||
			pViewDst->IsCursorInDiff()))
		{
			// Find out diff under cursor
			CEPoint ptCursor = GetActiveMergeView()->GetCursorPos();
			nDiff = m_diffList.LineToDiff(ptCursor.y);
		}
	}

	if (nDiff != -1)
	{
		DIFFRANGE cd;
		VERIFY(m_diffList.GetDiff(nDiff, cd));
		CDiffTextBuffer& sbuf = *m_ptBuf[srcPane];
		CDiffTextBuffer& dbuf = *m_ptBuf[dstPane];
		bool bSrcWasMod = sbuf.IsModified();
		const int cd_dbegin = cd.dbegin;
		const int cd_dend = cd.dend;
		const int cd_blank = cd.blank[srcPane];
		bool bInSync = SanityCheckDiff(cd);

		if (!bInSync)
		{
			I18n::MessageBox(IDS_VIEWS_OUTOFSYNC, MB_ICONSTOP);
			return false; // abort copying
		}

		// If we remove whole diff from current view, we must fix cursor
		// position first. Normally we would move to end of previous line,
		// but we want to move to begin of that line for usability.
		if (bUpdateView)
		{
			CEPoint currentPos = pViewDst->GetCursorPos();
			currentPos.x = 0;
			if (currentPos.y > cd_dend)
			{
				if (cd.blank[dstPane] >= 0)
					currentPos.y -= cd_dend - cd.blank[dstPane] + 1;
				else if (cd.blank[srcPane] >= 0)
					currentPos.y -= cd_dend - cd.blank[srcPane] + 1;
			}
			ForEachView(dstPane, [currentPos](auto& pView) { pView->SetCursorPos(currentPos); });
		}

		// if the current diff contains missing lines, remove them from both sides
		int limit = cd_dend;

		// curView is the view which is changed, so the opposite of the source view
		dbuf.BeginUndoGroup(bGroupWithPrevious);
		if (cd_blank >= 0)
		{
			// text was missing, so delete rest of lines on both sides
			// delete only on destination side since rescan will clear the other side
			if (cd_dend + 1 < dbuf.GetLineCount())
			{
				dbuf.DeleteText(pSource, cd_blank, 0, cd_dend + 1, 0, CE_ACTION_MERGE);
			}
			else
			{
				// To removing EOL chars of last line, deletes from the end of the line (cd_blank - 1).
				ASSERT(cd_blank > 0);
				dbuf.DeleteText(pSource, cd_blank - 1, dbuf.GetLineLength(cd_blank - 1), cd_dend, dbuf.GetLineLength(cd_dend), CE_ACTION_MERGE);
			}

			limit = cd_blank - 1;
			dbuf.FlushUndoGroup(pSource);
			dbuf.BeginUndoGroup(true);
		}


		// copy the selected text over
		if (cd_dbegin <= limit)
		{
			// text exists on left side, so just replace
			ReplaceFullLines(dbuf, sbuf, pSource, cd_dbegin, limit, CE_ACTION_MERGE);
			dbuf.FlushUndoGroup(pSource);
			dbuf.BeginUndoGroup(true);
		}
		dbuf.FlushUndoGroup(pSource);

		// remove the diff
		SetCurrentDiff(-1);

		// reset the mod status of the source view because we do make some
		// changes, but none that concern the source text
		sbuf.SetModified(bSrcWasMod);

		CMergeFrameCommon::LogCopyDiff(srcPane, dstPane, nDiff);
	}

	suppressRescan.Clear(); // done suppress Rescan
	FlushAndRescan();
	return true;
}

bool CMergeDoc::LineListCopy(int srcPane, int dstPane, int nDiff, int firstLine, int lastLine /*= -1*/,
	bool bGroupWithPrevious /*= false*/, bool bUpdateView /*= true*/)
{
	int nGroup = GetActiveMergeView()->m_nThisGroup;
	CMergeEditView* pViewDst = m_pView[nGroup][dstPane];
	CCrystalTextView* pSource = bUpdateView ? pViewDst : nullptr;

	// suppress Rescan during this method
	// (Not only do we not want to rescan a lot of times, but
	// it will wreck the line status array to rescan as we merge)
	RescanSuppress suppressRescan(*this);

	DIFFRANGE cd;
	VERIFY(m_diffList.GetDiff(nDiff, cd));
	CDiffTextBuffer& sbuf = *m_ptBuf[srcPane];
	CDiffTextBuffer& dbuf = *m_ptBuf[dstPane];
	bool bSrcWasMod = sbuf.IsModified();
	const int cd_dbegin = (firstLine > cd.dbegin) ? firstLine : cd.dbegin;
	const int cd_dend = cd.dend;
	const int cd_blank = cd.blank[srcPane];
	bool bInSync = SanityCheckDiff(cd);

	if (!bInSync)
	{
		I18n::MessageBox(IDS_VIEWS_OUTOFSYNC, MB_ICONSTOP);
		return false; // abort copying
	}

	// If we remove whole diff from current view, we must fix cursor
	// position first. Normally we would move to end of previous line,
	// but we want to move to begin of that line for usability.
	if (bUpdateView)
	{
		CEPoint currentPos = pViewDst->GetCursorPos();
		currentPos.x = 0;
		if (currentPos.y > cd_dend)
		{
			if (cd.blank[dstPane] >= 0)
				currentPos.y -= cd_dend - cd.blank[dstPane] + 1;
			else if (cd.blank[srcPane] >= 0)
				currentPos.y -= cd_dend - cd.blank[srcPane] + 1;
		}
		ForEachView(dstPane, [currentPos](auto& pView) { pView->SetCursorPos(currentPos); });
	}

	// if the current diff contains missing lines, remove them from both sides
	int limit = ((lastLine < 0) || (lastLine > cd_dend)) ? cd_dend : lastLine;

	// curView is the view which is changed, so the opposite of the source view
	dbuf.BeginUndoGroup(bGroupWithPrevious);
	if ((cd_blank >= 0) && (cd_dbegin >= cd_blank))
	{
		// text was missing, so delete rest of lines on both sides
		// delete only on destination side since rescan will clear the other side
		if (limit + 1 < dbuf.GetLineCount())
		{
			dbuf.DeleteText(pSource, cd_dbegin, 0, limit + 1, 0, CE_ACTION_MERGE);
		}
		else
		{
			// To removing EOL chars of last line, deletes from the end of the line (cd_blank - 1).
			ASSERT(cd_dbegin > 0);
			dbuf.DeleteText(pSource, cd_dbegin - 1, dbuf.GetLineLength(cd_dbegin - 1), limit, dbuf.GetLineLength(limit), CE_ACTION_MERGE);
		}

		limit = cd_dbegin - 1;
		dbuf.FlushUndoGroup(pSource);
		dbuf.BeginUndoGroup(true);
	}

	// copy the selected text over
	if (cd_dbegin <= limit)
	{
		// text exists on left side, so just replace
		ReplaceFullLines(dbuf, sbuf, pSource, cd_dbegin, limit, CE_ACTION_MERGE);
		dbuf.FlushUndoGroup(pSource);
		dbuf.BeginUndoGroup(true);
	}
	dbuf.FlushUndoGroup(pSource);

	// remove the diff
	SetCurrentDiff(-1);

	// reset the mod status of the source view because we do make some
	// changes, but none that concern the source text
	sbuf.SetModified(bSrcWasMod);

	CMergeFrameCommon::LogCopyLines(srcPane, dstPane, firstLine, lastLine);

	suppressRescan.Clear(); // done suppress Rescan
	FlushAndRescan();
	return true;
}

bool CMergeDoc::InlineDiffListCopy(int srcPane, int dstPane, int nDiff, int firstWordDiff, int lastWordDiff,
	const std::vector<int>* pWordDiffIndice, bool bGroupWithPrevious /*= false*/, bool bUpdateView /*= true*/)
{
	int nGroup = GetActiveMergeView()->m_nThisGroup;
	CMergeEditView* pViewDst = m_pView[nGroup][dstPane];
	CCrystalTextView* pSource = bUpdateView ? pViewDst : nullptr;

	// suppress Rescan during this method
	// (Not only do we not want to rescan a lot of times, but
	// it will wreck the line status array to rescan as we merge)
	RescanSuppress suppressRescan(*this);

	DIFFRANGE cd;
	VERIFY(m_diffList.GetDiff(nDiff, cd));
	CDiffTextBuffer& sbuf = *m_ptBuf[srcPane];
	CDiffTextBuffer& dbuf = *m_ptBuf[dstPane];
	bool bSrcWasMod = sbuf.IsModified();
	const int cd_dbegin = cd.dbegin;
	const int cd_dend = cd.dend;
	bool bInSync = SanityCheckDiff(cd);

	if (!bInSync)
	{
		I18n::MessageBox(IDS_VIEWS_OUTOFSYNC, MB_ICONSTOP);
		return false; // abort copying
	}

	std::vector<WordDiff> worddiffs = GetWordDiffArrayInDiffBlock(nDiff);

	if (worddiffs.empty())
		return false;

	if (cd.end[srcPane] < cd.begin[srcPane])
		return ListCopy(srcPane, dstPane, nDiff, bGroupWithPrevious, bUpdateView);

	if (firstWordDiff == -1)
		firstWordDiff = 0;
	if (lastWordDiff == -1)
		lastWordDiff = static_cast<int>(worddiffs.size() - 1);

	// If we remove whole diff from current view, we must fix cursor
	// position first. Normally we would move to end of previous line,
	// but we want to move to begin of that line for usability.
	if (bUpdateView)
	{
		CEPoint currentPos = pViewDst->GetCursorPos();
		currentPos.x = 0;
		if (currentPos.y > cd_dend)
		{
			if (cd.blank[dstPane] >= 0)
				currentPos.y -= cd_dend - cd.blank[dstPane] + 1;
			else if (cd.blank[srcPane] >= 0)
				currentPos.y -= cd_dend - cd.blank[srcPane] + 1;
		}
		ForEachView(dstPane, [currentPos](auto& pView) { pView->SetCursorPos(currentPos); });
	}

	// curView is the view which is changed, so the opposite of the source view
	dbuf.BeginUndoGroup(bGroupWithPrevious);

	String srcText, dstText;
	CEPoint ptDstStart, ptDstEnd;
	CEPoint ptSrcStart, ptSrcEnd;

	ptDstStart.x = worddiffs[firstWordDiff].begin[dstPane];
	ptDstStart.y = worddiffs[firstWordDiff].beginline[dstPane];
	ptDstEnd.x = worddiffs[lastWordDiff].end[dstPane];
	ptDstEnd.y = worddiffs[lastWordDiff].endline[dstPane];
	ptSrcStart.x = worddiffs[firstWordDiff].begin[srcPane];
	ptSrcStart.y = worddiffs[firstWordDiff].beginline[srcPane];
	ptSrcEnd.x = worddiffs[lastWordDiff].end[srcPane];
	ptSrcEnd.y = worddiffs[lastWordDiff].endline[srcPane];

	std::vector<int> nDstOffsets(ptDstEnd.y - ptDstStart.y + 2);
	std::vector<int> nSrcOffsets(ptSrcEnd.y - ptSrcStart.y + 2);

	dbuf.GetTextWithoutEmptys(ptDstStart.y, ptDstStart.x, ptDstEnd.y, ptDstEnd.x, dstText);
	sbuf.GetTextWithoutEmptys(ptSrcStart.y, ptSrcStart.x, ptSrcEnd.y, ptSrcEnd.x, srcText);

	nDstOffsets[0] = 0;
	for (int nLine = ptDstStart.y; nLine <= ptDstEnd.y; nLine++)
		nDstOffsets[nLine - ptDstStart.y + 1] = nDstOffsets[nLine - ptDstStart.y] + dbuf.GetFullLineLength(nLine);
	nSrcOffsets[0] = 0;
	for (int nLine = ptSrcStart.y; nLine <= ptSrcEnd.y; nLine++)
		nSrcOffsets[nLine - ptSrcStart.y + 1] = nSrcOffsets[nLine - ptSrcStart.y] + sbuf.GetFullLineLength(nLine);

	for (int i = lastWordDiff; i != firstWordDiff - 1; --i)
	{
		if (pWordDiffIndice && std::find(pWordDiffIndice->begin(), pWordDiffIndice->end(), i) == pWordDiffIndice->end())
			continue;
		int srcBegin = nSrcOffsets[worddiffs[i].beginline[srcPane] - ptSrcStart.y] + worddiffs[i].begin[srcPane];
		int srcEnd = nSrcOffsets[worddiffs[i].endline[srcPane] - ptSrcStart.y] + worddiffs[i].end[srcPane];
		int dstBegin = nDstOffsets[worddiffs[i].beginline[dstPane] - ptDstStart.y] + worddiffs[i].begin[dstPane];
		int dstEnd = nDstOffsets[worddiffs[i].endline[dstPane] - ptDstStart.y] + worddiffs[i].end[dstPane];
		dstText = dstText.substr(0, dstBegin - ptDstStart.x)
			+ srcText.substr(srcBegin - ptSrcStart.x, srcEnd - srcBegin)
			+ dstText.substr(dstEnd - ptDstStart.x);
	}

	dbuf.DeleteText(pSource, ptDstStart.y, ptDstStart.x, ptDstEnd.y, ptDstEnd.x, CE_ACTION_MERGE);

	TransformText(dstText);
	int endl, endc;
	dbuf.InsertText(pSource, ptDstStart.y, ptDstStart.x, dstText.c_str(), dstText.length(), endl, endc, CE_ACTION_MERGE);

	dbuf.FlushUndoGroup(pSource);

	// reset the mod status of the source view because we do make some
	// changes, but none that concern the source text
	sbuf.SetModified(bSrcWasMod);

	CMergeFrameCommon::LogCopyInlineDiffs(srcPane, dstPane, nDiff, firstWordDiff, lastWordDiff);

	suppressRescan.Clear(); // done suppress Rescan
	FlushAndRescan();

	return true;
}

static int GetDistance(const CDiffTextBuffer& buf, const CEPoint& pt1, const CEPoint& pt2)
{
	assert(pt1.y < pt2.y || (pt1.y == pt2.y && pt1.x <= pt2.x));
	int distance = 0;
	for (int y = pt1.y; y <= pt2.y; ++y)
	{
		distance += buf.GetFullLineLength(y);
		if (y == pt1.y)
			distance -= pt1.x;
		if (y == pt2.y)
			distance -= buf.GetFullLineLength(y) - pt2.x;
	}
	return distance;
};

static CEPoint Advance(const CDiffTextBuffer& buf, const CEPoint& pt, int distance)
{
	assert(distance >= 0);
	CEPoint ptMoved = pt;
	while (distance > 0)
	{
		if (buf.GetLineFlags(ptMoved.y) & LF_GHOST)
			break;
		else if (ptMoved.x + distance > buf.GetLineLength(ptMoved.y))
		{
			distance -= buf.GetFullLineLength(ptMoved.y) - ptMoved.x;
			ptMoved.x = 0;
			++ptMoved.y;
			const int nLineCount = buf.GetLineCount();
			if (ptMoved.y > nLineCount)
			{
				ptMoved.x = buf.GetLineLength(nLineCount - 1);
				ptMoved.y = nLineCount - 1;
				break;
			}
		}
		else
		{
			ptMoved.x += distance;
			distance = 0;
		}
	}
	return ptMoved;
}

std::tuple<CEPoint, CEPoint, CEPoint, CEPoint> CMergeDoc::GetCharacterRange(int srcPane, int dstPane, int activePane, int nDiff,
	const CEPoint& ptStart, const CEPoint& ptEnd)
{
	DIFFRANGE cd;
	VERIFY(m_diffList.GetDiff(nDiff, cd));
	const int cd_dbegin = cd.dbegin;
	const int cd_dend = cd.dend;

	std::vector<WordDiff> worddiffs = GetWordDiffArrayInDiffBlock(nDiff, true);

	int firstWordDiff = -1;
	const int nBeginLineFlag = m_ptBuf[activePane]->GetLineFlags(ptEnd.y);
	if ((nBeginLineFlag & LF_GHOST) == 0 && (nBeginLineFlag & LF_DIFF) != 0)
	{
		for (int i = 0; i < static_cast<int>(worddiffs.size()); ++i)
		{
			if ((ptStart.y == worddiffs[i].endline[activePane] && ptStart.x >= worddiffs[i].end[activePane]) ||
				(ptStart.y > worddiffs[i].endline[activePane]))
				firstWordDiff = i;
		}
	}
	int lastWordDiff = -1;
	const int nEndLineFlag = m_ptBuf[activePane]->GetLineFlags(ptEnd.y);
	if ((nEndLineFlag & LF_GHOST) == 0 && (nEndLineFlag & LF_DIFF) != 0)
	{
		for (int i = 0; i < static_cast<int>(worddiffs.size()); ++i)
		{
			if ((ptEnd.y == worddiffs[i].endline[activePane] && ptEnd.x >= worddiffs[i].end[activePane]) ||
				(ptEnd.y > worddiffs[i].endline[activePane]))
				lastWordDiff = i;
		}
	}

	CEPoint ptDstStart, ptDstEnd;
	CEPoint ptSrcStart, ptSrcEnd;

	if (firstWordDiff == -1)
	{
		if (ptStart.y < cd_dbegin)
		{
			ptSrcStart = { 0, cd_dbegin };
			ptDstStart = ptSrcStart;
		}
		else
		{
			ptSrcStart = ptStart;
			ptDstStart = ptStart;
			if (srcPane == activePane)
			{
				if (ptDstStart.x > m_ptBuf[dstPane]->GetLineLength(ptDstStart.y))
					ptDstStart.x = m_ptBuf[dstPane]->GetLineLength(ptDstStart.y);
			}
			else
			{
				if (ptSrcStart.x > m_ptBuf[dstPane]->GetLineLength(ptSrcStart.y))
					ptSrcStart.x = m_ptBuf[dstPane]->GetLineLength(ptSrcStart.y);
			}
		}
	}
	else
	{
		const auto& wdiffFirst = worddiffs[firstWordDiff];
		const int distanceBegin = GetDistance(*m_ptBuf[activePane], CEPoint{ wdiffFirst.begin[activePane], wdiffFirst.beginline[activePane] }, ptStart);
		const int distanceEnd = GetDistance(*m_ptBuf[activePane], CEPoint{ wdiffFirst.end[activePane], wdiffFirst.endline[activePane] }, ptStart);
		if (srcPane == activePane)
		{
			if (distanceBegin <= 0)
				ptDstStart = { wdiffFirst.begin[dstPane], wdiffFirst.beginline[dstPane] };
			else
				ptDstStart = Advance(*m_ptBuf[dstPane], CEPoint{ wdiffFirst.end[dstPane], wdiffFirst.endline[dstPane] }, distanceEnd);
			ptSrcStart = ptStart;
		}
		else
		{
			if (distanceBegin <= 0)
				ptSrcStart = { wdiffFirst.begin[srcPane], wdiffFirst.beginline[srcPane] };
			else
				ptSrcStart = Advance(*m_ptBuf[srcPane], CEPoint{ wdiffFirst.end[srcPane], wdiffFirst.endline[srcPane] }, distanceEnd);
			ptDstStart = ptStart;
		}
	}
	if (lastWordDiff == -1)
	{
		if (ptEnd.y > cd_dend)
		{
			ptDstEnd = { 0, cd_dend + 1 };
			ptSrcEnd = { 0, cd_dend + 1 };
		}
		else
		{
			ptDstEnd = ptEnd;
			ptSrcEnd = ptEnd;
			if (srcPane == activePane)
			{
				if (ptDstEnd.x > m_ptBuf[dstPane]->GetLineLength(ptDstEnd.y))
					ptDstEnd.x = m_ptBuf[dstPane]->GetLineLength(ptDstEnd.y);
			}
			else
			{
				if (ptDstEnd.x > m_ptBuf[dstPane]->GetLineLength(ptDstEnd.y))
					ptDstEnd.x = m_ptBuf[dstPane]->GetLineLength(ptDstEnd.y);
			}
		}
	}
	else
	{
		const auto& wdiffLast = worddiffs[lastWordDiff];
		const int distanceBegin = GetDistance(*m_ptBuf[activePane], CEPoint{ wdiffLast.begin[activePane], wdiffLast.beginline[activePane] }, ptEnd);
		const int distanceEnd = GetDistance(*m_ptBuf[activePane], CEPoint{ wdiffLast.end[activePane], wdiffLast.endline[activePane] }, ptEnd);
		if (srcPane == activePane)
		{
			if (distanceBegin <= 0)
				ptDstEnd = { wdiffLast.begin[dstPane], wdiffLast.beginline[dstPane] };
			else
				ptDstEnd = Advance(*m_ptBuf[dstPane], CEPoint{ wdiffLast.end[dstPane], wdiffLast.endline[dstPane] }, distanceEnd);
			ptSrcEnd = ptEnd;
		}
		else
		{
			if (distanceBegin <= 0)
				ptSrcEnd = { wdiffLast.begin[srcPane], wdiffLast.beginline[srcPane] };
			else
				ptSrcEnd = Advance(*m_ptBuf[srcPane], CEPoint{ wdiffLast.end[srcPane], wdiffLast.endline[srcPane] }, distanceEnd);
			ptDstEnd = ptEnd;
		}
	}
	if (ptDstStart.y > cd_dend)
	{
		ptDstStart = { 0, cd_dend + 1 };
	}
	if (ptSrcStart.y > cd_dend)
	{
		ptSrcStart = { 0, cd_dend + 1 };
	}
	if (ptDstEnd.y < ptDstStart.y)
		ptDstEnd.y = ptDstStart.y;
	if (ptSrcEnd.y < ptSrcStart.y)
		ptSrcEnd.y = ptSrcStart.y;
	if (ptDstEnd.y > cd_dend)
	{
		ptDstEnd = { 0, cd_dend + 1 };
	}
	if (ptSrcEnd.y > cd_dend)
	{
		ptSrcEnd = { 0, cd_dend + 1 };
	}

	return std::make_tuple(ptSrcStart, ptSrcEnd, ptDstStart, ptDstEnd);
}

bool CMergeDoc::CharacterListCopy(int srcPane, int dstPane, int activePane, int nDiff,
	const CEPoint& ptStart, const CEPoint& ptEnd, bool bGroupWithPrevious /*= false*/, bool bUpdateView /*= true*/)
{
	int nGroup = GetActiveMergeView()->m_nThisGroup;
	CMergeEditView* pViewDst = m_pView[nGroup][dstPane];
	CCrystalTextView* pSource = bUpdateView ? pViewDst : nullptr;

	// suppress Rescan during this method
	// (Not only do we not want to rescan a lot of times, but
	// it will wreck the line status array to rescan as we merge)
	RescanSuppress suppressRescan(*this);

	DIFFRANGE cd;
	VERIFY(m_diffList.GetDiff(nDiff, cd));
	CDiffTextBuffer& sbuf = *m_ptBuf[srcPane];
	CDiffTextBuffer& dbuf = *m_ptBuf[dstPane];
	bool bSrcWasMod = sbuf.IsModified();
	const int cd_dbegin = cd.dbegin;
	const int cd_dend = cd.dend;
	bool bInSync = SanityCheckDiff(cd);

	if (!bInSync)
	{
		I18n::MessageBox(IDS_VIEWS_OUTOFSYNC, MB_ICONSTOP);
		return false; // abort copying
	}

	// If we remove whole diff from current view, we must fix cursor
	// position first. Normally we would move to end of previous line,
	// but we want to move to begin of that line for usability.
	if (bUpdateView)
	{
		CEPoint currentPos = pViewDst->GetCursorPos();
		currentPos.x = 0;
		if (currentPos.y > cd_dend)
		{
			if (cd.blank[dstPane] >= 0)
				currentPos.y -= cd_dend - cd.blank[dstPane] + 1;
			else if (cd.blank[srcPane] >= 0)
				currentPos.y -= cd_dend - cd.blank[srcPane] + 1;
		}
		ForEachView(dstPane, [currentPos](auto& pView) { pView->SetCursorPos(currentPos); });
	}

	// curView is the view which is changed, so the opposite of the source view
	dbuf.BeginUndoGroup(bGroupWithPrevious);

	auto [ptSrcStart, ptSrcEnd, ptDstStart, ptDstEnd] =
		GetCharacterRange(srcPane, dstPane, activePane, nDiff, ptStart, ptEnd);

	String text;
	sbuf.GetTextWithoutEmptys(ptSrcStart.y, ptSrcStart.x, ptSrcEnd.y, ptSrcEnd.x, text);

	if (ptDstStart != ptDstEnd)
		dbuf.DeleteText(pSource, ptDstStart.y, ptDstStart.x, ptDstEnd.y, ptDstEnd.x, CE_ACTION_MERGE);

	TransformText(text);
	int endl, endc;
	dbuf.InsertText(pSource, ptDstStart.y, ptDstStart.x, text.c_str(), text.length(), endl, endc, CE_ACTION_MERGE);

	dbuf.FlushUndoGroup(pSource);

	// reset the mod status of the source view because we do make some
	// changes, but none that concern the source text
	sbuf.SetModified(bSrcWasMod);

	CMergeFrameCommon::LogCopyCharacters(srcPane, dstPane,  nDiff, ptStart, ptEnd);

	suppressRescan.Clear(); // done suppress Rescan
	FlushAndRescan();

	return true;
}

