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
  * @param dst Destination DIFFITEM to copy data into
  * @param dstindex Destination side index (0, 1, or 2 for left/middle/right)
  * @param src Source DIFFITEM to copy data from
  * @param srcindex Source side index
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
 * @param dst Destination DIFFITEM
 * @param dstindex Destination side index
 * @param src Source DIFFITEM
 * @param srcindex Source side index
 */
static void MoveDiffItemPartially(DIFFITEM& dst, int dstindex, DIFFITEM& src, int srcindex)
{
	dst.diffFileInfo[dstindex].m_pAdditionalProperties = std::move(src.diffFileInfo[srcindex].m_pAdditionalProperties);
}

RenameMoveDetection::RenameMoveDetection()
{
}

RenameMoveDetection::~RenameMoveDetection()
{
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
 * @brief Group items with same filename in same directory
 * @param ctxt Diff context
 * @param parents Parent items to search within
 * @param renameMoveItemGroups Output: groups of items that should be matched
 * @note This handles the simple case where files are renamed but stay in the same directory
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
			// Only process unmatched items (not already in a group and not existing on all sides)
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
			renameMoveItemGroups[renameMoveGroupId].insert(di);
		}
	}
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
	std::map<String, std::vector<DIFFITEM*>> unmatchedFiles;
	std::map<String, std::vector<DIFFITEM*>> unmatchedDirs;
	for (auto* parent : parents)
	{
		DIFFITEM* diffpos = ctxt.GetFirstChildDiffPosition(parent);
		while (diffpos != nullptr)
		{
			DIFFITEM& di = ctxt.GetNextSiblingDiffRefPosition(diffpos);
			if (di.renameMoveGroupId == -1 && !di.diffcode.existAll())
			{
				// Evaluate filter expression to get keys for each side
				std::vector<String> keys = m_pRenameMoveKeyExpression->EvaluateKeys(di);
				if (keys.size() != ctxt.GetCompareDirs())
					continue;
				for (int i = 0; i < keys.size(); ++i)
				{
					if (di.diffcode.exists(i))
					{
						if (di.diffcode.isDirectory())
							unmatchedDirs[keys[i]].push_back(&di);
						else
							unmatchedFiles[keys[i]].push_back(&di);
					}
				}
			}
		}
	}

	// Step 3: Create groups for items with matching keys
	for (auto& [key, items] : unmatchedFiles)
	{
		if (items.size() < 2)
			continue;
		renameMoveItemGroups.emplace_back();
		int renameMoveGroupId = static_cast<int>(renameMoveItemGroups.size() - 1);
		for (auto* di : items)
		{
			di->renameMoveGroupId = renameMoveGroupId;
			renameMoveItemGroups[renameMoveGroupId].insert(di);
		}
	}
	for (auto& [key, items] : unmatchedDirs)
	{
		if (items.size() < 2)
			continue;
		renameMoveItemGroups.emplace_back();
		int renameMoveGroupId = static_cast<int>(renameMoveItemGroups.size() - 1);
		for (auto* di : items)
		{
			di->renameMoveGroupId = renameMoveGroupId;
			renameMoveItemGroups[renameMoveGroupId].insert(di);
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
			DetectRenamedItems(ctxt, nextParents, renameMoveItemGroups);
		}
	}

	// Step 5: Recurse into renamed directories
	for (auto& [key, items] : unmatchedDirs)
	{
		// Group directory items by side
		std::array<std::vector<DIFFITEM*>, 3> sideItems;
		for (auto* pdi : items)
		{
			for (int i = 0; i < ctxt.GetCompareDirs(); ++i)
			{
				if (pdi->diffcode.exists(i))
					sideItems[i].push_back(pdi);
			}
		}

		// Only recurse if we have exactly one directory per side
		std::set<DIFFITEM*> nextParents;
		for (int i = 0; i < ctxt.GetCompareDirs(); ++i)
		{
			if (sideItems[i].size() != 1)
				continue;
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
	if (!m_pRenameMoveKeyExpression)
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
		std::map<String, std::vector<DIFFITEM*>> unmatchedFiles;
		std::map<String, std::vector<DIFFITEM*>> unmatchedDirs;
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
					{
						if (di.diffcode.isDirectory())
							unmatchedDirs[keys[i]].push_back(&di);
						else
							unmatchedFiles[keys[i]].push_back(&di);
					}
				}
			}
		}

		// Group moved files
		for (auto& [key, items] : unmatchedFiles)
		{
			if (items.size() < 2)
				continue;
			m_renameMoveItemGroups.emplace_back();
			int renameMoveGroupId = static_cast<int>(m_renameMoveItemGroups.size() - 1);
			for (auto* di : items)
			{
				di->renameMoveGroupId = renameMoveGroupId;
				m_renameMoveItemGroups[renameMoveGroupId].insert(di);
			}
		}

		// Group moved directories
		for (auto& [key, items] : unmatchedDirs)
		{
			if (items.size() < 2)
				continue;
			m_renameMoveItemGroups.emplace_back();
			int renameMoveGroupId = static_cast<int>(m_renameMoveItemGroups.size() - 1);
			for (auto* di : items)
			{
				di->renameMoveGroupId = renameMoveGroupId;
				m_renameMoveItemGroups[renameMoveGroupId].insert(di);
			}
		}
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
			std::array<std::vector<DIFFITEM*>, 3> sideItems;
			for (auto* pdi : renameMoveItemGroup)
			{
				for (int i = 0; i < ctxt.GetCompareDirs(); ++i)
				{
					if (pdi->diffcode.exists(i))
						sideItems[i].push_back(pdi);
				}
			}

			// Try to merge items from other sides into this item
			for (int i = 0; i < nDirs; ++i)
			{
				// Skip sides that already exist in this item
				if (di.diffcode.exists(i))
					continue;
				// Can only merge if exactly one item exists on this side
				if (sideItems[i].size() != 1)
					continue;
				auto pdi2 = sideItems[i][0];
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
		m_renameMoveItemGroups[pdi->renameMoveGroupId].erase(pdi);
		// Free memory
		delete pdi;
	}

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
std::vector<const DIFFITEM*> RenameMoveDetection::GetRenameMoveGroupItemsForSide(const CDiffContext& ctxt, const DIFFITEM& di, int sideIndex) const
{
	std::vector<const DIFFITEM*> items;

	// Not in a moved group
	if (di.renameMoveGroupId == -1)
		return items;

	if (di.renameMoveGroupId >= static_cast<int>(m_renameMoveItemGroups.size()))
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