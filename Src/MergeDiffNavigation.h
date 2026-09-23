/**
 * @file  MergeDiffNavigation.h
 * @brief Declaration of CMergeDiffNavigation class
 */
#pragma once

#include "MergeDoc.h"

class CCmdUI;

class CMergeDiffNavigation
{
public:
	/**
	 * @brief Find the first non-filtered diff
	 * @return Index of first non-filtered diff, or -1 if none found
	 */
	template <typename View>
	static int FindFirstNonFilteredDiff(View* pView)
	{
		const CMergeDoc *pd = pView->GetDocument();
		int nDiff = pd->m_diffList.FirstSignificantDiff();
		int lastDiff = pd->m_diffList.LastSignificantDiff();

		while (nDiff >= 0 && nDiff <= lastDiff && pView->IsDiffFiltered(nDiff))
		{
			if (nDiff < pd->m_diffList.GetSize() - 1)
				nDiff = pd->m_diffList.NextSignificantDiff(nDiff);
			else
				return -1;
		}

		return (nDiff >= 0 && nDiff <= lastDiff) ? nDiff : -1;
	}

	/**
	 * @brief Find the last non-filtered diff
	 * @return Index of last non-filtered diff, or -1 if none found
	 */
	template <typename View>
	static int FindLastNonFilteredDiff(View* pView)
	{
		const CMergeDoc *pd = pView->GetDocument();
		int nDiff = pd->m_diffList.LastSignificantDiff();
		int firstDiff = pd->m_diffList.FirstSignificantDiff();

		while (nDiff >= 0 && nDiff >= firstDiff && pView->IsDiffFiltered(nDiff))
		{
			if (nDiff > 0)
				nDiff = pd->m_diffList.PrevSignificantDiff(nDiff);
			else
				return -1;
		}

		return (nDiff >= 0 && nDiff >= firstDiff) ? nDiff : -1;
	}

	/**
	 * @brief Find next non-filtered diff from current position or given diff
	 * @param [in] startDiff Current diff index (-1 for search from cursor position)
	 * @return Index of next non-filtered diff, or -1 if none found
	 */
	template <typename View>
	static int FindNextNonFilteredDiff(View* pView, int startDiff)
	{
		const CMergeDoc *pd = pView->GetDocument();
		int nextDiff = -1;
		int lastDiff = pd->m_diffList.LastSignificantDiff();

		if (startDiff != -1)
		{
			// We're on a diff
			if (!pView->IsDiffVisible(startDiff))
			{
				// Selected difference not visible, select next from cursor
				int line = pView->GetCursorPos().y;
				// Make sure we aren't in the first line of the diff
				++line;
				if (!pView->IsValidTextPosY(CEPoint(0, line)))
					line = pView->m_nTopLine;
				nextDiff = pView->NextSignificantDiffFromLine(line);
			}
			else if (startDiff < pd->m_diffList.GetSize() - 1)
			{
				nextDiff = pd->m_diffList.NextSignificantDiff(startDiff);
			}
		}
		else
		{
			// No selected diff, search from cursor
			int line = pView->GetCursorPos().y;
			if (!pView->IsValidTextPosY(CEPoint(0, line)))
				line = pView->m_nTopLine;
			nextDiff = pView->NextSignificantDiffFromLine(line);
		}

		// Skip filtered (hidden) diffs
		while (nextDiff >= 0 && nextDiff <= lastDiff && pView->IsDiffFiltered(nextDiff))
		{
			if (nextDiff < pd->m_diffList.GetSize() - 1)
				nextDiff = pd->m_diffList.NextSignificantDiff(nextDiff);
			else
				return -1;
		}

		return (nextDiff >= 0 && nextDiff <= lastDiff) ? nextDiff : -1;
	}

	/**
	 * @brief Find previous non-filtered diff from current position or given diff
	 * @param [in] startDiff Current diff index (-1 for search from cursor position)
	 * @return Index of previous non-filtered diff, or -1 if none found
	 */
	template <typename View>
	static int FindPrevNonFilteredDiff(View* pView, int startDiff)
	{
		const CMergeDoc *pd = pView->GetDocument();
		int prevDiff = -1;
		int firstDiff = pd->m_diffList.FirstSignificantDiff();

		if (startDiff != -1)
		{
			// We're on a diff
			if (!pView->IsDiffVisible(startDiff))
			{
				// Selected difference not visible, select previous from cursor
				int line = pView->GetCursorPos().y;
				// Make sure we aren't in the last line of the diff
				--line;
				if (!pView->IsValidTextPosY(CEPoint(0, line)))
					line = pView->m_nTopLine;
				prevDiff = pView->PrevSignificantDiffFromLine(line);
			}
			else if (startDiff > 0)
			{
				prevDiff = pd->m_diffList.PrevSignificantDiff(startDiff);
			}
		}
		else
		{
			// No selected diff, search from cursor
			int line = pView->GetCursorPos().y;
			if (!pView->IsValidTextPosY(CEPoint(0, line)))
				line = pView->m_nTopLine;
			prevDiff = pView->PrevSignificantDiffFromLine(line);
		}

		// Skip filtered (hidden) diffs
		while (prevDiff >= 0 && prevDiff >= firstDiff && pView->IsDiffFiltered(prevDiff))
		{
			if (prevDiff > 0)
				prevDiff = pd->m_diffList.PrevSignificantDiff(prevDiff);
			else
				return -1;
		}

		return (prevDiff >= 0 && prevDiff >= firstDiff) ? prevDiff : -1;
	}

	/**
	 * @brief Check if there is a next non-filtered diff.
	 * @return true if there is a next non-filtered diff available.
	 */
	template <typename View>
	static bool HasNextNonFilteredDiff(View* pView)
	{
		const CMergeDoc *pd = pView->GetDocument();
		int curDiff = pd->GetCurrentDiff();
		return FindNextNonFilteredDiff(pView, curDiff) != -1;
	}

	/**
	 * @brief Check if there is a previous non-filtered diff.
	 * @return true if there is a previous non-filtered diff available.
	 */
	template <typename View>
	static bool HasPrevNonFilteredDiff(View* pView)
	{
		const CMergeDoc* pd = pView->GetDocument();
		int curDiff = pd->GetCurrentDiff();
		return FindPrevNonFilteredDiff(pView, curDiff) != -1;
	}

	/**
	 * @brief Find the next/previous difference that still needs a decision in
	 * the merge result pane (kdiff3's "unsolved conflict" navigation).
	 * @return Diff index, or -1 when there is none in that direction.
	 */
	template <typename View>
	static int FindPendingResultDiff(View* pView, bool bNext)
	{
		CMergeDoc* pd = pView->GetDocument();
		const int nDiffCount = pd->m_diffList.GetSize();
		const int nCurDiff = pd->GetCurrentDiff();
		int nBegin;
		if (nCurDiff != -1)
			nBegin = bNext ? nCurDiff + 1 : nCurDiff - 1;
		else
		{
			// No selected difference: anchor the scan on the cursor line
			// (the difference at the cursor counts as "next"/"previous")
			const int nLine = pView->GetCursorPos().y;
			nBegin = bNext ? pView->NextSignificantDiffFromLine(nLine) : pView->PrevSignificantDiffFromLine(nLine);
			if (nBegin == -1)
				nBegin = bNext ? nDiffCount : -1;
		}
		const int nStep = bNext ? 1 : -1;
		for (int i = nBegin; i >= 0 && i < nDiffCount; i += nStep)
		{
			if (pd->m_diffList.IsDiffSignificant(i) && !pView->IsDiffFiltered(i) &&
				pd->IsResultDiffPending(i))
				return i;
		}
		return -1;
	}

	/**
	 * @brief Called when user selects "Current Difference".
	 * Goes to active diff. If no active diff, selects diff under cursor
	 * @sa CMergeEditView::SelectDiff()
	 * @sa CMergeDoc::GetCurrentDiff()
	 * @sa CMergeDoc::LineToDiff()
	 */
	template <typename View>
	static void OnCurdiff(View* pView)
	{
		CMergeDoc *pd = pView->GetDocument();

		// If no diffs, nothing to select
		if (!pd->m_diffList.HasSignificantDiffs())
			return;

		// GetCurrentDiff() returns -1 if no diff selected
		int nDiff = pd->GetCurrentDiff();
		if (nDiff != -1)
		{
			// Scroll to the first line of the currently selected diff
			pView->SelectDiff(nDiff, true, false);
		}
		else
		{
			// If cursor is inside diff, select that diff
			CEPoint pos = pView->GetCursorPos();
			nDiff = pView->LineToDiff(pos.y);
			if (nDiff != -1 && pd->m_diffList.IsDiffSignificant(nDiff))
				pView->SelectDiff(nDiff, true, false);
		}
	}

	/**
	 * @brief Called when "Current diff" item is updated
	 */
	template <typename View>
	static void OnUpdateCurdiff(View* pView, CCmdUI* pCmdUI)
	{
		CMergeDoc* pd = pView->GetDocument();
		int nCurrentDiff = pd->GetCurrentDiff();
		if (nCurrentDiff == -1)
		{
			CEPoint pos = pView->GetCursorPos();
			int nNewDiff = pView->LineToDiff(pos.y);
			pCmdUI->Enable(nNewDiff != -1 && pd->m_diffList.IsDiffSignificant(nNewDiff));
		}
		else
			pCmdUI->Enable(true);
	}

	/**
	 * @brief Go to first diff
	 *
	 * Called when user selects "First Difference"
	 * @sa CMergeEditView::SelectDiff()
	 */
	template <typename View>
	static void OnFirstdiff(View* pView)
	{
		CMergeDoc* pd = pView->GetDocument();
		if (pd->m_diffList.HasSignificantDiffs())
		{
			int nDiff = FindFirstNonFilteredDiff(pView);
			if (nDiff >= 0)
				pView->SelectDiff(nDiff, true, false);
		}
	}

	/**
	 * @brief Update "First diff" UI items
	 */
	template <typename View>
	static void OnUpdateFirstdiff(View* pView, CCmdUI* pCmdUI)
	{
		CMergeDoc *pd = pView->GetDocument();
		pCmdUI->Enable(pd->m_diffList.HasSignificantDiffs() && FindFirstNonFilteredDiff(pView) >= 0);
	}

	/**
	 * @brief Go to last diff
	 */
	template <typename View>
	static void OnLastdiff(View* pView)
	{
		CMergeDoc *pd = pView->GetDocument();
		if (pd->m_diffList.HasSignificantDiffs())
		{
			int nDiff = FindLastNonFilteredDiff(pView);
			if (nDiff >= 0)
				pView->SelectDiff(nDiff, true, false);
		}
	}

	/**
	 * @brief Update "Last diff" UI items
	 */
	template <typename View>
	static void OnUpdateLastdiff(View* pView, CCmdUI* pCmdUI)
	{
		CMergeDoc *pd = pView->GetDocument();
		pCmdUI->Enable(pd->m_diffList.HasSignificantDiffs() && FindLastNonFilteredDiff(pView) >= 0);
	}

	/**
	 * @brief Go to next diff and select it.
	 *
	 * Finds and selects next difference. There are several cases:
	 * - if there is selected difference, and that difference is visible
	 * on screen, next found difference is selected.
	 * - if there is selected difference but it is not visible, next
	 * difference from cursor position is selected. This is what user
	 * expects to happen and is natural thing to do. Also reduces
	 * needless scrolling.
	 * - if there is no selected difference, next difference from cursor
	 * position is selected.
	 */
	template <typename View>
	static void OnNextdiff(View* pView)
	{
		CMergeDoc *pd = pView->GetDocument();
		int cnt = pView->LocateTextBuffer()->GetLineCount();
		if (cnt <= 0)
			return;

		// Returns -1 if no diff selected
		int curDiff = pd->GetCurrentDiff();
		int nextDiff = FindNextNonFilteredDiff(pView, curDiff);

		if (nextDiff >= 0)
			pView->SelectDiff(nextDiff, true, false);
		else if (IDirDoc *pDirDoc = pd->GetDirDoc())
		{
			if (pDirDoc->MoveableToNextDiff())
				pDirDoc->MoveToNextDiff(pd);
		}
	}

	/**
	 * @brief Update "Next diff" UI items
	 */
	template <typename View>
	static void OnUpdateNextdiff(View* pView, CCmdUI* pCmdUI)
	{
		CMergeDoc *pd = pView->GetDocument();
		bool enabled = HasNextNonFilteredDiff(pView);

		if (!enabled && pd->GetDirDoc())
			enabled = pd->GetDirDoc()->MoveableToNextDiff();

		pCmdUI->Enable(enabled);
	}

	/**
	 * @brief Go to previous diff and select it.
	 *
	 * Finds and selects previous difference. There are several cases:
	 * - if there is selected difference, and that difference is visible
	 * on screen, previous found difference is selected.
	 * - if there is selected difference but it is not visible, previous
	 * difference from cursor position is selected. This is what user
	 * expects to happen and is natural thing to do. Also reduces
	 * needless scrolling.
	 * - if there is no selected difference, previous difference from cursor
	 * position is selected.
	 */
	template <typename View>
	static void OnPrevdiff(View* pView)
	{
		CMergeDoc *pd = pView->GetDocument();
		int cnt = pView->LocateTextBuffer()->GetLineCount();
		if (cnt <= 0)
			return;

		// GetCurrentDiff() returns -1 if no diff selected
		int curDiff = pd->GetCurrentDiff();
		int prevDiff = FindPrevNonFilteredDiff(pView, curDiff);

		if (prevDiff >= 0)
			pView->SelectDiff(prevDiff, true, false);
		else if (IDirDoc *pDirDoc = pd->GetDirDoc())
		{
			if (pDirDoc->MoveableToPrevDiff())
				pDirDoc->MoveToPrevDiff(pd);
		}
	}

	/**
	 * @brief Update "Previous diff" UI items
	 */
	template <typename View>
	static void OnUpdatePrevdiff(View* pView, CCmdUI* pCmdUI)
	{
		CMergeDoc *pd = pView->GetDocument();
		bool enabled = HasPrevNonFilteredDiff(pView);

		if (!enabled && pd->GetDirDoc())
			enabled = pd->GetDirDoc()->MoveableToPrevDiff();

		pCmdUI->Enable(enabled);
	}

	template <typename View>
	static void OnNextConflict(View* pView)
	{
		// While merging, "next conflict" means the next difference that still
		// needs a decision, skipping the ones already resolved
		if (pView->GetDocument()->IsMergeResultPaneActive())
		{
			const int nDiff = FindPendingResultDiff(pView, true);
			if (nDiff >= 0)
			{
				pView->SelectDiff(nDiff, true, false);
				return;
			}
			// Nothing linked and pending. When unresolved segments remain the
			// segment <-> diff links were severed by a rescan: fall back to
			// plain conflict navigation instead of leaving the command dead
			if (pView->GetDocument()->GetResultUnresolvedCount() == 0)
				return;
		}
		OnNext3wayDiff(pView, THREEWAYDIFFTYPE_CONFLICT);
	}

	/**
	 * @brief Update "Next Conflict" UI items
	 */
	template <typename View>
	static void OnUpdateNextConflict(View* pView, CCmdUI* pCmdUI)
	{
		if (pView->GetDocument()->IsMergeResultPaneActive())
		{
			if (FindPendingResultDiff(pView, true) >= 0)
			{
				pCmdUI->Enable(TRUE);
				return;
			}
			if (pView->GetDocument()->GetResultUnresolvedCount() == 0)
			{
				pCmdUI->Enable(FALSE);
				return;
			}
			// severed links: use the plain conflict enablement below
		}
		pView->OnUpdateNext3wayDiff(pCmdUI, THREEWAYDIFFTYPE_CONFLICT);
	}

	/**
	 * @brief Update "Prev Conflict" UI items
	 */
	template <typename View>
	static void OnPrevConflict(View* pView)
	{
		if (pView->GetDocument()->IsMergeResultPaneActive())
		{
			const int nDiff = FindPendingResultDiff(pView, false);
			if (nDiff >= 0)
			{
				pView->SelectDiff(nDiff, true, false);
				return;
			}
			if (pView->GetDocument()->GetResultUnresolvedCount() == 0)
				return;
		}
		OnPrev3wayDiff(pView, THREEWAYDIFFTYPE_CONFLICT);
	}

	/**
	 * @brief Update "Prev Conflict" UI items
	 */
	template <typename View>
	static void OnUpdatePrevConflict(View* pView, CCmdUI* pCmdUI)
	{
		if (pView->GetDocument()->IsMergeResultPaneActive())
		{
			if (FindPendingResultDiff(pView, false) >= 0)
			{
				pCmdUI->Enable(TRUE);
				return;
			}
			if (pView->GetDocument()->GetResultUnresolvedCount() == 0)
			{
				pCmdUI->Enable(FALSE);
				return;
			}
		}
		pView->OnUpdatePrev3wayDiff(pCmdUI, THREEWAYDIFFTYPE_CONFLICT);
	}

	/**
	 * @brief Go to next 3-way diff and select it.
	 */
	template <typename View>
	static void OnNext3wayDiff(View* pView, int nDiffType)
	{
		CMergeDoc* pd = pView->GetDocument();
		int cnt = pView->LocateTextBuffer()->GetLineCount();
		if (cnt <= 0)
			return;

		// Returns -1 if no diff selected
		int curDiff = pd->GetCurrentDiff();
		if (curDiff != -1)
		{
			// We're on a diff
			int nextDiff = curDiff;
			if (!pView->IsDiffVisible(curDiff))
			{
				// Selected difference not visible, select next from cursor
				int line = pView->GetCursorPos().y;
				// Make sure we aren't in the first line of the diff
				++line;
				if (!pView->IsValidTextPosY(CEPoint(0, line)))
					line = pView->m_nTopLine;
				nextDiff = pView->NextSignificant3wayDiffFromLine(line, nDiffType);
			}
			else
			{
				// Find out if there is a following significant diff
				if (curDiff < pd->m_diffList.GetSize() - 1)
				{
					nextDiff = pd->m_diffList.NextSignificant3wayDiff(curDiff, nDiffType);
				}
			}

			// Skip filtered (hidden) diffs
			int lastDiff = pd->m_diffList.LastSignificant3wayDiff(nDiffType);
			while (nextDiff >= 0 && nextDiff <= lastDiff && pView->IsDiffFiltered(nextDiff))
			{
				if (nextDiff < pd->m_diffList.GetSize() - 1)
					nextDiff = pd->m_diffList.NextSignificant3wayDiff(nextDiff, nDiffType);
				else
					nextDiff = -1;
			}

			if (nextDiff == -1)
				nextDiff = curDiff;

			// nextDiff is the next one if there is one, else it is the one we're on
			pView->SelectDiff(nextDiff, true, false);
		}
		else
		{
			// We don't have a selected difference,
			// but cursor can be inside inactive diff
			int line = pView->GetCursorPos().y;
			if (!pView->IsValidTextPosY(CEPoint(0, line)))
				line = pView->m_nTopLine;
			curDiff = pView->NextSignificant3wayDiffFromLine(line, nDiffType);

			// Skip filtered (hidden) diffs
			int lastDiff = pd->m_diffList.LastSignificant3wayDiff(nDiffType);
			while (curDiff >= 0 && curDiff <= lastDiff && pView->IsDiffFiltered(curDiff))
			{
				if (curDiff < pd->m_diffList.GetSize() - 1)
					curDiff = pd->m_diffList.NextSignificant3wayDiff(curDiff, nDiffType);
				else
					curDiff = -1;
			}

			if (curDiff >= 0)
				pView->SelectDiff(curDiff, true, false);
		}
	}

	/**
	 * @brief Update "Next 3-way diff" UI items
	 */
	template <typename View>
	static void OnPrev3wayDiff(View* pView, int nDiffType)
	{
		CMergeDoc *pd = pView->GetDocument();

		int cnt = pView->LocateTextBuffer()->GetLineCount();
		if (cnt <= 0)
			return;

		// GetCurrentDiff() returns -1 if no diff selected
		int curDiff = pd->GetCurrentDiff();
		if (curDiff != -1)
		{
			// We're on a diff
			int prevDiff = curDiff;
			if (!pView->IsDiffVisible(curDiff))
			{
				// Selected difference not visible, select previous from cursor
				int line = pView->GetCursorPos().y;
				// Make sure we aren't in the last line of the diff
				--line;
				if (!pView->IsValidTextPosY(CEPoint(0, line)))
					line = pView->m_nTopLine;
				prevDiff = pView->PrevSignificant3wayDiffFromLine(line, nDiffType);
			}
			else
			{
				// Find out if there is a preceding significant diff
				if (curDiff > 0)
				{
					prevDiff = pd->m_diffList.PrevSignificant3wayDiff(curDiff, nDiffType);
				}
			}

			// Skip filtered (hidden) diffs
			int firstDiff = pd->m_diffList.FirstSignificant3wayDiff(nDiffType);
			while (prevDiff >= 0 && prevDiff >= firstDiff && pView->IsDiffFiltered(prevDiff))
			{
				if (prevDiff > 0)
					prevDiff = pd->m_diffList.PrevSignificant3wayDiff(prevDiff, nDiffType);
				else
					prevDiff = -1;
			}

			if (prevDiff == -1)
				prevDiff = curDiff;

			// prevDiff is the preceding one if there is one, else it is the one we're on
			pView->SelectDiff(prevDiff, true, false);
		}
		else
		{
			// We don't have a selected difference,
			// but cursor can be inside inactive diff
			int line = pView->GetCursorPos().y;
			if (!pView->IsValidTextPosY(CEPoint(0, line)))
				line = pView->m_nTopLine;
			curDiff = pView->PrevSignificant3wayDiffFromLine(line, nDiffType);

			// Skip filtered (hidden) diffs
			int firstDiff = pd->m_diffList.FirstSignificant3wayDiff(nDiffType);
			while (curDiff >= 0 && curDiff >= firstDiff && pView->IsDiffFiltered(curDiff))
			{
				if (curDiff > 0)
					curDiff = pd->m_diffList.PrevSignificant3wayDiff(curDiff, nDiffType);
				else
					curDiff = -1;
			}

			if (curDiff >= 0)
				pView->SelectDiff(curDiff, true, false);
		}
	}
};
