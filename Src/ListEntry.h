/**
 *  @file ListEntry.h
 *
 *  @brief Declaration of ListEntry
 */
// ID line follows -- this is updated by SVN
// $Id: $
#ifndef _LIST_ENTRY_H_INCLUDED
#define _LIST_ENTRY_H_INCLUDED

/**
 * @brief Simple LIST_ENTRY wrapper
 */
class ListEntry : public LIST_ENTRY
{
public:
	ListEntry()
	{
		Flink = Blink = this;
	}
	void Append(LIST_ENTRY *p)
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
	LIST_ENTRY *IsSibling(LIST_ENTRY *p) const
	{
		return p != static_cast<const LIST_ENTRY *>(this) ? p : 0;
	}
private:
	ListEntry(const ListEntry &); // disallow copy construction
	void operator=(const ListEntry &); // disallow assignment
};

#endif // _LIST_ENTRY_H_INCLUDED
