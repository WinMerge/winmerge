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
// $Id: gtools.h 21 2008-08-18 17:16:39Z kimmov $

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

template<size_t n>
class PString
{
	TCHAR buffer[n];
public:
	operator LPTSTR() { return buffer; }
};

void TRACE(const char* pszFormat,...);

#endif // gtools_h
