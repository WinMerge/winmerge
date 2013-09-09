/*
Frhed - Free hex editor
Copyright (C) 2000 Raihan Kibria

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.

Last change: 2013-02-24 by Jochen Neubeck
*/
/** 
 * @file  gtools.h
 *
 * @brief List declaration.
 *
 */
#ifndef gtools_h
#define gtools_h

class PNode : public LIST_ENTRY
{
public:
	PNode()
	{
		Flink = Blink = this;
	}
	virtual ~PNode()
	{
		Blink->Flink = Flink;
		Flink->Blink = Blink;
	}
private:
	PNode(const PNode &); // disallow copy construction
	void operator=(const PNode &); // disallow assignment
};

class PList : public PNode
{
public:
	PList();
	virtual ~PList();
	void AddTail(PNode *);
	bool IsEmpty()
	{
		return Flink == this;
	}
	void DeleteContents();
	long Count;
};

union PFormat
{
	unsigned capacity; // capacity in TCHARs - must be set prior to Format()
	TCHAR buffer[1];
	int Format(LPCTSTR, ...);
private:
	PFormat(); // disallow construction
};

template<size_t capacity>
class PString
{
	TCHAR buffer[capacity];
public:
	operator LPTSTR() { return buffer; }
	PFormat &operator[](unsigned position)
	{
		// only remaining capacity (minus padding to store it) is ours
		PFormat &r = reinterpret_cast<PFormat &>(buffer[position]);
		r.capacity = capacity - position - sizeof(unsigned) / sizeof(TCHAR) + 1;
		return r;
	}
	operator PFormat *()
	{
		// total capacity is ours
		PFormat *p = reinterpret_cast<PFormat *>(buffer);
		p->capacity = capacity;
		return p;
	}
	PFormat *operator->()
	{
		// total capacity is ours
		PFormat *p = reinterpret_cast<PFormat *>(buffer);
		p->capacity = capacity;
		return p;
	}
};

void TRACE(const char* pszFormat,...);

#endif // gtools_h
