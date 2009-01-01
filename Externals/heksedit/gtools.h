/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  gtools.h
 *
 * @brief List declaration.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: gtools.h 45 2008-09-04 14:05:26Z jtuc $

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
	size_t capacity; // capacity in TCHARs - must be set prior to Format()
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
	PFormat &operator[](size_t position)
	{
		// only remaining capacity (minus padding to store it) is ours
		PFormat &r = reinterpret_cast<PFormat &>(buffer[position]);
		r.capacity = capacity - position - sizeof(size_t) / sizeof(TCHAR) + 1;
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
