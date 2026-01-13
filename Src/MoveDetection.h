#pragma once

#include "UnicodeString.h"
#include <vector>
#include <map>
#include <memory>

class CDiffContext;
class DIFFITEM;
struct FilterExpression;

using MovedItemsArray = std::vector<std::map<int, std::vector<DIFFITEM*>>>;

class MoveDetection
{
public:
	MoveDetection();
	~MoveDetection();

	FilterExpression* GetMoveDetectionExpression() const { return m_pMoveDetectionExpression.get(); }
	void SetMoveDetectionExpression(const FilterExpression* expr);
	void Detect(CDiffContext& ctxt);
	
	std::shared_ptr<const MovedItemsArray> GetMovedItems() const { return std::atomic_load(&m_pMovedItems); }
	std::vector<const DIFFITEM*> GetMovedGroupItemsForSide(const CDiffContext& ctxt, const DIFFITEM* pdi, int sideIndex) const;

private:
	void DetectMovedItemsBetweenSides(const std::vector<DIFFITEM*>& unmatchedItems, int side0, int side1, CDiffContext& ctxt, MovedItemsArray& movedItems);

	std::unique_ptr<FilterExpression> m_pMoveDetectionExpression;
	std::shared_ptr<MovedItemsArray> m_pMovedItems;
};
