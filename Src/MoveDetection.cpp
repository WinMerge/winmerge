/**
 *  @file MoveDetection.cpp
 */

#include "pch.h"
#include "MoveDetection.h"
#include "DiffItem.h"
#include "DiffContext.h"
#include "FilterEngine/FilterExpression.h"

static void CopyDiffItemPartially(DIFFITEM& dst, int dstindex, DIFFITEM& src, int srcindex)
{
	dst.diffFileInfo[dstindex].filename = src.diffFileInfo[srcindex].filename;
	dst.diffFileInfo[dstindex].path = src.diffFileInfo[srcindex].path;
	dst.diffFileInfo[dstindex].size = src.diffFileInfo[srcindex].size;
	dst.diffFileInfo[dstindex].flags = src.diffFileInfo[srcindex].flags;
	dst.diffFileInfo[dstindex].ctime = src.diffFileInfo[srcindex].ctime;
	dst.diffFileInfo[dstindex].mtime = src.diffFileInfo[srcindex].mtime;
	dst.diffFileInfo[dstindex].version = src.diffFileInfo[srcindex].version;
	dst.diffFileInfo[dstindex].encoding = src.diffFileInfo[srcindex].encoding;
	dst.diffFileInfo[dstindex].m_textStats = src.diffFileInfo[srcindex].m_textStats;
}

static void MoveDiffItemPartially(DIFFITEM& dst, int dstindex, DIFFITEM& src, int srcindex)
{
	dst.diffFileInfo[dstindex].m_pAdditionalProperties = std::move(src.diffFileInfo[srcindex].m_pAdditionalProperties); // move temporarily
}

static bool EvaluatePair(DIFFITEM* pdi0, int i0, DIFFITEM* pdi1, int i1, CDiffContext& ctxt)
{
	DIFFITEM di;
	di.diffcode.setSideFlag(0);
	di.diffcode.setSideFlag(1);
	CopyDiffItemPartially(di, 0, *pdi0, i0);
	MoveDiffItemPartially(di, 0, *pdi0, i0);
	CopyDiffItemPartially(di, 1, *pdi1, i1);
	MoveDiffItemPartially(di, 1, *pdi1, i1);
	bool found = ctxt.m_pMoveDetectionExpression->Evaluate(di);
	MoveDiffItemPartially(*pdi0, i0, di, 0); // move back
	MoveDiffItemPartially(*pdi1, i1, di, 1); // move back
	return found;
}

namespace MoveDetection
{

void Detect(CDiffContext& ctxt)
{
	if (ctxt.m_pMoveDetectionExpression == nullptr)
		return;

	std::vector<DIFFITEM*> unmatchedItems;
	DIFFITEM* diffpos = ctxt.GetFirstDiffPosition();
	while (diffpos != nullptr)
	{
		DIFFITEM& di = ctxt.GetNextDiffRefPosition(diffpos);
		if (di.movedGroupId == -1 && !di.diffcode.existAll())
			unmatchedItems.push_back(&di);
	}

	auto& movedItems = ctxt.m_movedItems;
	for (DIFFITEM* pdi0 : unmatchedItems)
	{
		if (pdi0->diffcode.exists(0) && !pdi0->diffcode.exists(1))
		{
			const bool isfolder = pdi0->diffcode.isDirectory();
			for (DIFFITEM* pdi1 : unmatchedItems)
			{
				if (ctxt.ShouldAbort())
					return;
				if (pdi0 != pdi1 && !pdi1->diffcode.exists(0) && pdi1->diffcode.exists(1) && isfolder == pdi1->diffcode.isDirectory())
				{
					if (EvaluatePair(pdi0, 0, pdi1, 1, ctxt))
					{
						if (pdi1->movedGroupId != -1)
						{
							int existingGroupId = pdi1->movedGroupId;
							pdi0->movedGroupId = existingGroupId;
							movedItems[existingGroupId][0].push_back(pdi0);
						}
						else
						{
							movedItems.emplace_back();
							const int movedGroupId = static_cast<int>(movedItems.size() - 1);
							pdi0->movedGroupId = movedGroupId;
							pdi1->movedGroupId = movedGroupId;
							movedItems[movedGroupId][0].push_back(pdi0);
							movedItems[movedGroupId][1].push_back(pdi1);
						}
					}
				}
			}
		}
	}
	if (ctxt.GetCompareDirs() > 2)
	{
		for (DIFFITEM* pdi1 : unmatchedItems)
		{
			if (pdi1->diffcode.exists(1) && !pdi1->diffcode.exists(2))
			{
				const bool isfolder = pdi1->diffcode.isDirectory();
				for (DIFFITEM* pdi2 : unmatchedItems)
				{
					if (ctxt.ShouldAbort())
						return;
					if (pdi1 != pdi2 && !pdi2->diffcode.exists(1) && pdi2->diffcode.exists(2) && isfolder == pdi2->diffcode.isDirectory())
					{
						if (EvaluatePair(pdi1, 1, pdi2, 2, ctxt))
						{
							if (pdi1->movedGroupId != -1)
							{
								const int existingGroupId = pdi1->movedGroupId;
								pdi2->movedGroupId = existingGroupId;
								movedItems[existingGroupId][2].push_back(pdi2);
							}
							else if (pdi2->movedGroupId != -1)
							{
								int existingGroupId = pdi2->movedGroupId;
								pdi1->movedGroupId = existingGroupId;
								movedItems[existingGroupId][1].push_back(pdi1);
							}
							else
							{
								movedItems.emplace_back();
								int movedGroupId = static_cast<int>(movedItems.size() - 1);
								pdi1->movedGroupId = movedGroupId;
								pdi2->movedGroupId = movedGroupId;
								movedItems[movedGroupId][1].push_back(pdi1);
								movedItems[movedGroupId][2].push_back(pdi2);
							}
						}
					}
				}
			}
		}
	}
}

}
