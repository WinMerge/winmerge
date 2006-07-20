/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or (at
//    your option) any later version.
//    
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  MovedBlocks.cpp
 *
 * @brief Moved block detection code.
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "diff.h"


class IntSet
{
public:
	void Add(int val) { m_map.SetAt(val, 1); }
	void Remove(int val) { m_map.RemoveKey(val); }
	int count() const { return m_map.GetCount(); }
	bool isPresent(int val) const { int parm; return !!m_map.Lookup(val, parm); }
	int getSingle() const 
	{
		int val, parm;
		POSITION pos = m_map.GetStartPosition();
		m_map.GetNextAssoc(pos, val, parm); 
		return val; 
	}

private:
	CMap<int, int, int, int> m_map;
};

/** 
 * @brief  Set of equivalent lines
 * This uses diffutils line numbers, which are counted from the prefix
 */
struct EqGroup
{
	IntSet m_lines0; // equivalent lines on side#0
	IntSet m_lines1; // equivalent lines on side#1

	bool isPerfectMatch() const { return m_lines0.count()==1 && m_lines1.count()==1; }
};


/** @brief  Maps equivalency code to equivalency group */
class CodeToGroupMap : public CTypedPtrMap<CMapPtrToPtr, void*, EqGroup *>
{
public:
	/** @brief Add a line to the appropriate equivalency group */
	void Add(int lineno, int eqcode, int nside)
	{
		EqGroup * pgroup = 0;
		if (!Lookup((void *)eqcode, pgroup))
		{
			pgroup = new EqGroup;
			SetAt((void *)eqcode, pgroup);
		}
		if (nside)
			pgroup->m_lines1.Add(lineno);
		else
			pgroup->m_lines0.Add(lineno);
	}

	/** @brief Return the appropriate equivalency group */
	EqGroup * find(int eqcode)
	{
		EqGroup * pgroup=0;
		Lookup((void *)eqcode, pgroup);
		return pgroup;
	}

	~CodeToGroupMap()
	{
		for (POSITION pos = GetStartPosition(); pos; )
		{
			void * v=0;
			EqGroup * pgroup=0;
			GetNextAssoc(pos, v, pgroup);
			delete pgroup;
		}
	}
};

/*
 WinMerge moved block code
 This is called by diffutils code, by diff_2_files routine (in ANALYZE.C)
 read_files earlier computed the hash chains ("equivs" file variable) and freed them,
 but the equivs numerics are still available in each line

 match1 set by scan from line0 to deleted
 match0 set by scan from line1 to inserted

*/
extern "C" void moved_block_analysis(struct change ** pscript, struct file_data fd[])
{
	// Hash all altered lines
	CodeToGroupMap map;

	struct change * script = *pscript;
	struct change *p,*e;
	for (e = script; e; e = p)
	{
		p = e->link;
		int i=0;
		for (i=e->line0; i-(e->line0) < (e->deleted); ++i)
			map.Add(i, fd[0].equivs[i], 0);
		for (i=e->line1; i-(e->line1) < (e->inserted); ++i)
			map.Add(i, fd[1].equivs[i], 1);
	}


	// Scan through diff blocks, finding moved sections from left side
	// and splitting them out
	// That is, we actually fragment diff blocks as we find moved sections
	for (e = script; e; e = p)
	{
		// scan down block for a match
		p = e->link;
		EqGroup * pgroup = 0;
		int i=0;
		for (i=e->line0; i-(e->line0) < (e->deleted); ++i)
		{
			EqGroup * tempgroup = map.find(fd[0].equivs[i]);
			if (tempgroup->isPerfectMatch())
			{
				pgroup = tempgroup;
				break;
			}
		}

		// if no match, go to next diff block
		if (!pgroup)
			continue;

		// found a match
		int j = pgroup->m_lines1.getSingle();
		// Ok, now our moved block is the single line i,j

		// extend moved block upward as far as possible
		int i1 = i-1;
		int j1 = j-1;
		for ( ; i1>=e->line0; --i1, --j1)
		{
			EqGroup * pgroup0 = map.find(fd[0].equivs[i1]);
			EqGroup * pgroup1 = map.find(fd[1].equivs[j1]);
			if (pgroup0 != pgroup1)
				break;
			pgroup0->m_lines0.Remove(i1);
			pgroup1->m_lines1.Remove(j1);
		}
		++i1;
		++j1;
		// Ok, now our moved block is i1->i, j1->j

		// extend moved block downward as far as possible
		int i2 = i+1;
		int j2 = j+1;
		for ( ; i2-(e->line0) < (e->deleted); ++i2,++j2)
		{
			EqGroup * pgroup0 = map.find(fd[0].equivs[i2]);
			EqGroup * pgroup1 = map.find(fd[1].equivs[j2]);
			if (pgroup0 != pgroup1)
				break;
			pgroup0->m_lines0.Remove(i2);
			pgroup1->m_lines1.Remove(j2);
		}
		--i2;
		--j2;
		// Ok, now our moved block is i1->i2,j1->j2

		ASSERT(i2-i1 >= 0);
		ASSERT(i2-i1 == j2-j1);

		int prefix = i1 - (e->line0);
		if (prefix)
		{
			// break e (current change) into two pieces
			// first part is the prefix, before the moved part
			// that stays in e
			// second part is the moved part & anything after it
			// that goes in newob
			// leave the right side (e->inserted) on e
			// so no right side on newob
			// newob will be the moved part only, later after we split off any suffix from it
			struct change *newob = (struct change *) xmalloc (sizeof (struct change));
			memset(newob, 0, sizeof(*newob));

			newob->line0 = i1;
			newob->line1 = e->line1 + e->inserted;
			newob->inserted = 0;
			newob->deleted = e->deleted - prefix;
			newob->link = e->link;
			newob->match0 = -1;
			newob->match1 = -1;

			e->deleted = prefix;
			e->link = newob;

			// now make e point to the moved part (& any suffix)
			e = newob;
		}
		// now e points to a moved diff chunk with no prefix, but maybe a suffix

		e->match1 = j1;

		int suffix = (e->deleted) - (i2-(e->line0)) - 1;
		if (suffix)
		{
			// break off any suffix from e
			// newob will be the suffix, and will get all the right side
			struct change *newob = (struct change *) xmalloc (sizeof (struct change));
			memset(newob, 0, sizeof(*newob));

			newob->line0 = i2+1;
			newob->line1 = e->line1;
			newob->inserted = e->inserted;
			newob->deleted = suffix;
			newob->link = e->link;
			newob->match0 = -1;
			newob->match1 = -1;

			e->inserted = 0;
			e->deleted -= suffix;
			e->link = newob;

			p = newob; // next block to scan
		}
	}

	// Scan through diff blocks, finding moved sections from right side
	// and splitting them out
	// That is, we actually fragment diff blocks as we find moved sections
	for (e = script; e; e = p)
	{
		// scan down block for a match
		p = e->link;
		EqGroup * pgroup = 0;
		int j=0;
		for (j=e->line1; j-(e->line1) < (e->inserted); ++j)
		{
			EqGroup * tempgroup = map.find(fd[1].equivs[j]);
			if (tempgroup->isPerfectMatch())
			{
				pgroup = tempgroup;
				break;
			}
		}

		// if no match, go to next diff block
		if (!pgroup)
			continue;

		// found a match
		int i = pgroup->m_lines0.getSingle();
		// Ok, now our moved block is the single line i,j

		// extend moved block upward as far as possible
		int i1 = i-1;
		int j1 = j-1;
		for ( ; j1>=e->line1; --i1, --j1)
		{
			EqGroup * pgroup0 = map.find(fd[0].equivs[i1]);
			EqGroup * pgroup1 = map.find(fd[1].equivs[j1]);
			if (pgroup0 != pgroup1)
				break;
			pgroup0->m_lines0.Remove(i1);
			pgroup1->m_lines1.Remove(j1);
		}
		++i1;
		++j1;
		// Ok, now our moved block is i1->i, j1->j

		// extend moved block downward as far as possible
		int i2 = i+1;
		int j2 = j+1;
		for ( ; j2-(e->line1) < (e->inserted); ++i2,++j2)
		{
			EqGroup * pgroup0 = map.find(fd[0].equivs[i2]);
			EqGroup * pgroup1 = map.find(fd[1].equivs[j2]);
			if (pgroup0 != pgroup1)
				break;
			pgroup0->m_lines0.Remove(i2);
			pgroup1->m_lines1.Remove(j2);
		}
		--i2;
		--j2;
		// Ok, now our moved block is i1->i2,j1->j2

		ASSERT(i2-i1 >= 0);
		ASSERT(i2-i1 == j2-j1);

		int prefix = j1 - (e->line1);
		if (prefix)
		{
			// break e (current change) into two pieces
			// first part is the prefix, before the moved part
			// that stays in e
			// second part is the moved part & anything after it
			// that goes in newob
			// leave the left side (e->deleted) on e
			// so no right side on newob
			// newob will be the moved part only, later after we split off any suffix from it
			struct change *newob = (struct change *) xmalloc (sizeof (struct change));
			memset(newob, 0, sizeof(*newob));

			newob->line0 = e->line0 + e->deleted;
			newob->line1 = j1;
			newob->inserted = e->inserted - prefix;
			newob->deleted = 0;
			newob->link = e->link;
			newob->match0 = -1;
			newob->match1 = -1;

			e->inserted = prefix;
			e->link = newob;

			// now make e point to the moved part (& any suffix)
			e = newob;
		}
		// now e points to a moved diff chunk with no prefix, but maybe a suffix

		e->match0 = i1;

		int suffix = (e->inserted) - (j2-(e->line1)) - 1;
		if (suffix)
		{
			// break off any suffix from e
			// newob will be the suffix, and will get all the left side
			struct change *newob = (struct change *) xmalloc (sizeof (struct change));
			memset(newob, 0, sizeof(*newob));

			newob->line0 = e->line0;
			newob->line1 = j2+1;
			newob->inserted = suffix;
			newob->deleted = e->deleted;
			newob->link = e->link;
			newob->match0 = -1;
			newob->match1 = e->match1;

			e->inserted -= suffix;
			e->deleted = 0;
			e->match1 = -1;
			e->link = newob;

			p = newob; // next block to scan
		}
	}

}
