/**
 *  @file ListEntry.h
 *
 *  @brief Declaration of ListEntry
 */
// ID line follows -- this is updated by SVN
// $Id$
#ifndef _LIST_ENTRY_H_INCLUDED
#define _LIST_ENTRY_H_INCLUDED

/**
 * @brief Simple LIST_ENTRY wrapper
 */
class ListEntry
{
public:
	ListEntry()
	{
		Flink = Blink = this;
	}
	void Append(ListEntry *p)
	{
		p->Flink = Blink->Flink;
		p->Blink = Blink;
		Blink->Flink = p;
		Blink = p;
	}
	void RemoveSelf()
	{
		Blink->Flink = Flink;
		Flink->Blink = Blink;
		Flink = Blink = this;
	}
	ListEntry *IsSibling(ListEntry *p) const
	{
		return p != this ? p : 0;
	}
	ListEntry *Flink;
	ListEntry *Blink;
private:
	ListEntry(const ListEntry &); // disallow copy construction
	void operator=(const ListEntry &); // disallow assignment
};

#endif // _LIST_ENTRY_H_INCLUDED
