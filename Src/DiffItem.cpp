/**
 *  @file DiffItem.cpp
 *
 *  @brief Implementation of DIFFITEM
 */ 

#include "stdafx.h"
#include "DiffItem.h"
#include "paths.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DIFFITEM DIFFITEM::emptyitem;

/** @brief DIFFITEM's destructor */
DIFFITEM::~DIFFITEM()
{
	RemoveChildren();
	RemoveSiblings();
	assert(children == nullptr);
	assert(Flink == nullptr);
	assert(Blink == nullptr);
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

/** @brief Remove and delete all sibling DIFFITEM entries, via Flink */
void DIFFITEM::RemoveSiblings()
{
	DIFFITEM *pRem = Flink;
	while (pRem != nullptr)
	{
		assert(pRem->parent == parent);
		assert(pRem->Blink == this);
		DIFFITEM *pNext = pRem->Flink;
		pRem->DelinkFromSiblings();	// destroys Flink (so we use pRem instead)
		delete pRem;
		pRem = pNext;
	}
	DelinkFromSiblings();
}

/** @brief Remove and delete all children DIFFITEM entries */
void DIFFITEM::RemoveChildren()
{
	delete children;
	children = nullptr;
}

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
	if (Blink != nullptr)
		Blink->Flink = Flink;
	if (Flink != nullptr)
		Flink->Blink = Blink;
	Flink = Blink = nullptr;
}

