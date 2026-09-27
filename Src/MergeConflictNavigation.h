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
		if (nCurDiff != -1)
			nBegin = bNext ? nCurDiff + 1 : nCurDiff - 1;
		else
		{
			// With no selected difference, include the cursor's difference.
			const int nLine = pView->GetCursorPos().y;
			nBegin = bNext ? pView->NextSignificantDiffFromLine(nLine) :
				pView->PrevSignificantDiffFromLine(nLine);
			if (nBegin == -1)
				nBegin = bNext ? nDiffCount : -1;
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
			{
				pView->SelectDiff(nDiff, true, false);
				return;
			}
			// A rescan may have severed result-to-diff links. Keep navigation
			// available while unresolved result segments still exist.
			if (pDoc->GetResultUnresolvedCount() == 0)
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
			if (FindPendingResultDiff(pView, bNext) >= 0)
			{
				pCmdUI->Enable(TRUE);
				return;
			}
			if (pDoc->GetResultUnresolvedCount() == 0)
			{
				pCmdUI->Enable(FALSE);
				return;
			}
		}
		if (bNext)
			pView->OnUpdateNext3wayDiff(pCmdUI, THREEWAYDIFFTYPE_CONFLICT);
		else
			pView->OnUpdatePrev3wayDiff(pCmdUI, THREEWAYDIFFTYPE_CONFLICT);
	}
};
