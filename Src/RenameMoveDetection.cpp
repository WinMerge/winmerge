/**
 * @file RenameMoveDetection.cpp
 * @brief Detects renamed and moved files/folders in folder comparison
 */

#include "pch.h"
#include "RenameMoveDetection.h"
#include "DiffItem.h"
#include "DiffContext.h"
#include "FilterEngine/FilterExpression.h"
#include "CompareStats.h"
#include <set>

/**
 * @brief Transfer file info and properties from source to destination DIFFITEM
 * @param dst Destination DIFFITEM to transfer data into
 * @param dstindex Destination side index (0, 1, or 2 for left/middle/right)
 * @param src Source DIFFITEM to transfer data from
 * @param srcindex Source side index
 * @note This function copies basic file information and moves additional properties
 */
static void TransferDiffItemData(DIFFITEM& dst, int dstindex, DIFFITEM& src, int srcindex)
{
	// Copy basic file information
	dst.diffFileInfo[dstindex].filename = src.diffFileInfo[srcindex].filename;
	dst.diffFileInfo[dstindex].path = src.diffFileInfo[srcindex].path;
	dst.diffFileInfo[dstindex].size = src.diffFileInfo[srcindex].size;
	dst.diffFileInfo[dstindex].flags = src.diffFileInfo[srcindex].flags;
	dst.diffFileInfo[dstindex].ctime = src.diffFileInfo[srcindex].ctime;
	dst.diffFileInfo[dstindex].mtime = src.diffFileInfo[srcindex].mtime;
	dst.diffFileInfo[dstindex].version = src.diffFileInfo[srcindex].version;
	dst.diffFileInfo[dstindex].encoding = src.diffFileInfo[srcindex].encoding;
	dst.diffFileInfo[dstindex].m_textStats = src.diffFileInfo[srcindex].m_textStats;

	// Move additional properties (using move semantics for efficiency)
	dst.diffFileInfo[dstindex].m_pAdditionalProperties = std::move(src.diffFileInfo[srcindex].m_pAdditionalProperties);
}

/**
 * @brief Count how many sides have at least one item
 * @param items Items to check
 * @param nDirs Number of comparison directories
 * @return Number of sides that have at least one item
 */
template<typename Container>
static int CountSidesWithItems(const Container& items, int nDirs)
{
	bool sideHasItem[3] = { false, false, false };

	for (auto* di : items)
	{
		for (int side = 0; side < nDirs; ++side)
		{
			if (di->diffcode.exists(side))
				sideHasItem[side] = true;
		}
	}

	int sideCount = 0;
	for (int side = 0; side < nDirs; ++side)
	{
		if (sideHasItem[side])
			++sideCount;
	}

	return sideCount;
}

/**
 * @brief Collect unmatched items and group them by detection keys
 * @param ctxt Diff context
 * @param diffpos Starting position for iteration
 * @param keyExpression Expression to evaluate keys for matching
 * @param unmatchedFiles Output: map of file keys to items
 * @param unmatchedDirs Output: map of directory keys to items
 * @param iterateChildren If true, iterate children; if false, iterate all items
 */
static void CollectUnmatchedItemsByKey(
	CDiffContext& ctxt,
	DIFFITEM* diffpos,
	FilterExpression* keyExpression,
	std::map<String, std::set<DIFFITEM*>>& unmatchedFiles,
	std::map<String, std::set<DIFFITEM*>>& unmatchedDirs,
	bool iterateChildren)
{
	while (diffpos != nullptr)
	{
		DIFFITEM& di = iterateChildren
			? ctxt.GetNextSiblingDiffRefPosition(diffpos)
			: ctxt.GetNextDiffRefPosition(diffpos);

		if (di.renameMoveGroupId == -1 && !di.diffcode.existAll())
		{
			std::vector<String> keys = keyExpression->EvaluateKeys(di);
			if (keys.size() == ctxt.GetCompareDirs())
			{
				for (int i = 0; i < static_cast<int>(keys.size()); ++i)
				{
					if (di.diffcode.exists(i))
					{
						if (di.diffcode.isDirectory())
							unmatchedDirs[keys[i]].insert(&di);
						else
							unmatchedFiles[keys[i]].insert(&di);
					}
				}
			}
		}
	}
}

/**
 * @brief Create rename/move groups from items with matching keys
 * @param nDirs Number of comparison directories
 * @param itemMap Map of detection keys to items
 * @param renameMoveItemGroups Output: groups of matched items
 */
template<typename Container>
static void CreateGroupsFromMatchedItems(
	int nDirs,
	const std::map<String, Container>& itemMap,
	RenameMoveItemGroups& renameMoveItemGroups)
{
	for (const auto& [key, items] : itemMap)
	{
		// Only create groups for items that exist on at least 2 sides
		if (items.size() < 2 || CountSidesWithItems(items, nDirs) < 2)
			continue;

		renameMoveItemGroups.emplace_back();
		int renameMoveGroupId = static_cast<int>(renameMoveItemGroups.size() - 1);
		for (auto* di : items)
		{
			di->renameMoveGroupId = renameMoveGroupId;
			renameMoveItemGroups[renameMoveGroupId].insert(di);
		}
	}
}

/**
 * @brief Organize items by which side they exist on
 * @param items Items to organize
 * @param nDirs Number of comparison directories
 * @return Array of vectors, one per side, containing items that exist on that side
 */
static std::array<std::vector<DIFFITEM*>, 3> OrganizeItemsBySide(
	const std::set<DIFFITEM*>& items,
	int nDirs)
{
	std::array<std::vector<DIFFITEM*>, 3> sideItems;
	for (auto* pdi : items)
	{
		for (int i = 0; i < nDirs; ++i)
		{
			if (pdi->diffcode.exists(i))
				sideItems[i].push_back(pdi);
		}
	}
	return sideItems;
}

/**
 * @brief Group items with same filename in same directory
 * @param ctxt Diff context
 * @param parents Parent items to search within
 * @param renameMoveItemGroups Output: groups of items that should be matched
 * @note This handles the simple case where files are renamed but stay in the same directory
 */
static void GroupItemsBySameName(CDiffContext& ctxt, std::vector<DIFFITEM*> parents, RenameMoveItemGroups& renameMoveItemGroups)
{
	const int nDirs = ctxt.GetCompareDirs();

	// Build map: filename -> items with that name
	std::map<String, std::set<DIFFITEM*>> nameToItemsMap;
	for (auto* parent : parents)
	{
		DIFFITEM* diffpos = ctxt.GetFirstChildDiffPosition(parent);
		while (diffpos != nullptr)
		{
			DIFFITEM& di = ctxt.GetNextSiblingDiffRefPosition(diffpos);
			// Only process unmatched items (not already in a group and not existing on all sides)
			if (di.renameMoveGroupId == -1 && !di.diffcode.existAll())
			{
				for (int i = 0; i < nDirs; ++i)
				{
					if (di.diffcode.exists(i))
						nameToItemsMap[di.diffFileInfo[i].filename].insert(&di);
				}
			}
		}
	}

	// Create groups for items with same name
	CreateGroupsFromMatchedItems(nDirs, nameToItemsMap, renameMoveItemGroups);
}

/**
 * @brief Set the filter expression used for rename/move detection
 * @param expr Filter expression that generates keys for matching items across sides
 */
void RenameMoveDetection::SetRenameMoveKeyExpression(const FilterExpression* expr)
{
	m_pRenameMoveKeyExpression.reset(expr ? new FilterExpression(*expr) : nullptr);
}

/**
 * @brief Recursively detect renamed items under parent directories
 * @param ctxt Diff context
 * @param parents Parent directories to search within
 * @param renameMoveItemGroups Output: groups of items to be matched
 *
 * This function performs rename detection in multiple steps:
 * 1. Quick match - group items with identical filenames
 * 2. Key-based match - group items with matching detection keys (e.g., content hash)
 * 3. Recurse into common subdirectories
 * 4. Recurse into renamed directories
 */
void RenameMoveDetection::DetectRenamedItems(CDiffContext& ctxt, std::vector<DIFFITEM*> parents, RenameMoveItemGroups& renameMoveItemGroups)
{
	// Step 1: Quick win - group items by same name
	if (parents.size() >= 2)
		GroupItemsBySameName(ctxt, parents, renameMoveItemGroups);

	// Step 2: Collect unmatched items grouped by rename/move detection keys
	std::map<String, std::set<DIFFITEM*>> unmatchedFiles;
	std::map<String, std::set<DIFFITEM*>> unmatchedDirs;
	for (auto* parent : parents)
	{
		DIFFITEM* diffpos = ctxt.GetFirstChildDiffPosition(parent);
		CollectUnmatchedItemsByKey(ctxt, diffpos, m_pRenameMoveKeyExpression.get(),
			unmatchedFiles, unmatchedDirs, true);
	}

	// Step 3: Create groups for items with matching keys
	CreateGroupsFromMatchedItems(ctxt.GetCompareDirs(), unmatchedFiles, renameMoveItemGroups);
	CreateGroupsFromMatchedItems(ctxt.GetCompareDirs(), unmatchedDirs, renameMoveItemGroups);

	// Step 4: Recurse into subdirectories that exist on all sides
	DIFFITEM* diffpos = ctxt.GetFirstChildDiffPosition(parents[0]);
	while (diffpos != nullptr)
	{
		DIFFITEM& di = ctxt.GetNextSiblingDiffRefPosition(diffpos);
		if (di.diffcode.isDirectory() && di.diffcode.existAll())
		{
			std::vector<DIFFITEM*> nextParents = { &di };
			DetectRenamedItems(ctxt, nextParents, renameMoveItemGroups);
		}
	}

	// Step 5: Recurse into renamed directories
	for (auto& [key, items] : unmatchedDirs)
	{
		// Group directory items by side
		auto sideItems = OrganizeItemsBySide(items, ctxt.GetCompareDirs());

		// Only recurse if we have exactly one directory per side
		std::set<DIFFITEM*> nextParents;
		for (int i = 0; i < ctxt.GetCompareDirs(); ++i)
		{
			if (sideItems[i].size() == 1)
				nextParents.insert(sideItems[i][0]);
		}

		// Need at least 2 sides to compare
		if (nextParents.size() >= 2)
		{
			std::vector<DIFFITEM*> nextParentsVec(nextParents.begin(), nextParents.end());
			DetectRenamedItems(ctxt, nextParentsVec, renameMoveItemGroups);
		}
	}
}

/**
 * @brief Main detection entry point - detects renamed and moved items
 * @param ctxt Diff context containing items to analyze
 * @param doMoveDetection If true, also detect items moved to different directories
 *
 * Detection is performed in two phases:
 * Phase 1: Detect renamed items (same directory, different name) recursively
 * Phase 2: Detect moved items (different directory) if doMoveDetection is true
 */
void RenameMoveDetection::Detect(CDiffContext& ctxt, bool doMoveDetection)
{
	if (!m_pRenameMoveKeyExpression || m_pRenameMoveKeyExpression->errorCode != 0)
		return;

	// Save item count to restore later (we temporarily modify it during detection)
	const int totalItems = ctxt.m_pCompareStats ? ctxt.m_pCompareStats->GetTotalItems() : 0;

	// Phase 1: Detect renamed items (same dir, different name)
	std::vector<DIFFITEM*> rootParents = { nullptr };
	DetectRenamedItems(ctxt, rootParents, m_renameMoveItemGroups);

	// Phase 2: Detect moved items (different dir)
	if (doMoveDetection)
	{
		// Collect all remaining unmatched items across entire tree
		std::map<String, std::set<DIFFITEM*>> unmatchedFiles;
		std::map<String, std::set<DIFFITEM*>> unmatchedDirs;
		DIFFITEM* diffpos = ctxt.GetFirstDiffPosition();
		CollectUnmatchedItemsByKey(ctxt, diffpos, m_pRenameMoveKeyExpression.get(),
			unmatchedFiles, unmatchedDirs, false);

		// Create groups for moved items
		CreateGroupsFromMatchedItems(ctxt.GetCompareDirs(), unmatchedFiles, m_renameMoveItemGroups);
		CreateGroupsFromMatchedItems(ctxt.GetCompareDirs(), unmatchedDirs, m_renameMoveItemGroups);
	}

	// Restore item count
	if (ctxt.m_pCompareStats)
		ctxt.m_pCompareStats->IncreaseTotalItems(totalItems - ctxt.m_pCompareStats->GetTotalItems());
}

/**
 * @brief Merge grouped items into single diff items where possible
 * @param ctxt Diff context
 *
 * After detection, items in the same group that are in the same directory
 * can be merged into a single DIFFITEM representing the same logical entity
 * on different sides. This simplifies the comparison tree.
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
			auto& renameMoveItemGroup = m_renameMoveItemGroups[di.renameMoveGroupId];

			// Organize group items by side
			auto sideItems = OrganizeItemsBySide(renameMoveItemGroup, nDirs);

			// Can only merge if each side has at most one item
			bool canMerge = std::all_of(sideItems.begin(), sideItems.begin() + nDirs,
				[](const std::vector<DIFFITEM*>& items) { return items.size() <= 1; });

			if (!canMerge)
				continue;

			// Try to merge items from other sides into this item
			for (int i = 0; i < nDirs; ++i)
			{
				// Skip sides that already exist in this item or have no items
				if (di.diffcode.exists(i) || sideItems[i].empty())
					continue;

				auto* pdi2 = sideItems[i][0];
				// Only merge items in same directory
				if (di.GetParentLink() != pdi2->GetParentLink())
					continue;

				// Reparent children before deleting the merged item
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

				// Transfer data from item being merged
				TransferDiffItemData(di, i, *pdi2, i);
				di.diffcode.setSideFlag(i);
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
		m_renameMoveItemGroups[pdi->renameMoveGroupId].erase(pdi);
		// Free memory
		delete pdi;
	}

	for (auto& group : m_renameMoveItemGroups)
	{
		if (group.size() == 1)
		{
			(*group.begin())->renameMoveGroupId = -1;
			group.clear();
		}
	}

	// --- Pass 3: Remove empty groups and renumber renameMoveGroupId ---
	RenameMoveItemGroups newGroups;
	newGroups.reserve(m_renameMoveItemGroups.size());

	for (auto& group : m_renameMoveItemGroups)
	{
		if (group.empty())
			continue;

		int newId = static_cast<int>(newGroups.size());
		for (auto* di : group)
		{
			di->renameMoveGroupId = newId;
		}
		newGroups.push_back(std::move(group));
	}

	m_renameMoveItemGroups.swap(newGroups);

	// Update total items count to reflect deletions
	if (ctxt.m_pCompareStats)
		ctxt.m_pCompareStats->IncreaseTotalItems(-static_cast<int>(itemsToDelete.size()));
}

/**
 * @brief Get all items in a rename/move group for specified side
 * @param ctxt Diff context
 * @param di The item whose group to query
 * @param sideIndex Side index (0=left, 1=middle, 2=right)
 * @return Vector of items in the same group that exist on the specified side
 */
std::vector<const DIFFITEM*> RenameMoveDetection::GetRenameMoveGroupItemsForSide(const DIFFITEM& di, int sideIndex) const
{
	std::vector<const DIFFITEM*> items;

	// Not in a moved group
	if (di.renameMoveGroupId < 0 ||
	    di.renameMoveGroupId >= static_cast<int>(m_renameMoveItemGroups.size()))
		return items;

	const auto& renameMoveItemGroup = m_renameMoveItemGroups[di.renameMoveGroupId];

	// Return this item if it exists on requested side
	if (di.diffcode.exists(sideIndex))
	{
		items.push_back(&di);
	}
	else
	{
		// Otherwise find other items in group on this side
		for (auto pdi2 : renameMoveItemGroup)
		{
			if (pdi2->diffcode.exists(sideIndex))
				items.push_back(pdi2);
		}
	}
	return items;
}

/**
 * @brief Check if item is moved and/or renamed
 * @param ctxt Diff context
 * @param di Item to check
 * @param[out] moved Set to true if item was moved to different directory
 * @param[out] renamed Set to true if item has different names on different sides
 *
 * An item can be both moved and renamed (e.g., file "a.txt" in dir1 becomes "b.txt" in dir2)
 */
void RenameMoveDetection::CheckMovedOrRenamed(const CDiffContext& ctxt, const DIFFITEM& di, bool& moved, bool& renamed) const
{
	const int nDirs = ctxt.GetCompareDirs();
	std::vector<std::vector<const DIFFITEM*>> sideItems(nDirs);

	// Collect group items for each side
	for (int side = 0; side < nDirs; ++side)
		sideItems[side] = GetRenameMoveGroupItemsForSide(di, side);

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

					// Renamed: different filename
					if (a->diffFileInfo[i].filename != b->diffFileInfo[j].filename)
						renamed = true;

					// Moved: different parent directory
					if (a->GetParentLink() != b->GetParentLink())
						moved = true;
				}
			}
		}
	}
}