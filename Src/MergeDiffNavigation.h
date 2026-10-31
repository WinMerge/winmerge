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
	 * @brief Index range of the block of diffs that the given diff belongs to.
	 *
	 * A block is the run of joined significant diffs returned by
	 * DiffList::GetJoinedSignificantDiffRange: one change that "Align similar lines"
	 * (CMergeDoc::AdjustDiffBlocks) divided into several diffs is one block. Every diff move
	 * (next/previous difference, first/last difference, the 3-way moves, and the Alt+wheel and
	 * right-button+wheel shortcuts that post those commands in MouseHook.cpp) travels block by
	 * block, so one change counts as one difference. The highlight of the selected difference
	 * itself is not changed by this: it keeps covering the single diff that is selected.
	 * @param [in] nDiff Index of a diff inside the block; an out of range index is returned unchanged.
	 * @param [out] blockFirst Index of the first diff of the block.
	 * @param [out] blockLast Index of the last diff of the block.
	 */
	template <typename View>
	static void GetDiffBlockRange(View* pView, int nDiff, int& blockFirst, int& blockLast)
	{
		const CMergeDoc *pd = pView->GetDocument();
		blockFirst = blockLast = nDiff;
		if (nDiff >= 0 && nDiff < pd->m_diffList.GetSize())
			pd->m_diffList.GetJoinedSignificantDiffRange(nDiff, blockFirst, blockLast);
	}

	/**
	 * @brief Diff to select when a move lands on the block of the given diff.
	 * @return Index of the first diff of the block that the line filter does not hide,
	 *  or -1 when the line filter hides every diff of the block.
	 */
	template <typename View>
	static int FindBlockEntryDiff(View* pView, int nDiff)
	{
		if (nDiff < 0)
			return -1;
		int blockFirst = nDiff, blockLast = nDiff;
		GetDiffBlockRange(pView, nDiff, blockFirst, blockLast);
		for (int i = blockFirst; i <= blockLast; ++i)
		{
			if (!pView->IsDiffFiltered(i))
				return i;
		}
		return -1;
	}

	/**
	 * @brief Find the block that follows the block of the given diff.
	 * Blocks whose diffs are all hidden by the line filter are skipped.
	 * @param [in] nDiff Index of a diff inside the block to move away from.
	 * @return Index of the diff to select, or -1 if there is no such block.
	 */
	template <typename View>
	static int FindNextBlockEntryDiff(View* pView, int nDiff)
	{
		const CMergeDoc *pd = pView->GetDocument();
		int nSearch = nDiff;
		while (nSearch >= 0 && nSearch < pd->m_diffList.GetSize())
		{
			int blockFirst = nSearch, blockLast = nSearch;
			GetDiffBlockRange(pView, nSearch, blockFirst, blockLast);
			nSearch = pd->m_diffList.NextSignificantDiff(blockLast);
			const int nEntry = FindBlockEntryDiff(pView, nSearch);
			if (nEntry >= 0)
				return nEntry;
		}
		return -1;
	}

	/**
	 * @brief Find the block that precedes the block of the given diff.
	 * Blocks whose diffs are all hidden by the line filter are skipped.
	 * @param [in] nDiff Index of a diff inside the block to move away from.
	 * @return Index of the diff to select, or -1 if there is no such block.
	 */
	template <typename View>
	static int FindPrevBlockEntryDiff(View* pView, int nDiff)
	{
		const CMergeDoc *pd = pView->GetDocument();
		int nSearch = nDiff;
		while (nSearch >= 0 && nSearch < pd->m_diffList.GetSize())
		{
			int blockFirst = nSearch, blockLast = nSearch;
			GetDiffBlockRange(pView, nSearch, blockFirst, blockLast);
			nSearch = pd->m_diffList.PrevSignificantDiff(blockFirst);
			const int nEntry = FindBlockEntryDiff(pView, nSearch);
			if (nEntry >= 0)
				return nEntry;
		}
		return -1;
	}

	/**
	 * @brief Find the block to move to when searching forwards from a line.
	 * A block that starts above the line is the block the line is in, that is the block the user
	 * is on, so the move goes to the block after it.
	 * @return Index of the diff to select, or -1 if there is no such block.
	 */
	template <typename View>
	static int FindNextBlockEntryDiffFromLine(View* pView, int nLine)
	{
		const CMergeDoc *pd = pView->GetDocument();
		const int nFound = pView->NextSignificantDiffFromLine(nLine);
		if (nFound < 0)
			return -1;
		int blockFirst = nFound, blockLast = nFound;
		GetDiffBlockRange(pView, nFound, blockFirst, blockLast);
		DIFFRANGE di;
		if (pd->m_diffList.GetDiff(blockFirst, di) && di.dbegin < nLine)
			return FindNextBlockEntryDiff(pView, nFound);
		const int nEntry = FindBlockEntryDiff(pView, nFound);
		return (nEntry >= 0) ? nEntry : FindNextBlockEntryDiff(pView, nFound);
	}

	/**
	 * @brief Find the block to move to when searching backwards from a line.
	 * A block that ends below the line is the block the line is in, that is the block the user
	 * is on, so the move goes to the block before it.
	 * @return Index of the diff to select, or -1 if there is no such block.
	 */
	template <typename View>
	static int FindPrevBlockEntryDiffFromLine(View* pView, int nLine)
	{
		const CMergeDoc *pd = pView->GetDocument();
		const int nFound = pView->PrevSignificantDiffFromLine(nLine);
		if (nFound < 0)
			return -1;
		int blockFirst = nFound, blockLast = nFound;
		GetDiffBlockRange(pView, nFound, blockFirst, blockLast);
		DIFFRANGE di;
		if (pd->m_diffList.GetDiff(blockLast, di) && di.dend > nLine)
			return FindPrevBlockEntryDiff(pView, nFound);
		const int nEntry = FindBlockEntryDiff(pView, nFound);
		return (nEntry >= 0) ? nEntry : FindPrevBlockEntryDiff(pView, nFound);
	}

	/**
	 * @brief Find the first non-filtered diff
	 * @return Index of the diff to select for the first block, or -1 if none found
	 */
	template <typename View>
	static int FindFirstNonFilteredDiff(View* pView)
	{
		const CMergeDoc *pd = pView->GetDocument();
		const int nDiff = pd->m_diffList.FirstSignificantDiff();
		if (nDiff < 0)
			return -1;
		const int nEntry = FindBlockEntryDiff(pView, nDiff);
		return (nEntry >= 0) ? nEntry : FindNextBlockEntryDiff(pView, nDiff);
	}

	/**
	 * @brief Find the last non-filtered diff
	 * @return Index of the diff to select for the last block, or -1 if none found
	 */
	template <typename View>
	static int FindLastNonFilteredDiff(View* pView)
	{
		const CMergeDoc *pd = pView->GetDocument();
		const int nDiff = pd->m_diffList.LastSignificantDiff();
		if (nDiff < 0)
			return -1;
		const int nEntry = FindBlockEntryDiff(pView, nDiff);
		return (nEntry >= 0) ? nEntry : FindPrevBlockEntryDiff(pView, nDiff);
	}

	/**
	 * @brief Find next non-filtered diff from current position or given diff
	 * The move is block by block, see GetDiffBlockRange.
	 * @param [in] startDiff Current diff index (-1 for search from cursor position)
	 * @return Index of the diff to select for the next block, or -1 if none found
	 */
	template <typename View>
	static int FindNextNonFilteredDiff(View* pView, int startDiff)
	{
		if (startDiff != -1 && pView->IsDiffVisible(startDiff))
		{
			// We're on a visible diff: move to the block after the block it belongs to
			return FindNextBlockEntryDiff(pView, startDiff);
		}

		// No selected diff, or the selected difference is not visible: search from cursor
		int line = pView->GetCursorPos().y;
		if (startDiff != -1)
		{
			// Make sure we aren't in the first line of the diff
			++line;
		}
		if (!pView->IsValidTextPosY(CEPoint(0, line)))
			line = pView->m_nTopLine;
		return FindNextBlockEntryDiffFromLine(pView, line);
	}

	/**
	 * @brief Find previous non-filtered diff from current position or given diff
	 * The move is block by block, see GetDiffBlockRange.
	 * @param [in] startDiff Current diff index (-1 for search from cursor position)
	 * @return Index of the diff to select for the previous block, or -1 if none found
	 */
	template <typename View>
	static int FindPrevNonFilteredDiff(View* pView, int startDiff)
	{
		if (startDiff != -1 && pView->IsDiffVisible(startDiff))
		{
			// We're on a visible diff: move to the block before the block it belongs to
			return FindPrevBlockEntryDiff(pView, startDiff);
		}

		// No selected diff, or the selected difference is not visible: search from cursor
		int line = pView->GetCursorPos().y;
		if (startDiff != -1)
		{
			// Make sure we aren't in the last line of the diff
			--line;
		}
		if (!pView->IsValidTextPosY(CEPoint(0, line)))
			line = pView->m_nTopLine;
		return FindPrevBlockEntryDiffFromLine(pView, line);
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

	/**
	 * @brief Find the next diff of the given 3-way type, moving block by block.
	 * The search starts after the block of startDiff (see GetDiffBlockRange), so a change that is
	 * divided into joined diffs is one move. Inside the block it lands on, the result is the first
	 * diff of the requested 3-way type, which for a block of one type is the first diff of the block.
	 * @param [in] startDiff Index of a diff inside the block to move away from, -1 to search from nLine.
	 * @param [in] nLine Line the search starts from when startDiff is -1.
	 * @return Index of the diff to select, or -1 if none found.
	 */
	template <typename View>
	static int FindNext3wayDiff(View* pView, int startDiff, int nLine, int nDiffType)
	{
		const CMergeDoc* pd = pView->GetDocument();
		int nextDiff = -1;
		if (startDiff >= 0 && startDiff < pd->m_diffList.GetSize())
		{
			int blockFirst = startDiff, blockLast = startDiff;
			GetDiffBlockRange(pView, startDiff, blockFirst, blockLast);
			nextDiff = pd->m_diffList.NextSignificant3wayDiff(blockLast, nDiffType);
		}
		else
		{
			nextDiff = pView->NextSignificant3wayDiffFromLine(nLine, nDiffType);
			if (nextDiff >= 0)
			{
				// A block that starts above the line is the block the line is in, that is the block
				// the user is on, so the move goes to the block after it
				int blockFirst = nextDiff, blockLast = nextDiff;
				GetDiffBlockRange(pView, nextDiff, blockFirst, blockLast);
				DIFFRANGE di;
				if (pd->m_diffList.GetDiff(blockFirst, di) && di.dbegin < nLine)
					nextDiff = pd->m_diffList.NextSignificant3wayDiff(blockLast, nDiffType);
			}
		}

		// Skip filtered (hidden) diffs
		while (nextDiff >= 0 && pView->IsDiffFiltered(nextDiff))
			nextDiff = pd->m_diffList.NextSignificant3wayDiff(nextDiff, nDiffType);

		return nextDiff;
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
		int line = pView->GetCursorPos().y;
		if (curDiff != -1 && !pView->IsDiffVisible(curDiff))
		{
			// Make sure we aren't in the first line of the diff
			++line;
		}
		if (!pView->IsValidTextPosY(CEPoint(0, line)))
			line = pView->m_nTopLine;

		const bool bFromCurrentDiff = (curDiff != -1 && pView->IsDiffVisible(curDiff));
		int nextDiff = FindNext3wayDiff(pView, bFromCurrentDiff ? curDiff : -1, line, nDiffType);

		if (nextDiff == -1 && curDiff != -1)
		{
			// No next one: stay on the one we're on
			nextDiff = curDiff;
		}
		if (nextDiff >= 0)
			pView->SelectDiff(nextDiff, true, false);
	}

	/**
	 * @brief Find the previous diff of the given 3-way type, moving block by block.
	 * The search starts before the block of startDiff (see GetDiffBlockRange), so a change that is
	 * divided into joined diffs is one move.
	 * @param [in] startDiff Index of a diff inside the block to move away from, -1 to search from nLine.
	 * @param [in] nLine Line the search starts from when startDiff is -1.
	 * @return Index of the diff to select, or -1 if none found.
	 */
	template <typename View>
	static int FindPrev3wayDiff(View* pView, int startDiff, int nLine, int nDiffType)
	{
		const CMergeDoc* pd = pView->GetDocument();
		int prevDiff = -1;
		if (startDiff >= 0 && startDiff < pd->m_diffList.GetSize())
		{
			int blockFirst = startDiff, blockLast = startDiff;
			GetDiffBlockRange(pView, startDiff, blockFirst, blockLast);
			prevDiff = pd->m_diffList.PrevSignificant3wayDiff(blockFirst, nDiffType);
		}
		else
		{
			prevDiff = pView->PrevSignificant3wayDiffFromLine(nLine, nDiffType);
			if (prevDiff >= 0)
			{
				// A block that ends below the line is the block the line is in, that is the block
				// the user is on, so the move goes to the block before it
				int blockFirst = prevDiff, blockLast = prevDiff;
				GetDiffBlockRange(pView, prevDiff, blockFirst, blockLast);
				DIFFRANGE di;
				if (pd->m_diffList.GetDiff(blockLast, di) && di.dend > nLine)
					prevDiff = pd->m_diffList.PrevSignificant3wayDiff(blockFirst, nDiffType);
			}
		}

		// Skip filtered (hidden) diffs
		while (prevDiff >= 0 && pView->IsDiffFiltered(prevDiff))
			prevDiff = pd->m_diffList.PrevSignificant3wayDiff(prevDiff, nDiffType);

		// The found diff is the last one of the requested type inside its block; move to the first
		// one of that type in the same block so that the block is entered at its start
		while (prevDiff > 0)
		{
			int blockFirst = prevDiff, blockLast = prevDiff;
			GetDiffBlockRange(pView, prevDiff, blockFirst, blockLast);
			const int nEarlier = pd->m_diffList.PrevSignificant3wayDiff(prevDiff, nDiffType);
			if (nEarlier < blockFirst || nEarlier < 0 || pView->IsDiffFiltered(nEarlier))
				break;
			prevDiff = nEarlier;
		}

		return prevDiff;
	}

	/**
	 * @brief Update "Previous 3-way diff" UI items
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
		int line = pView->GetCursorPos().y;
		if (curDiff != -1 && !pView->IsDiffVisible(curDiff))
		{
			// Make sure we aren't in the last line of the diff
			--line;
		}
		if (!pView->IsValidTextPosY(CEPoint(0, line)))
			line = pView->m_nTopLine;

		const bool bFromCurrentDiff = (curDiff != -1 && pView->IsDiffVisible(curDiff));
		int prevDiff = FindPrev3wayDiff(pView, bFromCurrentDiff ? curDiff : -1, line, nDiffType);

		if (prevDiff == -1 && curDiff != -1)
		{
			// No preceding one: stay on the one we're on
			prevDiff = curDiff;
		}
		if (prevDiff >= 0)
			pView->SelectDiff(prevDiff, true, false);
	}
};
