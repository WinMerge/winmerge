/**
 *  @file MoveDetection.cpp
 */

#include "pch.h"
#include "MoveDetection.h"
#include "DiffItem.h"
#include "DiffContext.h"
#include "FilterEngine/FilterExpression.h"
#include "CompareStats.h"
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

static void ProcessPair(DIFFITEM* pdi0, int idx0, DIFFITEM* pdi1, int idx1, FilterExpression* pExpression, MovedItemGroups& movedItems)
{
	if (!EvaluatePair(pdi0, idx0, pdi1, idx1, pExpression))
		return;
	
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

MoveDetection::MoveDetection()
	: m_pMovedItemGroups(std::make_shared<MovedItemGroups>())
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
	const std::vector<DIFFITEM*>& unmatchedItems, int side0, int side1, CDiffContext& ctxt, MovedItemGroups& movedItems)
{
	PathContext paths(ctxt.GetPath(side0), ctxt.GetPath(side1));
	CDiffContext ctxtTmp(paths, ctxt.GetCompareMethod());
	m_pMoveDetectionExpression->SetDiffContext(&ctxtTmp);

	for (size_t i = 0; i < unmatchedItems.size(); ++i)
	{
		DIFFITEM* pdi0 = unmatchedItems[i];

		if (ctxt.m_pCompareStats)
		{
			ctxt.m_pCompareStats->BeginCompare(pdi0, 0);
			ctxt.m_pCompareStats->AddItem(-1);
		}

		if (!pdi0->diffcode.exists(side0) && !pdi0->diffcode.exists(side1))
			continue;
		
		const int idx0 = pdi0->diffcode.exists(side0) ? side0 : side1;
		const bool pdi0IsFolder = pdi0->diffcode.isDirectory();
		const bool pdi0ExistsSide0 = pdi0->diffcode.exists(side0);
		const bool pdi0ExistsSide1 = pdi0->diffcode.exists(side1);
		
		for (size_t j = i + 1; j < unmatchedItems.size(); ++j)
		{
			if (ctxt.ShouldAbort())
				return;
			
			DIFFITEM* pdi1 = unmatchedItems[j];
			const int idx1 = pdi1->diffcode.exists(side0) ? side0 : side1;

			if (idx0 == idx1)
				continue;
			
			if (pdi0 == pdi1 || pdi0IsFolder != pdi1->diffcode.isDirectory())
				continue;

			if (pdi0->movedGroupId != -1 && pdi1->movedGroupId != -1)
				continue;

			if ((!pdi1->diffcode.exists(side0) && !pdi1->diffcode.exists(side1)) ||
			    ( pdi0ExistsSide0              &&  pdi1->diffcode.exists(side0)) ||
			    ( pdi1->diffcode.exists(side1) &&  pdi0ExistsSide1            ))
				continue;
			
			ProcessPair(pdi0, idx0, pdi1, idx1, m_pMoveDetectionExpression.get(), movedItems);
		}
	}
}

void MoveDetection::Detect(CDiffContext& ctxt)
{
	if (!m_pMoveDetectionExpression)
		return;

	// Create new MovedItemGroups for this detection run
	auto newMovedItems = std::make_shared<MovedItemGroups>();

	// Collect all unmatched items
	std::vector<DIFFITEM*> unmatchedItems;
	DIFFITEM* diffpos = ctxt.GetFirstDiffPosition();
	while (diffpos != nullptr)
	{
		DIFFITEM& di = ctxt.GetNextDiffRefPosition(diffpos);
		if (di.movedGroupId == -1 && !di.diffcode.existAll())
			unmatchedItems.push_back(&di);
	}

	if (ctxt.m_pCompareStats)
		ctxt.m_pCompareStats->IncreaseTotalItems(static_cast<int>(unmatchedItems.size()) * (ctxt.GetCompareDirs() > 2 ? 3 : 1));

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

	// Atomically replace the old MovedItemGroups with the new one
	std::atomic_store(&m_pMovedItemGroups, newMovedItems);
}

void MoveDetection::MergeMovedItems(CDiffContext& ctxt)
{
	auto movedItemGroups = std::atomic_load(&m_pMovedItemGroups);
	if (!movedItemGroups || movedItemGroups->empty())
		return;

	std::set<DIFFITEM*> itemsToDelete;
	const int nDirs = ctxt.GetCompareDirs();
	DIFFITEM* diffpos = ctxt.GetFirstDiffPosition();
	while (diffpos != nullptr)
	{
		DIFFITEM& di = ctxt.GetNextDiffRefPosition(diffpos);
		if (di.movedGroupId != -1 && itemsToDelete.find(&di) == itemsToDelete.end())
		{
			auto& movedItemGroup = (*movedItemGroups)[di.movedGroupId];
			for (int i = 0; i < nDirs; ++i)
			{
				if (di.diffcode.exists(i))
					continue;
				if (movedItemGroup[i].size() != 1)
					continue;
				auto pdi2 = movedItemGroup[i][0];
				if (di.GetParentLink() != pdi2->GetParentLink())
					continue;
				// If the item to be deleted has children, reparent them
				if (pdi2->HasChildren())
				{
					DIFFITEM* child = pdi2->GetFirstChild();
					while (child != nullptr)
					{
						DIFFITEM* nextChild = child->GetFwdSiblingLink();
						child->DelinkFromSiblings();
						di.AddChildToParent(child);
						child = nextChild;
					}
				}
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
		auto& movedItemGroup = (*movedItemGroups)[pdi->movedGroupId];
		for (int i = 0; i < nDirs; ++i)
		{
			auto& vec = movedItemGroup[i];
			vec.erase(std::remove(vec.begin(), vec.end(), pdi), vec.end());
		}
		delete pdi;
	}
}

std::vector<const DIFFITEM*> MoveDetection::GetMovedGroupItemsForSide(const CDiffContext& ctxt, const DIFFITEM& di, int sideIndex) const
{
	std::vector<const DIFFITEM*> items;

	if (di.movedGroupId == -1)
		return items;
	
	// Get a local copy of the shared_ptr to prevent it from being deleted during access
	auto movedItemGroups = std::atomic_load(&m_pMovedItemGroups);
	if (!movedItemGroups || di.movedGroupId >= static_cast<int>(movedItemGroups->size()))
		return items;
	
	const auto& movedItemGroup = (*movedItemGroups)[di.movedGroupId];
	
	if (di.diffcode.exists(sideIndex))
	{
		items.push_back(&di);
	}
	else
	{
		for (int i = 0; i < ctxt.GetCompareDirs(); i++)
		{
			for (auto pdi2: movedItemGroup[i])
			{
				if (pdi2->diffcode.exists(sideIndex))
					items.push_back(pdi2);
			}
		}
	}
	return items;
}

void MoveDetection::CheckMovedOrRenamed(const CDiffContext& ctxt, const DIFFITEM& di, bool& moved, bool& renamed) const
{
	const int nDirs = ctxt.GetCompareDirs();
	std::vector<std::vector<const DIFFITEM*>> sideItems(nDirs);

	for (int side = 0; side < nDirs; ++side)
		sideItems[side] = GetMovedGroupItemsForSide(ctxt, di, side);

	// ---- moved / renamed detection ----
	moved = false;
	renamed = false;

	for (size_t i = 0; i < sideItems.size(); ++i)
	{
		for (size_t j = i + 1; j < sideItems.size(); ++j)
		{
			for (size_t k = 0; k < sideItems[i].size(); ++k)
			{
				const auto* a = sideItems[i][k];
				for (size_t l = 0; l < sideItems[j].size(); ++l)
				{
					const auto* b = sideItems[j][l];
					if (a->GetParentLink() == b->GetParentLink() && a->diffFileInfo[i].filename != b->diffFileInfo[j].filename)
						renamed = true;
					if (a->GetParentLink() != b->GetParentLink() && a->diffFileInfo[i].filename == b->diffFileInfo[j].filename)
						moved = true;
				}
			}
		}
	}
}
