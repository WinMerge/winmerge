#pragma once

#include <vector>
#include <memory>

class CDiffContext;
class DIFFITEM;
struct FilterExpression;

using MovedItemGroups = std::vector<std::array<std::vector<DIFFITEM*>, 3>>;

class MoveDetection
{
public:
	MoveDetection();
	~MoveDetection();

	FilterExpression* GetMoveDetectionExpression() const { return m_pMoveDetectionExpression.get(); }
	void SetMoveDetectionExpression(const FilterExpression* expr);
	void Detect(CDiffContext& ctxt);
	void MergeMovedItems(CDiffContext& ctxt);
	std::shared_ptr<const MovedItemGroups> GetMovedItemGroups() const { return std::atomic_load(&m_pMovedItemGroups); }
	std::vector<const DIFFITEM*> GetMovedGroupItemsForSide(const CDiffContext& ctxt, const DIFFITEM& di, int sideIndex) const;
	void CheckMovedOrRenamed(const CDiffContext& ctxt, const DIFFITEM& di, bool& moved, bool& renamed) const;

private:
	void DetectMovedItemsBetweenSides(const std::vector<DIFFITEM*>& unmatchedItems, int side0, int side1, CDiffContext& ctxt, MovedItemGroups& movedItemGroups);

	std::unique_ptr<FilterExpression> m_pMoveDetectionExpression;
	std::shared_ptr<MovedItemGroups> m_pMovedItemGroups;
};
