/**
 *  @file MoveDetection.cpp
 */

#include "pch.h"
#include "MoveDetection.h"
#include "DiffItem.h"
#include "DiffContext.h"
#include "FilterEngine/FilterExpression.h"
#include <set>

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
	dst.diffFileInfo[dstindex].m_pAdditionalProperties = std::move(src.diffFileInfo[srcindex].m_pAdditionalProperties);
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
	MoveDiffItemPartially(*pdi0, i0, di, 0);
	MoveDiffItemPartially(*pdi1, i1, di, 1);
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

MoveDetection::MoveDetection()
	: m_pMovedItems(std::make_shared<MovedItemsArray>())
{
}

MoveDetection::~MoveDetection()
{
}

void MoveDetection::SetMoveDetectionExpression(const FilterExpression* expr)
{
	m_pMoveDetectionExpression.reset(expr ? new FilterExpression(*expr) : nullptr);
}

void MoveDetection::DetectMovedItemsBetweenSides(
	const std::vector<DIFFITEM*>& unmatchedItems, int side0, int side1, CDiffContext& ctxt, MovedItemsArray& movedItems)
{
	PathContext paths(ctxt.GetPath(side0), ctxt.GetPath(side1));
	CDiffContext ctxtTmp(paths, ctxt.GetCompareMethod());
	m_pMoveDetectionExpression->SetDiffContext(&ctxtTmp);

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
			
			if ((!pdi1->diffcode.exists(side0) && !pdi1->diffcode.exists(side1)) ||
			    ( pdi0->diffcode.exists(side0) &&  pdi1->diffcode.exists(side0)) ||
			    ( pdi1->diffcode.exists(side1) &&  pdi0->diffcode.exists(side1)))
				continue;
			
			if (pdi0 != pdi1 && isfolder == pdi1->diffcode.isDirectory())
			{
				const int idx1 = pdi1->diffcode.exists(side0) ? side0 : side1;
				if (idx0 != idx1)
				{
					ProcessPair(pdi0, idx0, pdi1, idx1, m_pMoveDetectionExpression.get(), movedItems);
				}
			}
		}
	}
}

void MoveDetection::Detect(CDiffContext& ctxt)
{
	if (!m_pMoveDetectionExpression)
		return;

	m_isDetecting.store(true);

	// Create new MovedItemsArray for this detection run
	auto newMovedItems = std::make_shared<MovedItemsArray>();

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
	DetectMovedItemsBetweenSides(unmatchedItems, 0, 1, ctxt, *newMovedItems);
	
	// For 3-way comparison, check additional side pairs
	if (ctxt.GetCompareDirs() > 2)
	{
		// Detect moved items between side 1 and 2
		DetectMovedItemsBetweenSides(unmatchedItems, 1, 2, ctxt, *newMovedItems);
		
		// Detect moved items between side 0 and 2
		DetectMovedItemsBetweenSides(unmatchedItems, 0, 2, ctxt, *newMovedItems);
	}

	// Atomically replace the old MovedItemsArray with the new one
	std::atomic_store(&m_pMovedItems, newMovedItems);

	m_isDetecting.store(false);
}

std::vector<const DIFFITEM*> MoveDetection::GetMovedGroupItemsForSide(const CDiffContext& ctxt, const DIFFITEM* pdi, int sideIndex) const
{
	std::vector<const DIFFITEM*> items;

	if (IsDetecting() || pdi == nullptr || pdi->movedGroupId == -1)
		return items;
	
	// Get a local copy of the shared_ptr to prevent it from being deleted during access
	auto movedItems = std::atomic_load(&m_pMovedItems);
	if (!movedItems || pdi->movedGroupId >= static_cast<int>(movedItems->size()))
		return items;
	
	const auto& movedItem = (*movedItems)[pdi->movedGroupId];
	
	if (pdi->diffcode.exists(sideIndex))
	{
		items.push_back(pdi);
	}
	else
	{
		for (int i = 0; i < ctxt.GetCompareDirs(); i++)
		{
			auto it = movedItem.find(i);
			if (it != movedItem.end() && !it->second.empty())
			{
				for (auto pdi2: it->second)
				{
					if (pdi2->diffcode.exists(sideIndex))
						items.push_back(pdi2);
				}
			}
		}
	}
	return items;
}

void MoveDetection::MergeMovedItems(CDiffContext& ctxt)
{
	auto movedItems = std::atomic_load(&m_pMovedItems);
	if (!movedItems || movedItems->empty())
		return;

	std::set<DIFFITEM*> itemsToDelete;
	const int nDirs = ctxt.GetCompareDirs();
	DIFFITEM* diffpos = ctxt.GetFirstDiffPosition();
	while (diffpos != nullptr)
	{
		DIFFITEM& di = ctxt.GetNextDiffRefPosition(diffpos);
		if (di.movedGroupId != -1 && itemsToDelete.find(&di) == itemsToDelete.end())
		{
			auto& movedItemGroup = (*movedItems)[di.movedGroupId];
			for (int i = 0; i < nDirs; ++i)
			{
				if (di.diffcode.exists(i) || di.diffcode.isDirectory())
					continue;
				auto it = movedItemGroup.find(i);
				if (it == movedItemGroup.end() || it->second.size() != 1)
					continue;
				auto pdi2 = it->second[0];
				if (di.GetParentLink() != pdi2->GetParentLink())
					continue;
				CopyDiffItemPartially(di, i, *pdi2, i);
				MoveDiffItemPartially(di, i, *pdi2, i);
				di.diffcode.setSideFlag(i);
				di.diffcode.diffcode &= ~(DIFFCODE::TEXTFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY | DIFFCODE::EXPRFLAGS);
				di.diffcode.diffcode |= DIFFCODE::NEEDSCAN;
				di.movedGroupId = -1;
				itemsToDelete.insert(pdi2);
			}
		}
	}
	for (DIFFITEM* pdi : itemsToDelete)
	{
		pdi->DelinkFromSiblings();
		auto& movedItemGroup = (*movedItems)[pdi->movedGroupId];
		for (int i = 0; i < nDirs; ++i)
		{
			auto it = movedItemGroup.find(i);
			if (it != movedItemGroup.end())
			{
				auto vec = it->second;
				vec.erase(std::remove(vec.begin(), vec.end(), pdi), vec.end());
			}
		}
		delete pdi;
	}
}

