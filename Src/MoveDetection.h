#pragma once

#include "UnicodeString.h"
#include <vector>
#include <map>

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
	
	const MovedItemsArray& GetMovedItems() const { return m_movedItems; }
	const std::vector<const DIFFITEM*> GetMovedItemsByDIFFITEM(const CDiffContext& ctxt, const DIFFITEM* pdi, int sideIndex) const;
	bool IsDetecting() const { return m_isDetecting.load(); }

private:
	void DetectMovedItemsBetweenSides(const std::vector<DIFFITEM*>& unmatchedItems, int side0, int side1, CDiffContext& ctxt);

	std::atomic<bool> m_isDetecting{ false };
	std::unique_ptr<FilterExpression> m_pMoveDetectionExpression;
	MovedItemsArray m_movedItems;
};
