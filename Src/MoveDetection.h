#pragma once

#include "UnicodeString.h"

class CDiffContext;
class DIFFITEM;
struct FilterExpression;

using MovedItemsArray = std::vector<std::map<int, std::vector<DIFFITEM*>>>;

namespace MoveDetection
{
	MovedItemsArray Detect(CDiffContext& ctxt);
	const DIFFITEM* GetMovedItemByDIFFITEM(const CDiffContext& ctxt, const DIFFITEM* pdi, int sideIndex);
}
