/**
 * @file  MergeConflictNavigation.h
 * @brief Navigation to unresolved merge conflicts and result differences.
 */
#pragma once

#include "MergeDiffNavigation.h"

/**
 * @brief Conflict navigation shared by the compare and merge-result views.
 *
 * While a merge result exists, conflict navigation visits unresolved result
 * differences. Otherwise it retains the regular 3-way conflict navigation.
 */
class CMergeConflictNavigation
{
public:
	template <typename View>
	static int FindPendingResultDiff(View* pView, bool bNext)
	{
		CMergeDoc* pDoc = pView->GetDocument();
		const int nDiffCount = pDoc->m_diffList.GetSize();
		const int nCurDiff = pDoc->GetCurrentDiff();
		int nBegin;
		int blockFirst = nCurDiff, blockLast = nCurDiff;
		if (nCurDiff != -1)
		{
			// The search starts outside the block of joined diffs the selected difference
			// belongs to (CMergeDiffNavigation::GetDiffBlockRange), so one change divided into
			// joined diffs is one move; copying resolves such a block in one action.
			CMergeDiffNavigation::GetDiffBlockRange(pView, nCurDiff, blockFirst, blockLast);
			nBegin = bNext ? blockLast + 1 : blockFirst - 1;
		}
		else
		{
			// With no selected difference, include the cursor's difference, that is its whole block.
			const int nLine = pView->GetCursorPos().y;
			const int nFound = bNext ? pView->NextSignificantDiffFromLine(nLine) :
				pView->PrevSignificantDiffFromLine(nLine);
			if (nFound == -1)
				nBegin = bNext ? nDiffCount : -1;
			else
			{
				CMergeDiffNavigation::GetDiffBlockRange(pView, nFound, blockFirst, blockLast);
				nBegin = bNext ? blockFirst : blockLast;
			}
		}

		const int nStep = bNext ? 1 : -1;
		for (int nDiff = nBegin; nDiff >= 0 && nDiff < nDiffCount; nDiff += nStep)
		{
			if (pDoc->m_diffList.IsDiffSignificant(nDiff) &&
				!pView->IsDiffFiltered(nDiff) && pDoc->IsResultDiffPending(nDiff))
				return nDiff;
		}
		return -1;
	}

	template <typename View>
	static void OnConflict(View* pView, bool bNext)
	{
		CMergeDoc* pDoc = pView->GetDocument();
		if (pDoc->GetMergeResultBuildState())
		{
			const int nDiff = FindPendingResultDiff(pView, bNext);
			if (nDiff >= 0)
				pView->SelectDiff(nDiff, true, false);
			return;
		}
		if (bNext)
			CMergeDiffNavigation::OnNext3wayDiff(pView, THREEWAYDIFFTYPE_CONFLICT);
		else
			CMergeDiffNavigation::OnPrev3wayDiff(pView, THREEWAYDIFFTYPE_CONFLICT);
	}

	template <typename View>
	static void OnUpdateConflict(View* pView, CCmdUI* pCmdUI, bool bNext)
	{
		CMergeDoc* pDoc = pView->GetDocument();
		if (pDoc->GetMergeResultBuildState())
		{
			pCmdUI->Enable(FindPendingResultDiff(pView, bNext) >= 0);
			return;
		}
		if (bNext)
			pView->OnUpdateNext3wayDiff(pCmdUI, THREEWAYDIFFTYPE_CONFLICT);
		else
			pView->OnUpdatePrev3wayDiff(pCmdUI, THREEWAYDIFFTYPE_CONFLICT);
	}
};
