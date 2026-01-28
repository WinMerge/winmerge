/** 
 * @file RenameMoveDetection.h
 * @brief Detects renamed and moved files/folders in folder comparison
 */
#pragma once

#include <vector>
#include <set>
#include <memory>

class CDiffContext;
class DIFFITEM;
struct FilterExpression;

using RenameMoveItemGroups = std::vector<std::set<DIFFITEM*>>;

class RenameMoveDetection
{
public:
	RenameMoveDetection() = default;
	~RenameMoveDetection() = default;

	FilterExpression* GetRenameMoveKeyExpression() const { return m_pRenameMoveKeyExpression.get(); }
	void SetRenameMoveKeyExpression(const FilterExpression* expr);
	void Detect(CDiffContext& ctxt, bool doMoveDetection);
	void Merge(CDiffContext& ctxt);
	const RenameMoveItemGroups& GetRenameMoveItemGroups() const { return m_renameMoveItemGroups; }
	std::vector<const DIFFITEM*> GetRenameMoveGroupItemsForSide(const DIFFITEM& di, int sideIndex) const;
	void CheckMovedOrRenamed(const CDiffContext& ctxt, const DIFFITEM& di, bool& moved, bool& renamed) const;
	void RemoveItemFromGroup(DIFFITEM* pdi);

private:
	void DetectRenamedItems(CDiffContext& ctxt, std::vector<DIFFITEM*> parents, RenameMoveItemGroups& movedItemGroups);

	std::unique_ptr<FilterExpression> m_pRenameMoveKeyExpression; /** Filter expression for generating matching keys */
	RenameMoveItemGroups m_renameMoveItemGroups; /** Detected groups (index = renameMoveGroupId in DIFFITEM) */
};
