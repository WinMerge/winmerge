/**
 *  @file DiffItem.cpp
 *
 *  @brief Implementation of DIFFITEM
 */ 

#include "pch.h"
#include "DiffItem.h"
#include "paths.h"
#include "DebugNew.h"

DIFFITEM DIFFITEM::emptyitem;

/** @brief DIFFITEM's destructor */
DIFFITEM::~DIFFITEM()
{
	RemoveChildren();
	assert(children == nullptr);
}

/** @brief Return path to left/right file, including all but file name */
String DIFFITEM::getFilepath(int nIndex, const String &sRoot) const
{
	if (diffcode.exists(nIndex))
	{
		return paths::ConcatPath(sRoot, diffFileInfo[nIndex].path);
	}
	return _T("");
}

/** @brief Return the relative path to file/folder including the item name*/
String DIFFITEM::getItemRelativePath() const
{
	String resp = _T("");
	int compareIndex ;

	//determine what is the trees contain the item to be hidden
	for (compareIndex = 0; (compareIndex < 3) && (diffFileInfo[compareIndex].size == -1); compareIndex++);

	if (compareIndex < 3) 
	{
		resp = paths::ConcatPath(diffFileInfo[compareIndex].path, diffFileInfo[compareIndex].filename);
	}

	return resp; 
}
/** @brief Return depth of path */
int DIFFITEM::GetDepth() const
{
	const DIFFITEM *cur;
	int depth;
	for (depth = 0, cur = parent; cur->parent != nullptr; depth++, cur = cur->parent)
		;
	return depth;
}

/**
 * @brief Return whether the specified item is an ancestor of the current item
 */
bool DIFFITEM::IsAncestor(const DIFFITEM *pdi) const
{
	const DIFFITEM *cur;
	for (cur = this; cur->parent != nullptr; cur = cur->parent)
	{
		if (cur->parent == pdi)
			return true;
	}
	return false;
}

/**
 * @brief Return all ancestors of the current item.
 */
std::vector<const DIFFITEM*> DIFFITEM::GetAncestors() const
{
	int depth = GetDepth();
	std::vector<const DIFFITEM*> ancestors(depth);

	const DIFFITEM* cur;
	int i;
	for (i = 0, cur = parent; cur->parent != nullptr; i++, cur = cur->parent)
	{
		assert(depth - i - 1 >= 0 && depth - i - 1 < depth);
		ancestors[depth - i - 1] = cur;
	}
	return ancestors;
}

/** @brief Remove and delete all children DIFFITEM entries */
void DIFFITEM::RemoveChildren()
{
	DIFFITEM *pRem = children;
	while (pRem != nullptr)
	{
		assert(pRem->parent == this);
		DIFFITEM *pNext = pRem->Flink;
		delete pRem;
		pRem = pNext;
	}
	children = nullptr;
}

/** @brief Swap two items in `diffFileInfo[]`.  Used when swapping GUI panes. */
void DIFFITEM::Swap(int idx1, int idx2)
{
	std::swap(diffFileInfo[idx1], diffFileInfo[idx2]);
	diffcode.swap(idx1, idx2);
	if (HasChildren())
	{
		for (DIFFITEM *p = children; p != nullptr; p = p->Flink)
			p->Swap(idx1, idx2);
	}
}

void DIFFITEM::ClearAllAdditionalProperties()
{
	const int n = diffcode.isThreeway() ? 3 : 2;
	for (int i = 0; i < n; ++i)
		diffFileInfo[i].m_pAdditionalProperties.reset();
	if (HasChildren())
	{
		for (DIFFITEM *p = children; p != nullptr; p = p->Flink)
			p->ClearAllAdditionalProperties();
	}

}

/* static */
DIFFITEM *DIFFITEM::GetEmptyItem()  
{ 
	// TODO: It would be better if there were individual items
	// (for whatever these special items are?) because here we 
	// have to *hope* client does not modify this static (shared) item

	assert(emptyitem.parent == nullptr);
	assert(emptyitem.Flink == nullptr);
	assert(emptyitem.Blink == nullptr);
	assert(emptyitem.children == nullptr);
	assert(emptyitem.nidiffs == -1);
	assert(emptyitem.nsdiffs == -1);
	assert(emptyitem.customFlags == ViewCustomFlags::INVALID_CODE);
	assert(emptyitem.diffcode.diffcode == 0);

	return &emptyitem; 
}

 
/**
* @brief Add Sibling item
* @param [in] p The item to be added
*/
void DIFFITEM::AppendSibling(DIFFITEM *p)
{
	assert(parent->children == this);

	// Two situations

	if (Blink == nullptr)
	{
		// Insert first sibling (besides ourself)
		assert(Flink == nullptr);
		p->Flink = nullptr;
		p->Blink = this;
		Flink = p;
	}
	else
	{
		// Insert additional siblings
		assert(Flink != nullptr);
		p->Flink = nullptr;
		p->Blink = Blink;
		Blink->Flink = p;
	}
	Blink = p;
}

void DIFFITEM::AddChildToParent(DIFFITEM *p)
{
	p->parent = this;
	if (children == nullptr)
		// First child
		children = p;
	else
		// More siblings
		children->AppendSibling(p);
}

void DIFFITEM::DelinkFromSiblings()
{
	if (parent != nullptr && parent->children != nullptr)
	{
		// If `this` is at end of Sibling linkage, fix First Child's end link
		if (parent->children->Blink == this)
		{
			assert(Flink == nullptr);
			parent->children->Blink = Blink;
			if (Blink == this)
				Blink = nullptr;
		}
		// If `this` is the First Child, link parent to next Sibling
		if (parent->children == this)
		{
			parent->children = Flink;
		}
	}
	if (Blink != nullptr && Blink->Flink != nullptr)
		Blink->Flink = Flink;
	if (Flink != nullptr)
		Flink->Blink = Blink;
	Flink = Blink = nullptr;
}

void DIFFCODE::swap(int idx1, int idx2)
{
	bool e[3] = { false, false, false };
	for (int i = 0; i < 3; ++i)
		e[i] = exists(i);
	std::swap(e[idx1], e[idx2]);
	setSideNone();
	for (int i = 0; i < 3; ++i)
		if (e[i]) setSideFlag(i);
	bool binflag1 = (diffcode & (BINSIDE1 << idx1));
	bool binflag2 = (diffcode & (BINSIDE1 << idx2));
	Set(BINSIDE1 << idx1, binflag2 ? (BINSIDE1 << idx1) : 0);
	Set(BINSIDE1 << idx2, binflag1 ? (BINSIDE1 << idx2) : 0);
	if (isThreeway())
	{
		int idx = -1;
		switch (diffcode & COMPAREFLAGS3WAY)
		{
		case DIFF1STONLY:
			if (idx1 == 0 || idx2 == 0)
				idx = (idx1 == 0) ? idx2 : idx1;
			break;
		case DIFF2NDONLY:
			if (idx1 == 1 || idx2 == 1)
				idx = (idx1 == 1) ? idx2 : idx1;
			break;
		case DIFF3RDONLY:
			if (idx1 == 2 || idx2 == 2)
				idx = (idx1 == 2) ? idx2 : idx1;
			break;
		}
		if (idx == 0)
			Set(COMPAREFLAGS3WAY, DIFF1STONLY);
		else if (idx == 1)
			Set(COMPAREFLAGS3WAY, DIFF2NDONLY);
		else if (idx == 2)
			Set(COMPAREFLAGS3WAY, DIFF3RDONLY);
	}
}
