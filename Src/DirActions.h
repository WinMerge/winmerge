#pragma once

#include "UnicodeString.h"

struct DIFFITEM;
class CDiffContext;
class PathContext;

void GetItemFileNames(const CDiffContext *pctxt, const DIFFITEM& di, String& strLeft, String& strRight);
void GetItemFileNames(const CDiffContext *pctxt, const DIFFITEM& di, PathContext * paths);
bool IsItemCopyableToLeft(const DIFFITEM & di);
bool IsItemCopyableToRight(const DIFFITEM & di);
bool IsItemDeletableOnLeft(const DIFFITEM & di);
bool IsItemDeletableOnRight(const DIFFITEM & di);
bool IsItemDeletableOnBoth(const DIFFITEM & di);
bool IsItemOpenable(const CDiffContext *pctx, const DIFFITEM & di, bool treemode);
bool AreItemsOpenable(const CDiffContext *pctx, const DIFFITEM & di1, const DIFFITEM & di2, const DIFFITEM & di3);
bool IsItemOpenableOnLeft(const DIFFITEM & di);
bool IsItemOpenableOnRight(const DIFFITEM & di);
bool IsItemOpenableOnLeftWith(const DIFFITEM & di);
bool IsItemOpenableOnRightWith(const DIFFITEM & di);
bool IsItemCopyableToOnLeft(const DIFFITEM & di);
bool IsItemCopyableToOnRight(const DIFFITEM & di);
