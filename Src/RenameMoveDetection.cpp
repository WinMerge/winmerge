/**
 *  @file RenameMoveDetection.cpp
 *  @brief Detects renamed and moved files/folders in folder comparison
 */

#include "pch.h"
#include "RenameMoveDetection.h"
#include "DiffItem.h"
#include "DiffContext.h"
#include "FilterEngine/FilterExpression.h"
#include "CompareStats.h"
#include <set>

/**
 * @brief Copy file info from source to destination DIFFITEM
 */
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

/**
 * @brief Move additional properties from source to destination
 */
static void MoveDiffItemPartially(DIFFITEM& dst, int dstindex, DIFFITEM& src, int srcindex)
{
	dst.diffFileInfo[dstindex].m_pAdditionalProperties = std::move(src.diffFileInfo[srcindex].m_pAdditionalProperties);
}

/**
 * @brief Add matching pair to renamed/moved item groups
 */
static void ProcessPair(DIFFITEM* pdi0, int idx0, DIFFITEM* pdi1, int idx1, RenameMoveItemGroups& renameMoveItemGroups)
{
	// Case 1: pdi0 already grouped, add pdi1
	if (pdi0->renameMoveGroupId != -1 && pdi1->renameMoveGroupId == -1)
	{
		int existingGroupId = pdi0->renameMoveGroupId;
		pdi1->renameMoveGroupId = existingGroupId;
		renameMoveItemGroups[existingGroupId][idx1].push_back(pdi1);
	}
	// Case 2: pdi1 already grouped, add pdi0
	else if (pdi0->renameMoveGroupId == -1 && pdi1->renameMoveGroupId != -1)
	{
		int existingGroupId = pdi1->renameMoveGroupId;
		pdi0->renameMoveGroupId = existingGroupId;
		renameMoveItemGroups[existingGroupId][idx0].push_back(pdi0);
	}
	// Case 3: Neither grouped, create new group
	else if (pdi0->renameMoveGroupId == -1 && pdi1->renameMoveGroupId == -1)
	{
		renameMoveItemGroups.emplace_back();
		int renameMoveGroupId = static_cast<int>(renameMoveItemGroups.size() - 1);
		pdi0->renameMoveGroupId = renameMoveGroupId;
		pdi1->renameMoveGroupId = renameMoveGroupId;
		renameMoveItemGroups[renameMoveGroupId][idx0].push_back(pdi0);
		renameMoveItemGroups[renameMoveGroupId][idx1].push_back(pdi1);
	}
	// Case 4: Both already grouped (should be same group)
	else if (pdi0->renameMoveGroupId != -1 && pdi1->renameMoveGroupId != -1)
	{
		int groupA = pdi0->renameMoveGroupId;
		int groupB = pdi1->renameMoveGroupId;

		// Already same group -> nothing to do
		if (groupA == groupB)
			return;

		// Merge groupB into groupA (policy: keep smaller ID)
		if (groupA > groupB)
			std::swap(groupA, groupB);

		auto& dstGroup = renameMoveItemGroups[groupA];
		auto& srcGroup = renameMoveItemGroups[groupB];

		// Move all items from srcGroup to dstGroup
		for (size_t side = 0; side < srcGroup.size(); ++side)
		{
			for (DIFFITEM* pdi : srcGroup[side])
			{
				pdi->renameMoveGroupId = groupA;
				dstGroup[side].push_back(pdi);
			}
			srcGroup[side].clear();
		}

		// Note:
		// groupB entry remains empty; do not erase indices to keep IDs stable
	}
}

RenameMoveDetection::RenameMoveDetection()
{
}

RenameMoveDetection::~RenameMoveDetection()
{
}

void RenameMoveDetection::SetRenameMoveKeyExpression(const FilterExpression* expr)
{
	m_pRenameMoveKeyExpression.reset(expr ? new FilterExpression(*expr) : nullptr);
}

/**
 * @brief Detect renamed/moved items between two sides (O(n^2) algorithm)
 */
void RenameMoveDetection::DetectRenameMoveItemsBetweenSides(
	const std::vector<DIFFITEM*>& unmatchedItems, int side0, int side1, CDiffContext& ctxt, RenameMoveItemGroups& movedItems)
{
	m_pRenameMoveKeyExpression->SetDiffContext(&ctxt);

	// Compare each pair of items
	for (size_t i = 0; i < unmatchedItems.size(); ++i)
	{
		DIFFITEM* pdi0 = unmatchedItems[i];

		if (ctxt.m_pCompareStats)
			ctxt.m_pCompareStats->IncreaseTotalItems();

		// Skip if item doesn't exist on either side
		if (!pdi0->diffcode.exists(side0) && !pdi0->diffcode.exists(side1))
			continue;
		
		const int idx0 = pdi0->diffcode.exists(side0) ? side0 : side1;
		const bool pdi0IsFolder = pdi0->diffcode.isDirectory();
		const bool pdi0ExistsSide0 = pdi0->diffcode.exists(side0);
		const bool pdi0ExistsSide1 = pdi0->diffcode.exists(side1);
		
		// Compare with subsequent items
		for (size_t j = i + 1; j < unmatchedItems.size(); ++j)
		{
			if (ctxt.ShouldAbort())
				return;
			
			DIFFITEM* pdi1 = unmatchedItems[j];
			const int idx1 = pdi1->diffcode.exists(side0) ? side0 : side1;

			// Skip if both on same side
			if (idx0 == idx1)
				continue;
			
			// Skip if same item or type mismatch (file vs folder)
			if (pdi0 == pdi1 || pdi0IsFolder != pdi1->diffcode.isDirectory())
				continue;

			// Skip invalid combinations (both on same side)
			if ((!pdi1->diffcode.exists(side0) && !pdi1->diffcode.exists(side1)) ||
			    ( pdi0ExistsSide0              &&  pdi1->diffcode.exists(side0)) ||
			    ( pdi1->diffcode.exists(side1) &&  pdi0ExistsSide1            ))
				continue;
			
			ProcessPair(pdi0, idx0, pdi1, idx1, movedItems);
		}
	}
}

/**
 * @brief Group items with same name in same directory
 */
static void GroupItemsBySameName(CDiffContext& ctxt, std::vector<DIFFITEM*> parents, RenameMoveItemGroups& renameMoveItemGroups)
{
	// Build map: filename -> items with that name
	std::map<String, std::set<DIFFITEM*>> nameToItemsMap;
	for (auto* parent : parents)
	{
		DIFFITEM* diffpos = ctxt.GetFirstChildDiffPosition(parent);
		while (diffpos != nullptr)
		{
			DIFFITEM& di = ctxt.GetNextSiblingDiffRefPosition(diffpos);
			// Only unmatched items
			if (di.renameMoveGroupId == -1 && !di.diffcode.existAll())
			{
				for (int i = 0; i < ctxt.GetCompareDirs(); ++i)
				{
					if (di.diffcode.exists(i))
						nameToItemsMap[di.diffFileInfo[i].filename].insert(&di);
				}
			}
		}
	}
	// Create groups for items with same name
	for (auto& [name, items] : nameToItemsMap)
	{
		if (items.size() < 2)
			continue;
		renameMoveItemGroups.emplace_back();
		int renameMoveGroupId = static_cast<int>(renameMoveItemGroups.size() - 1);
		for (auto* di : items)
		{
			di->renameMoveGroupId = renameMoveGroupId;
			for (int i = 0; i < ctxt.GetCompareDirs(); ++i)
			{
				if (!di->diffcode.exists(i))
					continue;
				renameMoveItemGroups[renameMoveGroupId][i].push_back(di);
			}
		}
	}
}

/**
 * @brief Recursively detect renamed items under parent directories
 */
void RenameMoveDetection::DetectRenamedItems(CDiffContext& ctxt, std::vector<DIFFITEM*> parents, RenameMoveItemGroups& movedItemGroups)
{
	// Step 1: Quick win - group items by same name
	if (parents.size() >= 2)
		GroupItemsBySameName(ctxt, parents, movedItemGroups);

	// Step 2: Collect unmatched items grouped by rename/move detection keys
	std::map<String, std::vector<DIFFITEM*>> unmatchedItems;
	for (auto* parent : parents)
	{
		DIFFITEM* diffpos = ctxt.GetFirstChildDiffPosition(parent);
		while (diffpos != nullptr)
		{
			DIFFITEM& di = ctxt.GetNextSiblingDiffRefPosition(diffpos);
			if (di.renameMoveGroupId == -1 && !di.diffcode.existAll())
			{
				std::vector<String> keys = m_pRenameMoveKeyExpression->EvaluateKeys(di);
				if (keys.size() != ctxt.GetCompareDirs())
					continue;
				for (int i = 0; i < keys.size(); ++i)
				{
					if (di.diffcode.exists(i))
						unmatchedItems[keys[i]].push_back(&di);
				}
			}
		}
	}

	// Step 3: Detect renamed items between sides
	for (auto& [key, items] : unmatchedItems)
		DetectRenameMoveItemsBetweenSides(items, 0, 1, ctxt, movedItemGroups);

	// For 3-way comparison, check additional side pairs
	if (ctxt.GetCompareDirs() > 2)
	{
		for (auto& [key, items] : unmatchedItems)
		{
			DetectRenameMoveItemsBetweenSides(items, 1, 2, ctxt, movedItemGroups);
			DetectRenameMoveItemsBetweenSides(items, 0, 2, ctxt, movedItemGroups);
		}
	}

	// Step 4: Recurse into subdirectories that exist on all sides
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

	// Step 5: Recurse into renamed directories
	for (auto& [key, items] : unmatchedItems)
	{
		for (auto* pdi : items)
		{
			// Only process grouped directories
			if (!pdi->diffcode.isDirectory() || pdi->renameMoveGroupId == -1)
				continue;

			// Collect all instances of this renamed directory
			std::set<DIFFITEM*> nextParents;
			nextParents.insert(pdi);
			auto renameMoveItemGroup = movedItemGroups[pdi->renameMoveGroupId];
			for (int i = 0; i < ctxt.GetCompareDirs(); ++i)
			{
				auto& sideItems = renameMoveItemGroup[i];
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
}

/**
 * @brief Main detection entry point - detects renamed and moved items
 */
void RenameMoveDetection::Detect(CDiffContext& ctxt, bool doMoveDetection)
{
	if (!m_pRenameMoveKeyExpression)
		return;

	// Save item count to restore later (we add items for progress)
	const int totalItems = ctxt.m_pCompareStats ? ctxt.m_pCompareStats->GetTotalItems() : 0;

	// Phase 1: Detect renamed items (same dir, different name)
	std::vector<DIFFITEM*> rootParents = { nullptr };
	DetectRenamedItems(ctxt, rootParents, m_renameMoveItemGroups);

	// Phase 2: Detect moved items (different dir)
	if (doMoveDetection)
	{
		// Collect all remaining unmatched items
		std::map<String, std::vector<DIFFITEM*>> unmatchedItems;
		DIFFITEM* diffpos = ctxt.GetFirstDiffPosition();
		while (diffpos != nullptr)
		{
			DIFFITEM& di = ctxt.GetNextDiffRefPosition(diffpos);
			if (di.renameMoveGroupId == -1 && !di.diffcode.existAll())
			{
				std::vector<String> keys = m_pRenameMoveKeyExpression->EvaluateKeys(di);
				if (keys.size() != ctxt.GetCompareDirs())
					continue;
				for (int i = 0; i < keys.size(); ++i)
				{
					if (di.diffcode.exists(i))
						unmatchedItems[keys[i]].push_back(&di);
				}
			}
		}

		for (auto& [key, items] : unmatchedItems)
			DetectRenameMoveItemsBetweenSides(items, 0, 1, ctxt, m_renameMoveItemGroups);

		// For 3-way comparison, check additional side pairs
		if (ctxt.GetCompareDirs() > 2)
		{
			for (auto& [key, items] : unmatchedItems)
			{
				DetectRenameMoveItemsBetweenSides(items, 1, 2, ctxt, m_renameMoveItemGroups);
				DetectRenameMoveItemsBetweenSides(items, 0, 2, ctxt, m_renameMoveItemGroups);
			}
		}
	}

	// Restore item count
	if (ctxt.m_pCompareStats)
		ctxt.m_pCompareStats->IncreaseTotalItems(totalItems - ctxt.m_pCompareStats->GetTotalItems());
}

/**
 * @brief Merge grouped items into single diff items where possible
 */
void RenameMoveDetection::Merge(CDiffContext& ctxt)
{
	if (m_renameMoveItemGroups.empty())
		return;

	std::set<DIFFITEM*> itemsToDelete;
	const int nDirs = ctxt.GetCompareDirs();
	// Pass 1: Find items to merge
	DIFFITEM* diffpos = ctxt.GetFirstDiffPosition();
	while (diffpos != nullptr)
	{
		DIFFITEM& di = ctxt.GetNextDiffRefPosition(diffpos);
		if (di.renameMoveGroupId != -1 && itemsToDelete.find(&di) == itemsToDelete.end())
		{
			auto& movedItemGroup = m_renameMoveItemGroups[di.renameMoveGroupId];
			for (int i = 0; i < nDirs; ++i)
			{
				if (di.diffcode.exists(i))
					continue;
				if (movedItemGroup[i].size() != 1)
					continue;
				auto pdi2 = movedItemGroup[i][0];
				// Only merge items in same directory
				if (di.GetParentLink() != pdi2->GetParentLink())
					continue;
				// Reparent children before deleting
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
				// Copy data from item being merged
				CopyDiffItemPartially(di, i, *pdi2, i);
				MoveDiffItemPartially(di, i, *pdi2, i);
				di.diffcode.setSideFlag(i);
				di.renameMoveGroupId = -1;
				itemsToDelete.insert(pdi2);
			}
		}
	}
	// Pass 2: Delete merged items
	for (DIFFITEM* pdi : itemsToDelete)
	{
		// Remove from tree
		pdi->DelinkFromSiblings();
		// Remove from groups
		auto& renameMoveItemGroup = m_renameMoveItemGroups[pdi->renameMoveGroupId];
		for (int i = 0; i < nDirs; ++i)
		{
			auto& vec = renameMoveItemGroup[i];
			vec.erase(std::remove(vec.begin(), vec.end(), pdi), vec.end());
		}
		// Free memory
		delete pdi;
	}

	// Update total items count
	if (ctxt.m_pCompareStats)
		ctxt.m_pCompareStats->IncreaseTotalItems(-static_cast<int>(itemsToDelete.size()));

}

/**
 * @brief Get all items in a moved group for specified side
 */
std::vector<const DIFFITEM*> RenameMoveDetection::GetRenameMoveGroupItemsForSide(const CDiffContext& ctxt, const DIFFITEM& di, int sideIndex) const
{
	std::vector<const DIFFITEM*> items;

	// Not in a moved group
	if (di.renameMoveGroupId == -1)
		return items;
	
	if (di.renameMoveGroupId >= static_cast<int>(m_renameMoveItemGroups.size()))
		return items;
	
	const auto& movedItemGroup = m_renameMoveItemGroups[di.renameMoveGroupId];
	
	// Return this item if it exists on requested side
	if (di.diffcode.exists(sideIndex))
	{
		items.push_back(&di);
	}
	else
	{
		// Find all items in group that exist on requested side
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

/**
 * @brief Check if item is moved and/or renamed
 */
void RenameMoveDetection::CheckMovedOrRenamed(const CDiffContext& ctxt, const DIFFITEM& di, bool& moved, bool& renamed) const
{
	const int nDirs = ctxt.GetCompareDirs();
	std::vector<std::vector<const DIFFITEM*>> sideItems(nDirs);

	// Collect group items for each side
	for (int side = 0; side < nDirs; ++side)
		sideItems[side] = GetRenameMoveGroupItemsForSide(ctxt, di, side);

	moved = false;
	renamed = false;

	// Compare all side pairs to detect moves/renames
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
					
					// Renamed: different name
					if (a->diffFileInfo[i].filename != b->diffFileInfo[j].filename)
						renamed = true;
					
					// Moved: different directory
					if (a->GetParentLink() != b->GetParentLink())
						moved = true;
				}
			}
		}
	}
}
