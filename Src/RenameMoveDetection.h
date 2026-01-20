#pragma once

#include <vector>
#include <memory>

class CDiffContext;
class DIFFITEM;
struct FilterExpression;

using RenameMoveItemGroups = std::vector<std::array<std::vector<DIFFITEM*>, 3>>;

class RenameMoveDetection
{
public:
	RenameMoveDetection();
	~RenameMoveDetection();

	FilterExpression* GetRenameMoveKeyExpression() const { return m_pRenameMoveKeyExpression.get(); }
	void SetRenameMoveKeyExpression(const FilterExpression* expr);
	void Detect(CDiffContext& ctxt, bool doMoveDetection);
	void Merge(CDiffContext& ctxt);
	const RenameMoveItemGroups& GetMovedItemGroups() const { return m_renameMoveItemGroups; }
	std::vector<const DIFFITEM*> GetRenameMoveGroupItemsForSide(const CDiffContext& ctxt, const DIFFITEM& di, int sideIndex) const;
	void CheckMovedOrRenamed(const CDiffContext& ctxt, const DIFFITEM& di, bool& moved, bool& renamed) const;

private:
	void DetectRenamedItems(CDiffContext& ctxt, std::vector<DIFFITEM*> parents, RenameMoveItemGroups& movedItemGroups);
	void DetectRenameMoveItemsBetweenSides(const std::vector<DIFFITEM*>& unmatchedItems, int side0, int side1, CDiffContext& ctxt, RenameMoveItemGroups& movedItemGroups);

	std::unique_ptr<FilterExpression> m_pRenameMoveKeyExpression;
	RenameMoveItemGroups m_renameMoveItemGroups;
};
