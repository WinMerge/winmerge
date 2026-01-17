/**
 *  @file RenameMoveDetection.cpp
 */

#include "pch.h"
#include "RenameMoveDetection.h"
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
	else if (pdi0->movedGroupId != -1 && pdi1->movedGroupId != -1)
	{
		//assert(pdi0->movedGroupId == pdi1->movedGroupId);
	}
}

RenameMoveDetection::RenameMoveDetection()
{
}

RenameMoveDetection::~RenameMoveDetection()
{
}

void RenameMoveDetection::SetMoveDetectionExpression(const FilterExpression* expr)
{
	m_pRenameMoveDetectionExpression.reset(expr ? new FilterExpression(*expr) : nullptr);
}

void RenameMoveDetection::DetectMovedItemsBetweenSides(
	const std::vector<DIFFITEM*>& unmatchedItems, int side0, int side1, CDiffContext& ctxt, MovedItemGroups& movedItems)
{
	PathContext paths(ctxt.GetPath(side0), ctxt.GetPath(side1));
	CDiffContext ctxtTmp(paths, ctxt.GetCompareMethod());
	m_pRenameMoveDetectionExpression->SetDiffContext(&ctxtTmp);

	for (size_t i = 0; i < unmatchedItems.size(); ++i)
	{
		DIFFITEM* pdi0 = unmatchedItems[i];

		if (ctxt.m_pCompareStats)
			ctxt.m_pCompareStats->IncreaseTotalItems();

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
			
			ProcessPair(pdi0, idx0, pdi1, idx1, m_pRenameMoveDetectionExpression.get(), movedItems);
		}
	}
}

static void GroupItemsBySameName(CDiffContext& ctxt, std::vector<DIFFITEM*> parents, MovedItemGroups& movedItemGroups)
{
	std::map<String, std::set<DIFFITEM*>> nameToItemsMap;
	for (auto* parent : parents)
	{
		DIFFITEM* diffpos = ctxt.GetFirstChildDiffPosition(parent);
		while (diffpos != nullptr)
		{
			DIFFITEM& di = ctxt.GetNextSiblingDiffRefPosition(diffpos);
			if (di.movedGroupId == -1 && !di.diffcode.existAll())
			{
				for (int i = 0; i < ctxt.GetCompareDirs(); ++i)
				{
					if (di.diffcode.exists(i))
						nameToItemsMap[di.diffFileInfo[i].filename].insert(&di);
				}
			}
		}
	}
	for (auto& [name, items] : nameToItemsMap)
	{
		if (items.size() < 2)
			continue;
		movedItemGroups.emplace_back();
		int movedGroupId = static_cast<int>(movedItemGroups.size() - 1);
		for (auto* di : items)
		{
			di->movedGroupId = movedGroupId;
			for (int i = 0; i < ctxt.GetCompareDirs(); ++i)
			{
				if (!di->diffcode.exists(i))
					continue;
				movedItemGroups[movedGroupId][i].push_back(di);
			}
		}
	}
}

void RenameMoveDetection::DetectRenamedItems(CDiffContext& ctxt, std::vector<DIFFITEM*> parents, MovedItemGroups& movedItemGroups)
{
	// Group items by same name under the given parents
	if (parents.size() >= 2)
		GroupItemsBySameName(ctxt, parents, movedItemGroups);

	// Collect unmatched items under the given parents
	std::vector<DIFFITEM*> unmatchedItems;
	for (auto* parent : parents)
	{
		DIFFITEM* diffpos = ctxt.GetFirstChildDiffPosition(parent);
		while (diffpos != nullptr)
		{
			DIFFITEM& di = ctxt.GetNextSiblingDiffRefPosition(diffpos);
			if (di.movedGroupId == -1 && !di.diffcode.existAll())
				unmatchedItems.push_back(&di);
		}
	}

	// Detect renamed items between side 0 and 1
	DetectMovedItemsBetweenSides(unmatchedItems, 0, 1, ctxt, movedItemGroups);

	// For 3-way comparison, check additional side pairs
	if (ctxt.GetCompareDirs() > 2)
	{
		// Detect renamed items between side 1 and 2
		DetectMovedItemsBetweenSides(unmatchedItems, 1, 2, ctxt, movedItemGroups);

		// Detect renamed items between side 0 and 2
		DetectMovedItemsBetweenSides(unmatchedItems, 0, 2, ctxt, movedItemGroups);
	}

	// Recurse into subdirectories
	DIFFITEM* diffpos = ctxt.GetFirstChildDiffPosition(parents[0]);
	while (diffpos != nullptr)
	{
		DIFFITEM& di = ctxt.GetNextSiblingDiffRefPosition(diffpos);
		if (di.diffcode.isDirectory() && di.diffcode.existAll())
		{
			std::vector<DIFFITEM*> nextParents;
			nextParents.push_back(&di);
			DetectRenamedItems(ctxt, nextParents, movedItemGroups);
		}
	}

	// Recurse into renamed directories
	for (auto* pdi : unmatchedItems)
	{
		if (!pdi->diffcode.isDirectory() || pdi->movedGroupId == -1)
			continue;
		
		std::set<DIFFITEM*> nextParents;
		nextParents.insert(pdi);
		auto movedItemGroup = movedItemGroups[pdi->movedGroupId];
		for (int i = 0; i < ctxt.GetCompareDirs(); ++i)
		{
			auto& sideItems = movedItemGroup[i];
			if (sideItems.size() != 1)
				continue;
			nextParents.insert(const_cast<DIFFITEM*>(sideItems[0]));
		}
		if (nextParents.size() >= 2)
		{
			std::vector<DIFFITEM*> nextParentsVec(nextParents.begin(), nextParents.end());
			DetectRenamedItems(ctxt, nextParentsVec, movedItemGroups);
		}
	}
}

void RenameMoveDetection::Detect(CDiffContext& ctxt, bool doMoveDetection)
{
	if (!m_pRenameMoveDetectionExpression)
		return;

	const int totalItems = ctxt.m_pCompareStats ? ctxt.m_pCompareStats->GetTotalItems() : 0;

	std::vector<DIFFITEM*> rootParents = { nullptr };
	// First, detect renamed items under the root
	DetectRenamedItems(ctxt, rootParents, m_movedItemGroups);

	if (doMoveDetection)
	{
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
		DetectMovedItemsBetweenSides(unmatchedItems, 0, 1, ctxt, m_movedItemGroups);

		// For 3-way comparison, check additional side pairs
		if (ctxt.GetCompareDirs() > 2)
		{
			// Detect moved items between side 1 and 2
			DetectMovedItemsBetweenSides(unmatchedItems, 1, 2, ctxt, m_movedItemGroups);

			// Detect moved items between side 0 and 2
			DetectMovedItemsBetweenSides(unmatchedItems, 0, 2, ctxt, m_movedItemGroups);
		}
	}

	// Restore the total items count
	if (ctxt.m_pCompareStats)
		ctxt.m_pCompareStats->IncreaseTotalItems(totalItems - ctxt.m_pCompareStats->GetTotalItems());
}

void RenameMoveDetection::Merge(CDiffContext& ctxt)
{
	if (m_movedItemGroups.empty())
		return;

	std::set<DIFFITEM*> itemsToDelete;
	const int nDirs = ctxt.GetCompareDirs();
	DIFFITEM* diffpos = ctxt.GetFirstDiffPosition();
	while (diffpos != nullptr)
	{
		DIFFITEM& di = ctxt.GetNextDiffRefPosition(diffpos);
		if (di.movedGroupId != -1 && itemsToDelete.find(&di) == itemsToDelete.end())
		{
			auto& movedItemGroup = m_movedItemGroups[di.movedGroupId];
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
				di.movedGroupId = -1;
				itemsToDelete.insert(pdi2);
			}
		}
	}
	for (DIFFITEM* pdi : itemsToDelete)
	{
		pdi->DelinkFromSiblings();
		auto& movedItemGroup = m_movedItemGroups[pdi->movedGroupId];
		for (int i = 0; i < nDirs; ++i)
		{
			auto& vec = movedItemGroup[i];
			vec.erase(std::remove(vec.begin(), vec.end(), pdi), vec.end());
		}
		delete pdi;
	}

	// Update total items count
	if (ctxt.m_pCompareStats)
		ctxt.m_pCompareStats->IncreaseTotalItems(-static_cast<int>(itemsToDelete.size()));

}

std::vector<const DIFFITEM*> RenameMoveDetection::GetMovedGroupItemsForSide(const CDiffContext& ctxt, const DIFFITEM& di, int sideIndex) const
{
	std::vector<const DIFFITEM*> items;

	if (di.movedGroupId == -1)
		return items;
	
	if (di.movedGroupId >= static_cast<int>(m_movedItemGroups.size()))
		return items;
	
	const auto& movedItemGroup = m_movedItemGroups[di.movedGroupId];
	
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

void RenameMoveDetection::CheckMovedOrRenamed(const CDiffContext& ctxt, const DIFFITEM& di, bool& moved, bool& renamed) const
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
