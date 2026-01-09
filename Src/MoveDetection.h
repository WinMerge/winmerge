#pragma once

#include "UnicodeString.h"

class CDiffContext;
class DIFFITEM;
struct FilterExpression;

namespace MoveDetection
{
	void Detect(CDiffContext& ctxt);
}
