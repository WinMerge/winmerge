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

static bool EvaluatePair(DIFFITEM* pdi0, int i0, DIFFITEM* pdi1, int i1, FilterExpression* pExpression)
{
	DIFFITEM di;
	di.diffcode.setSideFlag(0);
	di.diffcode.setSideFlag(1);
	CopyDiffItemPartially(di, 0, *pdi0, i0);
	MoveDiffItemPartially(di, 0, *pdi0, i0);
	CopyDiffItemPartially(di, 1, *pdi1, i1);
	MoveDiffItemPartially(di, 1, *pdi1, i1);
	bool found = pExpression->Evaluate(di);
	MoveDiffItemPartially(*pdi0, i0, di, 0); // move back
	MoveDiffItemPartially(*pdi1, i1, di, 1); // move back
	return found;
}

static void ProcessPair(DIFFITEM* pdi0, int idx0, DIFFITEM* pdi1, int idx1, FilterExpression* pExpression, MovedItemsArray& movedItems)
{
	if (EvaluatePair(pdi0, idx0, pdi1, idx1, pExpression))
	{
		if (pdi0->movedGroupId != -1 && pdi1->movedGroupId == -1)
		{
			int existingGroupId = pdi0->movedGroupId;
			pdi1->movedGroupId = existingGroupId;
			movedItems[existingGroupId][idx1].push_back(pdi1);
		}
		else if (pdi0->movedGroupId == -1 && pdi1->movedGroupId != -1)
		{
			int existingGroupId = pdi1->movedGroupId;
			pdi0->movedGroupId = existingGroupId;
			movedItems[existingGroupId][idx0].push_back(pdi0);
		}
		else if (pdi0->movedGroupId == -1 && pdi1->movedGroupId == -1)
		{
			movedItems.emplace_back();
			int movedGroupId = static_cast<int>(movedItems.size() - 1);
			pdi0->movedGroupId = movedGroupId;
			pdi1->movedGroupId = movedGroupId;
			movedItems[movedGroupId][idx0].push_back(pdi0);
			movedItems[movedGroupId][idx1].push_back(pdi1);
		}
	}
}

static void DetectMovedItemsBetweenSides(
	const std::vector<DIFFITEM*>& unmatchedItems, int side0, int side1, CDiffContext& ctxt, MovedItemsArray& movedItems)
{
	PathContext paths(ctxt.GetPath(side0), ctxt.GetPath(side1));
	CDiffContext ctxtTmp(paths, ctxt.GetCompareMethod());
	std::unique_ptr<FilterExpression> pExpression(new FilterExpression());
	pExpression->Parse(ctxt.m_pMoveDetectionExpression->expression);
	pExpression->SetDiffContext(&ctxtTmp);

	for (DIFFITEM* pdi0 : unmatchedItems)
	{
		if (!pdi0->diffcode.exists(side0) && !pdi0->diffcode.exists(side1))
			continue;
		
		const int idx0 = pdi0->diffcode.exists(side0) ? side0 : side1;
		const bool isfolder = pdi0->diffcode.isDirectory();
		
		for (DIFFITEM* pdi1 : unmatchedItems)
		{
			if (ctxt.ShouldAbort())
				return;
			
			if (!pdi1->diffcode.exists(side0) && !pdi1->diffcode.exists(side1))
				continue;
			
			if (pdi0 != pdi1 && isfolder == pdi1->diffcode.isDirectory())
			{
				const int idx1 = pdi1->diffcode.exists(side0) ? side0 : side1;
				if (idx0 != idx1)
				{
					ProcessPair(pdi0, idx0, pdi1, idx1, pExpression.get(), movedItems);
				}
			}
		}
	}
}

namespace MoveDetection
{

MovedItemsArray Detect(CDiffContext& ctxt)
{
	MovedItemsArray movedItems;

	if (ctxt.m_pMoveDetectionExpression == nullptr)
		return movedItems;

	// Collect all unmatched items
	std::vector<DIFFITEM*> unmatchedItems;
	DIFFITEM* diffpos = ctxt.GetFirstDiffPosition();
	while (diffpos != nullptr)
	{
		DIFFITEM& di = ctxt.GetNextDiffRefPosition(diffpos);
		if (di.movedGroupId == -1 && !di.diffcode.existAll())
			unmatchedItems.push_back(&di);
	}

	// Detect moved items between side 0 and 1
	DetectMovedItemsBetweenSides(unmatchedItems, 0, 1, ctxt, movedItems);
	
	// For 3-way comparison, check additional side pairs
	if (ctxt.GetCompareDirs() > 2)
	{
		// Detect moved items between side 1 and 2
		DetectMovedItemsBetweenSides(unmatchedItems, 1, 2, ctxt, movedItems);
		
		// Detect moved items between side 0 and 2
		DetectMovedItemsBetweenSides(unmatchedItems, 0, 2, ctxt, movedItems);
	}

	return movedItems;
}

const DIFFITEM* GetMovedItemByDIFFITEM(const CDiffContext& ctxt, const DIFFITEM* pdi, int sideIndex)
{
	auto& movedItems = ctxt.m_movedItems;
	if (!pdi || movedItems.size() <= pdi->movedGroupId)
		return nullptr;
	auto& movedItem = movedItems[pdi->movedGroupId];
	const DIFFITEM* pdiTmp = nullptr;
	if (pdi->diffcode.exists(sideIndex))
	{
		pdiTmp = pdi;
	}
	else
	{
		for (int i = 0; i < ctxt.GetCompareDirs(); i++)
		{
			if (movedItem.count(i) && !movedItem.at(i).empty() && movedItem.at(i)[0]->diffcode.exists(sideIndex))
				pdiTmp = movedItem.at(i)[0];
		}
	}
	return pdiTmp;
}

}
